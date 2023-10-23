<?php

declare(strict_types=1);

use Colopl\ColoplBc\Rector\Switch_\SwitchCaseToEqualRector;
use Rector\Config\RectorConfig;

return static function (RectorConfig $rectorConfig): void {
    $rectorConfig->rule(SwitchCaseToEqualRector::class);
};
