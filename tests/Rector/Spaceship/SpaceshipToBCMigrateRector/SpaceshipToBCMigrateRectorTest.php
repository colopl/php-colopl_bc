<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Tests\Rector\Spaceship\SpaceshipToBCMigrateRector;

use Colopl\ColoplBc\Tests\Rector\AbstractSimpleRectorTestCase;

final class SpaceshipToBCMigrateRectorTest extends AbstractSimpleRectorTestCase
{
    /**
     * @return non-empty-string
     */
    protected function getDir(): string
    {
        return __DIR__;
    }
}
