<?php
if (!extension_loaded('colopl_bc')) {
    die('skip colopl_bc extension is not loaded');
}
if (!extension_loaded('session')) {
    die('skip session extension is not loaded');
}
if (PHP_VERSION_ID < 50400) {
    die('skip session.upload_progress requires PHP 5.4+');
}
