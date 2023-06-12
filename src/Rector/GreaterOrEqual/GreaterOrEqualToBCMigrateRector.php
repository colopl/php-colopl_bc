<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector\GreaterOrEqual;

use Colopl\ColoplBc\Rector\AbstractBinaryOpRector;
use PhpParser\Node\Expr\BinaryOp\GreaterOrEqual;

final class GreaterOrEqualToBCMigrateRector extends AbstractBinaryOpRector
{
    /**
     * @psalm-return 'gte'
     */
    public static function getFunctionName(): string
    {
        return 'gte';
    }

    /**
     * @return class-string<GreaterOrEqual>
     */
    protected function getBinaryOpNodeString(): string
    {
        return GreaterOrEqual::class;
    }
}
