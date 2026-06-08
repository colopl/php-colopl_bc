--TEST--
Check object compare diagnostics log without replacing original objects (deprecated)
--EXTENSIONS--
colopl_bc
--INI--
date.timezone=UTC
error_log=
colopl_bc.php74.compare_mode=1
--FILE--
<?php
class Box {
    public $v;
    public function __construct($v) { $this->v = $v; }
}

$a = new Box(0);
$b = new Box("foo");
$aobj = spl_object_id($a);
$bobj = spl_object_id($b);

echo 'before a:', $aobj, PHP_EOL;
echo 'before b:', $bobj, PHP_EOL;

var_dump(\Colopl\ColoplBc\Php74\eq($a, $b));

echo 'after a:', spl_object_id($a), PHP_EOL;
echo 'after b:', spl_object_id($b), PHP_EOL;

echo ($aobj === spl_object_id($a) && $bobj === spl_object_id($b)) ? 'OK' : 'NG', PHP_EOL;
?>
--EXPECTF--
before a:1
before b:2

Deprecated: Colopl\ColoplBc\Php74\eq(): Incompatible compare detected in %s on line %d
bool(true)
after a:1
after b:2
OK
