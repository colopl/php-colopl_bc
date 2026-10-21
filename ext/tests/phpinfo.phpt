--TEST--
Check phpinfo()
--EXTENSIONS--
colopl_bc
--INI--
date.timezone=UTC
--SKIPIF--
<?php if (!function_exists('Colopl\\ColoplBc\\Php74\\eq')) die('skip compatibility layer is not built on this PHP (security-only build)'); ?>
--FILE--
<?php
ob_start(
    static fn (string $phpinfo): string
      => strpos($phpinfo, 'colopl_bc support') !== false
        ? ('Success: ' . \phpversion('colopl_bc'))
        : 'Failure'
);
phpinfo();
ob_end_flush();
?>
--EXPECTF--
Success: %s
