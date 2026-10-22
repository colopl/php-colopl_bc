--TEST--
security: on a PHP build with a backport of the 2026-10-21 fix, the stopgaps it makes redundant are not applied
--EXTENSIONS--
colopl_bc
--SKIPIF--
<?php
if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded');
ob_start();
phpinfo(INFO_MODULES);
if (strpos(ob_get_clean(), '2026-10-21 fix backported into PHP (heuristic) => detected') === false) {
    die('skip this PHP build does not carry a backport of the 2026-10-21 fix');
}
?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
var_dump(strpos($info, 'colopl_bc.security.detect_backported_fix => 1') !== false);
var_dump(strpos($info, 'memory error process abort => inactive (backported fix detected in the running PHP)') !== false);
var_dump(strpos($info, 'stale request state reset => inactive (backported fix detected in the running PHP)') !== false);
var_dump(strpos($info, 'session.upload_progress pointer hardening => active') === false);
// The backport leaves max_execution_time alone.
var_dump(strpos($info, 'execution timeout process abort => active (zend_error_cb wrapped') !== false);
// The directives keep their values; only the state changes.
var_dump(ini_get('colopl_bc.security.abort_on_memory_error'));
var_dump(ini_get('colopl_bc.security.reset_stale_request_state'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(1) "1"
string(1) "1"
