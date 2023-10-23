<?php

declare(strict_types=1);

use Colopl\ColoplBc\Rector\New_\NewAPCIteratorToAPCUIteratorRector;
use Rector\Config\RectorConfig;

return static function (RectorConfig $rectorConfig): void {
    $rectorConfig->rule(NewAPCIteratorToAPCUIteratorRector::class);
};
