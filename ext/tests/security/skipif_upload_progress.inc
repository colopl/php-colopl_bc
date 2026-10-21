<?php
/*
 * Some PHP builds ship ext/session with the upload progress handler turned
 * into a no-op (shivammathur/homebrew-php php@8.1 8.1.34_4 backports the
 * 2026-10-21 security release that way while PHP_VERSION stays 8.1.34).
 * Send a real multipart request to the running SAPI without colopl_bc and
 * skip only when PHP itself never records the progress; any other probe
 * outcome lets the test run.
 */
if (!function_exists('proc_open')) {
    return;
}

$colopl_bc_probe_dir = sys_get_temp_dir() . DIRECTORY_SEPARATOR . 'colopl_bc_upload_progress_probe_' . getmypid();
if (!is_dir($colopl_bc_probe_dir) && !@mkdir($colopl_bc_probe_dir)) {
    return;
}
$colopl_bc_probe_script = $colopl_bc_probe_dir . DIRECTORY_SEPARATOR . 'probe.php';
file_put_contents($colopl_bc_probe_script, '<?php
if (!function_exists("session_start")) { echo "no-session"; return; }
session_start();
echo isset($_SESSION["upload_progress_probe"]) ? "recorded" : "missing";
session_destroy();
');

$colopl_bc_probe_boundary = 'colopl-bc-upload-progress-probe';
$colopl_bc_probe_body =
    "--{$colopl_bc_probe_boundary}\r\n" .
    "Content-Disposition: form-data; name=\"PHP_SESSION_UPLOAD_PROGRESS\"\r\n\r\n" .
    "probe\r\n" .
    "--{$colopl_bc_probe_boundary}\r\n" .
    "Content-Disposition: form-data; name=\"file1\"; filename=\"file1.txt\"\r\n" .
    "Content-Type: text/plain\r\n\r\n" .
    "1\r\n" .
    "--{$colopl_bc_probe_boundary}--\r\n";

$colopl_bc_probe_env = PHP_VERSION_ID >= 70100 ? getenv() : $_ENV;
unset($colopl_bc_probe_env['QUERY_STRING'], $colopl_bc_probe_env['PATH_TRANSLATED']);
$colopl_bc_probe_env['REDIRECT_STATUS'] = '1';
$colopl_bc_probe_env['REQUEST_METHOD'] = 'POST';
$colopl_bc_probe_env['SCRIPT_FILENAME'] = $colopl_bc_probe_script;
$colopl_bc_probe_env['CONTENT_TYPE'] = 'multipart/form-data; boundary=' . $colopl_bc_probe_boundary;
$colopl_bc_probe_env['CONTENT_LENGTH'] = (string) strlen($colopl_bc_probe_body);
$colopl_bc_probe_env['HTTP_COOKIE'] = 'PHPSESSID=colopl-bc-upload-progress-probe';

$colopl_bc_probe_ini = array(
    'display_errors=0',
    'file_uploads=1',
    'session.save_handler=files',
    'session.save_path=' . $colopl_bc_probe_dir,
    'session.name=PHPSESSID',
    'session.use_cookies=1',
    'session.use_strict_mode=0',
    'session.upload_progress.enabled=1',
    'session.upload_progress.cleanup=0',
    'session.upload_progress.prefix=upload_progress_',
    'session.upload_progress.name=PHP_SESSION_UPLOAD_PROGRESS',
    'session.upload_progress.freq=1',
    'session.upload_progress.min_freq=0.000000001',
    'colopl_bc.security.disable_session_upload_progress=0',
);
// php-cgi honours -d but not -q in CGI mode, so the output starts with headers
$colopl_bc_probe_cmd = escapeshellarg(PHP_BINARY);
foreach ($colopl_bc_probe_ini as $colopl_bc_probe_setting) {
    $colopl_bc_probe_cmd .= ' -d ' . escapeshellarg($colopl_bc_probe_setting);
}

$colopl_bc_probe_output = '';
$colopl_bc_probe_proc = proc_open(
    $colopl_bc_probe_cmd,
    array(0 => array('pipe', 'r'), 1 => array('pipe', 'w'), 2 => array('pipe', 'w')),
    $colopl_bc_probe_pipes,
    $colopl_bc_probe_dir,
    $colopl_bc_probe_env
);
if (is_resource($colopl_bc_probe_proc)) {
    fwrite($colopl_bc_probe_pipes[0], $colopl_bc_probe_body);
    fclose($colopl_bc_probe_pipes[0]);
    $colopl_bc_probe_output = stream_get_contents($colopl_bc_probe_pipes[1]);
    fclose($colopl_bc_probe_pipes[1]);
    fclose($colopl_bc_probe_pipes[2]);
    proc_close($colopl_bc_probe_proc);
}

foreach ((array) glob($colopl_bc_probe_dir . DIRECTORY_SEPARATOR . '*') as $colopl_bc_probe_file) {
    @unlink($colopl_bc_probe_file);
}
@rmdir($colopl_bc_probe_dir);

$colopl_bc_probe_parts = preg_split("/\r?\n\r?\n/", $colopl_bc_probe_output, 2);
if (trim(end($colopl_bc_probe_parts)) === 'missing') {
    die('skip this PHP build does not record session.upload_progress');
}
