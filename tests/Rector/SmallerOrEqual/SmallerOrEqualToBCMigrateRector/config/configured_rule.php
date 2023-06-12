<?php

declare(strict_types=1);

use Colopl\ColoplBc\Rector\SmallerOrEqual\SmallerOrEqualToBCMigrateRector;
use Rector\Config\RectorConfig;

return static function (RectorConfig $rectorConfig): void {
    $services = $rectorConfig->services();
    $services->set(SmallerOrEqualToBCMigrateRector::class);
};
