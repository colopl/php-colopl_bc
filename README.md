# colopl_bc

Provides various compatibility functions required for PHP (temporary) migration.

> [!WARNING]
> This extension is intended for temporary use only. In other words, code in the production environment should not rely on this implementation as much as possible, and should be gradually phased out when it is safe to do so.

## Security notice: `session.upload_progress` (2026-10-21)

> [!CAUTION]
> **THIS EXTENSION IS NOT A SECURITY FIX. DO NOT USE IT AS YOUR RESPONSE TO CVE-2026-109618 / CVE-2026-109619.**
>
> **The only fix is the PHP security release of 2026-10-21: PHP 8.2.35, 8.3.36, 8.4.27 or 8.5.12 (or later). Upgrade to it. If you run an end-of-life PHP version (5.4 - 8.1), migrate to a supported version: no complete fix exists for those releases, in this extension or anywhere else.**
>
> **What this extension provides is an emergency stopgap for the days between the disclosure and your upgrade. It cannot reproduce the upstream fix (see "What this extension cannot do" below), it has not been reviewed by the PHP security team, and it is silently inactive on builds whose session globals do not match the headers it was built against. Treat a host that relies on it as still vulnerable.**

> [!IMPORTANT]
> Since **13.2.1**, loading this extension hardens the `session.upload_progress` handling of `ext/session` **by default** on affected PHP versions, including EOL releases that will not receive an upstream fix (PHP 5.4 - 8.1). No configuration change is needed. Since **13.3.0** the mitigations follow the published upstream fix where an extension can.
>
> **CVEs:** CVE-2026-109618 / CVE-2026-109619 (GHSA-w8hw-w9fx-j79w / GHSA-6wmf-4rv5-p3rp, php-src commit 01a40e89, "Avoid state corruption via bailout"). On PHP older than 7.2.28 / 7.3.15 / 7.4.3 the same wrapper also guards CVE-2020-7062.

The upstream fix has two parts. (1) Engine-wide, the out-of-memory bailout is replaced by an unwinding exit so that a request that hits `memory_limit` can no longer leave half-initialized structures behind. (2) The `session.upload_progress` handler, which runs before RINIT, is made safe: the (un)serializer and session request state are reset at the start of every multipart body, the session is decoded without resolving any class (`__PHP_Incomplete_Class`, payload preserved), events delivered after RINIT are ignored, and the pre-RINIT `$_SESSION` is dropped at the end of the body.

This extension provides, by default on affected versions: the state reset at the start of the body, the `$_SESSION` cleanup, the post-RINIT guard, plus the earlier hardening (pointer rebinding, no session ID from the body with `session.use_only_cookies=1`, no tracking of keys the serializer cannot store). See [Security backports](#security-backports).

| PHP | `colopl_bc.security.harden_session_upload_progress` |
|---|---|
| 5.4 - 8.1 | every release: enabled by default (`1`) |
| 8.2 | < 8.2.35: enabled by default (`1`) |
| 8.3 | < 8.3.36: enabled by default (`1`) |
| 8.4 | < 8.4.27: enabled by default (`1`) |
| 8.5 | < 8.5.12: enabled by default (`1`) |
| fixed releases | not compiled in (`0`): nothing to do, PHP is fixed |

### What this extension cannot do

Read this before deciding that a host is covered. On an unfixed PHP build with this extension loaded, the following remain:

1. **Code runs before RINIT when the session is decoded.** The upstream fix decodes the session without resolving classes. An extension cannot do that, so objects stored in the session are instantiated before RINIT and their (internal-class) `__wakeup` / `__unserialize` run on uninitialized request state; `unserialize_callback_func`, if configured, is called as well. Reachable when an attacker can put objects into a session that gets decoded (serialize-handler mismatch, an application that stores attacker-controlled serialized data in the session, a known session ID).
2. **Out-of-memory during script execution, on every SAPI.** The upstream fix replaces the `memory_limit` bailout (`longjmp()`) engine-wide because a request that is cut off while a structure is allocated but not yet initialized leaves that structure behind, and the request shutdown then reads it (the fix adds tests for `array_fill()`, `array_pad()`, `SplFixedArray`, date interval parsing, SOAP, `getimagesizefromstring()`, stream copies, unfinished call frames, ...). An extension cannot provide this. It is independent of `session.upload_progress` and of the SAPI: after such a fatal error a php-fpm worker keeps serving requests, so an attacker can retry against the same process. What this extension's state reset protects is only the *next multipart request* after a bailout *during request startup* (the session path) on SAPIs that keep the process alive after a startup failure (mod_php, LiteSpeed); php-fpm and CGI end the worker in that case. Reaching the execution-time path needs an application code path where attacker input drives the memory use up to `memory_limit` inside one of the affected operations; nothing in this extension changes that.
3. **URL rewriter (`session.use_trans_sid`) state** is not reset at the start of the body; the upstream fix resets it with a new internal function.
4. **Memory exhaustion by multipart headers / parts** is not limited (the upstream fix de-duplicates headers and checks `post_max_size` early). Limit it at the web server (`LimitRequestBody`, `client_max_body_size`) and with `post_max_size` / `max_multipart_body_parts`.
5. **The wrapper is only installed when the running PHP matches the headers the extension was built against** (version range and `php_ps_globals` size). Distribution patches, a build reused on another release, ZTS or Windows (untested) can leave it `inactive` without any error. `php -i | grep "pointer hardening"` must say `active`; otherwise none of the session mitigations apply.
6. **`unserialize()` of untrusted input in the application** is object injection regardless of this extension; `colopl_bc.security.isolate_unserialize` only removes one memory-safety primitive from it.

The one setting that closes 1, 3 and 4 and the session half of 2 on an unfixed build is `colopl_bc.security.disable_session_upload_progress=1` (or `session.upload_progress.enabled=0`), at the cost of the upload progress feature. Nothing in this extension closes the execution-time half of 2 or 6; only the PHP upgrade does.

What to do:

1. **Upgrade PHP to a fixed release.** This is the fix. Everything below is for the time until that is done.
2. Until then, load colopl_bc 13.3.0 or later in every SAPI (php-fpm, Apache, CLI) and set `colopl_bc.security.disable_session_upload_progress=1` unless the application needs upload progress.
3. Check that the mitigation is active on every host: `php -i | grep "pointer hardening"` must report `active (php_rfc1867_callback wrapped)`; `inactive` shows the reason in parentheses and means the host is not covered.
4. Plan the removal of this stopgap together with the PHP upgrade; do not keep it as the long-term answer.

Compatibility: upload progress entries are created and updated as before. With `session.upload_progress.cleanup=0` on PHP 7.2+, the final `bytes_processed` is the total that was read instead of a stale value. Uploads are not tracked when, with `session.use_only_cookies=1`, the session ID only comes from the request body, or when the progress key contains a character the `php` serializer cannot store (`|`); neither case works correctly today. `$_SESSION` is no longer populated before `session_start()` after an upload (as in the fixed releases). See [Security backports](#security-backports) for details.

## セキュリティ対応について: `session.upload_progress` (2026-10-21)

> [!CAUTION]
> **本拡張はセキュリティ修正ではありません。CVE-2026-109618 / CVE-2026-109619 への対応として本拡張を使わないでください。**
>
> **唯一の修正は 2026-10-21 の PHP セキュリティリリース (PHP 8.2.35 / 8.3.36 / 8.4.27 / 8.5.12 以降) です。これに更新してください。EOL の PHP (5.4 - 8.1) を使っている場合は、サポート中のバージョンへ移行してください。それらのリリースに対する完全な修正は、本拡張にも他のどこにも存在しません。**
>
> **本拡張が提供するのは、公開から更新までの数日をしのぐための緊急避難です。upstream の修正を再現することはできず (後述「本拡張にできないこと」)、PHP セキュリティチームのレビューも受けておらず、ビルド時のヘッダとセッションのグローバル構造が一致しないビルドでは何も言わずに無効になります。本拡張に頼っているホストは、依然として脆弱なものとして扱ってください。**

> [!IMPORTANT]
> **13.2.1** 以降、本拡張を読み込むと、影響を受ける PHP バージョンで `ext/session` の `session.upload_progress` 処理が **デフォルトで** 堅牢化されます。upstream の修正が提供されない EOL バージョン (PHP 5.4 - 8.1) も対象です。設定変更は不要です。**13.3.0** 以降は、公開された upstream の修正のうち拡張から再現できる部分に追従しています。
>
> **対象 CVE:** CVE-2026-109618 / CVE-2026-109619 (GHSA-w8hw-w9fx-j79w / GHSA-6wmf-4rv5-p3rp、php-src コミット 01a40e89「Avoid state corruption via bailout」)。7.2.28 / 7.3.15 / 7.4.3 より古い PHP では同じラッパが CVE-2020-7062 も防ぎます。

upstream の修正は 2 本立てです。(1) エンジン全体で、OOM 時の bailout を unwinding exit に置き換え、`memory_limit` に達したリクエストが半初期化の構造を残せないようにする。(2) RINIT 前に走る `session.upload_progress` ハンドラを安全化する: multipart 本文の開始時に (un)serializer とセッションのリクエスト状態をリセットし、セッションをクラス解決なし (`__PHP_Incomplete_Class`、ペイロード保持) で復号し、RINIT 後に届いたイベントを無視し、本文終了時に RINIT 前の `$_SESSION` を削除する。

本拡張が影響バージョンでデフォルト提供するのは、本文開始時の状態リセット、`$_SESSION` の削除、RINIT 後のガード、およびそれ以前からの堅牢化 (ポインタの張り直し、`session.use_only_cookies=1` のとき本文からセッション ID を取らない、シリアライザが保存できないキーを追跡しない) です。詳細は [Security backports](#security-backports) を参照してください。

| PHP | `colopl_bc.security.harden_session_upload_progress` |
|---|---|
| 5.4 - 8.1 | 全リリース: デフォルト有効 (`1`) |
| 8.2 | 8.2.35 未満: デフォルト有効 (`1`) |
| 8.3 | 8.3.36 未満: デフォルト有効 (`1`) |
| 8.4 | 8.4.27 未満: デフォルト有効 (`1`) |
| 8.5 | 8.5.12 未満: デフォルト有効 (`1`) |
| 修正済みリリース | 組み込まれません (`0`): PHP 側で修正済みのため不要 |

### 本拡張にできないこと

ホストが保護されていると判断する前に必ず読んでください。未修正の PHP に本拡張を読み込んでも、以下は残ります。

1. **セッション復号時に RINIT 前でコードが走る。** upstream はクラス解決なしで復号しますが、拡張にはできません。セッションに保存されたオブジェクトは RINIT 前に復元され、(内部クラスの) `__wakeup` / `__unserialize` が未初期化のリクエスト状態の上で走ります。`unserialize_callback_func` が設定されていればそれも呼ばれます。攻撃者が復号対象のセッションにオブジェクトを置ける場合 (serialize handler の不一致、攻撃者制御のシリアライズ値をセッションに書くアプリ、既知のセッション ID) に到達します。
2. **スクリプト実行中の OOM (すべての SAPI)。** upstream が `memory_limit` 超過時の bailout (`longjmp()`) をエンジン全体で置き換えたのは、構造を確保して初期化し切る前に打ち切られたリクエストがその構造を残し、リクエスト終了処理がそれを読むためです (修正は `array_fill()`、`array_pad()`、`SplFixedArray`、日付間隔の解析、SOAP、`getimagesizefromstring()`、ストリームのコピー、未完了の呼び出しフレームなどのテストを追加しています)。これは拡張では提供できません。`session.upload_progress` とも SAPI とも無関係で、この fatal の後も php-fpm のワーカーは生き続けるため、攻撃者は同じプロセスに対して何度でも試行できます。本拡張の状態リセットが守るのは、*リクエスト起動中* (セッション経路) の bailout の後、起動失敗後もプロセスを生かす SAPI (mod_php、LiteSpeed) で *次の multipart リクエスト* が stale な状態を使うことだけです (php-fpm と CGI は起動失敗時にワーカーを終了します)。実行中の経路に到達するには、攻撃者の入力で影響を受ける処理の中のメモリ使用量を `memory_limit` まで押し上げられるアプリケーション側のコードが必要ですが、本拡張はそこに何の影響も与えません。
3. **URL 書き換え (`session.use_trans_sid`) の状態**は本文開始時にリセットされません (upstream は新設の内部関数でリセットしています)。
4. **multipart のヘッダ・パート数によるメモリ枯渇**は制限しません (upstream はヘッダの重複排除と `post_max_size` の早期チェックを追加)。Web サーバ側 (`LimitRequestBody`、`client_max_body_size`) と `post_max_size` / `max_multipart_body_parts` で制限してください。
5. **ラッパは、実行中の PHP がビルド時のヘッダと一致する場合にだけ組み込まれます** (バージョン範囲と `php_ps_globals` のサイズ)。ディストリビューションの独自パッチ、別リリースへのビルドの流用、ZTS や Windows (未検証) では、エラーなしに `inactive` になり得ます。`php -i | grep "pointer hardening"` が `active` でなければ、セッション関連の緩和はどれも効いていません。
6. **アプリケーションが信頼できない入力を `unserialize()` すること**は、本拡張の有無にかかわらずオブジェクト注入です。`colopl_bc.security.isolate_unserialize` はそこからメモリ安全性のプリミティブを 1 つ取り除くだけです。

未修正ビルドで 1、3、4 と 2 のセッション側を閉じる唯一の設定は `colopl_bc.security.disable_session_upload_progress=1` (または `session.upload_progress.enabled=0`) で、代わりにアップロード進捗機能を失います。2 の実行中の経路と 6 は本拡張では閉じられず、PHP の更新でしか解決しません。

対応手順:

1. **PHP を修正済みリリースに更新してください。** これが修正です。以下はそれまでの間のためのものです。
2. それまでは、colopl_bc 13.3.0 以降をすべての SAPI (php-fpm、Apache、CLI) で読み込み、アプリケーションがアップロード進捗を必要としない限り `colopl_bc.security.disable_session_upload_progress=1` を設定してください。
3. すべてのホストで緩和が有効になっていることを確認してください: `php -i | grep "pointer hardening"` が `active (php_rfc1867_callback wrapped)` と表示されること。`inactive` の場合は括弧内に理由が表示され、そのホストは保護されていません。
4. この応急処置の撤去を PHP の更新とあわせて計画してください。恒久的な対策として残さないでください。

互換性: アップロード進捗のエントリはこれまでどおり作成・更新されます。PHP 7.2 以降で `session.upload_progress.cleanup=0` の場合、最終的な `bytes_processed` は古い値ではなく実際に読み込んだ総量になります。また、`session.use_only_cookies=1` でセッション ID がリクエスト本文にしかない場合と、進捗キーに `php` シリアライザが保存できない文字 (`|`) が含まれる場合は、進捗を追跡しません。どちらも現状では正しく動作していないケースです。アップロード後、`session_start()` の前に `$_SESSION` が存在しなくなります (修正済みリリースと同じ挙動)。詳細は [Security backports](#security-backports) を参照してください。

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

> [!CAUTION]
> These are emergency stopgaps, not fixes. Read [What this extension cannot do](#what-this-extension-cannot-do) first; the fix for the 2026-10-21 CVEs is the PHP security release.

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

The wrapper also carries the parts of the 2026-10-21 upstream fix that an extension can provide:

- At the start of every multipart body it resets the (un)serializer state of `ext/standard` (`serialize_lock`, nesting state) and the request state of `ext/session` (session ID, status, save handler data, `$_SESSION` storage), as the fixed releases do. The multipart parser runs before RINIT, so without this a request that follows a bailout of the previous request in the same process runs the upload progress handler on stale globals.
- After the body it removes the `$_SESSION` global that the upload progress handler populated before RINIT, so the script starts without it (as in the fixed releases); `session_start()` loads the session as usual.
- On PHP 8.4+, multipart events delivered after RINIT (`request_parse_body()`) are not forwarded to `ext/session`, as in the fixed releases.

Two parts of the upstream fix cannot be provided by an extension: the engine-wide replacement of the out-of-memory bailout by an unwinding exit, and decoding the session before RINIT without resolving any class (`__PHP_Incomplete_Class` with the original payload preserved), which needs the new unserializer mode. With this extension the pre-RINIT decode still instantiates objects stored in the session; `colopl_bc.security.disable_session_upload_progress=1` removes that code path entirely and is the complete mitigation on unfixed PHP builds.

The wrapper also closes two logic gaps of the hook that public proof-of-concept code relies on:

- With `session.use_only_cookies=1`, a form field named after the session (`PHPSESSID`) in the multipart body is no longer taken as the session ID of the progress record. `session_start()` never takes the session ID from the request body in that configuration, so only requests that rely on the inconsistency are affected; the field still reaches `$_POST`, and `session.use_only_cookies=0` keeps the previous behaviour.
- With the `php` serializer (`session.serialize_handler=php`, the default), a progress key containing `|` (and `!` before PHP 7.2) is not tracked. Such a key makes `php_session_encode()` fail on every progress update, and the session is then saved as an empty string, wiping the data stored under that session ID.

Affected PHP versions (compiled in and enabled by default): PHP <= 8.1 (every release), PHP 8.2 < 8.2.35, PHP 8.3 < 8.3.36, PHP 8.4 < 8.4.27, PHP 8.5 < 8.5.12, i.e. every release before the 2026-10-21 security release (GHSA-w8hw-w9fx-j79w / GHSA-6wmf-4rv5-p3rp, CVE-2026-109618 / CVE-2026-109619). On other versions the mitigation is not compiled in and the directive defaults to `0`. Because the callback's data layout is taken from the PHP headers used at build time, the wrapper is also only installed when the *running* PHP version is affected and the session globals match those headers; otherwise `phpinfo()` reports it as `inactive` with the reason. It is not installed when `colopl_bc.security.disable_session_upload_progress=1` has already detached the hook.

### `colopl_bc.security.lock_session_serialize_handler` (default: `0`)

Narrows `session.serialize_handler` from `PHP_INI_ALL` to `PHP_INI_SYSTEM` in the engine's INI registry, after `ext/session` has registered it. The format a session file is decoded with can then only be chosen in `php.ini` or the SAPI configuration (`php_admin_value`, `-d`); `.user.ini`, `.htaccess`, `ini_set()` and `session_start()` options are refused by the engine, exactly as for any other `PHP_INI_SYSTEM` directive. Decoding session data that was written by one handler with another ("handler mismatch") is an object-injection primitive, which this removes for everything below the server configuration.

- `0`: not applied. `session.serialize_handler` stays `PHP_INI_ALL`.
- `1`: applied. The current value is kept; only its changeability is reduced. `ini_get_all('session', true)['session.serialize_handler']['access']` reports `4`.

This is an opt-in, feature-disabling mitigation: deployments that switch the handler per directory or per request keep working only with `0`. Applies to every PHP version (5.4+); when `ext/session` is not loaded there is nothing to lock and `phpinfo()` reports it as `inactive`.

### `colopl_bc.security.isolate_unserialize` (default: `1`)

Runs every `unserialize()` call with `ext/standard`'s `serialize_lock` held, so that a nested `unserialize()` / `serialize()` call started from inside it never shares the outer call's state (the table that resolves `r:` / `R:` back-references and queues delayed `__wakeup` / `__unserialize` / destructor calls). `ext/standard` already holds that lock around every nested entry point it supports (`__wakeup`, `__unserialize`, `Serializable::unserialize()`, `unserialize_callback_func`), so those keep their behaviour; what changes is that code reached by accident while a call is still parsing or tearing down its state (destructors of values that are replaced or released during the call) can no longer resolve back-references into that state.

- `0`: not applied.
- `1`: applied. `unserialize()` results, references inside one string and the supported nested calls are unchanged. A destructor-started nested `unserialize()` with a back-reference into the outer call fails as if the reference did not exist, which was never a supported use.

Enabled by default because the supported nested entry points already run under the lock and valid input is unaffected; the wrapper is only installed on PHP versions the extension considers affected (currently every version, to be narrowed once the upstream fix for the 2026-10-21 security release is published). Note that it wraps the `unserialize()` function only; internal callers such as session decoding are not wrapped, but a nested `unserialize()` started from them goes through the wrapper and is isolated.

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
