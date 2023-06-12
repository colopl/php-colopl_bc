<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Tests\Rector\GreaterOrEqual\GreaterOrEqualToBCMigrateRector;

use Colopl\ColoplBc\Tests\Rector\AbstractSimpleRectorTestCase;

final class GreaterOrEqualToBCMigrateRectorTest extends AbstractSimpleRectorTestCase
{
    /**
     * @return non-empty-string
     */
    protected function getDir(): string
    {
        return __DIR__;
    }
}
