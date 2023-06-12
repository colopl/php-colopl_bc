<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Tests\Rector\Switch_\SwitchCaseToEqualRector;

use Colopl\ColoplBc\Tests\Rector\AbstractSimpleRectorTestCase;

final class SwitchCaseToEqualRectorTest extends AbstractSimpleRectorTestCase
{
    /**
     * @return non-empty-string
     */
    protected function getDir(): string
    {
        return __DIR__;
    }
}
