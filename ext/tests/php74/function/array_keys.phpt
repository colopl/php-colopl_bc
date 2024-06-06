--TEST--
Check for \Colopl\ColoplBc\Php74\array_keys() function. (colopl_bc.php74.compare_mode=3)
--INI--
date.timezone=UTC
error_log=
colopl_bc.php74.compare_mode=3
colopl_bc.php74.sort_mode=3
--EXTENSIONS--
colopl_bc
--FILE--
<?php
$array = ['first' => 0, 'second' => ''];
if (!\Colopl\ColoplBc\Php74\array_keys($array, '') === ['first', 'second']) {
    die('Failure.');
}
die('Success.');
?>
--EXPECTF--
Incompatible compare detected

Deprecated: Colopl\ColoplBc\Php74\array_keys(): Incompatible compare detected in %s on line %d
Success.
