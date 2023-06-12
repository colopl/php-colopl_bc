<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector\Switch_;

use Colopl\ColoplBc\Rector\Equal\EqualToBCMigrateRector;
use Colopl\ColoplBc\Rector\BCTrait;
use LogicException;
use PhpParser\Node;
use PhpParser\Node\Expr\BinaryOp\Equal;
use PhpParser\Node\Expr\BinaryOp;
use PhpParser\Node\Expr\ConstFetch;
use PhpParser\Node\Expr\FuncCall;
use PhpParser\Node\Stmt\Switch_;
use Rector\Core\Rector\AbstractRector;
use Symplify\RuleDocGenerator\Exception\PoorDocumentationException;
use Symplify\RuleDocGenerator\ValueObject\CodeSample\CodeSample;
use Symplify\RuleDocGenerator\ValueObject\RuleDefinition;

final class SwitchCaseToEqualRector extends AbstractRector
{
    use BCTrait;

    /**
     * @codeCoverageIgnore
     * @throws PoorDocumentationException
     */
    public function getRuleDefinition(): RuleDefinition
    {
        return new RuleDefinition(
            'Change switch() to equals.',
            [
                new CodeSample(
                    <<<'CODE_SAMPLE'
switch ($input) {
    default:
        return 3;
        break;
    case Lexer::T_FOO:
        return 1;
        break;
    case Lexer::T_BAR:
        return 2;
        break;
}
CODE_SAMPLE
                    ,
                    <<<'CODE_SAMPLE'
switch (true) {
    default:
        return 3;
        break;
    case $input == Lexer::T_FOO:
        return 1;
        break;
    case $input == Lexer::T_BAR:
        return 2;
        break;
}
CODE_SAMPLE
                )
            ]
        );
    }

    /**
     * @return array<class-string<Switch_>>
     */
    public function getNodeTypes(): array
    {
        return [Switch_::class];
    }

    public function refactor(Node $node): ?Switch_
    {
        \assert($node instanceof Switch_);

        $switchCond = $node->cond;

        /* Not perform if switchCond is null */
        if ($switchCond instanceof ConstFetch && $this->getName($switchCond) === 'true') {
            return \null;
        }

        $isAdopt = \false;
        foreach ($node->cases as $case) {
            /* Not perform if case is null */
            if ($case->cond === \null) {
                continue;
            }
            /**
             * Not perform if case is converted
             * @see EqualToBCMigrateRector
             */
            if ($case->cond instanceof FuncCall &&
                (
                    '\\' . ($this->getName($case->cond->name) ?? '') ===
                    $this->getBCFunctionFQN74(EqualToBCMigrateRector::getFunctionName())
                )
            ) {
                continue;
            }
            /* Not perform if case cond is BinaryOp */
            if ($case->cond instanceof BinaryOp) {
                continue;
            }
            if ($switchCond instanceof ConstFetch && $this->getName($switchCond) === 'true') {
                /*
                 * If switchCond is true, it is probably merged after conversion.
                 * Since the original value passed to switch () is unknown, an error is displayed for safety.
                 */
                throw new LogicException('already converted switch statement, so can not get original value passed to switch(). please fix manually.');
            }
            $case->cond = new Equal($case->cond, $switchCond);
            $isAdopt = \true;
        }

        /* If is not adopted, cancel conversion */
        if (!$isAdopt) {
            return \null;
        }

        /* Force set to true */
        $node->cond = $this->nodeFactory->createTrue();

        return $node;
    }
}
