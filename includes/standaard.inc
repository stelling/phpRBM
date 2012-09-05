<?php
date_default_timezone_set('Europe/Amsterdam');
setlocale(LC_ALL,'nl_NL@euro');

if (session_start() === FALSE or strlen(session_id()) == 0) {
	$mess = "De sessie kan niet gestart worden, hierdoor kan er op dit moment niet gelogd worden.";
	db_logboek("add", $mess, 1, 0, 1);
} 

if ((isset($_SESSON['webmaster']) and $_SESSON['webmaster'] == 1) or $_SERVER["HTTP_HOST"] == "phprbm.telling.nl") {
	error_reporting(E_ALL);
} else {
	error_reporting(0);
}

if (!isset($_SESSION['lidid'])) {
	$_SESSION['lidid'] = 0;
}

$dir = substr(__FILE__, 0, strrpos(__FILE__, "/"));
$dir = substr($dir, 0, strrpos($dir, "/"));
require_once($dir . '/config.php');

$bestandsversie = filectime($dir . "/index.php");
$includes = array("database.inc", "authentication.inc", "leden.inc", "bewaking.inc", "evenement.inc", "mailing.inc", "webshop.inc", "class.phpmailer.php");
foreach ($includes as $incl) {
	$incl = $dir . "/includes/" . $incl;
	if ($bestandsversie < filectime($incl)) {
		$bestandsversie = filectime($incl);
	}
	require_once($incl);
}
if ($bestandsversie/100000 != db_param("versie")) {
	db_createtables();
	db_onderhoud();
	db_param("versie", "updval", $bestandsversie/100000);
}

// Vullen sessie met config variabelen
if (!isset($_SESSION['naamvereniging']) or strlen($_SESSION['naamvereniging']) < 3 or !isset($_SESSION['performance_trage_select'])) {
	$_SESSION['naamvereniging'] = db_param("naamvereniging");
	$_SESSION['urlvereniging'] = db_param("urlvereniging");
	$_SESSION['naamwebsite'] = db_param("naamwebsite");
	$_SESSION['urlwebsite'] = db_param("urlwebsite");
	$_SESSION['emailwebmaster'] = db_param("emailwebmaster");
	$_SESSION['emailsecretariaat'] = db_param("emailsecretariaat");
	$_SESSION['performance_trage_select'] = db_param("performance_trage_select");
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

$arrGeslacht["V"] = "Vrouw";
$arrGeslacht["M"] = "Man";
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
	$tabpages[] = "DB onderhoud";
	$tabpages[] = "Logboek";
} else {
	$tabpages[] = "Vereniging";
	$tabpages[] = "Vereniging/Introductie";
	$tabpages[] = "Vereniging/Kader";
	$tabpages[] = "Vereniging/Onderscheidingen";
	$tabpages[] = "Vereniging/Overige commissies";
	if (isset($_SESSION['lidid']) and $_SESSION['lidid'] > 0) {
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
		if (count(db_insbew("openblokken")) > 0) {
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
	$tabpages[] = "Zelfservice/Profiel";
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
	}
	if ($currenttab == "Wijzigen lid") {
		$tabpages[] = "Wijzigen lid";
		$tabpages[] = "Wijzigen lid/Algemene gegevens";
		$tabpages[] = "Wijzigen lid/Diploma's";
		$tabpages[] = "Wijzigen lid/Pasfoto";
		$tabpages[] = "Wijzigen lid/Bijzonderheden";
	}
	if ($_SESSION['aantalbewaking'] > 0) {
		$tabpages[] = "Bewaking";
		$tabpages[] = "Bewaking/Bewakingsrooster";
		$tabpages[] = "Bewaking/Postindeling";
		$tabpages[] = "Bewaking/Aantallen";
		$tabpages[] = "Bewaking/Nieuwe pasfoto";
		$tabpages[] = "Bewaking/Diploma's muteren";
		if (count(db_insbew("blokken")) > 0) {
			$tabpages[] = "Bewaking/Overzicht inschrijvingen";
		}
	}
	if (count(db_insbew("export")) > 0) {
		$tabpages[] = "Bewaking/Downloaden inschrijvingen";
	}
	if (db_logboek("aantal", "", 11) > 0) {
		$tabpages[] = "Bewaking/Logboek";
	}
	if ($_SESSION['aantalmutaties'] > 0) {
		$tabpages[] = "Kostenoverzicht";
	}
	$tabpages[] = "Mailing";
	$tabpages[] = "Mailing/Templates";
	$tabpages[] = "Mailing/Concepten";
	$tabpages[] = "Mailing/Nieuw";
	$tabpages[] = "Mailing/Verzonden";
	if (db_mailing("aantalprullenbak") > 0) {
		$tabpages[] = "Mailing/Prullenbak";
	}
	if (db_logboek("aantal", "", 4) > 0) {
		$tabpages[] = "Mailing/Logboek";
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
	$tabpages[] = "Bestellingen/Bestellingen muteren";
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
	
	if (strlen($currenttab) > 0 and strlen($currenttab2) > 0) {
		$title = $_SESSION['naamwebsite'] . " | " . $currenttab . " | " . $currenttab2;
	} elseif (strlen($currenttab) > 0) {
		$title = $_SESSION['naamwebsite'] . " | " . $currenttab;
	} else {
		$title = $_SESSION['naamwebsite'];
	}

//	header('Content-type: text/html; charset=utf-8');
   if (!isset($_GET['header']) or $_GET['header'] != "no") {
		echo("<!DOCTYPE HTML>\n");
		echo("<html lang='nl'>\n");
		echo("<head>\n");
		if (isset($_GET['tp']) and substr($_GET['tp'], 0, 7) == "Mailing") {
			echo("<script src='/includes/ckeditor/ckeditor.js'></script>\n");
		}
		if ($refresh == 1) {
			echo("<meta http-equiv='refresh' content='600'>\n");
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
		printf("<h1>%s</h1>\n", $_SESSION['naamwebsite']);
	
		echo("<div id='login'>\n");
		echo("<form name='Login' action='/auth.php' method='post'>");
		if (isset($_SESSION['username']) and strlen($_SESSION['username']) > 4 and $_SESSION['lidid'] > 0) {
			printf("Welkom %s\n", htmlentities($_SESSION['roepnaamingelogde']));
			echo("<a href='/?tp=Zelfservice/Profiel'><img src='images/user.png' title='Profiel wijzigen' alt='Profiel wijzigen'></a>\n");
			if ($_SERVER['PHP_SELF'] == "/index.php") {
				printf("<a href='auth.php?actie=uitloggen'><img src='images/power.png' title='Uitloggen' alt='Uitloggen'></a>\n");
			}
			if ($_SESSION['webmaster'] == 1) {
				if ($_SERVER['PHP_SELF'] != "/admin.php") {
					echo("<a href='admin.php'><img src='images/tool.png' title='Beheerdersmenu'></a>\n");
				} elseif ($_SERVER['PHP_SELF'] == "/admin.php") {
					echo("<a href='index.php'><img src='images/home.png' title='Beginpagina'></a>\n");
				}
			}
		} elseif ($_SESSION['aantallogin'] > 0) {
			echo("Login: <input type='text' name='username' size=20 maxlength=50>\n");
			echo("&nbsp;Wachtwoord: <input type='password' name='password' size=10 maxlength=12>\n");
			echo("&nbsp;<input type='submit' value='Inloggen' name='Inloggen'>\n");
			echo("<input type='checkbox' name='cookie' value=1 title='Automatisch inloggen'>\n");
		}
	
		echo("<a href='http://phprbm.telling.nl/uitleg.php'><img src='images/question.png' title='Uitleg gebruik en installatie'></a>\n");
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
}

function HTMLfooter() {
	global $table_prefix, $navpad, $bestandsversie;

	if (!isset($_GET['header']) or $_GET['header'] != "no") {
		if (strlen($_SESSION['naamvereniging']) > 2) {
			if (strlen($_SESSION['urlvereniging']) > 3) {
				$cr = sprintf("<a href='http://%s'>%s</a>", $_SESSION['urlvereniging'], $_SESSION['naamvereniging']);
			} else {
				$cr = $_SESSION['naamvereniging'];
			}
		} else {
			$cr = "<a href='http://www.telling.nl'>&copy;&nbsp;S.Telling</a>";
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
				 <a href='http://%s'>%s</a> %s\n
				 <div id='footerright'>%s</div>\n
				 </div>  <!-- Einde footer  -->\n", $cr, $_SESSION['urlwebsite'], $_SESSION['naamwebsite'], $np, strftime("%e %B %Y", $bestandsversie));
		echo("</div>  <!-- Einde container -->\n");
		echo("</body>\n");
		echo("</html>\n");
	}
}

function isValidMailAddress($address, $LeegIsGoed=0){

  	if ($LeegIsGoed == 1 and strlen($address) == 0) {
		return true;
	} elseif (strlen(trim($address)) < 5) {
		return false;
   } elseif(filter_var($address, FILTER_VALIDATE_EMAIL) === FALSE) {
		return false;
	} else {
		return true;
	}
}

function fnDispEmail($strEmail, $strNaam="", $booInclLink=1) {
	if (strpos($strEmail, "<") > 2 and strlen($strnaam) < 3) {
   	$strNaam = trim(substr(trim($strEmail), 0, strpos($strEmail, "<")));
		$strEmail = substr(trim($strEmail), strpos($strEmail, "<")+1, -1);
   }
	$strRet = "";
	if (isValidMailAddress($strEmail, 0)) {
		if (isset($_SESSION['lidid']) and $_SESSION['lidid'] > 0) {
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
	
	$typemenu = db_param("typemenu");
	$ulopen = false;

	if (strlen($xtra) > 0 and startwith($xtra, "&amp;") == false) {
		$xtra = "&amp;" . $xtra;
	}

	if (($typemenu == 2 or $typemenu == 3) and $toonnivo == 1) {  // Dit is het menu met hovering
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
	
	if ($typemenu == 1 or ($typemenu == 3 and $toonnivo == 2)) {
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
		if ($nivo > 1) {
			echo("<div class='clear'></div>\n");
		}
	}
}

function fnDisplayForm($row, $xtra_eind="", $class="formulier") {

	$ret = sprintf("<div id='%s'>\n", $class);
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
	$ret .= sprintf("</div>  <!-- Einde %s -->\n", $class);
	return $ret;
}

function fnDisplayTable($rows, $link="", $th="", $disptot=0, $link_lk="", $xtra_regel="", $class="lijst", $xtra_kolom="") {
	$ret = sprintf("<div class='%s'>\n", $class);
	if (isset($rows) and (count($rows) > 0 or strlen($xtra_regel) > 0)) {
		$ret .= "<table>\n";
		$dispkop = 1;
		$totaantal = 0;
		$hlk = "";
		foreach($rows as $row) {
			if ($dispkop == 1) {
				$hv = "<tr>";
				foreach($row as $key => $value) {
					if (strstr("dte, dtt, dtl, dts, boo, int, cur, eml, num, mrg", substr($key, 0, 3)) !== false) {
						$hv .= sprintf("<th>%s</th>", str_replace("_", " ", substr($key, 3)));
						$ktot[] = 0;
					} elseif (strstr("lnk", substr($key, 0, 3)) !== false) {
						$hv .= "<th>&nbsp;</th>";
						$ktot[] = 0;
					} elseif (strstr("llk", substr($key, 0, 3)) !== false) {
						if (strlen($link_lk) > 1) {
							$hlk .= sprintf("<th colspan=%d>&nbsp;</th>", substr_count($link_lk, "<td")+1);
							$hv .= $hlk;
						}
					} elseif (substr($key, 0, 2) != "nd") {
						$hv .= sprintf("<th>%s</th>", str_replace("_", " ", $key));
						$ktot[] = 0;
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
				} elseif (substr($key, 0, 3) == "lnk" and strlen($link) > 1) {
					$href = str_replace("%d", $value, $link);
					$href = str_replace("%s", "<img src='images/zoom.png' alt='Details'>", $href);
					$ret .= "<td class='details'>" . $href . "</td>";
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
		if ($disptot > 0 and $totaantal > 1) {
			for ($kn=0; $kn < count($ktot); $kn++) {
				if ($kn+1 == $disptot) {
					$ret .= sprintf("<th>%s records</th>", number_format($totaantal, 0, ",", "."));
				} elseif ($ktot[$kn] == 0) {
					$ret .= "<th></th>";
				} else {
					$ret .= sprintf("<th class='number'>%s</th>", number_format($ktot[$kn], 2, ",", "."));
				}
			}
			$ret .= $hlk . "</tr>\n";
		}
		$ret .= "</table>\n";
	} else {
		$ret .= "<p class='mededeling'>Er zijn geen records beschikbaar.</p>\n";
	}
	$ret .= sprintf("</div>  <!-- Einde %s -->\n", $class);
	return $ret;
}

function debug($tekst, $alleenwm=1, $alert=0, $logact=0) {
	if ($_SERVER["HTTP_HOST"] == "phprbm.telling.nl") {
		$alleenwm = 0;
	}
	if ($alleenwm != 1 or (isset($_SESSION['webmaster']) and $_SESSION['webmaster'] == 1)) {
		if ($alert == 1) {
			printf("<script>alert('Debug: %s');</script>\n", $tekst);
		} else {
			printf("<p class='debug'>Debug: %s</p>\n", $tekst);
		}
		if ($logact == 1) {
			db_logboek("add", "Debug: " . $tekst, $type=99);
		}
	}
}

function accessdate($sqldate) {
	if (strlen($sqldate) < 10) {
		return "##";
	} else {
		return "#" . date(m/d/Y, strtotime($sqldate)) . "#";
	}
}

function startwith($value, $start) {
	if (substr($value, 0, strlen($start)) == $start) {
		return true;
	} else {
		return false;
	}
}

function change_month_to_uk($strdate) {

	$arrMonth[]=array("From" => "january", "To" => "January");
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
	
	foreach ($arrMonth as $key => $value) {
		$strdate = str_ireplace(" " . substr($value["From"], 0, 3) . " ", " " . $value["To"] . " ", $strdate);
		$strdate = str_ireplace(" " . $value["From"], " " . $value["To"], $strdate);
	}
	
	return $strdate;
}

class RBMmailer extends PHPMailer {

	function __construct() {
	
		$smtphost = db_param("smtphost");
		if (strlen($smtphost) > 0) {
			$this->Host = $smtphost;
			if (db_param("smtpport") > 0) {
				$this->Port = db_param("smtpport");
			}
			$this->IsSMTP(true);
			$smtpuser = db_param("smtpuser");
			if (strlen($smtpuser) > 0) {
				$this->SMTPAuth = true;
				$this->Username = $smtpuser;
				$this->Password = db_param("smtppw");	
			} else {
				$this->SMTPAuth = false;
			}
		} else {
			$this->IsMail(true);
		}
		$this->IsHTML(true);
		$this->From = $_SESSION['emailwebmaster'];
		$this->FromName = $_SESSION['naamvereniging'];
		$this->WordWrap = 110;
	}
	
	public function InformerenOuders($geboren, $emailouders) {
		$rv = "";
		if ($geboren > date("Y-m-d", mktime(0, 0, 0, date("m"), date("d"), date("Y")-18)) and strlen($emailouders) > 5) {
			$emailouders = str_replace(" ", "", $emailouders);
			$emailouders = str_replace(";", ",", $emailouders);
			foreach(explode(",", $emailouders) as $e) {
				if (self::ValidateAddress($e) and in_array($e, $this->to) == false and in_array($e, $this->cc) == false) {
					$this->AddCC(strtolower($e));
					if (strlen($rv) > 0) {
						$rv .= ", ";
					}
					$rv .= $e;
				}
			}
		}
		return $rv;
	}
	
}

/*
* Author: Simon Jarvis
* Copyright: 2006 Simon Jarvis
* Date: 08/11/2006
* Link: http://www.white-hat-web-design.co.uk/articles/php-image-resizing.php
* 
* This program is free software; you can redistribute it and/or 
* modify it under the terms of the GNU General Public License 
* as published by the Free Software Foundation; either version 2 
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful, 
* but WITHOUT ANY WARRANTY; without even the implied warranty of 
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
* GNU General Public License for more details: 
* http://www.gnu.org/licenses/gpl.html
*
*/
 
class SimpleImage {
   
   var $image;
   var $image_type;
 
   function load($filename) {
      $image_info = getimagesize($filename);
      $this->image_type = $image_info[2];
      if( $this->image_type == IMAGETYPE_JPEG ) {
         $this->image = imagecreatefromjpeg($filename);
      } elseif( $this->image_type == IMAGETYPE_GIF ) {
         $this->image = imagecreatefromgif($filename);
      } elseif( $this->image_type == IMAGETYPE_PNG ) {
         $this->image = imagecreatefrompng($filename);
      }
   }
   function save($filename, $image_type=IMAGETYPE_JPEG, $compression=75, $permissions=null) {
      if( $image_type == IMAGETYPE_JPEG ) {
         imagejpeg($this->image,$filename,$compression);
      } elseif( $image_type == IMAGETYPE_GIF ) {
         imagegif($this->image,$filename);         
      } elseif( $image_type == IMAGETYPE_PNG ) {
         imagepng($this->image,$filename);
      }   
      if( $permissions != null) {
         chmod($filename,$permissions);
      }
   }
   function output($image_type=IMAGETYPE_JPEG) {
      if( $image_type == IMAGETYPE_JPEG ) {
         imagejpeg($this->image);
      } elseif( $image_type == IMAGETYPE_GIF ) {
         imagegif($this->image);         
      } elseif( $image_type == IMAGETYPE_PNG ) {
         imagepng($this->image);
      }   
   }
   function getWidth() {
      return imagesx($this->image);
   }
   function getHeight() {
      return imagesy($this->image);
   }
   function resizeToHeight($height) {
      $ratio = $height / $this->getHeight();
      $width = $this->getWidth() * $ratio;
      $this->resize($width,$height);
   }
   function resizeToWidth($width) {
      $ratio = $width / $this->getWidth();
      $height = $this->getheight() * $ratio;
      $this->resize($width,$height);
   }
   function scale($scale) {
      $width = $this->getWidth() * $scale/100;
      $height = $this->getheight() * $scale/100; 
      $this->resize($width,$height);
   }
   function resize($width,$height) {
      $new_image = imagecreatetruecolor($width, $height);
      imagecopyresampled($new_image, $this->image, 0, 0, 0, 0, $width, $height, $this->getWidth(), $this->getHeight());
      $this->image = $new_image;   
   }      
}

?>
