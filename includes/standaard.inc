<?php

date_default_timezone_set('Europe/Amsterdam');
setlocale(LC_ALL,'nl_NL.utf8');

if (session_start() === FALSE or strlen(session_id()) == 0) {
	$mess = "De sessie kan niet gestart worden, hierdoor kan er op dit moment niet ingelogd worden.";
	db_logboek("add", $mess, 1, 0, 1);
}

if (!isset($_SESSION['webmaster'])) {
	$_SESSION['webmaster'] = 0;
}
if ($_SESSION['webmaster'] == 1 or $_SERVER["HTTP_HOST"] == "phprbm.telling.nl") {
	error_reporting(E_ALL);
} else {
	error_reporting(0);
}

if (isset($_SERVER['HTTPS'])){
	$basisurl = "https://" . $_SERVER['HTTP_HOST'];
} else {
	$basisurl = "http://" . $_SERVER['HTTP_HOST'];
}

if (!isset($_SESSION['lidid'])) {
	$_SESSION['lidid'] = 0;
}

$dir = substr(__DIR__, 0, strrpos(__DIR__, "/"));
require($dir . '/config.php');
$bestandsversie = filectime($dir . "/index.php");

/* Include Open source libaries */
require_once("phpmailer/PHPMailer.php");
require_once("phpmailer/SMTP.php");
require_once("phpmailer/Exception.php");
require_once("simpleimage.php");
/* End open source libs*/

$includes = array("database", "authentication", "leden", "bewaking", "evenement", "mailing", "standaard", "webshop", "rbm_mailer");
foreach ($includes as $incl) {
	$incl .= ".inc";
	if ($bestandsversie < filectime(__DIR__ . "/" . $incl)) {
		$bestandsversie = filectime(__DIR__ . "/" . $incl);
	}
	require_once($incl);
}

if (!isset($_SESSION['settings']['naamvereniging'])) {
	db_param("", "curval");
}
if ($bestandsversie != db_param("versie")) {
	db_createtables();
	db_onderhoud();
	db_param("versie", "updval", $bestandsversie);
}

if (strlen(db_param("zs_muteerbarememos")) > 0) {
	$muteerbarememos = explode(",", db_param("zs_muteerbarememos"));
} else {
	$muteerbarememos = array();
}

if (isset($_GET['tp']) and strlen($_GET['tp']) > 1) {
	$sp = strpos($_GET['tp'], "/");
	if ($sp > 0) {
		$currenttab = substr($_GET['tp'], 0, $sp);
		$currenttab2 = substr($_GET['tp'], $sp+1);
	} else {
		$currenttab = $_GET['tp'];
		$currenttab2 = "";
	}
} else {
	$currenttab = $tabpages[0];
	$_GET['tp'] = $tabpages[0];
}

$fileverinfo = 'templates/verenigingsinfo.html';

$arrGeslacht["V"] = "Vrouwelijk";
$arrGeslacht["M"] = "Mannelijk";
$arrGeslacht["B"] = "Bedrijf";
$arrGeslacht["O"] = "Onbekend";

$arrTL = array("Alle", "Leden", "Klosleden", "Voormalig leden");

$arrLegitimatie["G"] = "Geen";
$arrLegitimatie["I"] = "Identiteitskaart";
$arrLegitimatie["P"] = "Paspoort";
$arrLegitimatie["R"] = "Rijbewijs";
	
$arrSoortMemo["A"] = "Algemeen";
$arrSoortMemo["B"] = "Bewaking";
$arrSoortMemo["D"] = "Dieet";
$arrSoortMemo["E"] = "Examen";
$arrSoortMemo["F"] = "Financiën";
$arrSoortMemo["G"] = "Gezondheid/medisch";
$arrSoortMemo["I"] = "Inschrijving bewaking";
$arrSoortMemo["WN"] = "Waarschuwen in geval van nood";

$pasfotoextenties = array("gif", "jpg", "jpeg", "png");

if ($_SERVER['PHP_SELF'] == "/admin.php") {
	$tabpages[] = "Beheer logins";
	$tabpages[] = "Autorisatie";
	$tabpages[] = "Instellingen";
	$tabpages[] = "Uploaden data";
	if (db_interface("aantalopenstaand") > 0) {
		$tabpages[] = "Downloaden wijzigingen";
	}
	$tabpages[] = "Onderhoud";
	$tabpages[] = "Logboek";
} else {
	$tabpages[] = "Vereniging";
	if (file_exists($fileverinfo)) {
		$tabpages[] = "Vereniging/Introductie";
	}
	if ($_SESSION['lidid'] == 0) {
		$tabpages[] = "Herstellen wachtwoord";
	}
	$tabpages[] = "Vereniging/Kader";
	$tabpages[] = "Vereniging/Onderscheidingen";
	$tabpages[] = "Vereniging/Overige commissies";
	if ($_SESSION['lidid'] > 0) {
		$tabpages[] = "Eigen gegevens";
		$tabpages[] = "Eigen gegevens/Algemeen";
		if (count(db_gegevenslid($_SESSION['lidid'], "Afdelingen")) > 0) {
			$tabpages[] = "Eigen gegevens/Afdelingen";
		}
		if (count(db_gegevenslid($_SESSION['lidid'], "Kader")) > 0) {
			$tabpages[] = "Eigen gegevens/Kader";
		}
		if (count(db_gegevenslid($_SESSION['lidid'], "Rollen")) > 0) {
			$tabpages[] = "Eigen gegevens/Rollen";
		}
		if (count(db_gegevenslid($_SESSION['lidid'], "Groepen")) > 0) {
			$tabpages[] = "Eigen gegevens/Groepen";
		}
		if (count(db_gegevenslid($_SESSION['lidid'], "Bew")) > 0 or count(db_insbew("overzichtlid", $_SESSION['lidid'])) > 0) {
			$tabpages[] = "Eigen gegevens/Bewaking";
		}
		$tabpages[] = "Eigen gegevens/Diploma's";
		$tabpages[] = "Eigen gegevens/Financieel";
		$tabpages[] = "Eigen gegevens/Mailing";
		if (count(db_gegevenslid($_SESSION['lidid'], "Evenementen")) > 0) {
			$tabpages[] = "Eigen gegevens/Evenementen";
		}
		$tabpages[] = "Eigen gegevens/Logboek";
		$tabpages[] = "Zelfservice";
		$tabpages[] = "Zelfservice/Algemene gegevens";
		if (count(db_diploma("zelfservice_lijst")) > 0) {
			$tabpages[] = "Zelfservice/Diploma's";
		}
		$tabpages[] = "Zelfservice/Pasfoto";
		if (count($muteerbarememos) > 0) { 
			$tabpages[] = "Zelfservice/Bijzonderheden";
		}
		if (count(db_bewaking_blok("openblokken")) > 0) {
			$tabpages[] = "Zelfservice/Inschrijving bewaking";
		}
		if (count(db_evenement("open")) > 0) {
			$tabpages[] = "Zelfservice/Inschrijving evenementen";
		}
		if (count(db_artikel("artikelbeschikbaar")) > 0) {
			$tabpages[] = "Zelfservice/Bestellingen";
		}
		$query = sprintf("SELECT COUNT(*) FROM %sLidmaatschap WHERE Lid=%d AND (Opgezegd IS NULL);", $table_prefix, $_SESSION['lidid']);
		if (db_scalar($query) > 0) {
			$tabpages[] = "Zelfservice/Opzegging";
		}
	}
	$tabpages[] = "Zelfservice/Wijzigen wachtwoord";
	$tabpages[] = "Ledenlijst";
	if ($currenttab == "Overzicht lid") {
		$tabpages[] = "Overzicht lid";
		$tabpages[] = "Overzicht lid/Algemeen";
		if ($_SESSION['aantalafdelingen'] > 0) {
			$tabpages[] = "Overzicht lid/Afdelingen";
		}
		$tabpages[] = "Overzicht lid/Kader";
		if ($_SESSION['aantalrollen'] > 0) {
			$tabpages[] = "Overzicht lid/Rollen";
		}
		$tabpages[] = "Overzicht lid/Groepen";
		if ($_SESSION['aantalbewaking'] > 0) {
			$tabpages[] = "Overzicht lid/Bewaking";
		}
		$tabpages[] = "Overzicht lid/Diploma's";
		$tabpages[] = "Overzicht lid/Financieel";
		$tabpages[] = "Overzicht lid/Mailing";
		$tabpages[] = "Overzicht lid/Evenementen";
		$tabpages[] = "Overzicht lid/Logboek";
	}
	if ($currenttab == "Wijzigen lid") {
		$tabpages[] = "Wijzigen lid";
		$tabpages[] = "Wijzigen lid/Algemene gegevens";
		$tabpages[] = "Wijzigen lid/Diploma's";
		$tabpages[] = "Wijzigen lid/Pasfoto";
		$tabpages[] = "Wijzigen lid/Bijzonderheden";
		$tabpages[] = "Wijzigen lid/Lidmaatschap";
	}
	if ($_SESSION['aantalmutaties'] > 0) {
		$tabpages[] = "Kostenoverzicht";
	}
	$tabpages[] = "Mailing";
	$tabpages[] = "Mailing/Templates";
	$tabpages[] = "Mailing/Concepten";
	$tabpages[] = "Mailing/Nieuw";
	if ($_SESSION['aantalrekreg'] > 0) {
		$tabpages[] = "Mailing/Rekeningen";
	}
	$tabpages[] = "Mailing/Verzonden";
	if (db_mailing("aantalprullenbak") > 0) {
		$tabpages[] = "Mailing/Prullenbak";
	}
	if (db_logboek("aantal", "", 4) > 0) {
		$tabpages[] = "Mailing/Logboek";
	}
	
	if ($_SESSION['aantalbewaking'] > 0) {
		$tabpages[] = "Bewaking";
		$tabpages[] = "Bewaking/Bewakingsrooster";
		$tabpages[] = "Bewaking/Postindeling";
		$tabpages[] = "Bewaking/Aantallen";
		$tabpages[] = "Bewaking/Nieuwe pasfoto";
		$tabpages[] = "Bewaking/Diploma's muteren";
		if (count(db_bewaking_blok("overzicht")) > 0) {
			$tabpages[] = "Bewaking/Overzicht inschrijvingen";
		}
		$tabpages[] = "Bewaking/Blokken muteren";
	}
	if (count(db_insbew("export")) > 0) {
		$tabpages[] = "Bewaking/Downloaden inschrijvingen";
	}
	if (db_logboek("aantal", "", 11) > 0) {
		$tabpages[] = "Bewaking/Logboek";
	}
	
	if (count(db_evenement("overzicht")) > 0 or toegang("Evenementen/Beheer", 0)) {
		$tabpages[] = "Evenementen";
		if (count(db_evenement("overzicht")) > 0) {
			$tabpages[] = "Evenementen/Overzicht";
		}
		$tabpages[] = "Evenementen/Beheer";
		$tabpages[] = "Evenementen/Types muteren";
	}
	if (db_logboek("aantal", "", 7) > 0) {
		$tabpages[] = "Evenementen/Logboek";
	}
	$tabpages[] = "Bestellingen";
	if (count(db_artikel("overzichtbeheer")) > 0) {
		$tabpages[] = "Bestellingen/Bestellingen muteren";
	}
	$tabpages[] = "Bestellingen/Artikelbeheer";
	if (db_logboek("aantal", "", 10) > 0) {
		$tabpages[] = "Bestellingen/Logboek";
	}
	
	if ($_SERVER["HTTP_HOST"] == "phprbm.telling.nl") {
//		$tabpages[] = "DMS";   Voorlopig geen tijd voor, dus uitgezet.
	}
}

if (isset($_GET['tp']) and strlen($_GET['tp']) > 1) {
	$curtab = explode("/", $_GET['tp']);
	for ($i=0; $i < 2; $i++) {
		if ($i == 0) {
			$ct = $curtab[$i];
		} else {
			$ct = $_GET['tp'];
		}
		$ia = false;
		foreach ($tabpages as $val) {
			if (str_replace("'", "", $val) === $ct) {
				$ia = true;
			}
		}
		if ($ia == false) {
			$tabpages[] = $ct;
		}
	}
} else {
	$curtab[0] = $tabpages[0];
	$_GET['tp'] = $tabpages[0];
	$currenttab = $tabpages[0];
}

if (!isset($currenttab2) or strlen($currenttab2) == 0) {
	$currenttab2 = "";
	foreach ($tabpages as $tp) {
		if (startwith($tp, $currenttab) == true and strlen($tp) > strlen($currenttab) and strpos($tp, "/") > 0 and toegang($tp, 0)) {
			$currenttab2 = substr($tp, strpos($tp, "/")+1);
			break;
		}
	}
}

$query = "SET lc_time_names='nl_NL';";
fnQuery($query);

function HTMLheader($refresh=0) {
	global $currenttab, $currenttab2;
	
	if (strlen(db_param("naamvereniging_afkorting")) > 0) {
		$title = db_param("naamvereniging_afkorting") . " | ";
	} else {
		$title = "";
	}
	if (strlen($currenttab) > 0 and strlen($currenttab2) > 0) {
		$title .= db_param("naamwebsite") . " | " . $currenttab . " | " . $currenttab2;
	} elseif (strlen($currenttab) > 0) {
		$title .= db_param("naamwebsite") . " | " . $currenttab;
	} else {
		$title .= db_param("naamwebsite");
	}

//	header('Content-type: text/html; charset=utf-8');
   if (!isset($_GET['header']) or $_GET['header'] != "no") {
		echo("<!DOCTYPE HTML>\n");
		echo("<html lang='nl'>\n");
		echo("<head>\n");
		echo("<meta charset='UTF-8'>\n");
		if (isset($_GET['tp']) and substr($_GET['tp'], 0, 7) == "Mailing") {
			echo("<script src='/includes/ckeditor/ckeditor.js'></script>\n");
		}
		if ($refresh == 1) {
			echo("<meta http-equiv='refresh' content='1200'>\n");
		}
		printf("<title>%s</title>\n", $title);
		if (file_exists("stylesheets/default.css")) {
			printf("<link rel='stylesheet' href='stylesheets/default.css?v%s'>\n", strftime("%Y%m%d%H%M", filectime("stylesheets/default.css")));
		}
		if (file_exists("stylesheets/kleur.css")) {
			printf("<link rel='stylesheet' href='stylesheets/kleur.css?v%s'>\n", strftime("%Y%m%d%H%M", filectime("stylesheets/kleur.css")));
		}
		echo("</head>\n");
		echo("<body>\n");
		echo("<div id='container'>\n");
		echo("<div id='header'>\n");
		printf("<h1>%s</h1>\n", db_param("naamwebsite"));
	
		echo("<div id='login'>\n");
		echo("<form name='Login' action='/auth.php' method='post'>");
		
		if ($_SESSION['lidid'] > 0) {
			printf("Welkom %s\n", htmlentities($_SESSION['roepnaamingelogde']));
			echo("<a href='/?tp=Zelfservice/Wijzigen wachtwoord'><img src='images/user.png' title='Wijzigen wachtwoord' alt='Wijzigen wachtwoord'></a>\n");
			if ($_SERVER['PHP_SELF'] == "/index.php") {
				printf("<a href='index.php?actie=uitloggen'><img src='images/power.png' title='Uitloggen' alt='Uitloggen'></a>\n");
			}
			if ($_SESSION['webmaster'] == 1) {
				if ($_SERVER['PHP_SELF'] != "/admin.php") {
					echo("<a href='admin.php'><img src='images/tool.png' title='Beheerdersmenu' alt='Tool'></a>\n");
				} elseif ($_SERVER['PHP_SELF'] == "/admin.php") {
					echo("<a href='index.php'><img src='images/home.png' title='Beginpagina' alt='Home'></a>\n");
				}
			}
		} elseif ($_SESSION['aantallogin'] > 0) {
			echo("Login: <input type='text' name='username' size=11>\n");
			echo("&nbsp;Wachtwoord: <input type='password' name='password' size=11>\n");
			echo("&nbsp;<input type='submit' value='Inloggen' name='Inloggen'>\n");
			echo("<input type='checkbox' name='cookie' value=1 title='Ingelogd blijven (werkt met cookies)'>\n");
		}
		if (strlen(db_param("url_eigen_help")) < 5) {
			echo("<a href='http://phprbm.telling.nl/uitleg.php'><img src='images/question.png' title='Uitleg gebruik en installatie' alt='Question mark'></a>\n");
		} else {
			printf("<a href='http://%s'><img src='images/question.png' title='Helpfunctie' alt='Question mark'></a>\n", db_param("url_eigen_help"));
		}
		echo("</form>\n");
		echo("</div>  <!-- Einde login -->\n");
		echo("<div class='clear'></div>\n");
		
		$xtra = "";
		if ($currenttab == "Overzicht lid" and isset($_GET['lidid']) and $_GET['lidid'] > 0) {
			$xtra = sprintf("lidid=%d", $_GET['lidid']);
		}
		fnDispMenu(1, $xtra);
		
		echo("</div> <!-- Einde header -->\n");
		echo("<div class='clear'></div>\n");
	}
} # HTMLheader

function HTMLfooter() {
	global $table_prefix, $navpad, $bestandsversie, $basisurl;

	if (!isset($_GET['header']) or $_GET['header'] != "no") {
		if (strlen(db_param("naamvereniging")) > 2) {
			if (strlen(db_param("urlvereniging")) > 3) {
				$cr = sprintf("<a href='http://%s'>%s</a>", db_param("urlvereniging"), db_param("naamvereniging"));
			} else {
				$cr = db_param("naamvereniging");
			}
		} else {
			$cr = "<a href='https://www.telling.nl'>&copy;&nbsp;S.Telling</a>";
		}
		echo("<div class='clear'></div>\n");
		
		$np = "";
		if (isset($navpad) and is_array($navpad)) {
			$i = 1;
			foreach ($navpad as $npl) {
				if (isset($npl['url']) and strlen($npl['url']) > 0 and $i < count($navpad)) {
					$np .= " | " . sprintf("<a href='%s'>%s</a>\n", $npl['url'], $npl['naam']);
				} else {
					$np .= " | " . $npl['naam'] . "\n";
				}
				$i++;
			}
		}
		printf("<div id='footer'>\n
				 <div id='footerleft'>%s</div>\n
				 <a href='%s'>%s</a> %s\n
				 <div id='footerright'>%s</div>\n
				 </div>  <!-- Einde footer  -->\n", $cr, $basisurl, db_param("naamwebsite"), $np, strftime("%e %B %Y", $bestandsversie));
		echo("</div>  <!-- Einde container -->\n");
		echo("</body>\n");
		echo("</html>\n");
	}
} # HTMLfooter

function isValidMailAddress($address, $LeegIsGoed=0){

  	if ($LeegIsGoed == 1 and strlen($address) == 0) {
		return true;
	} elseif (strlen(trim($address)) < 5) {
		return false;
   } else {
		return filter_var($address, FILTER_VALIDATE_EMAIL);
	}
} # isValidMailAddress

function fnDispEmail($strEmail, $strNaam="", $booInclLink=1) {
	if (strpos($strEmail, "<") > 2 and strlen($strnaam) < 3) {
   	$strNaam = trim(substr(trim($strEmail), 0, strpos($strEmail, "<")));
		$strEmail = substr(trim($strEmail), strpos($strEmail, "<")+1, -1);
   }
	$strRet = "";
	if (isValidMailAddress($strEmail, 0)) {
		if ($_SESSION['lidid'] > 0) {
			$strNaam = str_replace(" ", "%20", htmlentities($strNaam));
			if (strlen($strNaam) > 3 and $booInclLink == 1) {
				$strRet = sprintf('<a href="mailto:%1$s%%20<%2$s>">%2$s</a>', $strNaam, $strEmail);
			} elseif ($booInclLink == 1) {
				$strRet = sprintf('<a href="mailto:%1$s">%1$s</a>', $strEmail);
			} elseif (strlen($strNaam) > 3 and $booInclLink == 2) {
				$strRet = sprintf('<a href="mailto:%1$s%%20<%2$s>">%1$s</a>', $strNaam, $strEmail);
			} else {
				$strRet = $strEmail;
			}
		} else {
			$strHV = "";
			$iCounter=0;
			while ($iCounter < strlen($strEmail)) {
				$strHV .= chr(ord(substr($strEmail, $iCounter, 1))+1);
				$iCounter++;
			}
			$strRet = "<script>\n";
			$strRet .= "a0 = \"" . $strHV . "\"\n";

			$strRet .= "s0=\"\"\n";
			$strRet .= "for (i=0;i<a0.length; i++) {
					n=a0.charCodeAt(i)-1;
					s0+=String.fromCharCode(n);
					}\n";
			if (strlen($strNaam) > 3 and $booInclLink == 1) {
				$strRet .= "document.write('<a href=\"mailto: " . $strNaam . " <'+s0+'" . ">\">'+s0+'</a>');\n";
			} elseif ($booInclLink == 1) {
				$strRet .= "document.write('<a href=\"mailto: '+s0+'\">'+s0+'</a>');\n";
			} elseif (strlen($strNaam) > 3 and $booInclLink == 2) {
				$strRet .= "document.write('<a href=\"mailto: " . $strNaam . " <'+s0+'" . ">\">" . $strNaam . "</a>');\n";
			} else {
				$strRet .= "document.write(s0);\n";
			}
			$strRet .= "</script>\n";
		}
   } elseif (strlen($strEmail) > 0) {
   	$strRet = "Geen (geldig) e-mailadres";
   }
	return $strRet;
}

function getvar($nm, $riz="") {
	if (isset($_GET[$nm])) {
		return $_GET[$nm];
	} elseif (isset($_POST[$nm])) {
		return $_POST[$nm];
	} else {
		return $riz;
	}
}

function fnDispMenu($toonnivo=1, $xtra="") {
	global $tabpages, $currenttab, $currenttab2, $navpad, $typemenu;
	
	$ulopen = false;

	if (strlen($xtra) > 0 and startwith($xtra, "&amp;") == false) {
		$xtra = "&amp;" . $xtra;
	}
	
	if (db_param("typemenu") == 1 or db_param("typemenu") == 3) {
		printf("<div id='menu_l%s'>\n", $toonnivo);
	}

	if ((db_param("typemenu") == 2 or db_param("typemenu") == 3) and $toonnivo == 1) {  // Dit is het menu met hovering
		echo("<ul id='nav'>\n");
		$nivo = 0;
		foreach($tabpages as $tp) {
			if (toegang($tp, 0)) {
				$cl = "";
				if (strstr($tp, "/") === false) {
					if ($nivo > 0) {
						if ($ulopen) {
							echo("		</ul>\n	");
							$ulopen = false;
						}
						echo("</li>\n");
					}
					$nivo = 1;
					$tpd = $tp;
					$tp1 = $tp;
					$ulopen = false;
					if ($currenttab == str_replace("'", "", $tpd)) {
						$cl = " class='current'";
						$navpad[$nivo]['naam'] = $tpd;
						$navpad[$nivo]['url'] = sprintf("%s?tp=%s%s", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra);
					}
					printf("	<li%s><a href=\"%s?tp=%s%s\">%s</a>", $cl, $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra, $tpd);
				} elseif (strpos($tp, "/") > 1 and startwith($tp, $tp1) == true) {
					if ($nivo == 1) {
						echo("\n		<ul>\n");
						$ulopen = true;
					}
					$nivo = 2;
					$tpd = substr($tp, strpos($tp, "/")+1);
				
					if ($currenttab2 == str_replace("'", "", $tpd)) {
						$cl = " class='current'";
						$navpad[$nivo]['naam'] = $tpd;
						$navpad[$nivo]['url'] = sprintf("%s?tp=%s%s", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra);
					}
					printf("			<li%s><a href=\"%s?tp=%s%s\">%s</a></li>\n", $cl, $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra, $tpd);
				}
			}
		}
		if ($ulopen) {
			echo("		</ul>\n</li>\n");
		}
		echo("</ul>\n");
	}
	
	if (db_param("typemenu") == 1 or (db_param("typemenu") == 3 and $toonnivo == 2)) {
		$nivo = $toonnivo;
		printf("<ul class='tablist%d'>\n", $nivo);
		foreach($tabpages as $tp) {
			if (toegang($tp, 0)) {
				if ($nivo == 1 and strstr($tp, "/") === false) {
					$tpd = $tp;
				} elseif ($nivo == 2 and strpos($tp, "/") > 1 and $currenttab == substr($tp, 0, strlen($currenttab))) {
					$tpd = substr($tp, strpos($tp, "/")+1);
				} else {
					$tpd = "";
				}
				if (strlen($tpd) > 1) {
					$cl = "";
					if ($nivo == 1 and $currenttab == str_replace("'", "", $tp)) {
						$cl = " class='current'";
						$navpad[$nivo]['naam'] = $tpd;
						$navpad[$nivo]['url'] = sprintf("%s?tp=%s%s", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra);
					} elseif ($nivo == 2 and ($currenttab2 == str_replace("'", "", $tpd) or strlen($currenttab2) == 0)) {
						$cl = " class='current'";
						if (toegang($currenttab . "/" . str_replace("'", "", $tpd), 0)) {
							$currenttab2 = $tpd;
						}
						$navpad[$nivo]['naam'] = $tpd;
						$navpad[$nivo]['url'] = sprintf("%s?tp=%s%s", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra);
					}
					printf("<li%s><a href=\"%s?tp=%s%s\">%s</a></li>\n", $cl, $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra, $tpd);
				}
			}
		}
		echo("</ul>\n");
	}
	
	if (db_param("typemenu") == 1 or db_param("typemenu") == 3) {
		printf("</div> <!-- Einde menu_l%s -->\n", $toonnivo);
	}
	if ($nivo > 1) {
		echo("<div class='clear'></div>\n");
	}
}

function fnDisplayForm($row, $xtra_eind="", $id="formulier") {

	if (strlen($id) > 0) {
		$ret = sprintf("<div id='%s'>\n", $id);
	}
	if (isset($row)) {
		$ret .= "<table>\n";
		foreach($row as $key => $value) {
			if (substr($key, 0, 3) == "Kop") {
				$ret .= sprintf("<tr><th colspan=2>%s</th></tr>\n", $value);
			} elseif (substr($key, 0, 3) == "dte" and (!is_null($value))) {
				if (strtotime($value) == false) {
					$dv = $value;
				} else {
					$dv = strftime('%e %B %Y', strtotime($value));
				}
				$ret .= sprintf("<tr><td class='label'>%s</td><td class='datum'>%s</td></tr>\n", substr($key, 3), $dv);
			} elseif (substr($key, 0, 3) == "dtt" and (!is_null($value))) {
				$ret .= sprintf("<tr><td class='label'>%s</td><td class='datumtijd'>%s</td></tr>\n", substr($key, 3), strftime('%e %B %Y (%H:%M)', strtotime($value)));
			} elseif (substr($key, 0, 3) == "boo" and $value == 1) {
				$ret .= sprintf("<tr><td class='label'>%s</td><td>Ja</td></tr>\n", substr($key, 3));
			} elseif (substr($key, 0, 3) == "boo" and $value != 1) {
				$ret .= sprintf("<tr><td class='label'>%s</td><td>Nee</td></tr>\n", substr($key, 3));
			} elseif (substr($key, 0, 3) == "mem" and strlen($value) > 1) {
				$ret .= sprintf("<tr><td colspan=2>%s</td></tr\n>", htmlentities($value));
			} elseif (substr($key, 0, 3) == "eml" and strlen($value) > 1) {
				$ret .= sprintf("<tr><td class='label'>%s</td><td>%s</td></tr>\n", substr($key, 3), fnDispEmail($value, "", 1));
			} elseif (substr($key, 0, 2) != "nd" and strlen($value) > 0) {
				$ret .= sprintf("<tr><td class='label'>%s</td><td>%s</td></tr>\n", $key, htmlentities($value));
			}
		}
		$ret .= "</table>\n";
	} else {
		$ret .= "Er is geen record beschikbaar.";
	}
	if (strlen($xtra_eind) > 0) {
		$ret .= $xtra_eind;
	}
	if (strlen($id) > 0) {
		$ret .= sprintf("</div>  <!-- Einde %s -->\n", $id);
	}
	return $ret;
}

function fnDisplayFormLabels($row, $xtra_eind="", $id="formulier") {

	if (strlen($id) > 0) {
		$ret = sprintf("<div id='%s'>\n", $id);
	}
	if (isset($row)) {
		foreach($row as $key => $value) {
			if (substr($key, 0, 3) == "Kop") {
				$ret .= sprintf("<h3>%s</h3>\n", $value);
			} elseif (substr($key, 0, 3) == "dte" and (!is_null($value))) {
				if (strtotime($value) == false) {
					$dv = $value;
				} else {
					$dv = strftime('%e %B %Y', strtotime($value));
				}
				$ret .= sprintf("<label>%s</label><span class='datum'>%s</span>\n", substr($key, 3), $dv);
			} elseif (substr($key, 0, 3) == "dtt" and (!is_null($value))) {
				$ret .= sprintf("<label>%s<value class='datumtijd'>%s</value>\n", substr($key, 3), strftime('%e %B %Y (%H:%M)', strtotime($value)));
			} elseif (substr($key, 0, 3) == "boo" and $value == 1) {
				$ret .= sprintf("<label>%s</label><value>Ja</value>\n", substr($key, 3));
			} elseif (substr($key, 0, 3) == "boo" and $value != 1) {
				$ret .= sprintf("<label>%s</label><value>Nee</value>\n", substr($key, 3));
			} elseif (substr($key, 0, 3) == "mem" and strlen($value) > 1) {
				$ret .= sprintf("<value>%s</value>\n", htmlentities($value));
			} elseif (substr($key, 0, 3) == "eml" and strlen($value) > 1) {
				$ret .= sprintf("<span><label>%s</label>%s</span>\n", substr($key, 3), fnDispEmail($value, "", 1));
			} elseif (substr($key, 0, 2) != "nd" and strlen($value) > 0) {
				$ret .= sprintf("<span><label>%s</label>%s</span>\n", $key, htmlentities($value));
			}
		}
	} else {
		$ret .= "Er is geen record beschikbaar.";
	}
	if (strlen($xtra_eind) > 0) {
		$ret .= $xtra_eind;
	}
	if (strlen($id) > 0) {
		$ret .= sprintf("</div>  <!-- Einde %s -->\n", $id);
	}
	return $ret;
}

function fnDisplayTable($rows, $link="", $th="", $disptot=0, $link_lk="", $xtra_regel="", $class="lijst", $xtra_kolom="") {
	$ret = sprintf("<div class='%s'>\n", $class);
	if (isset($rows) and (count($rows) > 0 or strlen($xtra_regel) > 0)) {
		$ret .= "<table>\n";
		$dispkop = 1;
		$totaantal = 0;
		foreach($rows as $row) {
			if ($dispkop == 1) {
				$hv = "<tr>";
				foreach($row as $key => $value) {
					if (strstr("dte, dtt, dtl, dts, boo, int, cur, eml, num, mrg", substr($key, 0, 3)) !== false) {
						$hv .= sprintf("<th>%s</th>", str_replace("_", " ", substr($key, 3)));
					} elseif (substr($key, 0, 3) == "lnk") {
						if (strlen($link) > 1) {
							$hv .= "<th>&nbsp;</th>";
							$ktot[] = "geen";
						}
					} elseif (substr($key, 0, 3) == "llk") {
						if (strlen($link_lk) > 1) {
							$hv .= sprintf("<th colspan=%d>&nbsp;</th>", substr_count($link_lk, "<td")+1);
							$ktot[] = "geen";
						}
					} elseif (substr($key, 0, 2) != "nd") {
						$hv .= sprintf("<th>%s</th>", str_replace("_", " ", $key));
					}
					
					if (substr($key, 0, 3) == "cur") {
						$ktot[] = 0.00;
					} elseif (substr($key, 0, 3) == "int") {
						$ktot[] = 0;
					} elseif (substr($key, 0, 2) != "nd" and substr($key, 0, 3) != "lnk" and substr($key, 0, 3) != "llk") {
						$ktot[] = "geen";
					}
				}
				if (strlen($th) > 1) {
					$ret .= sprintf("<tr><th colspan=%d class='tabelkop'>%s</th></tr>\n", count($ktot), $th);
				}
				$ret .= $hv . "</tr>\n";
				$dispkop = 0;
			}
			$ret .= "<tr>";
			$kn = 0;
			foreach($row as $key => $value) {
				if (substr($key, 0, 3) == "dte") {
					if ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td class='datum'>%s</td>", strftime('%e %B %Y', strtotime($value)));
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
				} elseif (substr($key, 0, 3) == "dtl") {
					if ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td class='datumlang'>%s</td>", strftime('%A %e %B %Y', strtotime($value)));
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
				} elseif (substr($key, 0, 3) == "dtt") {
					if ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td class='datumtijd'>%s</td>", strftime('%e %B %Y (%H:%M)', strtotime($value)));
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
				} elseif (substr($key, 0, 3) == "dts") {
					if ((!is_null($value)) and $value > '1900-01-01' and strtotime($value) !== false) {
						$ret .= sprintf("<td class='datumtijd'>%s</td>", strftime('%e %B %Y (%H:%M:%S)', strtotime($value)));
					} elseif (strlen($value) > 0) {
						$ret .= sprintf("<td class='mededeling'>%s</td>", $value);
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
				} elseif (substr($key, 0, 3) == "int") {
					$ret .= sprintf("<td class='number'>%d</td>", $value);
					$ktot[$kn] += $value;
				} elseif (substr($key, 0, 3) == "cur") {
					$ret .= sprintf("<td class='number'>%s</td>", number_format($value, 2, ",", "."));
					$ktot[$kn] += $value;
				} elseif (substr($key, 0, 3) == "boo" and $value == 1) {
					$ret .= sprintf("<td>Ja</td>");
				} elseif (substr($key, 0, 3) == "boo" and $value != 1) {
					$ret .= sprintf("<td>Nee</td>");
				} elseif (substr($key, 0, 3) == "lnk") {
					if (strlen($link) > 1) {
						$href = str_replace("%d", $value, $link);
						$href = str_replace("%s", "<img src='images/zoom.png' alt='Details'>", $href);
						$ret .= "<td class='details'>" . $href . "</td>";
					}
				} elseif (substr($key, 0, 3) == "llk") {
					if ($value > 0 and strlen($link_lk) > 1) {
						$ret .= sprintf("<td class='details'>" . $link_lk . "</td>", $value, "<img src='images/zoom.png' alt='Details'>");
					} elseif (strlen($link_lk) > 1) {
						$ret .= "<td></td>\n";
					}
				} elseif (substr($key, 0, 3) == "eml") {
					$ret .= sprintf("<td>%s</td>", fnDispEmail($value, "", 1));
				} elseif (substr($key, 0, 3) == "num") {
					$ret .= sprintf("<td class='number'>%s</td>", $value);
				} elseif (substr($key, 0, 3) == "mrg") {
					$ret .= sprintf("<td>%s</td>", str_replace(" &amp; ", "<br>", htmlentities($value)));
				} elseif (substr($key, 0, 2) != "nd") {
					$ret .= sprintf("<td>%s</td>", htmlentities($value));
				}
				$kn++;
			}
			$ret .= "</tr>\n";
			$totaantal++;
		}
		if (strlen($xtra_regel) > 0) {
			$ret .= sprintf("<tr>%s</tr>\n", $xtra_regel);
		}
		if ($disptot > 0 and $totaantal > 0) {
			$ret .= "<tr>";
			for ($kn=0; $kn < count($ktot); $kn++) {
				if ($kn+1 == $disptot) {
					$ret .= sprintf("<th>%s records</th>", number_format($totaantal, 0, ",", "."));
				} elseif (gettype($ktot[$kn]) == "double") {
					$ret .= sprintf("<th class='number'>%s</th>", number_format($ktot[$kn], 2, ",", "."));
				} elseif (gettype($ktot[$kn]) == "integer") {
					$ret .= sprintf("<th class='number'>%s</th>", number_format($ktot[$kn], 0, ",", "."));
				} else {
					$ret .= "<th></th>";
				}
			}
			$ret .= "</tr>\n";
		}
		$ret .= "</table>\n";
	} else {
		$ret .= "<p class='mededeling'>Er zijn geen records beschikbaar.</p>\n";
	}
	$ret .= sprintf("</div>  <!-- Einde %s -->\n", $class);
	return $ret;
} # fnDisplayTable

function debug($tekst, $alleenwm=1, $alert=0, $logact=0) {
	if (is_array($tekst) or is_object($tekst)) {
		$tekst = print_r($tekst, true);
	}
	if ($_SERVER["HTTP_HOST"] == "phprbm.telling.nl") {
		$alleenwm = 0;
	}
	if ($alleenwm != 1 or (isset($_SESSION['webmaster']) and $_SESSION['webmaster'] == 1)) {
		if ($alert == 1) {
			printf("<script>alert('Debug: %s');</script>\n", $tekst);
		} else {
			printf("<p class='debug'>Debug: %s</p>\n", $tekst);
		}
	}
	if ($logact == 1) {
		db_logboek("add", "Debug: " . $tekst, $type=99);
	}
} # debug

function accessdate($sqldate) {
	if (strlen($sqldate) == 0) {
		return "NULL";
	} elseif (strtotime($sqldate) != false) {
		return "#" . date("m/d/Y", strtotime($sqldate)) . "#";
	} else {
		return false;
	}
} # accessdate

function startwith($haystack, $start) {
	if (substr($haystack, 0, strlen($start)) == $start) {
		return true;
	} else {
		return false;
	}
} # startwith

function change_month_to_uk($strdate) {

	$arrMonth[]=array("From" => "januari", "To" => "January");
	$arrMonth[]=array("From" => "februari", "To" => "February");
	$arrMonth[]=array("From" => "maart", "To" => "March");
	$arrMonth[]=array("From" => "april", "To" => "April");
	$arrMonth[]=array("From" => "mei", "To" => "May");
	$arrMonth[]=array("From" => "juni", "To" => "June");
	$arrMonth[]=array("From" => "juli", "To" => "July");
	$arrMonth[]=array("From" => "augustus", "To" => "August");
	$arrMonth[]=array("From" => "september", "To" => "September");
	$arrMonth[]=array("From" => "oktober", "To" => "October");
	$arrMonth[]=array("From" => "november", "To" => "November");
	$arrMonth[]=array("From" => "december", "To" => "December");
	$arrMonth[]=array("From" => "morgen", "To" => "tomorrow");
	$arrMonth[]=array("From" => "vandaag", "To" => "today");
	$arrMonth[]=array("From" => "gisteren", "To" => "yesterday");
	
	if (strlen($strdate) > 4) {
		foreach ($arrMonth as $key => $value) {
			$strdate = str_ireplace(" " . substr($value["From"], 0, 3) . " ", " " . $value["To"] . " ", $strdate);
			$strdate = str_ireplace(" " . $value["From"], " " . $value["To"], $strdate);
		}
	}
	return $strdate;
} # change_month_to_uk

function removetextblock($content, $start_block="<!-- Ingelogd -->", $end_block="<!-- /Ingelogd -->") {

	while (strpos($content, $start_block) !== false and strpos($content, $end_block) !== false) {
		$stb = strpos($content, $start_block);
		$etb = strpos($content, $end_block) + strlen($end_block);
		$content = substr($content, 0, $stb) . substr($content, $etb);
	}
	return $content;
	
} # removetextblock

function checked($val, $tp="box") {
	if (($val == 1 or $val == true) and $tp="radio") {
		return " checked='checked'";
	} elseif ($val == 1 or $val == true) {
		return " checked";
	} else {
		return "";
	}
} #checked

?>
