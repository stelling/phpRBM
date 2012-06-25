<?php
	include('./includes/standaard.inc');
	// db_logboek("add", "autojobs.php", 99);
	
	if (isset($termijnvervallendiplomasmailen) and $termijnvervallendiplomasmailen > 0) {
		$ed = date('Y-m-d', mktime(0, 0, 0, date("m")+$termijnvervallendiplomasmailen, date("d"), date("Y")));
		foreach(db_logins("lijst", "", "", 0, "Login.HerinneringVervallenDiplomas=1") as $lid) {
			$query = sprintf("SELECT IFNULL(MAX(LEFT(DATE_ADD(send_on, INTERVAL %d MONTH), 10)), '2012-01-01') FROM %sMailing_hist WHERE Xtra_Char='H_DP' AND LidID=%d;", $termijnvervallendiplomasmailen, $table_prefix, $lid->lnkNummer);
			$bd = db_scalar($query);
			if ($bd < date('Y-m-d')) {
				$bd = date('Y-m-d');
			}
			
			$herdipl = "<tr><th>Code</th><th>Naam diploma</th><th>Behaald op</th><th>Vervalt op</th></tr>\n";
			foreach(db_liddipl("lidgegevens", $lid->lnkNummer) as $ld) {
				$vp = "dteVervalt per";
				if ($ld->$vp >= $bd and $ld->$vp < $ed) {
					$herdipl .= sprintf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", $ld->Code, $ld->Diploma, strftime('%e %B %Y', strtotime($ld->dteDatum)), strftime('%e %B %Y', strtotime($ld->$vp)));
				}
			}
			print("<table border=1>\n" . $herdipl . "</table>\n");
		}
	}
	
?>