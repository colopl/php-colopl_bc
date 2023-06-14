# colopl_bc

Provides various compatibility functions required for PHP (temporary) migration.

> **WARNING**
> This extension is intended for temporary use only. In other words, code in the production environment should not rely on this implementation as much as possible, and should be gradually phased out when it is safe to do so.

## Supported Versions

- Extension: PHP >= 8.0, 8.2 >= PHP
- Library: PHP >= 7.4, 8.2 >= PHP

Library is introduced for migration to PHP 8.x with the same code base. If you do not need to work with the same code base, you can use only the Extension.

## Usage

### Install

This repository contains an Extension that provides the main compatibility and an assisting Library, each of which must be installed separately.

Extension can be installed in the following steps. Note that the PHP common build tool chain is required.

```bash
$ cd "ext"
$ phpize
$ ./configure
$ make -j$(nproc)
$ make test
$ sudo make install
```

Don't forget to enable extension. (Maybe create a `*.ini` file or use `docker-php-ext-enable`)

```bash
$ echo "extension=colopl_bc" | sudo tee "$(php-config --ini-dir)/10-colopl_bc.ini"
$ php -m | grep "colopl_bc"
colopl_bc
```

The library is installed in the following steps.

```bash
$ composer require "colopl/colopl_bc"
```

**Be sure to keep the Extension and Library the same version.**

### Code conversion with Rector

The `colopl/colopl_bc` package contains Rector rules to convert your code to the functions provided by the existing `ext-colopl_bc`. This can be utilized to save time when dealing with large code bases.

For each rule, check the rule definition file `src/Rector`. The most common configuration for migrating from PHP 7.x to 8.x is as follows:

```rector.php
<?php
/* ~~~ */
    $rectorConfig->rules([
        SwitchCaseToEqualRector::class,
        ArrayFuncToBCMigrateRector::class,
        SortToBCMigrateRector::class,
        EqualToBCMigrateRector::class,
        GreaterToBCMigrateRector::class,
        GreaterOrEqualToBCMigrateRector::class,
        NotEqualToBCMigrateRector::class,
        SmallerToBCMigrateRector::class,
        SmallerOrEqualToBCMigrateRector::class,
        SpaceshipToBCMigrateRector::class
    ]);
/* ~~~ */
```

## API references

### INI directives

#### `colopl_bc.php74.compare_mode` (default: `0`)

Sets the compatibility check mode for various comparison operators (e.g. `==`).

- 0 (`COLOPL_BC_PHP74_COMPARE_MODE_SILENT`)

It does not perform compatibility checks and always uses the results of older behavior. Suitable for use in production environments.

- 1 (`COLOPL_BC_PHP74_COMPARE_MODE_DEPRECATED`)

Compares the result with the result of a native PHP operator and raises an error equivalent to `E_DEPRECATED` if the results are different.

- 2 (`COLOPL_BC_PHP74_COMPARE_MODE_LOG`)

Compares the results of native PHP operators and sends a log to the running SAPI if the results are different.

#### `colopl_bc.php74.sort_mode` (default: `0`)

Sets the compatibility check mode for various unstable sort algorithms.

- 0 (`COLOPL_BC_PHP74_COMPARE_MODE_SILENT`)

It does not perform compatibility checks and always uses the results of older behavior. Suitable for use in production environments.

- 1 (`COLOPL_BC_PHP74_COMPARE_MODE_DEPRECATED`)

Compares the result with the result of a native PHP function and raises an error equivalent to `E_DEPRECATED` if the results are different.

- 2 (`COLOPL_BC_PHP74_COMPARE_MODE_LOG`)

Compares the results of native PHP functions and sends a log to the running SAPI if the results are different.

### Constants 

Please check the settings of the corresponding ini directives.

- `COLOPL_BC_PHP74_COMPARE_MODE_SILENT` = 0
- `COLOPL_BC_PHP74_COMPARE_MODE_DEPRECATED` = 1
- `COLOPL_BC_PHP75_COMPARE_MODE_LOG` = 2
- `COLOPL_BC_PHP74_SORT_MODE_SILENT` = 0
- `COLOPL_BC_PHP74_SORT_MODE_DEPRECATED` = 1
- `COLOPL_BC_PHP75_SORT_MODE_LOG` = 2

### Functions

- `Colopl\ColoplBc\Php74\ksort()`
- `Colopl\ColoplBc\Php74\krsort()`
- `Colopl\ColoplBc\Php74\asort()`
- `Colopl\ColoplBc\Php74\arsort()`
- `Colopl\ColoplBc\Php74\sort()`
- `Colopl\ColoplBc\Php74\rsort()`
- `Colopl\ColoplBc\Php74\usort()`
- `Colopl\ColoplBc\Php74\uasort()`
- `Colopl\ColoplBc\Php74\uksort()`
- `Colopl\ColoplBc\Php74\array_multisort()`

Sorting is performed using the previous unstable sorting algorithm.

These compatibilities can also be checked at runtime. See the `colopl_bc.php74.sort_mode` INI directive for details.

- `Colopl\ColoplBc\Php74\in_array()`
- `Colopl\ColoplBc\Php74\array_search()`
- `Colopl\ColoplBc\Php74\array_keys()`

These functions use older comparison algorithms than PHP 8.0 for their internal use.

These compatibilities can also be checked at runtime. See the `colopl_bc.php74.compare_mode` INI directive for details.

- `Colopl\ColoplBc\Php74\eq()` (`==`)
- `Colopl\ColoplBc\Php74\neq()` (`!=`)
- `Colopl\ColoplBc\Php74\lt()` (`<`)
- `Colopl\ColoplBc\Php74\lte()` (`<=`)
- `Colopl\ColoplBc\Php74\gt()` (`>`)
- `Colopl\ColoplBc\Php74\gte()` (`>=`)
- `Colopl\ColoplBc\Php74\spaceship()` (`<=>`)

For the corresponding comparison operator, returns results that reproduce the older behavior.

These compatibilities can also be checked at runtime. See the `colopl_bc.php74.compare_mode` INI directive for details.

- `Colopl\ColoplBc\Php70\srand()`

Seed random numbers.

> **WARNING**
> `srand()` provided by colopl_bc are based on the glibc `srand()` / `rand()` algorithm. The results may differ from those of the non glibc environments, e.g. Alpine Linux.

- `Colopl\ColoplBc\Php70\rand()`

Generate random numbers.

> **WARNING**
> `rand()` provided by colopl_bc are based on the glibc `srand()` / `rand()` algorithm. The results may differ from those of the non glibc environments, e.g. Alpine Linux.

- `Colopl\ColoplBc\Php70\getrandmax()`

Returns the maximum value that can be returned by a call to `Colopl\ColoplBc\Php70\rand()`.

> **WARNING**
> `rand()` provided by colopl_bc are based on the glibc `srand()` / `rand()` algorithm. The results may differ from those of the non glibc environments, e.g. Alpine Linux.


- `Colopl\ColoplBc\Php70\shuffle()`

This function shuffles (randomizes the order of the elements in) an array.

> **WARNING**
> `shuffle()` use `Colopl\ColoplBc\Php70\rand()` algorithm internally, by colopl_bc are based on the glibc `srand()` / `rand()` algorithm. The results may differ from those of the non glibc environments, e.g. Alpine Linux.

- `Colopl\ColoplBc\Php70\str_shuffle()`

str_shuffle() shuffles a string. One permutation of all possible is created.


> **WARNING**
> `str_shuffle()` use `Colopl\ColoplBc\Php70\rand()` algorighm internally, by colopl_bc are based on the glibc `srand()` / `rand()` algorithm. The results may differ from those of the non glibc environments, e.g. Alpine Linux.

- `Colopl\ColoplBc\Php70\array_rand()`

Picks one or more random entries out of an array, and returns the key (or keys) of the random entries.

> **WARNING**
> `array_rand()` use `Colopl\ColoplBc\Php70\rand()` algorithm internally, by colopl_bc are based on the glibc `srand()` / `rand()` algorithm. The results may differ from those of the non glibc environments, e.g. Alpine Linux.

- `Colopl\ColoplBc\Php70\mt_srand()`

Always initialize the state using a **broken** Mersenne twister.

- `Colopl\ColoplBc\Php70\mt_rand()`

Generate random numbers.

> **WARNING**
> This function previously caused undefined behavior and returned different results depending on the CPU architecture. The current behavior is forced to mimic the gcc + amd64 results, and may return different results for PHP in other environments.

- `Colopl\ColoplBc\Php70\date_create()`
- `Colopl\ColoplBc\Php70\date_create_immutable()`

Always instantiate DateTime without milliseconds.
