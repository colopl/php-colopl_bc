<?php
/*
 * The stopgaps that a PHP build with a backport of the 2026-10-21 fix does not
 * need are not applied there (colopl_bc.security.detect_backported_fix).
 */
if (!extension_loaded('colopl_bc')) {
    die('skip colopl_bc extension is not loaded');
}

ob_start();
phpinfo(INFO_MODULES);
$colopl_bc_info = ob_get_clean();
if (strpos($colopl_bc_info, '2026-10-21 fix backported into PHP (heuristic) => detected') !== false) {
    die('skip this PHP build carries a backport of the 2026-10-21 fix');
}
