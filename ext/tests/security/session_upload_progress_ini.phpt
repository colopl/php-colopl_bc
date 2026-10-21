--TEST--
security: colopl_bc.security.disable_session_upload_progress is PHP_INI_SYSTEM
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.disable_session_upload_progress=1
--SKIPIF--
<?php if (!extension_loaded('colopl_bc')) die('skip colopl_bc extension is not loaded'); ?>
--FILE--
<?php
var_dump(ini_get('colopl_bc.security.disable_session_upload_progress'));
var_dump(ini_set('colopl_bc.security.disable_session_upload_progress', '0'));
var_dump(ini_get('colopl_bc.security.disable_session_upload_progress'));
?>
--EXPECT--
string(1) "1"
bool(false)
string(1) "1"
