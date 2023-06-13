<?php

namespace Colopl\ColoplBc\Tests\Rector\SmallerOrEqual\SmallerOrEqualToBCMigrateRector\Fixture;

0 < '';
0 <= '';

?>
-----
<?php

namespace Colopl\ColoplBc\Tests\Rector\SmallerOrEqual\SmallerOrEqualToBCMigrateRector\Fixture;

0 < '';
\Colopl\ColoplBc\Php74\lte(0, '');

?>
