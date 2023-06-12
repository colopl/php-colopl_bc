<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\APCToAPCUFunctionMigrateRector\Fixture;

apc_sma_info();
apc_store('foobar');
apc_fetch('bongo');
apc_delete('conga');
apc_add('barbaz', 'hogefuga');
apc_exists('barbaz');
apc_inc('incr');
apc_dec('incr');
apc_cas('cas', 1, 10);

apc_cache_info('user', true);
apc_cache_info();

apc_clear_cache('user');
apc_clear_cache();

strlen('foobar');

?>
-----
<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\APCToAPCUFunctionMigrateRector\Fixture;

apcu_sma_info();
apcu_store('foobar');
apcu_fetch('bongo');
apcu_delete('conga');
apcu_add('barbaz', 'hogefuga');
apcu_exists('barbaz');
apcu_inc('incr');
apcu_dec('incr');
apcu_cas('cas', 1, 10);

apcu_cache_info(true);
apcu_cache_info();

apcu_clear_cache();
apcu_clear_cache();

strlen('foobar');

?>
