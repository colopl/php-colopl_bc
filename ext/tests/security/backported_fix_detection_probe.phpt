--TEST--
security: the backported fix detection agrees with how the running PHP handles a memory error
--EXTENSIONS--
colopl_bc
--SKIPIF--
<?php
if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded');
include_once dirname(__FILE__) . '/probe_memory_error.inc';
if (colopl_bc_probe_memory_error() === null) die('skip the memory error probe cannot tell how this PHP build handles a memory error');
?>
--FILE--
<?php
// The detection is a heuristic (an exported symbol); the probe observes the
// behaviour it stands for: an unversioned backport of the 2026-10-21 fix ends
// the process on a memory error instead of bailing out.
include_once dirname(__FILE__) . '/probe_memory_error.inc';
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
$detected = strpos($info, '2026-10-21 fix backported into PHP (heuristic) => detected') !== false;
$exits = colopl_bc_probe_memory_error() === 'exit';
var_dump($detected === $exits);
if ($detected !== $exits) {
    echo 'detected: ', var_export($detected, true), ', PHP ends the process itself: ', var_export($exits, true), "\n";
}
?>
--EXPECT--
bool(true)
