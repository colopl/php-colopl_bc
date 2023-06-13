<?php

namespace Colopl\ColoplBc\Tests\Rector\Equal\EqualToBCMigrateRector\Fixture;

0 == '';
0 === '';

?>
-----
<?php

namespace Colopl\ColoplBc\Tests\Rector\Equal\EqualToBCMigrateRector\Fixture;

\Colopl\ColoplBc\Php74\eq(0, '');
0 === '';

?>
