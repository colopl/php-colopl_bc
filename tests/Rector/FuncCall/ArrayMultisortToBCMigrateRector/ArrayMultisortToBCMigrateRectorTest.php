<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Tests\Rector\FuncCall\ArrayMultisortToBCMigrateRector;

use Colopl\ColoplBc\Tests\Rector\AbstractSimpleRectorTestCase;

final class ArrayMultisortToBCMigrateRectorTest extends AbstractSimpleRectorTestCase
{
    /**
     * @return non-empty-string
     */
    protected function getDir(): string
    {
        return __DIR__;
    }
}
