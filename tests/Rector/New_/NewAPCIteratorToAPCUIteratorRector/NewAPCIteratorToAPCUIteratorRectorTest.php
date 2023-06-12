<?php

declare(strict_types=1);

namespace Colopl\Rector\Tests\Php80\Rector\New_\NewAPCIteratorToAPCUIteratorRector;

use Colopl\ColoplBc\Tests\Rector\AbstractSimpleRectorTestCase;

final class NewAPCIteratorToAPCUIteratorRectorTest extends AbstractSimpleRectorTestCase
{
    /**
     * @return non-empty-string
     */
    protected function getDir(): string
    {
        return __DIR__;
    }
}
