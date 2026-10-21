--TEST--
security: unserialize() works as before with colopl_bc.security.isolate_unserialize=1
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.isolate_unserialize=1
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); ?>
--FILE--
<?php
class Inner {
    public $v;
    public $woke = 0;
    public function __wakeup() { $this->woke++; }
}
class Outer {
    public $inner;
    public $copy;
    public $nested;
    public function __wakeup() {
        // nested unserialize() from a supported entry point keeps working
        $this->nested = unserialize(serialize(array(1, 2, 3)));
    }
}
$o = new Outer();
$o->inner = new Inner();
$o->inner->v = 'x';
$o->copy = $o->inner;          // r: back-reference inside the same string
$a = array(&$o, 'ref' => null);
$a['ref'] = &$a[0];            // R: reference inside the same string

$s = serialize($a);
$r = unserialize($s);

var_dump($r[0] instanceof Outer);
var_dump($r[0]->inner === $r[0]->copy);
var_dump($r[0]->inner->woke);
var_dump($r[0]->nested);
var_dump($r['ref'] === $r[0]);
var_dump(unserialize('b:0;'));
var_dump(@unserialize('garbage'));
var_dump(ini_get('colopl_bc.security.isolate_unserialize'));
?>
--EXPECT--
bool(true)
bool(true)
int(1)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
bool(true)
bool(false)
bool(false)
string(1) "1"
