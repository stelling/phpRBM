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
		
		printf("<form method='post' id='filter' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
		echo("<input type='text' name='tbTekstFilter' id='tbTekstFilter' title='Tekstfilter op de tabel' placeholder='Tekstfilter' OnKeyUp=\"fnFilter('evenementlijst', this);\">\n");
		echo("<div class='form-check form-switch'>\n");
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
				printf("<span><input type='checkbox' class='form-check-input' value=1 name='%s' onClick='this.form.submit();'%s><p>%s</p></span>\n", $cn, checked($chk), $v);
			}
		}
		echo("</div>  <!-- Einde form-check form-switch -->\n");
		
		echo("</form>\n");
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
		
		$kols[0]['link'] = sprintf("%s?tp=%s&amp;op=edit&amp;eid=%%d' title='Muteer evenement'", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
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
				
				$i_mv->vulvars(-1, $row->EmailOrganisatie);
				if ($i_mv->mvid > 0) {
					$mailing->vanafadres = $i_mv->vanaf_email;
					$mailing->vanafnaam = $i_mv->vanaf_naam;
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
		if (!isset($ins->Status)) {
			printf("<input type='radio' name='reactie_%d' value='Geen' title='Geen reactie' checked>Geen&nbsp;", $row->RecordID);
		}
		
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
			printf("<br>\n<label>Aantal personen (%2\$d max)</label><input type='number' name='aantal_%1\$d' min=1 max=%2\$d value=%3\$d class='num2' title='Met hoeveel personen kom je?'>", $row->RecordID, $row->MaxPersonenPerDeelname, $ins->Aantal);
		}
		if (isset($ins->Opmerking)) {
			printf("<br>\n<input type='text' placeholder='Opmerking' name='opm_%d' class='w250' maxlength=250 value=\"%s\" title='Opmerking bij inschrijving'>", $row->RecordID, str_replace("\"", "'", $ins->Opmerking));
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

	$i_ev = new cls_Evenement($eventid);
	$i_ed = new cls_Evenement_Deelnemer();
	$i_lo = new cls_Lidond();
	$i_et = new cls_Evenement_Type();
	$i_ond = new cls_Onderdeel();
	
	$rowspd = $i_ev->potdeelnemers($eventid);
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {

		if ($eventid == 0 and isset($_POST['btnToevoegen'])) {
			
			$eventid = $i_ev->add();
			
			if (isset($_POST['Datum']) and strlen($_POST['Datum']) >= 10) {
				$d = $_POST['Datum'];
				if (isset($_POST['Starttijd']) and strlen($_POST['Starttijd']) > 4) {
					$d .= " " . $_POST['Starttijd'];
				}
				$i_ev->update($eventid, "Datum", $d);
			}
			
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
				if (strlen($eml) > 5) {
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
	
	$recordid = 0;
	$datum = date("Y-m-d");
	$oms = "";
	$loc = "";
	$org = 0;
	$starttijd = "";
	$eindtijd = "";
	$verzameltijd = "";
	$typeevenement = $i_et->min("RecordID");
	$insopen = 0;
	$standaardstatus = "I";
	$maxpersonenperdeelname = 1;
	$meerderestartmomenten = 0;
	$beperktotgroep = 0;
	$aantdln = 0;
	$aantalingeschreven = 0;
	$aantafgemeld = 0;
	
	if ($eventid > 0) {
		$row = $i_ev->record($eventid);
		if (isset($row->RecordID)) {
			$recordid = $row->RecordID;
			$datum = date("Y-m-d", strtotime($row->Datum));
			$oms = str_replace("\"", "'", $row->Omschrijving);
			$loc = str_replace("\"", "'", $row->Locatie);
			$org = $row->Organisatie;
			$starttijd = date("H:i", strtotime($row->Datum));
			$eindtijd = $row->Eindtijd;
			$verzameltijd = $row->Verzameltijd;
			$typeevenement = $row->TypeEvenement;
			$insopen = $row->InschrijvingOpen;
			$standaardstatus = $row->StandaardStatus;
			$maxpersonenperdeelname = $row->MaxPersonenPerDeelname;
			$meerderestartmomenten = $row->MeerdereStartMomenten;
			$beperktotgroep = $row->BeperkTotGroep;
			$aantdln = $row->AantDln;
			$aantalingeschreven = $row->AantInschr;
			$aantafgemeld = $row->AantAfgemeld;
			
			$rowspd = $i_ev->potdeelnemers($eventid);
		}
	}
	
	$optionstandstatus = "";
	foreach (ARRDLNSTATUS as $ds => $o) {
		$s = checked($standaardstatus, "option", $ds);
		$optionstandstatus .= sprintf("<option value='%s' %s>%s</option>\n", $ds, $s, $o);
	}
	
	echo("<div id='evenementenbeheer'>\n");
	printf("<form method='post' name='evenementenbeheer' action='%s?tp=Evenementen/Beheer&op=edit&eid=%d'>\n", $_SERVER['PHP_SELF'], $eventid);
	printf("<input type='hidden' name='eventid' value=%d>\n", $eventid);

	printf("<span><label id='lblDatum'>Datum</label><input type='date' id='Datum' name='Datum' value='%s' title='Datum evenement'></span>\n", $datum);
	printf("<span><label id='lblStarttijd'>Starttijd</label><input type='time' id='Starttijd' name='Starttijd' value='%s' title='Starttijd evenement'></span>\n", $starttijd);
	if ($eventid > 0) {
		printf("<span><label id='lblEindtijd'>Eindtijd</label><input type='time' id='Eindtijd' value='%s' title='Eindtijd evenement'></span>\n", $eindtijd);
		printf("<span><label id='lblVerzameltijd'>Verzamelen</label><input type='time' id='Verzameltijd' value='%s' title='Verzameltijd evenement'></span>\n", $verzameltijd);
	}

	printf("<label id='lblOmschrijving'>Omschrijving</label><input type='text' id='Omschrijving' name='Omschrijving' class='w50' value=\"%s\" maxlength=50 title='Omschrijving evenement'>\n", $oms);
	printf("<label id='lblLocatie'>Locatie</label><input type='text' id='Locatie' name='Locatie' value=\"%s\" class='w75' maxlength=75 title='Locatie evenement'>\n", $loc);
	printf("<label id='lblTypeEvenement'>Type evenement</label><select id='TypeEvenement' name='TypeEvenement'>\n<option value=0>Geen/onbekend</option>\n%s</select>\n", $i_et->htmloptions($typeevenement));
	
	if ($eventid > 0) {
		$f = sprintf("(O.RecordID=%d OR ((O.Kader=1 OR O.`Type`IN ('A', 'G', 'R')) AND IFNULL(O.VervallenPer, '9999-12-31') >= '%s'", $org, $datum);
		if ($_SESSION['webmaster'] == 0) {
			$f .= sprintf(" AND O.RecordID IN (%s)", $_SESSION['lidgroepen']);
		}
		$f .= "))";
		printf("<span><label id='lblOrganisatie'>Organisatie</label><select id='Organisatie'><option value=0>Onbekend</option>\n%s</select></span>\n", $i_ond->htmloptions($org, 0, "", $f, 0));
	
		printf("<label id='lblOpmaak'>Opmaak in agenda</label>%s\n", fnEvenementOmschrijving($row, 1, "p"));
	
		$ondrows = (new cls_Onderdeel())->lijst(1, "", $datum);
		printf("<label id='lblInschrijvingOpen'>Inschrijving online</label><input type='checkbox' id='InschrijvingOpen' value=1 %s title='Is de online-inschrijving open?'>\n", checked($insopen));
		printf("<label id='lblStandaardStatus'>Standaard status</label><select id='StandaardStatus'>%s</select>\n", $optionstandstatus);
		printf("<span><label id='lblMaxPersonenPerDeelname'>Max. per deelname</label><input type='number' id='MaxPersonenPerDeelname' class='num2' value=%d min=1 max=99 title='Met hoeveel personen mag je maximaal komen?'></span>\n", $maxpersonenperdeelname);
		printf("<span><label id='lblMeerdereStartmomenten'>Meerdere startmomenten</label><input type='checkbox' id='MeerdereStartMomenten' value=1 %s title='Kunnen deelnemers verschillende starttijden hebben?'></span>\n", checked($meerderestartmomenten));
		
		$optiongroepen = "<option value=0>Iedereen</option>\n";
		foreach ($ondrows as $t) {
			if ($t->RecordID == $beperktotgroep) {
				$s = " selected";
			} else {
				$s = "";
			}
			$optiongroepen .= sprintf("<option value=%d%s>%s</option>\n", $t->RecordID, $s, htmlentities($t->Naam));
		}
		printf("<span><label id='lblDoelgroep'>Doelgroep</label><select name='BeperkTotGroep' onChange='this.form.submit();'>\n%s</select></span>\n", $optiongroepen);
		
		$dtfmt->setPattern(DTLONG);
		printf("<label id='lblGewijzigd'>Gewijzigd op / door</label><p>%s / %s</p>\n", $dtfmt->format(strtotime($row->Gewijzigd)), htmlentities($row->GewijzigdDoorNaam));
		printf("<label id='lblDeelnemers'>Aantal deelnemers</label><p>%d</p>\n", $aantdln);
		if ($aantalingeschreven > 0) {
			printf("<label id='lblInschreven'>Aantal ingeschreven</label><p>%d</p>\n", $aantalingeschreven);
		}
		if ($aantafgemeld > 0) {
			printf("<span><label id='lblAfgemeld'>Aantal afgemeld</label><p>%d</p></span>\n", $aantafgemeld);
		}
	}
	
	if ($eventid > 0) {
		printf("<table id='deelnemersevenementmuteren' class='%s'>\n", TABLECLASSES);
		echo("<caption>Deelnemers</caption>\n");
		echo("<thead>\n<tr><th>Deelnemer</th>");
		if ($meerderestartmomenten == 1) {
			echo("<th>Start</th>");
		}
		if ($maxpersonenperdeelname > 1) {
			echo("<th>#</th>");
		}
		echo("<th>Status</th><th>Opmerking en functie</th><th class='ingevoerd'>Ingevoerd</th><th></th></tr>\n</thead>\n");
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
				
				printf("<td id='naamdln_%1\$d'><select name='lidid_nwdln' onChange='this.form.submit();'><option value=0>Selecteer lid ...</option>\n%2\$s</select></td>\n", $rd->RecordID, $optionsnieuw);
				echo("<td></td><td></td>");
			} else {
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
			}
			printf("<td class='ingevoerd'>%s<br>%s</td>\n", $dtfmt->format(strtotime($rd->Ingevoerd)), (new cls_Lid())->Naam($rd->IngevoerdDoor));
			$l = sprintf("%s?tp=%s&eid=%d&op=delete&edid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $eventid, $rd->RecordID);
			printf("<td><a href='%s'><i class='bi bi-trash'></i></a></td>", $l);
			echo("</tr>\n");
		}
		echo("</table>\n");
		
		if (count($rowspd) > 0 and 1 == 2) {
			$optionsnieuw = "<option value=0>Deelnemer toevoegen ...</option>\n<";
			foreach ($rowspd as $lidrow) {
				$optionsnieuw .= sprintf("<option value=%d>%s</option>\n", $lidrow->LidID, htmlentities($lidrow->Naam));
			}
			printf("<select name='nwdln' onChange='this.form.submit();'>%s</select>\n", $optionsnieuw);
		}
		if (strlen($uitleg) > 0) {
			printf("<p>%s</p>\n", $uitleg);
		}
	}
	echo("<div id='opdrachtknoppen'>\n");
	if ($eventid == 0) {
		echo("<button type='submit' name='btnToevoegen'><i class='bi bi-plus-circle'></i> Toevoegen</button>\n");
	} elseif ($row->VerwijderdOp < '2012-01-01') {
		$f = sprintf("LidID=0 AND EvenementID=%d", $eventid);
		if ($i_ed->aantal($f) == 0 and count($rowspd) > 0) {
			echo("<button type='submit' name='btnDlnToevoegen'><i class='bi bi-plus-circle'></i> Deelnemer</button>\n");
		}
		if ($i_ev->doelgroep > 0 and count($rowspd) > 0) {
			echo("<button type='submit' name='btnDoelgroepToevoegen'><i class='bi bi-plus-circle'></i> Doelgroep</button>\n");
		}
		echo("<button type='submit' name='Bewaren'><i class='bi bi-save'></i> Bewaren</button>\n");
		echo("<button type='submit' name='Bewaren_Sluiten'><i class='bi bi-door-closed'></i> Bewaren & Sluiten</button>\n");
	}
	
	if ($eventid > 0 and $row->VerwijderdOp < '2012-01-01' and $aantdln > 0 and toegang("Mailing/Nieuw", 0, 0)) {
		printf("<button type='submit' name='maildeeln'><i class='bi bi-envelope-at'></i> Mailing deelnemers (%d)</button>\n", $aantdln);
	}
	if ($eventid > 0 and $row->VerwijderdOp < '2012-01-01' and count($rowspd) > 1 and toegang("Mailing/Nieuw", 0, 0)) {
		printf("<button type='submit' name='mailpotdeeln'><i class='bi bi-envelope-at'></i> Mailing potenti&euml;le deelnemers (%d)</button>\n", count($rowspd));
	}
	if ($eventid > 0 and ($_SESSION['webmaster'] == 1 or $row->IngevoerdDoor == $_SESSION['lidid'])) {
		if ($row->VerwijderdOp > '2012-01-01') {
			echo("<input type='submit' name='undoverwijderen' value='Verwijderen terugdraaien'>\n");
		} else {
			echo("<button type='submit' name='verwijderen'><i class='bi bi-trash'></i> Verwijderen</button>\n");
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
			
			\$('#StandaardStatus').change(function(){
				savedata('evenement', %1\$d, this);
			});
			
			\$('#Organisatie, #TypeEvenement').change(function(){
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
		
		printf("<td><a href='%s?tp=%s&op=delete&tid=%d' title='Verwijder type evenement'><i class='bi bi-trash'></i></a></td>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $row->RecordID);
		echo("</tr>\n");
	}
	echo("</tbody>\n");
	echo("</table>\n");
	echo("<button type='submit' name='oms_nw' class='btn btn-default'>Nieuw type</button>");
	echo("</form>\n");
	
	$i_et = null;
}  # muteertypeevenement

function overzichtevenementen() {
	global $dtfmt;

	$i_ev = new cls_Evenement();
	$i_ed = new cls_Evenement_Deelnemer();

	echo("<div id='overzichtevenementen'>\n");
	printf("<table class='%s'>\n", TABLECLASSES);
	$vsrt = "Q";
	$vn = "";
	
	$dtfmt->setPattern(DTTEXTWD);
	$evlijst = $i_ev->lijst(1);
	foreach ($evlijst as $row) {
		$dlnlijst = $i_ed->overzichtevenement($row->RecordID, "'B','J'");
		
		if ($row->Soort == "W") {
			printf("<tr><th>%s</th><th>Dames</th><th>Heren</th></tr>\n", $dtfmt->format(strtotime($row->Datum)));
		} elseif($row->Soort == "B" and $row->AantalDln > 1) {
			printf("<tr><th>%s</th><th colspan=2>%d bewakers", $dtfmt->format(strtotime($row->Datum)), $row->AantalDln);
		} elseif($row->AantalDln > 1) {
			printf("<tr><th>%s</th><th colspan=2>%d deelnemers", $dtfmt->format(strtotime($row->Datum)), $row->AantalDln);
			if ($row->AantAfgemeld > 1) {
				printf(" / %d afmeldingen", $row->AantAfgemeld);
			}
			echo("</th></tr>\n");
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
