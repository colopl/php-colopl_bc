--TEST--
security: phpinfo() reports the stale request state reset as active by default
--EXTENSIONS--
colopl_bc
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); ?>
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
