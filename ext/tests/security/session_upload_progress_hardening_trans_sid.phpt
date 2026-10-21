--TEST--
security: session.upload_progress with session.use_trans_sid=1 registers the session ID with the URL rewriter after the START reset (colopl_bc.security.harden_session_upload_progress=1)
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
report_memleaks=0
colopl_bc.security.harden_session_upload_progress=1
file_uploads=1
upload_max_filesize=1024
session.save_path=
session.save_handler=files
session.name=PHPSESSID
session.use_cookies=1
session.use_only_cookies=0
session.use_trans_sid=1
session.trans_sid_tags="a=href"
session.use_strict_mode=0
session.upload_progress.enabled=1
session.upload_progress.cleanup=0
session.upload_progress.prefix=upload_progress_
session.upload_progress.name=PHP_SESSION_UPLOAD_PROGRESS
session.upload_progress.freq=1
session.upload_progress.min_freq=0.000000001
--SKIPIF--
<?php
include dirname(__FILE__) . '/skipif_hardening.inc';
include dirname(__FILE__) . '/skipif_upload_progress.inc';
if (PHP_VERSION_ID >= 80500) die('skip session.use_only_cookies=0 is deprecated on PHP 8.5+');
?>
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHPSESSID"

colopl-bc-hardening-trans-sid
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"

hardening_trans_sid
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file1"; filename="file1.txt"
Content-Type: text/plain

1
-----------------------------20896060251896012921717172737--
--FILE--
<?php
// report_memleaks=0: PHP itself leaks the rewriter's url_app / form_app strings
// of the pre-RINIT registration when session_start() activates the rewriter
// again (debug builds report that with or without this extension).
// The upload progress handler ran php_session_initialize() -> php_session_reset_id()
// before RINIT with use_trans_sid=1, i.e. right after the hardening wrapper
// zeroed the URL rewriter state; the rewriter must have been activated from
// that clean state and must still rewrite relative URLs for the script.
session_id('colopl-bc-hardening-trans-sid');
session_start();
$key = ini_get('session.upload_progress.prefix') . 'hardening_trans_sid';
echo 'progress: ', isset($_SESSION[$key]) ? 'present' : 'absent', "\n";
echo 'file1: ', $_FILES['file1']['name'], ' ', $_FILES['file1']['size'], "\n";
echo '<a href="next.php">next</a>', "\n";
session_destroy();
?>
--EXPECTF--
%Aprogress: present
file1: file1.txt 1
<a href="next.php?PHPSESSID=colopl-bc-hardening-trans-sid%S">next</a>
