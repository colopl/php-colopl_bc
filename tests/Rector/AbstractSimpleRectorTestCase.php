<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Tests\Rector;

use Iterator;
use Rector\Testing\PHPUnit\AbstractRectorTestCase;

use const DIRECTORY_SEPARATOR as DS;

abstract class AbstractSimpleRectorTestCase extends AbstractRectorTestCase
{
    /**
     * @dataProvider provideData()
     */
    public function test(string $file): void
    {
        $this->doTestFile($file);
    }

    public function provideData(): Iterator
    {
        return static::yieldFilesFromDirectory($this->getDir() . DS . 'Fixture');
    }


    public function provideConfigFilePath(): string
    {
        return $this->getDir() . DS . 'config' . DS . 'configured_rule.php';
    }

    abstract protected function getDir(): string;
}
