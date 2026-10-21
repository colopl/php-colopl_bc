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
?>
--EXPECT--
bool(true)
bool(true)
