<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector\FuncCall;

use Colopl\ColoplBc\Rector\BCTrait;
use PhpParser\Node;
use PhpParser\Node\Expr\FuncCall;
use PhpParser\Node\Name;
use Rector\Core\Rector\AbstractRector;
use Symplify\RuleDocGenerator\Exception\PoorDocumentationException;
use Symplify\RuleDocGenerator\ValueObject\CodeSample\CodeSample;
use Symplify\RuleDocGenerator\ValueObject\RuleDefinition;

final class RandomToBCMigrateRector extends AbstractRector
{
    use BCTrait;

    /**
     * @codeCoverageIgnore
     * @throws PoorDocumentationException
     */
    public function getRuleDefinition(): RuleDefinition
    {
        return new RuleDefinition(
            'Migrate incompatible PHP random functions to colopl_bc functions.',
            [new CodeSample('srand();', '\\Colopl\\ColoplBc\\Php70\\srand();')]
        );
    }

    /**
     * @psalm-return array<class-string<FuncCall>>
     */
    public function getNodeTypes(): array
    {
        return [FuncCall::class];
    }

    public function refactor(Node $node): ?FuncCall
    {
        \assert($node instanceof FuncCall);

        $nodeName = $this->getName($node);
        if ($nodeName === \null) {
            return \null;
        }

        return match (\true) {
            $nodeName === 'srand', $nodeName === 'rand', $nodeName === 'getrandmax', $nodeName === 'shuffle', $nodeName === 'str_shuffle', $nodeName === 'array_rand', $nodeName === 'mt_srand', $nodeName === 'mt_rand', $nodeName === 'mt_getrandmax' => new FuncCall(new Name($this->getBCFunctionFQN70($nodeName)), $node->args),
            default => \null,
        };
    }
}
