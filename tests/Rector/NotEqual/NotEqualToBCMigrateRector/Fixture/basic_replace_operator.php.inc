<?php

namespace Colopl\Rector\Tests\Php80\Rector\NotEqual\NotEqualToBCMigrateRector\Fixture;

0 != '';
0 !== '';

?>
-----
<?php

namespace Colopl\Rector\Tests\Php80\Rector\NotEqual\NotEqualToBCMigrateRector\Fixture;

\Colopl\ColoplBc\Php74\neq(0, '');
0 !== '';

?>
