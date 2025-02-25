--TEST--
Check for GitHub Issue #30 (date_create() to SEGV on 8.4)
--EXTENSIONS--
colopl_bc
--INI--
date.timezone=UTC
--FILE--
<?php

\Colopl\ColoplBc\Php70\date_create('bongo');
\Colopl\ColoplBc\Php70\date_create_immutable('bongo');

die('Success');

?>
--EXPECT--
Success
