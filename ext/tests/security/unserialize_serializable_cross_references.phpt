--TEST--
security: with the default settings, unserialize() inside Serializable::unserialize() still shares the outer state (cross references, allowed_classes), as PHP intends
--EXTENSIONS--
colopl_bc
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); ?>
--FILE--
<?php
// php-src bug #36424: a Serializable payload may back-reference objects of the
// enclosing data, which only works while the nested unserialize() shares the
// outer var_hash. colopl_bc.security.isolate_unserialize=1 breaks this (the
// nested call fails with "Error at offset"), which is why it is off by default.
#[AllowDynamicProperties]
class a implements Serializable {
    function serialize() { return serialize(get_object_vars($this)); }
    function unserialize($s) { foreach (unserialize($s) as $p => $v) { $this->$p = $v; } }
}
class b extends a {}
class c extends b {}

$c = new c;
$c->a = new a;
$c->a->b = new b;
$c->a->b->c = $c;
$c->a->c = $c;
$c->a->b->a = $c->a;
$c->a->a = $c->a;

$s = serialize($c);
$d = unserialize($s);
var_dump(
    $d === $d->a->b->c,
    $d->a->a === $d->a,
    $d->a->b->a === $d->a,
    $d->a->c === $d
);

if (PHP_VERSION_ID >= 70100) {
    // php-src bug #72785 (fixed in 7.1): allowed_classes applies inside Serializable::unserialize() too
    class holder implements Serializable {
        public $inner;
        function serialize() { return serialize($this->inner); }
        function unserialize($s) { $this->inner = unserialize($s); }
    }
    class inner {}
    $h = new holder; $h->inner = new inner;
    $u = unserialize(serialize($h), array('allowed_classes' => array('holder')));
    var_dump(get_class($u->inner));
} else {
    var_dump('__PHP_Incomplete_Class');
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
string(22) "__PHP_Incomplete_Class"
