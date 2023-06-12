--TEST--
Check for \Colopl\ColoplBc\Php70\srand() function other environment incompatibility.
--EXTENSIONS--
colopl_bc
--FILE--
<?php

$extension = ((bool) PHP_ZTS) ? '.zts_dat' : '.nts_dat';

foreach (scandir((realpath(__DIR__.'/data'))) as $env_dump_file) {
    if (!preg_match("/{$extension}$/", $env_dump_file)) {
        continue;
    }
    preg_match(
        '/^php_(.*?)_kernel_(.*?)_seed_(.*?)_count_(.*?)_min_(.*?)_max_(.*?)$/',
        pathinfo($env_dump_file, PATHINFO_FILENAME),
        $matches
    );
    
    [$file_name, $php_version, $kernel_type, $seed, $count, $min, $max] = $matches;

    echo "Checking: {$file_name}...";

    $file_pointer = fopen(realpath(__DIR__.'/data') . DIRECTORY_SEPARATOR . $env_dump_file, 'r');
    
    if ($file_pointer === false) {
        echo "\nFile opening error! : {$file_name}";
        exit;
    }

    srand((int) $seed);

    while ($generated_num = fgets($file_pointer)) {
        if (($generate_num = rand((int) $min, (int) $max)) !== (int) $generated_num) {
            echo "NG. (generate: {$generate_num} reference: {$generated_num})\n\nFailed.";
            exit;
        }
    }

    echo "OK.\n";
}

echo 'Success.';
?>
--EXPECTREGEX--
.*Failed\.$
