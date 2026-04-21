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
