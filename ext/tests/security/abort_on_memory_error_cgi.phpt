--TEST--
security: a memory_limit fatal error ends the process instead of bailing out (CGI)
--CGI--
--INI--
colopl_bc.security.abort_on_memory_error=1
memory_limit=8M
display_errors=1
log_errors=0
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); include dirname(__FILE__) . '/skipif_memory_error.inc'; ?>
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
