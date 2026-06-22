--TEST--
Check for colopl_bc.php74.compare_mode change runtime
--EXTENSIONS--
colopl_bc
--INI--
date.timezone=UTC
error_log=
--SKIPIF--
<?php if (PHP_VERSION_ID < 80000) die("skip PHP 8 compatibility diagnostics only"); ?>
--FILE--
<?php
error_reporting(E_ALL);
\Colopl\ColoplBc\Php74\eq('', 0);
ini_set('colopl_bc.php74.compare_mode', \COLOPL_BC_PHP74_COMPARE_MODE_DEPRECATED);
\Colopl\ColoplBc\Php74\eq('', 0);
ini_set('colopl_bc.php74.compare_mode', \COLOPL_BC_PHP74_COMPARE_MODE_LOG);
\Colopl\ColoplBc\Php74\eq('', 0);
ini_set('colopl_bc.php74.compare_mode', \COLOPL_BC_PHP74_COMPARE_MODE_DEPRECATED | \COLOPL_BC_PHP74_COMPARE_MODE_LOG);
\Colopl\ColoplBc\Php74\eq('', 0);
?>
--EXPECTF--
Deprecated: Colopl\ColoplBc\Php74\eq(): Incompatible compare detected in %s on line %d
Incompatible compare detected in %s on line %d
Incompatible compare detected in %s on line %d

Deprecated: Colopl\ColoplBc\Php74\eq(): Incompatible compare detected in %s on line %d
