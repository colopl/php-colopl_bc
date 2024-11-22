#!/bin/sh -eu

export USE_ZEND_ALLOC=1
export USE_TRACKED_ALLOC=0
export ZEND_DONT_UNLOAD_MODULES=1
cd "/project"
  if php -r 'exit((\PHP_VERSION_ID >= 80000) ? 0 : 1);'; then
    cd "ext"
      phpize
      ./configure
      make clean
      make -j"$(nproc)"
      TEST_PHP_ARGS="--show-diff -q" make test
      make install
      docker-php-ext-enable "colopl_bc"
    cd -
  fi
  composer install
  composer exec -- phpunit "tests"
  composer exec -- phpstan
cd -
