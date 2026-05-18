--TEST--
Check user sort diagnostics detect stable-sort incompatibility without invoking callbacks twice
--EXTENSIONS--
colopl_bc
--INI--
date.timezone=UTC
error_log=
colopl_bc.php74.compare_mode=0
colopl_bc.php74.sort_mode=3
--FILE--
<?php
$calls = 0;
$array = range(1, 64);

\Colopl\ColoplBc\Php74\usort($array, static function (int $left, int $right) use (&$calls): int {
    ++$calls;
    return 0;
});

var_dump($calls > 0 && $calls < 300);
var_dump(array_slice($array, 0, 5));
?>
--EXPECTF--
Incompatible sort detected

Deprecated: Colopl\ColoplBc\Php74\usort(): Incompatible sort detected in %s on line %d
bool(true)
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(49)
  [2]=>
  int(36)
  [3]=>
  int(37)
  [4]=>
  int(38)
}
