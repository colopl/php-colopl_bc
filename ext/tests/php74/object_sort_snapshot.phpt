--TEST--
Check sort diagnostics do not apply native sort comparisons to the original objects
--EXTENSIONS--
colopl_bc
--INI--
date.timezone=UTC
error_log=
colopl_bc.php74.compare_mode=0
colopl_bc.php74.sort_mode=3
--FILE--
<?php
class SortString
{
    public static int $globalCasts = 0;
    public int $casts = 0;

    public function __construct(public string $value)
    {
    }

    public function __toString(): string
    {
        ++self::$globalCasts;
        ++$this->casts;
        return $this->value;
    }
}

$array = [new SortString('2'), new SortString('1')];
\Colopl\ColoplBc\Php74\sort($array, SORT_STRING);

foreach ($array as $object) {
    echo $object->value, ':', $object->casts, "\n";
}
var_dump(SortString::$globalCasts);
?>
--EXPECT--
1:1
2:1
int(2)
