<?php

namespace Colopl\ColoplBc\Tests\Rector\GreaterOrEqual\GreaterOrEqualToBCMigrateRector\Fixture;

0 > '';
0 >= '';

?>
-----
<?php

namespace Colopl\ColoplBc\Tests\Rector\GreaterOrEqual\GreaterOrEqualToBCMigrateRector\Fixture;

0 > '';
\Colopl\ColoplBc\Php74\gte(0, '');

?>
