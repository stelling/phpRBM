<?php

$dlnstatus['I'] = "Ingeschreven";
$dlnstatus['B'] = "Bevestigd";
$dlnstatus['A'] = "Afgewezen";

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
					db_evenement("updatedeelnemer", $lidid, $row->RecordID, $_POST[$opm], $insrow->Status, $insrow->Functie);
				}
			} elseif (isset($_POST[$ins]) and $_POST[$ins] == "1") {
				db_evenement("adddeelnemer", $lidid, $row->RecordID, $_POST[$opm], "I");
			}
		}
	}
	
	fnDispMenu(2);

	echo("<div id='inschrijvingevenementen'>\n");
	printf("<form method='post' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	$geldig = false;
	echo("<table>\n");
	echo("<tr><th>Datum</th><th>Omschrijving</th><th>Aanmelden</th><th>Opmerking</th></tr>\n");
	foreach (db_evenement("open", $lidid) as $row) {
		printf("<tr><td>%s</td><td>%s</td>\n", strftime("%e %B %Y", strtotime($row->Datum)), $row->Omschrijving);
		$ins = db_evenement("inschrijving", $lidid, $row->RecordID);
		$c = "";
		$v = "&nbsp;";
		if (isset($ins->Status)) {
			$c = "checked";
			$v = "Ja";
		}
		printf("<td class='chk'><input type='checkbox' onClick='this.form.submit();' name='ins_%s' value=1 %s></td>\n", $row->RecordID, $c);
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
	
	if (isset($_GET['op']) and strlen($_GET['op']) > 0) {
		$op = $_GET['op'];
	} else {
		$op = "overzicht";
	}
	if (isset($_GET['eid']) and strlen($_GET['eid']) > 0) {
		$eid = $_GET['eid'];
	} else {
		$eid = 0;
	}
	
	fnDispMenu(2);
	
	if ($currenttab2 == "Beheer" and $op == "edit") {
		muteerevenement($eid);
	} elseif ($currenttab2 == "Beheer" and $op == "delete") {
		db_evenement("deletedeelnemer", $_GET['lidid'], $_GET['eid']);
		muteerevenement($eid);
	} elseif ($currenttab2 == "Beheer") {
		$lijst = db_evenement("overzicht");
		$lnk = sprintf("<a href='%s?tp=%s&amp;op=edit&amp;eid=%s' title='Bewerk evenement'>%s</a>", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), "%d", "%s");
		$er = sprintf("<td><a href='%s?%s&amp;op=edit&amp;eid=0' title='Nieuw evenement'><img src='images/star.png' alt='Nieuw'></td><td colspan=6>Nieuw evenement</td>", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
		echo(fnDisplayTable($lijst, $lnk, "", 0, "", $er));
	} else {
		$currenttab2 = "Overzicht";
		overzichtevenementen();
	}
	
}

function muteerevenement($eventid) {
	global $dlnstatus, $table_prefix;

	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$_POST['datum'] = change_month_to_uk($_POST['datum']);
		if (strtotime($_POST['datum']) === FALSE) {
			$datum = date('Y-m-d');
		} else {
			$datum = date('Y-m-d', strtotime($_POST['datum']));
		}
		
		if ($eventid > 0) {
			$row = db_evenement("record", 0, $eventid);
			$set_clause = "";
		} else {
			$set_clause = "Ingevoerd=CURDATE(), ";
		}
		
		if ($eventid == 0 or $row->Datum != $datum) {
			$set_clause .= sprintf("Datum='%s', ", $datum);
		}
		if ($eventid == 0 or $row->Omschrijving != $_POST['omschrijving']) {
			$set_clause .= sprintf("Omschrijving='%s', ", $_POST['omschrijving']);
		}
		if ($eventid == 0 or $row->Starttijd != $_POST['starttijd']) {
			$set_clause .= sprintf("Starttijd='%s', ", $_POST['starttijd']);
		}
		if ($eventid == 0 or $row->Eindtijd != $_POST['eindtijd']) {
			$set_clause .= sprintf("Eindtijd='%s', ", $_POST['eindtijd']);
		}
		if ($eventid == 0 or $row->Verzameltijd != $_POST['verzameltijd']) {
			$set_clause .= sprintf("Verzameltijd='%s', ", $_POST['verzameltijd']);
		}
		if ($eventid == 0 or $row->TypeEvenement != $_POST['typeevenement']) {
			$set_clause .= sprintf("TypeEvenement='%s', ", $_POST['typeevenement']);
		}
		if ($eventid == 0 or $row->InschrijvingOpen != $_POST['insopen']) {
			$set_clause .= sprintf("InschrijvingOpen=%d, ", $_POST['insopen']);
		}
		if ($eventid == 0 or $row->BeperkTotGroep != $_POST['beperktotgroep']) {
			$set_clause .= sprintf("BeperkTotGroep=%d, ", $_POST['beperktotgroep']);
		}
		if (strlen($set_clause) > 0) {
			$set_clause .= sprintf("GewijzigdDoor=%d", $_SESSION['lidid']);
			if ($eventid > 0) {
				$query = sprintf("UPDATE %sEvenement SET %s WHERE RecordID=%d;", $table_prefix, $set_clause, $eventid);
				fnQuery($query);
			} else {
				$query = sprintf("INSERT INTO %sEvenement SET %s;", $table_prefix, $set_clause);
				$eventid = fnQuery($query);
			}
		}
		foreach (db_evenement("deelnemers", 0, $eventid) as $rd) {
			$stat = sprintf("stat_%d", $rd->RecordID);
			$func = sprintf("func_%d", $rd->RecordID);
			if ((isset($_POST[$stat]) and $_POST[$stat] != $rd->Status) or (isset($_POST[$func]) and $_POST[$func] != $rd->Functie)) {
				db_evenement("updatedeelnemer", $rd->LidID, $eventid, $rd->Opmerking, $_POST[$stat], $_POST[$func]);
			}
		}
		if (isset($_POST['nwdln']) and $_POST['nwdln'] > 0 and is_numeric($_POST['nwdln'])) {
			db_evenement("adddeelnemer", $_POST['nwdln'], $eventid, "", "B");
		}
	}
	
	if ($eventid > 0) {
		$row = db_evenement("record", 0, $eventid);
		$recordid = $row->RecordID;
		$datum = $row->Datum;
		$oms = $row->Omschrijving;
		$starttijd = $row->Starttijd;
		$eindtijd = $row->Eindtijd;
		$verzameltijd = $row->Verzameltijd;
		$typeevenement = $row->TypeEvenement;
		if ($row->InschrijvingOpen == 1) {
			$insopen1 = "checked='checked'";
			$insopen2 = "";
		} else {
			$insopen1 = "";
			$insopen2 = "checked='checked'";
		}
		$beperktotgroep = $row->BeperkTotGroep;
	} else {
		$recordid = 0;
		$datum = date("Y-m-d");
		$oms = "";
		$starttijd = "";
		$eindtijd = "";
		$verzameltijd = "";
		$typeevenement = 1;
		$insopen1 = "checked='checked'";
		$insopen2 = "";
		$beperktotgroep = 0;
	}
	
	$optiontypes = "";
	foreach (db_evenement("types") as $t) {
		if ($t->RecordID == $typeevenement) {
			$s = "selected";
		} else {
			$s = "";
		}
		$optiontypes .= sprintf("<option value=%d %s>%s</option>\n", $t->RecordID, $s, $t->Omschrijving);
	}
	
	$optiongroepen = "<option value=0>Geen</option>\n";
	foreach (db_onderdelen() as $t) {
		if ($t->RecordID == $beperktotgroep) {
			$s = "selected";
		} else {
			$s = "";
		}
		$optiongroepen .= sprintf("<option value=%d %s>%s</option>\n", $t->RecordID, $s, $t->Oms);
	}
	
	echo("<div id='evenementenbeheer'>\n");
	printf("<form method='post' action='%s?tp=%s&amp;op=edit&amp;eid=%d'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $eventid);
	echo("<table>\n");
	printf("<tr><td class='label'>Datum:</td><td colspan=5><input type='text' name='datum' value='%s' size=50 onBlur='this.form.submit();'></td></tr>\n", strftime("%e %B %Y", strtotime($datum)));
	printf("<tr><td class='label'>Omschrijving:</td><td colspan=5><input type='text' name='omschrijving' value='%s' maxlength=75 size=100 onBlur='this.form.submit();'></td></tr>\n", $oms);
	printf("<tr><td class='label'>Starttijd:</td><td><input type='text' name='starttijd' value='%s' size=5 maxlength=5 onBlur='this.form.submit();'></td>\n", $starttijd);
	printf("<td class='label'>Eindtijd:</td><td><input type='text' name='eindtijd' value='%s' size=5 maxlength=5 onBlur='this.form.submit();'></td>\n", $eindtijd);
	printf("<td class='label'>Verzamelen:</td><td><input type='text' name='verzameltijd' value='%s' size=5 maxlength=5 onBlur='this.form.submit();'></td></tr>\n", $verzameltijd);
	printf("<tr><td class='label'>Type evenement:</td><td colspan=5><select name='typeevenement' onChange='this.form.submit();'>%s</select></td></tr>\n", $optiontypes);
	printf("<tr><td class='label'>Inschrijving is open:</td><td colspan=5><input type='radio' name='insopen' value=1 %s>Ja<input type='radio' name='insopen' value=0 %s>Nee</td></tr>\n", $insopen1, $insopen2);
	printf("<tr><td class='label'>Beperkt tot groep:</td><td colspan=5><select name='beperktotgroep' onChange='this.form.submit();'>%s</select></td></tr>\n", $optiongroepen);
	if ($eventid > 0) {
		printf("<tr><td class='label'>Gewijzigd op / door:</td><td colspan=5>%s / %s</td></tr>\n", strftime("%e %B %Y (%H:%M)", strtotime($row->Gewijzigd)), $row->GewijzigdDoorNaam);	
	}
	echo("</table>\n");
	
	if ($eventid > 0) {
		echo("<table>");
		echo("<tr><th colspan=5 class='tabelkop'>Deelnemers</th></tr>\n");
		echo("<tr><th>Naam deelnemer</th><th>Status</th><th>Opmerking</th><th>Functie</th><th>Del</th></tr>\n");
		foreach (db_evenement("deelnemers", 0, $recordid) as $rd) {
			$optstat = "";
			foreach ($dlnstatus as $key => $val) {
				$s = "";
				if ($key == $rd->Status) {
					$s = "selected";
				}
				$optstat .= sprintf("<option value='%s' %s>%s</option>\n", $key, $s, $val);
			}
			echo("<tr>\n");
			printf("<td>%s</td>\n", $rd->NaamDeelnemer);
			printf("<td><select name='stat_%d' onChange='this.form.submit();'>%s</select></td>\n", $rd->RecordID, $optstat);
			printf("<td>%s</td>\n", $rd->Opmerking);
			printf("<td><input type='text' name='func_%d' value='%s' size=30 maxlength=20 onBlur='this.form.submit();'></td>\n", $rd->RecordID, $rd->Functie);
			printf("<td><a href='%s?tp=%s&amp;op=delete&amp;eid=%d&amp;lidid=%d'><image src='images/del.png' alt='Delete'></a></td>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $eventid, $rd->LidID);
			echo("</tr>\n");
		}
		$optionsnieuw = "<option value=0>Nieuwe deelnemer ...</option>\n";
		foreach (db_evenement("potdeelnemers", 0, $recordid, "", "", "", $beperktotgroep) as $row) {
			$optionsnieuw .= sprintf("<option value=%d>%s</option>\n", $row->Nummer, $row->Naam);
		}
		printf("<tr>\n<td colspan=5><select name='nwdln' onChange='this.form.submit();'>%s</select></td>\n</tr>\n", $optionsnieuw);
	
		echo("</table>\n");
	}
	echo("</form>\n");
	echo("</div> <!-- Einde evenementenbeheer -->\n");
}

function overzichtevenementen() {

	$lijst = db_evenement("overzicht");
	echo("<div id='overzichtevenementen'>\n");
	echo("<table>\n");
	echo("<tr><th>Datum en tijden</th><th>Omschrijving</th><th>Dames</th><th>Heren</th><th>Type evenement</th></tr>\n");
	
	foreach ($lijst as $row) {
		if (strlen($row->Starttijd) > 3) {
			$td = "<br>Start: " . $row->Starttijd;
		} else {
			$td = "";
		}
		if (strlen($row->Eindtijd) > 3) {
			$td .= "<br>Einde: " . $row->Eindtijd;
		}
		if (strlen($row->Verzameltijd) > 3) {
			$td .= "<br>Verzamelen: " . $row->Verzameltijd;
		}
		$dames = "";
		$heren = "";
		foreach (db_evenement("deelnemers", 0, $row->lnkNummer, "", "B") as $deeln) {
			if ($deeln->Geslacht == "V") {
				$dames .= $deeln->NaamDeelnemer . "<br>\n";
			} else {
				$heren .= $deeln->NaamDeelnemer . "<br>\n";
			}
		}
		$te = "Type evenement";
		printf("<tr><td>%s %s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", strftime("%e %B %Y", strtotime($row->Datum)), $td, $row->Omschrijving, $dames, $heren, $row->$te);
	}
	
	echo("</table>\n");
	echo("</div>  <!-- Einde overzichtevenementen -->\n");

}
?>