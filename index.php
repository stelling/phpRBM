<?php

$_GET['tp'] = $_GET['tp'] ?? "";

require('./includes/standaard.inc');

if (isset($_GET['actie']) and $_GET['actie'] == "uitloggen") {
	(new cls_login())->uitloggen($_SESSION['lidid']);
	setcookie("username", "", time()-60);
	setcookie("password", "", time()-60);
	$_SESSION['lidid'] = 0;
	$_SESSION['webmaster'] = 0;
	echo("<script>location.href='/index.php';</script>\n");
	
} elseif ($_SERVER['REQUEST_METHOD'] == "POST" and isset($_POST['Inloggen']) and $_POST['Inloggen'] == "Inloggen") {
	if (strlen($_POST['password']) < 5) {
		$mess = "Om in te loggen is het invullen van een wachtwoord van minimaal 5 karakters vereist.";
		(new cls_Logboek())->add($mess, 1, 0, 2);
	} else {
		$_SESSION['username'] = cleanlogin($_POST['username']);
		if (isset($_POST['cookie']) and $_POST['cookie'] == 1) {                                    
			setcookie("username", $_SESSION['username'], time()+(3600*24*30));
			if (isset($_POST['password']) and strlen($_POST['password']) > 6) {
				setcookie("password", $_POST['password'], time()+(3600*24*30));
			}
		}
		fnAuthenticatie(1, $_POST['password'], 1);
	}
	echo("<script>location.href='/'; </script>\n");
} elseif ((!isset($_SESSION['lidid']) or $_SESSION['lidid'] == 0) and isset($_COOKIE['password']) and strlen($_COOKIE['password']) > 5) {
	fnAuthenticatie(0);
	if (isset($_GET['tp']) and strlen($_GET['tp']) > 0) {
		printf("<script>location.href='/?tp=%s'; </script>\n", $_GET['tp']);
	} else {
		echo("<script>location.href='/'; </script>\n");
	}
} elseif ($_SESSION['lidid'] > 0) {
	(new cls_Login())->setingelogd($_SESSION['lidid']);
}

if ((new cls_Lid())->aantal() == 0) {
	echo("<script>alert('Voordat deze website gebruikt kan worden moeten er eerst gegevens uit de Access-database ge-upload worden.');
		location.href='./admin.php?tp=Uploaden+data';</script>\n");
}

if ($currenttab != "Mailing") {
	HTMLheader();
}

$isafdelingstab = 0;
$f = sprintf("Type='A' AND Naam='%s'", $currenttab);
if (strlen($_SESSION['settings']['menu_met_afdelingen']) > 0 and (new cls_Onderdeel())->aantal($f) == 1) {
	$isafdelingstab = 1;
}

if (toegang($_GET['tp'], 1) == false) {
	if ($_SESSION['lidid'] == 0) {
		fnLoginAanvragen();
	}
	
} elseif ($currenttab == "Inloggen") {
	fnInloggen();
	
} elseif ($currenttab == "Login aanvragen") {
	fnLoginAanvragen();
	
} elseif ($currenttab == "Opvragen lidnr") {
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		fnOpvragenLidnr("mail");
	} else {
		fnOpvragenLidnr("form");
	}
	
} elseif ($currenttab == "Herstel wachtwoord") {
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		fnHerstellenWachtwoord("mail");
	} else {
		fnHerstellenWachtwoord("form");
	}
	
} elseif ($currenttab == "Validatie login") {
	if (isset($_GET['key']) and isset($_GET['lidid'])) {
		// Valideren van de nieuwe login op de website
		fnValidatieLogin($_GET['lidid'], $_GET['key'], "validatie");
	}
	printf("<p class='mededeling'><a href='%s'>Klik hier om verder te gaan.</p></p>\n", BASISURL);
	
} elseif ($currenttab == "Bevestiging login") {
		fnLoginAanvragen();

} elseif ($currenttab == "Eigen gegevens") {
	if ($_SESSION['lidid'] > 0) {
		fnEigenGegevens($_SESSION['lidid'], $currenttab2);
	} else {
		echo("<p class='mededeling'>Er is geen lid ingelogd.</p>\n");
	}
	
} elseif ($currenttab == "Zelfservice") {
	if ($currenttab2 == "Inschrijven bewaking") {
		inschrijvenbewaking($_SESSION['lidid']);
	} elseif ($currenttab2 == "Evenementen") {
		inschrijvenevenementen($_SESSION['lidid']);
	} elseif ($currenttab2 == "Bestellingen") {
		fnWinkelwagen($_SESSION['lidid']);
	} else {
		fnWijzigen($_SESSION['lidid'], $currenttab2);
	}
	
} elseif ($currenttab2 == "Overzicht lid" and toegang($currenttab, 0)) {
	if (isset($_GET['lidid']) and is_numeric($_GET['lidid']) and $_GET['lidid'] > 0) {
		fnEigenGegevens($_GET['lidid'], $currenttab3);
	} else {
		fnEigenGegevens(0, $currenttab3);
	}
	
} elseif ($currenttab2 == "Wijzigen lid") {
	fnWijzigen($_GET['lidid'], $currenttab3);
	
} elseif ($currenttab == "Vereniging") {
	$tabblad["Introductie"] = fnVoorblad();
	$tn = "Agenda";
	if (toegang($currenttab . "/" . $tn, 0, 0)) {
		$tabblad["Agenda"] = fnAgenda($_SESSION['lidid']);
	}
	
	if ((new cls_Onderdeel())->aantal("`Type`='A'") > 0) {
		$atn = array("Verenigingskader", "Afdelingskader");
	} else {
		$atn = array("Kader");
	}
	$atn[] = "Overig";
	$atn[] = "Onderscheidingen";
	foreach ($atn as $tn) {
		if (toegang($currenttab . "/" . $tn, 0, 0)) {
			$tabblad[$tn] = fnWieiswie($tn, $_SESSION['settings']['kaderoverzichtmetfoto']);
		}
	}
	DisplayTabs($tabblad);

} elseif ($currenttab == "Ledenlijst") {
	if ($currenttab2 == "Afdelingen") {
		fnOnderdelenmuteren("A");
		
	} elseif ($currenttab2 == "Commissies") {
		fnOnderdelenmuteren("C");
		
	} elseif ($currenttab2 == "Groepen") {
		fnOnderdelenmuteren("G");
		
	} elseif ($currenttab2 == "Rollen") {
		fnOnderdelenmuteren("R");
		
	} elseif ($currenttab2 == "Toestemmingen") {
		fnOnderdelenmuteren("T");
		
	} elseif ($currenttab2 == "Eigen lijsten") {
		fnDispMenu(2);
		fnDispMenu(3);
		if ($currenttab3 == "Muteren") {
			fnEigenlijstenmuteren();
		} else {
			$i_el = new cls_Eigen_lijst($currenttab3);
			$rows = $i_el->rowset();
			if ($rows !== false) {
				printf("<p>%s</p>", fnDisplayTable($rows, null, $currenttab3));
				printf("<p>%d rijen</p>\n", count($rows));
				$i_el->update($i_el->elid, "AantalRecords", count($rows));
			}
			$i_el = null;
		}
		
	} elseif ($currenttab2 == "Basisgegevens") {
		fnDispMenu(2);
		fnDispMenu(3);
		fnBasisgegevens($currenttab3);
		
	} else {
		fnLedenlijst();
	}
} elseif ($isafdelingstab == 1) {
	fnAfdeling();
} elseif ($currenttab == "Stukken" and toegang($currenttab, 1, 1)) {
	fnStukken();
} elseif ($currenttab == "Bewaking") {
	fnBewaking();
} elseif ($currenttab == "Rekeningen") {
	fnRekeningen();
} elseif ($currenttab == "Kostenoverzicht") {
	fnKostenoverzicht();
} elseif ($currenttab == "Mailing") {
	fnMailing();
} elseif ($currenttab == "Evenementen") {
	fnEvenementen();
} elseif ($currenttab == "Bestellingen") {
	fnWebshop();

} elseif (!isset($_SESSION['lidid']) or $_SESSION['lidid'] == 0) {
	if ($_SESSION['settings']['mailing_lidnr'] > 0) {
		fnOpvragenLidnr("form");
	}
	fnLoginAanvragen();
} else {
	debug("Geen voorblad");
}

if ($currenttab != "Mailing") {	
	HTMLfooter();
}

function fnVoorblad() {
	global $fileverinfo;

	if (file_exists($fileverinfo)) {
		$content = file_get_contents($fileverinfo);
	} else {
		$content = "";
	}
	
	if ($content !== false and strlen($content) > 0) {
		if ($_SESSION['lidid'] == 0) {
			$content = removetextblock($content, "<!-- Ingelogd -->", "<!-- /Ingelogd -->");
		}
		if ($_SESSION['webmaster'] == 0) {
			$content = removetextblock($content, "<!-- Webmaster -->", "<!-- /Webmaster -->");
		}
		
		$pos = 0;
		while ($pos < strlen($content)) {
			$p = strpos($content, "<!-- Ond_", $pos);
			if ($p > 0) {
				$ondid = intval(substr($content, $p+9, strpos($content, " ", $p+9)-($p+9)));
				if (in_array($ondid, explode(",", $_SESSION['lidgroepen'])) === false) {
					$b = sprintf("Ond_%d", $ondid);
					$content = removetextblock($content, sprintf("<!-- %s -->", $b), sprintf("<!-- /%s -->", $b));
				}
				$pos = $p;
			}
			$pos++;
		}
		
		(new cls_login())->uitloggen();
	
		// Algemene statistieken
		$stats = db_stats();
		foreach (array('aantalleden', 'aantalvrouwen', 'aantalmannen', 'gemiddeldeleeftijd', 'aantalkaderleden', 'nieuwstelogin', 'aantallogins', 'nuingelogd') as $v) {
			if (strpos($content, strtoupper($v)) !== false) {
				$content = str_replace("[%" . strtoupper($v) . "%]", htmlentities($stats[$v]), $content);
			}
		}
		
		if (strpos($content, "[%LAATSTGEWIJZIGD%]") !== false) {
			$content = str_replace("[%LAATSTGEWIJZIGD%]", strftime("%e %B %Y (%H:%M)", strtotime($stats['laatstgewijzigd'])), $content);
		}
		
		if (strpos($content, "[%LAATSTEUPLOAD%]") !== false) {
			$lu = (new cls_Logboek())->max("DatumTijd", "TypeActiviteit=9");
			$content = str_replace("[%LAATSTEUPLOAD%]", strftime("%e %B %Y", strtotime($lu)), $content);
		}
			
		if (strpos($content, "[%BEWAARTIJDLOGGING%]") !== false) {
			$lu = (new cls_Logboek())->max("DatumTijd", "TypeActiviteit=9");
			$content = str_replace("[%BEWAARTIJDLOGGING%]", $_SESSION['settings']['logboek_bewaartijd'], $content);
		}
		
		// Gebruiker-specifieke statistieken
		if ($_SESSION['lidid'] > 0) {
			$stats = db_stats($_SESSION['lidid']);
			$content = str_replace("[%NAAMLID%]", $_SESSION['naamingelogde'], $content);
			$content = str_replace("[%LIDNR%]", $_SESSION['lidnr'], $content);
			$content = str_replace("[%INGELOGDEGEWIJZIGD%]", strftime("%e %B %Y (%H:%m)", strtotime($stats['laatstgewijzigd'])), $content);
		} else {
			$content = str_replace("[%INGELOGDEGEWIJZIGD%]", "", $content);
		}
		if (strpos($content, "[%KOMENDEEVENEMENTEN%]") !== false) {
			$content = str_replace("[%KOMENDEEVENEMENTEN%]", fnAgendaItems(), $content);
		}
		$content = str_replace("[%ROEPNAAM%]", $_SESSION['roepnaamingelogde'], $content);
		if (strpos($content, "[%VERVALLENDIPLOMAS%]") !== false and $_SESSION['lidid'] > 0) {
			$strHV = "";
			$rows = (new cls_Liddipl())->vervallenbinnenkort();
			if (count($rows) > 0){
				$strHV = "<p>Je volgende diploma's zijn recent vervallen of komen binnenkort te vervallen.</p>\n<ul>";
				foreach ($rows as $row) {
					if ($row->VervaltPer <= date("Y-m-d")) {
						$strHV .= sprintf("<li>%s is per %s vervallen.</li>\n", $row->DiplOms, strftime("%e %h %Y", strtotime($row->VervaltPer)));
					} else {
						$strHV .= sprintf("<li>%s vervalt op %s.</li>\n", $row->DiplOms, strftime("%e %h %Y", strtotime($row->VervaltPer)));
					}
				}
				$strHV .= "</ul>\n";
			}
			$content = str_replace("[%VERVALLENDIPLOMAS%]", $strHV, $content);
		}
	
		if (strpos($content, "[%VORIGELOGIN%]") !== false) {
			$content = str_replace("[%VORIGELOGIN%]", (new cls_Logboek())->vorigelogin(1), $content);
		}
		if (strpos($content, "[%GEWIJZIGDESTUKKEN%]") !== false) {
			$content = str_replace("[%GEWIJZIGDESTUKKEN%]", fnGewijzigdeStukken(), $content);
		}
		
		if (strpos($content, "[%VERJAARFOTO%]") !== false) {
			$content = str_replace("[%VERJAARFOTO%]", overzichtverjaardagen(1), $content);
		}
		if (strpos($content, "[%VERJAARDAGEN%]") !== false) {
			$content = str_replace("[%VERJAARDAGEN%]", overzichtverjaardagen(0), $content);
		}
		
	} else {
		$content = "Er is geen introductie beschikbaar.";
	}
	
	
	return sprintf("<div id='welkomstekst'>\n%s</div>  <!-- Einde welkomstekst -->\n", $content);
	
}  # fnVoorblad

function fnAgenda($p_lidid=0) {
	
	$i_lid = new cls_Lid();
	
	if (strlen($_SESSION['settings']['agenda_url_feestdagen']) > 4) {
		$ics = file_get_contents($_SESSION['settings']['agenda_url_feestdagen']);
	} else {
		$ics = false;
	}
	
	if ($ics === FALSE) {
		$fds["99991231"] = "geen";
	} else {
		$icslines = explode("\n", $ics);
		foreach ($icslines as $line) {
			if (substr($line, 0, 7) == "DTSTART") {
				$d = substr(trim($line), -8);
			//} elseif (substr($line, 0, 7) == "SUMMARY" and intval($d) >= intval(date("Ymd", $Hulpdatum)) and intval($d) <= intval(date("Ymd", $Einddatum))) {
			} elseif (substr($line, 0, 7) == "SUMMARY") {
				$fds[$d] = substr($line, 8);
			}
		}
	}
	
	$dtStart = mktime(0, 0, 0, date("m"), 1, date("Y"));
	
	while (date("N", $dtStart) > 1) {
		$dtStart = strtotime("-1 day", $dtStart);
	}
	
	$txt = "<div id=agenda>\n";
//	$txt .= "<p class='mededeling'>De agenda is nog in ontwikkeling</p>\n";
	$txt .= "<table>\n<tr>\n";
	for ($dn=1;$dn<=7;$dn++) {
		$txt .= sprintf("<th>%s</th>", strftime("%A", strtotime(sprintf("+%d day", $dn-1), $dtStart)));
	}
	for ($sw=$dtStart;$sw <= strtotime("+370 day");$sw=strtotime("+7 day", $sw)) {
		$txt .= "<tr>\n";
		
		for ($dn=1;$dn<=7;$dn++) {
			
			$td = strtotime(sprintf("+%d day", $dn-1), $sw);
			$c = "";
			if (date("Ymd", $td) == date("Ymd")) {
				$c = " class='vandaag'";
			}
			if (array_key_exists(date("Ymd", $td), $fds)) {
				$txt .= sprintf("<td%s><ul><li>%s</li>", $c, $fds[date("Ymd", $td)]);
			} else {
				$txt .= sprintf("<td%s><ul><li>%s</li>", $c, strftime("%e %B", $td));
			}
			
			// Evenementen
			foreach ((new cls_Evenement())->lijst(5, date("Y-m-d", $td)) as $evrow) {
				$bt = "";
				if (substr($evrow->Datum, 11, 5) > "00:00") {
					$bt = substr($evrow->Datum, 11, 5) . "&nbsp;";
				}
				$txt .= sprintf("<li class='%s'>%s%s</li>\n", str_replace("'", "", str_replace(" ", "_", strtolower($evrow->OmsType))), $bt, $evrow->Omschrijving);
			}
			
			// Afdelingskalender
			foreach ((new cls_Afdelingskalender())->lijst(-1, date("Y-m-d", $td)) as $akrow) {
				if (strlen($akrow->Omschrijving) > 1) {
					if ($akrow->Activiteit == 1) {
						$oms = $akrow->Kode . ": " . $akrow->Omschrijving;
					} else {
						$oms = "Geen " . $akrow->Kode . " (" . $akrow->Omschrijving . ")";
					}
				} else {
					if (strlen($akrow->Activiteit) == 1) {
						$oms = $akrow->Naam;
					} else {
						$oms = "Geen " . $akrow->Naam;
					}
				}
				$txt .= sprintf("<li class='%s'>%s</li>\n", strtolower($akrow->Kode), $oms);
			}
			
			// Verjaardagen
			if ($_SESSION['settings']['agenda_verjaardagen'] > 0) {
				$aant = 0;
				$rows = $i_lid->verjaardagen($td);
				foreach ($rows as $row) {
					$aant++;
					if ($aant == 1) {
						$vj = $row->Naam_lid;
					} elseif ($aant == 2) {
						$vj = $row->Naam_lid . " en " . $vj;
					} else {
						$vj = $row->Naam_lid . ", " . $vj;
					}
				}
				if ($aant == 1) {
					$txt .= sprintf("<li class='jarigen'>%s is jarig</li>", $vj);
				} elseif ($aant > 1) {
					$txt .= sprintf("<li class='jarigen'>%s zijn jarig</li>", $vj);
				}
				$txt .= "</ul>";
			}
			$txt .= "</td>\n";
		}
		$txt .= "</tr>\n";
	}
	
	$txt .= "</tr>\n";
	$txt .= "</table>\n";
	$txt .= "</div> <!-- Einde agenda -->\n";
	
	return $txt;
	
}  # fnAgenda

function fnStukken() {
	
	$i_stuk = new cls_Stukken();
	if (isset($_GET['p_scherm']) and $_GET['p_scherm'] == "F") {
		$scherm = $_GET['p_scherm'];
	} else {
		$scherm = "";
	}
	if (isset($_POST['stid']) and $_POST['stid'] > 0) {
		$stid = $_POST['stid'];
	} elseif (isset($_GET['p_stid']) and $_GET['p_stid'] > 0) {
		$stid = $_GET['p_stid'];
	} else {
		$stid = 0;
	}
	
	if (isset($_GET['op']) and $_GET['op'] == "delete" and $stid > 0) {
		$i_stuk->delete($stid);
	}
	
	echo("<div id='stukkenmuteren'>\n");
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['Toevoegen'])) {
			$i_stuk->add();
		}
		
		if ($stid > 0) {
			$row = $i_stuk->record($stid);
			foreach ($row as $col => $val){
				if (isset($_POST[$col])) {
					$i_stuk->update($row->RecordID, $col, $_POST[$col]);
				}
			}
		}
	}
	
	if ($scherm == "F" and $stid > 0) {
		
		$row = $i_stuk->record($stid);
		
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		printf("<label>RecordID</label><p>%d</p>\n", $row->RecordID);
		printf("<input type='hidden' name='stid' value=%d>\n", $row->RecordID);
		printf("<label>Titel</label><input type='text' name='Titel' value=\"%s\">\n", str_replace("\"", "'", $row->Titel));
		printf("<label>Bestemd voor</label><input type='text' name='BestemdVoor' value=\"%s\">\n", str_replace("\"", "'", $row->BestemdVoor));
		printf("<label>Vastgesteld op</label><input type='date' name='VastgesteldOp' value='%s'>\n", $row->VastgesteldOp);
		printf("<label>Ingangsdatum</label><input type='date' name='Ingangsdatum' value='%s'>\n", $row->Ingangsdatum);
		printf("<label>Revisiedatum</label><input type='date' name='Revisiedatum' value='%s'>\n", $row->Revisiedatum);
		printf("<label>Vervallen per</label><input type='date' name='VervallenPer' value='%s'>\n", $row->VervallenPer);
			
		$options = "";
		foreach (ARRTYPESTUK as $k => $v) {
			$options .= sprintf("<option value='%s' %s>%s</option>\n", $k, checked($k, "option", $row->Type), $v);
		}
		printf("<label>Type</label><select name='Type'>%s</select>\n", $options);
		printf("<label>Link naar document</label><input type='url' name='Link' value='%1\$s'><p id='ganaarurl'><a href='%1\$s'>Ga naar</a></p>\n", $row->Link);
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<input type='submit' value='Bewaren'>\n");
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		
		echo("</form>");
		
	} else {
	
		$rows = $i_stuk->editlijst();
		$kols[0]['link'] = sprintf("%s?tp=%s&p_scherm=F&p_stid=%%d", BASISURL, $_GET['tp']);
		$kols[0]['columnname'] = "RecordID";
		$kols[1]['headertext'] = "Titel";
		$kols[2]['headertext'] = "Type";
		$kols[3]['headertext'] = "Bestemd voor";
		$kols[4]['headertext'] = "Vastgesteld op";
		$kols[4]['type'] = "dateshort";
		$kols[5]['headertext'] = "Revisiedatum";
		$kols[5]['type'] = "dateshort";
		$kols[6]['headertext'] = "Vervallen per";
		$kols[6]['type'] = "dateshort";
		
		$kols[7]['link'] = sprintf("%s?tp=%s&op=delete&p_stid=%%d", BASISURL, $_GET['tp']);
		$kols[7]['columnname'] = "RecordID";
		echo(fnDisplayTable($rows, $kols));
		
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		echo("<div id='opdrachtknoppen'>\n");
		echo("<input type='submit' name='Toevoegen' value='Stuk toevoegen'>\n");
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
		
	}
	$i_stuk = null;
	echo("</div> <!-- Einde stukkenmuteren -->\n");
}  # fnStukken

function fnGewijzigdeStukken() {

	$rv = "";
	if ($_SESSION['lidid'] > 0) {
		$rows = (new cls_Stukken())->gewijzigdestukken();
		if (count($rows) > 0) {
			$rv = "<p>De volgende stukken zijn gewijzigd sinds je laatste login of korter dan een week geleden.</p>\n
				   <ul>\n";
			foreach($rows as $row) {
				$rv .= sprintf("<li>%s</li>\n", $row->Titel);
			}
			$rv .= "</ul>\n";
		}
	}
	
	return $rv;

}  # fnGewijzigdeStukken

?>
