<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\ArrayKeyExistsToPropertyExistsRector\Fixture;

$foo = (object)['foo' => 'bar']; // stdClass
$bar = new \ArrayObject((object)['foo' => 'bar']); // ArrayObject
$baz = ['foo' => 'bar']; // array

\array_key_exists('foo', $foo);
\array_key_exists('foo', $bar);
\array_key_exists('foo', $baz);
\array_key_exists('foo'); // invalid args
\array_key_exists('foo', (static function () {
    $val = ['foo' => 'bar'];
    if (\random_int(1, 10) > 5) {
        return (object) $val;
    }
    return $val;
})());

$bongo = eval("['foo' => 'bar'];");
$conga = eval("(object)['foo' => 'bar'];");
\array_key_exists('foo', $bongo);
\array_key_exists('foo', $conga);

?>
-----
<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\ArrayKeyExistsToPropertyExistsRector\Fixture;

$foo = (object)['foo' => 'bar']; // stdClass
$bar = new \ArrayObject((object)['foo' => 'bar']); // ArrayObject
$baz = ['foo' => 'bar']; // array

\property_exists($foo, 'foo');
\property_exists($bar, 'foo');
\array_key_exists('foo', $baz);
\array_key_exists('foo'); // invalid args
\array_key_exists('foo', (static function () {
    $val = ['foo' => 'bar'];
    if (\random_int(1, 10) > 5) {
        return (object) $val;
    }
    return $val;
})());

$bongo = eval("['foo' => 'bar'];");
$conga = eval("(object)['foo' => 'bar'];");
\array_key_exists('foo', $bongo);
\array_key_exists('foo', $conga);

?>
