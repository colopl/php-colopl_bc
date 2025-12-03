ARG PLATFORM=${BUILDPLATFORM:-linux/amd64}
ARG IMAGE=php
ARG TAG=8.5-cli-trixie
ARG SKIP_VALGRIND=0

FROM --platform=${PLATFORM} ${IMAGE}:${TAG}

ENV USE_ZEND_ALLOC=0
ENV USE_TRACKED_ALLOC=1
ENV ZEND_DONT_UNLOAD_MODULES=1
ENV LC_ALL="C"

RUN docker-php-source extract \
 && if test -f "/etc/debian_version"; then \
      apt-get update \
 &&   DEBIAN_FRONTEND="noninteractive" apt-get install -y "bison" "re2c" "zlib1g-dev" "libsqlite3-dev" "libxml2-dev" \
        "autoconf" "pkg-config" "make" "gcc" "rsync" "git" "ssh" "libc6-dbg" \
        "ca-certificates" "tzdata" "lsb-release" "curl" "gnupg" \
        "lcov" "gzip" \
        "vim" \
        "unzip" && \
        mkdir -p "/usr/share/keyrings" && \
        curl -sSL "https://apt.llvm.org/llvm-snapshot.gpg.key" | gpg --dearmor > "/usr/share/keyrings/llvm-archive-keyring.gpg" && \
        echo "deb [signed-by=/usr/share/keyrings/llvm-archive-keyring.gpg] http://apt.llvm.org/$(lsb_release -cs)/ llvm-toolchain-$(lsb_release -cs) main" > "/etc/apt/sources.list.d/llvm.list" && \
        echo "deb [signed-by=/usr/share/keyrings/llvm-archive-keyring.gpg] http://apt.llvm.org/$(lsb_release -cs)/ llvm-toolchain-$(lsb_release -cs)-21 main" >> "/etc/apt/sources.list.d/llvm.list" && \
        apt-get update && \
        DEBIAN_FRONTEND="noninteractive" apt-get install --no-install-recommends -y \
        "clang-21" "clang-tools-21" "clang-format-21" "clang-tidy-21" \
        "libclang-rt-21-dev" "lld-21" "lldb-21" \
        "libc++-21-dev" "libc++abi-21-dev" \
        "llvm-21" "llvm-21-dev" "llvm-21-runtime" \
        "clang-format-21" && \
        update-alternatives --install "/usr/bin/clang" clang "/usr/bin/clang-21" 100 && \
        update-alternatives --install "/usr/bin/clang++" clang++ "/usr/bin/clang++-21" 100 && \
        update-alternatives --install "/usr/bin/clang-format" clang-format "/usr/bin/clang-format-21" 100 && \
        update-alternatives --install "/usr/bin/clang-tidy" clang-tidy "/usr/bin/clang-tidy-21" 100 && \
        update-alternatives --install "/usr/bin/lldb" lldb "/usr/bin/lldb-21" 100 && \
        update-alternatives --install "/usr/bin/ld.lld" ld.lld "/usr/bin/ld.lld-21" 100; \
        update-alternatives --install "/usr/bin/llvm-symbolizer" llvm-symbolizer "/usr/bin/llvm-symbolizer-21" 100 && \
        update-alternatives --install "/usr/bin/llvm-config" llvm-config "/usr/bin/llvm-config-21" 100; \
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
RUN if test "${SKIP_VALGRIND}" != "1"; then \
      cd "/third_party/valgrind" && \
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

COPY ./pskel.sh "/usr/local/bin/pskel"
COPY ./patches "/patches"
COPY ./ext "/ext"

RUN chmod +x "/usr/local/bin/pskel"

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
