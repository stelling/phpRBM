<?php

$_GET['tp'] = $_GET['tp'] ?? "";
$op = $_GET['op'] ?? "";

require("./includes/standaard.inc");

if (isset($_GET['actie']) and $_GET['actie'] == "uitloggen") {
	(new cls_login())->uitloggen($_SESSION['lidid']);
	setcookie("username", "", time()-60);
	setcookie("password", "", time()-60);
	$_SESSION['lidid'] = 0;
	$_SESSION['webmaster'] = 0;
	$_SESSION['lidgroepen'] = null;
	$_SESSION['lidauth'] = null;
	printf("<script>setTimeout('', 5000);location.href='%s';</script>\n", BASISURL);
	
} elseif ($_SERVER['REQUEST_METHOD'] == "POST" and isset($_POST['Inloggen'])) {
	if (strlen($_POST['password']) < 5) {
		$mess = "Om in te loggen is het invullen van een wachtwoord van minimaal 5 karakters vereist.";
		(new cls_Logboek())->add($mess, 1, 0, 2);
	} else {
		$_SESSION['username'] = cleanlogin($_POST['username']);
		if (isset($_POST['cookie']) and $_POST['cookie'] == 1) {                           
			setcookie("username", $_SESSION['username'], time()+(3600*24*120));
			if (isset($_POST['password']) and strlen($_POST['password']) > 6) {
				setcookie("password", $_POST['password'], time()+(3600*24*120));
			}
		}
		fnAuthenticatie(1, $_POST['password'], 1);
	}
	printf("<script>location.href='%s';</script>\n", BASISURL);
	
} elseif ((!isset($_SESSION['lidid']) or $_SESSION['lidid'] == 0) and isset($_COOKIE['password']) and strlen($_COOKIE['password']) > 5) {
	fnAuthenticatie(0);
	if (isset($_GET['tp']) and strlen($_GET['tp']) > 0) {
		printf("<script>location.href='%s?tp=%s';</script>\n", BASISURL, $_GET['tp']);
	} else {
		printf("<script>location.href='%s';</script>\n", BASISURL);
	}
} elseif ($_SESSION['lidid'] > 0) {
	(new cls_Login())->setingelogd($_SESSION['lidid']);
}

if ($currenttab2 != "previewwindow" and $op != "preview_hist" and $op != "preview_rek" and $currenttab2 != "DL-lijst") {
	HTMLheader();
	$kaal = 0;
} else {
	$kaal = 1;
}

$isafdelingstab = 0;
$f = sprintf("Type='A' AND Naam='%s'", $currenttab);
if (strlen($_SESSION['settings']['menu_met_afdelingen']) > 0 and (new cls_Onderdeel())->aantal($f) == 1) {
	$isafdelingstab = 1;
}

$eigenlijstid = 0;
$f = "";
if (strlen($currenttab2) > 0 and strlen($currenttab3) > 0) {
	$f = sprintf("EL.Tabpage='%s/%s' AND EL.Naam='%s'", $currenttab, $currenttab2, $currenttab3);
} elseif (strlen($currenttab2) > 0) {
	$f = sprintf("EL.Tabpage='%s' AND EL.Naam='%s'", $currenttab, $currenttab2);
}
if (strlen($f) > 0) {
	$f .= " AND LENGTH(EL.Tabpage) > 0";
	$eigenlijstid = (new cls_Eigen_lijst())->recordid($f);
}

$i_lid = new cls_Lid();

if ($i_lid->aantal() == 0) {
	$lidid = $i_lid->add("Webmaster");
	if ($lidid > 0) {
		$ww = "Webm" . rand(10000, 99999);
		$i_login = new cls_login();
		$id = $i_login->add($lidid, "webmaster", $ww);
		$query = sprintf("UPDATE %sAdmin_login SET Gewijzigd=SYSDATE(), ActivatieKey='' WHERE LidID=%d;", TABLE_PREFIX, $lidid);
		$i_login->execsql($query);
		$i_login = null;
	}
	printf("<p class='mededeling'>Er zitten nog geen leden in de database.</p>\n");
	printf("<p class='mededeling'>Om te starten is er één lid aangemaakt met LidID %d. Vul dit LidID in bij de 'lididwebmasters' in config.php.</p>\n", $lidid);
	printf("<p class='mededeling'>Er is ook een login aangemaakt, te weten 'webmaster' met '%s' als wachtwoord.</p>\n", $ww);
	echo("<p class='mededeling'>Om te starten log hiermee in, dit lid en login kunnen later worden verwijderd.</p>\n");
} elseif (toegang($_GET['tp'], 1) == false) {
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
	if ($currenttab2 == "Evenementen") {
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
	$tabidx = 0;
	$tabblad["Introductie"] = fnVoorblad();
	$tn = "Agenda";
	if ($tn == $currenttab2) {
		$tabidx = count($tabblad);
	}	
	if (toegang($currenttab . "/" . $tn, 0, 0)) {
		$tabblad["Agenda"] = fnAgendaTable($_SESSION['lidid']);
	}
	
	if ((new cls_Onderdeel())->aantal("`Type`='A'") > 0) {
		$atn = array("Verenigingskader", "Afdelingskader");
	} else {
		$atn = array("Kader");
	}
	
	if ((new cls_Onderdeel())->aantal("((O.TYPE='C' AND O.Kader=0) OR O.TYPE='F')") > 0) {
		$atn[] = "Overig";
	}
	if ((new cls_Onderdeel())->aantal("O.`Type`='O'") > 0) {
		$atn[] = "Onderscheidingen";
	}
	foreach ($atn as $tn) {
		if (toegang($currenttab . "/" . $tn, 0, 0)) {
			$tabblad[$tn] = fnWieiswie($tn, $_SESSION['settings']['kaderoverzichtmetfoto']);
		}
	}
	
	$tn = "Stukken";
	$i_stuk = new cls_Stukken();
	if ($i_stuk->aantal() > 0 and toegang($currenttab . "/" . $tn, 0, 0)) {
		$tabblad[$tn] = fnStukken("O");
	}
	
	DisplayTabs($tabblad, $tabidx);
	
} elseif ($currenttab == "Agenda") {
	echo(fnAgenda());
		
} elseif ($eigenlijstid > 0) {
	fnDispMenu(2);
	fnDispMenu(3);
	if ($eigenlijstid > 0) {
		$i_el = new cls_Eigen_lijst("", $eigenlijstid);
	} else {
		$i_el = new cls_Eigen_lijst($currenttab3);
	}
	if (strlen($i_el->mysql) >= 9) {
		$rows = $i_el->rowset();
		if ($rows !== false) {
			$id = str_replace(" ", "_", strtolower($i_el->elnaam));
			echo(fnDisplayTable($rows, null, $i_el->elnaam, 0, $i_el->uitleg, $id));
		}
	} elseif (strlen($i_el->eigenscript) >= 5) {
		$s = BASEDIR . "/maatwerk/" . $i_el->eigenscript;
		if (file_exists($s)) {
			$url = BASISURL . "/maatwerk/" . $i_el->eigenscript;
			printf("<script>location.href='%s';</script>\n", $url);
		} else {
			debug($s . " bestaat niet, vraag de webmaster om dit te verhelpen.");
		}
	}
	$i_el->controle($i_el->elid);
	$i_el = null;

} elseif ($currenttab == "Ledenlijst") {
	
	if ($currenttab2 == "Afdelingen") {
		fnOnderdelenmuteren("A");
		
	} elseif ($currenttab2 == "Commissies") {
		fnOnderdelenmuteren("C");
		
	} elseif ($currenttab2 == "Groepen") {
		fnOnderdelenmuteren("G");
		
	} elseif ($currenttab2 == "Rollen") {
		fnOnderdelenmuteren("R");
		
	} elseif ($currenttab2 == "Selecties") {
		fnOnderdelenmuteren("S");
		
	} elseif ($currenttab2 == "Rapporten") {
		fnDispMenu(2);
		fnDispMenu(3);
		if ($currenttab3 == "Jubilarissen") {
			Jubilarissen();
		}
		if ($currenttab3 == "Presentielijst") {
			presentielijst();
		}
		if ($currenttab3 == "Toestemmingen") {
			overzichttoestemmingen();
		}
		if ($currenttab3 == "NCS opgave") {
			ncsopgave();
		}
		
	} elseif ($currenttab2 == "Toestemmingen") {
		fnOnderdelenmuteren("T");
		
	} elseif ($currenttab2 == "Sportlink") {
		sportlink();
		
	} elseif ($currenttab2 == "Basisgegevens" and isset($_GET['Scherm']) and $_GET['Scherm'] == "W" and isset($_GET['OnderdeelID'])) {
		fnDispMenu(2);
		fnDispMenu(3);
		DetailsOnderdeelMuteren($_GET['OnderdeelID']);
		
	} elseif ($currenttab2 == "Basisgegevens") {
		fnDispMenu(2);
		fnDispMenu(3);
		fnBasisgegevens($currenttab3);
		
	} else {
		fnLedenlijst();
	}
} elseif ($isafdelingstab == 1) {
	fnAfdeling();

} elseif ($currenttab == "Website" and toegang($currenttab, 1, 1)) {
	if ($currenttab2 == "Stukken" and toegang($currenttab2, 1, 1)) {
		fnStukken();
		
	} elseif ($currenttab2 == "Menu" and toegang($currenttab2, 1, 1)) {
		fnWebsiteMenu();
		
	} elseif ($currenttab2 == "Inhoud" and toegang($currenttab2, 1, 1)) {
		fnWebsiteInhoud();
		
	} elseif ($currenttab2 == "Logboek" and toegang($currenttab2, 1, 1)) {
		fnDispMenu(2);
		
		$i_lb = new cls_Logboek();
		$f = "A.TypeActiviteit IN (22, 26)";
		$rows = $i_lb->lijst(-1, 1, -1, $f);
		$kols = fnStandaardKols("logboek", 1, $rows);
		echo(fnDisplayTable($rows, $kols));
		$i_lb = null;
	}
	
} elseif ($currenttab == "Rekeningen") {
	fnRekeningen();
} elseif ($currenttab == "Mailing") {
	fnMailing();
} elseif ($currenttab == "Diplomazaken") {
	fnDiplomazaken();
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

if ($currenttab != "Mailing" and $kaal == 0) {	
	HTMLfooter();
}

function fnVoorblad() {
	global $dtfmt;
	
	$i_tp = new cls_Template();
	
	(new cls_login())->uitloggen();

	$i_tp->vulvars(-1, "verenigingsinfo");
	$content = $i_tp->inhoud;
	
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
	
		// Algemene statistieken
		$i_lid = new cls_Lid();
		$i_login = new cls_Login();
		$i_lb = new cls_Logboek();
		
		$content = str_ireplace("[%AANTALLEDEN%]", $i_lid->aantallid("L", "*") , $content);
		$content = str_ireplace("[%AANTALMANNEN%]", $i_lid->aantallid("L", "M") , $content);
		$content = str_ireplace("[%AANTALVROUWEN%]", $i_lid->aantallid("L", "V") , $content);
		$content = str_ireplace("[%AANTALKADERLEDEN%]", $i_lid->aantallid("M") , $content);
		$content = str_ireplace("[%AANTALLOGINS%]", $i_login->aantal() , $content);
		$contect = str_ireplace("[%GEMIDDELDELEEFTIJD%]", $i_lid->gemiddeldeleeftijd(), $content);
		$content = str_ireplace("[%NUINGELOGD%]", $i_login->nuingelogd(), $content);
		
		if (strpos($content, "[%LAATSTEUPLOAD%]") !== false) {
			$lu = $i_lb->max("DatumTijd", "A.TypeActiviteit=9");
			$dtfmt->setPattern(DTTEXT);
			$content = str_replace("[%LAATSTEUPLOAD%]", $dtfmt->format(strtotime($lu)), $content);
		}
		$content = str_replace("[%BEWAARTIJDLOGGING%]", $_SESSION['settings']['logboek_bewaartijd'], $content);

		// Gebruiker-specifieke statistieken
		
		if (strpos($content, "[%LAATSTGEWIJZIGD%]") !== false) {
			$dtfmt->setPattern(DTLONG);
			if ($_SESSION['lidid'] > 0) {
				$f = sprintf("A.TypeActiviteit=6 AND A
				.LidID=%d", $_SESSION['lidid']);
				$lgw = $i_lb->max("DatumTijd", $f);
				if (strlen($lgw) >= 10) {
					$lgw = $dtfmt->format(strtotime($lgw));
				}
			} else {
				$lgw = "";
			}
			$content = str_replace("[%LAATSTGEWIJZIGD%]", $lgw, $content);
		}
		
		if ($_SESSION['lidid'] > 0) {
			$dtfmt->setPattern(DTLONG);
			$content = str_replace("[%NAAMLID%]", $_SESSION['naamingelogde'], $content);
			$content = str_replace("[%LIDNR%]", $_SESSION['lidnr'], $content);
		} else {
			$content = str_replace("[%INGELOGDEGEWIJZIGD%]", "", $content);
		}
		if (strpos($content, "[%KOMENDEEVENEMENTEN%]") !== false) {
			$content = str_replace("[%KOMENDEEVENEMENTEN%]", fnPersoonlijkeAgenda(), $content);
		}
		$content = str_replace("[%ROEPNAAM%]", $_SESSION['roepnaamingelogde'], $content);
		if (strpos($content, "[%VERVALLENDIPLOMAS%]") !== false and $_SESSION['lidid'] > 0) {
			$strHV = "";
			$rows = (new cls_Liddipl())->vervallenbinnenkort();
			if (count($rows) > 0){
				$strHV = "<p>Je volgende diploma's zijn recent vervallen of komen binnenkort te vervallen.</p>\n<ul>";
				$dtfmt->setPattern(DTTEXT);
				foreach ($rows as $row) {
					if ($row->VervaltPer <= date("Y-m-d")) {
						$strHV .= sprintf("<li>%s is per %s vervallen.</li>\n", $row->DiplOms, $dtfmt->format(strtotime($row->VervaltPer)));
					} else {
						$strHV .= sprintf("<li>%s vervalt op %s.</li>\n", $row->DiplOms, $dtfmt->format(strtotime($row->VervaltPer)));
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
		$content = "<p class='mededeling'>Er is geen introductie beschikbaar.</p>\n";
	}
	
	return sprintf("<div id='welkomsttekst'>\n%s</div>  <!-- Einde welkomsttekst -->\n", $content);
	
}  # fnVoorblad

function fnAgenda($p_lidid=0) {
	global $dtfmt;
	
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
			} elseif (substr($line, 0, 7) == "SUMMARY") {
				$fds[$d] = substr($line, 8);
			}
		}
	}
	
	$dtStart = strtotime("-1 day");
	while (date("N", $dtStart) > 1) {
		$dtStart = strtotime("-1 day", $dtStart);
	}
	
	//	$txt .= "<p class='mededeling'>De agenda is nog in ontwikkeling</p>\n";
	$txt = "<div id='agenda'>\n";
	
	$dtfmt->setPattern("EEEE");
	$txt .= "<div class='row'>\n";
	for ($dn=1;$dn<=7;$dn++) {
		$txt .= sprintf("<div class='col dagnaam'>%s</div>", substr($dtfmt->format(strtotime(sprintf("+%d day", $dn-1), $dtStart)), 0, 3));
	}
	$txt .= "</div> <!-- Einde row -->\n";
	
	for ($sw=$dtStart;$sw <= strtotime("+240 day");$sw=strtotime("+7 day", $sw)) {
		$txt .= "<div class='row'>\n";
		
		for ($dn=1;$dn<=7;$dn++) {
			
			$td = strtotime(sprintf("+%d day", $dn-1), $sw);
			$txt .= "<div class='col'>";
			if (date("Ymd", $td) == date("Ymd")) {
				$txt .= "<ul class='active'>";
			} else {
				$txt .= "<ul>";
			}
			
			if (array_key_exists(date("Ymd", $td), $fds)) {
				$dtfmt->setPattern("d MMM");
				$txt .= sprintf("<li title=\"%s\">%s</li>", $fds[date("Ymd", $td)], substr(str_replace(" ", "&nbsp;", $dtfmt->format($td)), 0, -1));
			} else {
				$dtfmt->setPattern(DTTEXTWD);
				$t = sprintf("title='%s'", $dtfmt->format($td), $td);
				$dtfmt->setPattern("d MMM");
				$txt .= sprintf("<li %s>%s</li>", $t, substr(str_replace(" ", "&nbsp;", $dtfmt->format($td)), 0, -1));
			}
			
			$ikal = null;
			// Evenementen
			foreach ((new cls_Evenement())->lijst(5, date("Y-m-d", $td)) as $evrow) {
				$ikal[strtotime($evrow->Datum)] = fnEvenementOmschrijving($evrow, 1, "li");
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
				$ikal[strtotime($akrow->Datum . " " . $akrow->Begintijd)] = sprintf("<li class='%1\$s' title=\"%2\$s\">%2\$s</li>", strtolower($akrow->Kode), $oms);
			}
			
			if (isset($ikal)) {
				ksort($ikal);
				foreach ($ikal as $k => $v) {
					$txt .= $v . "\n";
				}
			}
			
			// Verjaardagen
			if ($_SESSION['settings']['agenda_verjaardagen'] > 0 and $_SESSION['lidid'] > 0) {
				$aant = 0;
				$rows = $i_lid->verjaardagen($td);
				$vj = "";
				foreach ($rows as $row) {
					$aant++;
					if ($aant == 1) {
						$vj = $row->NaamLid;
					} elseif ($aant == 2) {
						$vj = $row->NaamLid . " en " . $vj;
					} else {
						$vj = $row->NaamLid . ", " . $vj;
					}
				}
				
				if ($aant == 1) {
					$vj .= " is jarig";
				} elseif ($aant > 1) {
					$vj .= " zijn jarig";
				}
				$txt .= sprintf("<li class='jarigen' title=\"%1\$s\">%1\$s</li>", $vj);
			}
			$txt .= "</ul>";
			$txt .= "</div> <!-- Einde col  -->\n";
		}
		$txt .= "</div> <!-- Einde row -->\n";
	}
	
	$txt .= "</div> <!-- Einde agenda -->\n";
	
	return $txt;
	
}  # fnAgenda

function fnAgendaTable($p_lidid=0) {
	global $dtfmt;
	
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
			} elseif (substr($line, 0, 7) == "SUMMARY") {
				$fds[$d] = substr($line, 8);
			}
		}
	}
	
	$dtStart = strtotime("-1 week");
	while (date("N", $dtStart) > 1) {
		$dtStart = strtotime("-1 day", $dtStart);
	}
	
	//	$txt .= "<p class='mededeling'>De agenda is nog in ontwikkeling</p>\n";
	$txt = sprintf("<table class='%s'>\n", TABLECLASSES);
	$txt .= "<tr>\n";
	$dtfmt->setPattern("EEEE");
	for ($dn=1;$dn<=7;$dn++) {
		$txt .= sprintf("<th>%s</th>", substr($dtfmt->format(strtotime(sprintf("+%d day", $dn-1), $dtStart)), 0, 3));
	}
	for ($sw=$dtStart;$sw <= strtotime("+370 day");$sw=strtotime("+7 day", $sw)) {
		$txt .= "<tr>\n";
		
		for ($dn=1;$dn<=7;$dn++) {
			
			$td = strtotime(sprintf("+%d day", $dn-1), $sw);
			$c = "";
			if (date("Ymd", $td) == date("Ymd")) {
				$c = " class='table-active'";
			}
			if (array_key_exists(date("Ymd", $td), $fds)) {
				$txt .= sprintf("<td%s><ul><li title=\"%s\">%d</li>", $c, $fds[date("Ymd", $td)], date("d", $td));
			} else {
				$dtfmt->setPattern(DTDAYMONTH);
				$txt .= sprintf("<td%s><ul><li title=\"%s\">%d</li>", $c, $dtfmt->format($td), date("d", $td));
			}
			
			$ikal = null;
			// Evenementen
			foreach ((new cls_Evenement())->lijst(5, date("Y-m-d", $td)) as $evrow) {
				$ikal[strtotime($evrow->Datum)] = fnEvenementOmschrijving($evrow, 1, "li");
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
				$ikal[strtotime($akrow->Datum . " " . $akrow->Begintijd)] = sprintf("<li class='%1\$s' title=\"%2\$s\">%2\$s</li>", strtolower($akrow->Kode), $oms);
			}
			
			if (isset($ikal)) {
				ksort($ikal);
				foreach ($ikal as $k => $v) {
					$txt .= $v . "\n";
				}
			}
			
			// Verjaardagen
			if ($_SESSION['settings']['agenda_verjaardagen'] > 0 and $_SESSION['lidid'] > 0) {
				$aant = 0;
				$rows = $i_lid->verjaardagen($td);
				$vj = "";
				foreach ($rows as $row) {
					$aant++;
					if ($aant == 1) {
						$vj = $row->NaamLid;
					} elseif ($aant == 2) {
						$vj = $row->NaamLid . " en " . $vj;
					} else {
						$vj = $row->NaamLid . ", " . $vj;
					}
				}
				
				if ($aant == 1) {
					$vj .= " is jarig";
				} elseif ($aant > 1) {
					$vj .= " zijn jarig";
				}
				$txt .= sprintf("<li class='jarigen' title=\"%1\$s\">%1\$s</li>", $vj);
			}
			$txt .= "</ul>";
			$txt .= "</td>\n";
		}
		$txt .= "</tr>\n";
	}
	
	$txt .= "</table>\n";
	
	return $txt;
	
}  # fnAgendaTable

?>
