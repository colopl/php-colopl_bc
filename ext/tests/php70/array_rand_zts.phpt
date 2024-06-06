--TEST--
Check for \Colopl\ColoplBc\Php70\array_rand() function compatibility. (ZTS)
--EXTENSIONS--
colopl_bc
--INI--
date.timezone=UTC
--SKIPIF--
<?php
if (PHP_ZTS === 0) print 'skip NTS not supported';
?>
--FILE--
<?php
const SEED = 1234;
\Colopl\ColoplBc\Php70\srand(\SEED);
var_dump(\Colopl\ColoplBc\Php70\array_rand([1, 2, 3, 4, 5], 3));
?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(3)
  [2]=>
  int(4)
}
