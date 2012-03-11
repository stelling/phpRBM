<?php

function ToekomstigeEvenementen() {
	global $fdlang;
	
	$rv = "<ul>\n";
	foreach (db_evenement("komende") as $row) {
		$rv .= sprintf("<li>%s: %s</li>\n", strftime("%e %B", strtotime($row->Datum)), $row->Omschrijving);
	}
	$rv .= "</ul>\n";
	if (strlen($rv) < 20) {
		$rv = "geen";
	}
	return $rv;
}

function inschrijvenevenementen($lidid) {
	global $naamvereniging, $naamwebsite, $urlwebsite;

	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		foreach (db_evenement("open") as $row) {
			$ins = sprintf("ins_%d", $row->RecordID);
			$opm = sprintf("opm_%d", $row->RecordID);
			$insrow = db_evenement("inschrijving", $lidid, $row->RecordID);
			if (isset($insrow->RecordID) and $insrow->RecordID > 0) {
				if (!isset($_POST[$ins])) {
					db_evenement("deletedeelnemer", $lidid, $row->RecordID);
				} elseif ($insrow->Opmerking != $_POST[$opm]) {
					db_evenement("updatedeelnemer", $lidid, $row->RecordID, $_POST[$opm]);
				}
			} elseif (isset($_POST[$ins]) and $_POST[$ins] == "1") {
				db_evenement("adddeelnemer", $lidid, $row->RecordID, $_POST[$opm]);
			}
		}
	}
	
	fnDispMenu(2);

	echo("<div id='inschrijvingevenementen'>\n");
	printf("<form method='post' action='/index.php?%s' name='ins_evenementen'>\n", $_SERVER['QUERY_STRING']);

	$geldig = false;
	echo("<table>\n");
	echo("<tr><th>Datum</th><th>Omschrijving</th><th>Aanmelden</th><th>Opmerking</th></tr>\n");
	foreach (db_evenement("open") as $row) {
		printf("<tr><td>%s</td><td>%s</td>\n", strftime("%e %B %Y", strtotime($row->Datum)), $row->Omschrijving);
		$ins = db_evenement("inschrijving", $lidid, $row->RecordID);
		$c = "";
		$v = "&nbsp;";
		if (isset($ins->Status)) {
			$c = "checked";
			$v = "Ja";
		}
		printf("<td class='chk'><input type='checkbox' onClick='this.form.submit();' name='ins_%s' value=1 %s></input></td>\n", $row->RecordID, $c);
		if (isset($ins->Opmerking)) {
			$v = $ins->Opmerking;
		}
		printf("<td><input type='text' onBlur='this.form.submit();' name='opm_%d' size=80 maxlength=75 value=\"%s\"></td>\n", $row->RecordID, $v);
		echo("</tr>\n");
	}
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde inschrijvingevenementen -->\n");
}

function fnEvenementen() {
	global $currenttab2;
	
	fnDispMenu(2);
	
	if ($currenttab2 == "Beheer") {
		$lijst = db_evenement("overzichtbeheer");
		$lnk = sprintf("<a href='%s?tp=%s&amp;op=edit&amp;eid=%s' title='Bewerk evenement'><img src='images/zoom.png'></a>", $_SERVER['PHP_SELF'], $_GET['tp'], "%d");
		echo(fnDisplayTable($lijst, $lnk));
	}
	
}

function muteerevenement($eventid) {
	
}

?>