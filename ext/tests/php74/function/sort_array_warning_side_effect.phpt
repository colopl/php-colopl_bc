--TEST--
Check sort diagnostics do not expose native warning handling side effects
--EXTENSIONS--
colopl_bc
--INI--
date.timezone=UTC
error_log=
colopl_bc.php74.compare_mode=0
colopl_bc.php74.sort_mode=3
--SKIPIF--
<?php if (PHP_VERSION_ID < 80000) die("skip PHP 8 compatibility diagnostics only"); ?>
--FILE--
<?php
error_reporting(E_ALL);

$errors = 0;
set_error_handler(static function (int $errno, string $errstr) use (&$errors): bool {
    if ($errstr === 'Array to string conversion') {
        ++$errors;
    }
    return true;
});

$array = [[2], [1]];
\Colopl\ColoplBc\Php74\sort($array, SORT_STRING);
restore_error_handler();

var_dump($errors);
?>
--EXPECT--
int(2)
