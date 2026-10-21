--TEST--
security: unserialize() isolation can be disabled with colopl_bc.security.isolate_unserialize=0
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.isolate_unserialize=0
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); ?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
var_dump(strpos($info, 'unserialize() nested-state isolation => inactive (disabled by INI)') !== false);
var_dump(unserialize(serialize(array('a' => 1))));
?>
--EXPECT--
bool(true)
array(1) {
  ["a"]=>
  int(1)
}
