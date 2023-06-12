<?php

namespace Colopl\ColoplBc\Tests\Rector\New_\NewAPCIteratorToAPCUIteratorRector\Fixture;

new \APCIterator('user');
new \APCIterator('user', null, APC_ITER_ALL, 100, APC_LIST_ACTIVE);

?>
-----
<?php

namespace Colopl\ColoplBc\Tests\Rector\New_\NewAPCIteratorToAPCUIteratorRector\Fixture;

new \APCUIterator();
new \APCUIterator(null, APC_ITER_ALL, 100, APC_LIST_ACTIVE);

?>
