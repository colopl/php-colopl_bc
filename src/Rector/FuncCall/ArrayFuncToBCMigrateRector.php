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

final class ArrayFuncToBCMigrateRector extends AbstractRector
{
    use BCTrait;

    /**
     * @codeCoverageIgnore
     * @throws PoorDocumentationException
     */
    public function getRuleDefinition(): RuleDefinition
    {
        return new RuleDefinition(
            'Migrate incompatible PHP array functions to colopl_bc functions.',
            [new CodeSample('$arr = range(1, 10); in_array(2, $arr);', '$arr = range(1, 10); \\Colopl\\BackwardsCompatibility\\Php74\\in_array(2, $arr);')]
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
            case $nodeName === 'in_array':
            case $nodeName === 'array_search':
            case $nodeName === 'array_keys':
                return  new FuncCall(new Name($this->getBCFunctionFQN74($nodeName)), $node->args);
            default:
                return \null;
        }
    }
}
