<?php

if (!isset($_GET['tp'])) {
	$_GET['tp'] = "";
}

require('./includes/standaard.inc');

if (isset($_GET['actie']) and $_GET['actie'] == "uitloggen") {
	(new cls_login())->uitloggen($_SESSION['lidid']);
	setcookie("username", "", time()-60);
	setcookie("password", "", time()-60);
	$_SESSION['lidid'] = 0;
	$_SESSION['webmaster'] = 0;
	// echo("<script>location.href='/'; </script>\n");
} else if ($_SERVER['REQUEST_METHOD'] == "POST" and isset($_POST['Inloggen']) and $_POST['Inloggen'] == "Inloggen") {
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
	if ($_SESSION['lidid'] > 0) {
		echo("<script>location.href='/'; </script>\n");
	}
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

if (isset($_GET['op']) and $_GET['op'] == "exportins") {
	header("Content-type: text/plain");
	header("Content-Disposition: attachment; filename=inschrijvingen.sql");
	foreach((new cls_InsBew())->export() as $row) {
		echo(SQLexport($row) . "\n");
	}
	exit();
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
	
} elseif ($currenttab == "Opvragen lidnr" and $_SERVER['REQUEST_METHOD'] == "POST") {
	fnOpvragenLidnr("mail");
	
} elseif ($currenttab == "Herstellen wachtwoord") {
	fnHerstellenWachtwoord("mail");
	
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
	fnDispMenu(2);
	if ($currenttab2 == "Introductie") {
		fnVoorblad();
		if ($_SESSION['lidid'] == 0) {
			fnLoginAanvragen();
			echo("<div id='kolomrechts'>\n");
			if ($_SESSION['settings']['mailing_lidnr'] > 0) {
				fnOpvragenLidnr("form");
			}
			fnHerstellenWachtwoord("form");
			echo("</div>  <!-- Einde kolomrechts -->\n");
		}
	} else {
		fnWieiswie($currenttab2, $_SESSION['settings']['kaderoverzichtmetfoto']);
	}
} elseif ($currenttab == "Ledenlijst") {
	if ($currenttab2 == "Commissies muteren") {
		fnOnderdelenmuteren("C");
	} elseif ($currenttab2 == "Groepen muteren") {
		fnOnderdelenmuteren("G");
	} elseif ($currenttab2 == "Rollen muteren") {
		fnOnderdelenmuteren("R");
	} else {
		fnLedenlijst();
	}
} elseif ($isafdelingstab == 1) {
	fnAfdeling();
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
} elseif ($currenttab == "Eigen lijsten") {
	
	fnDispMenu(2);
	if ($currenttab2 == "Muteren") {
		fnEigenlijstenmuteren();
	} else {
		$i_el = new cls_Eigen_lijst($currenttab2);
		$rows = (new cls_db_base())->execsql($i_el->mysql())->fetchAll();
		$i_el->update($i_el->elid, "AantalRecords", count($rows));
		printf("<p>%s</p>", fnDisplayTable($rows, "", $currenttab2, 0, "", "", "lijst", ""));
	}
	
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
			$content = str_replace("[%KOMENDEEVENEMENTEN%]", ToekomstigeEvenementen(), $content);
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

		printf("<div id='welkomstekst'>\n%s</div>  <!-- Einde welkomstekst -->\n", $content);
		
	} else {
		echo("<div id='welkomstekst'>Er is geen introductie beschikbaar.</div>  <!-- Einde welkomstekst -->\n");
	}
}  # fnVoorblad

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
