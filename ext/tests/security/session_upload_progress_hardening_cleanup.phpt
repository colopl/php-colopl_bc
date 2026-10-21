--TEST--
security: session.upload_progress.cleanup=1 still removes the progress entry with colopl_bc.security.harden_session_upload_progress=1
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
session.upload_progress.cleanup=1
session.upload_progress.prefix=upload_progress_
session.upload_progress.name=PHP_SESSION_UPLOAD_PROGRESS
session.upload_progress.freq=1
session.upload_progress.min_freq=0.000000001
--SKIPIF--
<?php include dirname(__FILE__) . '/skipif_hardening.inc'; ?>
--COOKIE--
PHPSESSID=colopl-bc-hardening-cleanup
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"

hardening_cleanup
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file1"; filename="file1.txt"
Content-Type: text/plain

1
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file2"; filename="file2.txt"
Content-Type: text/plain

22
-----------------------------20896060251896012921717172737--
--FILE--
<?php
session_start();
$key = ini_get('session.upload_progress.prefix') . 'hardening_cleanup';
echo 'progress: ', isset($_SESSION[$key]) ? 'present' : 'absent', "\n";
foreach ($_FILES as $name => $file) {
    echo $name, ': ', $file['name'], ' ', $file['size'], "\n";
}
echo 'post: ', $_POST['PHP_SESSION_UPLOAD_PROGRESS'], "\n";
session_destroy();
?>
--EXPECT--
progress: absent
file1: file1.txt 1
file2: file2.txt 2
post: hardening_cleanup
