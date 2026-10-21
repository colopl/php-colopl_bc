--TEST--
Check for GitHub Issue #30 (date_create() to SEGV on 8.4)
--EXTENSIONS--
colopl_bc
--INI--
date.timezone=UTC
--SKIPIF--
<?php if (!function_exists('Colopl\\ColoplBc\\Php74\\eq')) die('skip compatibility layer is not built on this PHP (security-only build)'); ?>
--FILE--
<?php

\Colopl\ColoplBc\Php70\date_create('bongo');
\Colopl\ColoplBc\Php70\date_create_immutable('bongo');

die('Success');

?>
--EXPECT--
Success
