<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Tests;

use PHPUnit\Framework\TestCase;

abstract class AbstractTestCase extends TestCase
{
    protected $backupStaticAttributes = false;
    protected $runTestInSeparateProcess = false;
}
