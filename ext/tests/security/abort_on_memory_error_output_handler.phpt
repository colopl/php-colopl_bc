--TEST--
security: a memory_limit fatal error inside a running output handler still ends the process (colopl_bc.security.abort_on_memory_error=1)
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.abort_on_memory_error=1
memory_limit=8M
display_errors=1
log_errors=0
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); include dirname(__FILE__) . '/skipif_memory_error.inc'; ?>
--FILE--
<?php
// php_error_cb() discards every output buffer for a memory error; with the
// handler that raised it still running, that frees the handler and raises a
// nested "Cannot use output buffering in output buffering display handlers"
// fatal error whose bailout would skip the abort. The abort must not go
// through php_error_cb() at all.
register_shutdown_function(function () { echo "shutdown function ran\n"; });
ob_start(function ($buffer) {
    $s = str_repeat('x', 64 * 1024 * 1024);
    return "handled: " . $buffer;
});
echo "buffered\n";
ob_end_flush();
echo "after\n";
?>
--EXPECTF--
%AFatal error: Allowed memory size of %d bytes exhausted%S (tried to allocate %d bytes) in %s on line %d
