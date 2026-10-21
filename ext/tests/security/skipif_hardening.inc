<?php
include dirname(__FILE__) . '/skipif.inc';

ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
if (preg_match('/pointer hardening.*not applicable/', $info)) {
    die('skip session.upload_progress pointer hardening is not applicable to this PHP version');
}
