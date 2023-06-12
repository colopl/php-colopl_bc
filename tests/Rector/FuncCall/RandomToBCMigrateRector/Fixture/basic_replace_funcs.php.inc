<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\RandomToBCMigrateRector\Fixture;

$arr = range(1, 10);

srand();
rand();
getrandmax();
shuffle($arr);
str_shuffle('foobar');
array_rand($arr);
?>
-----
<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\RandomToBCMigrateRector\Fixture;

$arr = range(1, 10);

\Colopl\ColoplBc\Php70\srand();
\Colopl\ColoplBc\Php70\rand();
\Colopl\ColoplBc\Php70\getrandmax();
\Colopl\ColoplBc\Php70\shuffle($arr);
\Colopl\ColoplBc\Php70\str_shuffle('foobar');
\Colopl\ColoplBc\Php70\array_rand($arr);
?>
