<?php

namespace Colopl\Rector\Tests\Php80\Rector\SmallerOrEqual\SmallerOrEqualToBCMigrateRector\Fixture;

0 < '';
0 <= '';

?>
-----
<?php

namespace Colopl\Rector\Tests\Php80\Rector\SmallerOrEqual\SmallerOrEqualToBCMigrateRector\Fixture;

0 < '';
\Colopl\ColoplBc\Php74\lte(0, '');

?>
