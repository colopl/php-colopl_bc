ARG IMAGE=php
ARG TAG=8.1-cli

FROM ${IMAGE}:${TAG}

ARG PSKEL_SKIP_DEBUG=

ENV USE_ZEND_ALLOC=0
ENV ZEND_DONT_UNLOAD_MODULES=1
ENV PSKEL_SKIP_DEBUG=${PSKEL_SKIP_DEBUG}

RUN if test -f "/etc/debian_version"; then \
      apt-get update && \
      DEBIAN_FRONTEND="noninteractive" apt-get install -y \
        "build-essential" "bison" "valgrind" "llvm" "clang" && \
      if test "${PSKEL_SKIP_DEBUG}" = ""; then \
        docker-php-source extract && \
        cd "/usr/src/php" && \
          CFLAGS="-fpic -fpie -DZEND_TRACK_ARENA_ALLOC" LDFLAGS="-pie" ./configure --disable-all \
              --includedir="/usr/local/include/gcc-debug-php" --program-prefix="gcc-debug-" \
              --disable-cgi --disable-fpm --disable-phpdbg --enable-cli \
              --enable-debug --without-pcre-jit "$(php -r "echo PHP_ZTS === 1 ? '--enable-zts' : '';")" \
              --with-valgrind \
              --enable-option-checking=fatal && \
          make -j$(nproc) && \
          make install && \
        cd - && \
        docker-php-source delete && \
        docker-php-source extract && \
        cd "/usr/src/php" && \
          CC=clang CXX=clang++ CFLAGS="-fpic -fpie -DZEND_TRACK_ARENA_ALLOC" LDFLAGS="-pie" ./configure \
              --includedir="/usr/local/include/clang-debug-php" --program-prefix="clang-debug-" \
              --disable-cgi --disable-all --disable-fpm --disable-phpdbg --enable-cli \
              --enable-debug --without-pcre-jit "$(php -r "echo PHP_ZTS === 1 ? '--enable-zts' : '';")" \
              --enable-memory-sanitizer \
              --enable-option-checking=fatal && \
          make -j$(nproc) && \
          make install && \
        cd - && \
        docker-php-source delete; \
      fi; \
    elif test -f "/etc/alpine-release"; then \
        apk add --no-cache ${PHPIZE_DEPS} "bison" "valgrind" "valgrind-dev" && \
        if test "${PSKEL_SKIP_DEBUG}" = ""; then \
          docker-php-source extract && \
          cd "/usr/src/php" && \
              CFLAGS="-fpic -fpie -DZEND_TRACK_ARENA_ALLOC" LDFLAGS="-pie" ./configure --disable-all \
                  --includedir="/usr/local/include/gcc-debug-php" --program-prefix="gcc-debug-" \
                  --disable-cgi --disable-fpm --disable-phpdbg --enable-cli \
                  --enable-debug --without-pcre-jit "$(php -r "echo PHP_ZTS === 1 ? '--enable-zts' : '';")" \
                  --with-valgrind \
                  --enable-option-checking=fatal && \
              make -j$(nproc) && \
              make install && \
          cd - && \
          docker-php-source delete; \
        fi; \
    fi && \
    docker-php-source extract

WORKDIR "/usr/src/php"

COPY ./ci.sh /usr/bin/ci

ENTRYPOINT ["/usr/bin/ci"]

COPY ./ext /ext
