--TEST--
security: an allocation-size overflow fatal error ends the process like a memory_limit error (colopl_bc.security.abort_on_memory_error=1)
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.abort_on_memory_error=1
memory_limit=8M
display_errors=1
log_errors=0
--SKIPIF--
<?php
if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded');
if (PHP_VERSION_ID < 70000) die('skip str_repeat() refuses the size before allocating on PHP 5');
include dirname(__FILE__) . '/skipif_memory_error.inc';
?>
--FILE--
<?php
// 2 * PHP_INT_MAX + the string header overflows size_t: "Possible integer
// overflow in memory allocation", which the official minimal backports for
// PHP 7.4 / 8.0 / 8.1 end the process on as well.
register_shutdown_function(function () { echo "shutdown function ran\n"; });
echo "before\n";
$s = str_repeat(str_repeat('x', 2), PHP_INT_MAX);
echo "after\n";
?>
--EXPECTF--
before

Fatal error: Possible integer overflow in memory allocation (%s) in %s on line %d
