<?php
include dirname(__FILE__) . '/skipif.inc';

ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
if (preg_match('/pointer hardening.*not applicable/', $info)) {
    die('skip session.upload_progress pointer hardening is not applicable to this PHP version');
}
if (preg_match('/pointer hardening.*backported fix detected/', $info)) {
    die('skip session.upload_progress pointer hardening is not applied: this PHP build carries a backport of the 2026-10-21 fix');
}
