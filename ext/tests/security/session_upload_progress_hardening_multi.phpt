--TEST--
security: session.upload_progress keeps tracking every file with colopl_bc.security.harden_session_upload_progress=1
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
<?php include dirname(__FILE__) . '/skipif_hardening.inc'; include dirname(__FILE__) . '/skipif_upload_progress.inc'; ?>
--COOKIE--
PHPSESSID=colopl-bc-hardening-multi
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"

hardening_multi
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file1"; filename="file1.txt"
Content-Type: text/plain

1
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file2"; filename="file2.txt"
Content-Type: text/plain

22
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file3"; filename="file3.txt"
Content-Type: text/plain

333
-----------------------------20896060251896012921717172737--
--FILE--
<?php
session_start();
$key = ini_get('session.upload_progress.prefix') . 'hardening_multi';
$progress = $_SESSION[$key];
echo 'done: ', var_export($progress['done'], true), "\n";
echo 'bytes_processed is the total read: ', var_export($progress['bytes_processed'] === $progress['content_length'], true), "\n";
echo 'files: ', count($progress['files']), "\n";
foreach ($progress['files'] as $i => $file) {
    echo $i, ': ', $file['field_name'], ' ', $file['name'], ' ', $file['bytes_processed'], ' ',
        var_export($file['done'], true), ' ', $file['error'], ' ', var_export(is_string($file['tmp_name']), true), "\n";
}
foreach ($_FILES as $name => $file) {
    echo $name, ': ', $file['name'], ' ', $file['size'], "\n";
}
session_destroy();
?>
--EXPECT--
done: true
bytes_processed is the total read: true
files: 3
0: file1 file1.txt 1 true 0 true
1: file2 file2.txt 2 true 0 true
2: file3 file3.txt 3 true 0 true
file1: file1.txt 1
file2: file2.txt 2
file3: file3.txt 3
