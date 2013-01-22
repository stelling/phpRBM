<?php

$dlnstatus['A'] = "Afgewezen";
$dlnstatus['B'] = "Bevestigd";
$dlnstatus['I'] = "Ingeschreven";
$dlnstatus['N'] = "Niet geweest";
$dlnstatus['R'] = "Reserve";
$dlnstatus['T'] = "Onder voorbehoud";
$dlnstatus['X'] = "Afgezegd";

$soortevenement['B'] = "Brigade-activiteit";
$soortevenement['G'] = "Gezellig/sociaal";
$soortevenement['W'] = "Wedstrijd";

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
		$lijst = db_evenement("overzichtbeheer");
		$lnk = sprintf("<a href='%s?tp=%s&amp;op=edit&amp;eid=%%d' title='Muteer evenement'><img src='images/edit.png' alt='Muteren'></a>", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
		if (count(db_evenement("types")) > 0) {
			$er = sprintf("<td><a href='%s?%s&amp;op=edit&amp;eid=0' title='Nieuw evenement'><img src='images/star.png' alt='Nieuw'></a></td><td colspan=10>Nieuw evenement</td>", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
		} else {
			$er = "";
		}
		echo(fnDisplayTable($lijst, $lnk, "", 0, "", $er));
	} elseif ($currenttab2 == "Logboek") {
		$lijst = db_logboek("lijst", "", 7);
		echo(fnDisplayTable($lijst, "", "", 0, "", "", "logboek"));
	} elseif ($currenttab2 == "Types muteren") {
		muteertypeevenement();
	} else {
		$currenttab2 = "Overzicht";
		overzichtevenementen();
	}
	
}

function ToekomstigeEvenementen() {
	global $fdlang;
	
	$rv = "";
	foreach (db_evenement("komende") as $row) {
		$dat = "Op " . strftime("%A %e %B", strtotime($row->Datum));
		if ($row->Datum == date("Y-m-d")) {
			$dat = "Vandaag";
		} elseif ($row->Datum == date("Y-m-d", mktime(0, 0, 0, date("m")  , date("d")+1, date("Y")))) {
			$dat = "Morgen";
		}
		$rv .= sprintf("<p>%s %s</p>\n", $dat, $row->Omschrijving);
	}
	if (strlen($rv) > 0) {
		$rv .= "\n";
	}
	return $rv;
}

function inschrijvenevenementen($lidid) {

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
				db_evenement("adddeelnemer", $lidid, $row->RecordID, $_POST[$opm], $row->StandaardStatus);
			}
		}
	}
	
	fnDispMenu(2);

	echo("<div id='inschrijvingevenementen'>\n");
	printf("<form method='post' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	$geldig = false;
	echo("<table>\n");
	echo("<tr><th>Datum</th><th>Starttijd</th><th>Omschrijving</th><th>Aanmelden</th><th>Opmerking</th></tr>\n");
	foreach (db_evenement("open", $lidid) as $row) {
		printf("<tr><td>%s</td><td>%s</td><td>%s</td>\n", strftime("%e %B %Y", strtotime($row->Datum)), strftime("%H:%M", strtotime($row->Datum)), $row->Omschrijving);
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

function muteerevenement($eventid) {
	global $dlnstatus, $table_prefix;

	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$_POST['Datum'] = change_month_to_uk($_POST['Datum']);
		if (strtotime($_POST['Datum']) === FALSE) {
			$_POST['Datum'] = date('Y-m-d');
		} else {
			$_POST['Datum'] = date('Y-m-d', strtotime($_POST['Datum']));
		}
		if (isset($_POST['Starttijd']) and strlen($_POST['Starttijd']) == 5) {
			$_POST['Datum'] .= " " . $_POST['Starttijd'] . ":00";
		}
		
		if ($eventid == 0) {
			$eventid = db_evenement("add");
		}
		
		$old = db_evenement("record", 0, $eventid);
		$set_clause = "";
		
		$setclause = ""; 
		$oms = "";
		foreach($_POST as $key => $val) {
			if (array_key_exists($key, $old) and $old->$key != $_POST[$key]) {
				if (is_numeric($_POST[$key])) {
					$set_clause .= sprintf("%s=%d, ", $key, $_POST[$key]);
					$oms .= sprintf("Veld '%s' is %d geworden. ", $key, $_POST[$key]);
				} else {
					$_POST[$key] = str_replace("\"", "'", $_POST[$key]);
					$set_clause .= sprintf("%s=\"%s\", ", $key, $_POST[$key]);
					$oms .= sprintf("Veld '%s' is '%s' geworden. ", $key, $_POST[$key]);
				}
			}
		}
		
		if (isset($_POST['verw'])) {
			if ($old->VerwijderdOp <= '2012-01-01' and $_POST['verw'] == 1) {
				$set_clause .= "VerwijderdOp=CURDATE(), ";
				$oms .= "Het evenement is verwijderd. ";
			} elseif ($old->VerwijderdOp > '2012-01-01' and $_POST['verw'] == 0) {
				$set_clause .= "VerwijderdOp='0000-00-00', ";
				$oms .= "De verwijdering is ongedaan gemaakt. ";
			}
		}
		if (strlen($set_clause) > 0) {
			$set_clause .= sprintf("GewijzigdDoor=%d", $_SESSION['lidid']);
			$query = sprintf("UPDATE %sEvenement SET %s WHERE RecordID=%d;", $table_prefix, $set_clause, $eventid);
			$result = fnQuery($query);
			if ($result > 0) {
				$oms = sprintf("In evenement %d zijn de volgende velden aangepast. %s", $eventid, $oms);
				db_logboek("add", $oms, 7, 0, 0, $eventid);
			}
		}
		foreach (db_evenement("deelnemers", 0, $eventid) as $rd) {
			$stat = sprintf("stat_%d", $rd->RecordID);
			$opm = sprintf("Opmerking_%d", $rd->RecordID);
			$func = sprintf("func_%d", $rd->RecordID);
			$result = db_evenement("updatedeelnemer", $rd->LidID, $eventid, $_POST[$opm], $_POST[$stat], $_POST[$func]);
			if ($result > 0) {
				$mess = sprintf("Deelnemer %s is bij evenement %d (%s) bijgewerkt. ", db_naamlid($rd->LidID), $eventid, db_evenement("oms", 0, $eventid));
				if ($_POST[$stat] != $rd->Status) {
					$mess .= sprintf("Status is '%s' geworden. ", $dlnstatus[$_POST[$stat]]);
				}
				if ($_POST[$func] != $rd->Functie) {
					$mess .= sprintf("Functie is '%s' geworden. ", $_POST[$func]);
				}
				if ($_POST[$opm] != $rd->Opmerking) {
					$mess .= sprintf("Opmerking is '%s' geworden.", $_POST[$opm]);
				}
				db_logboek("add", $mess, 7, 0, 0, $eventid);
			}
		}
		if (isset($_POST['nwdln']) and $_POST['nwdln'] > 0 and is_numeric($_POST['nwdln'])) {
			db_evenement("adddeelnemer", $_POST['nwdln'], $eventid, "", $_POST['StandaardStatus']);
		}
		
		if (isset($_POST['maildeeln']) or isset($_POST['mailpotdeeln']) or isset($_POST['mailbeidedeeln'])) {
			$mid = db_mailing("add", 0, 0, "", $_POST['Omschrijving'], 0, $_POST['Email'], "Deelnemers " . $_POST['Omschrijving']);
			if (isset($_POST['maildeeln']) or isset($_POST['mailbeidedeeln'])) {
				foreach (db_evenement("deelnemers", 0, $eventid, "", "'B', 'I', 'R', 'T'") as $row) {
					db_mailing("addrcpt", $mid, 0, "", "", $row->LidID);
				}
			} 
			if (isset($_POST['mailpotdeeln']) or isset($_POST['mailbeidedeeln'])) {
				foreach (db_evenement("potdeelnemers", 0, $eventid) as $row) {
					db_mailing("addrcpt", $mid, 0, "", "", $row->Nummer);
				}
			}
			printf("<script>location.href='%s?tp=Mailing/Concepten&op=edit&mid=%d'</script>\n", $_SERVER['PHP_SELF'], $mid);
		}
	}
	
	if ($eventid > 0) {
		$row = db_evenement("record", 0, $eventid);
		$recordid = $row->RecordID;
		$datum = date("Y-m-d", strtotime($row->Datum));
		$oms = $row->Omschrijving;
		$email = $row->Email;
		$starttijd = date("H:i", strtotime($row->Datum));
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
		$standaardstatus = $row->StandaardStatus;
		if ($row->VerwijderdOp >= '2012-01-01') {
			$verw1 = "checked='checked'";
			$verw2 = "";
		} else {
			$verw1 = "";
			$verw2 = "checked='checked'";
		}
		$beperktotgroep = $row->BeperkTotGroep;
		$zichtbaarvoor = $row->ZichtbaarVoor;
		$aantdln = $row->AantDln;
	} else {
		$recordid = 0;
		$datum = date("Y-m-d");
		$oms = "";
		$email = "";
		$starttijd = "";
		$eindtijd = "";
		$verzameltijd = "";
		$typeevenement = 1;
		$insopen1 = "checked='checked'";
		$insopen2 = "";
		$standaardstatus = "I";
		$beperktotgroep = 0;
		$zichtbaarvoor = 0;
		$aantdln = 0;
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
	
	$optionstandstatus = "";
	foreach ($dlnstatus as $ds => $o) {
		if ($ds == $standaardstatus) {
			$s = "selected";
		} else {
			$s = "";
		}
		$optionstandstatus .= sprintf("<option value='%s' %s>%s</option>\n", $ds, $s, $o);
	}
	
	echo("<div id='evenementenbeheer'>\n");
	printf("<form method='post' action='%s?tp=%s&amp;op=edit&amp;eid=%d'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $eventid);
	echo("<table>\n");
	printf("<tr><td class='label'>Datum:</td><td colspan=5><input type='text' name='Datum' value='%s' size=25></td></tr>\n", strftime("%e %B %Y", strtotime($datum)));
	printf("<tr><td class='label'>Omschrijving:</td><td colspan=5><input type='text' name='Omschrijving' value='%s' maxlength=50 size=50></td></tr>\n", $oms);
	printf("<tr><td class='label'>E-mail:</td><td colspan=5><input type='email' name='Email' value='%s' maxlength=45 size=50></td></tr>\n", $email);
	printf("<tr><td class='label'>Starttijd:</td><td><input type='text' name='Starttijd' value='%s' size=5 maxlength=5></td>\n", $starttijd);
	printf("<td class='label'>Eindtijd:</td><td><input type='text' name='Eindtijd' value='%s' size=5 maxlength=5></td>\n", $eindtijd);
	printf("<td class='label'>Verzamelen:</td><td><input type='text' name='Verzameltijd' value='%s' size=5 maxlength=5></td></tr>\n", $verzameltijd);
	printf("<tr><td class='label'>Type evenement:</td><td colspan=3><select name='TypeEvenement' onChange='this.form.submit();'>%s</select></td>\n", $optiontypes);
	printf("<td class='label'>Aantal deelnemers:</td><td>%d</td></tr>\n", $aantdln);
	if ($eventid > 0) {
		printf("<tr><td class='label'>Online inschrijving open:</td><td><input type='radio' name='InschrijvingOpen' value=1 %s>Ja<input type='radio' name='InschrijvingOpen' value=0 %s>Nee</td>\n", $insopen1, $insopen2);
		printf("<td class='label'>Standaard status:</td><td><select name='StandaardStatus'>%s</select></td>\n", $optionstandstatus);
		printf("<td class='label'>Verwijderd:</td><td><input type='radio' name='verw' value=1 %s>Ja<input type='radio' name='verw' value=0 %s>Nee</td></tr>\n", $verw1, $verw2);
		$optiongroepen = "<option value=0>Geen</option>\n";
		foreach (db_onderdelen() as $t) {
			if ($t->RecordID == $beperktotgroep) {
				$s = "selected";
			} else {
				$s = "";
			}
			$optiongroepen .= sprintf("<option value=%d %s>%s</option>\n", $t->RecordID, $s, htmlentities($t->Oms));
		}
		printf("<tr><td class='label'>Deelname beperken tot:</td><td colspan=2><select name='BeperkTotGroep' onChange='this.form.submit();'>%s</select></td>\n", $optiongroepen);
		$optiongroepen = "<option value=0>Iedereen</option>\n";
		foreach (db_onderdelen() as $t) {
			if ($t->RecordID == $zichtbaarvoor) {
				$s = "selected";
			} else {
				$s = "";
			}
			$optiongroepen .= sprintf("<option value=%d %s>%s</option>\n", $t->RecordID, $s, htmlentities($t->Oms));
		}
		printf("<td class='label'>Zichtbaar voor:</td><td colspan=2><select name='ZichtbaarVoor'>%s</select></td></tr>\n", $optiongroepen);
		printf("<tr><td class='label'>Gewijzigd op / door:</td><td colspan=5>%s / %s</td></tr>\n", strftime("%e %B %Y (%H:%M)", strtotime($row->Gewijzigd)), htmlentities($row->GewijzigdDoorNaam));	
	}
	echo("</table>\n");
	
	$aantdln = 0;
	echo("<table>");
	if ($eventid > 0) {
		echo("<tr><th colspan=6 class='tabelkop'>Deelnemers</th></tr>\n");
		echo("<tr><th>Naam deelnemer</th><th>Status</th><th>Opmerking bij inschrijving</th><th>Functie</th><th>Toegevoegd</th><th>Del</th></tr>\n");
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
			printf("<td>%s</td>\n", htmlentities($rd->NaamDeelnemer));
			printf("<td><select name='stat_%d' onChange='this.form.submit();'>%s</select></td>\n", $rd->RecordID, $optstat);
			printf("<td><input type='text' name='Opmerking_%d' value='%s' size=30 maxlength=75></td>\n", $rd->RecordID, $rd->Opmerking);
			printf("<td><input type='text' name='func_%d' value='%s' size=30 maxlength=20></td>\n", $rd->RecordID, $rd->Functie);
			printf("<td>%s</td>\n", strftime("%e %b %Y", strtotime($rd->Ingevoerd)));
			printf("<td><a href='%s?tp=%s&amp;op=delete&amp;eid=%d&amp;lidid=%d'><img src='images/del.png' alt='Delete'></a></td>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $eventid, $rd->LidID);
			echo("</tr>\n");
			$aantdln++;
		}
		$optionsnieuw = "<option value=0>Nieuwe deelnemer ...</option>\n";
		foreach (db_evenement("potdeelnemers", 0, $recordid) as $row) {
			$optionsnieuw .= sprintf("<option value=%d>%s</option>\n", $row->Nummer, htmlentities($row->Naam));
		}
		printf("<tr>\n<td colspan=6><select name='nwdln' onChange='this.form.submit();'>%s</select></td>\n</tr>\n", $optionsnieuw);
	}
	echo("<tr><th colspan=6>");
	echo("<input type=submit name='Opslaan' value='Opslaan'>\n");
	if ($aantdln > 1) {
		echo("<input type=submit name='maildeeln' value='Mailing deelnemers'>\n");
	}
	if ($eventid > 0) {
		echo("<input type=submit name='mailpotdeeln' value='Mailing potenti&euml;le deelnemers'>\n");
	}
	if ($aantdln > 1) {
		echo("<input type=submit name='mailbeidedeeln' value='Mailing (potenti&euml;le) deelnemers'>\n");
	}
	echo("</th></tr>\n");
	echo("</table>\n");
	echo("</form>\n");
	echo("</div> <!-- Einde evenementenbeheer -->\n");
}

function muteertypeevenement() {
	global $table_prefix, $soortevenement;
	
	$mess = "";
	if (isset($_GET['op']) and $_GET['op'] == "delete" and $_GET['tid'] > 0) {
		$query = sprintf("DELETE FROM %1\$sEvenement_Type WHERE RecordID=%2\$d AND RecordID NOT IN (SELECT TypeEvenement FROM %1\$sEvenement)", $table_prefix, $_GET['tid']);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Evenement type %d is verwijderd.", $_GET['tid']);
		} else {
			$mess = sprintf("Evenement type %d kon niet verwijderd worden, omdat het nog in gebruik is.", $_GET['tid']);
		}
	} elseif ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['oms_nw']) and strlen($_POST['oms_nw']) > 0) {
			$query = sprintf("INSERT INTO %sEvenement_Type SET Omschrijving='%s', Ingevoerd=CURDATE();", $table_prefix, $_POST['oms_nw']);
			$id = fnQuery($query);
			$mess = sprintf("Type evenement %d is toegevoegd.", $id);
		} else {
			foreach (db_evenement("types") as $row) {
				$set_clause = "";
				$oms = sprintf("oms_%d", $row->RecordID);
				$soort = sprintf("soort_%d", $row->RecordID);
				if (isset($_POST[$oms]) and $_POST[$oms] != $row->Omschrijving) {
					$set_clause = sprintf("Omschrijving='%s', ", $_POST[$oms]);
					$mess = sprintf("Omschrijving evenement type %d is in '%s' gewijzigd. ", $row->RecordID, $_POST[$oms]);
				}
				if (isset($_POST[$soort]) and $_POST[$soort] != $row->Soort) {
					$set_clause .= sprintf("Soort='%s', ", $_POST[$soort]);
					$mess = sprintf("De soort evenement type %d is in '%s' gewijzigd.", $row->RecordID, $_POST[$soort]);
				}
				if (strlen($set_clause) > 2) {
					$set_clause .= sprintf("GewijzigdDoor=%d", $_SESSION['lidid']);
					$query = sprintf("UPDATE %sEvenement_Type SET %s WHERE RecordID=%d;", $table_prefix, $set_clause, $row->RecordID);
					if (fnQuery($query) == 0) {
						$mess = sprintf("De wijzigingen in evenement type %d konden niet verwerkt worden.", $row->RecordID);
					}
				}
			}
		}
	}
	db_logboek("add", $mess, 7, 0, 1);
		
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	echo("<table>\n");
	echo("<tr><th></th><th>Nummer</th><th>Omschrijving</th><th>Soort</th></tr>\n");
	foreach (db_evenement("types") as $row) {
		echo("<tr>");
		printf("<td><a href='%s?tp=%s&amp;op=delete&amp;tid=%d' title='Verwijder type evenement'><img src='./images/del.png' title='Verwijder type'></a></td>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $row->RecordID);
		printf("<td class='number'>%d</td>\n", $row->RecordID);
		printf("<td><input type='text' name='oms_%d' value='%s' onBlur='this.form.submit();'></td>\n", $row->RecordID, $row->Omschrijving);
		$options = "\n";
		foreach ($soortevenement as $key => $val) {
			if ($key == $row->Soort) {
				$s = "selected";
			} else {
				$s = "";
			}
			$options .= sprintf("<option value='%s' %s>%s</option>\n", $key, $s, $val);
		}
		printf("<td><select name='soort_%d' onChange='this.form.submit();'>%s</select></td>", $row->RecordID, $options);
		echo("</tr>\n");
	}
	echo("<tr>");
	echo("<td><img src='./images/star.png' title='Nieuw type'></td>");
	echo("<td class='number'>Nw</td><td><input type='text' name='oms_nw' onBlur='this.form.submit();'></td>");
	echo("</tr>\n");
	echo("</table>\n");
	echo("</form>\n");
	
}

function overzichtevenementen() {

	echo("<div id='overzichtevenementen'>\n");
	echo("<table>\n");
	$vsrt = "Q";
	
	$lijst = db_evenement("overzicht");
	foreach ($lijst as $row) {
		if ($row->Soort != $vsrt) {
			if ($row->Soort == "W") {
				echo("<tr><th>Datum en tijden</th><th>Omschrijving</th><th>Dames</th><th>Heren</th></tr>\n");
			} else {
				echo("<tr><th>Datum en tijden</th><th>Omschrijving</th><th colspan=2>Deelnemers</th></tr>\n");
			}
			$vsrt = $row->Soort;
		}
		$oms = $row->Omschrijving;
		if (IsValidMailAddress($row->emlEmail, 0)) {
			$oms .= sprintf("<br>\n%s", fnDispEmail($row->emlEmail, "", 1));
		}
		$ad = "Aantal dln";
		if ($row->$ad > 1) {
			$oms .= sprintf("<br>\n%d&nbsp;deelnemers", $row->$ad);
		}
		if (strlen($row->Starttijd) > 3) {
			$td = "<br>\nStart:&nbsp;" . $row->Starttijd;
		} else {
			$td = "";
		}
		if (strlen($row->Eindtijd) > 3) {
			$td .= "<br>\nEinde:&nbsp;" . $row->Eindtijd;
		}
		if (strlen($row->Verzameltijd) > 3) {
			$td .= "<br>\nVerzamelen:&nbsp;" . $row->Verzameltijd;
		}
		$dames = "";
		$heren = "";
		$deelnemers = "";
		foreach (db_evenement("deelnemers", 0, $row->lnkNummer, "", "B") as $deeln) {
			if ($row->Soort != "W") {
				$vn = "deelnemers";
			} elseif ($deeln->Geslacht == "V") {
				$vn = "dames";
			} else {
				$vn = "heren";
			}
			if (strlen($$vn) > 0 and $row->Soort == "W") {
				$$vn .= "<br>\n";
			}
			$nd = htmlentities($deeln->NaamDeelnemer);
			if (strlen($deeln->Functie) > 0) {
				$nd .= sprintf(" (%s)", htmlentities($deeln->Functie));
			}
			if ($row->Soort == "W") {
				$$vn .= $nd;
			} else {
				$$vn .= sprintf("<li>%s</li>\n", $nd);
			}
		}
		if ($row->Soort == "W") {
			printf("<tr><td>%s %s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", strftime("%A %e %B %Y", strtotime($row->dteDatum)), $td, $oms, $dames, $heren);
		} else {
			printf("<tr><td class='datumtijdevenement'>%s %s</td><td class='omschrijvingevenement'>%s</td><td colspan=2><ul class='listdeelnemers'>%s</ul></td></tr>\n", strftime("%A %e %B %Y", strtotime($row->dteDatum)), $td, $oms, $deelnemers);
		}
	}
	
	echo("</table>\n");
	echo("</div>  <!-- Einde overzichtevenementen -->\n");

}
?>
