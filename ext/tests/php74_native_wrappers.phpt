--TEST--
Check PHP 7.4 native wrapper behavior for \Colopl\ColoplBc\Php74 functions.
--EXTENSIONS--
colopl_bc
--SKIPIF--
<?php if (PHP_VERSION_ID >= 80000) die('skip PHP 7 native wrapper behavior only'); ?>
--FILE--
<?php
function show(string $label, $value): void
{
    echo $label, ': ';
    var_export($value);
    echo "\n";
}

$array = ['b' => 2, 'a' => 1];
show('ksort', \Colopl\ColoplBc\Php74\ksort($array));
show('ksort.array', $array);

$array = ['a' => 1, 'b' => 2];
show('krsort', \Colopl\ColoplBc\Php74\krsort($array));
show('krsort.array', $array);

$array = ['b' => 2, 'a' => 1];
show('asort', \Colopl\ColoplBc\Php74\asort($array));
show('asort.array', $array);

$array = ['a' => 1, 'b' => 2];
show('arsort', \Colopl\ColoplBc\Php74\arsort($array));
show('arsort.array', $array);

$array = [3, 1, 2];
show('sort', \Colopl\ColoplBc\Php74\sort($array));
show('sort.array', $array);

$array = [1, 3, 2];
show('rsort', \Colopl\ColoplBc\Php74\rsort($array));
show('rsort.array', $array);

$array = [3, 1, 2];
show('usort', \Colopl\ColoplBc\Php74\usort($array, static function ($a, $b): int { return $a <=> $b; }));
show('usort.array', $array);

$array = ['c' => 3, 'a' => 1, 'b' => 2];
show('uasort', \Colopl\ColoplBc\Php74\uasort($array, static function ($a, $b): int { return $a <=> $b; }));
show('uasort.array', $array);

$array = ['c' => 3, 'a' => 1, 'b' => 2];
show('uksort', \Colopl\ColoplBc\Php74\uksort($array, static function ($a, $b): int { return strcmp($a, $b); }));
show('uksort.array', $array);

show('in_array', \Colopl\ColoplBc\Php74\in_array('123', [123]));
show('array_search', \Colopl\ColoplBc\Php74\array_search('123', [123]));
show('array_keys', \Colopl\ColoplBc\Php74\array_keys([1 => 1, 'a' => '1'], 1));

$multi1 = [2, 1];
$multi2 = ['b', 'a'];
show('array_multisort', \Colopl\ColoplBc\Php74\array_multisort($multi1, $multi2));
show('array_multisort.array1', $multi1);
show('array_multisort.array2', $multi2);

show('eq', \Colopl\ColoplBc\Php74\eq('a', 0));
show('neq', \Colopl\ColoplBc\Php74\neq('a', 0));
show('lt', \Colopl\ColoplBc\Php74\lt('a', 1));
show('lte', \Colopl\ColoplBc\Php74\lte('a', 1));
show('gt', \Colopl\ColoplBc\Php74\gt(2, 'a'));
show('gte', \Colopl\ColoplBc\Php74\gte(2, 'a'));
show('spaceship', \Colopl\ColoplBc\Php74\spaceship('a', 1));
?>
--EXPECT--
ksort: true
ksort.array: array (
  'a' => 1,
  'b' => 2,
)
krsort: true
krsort.array: array (
  'b' => 2,
  'a' => 1,
)
asort: true
asort.array: array (
  'a' => 1,
  'b' => 2,
)
arsort: true
arsort.array: array (
  'b' => 2,
  'a' => 1,
)
sort: true
sort.array: array (
  0 => 1,
  1 => 2,
  2 => 3,
)
rsort: true
rsort.array: array (
  0 => 3,
  1 => 2,
  2 => 1,
)
usort: true
usort.array: array (
  0 => 1,
  1 => 2,
  2 => 3,
)
uasort: true
uasort.array: array (
  'a' => 1,
  'b' => 2,
  'c' => 3,
)
uksort: true
uksort.array: array (
  'a' => 1,
  'b' => 2,
  'c' => 3,
)
in_array: true
array_search: 0
array_keys: array (
  0 => 1,
  1 => 'a',
)
array_multisort: true
array_multisort.array1: array (
  0 => 1,
  1 => 2,
)
array_multisort.array2: array (
  0 => 'a',
  1 => 'b',
)
eq: true
neq: false
lt: true
lte: true
gt: true
gte: true
spaceship: -1
