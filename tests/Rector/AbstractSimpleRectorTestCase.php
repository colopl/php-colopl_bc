<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Tests\Rector;

use Iterator;
use Rector\Exception\ShouldNotHappenException;
use Rector\Testing\PHPUnit\AbstractRectorTestCase;

use const DIRECTORY_SEPARATOR as DS;

abstract class AbstractSimpleRectorTestCase extends AbstractRectorTestCase
{
    /**
     * @dataProvider provideData()
     * @throws ShouldNotHappenException
     */
    public function test(string $file): void
    {
        $this->doTestFile($file);
    }

    /**
     * @return iterable<array-key, array<array-key, string>>
     */
    public function provideData()
    {
        /** @var iterable<array-key, array<array-key, string>> $iter */
        $iter = static::yieldFilesFromDirectory($this->getDir() . DS . 'Fixture');
        return $iter;
    }


    public function provideConfigFilePath(): string
    {
        return $this->getDir() . DS . 'config' . DS . 'configured_rule.php';
    }

    abstract protected function getDir(): string;
}
