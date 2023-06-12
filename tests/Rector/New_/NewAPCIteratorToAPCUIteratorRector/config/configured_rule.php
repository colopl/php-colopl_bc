<?php

declare(strict_types=1);

use Colopl\ColoplBc\Rector\New_\NewAPCIteratorToAPCUIteratorRector;
use Rector\Config\RectorConfig;

return static function (RectorConfig $rectorConfig): void {
    $services = $rectorConfig->services();
    $services->set(NewAPCIteratorToAPCUIteratorRector::class);
};
