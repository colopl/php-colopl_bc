# 13.5.0
- **`colopl_bc.security.isolate_unserialize` now defaults to `0`.** 13.2.3 - 13.4.1 enabled it by default on the claim that every supported nested entry point already ran under `serialize_lock`; that is wrong for `Serializable::unserialize()` (and `SplObjectStorage`), whose nested `unserialize()` shares the outer state on purpose so that `C:` payloads can back-reference the enclosing data (php-src bug #36424) and `allowed_classes` / `max_depth` apply inside them (bug #72785). With the wrapper enabled such data fails with "Error at offset" and `allowed_classes` does not propagate; seven php-src serialize tests fail. The directive stays available as an opt-in. The underlying teardown issue is php-src GH-24373, fixed properly by PR #24452
- Add `colopl_bc.security.reset_stale_request_state` INI directive (default: `1`): at the end of every request, clear the request-owned state that the next request of the same worker can read before its own initialization (the superglobal slots, `PG(modules_activated)`, the user error / exception handlers and their stacks, `EG(error_handling)`, `EG(exception)`, `EG(current_execute_data)`, the recorded-errors state on 8.1+, the last fatal error backtrace on 8.5+, the object store's bucket pointer, the SAPI request info strings), as the 2026-10-21 security release and the official minimal backports for 7.4 / 8.0 / 8.1 do in `php_request_shutdown()`, `shutdown_executor()` and `sapi_deactivate()`. Done from the module's post-RSHUTDOWN hook and a wrapped `sapi_module.deactivate`; nothing is freed
- `harden_session_upload_progress`: the reset at the start of every multipart body now also zeroes the URL rewriter state (`session.use_trans_sid`), as `php_basic_early_rinit()` does; with `use_trans_sid=1` the upload progress handler registers the session ID with the rewriter before RINIT, where a stale `active` flag would make it append to freed memory
- `abort_on_memory_error`: also ends the process on the allocation-size overflow errors (`Possible integer overflow in ...`, `Integer overflow in memory allocation`, `Balloc() failed to allocate memory`), which the official minimal backports end the process on as well
- `abort_on_memory_error` / `abort_on_timeout`: the error is logged and displayed by the extension itself instead of being handed to `php_error_cb()`, which for a memory error discards every output buffer (running their user callbacks after the error) and, when the error happened inside a running output handler, frees that handler and raises a nested fatal error whose bailout skipped the process exit (the abort could be bypassed from an output callback; PHP 8.2.35 guards the same call with `PHP_OUTPUT_LOCKED`). The display bypasses output buffering; a fatal error raised while the process is being ended ends it immediately
- Recognize PHP builds that carry an unversioned backport of the 2026-10-21 fix (the official minimal backports for 7.4 / 8.0 / 8.1, branches `PHP-x.y-CVE-2026-109618-109619`, and the vendor packages built from them: deb.sury.org 5.6 - 8.1, Remi 7.4 / 8.0 / 8.1, Ubuntu 22.04 `php8.1` 8.1.2-1ubuntu2.28, shivammathur/homebrew-php `php@8.1` 8.1.34_4), whose `PHP_VERSION_ID` does not change, by the exported function `zend_mm_safe_error_ex()` that all of them add; on such a build `abort_on_memory_error`, `harden_session_upload_progress` and `reset_stale_request_state` are not applied (the backport already ends the process on a memory error, turns upload progress off and clears the stale state) and `phpinfo()` reports them as `inactive (backported fix detected in the running PHP)`. `abort_on_timeout` stays applied. The detection is a heuristic (one symbol, not the behaviour) and can be wrong; the new `colopl_bc.security.detect_backported_fix` INI directive (default: `1`) set to `0` skips it
- Security-only builds (PHP 5.4 - 7.3, 8.0) get an RINIT that updates the TSRMLS cache on ZTS
- README: list the unversioned backports by vendor as of 2026-10-22 (deb.sury.org 5.6 - 8.1; Remi 7.4 / 8.0 / 8.1 and nothing for 5.4 - 7.3; Red Hat, AlmaLinux, Freexian ELTS, Ubuntu including ESM down to 5.5, TuxCare ELS for 5.4, CloudLinux alt-php in testing, Homebrew) instead of stating that nothing exists for 5.4 - 7.3, describe the detection and that it is a heuristic, and document the new directive
- Tests: memory error inside an output handler, allocation-size overflow, upload progress with `session.use_trans_sid=1`, three sequential requests in one CLI-server process with the stale-state reset, phpinfo for the new directives, the backport detection checked against the observed memory-error behaviour; the "active by default" phpinfo tests and the hardening tests are skipped on recognized backported builds. The CLI-server test is skipped on Windows (`proc_terminate()` only ends the `cmd.exe` wrapper there and the orphaned server kept `run-tests.php` waiting until the job timeout) and its check for a built-in extension no longer fails to parse

# 13.4.1
- Tests: skip the memory-error tests on PHP builds that already end the process on a memory error. The official minimal backports of the 2026-10-21 security release for PHP 7.4 / 8.0 / 8.1 (php-src branches `PHP-7.4-CVE-2026-109618-109619` etc., shipped for example by shivammathur/homebrew-php `php@8.1` 8.1.34_4 and Ubuntu 22.04 `php8.1` 8.1.2-1ubuntu2.28) print the error to stderr and `_exit(1)` before `zend_error_cb`, while `PHP_VERSION` stays unchanged; the skip is decided by a probe run in the tested SAPI, not by version
- README: rewrite the security notice around the two CVEs (engine-wide out-of-memory corruption and stale request state) instead of `session.upload_progress`, state more strongly that the extension must not be relied on, describe the official minimal backports for 7.4 / 8.0 / 8.1 and how they relate to each directive (`abort_on_memory_error` has no effect on such builds; the backport disables upload progress unconditionally), and add the stale-state and unlisted-path items to "What this extension cannot do"
- No code change in the extension itself

# 13.4.0
- Add `colopl_bc.security.abort_on_memory_error` INI directive (default: `1`): a `memory_limit` / out-of-memory fatal error ends the process (`_exit(255)` after the usual logging) instead of bailing out with `longjmp()`, so that neither the request shutdown nor a later request of the same process can read the state the interrupted code left behind (the class fixed engine-wide by the 2026-10-21 PHP security release). Shutdown functions, destructors, the session write and temporary upload files of that request are not processed
- Add `colopl_bc.security.abort_on_timeout` INI directive (default: `1`): the `max_execution_time` fatal error ends the process the same way
- README: the memory-error and timeout bailouts are now listed as mitigated; other fatal errors remain bailouts

# 13.3.0
- Align the `session.upload_progress` mitigations with the 2026-10-21 PHP security release (GHSA-w8hw-w9fx-j79w / GHSA-6wmf-4rv5-p3rp, CVE-2026-109618 / CVE-2026-109619, fixed in PHP 8.2.35 / 8.3.36 / 8.4.27 / 8.5.12):
  - The affected-version ranges now end at those releases; on fixed builds the pointer hardening is not compiled in
  - At the start of every multipart body the hardening wrapper resets the (un)serializer state of ext/standard and the request state of ext/session, as the fixed releases do, so that a request that follows a bailout does not run the upload progress handler on stale globals
  - After the body the `$_SESSION` global populated before RINIT by the upload progress handler is removed again, as the fixed releases do
  - On PHP 8.4+, multipart events delivered after RINIT (`request_parse_body()`) are no longer forwarded to ext/session, as the fixed releases do
- README: state explicitly that the extension is an emergency stopgap and not a response to the CVEs (the fix is the PHP security release), list what it cannot do (pre-RINIT class resolution, engine-wide OOM handling, URL rewriter state, multipart memory exhaustion, builds on which the wrapper is inactive, application-level `unserialize()`), and that `colopl_bc.security.disable_session_upload_progress=1` is the only setting that closes the session paths on unfixed builds

# 13.2.3
- Add `colopl_bc.security.isolate_unserialize` INI directive (default: `1`), which runs `unserialize()` with `serialize_lock` held so that nested calls started from destructors never share the outer call's back-reference / delayed-call state

# 13.2.2
- Add `colopl_bc.security.lock_session_serialize_handler` INI directive (default: `0`), which narrows `session.serialize_handler` to `PHP_INI_SYSTEM` so that the session decoding format cannot be changed per directory or at run time (handler-mismatch object injection)

# 13.2.1
- Add `colopl_bc.security.harden_session_upload_progress` INI directive, which wraps the `session.upload_progress` hook and rebinds the zval pointers that ext/session caches into its progress array before every event that writes through them (PHP 5.4+, separate PHP 5 / PHP 7+ code paths)
  - Enabled by default (`1`) on affected PHP versions: PHP <= 8.1 (every release), 8.2 <= 8.2.34, 8.3 <= 8.3.36, 8.4 <= 8.4.28, 8.5 <= 8.5.13; not compiled in (`0`) on other versions
  - Only installed when the running PHP version is affected as well and the session globals layout matches the headers the extension was built against
  - With `session.upload_progress.cleanup=0` on PHP 7.2+, the final `bytes_processed` is now the total that was read instead of a stale value
  - Also guards CVE-2020-7062 (bug #79221, crash at the end of an upload with `cleanup=0` and no file part) when the running PHP is older than 7.2.28 / 7.3.15 / 7.4.3
  - With `session.use_only_cookies=1`, a session ID form field in the multipart body is no longer taken for the progress record (as `session_start()` already behaves)
  - With the `php` serializer, a progress key containing `|` (and `!` before PHP 7.2) is not tracked instead of making every progress update save the session as an empty string
- Report the hardening and CVE-2020-7062 guard state in `phpinfo()`
- Fix `colopl_bc.security.disable_session_upload_progress` not being applied again on a subsequent MINIT when the module is not unloaded in between (static builds, `ZEND_DONT_UNLOAD_MODULES`)

# 13.2.0
- Add security backports (`ext/colopl_bc_security.c`), buildable on every PHP release since 5.4
- Build in "security only" mode on PHP versions that are not compatibility-layer targets (PHP 5.4 - 7.3, 8.0)
- Add `colopl_bc.security.disable_session_upload_progress` INI directive (default: `0`), which detaches the `session.upload_progress` hook (`php_rfc1867_callback`) at the engine level
- Declare an optional module dependency on `session` to guarantee MINIT ordering
- Report security backport state in `phpinfo()`

# 13.0.2
- Add Ubuntu 26.04 LTS deb package build

# 13.0.1
- fix extension version definition

# 13.0.0
- Fix: compatibility check mode affects side-effects

# 12.1.0
- Change the project license from PHP-3.01 to BSD-3-Clause
- Tighten package support to PHP 7.4 or PHP 8.1+
- Refactor internals toward PHP 8.1+ APIs
- Replace checkinstall-based Ubuntu 22.04 packaging with standard Debian package builds
- Restore tag-triggered GitHub Actions builds for Ubuntu 22.04 deb packages
- Fix in-source build on PHP 8.5 and later (#70)
- Improve `.deb` build from upstream method (#18)
- Official support for PIE

# 11.1.0
- Add Windows and macOS support
- Bump Pskel 2.0.0
- Fix SEGV on date_create, date_create_immutable (#30, https://github.com/colopl/php-colopl_bc/issues/30)

# 11.0.4
- Fix `phpinfo.phpt` for timezone

# 11.0.3
- Add support for PHP 8.4

# 11.0.2
- Add official support for PHP 8.3
- Upgrade pskel

# 11.0.1
- More strict bundled tests

# 11.0.0
- Support `colopl_timeshifter`

# 10.0.2
- Bump dependencies
- CI minor fixes

# 10.0.1
- Extension: Support PHP 8.2
- Library: Support PHP 8.2
- Rector: APCToAPCUFunctionMigrateRector to more safely
- Rector: Fix wrong code sample
- Tests: Fix invalid namespaces
- Tests: Add tests for ZTS build
- Other minor fixes (#1)

# 10.0.0

Initial release.
