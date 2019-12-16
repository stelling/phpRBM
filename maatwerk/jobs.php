<?php

error_reporting(E_ALL);
require('./includes/standaard.inc');
error_reporting(E_ALL);

sentoutbox(1);

if (date("G") == 3) {
	sleep(5);
	db_backup();
}

?>
