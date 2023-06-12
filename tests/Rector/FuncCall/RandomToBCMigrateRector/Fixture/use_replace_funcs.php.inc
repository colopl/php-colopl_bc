<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\RandomToBCMigrateRector\Fixture;

use function srand as foo, rand as bar, getrandmax as baz, shuffle as hoge, str_shuffle as fuga, array_rand as moge;

$arr = range(1, 10);

foo();
bar();
baz();
hoge($arr);
fuga('foobar');
moge($arr);
?>
-----
<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\RandomToBCMigrateRector\Fixture;

use function srand as foo, rand as bar, getrandmax as baz, shuffle as hoge, str_shuffle as fuga, array_rand as moge;

$arr = range(1, 10);

\Colopl\ColoplBc\Php70\srand();
\Colopl\ColoplBc\Php70\rand();
\Colopl\ColoplBc\Php70\getrandmax();
\Colopl\ColoplBc\Php70\shuffle($arr);
\Colopl\ColoplBc\Php70\str_shuffle('foobar');
\Colopl\ColoplBc\Php70\array_rand($arr);
?>
