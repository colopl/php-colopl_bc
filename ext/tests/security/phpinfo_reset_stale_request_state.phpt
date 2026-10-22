--TEST--
security: phpinfo() reports the stale request state reset as active by default
--EXTENSIONS--
colopl_bc
--SKIPIF--
<?php include dirname(__FILE__) . '/skipif_backport.inc'; ?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
var_dump(strpos($info, 'stale request state reset => active (post-RSHUTDOWN hook, sapi deactivate wrapped') !== false);
var_dump(ini_get('colopl_bc.security.reset_stale_request_state'));
var_dump(ini_set('colopl_bc.security.reset_stale_request_state', '0'));
?>
--EXPECT--
bool(true)
string(1) "1"
bool(false)
