<?php

namespace Colopl\ColoplBc\Tests\Rector\New_\NewAPCIteratorToAPCUIteratorRector\Fixture;

use APCIterator as Bongo;

new Bongo('user');
new Bongo('user', null, APC_ITER_ALL, 100, APC_LIST_ACTIVE);

?>
-----
<?php

namespace Colopl\ColoplBc\Tests\Rector\New_\NewAPCIteratorToAPCUIteratorRector\Fixture;

use APCIterator as Bongo;

new \APCUIterator();
new \APCUIterator(null, APC_ITER_ALL, 100, APC_LIST_ACTIVE);

?>
