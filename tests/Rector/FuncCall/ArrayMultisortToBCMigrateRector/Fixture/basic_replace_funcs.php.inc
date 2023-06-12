<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\ArrayMultisortToBCMigrateRector\Fixture;

$foo = range(1, 10);
$bar = range(10, 20);

array_multisort($foo, SORT_NUMERIC, SORT_ASC, $bar, SORT_ASC);

?>
-----
<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\ArrayMultisortToBCMigrateRector\Fixture;

$foo = range(1, 10);
$bar = range(10, 20);

PHP_VERSION_ID >= 80000 ? \Colopl\ColoplBc\Php74\array_multisort($foo, SORT_NUMERIC, SORT_ASC, $bar, SORT_ASC) : array_multisort($foo, SORT_NUMERIC, SORT_ASC, $bar, SORT_ASC);

?>
