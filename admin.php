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

$i_login = new cls_Login();
if ($_GET['op'] == "deletelogin" and $_GET['tp'] == "Beheer logins") {
	$i_login->delete($_GET['lidid']);
} elseif ($_GET['op'] == "unlocklogin" and $_GET['tp'] == "Beheer logins") {
	$i_login->update($_GET['lidid'], "FouteLogin", 0);
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
	$i_lb = new cls_Logboek();
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
			$i_lb->add($mess, 9, 0, 1);
			$mess = "";
			if (strpos($queries, TABLE_PREFIX) === false) {
				$mess = sprintf("In het upload bestand komt de juiste table name prefix (%s) niet voor. Dit bestand wordt niet verwerkt.", TABLE_PREFIX);
			} elseif (strpos($queries, TABLE_PREFIX . "Lid") === FALSE and (new cls_Lid())->aantal() < 5) {
				$mess = sprintf("De verplichte tabel '%sLid' zit niet in deze upload. Dit bestand wordt niet verwerkt.", TABLE_PREFIX);
			} elseif (strpos($queries, TABLE_PREFIX . "Lidond") === FALSE and (new cls_Lidond())->aantal() < 5) {
				$mess = sprintf("De verplichte tabel '%sLidond' zit niet in deze upload. Dit bestand wordt niet verwerkt.", TABLE_PREFIX);
			} else {
				
				while (strlen($queries) > 1) {
					$sp = strpos($queries, "DROP TABLE", 12);
					if ($sp === false) {
						$query = $queries;
						$queries = "";
					} else {
						$query = substr($queries, 0, $sp);
						$queries = substr($queries, $sp);
					}
//					debug($query . "<br><br>\n");
					(new cls_db_base())->execsql($query);				
				}
				
				$mess = "Bestand is in de database verwerkt.";
				db_onderhoud(1);
				fnMaatwerkNaUpload();
				printf("<script>setTimeout(\"location.href='%s';\", 30000);</script>\n", $_SERVER['PHP_SELF']);
			}
			if (strlen($mess) > 0) {
				$i_lb->add($mess, 9, 0, 1);
			}
		}
	} else {
		$mess = sprintf("Er is iets mis gegaan tijdens het uploaden. Error: %s. Klik <a href='http://nl3.php.net/manual/en/features.file-upload.errors.php'>hier</a> voor uitleg van de code.", $_FILES['SQLupload']['error']);
		$i_lb->add($mess, 2, 0, 1);
	}
	$i_lb = null;
} elseif ($_GET['op'] == "afmeldenwijz" and $_GET['tp'] == "Downloaden wijzigingen") {
	(new cls_interface())->afmelden();
	
} elseif ($_GET['op'] == "deleteint" and $_GET['tp'] == "Downloaden wijzigingen") {
	(new cls_interface())->delete($_GET['recid']);
	
} elseif ($_GET['op'] == "backup") {
	db_backup($_SESSION['settings']['db_backup_type']);
} elseif ($_GET['op'] == "FreeBackupFiles") {
	fnFreeBackupFiles();
} elseif ($_GET['op'] == "logboekopschonen") {
	(new cls_Logboek())->opschonen();
} elseif ($_GET['op'] == "loggingdebugopschonen") {
	(new cls_logboek())->debugopschonen();
} elseif ($_GET['op'] == "onderdelenopschonen") {
	(new cls_Onderdeel())->opschonen();
} elseif ($_GET['op'] == "lidondopschonen") {
	(new cls_Lidond())->opschonen();
} elseif ($_GET['op'] == "mailingsopschonen") {
	(new cls_Mailing())->opschonen();
} elseif ($_GET['op'] == "evenementenopschonen") {
	(new cls_Evenement())->opschonen();
} elseif ($_GET['op'] == "loginsopschonen") {
	(new cls_Login())->opschonen();
} elseif ($_GET['op'] == "autorisatieopschonen") {
	(new cls_Authorisation())->opschonen();
} elseif ($_GET['op'] == "orderregelsopschonen") {
	(new cls_Orderregel())->opschonen();
} elseif ($_GET['op'] == "artikelenopschonen") {
	(new cls_Artikel())->opschonen();
}

if ($currenttab == "Beheer logins" and toegang($currenttab, 1, 1)) {
	fnBeheerLogins();
	
} elseif ($currenttab == "Autorisatie" and toegang($currenttab, 1, 1)) {
	$i_auth = new cls_Authorisation();
	echo("<div id='filter'>\n");
	echo("<label>Onderdeel bevat</label><input name='tbOndFilter' id='tbOndFilter' OnKeyUp=\"fnFilter('lijst', 'tbOndFilter', 1);\">\n");
	echo("</div> <!-- Einde filter -->\n");
	echo("<div id='lijst'>\n");
	printf("<form method='post' action='%s?tp=%s&amp;op=changeaccess'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<table id='lijst'>\n");
	echo("<tr><th></th><th>Onderdeel</th><th>Toegankelijk voor</th><th>Ingevoerd</th><th>Laatst gebruikt</th></tr>\n");
	$ondrows = (new cls_Onderdeel())->lijst(0, "O.`Type`<>'T'");
	$authrows = $i_auth->lijst();
	foreach($authrows as $row) {
		$del = sprintf("<a href='%s?tp=%s&amp;op=deleteautorisatie&amp;recid=%d'><img src='%s' title='Verwijder record'></a>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID, BASE64_VERWIJDER);
		$selectopt = sprintf("<option value=-1%s>Alleen webmasters</option>\n", checked($row->Toegang, "option", -1));
		$selectopt .= sprintf("<option value=0%s>Iedereen</option>\n", checked($row->Toegang, "option", 0));
		foreach($ondrows as $ond) {
			$selectopt .= sprintf("<option value=%d%s>%s</option>\n", $ond->RecordID, checked($row->Toegang, "option", $ond->RecordID), htmlentities($ond->Naam));
		}
		printf("<tr>\n<td>%s</td>\n<td>%s</td>\n<td><select name='toegang%d' onchange='this.form.submit();'>\n%s</select></td>\n<td>%s</td><td>%s</td></tr>\n", $del, $row->Tabpage, $row->RecordID, $selectopt, strftime("%e %B %Y", strtotime($row->Ingevoerd)), strftime("%e %B %Y", strtotime($row->LaatstGebruikt)));
	}
	$optionstab = "<option value=''>Selecteer ...</option>\n";
	foreach ($i_auth->lijst("DISTINCT") as $row) {
		$optionstab .= sprintf("<option value='%1\$s'>%1\$s</option>\n", $row->Tabpage);
	}
	printf("<tr><td><img src='images/star.png' alt='Ster' title='Nieuw record'></td><td><select name='tabpage_nw' onChange='this.form.submit();'>%s</select></td><td></td><td></td></tr>\n", $optionstab);
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde lijst -->\n");
	
	echo(fnDisplayTable($i_auth->autorisatiesperonderdeel(), "", "Overzicht autorisaties per onderdeel"));
	$i_auth = null;
	
} elseif ($currenttab == "Stukken" and toegang($currenttab, 1, 1)) {
	fnStukken();
} elseif ($currenttab == "Instellingen" and toegang($currenttab, 1, 1)) {
	fnInstellingen();
	
} elseif ($currenttab == "Stamgegevens" and toegang($currenttab, 1, 1)) {
	fnStamgegevens();
	
} elseif ($currenttab == "Uploaden data" and ((new cls_Lid())->aantal() == 0 or toegang($currenttab, 1, 1))) {
	$aantal = (new cls_Interface())->aantal("IFNULL(Afgemeld, '1900-01-01') < '2011-01-01'");
	if ($aantal > 0) {
		printf("<p class='mededeling'>Er staan %d wijzigingen te wachten om verwerkt te worden. Het is niet verstandig om een upload te doen.</p>", $aantal);
	}
	$aantal = (new cls_Login())->aantal("Ingelogd=1");
	if ($aantal > 1) {
		printf("<p class='mededeling'>Er staan zijn momenteel %d gebruikers ingelogd. Het is niet verstandig om een upload te doen.</p>", $aantal);
	}
	echo("<div id='formulier'>\n");
	printf("<form name='formupload' method='post' action='%s?tp=%s&amp;op=uploaddata' enctype='multipart/form-data'>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	echo("<label>Bestand</label><input type='file' name='SQLupload'><input type='submit' value='Verwerk'>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde formulier -->\n");

} elseif ($currenttab == "Downloaden wijzigingen" and toegang($currenttab, 1, 1)) {
	$copytext = "";
	(new cls_lidond())->autogroepenbijwerken();
	printf("<form name='formdownload' method='post' action='%s?tp=%s&op=afmeldenwijz'>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	$linklk = sprintf("<a href='/admin.php?op=deleteint&amp;recid=%%d&amp;tp=%s'><img src='" . BASE64_VERWIJDER . "' title='Verwijder record'></a>", urlencode($_GET['tp']));
	$rows = (new cls_Interface())->lijst();
	if (count($rows) > 0) {
		echo("<h2>Wijzigen op de website, te verwerken in de Access database.</h2>");
		echo(fnDisplayTable($rows, "", "", 1, $linklk));
		foreach ($rows as $row) {
			$copytext .= $row->SQL . "\n";
		}
	} else {
		echo("<p class='mededeling'>Er zijn geen wijzigingen die nog verwerkt moeten worden.</p>\n");
	}
		
	if (strlen($copytext) > 0) {
		echo("<h2>SQL-code, te gebruiken in MS-Access:</h2>\n");
		printf("<textarea id='copywijzigingen' class='copypaste' rows=%d readonly>%s</textarea>\n", count($rows)+1, $copytext);
		echo("<button onClick='CopyFunction()'>Kopieer naar klembord</button>\n");
		echo("<button type='submit'>Wijzigingen afmelden</button>\n");
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
	
	$f = "TypeActiviteit=3";
	$laatstebackup = (new cls_Logboek())->max("DatumTijd", $f);
	
	printf("<form method='post' name='frmOnderhoud' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=backup\"' value='Backup'><p>Maak een backup van de database. Laatste backup is op %s gemaakt.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $laatstebackup);
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=FreeBackupFiles\"' value='Vrijgeven backup-bestanden'><p>Geef de backup-bestanden vrij door middel van een chmod 0755.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=logboekopschonen\"' value='Logboek opschonen'><p>Verwijder alle records uit het logboek, die ouder dan <input type='number' name='logboek_bewaartijd' onChange='this.form.submit();' value=%d> maanden zijn.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $_SESSION['settings']['logboek_bewaartijd']);
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=loggingdebugopschonen\"' value='Eigen logging voor debugging opschonen'><p>Verwijder alle records uit het logboek, die onder jouw account voor debugging zijn toegevoegd.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=onderdelenopschonen\"' value='Onderdelen opschonen'><p>Opschonen onderdelen die vervallen en niet meer in gebruik zijn.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=lidondopschonen\"' value='Leden bij onderdelen opschonen'><p>Opschonen op basis van historie en 'Alleen leden'.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));

	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=mailingsopschonen\"' value='Mailings opschonen'><p>Mailings, die langer dan <input type='number' name='mailing_bewaartijd' onChange='this.form.submit();' value=%d> maanden in de prullenbak zitten, definitief verwijderen.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $_SESSION['settings']['mailing_bewaartijd']);
	
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=evenementenopschonen\"' value='Evenementen opschonen'><p>Opschonen verwijderde evenementen, die geen deelnemers meer hebben.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));

	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=loginsopschonen\"' value='Logins opschonen'><p>Opschonen van logins die om diverse redenen niet meer nodig zijn.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=autorisatieopschonen\"' value='Autorisatie opschonen'><p>Verwijderen toegang waar alleen de webmaster toegang toe heeft en die ouder dan 3 maanden zijn.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	if ((new cls_Orderregel())->aantal() > 0) {
		printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=orderregelsopschonen\"' value='Orderregels opschonen'><p>Opschonen van de orderregels van de bestellingen.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	}
	if ((new cls_Artikel())->aantal() > 0) {
		printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=artikelenopschonen\"' value='Artikelen opschonen'><p>Opschonen van artikelen zonder bestellingen en zonder voorraadboekingen.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	}
	echo("</form>\n");
	echo("</div>  <!-- Einde dbonderhoud -->\n");
	
	printf("<div id='versies'>PHP: %s / Database: %s</div>  <!-- Einde versies -->\n", substr(phpversion(), 0, 6), (new cls_db_base())->versiedb());
	
} elseif ($currenttab == "Logboek" and toegang($currenttab, 1, 1)) {
	$i_lb = new cls_logboek();
	
	$arrSort[] = "Datum en tijd;RecordID";
	$arrSort[] = "Omschrijving;Omschrijving";
	$arrSort[] = "Type;Type";
	$arrSort[] = "Script / Functie;Script / Functie";
	$arrSort[] = "Ingelogd Lid;Ingelogd Lid";
	$arrSort[] = "IP adres;A.IP_adres";
	
	$ord = fnOrderBy($arrSort);
	
	if (!isset($_POST['tbLidFilter']) or strlen($_POST['tbLidFilter']) == 0) {
		$_POST['tbLidFilter'] = "";
	}
	if (!isset($_POST['typefilter']) or strlen($_POST['typefilter']) == 0) {
		$_POST['typefilter'] = -1;
	}
	if (!isset($_POST['aantalrijen']) or $_POST['aantalrijen'] < 2) {
		$_POST['aantalrijen'] = 100;
	}
	
	$rows = $i_lb->lijst($_POST['typefilter'], 0, 0, "", $ord, $_POST['aantalrijen']);
	
	echo("<div id='filter'>\n");
	printf("<form method='post' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	
	echo("<label>Omschrijving bevat</label><input type='text' id='tbOmsFilter' OnKeyUp=\"fnFilterTwee('logboek', 'tbOmsFilter', 'tbIPfilter', 1, 5, 'tbLidFilter', 4);\">");
	
	echo("<label>Ingelogd lid bevat</label><input type='text' id='tbLidFilter' OnKeyUp=\"fnFilterTwee('logboek', 'tbOmsFilter', 'tbIPfilter', 1, 5, 'tbLidFilter', 4);\">\n");
	
	echo("<label>Filter op type</label><select name='typefilter' onchange='this.form.submit();'>\n");
	echo("<option value=-1>Alle</option>\n");
	foreach ($TypeActiviteit as $key => $val) {
		printf("<option value=%d %s>%s</option>\n", $key, checked($key, "option", $_POST['typefilter']), htmlentities($val));
	}
	echo("</select>\n");
	
	echo("<label>IP-adres bevat</label><input type='text' id='tbIPfilter' name='tbIPfilter' OnKeyUp=\"fnFilterTwee('logboek', 'tbOmsFilter', 'tbIPfilter', 1, 6, 'tbLidFilter', 4);\">\n");
	$options = "";
	foreach (array(25, 100, 250, 750, 1500, 3000, 10000) as $a) {
		$options .= sprintf("<option value=%d %s>%s</option>\n", $a, checked($a, "option", $_POST['aantalrijen']), number_format($a, 0, ",", "."));
	}
	printf("<label>Max. aantal rijen</label><select name='aantalrijen' OnChange='this.form.submit();'>%s</select>\n", $options);
	echo("</form>\n");
	
	if (count($rows) > 1) {
		printf("<p class='aantrecords'>%s rijen</p>\n", number_format(count($rows), 0, ",", "."));
	}
	echo("</div>  <!-- Einde filter -->\n");
	
	echo(fnDisplayTable($rows, "", "", 0, "", "", "logboek", "", "", 0, $arrSort));
	
	$i_lb = null;
	
} elseif ($currenttab == "Info" and toegang($currenttab, 1, 1)) {
	phpinfo();
}

HTMLfooter();

function fnBeheerLogins() {
	
	$i_login = new cls_Login();
	
	$arrSort[0] = "Login;Login";
	$arrSort[1] = "Naam lid;Achternaam";
	$arrSort[2] = "Woonplaats;Woonplaats";
	$arrSort[3] = "Lidnr;Lidnr";
	$arrSort[4] = "E-mail;E-mail";
	$arrSort[5] = "Ingevoerd;Login.Ingevoerd";
	$arrSort[6] = "Laatste login;LastLogin";
	$arrSort[7] = "Status;Status";
	
	$ord = fnOrderBy($arrSort);
	
	$i_login->uitloggen();
	$rows = $i_login->lijst("", $ord);
	
	echo("<div id='filter'>\n");
	printf("<form action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
	echo("<label>Naam/login bevat</label><input type='text' name='tbNaamFilter' id='tbNaamFilter' OnKeyUp=\"fnFilter('lijst', 'tbNaamFilter', 1, 2);\">\n");
	
	if (count($rows) > 2) {
		printf("<p class='aantrecords'>%d logins</p>", count($rows));
	}
	echo("</form>\n");
	echo("</div>\n");
	echo("<div class='clear'></div>\n");
	
	echo("<div id='beheerlogins'>\n");
	$lnk_ek = sprintf("<a href='%s?op=deletelogin&amp;lidid=%%d'><img src='" . BASE64_VERWIJDER . "' title='Verwijder login'></a>", $_SERVER['PHP_SELF']);
	if ($_SESSION['settings']['login_maxinlogpogingen'] > 0) {
		$lnk_lk = sprintf("<a href='%s?op=unlocklogin&amp;lidid=%%d' title='Reset foutieve logins'><img src='images/unlocked_01.png'></a>", $_SERVER['PHP_SELF']);
	} else {
		$lnk_lk = "";
	}
	
	echo(fnDisplayTable($rows, "", "", 0, $lnk_lk, "", "lijst", $lnk_ek, "", 0, $arrSort));
	
	echo("</div>  <!-- Einde beheerlogins -->\n");
	
	// Nieuwe en gewijzigde logins in de tabel interface zetten en de tabel 'Lid' bijwerken.
	$f = "IFNULL(L.LoginWebsite, '') <> IFNULL(Login.Login, '')";
	foreach ($i_login->lijst($f) as $row) {
		(new cls_Lid())->update($row->lnkLidID, "LoginWebsite", $row->Login);
	}
	$i_login = null;
	
}  #  fnBeheerLogins

function fnInstellingen() {
	
	$i_p = new cls_Parameter();
	$i_p->controle();
	$i_p->vulsessie();
	$i_p = null;

	$arrParam['db_backup_type'] = "Welke tabellen moeten worden gebackuped?";
	$arrParam['db_backupsopschonen'] = "Na hoeveel dagen moeten back-ups automatisch verwijderd worden? 0 = nooit.";
	$arrParam['db_folderbackup'] = "In welke folder moet de backup worden geplaatst?";
	$arrParam['emailwebmaster'] = "Het e-mailadres van de webmaster.";
	$arrParam['kaderoverzichtmetfoto'] = "Moeten op het kaderoverzicht pasfoto's getoond worden?";
	$arrParam['toonpasfotoindiennietingelogd'] = "Mogen pasfoto's zichtbaar voor bezoekers (niet ingelogd) zijn?";
	$arrParam['muteerbarememos'] = "Welke soorten memo's zijn in gebruik? Bij meerdere scheiden door een komma.";
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
	$arrParam['path_pasfoto'] = "Waar staan de pasfotos?";
	$arrParam['performance_trage_select'] = "Vanaf hoeveel seconden moet een select-statement in het logboek worden gezet. 0 = nooit.";
	$arrParam['termijnvervallendiplomasmailen'] = "Hoeveel maanden vooruit moeten leden een herinnering krijgen als een diploma gaat vervallen. 0 = geen herinnering sturen.";
	$arrParam['termijnvervallendiplomasmelden'] = "Hoeveel maanden vooruit en achteraf moeten vervallen diploma op het voorblad getoond worden.";
	$arrParam['toneninschrijvingenbewakingen'] = "Moeten bij de gegevens van een lid ook inschrijvingen voor bewakingen getoond worden?";
	$arrParam['tonentoekomstigebewakingen'] = "Moeten bij de gegevens van een lid ook toekomstige bewakingen getoond worden?";
	$arrParam['typemenu'] = "Welk menu moet worden gebruikt?";
	$arrParam['urlvereniging'] = "De URL van de website van de vereniging.";
	$arrParam['url_eigen_help'] = "Als een gebruiker op de help klikt wordt hier naar verwezen in plaats van de standaard help.";
	$arrParam['verjaardagenaantal'] = "Aantal verjaardagen dat maximaal in de verenigingsinfo wordt getoond. Als er meerdere leden op dezelfde dag jarig zijn, wordt dit aantal overschreden.";
	$arrParam['verjaardagenvooruit'] = "Hoeveel dagen vooruit moeten de verjaardagen in de verenigingsinfo getoond worden?";
	
	$arrParam['zs_emailnieuwepasfoto'] = "Waar moet een nieuwe pasfoto naar toe gemaild worden?";
	$arrParam['zs_incl_machtiging'] = "Is het veld 'Machtiging incasso afgegeven' in de zelfservice beschikbaar?";
	$arrParam['zs_opzegtermijn'] = "De opzegtermijn van de vereniging in maanden.";
	$arrParam['zs_voorwaardenbestelling'] = "Deze regel wordt bij de online-bestellingen in de zelfservice vermeld.";
	$arrParam['zs_voorwaardeninschrijving'] = "Deze regel wordt bij de inschrijving als voorwaarde voor de inschrijving voor de bewaking vemeld.";
	
	$specmailing = array("mailing_bewakinginschrijving", "mailing_bevestigingbestelling");
	$i_p = new cls_Parameter();
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		foreach ($i_p->lijst() as $row) {
			$mess = "";
			$pvn = $row->Naam;
			if (isset($arrParam[$row->Naam])) {
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
				if (in_array($row->Naam, array("beperktotgroep", "zs_muteerbarememos", "menu_met_afdelingen"))) {
					$_POST[$pvn] = str_replace(";", ",", $_POST[$pvn]);
					$_POST[$pvn] = str_replace("'", "", $_POST[$pvn]);
				} elseif (in_array($row->Naam, $specmailing) and $_POST[$pvn] > 0 and (new cls_Mailing())->bestaat($_POST[$pvn]) == false) {
					$_POST[$pvn] = 0;
					$mess = sprintf("Parameter '%s' wordt 0 gemaakt, omdat de ingevoerde mailing niet (meer) bestaat. ", $row->Naam);
				} elseif (startwith($row->Naam, "url") and isset($_POST[$pvn]) and strlen($_POST[$pvn]) > 3 and startwith($_POST[$pvn], "http") == false) {
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
				$i_p->update($row->Naam, $_POST[$pvn]);
			}
		}
		
		$p = $_SESSION['settings']['db_folderbackup'];
		if (strlen($p) < 5 or !is_dir($p)) {
			$i_p->update("db_folderbackup", BASEDIR . "/backups/");
		} elseif (substr($p, -1) != "/") {
			$i_p->update("db_folderbackup", $p . "/");
		}
		
		$p = $_SESSION['settings']['path_pasfoto'];
		if (strlen($p) < 5 or !is_dir($p)) {
			$i_p->update("path_pasfoto", BASEDIR . "/pasfoto/");
		} elseif (substr($p, -1) != "/") {
			$i_p->update("path_pasfoto", $p . "/");
		}
		
		$p = $_SESSION['settings']['path_templates'];
		if (strlen($p) < 5 or !is_dir($p)) {
			$i_p->update("path_templates", BASEDIR . "/templates/");
		} elseif (substr($p, -1) != "/") {
			$i_p->update("path_templates", $p . "/");
		}
	}

	echo("<div id='instellingenmuteren'>\n");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	
	foreach ($i_p->lijst() as $row) {
		if (isset($arrParam[$row->Naam])) {
			$uitleg = htmlent($arrParam[$row->Naam]);
			if (strlen($row->ValueChar) > 60 and $row->ParamType="T") {
				printf("<label>%s</label><textarea cols=68 rows=2 name='%s'>%s</textarea>\n", $uitleg, $row->Naam, $row->ValueChar);
			} elseif ($row->Naam == "db_backup_type") {
				printf("<label>%s</label><select name='%s'>", $uitleg, $row->Naam);
				foreach (ARRTYPEBACKUP as $key => $val) {
					printf("<option value=%d %s>%s</option>\n", $key, checked($row->ValueNum, "option", $key), $val);
				}
				echo("</select>\n");
			} elseif ($row->Naam == "typemenu") {
				printf("<label>%s</label><select name='%s'>", $uitleg, $row->Naam);
				foreach (ARRTYPEMENU as $key => $val) {
					printf("<option value=%d %s>%s</option>\n", $key, checked($row->ValueNum, "option", $key), $val);
				}
				echo("</select>\n");
			} else {
				printf("<label>%s</label><input name='%s' ", $uitleg, $row->Naam);
				if ($row->ParamType == "B") {
					printf("type='checkbox' value='1' %s>\n", checked(intval($row->ValueNum)));
				} elseif ($row->ParamType == "I") {
					printf("type='number' value=%d>\n", $row->ValueNum);
				} elseif ($row->ParamType == "F") {
					printf("value=%F size=8>\n", $row->ValueNum);
				} else {
					printf("type='text' value=\"%s\">\n", $row->ValueChar);
				}
			}
		}
	}
	$i_p = null;
	echo("<div id='opdrachtknoppen'>\n");
	echo("<input class='knop' type='submit' value='Bewaren'>\n");
	echo("</div>  <!-- Einde opdrachtknoppen -->\n");
	
	echo("</form>\n");
	echo("</div>  <!-- Einde instellingenmuteren -->\n");
} # fnInstellingen

function fnStamgegevens() {
		
	$rows = (new cls_db_base("Diploma"))->basislijst();
	printf("<p>%s</p>", fnDisplayTable($rows, "", "Basislijst Diploma's", 0, "", "", "lijst", ""));
	
	$rows = (new cls_db_base("Onderdl"))->basislijst();
	printf("<p>%s</p>", fnDisplayTable($rows, "", "Basislijst Onderdelen", 0, "", "", "lijst", ""));
	
	$rows = (new cls_db_base("Organisatie"))->basislijst();
	printf("<p>%s</p>", fnDisplayTable($rows, "", "Basislijst Organisaties", 0, "", "", "lijst", ""));
	
	$rows = (new cls_db_base("Functie"))->basislijst();
	printf("<p>%s</p>", fnDisplayTable($rows, "", "Basislijst Functies", 0, "", "", "lijst", ""));
	
	$rows = (new cls_db_base("Groep"))->basislijst();
	printf("<p>%s</p>", fnDisplayTable($rows, "", "Basislijst Groepen", 0, "", "", "lijst", ""));
	
	
} # fnStamgegevens

function fnStukken() {
	
	$i_stuk = new cls_Stukken();
	
	echo("<div id='stukkenmuteren'>\n");
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['Toevoegen']) and $_POST['Toevoegen'] == "Toevoegen") {
			$i_stuk->add();
		}
		$rows = $i_stuk->editlijst();
		foreach ($rows as $row) {
			foreach ($row as $col => $val){
				$cn = $col . "_" . $row->RecordID;
				if (isset($_POST[$cn])) {
//					debug($cn . ": " . $val);
					$i_stuk->update($row->RecordID, $col, $_POST[$cn]);
				}
			}
		}
		
	}
	
	$rows = $i_stuk->editlijst();
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<table>\n");
	echo("<caption>Stukken muteren</caption>\n");
	echo("<tr><th rowspan=2>Titel</th><th>Bestemd voor</th><th>Vastgesteld op</th><th>Ingangsdatum/Versie</th><th>Revisiedatum</th><th>Vervallen per</th></tr>\n");
	echo("<tr>");
	echo("<th>Type</th><th colspan=4>Link naar document</th>");
	foreach ($rows as $row) {
		echo("<tr>\n");
		printf("<td rowspan=2><input type='text' name='Titel_%d' value='%s' class='tbtitel'></td>\n", $row->RecordID, $row->Titel);
		printf("<td><input type='text' name='BestemdVoor_%d' value='%s' class='tbbestemdvoor'></td>\n", $row->RecordID, $row->BestemdVoor);
		printf("<td><input type='date' name='VastgesteldOp_%d' value='%s'></td>\n", $row->RecordID, $row->VastgesteldOp);
		printf("<td><input type='date' name='Ingangsdatum_%d' value='%s'></td>\n", $row->RecordID, $row->Ingangsdatum);
		printf("<td><input type='date' name='Revisiedatum_%d' value='%s'></td>\n", $row->RecordID, $row->Revisiedatum);
		printf("<td><input type='date' name='VervallenPer_%d' value='%s'></td>\n", $row->RecordID, $row->VervallenPer);
		
		echo("</tr><tr>");
		
		$options = "";
		foreach (ARRTYPESTUK as $k => $v) {
			$options .= sprintf("<option value='%s' %s>%s</option>\n", $k, checked($k, "option", $row->Type), $v);
		}
		printf("<td><select name='Type_%d'>%s</select></td>\n", $row->RecordID, $options);
		printf("<td colspan=4><input type='url' name='Link_%d' value='%s'></td>\n", $row->RecordID, $row->Link);
		
		echo("</tr>\n");
	}
	echo("</table>\n");
	
	echo("<div id='opdrachtknoppen'>\n");
	
	echo("<button name='Toevoegen' value='Toevoegen'>Stuk toevoegen</button>\n");
	echo("<input type='submit' value='Bewaren'>\n");
	echo("</div> <!-- Einde opdrachtknoppen -->\n");
	echo("</form>");
	
	echo("</div> <!-- Einde stukkenmuteren -->\n");
	
	$i_stuk = null;
}  # fnStukken
?>

