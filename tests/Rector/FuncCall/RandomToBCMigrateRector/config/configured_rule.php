<?php

declare(strict_types=1);

use Colopl\ColoplBc\Rector\FuncCall\RandomToBCMigrateRector;
use Rector\Config\RectorConfig;

return static function (RectorConfig $rectorConfig): void {
    $services = $rectorConfig->services();
    $services->set(RandomToBCMigrateRector::class);
};
