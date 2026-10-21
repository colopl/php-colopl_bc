--TEST--
security: unserialize() isolation releases its lock when __wakeup throws
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.isolate_unserialize=1
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); ?>
--FILE--
<?php
class Thrower {
    public function __wakeup() { throw new Exception('wakeup'); }
}
try {
    unserialize('O:7:"Thrower":0:{}');
} catch (Exception $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}
// a following call must behave normally (lock balanced)
$r = unserialize(serialize(array('a' => 1)));
var_dump($r);
var_dump(unserialize(serialize(new stdClass)) instanceof stdClass);
?>
--EXPECT--
Exception: wakeup
array(1) {
  ["a"]=>
  int(1)
}
bool(true)
