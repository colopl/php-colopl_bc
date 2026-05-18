--TEST--
Check compare diagnostics do not run native object conversion error handling
--EXTENSIONS--
colopl_bc
--INI--
date.timezone=UTC
error_log=
colopl_bc.php74.compare_mode=3
--FILE--
<?php
error_reporting(E_ALL);

$errors = 0;
set_error_handler(static function (int $errno, string $errstr) use (&$errors): bool {
    if (str_contains($errstr, 'could not be converted to int')) {
        ++$errors;
    }
    return true;
});

var_dump(\Colopl\ColoplBc\Php74\eq(new stdClass(), 1));
restore_error_handler();
var_dump($errors);
?>
--EXPECT--
bool(true)
int(1)
