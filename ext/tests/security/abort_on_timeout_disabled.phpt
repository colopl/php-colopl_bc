--TEST--
security: with colopl_bc.security.abort_on_timeout=0 a max_execution_time fatal error bails out and runs the shutdown functions
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.abort_on_timeout=0
max_execution_time=1
display_errors=1
log_errors=0
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); ?>
--FILE--
<?php
register_shutdown_function(function () { echo "shutdown function ran\n"; });
echo "before\n";
$start = microtime(true);
while (microtime(true) - $start < 20) {
    // busy loop until the timeout fires
}
echo "after\n";
?>
--EXPECTF--
before

Fatal error: Maximum execution time of 1 second exceeded in %s on line %d
shutdown function ran
