<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector\Greater;

use Colopl\ColoplBc\Rector\AbstractBinaryOpRector;
use PhpParser\Node\Expr\BinaryOp\Greater;

final class GreaterToBCMigrateRector extends AbstractBinaryOpRector
{
    /**
     * @psalm-return 'gt'
     */
    public static function getFunctionName(): string
    {
        return 'gt';
    }

    /**
     * @return class-string<Greater>
     */
    protected function getBinaryOpNodeString(): string
    {
        return Greater::class;
    }
}
