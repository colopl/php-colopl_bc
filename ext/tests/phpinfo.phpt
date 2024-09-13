--TEST--
Check phpinfo()
--EXTENSIONS--
colopl_bc
--FILE--
<?php
ob_start(
    static fn (string $phpinfo): string
      => str_contains($phpinfo, 'colopl_bc support')
        ? ('Success: ' . \phpversion('colopl_bc'))
        : 'Failure'
);
phpinfo();
ob_end_flush();
?>
--EXPECTF--
Success: %s
