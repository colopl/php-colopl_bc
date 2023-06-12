<?php

namespace Colopl\Rector\Tests\Php80\Rector\GreaterOrEqual\GreaterOrEqualToBCMigrateRector\Fixture;

0 > '';
0 >= '';

?>
-----
<?php

namespace Colopl\Rector\Tests\Php80\Rector\GreaterOrEqual\GreaterOrEqualToBCMigrateRector\Fixture;

0 > '';
\Colopl\ColoplBc\Php74\gte(0, '');

?>
