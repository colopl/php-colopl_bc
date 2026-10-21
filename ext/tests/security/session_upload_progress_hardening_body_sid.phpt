--TEST--
security: session.upload_progress ignores a session ID form field when session.use_only_cookies=1 with colopl_bc.security.harden_session_upload_progress=1
--DESCRIPTION--
session_start() never takes the session ID from the request body when
session.use_only_cookies=1, but ext/session's upload progress hook does. With
the hardening the progress record keeps the ID it had before that field, so
no progress entry is written under the ID sent in the body. The field itself
still reaches $_POST.
--INI--
colopl_bc.security.harden_session_upload_progress=1
file_uploads=1
upload_max_filesize=1024
session.save_path=
session.save_handler=files
session.name=PHPSESSID
session.use_cookies=1
session.use_only_cookies=1
session.use_strict_mode=0
session.upload_progress.enabled=1
session.upload_progress.cleanup=0
session.upload_progress.prefix=upload_progress_
session.upload_progress.name=PHP_SESSION_UPLOAD_PROGRESS
session.upload_progress.freq=1
session.upload_progress.min_freq=0.000000001
--SKIPIF--
<?php include dirname(__FILE__) . '/skipif_hardening.inc'; ?>
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHPSESSID"

colopl-bc-hardening-body-sid
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"

hardening_body_sid
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file1"; filename="file1.txt"
Content-Type: text/plain

1
-----------------------------20896060251896012921717172737--
--FILE--
<?php
session_id('colopl-bc-hardening-body-sid');
session_start();
$key = ini_get('session.upload_progress.prefix') . 'hardening_body_sid';
echo 'progress: ', isset($_SESSION[$key]) ? 'present' : 'absent', "\n";
echo 'post: ', $_POST['PHPSESSID'], ' ', $_POST['PHP_SESSION_UPLOAD_PROGRESS'], "\n";
echo 'file1: ', $_FILES['file1']['name'], ' ', $_FILES['file1']['size'], "\n";
session_destroy();
?>
--EXPECT--
progress: absent
post: colopl-bc-hardening-body-sid hardening_body_sid
file1: file1.txt 1
