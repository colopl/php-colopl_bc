--TEST--
security: phpinfo() reports the memory error and timeout process aborts as active by default
--EXTENSIONS--
colopl_bc
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); ?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
var_dump(strpos($info, 'memory error process abort => active (zend_error_cb wrapped') !== false);
var_dump(strpos($info, 'execution timeout process abort => active (zend_error_cb wrapped') !== false);
var_dump(ini_get('colopl_bc.security.abort_on_memory_error'));
var_dump(ini_get('colopl_bc.security.abort_on_timeout'));
var_dump(ini_set('colopl_bc.security.abort_on_memory_error', '0'));
?>
--EXPECT--
bool(true)
bool(true)
string(1) "1"
string(1) "1"
bool(false)
