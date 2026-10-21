--TEST--
security: session.upload_progress does not save the session for a progress key the "php" serializer cannot encode with colopl_bc.security.harden_session_upload_progress=1
--DESCRIPTION--
A progress key containing "|" makes php_session_encode() fail on every
update(), and ext/session then saves the session as an empty string, wiping
the data stored under that ID. With the hardening such a key is not tracked,
so the session store is not written during the upload.
--INI--
colopl_bc.security.harden_session_upload_progress=1
file_uploads=1
upload_max_filesize=1024
session.save_path=
session.save_handler=files
session.serialize_handler=php
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
PHPSESSID=colopl-bc-hardening-unencodable
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"

a|b
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file1"; filename="file1.txt"
Content-Type: text/plain

1
-----------------------------20896060251896012921717172737--
--FILE--
<?php
$dir = session_save_path() !== '' ? session_save_path() : sys_get_temp_dir();
$file = $dir . DIRECTORY_SEPARATOR . 'sess_colopl-bc-hardening-unencodable';
echo 'session file written: ', var_export(file_exists($file), true), "\n";
echo 'post: ', $_POST['PHP_SESSION_UPLOAD_PROGRESS'], "\n";
echo 'file1: ', $_FILES['file1']['name'], ' ', $_FILES['file1']['size'], "\n";
?>
--CLEAN--
<?php
$dir = session_save_path() !== '' ? session_save_path() : sys_get_temp_dir();
@unlink($dir . DIRECTORY_SEPARATOR . 'sess_colopl-bc-hardening-unencodable');
?>
--EXPECT--
session file written: false
post: a|b
file1: file1.txt 1
