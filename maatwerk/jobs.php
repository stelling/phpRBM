<?php

error_reporting(E_ALL);
require('../includes/standaard.inc');
error_reporting(E_ALL);

sentoutbox(1);
if (date("i") <= 30) {
	(new cls_login())->uitloggen();
} else {
	(new cls_login())->autounlock();
}


if (date("G") == 3) {
	sleep(5);
	db_backup();
}

?>
