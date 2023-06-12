<?php

declare(strict_types=1);

use Colopl\ColoplBc\Rector\FuncCall\ArrayFuncToBCMigrateRector;
use Rector\Config\RectorConfig;

return static function (RectorConfig $rectorConfig): void {
    $services = $rectorConfig->services();
    $services->set(ArrayFuncToBCMigrateRector::class);
};
