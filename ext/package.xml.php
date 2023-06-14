<?php

declare(strict_types=1);

$now = new \DateTimeImmutable('now');

/** @var list<array{non-empty-string, non-empty-string}> $maintainers */
$maintainers = \array_map(
    static fn (string $value): array => \explode("\t", $value),
    \explode("\n", \trim(\shell_exec('git log --pretty=format:"%an%x09%ae" | sort | uniq') ?: ''))
);

/** @var list<array{non-empty-string, non-empty-string, non-empty-string}> $releases */
$releases = \array_reverse(\array_filter(
    \array_map(
        static fn (string $value): array => \explode("\t", $value),
        \explode("\n", \preg_replace('/^\n$/m', '',\shell_exec('git for-each-ref --format="%(refname:short)%09%(contents)" refs/tags') ?: '') ?? '')
    ),
    static fn (array $arr): bool => \count($arr) > 1
));

[$version_major, $version_minor, $version_patch] = \explode('.', \trim(\shell_exec('git describe --tags') ?: ''));

$files_src_headers = \array_map(
    static fn (string $path): string => \str_replace(__DIR__ . \DIRECTORY_SEPARATOR, '', $path),
    (static function (): array {
        $headers = [];
        foreach ((new RecursiveIteratorIterator(new RecursiveDirectoryIterator(__DIR__))) as $f) {
            \assert($f instanceof \SplFileInfo);
            if ($f->isFile() && \preg_match('/\.h$/i', $f->getFilename())) {
                $headers[] = $f->getPathname();
            }
        }
        return $headers;
    })()
);

$files_src_sources = \array_map(
    static fn (string $path): string => \str_replace(__DIR__ . \DIRECTORY_SEPARATOR, '', $path),
    (static function (): array {
        $sources = [];
        foreach ((new RecursiveIteratorIterator(new RecursiveDirectoryIterator(__DIR__))) as $f) {
            \assert($f instanceof \SplFileInfo);
            if ($f->isFile() && \preg_match('/\.c$/i', $f->getFilename())) {
                $sources[] = $f->getPathname();
            }
        }
        return $sources;
    })()
);

$files_tests = \array_map(
    static fn (string $path): string => \str_replace(__DIR__ . \DIRECTORY_SEPARATOR, '', $path),
    (static function (): array {
        $sources = [];
        foreach ((new RecursiveIteratorIterator(new RecursiveDirectoryIterator(__DIR__))) as $f) {
            \assert($f instanceof \SplFileInfo);
            if ($f->isFile() && \preg_match('/\.phpt$/i', $f->getFilename())) {
                $sources[] = $f->getPathname();
            }
        }
        return $sources;
    })()
);

?>
<?xml version="1.0" encoding="UTF-8"?>
<package packagerversion="1.10.13"
         version="2.0"
         xmlns="http://pear.php.net/dtd/package-2.0"
         xmlns:tasks="http://pear.php.net/dtd/tasks-1.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://pear.php.net/dtd/tasks-1.0 http://pear.php.net/dtd/tasks-1.0.xsd http://pear.php.net/dtd/package-2.0 http://pear.php.net/dtd/package-2.0.xsd">
    <name>colopl_bc</name>
    <channel>pecl.php.net</channel>
    <summary>colopl_bc - BC Extension</summary>
    <description>Provides backwards compatibility for Legacy PHP versions.</description>
<?php foreach ($maintainers as [$name, $email]) : ?>
    <lead>
        <name><?= $name ?></name>
        <user><?= \explode('@', $email)[0] ?></user>
        <email><?= $email ?></email>
        <active>yes</active>
    </lead>
<?php endforeach; ?>
    <date><?= $now->format('Y-m-d') ?></date>
    <time><?= $now->format('H:i:s') ?></time>
    <version>
        <release><?= $version_major ?>.<?= $version_minor ?>.<?= $version_patch ?></release>
        <api><?= $version_major?>.0.0</api>
    </version>
    <stability>
        <release>stable</release>
        <api>stable</api>
    </stability>
    <license uri="http://www.php.net/license/3_01.txt">PHP License 3.01</license>
    <notes><?= $releases[0][1] ?? '' ?></notes>
    <contents>
        <dir baseinstalldir="/" name="/">
<?php foreach ($files_src_headers as $header): ?>
            <file md5sum="<?= \md5_file(__DIR__ . \DIRECTORY_SEPARATOR . $header) ?>" name="<?= $header ?>" role="src" />
<?php endforeach; ?>
<?php foreach ($files_src_sources as $source): ?>
            <file md5sum="<?= \md5_file(__DIR__ . \DIRECTORY_SEPARATOR . $source) ?>" name="<?= $source ?>" role="src" />
<?php endforeach; ?>
<?php foreach ($files_tests as $test): ?>
            <file md5sum="<?= \md5_file(__DIR__ . \DIRECTORY_SEPARATOR . $test) ?>" name="<?= $test ?>" role="test" />
<?php endforeach; ?>
        </dir>
    </contents>
    <dependencies>
        <required>
            <php>
                <min>8.0.0</min>
            </php>
            <pearinstaller>
                <min>1.10.13</min>
            </pearinstaller>
        </required>
    </dependencies>
    <providesextension>colopl_bc</providesextension>
    <extsrcrelease />
    <changelog>
<?php foreach ($releases as [$version, $message]): ?>
        <release>
            <date><?= $now->format('Y-m-d') ?></date>
            <version>
                <release><?= $version ?></release>
                <api><?= $version_major?>.0.0</api>
            </version>
            <stability>
                <release>stable</release>
                <api>stable</api>
            </stability>
            <license uri="http://www.php.net/license/3_01.txt">PHP License 3.01</license>
            <notes><?= $message ?></notes>
        </release>
<?php endforeach; ?>
    </changelog>
</package>
