<?php

declare(strict_types=1);

use Colopl\ColoplBc\Rector\FuncCall\ArrayKeyExistsToPropertyExistsRector;
use Rector\Config\RectorConfig;

return static function (RectorConfig $rectorConfig): void {
    $rectorConfig->rule(ArrayKeyExistsToPropertyExistsRector::class);
};
