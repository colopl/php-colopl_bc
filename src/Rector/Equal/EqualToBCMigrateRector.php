<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector\Equal;

use Colopl\ColoplBc\Rector\AbstractBinaryOpRector;
use PhpParser\Node\Expr\BinaryOp\Equal;

final class EqualToBCMigrateRector extends AbstractBinaryOpRector
{
    /**
     * @psalm-return 'eq'
     */
    public static function getFunctionName(): string
    {
        return 'eq';
    }

    /**
     * @return class-string<Equal>
     */
    protected function getBinaryOpNodeString(): string
    {
        return Equal::class;
    }
}
