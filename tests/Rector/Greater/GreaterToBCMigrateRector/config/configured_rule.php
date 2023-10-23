<?php

declare(strict_types=1);

use Colopl\ColoplBc\Rector\Greater\GreaterToBCMigrateRector;
use Rector\Config\RectorConfig;

return static function (RectorConfig $rectorConfig): void {
    $rectorConfig->rule(GreaterToBCMigrateRector::class);
};
