<?php

$arrTables[] = "Admin_access";
$arrTables[] = "Admin_activiteit";
$arrTables[] = "Admin_interface";
$arrTables[] = "Admin_login";
$arrTables[] = "Admin_param";
$arrTables[] = "Bewaking_Blok";
$arrTables[] = "Bewaking_Inschrijving";
$arrTables[] = "Evenement";
$arrTables[] = "Evenement_Deelnemer";
$arrTables[] = "Evenement_Type";
$arrTables[] = "Mailing";
$arrTables[] = "Mailing_hist";
$arrTables[] = "Mailing_rcpt";
$arrTables[] = "Mailing_vanaf";
$arrTables[] = "Stukken";
$arrTables[] = "WS_Artikel";
$arrTables[] = "WS_Orderregel";
// $arrTables[] = "DMS_Document";
// $arrTables[] = "DMS_Folder";

// Overgenomen uit de Access-database
$arrTables[20] = "Bewaking";
$arrTables[] = "Bewseiz";
$arrTables[] = "Boekjaar";
$arrTables[] = "Diploma";
$arrTables[] = "Functie";
$arrTables[] = "GBR";
$arrTables[] = "Groep";
$arrTables[] = "Kostenplaats";
$arrTables[] = "Lid";
$arrTables[] = "Liddipl";
$arrTables[] = "Lidmaatschap";
$arrTables[] = "Lidond";
$arrTables[] = "LidRedNed";
$arrTables[] = "Memo";
$arrTables[] = "Mutatie";
$arrTables[] = "Onderdl";
$arrTables[] = "Organisatie";
$arrTables[] = "Rekening";
$arrTables[] = "Rekreg";
$arrTables[] = "Vereniging";

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
$TypeActiviteit[16] = "Toestemmingen";
$TypeActiviteit[17] = "Selecties";
$TypeActiviteit[18] = "Taken";
$TypeActiviteit[97] = "Foutieve login";
$TypeActiviteit[98] = "Performance";
$TypeActiviteit[99] = "Debugging";
asort($TypeActiviteit);

$fdlang = "'%e %M %Y'";
$fdtlang = "'%e %M %Y (%H:%i)'";
$fdkort = "'%e-%c-%Y'";
$fdaccess = "%m/%d/%Y";
$fdsql = "%Y-%m-%d";

$selectnaam = "CONCAT(IF(LENGTH(L.Roepnaam)>1, L.Roepnaam, L.Voorletter), ' ', IF(L.Tussenv>'', CONCAT(L.Tussenv, ' '), ''), L.Achternaam)";
$selectzoeknaam = "TRIM(CONCAT(L.Achternaam, ', ', IF(L.Tussenv>'', CONCAT(L.Tussenv, ' '), ''), IF(LENGTH(L.Roepnaam)>1, L.Roepnaam, L.Voorletter), ' '))";

$selectgeslacht = "CASE L.Geslacht ";
foreach (ARRGESLACHT as $c => $o) {
	$selectgeslacht .= sprintf("WHEN '%s' THEN '%s' ", $c, $o);
}
$selectgeslacht .= "ELSE 'Onbekend' END";

$selecttelefoon = "IF(LENGTH(L.Telefoon) < 10, L.Mobiel, L.Telefoon)";
$selectleeftijd = "IF(ISNULL(L.GEBDATUM) OR (NOT ISNULL(L.Overleden)), NULL, CONCAT(TIMESTAMPDIFF(YEAR, L.GEBDATUM, CURDATE()), ' jaar'))";
$selectlidnr = sprintf("SELECT MAX(Lidnr) FROM %sLidmaatschap AS LM WHERE LM.Lid=L.Nummer AND LM.LIDDATUM <= CURDATE() AND ((LM.Opgezegd IS NULL) OR LM.Opgezegd >= CURDATE())", TABLE_PREFIX);
$selectmailbox = "CASE WHEN (NOT M.deleted_on IS NULL) THEN 'Prullenbak'
							  WHEN (M.Template=1) THEN 'Templates'
							  WHEN (M.sent_on IS NULL) THEN 'Concepten'
							  ELSE 'Verzonden' END";  // Moet nog verder ingevoerd worden, indien wenselijk
$fromlid = sprintf('%1$sLid AS L LEFT OUTER JOIN %1$sLidmaatschap AS LM ON L.Nummer = LM.Lid', TABLE_PREFIX);
$wherelid = "LM.LIDDATUM <= CURDATE() AND IFNULL(LM.Opgezegd, CURDATE()) >= CURDATE()";
$fromlidond = sprintf('%1$sLid AS L INNER JOIN ((%1$sLidond AS LO INNER JOIN %1$sFunctie AS F ON LO.Functie = F.Nummer) INNER JOIN %1$sOnderdl AS O ON LO.OnderdeelID = O.RecordID) ON L.Nummer = LO.Lid', TABLE_PREFIX);
$wherelidond = "LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE()";
$whereliddipl = "IFNULL(LD.LicentieVervallenPer, CURDATE()) >= CURDATE()";
$sqllidgroepen = sprintf("SELECT OnderdeelID FROM %sLidond AS LO WHERE %s AND Lid=%d", TABLE_PREFIX, $wherelidond, $_SESSION['lidid']);

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

if (!isset($_SESSION['aantallid']) and (TableExists("Lid") == false or TableExists("Lidond") == false)) {
	db_createtables();
	db_onderhoud(2);
	$_SESSION['aantallid'] = 0;
} else if (!isset($_SESSION['aantallid']) or $_SESSION['aantallid'] < 10) {
	$_SESSION['aantallid'] = db_lid("aantal");
}

if (!isset($_SESSION['aantallogin']) or $_SESSION['aantallogin'] < 5) {
	$_SESSION['aantallogin'] = db_logins("aantal");
}

if (!isset($_SESSION['aantalrekeningen']) or $_SESSION['aantalrekeningen'] == 0) {
	$query = sprintf("SELECT COUNT(*) FROM %sRekening;", TABLE_PREFIX);
	$_SESSION['aantalrekeningen'] = db_scalar($query);
	if ($_SESSION['aantalrekeningen'] > 0) {
		$query = sprintf("SELECT COUNT(*) FROM %sRekreg;", TABLE_PREFIX);
		$_SESSION['aantalrekreg'] = db_scalar($query);
	} else {
		$_SESSION['aantalrekreg'] = 0;
	}
}

$pts = db_param("performance_trage_select");
function fnQuery($query, $logtype=0) {
	global $dbc, $pts;
	
	$starttijd = microtime(true);
	$mess = "";
	
	try {
		
		if (startwith($query, "SELECT")) {
			$rv = $dbc->query($query);
			$exec_tijd = microtime(true) - $starttijd;
			if ($pts > 0 and $exec_tijd >= $pts) {
				$mess = sprintf("%f seconden query: %s", $exec_tijd, $query);
				$logtype = 98;
			} elseif ($logtype > 0) {
				$mess = sprintf("Uitgevoerde query: %s", $query);
			}
		} elseif (startwith($query, "INSERT ")) {
			$rv = $dbc->prepare($query)->execute();
			if ($dbc->lastInsertID() > 0) {
				$rv = $dbc->lastInsertID();
			} 
			$mess = sprintf("Uitgevoerd: %s / nieuw ID: %d", $query, $rv);

		} else {
			$result = $dbc->prepare($query);
			$result->execute();
			$rv = $result->RowCount();
			if (startwith($query, "UPDATE") or startwith($query, "DELETE ")) {
				$mess = sprintf("Uitgevoerd: %s / records affected: %d", $query, $rv);
			} else {
				$mess = sprintf("Uitgevoerd: %s", $query);
			}
			$result = null;
		}
		if ($logtype > 0 and strlen($mess) > 0) {
			db_logboek("add", $mess, $logtype);
		}
		return $rv;
		
	} catch (Exception $e) {					
		$mess = sprintf("Error in SQL '%s': %s", $query, $e->getMessage());
		debug($mess, 1, 0, 99);
		return false;
	}
}

function db_scalar($query) {
	$result = fnQuery($query);
	return $result->fetchColumn();
} # db_scalar

function TableExists($table) {
	global $db_name;
	
	if (strlen(TABLE_PREFIX) > 1 and startwith($table, TABLE_PREFIX) == FALSE) {
		$table = TABLE_PREFIX . $table;
	}
	
	$query = sprintf("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema='%s' AND table_name='%s';", $db_name, $table);
	$result = fnQuery($query);
	if ($result->fetchColumn() > 0) {
		return true;
	} else {
		return false;
	}
} # TableExists

function db_gegevenslid($LidID, $soort="Alg") {
	global $selectnaam, $selectgeslacht, $selectleeftijd, $fromlid, $fromlidond, $wherelidond, $dlnstatus, $fdlang;

	if ($soort == "Alg") {
		
		$sqond = sprintf("SELECT MIN(O.Naam) FROM %1\$sOnderdl AS O INNER JOIN %1\$sLidond AS LO ON O.RecordID = LO.OnderdeelID
								WHERE O.`Type` = 'O' AND LO.Lid = L.Nummer AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd >= CURDATE())", TABLE_PREFIX);
		
		$sqllegitimatie = "IF(LENGTH(L.Legitimatienummer) > 4, CONCAT(CASE L.Legitimatietype ";
		foreach (ARRLEGITIMATIE as $t => $o) {
			$sqllegitimatie .= sprintf("WHEN '%s' THEN '%s' ", $t, $o);
		}
		$sqllegitimatie .= "END, ' ', L.Legitimatienummer), NULL)";
		
		if (db_param("WoonadresAnderenTonen") == 1 or $LidID == $_SESSION['lidid']) {
			$adresgegevens = "'Adresgegevens' AS Kop2,
					L.Adres,
					IF(LENGTH(L.Woonplaats) > 1, CONCAT(L.Postcode, '  ', L.Woonplaats), '') AS `Postcode en woonplaats`, ";
		} else {
			$adresgegevens = "";
		}
		$VOG = "`VOG afgegeven` AS 'dteVOG afgegeven op',";
		$sqlbd = sprintf("SELECT MAX(EINDE_PER) FROM %sBewaking WHERE Lid=L.Nummer AND BEGIN_PER < CURDATE()", TABLE_PREFIX);
		$query = sprintf("SELECT CONCAT(`Waarschuwen bij nood`, NamenOuders) FROM %sLid WHERE Nummer=%d;", TABLE_PREFIX, $LidID);
		if (strlen(db_scalar($query)) > 3) {
			$wbn = " 'Waarschuwen bij nood' as Kop7";
			$wbn .= ", `NamenOuders` AS `Namen ouders`";
			$wbn .= ", `Waarschuwen bij nood` AS `Contactgegevens`,";
		} else {
			$wbn = "";
		}
		$sqdieet = sprintf("SELECT `Memo` FROM %sMemo WHERE Lid=%d AND Soort='D'", TABLE_PREFIX, $LidID);
		if (strlen(db_scalar($sqdieet)) > 1) {
			$sqdieet = sprintf(" 'Dieet' as Kop7, (%s) AS memDieet,", $sqdieet);
		} else {
			$sqdieet = "";
		}
		
		$query = sprintf("SELECT 'Persoonsgegevens' AS Kop1, 
					%s AS Naam, 
					L.Voorletter AS Voorletters, 
					L.Roepnaam AS ndRoepnaam,
					%s AS Geslacht, 
					IF(NOT ISNULL(L.GEBDATUM), CONCAT(DATE_FORMAT(L.GEBDATUM, %s), IF(LENGTH(L.GEBPLAATS) > 1, CONCAT(' te ', L.GEBPLAATS), '')), '') AS Geboren, 
					L.Overleden AS `dteOverleden op`,
					%s AS Leeftijd,
					L.Burgerservicenummer,
					%s
					'Contactgegevens' AS Kop3, 
					L.Telefoon AS `Vast telefoonnummer`, 
					L.Mobiel, 
					L.Email AS `emlE-mail`,
					L.EmailVereniging AS `emlE-mail vereniging`,
					L.EmailOuders AS `emlE-mail ouders`,
					L.NamenOuders AS `Namen ouders`,
					'Lidmaatschap' AS Kop4,
					LM.Lidnr AS Lidnummer, 
					(%s) AS Onderscheiding, 
					LM.LIDDATUM AS `dteLid vanaf`, 
					LM.Opgezegd AS `dteLid totenmet`, 
					IF(DATEDIFF(CURDATE(), LM.LIDDATUM) < 500, NULL, CONCAT(REPLACE(ROUND(TIMESTAMPDIFF(MONTH, LM.LIDDATUM, IF(ISNULL(LM.Opgezegd), CURDATE(), LM.Opgezegd))/12, 1), '.', ','), ' jaar')) AS `Lengte lidmaatschap`,
					'Financieel' AS Kop5,
					L.`Machtiging afgegeven` AS `booMachtiging incasso`,
					L.Bankrekening,
					'Overig' AS Kop6,
					%s AS `Legitimatie`,
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
					ORDER BY LM.LIDDATUM DESC;", $selectnaam, $selectgeslacht, $fdlang, $selectleeftijd, $adresgegevens, $sqond, $sqllegitimatie, $VOG, $sqlbd, $wbn, $sqdieet, $fromlid, $LidID);
					//debug($query);
					
	} elseif ($soort == "Financieel") {
		$query = sprintf("SELECT L.Bankrekening AS Bankrekeningnummer, 
								L.`Machtiging afgegeven` AS `booMachtiging automatische incasso`
								FROM %sLid AS L
								WHERE L.Nummer=%d;", $fdlang, TABLE_PREFIX, $LidID);
								
	} elseif ($soort == "Afdelingen") {
		$query = sprintf("SELECT MAX(LO.Functie) FROM %1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON LO.OnderdeelID=O.RecordID WHERE O.`Type`='A' AND LO.Lid=%2\$d;", TABLE_PREFIX, $LidID);
		if (db_scalar($query) > 0) {
			$xtr_sel = "F.Omschrijv AS Functie, ";
		} else {
			$xtr_sel = "";
		}
		
		$query = sprintf("SELECT MAX(LO.OPMERK) FROM %1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON LO.OnderdeelID=O.RecordID WHERE O.`Type`='A' AND LO.Lid=%2\$d;", TABLE_PREFIX, $LidID);
		if (strlen(db_scalar($query)) > 0) {
			$xtr_sel .= "LO.OPMERK AS Opmerking, ";
		}
		
		$query = sprintf("SELECT MAX(LO.GroepID) FROM %sLidond AS LO WHERE LO.Lid=%d;", TABLE_PREFIX, $LidID);
		if (db_scalar($query) > 0) {
			$xtr_sel .= sprintf("(SELECT CONCAT(IF(LENGTH(GR.Starttijd)=5, CONCAT(GR.Starttijd, ' - '), ''), IF(LENGTH(GR.Omschrijving) > 0, GR.Omschrijving, GR.Kode)) FROM %sGroep AS GR WHERE GR.RecordID=LO.GroepID) AS Groep,", TABLE_PREFIX);
		}
		$query = sprintf("SELECT O.Kode AS Code,
					O.Naam,
					LO.Vanaf AS dteVanaf,
					%s
					LO.Opgezegd AS dteOpgezegd,
					CONCAT(FORMAT(DATEDIFF(IF(LO.Opgezegd IS NULL, CURDATE(), LO.Opgezegd), LO.Vanaf)/365.25, 1, 'nl_NL'), ' jaar') AS numDuur
					FROM %s
					WHERE O.`Type`='A' AND LO.Lid=%d
					ORDER BY LO.Vanaf DESC, O.Naam;", $xtr_sel, $fromlidond, $LidID);
					
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
					LO.Opgezegd AS dteTotenmet,
					CONCAT(FORMAT(DATEDIFF(IF(LO.Opgezegd IS NULL, CURDATE(), LO.Opgezegd), LO.Vanaf)/365.25, 1, 'nl_NL'), ' jaar') AS numDuur
					FROM %s
					WHERE O.`Type`='G' AND O.Kader=0 AND LO.Lid=%d
					ORDER BY IF(ISNULL(LO.Opgezegd), 0, 1), LO.Vanaf, O.Kode;", $fromlidond, $LidID);
					
	} elseif ($soort == "Toestemmingen") {
							
		$sq = sprintf("(SELECT IFNULL(MAX(LO.Vanaf), 'Nee') FROM %sLidond AS LO WHERE %s AND LO.Lid=%d AND LO.OnderdeelID=O.RecordID) AS `Afgegeven op`", TABLE_PREFIX, $wherelidond, $LidID);
		$sq2 = sprintf("(SELECT IFNULL(MAX(LO2.Vanaf), '') FROM %sLidond AS LO2 WHERE IFNULL(LO2.Opgezegd, '2999-12-31') < CURDATE() AND LO2.Lid=%d AND LO2.OnderdeelID=O.RecordID) AS `Laatste verlopen toestemming`", TABLE_PREFIX, $LidID);
		$query = sprintf("SELECT O.Naam AS Omschrijving, CONCAT(O.MaximaleLengtePeriode, ' maanden') AS Geldigheid,
					%1\$s,
					%2\$s
					FROM %3\$sOnderdl AS O
					WHERE IFNULL(O.VervallenPer, '2999-12-31') >= CURDATE() AND O.Type='T'", $sq, $sq2, TABLE_PREFIX);
					
	} elseif ($soort == "Lidbew") {
		$tw = sprintf("CONVERT((SELECT SUM(DATEDIFF(EINDE_PER, BEGIN_PER)+1) FROM %sBewaking AS BW WHERE BW.Lid = %d AND BEGIN_PER < SYSDATE())/7, SIGNED)", TABLE_PREFIX, $LidID);
		$ts = sprintf("(SELECT COUNT(DISTINCT SeizoenID) FROM %sBewaking AS BW WHERE BW.Lid = %d AND BEGIN_PER < CURDATE())", TABLE_PREFIX, $LidID);
		$query = sprintf("SELECT CONCAT(%1\$s, ' weken in ', %2\$s, ' seizoen(en)') AS `Totale bewaking`,
								(SELECT Memo FROM %3\$sMemo WHERE Soort='B' AND Lid=%4\$d) AS Opmerking,
								(SELECT Memo FROM %3\$sMemo WHERE Soort='D' AND Lid=%4\$d) AS Dieet;", $tw, $ts, TABLE_PREFIX, $LidID);

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
								ORDER BY BS.Begindatum DESC, BS.Lokatie, BW.BEGIN_PER DESC;", TABLE_PREFIX, $LidID, $xtra_where);

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
					ORDER BY Nummer DESC;", TABLE_PREFIX, $LidID);
	} elseif ($soort == "Mailing") {
		$query = db_mailing_hist("sqllijst", $LidID);
		
	} elseif ($soort == "Evenementen") {
		$query = "SELECT E.Datum AS dteDatum, E.Omschrijving, E.Verzameltijd, DATE_FORMAT(E.Datum, '%H:%i') AS Starttijd,
					E.Eindtijd, E.Email AS emlContact, ED.Opmerking, ED.Functie ";
		if (isset($dlnstatus)) {
			$query .= ", CASE ED.Status";
			foreach ($dlnstatus as $s => $o) {
				$query .= sprintf(" WHEN '%s' THEN '%s'", $s, $o);
			}
			$query .= " END AS Status ";
		}
		$query .= sprintf("FROM %1\$sEvenement AS E INNER JOIN %1\$sEvenement_Deelnemer AS ED ON E.RecordID = ED.EvenementID
								WHERE ED.Status IN ('B', 'I', 'R', 'X') AND ED.LidID=%2\$d
								ORDER BY E.Datum DESC;", TABLE_PREFIX, $LidID);
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
	global $selectnaam, $selectzoeknaam;
	
	if (strlen($uitv) == 0) {
		$uitv = "Naam";
	}
	
	if ($uitv == "Naam") {
		$query = sprintf("SELECT %s FROM %sLid AS L WHERE L.Nummer=%d;", $selectnaam, TABLE_PREFIX, $lidid);
	} elseif ($uitv == "Roepnaam") {
		$query = sprintf("SELECT IF(LENGTH(L.Roepnaam)=0, L.Voorletter, L.Roepnaam) FROM %sLid AS L WHERE L.Nummer=%d;", TABLE_PREFIX, $lidid);
	} else {
		$query = sprintf("SELECT %s FROM %sLid AS L WHERE L.Nummer=%d;", $selectzoeknaam, TABLE_PREFIX, $lidid);
	}
	$rv = db_scalar($query);
	if (strlen($rv) == 0) {
		$rv = $riz;
	}
	return $rv;
}

function db_lid($actie, $filter="", $lidid=0, $xtraselect="", $kolomnm="", $waarde="") {
	global $selectnaam, $selectzoeknaam, $selectlidnr, $fromlid, $wherelid, $selecttelefoon;
	
	if (strlen($filter) > 0 and ($actie != "verjaardagen" and $actie != "lididbijemail")) {
		$filter = " WHERE " . $filter;
	} elseif ($lidid > 0) {
		$filter = sprintf(" WHERE L.Nummer=%d", $lidid);
	}
	
	if ($actie == "record" and $lidid > 0) {
		$query = sprintf("SELECT *, %s as NaamLid FROM %sLid AS L WHERE Nummer=%d;", $selectnaam, TABLE_PREFIX, $lidid);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "aantal") {
		if ($lidid > 0) {
			$query = sprintf("SELECT COUNT(*) FROM %sLid WHERE Nummer=%d;", TABLE_PREFIX, $lidid);
		} else {
			$query = sprintf("SELECT COUNT(*) FROM %sLid;", TABLE_PREFIX);
		}
		return db_scalar($query);
		
	} elseif ($actie === "lijst") {
		$query = sprintf("SELECT L.Nummer, %1\$s AS `Naam_lid`, UPPER(L.Woonplaats) AS Woonplaats,
					L.Telefoon, L.Mobiel, IF((L.Email IS NULL), L.EmailOuders, L.Email) AS `emlE-mail`, L.GEBDATUM AS GEBDATUM, %2\$s AS Zoeknaam
					FROM %3\$sLid AS L
					%4\$s
					ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, L.Nummer DESC;", $selectnaam, $selectzoeknaam, TABLE_PREFIX, $filter);
					
	} elseif ($actie == "ledenlijst") {
		$query = sprintf("SELECT L.Nummer AS lnkNummer, LM.Lidnr AS numLidnr, %1\$s AS `Naam_lid`, UPPER(L.Woonplaats) AS Woonplaats,
					%6\$s AS Telefoon, IF((LENGTH(L.Email) > 5), L.Email, L.EmailOuders) AS `emlE-mail`%3\$s,
					L.Nummer AS llkNummer, L.GEBDATUM AS ndGEBDATUM, %2\$s AS ndZoeknaam
					FROM %4\$s
					%5\$s
					ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, LM.LIDDATUM DESC;", $selectnaam, $selectzoeknaam, $xtraselect, $fromlid, $filter, $selecttelefoon);
					
	} elseif ($actie == "lijstselect") {
		$query = sprintf("SELECT L.Nummer, %s AS NaamLid, %s AS Zoeknaam
					FROM %sLid AS L
					%s
					ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, L.GEBDATUM;", $selectnaam, $selectzoeknaam, TABLE_PREFIX, $filter);
					
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
					L.NamenOuders,
					L.GEBDATUM,
					L.GEBPLAATS,
					L.Bankrekening,
					L.`Machtiging afgegeven`,
					L.Burgerservicenummer,
					L.RelnrRedNed,
					L.Beroep,
					L.Legitimatietype,
					L.Legitimatienummer,
					L.`VOG afgegeven`
					FROM %sLid AS L WHERE Nummer=%d;", TABLE_PREFIX, $lidid);
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
					ORDER BY L.Achternaam, L.Tussenv;", $selectzoeknaam, TABLE_PREFIX);
					
	} elseif ($actie == "update") {
		if ($waarde == "NULL") {
			$query = sprintf("UPDATE %1\$sLid SET `%2\$s`=NULL WHERE Nummer=%3\$d AND `%2\$s`<>NULL;", TABLE_PREFIX, $kolomnm, $lidid);
		} elseif (is_numeric($waarde)) {
			$query = sprintf("UPDATE %1\$sLid SET `%2\$s`=%3\$s WHERE Nummer=%4\$d AND `%2\$s`<>%3\$s;", TABLE_PREFIX, $kolomnm, $waarde, $lidid);
		} else {
			$query = sprintf("UPDATE %1\$sLid SET `%2\$s`='%3\$s' WHERE BINARY Nummer=%4\$d AND `%2\$s`<>'%3\$s';", TABLE_PREFIX, $kolomnm, $waarde, $lidid);
		}
		if (fnQuery($query) > 0) {
			db_interface("add", $query, $lidid);
			return true;
		} else {
			return false;
		}

	} elseif ($actie == "lididbijemail") {
		$filter = strtolower($filter);
		$query = sprintf("SELECT IFNULL(MAX(L.Nummer), 0) FROM %1\$sLid AS L WHERE (LOWER(Email)='%2\$s' OR LOWER(EmailVereniging)='%2\$s' OR LOWER(EmailOuders)='%2\$s');", TABLE_PREFIX, $filter);
		return db_scalar($query);
		
	} elseif ($actie == "mingebdatum") {
		$query = sprintf("SELECT SUBSTRING(MIN(GEBDATUM), 1, 10) FROM %s WHERE %s;", $fromlid, $wherelid);
		return db_scalar($query);
		
	} elseif ($actie == "maxgebdatum") {
		$query = sprintf("SELECT SUBSTRING(MAX(GEBDATUM), 1, 10) FROM %s WHERE %s;", $fromlid, $wherelid);
		return db_scalar($query);
		
	} elseif ($actie == "minpostcode") {
		$query = sprintf("SELECT MIN(Postcode) FROM %s WHERE %s;", $fromlid, $wherelid);
		return db_scalar($query);
		
	} elseif ($actie == "maxpostcode") {
		$query = sprintf("SELECT MAX(Postcode) FROM %s WHERE %s;", $fromlid, $wherelid);
		return db_scalar($query);

	} elseif ($actie == "minliddatum") {
		$query = sprintf("SELECT SUBSTRING(MIN(LM.LIDDATUM), 1, 10) FROM %s WHERE %s;", $fromlid, $wherelid);
		return db_scalar($query);
		
	} elseif ($actie == "maxliddatum") {
		$query = sprintf("SELECT SUBSTRING(MAX(LM.LIDDATUM), 1, 10) FROM %s WHERE %s;", $fromlid, $wherelid);
		return db_scalar($query);
		
	} elseif ($actie == "minopgezegd") {
		$query = sprintf("SELECT SUBSTRING(MIN(LM.Opgezegd), 1, 10) FROM %s WHERE %s;", $fromlid, $wherelid);
		return db_scalar($query);
		
	} elseif ($actie == "maxopgezegd") {
		$query = sprintf("SELECT SUBSTRING(MAX(LM.Opgezegd), 1, 10) FROM %s WHERE %s;", $fromlid, $wherelid);
		return db_scalar($query);
		
	} else {
		$mess = sprintf("onbekende actie '%s' in db_lid", $actie);
		debug($mess, 1, 0, 1);
		$query = "";
	}
	
	$result = fnQuery($query);
	return $result->fetchAll();
} # db_lid

function db_lidmaatschap($actie, $lidid=0, $opgezegd="", $rid=0, $liddatum="", $lidnr=0) {
	global $fdaccess, $fdsql, $wherelid, $fdlang;

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
		$query = sprintf("SELECT LM.* FROM %sLidmaatschap AS LM WHERE %s ORDER BY LM.Lidnr;", TABLE_PREFIX, $wherelid);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "toekomstigeleden") {
		$query = sprintf("SELECT LM.* FROM %sLidmaatschap AS LM WHERE LM.LIDDATUM > CURDATE() ORDER BY LM.Lidnr;", TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "opzegging" and strlen($opgezegd) > 4) {
		$query = sprintf("UPDATE %sLidmaatschap SET Opgezegd='%s', Gewijzigd=SYSDATE() WHERE Lid=%d AND (Opgezegd IS NULL);", TABLE_PREFIX, $opgezegd, $lidid);
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
								WHERE LM.Lidnr=%d LIMIT 1;", $fdlang, TABLE_PREFIX, $lidnr);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "overzichtlid") {
		$query = sprintf("SELECT * FROM %sLidmaatschap
								WHERE Lid=%d
								ORDER BY LIDDATUM;", TABLE_PREFIX, $lidid);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "update" and $rid > 0) {
		$query = sprintf("SELECT * FROM %sLidmaatschap WHERE RecordID=%d AND Lid=%d;", TABLE_PREFIX, $rid, $lidid);
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
			$query = sprintf("UPDATE %sLidmaatschap SET %s Gewijzigd=SYSDATE() WHERE Lid=%d AND LIDDATUM='%s';", TABLE_PREFIX, $set_clause, $cur->Lid, $cur->LIDDATUM);
			$result = fnQuery($query);
			if ($result > 0) {
				$query = sprintf("UPDATE Lidmaatschap SET %s Gewijzigd=SYSDATE() WHERE Lid=%d AND LIDDATUM=%s;", $set_access, $cur->Lid, accessdate($cur->LIDDATUM));
				db_interface("add", $query, $lidid);
				$mess = sprintf("Lidmaatschap van lid %s is gewijzigd. %s", db_naamlid($lidid), $wijz);
				db_logboek("add", $mess, 6, $lidid);
			}
		}
	} elseif ($actie == "add" and $lidid > 0) {
		$query = sprintf("SELECT IFNULL(MAX(Lidnr), 100) FROM %sLidmaatschap;", TABLE_PREFIX);
		$lidnr = db_scalar($query) + 1;
		$liddatum = date("Y-m-d");
		$query = sprintf("INSERT INTO %sLidmaatschap (Lid, Lidnr, LIDDATUM, Opgezegd, Ingevoerd, Gewijzigd) VALUES (%d, %d, '%s', NULL, CURDATE(), SYSDATE());", TABLE_PREFIX, $lidid, $lidnr, $liddatum);
		$result = fnQuery($query);
		if ($result > 0) {
			$query = sprintf("INSERT INTO Lidmaatschap (Lid, Lidnr, LIDDATUM, Ingevoerd, Gewijzigd) VALUES (%d, %d, %s, CURDATE(), SYSDATE());", $lidid, $lidnr, accessdate($liddatum));
			db_interface("add", $query, $lidid);
			$mess = sprintf("Bij lid %s is een lidmaatschap met nummer %d toegevoegd.", db_naamlid($lidid), $lidnr);
			db_logboek("add", $mess, 6, $lidid);
		}
		
	} elseif ($actie == "soortlid") {
		$query = sprintf("SELECT * FROM %sLidmaatschap WHERE Lid=%d ORDER BY LIDDATUM DESC;", TABLE_PREFIX, $lidid);
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
	} elseif ($actie == "minlidnr") {
		$query = sprintf("SELECT MIN(Lidnr) FROM %sLidmaatschap AS LM WHERE ((LM.Opgezegd IS NULL) OR LM.Opgezegd > CURDATE());", TABLE_PREFIX);
		return db_scalar($query);
	} elseif ($actie == "maxlidnr") {
		$query = sprintf("SELECT MAX(Lidnr) FROM %sLidmaatschap AS LM WHERE ((LM.Opgezegd IS NULL) OR LM.Opgezegd > CURDATE());", TABLE_PREFIX);
		return db_scalar($query);
	}
	
} # db_lidmaatschap

function db_memo($actie, $lidid, $srt, $nv="") {
	
	$nv = str_replace("\"", "", str_replace("'", "", $nv));	
	
	if ($actie == "curval" and $lidid > 0) {
		if ($srt == "WN") {
			$query = sprintf("SELECT `Waarschuwen bij nood` FROM %sLid WHERE Nummer=%d;", TABLE_PREFIX, $lidid);
		} else {
			$query = sprintf("SELECT `Memo` FROM %sMemo WHERE Lid=%d AND Soort='%s';", TABLE_PREFIX, $lidid, $srt);
		}
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "add" and strlen($nv) > 0) {
		$query = sprintf("INSERT INTO %sMemo (Lid, Soort, Memo, Ingevoerd, Gewijzigd) VALUES (%d, '%s', \"%s\", CURDATE(), SYSDATE());", TABLE_PREFIX, $lidid, $srt, $nv);
		$result = fnQuery($query);
		if ($result > 0) {
			db_interface("add", $query, $lidid);
			$mess = sprintf("Memo '%s' is bij %s met waarde '%s' toegevoegd.", ARRSOORTMEMO[$srt], db_naamlid($lidid), $nv);
		} else {
			$mess = sprintf("Het toevoegen van memo '%s' is bij %s met waarde '%s' is mislukt.", ARRSOORTMEMO[$srt], db_naamlid($lidid), $nv);
		}
		db_logboek("add", $mess, 6, $lidid);
		
	} elseif ($actie == "update") {
		if ($srt == "WN") {
			$query = sprintf("UPDATE %sLid SET `Waarschuwen bij nood`=\"%s\", Gewijzigd=SYSDATE() WHERE Nummer=%d;", TABLE_PREFIX, $nv, $lidid);
		} else {
			$query = sprintf("UPDATE %sMemo SET Memo=\"%s\", Gewijzigd=SYSDATE() WHERE Lid=%d AND Soort='%s';", TABLE_PREFIX, $nv, $lidid, $srt);
		}
		$result = fnQuery($query);
		if ($result > 0) {
			db_interface("add", $query, $lidid);
			$mess = sprintf("Memo '%s' is bij %s in '%s' gewijzigd.", ARRSOORTMEMO[$srt], db_naamlid($lidid), $nv);
			db_logboek("add", $mess, 6, $lidid);
		}
		
	} elseif ($actie == "delete") {
		$query = sprintf("DELETE FROM %sMemo WHERE Lid=%d AND Soort='%s';", TABLE_PREFIX, $lidid, $srt);
		$result = fnQuery($query);
		if ($result > 0) {
			db_interface("add", $query, $lidid);
			$mess = sprintf("Memo '%s' is bij %s verwijderd.", ARRSOORTMEMO[$srt], db_naamlid($lidid));
			db_logboek("add", $mess, 6, $lidid, 1);
		}
	
	} elseif ($actie == "aantal") {
		if (strlen($srt) > 0) {
			$query = sprintf("SELECT COUNT(*) FROM %sMemo WHERE Soort='%s';", TABLE_PREFIX, $srt);
		} else {
			$query = sprintf("SELECT COUNT(*) FROM %sMemo;", TABLE_PREFIX);
		}
		return db_scalar($query);
	}
} # db_ledenwijzigingen

function db_authorisation($actie, $recid=0, $tabpage="") {
	global $sqllidgroepen;
	
	$tabpage = str_replace("'", "", $tabpage);
	if ($actie == "delete") {
		if ($_SESSION['webmaster'] == 1) {
			$query = sprintf("SELECT Tabpage FROM %sAdmin_access WHERE RecordID=%d;", TABLE_PREFIX, $recid);
			$tp = db_scalar($query);
			$query = sprintf("DELETE FROM %sAdmin_access WHERE RecordID=%d;", TABLE_PREFIX, $recid);
			if (fnQuery($query) > 0) {
				$mess = sprintf("Uit de tabel '%sAdmin_access' is record %d (%s) verwijderd.", TABLE_PREFIX, $recid, $tp);
			} else {
				$mess = sprintf("In tabel '%sAdmin_access' bestaat record %d niet.", TABLE_PREFIX, $recid);
			}
		} else {
			$mess = "Je hebt geen rechten om autorisaties te verwijderen.";
		}
		db_logboek("add", $mess, 15, 0, 1);
	} elseif ($actie == "lijst") {
		$query = sprintf("SELECT RecordID, Toegang, Tabpage, Ingevoerd FROM %sAdmin_access ORDER BY Tabpage;", TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "autorisatiesperonderdeel") {
		$query = sprintf("SELECT O.Naam AS Onderdeel, A.Tabpage as `Toegang tot` FROM %sAdmin_access AS A INNER JOIN %1\$sOnderdl AS O ON A.Toegang=O.RecordID ORDER BY O.Naam, A.Tabpage;", TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "tabpages") {
		$query = sprintf("SELECT DISTINCT Tabpage FROM %sAdmin_access WHERE Toegang > 0 ORDER BY Tabpage;", TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "add") {
		$query = sprintf("INSERT INTO %sAdmin_access SET Tabpage=\"%s\";", TABLE_PREFIX, $tabpage);
		$result = fnQuery($query);
		if ($tabpage == "Vereniging/Introductie") {
			$query = sprintf("UPDATE %sAdmin_access SET Toegang=0 WHERE Tabpage='Vereniging/Introductie';", TABLE_PREFIX);
			$result = fnQuery($query);
		}
		$mess = sprintf("Tabpage '%s' is aan de tabel 'Admin_access' toegevoegd.", $tabpage);
		db_logboek("add", $mess, 15, 0, 0, $result);
		
	} elseif ($actie == "update") {  // Nog niet af en nog niet in gebruik
		$query = sprintf("UPDATE %1\$sAdmin_access SET Toegang=%2\$d WHERE RecordID=%3\$d AND Toegang<>%2\$d;", TABLE_PREFIX, $_POST[$vn], $recid);
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
			$query = sprintf("SELECT COUNT(*) FROM %sAdmin_access WHERE Tabpage=\"%s\" AND (Toegang IN (%s) OR Toegang=0);", TABLE_PREFIX, $tabpage, $sqllidgroepen);
			if (db_scalar($query) > 0) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
		
	} elseif ($actie == "opschonen") {
		$query = sprintf("DELETE FROM %sAdmin_access WHERE Ingevoerd < DATE_ADD(CURDATE(), INTERVAL -3 MONTH) AND Toegang < 0;", TABLE_PREFIX);
		$result = fnQuery($query, 15);
	}
} # db_authorisation

function db_bewseiz($actie) {
	
	if ($actie == "aantal") {
		$query = sprintf("SELECT COUNT(*) FROM %sBewseiz;", TABLE_PREFIX);
		return db_scalar($query);
	}
	
} # db_bewseiz

function db_bewaking($actie, $seizoen=-1, $lidid=-1, $week=-1) {
	global $selectnaam;
	
	if ($actie == "aantallen" and $lidid > -1) {
		if ($seizoen > 0) {
			$xtrawhere = sprintf("AND BW.SeizoenID=%d", $seizoen);
		} else {
			$xtrawhere = "";
		}
		$query = sprintf("SELECT COUNT(*) FROM %sBewaking AS BW WHERE BW.Lid=%d %s;", TABLE_PREFIX, $lidid, $xtrawhere);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "huidigeweek") {
		$query = sprintf("SELECT IFNULL(MIN(WeekNr), 0) FROM %sBewaking
								WHERE SeizoenID=%d AND BEGIN_PER <= CURDATE() AND EINDE_PER >= CURDATE();", TABLE_PREFIX, $seizoen);
		return db_scalar($query);
		
	} else {
		if ($actie == "aantallen") {
			$query = sprintf("SELECT ST_LEN_BP FROM %sBewseiz WHERE RecordID=%d;", TABLE_PREFIX, $seizoen);
			$result = fnQuery($query);
			if ($result->fetchColumn() == 7) {
				$query = sprintf("SELECT Weeknr AS intWeeknr, MIN(BEGIN_PER) AS dteBegin, MAX(EINDE_PER) AS dteEinde, ROUND(SUM(DATEDIFF(BW.EINDE_PER, BW.BEGIN_PER)+1)/7, 0) AS intAantal,
							CONCAT(ROUND(AVG((SELECT IFNULL(SUM(DATEDIFF(BH.EINDE_PER, BH.BEGIN_PER)+1)/7, 0) FROM %1\$sBewaking AS BH WHERE BH.Lid = BW.Lid AND BH.EINDE_PER < BW.BEGIN_PER)), 1), ' weken') AS `numGem. Ervaring`,
							CONCAT(ROUND(AVG(DATEDIFF(BW.BEGIN_PER, L.GEBDATUM))/365.25, 1), ' jaar') AS `numGem. Leeftijd`
							FROM %1\$sBewaking AS BW INNER JOIN %1\$sLid AS L ON BW.Lid = L.Nummer
							WHERE BW.Weeknr > 0 AND BW.SeizoenID=%2\$d	
							GROUP BY BW.Weeknr
							ORDER BY BW.Weeknr;", TABLE_PREFIX, $seizoen);
			} else {
				$query = sprintf("SELECT BEGIN_PER AS dtlDatum, COUNT(Lid) AS intAantal,
							CONCAT(ROUND(AVG((SELECT IFNULL(SUM(DATEDIFF(BH.EINDE_PER, BH.BEGIN_PER)+1), 0) FROM %1\$sBewaking AS BH WHERE BH.Lid = BW.Lid AND BH.EINDE_PER < BW.BEGIN_PER)), 1), ' dagen') AS `numGem. Ervaring`,
							CONCAT(ROUND(AVG(DATEDIFF(BW.BEGIN_PER, L.GEBDATUM))/365.25, 1), ' jaar') AS `numGem. Leeftijd`
							FROM %1\$sBewaking AS BW INNER JOIN %1\$sLid AS L ON BW.Lid = L.Nummer
							WHERE BW.SeizoenID=%2\$d
							GROUP BY BW.BEGIN_PER
							ORDER BY BW.BEGIN_PER;", TABLE_PREFIX, $seizoen);
			}
	
		} elseif ($actie == "insseizoenen") {
			$query = sprintf("SELECT DISTINCT BS.RecordID, BS.Kode, BS.Gewijzigd, BS.ST_LEN_BP
									FROM %1\$sBewseiz AS BS INNER JOIN %1\$sBewaking_Blok AS BB ON BS.RecordID = BB.SeizoenID
									ORDER BY BS.Begindatum DESC, BS.Kode;", TABLE_PREFIX);
									
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
				$query = sprintf("SELECT ST_LEN_BP FROM %sBewseiz WHERE RecordID=%d;", TABLE_PREFIX, $seizoen);
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
								ORDER BY %5$s;', $selectnaam, TABLE_PREFIX, $sel_sz, $sel_wk, $orderby);
									
		} elseif ($actie == "seizoenen") {
			$query = sprintf("SELECT BS.RecordID, BS.Kode, BS.Gewijzigd, BS.ST_LEN_BP FROM %sBewseiz AS BS ORDER BY BS.Begindatum DESC, BS.Kode;", TABLE_PREFIX);
		}
		$result = fnQuery($query);
		return $result;
	}
}

function db_bewaking_blok($actie, $seizoen=-1, $blokid=0, $field="", $val="") {
	global $selectnaam, $fdsql;
	
	$w = "";
	if ($seizoen > 0) {
		$w = sprintf("WHERE BB.SeizoenID=%d", $seizoen);
	}
	
	if ($actie == "overzicht") {
		$query = sprintf("SELECT BB.*, BS.Kode AS KodeSeizoen, BS.Lokatie AS Locatie, BS.KeuzesBijInschrijving, BB.Kode AS Weeknr, BB.Kode AS intWeeknr
								FROM %1\$sBewaking_Blok AS BB LEFT OUTER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								%2\$s
								ORDER BY BS.Begindatum, BS.Kode, BB.Begindatum;", TABLE_PREFIX, $w);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "openblokken") {
		$query = sprintf("SELECT BB.*, BS.Kode AS KodeSeizoen, BS.Lokatie AS Locatie, BS.KeuzesBijInschrijving
								FROM %1\$sBewaking_Blok AS BB INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								WHERE BB.InschrijvingOpen=1 AND BB.Einddatum >= CURDATE()
								ORDER BY BS.Begindatum, BS.Kode, BB.Begindatum;", TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "omsblok") {
		$query = sprintf("SELECT CONCAT(BS.Kode, ' / ', IF(ISNULL(BB.Kode), BB.Begindatum, BB.Kode)) AS OmsBlok
								FROM %1\$sBewaking_Blok AS BB LEFT OUTER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								WHERE BB.RecordID=%2\$d;", TABLE_PREFIX, $blokid);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "ingebruik") {
		$query = sprintf("SELECT COUNT(*) FROM %sBewaking_Inschrijving WHERE BlokID=%d;", TABLE_PREFIX, $blokid);
		$result = fnQuery($query);
		if ($result->fetchColumn() > 0) {
			return true;
		} else {
			return false;
		}
		
	} elseif ($actie == "add" and $seizoen > 0) {
		$query = sprintf("SELECT DATE_ADD(BB.Einddatum, INTERVAL 1 DAY) AS B, DATE_ADD(BB.Einddatum, INTERVAL BS.ST_LEN_BP DAY) AS E, BB.Kode
								FROM %1\$sBewaking_Blok AS BB INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								WHERE BB.SeizoenID=%2\$d AND BB.Einddatum > '2001-01-01' ORDER BY BB.Einddatum DESC;", TABLE_PREFIX, $seizoen);
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
		$query = sprintf("INSERT INTO %sBewaking_Blok SET SeizoenID=%d, Kode='%s', Begindatum='%s', Einddatum='%s';", TABLE_PREFIX, $seizoen, $k, $b, $e);
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
		
		$query = sprintf("UPDATE %sBewaking_Blok SET %s WHERE RecordID=%d;", TABLE_PREFIX, $set_clause, $blokid);
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
			$query = sprintf("DELETE FROM %sBewaking_Blok WHERE RecordID='%d';", TABLE_PREFIX, $blokid);
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
	global $selectnaam;
	
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
								ORDER BY BS.Begindatum, BS.Kode, L.Achternaam, BI.Lid, BB.Begindatum;", TABLE_PREFIX, $selectnaam, $w);
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
								ORDER BY BS.Kode, BB.Kode, BB.Begindatum;", TABLE_PREFIX, $w);
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
								ORDER BY BS.Kode, BB.Kode, BB.Begindatum;", TABLE_PREFIX, $selectnaam, $w);
		$result = fnQuery($query);
		return $result->fetchAll();	
		
	} elseif ($actie == "laatstelid") {
		$query = sprintf("SELECT IFNULL(MAX(Nummer), 0) FROM %sBewaking_Inschrijving WHERE Lid=%d;", TABLE_PREFIX, $lidid);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "inschrijving") {
		$query = sprintf("SELECT BI.* FROM %sBewaking_Inschrijving AS BI
								WHERE BI.Lid=%d AND BI.BlokID=%d;", TABLE_PREFIX, $lidid, $blok);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "export") {
		$query = sprintf("SELECT %1\$s AS NaamLid, BI.Lid, BB.SeizoenID, BB.Begindatum, BB.Einddatum
								FROM (%2\$sBewaking_Inschrijving AS BI INNER JOIN %2\$sLid AS L ON BI.Lid = L.Nummer) INNER JOIN %2\$sBewaking_Blok AS BB ON BI.BlokID = BB.RecordID
								WHERE IFNULL(BI.Afgemeld, '0000-00-00') < BI.Ingevoerd AND BI.Definitief > BI.Ingevoerd AND BI.Keuze IN (1, 3)
								ORDER BY L.Achternaam, L.Tussenv, L.Roepnaam, BB.Begindatum;", $selectnaam, TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "add" and $lidid > 0 and $blok > 0 and $insnr > 0) {
		$query = sprintf("INSERT INTO %sBewaking_Inschrijving (BlokID, Lid, Nummer, Keuze, Opmerking, Ingevoerd)
								VALUES (%d, %d, %d, %d, \"%s\", CURDATE());", TABLE_PREFIX, $blok, $lidid, $insnr, $keuze, $opmerk);
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
								WHERE BlokID=%d AND Lid=%d;", TABLE_PREFIX, $insnr, $keuze, $opmerk, $blok, $lidid);
		fnQuery($query);
		$mess = sprintf("Inschrijving bewaking voor blok '%s' is bij ingelogd lid bijgewerkt. Keuze is %d.", db_bewaking_blok("omsblok", 0, $blok), $keuze);
		if (strlen($opmerk) > 0) {
			$mess .= sprintf(" Opmerking is '%s'.", $opmerk);
		}
		db_logboek("add", $mess, 11, 0, 0, $blok);

	} elseif ($actie == "delete" and $lidid > 0 and $blok > 0) {
		$query = sprintf("DELETE FROM %sBewaking_Inschrijving WHERE BlokID=%d AND Lid=%d;", TABLE_PREFIX, $blok, $lidid);
		fnQuery($query);
		$mess = sprintf("Inschrijving bewaking voor blok '%s' is bij %s verwijderd.", db_bewaking_blok("omsblok", 0, $blok), db_naamlid($lidid));
		db_logboek("add", $mess, 11, 0, 0, $blok);
		
	} elseif ($actie == "defmaken" and $lidid > 0 and $insnr > 0) {
		$query = sprintf("UPDATE %sBewaking_Inschrijving SET Definitief=SYSDATE() WHERE Nummer=%d AND Lid=%d;", TABLE_PREFIX, $insnr, $lidid);
		$result = fnQuery($query);
		return $result;
		
	} elseif ($actie == "afmelden") {
		$query = sprintf("UPDATE %sBewaking_Inschrijving AS BI SET Afgemeld=SYSDATE()
					WHERE IFNULL(BI.Afgemeld, '0000-00-00') < BI.Ingevoerd AND BI.Definitief > BI.Ingevoerd AND BI.Keuze IN (1, 3);", TABLE_PREFIX);
		$result = fnQuery($query);
		$mess = sprintf("Er zijn %d inschrijvingen voor bewaking afgemeld.", $result);
		db_logboek("add", $mess, 11);
		return $mess;
		
	} else {
		debug(sprintf("Onbekende actie '%s' in db_insbew", $actie));
	}
}

function db_onderdeel($actie, $type="", $lidid=0, $onderdlid=0, $kolomnaam="", $waarde="", $per="") {
	global $fromlidond, $wherelidond;
	
	if (strlen($per) >= 10) {
		$wherelidond = sprintf("LO.Vanaf <= '%1\$s' AND IFNULL(LO.Opgezegd, '2999-12-31') >= '%1\$s'", date("Y-m-d", strtotime($per)));
	}
	
	$where = "";
	if ($type == "bw") {
		$where = "O.`Tonen in bewakingsadministratie`=1";
	} elseif ($type == "kad") {
		$where = "O.Kader=1";
	} elseif (strlen($type) > 0) {
		$where = sprintf("O.`Type`='%s'", $type);
	} elseif ($onderdlid > 0) {
		$where = sprintf("O.RecordID=%d", $onderdlid);
	}
	
	
	if ($actie == "lijstingebruik" or $actie == "aantalingebruik" or $lidid > 0) {
		if (strlen($where) > 0) {
			$where .= " AND ";
		}
		if ($lidid > 0) {
			$where .= sprintf("O.RecordID IN (SELECT LO.OnderdeelID FROM %sLidond AS LO WHERE %s AND LO.Lid=%d)", TABLE_PREFIX, $wherelidond, $lidid);
		} else {
			$where .= sprintf("O.RecordID IN (SELECT LO.OnderdeelID FROM %sLidond AS LO WHERE %s)", TABLE_PREFIX, $wherelidond);
		}
	}
	
	if ($actie == "aantal" or $actie == "aantalingebruik") {
		$sel = "COUNT(*)";
	} elseif ($actie == "type") {
		$sel = "O.Type";
	} elseif ($actie == "Naam" or $actie == "GekoppeldAanQuery") {
		$query = sprintf("SELECT `%s` FROM %sOnderdl WHERE RecordID=%d;", $actie, TABLE_PREFIX, $onderdlid);
		return db_scalar($query);
	} elseif ($actie == "vorige") {
		$query = sprintf("SELECT IFNULL(O.RecordID, 0) FROM %sOnderdl AS O WHERE O.Naam < '%s' AND O.Type='%s' ORDER BY O.Naam DESC", TABLE_PREFIX, db_onderdeel("Naam", "", 0, $onderdlid), $type);
		$result = fnQuery($query);
		return $result->fetchColumn();
	} elseif ($actie == "volgende") {
		$query = sprintf("SELECT IFNULL(O.RecordID, 0) FROM %sOnderdl AS O WHERE O.Naam > '%s' AND O.Type='%s' ORDER BY O.Naam", TABLE_PREFIX, db_onderdeel("Naam", "", 0, $onderdlid), $type);
		$result = fnQuery($query);
		return $result->fetchColumn();
	} elseif ($actie == "basislijst") {
		$sel = "*";
	} elseif ($actie == "editlijst") {
		$sel = sprintf("RecordID, Kode, Naam, (SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.OnderdeelID=O.RecordID AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd > CURDATE())) AS `Aantal leden`, RecordID AS llkRecordID", TABLE_PREFIX);
	} else {
		$sel = sprintf("DISTINCT O.RecordID, CONCAT(LEFT(O.Naam, 30), ' (', O.Kode, ')') AS Oms, O.Naam, O.Kode, (SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.OnderdeelID=O.RecordID AND %s) AS Aantal", TABLE_PREFIX, $wherelidond);
	}
	
	if (strlen($where) > 0) {
		$where = "WHERE " . $where;
	}
	
	if ($actie == "add" and strlen($type) == 1) {
		$nid = db_nieuwrecordID("Onderdl");
		$query = sprintf("INSERT INTO %sOnderdl (RecordID, Kode, Type, Naam, GekoppeldAanQuery) VALUES (%d, '%s', '%s', 'Nieuw', 0);", TABLE_PREFIX, $nid, "Nw" . $nid, $type);
		$result = fnQuery($query, 99);
		if ($result > 0) {
			db_interface("add", $query, 0);
		}
		return $nid;
		
	} elseif ($actie == "update") {
		$query = sprintf("UPDATE %1\$sOnderdl SET `%2\$s`='%3\$s' WHERE BINARY RecordID=%4\$d AND `%2\$s` <> '%3\$s';", TABLE_PREFIX, $kolomnaam, $waarde, $onderdlid);
		$result = fnQuery($query);
		if ($result > 0) {
			$sql = sprintf("UPDATE Onderdl SET [%1\$s]='%2\$s', Gewijzigd=Now() WHERE BINARY RecordID=%3\$d AND [%1\$s] <> '%2\$s';", $kolomnaam, $waarde, $onderdlid);
			db_interface("add", $sql, 0);
		}
		return $result;
		
	} elseif ($actie == "lijstopkode" or $actie == "basislijst") {
		$query = sprintf("SELECT %s FROM %sOnderdl AS O %s ORDER BY O.Kode;", $sel, TABLE_PREFIX, $where);
	} elseif ($actie == "lijstingebruik") {
		$query = sprintf("SELECT %s FROM %sOnderdl AS O %s ORDER BY O.Type, O.Naam;", $sel, TABLE_PREFIX, $where);
	} else {
		$query = sprintf("SELECT %s FROM %sOnderdl AS O %s ORDER BY O.Naam, O.Kode;", $sel, TABLE_PREFIX, $where);
	}
	
	if ($actie == "aantal" or $actie == "aantalingebruik" or $actie == "type") {
		return db_scalar($query);
	} else {
		$result = fnQuery($query);
		return $result->fetchAll();
	}
} # db_onderdeel

function db_functie($actie) {
	
	if ($actie == "basislijst") {
		$query = sprintf("SELECT * FROM %sFunctie;", TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchAll();
	}
	
} # db_functie

function db_lidond($actie, $lidid=0, $per="", $filter="", $onderdlid=0, $loid=0) {
	global $selectnaam, $selectzoeknaam, $selectgeslacht, $selecttelefoon, $selectleeftijd, $fromlidond, $wherelidond, $selectlidnr, $fdaccess, $lididtestusers;
	
	$mess = "";
	
	if (strlen($per) >= 10) {
		$wherelidond = sprintf("LO.Vanaf <= '%1\$s' AND IFNULL(LO.Opgezegd, '2999-12-31') >= '%1\$s'", date("Y-m-d", strtotime($per)));
	}
//	debug($actie);
	
	if (strlen($filter) > 0 and $onderdlid > 0) {
		$filter .= sprintf(" AND LO.OnderdeelID=%d", $onderdlid);
	} elseif ($onderdlid > 0) {
		$filter = sprintf("LO.OnderdeelID=%d", $onderdlid);
	}

	if ($actie == "aantal") {
		$query = sprintf("SELECT COUNT(*) FROM %sLidond;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "laatstgewijzigd") {
		$query = sprintf("SELECT Max(Gewijzigd) FROM %sLidond;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "bewaking") {
		$query  = sprintf("SELECT DISTINCT O.Kode FROM %s
						   WHERE O.`Tonen in bewakingsadministratie`=1 AND LO.Lid=%d AND %s
						   ORDER BY O.Kode;", $fromlidond, $lidid, $wherelidond);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "onderdeellijst") {
		if (strlen($filter) > 0) {
			$filter = " WHERE " . $filter;
		}
		
		$query = sprintf("SELECT MAX(LO.Functie) FROM %s %s;", $fromlidond, $filter);
		if (db_scalar($query) == 0) {
			$xtr_sel = "";
		} else {
			$xtr_sel = ", F.Omschrijv AS Functie";
		}
		
		$query = sprintf("SELECT MAX(LO.GroepID) FROM %s %s;", $fromlidond, $filter);
		if (db_scalar($query) > 0) {
			$xtr_sel .= sprintf(", (SELECT GR.Kode FROM %sGroep AS GR WHERE LO.GroepID=GR.RecordID) AS Groep", TABLE_PREFIX);
		}
		
		$query = sprintf("SELECT MAX(LO.OPMERK) FROM %s %s;", $fromlidond, $filter);
		if (strlen(db_scalar($query)) > 0) {
			$xtr_sel .= ", LO.OPMERK AS Opmerking";
		}
		$xtr_sel .= ", LO.Vanaf AS dteVanaf";
		
		$query = sprintf("SELECT MAX(LO.Opgezegd) FROM %s %s;", $fromlidond, $filter);
		if (db_scalar($query) > '2001-01-01') {
			$xtr_sel .= ", LO.Opgezegd AS dteTotenmet";
		}
		
		$query = sprintf("SELECT L.Nummer AS lnkNummer, IFNULL((%5\$s), 0) AS numLidnr, %1\$s AS `Naam_lid`, UPPER(L.Woonplaats) AS Woonplaats,
								L.Mobiel, IF(LENGTH(L.Email) > 5, L.Email, L.EmailOuders) AS `emlE-mail`%6\$s, L.Nummer AS llkNummer, L.GEBDATUM AS ndGEBDATUM, %2\$s AS ndZoeknaam
								FROM %3\$s
								%4\$s
								ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", $selectnaam, $selectzoeknaam, $fromlidond, $filter, $selectlidnr, $xtr_sel);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "selectielijst") {
		if (strlen($filter) > 0) {
			$filter = " WHERE " . $filter;
		}
		$xtra_sel = "";
		$query = sprintf("SELECT MAX(LO.OPMERK) FROM %s %s;", $fromlidond, $filter);
		if (strlen(db_scalar($query)) > 0) {
			$xtra_sel .= ", LO.OPMERK AS Opmerking";
		}
		$xtra_sel .= ", LO.Vanaf AS dteVanaf";
		
		$query = sprintf("SELECT L.Nummer AS lnkNummer, IFNULL((%5\$s), 0) AS numLidnr, %1\$s AS `Naam_lid`, L.Woonplaats, %7\$s AS Geslacht, %8\$s AS Leeftijd, %9\$s AS Telefoon,
								IF(LENGTH(L.Email) > 5, L.Email, L.EmailOuders) AS `emlE-mail`%6\$s, L.Nummer AS llkNummer, L.GEBDATUM AS ndGEBDATUM
								FROM %3\$s
								%4\$s
								ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", $selectnaam, $selectzoeknaam, $fromlidond, $filter, $selectlidnr, $xtra_sel, $selectgeslacht, $selectleeftijd, $selecttelefoon);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "actueleleden") {
		if (strlen($filter) > 0) {
			$filter = $wherelidond . " AND " . $filter;
		} else {
			$filter = $wherelidond;
		}
		
		if ($onderdlid > 0) {
			$filter .= sprintf(" AND LO.OnderdeelID=%d", $onderdlid);
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
		
	} elseif ($actie == "selectieleden") {
		$query = sprintf("SELECT L.Nummer, %s AS NaamLid FROM %s WHERE %s AND LO.OnderdeelID=%d ORDER BY L.Achternaam, L.Tussenv, L.Roepnaam, L.GEBDATUM;", $selectnaam, $fromlidond, $wherelidond, $onderdlid);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "kaderonderdeel") {
		$query = sprintf("SELECT IF(O.`Type`='F', F.OMSCHRIJV, O.Naam) AS Ond FROM %s WHERE %s AND LO.Lid=%d AND O.Kader=1;", $fromlidond, $wherelidond, $lidid);
		return db_scalar($query);
		
	} elseif ($actie == "onderscheiding") {
		$query = sprintf("SELECT O.Naam FROM %1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON LO.OnderdeelID=O.RecordID
					WHERE LO.Lid=%2\$d AND O.`Type`='O' AND (LO.Opgezegd IS NULL) ORDER BY LO.Vanaf DESC;", TABLE_PREFIX, $lidid);
		return db_scalar($query);
		
	} elseif ($actie == "islid") {
		$query = sprintf("SELECT COUNT(*) FROM %sLidond AS LO WHERE %s AND Lid=%d AND OnderdeelID=%d;", TABLE_PREFIX, $wherelidond, $lidid, $onderdlid);
		if (db_scalar($query) == 0) {
			return false;
		} else {
			return true;
		}
		
	} elseif ($actie == "maxgewijzigd") {
		if (strlen($filter) > 0) {
			$filter = " WHERE " . $filter;
		}
		$query = sprintf("SELECT MAX(LO.Gewijzigd) FROM %1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON LO.OnderdeelID=O.RecordID%2\$s;", TABLE_PREFIX, $filter);
		return db_scalar($query);
	
	} elseif ($actie == "add") {
		if (db_onderdeel("aantal", "", 0, $onderdlid) == 0) {
			$mess = sprintf("OnderdeelID %d bestaat niet. Dit record wordt niet toegevoegd.", $onderdlid);
			debug($mess, 99);
		
		} elseif (is_array($lidid) and count($lidid) > 0) {
			
			
			
		} elseif (db_lid("aantal", "", $lidid) == 0) {
			$mess = sprintf("Lid %d bestaat niet. Dit record wordt niet toegevoegd.", $lidid);
			debug($mess, 99);
		} else {
			$nrid = db_nieuwrecordID("Lidond");
			
			$query = sprintf("INSERT INTO %sLidond (RecordID, Lid, OnderdeelID, Vanaf, Ingevoerd, Gewijzigd) VALUES (%d, %d, %d, CURDATE(), CURDATE(), SYSDATE());", TABLE_PREFIX, $nrid, $lidid, $onderdlid);
			$result = fnQuery($query);
			if ($result > 0 and in_array($lidid, $lididtestusers) == false) {
				db_interface("add", $query, $lidid);
				return true;
			} else {
				return false;
			}
		}
		
	} elseif ($actie == "update") { // was delete door invullen opgezegd
		$query = sprintf("UPDATE %sLidond SET Opgezegd=DATE_SUB(CURDATE(), INTERVAL 1 DAY), Gewijzigd=SYSDATE() WHERE Lid=%d AND OnderdeelID=%d AND ((Opgezegd IS NULL) OR Opgezegd >= CURDATE());", TABLE_PREFIX, $lidid, $onderdlid);
		$result = fnQuery($query);
		if ($result > 0) {
			$sql = sprintf("UPDATE Lidond SET Opgezegd=#%1\$s#, Gewijzigd=Now() WHERE Lid=%2\$d AND OnderdeelID=%3\$d AND ((Opgezegd IS NULL) OR Opgezegd > #%1\$s#);", strftime($fdaccess), $lidid, $onderdlid);
			db_interface("add", $sql, $lidid);
		}
		return $result;
		
	} elseif ($actie == "delete") {
		
		if ($loid > 0) {
			$where = sprintf("WHERE RecordID=%d", $loid);
		} elseif (is_array($lidid)) {
			$where = sprintf("WHERE Lid IN (%s) AND OnderdeelID=%d", implode(", ", $lidid), $onderdlid);
		} elseif (is_numeric($lidid) == false and startwith($lidid, "(") and substr($lidid, -1) == ")") {
			$where = sprintf("WHERE Lid IN %s AND OnderdeelID=%d", $lidid, $onderdlid);
		} else {
			$where = sprintf("WHERE Lid=%d AND OnderdeelID=%d", $lidid, $onderdlid);
		}
		
		$query = sprintf("DELETE FROM %sLidond %s;", TABLE_PREFIX, $where);
		$result = fnQuery($query);
		if ($result > 0) {
			db_interface("add", $query, $lidid);
		}
		return $result;
		
	} elseif ($actie == "opschonen") {
		$query = sprintf("DELETE FROM %sLidond WHERE Lid=%d AND Vanaf >= Opgezegd;", TABLE_PREFIX, $lidid);
		fnQuery($query);
		
		foreach(db_onderdeel("lijst", "S") as $row) {
			$query = sprintf("DELETE FROM %sLidond WHERE OnderdeelID=%d AND (Vanaf > CURDATE() OR ((NOT Opgezegd IS NULL) AND Opgezegd <= CURDATE()));", TABLE_PREFIX, $row->RecordID);
			fnQuery($query);
		}

	} elseif ($actie == "lidgroepen") {
		if ($lidid <= 0) {
			$lidid = $_SESSION['lidid'];
		}
		$query = sprintf("SELECT OnderdeelID FROM %sLidond AS LO WHERE Lid=%d AND %s;", TABLE_PREFIX, $lidid, $wherelidond);
		$result = fnQuery($query);
		$rows = $result->fetchAll(PDO::FETCH_COLUMN);
		if (count($rows) > 0) {
			return "(0, " . implode(", ", $rows) . ")";
		} else {
			return "(0)";
		}
		
	} elseif ($actie == "groepsindeling") {
		if ($onderdlid > 0) {
			$where = sprintf("AND LO.OnderdeelID=%d", $onderdlid);
		} elseif (strlen($filter) > 0) {
			$where = "AND " . $filter;
		} else {
			$where = sprintf("AND LO.OnderdeelID IN (SELECT OnderdeelID FROM %sGroep)", TABLE_PREFIX);
		}
		$query = sprintf("SELECT DISTINCT O.Naam AS AfdNaam, CONCAT(GR.Starttijd, IF(LENGTH(GR.Eindtijd) > 3, CONCAT(' - ', GR.Eindtijd), '')) AS Tijdsblok, GR.Kode, GR.Omschrijving, L.Roepnaam, %1\$s AS NaamLid,
					CASE 
					WHEN GroepID=0 AND Functie=0 THEN 'Niet ingedeeld'
					WHEN LENGTH(Instructeurs) > 1 THEN CONCAT(GR.Omschrijving, ' - ', GR.Instructeurs)
					ELSE GR.Omschrijving
					END AS GroepOms
					FROM ((%2\$sLidond AS LO INNER JOIN %2\$sLid AS L ON LO.Lid=L.Nummer) INNER JOIN %2\$sOnderdl AS O ON LO.OnderdeelID=O.RecordID) LEFT JOIN %2\$sGroep AS GR ON LO.GroepID=GR.RecordID
					WHERE LO.Vanaf <= CURDATE() AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd >= CURDATE()) AND LO.Functie=0 %3\$s
					ORDER BY O.Kode, GR.Starttijd, GR.Kode, L.Achternaam, L.Roepnaam;", $selectnaam, TABLE_PREFIX, $where);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} else {
		$mess = sprintf("Actie '%s' is onbekend", $actie);
		db_logboek("add", $mess, 99);
	}
} # db_lidond

function db_groep($actie, $onderdeelid=0) {
	
	if ($onderdeelid > 0) {
		$where = sprintf("WHERE OnderdeelID=%d", $onderdeelid);
	} else {
		$where = "";
	}
	
	if ($actie == "aantal") {
		$query = sprintf("SELECT COUNT(*) FROM %sGroep %s;", TABLE_PREFIX, $where);
		return db_scalar($query);
	} elseif ($actie == "basislijst") {
		$query = sprintf("SELECT * FROM %sGroep ORDER BY OnderdeelID, Kode;", TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchAll();
	}
} # db_groep

function db_logins($actie, $email="", $login="", $lidid=0, $xtrafilter="", $pw="", $xtraorder="") {
	global $dbc, $selectnaam, $selectlidnr, $lididwebmasters, $wherelidond, $wherelid, $dir, $pwfromcookie;
	
	if (strlen(db_param("login_beperkttotgroep")) > 0) {
		$beperkttotgroep = explode(",", db_param("login_beperkttotgroep"));
	} else {
		$beperkttotgroep = array(0);
	}
	
	$mess = "";
	if ($actie == "nieuwlogin" and isValidMailAddress($email) and strlen($login) > 5) {
		$query = sprintf("SELECT L.Nummer, Login.Login FROM %1\$sLid AS L LEFT OUTER JOIN %1\$sAdmin_login AS Login ON L.Nummer = Login.LidID
								WHERE (LOWER(L.Email) LIKE '%2\$s' OR LOWER(L.EmailVereniging) LIKE '%2\$s')", TABLE_PREFIX, strtolower($email));
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
		
	} elseif ($actie == "add") {
		$login = cleanlogin($login);
		$ok = false;
		$query = sprintf("SELECT Login FROM %sAdmin_login WHERE LidID=%d;", TABLE_PREFIX, $lidid);
		if (strlen(db_scalar($query)) > 0) {
			$mess = sprintf("%s heeft al een login, er wordt geen nieuwe aangemaakt.", db_naamlid($lidid));
		} elseif (db_logins("controlebestaat", "", $login, $lidid)) {
			$mess = sprintf("Login '%s' is al in gebruik bij een ander lid.", $login);
		} elseif (strlen($login) < 6) {
			$mess = "De login moet minimaal 6 karakters lang zijn.";
		} elseif (strlen($pw) < db_param("wachtwoord_minlengte")) {
			$mess = sprintf("Het wachtwoord moet minimaal %d karakters lang zijn.", db_param("wachtwoord_minlengte"));
		} elseif ($lidid > 0) {
			$nk = password_hash(newkey(), PASSWORD_DEFAULT);
			$query = sprintf("INSERT INTO %sAdmin_login (LidID, Login, Wachtwoord, ActivatieKey, LaatsteWachtwoordWijziging, Ingevoerd, Gewijzigd) VALUES 
																	  (%d, '%s', '%s', '%s', SYSDATE(), SYSDATE(), SYSDATE());", TABLE_PREFIX, $lidid, $login, password_hash($pw, PASSWORD_DEFAULT), $nk);
			fnQuery($query);
			$mess = sprintf("Login '%s' is voor %s aangemaakt.", $login, db_naamlid($lidid));
			$mess .= fnValidatieLogin($lidid, $nk, "mail");
			$ok = true;
		}
		db_logboek("add", $mess, 5, 0, 1, $lidid);
		return $ok;
		
	} elseif ($actie == "rowlogin") {
		$query = sprintf("SELECT L.Nummer, Login.Login, %s AS Naam, L.Roepnaam, L.Email, Login.Wachtwoord, Login.FouteLogin, Login.Gewijzigd,
								IFNULL((%s), L.Nummer) AS Lidnr,", $selectnaam, $selectlidnr);
		if (is_array($beperkttotgroep) and count($beperkttotgroep) > 0) {
			$query .= sprintf("(SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.Lid=L.Nummer AND LO.OnderdeelID IN (%s) AND LO.Vanaf <= CURDATE() AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd > CURDATE())) AS AanvragenMag", TABLE_PREFIX, implode(", ", $beperkttotgroep));
		} else {
			$query .= "1 AS AanvragenMag";
		}
		$query .= sprintf(" FROM %1\$sLid AS L LEFT OUTER JOIN %1\$sAdmin_login AS Login ON L.Nummer = Login.LidID WHERE ", TABLE_PREFIX);
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
		
	} elseif ($actie == "valideerlogin") {
		$query = sprintf("UPDATE %sAdmin_login SET ActivatieKey='', Gewijzigd=SYSDATE() WHERE LENGTH(ActivatieKey) > 5 AND LidID=%d;", TABLE_PREFIX, $lidid);
		$result = fnQuery($query);
		if ($result == 1) {
			$mess = sprintf("Login voor %s is gevalideerd en kan worden gebruikt.", db_naamlid($lidid, "Naam"));
		} else {
			$mess = sprintf("Er ging iets mis bij het valideren van de login voor %s is gevalideerd. Mogelijk is deze login al gevalideerd. Zo niet probeer het later opnieuw.", db_naamlid($lidid, "Naam"));
		}
		return $mess;
		
	} elseif ($actie == "controlebestaat" and strlen($login) > 5) {
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login WHERE Login LIKE '%s' AND LidID<>%d;", TABLE_PREFIX, $login, $lidid);
		if (db_scalar($query) > 0) {
			return true;
		} else {
			return false;
		}
		
	} elseif ($actie == "foutelogin" and $lidid > 0) {
		$query = sprintf("SELECT FouteLogin FROM %sAdmin_login WHERE LidID=%d", TABLE_PREFIX, $lidid);
		return db_scalar($query);
		
	} elseif ($actie == "uitloggen") {
		$query = sprintf("UPDATE %sAdmin_login SET Ingelogd=0 WHERE Ingelogd > 0 AND (LastActivity < ADDDATE(SYSDATE(), INTERVAL -%d MINUTE) OR LidID=%d);", TABLE_PREFIX, session_cache_expire(), $lidid);
		$result = fnQuery($query);
		if ($result > 0) {
			if ($lidid > 0) {
				$mess = sprintf("%s (LidID: %d) is uitgelogd.", db_naamlid($lidid), $lidid);
				db_logboek("add", $mess, 1, $lidid);
				$_SESSION['lidid'] = 0;
			}
			fnMaatwerkNaUitloggen();
		}
		
	} elseif ($actie == "aantalingelogd") {
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login WHERE Ingelogd=1;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "aantal") {
		$query = sprintf("SELECT COUNT(*) FROM %1\$sAdmin_login AS Login INNER JOIN %1\$sLid AS L ON Login.LidID = L.Nummer;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "delete" and $lidid > 0) {
		if ($_SESSION['webmaster'] == 1) {
			$query = sprintf("DELETE FROM %sAdmin_login WHERE LidID=%d;", TABLE_PREFIX, $lidid);
			$result = fnQuery($query);
			if ($result > 0) {
				$mess = sprintf("De login van %s is verwijderd.", db_naamlid($lidid));
				db_logboek("add", $mess, 5, $lidid);
			}
		} else {
			$mess = "Je hebt geen rechten om logins te verwijderen.";
			db_logboek("add", $mess, 5, 0, 1);
		}
		
	} elseif ($actie == "setingelogd") {
		db_logins("uitloggen");
		
		$query = sprintf("UPDATE %sAdmin_login SET LastLogin=SYSDATE(), LastActivity=SYSDATE(), Ingelogd=1, FouteLogin=0 WHERE Ingelogd=0 AND LidID=%d;", TABLE_PREFIX, $lidid);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("%s is met '%s' ingelogd.", db_naamlid($lidid), $_SESSION['username']);
			db_logboek("add", $mess, 1, $lidid);
		} elseif ($_SESSION['lidid'] > 0) {
			$query = sprintf("UPDATE %sAdmin_login SET LastActivity=SYSDATE() WHERE LidID=%d;", TABLE_PREFIX, $_SESSION['lidid']);
			fnQuery($query);
		}
		
	} elseif ($actie == "wachtwoordreset" and $lidid > 0) {
		$nk = newkey();
		if (strlen($nk) > 0) {
			$query = sprintf("UPDATE %sAdmin_login SET ActivatieKey='%s', Wachtwoord='', LaatsteWachtwoordWijziging=SYSDATE(), Gewijzigd=SYSDATE() WHERE LidID=%d;", TABLE_PREFIX, password_hash($nk, PASSWORD_DEFAULT), $lidid);
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
			$query = sprintf("UPDATE %sAdmin_login SET ActivatieKey='%s', Gewijzigd=SYSDATE() WHERE LidID=%d;", TABLE_PREFIX, password_hash($nk, PASSWORD_DEFAULT), $lidid);
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
		$query = sprintf("UPDATE %sAdmin_login SET FouteLogin=0, Gewijzigd=SYSDATE() WHERE LidID=%d AND FouteLogin > 0;", TABLE_PREFIX, $lidid);
		$result = fnQuery($query);
		if ($result == 1) {
			$mess = sprintf("Het aantal foutieve inlogpogingen van de login van %s is gereset.", db_naamlid($lidid));
			db_logboek("add", $mess, 1, $lidid);
		}
		
	} elseif ($actie === "controle") {
		if (db_param("login_autounlock") > 0) {
			$query = sprintf("SELECT Login FROM %sAdmin_login WHERE Gewijzigd < DATE_SUB(SYSDATE(), INTERVAL %d MINUTE) AND FouteLogin > 0;", TABLE_PREFIX, db_param("login_autounlock"));
			$result = fnQuery($query);
			foreach ($result->fetchAll() as $row) {
				$updqry = sprintf("UPDATE %sAdmin_login SET FouteLogin=0, Gewijzigd=SYSDATE() WHERE Login='%s';", TABLE_PREFIX, $row->Login);
				$updres = fnQuery($query);
				if ($updres > 0) {
					$mess = sprintf("Login %s is automatisch gedeblokkeerd.", $row->Login);
					db_logboek("add", $mess, 5);
				}
			}
		}
	
		$xtrawhere = "";
		if (isValidMailAddress($login, 0)) {
			$query = sprintf("SELECT Login FROM %1\$sLid AS L INNER JOIN %1\$sAdmin_login AS Login ON Login.LidID = L.Nummer WHERE (LOWER(L.Email) LIKE '%2\$s' OR LOWER(L.EmailVereniging) LIKE '%2\$s')", TABLE_PREFIX, strtolower($login));
			$result = fnQuery($query);
			$rows = $result->fetchAll();
			if (count($rows) == 1) {
				$login = $rows[0]->Login;
			}
		}
		$xtrawhere = sprintf("AND Login.Login LIKE '%s'", cleanlogin($login));
		if (is_array($beperkttotgroep) and count($beperkttotgroep) > 0) {
			$xtrawhere .= sprintf(" AND (Login.LidID IN (SELECT Lid FROM %sLidond AS LO WHERE %s AND LO.OnderdeelID IN (%s)) OR Login.LidID IN (%s))", TABLE_PREFIX, $wherelidond, implode(", ", $beperkttotgroep), implode(", ", $lididwebmasters));
		} else {
			$xtrawhere .= sprintf(" AND Login.LidID IN (%s)", implode(", ", $lididwebmasters));
		}
		$query = sprintf("SELECT Login.LidID, Login.Login, %3\$s AS NaamLid, L.Roepnaam, L.Email, L.EmailVereniging, L.EmailOuders, Login.Wachtwoord, Login.ActivatieKey,
					IFNULL((%4\$s), 0) AS Lidnr, Login.FouteLogin, Login.Gewijzigd
					FROM %1\$sAdmin_login AS Login INNER JOIN %1\$sLid AS L ON Login.LidID = L.Nummer
					WHERE LENGTH(Login.Wachtwoord) > 5 %2\$s
					ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", TABLE_PREFIX, $xtrawhere, $selectnaam, $selectlidnr);
		$result = fnQuery($query);
		$row = $result->fetch();
		if (isset($row->LidID) and $row->LidID > 0) {
			if (password_verify($pw, $row->Wachtwoord)) {
				return $row;
			} else {
				if ($pwfromcookie == 0) {
					$query = sprintf("UPDATE %sAdmin_login SET FouteLogin=FouteLogin+1, Gewijzigd=SYSDATE() WHERE Login='%s';", TABLE_PREFIX, $login);
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
		
		$query = sprintf("SELECT Login.LidID AS lnkNummer, Login.Login, %1\$s AS Naam_lid, L.Woonplaats, (%5\$s) AS Lidnr, L.Roepnaam AS ndRoepnaam,
					L.Achternaam AS ndAchternaam, L.Tussenv AS ndTussenvoegsels, L.Meisjesnm AS ndMeisjesnaam, L.GEBDATUM AS ndGeboren,
					IF(LENGTH(L.Email)=0, L.EmailOuders, L.Email) AS `E-mail`, L.EmailVereniging AS ndEmailVereniging, L.Telefoon AS ndTelefoon, L.Mobiel AS ndMobiel,
					Login.Ingevoerd AS dteIngevoerd, Login.LastLogin AS `dttLaatste login`,
					IF(LENGTH(Login.Wachtwoord) > 5 AND LENGTH(IFNULL(Login.ActivatieKey, '')) = 0, IF(Login.Ingelogd=1, 'Ingelogd', 'Gevalideerd'), 'Niet gevalideerd') AS `Status`,
					Login.Wachtwoord AS ndWachtwoord, IF(Login.FouteLogin > 0, Login.LidID, 0) AS llkNummer
					FROM %2\$sAdmin_login AS Login INNER JOIN %2\$sLid AS L ON Login.LidID = L.Nummer
					WHERE LENGTH(Login.Login) > 5 %3\$s
					ORDER BY %4\$sL.Achternaam, L.TUSSENV, L.Roepnaam, Login.LidID;", $selectnaam, TABLE_PREFIX, $xtrawhere, $xtraorder, $selectlidnr);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "lidid" and strlen($login) > 4) {
		$query = sprintf("SELECT MAX(LidID) FROM %Admin_login WHERE Login='%s';", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "opschonen") {
		if (db_param("login_bewaartijd") > 0) {
			$query = sprintf("DELETE FROM %1\$sAdmin_login
						WHERE LastLogin < DATE_ADD(CURDATE(), INTERVAL -%2\$d MONTH) AND Ingevoerd < DATE_ADD(CURDATE(), INTERVAL -%2\$d MONTH);", TABLE_PREFIX, db_param("login_bewaartijd"));
			$result = fnQuery($query, 2);
		}
		$mess = "";
		$where = "";
		if (is_array($beperkttotgroep) and count($beperkttotgroep) > 0) {
			$where .= sprintf("LidID NOT IN (SELECT Lid FROM %sLidond AS LO WHERE %s AND LO.OnderdeelID IN (%s))", TABLE_PREFIX, $wherelidond, implode(", ", $beperkttotgroep));
		}
		if (strlen($where) > 0) {
			$where .= sprintf(" AND (LidID NOT IN (%s))", implode(", ", $lididwebmasters));
			$query = sprintf("DELETE FROM %sAdmin_login WHERE LastLogin <= DATE_ADD(CURDATE(), INTERVAL -1 WEEK) AND %s;", TABLE_PREFIX, $where);
			$result = fnQuery($query);
			if ($result > 0) {
				$mess .= sprintf("Er zijn %d logins verwijderd", $result);
			} else {
				$mess .= "Er zijn geen logins verwijderd";
			}
			$mess .= ", omdat ze geen toegang meer hebben.<br>\n";
		}
		$query = sprintf("DELETE FROM %sAdmin_login
					WHERE ((LastLogin IS NULL) OR LastLogin < '2012-01-01') AND Ingevoerd < DATE_ADD(CURDATE(), INTERVAL -%d DAY);", TABLE_PREFIX, db_param("login_bewaartijdnietgebruikt"));
		$result = fnQuery($query);
		if ($result > 0) {
			$mess .= sprintf("Er zijn %d logins verwijderd", $result);
		} else {
			$mess .= "Er zijn geen logins verwijderd";
		}
		$mess .= sprintf(", omdat ze niet binnen %d dagen na aanvragen gebruikt zijn.<br>\n", db_param("login_bewaartijdnietgebruikt"));
		
		$query = sprintf("UPDATE %1\$sAdmin_login SET HerinneringVervallenDiplomas=0 WHERE HerinneringVervallenDiplomas=1 AND 
					LidID NOT IN (SELECT Lid FROM %1\$sLidmaatschap WHERE ((Opgezegd IS NULL) OR Opgezegd >= CURDATE()));", TABLE_PREFIX);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess .= sprintf("Bij %d logins is de herinnering voor vervallen diploma uitgezet, omdat ze geen lid meer zijn. ", $result);
		}
		
		db_logboek("add", $mess, 2, 0, 1);
	
	} elseif ($actie == "lididbijlogin" and strlen($login) > 0) {
		$query = sprintf("SELECT IFNULL(MAX(LidID), 0) FROM %sAdmin_login WHERE Login='%s';", TABLE_PREFIX, $login);
		return db_scalar($query);
		
	} else {
		$mess = sprintf("Actie '%s' is onbekend", $actie);
		debug($mess, 1, 0, 1);
	}
} # db_logins

function db_change_password($pw_new, $pw_old, $lidid) {
	
	$mess = "ok";
	if (strlen($pw_old) > 0) {
		$query = sprintf("SELECT Wachtwoord from %sAdmin_login WHERE LidID=%d;", TABLE_PREFIX, $lidid);
		$pw_hash = db_scalar($query);
		if (password_verify($pw_old, $pw_hash) == false) {
			$mess = "Oude wachtwoord is niet correct, wachtwoord is niet gewijzigd.";	
		}
	}
	if ($mess == "ok") {
		$query = sprintf("UPDATE %sAdmin_login SET Gewijzigd=SYSDATE(), GewijzigdDoor=%d, Wachtwoord='%s', LaatsteWachtwoordWijziging=SYSDATE(), ActivatieKey='' WHERE LidID=%d;",
					 TABLE_PREFIX, $_SESSION['lidid'], password_hash($pw_new, PASSWORD_DEFAULT), $lidid);
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

function db_naam_onderdeel($rid, $rin="Onbekend") {

	$query = sprintf("SELECT Naam FROM %sOnderdl WHERE RecordID=%d;", TABLE_PREFIX, $rid);
	$result = fnQuery($query);
	$rv = $result->fetchColumn();
	if (strlen($rv) == 0) {
		$rv = $rin;
	}
	debug("db_naam_onderdeel moet worden vervangen", 99);
	return $rv;
}  #  db_naam_onderdeel

function db_add_mailing_hist($lidid, $mid, $to_name, $to_addr="", $message="", $mail=null, $success=1, $xtrachar="", $xtranum=0) {
	
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
		$to_addr = implode(",", $to_addr);
	}
		
	if (strlen($to_addr) > 100) {
		$to_addr = substr($to_addr, 0, 96) . " ...";
	}
	if (strlen($cc_addr) > 100) {
		$cc_addr = substr($cc_addr, 0, 96) . " ...";
	}

	if ($mid > 0) {
		$query = sprintf("SELECT * FROM %sMailing WHERE MailingID=%d;", TABLE_PREFIX, $mid);
		$result = fnQuery($query);
		$row = $result->fetch();
		
		$from_name = $row->from_name;
		$from_addr = $row->from_addr;
		
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
	$from_name = html_entity_decode(str_replace("\"", "'", $from_name));
	$subject = html_entity_decode(str_replace("\"", "'", $subject));
	$message = html_entity_decode(str_replace("\"", "'", $message));
	
	if ($success == 1) {
		$query = sprintf("INSERT INTO %sMailing_hist SET
					LidID=%d, MailingID=%d, from_name=\"%s\", from_addr=\"%s\", to_name=\"%s\", subject=\"%s\", to_addr=\"%s\", cc_addr=\"%s\", message=\"%s\", send_by=%d, send_on=SYSDATE(), send_mt=%F, Successful=%d, Xtra_Char='%s', Xtra_Num=%d;", TABLE_PREFIX,
					$lidid, $mid, $from_name, $from_addr, $to_name, $subject, $to_addr, $cc_addr, $message, $_SESSION['lidid'], microtime(true), $success, $xtrachar, $xtranum);
	} else {
		$query = sprintf("INSERT INTO %sMailing_hist SET
					LidID=%d, MailingID=%d, from_name=\"%s\", from_addr=\"%s\", to_name=\"%s\", subject=\"%s\", to_addr=\"%s\", cc_addr=\"%s\", message=\"%s\", send_by=%d, Successful=%d, Xtra_Char='%s', Xtra_Num=%d;", TABLE_PREFIX,
					$lidid, $mid, $from_name, $from_addr, $to_name, $subject, $to_addr, $cc_addr, $message, $_SESSION['lidid'], $success, $xtrachar, $xtranum);
	}
	fnQuery($query);

}  # dn_add_mailing_hist

function db_mailing($actie, $mid=0, $filter="", $new_val="", $kolnaam="") {
	global $selectnaam, $selectzoeknaam, $fdtlang;
	
	$mess = "";
	if ($actie == "exist") {
		$query = sprintf("SELECT COUNT(*) FROM %sMailing WHERE MailingID=%d AND (deleted_on IS NULL);", TABLE_PREFIX, $mid);
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
				$w = sprintf("(NOT M.Sent_on IS NULL)", TABLE_PREFIX);
				$xtra_sel = sprintf(", CONCAT(IF(M.SentBy=0, '', (SELECT %s FROM %sLid AS L WHERE L.Nummer=M.SentBy)), ' & ', DATE_FORMAT(M.sent_on, %s)) AS `mrgLaatst verzonden door/op`", $selectnaam, TABLE_PREFIX, $fdtlang);
			} else {
				$w = sprintf("(M.Sent_on IS NULL)", TABLE_PREFIX);
			}
			$w .= " AND template=0 AND (deleted_on IS NULL)";
			if ($_SESSION['webmaster'] == 0) {
				$w .= sprintf(' AND (M.AddedBy=%1$d OR M.ChangedBy=%1$d OR M.confidential=0)', $_SESSION['lidid']);
			}
		} elseif ($filter == "Templates") {
			$w = "M.template=1 AND (deleted_on IS NULL)";
			$xtra_sel = sprintf(", CONCAT(IF(M.SentBy=0, '', (SELECT %s FROM %sLid AS L WHERE L.Nummer=M.SentBy)), ' & ', DATE_FORMAT(M.sent_on, %s)) AS `mrgLaatst verzonden door/op`", $selectnaam, TABLE_PREFIX, $fdtlang);
			$orderby = "M.subject, sent_on";
			
		} elseif ($filter == "Verzonden mails") {
			
		} elseif ($filter == "Prullenbak") {
			$w = "(NOT M.deleted_on IS NULL)";
			$orderby = "M.deleted_on DESC";
			$xtra_sel = sprintf(", CONCAT((SELECT %s FROM %sLid AS L WHERE Nummer=M.DeletedBy), ' & ', DATE_FORMAT(M.deleted_on, %s)) AS `mrgVerwijderd`", $selectnaam, TABLE_PREFIX, $fdtlang);
		} else {
			$w = "(M.deleted_on IS NULL)";
		}
		$query = sprintf("SELECT M.MailingID AS lnkMailingID, M.MailingID AS intNr, M.subject AS Onderwerp, M.from_name AS Van, M.to_name AS Aan,
					CONCAT(%1\$s, ' & ', DATE_FORMAT(M.changed_on, %6\$s)) AS `mrgLaatst gewijzigd`
					%2\$s,
					IF((SELECT COUNT(*) FROM %3\$sMailing_hist AS MH WHERE MH.MailingID=M.MailingID) > 0, M.MailingID, 0) AS llkHist
					FROM %3\$sMailing AS M LEFT JOIN %3\$sLid AS L ON M.ChangedBy = L.Nummer
					WHERE %4\$s ORDER BY %5\$s;", $selectnaam, $xtra_sel, TABLE_PREFIX, $w, $orderby, $fdtlang);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "rcpt") {
		$query = sprintf("SELECT R.LidID, IFNULL(L.Nummer, 0) AS Nummer, %1\$s AS Naam_lid, %2\$s AS Zoeknaam_lid, L.Adres, L.Postcode, 
					L.Woonplaats, L.Email, L.EmailVereniging, L.EmailOuders, L.GEBDATUM, R.to_address
					FROM %3\$sMailing_rcpt AS R LEFT OUTER JOIN %3\$sLid AS L ON R.LidID = L.Nummer
					WHERE MailingID=%4\$d
					ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, R.to_address, R.Ingevoerd;", $selectnaam, $selectzoeknaam, TABLE_PREFIX, $mid);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "potrcpt") {
		$query = sprintf("SELECT L.Nummer, %1\$s AS Zoeknaam_lid FROM %2\$sLid AS L
					WHERE (L.Verwijderd IS NULL) AND (L.Overleden IS NULL) AND (LENGTH(L.Email) > 5 OR LENGTH(L.EmailVereniging) > 5 OR LENGTH(L.EmailOuders) > 5 OR (SELECT COUNT(*) FROM %2\$sLidmaatschap AS LM WHERE IFNULL(LM.Opgezegd, CURDATE()) >= CURDATE() AND LM.Lid=L.Nummer) > 0)
					AND (SELECT COUNT(*) FROM %2\$sMailing_rcpt AS R WHERE R.LidID = L.Nummer AND R.MailingID=%3\$d)=0
					ORDER BY L.Achternaam, L.Roepnaam;", $selectzoeknaam, TABLE_PREFIX, $mid);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "aantalprullenbak") {
		$query = sprintf("SELECT COUNT(*) FROM %sMailing WHERE (NOT deleted_on IS NULL);", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "laatstehist" and $mid > 0) {
		$query = sprintf("SELECT send_on, send_by FROM %sMailing_hist WHERE Successful=1 AND MailingID=%d ORDER BY send_mt DESC LIMIT 1;", TABLE_PREFIX, $mid);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "laatstehist") {
		$query = sprintf("SELECT send_on FROM %sMailing_hist WHERE Successful=1 ORDER BY send_on DESC LIMIT 1;", TABLE_PREFIX, $mid);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "lastsendtime") {
		$query = sprintf("SELECT MAX(send_mt) FROM %sMailing_hist;", TABLE_PREFIX, $mid);
		return db_scalar($query);
		
	} elseif ($actie == "add") {
		$subj = str_replace("\"", "'", $new_val);
		$query = sprintf("INSERT INTO %sMailing (subject, AddedBy) VALUES (\"%s\", %d);", TABLE_PREFIX, $subj, $_SESSION['lidid']);
		$id = fnQuery($query);
		if ($id > 0) {
			$mess = sprintf("Mailing %d (%s) is door %s toegevoegd.", $id, $subj, $_SESSION['naamingelogde']);
		} else {
			$mess = sprintf("Het toevoegen van mailing '%s' is mislukt.", $subj);
		}
		db_logboek("add", $mess, 4, 0, 0, $id);
		return $id;
		
	} elseif ($actie === "update") {
		
		$mess = "";
		if (is_numeric($new_val)) {
			$clause = sprintf("`%s`=%d", $kolnaam, $new_val);
			$where = sprintf("`%s`<>%d", $kolnaam, $new_val);
		} else {
			$new_val = str_replace("\"", "'", $new_val);
			$clause = sprintf("`%s`=\"%s\"", $kolnaam, $new_val);
			$where = sprintf("BINARY IFNULL(`%s`, '')<>\"%s\"", $kolnaam, $new_val);
		}
		$clause .= sprintf(", changed_on=SYSDATE(), ChangedBy=%d", $_SESSION['lidid']);
		$where .= sprintf(" AND MailingID=%d", $mid);
		$query = sprintf("UPDATE %sMailing SET %s WHERE %s;", TABLE_PREFIX, $clause, $where);
//		debug($query);
		$result = fnQuery($query);
		if ($result > 0) {
			if (is_numeric($new_val)) {
				$mess = sprintf("Mailing %d: kolom '%s' is in %d gewijzigd. ", $mid, $kolnaam, $new_val);
			} else {
				if (strlen($new_val) > 125) {
					$new_val = substr($new_val, 0, 121) . " ...";
				}
				$mess = sprintf("Mailing %d: kolom '%s' is in '%s' gewijzigd. ", $mid, $kolnaam, $new_val);
			}
		}
		if (strlen($mess) > 0) {
			db_logboek("add", $mess, 4, 0, 0, $mid);
		}
		
	} elseif ($actie == "updatesent") {
		$query = sprintf("UPDATE %sMailing SET sent_on=SYSDATE(), SentBy=%d WHERE MailingID=%d;", TABLE_PREFIX, $_SESSION['lidid'], $mid);
		fnQuery($query);
		
	} elseif ($actie == "opschonen") {
		if (db_param("mailing_bewaartijd") > 0) {
			$query = sprintf("DELETE FROM %sMailing WHERE (NOT deleted_on IS NULL)
						AND deleted_on < ADDDATE(CURDATE(), INTERVAL -%d MONTH);", TABLE_PREFIX, db_param("mailing_bewaartijd"));
			$result = fnQuery($query, 2);
			
			$query = sprintf('DELETE FROM %1$sMailing_rcpt
						WHERE MailingID NOT IN (SELECT M.MailingID FROM %1$sMailing AS M);', TABLE_PREFIX);
			$result = fnQuery($query, 2);
			
			$query = sprintf('UPDATE %1$sMailing_hist SET MailingID=0
						WHERE MailingID > 0 AND MailingID NOT IN (SELECT M.MailingID FROM %1$sMailing AS M);', TABLE_PREFIX);
			$result = fnQuery($query, 2);
		}

	} elseif ($actie == "record") {
		$query = sprintf("SELECT * FROM %sMailing WHERE MailingID=%d;", TABLE_PREFIX, $mid);
		$result = fnQuery($query);
		return $result->fetch();
		
	} else {
		$mess = sprintf("Actie '%s' is onbekend.", $actie);
		db_logboek("add", $mess, 99);
	}
	
} # db_mailing

function db_mailing_hist($actie, $lidid=0, $mhid=0, $mid=0, $mail=null) {
	global $selectnaam;
	
	if ($actie == "outbox") {
		$lw = "Successful=0 AND send_on < '1901-01-01'";
	} else {
		$lw = "Successful=1";
	}
	if ($lidid > 0) {
		$lw .= sprintf(" AND LidID=%d", $lidid);
	} elseif ($_SESSION['webmaster'] == 0) {
		$lw .= sprintf(" AND (LidID=%1\$d OR send_by=%1\$d)", $lidid);
	}
	
	if ($actie == "lijst" or $actie == "outbox" or $actie == "sqllijst") {
		if ($mid > 0) {
			$query = sprintf("SELECT MH.RecordID AS lnkRecordID, M.subject AS ndOnderwerp, IF(MH.Successful=1, MH.send_on, 'Niet verzonden') AS dtsVerzonden, IF(MH.LidID=0, MH.to_name, %1\$s) AS Aan,
						to_addr AS `E-mail`, MH.cc_addr AS `CC`, L.Woonplaats, MH.RecordID AS llkRecordID
						FROM (%2\$sMailing_hist AS MH LEFT OUTER JOIN %2\$sLid AS L ON MH.LidID = L.Nummer) INNER JOIN %2\$sMailing AS M ON MH.MailingID = M.MailingID
						WHERE MH.MailingID=%3\$d
						ORDER BY MH.RecordID DESC;", $selectnaam, TABLE_PREFIX, $mid);
		} else {
			$query = sprintf("SELECT RecordID AS lnkRecordID,
						send_on AS dttVerzonden,
						IF(LENGTH(from_name)<3, from_addr, from_name) AS Van,
						CONCAT(to_name, ' (', to_addr, ')') AS Aan,
						subject AS Onderwerp,
						RecordID AS llkRecordID
						FROM %sMailing_hist
						WHERE %s
						ORDER BY RecordID DESC LIMIT 1000;", TABLE_PREFIX, $lw);
		}

		if ($actie == "sqllijst") {
			return $query;
		} else {
			return fnQuery($query);
		}
		
	} elseif ($actie === "record") {
		$query = sprintf("SELECT send_on, from_addr, from_name, to_name, to_addr, cc_addr, subject, message, Successful, send_by, LidID,CONCAT(to_name, ' (', to_addr, ')') AS aan
					FROM %sMailing_hist WHERE RecordID=%d LIMIT 1;", TABLE_PREFIX, $mhid);
		$result = fnQuery($query);
		return $result->fetch();
	
	} elseif ($actie == "lidbijmail") {
		$query = sprintf("SELECT IFNULL(MAX(LidID), 0) FROM %sMailing_hist WHERE RecordID=%d;", TABLE_PREFIX, $mhid);
		return db_scalar($query);
		
	} elseif ($actie == "aantal24uur") {
		$query = sprintf("SELECT COUNT(*) FROM %sMailing_hist WHERE Successful=1 AND send_on > DATE_SUB(NOW(), INTERVAL 24 HOUR);", TABLE_PREFIX);
		return db_scalar($query);
				
	} elseif ($actie == "updatesent") {
		$query = sprintf("SELECT * FROM %sMailing_hist WHERE RecordID=%d;", TABLE_PREFIX, $mhid);
		$result = fnQuery($query);
		$row = $result->fetch();
		if ($row->send_on > '2000-01-01') {
			db_add_mailing_hist($row->LidID, $row->MailingID, $row->to_name, $row->to_addr, $row->message, $mail, 1, $row->Xtra_Char, $row->Xtra_Num);
		} else {
			$query = sprintf("UPDATE %sMailing_hist SET send_by=%d, send_on=SYSDATE(), send_mt=%F, Successful=1 WHERE RecordID=%d;", TABLE_PREFIX, $_SESSION['lidid'], microtime(true), $mhid);
			fnQuery($query);
		}
		
	} else {
		$mess = sprintf("Actie '&s' is onbekend.", $actie);
		db_logboek("add", $mess, 99);
	}

} # db_mailing_hist

function db_mailing_rcpt($actie, $mid=0, $lidid=0, $email="", $lb=0) {
	global $selectnaam, $selectzoeknaam;
	
	$tlm = 1;

	if ($actie == "aantal") {
		$query = sprintf("SELECT COUNT(*) FROM %sMailing_rcpt WHERE MailingID=%d;", TABLE_PREFIX, $mid);
		return db_scalar($query);
	
	} elseif ($actie == "add" and $mid > 0) {
		$mess = "";
		if ($lidid > 0) {
			$query = sprintf("SELECT COUNT(*) FROM %sMailing_rcpt WHERE MailingID=%d AND LidID=%d;", TABLE_PREFIX, $mid, $lidid);
		} elseif (isValidMailAddress($email)) {
			$query = sprintf("SELECT COUNT(*) FROM %sMailing_rcpt WHERE MailingID=%d AND LOWER(to_address) LIKE '%s';", TABLE_PREFIX, $mid, strtolower($email));
		} else {
			$query = "";
		}
		if (strlen($query) > 0 and db_scalar($query) == 0) {
			if ($lidid > 0) {
				$rl = db_lid("record", "", $lidid);
				if (isValidMailAddress($rl->Email) or isValidMailAddress($rl->EmailOuders) or isValidMailAddress($rl->EmailVereniging)) {
					$query = sprintf("INSERT INTO %sMailing_rcpt SET MailingID=%d, LidID=%d;", TABLE_PREFIX, $mid, $lidid);
				} else {
					$mess = sprintf("%s is niet aan mailing %d toegevoegd, omdat er geen geldig emailadres bekend is.", $rl->NaamLid, $mid);
					db_logboek("add", $mess, 4, 0, 1, $lidid);
				}
				$r = $rl->NaamLid;
				$rl = null;
			} elseif (isValidMailAddress($email)) {
				$query = sprintf("INSERT INTO %sMailing_rcpt SET MailingID=%d, to_address='%s';", TABLE_PREFIX, $mid, $email);
				$r = $email;
			}
			if (strlen($mess) == 0 and fnQuery($query) > 0) {
				$mess = sprintf("%s is aan mailing %d toegevoegd.", $r, $mid);
				$tlm = 0;
			} else {
				$mess = "";
			}
		}
		if ($lb == 1 and strlen($mess) > 0) {
			db_logboek("add", $mess, 4, $lidid, $tlm);
		} else {
			$mess = "";
		}
		return $mess;
		
	} elseif ($actie == "del") {
		$mess = "";
		if ($lidid > 0) {
			$query = sprintf("DELETE FROM %sMailing_rcpt WHERE MailingID=%d AND LidID=%d;", TABLE_PREFIX, $mid, $lidid);
			$r = db_naamlid($lidid);
		} elseif (strlen($email) > 0) {
			$query = sprintf("DELETE FROM %sMailing_rcpt WHERE MailingID=%d AND to_address='%s';", TABLE_PREFIX, $mid, $email);
			$r = $email;
		}
		if (fnQuery($query) > 0) {
			$mess = sprintf("%s is bij mailing %d verwijderd.", $r, $mid);
		}
		return $mess;
		
	} else {
		$mess = sprintf("Actie '%s' is onbekend.", $actie);
		db_logboek("add", $mess, 99);
	}
		
} # db_mailing_rcpt

function db_mailing_vanaf($actie, $value="", $rid=0, $kolnaam="") {
	
	if ($actie == "lijst" or $actie == "editlijst") {
		if ($actie == "lijst") {
			$s = "RecordID, Vanaf_email, Vanaf_naam, SMTP_server, Ingevoerd, Gewijzigd";
		} else {
			$s = "RecordID AS intRecordID, Vanaf_email AS emlVanaf_email, Vanaf_naam, Ingevoerd AS dteIngevoerd, Gewijzigd AS dttGewijzigd";
		}
		if ($actie == "editlijst") {
			$s .= ", RecordID AS llkRecordID";
		}
		$query = sprintf("SELECT %s FROM %sMailing_vanaf ORDER BY Vanaf_email;", $s, TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "record" and strlen($value) > 5) {
		$query = sprintf("SELECT * FROM %sMailing_vanaf WHERE BINARY LOWER(Vanaf_email)='%s';", TABLE_PREFIX, strtolower($value));
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "add") {
		$query = sprintf("INSERT INTO %sMailing_vanaf (Vanaf_email, Ingevoerd) VALUES ('** nieuw **', CURDATE());", TABLE_PREFIX);
		$result = fnQuery($query);
		$mess = sprintf("Record aan tabel %sMailing_vanaf met RecordID %d toegevoegd", TABLE_PREFIX, $result);
		db_logboek("add", $mess, 4, 0, 1);
		return $result;
		
	} elseif ($actie == "update") {
		if ($kolnaam == "SMTP_poort") {
			$query = sprintf("UPDATE %1\$sMailing_vanaf SET %2\$s=%3\$d WHERE %2\$s <> %3\$d AND RecordID=%4\$d;", TABLE_PREFIX, $kolnaam, $value, $rid);
		} else {
			$query = sprintf("UPDATE %1\$sMailing_vanaf SET %2\$s='%3\$s' WHERE BINARY %2\$s <> '%3\$s' AND RecordID=%4\$d;", TABLE_PREFIX, $kolnaam, $value, $rid);
		}
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("%sMailing_vanaf/Record %d: %s is gewijzigd in %s", TABLE_PREFIX, $rid, $kolnaam, $value);
			db_logboek("add", $mess, 4);
		}
		
	} elseif ($actie == "delete") {
		$query = sprintf("DELETE FROM %sMailing_vanaf WHERE RecordID=%d", TABLE_PREFIX, $rid);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Record met RecordID %d is uit tabel %sMailing_vanaf verwijderd.", $rid, TABLE_PREFIX);
			db_logboek("add", $mess, 4, 0, 1);
		}
		
	} elseif ($actie == "curuser") {
		$query = sprintf("SELECT IFNULL(Vanaf_email, '') FROM %sMailing_vanaf WHERE LOWER(Vanaf_email)='%s';", TABLE_PREFIX, strtolower($_SESSION['emailingelogde']));
		$rv = db_scalar($query);
		if (strlen($rv) == 0) {
			$query = sprintf("SELECT IFNULL(from_addr, '') FROM %sMailing WHERE AddedBy=%d ORDER BY MailingID DESC LIMIT 1;", TABLE_PREFIX,$_SESSION['lidid']);
			$rv = db_scalar($query);
		}
		return $rv;
		
	} else {
		$mess = sprintf("Actie '%s' is onbekend.", $actie);
	}
	
} # db_mailing_vanaf

function db_mutatie($actie, $jaar=0, $gbr="*", $kpl="*") {
	
	if ($actie == "aantal") {
		$query = sprintf("SELECT COUNT(*) FROM %sMutatie;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "lijst") {
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
							ORDER BY J.Kode, GBR.Kode, KPL.Kode, Datum;", $sel_kpl, TABLE_PREFIX, $where);
		$result = fnQuery($query);
		return $result->fetchAll();
	} else {
		sprintf("<p class='mededeling'>Actie '%s' bestaat niet.</p>", $actie);
	}
}  # db_mutatie

function db_boekjaar() {
	
	$query = sprintf("SELECT RecordID, Kode AS Jaar FROM %sBoekjaar ORDER BY Begindatum DESC;", TABLE_PREFIX);
	$result = fnQuery($query);
	return $result->fetchAll();
	
}  # db_Boekjaar

function db_kostenplaats($actie) {

	if ($actie == "distinct") {
		$query = sprintf("SELECT DISTINCT KPL.Kode AS Kode
					FROM %1\$sMutatie AS M INNER JOIN %1\$sKostenplaats AS KPL ON M.KostenplaatsID = KPL.RecordID
					WHERE M.KostenplaatsID > 0 ORDER BY KPL.Kode;", TABLE_PREFIX);
	}
	$result = fnQuery($query);
	return $result->fetchAll();
}  # db_kostenplaatst

function db_logboek($actie, $oms="", $type=-1, $lidid=0, $tm=0, $referid=0, $ipfilter="", $rid=0) {
	global $selectnaam, $selectzoeknaam;
	
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
			if (isset($bt[3]['function'])) {
				$f = $bt[3]['function'] . "=>" . $f;
			}
			$f = substr($f, 0, 50);
		}
		
		$oms = str_replace("<p>", "", $oms);
		$oms = str_replace("</p>", "\n", $oms);
		$oms = str_replace("\"", "'", $oms);
		if (strlen($oms) > 64000) {
			$oms = substr($oms, 0, 64000);
		}
		
		$query = sprintf("INSERT INTO %sAdmin_activiteit 
				(DatumTijd, LidID, IP_adres, USER_AGENT, Omschrijving, ReferID, TypeActiviteit, Script, Getoond, RefFunction) VALUES 
				(SYSDATE(), %d, '%s', '%s', \"%s\",%d, %d, \"%s\", %d, '%s');",
				TABLE_PREFIX, $_SESSION['lidid'], $_SERVER['REMOTE_ADDR'], $_SERVER['HTTP_USER_AGENT'], $oms, $referid, $type, $script, $tm, $f);
		fnQuery($query);
		if ($tm == 1) {
			printf("<p class='mededeling'>%s</p>\n", $oms);
		} elseif ($tm == 2) {
			printf("<script>alert(\"%s\");</script>\n", $oms);
		}
		usleep(5);
		
	} elseif ($actie == "opschonen") {
		if (db_param("logboek_bewaartijd") > 0) {
			$query = sprintf("DELETE FROM %sAdmin_activiteit WHERE DatumTijd < DATE_ADD(CURDATE(), INTERVAL -%d MONTH);", TABLE_PREFIX, db_param("logboek_bewaartijd"));
			fnQuery($query, 2);
		}
				
	} elseif ($actie == "debugopschonen") {
		$query = sprintf("DELETE FROM %sAdmin_activiteit WHERE TypeActiviteit=99 AND LidID=%d;", TABLE_PREFIX, $_SESSION['lidid']);
		fnQuery($query, 2);
		
	} elseif ($actie == "lijst" or $actie == "lijstsmal") {
		if ($type < 0) {
			$s = ", TypeActiviteit AS `Type`";
		} else {
			$s = "";
		}
		if (strlen($ipfilter) <= 5 and $_SESSION['webmaster'] == 1 and $actie !== "lijstsmal") {
			$s .= ", A.IP_adres";
		}
		if ($_SESSION['webmaster'] == 1 and $actie !== "lijstsmal") {
			$s .= ", Script, RefFunction AS Functie";
		}
		if ($lidid > 0) {
			$w .= sprintf(" AND (LidID=%1\$d OR (TypeActiviteit IN (1, 6, 8, 10, 12, 16) AND ReferID=%1\$d)) AND TypeActiviteit < 98", $lidid);
		}
		$s .= sprintf(", %s AS `Ingelogd Lid`", $selectnaam);

		$query = sprintf("SELECT A.DatumTijd as `dtsDatum en tijd`, Omschrijving%1\$s
								FROM %2\$sAdmin_activiteit AS A LEFT OUTER JOIN %2\$sLid AS L ON A.LidID = L.Nummer
								WHERE %3\$s
								ORDER BY A.RecordID DESC LIMIT 1500;", $s, TABLE_PREFIX, $w);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "lidlijst") {
		$query = sprintf("SELECT DISTINCT LidID, %1\$s AS Naam
								FROM %2\$sAdmin_activiteit AS A INNER JOIN %2\$sLid AS L ON A.LidID = L.Nummer
								WHERE %3\$s
								ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", $selectzoeknaam, TABLE_PREFIX, $w);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "iplijst") {
		$query = sprintf("SELECT DISTINCT IP_adres FROM %sAdmin_activiteit WHERE %s ORDER BY IP_adres;", TABLE_PREFIX, $w);
		$result = fnQuery($query);
		return $result->fetchAll();
	
	} elseif ($actie == "aantal") {
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_activiteit WHERE %s;", TABLE_PREFIX, $w);
		return db_scalar($query);
	
	} elseif ($actie == "iplogincontrole") {
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_activiteit WHERE TypeActiviteit=97 AND IP_adres='%s' AND DatumTijd >= ", TABLE_PREFIX, $_SERVER['HTTP_USER_AGENT']);
		if (db_param("login_autounlock") > 0) {
			$query .= sprintf("DATE_ADD(SYSDATE(), INTERVAL -%d MINUTE);", db_param("login_autounlock"));
		} else {
			$query .= "DATE_ADD(SYSDATE(), INTERVAL -1 HOUR);";
		}
		return db_scalar($query);
		
	} elseif ($actie == "delete") {
		$query = sprintf("DELETE FROM %sAdmin_activiteit WHERE RecordID=%d AND LidID=%d;", TABLE_PREFIX, $rid, $_SESSION['lidid']);
		if (fnQuery($query) > 0) {
			return true;
		} else {
			return false;
		}
		
	} elseif ($actie == "vorigelogin") {
		if ($lidid == 0) {
			$lidid = $_SESSION['lidid'];
		}
		$query = sprintf("SELECT IFNULL(MAX(DatumTijd), 'Geen') FROM %sAdmin_activiteit WHERE LidID=%d AND TypeActiviteit=1 AND DatumTijd < DATE_SUB(SYSDATE(), INTERVAL 15 MINUTE) AND Omschrijving LIKE '%%ingelogd%%';", TABLE_PREFIX, $lidid);
		return db_scalar($query);
		
	} else {
		$mess = sprintf("Actie '%s' is onbekend.", $actie);
		debug($mess);
	}
} # db_logboek

function db_interface($actie="lijst", $query="", $lidid=0, $nip=0) {
	global $selectnaam;
	
	$maxrecordsperkeer = 99;
	
	if ($actie == "add" and strlen($query) > 1) {
		$msa_sql = str_replace(TABLE_PREFIX, "", $query);
		$msa_sql = str_replace("\"", "'", $msa_sql);
		$msa_sql = str_replace("SYSDATE()", "#" . strftime("%m/%d/%Y %H:%M:%S") . "#", $msa_sql);
		$msa_sql = str_replace("CURDATE()", "#" . strftime("%m/%d/%Y") . "#", $msa_sql);
		$msa_sql = str_replace(" BINARY", "", $msa_sql);
		if ($nip == 1) {
			$ip = "";
		} else {
			$ip = $_SERVER['REMOTE_ADDR'];
		}
		$query = sprintf("INSERT INTO %sAdmin_interface (`IP-adres`, LidID, `SQL-statement`, IngelogdLid, Ingevoerd) VALUES ('%s', %d, \"%s\", %d, SYSDATE());", TABLE_PREFIX, $ip, $lidid, $msa_sql, $_SESSION['lidid']);
		fnQuery($query);

	} elseif ($actie == "afmelden") {
		$query = sprintf("UPDATE %sAdmin_interface SET Afgemeld=SYSDATE() WHERE (Afgemeld < '2011-01-01' OR (Afgemeld IS NULL)) ORDER BY Ingevoerd, RecordID LIMIT %d;", TABLE_PREFIX, $maxrecordsperkeer);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Er zijn %d wijzigingen uit de interface afgemeld.", $result);
		} else {
			$mess = "Er zijn geen wijzigingen uit de interface afgemeld.";
		}
		db_logboek("add", $mess, 2);
		return $mess;
		
	} elseif ($actie == "aantalopenstaand") {
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_interface WHERE (Afgemeld < '2010-01-01' OR (Afgemeld IS NULL));", TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "lijst") {
		$query = sprintf("SELECT %1\$s AS `Naam lid`, I.Ingevoerd, `SQL-statement` AS `SQL`"
				 . " FROM %2\$sAdmin_interface AS I LEFT OUTER JOIN %2\$sLid AS L ON I.LidID = L.Nummer"
				 . " WHERE (I.Afgemeld IS NULL)"
				 . " ORDER BY I.Ingevoerd, I.RecordID LIMIT %3\$d;", $selectnaam, TABLE_PREFIX, $maxrecordsperkeer);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} else {
		$mess = sprintf("Actie '%s' is onbekend.", $actie);
		db_logboek("add", $mess, 99);
	}
} # db_interface

function db_diploma($actie, $did=0, $td="*") {
	
	$mess = "";
	if ($actie == "basislijst") {
		$query = sprintf("SELECT * FROM %sDiploma ORDER BY Kode;", TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "muteerlijst") {
		if ($td != "*" and strlen($td) > 0) {
			$filter = sprintf(" AND DP.`Type`='%s'", $td);
		} else {
			$filter = "";
		}
		$query = sprintf("SELECT * FROM %sDiploma AS DP
					WHERE ((DP.Vervallen IS NULL) OR DP.Vervallen > CURDATE()) %s
					ORDER BY DP.Volgnr, DP.Kode;", TABLE_PREFIX, $filter);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "zelfservice_lijst") {
		$query = sprintf("SELECT * FROM %sDiploma AS DP
					WHERE DP.Zelfservice=1 AND DP.AantalBeoordelingen < 2 AND ((DP.Vervallen IS NULL) OR DP.Vervallen > CURDATE())
					ORDER BY DP.Volgnr, DP.Kode;", TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "kode") {
		$query = sprintf("SELECT Kode FROM %sDiploma WHERE RecordID=%d;", TABLE_PREFIX, $did);
		
		return db_scalar($query);
	} elseif ($actie == "naam") {
		$query = sprintf("SELECT Naam FROM %sDiploma WHERE RecordID=%d;", TABLE_PREFIX, $did);
		return db_scalar($query);
		
	} elseif ($actie == "opschonen") {
		/* Tijdelijk opgeschort.
		$query = sprintf("DELETE FROM %sDiploma WHERE (NOT Vervallen IS NULL) AND Vervallen < CURDATE();", TABLE_PREFIX);
		fnQuery($query, 2);
		*/
	}
} # db_diploma

function db_liddipl($actie, $lidid=0, $rid=0, $did=0, $exdat="", $vervdat="", $dipnr="", $xtrafilter="") {
	global $fdaccess, $fdsql;
	
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
		
		$nrid = db_nieuwrecordID("Liddipl");
		
		if ($dwverv != "NULL") {
			$dwverv = sprintf("'%s'", $dwverv);
		}
		
		if ($dwexd > date("Y-m-d")) {
			$mess = sprintf("De examendatum mag niet in de toekomst liggen. Diploma '%s' is bij %s niet toegevoegd.", db_diploma("naam", $did), db_naamlid($lidid));
		} else {
			$query = sprintf("INSERT INTO %sLiddipl (RecordID, Lid, DiplomaID, DatumBehaald, LaatsteBeoordeling, LicentieVervallenPer, Diplomanummer, Ingevoerd, Gewijzigd)
									VALUES (%d, %d, %d, '%s', 1, %s, '%s', CURDATE(), SYSDATE());", TABLE_PREFIX, $nrid, $lidid, $did, $dwexd, $dwverv, $dipnr);
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
		$query = sprintf("SELECT * FROM %sLiddipl AS LD WHERE RecordID=%d;", TABLE_PREFIX, $rid);
		$result = fnQuery($query);
		$rowld = $result->fetch();
		if (strtotime($rowld->LicentieVervallenPer) != strtotime($dwverv) or $rowld->Diplomanummer != $dipnr) {
			if ($dwverv != "NULL") {
				$dwverv = sprintf("'%s'", $dwverv);
			}
			$query = sprintf("UPDATE %sLiddipl SET LicentieVervallenPer=%s, Diplomanummer='%s' WHERE RecordID=%d;", TABLE_PREFIX, $dwverv, $dipnr, $rid);
			$result = fnQuery($query);
			$query = sprintf("UPDATE Liddipl SET LicentieVervallenPer=%s, Diplomanummer='%s', Gewijzigd=SYSDATE() WHERE Lid=%d AND DiplomaID=%d AND DatumBehaald=%s;", $dwaverv, $dipnr, $rowld->Lid, $rowld->DiplomaID, $dwaexd);
			db_interface("add", $query, $rowld->Lid);
			$mess = sprintf("Diploma '%s' is gewijzigd bij %s", db_diploma("naam", $rowld->DiplomaID), db_naamlid($rowld->Lid));
			db_logboek("add", $mess, 12, $rowld->Lid);
		}
		return $mess;
		
	} elseif ($actie == "delete" and $lidid > 0 and $did > 0 and strlen($dwexd) == 10) {
		$query = sprintf("DELETE FROM %sLiddipl WHERE Lid=%d AND DiplomaID=%d AND DatumBehaald='%s';", TABLE_PREFIX, $lidid, $did, $dwexd);
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
		
	} elseif ($actie == "lidgegevens") {
		if (strlen($xtrafilter) > 1) {
			$xtrafilter = "AND " . $xtrafilter;
		}
		$query = sprintf("SELECT D.Kode AS Code,"
				 . " D.Naam AS Diploma,"
				 . " O.Naam AS `Uitgegeven door`,"
				 . " LD.DatumBehaald AS dteDatum,"
				 . " LD.Diplomanummer AS Gegevens,"
				 . " IFNULL(LD.LicentieVervallenPer, IF(D.GELDIGH>0, DATE_ADD(LD.DatumBehaald, INTERVAL D.GELDIGH MONTH), IF((NOT D.Vervallen IS NULL), D.Vervallen, null))) AS `dteGeldigheid vervalt per`"
				 . " FROM %1\$sLiddipl AS LD INNER JOIN (%1\$sDiploma AS D INNER JOIN %1\$sOrganisatie AS O ON D.ORGANIS = O.Nummer) ON LD.DiplomaID = D.RecordID"
				 . " WHERE LD.DatumBehaald <= CURDATE() AND LD.LaatsteBeoordeling=1 AND ((D.Vervallen IS NULL) OR D.Vervallen > CURDATE()) AND LD.Lid=%2\$d %3\$s"
				 . " ORDER BY D.Volgnr, D.Kode, LD.DatumBehaald DESC;", TABLE_PREFIX, $lidid, $xtrafilter);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "vervallenbinnenkort") {
		$query = sprintf("SELECT D.Kode AS Code,"
				 . " D.Naam AS Diploma,"
				 . " LD.DatumBehaald AS DatumBehaald,"
				 . " LD.Diplomanummer,"
				 . " IFNULL(LD.LicentieVervallenPer, IF(D.GELDIGH>0, DATE_ADD(LD.DatumBehaald, INTERVAL D.GELDIGH MONTH), NULL)) AS VervaltPer"
				 . " FROM %1\$sLiddipl AS LD INNER JOIN %1\$sDiploma AS D ON LD.DiplomaID=D.RecordID"
				 . " WHERE D.Zelfservice=1 AND ((D.Vervallen IS NULL) OR D.Vervallen > CURDATE()) AND ((NOT LD.LicentieVervallenPer IS NULL) OR D.GELDIGH > 0) AND LD.Lid=%2\$d"
				 . " AND IFNULL(LD.LicentieVervallenPer, DATE_ADD(LD.DatumBehaald, INTERVAL D.GELDIGH MONTH)) < DATE_ADD(CURDATE(), INTERVAL %3\$d MONTH) AND LD.LicentieVervallenPer > DATE_ADD(CURDATE(), INTERVAL -%3\$d MONTH)"
				 . " ORDER BY D.Volgnr, D.Kode, LD.DatumBehaald DESC;", TABLE_PREFIX, $lidid, db_param("termijnvervallendiplomasmelden"));
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "diplomabehaald" and $lidid > 0 and $did > 0) {
		$query = sprintf("SELECT * FROM %sLiddipl WHERE Lid=%d AND DiplomaID=%d ORDER BY DatumBehaald DESC;", TABLE_PREFIX, $lidid, $did);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "diplomalid" and $lidid > 0 and $did > 0) {
		$query = sprintf("SELECT * FROM %sLiddipl WHERE Lid=%d AND DiplomaID=%d ORDER BY DatumBehaald;", TABLE_PREFIX, $lidid, $did);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "opschonen") {
		$query = sprintf("DELETE FROM %1\$sLiddipl WHERE DiplomaID NOT IN (SELECT RecordID FROM %1\$sDiploma);", TABLE_PREFIX);
		fnQuery($query, 2);
		
		$query = sprintf("DELETE FROM %sLiddipl WHERE DatumBehaald > CURDATE();", TABLE_PREFIX);
		fnQuery($query, 2);
	}
} # db_liddipl

function db_evenement($actie, $lidid=0, $eventid=0, $opm="", $stat="", $functie="") {
	global $selectnaam, $selectzoeknaam;
	
	$sqlaantdln = sprintf("SELECT COUNT(*) FROM %sEvenement_Deelnemer AS ED WHERE ED.EvenementID = E.RecordID AND Status='B'", TABLE_PREFIX);
	$sqlaantafgemeld = sprintf("SELECT COUNT(*) FROM %sEvenement_Deelnemer AS ED WHERE ED.EvenementID = E.RecordID AND Status='X'", TABLE_PREFIX);

	$xtra_where = "";
	$mess = "";
	$lg = db_lidond("lidgroepen");
	if ($actie == "oms") {
		$query = sprintf("SELECT Omschrijving FROM %sEvenement WHERE RecordID=%d;", TABLE_PREFIX, $eventid);
		return db_scalar($query);
		
	} elseif ($actie == "komende") {
		$query = sprintf("SELECT E.Datum, E.Omschrijving
								FROM %sEvenement AS E
								WHERE E.Datum >= CURDATE() AND E.Datum < DATE_ADD(CURDATE(), INTERVAL 2 MONTH) AND E.VerwijderdOp < '2012-01-01' AND E.ZichtbaarVoor IN %s
								ORDER BY E.Datum LIMIT 5;", TABLE_PREFIX, $lg);
		$result = fnQuery($query);
		return $result->fetchAll();

	} elseif ($actie == "open") {
		$query = sprintf("SELECT E.*, ET.Omschrijving AS TypeEvenement
								FROM %1\$sEvenement AS E INNER JOIN %1\$sEvenement_Type AS ET ON E.TypeEvenement = ET.RecordID
								WHERE E.Datum >= SYSDATE() AND E.VerwijderdOp < '2012-01-01' AND InschrijvingOpen=1
								AND E.BeperkTotGroep IN %2\$s
								ORDER BY E.Datum;", TABLE_PREFIX, $lg);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "overzicht" or $actie == "overzichtbeheer") {
		if ($actie == "overzichtbeheer") {
			$where = "E.Datum >= DATE_ADD(CURDATE(), INTERVAL -3 MONTH) AND E.VerwijderdOp < '2012-01-01'";
		} else {
			$where = sprintf("E.Datum >= DATE_ADD(CURDATE(), INTERVAL -3 DAY) AND E.VerwijderdOp < '2012-01-01' AND E.ZichtbaarVoor IN %s", $lg);
		}
		$query = sprintf("SELECT E.RecordID AS lnkNummer, E.Datum AS dteDatum, DATE_FORMAT(E.Datum, '%%H:%%i') AS Starttijd, E.Omschrijving, E.Locatie,
								E.Email AS `emlEmail`, E.Verzameltijd, E.Eindtijd, ET.Soort AS ndSoort,
								CASE E.InschrijvingOpen WHEN 0 THEN 'Nee' ELSE 'Ja' END AS `Ins. open?`,
								(%1\$s) AS `Aantal dln`,
								ET.Omschrijving AS `Type`
								FROM %2\$sEvenement AS E LEFT OUTER JOIN %2\$sEvenement_Type AS ET ON E.TypeEvenement = ET.RecordID
								WHERE %3\$s
								ORDER BY E.Datum;", $sqlaantdln, TABLE_PREFIX, $where);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "add") {
		$query = sprintf("INSERT INTO %sEvenement SET IngevoerdDoor=%d;", TABLE_PREFIX, $_SESSION['lidid']);
		$id = fnQuery($query);
		$mess = sprintf("Evenement %d is toegevoegd.", $id);
		db_logboek("add", $mess, 7, 0, 0, $id);
		return $id;
		
	} elseif ($actie == "inschrijving" and $lidid > 0 and $eventid > 0) {
		$query = sprintf("SELECT ED.RecordID, ED.Opmerking, ED.Status
								FROM %1\$sEvenement AS E INNER JOIN %1\$sEvenement_Deelnemer AS ED ON E.RecordID = ED.EvenementID
								WHERE ED.LidID=%2\$d AND ED.EvenementID=%3\$d;", TABLE_PREFIX, $lidid, $eventid);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "record" and $eventid > 0) {
		$query = sprintf("SELECT E.*, %1\$s AS GewijzigdDoorNaam, (%2\$s) AS AantDln, (%3\$s) AS AantAfgemeld
								FROM %4\$sEvenement AS E LEFT OUTER JOIN %4\$sLid AS L ON E.GewijzigdDoor = L.Nummer
								WHERE E.RecordID=%5\$d;", $selectnaam, $sqlaantdln, $sqlaantafgemeld, TABLE_PREFIX, $eventid);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "types") {
		$query = sprintf("SELECT RecordID, Omschrijving, Soort FROM %sEvenement_Type ORDER BY Omschrijving;", TABLE_PREFIX);
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
								ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", $selectnaam, TABLE_PREFIX, $eventid, $xtra_where);
		$result = fnQuery($query);
		return $result->fetchAll();
				
	} elseif ($actie == "aantaldlnrs") {
		if ($lidid > 0) {
			$where = sprintf("WHERE ED.LidID=%d", $lidid);
		} elseif ($eventid > 0) {
			$where = sprintf("WHERE ED.EvenementID=%d", $eventid);
		} elseif (strlen($stat) == 1) {
			$where = sprintf("WHERE ED.Status='%s'", $stat);
		} elseif (strlen($stat) > 1) {
			$where = sprintf("WHERE ED.Status IN (%s)", $stat);
		} else {
			$where = "";
		}
		$query = sprintf("SELECT COUNT(*) FROM %sEvenement_Deelnemer AS ED %s;", TABLE_PREFIX, $where);
		return db_scalar($query);
		
	} elseif ($actie == "potdeelnemers") {
		$where = "";
		$query = sprintf("SELECT BeperkTotGroep FROM %sEvenement WHERE RecordID=%d;", TABLE_PREFIX, $eventid);
		$groepid = db_scalar($query);
		if ($groepid > 0) {
			$where = sprintf("AND LO.OnderdeelID=%d", $groepid);
		}
		$query = sprintf("SELECT DISTINCT L.Nummer, %1\$s AS Naam
								FROM %2\$sLid AS L LEFT OUTER JOIN %2\$sLidond AS LO ON L.Nummer = LO.Lid
								WHERE (SELECT COUNT(*) FROM %2\$sEvenement_Deelnemer AS ED WHERE L.Nummer=ED.LidID AND ED.EvenementID=%3\$d)=0 %4\$s
								ORDER BY %1\$s;", $selectzoeknaam, TABLE_PREFIX, $eventid, $where);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "opschonen") {
		$query = sprintf("DELETE FROM %1\$sEvenement_Deelnemer WHERE LidID NOT IN (SELECT RecordID FROM %1\$sLid);", TABLE_PREFIX);
		fnQuery($query, 2);
		
		$query = sprintf("SELECT * FROM %sEvenement WHERE VerwijderdOp > '2000-01-01';", TABLE_PREFIX);
		foreach(fnQuery($query)->fetchAll() as $row) {
			$contrqry = sprintf("SELECT COUNT(*) FROM %sEvenement_Deelnemer WHERE EvenementID=%d;", TABLE_PREFIX, $row->RecordID);
			if (db_scalar($contrqry) ==  0) {
				$delqry = sprintf("DELETE FROM %sEvenement WHERE RecordID=%d;", TABLE_PREFIX, $row->RecordID);
				if (fnQuery($delqry) > 0) {
					$mess = sprintf("Evenement %d (%s) is definitief verwijderd.", $row->RecordID, $row->Omschrijving);
					db_logboek("add", $mess, 7, 0, 1);
				}
			}
		}

		$query = sprintf("DELETE FROM %1\$sEvenement_Deelnemer WHERE EvenementID NOT IN (SELECT RecordID FROM %1\$sEvenement);", TABLE_PREFIX);
		fnQuery($query, 2);
		
	} else {
		$mess = sprintf("Actie '%s' is onbekend.", $actie);
		debug($mess, 1, 0, 1);
	}
} # db_evenement

function db_Evenement_Deelnemer($actie, $recid, $fld="", $val="", $lidid=0, $eventrow="") {
	$mess = "";
	$rv = 0;
	
	if ($actie == "record") {
		if ($recid > 0) {
			$w = sprintf("ED.RecordID=%d", $recid);
		} else {
			$w = sprintf("ED.LidID=%d AND ED.EvenementID=%d", $lidid, $eventrow->RecordID);
		}
		$query = sprintf("SELECT ED.*, E.Omschrijving AS OmsEvenement FROM %1\$sEvenement_Deelnemer AS ED INNER JOIN %1\$sEvenement AS E ON ED.EvenementID=E.RecordID WHERE %2\$s;", TABLE_PREFIX, $w);
		$result = fnQuery($query);
		return $result->fetch();
	
	} elseif ($actie == "insert") {
		$query = sprintf("INSERT INTO %sEvenement_Deelnemer (LidID, EvenementID, Status, Opmerking, Functie, IngevoerdDoor) VALUES (%d, %d, '%s', '', '', %d);", TABLE_PREFIX, $lidid, $eventrow->RecordID, $eventrow->StandaardStatus, $_SESSION['lidid']);
		$rv = fnQuery($query);
		if ($rv > 0) {
			$mess = sprintf("Deelnemer %s bij evenement %d (%s) met status '%s' toegevoegd. ", db_naamlid($lidid), $eventrow->RecordID, $eventrow->Omschrijving, $eventrow->StandaardStatus);
		}
		
	} elseif ($actie == "update") {
		$edrow = db_Evenement_Deelnemer("record", $recid);
		$oldval = $edrow->$fld;
		if ($oldval != $val) {
			$query = sprintf("UPDATE %1\$sEvenement_Deelnemer SET `%2\$s`='%3\$s', GewijzigdDoor=%4\$d WHERE RecordID=%5\$d;", TABLE_PREFIX, $fld, $val, $_SESSION['lidid'], $recid);
			$rv = fnQuery($query);
			if ($rv > 0) {
				$mess = sprintf("Van deelnemer %s bij evenement %d (%s) is kolom '%s' van '%s' naar '%s' bijgewerkt. ", db_naamlid($edrow->LidID), $edrow->EvenementID, $edrow->OmsEvenement, $fld, $oldval, $val);
			}
		}
		
	} elseif ($actie == "delete") {
		if ($recid > 0) {
			$edrow = db_Evenement_Deelnemer("record", $recid);
			$query = sprintf("DELETE FROM %sEvenement_Deelnemer WHERE LidID=%d AND RecordID=%d;", TABLE_PREFIX, $lidid, $recid);
			$rv = fnQuery($query);
			if ($rv > 0) {
				$mess = sprintf("Deelnemer %s bij evenement %d (%s) verwijderd. ", db_naamlid($lidid), $edrow->EvenementID, $edrow->OmsEvenement);
			}
		}
	} else {
		$mess = sprintf("Actie '%s' is onbekend of RecordID is 0.", $actie);
	}
	if (strlen($mess) > 0) {
		db_logboek("add", $mess, 7, 0, 0, $edrow->EvenementID);
	}
	
	return $rv;
	
}  #  db_Evenement_Deelnemer

function db_Evenement_Type($actie, $etid=0, $colname="", $newval="") {
	if ($actie == "insert") {
		$query = sprintf("INSERT INTO %sEvenement_Type (Omschrijving) VALUES ('%s');", TABLE_PREFIX, $newval);
		$etid = fnQuery($query);
		if ($etid > 0) {
			$mess = sprintf("Evenement type %d is toegevoegd.", $etid);
		} else {
			$mess = sprintf("Evenement type '%s' kon niet worden toegevoegd.", $newval);
		}
		db_logboek("add", $mess, 7, 0, 1, $etid);
		return $etid;
		
	} elseif ($actie == "update" and $etid > 0 and strlen($colname) > 0) {
		$query = sprintf("UPDATE %sEvenement_Type SET `%s`='%s' WHERE RecordID=%d;", TABLE_PREFIX, $colname, $newval, $etid);
		if (fnQuery($query) > 0) {
			$mess = sprintf("Kolom '%s' in Evenement type %d is in '%s' gewijzigd.", $colname, $etid, $newval);
			db_logboek("add", $mess, 7, 0, 1, $etid);
		}
		
	} elseif ($actie == "delete" and $etid > 0) {
		$query = sprintf("DELETE FROM %1\$sEvenement_Type WHERE RecordID=%2\$d AND RecordID NOT IN (SELECT TypeEvenement FROM %1\$sEvenement)", TABLE_PREFIX, $etid);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Evenement type %d is verwijderd.", $etid);
		} else {
			$mess = sprintf("Evenement type %d kon niet verwijderd worden, omdat het nog in gebruik is.", $etid);
		}
		db_logboek("add", $mess, 7, 0, 1, $etid);
	}
	
}  # db_Evenement_Type

function db_artikel($actie, $artikelid=0, $lidid=0) {

	$mess = "";
	$lg = db_lidond("lidgroepen");
	if ($actie == "overzichtbeheer") {
		$query = sprintf("SELECT *, CONCAT(Omschrijving, IF(LENGTH(Maat)>0, CONCAT(' (', Maat, ')'), '')) AS OmsMaat
					FROM %sWS_Artikel ORDER BY Code, Omschrijving, RecordID;", TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "select") {
		$query = sprintf("SELECT *, CONCAT(Omschrijving, IF(LENGTH(Maat)>0, CONCAT(' (', Maat, ')'), '')) AS OmsMaat
					FROM %1\$sWS_Artikel AS A
					WHERE (A.RecordID NOT IN (SELECT Artikel FROM %1\$sWS_Orderregel WHERE Lid=%2\$d))
					AND A.VervallenPer >= CURDATE()
					ORDER BY Code, Omschrijving, RecordID;", TABLE_PREFIX, $lidid);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "code" and $artikelid > 0) {
		$query = sprintf("SELECT IF(LENGTH(Code)=0, RecordID, Code) FROM %sWS_Artikel WHERE RecordID=%d;", TABLE_PREFIX, $artikelid);
		return db_scalar($query);
		
	} elseif($actie == "aantal") {
		$query = sprintf("SELECT COUNT(*) FROM %sWS_Artikel;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "artikelbeschikbaar") {
		$query = sprintf("SELECT * FROM %sWS_Artikel
					WHERE BeschikbaarTot >= CURDATE() AND (BeperkTotGroep=0 OR BeperkTotGroep IN %s)
					ORDER BY Code, Omschrijving, RecordID;", TABLE_PREFIX, $lg);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "add") {
		$query = sprintf("INSERT INTO %sWS_Artikel SET IngevoerdDoor=%d;", TABLE_PREFIX, $_SESSION['lidid']);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Artikel %d is toegevoegd.", $result);
			db_logboek("add", $mess, 10, 0, 0, $result);
		}

	} elseif ($actie == "delete" and $artikelid > 0) {
		if (db_artikel("ingebruik", $artikelid) > 0) {
			$mess = sprintf("Artikel %s is niet verwijderd, omdat het nog in gebruik is.", db_artikel("code", $artikelid));
		} else {
			$query = sprintf("DELETE FROM %sWS_Artikel WHERE RecordID=%d;", TABLE_PREFIX, $artikelid);
			$result = fnQuery($query);
			if ($result > 0) {
				$mess = sprintf("Artikel %s is verwijderd.", db_artikel("code", $artikelid));
			} else {
				$mess = sprintf("Artikel %s is niet verwijderd.", db_artikel("code", $artikelid));
			}
		}
		db_logboek("add", $mess, 10, 0, 0, $artikelid);
		
	} elseif ($actie == "prijs" and $artikelid > 0) {
		$query = sprintf("SELECT Verkoopprijs FROM %sWS_Artikel WHERE RecordID=%d;", TABLE_PREFIX, $artikelid);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "ingebruik") {
		$query = sprintf("SELECT COUNT(*) FROM %sWS_Orderregel WHERE Artikel=%d;", TABLE_PREFIX, $artikelid);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "opschonen") {
		$query = sprintf("DELETE FROM %1\$sWS_Artikel WHERE BeschikbaarTot < CURDATE() AND RecordID NOT IN (SELECT Artikel FROM %1\$sWS_Orderregel);", TABLE_PREFIX);
		$result = fnQuery($query, 2);

	}
} # db_artikel

function db_orderregel($actie, $lidid=0, $artikelid=0, $aantalbesteld=0, $filter="", $recordid=0, $ordernr=0) {
	global $selectnaam;
	
	$mess = "";
	if ($actie == "bestelling" and $lidid > 0 and $artikelid > 0) {
		$query = sprintf("SELECT * FROM %sWS_Orderregel WHERE Lid=%d AND Artikel=%d;", TABLE_PREFIX, $lidid, $artikelid);
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
					ORDER BY A.Code;", $selectnaam, TABLE_PREFIX, $lidid, $filter);
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
					ORDER BY IF(ORD.Artikel > 0 AND ORD.AantalBesteld > 0, 1, 0), L.Achternaam, L.Roepnaam, A.Code;", $selectnaam, TABLE_PREFIX, $filter);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "totalen") {
		$query = sprintf("SELECT A.Code, A.Omschrijving, A.Maat,
					SUM(ORD.AantalBesteld) AS intBesteld, SUM(ORD.AantalGeleverd) AS intGeleverd, SUM(ORD.AantalBesteld * ORD.PrijsPerStuk) AS `curBedrag besteld`,
					SUM(ORD.AantalBesteld-ORD.AantalGeleverd) AS `intNog te leveren`, SUM(ORD.AantalGeleverd * ORD.PrijsPerStuk) AS `curBedrag geleverd`
					FROM %1\$sWS_Orderregel AS ORD INNER JOIN %1\$sWS_Artikel AS A ON ORD.Artikel = A.RecordID
					WHERE (ORD.Ordernr=0 OR (NOT ORD.BestellingDefinitief IS NULL)) AND VervallenPer >= CURDATE()
					GROUP BY A.Code, A.Omschrijving, A.Maat
					ORDER BY A.Code, A.Omschrijving, A.Maat;", TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "maxordernr") {
		$query = sprintf("SELECT MAX(IFNULL(Ordernr, 0)) FROM %sWS_Orderregel;", TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "aantal") {
		$query = sprintf("SELECT COUNT(*) FROM %sWS_Orderregel;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "add") {
		$query = sprintf("INSERT INTO %sWS_Orderregel SET Lid=%d, Artikel=%d, AantalBesteld=%d, Ordernr=%d, IngevoerdDoor=%d;", TABLE_PREFIX, $lidid, $artikelid, $aantalbesteld, $ordernr, $_SESSION['lidid']);
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
			$query = sprintf("DELETE FROM %sWS_Orderregel WHERE RecordID=%d;", TABLE_PREFIX, $recordid);
		} else {
			$query = sprintf("DELETE FROM %sWS_Orderregel WHERE Lid=%d AND Artikel=%d AND ISNULL(AantalGeleverd)=0;", TABLE_PREFIX, $lidid, $artikelid);
		}
		$result = fnQuery($query);
		if ($result > 0) {
			if ($recordid > 0) {
				$mess = sprintf("Orderregel %d is verwijderd.", $recordid);
			} else {
				$mess = sprintf("De bestelling van artikel %d bij %s is verwijderd.", $artikelid, db_naamlid($lidid));
			}
			db_logboek("add", $mess, 10, $lidid);
		}
		return $mess;
		
	} elseif ($actie == "defmaken" and $lidid > 0) {
		$query = sprintf("UPDATE %sWS_Orderregel SET BestellingDefinitief=SYSDATE(), GewijzigdDoor=%d WHERE Lid=%d AND (BestellingDefinitief IS NULL) AND (NOT PrijsPerStuk IS NULL);", TABLE_PREFIX, $_SESSION['lidid'], $lidid);
		$result = fnQuery($query);
		
	} elseif ($actie == "opschonen") {
		$query = sprintf("DELETE FROM %sWS_Orderregel WHERE AantalBesteld=0 AND AantalGeleverd=0 AND Ingevoerd <= DATE_ADD(CURDATE(), INTERVAL -1 WEEK);", TABLE_PREFIX);
		fnQuery($query, 2);
		
	} elseif ($actie == "vulprijsperstuk") {
		$query = sprintf("UPDATE %1\$sWS_Orderregel AS ORD SET PrijsPerStuk=(SELECT MAX(Verkoopprijs) FROM %1\$sWS_Artikel AS A WHERE A.RecordID=ORD.Artikel)
					WHERE (ORD.PrijsPerStuk IS NULL) AND ORD.Artikel > 0;", TABLE_PREFIX);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Bij %d orderregels is de prijs per stuk ingevuld.", $result);
			db_logboek("add", $mess, 10);
		}
		return $mess;
	}
} # db_orderregel

function db_rekening($actie, $nr=0, $filter="", $lidid=0) {
	global $selectlidnr;
	
	if ($actie == "minnummer") {
		$query = sprintf("SELECT MIN(Nummer) FROM %sRekening;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "maxnummer") {
		$query = sprintf("SELECT MAX(Nummer) FROM %sRekening;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "mindatum") {
		$query = sprintf("SELECT MIN(Datum) FROM %sRekening;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "maxdatum") {
		$query = sprintf("SELECT MAX(Datum) FROM %sRekening;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "aantal") {
		if ($lidid > 0) {
			$query = sprintf("SELECT COUNT(*) FROM %sRekening WHERE Lid=%d;", TABLE_PREFIX, $lidid);
		} else {
			$query = sprintf("SELECT COUNT(*) FROM %sRekening;", TABLE_PREFIX);
		}
		return db_scalar($query);

	} elseif ($actie == "hoofd" and $nr > 0) {
		$query = sprintf("SELECT RK.*, L.Adres, L.Postcode, L.Woonplaats, L.`Machtiging afgegeven` AS Machtiging,
					L.Bankrekening AS Bankrekeningnummer,
					DATE_ADD(RK.Datum, INTERVAL RK.BET_TERM*RK.BETAALDAG DAY) AS UitersteBetaaldatum,
					(%1\$1s) AS Lidnr
					FROM %2\$sRekening AS RK INNER JOIN %2\$sLid AS L ON RK.Lid = L.Nummer
					WHERE RK.Nummer=%3\$d;", $selectlidnr, TABLE_PREFIX, $nr);
		$result = fnQuery($query);
		return $result->fetch();
	} elseif ($actie == "regels" and $nr > 0) {
		$query = sprintf("SELECT RR.*, L.GEBDATUM FROM %1\$sRekreg AS RR LEFT JOIN %1\$sLid AS L ON RR.Lid = L.Nummer WHERE RR.Rekening=%2\$d;", TABLE_PREFIX, $nr);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "seizoenen") {
		$query = sprintf("SELECT DISTINCT Seizoen FROM %sRekening ORDER BY Seizoen DESC;", TABLE_PREFIX);
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
					ORDER BY RK.Nummer;", $selectlidnr, TABLE_PREFIX, $filter);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "histmail" and $nr > 0) {
		$query = sprintf("SELECT message FROM %sMailing_hist WHERE Xtra_Char='REK' AND Xtra_Num=%d ORDER BY send_on DESC LIMIT 1;", TABLE_PREFIX, $nr);
		return db_scalar($query);
	}
} # db_rekening

function db_Stukken($actie, $lidid=0) {
	
	if ($actie == "gewijzigdestukken") {
		$vl = db_logboek("vorigelogin", "", 0, $lidid);
		if (strlen($vl) >= 10) {
			$query = sprintf("SELECT Titel FROM %sStukken WHERE ((VervallenPer IS NULL) OR VervallenPer > CURDATE()) AND GewijzigdOp > '%s' ORDER BY GewijzigdOp DESC;", TABLE_PREFIX, $vl);
		} else {
			$query = sprintf("SELECT Titel FROM %sStukken WHERE ((VervallenPer IS NULL) OR VervallenPer > CURDATE()) ORDER BY GewijzigdOp DESC;", TABLE_PREFIX);
		}
		return fnQuery($query);
	}
	
}  # db_Stukken

function db_dms() {
}

function db_param($naam, $actie="curval", $newval="") {
	
	if (isset($_SESSION['settings'])) {
		$settings = $_SESSION['settings'];
	}
	
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
	$arrIntegers['mailing_mailopnieuw'] = 0;
	$arrIntegers['mailing_rekening_stuurnaar'] = 1;
	$arrIntegers['mailing_type_editor'] = 1;
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
	
	$arrBooleans['kaderoverzichtmetfoto'] = 1;
	$arrBooleans['toonpasfotoindiennietingelogd'] = 0;
	$arrBooleans['meisjesnaamtonen'] = 1;
	$arrBooleans['WoonadresAnderenTonen'] = 1;
	$arrBooleans['login_lidnrnodigbijaanvraag'] = 1;
	$arrBooleans['mailing_rekening_nulregelsweglaten'] = 0;
	$arrBooleans['mailing_resultaatversturen'] = 0;
	$arrBooleans['toneninschrijvingenbewakingen'] = 1;
	$arrBooleans['tonentoekomstigebewakingen'] = 0;
	$arrBooleans['zs_incl_beroep'] = 1;
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
		$query = sprintf("SELECT *, IFNULL(ValueNum, ValueChar) AS CurVal FROM %sAdmin_param ORDER BY Naam;", TABLE_PREFIX);
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
				$query = sprintf("SELECT 1 AS RecordID, 'T' AS ParamType, Naam AS ValueChar FROM %sVereniging;", TABLE_PREFIX);
			} elseif ($naam == "naamvereniging_afkorting") {
				$query = sprintf("SELECT 1 AS RecordID, 'T' AS ParamType, AfkortingNaam AS ValueChar FROM %sVereniging;", TABLE_PREFIX);
			} elseif ($naam == "emailsecretariaat") {
				$query = sprintf("SELECT 1 AS RecordID, 'T' AS ParamType, EmailSecretariaat AS ValueChar FROM %sVereniging;", TABLE_PREFIX);
			} elseif ($naam == "emailledenadministratie") {
				$query = sprintf("SELECT 1 AS RecordID, 'T' AS ParamType, EmailLedenadministratie AS ValueChar FROM %sVereniging;", TABLE_PREFIX);
			} elseif ($naam == "laatstebackup") {
				$query = sprintf("SELECT 1 AS RecordID, 'T' AS ParamType, MAX(DatumTijd) AS ValueChar FROM %sAdmin_activiteit WHERE TypeActiviteit=3;", TABLE_PREFIX);
			} else {
				$query = sprintf("SELECT IFNULL(RecordID, 0) AS RecordID, ParamType, ValueNum, ValueChar FROM %sAdmin_param WHERE Naam='%s';", TABLE_PREFIX, $naam);
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
		$query = sprintf("SELECT * FROM %sAdmin_param WHERE Naam='%s';", TABLE_PREFIX, $naam);
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
			$query = sprintf("UPDATE %sAdmin_param SET %s WHERE RecordID=%d;", TABLE_PREFIX, $set, $cur->RecordID);
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
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_param WHERE Naam LIKE '%s';", TABLE_PREFIX, $naam);
		if (db_scalar($query) == 0) {
			$query = sprintf("INSERT INTO %sAdmin_param SET Naam='%s', ParamType='%s', IngevoerdDoor=%d, GewijzigdDoor=%d;", TABLE_PREFIX, $naam, $pt, $_SESSION['lidid'], $_SESSION['lidid']);
			$result = fnQuery($query);
			if ($result > 0) {
				$mess = sprintf("Parameter '%s' is toegevoegd.", $naam);
				db_logboek("add", $mess, 13, 0, 0, $result);
			}
		}
		
	} elseif ($actie == "delete" and strlen($naam) > 0) {
		$query = sprintf("DELETE FROM %sAdmin_param WHERE Naam='%s';", TABLE_PREFIX, $naam);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Parameter '%s' is verwijderd.", $naam);
			db_logboek("add", $mess, 13);
		}
	}
}

function db_delete_local_tables() {
	global $arrTables;
	
	foreach ($arrTables as $key => $tn) {
		if ($key >= 20) {
			fnQuery(sprintf("DELETE FROM %s%s;", TABLE_PREFIX, $tn), 2);
		}
	}
}

function db_onderhoud($type=9) {
	/* Type uitleg
	1 = na upload
	2 = zonder optimize tables
	*/
	global $arrTables, $db_name, $wherelid, $wherelidond, $lididwebmasters;
	
	if ($type == 1) {
		// Vaste aanpassingen aan de database na een upload, omdat MS-Access geen datum velden kent, alleen datum_tijd.
		fnQuery(sprintf("ALTER TABLE %sBewaking CHANGE `BEGIN_PER` `BEGIN_PER` DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sBewaking CHANGE `EINDE_PER` `EINDE_PER` DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sBewseiz CHANGE Begindatum Begindatum DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sBewseiz CHANGE `Einde` `Einde` DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sBewseiz CHANGE Geboren Geboren DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sDiploma CHANGE `Vervallen` `Vervallen` DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sFunctie CHANGE `Vervallen per` `Vervallen per` DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sLid CHANGE `GEBDATUM` `GEBDATUM` DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sLid CHANGE `Overleden` `Overleden` DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sLid CHANGE `VOG afgegeven` `VOG afgegeven` DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sLiddipl CHANGE `DatumBehaald` `DatumBehaald` DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sLiddipl CHANGE `LicentieVervallenPer` `LicentieVervallenPer` DATE DEFAULT NULL;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sLidmaatschap CHANGE `LIDDATUM` `LIDDATUM` DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sLidmaatschap CHANGE `Opgezegd` `Opgezegd` DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sLidond CHANGE `Vanaf` `Vanaf` DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sLidond CHANGE `Opgezegd` `Opgezegd` DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sLidRedNed CHANGE Geboortedatum Geboortedatum DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sMutatie CHANGE `Datum` `Datum` DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sOnderdl CHANGE `VervallenPer` `VervallenPer` DATE;", TABLE_PREFIX), 9);
		fnQuery(sprintf("ALTER TABLE %sRekening CHANGE `Datum` `Datum` DATE;", TABLE_PREFIX), 9);
		foreach ($arrTables as $key => $tn) {
			if ($key >= 20) {
				fnQuery(sprintf("ALTER TABLE %s%s CHANGE Ingevoerd Ingevoerd DATE;", TABLE_PREFIX, $tn), 9);
			}
		}
	
		// Overige vaste aanpassingen aan de database na een upload,
		db_addautoincrement("Lidond");
		db_addautoincrement("Liddipl");
		db_addautoincrement("Lidmaatschap");
		db_addautoincrement("Memo");
		$query = sprintf("ALTER TABLE `%sMemo` CHANGE `Vertrouwelijk` `Vertrouwelijk` TINYINT(4) NULL DEFAULT '0'", TABLE_PREFIX);
		fnQuery($query, 9);
	}
	
	/***** Aanpassen lengte login in de database als deze kleiner is dan login_maxlengte  *****/
	$table = TABLE_PREFIX . "Admin_login";
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE 'Login';", $db_name, $table);
	if (db_scalar($query) < db_param("login_maxlengte")) {
		$query = sprintf("ALTER TABLE `%s` CHANGE `Login` `Login` VARCHAR( %s ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table, db_param("login_maxlengte"));
		fnQuery($query, 2);
	}
	
	/***** Aanpassen lengte wachtwoord in de database als deze kleiner is dan 255. Deze lengte is nodig voor de password_hash()  *****/
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE 'Wachtwoord';", $db_name, $table);
	if (db_scalar($query) < 255) {
		$query = sprintf("ALTER TABLE `%s` CHANGE `Wachtwoord` `Wachtwoord` VARCHAR(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table);
		fnQuery($query, 2);
	}

	/***** Aanpassen lengte van de naam van paramters naar 50. Deze code kan na 1/1/2020 worden verwijderd. *****/
	$table = TABLE_PREFIX . "Admin_param";
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE 'Naam';", $db_name, $table);
	if (db_scalar($query) < 50) {
		$query = sprintf("ALTER TABLE `%s` CHANGE `Naam` `Naam` VARCHAR(50) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table);
		fnQuery($query, 2);
	}
	
	/***** Aanpassen lengte van het IP-adres in Admin_activiteit naar 45 voor IP v6 adressen. Deze code kan na 1/1/2020 worden verwijderd. *****/
	$table = TABLE_PREFIX . "Admin_activiteit";
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE 'IP_adres';", $db_name, $table);
	if (db_scalar($query) < 45) {
		$query = sprintf("ALTER TABLE `%s` CHANGE `IP_adres` `IP_adres` VARCHAR(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table);
		fnQuery($query, 2);
	}
	
	/***** Aanpassen lengte van to_addr en cc_addr in Mailing_hist naar 100. Deze code kan na 1/1/2020 worden verwijderd. *****/
	$table = TABLE_PREFIX . "Mailing_hist";
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE 'to_addr';", $db_name, $table);
	if (db_scalar($query) < 100) {
		$query = sprintf("ALTER TABLE `%s` CHANGE `to_addr` `to_addr` VARCHAR(100) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table);
		fnQuery($query, 2);
		$query = sprintf("ALTER TABLE `%s` CHANGE `cc_addr` `cc_addr` VARCHAR(100) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table);
		fnQuery($query, 2);
	}
	
	/***** Aanpassen type veld van Ingevoerd in Admin_login naar datumtijd. Deze code kan na 1/1/2020 worden verwijderd. *****/
	$query = sprintf("ALTER TABLE `%sAdmin_login` CHANGE `Ingevoerd` `Ingevoerd` DATETIME NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	
	/***** Aanpassen lengte van het RefFunction in Admin_activiteit naar 50. Deze code kan na 1/1/2020 worden verwijderd. *****/
	$table = TABLE_PREFIX . "Admin_activiteit";
	$col = "RefFunction";
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE '%s';", $db_name, $table, $col);
	if (db_scalar($query) < 50) {
		$query = sprintf("ALTER TABLE `%1\$s` CHANGE `%2\$s` `%2\$s` VARCHAR(50) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table, $col);
		fnQuery($query, 2);
	}
	
	/***** Velden die later zijn toegevoegd. *****/
	
	// Toevoegen veld 'NamenOuders' aan de tabel 'Lid'. Deze code kan na 1/12/2019 worden verwijderd.
	$tab = TABLE_PREFIX . "Lid";
	$col = "NamenOuders";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` varchar(90);", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'ActivatieKey' aan de tabel 'Admin_login'. Deze code kan na 1/12/2020 worden verwijderd.
	$tab = TABLE_PREFIX . "Admin_login";
	$col = "ActivatieKey";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(255);", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'LoginWebsite' aan de tabel 'Lid'. Deze code kan na 1/12/2019 worden verwijderd.
	$tab = TABLE_PREFIX . "Lid";
	$col = "LoginWebsite";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` varchar(20);", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'Getoond' aan de tabel 'Admin_activiteit'. Deze code kan na 1/12/2019 worden verwijderd.
	$tab = TABLE_PREFIX . "Admin_activiteit";
	$col = "Getoond";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` tinyint;", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'RefFunction' aan de tabel 'Admin_activiteit'. Deze code kan na 1/12/2019 worden verwijderd.
	$tab = TABLE_PREFIX . "Admin_activiteit";
	$col = "RefFunction";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` varchar(40);", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld '2FA' aan de tabel 'Admin_login'. Deze code kan na 1/1/2020 worden verwijderd.
	$tab = TABLE_PREFIX . "Admin_login";
	$col = "2FA";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` TINYINT;", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'LaatsteWachtwoordWijziging' aan de tabel 'Admin_login'. Deze code kan na 1/1/2020 worden verwijderd.
	$tab = TABLE_PREFIX . "Admin_login";
	$col = "LaatsteWachtwoordWijziging";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` DATETIME;", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'HTMLdirect' aan de tabel 'Mailing'. Deze code kan na 1/11/2020 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing";
	$col = "HTMLdirect";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` TINYINT DEFAULT '0';", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'Locatie' aan de tabel 'Evenement'. Deze code kan na 1/12/2020 worden verwijderd.
	$tab = TABLE_PREFIX . "Evenement";
	$col = "Locatie";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(75);", $tab, $col);
		fnQuery($query, 2);
	}
		
	// Toevoegen veld 'IngevoerdDoor' aan de tabel 'Evenement_Deelnemer'. Deze code kan na 1/12/2020 worden verwijderd.
	$tab = TABLE_PREFIX . "Evenement_Deelnemer";
	$col = "IngevoerdDoor";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NOT NULL DEFAULT '0' AFTER `Ingevoerd`;", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'IngevoerdDoor' aan de tabel 'Evenement'. Deze code kan na 1/12/2020 worden verwijderd.
	$tab = TABLE_PREFIX . "Evenement";
	$col = "IngevoerdDoor";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NOT NULL DEFAULT '0' AFTER `Ingevoerd`;", $tab, $col);
		fnQuery($query, 2);
	}
	
	/***** Velden die aangepast zijn. *****/
	
	// Admin_Login, deze code kan na 31/12/2019 worden verwijderd.
	$query = sprintf("ALTER TABLE %sAdmin_login CHANGE `Ingevoerd` `Ingevoerd` DATETIME;", TABLE_PREFIX);
	fnQuery($query);
	
	// Deze code kan na 1/12/2020 worden verwijderd.
	$query = sprintf("ALTER TABLE `%sEvenement_Deelnemer` CHANGE `Functie` `Functie` VARCHAR(30) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sEvenement_Deelnemer` CHANGE `Opmerking` `Opmerking` VARCHAR(75) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sEvenement_Deelnemer` CHANGE `Ingevoerd` `Ingevoerd` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sEvenement_Deelnemer` CHANGE `GewijzigdDoor` `GewijzigdDoor` INT(11) NOT NULL DEFAULT '0';", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sEvenement` CHANGE `Ingevoerd` `Ingevoerd` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sAdmin_access` CHANGE `Ingevoerd` `Ingevoerd` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sAdmin_activiteit` CHANGE `DatumTijd` `DatumTijd` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sAdmin_interface` CHANGE `Ingevoerd` `Ingevoerd` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sAdmin_login` CHANGE `Ingevoerd` `Ingevoerd` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sAdmin_param` CHANGE `Ingevoerd` `Ingevoerd` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sMailing` CHANGE `new_on` `new_on` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sMailing` CHANGE `changed_on` `changed_on` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sMailing_rcpt` CHANGE `Ingevoerd` `Ingevoerd` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sMailing_vanaf` CHANGE `Ingevoerd` `Ingevoerd` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sWS_Artikel` CHANGE `Ingevoerd` `Ingevoerd` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sWS_Orderregel` CHANGE `Ingevoerd` `Ingevoerd` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sWS_Artikel` CHANGE `Gewijzigd` `Gewijzigd` TIMESTAMP on update CURRENT_TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sWS_Orderregel` CHANGE `Gewijzigd` `Gewijzigd` TIMESTAMP on update CURRENT_TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sEvenement_Type` CHANGE `Ingevoerd` `Ingevoerd` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sBewaking_Blok` CHANGE `Ingevoerd` `Ingevoerd` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sMailing_vanaf` CHANGE `Gewijzigd` `Gewijzigd` TIMESTAMP on update CURRENT_TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	fnQuery($query);
	
	/***** Opschonen database na een upload uit de Access-database.  *****/

	if ($type == 1) {
		db_diploma("opschonen");
		db_liddipl("opschonen");
	
		$query = sprintf("DELETE FROM %sLid WHERE (NOT Verwijderd IS NULL);", TABLE_PREFIX);
		fnQuery($query, 2);
	
		$query = sprintf("DELETE FROM %sOnderdl WHERE (`Type` NOT IN ('A', 'G')) AND Kader<>1 AND (NOT VervallenPer IS NULL) AND VervallenPer < CURDATE();", TABLE_PREFIX);
		fnQuery($query, 2);
	
		$query = sprintf('DELETE FROM %1$sLidond WHERE OnderdeelID NOT IN (SELECT RecordID FROM %1$sOnderdl);', TABLE_PREFIX);
		fnQuery($query, 2);
	
		$query = sprintf("DELETE FROM %sGBR WHERE Balans=1;", TABLE_PREFIX);
		fnQuery($query, 2);
		$query = sprintf('DELETE FROM %1$sMutatie WHERE GBR NOT IN (SELECT Kode FROM %1$sGBR);', TABLE_PREFIX);
		fnQuery($query, 2);
	}
	
	if (db_param("login_geldigheidactivatie") > 0) {
		$query = sprintf("UPDATE %1\$sAdmin_login SET ActivatieKey='', Wachtwoord='', LaatsteWachtwoordWijziging=SYSDATE(), Gewijzigd=SYSDATE()
					WHERE Ingelogd=0 AND LENGTH(ActivatieKey) > 5 AND Gewijzigd < ADDDATE(SYSDATE(), INTERVAL -%2\$d HOUR) AND (Gewijzigd IS NOT NULL) AND Gewijzigd > '2000-01-01';"
					, TABLE_PREFIX, db_param("login_geldigheidactivatie"));
		$result = fnQuery($query);
	}
	
	if ($type != 2) {
		foreach ($arrTables as $tn) {
			fnQuery(sprintf("OPTIMIZE TABLE %s%s;", TABLE_PREFIX, $tn));
		}
	}
	
	if ($_SERVER["HTTP_HOST"] != "phprbm.telling.nl") {
		//  Deze tabellen moeten op dit moment alleen in de ontwikkelomgeving aanwezig zijn. 
		$query = sprintf("DROP TABLE IF EXISTS %sDMS_Folder;", TABLE_PREFIX);
		fnQuery($query);
		$query = sprintf("DROP TABLE IF EXISTS %sDMS_Document;", TABLE_PREFIX);
		fnQuery($query);
	}
}

function db_addautoincrement($table) {
	global $db_name;
	
	$table = TABLE_PREFIX . $table;
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

function db_backup() {
	global $db_name, $arrTables;
	
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
					$tabs .= TABLE_PREFIX . $tnm . ",";
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
			db_logboek("add", $mess, 3, 0, 1);

			if ($handle = opendir($db_folderbackup)) {
				while (false !== ($file = readdir($handle))) {
					sleep(2);
					if ($file != "." and $file != "..") {
						$vbn = $db_folderbackup . $file;
						if (db_param("db_backupsopschonen") > 1 and filectime($db_folderbackup . $file) < mktime(0, 0, 0, date("m"), date("d")-db_param("db_backupsopschonen"), date("Y")) or filesize($db_folderbackup . $file) < 500 ) {
							unlink($vbn);
							$mess = sprintf("Bacckup-bestand %s is verwijderd. ", $file);
							db_logboek("add", $mess, 2, 0, 1);
						} elseif (fileperms($vbn) != 32768) {
							if (chmod($vbn, 0000) == true) {
								$mess = sprintf("Op bestand %s is chmod 0000 uitgevoerd.", $file);
							} else {
								$mess = sprintf("chmod 0000 op bestand %s is niet gelukt.", $vbn);
							}
							db_logboek("add", $mess, 2, 0, 1);
						}
					}
				}
			} else {
				$mess = "";
			}
			
		} else {
			db_logboek("add", "Het backup-script heeft gedraaid, maar de backup is niet gelukt.\n", 3, 0, 1);
		}
	} else {
		db_logboek("add", "Het backupscript heeft korter dan 1 uur geleden met success gedraaid. Het wordt niet nogmaals uitgevoerd.", 2, 0, 1);
	}
}

function fnFreeBackupFiles() {
	
	$mess = "Start fnFreeBackupFiles";
	db_logboek("add", $mess, 3, 0, 1);
	
	$db_folderbackup = db_param("db_folderbackup");
	$mess = "";
	if ($handle = opendir($db_folderbackup)) {
		while (false !== ($file = readdir($handle))) {
			sleep(2);
			if ($file != "." and $file != "..") {
				$vbn = $db_folderbackup . $file;
				if (chmod($vbn, 0755) == true) {
					$mess = sprintf("Op bestand %s is chmod 0755 uitgevoerd.", $file);
				} else {
					$mess = sprintf("chmod 0755 op bestand %s is niet gelukt.", $vbn);
				}
				db_logboek("add", $mess, 3, 0, 1);
			}
		}
	}
	
}  # fnFreeBackupfiles

function fnBackupTables($tables="*") {
	global $dbc;

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
	global $fromlid, $wherelid, $fromlidond, $wherelidond, $selectnaam;

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
			 . " ORDER BY Login.Ingevoerd DESC LIMIT 1;", $selectnaam, TABLE_PREFIX);
	$stats['nieuwstelogin'] = db_scalar($query);
	
	$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login AS Login;", TABLE_PREFIX);
	$stats['aantallogins'] = db_scalar($query);
	
	$query = sprintf("SELECT MAX(DatumTijd) FROM %sAdmin_activiteit WHERE TypeActiviteit=9;", TABLE_PREFIX);
	$stats['laatsteupload'] = db_scalar($query);
	
	$query = sprintf("SELECT %1\$s AS LidNaam FROM %2\$sLid AS L INNER JOIN %2\$sAdmin_login AS Login ON L.Nummer = Login.LidID"
			 . " WHERE Login.Ingelogd=1 ORDER BY Login.Ingevoerd DESC;", $selectnaam, TABLE_PREFIX);
	$result = fnQuery($query);
	$stats['nuingelogd'] = "";
	foreach($result->fetchAll() as $row) {
		if (strlen($stats['nuingelogd']) > 1) { $stats['nuingelogd'] .= ", "; }
		$stats['nuingelogd'] .= $row->LidNaam;
	}
	
	if ($lidid > 0) {
		$query = sprintf("SELECT L.Roepnaam FROM %sLid AS L WHERE L.Nummer=%d;", TABLE_PREFIX, $lidid);
		$stats['roepnaamingelogde'] = db_scalar($query);
		
		$query = sprintf("SELECT L.GEBDATUM FROM %sLid AS L WHERE L.Nummer=%d;", TABLE_PREFIX, $lidid);
		$stats['geboortedatumingelogde'] = db_scalar($query);
		
		$query = sprintf("SELECT L.Gewijzigd FROM %sLid AS L WHERE L.Nummer=%d;", TABLE_PREFIX, $lidid);
		$filter = sprintf(" WHERE Lid=%d", $lidid);
	} else {
		$stats['roepnaamingelogde'] = "gast";
		
		$query = sprintf("SELECT MAX(L.Gewijzigd) FROM %sLid AS L;", TABLE_PREFIX);
		$filter = "";
	}
	$result = fnQuery($query);
	$stats['laatstgewijzigd'] = $result->fetchColumn();
	
	$query = sprintf("SELECT MAX(BW.Gewijzigd) FROM %sBewaking AS BW%s;", TABLE_PREFIX, $filter);
	$result = fnQuery($query);
	$lgw = $result->fetchColumn();
	if ($lgw > $stats['laatstgewijzigd']) {
		$stats['laatstgewijzigd'] = $lgw;
	}
	
	$query = sprintf("SELECT MAX(LO.Gewijzigd) FROM %sLidond AS LO%s;", TABLE_PREFIX, $filter);
	$lgw = db_scalar($query);
	if ($lgw > $stats['laatstgewijzigd']) {
		$stats['laatstgewijzigd'] = $lgw;
	}
	
	if ($lidid > 0) {
		$query = sprintf("SELECT MAX(LD.Gewijzigd) FROM %sLiddipl AS LD%s;", TABLE_PREFIX, $filter);
		$lgw = db_scalar($query);
		if ($lgw > $stats['laatstgewijzigd']) {
			$stats['laatstgewijzigd'] = $lgw;
		}
	}
	
	$query = sprintf("SELECT MAX(RK.Gewijzigd) FROM %sRekening AS RK%s;", TABLE_PREFIX, $filter);
	$lgw = db_scalar($query);
	if ($lgw > $stats['laatstgewijzigd']) {
		$stats['laatstgewijzigd'] = $lgw;
	}	
	
	if ($lidid == 0) {
		$query = sprintf("SELECT MAX(M.Gewijzigd) FROM %sMutatie AS M;", TABLE_PREFIX);
		$lgw = db_scalar($query);
		if ($lgw > $stats['laatstgewijzigd']) {
			$stats['laatstgewijzigd'] = $lgw;
		}
	}	
	
	return $stats;
} # db_stats

function bestaatkolom($tabel, $kolom) {
	global $db_name;
	
	if (!startwith($tabel, TABLE_PREFIX)) {
		$tabel = TABLE_PREFIX . $tabel;
	}
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE '%s';", $db_name, $tabel, $kolom);
	if (db_scalar($query) == 1) {
		return true;
	} else {
		return false;
	}
	
} # bestaatkolom

function db_nieuwrecordID($tabel) {
	/* Change log
	ST20181221 RecordID's voor tabellen die uit Access komen moeten oneven zijn. Dit om het onderscheid te kunnen maken met RecordID's die uit de database zelf komen.
	*/
	global $arrTables;
	
	$rid = 0;
	
	if (!startwith($tabel, TABLE_PREFIX)) {
		$tabel = TABLE_PREFIX . $tabel;
	}
	
	if (bestaatkolom($tabel, "RecordID")) {
		$kol = "RecordID";
	} else {
		$kol = "Nummer";
	}

	$query = sprintf("SELECT MAX(IFNULL(%s, 0)) FROM %s;", $kol, $tabel);
	$rid = db_scalar($query) + 1;
	
	if (array_search(str_replace(TABLE_PREFIX, "", $tabel), $arrTables) >= 20 and ($rid / 2) == intval($rid / 2)) {
		$rid++;
	}
	return $rid;
	
} # db_nieuwrecordID


function db_createtables() {

	$queries = sprintf("SET SQL_MODE = 'NO_AUTO_VALUE_ON_ZERO';

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_access` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Tabpage` varchar(50) NOT NULL,
  `Toegang` int(11) NOT NULL DEFAULT '-1',
  `Ingevoerd` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_activiteit` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL COMMENT 'Wie heeft deze activiteit uitgevoerd?',
  `DatumTijd` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Omschrijving` text NOT NULL,
  `ReferID` int(11) NOT NULL COMMENT 'Op welk RecordID heeft deze activiteit betrekking?',
  `Script` varchar(75) NOT NULL,
  `TypeActiviteit` tinyint(4) DEFAULT '0',
  `IP_adres` varchar(45) NOT NULL,
  `USER_AGENT` varchar(110) DEFAULT NULL,
  `Getoond` tinyint(4) DEFAULT NULL COMMENT 'Is deze melding aan de gebruiker getoond?',
  `RefFunction` varchar(50) NOT NULL,
  PRIMARY KEY (`RecordID`),
  KEY `TypeActiviteit` (`TypeActiviteit`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_interface` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) DEFAULT NULL,
  `IP-adres` char(15) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL,
  `SQL-statement` text CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `IngelogdLid` int(11) DEFAULT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Gedownload` datetime DEFAULT NULL,
  `Afgemeld` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_login` (
  `LidID` int(11) NOT NULL,
  `Login` varchar(15) NOT NULL,
  `Wachtwoord` varchar(255) NOT NULL,
  `HerinneringVervallenDiplomas` tinyint(4) NOT NULL DEFAULT '0',
  `Ingevoerd` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Gewijzigd` datetime NOT NULL,
  `GewijzigdDoor` int(11) NOT NULL DEFAULT '0',
  `LastLogin` datetime DEFAULT NULL,
  `LastActivity` datetime NOT NULL,
  `Ingelogd` tinyint(4) NOT NULL DEFAULT '0',
  `FouteLogin` smallint(6) NOT NULL DEFAULT '0',
  `ActivatieKey` varchar(255) DEFAULT NULL,
  `2FA` tinyint(4) DEFAULT NULL,
  `LaatsteWachtwoordWijziging` datetime DEFAULT NULL,
  PRIMARY KEY (`LidID`),
  UNIQUE KEY `Login` (`Login`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_param` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Naam` varchar(50) CHARACTER SET utf8 NOT NULL,
  `ParamType` char(2) COLLATE utf8_unicode_ci DEFAULT NULL,
  `ValueChar` text COLLATE utf8_unicode_ci,
  `ValueNum` decimal(16,6) DEFAULT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
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
  `BEGIN_PER` date NOT NULL,
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
  `Ingevoerd` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
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
  `RecordID` int(11) NOT NULL,
  `Kode` varchar(8) DEFAULT NULL,
  `Naam` varchar(75) DEFAULT NULL,
  `Type` varchar(1) DEFAULT NULL,
  `ORGANIS` smallint(6) DEFAULT NULL,
  `MIN_LFT` smallint(6) DEFAULT NULL,
  `Volgnr` smallint(6) DEFAULT NULL,
  `EXAM_OND` smallint(6) DEFAULT NULL,
  `GELDIGH` int(11) DEFAULT NULL,
  `AantalBeoordelingen` smallint(6) DEFAULT NULL,
  `Vervallen` date DEFAULT NULL,
  `HistorieOpschonen` int(11) DEFAULT NULL,
  `VoorgangerID` int(11) DEFAULT NULL,
  `Alternatief` varchar(20) DEFAULT NULL,
  `Tonen in bewakingsadministratie` tinyint(4) DEFAULT NULL,
  `Zelfservice` tinyint(4) DEFAULT NULL,
  `RolID` int(11) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
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
  `Locatie` varchar(75) DEFAULT NULL,
  `Email` varchar(45) NOT NULL,
  `TypeEvenement` int(11) NOT NULL,
  `InschrijvingOpen` tinyint(4) NOT NULL DEFAULT '1',
  `StandaardStatus` char(1) NOT NULL DEFAULT 'I',
  `BeperkTotGroep` int(11) NOT NULL DEFAULT '0' COMMENT 'Welke groep mag zich voor dit evenement inschrijven? 0 = iedereen.',
  `ZichtbaarVoor` int(11) NOT NULL DEFAULT '0' COMMENT 'Voor welke groep is dit evenement zichtbaar? 0 = iedereen.',
  `Ingevoerd` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `GewijzigdDoor` int(11) NOT NULL,
  `VerwijderdOp` date NOT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sEvenement_Deelnemer` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL,
  `EvenementID` int(11) NOT NULL,
  `Functie` varchar(30) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `Opmerking` varchar(75) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `Status` char(1) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT 'I',
  `Aantal` int(11) NOT NULL DEFAULT '1',
  `Ingevoerd` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `IngevoerdDoor` int(11) NOT NULL DEFAULT '0',
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `GewijzigdDoor` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `LidEvenement` (`LidID`,`EvenementID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sEvenement_Type` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Omschrijving` varchar(30) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL,
  `Soort` char(1) NOT NULL DEFAULT 'W',
  `Ingevoerd` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `GewijzigdDoor` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sFunctie` (
  `Nummer` smallint(6) NOT NULL,
  `Omschrijv` varchar(35) DEFAULT NULL,
  `Oms_Vrouw` varchar(35) DEFAULT NULL,
  `Afkorting` varchar(10) DEFAULT NULL,
  `Sorteringsvolgorde` smallint(6) DEFAULT NULL,
  `Afdelingsfunctie` tinyint(4) DEFAULT NULL,
  `Ledenadministratiefunctie` tinyint(4) DEFAULT NULL,
  `Bewakingsfunctie` tinyint(4) DEFAULT NULL,
  `Kader` tinyint(4) DEFAULT NULL,
  `Vervallen per` date DEFAULT NULL,
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
  `VerdichtingID` int(11) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  `Standaard kostenplaats` int(11) DEFAULT NULL,
  PRIMARY KEY (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sGroep` (
  `RecordID` int(11) NOT NULL,
  `OnderdeelID` int(11) DEFAULT NULL,
  `Kode` varchar(5) DEFAULT NULL,
  `Omschrijving` varchar(45) DEFAULT NULL,
  `Instructeurs` varchar(45) DEFAULT NULL,
  `DiplomaID` int(11) DEFAULT NULL,
  `Starttijd` varchar(5) DEFAULT NULL,
  `Eindtijd` varchar(5) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
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
  `RecordID` int(11) DEFAULT NULL,
  `Nummer` int(11) NOT NULL,
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
  `Telefoon` varchar(21) DEFAULT NULL,
  `Mobiel` varchar(21) DEFAULT NULL,
  `Email` varchar(45) DEFAULT NULL,
  `EmailVereniging` varchar(45) DEFAULT NULL,
  `EmailOuders` varchar(60) DEFAULT NULL,
  `NamenOuders` varchar(90) DEFAULT NULL,
  `Waarschuwen bij nood` varchar(255) DEFAULT NULL,
  `Bankrekening` varchar(18) DEFAULT NULL,
  `Burgerservicenummer` int(11) DEFAULT NULL,
  `Machtiging afgegeven` tinyint(4) DEFAULT NULL,
  `BezwaarMachtiging` datetime DEFAULT NULL,
  `Legitimatietype` varchar(1) DEFAULT NULL,
  `Legitimatienummer` varchar(15) DEFAULT NULL,
  `VOG afgegeven` date DEFAULT NULL,
  `LoginWebsite` varchar(20) DEFAULT NULL,
  `Wijknummer` int(11) DEFAULT NULL,
  `RelnrRedNed` varchar(8) DEFAULT NULL,
  `Beroep` varchar(40) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  `Verwijderd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLiddipl` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Lid` int(11) DEFAULT NULL,
  `DiplomaID` int(11) DEFAULT NULL,
  `DatumBehaald` date DEFAULT NULL,
  `EXPLAATS` varchar(23) DEFAULT NULL,
  `Beoordelaar` int(11) DEFAULT NULL,
  `LaatsteBeoordeling` tinyint(4) DEFAULT NULL,
  `Diplomanummer` varchar(25) DEFAULT NULL,
  `Examen` int(11) DEFAULT NULL,
  `LicentieVervallenPer` date DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `RecordID` (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLidmaatschap` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Lid` int(11) NOT NULL,
  `LIDDATUM` date NOT NULL,
  `Opgezegd` date DEFAULT NULL,
  `OpgezegdDoorVereniging` tinyint(4) DEFAULT NULL,
  `Lidnr` int(11) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Lid`,`LIDDATUM`),
  UNIQUE KEY `RecordID` (`RecordID`),
  UNIQUE KEY `Lidnr` (`Lidnr`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLidond` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Lid` int(11) NOT NULL,
  `OnderdeelID` int(11) NOT NULL,
  `Vanaf` date NOT NULL,
  `Opgezegd` date DEFAULT NULL,
  `Functie` smallint(6) NOT NULL,
  `EmailFunctie` varchar(45) DEFAULT NULL,
  `GroepID` int(11) DEFAULT NULL,
  `Opmerk` varchar(30) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Lid`,`OnderdeelID`,`Vanaf`,`Functie`),
  UNIQUE KEY `RecordID` (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLidRedNed` (
  `Lidnr` int(11) DEFAULT NULL,
  `RelnrRedNed` varchar(8) DEFAULT NULL,
  `Roepnaam` varchar(35) DEFAULT NULL,
  `Tussenvoegsels` varchar(7) DEFAULT NULL,
  `Achternaam` varchar(50) DEFAULT NULL,
  `Voorletters` varchar(10) DEFAULT NULL,
  `Geslacht` varchar(1) DEFAULT NULL,
  `Geboortedatum` date DEFAULT NULL,
  `Postcode` varchar(7) DEFAULT NULL,
  `Huisnr` int(11) DEFAULT NULL,
  `Foutboodschap` varchar(75) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMailing` (
  `MailingID` int(11) NOT NULL AUTO_INCREMENT,
  `from_name` varchar(50) DEFAULT '',
  `from_addr` varchar(50) DEFAULT '',
  `to_name` varchar(50) DEFAULT '' COMMENT 'Omschrijving van de groep personen aan wie deze mailing gericht is',
  `cc_addr` varchar(50) DEFAULT '',
  `subject` varchar(75) DEFAULT '',
  `message` text,
  `template` tinyint(4) NOT NULL DEFAULT '0',
  `confidential` tinyint(4) NOT NULL DEFAULT '0',
  `verzamelen` tinyint(4) NOT NULL DEFAULT '0',
  `new_on` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `AddedBy` int(11) NOT NULL,
  `changed_on` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `ChangedBy` int(11) NOT NULL,
  `sent_on` datetime DEFAULT NULL,
  `SentBy` int(11) NOT NULL,
  `deleted_on` datetime DEFAULT NULL,
  `DeletedBy` int(11) NOT NULL,
  `HTMLdirect` tinyint(4) DEFAULT '0',
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
  `to_addr` varchar(100) NOT NULL,
  `cc_addr` varchar(100) NOT NULL,
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
  `Ingevoerd` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMailing_vanaf` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Vanaf_email` varchar(30) NOT NULL,
  `Vanaf_naam` varchar(50) NOT NULL,
  `SMTP_server` tinyint(20) NOT NULL DEFAULT '0' COMMENT 'Indien 0, dan wordt de smtp server uit het config bestand gebruikt',
  `Ingevoerd` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `VanafEmail` (`Vanaf_email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMemo` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Lid` int(11) NOT NULL,
  `Soort` varchar(1) NOT NULL,
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
  `RecordID` int(11) NOT NULL,
  `Kode` varchar(6) DEFAULT NULL,
  `Naam` varchar(50) DEFAULT NULL,
  `LIDCB` decimal(8,2) DEFAULT NULL,
  `JEUGDCB` decimal(8,2) DEFAULT NULL,
  `FUNCTCB` decimal(8,2) DEFAULT NULL,
  `Type` varchar(1) DEFAULT NULL,
  `ORGANIS` smallint(6) DEFAULT NULL,
  `Kader` tinyint(4) DEFAULT NULL,
  `Alleen leden` tinyint(4) DEFAULT NULL,
  `Tonen in bewakingsadministratie` tinyint(4) DEFAULT NULL,
  `CentraalEmail` varchar(45) DEFAULT NULL,
  `VervallenPer` date DEFAULT NULL,
  `HistorieOpschonen` int(11) DEFAULT NULL,
  `MaximaleLengtePeriode` int(11) DEFAULT NULL,
  `GekoppeldAanQuery` int(11) DEFAULT NULL,
  `Beschrijving` varchar(255) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sOrganisatie` (
  `Nummer` smallint(6) NOT NULL,
  `Naam` varchar(8) DEFAULT NULL,
  `Volledige naam` varchar(50) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sRekening` (
  `Nummer` int(11) NOT NULL DEFAULT '0',
  `Seizoen` smallint(6) DEFAULT NULL,
  `Datum` date DEFAULT NULL,
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
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sRekreg` (
  `RecordID` int(11) DEFAULT NULL,
  `Rekening` int(11) NOT NULL DEFAULT '0',
  `Regelnr` smallint(6) NOT NULL DEFAULT '0',
  `Lid` int(11) DEFAULT NULL,
  `KSTNPLTS` varchar(5) DEFAULT NULL,
  `OMSCHRIJV` varchar(60) DEFAULT NULL,
  `Bedrag` decimal(8,2) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
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

CREATE TABLE IF NOT EXISTS `%1\$sStukken` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Titel` varchar(50) NOT NULL,
  `Type` char(1) NOT NULL DEFAULT 'R',
  `BestemdVoor` varchar(30) NOT NULL,
  `VastgesteldOp` date NOT NULL,
  `Ingangsdatum` date NOT NULL,
  `Revisiedatum` date NOT NULL,
  `Link` varchar(150) NOT NULL,
  `VervallenPer` date DEFAULT NULL,
  `IngevoerdOp` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `GewijzigdOp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sTaak` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Omschrijving` varchar(75) NOT NULL,
  `Beschrijving` text NOT NULL,
  `Verantwoordelijke` int(11) NOT NULL,
  `TaakGroep` int(11) NOT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `GewijzigdDoor` int(11) NOT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sTaakGroep` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Kode` varchar(5) NOT NULL,
  `Naam` varchar(50) NOT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `GewijzigdDoor` int(11) NOT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sVerdichting` (
  `RecordID` int(11) NOT NULL DEFAULT '0',
  `Kode` varchar(4) DEFAULT NULL,
  `Omschrijving` varchar(40) DEFAULT NULL,
  `DebetCredit` varchar(1) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sVereniging` (
  `Naam` varchar(50) DEFAULT NULL,
  `Brigadenaam` varchar(50) DEFAULT NULL,
  `AfkortingNaam` varchar(5) DEFAULT NULL,
  `Bankrekening` varchar(18) DEFAULT NULL,
  `EmailSecretariaat` varchar(45) DEFAULT NULL,
  `EmailLedenadministratie` varchar(45) DEFAULT NULL,
  `BewaartijdHistorie` int(11) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
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
  `Ingevoerd` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
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
  `Ingevoerd` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `GewijzigdDoor` int(11) NOT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `ArtikelLid` (`Artikel`,`Lid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;", TABLE_PREFIX);

	fnQuery($queries);
}  # db_createtables

?>
