--TEST--
security: phpinfo() reports the session.upload_progress pointer hardening as active by default on affected PHP versions
--EXTENSIONS--
colopl_bc
--SKIPIF--
<?php include dirname(__FILE__) . '/skipif_hardening.inc'; ?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
var_dump(strpos($info, 'session.upload_progress pointer hardening => active (php_rfc1867_callback wrapped)') !== false);
var_dump(strpos($info, 'colopl_bc.security.harden_session_upload_progress => 1') !== false);
$guard = (PHP_VERSION_ID < 70228 || (PHP_VERSION_ID >= 70300 && PHP_VERSION_ID < 70315) || (PHP_VERSION_ID >= 70400 && PHP_VERSION_ID < 70403))
    ? 'active'
    : 'inactive (fixed in the running PHP version)';
var_dump(strpos($info, 'session.upload_progress CVE-2020-7062 guard => ' . $guard) !== false);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
