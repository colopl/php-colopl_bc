# colopl_bc

Provides various compatibility functions required for PHP (temporary) migration.

> [!WARNING]
> This extension is intended for temporary use only. In other words, code in the production environment should not rely on this implementation as much as possible, and should be gradually phased out when it is safe to do so.

## Security notice: `session.upload_progress` (2026-10-21)

> [!CAUTION]
> **DO NOT RELY ON THIS EXTENSION FOR SECURITY. THE MITIGATIONS BELOW ARE A TEMPORARY, BEST-EFFORT STOPGAP AND ARE NOT A REPLACEMENT FOR THE OFFICIAL PHP SECURITY RELEASE. UPGRADE PHP TO A SUPPORTED, PATCHED RELEASE AS SOON AS POSSIBLE. IF YOU RUN AN END-OF-LIFE PHP VERSION (5.4 - 8.1), MIGRATE TO A SUPPORTED VERSION.**

> [!IMPORTANT]
> Since **13.2.1**, loading this extension hardens the `session.upload_progress` handling of `ext/session` **by default** on affected PHP versions, including EOL releases that will not receive an upstream fix (PHP 5.4 - 8.1). No configuration change is needed.
>
> **Targeted CVEs:**
>
> - **CVE-2026-109618 / CVE-2026-109619** (presumed): the upstream advisory and fix had not been published when this mitigation was written, so the mapping is based on the public reports and proof-of-concept code.
> - **CVE-2020-7062**: on PHP older than 7.2.28 / 7.3.15 / 7.4.3, including every PHP 5.4 - 7.1 release.

This relates to the PHP security release scheduled for 2026-10-21 (publicly reported as CVE-2026-109618 / CVE-2026-109619) and the proof-of-concept code published for it, which targets the zval pointers that the upload progress callback caches into its progress array. The extension rebinds those pointers before every multipart event so that they can never point at a stale copy, while keeping `session.upload_progress` functional. On PHP older than 7.2.28 / 7.3.15 / 7.4.3 it also prevents the crash of CVE-2020-7062 in the same code path. It also stops the progress hook from taking the session ID from the request body when `session.use_only_cookies=1`, and from saving a session it cannot encode (which wipes it). The upstream fix had not been published when this mitigation was written; it will be re-evaluated once it is.

| PHP | `colopl_bc.security.harden_session_upload_progress` |
|---|---|
| 5.4 - 8.1 | every release: enabled by default (`1`) |
| 8.2 | <= 8.2.34: enabled by default (`1`) |
| 8.3 | <= 8.3.36: enabled by default (`1`) |
| 8.4 | <= 8.4.28: enabled by default (`1`) |
| 8.5 | <= 8.5.13: enabled by default (`1`) |
| other releases | not compiled in (`0`) |

What to do:

1. Upgrade to colopl_bc 13.2.1 or later and load the extension (for every SAPI, e.g. php-fpm, Apache, CLI).
2. Check that the mitigation is active: `php -i | grep "pointer hardening"` must report `active (php_rfc1867_callback wrapped)`. When it reports `inactive`, the reason is shown in parentheses.
3. Still upgrade PHP to a fixed release where one is available. If your application does not use upload progress at all, `session.upload_progress.enabled=0` or `colopl_bc.security.disable_session_upload_progress=1` removes the feature entirely.

Compatibility: upload progress entries are created and updated as before. With `session.upload_progress.cleanup=0` on PHP 7.2+, the final `bytes_processed` is the total that was read instead of a stale value. Uploads are not tracked when, with `session.use_only_cookies=1`, the session ID only comes from the request body, or when the progress key contains a character the `php` serializer cannot store (`|`); neither case works correctly today. See [Security backports](#security-backports) for details.

## セキュリティ対応について: `session.upload_progress` (2026-10-21)

> [!CAUTION]
> **セキュリティ対策を本拡張に頼らないでください。以下の緩和は一時的かつベストエフォートの応急処置であり、PHP 公式のセキュリティリリースの代わりにはなりません。できるだけ早く、サポート中で修正済みの PHP リリースへ更新してください。EOL の PHP (5.4 - 8.1) を使っている場合は、サポート中のバージョンへ移行してください。**

> [!IMPORTANT]
> **13.2.1** 以降、本拡張を読み込むと、影響を受ける PHP バージョンで `ext/session` の `session.upload_progress` 処理が **デフォルトで** 堅牢化されます。upstream の修正が提供されない EOL バージョン (PHP 5.4 - 8.1) も対象です。設定変更は不要です。
>
> **対応を想定している CVE:**
>
> - **CVE-2026-109618 / CVE-2026-109619** (推定): 本緩和の作成時点では upstream のアドバイザリと修正が未公開のため、公開されている報道と PoC に基づく対応付けです。
> - **CVE-2020-7062**: 7.2.28 / 7.3.15 / 7.4.3 より古い PHP (PHP 5.4 - 7.1 の全リリースを含む) が対象です。

2026-10-21 に予定されている PHP のセキュリティリリース (CVE-2026-109618 / CVE-2026-109619 として公に報じられているもの) と、それに対して公開された PoC に関する対応です。PoC は、アップロード進捗コールバックが進捗配列に対してキャッシュしている zval ポインタを標的にしています。本拡張は multipart の各イベントの直前にそれらのポインタを張り直し、古いコピーを指したままにならないようにします。`session.upload_progress` の機能はそのまま利用できます。また 7.2.28 / 7.3.15 / 7.4.3 より古い PHP では、同じ処理経路にある CVE-2020-7062 によるクラッシュも防ぎます。さらに、`session.use_only_cookies=1` のときに進捗フックがリクエスト本文からセッション ID を取り込むことと、encode できないセッションを保存して中身を消してしまうことも防ぎます。本緩和の作成時点では upstream の修正は未公開のため、公開後に改めて評価します。

| PHP | `colopl_bc.security.harden_session_upload_progress` |
|---|---|
| 5.4 - 8.1 | 全リリース: デフォルト有効 (`1`) |
| 8.2 | 8.2.34 以下: デフォルト有効 (`1`) |
| 8.3 | 8.3.36 以下: デフォルト有効 (`1`) |
| 8.4 | 8.4.28 以下: デフォルト有効 (`1`) |
| 8.5 | 8.5.13 以下: デフォルト有効 (`1`) |
| 上記以外のリリース | 組み込まれません (`0`) |

対応手順:

1. colopl_bc を 13.2.1 以降に更新し、拡張を読み込みます (php-fpm、Apache、CLI など、利用しているすべての SAPI で)。
2. 緩和が有効になっていることを確認します: `php -i | grep "pointer hardening"` が `active (php_rfc1867_callback wrapped)` と表示されること。`inactive` の場合は括弧内に理由が表示されます。
3. 修正済みの PHP リリースが提供されている場合は、PHP 自体の更新も行ってください。アップロード進捗機能を使っていない場合は、`session.upload_progress.enabled=0` または `colopl_bc.security.disable_session_upload_progress=1` で機能自体を無効化できます。

互換性: アップロード進捗のエントリはこれまでどおり作成・更新されます。PHP 7.2 以降で `session.upload_progress.cleanup=0` の場合、最終的な `bytes_processed` は古い値ではなく実際に読み込んだ総量になります。また、`session.use_only_cookies=1` でセッション ID がリクエスト本文にしかない場合と、進捗キーに `php` シリアライザが保存できない文字 (`|`) が含まれる場合は、進捗を追跡しません。どちらも現状では正しく動作していないケースです。詳細は [Security backports](#security-backports) を参照してください。

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

When the running PHP is older than 7.2.28 / 7.3.15 / 7.4.3 (including every PHP 5.4 - 7.1 release), the wrapper also guards CVE-2020-7062 (bug #79221): with `cleanup=0`, a body that carries the session ID and the progress key but no file part makes `ext/session` store into a progress array that was never created at the end of the upload, which crashes the process. The guard makes `ext/session` skip that store, as the fixed releases do. `phpinfo()` reports it as `session.upload_progress CVE-2020-7062 guard`.

The wrapper also closes two logic gaps of the hook that public proof-of-concept code relies on:

- With `session.use_only_cookies=1`, a form field named after the session (`PHPSESSID`) in the multipart body is no longer taken as the session ID of the progress record. `session_start()` never takes the session ID from the request body in that configuration, so only requests that rely on the inconsistency are affected; the field still reaches `$_POST`, and `session.use_only_cookies=0` keeps the previous behaviour.
- With the `php` serializer (`session.serialize_handler=php`, the default), a progress key containing `|` (and `!` before PHP 7.2) is not tracked. Such a key makes `php_session_encode()` fail on every progress update, and the session is then saved as an empty string, wiping the data stored under that session ID.

Affected PHP versions (compiled in and enabled by default): PHP <= 8.1 (every release), PHP 8.2 <= 8.2.34, PHP 8.3 <= 8.3.36, PHP 8.4 <= 8.4.28, PHP 8.5 <= 8.5.13. On other versions the mitigation is not compiled in and the directive defaults to `0`. Because the callback's data layout is taken from the PHP headers used at build time, the wrapper is also only installed when the *running* PHP version is affected and the session globals match those headers; otherwise `phpinfo()` reports it as `inactive` with the reason. It is not installed when `colopl_bc.security.disable_session_upload_progress=1` has already detached the hook.

### `colopl_bc.security.lock_session_serialize_handler` (default: `0`)

Narrows `session.serialize_handler` from `PHP_INI_ALL` to `PHP_INI_SYSTEM` in the engine's INI registry, after `ext/session` has registered it. The format a session file is decoded with can then only be chosen in `php.ini` or the SAPI configuration (`php_admin_value`, `-d`); `.user.ini`, `.htaccess`, `ini_set()` and `session_start()` options are refused by the engine, exactly as for any other `PHP_INI_SYSTEM` directive. Decoding session data that was written by one handler with another ("handler mismatch") is an object-injection primitive, which this removes for everything below the server configuration.

- `0`: not applied. `session.serialize_handler` stays `PHP_INI_ALL`.
- `1`: applied. The current value is kept; only its changeability is reduced. `ini_get_all('session', true)['session.serialize_handler']['access']` reports `4`.

This is an opt-in, feature-disabling mitigation: deployments that switch the handler per directory or per request keep working only with `0`. Applies to every PHP version (5.4+); when `ext/session` is not loaded there is nothing to lock and `phpinfo()` reports it as `inactive`.

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
