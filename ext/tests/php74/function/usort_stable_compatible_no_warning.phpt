--TEST--
Check user sort diagnostics do not warn when stable-sort differences are not observable
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
$array = array_fill(0, 64, 1);

\Colopl\ColoplBc\Php74\usort($array, static function (int $left, int $right) use (&$calls): int {
    ++$calls;
    return 0;
});

var_dump($calls > 0 && $calls < 300);
var_dump($array === array_fill(0, 64, 1));
?>
--EXPECT--
bool(true)
bool(true)
