<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector\FuncCall;

use PhpParser\Node;
use PhpParser\Node\Expr\FuncCall;
use PhpParser\Node\Name;
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
            /* 単純置換対応 */
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
                /* "user" 以外は対応できない (そもそもそれはもう OPcache がやってる) が
                 * 少なくともコロプラでは必要ないので単に第一引数を無視する */
                $args = $node->args;
                if (\count($args) > 0) {
                    \array_shift($args);
                }
                return new FuncCall(new Name($this->replaceAPCUFunctionName($nodeName)), $args);
            case $nodeName === 'apc_clear_cache':
                /* "user" 引数以外は対応できない (そもそもそれはもう OPcache がやってる) が
                 * 少なくともコロプラでは必要ないので単に引数をなくす */
                return new FuncCall(new Name($this->replaceAPCUFunctionName($nodeName)));
            default:
                /* これ以外は別の関数 or APC 固有の機能なので非対応 */
                return \null;
        }
    }

    private function replaceAPCUFunctionName(string $apcFunctionName): string
    {
        return \str_replace('apc_', 'apcu_', $apcFunctionName);
    }
}
