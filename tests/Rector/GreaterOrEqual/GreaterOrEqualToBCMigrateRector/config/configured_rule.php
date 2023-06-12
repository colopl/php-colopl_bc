<?php

declare(strict_types=1);

use Colopl\ColoplBc\Rector\GreaterOrEqual\GreaterOrEqualToBCMigrateRector;
use Rector\Config\RectorConfig;

return static function (RectorConfig $rectorConfig): void {
    $services = $rectorConfig->services();
    $services->set(GreaterOrEqualToBCMigrateRector::class);
};
