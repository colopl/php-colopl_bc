<?php
/*
 * Some PHP builds already end the process on a memory error instead of
 * bailing out (see probe_memory_error.inc). Skip only when PHP itself ended
 * the process; any other probe outcome lets the test run.
 */
include_once dirname(__FILE__) . '/probe_memory_error.inc';

if (colopl_bc_probe_memory_error() === 'exit') {
    die('skip this PHP build already ends the process on a memory error (backport of the 2026-10-21 fix)');
}
