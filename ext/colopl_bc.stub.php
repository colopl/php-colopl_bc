<?php

/** @generate-class-entries */

namespace {
    /**
     * @var int
     * @cvalue COLOPL_BC_PHP74_COMPARE_MODE_SILENT
     */
    const COLOPL_BC_PHP74_COMPARE_MODE_SILENT = UNKNOWN;

    /**
     * @var int
     * @cvalue COLOPL_BC_PHP74_COMPARE_MODE_DEPRECATED
     */
    const COLOPL_BC_PHP74_COMPARE_MODE_DEPRECATED = UNKNOWN;

    /**
     * @var int
     * @cvalue COLOPL_BC_PHP74_COMPARE_MODE_LOG
     */
    const COLOPL_BC_PHP74_COMPARE_MODE_LOG = UNKNOWN;

    /**
     * @var int
     * @cvalue COLOPL_BC_PHP74_SORT_MODE_SILENT
     */
    const COLOPL_BC_PHP74_SORT_MODE_SILENT = UNKNOWN;

    /**
     * @var int
     * @cvalue COLOPL_BC_PHP74_SORT_MODE_DEPRECATED
     */
    const COLOPL_BC_PHP74_SORT_MODE_DEPRECATED = UNKNOWN;

    /**
     * @var int
     * @cvalue COLOPL_BC_PHP74_SORT_MODE_LOG
     */
    const COLOPL_BC_PHP74_SORT_MODE_LOG = UNKNOWN;
}

namespace Colopl\ColoplBc\Php70 {
    function srand(int $seed = 0): void {}
    function rand(int $seed = UNKNOWN, int $max = UNKNOWN): int {}
    function getrandmax(): int {}
    function shuffle(array &$array): bool {}
    function str_shuffle(string $string): string {}
    function array_rand(array $array, int $num = 1): int|string|array {}

    function mt_srand(int $seed = 0): void {}
    function mt_rand(int $min = UNKNOWN, int $max = UNKNOWN): int {}
    function mt_getrandmax(): int {}

    function date_create(string $datetime = 'now', ?\DateTimeZone $timezone = null): \DateTime|false {}
    function date_create_immutable(string $datetime = 'now', ?\DateTimeZone $timezone = null): \DateTimeImmutable|false {}
}

namespace Colopl\ColoplBc\Php74 {
    function ksort(array &$array, int $flags = SORT_REGULAR): bool {}
    function krsort(array &$array, int $flags = SORT_REGULAR): bool {}
    function asort(array &$array, int $flags = SORT_REGULAR): bool {}
    function arsort(array &$array, int $flags = SORT_REGULAR): bool {}
    function sort(array &$array, int $flags = SORT_REGULAR): bool {}
    function rsort(array &$array, int $flags = SORT_REGULAR): bool {}
    function usort(array &$array, callable $callback): bool {}
    function uasort(array &$array, callable $callback): bool {}
    function uksort(array &$array, callable $callback): bool {}

    function in_array(mixed $needle, array $haystack, bool $strict = false): bool {}
    function array_search(mixed $needle, array $haystack, bool $strict = false): int|string|false {}
    function array_keys(array $array, mixed $filter_value = UNKNOWN, bool $strict = false): array {}
    /**
     * @param array $array
     * @param array|int $rest
     * @prefer-ref $array
     * @prefer-ref $rest
     */
    function array_multisort(&$array, &...$rest): bool {}

    function eq(mixed $op1, mixed $op2): bool {}
    function neq(mixed $op1, mixed $op2): bool {}
    function lt(mixed $op1, mixed $op2): bool {}
    function lte(mixed $op1, mixed $op2): bool {}
    function gt(mixed $op1, mixed $op2): bool {}
    function gte(mixed $op1, mixed $op2): bool {}
    function spaceship(mixed $op1, mixed $op2): int {}
}
