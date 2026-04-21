ARG PLATFORM=${BUILDPLATFORM:-linux/amd64}
ARG IMAGE=php
ARG TAG=8.5-cli-trixie
ARG SKIP_VALGRIND=0
ARG ENABLE_CLANG=1

FROM --platform=${PLATFORM} ${IMAGE}:${TAG}

ARG ENABLE_CLANG=1

ENV USE_ZEND_ALLOC=0
ENV USE_TRACKED_ALLOC=1
ENV ZEND_DONT_UNLOAD_MODULES=1
ENV LC_ALL="C"

RUN docker-php-source extract \
 && if test -f "/etc/debian_version"; then \
      apt-get update  && \
      DEBIAN_FRONTEND="noninteractive" apt-get install -y "bison" "re2c" "zlib1g-dev" "libsqlite3-dev" "libxml2-dev" \
        "autoconf" "pkg-config" "make" "gcc" "rsync" "git" "ssh" "libc6-dbg" \
        "ca-certificates" "tzdata" "curl" "gnupg" \
        "lcov" "gzip" \
        "vim" \
        "unzip" && \
        if test "${ENABLE_CLANG}" = "1"; then \
          LLVM_APT_CODENAME="$(. "/etc/os-release" && printf '%s' "${VERSION_CODENAME}")" && \
          test -n "${LLVM_APT_CODENAME}" && \
          mkdir -p "/usr/share/keyrings" && \
          curl -fsSL "https://apt.llvm.org/llvm-snapshot.gpg.key" | gpg --dearmor --yes -o "/usr/share/keyrings/llvm-snapshot.gpg" && \
          echo "deb [signed-by=/usr/share/keyrings/llvm-snapshot.gpg] https://apt.llvm.org/${LLVM_APT_CODENAME}/ llvm-toolchain-${LLVM_APT_CODENAME}-22 main" > "/etc/apt/sources.list.d/llvm.list" && \
          apt-get update && \
          DEBIAN_FRONTEND="noninteractive" apt-get install --no-install-recommends -y \
          "clang-22" "clang-tools-22" "clang-format-22" "clang-tidy-22" \
          "libclang-rt-22-dev" "lld-22" "lldb-22" \
          "libc++-22-dev" "libc++abi-22-dev" \
          "llvm-22" "llvm-22-dev" "llvm-22-runtime" && \
          update-alternatives --install "/usr/bin/clang" clang "/usr/bin/clang-22" 100 && \
          update-alternatives --install "/usr/bin/clang++" clang++ "/usr/bin/clang++-22" 100 && \
          update-alternatives --install "/usr/bin/clang-format" clang-format "/usr/bin/clang-format-22" 100 && \
          update-alternatives --install "/usr/bin/clang-tidy" clang-tidy "/usr/bin/clang-tidy-22" 100 && \
          update-alternatives --install "/usr/bin/lldb" lldb "/usr/bin/lldb-22" 100 && \
          update-alternatives --install "/usr/bin/ld.lld" ld.lld "/usr/bin/ld.lld-22" 100 && \
          update-alternatives --install "/usr/bin/llvm-symbolizer" llvm-symbolizer "/usr/bin/llvm-symbolizer-22" 100 && \
          update-alternatives --install "/usr/bin/llvm-config" llvm-config "/usr/bin/llvm-config-22" 100; \
        fi; \
    else \
      apk add --no-cache "bison" "zlib-dev" "sqlite-dev" "libxml2-dev" "linux-headers" \
        "autoconf" "pkgconfig" "make" "gcc" "g++" "musl-dbg" \
        "musl-dev" "rsync" "git" "openssh" \
        "patch" "lcov" "gzip" \
        "vim" \
        "unzip"; \
    fi

COPY ./third_party/valgrind "/third_party/valgrind"
ARG SKIP_VALGRIND
RUN if test "${SKIP_VALGRIND}" != "1" && test -f "/etc/debian_version"; then \
      cd "/third_party/valgrind" && \
        test -x "./autogen.sh" || { \
          echo "Valgrind submodule is missing. Run 'git submodule update --init --recursive' before building the container." >&2; \
          exit 1; \
        } && \
        if test -f "/etc/debian_version"; then \
          apt-get update && \
          DEBIAN_FRONTEND="noninteractive" apt-get install -y \
            "build-essential" "autotools-dev" "automake" "autoconf" "libtool" \
            "libc6-dev" "linux-libc-dev" \
            "libxml2-dev"; \
        else \
          apk add --no-cache "build-base" "automake" "autoconf" "perl" "linux-headers"; \
        fi && \
        ./autogen.sh && \
        ./configure && \
        make -j"$(nproc)" && \
        make install && \
      cd -; \
    fi

COPY ./.pskel "/opt/pskel/.pskel"
COPY ./pskel.sh "/opt/pskel/pskel.sh"
COPY ./patches "/patches"
COPY ./ext "/ext"

RUN chmod +x "/opt/pskel/pskel.sh" \
 && ln -sf "/opt/pskel/pskel.sh" "/usr/local/bin/pskel"

RUN cat <<'EOF' > "/usr/local/bin/docker-entrypoint.sh"
#!/bin/sh
set -e

if test -n "${GITHUB_ACTIONS}" && test -d "${PHP_CACHE_DIR}"; then
  echo "[Pskel > Cache] GitHub Actions environment detected, checking for cached binaries..." >&2
  for CACHE_ENTRY in "${PHP_CACHE_DIR}"/*; do
    if test -f "${CACHE_ENTRY}/.build_complete"; then
      for BIN in "${CACHE_ENTRY}/usr/local/bin/"*; do
        if test -f "${BIN}"; then
          BIN_NAME="$(basename "${BIN}")"
          ln -sf "${BIN}" "/usr/local/bin/${BIN_NAME}"
          echo "[Pskel > Cache] Restored: ${BIN_NAME}" >&2
        fi
      done
      if test -d "${CACHE_ENTRY}/usr/local/include"; then
        cp -an "${CACHE_ENTRY}/usr/local/include/"* "/usr/local/include/" 2>/dev/null || true
      fi
    fi
  done
fi

exec "$@"
EOF

RUN chmod +x "/usr/local/bin/docker-entrypoint.sh"

ENTRYPOINT ["/usr/local/bin/docker-entrypoint.sh"]
CMD ["bash"]
