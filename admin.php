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
	db_delete_login($_GET['lidid']);
	printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $currenttab);
} elseif (isset($_GET['op']) and $_GET['op'] == "deleteautorisatie") {
	db_delete_authorisation($_GET['recid']);
	printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $currenttab);
} elseif (isset($_GET['op']) and $_GET['op'] == "changeaccess") {
	$query = sprintf("SELECT RecordID, Toegang, Tabpage FROM %sAdmin_access;", $table_prefix);
	$result = fnQuery($query);
	foreach($result->fetchAll() as $row) {
		$vn = sprintf("toegang%d", $row->RecordID);
		if (isset($_POST[$vn]) and $_POST[$vn] != $row->Toegang) {
			$query = sprintf('UPDATE %1$sAdmin_access SET Toegang=%2$d, Gewijzigd=SYSDATE() WHERE RecordID=%3$d AND Toegang<>%2$d;', $table_prefix, $_POST[$vn], $row->RecordID);
			$result = fnQuery($query);
			if ($result > 0) {
				db_add_activiteit(sprintf("Toegang '%s' is naar groep '%s' aangepast.", $row->Tabpage, db_naam_onderdeel($_POST[$vn])), 5);
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
				db_add_activiteit("Upload nieuwe data uit Access-database is gebeurd.", 2);
				db_onderhoud();
				printf("<script>setTimeout(\"location.href='%s';\", 15000);</script>\n", $_SERVER['PHP_SELF']);
			}
		}
	}
} elseif (isset($_GET['op']) and $_GET['op'] == "afmeldenwijz") {
	db_interface("afmelden");
//	debug("afmelden");
}

if ($currenttab == "Beheer logins" and toegang($_GET['tp'])) {
	db_createtables();
	db_onderhoud();
	echo(fnDiplayTable(db_logins(), "<a href='" . $_SERVER['PHP_SELF'] . "?op=deletelogin&amp;lidid=%d'><img src='./images/trash.png' alt='Trash bin' title='Verwijder login'></a>"));
} elseif ($currenttab == "Autorisatie" and toegang($_GET['tp'])) {
	echo("<div id='lijst'>\n");
	printf("<form name='formauth' method='post' action='%s?tp=%s&amp;op=changeaccess'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<table>\n");
	echo("<tr><th></th><th>Onderdeel</th><th>Toegankelijk voor</th></tr>\n");
	$query = "SELECT RecordID, Tabpage, Toegang FROM Admin_access ORDER BY Tabpage;";
	$result = fnQuery($query);
	foreach($result->fetchAll() as $row) {
		$del = sprintf("<a href='%s?tp=%s&amp;op=deleteautorisatie&amp;recid=%d'><img src='./images/trash.png' alt='Trash bin' title='Verwijder record'></a>", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
		$selectopt = "<option value=-1>Alleen webmasters</option>\n";
		if ($row->Toegang == 0) {
			$selectopt .= "<option value=0 selected>Iedereen</option>\n";
		} else {
			$selectopt .= "<option value=0>Iedereen</option>\n";
		}
		foreach(db_Onderdelen() as $ond) {
			if ($row->Toegang == $ond->RecordID) { $s = " selected"; } else { $s = ""; }
			$selectopt .= sprintf("<option value=%d%s>%s</option>", $ond->RecordID, $s, $ond->Naam);
		}
		printf("<tr><td>%s</td><td>%s</td><td><select name='toegang%d' onchange='form.submit()'>%s</select></td></tr>\n", $del, $row->Tabpage, $row->RecordID, $selectopt);
	}
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde lijst -->\n");
} elseif ($currenttab == "Uploaden data" and ($_SESSION['aantallid'] == 0 or toegang($_GET['tp']))) {
	echo("<div id='invulformulier'>\n");
	printf("<form name='formupload' method='post' action='%s?%s&amp;op=uploaddata' enctype='multipart/form-data'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	echo("<table>\n");
	echo("<tr><td class='label'>Bestand</td><td><input type='file' name='SQLupload' size=55><input type='submit' value='Verwerk'></td></tr>\n");
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde invulformulier -->\n");	
} elseif ($currenttab == "Downloaden wijzigingen" and toegang($_GET['tp'])) {
//	printf("<form name='formdownload' method='post' action='%s?%s&amp;op=downloadwijz'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	printf("<form name='formdownload' method='post' action='%s?op=downloadwijz'>\n", $_SERVER['PHP_SELF']);
	echo("<table>\n");
	echo("<tr><th>Naam lid</th><th>Datum wijziging</th><th>SQL</th></tr>\n");
	$counter = 0;
	foreach(db_interface() as $row) {
		printf("<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n", $row->NaamLid, strftime("%e %h %Y (%H:%S)", strtotime($row->Ingevoerd)), $row->SQL);
		$counter++;
	}
	echo("</table>\n");
	if ($counter > 0) {
		echo("<p class='mededeling'><input type='submit' value='Download wijzigingen'>\n");
		printf("&nbsp;<input type='button' value='Wijzigingen afmelden' OnClick=\"location.href='%s?tp=%s&amp;op=afmeldenwijz'\"></p>\n", $_SERVER['PHP_SELF'], $currenttab);
	} else {
		echo("<p class='mededeling'>Er zijn geen wijzigingen die nog verwerkt moeten worden.</p>\n");
	}
	echo("</form>\n");
} elseif ($currenttab == "DB backup" and toegang($_GET['tp'])) {
	$mess = db_backup();
	printf("<p class='mededeling'>%s</p>\n", $mess);

	echo("<script>\n");
	printf("setTimeout(\"location.href='%s';\", 10000);\n", $_SERVER['PHP_SELF']);
	echo("</script>\n");
} elseif ($currenttab == "Logboek" and toegang($_GET['tp'])) {
	echo(fnDiplayTable(db_lijstactiviteiten()));
} elseif ($currenttab == "Beginpagina") {
	echo("<script>location.href='/';</script>\n");
}

HTMLfooter();

?>
