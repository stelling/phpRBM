<?php

function fnEvenementen() {
	global $currenttab2;
	
	$i_ev = new cls_Evenement();
	
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
		$lijst = $i_ev->lijst(2);
		$lnk = sprintf("<a href='%s?tp=%s&amp;op=edit&amp;eid=%%d' title='Muteer evenement'><img src='images/edit.png' alt='Muteren'></a>", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
		if ((new cls_db_base("Evenement_Type"))->aantal() > 0) {
			$er = sprintf("<td><a href='%s?%s&amp;op=edit&amp;eid=0' title='Nieuw evenement'><img src='images/star.png' alt='Nieuw'></a></td><td colspan=10>Nieuw evenement</td>", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
		} else {
			$er = "";
		}
		echo(fnDisplayTable($lijst, $lnk, "", 0, ""));
	} elseif ($currenttab2 == "Logboek") {
		$rows = (new cls_Logboek())->lijst(7, 1);
		echo(fnDisplayTable($rows, "", "", 0, "", "", "logboek"));
	} elseif ($currenttab2 == "Types muteren") {
		muteertypeevenement();
	} else {
		$currenttab2 = "Overzicht";
		overzichtevenementen();
	}
	
	$i_ev = null;
}

function ToekomstigeEvenementen() {
	
	$rv = "";
	foreach ((new cls_Evenement())->lijst(4) as $row) {
		$dat = "Op " . strftime("%A %e %B", strtotime($row->Datum));
		if ($row->Datum == date("Y-m-d")) {
			$dat = "Vandaag";
		} elseif ($row->Datum == date("Y-m-d", mktime(0, 0, 0, date("m")  , date("d")+1, date("Y")))) {
			$dat = "Morgen";
		}
		$rv .= sprintf("<li>%s %s</li>\n", $dat, $row->Omschrijving);
	}
	if (strlen($rv) > 0) {
		$rv = "<p>De volgende evenementen komen eraan.</p><ul>" . $rv . "</ul>\n";
	}
	return $rv;
}

function inschrijvenevenementen($lidid) {
	// Onderdeel van de zelfservice
	$i_ev = new cls_evenement();
	$i_ed = new cls_Evenement_Deelnemer();

	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		foreach ($i_ev->lijst(3) as $row) {
			$react = sprintf("reactie_%d", $row->RecordID);
			$opm = sprintf("opm_%d", $row->RecordID);
			$insrow = $i_ed->record(0, $lidid, $row->RecordID);
			if (isset($insrow->RecordID)) {
				$edid = $insrow->RecordID;
			} else {
				$edid = 0;
			}
			
			if ($_POST[$react] == "Inschrijven") {
				$stat = "I";
			} elseif ($_POST[$react] == "Aanmelden") {
				$stat = "J";
			} elseif ($_POST[$react] == "Afmelden") {
				$stat = "X";
			} else {
				$stat = "";
			}
			if ($_POST[$react] == "Geen") {
				if ($edid > 0) {
					$i_ed->delete($edid);
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
	}

	fnDispMenu(2);

	echo("<div id='inschrijvingevenementen'>\n");
	printf("<form method='post' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	$geldig = false;
	echo("<table>\n");
	echo("<tr><th>Datum en tijden</th><th>Omschrijving</th><th>Reactie</th><th>Agenda</th></tr>\n");
	foreach ($i_ev->lijst(3) as $row) {
		printf("<tr><td>%s<br>van %s to %s uur</td><td>%s</td>\n", strftime("%A %e %B %Y", strtotime($row->Datum)), strftime("%R", strtotime($row->Datum)), $row->Eindtijd, $row->Omschrijving);
		$ins = $i_ed->record(0, $lidid, $row->RecordID);
		$v = "&nbsp;";
		
		echo("<td>");
		if (!isset($ins->Status)) {
			printf("<input type='radio' name='reactie_%d' value='Geen' checked onchange='this.form.submit();'>Geen&nbsp;", $row->RecordID);
		}
		
		if (isset($ins->Status) and ($ins->Status == "B" or $ins->Status == "I" or $ins->Status == "J")) {
			$c = "checked";
		} else {
			$c = "";
		}
		
		if ($row->StandaardStatus == "B" or $row->StandaardStatus == "J") {
			printf("<input type='radio' name='reactie_%d' value='Aanmelden' %s onchange='this.form.submit();'>Aanmelden&nbsp;\n", $row->RecordID, $c);
		} else {
			printf("<input type='radio' name='reactie_%d' value='Inschrijven' %s onchange='this.form.submit();'>Inschrijven&nbsp;\n", $row->RecordID, $c);
		}
		
		if (isset($ins->Status) and $ins->Status == "X") {
			$c = "checked";
		} else {
			$c = "";
		}
		printf("<input type='radio' name='reactie_%d' value='Afmelden' %s onchange='this.form.submit();'>Afmelden&nbsp;\n", $row->RecordID, $c);
		
		if (isset($ins->Opmerking)) {
			printf("<br>\n<input type='text' placeholder='Opmerking' onBlur='this.form.submit();' name='opm_%d' size=75 maxlength=250 value=\"%s\"></td>\n", $row->RecordID, $ins->Opmerking);
		}
		
		if (isset($ins->Status) and ($ins->Status == "B" or $ins->Status == "I" or $ins->Status == "J")) {
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
		
		if (!isset($_POST['InschrijvingOpen'])) {
			$_POST['InschrijvingOpen'] = 0;
		}
		
		if ($eventid == 0) {
			$eventid = $i_ev->add($_POST['TypeEvenement']);
		}
		
		$old = $i_ev->record($eventid);
		foreach($_POST as $key => $val) {
			if (array_key_exists($key, $old) and $old->$key != $_POST[$key]) {
				$i_ev->update($eventid, $key, $_POST[$key]);
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
			$i_ed->add($eventid, $_POST['nwdln'], $i_ev->standaardstatus($eventid));
		}
		
		if (isset($_POST['Bewaren_Sluiten'])) {
			printf("<script>location.href='%s?tp=Evenementen/Beheer'</script>\n", $_SERVER['PHP_SELF']);
		} elseif (isset($_POST['maildeeln']) or isset($_POST['mailpotdeeln']) or isset($_POST['mailbeidedeeln'])) {
			$i_m = new cls_Mailing();
			$i_mr = new cls_Mailing_rcpt();
			$mid = $i_m->add($_POST['Omschrijving']);
			if ($mid > 0) {
				if (isset($_POST['maildeeln']) or isset($_POST['mailbeidedeeln'])) {
					foreach ($i_ed->overzichtevenement($eventid, "'B', 'I', 'J', 'R', 'T'") as $row) {
						$i_mr->add($mid, $row->LidID);
					}
				}
				if (isset($_POST['mailpotdeeln']) or isset($_POST['mailbeidedeeln'])) {
					foreach ($i_ev->potdeelnemers($eventid) as $row) {
						$i_mr->add($mid, $row->LidID);
					}
				}
			}
			printf("<script>location.href='%s?tp=Mailing/Wijzigen+mailing&op=edit&mid=%d'</script>\n", $_SERVER['PHP_SELF'], $mid);
			$i_m = null;
			$i_mr = null;
		}
	}
	
	if ($eventid > 0) {
		$row = $i_ev->record($eventid);
		$recordid = $row->RecordID;
		$datum = date("Y-m-d", strtotime($row->Datum));
		$oms = $row->Omschrijving;
		$loc = $row->Locatie;
		$email = $row->Email;
		$starttijd = date("H:i", strtotime($row->Datum));
		$eindtijd = $row->Eindtijd;
		$verzameltijd = $row->Verzameltijd;
		$typeevenement = $row->TypeEvenement;
		$insopen = $row->InschrijvingOpen;
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
		$insopen = 0;
		$standaardstatus = "I";
		$maxpersonenperdeelname = 1;
		$beperktotgroep = 0;
		$zichtbaarvoor = 0;
		$aantdln = 0;
		$aantafgemeld = 0;
	}
	
	$optiontypes = (new cls_Evenement_Type())->htmloptions($typeevenement);
	
	$optionstandstatus = "";
	foreach (ARRDLNSTATUS as $ds => $o) {
		$s = checked($standaardstatus, "option", $o);
		$optionstandstatus .= sprintf("<option value='%s' %s>%s</option>\n", $ds, $s, $o);
	}
	
	echo("<div id='evenementenbeheer'>\n");
	printf("<form method='post' action='%s?tp=Evenementen/Beheer&amp;op=edit'>\n", $_SERVER['PHP_SELF']);
	printf("<input type='hidden' name='eventid' value=%d>\n", $eventid);
	echo("<table>\n");
	printf("<tr><td class='label'>Datum:</td><td colspan=5><input type='date' name='Datum' value='%s' size=25></td></tr>\n", $datum);
	printf("<tr><td class='label'>Omschrijving</td><td colspan=5><input type='text' name='Omschrijving' value='%s' maxlength=50 size=60></td></tr>\n", $oms);
	printf("<tr><td class='label'>Locatie</td><td colspan=5><input type='text' name='Locatie' value='%s' maxlength=75 size=60></td></tr>\n", $loc);
	printf("<tr><td class='label'>E-mail</td><td colspan=5><input type='email' name='Email' value='%s' maxlength=45 size=50></td></tr>\n", $email);
	printf("<tr><td class='label'>Starttijd</td><td><input type='time' name='Starttijd' value='%s' size=5 maxlength=5></td>\n", $starttijd);
	printf("<td class='label'>Eindtijd</td><td><input type='time' name='Eindtijd' value='%s' size=5 maxlength=5></td>\n", $eindtijd);
	printf("<td class='label'>Verzamelen</td><td><input type='time' name='Verzameltijd' value='%s' size=5 maxlength=5></td></tr>\n", $verzameltijd);
	printf("<tr><td class='label'>Type evenement</td><td><select name='TypeEvenement' onChange='this.form.submit();'>%s</select></td>\n", $optiontypes);
	printf("<td class='label'>Aantal deelnemers</td><td>%d</td>\n", $aantdln);
	printf("<td class='label'>Aantal afgemeld</td><td>%d</td></tr>\n", $aantafgemeld);
	
	if ($eventid > 0) {
		$ondrows = (new cls_Onderdeel())->lijst(1, "", $datum);
		printf("<tr><td class='label'>Online inschrijving open</td><td><input type='checkbox' name='InschrijvingOpen' value=1 %s></td>\n", checked($insopen));
		printf("<td class='label'>Standaard status</td><td><select name='StandaardStatus'>%s</select></td>\n", $optionstandstatus);
		printf("<td class='label'>Max. personen per deelname</td><td><input type='number' class='inputnumber' name='MaxPersonenPerDeelname' value=%d min=1></td></tr>\n", $maxpersonenperdeelname);
		
		$optiongroepen = "<option value=0>Niemand</option>\n";
		foreach ($ondrows as $t) {
			if ($t->RecordID == $beperktotgroep) {
				$s = " selected";
			} else {
				$s = "";
			}
			$optiongroepen .= sprintf("<option value=%d%s>%s</option>\n", $t->RecordID, $s, htmlentities($t->Naam));
		}
		printf("<tr><td class='label'>Deelname beperken tot</td><td colspan=2><select name='BeperkTotGroep' onChange='this.form.submit();'>%s</select></td>\n", $optiongroepen);
		$optiongroepen = "<option value=0>Iedereen</option>\n";
		foreach ($ondrows as $t) {
			if ($t->RecordID == $zichtbaarvoor) {
				$s = "selected";
			} else {
				$s = "";
			}
			$optiongroepen .= sprintf("<option value=%d %s>%s</option>\n", $t->RecordID, $s, htmlentities($t->Naam));
		}
		printf("<td class='label'>Zichtbaar voor</td><td colspan=2><select name='ZichtbaarVoor'>%s</select></td></tr>\n", $optiongroepen);
		printf("<tr><td class='label'>Gewijzigd op / door</td><td colspan=5>%s / %s</td></tr>\n", strftime("%e %B %Y (%H:%M)", strtotime($row->Gewijzigd)), htmlentities($row->GewijzigdDoorNaam));	
	}
	echo("</table>\n");
	
	echo("<div id='deelnemersevenementmuteren'>\n");
	echo("<table>\n");
	if ($eventid > 0) {
		echo("<caption>Deelnemers</caption>\n");
		echo("<thead>\n<tr><th>Deelnemer</th><th>Status</th><th>Opmerking en functie</th><th>Ingevoerd</th><th></th></tr>\n</thead>\n");
		foreach ($i_ed->overzichtevenement($recordid) as $rd) {
			$optstat = "";
			foreach (ARRDLNSTATUS as $key => $val) {
				$s = "";
				if ($key == $rd->Status) {
					$s = "selected";
				}
				$optstat .= sprintf("<option value='%s' %s>%s</option>\n", $key, $s, $val);
			}
			echo("<tr>\n");
			if (strlen($rd->Telefoon) >= 10) {
				$t = "<br>\n" . $rd->Telefoon;
			} else {
				$t = "";
			}
			printf("<td class='ed_status_%s'>%s %s</td>\n", $rd->Status, htmlentities($rd->NaamDeelnemer), $t);
			printf("<td><select name='Status_%d' onChange='this.form.submit();'>%s</select>", $rd->RecordID, $optstat);
			if ($maxpersonenperdeelname > 1) {
				printf("<br>\n# personen: <input type='number' class='inputnumber' name='Aantal_%d' value=%d size=5 min=1 max=%d>", $rd->RecordID, $rd->Aantal, $maxpersonenperdeelname);
			}
			echo("</td>\n");
			printf("<td><input type='text' name='Opmerking_%d' value='%s' placeholder='Opmerking' size=75 maxlength=250>\n", $rd->RecordID, $rd->Opmerking);
			printf("<br><input type='text' name='Functie_%d' value='%s' placeholder='Functie' maxlength=30></td>\n", $rd->RecordID, $rd->Functie);
			printf("<td>%s<br>%s</td>\n", strftime("%e %b %Y (%R)", strtotime($rd->Ingevoerd)), (new cls_Lid())->Naam($rd->IngevoerdDoor));
			printf("<td><a href='%s?tp=%s&amp;op=delete&amp;eid=%d&amp;lidid=%d&amp;edid=%d'><img src='%s' alt='Delete' title='Verwijder deelnemer'></a></td>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $eventid, $rd->LidID, $rd->RecordID, BASE64_VERWIJDER);
			echo("</tr>\n");
		}
		$optionsnieuw = "<option value=0>Nieuwe deelnemer ...</option>\n";
		foreach ($i_ev->potdeelnemers($recordid) as $lidrow) {
			$optionsnieuw .= sprintf("<option value=%d>%s</option>\n", $lidrow->LidID, htmlentities($lidrow->Naam));
		}
		printf("<tr>\n<td colspan=6><select name='nwdln' onChange='this.form.submit();'>%s</select></td>\n</tr>\n", $optionsnieuw);
	}
	echo("</table>\n");
	echo("</div> <!-- Einde deelnemersevenementmuteren -->\n");
	$aantpotdln = count(db_evenement("potdeelnemers", 0, $eventid));
	echo("<div id='opdrachtknoppen'>\n");
	if ($eventid == 0) {
		echo("<input type='submit' name='Toevoegen' value='Toevoegen'>\n");
	} elseif ($row->VerwijderdOp < '2012-01-01') {
		echo("<input type='submit' name='Bewaren' value='Bewaren'>\n");
		echo("<input type='submit' name='Bewaren_Sluiten' value='Bewaren & Sluiten'>\n");
	}
	
	if ($eventid > 0 and $row->VerwijderdOp < '2012-01-01' and $aantdln > 0 and toegang("Mailing/Nieuw", 0, 0)) {
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
	
	$i_et = new cls_evenement_type();
	if (isset($_GET['op']) and $_GET['op'] == "delete" and $_GET['tid'] > 0) {
		$i_et->delete($_GET['tid']);
	} elseif ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['oms_nw']) and strlen($_POST['oms_nw']) > 0) {
			$i_et->add($_POST['oms_nw']);
		} else {
			foreach ($i_et->lijst() as $row) {
				$oms = sprintf("oms_%d", $row->RecordID);
				$soort = sprintf("soort_%d", $row->RecordID);
				if (isset($_POST[$oms])) {
					$i_et->update($row->RecordID, "Omschrijving", $_POST[$oms]);
				}
				if (isset($_POST[$soort])) {
					$i_et->update($row->RecordID, "Soort", $_POST[$soort]);
				}
			}
		}
	}
		
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	echo("<table>\n");
	echo("<tr><th>Omschrijving</th><th>Soort</th><th></th></tr>\n");
	foreach ($i_et->lijst() as $row) {
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
		printf("<td><a href='%s?tp=%s&amp;op=delete&amp;tid=%d' title='Verwijder type evenement'><img src='%s' title='Verwijder type'></a></td>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $row->RecordID, BASE64_VERWIJDER);
		echo("</tr>\n");
	}
	echo("<tr>");
	echo("<td><input type='text' name='oms_nw' placeholder='Nieuw type' onBlur='this.form.submit();'></td>");
	echo("</tr>\n");
	echo("</table>\n");
	echo("</form>\n");
	
	$i_et = null;
}

function overzichtevenementen() {

	$i_ev = new cls_Evenement();
	$i_ed = new cls_Evenement_Deelnemer();

	echo("<div id='overzichtevenementen'>\n");
	echo("<table>\n");
	$vsrt = "Q";
	
	$lijst = $i_ev->lijst(1);
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
		$ad = 0;
		
		foreach($i_ed->overzichtevenement($row->lnkNummer, "'B','J'") as $deeln) {
			if ($row->ndSoort != "W") {
				$vn = "deelnemers";
				$ad++;
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
			printf("<tr><td>%s %s</td><td>%s</td><td>%s</td><td>%s", strftime("%A %e %B %Y", strtotime($row->dteDatum)), $td, $oms, $dames, $heren);
		} elseif ($ad > 3) {
			printf("<tr><td>%s%s</td><td>%s</td><td colspan=2><ul>%s<li>%d deelnemers</li></ul>", str_replace(" ", "&nbsp;", strftime("%A %e %B %Y", strtotime($row->dteDatum))), $td, $oms, $deelnemers, $ad);
		} else {
			printf("<tr><td>%s%s</td><td>%s</td><td colspan=2><ul>%s</ul>", str_replace(" ", "&nbsp;", strftime("%A %e %B %Y", strtotime($row->dteDatum))), $td, $oms, $deelnemers);
		}
		printf("</td></tr>\n");
	}
	
	echo("</table>\n");
	echo("</div>  <!-- Einde overzichtevenementen -->\n");
	
	$i_ev = null;
	$i_ed = null;

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
