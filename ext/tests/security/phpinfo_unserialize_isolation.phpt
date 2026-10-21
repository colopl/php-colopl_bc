--TEST--
security: phpinfo() reports the unserialize() isolation state
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.isolate_unserialize=1
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); ?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
var_dump(strpos($info, 'unserialize() nested-state isolation => active (unserialize() runs with serialize_lock held)') !== false);
var_dump(strpos($info, 'colopl_bc.security.isolate_unserialize => 1') !== false);
?>
--EXPECT--
bool(true)
bool(true)
