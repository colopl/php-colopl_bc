#!/bin/sh

if php -r 'exit((\PHP_VERSION_ID >= 80000) ? 0 : 1);'; then
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
if php -r 'exit((\PHP_VERSION_ID >= 80000) ? 0 : 1);'; then
  composer exec -- phpstan analyse --memory-limit=-1 --configuration="phpstan.neon"
  composer exec -- psalm --memory-limit=-1 --config="psalm.xml"
fi
