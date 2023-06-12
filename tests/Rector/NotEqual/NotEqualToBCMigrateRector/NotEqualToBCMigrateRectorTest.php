<?php

declare(strict_types=1);

namespace Colopl\Rector\Tests\Php80\Rector\NotEqual\NotEqualToBCMigrateRector;

use Colopl\ColoplBc\Tests\Rector\AbstractSimpleRectorTestCase;

final class NotEqualToBCMigrateRectorTest extends AbstractSimpleRectorTestCase
{
    /**
     * @return non-empty-string
     */
    protected function getDir(): string
    {
        return __DIR__;
    }
}
