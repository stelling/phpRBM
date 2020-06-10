<?php

// $dlnstatus, $soortevenement

function fnEvenementen() {
	global $currenttab2;
	
	if (isset($_GET['op']) and strlen($_GET['op']) > 0) {
		$op = $_GET['op'];
	} else {
		$op = "overzicht";
	}
	
	if (isset($_POST['eventid']) and strlen($_POST['eventid']) > 0) {
		$eid = $_POST['eventid'];
	} elseif (isset($_GET['eid']) and strlen($_GET['eid']) > 0) {
		$eid = $_GET['eid'];
	} else {
		$eid = 0;
	}
	
	fnDispMenu(2);
	
	if ($currenttab2 == "Beheer" and $op == "edit") {
		muteerevenement($eid);
	} elseif ($currenttab2 == "Nieuw") {
		muteerevenement(0);
		
	} elseif ($currenttab2 == "Beheer" and $op == "delete") {
		(new cls_Evenement_Deelnemer())->delete($_GET['edid']);
		muteerevenement($eid);
	} elseif ($currenttab2 == "Beheer") {
		$lijst = db_evenement("overzichtbeheer");
		$lnk = sprintf("<a href='%s?tp=%s&amp;op=edit&amp;eid=%%d' title='Muteer evenement'><img src='images/edit.png' alt='Muteren'></a>", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
		if (count(db_evenement("types")) > 0) {
			$er = sprintf("<td><a href='%s?%s&amp;op=edit&amp;eid=0' title='Nieuw evenement'><img src='images/star.png' alt='Nieuw'></a></td><td colspan=10>Nieuw evenement</td>", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
		} else {
			$er = "";
		}
		echo(fnDisplayTable($lijst, $lnk, "", 0, ""));
	} elseif ($currenttab2 == "Logboek") {
		$lijst = db_logboek("lijstsmal", "", 7);
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
	// Onderdeel van de zelfservice

	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$i_ed = new cls_Evenement_Deelnemer();
		foreach (db_evenement("open") as $row) {
			$react = sprintf("reactie_%d", $row->RecordID);
			$opm = sprintf("opm_%d", $row->RecordID);
			$insrow = $i_ed->record(0, $lidid, $row->RecordID);
			if (isset($insrow->RecordID)) {
				$edid = $insrow->RecordID;
			} else {
				$edid = 0;
			}
			
			if ($_POST[$react] == "Aanmelden") {
				$stat = "B";
			} elseif ($_POST[$react] == "Inschrijven") {
				$stat = "I";
			} elseif ($_POST[$react] == "Afmelden") {
				$stat = "X";
			} else {
				$stat = "";
			}
			if ($_POST[$react] == "Geen") {
				if ($edid > 0) {
					$i_ed($edid);
				}
			} else {
				if ($edid == 0) {
					$edid = $i_ed->add($row->RecordID, $lidid, $stat);
				}
				if (strlen($stat) == 1) {
					$i_ed->update($edid, "Status", $stat);
				}
				if (isset($_POST[$opm])) {
					$i_ed->update($edid, "Opmerking", $_POST[$opm]);
				}
			}
		}
		$i_ed = null;
	}

	fnDispMenu(2);

	echo("<div id='inschrijvingevenementen'>\n");
	printf("<form method='post' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	$geldig = false;
	echo("<table>\n");
	echo("<tr><th>Datum en tijden</th><th>Omschrijving</th><th>Reactie</th><th>In agenda</th></tr>\n");
	foreach (db_evenement("open", $lidid) as $row) {
		printf("<tr><td>%s<br>van %s to %s uur</td><td>%s</td>\n", strftime("%A %e %B %Y", strtotime($row->Datum)), strftime("%R", strtotime($row->Datum)), $row->Eindtijd, $row->Omschrijving);
		$ins = db_evenement("inschrijving", $lidid, $row->RecordID);
		$v = "&nbsp;";
		
		echo("<td>");
		if (!isset($ins->Status)) {
			printf("<input type='radio' name='reactie_%d' value='Geen' checked onchange='this.form.submit();'>Geen&nbsp;", $row->RecordID);
		}
		
		if (isset($ins->Status) and ($ins->Status == "B" or $ins->Status == "I")) {
			$c = "checked";
		} else {
			$c = "";
		}
		
		if ($row->StandaardStatus == "B") {
			printf("<input type='radio' name='reactie_%d' value='Aanmelden' %s onchange='this.form.submit();'>Aanmelden&nbsp;", $row->RecordID, $c);
		} else {
			printf("<input type='radio' name='reactie_%d' value='Inschrijven' %s onchange='this.form.submit();'>Inschrijven&nbsp;", $row->RecordID, $c);
		}
		
		if (isset($ins->Status) and $ins->Status == "X") {
			$c = "checked";
		} else {
			$c = "";
		}
		printf("<input type='radio' name='reactie_%d' value='Afmelden' %s onchange='this.form.submit();'>Afmelden&nbsp;", $row->RecordID, $c);
		
		if (isset($ins->Opmerking)) {
			printf("<br><br>Opmerking: <input type='text' onBlur='this.form.submit();' name='opm_%d' size=75 maxlength=250 value=\"%s\"></td>\n", $row->RecordID, $ins->Opmerking);
		}
		
		if (isset($ins->Status) and ($ins->Status == "B" or $ins->Status == "I")) {
			printf("<td>%s</td>\n", fnAgendaKnop($row->Datum, $row->Eindtijd, $row->Verzameltijd, $row->Omschrijving, $row->Locatie));
		} else {
			echo("<td>&nbsp;</td>\n");
		}
		echo("</tr>\n");
	}
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde inschrijvingevenementen -->\n");
}

function muteerevenement($eventid) {

	$i_ev = new cls_Evenement();
	$i_ed = new cls_Evenement_Deelnemer();
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
			$eventid = db_evenement("insert");
		}
		
		$old = db_evenement("record", 0, $eventid);
		foreach($_POST as $key => $val) {
			if (array_key_exists($key, $old) and $old->$key != $_POST[$key]) {
				$inst_ev->update($eventid, $key, $_POST[$key]);
			}
		}

		if (isset($_POST['verwijderen']) and $old->VerwijderdOp < '2012-01-01') {
			$i_ev->update($eventid, "VerwijderdOp", date("Y-m-d"));
		} elseif (isset($_POST['undoverwijderen']) and $old->VerwijderdOp > '2012-01-01') {
			$i_ev->update($eventid, "VerwijderdOp", "0000-00-00");
		}
		foreach (db_evenement("deelnemers", 0, $eventid) as $rd) {
			foreach(array("Status", "Opmerking", "Functie", "Aantal") as $fld) {
				$formfld = sprintf("%s_%d", $fld, $rd->RecordID);
				if (isset($_POST[$formfld])) {
					$i_ed->update($rd->RecordID, $fld, $_POST[$formfld]);
				}
			}
		}
		if (isset($_POST['nwdln']) and $_POST['nwdln'] > 0 and is_numeric($_POST['nwdln'])) {
			$i_ed->add($eventid, $_POST['nwdln'], $inst_ev->standaardstatus($eventid));
		}
		
		if (isset($_POST['Bewaren_Sluiten'])) {
			printf("<script>location.href='%s?tp=Evenementen/Beheer'</script>\n", $_SERVER['PHP_SELF']);
		} elseif (isset($_POST['maildeeln']) or isset($_POST['mailpotdeeln']) or isset($_POST['mailbeidedeeln'])) {
			$i_M = new cls_Mailing();
			$i_MR = new cls_Mailing_rcpt();
			$mid = $inst_M->add($_POST['Omschrijving']);
			if ($mid > 0) {
				if (strlen($_POST['Email']) > 4 and isValidMailAddress($_POST['Email'])) {
					$row = db_mailing_vanaf("record", $_POST['Email']);
					if (isset($row->Vanaf_email) and strlen($row->Vanaf_email) > 4) {
						$i_M->update($mid, "from_addr", $row->Vanaf_email);
					}
				}
				if (isset($_POST['maildeeln']) or isset($_POST['mailbeidedeeln'])) {
					foreach (db_evenement("deelnemers", 0, $eventid, "", "'B', 'I', 'R', 'T'") as $row) {
						$i_MR->add($mid, $row->LidID);
					}
				}
				if (isset($_POST['mailpotdeeln']) or isset($_POST['mailbeidedeeln'])) {
					foreach (db_evenement("potdeelnemers", 0, $eventid) as $row) {
						$i_MR->add($mid, $row->LidID);
					}
				}
			}
			printf("<script>location.href='%s?tp=Mailing/Concepten&op=edit&mid=%d'</script>\n", $_SERVER['PHP_SELF'], $mid);
			$i_M = null;
			$i_MR = null;
		}
	}
	
	if ($eventid > 0) {
		$row = db_evenement("record", 0, $eventid);
		$recordid = $row->RecordID;
		$datum = date("Y-m-d", strtotime($row->Datum));
		$oms = $row->Omschrijving;
		$loc = $row->Locatie;
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
		$maxpersonenperdeelname = $row->MaxPersonenPerDeelname;
		$beperktotgroep = $row->BeperkTotGroep;
		$zichtbaarvoor = $row->ZichtbaarVoor;
		$aantdln = $row->AantDln;
		$aantafgemeld = $row->AantAfgemeld;
	} else {
		$recordid = 0;
		$datum = date("Y-m-d");
		$oms = "";
		$loc = "";
		$email = $_SESSION['emailingelogde'];
		$starttijd = "";
		$eindtijd = "";
		$verzameltijd = "";
		$typeevenement = 1;
		$insopen1 = "";
		$insopen2 = "";
		$standaardstatus = "I";
		$maxpersonenperdeelname = 1;
		$beperktotgroep = 0;
		$zichtbaarvoor = 0;
		$aantdln = 0;
		$aantafgemeld = 0;
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
	foreach (ARRDLNSTATUS as $ds => $o) {
		if ($ds == $standaardstatus) {
			$s = " selected";
		} else {
			$s = "";
		}
		$optionstandstatus .= sprintf("<option value='%s'%s>%s</option>\n", $ds, $s, $o);
	}
	
	echo("<div id='evenementenbeheer'>\n");
	printf("<form method='post' action='%s?tp=Evenementen/Beheer&amp;op=edit'>\n", $_SERVER['PHP_SELF']);
	printf("<input type='hidden' name='eventid' value=%d>\n", $eventid);
	echo("<table>\n");
	printf("<tr><td class='label'>Datum:</td><td colspan=5><input type='date' name='Datum' value='%s' size=25></td></tr>\n", $datum);
	printf("<tr><td class='label'>Omschrijving:</td><td colspan=5><input type='text' name='Omschrijving' value='%s' maxlength=50 size=60></td></tr>\n", $oms);
	printf("<tr><td class='label'>Locatie:</td><td colspan=5><input type='text' name='Locatie' value='%s' maxlength=75 size=60></td></tr>\n", $loc);
	printf("<tr><td class='label'>E-mail:</td><td colspan=5><input type='email' name='Email' value='%s' maxlength=45 size=50></td></tr>\n", $email);
	printf("<tr><td class='label'>Starttijd:</td><td><input type='time' name='Starttijd' value='%s' size=5 maxlength=5></td>\n", $starttijd);
	printf("<td class='label'>Eindtijd:</td><td><input type='time' name='Eindtijd' value='%s' size=5 maxlength=5></td>\n", $eindtijd);
	printf("<td class='label'>Verzamelen:</td><td><input type='text' name='Verzameltijd' value='%s' size=5 maxlength=5></td></tr>\n", $verzameltijd);
	printf("<tr><td class='label'>Type evenement:</td><td><select name='TypeEvenement' onChange='this.form.submit();'>%s</select></td>\n", $optiontypes);
	printf("<td class='label'>Aantal deelnemers:</td><td>%d</td>\n", $aantdln);
	printf("<td class='label'>Aantal afgemeld:</td><td>%d</td></tr>\n", $aantafgemeld);
	
	if ($eventid > 0) {
		$ondrows = (new cls_Onderdeel())->lijst(1, "", $datum);
		printf("<tr><td class='label'>Online inschrijving open:</td><td><input type='radio' name='InschrijvingOpen' value=1 %s>Ja<input type='radio' name='InschrijvingOpen' value=0 %s>Nee</td>\n", $insopen1, $insopen2);
		printf("<td class='label'>Standaard status:</td><td><select name='StandaardStatus'>%s</select></td>\n", $optionstandstatus);
		printf("<td class='label'>Max. personen per deelname:</td><td><input type='number' class='inputnumber' name='MaxPersonenPerDeelname' value=%d min=1></td></tr>\n", $maxpersonenperdeelname);
		
		$optiongroepen = "<option value=0>Niemand</option>\n";
		foreach ($ondrows as $t) {
			if ($t->RecordID == $beperktotgroep) {
				$s = " selected";
			} else {
				$s = "";
			}
			$optiongroepen .= sprintf("<option value=%d%s>%s</option>\n", $t->RecordID, $s, htmlentities($t->Naam));
		}
		printf("<tr><td class='label'>Deelname beperken tot:</td><td colspan=2><select name='BeperkTotGroep' onChange='this.form.submit();'>%s</select></td>\n", $optiongroepen);
		$optiongroepen = "<option value=0>Iedereen</option>\n";
		foreach ($ondrows as $t) {
			if ($t->RecordID == $zichtbaarvoor) {
				$s = "selected";
			} else {
				$s = "";
			}
			$optiongroepen .= sprintf("<option value=%d %s>%s</option>\n", $t->RecordID, $s, htmlentities($t->Naam));
		}
		printf("<td class='label'>Zichtbaar voor:</td><td colspan=2><select name='ZichtbaarVoor'>%s</select></td></tr>\n", $optiongroepen);
		printf("<tr><td class='label'>Gewijzigd op / door:</td><td colspan=5>%s / %s</td></tr>\n", strftime("%e %B %Y (%H:%M)", strtotime($row->Gewijzigd)), htmlentities($row->GewijzigdDoorNaam));	
	}
	echo("</table>\n");
	
	echo("<table>");
	if ($eventid > 0) {
		echo("<tr><th colspan=6 class='tabelkop'>Deelnemers</th></tr>\n");
		echo("<tr><th>Naam deelnemer</th><th>Status</th><th>Opmerking en functie</th><th>Ingevoerd</th><th>Del</th></tr>\n");
		foreach (db_evenement("deelnemers", 0, $recordid) as $rd) {
			$optstat = "";
			foreach (ARRDLNSTATUS as $key => $val) {
				$s = "";
				if ($key == $rd->Status) {
					$s = "selected";
				}
				$optstat .= sprintf("<option value='%s' %s>%s</option>\n", $key, $s, $val);
			}
			echo("<tr>\n");
			printf("<td>%s</td>\n", htmlentities($rd->NaamDeelnemer));
			printf("<td><select name='Status_%d' onChange='this.form.submit();'>%s</select>", $rd->RecordID, $optstat);
			if ($maxpersonenperdeelname > 1) {
				printf("<br>\n# personen: <input type='number' class='inputnumber' name='Aantal_%d' value=%d size=5 min=1 max=%d>", $rd->RecordID, $rd->Aantal, $maxpersonenperdeelname);
			}
			echo("</td>\n");
			printf("<td><input type='text' name='Opmerking_%d' value='%s' placeholder='Opmerking' size=75 maxlength=250>\n", $rd->RecordID, $rd->Opmerking);
			printf("<br><input type='text' name='Functie_%d' value='%s' placeholder='Functie' size=40 maxlength=30></td>\n", $rd->RecordID, $rd->Functie);
			printf("<td>%s<br>%s</td>\n", strftime("%e %b %Y (%R)", strtotime($rd->Ingevoerd)), (new cls_Lid())->Naam($rd->IngevoerdDoor));
			printf("<td><a href='%s?tp=%s&amp;op=delete&amp;eid=%d&amp;lidid=%d&amp;edid=%d'><img src='images/del.png' alt='Delete'></a></td>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $eventid, $rd->LidID, $rd->RecordID);
			echo("</tr>\n");
		}
		$optionsnieuw = "<option value=0>Nieuwe deelnemer ...</option>\n";
		foreach (db_evenement("potdeelnemers", 0, $recordid) as $lidrow) {
			$optionsnieuw .= sprintf("<option value=%d>%s</option>\n", $lidrow->LidID, htmlentities($lidrow->Naam));
		}
		printf("<tr>\n<td colspan=6><select name='nwdln' onChange='this.form.submit();'>%s</select></td>\n</tr>\n", $optionsnieuw);
	}
	echo("</table>\n");
	$aantpotdln = count(db_evenement("potdeelnemers", 0, $eventid));
	echo("<div id='opdrachtknoppen'>\n");
	if ($eventid == 0) {
		echo("<input type='submit' name='Toevoegen' value='Toevoegen'>\n");
	} elseif ($row->VerwijderdOp < '2012-01-01') {
		echo("<input type='submit' name='Bewaren' value='Bewaren'>\n");
		echo("<input type='submit' name='Bewaren_Sluiten' value='Bewaren & Sluiten'>\n");
	}
	
	if ($eventid > 0 and $row->VerwijderdOp < '2012-01-01' and $aantdln > 1 and toegang("Mailing/Nieuw", 0, 0)) {
		printf("<input type='submit' name='maildeeln' value='Mailing deelnemers (%d)'>\n", $aantdln);
	}
	if ($eventid > 0 and $row->VerwijderdOp < '2012-01-01' and $aantpotdln > 1 and toegang("Mailing/Nieuw", 0, 0)) {
		printf("<input type='submit' name='mailpotdeeln' value='Mailing potenti&euml;le deelnemers (%d)'>\n", $aantpotdln);
	}
	if ($eventid > 0 and ($_SESSION['webmaster'] == 1 or $row->IngevoerdDoor == $_SESSION['lidid'])) {
		if ($row->VerwijderdOp > '2012-01-01') {
			echo("<input type='submit' name='undoverwijderen' value='Verwijderen terugdraaien'>\n");
		} else {
			echo("<input type='submit' name='verwijderen' value='Verwijderen'>\n");
		}
	}
	echo("</div>  <!-- Einde opdrachtknoppen -->\n");
	echo("</form>\n");
	echo("</div> <!-- Einde evenementenbeheer -->\n");
	
}  # muteerevenement

function muteertypeevenement() {
	
	$inst_et = new cls_evenement_type();
	if (isset($_GET['op']) and $_GET['op'] == "delete" and $_GET['tid'] > 0) {
		$inst_et->delete($_GET['tid']);
	} elseif ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['oms_nw']) and strlen($_POST['oms_nw']) > 0) {
			$inst_et->add($_POST['oms_nw']);
		} else {
			foreach (db_evenement("types") as $row) {
				$oms = sprintf("oms_%d", $row->RecordID);
				$soort = sprintf("soort_%d", $row->RecordID);
				if (isset($_POST[$oms])) {
					$inst_et->update($row->RecordID, "Omschrijving", $_POST[$oms]);
				}
				if (isset($_POST[$soort])) {
					$inst_et->update($row->RecordID, "Soort", $_POST[$soort]);
				}
			}
		}
	}
	$inst_et = null;
		
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	echo("<table>\n");
	echo("<tr><th>Omschrijving</th><th>Soort</th><th></th></tr>\n");
	foreach (db_evenement("types") as $row) {
		echo("<tr>");
		printf("<td><input type='text' name='oms_%d' value='%s' onBlur='this.form.submit();'></td>\n", $row->RecordID, $row->Omschrijving);
		$options = "\n";
		foreach (ARRSOORTEVENEMENT as $key => $val) {
			if ($key == $row->Soort) {
				$s = "selected";
			} else {
				$s = "";
			}
			$options .= sprintf("<option value='%s' %s>%s</option>\n", $key, $s, $val);
		}
		printf("<td><select name='soort_%d' onChange='this.form.submit();'>%s</select></td>", $row->RecordID, $options);
		printf("<td><a href='%s?tp=%s&amp;op=delete&amp;tid=%d' title='Verwijder type evenement'><img src='./images/del.png' title='Verwijder type'></a></td>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $row->RecordID);
		echo("</tr>\n");
	}
	echo("<tr>");
	echo("<td><input type='text' name='oms_nw' placeholder='Nieuw type' onBlur='this.form.submit();'></td>");
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
		if ($row->ndSoort != $vsrt) {
			if ($row->ndSoort == "W") {
				echo("<tr><th>Datum en tijden</th><th>Omschrijving</th><th>Dames</th><th>Heren</th></tr>\n");
			} else {
				echo("<tr><th>Datum en tijden</th><th>Omschrijving</th><th colspan=2>Deelnemers</th></tr>\n");
			}
			$vsrt = $row->ndSoort;
		}
		$oms = $row->Omschrijving;
		if (strlen($row->Locatie) > 3) {
			$oms .= "<br>Locatie: " . $row->Locatie;
		}
		if (IsValidMailAddress($row->emlEmail, 0)) {
			$oms .= sprintf("<br>\n%s", fnDispEmail($row->emlEmail, "", 1));
		}
		$ad = "Aantal dln";
		if ($row->$ad > 1) {
			$oms .= sprintf("<br>\n%d&nbsp;deelnemers", $row->$ad);
		}
		
		if (strlen($row->Starttijd) > 3 and strlen($row->Eindtijd) > 3) {
			$td = sprintf("<br>\nvan %s tot %s uur", $row->Starttijd, $row->Eindtijd);
		} elseif (strlen($row->Starttijd) > 3) {
			$td = "<br>\nStart:&nbsp;" . $row->Starttijd;
		} elseif (strlen($row->Eindtijd) > 3) {
			$td = "<br>\nEinde:&nbsp;" . $row->Eindtijd;
		} else {
			$td = "";
		}
		
		if (strlen($row->Verzameltijd) > 3) {
			$td .= sprintf("<br>\nVerzamelen:&nbsp;%s uur", $row->Verzameltijd);
		}
		
		$dames = "";
		$heren = "";
		$deelnemers = "";
		foreach (db_evenement("deelnemers", 0, $row->lnkNummer, "", "B") as $deeln) {
			if ($row->ndSoort != "W") {
				$vn = "deelnemers";
			} elseif ($deeln->Geslacht == "V") {
				$vn = "dames";
			} else {
				$vn = "heren";
			}
			if (strlen($$vn) > 0 and $row->ndSoort == "W") {
				$$vn .= "<br>\n";
			}
			$nd = htmlentities($deeln->NaamDeelnemer);
			if ($deeln->Aantal > 1) {
				$nd .= sprintf(" (%dp)", $deeln->Aantal);
			} elseif (strlen($deeln->Functie) > 0) {
				$nd .= sprintf(" (%s)", htmlentities($deeln->Functie));
			}
			if ($row->ndSoort == "W") {
				$$vn .= $nd;
			} else {
				$$vn .= sprintf("<li>%s</li>\n", $nd);
			}
		}
		if ($row->ndSoort == "W") {
			printf("<tr><td>%s %s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", strftime("%A %e %B %Y", strtotime($row->dteDatum)), $td, $oms, $dames, $heren);
		} else {
			printf("<tr><td class='datumtijdevenement'>%s%s</td><td class='omschrijvingevenement'>%s</td><td colspan=2><ul class='listdeelnemers'>%s</ul></td></tr>\n", str_replace(" ", "&nbsp;", strftime("%A %e %B %Y", strtotime($row->dteDatum))), $td, $oms, $deelnemers);
		}
	}
	
	echo("</table>\n");
	echo("</div>  <!-- Einde overzichtevenementen -->\n");

}  # overzichtevenementen 

function fnAgendaKnop($datum, $eindtijd, $verzameltijd, $omschrijving, $locatie) {
	
	$t = date("Ymd\THis", strtotime($datum)) . "/" . date("Ymd\THis", strtotime(substr($datum, 0, 10) . " " . $eindtijd));
	if (strlen($verzameltijd) > 3) {
		$d = sprintf("Om %s verzamelen/inloop", $verzameltijd);
	} else {
		$d = "";
	}
	$gu = sprintf("https://calendar.google.com/event?action=TEMPLATE&text=%s", urlencode($omschrijving));
	$gu .= sprintf("&#038;dates=%s", $t);
	$gu .= sprintf("&#038;details=%s", urlencode($d));
	$gu .= sprintf("&#038;location=%s", urldecode($locatie));
	$gu .= "&#038;trp=false";
	$gu .= sprintf("&#038;sprop=website:%s", BASISURL);
//			$gu .= "&#038;ctz=Europe%2FAmsterdam";
			
	return sprintf("<a href='%s' target='_blank' rel='nofollow'><img border='0' src='https://img.icons8.com/plasticine/100/000000/google-logo.png'></a>", $gu);
	
}  # fbAgendaKnop
?>
