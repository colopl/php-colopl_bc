<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\SortToBCMigrateRector\Fixture;

$arr = range(1, 10);

ksort($arr);
krsort($arr);
asort($arr);
arsort($arr);
sort($arr);
rsort($arr);
usort($arr, function (int $left, int $right): int { return 1; });
uasort($arr, function (int $left, int $right): int { return 1; });
uksort($arr, function (int $left, int $right): int { return 1; });
?>
-----
<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\SortToBCMigrateRector\Fixture;

$arr = range(1, 10);

\Colopl\ColoplBc\Php74\ksort($arr);
\Colopl\ColoplBc\Php74\krsort($arr);
\Colopl\ColoplBc\Php74\asort($arr);
\Colopl\ColoplBc\Php74\arsort($arr);
\Colopl\ColoplBc\Php74\sort($arr);
\Colopl\ColoplBc\Php74\rsort($arr);
\Colopl\ColoplBc\Php74\usort($arr, function (int $left, int $right): int { return 1; });
\Colopl\ColoplBc\Php74\uasort($arr, function (int $left, int $right): int { return 1; });
\Colopl\ColoplBc\Php74\uksort($arr, function (int $left, int $right): int { return 1; });
?>
