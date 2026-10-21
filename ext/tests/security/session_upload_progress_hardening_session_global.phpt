--TEST--
security: $_SESSION populated by the upload progress handler before RINIT is dropped at the end of the body
--INI--
colopl_bc.security.harden_session_upload_progress=1
file_uploads=1
upload_max_filesize=1024
session.save_path=
session.save_handler=files
session.name=PHPSESSID
session.use_cookies=1
session.upload_progress.enabled=1
session.upload_progress.cleanup=0
session.upload_progress.prefix=upload_progress_
session.upload_progress.name=PHP_SESSION_UPLOAD_PROGRESS
session.upload_progress.freq=1
session.upload_progress.min_freq=0.000000001
--SKIPIF--
<?php include dirname(__FILE__) . '/skipif_hardening.inc'; ?>
--COOKIE--
PHPSESSID=colopl-bc-secfix-global
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"

global
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file1"; filename="file1.txt"
Content-Type: text/plain

1
-----------------------------20896060251896012921717172737--
--FILE--
<?php
// before session_start(): nothing left over from the pre-RINIT update
// (checked before any output, which would prevent session_start() under CGI)
$before = isset($_SESSION);
session_start();
$key = ini_get('session.upload_progress.prefix') . 'global';
var_dump($before);
var_dump(isset($_SESSION[$key]));
var_dump($_SESSION[$key]['done']);
session_destroy();
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
