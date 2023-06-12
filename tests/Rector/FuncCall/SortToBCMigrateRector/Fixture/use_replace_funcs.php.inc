<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\SortToBCMigrateRector\Fixture;

use function ksort as a, krsort as b, asort as c, arsort as d,
    sort as e, rsort as f, usort as g, uasort as h, uksort as i;

$arr = range(1, 10);

a($arr);
b($arr);
c($arr);
d($arr);
e($arr);
f($arr);
g($arr, function (int $left, int $right): int { return 1; });
h($arr, function (int $left, int $right): int { return 1; });
i($arr, function (int $left, int $right): int { return 1; });
?>
-----
<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\SortToBCMigrateRector\Fixture;

use function ksort as a, krsort as b, asort as c, arsort as d,
    sort as e, rsort as f, usort as g, uasort as h, uksort as i;

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
