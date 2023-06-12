<?php

namespace Colopl\Rector\Tests\Php80\Rector\Switch_\SwitchCaseToEqualRector\SwitchCaseToEqualRector\Fixture;

$foo = '0';

switch ($foo) {
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

switch ('immediate') {
    default:
        echo 'pongo';
        break;
    case 'bongo':
        echo 'bongo';
        break;
    case 'conga':
        echo 'conga';
        break;
}

switch ($foo) {
    default:
        echo 'do not change this';
}

switch (true) {
    case 0:
        echo 'do not change this';
        break;
    case '0':
        echo 'do not change this';
        break;
    default:
        echo 'do not change this';
}

?>
-----
<?php

namespace Colopl\Rector\Tests\Php80\Rector\Switch_\SwitchCaseToEqualRector\SwitchCaseToEqualRector\Fixture;

$foo = '0';

switch (true) {
    case '0' == $foo:
        echo 'string zero';
        break;
    case 0 == $foo:
        echo 'integer zero';
        break;
    default:
        echo "default";
        break;
}

switch (true) {
    default:
        echo 'pongo';
        break;
    case 'bongo' == 'immediate':
        echo 'bongo';
        break;
    case 'conga' == 'immediate':
        echo 'conga';
        break;
}

switch ($foo) {
    default:
        echo 'do not change this';
}

switch (true) {
    case 0:
        echo 'do not change this';
        break;
    case '0':
        echo 'do not change this';
        break;
    default:
        echo 'do not change this';
}

?>
