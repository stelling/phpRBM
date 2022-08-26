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
		$kols[0]['link'] = sprintf("<a href='%s?tp=%s&amp;op=edit&amp;eid=%%d' title='Muteer evenement'>&nbsp;&nbsp;&nbsp;</a>", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
		$kols[0]['class'] = "muteren";
		$kols[0]['columnname'] = "RecordID";
		$kols[1]['headertext'] = "Datum";
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
			$er = sprintf("<td><a href='%s?%s&amp;op=edit&amp;eid=0' title='Nieuw evenement'><img src='images/star.png' alt='Nieuw'></a></td><td colspan=10>Nieuw evenement</td>", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
		} else {
			$er = "";
		}
		echo(fnDisplayTable($lijst, $kols));
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
		if ($row->Eindtijd > "00:00") {
			$tijden = sprintf("van %s tot %s uur", strftime("%R", strtotime($row->Datum)), $row->Eindtijd);
		} else {
			$tijden = sprintf("vanaf %s uur", strftime("%R", strtotime($row->Datum)));
		}
		printf("<tr><td>%s<br>%s</td><td>%s</td>\n", strftime("%A %e %B %Y", strtotime($row->Datum)), $tijden, $row->Omschrijving);
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
			printf("<br>\n<input type='text' placeholder='Opmerking' onBlur='this.form.submit();' name='opm_%d' maxlength=250 value=\"%s\"></td>\n", $row->RecordID, $ins->Opmerking);
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
	
}  # inschrijvenevenementen

function muteerevenement($eventid) {

	$i_ev = new cls_Evenement();
	$i_ed = new cls_Evenement_Deelnemer();
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (strtotime($_POST['Datum']) === FALSE) {
			$_POST['Datum'] = date('Y-m-d');
		} else {
			$_POST['Datum'] = date('Y-m-d', strtotime($_POST['Datum']));
		}
		if (isset($_POST['Starttijd']) and strlen($_POST['Starttijd']) == 5) {
			$_POST['Datum'] .= " " . $_POST['Starttijd'] . ":00";
		}

		if ($eventid == 0) {
			$eventid = $i_ev->add($_POST['TypeEvenement']);
		}
		
		$old = $i_ev->record($eventid);
		foreach($_POST as $key => $val) {
			if (property_exists($old, $key)) {
				$i_ev->update($eventid, $key, $_POST[$key]);
			}
		}

		if (isset($_POST['verwijderen']) and $old->VerwijderdOp < '2012-01-01') {
			$i_ev->update($eventid, "VerwijderdOp", date("Y-m-d"));
		} elseif (isset($_POST['undoverwijderen']) and $old->VerwijderdOp > '2012-01-01') {
			$i_ev->update($eventid, "VerwijderdOp", "0000-00-00");
		}
		foreach ($i_ed->overzichtevenement($eventid) as $rd) {
			foreach(array("Status", "StartMoment", "Opmerking", "Functie", "Aantal") as $fld) {
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
		$meerderestartmomenten = $row->MeerdereStartMomenten;
		$beperktotgroep = $row->BeperkTotGroep;
		$zichtbaarvoor = $row->ZichtbaarVoor;
		$aantdln = $row->AantDln;
		$aantalingeschreven = $row->AantInschr;
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
		$meerderestartmomenten = 0;
		$beperktotgroep = 0;
		$zichtbaarvoor = 0;
		$aantdln = 0;
		$aantalingeschreven = 0;
		$aantafgemeld = 0;
	}
	
	$optiontypes = (new cls_Evenement_Type())->htmloptions($typeevenement);
	
	$optionstandstatus = "";
	foreach (ARRDLNSTATUS as $ds => $o) {
		$s = checked($standaardstatus, "option", $ds);
		$optionstandstatus .= sprintf("<option value='%s' %s>%s</option>\n", $ds, $s, $o);
	}
	
	echo("<div id='evenementenbeheer'>\n");
	printf("<form method='post' action='%s?tp=Evenementen/Beheer&op=edit'>\n", $_SERVER['PHP_SELF']);
	printf("<input type='hidden' name='eventid' value=%d>\n", $eventid);
	
	echo("<span>\n");	
	printf("<label>Datum</label><input type='date' id='Datum' name='Datum' value='%s'>\n", $datum);
	printf("<label>Starttijd</label><input type='time' name='Starttijd' value='%s'>\n", $starttijd);
	printf("<label>Eindtijd</label><input type='time' name='Eindtijd' value='%s'>\n", $eindtijd);
	printf("<label>Verzamelen</label><input type='time' name='Verzameltijd' value='%s'>\n", $verzameltijd);
	echo("</span>\n");
	echo("<span>\n");
	printf("<label>Omschrijving</label><input type='text' name='Omschrijving' value='%s' maxlength=50>\n", $oms);
	printf("<label>Locatie</label><input type='text' name='Locatie' value='%s' maxlength=75>\n", $loc);
	echo("</span>\n");
	echo("<span>\n");
	printf("<label>E-mail</label><input type='email' name='Email' value='%s' maxlength=45>\n", $email);
	echo("</span>\n");
	
	echo("<span>\n");
	printf("<label>Type evenement</label><select name='TypeEvenement' onChange='this.form.submit();'>%s</select>\n", $optiontypes);

	echo("</span>\n");
	
	if ($eventid > 0) {
		$ondrows = (new cls_Onderdeel())->lijst(1, "", $datum);
		echo("<span>\n");
		printf("<label>Online inschrijving open</label><input type='checkbox' id='InschrijvingOpen' value=1 %s>\n", checked($insopen));
		printf("<label>Standaard status</label><select name='StandaardStatus'>%s</select>\n", $optionstandstatus);
		printf("<label>Max. pers. per deelname</label><input type='number' name='MaxPersonenPerDeelname' value=%d min=1 onChange='this.form.submit();'>\n", $maxpersonenperdeelname);
		printf("<label>Meerdere startmomenten</label><input type='checkbox' id='MeerdereStartMomenten' value=1' %s>\n", checked($meerderestartmomenten));
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
		printf("<label>Zichtbaar voor</label><select name='ZichtbaarVoor'>%s</select>\n", $optiongroepen);
		echo("</span>\n");
		echo("<span>\n");
		printf("<label>Gewijzigd op / door</label><p>%s / %s</p>\n", strftime("%e %B %Y (%H:%M)", strtotime($row->Gewijzigd)), htmlentities($row->GewijzigdDoorNaam));
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
			if ($meerderestartmomenten == 1) {
				printf("<td><input type='time' name='StartMoment_%d' value='%s'></td>\n", $rd->RecordID, substr($rd->StartMoment, 0, 5));
			}
			if ($maxpersonenperdeelname > 1) {
				printf("<td><input type='number' name='Aantal_%d' value=%d min=1 max=%d></td>\n", $rd->RecordID, $rd->Aantal, $maxpersonenperdeelname);
			}
			printf("<td><select name='Status_%d' onChange='this.form.submit();'>%s</select>", $rd->RecordID, $optstat);
			echo("</td>\n");
			printf("<td><input type='text' name='Opmerking_%d' value='%s' placeholder='Opmerking' maxlength=250>\n", $rd->RecordID, $rd->Opmerking);
			printf("<br><input type='text' name='Functie_%d' value='%s' placeholder='Functie' maxlength=30></td>\n", $rd->RecordID, $rd->Functie);
			printf("<td>%s<br>%s</td>\n", strftime("%e %b %Y (%R)", strtotime($rd->Ingevoerd)), (new cls_Lid())->Naam($rd->IngevoerdDoor));
			printf("<td><a href='%s?tp=%s&amp;op=delete&eid=%d&lidid=%d&edid=%d'>&nbsp;&nbsp;&nbsp;</a></td>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $eventid, $rd->LidID, $rd->RecordID);
			echo("</tr>\n");
		}
		echo("</table>\n");
		
		$optionsnieuw = "<option value=0>Deelnemer toevoegen ...</option>\n";
		foreach ($rowpd as $lidrow) {
			$optionsnieuw .= sprintf("<option value=%d>%s</option>\n", $lidrow->LidID, htmlentities($lidrow->Naam));
		}
		printf("<select name='nwdln' onChange='this.form.submit();'>%s</select>\n", $optionsnieuw);
	}
//	echo("</div> <!-- Einde deelnemersevenementmuteren -->\n");
	$aantpotdln = count($rowpd);
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
	
	printf("<script>
		\$( document ).ready(function() {
			\$(\"input[type='checkbox']\").click(function(){
				savedata('evenement', %1\$d, this);
			});
		});
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
				if (isset($_POST[$ak])) {
					$i_et->update($row->RecordID, "Achtergrondkleur", $_POST[$ak]);
				}
			}
		}
	}
		
	echo("<div id='muteertypeevenement'>\n");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	echo("<table>\n");
	echo("<tr><th>Omschrijving</th><th>Soort</th><th>Tekstkleur</th><th>Achtergrondkleur</th><th>Voorbeeld</th><th></th></tr>\n");
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
		
		printf("<td class='%s'%s>%s</td>\n", str_replace("'", "", str_replace(" ", "_", strtolower($row->Omschrijving))), $s, $row->Omschrijving);
		
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

	$i_ev = new cls_Evenement();
	$i_ed = new cls_Evenement_Deelnemer();

	echo("<div id='overzichtevenementen'>\n");
	echo("<table>\n");
	$vsrt = "Q";
	$vn = "";
	
	$evlijst = $i_ev->lijst(1);
	foreach ($evlijst as $row) {
		$dlnlijst = $i_ed->overzichtevenement($row->lnkNummer, "'B','J'");
		
		$k1 = sprintf("<strong>%s</strong>", $row->Omschrijving);
		if ($row->Starttijd > "00:00" and $row->Eindtijd > "00:00") {
			$k1 .= sprintf("<br>\nvan %s tot %s uur", $row->Starttijd, $row->Eindtijd);
		} elseif ($row->Starttijd > "00:00") {
			$k1 .= "<br>\nStart:&nbsp;" . $row->Starttijd;
		} elseif ($row->Eindtijd > "00:00") {
			$k1 .= "<br>\nEinde:&nbsp;" . $row->Eindtijd;
		}
		
		if ($row->ndSoort == "W") {
			printf("<tr><th>%s</th><th>Dames</th><th>Heren</th></tr>\n", strftime("%A %e %B %Y", strtotime($row->dteDatum)));
		} elseif(count($dlnlijst) > 1) {
			printf("<tr><th>%s</th><th>%d deelnemers</th></tr>\n", strftime("%A %e %B %Y", strtotime($row->dteDatum)), count($dlnlijst));
		} else {
			printf("<tr><th>%s</th><th>Deelnemers</th></tr>\n", strftime("%A %e %B %Y", strtotime($row->dteDatum)));
		}
		
		if (strlen($row->Locatie) > 3) {
			$k1 .= "<br>\nLocatie: " . $row->Locatie;
		}
		if (IsValidMailAddress($row->emlEmail, 0)) {
			$k1 .= sprintf("<br>\nContact: %s", fnDispEmail($row->emlEmail, "", 1));
		}
		
		if (strlen($row->Verzameltijd) > 3) {
			$k1 .= sprintf("<br>\nVerzamelen:&nbsp;%s uur", $row->Verzameltijd);
		}
		
		$dames = "";
		$heren = "";
		$deelnemers = "";
		$ad = 0;
		$vsm = "";
		$vn = "deelnemers";
		
		foreach($dlnlijst as $deeln) {
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
			if ($row->ndSoort == "W") {
				$$vn .= $nd;
			} else {
				$$vn .= sprintf("<li>%s</li>\n", $nd);
			}
		}
		$$vn .= "</ul>\n";
		if ($row->ndSoort == "W") {
			printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s", $k1, $dames, $heren);
		} else {
			printf("<tr><td>%s</td><td colspan=2>%s", $k1, $deelnemers);
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
