--TEST--
security: session.serialize_handler stays PHP_INI_ALL by default (colopl_bc.security.lock_session_serialize_handler=0)
--EXTENSIONS--
colopl_bc
--INI--
session.serialize_handler=php
--SKIPIF--
<?php include dirname(__FILE__) . '/skipif.inc'; ?>
--FILE--
<?php
var_dump(ini_set('session.serialize_handler', 'php_binary'));
var_dump(ini_get('session.serialize_handler'));
$all = ini_get_all('session', true);
var_dump($all['session.serialize_handler']['access'] === 7); // PHP_INI_ALL
var_dump(ini_get('colopl_bc.security.lock_session_serialize_handler'));
?>
--EXPECT--
string(3) "php"
string(10) "php_binary"
bool(true)
string(1) "0"
