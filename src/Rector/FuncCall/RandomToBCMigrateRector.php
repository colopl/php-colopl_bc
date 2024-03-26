<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector\FuncCall;

use Colopl\ColoplBc\Rector\BCTrait;
use PhpParser\Node;
use PhpParser\Node\Expr\FuncCall;
use PhpParser\Node\Name;
use Rector\Rector\AbstractRector;
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

        switch (\true) {
            case $nodeName === 'srand':
            case $nodeName === 'rand':
            case $nodeName === 'getrandmax':
            case $nodeName === 'shuffle':
            case $nodeName === 'str_shuffle':
            case $nodeName === 'array_rand':
            case $nodeName === 'mt_srand':
            case $nodeName === 'mt_rand':
            case $nodeName === 'mt_getrandmax':
                return new FuncCall(new Name($this->getBCFunctionFQN70($nodeName)), $node->args);
            default:
                return \null;
        }
    }
}
