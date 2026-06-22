--TEST--
Check sort diagnostics handle recursive arrays without diagnostic recursion
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
$array = [];
$array[0] = &$array;
$values = [$array];

\Colopl\ColoplBc\Php74\sort($values);
echo "done\n";
?>
--EXPECT--
done
