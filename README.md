# colopl_bc

Provides various compatibility functions required for PHP (temporary) migration.

> [!WARNING]
> This extension is intended for temporary use only. In other words, code in the production environment should not rely on this implementation as much as possible, and should be gradually phased out when it is safe to do so.

## Supported Versions

- Compatibility layer (`Colopl\ColoplBc\Php70`, `Colopl\ColoplBc\Php74`): PHP == 7.4 or PHP >= 8.1, 8.5 >= PHP
- Security backports only: PHP >= 5.4 (every release that is not a compatibility-layer target, including PHP 8.0)

PHP 8.0 is not supported by the compatibility layer. On PHP 7.4, the Extension provides the PHP 7.4 compatibility namespace as thin wrappers around the native PHP APIs.

On PHP versions outside the compatibility-layer targets the extension is built in "security only" mode: the compatibility functions and their INI directives are not compiled in, and `phpinfo()` reports `colopl_bc support => enabled (security backports only)`. See [Security backports](#security-backports).

## Usage

### Install

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

### Build Ubuntu 22.04 packages

Ubuntu 22.04 packages are built with standard Debian packaging via `dpkg-buildpackage`, not `checkinstall`.
The packaging definitions live alongside each build target under `build/ubuntu2204/debian` and `build/ubuntu2204_sury84/debian`.

Build packages for the official Ubuntu 22.04 PHP 8.1 stack:

```bash
$ docker build -f "build/ubuntu2204/Dockerfile" -t "colopl-bc-u2204-php81" .
$ mkdir -p "artifacts"
$ docker run --rm -e VERSION="x.y.z" -v "$(pwd)/artifacts:/tmp/artifacts" "colopl-bc-u2204-php81"
```

This target produces `php8.1-colopl-bc_x.y.z_<arch>_ubuntu22.04_default.deb`.

Build packages for Ubuntu 22.04 with the Ondrej Sury PHP 8.4 repository:

```bash
$ docker build -f "build/ubuntu2204_sury84/Dockerfile" -t "colopl-bc-u2204-php84" .
$ mkdir -p "artifacts"
$ docker run --rm -e VERSION="x.y.z" -v "$(pwd)/artifacts:/tmp/artifacts" "colopl-bc-u2204-php84"
```

This target produces `php8.4-colopl-bc_x.y.z_<arch>_ubuntu22.04_sury.deb`.

## Security backports

`ext/colopl_bc_security.c` carries mitigations for PHP security fixes that are not shipped for the PHP versions this extension targets (PHP 7.4, PHP 8.1 and older builds of supported branches). It is written against the lowest common denominator of the extension API and builds on every PHP release since 5.4, so it can also be loaded on PHP 5.x / 7.0 - 7.3 / 8.0 installations that cannot be upgraded immediately.

Each mitigation is controlled by its own `PHP_INI_SYSTEM` directive (`colopl_bc.security.*`), so it can only be set from `php.ini` or the SAPI command line, never from `.user.ini`, `.htaccess` or `ini_set()`. `phpinfo()` reports whether each mitigation is `active` (applied to this process) or `inactive` (disabled by INI, or not applicable to the running PHP version).

Default-value policy:

- A mitigation that re-implements the upstream fix while keeping the feature functional is enabled by default (`1`) when it cannot change the behaviour of valid input.
- A mitigation that can only work by disabling a feature is disabled by default (`0`) and must be opted in.

### `colopl_bc.security.disable_session_upload_progress` (default: `0`)

Detaches the `session.upload_progress` feature from the multipart/form-data parser by clearing the engine hook `php_rfc1867_callback` after `ext/session` has registered it (MINIT ordering is guaranteed through a module dependency on `session`).

- `0`: not applied. `session.upload_progress` behaves as in the running PHP version.
- `1`: applied. `$_SESSION` upload-progress entries are never created. File uploads themselves (`$_FILES`, `$_POST`) are not affected.

This is an opt-in, feature-disabling mitigation: unlike `session.upload_progress.enabled=0` it is enforced in the engine and cannot be re-enabled per directory or at runtime. Note that the hook is shared with any other extension that chains into `php_rfc1867_callback` (for example PECL `uploadprogress`); those are detached as well.

Applies to PHP >= 5.4 (where `session.upload_progress` exists). Once an upstream fix is published, the mitigation will become a no-op on fixed PHP builds.

### `colopl_bc.security.harden_session_upload_progress` (default: `1` on affected PHP versions)

Keeps `session.upload_progress` functional while removing the stale-pointer class in `ext/session`'s upload progress callback. The callback caches raw zval pointers into the progress array it exports to `$_SESSION` (`bytes_processed`, `files`, the current file entry and its `bytes_processed`) and writes through them on every multipart event, although that array is shared with the session and replaced or separated by each update cycle. This mitigation wraps the hook installed by `ext/session` and, before each event that writes through those pointers, rebinds them to the array tree the progress record owns; at the end of the upload with `session.upload_progress.cleanup=0` it separates the array first so that the final write lands in the array that is saved. If the tree does not have the expected layout, progress tracking is dropped for that upload instead of following a cached pointer.

- `0`: not applied. `session.upload_progress` behaves as in the running PHP version.
- `1`: applied. Upload progress entries are created and updated as before. The only observable difference is that with `session.upload_progress.cleanup=0` on PHP 7.2+ the final `bytes_processed` is the total that was read (as on PHP 5.4 - 7.1) instead of a stale value.

Affected PHP versions (compiled in and enabled by default): PHP <= 8.1 (every release), PHP 8.2 <= 8.2.34, PHP 8.3 <= 8.3.36, PHP 8.4 <= 8.4.28, PHP 8.5 <= 8.5.13. On other versions the mitigation is not compiled in and the directive defaults to `0`. Because the callback's data layout is taken from the PHP headers used at build time, the wrapper is also only installed when the *running* PHP version is affected and the session globals match those headers; otherwise `phpinfo()` reports it as `inactive` with the reason. It is not installed when `colopl_bc.security.disable_session_upload_progress=1` has already detached the hook.

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

> [!WARNING]
> `srand()` provided by colopl_bc are based on the glibc `srand()` / `rand()` algorithm. The results may differ from those of the non glibc environments, e.g. Alpine Linux.

- `Colopl\ColoplBc\Php70\rand()`

Generate random numbers.

> [!WARNING]
> `rand()` provided by colopl_bc are based on the glibc `srand()` / `rand()` algorithm. The results may differ from those of the non glibc environments, e.g. Alpine Linux.

- `Colopl\ColoplBc\Php70\getrandmax()`

Returns the maximum value that can be returned by a call to `Colopl\ColoplBc\Php70\rand()`.

> [!WARNING]
> `rand()` provided by colopl_bc are based on the glibc `srand()` / `rand()` algorithm. The results may differ from those of the non glibc environments, e.g. Alpine Linux.


- `Colopl\ColoplBc\Php70\shuffle()`

This function shuffles (randomizes the order of the elements in) an array.

> [!WARNING]
> `shuffle()` use `Colopl\ColoplBc\Php70\rand()` algorithm internally, by colopl_bc are based on the glibc `srand()` / `rand()` algorithm. The results may differ from those of the non glibc environments, e.g. Alpine Linux.

- `Colopl\ColoplBc\Php70\str_shuffle()`

str_shuffle() shuffles a string. One permutation of all possible is created.


> [!WARNING]
> `str_shuffle()` use `Colopl\ColoplBc\Php70\rand()` algorighm internally, by colopl_bc are based on the glibc `srand()` / `rand()` algorithm. The results may differ from those of the non glibc environments, e.g. Alpine Linux.

- `Colopl\ColoplBc\Php70\array_rand()`

Picks one or more random entries out of an array, and returns the key (or keys) of the random entries.

> [!WARNING]
> `array_rand()` use `Colopl\ColoplBc\Php70\rand()` algorithm internally, by colopl_bc are based on the glibc `srand()` / `rand()` algorithm. The results may differ from those of the non glibc environments, e.g. Alpine Linux.

- `Colopl\ColoplBc\Php70\mt_srand()`

Always initialize the state using a **broken** Mersenne twister.

- `Colopl\ColoplBc\Php70\mt_rand()`

Generate random numbers.

> [!WARNING]
> This function previously caused undefined behavior and returned different results depending on the CPU architecture. The current behavior is forced to mimic the gcc + amd64 results, and may return different results for PHP in other environments.

- `Colopl\ColoplBc\Php70\date_create()`
- `Colopl\ColoplBc\Php70\date_create_immutable()`

Always instantiate DateTime without milliseconds.

## License

This project is licensed under the BSD-3-Clause license. See [LICENSE](LICENSE).
