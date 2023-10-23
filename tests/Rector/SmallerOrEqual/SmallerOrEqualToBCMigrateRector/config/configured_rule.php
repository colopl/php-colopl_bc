<?php

declare(strict_types=1);

use Colopl\ColoplBc\Rector\SmallerOrEqual\SmallerOrEqualToBCMigrateRector;
use Rector\Config\RectorConfig;

return static function (RectorConfig $rectorConfig): void {
    $rectorConfig->rule(SmallerOrEqualToBCMigrateRector::class);
};
