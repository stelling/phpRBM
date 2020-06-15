<?php

error_reporting(E_ALL);
require('../includes/standaard.inc');
error_reporting(E_ALL);

if (date("G") == 0 or date("G") >= 7) {
	sentoutbox(1);  // Versturen van de e-mails die staan te wachten in de outbox.
}
if (date("i") <= 30) {
	(new cls_login())->uitloggen();  // Automatisch uitloggen van leden
} else {
	(new cls_login())->autounlock();  // Automatisch vrijgeven van geblokkeerde logins, dit op bases van de waarde in de instelling.
}

if ((date("G") == 0 or date("G") == 22) and date("i") <= 30) {
	sleep(5);
	(new cls_Lidond())->autogroepenbijwerken();  // Bijwerken van automatische groepen op basis van MySQL-code. 
} elseif (date("G") == 3) {
	sleep(5);
	db_backup();  // Maken van een backup
}

?>
