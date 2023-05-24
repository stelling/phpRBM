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
	
	$i_taak = new cls_taak();
	
	$ldl = sprintf("<a href='index.php?tp=%s/%s&amp;op=TaakEdit&amp;tid=%s'>%s</a>", $currenttab, $currenttab2, "%d", "%s");
	
	echo(fnDisplayTable($i_taak->lijst()));
	
}

function fnTaakEdit($p_tid) {
	
	echo("<div id='edittaakform'>\n");
	
	$i_taak = new cls_taak();
	$row = $i_taak->record($p_tid);
	
	printf("<p><label>Nummer:</label><input type='text' readonly name='RecordID' value=%d></p>\n", $p_tid);
	printf("<p><label>Omschrijving:</label><input type='text' name='Omschrijving' value='%s'></p>\n", $row->Omschrijving);
	$selectgroep = "<select name='taakgroepid'>\n";
	$query = sprintf("SELECT RecordID, CONCAT(Kode, ' - ', Naam) AS Oms FROM %sTaakgroep ORDER BY Kode;", TABLE_PREFIX);
	$result = $this->execsql($query);
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
		(new cls_Logboek())->add($mess, 18, 0, 1);
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


// Tijdens de ontwikkeling worden deze functies tijdelijk hier geplaatst. Uiteindelijk moeten deze verplaatst worden naar database.inc
class cls_taak extends cls_db_base {
	
	public function lijst() {
		$sq = sprintf("SELECT IFNULL(%1\$s, 'Niemand') FROM %2\$sLid AS L WHERE L.Nummer=(SELECT MAX(LidID) FROM %2\$sTaakLid AS TL WHERE TL.TaakID=T.RecordID AND Eindverantwoordelijke=1)", $this->selectnaam, TABLE_PREFIX);
		$query = sprintf("SELECT T.RecordID AS lnkNummer, Omschrijving, TG.Naam AS Groep, GeplandGereed AS `Planning gereed`, (%1\$s) AS Verantwoordelijke FROM %2\$sTaak AS T INNER JOIN %2\$sTaakgroep AS TG ON TG.RecordID=T.Taakgroep
		ORDER BY T.Omschrijving;", $sq, TABLE_PREFIX);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
		
	public function record($p_tid) {
		$query = sprintf("SELECT * FROM %sTaak WHERE RecordID=%d;", TABLE_PREFIX, $p_tid);
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
} # cls_taak

function db_taakgroep($actie, $rid=0, $waarde="", $kolomnm="") {
	if ($actie == "lijst") {
		$query = sprintf("SELECT RecordID, Kode, Naam FROM %sTaakgroep ORDER BY Naam;", TABLE_PREFIX);
		$result = $this->execsql($query);
		return $result->fetchAll();
	} elseif ($actie == "add") {
		$query = sprintf("INSERT INTO %sTaakgroep SET Kode='%s', IngevoerdDoor='%s';", TABLE_PREFIX, $waarde, $_SESSION['lidid']);
		$id = (new cls_db_base())->execsql($query);
		$mess = sprintf("Taakgroep %d met kode '%s' is toegevoegd.", $id, $waarde);
		(new cls_Logboek())->add($mess, 18);
		return $id;
		
	} elseif ($actie == "update" and strlen($kolomnm) > 0) {
		$query = sprintf("UPDATE %1\$sTaakgroep SET `%2\$s`='%3\$s', GewijzigdDoor=%4\$d WHERE RecordID=%5\$d;", TABLE_PREFIX, $kolomnm, $waarde, $_SESSION['lidid'], $rid);
		if ((new cls_db_base())->execsql($query) > 0) {
			$mess = sprintf("Kolom '%s' in Taakgroep %d is in '%s' gewijzigd.", $kolomnm, $rid, $waarde);
			(new cls_Logboek())->add($mess, 18);
			return true;
		} else {
			return false;
		}
	}
} # db_taakgroep

?>
