--TEST--
Check for \Colopl\ColoplBc\Php70\array_rand() function compatibility.
--EXTENSIONS--
colopl_bc
--INI--
date.timezone=UTC
--SKIPIF--
<?php
if ((bool) PHP_ZTS === true) print 'skip ZTS not supported';
if (PHP_OS_FAMILY === 'Windows') print 'skip Windows not supported';
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
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(3)
}
