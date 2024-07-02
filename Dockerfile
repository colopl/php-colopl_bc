ARG IMAGE=php
ARG TAG=8.3-cli

FROM ${IMAGE}:${TAG}

ARG PSKEL_SKIP_DEBUG=""
ARG PSKEL_EXTRA_CONFIGURE_OPTIONS=""

ENV USE_ZEND_ALLOC=0
ENV ZEND_DONT_UNLOAD_MODULES=1
ENV PSKEL_SKIP_DEBUG=${PSKEL_SKIP_DEBUG}
ENV PSKEL_EXTRA_CONFIGURE_OPTIONS=${PSKEL_EXTRA_CONFIGURE_OPTIONS}

RUN if test -f "/etc/debian_version"; then \
      apt-get update && \
      DEBIAN_FRONTEND="noninteractive" apt-get install -y \
        "build-essential" "bison" "valgrind" "llvm" "clang" "zlib1g-dev" "libsqlite3-dev" && \
      if test "${PSKEL_SKIP_DEBUG}" = ""; then \
        docker-php-source extract && \
        cd "/usr/src/php" && \
          CFLAGS="-fpic -fpie -DZEND_TRACK_ARENA_ALLOC" LDFLAGS="-pie" ./configure --disable-all \
              --includedir="/usr/local/include/gcc-valgrind-php" --program-prefix="gcc-valgrind-" \
              --disable-cgi --disable-fpm --enable-cli \
              --enable-mysqlnd --enable-pdo --with-pdo-mysql --with-pdo-sqlite \
              --enable-debug --without-pcre-jit "$(php -r "echo PHP_ZTS === 1 ? '--enable-zts' : '';")" \
              --with-valgrind \
              ${PSKEL_EXTRA_CONFIGURE_OPTIONS} \
              --enable-option-checking=fatal && \
          make -j$(nproc) && \
          make install && \
        cd - && \
        docker-php-source delete && \
        docker-php-source extract && \
        cd "/usr/src/php" && \
          CC=clang CXX=clang++ CFLAGS="-fsanitize=memory -fno-sanitize-recover -DZEND_TRACK_ARENA_ALLOC" LDFLAGS="-fsanitize=memory" ./configure \
              --includedir="/usr/local/include/clang-msan-php" --program-prefix="clang-msan-" \
              --disable-cgi --disable-all --disable-fpm --enable-cli \
              --enable-mysqlnd --enable-pdo --with-pdo-mysql --with-pdo-sqlite \
              --enable-debug --without-pcre-jit "$(php -r "echo PHP_ZTS === 1 ? '--enable-zts' : '';")" \
              --enable-memory-sanitizer \
              ${PSKEL_EXTRA_CONFIGURE_OPTIONS} \
              --enable-option-checking=fatal && \
          make -j$(nproc) && \
          make install && \
        cd - && \
        docker-php-source delete && \
        docker-php-source extract && \
        cd "/usr/src/php" && \
          CC=clang CXX=clang++ CFLAGS="-fsanitize=address -fno-sanitize-recover -DZEND_TRACK_ARENA_ALLOC" LDFLAGS="-fsanitize=address" ./configure \
              --includedir="/usr/local/include/clang-asan-php" --program-prefix="clang-asan-" \
              --disable-cgi --disable-all --disable-fpm --enable-cli \
              --enable-mysqlnd --enable-pdo --with-pdo-mysql --with-pdo-sqlite \
              --enable-debug --without-pcre-jit "$(php -r "echo PHP_ZTS === 1 ? '--enable-zts' : '';")" \
              --enable-address-sanitizer \
              ${PSKEL_EXTRA_CONFIGURE_OPTIONS} \
              --enable-option-checking=fatal && \
          make -j$(nproc) && \
          make install && \
        cd - && \
        docker-php-source delete && \
        docker-php-source extract && \
        cd "/usr/src/php" && \
          CC=clang CXX=clang++ CFLAGS="-fsanitize=undefined -fno-sanitize-recover -DZEND_TRACK_ARENA_ALLOC" LDFLAGS="-fsanitize=undefined" ./configure \
              --includedir="/usr/local/include/clang-ubsan-php" --program-prefix="clang-ubsan-" \
              --disable-cgi --disable-all --disable-fpm --enable-cli \
              --enable-mysqlnd --enable-pdo --with-pdo-mysql --with-pdo-sqlite \
              --enable-debug --without-pcre-jit "$(php -r "echo PHP_ZTS === 1 ? '--enable-zts' : '';")" \
              --enable-undefined-sanitizer \
              ${PSKEL_EXTRA_CONFIGURE_OPTIONS} \
              --enable-option-checking=fatal && \
          make -j$(nproc) && \
          make install && \
        cd - && \
        docker-php-source delete && \
        docker-php-source extract && \
        cd "/usr/src/php" && \
          CFLAGS="-fpic -fpie -DZEND_TRACK_ARENA_ALLOC" LDFLAGS="-pie" ./configure --disable-all \
              --includedir="/usr/local/include/debug-php" --program-prefix="debug-" \
              --disable-cgi --disable-fpm --enable-cli \
              --enable-mysqlnd --enable-pdo --with-pdo-mysql --with-pdo-sqlite \
              --enable-debug "$(php -r "echo PHP_ZTS === 1 ? '--enable-zts' : '';")" \
              ${PSKEL_EXTRA_CONFIGURE_OPTIONS} \
              --enable-option-checking=fatal && \
          make -j$(nproc) && \
          make install && \
        cd - && \
        docker-php-source delete; \
      fi; \
    elif test -f "/etc/alpine-release"; then \
        apk add --no-cache ${PHPIZE_DEPS} "bison" "valgrind" "valgrind-dev" "zlib-dev" "sqlite-dev" && \
        if test "${PSKEL_SKIP_DEBUG}" = ""; then \
          docker-php-source extract && \
          cd "/usr/src/php" && \
              CFLAGS="-fpic -fpie -DZEND_TRACK_ARENA_ALLOC" LDFLAGS="-pie" ./configure --disable-all \
                  --includedir="/usr/local/include/gcc-valgrind-php" --program-prefix="gcc-valgrind-" \
                  --disable-cgi --disable-fpm --enable-cli \
                  --enable-mysqlnd --enable-pdo --with-pdo-mysql --with-pdo-sqlite \
                  --enable-debug --without-pcre-jit "$(php -r "echo PHP_ZTS === 1 ? '--enable-zts' : '';")" \
                  --with-valgrind \
                  ${PSKEL_EXTRA_CONFIGURE_OPTIONS} \
                  --enable-option-checking=fatal && \
              make -j$(nproc) && \
              make install && \
          cd - && \
          docker-php-source delete && \
          docker-php-source extract && \
          cd "/usr/src/php" && \
              CFLAGS="-fpic -fpie -DZEND_TRACK_ARENA_ALLOC" LDFLAGS="-pie" ./configure --disable-all \
                  --includedir="/usr/local/include/debug-php" --program-prefix="debug-" \
                  --disable-cgi --disable-fpm --enable-cli \
                  --enable-mysqlnd --enable-pdo --with-pdo-mysql --with-pdo-sqlite \
                  --enable-debug "$(php -r "echo PHP_ZTS === 1 ? '--enable-zts' : '';")" \
                  ${PSKEL_EXTRA_CONFIGURE_OPTIONS} \
                  --enable-option-checking=fatal && \
              make -j$(nproc) && \
              make install && \
          cd - && \
          docker-php-source delete; \
        fi; \
    fi && \
    docker-php-source extract

WORKDIR "/usr/src/php"

COPY ./ext /ext

COPY ./ci.sh /usr/bin/ci
