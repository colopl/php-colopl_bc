--TEST--
Check for COLOPL PHP Backward Compatibility Extension availability.
--FILE--
<?php
echo extension_loaded('colopl_bc')
    ? 'COLOPL PHP Backward Compatibility Extension is available.'
    : 'COLOPL PHP Backward Compatibility Extension is NOT available.'
;
?>
--EXPECT--
COLOPL PHP Backward Compatibility Extension is available.
