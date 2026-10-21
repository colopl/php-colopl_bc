--TEST--
security: session.upload_progress with cleanup=0 and no file part completes with colopl_bc.security.harden_session_upload_progress=1 (CVE-2020-7062)
--DESCRIPTION--
A body that carries the session ID and the progress key but no file part never
initialises the progress array. Before PHP 7.2.28 / 7.3.15 / 7.4.3,
MULTIPART_EVENT_END with cleanup=0 stores into it anyway and the process
crashes (bug #79221).
--INI--
colopl_bc.security.harden_session_upload_progress=1
file_uploads=1
upload_max_filesize=1024
session.save_path=
session.save_handler=files
session.name=PHPSESSID
session.use_cookies=1
session.use_strict_mode=0
session.upload_progress.enabled=1
session.upload_progress.cleanup=0
session.upload_progress.prefix=upload_progress_
session.upload_progress.name=PHP_SESSION_UPLOAD_PROGRESS
session.upload_progress.freq=1
session.upload_progress.min_freq=0.000000001
--SKIPIF--
<?php include dirname(__FILE__) . '/skipif_hardening.inc'; ?>
--COOKIE--
PHPSESSID=colopl-bc-hardening-no-file
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"

hardening_no_file
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="field1"

value1
-----------------------------20896060251896012921717172737--
--FILE--
<?php
session_start();
$key = ini_get('session.upload_progress.prefix') . 'hardening_no_file';
echo 'progress: ', isset($_SESSION[$key]) ? 'present' : 'absent', "\n";
echo 'files: ', count($_FILES), "\n";
echo 'post: ', $_POST['PHP_SESSION_UPLOAD_PROGRESS'], ' ', $_POST['field1'], "\n";
session_destroy();
?>
--EXPECT--
progress: absent
files: 0
post: hardening_no_file value1
