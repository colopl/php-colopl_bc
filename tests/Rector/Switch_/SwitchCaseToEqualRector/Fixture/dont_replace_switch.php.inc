<?php

namespace Colopl\Rector\Tests\Php80\Rector\Switch_\SwitchCaseToEqualRector\SwitchCaseToEqualRector\Fixture;

switch (true) {
    case '0':
        echo 'string zero';
        break;
    case 0:
        echo 'integer zero';
        break;
    default:
        echo "default";
        break;
}

?>
-----
<?php

namespace Colopl\Rector\Tests\Php80\Rector\Switch_\SwitchCaseToEqualRector\SwitchCaseToEqualRector\Fixture;

switch (true) {
    case '0':
        echo 'string zero';
        break;
    case 0:
        echo 'integer zero';
        break;
    default:
        echo "default";
        break;
}

?>
