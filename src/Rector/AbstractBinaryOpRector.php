<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector;

use PhpParser\Node;
use PhpParser\Node\Arg;
use PhpParser\Node\Name;
use PhpParser\Node\Expr\BinaryOp;
use PhpParser\Node\Expr\FuncCall;
use PhpParser\Node\Expr\Variable;
use Rector\Core\Rector\AbstractRector;
use Symplify\RuleDocGenerator\Exception\PoorDocumentationException;
use Symplify\RuleDocGenerator\ValueObject\CodeSample\CodeSample;
use Symplify\RuleDocGenerator\ValueObject\RuleDefinition;

abstract class AbstractBinaryOpRector extends AbstractRector
{
    use BCTrait;

    /**
     * @codeCoverageIgnore
     * @throws PoorDocumentationException
     */
    public function getRuleDefinition(): RuleDefinition
    {
        $operator = $this->getBinaryOp()->getOperatorSigil();
        $function = $this->getBCFunctionFQN74(static::getFunctionName());

        return new RuleDefinition(
            'Migrate incompatible PHP BinaryOp to colopl_bc functions.',
            [new CodeSample("0 {$operator} '';", "0 {$function} '';")]
        );
    }

    /**
     * @return array<class-string<BinaryOp>>
     */
    public function getNodeTypes(): array
    {
        return [$this->getBinaryOpNodeString()];
    }

    public function refactor(Node $node): ?FuncCall
    {
        \assert($node instanceof BinaryOp);

        return $this->getFuncCall($node);
    }

    /**
     * @return non-empty-string
     */
    abstract public static function getFunctionName(): string;

    /**
     * @return class-string<BinaryOp>
     */
    abstract protected function getBinaryOpNodeString(): string;

    private function getFuncCall(BinaryOp $node): FuncCall
    {
        return new FuncCall(new Name($this->getBCFunctionFQN74(static::getFunctionName())), [new Arg($node->left), new Arg($node->right)]);
    }

    private function getBinaryOp(): BinaryOp
    {
        $fqn = $this->getBinaryOpNodeString();
        /** @psalm-suppress UnsafeInstantiation */
        return new $fqn(new Variable('stub'), new Variable('stub'));
    }
}
