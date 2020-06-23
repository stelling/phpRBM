<?php

error_reporting(E_ALL);
require('../includes/standaard.inc');
error_reporting(E_ALL);

$res = "";
if (date("G") == 0 or date("G") >= 7) {
	// Versturen van de e-mails die staan te wachten in de outbox.
	$a = sentoutbox(1);
	if  ($a > 0) {
		$res = sprintf("sentoutbox: %d verzonden", $a);
	}
}

if (date("i") <= 30) {
	// Automatisch uitloggen van leden
	$a = (new cls_login())->uitloggen();
} else {
	// Automatisch vrijgeven van geblokkeerde logins, dit op basis van de waarde in de instelling.
	$a = (new cls_login())->autounlock();
	if ($a > 0) {
		$res .= sprintf(". %d leden zijn automatisch unlocked.", $a);
	}
}

$lb = (new cls_Logboek())->max("DatumTijd", "TypeActiviteit=3");
$lagb = (new cls_Logboek())->max("DatumTijd", "TypeActiviteit=6 AND TypeActiviteitSpecifiek=30");
if ((date("d") != date('d', strtotime($lagb)) or $lagb < date("Y-m-d H:i:s", mktime(date("H")-21, date("i"), 0, date("m"), date("d"), date("Y"))))) {
	// Automatisch bijwerken van groepen.
	sleep(5);
	(new cls_Lidond())->autogroepenbijwerken(1);
} elseif ($lb < date("Y-m-d H:i:s", mktime(date("H")-1, date("i"), 0, date("m"), date("d"), date("Y"))) and (date("G") == 3 or $lb < date("Y-m-d H:i:s", mktime(date("H")-25, date("i"), 0, date("m"), date("d"), date("Y")))))  {
	// Maken backup
	sleep(5);
	db_backup();
}

if (strlen($res) > 0) {
	(new cls_logboek())->add($res, 2);
}

?>
