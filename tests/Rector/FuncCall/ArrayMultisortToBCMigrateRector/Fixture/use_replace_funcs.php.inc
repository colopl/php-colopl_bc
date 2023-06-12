<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\ArrayMultisortToBCMigrateRector\Fixture;

use function array_multisort as bongo;

$foo = range(1, 10);
$bar = range(10, 20);

bongo($foo, SORT_NUMERIC, SORT_ASC, $bar, SORT_ASC);

?>
-----
<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\ArrayMultisortToBCMigrateRector\Fixture;

use function array_multisort as bongo;

$foo = range(1, 10);
$bar = range(10, 20);

PHP_VERSION_ID >= 80000 ? \Colopl\ColoplBc\Php74\array_multisort($foo, SORT_NUMERIC, SORT_ASC, $bar, SORT_ASC) : bongo($foo, SORT_NUMERIC, SORT_ASC, $bar, SORT_ASC);

?>
