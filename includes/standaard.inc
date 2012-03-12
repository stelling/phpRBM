<?php
session_start();

date_default_timezone_set('Europe/Amsterdam');
setlocale(LC_ALL,'nl_NL@euro');

if ((isset($_SESSON['webmaster']) and $_SESSON['webmaster'] == 1) or $_SERVER["HTTP_HOST"] == "phprbm.telling.nl") {
	error_reporting(E_ALL);
} else {
	error_reporting(0);
	if (!isset($_SESSION['lidid'])) {
		$_SESSION['lidid'] = 0;
	}
}

$dir = substr(__FILE__, 0, strrpos(__FILE__, "/"));  // Dit is nodig voor PHP 5.2, vanaf 5.3 kan dit gewoon direct __DIR__ worden.

require_once($dir . '/../config.php');
if (!isset($rsswieiswie) and $rsswieiswie != 1) {
	$rsswieiswie = 0;
}
if (!isset($muteerbarememos)) {
	$muteerbarememos = "DG";
}
if (!isset($emailbevestiginginschrijving)) {
	$emailbevestiginginschrijving = $emailledenadministratie;
}
	
require_once($dir . "/database.inc");
require_once($dir . "/authentication.inc");
require_once($dir . "/bewaking.inc");
require_once($dir . "/evenement.inc");
require_once($dir . "/mailing.inc");
require_once($dir . "/class.phpmailer.php");

$arrTL[] = "Alle";
$arrTL[] = "Leden";
$arrTL[] = "Klosleden";
$arrTL[] = "Voormalig leden";

if ($_SERVER['PHP_SELF'] == "/admin.php") {
	$tabpages[] = "Beheer logins";
	$tabpages[] = "Autorisatie";
	$tabpages[] = "Uploaden data";
	if (db_interface("aantalopenstaand") > 0) {
		$tabpages[] = "Downloaden wijzigingen";
	}
	$tabpages[] = "DB backup";
	$tabpages[] = "Logboek";
} else {
	$tabpages[] = "Verenigingsinfo";
	$tabpages[] = "Verenigingsinfo/Introductie";
	$tabpages[] = "Verenigingsinfo/Kader";
	$tabpages[] = "Verenigingsinfo/Onderscheidingen";
	$tabpages[] = "Verenigingsinfo/Overige commissies";
	$tabpages[] = "Eigen gegevens";
	$tabpages[] = "Eigen gegevens/Algemeen";
	if ($_SESSION['aantalafdelingen'] > 0) {
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
	if ($_SESSION['aantalbewaking'] > 0 and ($_SESSION['webmaster'] == 1 or db_bewaking_aantallen(0, $_SESSION['lidid']))) {
		$tabpages[] = "Eigen gegevens/Bewaking";
	}
	$tabpages[] = "Eigen gegevens/Diploma's";
	$tabpages[] = "Eigen gegevens/Financieel";
	$tabpages[] = "Eigen gegevens/Mailing";
	$tabpages[] = "Eigen gegevens/Logboek";
	$tabpages[] = "Wijzigen";
	$tabpages[] = "Wijzigen/Algemene gegevens";
	if (strlen($selfservicediplomas) > 3) {
		$tabpages[] = "Wijzigen/Diploma's";
	}
	$tabpages[] = "Wijzigen/Pasfoto";
	if (strlen($muteerbarememos) > 0) { 
		$tabpages[] = "Wijzigen/Bijzonderheden";
	}
	if (count(db_insbew("openblokken")) > 0) {
		$tabpages[] = "Wijzigen/Inschrijving bewaking";
	}
	if ($_SERVER["HTTP_HOST"] == "phprbm.telling.nl" and db_evenement("aantalopen") > 0) {
		$tabpages[] = "Wijzigen/Inschrijving evenementen";
	}
	$query = sprintf("SELECT COUNT(*) FROM %sLidmaatschap WHERE Lid=%d AND (Opgezegd IS NULL);", $table_prefix, $_SESSION['lidid']);
	$result = fnQuery($query);
	if ($result->fetchColumn() > 0) {
		$tabpages[] = "Wijzigen/Opzegging";
	}
	$tabpages[] = "Ledenlijst";
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
	if ($_SESSION['aantalbewaking'] > 0) {
		$tabpages[] = "Bewaking";
		$tabpages[] = "Bewaking/Bewakingsrooster";
		$tabpages[] = "Bewaking/Postindeling";
		$tabpages[] = "Bewaking/Aantallen";
	}
	if (count(db_insbew("overzicht")) > 0) {
		$tabpages[] = "Bewaking/Overzicht inschrijvingen";
	}
	if (db_scalar(sprintf("SELECT COUNT(*) FROM %sAdmin_activiteit WHERE TypeActiviteit=6;", $table_prefix)) > 0) {
		$tabpages[] = "Bewaking/Logboek inschrijving";
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
	if ($_SERVER["HTTP_HOST"] == "phprbm.telling.nl") {
		$tabpages[] = "Evenementen";
		$tabpages[] = "Evenementen/Overzicht";
		$tabpages[] = "Evenementen/Beheer";
	}
	
	if ($_SERVER["HTTP_HOST"] == "phprbm.telling.nl") {
//		$tabpages[] = "DMS";   Voorlopig geen tijd voor, dus uitgezet.
	}
}

if (isset($_GET['tp']) and strlen($_GET['tp']) > 1) {
	$curtab = explode("/", $_GET['tp']);
	$ia = false;
	foreach ($tabpages as $val) {
		if (str_replace("'", "", $val) == $_GET['tp']) {
			$ia = true;
		}
	}
	if ($ia == false) {
		$tabpages[] = $_GET['tp'];
	}
} else {
	$curtab[0] = $tabpages[0];
	$_GET['tp'] = $tabpages[0];
}

$query = "SET lc_time_names = 'nl_NL';";
fnQuery($query);

$pasfotoextenties = array('gif', 'jpg', 'jpeg', 'png');

function HTMLheader() {
	global $naamwebsite, $urlwebsite, $naamvereniging, $tabpages;
	global $currenttab, $currenttab2;
	
	if (isset($_GET['tp']) and strlen($_GET['tp']) > 1) {
		$sp = strpos($_GET['tp'], "/");
		if ($sp > 0) {
			$currenttab = substr($_GET['tp'], 0, $sp);
			$currenttab2 = substr($_GET['tp'], $sp+1);
		} else {
			$currenttab = $_GET['tp'];
			$currenttab2 = "";
			foreach ($tabpages as $tp) {
				if (startwith($tp, $currenttab) == true and strlen($tp) > strlen($currenttab) and strpos($tp, "/") > 0 and strlen($currenttab2) == 0) {
					$currenttab2 = substr($tp, strpos($tp, "/")+1);
				}
			}
		}
	} else {
		$currenttab = $tabpages[0];
		$_GET['tp'] = $tabpages[0];
	}
	
	if (strlen($currenttab) > 0 and strlen($currenttab2) > 0) {
		$title = $naamwebsite . " | " . $currenttab . " | " . $currenttab2;
	} elseif (strlen($currenttab) > 0) {
		$title = $naamwebsite . " | " . $currenttab;
	} else {
		$title = $naamwebsite;
	}

   if (!isset($_GET['header']) or $_GET['header'] != "no") {
		echo("<!DOCTYPE HTML>\n");
		echo("<html lang='nl'>\n");
		echo("<head>\n");
		echo("<meta charset='UTF-8'>\n");
		if (isset($_GET['tp']) and substr($_GET['tp'], 0, 7) == "Mailing") {
			echo("<script src='/includes/ckeditor/ckeditor.js'></script>\n");
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
		printf("<h1>%s</h1>\n", $naamwebsite);
	
		echo("<div id='login'>\n");
		echo("<form name='Login' action='/auth.php' method='post'>");
		if (isset($_SESSION['username']) and strlen($_SESSION['username']) > 4 and $_SESSION['lidid'] > 0) {
			printf("Welkom %s\n", htmlentities($_SESSION['roepnaamingelogde']));
			echo("<a href='/?tp=Wijzigen/Profiel'><img src='images/user.png' title='Profiel wijzigen' alt='Profiel wijzigen'></a>\n");
			printf("<a href='auth.php?actie=uitloggen'><img src='images/power.png' title='Uitloggen' alt='Uitloggen'></a>\n");
			if ($_SESSION['webmaster'] == 1 and $_SERVER['PHP_SELF'] != "/admin.php") {
				echo("<a href='admin.php'><img src='images/tool.png' title='Beheerdersmenu'></a>\n");
			} else {
				echo("<a href='index.php'><img src='images/home.png' title='Beginpagina'></a>\n");
			}
		} else {
			echo("Login: <input type='text' name='username' size=20 maxlength=50>\n");
			echo("&nbsp;Password: <input type='password' name='password' size=10 maxlength=12>\n");
			echo("&nbsp;<input type='submit' value='Inloggen' name='Inloggen'>\n");
			echo("<input type='checkbox' name='cookie' value=1 title='Automatisch inloggen'>\n");
		}
		echo("<a href='http://phprbm.telling.nl/uitleg.php'><img src='images/question.png' title='Uitleg gebruik en installatie'></a>\n");
		echo("</form>\n");
		echo("</div>  <!-- Einde login -->\n");
		echo("<div class='clear'></div>\n");
	}
	
	if (!isset($_GET['header']) or $_GET['header'] != "no") {
		fnDispMenu(1);
		echo("</div> <!-- Einde header -->\n");
		echo("<div class='clear'></div>\n");
	}
}

function HTMLfooter() {
	global $urlwebsite, $naamwebsite, $naamvereniging, $urlvereniging, $table_prefix;

	if (!isset($_GET['header']) or $_GET['header'] != "no") {
		if (isset($naamvereniging) and strlen($naamvereniging) > 2) {
			if (isset($urlvereniging) and strlen($urlvereniging) > 3) {
				$cr = sprintf("<a href='http://%s'>%s</a>", $urlvereniging, $naamvereniging);
			} else {
				$cr = $naamvereniging;
			}
		} else {
			$cr = "<a href='http://www.telling.nl'>&copy;&nbsp;S.Telling</a>";
		}
		echo("<div class='clear'></div>\n");
		printf("<div id='footer'>\n
				 <div id='footerleft'>%s</div>\n
				 <a href='http://%s'>%s</a>\n
				 <div id='footerright'>%s</div>\n
				 </div>  <!-- Einde footer  -->\n", $cr, $urlwebsite, $naamwebsite, strftime("%e %B %Y", filectime($_SERVER['SCRIPT_FILENAME'])));
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

function fnDispMenu($nivo=1, $xtra="") {
	global $tabpages, $curtab, $currenttab, $currenttab2;

	if (strlen($xtra) > 0 and substr($xtra, 0, 5) != "&amp;") {
		$xtra = "&amp;" . $xtra;
	}

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
				if ($currenttab == str_replace("'", "", $tp) and $nivo == 1) {
					printf("<li class='current'>%s</li>\n", $tpd);
				} elseif ($nivo == 2 and ($currenttab2 == str_replace("'", "", $tpd) or strlen($currenttab2) == 0)) {
					printf("<li class='current'>%s</li>\n", $tpd);
					$currenttab2 = str_replace("'", "", $tpd);
				} else {
					printf("<li><a href=\"%s?tp=%s%s\">%s</a></li>\n", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra, $tpd);
				}
			}
		}
	}
	echo("</ul>\n");
	if ($nivo > 1) {
		echo("<div class='clear'></div>\n");
	}
}

function fnDisplayForm($row, $xtra_eind="") {

	$ret = "<div id='formulier'>\n";
	if (isset($row)) {
		$ret .= "<table>\n";
		foreach($row as $key => $value) {
			if (substr($key, 0, 3) == "Kop") {
				$ret .= sprintf("<tr><th colspan=2>%s</th></tr>\n", $value);
			} elseif (substr($key, 0, 3) == "dte" and (!is_null($value))) {
				$ret .= sprintf("<tr><td class='label'>%s</td><td>%s</td></tr>\n", substr($key, 3), strftime('%e %B %Y', strtotime($value)));
			} elseif (substr($key, 0, 3) == "dtt" and (!is_null($value))) {
				$ret .= sprintf("<tr><td class='label'>%s</td><td>%s</td></tr>\n", substr($key, 3), strftime('%e %B %Y (%H:%M)', strtotime($value)));
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
	$ret .= "</div>  <!-- Einde formulier -->\n";
	return $ret;
}

function fnDisplayTable($rows, $link="", $th="", $disptot=0, $link_lk="") {
	$ret = "<div id='lijst'>\n";
	if (isset($rows) and count($rows) > 0) {
		$ret .= "<table>\n";
		$dispkop = 1;
		foreach($rows as $row) {
			if ($dispkop == 1) {
				$hv = "<tr>";
				$aant = 0;
				foreach($row as $key => $value) {
					if (strstr("dte, dtt, dtl, boo, int, cur, eml, num", substr($key, 0, 3)) !== false) {
						$hv .= sprintf("<th>%s</th>", str_replace("_", " ", substr($key, 3)));
					} elseif (strstr("lnk", substr($key, 0, 3)) !== false or strstr("llk", substr($key, 0, 3)) !== false) {
						$hv .= "<th>&nbsp;</th>";
					} elseif (substr($key, 0, 2) != "nd") {
						$hv .= sprintf("<th>%s</th>", str_replace("_", " ", $key));
					}
					$ktot[] = 0;
					$aant++;
				}
				if (strlen($th) > 1) {
					$ret .= sprintf("<tr><th colspan=%d class='tabelkop'>%s</th></tr>\n", $aant, $th);
				}
				$ret .= $hv . "</tr>\n";
				$dispkop = 0;
			}
			$ret .= "<tr>";
			$kn = 0;
			foreach($row as $key => $value) {
				if (substr($key, 0, 3) == "dte") {
					if ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td>%s</td>", strftime('%e %b %Y', strtotime($value)));
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
				} elseif (substr($key, 0, 3) == "dtl") {
					if ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td>%s</td>", strftime('%A %e %B %Y', strtotime($value)));
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
				} elseif (substr($key, 0, 3) == "dtt") {
					if ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td>%s</td>", strftime('%e %B %Y (%H:%M)', strtotime($value)));
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
					$ret .= "<td>" . $href . "</td>";
				} elseif (substr($key, 0, 3) == "llk" and strlen($link_lk) > 1) {
					$ret .= sprintf("<td>" . $link_lk . "</td>", $value, "<img src='images/zoom.png' alt='Details'>");
				} elseif (substr($key, 0, 3) == "eml") {
					$ret .= sprintf("<td>%s</td>", fnDispEmail($value, "", 1));
				} elseif (substr($key, 0, 3) == "num") {
					$ret .= sprintf("<td class='number'>%s</td>", $value);
				} elseif (substr($key, 0, 2) != "nd") {
					$ret .= sprintf("<td>%s</td>", htmlentities($value));
				}
				$kn++;
			}
			$ret .= "</tr>\n";
		}
		if ($disptot == 1) {
			$ret .= "<tr><th>Totaal</th>";
			for ($kn=1; $kn < count($ktot); $kn++) {
				if ($ktot[$kn] == 0) {
					$ret .= "<th></th>";
				} else {
					$ret .= sprintf("<th class='number'>%s</th>", number_format($ktot[$kn], 2, ",", "."));
				}
			}
			$ret .= "</tr>\n";
		}
		$ret .= "</table>\n";
	} else {
		$ret .= "<p class='mededeling'>Er zijn geen records beschikbaar.</p>\n";
	}
	$ret .= "</div>  <!-- Einde lijst -->\n";
	return $ret;
}

function debug($tekst="debug", $alleenwm=1, $alert=0, $logact=0) {
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
		$strdate = str_ireplace(" " . $value["From"] . " ", " " . $value["To"] . " ", $strdate);
		$strdate = str_ireplace(" " . substr($value["From"], 0, 3) . " ", " " . $value["To"] . " ", $strdate);
	}
	
	return $strdate;
}

function strongPassword($psPassword, array $paCommonWords = null) {
    if ($paCommonWords !== null) {
        $psCommonWords = implode('|', array_map('preg_quote', $paCommonWords));
    }

    /*
     * Strong password with the following requirements.
     * - At least 6 characters long.
     * - At least 1 uppercase, AND at least 1 lowercase
     * - At least 1 digit OR at least 1 alphanumeric.
     * - No spaces
     */
    if (!preg_match('~^(?=[^a-z]*[a-z])(?=[^A-Z]*[A-Z])(?=\D*\d)(?=[A-Za-z0-9]*[^A-Za-z0-9])\S{6,}$~', $psPassword)) {
        return false;
    }
    // Look for common words, like for example is same as the username
    elseif ($paCommonWords !== null && preg_match('{' . $psCommonWords . '}i', $psPassword)) {
        return false;
    }

    return true;
}

class RBMmailer extends PHPMailer {

	function __construct() {
		global $smtphost, $smtpuser, $smtppw, $nameorganisation, $emailwebmaster;
	
		if (strlen($smtphost) > 0) {
			$this->Host = $smtphost;
			$this->IsSMTP(true);
			$this->IsHTML(true);
			if (strlen($smtpuser) > 0 and strlen($smtppw) > 0) {
				$this->SMTPAuth = true;
				$this->Username = $smtpuser;
				$this->Password = $smtppw;	
			} else {
				$this->SMTPAuth = false;
			}
		} else {
			$this->IsMail(true);
			$mail->Mailer = "mail";
		}
		$this->From = $emailwebmaster;
		$this->FromName = $nameorganisation;
		$this->WordWrap = 110;
		$this->AltBody    = "Om deze mail te bekijken gebruik een e-mailclient die met HTML overweg kan.";
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

function fnDiplayTable($rows, $link="", $th="", $disptot=0, $link_lk="") {
	debug("Oude functie fnDiplayTable");
	fnDisplayTable($rows, $link, $th, $disptot, $link_lk);
}

function fnDiplayForm($row, $xtra_eind="") {
	debug("Oude functie fnDiplayForm");
	fnDisplayForm($row, $xtra_eind);
}

?>