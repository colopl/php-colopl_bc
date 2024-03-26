<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector\New_;

use PhpParser\Node;
use PhpParser\Node\Expr\New_;
use PhpParser\Node\Name;
use Rector\Rector\AbstractRector;
use Symplify\RuleDocGenerator\Exception\PoorDocumentationException;
use Symplify\RuleDocGenerator\ValueObject\CodeSample\CodeSample;
use Symplify\RuleDocGenerator\ValueObject\RuleDefinition;

final class NewAPCIteratorToAPCUIteratorRector extends AbstractRector
{
    /**
     * @throws PoorDocumentationException
     */
    public function getRuleDefinition(): RuleDefinition
    {
        return new RuleDefinition(
            'Migrate APCIterator::__construct() to APCUIterator::__construct().',
            [new CodeSample('new APCIterator("user");', 'new APCUIterator();')]
        );
    }

    /**
     * @return array<class-string<New_>>
     */
    public function getNodeTypes(): array
    {
        return [New_::class];
    }

    public function refactor(Node $node): ?New_
    {
        \assert($node instanceof New_);
        if (!$node->class instanceof Name || !$this->isName($node->class, 'APCIterator')) {
            return \null;
        }

        $args = $node->args;
        /* Remove unneeded argument */
        \array_shift($args);

        return new New_(new Name\FullyQualified('APCUIterator'), $args);
    }
}
