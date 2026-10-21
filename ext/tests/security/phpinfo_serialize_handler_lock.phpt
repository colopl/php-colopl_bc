--TEST--
security: phpinfo() reports the session.serialize_handler lock
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.lock_session_serialize_handler=1
--SKIPIF--
<?php include dirname(__FILE__) . '/skipif.inc'; ?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
var_dump(strpos($info, 'session.serialize_handler lock => active (session.serialize_handler is PHP_INI_SYSTEM)') !== false);
var_dump(strpos($info, 'colopl_bc.security.lock_session_serialize_handler => 1') !== false);
?>
--EXPECT--
bool(true)
bool(true)
