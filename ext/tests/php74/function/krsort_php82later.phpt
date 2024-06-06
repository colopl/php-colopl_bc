--TEST--
Check for \Colopl\ColoplBc\Php74\krsort() function. PHP >= 8.2 (colopl_bc.php74.compare_mode=3)
--EXTENSIONS--
colopl_bc
--INI--
date.timezone=UTC
error_log=
colopl_bc.php74.compare_mode=3
colopl_bc.php74.sort_mode=3
--SKIPIF--
<?php if (80200 > PHP_VERSION_ID) print 'skip require PHP >= 8.2'; ?>
--FILE--
<?php
$array = ['0' => 1, '' => 1, 'a' => 1];
\Colopl\ColoplBc\Php74\krsort($array);
var_dump($array);
?>
--EXPECTF--
Incompatible sort detected

Deprecated: Colopl\ColoplBc\Php74\krsort(): Incompatible sort detected in %s on line %d
array(3) {
  [0]=>
  int(1)
  ["a"]=>
  int(1)
  [""]=>
  int(1)
}
