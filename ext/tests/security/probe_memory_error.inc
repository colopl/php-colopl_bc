<?php
/*
 * Runs a memory error in the running SAPI without colopl_bc's abort and
 * reports how PHP itself handled it:
 *
 * - 'exit':    PHP ended the process (error seen, shutdown function not run),
 *              as the unversioned backports of the 2026-10-21 security release
 *              do (the official minimal backports for 7.4 / 8.0 / 8.1, branches
 *              PHP-7.4-CVE-2026-109618-109619 etc., and deb.sury.org's ports
 *              down to 5.6), which print the error to stderr and _exit(1)
 *              without going through zend_error_cb while PHP_VERSION stays
 *              unchanged;
 * - 'bailout': the shutdown function ran after the error;
 * - null:      the probe could not tell.
 */
function colopl_bc_probe_memory_error()
{
    if (!function_exists('proc_open')) {
        return null;
    }

    $dir = sys_get_temp_dir() . DIRECTORY_SEPARATOR . 'colopl_bc_memory_error_probe_' . getmypid();
    if (!is_dir($dir) && !@mkdir($dir)) {
        return null;
    }
    $script = $dir . DIRECTORY_SEPARATOR . 'probe.php';
    file_put_contents($script, '<?php
register_shutdown_function(function () { echo "colopl-bc-probe-shutdown-ran"; });
$s = str_repeat("x", 64 * 1024 * 1024);
echo "colopl-bc-probe-after";
');

    $ini = array(
        'memory_limit=8M',
        'display_errors=1',
        'log_errors=0',
        'html_errors=0',
        'colopl_bc.security.abort_on_memory_error=0',
        'colopl_bc.security.abort_on_timeout=0',
    );
    // -n: no php.ini and no shared colopl_bc; the -d above covers static builds.
    // php-cgi honours -d but not -q in CGI mode, so the output starts with headers.
    $cmd = escapeshellarg(PHP_BINARY) . ' -n';
    foreach ($ini as $setting) {
        $cmd .= ' -d ' . escapeshellarg($setting);
    }
    $cmd .= ' ' . escapeshellarg($script);

    $env = array();
    foreach (array('PATH', 'SYSTEMROOT', 'TEMP', 'TMP') as $var) {
        if (getenv($var) !== false) {
            $env[$var] = getenv($var);
        }
    }
    $env['REDIRECT_STATUS'] = '1';
    $env['SCRIPT_FILENAME'] = $script;

    $output = '';
    $proc = proc_open(
        $cmd,
        array(0 => array('pipe', 'r'), 1 => array('pipe', 'w'), 2 => array('pipe', 'w')),
        $pipes,
        $dir,
        $env
    );
    if (is_resource($proc)) {
        fclose($pipes[0]);
        $output = stream_get_contents($pipes[1]);
        fclose($pipes[1]);
        $output .= stream_get_contents($pipes[2]);
        fclose($pipes[2]);
        proc_close($proc);
    }

    @unlink($script);
    @rmdir($dir);

    if (strpos($output, 'colopl-bc-probe-shutdown-ran') !== false) {
        return 'bailout';
    }
    if (strpos($output, 'Allowed memory size of') !== false) {
        return 'exit';
    }
    return null;
}
