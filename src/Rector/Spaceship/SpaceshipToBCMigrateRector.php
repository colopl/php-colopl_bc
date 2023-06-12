<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector\Spaceship;

use Colopl\ColoplBc\Rector\AbstractBinaryOpRector;
use PhpParser\Node\Expr\BinaryOp\Spaceship;

final class SpaceshipToBCMigrateRector extends AbstractBinaryOpRector
{
    /**
     * @psalm-return 'spaceship'
     */
    public static function getFunctionName(): string
    {
        return 'spaceship';
    }

    /**
     * @return class-string<Spaceship>
     */
    protected function getBinaryOpNodeString(): string
    {
        return Spaceship::class;
    }
}
