<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\ArrayFuncToBCMigrateRector\Fixture;

$arr = range(1, 10);

in_array(3, $arr);
array_search(3, $arr);
array_keys($arr);
?>
-----
<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\ArrayFuncToBCMigrateRector\Fixture;

$arr = range(1, 10);

\Colopl\ColoplBc\Php74\in_array(3, $arr);
\Colopl\ColoplBc\Php74\array_search(3, $arr);
\Colopl\ColoplBc\Php74\array_keys($arr);
?>
