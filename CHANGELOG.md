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
