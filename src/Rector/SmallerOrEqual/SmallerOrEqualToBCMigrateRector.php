<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector\SmallerOrEqual;

use Colopl\ColoplBc\Rector\AbstractBinaryOpRector;
use PhpParser\Node\Expr\BinaryOp\SmallerOrEqual;

final class SmallerOrEqualToBCMigrateRector extends AbstractBinaryOpRector
{
    /**
     * @psalm-return 'lte'
     */
    public static function getFunctionName(): string
    {
        return 'lte';
    }

    /**
     * @return class-string<SmallerOrEqual>
     */
    protected function getBinaryOpNodeString(): string
    {
        return SmallerOrEqual::class;
    }
}
