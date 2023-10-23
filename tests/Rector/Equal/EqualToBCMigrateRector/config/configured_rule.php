<?php

declare(strict_types=1);

use Colopl\ColoplBc\Rector\Equal\EqualToBCMigrateRector;
use Rector\Config\RectorConfig;

return static function (RectorConfig $rectorConfig): void {
    $rectorConfig->rule(EqualToBCMigrateRector::class);
};
