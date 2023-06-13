<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector\FuncCall;

use PhpParser\Node;
use PhpParser\Node\Expr\FuncCall;
use Rector\Core\Rector\AbstractRector;
use Symplify\RuleDocGenerator\Exception\PoorDocumentationException;
use Symplify\RuleDocGenerator\ValueObject\CodeSample\CodeSample;
use Symplify\RuleDocGenerator\ValueObject\RuleDefinition;

final class ArrayKeyExistsToPropertyExistsRector extends AbstractRector
{
    /**
     * @codeCoverageIgnore
     * @throws PoorDocumentationException
     */
    public function getRuleDefinition(): RuleDefinition
    {
        return new RuleDefinition(
            'Migrate incompatible array_key_exists to property_exists function.',
            [new CodeSample(
                '$foo = (object)["foo" => "bar"]; array_key_exists("foo", $foo);',
                '$foo = (object)["foo" => "bar"]; property_exists($foo, "foo");'
            )]
        );
    }

    /**
     * @return array{class-string<FuncCall>}
     */
    public function getNodeTypes(): array
    {
        return [FuncCall::class];
    }

    public function refactor(Node $node): ?Node
    {
        \assert($node instanceof FuncCall);

        if ($this->getName($node) !== 'array_key_exists') {
            return \null;
        }

        $args = $node->getArgs();
        if (!\array_key_exists(1, $args)) {
            /* Not perform it. */
            return \null;
        }

        $type = $this->getType($args[1]->value);

        if ($type->isArray()->yes()) {
            /* If it's array, Not perform it. */
            return \null;
        }

        if (!$type->isObject()->yes()) {
            /* If it's not clearly an object, Not perform it. */
            return \null;
        }

        return new FuncCall(new Node\Name('\\property_exists'), [$args[1], $args[0]]);
    }
}
