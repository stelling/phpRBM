<?php
include('./includes/standaard.inc');

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
		db_delete_local_tables();
		fnQuery("SET CHARACTER SET utf8;");
		$queries = file_get_contents($_FILES['SQLupload']["tmp_name"]);
		if ($queries !== false) {
			$mess = "Bestand is succesvol ge-upload.";
			db_logboek("add", $mess, 9, 0, 1);
			if (substr_count($queries, $table_prefix) == 0) {
				$mess = sprintf("In het upload bestand komt de juiste table name prefix (%s) niet voor. Dit bestand wordt niet verwerkt.", $table_prefix);
				db_logboek("add", $mess, 9, 0, 1);
			} elseif (strpos($queries, $table_prefix . "Lid") === FALSE) {
				$mess = sprintf("De verplichte tabel '%sLid' zit niet in deze upload. Dit bestand wordt niet verwerkt.", $table_prefix);
				db_logboek("add", $mess, 9, 0, 1);
			} elseif (strpos($queries, $table_prefix . "Lidond") === FALSE) {
				$mess = sprintf("De verplichte tabel '%sLidond' zit niet in deze upload. Dit bestand wordt niet verwerkt.", $table_prefix);
				db_logboek("add", $mess, 9, 0, 1);
			} elseif (fnQuery($queries) !== true) {
				$mess = "Bestand is in de database verwerkt.";
				db_logboek("add", $mess, 9, 0, 1);
				db_onderhoud();
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
	$mess = db_backup();
	printf("<p class='mededeling'>%s</p>\n", $mess);
} elseif ($_GET['op'] == "logboekopschonen") {
	$mess = db_logboek("opschonen");
	printf("<p class='mededeling'>%s</p>\n", $mess);
} elseif ($_GET['op'] == "evenementenopschonen") {
	$mess = db_evenement("opschonen");
	printf("<p class='mededeling'>%s</p>\n", $mess);
} elseif ($_GET['op'] == "mailingsopschonen") {
	$mess = db_mailing("opschonen");
	printf("<p class='mededeling'>%s</p>\n", $mess);
} elseif ($_GET['op'] == "loginsopschonen") {
	$mess = db_logins("opschonen");
	printf("<p class='mededeling'>%s</p>\n", $mess);
} elseif ($_GET['op'] == "authorisatieopschonen") {
	$mess = db_authorisation("opschonen");
	printf("<p class='mededeling'>%s</p>\n", $mess);
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
	if (db_param("maxinlogpogingen") > 0) {
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
		$del = sprintf("<a href='%s?tp=%s&amp;op=deleteautorisatie&amp;recid=%d'><img src='images/del.png' title='Verwijder record'></a>", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
		$selectopt = "<option value=-1>Alleen webmasters</option>\n";
		if ($row->Toegang == 0) {
			$selectopt .= "<option value=0 selected>Iedereen</option>\n";
		} else {
			$selectopt .= "<option value=0>Iedereen</option>\n";
		}
		foreach(db_Onderdelen() as $ond) {
			if ($row->Toegang == $ond->RecordID) { $s = " selected"; } else { $s = ""; }
			$selectopt .= sprintf("<option value=%d%s>%s</option>\n", $ond->RecordID, $s, htmlentities($ond->Naam));
		}
		printf("<tr><td>%s</td><td>%s</td><td><select name='toegang%d' onchange='this.form.submit();'>%s</select></td><td>%s</td></tr>\n", $del, $row->Tabpage, $row->RecordID, $selectopt, strftime("%e %B %Y", strtotime($row->Ingevoerd)));
	}
	$optionstab = "<option value=''>Selecteer ...</options>";
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

	printf("<form name='formdownload' method='post' action='%s?tp=%s&amp;op=downloadwijz'>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	$rows = db_interface("lijst");
	if (count($rows) > 0) {
		echo(fnDisplayTable($rows, "", "", 1));
		
		$copytext = "";
		foreach ($rows as $row) {
			$copytext .= $row->SQL . "\n";
		}
		
		echo("<p class='mededeling'><input type='submit' value='Download wijzigingen'>\n");
		printf("&nbsp;<input type='button' value='Wijzigingen afmelden' OnClick=\"location.href='%s?tp=%s&amp;op=afmeldenwijz'\"></p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
		printf("<div class='CopyPaste'><p>Om te kopiëren:</p>%s</div>\n", $copytext);
	} else {
		echo("<p class='mededeling'>Er zijn geen wijzigingen die nog verwerkt moeten worden.</p>\n");
	}
	echo("</form>\n");
} elseif ($currenttab == "Onderhoud" and toegang($_GET['tp'])) {
	db_createtables();
	db_onderhoud();

	echo("<div id='dbonderhoud'>\n");
	$bewaartijdlogging = db_param("bewaartijdlogging");
	
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=backup\"' value='Backup'>&nbsp;Maak een backup van de database.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	if ($bewaartijdlogging > 0) {
		printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=logboekopschonen\"' value='Logboek opschonen'>&nbsp;Verwijder alle records uit het logboek, die ouder dan %d maanden zijn.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $bewaartijdlogging);
	}
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=evenementenopschonen\"' value='Evenementen opschonen'>&nbsp;Opschonen evenementen, inclusief bijbehorende deelnemers, die langer dan 6 maanden geleden zijn verwijderd.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	if (db_param("bewaartijdmailings") > 0) {
		printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=mailingsopschonen\"' value='Mailings opschonen'>&nbsp;Mailings die langer dan %d maanden in de prullenbak zitten worden definitief verwijderd.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), db_param("bewaartijdmailings"));
	}
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=loginsopschonen\"' value='Logins opschonen'>&nbsp;Opschonen van logins die om diverse redenen niet meer nodig zijn.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=authorisatieopschonen\"' value='Authorisatie opschonen'>&nbsp;Verwijderen toegang waar alleen de webmaster toegang toe heeft en die ouder dan 3 maanden zijn.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=orderregelsopschonen\"' value='Orderregels opschonen'>&nbsp;Opschonen van de orderregels van de bestellingen.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<p><input type='button' onClick='location.href=\"%s?tp=%s&amp;op=artikelenopschonen\"' value='Artikelen opschonen'>&nbsp;Opschonen van artikelen zonder bestellingen uit de webshop.</p>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	echo("</div>  <!-- Einde dbonderhoud -->\n");
	
	$query = "SELECT Version() AS Version;";
	$result = fnQuery($query);
	printf("<div id='versies'>\nMySQL: %s / PHP: %s</div>  <!-- Einde versies -->\n", substr($result->fetchColumn(), 0, 6), substr(phpversion(), 0, 5));
	
} elseif ($currenttab == "Logboek" and toegang($_GET['tp'])) {
	if (!isset($_POST['lidfilter']) or strlen($_POST['lidfilter']) == 0) {
		$_POST['lidfilter'] = 0;
	}
	if (!isset($_POST['typefilter']) or strlen($_POST['typefilter']) == 0) {
		$_POST['typefilter'] = -1;
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
	
	echo("</tr>\n</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde filter -->\n");
	
	$rows = db_logboek("lijst", "", $_POST['typefilter'], $_POST['lidfilter']);
	echo(fnDisplayTable($rows, "", "", 0, "", "", "logboek"));
} elseif ($currenttab == "Info" and toegang($_GET['tp'])) {
	phpinfo();
}

HTMLfooter();

function fnInstellingen() {
	global $table_prefix;

	$arrParam['bewaartijdinloggen'] = "Hoeveel maanden moet logging van het in- en uitloggen bewaard blijven. 0 = altijd.";
	$arrParam['bewaartijdlogging'] = "Hoeveel maanden moet logging bewaard blijven. 0 = altijd.";
	$arrParam['bewaartijdlogins'] = "Het aantal maanden dat logins na het laatste gebruik bewaard worden. Als een login wordt verwijderd, wordt geen historie weggegooid. Historie wordt namelijk direct aan het lid gekopppeld en niet aan de login. 0 = altijd bewaren.";
	$arrParam['bewaartijdloginsnietgebruikt'] = "Het aantal dagen dat logins wordt bewaard, nadat het is aangevraagd en nog niet gebruikt is.";
	$arrParam['bewaartijdmailings'] = "Het aantal maanden dat een verwijderde mailing bewaard moet worden. 0 = altijd bewaren.";
	$arrParam['beperktotgroep'] = "Vul hier de RecordID's, gescheiden door een komma, van de groepen (zie tabel ONDERDL) in die toegang hebben. Als je geen groep invult hebben alleen webmasters toegang.";
	$arrParam['db_backuptarren'] = "Moet de backup gecomprimeerd worden? Let op, de webhost moet dit wel ondersteunen.";
	$arrParam['db_backupsopschonen'] = "Na hoeveel dagen moeten oude back-ups automatisch verwijderd worden? 0 = nooit.";
	$arrParam['db_folderbackup'] = "Deze variabele is optioneel. Mocht deze niet ingevuld worden, dan wordt de standaard folder gebruikt.";
	$arrParam['emailledenadministratie'] = "Het e-mailadres van de ledenadministratie.";
	$arrParam['emailsecretariaat'] = "Deze wordt gebruikt om het secretariaat op de hoogte te houden van verstuurde mailingen en opzeggingen. Dit veld is niet verplicht.";
	$arrParam['emailwebmaster'] = "Het e-mailadres van de webmaster.";
	$arrParam['kaderoverzichtmetfoto'] = "Moeten op het kaderoverzicht pasfoto's getoond worden?";
	$arrParam['lidnrnodigbijloginaanvraag'] = "Moet een lid zijn of haar lidnummer opgeven als er een login aangevraagd wordt?";
	$arrParam['loginautounlock'] = "Na hoeveel minuten moet een gelockede login automatisch geunlocked worden? 0 = alleen handmatig unlocken.";
	$arrParam['mailing_beperkfrom'] = "Indien deze is ingevuld moet het from adres in een mailing altijd vanaf dit domein zijn.";
	$arrParam['mailing_bevestigingbestelling'] = "Het nummer van de mailing die bij een bestelling verstuurd moet worden. 0 = geen.";
	$arrParam['mailing_bevestigingopzegging'] = "Het nummer van de mailing die verstuurd moet worden als een lid zijn lidmaatschap opgezegd heeft. 0 = geen.";
	$arrParam['mailing_bewakinginschrijving'] = "Het nummer van de mailing die als bevestiging van een inschrijving voor de bewaking verstuurd moet worden. 0 = geen.";
	$arrParam['mailing_extensies_toegestaan'] = "De extenties die zijn toegestaan bij bijlagen in een mailing. Als je niets specificeerd wordt een standaard lijst gebruikt.";
	$arrParam['mailing_lidnr'] = "Het nummer van de mailing die verstuurd moet worden als een lid zijn lidnummer opvraagt. 0 = geen.";
	$arrParam['mailing_rekening_from_adres'] = "Vanaf welk e-mailadres moeten de rekeningen gemailed worden?";
	$arrParam['mailing_rekening_from_naam'] = "Welke naam moeten de rekeningen verzonden worden? Standaard is vanaf de verenigingsnaam.";
	$arrParam['mailing_resultaatversturen'] = "Indien aangevinkt wordt naar de zender en het secretariaat een mail met het resultaat van deze mailing verzonden.";
	$arrParam['max_grootte_bijlage'] = "De maximalale grootte in bytes van één bijlage in een mailing. Optioneel veld. Als je niets specificeerd dan is 2MB het maximum.";
	$arrParam['maxinlogpogingen'] = "Na hoeveel foutieve inlogpogingen moet het account geblokkeerd worden? 0 = nooit.";
	$arrParam['maxlengtelogin'] = "De maximale lengte die een login mag zijn. Minimaal 7 en maximaal 15 invullen.";
	$arrParam['maxmailsperminuut'] = "Het maximaal aantal e-mails dat via een mailing per minuut verzonden mag worden. 0 = onbeperkt.";
	$arrParam['naamvereniging'] = "Wat is de naam van de vereniging?";
	$arrParam['naamvereniging_afkorting'] = "Wat is de afkorting van de naam van de vereniging?";
	$arrParam['naamwebsite'] = "Dit is de naam zoals deze in de titel en op elke pagina getoond wordt.";
	$arrParam['performance_trage_select'] = "Vanaf hoeveel seconden moet een select-statement in het logboek worden gezet. 0 = nooit.";
	$arrParam['scriptbijuitloggen'] = "Dit script wordt gedraaid nadat iemand is uitgelogd. Optioneel veld.";
	$arrParam['scriptnainloggen'] = "Dit script wordt gedraaid nadat iemand is ingelogd. Optioneel veld.";
	$arrParam['termijnvervallendiplomasmailen'] = "Hoeveel maanden vooruit moeten leden een herinnering krijgen als een diploma gaat vervallen. 0 = geen herinnering sturen.";
	$arrParam['termijnvervallendiplomasmelden'] = "Hoeveel maanden vooruit en achteraf moeten vervallen diploma op het voorblad getoond worden.";
	$arrParam['toneninschrijvingenbewakingen'] = "Moeten bij de gegevens van een lid ook inschrijvingen voor bewakingen getoond worden?";
	$arrParam['tonentoekomstigebewakingen'] = "Moeten bij de gegevens van een lid ook toekomstige bewakingen getoond worden?";
	$arrParam['typemenu'] = "1 = per niveau een aparte regel, 2 = één menu met dropdown, 3 = één menu met dropdown en extra menu voor niveau 2.";
	$arrParam['urlwebsite'] = "De URL van deze website. Zonder http://";
	$arrParam['urlvereniging'] = "De URL van de website van de vereniging. Zonder http://";
	$arrParam['verjaardagenaantal'] = "Hoeveel verjaardagen moeten er maximaal in de verenigingsinfo worden getoond. Als er meerdere leden op dezelfde dag jarig zijn, wordt dit aantal overschreden.";
	$arrParam['verjaardagenvooruit'] = "Hoeveel dagen vooruit moeten de verjaardagen in de verenigingsinfo getoond worden?";
	
	$arrParam['zs_emailbevestigingbestelling'] = "Vanaf welk e-mailadres moet de bevestiging van een bestelling verzonden worden.";
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
	
	$arrParam["laatstebackup"] = "";
	$arrParam["versie"] = "";
	
	foreach ($arrParam as $naam => $val) {
		db_param($naam, "controle");
	}
	$specmailing = array("mailing_bevestigingopzegging", "mailing_bewakinginschrijving", "mailing_lidnr", "mailing_bevestigingbestelling");
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
				} elseif ($row->Naam == "maxlengtelogin") {
					if (strlen($_POST[$pvn]) == 0 or is_numeric($_POST[$pvn]) == false or $_POST[$pvn] > 15) {
						$_POST[$pvn] = 15;
						$mess = sprintf("Parameter '%s' wordt 15 gemaakt, omdat dit de maximale waarde is. ", $row->Naam);
					} elseif ($_POST[$pvn] < 7) {
						$_POST[$pvn] = 7;
						$mess = sprintf("Parameter '%s' wordt 7 gemaakt, omdat dit de minimale waarde is. ", $row->Naam);
					}
				} elseif (substr($row->Naam, 0, 3) == "url" and isset($_POST[$pvn])) {
					$_POST[$pvn] = str_replace("http://", "", $_POST[$pvn]);
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
				db_param($row->Naam, "updval", $_POST[$pvn]);
			}
		}
	}

	echo("<div id='instellingenmuteren'>\n");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<table>\n");
	foreach (db_param("", "lijst") as $row) {
		if (array_key_exists($row->Naam, $arrParam)) {
			$uitleg = $arrParam[$row->Naam];
			if (strlen($uitleg) == 0) {
			} elseif ($row->ParamType == "B") {
				if ($row->ValueNum == 1) {
					$c = "checked";
				} else {
					$c = "";
				}
				printf("<tr><td class='label'>%s: </td><td><input type='checkbox' name='value_%d' value='1' %s></td><td>%s</td></tr>\n", $row->Naam, $row->RecordID, $c, $uitleg);
			} elseif ($row->ParamType == "I") {
				printf("<tr><td class='label'>%s: </td><td><input type='number' class='inputnumber' name='value_%d' value=%d size=8></td><td>%s</td></tr>\n", $row->Naam, $row->RecordID, $row->ValueNum, $uitleg);
			} elseif ($row->ParamType == "F") {
				printf("<tr><td class='label'>%s: </td><td><input name='value_%d' value=%F size=8></td><td>%s</td></tr>\n", $row->Naam, $row->RecordID, $row->ValueNum, $uitleg);
			} elseif (strlen($row->ValueChar) > 60) {
				printf("<tr><td class='label'>%s: </td><td><textarea cols=50 rows=10 name='value_%d'>%s</textarea></td><td>%s</td></tr>\n", $row->Naam, $row->RecordID, $row->ValueChar, $uitleg);
			} else {
				printf("<tr><td class='label'>%s: </td><td><input type='text' class='inputtext' name='value_%d' value=\"%s\"></td><td>%s</td></tr>\n", $row->Naam, $row->RecordID, $row->ValueChar, $uitleg);
			}
		} else {
			db_param($row->Naam, "delete");
		}
	}
	echo("<tr><th colspan=3><input type='submit' value='Bewaren'></th></tr>\n");
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde instellingenmuteren -->\n");
}

?>
