--TEST--
security: the session.upload_progress pointer hardening is not installed when the hook is detached
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.disable_session_upload_progress=1
colopl_bc.security.harden_session_upload_progress=1
--SKIPIF--
<?php include dirname(__FILE__) . '/skipif_hardening.inc'; ?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
var_dump(strpos($info, 'session.upload_progress hook mitigation => active (php_rfc1867_callback detached)') !== false);
var_dump(strpos($info, 'session.upload_progress pointer hardening => inactive (session.upload_progress hook detached)') !== false);
?>
--EXPECT--
bool(true)
bool(true)
