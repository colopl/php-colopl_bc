--TEST--
Check for \Colopl\ColoplBc\Php74\array_multisort() function. (colopl_bc.php74.compare_mode=3)
--INI--
date.timezone=UTC
error_log=
colopl_bc.php74.compare_mode=3
colopl_bc.php74.sort_mode=3
--FILE--
<?php

$ary1 = array_fill(0, 100, 0);
$origAry2 = $ary2 = array_merge(...array_fill(0, 50, [0, '0']));
\Colopl\ColoplBc\Php74\array_multisort($ary1, $ary2);
var_dump($ary2 !== $origAry2);

?>
--EXPECT--
bool(true)
