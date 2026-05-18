--TEST--
Check compare diagnostics do not apply native comparisons to the original object twice
--EXTENSIONS--
colopl_bc
--INI--
date.timezone=UTC
error_log=
colopl_bc.php74.compare_mode=3
--FILE--
<?php
class CounterString
{
    public static int $globalCasts = 0;
    public static int $clones = 0;
    public int $casts = 0;

    public function __construct(private string $value)
    {
    }

    public function __toString(): string
    {
        ++self::$globalCasts;
        ++$this->casts;
        return $this->value;
    }

    public function __clone()
    {
        ++self::$clones;
    }
}

$cases = [
    'eq-left' => static fn (CounterString $object): bool => \Colopl\ColoplBc\Php74\eq($object, '1'),
    'eq-right' => static fn (CounterString $object): bool => \Colopl\ColoplBc\Php74\eq('1', $object),
    'neq' => static fn (CounterString $object): bool => \Colopl\ColoplBc\Php74\neq($object, '1'),
    'lt' => static fn (CounterString $object): bool => \Colopl\ColoplBc\Php74\lt($object, '2'),
    'lte' => static fn (CounterString $object): bool => \Colopl\ColoplBc\Php74\lte($object, '1'),
    'gt' => static fn (CounterString $object): bool => \Colopl\ColoplBc\Php74\gt($object, '0'),
    'gte' => static fn (CounterString $object): bool => \Colopl\ColoplBc\Php74\gte($object, '1'),
    'spaceship' => static fn (CounterString $object): int => \Colopl\ColoplBc\Php74\spaceship($object, '1'),
];

foreach ($cases as $name => $case) {
    $object = new CounterString('1');
    var_dump($name, $case($object), $object->casts);
}
var_dump(CounterString::$globalCasts);
var_dump(CounterString::$clones);
?>
--EXPECT--
string(7) "eq-left"
bool(true)
int(1)
string(8) "eq-right"
bool(true)
int(1)
string(3) "neq"
bool(false)
int(1)
string(2) "lt"
bool(true)
int(1)
string(3) "lte"
bool(true)
int(1)
string(2) "gt"
bool(true)
int(1)
string(3) "gte"
bool(true)
int(1)
string(9) "spaceship"
int(0)
int(1)
int(8)
int(0)
