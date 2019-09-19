<?php

function fnTaak() {
	global $currenttab, $currenttab2;

	if (isset($_GET['op'])) {
		$op = $_GET['op'];
	} else {
		$op = "";
	}
	
	if (isset($_GET['tid']) and is_numeric($_GET['tid'])) {
		$taakid = $_GET['tid'];
	} elseif (isset($_POST['tid']) and is_numeric($_POST['tid'])) {
		$taakid = $_POST['tid'];
	} else {
		$taakid = 0;
	}
	
	fnDispMenu(2);
	
	if ($currenttab2 == "Taakbeheer") {
		if ($op == "TaakEdit") {
			fnTaakEdit($taakid);
		} else {
			fnTaakbeheer();
		}
	} elseif ($currenttab2 == "Taakgroepen") {
		fnTaakgroepen();
	} else {
		
	}
}

function fnTaakbeheer() {
	global $currenttab, $currenttab2;
	
	$ldl = sprintf("<a href='index.php?tp=%s/%s&amp;op=TaakEdit&amp;tid=%s'>%s</a>", $currenttab, $currenttab2, "%d", "%s");
	
	echo(fnDisplayTable(db_taak("lijst"), $ldl));
	
}

function fnTaakEdit($tid) {
	
	echo("<div id='edittaakform'>\n");
	
	$row = db_taak("record", $tid);
	
	printf("<p><label>Nummer:</label><input type='text' readonly name='RecordID' value=%d></p>\n", $tid);
	printf("<p><label>Omschrijving:</label><input type='text' name='Omschrijving' value='%s'></p>\n", $row->Omschrijving);
	$selectgroep = "<select name='taakgroepid'>\n";
	$query = sprintf("SELECT RecordID, CONCAT(Kode, ' - ', Naam) AS Oms FROM %sTaakgroep ORDER BY Kode;", TABLE_PREFIX);
	$result = fnQuery($query);
	foreach ($result->fetchAll() as $rowsel) {
		if ($rowsel->RecordID == $row->Taakgroep) {
			$s = " selected";
		} else {
			$s = "";
		}
		$selectgroep .= sprintf();
	}
	echo("</div>  <!-- Einde edittaakform -->\n");
	
}

function fnTaakgroepen() {
	
	$mess = "";
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['kode_nw']) and strlen($_POST['kode_nw']) > 0) {
			$id = db_taakgroep("add", 0, $_POST['kode_nw']);
		} else {
			foreach (db_taakgroep("lijst") as $row) {
				$kode = sprintf("kode_%d", $row->RecordID);
				$naam = sprintf("naam_%d", $row->RecordID);
				if (isset($_POST[$kode]) and $_POST[$kode] != $row->Kode) {
					db_taakgroep("update", $row->RecordID, $_POST[$kode], "Kode");
				}
				if (isset($_POST[$naam]) and $_POST[$naam] != $row->Naam) {
					db_taakgroep("update", $row->RecordID, $_POST[$naam], "Naam");
				}
			}
		}
	}
	if (strlen($mess) > 0) {
		db_logboek("add", $mess, 18, 0, 1);
	}
	
	printf("<form method='post' action='%s?tp=%s' name='BrowseTaakgroep'>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']));
	echo("<table>\n");
	echo("<tr><th>RecordID</th><th>Kode</th><th>Naam</th></tr>\n");
	foreach (db_taakgroep("lijst") as $row) {
		echo("<tr>");
//		printf("<td><a href='%s?tp=%s&amp;op=delete&amp;tid=%d' title='Verwijder groep'><img src='./images/del.png' title='Verwijder groep'></a></td>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $row->RecordID);
		printf("<td class='number'>%d</td>\n", $row->RecordID);
		printf("<td><input type='text' size=8 name='kode_%d' value='%s' onBlur='this.form.submit();'></td>\n", $row->RecordID, $row->Kode);
		printf("<td><input type='text' name='naam_%d' value='%s' onBlur='this.form.submit();'></td>\n", $row->RecordID, $row->Naam);
		echo("</tr>\n");
	}
	echo("<tr>");
//	echo("<td><img src='./images/star.png' title='Nieuwe taakgroep'></td>\n");
	echo("<td class='number'>Nieuw</td><td><input type='text' size=8 name='kode_nw' onBlur='this.form.submit();'></td>");
	echo("</tr>\n");
	echo("</table>\n");
	echo("</form>\n");
	
}

?>
