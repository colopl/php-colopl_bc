--TEST--
security: session.serialize_handler cannot be changed at run time when colopl_bc.security.lock_session_serialize_handler=1
--EXTENSIONS--
colopl_bc
--INI--
colopl_bc.security.lock_session_serialize_handler=1
session.serialize_handler=php
--SKIPIF--
<?php include dirname(__FILE__) . '/skipif.inc'; ?>
--FILE--
<?php
var_dump(ini_get('session.serialize_handler'));
var_dump(@ini_set('session.serialize_handler', 'php_binary'));
var_dump(ini_get('session.serialize_handler'));
$all = ini_get_all('session', true);
var_dump($all['session.serialize_handler']['access'] === 4); // PHP_INI_SYSTEM
var_dump(ini_get('colopl_bc.security.lock_session_serialize_handler'));
?>
--EXPECT--
string(3) "php"
bool(false)
string(3) "php"
bool(true)
string(1) "1"
