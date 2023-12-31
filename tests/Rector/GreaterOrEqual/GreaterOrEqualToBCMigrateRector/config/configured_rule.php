<?php

declare(strict_types=1);

use Colopl\ColoplBc\Rector\GreaterOrEqual\GreaterOrEqualToBCMigrateRector;
use Rector\Config\RectorConfig;

return static function (RectorConfig $rectorConfig): void {
    $rectorConfig->rule(GreaterOrEqualToBCMigrateRector::class);
};
