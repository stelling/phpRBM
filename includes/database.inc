<?php

$arrTables[] = "Admin_access";
$arrTables[] = "Admin_activiteit";
$arrTables[] = "Admin_interface";
$arrTables[] = "Admin_login";
$arrTables[] = "Bewaking_Blok";
$arrTables[] = "Bewaking_Inschrijving";
$arrTables[] = "Mailing";
$arrTables[] = "Mailing_hist";
$arrTables[] = "Mailing_rcpt";
// $arrTables[] = "DMS_Document";
// $arrTables[] = "DMS_Folder";

// Overgenomen uit de Access-database
$arrTables[20] = "Bewaking";
$arrTables[] = "Bewseiz";
$arrTables[] = "Functie";
$arrTables[] = "Lid";
$arrTables[] = "Lidmaatschap";
$arrTables[] = "Lidond";
$arrTables[] = "Onderdl";
$arrTables[] = "Liddipl";
$arrTables[] = "Diploma";
$arrTables[] = "LidRedNed";
$arrTables[] = "Memo";
$arrTables[] = "Boekjaar";
$arrTables[] = "Mutatie";
$arrTables[] = "GBR";
$arrTables[] = "Kostenplaats";
$arrTables[] = "Rekening";
$arrTables[] = "Rekreg";

$TypeActiviteit[0] = "N.T.B.";
$TypeActiviteit[1] = "Inloggen/Uitloggen";
$TypeActiviteit[2] = "Onderhoud/beheer";
$TypeActiviteit[3] = "DB backup";
$TypeActiviteit[4] = "Mailing";
$TypeActiviteit[5] = "Authenticatie";
$TypeActiviteit[6] = "Mutaties lidgegevens";
$TypeActiviteit[7] = "Evenementen";
$TypeActiviteit[8] = "Interface";
$TypeActiviteit[9] = "Upload data";
$TypeActiviteit[10] = "Webshop";
$TypeActiviteit[11] = "Bewaking";
$TypeActiviteit[12] = "Mutaties diploma's";
$TypeActiviteit[13] = "Parameters";
$TypeActiviteit[14] = "Rekeningen";
$TypeActiviteit[15] = "Autorisatie";
$TypeActiviteit[97] = "Foutieve login";
$TypeActiviteit[98] = "Performance";
$TypeActiviteit[99] = "Debugging";
asort($TypeActiviteit);

$fdlang = "'%e %M %Y'";
$fdtlang = "'%e %M %Y (%H:%i)'";
$fdkort = "'%e-%c-%Y'";
$fdaccess = "%m/%d/%Y";
$fdsql = "%Y-%m-%d";

if (strlen($table_prefix) > 0 and substr($table_prefix, -1) != "_") {
	$table_prefix .= "_";
}

$selectnaam = "CONCAT(IF(LENGTH(L.Roepnaam)>1, L.Roepnaam, L.Voorletter), ' ', IF(L.Tussenv>'', CONCAT(L.Tussenv, ' '), ''), L.Achternaam)";
$selectzoeknaam = "TRIM(CONCAT(L.Achternaam, ', ', IF(L.Tussenv>'', CONCAT(L.Tussenv, ' '), ''), IF(LENGTH(L.Roepnaam)>1, L.Roepnaam, L.Voorletter), ' '))";
$selectlidnr = sprintf("SELECT MAX(Lidnr) FROM %sLidmaatschap AS LM WHERE LM.Lid=L.Nummer AND LM.LIDDATUM <= CURDATE() AND ((LM.Opgezegd IS NULL) OR LM.Opgezegd >= CURDATE())", $table_prefix);
$selectmailbox = "CASE WHEN (NOT M.deleted_on IS NULL) THEN 'Prullenbak'
							  WHEN (M.Template=1) THEN 'Templates'
							  WHEN (M.sent_on IS NULL) THEN 'Concepten'
							  ELSE 'Verzonden' END";  // Moet nog verder ingevoerd worden, indien wenselijk
$fromlid = sprintf('%1$sLid AS L LEFT OUTER JOIN %1$sLidmaatschap AS LM ON L.Nummer = LM.Lid', $table_prefix);
$wherelid = "LM.LIDDATUM <= CURDATE() AND ((LM.Opgezegd IS NULL) OR LM.Opgezegd >= CURDATE())";
$fromlidond = sprintf('%1$sLid AS L INNER JOIN ((%1$sLidond AS LO INNER JOIN %1$sFunctie AS F ON LO.Functie = F.Nummer) INNER JOIN %1$sOnderdl AS O ON LO.OnderdeelID = O.RecordID) ON L.Nummer = LO.Lid', $table_prefix);
$wherelidond = "LO.Vanaf <= CURDATE() AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd >= CURDATE())";
$whereliddipl = "((LD.LicentieVervallenPer IS NULL) OR LD.LicentieVervallenPer >= CURDATE())";
$sqllidgroepen = sprintf("SELECT OnderdeelID FROM %sLidond AS LO WHERE %s AND Lid=%d", $table_prefix, $wherelidond, $_SESSION['lidid']);

$db_conn_pdo = sprintf("mysql:host=%s;dbname=%s;charset=utf8", $db_host, $db_name);
try {
	$dbc = new PDO($db_conn_pdo, $db_user, $db_pass);
	$dbc->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_OBJ);
	$dbc->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {
	printf("<p>Verbinding naar database '%s' mislukt: %s</p>\n", $db_name, $e->getMessage());
}

if (db_param("meisjesnaamtonen") == 1) {
	$selectnaam = "CONCAT(IF(LENGTH(L.Roepnaam)>1, L.Roepnaam, L.Voorletter), ' ', IF(L.Tussenv>'', CONCAT(L.Tussenv, ' '), ''), L.Achternaam, IF(L.Meisjesnm>'', CONCAT('-', L.Meisjesnm), ''))";	
}

if (!isset($_SESSION['aantallid']) and TableExists("Lid") == false) {
	db_createtables();
	db_onderhoud();
	$_SESSION['aantallid'] = 0;
} else if (!isset($_SESSION['aantallid']) or $_SESSION['aantallid'] < 25) {
	$query = sprintf("SELECT COUNT(*) FROM %sLid;", $table_prefix);
	$_SESSION['aantallid'] = db_scalar($query);
}

if (!isset($_SESSION['aantallogin']) or $_SESSION['aantallogin'] < 5) {
	$_SESSION['aantallogin'] = db_logins("aantal");
}

if (!isset($_SESSION['aantalafdelingen'])) {
	$query = sprintf("SELECT COUNT(*) FROM %sOnderdl WHERE `Type`='A';", $table_prefix);
	$_SESSION['aantalafdelingen'] = db_scalar($query);
}

if (!isset($_SESSION['aantalrollen'])) {
	$query = sprintf("SELECT COUNT(*) FROM %sOnderdl WHERE `Type`='R';", $table_prefix);
	$_SESSION['aantalrollen'] = db_scalar($query);
}

if (!isset($_SESSION['aantaltoestemmingen'])) {
	$query = sprintf("SELECT COUNT(*) FROM %sOnderdl WHERE `Type`='T';", $table_prefix);
	$_SESSION['aantaltoestemmingen'] = db_scalar($query);
}

if (!isset($_SESSION['aantalbewaking'])) {
	$query = sprintf("SELECT COUNT(*) FROM %sBewseiz;", $table_prefix);
	$_SESSION['aantalbewaking'] = db_scalar($query);
}

if (!isset($_SESSION['aantalrekeningen']) or $_SESSION['aantalrekeningen'] == 0) {
	$query = sprintf("SELECT COUNT(*) FROM %sRekening;", $table_prefix);
	$_SESSION['aantalrekeningen'] = db_scalar($query);
	if ($_SESSION['aantalrekeningen'] > 0) {
		$query = sprintf("SELECT COUNT(*) FROM %sRekreg;", $table_prefix);
		$_SESSION['aantalrekreg'] = db_scalar($query);
	} else {
		$_SESSION['aantalrekreg'] = 0;
	}
}

if (!isset($_SESSION['aantalmutaties'])) {
	$query = sprintf("SELECT COUNT(*) FROM %sMutatie;", $table_prefix);
	$_SESSION['aantalmutaties'] = db_scalar($query);
}

if (!isset($_SESSION['aantalkostenplaatsen'])) {
	$query = sprintf("SELECT COUNT(DISTINCT KostenplaatsID) FROM %sMutatie WHERE KostenplaatsID > 0;", $table_prefix);
	$_SESSION['aantalkostenplaatsen'] = db_scalar($query);
}

function fnQuery($query) {
	global $dbc;
	
	$starttijd = microtime(true);
	
	try {
		if (strtoupper(substr($query, 0, 6)) == "SELECT") {
// debug($query);
			$rs = $dbc->query($query);
			$exec_tijd = microtime(true) - $starttijd;
			$p = "performance_trage_select";
			if (isset($_SESSION['settings'][$p])) {
				if ($_SESSION['settings'][$p] > 0 and $exec_tijd >= $_SESSION['settings'][$p]) {
					$mess = sprintf("%f seconden query: %s", $exec_tijd, $query);
					db_logboek("add", $mess, 98);
				} else {
//					debug($exec_tijd . ": ". $query, 1, 0, 0);
				}
			} else {
				$_SESSION['settings'][$p] = 0;
				$_SESSION['settings'][$p] = db_param($p);
			}
			return $rs;
		} elseif (strtoupper(substr($query, 0, 6)) == "INSERT") {
			$rs = $dbc->query($query);
			return $dbc->lastInsertID();
		} else {
			$result = $dbc->prepare($query);
			$result->execute();
			if (strtoupper(substr($query, 0, 6)) == "INSERT") {
				$rv = $dbc->lastInsertId();
			} else {
				$rv = $result->RowCount();
			}
			$result = null;
			return $rv;
		}
	} catch (Exception $e) {					
		$mess = sprintf("Error in SQL '%s': %s", $query, $e->getMessage());
		db_logboek("add", $mess, 99, 0, 1);
		return false;
	}
}

function db_scalar($query) {
	$result = fnQuery($query);
	return $result->fetchColumn();
}

function TableExists($table) {
	global $db_name, $table_prefix;
	
	if (strlen($table_prefix) > 1 and startwith($table, $table_prefix) == FALSE) {
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

function db_gegevenslid($LidID, $soort="Alg") {
	global $selectnaam, $table_prefix, $fromlid, $fromlidond, $wherelidond, $dlnstatus, $fdlang;

	if ($soort == "Alg") {
		$sqond = sprintf("SELECT MIN(O.Naam) FROM %1\$sOnderdl AS O INNER JOIN %1\$sLidond AS LO ON O.RecordID = LO.OnderdeelID
								WHERE O.`Type` = 'O' AND LO.Lid = L.Nummer AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd >= CURDATE())", $table_prefix);
		$VOG = "`VOG afgegeven` AS 'dteVOG afgegeven op',";
		$sqllogin = sprintf("SELECT MAX(Login) FROM %sAdmin_login AS AL WHERE AL.LidID = L.Nummer", $table_prefix);
		$sqlbd = sprintf("SELECT MAX(EINDE_PER) FROM %sBewaking WHERE Lid=L.Nummer AND BEGIN_PER < CURDATE()", $table_prefix);
		$query = sprintf("SELECT CONCAT(`Waarschuwen bij nood`, NamenOuders) FROM %sLid WHERE Nummer=%d;", $table_prefix, $LidID);
		if (strlen(db_scalar($query)) > 3) {
			$wbn = " 'Waarschuwen bij nood' as Kop7";
			$wbn .= ", `NamenOuders` AS `Namen ouders`";
			$wbn .= ", `Waarschuwen bij nood` AS `Contactgegevens`,";
		} else {
			$wbn = "";
		}
		$sqdieet = sprintf("SELECT `Memo` FROM %sMemo WHERE Lid=%d AND Soort='D'", $table_prefix, $LidID);
		if (strlen(db_scalar($sqdieet)) > 1) {
			$sqdieet = sprintf(" 'Dieet' as Kop7, (%s) AS memDieet,", $sqdieet);
		} else {
			$sqdieet = "";
		}
		$query = sprintf("SELECT 'Persoonsgegevens' AS Kop1, 
					%s AS Naam, 
					L.Voorletter AS Voorletters, 
					L.Roepnaam AS ndRoepnaam,
					CASE L.Geslacht WHEN 'V' THEN 'Vrouwelijk' WHEN 'M' THEN 'Mannelijk' WHEN 'B' THEN 'Bedrijf' ELSE 'Onbekend' END AS Geslacht, 
					IF(NOT ISNULL(L.GEBDATUM), CONCAT(DATE_FORMAT(L.GEBDATUM, %s), IF(LENGTH(L.GEBPLAATS) > 1, CONCAT(' te ', L.GEBPLAATS), '')), '') AS Geboren, 
					L.Overleden AS `dteOverleden op`,
					IF(ISNULL(L.GEBDATUM) OR (NOT ISNULL(L.Overleden)), NULL, CONCAT(TIMESTAMPDIFF(YEAR, L.GEBDATUM, CURDATE()), ' jaar')) AS Leeftijd,
					L.Burgerservicenummer,
					'Adresgegevens' AS Kop2,
					L.Adres,
					IF(LENGTH(L.Woonplaats) > 1, CONCAT(L.Postcode, '  ', L.Woonplaats), '') AS `Postcode en woonplaats`, 
					'Contactgegevens' AS Kop3, 
					L.Telefoon, 
					L.Mobiel, 
					L.Email AS `emlE-mail`,
					L.EmailVereniging AS `emlE-mail vereniging`,
					L.EmailOuders AS `emlE-mail ouders`,
					'Lidmaatschap' AS Kop4,
					LM.Lidnr AS Lidnummer, 
					(%s) AS Onderscheiding, 
					LM.LIDDATUM AS `dteLid vanaf`, 
					LM.Opgezegd AS `dteLid totenmet`, 
					IF(DATEDIFF(CURDATE(), LM.LIDDATUM) < 500, NULL, CONCAT(ROUND(TIMESTAMPDIFF(MONTH, LM.LIDDATUM, IF(ISNULL(LM.Opgezegd), CURDATE(), LM.Opgezegd))/12, 1), ' jaar')) AS `Lengte lidmaatschap`,
					'Financieel' AS Kop5,
					L.`Machtiging afgegeven` AS `booMachtiging incasso`,
					L.Bankrekening,
					'Overig' AS Kop6, 
					L.RelnrRedNed AS `Sportlink ID`,
					L.LoginWebsite AS `Login website`,
					%s
					L.Beroep AS `Beroep`,
					(%s) AS `dteLaatste bewakingsdag`, 
					%s 
					%s
					'Recordinformatie' as Kop8, 
					L.Nummer AS `RecordID`, 
					L.Ingevoerd AS `dteIngevoerd op`, 
					L.Gewijzigd AS `dteLaatst gewijzigd op` 
					FROM %s 
					WHERE L.Nummer=%d 
					ORDER BY LM.LIDDATUM DESC;", $selectnaam, $fdlang, $sqond, $VOG, $sqlbd, $wbn, $sqdieet, $fromlid, $LidID);
					
	} elseif ($soort == "Financieel") {
		$query = sprintf("SELECT L.Bankrekening AS Bankrekeningnummer, 
								L.`Machtiging afgegeven` AS `booMachtiging automatische incasso`, 
								IF(ISNULL(L.BezwaarMachtiging), 'Nee', DATE_FORMAT(L.BezwaarMachtiging, %s)) AS `Bezwaar machtiging incasso`
								FROM %sLid AS L
								WHERE L.Nummer=%d;", $fdlang, $table_prefix, $LidID);
	} elseif ($soort == "Afdelingen") {
		$query = sprintf("SELECT O.Kode AS Code,
					O.Naam,
					LO.Vanaf AS dteVanaf,
					F.Omschrijv AS Functie,
					LO.Opmerk AS Opmerking,
					LO.Opgezegd AS dteOpgezegd,
					CONCAT(FORMAT(DATEDIFF(IF(LO.Opgezegd IS NULL, CURDATE(), LO.Opgezegd), LO.Vanaf)/365.25, 1, 'nl_NL'), ' jaar') AS numDuur
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
					CONCAT(FORMAT(DATEDIFF(IF(LO.Opgezegd IS NULL, CURDATE(), LO.Opgezegd), LO.Vanaf)/365.25, 1, 'nl_NL'), ' jaar') AS numDuur
					FROM %s
					WHERE O.Kader=1 AND LO.Lid=%d
					ORDER BY IF(ISNULL(LO.Opgezegd), 0, 1), LO.Vanaf DESC, O.Naam;", $fromlidond, $LidID);
	} elseif ($soort == "Rollen") {
		$query = sprintf("SELECT O.Kode AS Code, 
					O.Naam AS Omschrijving, 
					LO.Vanaf AS dteVanaf,
					LO.Opgezegd AS dteTotenmet
					FROM %s
					WHERE O.`Type`='R' AND LO.Lid=%d
					ORDER BY IF(ISNULL(LO.Opgezegd), 0, 1), LO.Vanaf, O.Kode;", $fromlidond, $LidID);
					
	} elseif ($soort == "Groepen") {
		$query = sprintf("SELECT O.Kode AS Code,
					O.Naam AS Omschrijving,
					LO.Vanaf AS dteVanaf,
					LO.Opmerk AS Opmerking,
					LO.Opgezegd AS dteTotenmet
					FROM %s
					WHERE O.`Type`='G' AND O.Kader=0 AND O.GekoppeldAanQuery=0 AND LO.Lid=%d
					ORDER BY IF(ISNULL(LO.Opgezegd), 0, 1), LO.Vanaf, O.Kode;", $fromlidond, $LidID);
					
	} elseif ($soort == "Toestemmingen") {
		$query = sprintf("SELECT O.Naam AS Omschrijving, 
					LO.Vanaf AS dteGegeven,
					IF((LO.Opgezegd IS NULL) AND O.MaximaleLengtePeriode > 0, DATE_ADD(LO.Vanaf, INTERVAL O.MaximaleLengtePeriode MONTH), LO.Opgezegd) AS `dteGeldig tot en met`
					FROM %s
					WHERE O.`Type`='T' AND LO.Lid=%d
					ORDER BY IF(ISNULL(LO.Opgezegd), 0, 1), LO.Vanaf, O.Kode;", $fromlidond, $LidID);
	} elseif ($soort == "Lidbew") {
		$tw = sprintf("CONVERT((SELECT SUM(DATEDIFF(EINDE_PER, BEGIN_PER)+1) FROM %sBewaking AS BW WHERE BW.Lid = %d AND BEGIN_PER < SYSDATE())/7, SIGNED)", $table_prefix, $LidID);
		$ts = sprintf("(SELECT COUNT(DISTINCT SeizoenID) FROM %sBewaking AS BW WHERE BW.Lid = %d AND BEGIN_PER < CURDATE())", $table_prefix, $LidID);
		$query = sprintf("SELECT CONCAT(%1\$s, ' weken in ', %2\$s, ' seizoen(en)') AS `Totale bewaking`,
								(SELECT Memo FROM %3\$sMemo WHERE Soort='B' AND Lid=%4\$d) AS Opmerking,
								(SELECT Memo FROM %3\$sMemo WHERE Soort='D' AND Lid=%4\$d) AS Dieet;", $tw, $ts, $table_prefix, $LidID);
	} elseif ($soort == "Bew") {
		$xtra_where = "";
		if (db_param("tonentoekomstigebewakingen") == 0) {
			$xtra_where = "AND BS.Einde <= CURDATE()";
		}
		$query = sprintf("SELECT BS.Kode AS Seizoen,
								Weeknr AS intWeek,
								BEGIN_PER AS dteBegin,
								EINDE_PER AS dteEinde,
								DATEDIFF(EINDE_PER, BEGIN_PER) + 1 AS intDagen,
								Post,
								F.Omschrijv AS Functie
								FROM %1\$sBewseiz AS BS INNER JOIN (%1\$sBewaking AS BW INNER JOIN %1\$sFunctie AS F ON BW.Functie = F.Nummer) ON BS.RecordID = BW.SeizoenID
								WHERE Lid=%2\$d %3\$s
								ORDER BY BS.Begindatum DESC, BS.Lokatie, BW.BEGIN_PER DESC;", $table_prefix, $LidID, $xtra_where);
	} elseif ($soort == "Rekening") {
		$query = sprintf("SELECT Nummer,
					Seizoen,
					Datum AS dteDatum,
					OMSCHRIJV AS Omschrijving,
					Bedrag AS curBedrag,
					Betaald AS curBetaald,
					(Bedrag-Betaald) AS curOpenstaand,
					Nummer AS lnkNummer
					FROM %sRekening
					WHERE Lid=%d
					ORDER BY Nummer DESC;", $table_prefix, $LidID);
	} elseif ($soort == "Mailing") {
		$query = db_mailing_hist("sqllijst", $LidID);
	} elseif ($soort == "Evenementen") {
		$query = "SELECT E.Datum AS dteDatum, E.Omschrijving, E.Verzameltijd, DATE_FORMAT(E.Datum, '%H:%i') AS Starttijd,
					E.Eindtijd, E.Email AS emlContact, ED.Functie ";
		if (isset($dlnstatus)) {
			$query .= ", CASE ED.Status";
			foreach ($dlnstatus as $s => $o) {
				$query .= sprintf(" WHEN '%s' THEN '%s'", $s, $o);
			}
			$query .= " END AS Status ";
		}
		$query .= sprintf("FROM %1\$sEvenement AS E INNER JOIN %1\$sEvenement_Deelnemer AS ED ON E.RecordID = ED.EvenementID
								WHERE ED.Status IN ('B', 'I', 'R') AND ED.LidID=%2\$d
								ORDER BY E.Datum DESC;", $table_prefix, $LidID);
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

function db_naamlid($lidid, $uitv="", $riz="Onbekend") {
	global $selectnaam, $selectzoeknaam, $table_prefix;
	
	if (strlen($uitv) == 0) {
		$uitv = "Naam";
	}
	
	if ($uitv == "Naam") {
		$query = sprintf("SELECT %s FROM %sLid AS L WHERE L.Nummer=%d;", $selectnaam, $table_prefix, $lidid);
	} elseif ($uitv == "Roepnaam") {
		$query = sprintf("SELECT IF(LENGTH(L.Roepnaam)=0, L.Voorletter, L.Roepnaam) FROM %sLid AS L WHERE L.Nummer=%d;", $table_prefix, $lidid);
	} else {
		$query = sprintf("SELECT %s FROM %sLid AS L WHERE L.Nummer=%d;", $selectzoeknaam, $table_prefix, $lidid);
	}
	$rv = db_scalar($query);
	if (strlen($rv) == 0) {
		$rv = $riz;
	}
	return $rv;
}

function db_lid($actie, $filter="", $lidid=0, $xtraselect="") {
	global $selectnaam, $selectzoeknaam, $selectlidnr, $table_prefix, $fromlid, $wherelid;
	
	if (strlen($filter) > 0 and ($actie != "verjaardagen" and $actie != "lididbijemail")) {
		$filter = " WHERE " . $filter;
	} elseif ($lidid > 0) {
		$filter = sprintf(" WHERE L.Nummer=%d", $lidid);
	}
	
	$scalar = 0;
	if ($actie == "record" and $lidid > 0) {
		$query = sprintf("SELECT *, %s as NaamLid FROM %sLid AS L WHERE Nummer=%d;", $selectnaam, $table_prefix, $lidid);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "lijst") {
		$query = sprintf("SELECT L.Nummer, %1\$s AS `Naam_lid`, UPPER(L.Woonplaats) AS Woonplaats,
					L.Telefoon, L.Mobiel, IF((L.Email IS NULL), L.EmailOuders, L.Email) AS `emlE-mail`, L.GEBDATUM AS GEBDATUM, %2\$s AS Zoeknaam
					FROM %3\$sLid AS L
					%4\$s
					ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, L.Nummer DESC;", $selectnaam, $selectzoeknaam, $table_prefix, $filter);
					
	} elseif ($actie == "ledenlijst") {
		$query = sprintf("SELECT L.Nummer AS lnkNummer, LM.Lidnr AS numLidnr, %1\$s AS `Naam_lid`, UPPER(L.Woonplaats) AS Woonplaats,
					L.Telefoon, L.Mobiel, IF((L.Email IS NULL), L.EmailOuders, L.Email) AS `emlE-mail`%3\$s,
					L.Nummer AS llkNummer, L.GEBDATUM AS ndGEBDATUM, %2\$s AS ndZoeknaam
					FROM %4\$s
					%5\$s
					ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, LM.LIDDATUM DESC;", $selectnaam, $selectzoeknaam, $xtraselect, $fromlid, $filter);
					
	} elseif ($actie == "wijzigingen") {
		$query = sprintf("SELECT L.Roepnaam,
					L.VOORLETTER,
					L.Tussenv,
					L.Achternaam,
					L.Meisjesnm,
					L.Geslacht,
					L.Adres,
					L.Postcode,
					L.Woonplaats,
					L.Telefoon,
					L.Mobiel,
					L.Email,
					L.EmailVereniging,
					L.EmailOuders,
					L.GEBDATUM,
					L.GEBPLAATS,
					L.Bankrekening,
					L.`Machtiging afgegeven`,
					L.BezwaarMachtiging,
					L.Burgerservicenummer,
					L.RelnrRedNed,
					L.Beroep,
					L.Legitimatietype,
					L.Legitimatienummer
					FROM %sLid AS L WHERE Nummer=%d;", $table_prefix, $lidid);
		$result = fnQuery($query);
		return $result->fetch();
					
	} elseif ($actie == "verjaardagen") {
		$df = sprintf("MONTH(L.GEBDATUM)=%s AND DAY(L.GEBDATUM)=%s", substr($filter, 5, 2), substr($filter, -2));
		$query = sprintf("SELECT L.Nummer, %1\$s AS `Naam_lid`, L.GEBDATUM, (YEAR('%5\$s')-YEAR(L.GEBDATUM))-IF(RIGHT('%5\$s', 5)<RIGHT(L.GEBDATUM, 5), 1, 0) AS Leeftijd
					FROM %2\$s
					WHERE %3\$s AND %4\$s AND L.GEBDATUM > '1901-01-01'
					ORDER BY L.GEBDATUM, L.Nummer;", $selectnaam, $fromlid, $wherelid, $df, $filter);
										
	} elseif ($actie == "ledenmetrekening") {
		$query = sprintf("SELECT L.Nummer, %1\$s AS `Naam_lid`
					FROM %2\$sLid AS L
					WHERE L.Nummer IN (SELECT MAX(RK.Lid) FROM %2\$sRekening AS RK WHERE RK.Lid = L.Nummer)
					ORDER BY L.Achternaam, L.Tussenv;", $selectzoeknaam, $table_prefix);
					
	} elseif ($actie == "lididbijemail") {
		$filter = strtolower($filter);
		$query = sprintf("SELECT IFNULL(MAX(L.Nummer), 0) FROM %1\$sLid AS L WHERE (LOWER(Email)='%2\$s' OR LOWER(EmailVereniging)='%2\$s' OR LOWER(EmailOuders)='%2\$s');", $table_prefix, $filter);
		$scalar = 1;
		
	} elseif ($actie == "mingebdatum") {
		$query = sprintf("SELECT SUBSTRING(MIN(GEBDATUM), 1, 10) FROM %s WHERE %s;", $fromlid, $wherelid);
		$scalar = 1;
		
	} elseif ($actie == "maxgebdatum") {
		$query = sprintf("SELECT SUBSTRING(MAX(GEBDATUM), 1, 10) FROM %s WHERE %s;", $fromlid, $wherelid);
		$scalar = 1;
		
	} elseif ($actie == "minpostcode") {
		$query = sprintf("SELECT MIN(Postcode) FROM %s WHERE %s;", $fromlid, $wherelid);
		$scalar = 1;
		
	} elseif ($actie == "maxpostcode") {
		$query = sprintf("SELECT MAX(Postcode) FROM %s WHERE %s;", $fromlid, $wherelid);
		$scalar = 1;

	} elseif ($actie == "minliddatum") {
		$query = sprintf("SELECT SUBSTRING(MIN(LM.LIDDATUM), 1, 10) FROM %s WHERE %s;", $fromlid, $wherelid);
		$scalar = 1;
		
	} elseif ($actie == "maxliddatum") {
		$query = sprintf("SELECT SUBSTRING(MAX(LM.LIDDATUM), 1, 10) FROM %s WHERE %s;", $fromlid, $wherelid);
		$scalar = 1;
		
	} elseif ($actie == "minopgezegd") {
		$query = sprintf("SELECT SUBSTRING(MIN(LM.Opgezegd), 1, 10) FROM %s WHERE %s;", $fromlid, $wherelid);
		$scalar = 1;
		
	} elseif ($actie == "maxopgezegd") {
		$query = sprintf("SELECT SUBSTRING(MAX(LM.Opgezegd), 1, 10) FROM %s WHERE %s;", $fromlid, $wherelid);
		$scalar = 1;
		
	} else {
		$mess = sprintf("onbekende actie '%s' in db_lid", $actie);
		debug($mess, 1, 0, 1);
		$query = "";
	}
	
	if ($scalar == 1) {
		return db_scalar($query);
	} else {
		$result = fnQuery($query);
		return $result->fetchAll();
	}
} # db_lid

function db_lidmaatschap($actie, $lidid=0, $opgezegd="", $rid=0, $liddatum="", $lidnr=0) {
	global $table_prefix, $fdaccess, $fdsql, $wherelid, $fdlang;

	$naamlid = db_naamlid($lidid);
	$rv = "";
	
	$liddatum = change_month_to_uk($liddatum);
	if (strlen($liddatum) > 4 and strtotime($liddatum) != false) {
		$liddatum = strftime($fdsql, strtotime($liddatum));
	} else {
		$liddatum = "";
	}
	
	$opgezegd = change_month_to_uk($opgezegd);
	if (strlen($opgezegd) > 4 and strtotime($opgezegd) != false) {
		$opgezegd = strftime($fdsql, strtotime($opgezegd));
	} else {
		$opgezegd = "";
	}

	if ($actie == "actueleleden") {
		$query = sprintf("SELECT LM.* FROM %sLidmaatschap AS LM WHERE %s ORDER BY LM.Lidnr;", $table_prefix, $wherelid);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "opzegging" and strlen($opgezegd) > 4) {
		$query = sprintf("UPDATE %sLidmaatschap SET Opgezegd='%s', Gewijzigd=SYSDATE() WHERE Lid=%d AND (Opgezegd IS NULL);", $table_prefix, $opgezegd, $lidid);
		$result = fnQuery($query);
		if ($result > 0) {
			$query = sprintf("UPDATE Lidmaatschap SET Opgezegd=%s, Gewijzigd=SYSDATE() WHERE Lid=%d AND (Opgezegd IS NULL);", accessdate($opgezegd), $lidid);
			db_interface("add", $query, $lidid);
			$mess = sprintf("De opzegging per %s van %s is verwerkt. ", $opgezegd, $naamlid);
		} else {
			$mess = sprintf("De opzegging per %s van %s is niet verwerkt. ", $opgezegd, $naamlid);
		}
		return $mess;
		
	} elseif ($actie == "record") {
		$query = sprintf("SELECT LM.*, IF(LM.OPGEZEGD IS NULL, 'Niet', DATE_FORMAT(LM.Opgezegd, %s)) AS OpgezegdPer
								FROM %sLidmaatschap AS LM
								WHERE LM.Lidnr=%d LIMIT 1;", $fdlang, $table_prefix, $lidnr);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "overzichtlid") {
		$query = sprintf("SELECT * FROM %sLidmaatschap
								WHERE Lid=%d
								ORDER BY LIDDATUM;", $table_prefix, $lidid);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "update" and $rid > 0) {
		$query = sprintf("SELECT * FROM %sLidmaatschap WHERE RecordID=%d AND Lid=%d;", $table_prefix, $rid, $lidid);
		$result = fnQuery($query);
		$cur = $result->fetch();
		
		if (strlen($lidnr) > 0 and is_numeric($lidnr) and $lidnr > 0 and $lidnr != $cur->Lidnr) {
			$set_clause = sprintf("Lidnr=%d, ", $lidnr);
			$wijz = sprintf("Veld Lidnr is %d geworden.", $lidnr);
		} else {
			$set_clause = "";
			$wijz = "";
		}
		$set_access = $set_clause;
		
		if (strlen($liddatum) > 4 and strtotime($liddatum) != false) {
			if ($liddatum != $cur->LIDDATUM) {
				$set_clause .= sprintf("LIDDATUM='%s', ", $liddatum);
				$set_access .= sprintf("LIDDATUM=%s, ", accessdate($liddatum));
				$wijz .= sprintf(" Veld Liddatum is '%s' geworden.", $liddatum);
			}
		}
		
		if (strlen($opgezegd) > 4 and strtotime($opgezegd) != false) {
			if ($opgezegd != $cur->Opgezegd) {
				$set_clause .= sprintf("Opgezegd='%s',", $opgezegd);
				$set_access .= sprintf("Opgezegd=%s,", accessdate($opgezegd));
				$wijz .= sprintf(" Veld Opgezegd is '%s' geworden.", $opgezegd);
			}
		} elseif (strlen($opgezegd) == 0 or strtolower($opgezegd) == "niet") {
			$set_clause .= "Opgezegd=NULL,";
			$set_access .= "Opgezegd=NULL,";
			$wijz .= " Veld Opgezegd is leeg gemaakt.";
		}
		if (strlen($set_clause) > 5) {
			$query = sprintf("UPDATE %sLidmaatschap SET %s Gewijzigd=SYSDATE() WHERE Lid=%d AND LIDDATUM='%s';", $table_prefix, $set_clause, $cur->Lid, $cur->LIDDATUM);
			$result = fnQuery($query);
			if ($result > 0) {
				$query = sprintf("UPDATE Lidmaatschap SET %s Gewijzigd=SYSDATE() WHERE Lid=%d AND LIDDATUM=%s;", $set_access, $cur->Lid, accessdate($cur->LIDDATUM));
				db_interface("add", $query, $lidid);
				$mess = sprintf("Lidmaatschap van lid %s is gewijzigd. %s", db_naamlid($lidid), $wijz);
				db_logboek("add", $mess, 6, $lidid);
			}
		}
	} elseif ($actie == "add" and $lidid > 0) {
		$query = sprintf("SELECT IFNULL(MAX(Lidnr), 100) FROM %sLidmaatschap;", $table_prefix);
		$lidnr = db_scalar($query) + 1;
		$liddatum = date("Y-m-d");
		$query = sprintf("INSERT INTO %sLidmaatschap (Lid, Lidnr, LIDDATUM, Opgezegd, Ingevoerd, Gewijzigd) VALUES (%d, %d, '%s', NULL, CURDATE(), SYSDATE());", $table_prefix, $lidid, $lidnr, $liddatum);
		$result = fnQuery($query);
		if ($result > 0) {
			$query = sprintf("INSERT INTO Lidmaatschap (Lid, Lidnr, LIDDATUM, Ingevoerd, Gewijzigd) VALUES (%d, %d, %s, CURDATE(), SYSDATE());", $lidid, $lidnr, accessdate($liddatum));
			db_interface("add", $query, $lidid);
			$mess = sprintf("Bij lid %s is een lidmaatschap met nummer %d toegevoegd.", db_naamlid($lidid), $lidnr);
			db_logboek("add", $mess, 6, $lidid);
		}
		
	} elseif ($actie == "soortlid" and $lidid > 0) {
		$query = sprintf("SELECT * FROM %sLidmaatschap WHERE Lid=%d ORDER BY LIDDATUM DESC;", $table_prefix, $lidid);
		$result = fnQuery($query);
		$row = $result->fetch();
		if (isset($row->Lid) and $row->Lid > 0) {
			if ($row->Opgezegd > '1970-01-01' and strtotime($row->Opgezegd) < time()) {
				$rv = "Voormalig lid";
			} elseif (strtotime($row->LIDDATUM) > time()) {
				$rv = "Toekomstig lid";
			}
		} else {
			$rv = "Kloslid";
		}
		return $rv;
	}
	
} # db_lidmaatschap

function db_memo($actie, $lidid, $srt, $nv="") {
	global $table_prefix, $arrSoortMemo;
	
	$nv = str_replace("\"", "", str_replace("'", "", $nv));	
	
	if ($actie == "curval" and $lidid > 0) {
		if ($srt == "WN") {
			$query = sprintf("SELECT `Waarschuwen bij nood` FROM %sLid WHERE Nummer=%d;", $table_prefix, $lidid);
		} else {
			$query = sprintf("SELECT `Memo` FROM %sMemo WHERE Lid=%d AND Soort='%s';", $table_prefix, $lidid, $srt);
		}
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "add" and strlen($nv) > 0) {
		$query = sprintf("INSERT INTO %sMemo (Lid, Soort, Memo, Ingevoerd, Gewijzigd) VALUES (%d, '%s', \"%s\", CURDATE(), SYSDATE());", $table_prefix, $lidid, $srt, $nv);
		$result = fnQuery($query);
		if ($result > 0) {
			db_interface("add", $query, $lidid);
			$mess = sprintf("Memo '%s' is bij %s met waarde '%s' toegevoegd.", $arrSoortMemo[$srt], db_naamlid($lidid), $nv);
		} else {
			$mess = sprintf("Het toevoegen van memo '%s' is bij %s met waarde '%s' is mislukt.", $arrSoortMemo[$srt], db_naamlid($lidid), $nv);
		}
		db_logboek("add", $mess, 6, $lidid);
		
	} elseif ($actie == "update") {
		if ($srt == "WN") {
			$query = sprintf("UPDATE %sLid SET `Waarschuwen bij nood`=\"%s\", Gewijzigd=SYSDATE() WHERE Nummer=%d;", $table_prefix, $nv, $lidid);
		} else {
			$query = sprintf("UPDATE %sMemo SET Memo=\"%s\", Gewijzigd=SYSDATE() WHERE Lid=%d AND Soort='%s';", $table_prefix, $nv, $lidid, $srt);
		}
		$result = fnQuery($query);
		if ($result > 0) {
			db_interface("add", $query, $lidid);
			$mess = sprintf("Memo '%s' is bij %s in '%s' gewijzigd.", $arrSoortMemo[$srt], db_naamlid($lidid), $nv);
			db_logboek("add", $mess, 6, $lidid);
		}
		
	} elseif ($actie == "delete") {
		$query = sprintf("DELETE FROM %sMemo WHERE Lid=%d AND Soort='%s';", $table_prefix, $lidid, $srt);
		$result = fnQuery($query);
		if ($result > 0) {
			db_interface("add", $query, $lidid);
			$mess = sprintf("Memo '%s' is bij %s verwijderd.", $arrSoortMemo[$srt], db_naamlid($lidid));
			db_logboek("add", $mess, 6, $lidid, 1);
		}
	
	} elseif ($actie == "aantal") {
		if (strlen($srt) > 0) {
			$query = sprintf("SELECT COUNT(*) FROM %sMemo WHERE Soort='%s';", $table_prefix, $srt);
		} else {
			$query = sprintf("SELECT COUNT(*) FROM %sMemo;", $table_prefix);
		}
		return db_scalar($query);
	}
} # db_ledenwijzigingen

function db_authorisation($actie, $recid=0, $tabpage="") {
	global $table_prefix, $sqllidgroepen;
	
	$tabpage = str_replace("'", "", $tabpage);
	if ($actie == "delete") {
		if ($_SESSION['webmaster'] == 1) {
			$query = sprintf("DELETE FROM %sAdmin_access WHERE RecordID=%d;", $table_prefix, $recid);
			if (fnQuery($query) > 0) {
				$mess = sprintf("In de tabel '%sAdmin_access' is record %d verwijderd.", $table_prefix, $recid);
				db_logboek("add", $mess, 15, 0, 0, $recid);
				$query = sprintf("OPTIMIZE TABLE %sAdmin_access;", $table_prefix);
				fnQuery($query);
			}
		} else {
			$mess = "Je hebt geen rechten om autorisaties te verwijderen.";
			db_logboek("add", $mess, 15, 0, 1);
		}
	} elseif ($actie == "lijst") {
		$query = sprintf("SELECT RecordID, Toegang, Tabpage, Ingevoerd FROM %sAdmin_access ORDER BY Tabpage;", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "tabpages") {
		$query = sprintf("SELECT DISTINCT Tabpage FROM %sAdmin_access WHERE Toegang > 0 ORDER BY Tabpage;", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "add") {
		$query = sprintf("INSERT INTO %sAdmin_access SET Tabpage=\"%s\", Ingevoerd=CURDATE();", $table_prefix, $tabpage);
		$result = fnQuery($query);
		if ($tabpage == "Vereniging/Introductie") {
			$query = sprintf("UPDATE %sAdmin_access SET Toegang=0 WHERE Tabpage='Vereniging/Introductie';", $table_prefix);
			$result = fnQuery($query);
		}
		$mess = sprintf("Tabpage '%s' is aan de tabel 'Admin_access' toegevoegd.", $tabpage);
		db_logboek("add", $mess, 15, 0, 0, $result);
		
	} elseif ($actie == "update") {  // Nog niet af en nog niet in gebruik
		$query = sprintf("UPDATE %1\$sAdmin_access SET Toegang=%2\$d, Gewijzigd=SYSDATE() WHERE RecordID=%3\$d AND Toegang<>%2\$d;", $table_prefix, $_POST[$vn], $recid);
		$result = fnQuery($query);
		if ($result > 0) {
			if ($_POST[$vn] == "-1") {
				$mess = sprintf("Totgang tot '%s' is alleen voor webmasters beschibaar gemaakt.", $row->Tabpage);
			} else {
				$mess = sprintf("Toegang '%s' is naar groep '%s' aangepast.", $row->Tabpage, db_naam_onderdeel($_POST[$vn], "Iedereen"));
			}
			db_logboek("add", $mess, 15, 0, 0, $recid);
		}
		
	} elseif ($actie == "heefttoegang") {
		if (strlen($tabpage) > 0) {
			$query = sprintf("SELECT COUNT(*) FROM %sAdmin_access WHERE Tabpage=\"%s\" AND (Toegang IN (%s) OR Toegang=0);", $table_prefix, $tabpage, $sqllidgroepen);
			if (db_scalar($query) > 0) {
				return true;
			} else {
//				debug($query, 0);
				return false;
			}
		} else {
			return false;
		}
		
	} elseif ($actie == "opschonen") {
		$query = sprintf("DELETE FROM %sAdmin_access WHERE Ingevoerd < DATE_ADD(CURDATE(), INTERVAL -3 MONTH) AND Toegang < 0;", $table_prefix);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Er zijn %d records verwijderd uit Admin_access.", $result);
			$query = sprintf("OPTIMIZE TABLE %sAdmin_access;", $table_prefix);
			fnQuery($query);
		} else {
			$mess = "Bij het opschonen zijn geen records uit Admin_access verwijderd.";
		}
		db_logboek("add", $mess, 15, 0, 1);
	}
} # db_authorisation

function db_bewaking($actie, $seizoen=-1, $lidid=-1, $week=-1) {
	global $table_prefix, $selectnaam;
	
	if ($actie == "aantallen" and $lidid > -1) {
		if ($seizoen > 0) {
			$xtrawhere = sprintf("AND BW.SeizoenID=%d", $seizoen);
		} else {
			$xtrawhere = "";
		}
		$query = sprintf("SELECT COUNT(*) FROM %sBewaking AS BW WHERE BW.Lid=%d %s;", $table_prefix, $lidid, $xtrawhere);
		$result = fnQuery($query);
		return $result->fetchColumn();
	} elseif ($actie == "huidigeweek") {
		$query = sprintf("SELECT IFNULL(MIN(WeekNr), 0) FROM %sBewaking
								WHERE SeizoenID=%d AND BEGIN_PER <= CURDATE() AND EINDE_PER >= CURDATE();", $table_prefix, $seizoen);
		return db_scalar($query);
	} else {
		if ($actie == "aantallen") {
			$query = sprintf("SELECT ST_LEN_BP FROM %sBewseiz WHERE RecordID=%d;", $table_prefix, $seizoen);
			$result = fnQuery($query);
			if ($result->fetchColumn() == 7) {
				$query = sprintf("SELECT Weeknr AS intWeeknr, MIN(BEGIN_PER) AS dteBegin, MAX(EINDE_PER) AS dteEinde, ROUND(SUM(DATEDIFF(BW.EINDE_PER, BW.BEGIN_PER)+1)/7, 0) AS intAantal,
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
	
		} elseif ($actie == "insseizoenen") {
			$query = sprintf("SELECT DISTINCT BS.RecordID, BS.Kode, BS.Gewijzigd, BS.ST_LEN_BP
									FROM %1\$sBewseiz AS BS INNER JOIN %1\$sBewaking_Blok AS BB ON BS.RecordID = BB.SeizoenID
									ORDER BY BS.Begindatum DESC, BS.Kode;", $table_prefix);
									
		} elseif ($actie == "rooster") {
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
									
		} elseif ($actie == "seizoenen") {
			$query = sprintf("SELECT BS.RecordID, BS.Kode, BS.Gewijzigd, BS.ST_LEN_BP FROM %sBewseiz AS BS ORDER BY BS.Begindatum DESC, BS.Kode;", $table_prefix);
		}
		$result = fnQuery($query);
		return $result;
	}
}

function db_bewaking_blok($actie, $seizoen=-1, $blokid=0, $field="", $val="") {
	global $table_prefix, $selectnaam, $fdsql;
	
	$w = "";
	if ($seizoen > 0) {
		$w = sprintf("WHERE BB.SeizoenID=%d", $seizoen);
	}
	
	if ($actie == "overzicht") {
		$query = sprintf("SELECT BB.*, BS.Kode AS KodeSeizoen, BS.Lokatie AS Locatie, BS.KeuzesBijInschrijving, BB.Kode AS Weeknr, BB.Kode AS intWeeknr
								FROM %1\$sBewaking_Blok AS BB LEFT OUTER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								%2\$s
								ORDER BY BS.Begindatum, BS.Kode, BB.Begindatum;", $table_prefix, $w);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "openblokken") {
		$query = sprintf("SELECT BB.*, BS.Kode AS KodeSeizoen, BS.Lokatie AS Locatie, BS.KeuzesBijInschrijving
								FROM %1\$sBewaking_Blok AS BB INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								WHERE BB.InschrijvingOpen=1 AND BB.Einddatum >= CURDATE()
								ORDER BY BS.Begindatum, BS.Kode, BB.Begindatum;", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "omsblok") {
		$query = sprintf("SELECT CONCAT(BS.Kode, ' / ', IF(ISNULL(BB.Kode), BB.Begindatum, BB.Kode)) AS OmsBlok
								FROM %1\$sBewaking_Blok AS BB LEFT OUTER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								WHERE BB.RecordID=%2\$d;", $table_prefix, $blokid);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "ingebruik") {
		$query = sprintf("SELECT COUNT(*) FROM %sBewaking_Inschrijving WHERE BlokID=%d;", $table_prefix, $blokid);
		$result = fnQuery($query);
		if ($result->fetchColumn() > 0) {
			return true;
		} else {
			return false;
		}
		
	} elseif ($actie == "add" and $seizoen > 0) {
		$query = sprintf("SELECT DATE_ADD(BB.Einddatum, INTERVAL 1 DAY) AS B, DATE_ADD(BB.Einddatum, INTERVAL BS.ST_LEN_BP DAY) AS E, BB.Kode
								FROM %1\$sBewaking_Blok AS BB INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								WHERE BB.SeizoenID=%2\$d AND BB.Einddatum > '2001-01-01' ORDER BY BB.Einddatum DESC;", $table_prefix, $seizoen);
		$result = fnQuery($query);
		$vb = $result->fetch();
		if (isset($vb->B)) {
			$b = $vb->B;
			$e = $vb->E;
			if (strlen($vb->Kode) > 0 and is_numeric($vb->Kode)) {
				$k = $vb->Kode + 1;
			} else {
				$k = "";
			}
		} else {
			$b = date("Y-m-d");
			$e = date("Y-m-d");
			$k = "";
		}
		$query = sprintf("INSERT INTO %sBewaking_Blok SET SeizoenID=%d, Kode='%s', Begindatum='%s', Einddatum='%s', Ingevoerd=CURDATE(), Gewijzigd=SYSDATE();", $table_prefix, $seizoen, $k, $b, $e);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Bewakingsblok %d is toegevoegd.", $result);
			db_logboek("add", $mess, 11, 0, 1, $result);
		}
		
	} elseif ($actie == "edit" and $blokid > 0 and strlen($field) > 0) {
		if ($field == "InschrijvingOpen") {
			$set_clause = sprintf("`%s`=%d", $field, $val);
		} else {
			$set_clause = sprintf("`%s`='%s'", $field, $val);
		}
		
		$query = sprintf("UPDATE %sBewaking_Blok SET %s, Gewijzigd=SYSDATE() WHERE RecordID=%d;", $table_prefix, $set_clause, $blokid);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Veld '%s' in Bewakingsblok %d is in '%s' gewijzigd.", $field, $blokid, $val);
		} else {
			$mess = sprintf("Veld '%s' in Bewakingsblok %d is niet gewijzigd.", $field, $blokid);
		}
		db_logboek("add", $mess, 11, 0, 0, $blokid);
		
	} elseif ($actie == "delete" and $blokid > 0) {
		$oms = db_bewaking_blok("omsblok", 0, $blokid);
		if (db_bewaking_blok("ingebruik", 0, $blokid)) {
			$mess = sprintf("Bewakingsblok %d (%s) wordt niet verwijderd, omdat het nog gebruikt wordt.", $oms);
		} else {
			$query = sprintf("DELETE FROM %sBewaking_Blok WHERE RecordID='%d';", $table_prefix, $blokid);
			$result = fnQuery($query);
			if ($result > 0) {
				$mess = sprintf("Bewakingsblok %d (%s) is verwijderd.", $blokid, $oms);
			} else {
				$mess = sprintf("Bewakingsblok %d is niet verwijderd, omdat het niet bestaat.", $blokid);
			}
		}
		db_logboek("add", $mess, 11, 0, 1, $blokid);
		
	} else {
		debug(sprintf("Onbekende actie '%s' in db_bewaking_blok", $actie));
	}
} # db_bewaking_blok

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
	
	if ($actie == "overzicht") {
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
								SUM(IF(BI.Definitief <= BI.Ingevoerd, 1, 0)) AS `Niet definitief`,
								IF(BB.InschrijvingOpen=1, 'Ja', 'Nee') AS `Open?`
								FROM (%1\$sBewaking_Blok AS BB INNER JOIN (%1\$sBewaking_Inschrijving AS BI INNER JOIN %1\$sLid AS L ON BI.Lid = L.Nummer) ON BB.RecordID = BI.BlokID) INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								%2\$s
								GROUP BY BS.Kode, BB.Kode, BB.Begindatum, BB.Einddatum, IF(BB.InschrijvingOpen=1, 'Ja', 'Nee')
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
								IF(BI.Keuze=3, 1, BI.Keuze) AS Keuze, BI.Opmerking, %2\$s AS ndNaamBewaker, L.Roepnaam AS ndRoepnaamBewaker,
								L.Email AS ndEmailBewaker, L.EmailOuders AS ndEmailOuders, L.GEBDATUM AS ndGeboren,
								L.Adres AS ndAdres, L.Postcode AS ndPostcode, L.Woonplaats AS ndWoonplaats
								FROM (%1\$sBewaking_Blok AS BB INNER JOIN (%1\$sBewaking_Inschrijving AS BI INNER JOIN %1\$sLid AS L ON BI.Lid = L.Nummer) ON BB.RecordID = BI.BlokID) INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								WHERE BB.Einddatum > CURDATE() AND %3\$s AND BI.Definitief > BI.Ingevoerd
								ORDER BY BS.Kode, BB.Kode, BB.Begindatum;", $table_prefix, $selectnaam, $w);
		$result = fnQuery($query);
		return $result->fetchAll();	
		
	} elseif ($actie == "laatstelid") {
		$query = sprintf("SELECT IFNULL(MAX(Nummer), 0) FROM %sBewaking_Inschrijving WHERE Lid=%d;", $table_prefix, $lidid);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "inschrijving") {
		$query = sprintf("SELECT BI.* FROM %sBewaking_Inschrijving AS BI
								WHERE BI.Lid=%d AND BI.BlokID=%d;", $table_prefix, $lidid, $blok);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "export") {
		$query = sprintf("SELECT %1\$s AS NaamLid, BI.Lid, BB.SeizoenID, BB.Begindatum, BB.Einddatum
								FROM (%2\$sBewaking_Inschrijving AS BI INNER JOIN %2\$sLid AS L ON BI.Lid = L.Nummer) INNER JOIN %2\$sBewaking_Blok AS BB ON BI.BlokID = BB.RecordID
								WHERE IFNULL(BI.Afgemeld, '0000-00-00') < BI.Ingevoerd AND BI.Definitief > BI.Ingevoerd AND BI.Keuze IN (1, 3)
								ORDER BY L.Achternaam, L.Tussenv, L.Roepnaam, BB.Begindatum;", $selectnaam, $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "add" and $lidid > 0 and $blok > 0 and $insnr > 0) {
		$query = sprintf("INSERT INTO %sBewaking_Inschrijving (BlokID, Lid, Nummer, Keuze, Opmerking, Ingevoerd)
								VALUES (%d, %d, %d, %d, \"%s\", CURDATE());", $table_prefix, $blok, $lidid, $insnr, $keuze, $opmerk);
		fnQuery($query);
		if ($keuze > 0) {
			$mess = sprintf("Inschrijving bewaking voor blok '%s' met keuze %d is bij ingelogd lid toegevoegd.", db_bewaking_blok("omsblok", 0, $blok), $keuze);
			if (strlen($opmerk) > 1) {
				$mess .= sprintf(" Opmerking is '%s'.", $opmerk); 
			}
			db_logboek("add", $mess, 11, 0, 0, $blok);
		}
		
	} elseif ($actie == "update" and $lidid > 0 and $blok > 0) {
		$query = sprintf("UPDATE %sBewaking_Inschrijving SET Nummer=%d, Keuze=%d, Opmerking=\"%s\", Definitief=NULL
								WHERE BlokID=%d AND Lid=%d;", $table_prefix, $insnr, $keuze, $opmerk, $blok, $lidid);
		fnQuery($query);
		$mess = sprintf("Inschrijving bewaking voor blok '%s' is bij ingelogd lid bijgewerkt. Keuze is %d.", db_bewaking_blok("omsblok", 0, $blok), $keuze);
		if (strlen($opmerk) > 0) {
			$mess .= sprintf(" Opmerking is '%s'.", $opmerk);
		}
		db_logboek("add", $mess, 11, 0, 0, $blok);

	} elseif ($actie == "delete" and $lidid > 0 and $blok > 0) {
		$query = sprintf("DELETE FROM %sBewaking_Inschrijving WHERE BlokID=%d AND Lid=%d;", $table_prefix, $blok, $lidid);
		fnQuery($query);
		$mess = sprintf("Inschrijving bewaking voor blok '%s' is bij %s verwijderd.", db_bewaking_blok("omsblok", 0, $blok), db_naamlid($lidid));
		db_logboek("add", $mess, 11, 0, 0, $blok);
		
	} elseif ($actie == "defmaken" and $lidid > 0 and $insnr > 0) {
		$query = sprintf("UPDATE %sBewaking_Inschrijving SET Definitief=SYSDATE() WHERE Nummer=%d AND Lid=%d;", $table_prefix, $insnr, $lidid);
		$result = fnQuery($query);
		return $result;
		
	} elseif ($actie == "afmelden") {
		$query = sprintf("UPDATE %sBewaking_Inschrijving AS BI SET Afgemeld=SYSDATE()
					WHERE IFNULL(BI.Afgemeld, '0000-00-00') < BI.Ingevoerd AND BI.Definitief > BI.Ingevoerd AND BI.Keuze IN (1, 3);", $table_prefix);
		$result = fnQuery($query);
		$mess = sprintf("Er zijn %d inschrijvingen voor bewaking afgemeld.", $result);
		db_logboek("add", $mess, 11);
		return $mess;
		
	} else {
		debug(sprintf("Onbekende actie '%s' in db_insbew", $actie));
	}
}

function db_lidond($actie, $lidid=0, $per="", $filter="", $onderdlid=0) {
	global $selectnaam, $selectzoeknaam, $table_prefix, $fromlidond, $wherelidond, $selectlidnr;

	if ($actie == "laatstgewijzigd") {
		$query = sprintf("SELECT Max(Gewijzigd) FROM %sLidond;", $table_prefix);
		return db_scalar($query);
		
	} elseif ($actie == "bewaking" and strlen($per) == 10) {
		$query  = sprintf("SELECT DISTINCT O.Kode FROM %s
					 WHERE O.`Tonen in bewakingsadministratie`=1 AND LO.Lid=%2\$d
					 AND LO.Vanaf <= '%3\$s' AND ((LO.OPGEZEGD IS NULL) OR OPGEZEGD > '%3\$s')
					 ORDER BY O.Kode;", $fromlidond, $lidid, $per);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "onderdeellijst") {
		if (strlen($filter) > 0) {
			$filter = " WHERE " . $filter;
		}
		$query = sprintf("SELECT L.Nummer AS lnkNummer, IFNULL((%5\$s), 0) AS numLidnr, %1\$s AS `Naam_lid`, UPPER(L.Woonplaats) AS Woonplaats,
								L.Mobiel, L.Email AS `emlE-mail`, F.Omschrijv AS Functie, LO.OPMERK AS Opmerking, LO.Vanaf AS dteVanaf,
								LO.Opgezegd AS dteTotenmet, L.Nummer AS llkNummer, L.GEBDATUM AS ndGEBDATUM, %2\$s AS ndZoeknaam
								FROM %3\$s
								%4\$s
								ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", $selectnaam, $selectzoeknaam, $fromlidond, $filter, $selectlidnr);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "actueleleden") {
		if (strlen($filter) > 0) {
			$filter = $wherelidond . " AND " . $filter;
		} else {
			$filter = $wherelidond;
		}
		$query = sprintf("SELECT LO.Lid, LO.EmailFunctie, LO.Opmerk, LO.Vanaf, LO.Opgezegd,
								L.Nummer, %s AS NaamLid, UPPER(L.Woonplaats) AS Woonplaats, L.Mobiel, L.Email, L.EmailVereniging, L.GEBDATUM, %s AS ZoeknaamLid,
								F.Omschrijv AS Functie,
								O.Kode, O.Naam AS OndNaam, O.CentraalEmail, O.TYPE
								FROM %s
								WHERE %s
								ORDER BY O.Kader DESC, O.TYPE, O.Naam, F.Sorteringsvolgorde, F.Omschrijv, LO.Opmerk, LO.Vanaf, L.Achternaam, L.TUSSENV, L.Roepnaam;", $selectnaam, $selectzoeknaam, $fromlidond, $filter);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "kaderonderdeel" and $lidid > 0) {
		$query = sprintf("SELECT IF(O.`Type`='F', F.OMSCHRIJV, O.Naam) AS Ond FROM %s WHERE %s AND LO.Lid=%d AND O.Kader=1;", $fromlidond, $wherelidond, $lidid);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "onderscheiding" and $lidid > 0) {
		$query = sprintf("SELECT O.Naam FROM %1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON LO.OnderdeelID=O.RecordID
					WHERE LO.Lid=%2\$d AND O.`Type`='O' AND (LO.Opgezegd IS NULL) ORDER BY LO.Vanaf DESC;", $table_prefix, $lidid);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "islid" and $lidid > 0 and $onderdlid > 0) {
		
		$query = sprintf("SELECT COUNT(*) FROM %sLidond AS LO WHERE %s AND Lid=%d AND OnderdeelID=%d;", $table_prefix, $wherelidond, $lidid, $onderdlid);
		if (db_scalar($query) == 0) {
			return false;
		} else {
			return true;
		}
	} elseif ($actie == "lidgroepen") {
		$rv = "";
		$query = sprintf("SELECT OnderdeelID FROM %sLidond AS LO WHERE Lid=%d AND %s;", $table_prefix, $_SESSION['lidid'], $wherelidond);
		$result = fnQuery($query);
		$rows = $result->fetchAll(PDO::FETCH_COLUMN);
		if (count($rows) > 0) {
			return "(0, " . implode(", ", $rows) . ")";
		} else {
			return "(0)";
		}
	}
} # db_lidond

function db_logins($actie, $email="", $login="", $lidid=0, $xtrafilter="", $pw="", $xtraorder="") {
	global $dbc, $selectnaam, $selectlidnr, $table_prefix, $lididwebmasters, $wherelidond, $wherelid, $dir, $pwfromcookie;
	
	if (strlen(db_param("login_beperkttotgroep")) > 0) {
		$beperkttotgroep = explode(",", db_param("login_beperkttotgroep"));
	} else {
		$beperkttotgroep = array(0);
	}
	
	$mess = "";
	if ($actie == "nieuwlogin" and isValidMailAddress($email) and strlen($login) > 5) {
		$query = sprintf("SELECT L.Nummer, Login.Login FROM %1\$sLid AS L LEFT OUTER JOIN %1\$sAdmin_login AS Login ON L.Nummer = Login.LidID
								WHERE (LOWER(L.Email) LIKE '%2\$s' OR LOWER(L.EmailVereniging) LIKE '%2\$s')", $table_prefix, strtolower($email));
		$result = fnQuery($query);
		$row = $result->fetch();
		if (isset($row->Nummer) and $row->Nummer > 0) {
			if (strlen($row->Login) > 5 and $row->Login != $login) {
				$mess = sprintf("Er is al een ander login aan dit emailadres gekoppeld.");
			} elseif (strlen($login) > 5 and strlen($login) <= db_param("login_maxlengte")) {
				$mess = db_logins("add", "", $login, $row->Nummer, "", $pw);
			}
		} else {
			$mess = sprintf("E-mailadres '%s' is onbekend in onze database. Er wordt geen login aangemaakt.", $email);
		}
		db_logboek("add", $mess, 5);
		return $mess;
		
	} elseif ($actie == "add" and strlen($login) > 0 and strlen($pw) > 0) {
		$login = cleanlogin($login);
		$query = sprintf("SELECT Login FROM %sAdmin_login WHERE LidID=%d;", $table_prefix, $lidid);
		$ok = false;
		if (strlen(db_scalar($query)) == 0) {
			if (db_logins("controlebestaat", "", $login, $lidid)) {
				$mess = sprintf("Login '%s' is al in gebruik bij een ander lid.", $login);
			} elseif (strlen($login) > 5 and $lidid > 0 and strlen($pw) > 5) {
				$nk = password_hash(newkey(), PASSWORD_DEFAULT);
				$query = sprintf("INSERT INTO %sAdmin_login SET LidID=%d, Login='%s', Wachtwoord='%s', ActivatieKey='%s', Ingevoerd=SYSDATE(), Gewijzigd=SYSDATE();", $table_prefix, $lidid, $login, password_hash($pw, PASSWORD_DEFAULT), $nk);
				$result = fnQuery($query);
				$mess = sprintf("Login '%s' is voor %s aangemaakt.", $login, db_naamlid($lidid));
				$mess .= fnValidatieLogin($lidid, $nk, "mail");
				$ok = true;
			}
		} else {
			$mess = sprintf("%s heeft al een login, er wordt geen nieuwe aangemaakt.", db_naamlid($lidid));
		}
		db_logboek("add", $mess, 5, 0, 1, $lidid);
		return $ok;
		
	} elseif ($actie == "rowlogin" and (isValidMailAddress($email, 0) or $lidid > 0)) {
		$query = sprintf("SELECT L.Nummer, Login.Login, %s AS Naam, L.Roepnaam, L.Email, Login.Wachtwoord, Login.FouteLogin, Login.Gewijzigd,
								IFNULL((%s), L.Nummer) AS Lidnr,", $selectnaam, $selectlidnr);
		if (is_array($beperkttotgroep) and count($beperktotgroep) > 0) {
			$query .= sprintf("(SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.Lid=L.Nummer AND LO.OnderdeelID IN (%s) AND LO.Vanaf <= CURDATE() AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd > CURDATE())) AS AanvragenMag", $table_prefix, implode(", ", $beperkttotgroep));
		} else {
			$query .= "1 AS AanvragenMag";
		}
		$query .= sprintf(" FROM %1\$sLid AS L LEFT OUTER JOIN %1\$sAdmin_login AS Login ON L.Nummer = Login.LidID WHERE ", $table_prefix);
		if (isValidMailAddress($email, 0)) {
			$query .= sprintf("(LOWER(L.Email) LIKE '%1\$s' OR LOWER(L.EmailVereniging) LIKE '%1\$s' OR LOWER(L.EmailOuders) LIKE '%1\$s')", strtolower($email));
		} else {
			$query .= sprintf("L.Nummer=%d", $lidid);
		}
		if (strlen($xtrafilter) > 0) {
			$query .= " AND " . $xtrafilter;
		}
		$query .= ";";
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "valideerlogin" and $lidid > 0) {
		$query = sprintf("UPDATE %sAdmin_login SET ActivatieKey='', Gewijzigd=SYSDATE() WHERE LENGTH(ActivatieKey) > 5 AND LidID=%d;", $table_prefix, $lidid);
		$result = fnQuery($query);
		if ($result == 1) {
			$mess = sprintf("Login voor %s is gevalideerd en kan worden gebruikt.", db_naamlid($lidid, "Naam"));
		} else {
			$mess = sprintf("Er ging iets mis bij het valideren van de login voor %s is gevalideerd. Mogelijk is deze login al gevalideerd. Zo niet probeer het later opnieuw.", db_naamlid($lidid, "Naam"));
		}
		db_logboek("add", $mess, 5);
		return $mess;
		
	} elseif ($actie == "controlebestaat" and strlen($login) > 5) {
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login WHERE Login LIKE '%s' AND LidID<>%d;", $table_prefix, $login, $lidid);
		if (db_scalar($query) > 0) {
			return true;
		} else {
			return false;
		}
		
	} elseif ($actie == "foutelogin" and $lidid > 0) {
		$query = sprintf("SELECT FouteLogin FROM %sAdmin_login WHERE LidID=%d", $table_prefix, $lidid);
		return db_scalar($query);
		
	} elseif ($actie == "uitloggen") {
		$query = sprintf("UPDATE %sAdmin_login SET Ingelogd=0 WHERE Ingelogd=1 AND (LastActivity < ADDDATE(SYSDATE(), INTERVAL -21 MINUTE) OR LidID=%d);", $table_prefix, $lidid);
		$result = fnQuery($query);
		if ($result > 0) {
			if ($_SESSION['lidid'] == $lidid and $lidid > 0) {
				$mess = sprintf("%s (LidID: %d) is uitgelogd.", $_SESSION['naamingelogde'], $lidid);
				db_logboek("add", $mess, 1, $lidid);
				$_SESSION['lidid'] = 0;
				fnMaatwerkNaUitloggen();
			}
		}
		if (db_param("login_geldigheidactivatie") > 0) {
			$query = sprintf("UPDATE %1\$sAdmin_login SET ActivatieKey='', Wachtwoord='', Gewijzigd=SYSDATE()
						WHERE Ingelogd=0 AND LENGTH(ActivatieKey) > 5 AND Gewijzigd < ADDDATE(SYSDATE(), INTERVAL -%2\$d HOUR) AND (Gewijzigd IS NOT NULL) AND Gewijzigd > '2000-01-01';"
						, $table_prefix, db_param("login_geldigheidactivatie"));
			$result = fnQuery($query);
//			debug($query, 1, 0, 1);
			if ($result > 0) {
				db_logboek("add", sprintf("Er zijn van %d logins de validatie ongeldig gemaakt.", $result), 5);
			}
		}
		
	} elseif ($actie == "aantalingelogd") {
		db_logins("uitloggen");
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login WHERE Ingelogd=1;", $table_prefix);
		return db_scalar($query);
		
	} elseif ($actie == "aantal") {
		$query = sprintf("SELECT COUNT(*) FROM %1\$sAdmin_login AS Login INNER JOIN %1\$sLid AS L ON Login.LidID = L.Nummer;", $table_prefix);
		return db_scalar($query);
		
	} elseif ($actie == "delete" and $lidid > 0) {
		if ($_SESSION['webmaster'] == 1) {
			$query = sprintf("DELETE FROM %sAdmin_login WHERE LidID=%d;", $table_prefix, $lidid);
			$result = fnQuery($query);
			if ($result > 0) {
				$mess = sprintf("De login van %s is verwijderd.", db_naamlid($lidid));
				db_logboek("add", $mess, 5, $lidid);
				fnQuery(sprintf("OPTIMIZE TABLE %sAdmin_login;", $table_prefix));
			}
		} else {
			$mess = "Je hebt geen rechten om logins te verwijderen.";
			db_logboek("add", $mess, 5, 0, 1);
		}
		
	} elseif ($actie == "setingelogd" and $lidid > 0) {
		$query = sprintf("UPDATE %sAdmin_login SET LastLogin=SYSDATE(), Ingelogd=1, FouteLogin=0 WHERE Ingelogd=0 AND LidID=%d;", $table_prefix, $lidid);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("%s is met '%s' ingelogd.", db_naamlid($lidid), $_SESSION['username']);
			db_logboek("add", $mess, 1, $lidid);
		}
		
	} elseif ($actie == "updateactiviteit" and $lidid > 0) {
		$query = sprintf("UPDATE %sAdmin_login SET LastActivity=SYSDATE() WHERE LidID=%d;", $table_prefix, $lidid);
		fnQuery($query);	
		
	} elseif ($actie == "wachtwoordreset" and $lidid > 0) {
		$nk = newkey();
		if (strlen($nk) > 0) {
			$query = sprintf("UPDATE %sAdmin_login SET ActivatieKey='%s', Wachtwoord='', Gewijzigd=SYSDATE() WHERE LidID=%d;", $table_prefix, password_hash($nk, PASSWORD_DEFAULT), $lidid);
			$result = fnQuery($query);
			if ($result == 1) {
				$mess = sprintf("Het wachtwoord van %s is leeg gemaakt op verzoek van een gebruiker, zodat het hersteld kan worden.", db_naamlid($lidid));
				db_logboek("add", $mess, 5);
				return $nk;
			} else {
				return false;
			}
		} else {
			return false;
		}
		
	} elseif ($actie == "nieuweactivitiekey") {
		$nk = newkey();
		if (strlen($nk) > 5 and $lidid > 0) {
			$query = sprintf("UPDATE %sAdmin_login SET ActivatieKey='%s', Gewijzigd=SYSDATE() WHERE LidID=%d;", $table_prefix, password_hash($nk, PASSWORD_DEFAULT), $lidid);
			$result = fnQuery($query);
			if ($result == 1) {
				$mess = sprintf("Er is een nieuwe activatiekey gemaakt voor de login van %s.", db_naamlid($lidid));
				db_logboek("add", $mess, 5, 0, 0, $lidid);
				return $nk;
			} else {
				return false;
			}
		} else {
			return false;
		}
		
	} elseif ($actie == "unlock" and $lidid > 0 and $_SESSION['webmaster'] == 1) {
		$query = sprintf("UPDATE %sAdmin_login SET FouteLogin=0, Gewijzigd=SYSDATE() WHERE LidID=%d AND FouteLogin > 0;", $table_prefix, $lidid);
		$result = fnQuery($query);
		if ($result == 1) {
			$mess = sprintf("Het aantal foutieve inlogpogingen van de login van %s is gereset.", db_naamlid($lidid));
			db_logboek("add", $mess, 1, $lidid);
		}
		
	} elseif ($actie === "controle") {
		if (db_param("login_autounlock") > 0) {
			$query = sprintf("UPDATE %sAdmin_login SET FouteLogin=0, Gewijzigd=SYSDATE()
						WHERE Gewijzigd < DATE_ADD(SYSDATE(), INTERVAL -%d MINUTE) AND FouteLogin > 0;", $table_prefix, db_param("login_autounlock"));
			$result = fnQuery($query);
			if ($result > 0) {
				$mess = sprintf("%d logins zijn automatisch ge-unlocked.", $result);
				db_logboek("add", $mess, 5);
			}
		}
	
		$xtrawhere = "";
		if (isValidMailAddress($login, 0)) {
			$query = sprintf("SELECT Login FROM %1\$sLid AS L INNER JOIN %1\$sAdmin_login AS Login ON Login.LidID = L.Nummer WHERE (LOWER(L.Email) LIKE '%2\$s' OR LOWER(L.EmailVereniging) LIKE '%2\$s')", $table_prefix, strtolower($login));
			$result = fnQuery($query);
			$rows = $result->fetchAll();
			if (count($rows) == 1) {
				$login = $rows[0]->Login;
			}
		}
		$xtrawhere = sprintf("AND Login.Login LIKE '%s'", cleanlogin($login));
		if (is_array($beperkttotgroep) and count($beperkttotgroep) > 0) {
			$xtrawhere .= sprintf(" AND (Login.LidID IN (SELECT Lid FROM %sLidond AS LO WHERE %s AND LO.OnderdeelID IN (%s)) OR Login.LidID IN (%s))", $table_prefix, $wherelidond, implode(", ", $beperkttotgroep), implode(", ", $lididwebmasters));
		} else {
			$xtrawhere .= sprintf(" AND Login.LidID IN (%s)", implode(", ", $lididwebmasters));
		}
		$query = sprintf("SELECT Login.LidID, Login.Login, %3\$s AS NaamLid, L.Roepnaam, L.Email, L.EmailVereniging, L.EmailOuders, Login.Wachtwoord, Login.ActivatieKey,
					IFNULL((%4\$s), 0) AS Lidnr, Login.FouteLogin, Login.Gewijzigd
					FROM %1\$sAdmin_login AS Login INNER JOIN %1\$sLid AS L ON Login.LidID = L.Nummer
					WHERE LENGTH(Login.Wachtwoord) > 5 %2\$s
					ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", $table_prefix, $xtrawhere, $selectnaam, $selectlidnr);
		$result = fnQuery($query);
		$row = $result->fetch();
		if (isset($row->LidID) and $row->LidID > 0) {
			if (password_verify($pw, $row->Wachtwoord)) {
				return $row;
			} else {
				if ($pwfromcookie == 0) {
					$query = sprintf("UPDATE %sAdmin_login SET FouteLogin=FouteLogin+1, Gewijzigd=SYSDATE() WHERE Login='%s';", $table_prefix, $login);
					fnQuery($query);
				}
				return false;
			}
		} else {
			return false;
		}
		
	} elseif ($actie == "lijst") {
		$xtrawhere = "";
		if (strlen($xtrafilter) > 0) {
			$xtrawhere = " AND " . $xtrafilter;
		}
		if (strlen($xtraorder) > 0) {
			$xtraorder .= ", ";
		}
		
		$query = sprintf("SELECT Login.LidID AS lnkNummer, Login.LidID, Login.Login, %1\$s AS Naam_lid, L.Woonplaats, (%5\$s) AS Lidnr, L.Roepnaam AS ndRoepnaam,
					L.Achternaam AS ndAchternaam, L.Tussenv AS ndTussenvoegsels, L.Meisjesnm AS ndMeisjesnaam, L.GEBDATUM AS ndGeboren,
					IF(LENGTH(L.Email)=0, L.EmailOuders, L.Email) AS `E-mail`, L.EmailVereniging AS ndEmailVereniging, L.Telefoon AS ndTelefoon, L.Mobiel AS ndMobiel,
					Login.Ingevoerd AS dteIngevoerd, Login.LastLogin AS `dttLaatste login`,
					IF(LENGTH(Login.Wachtwoord) > 5 AND LENGTH(IFNULL(Login.ActivatieKey, '')) = 0, 'Ja', 'Nee') AS `Gevalideerd`,
					Login.Wachtwoord AS ndWachtwoord, IF(Login.FouteLogin > 0, Login.LidID, 0) AS llkNummer
					FROM %2\$sAdmin_login AS Login INNER JOIN %2\$sLid AS L ON Login.LidID = L.Nummer
					WHERE LENGTH(Login.Login) > 5 %3\$s
					ORDER BY %4\$sL.Achternaam, L.TUSSENV, L.Roepnaam, Login.LidID;", $selectnaam, $table_prefix, $xtrawhere, $xtraorder, $selectlidnr);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "lidid" and strlen($login) > 4) {
		$query = sprintf("SELECT MAX(LidID) FROM %Admin_login WHERE Login='%s';", $table_prefix);
		return db_scalar($query);
		
	} elseif ($actie == "opschonen") {
		if (db_param("login_bewaartijd") > 0) {
			$query = sprintf("DELETE FROM %1\$sAdmin_login
						WHERE LastLogin < DATE_ADD(CURDATE(), INTERVAL -%2\$d MONTH) AND Ingevoerd < DATE_ADD(CURDATE(), INTERVAL -%2\$d MONTH);", $table_prefix, db_param("login_bewaartijd"));
			$result = fnQuery($query);
			if ($result > 0) {
				$mess = sprintf("Er zijn %d logins verwijderd", $result);
			} else {
				$mess = "Er zijn geen logins verwijderd";
			}
			$mess .= sprintf(", omdat ze %d maanden niet meer zijn gebruikt.<br>\n", db_param("login_bewaartijd"));
			db_logboek("add", $mess, 2, 0, 1);
		}
		$mess = "";
		$where = "";
		if (is_array($beperkttotgroep) and count($beperkttotgroep) > 0) {
			$where .= sprintf("LidID NOT IN (SELECT Lid FROM %sLidond AS LO WHERE %s AND LO.OnderdeelID IN (%s))", $table_prefix, $wherelidond, implode(", ", $beperkttotgroep));
		}
		if (strlen($where) > 0) {
			$where .= sprintf(" AND (LidID NOT IN (%s))", implode(", ", $lididwebmasters));
			$query = sprintf("DELETE FROM %sAdmin_login WHERE LastLogin <= DATE_ADD(CURDATE(), INTERVAL -1 WEEK) AND %s;", $table_prefix, $where);
			$result = fnQuery($query);
			if ($result > 0) {
				$mess .= sprintf("Er zijn %d logins verwijderd", $result);
				$query = "OPTIMIZE TABLE %sAdmin_login;";
				fnQuery($query);
			} else {
				$mess .= "Er zijn geen logins verwijderd";
			}
			$mess .= ", omdat ze geen toegang meer hebben.<br>\n";
		}
		$query = sprintf("DELETE FROM %sAdmin_login
					WHERE ((LastLogin IS NULL) OR LastLogin < '2012-01-01') AND Ingevoerd < DATE_ADD(CURDATE(), INTERVAL -%d DAY);", $table_prefix, db_param("login_bewaartijdnietgebruikt"));
		$result = fnQuery($query);
		if ($result > 0) {
			$mess .= sprintf("Er zijn %d logins verwijderd", $result);
		} else {
			$mess .= "Er zijn geen logins verwijderd";
		}
		$mess .= sprintf(", omdat ze niet binnen %d dagen na aanvragen gebruikt zijn.<br>\n", db_param("login_bewaartijdnietgebruikt"));
		
		$query = sprintf("UPDATE %1\$sAdmin_login SET HerinneringVervallenDiplomas=0 WHERE HerinneringVervallenDiplomas=1 AND 
					LidID NOT IN (SELECT Lid FROM %1\$sLidmaatschap WHERE ((Opgezegd IS NULL) OR Opgezegd >= CURDATE()));", $table_prefix);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess .= sprintf("Bij %d logins is de herinnering voor vervallen diploma uitgezet, omdat ze geen lid meer zijn. ", $result);
		}
		
		db_logboek("add", $mess, 2, 0, 1);
	
	} elseif ($actie == "lididbijlogin" and strlen($login) > 0) {
		$query = sprintf("SELECT IFNULL(MAX(LidID), 0) FROM %sAdmin_login WHERE Login='%s';", $table_prefix, $login);
		return db_scalar($query);
		
	} else {
		$mess = sprintf("onbekende actie '%s' in db_logins", $actie);
		debug($mess, 1, 0, 1);
	}
} # db_logins

function db_change_password($pw_new, $pw_old, $lidid) {
	global $table_prefix;
	
	$mess = "ok";
	if (strlen($pw_old) > 0) {
		$query = sprintf("SELECT Wachtwoord from %sAdmin_login WHERE LidID=%d;", $table_prefix, $lidid);
		$pw_hash = db_scalar($query);
		if (password_verify($pw_old, $pw_hash) == false) {
			$mess = "Oude wachtwoord is niet correct, wachtwoord is niet gewijzigd.";	
		}
	}
	if ($mess == "ok") {
		$query = sprintf("UPDATE %sAdmin_login SET Gewijzigd=SYSDATE(), GewijzigdDoor=%d, Wachtwoord='%s', ActivatieKey='' WHERE LidID=%d;",
					 $table_prefix, $_SESSION['lidid'], password_hash($pw_new, PASSWORD_DEFAULT), $lidid);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Het wachtwoord van de login van %s is gewijzigd.", db_naamlid($lidid));
			fnMaatwerkNaWijzigenWachtwoord($lidid, $pw_new);
		} else {
			$mess = "Wachtwoord is niet gewijzigd.";
		}
	}
	return $mess;
} # db_change_password

function db_onderdelen($actie="lijst", $type="") {
	global $fromlidond, $wherelidond;

	
	if ($actie == "aantal") {
		$sel = "COUNT(DISTINCT O.RecordID)";
	} else {
		$sel = "DISTINCT O.RecordID, CONCAT(LEFT(O.Naam, 30), ' (', O.Kode, ')') AS Oms, O.Naam";
	}
	
	$where = $wherelidond;
	if ($type == "bw") {
		$where = $where . " AND O.`Tonen in bewakingsadministratie`=1";
	} elseif (strlen($type) > 0) {
		$where = $where . sprintf(" AND O.`Type`='%s'", $type);
	}
	$query = sprintf("SELECT %s FROM %s WHERE %s ORDER BY O.Naam;", $sel, $fromlidond, $where);
	$result = fnQuery($query);
	
	if ($actie == "aantal") {
		return $result->fetchColumn();
	} else {	
		return $result->fetchAll();
	}
} # db_onderdelen

function db_naam_onderdeel($rid, $rin="Onbekend") {
	global $table_prefix;

	$query = sprintf("SELECT Naam FROM %sOnderdl WHERE RecordID=%d;", $table_prefix, $rid);
	$result = fnQuery($query);
	$rv = $result->fetchColumn();
	if (strlen($rv) == 0) {
		$rv = $rin;
	}
	return $rv;
}

function db_add_mailing_hist($lidid, $mid, $to_name, $to_addr, $message="", $mail=null, $success=1, $xtrachar="", $xtranum=0) {
	global $table_prefix;
	
	if (is_null($mail)) {
		$from_name = "";
		$from_addr = "";
		$subject = "";
		$cc_addr = "";
	} else {
		$from_name = $mail->FromName;
		$from_addr = $mail->From;
		$subject = $mail->Subject;
		$to_addr = $mail->ListAddresses("to");
		$cc_addr = $mail->ListAddresses("cc");
	}
	
	if (is_array($to_addr)) {
		$to_addr = implode(", ", $to_addr);
	}
		
	if (strlen($to_addr) > 100) {
		$to_addr = substr($to_addr, 0, 96) . " ...";
	}
	if (strlen($cc_addr) > 100) {
		$cc_addr = substr($cc_addr, 0, 96) . " ...";
	}

	if ($mid > 0) {
		$query = sprintf("SELECT * FROM %sMailing WHERE MailingID=%d;", $table_prefix, $mid);
		$result = fnQuery($query);
		$row = $result->fetch();
		if (strlen($from_name) == 0) {
			$from_name = $row->from_name;
		}
		if (strlen($from_addr) == 0) {
			$from_addr = $row->from_addr;
		}
		if (strlen($subject) == 0) {
			$subject = $row->subject;
		}
		if (strlen($message) == 0) {
			$message = $row->message;
		}
		if (strlen($row->to_name) > 0) {
			$to_name = $row->to_name;
		}
		if (strlen($cc_addr) == 0) {
			$cc_addr = $row->cc_addr;
		}
	}
	$subject = str_replace("\"", "'", $subject);
	$message = str_replace("\"", "'", $message);
	
	$query = sprintf("INSERT INTO %sMailing_hist SET
				LidID=%d, MailingID=%d, from_name=\"%s\", from_addr=\"%s\", to_name=\"%s\", subject=\"%s\", to_addr=\"%s\", cc_addr=\"%s\", message=\"%s\", send_by=%d, send_on=SYSDATE(), send_mt=%F, Successful=%d, Xtra_Char='%s', Xtra_Num=%d;", $table_prefix,
				$lidid, $mid, html_entity_decode($from_name), $from_addr, $to_name, html_entity_decode($subject), $to_addr, $cc_addr, html_entity_decode($message), $_SESSION['lidid'], microtime(true), $success, $xtrachar, $xtranum);
//	debug($query, 1, 0, 0);
	fnQuery($query);
		
	if ($success == 1 and $mid > 0) {
		$query = sprintf("UPDATE %sMailing SET sent_on=SYSDATE(), SentBy=%d WHERE MailingID=%d;", $table_prefix, $_SESSION['lidid'], $mid);
		fnQuery($query);
	}
}

function db_mailing($actie, $mid=0, $rid=0, $filter="", $subj="", $lidid=0, $email="", $aan="") {
	global $selectnaam, $selectzoeknaam, $table_prefix, $fdtlang;
	
	$mess = "";
	if ($actie == "exist") {
		$query = sprintf("SELECT COUNT(*) FROM %sMailing WHERE MailingID=%d AND (deleted_on IS NULL);", $table_prefix, $mid);
		$result = fnQuery($query);
		if ($result->fetchColumn() == 1) {
			return true;
		} else {
			return false;
		}
		
	} elseif ($actie == "lijst") {
		$xtra_sel = "";
		$orderby = "sent_on DESC";
		if ($filter == "Concepten" or $filter === "Verzonden") {
			if ($filter == "Verzonden") {
				$w = sprintf("(SELECT COUNT(*) FROM %sMailing_hist AS MH WHERE MH.MailingID=M.MailingID) > 0", $table_prefix);
				$xtra_sel = sprintf(", CONCAT(IF(M.SentBy=0, '', (SELECT %s FROM %sLid WHERE Nummer=M.SentBy)), ' & ', DATE_FORMAT(M.sent_on, %s)) AS `mrgLaatst verzonden`", $selectnaam, $table_prefix, $fdtlang);
			} else {
				$w = sprintf("(SELECT COUNT(*) FROM %sMailing_hist AS MH WHERE MH.MailingID=M.MailingID) = 0", $table_prefix);
			}
			$w .= " AND template=0 AND (deleted_on IS NULL)";
			if ($_SESSION['webmaster'] == 0) {
				$w .= sprintf(' AND (M.AddedBy=%1$d OR M.ChangedBy=%1$d OR M.confidential=0)', $_SESSION['lidid']);
			}
		} elseif ($filter == "Templates") {
			$w = "M.template=1 AND (deleted_on IS NULL)";
			$xtra_sel = sprintf(", CONCAT(IF(M.SentBy=0, '', (SELECT %s FROM %sLid WHERE Nummer=M.SentBy)), ' & ', DATE_FORMAT(M.sent_on, %s)) AS `mrgLaatst verzonden`", $selectnaam, $table_prefix, $fdtlang);
			$orderby = "M.subject, sent_on";
		} elseif ($filter == "Verzonden mails") {
			
		} elseif ($filter == "Prullenbak") {
			$w = "(NOT M.deleted_on IS NULL)";
			$orderby = "M.deleted_on DESC";
			$xtra_sel = sprintf(", CONCAT((SELECT %s FROM %sLid AS L WHERE Nummer=M.DeletedBy), ' & ', DATE_FORMAT(M.deleted_on, %s)) AS `mrgVerwijderd`", $selectnaam, $table_prefix, $fdtlang);
		} else {
			$w = "(M.deleted_on IS NULL)";
		}
		$query = sprintf("SELECT M.MailingID AS lnkMailingID, M.MailingID AS intNr, M.subject AS Onderwerp, M.from_name AS Van, M.to_name AS Aan,
					CONCAT(%1\$s, ' & ', DATE_FORMAT(M.changed_on, %6\$s)) AS `mrgLaatst gewijzigd`
					%2\$s,
					IF((SELECT COUNT(*) FROM %3\$sMailing_hist AS MH WHERE MH.MailingID=M.MailingID) > 0, M.MailingID, 0) AS llkHist
					FROM %3\$sMailing AS M LEFT JOIN %3\$sLid AS L ON M.ChangedBy = L.Nummer
					WHERE %4\$s ORDER BY %5\$s;", $selectnaam, $xtra_sel, $table_prefix, $w, $orderby, $fdtlang);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "rcpt") {
		$query = sprintf("SELECT R.LidID, IFNULL(L.Nummer, 0) AS Nummer, %1\$s AS Naam_lid, %2\$s AS Zoeknaam_lid, L.Adres, L.Postcode, 
					L.Woonplaats, L.Email, L.EmailVereniging, L.EmailOuders, L.GEBDATUM, R.to_address
					FROM %3\$sMailing_rcpt AS R LEFT OUTER JOIN %3\$sLid AS L ON R.LidID = L.Nummer
					WHERE MailingID=%4\$d
					ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, R.to_address, R.Ingevoerd;", $selectnaam, $selectzoeknaam, $table_prefix, $mid);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "rcptaantal") {
		$query = sprintf("SELECT COUNT(*) FROM %sMailing_rcpt WHERE MailingID=%d;", $table_prefix, $mid);
		return db_scalar($query);
		
	} elseif ($actie == "potrcpt") {
		$query = sprintf("SELECT L.Nummer, %1\$s AS Zoeknaam_lid FROM %2\$sLid AS L
					WHERE (L.Verwijderd IS NULL) AND (L.Overleden IS NULL) AND (LENGTH(L.Email) > 5 OR LENGTH(L.EmailVereniging) > 5 OR LENGTH(L.EmailOuders) > 5)
					AND (NOT EXISTS (SELECT LidID FROM %2\$sMailing_rcpt AS R WHERE R.LidID = L.Nummer AND R.MailingID=%3\$d))
					ORDER BY L.Achternaam, L.Roepnaam;", $selectzoeknaam, $table_prefix, $mid);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "aantalprullenbak") {
		$query = sprintf("SELECT COUNT(*) FROM %sMailing WHERE (NOT deleted_on IS NULL);", $table_prefix);
		return db_scalar($query);
		
	} elseif ($actie == "laatstehist" and $mid > 0) {
		$query = sprintf("SELECT send_on, send_by FROM %sMailing_hist WHERE Successful=1 AND MailingID=%d ORDER BY send_mt DESC LIMIT 1;", $table_prefix, $mid);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "laatstehist") {
		$query = sprintf("SELECT send_on FROM %sMailing_hist WHERE Successful=1 ORDER BY send_on DESC LIMIT 1;", $table_prefix, $mid);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "lastsendtime") {
		$query = sprintf("SELECT send_mt FROM %sMailing_hist WHERE Successful=1 ORDER BY send_mt DESC, RecordID DESC LIMIT 1;", $table_prefix, $mid);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "add") {
		if (strlen($email) < 5) {
			$email = "Webmaster " . db_param("naamwebsite");
		}
		$aan = str_replace("\"", "'", $aan);
		$subj = str_replace("\"", "'", $subj);
		$query = sprintf("INSERT INTO %sMailing (from_name, from_addr, to_name, subject, new_on, AddedBy)
								VALUES (\"%s\", \"%s\", \"%s\", \"%s\", SYSDATE(), %d);", $table_prefix, db_param("naamvereniging"), $email, $aan, $subj, $_SESSION['lidid']);
		$id = fnQuery($query);
		if ($id > 0) {
			$mess = sprintf("Mailing %d (%s) is door %s toegevoegd.", $id, $subj, $_SESSION['naamingelogde']);
		} else {
			$mess = "Het toevoegen van de mailing is mislukt.";
		}
		db_logboek("add", $mess, 4, 0, 0, $id);
		return $id;
		
	} elseif ($actie == "addrcpt" and $mid > 0) {
		$mess = "";
		if ($lidid > 0) {
			$query = sprintf("SELECT COUNT(*) FROM %sMailing_rcpt WHERE MailingID=%d AND LidID=%d;", $table_prefix, $mid, $lidid);
		} elseif (isValidMailAddress($email)) {
			$query = sprintf("SELECT COUNT(*) FROM %sMailing_rcpt WHERE MailingID=%d AND LOWER(to_address) LIKE '%s';", $table_prefix, $mid, strtolower($email));
		} else {
			$query = "";
		}
		if (strlen($query) > 0 and db_scalar($query) == 0) {
			if ($lidid > 0) {
				$rl = db_lid("record", "", $lidid);
				if (isValidMailAddress($rl->Email) or isValidMailAddress($rl->EmailOuders) or isValidMailAddress($rl->EmailVereniging)) {
					$query = sprintf("INSERT INTO %sMailing_rcpt SET MailingID=%d, LidID=%d, Ingevoerd=CURDATE();", $table_prefix, $mid, $lidid);
				} else {
					$mess = sprintf("%s is niet aan mailing %d toegevoegd, omdat er geen geldig emailadres bekend is.", $rl->NaamLid, $mid);
					db_logboek("add", $mess, 4, 0, 0, $lidid);
				}
				$r = $rl->NaamLid;
				$rl = null;
			} else {
				$query = sprintf("INSERT INTO %sMailing_rcpt SET MailingID=%d, to_address='%s', Ingevoerd=SYSDATE();", $table_prefix, $mid, $email);
				$r = $email;
			}
			if (strlen($mess) == 0 and fnQuery($query) > 0) {
				$mess = sprintf("%s is aan mailing %d toegevoegd.", $r, $mid);
			} else {
				$mess = "";
			}
		}
		return $mess;
	} elseif ($actie == "delrcpt" and $mid > 0) {
		$mess = "";
		if ($lidid > 0) {
			$query = sprintf("DELETE FROM %sMailing_rcpt WHERE MailingID=%d AND LidID=%d;", $table_prefix, $mid, $lidid);
			$r = db_naamlid($lidid);
		} elseif (strlen($email) > 0) {
			$query = sprintf("DELETE FROM %sMailing_rcpt WHERE MailingID=%d AND to_address='%s';", $table_prefix, $mid, $email);
			$r = $email;
		}
		if (fnQuery($query) > 0) {
			$mess = sprintf("%s is bij mailing %d verwijderd.", $r, $mid);
		}
		return $mess;
		
	} elseif ($actie == "opschonen") {
		if (db_param("mailing_bewaartijd") > 0) {
			$query = sprintf("DELETE FROM %sMailing WHERE (NOT deleted_on IS NULL)
						AND deleted_on < ADDDATE(CURDATE(), INTERVAL -%d MONTH);", $table_prefix, db_param("mailing_bewaartijd"));
			$result = fnQuery($query);
			if ($result > 0) {
				$mess = sprintf("Er zijn %d mailings uit de prullenbak definitief verwijderd. ", $result);
				fnQuery(sprintf("OPTIMIZE TABLE %sMailing;", $table_prefix));
			} else {
				$mess = "Bij het opschonen van de prullenbak zijn er geen mailings definitief verwijderd. ";
			}
			db_logboek("add", $mess, 4, 0, 1);
			
			$query = sprintf('DELETE FROM %1$sMailing_rcpt
						WHERE MailingID NOT IN (SELECT M.MailingID FROM %1$sMailing AS M);', $table_prefix);
			$result = fnQuery($query);
			if ($result > 0) {
				$mess = sprintf("Er zijn %d ontvangers van mailings verwijderd, omdat de mailings niet meer bestaan.", $result);
				db_logboek("add", $mess, 4, 0, 1);
				$query = sprintf("OPTIMIZE TABLE %sMailing_rcpt;", $table_prefix);
				fnQuery($query);
			}
		}

	} elseif ($mid > 0) {
		$query = sprintf("SELECT * FROM %sMailing WHERE MailingID=%d LIMIT 1;", $table_prefix, $mid);
		$result = fnQuery($query);
		return $result->fetch();
	}
	
	return;
} # db_mailing

function db_mailing_hist($actie, $lidid=0, $mhid=0, $mid=0) {
	global $table_prefix, $selectnaam;
	
	if ($lidid > 0) {
		$lw = sprintf("AND LidID=%d", $lidid);
	} elseif ($_SESSION['webmaster'] == 1) {
		$lw = "";
	} else {
		$lw = sprintf("AND (LidID=%1\$d OR send_by=%1\$d)", $lidid);
	}
	
	if ($actie == "lijst" or $actie == "sqllijst") {
		if ($mid > 0) {
			$query = sprintf("SELECT M.subject AS ndOnderwerp, IF(MH.Successful=1, MH.send_on, 'Niet verzonden') AS dtsVerzonden, IF(MH.LidID=0, MH.to_name, %1\$s) AS Aan,
						to_addr AS `E-mail`, MH.cc_addr AS `CC`, L.Woonplaats, MH.RecordID AS lnkRecordID
						FROM (%2\$sMailing_hist AS MH LEFT OUTER JOIN %2\$sLid AS L ON MH.LidID = L.Nummer) INNER JOIN %2\$sMailing AS M ON MH.MailingID = M.MailingID
						WHERE MH.MailingID=%3\$d
						ORDER BY MH.RecordID DESC;", $selectnaam, $table_prefix, $mid);
		} else {
			$query = sprintf("SELECT send_on AS dttVerzonden,
						IF(LENGTH(from_name)<3, from_addr, from_name) AS Van,
						CONCAT(to_name, ' (', to_addr, ')') AS Aan,
						subject AS Onderwerp,
						RecordID AS lnkRecordID
						FROM %sMailing_hist
						WHERE Successful=1 %s
						ORDER BY RecordID DESC LIMIT 1000;", $table_prefix, $lw);
		}

		if ($actie == "sqllijst") {
			return $query;
		} else {
			return fnQuery($query);
		}
		
	} elseif ($actie === "record") {
		$query = sprintf("SELECT send_on, from_addr, from_name, to_name, to_addr, cc_addr, subject, message, Successful, send_by
					FROM %sMailing_hist WHERE RecordID=%d LIMIT 1;", $table_prefix, $mhid);
		$result = fnQuery($query);
		return $result->fetch();
	
	} elseif ($actie == "lidbijmail") {
		$query = sprintf("SELECT IFNULL(MAX(LidID), 0) FROM %sMailing_hist WHERE RecordID=%d;", $table_prefix, $mhid);
		return db_scalar($query);
		
	} elseif ($actie == "aantal24uur") {
		$query = sprintf("SELECT COUNT(*) FROM %sMailing_hist WHERE Successful=1 AND send_on > DATE_SUB(NOW(), INTERVAL 24 HOUR);", $table_prefix);
		return db_scalar($query);
		
	} else {
		db_logboek("add", "onbekende actie", 4, 0, 0);
	}
				 
} # db_mailinghist

function db_mutatie($jaar, $gbr="*", $kpl="*") {
	global $table_prefix;
	
	$sel_kpl = "";
	$where = sprintf("M.BoekjaarID=%d", $jaar);
	if ($gbr != "*" and strlen($gbr) > 0) {
		$where .= sprintf(" AND M.GBR='%s'", $gbr);
	}
	if ($kpl != "*" and strlen($kpl) > 0 and is_numeric($kpl)) {
		$where .= sprintf(" AND M.KostenplaatsID=%d", $kpl);
	} elseif ($_SESSION['aantalkostenplaatsen'] > 0) {
		$sel_kpl = ", KPL.Kode AS Kostenplaats";
	}

	$query = sprintf("SELECT J.Kode AS Jaar, CONCAT(GBR.Kode, ' - ', GBR.OMSCHRIJV) AS Grootboekrekening%1\$s, Datum AS dteDatum, M.OMSCHRIJV AS Omschrijving, Debet-Credit AS curBedrag
							FROM ((%2\$sMutatie AS M LEFT OUTER JOIN %2\$sKostenplaats AS KPL ON M.KostenplaatsID = KPL.RecordID) INNER JOIN %2\$sGBR AS GBR ON M.GBR = GBR.Kode) INNER JOIN %2\$sBoekjaar AS J ON M.BoekjaarID = J.RecordID WHERE %3\$s
							ORDER BY J.Kode, GBR.Kode, KPL.Kode, Datum;", $sel_kpl, $table_prefix, $where);
	$result = fnQuery($query);
	return $result->fetchAll();
}

function db_boekjaar() {
	global $table_prefix;
	
	$query = sprintf("SELECT RecordID, Kode AS Jaar FROM %sBoekjaar ORDER BY Begindatum DESC;", $table_prefix);
	$result = fnQuery($query);
	return $result->fetchAll();
}

function db_kostenplaats($actie) {
	global $table_prefix;

	if ($actie == "distinct") {
		$query = sprintf("SELECT DISTINCT KPL.Kode AS Kode
					FROM %1\$sMutatie AS M INNER JOIN %1\$sKostenplaats AS KPL ON M.KostenplaatsID = KPL.RecordID
					WHERE M.KostenplaatsID > 0 ORDER BY KPL.Kode;", $table_prefix);
	}
	$result = fnQuery($query);
	return $result->fetchAll();
}

function db_logboek($actie, $oms="", $type=-1, $lidid=0, $tm=0, $referid=0, $ipfilter="") {
	global $table_prefix, $selectnaam, $selectzoeknaam;
	
	if ($type >= 0 and strlen($ipfilter) > 5) {
		$w = sprintf("TypeActiviteit=%d AND `IP_adres`='%s'", $type, $ipfilter);
	} elseif ($type >= 0) {
		$w = sprintf("TypeActiviteit=%d", $type);
	} elseif (strlen($ipfilter) > 5) {
		$w = sprintf("`IP_adres`='%s'", $ipfilter);
	} else {
		$w = "TypeActiviteit >= 0";
	}
	$mess = "";
	$script = $_SERVER['PHP_SELF'];
	if (strlen($_SERVER['QUERY_STRING']) > 0) {
		$script .= "?" . $_SERVER['QUERY_STRING'];
	}
	
	if ($lidid > 0 and $referid == 0) {
		$referid = $lidid;
	}
	
	if ($actie == "add") {
		if (strlen($oms) < 5) {
			$tm = 0;
		}
		$bt = debug_backtrace(2, 3);
		$f = "";
		if (isset($bt[1]['function'])) {
			$f = $bt[1]['function'];
			if (isset($bt[2]['function'])) {
				$f = $bt[2]['function'] . "=>" . $f;
			}
			$f = substr($f, 0, 40);
		}
		$query = sprintf("INSERT INTO %sAdmin_activiteit SET
				DatumTijd=SYSDATE(),
				LidID=%d,
				IP_adres='%s',
				USER_AGENT='%s',
				Omschrijving=\"%s\",
				ReferID=%d,
				TypeActiviteit=%d,
				Script=\"%s\",
				Getoond=%d,
				RefFunction='%s';", $table_prefix, $_SESSION['lidid'], $_SERVER['REMOTE_ADDR'], $_SERVER['HTTP_USER_AGENT'], $oms, $referid, $type, $script, $tm, $f);
		fnQuery($query);
		if ($tm == 1) {
			printf("<p class='mededeling'>%s</p>\n", $oms);
		} elseif ($tm == 2) {
			printf("<script>alert(\"%s\");</script>\n", str_replace("\"", "'", $oms));
		}
		usleep(5);
		
	} elseif ($actie == "opschonen") {
		if (db_param("logboek_bewaartijd") > 0) {
			$query = sprintf("DELETE FROM %sAdmin_activiteit WHERE TypeActiviteit<>1 AND DatumTijd < DATE_ADD(CURDATE(), INTERVAL -%d MONTH);", $table_prefix, db_param("logboek_bewaartijd"));
			$aant = fnQuery($query);
		} else {
			$aant = 0;
		}
		if (db_param("logboek_bewaartijdinloggen") > 0) {
			$query = sprintf("DELETE FROM %sAdmin_activiteit WHERE TypeActiviteit=1 AND DatumTijd < DATE_ADD(CURDATE(), INTERVAL -%d MONTH);", $table_prefix, db_param("logboek_bewaartijdinloggen"));
			$aant += fnQuery($query);
		}
		if ($aant > 0) {
			$mess = sprintf("Bij het opschonen zijn %d records uit het logboek verwijderd.", $aant);
			$query = sprintf("OPTIMIZE TABLE %sAdmin_activiteit;", $table_prefix);
			fnQuery($query);
		} else {
			$mess = "Bij het opschonen van het logboek zijn geen records verwijderd.";
		}
		db_logboek("add", $mess, 2, 0, 1);
		return $mess;
		
	} elseif ($actie == "lijst") {
		if ($type == -1) {
			$s = ", TypeActiviteit AS `Type`";
		} else {
			$s = "";
		}
		if (strlen($ipfilter) <= 5 and $_SESSION['webmaster'] == 1) {
			$s .= ", A.IP_adres";
		}
		if ($_SESSION['webmaster'] == 1) {
			$s .= ", Script, RefFunction AS Functie";
		}
		if ($lidid > 0) {
			$w .= sprintf(" AND (LidID=%1\$d OR (TypeActiviteit IN (1, 6, 8, 10, 12) AND ReferID=%1\$d)) AND TypeActiviteit < 98", $lidid);
		}
		$s .= sprintf(", %s AS `Ingelogd Lid`", $selectnaam);
	
		$query = sprintf("SELECT A.DatumTijd as `dtsDatum en tijd`, Omschrijving%1\$s
								FROM %2\$sAdmin_activiteit AS A LEFT OUTER JOIN %2\$sLid AS L ON A.LidID = L.Nummer
								WHERE %3\$s
								ORDER BY A.RecordID DESC LIMIT 750;", $s, $table_prefix, $w);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "lidlijst") {
		$query = sprintf("SELECT DISTINCT LidID, %1\$s AS Naam
								FROM %2\$sAdmin_activiteit AS A INNER JOIN %2\$sLid AS L ON A.LidID = L.Nummer
								WHERE %3\$s
								ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", $selectzoeknaam, $table_prefix, $w);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "iplijst") {
		$query = sprintf("SELECT DISTINCT IP_adres FROM %sAdmin_activiteit WHERE %s ORDER BY IP_adres;", $table_prefix, $w);
		$result = fnQuery($query);
		return $result->fetchAll();
	
	} elseif ($actie == "aantal") {
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_activiteit WHERE %s;", $table_prefix, $w);
		return db_scalar($query);
	
	} elseif ($actie == "iplogincontrole") {
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_activiteit WHERE TypeActiviteit=97 AND IP_adres='%s' AND DatumTijd >= ", $table_prefix, $_SERVER['HTTP_USER_AGENT']);
		if (db_param("login_autounlock") > 0) {
			$query .= sprintf("DATE_ADD(SYSDATE(), INTERVAL -%d MINUTE);", db_param("login_autounlock"));
		} else {
			$query .= "DATE_ADD(SYSDATE(), INTERVAL -1 HOUR);";
		}
		return db_scalar($query);
	}
} # db_logboek

function db_interface($actie="lijst", $query="", $lidid=0, $nip=0) {
	global $selectnaam, $table_prefix;
	
	if ($lidid == 0) {
		$lidid = $_SESSION['lidid'];
	}
	
	if ($actie == "add" and strlen($query) > 1) {
		if (strlen($table_prefix) > 1) {
			$query = str_replace($table_prefix, "", $query);
		}
		$msa_sql = str_replace("SYSDATE()", "#" . strftime("%m/%d/%Y %H:%M") . "#", str_replace("\"", "'", $query));
		$msa_sql = str_replace("CURDATE()", "#" . strftime("%m/%d/%Y") . "#", $msa_sql);
		if ($nip == 1) {
			$ip = "";
		} else {
			$ip = $_SERVER['REMOTE_ADDR'];
		}
		$query = sprintf("INSERT INTO %sAdmin_interface SET `IP-adres`='%s', LidID=%d, `SQL-statement`=\"%s\", Ingevoerd=SYSDATE();", $table_prefix, $ip, $lidid, $msa_sql);
		fnQuery($query, 0);
		
	} elseif ($actie == "afmelden") {
		$query = sprintf("UPDATE %sAdmin_interface SET Afgemeld=SYSDATE() WHERE (Afgemeld < '2011-01-01' OR (Afgemeld IS NULL));", $table_prefix);
		$result = fnQuery($query);
		if ($result > 0) {
			fnQuery(sprintf("OPTIMIZE TABLE %sAdmin_interface;", $table_prefix));
			$mess = sprintf("Er zijn %d online-wijzigingen afgemeld.", $result);
		} else {
			$mess = "Er zijn geen online-wijzigingen afgemeld.";
		}
		db_logboek("add", $mess, 2);
		return $mess;
		
	} elseif ($actie == "aantalopenstaand") {
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_interface WHERE (Afgemeld < '2010-01-01' OR (Afgemeld IS NULL));", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "lijst") {
		$query = sprintf("SELECT %1\$s AS `Naam lid`, I.Ingevoerd, `SQL-statement` AS `SQL`"
				 . " FROM %2\$sAdmin_interface AS I LEFT OUTER JOIN %2\$sLid AS L ON I.LidID = L.Nummer"
				 . " WHERE (I.Afgemeld IS NULL)"
				 . " ORDER BY I.Ingevoerd, I.RecordID;", $selectnaam, $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();
	}
} # db_interface

function db_diploma($actie, $did=0, $td="*") {
	global $table_prefix;
	
	$mess = "";
	if ($actie == "muteerlijst") {
		if ($td != "*" and strlen($td) > 0) {
			$filter = sprintf(" AND DP.`Type`='%s'", $td);
		} else {
			$filter = "";
		}
		$query = sprintf("SELECT * FROM %sDiploma AS DP
					WHERE ((DP.Vervallen IS NULL) OR DP.Vervallen > CURDATE()) %s
					ORDER BY DP.Volgnr, DP.Kode;", $table_prefix, $filter);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "zelfservice_lijst") {
		$query = sprintf("SELECT * FROM %sDiploma AS DP
					WHERE DP.Zelfservice=1 AND DP.AantalBeoordelingen < 2 AND ((DP.Vervallen IS NULL) OR DP.Vervallen > CURDATE())
					ORDER BY DP.Volgnr, DP.Kode;", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "kode") {
		$query = sprintf("SELECT Kode FROM %sDiploma WHERE RecordID=%d;", $table_prefix, $did);
		return db_scalar($query);
	} elseif ($actie == "naam") {
		$query = sprintf("SELECT Naam FROM %sDiploma WHERE RecordID=%d;", $table_prefix, $did);
		return db_scalar($query);
	} elseif ($actie == "opschonen") {
		/* Tijdelijk opgeschort.
		$query = sprintf("DELETE FROM %sDiploma WHERE (NOT Vervallen IS NULL) AND Vervallen < CURDATE();", $table_prefix);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Er zijn %d diploma's verwijderd, omdat ze vervallen zijn. ", $result);
			db_logboek("add", $mess, 2, 0, 1);
		}
		*/
	}
} # db_diploma

function db_liddipl($actie, $lidid=0, $rid=0, $did=0, $exdat="", $vervdat="", $dipnr="", $xtrafilter="") {
	global $table_prefix, $fdaccess, $fdsql;
	
	$mess = "";
	if (strlen($exdat) >= 4 and !is_numeric(substr($exdat, -4))) {
		$exdat = trim($exdat) . " " . date("Y");
	}
	$exdat = change_month_to_uk($exdat);
	$vervdat = change_month_to_uk($vervdat);
	
	if (strlen($exdat) > 4 and strtotime($exdat) !== FALSE) {
		$dwexd = strftime($fdsql, strtotime($exdat));
		$dwaexd = sprintf("#%s#", strftime($fdaccess, strtotime($exdat)));
	} else {
		$dwexd = "";
	}
	
	if (strlen($vervdat) > 4 and strtotime($vervdat) !== FALSE and strtotime($vervdat) > strtotime($dwexd)) {
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
		if ($dwexd > date("Y-m-d")) {
			$mess = sprintf("De examendatum mag niet in de toekomst liggen. Diploma '%s' is bij %s niet toegevoegd.", db_diploma("naam", $did), db_naamlid($lidid));
		} else {
			$query = sprintf("INSERT INTO %sLiddipl (Lid, DiplomaID, DatumBehaald, LaatsteBeoordeling, LicentieVervallenPer, Diplomanummer, Ingevoerd, Gewijzigd)
									VALUES (%d, %d, '%s', 1, %s, '%s', CURDATE(), SYSDATE());", $table_prefix, $lidid, $did, $dwexd, $dwverv, $dipnr);
			$result = fnQuery($query);
			if ($result > 0) {
				$query = sprintf("INSERT INTO Liddipl (Lid, DiplomaID, DatumBehaald, LicentieVervallenPer, Diplomanummer, Ingevoerd, Gewijzigd) VALUES (%d, %d, %s, %s, '%s', CURDATE(), SYSDATE());", $lidid, $did, $dwaexd, $dwaverv, $dipnr);
				db_interface("add", $query, $lidid);
				$mess = sprintf("Diploma '%s' is met examendatum '%s' bij %s toegevoegd.", db_diploma("naam", $did), $dwexd, db_naamlid($lidid));
			} else {
				$mess = sprintf("Diploma '%s' is bij %s niet toegevoegd.", db_diploma("naam", $did), db_naamlid($lidid));
			}
		}
		db_logboek("add", $mess, 12, $lidid);
		return $mess;
		
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
			$query = sprintf("UPDATE Liddipl SET LicentieVervallenPer=%s, Diplomanummer='%s', Gewijzigd=SYSDATE() WHERE Lid=%d AND DiplomaID=%d AND DatumBehaald=%s;", $dwaverv, $dipnr, $rowld->Lid, $rowld->DiplomaID, $dwaexd);
			db_interface("add", $query, $rowld->Lid);
			$mess = sprintf("Diploma '%s' is gewijzigd bij %s", db_diploma("naam", $rowld->DiplomaID), db_naamlid($rowld->Lid));
			db_logboek("add", $mess, 12, $rowld->Lid);
		}
		return $mess;
		
	} elseif ($actie == "delete" and $lidid > 0 and $did > 0 and strlen($dwexd) == 10) {
		$query = sprintf("DELETE FROM %sLiddipl WHERE Lid=%d AND DiplomaID=%d AND DatumBehaald='%s';", $table_prefix, $lidid, $did, $dwexd);
		$result = fnQuery($query);
		if ($result > 0) {
			$query = sprintf("DELETE FROM Liddipl WHERE Lid=%d AND DiplomaID=%d AND DatumBehaald=%s;", $lidid, $did, $dwaexd);
			db_interface("add", $query, $lidid);
			$mess = sprintf("%s is bij %s verwijderd.", db_diploma("naam", $did), db_naamlid($lidid));
		} else {
			$mess = sprintf("%s is bij %s niet gevonden en dus niet verwijderd.", db_diploma("naam", $did), db_naamlid($lidid));
		}
		db_logboek("add", $mess, 12, $lidid);
		return $mess;
		
	} elseif ($actie == "lidgegevens" and $lidid > 0) {
		if (strlen($xtrafilter) > 1) {
			$xtrafilter = "AND " . $xtrafilter;
		}
		$query = sprintf("SELECT D.Kode AS Code,"
				 . " D.Naam AS Diploma,"
				 . " LD.DatumBehaald AS dteDatum,"
				 . " LD.Diplomanummer AS Gegevens,"
				 . " IFNULL(LD.LicentieVervallenPer, IF(D.GELDIGH>0, DATE_ADD(LD.DatumBehaald, INTERVAL D.GELDIGH MONTH), IF((NOT D.Vervallen IS NULL), D.Vervallen, null))) AS `dteVervalt per`"
				 . " FROM %1\$sLiddipl AS LD INNER JOIN %1\$sDiploma AS D ON LD.DiplomaID = D.RecordID"
				 . " WHERE LD.DatumBehaald <= CURDATE() AND LD.LaatsteBeoordeling=1 AND ((D.Vervallen IS NULL) OR D.Vervallen > CURDATE()) AND LD.Lid=%2\$d %3\$s"
				 . " ORDER BY D.Volgnr, D.Kode, LD.DatumBehaald DESC;", $table_prefix, $lidid, $xtrafilter);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "vervallenbinnenkort" and $lidid > 0) {
		$termijn = db_param("termijnvervallendiplomasmelden");
	
		$query = sprintf("SELECT D.Kode AS Code,"
				 . " D.Naam AS Diploma,"
				 . " LD.DatumBehaald AS DatumBehaald,"
				 . " LD.Diplomanummer,"
				 . " IFNULL(LD.LicentieVervallenPer, IF(D.GELDIGH>0, DATE_ADD(LD.DatumBehaald, INTERVAL D.GELDIGH MONTH), NULL)) AS VervaltPer"
				 . " FROM %1\$sLiddipl AS LD INNER JOIN %1\$sDiploma AS D ON LD.DiplomaID=D.RecordID"
				 . " WHERE D.Zelfservice=1 AND ((D.Vervallen IS NULL) OR D.Vervallen > CURDATE()) AND ((NOT LD.LicentieVervallenPer IS NULL) OR D.GELDIGH > 0) AND LD.Lid=%2\$d"
				 . " AND IFNULL(LD.LicentieVervallenPer, DATE_ADD(LD.DatumBehaald, INTERVAL D.GELDIGH MONTH)) < DATE_ADD(CURDATE(), INTERVAL %3\$d MONTH) AND LD.LicentieVervallenPer > DATE_ADD(CURDATE(), INTERVAL -%3\$d MONTH)"
				 . " ORDER BY D.Volgnr, D.Kode, LD.DatumBehaald DESC;", $table_prefix, $lidid, $termijn);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "diplomabehaald" and $lidid > 0 and $did > 0) {
		$query = sprintf("SELECT * FROM %sLiddipl WHERE Lid=%d AND DiplomaID=%d ORDER BY DatumBehaald DESC;", $table_prefix, $lidid, $did);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "diplomalid" and $lidid > 0 and $did > 0) {
		$query = sprintf("SELECT * FROM %sLiddipl WHERE Lid=%d AND DiplomaID=%d ORDER BY DatumBehaald;", $table_prefix, $lidid, $did);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "opschonen") {
		$query = sprintf("DELETE FROM %1\$sLiddipl WHERE DiplomaID NOT IN (SELECT RecordID FROM %1\$sDiploma);", $table_prefix);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Er zijn %d diploma's bij leden verwijderd, omdat deze diploma's vervallen zijn. ", $result);
			fnQuery(sprintf("OPTIMIZE TABLE %sLiddipl;", $table_prefix));
			db_logboek("add", $mess, 2, 0, 1);
		}
		$query = sprintf("DELETE FROM %sLiddipl WHERE DatumBehaald > CURDATE();", $table_prefix);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Er zijn %d diploma's bij leden verwijderd, omdat de examendatum in de toekomst ligt. ", $result);
			fnQuery(sprintf("OPTIMIZE TABLE %sLiddipl;", $table_prefix));
			db_logboek("add", $mess, 2, 0, 1);
		}
	}
} # db_liddipl

function db_evenement($actie, $lidid=0, $eventid=0, $opm="", $stat="", $functie="") {
	global $table_prefix, $selectnaam, $selectzoeknaam;
	
	$sqlaantdln = sprintf("SELECT COUNT(*) FROM %sEvenement_Deelnemer AS ED WHERE ED.EvenementID = E.RecordID AND Status='B'", $table_prefix);

	$xtra_where = "";
	$mess = "";
	$lg = db_lidond("lidgroepen");
	if ($actie == "oms") {
		$query = sprintf("SELECT Omschrijving FROM %sEvenement WHERE RecordID=%d;", $table_prefix, $eventid);
		return db_scalar($query);
		
	} elseif ($actie == "komende") {
		$query = sprintf("SELECT E.Datum, E.Omschrijving
								FROM %sEvenement AS E
								WHERE E.Datum >= CURDATE() AND E.Datum < DATE_ADD(CURDATE(), INTERVAL 2 MONTH) AND E.VerwijderdOp < '2012-01-01' AND E.ZichtbaarVoor IN %s
								ORDER BY E.Datum LIMIT 5;", $table_prefix, $lg);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "open") {
		$query = sprintf("SELECT E.*, ET.Omschrijving AS TypeEvenement
								FROM %1\$sEvenement AS E INNER JOIN %1\$sEvenement_Type AS ET ON E.TypeEvenement = ET.RecordID
								WHERE E.Datum >= CURDATE() AND E.VerwijderdOp < '2012-01-01' AND InschrijvingOpen=1
								AND E.BeperkTotGroep IN %2\$s
								ORDER BY E.Datum;", $table_prefix, $lg);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "overzicht" or $actie == "overzichtbeheer") {
		if ($actie == "overzichtbeheer") {
			$where = "E.Datum >= DATE_ADD(CURDATE(), INTERVAL -3 MONTH) AND E.VerwijderdOp < '2012-01-01'";
			$order = "E.Datum";
		} else {
			$where = sprintf("E.Datum >= DATE_ADD(CURDATE(), INTERVAL -3 DAY) AND E.VerwijderdOp < '2012-01-01' AND E.ZichtbaarVoor IN %s", $lg);
			$order = "ET.Soort, E.Datum";
		}
		$query = sprintf("SELECT E.RecordID AS lnkNummer, E.Datum AS dteDatum, DATE_FORMAT(E.Datum, '%%H:%%i') AS Starttijd, E.Omschrijving,
								E.Email AS `emlEmail`, E.Verzameltijd, E.Eindtijd,
								CASE E.InschrijvingOpen WHEN 0 THEN 'Nee' ELSE 'Ja' END AS `Ins. open?`,
								(%1\$s) AS `Aantal dln`,
								ET.Omschrijving AS `Type`, ET.Soort
								FROM %2\$sEvenement AS E LEFT OUTER JOIN %2\$sEvenement_Type AS ET ON E.TypeEvenement = ET.RecordID
								WHERE %3\$s
								ORDER BY %4\$s;", $sqlaantdln, $table_prefix, $where, $order);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "add") {
			$query = sprintf("INSERT INTO %sEvenement SET Ingevoerd=CURDATE();", $table_prefix);
			$id = fnQuery($query);
			$mess = sprintf("Evenement %d is toegevoegd.", $id);
			db_logboek("add", $mess, 7, 0, 0, $id);
			return $id;
		
	} elseif ($actie == "inschrijving" and $lidid > 0 and $eventid > 0) {
		$query = sprintf("SELECT ED.RecordID, ED.Opmerking, ED.Status
								FROM %1\$sEvenement AS E INNER JOIN %1\$sEvenement_Deelnemer AS ED ON E.RecordID = ED.EvenementID
								WHERE ED.LidID=%2\$d AND ED.EvenementID=%3\$d;", $table_prefix, $lidid, $eventid);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "record" and $eventid > 0) {
		$query = sprintf("SELECT E.*, %1\$s AS GewijzigdDoorNaam, (%2\$s) AS AantDln
								FROM %3\$sEvenement AS E LEFT OUTER JOIN %3\$sLid AS L ON E.GewijzigdDoor = L.Nummer
								WHERE E.RecordID=%4\$d;", $selectnaam, $sqlaantdln, $table_prefix, $eventid);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "types") {
		$query = sprintf("SELECT RecordID, Omschrijving, Soort FROM %sEvenement_Type ORDER BY Omschrijving;", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "deelnemers") {
		if (strlen($stat) == 1) {
			$xtra_where = sprintf("AND ED.Status='%s'", $stat);
		} elseif (strlen($stat) > 1) {
			$xtra_where = sprintf("AND ED.Status IN (%s)", $stat);
		}
		$query = sprintf("SELECT ED.RecordID, %1\$s AS NaamDeelnemer, ED.Status, ED.Opmerking, ED.Functie, ED.LidID, L.Geslacht, ED.Ingevoerd
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
		$mess = sprintf("%s is met status '%s' aan evenement %d (%s) toegevoegd.", db_naamlid($lidid), $stat, $eventid, db_evenement("oms", 0, $eventid));
		db_logboek("add", $mess, 7, 0, 0, $eventid);
		
	} elseif ($actie == "updatedeelnemer" and $lidid > 0 and $eventid > 0) {
		$query = sprintf("UPDATE %1\$sEvenement_Deelnemer SET Opmerking='%2\$s', Status='%3\$s', Functie='%4\$s', GewijzigdDoor=%5\$d
					WHERE LidID=%6\$d AND EvenementID=%7\$d AND (Opmerking <> '%2\$s' OR Status <> '%3\$s' OR Functie <> '%4\$s');", $table_prefix, $opm, $stat, $functie, $_SESSION['lidid'], $lidid, $eventid);
		$result = fnQuery($query);
		return $result;
		
	} elseif ($actie == "deletedeelnemer" and $lidid > 0 and $eventid > 0) {
		$query = sprintf("DELETE FROM %sEvenement_Deelnemer WHERE LidID=%d AND EvenementID=%d;", $table_prefix, $lidid, $eventid);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("%s is bij evenement %d (%s) verwijderd.", db_naamlid($lidid), $eventid, db_evenement("oms", 0, $eventid));
			db_logboek("add", $mess, 7, 0, 0, $eventid);
		}
		
	} elseif ($actie == "opschonen") {
		$query = sprintf("DELETE FROM %sEvenement WHERE VerwijderdOp >= '2012-01-01' AND VerwijderdOp <= DATE_ADD(CURDATE(), INTERVAL -6 MONTH);", $table_prefix);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Er zijn %d evenementen definitief verwijderd. ", $result);
			fnQuery(sprintf("OPTIMIZE TABLE %sEvenement;", $table_prefix));
		} else {
			$mess = "Er zijn geen evenementen bij het opschonen verwijderd. ";
		}
		$query = sprintf("DELETE FROM %1\$sEvenement_Deelnemer WHERE EvenementID NOT IN (SELECT RecordID FROM %1\$sEvenement);", $table_prefix);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess .= sprintf("Er zijn %d deelnemers aan evenementen definitief verwijderd, omdat de evenementen niet meer bestaan.", $result);
			fnQuery(sprintf("OPTIMIZE TABLE %sEvenement_Deelnemer;", $table_prefix));
		}
		db_logboek("add", $mess, 2);
		return $mess;
	} else {
		debug(sprintf("Onbekende actie '%s' in db_evenement.", $actie), 1, 0, 1);
	}
}

function db_artikel($actie, $artikelid=0, $lidid=0) {
	global $table_prefix;

	$mess = "";
	$lg = db_lidond("lidgroepen");
	if ($actie == "overzichtbeheer") {
		$query = sprintf("SELECT *, CONCAT(Omschrijving, IF(LENGTH(Maat)>0, CONCAT(' (', Maat, ')'), '')) AS OmsMaat
					FROM %sWS_Artikel ORDER BY Code, Omschrijving, RecordID;", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "select") {
		$query = sprintf("SELECT *, CONCAT(Omschrijving, IF(LENGTH(Maat)>0, CONCAT(' (', Maat, ')'), '')) AS OmsMaat
					FROM %1\$sWS_Artikel AS A
					WHERE (A.RecordID NOT IN (SELECT Artikel FROM %1\$sWS_Orderregel WHERE Lid=%2\$d))
					AND A.VervallenPer >= CURDATE()
					ORDER BY Code, Omschrijving, RecordID;", $table_prefix, $lidid);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "code" and $artikelid > 0) {
		$query = sprintf("SELECT IF(LENGTH(Code)=0, RecordID, Code) FROM %sWS_Artikel WHERE RecordID=%d;", $table_prefix, $artikelid);
		return db_scalar($query);
		
	} elseif ($actie == "artikelbeschikbaar") {
		$query = sprintf("SELECT * FROM %sWS_Artikel
					WHERE BeschikbaarTot >= CURDATE() AND (BeperkTotGroep=0 OR BeperkTotGroep IN %s)
					ORDER BY Code, Omschrijving, RecordID;", $table_prefix, $lg);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "add") {
		$query = sprintf("INSERT INTO %sWS_Artikel SET Ingevoerd=CURDATE(), IngevoerdDoor=%d;", $table_prefix, $_SESSION['lidid']);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Artikel %d is toegevoegd.", $result);
			db_logboek("add", $mess, 10, 0, 0, $result);
		}

	} elseif ($actie == "delete" and $artikelid > 0) {
		if (db_artikel("ingebruik", $artikelid) > 0) {
			$mess = sprintf("Artikel %s is niet verwijderd, omdat het nog in gebruik is.", db_artikel("code", $artikelid));
		} else {
			$query = sprintf("DELETE FROM %sWS_Artikel WHERE RecordID=%d;", $table_prefix, $artikelid);
			$result = fnQuery($query);
			if ($result > 0) {
				$mess = sprintf("Artikel %s is verwijderd.", db_artikel("code", $artikelid));
			} else {
				$mess = sprintf("Artikel %s is niet verwijderd.", db_artikel("code", $artikelid));
			}
		}
		db_logboek("add", $mess, 10, 0, 0, $artikelid);
		
	} elseif ($actie == "prijs" and $artikelid > 0) {
		$query = sprintf("SELECT Verkoopprijs FROM %sWS_Artikel WHERE RecordID=%d;", $table_prefix, $artikelid);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "ingebruik") {
		$query = sprintf("SELECT COUNT(*) FROM %sWS_Orderregel WHERE Artikel=%d;", $table_prefix, $artikelid);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "opschonen") {
		$query = sprintf("DELETE FROM %1\$sWS_Artikel WHERE BeschikbaarTot < CURDATE() AND RecordID NOT IN (SELECT Artikel FROM %1\$sWS_Orderregel);", $table_prefix);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Er zijn bij het opschonen %d artikelen verwijderd.", $result);
		} else {
			$mess = "Er zijn bij het opschonen geen artikelen verwijderd.";
		}
		db_logboek("add", $mess, 10);
		return $mess;
	}
}

function db_orderregel($actie, $lidid=0, $artikelid=0, $aantalbesteld=0, $filter="", $recordid=0, $ordernr=0) {
	global $table_prefix, $selectnaam;
	
	$mess = "";
	if ($actie == "bestelling" and $lidid > 0 and $artikelid > 0) {
		$query = sprintf("SELECT * FROM %sWS_Orderregel WHERE Lid=%d AND Artikel=%d;", $table_prefix, $lidid, $artikelid);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "bevestiging" and $lidid > 0) {
		if (strlen($filter) > 0) {
			$filter = " AND " . $filter;
		}
		$query = sprintf("SELECT ORD.Ordernr, ORD.AantalBesteld, ORD.Opmerking, ORD.PrijsPerStuk * ORD.AantalBesteld AS Bedrag,
					%1\$s AS NaamLid, L.Roepnaam, L.Adres, L.Postcode, L.Woonplaats, L.Email, L.GEBDATUM,
					A.Code, A.Omschrijving, A.Maat
					FROM (%2\$sWS_Orderregel AS ORD INNER JOIN %2\$sWS_Artikel AS A ON A.RecordID=ORD.Artikel) INNER JOIN %2\$sLid AS L ON ORD.Lid=L.Nummer
					WHERE ORD.Lid=%3\$d AND ((ORD.AantalGeleverd IS NULL) OR ORD.AantalGeleverd < ORD.AantalBesteld) AND (NOT ORD.BestellingDefinitief IS NULL) %4\$s
					ORDER BY A.Code;", $selectnaam, $table_prefix, $lidid, $filter);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "overzichtbeheer") {
		if (strlen($filter) > 0) {
			$filter = "AND (" . $filter . ")";
		}
		$query = sprintf("SELECT ORD.RecordID, ORD.Ordernr, ORD.Lid, ORD.Artikel, ORD.AantalBesteld, ORD.AantalGeleverd, ORD.PrijsPerStuk, ORD.Opmerking,
					ORD.PrijsPerStuk * ORD.AantalBesteld AS Bedrag, ORD.Ingevoerd, ORD.BestellingDefinitief,
					%1\$s AS NaamLid, L.Roepnaam, L.Adres, L.Postcode, L.Woonplaats, L.Email, L.Email,
					A.Code, A.Omschrijving, A.Maat
					FROM (%2\$sWS_Orderregel AS ORD LEFT OUTER JOIN %2\$sWS_Artikel AS A ON A.RecordID=ORD.Artikel) INNER JOIN %2\$sLid AS L ON ORD.Lid=L.Nummer
					WHERE (A.VervallenPer >= CURDATE() OR (A.RecordID IS NULL)) %3\$s
					ORDER BY IF(ORD.Artikel > 0 AND ORD.AantalBesteld > 0, 1, 0), L.Achternaam, L.Roepnaam, A.Code;", $selectnaam, $table_prefix, $filter);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "totalen") {
		$query = sprintf("SELECT A.Code, A.Omschrijving, A.Maat,
					SUM(ORD.AantalBesteld) AS intBesteld, SUM(ORD.AantalGeleverd) AS intGeleverd, SUM(ORD.AantalBesteld * ORD.PrijsPerStuk) AS `curBedrag besteld`,
					SUM(ORD.AantalBesteld-ORD.AantalGeleverd) AS `intNog te leveren`, SUM(ORD.AantalGeleverd * ORD.PrijsPerStuk) AS `curBedrag geleverd`
					FROM %1\$sWS_Orderregel AS ORD INNER JOIN %1\$sWS_Artikel AS A ON ORD.Artikel = A.RecordID
					WHERE (ORD.Ordernr=0 OR (NOT ORD.BestellingDefinitief IS NULL)) AND VervallenPer >= CURDATE()
					GROUP BY A.Code, A.Omschrijving, A.Maat
					ORDER BY A.Code, A.Omschrijving, A.Maat;", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "maxordernr") {
		$query = sprintf("SELECT MAX(IFNULL(Ordernr, 0)) FROM %sWS_Orderregel;", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "add") {
		$query = sprintf("INSERT INTO %sWS_Orderregel SET Lid=%d, Artikel=%d, AantalBesteld=%d, Ordernr=%d, Ingevoerd=CURDATE(), IngevoerdDoor=%d;", $table_prefix, $lidid, $artikelid, $aantalbesteld, $ordernr, $_SESSION['lidid']);
		$result = fnQuery($query);
		if ($result > 0) {
			if ($artikelid > 0) {
				$mess = sprintf("Orderregel %d van %d stuks van artikel '%s' is aan order %d van %s is toegevoegd.", $result, $aantalbesteld, db_artikel("code", $artikelid), $ordernr, db_naamlid($lidid));
			} else {
				$mess = sprintf("Orderregel %d is aan order %d van %s is toegevoegd.", $result, $ordernr, db_naamlid($lidid));
			}
			db_logboek("add", $mess, 10, $lidid);
			if ($artikelid > 0 and $aantalbesteld > 0) {
				db_orderregel("vulprijsperstuk");
			}
		}
		return $mess;
		
	} elseif ($actie == "delete" and (($lidid > 0 and $artikelid > 0) or $recordid > 0)) {
		if ($recordid > 0) {
			$query = sprintf("DELETE FROM %sWS_Orderregel WHERE RecordID=%d;", $table_prefix, $recordid);
		} else {
			$query = sprintf("DELETE FROM %sWS_Orderregel WHERE Lid=%d AND Artikel=%d AND ISNULL(AantalGeleverd)=0;", $table_prefix, $lidid, $artikelid);
		}
		$result = fnQuery($query);
		if ($result > 0) {
			$query = sprintf("OPTIMIZE TABLE %sWS_Orderregel;", $table_prefix);
			fnQuery($query);
			if ($recordid > 0) {
				$mess = sprintf("Orderregel %d is verwijderd.", $recordid);
			} else {
				$mess = sprintf("De bestelling van artikel %d bij %s is verwijderd.", $artikelid, db_naamlid($lidid));
			}
			db_logboek("add", $mess, 10, $lidid);
		}
		return $mess;
		
	} elseif ($actie == "defmaken" and $lidid > 0) {
		$query = sprintf("UPDATE %sWS_Orderregel SET BestellingDefinitief=SYSDATE(), GewijzigdDoor=%d WHERE Lid=%d AND (BestellingDefinitief IS NULL) AND (NOT PrijsPerStuk IS NULL);", $table_prefix, $_SESSION['lidid'], $lidid);
		$result = fnQuery($query);
		
	} elseif ($actie == "opschonen") {
		$query = sprintf("DELETE FROM %sWS_Orderregel WHERE AantalBesteld=0 AND AantalGeleverd=0 AND Ingevoerd <= DATE_ADD(CURDATE(), INTERVAL -1 WEEK);", $table_prefix);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Er zijn bij het opschonen %d orderregels verwijderd.", $result);
		} else {
			$mess = "Er zijn bij het opschonen geen orderregels verwijderd.";
		}
		db_logboek("add", $mess, 10);
		return $mess;
		
	} elseif ($actie == "vulprijsperstuk") {
		$query = sprintf("UPDATE %1\$sWS_Orderregel AS ORD SET PrijsPerStuk=(SELECT MAX(Verkoopprijs) FROM %1\$sWS_Artikel AS A WHERE A.RecordID=ORD.Artikel)
					WHERE (ORD.PrijsPerStuk IS NULL) AND ORD.Artikel > 0;", $table_prefix);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Bij %d orderregels is de prijs per stuk ingevuld.", $result);
			db_logboek("add", $mess, 10);
		}
		return $mess;
	}
}

function db_rekening($actie, $nr=0, $filter="") {
	global $table_prefix, $selectlidnr;
	
	if ($actie == "minnummer") {
		$query = sprintf("SELECT MIN(Nummer) FROM %sRekening;", $table_prefix);
		return db_scalar($query);
		
	} elseif ($actie == "maxnummer") {
		$query = sprintf("SELECT MAX(Nummer) FROM %sRekening;", $table_prefix);
		return db_scalar($query);
		
	} elseif ($actie == "mindatum") {
		$query = sprintf("SELECT MIN(Datum) FROM %sRekening;", $table_prefix);
		return db_scalar($query);
		
	} elseif ($actie == "maxdatum") {
		$query = sprintf("SELECT MAX(Datum) FROM %sRekening;", $table_prefix);
		return db_scalar($query);

	} elseif ($actie == "hoofd" and $nr > 0) {
		$query = sprintf("SELECT RK.*, L.Adres, L.Postcode, L.Woonplaats, L.`Machtiging afgegeven` AS Machtiging,
					L.Bankrekening AS Bankrekeningnummer,
					DATE_ADD(RK.Datum, INTERVAL RK.BET_TERM*RK.BETAALDAG DAY) AS UitersteBetaaldatum,
					(%1\$1s) AS Lidnr
					FROM %2\$sRekening AS RK INNER JOIN %2\$sLid AS L ON RK.Lid = L.Nummer
					WHERE RK.Nummer=%3\$d;", $selectlidnr, $table_prefix, $nr);
		$result = fnQuery($query);
		return $result->fetch();
	} elseif ($actie == "regels" and $nr > 0) {
		$query = sprintf("SELECT RR.*, L.GEBDATUM FROM %1\$sRekreg AS RR LEFT JOIN %1\$sLid AS L ON RR.Lid = L.Nummer WHERE RR.Rekening=%2\$d;", $table_prefix, $nr);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "seizoenen") {
		$query = sprintf("SELECT DISTINCT Seizoen FROM %sRekening ORDER BY Seizoen DESC;", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "selectie") {
		if (strlen($filter) > 0 and startwith($filter, "WHERE") == false) {
			$filter = "WHERE " . $filter;
		}
		$query = sprintf("SELECT RK.*, L.Adres, L.Postcode, L.Woonplaats, L.Bankrekening AS Bankrekeningnummer,
					DATE_ADD(RK.Datum, INTERVAL RK.BET_TERM*RK.BETAALDAG DAY) AS UitersteBetaaldatum,
					(%1\$1s) AS Lidnr
					FROM %2\$sRekening AS RK INNER JOIN %2\$sLid AS L ON RK.Lid = L.Nummer
					%3\$s
					ORDER BY RK.Nummer;", $selectlidnr, $table_prefix, $filter);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "histmail" and $nr > 0) {
		$query = sprintf("SELECT message FROM %sMailing_hist WHERE Xtra_Char='REK' AND Xtra_Num=%d ORDER BY send_on DESC LIMIT 1;", $table_prefix, $nr);
		return db_scalar($query);
	}
} # db_rekening

function db_dms() {
}

function db_param($naam, $actie="curval", $newval="") {
	global $table_prefix;
	
	if (isset($_SESSION['settings'])) {
		$settings = $_SESSION['settings'];
	}
	
	$arrIntegers['logboek_bewaartijdinloggen'] = 2;
	$arrIntegers['logboek_bewaartijd'] = 6;
	$arrIntegers['login_geldigheidactivatie'] = 36;
	$arrIntegers['db_backup_type'] = 3;
	$arrIntegers['db_backupsopschonen'] = 11;
	$arrIntegers['login_autounlock'] = 120;
	$arrIntegers['mailing_bevestigingbestelling'] = 0;
	$arrIntegers['mailing_bevestigingopzegging'] = 0;
	$arrIntegers['mailing_bewaartijd'] = 3;
	$arrIntegers['mailing_bewakinginschrijving'] = 0;
	$arrIntegers['mailing_herstellenwachtwoord'] = 0;
	$arrIntegers['mailing_lidnr'] = 0;
	$arrIntegers['mailing_rekening_stuurnaar'] = 1;
	$arrIntegers['mailing_validatielogin'] = 0;
	$arrIntegers['max_grootte_bijlage'] = 2048;
	$arrIntegers['maxmailsperdag'] = 495;	
	$arrIntegers['maxmailsperminuut'] = 360;
	$arrIntegers['login_bewaartijd'] = 6;
	$arrIntegers['login_bewaartijdnietgebruikt'] = 21;
	$arrIntegers['login_maxlengte'] = 12;
	$arrIntegers['wachtwoord_minlengte'] = 7;
	$arrIntegers['wachtwoord_maxlengte'] = 12;
	$arrIntegers['login_maxinlogpogingen'] = 4;
	$arrIntegers['smtpport'] = 0;
	$arrIntegers['termijnvervallendiplomasmailen'] = 3;
	$arrIntegers['termijnvervallendiplomasmelden'] = 6;
	$arrIntegers['typemenu'] = 1;
	$arrIntegers['verjaardagenaantal'] = 7;
	$arrIntegers['verjaardagenvooruit'] = 5;
	$arrIntegers['zs_opzegtermijn'] = 1;
	
	$arrBooleans['db_backuptarren'] = 0;
	$arrBooleans['kaderoverzichtmetfoto'] = 1;
	$arrBooleans['toonpasfotoindiennietingelogd'] = 0;
	$arrBooleans['meisjesnaamtonen'] = 1;
	$arrBooleans['login_lidnrnodigbijaanvraag'] = 1;
	$arrBooleans['mailing_rekening_nulregelsweglaten'] = 0;
	$arrBooleans['mailing_resultaatversturen'] = 0;
	$arrBooleans['toneninschrijvingenbewakingen'] = 1;
	$arrBooleans['tonentoekomstigebewakingen'] = 0;
	$arrBooleans['zs_incl_beroep'] = 1;
	$arrBooleans['zs_incl_bezwmachtiging'] = 0;
	$arrBooleans['zs_incl_vogafgegeven'] = 0;
	$arrBooleans['zs_incl_bsn'] = 0;
	$arrBooleans['zs_incl_emailouders'] = 0;
	$arrBooleans['zs_incl_emailvereniging'] = 0;
	$arrBooleans['zs_incl_iban'] = 0;
	$arrBooleans['zs_incl_legitimatie'] = 1;
	$arrBooleans['zs_incl_machtiging'] = 0;
	$arrBooleans['zs_incl_slid'] = 0;
	$arrBooleans['zs_opzeggingautomatischverwerken'] = 0;
	
	$arrStrings['login_beperkttotgroep'] = "";
	$arrStrings['muteerbarememos'] = "D,G,WN";
	$arrStrings['naamwebsite'] = "Naam website";
	$arrStrings['mailing_extensies_toegestaan'] = "bmp, gif, jpeg, jpg, pdf, png, pps, rar, txt, zip";
	$arrStrings['mailing_rekening_from'] = "";
	$arrStrings['mailing_rekening_from_naam'] = "";
	$arrStrings['urlvereniging'] = "";
	$arrStrings['url_eigen_help'] = "";
	$arrStrings['zs_emailnieuwepasfoto'] = "";
	$arrStrings['zs_muteerbarememos'] = "";
	$arrStrings['zs_voorwaardenbestelling'] = "";
	$arrStrings['zs_voorwaardeninschrijving'] = "";

	$arrFloats['performance_trage_select'] = 0.5;
	
	// Onderstaande zijn niet in te stellen via een instellingen scherm.
	$arrStrings["emailsecretariaat"] = ""; // Komt uit de Access-database
	$arrStrings["emailledenadministratie"] = ""; // Komt uit de Access-database
	$arrStrings['laatstebackup'] = "";  // Komt uit de logging
	$arrStrings['naamvereniging'] = "Naam vereniging"; // Komt uit de Access-database
	$arrStrings['naamvereniging_afkorting'] = ""; // Komt uit de Access-database
	$arrIntegers['versie'] = 0; // Wordt automatisch bijgewerkt op basis van de php-bestanden
	$arrStrings['versiephp'] = ""; // Wordt automatisch bijgewerkt op basis van de laatst waargenomen PHP versie
	// *******
	
	if (array_key_exists($naam, $arrBooleans)) {
		$pt = "B";
	} elseif (array_key_exists($naam, $arrFloats)) {
		$pt = "F";
	} elseif (array_key_exists($naam, $arrIntegers)) {
		$pt = "I";
	} else {
		$pt = "T";
	}
	
	if ($actie == "lijst") {
		$query = sprintf("SELECT *, IFNULL(ValueNum, ValueChar) AS CurVal FROM %sAdmin_param ORDER BY Naam;", $table_prefix);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "curval" and strlen($naam) > 0) {
		if (isset($_SESSION['settings'][$naam]) and $naam != "laatstebackup") {
			$retval = $_SESSION['settings'][$naam];
		} else {
			if (array_key_exists($naam, $arrBooleans)) {
				$retval = $arrBooleans[$naam];
			} elseif (array_key_exists($naam, $arrIntegers)) {
				$retval = $arrIntegers[$naam];
			} elseif (array_key_exists($naam, $arrFloats)) {
				$retval = $arrFloats[$naam];
			} elseif (array_key_exists($naam, $arrStrings)) {
				$retval = $arrStrings[$naam];
			} else {
				$mess = sprintf("Parameter %s bestaat niet. Neem contact op met de ontwikkelaar.", $naam);
				debug($mess, 1, 0, 1);
				$retval = "";
			}
			
			if ($naam == "naamvereniging") {
				$query = sprintf("SELECT 1 AS RecordID, 'T' AS ParamType, Naam AS ValueChar FROM %sVereniging;", $table_prefix);
			} elseif ($naam == "naamvereniging_afkorting") {
				$query = sprintf("SELECT 1 AS RecordID, 'T' AS ParamType, AfkortingNaam AS ValueChar FROM %sVereniging;", $table_prefix);
			} elseif ($naam == "emailsecretariaat") {
				$query = sprintf("SELECT 1 AS RecordID, 'T' AS ParamType, EmailSecretariaat AS ValueChar FROM %sVereniging;", $table_prefix);
			} elseif ($naam == "emailledenadministratie") {
				$query = sprintf("SELECT 1 AS RecordID, 'T' AS ParamType, EmailLedenadministratie AS ValueChar FROM %sVereniging;", $table_prefix);
			} elseif ($naam == "laatstebackup") {
				$query = sprintf("SELECT 1 AS RecordID, 'T' AS ParamType, MAX(DatumTijd) AS ValueChar FROM %sAdmin_activiteit WHERE TypeActiviteit=3;", $table_prefix);
			} else {
				$query = sprintf("SELECT IFNULL(RecordID, 0) AS RecordID, ParamType, ValueNum, ValueChar FROM %sAdmin_param WHERE Naam='%s';", $table_prefix, $naam);
			}
			$result = fnQuery($query);
			$row = $result->fetch();
			if (isset($row->RecordID) and $row->RecordID > 0) {
				if (($row->ParamType == "F" or $row->ParamType == "I") and empty($row->ValueNum)) {
					$retval = 0;
				} elseif ($row->ParamType == "B" or $row->ParamType == "F" or $row->ParamType == "I") {
					$retval = $row->ValueNum;
				} elseif (empty($row->ValueChar)) {
					$retval = "";
				} else {
					$retval = $row->ValueChar;
				}
			}
			$_SESSION['settings'][$naam] = $retval;
		}
		return $retval;
		
	} elseif ($actie == "curval") {
		foreach (db_param("", "lijst") as $row) {
			if (($row->ParamType == "F" or $row->ParamType == "I") and empty($row->ValueNum)) {
				$retval = 0;
			} elseif ($row->ParamType == "B" or $row->ParamType == "F" or $row->ParamType == "I") {
				$retval = $row->ValueNum;
			} elseif (empty($row->ValueChar)) {
				$retval = "";
			} else {
				$retval = $row->ValueChar;
			}
			$_SESSION['settings'][$row->Naam] = $retval;
		}
		
	} elseif ($actie == "updval") {
		$query = sprintf("SELECT * FROM %sAdmin_param WHERE Naam='%s';", $table_prefix, $naam);
		$result = fnQuery($query);
		$cur = $result->fetch();
		$set = "";
		if (($cur->ParamType == "B" or $cur->ParamType == "I") and $cur->ValueNum != $newval) {
			$set = sprintf("ValueNum=%d", $newval);
		} elseif ($cur->ParamType == "F" and $cur->ValueNum != $newval) {
			$set = sprintf("ValueNum=%F", $newval);
		} elseif (($cur->ParamType == "S" or $cur->ParamType == "T") and $cur->ValueChar != $newval) {
			if (strlen($newval) == 0) {
				$set = "ValueChar=NULL";
			} else {
				$set = sprintf("ValueChar='%s'", $newval);
			}
		}
		if (strlen($set) > 0) {
			$set .= sprintf(", ParamType='%s', GewijzigdDoor=%d", $pt, $_SESSION['lidid']);
			$query = sprintf("UPDATE %sAdmin_param SET %s WHERE RecordID=%d;", $table_prefix, $set, $cur->RecordID);
			$result = fnQuery($query);
			if ($result > 0) {
				$_SESSION['settings'][$naam] = $newval;
				$mess = sprintf("Parameter '%s' is in '%s' gewijzigd.", $naam, $newval);
				if ($naam !== "versie") {
					db_logboek("add", $mess, 13, 0, 0, $cur->RecordID);
				}
			}
		}
		
	} elseif ($actie == "controle" and strlen($naam) > 0) {
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_param WHERE Naam LIKE '%s';", $table_prefix, $naam);
		if (db_scalar($query) == 0) {
			$query = sprintf("INSERT INTO %sAdmin_param SET Naam='%s', ParamType='%s', Ingevoerd=CURDATE(), IngevoerdDoor=%d;", $table_prefix, $naam, $pt, $_SESSION['lidid']);
			$result = fnQuery($query);
			if ($result > 0) {
				$mess = sprintf("Parameter '%s' is toegevoegd.", $naam);
				db_logboek("add", $mess, 13, 0, 0, $result);
			}
		}
		
	} elseif ($actie == "delete" and strlen($naam) > 0) {
		$query = sprintf("DELETE FROM %sAdmin_param WHERE Naam='%s';", $table_prefix, $naam);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Parameter '%s' is verwijderd.", $naam);
			db_logboek("add", $mess, 13);
		}
	}
}

function db_delete_local_tables() {
	global $arrTables, $table_prefix;
	
	foreach ($arrTables as $key => $tn) {
		if ($key >= 20) {
			fnQuery(sprintf("DELETE FROM %s%s;", $table_prefix, $tn));
		}
	}
}

function db_onderhoud() {
	global $table_prefix, $arrTables, $db_name, $wherelid, $wherelidond, $lididwebmasters;
	
	// Vaste aanpassingen aan de database na een upload, omdat MS-Access geen datum velden kent, alleen datum_tijd.
	$query = sprintf("SELECT DATA_TYPE FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%sBewaking' AND COLUMN_NAME='BEGIN_PER';", $db_name, $table_prefix);
	if (db_scalar($query) != "date") {
		fnQuery(sprintf("ALTER TABLE %sBewaking CHANGE `BEGIN_PER` `BEGIN_PER` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sBewaking CHANGE `EINDE_PER` `EINDE_PER` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sBewseiz CHANGE Begindatum Begindatum DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sBewseiz CHANGE `Einde` `Einde` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sBewseiz CHANGE Geboren Geboren DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sDiploma CHANGE `Vervallen` `Vervallen` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLid CHANGE `GEBDATUM` `GEBDATUM` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLid CHANGE `Overleden` `Overleden` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLid CHANGE `BezwaarMachtiging` `BezwaarMachtiging` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLid CHANGE `VOG afgegeven` `Datum` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLiddipl CHANGE `DatumBehaald` `DatumBehaald` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLiddipl CHANGE `LicentieVervallenPer` `LicentieVervallenPer` DATE DEFAULT NULL;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLidmaatschap CHANGE `LIDDATUM` `LIDDATUM` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLidmaatschap CHANGE `Opgezegd` `Opgezegd` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLidond CHANGE `Vanaf` `Vanaf` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLidond CHANGE `Opgezegd` `Opgezegd` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sLidRedNed CHANGE Geboortedatum Geboortedatum DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sMutatie CHANGE `Datum` `Datum` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sOnderdl CHANGE `VervallenPer` `VervallenPer` DATE;", $table_prefix));
		fnQuery(sprintf("ALTER TABLE %sRekening CHANGE `Datum` `Datum` DATE;", $table_prefix));
		foreach ($arrTables as $tn) {
			if (startwith($tn, "Admin_") == false and startwith($tn, "Mailing") == false) {
				fnQuery(sprintf("ALTER TABLE %s%s CHANGE Ingevoerd Ingevoerd DATE;", $table_prefix, $tn));
			}
		}
	}
	
	// Overige vaste aanpassingen aan de database na een upload,
	db_addautoincrement("Liddipl");
	db_addautoincrement("Lidmaatschap");
	db_addautoincrement("Memo");
	$query = sprintf("ALTER TABLE `%sMemo` CHANGE `Vertrouwelijk` `Vertrouwelijk` TINYINT(4) NULL DEFAULT '0'", $table_prefix);
	fnQuery($query);
	
	/***** Aanpassen lengte login in de database als deze kleiner is dan login_maxlengte  *****/
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%sAdmin_login' AND COLUMN_NAME LIKE 'Login';", $db_name, $table_prefix);
	if (db_scalar($query) < db_param("login_maxlengte")) {
		$query = sprintf("ALTER TABLE `%sAdmin_login` CHANGE `Login` `Login` VARCHAR( %s ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table_prefix, db_param("login_maxlengte"));
		fnQuery($query);
	}
	
	/***** Aanpassen lengte wachtwoord in de database als deze kleiner is dan 255. Deze lengte is nodig voor de password_hash()  *****/
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%sAdmin_login' AND COLUMN_NAME LIKE 'Wachtwoord';", $db_name, $table_prefix);
	if (db_scalar($query) < 255) {
		$query = sprintf("ALTER TABLE `%sAdmin_login` CHANGE `Wachtwoord` `Wachtwoord` VARCHAR(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table_prefix);
		fnQuery($query);
	}

	/***** Aanpassen lengte van de naam van paramters naar 50. Deze code kan na 1/1/2020 worden verwijderd. *****/
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%sAdmin_param' AND COLUMN_NAME LIKE 'Naam';", $db_name, $table_prefix);
	if (db_scalar($query) < 50) {
		$query = sprintf("ALTER TABLE `%sAdmin_param` CHANGE `Naam` `Naam` VARCHAR(50) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table_prefix);
		fnQuery($query);
	}
	
	/***** Aanpassen lengte van het IP-adres in Admin_activiteit naar 45 voor IP v6 adressen. Deze code kan na 1/1/2020 worden verwijderd. *****/
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%sAdmin_activiteit' AND COLUMN_NAME LIKE 'IP_adres';", $db_name, $table_prefix);
	if (db_scalar($query) < 45) {
		$query = sprintf("ALTER TABLE `%sAdmin_activiteit` CHANGE `IP_adres` `IP_adres` VARCHAR(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table_prefix);
		fnQuery($query);
	}
	
	/***** Aanpassen lengte van to_addr in Mailing_hist naar 100. Deze code kan na 1/1/2020 worden verwijderd. *****/
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%sMailing_hist' AND COLUMN_NAME LIKE 'to_addr';", $db_name, $table_prefix);
	if (db_scalar($query) < 100) {
		$query = sprintf("ALTER TABLE `%sMailing_hist` CHANGE `to_addr` `to_addr` VARCHAR(100) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table_prefix);
		fnQuery($query);
	}
	
	/***** Aanpassen lengte van cc_addr in Mailing_hist naar 100. Deze code kan na 1/1/2020 worden verwijderd. *****/
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%sMailing_hist' AND COLUMN_NAME LIKE 'cc_addr';", $db_name, $table_prefix);
	if (db_scalar($query) < 100) {
		$query = sprintf("ALTER TABLE `%sMailing_hist` CHANGE `cc_addr` `cc_addr` VARCHAR(100) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table_prefix);
		fnQuery($query);
	}
	
	/***** Aanpassen type veld van Ingevoerd in Admin_login naar datumtijd. Deze code kan na 1/1/2020 worden verwijderd. *****/
	$query = sprintf("ALTER TABLE `%sAdmin_login` CHANGE `Ingevoerd` `Ingevoerd` DATETIME NULL DEFAULT CURRENT_TIMESTAMP;", $table_prefix);
	fnQuery($query);
	
	/***** Velden die later zijn toegevoegd. *****/
	
	// Toevoegen veld 'VOG afgegeven' aan de tabel 'Lid'. Deze code kan na 1/9/2019 worden verwijderd.
	$tab = $table_prefix . "Lid";
	$col = "VOG afgegeven";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` DATE;", $tab, $col);
		fnQuery($query);
	}
	
	// Toevoegen veld 'Standaard kostenplaats' aan de tabel 'GBR'. Deze code kan na 1/9/2019 worden verwijderd.
	$tab = $table_prefix . "GBR";
	$col = "Standaard kostenplaats";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT(11);", $tab, $col);
		fnQuery($query);
	}
	
	// Toevoegen veld 'NamenOuders' aan de tabel 'Lid'. Deze code kan na 1/12/2019 worden verwijderd.
	$tab = $table_prefix . "Lid";
	$col = "NamenOuders";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` varchar(90);", $tab, $col);
		fnQuery($query);
	}
	
	// Toevoegen veld 'ActivatieKey' aan de tabel 'Admin_login'. Deze code kan na 1/12/2020 worden verwijderd.
	$tab = $table_prefix . "Admin_login";
	$col = "ActivatieKey";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(255);", $tab, $col);
		fnQuery($query);
	}
	
	// Toevoegen veld 'LoginWebsite' aan de tabel 'Lid'. Deze code kan na 1/12/2019 worden verwijderd.
	$tab = $table_prefix . "Lid";
	$col = "LoginWebsite";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` varchar(20);", $tab, $col);
		fnQuery($query);
	}
	
	// Toevoegen veld 'Getoond' aan de tabel 'Admin_activiteit'. Deze code kan na 1/12/2019 worden verwijderd.
	$tab = $table_prefix . "Admin_activiteit";
	$col = "Getoond";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` tinyint;", $tab, $col);
		fnQuery($query);
	}
	
	// Toevoegen veld 'RefFunction' aan de tabel 'Admin_activiteit'. Deze code kan na 1/12/2019 worden verwijderd.
	$tab = $table_prefix . "Admin_activiteit";
	$col = "RefFunction";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` varchar(40);", $tab, $col);
		fnQuery($query);
	}
	
	// Toevoegen veld '2FA' aan de tabel 'Admin_login'. Deze code kan na 1/1/2020 worden verwijderd.
	$tab = $table_prefix . "Admin_login";
	$col = "2FA";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` tinyint;", $tab, $col);
		fnQuery($query);
	}
	
	/***** Velden die aangepast zijn. *****/
	
	// Admin_Login, deze code kan na 31/12/2019 worden verwijderd.
	fnQuery(sprintf("ALTER TABLE %sAdmin_login CHANGE `Ingevoerd` `Ingevoerd` DATETIME;", $table_prefix));
	fnQuery($query);
	
	/***** Opschonen database na een upload uit de Access-database.  *****/

	$query = sprintf("DELETE FROM %sLid WHERE (NOT Verwijderd IS NULL);", $table_prefix);
	$result = fnQuery($query);
	if ($result > 0) {
		db_logboek("add", sprintf("Er zijn %d leden verwijderd, omdat ze een verwijderd markering hebben.", $result), 2);
		fnQuery(sprintf("OPTIMIZE TABLE %sLid;", $table_prefix));
	}
	
	$query = sprintf("DELETE FROM %sOnderdl WHERE `Type`<>'A' AND Kader<>1 AND (NOT VervallenPer IS NULL) AND VervallenPer < CURDATE();", $table_prefix);
	fnQuery($query);
	if ($result > 0) {
		$mess = sprintf("Er zijn %d onderdelen verwijderd, omdat ze vervallen zijn.", $result);
		fnQuery(sprintf("OPTIMIZE TABLE %sOnderdl;", $table_prefix));
		db_logboek("add", $mess, 2, 0, 1);
	}
	$query = sprintf('DELETE FROM %1$sLidond WHERE OnderdeelID NOT IN (SELECT RecordID FROM %1$sOnderdl);', $table_prefix);
	$result = fnQuery($query);
	if ($result > 0) {
		$mess = sprintf("Er zijn %d onderdelen bij leden verwijderd, omdat de onderdelen vervallen zijn.", $result);
		fnQuery(sprintf("OPTIMIZE TABLE %sLidond;", $table_prefix));
		db_logboek("add", $mess, 2, 0, 1);
	}

	db_diploma("opschonen");
	db_liddipl("opschonen");
	
	$query = sprintf("DELETE FROM %sGBR WHERE Balans=1;", $table_prefix);
	fnQuery($query);
	$query = sprintf('DELETE FROM %1$sMutatie WHERE GBR NOT IN (SELECT Kode FROM %1$sGBR);', $table_prefix);
	$result = fnQuery($query);
	if ($result > 0) {
		fnQuery(sprintf("OPTIMIZE TABLE %sGBR;", $table_prefix));
		fnQuery(sprintf("OPTIMIZE TABLE %sMutatie;", $table_prefix));
		$mess = sprintf("Er zijn %d mutaties verwijderd, omdat er geen resultaatrekening aan gekoppeld was.", $result);
		db_logboek("add", $mess, 2, 0, 1);
	}
	
	if ($_SERVER["HTTP_HOST"] != "phprbm.telling.nl") {
		//  Deze tabellen moeten op dit moment alleen in de ontwikkelomgeving aanwezig zijn. 
		$query = sprintf("DROP TABLE IF EXISTS %sDMS_Folder;", $table_prefix);
		fnQuery($query);
		$query = sprintf("DROP TABLE IF EXISTS %sDMS_Document;", $table_prefix);
		fnQuery($query);
	}
}

function db_addautoincrement($table) {
	global $table_prefix, $db_name;
	
	$table = $table_prefix . $table;
	$query = sprintf("SELECT EXTRA FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='RecordID';", $db_name, $table);
	$result = fnQuery($query);
	if ($result->fetchColumn() != "auto_increment") {
		$query = sprintf("DELETE FROM %s WHERE RecordID=0;", $table);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Er zijn %d records uit %s verwijderd, omdat RecordID leeg was.", $result, $table);
			db_logboek("add", $mess, 2);
		}
		fnQuery(sprintf("ALTER TABLE %s ADD UNIQUE (`RecordID`);", $table));
		fnQuery(sprintf("ALTER TABLE %s CHANGE RecordID RecordID INT( 11 ) NOT NULL AUTO_INCREMENT;", $table));
	}
}

function db_createtables() {
	global $table_prefix;

	$queries = sprintf("SET SQL_MODE='NO_AUTO_VALUE_ON_ZERO';

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_access` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Tabpage` varchar(50) NOT NULL,
  `Toegang` int(11) NOT NULL DEFAULT '-1',
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_activiteit` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL COMMENT 'Wie heeft deze activiteit uitgevoerd?',
  `DatumTijd` datetime NOT NULL,
  `Omschrijving` text NOT NULL,
  `ReferID` int(11) NOT NULL COMMENT 'Op welk RecordID heeft deze activiteit betrekking?',
  `Script` varchar(75) NOT NULL,
  `TypeActiviteit` tinyint(4) DEFAULT '0',
  `IP_adres` varchar(25) NOT NULL,
  `USER_AGENT` varchar(110) DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  KEY `TypeActiviteit` (`TypeActiviteit`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_interface` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL,
  `IP-adres` char(15) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `SQL-statement` text CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `Ingevoerd` datetime DEFAULT NULL,
  `Gedownload` datetime DEFAULT NULL,
  `Afgemeld` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_login` (
  `LidID` int(11) NOT NULL,
  `Login` varchar(15) NOT NULL,
  `openid_identity` varchar(50) DEFAULT NULL,
  `Wachtwoord` varchar(255) NOT NULL,
  `HerinneringVervallenDiplomas` tinyint(4) NOT NULL DEFAULT '0',
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime NOT NULL,
  `GewijzigdDoor` int(11) NOT NULL DEFAULT '0',
  `LastLogin` datetime DEFAULT NULL,
  `LastActivity` datetime NOT NULL,
  `Ingelogd` tinyint(4) NOT NULL DEFAULT '0',
  `FouteLogin` smallint(6) NOT NULL DEFAULT '0',
  `ActivatieKey` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`LidID`),
  UNIQUE KEY `Login` (`Login`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_param` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Naam` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `ParamType` char(2) COLLATE utf8_unicode_ci DEFAULT NULL,
  `ValueChar` text COLLATE utf8_unicode_ci,
  `ValueNum` decimal(16,6) DEFAULT NULL,
  `Ingevoerd` date NOT NULL DEFAULT '0000-00-00',
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP,
  `GewijzigdDoor` int(11) NOT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Naam` (`Naam`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci COMMENT='Parameters en instellingen';

CREATE TABLE IF NOT EXISTS `%1\$sBewaking` (
  `RecordID` int(11) DEFAULT NULL,
  `Lid` int(11) NOT NULL DEFAULT '0',
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
  `Kode` char(2) DEFAULT NULL,
  `Begindatum` date NOT NULL,
  `Einddatum` date NOT NULL,
  `InschrijvingOpen` tinyint(4) NOT NULL DEFAULT '1',
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `SeizoenBegindatum` (`SeizoenID`,`Begindatum`),
  UNIQUE KEY `SeizoenKode` (`SeizoenID`,`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sBewaking_Inschrijving` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Lid` int(11) NOT NULL,
  `Nummer` int(11) NOT NULL DEFAULT '0',
  `BlokID` int(11) NOT NULL,
  `Keuze` tinyint(4) NOT NULL DEFAULT '1',
  `Opmerking` varchar(50) DEFAULT NULL,
  `Definitief` datetime DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `Afgemeld` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

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
  `Vervallen` datetime DEFAULT NULL,
  `HistorieOpschonen` smallint(6) DEFAULT NULL,
  `VoorgangerID` int(11) DEFAULT NULL,
  `Alternatief` varchar(20) DEFAULT NULL,
  `Tonen in bewakingsadministratie` tinyint(4) DEFAULT NULL,
  `Zelfservice` tinyint(4) DEFAULT NULL,
  `RolID` int(11) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sEvenement` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Datum` datetime NOT NULL,
  `Eindtijd` varchar(5) NOT NULL,
  `Verzameltijd` varchar(5) NOT NULL,
  `Omschrijving` varchar(50) NOT NULL,
  `Email` varchar(45) NOT NULL,
  `TypeEvenement` int(11) NOT NULL,
  `InschrijvingOpen` tinyint(4) NOT NULL DEFAULT '1',
  `StandaardStatus` char(1) NOT NULL DEFAULT 'I',
  `BeperkTotGroep` int(11) NOT NULL DEFAULT '0' COMMENT 'Welke groep mag zich voor dit evenement inschrijven? 0 = iedereen.',
  `ZichtbaarVoor` int(11) NOT NULL DEFAULT '0' COMMENT 'Voor welke groep is dit evenement zichtbaar? 0 = iedereen.',
  `Ingevoerd` date NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `GewijzigdDoor` int(11) NOT NULL,
  `VerwijderdOp` date NOT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

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
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sEvenement_Type` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Omschrijving` varchar(30) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL,
  `Soort` char(1) NOT NULL DEFAULT 'W',
  `Ingevoerd` date NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `GewijzigdDoor` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

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
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sGBR` (
  `RecordID` int(11) DEFAULT NULL,
  `Kode` varchar(4) NOT NULL DEFAULT '',
  `OMSCHRIJV` varchar(40) DEFAULT NULL,
  `Balans` tinyint(4) DEFAULT NULL,
  `KSTNPLTS` tinyint(4) DEFAULT NULL,
  `VerdichtingID` int(11) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  `Standaard kostenplaats` int(11) DEFAULT NULL,
  PRIMARY KEY (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sKostenplaats` (
  `RecordID` int(11) NOT NULL DEFAULT '0',
  `Kode` varchar(5) DEFAULT NULL,
  `Naam` varchar(35) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT NULL,
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
  `GEBDATUM` datetime DEFAULT NULL,
  `Overleden` datetime DEFAULT NULL,
  `GEBPLAATS` varchar(22) DEFAULT NULL,
  `Adres` varchar(30) DEFAULT NULL,
  `Huisnr` int(11) DEFAULT NULL,
  `Toevoeging` varchar(5) DEFAULT NULL,
  `Postcode` varchar(7) DEFAULT NULL,
  `Woonplaats` varchar(22) DEFAULT NULL,
  `Buitenland` tinyint(4) DEFAULT NULL,
  `Telefoon` varchar(21) DEFAULT NULL,
  `Telefoonnummer is geheim` tinyint(4) DEFAULT NULL,
  `Mobiel` varchar(21) DEFAULT NULL,
  `Mobiel is geheim` tinyint(4) DEFAULT NULL,
  `Email` varchar(45) DEFAULT NULL,
  `EmailVereniging` varchar(45) DEFAULT NULL,
  `EmailOuders` varchar(60) DEFAULT NULL,
  `NamenOuders` varchar(90) DEFAULT NULL,
  `Waarschuwen bij nood` varchar(255) DEFAULT NULL,
  `BANKGIRO` varchar(10) DEFAULT NULL,
  `Bankrekening` varchar(18) DEFAULT NULL,
  `Burgerservicenummer` int(11) DEFAULT NULL,
  `Machtiging afgegeven` tinyint(4) DEFAULT NULL,
  `BezwaarMachtiging` datetime DEFAULT NULL,
  `Legitimatietype` varchar(1) DEFAULT NULL,
  `Legitimatienummer` varchar(15) DEFAULT NULL,
  `VOG afgegeven` datetime DEFAULT NULL,
  `Wijknummer` smallint(6) DEFAULT NULL,
  `RelnrRedNed` varchar(8) DEFAULT NULL,
  `Beroep` varchar(40) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  `Verwijderd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLiddipl` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Lid` int(11) DEFAULT NULL,
  `DiplomaID` int(11) DEFAULT NULL,
  `DatumBehaald` datetime DEFAULT NULL,
  `EXPLAATS` varchar(23) DEFAULT NULL,
  `Beoordelaar` int(11) DEFAULT NULL,
  `LaatsteBeoordeling` tinyint(4) DEFAULT NULL,
  `Diplomanummer` varchar(25) NOT NULL,
  `Examen` int(11) DEFAULT NULL,
  `LicentieVervallenPer` datetime DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  UNIQUE KEY `RecordID` (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLidmaatschap` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Lid` int(11) NOT NULL DEFAULT '0',
  `LIDDATUM` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `Opgezegd` datetime DEFAULT NULL,
  `OpgezegdDoorVereniging` tinyint(4) DEFAULT NULL,
  `Lidnr` int(11) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Lid`,`LIDDATUM`),
  UNIQUE KEY `RecordID` (`RecordID`),
  UNIQUE KEY `Lidnr` (`Lidnr`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLidond` (
  `RecordID` int(11) DEFAULT NULL,
  `Lid` int(11) NOT NULL DEFAULT '0',
  `OnderdeelID` int(11) NOT NULL DEFAULT '0',
  `Vanaf` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `Opgezegd` datetime DEFAULT NULL,
  `Functie` tinyint(4) NOT NULL DEFAULT '0',
  `EmailFunctie` varchar(45) DEFAULT NULL,
  `GroepID` int(11) DEFAULT NULL,
  `Opmerk` varchar(30) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Lid`,`OnderdeelID`,`Vanaf`,`Functie`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLidRedNed` (
  `Lidnr` int(11) DEFAULT NULL,
  `RelnrRedNed` varchar(8) DEFAULT NULL,
  `Roepnaam` varchar(35) DEFAULT NULL,
  `Tussenvoegsels` varchar(7) DEFAULT NULL,
  `Achternaam` varchar(50) DEFAULT NULL,
  `Voorletters` varchar(10) DEFAULT NULL,
  `Geslacht` varchar(1) DEFAULT NULL,
  `Geboortedatum` datetime DEFAULT NULL,
  `Postcode` varchar(7) DEFAULT NULL,
  `Huisnr` smallint(6) DEFAULT NULL,
  `Foutboodschap` varchar(75) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMailing` (
  `MailingID` int(11) NOT NULL AUTO_INCREMENT,
  `from_name` varchar(50) DEFAULT NULL COMMENT 'Omschrijving van de groep personen aan wie deze mailing gericht is',
  `from_addr` varchar(50) DEFAULT NULL,
  `to_name` varchar(50) DEFAULT NULL,
  `cc_addr` varchar(50) DEFAULT NULL,
  `subject` varchar(75) DEFAULT NULL,
  `message` text,
  `template` tinyint(4) NOT NULL DEFAULT '0',
  `confidential` tinyint(4) NOT NULL DEFAULT '0',
  `verzamelen` tinyint(4) NOT NULL DEFAULT '0',
  `new_on` datetime DEFAULT NULL,
  `AddedBy` int(11) NOT NULL,
  `changed_on` datetime DEFAULT NULL,
  `ChangedBy` int(11) NOT NULL,
  `sent_on` datetime DEFAULT NULL,
  `SentBy` int(11) NOT NULL,
  `deleted_on` datetime DEFAULT NULL,
  `DeletedBy` int(11) NOT NULL,
  PRIMARY KEY (`MailingID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMailing_hist` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL DEFAULT '0',
  `MailingID` int(11) NOT NULL,
  `Xtra_Char` varchar(5) DEFAULT NULL,
  `Xtra_Num` int(11) DEFAULT NULL,
  `from_name` varchar(50) NOT NULL,
  `from_addr` varchar(50) NOT NULL,
  `to_name` varchar(50) NOT NULL,
  `subject` varchar(75) NOT NULL,
  `to_addr` varchar(50) NOT NULL,
  `cc_addr` varchar(80) NOT NULL,
  `message` text NOT NULL,
  `send_by` int(11) NOT NULL,
  `send_on` datetime NOT NULL,
  `send_mt` decimal(16,6) NOT NULL,
  `Successful` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`RecordID`),
  KEY `LidID` (`LidID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMailing_rcpt` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `MailingID` int(11) NOT NULL DEFAULT '0',
  `LidID` int(11) NOT NULL DEFAULT '0',
  `to_address` varchar(50) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMemo` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Lid` int(11) NOT NULL DEFAULT '0',
  `Soort` varchar(1) NOT NULL DEFAULT '',
  `Vertrouwelijk` tinyint(4) DEFAULT '0',
  `Memo` longtext,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Lid`,`Soort`),
  UNIQUE KEY `RecordID` (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMutatie` (
  `RecordID` int(11) DEFAULT NULL,
  `BoekjaarID` int(11) NOT NULL DEFAULT '0',
  `Periode` tinyint(4) NOT NULL DEFAULT '0',
  `DagboekID` int(11) NOT NULL DEFAULT '0',
  `GBR` varchar(4) DEFAULT NULL,
  `KostenplaatsID` int(11) DEFAULT NULL,
  `Stuknr` smallint(6) NOT NULL DEFAULT '0',
  `Regelnr` smallint(6) NOT NULL DEFAULT '0',
  `Datum` date DEFAULT NULL,
  `Debet` decimal(8,2) DEFAULT NULL,
  `Credit` decimal(8,2) DEFAULT NULL,
  `OMSCHRIJV` varchar(45) DEFAULT NULL,
  `Rekening` int(11) DEFAULT NULL,
  `BTWcode` tinyint(4) DEFAULT NULL,
  `BTWbedrag` decimal(8,2) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`BoekjaarID`,`Periode`,`DagboekID`,`Stuknr`,`Regelnr`)
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
  `VervallenPer` datetime DEFAULT NULL,
  `HistorieOpschonen` smallint(6) DEFAULT NULL,
  `GekoppeldAanQuery` int(11) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT NULL,
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
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sRekreg` (
  `RecordID` int(11) DEFAULT NULL,
  `Rekening` int(11) NOT NULL DEFAULT '0',
  `Regelnr` smallint(6) NOT NULL DEFAULT '0',
  `Lid` int(11) DEFAULT NULL,
  `KSTNPLTS` varchar(5) DEFAULT NULL,
  `OMSCHRIJV` varchar(60) DEFAULT NULL,
  `Bedrag` decimal(8,2) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Rekening`,`Regelnr`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sSeizoen` (
  `Nummer` smallint(6) NOT NULL DEFAULT '0',
  `Begindatum` datetime DEFAULT NULL,
  `Einddatum` datetime DEFAULT NULL,
  `Leeftijdsgrens jeugdleden` tinyint(4) DEFAULT NULL,
  `Contributie leden` decimal(8,2) DEFAULT NULL,
  `Contributie jeugdleden` decimal(8,2) DEFAULT NULL,
  `Contributie kader` decimal(8,2) DEFAULT NULL,
  `Contributie Onderscheidingen` decimal(8,2) DEFAULT NULL,
  `RekeningenVerzamelen` tinyint(4) DEFAULT NULL,
  `Rekeningomschrijving` varchar(30) DEFAULT NULL,
  `BetaaldagenTermijn` smallint(6) DEFAULT NULL,
  `StandaardAantalTermijnen` smallint(6) DEFAULT NULL,
  `Verenigingscontributie omschrijving` varchar(50) DEFAULT NULL,
  `Verenigingscontributie kostenplaats` varchar(5) DEFAULT NULL,
  `Gezinskorting bedrag` decimal(8,2) DEFAULT NULL,
  `Maximale verenigingscontributie` decimal(8,2) DEFAULT NULL,
  `Gezinskorting omschrijving` varchar(50) DEFAULT NULL,
  `Gezinskorting kostenplaats` varchar(4) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sVereniging` (
  `Naam` varchar(50) DEFAULT NULL,
  `Brigadenaam` varchar(50) DEFAULT NULL,
  `AfkortingNaam` varchar(5) DEFAULT NULL,
  `Bankrekening` varchar(18) DEFAULT NULL,
  `BewaartijdHistorie` smallint(6) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sWS_Artikel` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Code` char(6) COLLATE utf8_unicode_ci NOT NULL,
  `Omschrijving` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `Beschrijving` text COLLATE utf8_unicode_ci NOT NULL,
  `Maat` char(4) COLLATE utf8_unicode_ci DEFAULT NULL,
  `Verkoopprijs` decimal(10,2) NOT NULL DEFAULT '0.00',
  `BeschikbaarTot` date NOT NULL DEFAULT '9999-12-31' COMMENT 'Tot welke datum is dit artikel in de zelfservice beschikbaar?',
  `VervallenPer` date NOT NULL DEFAULT '9999-12-31' COMMENT 'Na deze datum is dit artikel voor niemand meer beschikbaar.',
  `MaxAantalPerLid` smallint(6) NOT NULL DEFAULT '999',
  `BeperkTotGroep` int(11) NOT NULL DEFAULT '0' COMMENT 'Welke groep mag dit artikel in de zelfservice bestellen? 0 is iedereen.',
  `Ingevoerd` date NOT NULL,
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP,
  `GewijzigdDoor` int(11) NOT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

CREATE TABLE IF NOT EXISTS `%1\$sWS_Orderregel` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Ordernr` int(11) NOT NULL DEFAULT '0',
  `Artikel` int(11) NOT NULL DEFAULT '0',
  `Lid` int(11) NOT NULL DEFAULT '0',
  `AantalBesteld` smallint(6) NOT NULL DEFAULT '0',
  `PrijsPerStuk` decimal(10,2) DEFAULT NULL,
  `Opmerking` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `BestellingDefinitief` datetime DEFAULT NULL COMMENT 'Heeft het lid de bestelling bevestigd?',
  `AantalGeleverd` smallint(6) NOT NULL DEFAULT '0',
  `Ingevoerd` date NOT NULL,
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP,
  `GewijzigdDoor` int(11) NOT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `ArtikelLid` (`Artikel`,`Lid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;", $table_prefix);

	fnQuery($queries);
}

function db_backup() {
	global $db_name, $arrTables, $table_prefix;
	
	$db_folderbackup = db_param("db_folderbackup");
	
	$buname = $db_name . "_backup_" . Date("Y-m-d-H-i-s") . "-" . rand(10001, 99998);
	
	if (strlen($db_folderbackup) < 5) {
		$db_folderbackup = str_replace($_SERVER['PHP_SELF'], "", $_SERVER["SCRIPT_FILENAME"]) . "/backups/";
		if (!file_exists($db_folderbackup)) {
			if (mkdir($db_folderbackup, 0755, true) == true) {
				$ret = sprintf("Directory '%s' is aangemaakt.", $db_folderbackup);
				db_logboek("add", $ret, 2, 0, 1);
			}
		}
	} elseif (substr($db_folderbackup, -1) != "/") {
		$db_folderbackup .= "/";
	}
	db_param("db_folderbackup", "updval", $db_folderbackup);

	$mess = "";
	if (strtotime(db_param("laatstebackup")) < mktime(date("H")-1, date("m"), 0, date("m"), date("d"), date("Y"))) {
		$FileName = $db_folderbackup . $buname;
		
		if (db_param("db_backup_type") == 1 or db_param("db_backup_type") == 2) {
			$tabs = "";
			foreach ($arrTables as $tnr => $tnm) {
				if ((db_param("db_backup_type") == 1 and $tnr < 20) or (db_param("db_backup_type") == 2 and $tnr >= 20)) {
					$tabs .= $table_prefix . $tnm . ",";
				}
			}
		} else {
			$tabs = "*";
		}
		
		$data = fnBackupTables($tabs);

		if (strlen($data) > 500) {
		
			$buf = fopen($FileName . ".sql", 'w');		
			fwrite($buf, $data);
			fclose($buf);
			
			$mess = sprintf("Backup, type %d, is in '%s.sql' gemaakt. ", db_param("db_backup_type"), str_replace($db_folderbackup, "", $FileName));

			if (db_param("db_backuptarren") == 1) {
				exec("/bin/tar -czf " . $FileName . ".tar.gz" . " " . $FileName . ".sql");
				exec("/bin/rm " . $FileName . ".sql");
				chmod($FileName . ".tar.gz", 0000);
			} else {
				chmod($FileName . ".sql", 0000);
			}

			if (db_param("db_backupsopschonen") > 0 and $handle = opendir($db_folderbackup)) {
				$mess .= "Opschonen backupfolder: ";
				while (false !== ($file = readdir($handle))) {
					sleep(5);
					if ($file != "." and $file != ".." and startwith($file, $buname) == false) {
						if (filectime($db_folderbackup . $file) < mktime(0, 0, 0, date("m"), date("d")-db_param("db_backupsopschonen"), date("Y")) or filesize($db_folderbackup . $file) < 500 ) {
							unlink($db_folderbackup . $file);
							$mess .= sprintf("Bestand %s is verwijderd. ", $file);
						}
					}
				}
			}
			
		} else {
			$mess = "Het backup-script heeft gedraaid, maar de backup is niet gelukt.\n";
		}
	} else {
		db_logboek("add", "Het backupscript heeft korter dan 1 uur geleden met success gedraaid. Het wordt niet nogmaals uitgevoerd.", 2, 0, 1);
	}
	
	if (strlen($mess) > 2) {
		db_logboek("add", $mess, 3, 0, 1);
	}
}

function fnBackupTables($tables="*") {
	global $dbc, $table_prefix;

	$data = "";
   if($tables == "*") {
      $tables = array();
      $result = $dbc->prepare('SHOW TABLES'); 
      $result->execute();                         
      while($row = $result->fetch(PDO::FETCH_NUM)) {
			$tables[] = $row[0]; 
      }
   } else {
		if (endswith($tables, ",")) {
			$tables = substr($tables, 0, strlen($tables)-1);
		}
      $tables = is_array($tables) ? $tables : explode(',',$tables);
   }
   
	foreach($tables as $table) {
		$query = sprintf("SELECT * FROM `%s`;", $table);
		$resultcount = $dbc->prepare($query);
      $resultcount->execute();
		$num_fields = $resultcount->columnCount();

		$query = sprintf("SELECT * FROM `%s`;", $table);
		$result = $dbc->prepare($query);
      $result->execute();
      $data .= sprintf("DROP TABLE IF EXISTS `%s`;", $table);

      $result2 = $dbc->prepare('SHOW CREATE TABLE '.$table);  
      $result2->execute();                            
      $row2 = $result2->fetch(PDO::FETCH_NUM);
      $data .= sprintf("\n\n%s;\n\n", $row2[1]);
		
		for ($i = 0; $i < $num_fields; $i++) {
			while($row = $result->fetch(PDO::FETCH_NUM)) { 
				$data .= sprintf("INSERT INTO `%s` VALUES(", $table);
            for($j=0; $j<$num_fields; $j++) {
					$row[$j] = addslashes($row[$j]); 
               $row[$j] = str_replace("\n","\\n",$row[$j]);
               if (isset($row[$j])) {
						$data .= '"' . $row[$j] . '"' ;
					} else {
						$data .= '""';
					}
               if ($j < ($num_fields-1)) {
						$data .= ",";
					}
            }
            $data .= ");\n";
         }
      }
      $data .= "\n\n\n";
   }

	return $data;
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
	$stats['gemiddeldeleeftijd'] = db_scalar($query);
	
   $query = sprintf("SELECT COUNT(DISTINCT LO.Lid) FROM %s WHERE (O.Kader=1 OR F.Kader=1) AND %s;", $fromlidond, $wherelidond);
	$stats['aantalkaderleden'] = db_scalar($query);
	
	$query = sprintf("SELECT %1\$s AS LidNaam FROM %2\$sLid AS L INNER JOIN %2\$sAdmin_login AS Login ON L.Nummer = Login.LidID"
			 . " ORDER BY Login.Ingevoerd DESC LIMIT 1;", $selectnaam, $table_prefix);
	$stats['nieuwstelogin'] = db_scalar($query);
	
	$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login AS Login;", $table_prefix);
	$stats['aantallogins'] = db_scalar($query);
	
	$query = sprintf("SELECT MAX(DatumTijd) FROM %sAdmin_activiteit WHERE TypeActiviteit=9;", $table_prefix);
	$stats['laatsteupload'] = db_scalar($query);
	
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
	$lgw = db_scalar($query);
	if ($lgw > $stats['laatstgewijzigd']) {
		$stats['laatstgewijzigd'] = $lgw;
	}
	
	if ($lidid > 0) {
		$query = sprintf("SELECT MAX(LD.Gewijzigd) FROM %sLiddipl AS LD%s;", $table_prefix, $filter);
		$lgw = db_scalar($query);
		if ($lgw > $stats['laatstgewijzigd']) {
			$stats['laatstgewijzigd'] = $lgw;
		}
	}
	
	$query = sprintf("SELECT MAX(RK.Gewijzigd) FROM %sRekening AS RK%s;", $table_prefix, $filter);
	$lgw = db_scalar($query);
	if ($lgw > $stats['laatstgewijzigd']) {
		$stats['laatstgewijzigd'] = $lgw;
	}	
	
	if ($lidid == 0) {
		$query = sprintf("SELECT MAX(M.Gewijzigd) FROM %sMutatie AS M;", $table_prefix);
		$lgw = db_scalar($query);
		if ($lgw > $stats['laatstgewijzigd']) {
			$stats['laatstgewijzigd'] = $lgw;
		}
	}	
	
	return $stats;
} # db_stats

?>
