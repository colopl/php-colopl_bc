--TEST--
Check for colopl_bc.php74.sort_mode change runtime
--EXTENSIONS--
colopl_bc
--INI--
error_log=
--FILE--
<?php
error_reporting(E_ALL);

$array = ['a' => 1, '0' => 1, '' => 1, 'b' => 1, 'c' => 1, 'd' => 1, 'e' => 1, 'f' => 1, 'g' => 1, 'h' => 1, 'i' => 1, 'j' => 1, 'k' => 1, 'l' => 1, 'm' => 1, 'o' => 1, 'p' => 1, 'q' => 1, 'r' => 1, 's' => 1, 't' => 1, 'u' => 1, 'v' => 1, 'w' => 1, 'x' => 1, 'y' => 1, 'z' => 1];
$functions = ['arsort', 'asort', 'krsort', 'ksort', 'rsort', 'sort'];
$ufunctions = ['uasort', 'uksort', 'usort'];

foreach ($functions as $function) {
    $temp = $array;
    "\\Colopl\\ColoplBc\\Php74\\{$function}"($temp);
}
foreach ($ufunctions as $ufunction) {
    $temp = $array;
    "\\Colopl\\ColoplBc\\Php74\\{$ufunction}"($temp, static fn (): int => 0);
}

ini_set('colopl_bc.php74.sort_mode', \COLOPL_BC_PHP74_SORT_MODE_DEPRECATED);
foreach ($functions as $function) {
    $temp = $array;
    "\\Colopl\\ColoplBc\\Php74\\{$function}"($temp);
}
foreach ($ufunctions as $ufunction) {
    $temp = $array;
    "\\Colopl\\ColoplBc\\Php74\\{$ufunction}"($temp, static fn (): int => 0);
}

ini_set('colopl_bc.php74.sort_mode', \COLOPL_BC_PHP74_SORT_MODE_LOG);
foreach ($functions as $function) {
    $temp = $array;
    "\\Colopl\\ColoplBc\\Php74\\{$function}"($temp);
}
foreach ($ufunctions as $ufunction) {
    $temp = $array;
    "\\Colopl\\ColoplBc\\Php74\\{$ufunction}"($temp, static fn (): int => 0);
}

ini_set('colopl_bc.php74.sort_mode', \COLOPL_BC_PHP74_SORT_MODE_DEPRECATED | \COLOPL_BC_PHP74_SORT_MODE_LOG);
foreach ($functions as $function) {
    $temp = $array;
    "\\Colopl\\ColoplBc\\Php74\\{$function}"($temp);
}
foreach ($ufunctions as $ufunction) {
    $temp = $array;
    "\\Colopl\\ColoplBc\\Php74\\{$ufunction}"($temp, static fn (): int => 0);
}

?>
--EXPECTF--
Deprecated: Colopl\ColoplBc\Php74\arsort(): Incompatible sort detected in %s on line %d

Deprecated: Colopl\ColoplBc\Php74\asort(): Incompatible sort detected in %s on line %d

Deprecated: Colopl\ColoplBc\Php74\krsort(): Incompatible sort detected in %s on line %d

Deprecated: Colopl\ColoplBc\Php74\ksort(): Incompatible sort detected in %s on line %d

Deprecated: Colopl\ColoplBc\Php74\uasort(): Incompatible sort detected in %s on line %d

Deprecated: Colopl\ColoplBc\Php74\uksort(): Incompatible sort detected in %s on line %d

Deprecated: Colopl\ColoplBc\Php74\usort(): Incompatible sort detected in %s on line %d
Incompatible sort detected
Incompatible sort detected
Incompatible sort detected
Incompatible sort detected
Incompatible sort detected
Incompatible sort detected
Incompatible sort detected
Incompatible sort detected

Deprecated: Colopl\ColoplBc\Php74\arsort(): Incompatible sort detected in %s on line %d
Incompatible sort detected

Deprecated: Colopl\ColoplBc\Php74\asort(): Incompatible sort detected in %s on line %d
Incompatible sort detected

Deprecated: Colopl\ColoplBc\Php74\krsort(): Incompatible sort detected in %s on line %d
Incompatible sort detected

Deprecated: Colopl\ColoplBc\Php74\ksort(): Incompatible sort detected in %s on line %d
Incompatible sort detected

Deprecated: Colopl\ColoplBc\Php74\uasort(): Incompatible sort detected in %s on line %d
Incompatible sort detected

Deprecated: Colopl\ColoplBc\Php74\uksort(): Incompatible sort detected in %s on line %d
Incompatible sort detected

Deprecated: Colopl\ColoplBc\Php74\usort(): Incompatible sort detected in %s on line %d
