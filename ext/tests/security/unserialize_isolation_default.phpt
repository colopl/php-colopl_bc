--TEST--
security: unserialize() isolation is on by default and PHP_INI_SYSTEM
--EXTENSIONS--
colopl_bc
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); ?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
var_dump(strpos($info, 'unserialize() nested-state isolation => active (unserialize() runs with serialize_lock held)') !== false);
var_dump(ini_get('colopl_bc.security.isolate_unserialize'));
var_dump(ini_set('colopl_bc.security.isolate_unserialize', '0'));
?>
--EXPECT--
bool(true)
string(1) "1"
bool(false)
