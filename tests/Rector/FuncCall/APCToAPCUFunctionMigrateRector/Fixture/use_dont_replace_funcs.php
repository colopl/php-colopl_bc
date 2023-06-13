<?php

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\APCToAPCUFunctionMigrateRector\Fixture;

use function apc_sma_info as a, apc_store as b, apc_fetch as c, apc_delete as d, apc_add as e,
    apc_exists as f, apc_inc as g, apc_dec as h, apc_cas as i, apc_cache_info as j, apc_clear_cache as k,
    strlen as l, apc_bin_dump as m;

a();
b('foobar');
c('bongo');
d('conga');
e('barbaz', 'hogefuga');
f('barbaz');
g('incr');
h('incr');
i('cas', 1, 10);

j('user', true);
j();

k('user');
k();

l();
m();

?>
