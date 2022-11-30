<?php

function fnEvenementen() {
	global $currenttab2;
	
	$i_ev = new cls_Evenement();
	$i_et = new cls_Evenement_Type();
	
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
		
		echo("<div id='evenementenbeheer'>\n");
		
		echo("<div id='form'>\n");
		printf("<form method='post' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
		echo("<input type='text' name='tbTekstFilter' id='tbTekstFilter' placeholder='Tekstfilter' OnKeyUp=\"fnFilter('evenementlijst', this);\">\n");
		foreach(ARRSOORTEVENEMENT as $k => $v) {
			$f = sprintf("ET.Soort='%s'", $k);
			if ($i_et->aantal($f) > 0) {
				$cn = sprintf("chk_%s", $k);
				if ($_SERVER['REQUEST_METHOD'] == "POST") {
					$chk = $_POST[$cn] ?? 0;
				} else {
					$chk = 1;
				}
				printf("<label>%s</label><input type='checkbox' value=1 name='chk_%s' onClick='this.form.submit();'%s>\n", $v, $k, checked($chk));
			}
		}
		echo("</form>\n");
		echo("</div> <!-- Einde form -->\n");
		echo("<div class='clear'></div>\n");
		
		$in = "";
		foreach(ARRSOORTEVENEMENT as $k => $v) {
			$cn = sprintf("chk_%s", $k); 	
			if (isset($_POST[$cn]) and $_POST[$cn] == "1") {
				if (strlen($in) > 0) {
					$in .= ", ";
				}
				$in .= sprintf("'%s'", $k);
			}
		}
		if (strlen($in) > 2) {
			$in = sprintf("ET.Soort IN (%s)", $in);
		}
		$lijst = $i_ev->lijst(2, "", $in);
		
		$kols[0]['link'] = sprintf("<a href='%s?tp=%s&amp;op=edit&amp;eid=%%d' title='Muteer evenement'>&nbsp;&nbsp;&nbsp;</a>", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
		$kols[0]['class'] = "muteren";
		$kols[0]['columnname'] = "RecordID";
		$kols[1]['headertext'] = "Datum";
		$kols[1]['type'] = "date";
		$kols[2]['headertext'] = "Starttijd";
		$kols[3]['headertext'] = "Omschrijving";
		$kols[4]['headertext'] = "Locatie";
		$kols[5]['headertext'] = "Dln";
		$kols[5]['type'] = "integer";
		$kols[6]['headertext'] = "Email";
		$kols[7]['headertext'] = "Einddtijd";
		$kols[8]['headertext'] = "Ins open?";
		$kols[9]['headertext'] = "Type";
		
		if ((new cls_Evenement_Type())->aantal() > 0) {
			$er = sprintf("<td><a href='%s?%s&op=edit&eid=0' title='Nieuw evenement'><img src='images/star.png' alt='Nieuw'></a></td><td colspan=10>Nieuw evenement</td>", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
		} else {
			$er = "";
		}
		echo(fnDisplayTable($lijst, $kols, "", 0, "", "evenementlijst"));
		
		echo("</div> <!--  Einde evenementenbeheer -->\n");
		
	} elseif ($currenttab2 == "Logboek") {
		$rows = (new cls_Logboek())->lijst(7, 1);
		
		echo(fnDisplayTable($rows, "logboek", "", 0, "", "", "logboek"));
	} elseif ($currenttab2 == "Types muteren") {
		muteertypeevenement();
	} else {
		$currenttab2 = "Overzicht";
		overzichtevenementen();
	}
	
	$i_ev = null;
	
}  # fnEvenementen

function inschrijvenevenementen($lidid) {
	// Onderdeel van de zelfservice
	global $dtfmt;
	
	$i_ev = new cls_evenement();
	$i_ed = new cls_Evenement_Deelnemer();

	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		foreach ($i_ev->lijst(3) as $row) {
			$rv = 0;
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
					$rv += $edid;
				}
				if (strlen($stat) == 1) {
					$rv += $i_ed->update($edid, "Status", $stat);
				}
				
				$aantal = sprintf("aantal_%d", $row->RecordID);
				if (isset($_POST[$aantal]) and strlen($_POST[$aantal]) > 0) {
					$rv += $i_ed->update($edid, "Aantal", $_POST[$aantal]);
				}
				
				if (isset($_POST[$opm])) {
					$rv += $i_ed->update($edid, "Opmerking", $_POST[$opm]);
				}
			}
		
			if ($_SESSION['settings']['mailing_bevestigingdeelnameevenement'] > 0 and $rv > 0) {
				$mailing = new Mailing($_SESSION['settings']['mailing_bevestigingdeelnameevenement']);
				$mailing->xtrachar = "EVD";
				$mailing->xtranum = $edid;
			
				if ($mailing->send($lidid, 0) > 0) {
					$mess = sprintf("Bevestiging deelname evenement (%d) is aan %s verzonden.", $edid, (new cls_Lid())->Naam($lidid));
				} else {
					$mess = sprintf("Fout bij het versturen van de e-mail. Probeer het later nogmaals of neem contact op met de webmaster.");
				}
				$mailing = null;
				(new cls_Logboek())->add($mess, 7, $lidid, 2);
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
		$oms = $row->Omschrijving;
		if (strlen($row->Locatie) > 1) {
			$oms .= "<br>\n" . $row->Locatie;
		}
		if ($row->Eindtijd > "00:00") {
			$tijden = sprintf("van %s tot %s uur", date("H:i", strtotime($row->Datum)), $row->Eindtijd);
		} else {
			$tijden = sprintf("vanaf %s uur", date("H:i", strtotime($row->Datum)));
		}
		$dtfmt->setPattern(DTTEXTWD);
		printf("<tr><td>%s<br>%s</td><td>%s</td>\n", $dtfmt->format(strtotime($row->Datum)), $tijden, $oms);
		$ins = $i_ed->record(0, $lidid, $row->RecordID);
		$v = "&nbsp;";
		
		echo("<td>");
		if (!isset($ins->Status)) {
			printf("<input type='radio' name='reactie_%d' value='Geen' checked>Geen&nbsp;", $row->RecordID);
		}
		
		if (isset($ins->Status) and ($ins->Status == "B" or $ins->Status == "I" or $ins->Status == "J")) {
			$c = "checked";
		} else {
			$c = "";
		}
		
		if ($row->StandaardStatus == "B" or $row->StandaardStatus == "J") {
			printf("<input type='radio' name='reactie_%d' value='Aanmelden' %s>Aanmelden&nbsp;\n", $row->RecordID, $c);
		} else {
			printf("<input type='radio' name='reactie_%d' value='Inschrijven' %s>Inschrijven&nbsp;\n", $row->RecordID, $c);
		}
		
		if (isset($ins->Status) and $ins->Status == "X") {
			$c = "checked";
		} else {
			$c = "";
		}
		printf("<input type='radio' name='reactie_%d' value='Afmelden' %s>Afmelden&nbsp;\n", $row->RecordID, $c);
		
		if ($row->MaxPersonenPerDeelname > 1) {
			printf("<br>\n<label>Aantal personen (%2\$d max)</label><input type='number' name='aantal_%1\$d' min=1 max=%2\$d value=%3\$d class='num2'>", $row->RecordID, $row->MaxPersonenPerDeelname, $ins->Aantal);
		}
		if (isset($ins->Opmerking)) {
			printf("<br>\n<input type='text' placeholder='Opmerking' name='opm_%d' class='w250' maxlength=250 value=\"%s\">", $row->RecordID, str_replace("\"", "'", $ins->Opmerking));
		}
		echo("</td>\n");
		
		if (isset($ins->Status) and ($ins->Status == "B" or $ins->Status == "I" or $ins->Status == "J")) {
			printf("<td>%s</td>\n", fnAgendaKnop($row->Datum, $row->Eindtijd, $row->Verzameltijd, $row->Omschrijving, $row->Locatie));
		} else {
			echo("<td>&nbsp;</td>\n");
		}
		echo("</tr>\n");
	}
	echo("</table>\n");
	echo("<div id='opdrachtknoppen'>\n");
	echo("<button type='submit'>Bevestigen</button>\n");
	echo("</div> <!-- Einde opdrachtknoppen -->\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde inschrijvingevenementen -->\n");
	
}  # inschrijvenevenementen

function muteerevenement($eventid) {
	global $dtfmt;

	$i_ev = new cls_Evenement();
	$i_ed = new cls_Evenement_Deelnemer();
	$i_lo = new cls_Lidond();
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {

		if ($eventid == 0 and isset($_POST['btnToevoegen'])) {
			
			$d = $_POST['Datum'];
			if (isset($_POST['Starttijd']) and strlen($_POST['Starttijd']) > 4) {
				$d .= " " . $_POST['Starttijd'];
			}
			$eventid = $i_ev->add($d);
			
			if (isset($_POST['Omschrijving']) and strlen($_POST['Omschrijving']) > 0) {
				$i_ev->update($eventid, "Omschrijving", $_POST['Omschrijving']);
			}
			if (isset($_POST['Locatie']) and strlen($_POST['Locatie']) > 0) {
				$i_ev->update($eventid, "Locatie", $_POST['Locatie']);
			}
		}
		
		if (isset($_POST['BeperkTotGroep'])) {
			$i_ev->update($eventid, "BeperkTotGroep", $_POST['BeperkTotGroep']);
		}

		if (isset($_POST['verwijderen'])) {
			$i_ev->update($eventid, "VerwijderdOp", date("Y-m-d"));
		} elseif (isset($_POST['undoverwijderen'])) {
			$i_ev->update($eventid, "VerwijderdOp", "0000-00-00");
		}
		
		if (isset($_POST['nwdln']) and $_POST['nwdln'] > 0 and is_numeric($_POST['nwdln'])) {
			$i_ed->add($eventid, $_POST['nwdln'], $i_ev->standaardstatus($eventid));
		}
		
		if (isset($_POST['Bewaren_Sluiten'])) {
			printf("<script>location.href='%s?tp=Evenementen/Beheer'</script>\n", $_SERVER['PHP_SELF']);
		} elseif (isset($_POST['maildeeln']) or isset($_POST['mailpotdeeln']) or isset($_POST['mailbeidedeeln'])) {
			$i_m = new cls_Mailing();
			$i_mr = new cls_Mailing_rcpt();
			$eo = sprintf("Evenement %d", $eventid);
			if (isset($_POST['Omschrijving']) and strlen($_POST['Omschrijving']) >= 4) {
				$eo = $_POST['Omschrijving'];
			}

			$mid = $i_m->add($eo);
			$i_m->update($mid, "EvenementID", $eventid);
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
	$meerderestartmomenten = 0;
	$beperktotgroep = 0;
	$zichtbaarvoor = 0;
	$aantdln = 0;
	$aantalingeschreven = 0;
	$aantafgemeld = 0;
	
	if ($eventid > 0) {
		$row = $i_ev->record($eventid);
		if (isset($row->RecordID)) {
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
			$meerderestartmomenten = $row->MeerdereStartMomenten;
			$beperktotgroep = $row->BeperkTotGroep;
			$zichtbaarvoor = $row->ZichtbaarVoor;
			$aantdln = $row->AantDln;
			$aantalingeschreven = $row->AantInschr;
			$aantafgemeld = $row->AantAfgemeld;
		}
	}
	
	$optiontypes = (new cls_Evenement_Type())->htmloptions($typeevenement);
	
	$optionstandstatus = "";
	foreach (ARRDLNSTATUS as $ds => $o) {
		$s = checked($standaardstatus, "option", $ds);
		$optionstandstatus .= sprintf("<option value='%s' %s>%s</option>\n", $ds, $s, $o);
	}
	
	echo("<div id='evenementenbeheer'>\n");
	printf("<form method='post' action='%s?tp=Evenementen/Beheer&op=edit&eid=%d'>\n", $_SERVER['PHP_SELF'], $eventid);
	printf("<input type='hidden' name='eventid' value=%d>\n", $eventid);

	printf("<label id='lblDatum'>Datum</label><input type='date' id='Datum' name='Datum' value='%s'>\n", $datum);
	printf("<label id='lblStarttijd'>Starttijd</label><input type='time' id='Starttijd' name='Starttijd' value='%s'>\n", $starttijd);
	if ($eventid > 0) {
		printf("<label id='lblEindtijd'>Eindtijd</label><input type='time' id='Eindtijd' value='%s'>\n", $eindtijd);
		printf("<label id='lblVerzameltijd'>Verzamelen</label><input type='time' id='Verzameltijd' value='%s'>\n", $verzameltijd);
	}

	printf("<label id='lblOmschrijving'>Omschrijving</label><input type='text' id='Omschrijving' name='Omschrijving' class='w50' value='%s' maxlength=50>\n", $oms);
	printf("<label id='lblLocatie'>Locatie</label><input type='text' id='Locatie' name='Locatie' value='%s' class='w75' maxlength=75>\n", $loc);
	if ($eventid > 0) {
		printf("<label id='lblEmail'>E-mail</label><input type='email' id='Email' value='%s' class='w50' maxlength=45>\n", $email);
	
		echo("<span>\n");
		printf("<label>Type evenement</label><select name='TypeEvenement'>%s</select>\n", $optiontypes);
		printf("<label>Opmaak in agenda</label>%s\n", fnEvenementOmschrijving($row, 1, "p"));
		echo("</span>\n");
	
		$ondrows = (new cls_Onderdeel())->lijst(1, "", $datum);
		echo("<span>\n");
		printf("<label>Online inschrijving open</label><input type='checkbox' id='InschrijvingOpen' value=1 %s>\n", checked($insopen));
		printf("<label>Standaard status</label><select id='StandaardStatus'>%s</select>\n", $optionstandstatus);
		printf("<label>Max. pers. per deelname</label><input type='number' id='MaxPersonenPerDeelname' class='num2' value=%d min=1 max=99>\n", $maxpersonenperdeelname);
		printf("<label>Meerdere startmomenten</label><input type='checkbox' id='MeerdereStartMomenten' value=1 %s>\n", checked($meerderestartmomenten));
		echo("</span>\n");
		
		$optiongroepen = "<option value=0>Niemand</option>\n";
		foreach ($ondrows as $t) {
			if ($t->RecordID == $beperktotgroep) {
				$s = " selected";
			} else {
				$s = "";
			}
			$optiongroepen .= sprintf("<option value=%d%s>%s</option>\n", $t->RecordID, $s, htmlentities($t->Naam));
		}
		echo("<span>\n");
		printf("<label>Deelname beperken tot</label><select name='BeperkTotGroep' onChange='this.form.submit();'>\n%s</select>\n", $optiongroepen);
		
		$optiongroepen = "<option value=0>Iedereen</option>\n";
		foreach ($ondrows as $t) {
			if ($t->RecordID == $zichtbaarvoor) {
				$s = "selected";
			} else {
				$s = "";
			}
			$optiongroepen .= sprintf("<option value=%d %s>%s</option>\n", $t->RecordID, $s, htmlentities($t->Naam));
		}
		printf("<label>Zichtbaar voor</label><select id='ZichtbaarVoor'>%s</select>\n", $optiongroepen);
		echo("</span>\n");
		echo("<span>\n");
		
		$dtfmt->setPattern(DTLONG);
		printf("<label>Gewijzigd op / door</label><p>%s / %s</p>\n", $dtfmt->format(strtotime($row->Gewijzigd)), htmlentities($row->GewijzigdDoorNaam));
		printf("<label>Aantal deelnemers</label><p>%d</p>\n", $aantdln);
		printf("<label>Aantal ingeschreven</label><p>%d</p>\n", $aantalingeschreven);
		printf("<label>Aantal afgemeld</label><p>%d</p>\n", $aantafgemeld);
		echo("</span>\n");
	}
	
//	echo("<div id='deelnemersevenementmuteren'>\n");
	$rowpd = $i_ev->potdeelnemers($recordid, $datum);
	if ($eventid > 0) {
		echo("<table id='deelnemersevenementmuteren'>\n");
		echo("<caption>Deelnemers</caption>\n");
		echo("<thead>\n<tr><th>Deelnemer</th>");
		if ($meerderestartmomenten == 1) {
			echo("<th>Start</th>");
		}
		if ($maxpersonenperdeelname > 1) {
			echo("<th>#</th>");
		}
		echo("<th>Status</th><th>Opmerking en functie</th><th>Ingevoerd</th><th></th></tr>\n</thead>\n");
		$dtfmt->setPattern(DTTEXT);
		$uitleg = "";
		foreach ($i_ed->overzichtevenement($recordid) as $rd) {
			$optstat = "";
			foreach (ARRDLNSTATUS as $key => $val) {
				$s = "";
				if ($key == $rd->Status) {
					$s = "selected";
				}
				$optstat .= sprintf("<option value='%s' %s>%s</option>\n", $key, $s, $val);
			}
			printf("<tr id='rid_%d'>\n", $rd->RecordID);
			if ($i_lo->islid($rd->LidID, $beperktotgroep, $datum)) {
				$t = "";
			} else {
				$t = " *";
				$uitleg = "* = geen lid van 'Deelname beperken tot'";
			}
			if (strlen($rd->Telefoon) >= 10) {
				$t .= "<br>\n" . $rd->Telefoon;
			}
			printf("<td id='naamdln_%d' class='ed_status_%s'>%s %s</td>\n", $rd->RecordID, $rd->Status, htmlentities($rd->NaamDeelnemer), $t);
			if ($meerderestartmomenten == 1) {
				printf("<td><input type='time' id='StartMoment_%d' value='%s'></td>\n", $rd->RecordID, substr($rd->StartMoment, 0, 5));
			}
			if ($maxpersonenperdeelname > 1) {
				printf("<td><input type='number' id='Aantal_%d' value=%d class='num2' min=1 max=%d></td>\n", $rd->RecordID, $rd->Aantal, $maxpersonenperdeelname);
			}
			printf("<td><select id='Status_%d'>%s</select>", $rd->RecordID, $optstat);
			echo("</td>\n");
			printf("<td><input type='text' id='Opmerking_%d' value=\"%s\" placeholder='Opmerking' class='w250' maxlength=250>\n", $rd->RecordID, str_replace("\"", "'", $rd->Opmerking));
			printf("<br><input type='text' id='Functie_%d' value=\"%s\" placeholder='Functie' class='w30' maxlength=30></td>\n", $rd->RecordID, str_replace("\"", "'", $rd->Functie));
			printf("<td>%s<br>%s</td>\n", $dtfmt->format(strtotime($rd->Ingevoerd)), (new cls_Lid())->Naam($rd->IngevoerdDoor));
			
			printf("<td id='Delete_%2\$d'><img src='%1\$s' onClick='verw_dln(%2\$d);'</td>", BASE64_VERWIJDER_KLEIN, $rd->RecordID);
			echo("</tr>\n");
		}
		echo("</table>\n");
		
		$optionsnieuw = "<option value=0>Deelnemer toevoegen ...</option>\n";
		foreach ($rowpd as $lidrow) {
			$optionsnieuw .= sprintf("<option value=%d>%s</option>\n", $lidrow->LidID, htmlentities($lidrow->Naam));
		}
		printf("<select name='nwdln' onChange='this.form.submit();'>%s</select>\n", $optionsnieuw);
		if (strlen($uitleg) > 0) {
			printf("<p>%s</p>\n", $uitleg);
		}
	}
//	echo("</div> <!-- Einde deelnemersevenementmuteren -->\n");
	$aantpotdln = count($rowpd);
	echo("<div id='opdrachtknoppen'>\n");
	if ($eventid == 0) {
		echo("<button type='submit' name='btnToevoegen'>Toevoegen</button>\n");
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
	
	printf("<script>
		\$( document ).ready(function() {
			\$(\"input\").blur(function(){
				if (this.id == 'Datum' || this.id == 'Starttijd') {
					value = \$('#Datum').val() + ' ' + \$('#Starttijd').val();
					$.ajax({
						url: 'ajax_update.php?entiteit=evenement',
						type: 'post',
						dataType: 'json',
						data: { field: 'Datum', value: value, id: %1\$d },
						success: function(response){}
					});
					
					savedata('evenement', %1\$d, nw);
				} else {
					savedata('evenement', %1\$d, this);
				}
			});
			
			\$('#ZichtbaarVoor').change(function(){
				savedata('evenement', %1\$d, this);
			});
			
			\$('#StandaardStatus').change(function(){
				savedata('evenement', %1\$d, this);
			});
			
			\$('#MaxPersonenPerDeelname').change(function(){
				savedata('evenement', %1\$d, this);
			});		
			
			\$(\"select[id^='Status_'], input[id^='Functie_']\").change(function() {
				savedata('evenementdln', 0, this);
				
				var rid = this.id.split('_')[1];
				\$('#naamdln_' + rid).removeClass();
				\$('#naamdln_' + rid).addClass('ed_status_' + this.value)
			});
			
			\$(\"input[id^='StartMoment_'], input[id^='Opmerking_'], input[id^='Functie_']\").blur(function() {
				savedata('evenementdln', 0, this);
			});
			
			\$(\"input[id^='Aantal_']\").change(function() {
				savedata('evenementdln', 0, this);
			});
		});
		function verw_dln(rid) {
			deleterecord('verw_dln', rid);
			\$('#rid_' + rid).remove();
		}
	</script>\n", $eventid);
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
				$tk = sprintf("tekstkleur_%d", $row->RecordID);
				$vet = sprintf("vet_%d", $row->RecordID);
				$cur = sprintf("cursief_%d", $row->RecordID);
				$ak = sprintf("achtergrondkleur_%d", $row->RecordID);
				if (isset($_POST[$oms])) {
					$i_et->update($row->RecordID, "Omschrijving", $_POST[$oms]);
				}
				if (isset($_POST[$soort])) {
					$i_et->update($row->RecordID, "Soort", $_POST[$soort]);
				}
				if (isset($_POST[$tk])) {
					$i_et->update($row->RecordID, "Tekstkleur", $_POST[$tk]);
				}
				if (isset($_POST[$vet])) {
					$i_et->update($row->RecordID, "Vet", 1);
				} else {
					$i_et->update($row->RecordID, "Vet", 0);
				}
				if (isset($_POST[$cur])) {
					$i_et->update($row->RecordID, "Cursief", 1);
				} else {
					$i_et->update($row->RecordID, "Cursief", 0);
				}
				if (isset($_POST[$ak])) {
					$i_et->update($row->RecordID, "Achtergrondkleur", $_POST[$ak]);
				}
			}
		}
	}
		
	echo("<div id='muteertypeevenement'>\n");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	echo("<table>\n");
	echo("<tr><th>Omschrijving</th><th>Soort</th><th>Tekstkleur</th><th>Vet</th><th>Cursief</th><th>Achtergrondkleur</th><th>Voorbeeld</th><th></th></tr>\n");
	foreach ($i_et->lijst() as $row) {
		echo("<tr>");
		printf("<td><input type='text' name='oms_%d' value='%s' class='w30' max-length=30 onBlur='this.form.submit();'></td>\n", $row->RecordID, $row->Omschrijving);
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
		
		printf("<td><input type='text' name='tekstkleur_%d' value='%s' class='w12' onBlur='this.form.submit();'></td>\n", $row->RecordID, $row->Tekstkleur);
		
		printf("<td><input type='checkbox'%s name='vet_%d' value=1 onClick='this.form.submit();'></td>", checked($row->Vet), $row->RecordID);
		printf("<td><input type='checkbox'%s name='cursief_%d' value=1 onClick='this.form.submit();'></td>", checked($row->Cursief), $row->RecordID);
		
		printf("<td><input type='text' name='achtergrondkleur_%d' class='w12' value='%s' onBlur='this.form.submit();'></td>\n", $row->RecordID, $row->Achtergrondkleur);
		
		$s = "";
		if (strlen($row->Tekstkleur) > 2 or strlen($row->Achtergrondkleur) > 2) {
			$s = " style='";
			if (strlen($row->Tekstkleur) > 2) {
				$s .= "color: " . $row->Tekstkleur . ";";
			}
			if (strlen($row->Achtergrondkleur) > 2) {
				$s .= "background-color: " . $row->Achtergrondkleur . ";";
			}
			$s .= "'";
		}
		
		echo(fnEvenementOmschrijving($row, 0, "td"));
		
		printf("<td><a href='%s?tp=%s&amp;op=delete&amp;tid=%d' title='Verwijder type evenement'>&nbsp;&nbsp;&nbsp;</a></td>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $row->RecordID);
		echo("</tr>\n");
	}
	echo("</table>\n");
	echo("<input type='text' name='oms_nw' placeholder='Nieuw type' onBlur='this.form.submit();'>");
	echo("</form>\n");
	echo("</div> <!-- Einde muteertypeevenement -->\n");
	
	$i_et = null;
}

function overzichtevenementen() {
	global $dtfmt;

	$i_ev = new cls_Evenement();
	$i_ed = new cls_Evenement_Deelnemer();

	echo("<div id='overzichtevenementen'>\n");
	echo("<table>\n");
	$vsrt = "Q";
	$vn = "";
	
	$dtfmt->setPattern(DTTEXTWD);
	$evlijst = $i_ev->lijst(1);
	foreach ($evlijst as $row) {
		$dlnlijst = $i_ed->overzichtevenement($row->RecordID, "'B','J'");
		
		if ($row->Soort == "W") {
			printf("<tr><th>%s</th><th>Dames</th><th>Heren</th></tr>\n", $dtfmt->format(strtotime($row->Datum)));
		} elseif(count($dlnlijst) > 1) {
			printf("<tr><th>%s</th><th colspan=2>%d deelnemers</th></tr>\n", $dtfmt->format(strtotime($row->Datum)), count($dlnlijst));
		} else {
			printf("<tr><th colspan=3>%s</th></tr>\n", $dtfmt->format(strtotime($row->Datum)));
		}
		
		$dames = "";
		$heren = "";
		$deelnemers = "";
		$ad = 0;
		$vsm = "";
		$vn = "deelnemers";
		
		foreach($dlnlijst as $deeln) {
			if ($row->Soort != "W") {
				$vn = "deelnemers";
				$ad++;
			} elseif ($deeln->Geslacht == "V") {
				$vn = "dames";
			} else {
				$vn = "heren";
			}
			if (strlen($$vn) > 0 and $row->Soort == "W") {
				$$vn .= "<br>\n";
			}
			if ($vsm != $deeln->Starttijd and $deeln->MeerdereStartMomenten == 1) {
				if (empty($vsm) == false) {
					$$vn .= "</ul><br>\n";
				}
				$$vn .= sprintf("<strong>%s</strong>\n<ul>\n", substr($deeln->Starttijd, 0, 5));
				$vsm = $deeln->Starttijd;
			}
			
			$nd = htmlentities($deeln->NaamDeelnemer);
			if ($deeln->Aantal > 1) {
				$nd .= sprintf(" (%dp)", $deeln->Aantal);
			} elseif (strlen($deeln->Functie) > 0) {
				$nd .= sprintf(" (%s)", htmlentities($deeln->Functie));
			}
			if ($row->Soort == "W") {
				$$vn .= $nd;
			} else {
				$$vn .= sprintf("<li>%s</li>\n", $nd);
			}
		}
		$$vn .= "</ul>\n";
		if ($row->Soort == "W") {
			printf("<tr>%s<td>%s</td><td>%s</td>", fnEvenementOmschrijving($row, 3, "td"), $dames, $heren);
		} else {
			printf("<tr>%s<td colpspan=2>%s</td>", fnEvenementOmschrijving($row, 3, "td"), $deelnemers);
		}
		printf("</tr>\n");
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

function fnEvenementOmschrijving($p_row, $p_mettijd=0, $p_element="") {
	
	/*
		$p_mettijd
		0: geen tijd
		1: tijd voor de omschrijving, gebruikt in de agenda
		2: tijd, tussen haakjes, achter de omschrijving
		3: uitgebreid voor overzicht 
	*/
	
	if ($p_mettijd == 3) {
	
		$eo = sprintf("<strong>%s</strong>", $p_row->Omschrijving);
		if ($p_row->Starttijd > "00:00" and $p_row->Eindtijd > "00:00") {
			$eo .= sprintf("<br>\nvan %s tot %s uur", $p_row->Starttijd, $p_row->Eindtijd);
		} elseif ($p_row->Starttijd > "00:00") {
			$eo .= "<br>\nStart:&nbsp;" . $p_row->Starttijd;
		} elseif ($p_row->Eindtijd > "00:00") {
			$eo .= "<br>\nEinde:&nbsp;" . $p_row->Eindtijd;
		}
		
		if (strlen($p_row->Locatie) > 3) {
			$eo .= "<br>\nLocatie: " . $p_row->Locatie;
		}
		if (IsValidMailAddress($p_row->Email, 0)) {
			$eo .= sprintf("<br>\nContact: %s", fnDispEmail($p_row->Email, "", 1));
		}
		
		if (strlen($p_row->Verzameltijd) > 3) {
			$eo .= sprintf("<br>\nVerzamelen:&nbsp;%s uur", $p_row->Verzameltijd);
		}

	} else {
		$eo = $p_row->Omschrijving;
		if (isset($p_row->Datum)) {
			$st = date("H:i", strtotime($p_row->Datum));
			if ($p_mettijd > 0 and $st > "00:00") {
				if ($p_mettijd == 1) {
					$eo = $st . "&nbsp;" . $eo;
				} elseif ($p_mettijd == 2) {
					$eo .= " (" . $st . ")";
				}
			}
		}
		if ($p_mettijd == 1 and strlen($p_row->Locatie) > 1) {
			$eo .= sprintf(" (%s)", $p_row->Locatie);
		}
	}
	
	$s = "";
	if (strlen($p_row->Tekstkleur) > 2 or strlen($p_row->Achtergrondkleur) > 2 or $p_row->Vet == 1 or $p_row->Cursief == 1) {
		$s = " style='";
		if (strlen($p_row->Tekstkleur) > 2) {
			$s .= "color: " . $p_row->Tekstkleur . ";";
		}
		if (strlen($p_row->Achtergrondkleur) > 2) {
			$s .= "background-color: " . $p_row->Achtergrondkleur . ";";
		}
		if ($p_row->Vet == 1) {
			$s .= "font-weight: bold;";
		}
		if ($p_row->Cursief == 1) {
			$s .= "font-style: italic;";
		}
		$s .= "'";
	}
	
	if ($p_element === "td") {
		return sprintf("<td class='%s'%s>%s</td>", str_replace("'", "", str_replace(" ", "_", strtolower($p_row->OmsType))), $s, $eo);
	} elseif ($p_element === "li") {
		return sprintf("<li class='%s'%s>%s</li>", str_replace("'", "", str_replace(" ", "_", strtolower($p_row->OmsType))), $s, $eo);
	} elseif ($p_element === "p") {
		return sprintf("<p class='%s'%s>%s</p>", str_replace("'", "", str_replace(" ", "_", strtolower($p_row->OmsType))), $s, $eo);
	} else {
		return $eo;
	}
	
}  # fnEvenementOmschrijving

?>
