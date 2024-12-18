<?php

include('./includes/standaard.inc');
set_time_limit(90);

$_GET['tp'] = $_GET['tp'] ?? "Beheer logins";

if ((!isset($_SESSION['lidid']) or $_SESSION['lidid'] == 0) and isset($_COOKIE['password']) and strlen($_COOKIE['password']) > 5) {
	fnAuthenticatie(0);
}

if ($_SESSION['lidid'] == 0) {
	header("location: index.php");
}

$i_p = new cls_Parameter();
if ($bestandsversie != $_SESSION['settings']['versie']) {
	debug(sprintf("Nieuwe versie: %s", $dtfmt->format(time())), 0);
	$i_p->controle();
	$i_p->update("versie", $bestandsversie);
	$i_p->vulsessie();
	db_createtables();
	db_onderhoud(2);
}
$i_p = null;

if (!isset($_GET['op']) or ((new cls_lid())->aantal() == 0 and toegang($_GET['tp'], 0, 1) === false)) {
	$_GET['op'] = "";
}

if ($currenttab == "Logboek") {
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
		$i_base->setcharset();
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
				if (function_exists("fnMaatwerkNaUpload")) {	
					fnMaatwerkNaUpload();
				}
				printf("<script>setTimeout(\"location.href='%s';\", 90000);</script>\n", $_SERVER['PHP_SELF']);
			}
			if (strlen($mess) > 0) {
				$i_lb->add($mess, 9, 0, 1);
			}
		}
	} else {
//		$mess = sprintf("Er is iets mis gegaan tijdens het uploaden. Error: %s. Klik <a href='http://nl3.php.net/manual/en/features.file-upload.errors.php'>hier</a> voor uitleg van de code.", $_FILES['SQLupload']['error']);
		$mess = "Het upload bestand bestaat niet, de upload wordt niet uitgevoerd.";
		$i_lb->add($mess, 2, 0, 1);
	}
	$i_lb = null;
	
} elseif ($_GET['op'] == "deleteint" and $_GET['tp'] == "Downloaden wijzigingen") {
	(new cls_interface())->delete($_GET['recid']);
	
} elseif ($_GET['op'] == "backup") {
	db_backup($_SESSION['settings']['db_backup_type']);
	
} elseif ($_GET['op'] == "FreeBackupFiles") {
	fnFreeBackupFiles();
}

if ($currenttab == "Beheer logins") {
	if (toegang($currenttab, 1, 1)) {
		if ($_GET['op'] == "validatielink" and isset($_GET['lidid']) and $_GET['lidid'] > 0) {
			fnValidatieLogin($_GET['lidid'], "", "mail");
		}
		fnBeheerLogins();
	}
	
} elseif ($currenttab == "Autorisatie") {
	if (toegang($currenttab, 1, 1)) {
		beheerautorisatie();
	}

} elseif ($currenttab == "Eigen lijsten") {
	if (toegang($currenttab, 1, 1)) {
		eigenlijstenmuteren();
	}
	
} elseif ($currenttab == "Templates") {
	if (toegang($currenttab, 1, 1)) {
		fnTemplatesmuteren();
	}
	
} elseif ($currenttab == "Instellingen") {
	if (toegang($currenttab, 1, 1)) {
		fnInstellingen();
	}
	
} elseif ($currenttab == "Stamgegevens") {
	if (toegang($currenttab, 1, 1)) {
		fnStamgegevens();
	}
	
} elseif ($currenttab == "Downloaden wijzigingen") {
	if (toegang($currenttab, 1, 1)) {
		downloadwijzigingen();
	}
	
} elseif ($currenttab == "Export data") {
	if (toegang($currenttab, 1, 1)) {
		db_backup(4);
	}
	
} elseif ($currenttab == "Onderhoud") {
	if (toegang($currenttab, 1, 1)) {
		onderhoud();
	}
	
} elseif ($currenttab == "Logboek") {
	if (toegang($currenttab, 1, 1)) {
		logboek();
	}
	
} elseif ($currenttab == "Info" and toegang($currenttab, 1, 1)) {
	
//    echo memory_get_usage()/1024.0 . " kb \n";
	phpinfo();
} else {
	debug("Optie bestaat niet: " . $currenttab, 1, 1);
}

HTMLfooter();

function fnBeheerLogins() {
	global $currenttab;
	
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
	
	$kols[0] = array('columnname' => "Login", 'sortcolumn' => "Login");
	$kols[1] = array('columnname' => "NaamLid", 'headertext' => "Naam lid", 'sortcolumn' => "L.Achternaam");	
	$kols[2] = array('headertext' => "Woonplaats", 'columnname' => "Woonplaats");
	$kols[3] = array('headertext' => "Lidnr", 'columnname' => "Lidnr", 'sortcolumn' => "Lidnr");
	$kols[4] = array('headertext' => "E-mail", 'columnname' => "E-mail");
	$kols[5] = array('headertext' => "Ingevoerd", 'columnname' => "Ingevoerd", 'type' => "date", 'sortcolumn' => "Login.Ingevoerd");
	$kols[6] = array('headertext' => "Laatste login", 'columnname' => "LastLogin", 'sortcolumn' => "Login.LastLogin", 'type' => "DTLONG");
	$kols[7] = array('headertext' => "Status", 'columnname' => "Status", 'sortcolumn' => "Status");
	
	$ord = fnOrderBy($kols);
	$rows = $i_login->lijst("", $ord);

	if (count($rows) > 0 and max(array_column($rows, "Unlock")) > 0) {
		$l = sprintf("%s?tp=%s&op=unlocklogin&lidid=%%d", $_SERVER['PHP_SELF'], $currenttab);
		$kols[] = array('headertext' => "&nbsp;", 'link' => $l, 'columnname' => "Unlock", 'class' => "unlock");
	}
	
	$l = sprintf("%s?op=deletelogin&tp=Beheer logins&lidid=%%d'", $_SERVER['PHP_SELF']);
	$kols[] = array('columnname' => "LidID", 'link' => $l, 'class' => "trash");
	
	if (count($rows) > 0 and max(array_column($rows, "ValLink")) > 0) {
		$l = sprintf("<a href='%s?op=validatielink&tp=%s&lidid=%%d'>Stuur validatielink</a>", $_SERVER['PHP_SELF'], $_GET['tp']);
		$kols[] = array('headertext' => "&nbsp;", 'columnname' => "ValLink", 'type' => "link", 'link' => $l);
	}
	
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

function beheerautorisatie() {
	global $dtfmt, $currenttab;
	
	$i_auth = new cls_Authorisation();
	
	$tf = $_GET['tekstfilter'] ?? "";
	$if = $_GET['ingevoerdfilter'] ?? substr($i_auth->min("Ingevoerd"), 0, 10);
	
	$i_auth->where = sprintf("Ingevoerd >= '%s'", $if);
	if (strlen($tf) > 0) {
		$i_auth->where .= " AND Tabpage LIKE '%" . $tf . "%'"; 
	}
	$authrows = $i_auth->lijst();
	
	printf("<form method='GET' id='filter'>\n<input type='hidden' name='tp' value='%s'>\n", $currenttab);
	printf("<input type='text' name='tekstfilter' value='%s' title='Filter op tekst' placeholder='Tekstfilter' onBlur='this.form.submit();'>\n", $tf, __FUNCTION__);
	printf("<label class='form-label'>Ingevoerd na</label><input type='date' name='ingevoerdfilter' value='%s' title='Filter op datum ingevoerd' onBlur='this.form.submit();'>\n", $if);
	printf("<p class='aantrecords'>%d rijen</p>\n", count($authrows));
	echo("</form>\n");
	
	printf("<table id='%s' class='%s'>\n", __FUNCTION__, TABLECLASSES);
	echo("<thead>\n");
	echo("<tr><th></th><th>Onderdeel</th><th>Toegankelijk voor</th><th>Ingevoerd</th><th>Laatst gebruikt</th><th></th></tr>\n");
	echo("</thead>\n");
	echo("<tbody>\n");
	$ondrows = (new cls_Onderdeel())->lijst(0, "O.`Type`<>'T'");
	$dtfmt->setPattern(DTTEXT);
	foreach($authrows as $row) {
		$i_auth->vulvars($row->RecordID);
		if ($row->Toegang > 0) {
			$add = sprintf("<i class='bi bi-plus-circle' alt='Regel toevoegen' title='Regel toevoegen' onClick=\"add_auth('%s');window.location.reload(true);\"></i>", $row->Tabpage);
		} else {
			$add = "";
		}
		$del = sprintf("<i id='delete_%1\$d' class='bi bi-trash' alt='Verwijderen' onClick='verw_auth(%1\$d);'></i>", $row->RecordID);
		$selectopt = sprintf("<option value=-1%s>Alleen webmasters</option>\n", checked($row->Toegang, "option", -1));
		$selectopt .= sprintf("<option value=-2%s>Niemand</option>\n", checked($row->Toegang, "option", -2));
		$selectopt .= sprintf("<option value=0%s>Iedereen</option>\n", checked($row->Toegang, "option", 0));
		foreach($ondrows as $ond) {
			$selectopt .= sprintf("<option value=%d%s>%s</option>\n", $ond->RecordID, checked($row->Toegang, "option", $ond->RecordID), htmlentities($ond->Naam));
		}
		$clnw = "";
		if ($row->Ingevoerd >= date("Y-dm-d", strtotime("-7 day"))) {
			$clnw = " class='nieuw'";
		}
		$cllg = "";
		if ($row->LaatstGebruikt <= date("Y-m-d", mktime(0, 0, 0, date("m")-6, date("d"), date("Y")))) {
			$cllg = " class='attentie'";
		}
		printf("<tr>\n<td>%s</td><td id='name_%d'>%s</td>\n", $add, $row->RecordID, $row->Tabpage);
		printf("<td><select id='Toegang_%d' class='form-select form-select-sm'>\n%s</select></td>\n<td%s>%s</td><td%s>%s</td><td>%s</td>\n</tr>\n", $row->RecordID, $selectopt, $clnw, $dtfmt->format(strtotime($i_auth->ingevoerd)), $cllg, $dtfmt->format(strtotime($i_auth->laatstgebruikt)), $del);
	}
	echo("</tbody>\n");
	echo("</table>\n");
/*
	$optionstab = "<option value=''>Selecteer ...</option>\n";
	foreach ($i_auth->lijst("DISTINCT") as $row) {
		$optionstab .= sprintf("<option value='%1\$s'>%1\$s</option>\n", $row->Tabpage);
	}
	printf("<label>Nieuw</label><select name='tabpage_nw' onBlur='this.form.submit();'>%s</select>\n", $optionstab);
*/
	
	echo("<div id='autorisatiesperonderdeel'>\n");
	echo(fnDisplayTable($i_auth->autorisatiesperonderdeel(), null, "Autorisaties per onderdeel"));
	echo("</div> <!-- Einde autorisatiesperonderdeel -->\n");

	echo("<script>
	$('select').change(function() {
		savedata('update_autorisatie', 0, this);
	});
	</script>\n");
	$i_auth = null;
	
}  # beheerautorisatie

function fnInstellingen() {
	
	$i_p = new cls_Parameter();
	$i_p->controle();
	$i_p->vulsessie();
	$i_p = null;

	$arrParam['naamvereniging'] = "Naam van de vereniging";
	$arrParam['naamvereniging_afkorting'] = "Verkorte naam van de vereniging";
	$arrParam['db_backup_type'] = "Welke tabellen moeten worden gebackuped?";
	$arrParam['db_backupsopschonen'] = array('label' => "Na hoeveel dagen moeten back-ups verwijderd worden?", 'uitleg' => "0 = nooit");
	$arrParam['logboek_bewaartijd'] = array('label' => "Hoelang moet logging worden bewaard?", 'uitleg' => "maanden");
	$arrParam['db_folderbackup'] = "In welke folder moet de backup worden geplaatst?";
	$arrParam['interface_access_db'] = "Moet de tabel voor de interface naar MS-Access worden gevuld?";
	$arrParam['kaderoverzichtmetfoto'] = "Moeten op het kaderoverzicht pasfoto's getoond worden?";
	$arrParam['toonpasfotoindiennietingelogd'] = "Mogen pasfoto's zichtbaar voor bezoekers (niet ingelogd) zijn?";
	$arrParam['login_autounlock'] = array('label' => "Wachttijd automatisch unlocken logins", 'uitleg' => "minuten / 0 = alleen handmatig unlocken");
	$arrParam['login_beperkttotgroep'] = array('label' => "De onderdelen (zie tabel ONDERDL) die toegang hebben", 'uitleg' => "Bij meerdere: de RecordID's scheiden door een komma / Leeg: alleen webmasters");
	$arrParam['login_bewaartijd'] = array('label' => "Het aantal maanden dat logins na het laatste gebruik bewaard blijven", 'uitleg' => "0 = altijd");
	$arrParam['login_geldigheidactivatie'] = array('label' => "Hoelang in uren is een activatielink geldig?", 'uitleg' => "0 = altijd");
	$arrParam['login_bewaartijdnietgebruikt'] = array('label' => "Hoelang moeten logins worden bewaard, na aangevragen en zonder gebruik", 'uitleg' => "dagen");
	$arrParam['menu_met_afdelingen'] = array('label' => "Voor welke afdelingen/onderdelen moeten een tabblad worden gemaakt?", 'uitleg' => "bij meerdere: RecordID's scheiden met een komma");
	$arrParam['login_maxinlogpogingen'] = array('label' => "Na hoeveel foutieve inlogpogingen moet het account geblokkeerd worden?", 'uitleg' => "0 = nooit");
	$arrParam['login_maxlengte'] = array('label' => "Maximale lengte van een login", 'uitleg' => "Minimaal 7 en maximaal 20");
	$arrParam['wachtwoord_minlengte'] = array('label' => "De minimale lengte van een wachtwoord", 'uitleg' => "Minimaal 7 en maximaal 15");
	$arrParam['wachtwoord_maxlengte'] = array('label' => "De maximale lengte van een wachtwoord", 'uitleg' => "Minimaal 7 en maximaal 15");
	$arrParam['naamwebsite'] = "Dit is de naam zoals deze in de titel en op elke pagina getoond wordt.";
	$arrParam['title_head_html'] = "Hiermee start de HTML-titel van elke pagina";
	$arrParam['performance_trage_select'] = array('label' => "Vanaf hoeveel seconden moet een SQL-statement worden gelogd", 'uitleg' => "0 = nooit");
	$arrParam['termijnvervallendiplomasmailen'] = array('label' => "Hoelang vooruit moeten leden een herinnering krijgen van vervallen diploma's?", 'uitleg' => "maanden / 0 = geen herinnering sturen");
	$arrParam['termijnvervallendiplomasmelden'] = array('label' => "Hoelang vooruit en achteraf vervallen diploma op het voorblad tonen", 'uitleg' => "maanden / 0 = geen tonen");
	$arrParam['urlvereniging'] = "De URL van de website van de vereniging.";
	$arrParam['verjaardagenaantal'] = array('label' => "Aantal verjaardagen dat maximaal in de verenigingsinfo wordt getoond", 'uitleg' => "Als er meerdere leden op dezelfde dag jarig zijn, wordt dit aantal overschreden");
	$arrParam['verjaardagenvooruit'] = "Aantal dagen vooruit verjaardagen in de verenigingsinfo tonen?";
	
	$arrParam['zs_voorwaardenbestelling'] = "Deze regel wordt bij de online-bestellingen in de zelfservice vermeld.";
	
	$specmailing = array("mailing_bevestigingbestelling");
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
				if (in_array($row->Naam, array("login_beperkttotgroep", "zs_muteerbarememos", "menu_met_afdelingen"))) {
					$_POST[$pvn] = str_replace(";", ",", $_POST[$pvn]);
					$_POST[$pvn] = str_replace("'", "", $_POST[$pvn]);
					$_POST[$pvn] = str_replace("\"", "", $_POST[$pvn]);
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
	}

	printf("<form method='post' id='algemeen_instellingen' class='form-check form-switch' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	
	foreach ($i_p->lijst() as $row) {
		if (isset($arrParam[$row->Naam])) {
			
			if (is_array($arrParam[$row->Naam])) {
				$label = htmlent($arrParam[$row->Naam]['label']);
				$uitleg = "<p>" . htmlent($arrParam[$row->Naam]['uitleg']) . "</p>";
			} else {
				$label = htmlent($arrParam[$row->Naam]);
				$uitleg = "";
			}
			if (isset($row->ValueChar) and strlen($row->ValueChar) > 60 and $row->ParamType="T") {
				printf("<label class='form-label'>%s</label><textarea name='%s'>%s</textarea>\n", $label, $row->Naam, $row->ValueChar);
			} elseif ($row->Naam == "db_backup_type") {
				printf("<label class='form-label'>%s</label><select name='%s' id='%s' class='form-select form-select-sm'>", $label, $row->Naam, str_replace(" ", "_", strtolower($row->Naam)));
				foreach (ARRTYPEBACKUP as $key => $val) {
					printf("<option value=%d %s>%s</option>\n", $key, checked($row->ValueNum, "option", $key), $val);
				}
				echo("</select>\n");
			} else {
				printf("<label class='form-label'>%s</label><input name='%s' id='%s' ", $label, $row->Naam, str_replace(" ", "_", strtolower($row->Naam)));
				if ($row->ParamType == "B") {
					printf("type='checkbox' class='form-check-input' value='1' %s>\n", checked(intval($row->ValueNum)));
				} elseif ($row->ParamType == "I") {
					printf("type='number' value=%d>%s\n", $row->ValueNum, $uitleg);
				} elseif ($row->ParamType == "F") {
					printf("value=%F size=8>%s\n", $row->ValueNum, $uitleg);
				} else {
					printf("type='text' value=\"%s\">%s\n", $row->ValueChar, $uitleg);
				}
			}
		}
	}
	$i_p = null;
	echo("<div id='opdrachtknoppen'>\n");
	printf("<button type='submit' class='%s' value='Bewaren'>%s Bewaren</button>\n", CLASSBUTTON, ICONBEWAAR);
	echo("</div>  <!-- Einde opdrachtknoppen -->\n");
	
	echo("</form>\n");
	
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

function eigenlijstenmuteren() {
	global $dtfmt;
	
	$tp = $_GET['tp'] ?? "";
		
	if (isset($_GET['paramID']) and $_GET['paramID'] > 0) {
		$elid = intval($_GET['paramID']);
	} elseif (isset($_POST['recordid']) and $_POST['recordid'] > 0) {
		$elid = intval($_POST['recordid']);
	} else {
		$elid = 0;
	}
	$i_el = new cls_Eigen_lijst("", $elid);
	$i_ond = new cls_Onderdeel();
	$i_mh = new cls_Mailing_hist();
	
	$toonlijst = true;
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		if (isset($_POST['Toevoegen']) and $_POST['Toevoegen'] == "lijst_toevoegen") {
			$i_el->add();
			
		} elseif ($elid > 0) {

			foreach (array("Naam", "Uitleg", "MySQL", "EigenScript", "GroepMelding") as $fn) {
				$cn = strtolower($fn);
				if (isset($_POST[$cn])) {
					$i_el->update($elid, $fn, $_POST[$cn]);
				}
			}
		
			if (isset($_POST['tabpage'])) {
				$_POST['tabpage'] = trim($_POST['tabpage']);
				if (substr($_POST['tabpage'], -1) == "\\") {
					$_POST['tabpage'] = substr($_POST['tabpage'], 0, -1);
				}
				$_POST['tabpage'] = str_replace("\\" . $_POST['naam'], "", $_POST['tabpage']);
				$i_el->update($elid, "Tabpage", $_POST['tabpage']);
			}
			
			$pw = "";
			for ($p=1;$p<=9;$p++) {
				$cn = sprintf("waarde_param%d", $p);
				$w = $_POST[$cn] ?? "";
				$w = str_replace("\"", "'", $w);
				$pw .= $w . ";";
			}
			$i_el->update($elid, "Default_value_params", $pw);
		
			if (!isset($_POST['BewarenSluiten'])) {
				printf("<script>location.href='%s?tp=%s&paramActie=2&paramID=%d';</script>\n", $_SERVER['PHP_SELF'], $tp, $elid);
			}
		}
		
	} elseif (isset($_GET['paramActie']) and $_GET['paramActie'] == 3 and $elid > 0) {
		$i_el->delete($elid);
	}

	if ($elid > 0) {
		$i_el->controle($elid, 1);
	}
	
	if (isset($_GET['paramActie']) and $_GET['paramActie'] == 2 and $i_el->elid > 0) {
		$i_el->vulvars($i_el->elid);
		$toonlijst = false;
		
		printf("<form method='post' id='eigenlijstmuteren' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		printf("<label class='form-label'>RecordID</label><p>%d</p>\n", $i_el->elid);
		printf("<input type='hidden' name='recordid' value=%d>\n", $i_el->elid);
		printf("<label class='form-label'>Naam</label><input type='text' name='naam' class='w50' value='%s' maxlength=50>\n", $i_el->naam);
		printf("<label for='uitleg'>Uitleg</label><textarea id='uitleg' name='uitleg' placeholder='Uitleg over de eigen lijst'>%s</textarea>\n", $i_el->uitleg);

		echo("<label for='mysql'>MySQL code</label>\n");
		printf("<textarea id='mysql' name='mysql' title='MySQL code' placeholder='MySQL code'>%s</textarea>\n", $i_el->mysql);
		echo("<p>Parameters kunnen worden gebruikt. Een parameter start met '@P', gevolgd door 1 t/m 9.</p>\n");
		
		printf("<label class='form-label'>Eigen script</label><p>%s/maatwerk/</p><input type='text' name='eigenscript' class='w30' value='%s' maxlength=30>\n", BASISURL, $i_el->eigenscript);
		printf("<label class='form-label'>Tonen in tabblad</label><input type='text' name='tabpage' class='w75' value='%s' maxlength=75>\n", $i_el->tabpage);
		printf("<label class='form-label'>Groep voor melding op voorblad</label><select name='groepmelding' class='form-select form-select-sm'><option value=0>Geen</option>\n%s</select>\n", $i_ond->htmloptions($i_el->groepmelding));
		if ($i_el->aantal_params > 0) {
			echo("<label class='form-label'>Waarde parameter(s)</label>");
			foreach ($i_el->array_params as $k => $v) {
				printf("<input type='text' name='waarde_param%1\$d' class='w15' value=\"%2\$s\" placeholder='@P%1\$d' title='@P%1\$d'>", $k, $v);
			}
			echo("\n");
		}
		echo("<label class='form-label'>Beschikbare variabelen</label><p>[%LIDNAAM%], [%TELEFOON%], [%EMAIL%], [%LEEFTIJD%]</p>");
		if (strlen($i_el->sqlerror) == 0) {
			printf("<label class='form-label'>Aantal rijen</label><p>%d</p>\n", $i_el->aantalrijen);
			printf("<label class='form-label'>Aantal kolommen</label><p>%d</p>\n", $i_el->aantalkolommen);
		} else {
			$i_el->mess = sprintf("In Eigen_lijst %d is de MySQL-code niet correct. Foutmelding: %s", $elid, $i_el->sqlerror);
			printf("<p>%s</p>\n", $i_el->mess);
			$i_el->Log($elid);
		}
		$dtfmt->setPattern(DTLONGSEC);
		
		printf("<label class='form-label'>Laatste controle</label><p>%s</p>\n", $dtfmt->format(strtotime($i_el->laatstecontrole)));
		printf("<label class='form-label'>Ingevoerd op</label><p>%s</p>\n", $i_el->ingevoerdtekst());
		
		$i_mh->where = sprintf("Xtra_Char='NOTIF' AND Xtra_Num=%d", $i_el->elid);
		$lm = $i_mh->max("send_on");
		if (strlen($lm) >= 10) {
			printf("<label class='form-label'>Laatste notificatie-mail</label><p>%s</p>\n", $dtfmt->format(strtotime($lm)));
		}
		
		echo("<div id='opdrachtknoppen'>\n");
		printf("<button type='submit' class='%s' name='Bewaren'>%s Bewaren</button>\n", CLASSBUTTON, ICONBEWAAR);
		printf("<button type='submit' class='%s' name='BewarenSluiten'>%s Bewaren & Sluiten</button>\n", CLASSBUTTON, ICONSLUIT);
		if ($i_el->aantalrijen > 0 and $i_el->aantalkolommen > 0) {
			printf("<button type='button' class='%s' onClick=\"$('#resultaatlijst').toggle();\">%s Toon/verberg resultaat</button>\n", CLASSBUTTON, ICONLIJST);
		}
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");

		if ($i_el->aantalkolommen > 0) {
			echo("<div id='resultaatlijst'>\n");
			$rows = $i_el->rowset($i_el->elid);
			if ($rows !== false) {
				$id = str_replace(" ", "_", strtolower($i_el->naam));
				echo(fnDisplayTable($rows, null, "", 0, "", $id));
			}
			echo("</div>  <!-- Einde resultaatlijst -->\n");			
		}	
	}
	
	if ($toonlijst) {
		
		$rows = $i_el->lijst();
		if (count($rows) > 0) {
			
			echo("<div id='filter'>\n");
			echo("<input type='text' title='Filter naam eigen lijst' placeholder='Filter op naam' OnKeyUp=\"fnFilter('overzichteigenlijsten', this);\">\n");
			if (count($rows) > 2) {
				printf("<p class='aantrecords'>%d lijsten</p>\n", count($rows));
			}
			printf("<button type='submit' class='%s' name='Toevoegen' value='lijst_toevoegen'>%s Eigen lijst</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
			echo("</div>  <!-- Einde filter -->\n");
			
			printf("<table id='overzichteigenlijsten' class='%s'>\n", TABLECLASSES);
			echo("<tr><th></th><th>Naam</th><th># records</th><th># kolommen</th><th>Tabblad</th><th>Laatste controle</th><th></th></tr>\n");
			foreach($rows as $row) {
				$bl1 = sprintf("<a href='%s?tp=%s&paramID=%d&paramActie=2'><i class='bi bi-pencil-square' style='font-size: 14pt;'></i></a>", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
				$bl2 = sprintf("<a href='%s?tp=%s&paramID=%d&paramActie=3'><i class='bi bi-trash' alt='Verwijderen'></i></a>", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
				printf("<tr><td>%s</td><td>%s</td><td class='number'>%d</td><td class='number'>%d</td><td>%s</td><td>%s</td><td>%s</td>", $bl1, $row->Naam, $row->AantalRecords, $row->AantalKolommen, $row->Tabpage, date("d-m-Y H:i", strtotime($row->LaatsteControle)), $bl2);
				echo("</tr>\n");
			}
			echo("</table>\n");
		}
		
	}
	$i_mh = null;
	
}  # eigenlijstenmuteren

function downloadwijzigingen() {
	$copytext = "";
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['afmelden'])) {
			(new cls_interface())->afmelden();
		}
	}
	
	$kols[] = array('headertext' => "Betreft lid", 'columnname' => "betreftLid");
	$kols[]['headertext'] = "ingevoerd";
	$kols[]['headertext'] = "SQL-statement";
	
	$l = sprintf("/admin.php?op=deleteint&recid=%%d&tp=%s", urlencode($_GET['tp']));
	$kols[] = array('columnname' => "RecordID", 'link' => $l, 'class' => "trash");

	printf("<form method='post' id='%s' action='%s?tp=%s'>\n", __FUNCTION__, $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
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
		echo("<div id='opdrachtknoppen'>\n");
		echo("<button name='kopieer' onClick='CopyFunction()'>Kopieer naar klembord</button>\n");
		echo("<button name='afmelden' type='submit'>Wijzigingen afmelden</button>\n");
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
	}
	echo("</form>\n");
	
}  # downloadwijzigingen

function fnTemplatesmuteren() {
	
	$i_tp = new cls_Template();
	$i_tp->controle();
	
	$seltp = $_GET['tpid'] ?? -1;
	
	if (isset($_POST['inhoud']) and $seltp > 0) {
		$i_tp->update($seltp, "Inhoud", $_POST['inhoud']);
	}
	
	$kols[] = array('columnname' => "RecordID", 'headertext' => "#", 'type' => "pk");
	$kols[] = array('columnname' => "Naam", 'headertext' => "Naam");
	$kols[] = array('columnname' => "RecordID", 'type' => "link", 'headertext' => "&nbsp;", 'class' => "muteren", 'link' => sprintf("%s?tp=%s&op=edittemplate&tpid=%%d", $_SERVER['PHP_SELF'], $_GET['tp']));
	
	$rows = $i_tp->basislijst();
	
	echo(fnDisplayTable($rows, $kols, "", 0, "", "templatesmuteren", "", $seltp));
	
	if (isset($_GET['op']) and $_GET['op'] == "edittemplate" and $seltp > 0) {
		$i_tp->vulvars($seltp);
		printf("<form method='post' id='templateedit' action='%s?tp=%s&op=edittemplate&tpid=%d'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $seltp);
		printf("<textarea name='inhoud'>%s</textarea>\n", $i_tp->inhoud);
		echo("<div id='opdrachtknoppen'>\n");
		printf("<button type='submit' class='%s' value='Bewaren'>%s Bewaren</button>\n", CLASSBUTTON, ICONBEWAAR);
		echo("</div>\n");
		echo("</form>\n");
	}
	
}  # fnTemplatesmuteren

function onderhoud() {
	
	$op = $_GET['op'] ?? "";
	
	if ($op == "ledenonderdelenbijwerken") {
		$i_lo = new cls_Lidond();
		$i_lo->autogroepenbijwerken(0, 5);
		$i_lo->auto_einde(0, 5);
		$i_lo->controle();
		$i_lo->opschonen();
		$i_lo = null;
	
		$i_aanw = new cls_Aanwezigheid();
		$i_aanw->controle();
		$i_aanw->opschonen();
		$i_aanw = null;
	
	} elseif ($op == "beheeronderdelen") {
		(new cls_Onderdeel())->opschonen();
		(new cls_Onderdeel())->controle();
		(new cls_Groep())->opschonen();
		(new cls_Groep())->controle();
		(new cls_Functie())->opschonen();
		(new cls_Functie())->controle();
		(new cls_Stukken())->controle();
		(new cls_Stukken())->opschonen();
		(new cls_Organisatie())->opschonen();
		(new cls_Organisatie())->controle();
		(new cls_Activiteit())->opschonen();
		(new cls_Activiteit())->controle();
		(new cls_Eigen_lijst())->controle(-1, 30, 30);
		(new cls_Eigen_lijst())->opschonen();

	} elseif ($op == "logboekopschonen") {
		(new cls_Logboek())->controle();
		(new cls_Logboek())->opschonen();

	} elseif ($op == "ledenopschonen") {
		(new cls_Lid())->controle();
		(new cls_Lid())->opschonen();

		(new cls_Lidmaatschap())->controle();
		(new cls_Lidmaatschap())->opschonen();
	
		(new cls_Memo())->controle();
		(new cls_Memo())->opschonen();

		(new cls_Foto())->controle();
		(new cls_Foto())->opschonen();

		(new cls_Inschrijving())->controle();
		(new cls_Inschrijving())->opschonen();
	
	} elseif ($op == "beheerdiplomas") {
		$i_dp = new cls_Diploma();
		$i_dp->controle();
		$i_dp->opschonen();
		$i_dp = null;

		$i_ld = new cls_Liddipl();
		$i_ld->controle();
		$i_ld->opschonen();
		$i_ld = null;

		$i_ex = new cls_Examen();
		$i_ex->controle();
		$i_ex->opschonen();
		$i_ex = null;

	} elseif ($op == "mailingsopschonen") {
		(new cls_Mailing())->controle();
		(new cls_Mailing())->opschonen();

		(new cls_Mailing_rcpt())->controle();
		(new cls_Mailing_rcpt())->opschonen();
		
		(new cls_Mailing_hist())->controle();
		(new cls_Mailing_hist())->opschonen();
		
		(new cls_Mailing_vanaf())->controle();
		(new cls_Mailing_vanaf())->opschonen();
		
	} elseif ($op == "evenementenopschonen") {
		(new cls_Evenement())->controle();
		(new cls_Evenement())->opschonen();

		(new cls_Evenement_Deelnemer())->controle();
		(new cls_Evenement_Deelnemer())->opschonen();

		(new cls_Evenement_Type())->controle();
		(new cls_Evenement_Type())->opschonen();

	} elseif ($op == "rekeningenopschonen") {
		(new cls_Rekeningregel())->controle();
		(new cls_Rekeningregel())->opschonen();

		(new cls_Rekening())->controle();
		(new cls_Rekening())->opschonen();

		(new cls_RekeningBetaling())->controle();
		(new cls_RekeningBetaling())->opschonen();

		(new cls_Seizoen())->controle();
		(new cls_Seizoen())->opschonen();

	} elseif ($op == "loginsopschonen") {
		(new cls_Login())->opschonen();

	} elseif ($op == "autorisatieopschonen") {
		(new cls_Authorisation())->controle();
		(new cls_Authorisation())->opschonen();
	
	} elseif ($op == "webshopopschonen") {
		(new cls_Orderregel())->controle();
		(new cls_Orderregel())->opschonen();
	
		(new cls_Artikel())->controle();
		(new cls_Artikel())->opschonen();
	
		(new cls_Voorraadboeking())->controle();
		(new cls_Voorraadboeking())->opschonen();
	}
	
	if (isset($_POST['logboek_bewaartijd']) and $_POST['logboek_bewaartijd'] > 0) {
		(new cls_Parameter())->update("logboek_bewaartijd", $_POST['logboek_bewaartijd']);
	}
	
	$f = "TypeActiviteit=3";
	$laatstebackup = (new cls_Logboek())->max("DatumTijd", $f);
	
	printf("<form method='GET' id='%s'>\n", __FUNCTION__);
	printf("<input type='hidden' name='tp' value='%s'>\n", $_GET['tp']);
	
	printf("<button type='submit' class='%s' name='op' value='backup'>Backup</button><p>Maak een backup van de database. Laatste backup is op %s gemaakt.</p>\n", CLASSBUTTON, $laatstebackup);
	printf("<button type='submit' class='%s' name='op' value='FreeBackupFiles'>Vrijgeven backup-bestanden</button><p>Geef de backup-bestanden vrij door middel van een chmod 0755.</p>\n", CLASSBUTTON);
	printf("<button type='submit' class='%s' name='op' value='ledenonderdelenbijwerken'>Beheer leden van onderdelen</button><p>Beheer van leden van onderdelen en presentie.</p>\n", CLASSBUTTON);
	printf("<button type='submit' class='%s' name='op' value='beheeronderdelen'>Beheer onderdelen</button><p>Controle en opschonen van onderdelen, activiteiten, afdelingsgroepen, functies, stukken en organisaties.</p>\n", CLASSBUTTON);
	printf("<button type='submit' class='%s' name='op' value='logboekopschonen'>Logboek opschonen</button><p>Opschonen van het logboek, op basis van diverse regels.</p>\n", CLASSBUTTON);
	printf("<button type='submit' class='%s' name='op' value='ledenopschonen'>Leden en lidmaatschappen</button><p>Controle en opschonen leden, lidmaatschappen, memo's, foto's en inschrijvingen.</p>\n", CLASSBUTTON);
	printf("<button type='submit' class='%s' name='op' value='beheerdiplomas'>Onderhoud diploma's</button><p>Controle en opschonen van diploma's en leden per diploma en examens.</p>\n", CLASSBUTTON);
	printf("<button type='submit' class='%s' name='op' value='mailingsopschonen'>Onderhoud mailings	</button><p>Controle en opschonen van mailings en verzonden e-mails.</p>\n", CLASSBUTTON);
	printf("<button type='submit' class='%s' name='op' value='evenementenopschonen'>Onderhoud evenementen</button><p>Controle en opschonen evenementen.</p>\n", CLASSBUTTON);
	printf("<button type='submit' class='%s' name='op' value='rekeningenopschonen'>Onderhoud rekeningen en betalingen</button><p>Controle en opschonen van rekeningen, rekeningregels, betalingen en seizoenen.</p>\n", CLASSBUTTON);
	printf("<button type='submit' class='%s' name='op' value='loginsopschonen'>Logins opschonen</button><p>Opschonen van logins die om diverse redenen niet meer nodig zijn.</p>\n", CLASSBUTTON);
	printf("<button type='submit' class='%s' name='op' value='autorisatieopschonen'>Autorisatie opschonen</button><p>Verwijderen toegang waar alleen de webmaster toegang toe heeft en die ouder dan 3 maanden zijn.</p>\n", CLASSBUTTON);
	if ((new cls_Orderregel())->aantal() > 0 or (new cls_Artikel())->aantal() > 0) {
		printf("<button type='submit' class='%s' name='op' value='webshopopschonen'>Webshop opschonen</button><p>Opschonen van de artikelen, bestellingen en voorraadboekingen.</p>\n", CLASSBUTTON);
	}
	echo("</form>\n");
	
	printf("<div id='versies'>PHP: %s / Database: %s</div>  <!-- Einde versies -->\n", substr(phpversion(), 0, 6), (new cls_db_base())->versiedb());
	
}  # onderhoud

function logboek() {
	global $TypeActiviteit;
	
	$tp = $_GET['tp'] ?? "";
	
	$i_lb = new cls_logboek();

	$kols[0]['sortcolumn'] = "RecordID";
	$kols[1]['sortcolumn'] = "Omschrijving";
	$kols[3]['sortcolumn'] = "Type";
	$kols[4]['sortcolumn'] = "ingelogdLid";
	$kols[6]['sortcolumn'] = "scriptFunctie";
	$kols[7]['sortcolumn'] = "A.IP_adres";
	
	$ord = fnOrderBy($kols);
	
	$tekstfilter = $_GET['tekstfilter'] ?? "";
	$typefilter = $_GET['typefilter'] ?? -1;
	$kolomfilter = $_GET['kolomfilter'] ?? "";
	$referidfilter = $_GET['referidfilter'] ?? -1;
	$aantalrijen = $_GET['aantalrijen'] ?? 1500;
	$alleeningelogd = $_GET['alleeningelogd'] ?? 0;
	
	$f = "";
	if (strlen($tekstfilter) > 1 and $aantalrijen >= 1500) {
		$f = sprintf("Omschrijving LIKE '%%%s%%'", $tekstfilter);
	}
	if (strlen($kolomfilter) > 0) {
		if (strlen($f) > 0) {
			$f .= " AND ";
		}
		$f .= sprintf("CONCAT(A.RefTable, '-', A.refColumn)='%s'", $kolomfilter);
	}
	if ($referidfilter >= 0) {
		if (strlen($f) > 0) {
			$f .= " AND ";
		}
		$f .= sprintf("A.ReferID=%d", $referidfilter);
	}
	if ($alleeningelogd == 1) {
		if (strlen($f) > 0) {
			$f .= " AND ";
		}
		$f .= "A.LidID > 0";
	}
	$rows = $i_lb->lijst($typefilter, 0, 0, $f, $ord, $aantalrijen);
	
	printf("<form class='form-check form-switch' method='GET' id='filter'>\n<input type='hidden' name='tp' value='%s'>\n", $tp);
	
	printf("<input type='text' id='tbTekstFilter' name='tekstfilter' title='Filter tabel' value='%s' placeholder='Tekst filter' OnKeyUp=\"fnFilter('%s', this);\">\n", $tekstfilter, __FUNCTION__);
	
	echo("<select name='typefilter' class='form-select form-select-sm' onchange='this.form.submit();'>\n");
	echo("<option value=-1>Filter op type ....</option>\n");
	foreach ($TypeActiviteit as $key => $val) {
		$f = sprintf("TypeActiviteit=%d", $key);
		if ($i_lb->aantal($f) > 0) {
			printf("<option value=%d %s>%s</option>\n", $key, checked($key, "option", $typefilter), $val);
		}
	}
	echo("</select>\n");
			
	echo("<select name='kolomfilter' class='form-select form-select-sm' onchange='this.form.submit();'>\n");
	echo("<option value=''>Filter op tabel/kolom ....</option>\n");
	$f = "IFNULL(A.refColumn, '') > ''";
	if ($_POST['typefilter'] > 0) {
		$f .= sprintf(" AND TypeActiviteit=%d", $_POST['typefilter']);
	}
	foreach ($i_lb->uniekelijst("A.RefTable, A.refColumn", $f) as $row) {
		printf("<option value='%1\$s-%2\$s' %3\$s>%1\$s->%2\$s</option>\n", $row->RefTable, $row->refColumn, checked($row->RefTable . "-" . $row->refColumn, "option", $kolomfilter));
	}
	echo("</select>\n");
	
	printf("<input type='number' name='referidfilter' value=%d title='Filter op ReferID (-1 = geen filter)' onBlur='this.form.submit();'>", $referidfilter);

	$options = "";
	$ta = $i_lb->aantal();
	$va = 0;
	foreach (array(25, 100, 250, 750, 1500, 3000, 10000, 25000, 50000) as $a) {
		if ($ta > $va) {
			$options .= sprintf("<option value=%d %s>%s</option>\n", $a, checked($a, "option", $aantalrijen), number_format($a, 0, ",", "."));
		}
		$va = $a;
	}
	printf("<span><label class='form-label'>Aantal rijen</label><select name='aantalrijen' class='form-select form-select-sm' OnChange='this.form.submit();'>%s</select></span>\n", $options);
	printf("<label class='form-check-label'><input type='checkbox' class='form-check-input' name='alleeningelogd'%s value=1 onClick='this.form.submit();'>Alleen ingelogde</label>\n", checked($alleeningelogd));
	
	if (count($rows) > 1) {
		printf("<p class='aantrecords'>%s van %s rijen</p>\n", number_format(count($rows), 0, ",", "."), number_format($i_lb->aantal(), 0, ",", "."));
	}
	echo("</form>\n");
	
	$kols = fnStandaardKols(__FUNCTION__, 1, $rows);
	
	echo(fnDisplayTable($rows, $kols, "", 0, "", __FUNCTION__));
	
	$i_lb = null;
	
}  # logboek

?>

