<?php

declare(strict_types=1);

namespace Colopl\ColoplBc\Rector;

trait BCTrait
{
    /**
     * @return non-empty-string
     */
    public function getBCFunctionNamespace(): string
    {
        return '\\Colopl\\ColoplBc\\';
    }

    /**
     * @return non-empty-string
     */
    public function getBCFunctionNamespace70(): string
    {
        return $this->getBCFunctionNamespace() . 'Php70\\';
    }

    /**
     * @return non-empty-string
     */
    public function getBCFunctionFQN70(string $functionName): string
    {
        return $this->getBCFunctionNamespace70() . $functionName;
    }

    /**
     * @return non-empty-string
     */
    public function getBCFunctionNamespace74(): string
    {
        return $this->getBCFunctionNamespace() . 'Php74\\';
    }

    /**
     * @return non-empty-string
     */
    public function getBCFunctionFQN74(string $functionName): string
    {
        return $this->getBCFunctionNamespace74() . $functionName;
    }
}
