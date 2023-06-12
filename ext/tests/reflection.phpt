--TEST--
Check for colopl_bc functions reflection information.
--EXTENSIONS--
colopl_bc
--FILE--
<?php
foreach ((new ReflectionExtension('colopl_bc'))->getFunctions() as $function) { echo $function; }
--EXPECT--
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php70\srand ] {

  - Parameters [1] {
    Parameter #0 [ <optional> int $seed = 0 ]
  }
  - Return [ void ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php70\rand ] {

  - Parameters [2] {
    Parameter #0 [ <optional> int $seed = <default> ]
    Parameter #1 [ <optional> int $max = <default> ]
  }
  - Return [ int ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php70\getrandmax ] {

  - Parameters [0] {
  }
  - Return [ int ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php70\shuffle ] {

  - Parameters [1] {
    Parameter #0 [ <required> array &$array ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php70\str_shuffle ] {

  - Parameters [1] {
    Parameter #0 [ <required> string $string ]
  }
  - Return [ string ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php70\array_rand ] {

  - Parameters [2] {
    Parameter #0 [ <required> array $array ]
    Parameter #1 [ <optional> int $num = 1 ]
  }
  - Return [ array|string|int ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php70\mt_srand ] {

  - Parameters [1] {
    Parameter #0 [ <optional> int $seed = 0 ]
  }
  - Return [ void ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php70\mt_rand ] {

  - Parameters [2] {
    Parameter #0 [ <optional> int $min = <default> ]
    Parameter #1 [ <optional> int $max = <default> ]
  }
  - Return [ int ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php70\mt_getrandmax ] {

  - Parameters [0] {
  }
  - Return [ int ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php70\date_create ] {

  - Parameters [2] {
    Parameter #0 [ <optional> string $datetime = 'now' ]
    Parameter #1 [ <optional> ?DateTimeZone $timezone = null ]
  }
  - Return [ DateTime|false ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php70\date_create_immutable ] {

  - Parameters [2] {
    Parameter #0 [ <optional> string $datetime = 'now' ]
    Parameter #1 [ <optional> ?DateTimeZone $timezone = null ]
  }
  - Return [ DateTimeImmutable|false ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\ksort ] {

  - Parameters [2] {
    Parameter #0 [ <required> array &$array ]
    Parameter #1 [ <optional> int $flags = SORT_REGULAR ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\krsort ] {

  - Parameters [2] {
    Parameter #0 [ <required> array &$array ]
    Parameter #1 [ <optional> int $flags = SORT_REGULAR ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\asort ] {

  - Parameters [2] {
    Parameter #0 [ <required> array &$array ]
    Parameter #1 [ <optional> int $flags = SORT_REGULAR ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\arsort ] {

  - Parameters [2] {
    Parameter #0 [ <required> array &$array ]
    Parameter #1 [ <optional> int $flags = SORT_REGULAR ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\sort ] {

  - Parameters [2] {
    Parameter #0 [ <required> array &$array ]
    Parameter #1 [ <optional> int $flags = SORT_REGULAR ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\rsort ] {

  - Parameters [2] {
    Parameter #0 [ <required> array &$array ]
    Parameter #1 [ <optional> int $flags = SORT_REGULAR ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\usort ] {

  - Parameters [2] {
    Parameter #0 [ <required> array &$array ]
    Parameter #1 [ <required> callable $callback ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\uasort ] {

  - Parameters [2] {
    Parameter #0 [ <required> array &$array ]
    Parameter #1 [ <required> callable $callback ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\uksort ] {

  - Parameters [2] {
    Parameter #0 [ <required> array &$array ]
    Parameter #1 [ <required> callable $callback ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\in_array ] {

  - Parameters [3] {
    Parameter #0 [ <required> mixed $needle ]
    Parameter #1 [ <required> array $haystack ]
    Parameter #2 [ <optional> bool $strict = false ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\array_search ] {

  - Parameters [3] {
    Parameter #0 [ <required> mixed $needle ]
    Parameter #1 [ <required> array $haystack ]
    Parameter #2 [ <optional> bool $strict = false ]
  }
  - Return [ string|int|false ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\array_keys ] {

  - Parameters [3] {
    Parameter #0 [ <required> array $array ]
    Parameter #1 [ <optional> mixed $filter_value = <default> ]
    Parameter #2 [ <optional> bool $strict = false ]
  }
  - Return [ array ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\array_multisort ] {

  - Parameters [2] {
    Parameter #0 [ <required> &$array ]
    Parameter #1 [ <optional> &...$rest ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\eq ] {

  - Parameters [2] {
    Parameter #0 [ <required> mixed $op1 ]
    Parameter #1 [ <required> mixed $op2 ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\neq ] {

  - Parameters [2] {
    Parameter #0 [ <required> mixed $op1 ]
    Parameter #1 [ <required> mixed $op2 ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\lt ] {

  - Parameters [2] {
    Parameter #0 [ <required> mixed $op1 ]
    Parameter #1 [ <required> mixed $op2 ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\lte ] {

  - Parameters [2] {
    Parameter #0 [ <required> mixed $op1 ]
    Parameter #1 [ <required> mixed $op2 ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\gt ] {

  - Parameters [2] {
    Parameter #0 [ <required> mixed $op1 ]
    Parameter #1 [ <required> mixed $op2 ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\gte ] {

  - Parameters [2] {
    Parameter #0 [ <required> mixed $op1 ]
    Parameter #1 [ <required> mixed $op2 ]
  }
  - Return [ bool ]
}
Function [ <internal:colopl_bc> function Colopl\ColoplBc\Php74\spaceship ] {

  - Parameters [2] {
    Parameter #0 [ <required> mixed $op1 ]
    Parameter #1 [ <required> mixed $op2 ]
  }
  - Return [ int ]
}
