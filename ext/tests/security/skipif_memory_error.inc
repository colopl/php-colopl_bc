<?php
/*
 * Some PHP builds already end the process on a memory error instead of
 * bailing out: the official minimal backports for 7.4 / 8.0 / 8.1
 * (branches PHP-7.4-CVE-2026-109618-109619 etc., shipped for example by
 * shivammathur/homebrew-php php@8.1 8.1.34_4 and Ubuntu 22.04 php8.1
 * 8.1.2-1ubuntu2.28) print the error to stderr and _exit(1) without going
 * through zend_error_cb, while PHP_VERSION stays unchanged. Run a memory
 * error in the running SAPI without colopl_bc's abort and skip only when
 * PHP itself ended the process (error seen, shutdown function not run);
 * any other probe outcome lets the test run.
 */
if (!function_exists('proc_open')) {
    return;
}

$colopl_bc_probe_dir = sys_get_temp_dir() . DIRECTORY_SEPARATOR . 'colopl_bc_memory_error_probe_' . getmypid();
if (!is_dir($colopl_bc_probe_dir) && !@mkdir($colopl_bc_probe_dir)) {
    return;
}
$colopl_bc_probe_script = $colopl_bc_probe_dir . DIRECTORY_SEPARATOR . 'probe.php';
file_put_contents($colopl_bc_probe_script, '<?php
register_shutdown_function(function () { echo "colopl-bc-probe-shutdown-ran"; });
$s = str_repeat("x", 64 * 1024 * 1024);
echo "colopl-bc-probe-after";
');

$colopl_bc_probe_ini = array(
    'memory_limit=8M',
    'display_errors=1',
    'log_errors=0',
    'html_errors=0',
    'colopl_bc.security.abort_on_memory_error=0',
    'colopl_bc.security.abort_on_timeout=0',
);
// -n: no php.ini and no shared colopl_bc; the -d above covers static builds.
// php-cgi honours -d but not -q in CGI mode, so the output starts with headers.
$colopl_bc_probe_cmd = escapeshellarg(PHP_BINARY) . ' -n';
foreach ($colopl_bc_probe_ini as $colopl_bc_probe_setting) {
    $colopl_bc_probe_cmd .= ' -d ' . escapeshellarg($colopl_bc_probe_setting);
}
$colopl_bc_probe_cmd .= ' ' . escapeshellarg($colopl_bc_probe_script);

$colopl_bc_probe_env = array();
foreach (array('PATH', 'SYSTEMROOT', 'TEMP', 'TMP') as $colopl_bc_probe_var) {
    if (getenv($colopl_bc_probe_var) !== false) {
        $colopl_bc_probe_env[$colopl_bc_probe_var] = getenv($colopl_bc_probe_var);
    }
}
$colopl_bc_probe_env['REDIRECT_STATUS'] = '1';
$colopl_bc_probe_env['SCRIPT_FILENAME'] = $colopl_bc_probe_script;

$colopl_bc_probe_output = '';
$colopl_bc_probe_proc = proc_open(
    $colopl_bc_probe_cmd,
    array(0 => array('pipe', 'r'), 1 => array('pipe', 'w'), 2 => array('pipe', 'w')),
    $colopl_bc_probe_pipes,
    $colopl_bc_probe_dir,
    $colopl_bc_probe_env
);
if (is_resource($colopl_bc_probe_proc)) {
    fclose($colopl_bc_probe_pipes[0]);
    $colopl_bc_probe_output = stream_get_contents($colopl_bc_probe_pipes[1]);
    fclose($colopl_bc_probe_pipes[1]);
    $colopl_bc_probe_output .= stream_get_contents($colopl_bc_probe_pipes[2]);
    fclose($colopl_bc_probe_pipes[2]);
    proc_close($colopl_bc_probe_proc);
}

@unlink($colopl_bc_probe_script);
@rmdir($colopl_bc_probe_dir);

if (strpos($colopl_bc_probe_output, 'Allowed memory size of') !== false
    && strpos($colopl_bc_probe_output, 'colopl-bc-probe-shutdown-ran') === false) {
    die('skip this PHP build already ends the process on a memory error (official EOL backport)');
}
