<?php

declare(strict_types=1);

use Colopl\ColoplBc\Rector\Spaceship\SpaceshipToBCMigrateRector;
use Rector\Config\RectorConfig;

return static function (RectorConfig $rectorConfig): void {
    $rectorConfig->rule(SpaceshipToBCMigrateRector::class);
};
