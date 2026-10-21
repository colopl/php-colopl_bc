--TEST--
security: phpinfo() reports the session.upload_progress mitigation as active
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.disable_session_upload_progress=1
--SKIPIF--
<?php include dirname(__FILE__) . '/skipif.inc'; ?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
var_dump(strpos($info, 'colopl_bc security backports') !== false);
var_dump(strpos($info, 'session.upload_progress hook mitigation => active (php_rfc1867_callback detached)') !== false);
var_dump(strpos($info, 'colopl_bc.security.disable_session_upload_progress => 1') !== false);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
