<?php

namespace {
    if (\PHP_VERSION_ID >= 80000 && !\extension_loaded('colopl_bc')) {
        \user_error('colopl_bc extension is not loaded', \E_USER_ERROR);
    }
}

namespace Colopl\ColoplBc\Php74 {
    if (!\function_exists(__NAMESPACE__ . '\\ksort')) {
        /**
         * @param array<array-key, mixed> $array
         * @param int $flags
         * @return bool|null
         * @deprecated
         * @phpstan-ignore-next-line
         */
        function ksort(&$array, $flags = \SORT_REGULAR)
        {
            return \ksort($array, $flags);
        }
    }

    if (!\function_exists(__NAMESPACE__ . '\\krsort')) {
        /**
         * @param array<array-key, mixed> $array
         * @param int $flags
         * @return bool|null
         * @deprecated
         * @phpstan-ignore-next-line
         */
        function krsort(&$array, $flags = \SORT_REGULAR)
        {
            return \krsort($array, $flags);
        }
    }
    if (!\function_exists(__NAMESPACE__ . '\\asort')) {
        /**
         * @param array<array-key, mixed> $array
         * @param int $flags
         * @return bool|null
         * @deprecated
         * @phpstan-ignore-next-line
         */
        function asort(&$array, $flags = \SORT_REGULAR)
        {
            return \asort($array, $flags);
        }
    }

    if (!\function_exists(__NAMESPACE__ . '\\arsort')) {
        /**
         * @param array<array-key, mixed> $array
         * @param int $flags
         * @return bool|null
         * @deprecated
         * @phpstan-ignore-next-line
         */
        function arsort(&$array, $flags = \SORT_REGULAR)
        {
            return \arsort($array, $flags);
        }
    }

    if (!\function_exists(__NAMESPACE__ . '\\sort')) {
        /**
         * @param array<array-key, mixed> $array
         * @param int $flags
         * @return bool|null
         * @deprecated
         * @phpstan-ignore-next-line
         */
        function sort(&$array, $flags = \SORT_REGULAR)
        {
            return \sort($array, $flags);
        }
    }

    if (!\function_exists(__NAMESPACE__ . '\\rsort')) {
        /**
         * @param array<array-key, mixed> $array
         * @param int $flags
         * @return bool|null
         * @deprecated
         * @phpstan-ignore-next-line
         */
        function rsort(&$array, $flags = \SORT_REGULAR)
        {
            return \rsort($array, $flags);
        }
    }
    if (!\function_exists(__NAMESPACE__ . '\\usort')) {
        /**
         * @param array<array-key, mixed> $array
         * @param callable(mixed, mixed): int $callback
         * @phpstan-param callable $callback
         * @psalm-param callable(mixed, mixed): int $callback
         * @return bool|null
         * @deprecated
         * @phpstan-ignore-next-line
         */
        function usort(&$array, $callback)
        {
            return \usort($array, $callback);
        }
    }

    if (!\function_exists(__NAMESPACE__ . '\\uasort')) {
        /**
         * @param array<array-key, mixed> $array
         * @param callable(mixed, mixed): int $callback
         * @phpstan-param callable $callback
         * @psalm-param callable(mixed, mixed): int $callback
         * @return bool|null
         * @deprecated
         * @phpstan-ignore-next-line
         */
        function uasort(&$array, $callback)
        {
            return \uasort($array, $callback);
        }
    }

    if (!\function_exists(__NAMESPACE__ . '\\uksort')) {
        /**
         * @param array<array-key, mixed> $array
         * @param callable(mixed, mixed): int $callback
         * @psalm-param callable(mixed, mixed): int $callback
         * @phpstan-param callable $callback
         * @return bool|null
         * @deprecated
         * @phpstan-ignore-next-line
         */
        function uksort(&$array, $callback)
        {
            return \uksort($array, $callback);
        }
    }

    if (!\function_exists(__NAMESPACE__ . '\\in_array')) {
        /**
         * @param mixed $needle
         * @param array<array-key, mixed> $haystack
         * @param bool $strict
         * @deprecated
         */
        function in_array($needle, $haystack, $strict = false): bool
        {
            return \in_array($needle, $haystack, $strict);
        }
    }

    if (!\function_exists(__NAMESPACE__ . '\\array_search')) {
        /**
         * @param mixed $needle
         * @param array<array-key, mixed> $haystack
         * @param bool $strict
         * @return array-key|false|null
         * @deprecated
         * @phpstan-ignore-next-line
         */
        function array_search($needle, $haystack, $strict = false)
        {
            return \array_search($needle, $haystack, $strict);
        }
    }

    if (!\function_exists(__NAMESPACE__ . '\\array_keys')) {
        /**
         * @param mixed ...$args
         * @return array<int, array-key>
         * @deprecated
         */
        function array_keys(...$args): array
        {
            /** @var array<int, array-key> $ret */
            $ret = \call_user_func_array('\\array_keys', $args);
            return $ret;
        }
    }

    if (!\function_exists(__NAMESPACE__ . '\\array_multisort')) {
        /**
         * @param mixed ...$_args
         * @deprecated
         */
        function array_multisort(...$_args): bool
        {
            throw new \LogicException('array_multisort doesn\'t supported, Please applying migration rector rule.');
        }
    }

    if (!function_exists(__NAMESPACE__ . '\\eq')) {
        /**
         * @param mixed $op1
         * @param mixed $op2
         * @deprecated
         */
        function eq($op1, $op2): bool
        {
            return $op1 == $op2;
        }
    }

    if (!function_exists(__NAMESPACE__ . '\\neq')) {
        /**
         * @param mixed $op1
         * @param mixed $op2
         * @deprecated
         */
        function neq($op1, $op2): bool
        {
            return $op1 != $op2;
        }
    }

    if (!function_exists(__NAMESPACE__ . '\\lt')) {
        /**
         * @param mixed $op1
         * @param mixed $op2
         * @deprecated
         */
        function lt($op1, $op2): bool
        {
            return $op1 < $op2;
        }
    }

    if (!function_exists(__NAMESPACE__ . '\\lte')) {
        /**
         * @param mixed $op1
         * @param mixed $op2
         * @deprecated
         */
        function lte($op1, $op2): bool
        {
            return $op1 <= $op2;
        }
    }

    if (!function_exists(__NAMESPACE__ . '\\gt')) {
        /**
         * @param mixed $op1
         * @param mixed $op2
         * @deprecated
         */
        function gt($op1, $op2): bool
        {
            return $op1 > $op2;
        }
    }

    if (!function_exists(__NAMESPACE__ . '\\gte')) {
        /**
         * @param mixed $op1
         * @param mixed $op2
         * @deprecated
         */
        function gte($op1, $op2): bool
        {
            return $op1 >= $op2;
        }
    }

    if (!function_exists(__NAMESPACE__ . '\\spaceship')) {
        /**
         * @param mixed $op1
         * @param mixed $op2
         * @deprecated
         */
        function spaceship($op1, $op2): int
        {
            return $op1 <=> $op2;
        }
    }
}