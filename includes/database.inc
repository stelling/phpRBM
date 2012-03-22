<?php

$arrTables[] = "Admin_access";
$arrTables[] = "Admin_activiteit";
$arrTables[] = "Admin_interface";
$arrTables[] = "Admin_login";
$arrTables[] = "Bewaking";
$arrTables[] = "Bewaking_Blok";
$arrTables[] = "Bewaking_Inschrijving";
$arrTables[] = "Bewseiz";
$arrTables[] = "Boekjaar";
$arrTables[] = "Diploma";
$arrTables[] = "Functie";
$arrTables[] = "GBR";
$arrTables[] = "Lid";
$arrTables[] = "Memo";
$arrTables[] = "Liddipl";
$arrTables[] = "Lidmaatschap";
$arrTables[] = "Lidond";
$arrTables[] = "LidRedNed";
$arrTables[] = "Mailing";
$arrTables[] = "Mailing_hist";
$arrTables[] = "Mailing_rcpt";
$arrTables[] = "Mutatie";
$arrTables[] = "Kostenplaats";
$arrTables[] = "Onderdl";
$arrTables[] = "Rekening";
// $arrTables[] = "DMS_Document";
// $arrTables[] = "DMS_Folder";

$TypeActiviteit[] = "N.T.B.";
$TypeActiviteit[] = "Inloggen/Uitloggen";
$TypeActiviteit[] = "DB onderhoud";
$TypeActiviteit[] = "DB backup";
$TypeActiviteit[] = "Mailing";
$TypeActiviteit[] = "Autorisaties";
$TypeActiviteit[] = "Self-service";  // 6
$TypeActiviteit[] = "Evenementen";
$TypeActiviteit[] = "Interface";
$TypeActiviteit[99] = "Debugging";

$fdlang = "'%e %M %Y'";
$fdkort = "'%e-%c-%Y'";
$fdaccess = "%m/%d/%Y";
$fdsql = "%Y-%m-%d";

if (strlen($table_prefix) > 0 and substr($table_prefix, -1) != "_") {
	$table_prefix .= "_";
}

$selectnaam = "CONCAT(IF(LENGTH(L.Roepnaam)>1, L.Roepnaam, L.Voorletter), ' ', IF(L.Tussenv>'', CONCAT(L.Tussenv, ' '), ''), L.Achternaam, IF(L.Meisjesnm>'', CONCAT('-', L.Meisjesnm), ''))";
$selectzoeknaam = "CONCAT(L.Achternaam, ', ', IF(L.Tussenv>'', CONCAT(L.Tussenv, ' '), ''), IF(LENGTH(L.Roepnaam)>1, L.Roepnaam, L.Voorletter), ' ')";
$fromlid = sprintf('%1$sLid AS L LEFT OUTER JOIN %1$sLidmaatschap AS LM ON L.Nummer = LM.Lid', $table_prefix);
$wherelid = "LM.LIDDATUM <= CURDATE() AND ((LM.Opgezegd IS NULL) OR LM.Opgezegd >= CURDATE())";
$fromlidond = sprintf('%1$sLid AS L INNER JOIN ((%1$sLidond AS LO INNER JOIN %1$sFunctie AS F ON LO.Functie = F.Nummer) INNER JOIN %1$sOnderdl AS O ON LO.OnderdeelID = O.RecordID) ON L.NUMMER = LO.LID', $table_prefix);
$wherelidond = "LO.Vanaf <= CURDATE() AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd >= CURDATE())";

$db_conn_pdo = sprintf("mysql:host=%s;dbname=%s", $db_host, $db_name);
try {
	$dbc = new PDO($db_conn_pdo, $db_user, $db_pass);
	$dbc->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_OBJ);
	$dbc->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {
	echo("<p>Connection failed: " . $e->getMessage() . "</p>\n");
}

if (TableExists(sprintf("%sLid", $table_prefix)) == false) {
	db_createtables();
	db_onderhoud();
	$_SESSION['aantallid'] = 0;
} else if (!isset($_SESSION['aantallid']) or $_SESSION['aantallid'] == 0) {
	$query = sprintf("SELECT COUNT(*) FROM %sLid;", $table_prefix);
	$result = fnQuery($query);
	$_SESSION['aantallid'] = $result->fetchColumn();
}

if (!isset($_SESSION['aantallogin']) or $_SESSION['aantallogin'] == 0) {
	$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login;", $table_prefix);
	$result = fnQuery($query);
	$_SESSION['aantallogin'] = $result->fetchColumn();
}

if (!isset($_SESSION['aantalafdelingen'])) {
	$query = sprintf("SELECT COUNT(*) FROM %sOnderdl WHERE `Type`='A';", $table_prefix);
	$_SESSION['aantalafdelingen'] = db_scalar($query);
}

if (!isset($_SESSION['aantalrollen'])) {
	$query = sprintf("SELECT COUNT(*) FROM %sOnderdl WHERE `Type`='R';", $table_prefix);
	$_SESSION['aantalrollen'] = db_scalar($query);
}

if (!isset($_SESSION['aantalbewaking'])) {
	$query = sprintf("SELECT COUNT(*) FROM %sBewaking;", $table_prefix);
	$_SESSION['aantalbewaking'] = db_scalar($query);
}

if (!isset($_SESSION['aantalrekeningen'])) {
	$query = sprintf("SELECT COUNT(*) FROM %sRekening;", $table_prefix);
	$_SESSION['aantalrekeningen'] = db_scalar($query);
}

if (!isset($_SESSION['aantalmutaties'])) {
	$query = sprintf("SELECT COUNT(*) FROM %sMutatie;", $table_prefix);
	$_SESSION['aantalmutaties'] = db_scalar($query);
}

if (!isset($_SESSION['aantalkostenplaatsen'])) {
	$query = sprintf("SELECT COUNT(DISTINCT KostenplaatsID) FROM %sMutatie WHERE KostenplaatsID > 0;", $table_prefix);
	$_SESSION['aantalkostenplaatsen'] = db_scalar($query);
}

function fnQuery($query, $add_prefix=1) {
   global $dbc, $table_prefix, $arrTables;
	
	$comm[] = "FROM ";
	$comm[] = "JOIN ";
	$comm[] = "INSERT INTO ";
	$comm[] = "UPDATE ";
	
	if (strlen($table_prefix) > 1 and strlen($query) > 1 and $add_prefix == 1) {
		foreach($arrTables as $tn) {
			foreach($comm as $cm) {
				$query = str_replace($cm . $tn, $cm . $table_prefix . $tn, $query);
			}
		}
	}

	try {
		if (strtoupper(substr($query, 0, 6)) == "SELECT") {
			$rs = $dbc->query($query);
			return $rs;
		} else {			
			$result = $dbc->prepare($query);
			$result->execute();
			if (strtoupper(substr($query, 0, 6)) == "INSERT") {
				return $dbc->lastInsertId();
			} else {
				return $result->RowCount();
			}
		}
	} catch (Exception $e) {
		$mess = sprintf("Error in SQL '%s': %s", $query, $e->getMessage());
		printf("<p class=''mededeling'>%s</p>\n", $mess);
		db_logboek("add", $mess, 99);
		return false;
	}
}

function db_scalar($query) {
	$result = fnQuery($query);
	return $result->fetchColumn();
}

function db_gegevenslid($LidID, $soort="Alg") {
	global $selectnaam, $table_prefix, $fromlid, $fromlidond, $wherelidond;

	if ($soort == "Alg") {
		$sqond = sprintf("SELECT MIN(O.Naam) FROM %1\$sOnderdl AS O INNER JOIN %1\$sLidond AS LO ON O.RecordID = LO.OnderdeelID
								WHERE O.`Type` = 'O' AND LO.Lid = L.Nummer AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd >= CURDATE())", $table_prefix);
		$sqlogin = sprintf("SELECT MAX(Login) FROM %sAdmin_login AS AL WHERE AL.LidID = L.Nummer", $table_prefix);
		$sqlbd = sprintf("SELECT MAX(EINDE_PER) FROM %sBewaking WHERE Lid=L.Nummer AND BEGIN_PER < CURDATE()", $table_prefix);
		$query = sprintf("SELECT `Waarschuwen bij nood` FROM %sLid WHERE Nummer=%d;", $table_prefix, $LidID);
		$result = fnQuery($query);
		if (strlen($result->fetchColumn()) > 1) {
			$wbn = " 'Waarschuwen bij nood' as Kop6, `Waarschuwen bij nood` AS memNood,";
		} else {
			$wbn = "";
		}
		$query = sprintf("SELECT 'Persoonsgegevens' AS Kop1,"
				 . " %s AS Naam,"
				 . " L.Voorletter AS Voorletters,"
				 . " L.Roepnaam AS ndRoepnaam,"
				 . " CASE L.Geslacht WHEN 'V' THEN 'Vrouwelijk' WHEN 'M' THEN 'Mannelijk' WHEN 'B' THEN 'Bedrijf' ELSE 'Onbekend' END AS Geslacht,"
				 . " L.Overleden AS `dteOverleden op`,"
				 . " IF(NOT ISNULL(L.GEBDATUM), CONCAT(DATE_FORMAT(L.GEBDATUM, '%s'), IF(LENGTH(L.GEBPLAATS) > 1, CONCAT(' te ', L.GEBPLAATS), '')), '') AS Geboren,"
				 . " 'Adresgegevens' AS Kop2,"
				 . " L.Adres,"
				 . " IF(LENGTH(L.Woonplaats) > 1, CONCAT(L.Postcode, '  ', L.Woonplaats), '') AS `Postcode en woonplaats`,"
				 . " 'Bereiken' AS Kop3,"
				 . " L.Telefoon,"
				 . " L.Mobiel,"
				 . " L.Email AS `emlE-mail`,"
				 . " L.EmailVereniging AS `emlE-mail vereniging`,"
				 . " 'Lidmaatschap' AS Kop4,"
				 . " LM.Lidnr AS Lidnummer,"
				 . " (%s) AS Onderscheiding,"
				 . " LM.LIDDATUM AS `dteLid vanaf`,"
				 . " LM.Opgezegd AS `dteLid totenmet`,"
				 . " 'Overig' AS Kop5,"
				 . " L.RelnrRedNed AS `Relatienummer RedNed`,"
				 . " (%s) AS Login,"
				 . " L.Beroep AS `Beroep`,"
				 . " (%s) AS `dteLaatste bewakingsdag`,"
				 . " %s"
				 . " 'Recordinformatie' as Kop7,"
				 . " L.Nummer AS `RecordID`,"
				 . " L.Ingevoerd AS `dteIngevoerd op`,"
				 . " L.Gewijzigd AS `dteLaatst gewijzigd op`"
				 . " FROM %s"
				 . " WHERE L.Nummer=%d"
				 . " ORDER BY LM.LIDDATUM DESC;", $selectnaam, "%e %M %Y", $sqond, $sqlogin, $sqlbd, $wbn, $fromlid, $LidID);
	} elseif ($soort == "Financieel") {			 
		$query = sprintf("SELECT L.BANKGIRO AS Bankrekeningnummer,"
				 . " L.`Machtiging afgegeven` AS `booMachtiging afgegeven?`"
				 . " FROM %sLid AS L"
				 . " WHERE L.Nummer=%d;", $table_prefix, $LidID);
	} elseif ($soort == "Afdelingen") {
		$query = sprintf("SELECT O.Kode AS Code,
					O.Naam,
					LO.Vanaf AS dteVanaf,
					F.Omschrijv AS Functie,
					LO.Opmerk AS Opmerking,
					LO.Opgezegd AS dteOpgezegd,
					CONCAT(ROUND(DATEDIFF(IF(LO.Opgezegd IS NULL, CURDATE(), LO.Opgezegd), LO.Vanaf)/365.25, 0), ' jaar') AS numDuur
					FROM %s
					WHERE O.`Type`='A' AND LO.Lid=%d
					ORDER BY LO.Vanaf DESC, O.Naam;", $fromlidond, $LidID);
	} elseif ($soort == "Kader") {
		$query = sprintf("SELECT O.Kode AS Code,
					O.Naam,
					LO.Vanaf AS dteVanaf,
					F.Omschrijv AS Functie,
					LO.Opmerk AS Opmerking,
					LO.Opgezegd AS dteTotenmet,
					CONCAT(ROUND(DATEDIFF(IF(LO.Opgezegd IS NULL, CURDATE(), LO.Opgezegd), LO.Vanaf)/365.25, 0), ' jaar') AS numDuur
					FROM %s
					WHERE O.Kader=1 AND LO.Lid=%d
					ORDER BY LO.Vanaf DESC, O.Naam;", $fromlidond, $LidID);
	} elseif ($soort == "Rollen") {
		$query = sprintf("SELECT O.Kode AS Code,"
				 . " O.Naam AS Omschrijving,"
				 . " LO.Vanaf AS dteVanaf"
				 . " FROM %s"
				 . " WHERE %s"
				 . " AND O.`Type` = 'R' AND LO.Lid=%d"
				 . " ORDER BY LO.Vanaf, O.Kode;", $fromlidond, $wherelidond, $LidID);
	} elseif ($soort == "Groepen") {
		$query = sprintf("SELECT O.Kode AS Code,"
				 . " O.Naam AS Omschrijving,"
				 . " LO.Vanaf AS dteVanaf,"
				 . " IF(LO.Opgezegd IS NULL, LO.Opmerk, CONCAT('t/m ', LO.Opgezegd)) AS Opmerking"
				 . " FROM %s"
				 . " WHERE %s AND O.`Type`='G' AND O.GekoppeldAanQuery=0 AND LO.Lid=%d"
				 . " ORDER BY LO.Vanaf, O.Kode;", $fromlidond, $wherelidond, $LidID);
	} elseif ($soort == "Lidbew") {
		$tw = sprintf("CONVERT((SELECT SUM(DATEDIFF(EINDE_PER, BEGIN_PER)+1) FROM %sBewaking AS BW WHERE BW.Lid = %d AND BEGIN_PER < SYSDATE())/7, SIGNED)", $table_prefix, $LidID);
		$ts = sprintf("(SELECT COUNT(DISTINCT SeizoenID) FROM %sBewaking AS BW WHERE BW.Lid = %d AND BEGIN_PER < CURDATE())", $table_prefix, $LidID);
		$query = sprintf("SELECT CONCAT(%1\$s, ' weken in ', %2\$s, ' seizoen(en)') AS `Totale bewaking`,
								(SELECT Memo FROM %3\$sMemo WHERE Soort='B' AND Lid=%4\$d) AS Opmerking,
								(SELECT Memo FROM %3\$sMemo WHERE Soort='D' AND Lid=%4\$d) AS Dieet;", $tw, $ts, $table_prefix, $LidID);
	} elseif ($soort == "Bew") {
		$query = sprintf("SELECT BS.Kode AS Seizoen,
								Weeknr AS intWeek,
								BEGIN_PER AS dteBegin,
								EINDE_PER AS dteEinde,
								DATEDIFF(EINDE_PER, BEGIN_PER) + 1 AS intDagen,
								Post,
								F.Omschrijv AS Functie
								FROM %1\$sBewseiz AS BS INNER JOIN (%1\$sBewaking AS BW INNER JOIN %1\$sFunctie AS F ON BW.Functie = F.Nummer) ON BS.RecordID = BW.SeizoenID
								WHERE Lid=%2\$d AND BS.Begindatum <= CURDATE()
								ORDER BY BS.Begindatum DESC, BS.Lokatie, BW.BEGIN_PER DESC;", $table_prefix, $LidID);
	} elseif ($soort == "Rekening") {
		$query = sprintf("SELECT Nummer,"
				 . " Datum AS dteDatum,"
				 . " OMSCHRIJV AS Omschrijving,"
				 . " Bedrag AS curBedrag,"
				 . " Betaald as curBetaald,"
				 . " (Bedrag-Betaald) AS curOpenstaand"
				 . " FROM %sRekening"
				 . " WHERE Lid=%d"
				 . " ORDER BY Nummer DESC;", $table_prefix, $LidID);
	} elseif ($soort == "Mailing") {
		$query = sprintf("SELECT send_on AS dteVerzonden,"
				 . " IF(LENGTH(from_name)<3, from_addr, from_name) AS Van,"
				 . " to_name AS Aan,"
				 . " subject AS Onderwerp,"
				 . " RecordID AS lnkRecordID"
				 . " FROM %sMailing_hist"
				 . " WHERE LidID=%d"
				 . " ORDER BY send_on DESC LIMIT 25;", $table_prefix, $LidID);
	} else {
		$query = "";
	}
	
	if (strlen($query) > 1) {
		$result = fnQuery($query);
		return $result->fetchAll();
	} else {
		return false;
	}
}

function db_naamlid($lidid, $uitv="Naam") {
	global $selectnaam, $selectzoeknaam, $table_prefix;
	
	if ($uitv == "Naam") {
		$query = sprintf("SELECT %s FROM %sLid AS L WHERE L.Nummer=%d;", $selectnaam, $table_prefix, $lidid);
	} else {
		$query = sprintf("SELECT %s FROM %sLid AS L WHERE L.Nummer=%d;", $selectzoeknaam, $table_prefix, $lidid);
	}
	return db_scalar($query);
}

function db_adressenlijst($filter = "O.Kader=True") {
	global $selectnaam, $fromlidond, $wherelidond;
	
	if (strlen($filter) > 3) {
		$filter = $wherelidond . " AND " . $filter;
	} else {
		$filter = $wherelidond;
	}
	$query = sprintf("SELECT L.*, %s AS LidNaam, O.Kode, O.Naam AS OndNaam, O.CentraalEmail, O.TYPE, F.Omschrijv AS OmsFunctie, LO.*
							FROM %s
							WHERE %s 
							ORDER BY O.Kader DESC, O.TYPE, O.Naam, F.Sorteringsvolgorde, LO.Vanaf, L.Achternaam, L.TUSSENV;"
							, $selectnaam, $fromlidond, $filter);
	$result = fnQuery($query);
	return($result->fetchAll());
}

function db_ledenlijst($filter="") {
	global $selectnaam, $selectzoeknaam, $table_prefix;
	
	if (strlen($filter) > 0) {
		$filter = " WHERE " . $filter;
	}
	$query = sprintf("SELECT L.Nummer AS lnkNummer, LM.Lidnr, %1\$s AS `Naam_lid`, UPPER(L.Woonplaats) AS Woonplaats,
							L.Telefoon, L.Mobiel, L.EMAIL AS `emlE-mail`, L.GEBDATUM AS ndGEBDATUM, %2\$s AS ndZoeknaam
							FROM %3\$sLid AS L LEFT OUTER JOIN %3\$sLidmaatschap AS LM ON L.Nummer = LM.Lid
							%4\$s
							ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, LM.LIDDATUM DESC;", $selectnaam, $selectzoeknaam, $table_prefix, $filter);
	$result = fnQuery($query);
	return($result->fetchAll());
}

function db_lid($filter="") {
	global $selectnaam, $selectzoeknaam, $table_prefix;
	
	if (strlen($filter) > 0) {
		$filter = " WHERE " . $filter;
	}
	$query = sprintf("SELECT L.Nummer, %1\$s AS `Naam_lid`, UPPER(L.Woonplaats) AS Woonplaats,
							L.Telefoon, L.Mobiel, L.EMAIL AS `emlE-mail`, L.GEBDATUM AS GEBDATUM, %2\$s AS Zoeknaam
							FROM %3\$sLid AS L
							%4\$s
							ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, L.Nummer DESC;", $selectnaam, $selectzoeknaam, $table_prefix, $filter);
	$result = fnQuery($query);
	return($result->fetchAll());
}

function db_ledenwijzigingen($lidid) {
	global $selectnaam, $table_prefix;
	
	$query = sprintf("SELECT L.Roepnaam, 
				L.VOORLETTER,
				L.Tussenv,
				L.Achternaam,
				L.Meisjesnm,
				L.Adres,
				L.Postcode,
				L.Woonplaats,
				L.Telefoon,
				L.Mobiel,
				L.EMAIL,
				L.GEBDATUM,
				L.GEBPLAATS,
				L.BANKGIRO,
				L.Beroep,
				L.Legitimatietype,
				L.Legitimatienummer
				FROM %sLid AS L WHERE Nummer=%d;", $table_prefix, $lidid);
	$result = fnQuery($query);
	
	return $result->fetch();
}

function db_memo($lidid, $srt, $actie="curval", $nv="") {
	global $table_prefix, $muteerbarememos;
	
	$nv = str_replace("\"", "'", $nv);
	
	if ($actie == "curval" and $lidid > 0) {
		$query = sprintf("SELECT MAX(Memo) FROM %sMemo WHERE Lid=%d AND Soort='%s';", $table_prefix, $lidid, $srt);
		$result = fnQuery($query);
		return $result->fetchColumn();
	} elseif ($actie == "insert" and strlen($nv) > 0) {
		$query = sprintf("INSERT INTO %sMemo (Lid, Soort, Memo, Ingevoerd, Gewijzigd) VALUES (%d, '%s', \"%s\", CURDATE(), SYSDATE());", $table_prefix, $lidid, $srt, $nv);
		$result = fnQuery($query);
		db_interface("add", $query);
	} elseif ($actie == "update" and strlen($nv) > 0) {
		$query = sprintf("UPDATE %sMemo SET Memo=\"%s\", Gewijzigd=SYSDATE() WHERE Lid=%d AND Soort='%s';", $table_prefix, $nv, $lidid, $srt);
		$result = fnQuery($query);
		if ($result > 0) {
			db_interface("add", $query);
		}
	} elseif ($actie == "delete") {
		$query = sprintf("DELETE FROM %sMemo WHERE Lid=%d AND Soort='%s';", $table_prefix, $lidid, $srt);
		$result = fnQuery($query);
		if ($result > 0) {
			db_interface("add", $query);
		}
	}
}

function db_add_login($login, $lidid) {
	global $selectnaam, $lididwebmasters, $beperktotgroep, $table_prefix, $naamvereniging, $dbc;
	
	$login = cleanlogin($login);
	$query = sprintf("SELECT Login FROM %sAdmin_login WHERE LidID=%d;", $table_prefix, $lidid);
	$result = fnQuery($query);
	$l = $result->fetchColumn();
	$ok = 0;
	if (strlen($l) == 0) {		
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login WHERE Login='%s' AND LidID<>%d;", $table_prefix, $login, $lidid);
		$result = fnQuery($query);
		if ($result->fetchColumn() > 0) {
			$mess = sprintf("Login '%s' is al in gebruik bij een ander lid.", $login);
			db_logboek("add", $mess, 1);
		} elseif (strlen($login) > 5 and $lidid > 0) {
			$query = sprintf("INSERT INTO %sAdmin_login SET LidID=%d, Login='%s', Ingevoerd=CURDATE(), Gewijzigd=SYSDATE();", $table_prefix, $lidid, $login);
			
			fnQuery($query);
			$ok = 1;
			$mess = sprintf("Login '%s' is aangemaakt.", $login);
			db_logboek("add", $mess, 1);
		}
	}
	
//	Lege wachtwoorden invullen
	$query = sprintf("SELECT * FROM %sAdmin_login WHERE ((Wachtwoord IS NULL) OR LENGTH(Wachtwoord) < 6) AND (openid_identity IS NULL);", $table_prefix);
	$result = fnQuery($query);
   foreach ($result->fetchAll() as $row) {
		$WW = strtoupper(substr($row->Login, 0, 1)) . strtolower(substr($row->Login, 1, 4)) . rand(101, 997);
		$query = sprintf("UPDATE %sAdmin_login SET Wachtwoord='%s', Gewijzigd=SYSDATE() WHERE Login='%s';", $table_prefix, encript_decript($WW), $row->Login);
		fnQuery($query);
	}
	
	return $ok;
}

function db_lidnr_bevestiging($email="") {
	global $selectnaam, $table_prefix;

	$query = sprintf("SELECT %1\$s AS Naam, L.EMAIL, L.EmailVereniging, LM.Lidnr, L.Roepnaam
			FROM %2\$sLid AS L INNER JOIN %2\$sLidmaatschap AS LM ON L.Nummer = LM.Lid
			WHERE LM.Lidnr > 0 AND (LOWER(L.EMAIL)='%3\$s' OR LOWER(L.EmailVereniging)='%3\$s')
			ORDER BY LM.LIDDATUM DESC LIMIT 1;", $selectnaam, $table_prefix, strtolower($email));
	$result = fnQuery($query);
	return $result->fetch();
}

function db_change_password($pw_new, $pw_old) {
	global $table_prefix;
	
	$query = sprintf("UPDATE %sAdmin_login SET"
			 . " Gewijzigd = SYSDATE(),"
			 . " GewijzigdDoor=%d,"
			 . " Wachtwoord='%s'"
			 . " WHERE Wachtwoord='%s' AND Login='%s' AND LidID=%d;"
			 , $table_prefix, $_SESSION['lidid'], encript_decript($pw_new), encript_decript($pw_old), $_SESSION['username'], $_SESSION['lidid']);
	$result = fnQuery($query);
	if ($result > 0) {
		$_SESSION['password'] = $pw_new;
		$mess = sprintf("Het wachtwoord van login '%s' is gewijzigd.", $_SESSION['username']);
		db_logboek("add", $mess, 1);
	} else {
		$mess = "Oude wachtwoord is niet correct, wachtwoord is niet gewijzigd.";
	}	
	return $mess;
}

function db_delete_authorisation($recid) {
	global $table_prefix;
	
	if ($_SESSION['webmaster'] == 1) {
		$query = sprintf("DELETE FROM %sAdmin_access WHERE RecordID=%d;", $table_prefix, $recid);
		fnQuery($query);
	} else {
		echo("<p class='mededeling'>Je hebt geen rechten om authorisaties te verwijderen.</p>\n");
	}
}

function TableExists($table) {
	global $db_name, $table_prefix;
	
	if (startwith($table, $table_prefix) == FALSE) {
		$table = $table_prefix . $table;
	}
	
	$query = sprintf("SELECT COUNT(*) FROM information_schema.tables
							WHERE table_schema='%s' AND table_name='%s';", $db_name, $table);
	$result = fnQuery($query);
	if ($result->fetchColumn() > 0) {
		return true;
	} else {
		return false;
	}
}

function db_bewakingsrooster($seizoen=-1, $week=-1) {
	global $selectnaam, $table_prefix;

	if ($seizoen < 1) {
		$sel_sz = "";
		$sel_wk = "";
		$orderby = "BS.Begindatum DESC, BW.BEGIN_PER, BW.Post, F.Sorteringsvolgorde, L.Achternaam, L.Tussenv, L.Roepnaam";
	} else {	
		$sel_sz = sprintf("WHERE BW.SeizoenID=%d", $seizoen);
		if ($week > 0) {
			$sel_wk = sprintf("AND BW.Weeknr=%d", $week);
		} else {
			$sel_wk = "AND BW.Weeknr > 0";
		}
		$query = sprintf("SELECT ST_LEN_BP FROM %sBewseiz WHERE RecordID=%d;", $table_prefix, $seizoen);
		$result = fnQuery($query);
		if ($result->fetchColumn() == 7) {
			$orderby = "BW.Weeknr, BW.Post, F.Sorteringsvolgorde, BW.BEGIN_PER, L.Achternaam, L.Tussenv, L.Roepnaam";
		} else {
			$orderby = "BW.BEGIN_PER, BW.Post, F.Sorteringsvolgorde, L.Achternaam, L.Tussenv, L.Roepnaam";
		}
	}
	$query = sprintf('SELECT BW.*, %1$s AS NaamBewaker, L.Nummer, F.Omschrijv AS OmsFunctie, F.Afkorting AS AfkFunctie, BS.Kode, BS.Lokatie, BS.ST_LEN_BP, BS.TOONERV,
							DATEDIFF(BW.EINDE_PER, BW.BEGIN_PER)+1 AS Dagen, BW.BEGIN_PER, L.GEBDATUM, 
							(SELECT SUM(DATEDIFF(BH.EINDE_PER, BH.BEGIN_PER)+1) FROM %2$sBewaking BH WHERE BH.LID = BW.LID AND BH.EINDE_PER < BW.BEGIN_PER) AS ErvaringDagen
							FROM ((%2$sBewaking AS BW INNER JOIN %2$sBewseiz AS BS ON BS.RecordID = BW.SeizoenID) INNER JOIN %2$sFunctie AS F ON BW.Functie = F.Nummer) INNER JOIN %2$sLid AS L ON L.Nummer = BW.Lid
							%3$s %4$s
							ORDER BY %5$s;', $selectnaam, $table_prefix, $sel_sz, $sel_wk, $orderby);
	$result = fnQuery($query);
	return $result->fetchAll();
}

function db_bewaking_seizoenen($ins=0) {
	global $table_prefix;

	if ($ins == 1) {
		$query = sprintf("SELECT DISTINCT BS.RecordID, BS.Kode, BS.Gewijzigd, BS.ST_LEN_BP
								FROM %1\$sBewseiz AS BS INNER JOIN %1\$sBewaking_Blok AS BB ON BS.RecordID = BB.SeizoenID
								ORDER BY BS.Begindatum DESC, BS.Kode;", $table_prefix);
	} else {
		$query = sprintf("SELECT BS.RecordID, BS.Kode, BS.Gewijzigd, BS.ST_LEN_BP FROM %sBewseiz AS BS ORDER BY BS.Begindatum DESC, BS.Kode;", $table_prefix);
	}
	$result = fnQuery($query);
	return $result;
}

function db_bewaking_onderdelen_lid($lidid, $per) {
	global $fromlidond;

	$query  = sprintf("SELECT DISTINCT O.Kode
				 FROM %s
				 WHERE O.`Tonen in bewakingsadministratie`=1 AND LO.Lid=%2\$d
				 AND LO.Vanaf <= '%3\$s' AND ((LO.OPGEZEGD IS NULL) OR OPGEZEGD > '%3\$s')
				 ORDER BY O.Kode;", $fromlidond, $lidid, $per);
	$result = fnQuery($query);
	return $result->fetchAll();
}

function db_lidond($actie) {
	global $table_prefix, $fromlidond;

	if ($actie == "laatstgewijzigd") {
		$query = sprintf("SELECT Max(Gewijzigd) FROM %sLidond;", $table_prefix);
		return db_scalar($query);
	} elseif ($actie == "") {
	}
}

function db_bewaking_aantallen($seizoen, $lidid=-1) {
	global $table_prefix;
	
	if ($lidid > -1) {
		if ($seizoen > 0) {
			$xtrawhere = sprintf("AND BW.SeizoenID=%d", $seizoen);
		} else {
			$xtrawhere = "";
		}
		$query = sprintf("SELECT COUNT(*) FROM %sBewaking AS BW WHERE BW.Lid=%d %s;", $table_prefix, $lidid, $xtrawhere);
		$result = fnQuery($query);
		return $result->fetchColumn();
	} else {
		$query = sprintf("SELECT ST_LEN_BP FROM %sBewseiz WHERE RecordID=%d;", $table_prefix, $seizoen);
		$result = fnQuery($query);
		if ($result->fetchColumn() == 7) {
			$query = sprintf("SELECT Weeknr, MIN(BEGIN_PER) AS dteBegin, MAX(EINDE_PER) AS dteEinde, ROUND(SUM(DATEDIFF(BW.EINDE_PER, BW.BEGIN_PER)+1)/7, 0) AS intAantal,
						CONCAT(ROUND(AVG((SELECT IFNULL(SUM(DATEDIFF(BH.EINDE_PER, BH.BEGIN_PER)+1)/7, 0) FROM %1\$sBewaking AS BH WHERE BH.Lid = BW.Lid AND BH.EINDE_PER < BW.BEGIN_PER)), 1), ' weken') AS `numGem. Ervaring`,
						CONCAT(ROUND(AVG(DATEDIFF(BW.BEGIN_PER, L.GEBDATUM))/365.25, 1), ' jaar') AS `numGem. Leeftijd`
						FROM %1\$sBewaking AS BW INNER JOIN %1\$sLid AS L ON BW.Lid = L.Nummer
						WHERE BW.Weeknr > 0 AND BW.SeizoenID=%2\$d
						GROUP BY BW.Weeknr
						ORDER BY BW.Weeknr;", $table_prefix, $seizoen);
		} else {
			$query = sprintf("SELECT BEGIN_PER AS dtlDatum, COUNT(Lid) AS intAantal,
						CONCAT(ROUND(AVG((SELECT IFNULL(SUM(DATEDIFF(BH.EINDE_PER, BH.BEGIN_PER)+1), 0) FROM %1\$sBewaking AS BH WHERE BH.Lid = BW.Lid AND BH.EINDE_PER < BW.BEGIN_PER)), 1), ' dagen') AS `numGem. Ervaring`,
						CONCAT(ROUND(AVG(DATEDIFF(BW.BEGIN_PER, L.GEBDATUM))/365.25, 1), ' jaar') AS `numGem. Leeftijd`
						FROM %1\$sBewaking AS BW INNER JOIN %1\$sLid AS L ON BW.Lid = L.Nummer
						WHERE BW.SeizoenID=%2\$d
						GROUP BY BW.BEGIN_PER
						ORDER BY BW.BEGIN_PER;", $table_prefix, $seizoen);
		}

		$result = fnQuery($query);			
		return $result->fetchAll();
	}
}

function db_insbew($actie, $lidid=0, $insnr=0, $blok=0, $keuze=0, $opmerk="", $seizoen=-1, $week=-1) {
	global $table_prefix, $selectnaam;
	
	$opmerk = str_replace("\"", "'", $opmerk);
	$w = "";
	if ($seizoen > 0) {
		$w = sprintf("WHERE BB.SeizoenID=%d", $seizoen);
	}
	if ($week > 0) {
		if (strlen($w) > 0) {
			$w .= " AND ";
		} else {
			$w = "WHERE ";
		}
		$w .= sprintf("BB.Kode='%d'", $week);
	}
	
	if ($actie == "openblokken") {
		$query = sprintf("SELECT BB.*, BS.Kode AS KodeSeizoen, BS.Lokatie AS Locatie, BS.KeuzesBijInschrijving
								FROM %1\$sBewaking_Blok AS BB INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								WHERE BB.InschrijvingOpen=1 AND BB.Einddatum >= CURDATE()
								ORDER BY BS.Begindatum, BS.Kode, BB.Begindatum;", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();	
	} elseif ($actie == "blokken") {
		$query = sprintf("SELECT BB.*, BS.Kode AS KodeSeizoen, BS.Lokatie AS Locatie, BS.KeuzesBijInschrijving, BB.Kode AS Weeknr
								FROM %1\$sBewaking_Blok AS BB INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								%2\$s
								ORDER BY BS.Begindatum, BS.Kode, BB.Begindatum;", $table_prefix, $w);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "omsblok" and $blok > 0) {
		$query = sprintf("SELECT CONCAT(BS.Kode, '-', BB.Kode) AS OmsBlok
								FROM %1\$sBewaking_Blok AS BB INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								WHERE BB.RecordID=%2\$d;", $table_prefix, $blok);
		$result = fnQuery($query);
		return $result->fetchColumn();
	} elseif ($actie == "overzicht") {
		$query = sprintf("SELECT BI.Lid AS ndLidID, BB.Kode AS Week, BB.Begindatum AS dteBegin, BB.Einddatum AS dteEinde, %2\$s AS Bewaker, BI.Keuze,
								IF(BI.Keuze IN (1, 3), 'Ja', '') AS `Keuze 1`, IF(BI.Keuze IN (2, 3), 'Ja', '') AS `Keuze 2`, BI.Opmerking, BI.Definitief, BI.Ingevoerd
								FROM (%1\$sBewaking_Blok AS BB INNER JOIN (%1\$sBewaking_Inschrijving AS BI INNER JOIN %1\$sLid AS L ON BI.Lid = L.Nummer) ON BB.RecordID = BI.BlokID) INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								%3\$s
								ORDER BY BS.Begindatum, BS.Kode, L.Achternaam, BI.Lid, BB.Begindatum;", $table_prefix, $selectnaam, $w);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "matrixaantallen") {
		$query = sprintf("SELECT BS.Kode AS Seizoen, BB.Kode AS Week, BB.Begindatum AS dteBegin, BB.Einddatum AS dteEinde,
								SUM(CASE WHEN BI.Definitief > BI.Ingevoerd AND BI.Keuze IN (1, 3) THEN 1 ELSE 0 END) AS `Keuze 1`,
								SUM(CASE WHEN BI.Definitief > BI.Ingevoerd AND BI.Keuze=2 THEN 1 ELSE 0 END) AS `Keuze 2`,
								SUM(CASE WHEN BI.Definitief <= BI.Ingevoerd THEN 1 ELSE 0 END) AS `Niet definitief`
								FROM (%1\$sBewaking_Blok AS BB INNER JOIN (%1\$sBewaking_Inschrijving AS BI INNER JOIN %1\$sLid AS L ON BI.Lid = L.Nummer) ON BB.RecordID = BI.BlokID) INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								%2\$s
								GROUP BY BS.Kode, BB.Kode, BB.Begindatum, BB.Einddatum
								ORDER BY BS.Kode, BB.Kode, BB.Begindatum;", $table_prefix, $w);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "overzichtlid") {
		if ($insnr > 0) {
			$w = sprintf("BI.Nummer=%d", $insnr);
		} else {
			$w = sprintf("BI.Lid=%d", $lidid);
		}
		$query = sprintf("SELECT BS.Kode AS Seizoen, BB.Kode AS Week, BB.Begindatum AS dteBegin, BB.Einddatum AS dteEinde,
								IF(BI.Keuze=3, 1, BI.Keuze) AS Keuze, BI.Opmerking, %2\$s AS ndNaamBewaker, L.Roepnaam AS ndRoepnaamBewaker, L.EMAIL AS ndEmailBewaker
								FROM (%1\$sBewaking_Blok AS BB INNER JOIN (%1\$sBewaking_Inschrijving AS BI INNER JOIN %1\$sLid AS L ON BI.Lid = L.Nummer) ON BB.RecordID = BI.BlokID) INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								WHERE BB.Einddatum > CURDATE() AND %3\$s AND BI.Definitief > BI.Ingevoerd
								ORDER BY BS.Kode, BB.Kode, BB.Begindatum;", $table_prefix, $selectnaam, $w);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "inschrijving") {
		$query = sprintf("SELECT BI.* FROM %sBewaking_Inschrijving AS BI
								WHERE BI.Lid=%d AND BI.BlokID=%d;", $table_prefix, $lidid, $blok);
		$result = fnQuery($query);
		return $result->fetch();
	} elseif ($actie == "export") {
		$query = sprintf("SELECT %1\$s AS NaamLid, BI.Lid, BB.SeizoenID, BB.Begindatum, BB.Einddatum
								FROM (%2\$sBewaking_Inschrijving AS BI INNER JOIN %2\$sLid AS L ON BI.Lid = L.Nummer) INNER JOIN %2\$sBewaking_Blok AS BB ON BI.BlokID = BB.RecordID
								WHERE BI.Afgemeld < BI.Ingevoerd AND BI.Definitief > BI.Ingevoerd AND BI.Keuze IN (1, 3)
								ORDER BY L.Achternaam, L.Tussenv, L.Roepnaam, BB.Begindatum;", $selectnaam, $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "add" and $lidid > 0 and $blok > 0 and $insnr > 0) {
		$query = sprintf("INSERT INTO %sBewaking_Inschrijving (BlokID, Lid, Nummer, Keuze, Opmerking, Ingevoerd)
								VALUES (%d, %d, %d, %d, \"%s\", CURDATE());", $table_prefix, $blok, $lidid, $insnr, $keuze, $opmerk);
		fnQuery($query);
		if ($keuze > 0) {
			$mess = sprintf("Inschrijving bewaking voor blok %s met keuze %d is bij ingelogd lid toegevoegd.", db_insbew("omsblok", 0, 0, $blok), $keuze);
			if (strlen($opmerk) > 1) {
				$mess .= sprintf(" Opmerking is '%s'.", $opmerk); 
			}
			db_logboek("add", $mess, 6);
		}
	} elseif ($actie == "update" and $lidid > 0 and $blok > 0) {
		$query = sprintf("UPDATE %sBewaking_Inschrijving SET Nummer=%d, Keuze=%d, Opmerking=\"%s\", Definitief=NULL
								WHERE BlokID=%d AND Lid=%d;", $table_prefix, $insnr, $keuze, $opmerk, $blok, $lidid);
		fnQuery($query);
		$mess = sprintf("Inschrijving bewaking voor blok %s is bij ingelogd lid bijgewerkt. Keuze is %d.", db_insbew("omsblok", 0, 0, $blok), $keuze);
		if (strlen($opmerk) > 0) {
			$mess .= sprintf(" Opmerking is '%s'.", $opmerk);
		}
		db_logboek("add", $mess, 6);
	} elseif ($actie == "delete" and $lidid > 0 and $blok > 0) {
		$query = sprintf("DELETE FROM %sBewaking_Inschrijving WHERE BlokID=%d AND Lid=%d;", $table_prefix, $blok, $lidid);
		fnQuery($query);
		$mess = sprintf("Inschrijving bewaking voor blok %s is bij %s verwijderd.", db_insbew("omsblok", 0, 0, $blok), db_naamlid($lidid));
		db_logboek("add", $mess, 6);
	} elseif ($actie == "defmaken" and $lidid > 0 and $insnr > 0) {
		$query = sprintf("UPDATE %sBewaking_Inschrijving SET Definitief=SYSDATE() WHERE Nummer=%d AND Lid=%d;", $table_prefix, $insnr, $lidid);
		fnQuery($query);
	} elseif ($actie == "afmelden") {
		$query = sprintf("UPDATE %sBewaking_Inschrijving AS BI SET Afgemeld=SYSDATE()
								WHERE BI.Afgemeld < BI.Ingevoerd AND BI.Definitief > BI.Ingevoerd AND BI.Keuze IN (1, 3);", $table_prefix);
		$result = fnQuery($query);
		$mess = sprintf("Er zijn %d inschrijvingen voor bewaking afgemeld.", $result);
		db_logboek("add", $mess, 6);
		return $mess;
	}
}

function db_stats($lidid=0) {
	global $fromlid, $wherelid, $fromlidond, $wherelidond, $selectnaam, $table_prefix;

	$base = sprintf("SELECT COUNT(*) FROM %s WHERE %s", $fromlid, $wherelid);
	
	$result = fnQuery($base . ";");
	$stats['aantalleden'] = $result->fetchColumn();
	
	$result = fnQuery($base . " AND L.Geslacht='V';");
	$stats['aantalvrouwen'] = $result->fetchColumn();
	
	$result = fnQuery($base . " AND L.Geslacht='M';");
	$stats['aantalmannen'] = $result->fetchColumn();
	
	$query = sprintf("SELECT ROUND(AVG(DATEDIFF(CURDATE(), L.GEBDATUM))/365.25, 1) FROM %s WHERE %s AND (NOT L.GEBDATUM IS NULL);", $fromlid, $wherelid);
	$result = fnQuery($query);
	$stats['gemiddeldeleeftijd'] = $result->fetchColumn();
	
   $query = sprintf("SELECT COUNT(DISTINCT LO.Lid) FROM %s WHERE (O.Kader=1 OR F.Kader=1) AND %s;", $fromlidond, $wherelidond);
	$result = fnQuery($query);
	$stats['aantalkaderleden'] = $result->fetchColumn();
	
	$query = sprintf("SELECT %1\$s AS LidNaam FROM %2\$sLid AS L INNER JOIN %2\$sAdmin_login AS Login ON L.Nummer = Login.LidID"
			 . " ORDER BY Login.Ingevoerd DESC LIMIT 1;", $selectnaam, $table_prefix);
	$result = fnQuery($query);
	$stats['nieuwstelogin'] = $result->fetchColumn();
	
	$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login AS Login;", $table_prefix);
	$result = fnQuery($query);
	$stats['aantallogins'] = $result->fetchColumn();
		
	$query = sprintf("SELECT %1\$s AS LidNaam FROM %2\$sLid AS L INNER JOIN %2\$sAdmin_login AS Login ON L.Nummer = Login.LidID"
			 . " WHERE Login.Ingelogd=1 ORDER BY Login.Ingevoerd DESC;", $selectnaam, $table_prefix);
	$result = fnQuery($query);
	$stats['nuingelogd'] = "";
	foreach($result->fetchAll() as $row) {
		if (strlen($stats['nuingelogd']) > 1) { $stats['nuingelogd'] .= ", "; }
		$stats['nuingelogd'] .= $row->LidNaam;
	}
	
	if ($lidid > 0) {
		$query = sprintf("SELECT L.Roepnaam FROM %sLid AS L WHERE L.Nummer=%d;", $table_prefix, $lidid);
		$stats['roepnaamingelogde'] = db_scalar($query);
		
		$query = sprintf("SELECT L.GEBDATUM FROM %sLid AS L WHERE L.Nummer=%d;", $table_prefix, $lidid);
		$stats['geboortedatumingelogde'] = db_scalar($query);
		
		$query = sprintf("SELECT L.Gewijzigd FROM %sLid AS L WHERE L.Nummer=%d;", $table_prefix, $lidid);
		$filter = sprintf(" WHERE Lid=%d", $lidid);
	} else {
		$stats['roepnaamingelogde'] = "gast";
		
		$query = sprintf("SELECT MAX(L.Gewijzigd) FROM %sLid AS L;", $table_prefix);
		$filter = "";
	}
	$result = fnQuery($query);
	$stats['laatstgewijzigd'] = $result->fetchColumn();
	
	$query = sprintf("SELECT MAX(BW.Gewijzigd) FROM %sBewaking AS BW%s;", $table_prefix, $filter);
	$result = fnQuery($query);
	$lgw = $result->fetchColumn();
	if ($lgw > $stats['laatstgewijzigd']) {
		$stats['laatstgewijzigd'] = $lgw;
	}
	
	$query = sprintf("SELECT MAX(LO.Gewijzigd) FROM %sLidond AS LO%s;", $table_prefix, $filter);
	$result = fnQuery($query);
	$lgw = $result->fetchColumn();
	if ($lgw > $stats['laatstgewijzigd']) {
		$stats['laatstgewijzigd'] = $lgw;
	}
	
	if ($lidid > 0) {
		$query = sprintf("SELECT MAX(LD.Gewijzigd) FROM %sLiddipl AS LD%s;", $table_prefix, $filter);
		$result = fnQuery($query);
		$lgw = $result->fetchColumn();
		if ($lgw > $stats['laatstgewijzigd']) {
			$stats['laatstgewijzigd'] = $lgw;
		}
	}
	
	$query = sprintf("SELECT MAX(RK.Gewijzigd) FROM %sRekening AS RK%s;", $table_prefix, $filter);
	$result = fnQuery($query);
	$lgw = $result->fetchColumn();
	if ($lgw > $stats['laatstgewijzigd']) {
		$stats['laatstgewijzigd'] = $lgw;
	}	
	
	if ($lidid == 0) {
		$query = sprintf("SELECT MAX(M.Gewijzigd) FROM %sMutatie AS M;", $table_prefix);
		$result = fnQuery($query);
		$lgw = $result->fetchColumn();
		if ($lgw > $stats['laatstgewijzigd']) {
			$stats['laatstgewijzigd'] = $lgw;
		}
	}	
	
	return $stats;
}

function db_logins($actie="lijst", $email="", $gewenstelogin="", $lidid=0, $xtrafilter="") {
	global $selectnaam, $beperktotgroep, $table_prefix, $lididwebmasters, $wherelidond, $wherelid, $gebruikopenid, $lidnrnodigbijloginaanvraag;
	global $scriptbijuitloggen;
	
	if ($actie == "nieuwlogin" and isValidMailAddress($email) and strlen($gewenstelogin) > 5) {
		$query = sprintf("SELECT L.Nummer, Login.Login FROM %1\$sLid AS L LEFT OUTER JOIN %1\$sAdmin_login AS Login ON L.Nummer = Login.LidID
								WHERE (LOWER(L.EMAIL)='%2\$s' OR LOWER(L.EmailVereniging)='%2\$s')", $table_prefix, strtolower($email));
		$result = fnQuery($query);
		$row = $result->fetch();
		if ($row->Nummer > 0) {
			if (strlen($row->Login) > 5 and $row->Login != $gewenstelogin) {
				$mess = sprintf("Er is al een ander login aan dit emailadres gekoppeld.");
			} elseif (strlen($gewenstelogin) > 5 and strlen($gewenstelogin <= 15)) {
				$mess = db_add_login($gewenstelogin, $row->Nummer);
			}
		} else {
			$mess = sprintf("E-mailadres '%s' is onbekend in onze database. Er wordt geen login aangemaakt.");
		}
		db_logboek("add", $mess, 1);
		return $mess;
	} elseif ($actie == "rowlogin" and (isValidMailAddress($email, 0) or $lidid > 0)) {
		$query = sprintf("SELECT L.Nummer, Login.Login, %1\$s AS Naam, L.Roepnaam, L.EMAIL, L.EmailVereniging, Login.Wachtwoord,
								(SELECT MAX(Lidnr) FROM %2\$sLidmaatschap AS LM WHERE LM.Lid = L.Nummer AND ((LM.Opgezegd IS NULL) OR LM.Opgezegd > CURDATE())) AS Lidnr,", $selectnaam, $table_prefix);
		if (is_array($beperktotgroep) and count($beperktotgroep) > 0) {
			$query .= sprintf("(SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.Lid=L.Nummer AND LO.OnderdeelID IN (%s) AND LO.Vanaf <= CURDATE() AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd > CURDATE())) AS AanvragenMag", $table_prefix, implode(", ", $beperktotgroep));
		} elseif ($beperktotgroep > 0) {
			$query .= sprintf("(SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.Lid=L.Nummer AND LO.OnderdeelID=%d AND LO.Vanaf <= CURDATE() AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd > CURDATE())) AS AanvragenMag", $table_prefix, $beperktotgroep);
		} else {
			$query .= "1 AS AanvragenMag";
		}
		$query .= sprintf(" FROM %1\$sLid AS L LEFT OUTER JOIN %1\$sAdmin_login AS Login ON L.Nummer = Login.LidID WHERE ", $table_prefix);
		if (isValidMailAddress($email, 0)) {
			$query .= sprintf("(LOWER(L.EMAIL)='%1\$s' OR LOWER(L.EmailVereniging)='%1\$s');", strtolower($email));
		} else {
			$query .= sprintf("L.Nummer=%d;", $lidid);
		}
		$result = fnQuery($query);
		return $result->fetch();
	} elseif ($actie == "controlebestaat" and strlen($gewenstelogin) > 5) {
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login WHERE Login='%s' AND LidID<>%d;", $table_prefix, $gewenstelogin, $lidid);
		return db_scalar($query);
	} elseif ($actie == "uitloggen") {
		$query = sprintf("UPDATE %sAdmin_login SET Ingelogd=0 WHERE Ingelogd=1 AND (LastActivity < ADDDATE(SYSDATE(), INTERVAL -21 MINUTE) OR LidID=%d);", $table_prefix, $lidid);
		$result = fnQuery($query);
		if ($result > 0) {
			if (isset($scriptbijuitloggen) and strlen($scriptbijuitloggen) > 0) {
				@exec($scriptbijuitloggen);
			}
			if (isset($_SESSION['lidid']) and $_SESSION['lidid'] == $lidid and $lidid > 0) {
				$mess = sprintf("%s is uitgelogd.", $_SESSION['naamingelogde']);
				db_logboek("add", $mess, 1);
			}
		}
	} elseif ($actie == "aantalingelogd") {
		db_logins("uitloggen");
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login WHERE Ingelogd=1;", $table_prefix);
		return db_scalar($query);
	} elseif ($actie == "delete" and $lidid > 0) {
		if ($_SESSION['webmaster'] == 1) {
			$query = sprintf("DELETE FROM %sAdmin_login WHERE LidID=%d;", $table_prefix, $lidid);
			$result = fnQuery($query);
			if ($result > 0) {
				$mess = sprintf("De login van %s is verwijderd.", db_naamlid($lidid));
				db_logboek("add", $mess, 1);
				fnQuery(sprintf("OPTIMIZE TABLE %sAdmin_login;", $table_prefix));
			}
		} else {
			echo("<p class='mededeling'>Je hebt geen rechten om logins te verwijderen.</p>\n");
		}
	} elseif ($actie == "unlock" and $lidid > 0 and $_SESSION['webmaster'] == 1) {
		$query = sprintf("UPDATE %sAdmin_login SET FouteLogin=0 WHERE LidID=%d AND FouteLogin > 0;", $table_prefix, $lidid);
		$result = fnQuery($query);
		if ($result == 1) {
			$mess = sprintf("Het aantal foutieve inlogpogingen van de login van %s is gereset.", db_naamlid($lidid));
			db_logboek("add", $mess, 1);
		}
	} elseif ($actie == "lijst" or $actie == "controle") {
		$xtraselect = "";
		$xtrawhere = "";
		if ($actie == "controle") {
			if (isValidMailAddress($_SESSION['username'], 0)) {
				$xtrawhere = sprintf("AND (LOWER(L.EMAIL)='%1\$s' OR LOWER(L.EmailVereniging)='%1\$s')", strtolower($_SESSION['username']));
			} else {
				$xtrawhere = sprintf("AND Login.Login='%s'", cleanlogin($_SESSION['username']));
			}
			$xtrawhere .= sprintf(" AND Login.Wachtwoord='%s' AND LENGTH(Login.Wachtwoord) > 5", encript_decript($_SESSION['password']));
			if (isset($maxinlogpogingen) and $maxinlogpogingen > 0) {
				$xtrawhere .= sprintf(" AND Login.FouteLogin <= %d", $maxinlogpogingen);
			}
			if (is_array($beperktotgroep) and count($beperktotgroep) > 0) {
				$xtrawhere .= sprintf(" AND (Login.LidID IN (SELECT Lid FROM %sLidond AS LO WHERE %s AND LO.OnderdeelID IN (%s)) OR Login.LidID IN (%s))", $table_prefix, $wherelidond, implode(", ", $beperktotgroep), implode(", ", $lididwebmasters));
			} elseif ($beperktotgroep > 0) {
				$xtrawhere .= sprintf(" AND (Login.LidID IN (SELECT Lid FROM %sLidond AS LO WHERE %s AND LO.OnderdeelID=%d) OR Login.LidID IN (%s))", $table_prefix, $wherelidond, $beperktotgroep, implode(", ", $lididwebmasters));
			}
		} elseif (strlen($xtrafilter) > 0) {
			$xtrawhere = " AND " . $xtrafilter;
		}
		$query = sprintf('SELECT Login.LidID AS lnkNummer, Login.LidID, LM.Lidnr, Login.Login, %1$s %2$s AS Naam_lid, L.Roepnaam AS ndRoepnaam,
					L.EMAIL AS `E-mail`, L.EmailVereniging AS `E-mail vereniging`, Login.FouteLogin AS `numFoutieve logins`, Login.Ingevoerd AS dteIngevoerd, Login.LastLogin AS `dttLaatste login`,
					Login.Wachtwoord AS ndWachtwoord, IF(Login.FouteLogin > 0, Login.LidID, 0) AS llkNummer
					FROM %3$sAdmin_login AS Login INNER JOIN (%3$sLid AS L LEFT OUTER JOIN %3$sLidmaatschap AS LM ON L.Nummer = LM.Lid) ON Login.LidID = L.Nummer
					WHERE LENGTH(Login.Wachtwoord) > 5 %4$s
					ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;', $xtraselect, $selectnaam, $table_prefix, $xtrawhere);
		$result = fnQuery($query);
		if ($actie == "controle") {
			$row = $result->fetch();
			if (isset($row->LidID) and $row->LidID > 0) {
				return $row;
			} else {
				$query = sprintf("UPDATE %sAdmin_login SET FouteLogin = FouteLogin + 1 WHERE Login='%s';", $table_prefix, mysql_escape_string($_SESSION['username']));
				fnQuery($query);
				return false;
			}
		} else {
			return $result->fetchAll();
		}
	}
}

function db_rcpt_mailing($mid) {
	global $selectnaam, $selectzoeknaam, $table_prefix;
	
	$query = sprintf("SELECT IFNULL(L.Nummer, 0) AS Nummer, %1\$s AS Naam_lid, %2\$s AS Zoeknaam_lid, L.Adres, L.Postcode, L.Woonplaats, L.EMAIL, L.EmailVereniging, R.to_address"
			 . " FROM %3\$sMailing_rcpt AS R LEFT OUTER JOIN %3\$sLid AS L ON R.LidID = L.Nummer"
			 . " WHERE MailingID=%4\$d"
			 . " ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", $selectnaam, $selectzoeknaam, $table_prefix, $mid);
	$result = fnQuery($query);
	
	return $result->fetchAll();
}

function db_pot_rcpt_mailing($mid) {
	global $selectzoeknaam, $table_prefix;
	
	$query = sprintf("SELECT L.Nummer, %1\$s AS Zoeknaam_lid FROM %2\$sLid AS L"
			 . " WHERE (Verwijderd IS NULL) AND (Overleden IS NULL)"
			 . " AND (NOT EXISTS (SELECT LidID FROM %2\$sMailing_rcpt AS R WHERE R.LidID = L.Nummer AND R.MailingID=%3\$d))"
			 . " ORDER BY L.Achternaam, L.Roepnaam;", $selectzoeknaam, $table_prefix, $mid);
	$result = fnQuery($query);
	
	return $result->fetchAll();
}

function db_onderdelen($type="", $aantal=0) {
	global $fromlidond, $wherelidond;

	if ($aantal == 0) {
		$sel = "DISTINCT O.RecordID, CONCAT(LEFT(O.Naam, 30), ' (', O.Kode, ')') AS Oms, O.Naam";
	} else {
		$sel = "COUNT(DISTINCT O.RecordID)";
	}
	
	$where = $wherelidond;
	if ($type == "bw") {
		$where = $where . " AND O.`Tonen in bewakingsadministratie`=1";
	} elseif (strlen($type) > 0) {
		$where = $where . sprintf(" AND O.`Type`='%s'", $type);
	}
	$query = sprintf("SELECT %s FROM %s WHERE %s ORDER BY O.Naam;", $sel, $fromlidond, $where);
	$result=fnQuery($query);
	
	if ($aantal == 0) {
		return $result->fetchAll();
	} else {
		return $result->fetchColumn();
	}
}

function db_naam_onderdeel($rid, $rin="Onbekend") {
	$query = sprintf("SELECT Naam FROM Onderdl WHERE RecordID=%d;", $rid);
	$result = fnQuery($query);
	$rv = $result->fetchColumn();
	if (strlen($rv) == 0) {
		$rv = $rin;
	}
	return $rv;
}

function db_add_mailing_hist($LidID, $MailingID, $to_name, $to_addr, $message="", $from_addr="", $subject="", $cc_addr="") {
	global $table_prefix;

	$from_name = "";
	if ($MailingID > 0) {
		$query = sprintf("SELECT * FROM %sMailing WHERE MailingID=%d;", $table_prefix, $MailingID);
		$result = fnQuery($query);
		$row = $result->fetch();
		if (strlen($message) == 0) {
			$message = $row->message;
		}
		$from_name = $row->from_name;
		$from_addr = $row->from_addr;
		if (strlen($to_name) == 0) {
			$to_name = $row->to_name;
		}
		$subject = $row->subject;
		$cc_addr = $row->cc_addr;
	}
	$subject = str_replace("\"", "'", $subject);
	$message = str_replace("\"", "'", $message);
	if (isset($_SESSION['lidid']) and $_SESSION['lidid'] > 0) {
		$query = sprintf("INSERT INTO %sMailing_hist SET"
				 . " LidID=%d, MailingID=%d, from_name=\"%s\", from_addr=\"%s\", to_name=\"%s\", subject=\"%s\", to_addr=\"%s\", cc_addr=\"%s\", message=\"%s\", send_by=%d, send_on=SYSDATE();"
				 , $table_prefix, $LidID, $MailingID, $from_name, $from_addr, $to_name, $subject, $to_addr, $cc_addr, $message, $_SESSION['lidid']);
		fnQuery($query);
	}
}

function db_mailing($actie="", $mid=0, $rid=0, $filter="", $subj="", $lidid=0, $email="") {
	global $selectnaam, $table_prefix, $naamvereniging;
	
	if ($actie == "histdetails" and $rid > 0) {
		$query = sprintf("SELECT send_on, from_addr, from_name, to_name, to_addr, cc_addr, subject, message
					FROM %sMailing_hist WHERE RecordID=%d LIMIT 1;", $table_prefix, $rid);
		$result = fnQuery($query);
		return $result->fetch();
	} elseif ($actie == "hist" and $mid > 0) {
		$query = sprintf('SELECT M.subject AS ndOnderwerp, MH.send_on AS dttVerzonden, IF(MH.LidID=0, MH.to_name, %1$s) AS Aan, to_addr AS `E-mail`, L.Woonplaats,
					MH.RecordID AS lnkRecordID
					FROM (%2$sMailing_hist AS MH LEFT OUTER JOIN %2$sLid AS L ON MH.LidID = L.Nummer) INNER JOIN %2$sMailing AS M ON MH.MailingID = M.MailingID
					WHERE MH.MailingID=%3$d
					ORDER BY M.subject, MH.send_on DESC, L.Achternaam, L.Tussenv, MH.to_addr;', $selectnaam, $table_prefix, $mid);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "lijst") {
		$xtra_sel = "";
		$orderby = "sent_on DESC";
		if ($filter == "Concepten" or $filter == "Verzonden") {
			if ($filter == "Verzonden") {
				$w = "(NOT M.sent_on IS NULL)";
				$xtra_sel = ", M.sent_on `dteVerzonden op`";
			} else {
				$w = "(M.sent_on IS NULL)";
			}
			$w .= " AND template=0 AND (deleted_on IS NULL)";
			if ($_SESSION['webmaster'] == 0) {
				$w .= sprintf(' AND (M.AddedBy=%1$d OR M.ChangedBy=%1$d OR M.confidential=0)', $_SESSION['lidid']);
			}
		} elseif ($filter == "Templates") {
			$w = "M.template=1 AND (deleted_on IS NULL)";
			$orderby = "M.subject, sent_on";
		} elseif ($filter == "Prullenbak") {
			$w = "(NOT M.deleted_on IS NULL)";
			$orderby = "M.deleted_on DESC";
			$xtra_sel = ", M.deleted_on `dteVerwijderd op`";
		} else {
			$w = "(M.deleted_on IS NULL)";
		}
		$query = sprintf("SELECT M.MailingID AS lnkMailingID, M.subject AS Onderwerp, M.from_name AS Van, M.to_name AS Aan, %1\$s AS Door %2\$s, M.MailingID AS llkHist
					FROM %3\$sMailing AS M LEFT JOIN %3\$sLid AS L ON M.ChangedBy = L.Nummer
					WHERE %4\$s ORDER BY %5\$s;", $selectnaam, $xtra_sel, $table_prefix, $w, $orderby);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "aantalprullenbak") {
		$query = sprintf("SELECT COUNT(*) FROM %sMailing WHERE (NOT deleted_on IS NULL);", $table_prefix);
		return db_scalar($query);
	} elseif ($actie == "add" and strlen($subj) > 0) {
		$query = sprintf("INSERT INTO %sMailing (from_name, from_addr, subject, template, confidential, new_on, AddedBy)
								VALUES ('%s', '%s', '%s', 0, 0, SYSDATE(), %d);", $table_prefix, $naamvereniging, $email, $subj, $_SESSION['lidid']);
		$id = fnQuery($query);
		$mess = sprintf("Mailing %s (%d) is door %s toegevoegd.", $subj, $id, $_SESSION['naamingelogde']);
		db_logboek("add", $mess, 4);
		return $id;
	} elseif ($actie == "add_rcpt" and $lidid > 0 and $mid > 0) {
		$query = sprintf("SELECT COUNT(*) FROM Mailing_rcpt WHERE MailingID=%d AND LidID=%d;", $mid, $lidid);
		$result = fnQuery($query);
		if ($result->fetchColumn() == 0) {
			$query = sprintf("INSERT INTO Mailing_rcpt SET MailingID=%d, LidID=%d, Ingevoerd=SYSDATE();", $mid, $lidid);
			fnQuery($query);
		}
		
	} elseif ($mid > 0) {
		$query = sprintf("SELECT * FROM %sMailing WHERE MailingID=%d LIMIT 1;", $table_prefix, $mid);
		$result = fnQuery($query);
		$row = $result->fetch();
		if ($actie == "folder") {
			if ($row->deleted_on > "2001-01-01") {
				return "Prullenbak";
			} elseif ($row->template == 1) {
				return "Templates";
			} elseif ($row->sent_on < "2001-01-01") {
				return "Concepten";
			} else {
				return "Verzonden";
			}
		} else {
			return $row;
		}
	}
}

function db_mutatie($jaar, $gbr="*", $kpl="*") {
	global $table_prefix;
	
	$where = sprintf("M.BoekjaarID=%d", $jaar);
	if ($gbr != "*" and strlen($gbr) > 0) {
		$where .= sprintf(" AND M.GBR='%s'", $gbr);
	}
	if ($kpl != "*" and strlen($kpl) > 0 and is_numeric($kpl)) {
		$where .= sprintf(" AND M.KostenplaatsID=%d", $kpl);
	}
	
	if ($_SESSION['aantalkostenplaatsen'] > 0 and (strlen($kpl) == 0 or $kpl == "*")) {
		$sel_kpl = ", M.KSTNPLTS AS Kostenplaats";
	} else {
		$sel_kpl = "";
	}

	$query = sprintf("SELECT J.Kode AS Jaar, CONCAT(GBR.Kode, ' - ', GBR.OMSCHRIJV) AS Grootboekrekening%1\$s, Datum AS dteDatum, M.OMSCHRIJV AS Omschrijving, Debet-Credit AS curBedrag
							FROM ((%2\$sMutatie AS M LEFT OUTER JOIN %2\$sKostenplaats AS KPL ON M.KostenplaatsID = KPL.RecordID) INNER JOIN %2\$sGBR AS GBR ON M.GBR = GBR.Kode) INNER JOIN %2\$sBoekjaar AS J ON M.BoekjaarID = J.RecordID WHERE %3\$s
							ORDER BY J.Kode, GBR.Kode, KPL.Kode, Datum;", $sel_kpl, $table_prefix, $where);
	$result = fnQuery($query);
	return $result->fetchAll();
}

function db_logboek($actie, $oms="", $type=-1, $lidid=0) {
	global $table_prefix, $selectnaam, $selectzoeknaam;
	
	if ($type >= 0) {
		$w = sprintf("TypeActiviteit=%d", $type);
	} else {
		$w = "TypeActiviteit <> 99";
	}
	
	if ($actie == "add" and strlen($oms) > 0) {
		$query = sprintf("INSERT INTO %sAdmin_activiteit SET
				DatumTijd=SYSDATE(),
				LidID=%d,
				IP_adres='%s',
				USER_AGENT='%s',
				Omschrijving=\"%s\",
				TypeActiviteit=%d;", $table_prefix, $_SESSION['lidid'], $_SERVER['REMOTE_ADDR'], $_SERVER['HTTP_USER_AGENT'], $oms, $type);
		fnQuery($query);
	} elseif ($actie == "opschonen") {
		$query = sprintf("DELETE FROM %sAdmin_activiteit WHERE
								(TypeActiviteit=1 AND LidID > 0 AND DatumTijd <= DATE_ADD(CURDATE(), INTERVAL -1 MONTH))
								OR (TypeActiviteit=99 AND DatumTijd <= DATE_ADD(CURDATE(), INTERVAL -1 MONTH))
								OR (DatumTijd <= DATE_ADD(CURDATE(), INTERVAL -18 MONTH));", $table_prefix);
		$result = fnQuery($query);
		if ($result > 1) {
			$mess = sprintf("Er zijn %d records uit het logboek verwijderd.", $result);
			db_logboek("add", $mess, 2);
			$query = sprintf("OPTIMIZE TABLE %sAdmin_activiteit;", $table_prefix);
			fnQuery($query);
		}
	} elseif ($actie == "lijst") {
		if ($type == -1) {
			$s = ", TypeActiviteit AS `Type`";
		} else {
			$s = "";
		}
		if ($lidid > 0) {
			$w .= sprintf(" AND LidID=%d", $lidid);
		} else {
			$s .= sprintf(", A.LidID, %s AS `Ingelogd Lid`", $selectnaam);
		}
	
		$query = sprintf("SELECT A.DatumTijd as `dttDatum en tijd`, Omschrijving, IP_adres%1\$s
								FROM %2\$sAdmin_activiteit AS A LEFT OUTER JOIN %2\$sLid AS L ON A.LidID = L.Nummer
								WHERE %3\$s
								ORDER BY A.DatumTijd DESC, A.RecordID DESC LIMIT 100;", $s, $table_prefix, $w);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "lidlijst") {
		$query = sprintf("SELECT DISTINCT LidID, %1\$s AS Naam
								FROM %2\$sAdmin_activiteit AS A INNER JOIN %2\$sLid AS L ON A.LidID = L.Nummer
								WHERE %3\$s
								ORDER BY %1\$s;", $selectzoeknaam, $table_prefix, $w);
		$result = fnQuery($query);
		return $result->fetchAll();
	}
}

function db_interface($actie="lijst", $query="") {
	global $selectnaam, $table_prefix;
	
	if ($actie == "add" and strlen($query) > 1) {
		if (strlen($table_prefix) > 1) {
			$query = str_replace($table_prefix, "", $query);
		}
		$msa_sql = str_replace("SYSDATE()", "#" . strftime("%m/%d/%Y %H:%M") . "#", str_replace("\"", "'", $query));
		$msa_sql = str_replace("CURDATE()", "#" . strftime("%m/%d/%Y") . "#", $msa_sql);
		$insquery = sprintf("INSERT INTO %sAdmin_interface SET `IP-adres`='%s', LidID=%d, `SQL-statement`=\"%s\", Ingevoerd=SYSDATE();", $table_prefix, $_SERVER['REMOTE_ADDR'], $_SESSION['lidid'], $msa_sql);
		fnQuery($insquery, 0);
		
	} elseif ($actie == "afmelden") {
		$query = sprintf("UPDATE %sAdmin_interface SET Afgemeld = SYSDATE() WHERE (Afgemeld < '2011-01-01' OR (Afgemeld IS NULL));", $table_prefix);
		$result = fnQuery($query);
		if ($result > 0) {
			fnQuery(sprintf("OPTIMIZE TABLE %sAdmin_interface;", $table_prefix));
			$mess = sprintf("Er zijn %d online-wijzigingen afgemeld.", $result);
			db_logboek("add", $mess, 2);
		}
		return $result;
		
	} elseif ($actie == "aantalopenstaand") {
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_interface WHERE (Afgemeld < '2010-01-01' OR (Afgemeld IS NULL));", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "lijst") {
		$query = sprintf("SELECT %1\$s AS NaamLid, I.Ingevoerd, `SQL-statement` AS `SQL`"
				 . " FROM %2\$sAdmin_interface AS I LEFT OUTER JOIN %2\$sLid AS L ON I.LidID = L.Nummer"
				 . " WHERE (I.Afgemeld < '2010-01-01' OR (Afgemeld IS NULL))"
				 . " ORDER BY I.Ingevoerd, I.RecordID;", $selectnaam, $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();
	}
}

function db_diploma($actie, $did=0) {
	global $table_prefix, $selfservicediplomas;
	
	if ($actie == "selfservice_lijst") {
		$query = sprintf("SELECT * FROM %sDiploma AS DP WHERE DP.Kode IN %s AND DP.AantalBeoordelingen < 2 ORDER BY DP.Volgnr, DP.Kode;", $table_prefix, $selfservicediplomas);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "Kode") {
		$query = sprintf("SELECT Kode FROM %sDiploma WHERE RecordID=%d;", $table_prefix, $did);
		return db_scalar($query);
	} elseif ($actie == "Naam") {
		$query = sprintf("SELECT Naam FROM %sDiploma WHERE RecordID=%d;", $table_prefix, $did);
		return db_scalar($query);
	} else {
		return null;
	}
}

function db_liddipl($actie, $lidid=0, $rid=0, $did=0, $exdat="", $vervdat="", $dipnr="", $xtrafilter="") {
	global $table_prefix, $fdaccess, $fdsql;
	
	$exdat = change_month_to_uk($exdat);
	$vervdat = change_month_to_uk($vervdat);
	
	if (strlen($exdat) >= 6 and strtotime($exdat) !== FALSE) {
		$dwexd = strftime($fdsql, strtotime($exdat));
		$dwaexd = sprintf("#%s#", strftime($fdaccess, strtotime($exdat)));
	} else {
		$dwexd = "";
	}
	
	if (strlen($vervdat) >= 6 and strtotime($vervdat) !== FALSE and strtotime($vervdat) > strtotime($dwexd)) {
		$dwverv = strftime($fdsql, strtotime($vervdat));
		$dwaverv = sprintf("#%s#", strftime($fdaccess, strtotime($vervdat)));
	} else {
		$dwverv = "NULL";
		$dwaverv = "NULL";
	}

	if ($actie == "add" and $lidid > 0 and $did > 0 and strlen($dwexd) == 10) {
		if ($dwverv != "NULL") {
			$dwverv = sprintf("'%s'", $dwverv);
		}
		$query = sprintf("INSERT INTO %sLiddipl (Lid, DiplomaID, EXDATUM, LaatsteBeoordeling, LicentieVervallenPer, Diplomanummer, Ingevoerd, Gewijzigd) VALUES (%d, %d, '%s', 1, %s, '%s', CURDATE(), SYSDATE());", $table_prefix, $lidid, $did, $dwexd, $dwverv, $dipnr);
		$result = fnQuery($query);
		if ($result > 0) {
			$query = sprintf("INSERT INTO Liddipl (Lid, DiplomaID, EXDATUM, LicentieVervallenPer, Diplomanummer, Ingevoerd, Gewijzigd) 
						VALUES (%d, %d, %s, %s, '%s', CURDATE(), SYSDATE());", $lidid, $did, $dwaexd, $dwaverv, $dipnr);
			db_interface("add", $query);
			$mess = sprintf("Diploma %s is toegevoegd bij %s", db_diploma("Kode", $did), db_naamlid($lidid));
			db_logboek("add", $mess, 6);
		}
	} elseif ($actie == "update" and $rid > 0) {
		$query = sprintf("SELECT * FROM %sLiddipl AS LD WHERE RecordID=%d;", $table_prefix, $rid);
		$result = fnQuery($query);
		$rowld = $result->fetch();
		if (strtotime($rowld->LicentieVervallenPer) != strtotime($dwverv) or $rowld->Diplomanummer != $dipnr) {
			if ($dwverv != "NULL") {
				$dwverv = sprintf("'%s'", $dwverv);
			}
			$query = sprintf("UPDATE %sLiddipl SET LicentieVervallenPer=%s, Diplomanummer='%s' WHERE RecordID=%d;", $table_prefix, $dwverv, $dipnr, $rid);
			$result = fnQuery($query);
			$query = sprintf("UPDATE Liddipl SET LicentieVervallenPer=%s, Diplomanummer='%s' WHERE Lid=%d AND DiplomaID=%d AND EXDATUM=%s;", $dwaverv, $dipnr, $rowld->Lid, $rowld->DiplomaID, $dwaexd);
			db_interface("add", $query);
			$mess = sprintf("Diploma %s is gewijzigd bij %s", db_diploma("Kode", $rowld->DiplomaID), db_naamlid($rowld->Lid));
			db_logboek("add", $mess, 6);
		}
	} elseif ($actie == "delete" and $lidid > 0 and $did > 0 and strlen($dwexd) == 10) {
		$query = sprintf("DELETE FROM %sLiddipl WHERE Lid=%d AND DiplomaID=%d AND EXDATUM='%s';", $table_prefix, $lidid, $did, $dwexd);
		$result = fnQuery($query);
		if ($result > 0) {
			$query = sprintf("DELETE FROM Liddipl WHERE Lid=%d AND DiplomaID=%d AND EXDATUM=%s;", $lidid, $did, $dwaexd);
			db_interface("add", $query);
			db_logboek("add", sprintf("Diploma %s is verwijderd bij %s.", db_diploma("Kode", $did), db_naamlid($lidid)), 6);
		}
	} elseif ($actie == "lidgegevens" and $lidid > 0) {
		if (strlen($xtrafilter) > 1) {
			$xtrafilter = "AND " . $xtrafilter;
		}
		$query = sprintf("SELECT D.Kode AS Code,"
				 . " D.Naam AS Diploma,"
				 . " LD.EXDATUM AS dteDatum,"
				 . " LD.Diplomanummer AS Gegevens,"
				 . " IFNULL(LD.LicentieVervallenPer, IF(D.GELDIGH>0, DATE_ADD(LD.EXDATUM, INTERVAL D.GELDIGH MONTH), NULL)) AS `dteVervalt per`"
				 . " FROM %1\$sLiddipl AS LD INNER JOIN %1\$sDiploma AS D ON LD.DiplomaID = D.RecordID"
				 . " WHERE LD.LaatsteBeoordeling=1 AND ((D.Vervallen IS NULL) OR D.Vervallen > CURDATE()) AND LD.Lid=%2\$d %3\$s"
				 . " ORDER BY D.Volgnr, D.Kode, LD.EXDATUM DESC;", $table_prefix, $lidid, $xtrafilter);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "diplomabehaald" and $lidid > 0 and $did > 0) {
		$query = sprintf("SELECT * FROM %sLiddipl WHERE Lid=%d AND DiplomaID=%d ORDER BY EXDATUM DESC;", $table_prefix, $lidid, $did);
		$result = fnQuery($query);
		return $result->fetch();		
	}
}

function db_evenement($actie, $lidid=0, $eventid=0, $opm="", $stat="", $functie="") {
	global $table_prefix, $selectnaam, $selectzoeknaam, $wherelidond;

	$xtra_where = "";
	if ($actie == "komende") {
		$query = sprintf("SELECT E.*, ET.Omschrijving AS TypeEvenement
								FROM %1\$sEvenement AS E INNER JOIN %1\$sEvenement_Type AS ET ON E.TypeEvenement = ET.RecordID
								WHERE E.Datum >= CURDATE()
								ORDER BY E.Datum LIMIT 5;", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "open") {
		if ($lidid > 0) {
			$xtra_where = sprintf("AND (E.BeperkTotGroep=0 OR E.BeperkTotGroep IN (SELECT OnderdeelID FROM %sLidond AS LO WHERE Lid=%d AND %s))", $table_prefix, $lidid, $wherelidond);
		}
		$query = sprintf("SELECT E.*, ET.Omschrijving AS TypeEvenement
								FROM %1\$sEvenement AS E INNER JOIN %1\$sEvenement_Type AS ET ON E.TypeEvenement = ET.RecordID
								WHERE E.Datum >= CURDATE() AND InschrijvingOpen=1 %2\$s
								ORDER BY E.Datum;", $table_prefix, $xtra_where);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "overzicht") {
		$query = sprintf("SELECT E.RecordID AS lnkNummer, E.Datum, E.Omschrijving, E.Email, E.Starttijd, E.Eindtijd, E.Verzameltijd,
								ET.Omschrijving AS `Type evenement`
								FROM %1\$sEvenement AS E INNER JOIN %1\$sEvenement_Type AS ET ON E.TypeEvenement = ET.RecordID
								WHERE E.Datum >= DATE_ADD(CURDATE(), INTERVAL -1 WEEK)
								ORDER BY E.Datum;", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "aantalopen") {
		$query = sprintf("SELECT COUNT(*) FROM %sEvenement AS E 
								WHERE E.Datum >= CURDATE() AND E.InschrijvingOpen=1;", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchColumn();
	} elseif ($actie == "inschrijving" and $lidid > 0 and $eventid > 0) {
		$query = sprintf("SELECT ED.RecordID, ED.Opmerking, ED.Status
								FROM %1\$sEvenement AS E INNER JOIN %1\$sEvenement_Deelnemer AS ED ON E.RecordID = ED.EvenementID
								WHERE ED.LidID=%2\$d AND ED.EvenementID=%3\$d;", $table_prefix, $lidid, $eventid);
		$result = fnQuery($query);
		return $result->fetch();
	} elseif ($actie == "record" and $eventid > 0) {
		$query = sprintf("SELECT E.*, %1\$s AS GewijzigdDoorNaam FROM %2\$sEvenement AS E LEFT OUTER JOIN %2\$sLid AS L ON E.GewijzigdDoor = L.Nummer
								WHERE E.RecordID=%3\$d;", $selectnaam, $table_prefix, $eventid);
		$result = fnQuery($query);
		return $result->fetch();
	} elseif ($actie == "types") {
		$query = sprintf("SELECT * FROM %sEvenement_Type ORDER BY Omschrijving;", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "deelnemers") {
		if (strlen($stat) > 0) {
			$xtra_where = sprintf("AND ED.Status='%s'", $stat);
		}
		$query = sprintf("SELECT ED.RecordID, %1\$s AS NaamDeelnemer, ED.Status, ED.Opmerking, ED.Functie, ED.LidID, L.Geslacht
								FROM %2\$sEvenement_Deelnemer AS ED INNER JOIN %2\$sLid AS L ON ED.LidID = L.Nummer
								WHERE ED.EvenementID=%3\$d %4\$s
								ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", $selectnaam, $table_prefix, $eventid, $xtra_where);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "potdeelnemers") {
		$where = "";
		$query = sprintf("SELECT BeperkTotGroep FROM %sEvenement WHERE RecordID=%d;", $table_prefix, $eventid);
		$groepid = db_scalar($query);
		if ($groepid > 0) {
			$where = sprintf("AND LO.OnderdeelID=%d", $groepid);
		}
		$query = sprintf("SELECT DISTINCT L.Nummer, %1\$s AS Naam
								FROM %2\$sLid AS L LEFT OUTER JOIN %2\$sLidond AS LO ON L.Nummer = LO.Lid
								WHERE (L.Nummer NOT IN (SELECT LidID FROM %2\$sEvenement_Deelnemer WHERE EvenementID=%3\$d)) %4\$s
								ORDER BY %1\$s;", $selectzoeknaam, $table_prefix, $eventid, $where);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "adddeelnemer" and $lidid > 0 and $eventid > 0) {
		$query = sprintf("INSERT INTO %sEvenement_Deelnemer SET LidID=%d, EvenementID=%d, Opmerking='%s', Status='%s', Ingevoerd=CURDATE(), GewijzigdDoor=%d;", $table_prefix, $lidid, $eventid, $opm, $stat, $_SESSION['lidid']);
		fnQuery($query);
		$mess = sprintf("%s is met status '%s' aan evenement %d toegevoegd.", db_naamlid($lidid), $stat, $eventid);
		db_logboek("add", $mess, 7);
	} elseif ($actie == "updatedeelnemer" and $lidid > 0 and $eventid > 0) {
		$query = sprintf("UPDATE %sEvenement_Deelnemer SET Opmerking='%s', Status='%s', Functie='%s', GewijzigdDoor=%d WHERE LidID=%d AND EvenementID=%d;", $table_prefix, $opm, $stat, $functie, $_SESSION['lidid'], $lidid, $eventid);
		fnQuery($query);
	} elseif ($actie == "deletedeelnemer" and $lidid > 0 and $eventid > 0) {
		$query = sprintf("DELETE FROM %sEvenement_Deelnemer WHERE LidID=%d AND EvenementID=%d;", $table_prefix, $lidid, $eventid);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Inschrijving bij evenement %d is bij %s verwijderd.", $eventid, db_naamlid($lidid));
			db_logboek("add", $mess, 7);
		}
	}
}

function db_dms() {
}

function db_delete_local_tables() {
	global $arrTables, $table_prefix;
	
	foreach ($arrTables as $tn) {
		if (startwith($tn, "Admin_") == false and startwith($tn, "Bewaking_") == false and startwith($tn, "DMS_") == false and startwith($tn, "Mailing") == false and startwith($tn, "Evenement") == false) {
			fnQuery(sprintf("DELETE FROM %s%s;", $table_prefix, $tn));
		}
	}
}

function db_onderhoud() {
	global $bewaartijdmailings, $bewaartijdlogins, $table_prefix, $arrTables, $db_name, $wherelid, $beperktotgroep, $wherelidond, $lididwebmasters;

	// Vaste aanpassingen aan de database na een upload, omdat MS-Access geen datum velden kent, alleen datum_tijd.
	$query = sprintf("SELECT DATA_TYPE FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%sBewaking' AND COLUMN_NAME='BEGIN_PER';", $db_name, $table_prefix);
	$result = fnQuery($query);
	if ($result->fetchColumn() != "date") {
		fnQuery(sprintf("ALTER TABLE %sBewaking CHANGE `BEGIN_PER` `BEGIN_PER` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sBewaking CHANGE `EINDE_PER` `EINDE_PER` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sBewseiz CHANGE Begindatum Begindatum DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sBewseiz CHANGE `Einde` `Einde` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sBewseiz CHANGE Geboren Geboren DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLid CHANGE `GEBDATUM` `GEBDATUM` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLid CHANGE `Overleden` `Overleden` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLidmaatschap CHANGE `LIDDATUM` `LIDDATUM` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLidmaatschap CHANGE `Opgezegd` `Opgezegd` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sOnderdl CHANGE `VervallenPer` `VervallenPer` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLidond CHANGE `Vanaf` `Vanaf` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLidond CHANGE `Opgezegd` `Opgezegd` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sDiploma CHANGE `Vervallen` `Vervallen` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLiddipl CHANGE `EXDATUM` `EXDATUM` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLiddipl CHANGE `LicentieVervallenPer` `LicentieVervallenPer` DATE DEFAULT NULL;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sMutatie CHANGE `Datum` `Datum` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLidRedNed CHANGE Geboortedatum Geboortedatum DATE;", $table_prefix));
		foreach ($arrTables as $tn) {
			if (startwith($tn, "Admin_") == false and startwith($tn, "Mailing") == false) {
				fnQuery(sprintf("ALTER TABLE %s%s CHANGE Ingevoerd Ingevoerd DATE;", $table_prefix, $tn));
			}
		}
	}
	// Einde code
	
	$query = sprintf("SELECT EXTRA FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%sLiddipl' AND COLUMN_NAME='RecordID';", $db_name, $table_prefix);
	$result = fnQuery($query);
	if ($result->fetchColumn() != "auto_increment") {
		$query = sprintf("DELETE FROM %sLiddipl WHERE RecordID = 0;", $table_prefix);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Er zijn %d records uit Liddipl verwijderd, omdat het RecordID nul was.", $result);
			db_logboek("add", $mess, 2);
		}
		fnQuery(sprintf("ALTER TABLE %sLiddipl ADD PRIMARY KEY (`RecordID`);", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLiddipl CHANGE RecordID RecordID INT( 11 ) NOT NULL AUTO_INCREMENT;", $table_prefix));
	}
	
	// Toevoegen veld 'cc_addr' aan de tabel Mailing_hist. Deze code kan na 5/3/2013 worden verwijderd.
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%sMailing_hist' AND COLUMN_NAME='cc_addr';", $db_name, $table_prefix);
	if (db_scalar($query) == 0) {
		fnQuery(sprintf("ALTER TABLE `%sMailing_hist` ADD `cc_addr` VARCHAR(50);", $table_prefix));
	}
	
	// Toevoegen veld 'Afgemeld' aan de tabel Bewaking_Inschrijving. Deze code kan na 1/3/2013 worden verwijderd.
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%sBewaking_Inschrijving' AND COLUMN_NAME='Afgemeld';", $db_name, $table_prefix);
	if (db_scalar($query) == 0) {
		fnQuery(sprintf("ALTER TABLE `%sBewaking_Inschrijving` ADD `Afgemeld` DATETIME;", $table_prefix));
	}	
	
	// Toevoegen veld 'LastActivity' aan de tabel Admin_login. Deze code kan na 7/3/2013 worden verwijderd.
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%sAdmin_login' AND COLUMN_NAME='LastActivity';", $db_name, $table_prefix);
	if (db_scalar($query) == 0) {
		fnQuery(sprintf("ALTER TABLE `%sAdmin_login` ADD `LastActivity` DATETIME;", $table_prefix));
	}
	
		
	// Toevoegen veld 'FouteLogin' aan de tabel Admin_login. Deze code kan na 16/3/2013 worden verwijderd.
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%sAdmin_login' AND COLUMN_NAME='FouteLogin';", $db_name, $table_prefix);
	if (db_scalar($query) == 0) {
		fnQuery(sprintf("ALTER TABLE `%sAdmin_login` ADD `FouteLogin` SMALLINT NOT NULL DEFAULT '0';", $table_prefix));
	}

	if ($bewaartijdlogins > 0) {
		$query = sprintf('DELETE FROM %1$sAdmin_login
					WHERE LastLogin < ADDDATE(CURDATE(), INTERVAL -%2$d MONTH)
					AND Ingevoerd < ADDDATE(CURDATE(), INTERVAL -%2$d MONTH);', $table_prefix, $bewaartijdlogins);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Er zijn %d logins verwijderd, omdat ze %d maanden niet meer gebruikt zijn.", $result, $bewaartijdlogins);
			db_logboek("add", $mess, 2);
			fnQuery(sprintf("OPTIMIZE TABLE %sAdmin_login;", $table_prefix));
		}
	}
	$where = "";
	if (is_array($beperktotgroep) and count($beperktotgroep) > 0) {
		$where .= sprintf("LidID NOT IN (SELECT Lid FROM %sLidond AS LO WHERE %s AND LO.OnderdeelID IN (%s))", $table_prefix, $wherelidond, implode(", ", $beperktotgroep));
	} elseif ($beperktotgroep > 0) {
		$where .= sprintf("LidID NOT IN (SELECT Lid FROM %sLidond AS LO WHERE %s AND LO.OnderdeelID=%d)", $table_prefix, $wherelidond, $beperktotgroep);
	}
	if (strlen($where) > 0) {
		$where .= sprintf(" AND (LidID NOT IN (%s))", implode(", ", $lididwebmasters));
		$query = sprintf("DELETE FROM %sAdmin_login WHERE LastLogin <= DATE_ADD(CURDATE(), INTERVAL -1 WEEK) AND %s;", $table_prefix, $where);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Er zijn %d logins verwijderd, omdat ze geen toegang meer hebben.", $result, $bewaartijdlogins);
			db_logboek("add", $mess, 2);
		}
	}

	$query = sprintf("DELETE FROM %sLid WHERE (NOT Verwijderd IS NULL);", $table_prefix);
	$result = fnQuery($query);
	if ($result > 0) {
		db_logboek("add", sprintf("%d verwijderde leden definitief verwijderd.", $result), 2);
		fnQuery(sprintf("OPTIMIZE TABLE %sLid;", $table_prefix));
	}
	$query = sprintf("UPDATE %sLid SET Telefoon = NULL WHERE `Telefoonnummer is geheim` = 1;", $table_prefix);
	$result = fnQuery($query);
	sprintf("UPDATE %sLid SET Mobiel = NULL WHERE `Mobiel is geheim` = 1;", $table_prefix);
	$result += fnQuery($query);
	if ($result > 0) {
		$mess = sprintf("Er zijn %d geheime telefoonnummers verwijderd.", $result);
		db_logboek("add", $mess, 2);
	}
	
	$query = sprintf("DELETE FROM %sOnderdl WHERE `Type`<>'A' AND Kader<>1 AND (NOT VervallenPer IS NULL) AND VervallenPer < CURDATE();", $table_prefix);
	fnQuery($query);
	$query = sprintf('DELETE FROM %1$sLidond WHERE OnderdeelID NOT IN (SELECT RecordID FROM %1$sOnderdl);', $table_prefix);
	$result = fnQuery($query);
	if ($result > 0) {
		$mess = sprintf("Er zijn %d onderdelen bij leden verwijderd, omdat deze onderdelen vervallen zijn.", $result);
		db_logboek("add", $mess , 2);
		printf("<p class='mededeling'>%s</p>\n", $mess);
		fnQuery(sprintf("OPTIMIZE TABLE %sOnderdl;", $table_prefix));
		fnQuery(sprintf("OPTIMIZE TABLE %sLidond;", $table_prefix));
	}
	
	if ($bewaartijdmailings > 0) {
		$query = sprintf("DELETE FROM %sMailing"
				 . " WHERE (NOT deleted_on IS NULL)"
				 . " AND deleted_on < ADDDATE(CURDATE(), INTERVAL -%d MONTH);", $table_prefix, $bewaartijdmailings);
		$result = fnQuery($query);
		if ($result > 0) {
			db_logboek("add", sprintf("Er zijn %d mailings uit de prullenbak verwijderd.", $result), 2);
			fnQuery(sprintf("OPTIMIZE TABLE %sMailing;", $table_prefix));
			$query = sprintf('DELETE FROM %1$sMailing_rcpt
						WHERE MailingID NOT IN (SELECT M.MailingID FROM %1$sMailing AS M);', $table_prefix);
			$result = fnQuery($query);
			if ($result > 0) {
				fnQuery(sprintf("OPTIMIZE TABLE %sMailing_rcpt;", $table_prefix));
			}
		}
	}
	
	$query = sprintf("DELETE FROM %sDiploma WHERE (NOT Vervallen IS NULL) AND Vervallen < CURDATE();", $table_prefix);
	fnQuery($query);
	$query = sprintf('DELETE FROM %1$sLiddipl WHERE DiplomaID NOT IN (SELECT RecordID FROM %1$sDiploma);', $table_prefix);
	$result = fnQuery($query);
	if ($result > 0) {
		db_logboek("add", sprintf("Er zijn %d diploma's bij leden verwijderd, omdat deze diploma's vervallen zijn.", $result), 2);
		fnQuery(sprintf("OPTIMIZE TABLE %sDiploma;", $table_prefix));
		fnQuery(sprintf("OPTIMIZE TABLE %sLiddipl;", $table_prefix));
	}
	
	$query = sprintf("DELETE FROM %sGBR WHERE Balans=1;", $table_prefix);
	fnQuery($query);
	$query = sprintf('DELETE FROM %1$sMutatie WHERE GBR NOT IN (SELECT Kode FROM %1$sGBR);', $table_prefix);
	$result = fnQuery($query);
	if ($result > 0) {
		fnQuery(sprintf("OPTIMIZE TABLE %sGBR;", $table_prefix));
		fnQuery(sprintf("OPTIMIZE TABLE %sMutatie;", $table_prefix));
		db_logboek("add", sprintf("Er zijn %d mutaties verwijderd, omdat er geen resultaatrekening aan gekoppeld was.", $result), 2);
	}
	
	db_logboek("opschonen");
}

function db_createtables() {
	global $arrTables, $table_prefix;

	$queries = sprintf("SET SQL_MODE='NO_AUTO_VALUE_ON_ZERO';

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_access` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Tabpage` varchar(50) NOT NULL,
  `Toegang` int(11) NOT NULL DEFAULT '-1',
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Tabpage` (`Tabpage`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_activiteit` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL,
  `DatumTijd` datetime NOT NULL,
  `Omschrijving` text NOT NULL,
  `TypeActiviteit` tinyint(4) DEFAULT '0',
  `IP_adres` char(15) NOT NULL,
  `USER_AGENT` varchar(110) DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_interface` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL,
  `IP-adres` char(15) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `SQL-statement` text CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `Ingevoerd` datetime DEFAULT NULL,
  `Afgemeld` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_login` (
  `LidID` int(11) NOT NULL,
  `Login` varchar(15) NOT NULL,
  `openid_identity` varchar(50) DEFAULT NULL,
  `Wachtwoord` varchar(12) NOT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime NOT NULL,
  `GewijzigdDoor` int(11) NOT NULL,
  `LastLogin` datetime NOT NULL,
  `LastActivity` datetime NOT NULL,
  `Ingelogd` tinyint(4) NOT NULL DEFAULT '0',
  `FouteLogin` smallint(6) NOT NULL DEFAULT '0',
  PRIMARY KEY (`LidID`),
  UNIQUE KEY `Login` (`Login`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sBewaking` (
  `RecordID` int(11) DEFAULT NULL,
  `Lid` int(11) NOT NULL DEFAULT '0',
  `Seizoen` varchar(5) DEFAULT NULL,
  `SeizoenID` int(11) NOT NULL DEFAULT '0',
  `Weeknr` tinyint(4) DEFAULT NULL,
  `BEGIN_PER` date NOT NULL DEFAULT '0000-00-00',
  `EINDE_PER` date DEFAULT NULL,
  `Post` varchar(7) DEFAULT NULL,
  `Status` varchar(1) DEFAULT NULL,
  `Functie` tinyint(4) DEFAULT NULL,
  `Opmerking` varchar(50) DEFAULT NULL,
  `Beoordeling` longtext,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`SeizoenID`,`Lid`,`BEGIN_PER`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sBewaking_Blok` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `SeizoenID` int(11) NOT NULL,
  `Kode` char(2) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL,
  `Begindatum` date NOT NULL,
  `Einddatum` date NOT NULL,
  `InschrijvingOpen` tinyint(4) NOT NULL DEFAULT '1',
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `SeizoenBegindatum` (`SeizoenID`,`Begindatum`),
  UNIQUE KEY `SeizoenKode` (`SeizoenID`,`Kode`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sBewaking_Inschrijving` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Lid` int(11) NOT NULL,
  `Nummer` int(11) NOT NULL DEFAULT '0',
  `BlokID` int(11) NOT NULL,
  `Keuze` tinyint(4) NOT NULL DEFAULT '1',
  `Opmerking` varchar(50) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL,
  `Definitief` datetime NOT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `Afgemeld` datetime NOT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sBewseiz` (
  `RecordID` int(11) NOT NULL DEFAULT '0',
  `Kode` varchar(5) DEFAULT NULL,
  `Begindatum` date DEFAULT NULL,
  `Einde` date DEFAULT NULL,
  `Lokatie` varchar(30) DEFAULT NULL,
  `Geboren` date DEFAULT NULL,
  `MIN_LFT` tinyint(4) DEFAULT NULL,
  `ST_LEN_BP` tinyint(4) DEFAULT NULL,
  `EersteDagWeek` tinyint(4) DEFAULT NULL,
  `CorrectieOpWeeknr` smallint(6) DEFAULT NULL,
  `Afgesloten` tinyint(4) DEFAULT NULL,
  `TOONERV` varchar(1) DEFAULT NULL,
  `Posten` varchar(30) DEFAULT NULL,
  `KeuzesBijInschrijving` tinyint(4) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sBoekjaar` (
  `RecordID` int(11) NOT NULL DEFAULT '0',
  `Kode` varchar(5) DEFAULT NULL,
  `Omschrijving` varchar(50) DEFAULT NULL,
  `Begindatum` datetime DEFAULT NULL,
  `Einde` datetime DEFAULT NULL,
  `Afgesloten` tinyint(4) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sDiploma` (
  `RecordID` int(11) NOT NULL DEFAULT '0',
  `Kode` varchar(6) DEFAULT NULL,
  `Naam` varchar(75) DEFAULT NULL,
  `Type` varchar(1) DEFAULT NULL,
  `ORGANIS` tinyint(4) DEFAULT NULL,
  `MIN_LFT` tinyint(4) DEFAULT NULL,
  `Volgnr` tinyint(4) DEFAULT NULL,
  `EXAM_OND` tinyint(4) DEFAULT NULL,
  `GELDIGH` smallint(6) DEFAULT NULL,
  `AantalBeoordelingen` tinyint(4) DEFAULT NULL,
  `Vervallen` date DEFAULT NULL,
  `HistorieOpschonen` smallint(6) DEFAULT NULL,
  `VoorgangerID` int(11) DEFAULT NULL,
  `Voorganger` varchar(5) DEFAULT NULL,
  `Alternatief` varchar(20) DEFAULT NULL,
  `Tonen in bewakingsadministratie` tinyint(4) DEFAULT NULL,
  `RolID` int(11) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sDMS_Document` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Bestandsnaam` varchar(30) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `FolderID` int(11) NOT NULL,
  `Beschrijving` varchar(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `Volgnummer` tinyint(4) unsigned NOT NULL DEFAULT '0',
  `EigenaarID` int(11) NOT NULL,
  `LockedBy` int(11) NOT NULL,
  `Locked` datetime NOT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime NOT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sDMS_Folder` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Foldernaam` varchar(30) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `Beschrijving` varchar(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `Volgnummer` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `BovenliggendeFolderID` int(11) NOT NULL DEFAULT '0',
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime NOT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sEvenement` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Datum` date NOT NULL,
  `Starttijd` varchar(5) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL,
  `Eindtijd` varchar(5) NOT NULL,
  `Verzameltijd` varchar(5) NOT NULL,
  `Omschrijving` varchar(75) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `Email` varchar(45) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `TypeEvenement` int(11) NOT NULL,
  `InschrijvingOpen` tinyint(4) NOT NULL DEFAULT '1',
  `BeperkTotGroep` int(11) NOT NULL DEFAULT '0',
  `Ingevoerd` date NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `GewijzigdDoor` int(11) NOT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sEvenement_Deelnemer` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL,
  `EvenementID` int(11) NOT NULL,
  `Functie` varchar(20) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL,
  `Opmerking` varchar(75) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL,
  `Status` char(1) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT 'I',
  `Ingevoerd` date NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `GewijzigdDoor` int(11) NOT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `LidEvenement` (`LidID`,`EvenementID`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sEvenement_Type` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Omschrijving` varchar(30) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL,
  `Soort` char(1) NOT NULL DEFAULT 'W',
  `Ingevoerd` date NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sFunctie` (
  `Nummer` tinyint(4) NOT NULL DEFAULT '0',
  `Omschrijv` varchar(35) DEFAULT NULL,
  `Oms_Vrouw` varchar(35) DEFAULT NULL,
  `Afkorting` varchar(10) DEFAULT NULL,
  `Sorteringsvolgorde` tinyint(4) DEFAULT NULL,
  `Afdelingsfunctie` tinyint(4) DEFAULT NULL,
  `Ledenadministratiefunctie` tinyint(4) DEFAULT NULL,
  `Bewakingsfunctie` tinyint(4) DEFAULT NULL,
  `Kader` tinyint(4) DEFAULT NULL,
  `Vervallen per` datetime DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sGBR` (
  `RecordID` int(11) DEFAULT NULL,
  `Kode` varchar(4) NOT NULL DEFAULT '',
  `OMSCHRIJV` varchar(40) DEFAULT NULL,
  `Balans` tinyint(4) DEFAULT NULL,
  `KSTNPLTS` tinyint(4) DEFAULT NULL,
  `Verdichting` varchar(4) DEFAULT NULL,
  `VerdichtingID` int(11) DEFAULT NULL,
  `Debiteur` tinyint(4) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sKostenplaats` (
  `RecordID` int(11) NOT NULL DEFAULT '0',
  `Kode` varchar(5) DEFAULT NULL,
  `Naam` varchar(35) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLid` (
  `Nummer` int(11) NOT NULL DEFAULT '0',
  `Roepnaam` varchar(17) DEFAULT NULL,
  `Tussenv` varchar(7) DEFAULT NULL,
  `Achternaam` varchar(30) DEFAULT NULL,
  `Meisjesnm` varchar(25) DEFAULT NULL,
  `Voorletter` varchar(10) DEFAULT NULL,
  `Geslacht` varchar(1) DEFAULT NULL,
  `GEBDATUM` date DEFAULT NULL,
  `Overleden` date DEFAULT NULL,
  `GEBPLAATS` varchar(22) DEFAULT NULL,
  `Adres` varchar(30) DEFAULT NULL,
  `Huisnr` int(11) DEFAULT NULL,
  `Toevoeging` varchar(5) DEFAULT NULL,
  `Postcode` varchar(7) DEFAULT NULL,
  `Woonplaats` varchar(22) DEFAULT NULL,
  `Buitenland` tinyint(4) DEFAULT NULL,
  `Telefoon` varchar(16) DEFAULT NULL,
  `Telefoonnummer is geheim` tinyint(4) DEFAULT NULL,
  `Mobiel` varchar(16) DEFAULT NULL,
  `Mobiel is geheim` tinyint(4) DEFAULT NULL,
  `EMAIL` varchar(45) DEFAULT NULL,
  `EmailVereniging` varchar(45) DEFAULT NULL,
  `Waarschuwen bij nood` varchar(255) DEFAULT NULL,
  `BANKGIRO` varchar(10) DEFAULT NULL,
  `Bankrekening` varchar(18) DEFAULT NULL,
  `Machtiging afgegeven` tinyint(4) DEFAULT NULL,
  `BezwaarMachtiging` datetime DEFAULT NULL,
  `Legitimatietype` varchar(1) DEFAULT NULL,
  `Legitimatienummer` varchar(15) DEFAULT NULL,
  `Wijknummer` smallint(6) DEFAULT NULL,
  `Memo` longtext,
  `RelnrRedNed` varchar(8) DEFAULT NULL,
  `Beroep` varchar(40) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  `Verwijderd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLidbew` (
  `Lid` int(11) NOT NULL DEFAULT '0',
  `Opmerking` longtext,
  `Dieet` varchar(50) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Lid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLiddipl` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Lid` int(11) DEFAULT NULL,
  `DiplomaID` int(11) DEFAULT NULL,
  `EXDATUM` date DEFAULT NULL,
  `EXPLAATS` varchar(23) DEFAULT NULL,
  `Beoordelaar` int(11) DEFAULT NULL,
  `LaatsteBeoordeling` tinyint(4) DEFAULT NULL,
  `Diplomanummer` varchar(15) DEFAULT NULL,
  `Examen` int(11) DEFAULT NULL,
  `LicentieVervallenPer` date DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLidmaatschap` (
  `RecordID` int(11) DEFAULT NULL,
  `Lid` int(11) NOT NULL DEFAULT '0',
  `LIDDATUM` date NOT NULL DEFAULT '0000-00-00',
  `Opgezegd` date DEFAULT NULL,
  `OpgezegdDoorVereniging` tinyint(4) DEFAULT NULL,
  `Lidnr` int(11) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Lid`,`LIDDATUM`),
  UNIQUE KEY `Lidnr` (`Lidnr`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLidond` (
  `RecordID` int(11) DEFAULT NULL,
  `Lid` int(11) NOT NULL DEFAULT '0',
  `OnderdeelID` int(11) NOT NULL DEFAULT '0',
  `Vanaf` date NOT NULL DEFAULT '0000-00-00',
  `Opgezegd` date DEFAULT NULL,
  `Functie` tinyint(4) NOT NULL DEFAULT '0',
  `EmailFunctie` varchar(45) DEFAULT NULL,
  `Groep` varchar(5) DEFAULT NULL,
  `Opmerk` varchar(30) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Lid`,`OnderdeelID`,`Vanaf`,`Functie`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLidRedNed` (
  `Lidnr` int(11) NOT NULL DEFAULT '0',
  `RelnrRedNed` varchar(8) DEFAULT NULL,
  `Roepnaam` varchar(35) DEFAULT NULL,
  `Tussenvoegsels` varchar(7) DEFAULT NULL,
  `Achternaam` varchar(50) DEFAULT NULL,
  `Voorletters` varchar(10) DEFAULT NULL,
  `Geslacht` varchar(1) DEFAULT NULL,
  `Geboortedatum` date DEFAULT NULL,
  `Postcode` varchar(7) DEFAULT NULL,
  `Huisnr` smallint(6) DEFAULT NULL,
  `Foutboodschap` varchar(75) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  PRIMARY KEY (`Lidnr`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMailing` (
  `MailingID` int(11) NOT NULL AUTO_INCREMENT,
  `from_name` varchar(50) DEFAULT NULL,
  `from_addr` varchar(50) DEFAULT NULL,
  `to_name` varchar(50) DEFAULT NULL,
  `cc_addr` varchar(50) DEFAULT NULL,
  `subject` varchar(75) DEFAULT NULL,
  `message` text,
  `template` tinyint(4) NOT NULL DEFAULT '0',
  `confidential` tinyint(4) NOT NULL DEFAULT '1',
  `new_on` datetime DEFAULT NULL,
  `AddedBy` int(11) NOT NULL,
  `changed_on` datetime DEFAULT NULL,
  `ChangedBy` int(11) NOT NULL,
  `sent_on` datetime DEFAULT NULL,
  `SentBy` int(11) NOT NULL,
  `sent_cnt` int(11) DEFAULT '0',
  `deleted_on` datetime DEFAULT NULL,
  `DeletedBy` int(11) NOT NULL,
  PRIMARY KEY (`MailingID`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMailing_hist` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL DEFAULT '0',
  `MailingID` int(11) NOT NULL,
  `from_name` varchar(50) NOT NULL,
  `from_addr` varchar(50) NOT NULL,
  `to_name` varchar(50) NOT NULL,
  `subject` varchar(75) NOT NULL,
  `to_addr` varchar(50) NOT NULL,
  `cc_addr` varchar(50) NOT NULL,
  `message` text NOT NULL,
  `send_by` int(11) NOT NULL,
  `send_on` datetime NOT NULL,
  PRIMARY KEY (`RecordID`),
  KEY `LidID` (`LidID`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMailing_rcpt` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `MailingID` int(11) NOT NULL DEFAULT '0',
  `LidID` int(11) NOT NULL DEFAULT '0',
  `to_address` varchar(50) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMemo` (
  `RecordID` int(11) DEFAULT NULL,
  `Lid` int(11) NOT NULL DEFAULT '0',
  `Soort` varchar(1) NOT NULL DEFAULT '',
  `Vertrouwelijk` tinyint(4) DEFAULT NULL,
  `Memo` longtext,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Lid`,`Soort`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMutatie` (
  `RecordID` int(11) DEFAULT NULL,
  `BoekjaarID` int(11) NOT NULL DEFAULT '0',
  `Jaar` varchar(5) DEFAULT NULL,
  `Periode` tinyint(4) NOT NULL DEFAULT '0',
  `Dagboek` varchar(4) NOT NULL DEFAULT '',
  `DagboekID` int(11) DEFAULT NULL,
  `GBR` varchar(4) DEFAULT NULL,
  `KSTNPLTS` varchar(5) DEFAULT NULL,
  `KostenplaatsID` int(11) DEFAULT NULL,
  `Stuknr` tinyint(4) NOT NULL DEFAULT '0',
  `Regelnr` smallint(6) NOT NULL DEFAULT '0',
  `Datum` date DEFAULT NULL,
  `Debet` decimal(8,2) DEFAULT NULL,
  `Credit` decimal(8,2) DEFAULT NULL,
  `OMSCHRIJV` varchar(45) DEFAULT NULL,
  `Rekening` int(11) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`BoekjaarID`,`Periode`,`Dagboek`,`Stuknr`,`Regelnr`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sOnderdl` (
  `RecordID` int(11) NOT NULL DEFAULT '0',
  `Kode` varchar(6) DEFAULT NULL,
  `Naam` varchar(50) DEFAULT NULL,
  `LIDCB` decimal(8,2) DEFAULT NULL,
  `JEUGDCB` decimal(8,2) DEFAULT NULL,
  `FUNCTCB` decimal(8,2) DEFAULT NULL,
  `Type` varchar(1) DEFAULT NULL,
  `ORGANIS` tinyint(4) DEFAULT NULL,
  `Kader` tinyint(4) DEFAULT NULL,
  `Alleen leden` tinyint(4) DEFAULT NULL,
  `Tonen in bewakingsadministratie` tinyint(4) DEFAULT NULL,
  `CentraalEmail` varchar(45) DEFAULT NULL,
  `VervallenPer` date DEFAULT NULL,
  `HistorieOpschonen` smallint(6) DEFAULT NULL,
  `GekoppeldAanQuery` int(11) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sRekening` (
  `Nummer` int(11) NOT NULL DEFAULT '0',
  `Seizoen` smallint(6) DEFAULT NULL,
  `Datum` datetime DEFAULT NULL,
  `OMSCHRIJV` varchar(30) DEFAULT NULL,
  `Lid` int(11) DEFAULT NULL,
  `DEBNAAM` varchar(60) DEFAULT NULL,
  `Ouders` tinyint(4) DEFAULT NULL,
  `BET_TERM` tinyint(4) DEFAULT NULL,
  `BETAALDAG` smallint(6) DEFAULT NULL,
  `Bedrag` decimal(8,2) DEFAULT NULL,
  `Betaald` decimal(8,2) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;", $table_prefix);

	fnQuery($queries);
}

function db_backup() {
	global $db_host, $db_user, $db_pass, $db_name, $db_backupsopschonen, $db_backuptarren, $db_folderbackup;
	
	$buname = $db_name . "_backup_" . Date("Y-m-d-H");
	if (!isset($db_folderbackup) or strlen($db_folderbackup) < 5) {
		$db_folderbackup = str_replace($_SERVER['PHP_SELF'], "", $_SERVER["SCRIPT_FILENAME"]) . "/backups/";
		if (!file_exists($db_folderbackup)) {
			if (mkdir($db_folderbackup, 0755, true) == true) {
				$ret = sprintf("Directory '%s' is aangemaakt.", $db_folderbackup);
				db_logboek("add", $ret, 3);
			}
		}
	} elseif (substr($db_folderbackup, -1) != "/") {
		$db_folderbackup .= "/";
	}

	$FileName = $db_folderbackup . $buname;
	$strCmd = sprintf("mysqldump -h%s -u%s -p%s -f %s> %s.sql", $db_host, $db_user, $db_pass, $db_name, $FileName);
	exec($strCmd, $output, $output);
	if (filesize($FileName . ".sql") > 1500) {
		$ret = sprintf("De backup is in '%s.sql' gemaakt. ", str_replace($db_folderbackup, "", $FileName));
		db_logboek("add", $ret, 3);

		if (isset($db_backuptarren) and $db_backuptarren == 1) {
			// De dump tarren
			exec("/bin/tar -czf " . $FileName . ".tar.gz" . " " . $FileName . ".sql");

			// De niet ingepakte dump weggooien
			exec("/bin/rm " . $FileName . ".sql");

			// De backup 700 chmodden, zodat niemand er bij kan!
			exec("/bin/chmod 700 " . $FileName . ".tar.gz");
		} else {
			// De backup 700 chmodden, zodat niemand er bij kan!
			exec("/bin/chmod 700 " . $FileName . ".sql");
		}

		if ($db_backupsopschonen > 0 and $handle = opendir($db_folderbackup)) {
			while (false !== ($file = readdir($handle))) {
				if ($file != "." and $file != "..") {
					if (filectime($db_folderbackup . "/" . $file) < mktime(0, 0, 0, date("m"), date("d")-$db_backupsopschonen, date("Y")) or filesize($db_folderbackup . "/" . $file) < 500 ) {
						unlink($db_folderbackup . "/" . $file);
						$mess = sprintf("Bestand %s is verwijderd. ", $db_folderbackup . $file);
						db_logboek("add", $mess, 3);
						$ret .= "<br>\n" . $mess;
					}
				}
			}
		}
	} else {
	   $ret = "Het backup-script heeft gedraaid, maar de backup is niet gelukt.\n";
	   $ret .= sprintf("Melding mysqldump: %s", $output);
		db_logboek("add", $ret, 3);
	}
	return $ret;
}




?>
