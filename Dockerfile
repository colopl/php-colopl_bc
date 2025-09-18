ARG PLATFORM=${BUILDPLATFORM:-linux/amd64}
ARG IMAGE=php
ARG TAG=8.4-cli-trixie

FROM --platform=${PLATFORM} ${IMAGE}:${TAG}

ENV USE_ZEND_ALLOC=0
ENV USE_TRACKED_ALLOC=1
ENV ZEND_DONT_UNLOAD_MODULES=1
ENV LC_ALL="C"

RUN docker-php-source extract \
 && if test -f "/etc/debian_version"; then \
      apt-get update \
 &&   DEBIAN_FRONTEND="noninteractive" apt-get install -y "bison" "re2c" "zlib1g-dev" "libsqlite3-dev" "libxml2-dev" \
        "autoconf" "pkg-config" "make" "gcc" "valgrind" "rsync" "git" "ssh" \
        "clang" \
        "lcov" "gzip" \
        "vim"; \
    else \
      apk add --no-cache "bison" "zlib-dev" "sqlite-dev" "libxml2-dev" "linux-headers" \
        "autoconf" "pkgconfig" "make" "gcc" "g++" "valgrind" "valgrind-dev" \
        "musl-dev" "rsync" "git" "openssh" \
        "patch" "lcov" "gzip" \
        "vim"; \
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
