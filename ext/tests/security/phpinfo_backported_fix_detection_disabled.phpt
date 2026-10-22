--TEST--
security: with colopl_bc.security.detect_backported_fix=0 the backport is not looked for and the stopgaps are applied as on an unfixed build
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.detect_backported_fix=0
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); ?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
var_dump(strpos($info, '2026-10-21 fix backported into PHP (heuristic) => not checked (disabled by INI)') !== false);
var_dump(strpos($info, 'colopl_bc.security.detect_backported_fix => 0') !== false);
var_dump(strpos($info, 'memory error process abort => active (zend_error_cb wrapped') !== false);
var_dump(strpos($info, 'stale request state reset => active (post-RSHUTDOWN hook, sapi deactivate wrapped') !== false);
var_dump(strpos($info, 'backported fix detected') === false);
var_dump(ini_set('colopl_bc.security.detect_backported_fix', '1'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
