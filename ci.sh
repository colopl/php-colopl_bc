#!/bin/sh -ex

if test "${TEST_EXTENSION}" != ""; then
  cd "/ext"
  phpize
  ./configure --with-php-config=$(which php-config)
  make clean
  make -j$(nproc)
  TEST_PHP_ARGS="--show-diff -q" make test
fi

if test "${TEST_EXTENSION_VALGRIND}" != ""; then
  if type "gcc-debug-php" > /dev/null 2>&1; then
    cd "/ext"
    gcc-debug-phpize
    ./configure --with-php-config=$(which gcc-debug-php-config)
    make clean
    make -j$(nproc)
    TEST_PHP_ARGS="--show-diff -q -m" make test
  else
    echo "missing gcc-debug-php"
    exit 1
  fi
fi

if test "${TEST_EXTENSION_MSAN}" != ""; then
  if type "clang-debug-php" > /dev/null 2>&1; then
    cd "/ext"
    clang-debug-phpize
    CC=clang CXX=clang++ CFLAGS="-fsanitize=memory -DZEND_TRACK_ARENA_ALLOC" LDFLAGS="-fsanitize=memory" ./configure --with-php-config=$(which clang-debug-php-config)
    make clean
    make -j$(nproc)
    TEST_PHP_ARGS="--show-diff -q --msan" make test
  else
    echo "missing clang-debug-php"
    exit 1
  fi
fi
