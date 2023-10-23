<?php

declare(strict_types=1);

use Colopl\ColoplBc\Rector\FuncCall\ArrayMultisortToBCMigrateRector;
use Rector\Config\RectorConfig;

return static function (RectorConfig $rectorConfig): void {
    $rectorConfig->rule(ArrayMultisortToBCMigrateRector::class);
};
