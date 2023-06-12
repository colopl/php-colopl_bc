<?php

if (version_compare(PHP_VERSION, '7.1.0', '>=')) {
    echo "ERROR: Generator script only support php 7.0.x or earlier.\n";
    return;
}

if (!isset($argv[1])) {
    echo "ERROR: Require first parameter : \"seed\".\n";
    return;
}

if (!isset($argv[2])) {
    echo "ERROR: Require second parameter : \"count\".\n";
    return;
}

if (!isset($argv[3])) {
    echo "ERROR: Require third parameter : \"min\".\n";
    return;
}

if (!isset($argv[4])) {
    echo "ERROR: Require third parameter : \"max\".\n";
    return;
}

$seed         = $argv[1];
$php_version  = PHP_VERSION;
$generate_num = (int) $argv[2];
if ($generate_num <= 0) {
    echo "ERROR: Generating count must greater than 0.\n";
    return;
}
$min           = (int) $argv[3];
$max           = (int) $argv[4];
$kernel        = PHP_OS;
$is_zts_enable = (bool) PHP_ZTS;
$zts_state     = $is_zts_enable ? 'Yes' : 'No';
$zts_extension = $is_zts_enable ? 'zts' : 'nts';

$file_name    = strtolower("php_{$php_version}_kernel_{$kernel}_seed_{$seed}_count_{$generate_num}_min_{$min}_max_{$max}.{$zts_extension}_dat");

echo <<<DISP
Generating rand() data
  PHP version      : {$php_version}
  ZTS enabled      : {$zts_state}
  Seed value       : {$argv[1]}
  Generating count : {$generate_num}
  Generating range : {$min}~{$max}
  File name        : {$file_name}

DISP;

echo "Generating...\n";

if (! ($file_handle = fopen(__DIR__.'/'.$file_name, 'w'))) {
    echo "ERROR: Cannot open file ./{$file_name}\n";
    return;
}

srand($seed);
for ($i = 0; $i < $generate_num; $i++) {
    $write  = rand($min, $max);
    if ($generate_num !== ($i + 1)) {
        $write .= PHP_EOL;
    }

    fwrite($file_handle, $write);
}

echo "Finished.\n";