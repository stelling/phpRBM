<?php
include('includes/standaard.inc');

if (isset($_GET['op']) and $_GET['op'] == "downloadwijz") {
	header("Content-type: text/plain");
	header("Content-Disposition: attachment; filename=import.sql");
	foreach(db_interface() as $row) {
		echo($row->SQL . "\n");
	}
	exit();
}
	
HTMLheader();
	
if (isset($_GET['op']) and $_GET['op'] == "deletelogin") {
	db_logins("delete", "", "", $_GET['lidid']);
	printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $currenttab);
} elseif (isset($_GET['op']) and $_GET['op'] == "unlocklogin") {
	db_logins("unlock", "", "", $_GET['lidid']);
	printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $currenttab);
} elseif (isset($_POST['tabpage_nw']) and strlen($_POST['tabpage_nw']) > 0) {
	db_authorisation("add", 0, $_POST['tabpage_nw']);
} elseif (isset($_GET['op']) and $_GET['op'] == "deleteautorisatie") {
	db_authorisation("delete", $_GET['recid']);
	printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $currenttab);
} elseif (isset($_GET['op']) and $_GET['op'] == "changeaccess") {
	foreach(db_authorisation("lijst") as $row) {
		$vn = sprintf("toegang%d", $row->RecordID);
		if (isset($_POST[$vn]) and $_POST[$vn] != $row->Toegang) {
			$query = sprintf("UPDATE %1\$sAdmin_access SET Toegang=%2\$d, Gewijzigd=SYSDATE() WHERE RecordID=%3\$d AND Toegang<>%2\$d;", $table_prefix, $_POST[$vn], $row->RecordID);
			$result = fnQuery($query);
			if ($result > 0) {
				$mess = sprintf("Toegang '%s' is naar groep '%s' aangepast.", $row->Tabpage, db_naam_onderdeel($_POST[$vn], "Iedereen"));
				db_logboek("add", $mess, 5);
			}
		}
	}
} elseif (isset($_GET['op']) and $_GET['op'] == "uploaddata") {
	if (isset($_FILES['SQLupload']['name']) and strlen($_FILES['SQLupload']['name']) > 3) {
		db_delete_local_tables();
		fnQuery("SET CHARACTER SET utf8;");
		$queries = file_get_contents($_FILES['SQLupload']["tmp_name"]);
		if ($queries !== false) {
			echo("<p class='mededeling'>Bestand is succesvol ge-upload.</p>\n");
			if (fnQuery($queries) !== true) {
				echo("<p class='mededeling'>Bestand is in de database verwerkt.</p>\n");
				db_logboek("add", "Upload data uit Access-database.", 9);
				db_onderhoud();
				printf("<script>setTimeout(\"location.href='%s';\", 15000);</script>\n", $_SERVER['PHP_SELF']);
			}
		}
	}
} elseif (isset($_GET['op']) and $_GET['op'] == "afmeldenwijz") {
	$mess = db_interface("afmelden");
	printf("<p class='mededeling'>%s</p>\n", $mess);
}

if ($currenttab == "Beheer logins" and toegang($_GET['tp'])) {
	db_createtables();
	db_onderhoud();
	$lnk = sprintf("<a href='%s?op=deletelogin&amp;lidid=%s'><img src='images/del.png' title='Verwijder login'></a>", $_SERVER['PHP_SELF'], "%d");
	if ($maxinlogpogingen > 0) {
		$lnk_lk = sprintf("<a href='%s?op=unlocklogin&amp;lidid=%s' title='Reset foutieve logins'><img src='images/unlocked_01.png'></a>", $_SERVER['PHP_SELF'], "%d");
	} else {
		$lnk_lk = "";
	}
	echo(fnDisplayTable(db_logins("lijst"), $lnk, "", 5, $lnk_lk));
} elseif ($currenttab == "Autorisatie" and toegang($_GET['tp'])) {
	echo("<div id='lijst'>\n");
	printf("<form name='formauth' method='post' action='%s?tp=%s&amp;op=changeaccess'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<table>\n");
	echo("<tr><th></th><th>Onderdeel</th><th>Toegankelijk voor</th></tr>\n");
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
		printf("<tr><td>%s</td><td>%s</td><td><select name='toegang%d' onchange='this.form.submit();'>%s</select></td></tr>\n", $del, $row->Tabpage, $row->RecordID, $selectopt);
	}
	$optionstab = "<option value=''>Selecteer ...</options>";
	foreach (db_authorisation("tabpages") as $row) {
		$optionstab .= sprintf("<option value='%1\$s'>%1\$s</option>\n", $row->Tabpage);
	}
	printf("<tr><td><img src='images/star.png' alt='Ster' title='Nieuw record'></td><td><select name='tabpage_nw' onChange='this.form.submit();'>%s</select></td><td></td></tr>\n", $optionstab);
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde lijst -->\n");
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
	printf("<form name='formupload' method='post' action='%s?%s&amp;op=uploaddata' enctype='multipart/form-data'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	echo("<table>\n");
	echo("<tr><td class='label'>Bestand</td><td><input type='file' name='SQLupload'><input type='submit' value='Verwerk'></td></tr>\n");
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde invulformulier -->\n");	
} elseif ($currenttab == "Downloaden wijzigingen" and toegang($_GET['tp'])) {
	printf("<form name='formdownload' method='post' action='%s?op=downloadwijz'>\n", $_SERVER['PHP_SELF']);
	$rows = db_interface("lijst");
	if (count($rows) > 0) {
		echo(fnDisplayTable($rows, "", "", 1));
		echo("<p class='mededeling'><input type='submit' value='Download wijzigingen'>\n");
		printf("&nbsp;<input type='button' value='Wijzigingen afmelden' OnClick=\"location.href='%s?tp=%s&amp;op=afmeldenwijz'\"></p>\n", $_SERVER['PHP_SELF'], $currenttab);
	} else {
		echo("<p class='mededeling'>Er zijn geen wijzigingen die nog verwerkt moeten worden.</p>\n");
	}
	echo("</form>\n");
} elseif ($currenttab == "DB backup" and toegang($_GET['tp'])) {
	$mess = db_backup();
	printf("<p class='mededeling'>%s</p>\n", $mess);

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
		if ($key == $_POST['typefilter']) {
			$s = "selected";
		} else {
			$s = "";
		}
		printf("<option value=%d %s>%s</option>\n", $key, $s, htmlentities($val));
	}
	echo("</select>\n</td>\n");
	
	echo("</tr>\n</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde filter -->\n");
	
	$rows = db_logboek("lijst", "", $_POST['typefilter'], $_POST['lidfilter']);
	echo(fnDisplayTable($rows, "", "", 0, "", "", "logboek"));
} elseif (toegang("Info")) {
	phpinfo();
}

HTMLfooter();

?>
