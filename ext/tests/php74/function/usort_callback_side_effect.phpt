--TEST--
Check sort diagnostics do not invoke user comparison callbacks twice
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
$array = [2, 1];

\Colopl\ColoplBc\Php74\usort($array, static function (int $left, int $right) use (&$calls): int {
    ++$calls;
    return $left <=> $right;
});

var_dump($array);
var_dump($calls);
?>
--EXPECT--
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
int(1)
