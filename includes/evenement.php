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
		
		printf("<form method='post' class='form-check form-switch' id='filter' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
		echo("<input type='text' title='Tekstfilter op de tabel' placeholder='Tekstfilter' OnKeyUp=\"fnFilter('evenementenbeheer', this);\">\n");
		$in = "";
		foreach(ARRSOORTEVENEMENT as $k => $v) {
			$f = sprintf("ET.Soort='%s'", $k);
			if ($i_ev->i_et->aantal($f) > 0) {
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
				printf("<label class='form-check-label'><input type='checkbox' class='form-check-input' value=1 name='%s' onClick='this.form.submit();'%s>%s</label>\n", $cn, checked($chk), $v);
				if ($chk == 1) {
					if (strlen($in) > 0) {
						$in .= ", ";
					}
					$in .= sprintf("'%s'", $k);
				}
			}
		}
		echo("</form>\n");
		
		echo("<div class='clear'></div>\n");
		if (strlen($in) > 2) {
			$in = sprintf("ET.Soort IN (%s)", $in);
		}
		$lijst = $i_ev->lijst(2, "", $in);
		
		$l = sprintf("%s?tp=%s&op=edit&eid=%%d", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
		$kols[] = ['columnname' => "RecordID", 'class' => "muteren", 'link' => $l];

		$kols[] = ['columnname' => "Datum", 'type' => "date"];

		$kols[]['columnname'] = "Starttijd";
		$kols[]['columnname'] = "Omschrijving";
		$kols[]['columnname'] = "Locatie";
		$kols[] = array('columnname' => "Dln", 'headertext' => "Dln", 'type' => "integer");
		$kols[] = array ('columnname' => "OrgNaam", 'headertext' => "Organisatie");
		$kols[]['headertext'] = "Einddtijd";
		$kols[] = array('columnname' => "InschrijvingOpen", 'headertext' => "Ins open?", 'type' => "boolean");
		$kols[] = array('columnname' => "typeOms", 'headertext' => "Type");
		
		echo(fnDisplayTable($lijst, $kols, "", 0, "", "evenementenbeheer"));
		
	} elseif ($currenttab2 == "Presentielijst") {
		presentielijst(1);
		
	} elseif ($currenttab2 == "Groepen muteren") {
		if (isset($_GET['OnderdeelID']) and $_GET['OnderdeelID'] > 0) {
			LedenOnderdeelMuteren($_GET['OnderdeelID']);
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

	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		foreach ($i_ev->lijst(3) as $row) {
			$i_ev->vulvars($row->RecordID);
			$i_ed->vulvars($row->RecordID, $lidid, 0);
			$rv = 0;
			$react = sprintf("reactie_%d", $row->RecordID);
			$cn_aantal = sprintf("aantal_%d", $row->RecordID);
			$cn_opm = sprintf("opm_%d", $row->RecordID);

			if ($_POST[$react] == "Inschrijven") {
				$stat = "I";
			} elseif ($_POST[$react] == "Aanmelden" and $row->StandaardStatus == "B") {
				$stat = $row->StandaardStatus;
			} elseif ($_POST[$react] == "Aanmelden") {
				$stat = "J";
			} elseif ($_POST[$react] == "Afmelden") {
				$stat = "X";
			} else {
				$stat = "G";
			}
			$aantal = $_POST[$cn_aantal] ?? 1;
			$opmerk = $_POST[$cn_opm] ?? "";
			if ($stat != "G" or strlen($opmerk) > 0 or $i_ed->edid > 0) {
				if ($i_ed->edid == 0) {
					$i_ed->add($row->RecordID, $lidid, $stat);
					$rv += $i_ed->edid;
				}
				$rv += $i_ed->update($i_ed->edid, "Status", $stat);
				$rv += $i_ed->update($i_ed->edid, "Aantal", $aantal);
				$rv += $i_ed->update($i_ed->edid, "Opmerking", $opmerk);
			}
		
			if ($_SESSION['settings']['mailing_bevestigingdeelnameevenement'] > 0 and $rv > 0 and $stat != "G") {
				$mailing = new Mailing($_SESSION['settings']['mailing_bevestigingdeelnameevenement']);
				$mailing->xtrachar = "EVD";
				$mailing->xtranum = $i_ed->edid;
				
				if (strlen($row->EmailOrganisatie) > 5) {
					$i_mv = new cls_Mailing_vanaf(0, $row->EmailOrganisatie);
					if ($i_mv->mvid > 0) {
						$mailing->setVanaf($i_mv->mvid, 1);
					}
					$i_mv = null;
				}
	
				if ($mailing->send($lidid, 0, 3) > 0) {
					$mess = sprintf("Bevestiging deelname evenement %d is verzonden.", $i_ed->edid);
				} else {
					$mess = sprintf("Fout bij het versturen van de e-mail. Probeer het later nogmaals of neem contact op met de webmaster.");
				}
				$mailing = null;
				(new cls_Logboek())->add($mess, 7, $lidid, 0, $i_ed->edid, 14);
			}
		}
	}

	fnDispMenu(2);

	printf("<form method='post' id='%s' action='%s?%s'>\n", __FUNCTION__, $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	$vtype = "zzzzz";
	foreach ($i_ev->lijst(3) as $row) {
		$i_ev->vulvars($row->RecordID);
		$i_ed->vulvars($i_ev->evid, $lidid, 0);
		if ($vtype != $i_ev->i_et->omschrijving) {
			printf("<div class='row koprow'><div class='col'>%s</div></div>\n", $i_ev->i_et->omschrijving);
			$vtype = $i_ev->i_et->omschrijving;
		}
		$t = "";
		if ($i_ev->eindtijd > "00:00") {
			$t = sprintf(" title='%s'", $i_ev->tijden);
		}
		printf("<div class='row'%s>\n", $t);
		$oms = $i_ev->omschrijving;
		if (strlen($i_ev->locatie) > 1 and strpos($oms, $i_ev->locatie) === false) {
			$oms .= "<br>\n" . $i_ev->locatie;
		}
		$dtfmt->setPattern(DTTEXTWD);
		printf("<div class='col-2'>%s", $i_ev->datumtekst);
		if ($i_ev->starttijd > "00:00") {
			printf(" om %s&nbsp;uur", $i_ev->starttijd);
		}
		
		echo("</div>\n");
		
		printf("<div class='col-2'>%s</div>\n", $oms);
		
		if ($i_ev->maxpersonenperdeelname > 1) {
			echo("<div class='col-4'>");
		} else {
			echo("<div class='col-3'>");
		}

		if ($i_ed->status == "G") {
			$c = " checked";
		} else {
			$c = "";
		}
		printf("<input type='radio' class='btn-check' id='geen_%1\$d' name='reactie_%1\$d' value='Geen' autocomplete='off'%2\$s>", $i_ev->evid, $c);
		printf("<label class='btn btn-outline-secondary btn-sm' for='geen_%d' title='Geen reactie'>Geen reactie</label>\n", $i_ev->evid);

		if ($i_ev->standaardstatus == "B" or $i_ev->standaardstatus == "J") {
			$bt = "Aanmelden";
		} else {
			$bt = "Inschrijven";
		}
		$d = "";
		if ($i_ev->maxdeelnemers > 0 and $i_ev->aantaldeelnemers >= $i_ev->maxdeelnemers and $i_ed->aanwezig == 0) {
			$d = " disabled";
			$bt = "Vol";
		}
		printf("<input type='radio' class='btn-check' id='aanmelden_%1\$d' name='reactie_%1\$d' value='%2\$s' autocomplete='off'%3\$s%4\$s>", $i_ev->evid, $bt, checked($i_ed->aanwezig), $d);
		printf("<label class='btn btn-outline-primary btn-sm' for='aanmelden_%1\$d' title='%2\$s'>%2\$s</label>\n", $i_ev->evid, $bt);

		printf("<input type='radio' class='btn-check' id='afmelden_%1\$d' name='reactie_%1\$d' value='Afmelden' autocomplete='off'%2\$s>", $i_ev->evid, checked($i_ed->status, "checkbox", "X"));
		printf("<label class='btn btn-outline-danger btn-sm' for='afmelden_%d' title='Afmelden'>Afmelden</label>\n", $i_ev->evid);
		
		if ($i_ev->maxpersonenperdeelname > 1) {
			printf("<span><label class='form-label'>Aantal</label><input type='number' name='aantal_%1\$d' min=1 max=%2\$d value=%3\$d class='num2' title='Met hoeveel personen (max %2\$d) kom je?'></span>", $i_ev->evid, $i_ev->maxpersonenperdeelname, $i_ed->aantal);
		}
		echo("</div> <!-- Einde col -->\n");
		
		if ($i_ev->maxpersonenperdeelname > 1) {
			echo("<div class='col-4 opmerking'>\n");
		} else {
			echo("<div class='col-5 opmerking'>\n");
		}
		printf("<textarea placeholder='Opmerking' name='opm_%d' maxlength=250 title='Opmerking bij inschrijving'>%s</textarea>", $i_ev->evid, $i_ed->opmerking);
		echo("</div> <!-- Einde col -->\n");
	
		echo("</div> <!-- Einde row -->\n");
	}
	echo("<div id='opdrachtknoppen'>\n");
	printf("<button type='submit' class='%s'>%s Bevestigen</button>\n", CLASSBUTTON, ICONVERSTUUR);
	echo("</div> <!-- Einde opdrachtknoppen -->\n");
	echo("</form>\n");
	
}  # inschrijvenevenementen

function muteerevenement($eventid) {
	global $dtfmt;

	$i_ev = new cls_Evenement($eventid);
	$i_ed = new cls_Evenement_Deelnemer();
	$i_lo = new cls_Lidond();
	$i_et = new cls_Evenement_Type();
	$i_ond = new cls_Onderdeel();
	$i_lb = new cls_Logboek();
	
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
			$rowspd = $i_ev->potdeelnemers($eventid);
			foreach ($rowspd as $rowpd) {
				$i_ed->add($eventid, $rowpd->LidID);
			}

		} elseif (isset($_POST['btnOpschonen']) and $eventid > 0) {
			$i_ed->opschonen($eventid);
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
			if ($mid > 0) {
				$query = sprintf("SELECT IFNULL(MAX(M.MailingVanafID), 0) FROM %1\$sMailing AS M INNER JOIN %1\$sEvenement AS EV ON M.EvenementID=EV.RecordID WHERE EV.TypeEvenement=%2\$d;", TABLE_PREFIX, $i_ev->typeevenement);
				$mvid = $i_ev->scalar($query);
				$i_m->update($mid, "MailingVanafID", $mvid);
				$i_m->update($mid, "EvenementID", $eventid);
				if (isset($_POST['maildeeln']) or isset($_POST['mailbeidedeeln'])) {
					foreach ($i_ed->overzichtevenement($eventid, "'B', 'I', 'J', 'R', 'T'") as $row) {
						$i_mr->add($mid, $row->LidID);
					}
				}
				if (isset($_POST['mailpotdeeln']) or isset($_POST['mailbeidedeeln'])) {
					$rowspd = $i_ev->potdeelnemers($eventid);
					foreach ($rowspd as $row) {
						$i_mr->add($mid, $row->LidID);
					}
				}
			}
			printf("<script>location.href='%s?tp=Mailing/Wijzigen+mailing&op=edit&mid=%d'</script>\n", $_SERVER['PHP_SELF'], $mid);
			$i_m = null;
			$i_mr = null;
		}
	}

	$rowspd = $i_ev->potdeelnemers($eventid);
	
	$optionstandstatus = "";
	foreach (ARRDLNSTATUS as $ds => $o) {
		$s = checked($i_ev->standaardstatus, "option", $ds);
		$optionstandstatus .= sprintf("<option value='%s'%s>%s</option>\n", $ds, $s, $o);
	}
	
	printf("<form method='post' id='%s' class='form-check form-switch' action='%s?tp=Evenementen/Beheer&op=edit&eid=%d'>\n", __FUNCTION__, $_SERVER['PHP_SELF'], $eventid);
	printf("<input type='hidden' name='eventid' value=%d>\n", $eventid);

	printf("<label id='lblDatum' class='form-label'>Datum</label><input type='date' id='Datum' name='Datum' value='%s' title='Datum evenement'>\n", $i_ev->datum);
	printf("<label id='lblStarttijd' class='form-label'>Starttijd</label><input type='time' id='Starttijd' name='Starttijd' value='%s' title='Starttijd evenement'>\n", $i_ev->starttijd);
	if ($eventid > 0) {
		printf("<label id='lblEindtijd' class='form-label'>Eindtijd</label><input type='time' id='Eindtijd' value='%s' title='Eindtijd evenement'>\n", $i_ev->eindtijd);
		printf("<label id='lblVerzameltijd' class='form-label'>Verzamelen</label><input type='time' id='Verzameltijd' value='%s' title='Verzameltijd evenement'>\n", $i_ev->verzameltijd);
	}

	printf("<label id='lblOmschrijving' class='form-label'>Omschrijving</label><input type='text' id='Omschrijving' name='Omschrijving' class='w50' value=\"%s\" maxlength=50 title='Omschrijving evenement'>\n", $i_ev->omschrijving);
	printf("<label id='lblLocatie' class='form-label'>Locatie</label><input type='text' id='Locatie' name='Locatie' value=\"%s\" class='w75' maxlength=75 title='Locatie evenement'>\n", $i_ev->locatie);
	printf("<label id='lblTypeEvenement' class='form-label'>Type evenement</label><select id='TypeEvenement' class='form-select form-select-sm' name='TypeEvenement'>\n<option value=0>Geen/onbekend</option>\n%s</select>\n", $i_et->htmloptions($i_ev->typeevenement));
	
	if ($eventid > 0) {
		$i_lo->per = $i_ev->datum;
		$i_ond->where = sprintf("(O.RecordID=%d OR ((O.Kader=1 OR O.`Type`IN ('A', 'G', 'R')) AND IFNULL(O.VervallenPer, '9999-12-31') >= '%s'", $i_ev->organisatie, $i_ev->datum);
		if (WEBMASTER == false) {
			$i_ond->where .= sprintf(" AND O.RecordID IN (%s)", $i_lo->lidgroepen());
		}
		$i_ond->where .= "))";
		printf("<label id='lblOrganisatie' class='form-label'>Organisatie</label><select id='Organisatie' class='form-select form-select-sm'><option value=0>Onbekend</option>\n%s</select>\n", $i_ond->htmloptions($i_ev->organisatie, 0, "", 0));
		if (strlen($i_ev->omschrijving) > 0) {
			printf("<label id='lblOpmaak' class='form-label'>Opmaak in agenda</label>%s\n", fnEvenementOmschrijving($i_ev->evid, 1, "p"));
		}

		printf("<label id='lblInschrijvingOpen' class='form-label'>Inschrijving online</label><input type='checkbox' class='form-check-input' id='InschrijvingOpen' value=1 %s title='Is de online-inschrijving open?'>\n", checked($i_ev->inschrijvingopen));
		printf("<label id='lblMaxDeelnemers' class='form-label'>Maximaal aantal deelnemers</label><input type='number' class='num3' id='MaxDeelnemers' value='%d'><p>0 = onbeperkt</p>", $i_ev->maxdeelnemers);
		
		printf("<label id='lblStandaardStatus' class='form-label'>Standaard status</label><select id='StandaardStatus' class='form-select form-select-sm'>\n%s</select>\n", $optionstandstatus);
		printf("<label id='lblMaxPersonenPerDeelname' class='form-label'>Max. per deelname</label><input type='number' id='MaxPersonenPerDeelname' class='num2' value=%d min=1 max=99 title='Met hoeveel personen mag je maximaal komen?'>\n", $i_ev->maxpersonenperdeelname);
		printf("<label id='lblMeerdereStartmomenten' class='form-label'>Meerdere startmomenten</label><input type='checkbox' class='form-check-input' id='MeerdereStartMomenten' value=1%s title='Kunnen deelnemers verschillende starttijden hebben?'>\n", checked($i_ev->meerderestartmomenten));
		
		$i_ond->where = "(NOT O.`Type` IN ('E', 'M', 'O', 'T'))";
		$i_ond->per = $i_ev->datum;
		printf("<label id='lblDoelgroep' class='form-label'>Doelgroep</label><select name='BeperkTotGroep' class='form-select form-select-sm' onChange='this.form.submit();'>\n<option value=0>Iedereen</option>\n%s</select>\n", $i_ond->htmloptions($i_ev->doelgroep, 1, "", 1));

		printf("<label id='lblGewijzigd'>Gewijzigd</label><p>%s</p>\n", $i_ev->laatstemutatie($i_ev->evid, 2, " / "));
		if ($i_ev->aantaldeelnemers > $i_ev->maxdeelnemers and $i_ev->maxdeelnemers > 0) {
			$cl = " class='attentie'";
		} else {
			$cl = "";
		}
		printf("<label id='lblDeelnemers'>Aantal deelnemers</label><p%s>%d</p>\n", $cl, $i_ev->aantaldeelnemers);
		if ($i_ev->aantalingeschreven > 0) {
			printf("<label id='lblInschreven' class='form-label'>Aantal ingeschreven</label><p>%d</p>\n", $i_ev->aantalingeschreven);
		}
		if ($i_ev->aantalafgemeld > 0) {
			printf("<label id='lblAfgemeld' class='form-label'>Aantal afgemeld</label><p>%d</p>\n", $i_ev->aantalafgemeld);
		}
	}
	
	if ($eventid > 0) {
		$opschoonknop = false;
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
			$i_ed->vulvars(0, 0, $rd->RecordID);
			if ($i_ed->status == "G" and strlen($i_ed->opmerking) == 0 and strlen($i_ed->functie) == 0) {
				$opschoonknop = true;
			}
			$optstat = "";
			foreach (ARRDLNSTATUS as $key => $val) {
				$s = "";
				if ($key == $rd->Status) {
					$s = "selected";
				}
				$optstat .= sprintf("<option value='%s' %s>%s</option>\n", $key, $s, $val);
			}
			printf("<tr id='rid_%d'>\n", $rd->RecordID);
			if ($i_lo->islid($rd->LidID, $i_ev->doelgroep, $i_ev->datum)) {
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
				
				printf("<td id='naamdln_%1\$d'><select name='lidid_nwdln' class='form-select form-select-sm' onChange='this.form.submit();'><option value=0>Selecteer lid ...</option>\n%2\$s</select></td>\n", $rd->RecordID, $optionsnieuw);
				echo("<td></td><td></td>");;
				if ($i_ev->meerderestartmomenten == 1) {
					echo("<td></td>");
				}
				if ($i_ev->maxpersonenperdeelname > 1) {
					echo("<td></td>");
				}
				
			} else {
				printf("<td id='naamdln_%d' class='ed_status_%s'>%s %s</td>\n", $i_ed->edid, $i_ed->status, htmlentities($i_ed->i_lid->naam), $t);
				if ($i_ev->meerderestartmomenten == 1) {
					printf("<td><input type='time' id='StartMoment_%d' value='%s' title='Wat is de starttijd voor deze deelnemer?'></td>\n", $i_ed->edid, $i_ed->starttijd);
				}
				if ($i_ev->maxpersonenperdeelname > 1) {
					if ($i_ed->aanwezig == 1) {
						printf("<td><input type='number' id='Aantal_%d' value=%d class='num2' min=1 max=%d title='Voor hoeveel personen geldt deze aanmelding?'></td>", $i_ed->edid, $i_ed->aantal, $i_ev->maxpersonenperdeelname);
					} else {
						echo("<td></td>");
					}
				}
				printf("<td><select id='Status_%d' class='form-select form-select-sm'>%s</select>", $i_ed->edid, $optstat);
				echo("</td>\n");
				printf("<td><input type='text' id='Opmerking_%d' value=\"%s\" placeholder='Opmerking' class='w250' maxlength=250>\n", $rd->RecordID, $i_ed->opmerking);
				printf("<br><input type='text' id='Functie_%d' value=\"%s\" placeholder='Functie' class='w30' maxlength=30></td>\n", $rd->RecordID, $i_ed->functie);
			}
			$lm = laatstemutatie("Evenement_Deelnemer", $i_ed->edid, 11);
			if (strlen($lm) < 10) {
				$dtfmt->setPattern(DTTEXT);
				$lm = $dtfmt->format(strtotime($i_ed->ingevoerd));
			}
			printf("<td class='ingevoerd'>%s</td>\n", $lm);
			$l = sprintf("%s?tp=%s&eid=%d&op=delete&edid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $eventid, $i_ed->edid);
			printf("<td><a href='%s'>%s</a></td>", $l, ICONVERWIJDER);
			echo("</tr>\n");
		}
		echo("</table>\n");
		
		if (strlen($uitleg) > 0) {
			printf("<p>%s</p>\n", $uitleg);
		}
	}
	echo("<div id='opdrachtknoppen'>\n");
	if ($eventid == 0) {
		printf("<button type='submit' class='%s' name='btnToevoegen'>%s Toevoegen</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
	} elseif ($i_ev->verwijderdop < '2012-01-01') {
		$f = sprintf("LidID=0 AND EvenementID=%d", $eventid);
		$d = "";
		if ($i_ed->aantal($f) > 0 or count($rowspd) == 0) {
			$d = " disabled";
		}
		printf("<button type='submit' class='%s' name='btnDlnToevoegen'%s>%s Deelnemer</button>\n", CLASSBUTTON, $d, ICONTOEVOEGEN);
		if ($i_ev->doelgroep > 0 and count($rowspd) > 0) {
			printf("<button type='submit' class='%s' name='btnDoelgroepToevoegen'>%s Doelgroep (%d)</button>\n", CLASSBUTTON, ICONTOEVOEGEN, count($rowspd));
		}
		printf("<button type='submit' class='%s' name='Bewaren' title='Bewaren'>%s</button>\n", CLASSBUTTON, ICONBEWAAR);
		printf("<button type='submit' class='%s' name='Bewaren_Sluiten' title='Bewaren & Sluiten'>%s</button>\n", CLASSBUTTON, ICONSLUIT);
		if ($opschoonknop) {
			printf("<button type='submit' class='%s' name='btnOpschonen'>Deelnemers opschonen</button>\n", CLASSBUTTON);
		}
	}
	
	if ($eventid > 0 and $i_ev->verwijderdop < '2012-01-01' and $i_ev->aantaldeelnemers > 0 and toegang("Mailing/Nieuw", 0, 0)) {
		printf("<button type='submit' class='%s' name='maildeeln' title='Mailing naar deelnemers'>%s Deelnemers (%d)</button>\n", CLASSBUTTON, ICONVERSTUUR, $i_ev->aantaldeelnemers);
	}
	if ($eventid > 0 and $i_ev->doelgroep > 0 and $i_ev->verwijderdop < '2012-01-01' and count($rowspd) > 1 and toegang("Mailing/Nieuw", 0, 0)) {
		printf("<button type='submit' class='%s' name='mailpotdeeln' title='Mailing naar potentiële deelnemers'>%s Potenti&euml;le deelnemers (%d)</button>\n", CLASSBUTTON, ICONVERSTUUR, count($rowspd));
	}
	if ($eventid > 0) {
		if ($i_ev->verwijderdop > '2012-01-01') {
			printf("<input type='submit' class='%s' name='undoverwijderen' value='Verwijderen terugdraaien'>\n", CLASSBUTTON);
		} else {
			printf("<button type='submit' class='%s' name='verwijderen' title='Evenement verwijderen'>%s</button>\n", CLASSBUTTON, ICONVERWIJDER);
		}
	}
	echo("</div>  <!-- Einde opdrachtknoppen -->\n");
	echo("</form>\n");
	
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
	
	$i_et = new cls_Evenement_Type();
	
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
		
	printf("<form method='post' id='%s' action='%s?tp=%s'>\n", __FUNCTION__, $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<table class='%s'>\n", TABLECLASSES);
	echo("<thead>\n");
	echo("<tr><th>Omschrijving</th><th>Soort</th><th>Tekstkleur</th><th>Vet</th><th>Cursief</th><th>Achtergrondkleur</th><th>Voorbeeld</th><th></th></tr>\n");
	echo("</thead>\n");
	echo("<tbody>\n");
	foreach ($i_et->basislijst() as $row) {
		$i_et->vulvars($row->RecordID);
		echo("<tr>");
		printf("<td><input type='text' name='oms_%d' value='%s' class='w30' max-length=30 onBlur='this.form.submit();'></td>\n", $i_et->etid, $i_et->omschrijving);
		$options = "\n";
		foreach (ARRSOORTEVENEMENT as $key => $val) {
			if ($key == $i_et->soort) {
				$s = "selected";
			} else {
				$s = "";
			}	
			$options .= sprintf("<option value='%s' %s>%s</option>\n", $key, $s, $val);
		}
		printf("<td><select name='soort_%d' class='form-select' onChange='this.form.submit();'>%s</select></td>", $i_et->etid, $options);
		
		printf("<td><input type='text' name='tekstkleur_%d' value='%s' class='w12' onBlur='this.form.submit();'></td>\n", $i_et->etid, $i_et->tekstkleur);
		
		printf("<td><input type='checkbox'%s name='vet_%d' value=1 onClick='this.form.submit();'></td>", checked($row->Vet), $i_et->etid);
		printf("<td><input type='checkbox'%s name='cursief_%d' value=1 onClick='this.form.submit();'></td>", checked($row->Cursief), $i_et->etid);
		
		printf("<td><input type='text' name='achtergrondkleur_%d' class='w12' value='%s' onBlur='this.form.submit();'></td>", $i_et->etid, $row->Achtergrondkleur);
		
		printf("<td class='%s'%s>%s</td>", $i_et->evclass, $i_et->style, $i_et->omschrijving);		
		
		if ($i_et->aantalgekoppeldeevenementen == 0) {
			printf("<td><a href='%s?tp=%s&op=delete&tid=%d' title='Verwijder type evenement'>%s</a></td>", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $row->RecordID, ICONVERWIJDER);
		} else {
			printf("<td class='number'>%d</td>", $i_et->aantalgekoppeldeevenementen);
		}
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

	printf("<div id='%s'>\n", __FUNCTION__);
	$vsrt = "Q";
	$vn = "";
	$dtfmt->setPattern(DTTEXTWD);
	$evlijst = $i_ev->lijst(1);
	foreach ($evlijst as $row) {
		echo("<div class='row'>\n");
		
		printf("<div class='col col-sm-4'>%s</div>\n", fnEvenementOmschrijving($row->RecordID, 3));
		$dlnlijst = $i_ed->overzichtevenement($row->RecordID, "'B','J'", 1);
		
		echo("<div class='col'>\n");
		
		$f = sprintf("ED.Status IN ('B','J') AND ED.EvenementID=%d AND ED.LidID > 0", $row->RecordID);
		$ap = $i_ed->totaal("Aantal", $f);
		
		if ($row->Soort == "B" and count($dlnlijst) > 1) {
			printf("<p><strong>%d bewakers</strong></p>\n", count($dlnlijst));
		} elseif (count($dlnlijst) > 1 and $ap > count($dlnlijst)) {
			printf("<p><strong>%d deelnemers met %d personen</strong></p>\n", count($dlnlijst), $ap);
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

function fnEvenementOmschrijving($p_evid, $p_mettijd=0, $p_element="") {
	$i_ev = new cls_Evenement($p_evid);
	
	/*
		$p_mettijd
		0: geen tijd
		1: tijd voor de omschrijving, gebruikt in de agenda
		2: tijd, tussen haakjes, achter de omschrijving
		3: uitgebreid voor overzicht 
	*/
	
	if ($p_mettijd == 3) {
	
		$eo = sprintf("<p><strong>%s</strong></p>\n", $i_ev->omschrijving);
		$eo .= "<p>" . $i_ev->datumtekst;
		if (strlen($i_ev->tijden) > 4) {
			$eo .= " " . $i_ev->tijden;
		}
		$eo .= "</p>\n";
		
		if (strlen($i_ev->locatie) > 4) {
			$eo .= "<p>Locatie: " . $i_ev->locatie . "</p>\n";
		}
		if ($i_ev->organisatie > 0) {
			$eo .= sprintf("<p>Contact: %s", $i_ev->naamorganisatie);
			if (strlen($i_ev->emailcontact) > 0) {
				$eo .= sprintf(" (%s)", fnDispEmail($i_ev->emailcontact, "", 1));
			}
			$eo .= "</p>\n";
		}
		
		if (strlen($i_ev->verzameltijd) > 3) {
			$eo .= sprintf("<p>Verzamelen:&nbsp;%s uur</p>\n", $i_ev->verzameltijd);
		}

	} else {
		$eo = $i_ev->omschrijving;
		if ($p_mettijd > 0 and $i_ev->starttijd > "00:00") {
			if ($p_mettijd == 1) {
				$eo = $i_ev->starttijd . "&nbsp;" . $eo;
			} elseif ($p_mettijd == 2) {
				$eo .= " (" . $i_ev->starttijd . ")";
			}
		}
		if ($p_mettijd == 1 and strlen($i_ev->locatie) > 1) {
			$eo .= sprintf(" (%s)", $i_ev->locatie);
		}
	}
	
	if ($p_element === "td") {
		return sprintf("<td class='%s'%s>%s</td>", $i_ev->evclass, $i_ev->i_et->style, $eo);
	} elseif ($p_element === "li") {
		return sprintf("<li class='%1\$s'%2\$s title=\"%3\$s\">%3\$s</li>", $i_ev->evclass, $i_ev->i_et->style, $eo);
	} elseif ($p_element == "p") {
		return sprintf("<p class='%s'%s>%s</p>", $i_ev->evclass, $i_ev->i_et->style, $eo);
	} else {
		return $eo;
	}
	
}  # fnEvenementOmschrijving

?>
