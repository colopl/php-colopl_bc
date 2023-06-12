<?php

namespace Colopl\Rector\Tests\Php80\Rector\Equal\EqualToBCMigrateRector\Fixture;

0 == '';
0 === '';

?>
-----
<?php

namespace Colopl\Rector\Tests\Php80\Rector\Equal\EqualToBCMigrateRector\Fixture;

\Colopl\ColoplBc\Php74\eq(0, '');
0 === '';

?>
