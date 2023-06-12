<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector\Smaller;

use Colopl\ColoplBc\Rector\AbstractBinaryOpRector;
use PhpParser\Node\Expr\BinaryOp\Smaller;

final class SmallerToBCMigrateRector extends AbstractBinaryOpRector
{
    /**
     * @psalm-return 'lt'
     */
    public static function getFunctionName(): string
    {
        return 'lt';
    }

    /**
     * @return class-string<Smaller>
     */
    protected function getBinaryOpNodeString(): string
    {
        return Smaller::class;
    }
}
