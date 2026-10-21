--TEST--
security: phpinfo() reports the session.upload_progress mitigation as inactive by default
--EXTENSIONS--
colopl_bc
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); ?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
var_dump(strpos($info, 'session.upload_progress hook mitigation => inactive') !== false);
var_dump(strpos($info, 'colopl_bc.security.disable_session_upload_progress => 0') !== false);
?>
--EXPECT--
bool(true)
bool(true)
