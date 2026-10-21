# colopl_bc

Provides various compatibility functions required for PHP (temporary) migration.

> [!WARNING]
> This extension is intended for temporary use only. In other words, code in the production environment should not rely on this implementation as much as possible, and should be gradually phased out when it is safe to do so.

## Security notice: CVE-2026-109618 / CVE-2026-109619 (2026-10-21)

> [!CAUTION]
> **DO NOT USE THIS EXTENSION AS YOUR RESPONSE TO CVE-2026-109618 / CVE-2026-109619. IT IS NOT A FIX, IT IS NOT A WORKAROUND, AND A HOST THAT RUNS IT IS STILL VULNERABLE.**
>
> These are not `session.upload_progress` bugs. They are engine-wide: (1) a request that is cut off by an out-of-memory error leaves half-initialized engine structures behind, and the request shutdown then reads them; (2) request-owned state can survive in the worker after its memory is freed, and the next request uses it before initialization resets it. `session.upload_progress` is only the first public path into them. php.net names others (fatal error backtraces on 8.5, diagnostics emitted before initialization, any attacker-reachable fatal error or execution timeout), and the fix (php-src 01a40e89) touches the allocator, the hash table, the VM, the request lifecycle and several extensions. No extension can fix that, and this one does not try to: everything it does reduces exposure to specific known paths, nothing more.
>
> **The fix is PHP itself:**
> - PHP 8.2 - 8.5: **upgrade to PHP 8.2.35, 8.3.36, 8.4.27, 8.5.12 or later.**
> - PHP 7.4 / 8.0 / 8.1 (end of life): the PHP security team published **minimal source backports** as git branches, not releases: [`PHP-7.4-CVE-2026-109618-109619`](https://github.com/php/php-src/tree/PHP-7.4-CVE-2026-109618-109619), [`PHP-8.0-CVE-2026-109618-109619`](https://github.com/php/php-src/tree/PHP-8.0-CVE-2026-109618-109619), [`PHP-8.1-CVE-2026-109618-109619`](https://github.com/php/php-src/tree/PHP-8.1-CVE-2026-109618-109619). Apply that commit if you build from source, or install the vendor package that carries it (see [Official minimal backports](#official-minimal-backports-for-php-74--80--81)), then plan the upgrade to a supported version.
> - PHP 5.4 - 7.3: nothing exists. Migrate.
>
> **This extension** carries emergency stopgaps written during the disclosure window for the hours or days until one of the above is in place. They have not been reviewed by the PHP security team, they cover only the paths listed under [Security backports](#security-backports), they cannot reproduce the fix (see [What this extension cannot do](#what-this-extension-cannot-do)), and the session-related parts are silently inactive on builds whose session globals do not match the headers they were built against. If you are reading this to decide whether a host is covered: it is not. Treat it as vulnerable until PHP itself is fixed, and remove the stopgaps afterwards.

> [!IMPORTANT]
> Since **13.2.1** loading this extension applies the stopgaps **by default** on affected PHP versions; no configuration change is needed, and none makes a host safe. Since **13.3.0** the session-related parts follow the published upstream fix where an extension can. Since **13.4.0** a memory error or an execution timeout ends the worker instead of bailing out, which is the only answer an extension has to the engine-wide part. Since **13.5.0** the request state that the next request of a worker could read before its own initialization is cleared at the end of every request, as the fix does.
>
> **CVEs:** CVE-2026-109618 (RCE via OOM corruption, GHSA-w8hw-w9fx-j79w) / CVE-2026-109619 (RCE via stale global state, GHSA-6wmf-4rv5-p3rp); php-src commit 01a40e89, "Avoid state corruption via bailout"; fixed in PHP 8.2.35 / 8.3.36 / 8.4.27 / 8.5.12. On PHP older than 7.2.28 / 7.3.15 / 7.4.3 the session wrapper also guards CVE-2020-7062.

The upstream fix has two parts. (1) Engine-wide, the out-of-memory bailout is replaced by an unwinding exit so that a request that hits `memory_limit` can no longer leave half-initialized structures behind, and request-owned state is reset where the next request could see it. (2) The `session.upload_progress` handler, which runs before RINIT, is made safe: the (un)serializer and session request state are reset at the start of every multipart body, the session is decoded without resolving any class (`__PHP_Incomplete_Class`, payload preserved), events delivered after RINIT are ignored, and the pre-RINIT `$_SESSION` is dropped at the end of the body.

This extension provides, by default on affected versions: for (1), ending the worker at a memory error or execution timeout instead of bailing out (`colopl_bc.security.abort_on_memory_error` / `abort_on_timeout`, 13.4.0+); for (2), the state reset at the start of the body, the `$_SESSION` cleanup, the post-RINIT guard, plus the earlier hardening (pointer rebinding, no session ID from the body with `session.use_only_cookies=1`, no tracking of keys the serializer cannot store). See [Security backports](#security-backports).

| PHP | `colopl_bc.security.harden_session_upload_progress` |
|---|---|
| 5.4 - 8.1 | every release: enabled by default (`1`), also on builds with the official minimal backport (see below) |
| 8.2 | < 8.2.35: enabled by default (`1`) |
| 8.3 | < 8.3.36: enabled by default (`1`) |
| 8.4 | < 8.4.27: enabled by default (`1`) |
| 8.5 | < 8.5.12: enabled by default (`1`) |
| fixed releases | not compiled in (`0`): nothing to do, PHP is fixed |

### Official minimal backports for PHP 7.4 / 8.0 / 8.1

The branches linked above hold one commit each ("Avoid state corruption via bailout — Minimal mitigation for EOL version", by the PHP security team). Vendors have started shipping it; shivammathur/homebrew-php `php@8.1` 8.1.34_4 (used by setup-php on macOS) is one, and the Ubuntu 22.04 `php8.1` update published the same day (8.1.2-1ubuntu2.28) shows the same behaviour in this repository's package build. **`PHP_VERSION` / `PHP_VERSION_ID` do not change** (the 7.4 branch even reports `7.4.31-dev`), so neither this extension nor your tooling can tell a backported build from a vulnerable one by version. What the backport does:

| Official minimal backport (7.4 / 8.0 / 8.1) | This extension |
|---|---|
| A memory error (`memory_limit`, out of memory, integer overflow in an allocation) prints `Fatal error: ...` to **stderr** and ends the process with `_exit(1)`, without going through the error callback: no error log entry, no `display_errors` output, no shutdown functions. | `colopl_bc.security.abort_on_memory_error` does the same for the same errors from the error callback, with the error log entry and the display. On a backported build the callback is never reached, so the directive has no effect. |
| `session.upload_progress` is turned off unconditionally (the handler returns before doing anything; the INI still reads `1`). | `colopl_bc.security.disable_session_upload_progress=1`. The hardening wrapper still installs on such builds and reports `active`; it is harmless there and does nothing useful. |
| State the next request could see is reset at request shutdown: `PG(modules_activated)`, the `$_GET` / `$_POST` / ... globals, SAPI request info, user error / exception handlers, the object store. | `colopl_bc.security.reset_stale_request_state` (13.5.0+) clears the same state from the module's post-RSHUTDOWN hook and a wrapped SAPI deactivation. |
| Multipart parser: duplicate and unused part headers are dropped, new `max_multipart_body_parts` INI (default `max_input_vars + max_file_uploads`), size fixes. | Not possible in an extension (item 4 below). |
| GPC arrays allocated without hash storage, `HT_HASH_RESET` fix, 8.1 only: opcache persistence fixes. | Not possible in an extension. |
| `max_execution_time` handling is unchanged. | `colopl_bc.security.abort_on_timeout` stays relevant. |

The backport is not the fix of 8.2.35+: there is no unwinding exit and no class-free session decode, upload progress is simply removed, and a memory error now kills the worker without a trace in the PHP error log. To tell the builds apart, run `php -n -d memory_limit=2M -d colopl_bc.security.abort_on_memory_error=0 -r 'register_shutdown_function(function () { echo "bailout\n"; }); str_repeat("x", 1 << 26);'`: an unpatched build prints `bailout` (the shutdown function ran after the bailout), a backported build prints only the `Fatal error` line on stderr and exits with status 1.

### What this extension cannot do

Read this before deciding that a host is covered. This list is what is known today, not everything. On an unfixed PHP build with this extension loaded, the following remain:

1. **Code runs before RINIT when the session is decoded.** The upstream fix decodes the session without resolving classes. An extension cannot do that, so objects stored in the session are instantiated before RINIT and their (internal-class) `__wakeup` / `__unserialize` run on uninitialized request state; `unserialize_callback_func`, if configured, is called as well. Reachable when an attacker can put objects into a session that gets decoded (serialize-handler mismatch, an application that stores attacker-controlled serialized data in the session, a known session ID).
2. **Out-of-memory bailouts in general (mitigated since 13.4.0, see below).** The upstream fix replaces the `memory_limit` bailout (`longjmp()`) engine-wide because a request that is cut off while a structure is allocated but not yet initialized leaves that structure behind, and the request shutdown then reads it (the fix adds tests for `array_fill()`, `array_pad()`, `SplFixedArray`, date interval parsing, SOAP, `getimagesizefromstring()`, stream copies, unfinished call frames, ...). It is independent of `session.upload_progress` and of the SAPI: after such a fatal error a php-fpm worker keeps serving requests, so an attacker can retry against the same process. An extension cannot make the engine unwind safely; what it can do, and does since 13.4.0 with `colopl_bc.security.abort_on_memory_error=1` (default), is to end the process at the memory error so that nothing reads the corrupted state. `colopl_bc.security.abort_on_timeout=1` (default) does the same for `max_execution_time`, which on PHP 5.4 - 7.0 is the other bailout that can start inside arbitrary code (since 7.1 it starts only at interrupt checks; see [its section](#colopl_bcsecurityabort_on_timeout-default-1)). Bailouts for other fatal errors are not covered.
3. **Stale request state (mitigated since 13.5.0, see below).** The upstream fix and the EOL backports clear, at request shutdown, the state that the next request of the worker could read before its own initialization (the `$_GET` / `$_POST` globals, SAPI request info, user error handlers, the object store, `PG(modules_activated)`); `colopl_bc.security.reset_stale_request_state=1` (default) clears the same state from the module's post-RSHUTDOWN hook. Since 13.5.0 the session wrapper also resets the URL rewriter (`session.use_trans_sid`) state at the start of the body, as the fix does. What remains is state this extension does not know about.
4. **Memory exhaustion by multipart headers / parts** is not limited (the upstream fix de-duplicates headers and checks `post_max_size` early; the backports add `max_multipart_body_parts`). Limit it at the web server (`LimitRequestBody`, `client_max_body_size`) and with `post_max_size` / `max_multipart_body_parts`.
5. **The session wrapper is only installed when the running PHP matches the headers the extension was built against** (version range and `php_ps_globals` size). Distribution patches, a build reused on another release, ZTS or Windows (untested) can leave it `inactive` without any error. `php -i | grep "pointer hardening"` must say `active`; otherwise none of the session mitigations apply.
6. **`unserialize()` of untrusted input in the application** is object injection regardless of this extension (`colopl_bc.security.isolate_unserialize` is off by default since 13.5.0, see its section).
7. **Any path that is not listed here.** The advisories describe classes of bugs, not a list of endpoints; php.net names paths this extension covers only in part: a diagnostic emitted by an invalid per-request setting before initialization and the `fatal_error_backtraces` path on 8.5 no longer find the previous request's freed handlers and object store (`reset_stale_request_state`), but whatever else such a diagnostic touches is not reset; fatal errors in callbacks or cleanup are bailouts this extension does not turn into process exits.

On an unfixed build, `colopl_bc.security.disable_session_upload_progress=1` (or `session.upload_progress.enabled=0`) closes 1 at the cost of the upload progress feature, `colopl_bc.security.abort_on_memory_error=1` / `abort_on_timeout=1` (default) turn 2 into a worker restart, and `colopl_bc.security.reset_stale_request_state=1` (default) clears the known part of 3. Nothing in this extension closes the rest; only the PHP fix does.

What to do:

1. **Fix PHP.** Upgrade to a fixed release (8.2+), apply the official minimal backport or your vendor's package (7.4 / 8.0 / 8.1), or migrate (5.4 - 7.3). This is the only fix. Everything below is for the hours until that is done.
2. Until then, reduce exposure as php.net recommends: `session.upload_progress.enabled=0` applied before request parsing (php.ini, FPM pool, server configuration) and a restart of every worker; `memory_limit=-1` set so that applications cannot override it; a low `post_max_size`; request size, part and header limits at the web server; on 8.5 `fatal_error_backtraces=0`. Load colopl_bc 13.5.0 or later in every SAPI (php-fpm, Apache, CLI) and set `colopl_bc.security.disable_session_upload_progress=1` unless the application needs upload progress.
3. Check that the stopgaps are active on every host: `php -i | grep "pointer hardening"` must report `active (php_rfc1867_callback wrapped)` (`inactive` shows the reason and means the session stopgaps do not apply), `php -i | grep "process abort"` must report `active` twice, and `php -i | grep "stale request state"` must report `active`.
4. Remove the stopgaps together with the PHP fix; do not keep them as the long-term answer.

Compatibility: upload progress entries are created and updated as before. With `session.upload_progress.cleanup=0` on PHP 7.2+, the final `bytes_processed` is the total that was read instead of a stale value. Uploads are not tracked when, with `session.use_only_cookies=1`, the session ID only comes from the request body, or when the progress key contains a character the `php` serializer cannot store (`|`); neither case works correctly today. `$_SESSION` is no longer populated before `session_start()` after an upload (as in the fixed releases). A request that hits `memory_limit` or `max_execution_time` ends the worker instead of running its shutdown functions. See [Security backports](#security-backports) for details.

## セキュリティ対応について: CVE-2026-109618 / CVE-2026-109619 (2026-10-21)

> [!CAUTION]
> **CVE-2026-109618 / CVE-2026-109619 への対応として本拡張を使わないでください。本拡張は修正でも回避策でもなく、本拡張を読み込んだホストは依然として脆弱です。**
>
> この 2 件は `session.upload_progress` のバグではありません。エンジン全体の問題です。(1) OOM エラーで打ち切られたリクエストは半初期化のエンジン内部構造を残し、リクエスト終了処理がそれを読みます。(2) リクエストが所有する状態がメモリ解放後もワーカーに残り、初期化でリセットされる前に次のリクエストがそれを使います。`session.upload_progress` は最初に公開された経路にすぎません。php.net は他の経路 (8.5 の fatal error バックトレース、初期化前に出力される診断メッセージ、攻撃者が到達できるあらゆる fatal error と実行タイムアウト) を挙げており、修正 (php-src 01a40e89) はアロケータ、ハッシュテーブル、VM、リクエストライフサイクル、複数の拡張に及びます。拡張にこれを修正することはできず、本拡張も修正を試みてはいません。本拡張がしているのは、既知の特定経路への露出を減らすことだけです。
>
> **修正は PHP 本体です:**
> - PHP 8.2 - 8.5: **PHP 8.2.35 / 8.3.36 / 8.4.27 / 8.5.12 以降に更新してください。**
> - PHP 7.4 / 8.0 / 8.1 (EOL): PHP セキュリティチームが **最小限のソースバックポート** をリリースではなく git ブランチとして公開しています: [`PHP-7.4-CVE-2026-109618-109619`](https://github.com/php/php-src/tree/PHP-7.4-CVE-2026-109618-109619)、[`PHP-8.0-CVE-2026-109618-109619`](https://github.com/php/php-src/tree/PHP-8.0-CVE-2026-109618-109619)、[`PHP-8.1-CVE-2026-109618-109619`](https://github.com/php/php-src/tree/PHP-8.1-CVE-2026-109618-109619)。ソースからビルドしている場合はこのコミットを適用し、そうでなければこれを含むベンダーのパッケージを導入してください (後述「[公式の最小バックポート](#公式の最小バックポート-php-74--80--81)」)。その上でサポート中のバージョンへの移行を計画してください。
> - PHP 5.4 - 7.3: 何も存在しません。移行してください。
>
> **本拡張** が持つのは、公開から上記のいずれかが入るまでの数時間〜数日をしのぐために公開期間中に書かれた緊急避難です。PHP セキュリティチームのレビューは受けておらず、[Security backports](#security-backports) に列挙した経路しか扱わず、修正を再現することはできず (「[本拡張にできないこと](#本拡張にできないこと)」)、セッション関連の部分はビルド時のヘッダとセッションのグローバル構造が一致しないビルドでは何も言わずに無効になります。ホストが保護されているかどうかを判断するためにこれを読んでいるなら、答えは「保護されていない」です。PHP 本体が修正されるまで脆弱なものとして扱い、修正後は緊急避難を取り除いてください。

> [!IMPORTANT]
> **13.2.1** 以降、本拡張を読み込むと、影響を受ける PHP バージョンで緊急避難が **デフォルトで** 適用されます。設定変更は不要ですが、どの設定もホストを安全にはしません。**13.3.0** 以降、セッション関連の部分は公開された upstream の修正のうち拡張から再現できる部分に追従しています。**13.4.0** 以降、メモリエラーと実行タイムアウトは bailout の代わりにワーカーを終了させます。これがエンジン全体の問題に対して拡張が持つ唯一の答えです。**13.5.0** 以降、ワーカーの次のリクエストが自身の初期化前に読み得るリクエスト状態を、修正と同様にリクエスト終了時にクリアします。
>
> **対象 CVE:** CVE-2026-109618 (OOM による破損経由の RCE、GHSA-w8hw-w9fx-j79w) / CVE-2026-109619 (残留したグローバル状態経由の RCE、GHSA-6wmf-4rv5-p3rp)。php-src コミット 01a40e89「Avoid state corruption via bailout」、PHP 8.2.35 / 8.3.36 / 8.4.27 / 8.5.12 で修正。7.2.28 / 7.3.15 / 7.4.3 より古い PHP ではセッションのラッパが CVE-2020-7062 も防ぎます。

upstream の修正は 2 本立てです。(1) エンジン全体で、OOM 時の bailout を unwinding exit に置き換え、`memory_limit` に達したリクエストが半初期化の構造を残せないようにし、次のリクエストから見える可能性のある状態をリセットする。(2) RINIT 前に走る `session.upload_progress` ハンドラを安全化する: multipart 本文の開始時に (un)serializer とセッションのリクエスト状態をリセットし、セッションをクラス解決なし (`__PHP_Incomplete_Class`、ペイロード保持) で復号し、RINIT 後に届いたイベントを無視し、本文終了時に RINIT 前の `$_SESSION` を削除する。

本拡張が影響バージョンでデフォルト提供するのは、(1) に対しては bailout の代わりにメモリエラー・実行タイムアウトでワーカーを終了させること (`colopl_bc.security.abort_on_memory_error` / `abort_on_timeout`、13.4.0 以降)、(2) に対しては本文開始時の状態リセット、`$_SESSION` の削除、RINIT 後のガード、およびそれ以前からの堅牢化 (ポインタの張り直し、`session.use_only_cookies=1` のとき本文からセッション ID を取らない、シリアライザが保存できないキーを追跡しない) です。詳細は [Security backports](#security-backports) を参照してください。

| PHP | `colopl_bc.security.harden_session_upload_progress` |
|---|---|
| 5.4 - 8.1 | 全リリース: デフォルト有効 (`1`)。公式の最小バックポートを含むビルド (後述) でも同じ |
| 8.2 | 8.2.35 未満: デフォルト有効 (`1`) |
| 8.3 | 8.3.36 未満: デフォルト有効 (`1`) |
| 8.4 | 8.4.27 未満: デフォルト有効 (`1`) |
| 8.5 | 8.5.12 未満: デフォルト有効 (`1`) |
| 修正済みリリース | 組み込まれません (`0`): PHP 側で修正済みのため不要 |

### 公式の最小バックポート (PHP 7.4 / 8.0 / 8.1)

上記のブランチはそれぞれ 1 コミット (「Avoid state corruption via bailout — Minimal mitigation for EOL version」、PHP セキュリティチーム) です。ベンダーの配布は始まっています。shivammathur/homebrew-php の `php@8.1` 8.1.34_4 (macOS の setup-php が使用) がその一つで、同日に公開された Ubuntu 22.04 の `php8.1` 更新 (8.1.2-1ubuntu2.28) も本リポジトリのパッケージビルドで同じ挙動を示します。**`PHP_VERSION` / `PHP_VERSION_ID` は変わりません** (7.4 ブランチは `7.4.31-dev` を名乗ります) ので、本拡張にも運用ツールにも、バックポート済みビルドと脆弱なビルドをバージョンで見分けることはできません。バックポートの内容:

| 公式の最小バックポート (7.4 / 8.0 / 8.1) | 本拡張 |
|---|---|
| メモリエラー (`memory_limit`、OOM、確保サイズの整数オーバーフロー) は `Fatal error: ...` を **stderr** に出力して `_exit(1)` でプロセスを終了する。エラーコールバックを通らないため、エラーログにも `display_errors` にも出力されず、shutdown 関数も走らない。 | `colopl_bc.security.abort_on_memory_error` は同じエラーに対してエラーコールバックから同じことをし、エラーログへの記録と表示は行う。バックポート済みビルドではコールバックに到達しないため、この設定は効果を持たない。 |
| `session.upload_progress` を無条件に無効化する (ハンドラが何もせずに戻る。INI の値は `1` のまま)。 | `colopl_bc.security.disable_session_upload_progress=1` 相当。堅牢化ラッパはこのビルドにも組み込まれ `active` と表示されるが、無害であり役にも立たない。 |
| 次のリクエストから見える状態をリクエスト終了時にリセットする: `PG(modules_activated)`、`$_GET` / `$_POST` などのグローバル、SAPI のリクエスト情報、ユーザーのエラー/例外ハンドラ、オブジェクトストア。 | `colopl_bc.security.reset_stale_request_state` (13.5.0 以降) がモジュールの post-RSHUTDOWN フックと SAPI deactivate の差し替えで同じ状態をクリアする。 |
| multipart パーサ: 重複ヘッダ・未使用ヘッダの破棄、新設の `max_multipart_body_parts` INI (デフォルト `max_input_vars + max_file_uploads`)、サイズ計算の修正。 | 拡張には不可能 (後述 4)。 |
| GPC 配列をハッシュ領域なしで確保、`HT_HASH_RESET` の修正、8.1 のみ: opcache の永続化の修正。 | 拡張には不可能。 |
| `max_execution_time` の扱いは変更なし。 | `colopl_bc.security.abort_on_timeout` は引き続き意味を持つ。 |

このバックポートは 8.2.35 以降の修正と同じものではありません。unwinding exit もクラス解決なしのセッション復号もなく、アップロード進捗は単に削除され、メモリエラーは PHP のエラーログに痕跡を残さずワーカーを終了させます。ビルドの見分け方: `php -n -d memory_limit=2M -d colopl_bc.security.abort_on_memory_error=0 -r 'register_shutdown_function(function () { echo "bailout\n"; }); str_repeat("x", 1 << 26);'` を実行して、未適用のビルドは `bailout` を出力し (bailout 後に shutdown 関数が走る)、バックポート済みのビルドは stderr に `Fatal error` 行だけを出力して終了ステータス 1 で終了します。

### 本拡張にできないこと

ホストが保護されていると判断する前に必ず読んでください。この一覧は現時点で分かっていることであり、すべてではありません。未修正の PHP に本拡張を読み込んでも、以下は残ります。

1. **セッション復号時に RINIT 前でコードが走る。** upstream はクラス解決なしで復号しますが、拡張にはできません。セッションに保存されたオブジェクトは RINIT 前に復元され、(内部クラスの) `__wakeup` / `__unserialize` が未初期化のリクエスト状態の上で走ります。`unserialize_callback_func` が設定されていればそれも呼ばれます。攻撃者が復号対象のセッションにオブジェクトを置ける場合 (serialize handler の不一致、攻撃者制御のシリアライズ値をセッションに書くアプリ、既知のセッション ID) に到達します。
2. **OOM による bailout 全般 (13.4.0 以降は緩和あり、後述)。** upstream が `memory_limit` 超過時の bailout (`longjmp()`) をエンジン全体で置き換えたのは、構造を確保して初期化し切る前に打ち切られたリクエストがその構造を残し、リクエスト終了処理がそれを読むためです (修正は `array_fill()`、`array_pad()`、`SplFixedArray`、日付間隔の解析、SOAP、`getimagesizefromstring()`、ストリームのコピー、未完了の呼び出しフレームなどのテストを追加しています)。`session.upload_progress` とも SAPI とも無関係で、この fatal の後も php-fpm のワーカーは生き続けるため、攻撃者は同じプロセスに対して何度でも試行できます。拡張にはエンジンを安全に巻き戻させることはできませんが、13.4.0 以降は `colopl_bc.security.abort_on_memory_error=1` (デフォルト) により、メモリエラーの時点でプロセスを終了させて壊れた状態を誰にも読ませないようにします。`colopl_bc.security.abort_on_timeout=1` (デフォルト) は `max_execution_time` に同じ扱いをします (PHP 5.4 - 7.0 ではこれも任意のコードの途中で始まり得る bailout です。7.1 以降は interrupt check でのみ始まります。詳細は [当該節](#colopl_bcsecurityabort_on_timeout-default-1))。その他の fatal error による bailout は対象外です。
3. **残留リクエスト状態 (13.5.0 以降は緩和あり、後述)。** upstream の修正と EOL 向けバックポートは、ワーカーの次のリクエストが自身の初期化前に読み得る状態 (`$_GET` / `$_POST` のグローバル、SAPI のリクエスト情報、ユーザーのエラーハンドラ、オブジェクトストア、`PG(modules_activated)`) をリクエスト終了時にクリアします。`colopl_bc.security.reset_stale_request_state=1` (デフォルト) はモジュールの post-RSHUTDOWN フックから同じ状態をクリアします。13.5.0 以降、セッションのラッパは修正と同様に本文開始時に URL 書き換え (`session.use_trans_sid`) の状態もリセットします。残るのは本拡張が知らない状態です。
4. **multipart のヘッダ・パート数によるメモリ枯渇**は制限しません (upstream はヘッダの重複排除と `post_max_size` の早期チェックを追加し、バックポートは `max_multipart_body_parts` を追加)。Web サーバ側 (`LimitRequestBody`、`client_max_body_size`) と `post_max_size` / `max_multipart_body_parts` で制限してください。
5. **セッションのラッパは、実行中の PHP がビルド時のヘッダと一致する場合にだけ組み込まれます** (バージョン範囲と `php_ps_globals` のサイズ)。ディストリビューションの独自パッチ、別リリースへのビルドの流用、ZTS や Windows (未検証) では、エラーなしに `inactive` になり得ます。`php -i | grep "pointer hardening"` が `active` でなければ、セッション関連の緩和はどれも効いていません。
6. **アプリケーションが信頼できない入力を `unserialize()` すること**は、本拡張の有無にかかわらずオブジェクト注入です (`colopl_bc.security.isolate_unserialize` は 13.5.0 以降デフォルト無効です。当該節を参照)。
7. **ここに書かれていないあらゆる経路。** アドバイザリが説明しているのはバグのクラスであってエンドポイントの一覧ではありません。php.net が挙げる経路のうち本拡張が部分的にしか覆わないものがあります。不正なリクエスト単位の設定が初期化前に出す診断メッセージと 8.5 の `fatal_error_backtraces` は、前リクエストの解放済みハンドラやオブジェクトストアには到達しなくなりますが (`reset_stale_request_state`)、その診断が触る他の状態はリセットされません。コールバックやクリーンアップ中の fatal error は、本拡張がプロセス終了に変えない bailout です。

未修正ビルドでは、`colopl_bc.security.disable_session_upload_progress=1` (または `session.upload_progress.enabled=0`) がアップロード進捗機能と引き換えに 1 を閉じ、`colopl_bc.security.abort_on_memory_error=1` / `abort_on_timeout=1` (デフォルト) が 2 をワーカーの再起動に変え、`colopl_bc.security.reset_stale_request_state=1` (デフォルト) が 3 の既知の部分をクリアします。残りは本拡張では閉じられず、PHP の修正でしか解決しません。

対応手順:

1. **PHP を修正してください。** 修正済みリリースへの更新 (8.2 以降)、公式の最小バックポートまたはそれを含むベンダーパッケージの適用 (7.4 / 8.0 / 8.1)、移行 (5.4 - 7.3)。これが唯一の修正です。以下はそれまでの数時間のためのものです。
2. それまでは、php.net の推奨どおり露出を減らしてください: リクエスト解析より前に適用される場所 (php.ini、FPM プール、サーバ設定) での `session.upload_progress.enabled=0` と全ワーカーの再起動、アプリケーションが上書きできない形での `memory_limit=-1`、小さい `post_max_size`、Web サーバでのリクエストサイズ・パート数・ヘッダ数の制限、8.5 では `fatal_error_backtraces=0`。あわせて colopl_bc 13.5.0 以降をすべての SAPI (php-fpm、Apache、CLI) で読み込み、アプリケーションがアップロード進捗を必要としない限り `colopl_bc.security.disable_session_upload_progress=1` を設定してください。
3. すべてのホストで緊急避難が有効になっていることを確認してください: `php -i | grep "pointer hardening"` が `active (php_rfc1867_callback wrapped)` と表示されること (`inactive` の場合は括弧内に理由が表示され、セッション関連の緊急避難は効いていません)、`php -i | grep "process abort"` が `active` を 2 行表示すること、`php -i | grep "stale request state"` が `active` を表示すること。
4. 緊急避難は PHP の修正とあわせて取り除いてください。恒久的な対策として残さないでください。

互換性: アップロード進捗のエントリはこれまでどおり作成・更新されます。PHP 7.2 以降で `session.upload_progress.cleanup=0` の場合、最終的な `bytes_processed` は古い値ではなく実際に読み込んだ総量になります。また、`session.use_only_cookies=1` でセッション ID がリクエスト本文にしかない場合と、進捗キーに `php` シリアライザが保存できない文字 (`|`) が含まれる場合は、進捗を追跡しません。どちらも現状では正しく動作していないケースです。アップロード後、`session_start()` の前に `$_SESSION` が存在しなくなります (修正済みリリースと同じ挙動)。`memory_limit` や `max_execution_time` に達したリクエストは shutdown 関数を実行せずにワーカーを終了させます。詳細は [Security backports](#security-backports) を参照してください。

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

`ext/colopl_bc_security.c` carries stopgaps for PHP security fixes that are not shipped, or shipped only as an unversioned source backport, for the PHP versions this extension targets (PHP 7.4, PHP 8.1 and older builds of supported branches). It is written against the lowest common denominator of the extension API and builds on every PHP release since 5.4, so it can also be loaded on PHP 5.x / 7.0 - 7.3 / 8.0 installations that cannot be upgraded immediately.

Each mitigation is controlled by its own `PHP_INI_SYSTEM` directive (`colopl_bc.security.*`), so it can only be set from `php.ini` or the SAPI command line, never from `.user.ini`, `.htaccess` or `ini_set()`. `phpinfo()` reports whether each mitigation is `active` (applied to this process) or `inactive` (disabled by INI, or not applicable to the running PHP version).

Default-value policy:

- A mitigation that re-implements the upstream fix while keeping the feature functional is enabled by default (`1`) when it cannot change the behaviour of valid input.
- A mitigation that can only work by disabling a feature is disabled by default (`0`) and must be opted in.
- A mitigation that only changes how an already failed request fails (process exit instead of a bailout) is enabled by default (`1`); it has no effect on requests that complete.

### `colopl_bc.security.disable_session_upload_progress` (default: `0`)

Detaches the `session.upload_progress` feature from the multipart/form-data parser by clearing the engine hook `php_rfc1867_callback` after `ext/session` has registered it (MINIT ordering is guaranteed through a module dependency on `session`).

- `0`: not applied. `session.upload_progress` behaves as in the running PHP version.
- `1`: applied. `$_SESSION` upload-progress entries are never created. File uploads themselves (`$_FILES`, `$_POST`) are not affected.

This is an opt-in, feature-disabling mitigation: unlike `session.upload_progress.enabled=0` it is enforced in the engine and cannot be re-enabled per directory or at runtime. Note that the hook is shared with any other extension that chains into `php_rfc1867_callback` (for example PECL `uploadprogress`); those are detached as well.

Applies to PHP >= 5.4 (where `session.upload_progress` exists), on fixed releases too: it is a feature switch, not a version-gated mitigation. On a 7.4 / 8.0 / 8.1 build with the [official minimal backport](#official-minimal-backports-for-php-74--80--81) the feature is already off and this changes nothing.

### `colopl_bc.security.harden_session_upload_progress` (default: `1` on affected PHP versions)

Keeps `session.upload_progress` functional while removing the stale-pointer class in `ext/session`'s upload progress callback. The callback caches raw zval pointers into the progress array it exports to `$_SESSION` (`bytes_processed`, `files`, the current file entry and its `bytes_processed`) and writes through them on every multipart event, although that array is shared with the session and replaced or separated by each update cycle. This mitigation wraps the hook installed by `ext/session` and, before each event that writes through those pointers, rebinds them to the array tree the progress record owns; at the end of the upload with `session.upload_progress.cleanup=0` it separates the array first so that the final write lands in the array that is saved. If the tree does not have the expected layout, progress tracking is dropped for that upload instead of following a cached pointer.

- `0`: not applied. `session.upload_progress` behaves as in the running PHP version.
- `1`: applied. Upload progress entries are created and updated as before. The only observable difference is that with `session.upload_progress.cleanup=0` on PHP 7.2+ the final `bytes_processed` is the total that was read (as on PHP 5.4 - 7.1) instead of a stale value.

When the running PHP is older than 7.2.28 / 7.3.15 / 7.4.3 (including every PHP 5.4 - 7.1 release), the wrapper also guards CVE-2020-7062 (bug #79221): with `cleanup=0`, a body that carries the session ID and the progress key but no file part makes `ext/session` store into a progress array that was never created at the end of the upload, which crashes the process. The guard makes `ext/session` skip that store, as the fixed releases do. `phpinfo()` reports it as `session.upload_progress CVE-2020-7062 guard`.

The wrapper also carries the parts of the 2026-10-21 upstream fix that an extension can provide:

- At the start of every multipart body it resets the (un)serializer state of `ext/standard` (`serialize_lock`, nesting state), the URL rewriter state (`session.use_trans_sid`; since 13.5.0) and the request state of `ext/session` (session ID, status, save handler data, `$_SESSION` storage), as the fixed releases do (`php_basic_early_rinit()`, `php_rinit_session_globals()`). The multipart parser runs before RINIT, so without this a request that follows a bailout of the previous request in the same process runs the upload progress handler on stale globals.
- After the body it removes the `$_SESSION` global that the upload progress handler populated before RINIT, so the script starts without it (as in the fixed releases); `session_start()` loads the session as usual.
- On PHP 8.4+, multipart events delivered after RINIT (`request_parse_body()`) are not forwarded to `ext/session`, as in the fixed releases.

Two parts of the upstream fix cannot be provided by an extension: the engine-wide replacement of the out-of-memory bailout by an unwinding exit, and decoding the session before RINIT without resolving any class (`__PHP_Incomplete_Class` with the original payload preserved), which needs the new unserializer mode. With this extension the pre-RINIT decode still instantiates objects stored in the session; `colopl_bc.security.disable_session_upload_progress=1` removes that code path entirely and is the complete mitigation on unfixed PHP builds.

The wrapper also closes two logic gaps of the hook that public proof-of-concept code relies on:

- With `session.use_only_cookies=1`, a form field named after the session (`PHPSESSID`) in the multipart body is no longer taken as the session ID of the progress record. `session_start()` never takes the session ID from the request body in that configuration, so only requests that rely on the inconsistency are affected; the field still reaches `$_POST`, and `session.use_only_cookies=0` keeps the previous behaviour.
- With the `php` serializer (`session.serialize_handler=php`, the default), a progress key containing `|` (and `!` before PHP 7.2) is not tracked. Such a key makes `php_session_encode()` fail on every progress update, and the session is then saved as an empty string, wiping the data stored under that session ID.

Affected PHP versions (compiled in and enabled by default): PHP <= 8.1 (every release), PHP 8.2 < 8.2.35, PHP 8.3 < 8.3.36, PHP 8.4 < 8.4.27, PHP 8.5 < 8.5.12, i.e. every release before the 2026-10-21 security release (GHSA-w8hw-w9fx-j79w / GHSA-6wmf-4rv5-p3rp, CVE-2026-109618 / CVE-2026-109619). On other versions the mitigation is not compiled in and the directive defaults to `0`. Because the callback's data layout is taken from the PHP headers used at build time, the wrapper is also only installed when the *running* PHP version is affected and the session globals match those headers; otherwise `phpinfo()` reports it as `inactive` with the reason. It is not installed when `colopl_bc.security.disable_session_upload_progress=1` has already detached the hook. On a 7.4 / 8.0 / 8.1 build with the [official minimal backport](#official-minimal-backports-for-php-74--80--81) it is installed and reports `active` (the version does not change), but `ext/session` no longer records upload progress on such builds, so it does nothing useful there.

### `colopl_bc.security.lock_session_serialize_handler` (default: `0`)

Narrows `session.serialize_handler` from `PHP_INI_ALL` to `PHP_INI_SYSTEM` in the engine's INI registry, after `ext/session` has registered it. The format a session file is decoded with can then only be chosen in `php.ini` or the SAPI configuration (`php_admin_value`, `-d`); `.user.ini`, `.htaccess`, `ini_set()` and `session_start()` options are refused by the engine, exactly as for any other `PHP_INI_SYSTEM` directive. Decoding session data that was written by one handler with another ("handler mismatch") is an object-injection primitive, which this removes for everything below the server configuration.

- `0`: not applied. `session.serialize_handler` stays `PHP_INI_ALL`.
- `1`: applied. The current value is kept; only its changeability is reduced. `ini_get_all('session', true)['session.serialize_handler']['access']` reports `4`.

This is an opt-in, feature-disabling mitigation: deployments that switch the handler per directory or per request keep working only with `0`. Applies to every PHP version (5.4+); when `ext/session` is not loaded there is nothing to lock and `phpinfo()` reports it as `inactive`.

### `colopl_bc.security.isolate_unserialize` (default: `0` since 13.5.0; `1` in 13.2.3 - 13.4.1)

> [!WARNING]
> **13.2.3 - 13.4.1 shipped this enabled by default, and that was a mistake: it breaks valid data.** PHP shares the `unserialize()` state with the nested `unserialize()` call that a `Serializable::unserialize()` implementation (or `SplObjectStorage`) makes, on purpose: a `C:` payload may back-reference objects of the enclosing data (php-src bug #36424, `SplObjectStorage_object_reference.phpt`), and `allowed_classes` / `max_depth` of the outer call apply inside the nested one (bug #72785). With this directive enabled those nested calls run on their own state: such data fails with `unserialize(): Error at offset ...`, and `allowed_classes` no longer applies inside `Serializable::unserialize()`. Seven tests of php-src's `ext/standard/tests/serialize` fail with it enabled. Upgrade to 13.5.0, or set `colopl_bc.security.isolate_unserialize=0`, if your application serializes `Serializable` objects or `SplObjectStorage` with cross references.

Runs every `unserialize()` call with `ext/standard`'s `serialize_lock` held, so that a nested `unserialize()` / `serialize()` call started from inside it never shares the outer call's state (the table that resolves `r:` / `R:` back-references and queues delayed `__wakeup` / `__unserialize` / destructor calls). It was meant for the case of a destructor that runs while the outer call tears its state down and calls `unserialize()` again (that nested call could resolve back-references into the state being destroyed); but the lock also cuts off the sharing that `Serializable::unserialize()` relies on, which an extension cannot tell apart from the wrapper.

- `0`: not applied (default). `unserialize()` behaves as in the running PHP version.
- `1`: applied. Data whose `Serializable` payloads back-reference the enclosing data cannot be unserialized, and `allowed_classes` / `max_depth` do not propagate into `Serializable::unserialize()`.

The underlying issue is php-src [GH-24373](https://github.com/php/php-src/issues/24373) (reported 2026-10-16, for `serialize()` and `unserialize()` alike) with the fix in [php-src PR #24452](https://github.com/php/php-src/pull/24452), which unregisters the shared state before destroying it; that is the correct fix and it cannot be reproduced from an extension. Note that the wrapper wraps the `unserialize()` function only.

### `colopl_bc.security.abort_on_memory_error` (default: `1`)

Ends the process when an allocation fails, instead of letting the engine bail out of the interrupted code with `longjmp()`: `memory_limit` exhausted, the system out of memory, or an allocation size that overflowed (`Possible integer overflow in memory allocation`, `Integer overflow in memory allocation`, `Balloc() failed to allocate memory`: the errors the official minimal backports for 7.4 / 8.0 / 8.1 end the process on as well). Every such error passes through `zend_error_cb` before the bailout; this mitigation replaces that callback and, for these errors, logs the error (`log_errors`, `error_log`) and displays it (`display_errors`, `html_errors`, `error_prepend_string` / `error_append_string`) in `php_error_cb()`'s formats, flushes the SAPI and calls `_exit(255)`. Nothing else of that request runs, and no later request of the process can read what the interrupted code left behind, which is the state-corruption class the 2026-10-21 security release fixes engine-wide (php-src 01a40e89). Other error types are passed to the original callback unchanged.

The error is not handed to `php_error_cb()` (since 13.5.0): for a memory error that function first discards every output buffer, which runs the user callbacks of those buffers after the error and, when the error happened inside a running output handler, frees that handler and raises a nested fatal error whose bailout would skip the exit (PHP 8.2.35 guards that call). The display therefore bypasses output buffering (`php_output_write_unbuffered()`); unflushed buffer contents are lost, as they are with a bailout. A fatal error raised while the process is being ended ends it immediately.

- `0`: not applied. A memory error bails out as before.
- `1`: applied. A memory error ends the worker; php-fpm, Apache (prefork) and CGI-style SAPIs start a new one, the client sees a dropped connection or a 502 instead of a 500.

Enabled by default because a request that hit `memory_limit` has already failed; what changes is only how it fails. What is lost for that request: shutdown functions (including custom out-of-memory error pages), destructors, output buffer callbacks, the session write, output buffers not yet flushed, `error_get_last()` for the error, and the deletion of temporary upload files. In a threaded SAPI (ZTS: Apache worker/event, Windows) the whole process with all its threads ends. If any of this matters more than the memory-safety of unfixed PHP builds, set it to `0`. Applies to every PHP version (5.4+). On a build with the [official minimal backport](#official-minimal-backports-for-php-74--80--81) for 7.4 / 8.0 / 8.1 the engine already ends the process on a memory error before the error callback is reached, so this directive has no effect there (and nothing is logged or displayed; the line goes to stderr).

### `colopl_bc.security.abort_on_timeout` (default: `1`)

The same treatment for the `max_execution_time` fatal error, which bails out through the same `zend_error_cb` path: with `1` the error is logged and displayed as usual and the process ends with `_exit(255)`; with `0` it bails out as before. The same losses as for `colopl_bc.security.abort_on_memory_error` apply to that request. Shares the callback with `abort_on_memory_error`; either directive installs it.

The upstream fix has no equivalent: it deliberately leaves `zend_timeout()` as a bailout (the delayed out-of-memory handling it adds sits right next to it in the interrupt helper). Where a timeout can interrupt differs by version, and with it what this directive buys:

- PHP 5.4 - 7.0: `zend_timeout()` is called from the `SIGPROF` handler itself, so the bailout can start anywhere, inside the allocator or in the middle of building a structure: the same hazard class as the memory error. Here the directive is a real mitigation, and these versions have no `hard_timeout`.
- PHP 7.1+: the signal handler only sets `EG(timed_out)` and the VM interrupt flag; the bailout starts at the next interrupt check, i.e. between two opcodes or in `zend_call_function()` after a user callback returned to an internal function. C code has to be exception-safe at those points already, so what a bailout leaves behind there is what any fatal error leaves behind (unfinished frames, per-request flags that RINIT resets), not a half-built structure. A timeout that is not acted on within `hard_timeout` seconds (default `2`) already ends the process from the signal handler (`_exit`, message to stderr): that is upstream's own answer to a timeout inside C code. On these versions the directive is belt and braces: it applies the hard-timeout treatment at the first opportunity and on every SAPI, at the cost of the shutdown processing listed above.

Enabled by default for the same reason as `abort_on_memory_error`: the request has already failed, only the way it fails changes. Set it to `0` if a timed-out request must still run its shutdown functions (an error page, for instance) or if worker churn on a slow endpoint matters more; on PHP 7.1+ `hard_timeout` then remains the upstream backstop.

### `colopl_bc.security.reset_stale_request_state` (default: `1`)

Clears, at the end of every request, the request-owned state that the next request of the same worker can read before its own initialization resets it, as the 2026-10-21 fix and the official minimal backports do in `php_request_shutdown()`, `shutdown_executor()` and `sapi_deactivate()` (CVE-2026-109619, "stale global state"). The state is released at shutdown but the pointers to it are kept; a diagnostic emitted before `init_executor()` (an invalid or deprecated per-directory INI value under mod_php, a `.user.ini` problem) is handed to the previous request's freed `set_error_handler()` callback, a fatal error there marks the previous request's freed objects as destructed, and a bailout early in request startup makes `php_request_shutdown()` release the previous request's `$_GET` / `$_POST` / ... a second time.

An extension cannot run inside those engine functions, but it can run right after them: the module's post-RSHUTDOWN hook (`post_deactivate_func`) is called after the superglobals were released and the executor was shut down, and `sapi_module.deactivate` is called by `sapi_deactivate()` right after it freed the request info strings. From there this mitigation clears:

- `$_GET` / `$_POST` / `$_COOKIE` / `$_SERVER` / `$_ENV` / `$_FILES` (`PG(http_globals)`) and `PG(modules_activated)`;
- the user error and exception handlers and their stacks, `EG(error_handling)`, `EG(exception)`, `EG(current_execute_data)`, the recorded-errors state (8.1+) and the last fatal error backtrace (8.5+);
- the object store's bucket array pointer (abandoned to the allocator by the fast shutdown) and its top index;
- the SAPI request info strings (`auth_user`, `auth_password`, `auth_digest`, `content_type_dup`, `current_user`; `post_data` / `raw_post_data` before PHP 7).

- `0`: not applied.
- `1`: applied. Nothing is freed or allocated; only pointers and flags of a finished request are cleared, so a request that completes is not affected in any way.

Enabled by default because it only touches state that is dead at that point. Applies to every PHP version (5.4+). `phpinfo()` reports it as `active (post-RSHUTDOWN hook, sapi deactivate wrapped)`; the SAPI deactivation wrapper chains to the original one. This closes the known pre-initialization readers of stale state; state this extension does not know about, and the diagnostics themselves, are not covered.

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

The values are bit flags and can be combined: `3` raises the deprecation and sends the log.

#### `colopl_bc.php74.sort_mode` (default: `0`)

Sets the compatibility check mode for various unstable sort algorithms.

- 0 (`COLOPL_BC_PHP74_SORT_MODE_SILENT`)

It does not perform compatibility checks and always uses the results of older behavior. Suitable for use in production environments.

- 1 (`COLOPL_BC_PHP74_SORT_MODE_DEPRECATED`)

Compares the result with the result of a native PHP function and raises an error equivalent to `E_DEPRECATED` if the results are different.

- 2 (`COLOPL_BC_PHP74_SORT_MODE_LOG`)

Compares the results of native PHP functions and sends a log to the running SAPI if the results are different.

The values are bit flags and can be combined: `3` raises the deprecation and sends the log.

### Constants

Please check the settings of the corresponding ini directives.

- `COLOPL_BC_PHP74_COMPARE_MODE_SILENT` = 0
- `COLOPL_BC_PHP74_COMPARE_MODE_DEPRECATED` = 1
- `COLOPL_BC_PHP74_COMPARE_MODE_LOG` = 2
- `COLOPL_BC_PHP74_SORT_MODE_SILENT` = 0
- `COLOPL_BC_PHP74_SORT_MODE_DEPRECATED` = 1
- `COLOPL_BC_PHP74_SORT_MODE_LOG` = 2

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
