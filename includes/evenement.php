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
		
		printf("<form method='post' id='filter' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
		echo("<input type='text' name='tbTekstFilter' id='tbTekstFilter' title='Tekstfilter op de tabel' placeholder='Tekstfilter' OnKeyUp=\"fnFilter('evenementlijst', this);\">\n");
		echo("<div class='form-check form-switch'>\n");
		$in = "";
		foreach(ARRSOORTEVENEMENT as $k => $v) {
			$f = sprintf("ET.Soort='%s'", $k);
			if ($i_et->aantal($f) > 0) {
				$cn = sprintf("evlijst_%s", $k);
				if ($_SERVER['REQUEST_METHOD'] == "POST") {
					$chk = $_POST[$cn] ?? 0;
					setcookie($cn, $chk, time()+(3600 * 24 * 180));
				} else {
					$chk = 1;
					if (isset($_COOKIE[$cn]) and $_COOKIE[$cn] == "0") {
						$chk = 0;
					}
				}
				printf("<input type='checkbox' class='form-check-input' value=1 name='%s' onClick='this.form.submit();'%s><p>%s</p>\n", $cn, checked($chk), $v);
				if ($chk == 1) {
					if (strlen($in) > 0) {
						$in .= ", ";
					}
					$in .= sprintf("'%s'", $k);
				}
			}
		}
		echo("</div>  <!-- Einde form-check form-switch -->\n");
		
		echo("</form>\n");
		echo("<div class='clear'></div>\n");
		if (strlen($in) > 2) {
			$in = sprintf("ET.Soort IN (%s)", $in);
		}
		$lijst = $i_ev->lijst(2, "", $in);
		
		$l = sprintf("%s?tp=%s&op=edit&eid=%%d' title='Muteer evenement'", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
		$kols[0] = ['class' => "muteren", 'columnname' => "RecordID", 'link' => $l];
		
		$kols[1] = ['columnname' => "Datum", 'type' => "date"];
		
		$kols[2]['columnname'] = "Starttijd";
		$kols[3]['headertext'] = "Omschrijving";
		$kols[4]['headertext'] = "Locatie";
		$kols[5] = array(['headertext' => "Dln", 'type' => "integer"]);
		$kols[6]['headertext'] = "Email";
		$kols[7]['headertext'] = "Einddtijd";
		$kols[8]['headertext'] = "Ins open?";
		$kols[9]['headertext'] = "Type";
		
		if ((new cls_Evenement_Type())->aantal() > 0) {
			$er = sprintf("<td><a href='%s?%s&op=edit&eid=0' title='Nieuw evenement'><img src='images/star.png' alt='Nieuw'></a></td><td colspan=10>Nieuw evenement</td>", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
		} else {
			$er = "";
		}
		echo(fnDisplayTable($lijst, $kols, "", 0, "", "evenementenbeheer"));
		
	} elseif ($currenttab2 == "Presentielijst") {
		presentielijst(1);
		
	} elseif ($currenttab2 == "Groepen muteren") {
		if (isset($_GET['OnderdeelID']) and $_GET['OnderdeelID'] > 0) {
			LedenOnderdeelMuteren($_GET['OnderdeelID'], 1);
		} else {
			persoonlijkeGroepMuteren();
		}
		
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
	$i_mv = new cls_Mailing_vanaf();

	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		foreach ($i_ev->lijst(3) as $row) {
			$rv = 0;
			$react = sprintf("reactie_%d", $row->RecordID);
			$cn_aantal = sprintf("aantal_%d", $row->RecordID);
			$opm = sprintf("opm_%d", $row->RecordID);
			$insrow = $i_ed->record(0, $lidid, $row->RecordID);
			if (isset($insrow->RecordID)) {
				$edid = $insrow->RecordID;
			} else {
				$edid = 0;
			}
			
			if ($_POST[$react] == "Inschrijven") {
				$stat = "I";
			} elseif ($_POST[$react] == "Aanmelden" and $row->StandaardStatus == "B") {
				$stat = $row->StandaardStatus;
			} elseif ($_POST[$react] == "Geen") {
				$stat = "G";
			} elseif ($_POST[$react] == "Aanmelden") {
				$stat = "J";
			} elseif ($_POST[$react] == "Afmelden") {
				$stat = "X";
			} else {
				$stat = "";
			}
			$aantal = $_POST[$cn_aantal] ?? 1;
			if ($stat != "G" or strlen($_POST[$opm]) > 0 or $edid > 0) {
				if ($edid == 0) {
					$edid = $i_ed->add($row->RecordID, $lidid, $stat);
					$rv += $edid;
				}
				if (strlen($stat) == 1) {
					$rv += $i_ed->update($edid, "Status", $stat);
				}
				
				$rv += $i_ed->update($edid, "Aantal", $aantal);
								
				if (isset($_POST[$opm])) {
					$rv += $i_ed->update($edid, "Opmerking", $_POST[$opm]);
				}
			}
		
			if ($_SESSION['settings']['mailing_bevestigingdeelnameevenement'] > 0 and $rv > 0 and $stat != "G") {
				$mailing = new Mailing($_SESSION['settings']['mailing_bevestigingdeelnameevenement']);
				$mailing->xtrachar = "EVD";
				$mailing->xtranum = $edid;
				
				$i_mv->vulvars(-1, $row->EmailOrganisatie);
				if ($i_mv->mvid > 0) {
					$mailing->mailingvanafid = $i_mv->mvid;
				}
			
				if ($mailing->send($lidid, 0) > 0) {
					$mess = sprintf("Bevestiging deelname evenement (%d) is aan %s verzonden.", $edid, (new cls_Lid())->Naam($lidid));
				} else {
					$mess = sprintf("Fout bij het versturen van de e-mail. Probeer het later nogmaals of neem contact op met de webmaster.");
				}
				$mailing = null;
				(new cls_Logboek())->add($mess, 7, $lidid, 0, $edid, 14);
			}
		}
	}

	fnDispMenu(2);

	echo("<div id='inschrijvingevenementen'>\n");
	printf("<form method='post' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	$geldig = false;
	printf("<table class='%s'>\n", TABLECLASSES);
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
		printf("<input type='radio' name='reactie_%d' value='Geen' title='Geen reactie' checked>Geen&nbsp;", $row->RecordID);
		
		if (isset($ins->Status) and ($ins->Status == "B" or $ins->Status == "I" or $ins->Status == "J")) {
			$c = "checked";
		} else {
			$c = "";
		}
		
		if ($row->StandaardStatus == "B" or $row->StandaardStatus == "J") {
			printf("<input type='radio' name='reactie_%d' value='Aanmelden' title='Aanmelding voor evenement' %s>Aanmelden&nbsp;\n", $row->RecordID, $c);
		} else {
			printf("<input type='radio' name='reactie_%d' value='Inschrijven' title='Inschrijven voor evenement' %s>Inschrijven&nbsp;\n", $row->RecordID, $c);
		}
		
		if (isset($ins->Status) and $ins->Status == "X") {
			$c = "checked";
		} else {
			$c = "";
		}
		printf("<input type='radio' name='reactie_%d' value='Afmelden' title='Afmeldingen voor evenement' %s>Afmelden&nbsp;\n", $row->RecordID, $c);
		
		if ($row->MaxPersonenPerDeelname > 1) {
			printf("<br>\n<label class='form-label'>Aantal personen (%2\$d max)</label><input type='number' name='aantal_%1\$d' min=1 max=%2\$d value=%3\$d class='num2' title='Met hoeveel personen kom je?'>", $row->RecordID, $row->MaxPersonenPerDeelname, $ins->Aantal);
		}
		$opm = $ins->Opmerking ?? "";
		printf("<br>\n<input type='text' placeholder='Opmerking' name='opm_%d' class='w250' maxlength=250 value=\"%s\" title='Opmerking bij inschrijving'>", $row->RecordID, str_replace("\"", "'", $opm));
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

	$i_ev = new cls_Evenement($eventid);
	$i_ed = new cls_Evenement_Deelnemer();
	$i_lo = new cls_Lidond();
	$i_et = new cls_Evenement_Type();
	$i_ond = new cls_Onderdeel();
	
	$rowspd = $i_ev->potdeelnemers($eventid);
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {

		if ($eventid == 0 and isset($_POST['btnToevoegen'])) {
			
			$eventid = $i_ev->add();
			
			if (isset($_POST['Datum']) and strlen($_POST['Datum']) == 10) {
				$d = $_POST['Datum'];
				if (isset($_POST['Starttijd']) and strlen($_POST['Starttijd']) > 4) {
					$d .= " " . $_POST['Starttijd'];
				}
			} else {
				$d = date("Y-m-d");
			}
			$i_ev->update($eventid, "Datum", $d);
			
			if (isset($_POST['Omschrijving']) and strlen($_POST['Omschrijving']) > 0) {
				$i_ev->update($eventid, "Omschrijving", $_POST['Omschrijving']);
			}
			if (isset($_POST['Locatie']) and strlen($_POST['Locatie']) > 0) {
				$i_ev->update($eventid, "Locatie", $_POST['Locatie']);
			}
			
			if (isset($_POST['TypeEvenement']) and $_POST['TypeEvenement'] > 0) {
				$i_ev->update($eventid, "TypeEvenement", $_POST['TypeEvenement']);
				
				$f = sprintf("E.TypeEvenement=%d AND IFNULL(E.VerwijderdOp, '0000-00-00') < '1970-01-01'", $_POST['TypeEvenement']);
				$org = $i_ev->laatste("Organisatie", $f, "E.Datum DESC");
				if ($org > 0) {
					$i_ev->update($eventid, "Organisatie", $org);
				}
				
				$dg = $i_ev->laatste("BeperkTotGroep", $f, "E.Datum DESC");
				if ($dg > 0) {
					$i_ev->update($eventid, "BeperkTotGroep", $dg);
				}
			}
		}
		
		if (isset($_POST['lidid_nwdln']) and $_POST['lidid_nwdln'] > 0) {
			$f = sprintf("LidID=0 AND EvenementID=%d", $eventid);
			$edid = $i_ed->max("RecordID", $f);
			$i_ed->update($edid, "LidID", $_POST['lidid_nwdln']);
		}
		
		if (isset($_POST['BeperkTotGroep'])) {
			$i_ev->update($eventid, "BeperkTotGroep", $_POST['BeperkTotGroep']);
		}

		if (isset($_POST['verwijderen'])) {
			$i_ev->update($eventid, "VerwijderdOp", date("Y-m-d"));
		} elseif (isset($_POST['undoverwijderen'])) {
			$i_ev->update($eventid, "VerwijderdOp", "0000-00-00");
		}
		
		if (isset($_POST['btnDlnToevoegen']) and $eventid > 0) {
			$i_ed->add($eventid, 0);
			
		} elseif (isset($_POST['btnDoelgroepToevoegen']) and $eventid > 0) {
			foreach ($rowspd as $rowpd) {
				$i_ed->add($eventid, $rowpd->LidID);
			}

		} elseif (isset($_POST['nwdln']) and $_POST['nwdln'] > 0 and is_numeric($_POST['nwdln'])) {
			$i_ed->add($eventid, $_POST['nwdln']);
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
	
	$aantdln = 0;
	$aantalingeschreven = 0;
	$aantafgemeld = 0;
	
	if ($eventid > 0) {
		$row = $i_ev->record($eventid);
		$i_ev->vulvars($eventid);
		if (isset($row->RecordID)) {
			$aantdln = $row->AantDln;
			$aantalingeschreven = $row->AantInschr;
			$aantafgemeld = $row->AantAfgemeld;
			
			$rowspd = $i_ev->potdeelnemers($i_ev->evid);
		}
	}
	
	$optionstandstatus = "";
	foreach (ARRDLNSTATUS as $ds => $o) {
		$s = checked($i_ev->standaardstatus, "option", $ds);
		$optionstandstatus .= sprintf("<option value='%s' %s>%s</option>\n", $ds, $s, $o);
	}
	
	echo("<div id='evenementenbeheer'>\n");
	printf("<form method='post' name='evenementenbeheer' action='%s?tp=Evenementen/Beheer&op=edit&eid=%d'>\n", $_SERVER['PHP_SELF'], $eventid);
	printf("<input type='hidden' name='eventid' value=%d>\n", $eventid);

	printf("<label id='lblDatum' class='form-label'>Datum</label><input type='date' id='Datum' name='Datum' value='%s' title='Datum evenement'>\n", $i_ev->evdatum);
	printf("<label id='lblStarttijd' class='form-label'>Starttijd</label><input type='time' id='Starttijd' name='Starttijd' value='%s' title='Starttijd evenement'>\n", $i_ev->starttijd);
	if ($eventid > 0) {
		printf("<label id='lblEindtijd' class='form-label'>Eindtijd</label><input type='time' id='Eindtijd' value='%s' title='Eindtijd evenement'>\n", $i_ev->eindtijd);
		printf("<label id='lblVerzameltijd' class='form-label'>Verzamelen</label><input type='time' id='Verzameltijd' value='%s' title='Verzameltijd evenement'>\n", $i_ev->verzameltijd);
	}

	printf("<label id='lblOmschrijving' class='form-label'>Omschrijving</label><input type='text' id='Omschrijving' name='Omschrijving' class='w50' value=\"%s\" maxlength=50 title='Omschrijving evenement'>\n", $i_ev->omschrijving);
	printf("<label id='lblLocatie' class='form-label'>Locatie</label><input type='text' id='Locatie' name='Locatie' value=\"%s\" class='w75' maxlength=75 title='Locatie evenement'>\n", $i_ev->locatie);
	printf("<label id='lblTypeEvenement' class='form-label'>Type evenement</label><select id='TypeEvenement' class='form-select' name='TypeEvenement'>\n<option value=0>Geen/onbekend</option>\n%s</select>\n", $i_et->htmloptions($i_ev->typeevenement));
	
	if ($eventid > 0) {
		$f = sprintf("(O.RecordID=%d OR ((O.Kader=1 OR O.`Type`IN ('A', 'G', 'R')) AND IFNULL(O.VervallenPer, '9999-12-31') >= '%s'", $i_ev->organisatie, $i_ev->evdatum);
		if (WEBMASTER == false) {
			$f .= sprintf(" AND O.RecordID IN (%s)", $_SESSION['lidgroepen']);
		}
		$f .= "))";
		printf("<label id='lblOrganisatie' class='form-label'>Organisatie</label><select id='Organisatie' class='form-select'><option value=0>Onbekend</option>\n%s</select>\n", $i_ond->htmloptions($i_ev->organisatie, 0, "", $f, 0));
		if (strlen($i_ev->omschrijving) > 0) {
			printf("<label id='lblOpmaak' class='form-label'>Opmaak in agenda</label>%s\n", fnEvenementOmschrijving($row, 1, "p"));
		}
	
		$ondrows = (new cls_Onderdeel())->lijst(1, "", $i_ev->evdatum);
		printf("<label id='lblInschrijvingOpen' class='form-label'>Inschrijving online</label><input type='checkbox' class='form-check-input' id='InschrijvingOpen' value=1 %s title='Is de online-inschrijving open?'>\n", checked($i_ev->inschrijvingopen));
		printf("<label id='lblStandaardStatus' class='form-label'>Standaard status</label><select id='StandaardStatus' class='form-select'>%s</select>\n", $optionstandstatus);
		printf("<label id='lblMaxPersonenPerDeelname' class='form-label'>Max. per deelname</label><input type='number' id='MaxPersonenPerDeelname' class='num2' value=%d min=1 max=99 title='Met hoeveel personen mag je maximaal komen?'>\n", $i_ev->maxpersonenperdeelname);
		printf("<label id='lblMeerdereStartmomenten' class='form-label'>Meerdere startmomenten</label><input type='checkbox' class='form-check-input' id='MeerdereStartMomenten' value=1 %s title='Kunnen deelnemers verschillende starttijden hebben?'>\n", checked($i_ev->meerderestartmomenten));
		
		$optiongroepen = "<option value=0>Iedereen</option>\n";
		foreach ($ondrows as $t) {
			if ($t->RecordID == $i_ev->doelgroep) {
				$s = " selected";
			} else {
				$s = "";
			}
			$optiongroepen .= sprintf("<option value=%d%s>%s</option>\n", $t->RecordID, $s, htmlentities($t->Naam));
		}
		printf("<label id='lblDoelgroep' class='form-label'>Doelgroep</label><select name='BeperkTotGroep' class='form-select' onChange='this.form.submit();'>\n%s</select>\n", $optiongroepen);
		
		$dtfmt->setPattern(DTLONG);
		printf("<label id='lblGewijzigd'>Gewijzigd op / door</label><p>%s / %s</p>\n", $dtfmt->format(strtotime($row->Gewijzigd)), htmlentities($row->GewijzigdDoorNaam));
		printf("<label id='lblDeelnemers'>Aantal deelnemers</label><p>%d</p>\n", $aantdln);
		if ($aantalingeschreven > 0) {
			printf("<label id='lblInschreven' class='form-label'>Aantal ingeschreven</label><p>%d</p>\n", $aantalingeschreven);
		}
		if ($aantafgemeld > 0) {
			printf("<label id='lblAfgemeld' class='form-label'>Aantal afgemeld</label><p>%d</p>\n", $aantafgemeld);
		}
	}
	
	if ($eventid > 0) {
		printf("<table id='deelnemersevenementmuteren' class='%s'>\n", TABLECLASSES);
		echo("<caption>Deelnemers</caption>\n");
		echo("<thead>\n<tr><th>Deelnemer</th>");
		if ($i_ev->meerderestartmomenten == 1) {
			echo("<th>Start</th>");
		}
		if ($i_ev->maxpersonenperdeelname > 1) {
			echo("<th>#</th>");
		}
		echo("<th>Status</th><th>Opmerking en functie</th><th class='ingevoerd'>Ingevoerd</th><th></th></tr>\n</thead>\n");
		$dtfmt->setPattern(DTTEXT);
		$uitleg = "";
		foreach ($i_ed->overzichtevenement($i_ev->evid) as $rd) {
			$optstat = "";
			foreach (ARRDLNSTATUS as $key => $val) {
				$s = "";
				if ($key == $rd->Status) {
					$s = "selected";
				}
				$optstat .= sprintf("<option value='%s' %s>%s</option>\n", $key, $s, $val);
			}
			printf("<tr id='rid_%d'>\n", $rd->RecordID);
			if ($i_lo->islid($rd->LidID, $i_ev->doelgroep, $i_ev->evdatum)) {
				$t = "";
			} else {
				$t = " *";
				$uitleg = "* = geen lid van de doelgroep";
			}
			if (strlen($rd->Telefoon) >= 10) {
				$t .= "<br>\n" . $rd->Telefoon;
			}
			if ($rd->LidID == 0) {
				$optionsnieuw = "";
				foreach ($rowspd as $lidrow) {
					$optionsnieuw .= sprintf("<option value=%d>%s</option>\n", $lidrow->LidID, htmlentities($lidrow->Naam));
				}
				
				printf("<td id='naamdln_%1\$d'><select name='lidid_nwdln' class='form-select' onChange='this.form.submit();'><option value=0>Selecteer lid ...</option>\n%2\$s</select></td>\n", $rd->RecordID, $optionsnieuw);
				echo("<td></td><td></td>");
			} else {
				printf("<td id='naamdln_%d' class='ed_status_%s'>%s %s</td>\n", $rd->RecordID, $rd->Status, htmlentities($rd->NaamDeelnemer), $t);
				if ($i_ev->meerderestartmomenten == 1) {
					printf("<td><input type='time' id='StartMoment_%d' value='%s'></td>\n", $rd->RecordID, substr($rd->StartMoment, 0, 5));
				}
				if ($i_ev->maxpersonenperdeelname > 1) {
					printf("<td><input type='number' id='Aantal_%d' value=%d class='num2' min=1 max=%d></td>\n", $rd->RecordID, $rd->Aantal, $i_ev->maxpersonenperdeelname);
				}
				printf("<td><select id='Status_%d' class='form-select'>%s</select>", $rd->RecordID, $optstat);
				echo("</td>\n");
				printf("<td><input type='text' id='Opmerking_%d' value=\"%s\" placeholder='Opmerking' class='w250' maxlength=250>\n", $rd->RecordID, str_replace("\"", "'", $rd->Opmerking));
				printf("<br><input type='text' id='Functie_%d' value=\"%s\" placeholder='Functie' class='w30' maxlength=30></td>\n", $rd->RecordID, str_replace("\"", "'", $rd->Functie));
			}
			printf("<td class='ingevoerd'>%s<br>%s</td>\n", $dtfmt->format(strtotime($rd->Ingevoerd)), (new cls_Lid())->Naam($rd->IngevoerdDoor));
			$l = sprintf("%s?tp=%s&eid=%d&op=delete&edid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $eventid, $rd->RecordID);
			printf("<td><a href='%s'>%s</a></td>", $l, ICONVERWIJDER);
			echo("</tr>\n");
		}
		echo("</table>\n");
		
		if (count($rowspd) > 0 and 1 == 2) {
			$optionsnieuw = "<option value=0>Deelnemer toevoegen ...</option>\n<";
			foreach ($rowspd as $lidrow) {
				$optionsnieuw .= sprintf("<option value=%d>%s</option>\n", $lidrow->LidID, htmlentities($lidrow->Naam));
			}
			printf("<select name='nwdln' class='form-select' onChange='this.form.submit();'>%s</select>\n", $optionsnieuw);
		}
		if (strlen($uitleg) > 0) {
			printf("<p>%s</p>\n", $uitleg);
		}
	}
	echo("<div id='opdrachtknoppen'>\n");
	if ($eventid == 0) {
		printf("<button type='submit' class='%s' name='btnToevoegen'>%s Toevoegen</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
	} elseif ($row->VerwijderdOp < '2012-01-01') {
		$f = sprintf("LidID=0 AND EvenementID=%d", $eventid);
		if ($i_ed->aantal($f) == 0 and count($rowspd) > 0) {
			printf("<button type='submit' class='%s' name='btnDlnToevoegen'>%s Deelnemer</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
		}
		if ($i_ev->doelgroep > 0 and count($rowspd) > 0) {
			printf("<button type='submit' class='%s' name='btnDoelgroepToevoegen'>%s Doelgroep</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
		}
		printf("<button type='submit' class='%s' name='Bewaren'>%s Bewaren</button>\n", CLASSBUTTON, ICONBEWAAR);
		printf("<button type='submit' class='%s' name='Bewaren_Sluiten'>%s Bewaren & Sluiten</button>\n", CLASSBUTTON, ICONSLUIT);
	}
	
	if ($eventid > 0 and $row->VerwijderdOp < '2012-01-01' and $aantdln > 0 and toegang("Mailing/Nieuw", 0, 0)) {
		printf("<button type='submit' class='%s' name='maildeeln'>%s</i> Mailing deelnemers (%d)</button>\n", CLASSBUTTON, ICONVERSTUUR, $aantdln);
	}
	if ($eventid > 0 and $row->VerwijderdOp < '2012-01-01' and count($rowspd) > 1 and toegang("Mailing/Nieuw", 0, 0)) {
		printf("<button type='submit' class='%s' name='mailpotdeeln'>%s Mailing potenti&euml;le deelnemers (%d)</button>\n", CLASSBUTTON, ICONVERSTUUR, count($rowspd));
	}
	if ($eventid > 0 and (WEBMASTER or $row->IngevoerdDoor == $_SESSION['lidid'])) {
		if ($row->VerwijderdOp > '2012-01-01') {
			printf("<input type='submit' class='%s' name='undoverwijderen' value='Verwijderen terugdraaien'>\n", CLASSBUTTON);
		} else {
			printf("<button type='submit' class='%s' name='verwijderen'>%s Verwijderen</button>\n", CLASSBUTTON, ICONVERWIJDER);
		}
	}
	echo("</div>  <!-- Einde opdrachtknoppen -->\n");
	echo("</form>\n");
	echo("</div> <!-- Einde evenementenbeheer -->\n");
	
	printf("<script>
		\$( document ).ready(function() {
			\$(\"input\").on('blur', function(){
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
			
			\$('#StandaardStatus').on('change', function(){
				savedata('evenement', %1\$d, this);
			});
			
			\$('#Organisatie, #TypeEvenement').on('change', function(){
				savedata('evenement', %1\$d, this);
			});
			
			\$('#MaxPersonenPerDeelname').on('change', function(){
				savedata('evenement', %1\$d, this);
			});		
			
			\$(\"select[id^='Status_'], input[id^='Functie_']\").on('change', function() {
				savedata('evenementdln', 0, this);
				
				var rid = this.id.split('_')[1];
				\$('#naamdln_' + rid).removeClass();
				\$('#naamdln_' + rid).addClass('ed_status_' + this.value)
			});
			
			\$(\"input[id^='StartMoment_'], input[id^='Opmerking_'], input[id^='Functie_']\").on('blur', function() {
				savedata('evenementdln', 0, this);
			});
			
			\$(\"input[id^='Aantal_']\").on('change', function() {
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
		if (isset($_POST['oms_nw'])) {
			$i_et->add();
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
		
	printf("<form method='post' id='muteertypeevenement' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<table class='%s'>\n", TABLECLASSES);
	echo("<thead>\n");
	echo("<tr><th>Omschrijving</th><th>Soort</th><th>Tekstkleur</th><th>Vet</th><th>Cursief</th><th>Achtergrondkleur</th><th>Voorbeeld</th><th></th></tr>\n");
	echo("</thead>\n");
	echo("<tbody>\n");
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
		printf("<td><select name='soort_%d' class='form-select' onChange='this.form.submit();'>%s</select></td>", $row->RecordID, $options);
		
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
		
		printf("<td><a href='%s?tp=%s&op=delete&tid=%d' title='Verwijder type evenement'>%s</i></a></td>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $row->RecordID, ICONVERWIJDER);
		echo("</tr>\n");
	}
	echo("</tbody>\n");
	echo("</table>\n");
	echo("<div id='opdrachtknoppen'>\n");
	printf("<button type='submit' class='%s' name='oms_nw'>%s Type</button>", CLASSBUTTON, ICONTOEVOEGEN);
	echo("</div> <!-- Einde opdrachtknoppen -->\n");
	echo("</form>\n");
	
	$i_et = null;
}  # muteertypeevenement

function overzichtevenementen() {
	global $dtfmt;

	$i_ev = new cls_Evenement();
	$i_ed = new cls_Evenement_Deelnemer();

	echo("<div id='overzichtevenementen'>\n");
	$vsrt = "Q";
	$vn = "";
	$dtfmt->setPattern(DTTEXTWD);
	$evlijst = $i_ev->lijst(1);
	foreach ($evlijst as $row) {
		echo("<div class='row'>\n");
		
		printf("<div class='col col-sm-4'>%s</div>\n", fnEvenementOmschrijving($row, 3));
		$dlnlijst = $i_ed->overzichtevenement($row->RecordID, "'B','J'");
		
		echo("<div class='col'>\n");
		
		if ($row->Soort == "B" and count($dlnlijst) > 1) {
			printf("<p><strong>%d bewakers</strong></p>\n", count($dlnlijst));
		} elseif (count($dlnlijst) > 1) {
			printf("<p><strong>%d deelnemers</strong></p>\n", count($dlnlijst));
		}
		echo("<ul>\n");
		foreach($dlnlijst as $deeln) {
			$nd = htmlentities($deeln->NaamDeelnemer);
			if ($deeln->Aantal > 1) {
				$nd .= sprintf(" (%dp)", $deeln->Aantal);
			} elseif (strlen($deeln->Functie) > 0) {
				$nd .= sprintf(" (%s)", htmlentities($deeln->Functie));
			}
			printf("<li>%s</li>\n", $nd);
		}
		echo("</ul>\n");
		echo("</div> <!-- Einde col -->\n");
		echo("</div> <!-- Einde row -->\n");
	}
	
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
	global $dtfmt;
	
	/*
		$p_mettijd
		0: geen tijd
		1: tijd voor de omschrijving, gebruikt in de agenda
		2: tijd, tussen haakjes, achter de omschrijving
		3: uitgebreid voor overzicht 
	*/
	
	if ($p_mettijd == 3) {
	
		$eo = sprintf("<p><strong>%s</strong></p>\n", $p_row->Omschrijving);
		$eo .= "<p>" . $dtfmt->format(strtotime($p_row->Datum));
		if ($p_row->Starttijd > "00:00" and $p_row->Eindtijd > "00:00") {
			$eo .= sprintf(" van %s tot %s uur", $p_row->Starttijd, $p_row->Eindtijd);
		} elseif ($p_row->Starttijd > "00:00") {
			$eo .= " vanaf&nbsp;" . $p_row->Starttijd . "&nbsp;uur";
		} elseif ($p_row->Eindtijd > "00:00") {
			$eo .= " tot&nbsp;" . $p_row->Eindtijd . "&nbsp;uur";
		}
		$eo .= "</p>\n";
		
		if (strlen($p_row->Locatie) > 4) {
			$eo .= "<p>Locatie: " . $p_row->Locatie . "</p>\n";
		}
		if (strlen($p_row->OrgNaam) > 0) {
			$eo .= sprintf("<p>Contact: %s", $p_row->OrgNaam);
			if (IsValidMailAddress($p_row->Email, 0)) {
				$eo .= sprintf(" (%s)", fnDispEmail($p_row->Email, "", 1));
			}
			$eo .= "</p>\n";
		}
		
		if (strlen($p_row->Verzameltijd) > 3) {
			$eo .= sprintf("<p>Verzamelen:&nbsp;%s uur</p>\n", $p_row->Verzameltijd);
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
	
	$cl = str_replace("'", "", str_replace(" ", "_", trim(strtolower($p_row->OmsType))));
	if (isset($p_row->Locatie) and strlen($p_row->Locatie) > 1) {
		$cl .= " " . str_replace("'", "", str_replace(" ", "_", trim(strtolower($p_row->Locatie))));
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
		return sprintf("<td class='%s'%s>%s</td>", $cl, $s, $eo);
	} elseif ($p_element === "li") {
		return sprintf("<li class='%1\$s'%2\$s title=\"%3\$s\">%3\$s</li>", $cl, $s, $eo);
	} elseif ($p_element === "p") {
		return sprintf("<p class='%s'%s>%s</p>", $cl, $s, $eo);
	} else {
		return $eo;
	}
	
}  # fnEvenementOmschrijving

?>
