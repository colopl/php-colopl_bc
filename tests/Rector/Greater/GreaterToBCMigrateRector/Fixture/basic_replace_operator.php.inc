<?php

namespace Colopl\ColoplBc\Tests\Rector\Greater\GreaterToBCMigrateRector\Fixture;

0 > '';
0 >= '';

?>
-----
<?php

namespace Colopl\ColoplBc\Tests\Rector\Greater\GreaterToBCMigrateRector\Fixture;

\Colopl\ColoplBc\Php74\gt(0, '');
0 >= '';

?>
