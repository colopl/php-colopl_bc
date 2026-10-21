--TEST--
security: abort_on_memory_error leaves other errors to the original handler
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.abort_on_memory_error=1
display_errors=1
log_errors=0
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); ?>
--FILE--
<?php
register_shutdown_function(function () { echo "shutdown function ran\n"; });
set_error_handler(function ($no, $str) { echo "handler: $str\n"; return true; });
trigger_error('user notice', E_USER_NOTICE);
restore_error_handler();
echo "before\n";
undefined_function_for_colopl_bc_test();
echo "after\n";
?>
--EXPECTF--
handler: user notice
before

Fatal error: %Ashutdown function ran
