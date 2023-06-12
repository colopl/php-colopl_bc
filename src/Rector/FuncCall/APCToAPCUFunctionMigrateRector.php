<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector\FuncCall;

use PhpParser\Node;
use PhpParser\Node\Expr\FuncCall;
use PhpParser\Node\Name;
use PhpParser\Node\Scalar\String_;
use Rector\Core\Rector\AbstractRector;
use Symplify\RuleDocGenerator\Exception\PoorDocumentationException;
use Symplify\RuleDocGenerator\ValueObject\CodeSample\CodeSample;
use Symplify\RuleDocGenerator\ValueObject\RuleDefinition;

final class APCToAPCUFunctionMigrateRector extends AbstractRector
{
    /**
     * @codeCoverageIgnore
     * @throws PoorDocumentationException
     */
    public function getRuleDefinition(): RuleDefinition
    {
        return new RuleDefinition(
            'Migrate APC (apc_bc) functions to native APCu functions.',
            [new CodeSample('apc_clear_cache("user");', 'apcu_clear_cache();')]
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

        /**
         * @see https://github.com/krakjoe/apcu/blob/master/php_apc.stub.php
         */
        switch (\true) {
            /* Simply replacement */
            case $nodeName === 'apc_sma_info':
            case $nodeName === 'apc_store':
            case $nodeName === 'apc_fetch':
            case $nodeName === 'apc_delete':
            case $nodeName === 'apc_add':
            case $nodeName === 'apc_exists':
            case $nodeName === 'apc_inc':
            case $nodeName === 'apc_dec':
            case $nodeName === 'apc_cas':
                return new FuncCall(new Name($this->replaceAPCUFunctionName($nodeName)), $node->args);
            case $nodeName === 'apc_cache_info':
                /* Migratable operations, argument must be "user" */
                $args = $node->getArgs();
                if (\count($args) > 0) {
                    $expr = $args[0]->value;
                    /* Check first argument has "user" */
                    if ($expr instanceof String_) {
                        if (\strtolower($expr->value) !== 'user') {
                            return \null;
                        }
                    }
                    \array_shift($args);
                }
                return new FuncCall(new Name($this->replaceAPCUFunctionName($nodeName)), $args);
            case $nodeName === 'apc_clear_cache':
                /* Remove unneeded argument */
                return new FuncCall(new Name($this->replaceAPCUFunctionName($nodeName)));
            default:
                /* Not supported */
                return \null;
        }
    }

    private function replaceAPCUFunctionName(string $apcFunctionName): string
    {
        return \str_replace('apc_', 'apcu_', $apcFunctionName);
    }
}
