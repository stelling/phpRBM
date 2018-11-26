<?php
include('./includes/standaard.inc');

if ((!isset($_SESSION['lidid']) or $_SESSION['lidid'] == 0) and isset($_COOKIE['password']) and strlen($_COOKIE['password']) > 5) {
	fnAuthenticatie(0);
}

if (!isset($_GET['op']) or (toegang($_GET['tp']) == false and $_SESSION['aantallid'] > 1)) {
	$_GET['op'] = "";
}

if (isset($_GET['op']) and $_GET['op'] == "downloadwijz" and $_GET['tp'] == "Downloaden wijzigingen") {
	header("Content-type: text/plain");
	header("Content-Disposition: attachment; filename=import.sql");
	foreach(db_interface() as $row) {
		echo($row->SQL . "\n");
	}
	exit();
}

if ($currenttab == "Beheer logins" or $currenttab == "Autorisatie" or $currenttab == "Logboek") {
	HTMLheader(1);
} else {
	HTMLheader(0);
}

if ($_GET['op'] == "deletelogin" and $_GET['tp'] == "Beheer logins") {
	db_logins("delete", "", "", $_GET['lidid']);
	printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $currenttab);
} elseif ($_GET['op'] == "unlocklogin" and $_GET['tp'] == "Beheer logins") {
	db_logins("unlock", "", "", $_GET['lidid']);
	printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $currenttab);
} elseif (isset($_POST['tabpage_nw']) and strlen($_POST['tabpage_nw']) > 0 and $_GET['tp'] == "Autorisatie") {
	db_authorisation("add", 0, $_POST['tabpage_nw']);
} elseif ($_GET['op'] == "deleteautorisatie" and $_GET['tp'] == "Autorisatie") {
	db_authorisation("delete", $_GET['recid']);
	printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $currenttab);
} elseif ($_GET['op'] == "changeaccess" and $_GET['tp'] == "Autorisatie") {
	foreach(db_authorisation("lijst") as $row) {
		$vn = sprintf("toegang%d", $row->RecordID);
		if (isset($_POST[$vn]) and $_POST[$vn] != $row->Toegang) {
			$query = sprintf("UPDATE %1\$sAdmin_access SET Toegang=%2\$d, Gewijzigd=SYSDATE() WHERE RecordID=%3\$d AND Toegang<>%2\$d;", $table_prefix, $_POST[$vn], $row->RecordID);
			$result = fnQuery($query);
			if ($result > 0) {
				if ($_POST[$vn] == -1) {
					$mess = sprintf("Totgang tot '%s' is alleen voor webmasters beschibaar gemaakt.", $row->Tabpage);
				} else {
					$mess = sprintf("Toegang '%s' is naar groep '%s' aangepast.", $row->Tabpage, db_naam_onderdeel($_POST[$vn], "Iedereen"));
				}
				db_logboek("add", $mess, 5);
			}
		}
	}
} elseif ($_GET['op'] == "uploaddata") {
	if (isset($_FILES['SQLupload']['tmp_name']) and strlen($_FILES['SQLupload']['tmp_name']) > 3) {
		fnQuery("SET CHARACTER SET utf8;");
		$queries = file_get_contents($_FILES['SQLupload']["tmp_name"]);
		if ($queries !== false) {
			$mess = "Bestand is succesvol ge-upload.";
			db_logboek("add", $mess, 9, 0, 1);
			$querylid = sprintf("SELECT COUNT(*) FROM %sLid;", $table_prefix);
			$querylo = sprintf("SELECT COUNT(*) FROM %sLidond;", $table_prefix);
			if (substr_count($queries, $table_prefix) == 0) {
				$mess = sprintf("In het upload bestand komt de juiste table name prefix (%s) niet voor. Dit bestand wordt niet verwerkt.", $table_prefix);
				db_logboek("add", $mess, 9, 0, 1);
			} elseif (strpos($queries, $table_prefix . "Lid") === FALSE and db_scalar($querylid) < 5) {
				$mess = sprintf("De verplichte tabel '%sLid' zit niet in deze upload. Dit bestand wordt niet verwerkt.", $table_prefix);
				db_logboek("add", $mess, 9, 0, 1);
			} elseif (strpos($queries, $table_prefix . "Lidond") === FALSE and db_scalar($querylo) < 5) {
				$mess = sprintf("De verplichte tabel '%sLidond' zit niet in deze upload. Dit bestand wordt niet verwerkt.", $table_prefix);
				db_logboek("add", $mess, 9, 0, 1);
			} elseif (fnQuery($queries) !== true) {
				$mess = "Bestand is in de database verwerkt.";
				db_logboek("add", $mess, 9, 0, 1);
				db_onderhoud(1);
				printf("<script>setTimeout(\"location.href='%s';\", 15000);</script>\n", $_SERVER['PHP_SELF']);
			}
		}
	} else {
		$mess = sprintf("Er is iets mis gegaan tijdens het uploaden. Error: %s. Klik <a href='http://nl3.php.net/manual/en/features.file-upload.errors.php'>hier</a> voor uitleg van de code.", $_FILES['SQLupload']['error']);
		db_logboek("add", $mess, 2, 0, 1);
	}
} elseif ($_GET['op'] == "afmeldenwijz" and $_GET['tp'] == "Downloaden wijzigingen") {
	$mess = db_interface("afmelden");
	printf("<p class='mededeling'>%s</p>\n", $mess);
} elseif ($_GET['op'] == "backup") {
	db_backup();
} elseif ($_GET['op'] == "logboekopschonen") {
	db_logboek("opschonen");
} elseif ($_GET['op'] == "evenementenopschonen") {
	$mess = db_evenement("opschonen");
	printf("<p class='mededeling'>%s</p>\n", $mess);
} elseif ($_GET['op'] == "mailingsopschonen") {
	db_mailing("opschonen");
} elseif ($_GET['op'] == "loginsopschonen") {
	$mess = db_logins("opschonen");
	printf("<p class='mededeling'>%s</p>\n", $mess);
} elseif ($_GET['op'] == "autorisatieopschonen") {
	db_authorisation("opschonen");
} elseif ($_GET['op'] == "orderregelsopschonen") {
	$mess = db_orderregel("opschonen");
	printf("<p class='mededeling'>%s</p>\n", $mess);
} elseif ($_GET['op'] == "artikelenopschonen") {
	$mess = db_artikel("opschonen");
	printf("<p class='mededeling'>%s</p>\n", $mess);
}

if ($currenttab == "Beheer logins" and toegang()) {

	$arrSort[] = "Login";
	$arrSort[] = "Achternaam";
	$arrSort[] = "Naam";
	$arrSort[] = "Woonplaats";
	$arrSort[] = "Ingevoerd";
	$arrSort[] = "Laatste login";
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$naamfilter = $_POST['NaamFilter'];
		$sorteren = $_POST['Sorteren'];
		if (isset($_POST['sortdesc'])) {
			$sortdesc = true;
		} else {
			$sortdesc = false;
		}
	} else {
		$naamfilter = "";
		$sorteren = $arrSort[1];
		$sortdesc = false;
	}
	
	echo("<div id='filter'>\n");
	printf("<form name='filter' action='%s?%s' method='post'>", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
	echo("<table>\n");
	echo("<tr>\n");
	printf("<td class='label'>Naam/login bevat</td><td><input type='text' name='NaamFilter' size=20 value='%s' onblur='form.submit();'></td>\n", $naamfilter);
	echo("<td class='label'>Sorteren op</td><td>");
	foreach($arrSort as $s) {
		if ($s == $sorteren) {$c=" checked"; } else { $c=""; }
		printf("<input type='radio'%2\$s name='Sorteren' value='%1\$s' onclick='this.form.submit();'>%1\$s\n", $s, $c);
	}
	if ($sortdesc) {$c = " checked";	} else {	$c = "";	}
	printf("&nbsp;<input type='checkbox' value='1' name='sortdesc'%s onclick='this.form.submit();'> Desc</td>\n", $c);
	echo("</tr>\n");
	echo("</table>\n");
	echo("</form>");
	echo("</div>  <!-- Einde filter -->\n");
	
	if (strlen($naamfilter) > 0) {
		$w = sprintf("(L.Achternaam LIKE '%%%1\$s%%' OR L.Roepnaam LIKE '%%%1\$s%%' OR Login.Login LIKE '%%%1\$s%%')", $naamfilter);
	} else {
		$w = "";
	}
	
	if ($sorteren == $arrSort[0]) {
		$ord = "Login.Login";
	} elseif ($sorteren == $arrSort[2]) {
		if ($sortdesc) {
			$ord = "L.Roepnaam DESC, L.Tussenv DESC, L.Achternaam";
		} else {
			$ord = "L.Roepnaam, L.Tussenv";
		}
	} elseif ($sorteren == $arrSort[4]) {
		$ord = "Login.Ingevoerd";
	} elseif ($sorteren == $arrSort[5]) {
		$ord = "Login.LastLogin";
	} elseif (strlen($sorteren) > 0) {
		$ord = "L." . $sorteren;
	}
	if ($sortdesc) {
		$ord .= " DESC";
	}
	
	$lnk = sprintf("<a href='%s?op=deletelogin&amp;lidid=%%d'><img src='images/del.png' title='Verwijder login'></a>", $_SERVER['PHP_SELF']);
	if (db_param("login_maxinlogpogingen") > 0) {
		$lnk_lk = sprintf("<a href='%s?op=unlocklogin&amp;lidid=%%d' title='Reset foutieve logins'><img src='images/unlocked_01.png'></a>", $_SERVER['PHP_SELF']);
	} else {
		$lnk_lk = "";
	}

	echo(fnDisplayTable(db_logins("lijst", "", "", 0, $w, "", $ord), $lnk, "", 5, $lnk_lk));
} elseif ($currenttab == "Autorisatie" and toegang($_GET['tp'])) {
	echo("<div id='lijst'>\n");
	printf("<form name='formauth' method='post' action='%s?tp=%s&amp;op=changeaccess'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<table>\n");
	echo("<tr><th></th><th>Onderdeel</th><th>Toegankelijk voor</th><th>Ingevoerd</th></tr>\n");
	foreach(db_authorisation("lijst") as $row) {
		$del = sprintf("<a href='%s?tp=%s&amp;op=deleteautorisatie&amp;recid=%d'><img src='images/del.png' title='Verwijder record'></a>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
		$selectopt = sprintf("<option value=-1%s>Alleen webmasters</option>\n", checked($row->Toegang, "option", -1));
		$selectopt .= sprintf("<option value=0%s>Iedereen</option>\n", checked($row->Toegang, "option", 0));
		foreach(db_Onderdelen("lijst") as $ond) {
			$selectopt .= sprintf("<option value=%d%s>%s</option>\n", $ond->RecordID, checked($row->Toegang, "option", $ond->RecordID), htmlentities($ond->Naam));
		}
		printf("<tr>\n<td>%s</td>\n<td>%s</td>\n<td><select name='toegang%d' onchange='this.form.submit();'>\n%s</select></td>\n<td>%s</td></tr>\n", $del, $row->Tabpage, $row->RecordID, $selectopt, strftime("%e %B %Y", strtotime($row->Ingevoerd)));
	}
	$optionstab = "<option value=''>Selecteer ...</option>\n";
	foreach (db_authorisation("tabpages") as $row) {
		$optionstab .= sprintf("<option value='%1\$s'>%1\$s</option>\n", $row->Tabpage);
	}
	printf("<tr><td><img src='images/star.png' alt='Ster' title='Nieuw record'></td><td><select name='tabpage_nw' onChange='this.form.submit();'>%s</select></td><td></td><td></td></tr>\n", $optionstab);
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde lijst -->\n");
	
} elseif ($currenttab == "Instellingen" and toegang($_GET['tp'])) {
	fnInstellingen();
} elseif ($currenttab == "Uploaden data" and ($_SESSION['aantallid'] == 0 or toegang($_GET['tp']))) {
	$aantal = db_interface("aantalopenstaand");
	if ($aantal > 0) {
		printf("<p class='mededeling'>Er staan %d wijzigingen te wachten om verwerkt te worden. Het is daarom niet verstandig om een upload te doen.</p>", $aantal);
	}
	$aantal = db_logins("aantalingelogd");
	if ($aantal > 1) {
		printf("<p class='mededeling'>Er staan zijn momenteel %d gebruikers ingelogd. Het is daarom niet verstandig om een upload te doen.</p>", $aantal);
	}
	echo("<div id='invulformulier'>\n");
	printf("<form name='formupload' method='post' action='%s?tp=%s&amp;op=uploaddata' enctype='multipart/form-data'>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	echo("<table>\n");
	echo("<tr><td class='label'>Bestand</td><td><input type='file' name='SQLupload'><input type='submit' value='Verwerk'></td></tr>\n");
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde invulformulier -->\n");	
} elseif ($currenttab == "Downloaden wijzigingen" and toegang($_GET['tp'])) {
	
	$xf = "IFNULL(L.LoginWebsite, '') <> Login.Login";
	$rows = db_logins("lijst", "", "", 0, $xf);
	foreach ($rows as $row) {
		$query = sprintf("UPDATE %sLid SET LoginWebsite='%s' WHERE Nummer=%d;", $table_prefix, $row->Login, $row->LidID);
		if (fnQuery($query) > 0) {
			$sql = sprintf("UPDATE Lid SET `LoginWebsite`='%s', Gewijzigd=Date() WHERE Nummer=%d;", $row->Login, $row->LidID);
			db_interface("add", $sql, $row->LidID, 1);
		}
	}

	$copytext = "";
	echo("<h2>Wijzigen op de website, te verwerken in de Access database.</h2>");
	printf("<form name='formdownload' method='post' action='%s?tp=%s&amp;op=downloadwijz'>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	$rows = db_interface("lijst");
	if (count($rows) > 0) {
		echo(fnDisplayTable($rows, "", "", 1));
		
		foreach ($rows as $row) {
			$copytext .= $row->SQL . "\n";
		}
		
		echo("<p class='mededeling'><input type='submit' value='Download wijzigingen'>\n");
		printf("&nbsp;<input type='button' value='Wijzigingen afmelden' OnClick=\"location.href='%s?tp=%s&amp;op=afmeldenwijz'\"></p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
		$copytext .= "\n";
	} else {
		echo("<p class='mededeling'>Er zijn geen wijzigingen die nog verwerkt moeten worden.</p>\n");
	}
	echo("</form>\n");
		
	if (strlen($copytext) > 0) {
		echo("<h2>SQL code voor in MS-Access, om te kopi&euml;ren:</h2>\n");
		printf("<div class='CopyPaste'>%s</div>\n", $copytext);
	}
	
} elseif ($currenttab == "Onderhoud" and toegang($_GET['tp'])) {
	db_createtables();
	db_onderhoud();

	echo("<div id='dbonderhoud'>\n");
	
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=backup\"' value='Backup'>&nbsp;Maak een backup van de database. Laatste backup is op %s gemaakt.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), db_param("laatstebackup"));
	if (db_param("logboek_bewaartijd") > 0) {
		printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=logboekopschonen\"' value='Logboek opschonen'>&nbsp;Verwijder alle records uit het logboek, die ouder dan %d maanden zijn.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $bewaartijdlogging);
	}
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=evenementenopschonen\"' value='Evenementen opschonen'>&nbsp;Opschonen evenementen, inclusief bijbehorende deelnemers, die langer dan 6 maanden geleden zijn verwijderd.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	if (db_param("mailing_bewaartijd") > 0) {
		printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=mailingsopschonen\"' value='Mailings opschonen'>&nbsp;Mailings die langer dan %d maanden in de prullenbak zitten worden definitief verwijderd.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), db_param("mailing_bewaartijd"));
	}
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=loginsopschonen\"' value='Logins opschonen'>&nbsp;Opschonen van logins die om diverse redenen niet meer nodig zijn.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=autorisatieopschonen\"' value='Autorisatie opschonen'>&nbsp;Verwijderen toegang waar alleen de webmaster toegang toe heeft en die ouder dan 3 maanden zijn.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=orderregelsopschonen\"' value='Orderregels opschonen'>&nbsp;Opschonen van de orderregels van de bestellingen.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=artikelenopschonen\"' value='Artikelen opschonen'>&nbsp;Opschonen van artikelen zonder bestellingen uit de webshop.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	echo("</div>  <!-- Einde dbonderhoud -->\n");
	
	$query = "SELECT Version() AS Version;";
	$result = fnQuery($query);
	printf("<div id='versies'>PHP: %s / Database: %s</div>  <!-- Einde versies -->\n", substr(phpversion(), 0, 6), $result->fetchColumn());
	db_param("versiephp", "updval", substr(phpversion(), 0, 6));
	
} elseif ($currenttab == "Logboek" and toegang($_GET['tp'])) {
	if (!isset($_POST['lidfilter']) or strlen($_POST['lidfilter']) == 0) {
		$_POST['lidfilter'] = 0;
	}
	if (!isset($_POST['typefilter']) or strlen($_POST['typefilter']) == 0) {
		$_POST['typefilter'] = -1;
	}
	if (!isset($_POST['ipfilter']) or strlen($_POST['ipfilter']) == 0) {
		$_POST['ipfilter'] = "";
	}
	echo("<div id='filter'>\n");
	printf("<form method='post' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	echo("<table>\n<tr>\n");
	
	echo("<td class='label'>Filter op lid:</td><td><select name='lidfilter' id='lidfilter' onchange='form.submit();'>\n");
	echo("<option value=0>Alle</option>\n");
	foreach (db_logboek("lidlijst") as $row) {
		if ($row->LidID == $_POST['lidfilter']) {
			$s = "selected";
		} else {
			$s = "";
		}
		printf("<option value=%d %s>%s</option>\n", $row->LidID, $s, htmlentities($row->Naam));
	}
	echo("</select>\n</td>\n");
	
	echo("<td class='label'>Filter op type:</td><td><select name='typefilter' id='typefilter' onchange='form.submit();'>\n");
	echo("<option value=-1>Alle</option>\n");
	foreach ($TypeActiviteit as $key => $val) {
		if (db_logboek("aantal", "", $key) > 0) {
			if ($key == $_POST['typefilter']) {
				$s = "selected";
			} else {
				$s = "";
			}
			printf("<option value=%d %s>%s (%d)</option>\n", $key, $s, htmlentities($val), $key);
		}
	}
	echo("</select>\n</td>\n");
	
	echo("<td class='label'>Filter op IP-adres:</td><td><select name='ipfilter' id='ipfilter' onchange='form.submit();'>\n");
	echo("<option value='*'>Alle</option>\n");
	foreach (db_logboek("iplijst") as $row) {
		if ($row->IP_adres == $_POST['ipfilter']) {
			$s = "selected";
		} else {
			$s = "";
		}
		printf("<option value='%s'%s>%s</option>\n", $row->IP_adres, $s, $row->IP_adres);
	}
	echo("</select>\n</td>\n");
	
	echo("</tr>\n</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde filter -->\n");
	
	$rows = db_logboek("lijst", "", $_POST['typefilter'], $_POST['lidfilter'], 0, 0, $_POST['ipfilter']);
	echo(fnDisplayTable($rows, "", "", 0, "", "", "logboek"));
} elseif ($currenttab == "Info" and toegang($_GET['tp'])) {
	phpinfo();
}

HTMLfooter();

function fnInstellingen() {
	global $table_prefix;

	// Omschrijving NT = Niet tonen in dit scherm
	$arrParam['db_backup_type'] = "Welke taballen moeten worden gebackuped? 1=interne phpRBM-tabellen, 2=tabellen uit Access, 3=beide.";
	$arrParam['db_backuptarren'] = "Moet de backup gecomprimeerd worden? Let op, de webhost moet dit wel ondersteunen. En ook moet de PHP-functie 'exec' beschikbaar zijn.";
	$arrParam['db_backupsopschonen'] = "Na hoeveel dagen moeten back-ups automatisch verwijderd worden? 0 = nooit.";
	$arrParam['db_folderbackup'] = "In welke folder moet de backup worden geplaatst?";
	$arrParam['emailwebmaster'] = "Het e-mailadres van de webmaster.";
	$arrParam['kaderoverzichtmetfoto'] = "Moeten op het kaderoverzicht pasfoto's getoond worden?";
	$arrParam['toonpasfotoindiennietingelogd'] = "Mag de bezoeker een pasfoto worden getoond als deze niet ingelogd?";
	$arrParam['meisjesnaamtonen'] = "Moeten de namen van leden ook de meisjesnaam bevatten?";
	$arrParam['login_lidnrnodigbijaanvraag'] = "Moet een lid zijn of haar lidnummer opgeven als er een login aangevraagd wordt?";
	$arrParam['login_autounlock'] = "Na hoeveel minuten moet een gelockede login automatisch geunlocked worden? 0 = alleen handmatig unlocken.";
	$arrParam['login_beperkttotgroep'] = "Vul hier de RecordID's, gescheiden door een komma, van de groepen (zie tabel ONDERDL) in die toegang hebben. Leeg = alleen webmasters hebben toegang.";
	$arrParam['login_bewaartijd'] = "Het aantal maanden dat logins na het laatste gebruik bewaard blijven. Historie is direct aan het lid gekopppeld en wordt dus niet verwijderd. 0 = altijd bewaren.";
	$arrParam['login_geldigheidactivatie'] = "Hoelang in uren is een activatielink geldig? 0 = altijd.";
	$arrParam['login_bewaartijdnietgebruikt'] = "Het aantal dagen dat logins wordt bewaard, nadat het is aangevraagd en nog niet gebruikt is.";
	$arrParam['logboek_bewaartijd'] = "Hoeveel maanden moet de logging bewaard blijven. 0 = altijd bewaren.";
	$arrParam['mailing_beperkfrom'] = "NT";
	$arrParam['mailing_bevestigingbestelling'] = "Het nummer van de mailing die bij een bestelling verstuurd moet worden. 0 = geen.";
	$arrParam['mailing_bevestigingopzegging'] = "NT";
	$arrParam['mailing_bewaartijd'] = "NT";
	$arrParam['mailing_bewakinginschrijving'] = "Het nummer van de mailing die als bevestiging van een inschrijving voor de bewaking verstuurd moet worden. 0 = geen.";
	$arrParam['mailing_extensies_toegestaan'] = "NT";
	$arrParam['mailing_herstellenwachtwoord'] = "NT";
	$arrParam['mailing_lidnr'] = "NT";
	$arrParam['mailing_rekening_stuurnaar'] = "NT";
	$arrParam['mailing_rekening_from_adres'] = "NT";
	$arrParam['mailing_rekening_from_naam'] = "NT";
	$arrParam['mailing_rekening_nulregelsweglaten'] = "NT";
	$arrParam['mailing_resultaatversturen'] = "NT";
	$arrParam['mailing_validatielogin'] = "NT";
	$arrParam['max_grootte_bijlage'] = "De maximalale grootte in bytes van ��n bijlage in een mailing. Optioneel veld. Als je niets specificeerd dan is 2MB het maximum.";
	$arrParam['login_maxinlogpogingen'] = "Na hoeveel foutieve inlogpogingen moet het account geblokkeerd worden? 0 = nooit.";
	$arrParam['login_maxlengte'] = "De maximale lengte die een login mag zijn. Minimaal 7 en maximaal 15 invullen.";
	$arrParam['wachtwoord_minlengte'] = "De minimale lengte van een wachtwoord. Minimaal 7 en maximaal 15 invullen.";
	$arrParam['wachtwoord_maxlengte'] = "De maximale lengte van een wachtwoord. Minimaal 7 en maximaal 15 invullen.";
	$arrParam['maxmailsperdag'] = "NT";
	$arrParam['maxmailsperminuut'] = "NT";
	$arrParam['naamwebsite'] = "Dit is de naam zoals deze in de titel en op elke pagina getoond wordt.";
	$arrParam['performance_trage_select'] = "Vanaf hoeveel seconden moet een select-statement in het logboek worden gezet. 0 = nooit.";
	$arrParam['termijnvervallendiplomasmailen'] = "Hoeveel maanden vooruit moeten leden een herinnering krijgen als een diploma gaat vervallen. 0 = geen herinnering sturen.";
	$arrParam['termijnvervallendiplomasmelden'] = "Hoeveel maanden vooruit en achteraf moeten vervallen diploma op het voorblad getoond worden.";
	$arrParam['toneninschrijvingenbewakingen'] = "Moeten bij de gegevens van een lid ook inschrijvingen voor bewakingen getoond worden?";
	$arrParam['tonentoekomstigebewakingen'] = "Moeten bij de gegevens van een lid ook toekomstige bewakingen getoond worden?";
	$arrParam['typemenu'] = "1 = per niveau een aparte regel, 2 = ��n menu met dropdown, 3 = ��n menu met dropdown en extra menu voor niveau 2.";
	$arrParam['urlvereniging'] = "De URL van de website van de vereniging.";
	$arrParam['url_eigen_help'] = "Als een gebruiker op de help klikt wordt hier naar verwezen in plaats van de standaard help.";
	$arrParam['verjaardagenaantal'] = "Hoeveel verjaardagen moeten er maximaal in de verenigingsinfo worden getoond. Als er meerdere leden op dezelfde dag jarig zijn, wordt dit aantal overschreden.";
	$arrParam['verjaardagenvooruit'] = "Hoeveel dagen vooruit moeten de verjaardagen in de verenigingsinfo getoond worden?";
	$arrParam['versie'] = "NT";
	$arrParam['versiephp'] = "NT";
	
	$arrParam['zs_emailnieuwepasfoto'] = "Waar moet een nieuwe pasfoto naar toe gemaild worden?";
	$arrParam['zs_incl_beroep'] = "Is het veld 'Beroep' in de zelfservice beschikbaar?";
	$arrParam['zs_incl_bsn'] = "Is het veld 'Burgerservicenummer' in de zelfservice beschikbaar?";
	$arrParam['zs_incl_machtiging'] = "Is het veld 'Machtiging incasso afgegeven' in de zelfservice beschikbaar?";
	$arrParam['zs_incl_bezwmachtiging'] = "Is het veld 'Bezwaar machtiging' in de zelfservice beschikbaar?";
	$arrParam['zs_incl_emailouders'] = "Is het veld 'E-mail ouders' in de zelfservice beschikbaar?";
	$arrParam['zs_incl_emailvereniging'] = "Is het veld 'E-mail vereniging' in de zelfservice beschikbaar?";
	$arrParam['zs_incl_iban'] = "Moet het IBAN via de zelfservice gemuteerd kunnen worden?";
	$arrParam['zs_incl_legitimatie'] = "Is de legitimatie in de zelfservice beschikbaar?";
	$arrParam['zs_incl_slid'] = "Is het veld 'Sportlink ID' in de zelfservice beschikbaar?";
	$arrParam['zs_muteerbarememos'] = "Welke soorten memo's mogen leden zelf muteren, scheiden door een komma.";
	$arrParam['zs_opzeggingautomatischverwerken'] = "Moet een opzegging van een lid in de zelfservice automatisch verwerkt worden?";
	$arrParam['zs_opzegtermijn'] = "De opzegtermijn van de vereniging in maanden.";
	$arrParam['zs_voorwaardenbestelling'] = "Deze regel wordt bij de online-bestellingen in de zelfservice vermeld.";
	$arrParam['zs_voorwaardeninschrijving'] = "Deze regel wordt bij de inschrijving als voorwaarde voor de inschrijving voor de bewaking vemeld.";
	
	foreach ($arrParam as $naam => $val) {
		db_param($naam, "controle");
	}
	$specmailing = array("mailing_bewakinginschrijving", "mailing_bevestigingbestelling");
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		foreach (db_param("", "lijst") as $row) {
			$mess = "";
			$pvn = sprintf("value_%d", $row->RecordID);
			if (isset($_POST[$pvn]) or $row->ParamType == "B") {
				if (isset($_POST[$pvn])) {
					$_POST[$pvn] = str_replace("\"", "'", $_POST[$pvn]);
				}
				if (in_array($row->Naam, array("beperktotgroep", "muteerbarememos"))) {
					$_POST[$pvn] = str_replace(" ", "", $_POST[$pvn]);
				} elseif (in_array($row->Naam, $specmailing) and $_POST[$pvn] > 0 and db_mailing("exist", $_POST[$pvn]) == false) {
					$_POST[$pvn] = 0;
					$mess = sprintf("Parameter '%s' wordt 0 gemaakt, omdat de ingevoerde mailing niet (meer) bestaat. ", $row->Naam);
				} elseif ($row->Naam == "mailing_beperkfrom" and substr($_POST[$pvn], 0, 1) == "@") {
					$_POST[$pvn] = substr($_POST[$pvn], 1);
				} elseif ($row->Naam == "mailing_beperkfrom" and substr($_POST[$pvn], 0, 4) == "www.") {
					$_POST[$pvn] = substr($_POST[$pvn], 4);
				} elseif ($row->Naam == "typemenu" and (strlen($_POST[$pvn]) == 0 or $_POST[$pvn] < 1 or $_POST[$pvn] > 3)) {
					$_POST[$pvn] = 1;
					$mess = sprintf("Parameter '%s' wordt 1 gemaakt, omdat deze alleen 1, 2 of 3 mag zijn. ", $row->Naam);
				} elseif (substr($row->Naam, 0, 3) == "url" and isset($_POST[$pvn])) {
					$_POST[$pvn] = str_replace("http://", "", $_POST[$pvn]);
					$_POST[$pvn] = str_replace("https://", "", $_POST[$pvn]);
				}
				if ($row->ParamType == "B") {
					if (isset($_POST[$pvn]) and $_POST[$pvn] == "1") {
						$_POST[$pvn] = 1;
					} else {
						$_POST[$pvn] = 0;
					}
				} elseif ($row->ParamType == "I" or $row->ParamType == "F") {
					if ($row->ParamType == "F") {
						$_POST[$pvn] = str_replace(",", ".", $_POST[$pvn]);
					}
					if (strlen($_POST[$pvn]) == 0 or is_numeric($_POST[$pvn]) == false) {
						$_POST[$pvn] = 0;
					}
				}
				if (strlen($mess) > 0) {
					db_logboek("add", $mess, 13, 0, 1);
				}
				if (strlen($arrParam[$row->Naam]) > 2) {
					db_param($row->Naam, "updval", $_POST[$pvn]);
				}
			}
		}
		if (db_param("wachtwoord_minlengte") < 7) {
			db_param("wachtwoord_minlengte", "updval", 7);
		} elseif (db_param("wachtwoord_minlengte") > 15) {
			db_param("wachtwoord_minlengte", "updval", 15);
		} elseif (db_param("wachtwoord_minlengte") > db_param("wachtwoord_maxlengte")) {
			db_param("wachtwoord_maxlengte", "updval", db_param("wachtwoord_minlengte"));
		}
		if (db_param("wachtwoord_maxlengte") < 7) {
			db_param("wachtwoord_maxlengte", "updval", 7);
		} elseif (db_param("wachtwoord_maxlengte") > 15) {
			db_param("wachtwoord_maxlengte", "updval", 15);
		}
		if (db_param("login_maxlengte") < 7) {
			db_param("login_maxlengte", "updval", 7);
		} elseif (db_param("login_maxlengte") > 15) {
			db_param("login_maxlengte", "updval", 15);
		}
	}

	echo("<div id='instellingenmuteren'>\n");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	
	foreach (db_param("", "lijst") as $row) {
		if (array_key_exists($row->Naam, $arrParam)) {
			$uitleg = htmlentities($arrParam[$row->Naam]);
			if (strlen($uitleg) < 5) {
			} elseif ($row->ParamType == "B") {
				printf("<label>%s</label><p><input type='checkbox' name='value_%d' value='1' %s></p>\n", $uitleg, $row->RecordID, checked($row->ValueNum));
			} elseif ($row->ParamType == "I") {
				printf("<label>%s</label><p><input type='number' class='inputnumber' name='value_%d' value=%d></p>\n", $uitleg, $row->RecordID, $row->ValueNum);
			} elseif ($row->ParamType == "F") {
				printf("<label>%s</label><p><input name='value_%d' value=%F size=8></p>\n", $uitleg, $row->RecordID, $row->ValueNum);
			} elseif (strlen($row->ValueChar) > 60) {
				printf("<label>%s</label><p><textarea cols=50 rows=10 name='value_%d'>%s</textarea></p>\n", $uitleg, $row->RecordID, $row->ValueChar);
			} else {
				printf("<label>%s</label><p><input type='text' class='inputtext' name='value_%d' value=\"%s\"></p>\n", $uitleg, $row->RecordID, $row->ValueChar);
			}
		} else {
			db_param($row->Naam, "delete");
		}
	}
	echo("<div class='clear'></div>\n");
	echo("<input class='knop' type='submit' value='Bewaren'>\n");
	
	echo("</form>\n");
	echo("</div>  <!-- Einde instellingenmuteren -->\n");
}

?>
