--TEST--
Check key-preserving user sort diagnostics detect stable-sort incompatibility without invoking callbacks twice
--EXTENSIONS--
colopl_bc
--INI--
date.timezone=UTC
error_log=
colopl_bc.php74.compare_mode=0
colopl_bc.php74.sort_mode=3
--FILE--
<?php
foreach (['uasort', 'uksort'] as $function) {
    $calls = 0;
    $array = range(1, 64);
    $callable = "\\Colopl\\ColoplBc\\Php74\\{$function}";

    $callable($array, static function ($left, $right) use (&$calls): int {
        ++$calls;
        return 0;
    });

    echo $function, "\n";
    var_dump($calls > 0 && $calls < 300);
    var_dump(array_slice(array_keys($array), 0, 5));
    var_dump(array_slice(array_values($array), 0, 5));
}
?>
--EXPECTF--
Incompatible sort detected in %s on line %d

Deprecated: Colopl\ColoplBc\Php74\uasort(): Incompatible sort detected in %s on line %d
uasort
bool(true)
array(5) {
  [0]=>
  int(0)
  [1]=>
  int(48)
  [2]=>
  int(35)
  [3]=>
  int(36)
  [4]=>
  int(37)
}
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
Incompatible sort detected in %s on line %d

Deprecated: Colopl\ColoplBc\Php74\uksort(): Incompatible sort detected in %s on line %d
uksort
bool(true)
array(5) {
  [0]=>
  int(0)
  [1]=>
  int(48)
  [2]=>
  int(35)
  [3]=>
  int(36)
  [4]=>
  int(37)
}
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
