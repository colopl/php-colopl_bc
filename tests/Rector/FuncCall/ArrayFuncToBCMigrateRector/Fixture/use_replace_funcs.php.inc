<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\ArrayFuncToBCMigrateRector\Fixture;

use function in_array as foo, array_search as bar, array_keys as baz;

$arr = range(1, 10);

foo(3, $arr);
bar(3, $arr);
baz($arr);
?>
-----
<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\ArrayFuncToBCMigrateRector\Fixture;

use function in_array as foo, array_search as bar, array_keys as baz;

$arr = range(1, 10);

\Colopl\ColoplBc\Php74\in_array(3, $arr);
\Colopl\ColoplBc\Php74\array_search(3, $arr);
\Colopl\ColoplBc\Php74\array_keys($arr);
?>
