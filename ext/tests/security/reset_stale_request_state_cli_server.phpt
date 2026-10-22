--TEST--
security: the stale request state reset runs between the requests of one process (CLI server) without breaking them (colopl_bc.security.reset_stale_request_state=1)
--EXTENSIONS--
colopl_bc
--SKIPIF--
<?php
if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded');
if (php_sapi_name() !== 'cli') die('skip needs the CLI binary (php -S)');
// On Windows proc_terminate() only ends the cmd.exe wrapper; the orphaned server keeps
// the test's inherited stdout open and run-tests.php waits for it forever.
if (substr(PHP_OS, 0, 3) === 'WIN') die('skip Windows not supported');
if (!function_exists('proc_open') || !function_exists('stream_socket_server')) die('skip proc_open / sockets not available');
if (!ini_get('allow_url_fopen')) die('skip allow_url_fopen=0');
?>
--FILE--
<?php
// The built-in web server handles requests sequentially in one process, so the
// post-RSHUTDOWN hook and the wrapped sapi deactivate run between requests that
// install a user error handler, receive POST data and HTTP auth, and create
// objects: the next request must start from a clean state and work normally.
$docroot = sys_get_temp_dir() . DIRECTORY_SEPARATOR . 'colopl_bc_rsrs_' . getmypid();
mkdir($docroot);
file_put_contents($docroot . DIRECTORY_SEPARATOR . 'index.php', '<?php
set_error_handler(function ($no, $str) { echo "handler:", $str, ";"; return true; });
set_exception_handler(function ($e) { echo "exception handler"; });
class Holder { public $data; function __destruct() { $this->data = null; } }
$h = new Holder;
$h->data = str_repeat("x", 4096);
trigger_error("notice " . (isset($_POST["n"]) ? $_POST["n"] : "-"), E_USER_NOTICE);
echo "post:", count($_POST), ";auth:", isset($_SERVER["PHP_AUTH_USER"]) ? $_SERVER["PHP_AUTH_USER"] : "-", ";ok\n";
');

$socket = stream_socket_server('tcp://127.0.0.1:0');
$port = (int) substr(strrchr(stream_socket_get_name($socket, false), ':'), 1);
fclose($socket);

$cmd = escapeshellarg(PHP_BINARY) . ' -n -d display_errors=1 -d log_errors=0';
// -m instead of -r: no PHP code to quote for the shell. Load the shared
// extension only when it is not built in.
$modules = array_map('trim', explode("\n", (string) shell_exec(escapeshellarg(PHP_BINARY) . ' -n -m')));
if (!in_array('colopl_bc', $modules, true)) {
    $cmd .= ' -d extension_dir=' . escapeshellarg(ini_get('extension_dir')) . ' -d extension=colopl_bc';
}
$cmd .= ' -d colopl_bc.security.reset_stale_request_state=1 -S 127.0.0.1:' . $port . ' -t ' . escapeshellarg($docroot);
$proc = proc_open($cmd, array(0 => array('pipe', 'r'), 1 => array('pipe', 'w'), 2 => array('pipe', 'w')), $pipes, $docroot);
if (!is_resource($proc)) {
    die('proc_open failed');
}
foreach ($pipes as $pipe) {
    stream_set_blocking($pipe, false);
}

$ready = false;
for ($i = 0; $i < 100 && !$ready; $i++) {
    $conn = @stream_socket_client('tcp://127.0.0.1:' . $port, $errno, $errstr, 0.1);
    if ($conn) {
        fclose($conn);
        $ready = true;
    } else {
        usleep(50000);
    }
}
if (!$ready) {
    proc_terminate($proc);
    die('server did not start: ' . stream_get_contents($pipes[2]));
}

for ($i = 1; $i <= 3; $i++) {
    $context = stream_context_create(array('http' => array(
        'method' => 'POST',
        'header' => "Content-Type: application/x-www-form-urlencoded\r\nAuthorization: Basic " . base64_encode("user$i:pass") . "\r\n",
        'content' => http_build_query(array('n' => $i, 'b' => 'c')),
        'timeout' => 10,
    )));
    $body = file_get_contents('http://127.0.0.1:' . $port . '/index.php', false, $context);
    echo "request $i: ", $body === false ? 'FAILED' : trim($body), "\n";
}

proc_terminate($proc);
proc_close($proc);
@unlink($docroot . DIRECTORY_SEPARATOR . 'index.php');
@rmdir($docroot);
?>
--EXPECT--
request 1: handler:notice 1;post:2;auth:user1;ok
request 2: handler:notice 2;post:2;auth:user2;ok
request 3: handler:notice 3;post:2;auth:user3;ok
