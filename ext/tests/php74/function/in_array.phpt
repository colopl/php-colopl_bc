--TEST--
Check for \Colopl\ColoplBc\Php74\in_array() function. (colopl_bc.php74.compare_mode=3)
--INI--
date.timezone=UTC
error_log=
colopl_bc.php74.compare_mode=3
colopl_bc.php74.sort_mode=3
--EXTENSIONS--
colopl_bc
--FILE--
<?php
$array = [0];
if (!\Colopl\ColoplBc\Php74\in_array('', $array)) {
    die('Failure.');
}
die('Success.');
?>
--EXPECTF--
Incompatible compare detected

Deprecated: Colopl\ColoplBc\Php74\in_array(): Incompatible compare detected in %s on line %d
Success.