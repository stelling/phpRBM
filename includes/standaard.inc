<?php

date_default_timezone_set('Europe/Amsterdam');
setlocale(LC_ALL,'nl_NL.utf8');
header("Cache-Control: max-age=3600");
$dtfmt = new IntlDateFormatter(
    'nl_NL',
    IntlDateFormatter::LONG,
    IntlDateFormatter::LONG,
    'Europe/Amsterdam'
);

define("BASEDIR", str_replace("/includes", "", dirname(__FILE__)));

if (isset($_SERVER['HTTPS'])) {
	$basisurl = "https://" . $_SERVER['HTTP_HOST'];
} elseif (isset($_SERVER['HTTP'])) {
	$basisurl = "http://" . $_SERVER['HTTP_HOST'];
} else {
	$basisurl = "";
}

$sub = substr($_SERVER['PHP_SELF'], 0, strripos($_SERVER['PHP_SELF'], "/"));
if (strlen($sub) > 1) {
	$basisurl .= $sub;
}

define("BASISURL", $basisurl);
require(BASEDIR . '/config.php');

define("ARRGESLACHT", ["O" => "Onbekend", "V" => "Vrouwelijk", "M" => "Mannelijk", "N" => "Neutraal", "B" => "Bedrijf/Vereniging/Stichting"]);
define("ARRTL", ["Alle", "Leden", "Klosleden", "Voormalig leden"]);
define("ARRLEGITIMATIE", ["G" => "Geen", "I" => "Identiteitskaart", "P" => "Paspoort", "R" => "Rijbewijs"]);
define("ARRSOORTMEMO", ["A" => "Algemeen", "B" => "Bewaking", "D" => "Dieet", "E" => "Examen", "F" => "Financiën", "G" => "Gezondheid/medisch", "Z" => "Zwemervaring"]);
define("ARRTYPEONDERDEEL", ["A" => "Afdeling", "B" => "Bestuur", "C" => "Commissie", "E" => "Eigenschap", "F" => "Functionarissen", "G" => "Groep", "M" => "Materiaaluitleen", "O" => "Onderscheiding", "R" => "Rol", "S" => "Selectie", "T" => "Toestemming"]);
define("ARRTYPEDIPLOMA", ["B" => "Brevet", "C" => "Competentie", "D" => "Diploma", "I" => "Insigne", "L" => "Licentie", "M" => "Module", "P" => "PvB"]);
define("PASFOTOEXTENTIES", ["gif", "jpg", "jpeg", "png"]);
define("ARRSOORTEVENEMENT", ["B" => "Bewaking", "G" => "Gezellig/sociaal", "O" => "Opleiding", "R" => "Reddingsbrigade-activiteit", "V" => "Vergadering/overleg", "W" => "Wedstrijd"]);
define("ARRDLNSTATUS", ["A" => "Afgewezen", "B" => "Bevestigd", "G" => "Geen", "I" => "Ingeschreven", "J" => "Aangemeld", "N" => "Onafgemeld afwezig", "R" => "Reserve", "T" => "Onder voorbehoud", "X" => "Afgemeld/Afgezegd"]);
define("ARRPRESENTIESTATUS", ["A" => "Aanwezig", "H" => "Afgezegd door vereniging", "J" => "Aangemeld", "L" => "Te laat aanwezig", "R" => "Afwezig met bekende reden", "T" => "Onder voorbehoud aanwezig", "V" => "Les is vervallen", "X" => "Niet aanwezig", "Z" => "Ziek"]);
define("ZS_PRESENTIESTATUS", ["A" => "Aanwezig", "Z" => "Ziek", "R" => "Afwezig met andere reden"]);
define("ARRTYPEBACKUP", [1 => "phpRBM-tabellen", 2 => "Tabellen uit MS-Access", 3 => "Alle tabellen"]);
define("ARRTYPESTUK", ["A" => "Algemene Vergadering", "B" => "Beleidsstuk", "F" => "Formulier", "H" => "Handleiding", "P" => "Promotiemateriaal", "R" => "Regeling"]);
define("ARRKLEDINGMAAT", array("", "XS", "S", "M", "L", "XL", "XXL"));
define("ATTRCODEELEMENT", "autocomplete='off' autocorrect='off' autocapitalize='off' spellcheck='false'");
define("BEWAARTIJDNIEUWERECORDS", 120);
define("TABLECLASSES", "table table-bordered table-hover border-primary");
define("OPTIONSCOOKIES", array('expires' => time()+(3600*24*180), 'samesite' => 'Strict'));
define("OPTIONSCOOKIESLOGIN", array('expires' => time()+(3600*24*90), 'samesite' => 'Strict', 'secure' => true));

define("DTSHORT", "dd-MM-yyyy");
define("DTDAYMONTH", "d MMMM");
define("DTTEXT", "d MMMM yyyy");
define("DTPERSCAL", "EEEE d MMMM");
define("DTTEXTWD", "EEEE d MMMM yyyy");
define("DTLONG", "d MMMM yyyy (HH:mm)");
define("DTLONGWD", "EEEE d MMMM yyyy (HH:mm)");
define("DTLONGSEC", "d MMMM yyyy (HH:mm:ss)");
define("DTLONGWDSEC", "EEEE d MMMM yyyy (HH:mm:ss)");
define("DTHMS", "HH:mm:ss");

define("ICONBEWAAR", "<i class='bi bi-floppy' title='Bewaren'></i>");
define("ICONCHECK", "<i class='bi bi-check'></i>");
define("ICONLEDEN", "<i class='bi bi-people' title='Leden'></i></i>");
define("ICONLIJST", "<i class='bi bi-file-earmark-spreadsheet'></i>");
define("ICONLOGIN", "<i class='bi bi-arrow-right-circle'></i>");
define("ICONMISLUKT", "<i class='bi bi-x-circle' title='mislukt'></i>");
define("ICONMUTEER", "<i class='bi bi-pencil-square' title='Muteren'></i>");
define("ICONOVERZICHT", "<i class='bi bi-card-text'></i>");
define("ICONPDF", "<i class='bi bi-file-earmark-pdf' title='Toon PDF'></i>");
define("ICONPRINT", "<i class='bi bi-printer'></i>");
define("ICONSLUIT", "<i class='bi bi-door-closed' title='Bewaren en sluiten'></i>");
define("ICONSUBMIT", "<i class='bi bi-arrow-right-circle-fill'></i>");
define("ICONTOEVOEGEN", "<i class='bi bi-plus-circle' title='Toevoegen'></i>");
define("ICONVERSTUUR", "<i class='bi bi-envelope-at' title='Versturen'></i>");
define("ICONUNLOCK", "<i class='bi bi-unlock' title='Vrijgeven'></i>");
define("ICONVERVERS", "<i class='bi bi-arrow-clockwise' title='Ververs scherm'></i>");
define("ICONVERWERK", "<i class='bi bi-check2-square' title='Verwerk'></i>");
define("ICONVERWIJDER", "<i class='bi bi-trash' title='Verwijderen'></i>");
define("ICONVOLGENDE", "<i class='bi bi-skip-forward-circle' title='Volgende'></i>");
define("ICONVOORBEELD", "<i class='bi bi-eye' title='Voorbeeld'></i>");
define("ICONVORIGE", "<i class='bi bi-skip-backward-circle' title='Vorige'></i>");
define("ICONZEVEN", "<i class='bi bi-7-circle'></i>");

if (isset($classbutton) and strlen($classbutton) > 0) {
	define("CLASSBUTTON", $classbutton);
} else {
	define("CLASSBUTTON", "btn btn-light");
}

define("LIDIDWEBMASTERS", $lididwebmasters);
$bestandsversie = filectime(BASEDIR . "/index.php");
if (strlen($table_prefix) > 0 and substr($table_prefix, -1) != "_") {
	$table_prefix .= "_";
}
define("TABLE_PREFIX", $table_prefix);

if (isset($httpsverplicht) and $httpsverplicht == 1 and !isset($_SERVER['HTTPS']) and isset($_SERVER['HTTP_HOST'])) {
	header(sprintf("location: https:%s", $_SERVER['HTTP_HOST']));
}

session_cache_expire(25);
if (isset($_SESSION['lidid'])) {
	// Sessie bestaat al
} elseif (session_start() === FALSE or strlen(session_id()) == 0) {
	$mess = "De sessie kan niet gestart worden, hierdoor kan er op dit moment niet ingelogd worden.";
	debug($mess, 1);
}

if (!isset($_SESSION['lidid']) or $_SESSION['lidid'] == 0) {
	$_SESSION['lidid'] = 0;
	$_SESSION['lidnr'] = 0;
	$_SESSION['emailingelogde'] = "";
	$_SESSION['naamingelogde'] = "gast";
	$_SESSION['roepnaamingelogde'] = "gast";
	$_SESSION['webmaster'] = 0;
	$_SESSION['lidgroepen'] = "0";
}

$currenttab2 = "";
$currenttab3 = "";
$tp = $_GET['tp'] ?? "";
$tp = str_replace("'", "", trim($tp));
$tp = str_replace("\"", "", $tp);
$tp = str_replace("$", "", $tp);
$tp = str_replace("?", "", $tp);
$tp = str_replace("&", "", $tp);
if (mb_substr_count($tp, "/") == 2) {
	$currenttab = explode("/", $tp)[0];
	$currenttab2 = explode("/", $tp)[1];
	$currenttab3 = explode("/", $tp)[2];
	
} elseif (mb_substr_count($tp, "/") == 1) {
	$currenttab = explode("/", $tp)[0];
	$currenttab2 = explode("/", $tp)[1];
	
} elseif (strlen($tp) > 1) {
	$currenttab = $tp;
	
} else {
	if (substr($_SERVER['PHP_SELF'], -9) == "admin.php") {
		$currenttab = "Beheer logins";
	} else {
		$currenttab = "Vereniging";
	}
}
$currenttab = str_replace("'", "", $currenttab);
$currenttab2 = str_replace("'", "", $currenttab2);
$currenttab3 = str_replace("'", "", $currenttab3);

/* Include maatwerk */
$incl = BASEDIR . "/maatwerk/maatwerk";
if (file_exists($incl . ".inc")) {
	require_once($incl . ".inc");
} elseif (file_exists($incl . ".php")) {
	require_once($incl . ".php");
}
/* Einde includen maatwerk*/

/* Include Open source libaries */
require_once("phpmailer/PHPMailer.php");
require_once("phpmailer/SMTP.php");
require_once("phpmailer/Exception.php");
/* Einde includen open source libs */

foreach (array("database", "authentication", "afdelingen", "leden", "evenement", "financieel", "mailing", "diploma", "standaard", "webshop", "website") as $incl) {
	$inclpath = __DIR__ . "/" . $incl . ".php";
	if (file_exists($inclpath)) {
		if ($bestandsversie < filectime($inclpath)) {
			$bestandsversie = filectime($inclpath);
		}
		require_once($inclpath);
	} else {
		$inclpath = __DIR__ . "/" . $incl . ".inc";
		if (file_exists($inclpath)) {
			if ($bestandsversie < filectime($inclpath)) {
				$bestandsversie = filectime($inclpath);
			}
			require_once($inclpath);
		}
	}
}

if (WEBMASTER or strpos(BASISURL, "telling.nl") > 0) {
	error_reporting(E_ALL);
} else {
	error_reporting(0);
//	error_reporting(E_ALL);
}

$i_p = new cls_Parameter();
if (!isset($_SESSION['settings']['naamvereniging'])) {
	$i_p->vulsessie();
}
$i_p = null;

if ($_SESSION['lidid'] > 0) {
	(new cls_Login())->setlastactivity();
}
$dtfmt->setPattern(DTTEXT);
// $fileverinfo = 'templates/verenigingsinfo.html';

if (isset($tabpages)) {
	foreach ($tabpages as $tp) {
		$arrTP = explode("/", $tp);
		if (strlen($currenttab2) == 0 and count($arrTP) > 1 and $arrTP[0] == $currenttab and toegang($tp, 0)) {
			$currenttab2 = $arrTP[1];
		}
		if (strlen($currenttab3) == 0 and count($arrTP) > 2 and $arrTP[0] == $currenttab and $arrTP[1] == $currenttab2 and toegang($tp, 0)) {
			$currenttab3 = $arrTP[2];
		}
	}
} else {
	$currenttab2 = "";
	$currenttab3 = "";
	$tabpages = array();
}

function HTMLheader($breed=0) {
	global $currenttab, $currenttab2, $currenttab3;
	
	if (isset($_SESSION['settings']['title_head_html']) and strlen($_SESSION['settings']['title_head_html']) > 0) {
		$title = $_SESSION['settings']['title_head_html'];
	} else {
		if (isset($_SESSION['settings']['naamvereniging_afkorting']) and strlen($_SESSION['settings']['naamvereniging_afkorting']) > 0) {
			$title = $_SESSION['settings']['naamvereniging_afkorting'] . " | ";
		} else {
			$title = "";
		}
		$title .= $_SESSION['settings']['naamwebsite'];
	}

	if (strlen($currenttab) > 0) {
		$title .= " | " . $currenttab;
	}
	if (isset($currenttab3) and strlen($currenttab2) > 0) {
		$title .= " | " . $currenttab2;
	}
	if (isset($currenttab3) and strlen($currenttab3) > 0) {
		$title .= " | " . $currenttab3;
	}
	
	$printform = false;
	if ($currenttab2 == "Aftekenlijst" or $currenttab2 == "DL-lijst") {
		$printform = true;
	}

	if (!isset($_GET['header']) or $_GET['header'] != "no") {
		echo("<!DOCTYPE HTML>\n");
		echo("<html lang='nl'>\n");
		echo("<head>\n");
		if (file_exists("includes/javascript.js")) {
			printf("<script src='includes/javascript.js?v%d'></script>\n", filectime("includes/javascript.js"));
		}
		echo("<meta charset='UTF-8'>\n");
		echo("<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n");
		printf("<title>%s</title>\n", $title);
		echo("<link rel='stylesheet' href='//code.jquery.com/ui/1.13.1/themes/base/jquery-ui.css'>\n");
		if ($currenttab == "Vereniging" or $currenttab == "Agenda") {
			echo("<meta http-equiv='refresh' content=900>\n");
		}
		echo("<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css' rel='stylesheet'>\n");
		echo("<link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.2/font/bootstrap-icons.css'>\n");
		echo("<script src='https://code.jquery.com/jquery-3.7.1.js'></script>\n");
		if ($currenttab == "Vereniging" or $currenttab == "Eigen gegevens"  or $currenttab == "Ledenlijst" or $currenttab2 == "Overzicht lid") {
			echo("<script src='https://code.jquery.com/ui/1.13.1/jquery-ui.js'></script>\n");
		}

		if ($currenttab == "Mailing" or $currenttab == "Website") {
			$src = "/includes/tinymce/js/tinymce/tinymce.min.js";
			if (file_exists(BASEDIR . $src)) {
				$src = BASISURL . $src;
				
			} elseif ($_SERVER["HTTP_HOST"] == "phprbm.telling.nl" or 1 == 1) {
				$src = sprintf("https://cdn.tiny.cloud/1/%s/tinymce/7/tinymce.min.js", $_SESSION['settings']['mailing_tinymce_apikey']);
			} else {
				// mag weg na 1 juni 2024
				$src = sprintf("https://cdn.tiny.cloud/1/%s/tinymce/6/tinymce.min.js", $_SESSION['settings']['mailing_tinymce_apikey']);
			}
			printf("<script src='%s' referrerpolicy='origin'></script>\n", $src);
		}
		
		if (file_exists("default.css")) {
			printf("<link rel='stylesheet' href='default.css?v%d'>\n", filectime("default.css"));
		} elseif (file_exists("../default.css")) {
			printf("<link rel='stylesheet' href='../default.css?v%d'>\n", filectime("../default.css"));
		}			
		
		if (file_exists("maatwerk/kleur.css")) {
			printf("<link rel='stylesheet' href='maatwerk/kleur.css?v%d'>\n", filectime("maatwerk/kleur.css"));
		} elseif (file_exists("kleur.css")) {
			printf("<link rel='stylesheet' href='kleur.css?v%d'>\n", filectime("kleur.css"));
		}
		
		echo("</head>\n");
		echo("<body>\n");
		if ($breed == 0) {
			echo("<div class='container-xl'>\n");
		} else {
			echo("<div class='container-fluid'>\n");
		}
		
		if ($printform == false) {
		
			echo("<header>\n");
		
			if (file_exists("maatwerk/help.html")) {
				$l = BASISURL . "/maatwerk/help.html";
			} else {
				$l = "https://software.telling.nl/phpRBM/uitleg.php";
			}
			$htmlhelp = sprintf("<a href='%s'> <i class='bi bi-question-circle' style='font-size: 16pt;' title='Helpfunctie'></i></a>\n", $l);
	
			echo("<div id='welkom'>\n");
		
			if ($_SESSION['lidid'] > 0) {
				printf("<p id='welkomuser'>Welkom %s</p>\n", htmlentities($_SESSION['roepnaamingelogde']));
				echo("<a href='/?tp=Zelfservice/Wijzigen+wachtwoord'> <i class='bi bi-person-circle' style='font-size: 16pt;' title='Wijzigen wachtwoord'></i></a>\n");
				echo("<a href='index.php?actie=uitloggen'> <i class='bi bi-box-arrow-left' style='font-size: 16pt;' title='Uitloggen'></i></a>\n");
				if (WEBMASTER) {
					if (substr($_SERVER['PHP_SELF'], -9) == "index.php") {
						echo("<a href='admin.php'> <i class='bi bi-gear' style='font-size: 16pt;' title='Beheerdersmenu'></i></a>\n");
					} else {
						echo("<a href='index.php'> <i class='bi bi-house' style='font-size: 16pt;' title='Beginpagina'></i></a>\n");
					}
				}
				echo($htmlhelp);
			} else {
				echo($htmlhelp);
			}

			echo("</div>  <!-- Einde welkom -->\n");
		
			printf("<h1 id='naamwebsite'><a href='%s'>%s</a></h1>\n", BASISURL, $_SESSION['settings']['naamwebsite']);
		
			$xtra = "";
			if ($currenttab == "Overzicht lid" and isset($_GET['lidid']) and $_GET['lidid'] > 0) {
				$xtra = sprintf("lidid=%d", $_GET['lidid']);
			}
		
			echo("</header>\n");
		
			fnDispMenu(1, $xtra);
		}
	}
} # HTMLheader

function HTMLfooter() {
	global $navpad, $bestandsversie, $currenttab, $currenttab2, $currenttab3, $dtfmt;

	if (!isset($_GET['header']) or $_GET['header'] != "no") {
		if (isset($_SESSION['settings']['naamvereniging']) and strlen($_SESSION['settings']['naamvereniging']) > 2) {
			if (strlen($_SESSION['settings']['urlvereniging']) > 3) {
				$cr = sprintf("<a href='%s'>%s</a>", $_SESSION['settings']['urlvereniging'], $_SESSION['settings']['naamvereniging']);
			} else {
				$cr = $_SESSION['settings']['naamvereniging'];
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
		echo("<footer>\n");
		printf("<a href='%s'>%s</a> %s\n", BASISURL, $_SESSION['settings']['naamwebsite'], $np);
		$dtfmt->setPattern(DTTEXT);
		printf("<div id='footerright'>%s</div>\n", $dtfmt->format($bestandsversie));
		echo("</footer>\n");
		echo("</div>  <!-- Einde container -->\n");
		echo("</body>\n");
		echo("</html>\n");
	}
} # HTMLfooter

function fnDispMenu($toonnivo=1, $xtra="") {
	global $tabpages, $currenttab, $currenttab2, $currenttab3, $navpad;
	
	$tpd = ""; // Dit wordt getoond in het menu.
	$ulopen = false;

	if (strlen($xtra) > 0 and startwith($xtra, "&") == false) {
		$xtra = "&" . $xtra;
	}
	
	if ($toonnivo == 1) {
		echo("<nav>\n");
	}
		
	$nivo = $toonnivo;
	printf("<div id='menu_l%d'>\n", $toonnivo);
	$itp = 1;
	foreach($tabpages as $tp) {
		if ($nivo == 1 and strstr($tp, "/") === false) {
			$tpd = $tp;
		} elseif ($nivo == 2 and mb_substr_count($tp, "/") == 1 and startwith($tp, $currenttab . "/")) {
			$tpd = str_replace($currenttab . "/", "", $tp);
		} elseif ($nivo == 3 and mb_substr_count($tp, "/") == 2 and startwith($tp, $currenttab . "/") and strpos($tp, $currenttab2) > 1) {
			$tpd = str_replace($currenttab2 . "/", "", str_replace($currenttab . "/", "", $tp));
			
//		} elseif ($currenttab2 == "Overzicht lid") {

		} else {
			$tpd = "";
		}
		if (strlen($tpd) > 1) {
			$cl = "";
			if ($nivo == 1 and $currenttab == str_replace("'", "", $tp)) {
				$cl = " class='active'";
				$navpad[$nivo]['naam'] = $tpd;
				$navpad[$nivo]['url'] = sprintf("%s?tp=%s%s", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra);
			} elseif ($nivo == 2 and ($currenttab2 == str_replace("'", "", $tpd) or strlen($currenttab2) == 0)) {
				$cl = " class='active'";
				if (toegang($currenttab . "/" . str_replace("'", "", $tpd), 0)) {
					$currenttab2 = str_replace("'", "", $tpd);
				}
				$navpad[$nivo]['naam'] = $tpd;
				$navpad[$nivo]['url'] = sprintf("%s?tp=%s%s", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra);
			} elseif ($nivo == 3 and ($currenttab3 == str_replace("'", "", $tpd) or strlen($currenttab3) == 0)) {
				$cl = " class='active'";
				$navpad[$nivo]['naam'] = $tpd;
				$navpad[$nivo]['url'] = sprintf("%s?tp=%s%s", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra);
			}
			printf("<a href=\"%s?tp=%s%s\"%s>%s</a>\n", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra, $cl, $tpd);
			$itp++;
		}
	}
	echo("</div>\n");
	
	if ($toonnivo == 1 and strlen($currenttab2) == 0) {
		echo("</nav>\n");
	} elseif ($toonnivo == 2 and strlen($currenttab3) == 0) {
		echo("</nav>\n");
	} elseif ($toonnivo == 3) {
		echo("</nav>\n");
	}
	
	if (WEBMASTER and $_SESSION['settings']['versie'] < strtotime("-6 month") and $_SERVER['PHP_SELF'] == "/admin.php") {
		(new cls_Logboek())->add("Deze versie is ouder dan 6 maanden, update naar de nieuwste versie.", 99, -1, 2);
	}

}  # fnDispMenu

function isValidMailAddress($address, $p_LeegIsGoed=0){

  	if ($p_LeegIsGoed == 1 and strlen($address) == 0) {
		return true;
	} elseif (strlen(trim($address)) < 5) {
		return false;
   } else {
   	$address = str_replace(";", ",", $address);
   	if (strpos($address, ",") === false) {
			return filter_var($address, FILTER_VALIDATE_EMAIL);
   	} else {
   		$rv = true;
   		foreach (explode(",", $address) as $e) {
   			if (filter_var($e, FILTER_VALIDATE_EMAIL) == false) {
   				$rv = false;
   			}
   		}
   		return $rv;
   	}
	}
} # isValidMailAddress

function fnDispEmail($strEmail, $strNaam="", $booInclLink=1) {
	
	/*
	booInclLink
	0 = zonder link
	1 = met gecodeerde link, indien gebruiker niet is ingelogd.
	2 = met HTML-link
	*/
	
	if (strpos($strEmail, "<") > 2 and strlen($strnaam) < 3) {
   	$strNaam = trim(substr(trim($strEmail), 0, strpos($strEmail, "<")));
		$strEmail = substr(trim($strEmail), strpos($strEmail, "<")+1, -1);
   }
	$strRet = "";
	if (isValidMailAddress($strEmail, 0)) {
		if ($_SESSION['lidid'] > 0) {
			$strNaam = str_replace(" ", "%20", htmlentities($strNaam));
			if ($booInclLink >= 1) {
				$strRet = sprintf('<a href="mailto:%1$s">%1$s</a>', $strEmail);
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
	
}  # fnDispEmail

function getvar($nm, $riz="") {
	if (isset($_GET[$nm])) {
		return $_GET[$nm];
	} elseif (isset($_POST[$nm])) {
		return $_POST[$nm];
	} elseif (isset($_SESSION[$nm])) {
		return $_SESSION[$nm];
	} else {
		return $riz;
	}
}

function fnDisplayFormLabels($row, $xtra_eind="", $id="") {
	global $dtfmt;
	
	$dtfmt->setPattern(DTTEXT);

	if (strlen($id) > 0) {
		$ret = sprintf("<div id='%s'>\n", $id);
	}
	if (isset($row) and $row != null) {
		foreach($row as $key => $value) {
			if (substr($key, 0, 3) == "Kop") {
				$ret .= sprintf("<h2>%s</h2>\n", $value);
			} elseif (substr($key, 0, 3) == "dte" and (!is_null($value))) {
				if ($value <= '1900-01-01') {
					$dv = "";
				} elseif (strtotime($value) == false) {
					$dv = $value;
				} else {
					$dv = $dtfmt->format(strtotime($value));
				}
				$ret .= sprintf("<label class='form-label'>%s</label><p>%s</p>\n", substr($key, 3), $dv);
			} elseif (substr($key, 0, 3) == "dtt" and (!is_null($value))) {
				$dtfmt->setPattern(DTLONG);
				$ret .= sprintf("<label class='form-label'>%s<p>%s</p>\n", substr($key, 3), $dtfmt->format(strtotime($value)));
			} elseif (substr($key, 0, 3) == "boo" and $value == 1) {
				$ret .= sprintf("<label class='form-label'>%s</label><p>Ja</p>\n", substr($key, 3));
			} elseif (substr($key, 0, 3) == "boo" and $value != 1) {
				$ret .= sprintf("<label class='form-label'>%s</label><p>Nee</p>\n", substr($key, 3));
			} elseif (substr($key, 0, 3) == "mem" and strlen($value) > 1) {
				$ret .= sprintf("<value>%s</value>\n", htmlentities($value));
			} elseif (substr($key, 0, 3) == "eml" and strlen($value) > 1) {
				$ret .= sprintf("<label class='form-label'>%s</label><p>%s</p>\n", substr($key, 3), fnDispEmail($value, "", 1));
			} elseif (substr($key, 0, 2) != "nd" and strlen($value) > 0) {
				$ret .= sprintf("<label class='form-label'>%s</label><p>%s</p>\n", $key, htmlentities($value));
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
}  # fnDisplayFormLabels

function fnDisplayTable($rows, $kols=null, $th="", $disptot=0, $xtra_regel="", $p_id="", $p_class="", $p_selected=-1, $p_varrowclass="") {
	global $currenttab, $currenttab2, $dtfmt;
	global $i_lo;
	
	$ret = "";
	$kol_pk = "";
	if (isset($rows) and (count($rows) > 0 or strlen($xtra_regel) > 0)) {
		if (strlen($p_class) > 0) {
			$cl = $p_class . " " . TABLECLASSES;
		} else {
			$cl = TABLECLASSES;
		}
		if (strlen($p_id) > 0) {
			$ret .= sprintf("<table id='%s' class='%s'>\n", $p_id, $cl);
		} else {
			$ret .= sprintf("<table class='%s'>\n", $cl);
		}
		$dispkop = 1;
		$totaantal = 0;
		
		if (strlen($th) > 1) {
			if (count($rows) > 3) {
				$t = sprintf(" title='%d rijen'", count($rows));
			} else {
				$t = "";
			}
			$ret .= sprintf("<caption%s>%s</caption>\n", $t, $th);
		}
		$ret .= "<thead>\n<tr>";
		
		if ($kols == "logboek") {
			$kols = fnStandaardKols($kols, 1, $rows);
		} elseif ($kols == null and count($rows) > 0) {
			$kn = 0;
			foreach($rows[0] as $key => $value) {
				$kols[$kn]['columnname'] = $key;
				$kols[$kn]['headertext'] = str_replace("_", " ", $key);
				if ($key == "RecordID") {
					$kols[$kn]['type'] = "pk";
				}
				$kn++;
			}
		}
		
		if ($kols != null) {
			ksort($kols);
		
			foreach ($kols as $k => $kol) {
				$kols[$k]['columnname'] = $kols[$k]['columnname'] ?? "";
				$kols[$k]['headertext'] = $kols[$k]['headertext'] ?? "";
				$kols[$k]['type'] = $kols[$k]['type'] ?? "text";
				$kols[$k]['class'] = $kols[$k]['class'] ?? "";
				
				if (strlen($kols[$k]['columnname']) == 0 and strlen($kols[$k]['headertext']) == 0 and !isset($kols[$k]['subqry'])) {
					$kol['skip'] = true;
					$kols[$k]['skip'] = true;
					$iq = false;
					
				} else {
					if (strlen($kols[$k]['headertext']) == 0) {
						if (isset($kols[$k]['link'])) {
							$kols[$k]['headertext'] = "&nbsp;";
							$kols[$k]['type'] = "link";
						} else {
							$kols[$k]['headertext'] = str_replace("_", " ", $kols[$k]['columnname']);
						}
					} elseif (strlen($kols[$k]['columnname']) == 0) {
						$kol['columnname'] = $kol['headertext'];
						$kols[$k]['columnname'] = $kol['headertext'];
					}
				
					if (!isset($kol['columnname']) or strlen($kol['columnname']) == 0) {
	//	debug($k . ": geen columnname");
					}
					
					$iq = false;
					if (count($rows) > 0) {
						foreach($rows[0] as $key => $val) {
							if ($key == $kol['columnname']) {
								$iq = true;
							}
						}
					}

					if (isset($kol['subqry']) and strlen($kol['subqry']) > 6) {
						$iq = true;
					 
					} elseif ($iq and strlen($kol['columnname']) > 0 and strlen(max(array_column($rows, $kol['columnname']))) == 0) {
						$iq = false;
					}
				}
				
				if (strlen($kols[$k]['columnname']) > 0 and strlen($kols[$k]['class']) == 0) {
					$kols[$k]['class'] = strtolower(str_replace(" ", "_", $kols[$k]['columnname']));
				}
				
				if (isset($kols[$k]['skip']) and $kols[$k]['skip'] == true) {
					$kol['skip'] = true;
					
				} elseif ($iq) {
					if (substr($kols[$k]['headertext'], 0, 3) == "boo") {
						$kols[$k]['headertext'] = substr($kol['headertext'], 3);
						$kols[$k]['type'] = "boolean";
					} elseif (substr($kols[$k]['headertext'], 0, 3) == "int") {
						$kols[$k]['headertext'] = substr($kol['headertext'], 3);
						$kols[$k]['type'] = "integer";
					} elseif (substr($kols[$k]['headertext'], 0, 3) == "cur") {
						$kols[$k]['headertext'] = substr($kol['headertext'], 3);
						$kols[$k]['type'] = "bedrag";
					} elseif (substr($kols[$k]['headertext'], 0, 3) == "dte") {
						$kols[$k]['headertext'] = substr($kol['headertext'], 3);
						$kols[$k]['type'] = "date";
					}

					if ($kols[$k]['type'] != "text" and $kols[$k]['type'] != "link") {
						if (strlen($kols[$k]['class']) > 0) {
							if (strpos($kols[$k]['class'], $kols[$k]['type']) === false) {
								$kols[$k]['class'] .= " " . $kols[$k]['type'];
							}
						} else {
							$kols[$k]['class'] = $kols[$k]['type'];
						}
					}
					
					if (isset($kols[$k]['class']) and strlen($kols[$k]['class']) > 0) {
						$kol_class = sprintf(" class='%s'", $kols[$k]['class']);
					} elseif (isset($kols[$k]['columnname'])) {
						$kols[$k]['class'] = strtolower(str_replace(" ", "_", $kols[$k]['columnname']));
						$kol_class = sprintf(" class='%s'", $kols[$k]['class']);
					} else {
						$kol_class = "";
					}
					
					$t = "";
					if ($kols[$k]['type'] == "date" or $kols[$k]['type'] == "bedrag") {
						$ag = 0;
						foreach (array_column($rows, $kols[$k]['columnname']) as $val) {
							if ($kols[$k]['type'] == "bedrag" and $val != 0) {
								$ag++;
							} elseif (strlen($val) >= 10) {
								$ag++;
							}
						}
						if ($ag >= 0 and $ag < count($rows)) {
							if ($ag == 1) {
								$t = " title='1 rij met waarde'";
							} else {
								$t = sprintf(" title='%d rijen met waarde'", $ag);
							}
						}
					}
					if (isset($kol['sortcolumn'])) {
						if (isset($_SESSION['huidsort']) and $_SESSION['huidsort'] < 50 and $_SESSION['huidsort'] == $k and $k >= 0) {
							$skn = $k + 50;
						} else {
							$skn = $k;
						}
						if (strlen($currenttab2) > 0) {
							$ct = $currenttab . "/" . $currenttab2;
						} else {
							$ct = $currenttab;
						}
						$sortlink = sprintf("%s?tp=%s&sort=%d", $_SERVER["PHP_SELF"], $ct, $skn);
						$ret .= sprintf("<th%s%s><a href='%s'>%s</a></th>", $kol_class, $t, $sortlink, $kols[$k]['headertext']);
					} else {
						$ret .= sprintf("<th%s%s>%s</th>", $kol_class, $t, $kols[$k]['headertext']);
					}
				} else {
					$kols[$k]['skip'] = true;
					$kol['skip'] = true;
				}
				if ($kols[$k]['type'] == "pk") {
					$kol_pk = $kols[$k]['columnname'];
				}
				$ktot[$k] = 0;
//				debug($kols[$k]['columnname'] . ": " . $kols[$k]['type']);
			}
		}
		$ret .= "</tr>\n</thead>\n<tbody>\n";
		
		foreach($rows as $row) {
			$kn = 0;
			if ($p_selected > 0 and $p_selected == $row->$kol_pk) {
				$ret .= "<tr class='table-active'>";
			} elseif (strlen($p_varrowclass) > 0 and strpos($p_varrowclass, "->") !== false) {
				$in = substr($p_varrowclass, 0, strpos($p_varrowclass, "->"));
				$vn = substr($p_varrowclass, strpos($p_varrowclass, "->")+2);
				$$in->vulvars($row->RecordID);
				$cl = $$in->$vn;
				if (strlen($cl) > 0) {
					$ret .= sprintf("<tr class='%s'>", $cl);
				} else {
					$ret .= "<tr>";
				}
			} else {
				$ret .= "<tr>";
			}
			foreach ($kols as $kn => $kol) {
				if (isset($kol['skip']) and $kol['skip'] == true) {
					// skipped
				} elseif (isset($kol['columnname'])) {
					$key = $kol['columnname'];
					if (isset($kol['skip']) and $kol['skip'] == true) {
						$value = null;
					} elseif (isset($kol['subqry']) and strlen($kol['subqry']) > 6) {
						$sq = str_replace("%d", $row->{$key}, $kol['subqry']);
						$value = (new cls_db_base())->scalar($sq);
						$kol['skip'] = false;
					} else {
						$value = $row->{$key};
						$kol['skip'] = false;
					}
				} else {
					$key = "";
					$kol['skip'] = true;
					$value = null;
				}
				$kol_edit = 0;
				if (isset($kols[$kn]['edit'])) {
					$kol_edit = $kols[$kn]['edit'];
				}
				$kol_link = "";
				$kol_class = $kols[$kn]['class'];
				
				if (isset($kols[$kn]['type'])) {
					$kol_type = $kols[$kn]['type'];
				} elseif (isset($kols[$kn]['link'])) {
					$kol_type = "link";
					$kols[$kn]['type'] = "link";
				} else {
					$kol_type = "text";
				}

				$row_title = "";
				if (isset($kols[$kn]['columntitle']) and strlen($kols[$kn]['columntitle']) > 0 and isset($row->{$kols[$kn]['columntitle']}) and strlen($row->{$kols[$kn]['columntitle']}) > 0) {
					$row_title = sprintf(" title=\"%s\"", str_replace("\"", "'", $row->{$kols[$kn]['columntitle']}));
				}
				
//				debug($kols[$kn]['columnname'] . " / " . $kols[$kn]['type']);
				if ($kols[$kn]['type'] == "link") {
					if (startwith($kols[$kn]['link'], "<a href") == true) {
						$kol_link = $kols[$kn]['link'];
					} else {
						$kol_link = "<a href='" . $kols[$kn]['link'] . "'";
						if (isset($kols[$kn]['title'])) {
							$kol_link .= sprintf(" title='%s'", $kols[$kn]['title']);
						} elseif ($kol_class == "trash") {
							$kol_link .= " title='Verwijderen'";
						}
						$kol_link .= ">";
						if (substr($kol_class, 0, 7) === "details") {
							$kol_link .= "<i class='bi bi-file-text'></i></a>";
							
						} elseif (substr($kol_class, 0, 12) === "detailregels") {
							$kol_link .= "<i class='bi bi-chevron-double-down'></i></a>";
							
						} elseif (substr($kol_class, 0, 10) === "detailslid") {
							$kol_link .= "<i class='bi bi-person-lines-fill' title='Gegevens van het lid'></i></a>";
							
						} elseif ($kol_class === "pdf") {
							$kol_link .= sprintf("%s</a>", ICONPDF);
							
						} elseif ($kol_class == "leden") {
							$kol_link .= sprintf("%s</a>", ICONLEDEN);
							
						} elseif (substr($kol_class, 0, 7) == "muteren") {
							$kol_link .= "<i class='bi bi-pencil-square' title='Muteren'></i></a>";
							
						} elseif ($kol_class == "sendmail") {
							$kol_link .= "<i class='bi bi-envelope-at' title='Verstuur e-mail'></i></a>";
							
						} elseif ($kol_class == "viewmail") {
							$kol_link .= "<i class='bi bi-file-text' title='Bekijk e-mail'></i></a>";
							
						} elseif ($kol_class == "verwerk") {
							$kol_link .= sprintf("%s</a>", ICONVERWERK);
							
						} elseif ($kol_class == "trash") {
							$kol_link .= sprintf("%s</a>", ICONVERWIJDER);
							
						} elseif ($kol_class == "unlock") {
							$kol_link .= "<i class='bi bi-unlock' alt='Account vrijgeven'></i></a>";
							
						} else {
							$kol_link .= "&nbsp;&nbsp;&nbsp;</a>";
						}
					}
				}
				if (strlen($kol_class) > 0 and $kol_class != "mislukt") {
					$kol_class = sprintf(" class='%s'", $kol_class);
				}
				$kol_ml = $kol['maxlength'] ?? 100;
				
				if ($kol['skip'] == true) {
					// Deze kolom moet worden overgeslagen.
						
				} elseif ($kols[$kn]['type'] == "chk") {
					if ($value == 1) {
						$ret .= sprintf("<td%s>Ja</td>", kol_class);
					} else {
						$ret .= sprintf("<td%s>Nee</td>", kol_class);
					}

				} elseif ($kols[$kn]['type'] == "date" or $kols[$kn]['type'] == "DTTEXT") {
					// 1 januari 2020
					if (strlen($value) > 15 and substr($value, 11, 5) > "00:00") {
						$dtfmt->setPattern(DTLONG);
						$t = sprintf(" title='%s'", $dtfmt->format(strtotime($value)));
					} else {
						$t = "";
					}
					if ((!is_null($value)) and $value > '1900-01-01' and strtotime($value) !== false) {
						$dtfmt->setPattern(DTTEXT);
						$ret .= sprintf("<td%s%s>%s</td>", $kol_class, $t, $dtfmt->format(strtotime($value)));
					} elseif ((!is_null($value)) and strlen($value) > 0) {
						$ret .= sprintf("<td%a>%s</td>", $kol_class, $value);
					} else {
						$ret .= sprintf("<td%s></td>", $kol_class);
					}
					
				} elseif ($kols[$kn]['type'] == "dtk" or $kols[$kn]['type'] == "dateshort") {
					
					if ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td%s>%s</td>", $kol_class, date('d/m/Y', strtotime($value)));
					} else {
						$ret .= sprintf("<td%s></td>", $kol_class);
					}
				} elseif ($kol_type == "dtl" or $kol_type == "DTTEXTWD") {
					$dtfmt->setPattern(DTTEXTWD);
					if ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td%s>%s</td>", $kol_class, $dtfmt->format(strtotime($value)));
					} else {
						$ret .= sprintf("<td%s></td>", $col_class);
					}
				} elseif ($kols[$kn]['type'] == "dtt" or $kols[$kn]['type'] == "datetime" or $kols[$kn]['type'] == "DTLONG") {
					$dtfmt->setPattern(DTLONG);
					if ((!is_null($value)) and $value > '1900-01-01' and strlen($kol_pk) > 0) {
						$ret .= sprintf("<td id='%s_%d'%s>%s</td>", $key, $row->$kol_pk, $kol_class, $dtfmt->format(strtotime($value)));
					} elseif (!is_null($value) and $value > '1900-01-01') {
						$ret .= sprintf("<td%s>%s</td>", $kol_class, $dtfmt->format(strtotime($value)));
					} else {
						$ret .= sprintf("<td%s></td>", $kol_class);
					}
					
				} elseif ($kols[$kn]['type'] == "dts" or $kols[$kn]['type'] == "datetimesecond" or $kols[$kn]['type'] == "DTLONGSEC") {
					if ((!is_null($value)) and $value > '1900-01-01' and strtotime($value) !== false) {
						$dtfmt->setPattern(DTLONGSEC);
						$ret .= sprintf("<td%s>%s</td>", $kol_class, $dtfmt->format(strtotime($value)));
					} elseif (strlen($value) > 0) {
						$ret .= sprintf("<td%s>%s</td>", $kol_class, $value);
					} else {
						$ret .= sprintf("<td%s></td>", $kol_class);
					}
					
				} elseif ($kols[$kn]['type'] == "DTHMS")	{
					// Tijd met seconden, datum wordt alleen toegevoegd als deze ongelijk is aan vandaag
					if ((!is_null($value)) and $value > '1900-01-01' and strtotime($value) !== false) {
						if (date("Y-m-d") == date("Y-m-d", strtotime($value))) {
							$dtfmt->setPattern(DTHMS);
						} else {
							$dtfmt->setPattern("d MMMM " . DTHMS);
						}
						$ret .= sprintf("<td%s>%s</td>", $kol_class, $dtfmt->format(strtotime($value)));
					} elseif (strlen($value) > 0) {
						$ret .= sprintf("<td%s>%s</td>", $kol_class, $value);
					} else {
						$ret .= sprintf("<td%s></td>", $kol_class);
					}		
										
				} elseif ($kols[$kn]['type'] == "int" or $kols[$kn]['type'] == "integer") {
					if (strlen($kol_class) == 0) {
						$kol_class = " class='number'";
					} else {
						$kol_class = sprintf(" class='number %s'", $kol['class']);
					}
					$ret .= sprintf("<td%s>%d</td>", $kol_class, $value);
					$ktot[$kn] += intval($value);
					
				} elseif ($kol_type == "currency" or $kol_type == "bedrag") {
					$ret .= sprintf("<td%s>%s</td>", $kol_class, number_format($value, 2, ",", "."));
					$ktot[$kn] += $value;

				} elseif ($kol_type == "boolean" or $kol_type == "checkbox") {
					if ($value == 1) {
						$ret .= "<td class='boolean'>Ja</td>";
					} elseif ($value == 0 or $value == -1) {
						$ret .= "<td class='boolean'>Nee</td>";
					} else {
						$ret .= "<td>Onbekend</td>";
					}
					
				} elseif (($kol_type == "lnm" or $kol_type == "lidnaam") and strlen($value) > 0) {
					if (is_numeric($value)) {
						$ret .= sprintf("<td%s>%s</td>", $kol_class, (new cls_lid())->naam($value, "onbekend"));
					} else {
						$ret .= sprintf("<td%s>%s</td>", $kol_class, $value);
					}
					
				} elseif ($kol_type == "onderdeelnaam" and strlen($value) > 0) {
					if (is_numeric($value)) {
						$ret .= sprintf("<td%s>%s</td>", $kol_class, (new cls_Onderdeel())->naam($value, "onbekend", 25));
					} else {
						$ret .= sprintf("<td%s>%s</td>", $kol_class, $value);
					}
					
				} elseif (strlen($kol_link) > 1) {
					if ($value > 0) {
						$href = str_replace("%d", $value, $kol_link);
						$href = str_replace("%s", "&nbsp;&nbsp;&nbsp;", $href);
						$ret .= sprintf("<td%s>%s</td>", $kol_class, $href);
					} else {
						$ret .= "<td></td>";
					}
						
				} elseif ($kol_type == "link") {
					if ($value > 0 and isset($kols[-1]['link'])) {
						if (startwith($kols[-1]['link'], "<a href") == true) {
							$kol_link = $kols[-1]['link'];
						} else {
							$kol_link = "<a href='" . $kols[-1]['link'] . "'>&nbsp;&nbsp;&nbsp;</a>";
						}
						$href = str_replace("%d", $value, $kol_link);
						$ret .= sprintf("<td%s>%s</td>", $kol_class, $href);
					} elseif (isset($kols[-1]['link'])) {
						$ret .= "<td></td>\n";
					}
					
				} elseif ($kol_type == "click") {
					if ($kol_class == "mislukt" and $value > 0) {
						if (isset($kol['title']) and strlen($kol['title']) > 0) {
							$t = sprintf(" title=\"%s\"", $kol['title']);
						} else {
							$t = "";
						}
						$ret .= sprintf("<td id='mislukt_%d'%s onClick=\"%s;\"%s>%s</td>", $value, $t,  str_replace("%d", $value, $kol['onclick']), $kol_class, ICONMISLUKT);
					} else {
						$ret .= "<td></td>\n";
					}
					
				} elseif ($kol_type == "eml" or $kol_type == "email") {
					if ($kol_edit == 0) {
						$ret .= sprintf("<td%s>%s</td>", $kol_class, fnDispEmail($value, "", 1));
					} else {
						$ret .= sprintf("<td><input type='email' name='%s_%d' value='%s'%s %s></td>", str_replace(" ", "_", substr($key, 3)), $row->$kol_pk, $value, $kol_class, $js);
					}
					
				} elseif (startwith($key, "pwd")) {
					if ($kol_edit == 0) {
						$ret .= "<td></td>";
					} else {
						$ret .= sprintf("<td><input type='password' name='%s_%d'></td>", substr($key, 3), $row->$kol_pk);
					}
				} elseif (substr($key, 0, 3) == "nm2" or $kol_type == "num2") {
					$ret .= sprintf("<td class='number'>%s</td>", number_format(floatval($value), 2, ",", "."));
				} elseif ($kol_type == "mrg" or $kol_type == "combitext") {
					$ret .= sprintf("<td%s>%s</td>", $kol_class, str_replace(" &amp; ", "<br>", htmlentities($value, ENT_HTML5)));

				} elseif ($kol_type == "laatsteMutatie_Naam" and isset($kol['table']) and $value > 0) {
					$tas = $kol['tas'] ?? -1;
					$ret .= sprintf("<td>%s</td>", laatstemutatie($kol['table'], $value, $tas, "<br>"));

				} elseif ($kol_type == "subqry") {
					$ret .= sprintf("<td>%s</td>", $value);
					
				} else {
					// tekstkolom
					if (strlen($value) > $kol_ml) {
						$ret .= sprintf("<td%s title='%s'>%s</td>", $kol_class, $value, htmlentities(substr($value, 0, ($kol_ml-4)) . " ..."));
					} else {
						$ret .= sprintf("<td%s%s>%s</td>", $kol_class, $row_title, htmlentities($value));
					}
				}
				$kn++;
			}
			$ret .= "</tr>\n";
			$totaantal++;
		}
		$ret .= "</tbody>\n";
		if ($disptot > 0 and $totaantal > 1) {
			$ret .= "<tfoot>\n<tr>";
			for ($kn=0; $kn < count($ktot); $kn++) {
				if ($kn+1 == $disptot and $totaantal > 1) {
					$ret .= sprintf("<th colspan=2>%s rijen</th>", number_format($totaantal, 0, ",", "."));
					$kn++;
				} elseif (gettype($ktot[$kn]) == "double" and $ktot[$kn] <> 0) {
					$ret .= sprintf("<th class='number'>%s</th>", number_format($ktot[$kn], 2, ",", "."));
				} elseif (gettype($ktot[$kn]) == "integer" and $ktot[$kn] <> 0) {
					$ret .= sprintf("<th class='number'>%s</th>", number_format($ktot[$kn], 0, ",", "."));
				} else {
					$ret .= "<th></th>";
				}
			}
			$ret .= "</tr>\n</tfoot>\n";
		}
		$ret .= "</table>\n";
		if (strlen($xtra_regel) > 0) {
			$ret .= sprintf("<p>%s</p>\n", $xtra_regel);
		}
	} else {
		$ret .= "<p class='mededeling'>Er zijn geen records beschikbaar.</p>\n";
	}

	return $ret;
	
} # fnDisplayTable

function fnEditTable($res, $kols, $id="", $th="") {
	global $dtfmt;
	
	$rows = $res->fetchAll();
	
	$ret = "";
	$xtra_script = "";
	
	if (strlen($id) == 0 and strlen($th) > 0) {
		$id = str_replace("'", "", str_replace(" ", "_", strtolower($th)));
	}
//	print_r($rows);
//	debug($kols);

	$ret = sprintf("<table id='%1\$s' class='%2\$s'>\n", $id, TABLECLASSES);
	if (strlen($th) > 0) {
		if (count($rows) > 3) {
			$t = sprintf(" title='%d rijen'", count($rows));
		} else {
			$t = "";
		}
		$ret .= sprintf("<caption%s>%s</caption>\n", $t, $th);
	}
	$ret .= "<thead>\n";
	$ret .= "<tr>";
	$colpk = $res->getColumnMeta(0)['name'];
	foreach ($kols as $kn => $kol) {
		if (!isset($kols[$kn]['headertext'])) {
			$kols[$kn]['headertext'] = $kols[$kn]['columnname'];
		}
		if (!isset($kols[$kn]['type']) or strlen($kols[$kn]['type']) == 0) {
			$kols[$kn]['type'] = "text";
		}
		if (isset($kol['link']) and strlen($kol['link']) > 1) {
			$kols[$kn]['type'] = "link";
		}
		
		if (($kols[$kn]['type'] == "text" or $kols[$kn]['type'] == "email") and !isset($kols[$kn]['collen']) and count($rows) > 0) {
			for ($cn=0;$cn<$res->ColumnCount();$cn++) {
				if ($kols[$kn]['columnname'] ==  $res->getColumnMeta($cn)['name']) {
					$kols[$kn]['collen'] = intval($res->getColumnMeta($cn)['len'] / 3);		
				}
			}
		}
		if (isset($kol['type']) and $kol['type'] == "pk") {
			$colpk = $kols[$kn]['columnname'];
		}
			
		if (isset($kol['autosubmit']) and $kol['autosubmit'] == true and isset($kols[$kn]['columnname']) and 1 == 2) {
			$xtra_script = sprintf("$(\"[id^=%s]\").on('blur', function() {
			location.reload(true);
			});\n", $kols[$kn]['columnname']);
			}
		if (isset($kols[$kn]['headertext']) and strlen($kols[$kn]['headertext']) > 0) {
			$ret .= sprintf("<th>%s</th>", $kols[$kn]['headertext']);
		}
	}
	$ret .= "</tr>\n";
	$ret .= "</thead>\n";
		
	$ret .= "<tbody>\n";
	$dispkop = 1;
	$totaantal = 0;
	foreach($rows as $row) {
		$ret .= "<tr>";
		$valpk = $row->$colpk;
		foreach ($kols as $key => $kol) {
			$tp = $kol['type'] ?? "text";
			$kol_class = "";
			if (isset($kol['class'])) {
				$kol_class = sprintf("class='%s'", $kol['class']);
			}
			$kol_title = "";
			if (isset($kol['title']) and strlen($kol['title']) > 0) {
				$kol_title = sprintf(" title=\"%s\"", str_replace("\"", "'", $kol['title']));
			}
			$ro = $kol['readonly'] ?? false;
			if (isset($kol['cond_ro']) and strlen($kol['cond_ro']) > 0) {
				$v = $kol['cond_ro'];
				if ($row->$v > 0) {
					$ro = true;
				}
			}
			if (isset($kol['link']) and strlen($kol['link']) > 0) {
				$tp = "link";
				if ($ro and $kol['class'] == "trash") {
					$kol_link = "";
				} elseif (substr($kol['link'], 0, 7) == "<a href") {
					$kol_link = $kol['link'];
				} else {
					$kol_link = "<a href='" . $kol['link'] . "'";
					if (isset($kol['title'])) {
						if (isset($kol['columntitle']) and strpos($kol['title'], "%") !== false) {
							$t = sprintf($kol['title'], $row[$kol['columntitle']]);
						} else {
							$t = $kol['title'];
						}
						$kol_link .= sprintf(" title='%s'", $t);
					}
					if (isset($kol['class']) and $kol['class'] == "toevoegen") {
						$kol_link .= sprintf(">%s</a>", ICONTOEVOEGEN);
					} elseif (isset($kol['class']) and $kol['class'] == "muteren") {
						$kol_link .= sprintf(">%s</a>", ICONMUTEER);
					} elseif (isset($kol['class']) and $kol['class'] == "trash") {
						$kol_link .= sprintf(">%s</a>", ICONVERWIJDER);
					} elseif (isset($kol['class']) and $kol['class'] == "pdf") {
						$kol_link .= sprintf(">%s</a>", ICONPDF);
					} elseif (isset($kol['class']) and $kol['class'] == "leden") {
						$kol_link .= sprintf(">%s</a>", ICONLEDEN);
					} elseif (isset($kol['class']) and $kol['class'] == "verwerk") {
						$kol_link .= sprintf(">%s</a>", ICONVERWERK);
					} else {
						$kol_link .= ">&nbsp;&nbsp;&nbsp;</a>";
					}
				}
			} elseif (isset($kol['onclick']) and strlen($kol['onclick']) > 0) {
				$tp = "click";
			}
			$colname = $kol['columnname'];
			$value = $row->$colname;
			
			$col2 = $kol['secondcolumn'] ?? "";
			$value2 = $row->$col2 ?? "";
		
			if (strlen($kol['headertext']) == 0) {
				// Kolom wordt niet getoond.

			} elseif ($tp == "link") {
				if ($value > 0 and strlen($kol_link) > 0) {
					$href = str_replace("%d", $value, $kol_link);
					$ret .= sprintf("<td>%s</td>", $href);
				} else {
					$ret .= "<td>&nbsp;</td>";
				}
				
			} elseif ($tp == "click") {
				if ($value > 0) {
					$oc = str_replace("%d", $value, $kol['onclick']);
					if ($kol['class'] == "trash") {
						$t = ICONVERWIJDER;
					} elseif ($kol['class'] == "mislukt") {
						$t = ICONMISLUKT;
					} else {
						$t = "&nbsp;";
					}
					$ret .= sprintf("<td class='%1\$s' id='%1\$s_%2\$d' onClick='%3\$s'>%4\$s</td>", $kol['class'], $value, $oc, $t);
				} else {
					$ret .= "<td>&nbsp;</td>";
				}
			
			} elseif ($tp == "checkbox") {
				$ret .= "<td class='checkbox'>";
				if ($ro) {
					if ($value == 1) {
						$ret .= "Ja</td>";
					} else {
						$ret .= "Nee</td>";
					}
				} else {
					$ret .= sprintf("<input type='checkbox' class='form-check-input' id='%s_%d' value=1%s></td>", $colname, $valpk, checked($value));
				}

			} elseif (isset($kol['bronselect']) and is_array($kol['bronselect']) and count($kol['bronselect']) > 0) {
				$opt = "";
				foreach ($kol['bronselect'] as $k => $v) {
					$opt .= sprintf("<option%s value=%s>%s</option>\n", checked($k, "option", $value), $k, $v);
				}
				
				$ret .= sprintf("<td><select id='%s_%d' class='form-select form-select-sm %s'>\n%s</select></td>\n", $colname, $valpk, $kol['class'] ?? "", $opt);
				
			} elseif (isset($kol['bronselect']) and strlen($kol['bronselect']) > 0) {
					
				$colnamebron = $kol['bronselect'];
				if (strlen($row->$colnamebron) > 0) {
					$opt = "";
					foreach (explode(";", $row->$colnamebron) as $v) {
						$opt .= sprintf("<option value='%s'%s>%s</option>\n", str_replace(",", ".", trim($v)), checked($v, "option", $value), $v);
					}
					$ret .= sprintf("<td><select id='%s_%d' class='form-select form-select-sm %s'>\n%s</select></td>\n", $colname, $valpk, $kol['class'] ?? "", $opt);
				} else {
					$ret .= "<td></td>\n";
				}

			} elseif ($tp == "groep" and isset($kol['columnfilter'])) {
				$o = $row->{$kol['columnfilter']};
				if ((new cls_Groep())->aantal(sprintf("OnderdeelID=%d", $o)) > 1) {
					$opt = (new cls_Groep())->htmloptions($value, $o);
					$ret .= sprintf("<td><select id='%s_%d' class='form-select form-select-sm'>\n%s</select></td>\n", $colname, $valpk, $opt);
				} else {
					$ret .= "<td></td>";
				}

			} elseif ($tp == "DTSHORT" and $ro) {
				if ((!is_null($value)) and $value > '1900-01-01') {
					$ret .= sprintf("<td>%s</td>", date("d/m/Y", strtotime($value)));
				} else {
					$ret .= "<td>&nbsp;</td>";
				}
					
			} elseif ($tp == "date" or $tp == "rqdate" or $tp == "DTTEXT") {
				if ($ro == false) {
					$rq = "";
					if ($tp == "rqdate") {
						$rq = " required";
					}
					$ret .= sprintf("<td><input type='date' name='%1\$s_%2\$d' id='%1\$s_%2\$d' value='%3\$s'%4\$s></td>", $colname, $valpk, $value, $rq);
				} elseif ((!is_null($value)) and $value > '1900-01-01') {
					$dtfmt->setPattern(DTTEXT);
					$ret .= sprintf("<td>%s</td>", $dtfmt->format(strtotime($value)));
				} else {
					$ret .= "<td>&nbsp;</td>";
				}
				
			} elseif ($tp == "geboren_leeftijd") {
					$dtfmt->setPattern(DTTEXT);
					if (strlen($value) == 10) {
						$ret .= sprintf("<td>%s (%s)</td>", $dtfmt->format(strtotime($value)), fnLeeftijd($value));
					} else {
						$ret .= "<td></td>";
					}
				
			} elseif ($tp == "datetime" or $tp == "DTLONG") {
				$dtfmt->setPattern(DTLONG); // 1 januari 2020 (14:12)
				if ($ro == false) {
					// Nog aanpassen, klopt niet
					$ret .= sprintf("<td><input type='date' id='%s_%d' value='%s'></td>", $colname, $valpk, $value);
					
				} elseif ((!is_null($value)) and $value > '1900-01-01') {
					$ret .= sprintf("<td>%s</td>", $dtfmt->format(strtotime($value)));
				} else {
					$ret .= "<td>&nbsp;</td>";
				}

			} elseif ($tp == "tijd") {
				if ($ro) {
					$ret .= sprintf("<td>%s</td>", $value);
				} else {
					$ret .= sprintf("<td><input type='time' id='%s_%d' value='%s'></td>", $colname, $valpk, $value);
				}

			} elseif ($tp == "integer" or $tp == "pk") {
				if ($ro) {
					$ret .= sprintf("<td class='number'>%d</td>", $value);
					
				} else {
					if (isset($kol['max']) and $kol['max'] <= 99) {
						$c = "num2";
						$m = sprintf(" max=%d", $kol['max']);
					} elseif (isset($kol['max']) and $kol['max'] <= 999) {
						$c = "num3";
						$m = sprintf(" max=%d", $kol['max']);
					} else {
						$c = "w10";
						$m = "";
					}
					$ret .= sprintf("<td><input type='number' id='%s_%d' value=%d class='%s'%s%s></td>", $colname, $valpk, $value, $c, $m, $kol_title);
				}

			} elseif ($tp == "leeftijd") {
				if ($ro) {
					$ret .= sprintf("<td class='number'>%d</td>", $value);
				} else {
					$ret .= sprintf("<td><input type='number' id='%s_%d' value=%d class='num2' min=0 max=99></td>", $colname, $valpk, $value);
				}
					
			} elseif ($tp == "dagen") {
				if ($ro) {
					if ($value == 1) {
						$ret .= "<td class='number'>1 dag</td>";
					} else {
						$ret .= sprintf("<td class='number'>%d dagen</td>", $value);
					}
				} else {
					$ret .= sprintf("<td><input type='number' id='%s_%d' value=%d class='num2' min=0 max=999>dagen</td>", $colname, $valpk, $value);
				}

			} elseif ($tp == "bedrag") {
				if ($ro) {
					$ret .= sprintf("<td class='number'>%.2F</td>", $value);
				} else {
					$ret .= sprintf("<td><input type='text' id='%s_%d' value='%.2F' class='d8'%s></td>", $colname, $valpk, $value, $kol_title);
				}				
					
			} elseif ($tp == "email") {
				if ($ro) {
					$ret .= sprintf("<td>%s</td>", fnDispEmail($value, "", 1));
				} else {
					if (strlen($kol_class) == 0 and $kol['collen'] > 0) {
						$kol_class = sprintf("class='w%d'", $kol['collen']);
					}
					$ret .= sprintf("<td><input type='email' %s id='%s_%d' value='%s' maxlength=%d></td>", $kol_class, $colname, $valpk, $value, $kol['collen']);
				}
					
			} elseif ($tp == "pwd") {
				if ($kol_edit == 0) {
					$ret .= "<td></td>";
				} else {
					$ret .= sprintf("<td><input type='password' name='%s_%d'></td>", $colname, $valpk);
				}

			} else {
				// tekst
				if ($ro) {
					if (strlen($col2) > 0) {
						if (isset($kol['secondcolumntype']) and $kol['secondcolumntype'] == "geboren_leeftijd") {	
							$dtfmt->setPattern(DTTEXT);
							if (strlen($row->$col2) == 10) {
								$value2 = $dtfmt->format(strtotime($row->$col2));
								if ($row->$col2 <= date("Y-m-d", strtotime("-1 year"))) {
									$value2 .= sprintf(" (%s)", fnLeeftijd($row->$col2));
								}
							} else {
								$value2 = "";
							}
						} else {
							$value2 = htmlentities($row->$col2);
						}
						$ret .= sprintf("<td id='%s_%d'>%s<br>%s</td>", $colname, $valpk, htmlentities($value), $value2);
					} else {
						$ret .= sprintf("<td id='%s_%d'>%s</td>", $colname, $valpk, htmlentities($value));
						}
				} else {
					if (strlen($kol_class) == 0 and $kol['collen'] > 0) {
						$kol_class = sprintf("class='w%d'", $kol['collen']);
					}
					$ret .= sprintf("<td><input type='text' id='%s_%d' value=\"%s\" %s maxlength=%d%s>", $colname, $valpk, $value, $kol_class, $kol['collen'], $kol_title);
					if (strlen($col2) > 0) {
						if (isset($kol['secondcolumntype']) and $kol['secondcolumntype'] == "date") {
							$ret .= sprintf("<br><input type='date' id='%s_%d' value=\"%s\">", $col2, $valpk, $value2);
						} else {
							$ret .= sprintf("<br><input type='text' id='%s_%d' value=\"%s\">", $col2, $valpk, $value2);
						}
					}
					$ret .= "</td>";
				}
			}
		}
		$ret .= "</tr>\n";
	}
	$ret .= "</tbody>\n";
	$ret .= "</table>\n";
		
	$ret .= sprintf("<script>
	
	$('select').on('change', function() {
		savedata('%1\$s', 0, this);
	});

	$('input').on('blur', function() {
		savedata('%1\$s', 0, this);
	});
	
	$(\"input[type='number']\").on('change', function() {
		savedata('%1\$s', 0, this);
	});
		
	%2\$s

	</script>\n", $id, $xtra_script);

	return $ret;
	
} # fnEditTable

function fnStandaardKols($p_ent, $p_sort=1, $rows=null) {
	
	if ($p_ent == "logboek" or $p_ent == "logboekafd") {
		$kols = null;
		$kols[0] = array('columnname' => "DatumTijd", 'headertext' => "Datum en tijd", 'type' => "DTLONGSEC");
		if ($p_sort == 1) {
			$kols[0]['sortcolumn'] = "RecordID";
		}
		$kols[1] = array('columnname' => "Omschrijving", 'maxlength' => 250);
		if ($p_sort == 1) {
			$kols[1]['sortcolumn'] = "Omschrijving";
		}
		
		if ($rows != null and count($rows) > 0 and isset($rows[0]->betreftLid) and strlen(max(array_column($rows, "betreftLid"))) > 0) {
			$kols[2] = ['headertext' => "Betreft lid", 'columnname' => "betreftLid", 'type' => "lidnaam"];
		}
		
		if ($p_ent != "logboekafd" and $rows != null and count($rows) > 0 and isset($rows[0]->betreftOnderdeel) and strlen(max(array_column($rows, "betreftOnderdeel"))) > 0) {
			$kols[3] = ['headertext' => "Onderdeel", 'columnname' => "betreftOnderdeel", 'type' => "onderdeelnaam"];
		}
		
		if (WEBMASTER) {
			$kols[4] = ['headertext' => "Type", 'columnname' => "Type"];
			if ($p_sort == 1) {
				$kols[4]['sortcolumn'] = "Type";
			}
			$kols[5] = ['headertext' => "Ingelogd lid", 'columnname' => "ingelogdLid"];
			if ($p_sort == 1) {
				$kols[5]['sortcolumn'] = "ingelogdLid";
			}
			$kols[5]['type'] = "lidnaam";
		}
		if ($rows != null and count($rows) > 0 and isset($rows[0]->Getoond) and strlen(max(array_column($rows, "Getoond"))) > 0) {
			$kols[6] = ['headertext' => "Getoond?", 'columnname' => "Getoond", 'type' => "text"];
		}
		if (WEBMASTER) {
			$kols[7] = array('headertext' => "Script / functie", 'columnname' => "scriptFunctie", 'maxlength' => 150);
			if ($p_sort == 1) {
				$kols[7]['sortcolumn'] = "scriptFunctie";
			}
			/*
			$kols[8] = ['headertext' => "IP adres", 'columnname' => "IP_adres"];
			if ($p_sort == 1) {
				$kols[8]['sortcolumn'] = "A.IP_adres";
			}
			*/
		}
	}
	
	return $kols;
	
}  # fnStandaardKols

function fnOrderBy($p_arrSort) {
	$ord = "";
	
	$_SESSION['huidsort'] = $_GET['sort'] ?? -1;
		
	if ($_SESSION['huidsort'] >= 50) {
		$sk = $_SESSION['huidsort'] - 50;
	} else {
		$sk = $_SESSION['huidsort'];
	}
	
	$sw = "";
	if ($sk >= 0 and isset($p_arrSort[$sk])) {
		if (isset($p_arrSort[$sk]['sortcolumn'])) {
			$sw = $p_arrSort[$sk]['sortcolumn'];
		}
	}
	
	if (strlen($sw) > 0 and (strpos($sw, " ") !== false or strpos($sw, "-") !== false)) {
		if (strpos($sw, ",") !== false) {
			$ord = $sw;
		} elseif (strpos($sw, ".") !== false) {
			$ord = explode(".", $sw)[0] . ".`" . explode(".", $sw)[1] . "`";
		} else {
			$ord = "`" . $sw . "`";
		}
	} else {
		$ord = $sw;
	}
	
	if ($_SESSION['huidsort'] >= 50 and strlen($ord) > 0) {
		if (isset($p_arrSort[$sk]['sortcolumndesc'])) {
			$ord = $p_arrSort[$sk]['sortcolumndesc'];
		} else {
			$ord .= " DESC";
		}
	}
	
	return $ord;

}  # fnOrderBy

function debug($tekst, $p_tm=2, $logact=0, $p_lengte=0) {
	
	/*
	$p_tm
		* 0: nooit tonen
		* 1: aan iedereen tonen
		* 2: alleen tonen aan webmasters
		* 3: aan iedereen als alert (popup) tonen
	*/
	
	if (is_array($tekst) or is_object($tekst) and $p_tm > 0) {
		$tekst = print_r($tekst, true);
	} elseif ($p_lengte == 1) {
		$tekst .= sprintf(" (%d)", strlen($tekst));
	}
	
	if ($_SERVER["HTTP_HOST"] == "phprbm.telling.nl" and $p_tm == 2) {
		$p_tm = 1;
	}
	
	$f = "";
	if ($_SERVER["HTTP_HOST"] == "phprbm.telling.nl" or WEBMASTER) {
		$bt = debug_backtrace(DEBUG_BACKTRACE_PROVIDE_OBJECT, 2);
		for ($t=count($bt)-1;$t >= 0;$t--) {
			if (strlen($f) > 0) {
				$f .= "=>";
			}
			if (isset($bt[$t]['class']) and strlen($bt[$t]['class']) > 0) {
				$f .= $bt[$t]['class'];
			}
			if (isset($bt[$t]['file']) and strlen($bt[$t]['file']) > 0) {
				$f .= str_replace(BASEDIR, "", $bt[$t]['file']);
				if (isset($bt[$t]['line'])) {
					$f .= sprintf(" (%d)", $bt[$t]['line']);
				}
			}
		}
		$f .= ":";
	}
	
	if ($p_tm == 3) {
		printf("<script>alert('Debug: %s');</script>\n", $tekst);
	} elseif ($p_tm == 1 or ($p_tm == 2 and WEBMASTER)) {
		if (WEBMASTER == false) {
			$et = ". Neem contact op met de webmaster om dit op te laten lossen.";
		} else {
			$et = "";
		}
		printf("<p class='debug'>%s %s%s</p>\n", $f, $tekst, $et);
	}

	if ($logact >= 1) {
		if ($logact == 1) {
			(new cls_Logboek())->add("Debug: " . $tekst, 99);
		} else {
			(new cls_Logboek())->add("Debug: " . $tekst, $logact);
		}
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
	if (strlen($haystack) < strlen($start)) {
		return false;
	} elseif (substr($haystack, 0, strlen($start)) == $start) {
		return true;
	} else {
		return false;
	}
} # startwith

function endswith($haystack, $end) {
    $length = strlen($end);
	if (strlen($haystack) == 0) {
		 return false;
    } elseif ($length == 0) {
		return true;
    } else {
		return (substr($haystack, -$length) === $end);
	}
} # endswith

function change_month_to_uk($strdate) {
	
	debug(__FUNCTION__ . ": evalueren", 2, 99);

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
	
	debug("change_month_to_uk", 1, 1);
	
	return $strdate;
} # change_month_to_uk

function removetextblock($content, $start_block, $end_block="") {
	
	if (strlen($end_block) == 0 and strlen($start_block) > 5) {
		$end_block = str_replace("<!-- ", "<!-- /", $start_block);
	}

	while (strlen($start_block) > 5 and strlen($end_block) > 5 and strpos($content, $start_block) !== false and strpos($content, $end_block) !== false) {
		$stb = strpos($content, $start_block);
		$etb = strpos($content, $end_block) + strlen($end_block);
		$content = substr($content, 0, $stb) . substr($content, $etb);
	}
	return $content;
	
} # removetextblock

function checked($val, $tp="checkbox", $sv=1) {
	if ($tp == "option" and $val == $sv) {
		return " selected";
	} elseif ($tp == "checkbox" and (intval($val) == 1 or $val === true or (strlen($val) >= 10 and $val > "1970-01-01"))) {
		return " checked";
	} elseif ($tp == "checkbox" and $val == $sv) {
		return " checked";
	} else {
		return "";
	}
} #checked

function htmlent($text) {
	return htmlentities($text, ENT_COMPAT,'ISO-8859-1', true);
} # htmlent

function fnExtBestand($p_fn) {
	
	$p_fn = str_replace("!", ".", $p_fn);
	$hv_ext = explode(".", $p_fn);
	return $hv_ext[count($hv_ext)-1];
	
}  # extbestand

function pv($p_control, $p_ookget=1) {
	if (isset($_POST[$p_control])) {
		return $_POST[$p_control];
	} elseif ($p_ookget == 1 and isset($_GET[$p_control])) {
		return $_GET[$p_control];
	} else {
		return "";
	}
}  # pv

function aantaluniekeleden($rows, $veldnm) {
	
	$aant = 0;
	$vw = -1;
	
	$columns = array_column($rows, $veldnm);
	array_multisort($columns, SORT_ASC, $rows);
	
	foreach ($rows as $row) {
		if ($vw != $row->$veldnm) {
			$aant++;
		}
		$vw = $row->$veldnm;
	}
	
	return $aant;
	
}  # aantaluniekeleden

function fnControleTelefoon($tn, $srt="telefoon") {
	
	if (strlen(trim($tn)) == 0) {
		$rv = "";
	} elseif (substr($tn, 2, 1) == "-" and strlen($tn) == 11 and $srt == "mobiel") {
		$rv = "";
	} elseif (substr($tn, 3, 1) == "-" and strlen($tn) == 11 and $srt == "telefoon") {
		$rv = "";
	} elseif (substr($tn, 4, 1) == "-" and strlen($tn) == 11 and $srt == "telefoon") {
		$rv = "";
	} elseif (strlen($tn) == 10 and strstr($tn, "-") === false) {
		$rv = "";
	} elseif ($srt == "telefoon") {
		$rv = "Formaat telefoonnummer is niet correct.";
	} else {
		$rv = "Formaat mobiele nummer is niet correct.";
	}
	
	return $rv;
	
}  # fnControleTelefoon

function fnControleEmail($p_email) {

	if (strlen($p_email) == 0 or strpos($p_email, ",") !== false) {
		$rv = "";
	} elseif (isValidMailAddress($p_email) == false) {
		$rv = "Het formaat van het e-mailadres is niet correct.";
	} else {
		$domein = explode("@", $p_email)[1];
		if (checkdnsrr($domein, "MX") == false) {
			$rv = "Het domein van dit e-mailadres bestaat niet of kan geen e-mail ontvangen.";
		} else {
			$rv = "";
		}
	}
	
	return $rv;
	
}  # fnControleEmail

function DisplayTabs($tabblad, $inittab=0) {
		
	echo("<div id='tabs'>\n");
	echo("<ul>\n");
	$tn = 1;
	foreach ($tabblad as $nm=>$inhoud) {
		printf("<li><a href='#tab-%s'>%s</a></li>\n", str_replace("'", "", strtolower($nm)), $nm);
		$tn++;
	}
	echo("</ul>\n");
		
	$tn = 1;
	foreach ($tabblad as $nm=>$inhoud) {
		printf("<div id='tab-%1\$s'>\n%2\$s</div> <!-- Einde tab-%1\$s -->\n", str_replace("'", "", strtolower($nm)), $inhoud);
		$tn++;
	}
	
	echo("</div> <!-- Einde tabs -->\n");
	
	printf("<script>
		$(document).ready(function() {
			$( function() {
				$('#tabs').tabs({
					active: %d
				});
			});
		});
	</script>\n", $inittab);	
	
}  # DisplayTabs

function fnOptionsFromArray($p_array, $p_cv=-1, $p_geenwaarde="") {
	
	if (strlen($p_geenwaarde) > 0) {
		$rv = sprintf("<option value=0%s>%s</option>\n", checked($p_cv, "option", 0), $p_geenwaarde);
	} else {
		$rv = "";
	}
	
	foreach ($p_array as $k => $v) {
		$rv .= sprintf("<option value='%s'%s>%s</option>\n", $k, checked($p_cv, "option", $k), $v);
	}
	
	return $rv;
	
}  # fnOptionsFromArray

function validTime($time, $format='H:i') {
    $d = DateTime::createFromFormat("Y-m-d " . $format, "2022-12-01 " . $time);
    return $d && $d->format($format) == $time;
}  # validTime

function fnLeeftijd($p_gd, $p_per="", $p_incl_jr=1) {
	if (strlen($p_per) < 10) {
		$p_per = date("Y-m-d");
	}
	
	if (strlen($p_gd) == 10) {
		$rv = intval(substr($p_per, 0, 4) - substr($p_gd, 0, 4));
		if (substr($p_per, 5, 2) < substr($p_gd, 5, 2)) {
			$rv--;
		} elseif (substr($p_per, 5, 2) == substr($p_gd, 5, 2) and substr($p_per, 8, 2) < substr($p_gd, 8, 2)) {
			$rv--;
		}
	
		if ($p_incl_jr == 1) {
			$rv = sprintf("%d&nbsp;jaar", $rv);
		}
	} else {
		$rv = false;
	}
	
	return $rv;
}  # fnLeeftijd

function laatstemutatie($p_table, $p_rid, $p_tas=-1, $p_glue="<br>") {
	global $dtfmt;

//	debug(__FUNCTION__ . ": evalueren", 2, 99);
	
	$rv = "";
	$i_lb = new cls_logboek();
	$i_lb->where = sprintf("A.LidID > 0 AND A.RefTable='%s' AND ReferID=%d", $p_table, $p_rid);
	if ($p_tas > 0) {
		$i_lb->where .= sprintf(" AND A.TypeActiviteitSpecifiek=%d", $p_tas);
	}
//	debug($i_lb->where);
	$dtfmt->setPattern(DTLONG);
	$lbrow = $i_lb->lijst(-1, 0, 0, "", "A.DatumTijd DESC", 1, 0)->fetch();
	if (isset($lbrow->DatumTijd)) {
		$rv = sprintf("%s%s%s", (new cls_Lid())->naam($lbrow->ingelogdLid), $p_glue, $dtfmt->format(strtotime($lbrow->DatumTijd)));
	} else {
		
	}
	$i_lb = null;
	
	return $rv;
}  # laatstemutatie

function cleanipaddr($p_ipaddr) {
	$rv = "";
	
	if (strpos($p_ipaddr, ":") > 0) {
		// IP V6
		
		$dpt = 0;
		$l = 15;
		for ($i=0;$i<20;$i++) {
			if (substr($p_ipaddr, $i, 1) == ":") {
				$dpt++;
				if ($dpt == 4) {
					$l = $i;
				}
			}
		}
		
		$rv = substr($p_ipaddr, 0, $l);

	} elseif (strpos($p_ipaddr, ".") > 0) {
		// IP V4
		$rv = $p_ipaddr;
	}
	
	return $rv;
}  # cleanipaddr

?>
