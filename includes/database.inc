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
$TypeActiviteit[6] = "Lidgegevens";
$TypeActiviteit[7] = "Evenementen";
$TypeActiviteit[8] = "Interface";
$TypeActiviteit[9] = "Upload data";
$TypeActiviteit[10] = "Webshop";
$TypeActiviteit[11] = "Bewaking";
$TypeActiviteit[12] = "Diploma's per lid";
$TypeActiviteit[13] = "Parameters";
$TypeActiviteit[14] = "Rekeningen";
$TypeActiviteit[15] = "Autorisatie";
$TypeActiviteit[16] = "Toestemmingen per lid";
$TypeActiviteit[17] = "";  // was selecties
$TypeActiviteit[18] = "Taken";
$TypeActiviteit[19] = "Groepsindeling";
$TypeActiviteit[20] = "Stamgegevens";
$TypeActiviteit[97] = "Foutieve login";
$TypeActiviteit[98] = "Performance";
$TypeActiviteit[99] = "Debugging";
asort($TypeActiviteit);

$fdlang = "'%e %M %Y'";
$fdtlang = "'%e %M %Y (%H:%i)'";
$fdkort = "'%e-%c-%Y'";
$fdaccess = "%m/%d/%Y";
$fdsql = "%Y-%m-%d";

$selectnaam = "CONCAT(IF(LENGTH(IFNULL(L.Roepnaam, ''))>1, L.Roepnaam, IFNULL(L.Voorletter, '')), ' ', IF(IFNULL(L.Tussenv, '')>'', CONCAT(L.Tussenv, ' '), ''), L.Achternaam)";
$selectzoeknaam = "TRIM(CONCAT(L.Achternaam, ', ', IF(L.Tussenv>'', CONCAT(L.Tussenv, ' '), ''), IF(LENGTH(L.Roepnaam)>1, L.Roepnaam, L.Voorletter), ' '))";

$selectgeslacht = "CASE L.Geslacht ";
foreach (ARRGESLACHT as $c => $o) {
	$selectgeslacht .= sprintf("WHEN '%s' THEN '%s' ", $c, $o);
}
$selectgeslacht .= "ELSE 'Onbekend' END";

$selecttelefoon = "IF(LENGTH(L.Mobiel) < 10, L.Telefoon, L.Mobiel)";
$selectemail = "IF(LENGTH(L.Email) > 5, L.Email, IF(LENGTH(L.EmailOuders) > 5, L.EmailOuders, L.EmailVereniging))";
$selectleeftijd = "IF(ISNULL(L.GEBDATUM) OR L.GEBDATUM < '1920-01-01' OR (NOT ISNULL(L.Overleden)), NULL, CONCAT(TIMESTAMPDIFF(YEAR, L.GEBDATUM, CURDATE()), ' jaar'))";
$selectlidnr = sprintf("SELECT MAX(IFNULL(Lidnr, 0)) FROM %sLidmaatschap AS LM WHERE LM.Lid=L.RecordID AND LM.LIDDATUM <= CURDATE() AND IFNULL(LM.Opgezegd, CURDATE()) >= CURDATE()", TABLE_PREFIX);
$selectmailbox = "CASE WHEN (NOT M.deleted_on IS NULL) THEN 'Prullenbak'
							  WHEN (M.Template=1) THEN 'Templates'
							  WHEN (M.sent_on IS NULL) THEN 'Concepten'
							  ELSE 'Verzonden' END";  // Moet nog verder ingevoerd worden, indien wenselijk

$fromlid = sprintf('%1$sLid AS L LEFT OUTER JOIN %1$sLidmaatschap AS LM ON L.RecordID=LM.Lid', TABLE_PREFIX);
$wherelid = "LM.LIDDATUM <= CURDATE() AND IFNULL(LM.Opgezegd, CURDATE()) >= CURDATE()";
$fromlidond = sprintf('%1$sLid AS L INNER JOIN ((%1$sLidond AS LO INNER JOIN %1$sFunctie AS F ON LO.Functie=F.Nummer) INNER JOIN %1$sOnderdl AS O ON LO.OnderdeelID=O.RecordID) ON L.RecordID=LO.Lid', TABLE_PREFIX);
$wherelidond = "LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE()";
$whereliddipl = "IFNULL(LD.LicentieVervallenPer, CURDATE()) >= CURDATE()";
$sqllidgroepen = sprintf("SELECT OnderdeelID FROM %sLidond AS LO WHERE %s AND Lid=%d", TABLE_PREFIX, $wherelidond, $_SESSION['lidid']);

// $db_conn_pdo = sprintf("mysql:host=%s;dbname=%s;charset=utf8", $db_host, $db_name);
$db_conn_pdo = sprintf("mysql:host=%s;dbname=%s", $db_host, $db_name);
try {
	$dbc = new PDO($db_conn_pdo, $db_user, $db_pass, array(PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES utf8"));
	$dbc->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_OBJ);
	$dbc->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {
	printf("<p>Verbinding naar database '%s' mislukt: %s</p>\n", $db_name, $e->getMessage());
}

if (db_param("meisjesnaamtonen") == 1) {
	$selectnaam = "CONCAT(IF(LENGTH(L.Roepnaam)>1, L.Roepnaam, L.Voorletter), ' ', IF(L.Tussenv>'', CONCAT(L.Tussenv, ' '), ''), L.Achternaam, IF(L.Meisjesnm>'', CONCAT('-', L.Meisjesnm), ''))";	
}

if ((TableExists("Lid") == false or TableExists("Lidond") == false)) {
	db_createtables();
	db_onderhoud(2);
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
				if ($rv > 0) {
					$mess = sprintf("Uitgevoerd: %s / records affected: %d", $query, $rv);
				} else {
//					$mess = sprintf("Uitgevoerd zonder resultaat: %s", $query);
				}
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

class cls_db_base {
	public $table = "";	// Naam van de tabel
	public $mess = "";	// Boodschap in de logging
	public $ta = 0;		// Type activiteit van de logging
	public $tas = 0;	// Type activiteit specifiek van de logging
	public $tm = 0; 	// Toon boodschap 1 = ja, 0 = nee
	public $lidid = 0;	// RecordID van het lid
	
	public $fromlid = TABLE_PREFIX . "Lid AS L LEFT OUTER JOIN " . TABLE_PREFIX . "Lidmaatschap AS LM ON L.RecordID=LM.Lid";
	public $wherelid = "LM.LIDDATUM <= CURDATE() AND IFNULL(LM.Opgezegd, CURDATE()) >= CURDATE()";
	public $selectnaam = "CONCAT(IF(LENGTH(IFNULL(L.Roepnaam, ''))>1, L.Roepnaam, IFNULL(L.Voorletter, '')), ' ', IF(IFNULL(L.Tussenv, '')>'', CONCAT(L.Tussenv, ' '), ''), L.Achternaam)";
	public $selectzoeknaam = "TRIM(CONCAT(L.Achternaam, ', ', IF(L.Tussenv>'', CONCAT(L.Tussenv, ' '), ''), IF(LENGTH(L.Roepnaam)>1, L.Roepnaam, L.Voorletter), ' '))";
	public $selectlidnr = "SELECT MAX(IFNULL(Lidnr, 0)) FROM " . TABLE_PREFIX . "Lidmaatschap AS LM WHERE LM.Lid=L.RecordID AND LM.LIDDATUM <= CURDATE() AND IFNULL(LM.Opgezegd, CURDATE()) >= CURDATE()";
	
	function __construct($p_table="") {
		$this->table = TABLE_PREFIX . $p_table;
		$this->execsql("SET lc_time_names='nl_NL';");
		if (db_param("meisjesnaamtonen") == 1) {
			$this->selectnaam = "CONCAT(IF(LENGTH(L.Roepnaam)>1, L.Roepnaam, L.Voorletter), ' ', IF(L.Tussenv>'', CONCAT(L.Tussenv, ' '), ''), L.Achternaam, IF(L.Meisjesnm>'', CONCAT('-', L.Meisjesnm), ''))";	
		}
	}
	
	public function bestaat_kolom($p_table, $p_kolom) {
		return bestaatkolom($p_table, $p_kolom);
	}
	
	public function is_kolom_numeriek($p_kolom) {
		if (startwith($this->table, TABLE_PREFIX) == false) {
			$this->table = TABLE_PREFIX . $this->table;
		}

		if (in_array(db_typekolom($this->table, $p_kolom), array("bigint", "int", "tinyint"))) {
			return true;
		} else {
			return false;
		}
	}
	
	public function versiedb() {
		return $this->execsql("SELECT Version() AS Version;")->fetchColumn();
	}
	
	public function setcharset() {
		$this->execsql("SET CHARACTER SET utf8;");
	}
	
	public function execsql($p_query, $p_tm=-1) {
	
		if ($p_tm > -1) {
			$this->tm = $p_tm;
		}
		return fnQuery($p_query, $p_tm);
	}
	
	public function scalar($query) {
		$result = $this->execsql($query);
		return $result->fetchColumn();
	} 
	
	public function aantal($p_filter="") {
		
		if (strlen($p_filter) > 0) {
			$p_filter = "WHERE " . $p_filter;
		}
		$query = sprintf("SELECT COUNT(*) FROM %s %s;", $this->table, $p_filter);
		return $this->scalar($query);
	}
	
	public function basislijst($p_filter="", $p_orderby="") {
		if (strlen($p_orderby) > 0) {
			$p_orderby = "ORDER BY " . $p_orderby;
		}
		
		if (strlen($p_filter) > 0) {
			$p_filter = "WHERE " . $p_filter;
		}
	
		$query = sprintf("SELECT * FROM %s %s %s;", $this->table, $p_filter, $p_orderby);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function Log($p_refID=0) {
		if (strlen($this->mess) > 0) {
			$lbid = db_Logboek("add", $this->mess, $this->ta, $this->lidid, $this->tm, $p_refID, $this->table);
			if ($this->tas > 0 and $this->bestaat_kolom("Admin_activiteit", "TypeActiviteitSpecifiek")) {
				$query = sprintf("UPDATE %sAdmin_activiteit SET `TypeActiviteitSpecifiek`=%d WHERE RecordID=%d;", TABLE_PREFIX, $this->tas, $lbid);
				$this->execsql($query);
			}
			if (strlen($this->table) > 0 and $this->bestaat_kolom("Admin_activiteit", "RefTable")) {
				$query = sprintf("UPDATE %sAdmin_activiteit SET `RefTable`='%s' WHERE RecordID=%d;", TABLE_PREFIX, $this->table, $lbid);
				$this->execsql($query);
			}
			$this->mess = "";
		}
	}
	
	public function Interface($p_query) {
		db_Interface("add", $p_query, $this->lidid);
	}
}

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
								WHERE O.`Type` = 'O' AND LO.Lid = L.RecordID AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd >= CURDATE())", TABLE_PREFIX);
		
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
		$sqlbd = sprintf("SELECT MAX(EINDE_PER) FROM %sBewaking WHERE Lid=L.RecordID AND BEGIN_PER < CURDATE()", TABLE_PREFIX);
		$query = sprintf("SELECT CONCAT(`Waarschuwen bij nood`, NamenOuders) FROM %sLid AS L WHERE L.RecordID=%d;", TABLE_PREFIX, $LidID);
		if (strlen(db_scalar($query)) > 3) {
			$wbn = " 'Waarschuwen bij nood' as Kop7";
			$wbn .= ", `NamenOuders` AS `Namen ouders`";
			$wbn .= ", `Waarschuwen bij nood` AS `Contactgegevens`,";
		} else {
			$wbn = "";
		}
		$sqdieet = sprintf("SELECT `Memo` FROM %sMemo WHERE Lid=%d AND Soort='D'", TABLE_PREFIX, $LidID);
		if (strlen(db_scalar($sqdieet)) > 1) {
			$sqdieet = sprintf(" 'Dieet' as Kop6, (%s) AS memDieet,", $sqdieet);
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
					L.`Waarschuwen bij nood`,
					'Lidmaatschap' AS Kop4,
					LM.Lidnr AS Lidnummer, 
					(%s) AS Onderscheiding, 
					LM.LIDDATUM AS `dteLid vanaf`, 
					LM.Opgezegd AS `dteLid totenmet`, 
					IF(DATEDIFF(CURDATE(), LM.LIDDATUM) < 500, NULL, CONCAT(REPLACE(ROUND(TIMESTAMPDIFF(MONTH, LM.LIDDATUM, IF(ISNULL(LM.Opgezegd), CURDATE(), LM.Opgezegd))/12, 1), '.', ','), ' jaar')) AS `Lengte lidmaatschap`,
					'Overig' AS Kop5,
					L.Bankrekening,
					IF(L.`Machtiging afgegeven`=1, 'Ja', '') AS `Machtiging incasso`,
					%s AS `Legitimatie`,
					L.RelnrRedNed AS `Sportlink ID`,
					L.LoginWebsite AS `Login website`,
					%s
					L.Beroep AS `Beroep`,
					(%s) AS `dteLaatste bewakingsdag`, 
					%s 
					'Recordinformatie' as Kop7,
					L.RecordID, 
					L.Ingevoerd AS `dteIngevoerd op`, 
					L.Gewijzigd AS `dteLaatst gewijzigd op` 
					FROM %s 
					WHERE L.RecordID=%d 
					ORDER BY LM.LIDDATUM DESC;", $selectnaam, $selectgeslacht, $fdlang, $selectleeftijd, $adresgegevens, $sqond, $sqllegitimatie, $VOG, $sqlbd, $sqdieet, $fromlid, $LidID);
					//debug($query);
					
	} elseif ($soort == "Financieel") {
		$query = sprintf("SELECT L.Bankrekening AS Bankrekeningnummer, 
								L.`Machtiging afgegeven` AS `booMachtiging automatische incasso`
								FROM %sLid AS L
								WHERE L.RecordID=%d;", $fdlang, TABLE_PREFIX, $LidID);
								
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
		
		if (db_Onderdeel("maxgeldigheidtoestemmingen") > 0) {
			$sg = ", IF(O.MaximaleLengtePeriode > 0, CONCAT(O.MaximaleLengtePeriode, ' maanden'), 'tot intrekking') AS Geldigheid";
			$sq2 = sprintf(", (SELECT IFNULL(MAX(LO2.Vanaf), '') FROM %sLidond AS LO2 WHERE IFNULL(LO2.Opgezegd, '2999-12-31') < CURDATE() AND LO2.Lid=%d AND LO2.OnderdeelID=O.RecordID) AS `Laatste verlopen toestemming`", TABLE_PREFIX, $LidID);
		} else {
			$sg = "";
			$sq2 = "";
		}
							
		$sq = sprintf("(SELECT IFNULL(MAX(LO.Vanaf), 'Nee') FROM %sLidond AS LO WHERE %s AND LO.Lid=%d AND LO.OnderdeelID=O.RecordID) AS `Afgegeven op`", TABLE_PREFIX, $wherelidond, $LidID);
		$query = sprintf("SELECT O.Naam AS Omschrijving,
					%1\$s
					%2\$s
					%3\$s
					FROM %4\$sOnderdl AS O
					WHERE IFNULL(O.VervallenPer, '2999-12-31') >= CURDATE() AND O.Type='T'", $sq, $sg, $sq2, TABLE_PREFIX);
					
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
								FROM %1\$sBewseiz AS BS INNER JOIN (%1\$sBewaking AS BW INNER JOIN %1\$sFunctie AS F ON BW.Functie=F.Nummer) ON BS.RecordID=BW.SeizoenID
								WHERE Lid=%2\$d %3\$s
								ORDER BY BS.Begindatum DESC, BS.Lokatie, BW.BEGIN_PER DESC;", TABLE_PREFIX, $LidID, $xtra_where);

	} elseif ($soort == "Rekening") {
		$query = sprintf("SELECT
					IF ((SELECT COUNT(*) FROM %1\$sRekreg AS RR WHERE RR.Rekening=RK.Nummer AND RR.ToonOpRekening=1) > 0, RK.Nummer, 0) AS lnkNummer,
					RK.Nummer,
					RK.Seizoen,
					RK.Datum AS dteDatum,
					RK.OMSCHRIJV AS Omschrijving,
					RK.Bedrag AS curBedrag,
					RK.Betaald AS curBetaald,
					(RK.Bedrag-RK.Betaald) AS curOpenstaand
					FROM %1\$sRekening AS RK
					WHERE RK.Lid=%2\$d
					ORDER BY RK.Nummer DESC;", TABLE_PREFIX, $LidID);
	} elseif ($soort == "Mailing") {
		$query = db_mailing_hist("sqllijst", $LidID);
		
	} elseif ($soort == "Evenementen") {
		$query = "SELECT E.Datum AS dteDatum, E.Omschrijving, E.Verzameltijd, DATE_FORMAT(E.Datum, '%H:%i') AS Starttijd, Locatie AS ndLocatie,
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

function db_naamlid($lidid, $uitv="", $riz="gast") {
	global $selectnaam, $selectzoeknaam;
	
	if (strlen($uitv) == 0) {
		$uitv = "Naam";
	}
	
	debug(sprintf("%s is verplaatst naar cls_Lid.", $uitv), 1, 0, 1);
	
	if ($lidid == 0) {
		$query = sprintf("SELECT '%s'", $riz);
	} elseif ($uitv == "Naam") {
		$query = sprintf("SELECT %s FROM %sLid AS L WHERE L.RecordID=%d;", $selectnaam, TABLE_PREFIX, $lidid);
	} elseif ($uitv == "Roepnaam") {
		$query = sprintf("SELECT IF(LENGTH(L.Roepnaam)=0, L.Voorletter, L.Roepnaam) FROM %sLid AS L WHERE L.RecordID=%d;", TABLE_PREFIX, $lidid);
	} elseif ($uitv == "Geslacht") {
		$query = sprintf("SELECT L.Geslacht FROM %sLid AS L WHERE L.RecordID=%d;", TABLE_PREFIX, $lidid);
	} else {
		$query = sprintf("SELECT %s FROM %sLid AS L WHERE L.RecordID=%d;", $selectzoeknaam, TABLE_PREFIX, $lidid);
	}
	$rv = db_scalar($query);
	if (strlen($rv) == 0) {
		$rv = $riz . " " . $lidid;
	}
	return $rv;
}

function db_Lid($actie, $filter="", $lidid=0, $xtraselect="", $kolomnm="", $waarde="") {
	global $selectnaam, $selectzoeknaam, $selectlidnr, $fromlid, $wherelid, $selecttelefoon, $fdaccess;
	
	if (strlen($filter) > 0 and ($actie != "verjaardagen" and $actie != "lididbijemail")) {
		$filter = " WHERE " . $filter;
	} elseif ($lidid > 0) {
		$filter = sprintf(" WHERE L.RecordID=%d", $lidid);
	}
	$waarde = str_replace("\"", "'", $waarde);
	
	if ($actie == "record" and $lidid > 0) {
		$query = sprintf("SELECT *, %s as NaamLid FROM %sLid AS L WHERE L.RecordID=%d;", $selectnaam, TABLE_PREFIX, $lidid);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "aantal") {
		if ($lidid > 0) {
			$query = sprintf("SELECT COUNT(*) FROM %sLid AS L WHERE L.RecordID=%d;", TABLE_PREFIX, $lidid);
		} else {
			$query = sprintf("SELECT COUNT(*) FROM %sLid;", TABLE_PREFIX);
		}
		return db_scalar($query);
		
	} elseif ($actie === "lijst") {
		$query = sprintf("SELECT L.RecordID, %1\$s AS `Naam_lid`, UPPER(L.Woonplaats) AS Woonplaats,
						L.Telefoon, L.Mobiel, IF((L.Email IS NULL), L.EmailOuders, L.Email) AS `emlE-mail`, L.GEBDATUM AS GEBDATUM, %2\$s AS Zoeknaam
						FROM %3\$sLid AS L
						%4\$s
						ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, L.RecordID DESC;", $selectnaam, $selectzoeknaam, TABLE_PREFIX, $filter);
					
	} elseif ($actie == "ledenlijst") {
		if (strpos($filter, "(LM.Lidnr IS NULL)") === false) {
			$ln = ", LM.Lidnr AS numLidnr";
			$k4 = $selecttelefoon . " AS Telefoon";
		} else {
			$ln = "";
			$k4 = "L.Opmerking";
		}
		$query = sprintf("SELECT L.RecordID AS lnkNummer%1\$s, %2\$s AS `Naam_lid`, UPPER(L.Woonplaats) AS Woonplaats,
					%7\$s, IF((LENGTH(L.Email) > 5), L.Email, L.EmailOuders) AS `emlE-mail`%4\$s,
					L.RecordID AS llkNummer, L.GEBDATUM AS ndGEBDATUM, %3\$s AS ndZoeknaam
					FROM %5\$s
					%6\$s
					ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, LM.LIDDATUM DESC;", $ln, $selectnaam, $selectzoeknaam, $xtraselect, $fromlid, $filter, $k4);
					
	} elseif ($actie == "lijstselect") {
		$query = sprintf("SELECT L.RecordID, %s AS NaamLid, %s AS Zoeknaam
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
					L.Opmerking,
					L.Adres,
					L.Postcode,
					L.Woonplaats,
					L.Telefoon,
					L.Mobiel,
					L.Email,
					L.EmailVereniging,
					L.EmailOuders,
					L.NamenOuders,
					L.`Waarschuwen bij nood`,
					L.GEBDATUM,
					L.GEBPLAATS,
					L.Bankrekening,
					L.`Machtiging afgegeven`,
					L.Burgerservicenummer,
					L.RelnrRedNed,
					L.Beroep,
					L.Legitimatietype,
					L.Legitimatienummer,
					L.`VOG afgegeven`,
					IFNULL(L.Verwijderd, '1900-01-01') AS Verwijderd
					FROM %sLid AS L WHERE L.RecordID=%d;", TABLE_PREFIX, $lidid);
		$result = fnQuery($query);
		return $result->fetch();
					
	} elseif ($actie == "verjaardagen") {
		$df = sprintf("MONTH(L.GEBDATUM)=%s AND DAY(L.GEBDATUM)=%s", substr($filter, 5, 2), substr($filter, -2));
		$query = sprintf("SELECT L.RecordID, %1\$s AS `Naam_lid`, L.GEBDATUM, (YEAR('%5\$s')-YEAR(L.GEBDATUM))-IF(RIGHT('%5\$s', 5)<RIGHT(L.GEBDATUM, 5), 1, 0) AS Leeftijd
					FROM %2\$s
					WHERE %3\$s AND %4\$s AND L.GEBDATUM > '1901-01-01'
					ORDER BY L.GEBDATUM, L.RecordID;", $selectnaam, $fromlid, $wherelid, $df, $filter);
										
	} elseif ($actie == "ledenmetrekening") {
		$query = sprintf("SELECT DISTINCT L.RecordID, %1\$s AS `Naam_lid`
					FROM %2\$sLid AS L INNER JOIN %2\$sRekening AS RK ON RK.Lid=L.RecordID
					ORDER BY L.Achternaam, L.Tussenv, L.Roepnaam;", $selectzoeknaam, TABLE_PREFIX);
					
	} elseif ($actie == "add" or $actie == "insert") {
		$lidid = db_nieuwrecordID("Lid");
		$query = sprintf("INSERT INTO %1\$sLid (RecordID, Nummer, Achternaam, Geslacht, Roepnaam, Tussenv, Voorletter) VALUES (%2\$d, %2\$d, \"%3\$s\", 'O', '', '', '');", TABLE_PREFIX, $lidid, $waarde);
		if (fnQuery($query) > 0) {
			db_interface("add", $query, $lidid);
			$mess = sprintf("Kloslid %d (%s) is toegevoegd", $lidid, $waarde);
			db_Logboek("add", $mess, 6, $lidid, 1);
			return $lidid;
		} else {
			return false;
		}
					
	} elseif ($actie == "update") {
		if (db_kolom_numeriek("Lid", $kolomnm)) {
			
		} else {
		}
		
		if ($waarde == "NULL") {
			$query = sprintf("UPDATE %1\$sLid SET `%2\$s`=NULL, Gewijzigd=SYSDATE() WHERE RecordID=%3\$d AND (`%2\$s` NOT IS NULL);", TABLE_PREFIX, $kolomnm, $lidid);
		} elseif (db_kolom_numeriek("Lid", $kolomnm)) {
			$query = sprintf("UPDATE %1\$sLid SET `%2\$s`=%3\$d, Gewijzigd=SYSDATE() WHERE RecordID=%4\$d AND IFNULL(`%2\$s`, 0)<>%3\$d;", TABLE_PREFIX, $kolomnm, $waarde, $lidid);
		} else {
			$query = sprintf("UPDATE %1\$sLid SET `%2\$s`='%3\$s', Gewijzigd=SYSDATE() WHERE BINARY RecordID=%4\$d AND IFNULL(`%2\$s`, '')<>'%3\$s';", TABLE_PREFIX, $kolomnm, $waarde, $lidid);
		}
		if (fnQuery($query) > 0) {
			if (($kolomnm == "GEBDATUM" or $kolomnm == "Ingevoerd" or $kolomnm == "Verwijderd") and $waarde !== "NULL") {
				$query = sprintf("UPDATE Lid SET `%s`=#%s#, Gewijzigd=SYSDATE() WHERE RecordID=%d;", $kolomnm, strftime($fdaccess, strtotime($waarde)), $lidid);
			}
			db_interface("add", $query, $lidid);
			if ($waarde == "NULL") {
				$mess = sprintf("%s is leeg gemaakt.", $kolomnm);
			} else {
				$mess = sprintf("%s is in '%s' gewijzigd.", $kolomnm, $waarde);
			}
			$rv = true;
		} else {
			// debug($query);
			$mess = sprintf("De wijziging van kolom %s is niet gelukt.", $kolomnm);
			$rv = false;
		}
		db_Logboek("add", $mess, 6, $lidid, 1, $lidid);
		return $rv;

	} elseif ($actie == "lididbijemail") {
		$filter = strtolower($filter);
		$query = sprintf("SELECT IFNULL(MAX(L.RecordID), 0) FROM %1\$sLid AS L WHERE (LOWER(Email)='%2\$s' OR LOWER(EmailVereniging)='%2\$s' OR LOWER(EmailOuders)='%2\$s');", TABLE_PREFIX, $filter);
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
		
	} else {
		$mess = sprintf("db_Lid: onbekende actie '%s'.", $actie);
		debug($mess, 1, 0, 1);
		$query = "";
	}
	
	$result = fnQuery($query);
	return $result->fetchAll();
} # db_Lid

class cls_Lid extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Lid";
		$this->ta = 6;
	}
	
	public function Geslacht($p_lidid) {
		$query = sprintf("SELECT IFNULL(MAX(L.Geslacht), '0') FROM %sLid AS L WHERE L.RecordID=%d;", TABLE_PREFIX, $p_lidid);
		return db_scalar($query);
	}
	
	public function Naam($p_lidid, $p_riz="gast") {
		$query = sprintf("SELECT %s FROM %sLid AS L WHERE L.RecordID=%d;", $this->selectnaam, TABLE_PREFIX, $p_lidid);
		$rv = db_scalar($query);
		if (strlen($rv) == 0) {
			$rv = $p_riz;
		}
		return $rv;
	}
	
	public function Roepnaam($p_lidid, $p_riz="gast") {
		$query = sprintf("SELECT IF(LENGTH(IFNULL(L.Roepnaam, ''))=0, IFNULL(L.Voorletter, '%s'), L.Roepnaam) FROM %sLid AS L WHERE L.RecordID=%d;", $p_riz, TABLE_PREFIX, $p_lidid);
		$rv = $this->scalar($query);
		if (strlen($rv) == 0) {
			$rv = $p_riz;
		}
		return $rv;
	}
	
	public function Zoeknaam($p_lidid) {
		$query = sprintf("SELECT %s FROM %sLid AS L WHERE L.RecordID=%d;", $this->selectzoeknaam, TABLE_PREFIX, $p_lidid);
		return $this-scalar($query);
	}
	
	public function mingebdatum() {
		$query = sprintf("SELECT SUBSTRING(MIN(GEBDATUM), 1, 10) FROM %s WHERE %s;", $this->fromlid, $this->wherelid);
		return $this->scalar($query);
	}
	
	public function maxgebdatum() {
		$query = sprintf("SELECT SUBSTRING(MAX(GEBDATUM), 1, 10) FROM %s WHERE %s;", $this->fromlid, $this->wherelid);
		return db_scalar($query);
	}
	
	public function minpostcode() {
		$query = sprintf("SELECT MIN(Postcode) FROM %s WHERE %s;", $this->fromlid, $this->wherelid);
		return $this->scalar($query);
	}
		
	public function maxpostcode() {
		$query = sprintf("SELECT MAX(Postcode) FROM %s WHERE %s;", $this->fromlid, $this->wherelid);
		return $this->scalar($query);
	}
		
	public function add($p_achternaam, $p_geslacht) {
		$this->lidid = db_nieuwrecordID($this->table);
		$query = sprintf("INSERT INTO %sLid (RecordID, Nummer, Achternaam, Geslacht, Ingevoerd) VALUES (%d, %d, \"%s\", '%s', CURDATE());", TABLE_PREFIX, $this->lidid, $this->lidid, $p_achternaam, $p_geslacht);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Kloslid %d (%s) is toegevoegd", $this->lidid, $p_achternaam);
			$this->Log();
			db_interface("add", $query, $this->lidid);
		}
		return $this->lidid;
	}
	
	public function update($p_lidid, $p_kolom, $p_waarde) {
		$this->lidid = $p_lidid;
		
		if ($p_kolom == "Postcode" and $p_waarde != "NULL") {
			$p_waarde = trim(strtoupper($p_waarde));
			if (strlen($p_waarde) < 4) {
				$p_waarde = "";
			} elseif (strlen($p_waarde) == 6) {
				$p_waarde = substr($p_waarde, 0, 4) . " " . substr($p_waarde, -2);
			}
		}
		
		if ($p_kolom == "Bankrekening" and strlen($p_waarde) > 0 and $p_waarde != "NULL" and IsIBANgoed($p_waarde, 1) == false) {
			$this->mess = "De bankrekening is niet correct, deze wijziging wordt niet verwerkt.";
			$this->tm = 1;
				
		} elseif ($p_kolom == "Burgerservicenummer" and strlen($p_waarde) > 0 and $p_waarde != "NULL" and (!is_numeric($p_waarde) or $p_waarde < 100000000 or $p_waarde > 999999999)) {
			$this->mess = "Burgerservicenummer is niet correct, deze wijziging wordt niet verwerkt.";
			$this->tm = 1;
			
		} elseif ($p_kolom == "RelnrRedNed" and strlen($p_waarde) != 7 and $p_waarde != "NULL" and strlen($p_waarde) > 0) {
			$this->mess = "Het Sportlink ID moet 7 karakters lang zijn, deze wijziging wordt niet verwerkt.";
			$this->tm = 1;
			
		} else {
			
			if ($p_kolom == "Adres" or $p_kolom == "Postcode") {
				$this->tas = 1;  // Adreswijziging
			} elseif ($p_kolom == "Telefoon" or $p_kolom == "Mobiel") {
				$this->tas = 2;
			} elseif ($p_kolom == "Email" or $p_kolom == "EmailOuders") {
				$this->tas = 3;
			} else {
				$this->tas = 0;
			}
			
			if ($p_waarde == "NULL") {
				$xw = sprintf("(`%s` IS NOT NULL)", $p_kolom);
			} elseif ($this->is_kolom_numeriek($p_kolom)) {
				$p_waarde = intval($p_waarde);
				$xw = sprintf("IFNULL(`%s`, 0)<>%d", $p_kolom, $p_waarde);
			} else {
				$p_waarde = "'" . $p_waarde . "'";
				$xw = sprintf("IFNULL(`%s`, '')<>%s", $p_kolom, $p_waarde);
			}
		
			$query = sprintf("UPDATE %sLid SET `%s`=%s, Gewijzigd=SYSDATE() WHERE RecordID=%d AND %s;", TABLE_PREFIX, $p_kolom, $p_waarde, $this->lidid, $xw);
			if ($this->execsql($query) > 0) {
				$this->mess = sprintf("Kolom '%s' is in %s gewijzigd.", $p_kolom, $p_waarde);
				
				if (db_typekolom("Lid", $p_kolom) == "date" and $p_waarde !== "NULL") {
					$p_waarde = "#" . substr($p_waarde, 1, -1) . "#";
				}
				$sql = sprintf("UPDATE Lid SET [%s]=%s, Gewijzigd=SYSDATE() WHERE RecordID=%d;", $p_kolom, $p_waarde, $this->lidid);
				db_interface("add", $sql, $this->lidid);
			}
		}
		
		$this->Log();
	}
	
}  # cls_Lid

function db_Lidmaatschap($actie, $lidid=0, $opgezegd="", $lmid=0, $liddatum="", $lidnr=0, $kolomnaam="", $waarde="") {
	global $fdaccess, $fdsql, $wherelid, $fdlang;

	$naamlid = (new cls_Lid())->Naam($lidid);
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
		
	} elseif ($actie == "opzegging" and strlen($opgezegd) > 5) {
		$query = sprintf("UPDATE %sLidmaatschap AS LM SET LM.Opgezegd='%s', LM.Gewijzigd=SYSDATE() WHERE LM.Lid=%d AND (LM.Opgezegd IS NULL);", TABLE_PREFIX, $opgezegd, $lidid);
		$result = fnQuery($query);
		if ($result > 0) {
			$query = sprintf("UPDATE Lidmaatschap SET Opgezegd=%s, Gewijzigd=SYSDATE() WHERE Lid=%d AND (Opgezegd IS NULL);", accessdate($opgezegd), $lidid);
			db_interface("add", $query, $lidid);
			$mess = sprintf("De opzegging per %s is verwerkt. ", $opgezegd);
		} else {
			$mess = sprintf("De opzegging per %s is niet verwerkt. ", $opgezegd);
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
		
	} elseif ($actie == "islid") {
		$query = sprintf("SELECT COUNT(*) FROM %sLidmaatschap AS LM WHERE %s AND Lid=%d;", TABLE_PREFIX, $wherelid, $lidid);
		if (db_scalar($query) > 0) {
			return true;
		} else {
			return false;
		}
		
	} elseif ($actie == "isKloslid") {
		$query = sprintf("SELECT COUNT(*) FROM %sLidmaatschap AS LM WHERE Lid=%d;", TABLE_PREFIX, $lidid);
		if (db_scalar($query) > 0) {
			return false;
		} else {
			return true;
		}
		
	} elseif ($actie == "kanopzeggen") {
		$query = sprintf("SELECT COUNT(*) FROM %sLidmaatschap AS LM WHERE (LM.Opgezegd IS NULL) AND Lid=%d;", TABLE_PREFIX, $lidid);
		if (db_scalar($query) > 0) {
			return true;
		} else {
			return false;
		}
		
	} elseif ($actie == "update") {
		$tm = 0;
				
		$query = sprintf("SELECT COUNT(*) FROM %sLidmaatschap AS LM WHERE LM.RecordID<>%d AND LM.Lidnr=%d;", TABLE_PREFIX, $lmid, $waarde);
		if (strlen($waarde) < 5 and $kolomnaam == "LIDDATUM") {
			$mess = "De datum van lid worden mag niet leeg zijn, wijziging wordt niet verwerkt.";
			$tm = 1;
		} elseif ((strlen($waarde) == 0 or intval($waarde)<= 0) and $kolomnaam == "Lidnr") {
			$mess = "Het lidnummer mag geen 0 zijn, wijziging wordt niet verwerkt.";
			$tm = 1;
		} elseif ($kolomnaam == "Lidnr" and db_scalar($query) > 0) {
			$mess = "Dit lidnummer wordt al ergens anders gebruikt, wijziging wordt niet verwerkt.";
			$tm = 1;
		} else {
				
			if ($kolomnaam == "Opgezegd" and strlen($waarde) < 5) {
				$waarde = "NULL";
				$xw = sprintf("(`%s` IS NOT NULL)", $kolomnaam);
			} elseif ($kolomnaam == "Lidnr") {
				$waarde = intval($waarde);
				$xw = sprintf("ifnull(`%s`, 0)<>%d", $kolomnaam, $waarde);
			} else {
				$waarde = "'" . $waarde . "'";
				$xw = sprintf("IFNULL(`%s`, '')<>%s", $kolomnaam, $waarde);
			}
		
			$query = sprintf("UPDATE %sLidmaatschap AS LM SET `%s`=%s, Gewijzigd=SYSDATE() WHERE LM.RecordID=%d AND %s;", TABLE_PREFIX, $kolomnaam, $waarde, $lmid, $xw);
			$result = fnQuery($query);
			if ($result > 0) {
				$mess = sprintf("De kolom '%s' is in %s gewijzigd.", $kolomnaam, $waarde);
				if ($kolomnaam == "LIDDATUM" or $kolomnaam == "Opgezegd") {
					$waarde = "#" . substr($waarde, 1, -1) . "#";
				}
				$query = sprintf("UPDATE Lidmaatschap SET [%s]=%s, Gewijzigd=SYSDATE() WHERE RecordID=%d;", $kolomnaam, $waarde, $lmid);
				db_interface("add", $query, $lidid);
			} else {
				$mess = "";
			}
		}
		if (strlen($mess) > 0) {
			db_logboek("add", $mess, 6, $lidid, $tm, $lmid);
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
		
	} else {
		$mess = sprintf("db_Lidmaatschap: onbekende actie '%s'.", $actie);
		debug($mess, 1, 0, 1);
	}
	
} # db_Lidmaatschap

class cls_Lidmaatschap extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Lidmaatschap";
		$this->ta = 6;
		$this->tas = 9;
	}
	
	public function lidid($p_lmid, $p_lidnr=0) {
		if ($p_lidnr > 0) {
			$query = sprintf("SELECT LM.Lid FROM %sLidmaatschap AS LM WHERE LM.Lidnr=%d;", TABLE_PREFIX, $p_lidnr);
		} else {
			$query = sprintf("SELECT LM.Lid FROM %sLidmaatschap AS LM WHERE LM.RecordID=%d;", TABLE_PREFIX, $p_lmid);
		}
		$this->lidid = db_scalar($query);
		return $this->lidid;
	}
	
	public function soortlid($p_lidid) {
		$rv = "Lid";
			
		$query = sprintf("SELECT * FROM %s WHERE Lid=%d ORDER BY LIDDATUM DESC;", $this->table, $p_lidid);
		$result = $this->execsql($query);
		$row = $result->fetch();
		if (isset($row->Lid) and $row->Lid > 0) {
			if ($row->Opgezegd > '1970-01-01' and $row->Opgezegd < date("Y-m-d")) {
				$rv = "Voormalig lid";
			} elseif (strtotime($row->LIDDATUM) > time()) {
				$rv = "Toekomstig lid";
			}
		} else {
			$rv = "Kloslid";
		}
		return $rv;
	}
	
	public function kanopzeggen($p_lidid) {
		
		$query = sprintf("SELECT COUNT(*) FROM %s AS LM WHERE (LM.Opgezegd IS NULL) AND LM.Lid=%d;", $this->table, $p_lidid);
		if (db_scalar($query) > 0) {
			return true;
		} else {
			return false;
		}
	}
	
	public function minlidnr() {
		$query = sprintf("SELECT MIN(LM.Lidnr) FROM %s AS LM;", $this->table);
		return db_scalar($query);
	}
	
	public function maxlidnr() {
		$query = sprintf("SELECT MAX(LM.Lidnr) FROM %s AS LM;", $this->table);
		return db_scalar($query);
	}
	
	public function minliddatum() {
		$query = sprintf("SELECT SUBSTRING(MIN(LM.LIDDATUM), 1, 10) FROM %s AS LM;", $this->table);
		return db_scalar($query);
	}
	
	public function maxliddatum() {
		$query = sprintf("SELECT SUBSTRING(MAX(LM.LIDDATUM), 1, 10) FROM %s AS LM;", $this->table);
		return db_scalar($query);
	}
	
	public function minopgezegd() {
		$query = sprintf("SELECT SUBSTRING(MIN(LM.Opgezegd), 1, 10) FROM %s AS LM WHERE (LM.Opgezegd NOT IS NULL);", $this->table);
		return db_scalar($query);
	}
	
	public function maxopgezegd() {
		$query = sprintf("SELECT SUBSTRING(MAX(LM.Opgezegd), 1, 10) FROM %s AS LM WHERE %s;", $this->table, $this->wherelid);
		return db_scalar($query);
	}
			
	public function add($p_lidid) {
		$this->lidid = $p_lidid;
		$lidnr = $this->maxlidnr() + 1;
		$nrid = db_nieuwrecordID($this->table);
		$query = sprintf("INSERT INTO %s (RecordID, Lid, Lidnr, LIDDATUM, Ingevoerd) VALUES (%d, %d, %d, CURDATE(), CURDATE());", $this->table, $nrid, $p_lidid, $lidnr);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Lidmaatschap met lidnummer %d toegevoegd.", $lidnr);
			
			$this->Interface($query);
		}
		$this->Log($nrid);
	}
	
	public function update($p_lmid, $p_kolom, $p_waarde) {
		$this->lidid($p_lmid);
				
		$query = sprintf("SELECT COUNT(*) FROM %sLidmaatschap AS LM WHERE LM.RecordID<>%d AND LM.Lidnr=%d;", TABLE_PREFIX, $p_lmid, $p_waarde);
		if (strlen($p_waarde) < 5 and $p_kolom == "LIDDATUM") {
			$this->mess = "De datum van lid worden mag niet leeg zijn, wijziging wordt niet verwerkt.";
			$this->tm = 1;
		} elseif ((strlen($p_waarde) == 0 or intval($p_waarde)<= 0) and $p_kolom == "Lidnr") {
			$this->mess = "Het lidnummer mag geen 0 zijn, wijziging wordt niet verwerkt.";
			$this->tm = 1;
		} elseif ($p_kolom == "Lidnr" and db_scalar($query) > 0) {
			$this->mess = "Dit lidnummer wordt al ergens anders gebruikt, wijziging wordt niet verwerkt.";
			$this->tm = 1;
		} else {
				
			if ($p_kolom == "Opgezegd" and strlen($p_waarde) < 5) {
				$p_waarde = "NULL";
				$xw = sprintf("(`%s` IS NOT NULL)", $p_kolom);
			} elseif ($p_kolom == "Lidnr") {
				$p_waarde = intval($p_waarde);
				$xw = sprintf("ifnull(`%s`, 0)<>%d", $p_kolom, $p_waarde);
			} else {
				$p_waarde = "'" . $p_waarde . "'";
				$xw = sprintf("IFNULL(`%s`, '')<>%s", $p_kolom, $p_waarde);
			}
		
			$query = sprintf("UPDATE %sLidmaatschap AS LM SET `%s`=%s, Gewijzigd=SYSDATE() WHERE LM.RecordID=%d AND %s;", TABLE_PREFIX, $p_kolom, $p_waarde, $p_lmid, $xw);
			if ($this->execsql($query) > 0) {
				$this->mess = sprintf("In lidmaatschap is de kolom '%s' is in %s gewijzigd.", $p_kolom, $p_waarde);
				
				if ($p_waarde != "NULL" and ($p_kolom == "LIDDATUM" or $p_kolom == "Opgezegd")) {
					$p_waarde = "#" . substr($p_waarde, 1, -1) . "#";
				}
				$query = sprintf("UPDATE Lidmaatschap SET [%s]=%s, Gewijzigd=SYSDATE() WHERE RecordID=%d;", $p_kolom, $p_waarde, $p_lmid);
				$this->Interface($query);
			}
		}
		$this->Log($p_lmid);
	}
	
	public function opzegging($p_lidid, $p_per) {
		
		$query = sprintf("SELECT LM.RecordID FROM %s AS LM WHERE LM.Lid=%d AND (LM.Opgezegd IS NULL);", $this->table, $p_lidid);
		$row = fnQuery($query)->fetch();
		if (isset($row->RecordID) and strlen($p_per) > 5) {
			$this->update($row->RecordID, "Opgezegd", $p_per);
		}
	}
	
}  # cls_Lidmaatschap

function db_memo($actie, $lidid, $srt, $nv="") {
	
	$nv = str_replace("\"", "", str_replace("'", "", $nv));	
	
	if ($actie == "curval" and $lidid > 0) {
		if ($srt == "WN") {
			$query = sprintf("SELECT `Waarschuwen bij nood` FROM %sLid AS L WHERE L.RecordID=%d;", TABLE_PREFIX, $lidid);
		} else {
			$query = sprintf("SELECT `Memo` FROM %sMemo WHERE Lid=%d AND Soort='%s';", TABLE_PREFIX, $lidid, $srt);
		}
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "add" or $actie == "insert") {
		$nrid = db_nieuwrecordID("Memo");
		$query = sprintf("INSERT INTO %sMemo (RecordID, Lid, Soort, Memo, Ingevoerd, Gewijzigd) VALUES (%d, '%s', \"%s\", CURDATE(), SYSDATE());", TABLE_PREFIX, $nrid, $lidid, $srt, $nv);
		$result = fnQuery($query);
		if ($result > 0) {
			$query = sprintf("INSERT INTO Memo (RecordID, Lid, Soort, Memo, Ingevoerd, Gewijzigd) VALUES (%d, '%s', \"%s\", CURDATE(), SYSDATE());", $nrid, $lidid, $srt, $nv);
			db_interface("add", $query, $lidid);
			$mess = sprintf("Memo '%s' met waarde '%s' is toegevoegd.", ARRSOORTMEMO[$srt], $nv);
		} else {
			$mess = sprintf("Het toevoegen van memo '%s' is mislukt.", ARRSOORTMEMO[$srt]);
		}
		db_logboek("add", $mess, 6, $lidid, 0, $nrid);
		
	} elseif ($actie == "update") {
		if ($srt == "WN") {
			$query = sprintf("UPDATE %sLid AS L SET `Waarschuwen bij nood`=\"%s\", Gewijzigd=SYSDATE() WHERE L.RecordID=%d;", TABLE_PREFIX, $nv, $lidid);
		} else {
			$query = sprintf("UPDATE %sMemo SET Memo=\"%s\", Gewijzigd=SYSDATE() WHERE Lid=%d AND Soort='%s';", TABLE_PREFIX, $nv, $lidid, $srt);
		}
		$result = fnQuery($query);
		if ($result > 0) {
			db_interface("add", $query, $lidid);
			$mess = sprintf("Memo '%s' is in '%s' gewijzigd.", ARRSOORTMEMO[$srt], $nv);
			db_logboek("add", $mess, 6, $lidid);
		}
		
	} elseif ($actie == "delete") {
		$query = sprintf("DELETE FROM %sMemo WHERE Lid=%d AND Soort='%s';", TABLE_PREFIX, $lidid, $srt);
		$result = fnQuery($query);
		if ($result > 0) {
			db_interface("add", $query, $lidid);
			$mess = sprintf("Memo '%s' is verwijderd.", ARRSOORTMEMO[$srt]);
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
} # db_memo

function db_authorisation($actie, $recid=0, $tabpage="") {

	$tabpage = str_replace("'", "", $tabpage);
	if ($actie == "delete") {
		if ($_SESSION['webmaster'] == 1) {
			$query = sprintf("SELECT Tabpage FROM %sAdmin_access WHERE RecordID=%d;", TABLE_PREFIX, $recid);
			$tp = db_scalar($query);
			$query = sprintf("DELETE FROM %sAdmin_access WHERE RecordID=%d;", TABLE_PREFIX, $recid);
			if (fnQuery($query) > 0) {
				$mess = sprintf("Uit de tabel '%sAdmin_access' is record %d (%s) verwijderd.", TABLE_PREFIX, $recid, $tp);
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
		
	} elseif ($actie == "update") {  // Nog niet af en nog niet in gebruik
		$query = sprintf("UPDATE %1\$sAdmin_access SET Toegang=%2\$d WHERE RecordID=%3\$d AND Toegang<>%2\$d;", TABLE_PREFIX, $_POST[$vn], $recid);
		$result = fnQuery($query);
		if ($result > 0) {
			if ($_POST[$vn] == "-1") {
				$mess = sprintf("Toegang tot '%s' is alleen voor webmasters beschibaar gemaakt.", $row->Tabpage);
			} else {
				$mess = sprintf("Toegang '%s' is naar groep '%s' aangepast.", $row->Tabpage, db_naam_onderdeel($_POST[$vn], "Iedereen"));
			}
			db_logboek("add", $mess, 15, 0, 0, $recid);
		}
		
	} elseif ($actie == "heefttoegang") {
		if (strlen($tabpage) > 0) {
			$query = sprintf("SELECT COUNT(*) FROM %sAdmin_access WHERE Tabpage=\"%s\" AND Toegang IN (%s);", TABLE_PREFIX, $tabpage, $_SESSION['lidgroepen']);
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

class cls_Authorisation extends cls_db_base {
	
	public $aid = 0;
	public $naamtp = "";
	public $ondnaam = "";
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Admin_access";
		$this->ta = 15;
	}
	
	private function vulvars($p_aid=0) {
		$this->aid = $p_aid;

		if ($this->aid > 0) {	
			$query = sprintf("SELECT Tabpage, Toegang FROM %s WHERE RecordID=%d;", $this->table, $this->aid);
			$row = $this->execsql($query)->fetch();
			$this->naamtp = $row->Tabpage;
			if ($row->Toegang == -1) {
				$this->ondnaam = "Alleen webmasters";
			} elseif ($row->Toegang == 0) {
				$this->ondnaam = "Iedereen";
			} else {
				$this->ondnaam = (new cls_Onderdeel())->Naam($row->Toegang);
			}
		}
	}
	
	public function Naam($p_aid) {
		$this->vulvars($p_aid);
		return $this->naamtp;
	}
	
	public function lijst() {
		$query = sprintf("SELECT RecordID, Toegang, Tabpage, Ingevoerd FROM %s ORDER BY Tabpage;", $this->table);
		$result = fnQuery($query);
		return $result->fetchAll();
	}
	
	public function add($p_tabpage) {
		$p_tabpage = str_replace("'", "", $p_tabpage);
		$nrid = db_nieuwrecordID($this->table);
		$query = sprintf("INSERT INTO %s (RecordID, Tabpage) VALUES (%d, \"%s\");", $this->table, $nrid, $p_tabpage);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Tabblad '%s' is aan de tabel 'Admin_access' toegevoegd.", $p_tabpage);
			$this->Log($nrid);
		}
		$query = sprintf("UPDATE %s SET Toegang=0 WHERE Tabpage='Vereniging/Introductie' AND Toegang<>0;", $this->table);
		$this->execsql($query);
	}
	
	public function update($p_aid, $p_kolom, $p_waarde) {
		$this->vulvars($p_aid);
		
		$p_waarde = intval($p_waarde);
		$xw = sprintf("IFNULL(`%s`, 0)<>%d", $p_kolom, $p_waarde);
		
		$query = sprintf("UPDATE %s SET `%s`=%d, Gewijzigd=SYSDATE() WHERE RecordID=%d AND %s;", $this->table, $p_kolom, $p_waarde, $p_aid, $xw);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Toegang '%s' is naar groep '%s' aangepast.", $this->naamtp, $this->ondnaam);
			$this->Log($p_aid);
		}
	}
	
	public function delete($p_aid) {
		$this->vulvars($p_aid);
		
		$query = sprintf("DELETE FROM %s WHERE RecordID=%d;", $this->table, $p_aid);
		if (fnQuery($query) > 0) {
			$this->mess = sprintf("Uit de tabel '%s' is record %d (%s) verwijderd.", $this->table, $p_aid, $this->naamtp);
			$this->Log($p_aid);
		}
		
	}
	
}  # cls_Authorisation

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
							FROM %1\$sBewaking AS BW INNER JOIN %1\$sLid AS L ON BW.Lid=L.RecordID
							WHERE BW.Weeknr > 0 AND BW.SeizoenID=%2\$d	
							GROUP BY BW.Weeknr
							ORDER BY BW.Weeknr;", TABLE_PREFIX, $seizoen);
			} else {
				$query = sprintf("SELECT BEGIN_PER AS dtlDatum, COUNT(Lid) AS intAantal,
							CONCAT(ROUND(AVG((SELECT IFNULL(SUM(DATEDIFF(BH.EINDE_PER, BH.BEGIN_PER)+1), 0) FROM %1\$sBewaking AS BH WHERE BH.Lid = BW.Lid AND BH.EINDE_PER < BW.BEGIN_PER)), 1), ' dagen') AS `numGem. Ervaring`,
							CONCAT(ROUND(AVG(DATEDIFF(BW.BEGIN_PER, L.GEBDATUM))/365.25, 1), ' jaar') AS `numGem. Leeftijd`
							FROM %1\$sBewaking AS BW INNER JOIN %1\$sLid AS L ON BW.Lid=L.RecordID
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
			$query = sprintf('SELECT BW.*, %1$s AS NaamBewaker, L.RecordID, F.Omschrijv AS OmsFunctie, F.Afkorting AS AfkFunctie, BS.Kode, BS.Lokatie, BS.ST_LEN_BP, BS.TOONERV,
								DATEDIFF(BW.EINDE_PER, BW.BEGIN_PER)+1 AS Dagen, BW.BEGIN_PER, L.GEBDATUM, 
								(SELECT SUM(DATEDIFF(BH.EINDE_PER, BH.BEGIN_PER)+1) FROM %2$sBewaking BH WHERE BH.LID = BW.LID AND BH.EINDE_PER < BW.BEGIN_PER) AS ErvaringDagen
								FROM ((%2$sBewaking AS BW INNER JOIN %2$sBewseiz AS BS ON BS.RecordID = BW.SeizoenID) INNER JOIN %2$sFunctie AS F ON BW.Functie=F.Nummer) INNER JOIN %2$sLid AS L ON L.RecordID=BW.Lid
								%3$s %4$s
								ORDER BY %5$s;', $selectnaam, TABLE_PREFIX, $sel_sz, $sel_wk, $orderby);
									
		} elseif ($actie == "seizoenen") {
			$query = sprintf("SELECT BS.RecordID, BS.Kode, BS.Gewijzigd, BS.ST_LEN_BP FROM %sBewseiz AS BS ORDER BY BS.Begindatum DESC, BS.Kode;", TABLE_PREFIX);
		}
		$result = fnQuery($query);
		return $result;
	}
}  # db_bewaking

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
		$nrid = db_nieuwrecordID("Bewaking_Blok");
		$query = sprintf("INSERT INTO %sBewaking_Blok (RecordID, SeizoenID, Kode, Begindatum, Einddatum) VALUES (%d, %d, '%s', '%s', '%s');", TABLE_PREFIX, $nrid, $seizoen, $k, $b, $e);
		if (fnQuery($query) > 0) {
			$mess = sprintf("Bewakingsblok %d is toegevoegd.", $nrid);
			db_logboek("add", $mess, 11, 0, 1, $nrid);
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
								FROM (%1\$sBewaking_Blok AS BB INNER JOIN (%1\$sBewaking_Inschrijving AS BI INNER JOIN %1\$sLid AS L ON BI.Lid=L.RecordID) ON BB.RecordID = BI.BlokID) INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
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
								FROM (%1\$sBewaking_Blok AS BB INNER JOIN (%1\$sBewaking_Inschrijving AS BI INNER JOIN %1\$sLid AS L ON BI.Lid=L.RecordID) ON BB.RecordID = BI.BlokID) INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
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
								FROM (%1\$sBewaking_Blok AS BB INNER JOIN (%1\$sBewaking_Inschrijving AS BI INNER JOIN %1\$sLid AS L ON BI.Lid=L.RecordID) ON BB.RecordID = BI.BlokID) INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								WHERE BB.Einddatum > CURDATE() AND %3\$s AND BI.Definitief > BI.Ingevoerd
								ORDER BY BS.Kode, BB.Kode, BB.Begindatum;", TABLE_PREFIX, $selectnaam, $w);
		$result = fnQuery($query);
		return $result->fetchAll();	
		
	} elseif ($actie == "laatstelid") {
		$query = sprintf("SELECT IFNULL(MAX(BI.Nummer), 0) FROM %sBewaking_Inschrijving AS BI WHERE BI.Lid=%d;", TABLE_PREFIX, $lidid);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "inschrijving") {
		$query = sprintf("SELECT BI.* FROM %sBewaking_Inschrijving AS BI
								WHERE BI.Lid=%d AND BI.BlokID=%d;", TABLE_PREFIX, $lidid, $blok);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "export") {
		$query = sprintf("SELECT %1\$s AS NaamLid, BI.Lid, BB.SeizoenID, BB.Begindatum, BB.Einddatum
								FROM (%2\$sBewaking_Inschrijving AS BI INNER JOIN %2\$sLid AS L ON BI.Lid=L.RecordID) INNER JOIN %2\$sBewaking_Blok AS BB ON BI.BlokID = BB.RecordID
								WHERE IFNULL(BI.Afgemeld, '0000-00-00') < BI.Ingevoerd AND BI.Definitief > BI.Ingevoerd AND BI.Keuze IN (1, 3)
								ORDER BY L.Achternaam, L.Tussenv, L.Roepnaam, BB.Begindatum;", $selectnaam, TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "add" and $lidid > 0 and $blok > 0 and $insnr > 0) {
		$nrid = db_nieuwrecordID("Bewaking_Inschrijving");
		$query = sprintf("INSERT INTO %sBewaking_Inschrijving (RecordID, BlokID, Lid, Nummer, Keuze, Opmerking, Ingevoerd)
								VALUES (%d, %d, %d, %d, %d, \"%s\", CURDATE());", TABLE_PREFIX, $nrid, $blok, $lidid, $insnr, $keuze, $opmerk);
		fnQuery($query);
		if ($keuze > 0) {
			$mess = sprintf("Inschrijving bewaking voor blok '%s' met keuze %d is toegevoegd.", db_bewaking_blok("omsblok", 0, $blok), $keuze);
			if (strlen($opmerk) > 1) {
				$mess .= sprintf(" Opmerking is '%s'.", $opmerk); 
			}
			db_logboek("add", $mess, 11, $lidid, 0, $nrid);
		}
		
	} elseif ($actie == "update") {
		$query = sprintf("UPDATE %sBewaking_Inschrijving AS BI SET BI.Nummer=%d, BI.Keuze=%d, BI.Opmerking=\"%s\", BI.Definitief=NULL WHERE BlokID=%d AND Lid=%d;", TABLE_PREFIX, $insnr, $keuze, $opmerk, $blok, $lidid);
		if (fnQuery($query) > 0) {
			$mess = sprintf("Inschrijving bewaking voor blok '%s' is bijgewerkt. Keuze is %d.", db_bewaking_blok("omsblok", 0, $blok), $keuze);
			if (strlen($opmerk) > 0) {
				$mess .= sprintf(" Opmerking is '%s'.", $opmerk);
			}
			db_logboek("add", $mess, 11, $lidid, 0, $insnr);
		}

	} elseif ($actie == "delete") {
		$query = sprintf("DELETE FROM %sBewaking_Inschrijving WHERE BlokID=%d AND Lid=%d;", TABLE_PREFIX, $blok, $lidid);
		if (fnQuery($query) > 0) {
			$mess = sprintf("Inschrijving bewaking voor blok '%s' is verwijderd.", db_bewaking_blok("omsblok", 0, $blok));
			db_logboek("add", $mess, 11, $lidid, 0, $blok);
		}
		
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

function db_Onderdeel($actie, $type="", $lidid=0, $onderdlid=0, $kolomnaam="", $waarde="", $per="", $naamfilter="") {
	global $fromlidond, $wherelidond, $fdaccess;
	
	if (strlen($per) >= 10) {
		$wherelidond = sprintf("LO.Vanaf <= '%1\$s' AND IFNULL(LO.Opgezegd, '2999-12-31') >= '%1\$s'", date("Y-m-d", strtotime($per)));
	}
	
	$where = "";
	if ($type == "bw") {
		$where = "O.`Tonen in bewakingsadministratie`=1";
	} elseif ($type == "kad") {
		$where = "O.Kader=1";
	} elseif (strlen($type) == 1) {
		$where = sprintf("O.`Type`='%s'", $type);
	} elseif (strlen($type) > 1) {
		$where = sprintf("O.`Type`%s", $type);
	} elseif ($onderdlid > 0) {
		$where = sprintf("O.RecordID=%d", $onderdlid);
	}
	if (strlen($naamfilter) > 0) {
		if (strlen($where) > 0) {
			$where .= " AND ";
		}
		$where .= sprintf("Naam='%s'", $naamfilter);
	}
	
	
	if ($actie == "editlijst") {
		if (strlen($where) > 0) {
			$where .= " AND ";
		}
//		$where .= "IFNULL(O.VervallenPer, CURDATE()) >= CURDATE() AND O.GekoppeldAanQuery=0";
		$where .= "IFNULL(O.VervallenPer, CURDATE()) >= CURDATE()";
	}
	
	if ($actie == "lijstingebruik" or $actie == "aantalingebruik" or $lidid > 0) {
		if (strlen($where) > 0) {
			$where .= " AND ";
		}
		if ($_SESSION['webmaster'] == 0 and $lidid > 0) {
			$where .= sprintf("O.RecordID IN (SELECT LO.OnderdeelID FROM %sLidond AS LO WHERE %s AND LO.Lid=%d)", TABLE_PREFIX, $wherelidond, $lidid);
		} else {
			$where .= sprintf("O.RecordID IN (SELECT LO.OnderdeelID FROM %sLidond AS LO WHERE %s)", TABLE_PREFIX, $wherelidond);
		}
	}
	
	if ($actie == "aantal" or $actie == "aantalingebruik") {
		$sel = "COUNT(*)";
	} elseif ($actie == "type") {
		$sel = "O.Type";
	} elseif ($actie == "Naam" or $actie == "GekoppeldAanQuery" or $actie == "Alleen leden") {
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
	} elseif ($actie == "editlijst") {
		$sel = sprintf("RecordID AS ndRecordID, Kode, Naam, CentraalEmail AS `emlE-mail`, `Alleen leden` AS `chkAlleen leden`, VervallenPer AS `dteVervallen per`, (SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.OnderdeelID=O.RecordID AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd > CURDATE())) AS `intAantal leden`,
		IF(O.GekoppeldAanQuery > 0, 0, O.RecordID) AS llkRecordID", TABLE_PREFIX);
	} elseif ($actie == "recordid") {
		$sel = "RecordID";
	} else {
		$sel = sprintf("DISTINCT O.RecordID, CONCAT(LEFT(O.Naam, 30), ' (', O.Kode, ')') AS Oms, O.Naam, O.Kode, (SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.OnderdeelID=O.RecordID AND %s) AS Aantal", TABLE_PREFIX, $wherelidond);
	}
	
	if (strlen($where) > 0) {
		$where = "WHERE " . $where;
	}
	
	if ($actie == "add") {
		debug("Deze actie is naar cls_Onderdeel verplaatst.", 1, 0, 99);
		
	} elseif ($actie == "update") {
		debug("Deze actie is naar cls_Onderdeel verplaatst.", 1, 0, 99);
		
	} elseif ($actie == "maxgeldigheidtoestemmingen") {
		$query = sprintf("SELECT IFNULL(MAX(O.MaximaleLengtePeriode), 0) FROM %sOnderdl AS O WHERE O.Type='T' AND IFNULL(VervallenPer, CURDATE()) >= CURDATE();", TABLE_PREFIX);
	} elseif ($actie == "lijstopkode") {
		$query = sprintf("SELECT %s FROM %sOnderdl AS O %s ORDER BY O.Kode;", $sel, TABLE_PREFIX, $where);
	} elseif ($actie == "lijstingebruik") {
		$query = sprintf("SELECT %s FROM %sOnderdl AS O %s ORDER BY O.Naam;", $sel, TABLE_PREFIX, $where);
	} else {
		$query = sprintf("SELECT %s FROM %sOnderdl AS O %s ORDER BY O.Naam, O.Kode;", $sel, TABLE_PREFIX, $where);
	}
	
	if ($actie == "aantal" or $actie == "aantalingebruik" or $actie == "type" or $actie == "maxgeldigheidtoestemmingen" or $actie == "recordid") {
		return db_scalar($query);
	} else {
		$result = fnQuery($query);
		return $result->fetchAll();
	}
} # db_onderdeel 

class cls_Onderdeel extends cls_db_base {
	
	private $ondid = 0;
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Onderdl";
		$this->ta = 20;
	}
	
	public function Naam($p_oid, $p_riz="") {
		$query = sprintf("SELECT IFNULL(MAX(O.Naam), '%s') FROM %s AS O WHERE O.RecordID=%d;", $p_riz, $this->table, $p_oid);
		return db_Scalar($query);
	}
	
	public function add($p_type, $p_code) {
		$query = sprintf("SELECT COUNT(*) FROM %s WHERE Kode='%s';", $this->table, $p_code);
		if (db_scalar($query) > 0) {
			$mess = sprintf("De code '%s' is al ingebruik. Deze %s wordt niet toegevoegd.", $p_code, strtolower(ARRTYPEONDERDEEL[$p_type]));
			db_logboek("add", $mess, 20, 0, 1);
		} else {
			$nrid = db_nieuwrecordID("Onderdl");
			$query = sprintf("INSERT INTO %s (RecordID, Kode, Type, Naam, `Alleen leden`, GekoppeldAanQuery) VALUES (%d, '%s', '%s', 'Nieuw', 1, 0);", $this->table, $nrid, $p_code, $p_type);
			if ($this->execsql($query) > 0) {
				$mess = sprintf("%s %d met code '%s' is toegevoegd.", ARRTYPEONDERDEEL[$p_type], $nrid, $p_code);
				db_logboek("add", $mess, 20, 0, 0, $nrid);
			
				db_interface("add", $query, 0);
			}
		}
		
		$this->Log($nrid);
	}
	
	public function update($p_ondid, $p_kolom, $p_waarde) {
		$this->ondid = $p_ondid;
		
		if ($this->is_kolom_numeriek($p_kolom)) {
			$xw = sprintf("IFNULL(`%s`, 0)<>%d;", $p_kolom, $p_waarde);
		} elseif ($p_kolom == "VervallenPer" and strlen($p_waarde) < 5) {
			$p_waarde = "NULL";
			$xw = sprintf("(`%s` IS NOT NULL)", $p_kolom);
		} else {
			$p_waarde = "'" . $p_waarde . "'";
			$xw = sprintf("IFNULL(`%s`, '')<>%s;", $p_kolom, $p_waarde);
		}
		
		$query = sprintf("UPDATE %s SET `%s`=%s, Gewijzigd=SYSDATE() WHERE RecordID=%d AND %s;", $this->table, $p_kolom, $p_waarde, $p_ondid, $xw);
		if (fnQuery($query) > 0) {
			$mess = sprintf("Bij onderdeel %d is kolom '%s' in %s gewijzigd.", $p_ondid, $p_kolom, $p_waarde);
			db_Logboek("add", $mess, 20, 0, 0, $p_ondid);
			
			if ($p_kolom == "VervallenPer" and strlen($p_waarde) > 5) {
				$p_waarde = "#" . substr($p_waarde, 1, -1) . "#";
			}
			$sql = sprintf("UPDATE Onderdl SET [%s]=%s, Gewijzigd=Now() WHERE RecordID=%d;", $p_kolom, $p_waarde, $p_ondid);
			db_interface("add", $sql, 0);
		}
		
		$this->Log($this->ondid);
	}
	
}  # cls_Onderdeel

function db_Functie($actie) {
	
	if ($actie == "basislijst") {
		$query = sprintf("SELECT * FROM %sFunctie;", TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchAll();
	}
	
} # db_functie

class cls_Functie extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Functie";
	}
	
	public function selectlijst($p_soort) {
		
		if ($p_soort == "A") {
			$f = "F.Afdelingsfunctie=1";
		} elseif ($p_soort == "L") {
			$f = "F.Ledenadministratiefunctie=1";
		} else {
			$f = "";
		}
		
		$query = sprintf("SELECT F.Nummer, F.Omschrijv FROM %s AS F WHERE %s ORDER BY F.Omschrijv;", $this->table, $f);
		return $this->execsql($query)->fetchAll();
	}
	
}  # cls_Functie

function db_Lidond($actie, $lidid=0, $per="", $filter="", $onderdlid=0, $loid=0, $kolnaam="", $waarde="") {
	global $selectnaam, $selectzoeknaam, $selectgeslacht, $selecttelefoon, $selectemail, $selectleeftijd, $fromlidond, $wherelidond, $selectlidnr, $fdaccess, $lididtestusers;
	
	$mess = "";
//	debug($actie);
	
	if (strlen($per) >= 10) {
		$wherelidond = sprintf("LO.Vanaf <= '%1\$s' AND IFNULL(LO.Opgezegd, '2999-12-31') >= '%1\$s'", date("Y-m-d", strtotime($per)));
	}
	
	if ($lidid > 0) {
		if (strlen($filter) > 0) {
			$filter .= sprintf(" AND LO.Lid=%d", $lidid);
		} else {
			$filter = sprintf("LO.Lid=%d", $lidid);
		}
	}
	
	if ($onderdlid > 0) {
		if (strlen($filter) > 0) {
			$filter .= sprintf(" AND LO.OnderdeelID=%d", $onderdlid);
		} else {
			$filter = sprintf("LO.OnderdeelID=%d", $onderdlid);
		}
	}

	if ($loid > 0) {
		$query = sprintf("SELECT LO.Lid, LO.OnderdeelID FROM %sLidond AS LO WHERE LO.RecordID=%d;", TABLE_PREFIX, $loid);
		$row = fnQuery($query)->fetch();
		$lidid = $row->Lid;
		$onderdlid = $row->OnderdeelID;
	}

	if ($actie == "aantal") {
		$query = sprintf("SELECT COUNT(*) FROM %sLidond;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "LidID") {
		$query = sprintf("SELECT IFNULL(LO.Lid, 0) FROM %sLidond AS LO WHERE LO.RecordID=%d;", TABLE_PREFIX, $loid);
		return db_scalar($query);
		
	} elseif ($actie == "OnderdeelD") {
		$query = sprintf("SELECT IFNULL(LO.OnderdeelD, 0) FROM %sLidond AS LO WHERE LO.RecordID=%d;", TABLE_PREFIX, $loid);
		return db_scalar($query);
		
	} elseif ($actie == "laatstgewijzigd") {
		$query = sprintf("SELECT Max(LO.Gewijzigd) FROM %sLidond AS LO;", TABLE_PREFIX);
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
		
		$query = sprintf("SELECT L.RecordID AS lnkNummer, IFNULL((%5\$s), 0) AS numLidnr, %1\$s AS `Naam_lid`, UPPER(L.Woonplaats) AS Woonplaats,
								L.Mobiel, IF(LENGTH(L.Email) > 5, L.Email, L.EmailOuders) AS `emlE-mail`%6\$s, L.RecordID AS llkNummer, L.GEBDATUM AS ndGEBDATUM, %2\$s AS ndZoeknaam
								FROM %3\$s
								%4\$s
								ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", $selectnaam, $selectzoeknaam, $fromlidond, $filter, $selectlidnr, $xtr_sel);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "selectielijst") {
		if (strlen($filter) > 0) {
			$filter = " WHERE " . $filter;
		}
		
		$query = sprintf("SELECT LO.RecordID AS lnkNummer, %1\$s AS `Naam_lid`, %2\$s AS Telefoon, %3\$s AS `emlE-mail`, LO.Vanaf AS dteVanaf, LO.Opmerk AS Opmerking, LO.Opgezegd AS `dteTot en met`, LO.RecordID AS llkNummer, L.GEBDATUM AS ndGEBDATUM
								FROM %4\$s
								%5\$s
								ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", $selectnaam, $selecttelefoon, $selectemail, $fromlidond, $filter);
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
								L.RecordID, %s AS NaamLid, UPPER(L.Woonplaats) AS Woonplaats, L.Mobiel, L.Email, L.EmailVereniging, L.GEBDATUM, %s AS ZoeknaamLid,
								F.Omschrijv AS Functie,
								O.Kode, O.Naam AS OndNaam, O.CentraalEmail, O.TYPE
								FROM %s
								WHERE %s
								ORDER BY O.Kader DESC, O.TYPE, O.Naam, F.Sorteringsvolgorde, F.Omschrijv, LO.Opmerk, LO.Vanaf, L.Achternaam, L.TUSSENV, L.Roepnaam;", $selectnaam, $selectzoeknaam, $fromlidond, $filter);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "selectieleden") {
		$query = sprintf("SELECT SQL_NO_CACHE L.RecordID, %s AS NaamLid FROM %s WHERE %s AND LO.OnderdeelID=%d ORDER BY L.Achternaam, L.Tussenv, L.Roepnaam, L.GEBDATUM;", $selectnaam, $fromlidond, $wherelidond, $onderdlid);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "editlijst") {
		$filter .= " AND O.GekoppeldAanQuery=0";
		$query = sprintf("SELECT LO.RecordID, O.Kode, O.Naam, LO.OPMERK, LO.Vanaf, LO.Opgezegd, LO.OnderdeelID, LO.Functie FROM %s WHERE %s ORDER BY LO.Opgezegd;", $fromlidond, $filter);
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
	
	} elseif ($actie == "add" or $actie == "insert") {
		if (db_onderdeel("aantal", "", 0, $onderdlid) == 0) {
			$mess = sprintf("OnderdeelID %d bestaat niet. Dit record wordt niet toegevoegd.", $onderdlid);
			db_Logboek("add", $mess, 99, $lidid, 1, $onderdlid);
		
		} elseif (is_array($lidid) and count($lidid) > 0) {
			
			
			
		} elseif (db_lid("aantal", "", $lidid) == 0) {
			$mess = sprintf("Lid %d bestaat niet. Dit record wordt niet toegevoegd.", $lidid);
			db_Logboek("add", $mess, 99, $lidid, 1);
			
		} else {
			$nrid = db_nieuwrecordID("Lidond");
			$query = sprintf("INSERT INTO %sLidond (RecordID, Lid, OnderdeelID, Functie, Vanaf, Ingevoerd, Gewijzigd) VALUES (%d, %d, %d, 0, CURDATE(), CURDATE(), SYSDATE());", TABLE_PREFIX, $nrid, $lidid, $onderdlid);
			$result = fnQuery($query);
			if ($result > 0 and in_array($lidid, $lididtestusers) == false) {
				$mess = sprintf("Is aan '%s' toegevoegd.", db_Onderdeel("Naam", "", 0, $onderdlid));
				db_Logboek("add", $mess, 6, $lidid, 0, $nrid);
				db_interface("add", $query, $lidid);
				return true;
			} else {
				return false;
			}
		}
		
	} elseif ($actie == "update") {
		$ta = 6;
		$sql = "";
		if ($kolnaam == "Opgezegd" and strlen($waarde) < 4) {
			$query = sprintf("UPDATE %1\$sLidond SET %2\$s=NULL, Gewijzigd=SYSDATE() WHERE RecordID=%3\$d AND `%2\$s`<>NULL;", TABLE_PREFIX, $kolnaam, $loid);
		} elseif ($kolnaam == "Vanaf" and strlen($waarde) < 4) {
			$mess = "De datum 'Vanaf' mag niet leeg zijn, deze aanpassing wordt niet verwerkt.";
			db_Logboek("add", $mess, $ta, $lidid, 1);
		} elseif ($kolnaam == "GroepID") {
			$ta = 19;
			$query = sprintf("UPDATE %1\$sLidond SET %2\$s=%3\$d, Gewijzigd=Now() WHERE RecordID=%4\$d AND %2\$s<>%3\$d;", TABLE_PREFIX, $kolnaam, $waarde, $loid);
		} else {
			$query = sprintf("UPDATE %1\$sLidond SET %2\$s='%3\$s', Gewijzigd=SYSDATE() WHERE RecordID=%4\$d AND %2\$s<>'%3\$s';", TABLE_PREFIX, $kolnaam, $waarde, $loid);
		}
		if (strlen($query) > 5 and fnQuery($query) > 0) {
			if (($kolnaam == "Vanaf" or $kolnaam == "Opgezegd") and strlen($waarde) > 4) {
				$sql = sprintf("UPDATE Lidond SET %s=#%s#, Gewijzigd=Now() WHERE RecordID=%d;", $kolnaam, strftime($fdaccess, strtotime($waarde)), $loid);
				db_interface("add", $sql, $lidid);
			} else {
				db_interface("add", $query, $lidid);
			}
			
			$mess = sprintf("Bij '%s' is '%s' in '%s' gewijzigd.", db_Onderdeel("Naam", "", 0, $onderdlid), $kolnaam, $waarde);
			db_Logboek("add", $mess, $ta, $lidid, 0, $loid);
		}
		
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
			$mess = sprintf("Is bij '%s' verwijderd.", db_Onderdeel("Naam", "", 0, $onderdlid));
			db_logboek("add", $mess, 6, $lidid, 0, $loid);
			db_interface("add", $query, $lidid);
		}
		return $result;
		
	} elseif ($actie == "opschonen") {
		$query = sprintf("DELETE FROM %sLidond WHERE Lid=%d AND Vanaf >= Opgezegd;", TABLE_PREFIX, $lidid);
		fnQuery($query);

	} elseif ($actie == "lidgroepen") {
		if ($lidid < 1) {
			$lidid = $_SESSION['lidid'];
		}
		$query = sprintf("SELECT OnderdeelID FROM %sLidond AS LO WHERE Lid=%d AND %s;", TABLE_PREFIX, $lidid, $wherelidond);
		$result = fnQuery($query);
		$rows = $result->fetchAll(PDO::FETCH_COLUMN);
		if ($_SESSION['webmaster'] == 1) {
			$b = "-1, 0";
		} else {
			$b = "0";
		}
		if (count($rows) > 0) {
			return $b . ", " . implode(", ", $rows);
		} else {
			return $b;
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
					WHEN LO.GroepID=0 AND LO.Functie=0 THEN 'Niet ingedeeld'
					WHEN LENGTH(Instructeurs) > 1 THEN CONCAT(GR.Omschrijving, ' - ', GR.Instructeurs)
					ELSE GR.Omschrijving
					END AS GroepOms,
					LO.RecordID, LO.GroepID, LO.Lid, %2\$s AS Leeftijd
					FROM ((%3\$sLidond AS LO INNER JOIN %3\$sLid AS L ON LO.Lid=L.RecordID) INNER JOIN %3\$sOnderdl AS O ON LO.OnderdeelID=O.RecordID) LEFT JOIN %3\$sGroep AS GR ON LO.GroepID=GR.RecordID
					WHERE ((LO.Opgezegd IS NULL) OR LO.Opgezegd >= CURDATE()) AND (LO.Functie=0 OR GroepID > 0) %4\$s
					ORDER BY O.Kode, GR.Starttijd, GR.Kode, L.Achternaam, L.Roepnaam;", $selectnaam, $selectleeftijd, TABLE_PREFIX, $where);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} else {
		$mess = sprintf("db_lidond: Actie '%s' is onbekend", $actie);
		db_logboek("add", $mess, 99);
	}
	
} # db_lidond

class cls_Lidond extends cls_db_base {
	
	private $loid = 0;  // RecordID van het record in Lidond
	private $ondid = 0; // RecordID van het onderdeel
	private $ondnaam = ""; // Naam van het onderdeel
	
	function __construct() {
		$this->ta = 6;
	}
	
	private function vulvars() {
	
		if ($this->loid > 0) {
			$query = sprintf("SELECT LO.Lid, LO.OnderdeelID, O.Naam FROM %1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON O.RecordID=LO.OnderdeelID WHERE LO.RecordID=%2\$d;", TABLE_PREFIX, $this->loid);
			$row = fnQuery($query)->fetch();
			if (isset($row)) {
				$this->lidid = $row->Lid;
				$this->ondid = $row->OnderdeelID;
				$this->ondnaam = $row->Naam;
			} else {
				$this->mess = sprintf("Record in lidond met ID %d bestaat niet.", $this->loid);
				$this->ta = 99;
				$this->tm = 1;
			}
		} elseif ($this->ondid > 0) {
			$query = sprintf("SELECT IFNULL(MAX(O.Naam), '') AS Naam FROM %sOnderdl AS O WHERE O.RecordID=%d;", TABLE_PREFIX, $this->ondid);
			$this->ondnaam = db_scalar($query);
		}
	}
	
	public function add($p_ondid, $p_lidid) {
		global $lididtestusers;
		
		$this->ondid = $p_ondid;
		$this->lidid = $p_lidid;
		$this->vulvars();
		$nrid = 0;
		
		$query = sprintf("SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.Lid=%d AND LO.OnderdeelID=%d AND LO.Vanaf=CURDATE();", TABLE_PREFIX, $this->lidid, $this->ondid);
		if (strlen($this->ondnaam) == 0) {
			$this->mess = sprintf("OnderdeelID %d bestaat niet. Record wordt niet toegevoegd.", $this->ondid);
			$this->ta = 99;
			$this->tm = 1;
		} elseif (db_scalar($query) > 0) {	
			$this->mess = "Deze combinatie bestaat al en wordt niet dubbel ingevoerd.";
			$this->tm = 1;
		} elseif (db_lid("aantal", "", $this->lidid) == 0) {
			$mess = sprintf("Lid %d bestaat niet. Dit record wordt niet toegevoegd.", $this->lidid);
			$this->ta = 99;
			$this->tm = 1;
			
		} else {
			$nrid = db_nieuwrecordID("Lidond");
			$query = sprintf("INSERT INTO %sLidond (RecordID, Lid, OnderdeelID, Functie, Vanaf, Ingevoerd, Gewijzigd) VALUES (%d, %d, %d, 0, CURDATE(), CURDATE(), SYSDATE());", TABLE_PREFIX, $nrid, $this->lidid, $this->ondid);
			if (fnQuery($query) > 0) {
				$this->mess = sprintf("Is aan '%s' toegevoegd.", $this->ondnaam);
				$this->loid = $nrid;
				if (in_array($this->lidid, $lididtestusers) == false) {
					db_interface("add", $query, $this->lidid);
				}
			}
		}
		
		$this->log($nrid);
	}
	
	public function update($p_loid, $p_kolom, $p_waarde) {
		
		$this->loid = $p_loid;
		$this->vulvars();
		
		if ($p_kolom == "Vanaf" and strlen($p_waarde) < 5) {
			$this->mess = "De datum 'Vanaf' mag niet leeg zijn, deze aanpassing wordt niet verwerkt.";
			$this->tm = 1;
		} else {
			if ($p_kolom == "Opgezegd" and strlen($p_waarde) < 5) {
				$xw = sprintf("AND (%s IS NOT NULL)", $p_kolom);
				$p_waarde = "NULL";
			} elseif ($p_kolom == "GroepID") {
				$this->ta = 19;
				$p_waarde = intval($p_waarde);
				$xw = sprintf("AND `%s`<>%d", $p_kolom, $p_waarde);
			} else {
				$p_waarde = "'" . $p_waarde . "'";
				$xw = sprintf("AND IFNULL(`%s`, '')<>%s", $p_kolom, $p_waarde);
			}
			$query = sprintf("UPDATE %sLidond SET %s=%s, Gewijzigd=SYSDATE() WHERE RecordID=%d %s;", TABLE_PREFIX, $p_kolom, $p_waarde, $this->loid, $xw);
			if ($this->execsql($query) > 0) {
				$this->mess = sprintf("Bij %s is kolom '%s' in %s gewijzigd.", $this->ondnaam, $p_kolom, $p_waarde);
				
				if (($p_kolom == "Vanaf" or $p_kolom == "Opgezegd") and strlen($p_waarde) > 5) {
					$p_waarde = "#" . substr($p_waarde, 1, -1) . "#";
				}
				$sql = sprintf("UPDATE Lidond SET [%s]=%s, Gewijzigd=Now() WHERE RecordID=%d;", $p_kolom, $p_waarde, $this->loid);
				db_interface("add", $sql, $this->lidid);
			}
		}
		$this->Log();
	}
	
	public function delete($p_loid, $p_lidid=0, $p_ondid=0) {
		
		if ($p_loid > 0) {
			$this->loid = $p_loid;
			$query = sprintf("DELETE FROM %sLidond WHERE RecordID=%d;", TABLE_PREFIX, $this->loid);
		} else {
			$this->lidid = $p_lidid;
			$this->ondid = $p_ondid;
			$query = sprintf("DELETE FROM %sLidond WHERE Lid=%d AND OnderdeelID=%d;", TABLE_PREFIX, $this->lidid, $this->ondid);
		}
		$this->vulvars();
		if (fnQuery($query) > 0) {
			$this->mess = sprintf("Is bij '%s' verwijderd.", $this->ondnaam);
			
			db_interface("add", $query, $this->lidid);
		}
		$this->Log();
	}
	
}  # cls_Lidond

function db_Groep($actie, $onderdeelid=0, $recid=0, $colname="", $value="") {
	
	if ($onderdeelid > 0) {
		$where = sprintf("WHERE OnderdeelID=%d", $onderdeelid);
	} else {
		$where = "";
	}
	
	if ($actie == "aantal") {
		$query = sprintf("SELECT COUNT(*) FROM %sGroep %s;", TABLE_PREFIX, $where);
		return db_scalar($query);
	} elseif ($actie == "selectlijst") {
		$query = sprintf("SELECT *,
						  CASE 
							WHEN GR.RecordID=0 THEN 'Niet ingedeeld'
							WHEN LENGTH(Instructeurs) > 1 THEN CONCAT(GR.Kode, ' - ', GR.Omschrijving, ' - ', GR.Instructeurs)
							ELSE CONCAT(GR.Kode, ' - ', GR.Omschrijving)
						  END AS GroepOms
						  FROM %sGroep AS GR %s ORDER BY GR.Starttijd, GR.Kode;", TABLE_PREFIX, $where);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "add" or $actie == "insert") {
		$nrid = db_nieuwrecordID("Groep");
		$query = sprintf("INSERT INTO %sGroep (RecordID, OnderdeelID, Kode, DiplomaID) VALUES (%d, %d, '%s', 0);", TABLE_PREFIX, $nrid, $onderdeelid, $value);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("In tabel 'Groep' is record met RecordID %d en kode '%s' toegevoegd.", $nrid, $value);
			db_Logboek("add", $mess, 20, 0, 0, $nrid);
			$sql = sprintf("INSERT INTO Groep (RecordID, OnderdeelID, Kode, DiplomaID) VALUES (%d, %d, '%s', 0);", $nrid, $onderdeelid, $value);
			db_Interface("add", $sql);
		}
		
	} elseif ($actie == "update") {
		$query = sprintf("UPDATE %sGroep SET `%s`='%s' WHERE RecordID=%d;", TABLE_PREFIX, $colname, $value, $recid);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("In RecordID %d in de tabel 'Groep' is kolom '%s' in '%s' gewijzigd.", $recid, $colname, $value);
			db_Logboek("add", $mess, 20);
			$sql = sprintf("UPDATE Groep SET %s='%s' WHERE RecordID=%d;", $colname, $value, $recid);
			db_Interface("add", $sql);
		}
		
	}
} # db_Groep

class cls_Groep extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Groep";
		$this->ta = 19;
	}
	
	public function add($p_onderdeelid, $p_waarde) {
		$nrid = db_nieuwrecordID("Groep");
		$query = sprintf("INSERT INTO %sGroep (RecordID, OnderdeelID, Kode, DiplomaID, Ingevoerd) VALUES (%d, %d, '%s', 0, CURDATE());", TABLE_PREFIX, $nrid, $p_onderdeelid, $p_waarde);
		if (fnQuery($query) > 0) {
			$this->mess = sprintf("Groep %d met kode '%s' is toegevoegd.", $nrid, $p_waarde);
			
			$this->Interface($query);
		}
	}
	
	public function update($p_grid, $p_kolom, $p_waarde) {
		if ($this->is_kolom_numeriek($p_kolom)) {
			$p_waarde = intval($p_waarde);
			$xw = sprintf("IFNULL(`%s`, 0)<>%d", $p_kolom, $p_waarde);
		} else {
			$p_waarde = "'" . $p_waarde . "'";
			$xw = sprintf("IFNULL(`%s`, '')<>%s", $p_kolom, $p_waarde);
		}
		$query = sprintf("UPDATE %s SET `%s`=%s, Gewijzigd=SYSDATE() WHERE RecordID=%d AND %s;", $this->table, $p_kolom, $p_waarde, $p_grid, $xw);
		if (fnQuery($query) > 0) {
			$this->mess = sprintf("Kolom '%s' van groep %d in %s gewijzigd.", $p_kolom, $p_grid, $p_waarde);

			$sql = sprintf("UPDATE Groep SET [%s]=%s, Gewijzigd=Now() WHERE RecordID=%d;", $p_kolom, $p_waarde, $p_grid);
			$this->Interface($sql);
		}
		$this->Log($p_grid);
	}
	
}  # cls_Groep

function db_logins($actie, $email="", $login="", $lidid=0, $xtrafilter="", $pw="", $xtraorder="") {
	global $dbc, $selectnaam, $selectlidnr, $lididwebmasters, $wherelidond, $wherelid, $dir, $pwfromcookie;
	
	if (strlen(db_param("login_beperkttotgroep")) > 0) {
		$beperkttotgroep = explode(",", db_param("login_beperkttotgroep"));
	} else {
		$beperkttotgroep = array(0);
	}
	
	$mess = "";
	if ($actie == "nieuwlogin" and isValidMailAddress($email) and strlen($login) > 5) {
		$query = sprintf("SELECT L.RecordID, Login.Login FROM %1\$sLid AS L LEFT OUTER JOIN %1\$sAdmin_login AS Login ON L.RecordID = Login.LidID
								WHERE (LOWER(L.Email) LIKE '%2\$s' OR LOWER(L.EmailVereniging) LIKE '%2\$s')", TABLE_PREFIX, strtolower($email));
		$result = fnQuery($query);
		$row = $result->fetch();
		if (isset($row->RecordID) and $row->RecordID > 0) {
			$lidid = $row->RecordID;
			if (strlen($row->Login) > 5 and $row->Login != $login) {
				$mess = sprintf("Er is al een ander login aan dit emailadres gekoppeld.");
			} elseif (strlen($login) > 5 and strlen($login) <= db_param("login_maxlengte")) {
				$mess = db_logins("add", "", $login, $row->RecordID, "", $pw);
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
			$mess = "Heeft al een login, er wordt geen nieuwe aangemaakt.";
		} elseif (db_logins("controlebestaat", "", $login, $lidid)) {
			$mess = sprintf("Login '%s' is al in gebruik bij een ander lid.", $login);
		} elseif (strlen($login) < 6) {
			$mess = "De login moet minimaal 6 karakters lang zijn.";
		} elseif (strlen($pw) < db_param("wachtwoord_minlengte")) {
			$mess = sprintf("Het wachtwoord moet minimaal %d karakters lang zijn.", db_param("wachtwoord_minlengte"));
		} elseif ($lidid > 0) {
			$nrid = db_nieuwrecordID("Admin_login");
			$nk = password_hash(newkey(), PASSWORD_DEFAULT);
			$query = sprintf("INSERT INTO %sAdmin_login (RecordID, LidID, Login, Wachtwoord, ActivatieKey, LaatsteWachtwoordWijziging, Ingevoerd, Gewijzigd) VALUES 
																	  (%d, %d, '%s', '%s', '%s', SYSDATE(), SYSDATE(), SYSDATE());", TABLE_PREFIX, $nrid, $lidid, $login, password_hash($pw, PASSWORD_DEFAULT), $nk);
			$rv = fnQuery($query);
			if ($rv == 1) {
				$mess = sprintf("Login '%s' is aangemaakt.", $login);
				$mess .= fnValidatieLogin($lidid, $nk, "mail");
				$ok = true;
			} else {
				$mess = sprintf("Er ging iets mis bij het aanmaken van login '%s'.", $login);
			}
		}
		db_logboek("add", $mess, 5, $lidid, 1, $nrid);
		return $ok;
		
	} elseif ($actie == "rowlogin") {
		$query = sprintf("SELECT L.RecordID, Login.Login, %s AS Naam, L.Roepnaam, L.Email, Login.Wachtwoord, Login.FouteLogin, Login.Gewijzigd, MAX(IFNULL(LM.Lidnr, 0)) AS Lidnr, Login.LidID, ", $selectnaam);
		if (is_array($beperkttotgroep) and count($beperkttotgroep) > 0) {
			$query .= sprintf("(SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.Lid=L.RecordID AND LO.OnderdeelID IN (%s) AND LO.Vanaf <= CURDATE() AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd > CURDATE())) AS AanvragenMag", TABLE_PREFIX, implode(", ", $beperkttotgroep));
		} else {
			$query .= "1 AS AanvragenMag";
		}
		$query .= sprintf(" FROM (%1\$sLid AS L INNER JOIN %1\$sLidmaatschap AS LM ON LM.Lid=L.RecordID) LEFT JOIN %1\$sAdmin_login AS Login ON L.RecordID=Login.LidID WHERE LM.LIDDATUM < CURDATE() AND (IFNULL(LM.Opgezegd, CURDATE())) >= CURDATE() AND ", TABLE_PREFIX);
		if (isValidMailAddress($email, 0)) {
			$query .= sprintf("(LOWER(L.Email) LIKE '%1\$s' OR LOWER(L.EmailVereniging) LIKE '%1\$s' OR LOWER(L.EmailOuders) LIKE '%1\$s')", strtolower($email));
		} else {
			$query .= sprintf("L.RecordID=%d", $lidid);
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
			$mess = "Login is gevalideerd en kan worden gebruikt.";
		} else {
			$mess = sprintf("Er ging iets mis bij het valideren van de login voor %s is gevalideerd. Mogelijk is deze login al gevalideerd. Zo niet probeer het later opnieuw.", (new cls_Lid())->Naam($lidid));
		}
		return $mess;
		
	} elseif ($actie == "controlebestaat" and strlen($login) > 5) {
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login WHERE Login LIKE '%s' AND LidID<>%d;", TABLE_PREFIX, $login, $lidid);
		if (db_scalar($query) > 0) {
			return true;
		} else {
			return false;
		}
		
	} elseif ($actie == "foutelogin") {
		$query = sprintf("SELECT FouteLogin FROM %sAdmin_login WHERE LidID=%d", TABLE_PREFIX, $lidid);
		return db_scalar($query);
		
	} elseif ($actie == "uitloggen") {
		$query = sprintf("UPDATE %sAdmin_login SET Ingelogd=0 WHERE Ingelogd > 0 AND (LastActivity < ADDDATE(SYSDATE(), INTERVAL -%d MINUTE) OR LidID=%d);", TABLE_PREFIX, session_cache_expire(), $lidid);
		if (fnQuery($query) > 0) {
			if ($lidid > 0) {
				db_logboek("add", "Heeft uitgelogd.", 1, $lidid);
				$_SESSION['lidid'] = 0;
			}
			fnMaatwerkNaUitloggen();
		}
		
	} elseif ($actie == "aantalingelogd") {
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login WHERE Ingelogd=1;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "aantal") {
		$query = sprintf("SELECT COUNT(*) FROM %1\$sAdmin_login AS Login INNER JOIN %1\$sLid AS L ON Login.LidID = L.RecordID;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "delete") {
		if ($_SESSION['webmaster'] == 1) {
			$query = sprintf("SELECT Login FROM %sAdmin_login WHERE LidID=%d;", TABLE_PREFIX, $lidid);
			$login = db_scalar($query);
			$query = sprintf("DELETE FROM %sAdmin_login WHERE LidID=%d;", TABLE_PREFIX, $lidid);
			$result = fnQuery($query);
			if ($result > 0) {
				$mess = sprintf("Login '%s' is verwijderd.", $login);
				db_logboek("add", $mess, 5, $lidid);
			}
		} else {
			$mess = "Je hebt geen rechten om logins te verwijderen.";
			db_logboek("add", $mess, 5, 0, 1);
		}
		
	} elseif ($actie == "wachtwoordreset" and $lidid > 0) {
		$nk = newkey();
		if (strlen($nk) > 0) {
			$query = sprintf("UPDATE %sAdmin_login SET ActivatieKey='%s', Wachtwoord='', LaatsteWachtwoordWijziging=SYSDATE(), Gewijzigd=SYSDATE(), GewijzigdDoor=%d WHERE LidID=%d;", TABLE_PREFIX, password_hash($nk, PASSWORD_DEFAULT), $_SESSION['lidid'], $lidid);
			$result = fnQuery($query);
			if ($result == 1) {
				$mess = "Het wachtwoord is leeg gemaakt op verzoek van de gebruiker, zodat het hersteld kan worden.";
				db_logboek("add", $mess, 5, $lidid);
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
				$mess = "Er is een nieuwe activatiekey gemaakt voor de login.";
				db_logboek("add", $mess, 5, $lidid);
				return $nk;
			} else {
				return false;
			}
		} else {
			return false;
		}
		
	} elseif ($actie == "unlock" and $_SESSION['webmaster'] == 1) {
		$query = sprintf("UPDATE %sAdmin_login SET FouteLogin=0, Gewijzigd=SYSDATE(), GewijzigdDoor=%d WHERE LidID=%d AND FouteLogin > 0;", TABLE_PREFIX, $_SESSION['lidid'], $lidid);
		$result = fnQuery($query);
		if ($result == 1) {
			$mess = "Het aantal foutieve inlogpogingen van de login is gereset.";
			db_logboek("add", $mess, 1, $lidid);
		}
		
	} elseif ($actie === "controle") {
		if (db_param("login_autounlock") > 0) {
			$query = sprintf("SELECT Login, LidID FROM %sAdmin_login WHERE Gewijzigd < DATE_SUB(SYSDATE(), INTERVAL %d MINUTE) AND FouteLogin > 0;", TABLE_PREFIX, db_param("login_autounlock"));
			$result = fnQuery($query);
			foreach ($result->fetchAll() as $row) {
				$updqry = sprintf("UPDATE %sAdmin_login SET FouteLogin=0, Gewijzigd=SYSDATE() WHERE Login='%s';", TABLE_PREFIX, $row->Login);
				$updres = fnQuery($updqry);
				if ($updres > 0) {
					$mess = sprintf("Login %s is automatisch gedeblokkeerd.", $row->Login);
					db_logboek("add", $mess, 5, $row->LidID);
				}
			}
		}

		if (isValidMailAddress($login, 0)) {
			$query = sprintf("SELECT Login.Login FROM %1\$sLid AS L INNER JOIN %1\$sAdmin_login AS Login ON Login.LidID=L.RecordID WHERE (LOWER(L.Email) LIKE '%2\$s' OR LOWER(L.EmailVereniging) LIKE '%2\$s')", TABLE_PREFIX, strtolower($login));
			$result = fnQuery($query);
			$rows = $result->fetchAll();
			if (count($rows) == 1) {
				$login = $rows[0]->Login;
			}
		}
		$xw = sprintf("AND Login.Login LIKE '%s'", cleanlogin($login));
		if (is_array($beperkttotgroep) and count($beperkttotgroep) > 0) {
			$xw .= sprintf(" AND (Login.LidID IN (SELECT Lid FROM %sLidond AS LO WHERE %s AND LO.OnderdeelID IN (%s)) OR Login.LidID IN (%s))", TABLE_PREFIX, $wherelidond, implode(", ", $beperkttotgroep), implode(", ", $lididwebmasters));
		} else {
			$xw .= sprintf(" AND Login.LidID IN (%s)", implode(", ", $lididwebmasters));
		}
		$query = sprintf("SELECT Login.LidID, Login.Login, %3\$s AS NaamLid, L.Roepnaam, L.Email, L.EmailVereniging, L.EmailOuders, Login.Wachtwoord, Login.ActivatieKey,
					IFNULL((%4\$s), 0) AS Lidnr, Login.FouteLogin, Login.Gewijzigd
					FROM %1\$sAdmin_login AS Login INNER JOIN %1\$sLid AS L ON Login.LidID = L.RecordID
					WHERE LENGTH(Login.Wachtwoord) > 5 %2\$s
					ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", TABLE_PREFIX, $xw, $selectnaam, $selectlidnr);
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
		$xw = "";
		if (strlen($xtrafilter) > 0) {
			$xw = " AND " . $xtrafilter;
		}
		if (strlen($xtraorder) > 0) {
			$xtraorder .= ", ";
		}
		
		$query = sprintf("SELECT Login.LidID AS lnkNummer, Login.Login, %1\$s AS Naam_lid, L.Woonplaats, (%5\$s) AS Lidnr, L.Roepnaam AS ndRoepnaam,
					L.Achternaam AS ndAchternaam, L.Tussenv AS ndTussenvoegsels, L.Meisjesnm AS ndMeisjesnaam, L.GEBDATUM AS ndGeboren,
					IF(LENGTH(L.Email)=0, L.EmailOuders, L.Email) AS `E-mail`, L.EmailVereniging AS ndEmailVereniging, L.Telefoon AS ndTelefoon, L.Mobiel AS ndMobiel,
					Login.Ingevoerd AS dteIngevoerd, Login.LastLogin AS `dttLaatste login`,
					IF(LENGTH(Login.Wachtwoord) > 5 AND LENGTH(IFNULL(Login.ActivatieKey, '')) = 0, IF(Login.Ingelogd=1, 'Ingelogd', 'Gevalideerd'), 'Niet gevalideerd') AS `Status`,
					Login.Wachtwoord AS ndWachtwoord, IF(Login.FouteLogin > 0, Login.LidID, 0) AS llkNummer, Login.LidID AS lekNummer
					FROM %2\$sAdmin_login AS Login INNER JOIN %2\$sLid AS L ON Login.LidID = L.RecordID
					WHERE LENGTH(Login.Login) > 5 %3\$s
					ORDER BY %4\$sL.Achternaam, L.TUSSENV, L.Roepnaam, Login.LidID;", $selectnaam, TABLE_PREFIX, $xw, $xtraorder, $selectlidnr);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "login") {
		$query = sprintf("SELECT MAX(Login) FROM %Admin_login WHERE LidID=%d;", TABLE_PREFIX, $lidid);
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
					LidID NOT IN (SELECT Lid FROM %1\$sLidmaatschap WHERE IFNULL(Opgezegd, CURDATE()) >= CURDATE());", TABLE_PREFIX);
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

class cls_Login extends cls_db_base {
	
	public $login = "";
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Admin_login";
		$this->ta = 5;
	}
	
	private function vulvars() {
		if ($this->lidid > 0) {
			$query = sprintf("SELECT Login FROM %s WHERE LidID=%d;", $this->table, $this->lidid);
			$this->login = db_scalar($query);
		}
	}
	
	public function record($p_lidid) {
		$query = sprintf("SELECT * FROM %s WHERE LidID=%d;", $this->table, $p_lidid);
		return $this->execsql($query)->fetch();
	}
	
	public function lijst($p_filter="", $p_orderby="") {
		if (strlen($p_filter) > 0) {
			$p_filter = " AND " . $p_filter;
		}
		if (strlen($p_orderby) > 0) {
			$p_orderby .= ", ";
		}
		
		$query = sprintf("SELECT Login.LidID AS lnkNummer, Login.Login, %1\$s AS Naam_lid, L.Woonplaats, (%5\$s) AS Lidnr, L.Roepnaam AS ndRoepnaam,
					L.Achternaam AS ndAchternaam, L.Tussenv AS ndTussenvoegsels, L.Meisjesnm AS ndMeisjesnaam, L.GEBDATUM AS ndGeboren,
					IF(LENGTH(L.Email)=0, L.EmailOuders, L.Email) AS `E-mail`, L.EmailVereniging AS ndEmailVereniging, L.Telefoon AS ndTelefoon, L.Mobiel AS ndMobiel,
					Login.Ingevoerd AS dteIngevoerd, Login.LastLogin AS `dttLaatste login`,
					IF(LENGTH(Login.Wachtwoord) > 5 AND LENGTH(IFNULL(Login.ActivatieKey, '')) = 0, IF(Login.Ingelogd=1, 'Ingelogd', 'Gevalideerd'), 'Niet gevalideerd') AS `Status`,
					Login.Wachtwoord AS ndWachtwoord, IF(Login.FouteLogin > 0, Login.LidID, 0) AS llkNummer, Login.LidID AS lekNummer
					FROM %2\$sAdmin_login AS Login INNER JOIN %2\$sLid AS L ON Login.LidID = L.RecordID
					WHERE LENGTH(Login.Login) > 5 %3\$s
					ORDER BY %4\$sL.Achternaam, L.TUSSENV, L.Roepnaam, Login.LidID;", $this->selectnaam, TABLE_PREFIX, $p_filter, $p_orderby, $this->selectlidnr);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function add($p_lidid, $p_login, $p_wachtwoord) {
		$this->lidid = $p_lidid;
		$p_login = cleanlogin($p_login);
		$this->tm = 1;
		$ok = false;
		
		$query = sprintf("SELECT Login FROM %sAdmin_login WHERE LidID=%d;", TABLE_PREFIX, $p_lidid);
		if (strlen(db_scalar($query)) > 0) {
			$this->mess = "Heeft al een login, er wordt geen nieuwe aangemaakt.";
		} elseif (db_logins("controlebestaat", "", $p_login, $p_lidid)) {
			$this->mess = sprintf("Login '%s' is al in gebruik bij een ander lid.", $p_login);
		} elseif (strlen($p_login) < 6) {
			$this->mess = "De login moet minimaal 6 karakters lang zijn.";
		} elseif (strlen($p_wachtwoord) < db_param("wachtwoord_minlengte")) {
			$this->mess = sprintf("Het wachtwoord moet minimaal %d karakters lang zijn.", db_param("wachtwoord_minlengte"));
		} elseif ($lidid > 0) {
			$nrid = db_nieuwrecordID("Admin_login");
			$nk = password_hash(newkey(), PASSWORD_DEFAULT);
			$query = sprintf("INSERT INTO %sAdmin_login (RecordID, LidID, Login, Wachtwoord, ActivatieKey, LaatsteWachtwoordWijziging, Ingevoerd) VALUES 
																	  (%d, %d, '%s', '%s', '%s', SYSDATE(), SYSDATE());", TABLE_PREFIX, $nrid, $this->lidid, $p_login, password_hash($p_wachtwoord, PASSWORD_DEFAULT), $nk);
			if (fnQuery($query) == 1) {
				$this->mess = sprintf("Login '%s' is aangemaakt.", $p_login);
				$this->mess .= fnValidatieLogin($lidid, $nk, "mail");
				$ok = true;
			} else {
				$this->mess = sprintf("Er ging iets mis bij het aanmaken van login '%s'.", $p_login);
			}
		}
		
		$this->Log($nrid);
		return $ok;
	}
	
	public function update($p_lidid, $p_kolom, $p_waarde) {
		$this->lidid = $p_lidid;
		$this->vulvars();
		
		if ($this->is_kolom_numeriek($p_kolom)) {
			$p_waarde = intval($p_waarde);
			$xw = sprintf("IFNULL(`%s`, 0) <>%d", $p_kolom, $p_waarde);
		} else {
			$p_waarde = "'" . $p_waarde . "";
			$xw = sprintf("IFNULL(`%s`, '') <>%s", $p_kolom, $p_waarde);
		}
		
		if ($_SESSION['webmaster'] == 1) {
			$query = sprintf("UPDATE %s SET `%s`=%s, Gewijzigd=SYSDATE(), GewijzigdDoor=%d WHERE Lid=%d AND %s;", $this->table, $p_kolom, $p_waarde, $_SESSION['lidid'], $p_lidid, $xw);
			if (fnQuery($query) > 0) {
				$this->mess = sprintf("Kolom '%s' van login '%s' in %s gewijzigd.", $p_kolom, $this->login, $p_waarde);
			}
		} else {
			$this->mess = "Je bent niet bevoegd om logins te wijzigen.";
		}
			
		$this->Log();
	}
	
	public function delete($p_lidid) {
		$this->lidid = $p_lidid;
		$this->vulvars();
		
		$query = sprintf("DELETE FROM %s WHERE LidID=%d;", $this->table, $p_lidid);
		if (fnQuery($query) > 0) {
			$this->mess = sprintf("Login '%s' is verwijderd.", $this->login);
			$this->tas = 3;
			$this->Log();
		}
	}
	
	public function setlastactivity() {
		$query = sprintf("UPDATE %s SET LastActivity=SYSDATE() WHERE Ingelogd=1 AND LidID=%d;", $this->table, $_SESSION['lidid']);
		$this->execsql($query);
	}
	
	public function autounlock() {
		if (db_param("login_autounlock") > 0) {
			$query = sprintf("SELECT Login, LidID FROM %s WHERE Gewijzigd < DATE_SUB(SYSDATE(), INTERVAL %d MINUTE) AND FouteLogin > 0;", $this->table, db_param("login_autounlock"));
			$result = $this->execsql($query);
			foreach ($result->fetchAll() as $row) {
				$updqry = sprintf("UPDATE %s SET FouteLogin=0, Gewijzigd=SYSDATE() WHERE Login='%s';", $this->table, $row->Login);
				if ($this->execsql($updqry) > 0) {
					$this->lidid = $row->LidID;
					$this->mess = sprintf("Login %s is automatisch gedeblokkeerd.", $row->Login);
					$this->tas = 3;
					$this->Log();
				}
			}
		}
	}
		
	public function setingelogd($p_lidid) {
		$this->lidid = $p_lidid;
		$this->ta = 1;
		
		$query = sprintf("UPDATE %s SET LastLogin=SYSDATE(), LastActivity=SYSDATE(), Ingelogd=1, FouteLogin=0 WHERE Ingelogd=0 AND LidID=%d;", $this->table, $this->lidid);
		if (fnQuery($query) > 0) {
			$this->tas = 1;
			$this->mess = sprintf("Heeft met '%s' ingelogd.", $_SESSION['username']);
			if (db_Logboek("aantal", "", -1, $p_lidid, 0, 0, $_SERVER['REMOTE_ADDR']) == 0) {
				$this->mess .= sprintf(" Dit is voor het eerst vanaf IP-adres %s.", $_SERVER['REMOTE_ADDR']);
				if (db_param("mailing_meldingnieuwip") > 0) {
				}
			}
			$this->Log();
		} elseif ($_SESSION['lidid'] > 0) {
			$query = sprintf("UPDATE %sAdmin_login SET LastActivity=SYSDATE() WHERE LidID=%d;", TABLE_PREFIX, $_SESSION['lidid']);
			fnQuery($query);
		}
	}
	
	public function uitloggen($p_lidid=0) {
		$this->ta = 1;
		$this->tas = 2;
		if ($p_lidid > 0) {
			$this->lidid = $p_lidid;
			$query = sprintf("UPDATE %s SET Ingelogd=0 WHERE Ingelogd > 0 AND LidID=%d;", $this->table, $p_lidid);
			if ($this->execsql($query) > 0) {
				$this->mess = "Heeft uitgelogd.";
				$this->Log();
				$_SESSION['lidid'] = 0;
			}
			fnMaatwerkNaUitloggen();
		} else {
			$query = sprintf("SELECT LidID FROM %s WHERE Ingelogd > 0 AND (LastActivity < ADDDATE(SYSDATE(), INTERVAL -%d MINUTE));", $this->table, session_cache_expire());
			$result = $this->execsql($query);
			foreach($result->fetchAll() as $row) {
				$updqry = sprintf("UPDATE %s SET Ingelogd=0 WHERE LidID=%d;", $this->table, $row->LidID);
				if ($this->execsql($updqry) > 0) {
					$this->lidid = $row->LidID;
					$this->mess = "Is automatisch uitgelogd.";
					$this->Log();
				}
			}
		}
	}
	
}  # cls_Login

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
			$mess = "Het wachtwoord van de login is gewijzigd.";
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

function db_add_mailing_hist($lidid, $mid, $to_name="", $to_addr="", $cc_addr="", $message="", $xtrachar="", $xtranum=0, $onderwerp="") {
	
	$from_name = "";
	$from_addr = "";
	$subject = $onderwerp;
	$zichtbaarvoor = 0;
	
	if (is_array($to_addr)) {
		$to_addr = implode(",", $to_addr);
	}
	if (is_array($cc_addr)) {
		$cc_addr = implode(",", $cc_addr);
	}
		
	if (strlen($to_addr) > 250) {
		$to_addr = substr($to_addr, 0, 251) . " ...";
	}
	if (strlen($cc_addr) > 250) {
		$cc_addr = substr($cc_addr, 0, 251) . " ...";
	}

	if ($mid > 0) {
		$query = sprintf("SELECT * FROM %sMailing WHERE MailingID=%d;", TABLE_PREFIX, $mid);
		$result = fnQuery($query);
		$row = $result->fetch();
		
		$from_name = $row->from_name;
		$from_addr = $row->from_addr;
		$zichtbaarvoor = $row->ZichtbaarVoor;
		
		if (strlen($subject) == 0) {
			$subject = $row->subject;
		}
		if (strlen($to_name) == 0) {
			$to_name = $row->to_name;
		}
		if (strlen($message) == 0) {
			$message = $row->message;
		}
		if (strlen($cc_addr) == 0) {
			$cc_addr = $row->cc_addr;
		}
	} elseif ($xtrachar == "REK") {
		$from_addr = db_param("mailing_rekening_from_adres");
		$from_name = db_param("mailing_rekening_from_naam");
	}
	$from_name = html_entity_decode(str_replace("\"", "'", $from_name));
	$subject = html_entity_decode(str_replace("\"", "'", $subject));
	$to_name = html_entity_decode(str_replace("\"", "'", $to_name));
	$message = html_entity_decode(str_replace("\"", "'", $message));
	
	$nrid = db_nieuwrecordID("Mailing_hist");
	$query = sprintf("INSERT INTO %sMailing_hist (RecordID, LidID, MailingID, from_name, from_addr, subject, to_name, to_addr, cc_addr, message, ZichtbaarVoor, Xtra_Char, Xtra_Num, IngevoerdDoor)
					VALUES (%d, %d, %d, \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", %d, '%s', %d, %d);",
					TABLE_PREFIX, $nrid, $lidid, $mid, $from_name, $from_addr, $subject, $to_name, $to_addr, $cc_addr, $message, $zichtbaarvoor, $xtrachar, $xtranum, $_SESSION['lidid']);
	$rv = fnQuery($query);
	
	return $rv;

}  # db_add_mailing_hist

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
		if (in_array(db_param("mailing_alle_zien"), explode(", ", $_SESSION['lidgroepen']))) {
			$w = "1=1";
		} else {
			$w = sprintf("M.ZichtbaarVoor IN (%s)", $_SESSION['lidgroepen']);
		}
		$orderby = "sent_on DESC";
		if ($filter == "Concepten" or $filter === "Verzonden") {
			if ($filter == "Verzonden") {
				$w .= sprintf(" AND (NOT M.Sent_on IS NULL)", TABLE_PREFIX);
//				$xtra_sel = sprintf(", CONCAT(IF(M.SentBy=0, '', (SELECT %s FROM %sLid AS L WHERE L.RecordID=M.SentBy)), ' & ', DATE_FORMAT(M.sent_on, %s)) AS `mrgLaatst verzonden door/op`", $selectnaam, TABLE_PREFIX, $fdtlang);
			} else {
				$w .= sprintf(" AND (M.Sent_on IS NULL)", TABLE_PREFIX);
			}
			$w .= " AND template=0 AND (deleted_on IS NULL)";
		} elseif ($filter == "Templates") {
			$w .= " AND M.template=1 AND (deleted_on IS NULL)";
//			$xtra_sel = sprintf(", CONCAT(IF(M.SentBy=0, '', (SELECT %s FROM %sLid AS L WHERE L.RecordID=M.SentBy)), ' & ', DATE_FORMAT(M.sent_on, %s)) AS `mrgLaatst verzonden door/op`", $selectnaam, TABLE_PREFIX, $fdtlang);
			$orderby = "M.subject, sent_on";
			
		} elseif ($filter == "Verzonden mails") {
			
		} elseif ($filter == "Prullenbak") {
			$w .= " AND (NOT M.deleted_on IS NULL)";
			$orderby = "M.deleted_on DESC";
			$xtra_sel = sprintf(", CONCAT((SELECT %s FROM %sLid AS L WHERE L.RecordID=M.DeletedBy), ' & ', DATE_FORMAT(M.deleted_on, %s)) AS `mrgVerwijderd`", $selectnaam, TABLE_PREFIX, $fdtlang);
		} else {
			$w .= " AND (M.deleted_on IS NULL)";
		}
		
		$query = sprintf("SELECT M.MailingID AS lnkMailingID, M.MailingID AS intNr, M.subject AS Onderwerp, M.from_name AS Van, M.to_name AS Aan,
					CONCAT(%1\$s, ' & ', DATE_FORMAT(M.changed_on, %6\$s)) AS `mrgLaatst gewijzigd`
					%2\$s,
					IF((SELECT COUNT(*) FROM %3\$sMailing_hist AS MH WHERE MH.MailingID=M.MailingID) > 0, M.MailingID, 0) AS llkHist
					FROM %3\$sMailing AS M LEFT JOIN %3\$sLid AS L ON M.GewijzigdDoor=L.RecordID
					WHERE %4\$s ORDER BY %5\$s;", $selectnaam, $xtra_sel, TABLE_PREFIX, $w, $orderby, $fdtlang);
		$result = fnQuery($query);
//		debug($query);
		return $result->fetchAll();
		
	} elseif ($actie == "rcpt") {
		$query = sprintf("SELECT R.LidID, IFNULL(L.RecordID, 0) AS RecordID, %1\$s AS Naam_lid, %2\$s AS Zoeknaam_lid, L.Adres, L.Postcode, 
					L.Woonplaats, L.Email, L.EmailVereniging, L.EmailOuders, L.GEBDATUM, R.to_address
					FROM %3\$sMailing_rcpt AS R LEFT OUTER JOIN %3\$sLid AS L ON R.LidID = L.RecordID
					WHERE MailingID=%4\$d
					ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, R.to_address, R.Ingevoerd;", $selectnaam, $selectzoeknaam, TABLE_PREFIX, $mid);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "potrcpt") {
		$query = sprintf("SELECT L.RecordID AS LidID, %1\$s AS Zoeknaam_lid FROM %2\$sLid AS L
					WHERE (L.Verwijderd IS NULL) AND (L.Overleden IS NULL) AND (LENGTH(L.Email) > 5 OR LENGTH(L.EmailVereniging) > 5 OR LENGTH(L.EmailOuders) > 5 OR (SELECT COUNT(*) FROM %2\$sLidmaatschap AS LM WHERE IFNULL(LM.Opgezegd, CURDATE()) >= CURDATE() AND LM.Lid=L.RecordID) > 0)
					AND (SELECT COUNT(*) FROM %2\$sMailing_rcpt AS R WHERE R.LidID=L.RecordID AND R.MailingID=%3\$d)=0
					ORDER BY L.Achternaam, L.Roepnaam;", $selectzoeknaam, TABLE_PREFIX, $mid);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "aantalprullenbak") {
		$query = sprintf("SELECT COUNT(*) FROM %sMailing WHERE (NOT deleted_on IS NULL);", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "laatstehist" and $mid > 0) {
		$query = sprintf("SELECT send_on, send_by FROM %sMailing_hist WHERE send_on > '2000-01-01' AND MailingID=%d ORDER BY send_on DESC, RecordID DESC LIMIT 1;", TABLE_PREFIX, $mid);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "laatstehist") {
		$query = sprintf("SELECT send_on FROM %sMailing_hist WHERE send_on > '2000-01-01' ORDER BY send_on DESC, RecordID DESC LIMIT 1;", TABLE_PREFIX, $mid);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "add") {
		$subj = str_replace("\"", "'", $new_val);
		$nrid = db_nieuwrecordID("Mailing");
		$query = sprintf("INSERT INTO %sMailing (MailingID, subject, AddedBy) VALUES (%d, \"%s\", %d);", TABLE_PREFIX, $nrid, $subj, $_SESSION['lidid']);
		if (fnQuery($query) > 0) {
			$mess = sprintf("Mailing %d (%s) is toegevoegd.", $nrid, $subj);
		} else {
			$mess = sprintf("Het toevoegen van mailing '%s' is mislukt.", $subj);
			$nrid = 0;
		}
		db_logboek("add", $mess, 4, 0, 0, $nrid);
		return $nrid;
		
	} elseif ($actie === "update") {
		
		$mess = "";
		$in = db_kolom_numeriek("Mailing", $kolnaam);
		if ($in) {
			$clause = sprintf("`%s`=%d", $kolnaam, $new_val);
			$where = sprintf("`%s`<>%d", $kolnaam, $new_val);
		} else {
			$new_val = str_replace("\"", "'", $new_val);
			$clause = sprintf("`%s`=\"%s\"", $kolnaam, $new_val);
			$where = sprintf("BINARY IFNULL(`%s`, '')<>\"%s\"", $kolnaam, $new_val);
		}
		$clause .= sprintf(", changed_on=SYSDATE(), ChangedBy=%1\$d, GewijzigdDoor=%1\$d", $_SESSION['lidid']);
		$where .= sprintf(" AND MailingID=%d", $mid);
		$query = sprintf("UPDATE %sMailing SET %s WHERE %s;", TABLE_PREFIX, $clause, $where);
//		debug($query);
		$result = fnQuery($query);
		if ($result > 0) {
			if ($in) {
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
		}
			
		$query = sprintf('DELETE FROM %1$sMailing_rcpt
						  WHERE MailingID NOT IN (SELECT M.MailingID FROM %1$sMailing AS M);', TABLE_PREFIX);
		$result = fnQuery($query, 2);
			
		$query = sprintf('UPDATE %1$sMailing_hist SET MailingID=0
						  WHERE MailingID > 0 AND MailingID NOT IN (SELECT M.MailingID FROM %1$sMailing AS M);', TABLE_PREFIX);
		$result = fnQuery($query, 2);
		
		// Deze code kan verwijderd worden na 1/1/2021
		$query = sprintf('UPDATE %1$sMailing AS M SET M.GewijzigdDoor=ChangedBy WHERE GewijzigdDoor=0;', TABLE_PREFIX);
		$result = fnQuery($query, 2);
		//
		
	} elseif ($actie == "record") {
		$query = sprintf("SELECT M.* FROM %sMailing AS M WHERE MailingID=%d;", TABLE_PREFIX, $mid);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "zichtbaarvoor") {
		$query = sprintf("SELECT IFNULL(MAX(M.ZichtbaarVoor), 0) FROM %sMailing AS M WHERE M.MailingID=%d;", TABLE_PREFIX, $mid);
		return db_scalar($query);
		
	} else {
		$mess = sprintf("Actie '%s' is onbekend.", $actie);
		db_logboek("add", $mess, 99);
	}
	
} # db_mailing

class cls_Mailing extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Mailing";
		$this->ta = 4;
	}
	
	public function add($p_subject) {
		
		$p_subject = str_replace("\"", "'", $p_subject);
		$nrid = db_nieuwrecordID("Mailing");
		$query = sprintf("INSERT INTO %s (MailingID, subject, OmschrijvingOntvangers, AddedBy) VALUES (%d, \"%s\", '', %d);", $this->table, $nrid, $p_subject, $_SESSION['lidid']);
		if (fnQuery($query) > 0) {
			$this->mess = sprintf("Mailing %d (%s) is toegevoegd.", $nrid, $p_subject);
			$this->Log($nrid);
			
			$query = sprintf("UPDATE %s SET RecordID=MailingID WHERE MailingID > 0 AND IFNULL(RecordID, 0)=0;", $this->table);
			$this->execsql($query);
			return $nrid;
		} else {
			return 0;
		}
	}
	
	public function update($p_mid, $p_kolom, $p_waarde) {
		
		if ($this->is_kolom_numeriek($p_kolom)) {
			$p_waarde = intval($p_waarde);
			$xw = sprintf("IFNULL(`%s`, 0)<>%d", $p_kolom, $p_waarde);
		} else {
			$p_waarde = "\"" . str_replace("\"", "'", $p_waarde) . "\"";
			$xw = sprintf("BINARY IFNULL(`%s`, '')<>%s", $p_kolom, $p_waarde);
		}
		$query = sprintf("UPDATE %s SET `%s`=%s, Gewijzigd=SYSDATE(), GewijzigdDoor=%d WHERE MailingID=%d AND %s;", $this->table, $p_kolom, $p_waarde, $_SESSION['lidid'], $p_mid, $xw);
		if (fnQuery($query) > 0) {
			if (strlen($p_waarde) > 125) {
				$p_waarde = substr($p_waarde, 0, 121) . " ...";
			}
			$this->mess = sprintf("In mailing %d is kolom '%s' is in %s gewijzigd. ", $p_mid, $p_kolom, $p_waarde);
			$this->Log($p_mid);
		}
	}
	
	public function trash($p_mid, $p_direction="in") {
		
		if ($p_direction == "in") {
			$query = sprintf("UPDATE %s SET deleted_on=SYSDATE(), DeletedBy=%d WHERE MailingID=%d AND (deleted_on IS NULL);", $this->table, $_SESSION['lidid'], $p_mid);
			if (fnQuery($query) > 0) {
				$this->mess = sprintf("Mailing %d is naar de prullenbak verplaatst.", $p_mid);
			}
		} else {
			$query = sprintf("UPDATE %s SET deleted_on=NULL WHERE MailingID=%d AND (deleted_on NOT IS NULL);", $this->table, $p_mid);
			if (fnQuery($query) > 0) {
				$this->mess = sprintf("Mailing %d is uit de prullenbak gehaald.", $p_mid);
			}
		}
		
		$this->Log($p_mid);
	}
		
}  # cls_Mailing

function db_mailing_hist($actie, $lidid=0, $mhid=0, $mid=0) {
	global $selectnaam;
	
	$xw = "";
	if ($lidid > 0) {
		$xw = sprintf("AND MH.LidID=%d", $lidid);
	} elseif ($_SESSION['webmaster'] == 0) {
		$xw = sprintf("AND (MH.ZichtbaarVoor IN (%s) OR MH.LidID=%d)", $_SESSION['lidgroepen'], $_SESSION['lidid']);
		if (toegang("Mailing/Rekeningen", 0, 0) == false) {
			$xw .= sprintf(" AND (MH.LidID=%d OR MH.Xtra_Char<>'REK')", $_SESSION['lidid']);
		}
	}
	
	if ($actie == "outbox" or $actie == "outboxbatch" or $actie == "outboxspeciaal") {
		$lm = 1000;
		if (db_param("maxmailsperdag") > 0 and db_param("maxmailsperdag") < $lm) {
			$lm = db_param("maxmailsperdag");
		}
		if (db_param("maxmailsperuur") > 0 and db_param("maxmailsperuur") < $lm) {
			$lm = db_param("maxmailsperuur");
		}
		$xw = "";
		if ($actie == "outboxbatch") {
			$xw = "AND MH.Ingevoerd < DATE_SUB(SYSDATE(), INTERVAL 15 MINUTE)";
		} elseif ($actie == "outboxspeciaal") {
			$xw = sprintf("AND MH.MailingID > 0 AND MH.MailingID IN (%d, %d, %d)", db_param("mailing_lidnr"), db_param("mailing_validatielogin"), db_param("mailing_herstellenwachtwoord"));
		} elseif ($_SESSION['webmaster'] == 0) {
			$xw = sprintf("AND MH.ZichtbaarVoor IN (%s)", $_SESSION['lidgroepen']);
		} else {
			$xw = "";
		}
		$query = sprintf("SELECT MH.RecordID AS lnkRecordID, MH.Ingevoerd AS `dttWacht vanaf`, MH.subject AS Onderwerp, MH.to_name AS Aan,
						MH.to_addr AS `E-mail`, MH.cc_addr AS `CC`, MH.RecordID AS llkRecordID
						FROM (%1\$sMailing_hist AS MH LEFT OUTER JOIN %1\$sLid AS L ON MH.LidID=L.RecordID) LEFT JOIN %1\$sMailing AS M ON M.MailingID=MH.MailingID
						WHERE IFNULL(MH.send_on, '1970-01-01') <= '2000-01-01' %2\$s
						ORDER BY MH.RecordID LIMIT %3\$d;", TABLE_PREFIX, $xw, $lm);
		return fnQuery($query);
		
	} elseif ($actie == "aantaloutbox") {
		$query = sprintf("SELECT SQL_NO_CACHE COUNT(*) FROM %sMailing_hist AS MH WHERE IFNULL(MH.send_on, '1970-01-01') <= '2000-01-01';", TABLE_PREFIX);
		return db_scalar($query);

	} elseif ($actie == "lijst" or $actie == "sqllijst") {
		if ($mid > 0) {
			$query = sprintf("SELECT MH.RecordID AS lnkRecordID, M.subject AS ndOnderwerp, IF(MH.send_on > '2000-01-01', MH.send_on, 'Niet verzonden') AS dtsVerzonden, IF(MH.LidID=0, M.to_name, %1\$s) AS Aan,
						to_addr AS `E-mail`, MH.cc_addr AS `CC`, MH.RecordID AS llkRecordID
						FROM (%2\$sMailing_hist AS MH LEFT JOIN %2\$sLid AS L ON MH.LidID=L.RecordID) LEFT JOIN %2\$sMailing AS M ON MH.MailingID=M.MailingID
						WHERE MH.send_on > '2000-01-01' AND MH.MailingID=%3\$d %4\$s
						ORDER BY MH.RecordID DESC;", $selectnaam, TABLE_PREFIX, $mid, $xw);
						
		} else {
			$query = sprintf("SELECT MH.RecordID AS lnkRecordID,
						MH.send_on AS dttVerzonden,
						IF(LENGTH(MH.from_name)<3, MH.from_addr, MH.from_name) AS Van,
						IF(MH.LidID=0, MH.to_addr, %1\$s) AS Aan,
						MH.subject AS Onderwerp,
						MH.RecordID AS llkRecordID
						FROM %2\$sMailing_hist AS MH LEFT JOIN %2\$sLid AS L ON MH.LidID=L.RecordID
						WHERE MH.send_on > '2000-01-01' %3\$s
						ORDER BY MH.RecordID DESC LIMIT 1000;", $selectnaam, TABLE_PREFIX, $xw);
		}
		if ($actie == "sqllijst") {
			return $query;
		} else {
			return fnQuery($query);
		}
		
	} elseif ($actie === "record") {
		$query = sprintf("SELECT MH.Ingevoerd, MH.send_on, MH.from_addr, MH.from_name, M.to_name, MH.to_addr, MH.cc_addr, MH.subject, MH.message, MH.send_by, MH.LidID, MH.to_name AS Aan, MH.IngevoerdDoor, MH.LidID, 
					MH.MailingID, MH.Xtra_Char, MH.Xtra_Num, MH.ZichtbaarVoor, M.ZonderBriefpapier
					FROM (%1\$sMailing_hist AS MH LEFT JOIN %1\$sLid AS L ON MH.LidID=L.RecordID) LEFT JOIN %1\$sMailing AS M ON MH.MailingID=M.MailingID
					WHERE MH.RecordID=%2\$d LIMIT 1;", TABLE_PREFIX, $mhid);
		$result = fnQuery($query);
		return $result->fetch();
	
	} elseif ($actie == "lidbijmail") {
		$query = sprintf("SELECT IFNULL(MAX(LidID), 0) FROM %sMailing_hist WHERE RecordID=%d;", TABLE_PREFIX, $mhid);
		return db_scalar($query);
		
	} elseif ($actie == "aantalperminuut") {
		$query = sprintf("SELECT SQL_NO_CACHE COUNT(*) FROM %sMailing_hist WHERE send_on > DATE_SUB(NOW(), INTERVAL 1 MINUTE);", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "aantalperuur") {
		$query = sprintf("SELECT COUNT(*) FROM %sMailing_hist WHERE send_on > DATE_SUB(NOW(), INTERVAL 1 HOUR);", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "aantal24uur") {
		$query = sprintf("SELECT COUNT(*) FROM %sMailing_hist WHERE send_on > DATE_SUB(NOW(), INTERVAL 24 HOUR);", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "laatste") {
		$query = sprintf("SELECT IFNULL(MH.Ingevoerd, '1999-01-01') AS VerzondenOp, %1\$s AS Door FROM %2\$sMailing_hist AS MH LEFT OUTER JOIN %2\$sLid AS L ON MH.IngevoerdDoor=L.RecordID WHERE MailingID=%3\$d ORDER BY MH.RecordID DESC;", $selectnaam, TABLE_PREFIX, $mid);
		$result = fnQuery($query);
		return $result->fetch();

	} elseif ($actie == "updatesent") {
		$query = sprintf("UPDATE %sMailing_hist SET send_on=SYSDATE(), send_by=%d WHERE RecordID=%d;", TABLE_PREFIX, $_SESSION['lidid'], $mhid);
		fnQuery($query);
		
	} elseif ($actie == "delete") {
		$query = sprintf("SELECT LidID FROM %sMailing_hist WHERE RecordID=%d;", TABLE_PREFIX, $mhid);
		$lidid = db_scalar($query);
		$query = sprintf("DELETE FROM %sMailing_hist WHERE RecordID=%d;", TABLE_PREFIX, $mhid);
		if (fnQuery($query) > 0) {
			$mess = sprintf("Record %d in Mailing_hist is verwijderd.", $mhid);
			db_logboek("add", $mess, 4, $lidid);
		}
		
	} else {
		$mess = sprintf("Actie '&s' is onbekend.", $actie);
		db_logboek("add", $mess, 99);
	}

} # db_mailing_hist

function db_mailing_hist_add($email) {
	
	if (strlen($email->aanadres) > 250) {
		$email->aanadres = substr($email->aanadres, 0, 251) . " ...";
	}
	if (strlen($email->cc) > 250) {
		$email->cc = substr($email->cc, 0, 251) . " ...";
	}

	$email->vanafnaam = html_entity_decode(str_replace("\"", "'", $email->vanafnaam));
	$email->onderwerp = html_entity_decode(str_replace("\"", "'", $email->onderwerp));
	$email->aannaam = html_entity_decode(str_replace("\"", "'", $email->aannaam));
	$email->bericht = html_entity_decode(str_replace("\"", "'", $email->bericht));
	
	$query = sprintf("INSERT INTO %sMailing_hist SET
					LidID=%d, MailingID=%d, from_name=\"%s\", from_addr=\"%s\", subject=\"%s\", to_name=\"%s\", to_addr=\"%s\", cc_addr=\"%s\", message=\"%s\", ZichtbaarVoor=%d, Xtra_Char='%s', Xtra_Num=%d, IngevoerdDoor=%d;",
					TABLE_PREFIX, $email->lidid, $email->mailingid, $email->vanafnaam, $email->vanafadres, $email->onderwerp, $email->aannaam, $email->aanadres, $email->cc, $email->bericht, $email->zichtbaarvoor, $email->xtrachar, $email->xtranum, $_SESSION['lidid']);
	$rv = fnQuery($query);
	
	return $rv;

}  # db_mailing_hist_add

class cls_Mailing_hist extends cls_db_base {
		
	function __construct() {
		$this->table = TABLE_PREFIX . "Mailing_hist";
		$this->ta = 4;
	}
		
	public function record($p_mhid) {
		
		$query = sprintf("SELECT MH.Ingevoerd, MH.send_on, MH.from_addr, MH.from_name, M.to_name, MH.to_addr, MH.cc_addr, MH.subject, MH.message, MH.send_by, MH.LidID, MH.to_name AS Aan, MH.IngevoerdDoor, MH.LidID, 
						  MH.MailingID, MH.Xtra_Char, MH.Xtra_Num, MH.ZichtbaarVoor, M.ZonderBriefpapier
						  FROM (%1\$sMailing_hist AS MH LEFT JOIN %1\$sLid AS L ON MH.LidID=L.RecordID) LEFT JOIN %1\$sMailing AS M ON MH.MailingID=M.MailingID
						  WHERE MH.RecordID=%2\$d LIMIT 1;", TABLE_PREFIX, $p_mhid);
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
	public function add($p_email) {
		$this->lidid = $p_email->lidid;
		$nrid = db_nieuwrecordID($this->table);
		
		if (strlen($p_email->aanadres) > 250) {
			$p_email->aanadres = substr($p_email->aanadres, 0, 251) . " ...";
		}
		if (strlen($p_email->cc) > 250) {
			$p_email->cc = substr($p_email->cc, 0, 251) . " ...";
		}

		$p_email->vanafnaam = html_entity_decode(str_replace("\"", "'", $p_email->vanafnaam));
		$p_email->onderwerp = html_entity_decode(str_replace("\"", "'", $p_email->onderwerp));
		$p_email->aannaam = html_entity_decode(str_replace("\"", "'", $p_email->aannaam));
		$p_email->bericht = html_entity_decode(str_replace("\"", "'", $p_email->bericht));
	
		$query = sprintf("INSERT INTO %sMailing_hist SET
						RecordID=%d, LidID=%d, MailingID=%d, from_name=\"%s\", from_addr=\"%s\", subject=\"%s\", to_name=\"%s\", to_addr=\"%s\", cc_addr=\"%s\", message=\"%s\", ZichtbaarVoor=%d, Xtra_Char='%s', Xtra_Num=%d, IngevoerdDoor=%d;",
						TABLE_PREFIX, $nrid, $p_email->lidid, $p_email->mailingid, $p_email->vanafnaam, $p_email->vanafadres, $p_email->onderwerp, $p_email->aannaam, $p_email->aanadres, $p_email->cc, $p_email->bericht, $p_email->zichtbaarvoor, $p_email->xtrachar, $p_email->xtranum, $_SESSION['lidid']);
		if (fnQuery($query) > 0) {
			return $nrid;
		} else {
			return 0;
		}
	}
	
	public function update($p_mhid, $p_kolom, $p_waarde) {
		
		if ($this->is_kolom_numeriek($p_kolom)) {
			$p_waarde = intval($p_waarde);
			$xw = sprintf("IFNULL(`%s`, 0)<>%s", $p_kolom, $p_waarde);
		} else {
			$p_waarde = "'" . $p_waarde . "'";
			$xw = sprintf("IFNULL(`%s`, '')<>%s", $p_kolom, $p_waarde);
		}
		
		$query = sprintf("UPDATE %s SET `%s`=%s, Gewijzigd=SYSDATE() WHERE RecordID=%d AND %s;", $this->table, $p_kolom, $p_waarde, $p_mhid, $xw);
		if (fnQuery($query) > 0) {
			$this->mess = sprintf("in %s-record %d is kolom '%s' in %s gewijzigd.", $this->table, $p_mhid, $p_kolom, $p_waarde);
		} else {
			$this->ta = 99;
			$this->mess = sprintf("Update mislukt: %s", $query);
			$this->Log($p_mhid);
		}
//		$this->Log($p_mhid);
	}
}  # cls_Mailing_hist

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
			$query = "";
			$nrid = db_nieuwrecordID("Mailing_rcpt");
			if ($lidid > 0) {
				$rl = db_lid("record", "", $lidid);
				$r = $rl->NaamLid;
				if (isValidMailAddress($rl->Email) or isValidMailAddress($rl->EmailOuders) or isValidMailAddress($rl->EmailVereniging)) {
					$query = sprintf("INSERT INTO %sMailing_rcpt (RecordID, MailingID, LidID) VALUES (%d, %d, %d);", TABLE_PREFIX, $nrid, $mid, $lidid);
				} else {
					$mess = sprintf("%s is niet aan mailing %d toegevoegd, omdat er geen geldig emailadres bekend is.", $r, $mid);
				}
				$rl = null;
			} elseif (isValidMailAddress($email)) {
				$query = sprintf("INSERT INTO %sMailing_rcpt (RecordID, MailingID, to_address) VALUES (%d, %d, '%s');", TABLE_PREFIX, $nrid, $mid, $email);
				$r = $email;
			} else {
				$mess = sprintf("%s is niet aan mailing %d toegevoegd, omdat dit geen correct emailadres is.", $email, $mid);
			}
			if (strlen($query) > 5 and fnQuery($query) > 0) {
				$mess = sprintf("%s is aan mailing %d toegevoegd.", $r, $mid);
				$tlm = 0;
			} else {
				$nrid = 0;
			}
		}
		if ($lb == 1 and strlen($mess) > 0) {
			db_logboek("add", $mess, 4, $lidid, $tlm, $nrid);
			$mess = "";
		}
		return $mess;
		
	} elseif ($actie == "del" or $actie == "delete") {
		
		if ($lidid > 0) {
			$query = sprintf("DELETE FROM %sMailing_rcpt WHERE MailingID=%d AND LidID=%d;", TABLE_PREFIX, $mid, $lidid);
			$r = (new cls_Lid())->Naam($lidid);
		} elseif (strlen($email) > 0) {
			$query = sprintf("DELETE FROM %sMailing_rcpt WHERE MailingID=%d AND LOWER(to_address)='%s';", TABLE_PREFIX, $mid, strtolower($email));
			$r = $email;
		}
		if (fnQuery($query) > 0) {
			$mess = sprintf("%s is bij mailing %d verwijderd.", $r, $mid);
		} else {
			$mess = sprintf("Foutmelding: %s is niet bij mailing %d verwijderd.", $r, $mid);
		}
		
		db_logboek("add", $mess, 4, $lidid, 0, $mid);
		
	} else {
		$mess = sprintf("Actie '%s' is onbekend.", $actie);
		db_logboek("add", $mess, 99);
	}
		
} # db_mailing_rcpt

class cls_Mailing_rcpt extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Mailing_rcpt";
		$this->ta = 4;
	}
	
	public function add($p_mid, $p_lidid, $p_email="") {
		$this->lidid = $p_lidid;
		$nrid = 0;
		
		if ($p_lidid > 0) {
			$query = sprintf("SELECT COUNT(*) FROM %sMailing_rcpt AS MR WHERE MR.MailingID=%d AND MR.LidID=%d;", TABLE_PREFIX, $p_mid, $p_lidid);
		} elseif (isValidMailAddress($p_email)) {
			$query = sprintf("SELECT COUNT(*) FROM %sMailing_rcpt WHERE MailingID=%d AND LOWER(to_address) LIKE '%s';", TABLE_PREFIX, $p_mid, strtolower($p_email));
		} else {
			$query = "";
		}
		if (strlen($query) > 0 and db_scalar($query) == 0) {
			$query = "";
			$nrid = db_nieuwrecordID("Mailing_rcpt");
			if ($p_lidid > 0) {
				$rl = db_lid("record", "", $p_lidid);
				$r = $rl->NaamLid;
				if (isValidMailAddress($rl->Email) or isValidMailAddress($rl->EmailOuders) or isValidMailAddress($rl->EmailVereniging)) {
					$query = sprintf("INSERT INTO %sMailing_rcpt (RecordID, MailingID, LidID) VALUES (%d, %d, %d);", TABLE_PREFIX, $nrid, $p_mid, $p_lidid);
				} else {
					$ths->mess = sprintf("%s is niet aan mailing %d toegevoegd, omdat er geen geldig emailadres bekend is.", $r, $p_mid);
					$this->tm = 1;
				}
				$rl = null;
			} elseif (isValidMailAddress($p_email)) {
				$query = sprintf("INSERT INTO %sMailing_rcpt (RecordID, MailingID, to_address) VALUES (%d, %d, '%s');", TABLE_PREFIX, $p_nrid, $p_mid, $p_email);
				$r = $email;
			} else {
				$this->mess = sprintf("%s is niet aan mailing %d toegevoegd, omdat dit geen correct emailadres is.", $p_email, $p_mid);
			}
			if (strlen($query) > 5 and fnQuery($query) > 0) {
				$this->mess = sprintf("%s is aan mailing %d toegevoegd.", $r, $p_mid);
				$this->tm = 0;
			}
		}
		$this->Log($nrid);
		return $nrid;
	}
	
	public function delete($p_mid, $p_lidid, $p_email="") {
		$this->lidid = $p_lidid;
		
		if ($p_lidid > 0) {
			$query = sprintf("DELETE FROM %s WHERE MailingID=%d AND LidID=%d;", $this->table, $p_mid, $p_lidid);
			$r = (new cls_Lid())->Naam($this->lidid);
		} elseif (strlen($p_email) > 0) {
			$query = sprintf("DELETE FROM %s WHERE MailingID=%d AND LOWER(to_address)='%s';", $this->table, $p_mid, strtolower($p_email));
			$r = $email;
		}
		$rv = fnQuery($query);
		if ($rv > 0) {
			$this->mess = sprintf("%s is bij mailing %d verwijderd.", $r, $p_mid);
			$this->Log($p_mid);
		}
		
		return $rv;
	}
	
	public function delete_all($p_mid) {
		
		$query = sprintf("DELETE FROM %s WHERE MailingID=%d;", $this->table, $p_mid);
		$rv = $this->execsql($query);
		if ($rv > 0) {
			$this->mess = sprintf("Alle %d ontvangers zijn bij mailing %d verwijderd.", $rv, $p_mid);
			$this->Log($p_mid);
		}
		
		return $rv;
	}
	
}  # cls_Mailing_rcpt

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
		if ($result > 0) {
			$mess = sprintf("Record aan tabel Mailing_vanaf met RecordID %d toegevoegd.", $result);
			db_logboek("add", $mess, 4, 0, 1, $result);
		}
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
		} elseif ((new cls_Kostenplaats())->aantal() > 0) {
			$sel_kpl = ", KPL.Kode AS Kostenplaats";
		}

		$query = sprintf("SELECT BJ.Kode AS Jaar, CONCAT(GBR.Kode, ' - ', GBR.OMSCHRIJV) AS Grootboekrekening%1\$s, Datum AS dteDatum, M.OMSCHRIJV AS Omschrijving, Debet-Credit AS curBedrag
							FROM ((%2\$sMutatie AS M LEFT OUTER JOIN %2\$sKostenplaats AS KPL ON M.KostenplaatsID = KPL.RecordID) INNER JOIN %2\$sGBR AS GBR ON M.GBR = GBR.Kode) INNER JOIN %2\$sBoekjaar AS BJ ON M.BoekjaarID=BJ.RecordID WHERE %3\$s
							ORDER BY BJ.Kode, GBR.Kode, KPL.Kode, Datum;", $sel_kpl, TABLE_PREFIX, $where);
		$result = fnQuery($query);
		return $result->fetchAll();
	} else {
		sprintf("<p class='mededeling'>Actie '%s' bestaat niet.</p>", $actie);
	}
}  # db_mutatie

function db_boekjaar() {
	
	$query = sprintf("SELECT BJ.RecordID, BJ.Kode AS Jaar FROM %sBoekjaar AS BJ ORDER BY BJ.Begindatum DESC;", TABLE_PREFIX);
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

class cls_Kostenplaats extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Kostenplaats";
	}
	
}  # cls_Kostenplaats

function db_logboek($actie, $oms="", $type=-1, $lidid=0, $tm=0, $referid=0, $ipfilter="", $rid=0, $p_tabel="") {
	global $selectnaam, $selectzoeknaam;
	
	$p_tabel = str_replace(TABLE_PREFIX, "", $p_tabel);
	
	if ($type >= 0 and strlen($ipfilter) > 5) {
		$w = sprintf("TypeActiviteit=%d AND `IP_adres`='%s'", $type, $ipfilter);
	} elseif ($lidid > 0 and strlen($ipfilter) > 5) {
		$w = sprintf("LidID=%d AND `IP_adres`='%s'", $lidid, $ipfilter);
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
	if (strlen($script) > 90) {
		$script = substr($script, 0, 86) . " ...";
	}
	
	if ($actie == "add" or $actie == "insert") {
		$nrid = db_nieuwrecordID("Admin_activiteit");
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
		} elseif (strlen($oms) == 0) {
			debug("db_logboek: lege omschrijving");
		}
		
		$ua = $_SERVER['HTTP_USER_AGENT'];
		if (strlen($ua) > 125) {
			$ua = substr($ua, 0, 125);
		}
		$query = sprintf("INSERT INTO %sAdmin_activiteit 
				(RecordID, DatumTijd, LidID, IP_adres, USER_AGENT, Omschrijving, ReferID, ReferLidID, TypeActiviteit, Script, Getoond, RefFunction) VALUES 
				(%d, SYSDATE(), %d, '%s', '%s', \"%s\",%d, %d, %d, \"%s\", %d, '%s');",
				TABLE_PREFIX, $nrid, $_SESSION['lidid'], $_SERVER['REMOTE_ADDR'], $ua, $oms, $referid, $lidid, $type, $script, $tm, $f);
		fnQuery($query);
		if ($tm == 1) {
			printf("<p class='mededeling'>%s</p>\n", $oms);
		} elseif ($tm == 2) {
			printf("<script>alert(\"%s\");</script>\n", $oms);
		}
		usleep(5);
		return $nrid;
		
	} elseif ($actie == "opschonen") {
		if (db_param("logboek_bewaartijd") > 0) {
			$query = sprintf("DELETE FROM %sAdmin_activiteit WHERE DatumTijd < DATE_ADD(CURDATE(), INTERVAL -%d MONTH);", TABLE_PREFIX, db_param("logboek_bewaartijd"));
			fnQuery($query, 2);
		}
				
	} elseif ($actie == "debugopschonen") {
		$query = sprintf("DELETE FROM %sAdmin_activiteit WHERE TypeActiviteit=99 AND LidID=%d;", TABLE_PREFIX, $_SESSION['lidid']);
		fnQuery($query, 2);
		
	} elseif ($actie == "lijst" or $actie == "lijstsmal") {
		
		if (in_array($type, array(-1, 1, 4, 5, 6, 7, 10, 12, 16, 19))) {
			$selbetreftlid = sprintf(", IF(A.ReferLidID > 0, (SELECT MAX(%s) FROM %sLid AS L2 WHERE L2.RecordID=A.ReferLidID), '') AS `Betreft lid`", str_replace("L.", "L2.", $selectnaam), TABLE_PREFIX);
		} else {
			$selbetreftlid = "";
		}
		if ($type < 0) {
			$s = ", CONCAT(TypeActiviteit, IF(TypeActiviteitSpecifiek > 0, CONCAT('-', TypeActiviteitSpecifiek), '')) AS `Type`";
			$w .= " AND TypeActiviteit < 98";
		} else {
			$s = "";
		}
		if ($_SESSION['webmaster'] == 1 and $actie !== "lijstsmal") {
			$s .= ", CONCAT(Script, ' / ', RefFunction) AS `Script / Functie`";
		}
		if ($lidid > 0) {
			$w .= sprintf(" AND (LidID=%1\$d OR ReferLidID=%1\$d) AND (TypeActiviteit NOT IN (2, 9, 98, 99))", $lidid);
		}
		if ($actie == "lijstsmal") {
			$w .= " AND DatumTijd >= DATE_SUB(NOW(), INTERVAL 6 MONTH)";
		}
		$s .= sprintf(", %s AS `Ingelogd Lid`", $selectnaam);
		if (strlen($ipfilter) <= 5 and $_SESSION['webmaster'] == 1 and $actie !== "lijstsmal") {
			$s .= ", A.IP_adres";
		}

		$query = sprintf("SELECT A.DatumTijd as `dtsDatum en tijd`%1\$s, Omschrijving%2\$s
								FROM %3\$sAdmin_activiteit AS A LEFT OUTER JOIN %3\$sLid AS L ON A.LidID=L.RecordID
								WHERE %4\$s
								ORDER BY A.RecordID DESC LIMIT 1500;", $selbetreftlid, $s, TABLE_PREFIX, $w);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "lidlijst") {
		$query = sprintf("SELECT DISTINCT LidID, %1\$s AS Naam
								FROM %2\$sAdmin_activiteit AS A INNER JOIN %2\$sLid AS L ON A.LidID=L.RecordID
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
		if ($_SESSION['lidid'] > 0) {
			$lidid = $_SESSION['lidid'];
		}
		$query = sprintf("SELECT IFNULL(MAX(DatumTijd), 'Geen') FROM %sAdmin_activiteit WHERE LidID=%d AND TypeActiviteit=1 AND DatumTijd < DATE_SUB(SYSDATE(), INTERVAL 15 MINUTE) AND Omschrijving LIKE '%%ingelogd%%';", TABLE_PREFIX, $lidid);
		return db_scalar($query);
		
	} else {
		$mess = sprintf("db_Logboek: actie '%s' is onbekend.", $actie);
		debug($mess, 0, 99);
	}
} # db_logboek

class cls_Logboek extends cls_db_base {
	
	private function script() {
		$script = $_SERVER['PHP_SELF'];
		if (strlen($_SERVER['QUERY_STRING']) > 0) {
			$script .= "?" . $_SERVER['QUERY_STRING'];
		}
		return $script;
	}
	
	public function add($p_oms, $p_ta=0, $p_lidid=0, $p_tm=0, $p_referid=0) {
		$this->ta = $p_ta;
		$this->lidid = $p_lidid;
		$this->tm = $p_tm;

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
		
		$p_oms = str_replace("<p>", "", $p_oms);
		$p_oms = str_replace("</p>", "\n", $p_oms);
		$p_oms = str_replace("\"", "'", $p_oms);
		if (strlen($p_oms) > 64000) {
			$p_oms = substr($p_oms, 0, 64000);
		} elseif (strlen($p_oms) == 0) {
			$this->tm = 0;
			debug("db_logboek: lege omschrijving");
		}
		
		$ua = $_SERVER['HTTP_USER_AGENT'];
		if (strlen($ua) > 125) {
			$ua = substr($ua, 0, 125);
		}
		$query = sprintf("INSERT INTO %sAdmin_activiteit 
				(DatumTijd, LidID, IP_adres, USER_AGENT, Omschrijving, ReferID, ReferLidID, TypeActiviteit, Script, Getoond, RefFunction) VALUES 
				(SYSDATE(), %d, '%s', '%s', \"%s\",%d, %d, %d, \"%s\", %d, '%s');",
				TABLE_PREFIX, $_SESSION['lidid'], $_SERVER['REMOTE_ADDR'], $ua, $p_oms, $p_referid, $this->lidid, $this->ta, $this->script(), $this->tm, $f);
		$this->execsql($query);
		if ($this->tm == 1) {
			printf("<p class='mededeling'>%s</p>\n", $p_oms);
		} elseif ($this->tm == 2) {
			printf("<script>alert(\"%s\");</script>\n", $p_oms);
		}
		usleep(15);
	}
	
	public function opschonen() {
		
		if (db_param("logboek_bewaartijd") > 0) {
			$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_ADD(CURDATE(), INTERVAL -%d MONTH);", $this->table, db_param("logboek_bewaartijd"));
			$this->execsql($query, 2);
		}
	}
}  # cls_Logboek

function db_interface($actie="lijst", $query="", $lidid=0, $nip=0) {
	global $selectnaam;
	
	$maxrecordsperkeer = 99;
	
	if ($actie == "add" and strlen($query) > 1) {
		$msa_sql = str_replace(TABLE_PREFIX, "", $query);
		$msa_sql = str_replace("\"", "'", $msa_sql);
		$msa_sql = str_replace("SYSDATE()", "#" . strftime("%m/%d/%Y %H:%M:%S") . "#", $msa_sql);
		$msa_sql = str_replace("Now()", "#" . strftime("%m/%d/%Y %H:%M:%S") . "#", $msa_sql);
		$msa_sql = str_replace("CURDATE()", "#" . strftime("%m/%d/%Y") . "#", $msa_sql);
		$msa_sql = str_replace(" BINARY", "", $msa_sql);
		$msa_sql = str_replace("IFNULL(", "Nz(", $msa_sql);
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
		(new cls_Logboek())->add($mess, 8);
		return $mess;
		
	} elseif ($actie == "aantalopenstaand") {
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_interface WHERE (Afgemeld < '2010-01-01' OR (Afgemeld IS NULL));", TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchColumn();
		
	} elseif ($actie == "lijst") {
		$query = sprintf("SELECT %1\$s AS `Naam lid`, I.Ingevoerd, `SQL-statement` AS `SQL`
						FROM %2\$sAdmin_interface AS I LEFT OUTER JOIN %2\$sLid AS L ON I.LidID=L.RecordID
						WHERE (I.Afgemeld IS NULL)
						ORDER BY I.Ingevoerd, I.RecordID LIMIT %3\$d;", $selectnaam, TABLE_PREFIX, $maxrecordsperkeer);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} else {
		$mess = sprintf("db_interface: actie '%s' is onbekend.", $actie);
		db_logboek("add", $mess, 99);
	}
} # db_interface

class cls_interface extends cls_db_base {
	
	public $maxrecords = 99;
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Admin_interface";
		$this->ta = 8;
	}
	
	public function add ($query, $lidid=0, $nip=0) {
		$msa_sql = str_replace(TABLE_PREFIX, "", $query);
		$msa_sql = str_replace("\"", "'", $msa_sql);
		$msa_sql = str_replace("SYSDATE()", "#" . strftime("%m/%d/%Y %H:%M:%S") . "#", $msa_sql);
		$msa_sql = str_replace("CURDATE()", "#" . strftime("%m/%d/%Y") . "#", $msa_sql);
		$msa_sql = str_replace(" BINARY", "", $msa_sql);
		$msa_sql = str_replace("IFNULL(", "Nz(", $msa_sql);
		if ($nip == 1) {
			$ip = "";
		} else {
			$ip = $_SERVER['REMOTE_ADDR'];
		}
		$query = sprintf("INSERT INTO %s (`IP-adres`, LidID, `SQL-statement`, IngelogdLid, Ingevoerd) VALUES ('%s', %d, \"%s\", %d, SYSDATE());", $this->table, $ip, $lidid, $msa_sql, $_SESSION['lidid']);
		$this->execsql($query);
	}
	
	public function afmelden() {
		
		$query = sprintf("UPDATE %s SET Afgemeld=SYSDATE() WHERE IFNULL(Afgemeld, '1900-01-01') < '2011-01-01' ORDER BY Ingevoerd, RecordID LIMIT %d;", $this->table, $this->maxrecords);
		$result = $this->execsql($query);
		if ($result > 0) {
			$this->mess = sprintf("Er zijn %d wijzigingen uit de interface afgemeld.", $result);
			$this->tm = 1;
			$this->Log();
		}
	}
	
}  # cls_interface

function db_diploma($actie, $did=0, $td="*", $lidid=0) {
	
	$mess = "";
	$xtra_order = "";
	if ($lidid > 0) {
		$xtra_order = sprintf(" (SELECT COUNT(*) FROM %sLiddipl AS LD WHERE LD.Lid=%d AND LD.DiplomaID=DP.RecordID AND IFNULL(LD.LicentieVervallenPer, CURDATE()) >= CURDATE()) DESC, ", TABLE_PREFIX, $lidid);
	}
	if ($actie == "muteerlijst") {
		if ($td != "*" and strlen($td) > 0) {
			$filter = sprintf(" AND DP.`Type`='%s'", $td);
		} else {
			$filter = "";
		}
		$query = sprintf("SELECT * FROM %sDiploma AS DP
					WHERE ((DP.Vervallen IS NULL) OR DP.Vervallen > CURDATE()) %s
					ORDER BY%s DP.Volgnr, DP.Kode;", TABLE_PREFIX, $filter, $xtra_order);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "zelfservice_lijst") {
		$query = sprintf("SELECT * FROM %sDiploma AS DP
					WHERE DP.Zelfservice=1 AND DP.AantalBeoordelingen < 2 AND ((DP.Vervallen IS NULL) OR DP.Vervallen > CURDATE())
					ORDER BY%s DP.Volgnr, DP.Kode;", TABLE_PREFIX, $xtra_order);
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

function db_liddipl($actie, $lidid=0, $ldid=0, $dpid=0, $exdat="", $vervdat="", $dipnr="", $xtrafilter="", $kolomnaam="", $waarde="") {
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

	if ($actie == "add" and $lidid > 0 and $dpid > 0) {
		
		$nrid = db_nieuwrecordID("Liddipl");
		$tm = 0;
		if (strlen($dwexd) < 4) {
			$mess = sprintf("'Behaald op' mag niet leeg zijn. Diploma '%s' is niet toegevoegd.", db_diploma("naam", $dpid));
			$tm = 1;
		} elseif ($dwexd > date("Y-m-d")) {
			$mess = sprintf("'Behaald op' mag niet in de toekomst liggen. Diploma '%s' is niet toegevoegd.", db_diploma("naam", $dpid));
			$tm = 1;
		} else {
			$query = sprintf("INSERT INTO %sLiddipl (RecordID, Lid, DiplomaID, DatumBehaald, LaatsteBeoordeling, Ingevoerd, Gewijzigd)
									VALUES (%d, %d, %d, '%s', 1, CURDATE(), SYSDATE());", TABLE_PREFIX, $nrid, $lidid, $dpid, $dwexd);
			$result = fnQuery($query);
			if ($result > 0) {
				$query = sprintf("INSERT INTO Liddipl (RecordID, Lid, DiplomaID, DatumBehaald, Ingevoerd, Gewijzigd) VALUES (%d, %d, %s, CURDATE(), Now());", $nrid, $lidid, $dpid, $dwaexd);
				db_interface("add", $query, $lidid);
				$mess = sprintf("Diploma '%s', behaald op '%s', is toegevoegd.", db_diploma("naam", $dpid), $dwexd);
			} else {
				$mess = sprintf("Diploma '%s' is niet toegevoegd.", db_diploma("naam", $dpid));
				$tm = 1;
				$nrid = 0;
			}
		}
		db_logboek("add", $mess, 12, $lidid, $tm, $nrid);
		return $nrid;
		
	} elseif ($actie == "update") {
		
		$tm = 1;
		$mess = "";
		if ($kolomnaam == "DatumBehaald" and strlen($waarde) < 4) {
			$mess = "'Behaald op' mag niet leeg zijn, deze wijziging wordt niet verwerkt.";
		} elseif ($kolomnaam == "DatumBehaald" and $waarde > date("Y-m-d")) {
			$mess = "'Behaald op' mag niet in de toekomst liggen, deze wijziging wordt niet verwerkt.";
		} else {
			if ($kolomnaam == "LicentieVervallenPer" and (strlen($waarde < 4))) {
				$waarde = "NULL";
				$xw = sprintf("(`%s` IS NOT NULL)", $kolomnaam);
			} else {
				$waarde = "'" . $waarde . "'";
				$xw = sprintf("IFNULL(`%s`, '')<>%s", $kolomnaam, $waarde);
			}
			$query = sprintf("UPDATE %sLiddipl SET `%s`=%s, Gewijzigd=SYSDATE() WHERE RecordID=%d AND %s;", TABLE_PREFIX, $kolomnaam, $waarde, $ldid, $xw);
			if (fnQuery($query) > 0) {
				$mess = sprintf("%s is bij diploma '%s' in %s gewijzigd", $kolomnaam, db_diploma("naam", $dpid), $waarde);
				$tm= 0;
				
				if (strlen($waarde) > 5 and ($kolomnaam == "DatumBehaald" or $kolomnaam == "LicentieVervallenPer")) {
					$waarde = "#" . substr($waarde, 1, -1) . "#";
				}
				$query = sprintf("UPDATE Liddipl SET [%s]=%s, Gewijzigd=Now() WHERE RecordID=%d;", $kolomnaam, $waarde, $ldid);
				db_interface("add", $query, $lidid);
			}
		}
		if (strlen($mess) > 0) {
			db_logboek("add", $mess, 12, $lidid, $tm, $ldid);
		}
		
	} elseif ($actie == "vervallen_delete" and $lidid > 0 and $dpid > 0 and strlen($dwexd) == 10) {
		$query = sprintf("DELETE FROM %sLiddipl WHERE Lid=%d AND DiplomaID=%d AND DatumBehaald='%s';", TABLE_PREFIX, $lidid, $dpid, $dwexd);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("%s is verwijderd.", db_diploma("naam", $dpid));
			
			$query = sprintf("DELETE FROM Liddipl WHERE Lid=%d AND DiplomaID=%d AND DatumBehaald=%s;", $lidid, $did, $dwaexd);
			db_interface("add", $query, $lidid);
		} else {
			$mess = sprintf("%s is niet gevonden en dus niet verwijderd.", db_diploma("naam", $dpid));
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
		
	} elseif ($actie == "diplomabehaald") {
		$query = sprintf("SELECT * FROM %sLiddipl WHERE Lid=%d AND DiplomaID=%d ORDER BY DatumBehaald DESC;", TABLE_PREFIX, $lidid, $dpid);
		$result = fnQuery($query);
		return $result->fetch();
		
	} elseif ($actie == "diplomalid") {
		$query = sprintf("SELECT * FROM %sLiddipl WHERE Lid=%d AND DiplomaID=%d ORDER BY DatumBehaald;", TABLE_PREFIX, $lidid, $dpid);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "opschonen") {
		$query = sprintf("DELETE FROM %1\$sLiddipl WHERE DiplomaID NOT IN (SELECT RecordID FROM %1\$sDiploma);", TABLE_PREFIX);
		fnQuery($query, 2);
		
		$query = sprintf("DELETE FROM %sLiddipl WHERE DatumBehaald > CURDATE();", TABLE_PREFIX);
		fnQuery($query, 2);
	}
} # db_liddipl

class cls_liddipl extends cls_db_base {
	
	private $ldid = 0;
	private $dpnaam = "";
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Liddipl";
		$this->ta = 12;
	}
	
	private function vulvars($p_ldid=0) {
		if ($p_ldid > 0) {
			$this->ldid = $p_ldid;
		}
		if ($this->ldid > 0) {
			$query = sprintf("SELECT LD.Lid, DP.Naam FROM %s AS LD INNER JOIN %sDiploma AS DP on DP.RecordID=LD.DiplomaID WHERE LD.RecodID=%d;", $this->table, TABLE_PREFIX, $this->ldid);
			$row = $this->execsql($query)->fetch();
			$this->lidid = $row->Lid;
			$this->dpnaam = $row->Naam;
		}
	}
	
	public function update($p_ldid, $p_kolom, $p_waarde) {
		$this->valvars($p_ldid);
		$this->tm = 1;
		if ($p_kolom == "DatumBehaald" and strlen($p_waarde) < 4) {
			$this->mess = "'Behaald op' mag niet leeg zijn, deze wijziging wordt niet verwerkt.";
		} elseif ($p_kolom == "DatumBehaald" and $p_waarde > date("Y-m-d")) {
			$this->mess = "'Behaald op' mag niet in de toekomst liggen, deze wijziging wordt niet verwerkt.";
		} else {
			
			if ($p_kolom == "LicentieVervallenPer" and strlen($p_waarde < 5)) {
				$p_waarde = "NULL";
				$xw = sprintf("(`%s` IS NOT NULL)", $p_kolom);
			} elseif ($this->is_kolom_numeriek($p_kolom)) {
				$p_waarde = intval($p_waarde);
				$xw = sprintf("IFNULL(`%s`, 0)<>%d", $p_kolom, $p_waarde);
			} else {
				$p_waarde = "'" . $p_waarde . "'";
				$xw = sprintf("IFNULL(`%s`, '')<>%s", $p_kolom, $p_waarde);
			}
			$query = sprintf("UPDATE %s SET `%s`=%s, Gewijzigd=SYSDATE() WHERE RecordID=%d AND %s;", $thia->table, $p_kolom, $p_waarde, $p_ldid, $xw);
			if (fnQuery($query) > 0) {
				$this->mess = sprintf("%s is bij diploma '%s' in %s gewijzigd", $p_kolom, $dpnaam, $p_waarde);
				$this->tm = 0;
				
				if (strlen($p_waarde) > 5 and ($p_kolom == "DatumBehaald" or $p_kolom == "LicentieVervallenPer")) {
					$p_waarde = "#" . substr($p_waarde, 1, -1) . "#";
				}
				$query = sprintf("UPDATE Liddipl SET [%s]=%s, Gewijzigd=Now() WHERE RecordID=%d;", $p_kolom, $p_waarde, $p_ldid);
				db_interface("add", $query, $this->lidid);
			}
		}
		$this->tas = 2;
		$this->Log($p_ldid);
	}
	
}  #  cls_Liddipl

function db_evenement($actie, $lidid=0, $eventid=0, $opm="", $stat="", $functie="") {
	global $selectnaam, $selectzoeknaam;
	
	$sqlaantdln = sprintf("SELECT SUM(Aantal) FROM %sEvenement_Deelnemer AS ED WHERE ED.EvenementID = E.RecordID AND Status='B'", TABLE_PREFIX);
	$sqlaantafgemeld = sprintf("SELECT COUNT(*) FROM %sEvenement_Deelnemer AS ED WHERE ED.EvenementID = E.RecordID AND Status='X'", TABLE_PREFIX);

	$xtra_where = "";
	$mess = "";
	if ($actie == "aantal") {
		$query = sprintf("SELECT COUNT(*) FROM %sEvenement;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "oms") {
		$query = sprintf("SELECT Omschrijving FROM %sEvenement WHERE RecordID=%d;", TABLE_PREFIX, $eventid);
		return db_scalar($query);
		
	} elseif ($actie == "komende") {
		$query = sprintf("SELECT E.Datum, E.Omschrijving
								FROM %sEvenement AS E
								WHERE E.Datum >= CURDATE() AND E.Datum < DATE_ADD(CURDATE(), INTERVAL 2 MONTH) AND E.VerwijderdOp < '2012-01-01' AND E.ZichtbaarVoor IN (%s)
								ORDER BY E.Datum LIMIT 5;", TABLE_PREFIX, $_SESSION["lidgroepen"]);
		$result = fnQuery($query);
		return $result->fetchAll();

	} elseif ($actie == "open") {
		$query = sprintf("SELECT E.*, ET.Omschrijving AS TypeEvenement
								FROM %1\$sEvenement AS E INNER JOIN %1\$sEvenement_Type AS ET ON E.TypeEvenement = ET.RecordID
								WHERE E.Datum >= SYSDATE() AND E.VerwijderdOp < '2012-01-01' AND InschrijvingOpen=1
								AND E.BeperkTotGroep IN (%2\$s)
								ORDER BY E.Datum;", TABLE_PREFIX, $_SESSION["lidgroepen"]);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "overzicht" or $actie == "overzichtbeheer") {
		if ($actie == "overzichtbeheer") {
			$select = "E.RecordID AS lnkNummer, E.Datum AS dteDatum, DATE_FORMAT(E.Datum, '%H:%i') AS Starttijd, E.Omschrijving, E.Locatie, E.Email AS `emlEmail`, E.Eindtijd, ET.Soort AS ndSoort,
					   CASE E.InschrijvingOpen WHEN 0 THEN 'Nee' ELSE 'Ja' END AS `Ins. open?`, ET.Omschrijving AS `Type`";
			$where = "E.VerwijderdOp < '2012-01-01'";
		} else {
			$select = sprintf("E.RecordID AS lnkNummer, E.Datum AS dteDatum, DATE_FORMAT(E.Datum, '%%H:%%i') AS Starttijd, E.Omschrijving, E.Locatie,
								E.Email AS `emlEmail`, E.Verzameltijd, E.Eindtijd, ET.Soort AS ndSoort,
								CASE E.InschrijvingOpen WHEN 0 THEN 'Nee' ELSE 'Ja' END AS `Ins. open?`,
								(%s) AS `Aantal dln`,
								ET.Omschrijving AS `Type`", $sqlaantdln);
			$where = sprintf("E.Datum >= DATE_ADD(CURDATE(), INTERVAL -3 DAY) AND E.VerwijderdOp < '2012-01-01' AND E.ZichtbaarVoor IN (%s)", $_SESSION["lidgroepen"]);
		}
		$query = sprintf("SELECT %1\$s
								FROM %2\$sEvenement AS E LEFT OUTER JOIN %2\$sEvenement_Type AS ET ON E.TypeEvenement=ET.RecordID
								WHERE %3\$s
								ORDER BY E.Datum DESC;", $select, TABLE_PREFIX, $where);
		$result = fnQuery($query);
		return $result->fetchAll();
		
	} elseif ($actie == "add" or $actie == "insert") {
		$query = sprintf("INSERT INTO %sEvenement (RecordID, InschrijvingOpen, IngevoerdDoor) VALUES (%d, 0, %d);", TABLE_PREFIX, db_nieuwrecordID("Evenement"), $_SESSION['lidid']);
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
								FROM %4\$sEvenement AS E LEFT OUTER JOIN %4\$sLid AS L ON E.GewijzigdDoor = L.RecordID
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
		$query = sprintf("SELECT ED.RecordID, %1\$s AS NaamDeelnemer, ED.Status, ED.Opmerking, ED.Functie, ED.LidID, ED.Aantal, L.Geslacht, ED.Ingevoerd, ED.LidID, ED.IngevoerdDoor
							FROM (%2\$sEvenement_Deelnemer AS ED INNER JOIN %2\$sLid AS L ON ED.LidID = L.RecordID) INNER JOIN %2\$sEvenement AS E ON E.RecordID=ED.EvenementID
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
		$query = sprintf("SELECT SUM(Aantal) FROM %sEvenement_Deelnemer AS ED %s;", TABLE_PREFIX, $where);
		return db_scalar($query);
		
	} elseif ($actie == "potdeelnemers") {
		$where = "";
		$query = sprintf("SELECT BeperkTotGroep FROM %sEvenement WHERE RecordID=%d;", TABLE_PREFIX, $eventid);
		$groepid = db_scalar($query);
		if ($groepid > 0) {
			$where = sprintf("AND LO.OnderdeelID=%d", $groepid);
		}
		$query = sprintf("SELECT DISTINCT L.RecordID AS LidID, %1\$s AS Naam
								FROM %2\$sLid AS L LEFT OUTER JOIN %2\$sLidond AS LO ON L.RecordID=LO.Lid
								WHERE (SELECT COUNT(*) FROM %2\$sEvenement_Deelnemer AS ED WHERE L.RecordID=ED.LidID AND ED.EvenementID=%3\$d)=0 %4\$s
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

class cls_Evenement extends cls_db_base {
	
	function  __construct() {
		$this->table = TABLE_PREFIX . "Evenement";
		$this->ta = 7;
	}
	
	public function standaardstatus($p_evid) {
		$query = sprintf("SELECT StandaardStatus FROM %sEvenement WHERE RecordID=%d;", TABLE_PREFIX, $p_evid);
		return db_scalar($query);
	}
	
	public function add() {
		$nrid = db_nieuwrecordID("Evenement");
		$query = sprintf("INSERT INTO %s (RecordID, InschrijvingOpen, StandaardStatus, IngevoerdDoor) VALUES (%d, 0, 'I', %d);", $this->table, $nrid, $_SESSION['lidid']);
		if (fnQuery($query) > 0) {
			$this->mess = sprintf("Evenement %d is toegevoegd.", $nrid);
			$this-Log($nrid);
			return $nrid;
		} else {
			return 0;
		}
	}
	
	public function update($p_evid, $p_kolom, $p_waarde) {
		
		if ($this->is_kolom_numeriek($p_kolom)) {
			$p_waarde = intval($p_waarde);
			$xw = sprintf("IFNULL(`%s`, 0)<>%d", $p_kolom, $p_waarde);
		} else {
			$p_waarde = "\"" . str_replace("\"", "'", $p_waarde) . "\"";
			$xw = sprintf("IFNULL(`%s`, '')<>%s", $p_kolom, $p_waarde);
		}
		
		$query = sprintf("UPDATE %sEvenement SET `%s`=%s, Gewijzigd=SYSDATE(), GewijzigdDoor=%d WHERE RecordID=%d AND %s;", TABLE_PREFIX, $p_kolom, $p_waarde, $_SESSION['lidid'], $p_evid, $xw);
		if (fnQuery($query) > 0) {
			$this->mess = sprintf("In evenement %d is kolom '%s' in %s gewijzigd.", $p_evid, $p_kolom, $p_waarde);
			$this->Log($p_evid);
		}
	}
	
}  # cls_Evenement

function db_Evenement_Deelnemer($actie, $recid=0, $fld="", $val="", $lidid=0, $eventrow="") {
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
		$query = sprintf("INSERT INTO %sEvenement_Deelnemer (RecordID, LidID, EvenementID, Status, Opmerking, Functie, IngevoerdDoor) VALUES (%d, %d, %d, '%s', '', '', %d);", TABLE_PREFIX, db_nieuwrecordID("Evenement_Deelnemer"), $lidid, $eventrow->RecordID, $eventrow->StandaardStatus, $_SESSION['lidid']);
		$recid = fnQuery($query);
		if ($recid > 0) {
			$mess = sprintf("Is bij evenement %d (%s) met status '%s' toegevoegd.", $eventrow->RecordID, $eventrow->Omschrijving, $eventrow->StandaardStatus);
		}
		
	} elseif ($actie == "update") {
		$edrow = db_Evenement_Deelnemer("record", $recid);
		$lidid = $edrow->LidID;
		$oldval = $edrow->$fld;
		if ($oldval != $val) {
			$query = sprintf("UPDATE %1\$sEvenement_Deelnemer SET `%2\$s`='%3\$s', GewijzigdDoor=%4\$d WHERE RecordID=%5\$d;", TABLE_PREFIX, $fld, $val, $_SESSION['lidid'], $recid);
			if ($fnQuery($query) > 0) {
				$mess = sprintf("Bij evenement %d (%s) is kolom '%s' van '%s' naar '%s' bijgewerkt.", $edrow->EvenementID, $edrow->OmsEvenement, $fld, $oldval, $val);
			}
		}
		
	} elseif ($actie == "delete") {
		$edrow = db_Evenement_Deelnemer("record", $recid);
		$lidid = $edrow->LidID;
		$query = sprintf("DELETE FROM %sEvenement_Deelnemer WHERE RecordID=%d;", TABLE_PREFIX, $recid);
		if (fnQuery($query) > 0) {
			$mess = sprintf("Is bij evenement %d (%s) verwijderd.", $edrow->EvenementID, $edrow->OmsEvenement);
		}
		
	} else {
		$mess = sprintf("db_Evenement_Deelnemer: actie '%s' is onbekend.", $actie);
		$lidid = 0;
	}
	if (strlen($mess) > 0) {
		db_logboek("add", $mess, 7, $lidid, 0, $recid);
	}
	
	return $rv;
	
}  #  db_Evenement_Deelnemer

class cls_Evenement_Deelnemer extends cls_db_base {
	
	private $edid = 0;
	private $evid = 0;
	private $omsEvenement = "";
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Evenement_Deelnemer";
		$this->ta = 7;
	}
	
	private function vulvars() {
		if ($this->edid > 0) {
			$query = sprintf("SELECT E.Omschrijving AS OE, ED.LidID, ED.EvenementID FROM %1\$sEvenement_Deelnemer AS ED INNER JOIN %1\$sEvenement AS E ON E.RecordID=ED.EvenementID WHERE ED.RecordID=%2\$d;", TABLE_PREFIX, $this->edid);
			$row = fnQuery($query)->fetch();
			$this->evid = $row->EvenementID;
			$this->lidid = $row->LidID;
			$this->OmsEvenement = $row->OE;
		} elseif ($this->evid > 0) {
			$query = sprintf("SELECT IFNULL(MAX(E.Omschrijving), '') FROM %sEvenement AS E WHERE E.RecordID=%d;", TABLE_PREFIX, $this->evid);
			$this->OmsEvenement = db_scalar($query);
		}
	}
	
	public function add($p_evid, $p_lidid, $p_status="I") {
		$this->lidid = $p_lidid;
		$this->evid = $p_evid;
		$this->vulvars();
		
		$nrid = db_nieuwrecordID($this->table);
		
		$query = sprintf("INSERT INTO %sEvenement_Deelnemer (RecordID, LidID, EvenementID, Status, Opmerking, Functie, IngevoerdDoor) VALUES (%d, %d, %d, '%s', '', '', %d);", TABLE_PREFIX, $nrid, $p_lidid, $p_evid, $p_status, $_SESSION['lidid']);
		if (fnQuery($query) > 0) {
			$this->mess = sprintf("Is bij evenement %d (%s) met status '%s' toegevoegd.", $p_evid, $this->OmsEvenement, $p_status);
			$this->Log($nrid);
			return $nrid;
		}
	}
	
	public function update($p_edid, $p_kolom, $p_waarde) {
		$this->edid = $p_edid;
		$this->vulvars();
		
		if ($this->is_kolom_numeriek($p_kolom)) {
			$p_waarde = intval($p_waarde);
			$xw = sprintf("IFNULL(`%s`, 0)<>%d", $p_kolom, $p_waarde);
		} else {
			$p_waarde = "'" . $p_waarde . "'";
			$xw = sprintf("IFNULL(`%s`, '')<>%s", $p_kolom, $p_waarde);
		}
		
		$query = sprintf("UPDATE %sEvenement_Deelnemer SET `%s`=%s, Gewijzigd=SYSDATE(), GewijzigdDoor=%d WHERE RecordID=%d AND %s;", TABLE_PREFIX, $p_kolom, $p_waarde, $_SESSION['lidid'], $p_edid, $xw);
		if (fnQuery($query) > 0) {
			$this->mess = sprintf("Bij deelname aan evenement %d (%s) is kolom '%s' in %s gewijzigd.", $this->evid, $this->OmsEvenement, $p_kolom, $p_waarde);
			$this->Log($p_edid);
		}
	}
	
	public function delete($p_edid) {
		$this->edid = $p_edid;
		$this->vulvars();
		
		$query = sprintf("DELETE FROM %sEvenement_Deelnemer WHERE RecordID=%d;", TABLE_PREFIX, $p_edid);
		if (fnQuery($query) > 0) {
			$mess = sprintf("Is bij evenement %d (%s) verwijderd.", $this->evid, $this->OmsEvenement);
			$this-Log($p_edid);
		}
	}
	
}  #  cls_Evenement_Deelnemer

function db_Evenement_Type($actie, $etid=0, $colname="", $newval="") {
	if ($actie == "aantal") {
		$query = sprintf("SELECT COUNT(*) FROM %sEvenement_Type;", TABLE_PREFIX);
		return db_scalar($query);
	} elseif ($actie == "insert") {
		$query = sprintf("INSERT INTO %sEvenement_Type (RecordID, Omschrijving) VALUES (%d, '%s');", TABLE_PREFIX, db_nieuwrecordID("Evenement_Type"), $newval);
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

class cls_Evenement_Type extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Evenement_Type";
		$this->ta = 7;
	}
	
	public function add($p_waarde) {
		$nrid = db_nieuwrecordID($this->table);
		
		$query = sprintf("INSERT INTO %s (RecordID, Omschrijving) VALUES (%d, '%s');", $this->table, $nrid, $p_waarde);
		if (fnQuery($query) > 0) {
			$this->mess = sprintf("Evenement type %d (%s) is toegevoegd.", $nrid, $p_waarde);
			$this->Log($nrid);
			return $nrid;
		} else {
			return 0;
		}
	}
	
	public function update($p_etid, $p_kolom, $p_waarde) {
		
		if ($this->is_kolom_numeriek($p_kolom)) {
			$p_waarde = intval($p_waarde);
			$xw = sprintf("IFNULL(`%s`, 0)<>%d", $p_kolom, $p_waarde);
		} else {
			$p_waarde = "'" . $p_waarde . "'";
			$xw = sprintf("IFNULL(`%s`, '')<>%s", $p_kolom, $p_waarde);
		}
		
		$query = sprintf("UPDATE %s SET `%s`=%s, Gewijzigd=SYSDATE(), GewijzigdDoor=%d WHERE RecordID=%d AND %s;", $this->table, $p_kolom, $p_waarde, $_SESSION['lidid'], $p_etid, $xw);
		if (fnQuery($query) > 0) {
			$this->mess = sprintf("Kolom '%s' in Evenement type %d is in %s gewijzigd.", $p_kolom, $p_etid, $p_waarde);
			$this->Log($p_etid);
		}
	}
	
	public function delete($p_etid) {
		
		$query = sprintf("SELECT COUNT(*) FROM %sEvenement AS E WHERE E.TypeEvenement=%d;", TABLE_PREFIX, $p_etid);
		if (db_scalar($query) > 0) {
			$this->mess = "Dit type is nog in gebruik en mag niet worden verwijderd.";
		} else {
			$query = sprintf("DELETE FROM %s WHERE RecordID=%d;", $this->table, $p_etid);
			if (fnQuery($query) > 0) {
				$this->mess = sprintf("Evenement type %d is verwijderd.", $p_etid);
			}
		}
		$this->tm = 1;
		$this->Log($p_etid);
	}
	
}  # cls_Evenement_Type

function db_artikel($actie, $artikelid=0, $lidid=0) {

	$mess = "";
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
					WHERE BeschikbaarTot >= CURDATE() AND VervallenPer > CURDATE() AND (BeperkTotGroep IN (%s))
					ORDER BY Code, Omschrijving, RecordID;", TABLE_PREFIX, $_SESSION["lidgroepen"]);
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

class cls_Artikel extends cls_db_base{
	
	function __construct() {
		$this->table = TABLE_PREFIX . "WS_Artikel";
		$this->ta = 10;
	}
	
	public function add() {
		$nrid = db_nieuwrecordID("WS_Artikel");
		
		$query = sprintf("INSERT INTO %sWS_Artikel (RecordID, IngevoerdDoor) VALUES  (%d, %d);", TABLE_PREFIX, $nrid, $_SESSION['lidid']);
		if (fnQuery($query) > 0) {
			$this->mess = sprintf("Artikel %d is toegevoegd.", $nrid);
		}
		
		$this->Log($nrid);
	}

	public function update($p_artid, $p_kolom, $p_waarde) {
		
		if ($p_kolom == 'Verkoopprijs') {
			$p_waarde = number_format($p_waarde, 2, ".", "");
			$xw = sprintf("IFNULL(`%s`, 0)<>%s", $p_kolom, $p_waarde);
		} elseif ($this->is_kolom_numeriek($p_kolom)) {
			$p_waarde = intval($p_waarde);
			$xw = sprintf("IFNULL(`%s`, 0)<>%d", $p_kolom, $p_waarde);
		} else {
			$p_waarde = "'" . $p_waarde . "'";
			$xw = sprintf("IFNULL(`%s`, '')<>%s", $p_kolom, $p_waarde);
		}
		
		$query = sprintf("UPDATE %sWS_Artikel SET `%s`=%s, Gewijzigd=SYSDATE(), GewijzigdDoor=%d WHERE RecordID=%d AND %s;", TABLE_PREFIX, $p_kolom, $p_waarde, $_SESSION['lidid'], $p_artid, $xw);
//		debug($query);
		if (fnQuery($query) > 0) {
			$this->mess = sprintf("Kolom '%s' van artikel %d is in %s gewijzigd.", $p_kolom, $p_artid, $p_waarde);
		}
		
		$this->Log($p_artid);
	}
	
}

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
					FROM (%2\$sWS_Orderregel AS ORD INNER JOIN %2\$sWS_Artikel AS A ON A.RecordID=ORD.Artikel) INNER JOIN %2\$sLid AS L ON ORD.Lid=L.RecordID
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
					FROM (%2\$sWS_Orderregel AS ORD LEFT OUTER JOIN %2\$sWS_Artikel AS A ON A.RecordID=ORD.Artikel) INNER JOIN %2\$sLid AS L ON ORD.Lid=L.RecordID
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
				$mess = sprintf("Orderregel %d van %d stuks van artikel '%s' is aan order %d is toegevoegd.", $result, $aantalbesteld, db_artikel("code", $artikelid), $ordernr);
			} else {
				$mess = sprintf("Orderregel %d is aan order %d is toegevoegd.", $result, $ordernr);
			}
			db_logboek("add", $mess, 10, $lidid, 0, $ordernr);
			if ($artikelid > 0 and $aantalbesteld > 0) {
				db_orderregel("vulprijsperstuk");
			}
		}
		return $mess;
		
	} elseif ($actie == "delete") {
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
				$mess = sprintf("De bestelling van artikel %d is verwijderd.", $artikelid);
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

class cls_Orderregel extends cls_db_base {

	function __construct($p_lidid=0) {
		$this->table = TABLE_PREFIX . "WS_Orderregel";
		$this->lidid = $p_lidid;
		$this->ta = 10;
	}
	
	public function add() {
		$nrid = db_nieuwrecordID("WS_Orderregel");
		
		$query = sprintf("INSERT INTO %sWS_Orderregel (RecordID, Lid, Artikel, AantalBesteld, Ordernr, Opmerking, IngevoerdDoor) VALUES (%d, %d, %d, %d, %d, '', %d);", TABLE_PREFIX, $nrid, $lidid, $artikelid, $aantalbesteld, $ordernr, $_SESSION['lidid']);
		$result = fnQuery($query);
		if ($result > 0) {
			if ($artikelid > 0) {
				$mess = sprintf("Orderregel %d van %d stuks van artikel '%s' is aan order %d is toegevoegd.", $result, $aantalbesteld, db_artikel("code", $artikelid), $ordernr);
			} else {
				$mess = sprintf("Orderregel %d is aan order %d is toegevoegd.", $result, $ordernr);
			}
			db_logboek("add", $mess, 10, $lidid, 0, $nrid);
			if ($artikelid > 0 and $aantalbesteld > 0) {
				db_orderregel("vulprijsperstuk");
			}
		}
		$this-Log();
	}

	public function update($p_orid, $p_kolom, $p_waarde) {
		
		if ($p_kolom == 'PrijsPerStuk') {
			$p_waarde = number_format($p_waarde, 2, ".", "");
			$xw = sprintf("IFNULL(`%s`, 0)<>%s", $p_kolom, $p_waarde);
		} elseif ($this->is_kolom_numeriek($p_kolom)) {
			$p_waarde = intval($p_waarde);
			$xw = sprintf("IFNULL(`%s`, 0)<>%d", $p_kolom, $p_waarde);
		} else {
			$p_waarde = "'" . $p_waarde . "'";
			$xw = sprintf("IFNULL(`%s`, '')<>%s", $p_kolom, $p_waarde);
		}
		
		$query = sprintf("UPDATE %s SET `%s`=%s, Gewijzigd=SYSDATE(), GewijzigdDoor=%d WHERE RecordID=%d AND %s;", $this->table, $p_kolom, $p_waarde, $_SESSION['lidid'], $p_orid, $xw);
		if (fnQuery($query) > 0) {
			$this->mess = sprintf("Kolom '%s' van orderregel %d is in %s gewijzigd.", $p_kolom, $p_orid, $p_waarde);
			$this->Log($p_orid);
		}
	}
	
	public function definitiefmaken($p_lidid) {
		$this->lidid = $p_lidid;
		
		$query = sprintf("UPDATE %s SET BestellingDefinitief=SYSDATE(), GewijzigdDoor=%d WHERE Lid=%d AND (BestellingDefinitief IS NULL) AND (NOT PrijsPerStuk IS NULL);", $this->table, $_SESSION['lidid'], $p_lidid);
		if (fnQuery($query) > 0) {
			$this->mess = "Bestelling is definitief gemaakt.";
			$this->Log();
		}
	}

}  # cls_Orderregel

function db_rekening($actie, $nr=0, $filter="", $lidid=0) {
	global $selectlidnr;
	
	if ($actie == "minnummer") {
		$query = sprintf("SELECT MIN(RK.Nummer) FROM %sRekening AS RK;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "maxnummer") {
		$query = sprintf("SELECT MAX(RK.Nummer) FROM %sRekening AS RK;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "mindatum") {
		$query = sprintf("SELECT MIN(RK.Datum) FROM %sRekening AS RK;", TABLE_PREFIX);
		return db_scalar($query);
		
	} elseif ($actie == "maxdatum") {
		$query = sprintf("SELECT MAX(RK.Datum) FROM %sRekening AS RK;", TABLE_PREFIX);
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
					FROM %2\$sRekening AS RK INNER JOIN %2\$sLid AS L ON RK.Lid = L.RecordID
					WHERE RK.Nummer=%3\$d;", $selectlidnr, TABLE_PREFIX, $nr);
		$result = fnQuery($query);
		return $result->fetch();
	} elseif ($actie == "regels") {
		$query = sprintf("SELECT RR.*, L.GEBDATUM, L.EmailOuders FROM %1\$sRekreg AS RR LEFT JOIN %1\$sLid AS L ON RR.Lid=L.RecordID WHERE RR.Rekening=%2\$d;", TABLE_PREFIX, $nr);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "seizoenen") {
		$query = sprintf("SELECT DISTINCT RK.Seizoen FROM %sRekening AS RK ORDER BY RK.Seizoen DESC;", TABLE_PREFIX);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "selectie") {
		if (strlen($filter) > 0 and startwith($filter, "WHERE") == false) {
			$filter = "WHERE " . $filter;
		}
		$query = sprintf("SELECT RK.*, L.Adres, L.Postcode, L.Woonplaats, L.Bankrekening AS Bankrekeningnummer,
					DATE_ADD(RK.Datum, INTERVAL RK.BET_TERM*RK.BETAALDAG DAY) AS UitersteBetaaldatum,
					(%1\$1s) AS Lidnr
					FROM %2\$sRekening AS RK INNER JOIN %2\$sLid AS L ON RK.Lid = L.RecordID
					%3\$s
					ORDER BY RK.Nummer;", $selectlidnr, TABLE_PREFIX, $filter);
		$result = fnQuery($query);
		return $result->fetchAll();
	} elseif ($actie == "histmail" and $nr > 0) {
		$query = sprintf("SELECT message FROM %sMailing_hist WHERE Xtra_Char='REK' AND Xtra_Num=%d ORDER BY send_on DESC LIMIT 1;", TABLE_PREFIX, $nr);
		return db_scalar($query);
	}
} # db_rekening

class cls_Rekening extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Rekening";
	}
	
}  # cls_Rekening

class cls_Rekeningregel extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Rekreg";
	}
	
	function perrekening($p_nummer) {
		$query = sprintf("SELECT RR.*, L.GEBDATUM, L.EmailOuders FROM %1\$sRekreg AS RR LEFT JOIN %1\$sLid AS L ON RR.Lid=L.RecordID WHERE RR.Rekening=%2\$d;", TABLE_PREFIX, $p_nummer);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
}  # cls_Rekeningregel

function db_Stukken($actie, $lidid=0) {
	
	if ($actie == "gewijzigdestukken") {
		$vl = db_logboek("vorigelogin", "", 0, $lidid);
		if (strlen($vl) >= 10) {
			$query = sprintf("SELECT Titel FROM %sStukken WHERE ((VervallenPer IS NULL) OR VervallenPer > CURDATE()) AND GewijzigdOp > '%s' ORDER BY GewijzigdOp DESC;", TABLE_PREFIX, $vl);
		} else {
			$query = sprintf("SELECT Titel FROM %sStukken WHERE ((VervallenPer IS NULL) OR VervallenPer > CURDATE()) ORDER BY GewijzigdOp DESC;", TABLE_PREFIX);
		}
		return fnQuery($query)->fetchAll();
	}
	
}  # db_Stukken

function db_dms() {
}  # db_dms

function db_param($naam, $actie="curval", $newval="") {
	
	if (isset($_SESSION['settings'])) {
		$settings = $_SESSION['settings'];
	}
	
	$arrIntegers['logboek_bewaartijd'] = 6;
	$arrIntegers['login_geldigheidactivatie'] = 36;
	$arrIntegers['db_backup_type'] = 3;
	$arrIntegers['db_backupsopschonen'] = 11;
	$arrIntegers['login_autounlock'] = 120;
	$arrIntegers['mailing_alle_zien'] = -1;
	$arrIntegers['mailing_bevestigingbestelling'] = 0;
	$arrIntegers['mailing_bevestigingopzegging'] = 0;
	$arrIntegers['mailing_bewaartijd'] = 3;
	$arrIntegers['mailing_bewakinginschrijving'] = 0;
	$arrIntegers['mailing_herstellenwachtwoord'] = 0;
	$arrIntegers['mailing_meldingnieuwip'] = 0;
	$arrIntegers['mailing_lidnr'] = 0;
	$arrIntegers['mailing_mailopnieuw'] = 0;
	$arrIntegers['mailing_rekening_stuurnaar'] = 1;
	$arrIntegers['mailing_type_editor'] = 1;
	$arrIntegers['mailing_validatielogin'] = 0;
	$arrIntegers['max_grootte_bijlage'] = 2048;
	$arrIntegers['maxmailsperuur'] = 150;
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
	$arrBooleans['mailing_direct_verzenden'] = 0;
	$arrBooleans['mailing_sentoutbox_auto'] = 1;
	$arrBooleans['mailing_rekening_regelnrsweglaten'] = 0;
	$arrBooleans['mailing_rekening_nulrekversturen'] = 1;
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
	$arrStrings['mailing_rekening_valuta'] = "&euro&nbsp;";
	$arrStrings['menu_met_afdelingen'] = "";
	$arrStrings['path_attachments'] = "";
	$arrStrings['path_templates'] = "";
	$arrStrings['uitleg_toestemmingen'] = "";
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
		
	} elseif ($actie == "updval" or $actie == "update") {
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
				$set = sprintf("ValueChar=\"%s\"", str_replace("\"", "'", $newval));
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

class cls_Parameter extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Admin_param";
		$this->ta = 13;
	}
	
	function update($p_naam, $p_waarde) {
		$query = sprintf("SELECT * FROM %s WHERE Naam='%s';", $this->table, $p_naam);
		$cur = $this->execsql($query)->fetch();
		$set = "";
		if (($cur->ParamType == "B" or $cur->ParamType == "I") and $cur->ValueNum != $p_waarde) {
			$set = sprintf("ValueNum=%d", $p_waarde);
			$xw = sprintf("IFNULL(ValueNum, 0)<>%d", $p_waarde);
		} elseif ($cur->ParamType == "F" and $cur->ValueNum != $p_waarde) {
			$p_waarde = sprintf("%F", $p_waarde);
			$set = sprintf("ValueNum=%s", $p_waarde);
			$xw = sprintf("IFNULL(ValueNum, 0)<>%F", $p_waarde);
		} elseif (($cur->ParamType == "S" or $cur->ParamType == "T") and $cur->ValueChar != $p_waarde) {
			if (strlen($p_waarde) == 0) {
				$set = "ValueChar=NULL";
				$xw = "(ValueChar NOT IS NULL)";
			} else {
				$p_waarde = "\"" . str_replace("\"", "'", $p_waarde) . "\"";
				$set = sprintf("ValueChar=%s", $p_waarde);
				$xw = sprintf("IFNULL(ValueNum, '')<>%s", $p_waarde);
			}
		}
		if (strlen($set) > 0) {
			$query = sprintf("UPDATE %s SET %s, Gewijzigd=SYSDATE(), GewijzigdDoor=%d WHERE RecordID=%d AND %s;", $this->table, $set, $_SESSION['lidid'], $cur->RecordID, $xw);
			if ($this->execsql($query) > 0) {
				$_SESSION['settings'][$p_naam] = $p_waarde;
				if ($p_naam !== "versie") {
					$this->mess = sprintf("Parameter '%s' is in %s gewijzigd.", $p_naam, $p_waarde);
					$this->Log($cur->RecordID);
				}
			}
		}
	}
	
}  # cls_Parameter

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
				fnQuery(sprintf("ALTER TABLE %s%s CHANGE Ingevoerd Ingevoerd DATE;", TABLE_PREFIX, $tn));
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
	$col = "Login";
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE '%s';", $db_name, $table, $col);
	if (db_scalar($query) < db_param("login_maxlengte")) {
		$query = sprintf("ALTER TABLE `%1\$s` CHANGE `%2\$s` `%2\$s` VARCHAR( %3\$d ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table, $col, db_param("login_maxlengte"));
		fnQuery($query, 2);
	}
	
	/***** Aanpassen lengte wachtwoord in de database als deze kleiner is dan 255. Deze lengte is nodig voor de password_hash()  *****/
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE 'Wachtwoord';", $db_name, $table);
	if (db_scalar($query) < 255) {
		$query = sprintf("ALTER TABLE `%s` CHANGE `Wachtwoord` `Wachtwoord` VARCHAR(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table);
		fnQuery($query, 2);
	}
	
	/***** Aanpassen lengte van het IP-adres in Admin_activiteit naar 45 voor IP v6 adressen. Deze code kan na 1/1/2020 worden verwijderd. *****/
	$table = TABLE_PREFIX . "Admin_activiteit";
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE 'IP_adres';", $db_name, $table);
	if (db_scalar($query) < 45) {
		$query = sprintf("ALTER TABLE `%s` CHANGE `IP_adres` `IP_adres` VARCHAR(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table);
		fnQuery($query, 2);
	}
	
	/***** Aanpassen type veld van Ingevoerd in Admin_login naar datumtijd. Deze code kan na 1/3/2021 worden verwijderd. *****/
	$query = sprintf("ALTER TABLE `%sAdmin_login` CHANGE `LastLogin` `LastLogin` DATETIME NULL;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sAdmin_login` CHANGE `LastActivity` `LastActivity` DATETIME NULL;", TABLE_PREFIX);
	fnQuery($query);
	
	/***** Aanpassen lengte van Opmerking in Evenement_Deelnemer naar 255. Deze code kan na 1/1/2021 worden verwijderd. *****/
	$table = TABLE_PREFIX . "Evenement_Deelnemer";
	$col = "Opmerking";
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE '%s';", $db_name, $table, $col);
	if (db_scalar($query) < 255) {
		$query = sprintf("ALTER TABLE `%1\$s` CHANGE `%2\$s` `%2\$s` VARCHAR(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL;", $table, $col);
		fnQuery($query, 2);
	}
	
	/***** Aanpassen lengte van het IP-adres in Admin_interface naar 45 voor IP v6 adressen. Deze code kan na 1/4/2021 worden verwijderd. *****/
	$table = TABLE_PREFIX . "Admin_interface";
	$col = "IP-adres";
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE '%s';", $db_name, $table, $col);
	if (db_scalar($query) < 45) {
		$query = sprintf("ALTER TABLE `%1\$s` CHANGE `%2\$s` `%2\$s` VARCHAR(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table, $col);
		fnQuery($query, 2);
	}
	
	/***** Velden die later zijn toegevoegd. *****/
	
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
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NOT NULL DEFAULT '0'`;", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'IngevoerdDoor' aan de tabel 'Evenement'. Deze code kan na 1/12/2020 worden verwijderd.
	$tab = TABLE_PREFIX . "Evenement";
	$col = "IngevoerdDoor";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NOT NULL DEFAULT '0';", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'Ingevoerd' aan de tabel 'Mailing_hist'. Deze code kan na 1/12/2020 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing_hist";
	$col = "Ingevoerd";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", $tab, $col);
		fnQuery($query, 2);
	}
		
	// Toevoegen veld 'Ingevoerd' aan de tabel 'Mailing_hist'. Deze code kan na 1 december 2020 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing_hist";
	$col = "IngevoerdDoor";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NOT NULL;", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'ZichtbaarVoor' aan de tabel 'Mailing'. Deze code kan na 1/1/2021 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing";
	$col = "ZichtbaarVoor";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NOT NULL DEFAULT '0' AFTER `confidential`;", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'Gewijzigd' aan de tabel 'Mailing'. Deze code kan na 1/1/2021 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing";
	$col = "Gewijzigd";
	$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", $db_name, $tab, $col);
	if (db_scalar($query) == 0) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` TIMESTAMP NOT NULL;", $tab, $col);
		fnQuery($query, 2);
	}
		
	// Toevoegen veld 'ZichtbaarVoor' aan de tabel 'Mailing_hist'. Deze code kan na 1/1/2021 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing_hist";
	$col = "ZichtbaarVoor";
	if (bestaatkolom($tab, $col) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NOT NULL DEFAULT '0' AFTER `message`;", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'Gewijzigd' aan de tabel 'Mailing_hist'. Deze code kan na 1/1/2021 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing_hist";
	$col = "Gewijzigd";
	if (bestaatkolom($tab, $col) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` TIMESTAMP NOT NULL;", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'GewijzigdDoor' aan de tabel 'Mailing'. Deze code kan na 1/1/2021 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing";
	$col = "GewijzigdDoor";
	if (bestaatkolom($tab, $col) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NOT NULL DEFAULT '0';", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'MaxPersonenPerDeelname' aan de tabel 'Evenement'. Deze code kan na 1/1/2021 worden verwijderd.
	$tab = TABLE_PREFIX . "Evenement";
	$col = "MaxPersonenPerDeelname";
	if (bestaatkolom($tab, $col) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NOT NULL DEFAULT '1' AFTER `StandaardStatus`;", $tab, $col);
		fnQuery($query, 2);
	}
		
	// Toevoegen veld 'OmschrijvingOntvangers' aan de tabel 'Mailing'. Deze code kan na 1/1/2021 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing";
	$col = "OmschrijvingOntvangers";
	if (bestaatkolom($tab, $col) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(50) NULL DEFAULT  '' COMMENT 'Omschrijving van de groep personen aan wie deze mailing gericht is' AFTER `to_name`;", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'SentBy' aan de tabel 'Mailing'. Deze code kan na 1/3/2021 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing";
	$col = "SentBy";
	if (bestaatkolom($tab, $col) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT(11) NOT NULL DEFAULT 0 AFTER `ChangedBy`;", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen veld 'DeletedBy' aan de tabel 'Mailing'. Deze code kan na 1/3/2021 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing";
	$col = "DeletedBy";
	if (bestaatkolom($tab, $col) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT(11) NOT NULL DEFAULT 0 AFTER `SentBy`;", $tab, $col);
		fnQuery($query, 2);
	}
		
	// Toevoegen veld 'ReferLidID' aan de tabel 'Admin_activiteit'. Deze code kan na 1/4/2021 worden verwijderd.
	$tab = TABLE_PREFIX . "Admin_activiteit";
	$col = "ReferLidID";
	if (bestaatkolom($tab, $col) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT(11) NOT NULL DEFAULT 0 AFTER `ReferID`;", $tab, $col);
		fnQuery($query, 2);
	}
		
	// Toevoegen kolom 'RefTable' aan de tabel 'Admin_activiteit'. Deze code kan na 1 mei 2021 worden verwijderd.
	$tab = TABLE_PREFIX . "Admin_activiteit";
	$col = "RefTable";
	if (bestaatkolom($tab, $col) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(30) NULL AFTER `RefFunction`;", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen kolom 'TypeActiviteitSpecifiek' aan de tabel 'Admin_activiteit'. Deze code kan na 1 mei 2021 worden verwijderd.
	$tab = TABLE_PREFIX . "Admin_activiteit";
	$col = "TypeActiviteitSpecifiek";
	if (bestaatkolom($tab, $col) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` SMALLINT NULL AFTER `TypeActiviteit`;", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen kolom 'RecordID' aan de tabel 'Mailing'. Deze code kan na 1 mei 2021 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing";
	$col = "RecordID";
	if (bestaatkolom($tab, $col) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NOT NULL DEFAULT '0' FIRST;", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen kolom 'Opmerking' aan de tabel 'Lid'. Deze code kan na 1 mei 2021 worden verwijderd.
	$tab = TABLE_PREFIX . "Lid";
	$col = "Opmerking";
	if (bestaatkolom($tab, $col) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(60) NULL AFTER `Beroep`;", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen kolom 'Aantal' aan de tabel 'Evenement_Deelnemer'. Deze code kan na 1 mei 2021 worden verwijderd.
	$tab = TABLE_PREFIX . "Evenement_Deelnemer";
	$col = "Aantal";
	if (bestaatkolom($tab, $col) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NOT NULL DEFAULT '1' AFTER `Status`;", $tab, $col);
		fnQuery($query, 2);
	}
	
	// Toevoegen kolom 'ZonderBriefpapier' aan de tabel 'Mailing'. Deze code kan na 1 mei 2021 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing";
	$col = "ZonderBriefpapier";
	if (bestaatkolom($tab, $col) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` TINYINT NULL DEFAULT '0' AFTER `message`;", $tab, $col);
		fnQuery($query, 2);
	}
	
	/***** Velden die aangepast zijn. *****/
		
	// Deze code kan na 1/12/2020 worden verwijderd.
	$query = sprintf("ALTER TABLE `%sEvenement_Deelnemer` CHANGE `Functie` `Functie` VARCHAR(30) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL;", TABLE_PREFIX);
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
	$query = sprintf("ALTER TABLE `%sMailing` CHANGE `SentBy` `SentBy` INT(11) NOT NULL DEFAULT '0';", TABLE_PREFIX);
	fnQuery($query);
	
	// Admin_Login, deze code kan na 31/12/2020 worden verwijderd.\
	$query = sprintf("ALTER TABLE %sAdmin_login CHANGE `Ingevoerd` `Ingevoerd` DATETIME;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sMailing` CHANGE `ChangedBy` `ChangedBy` INT(11) NOT NULL DEFAULT '0';", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sMailing_hist` CHANGE `to_addr` `to_addr` VARCHAR(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sMailing_hist` CHANGE `cc_addr` `cc_addr` VARCHAR(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", TABLE_PREFIX);
	fnQuery($query);
	
	// Admin_activiteit, deze code kan na 1/3/2021 worden verwijderd.
	$query = sprintf("ALTER TABLE `%sAdmin_activiteit` CHANGE `USER_AGENT` `USER_AGENT` VARCHAR(125) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL;", TABLE_PREFIX);
	fnQuery($query);
	
	// Deze code kan na 1 mei 2021 worden verwijderd.
	$query = sprintf("ALTER TABLE `%sMailing` CHANGE `OmschrijvingOntvangers` `OmschrijvingOntvangers` VARCHAR(50) CHARACTER SET utf8 COLLATE utf8_general_ci NULL COMMENT 'Omschrijving van de groep personen aan wie deze mailing gericht is';", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sMailing` CHANGE `DeletedBy` `DeletedBy` INT(11) NULL;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sMailing_hist` CHANGE `send_by` `send_by` INT(11) NULL;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sMailing_hist` CHANGE `send_on` `send_on` DATETIME NULL;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sMailing_hist` CHANGE `send_mt` `send_mt` DECIMAL(16,6) NULL;", TABLE_PREFIX);
	fnQuery($query);
	$query = sprintf("ALTER TABLE `%sAdmin_activiteit` CHANGE `Script` `Script` VARCHAR(90) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", TABLE_PREFIX);
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
			if (fclose($buf) == true) {
			
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
				db_logboek("add", "Het backup-script heeft gedraaid, maar de backup is niet gelukt.\n", 2, 0, 1);
			}
		} else {
			db_logboek("add", "Het backup-script heeft gedraaid, maar de backup is niet gelukt.\n", 2, 0, 1);
		}
	} else {
//		db_logboek("add", "Het backupscript heeft korter dan 1 uur geleden met succes gedraaid. Het wordt niet nogmaals uitgevoerd.", 2, 0, 1);
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
	
	$query = sprintf("SELECT %1\$s AS LidNaam FROM %2\$sLid AS L INNER JOIN %2\$sAdmin_login AS Login ON L.RecordID = Login.LidID"
			 . " ORDER BY Login.Ingevoerd DESC LIMIT 1;", $selectnaam, TABLE_PREFIX);
	$stats['nieuwstelogin'] = db_scalar($query);
	
	$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login AS Login;", TABLE_PREFIX);
	$stats['aantallogins'] = db_scalar($query);
	
	$query = sprintf("SELECT MAX(DatumTijd) FROM %sAdmin_activiteit WHERE TypeActiviteit=9;", TABLE_PREFIX);
	$stats['laatsteupload'] = db_scalar($query);
	
	$query = sprintf("SELECT %1\$s AS LidNaam FROM %2\$sLid AS L INNER JOIN %2\$sAdmin_login AS Login ON L.RecordID = Login.LidID"
			 . " WHERE Login.Ingelogd=1 ORDER BY Login.Ingevoerd DESC;", $selectnaam, TABLE_PREFIX);
	$result = fnQuery($query);
	$stats['nuingelogd'] = "";
	foreach($result->fetchAll() as $row) {
		if (strlen($stats['nuingelogd']) > 1) { $stats['nuingelogd'] .= ", "; }
		$stats['nuingelogd'] .= $row->LidNaam;
	}
	
	if ($lidid > 0) {
		$query = sprintf("SELECT L.Roepnaam FROM %sLid AS L WHERE L.RecordID=%d;", TABLE_PREFIX, $lidid);
		$stats['roepnaamingelogde'] = db_scalar($query);
		
		$query = sprintf("SELECT L.GEBDATUM FROM %sLid AS L WHERE L.RecordID=%d;", TABLE_PREFIX, $lidid);
		$stats['geboortedatumingelogde'] = db_scalar($query);
		
		$query = sprintf("SELECT L.Gewijzigd FROM %sLid AS L WHERE L.RecordID=%d;", TABLE_PREFIX, $lidid);
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

function db_typekolom($tabel, $kolom) {
	global $db_name;
	
	if (!startwith($tabel, TABLE_PREFIX)) {
		$tabel = TABLE_PREFIX . $tabel;
	}
	$query = sprintf("SELECT DATA_TYPE FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE '%s';", $db_name, $tabel, $kolom);
	return db_scalar($query);
	
} # db_typekolom

function db_kolom_numeriek($tabel, $kolom) {
	
	if (!startwith($tabel, TABLE_PREFIX)) {
		$tabel = TABLE_PREFIX . $tabel;
	}

	if (in_array(db_typekolom($tabel, $kolom), array("bigint", "int", "tinyint"))) {
		return true;
	} else {
		return false;
	}
}  # db_kolom_numeriek

function db_nieuwrecordID($tabel) {
	/* Change log
	ST20181221 RecordID's voor tabellen die uit Access komen moeten oneven zijn. Dit om het onderscheid te kunnen maken met RecordID's die uit de Access-database komen.
	ST20191225 RecordID's voor tabellen die bij elkaar horen uniek binnen die groep gemaakt.
	ST20200327: alleen recordid's van tabellen die ook in de database staan moeten oneven zijn. Hierdoor lopen de nummers minder snel op.
	*/
	
	global $arrTables;
	
	$tabel = str_replace(TABLE_PREFIX, "", $tabel);
	
	$rid = 0;
	
	if (startwith($tabel, "Evenement")) {
		$rid = db_scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sEvenement;", TABLE_PREFIX));
		$r = db_scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sEvenement_Deelnemer;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		$r = db_scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sEvenement_Type;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		
	} elseif ($tabel == "Lid" or $tabel == "Lidmaatschap" or $tabel == "Lidond" or $tabel == "Memo") {
		$rid = db_scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sLid;", TABLE_PREFIX));
		$r = db_scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sLidond;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		$r = db_scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sLidmaatschap;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		$r = db_scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sMemo;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		
	} elseif ($tabel == "Onderdl" or $tabel == "Groep" or $tabel == "Diploma" or $tabel == "Functie") {
		$rid = db_scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sOnderdl;", TABLE_PREFIX));
		$r = db_scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sGroep;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		$r = db_scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sDiploma;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		$r = db_scalar(sprintf("SELECT IFNULL(MAX(F.Nummer), 0) FROM %sFunctie AS F;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		
	} elseif ($tabel == "Mailing_hist" or $tabel == "Mailing_rcpt") {
		$rid = db_scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sMailing_hist;", TABLE_PREFIX));
		$r = db_scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sMailing_rcpt;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		
	} else {
		if ($tabel == "Mailing") {
			$kol = "MailingID";
		} elseif (bestaatkolom($tabel, "RecordID")) {
			$kol = "RecordID";
		} else {
			$kol = "Nummer";
		}
		$query = sprintf("SELECT MAX(IFNULL(%s, 0)) FROM %s;", $kol, TABLE_PREFIX . $tabel);
		$rid = db_scalar($query);
	}
	
	$rid++;
	if (($rid / 2) == intval($rid / 2) and array_search($tabel, $arrTables) >= 20) {
		$rid++;
	}
	return $rid;
	
} # db_nieuwrecordID

function db_createtables() {

	$queries = sprintf("SET SQL_MODE = 'NO_AUTO_VALUE_ON_ZERO';

CREATE TABLE `%1\$sAdmin_access` (
  `RecordID` int(11) NOT NULL,
  `Tabpage` varchar(50) NOT NULL,
  `Toegang` int(11) NOT NULL DEFAULT -1,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp
) ;

CREATE TABLE `%1\$sAdmin_activiteit` (
  `RecordID` int(11) NOT NULL,
  `LidID` int(11) NOT NULL COMMENT 'Wie heeft deze activiteit uitgevoerd?',
  `DatumTijd` datetime NOT NULL DEFAULT current_timestamp(),
  `Omschrijving` text NOT NULL,
  `ReferID` int(11) NOT NULL DEFAULT 0 COMMENT 'Op welk RecordID heeft deze mutatie betrekking?',
  `ReferLidID` int(11) NOT NULL DEFAULT 0 COMMENT 'Op welk lid heeft deze mutatie betrekking?',
  `Script` varchar(75) NOT NULL,
  `TypeActiviteit` tinyint(4) DEFAULT 0,
  `TypeActiviteitSpecifiek` smallint(6) DEFAULT NULL,
  `IP_adres` varchar(45) NOT NULL,
  `USER_AGENT` varchar(125) DEFAULT NULL,
  `Getoond` tinyint(4) DEFAULT NULL COMMENT 'Is deze melding aan de gebruiker getoond?',
  `RefFunction` varchar(50) DEFAULT NULL,
  `RefTable` varchar(30) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sAdmin_interface` (
  `RecordID` int(11) NOT NULL,
  `LidID` int(11) DEFAULT NULL,
  `IP-adres` varchar(45) NOT NULL,
  `SQL-statement` text CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `IngelogdLid` int(11) DEFAULT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `Gedownload` datetime DEFAULT NULL,
  `Afgemeld` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sAdmin_login` (
  `LidID` int(11) NOT NULL,
  `Login` varchar(15) NOT NULL,
  `Wachtwoord` varchar(255) NOT NULL,
  `HerinneringVervallenDiplomas` tinyint(4) NOT NULL DEFAULT 0,
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  `GewijzigdDoor` int(11) NOT NULL DEFAULT 0,
  `LastLogin` datetime DEFAULT NULL,
  `LastActivity` datetime DEFAULT NULL,
  `Ingelogd` tinyint(4) NOT NULL DEFAULT 0,
  `FouteLogin` smallint(6) NOT NULL DEFAULT 0,
  `ActivatieKey` varchar(255) DEFAULT NULL,
  `2FA` tinyint(4) DEFAULT NULL,
  `LaatsteWachtwoordWijziging` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sAdmin_param` (
  `RecordID` int(11) NOT NULL,
  `Naam` varchar(50) CHARACTER SET utf8 NOT NULL,
  `ParamType` char(2) COLLATE utf8_unicode_ci DEFAULT NULL,
  `ValueChar` text COLLATE utf8_unicode_ci DEFAULT NULL,
  `ValueNum` decimal(16,6) DEFAULT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE current_timestamp
) ;

CREATE TABLE `%1\$sBewaking` (
  `RecordID` int(11) DEFAULT NULL,
  `Lid` int(11) NOT NULL DEFAULT 0,
  `SeizoenID` int(11) NOT NULL DEFAULT 0,
  `Weeknr` tinyint(4) DEFAULT NULL,
  `BEGIN_PER` date NOT NULL,
  `EINDE_PER` date DEFAULT NULL,
  `Post` varchar(7) DEFAULT NULL,
  `Status` varchar(1) DEFAULT NULL,
  `Functie` tinyint(4) DEFAULT NULL,
  `Opmerking` varchar(50) DEFAULT NULL,
  `Beoordeling` longtext DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sBewaking_Blok` (
  `RecordID` int(11) NOT NULL,
  `SeizoenID` int(11) NOT NULL,
  `Kode` char(2) DEFAULT NULL,
  `Begindatum` date NOT NULL,
  `Einddatum` date NOT NULL,
  `InschrijvingOpen` tinyint(4) NOT NULL DEFAULT 1,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp
) ;

CREATE TABLE `%1\$sBewaking_Inschrijving` (
  `RecordID` int(11) NOT NULL,
  `Lid` int(11) NOT NULL,
  `Nummer` int(11) NOT NULL DEFAULT 0,
  `BlokID` int(11) NOT NULL,
  `Keuze` tinyint(4) NOT NULL DEFAULT 1,
  `Opmerking` varchar(50) DEFAULT NULL,
  `Definitief` datetime DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp
) ;

CREATE TABLE `%1\$sBewseiz` (
  `RecordID` int(11) NOT NULL DEFAULT 0,
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
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sBoekjaar` (
  `RecordID` int(11) NOT NULL DEFAULT 0,
  `Kode` varchar(5) DEFAULT NULL,
  `Omschrijving` varchar(50) DEFAULT NULL,
  `Begindatum` datetime DEFAULT NULL,
  `Einde` datetime DEFAULT NULL,
  `Afgesloten` tinyint(4) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sDiploma` (
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
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sEvenement` (
  `RecordID` int(11) NOT NULL,
  `Datum` datetime NOT NULL,
  `Eindtijd` varchar(5) NOT NULL,
  `Verzameltijd` varchar(5) NOT NULL,
  `Omschrijving` varchar(50) NOT NULL,
  `Locatie` varchar(75) DEFAULT NULL,
  `Email` varchar(45) NOT NULL,
  `TypeEvenement` int(11) NOT NULL,
  `InschrijvingOpen` tinyint(4) NOT NULL DEFAULT 1,
  `StandaardStatus` char(1) NOT NULL DEFAULT 'I',
  `MaxPersonenPerDeelname` int(11) NOT NULL DEFAULT 1,
  `BeperkTotGroep` int(11) NOT NULL DEFAULT 0 COMMENT 'Welke groep mag zich voor dit evenement inschrijven? 0 = iedereen.',
  `ZichtbaarVoor` int(11) NOT NULL DEFAULT 0 COMMENT 'Voor welke groep is dit evenement zichtbaar? 0 = iedereen.',
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp
) ;

CREATE TABLE `%1\$sEvenement_Deelnemer` (
  `RecordID` int(11) NOT NULL,
  `LidID` int(11) NOT NULL,
  `EvenementID` int(11) NOT NULL,
  `Functie` varchar(30) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `Opmerking` varchar(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `Status` char(1) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT 'I',
  `Aantal` int(11) NOT NULL DEFAULT 1,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL DEFAULT 0,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp
) ;

CREATE TABLE `%1\$sEvenement_Type` (
  `RecordID` int(11) NOT NULL,
  `Omschrijving` varchar(30) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL,
  `Soort` char(1) NOT NULL DEFAULT 'W',
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp
) ;

CREATE TABLE `%1\$sFunctie` (
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
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sGBR` (
  `RecordID` int(11) DEFAULT NULL,
  `Kode` varchar(4) NOT NULL DEFAULT '',
  `OMSCHRIJV` varchar(40) DEFAULT NULL,
  `Balans` tinyint(4) DEFAULT NULL,
  `KSTNPLTS` tinyint(4) DEFAULT NULL,
  `VerdichtingID` int(11) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  `Standaard kostenplaats` int(11) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sGroep` (
  `RecordID` int(11) NOT NULL,
  `OnderdeelID` int(11) DEFAULT NULL,
  `Kode` varchar(5) DEFAULT NULL,
  `Omschrijving` varchar(45) DEFAULT NULL,
  `Instructeurs` varchar(45) DEFAULT NULL,
  `DiplomaID` int(11) DEFAULT NULL,
  `Starttijd` varchar(5) DEFAULT NULL,
  `Eindtijd` varchar(5) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sKostenplaats` (
  `RecordID` int(11) NOT NULL DEFAULT 0,
  `Kode` varchar(5) DEFAULT NULL,
  `Naam` varchar(35) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sLid` (
  `RecordID` int(11) DEFAULT NULL,
  `Nummer` int(11) NOT NULL,
  `Roepnaam` varchar(17) DEFAULT NULL,
  `Tussenv` varchar(7) DEFAULT NULL,
  `Achternaam` varchar(40) DEFAULT NULL,
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
  `EmailOuders` varchar(75) DEFAULT NULL,
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
  `Opmerking` varchar(60) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  `Verwijderd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sLiddipl` (
  `RecordID` int(11) NOT NULL,
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
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sLidmaatschap` (
  `RecordID` int(11) NOT NULL,
  `Lid` int(11) NOT NULL,
  `LIDDATUM` date NOT NULL,
  `Opgezegd` date DEFAULT NULL,
  `OpgezegdDoorVereniging` tinyint(4) DEFAULT NULL,
  `Lidnr` int(11) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sLidond` (
  `RecordID` int(11) NOT NULL,
  `Lid` int(11) NOT NULL,
  `OnderdeelID` int(11) NOT NULL,
  `Vanaf` date NOT NULL,
  `Opgezegd` date DEFAULT NULL,
  `Functie` smallint(6) NOT NULL,
  `EmailFunctie` varchar(45) DEFAULT NULL,
  `GroepID` int(11) DEFAULT NULL,
  `Opmerk` varchar(30) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sLidRedNed` (
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

CREATE TABLE `%1\$sMailing` (
  `RecordID` int(11) NOT NULL DEFAULT 0,
  `MailingID` int(11) NOT NULL,
  `from_name` varchar(50) DEFAULT '',
  `from_addr` varchar(50) DEFAULT '',
  `to_name` varchar(50) DEFAULT '' COMMENT 'Omschrijving van de groep personen aan wie deze mailing gericht is',
  `OmschrijvingOntvangers` varchar(50) NOT NULL DEFAULT '' COMMENT 'Omschrijving van de groep personen aan wie deze mailing gericht is',
  `cc_addr` varchar(50) DEFAULT '',
  `subject` varchar(75) DEFAULT '',
  `message` text DEFAULT NULL,
  `template` tinyint(4) NOT NULL DEFAULT 0,
  `confidential` tinyint(4) NOT NULL DEFAULT 0,
  `ZichtbaarVoor` int(11) NOT NULL DEFAULT 0,
  `verzamelen` tinyint(4) NOT NULL DEFAULT 0,
  `new_on` datetime NOT NULL DEFAULT current_timestamp(),
  `AddedBy` int(11) NOT NULL,
  `changed_on` datetime NOT NULL DEFAULT current_timestamp(),
  `ChangedBy` int(11) NOT NULL DEFAULT 0,
  `sent_on` datetime DEFAULT NULL,
  `SentBy` int(11) NOT NULL DEFAULT 0,
  `deleted_on` datetime DEFAULT NULL,
  `DeletedBy` int(11) NOT NULL DEFAULT 0,
  `HTMLdirect` tinyint(4) DEFAULT 0,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp
) ;

CREATE TABLE `%1\$sMailing_hist` (
  `RecordID` int(11) NOT NULL,
  `LidID` int(11) NOT NULL DEFAULT 0,
  `MailingID` int(11) NOT NULL,
  `Xtra_Char` varchar(5) DEFAULT NULL,
  `Xtra_Num` int(11) DEFAULT NULL,
  `from_name` varchar(50) NOT NULL,
  `from_addr` varchar(50) NOT NULL,
  `to_name` varchar(50) NOT NULL,
  `subject` varchar(75) NOT NULL,
  `to_addr` varchar(255) NOT NULL,
  `cc_addr` varchar(255) NOT NULL,
  `message` text NOT NULL,
  `ZichtbaarVoor` int(11) NOT NULL DEFAULT 0,
  `send_by` int(11) DEFAULT NULL,
  `send_on` datetime DEFAULT NULL,
  `send_mt` decimal(16,0) DEFAULT NULL,
  `Successful` tinyint(4) NOT NULL DEFAULT 1,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp
) ;

CREATE TABLE `%1\$sMailing_rcpt` (
  `RecordID` int(11) NOT NULL,
  `MailingID` int(11) NOT NULL DEFAULT 0,
  `LidID` int(11) NOT NULL DEFAULT 0,
  `to_address` varchar(50) DEFAULT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sMailing_vanaf` (
  `RecordID` int(11) NOT NULL,
  `Vanaf_email` varchar(30) NOT NULL,
  `Vanaf_naam` varchar(50) NOT NULL,
  `SMTP_server` tinyint(20) NOT NULL DEFAULT 0 COMMENT 'Indien 0, dan wordt de smtp server uit het config bestand gebruikt',
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp
) ;

CREATE TABLE `%1\$sMemo` (
  `RecordID` int(11) NOT NULL,
  `Lid` int(11) NOT NULL,
  `Soort` varchar(1) NOT NULL,
  `Vertrouwelijk` tinyint(4) DEFAULT 0,
  `Memo` longtext DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sMutatie` (
  `RecordID` int(11) DEFAULT NULL,
  `BoekjaarID` int(11) NOT NULL DEFAULT 0,
  `Periode` tinyint(4) NOT NULL DEFAULT 0,
  `DagboekID` int(11) NOT NULL DEFAULT 0,
  `GBR` varchar(4) DEFAULT NULL,
  `KostenplaatsID` int(11) DEFAULT NULL,
  `Stuknr` smallint(6) NOT NULL DEFAULT 0,
  `Regelnr` smallint(6) NOT NULL DEFAULT 0,
  `Datum` date DEFAULT NULL,
  `Debet` decimal(8,2) DEFAULT NULL,
  `Credit` decimal(8,2) DEFAULT NULL,
  `OMSCHRIJV` varchar(45) DEFAULT NULL,
  `Rekening` int(11) DEFAULT NULL,
  `BTWcode` tinyint(4) DEFAULT NULL,
  `BTWbedrag` decimal(8,2) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sOnderdl` (
  `RecordID` int(11) NOT NULL,
  `Kode` varchar(7) DEFAULT NULL,
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
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sOrganisatie` (
  `Nummer` smallint(6) NOT NULL,
  `Naam` varchar(8) DEFAULT NULL,
  `Volledige naam` varchar(50) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sRekening` (
  `Nummer` int(11) NOT NULL,
  `Seizoen` int(11) DEFAULT NULL,
  `Datum` date DEFAULT NULL,
  `OMSCHRIJV` varchar(30) DEFAULT NULL,
  `Lid` int(11) DEFAULT NULL,
  `DEBNAAM` varchar(60) DEFAULT NULL,
  `Ouders` tinyint(4) DEFAULT NULL,
  `BetaaldDoor` int(11) DEFAULT NULL,
  `BET_TERM` smallint(6) DEFAULT NULL,
  `BETAALDAG` int(11) DEFAULT NULL,
  `Bedrag` decimal(8,2) DEFAULT NULL,
  `Betaald` decimal(8,2) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sRekreg` (
  `RecordID` int(11) DEFAULT NULL,
  `Rekening` int(11) NOT NULL,
  `Regelnr` int(11) NOT NULL,
  `Lid` int(11) DEFAULT NULL,
  `KSTNPLTS` varchar(5) DEFAULT NULL,
  `OMSCHRIJV` varchar(60) DEFAULT NULL,
  `Bedrag` decimal(8,2) DEFAULT NULL,
  `ToonOpRekening` tinyint(4) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sStukken` (
  `RecordID` int(11) NOT NULL,
  `Titel` varchar(50) NOT NULL,
  `Type` char(1) NOT NULL DEFAULT 'R',
  `BestemdVoor` varchar(30) NOT NULL,
  `VastgesteldOp` date NOT NULL,
  `Ingangsdatum` date NOT NULL,
  `Revisiedatum` date NOT NULL,
  `Link` varchar(150) NOT NULL,
  `VervallenPer` date DEFAULT NULL,
  `IngevoerdOp` datetime NOT NULL DEFAULT current_timestamp(),
  `GewijzigdOp` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp
) ;

CREATE TABLE `%1\$sTaak` (
  `RecordID` int(11) NOT NULL,
  `Omschrijving` varchar(75) NOT NULL,
  `Beschrijving` text NOT NULL,
  `Verantwoordelijke` int(11) NOT NULL,
  `TaakGroep` int(11) NOT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp
) ;

CREATE TABLE `%1\$sTaakGroep` (
  `RecordID` int(11) NOT NULL,
  `Kode` varchar(5) NOT NULL,
  `Naam` varchar(50) NOT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp
) ;

CREATE TABLE `%1\$sVerdichting` (
  `RecordID` int(11) NOT NULL DEFAULT 0,
  `Kode` varchar(4) DEFAULT NULL,
  `Omschrijving` varchar(40) DEFAULT NULL,
  `DebetCredit` varchar(1) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sVereniging` (
  `Naam` varchar(50) DEFAULT NULL,
  `Brigadenaam` varchar(50) DEFAULT NULL,
  `AfkortingNaam` varchar(5) DEFAULT NULL,
  `Bankrekening` varchar(18) DEFAULT NULL,
  `EmailSecretariaat` varchar(45) DEFAULT NULL,
  `EmailLedenadministratie` varchar(45) DEFAULT NULL,
  `BewaartijdHistorie` int(11) DEFAULT NULL,
  `GroepKaderleden` int(11) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `%1\$sWS_Artikel` (
  `RecordID` int(11) NOT NULL,
  `Code` char(6) COLLATE utf8_unicode_ci DEFAULT NULL,
  `Omschrijving` varchar(50) COLLATE utf8_unicode_ci DEFAULT NULL,
  `Beschrijving` text COLLATE utf8_unicode_ci DEFAULT NULL,
  `Maat` char(4) COLLATE utf8_unicode_ci DEFAULT NULL,
  `Verkoopprijs` decimal(10,2) NOT NULL DEFAULT 0.00,
  `BeschikbaarTot` date NOT NULL DEFAULT '9999-12-31' COMMENT 'Tot welke datum is dit artikel in de zelfservice beschikbaar?',
  `VervallenPer` date NOT NULL DEFAULT '9999-12-31' COMMENT 'Na deze datum is dit artikel voor niemand meer beschikbaar.',
  `MaxAantalPerLid` smallint(6) NOT NULL DEFAULT 999,
  `BeperkTotGroep` int(11) NOT NULL DEFAULT 0 COMMENT 'Welke groep mag dit artikel in de zelfservice bestellen? 0 is iedereen.',
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp
) ;

CREATE TABLE `%1\$sWS_Orderregel` (
  `RecordID` int(11) NOT NULL,
  `Ordernr` int(11) NOT NULL DEFAULT 0,
  `Artikel` int(11) NOT NULL DEFAULT 0,
  `Lid` int(11) NOT NULL DEFAULT 0,
  `AantalBesteld` smallint(6) NOT NULL DEFAULT 0,
  `PrijsPerStuk` decimal(10,2) DEFAULT NULL,
  `Opmerking` varchar(30) COLLATE utf8_unicode_ci DEFAULT NULL,
  `BestellingDefinitief` datetime DEFAULT NULL COMMENT 'Heeft het lid de bestelling bevestigd?',
  `AantalGeleverd` smallint(6) NOT NULL DEFAULT 0,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp
) ;


ALTER TABLE `%1\$sAdmin_activiteit`
  ADD PRIMARY KEY (`RecordID`),
  ADD KEY `TypeActiviteit` (`TypeActiviteit`);

ALTER TABLE `%1\$sAdmin_interface`
  ADD PRIMARY KEY (`RecordID`);

ALTER TABLE `%1\$sAdmin_login`
  ADD PRIMARY KEY (`LidID`),
  ADD UNIQUE KEY `Login` (`Login`);

ALTER TABLE `%1\$sBewaking`
  ADD PRIMARY KEY (`SeizoenID`,`Lid`,`BEGIN_PER`);

ALTER TABLE `%1\$sBewseiz`
  ADD PRIMARY KEY (`RecordID`),
  ADD UNIQUE KEY `Kode` (`Kode`);

ALTER TABLE `%1\$sBoekjaar`
  ADD PRIMARY KEY (`RecordID`),
  ADD UNIQUE KEY `Kode` (`Kode`);

ALTER TABLE `%1\$sDiploma`
  ADD PRIMARY KEY (`RecordID`),
  ADD UNIQUE KEY `Kode` (`Kode`);

ALTER TABLE `%1\$sEigenQueries`
  ADD PRIMARY KEY (`Nummer`);

ALTER TABLE `%1\$sFunctie`
  ADD PRIMARY KEY (`Nummer`);

ALTER TABLE `%1\$sGBR`
  ADD PRIMARY KEY (`Kode`);

ALTER TABLE `%1\$sGroep`
  ADD PRIMARY KEY (`RecordID`),
  ADD UNIQUE KEY `Kode` (`Kode`);

ALTER TABLE `%1\$sKostenplaats`
  ADD PRIMARY KEY (`RecordID`),
  ADD UNIQUE KEY `Kode` (`Kode`);

ALTER TABLE `%1\$sLid`
  ADD PRIMARY KEY (`Nummer`);

ALTER TABLE `%1\$sLiddipl`
  ADD PRIMARY KEY (`RecordID`),
  ADD UNIQUE KEY `RecordID` (`RecordID`);

ALTER TABLE `%1\$sLidmaatschap`
  ADD PRIMARY KEY (`Lid`,`LIDDATUM`),
  ADD UNIQUE KEY `RecordID` (`RecordID`),
  ADD UNIQUE KEY `Lidnr` (`Lidnr`);

ALTER TABLE `%1\$sLidond`
  ADD PRIMARY KEY (`Lid`,`OnderdeelID`,`Vanaf`,`Functie`),
  ADD UNIQUE KEY `RecordID` (`RecordID`);

ALTER TABLE `%1\$sMailing_rcpt`
  ADD PRIMARY KEY (`RecordID`);

ALTER TABLE `%1\$sMemo`
  ADD PRIMARY KEY (`Lid`,`Soort`),
  ADD UNIQUE KEY `RecordID` (`RecordID`);

ALTER TABLE `%1\$sMutatie`
  ADD PRIMARY KEY (`BoekjaarID`,`Periode`,`DagboekID`,`Stuknr`,`Regelnr`);

ALTER TABLE `%1\$sOnderdl`
  ADD PRIMARY KEY (`RecordID`),
  ADD UNIQUE KEY `Kode` (`Kode`);

ALTER TABLE `%1\$sOrganisatie`
  ADD PRIMARY KEY (`Nummer`);

ALTER TABLE `%1\$sRekening`
  ADD PRIMARY KEY (`Nummer`);

ALTER TABLE `%1\$sRekreg`
  ADD PRIMARY KEY (`Rekening`,`Regelnr`);

ALTER TABLE `%1\$sVerdichting`
  ADD PRIMARY KEY (`RecordID`),
  ADD UNIQUE KEY `Kode` (`Kode`);


ALTER TABLE `%1\$sAdmin_access`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT;
ALTER TABLE `%1\$sAdmin_activiteit`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=23487;
ALTER TABLE `%1\$sAdmin_interface`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3161;
ALTER TABLE `%1\$sAdmin_param`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT;
ALTER TABLE `%1\$sBewaking_Blok`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT;
ALTER TABLE `%1\$sBewaking_Inschrijving`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT;
ALTER TABLE `%1\$sEvenement`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT;
ALTER TABLE `%1\$sEvenement_Deelnemer`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT;
ALTER TABLE `%1\$sEvenement_Type`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT;
ALTER TABLE `%1\$sLiddipl`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=121967;
ALTER TABLE `%1\$sLidmaatschap`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=117291;
ALTER TABLE `%1\$sLidond`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=129763;
ALTER TABLE `%1\$sMailing`
  MODIFY `MailingID` int(11) NOT NULL AUTO_INCREMENT;
ALTER TABLE `%1\$sMailing_hist`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT;
ALTER TABLE `%1\$sMailing_rcpt`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=8424;
ALTER TABLE `%1\$sMailing_vanaf`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT;
ALTER TABLE `%1\$sMemo`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT;
ALTER TABLE `%1\$sStukken`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT;
ALTER TABLE `%1\$sTaak`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT;
ALTER TABLE `%1\$sTaakGroep`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT;
ALTER TABLE `%1\$sWS_Artikel`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT;
ALTER TABLE `%1\$sWS_Orderregel`
  MODIFY `RecordID` int(11) NOT NULL AUTO_INCREMENT;", TABLE_PREFIX);

	fnQuery($queries);
}  # db_createtables

?>
