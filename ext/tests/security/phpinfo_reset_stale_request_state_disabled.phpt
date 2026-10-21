--TEST--
security: phpinfo() reports the stale request state reset as inactive with colopl_bc.security.reset_stale_request_state=0
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.reset_stale_request_state=0
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); ?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
var_dump(strpos($info, 'stale request state reset => inactive (disabled by INI)') !== false);
var_dump(ini_get('colopl_bc.security.reset_stale_request_state'));
?>
--EXPECT--
bool(true)
string(1) "0"
