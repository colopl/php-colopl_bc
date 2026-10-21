--TEST--
security: session.upload_progress still takes a session ID form field when session.use_only_cookies=0 with colopl_bc.security.harden_session_upload_progress=1
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
colopl_bc.security.harden_session_upload_progress=1
file_uploads=1
upload_max_filesize=1024
session.save_path=
session.save_handler=files
session.name=PHPSESSID
session.use_cookies=1
session.use_only_cookies=0
session.use_trans_sid=0
session.use_strict_mode=0
session.upload_progress.enabled=1
session.upload_progress.cleanup=0
session.upload_progress.prefix=upload_progress_
session.upload_progress.name=PHP_SESSION_UPLOAD_PROGRESS
session.upload_progress.freq=1
session.upload_progress.min_freq=0.000000001
--SKIPIF--
<?php include dirname(__FILE__) . '/skipif_hardening.inc'; include dirname(__FILE__) . '/skipif_upload_progress.inc'; ?>
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHPSESSID"

colopl-bc-hardening-body-sid-allowed
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"

hardening_body_sid_allowed
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file1"; filename="file1.txt"
Content-Type: text/plain

1
-----------------------------20896060251896012921717172737--
--FILE--
<?php
session_id('colopl-bc-hardening-body-sid-allowed');
session_start();
$key = ini_get('session.upload_progress.prefix') . 'hardening_body_sid_allowed';
echo 'progress: ', isset($_SESSION[$key]) ? 'present' : 'absent', "\n";
echo 'file1: ', $_FILES['file1']['name'], ' ', $_FILES['file1']['size'], "\n";
session_destroy();
?>
--EXPECTF--
%Aprogress: present
file1: file1.txt 1
