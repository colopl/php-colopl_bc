--TEST--
security: phpinfo() reports the session.upload_progress pointer hardening as inactive when disabled by INI
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.harden_session_upload_progress=0
--SKIPIF--
<?php include dirname(__FILE__) . '/skipif_hardening.inc'; ?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
var_dump(strpos($info, 'session.upload_progress pointer hardening => inactive (disabled by INI)') !== false);
var_dump(strpos($info, 'colopl_bc.security.harden_session_upload_progress => 0') !== false);
?>
--EXPECT--
bool(true)
bool(true)
