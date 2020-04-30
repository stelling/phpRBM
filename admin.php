<?php
include('./includes/standaard.inc');

if ((!isset($_SESSION['lidid']) or $_SESSION['lidid'] == 0) and isset($_COOKIE['password']) and strlen($_COOKIE['password']) > 5) {
	fnAuthenticatie(0);
}

if ($_SESSION['lidid'] == 0) {
	header("location: index.php");
}

if (!isset($_GET['op']) or ((new cls_lid())->aantal() == 0 and toegang($_GET['tp'], 0, 1) === false)) {
	$_GET['op'] = "";
}

if ($currenttab == "Logboek" or $currenttab == "Stamgegevens") {
	HTMLheader(1);
} else {
	HTMLheader(0);
}

$inst_login = new cls_Login();
if ($_GET['op'] == "deletelogin" and $_GET['tp'] == "Beheer logins") {
	$inst_login->delete($_GET['lidid']);
} elseif ($_GET['op'] == "unlocklogin" and $_GET['tp'] == "Beheer logins") {
	$inst_login->update($_GET['lidid'], "FouteLogin", 0);
	printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $currenttab);
} elseif (isset($_POST['tabpage_nw']) and strlen($_POST['tabpage_nw']) > 0 and $_GET['tp'] == "Autorisatie") {
	(new cls_Authorisation())->add($_POST['tabpage_nw']);
} elseif ($_GET['op'] == "deleteautorisatie" and $_GET['tp'] == "Autorisatie") {
	(new cls_Authorisation())->delete($_GET['recid']);
	printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $currenttab);
} elseif ($_GET['op'] == "changeaccess" and $_GET['tp'] == "Autorisatie") {
	foreach((new cls_Authorisation())->lijst() as $row) {
		$vn = sprintf("toegang%d", $row->RecordID);
		if (isset($_POST[$vn])) {
			(new cls_Authorisation())->update($row->RecordID, "Toegang", $_POST[$vn]);
		}
	}
} elseif ($_GET['op'] == "uploaddata") {
	$inst_lb = new cls_Logboek();
	if (isset($_FILES['SQLupload']['tmp_name']) and strlen($_FILES['SQLupload']['tmp_name']) > 3) {
		(new cls_db_base())->setcharset();
		$queries = file_get_contents($_FILES['SQLupload']["tmp_name"]);
		if ($queries !== false) {
			$sp = strpos($queries, "DROP");
			if ($sp > 0) {
				$queries = substr($queries, $sp);
			} else {
				$sp = strpos($queries, "DELETE");
				if ($sp > 0) {
					$queries = substr($queries, $sp);
				}
			}
			$mess = "Bestand is succesvol ge-upload.";
			$inst_lb->add($mess, 9, 0, 1);
			$mess = "";
			if (strpos($queries, TABLE_PREFIX) === false) {
				$mess = sprintf("In het upload bestand komt de juiste table name prefix (%s) niet voor. Dit bestand wordt niet verwerkt.", TABLE_PREFIX);
			} elseif (strpos($queries, TABLE_PREFIX . "Lid") === FALSE and (new cls_Lid())->aantal() < 5) {
				$mess = sprintf("De verplichte tabel '%sLid' zit niet in deze upload. Dit bestand wordt niet verwerkt.", TABLE_PREFIX);
			} elseif (strpos($queries, TABLE_PREFIX . "Lidond") === FALSE and (new cls_Lidond())->aantal() < 5) {
				$mess = sprintf("De verplichte tabel '%sLidond' zit niet in deze upload. Dit bestand wordt niet verwerkt.", TABLE_PREFIX);
			} elseif ((new cls_db_base())->execsql($queries) !== true) {
				$mess = "Bestand is in de database verwerkt.";
				db_onderhoud(1);
				fnMaatwerkNaUpload();
				printf("<script>setTimeout(\"location.href='%s';\", 30000);</script>\n", $_SERVER['PHP_SELF']);
			}
			if (strlen($mess) > 0) {
				$inst_lb->add($mess, 9, 0, 1);
			}
		}
	} else {
		$mess = sprintf("Er is iets mis gegaan tijdens het uploaden. Error: %s. Klik <a href='http://nl3.php.net/manual/en/features.file-upload.errors.php'>hier</a> voor uitleg van de code.", $_FILES['SQLupload']['error']);
		$inst_lb->add($mess, 2, 0, 1);
	}
	$inst_lb = null;
} elseif ($_GET['op'] == "afmeldenwijz" and $_GET['tp'] == "Downloaden wijzigingen") {
	(new cls_interface())->afmelden();
} elseif ($_GET['op'] == "backup") {
	db_backup();
} elseif ($_GET['op'] == "FreeBackupFiles") {
	fnFreeBackupFiles();
} elseif ($_GET['op'] == "logboekopschonen") {
	db_logboek("opschonen");
} elseif ($_GET['op'] == "loggingdebugopschonen") {
	db_logboek("debugopschonen");
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

if ($currenttab == "Beheer logins" and toegang($currenttab, 1, 1)) {
	fnBeheerLogins();
	
} elseif ($currenttab == "Autorisatie" and toegang($currenttab, 1, 1)) {
	echo("<div id='lijst'>\n");
	printf("<form name='formauth' method='post' action='%s?tp=%s&amp;op=changeaccess'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<table>\n");
	echo("<tr><th></th><th>Onderdeel</th><th>Toegankelijk voor</th><th>Ingevoerd</th></tr>\n");
	$ondrows = db_Onderdeel("lijst");
	foreach(db_authorisation("lijst") as $row) {
		$del = sprintf("<a href='%s?tp=%s&amp;op=deleteautorisatie&amp;recid=%d'><img src='images/del.png' title='Verwijder record'></a>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
		$selectopt = sprintf("<option value=-1%s>Alleen webmasters</option>\n", checked($row->Toegang, "option", -1));
		$selectopt .= sprintf("<option value=0%s>Iedereen</option>\n", checked($row->Toegang, "option", 0));
		foreach($ondrows as $ond) {
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
	
	echo(fnDisplayTable(db_authorisation("autorisatiesperonderdeel"), "", "Overzicht autorisaties per onderdeel"));
	
} elseif ($currenttab == "Instellingen" and toegang($currenttab, 1, 1)) {
	fnInstellingen();
	
} elseif ($currenttab == "Stamgegevens" and toegang($currenttab, 1, 1)) {
	fnStamgegevens();
	
} elseif ($currenttab == "Uploaden data" and (db_lid("aantal") == 0 or toegang($currenttab, 1, 1))) {
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
	
} elseif ($currenttab == "Downloaden wijzigingen" and toegang($currenttab, 1, 1)) {
	$copytext = "";
	echo("<h2>Wijzigen op de website, te verwerken in de Access database.</h2>");
	printf("<form name='formdownload' method='post' action='%s?tp=%s&amp;op=downloadwijz'>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	$rows = db_interface("lijst");
	if (count($rows) > 0) {
		echo(fnDisplayTable($rows, "", "", 1));
		foreach ($rows as $row) {
			$copytext .= $row->SQL . "\n";
		}
	} else {
		echo("<p class='mededeling'>Er zijn geen wijzigingen die nog verwerkt moeten worden.</p>\n");
	}
		
	if (strlen($copytext) > 0) {
		echo("<h2>SQL-code voor in MS-Access, om te gebruiken:</h2>\n");
		printf("<textarea id='copywijzigingen' class='copypaste' rows=%d readonly>%s</textarea>\n", count($rows)+1, $copytext);
		echo("<p><button onClick='CopyFunction()'>Kopieer naar klembord</button>\n");
		printf("<input type='button' value='Wijzigingen afmelden' OnClick=\"location.href='%s?tp=%s&amp;op=afmeldenwijz'\"></p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	}
	
} elseif ($currenttab == "Onderhoud" and toegang($currenttab, 1, 1)) {
	
	if (isset($_POST['logboek_bewaartijd']) and $_POST['logboek_bewaartijd'] > 0) {
		(new cls_Parameter())->update("logboek_bewaartijd", $_POST['logboek_bewaartijd']);
	}
	if (isset($_POST['mailing_bewaartijd']) and $_POST['mailing_bewaartijd'] > 0) {
		(new cls_Parameter())->update("mailing_bewaartijd", $_POST['mailing_bewaartijd']);
	}
	
	db_createtables();
	db_onderhoud();

	echo("<div id='dbonderhoud'>\n");
	
	$query = sprintf("SELECT MAX(DatumTijd) AS LB FROM %sAdmin_activiteit WHERE TypeActiviteit=3;", TABLE_PREFIX);
	$laatstebackup = db_scalar($query);
	
	printf("<form method='post' name='frmOnderhoud' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=backup\"' value='Backup'>&nbsp;Maak een backup van de database. Laatste backup is op %s gemaakt.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $laatstebackup);
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=FreeBackupFiles\"' value='Vrijgeven backup-bestanden'>&nbsp;Geef de backup-bestanden vrij door middel van een chmod 0755.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=logboekopschonen\"' value='Logboek opschonen'>&nbsp;Verwijder alle records uit het logboek, die ouder dan <input type='number' class='inputnumber' name='logboek_bewaartijd' onChange='this.form.submit();' value=%d> maanden zijn.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $_SESSION['settings']['logboek_bewaartijd']);
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=loggingdebugopschonen\"' value='Eigen logging voor debugging opschonen'>&nbsp;Verwijder alle records uit het logboek, die onder jouw account voor debugging zijn toegevoegd.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=evenementenopschonen\"' value='Evenementen opschonen'>&nbsp;Opschonen verwijderde evenementen, die geen deelnemers meer hebben.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	if (db_mailing("aantalprullenbak") > 0) {
		printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=mailingsopschonen\"' value='Mailings opschonen'>&nbsp;Mailings, die langer dan <input type='number' name='mailing_bewaartijd' class='inputnumber' onChange='this.form.submit();' value=%d> maanden in de prullenbak zitten, definitief verwijderen.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $_SESSION['settings']['mailing_bewaartijd']);
	}
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=loginsopschonen\"' value='Logins opschonen'>&nbsp;Opschonen van logins die om diverse redenen niet meer nodig zijn.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=autorisatieopschonen\"' value='Autorisatie opschonen'>&nbsp;Verwijderen toegang waar alleen de webmaster toegang toe heeft en die ouder dan 3 maanden zijn.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	if (db_orderregel("aantal") > 0) {
		printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=orderregelsopschonen\"' value='Orderregels opschonen'>&nbsp;Opschonen van de orderregels van de bestellingen.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	}
	if (db_artikel("aantal") > 0) {
		printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=artikelenopschonen\"' value='Artikelen opschonen'>&nbsp;Opschonen van artikelen zonder bestellingen uit de webshop.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	}
	echo("</form>\n");
	echo("</div>  <!-- Einde dbonderhoud -->\n");
	
	$query = "SELECT Version() AS Version;";
	$vdb = (new cls_db_base())->versiedb();
	printf("<div id='versies'>PHP: %s / Database: %s</div>  <!-- Einde versies -->\n", substr(phpversion(), 0, 6), $vdb);
	
} elseif ($currenttab == "Logboek" and toegang($currenttab, 1, 1)) {
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
			printf("<option value=%d %s>%s</option>\n", $key, $s, htmlentities($val));
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
	
} elseif ($currenttab == "Info" and toegang($currenttab, 1, 1)) {
	phpinfo();
}

HTMLfooter();

function fnBeheerLogins() {
	
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
	
	$inst_login = new cls_Login();
	
	db_logins("uitloggen");
	$rows = db_logins("lijst", "", "", 0, $w, "", $ord);
		
	echo("<div id='beheerlogins'>\n");
	printf("<form name='filter' action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
	
	printf("<label>Naam/login</label><div class='waarde'><input type='text' name='NaamFilter' size=20 value='%s' onblur='form.submit();'></div>\n", $naamfilter);
	echo("<label>Sorteren op</label><div class='waarde'>");
	foreach($arrSort as $s) {
		if ($s == $sorteren) {$c=" checked"; } else { $c=""; }
		printf("<input type='radio'%2\$s name='Sorteren' value='%1\$s' onclick='this.form.submit();'>%1\$s\n", $s, $c);
	}
	if ($sortdesc) {$c = " checked";	} else {	$c = "";	}
	printf("&nbsp;<input type='checkbox' value='1' name='sortdesc'%s onclick='this.form.submit();'>&nbsp;Desc</div>\n", $c);
	$al = count($rows);
	if ($al > 1) {
		printf("<div class='waarde'>%d logins</div>\n", $al);
	}
	echo("</form>\n");
	echo("<div class='clear'></div>\n");
	
	$lnk_ek = sprintf("<a href='%s?op=deletelogin&amp;lidid=%%d'><img src='images/del.png' title='Verwijder login'></a>", $_SERVER['PHP_SELF']);
	if ($_SESSION['settings']['login_maxinlogpogingen'] > 0) {
		$lnk_lk = sprintf("<a href='%s?op=unlocklogin&amp;lidid=%%d' title='Reset foutieve logins'><img src='images/unlocked_01.png'></a>", $_SERVER['PHP_SELF']);
	} else {
		$lnk_lk = "";
	}
	
	echo(fnDisplayTable($rows, "", "", 0, $lnk_lk, "", "lijst", $lnk_ek));
	
	echo("</div>  <!-- Einde beheerlogins -->\n");
	
	// Nieuwe en gewijzigde logins in de tabel interface zetten en de tabel 'Lid' bijwerken.
	$inst_lid = new cls_Lid();
	$f = "IFNULL(L.LoginWebsite, '') <> IFNULL(Login.Login, '')";
	foreach ($inst_login->lijst($f) as $row) {
		$inst_lid->update($row->lnkNummer, "LoginWebsite", $row->Login);
	}
	$inst_lid = null;
	$inst_login = null;
}  #  fnBeheerLogins

function fnInstellingen() {
	
	$inst_p = new cls_Parameter();
	$inst_p->controle();
	$inst_p->vulsessie();
	$inst_p = null;

	$arrParam['db_backup_type'] = "Welke taballen moeten worden gebackuped? 1=interne phpRBM-tabellen, 2=tabellen uit Access, 3=beide.";
	$arrParam['db_backupsopschonen'] = "Na hoeveel dagen moeten back-ups automatisch verwijderd worden? 0 = nooit.";
	$arrParam['db_folderbackup'] = "In welke folder moet de backup worden geplaatst?";
	$arrParam['emailwebmaster'] = "Het e-mailadres van de webmaster.";
	$arrParam['kaderoverzichtmetfoto'] = "Moeten op het kaderoverzicht pasfoto's getoond worden?";
	$arrParam['toonpasfotoindiennietingelogd'] = "Mogen pasfoto's zichtbaar voor bezoekers (niet ingelogd) zijn?";
	$arrParam['meisjesnaamtonen'] = "Moeten de namen van leden ook de meisjesnaam bevatten?";
	$arrParam['WoonadresAnderenTonen'] = "Moet het woonadres van een lid ook aan andere leden worden getoond?";
	$arrParam['login_lidnrnodigbijaanvraag'] = "Moet een lid zijn of haar lidnummer opgeven als er een login aangevraagd wordt?";
	$arrParam['login_autounlock'] = "Na hoeveel minuten moet een gelockede login automatisch geunlocked worden? 0 = alleen handmatig unlocken.";
	$arrParam['login_beperkttotgroep'] = "Vul hier de RecordID's, gescheiden door een komma, van de groepen (zie tabel ONDERDL) in die toegang hebben. Leeg = alleen webmasters hebben toegang.";
	$arrParam['login_bewaartijd'] = "Het aantal maanden dat logins na het laatste gebruik bewaard blijven. Historie is direct aan het lid gekopppeld en wordt dus niet verwijderd. 0 = altijd bewaren.";
	$arrParam['login_geldigheidactivatie'] = "Hoelang in uren is een activatielink geldig? 0 = altijd.";
	$arrParam['login_bewaartijdnietgebruikt'] = "Het aantal dagen dat logins wordt bewaard, nadat het is aangevraagd en nog niet gebruikt is.";
	$arrParam['mailing_bevestigingbestelling'] = "Het nummer van de mailing die bij een bestelling verstuurd moet worden. 0 = geen.";
	$arrParam['mailing_bewakinginschrijving'] = "Het nummer van de mailing die als bevestiging van een inschrijving voor de bewaking verstuurd moet worden. 0 = geen.";
	$arrParam['menu_met_afdelingen'] = "Voor welke afdelingen moeten een tabblad worden gemaakt? (bij meerdere scheiden met een komma)";
	$arrParam['login_maxinlogpogingen'] = "Na hoeveel foutieve inlogpogingen moet het account geblokkeerd worden? 0 = nooit.";
	$arrParam['login_maxlengte'] = "De maximale lengte die een login mag zijn. Minimaal 7 en maximaal 20 invullen.";
	$arrParam['wachtwoord_minlengte'] = "De minimale lengte van een wachtwoord. Minimaal 7 en maximaal 15 invullen.";
	$arrParam['wachtwoord_maxlengte'] = "De maximale lengte van een wachtwoord. Minimaal 7 en maximaal 15 invullen.";
	$arrParam['naamwebsite'] = "Dit is de naam zoals deze in de titel en op elke pagina getoond wordt.";
	$arrParam['path_templates'] = "Waar staan de templates?";
	$arrParam['performance_trage_select'] = "Vanaf hoeveel seconden moet een select-statement in het logboek worden gezet. 0 = nooit.";
	$arrParam['termijnvervallendiplomasmailen'] = "Hoeveel maanden vooruit moeten leden een herinnering krijgen als een diploma gaat vervallen. 0 = geen herinnering sturen.";
	$arrParam['termijnvervallendiplomasmelden'] = "Hoeveel maanden vooruit en achteraf moeten vervallen diploma op het voorblad getoond worden.";
	$arrParam['toneninschrijvingenbewakingen'] = "Moeten bij de gegevens van een lid ook inschrijvingen voor bewakingen getoond worden?";
	$arrParam['tonentoekomstigebewakingen'] = "Moeten bij de gegevens van een lid ook toekomstige bewakingen getoond worden?";
	$arrParam['typemenu'] = "Hoe moet het menu eruit zien? 1 = per niveau een aparte regel, 2 = ��n menu met dropdown, 3 = ��n menu met dropdown en extra menu voor niveau 2.";
	$arrParam['uitleg_toestemmingen'] = "Welke tekst moet er bij het verlenen van de toestemmingen worden vermeld?";
	$arrParam['urlvereniging'] = "De URL van de website van de vereniging.";
	$arrParam['url_eigen_help'] = "Als een gebruiker op de help klikt wordt hier naar verwezen in plaats van de standaard help.";
	$arrParam['verjaardagenaantal'] = "Hoeveel verjaardagen moeten er maximaal in de verenigingsinfo worden getoond. Als er meerdere leden op dezelfde dag jarig zijn, wordt dit aantal overschreden.";
	$arrParam['verjaardagenvooruit'] = "Hoeveel dagen vooruit moeten de verjaardagen in de verenigingsinfo getoond worden?";
	
	$arrParam['zs_emailnieuwepasfoto'] = "Waar moet een nieuwe pasfoto naar toe gemaild worden?";
	$arrParam['zs_incl_beroep'] = "Is het veld 'Beroep' in de zelfservice beschikbaar?";
	$arrParam['zs_incl_bsn'] = "Is het veld 'Burgerservicenummer' in de zelfservice beschikbaar?";
	$arrParam['zs_incl_machtiging'] = "Is het veld 'Machtiging incasso afgegeven' in de zelfservice beschikbaar?";
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
	
	$specmailing = array("mailing_bewakinginschrijving", "mailing_bevestigingbestelling");
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$inst_p = new cls_Parameter();
		foreach (db_param("", "lijst") as $row) {
			$mess = "";
			$pvn = $row->Naam;
			if (array_key_exists($row->Naam, $arrParam) == true) {
				if ($pvn == "wachtwoord_minlengte") {
					if ($_POST[$pvn] < 7) {
						$_POST[$pvn] = 7;
					} elseif ($_POST[$pvn] > 15) {
						$_POST[$pvn] = 15;
					}
				} elseif ($pvn == "wachtwoord_maxlengte") {
					if ($_POST[$pvn] < 7) {
						$_POST[$pvn] = 7;
					} elseif ($_POST[$pvn] > 15) {
						$_POST[$pvn] = 15;
					}
				} elseif ($pvn == "login_maxlengte") {
					if ($_POST[$pvn] < 7) {
						$_POST[$pvn] = 7;
					} elseif ($_POST[$pvn] > 20) {
						$_POST[$pvn] = 20;
					}
				}
				if ($_POST['wachtwoord_minlengte'] > $_POST['wachtwoord_maxlengte']) {
					$_POST['wachtwoord_minlengte'] = $_POST['wachtwoord_maxlengte'];
				}
				
				if (isset($_POST[$pvn])) {
					$_POST[$pvn] = str_replace("\"", "'", $_POST[$pvn]);
				}
				if (in_array($row->Naam, array("beperktotgroep", "muteerbarememos"))) {
					$_POST[$pvn] = str_replace(" ", "", $_POST[$pvn]);
				} elseif (in_array($row->Naam, $specmailing) and $_POST[$pvn] > 0 and db_mailing("exist", $_POST[$pvn]) == false) {
					$_POST[$pvn] = 0;
					$mess = sprintf("Parameter '%s' wordt 0 gemaakt, omdat de ingevoerde mailing niet (meer) bestaat. ", $row->Naam);
				} elseif ($row->Naam == "typemenu" and (strlen($_POST[$pvn]) == 0 or $_POST[$pvn] < 1 or $_POST[$pvn] > 3)) {
					$_POST[$pvn] = 1;
					$mess = sprintf("Parameter '%s' wordt 1 gemaakt, omdat deze alleen 1, 2 of 3 mag zijn. ", $row->Naam);
				} elseif (startwith($row->Naam, "url") and isset($_POST[$pvn]) and !startwith($_POST[$pvn], "http")) {
					$_POST[$pvn] = "https://" . $_POST[$pvn];
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
					(new cls_Logboek())->add($mess, 13, 0, 1);
				}
				if (strlen($arrParam[$row->Naam]) > 2) {
					$inst_p->update($row->Naam, $_POST[$pvn]);
				}
			}
		}
		
		$p = $_SESSION['settings']['db_folderbackup'];
		if (strlen($p) < 5 or !is_dir($p)) {
			$inst_p->update("db_folderbackup", __DIR__ . "/backups/");
		} elseif (substr($p, -1) != "/") {
			$inst_p->update("db_folderbackup", $p . "/");
		}
		
		$p = $_SESSION['settings']['path_templates'];
		if (strlen($p) < 5 or !is_dir($p)) {
			$instr_p->update("path_templates", __DIR__ . "/templates/");
		} elseif (substr($p, -1) != "/") {
			$inst_p->update("path_templates", $p . "/");
		}
		$inst_p = null;
		
	}

	echo("<div id='instellingenmuteren'>\n");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	
	foreach (db_param("", "lijst") as $row) {
		if (array_key_exists($row->Naam, $arrParam)) {
			$uitleg = htmlent($arrParam[$row->Naam]);
			echo("<div class='invoerblok'>\n");
			if (strlen($row->ValueChar) > 60 and $row->ParamType="T") {
				printf("<label>%s</label><p><textarea cols=68 rows=2 name='%s'>%s</textarea></p>\n", $uitleg, $row->Naam, $row->ValueChar);
			} else {
				printf("<label>%s</label><p><input name='%s' ", $uitleg, $row->Naam);
				if ($row->ParamType == "B") {
					printf("type='checkbox' value='1' %s></p>\n", checked($row->ValueNum));
				} elseif ($row->ParamType == "I") {
					printf("type='number' class='inputnumber' value=%d></p>\n", $row->ValueNum);
				} elseif ($row->ParamType == "F") {
					printf("value=%F size=8></p>\n", $row->ValueNum);
				} else {
					printf("type='text' class='inputtext' value=\"%s\"></p>\n", $row->ValueChar);
				}
			}
			echo("</div> <!-- Einde invoerblok -->\n");
		}
	}
	echo("<div id='opdrachtknoppen'>\n");
	echo("<input class='knop' type='submit' value='Bewaren'>\n");
	echo("</div>  <!-- Einde opdrachtknoppen -->\n");
	
	echo("</form>\n");
	echo("</div>  <!-- Einde instellingenmuteren -->\n");
} # fnInstellingen

function fnStamgegevens() {
		
	$rows = (new cls_db_base("Diploma"))->basislijst();
	printf("<p>%s</p>", fnDisplayTable($rows, "", "Basislijst Diploma's", 0, "", "", "lijst", ""));
	
	$rows = (new cls_db_base("Functie"))->basislijst();
	printf("<p>%s</p>", fnDisplayTable($rows, "", "Basislijst Functies", 0, "", "", "lijst", ""));
	
	$rows = (new cls_db_base("Groep"))->basislijst();
	printf("<p>%s</p>", fnDisplayTable($rows, "", "Basislijst Groepen", 0, "", "", "lijst", ""));
	
	$rows = (new cls_db_base("Onderdl"))->basislijst();
	printf("<p>%s</p>", fnDisplayTable($rows, "", "Basislijst Onderdelen", 0, "", "", "lijst", ""));
	
} # fnStamgegevens

?>

<script>
	function CopyFunction() {
		let textarea = document.getElementById("copywijzigingen");
		textarea.select();
		document.execCommand('copy');
		alert("De wijzigingen zijn naar het klembord gekopieerd.");
	}
</script>
