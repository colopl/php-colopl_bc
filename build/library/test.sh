#!/bin/sh

if php -r 'exit((\PHP_VERSION_ID >= 80000 && \PHP_VERSION_ID < 80100) ? 0 : 1);'; then
  echo 'colopl_bc does not support PHP 8.0' >&2
  exit 1
fi

if php -r 'exit((\PHP_VERSION_ID >= 80100) ? 0 : 1);'; then
  cd "ext"
    phpize
    ./configure --with-php-config="$(which php-config)"
    make -j$(nproc)
    make test
    make install
    docker-php-ext-enable "colopl_bc"
    php --re "colopl_bc"
  cd -
fi

composer install
composer exec -- phpunit --config="phpunit.xml"
if php -r 'exit((\PHP_VERSION_ID >= 80100) ? 0 : 1);'; then
  composer exec -- phpstan analyse --memory-limit=-1 --configuration="phpstan.neon"
  composer exec -- psalm --memory-limit=-1 --config="psalm.xml"
fi
