<?php

error_reporting(E_ALL);
require('../includes/standaard.inc');
error_reporting(E_ALL);

if (date("G") == 0 or date("G") >= 7) {
	$res = sprintf("sentoutbox: %d verzonden", sentoutbox(1));  // Versturen van de e-mails die staan te wachten in de outbox.
} else {
	$res = "";
}
if (date("i") <= 30) {
	(new cls_login())->uitloggen();  // Automatisch uitloggen van leden
} else {
	(new cls_login())->autounlock();  // Automatisch vrijgeven van geblokkeerde logins, dit op bases van de waarde in de instelling.
}

$lb = (new cls_Logboek())->max("DatumTijd", "TypeActiviteit=3");
if ((date("G") == 0 or date("G") == 22) and date("i") <= 30) {
	sleep(5);
	$res .= sprintf(". %d automatische mutaties in leden van groepen", (new cls_Lidond())->autogroepenbijwerken());  // Automatisch bijwerken van groepen. 
} elseif ($lb < date("Y-m-d H:i:s", mktime(date("H")-1, date("i"), 0, date("m"), date("d"), date("Y"))) and (date("G") == 3 or $lb < date("Y-m-d H:i:s", mktime(date("H")-25, date("i"), 0, date("m"), date("d"), date("Y")))))  {
	sleep(5);
	if (db_backup()) {  // Maken van een backup
		$res .= ". Backup is gemaakt";
	}
}

(new cls_logboek())->add("jobs.php heeft gedraaid: " . $res, 2);

?>
