<?php

error_reporting(E_ALL);
require('../includes/standaard.inc');
error_reporting(E_ALL);

$res = "";
$lb = (new cls_Logboek())->max("DatumTijd", "TypeActiviteit=3");
$lagb = (new cls_Logboek())->max("DatumTijd", "TypeActiviteit=6 AND TypeActiviteitSpecifiek=30");

if ((strtotime($lb) < mktime(date("H")-2, date("i"), 0, date("m"), date("d"), date("Y")) and intval(date("H")) == 3) or strtotime($lb) < mktime(date("H")-24, date("i"), 0, date("m"), date("d"), date("Y"))) {
	// Maken backup
	sleep(5);
	db_backup();
	$res .= ", inclusief backup.";
} elseif (date("d") != date("d", strtotime($lagb))) {
	// Automatisch bijwerken van groepen
	sleep(5);
	$res .= ", inclusief autogroepenbijwerken.";
	(new cls_Lidond())->autogroepenbijwerken(1);
	(new cls_lidond())->auto_einde();
} else {
	
	// Automatisch bijwerken van groepen, zonder verplichte melding.
	if ((new cls_Lidond())->autogroepenbijwerken(0) > 0) {
		$res .= ", inclusief autogroepenbijwerken.";
	}
	
	// Bijwerken van eigen lijsten
	$rv = (new cls_Eigen_lijst())->controle(-1, 0, 10);
	if ($rv > 0) {
		$res .= sprintf(", inclusief controle %d eigen lijsten.", $rv);
	}
	
	// Automatisch uitloggen van leden
	$a = (new cls_login())->uitloggen();
	if ($a > 0) {
		$res .= ", inclusief het automatisch uitloggen van gebruikers.";
	}

	// Automatisch vrijgeven van geblokkeerde logins, dit op basis van de waarde in de instelling
	$a = (new cls_login())->autounlock();
	if ($a > 0) {
		$res .= sprintf(". %d leden zijn automatisch unlocked.", $a);
	}
}

if (date("G") > 7 and (date("w") != 0) or date("G") > 10) {
	// Versturen van de e-mails die staan te wachten in de outbox.
	$a = sentoutbox(2);
	if  ($a > 0) {
		$res = sprintf("sentoutbox: %d verzonden", $a);	}
}

if (strlen($res) > 0) {
	$res = "jobs.php heeft gedraaid" . $res;
	(new cls_logboek())->add($res, 2);
}

?>
