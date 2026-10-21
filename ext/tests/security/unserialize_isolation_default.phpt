--TEST--
security: unserialize() isolation is off by default (it breaks Serializable cross references) and PHP_INI_SYSTEM
--EXTENSIONS--
colopl_bc
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); ?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
var_dump(strpos($info, 'unserialize() nested-state isolation => inactive (disabled by INI)') !== false);
var_dump(ini_get('colopl_bc.security.isolate_unserialize'));
var_dump(ini_set('colopl_bc.security.isolate_unserialize', '1'));
?>
--EXPECT--
bool(true)
string(1) "0"
bool(false)
