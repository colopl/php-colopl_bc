<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector\NotEqual;

use Colopl\ColoplBc\Rector\AbstractBinaryOpRector;
use PhpParser\Node\Expr\BinaryOp\NotEqual;

final class NotEqualToBCMigrateRector extends AbstractBinaryOpRector
{
    /**
     * @psalm-return 'neq'
     */
    public static function getFunctionName(): string
    {
        return 'neq';
    }

    /**
     * @return class-string<NotEqual>
     */
    protected function getBinaryOpNodeString(): string
    {
        return NotEqual::class;
    }
}
