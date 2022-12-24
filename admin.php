<?php

include('./includes/standaard.inc');
set_time_limit(90);

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
if ($_GET['op'] == "deletelogin" and isset($_GET['tp']) and $_GET['tp'] == "Beheer logins") {
	$i_login->delete($_GET['lidid']);
} elseif ($_GET['op'] == "unlocklogin" and $_GET['tp'] == "Beheer logins") {
	$i_login->update($_GET['lidid'], "FouteLogin", 0);
	printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $currenttab);
} elseif (isset($_POST['tabpage_nw']) and strlen($_POST['tabpage_nw']) > 0 and $_GET['tp'] == "Autorisatie") {
	(new cls_Authorisation())->add($_POST['tabpage_nw']);
} elseif ($_GET['op'] == "deleteautorisatie" and $_GET['tp'] == "Autorisatie") {
	(new cls_Authorisation())->delete($_GET['recid']);
} elseif ($_GET['op'] == "changeaccess" and $_GET['tp'] == "Autorisatie") {
	foreach((new cls_Authorisation())->lijst() as $row) {
		$vn = sprintf("toegang%d", $row->RecordID);
		if (isset($_POST[$vn])) {
			(new cls_Authorisation())->update($row->RecordID, "Toegang", $_POST[$vn]);
		}
	}
} elseif ($_GET['op'] == "uploaddata") {
	$i_lb = new cls_Logboek();
	$i_base = new cls_db_base();
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
			if (strpos($queries, TABLE_PREFIX . "Lid") === FALSE and (new cls_Lid())->aantal() < 5) {
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
					$i_base->execsql($query);
				}
				$mess = "Bestand is in de database verwerkt.";
				db_onderhoud(1);
				fnMaatwerkNaUpload();
				printf("<script>setTimeout(\"location.href='%s';\", 90000);</script>\n", $_SERVER['PHP_SELF']);
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
	
} elseif ($_GET['op'] == "deleteint" and $_GET['tp'] == "Downloaden wijzigingen") {
	(new cls_interface())->delete($_GET['recid']);
	
} elseif ($_GET['op'] == "backup") {
	db_backup($_SESSION['settings']['db_backup_type']);
} elseif ($_GET['op'] == "FreeBackupFiles") {
	fnFreeBackupFiles();
} elseif ($_GET['op'] == "ledenonderdelenbijwerken") {
	$i_lo = new cls_Lidond();
	$i_lo->autogroepenbijwerken(0, 1);
	$i_lo->auto_einde();
	$i_lo->controle();
	$i_lo->opschonen();
} elseif ($_GET['op'] == "beheeronderdelen") {
	(new cls_Onderdeel())->opschonen();
	(new cls_Onderdeel())->controle();
	(new cls_Groep())->opschonen();
	(new cls_Groep())->controle();
	(new cls_Functie())->opschonen();
	(new cls_Functie())->controle();
	(new cls_Organisatie())->opschonen();
	(new cls_Organisatie())->controle();
	
} elseif ($_GET['op'] == "logboekopschonen") {
	(new cls_Logboek())->opschonen();
	(new cls_logboek())->debugopschonen();
	
} elseif ($_GET['op'] == "ledenopschonen") {
	(new cls_Lid())->controle();
	(new cls_Lid())->opschonen();
	
	(new cls_Lidmaatschap())->controle();
	(new cls_Lidmaatschap())->opschonen();
	
	(new cls_Foto())->opschonen();
} elseif ($_GET['op'] == "beheerdiplomas") {
	$i_dp = new cls_Diploma();
	$i_dp->controle();
	$i_dp->opschonen();
	$i_dp = null;

	$i_ld = new cls_Liddipl();
	$i_ld->controle();
	$i_ld->opschonen();
	$i_ld = null;
	
	$i_ex = new cls_Examen();
	$i_ex->opschonen();
	$i_ex = null;
} elseif ($_GET['op'] == "mailingsopschonen") {
	(new cls_Mailing())->opschonen();
	(new cls_Mailing_hist())->opschonen();
	(new cls_Mailing_rcpt())->opschonen();
} elseif ($_GET['op'] == "evenementenopschonen") {
	(new cls_Evenement())->opschonen();
} elseif ($_GET['op'] == "rekeningenopschonen") {
	(new cls_Rekeningregel())->opschonen();
	(new cls_Rekening())->opschonen();
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
	if ($_GET['op'] == "validatielink" and isset($_GET['lidid'])) {
		fnHerstellenWachtwoord("mail", $_GET['lidid']);
	}
	fnBeheerLogins();
	
} elseif ($currenttab == "Autorisatie" and toegang($currenttab, 1, 1)) {
	$i_auth = new cls_Authorisation();
	$authrows = $i_auth->lijst();
	
	echo("<div id='filter'>\n");
	echo("<input type='text' title='filter tabel' placeholder='Tekstfilter' OnKeyUp=\"fnFilter('beheerautorisatie', this, 2);\">\n");
	printf("<p class='aantrecords'>%d rijen</p>\n", count($authrows));
	echo("</div> <!-- Einde filter -->\n\n");
	
	echo("<div id='beheerautorisatie'>\n");
	printf("<form method='post' action='%s?tp=%s&op=changeaccess'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<table id='beheerautorisatie'>\n");
	echo("<thead>\n");
	echo("<tr><th></th><th>Onderdeel</th><th>Toegankelijk voor</th><th>Ingevoerd</th><th>Laatst gebruikt</th><th></th></tr>\n");
	echo("</thead>\n");
	echo("<tbody>\n");
	$ondrows = (new cls_Onderdeel())->lijst(0, "O.`Type`<>'T'");
	$dtfmt->setPattern(DTTEXT);
	foreach($authrows as $row) {
		if ($row->Toegang > 0) {
			$add = sprintf("<img src='%s' onClick=\"add_auth('%s');window.location.reload(true);\">", BASE64_TOEVOEGEN, $row->Tabpage);
		} else {
			$add = "";
		}
		$del = sprintf("<img src='%s' onClick='verw_auth(%d);'>", BASE64_VERWIJDER, $row->RecordID);
		$selectopt = sprintf("<option value=-1%s>Alleen webmasters</option>\n", checked($row->Toegang, "option", -1));
		$selectopt .= sprintf("<option value=0%s>Iedereen</option>\n", checked($row->Toegang, "option", 0));
		foreach($ondrows as $ond) {
			$selectopt .= sprintf("<option value=%d%s>%s</option>\n", $ond->RecordID, checked($row->Toegang, "option", $ond->RecordID), htmlentities($ond->Naam));
		}
		$cllg = "";
		if ($row->LaatstGebruikt <= date("Y-m-d", mktime(0, 0, 0, date("m")-6, date("d"), date("Y")))) {
			$cllg = "class='attentie'";
		}
		printf("<tr>\n<td>%s</td><td id='name_%d'>%s</td>", $add, $row->RecordID, $row->Tabpage);
		printf("<td><select id='Toegang_%d'>\n%s</select></td>\n<td>%s</td><td %s>%s</td><td>%s</td>\n</tr>\n", $row->RecordID, $selectopt, $dtfmt->format(strtotime($row->Ingevoerd)), $cllg, $dtfmt->format(strtotime($row->LaatstGebruikt)), $del);
	}
	$optionstab = "<option value=''>Selecteer ...</option>\n";
	foreach ($i_auth->lijst("DISTINCT") as $row) {
		$optionstab .= sprintf("<option value='%1\$s'>%1\$s</option>\n", $row->Tabpage);
	}
	echo("</tbody>\n");
	echo("</table>\n");
//	printf("<label>Nieuw</label><select name='tabpage_nw' onBlur='this.form.submit();'>%s</select>\n", $optionstab);
	echo("</form>\n");
	echo("</div> <!-- Einde beheerautorisatie -->\n\n");	
	
	echo("<div id='autorisatiesperonderdeel'>\n");
	echo(fnDisplayTable($i_auth->autorisatiesperonderdeel(), null, "Autorisaties per onderdeel"));
	echo("</div> <!-- Einde autorisatiesperonderdeel -->\n");
?>
<script>
	$('select').change(function() {
		savedata('update_autorisatie', 0, this);
	});

</script>
<?php
	$i_auth = null;
		
} elseif ($currenttab == "Eigen lijsten" and toegang($currenttab, 1, 1)) {
	fnEigenlijstenmuteren();
	
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
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['afmelden'])) {
			(new cls_interface())->afmelden();
		}
	}
	
	$kols[0]['headertext'] = "Betreft lid";
	$kols[0]['columnname'] = "betreftLid";
	$kols[1]['headertext'] = "ingevoerd";
	$kols[2]['headertext'] = "SQL-statement";
	$kols[3]['link'] = sprintf("<a href='/admin.php?op=deleteint&amp;recid=%%d&tp=%s'>&nbsp;&nbsp;&nbsp;</a>", urlencode($_GET['tp']));
	$kols[3]['columnname'] = "RecordID";
	$kols[3]['class'] = "delete";

	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	$i_int = new cls_Interface();
	$rows = $i_int->lijst();
	if (count($rows) > 0) {
		$t = $i_int->aantal("(Afgemeld IS NULL)");
		if (count($rows) > $t) {
			$th = sprintf("%d wijzigingen", count($rows));
		} else {
			$th = sprintf("%d van %d wijzigingen", count($rows), $t);
		}
		$th .= ", te verwerken in de Access database";
		echo(fnDisplayTable($rows, $kols, $th, 0, "", "beheerwijzigingen"));
		foreach ($rows as $row) {
			$copytext .= $row->SQL . "\n";
		}
	} else {
		echo("<p class='mededeling'>Er zijn geen wijzigingen die nog verwerkt moeten worden</p>\n");
	}
		
	if (strlen($copytext) > 0) {
		echo("<h2>SQL-code, te gebruiken in MS-Access:</h2>\n");
		printf("<textarea id='copywijzigingen' class='copypaste' rows=%d readonly>%s</textarea>\n", count($rows)+1, $copytext);
		echo("<button name='kopieer' onClick='CopyFunction()'>Kopieer naar klembord</button>\n");
		echo("<button name='afmelden' type='submit'>Wijzigingen afmelden</button>\n");
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
	(new cls_Eigen_lijst())->controle();

	echo("<div id='dbonderhoud'>\n");
	
	$f = "TypeActiviteit=3";
	$laatstebackup = (new cls_Logboek())->max("DatumTijd", $f);
	
	printf("<form method='post' name='frmOnderhoud' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&op=backup\"' value='Backup'><p>Maak een backup van de database. Laatste backup is op %s gemaakt.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $laatstebackup);
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&op=FreeBackupFiles\"' value='Vrijgeven backup-bestanden'><p>Geef de backup-bestanden vrij door middel van een chmod 0755.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&op=ledenonderdelenbijwerken\"' value='Beheer leden van onderdelen'><p>Bijwerken van leden van onderdelen.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&op=beheeronderdelen\"' value='Beheer onderdelen'><p>Opschonen en controle op data-integriteit van onderdelen, afdelingsgroepen, functies en organisaties.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));	
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&op=logboekopschonen\"' value='Logboek opschonen'><p>Verwijder alle records uit het logboek, die ouder dan <input type='number' name='logboek_bewaartijd' onChange='this.form.submit();' value=%d> maanden zijn.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $_SESSION['settings']['logboek_bewaartijd']);

	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&op=ledenopschonen\"' value='Leden en lidmaatschappen'><p>Controleren en opschonen leden, lidmaatschappen en foto's.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&op=beheerdiplomas\"' value=\"Diploma's beheren\"><p>Opschonen en controleren van diploma's en leden per diploma en examens.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));

	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&op=mailingsopschonen\"' value='Mailings opschonen'><p>Mailings en verzonden e-mails, op basis van bewaarinstellingen, opschonen.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&op=evenementenopschonen\"' value='Evenementen opschonen'><p>Opschonen verwijderde evenementen, die geen deelnemers meer hebben.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&op=rekeningenopschonen\"' value='Rekeningen opschonen'><p>Rekeningen en rekeningregels opschonen op basis van instellingen.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));

	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&op=loginsopschonen\"' value='Logins opschonen'><p>Opschonen van logins die om diverse redenen niet meer nodig zijn.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&op=autorisatieopschonen\"' value='Autorisatie opschonen'><p>Verwijderen toegang waar alleen de webmaster toegang toe heeft en die ouder dan 3 maanden zijn.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	if ((new cls_Orderregel())->aantal() > 0) {
		printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&op=orderregelsopschonen\"' value='Orderregels opschonen'><p>Opschonen van de orderregels van de bestellingen.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	}
	if ((new cls_Artikel())->aantal() > 0) {
		printf("<fieldset><input type='button' onClick='location.href=\"%s?tp=%s&op=artikelenopschonen\"' value='Artikelen opschonen'><p>Opschonen van artikelen zonder bestellingen en zonder voorraadboekingen.</p></fieldset>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	}
	echo("</form>\n");
	echo("</div>  <!-- Einde dbonderhoud -->\n");
	
	printf("<div id='versies'>PHP: %s / Database: %s</div>  <!-- Einde versies -->\n", substr(phpversion(), 0, 6), (new cls_db_base())->versiedb());
	
} elseif ($currenttab == "Logboek" and toegang($currenttab, 1, 1)) {
	$i_lb = new cls_logboek();
	
	$kols = fnStandaardKols("logboek");
	$ord = fnOrderBy($kols);
	
	if (!isset($_POST['tbTekstFilter']) or strlen($_POST['tbTekstFilter']) == 0) {
		$_POST['tbTekstFilter'] = "";
	}
	if (!isset($_POST['typefilter']) or strlen($_POST['typefilter']) == 0) {
		$_POST['typefilter'] = -1;
	}
	$_POST['kolomfilter'] = $_POST['kolomfilter'] ?? "";
	if (!isset($_POST['aantalrijen']) or $_POST['aantalrijen'] < 2) {
		$_POST['aantalrijen'] = 1500;
	}
	$_POST['ingelogdeanderen'] = $_POST['ingelogdeanderen'] ?? 0;
	$_POST['ingelogdeanderen'] = intval($_POST['ingelogdeanderen']);
	
	$f = "";
	if (strlen($_POST['kolomfilter']) > 0) {
		$f = sprintf("CONCAT(A.RefTable, '-', A.refColumn)='%s'", $_POST['kolomfilter']);
	}
	if ($_POST['ingelogdeanderen'] == 1) {
		if (strlen($f) > 0) {
			$f .= " AND ";
		}
		$f .= sprintf("A.LidID > 0 AND A.LidID<>%d", $_SESSION['lidid']);
	}
	$rows = $i_lb->lijst($_POST['typefilter'], 0, 0, $f, $ord, $_POST['aantalrijen']);
	
	echo("<div id='filter'>\n");
	printf("<form method='post' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	
	echo("<input type='text' id='tbTekstFilter' title='Filter tabel' placeholder='Tekst filter' OnKeyUp=\"fnFilter('logboek', this);\">");
	
	echo("<select name='typefilter' onchange='this.form.submit();'>\n");
	echo("<option value=-1>Filter op type ....</option>\n");
	foreach ($TypeActiviteit as $key => $val) {
		printf("<option value=%d %s>%s</option>\n", $key, checked($key, "option", $_POST['typefilter']), htmlentities($val));
	}
	echo("</select>\n");
	
		
	echo("<select name='kolomfilter' onchange='this.form.submit();'>\n");
	echo("<option value=''>Filter op tabel/kolom ....</option>\n");
	foreach ($i_lb->uniekelijst("A.RefTable, A.refColumn", "IFNULL(A.refColumn, '') > ''") as $row) {
		printf("<option value='%1\$s-%2\$s' %s>%1\$s->%2\$s</option>\n", $row->RefTable, $row->refColumn, checked($row->RefTable . "-" . $row->refColumn, "option", $_POST['kolomfilter']), htmlentities($val));
	}
	echo("</select>\n");
	

	$options = "";
	foreach (array(25, 100, 250, 750, 1500, 3000, 10000, 25000) as $a) {
		$options .= sprintf("<option value=%d %s>%s</option>\n", $a, checked($a, "option", $_POST['aantalrijen']), number_format($a, 0, ",", "."));
	}
	printf("<label>Max. aantal rijen</label><select name='aantalrijen' OnChange='this.form.submit();'>%s</select>\n", $options);
	printf("<label>Alleen ingelogde anderen</label><input type='checkbox' name='ingelogdeanderen'%s value=1 onClick='this.form.submit();'>\n", checked($_POST['ingelogdeanderen']));
	echo("</form>\n");
	
	if (count($rows) > 1) {
		printf("<p class='aantrecords'>%s rijen</p>\n", number_format(count($rows), 0, ",", "."));
	}
	echo("</div>  <!-- Einde filter -->\n");
	
	echo(fnDisplayTable($rows, $kols, "", 0, "", "logboek"));
	
	$i_lb = null;
	
} elseif ($currenttab == "Info" and toegang($currenttab, 1, 1)) {
	
//    echo memory_get_usage()/1024.0 . " kb \n";
	phpinfo();
}

HTMLfooter();

function fnBeheerLogins() {
	
	$i_login = new cls_Login();
	$i_login->uitloggen();
	
	$arrSort[0] = "Login;Login";
	$arrSort[1] = "Naam lid;Achternaam";
	$arrSort[2] = "Woonplaats;Woonplaats";
	$arrSort[3] = "Lidnr;Lidnr";
	$arrSort[4] = "E-mail;E-mail";
	$arrSort[5] = "Ingevoerd;Login.Ingevoerd";
	$arrSort[6] = "Laatste login;LastLogin";
	$arrSort[7] = "Status;Status";
	
	$kols[0]['headertext'] = "Login";
	$kols[0]['sortcolumn'] = "Login";
	$kols[1]['headertext'] = "Naam lid";
	$kols[1]['sortcolumn'] = "L.Achternaam";
	$kols[2]['headertext'] = "Woonplaats";
	$kols[3]['headertext'] = "Lidnr";
	$kols[3]['sortcolumn'] = "Lidnr";
	$kols[4]['headertext'] = "E-mail";
	
	$kols[5]['headertext'] = "Ingevoerd";
	$kols[5]['columnname'] = "Ingevoerd";
	$kols[5]['type'] = "date";
	$kols[5]['sortcolumn'] = "Login.Ingevoerd";
	
	$kols[6]['headertext'] = "Laatste login";
	$kols[6]['sortcolumn'] = "Login.LastLogin";
	$kols[6]['columnname'] = "LastLogin";
	$kols[6]['type'] = "DTLONG";
	
	$kols[7]['headertext'] = "Status";
	$kols[7]['sortcolumn'] = "Status";

	if ($_SESSION['settings']['login_maxinlogpogingen'] > 0) {
		$kols[8]['link'] = sprintf("<a href='%s?op=unlocklogin&tp=Beheer logins&lidid=%%d' title='Reset foutieve logins'>&nbsp;&nbsp;&nbsp;</a>", $_SERVER['PHP_SELF']);
		$kols[8]['columnname'] = "Unlock";
		$kols[8]['class'] = "unlock";
		$kols[8]['headertext'] = "&nbsp;";
	} else {
		$kols[8]['skip'] = true;
	}
	
	$kols[9]['link'] = sprintf("<a href='%s?op=deletelogin&tp=Beheer logins&lidid=%%d'>&nbsp;&nbsp;&nbsp;</a>", $_SERVER['PHP_SELF']);
	$kols[9]['class'] = "trash";
	
	$kols[10]['columnname'] = "ValLink";
	$kols[10]['link'] = sprintf("<a href='%s?op=validatielink&tp=Beheer logins&lidid=%%d'>Stuur validatielink</a>", $_SERVER['PHP_SELF']);
	$kols[10]['headertext'] = "&nbsp;";
	
	$ord = fnOrderBy($kols);
	$rows = $i_login->lijst("", $ord);
	echo("<div id='filter'>\n");
	echo("<input type='text' title='Filter tabel' placeholder='Filter op naam/login' OnKeyUp=\"fnFilter('beheerlogins', this);\">\n");
	if (count($rows) > 2) {
		printf("<p class='aantrecords'>%d logins</p>\n", count($rows));
	}
	echo("</div> <!-- Einde filter -->\n");
	echo("<div class='clear'></div>\n");
	echo(fnDisplayTable($rows, $kols, "", 0, "", "beheerlogins"));
	
	$i_login = null;
	
}  # fnBeheerLogins

function fnInstellingen() {
	
	$i_p = new cls_Parameter();
	$i_p->controle();
	$i_p->vulsessie();
	$i_p = null;

	$arrParam['naamvereniging'] = "Naam van de vereniging";
	$arrParam['naamvereniging_afkorting'] = "Verkorte naam van de vereniging";
	$arrParam['db_backup_type'] = "Welke tabellen moeten worden gebackuped?";
	$arrParam['db_backupsopschonen'] = "Na hoeveel dagen moeten back-ups automatisch verwijderd worden? 0 = nooit.";
	$arrParam['db_folderbackup'] = "In welke folder moet de backup worden geplaatst?";
	$arrParam['emailwebmaster'] = "Het e-mailadres van de webmaster.";
	$arrParam['kaderoverzichtmetfoto'] = "Moeten op het kaderoverzicht pasfoto's getoond worden?";
	$arrParam['toonpasfotoindiennietingelogd'] = "Mogen pasfoto's zichtbaar voor bezoekers (niet ingelogd) zijn?";
	$arrParam['muteerbarememos'] = "Welke soorten memo's zijn in gebruik? Bij meerdere scheiden door een komma.";
	$arrParam['login_autounlock'] = "Na hoeveel minuten moet een gelockede login automatisch geunlocked worden? 0 = alleen handmatig unlocken.";
	$arrParam['login_beperkttotgroep'] = "Vul hier de RecordID's, gescheiden door een komma, van de groepen (zie tabel ONDERDL) in die toegang hebben. Leeg = alleen webmasters hebben toegang.";
	$arrParam['login_bewaartijd'] = "Het aantal maanden dat logins na het laatste gebruik bewaard blijven. 0 = altijd bewaren.";
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
	$arrParam['title_head_html'] = "Hiermee start de HTML-titel van elke pagina.";
	$arrParam['path_templates'] = "Waar staan de templates?";
	$arrParam['path_pasfoto'] = "Waar staan de pasfotos?";
	$arrParam['performance_trage_select'] = "Vanaf hoeveel seconden moet een select-statement in het logboek worden gezet. 0 = nooit.";
	$arrParam['termijnvervallendiplomasmailen'] = "Hoeveel maanden vooruit moeten leden een herinnering krijgen als een diploma gaat vervallen. 0 = geen herinnering sturen.";
	$arrParam['termijnvervallendiplomasmelden'] = "Hoeveel maanden vooruit en achteraf moeten vervallen diploma op het voorblad getoond worden.";
	$arrParam['liddipl_bewaartermijn'] = "Na Hoeveel maanden diploma's bij een lid worden verwijderd? Zowel na einde lidmaatschap als na einde geldigheid.";
	$arrParam['toneninschrijvingenbewakingen'] = "Moeten bij de gegevens van een lid ook inschrijvingen voor bewakingen getoond worden?";
	$arrParam['tonentoekomstigebewakingen'] = "Moeten bij de gegevens van een lid ook toekomstige bewakingen getoond worden?";
	$arrParam['urlvereniging'] = "De URL van de website van de vereniging.";
	$arrParam['url_eigen_help'] = "Als een gebruiker op de help klikt wordt hier naar verwezen in plaats van de standaard help.";
	$arrParam['verjaardagenaantal'] = "Aantal verjaardagen dat maximaal in de verenigingsinfo wordt getoond. Als er meerdere leden op dezelfde dag jarig zijn, wordt dit aantal overschreden.";
	$arrParam['verjaardagenvooruit'] = "Hoeveel dagen vooruit moeten de verjaardagen in de verenigingsinfo getoond worden?";
	
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
		} elseif (strlen($p) > 0 and substr($p, -1) != "/") {
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
	printf("<p>%s</p>", fnDisplayTable($rows, null, "Basislijst Diploma's", 0, "", "", "lijst"));
	
	$rows = (new cls_db_base("Onderdl"))->basislijst();
	printf("<p>%s</p>", fnDisplayTable($rows, null, "Basislijst Onderdelen", 0, "", "", "lijst"));
	
	$rows = (new cls_db_base("Organisatie"))->basislijst();
	printf("<p>%s</p>", fnDisplayTable($rows, null, "Basislijst Organisaties", 0, "", "", "lijst"));
	
	$rows = (new cls_db_base("Functie"))->basislijst();
	printf("<p>%s</p>", fnDisplayTable($rows, null, "Basislijst Functies", 0, "", "", "lijst"));
	
	$rows = (new cls_db_base("Groep"))->basislijst();
	printf("<p>%s</p>", fnDisplayTable($rows, null, "Basislijst Groepen", 0, "", "", "lijst"));
	
	
} # fnStamgegevens

function fnEigenlijstenmuteren() {
	global $dtfmt;
	
	if (isset($_GET['tp'])) {
		$tp = $_GET['tp'];
	}
	
	if (isset($_GET['paramID']) and $_GET['paramID'] > 0) {
		$elid = intval($_GET['paramID']);
	} else {
		$elid = 0;
	}
	$i_el = new cls_Eigen_lijst("", $elid);
	$i_el->controle($elid, 1);
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		if (isset($_POST['Toevoegen']) and $_POST['Toevoegen'] == "lijst_toevoegen") {
			$i_el->add();
		}
		
		if (isset($_POST['naam'])) {
			$i_el->update($elid, "Naam", $_POST['naam']);
		}
		
		if (isset($_POST['tabpage'])) {
			$_POST['tabpage'] = trim($_POST['tabpage']);
			if (substr($_POST['tabpage'], -1) == "\\") {
				$_POST['tabpage'] = substr($_POST['tabpage'], 0, -1);
			}
			$_POST['tabpage'] = str_replace("\\" . $_POST['naam'], "", $_POST['tabpage']);
			$i_el->update($elid, "Tabpage", $_POST['tabpage']);
		}
		
		if (isset($_POST['mysql'])) {
			$i_el->update($elid, "MySQL", $_POST['mysql']);
		}
		
		if (isset($_POST['EigenScript'])) {
			$i_el->update($elid, "EigenScript", $_POST['EigenScript']);
		}
		
		if (isset($_POST['default_waarde_params'])) {
			$i_el->update($elid, "Default_value_params", $_POST['default_waarde_params']);
		}

		$i_el->controle($elid);
		
		if (isset($_POST['BewarenSluiten'])) {
			printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $tp);
		} else {
			printf("<script>location.href='%s?tp=%s&paramActie=2&paramID=%d';</script>\n", $_SERVER['PHP_SELF'], $tp, $elid);
		}
		
	} elseif (isset($_GET['paramActie']) and $_GET['paramActie'] == 3 and $elid > 0) {
		
		$i_el->delete($elid);
		printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $tp);
		
	} elseif (isset($_GET['paramActie']) and $_GET['paramActie'] == 2 and $elid > 0) {
		
		$row = $i_el->record();
		
		echo("<div id='eigenlijstmuteren'>\n");
		printf("<form method='post' action='%s?tp=%s&paramID=%d'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $elid);
		
		printf("<label>Naam eigen lijst</label><input type='text' name='naam' class='w40' value='%s' maxlength=40>\n", $row->Naam);
		printf("<label>MySQL-code</label><textarea id='mysql' name='mysql' rows=16 cols=100>%s</textarea>\n", $row->MySQL);
		printf("<label>Eigen script</label><p>%s/maatwerk/</p><input type='text' name='EigenScript' class='w30' value='%s' maxlength=30>\n", BASISURL, $row->EigenScript);
		printf("<label>Tonen in tabblad</label><input type='text' name='tabpage' class='w75' value='%s' maxlength=75>\n", $row->Tabpage);
		printf("<label>Standaard waarden parameter(s)</label><input type='text' name='default_waarde_params' value='%s' maxlength=100><p>(bij meedere scheiden met een ;)</p>\n", $row->Default_value_params);
		echo("<label>Beschikbare variabelen</label><p>[%LIDNAAM%], [%TELEFOON%], [%EMAIL%], [%LEEFTIJD%]</p>");
		if (strlen($i_el->sqlerror) == 0) {
			printf("<label>Aantal records</label><p>%d</p>\n", $row->AantalRecords);
			printf("<label>Aantal kolommen</label><p>%d</p>\n", $row->AantalKolommen);
		} else {
			$i_el->mess = sprintf("In Eigen_lijst %d is de MySQL-code niet correct. Foutmelding: %s", $elid, $i_el->sqlerror);
			printf("<p>%s</p>\n", $i_el->mess);
			$i_el->Log($elid);
		}
		$dtfmt->setPattern(DTLONGSEC);
		printf("<label>Laatste controle</label><p>%s</p>\n", $dtfmt->format(strtotime($row->LaatsteControle)));
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<input type='submit' value='Bewaren' name='Bewaren'>\n");
		echo("<input type='submit' value='Bewaren & Sluiten' name='BewarenSluiten'>\n");
		if ($row->AantalRecords > 0 and $row->AantalKolommen > 0) {
			echo("<button type='button' onClick=\"$('#resultaatlijst').toggle();\">Toon/verberg resultaat</button>\n");
		}
		echo("</div>\n");
		
		echo("</form>\n");
		echo("</div> <!-- Einde eigenlijstmuteren -->\n");
		
		if ($row->AantalRecords > 0 and $row->AantalKolommen > 0) {
			echo("<div id='resultaatlijst'>\n");
			$i_el = new cls_Eigen_lijst($row->Naam);
			$rows = $i_el->rowset();
			if ($rows !== false) {
				printf("<p>%s</p>", fnDisplayTable($rows, null, "Resultaat"));
				printf("<p>%d rijen</p>\n", count($rows));
			}
			echo("</div> <!-- Einde resultaatlijst -->\n");
		}
		
	} else {
		
		echo("<div id='overzichteigenlijsten'>\n");
		$rows = $i_el->lijst();
		if (count($rows) > 0) {
			echo("<table>\n");
			echo("<caption>Overzicht Eigen lijsten</caption>\n");
			echo("<tr><th></th><th>Naam</th><th># records</th><th># kolommen</th><th>Tabblad</th><th>Laatste controle</th><th></th></tr>\n");
			foreach($rows as $row) {
				$bl = sprintf("<a href='%s?tp=%s&paramID=%d&paramActie=%%d'>&nbsp;&nbsp;&nbsp;</a>", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
				printf("<tr><td>%s</td><td>%s</td><td class='number'>%d</td><td class='number'>%d</td><td>%s</td><td>%s</td><td>%s</td>", sprintf($bl, 2), $row->Naam, $row->AantalRecords, $row->AantalKolommen, $row->Tabpage, date("d-m-Y H:i", strtotime($row->LaatsteControle)), sprintf($bl, 3));
				echo("</tr>\n");
			}
			echo("</table>\n");
		}
		echo("</div> <!-- Einde overzichteigenlijsten -->\n");
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		echo("<div id='opdrachtknoppen'>\n");
		echo("<button type='submit' name='Toevoegen' value='lijst_toevoegen'>Lijst toevoegen</button>\n");
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
	}
	
}  # fnEigenlijstenmuteren

function fnExportSQL() {
	global $arrTables;
	
	
	
}

?>

