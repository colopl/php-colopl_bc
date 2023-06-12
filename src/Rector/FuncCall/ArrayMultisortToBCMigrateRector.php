<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector\FuncCall;

use Colopl\ColoplBc\Rector\BCTrait;
use PhpParser\Node;
use PhpParser\Node\Expr\BinaryOp\GreaterOrEqual;
use PhpParser\Node\Expr\ConstFetch;
use PhpParser\Node\Expr\FuncCall;
use PhpParser\Node\Expr\Ternary;
use PhpParser\Node\Name;
use PhpParser\Node\Scalar\LNumber;
use Rector\Core\Rector\AbstractRector;
use Symplify\RuleDocGenerator\Exception\PoorDocumentationException;
use Symplify\RuleDocGenerator\ValueObject\CodeSample\CodeSample;
use Symplify\RuleDocGenerator\ValueObject\RuleDefinition;

final class ArrayMultisortToBCMigrateRector extends AbstractRector
{
    use BCTrait;

    /**
     * @codeCoverageIgnore
     * @throws PoorDocumentationException
     */
    public function getRuleDefinition(): RuleDefinition
    {
        return new RuleDefinition(
            'Migrate incompatible PHP array_multisort function to colopl_bc function.',
            [
                new CodeSample(
                    'array_multisort($foo, SORT_ASC);',
                    'PHP_VERSION_ID >= 80000 ? \\Colopl\\BackwardsCompatibility\\Php74\\array_multisort($foo, SORT_ASC) : array_multisort($foo, SORT_ASC);'
                )
            ]
        );
    }

    /**
     * @return array<class-string<FuncCall>>
     */
    public function getNodeTypes(): array
    {
        return [FuncCall::class];
    }

    public function refactor(Node $node): ?Node
    {
        \assert($node instanceof FuncCall);

        if ($this->getName($node) !== 'array_multisort') {
            return \null;
        }

        return new Ternary(
            new GreaterOrEqual(
                new ConstFetch(new Name('PHP_VERSION_ID')),
                new LNumber(80000)
            ),
            new FuncCall(new Name($this->getBCFunctionFQN74('array_multisort')), $node->args),
            $node
        );
    }
}
