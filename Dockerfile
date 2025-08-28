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
        "vim" \
 &&   update-alternatives --install "/usr/bin/clang" clang "/usr/bin/clang-19" 100 \
 &&   update-alternatives --install "/usr/bin/clang++" clang++ "/usr/bin/clang++-19" 100; \
    else \
      apk add --no-cache "bison" "zlib-dev" "sqlite-dev" "libxml2-dev" "linux-headers" \
        "autoconf" "pkgconfig" "make" "gcc" "g++" "valgrind" "valgrind-dev" \
        "musl-dev" "rsync" "git" "openssh" \
        "patch" "lcov" "gzip" \
        "vim"; \
    fi

COPY ./pskel.sh /usr/local/bin/pskel
COPY ./patches /patches
COPY ./ext /ext

# ----

COPY --from=composer:2 /usr/bin/composer /usr/bin/composer

COPY ./ /project
COPY ./library_test.sh /usr/local/bin/library_test

ENV COMPOSER_ROOT_VERSION="9.9.9"

RUN if test -f "/etc/debian_version"; then \
      apt-get update && apt-get install -y "unzip"; \
    else \
      apk add --no-cache "unzip"; \
    fi
