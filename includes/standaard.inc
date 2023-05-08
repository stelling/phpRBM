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

if (isset($_SERVER['HTTPS'])){
	$basisurl = "https://" . $_SERVER['HTTP_HOST'];
} else {
	$basisurl = "http://" . $_SERVER['HTTP_HOST'];
}

$sub = substr($_SERVER['PHP_SELF'], 0, strripos($_SERVER['PHP_SELF'], "/"));
if (strlen($sub) > 1) {
	$basisurl .= $sub;
}
define("BASISURL", $basisurl);

define("ARRGESLACHT", ["O" => "Onbekend", "V" => "Vrouwelijk", "M" => "Mannelijk", "N" => "Neutraal", "B" => "Bedrijf/Vereniging/Stichting"]);
define("ARRTL", ["Alle", "Leden", "Klosleden", "Voormalig leden"]);
define("ARRLEGITIMATIE", ["G" => "Geen", "I" => "Identiteitskaart", "P" => "Paspoort", "R" => "Rijbewijs"]);
define("ARRSOORTMEMO", ["A" => "Algemeen", "B" => "Bewaking", "D" => "Dieet", "E" => "Examen", "F" => "Financiën", "G" => "Gezondheid/medisch", "I" => "Inschrijving bewaking", "Z" => "Zwemervaring"]);
define("ARRTYPEONDERDEEL", ["A" => "Afdeling", "B" => "Bestuur", "C" => "Commissie", "E" => "Eigenschap", "F" => "Functionarissen", "G" => "Groep", "O" => "Onderscheiding", "R" => "Rol", "S" => "Selectie", "T" => "Toestemming"]);
define("ARRTYPEDIPLOMA", ["B" => "Brevet", "C" => "Competentie", "D" => "Diploma", "I" => "Insigne", "L" => "Licentie", "M" => "Module", "P" => "PvB"]);
define("PASFOTOEXTENTIES", ["gif", "jpg", "jpeg", "png"]);
define("ARRSOORTEVENEMENT", ["B" => "Brigade-activiteit", "G" => "Gezellig/sociaal", "O" => "Opleiding", "V" => "Vergadering/overleg", "W" => "Wedstrijd"]);
define("ARRDLNSTATUS", ["A" => "Afgewezen", "B" => "Bevestigd", "I" => "Ingeschreven", "J" => "Aangemeld", "N" => "Niet geweest zonder afmelding", "R" => "Reserve", "T" => "Onder voorbehoud", "X" => "Afgemeld/Afgezegd"]);
define("ARRPRESENTIESTATUS", ["A" => "Aanwezig", "H" => "Aangemeld, door vereniging afgezegd", "J" => "Aangemeld", "L" => "Te laat aanwezig", "N" => "Niet aanwezig (onafgemeld)", "R" => "Afwezig met bekende reden", "T" => "Onder voorbehoud", "V" => "Les is vervallen", "X" => "Afgemeld zonder bekende reden", "Z" => "Ziek"]);
define("ARRTYPEBACKUP", [1 => "phpRBM-tabellen", 2 => "Tabellen uit MS-Access", 3 => "Alle tabellen"]);
define("ARRTYPESTUK", ["A" => "Algemene Vergadering", "B" => "Beleidsstuk", "F" => "Formulier", "H" => "Handleiding", "P" => "Promotiemateriaal", "R" => "Regeling", "V" => "Verklaring"]);
define("ARRTYPEMENU", [1 => "per niveau een aparte regel", 2 => "&eacute;&eacute;n menu met dropdown", 3 => "&eacute;&eacute;n menu met dropdown en extra menu voor niveau 2"]);
define("BEWAARTIJDNIEUWERECORDS", 120);

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

require(BASEDIR . '/config.php');
define("LIDIDWEBMASTERS", $lididwebmasters);
$bestandsversie = filectime(BASEDIR . "/index.php");
if (strlen($table_prefix) > 0 and substr($table_prefix, -1) != "_") {
	$table_prefix .= "_";
}
define("TABLE_PREFIX", $table_prefix);

if (isset($httpsverplicht) and $httpsverplicht == 1 and !isset($_SERVER['HTTPS'])) {
	header(sprintf("location: https:%s", $_SERVER['HTTP_HOST']));
}

session_cache_expire(25);
if (isset($_SESSION['lidid'])) {
	// Sessie bestaat al
} elseif (session_start() === FALSE or strlen(session_id()) == 0) {
	$mess = "De sessie kan niet gestart worden, hierdoor kan er op dit moment niet ingelogd worden.";
	(new cls_Logboek())->add($mess, 1, 0, 1);
}

if ((isset($_SESSION['webmaster']) and $_SESSION['webmaster'] == 1) or strpos(BASISURL, "telling.nl") > 0) {
	error_reporting(E_ALL);
} else {
	error_reporting(0);
//	error_reporting(E_ALL);
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
if (isset($_GET['tp']) and mb_substr_count($_GET['tp'], "/") == 2) {
	$currenttab = explode("/", $_GET['tp'])[0];
	$currenttab2 = explode("/", $_GET['tp'])[1];
	$currenttab3 = explode("/", $_GET['tp'])[2];
	
} elseif (isset($_GET['tp']) and mb_substr_count($_GET['tp'], "/") == 1) {
	$currenttab = explode("/", $_GET['tp'])[0];
	$currenttab2 = explode("/", $_GET['tp'])[1];
	
} elseif (isset($_GET['tp']) and strlen($_GET['tp']) > 1) {
	$currenttab = $_GET['tp'];
	
} else {
	if (substr($_SERVER['PHP_SELF'], -9) == "admin.php") {
		$currenttab = "Beheer logins";
	} else {
		$currenttab = "Vereniging";
	}
}

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
/* Einde includen open source libs*/

foreach (array("database", "authentication", "afdelingen", "leden", "evenement", "financieel", "mailing", "diploma", "standaard", "webshop", "taken") as $incl) {
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

$i_p = new cls_Parameter();

if (!isset($_SESSION['settings']['naamvereniging'])) {
	$i_p->vulsessie();
}
if ($bestandsversie != $_SESSION['settings']['versie']) {
	debug(sprintf("Nieuwe versie: %s", $dtfmt->format(time())), 0);
	$i_p->controle();
	db_createtables();
	db_onderhoud(2);
	$i_p->update("versie", $bestandsversie);
	$i_p->vulsessie();
}
$i_p = null;

if ($_SESSION['lidid'] > 0) {
	(new cls_Login())->setlastactivity();
}
$dtfmt->setPattern(DTTEXT);
$fileverinfo = 'templates/verenigingsinfo.html';

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
	if (strlen($currenttab2) > 0) {
		$title .= " | " . $currenttab2;
	}
	if (strlen($currenttab3) > 0) {
		$title .= " | " . $currenttab3;
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
		
		echo("<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css' rel='stylesheet'>\n");
		echo("<link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap-icons@1.10.4/font/bootstrap-icons.css'>\n");
		
//		echo("<link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap-icons@1.8.1/font/bootstrap-icons.css'>\n");
//		echo("<link rel='stylesheet' href='//cdnjs.cloudflare.com/ajax/libs/timepicker/1.3.5/jquery.timepicker.min.css'>\n");
		echo("<script src='https://code.jquery.com/jquery-3.6.0.js'></script>\n");
		if ($currenttab == "Vereniging" or $currenttab == "Eigen gegevens"  or $currenttab == "Ledenlijst" or $currenttab2 == "Overzicht lid") {
			echo("<script src='https://code.jquery.com/ui/1.13.1/jquery-ui.js'></script>\n");
		}

		if ($currenttab == "Mailing") {
			$src = "/includes/tinymce/js/tinymce/tinymce.min.js";
			if (file_exists(BASEDIR . $src)) {
				$src = BASISURL . $src;
			} else {
			$src = sprintf("https://cdn.tiny.cloud/1/%s/tinymce/6/tinymce.min.js", $_SESSION['settings']['mailing_tinymce_apikey']);
			}
			printf("<script src='%s' referrerpolicy='origin'></script>\n", $src);
		}
		
		if (file_exists("default.css")) {
			printf("<link rel='stylesheet' href='default.css?v%d'>\n", filectime("default.css"));
		}
		if (file_exists("maatwerk/kleur.css")) {
			printf("<link rel='stylesheet' href='maatwerk/kleur.css?v%d'>\n", filectime("maatwerk/kleur.css"));
		}
		
		echo("</head>\n");
		echo("<body>\n");
		if ($breed == 0) {
			echo("<div class='container-xl'>\n");
		} else {
			echo("<div class='container-fluid'>\n");
		}
		echo("<header>\n");
		
		if (strlen($_SESSION['settings']['url_eigen_help']) < 10) {
			$l = "https://software.telling.nl/phpRBM/uitleg.php";
		} else {
			$l = $_SESSION['settings']['url_eigen_help'];
		}
		$htmlhelp = sprintf("<a href='%s'> <i class='bi bi-question-circle' style='font-size: 16pt;' title='Helpfunctie'></i></a>\n", $l);
	
		echo("<div id='welkom'>\n");
		
		if ($_SESSION['lidid'] > 0) {
			printf("<p id='welkomuser'>Welkom %s</p>\n", htmlentities($_SESSION['roepnaamingelogde']));
			echo("<a href='/?tp=Zelfservice/Wijzigen+wachtwoord'> <i class='bi bi-person-circle' style='font-size: 16pt;' title='Wijzigen wachtwoord'></i></a>\n");
			echo("<a href='index.php?actie=uitloggen'> <i class='bi bi-box-arrow-left' style='font-size: 16pt;' title='Uitloggen'></i></a>\n");
			if ($_SESSION['webmaster'] == 1) {
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
		
		printf("<h1>%s</h1>\n", $_SESSION['settings']['naamwebsite']);
//		echo("<div class='clear'></div>\n");
		
		$xtra = "";
		if ($currenttab == "Overzicht lid" and isset($_GET['lidid']) and $_GET['lidid'] > 0) {
			$xtra = sprintf("lidid=%d", $_GET['lidid']);
		}
		
		echo("</header>\n");
		
		fnDispMenu(1, $xtra);
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

function fnDispMenu_nw($toonnivo=1, $xtra="") {
	global $tabpages, $currenttab, $currenttab2, $currenttab3, $navpad;
	
	if ($toonnivo == 1) {
		echo("<nav>\n");
	}
	$tpd = ""; // Dit wordt getoond in het menu.
	$ulopen = false;

	if (strlen($xtra) > 0 and startwith($xtra, "&amp;") == false) {
		$xtra = "&" . $xtra;
	}
	
	$nivo = $toonnivo;
	printf("<ul id='menu_l%d'>\n", $toonnivo);
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
			} elseif ($nivo == 3 and ($currenttab3 == str_replace("'", "", $tpd) or strlen($currenttab3) == 0)) {
				$cl = " class='current'";
				$navpad[$nivo]['naam'] = $tpd;
				$navpad[$nivo]['url'] = sprintf("%s?tp=%s%s", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra);
			}
			if ($itp == 5) {
				echo("<li class='dropdown'>&nbsp;\n");
				echo("<ul>\n");
			}
			printf("<li%s><a href=\"%s?tp=%s%s\">%s</a></li>\n", $cl, $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra, $tpd);
			$itp++;
		}
	}
	if ($itp > 5) {
		echo("</ul> <!-- Einde dropdown -->\n");
		echo("</li>\n");
	}
	echo("</ul>\n");
	
	if ($toonnivo == 1 and strlen($currenttab2) == 0) {
		echo("</nav>\n");
	} elseif ($toonnivo == 2 and strlen($currenttab3) == 0) {
		echo("</nav>\n");
	} elseif ($toonnivo == 3) {
		echo("</nav>\n");
	}
}  # fnDispMenu

function fnDispMenu($toonnivo=1, $xtra="") {
	global $tabpages, $currenttab, $currenttab2, $currenttab3, $navpad;
	
	if ($toonnivo == 1) {
		echo("<nav>\n");
	}
	$tpd = ""; // Dit wordt getoond in het menu.
	$ulopen = false;
	$_SESSION['settings']['typemenu'] = 1;

	if (strlen($xtra) > 0 and startwith($xtra, "&amp;") == false) {
		$xtra = "&" . $xtra;
	}
	
	$nivo = $toonnivo;
	if ($_SESSION['settings']['typemenu'] == 1 or ($_SESSION['settings']['typemenu'] == 3 and $nivo == 2)) {
		printf("<div id='menu_l%d'>\n", $toonnivo);
		$itp = 1;
		foreach($tabpages as $tp) {
			if ($nivo == 1 and strstr($tp, "/") === false) {
				$tpd = $tp;
			} elseif ($nivo == 2 and mb_substr_count($tp, "/") == 1 and startwith($tp, $currenttab . "/")) {
				$tpd = str_replace($currenttab . "/", "", $tp);
			
			} elseif ($nivo == 3 and mb_substr_count($tp, "/") == 2 and startwith($tp, $currenttab . "/") and strpos($tp, $currenttab2) > 1) {
				$tpd = str_replace($currenttab2 . "/", "", str_replace($currenttab . "/", "", $tp));
				
//			} elseif ($currenttab2 == "Overzicht lid") {
				
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
				} elseif ($nivo == 3 and ($currenttab3 == str_replace("'", "", $tpd) or strlen($currenttab3) == 0)) {
					$cl = " class='current'";
					$navpad[$nivo]['naam'] = $tpd;
					$navpad[$nivo]['url'] = sprintf("%s?tp=%s%s", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra);
				}
				printf("<a href=\"%s?tp=%s%s\"><p%s>%s</p></a>\n", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra, $cl, $tpd);
				$itp++;
			}
		}
		echo("</div>\n");
	}
	
	if ($toonnivo == 1 and strlen($currenttab2) == 0) {
		echo("</nav>\n");
	} elseif ($toonnivo == 2 and strlen($currenttab3) == 0) {
		echo("</nav>\n");
	} elseif ($toonnivo == 3) {
		echo("</nav>\n");
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

function fnDisplayForm($row, $xtra_eind="", $id="formulier") {
	global $dtfmt;
	
	debug("fnDisplayForm", 1, 1);

	if (strlen($id) > 0) {
		$ret = sprintf("<div id='%s'>\n", $id);
	}
	if (isset($row) and $row != null) {
		$ret .= "<table>\n";
		foreach($row as $key => $value) {
			if (substr($key, 0, 3) == "Kop") {
				$ret .= sprintf("<tr><th colspan=2>%s</th></tr>\n", $value);
			} elseif (substr($key, 0, 3) == "dte" and (!is_null($value))) {
				if (strtotime($value) == false) {
					$dv = $value;
				} else {
					$dtfmt->setPattern(DTTEXT);
					$dv = $dtfmt->format(strtotime($value));
				}
				$ret .= sprintf("<tr><td class='label'>%s</td><td class='datum'>%s</td></tr>\n", substr($key, 3), $dv);
			} elseif (substr($key, 0, 3) == "dtt" and (!is_null($value))) {
				$dtfmt->setPattern(DTLONG);
				$ret .= sprintf("<tr><td class='label'>%s</td><td class='datumtijd'>%s</td></tr>\n", substr($key, 3), $dtfmt->format(strtotime($value)));
			} elseif (substr($key, 0, 3) == "boo" and $value == 1) {
				$ret .= sprintf("<tr><td class='label'>%s</td><td>Ja</td></tr>\n", substr($key, 3));
			} elseif (substr($key, 0, 3) == "boo" and $value != 1) {
				$ret .= sprintf("<tr><td class='label'>%s</td><td>Nee</td></tr>\n", substr($key, 3));
			} elseif (substr($key, 0, 3) == "mem" and strlen($value) > 1) {
				$ret .= sprintf("<tr><td colspan=2>%s</td></tr\n>", htmlentities($value));
			} elseif (substr($key, 0, 3) == "eml" and strlen($value) > 1) {
				$ret .= sprintf("<tr><td class='label'>%s</td><td>%s</td></tr>\n", substr($key, 3), fnDispEmail($value, "", 1));
			} elseif (substr($key, 0, 2) != "nd" and strlen($value) > 0) {
				$ret .= sprintf("<tr><td class='label'>%s</td><td>%s</td></tr>\n", $key, str_replace("\n", "<br>\n", htmlentities($value)));
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
}  # fnDisplayForm

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
				$ret .= sprintf("<label>%s</label><p>%s</p>\n", substr($key, 3), $dv);
			} elseif (substr($key, 0, 3) == "dtt" and (!is_null($value))) {
				$dtfmt->setPattern(DTLONG);
				$ret .= sprintf("<label>%s<p>%s</p>\n", substr($key, 3), $dtfmt->format(strtotime($value)));
			} elseif (substr($key, 0, 3) == "boo" and $value == 1) {
				$ret .= sprintf("<label>%s</label><p>Ja</p>\n", substr($key, 3));
			} elseif (substr($key, 0, 3) == "boo" and $value != 1) {
				$ret .= sprintf("<label>%s</label><p>Nee</p>\n", substr($key, 3));
			} elseif (substr($key, 0, 3) == "mem" and strlen($value) > 1) {
				$ret .= sprintf("<value>%s</value>\n", htmlentities($value));
			} elseif (substr($key, 0, 3) == "eml" and strlen($value) > 1) {
				$ret .= sprintf("<label>%s</label><p>%s</p>\n", substr($key, 3), fnDispEmail($value, "", 1));
			} elseif (substr($key, 0, 2) != "nd" and strlen($value) > 0) {
				$ret .= sprintf("<label>%s</label><p>%s</p>\n", $key, htmlentities($value));
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

function fnDisplayTable($rows, $kols=null, $th="", $disptot=0, $xtra_regel="", $p_id="", $xtra_kolom="", $p_selected=-1) {
	global $currenttab, $currenttab2, $dtfmt;
	
	$ret = "";
	$kol_pk = "";
	if (isset($rows) and (count($rows) > 0 or strlen($xtra_regel) > 0)) {
		if (strlen($p_id) > 0) {
			$ret .= sprintf("<table id='%s' class='table table-hover'>\n", $p_id);
		} else {
			$ret .= "<table class='table table-hover'>\n";
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
			$kols = fnStandaardKols($kols);
		}
			
		if ($kols == null) {
			$kn = 0;
			$skn = 0;
			$hv = "";
			foreach($rows[0] as $key => $value) {
				$kols[$kn]['columnname'] = $key;
				$sortlink = "";
				$ht = "";
				if (strstr("cbo, chk, dte, dtk, dtt, dtl, dts, boo, int, cur, eml, num, mrg, pwd, lnm", substr($key, 0, 3)) !== false) {
					$ht = str_replace("_", " ", substr($key, 3));
					
				} elseif (isset($kols[$kn]['link'])) {
					$hv .= "<th>&nbsp;</th>";
					$ktot[$kn] = "geen";

				} elseif (startwith($key, "llk") and isset($kols[-1]['link'])) {
					$hv .= "<th>&nbsp;</th>";
					$ktot[$kn] = "geen";
						
				} elseif (isset($kols[$kn]['skip']) and $kols[$kn]['skip'] == true) {
					// Deze kolom moet worden overgeslagen.
					
				} elseif (isset($kols[$kn]['headertext'])) {
					$hv .= sprintf("<th>%s</th>", $kols[$kn]['headertext']);
					
				} elseif (startwith($key, "lek")) {
					if (strlen($xtra_kolom) > 1) {
						$hv .= "<th>&nbsp;</th>";
						$ktot[$kn] = "geen";
					}
				} elseif (substr($key, 0, 2) != "nd") {
					$ht = str_replace("_", " ", $key);
				}
					
				if (empty($sortkols) == false) {
					$skn = -1;
					foreach ($sortkols as $k => $v) {
						if (startwith($v, $ht)) {
							$skn = $k;
						}
					}
					if (isset($_SESSION['huidsort']) and $_SESSION['huidsort'] < 50 and $_SESSION['huidsort'] == $skn and $skn >= 0) {
						$skn += 50;
					}
					if ($skn >= 0) {
						$sortlink = sprintf("%s?tp=%s&sort=%d", $_SERVER["PHP_SELF"], $currenttab, $skn);
					}
				}
					
				if (strlen($ht) > 0 and strlen($sortlink) > 0) {
					$hv .= sprintf("<th><a href='%s'>%s</a></th>", $sortlink, $ht);
				} elseif (strlen($ht) > 0) {
					$hv .= sprintf("<th>%s</th>", $ht);
				}
					
				if (startwith($key, "cur")) {
					$ktot[$kn] = 0.00;
				} elseif (substr($key, 0, 3) == "int") {
					$ktot[$kn] = 0;
				} elseif ($key == "RecordID") {
					$ktot[$kn] = 0;
				} elseif (substr($key, 0, 2) != "nd" and substr($key, 0, 3) != "lnk" and substr($key, 0, 3) != "llk") {
					$ktot[$kn] = "geen";
				}
				if (strlen($kol_pk) == 0 and ($key == "RecordID" or startwith($key, "lnk") or startwith($key, "llk"))) {
					$kol_pk = $key;
				}
				$kn++;
			}
			
			$ret .= $hv;
			
		} else {
			
			ksort($kols);
		
			foreach ($kols as $k => $kol) {
				$iq = false;
				if (isset($kol['headertext']) or isset($kol['link'])) {
					if (isset($kol['subqry']) and strlen($kol['subqry']) > 6) {
						$iq = true;
					} elseif (isset($kol['columnname']) and strlen($kol['columnname']) > 0) {
						foreach($rows[0] as $key => $val) {
							if ($key == $kol['columnname']) {
								$iq = true;
							}
						}
					} else {
						$iq = true;
						$kn = 0;
						foreach($rows[0] as $key => $val) {
							if ($k == $kn) {
								$kols[$k]['columnname'] = $key;
							}
							$kn++;
						}
					}
				}
				
				if ($iq) {
					if (!isset($kol['headertext'])) {
						$kols[$k]['headertext'] = "&nbsp;";
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
						$ret .= sprintf("<th><a href='%s'>%s</a></th>", $sortlink, $kols[$k]['headertext']);
					} else {
						$ret .= sprintf("<th>%s</th>", $kols[$k]['headertext']);
					}
					if (isset($kol['type']) and $kol['type'] == "pk") {
						$kol_pk = $kol['columnname'];
					}
//					debug($k . ": " . $kols[$k]['headertext'] . ": " . $kols[$k]['columnname']);
				} else {
					$kols[$k]['skip'] = true;
				}
				$ktot[$k] = 0;
			}
		}
		$ret .= "</tr>\n</thead>\n<tbody>\n";
		
		foreach($rows as $row) {
			$kn = 0;
			if ($p_selected > 0 and $p_selected == $row->$kol_pk) {
				$ret .= "<tr class='selected'>";
			} else {
				$ret .= "<tr>";
			}
			foreach ($kols as $kn => $kol) {
				if (isset($kol['columnname'])) {
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
				$kol_link = "";
				$kol_class = "";
				if (isset($kols[$kn]['edit'])) {
					$kol_edit = $kols[$kn]['edit'];
				}
				
				if ($kol_edit == 1) {
					debug("Edit in display");
				}
				
				if (isset($kols[$kn]['type'])) {
					$kol_type = $kols[$kn]['type'];
				} elseif (strlen($key) >= 3 and strstr("cbo, chk, dte, dtk, dtt, dtl, dts, boo, int, cur, eml, num, mrg, pwd, lnm", substr($key, 0, 3)) !== false) {
					$kol_type = substr($key, 0, 3);
				} else {
					$kol_type = "text";
				}
				
				if (isset($kols[$kn]['link'])) {
					$kol_type = "link";
					if (startwith($kols[$kn]['link'], "<a href") == true) {
						$kol_link = $kols[$kn]['link'];
					} else {
						$kol_link = "<a href='" . $kols[$kn]['link'] . "'";
						if (isset($kols[$kn]['title'])) {
							$kol_link .= sprintf(" title='%s'", $kols[$kn]['title']);
						}
						$kol_link .= ">";
						if (isset($kols[$kn]['class']) and $kols[$kn]['class'] === "details") {
							$kol_link .= "<i class='bi bi-file-text'></i></a>";
							
						} elseif (isset($kols[$kn]['class']) and $kols[$kn]['class'] === "detailregels") {
							$kol_link .= "<i class='bi bi-chevron-double-down'></i></a>";
							
						} elseif (isset($kols[$kn]['class']) and $kols[$kn]['class'] === "detailslid") {
							$kol_link .= "<i class='bi bi-person-lines-fill'></i></a>";
							
						} elseif (isset($kols[$kn]['class']) and $kols[$kn]['class'] === "pdf") {
							$kol_link .= "<i class='bi bi-file-earmark-pdf'></i></a>";
							
						} elseif (isset($kols[$kn]['class']) and $kols[$kn]['class'] == "leden") {
							$kol_link .= "<i class='bi bi-people'></i></a>";
							
						} elseif (isset($kols[$kn]['class']) and $kols[$kn]['class'] == "muteren") {
							$kol_link .= "<i class='bi bi-pencil-square' alt='Muteren'></i></a>";
							
						} elseif (isset($kols[$kn]['class']) and $kols[$kn]['class'] == "viewmail") {
							$kol_link .= "<i class='bi bi-file-text' alt='Bekijk e-mail'></i></a>";
							
						} elseif (isset($kols[$kn]['class']) and $kols[$kn]['class'] == "trash") {
							$kol_link .= "<i class='bi bi-trash' als='Verwijderen'></i></a>";
							
						} elseif (isset($kols[$kn]['class']) and $kols[$kn]['class'] == "unlock") {
							$kol_link .= "<i class='bi bi-unlock' alt='Account vrijgeven'></i></a>";
							
						} else {
							$kol_link .= "&nbsp;&nbsp;&nbsp;</a>";
							if (isset($kols[$kn]['class']) and strlen($kols[$kn]['class']) > 0) {
								$kol_class = sprintf("class='%s'", $kols[$kn]['class']);
							}
						}
					}
				} elseif (isset($kols[$kn]['class']) and strlen($kols[$kn]['class']) > 0) {
					$kol_class = sprintf("class='%s'", $kols[$kn]['class']);
				}
				
				if ($kol['skip'] == true) {
					// Deze kolom moet worden overgeslagen.
						
				} elseif ($kol_type == "chk") {
					if ($kol_edit == 0) {
						if ($value == 1) {
							$ret .= "<td>Ja</td>";
						} else {
							$ret .= "<td>Nee</td>";
						}
					} else {
						$ret .= sprintf("<td><input type='checkbox'%s name='%s_%d' value=1%s></td>", $js, str_replace(" ", "_", substr($key, 3)), $row->$kol_pk, checked($value, "", 1));
					}
					
				} elseif ($key == "cboFunctie") {
					debug("fnDisplayTable/cboFunctie", 2, 1);
					$ret .= "<td>";
					if ($rowscbo != null) {
						$ret .= sprintf("<select name='%s_%d' %s>", substr($key, 3), $row->$kol_pk, $js);
						foreach ($rowscbo as $rowcbo) {
							if ($rowcbo->Vervallen >= $row->dteVanaf) {
								$ret .= sprintf("<option value=%d %s>%s</option>", $rowcbo->Nummer, checked($rowcbo->Nummer, "option", $value), $rowcbo->Omschrijv);
							}
						}
						$ret.= "</select>";
					}
					$ret .= "</td>\n";

				} elseif ($kol_type == "dte" or $kol_type == "rodte" or $kol_type == "date" or $kol_type == "DTTEXT") {
					// 1 januari 2020
					if ((!is_null($value)) and $value > '1900-01-01' and strtotime($value) !== false) {
						$dtfmt->setPattern(DTTEXT);
						$ret .= sprintf("<td>%s</td>", $dtfmt->format(strtotime($value)));
					} elseif ((!is_null($value)) and strlen($value) > 0) {
						$ret .= sprintf("<td>%s</td>", $value);
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
					
				} elseif ($kol_type == "dtk" or $kol_type == "dateshort") {
					if ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td>%s</td>", date('d/m/Y', strtotime($value)));
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
				} elseif ($kol_type == "dtl" or $kol_type == "DTTEXTWD") {
					$dtfmt->setPattern(DTTEXTWD);
					if ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td>%s</td>", $dtfmt->format(strtotime($value)));
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
				} elseif ($kol_type == "dtt" or $kol_type == "rodtt" or $kol_type == "datetime" or $kol_type == "DTLONG") {
					$dtfmt->setPattern(DTLONG);
					if ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td>%s</td>", $dtfmt->format(strtotime($value)));
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
					
				} elseif ($kol_type == "dts" or $kol_type == "datetimesecond" or $kol_type == "DTLONGSEC") {
					if ((!is_null($value)) and $value > '1900-01-01' and strtotime($value) !== false) {
						$dtfmt->setPattern(DTLONGSEC);
						$ret .= sprintf("<td>%s</td>", $dtfmt->format(strtotime($value)));
					} elseif (strlen($value) > 0) {
						$ret .= sprintf("<td>%s</td>", $value);
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
					
				} elseif ($kol_type == "DTHMS")	{
					if ((!is_null($value)) and $value > '1900-01-01' and strtotime($value) !== false) {
						$dtfmt->setPattern(DTHMS);
						$ret .= sprintf("<td>%s</td>", $dtfmt->format(strtotime($value)));
					} elseif (strlen($value) > 0) {
						$ret .= sprintf("<td>%s</td>", $value);
					} else {
						$ret .= "<td>&nbsp;</td>";
					}				
					
				} elseif ($kol_type == "int" or $kol_type == "integer") {
					if ($kol_edit == 0) {
						$ret .= sprintf("<td class='number'>%d</td>", $value);
					} else {
						$ret .= sprintf("<td><input type='number' name='%s_%d' value='%s'></td>", substr($key, 3), $row->$kol_pk, $value);
					}
					$ktot[$kn] += intval($value);
					
				} elseif ($kol_type == "cur" or $kol_type == "currency" or $kol_type == "bedrag") {
					$ret .= sprintf("<td class='number'>%s</td>", number_format($value, 2, ",", "."));
					$ktot[$kn] += $value;

				} elseif ($key == "Getoond") {
					if ($value == 0) {
						$ret .= "<td>Nee</td>";
					} else {
						$ret .= "<td>Ja</td>";
					}

				} elseif ($kol_type == "boo" or $kol_type == "boolean" or $kol_type == "checkbox") {
					if ($value == 1) {
						$ret .= "<td>Ja</td>";
					} elseif ($value == 0 or $value == -1) {
						$ret .= "<td>Nee</td>";
					} else {
						$ret .= "<td>Onbekend</td>";
					}
					
				} elseif (($kol_type == "lnm" or $kol_type == "lidnaam") and strlen($value) > 0) {
					if (is_numeric($value)) {
						$ret .= sprintf("<td>%s</td>", (new cls_lid())->Naam($value, "onbekend"));
					} else {
						$ret .= sprintf("<td>%s</td>", $value);
					}
					
				} elseif (strlen($kol_link) > 1) {
					if ($value > 0) {
						$href = str_replace("%d", $value, $kol_link);
						$href = str_replace("%s", "&nbsp;&nbsp;&nbsp;", $href);
						$ret .= sprintf("<td %s>%s</td>", $kol_class, $href);
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
						
				} elseif ($kol_type == "link") {
					if ($value > 0 and isset($kols[-1]['link'])) {
						if (startwith($kols[-1]['link'], "<a href") == true) {
							$kol_link = $kols[-1]['link'];
						} else {
							$kol_link = "<a href='" . $kols[-1]['link'] . "'>&nbsp;&nbsp;&nbsp;</a>";
						}
						$href = str_replace("%d", $value, $kol_link);
						$kol_class = "";
						if (isset($kols[-1]['class'])) {
							$kol_class = sprintf("class='%s'", $kols[-1]['class']);
						}
						$ret .= sprintf("<td %s>%s</td>", $kol_class, $href);
					} elseif (isset($kols[-1]['link'])) {
						$ret .= "<td>&nbsp;</td>\n";
					}
					
				} elseif ($kol_type == "eml" or $kol_type == "email") {
					if ($kol_edit == 0) {
						$ret .= sprintf("<td>%s</td>", fnDispEmail($value, "", 1));
					} else {
						$ret .= sprintf("<td><input type='email' name='%s_%d' value='%s' class='email' %s></td>", str_replace(" ", "_", substr($key, 3)), $row->$kol_pk, $value, $js);
					}
					
				} elseif (startwith($key, "pwd")) {
					if ($kol_edit == 0) {
						$ret .= "<td></td>";
					} else {
						$ret .= sprintf("<td><input type='password' name='%s_%d'></td>", substr($key, 3), $row->$kol_pk);
					}
				} elseif (substr($key, 0, 3) === "num") {
					$ret .= sprintf("<td class='number'>%s</td>", $value);
				} elseif (substr($key, 0, 3) == "nm2" or $kol_type == "num2") {
					$ret .= sprintf("<td class='number'>%s</td>", number_format(floatval($value), 2, ",", "."));
				} elseif ($kol_type == "mrg" or $kol_type == "combitext") {
					$ret .= sprintf("<td>%s</td>", str_replace(" &amp; ", "<br>", htmlentities($value, ENT_HTML5)));
					
				} elseif ($kol_type == "subqry") {
					$ret .= sprintf("<td>%s</td>", $value);
					
				} else {
					if ($kol_edit == 0 or $row->$kol_pk == 0) {
						$ret .= sprintf("<td>%s</td>", htmlentities($value));
					} else {
						$ret .= sprintf("<td><input name='%s_%d' value='%s' class='%s'%s></td>", $key, $row->$kol_pk, $value, str_replace(" ", "", strtolower($key)), $js);
					}
				}
				$kn++;
			}
			$ret .= "</tr>\n";
			$totaantal++;
		}
		if (strlen($xtra_regel) > 0) {
			$ret .= sprintf("<tr><td colspan=%d>%s</td></tr>\n", count($kols), $xtra_regel);
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
	} else {
		$ret .= "<p class='mededeling'>Er zijn geen records beschikbaar.</p>\n";
	}
	
	return $ret;
	
} # fnDisplayTable

function fnEditTable($res, $kols, $id, $th) {
	global $dtfmt;

	$rows = $res->fetchAll();
	
	$ret = sprintf("<table id='%s' class='table table-hover'>\n", $id);
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
		if (count($rows) > 0) {
			if (!isset($kol['columnname'])) {
				$kols[$kn]['columnname'] = $res->getColumnMeta($kn)['name'];
			}
			if (!isset($kols[$kn]['type']) or strlen($kols[$kn]['type']) == 0) {
				$kols[$kn]['type'] = "text";
			}
			if (isset($kol['link']) and strlen($kol['link']) > 1) {
				$kols[$kn]['type'] = "link";
			}
			if ($kols[$kn]['type'] == "text") {
				for ($cn=0;$cn<$res->ColumnCount();$cn++) {
					if ($kols[$kn]['columnname'] ==  $res->getColumnMeta($cn)['name']) {
						$kols[$kn]['collen'] = intval($res->getColumnMeta($cn)['len'] / 3);		
					}
				}
			}
			if (isset($kol['type']) and $kol['type'] == "pk") {
				$colpk = $kols[$kn]['columnname'];
			}
		}
		if (isset($kol['headertext']) and strlen($kol['headertext']) > 0) {
			$ret .= sprintf("<th>%s</th>", $kol['headertext']);
		}
	}
	
//	debug($kols);
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
			if (isset($kol['link']) and strlen($kol['link']) > 0) {
				$tp = "link";
				if (substr($kol['link'], 0, 7) == "<a href") {
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
					if (isset($kol['class']) and $kol['class'] == "muteren") {
						$kol_link .= "><i class='bi bi-pencil-square' alt='Muteren'></i></a>";
					} elseif (isset($kol['class']) and $kol['class'] == "trash") {
						$kol_link .= "><i class='bi bi-trash' alt='Verwijderen'></i></a>";
					} else {
						$kol_link .= ">&nbsp;&nbsp;&nbsp;</a>";
					}
				}
			}
			$ro = $kol['readonly'] ?? false;
			if (isset($kol['cond_ro']) and strlen($kol['cond_ro']) > 0) {
				$v = $kol['cond_ro'];
				if ($row->$v > 0) {
					$ro = true;
				}
			}
			$colname = $kol['columnname'];
			$value = $row->$colname;
			$col2 = $kol['secondcolumn'] ?? "";
			$value2 = $row->$col2 ?? "";
			
			if (!isset($kol['headertext']) or strlen($kol['headertext']) == 0) {
				// Kolom wordt niet getoond.
			
			} elseif ($tp == "link") {
				if ($value > 0) {
					$href = str_replace("%d", $value, $kol_link);
					$ret .= sprintf("<td>%s</td>", $href);
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
					$ret .= sprintf("<input type='checkbox' id='%s_%d' value=1%s></td>", $colname, $valpk, checked($value));
				}
				
			} elseif (isset($kol['bronselect']) and count($kol['bronselect']) > 0) {
				$opt = "";
				foreach ($kol['bronselect'] as $k => $v) {
					$opt .= sprintf("<option%s value=%s>%s</option>\n", checked($k, "option", $value), $k, $v);
				}
				$ret .= sprintf("<td><select id='%s_%d'>\n%s</select></td>\n", $colname, $valpk, $opt);

			} elseif ($tp == "date" or $tp == "rqdate" or $tp == "DTTEXT") {
				if ($ro == false) {
					$rq = "";
					if ($tp == "rqdate") {
						$rq = " required";
					}
					$ret .= sprintf("<td><input type='date' id='%s_%d' value='%s'%s></td>", $colname, $valpk, $value, $rq);
				} elseif ((!is_null($value)) and $value > '1900-01-01') {
					$dtfmt->setPattern(DTTEXT);
					$ret .= sprintf("<td>%s</td>", $dtfmt->format(strtotime($value)));
				} else {
					$ret .= "<td>&nbsp;</td>";
				}
				
			} elseif ($tp == "geboren_leeftijd") {
					$dtfmt->setPattern(DTTEXT);
					$ret .= sprintf("<td>%s (%s)</td>", $dtfmt->format(strtotime($value)), fnLeeftijd($value));
				
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
					$ret .= sprintf("<td><input type='number' id='%s_%d' value=%d class='%s'%s></td>", $colname, $valpk, $value, $c, $m);
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
					$ret .= sprintf("<td><input type='text' id='%s_%d' value='%.2F' class='d8'></td>", $colname, $valpk, $value);
				}				
				
			} elseif ($tp == "email") {
				if ($ro) {
					$ret .= sprintf("<td>%s</td>", fnDispEmail($value, "", 1));
				} else {
					$ret .= sprintf("<td><input type='email' id='%s_%d' value='%s'></td>", $colname, $valpk, $value);
				}
				
			} elseif ($tp == "pwd") {
				if ($kol_edit == 0) {
					$ret .= "<td></td>";
				} else {
					$ret .= sprintf("<td><input type='password' name='%s_%d'></td>", $colname, $valpk);
				}

			} else {
				if ($ro) {
					if (strlen($col2) > 0) {
						if (isset($kol['secondcolumntype']) and $kol['secondcolumntype'] == "geboren_leeftijd") {	
							$dtfmt->setPattern(DTTEXT);
							$value2 = sprintf("%s (%s)", $dtfmt->format(strtotime($row->$col2)), fnLeeftijd($row->$col2));
						} else {
							$value2 = htmlentities($row->$col2);
						}
						$ret .= sprintf("<td>%s<br>%s</td>", htmlentities($value), $value2);
					} else {
						$ret .= sprintf("<td>%s</td>", htmlentities($value));
					}
				} else {
					$xc = sprintf("w%d", $kol['collen']);
					$ret .= sprintf("<td><input type='text' id='%s_%d' value=\"%s\" class='%s' maxlength=%d>", $colname, $valpk, $value, $xc, $kol['collen']);
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
	
	$('select').change(function() {
		savedata('%1\$s', 0, this);
	});
	
	$('input').blur(function() {
		savedata('%1\$s', 0, this);
	});
	
	$(\"input[type='number']\").change(function() {
		savedata('%1\$s', 0, this);
	});

</script>\n", $id);

	return $ret;
	
} # fnEditTable

function fnStandaardKols($p_ent, $p_sort=1) {
	
	if ($p_ent == "logboek") {
		$kols = null;
		$kols[0]['headertext'] = "Datum en tijd";
		$kols[0]['type'] = "DTLONGSEC";
		if ($p_sort == 1) {
			$kols[0]['sortcolumn'] = "RecordID";
		}
		$kols[1]['headertext'] = "Omschrijving";
		if ($p_sort == 1) {
			$kols[1]['sortcolumn'] = "Omschrijving";
		}
		$kols[2]['headertext'] = "Betreft lid";
		$kols[2]['columnname'] = "betreftLid";
		$kols[2]['type'] = "lidnaam";
		if ($_SESSION['webmaster'] == 1) {
			$kols[3]['headertext'] = "Type";
			$kols[3]['columnname'] = "Type";
			if ($p_sort == 1) {
				$kols[3]['sortcolumn'] = "Type";
			}
			$kols[4]['headertext'] = "Ingelogd lid";
			$kols[4]['columnname'] = "ingelogdLid";
			if ($p_sort == 1) {
				$kols[4]['sortcolumn'] = "ingelogdLid";
			}
			$kols[4]['type'] = "lidnaam";
		}
		$kols[5]['headertext'] = "Getoond?";
		$kols[5]['columnname'] = "Getoond";
		$kols[5]['type'] = "boolean";
		if ($_SESSION['webmaster'] == 1) {
			$kols[6]['headertext'] = "Script / functie";
			$kols[6]['columnname'] = "scriptFunctie";
			if ($p_sort == 1) {
				$kols[6]['sortcolumn'] = "scriptFunctie";
			}
			$kols[7]['headertext'] = "IP adres";
			$kols[7]['columnname'] = "IP_adres";
			if ($p_sort == 1) {
				$kols[7]['sortcolumn'] = "A.IP_adres";
			}
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
		if (strpos($sw, ".") !== false) {
			$ord = explode(".", $sw)[0] . ".`" . explode(".", $sw)[1] . "`";
		} else {
			$ord = "`" . $sw . "`";
		}
	} else {
		$ord = $sw;
	}
	
	if ($_SESSION['huidsort'] >= 50 and strlen($ord) > 0) {
		$ord .= " DESC";
	}
	
	return $ord;

}  # fnOrderBy

function debug($tekst, $p_tm=2, $logact=0, $p_lengte=0) {
	
	/*
	$p_tm
		* 0: nooit tonen
		* 1: aan iedereen tonen
		* 2: alleen tonen aan webmasters
		* 3: aan iedereen als alert tonen
	*/
	
	if (is_array($tekst) or is_object($tekst) and $p_tm > 0) {
		$tekst = print_r($tekst, true);
	} elseif ($p_lengte == 1) {
		$tekst .= sprintf(" (%d)", strlen($tekst));
	}
	
	if ($_SERVER["HTTP_HOST"] == "phprbm.telling.nl" and $p_tm == 2) {
		$p_tm = 1;
	}
	
	$_SESSION['webmaster'] = $_SESSION['webmaster'] ?? 0;
	$f = "";
	if ($_SESSION['webmaster'] == 1 or $_SERVER["HTTP_HOST"] == "phprbm.telling.nl") {
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
	} elseif ($p_tm == 1 or ($p_tm == 2 and $_SESSION['webmaster'] == 1)) {
		if ($_SESSION['webmaster'] == 0) {
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

function removetextblock($content, $start_block="<!-- Ingelogd -->", $end_block="<!-- /Ingelogd -->") {

	while (strpos($content, $start_block) !== false and strpos($content, $end_block) !== false) {
		$stb = strpos($content, $start_block);
		$etb = strpos($content, $end_block) + strlen($end_block);
		$content = substr($content, 0, $stb) . substr($content, $etb);
	}
	return $content;
	
} # removetextblock

function checked($val, $tp="checkbox", $sv=1) {
	if ($tp === "option" and $val == $sv) {
		return " selected";
	} elseif ($tp === "checkbox" and (intval($val) == 1 or $val === true)) {
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
	
	asort($rows);
	
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
	
	$rv = intval(substr($p_per, 0, 4) - substr($p_gd, 0, 4));
	if (substr($p_per, 5, 2) < substr($p_gd, 5, 2)) {
		$rv--;
	} elseif (substr($p_per, 5, 2) == substr($p_gd, 5, 2) and substr($p_per, 8, 2) < substr($p_gd, 8, 2)) {
		$rv--;
	}
	
	if ($p_incl_jr == 1) {
		$rv = sprintf("%d&nbsp;jaar", $rv);
	}
	
	return $rv;
}  # fnLeeftijd

?>
