<?php

declare(strict_types=1);

use Colopl\ColoplBc\Rector\Smaller\SmallerToBCMigrateRector;
use Rector\Config\RectorConfig;

return static function (RectorConfig $rectorConfig): void {
    $rectorConfig->rule(SmallerToBCMigrateRector::class);
};
