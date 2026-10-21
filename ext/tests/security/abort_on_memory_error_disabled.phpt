--TEST--
security: with colopl_bc.security.abort_on_memory_error=0 a memory_limit fatal error bails out and runs the shutdown functions
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.abort_on_memory_error=0
memory_limit=8M
display_errors=1
log_errors=0
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); ?>
--FILE--
<?php
register_shutdown_function(function () { echo "shutdown function ran\n"; });
echo "before\n";
$s = str_repeat('x', 64 * 1024 * 1024);
echo "after\n";
?>
--EXPECTF--
before

Fatal error: Allowed memory size of %d bytes exhausted%S (tried to allocate %d bytes) in %s on line %d
shutdown function ran
