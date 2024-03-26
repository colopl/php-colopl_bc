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

final class SortToBCMigrateRector extends AbstractRector
{
    use BCTrait;

    /**
     * @codeCoverageIgnore
     * @throws PoorDocumentationException
     */
    public function getRuleDefinition(): RuleDefinition
    {
        return new RuleDefinition(
            'Migrate incompatible PHP sort functions to colopl_bc functions.',
            [new CodeSample('$arr = range(1, 10); sort($arr);', '$arr = range(1, 10); \\Colopl\\BackwardsCompatibility\\Php74\\sort($arr);')]
        );
    }

    /**
     * @return array<class-string<FuncCall>>
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
            case $nodeName === 'ksort':
            case $nodeName === 'krsort':
            case $nodeName === 'asort':
            case $nodeName === 'arsort':
            case $nodeName === 'sort':
            case $nodeName === 'rsort':
            case $nodeName === 'usort':
            case $nodeName === 'uasort':
            case $nodeName === 'uksort':
                return new FuncCall(new Name($this->getBCFunctionFQN74($nodeName)), $node->args);
            default:
                return \null;
        }
    }
}
