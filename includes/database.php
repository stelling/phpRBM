<?php

$arrTables[0] = "Admin_access";
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
$arrTables[] = "Afdelingskalender";
$arrTables[] = "Aanwezigheid";
$arrTables[] = "WS_Voorraadboeking";
$arrTables[] = "RekeningBetaling";
// $arrTables[] = "DMS_Document";
// $arrTables[] = "DMS_Folder";

// Overgenomen uit de Access-database
$arrTables[30] = "Activiteit";
$arrTables[] = "Bewaking";
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
$arrTables[] = "Activiteit";
//$arrTables[] = "LidRedNed";
$arrTables[] = "Memo";
$arrTables[] = "Mutatie";
$arrTables[] = "Onderdl";
$arrTables[] = "Organisatie";
$arrTables[] = "Rekening";
$arrTables[] = "Rekreg";
$arrTables[] = "Seizoen";
// $arrTables[] = "Vereniging";

$TypeActiviteit[0] = "N.T.B.";
$TypeActiviteit[1] = "Inloggen/Uitloggen";
$TypeActiviteit[2] = "Onderhoud/beheer/Jobs";
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
$TypeActiviteit[14] = "Rekeningen en betalingen";
$TypeActiviteit[15] = "Autorisatie";
$TypeActiviteit[16] = "Toestemmingen per lid";
$TypeActiviteit[18] = "Taken";
$TypeActiviteit[19] = "Groepsindeling";
$TypeActiviteit[20] = "Stamgegevens";
$TypeActiviteit[21] = "Afdelingskalender";
$TypeActiviteit[22] = "Stukken";
$TypeActiviteit[23] = "Eigen lijsten";
$TypeActiviteit[24] = "Afwezigheid/presentie";
$TypeActiviteit[97] = "Foutieve login";
$TypeActiviteit[98] = "Performance";
$TypeActiviteit[99] = "Debug- en foutmeldingen";
asort($TypeActiviteit);

define("DB_NAME", $db_name);
$db_conn_pdo = sprintf("mysql:host=%s;dbname=%s", $db_host, DB_NAME);
try {
	$dbc = new PDO($db_conn_pdo, $db_user, $db_pass, array(PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES utf8;SET session wait_timeout=300;"));
	$dbc->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_OBJ);
	$dbc->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {
	printf("<p>Verbinding naar database '%s' mislukt: %s</p>\n", DB_NAME, $e->getMessage());
}

$i_base = new cls_db_base();
if ($i_base->bestaat_tabel("Lid") == false or $i_base->bestaat_tabel("Lidond") == false) {
	db_createtables();
	db_onderhoud(2);
}
$i_base = null;

class cls_db_base {
	public $table = "";				// Naam van de tabel
	private $alias = "";
	public $basefrom = "";			// Naam van de tabel met alias
	private $refcolumn = "";		// Naam van de kolom bij een update
	public $pkkol = "RecordID";		// Naam van de kolom met de primary key
	private $aantalkolommen = -1;	// Het aantal kolommen in het SQL-statement
	private $aantalrijen = -1;	// Het aantal rijen in het SQL-statement
	public $mess = "";				// Boodschap in de logging
	public $ta = 0;					// Type activiteit van de logging
	public $tas = 0;				// Type activiteit specifiek van de logging
	public $tm = 0; 				// Toon boodschap 1 = ja, 0 = nee, 2 = alleen voor webmasters
	public $lidid = 0;				// RecordID van het lid
	public $query = "";				// De SQL-code die moet worden uitgevoerd.
	public $where = "";
	
	public $fdlang = "'%e %M %Y'";
	public $fdtlang = "'%e %M %Y (%H:%i)'";
	public $fdkort = "'%e-%c-%Y'";
	
	public $db_language = "'nl_NL'";
	
	public $fromlid = TABLE_PREFIX . "Lid AS L INNER JOIN " . TABLE_PREFIX . "Lidmaatschap AS LM ON L.RecordID=LM.Lid";
	public $fromlidond = '(' . TABLE_PREFIX . 'Lid AS L INNER JOIN ((' . TABLE_PREFIX . 'Lidond AS LO INNER JOIN ' . TABLE_PREFIX . 'Functie AS F ON LO.Functie=F.Nummer) INNER JOIN ' . TABLE_PREFIX . 'Onderdl AS O ON LO.OnderdeelID=O.RecordID) ON L.RecordID=LO.Lid) LEFT JOIN (' . TABLE_PREFIX . 'Groep AS GR LEFT JOIN ' . TABLE_PREFIX . 'Activiteit AS Act ON Act.RecordID=GR.ActiviteitID) ON LO.GroepID=GR.RecordID';

	public $wherelid = "LM.LIDDATUM <= CURDATE() AND IFNULL(LM.Opgezegd, '9999-12-31') >= CURDATE()";
	public static $wherelidond = "LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE()";
	public $selectnaam = "CONCAT(IF(LENGTH(IFNULL(L.Roepnaam, ''))>1, L.Roepnaam, IFNULL(L.Voorletter, '')), ' ', IF(IFNULL(L.Tussenv, '')>'', CONCAT(L.Tussenv, ' '), ''), L.Achternaam)";
	public $selectzoeknaam = "TRIM(CONCAT(L.Achternaam, ', ', IF(L.Tussenv>'', CONCAT(L.Tussenv, ' '), ''), IF(LENGTH(L.Roepnaam)>1, L.Roepnaam, L.Voorletter), ' '))";
	public $selectavgnaam = "CONCAT(IFNULL(L.Roepnaam, ''), ' ', IF(IFNULL(L.Tussenv, '')>'', CONCAT(L.Tussenv, ' '), ''), SUBSTRING(L.Achternaam, 1, 1), '.')";
	public $selectgeslacht = "";
	public $selectleeftijd = "IF(ISNULL(L.GEBDATUM) OR L.GEBDATUM < '1910-01-01' OR (NOT ISNULL(L.Overleden)), NULL, CONCAT(TIMESTAMPDIFF(YEAR, L.GEBDATUM, CURDATE()), ' jaar'))";
	public $selectlidnr = "SELECT MAX(IFNULL(Lidnr, 0)) FROM " . TABLE_PREFIX . "Lidmaatschap AS LM WHERE LM.Lid=L.RecordID AND LM.LIDDATUM <= CURDATE() AND IFNULL(LM.Opgezegd, '9999-12-31') >= CURDATE()";
	public $selecttelefoon = "IF(LENGTH(IFNULL(L.Mobiel, '')) < 10, IFNULL(L.Telefoon, ''), L.Mobiel)";
	public $selectemail = "IF(LENGTH(L.Email) > 5, L.Email, IF(LENGTH(L.EmailOuders) > 5, L.EmailOuders, L.EmailVereniging))";
	public $selectgroep = "CASE 
					WHEN IFNULL(LO.GroepID, 0)=0 AND LO.Functie=0 THEN 'Niet ingedeeld'
					WHEN LENGTH(Instructeurs) > 1 THEN CONCAT(GR.Omschrijving, ' | ', GR.Instructeurs)
					ELSE GR.Omschrijving
					END";
	public $selectgebdatum = "";
	public $selectmhaan = "";
	
	function __construct($p_table="") {
				
		if (strlen($p_table) > 0) {
			$this->table = TABLE_PREFIX . $p_table;
		}
		
		$this->selectgeslacht = "CASE L.Geslacht ";
		foreach (ARRGESLACHT as $c => $o) {
			$this->selectgeslacht .= sprintf("WHEN '%s' THEN '%s' ", $c, $o);
		}
		$this->selectgeslacht .= "ELSE 'Onbekend' END";
		$this->selectgebdatum = sprintf("IF(ISNULL(L.GEBDATUM), '', DATE_FORMAT(L.GEBDATUM, %s, %s))", $this->fdlang, $this->db_language);
		$this->selectmhaan = sprintf("CONCAT(IF(MH.LidID=0, '', CONCAT(%s, ' & ')), MH.to_addr, IF(LENGTH(IFNULL(MH.cc_addr, '')) < 5, '', CONCAT(' & CC: ', MH.cc_addr)))", $this->selectnaam);
	}
		
	public function execsql($p_query="", $p_logtype=-1, $p_debug=0) {
		global $dbc;
		
		if (strlen($p_query) > 5) {
			$this->query = $p_query;
		}
		if ($p_debug == 1) {
			debug($this->query, 1);
		} elseif ($p_debug == 2) {
			debug($this->query, 0, 1);
		}
		
		if (strlen($this->query) > 5) {
			$starttijd = microtime(true);
			$mess = "";

			try {
				if (startwith($this->query, "SELECT ")) {
					$rv = $dbc->query($this->query);
					$exec_tijd = microtime(true) - $starttijd;
					if (isset($_SESSION['settings']['performance_trage_select']) and $_SESSION['settings']['performance_trage_select'] > 0 and $exec_tijd >= $_SESSION['settings']['performance_trage_select']) {
						$mess = sprintf("%.3f seconden query: %s", $exec_tijd, $this->query);
						$p_logtype = 98;
					} elseif ($p_logtype > 0) {
						$mess = sprintf("Uitgevoerde query: %s", $this->query);
					}
					$this->aantalkolommen = $rv->ColumnCount();
					$this->aantalrijen = $rv->RowCount();
			
				} elseif (startwith($this->query, "INSERT ")) {
					$rv = $dbc->prepare($this->query)->execute();
					if ($dbc->lastInsertID() > 0) {
						$rv = $dbc->lastInsertID();
					}
					$mess = sprintf("Uitgevoerd: %s / nieuw ID: %d", $this->query, $rv);

				} else {
					$result = $dbc->prepare($this->query);
					$result->execute();
					$rv = $result->RowCount();
					if (startwith($this->query, "UPDATE") or startwith($this->query, "DELETE ")) {
						if ($rv > 0) {
							$mess = sprintf("Uitgevoerd: %s / records affected: %d", $this->query, $rv);
						} else {
//							$mess = sprintf("Uitgevoerd zonder resultaat: %s", $this->query);
						}
					} else {
						$mess = sprintf("Uitgevoerd: %s", $this->query);
					}
					$result = null;
				}
				if ($p_logtype > 0 and strlen($mess) > 0) {
					(new cls_Logboek())->add($mess, $p_logtype);
				}
				return $rv;
		
			} catch (Exception $e) {					
				$mess = sprintf("Error in SQL '%s': %s", $this->query, $e->getMessage());
				debug($mess, 2, 0);
				return false;
			}
				
		} else {
			debug("Het uit te voeren SQL-statement is leeg.", 2, 1);
			return false;
		}
	}
	
	public function scalar($p_query, $p_debug=0) {
		global $dbc;
		
		$_SESSION['settings']['performance_trage_select'] = $_SESSION['settings']['performance_trage_select'] ?? 0;
		$starttijd = microtime(true);
		$rv = false;
		
		try {
			$scalarres = $this->execsql($p_query);
			$rv = $scalarres->fetchColumn();
			if ($p_debug == 1) {
				$mess = sprintf("%s: %d", $p_query, $rv);
				debug($mess, 0, 1);
			}
		
		} catch (Exception $e) {
			$this->mess = sprintf("Probleem met uitvoeren van '%s': %s", $p_query, $e->getMessage());
			$this->log(0, 2);
		}
		
		return $rv;
	}

	public function uniekelijst($p_kols, $p_filter="") {
		
		if (strlen($p_filter) > 0) {
			$p_filter = "WHERE " . $p_filter;
		}
		
		$query = sprintf("SELECT DISTINCT %1\$s FROM %2\$s %3\$s ORDER BY %1\$s;", $p_kols, $this->basefrom, $p_filter);
		$result = $this->execsql($query);
		
		return $result->fetchAll();
	}
	
	public function alaanwezig() {
	}
	
	public function bestaat_tabel($p_table) {
		if (strlen(TABLE_PREFIX) > 1 and startwith($p_table, TABLE_PREFIX) == FALSE) {
			$this->table = TABLE_PREFIX . $p_table;
		} else {
			$this->table = $p_table;
		}
	
		$query = sprintf("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema='%s' AND table_name='%s';", DB_NAME, $this->table);
		if ($this->scalar($query) > 0) {
			return true;
		} else {
			return false;
		}
	}
	
	public function bestaat_kolom($p_kolom, $p_table="") {
		
		if (strlen($p_table) > 0) {
			if (startwith($p_table, TABLE_PREFIX)) {
				$this->table = $p_table;
			} else {
				$this->table = TABLE_PREFIX . $p_table;
			}
		}
		
		$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE '%s';", DB_NAME, $this->table, $p_kolom);
		if ($this->scalar($query) == 1) {
			return true;
		} else {
			return false;
		}
	}
	
	public function typekolom($p_kolom) {
		
		$p = strpos($p_kolom, ".");
		if ($p !== false and $p > 0) {
			$p_kolom = substr($p_kolom, $p+1);
		}
		
		$query = sprintf("SELECT DATA_TYPE FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE '%s';", DB_NAME, $this->table, $p_kolom);
		$rv = $this->scalar($query);
		return $rv;
	}
	
	public function lengtekolom($p_kolom) {		
		$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE '%s';", DB_NAME, $this->table, $p_kolom);
		return $this->scalar($query);
	}
	
	public function scalekolom($p_kolom) {
		$query = sprintf("SELECT NUMERIC_SCALE FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE '%s';", DB_NAME, $this->table, $p_kolom);
		return $this->scalar($query);
	}
	
	public function is_kolom_tekst($p_kolom, $p_type="") {
		
		$txt_types = array("char", "longtext", "text", "varchar");
		
		if (strlen($p_type) > 2 and in_array($p_type, $txt_types)) {
			return true;
		} elseif (in_array($this->typekolom($p_kolom), $txt_types)) {
			return true;
		} else {
			return false;
		}
	}
	
	public function is_kolom_numeriek($p_kolom, $p_type="") {

		$num_types = array("bigint", "decimal", "int", "smallint", "tinyint");
		
		if (strlen($p_type) > 2 and in_array($p_type, $num_types)) {
			return true;
		} elseif (in_array($this->typekolom($p_kolom), $num_types)) {
			return true;
		} else {
			return false;
		}
	}
	
	public function nieuwrecordid($p_min=1) {
		/* Change log
			ST20181221 RecordID's voor tabellen die uit Access komen moeten oneven zijn. Dit om het onderscheid te kunnen maken met RecordID's die uit de Access-database komen.
			ST20191225 RecordID's voor tabellen die bij elkaar horen uniek binnen die groep gemaakt.
			ST20200327 alleen recordid's van tabellen die ook in de Access-database staan moeten oneven zijn. Hierdoor lopen de nummers minder snel op.
		*/
	
		global $arrTables;
	
		$tabel = str_replace(TABLE_PREFIX, "", $this->table);
	
		$rid = $this->scalar(sprintf("SELECT IFNULL(MAX(O.RecordID), 1) FROM %sOnderdl AS O;", TABLE_PREFIX));
		$r = $this->scalar(sprintf("SELECT IFNULL(MAX(DP.RecordID), 0) FROM %sDiploma AS DP;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		$r = $this->scalar(sprintf("SELECT IFNULL(MAX(EL.RecordID), 0) FROM %sEigen_lijst AS EL;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		
		if ($tabel == "Organisatie") {
			$rid = 1;
		} elseif ($tabel == "Seizoen") {
			$rid = 2000;
		} elseif (startwith($tabel, "Evenement")) {
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sEvenement;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sEvenement_Deelnemer;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sEvenement_Type;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
		
		} elseif ($tabel == "Lid" or $tabel == "Lidmaatschap" or $tabel == "Memo") {
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sLid;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sLidmaatschap;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sMemo;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
		
		} elseif ($tabel == "Mailing"  or $tabel == "Mailing_hist" or $tabel == "Mailing_rcpt") {
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(MailingID), 0) FROM %sMailing;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sMailing;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sMailing_hist;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %sMailing_rcpt;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
				
		} elseif ($tabel == "Liddipl") {
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(LD.RecordID), 0) FROM %sLiddipl AS LD;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }

		} elseif ($tabel == "Lidond" or $tabel == "Groep" or $tabel == "sAfdelingskalender") {
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(LO.RecordID), 0) FROM %sLidond AS LO;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(AK.RecordID), 0) FROM %sAfdelingskalender AS AK;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(GR.RecordID), 0) FROM %sGroep AS GR;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
		
		} elseif ($tabel == "Rekening") {
			$rid = $this->scalar(sprintf("SELECT IFNULL(MAX(RK.Nummer), 0) FROM %sRekening AS RK;", TABLE_PREFIX));
		
		} elseif ($tabel == "Rekreg" or $tabel == "RekeningBetaling") {
			$rid = $this->scalar(sprintf("SELECT IFNULL(MAX(RR.RecordID), 0) FROM %sRekreg AS RR;", TABLE_PREFIX));
			
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(RB.RecordID), 0) FROM %sRekeningBetaling AS RB;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
		
		} elseif ($tabel == "WS_Orderregel" or $tabel == "WS_Artikel" or $tabel == "WS_Voorraadboeking") {
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(Art.RecordID), 0) FROM %sWS_Artikel AS Art;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(Ord.RecordID), 0) FROM %sWS_Orderregel AS Ord;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(VB.RecordID), 0) FROM %sWS_Voorraadboeking AS VB;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
		
		}

		if ($this->bestaat_tabel($this->table)) {
			$query = sprintf("SELECT MAX(IFNULL(%s, 0)) FROM %s;", $this->pkkol, $this->table);
			$r = $this->scalar($query);
			if ($r > $rid) {$rid = $r; }
		}
		
		$rid++;
		if ($rid < $p_min) {
			$rid = $p_min;
		}
		if (($rid / 2) == intval($rid / 2) and (array_search($tabel, $arrTables) >= 30 or $tabel == "Eigen_lijst") and $tabel != "Seizoen") {
			$rid++;
		}
		return $rid;
		
	}
	
	public function waarde($p_recid, $p_kolom) {
	}
	
	public function aantalkolommen($p_sql) {
		$this->aantalkolommen = $this->execsql($p_sql)->ColumnCount();
		return $this->aantalkolommen;
	}
		
	public function aantal($p_filter="") {
		
		if (strlen($p_filter) > 0) {
			$p_filter = "WHERE " . $p_filter;
		}
		if (strlen($this->basefrom) == 0) {
			$this->basefrom = $this->table;
		}
		
		if (strlen($this->basefrom) > 0) {
			$query = sprintf("SELECT COUNT(*) FROM %s %s;", $this->basefrom, $p_filter);
			return $this->scalar($query);
		} else {
			debug("Geen tabel bekend.", 2, 1);
			return false;
		}
	}
	
	public function min($p_kolom, $p_filter="") {
		if ($this->is_kolom_numeriek($p_kolom) == true) {
			$query = sprintf("SELECT MIN(IFNULL(%1\$s, 0)) FROM %2\$s WHERE (%1\$s IS NOT NULL)", $p_kolom, $this->basefrom);
		} else {
			$query = sprintf("SELECT MIN(IFNULL(%1\$s, '')) FROM %2\$s WHERE (%1\$s IS NOT NULL)", $p_kolom, $this->basefrom);
		}
		if (strlen($p_filter) > 0) {
			$query .= " AND " . $p_filter;
		}
		$query .= ";";
		$result = $this->execsql($query);
		return $result->fetchColumn();
	}
	
	public function max($p_kolom, $p_filter="") {
		$tk = $this->typekolom($p_kolom);
		
		if ($tk == "date") {
			$query = sprintf("SELECT IFNULL(MAX(%s), '') FROM %s", $p_kolom, $this->basefrom);
		} elseif ($this->is_kolom_numeriek($p_kolom) == true) {
			$query = sprintf("SELECT IFNULL(MAX(%s), 0) FROM %s", $p_kolom, $this->basefrom);
		} elseif ($this->bestaat_kolom($p_kolom) == true) {
			$query = sprintf("SELECT MAX(IFNULL(%s, '')) FROM %s", $p_kolom, $this->basefrom);
		} else {
			$query = sprintf("SELECT MAX(IFNULL(%s, '')) FROM %s", $p_kolom, $this->basefrom);
		}
		if (strlen($p_filter) > 0) {
			$query .= " WHERE " . $p_filter;
		}
		$query .= ";";
		$result = $this->execsql($query);
		return $result->fetchColumn();
	}
		
	public function totaal($p_kolom, $p_filter="") {
		$query = sprintf("SELECT SUM(IFNULL(%s, 0)) FROM %s", $p_kolom, $this->basefrom);
		if (strlen($p_filter) > 0) {
			$query .= " WHERE " . $p_filter;
		}
		$query .= ";";
		$result = $this->execsql($query);
		return $result->fetchColumn();
	}
	
	public function uniekewaarden($p_kolom) {
		
		$this->query = sprintf("SELECT DISTINCT %s FROM %s;", $p_kolom, $this->basefrom);
		$result = $this->execsql();
		return $result->fetchAll();
	}
	
	public function versiedb() {
		return $this->execsql("SELECT Version() AS Version;")->fetchColumn();
	}
	
	public function setcharset() {
		$this->execsql("SET CHARACTER SET utf8;");
	}
	
	public function pdoupdate($p_recid, $p_kolom, $p_waarde, $p_log=0) {
		global $dbc, $arrTables;
		
		$this->refcolumn = $p_kolom;
		$rv = false;
		
//		$mess = sprintf("%d / %s / %s", $p_recid, $p_kolom, $p_waarde);
//		debug($mess, 0, 1);
		
		if ($p_recid > 0 and strlen($p_kolom) > 0 and strlen($this->table) > 0) {
		
			$tk = $this->typekolom($p_kolom);
			if (($tk == "date" or $tk == "datetime") and (strlen($p_waarde) < 8 or $p_waarde <= "1900-01-01")) {
				$p_waarde = "NULL";
			} elseif ($tk == "time" and strlen($p_waarde) < 3) {
				$p_waarde = "NULL";
			} elseif (strlen($p_waarde) == 0 and $this->is_kolom_numeriek("", $tk)) {
				$p_waarde = 0;
			} elseif ($tk == "decimal") {
				$s = $this->scalekolom($p_kolom);
				$p_waarde = str_replace(",", ".", $p_waarde);
				if ($s > 0) {
					$p_waarde = round(floatval($p_waarde), $s);
				} else {
					$p_waarde = floatval($p_waarde);
				}
			}
		
			if ($this->bestaat_kolom("Gewijzigd")) {
				$gw = ", Gewijzigd=SYSDATE()";
			} else {
				$gw = "";
			}
			if ($this->bestaat_kolom("GewijzigdDoor")) {
				if (strlen($gw) > 0) {
					$gw .= ", ";
				}
				$gw .= sprintf("GewijzigdDoor=%d", $_SESSION['lidid']);
			}
			if ($p_waarde === "NULL") {
				$xw = sprintf("(`%s` IS NOT NULL)", $p_kolom);
			
			} elseif ($this->is_kolom_numeriek($p_kolom) == true) {
				$xw = sprintf("IFNULL(`%s`, 0)<>:nw", $p_kolom);

			} elseif ($this->typekolom($p_kolom) == "date") {
				$xw = sprintf("IFNULL(`%s`, '')<>:nw", $p_kolom);

			} elseif($this->typekolom($p_kolom) == "text" or $this->typekolom($p_kolom) == "longtext" or $this->typekolom($p_kolom) == "longblob") {
				$xw = sprintf("(IFNULL(`%s`, '') NOT LIKE :nw)", $p_kolom);

			} else {
				$p_waarde = str_replace("\"", "'", $p_waarde);
				$xw = sprintf("BINARY IFNULL(`%s`, '')<>:nw", $p_kolom, $p_waarde);
			}
			$query = sprintf("UPDATE %s SET `%s`=:nw %s WHERE `%s`=:id AND %s;", $this->table, $p_kolom, $gw, $this->pkkol, $xw);
			$stmt = $dbc->prepare($query);
			if ($p_waarde === "NULL") {
				$n = null;
				$stmt->bindValue(":nw", $n, PDO::PARAM_NULL);
			
			} elseif ($this->is_kolom_numeriek($p_kolom)) {
				$stmt->bindValue(":nw", $p_waarde);
			
			} elseif($this->typekolom($p_kolom) == "text" or $this->typekolom($p_kolom) == "longblob") {
				$stmt->bindValue(":nw", $p_waarde, PDO::PARAM_LOB);
			
			} else {
				$stmt->bindValue(":nw", $p_waarde, PDO::PARAM_STR);
			}
			$stmt->bindValue(":id", $p_recid);
		
			if ($stmt->execute()) {
				$effectedRows = $stmt->rowCount();
				$t = str_replace(TABLE_PREFIX, "", $this->table);
				if ($effectedRows > 0) {
					$kt = $this->typekolom($p_kolom);
					if (array_search($t, $arrTables) >= 30 and $_SESSION['settings']['interface_access_db'] == 1) {
						
						if ($kt == "date" and $p_waarde !== "NULL") {
							$p_waarde = "#" . date("m/d/Y", strtotime($p_waarde)) . "#";
							
						} elseif ($kt == "datetime" and $p_waarde !== "NULL") {
							$p_waarde = "#" . date("m/d/Y H:i:s", strtotime($p_waarde)) . "#";
							
						} elseif ($p_kolom == "MySQL") {
							$p_waarde = str_replace(";", "", $p_waarde);
							$p_waarde = "\"" . $p_waarde . "\"";
							
						} elseif ($this->is_kolom_tekst($p_kolom, $kt)) {
							$p_waarde = "\"" . str_replace("\"", "'", $p_waarde) . "\"";
						} elseif ($kt == "decimal" and (strlen($p_waarde) > 1)) {
							$p_waarde = str_replace(",", ".", $p_waarde);
						
						} elseif ($this->is_kolom_numeriek($p_kolom, $kt) == false and strpos($p_waarde, "'") !== false) {
							$p_waarde = "\"" . $p_waarde . "\"";
							
						} elseif ($this->is_kolom_numeriek($p_kolom, $kt) == false) {
							$p_waarde = "'" . $p_waarde . "'";
						}
					
						$sql = sprintf("UPDATE [%s] SET [%s]=%s, Gewijzigd=#%s# WHERE %s=%d;", $t, $p_kolom, $p_waarde, date("m/d/Y H:i:s"), $this->pkkol, $p_recid);
						(new cls_Interface())->add($sql, $this->lidid);
					} else {
						if ($this->is_kolom_numeriek($p_kolom, $kt) == false) {
							$p_waarde = "'" . $p_waarde . "'";
						}
					}
					if ($kt == "date" or $kt == "datetime") {
						$p_waarde = str_replace("#", "'", $p_waarde);
					}
					$this->mess = sprintf("Tabel %s: van record %d is kolom '%s' in %s gewijzigd", str_replace(TABLE_PREFIX, "", $this->table), $p_recid, $p_kolom, $p_waarde);
					$rv = true;
				}
			} else {
				
			}
		}
		
		return $rv;
	}
	
	public function pdodelete($p_recid, $p_reden="") {
		global $dbc, $arrTables;
		
		$query = sprintf("DELETE FROM `%s` WHERE `%s`=:id;", $this->table, $this->pkkol);
		$stmt = $dbc->prepare($query);
		$stmt->bindValue(":id", $p_recid);
		if ($stmt->execute()) {
			if (strlen($p_reden) > 0) {
				$p_reden = ", omdat " . $p_reden;
			}
			
			$t = str_replace(TABLE_PREFIX, "", $this->table);
			$this->mess = sprintf("Tabel %s: Record %d is verwijderd%s", $t, $p_recid, $p_reden);
			
			if (array_search($t, $arrTables) >= 30 and $_SESSION['settings']['interface_access_db'] == 1) {
				$sql = sprintf("DELETE FROM %s WHERE %s=%d;", $t, $this->pkkol, $p_recid);
				(new cls_Interface())->add($sql, $this->lidid);
			}
			
			return $stmt->rowCount();
		} else {
			return 0;
		}
	}

	public function controleersql($p_query, $p_melding=0) {
		global $dbc;
		
		try {
			$dbc->query($p_query);
		} catch (Exception $e) {
			if ($p_melding == 1) {
				printf("<p class='mededeling'>Foutmelding in SQL: %s.</p>", $e->getMessage());
			}
			return false;
		}
		return true;
	}
	
	public function basislijst($p_filter="", $p_orderby="", $p_fetched=1) {
		if (strlen($p_filter) > 0) {
			$p_filter = "WHERE " . $p_filter;
		}
		if (strlen($p_orderby) > 0) {
			$p_orderby = "ORDER BY " . $p_orderby;
		}
		
		if (strlen($this->basefrom) > 1) {
			$fr = $this->basefrom;
		} else {
			$fr = $this->table;
		}

		$query = sprintf("SELECT * FROM %s %s %s;", $fr, $p_filter, $p_orderby);
		$result = $this->execsql($query);
		
		if ($p_fetched == 1) {
			return $result->fetchAll();
		} else {
			return $result;
		}
	}
	
	public function log($p_refID=0, $p_prtmess=0, $p_autom=0) {
		/* p_prtmess (tonen melding)
			0 = Nee
			1 = ja, altijd
			2 = ja, alleen bij webmasters
			3 = Ja, via popup (alert)
		*/

		if (strlen($this->mess) > 0) {
			if (substr($this->mess, -1) != ".") {
				$this->mess .= ".";
			}
			$lbid = (new cls_Logboek())->add($this->mess, $this->ta, $this->lidid, $p_prtmess, $p_refID, $this->tas, $this->table, $this->refcolumn);
			$this->mess = "";
		}
	}
	
	public function Interface($p_query) {
		(new cls_Interface())->add($p_query, $this->lidid);
	}
	
	public function exporttosql($p_type) {
		/*
			2 = structuur tabel
		*/
		global $dbc;
		
		$rv = "";
		
		if ($p_type == 2) {
			$rv = sprintf("DROP TABLE IF EXISTS `%s`;", $this->table);

			$query = sprintf("SHOW CREATE TABLE %s;", $this->table);
			$result2 = $dbc->prepare($query);  
			$result2->execute();                            
			$row2 = $result2->fetch(PDO::FETCH_NUM);
			$rv .= sprintf("\n\n%s;\n\n", $row2[1]);
		}
		
		return $rv;
		
	}  # exporttosql
	
}  # cls_db_base

class cls_Lid extends cls_db_base {
	
	public $naamlid = "";
	public $zoeknaam = "";
	public $roepnaam = "";
	public $geslacht = "O";
	public $geboortedatum = "1900-01-01";
	public $telefoon = "";
	public $iskader = false;
	public $islid = false;
	public $lidvanaf = "";
	
	function __construct($p_lidid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Lid";
		$this->basefrom = $this->table . " AS L";
		$this->vulvars($p_lidid);
		$this->ta = 6;
	}
	
	private function vulvars($p_lidid=-1) {
		if ($p_lidid < 0) {
			$p_lidid = $this->lidid;
		}
		if ($p_lidid != $this->lidid) {
			$this->lidid = $p_lidid;
			
			$this->naamlid = "";
			$this->zoeknaam = "";
			$this->roepnaam = "";
			$this->geslacht = "O";
			$this->geboortedatum = "1900-01-01";
			$this->telefoon = "";
			$this->lidvanaf = "";
		
			if ($this->lidid > 0) {
				$query = sprintf("SELECT *, %s AS NaamLid, %s AS Zoeknaam, %s AS Telefoon FROM %s WHERE L.RecordID=%d;", $this->selectnaam, $this->selectzoeknaam, $this->selecttelefoon, $this->basefrom, $this->lidid);
				$result = $this->execsql($query);
				$row = $result->fetch();
				if (isset($row->RecordID) and $row->RecordID > 0) {
					$this->naamlid = $row->NaamLid;
					$this->zoeknaam = $row->Zoeknaam;
					if (strlen($row->Roepnaam) == 0) {
						$this->roepnaam = trim($row->Voorletter);
					} else {
						$this->roepnaam = $row->Roepnaam;
					}
					$this->geslacht = $row->Geslacht;
					$this->geboortedatum = $row->GEBDATUM;
					$this->telefoon = $row->Telefoon;
					$lmqry = sprintf("SELECT LM.RecordID, LM.LIDDATUM FROM %sLidmaatschap AS LM WHERE LM.Lid=%d AND LM.LIDDATUM <= CURDATE() AND IFNULL(LM.Opgezegd, '9999-12-31') >= CURDATE();", TABLE_PREFIX, $this->lidid);
					$lmres = $this->execsql($lmqry);
					$lmrow = $lmres->fetch();
					if (isset($lmrow->RecordID)) {
						$this->islid = true;
						$this->lidvanaf = $lmrow->LIDDATUM;
					} else {
						$this->islid = false;
					}
				} else {
					$this->iskader = false;
					$this->islid = false;
				}
			} else {
				$this->iskader = false;
				$this->islid = false;
			}
		}
	}  # vulvars

	public function record($p_lidid=-1) {
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		
		if ($this->lidid > 0) {
			$query = sprintf("SELECT L.*, %s as NaamLid,
							  (SELECT IFNULL(MAX(LM.LIDDATUM), '') FROM %sLidmaatschap AS LM WHERE LM.Lid=L.RecordID AND IFNULL(LM.Opgezegd, '9999-12-31') >= CURDATE()) AS LidVanaf
							  FROM %s WHERE L.RecordID=%d;", $this->selectnaam, TABLE_PREFIX, $this->basefrom, $this->lidid);
			$result = $this->execsql($query);
			$row = $result->fetch();
			if (isset($row->RecordID)) {
				$this->lidvanaf = $row->LidVanaf;
				return $row;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}  # record
	
	public function ledenlijst($p_soortlid=1, $p_ondfilter=-1, $p_ord="", $p_filter="", $p_metadres=0) {
		
		/*
			p_soortlid:
			0 = geen filter
			1 = Lid
			2 = Toekomstig lid
			3 = Voormalig lid
			4 = Kloslid
		*/
		
		$xtraselect = "";
		
		if (toegang("Woonadres_tonen", 0, 0) or $p_metadres == 1) {
			$xtraselect .= ", L.Adres, L.Postcode, L.Woonplaats";
		}
		
		$xtraselect .= sprintf(", CONCAT(%s, ' & ', IFNULL(%s, '')) AS Bereiken", $this->selecttelefoon, $this->selectemail);
		$xtraselect .= sprintf(", IFNULL(%s, '') AS Telefoon", $this->selecttelefoon);
		$xtraselect .= sprintf(", IFNULL(%s, '') AS Email", $this->selectemail);
		$xtraselect .= ", MAX(LM.Lidnr) AS Lidnr";
		if ($p_ondfilter <= 0) {
			$xtraselect .= ", LM.LIDDATUM";
			if ($p_soortlid != 2 and (new cls_Lidmaatschap())->max("LM.Opgezegd", "Opgezegd <= DATE_ADD(CURDATE(), INTERVAL 3 MONTH)") >= date("Y-m-d")) {
				$xtraselect .= ", LM.Opgezegd";
			}
		}
		if ($p_soortlid < 2) {
//			$xtraselect .= sprintf(", (SELECT IFNULL(MAX(O.Naam), '') FROM %1\$sOnderdl AS O INNER JOIN %1\$sLidond AS LO ON O.RecordID=LO.OnderdeelID WHERE LO.Lid=L.RecordID AND O.`Type`='O' AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE()) AS Onderscheiding", TABLE_PREFIX);
		}
		

		if ($p_soortlid >= 1 and $p_soortlid <= 3 and $p_ondfilter == 0) {
			$from = $this->fromlid;
		} else {
			$from = sprintf("%s LEFT JOIN %sLidmaatschap AS LM ON L.RecordID=LM.Lid", $this->basefrom, TABLE_PREFIX);
		}
		
		$filter = "WHERE (L.Verwijderd IS NULL)";
		if ($p_soortlid == 1 and $p_ondfilter == 0) {
			$filter .= " AND " . $this->wherelid;
		} elseif ($p_soortlid == 2) {
			$filter .= " AND LM.LIDDATUM > CURDATE()";
		} elseif ($p_soortlid == 3) {
			$filter .= " AND IFNULL(LM.Opgezegd, '9999-12-31') < CURDATE()";
		} elseif ($p_soortlid == 4) {
			$filter .= " AND (LM.Lid IS NULL)";
		}
		
		if ($p_ondfilter > 0) {
			$filter .= sprintf(" AND L.RecordID IN (SELECT LO.Lid FROM %sLidond AS LO WHERE ", TABLE_PREFIX);
			$filter .= sprintf("LO.OnderdeelID=%d", $p_ondfilter);
			if ($p_soortlid == 1) {
				$filter .= " AND " . $this::$wherelidond . ")";
			} elseif ($p_soortlid == 3) {
				$filter .= " AND IFNULL(LO.Opgezegd, CURDATE()) < CURDATE())";
			} else {
				$filter .= ")";
			}
		}
		if (strlen($p_filter) > 0) {
			$filter .= " AND " . $p_filter;
		}
		
		if (strlen($p_ord) > 0) {
			$p_ord .= ", ";
		}
		
		$query = sprintf("SELECT DISTINCT L.RecordID, %s AS `Naam_lid`%s, L.GEBDATUM, %s AS Zoeknaam, L.Postcode, L.RekeningBetaaldDoor, L.Achternaam, L.TUSSENV, L.Roepnaam, L.RecordID AS LidID, L.Opmerking
					FROM %s
					%s
					GROUP BY L.RecordID
					ORDER BY %sL.Achternaam, L.TUSSENV, L.Roepnaam, LM.LIDDATUM;", $this->selectnaam, $xtraselect, $this->selectzoeknaam, $from, $filter, $p_ord);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function kl22osledenlijst($p_filter="", $p_xtraselect="", $p_ord="") {
		
		$opg = "";
		$k3 = sprintf(", CONCAT(%s, ' & ', %s) AS Bereiken", $this->selecttelefoon, $this->selectemail);
//		$k4 = sprintf("(SELECT GROUP_CONCAT(DISTINCT O.Kode SEPARATOR '/') FROM %1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON O.RecordID=LO.OnderdeelID WHERE LO.Lid=L.RecordID AND IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE()) AS `Ond.`", TABLE_PREFIX);
		if (strlen($p_filter) > 0) {
			$p_filter = "AND " . $p_filter;
		}

		if (strlen($p_ord) > 0) {
			$p_ord .= ", ";
		}
		
		$query = sprintf("SELECT L.RecordID, %s AS `Naam_lid` %s, L.Opmerking %s %s, L.GEBDATUM AS GEBDATUM, %s AS Zoeknaam
					FROM %s AS L LEFT OUTER JOIN %sLidmaatschap AS LM ON L.RecordID=LM.Lid
					WHERE (LM.Lid IS NULL) AND (L.Verwijderd IS NULL) %s
					ORDER BY %sL.Achternaam, L.TUSSENV, L.Roepnaam, LM.LIDDATUM;", $this->selectnaam, $k3, $p_xtraselect, $opg, $this->selectzoeknaam, $this->table, TABLE_PREFIX, $p_filter, $p_ord);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function jubilarissen($p_per) {
		
		$ondidkader = $_SESSION['settings']['kaderonderdeelid'] ?? 0;
		
		$sqlm = sprintf("SELECT SUM(TIMESTAMPDIFF(DAY, LM2.LIDDATUM, IFNULL(LM2.Opgezegd, '%1\$s'))) FROM %2\$sLidmaatschap AS LM2 WHERE LM2.Lid=L.RecordID AND LM2.LIDDATUM < '%1\$s' GROUP BY LM2.Lid", $p_per, TABLE_PREFIX);
		if ($ondidkader > 0) {
			$sqkad = sprintf("SELECT IFNULL(SUM(TIMESTAMPDIFF(DAY, LO.Vanaf, IFNULL(LO.Opgezegd, '%1\$s'))), 0) FROM %2\$sLidond AS LO WHERE LO.Lid=L.RecordID AND LO.OnderdeelID=%3\$d", $p_per, TABLE_PREFIX, $ondidkader);
			$sqek = sprintf("SELECT MAX(IFNULL(LO.Opgezegd, '9999-12-31')) FROM %2\$sLidond AS LO WHERE LO.Lid=L.RecordID AND LO.OnderdeelID=%3\$d", $p_per, TABLE_PREFIX, $ondidkader);
		} else {
			$sqkad = "0";
			$sqek = "''";
		}
		
		$query = sprintf("SELECT L.RecordID, %1\$s AS Naam, (%4\$s) AS LengteLidmaatschap, (%5\$s) AS LengteKaderlidmaatschap, (%6\$s) AS EindeKaderlidmaatschap
								FROM %2\$s WHERE LM.LIDDATUM < '%3\$s' AND IFNULL(LM.Opgezegd, '9999-12-31') >= '%3\$s'
								ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", $this->selectnaam, $this->fromlid, $p_per, $sqlm, $sqkad, $sqek);
		$result = $this->execsql($query);
		
		return $result->fetchAll();
	}  #  jubilarissen
	
	public function aantalklosleden() {
		$query = sprintf("SELECT COUNT(*) FROM %s LEFT OUTER JOIN %sLidmaatschap AS LM ON L.RecordID=LM.Lid WHERE (LM.Lid IS NULL);", $this->basefrom, TABLE_PREFIX);		
		return $this->scalar($query);
	}
	
	public function overzichtlid($p_lidid) {
		
		$sqond = sprintf("SELECT MIN(O.Naam) FROM %1\$sOnderdl AS O INNER JOIN %1\$sLidond AS LO ON O.RecordID = LO.OnderdeelID
								WHERE O.`Type` = 'O' AND LO.Lid = L.RecordID AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd >= CURDATE())", TABLE_PREFIX);
		
		$sqllegitimatie = "IF(LENGTH(L.Legitimatienummer) > 4, CONCAT(CASE L.Legitimatietype ";
		foreach (ARRLEGITIMATIE as $t => $o) {
			$sqllegitimatie .= sprintf("WHEN '%s' THEN '%s' ", $t, $o);
		}
		$sqllegitimatie .= "END, ' ', L.Legitimatienummer), NULL)";
		
		if (toegang("woonadres_tonen", 0, 0) or $p_lidid == $_SESSION['lidid']) {
			$adresgegevens = "'Adresgegevens' AS Kop2,
					L.Adres,
					IF(LENGTH(L.Woonplaats) > 1, CONCAT(L.Postcode, '  ', L.Woonplaats), '') AS `Postcode en woonplaats`, ";
		} else {
			$adresgegevens = "";
		}
		$VOG = "`VOG afgegeven` AS 'dteVOG afgegeven op',";
		$sqlbd = sprintf("SELECT MAX(EINDE_PER) FROM %sBewaking WHERE Lid=L.RecordID AND BEGIN_PER < CURDATE()", TABLE_PREFIX);
		$query = sprintf("SELECT CONCAT(`Waarschuwen bij nood`, NamenOuders) FROM %sLid AS L WHERE L.RecordID=%d;", TABLE_PREFIX, $p_lidid);
		if (strlen((new cls_db_base())->scalar($query)) > 3) {
			$wbn = " 'Waarschuwen bij nood' as Kop7";
			$wbn .= ", `NamenOuders` AS `Namen ouders`";
			$wbn .= ", `Waarschuwen bij nood` AS `Contactgegevens`,";
		} else {
			$wbn = "";
		}
		
		$sqllogin = sprintf("SELECT IFNULL(MAX(Login), 'Geen') FROM %sAdmin_login AS AL WHERE LidID=L.RecordID", TABLE_PREFIX);
		$tot = "IFNULL(LM.Opgezegd, CURDATE())";
		$sqlll = sprintf("IF(DATEDIFF(%1\$s, LM.LIDDATUM) < 91, NULL,
						  IF(DATEDIFF(%1\$s, LM.LIDDATUM) < 366, CONCAT(TIMESTAMPDIFF(MONTH, LM.LIDDATUM, IF(ISNULL(LM.Opgezegd), CURDATE(), LM.Opgezegd)), ' maanden'),
						  CONCAT(REPLACE(ROUND(TIMESTAMPDIFF(MONTH, LM.LIDDATUM, %1\$s)/12, 1), '.', ','), ' jaar')))", $tot);
		
		$query = sprintf("SELECT 'Persoonsgegevens' AS Kop1, 
					%s AS Naam, 
					L.Voorletter AS Voorletters, 
					L.Roepnaam AS ndRoepnaam,
					%s AS Geslacht, 
					CONCAT(%s, IF(LENGTH(L.GEBPLAATS) > 1, CONCAT(' te ', L.GEBPLAATS), '')) AS Geboren, 
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
					%s AS `Lengte lidmaatschap`,
					'Overig' AS Kop5,
					L.Bankrekening,
					IF(L.`Machtiging afgegeven`=1, 'Ja', '') AS `Machtiging incasso`,
					%s AS `Legitimatie`,
					L.RelnrRedNed AS `Sportlink ID`,
					(%s) AS `Login website`,
					%s
					L.Beroep AS `Beroep`,
					(%s) AS `dteLaatste bewakingsdag`,
					'Recordinformatie' as Kop7,
					L.RecordID, 
					L.Ingevoerd AS `dteIngevoerd op`,
					L.Gewijzigd AS `dteLaatst gewijzigd op` 
					FROM %s AS L LEFT OUTER JOIN %sLidmaatschap AS LM ON L.RecordID=LM.Lid 
					WHERE L.RecordID=%d 
					ORDER BY LM.LIDDATUM DESC;", $this->selectnaam, $this->selectgeslacht, $this->selectgebdatum, $this->selectleeftijd, $adresgegevens, $sqond, $sqlll, $sqllegitimatie, $sqllogin, $VOG, $sqlbd, $this->table, TABLE_PREFIX, $p_lidid);
		$result = $this->execsql($query);
		return $result->fetch();
		
	}  # overzichtlid
	
	public function lidredned($p_slid="") {
		$query = sprintf("SELECT L.RelnrRedNed, L.TUSSENV, L.Achternaam, L.Roepnaam, L.Voorletter, L.Geslacht, L.GEBDATUM, L.GEBPLAATS, L.Postcode, L.Huisnr,
						  IF(LENGTH(L.Huisletter)>0 AND LENGTH(L.Toevoeging)>0, CONCAT(L.Huisletter, '-', L.Toevoeging), IF(LENGTH(L.Toevoeging) > 0, L.Toevoeging, L.Huisletter)) AS Toev,
						  CONCAT(L.Achternaam, ' - ', L.Meisjesnm) AS Anaam2, L.RecordID
						  FROM %1\$s INNER JOIN (%2\$sLidond AS LO INNER JOIN %2\$sOnderdl AS O ON LO.OnderdeelID=O.RecordID) ON LO.Lid=L.RecordID
						  WHERE (O.ORGANIS=1 OR O.`Type`='B') AND LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE()", $this->basefrom, TABLE_PREFIX);
		if (strlen($p_slid) == 7) {
			$query .= sprintf(" AND UPPER(L.RelnrRedNed)='%s'", strtoupper($p_slid));
		}
		$query .= ";";
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function naam($p_lidid=-1, $p_riz="gast") {
		$this->vulvars($p_lidid);
		
		if ($this->lidid <= 0) {
			$rv = $p_riz;
		} elseif (strlen($this->naamlid) == 0) {
			$rv = "LidID " . $this->lidid;
		} else {
			$rv = $this->naamlid;
		}
		
		return $rv;
		
	}  # Naam
	
	public function roepnaam($p_lidid, $p_riz="gast") {
		$this->vulvars($p_lidid);

		if ($this->lidid <= 0) {
			$rv = $p_riz;
		} elseif (strlen($this->roepnaam) == 0) {
			$rv = "LidID " . $this->lidid;
		} else {
			$rv = $this->roepnaam;
		}
		
		return $rv;
		
	}  # roepnaam
	
	public function Zoeknaam($p_lidid=-1) {
		$thia->vulvars($p_lidid);
		return $this-zoeknaam;
	}
	
	public function Geslacht($p_lidid=-1) {
		$this->vulvars($p_lidid);
		return $this->geslacht;
	}
	
	public function Geboortedatum($p_lidid=-1) {
		$this->vulvars($p_lidid);
		return $this->geboortedatum;
	}
	
	public function woonplaats($p_postcode) {
		if (strlen($p_postcode) < 4) {
			return "";
		} else {
			$postcode = substr($p_postcode, 0, 4);
			$query = sprintf("SELECT IFNULL(Woonplaats, '') FROM %s WHERE SUBSTRING(Postcode, 1, 4)='%s' ORDER BY Gewijzigd DESC;", $this->basefrom, $postcode);
			return $this->scalar($query);			
		}
	}
	
	public function telefoon($p_lidid=-1) {
		$this->vulvars($p_lidid);
		return $this->telefoon;
	}
	
	public function email($p_lidid=-1) {
		if ($p_lidid > 0) {
			$this->lidid = $p_lidid;
		}
		$query = sprintf("SELECT %s FROM %s WHERE L.RecordID=%d;", $this->selectemail, $this->basefrom, $this->lidid);
		return $this->scalar($query);
	}
	
	public function islid($p_lidid=-1, $p_per="") {
		$this->vulvars($p_lidid);
		debug("functie vervangen");
		return (new cls_lidmaatschap())->islid($this->lidid, $p_per);
	}
	
	public function iskader($p_lidid=-1, $p_per="") {
		$this->vulvars($p_lidid);
		$this->lidid = $p_lidid;
		if (strlen($p_per) < 10) {
			$p_per = date("Y-m-d");
		}
		$query = sprintf("SELECT COUNT(*) FROM %1\$s WHERE LO.Lid=%2\$d AND (O.Kader=1 OR F.Kader=1) AND LO.Vanaf <= '%3\$s' AND IFNULL(LO.Opgezegd, '9999-12-31') >= %3\$s;", $this->fromlidond, $this->lidid, $p_per);
		if ($this->scalar($query) > 0) {
			$this->iskader = true;
		} else {
			$this->iskader = false;
		}
		
		return $this->iskader;
	}
	
	public function onderscheiding($p_lidid, $p_per="") {
		$this->vulvars($p_lidid);
		if (strlen($p_per) < 10) {
			$p_per = date("Y-m-d");
		}
		$query = sprintf("SELECT IFNULL(MAX(O.Naam), '') FROM %s WHERE LO.Lid=%d AND O.`Type`='O' AND LO.Vanaf <= '%3\$s' AND IFNULL(LO.Opgezegd, '9999-12-31') >= %3\$s;", $this->fromlidond, $this->lidid, $p_per);
		return $this->scalar($query);
	}
	
	public function lijstselect($p_filter=0, $p_xf="", $p_per="", $p_ondid=-1) {
		
		if (strlen($p_per) < 10) {
			$p_per = date("Y-m-d");
		}
	
		$query = sprintf("SELECT L.RecordID, %s AS NaamLid, %s AS Zoeknaam ", $this->selectnaam, $this->selectzoeknaam);
		if ($p_filter == 1) {
			// Alleen leden
			$query .= sprintf("FROM %1\$s INNER JOIN %2\$sLidmaatschap AS LM ON L.RecordID=LM.Lid WHERE LM.LIDDATUM <= '%3\$s' AND IFNULL(LM.Opgezegd, '9999-12-31') >= '%3\$s'", $this->basefrom, TABLE_PREFIX, $p_per);

		} elseif ($p_filter == 2) {
			// Geen voormalige leden, dus wel leden, klosleden en toekomstige leden
			$query .= sprintf("FROM %s LEFT OUTER JOIN %sLidmaatschap AS LM ON L.RecordID=LM.Lid WHERE ((LM.Lid IS NULL) OR IFNULL(LM.Opgezegd, '999-12-31') >= '%s')", $this->basefrom, TABLE_PREFIX, $p_per);

		} elseif ($p_filter == 3) {
			// Leden die aan een rekening zijn gekoppeld middels BetaaldDoor
			$query .= sprintf("FROM %s AS L WHERE L.RecordID IN (SELECT RK.BetaaldDoor FROM %sRekening AS RK)", $this->table, TABLE_PREFIX);

		} elseif ($p_filter == 4) {
			// Leden die in een niet-afgesloten bewakingsseizoen actief zijn.	
			$query .= sprintf("FROM %1\$s AS L WHERE L.RecordID IN (SELECT BW.Lid FROM %2\$sBewaking AS BW INNER JOIN %2\$sBewseiz AS BS ON BW.SeizoenID=BS.RecordID WHERE BS.Afgesloten=0)", $this->table, TABLE_PREFIX);

		} elseif ($p_filter == 5) {
			// Leden en toekomstige leden
			$query .= sprintf("FROM %s INNER JOIN %sLidmaatschap AS LM ON L.RecordID=LM.Lid WHERE IFNULL(LM.Opgezegd, '999-12-31') >= '%s'", $this->basefrom, TABLE_PREFIX, $p_per);
			
		} elseif ($p_filter == 6) {
			// Alleen Klosleden
			$query .= sprintf("FROM %s LEFT OUTER JOIN %sLidmaatschap AS LM ON L.RecordID=LM.Lid WHERE (LM.Lid IS NULL)", $this->basefrom, TABLE_PREFIX, $p_per);
			
		} elseif ($p_filter == 7) {
			// Iedereen in de tabel Lid
			$query .= sprintf("FROM %s WHERE (L.Verwijderd IS NULL)", $this->basefrom);
			
		} else {
			$query .= sprintf("FROM %s WHERE L.RecordID > 0", $this->basefrom);
		}

		if ($p_ondid > 0) {
			$p_xf = sprintf("L.RecordID IN (SELECT LO.Lid FROM %1\$sLidond AS LO WHERE LO.OnderdeelID=%2\$d AND LO.Vanaf <= '%3\$s' AND IFNULL(LO.Opgezegd, '9999-12-31') >= '%3\$s')", TABLE_PREFIX, $p_ondid, $p_per);
		}

		if (strlen($p_xf) > 0) {
			$query .= " AND " . $p_xf;
		}
		$query .= "	ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;";

		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function htmloptions($p_cv=-1, $p_filter=0, $p_xf="", $p_per="", $p_ondid=-1) {
		if (strlen($p_per) < 10) {
			$p_per = date("Y-m-d");
		}
		$rv = "";
		
		foreach($this->lijstselect($p_filter, $p_xf, $p_per, $p_ondid) as $lid) {
			if ($lid->RecordID == $p_cv) {
				$s = " selected";
			} else {
				$s = "";
			}
			$tw = $lid->Zoeknaam;
			$sl = substr((new cls_Lidmaatschap())->soortlid($lid->RecordID, $p_per), 0, 1);
			if ($p_filter == 0 and $sl != "L") {
				$tw .= sprintf(" (%s)", $sl);
			}
			$rv .= sprintf("<option%s value=%d>%s</option>\n", $s, $lid->RecordID, $tw);
		}
		
		return $rv;
	}
	
	public function lidbijemail($p_email) {
		$p_email = strtolower($p_email);
		$query = sprintf("SELECT IFNULL(L.RecordID, 0) AS LidID, LM.Lidnr, %1\$s AS NaamLid FROM %2\$s AS L INNER JOIN %3\$sLidmaatschap AS LM ON L.RecordID=LM.Lid
						  WHERE IFNULL(LM.Opgezegd, CURDATE()) >= CURDATE() AND (LOWER(L.Email)='%4\$s' OR LOWER(L.EmailOuders)='%4\$s' OR LOWER(L.EmailVereniging)='%4\$s')
						  ORDER BY LM.Lidnr;", $this->selectnaam, $this->table, TABLE_PREFIX, $p_email);
		$rows = $this->execsql($query)->fetchAll();
		if (count($rows) == 1) {
			$this->lidid = $rows[0]->LidID;
		}
		return $rows;
	}
					
	public function verjaardagen($p_datum) {
		
		if (is_numeric($p_datum)) {
			$p_datum = date("Y-m-d", $p_datum);
		}
		
		if ($_SESSION['settings']['agenda_verjaardagen'] > 0) {
			$elrec = (new cls_Eigen_lijst())->record($_SESSION['settings']['agenda_verjaardagen']);
			
			if ($elrec->AantalKolommen == 1) {
				$verjqry = $elrec->MySQL;
				if (substr($verjqry, -1) == ";") {
					$verjqry = substr($verjqry, 0, -1);
				}
				$wl = sprintf("L.RecordID IN (%s)", $verjqry);
			} else {
				(new cls_Parameter())->update("agenda_verjaardagen", 0, "deze eigen lijst niet geschikt is voor de verjaardagen");
				$wl = "1=2";
			}
		} else {
			$wl = "1=2";
		}
		
		$df = sprintf("RIGHT(L.GEBDATUM, 5)='%s'", substr($p_datum, -5));
		$query = sprintf("SELECT L.RecordID, %1\$s AS `Naam_lid`, L.GEBDATUM, (YEAR('%5\$s')-YEAR(L.GEBDATUM))-IF(RIGHT('%5\$s', 5)<RIGHT(L.GEBDATUM, 5), 1, 0) AS Leeftijd
					FROM %2\$s
					WHERE %3\$s AND %4\$s AND L.GEBDATUM > '1901-01-01'
					ORDER BY L.GEBDATUM DESC, L.RecordID;", $this->selectnaam, $this->basefrom, $wl, $df, $p_datum);
					
		return $this->execsql($query)->fetchAll();
	}
	
	public function add($p_achternaam, $p_postcode="") {
		$this->lidid = $this->nieuwrecordid();
		$p_postcode = trim(strtoupper($p_postcode));
		if (strlen($p_postcode) == 6) {
			$p_postcode = substr($p_postcode, 0, 4) . " " . substr($p_postcode, -2);
		}
		$query = sprintf("INSERT INTO %s (RecordID, Achternaam, Ingevoerd) VALUES (%d, \"%s\", NOW());", $this->table, $this->lidid, $p_achternaam);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Kloslid %d (%s) is toegevoegd", $this->lidid, $p_achternaam);
			$this->log($this->lidid);
			(new cls_Interface())->add($query, $this->lidid);
			if (strlen($p_postcode) >= 4) {
				$this->update($this->lidid, "Postcode", $p_postcode);
			}
		} else {
			$this->lidid = 0;
		}
		return $this->lidid;
	}
	
	public function update($p_lidid, $p_kolom, $p_waarde, $p_reden="") {
		$this->lidid = $p_lidid;
		
		$p_waarde = ltrim(trim($p_waarde));
		if ($p_kolom == "Voorletter") {
			$p_waarde = trim(strtoupper($p_waarde));
			if (strlen($p_waarde) > 0 and substr($p_waarde, -1) != ".") {
				$p_waarde .= ".";
			}
			if (strlen($p_waarde) > 2) {
				$p_waarde = str_replace(" ", "", $p_waarde);
			}
		}
		
		if ($p_kolom == "Postcode" and $p_waarde != "NULL") {
			$p_waarde = trim(strtoupper($p_waarde));
			if (strlen($p_waarde) == 6) {
				$p_waarde = substr($p_waarde, 0, 4) . " " . substr($p_waarde, -2);
			}
		}
		
		if ($p_kolom == "Toevoeging" and strlen($p_waarde) > 1 and substr($p_waarde, 0, 1) == "-") {
			$p_waarde = substr($p_waarde, 1, 4);
		}
		
		if ($p_kolom == "Mobiel" and strlen($p_waarde) == 10) {
			$p_waarde = "06-" . substr($p_waarde, -8);
		}
		
		if ($p_kolom == "EmailOuders" and strlen($p_waarde) > 5) {
			$p_waarde = str_replace(";", ",", $p_waarde);
		}
		
		if ($p_kolom == "Bankrekening") {
			$p_waarde = strtoupper($p_waarde);
		}
		
		if ($p_kolom == "Postcode" and strlen($p_waarde) != 0 and strlen($p_waarde) != 7 and substr($p_waarde, 0, 4) < "9999" and substr($p_waarde, 0, 4) >= "1000") {
			$this->mess = "De postcode is niet correct, deze wijziging wordt niet verwerkt.";
			$this->tm = 1;
			
		} elseif (($p_kolom == "Email" or $p_kolom == "EmailVereniging") and strlen($p_waarde) > 0 and isValidMailAddress($p_waarde, 0) == false) {
			$this->mess = sprintf("%s is niet correct, deze wijziging wordt niet verwerkt.", $p_kolom);
			$this->tm = 1;
			
		} elseif ($p_kolom == "GEBDATUM" and $p_waarde > date("Y-m-d")) {
			$this->mess = "De geboortedatum kan niet in de toekomst liggen, deze wijziging wordt niet verwerkt.";
			$this->tm = 1;
			
		} elseif ($p_kolom == "Bankrekening" and strlen($p_waarde) > 0 and strlen($p_waarde) != 18) {
			$this->mess = "Een bankrekening (IBAN) moet uit 18 karakters bestaan, deze wijziging wordt niet verwerkt.";
			$this->tm = 1;
			
		} elseif ($p_kolom == "Bankrekening" and strlen($p_waarde) > 0 and $p_waarde != "NULL" and IsIBANgoed($p_waarde, 1) == false) {
			$this->mess = "Het controlegetal van de bankrekening is niet correct, deze wijziging wordt niet verwerkt.";
			$this->tm = 1;
				
		} elseif ($p_kolom == "Burgerservicenummer" and strlen($p_waarde) > 0 and $p_waarde != "NULL" and (!is_numeric($p_waarde) or $p_waarde < 100000000 or $p_waarde > 999999999)) {
			$this->mess = "Burgerservicenummer is niet correct, deze wijziging wordt niet verwerkt.";
			$this->tm = 1;
			
		} elseif ($p_kolom == "RelnrRedNed" and strlen($p_waarde) != 7 and $p_waarde != "NULL" and strlen($p_waarde) > 0) {
			$this->mess = "Het Sportlink ID moet 7 karakters lang zijn, deze wijziging wordt niet verwerkt.";
			$this->tm = 1;
			
		} else {
			
			if ($p_kolom == "Adres" or $p_kolom == "Postcode") {
				$this->tas = 1;
			} elseif ($p_kolom == "Telefoon" or $p_kolom == "Mobiel") {
				$this->tas = 2;
			} elseif ($p_kolom == "Email" or $p_kolom == "EmailOuders" or $p_kolom == "EmailVereniging") {
				$this->tas = 3;
			} else {
				$this->tas = 4;
			}
			
			if ($this->is_kolom_tekst($p_kolom) == true and $p_waarde == "NULL") {
				$p_waarde = "";
			}
			
			if ($this->pdoupdate($this->lidid, $p_kolom, $p_waarde)) {
				if (strlen($p_reden) > 0) {
					$this->mess .= ", omdat " . $p_reden;
				}
				
				if ($_SESSION['settings']['mailingbijadreswijziging'] > 0 and $p_kolom == "Postcode" and (new cls_Lidmaatschap())->islid($this->lidid)) {
					$mailing = new mailing($_SESSION['settings']['mailingbijadreswijziging']);
					$mailing->Merge($this->lidid);
					$e = $this->email($this->lidid);
					if (isValidMailAddress($e)) {
						$mailing->cc_addr = $e;
					} else {
						$mailing->cc_addr = "";
					}
					if ($mailing->Send()) {
						$this->mess .= " En er is een e-mail in de outbox klaargezet.";
					}
					$mailing = null;
				}
			}
		}
		
		if ($this->tm == 1) {
			$rv = $this->mess;
		} else {
			$rv = "";
		}
		
		$this->log($this->lidid);
		
		return $rv;

	}  # update
	
	public function controle($p_lidid=-1) {
		if ($p_lidid > 0) {
			$f = sprintf("L.RecordID=%d", $p_lidid);
		} else {
			$f = "";
			debug("Controle alle leden", 0, 1);
		}
		$lrows = $this->basislijst($f);
		foreach ($lrows as $lrow) {
			if (strlen($lrow->Email) > 0 and strlen($lrow->EmailOuders) > 0 and $lrow->Email == $lrow->EmailOuders) {
				$this->update($lrow->RecordID, "Email", "", "het emailadres gelijk is aan die van de ouders.");
			} elseif (array_key_exists($lrow->Geslacht, ARRGESLACHT) == false) {
				$this->update($lrow->RecordID, "Geslacht", "O", "geslacht een ongeldige waarde had.");
			} elseif (array_key_exists($lrow->Legitimatietype, ARRLEGITIMATIE) == false) {
				$this->update($lrow->RecordID, "Legitimatietype", "G", "legitimatietype een ongeldige waarde had.");
			} elseif (strlen($lrow->Toevoeging) > 1 and substr($lrow->Toevoeging, 0, 1) == "-") {
				$this->update($lrow->RecordID, "Toevoeging", substr($lrow->Toevoeging, 1, 4), "de toevoeging hoort niet met een streepje te beginnen.");
			}
			
			/*
			$t = substr($lrow->Adres, strrpos(trim($lrow->Adres), " "));
			$hn = 0;
			$hl = "";
			$tv = "";
			if (is_numeric($t) and intval($t) == $t) {
				$hn = intval($t);
			} elseif (strpos($t, "-") !== false) {
				$hn = substr($t, 0, strpos($t, "-"));
				$tv = substr($t, strpos($t, "-")+1);
			} else {
				if (is_numeric(substr($t, -1)) == false) {
					$hl = substr($t, -1);
					$hn = intval(str_replace($hl, "", $t));
				}
			}
			$this->update($lrow->RecordID, "Huisnr", $hn);
			$this->update($lrow->RecordID, "Huisletter", trim(strtoupper($hl)));
			$this->update($lrow->RecordID, "Toevoeging", trim($tv));
			*/
		}
	}
	
	public function opschonen() {
	}
	
}  # cls_Lid

class cls_Lidmaatschap extends cls_db_base {
	
	private $lmid = 0;
	
	function __construct($p_lmid=0, $p_lidid=0) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Lidmaatschap";
		$this->basefrom = $this->table . " AS LM";
		$this->vulvars($p_lmid, $p_lidid);
		$this->ta = 6;
		$this->tas = 8;
	}
	
	private function vulvars($p_lmid, $p_lidid=0) {
		if ($p_lmid >= 0) {
			$this->lmid = $p_lmid;
		}
		if ($this->lmid > 0) {
			$f = sprintf("RecordID=%d", $this->lmid);
			$this->lidid = $this->max("LM.Lid", $f);
		} elseif ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
	}
	
	public function lidid($p_lmid, $p_lidnr=0) {
		if ($p_lidnr > 0) {
			$query = sprintf("SELECT IFNULL(LM.Lid, 0) FROM %s WHERE LM.Lidnr=%d;", $this->basefrom, $p_lidnr);
			$this->lidid = $this->scalar($query);
		} elseIF ($p_lmid > 0) {
			$this->vulvars($p_lmid);
		} else {
			$this->lidid = 0;
		}
		return $this->lidid;
	}
	
	public function islid($p_lidid=-1, $p_per="") {
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		if (strlen($p_per) < 10) {
			$p_per = date("Y-m-d");
		}
		
		$query = sprintf("SELECT COUNT(*) FROM %1\$s WHERE LM.LIDDATUM <= '%2\$s' AND IFNULL(LM.Opgezegd, '9999-12-31') >= '%2\$s' AND LM.Lid=%3\$d;", $this->basefrom, $p_per, $this->lidid);
		if ($this->scalar($query) > 0) {
			return true;
		} else {
			return false;
		}
	}
	
	public function soortlid($p_lidid, $p_per="") {
		if (strlen($p_per) < 10) {
			$p_per = date("Y-m-d");
		}
		
		if ($p_lidid > 0) {			
			$rv = "Lid";
			$query = sprintf("SELECT LM.* FROM %s WHERE LM.Lid=%d ORDER BY LM.LIDDATUM DESC;", $this->basefrom, $p_lidid);
			$result = $this->execsql($query);
			$row = $result->fetch();
			if (isset($row->Lid) and $row->Lid > 0) {
				if ($row->Opgezegd > '1900-01-01' and $row->Opgezegd < $p_per) {
					$rv = "Voormalig lid";
				} elseif ($row->LIDDATUM > $p_per) {
					$rv = "Toekomstig lid";
				}
			} else {
				$query = sprintf("SELECT IFNULL(L.RecordID, 0) FROM %sLid AS L WHERE L.RecordID=%d AND (L.Verwijderd IS NULL);", TABLE_PREFIX, $p_lidid);
				if ($this->scalar($query) > 0) {
					$rv = "Kloslid";
				} else {
					$rv = sprintf("Record %d bestaat niet (meer)", $p_lidid);
				}
			}
		} else {
			$rv = "";
		}
		
		return $rv;
	}
	
	public function lidnummer($p_lidid, $p_riz="geen") {
		$this->lidid = $p_lidid;
		
		$query = sprintf("SELECT IFNULL(MAX(LM.Lidnr), '%s') FROM %s WHERE IFNULL(LM.Opgezegd, '9999-12-31') >= CURDATE() AND LM.Lid=%d;", $p_riz, $this->basefrom, $this->lidid);
		return $this->scalar($query);
		
	}
	
	public function overzichtlid($p_lidid=-1) {
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		$this->query = sprintf("SELECT LM.Lidnr, LM.LIDDATUM, LM.Opgezegd, CONCAT(FORMAT(DATEDIFF(IFNULL(LM.Opgezegd, CURDATE()), LM.LIDDATUM)/365.25, 1, 'nl_NL'), ' jaar') AS Duur
										FROM %s WHERE LM.Lid=%d ORDER BY LM.LIDDATUM;", $this->basefrom, $this->lidid);
		$result = $this->execsql();
		return $result->fetchAll();
	}
	
	public function beginlidmaatschap($p_lidid=-1) {
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		$query = sprintf("SELECT MIN(LM.LIDDATUM) FROM %s WHERE LM.Lid=%d;", $this->basefrom, $this->lidid);
		return $this->scalar($query);
	}
	
	public function eindelidmaatschap($p_lidid=-1) {
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		$query = sprintf("SELECT MAX(IFNULL(LM.Opgezegd, '9999-12-31')) FROM %s WHERE LM.Lid=%d;", $this->basefrom, $this->lidid);
		return $this->scalar($query);
	}
	
	public function kanopzeggen($p_lidid) {
		
		$query = sprintf("SELECT COUNT(*) FROM %s WHERE (LM.Opgezegd IS NULL) AND LM.Lid=%d;", $this->basefrom, $p_lidid);
		if ((new cls_db_base())->scalar($query) > 0) {
			return true;
		} else {
			return false;
		}
	}
		
	public function add($p_lidid, $p_vanaf="") {
		$this->lidid = $p_lidid;
		$lidnr = $this->max("LM.Lidnr") + 1;
		if (($lidnr / 2) == intval($lidnr / 2)) {
			$lidnr++;
		}
		
		$nrid = $this->nieuwrecordid();
		$query = sprintf("INSERT INTO %s (RecordID, Lid, Lidnr, LIDDATUM, Ingevoerd) VALUES (%d, %d, %d, CURDATE(), NOW());", $this->table, $nrid, $this->lidid, $lidnr);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Lidmaatschap %d met lidnummer %d is toegevoegd.", $nrid, $lidnr);	

			$query = sprintf("INSERT INTO %s (RecordID, Lid, Lidnr, LIDDATUM, Ingevoerd) VALUES (%d, %d, %d, CURDATE(), NOW());", $this->table, $nrid, $this->lidid, $lidnr);
			$this->Interface($query);
		}
		$this->Log($nrid);
		
		if ($p_vanaf > "2000-01-01" and strlen($p_vanaf) == 10) {
			$this->update($nrid, "LIDDATUM", $p_vanaf);
		}
	}
	
	public function update($p_lmid, $p_kolom, $p_waarde) {
		$this->vulvars($p_lmid);
				
		$query = sprintf("SELECT COUNT(*) FROM %s AS LM WHERE LM.RecordID<>%d AND LM.Lidnr=%d;", $this->table, $p_lmid, $p_waarde);
		if (strlen($p_waarde) < 5 and $p_kolom == "LIDDATUM") {
			$this->mess = "De datum van lid worden mag niet leeg zijn, wijziging wordt niet verwerkt.";
			$this->tm = 1;
			
		} elseif ((strlen($p_waarde) == 0 or intval($p_waarde)<= 0) and $p_kolom == "Lidnr") {
			$this->mess = "Het lidnummer mag geen 0 zijn, wijziging wordt niet verwerkt.";
			$this->tm = 1;
			
		} elseif ($p_kolom == "Lidnr" and $this->scalar($query) > 0) {
			$this->mess = sprintf("Lidnummer %d is al in gebruik, wijziging wordt niet verwerkt.", $p_waarde);
			$this->tm = 1;
			
		} else {
			$this->pdoupdate($p_lmid, $p_kolom, $p_waarde);
		}
		$this->log($p_lmid);
	}
	
	public function opzegging($p_lidid, $p_per) {
		
		if (strlen($p_per) > 7) {
			$this->query = sprintf("SELECT LM.RecordID FROM %s AS LM WHERE LM.Lid=%d AND (LM.Opgezegd IS NULL);", $this->table, $p_lidid);
			foreach ($this->execsql()->fetchAll() as $row) {
				$this->update($row->RecordID, "Opgezegd", $p_per);
			}
		}
	}
	
	public function controle() {
	}
	
	public function opschonen() {
		
		$query = sprintf("SELECT LM.RecordID FROM %s LEFT JOIN %sLid AS L ON LM.Lid=L.RecordID WHERE (L.RecordID IS NULL);", $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		
		foreach ($result->fetchAll() as $row) {
			$this->lidid = $row->Lid;
			$this->pdodelete($row->RecordID, "het gerelateerde record in de tabel Lid niet bestaat.");
			$this->Log($row->RecordID);
		}
		
		$query = sprintf("SELECT LM.RecordID, LM.Lid FROM %s WHERE LM.LIDDATUM > IFNULL(LM.Opgezegd, '9999-12-31');", $this->basefrom);
		$result = $this->execsql($query);
		
		foreach ($result->fetchAll() as $row) {
			$this->lidid = $row->Lid;
			$this->pdodelete($row->RecordID, "de datum van lid worden na de datum van opzeggen ligt");
			$this->Log($row->RecordID);
		}
	}
	
}  # cls_Lidmaatschap

class cls_Memo extends cls_db_base {
	
	private $mmid = 0;
	private $soort = "";
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Memo";
		$this->basefrom = $this->table . " AS M";
		$this->ta = 6;
		$this->tas = 7;
	}
	
	private function vulvars($p_lidid, $p_soort) {
		$this->lidid = $p_lidid;
		$this->soort = $p_soort;
		
		if ($this->lidid > 0 and strlen($this->soort) > 0) {
			$f = sprintf("Lid=%d AND Soort='%s'", $this->lidid, $this->soort);
			$this->mmid = $this->max("M.RecordID", $f);
		}
	}

	public function inhoud($p_lidid, $p_soort) {
		$this->lidid = $p_lidid;
		$query = sprintf("SELECT Memo FROM %s WHERE Lid=%d AND Soort='%s';", $this->table, $p_lidid, $p_soort);
		return $this->scalar($query);
	}
	
	public function overzichtlid($p_lidid) {
		$this->lidid = $p_lidid;
		
		$query = sprintf("SELECT Soort, Memo FROM %s WHERE Lid=%d;", $this->table, $p_lidid);
		return $this->execsql($query)->fetchAll();		
	}
	
	public function add($p_lidid, $p_soort, $p_waarde) {
		$this->lidid = $p_lidid;
		$nrid = $this->nieuwrecordid();
		
		$query = sprintf("INSERT INTO %s (RecordID, Lid, Soort, Memo, Ingevoerd) VALUES (%d, %d, '%s', \"%s\", NOW());", $this->table, $nrid, $p_lidid, $p_soort, $p_waarde);
		if ($this->execsql($query) > 0) {
			$query = sprintf("INSERT INTO Memo (RecordID, Lid, Soort, Memo, Ingevoerd) VALUES (%d, %d, '%s', \"%s\", NOW());", $nrid, $p_lidid, $p_soort, $p_waarde);
			(new cls_Interface())->add($query, $p_lidid);
			$this->mess = sprintf("Memo '%s' met waarde '%s' is toegevoegd.", ARRSOORTMEMO[$p_soort], $p_waarde);
			$this->log($nrid);
		}
	}
	
	public function update($p_lidid, $p_soort, $p_waarde) {
		$this->vulvars($p_lidid, $p_soort);
		
		$p_waarde = str_replace("\"", "'", $p_waarde);
		
		if ($this->pdoupdate($this->mmid, "Memo", $p_waarde) > 0) {
			$this->log($this->mmid);
		}
	}
	
	public function delete($p_lidid, $p_soort) {
		$this->vulvars($p_lidid, $p_soort);
		
		$this->pdodelete($this->mmid);
		$this->log($this->mmid);
	}
}  # cls_Memo

class cls_Authorisation extends cls_db_base {
	
	public $aid = 0;
	public $naamtp = "";
	public $ondnaam = "";
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Admin_access";
		$this->basefrom = $this->table . " AS AA";
		$this->ta = 15;
	}
	
	private function vulvars($p_aid=-1) {
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
	
	public function recordid($p_tp) {
		$tp = str_replace("'", "", $p_tp);
		$query = sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %s WHERE Tabpage=\"%s\"", $this->table, str_replace("'", "", $tp));
		return $this->scalar($query);
	}
	
	public function lijst($p_distinct="") {
		$query = sprintf("SELECT %s RecordID, Toegang, Tabpage, Ingevoerd, LaatstGebruikt FROM %s ORDER BY Tabpage;", $p_distinct, $this->table);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function toegang($p_tabpage, $p_aid=-1, $p_alleenmenu=0) {
		
		$rv = false;
		
		if ($p_aid > 0) {
			$this->vulvars($p_aid);
			$p_tabpage = $this->naamtp;
		}
		
		if ($_SESSION['webmaster'] == 1) {
			$query = sprintf("SELECT IFNULL(MIN(AA.RecordID), 0) FROM %s WHERE AA.Tabpage='%s';", $this->basefrom, $p_tabpage);
			$aid = $this->scalar($query);
			if (strlen($p_tabpage) > 0 and $aid == 0) {
				$aid = $this->add($p_tabpage);
			}
			$rv = true;
		} else {
			$query = sprintf("SELECT IFNULL(MIN(AA.RecordID), 0) FROM %s WHERE AA.Tabpage='%s' AND AA.Toegang IN (%s);", $this->basefrom, $p_tabpage, $_SESSION['lidgroepen']);
			$aid = $this->scalar($query);
			if ($aid > 0) {
				$rv = true;
			}
		}
		
		if ($rv and $aid > 0 and $p_alleenmenu == 0) {
			$this->update($aid, "LaatstGebruikt", date("Y-m-d"));
		}
		
		return $rv;
	}  # toegang
	
	public function autorisatiesperonderdeel() {
		$query = sprintf("SELECT O.Naam AS Onderdeel, A.Tabpage as `Toegang tot` FROM %sAdmin_access AS A INNER JOIN %1\$sOnderdl AS O ON A.Toegang=O.RecordID ORDER BY O.Naam, A.Tabpage;", TABLE_PREFIX);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function add($p_tabpage) {
		$this->tas = 1;
		$p_tabpage = str_replace("'", "", $p_tabpage);
		$nrid = $this->nieuwrecordid();
		$query = sprintf("INSERT INTO %s (RecordID, Tabpage) VALUES (%d, \"%s\");", $this->table, $nrid, $p_tabpage);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Tabblad '%s' is aan de tabel 'Admin_access' toegevoegd.", $p_tabpage);
			$this->log($nrid);
		} else {
			$nrid = 0;
		}
		$query = sprintf("UPDATE %s SET Toegang=0 WHERE Tabpage='Vereniging/Introductie' AND Toegang<>0;", $this->table);
		$this->execsql($query);
		
		return $nrid;
	}
	
	public function update($p_aid, $p_kolom, $p_waarde) {
		$this->tas = 2;
		if ($this->pdoupdate($p_aid, $p_kolom, $p_waarde) > 0) {
			$this->vulvars($p_aid);
			if ($p_kolom == "Toegang") {
				$this->mess = sprintf("Toegang '%s' is naar groep '%s' aangepast.", $this->naamtp, $this->ondnaam);
				$this->log($p_aid);
			}
		}
	}
	
	public function delete($p_aid, $p_reden="") {
		$this->tas = 3;
		$this->vulvars($p_aid);
		
		if ($this->pdodelete($p_aid) > 0) {
			$this->mess = sprintf("Uit de tabel '%s' is record %d (%s) verwijderd", $this->table, $p_aid, $this->naamtp);
			$this->log($p_aid);
		}
	}
	
	public function opschonen() {
		$query = sprintf("SELECT RecordID FROM %s WHERE LaatstGebruikt < DATE_ADD(CURDATE(), INTERVAL -3 MONTH) AND Toegang < 0;", $this->table);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID);
		}
	}
	
}  # cls_Authorisation

class cls_Bewaking extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Bewaking";
		$this->basefrom = $this->table . " AS BW";
	}
	
	public function overzichtlid($p_lidid) {
		$w = sprintf("BW.Lid=%d", $p_lidid);
		if ($_SESSION['settings']['tonentoekomstigebewakingen'] == 0) {
			$w .= " AND BW.EINDE_PER < CURDATE()";
		}
		
		$query = sprintf("SELECT MAX(Post) FROM %s AS BW WHERE %s;", $this->table, $w);
		if (strlen($this->scalar($query)) > 0) {
			$sp = "Post,";
		} else {
			$sp = "";
		}
		
		$query = sprintf("SELECT BS.Kode AS Seizoen,
								Weeknr AS intWeek,
								BEGIN_PER AS dteBegin,
								EINDE_PER AS dteEinde,
								DATEDIFF(EINDE_PER, BEGIN_PER) + 1 AS intDagen,
								%1\$s
								F.Omschrijv AS Functie
								FROM %2\$sBewseiz AS BS INNER JOIN (%2\$sBewaking AS BW INNER JOIN %2\$sFunctie AS F ON BW.Functie=F.Nummer) ON BS.RecordID=BW.SeizoenID
								WHERE %3\$s
								ORDER BY BS.Begindatum DESC, BS.Lokatie, BW.BEGIN_PER DESC;", $sp, TABLE_PREFIX, $w);
								
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function lidbew($p_lidid) {
		
		$tw = sprintf("CONVERT((SELECT SUM(DATEDIFF(EINDE_PER, BEGIN_PER)+1) FROM %sBewaking AS BW WHERE BW.Lid = %d AND BEGIN_PER < SYSDATE())/7, SIGNED)", TABLE_PREFIX, $p_lidid);
		$ts = sprintf("(SELECT COUNT(DISTINCT SeizoenID) FROM %sBewaking AS BW WHERE BW.Lid = %d AND BEGIN_PER < CURDATE())", TABLE_PREFIX, $p_lidid);
		$query = sprintf("SELECT CONCAT(%1\$s, ' weken in ', %2\$s, ' seizoen(en)') AS `Totale bewaking`,
								(SELECT Memo FROM %3\$sMemo WHERE Soort='B' AND Lid=%4\$d) AS Opmerking,
								(SELECT Memo FROM %3\$sMemo WHERE Soort='D' AND Lid=%4\$d) AS Dieet;", $tw, $ts, TABLE_PREFIX, $p_lidid);
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
	public function aantallen($p_seizoen) {
		
		$query = sprintf("SELECT ST_LEN_BP FROM %sBewseiz WHERE RecordID=%d;", TABLE_PREFIX, $p_seizoen);
		$result = $this->scalar($query);
		
		if ($result == 7) {
			$query = sprintf("SELECT Weeknr AS intWeeknr, MIN(BEGIN_PER) AS dteBegin, MAX(EINDE_PER) AS dteEinde, ROUND(SUM(DATEDIFF(BW.EINDE_PER, BW.BEGIN_PER)+1)/7, 0) AS intAantal,
							CONCAT(ROUND(AVG((SELECT IFNULL(SUM(DATEDIFF(BH.EINDE_PER, BH.BEGIN_PER)+1)/7, 0) FROM %1\$sBewaking AS BH WHERE BH.Lid = BW.Lid AND BH.EINDE_PER < BW.BEGIN_PER)), 1), ' weken') AS `numGem. Ervaring`,
							CONCAT(ROUND(AVG(DATEDIFF(BW.BEGIN_PER, L.GEBDATUM))/365.25, 1), ' jaar') AS `numGem. Leeftijd`
							FROM %1\$sBewaking AS BW INNER JOIN %1\$sLid AS L ON BW.Lid=L.RecordID
							WHERE BW.Weeknr > 0 AND BW.SeizoenID=%2\$d	
							GROUP BY BW.Weeknr
							ORDER BY BW.Weeknr;", TABLE_PREFIX, $p_seizoen);
		} else {
			$query = sprintf("SELECT BEGIN_PER AS dtlDatum, COUNT(Lid) AS intAantal,
							CONCAT(ROUND(AVG((SELECT IFNULL(SUM(DATEDIFF(BH.EINDE_PER, BH.BEGIN_PER)+1), 0) FROM %1\$sBewaking AS BH WHERE BH.Lid = BW.Lid AND BH.EINDE_PER < BW.BEGIN_PER)), 1), ' dagen') AS `numGem. Ervaring`,
							CONCAT(ROUND(AVG(DATEDIFF(BW.BEGIN_PER, L.GEBDATUM))/365.25, 1), ' jaar') AS `numGem. Leeftijd`
							FROM %1\$sBewaking AS BW INNER JOIN %1\$sLid AS L ON BW.Lid=L.RecordID
							WHERE BW.SeizoenID=%2\$d
							GROUP BY BW.BEGIN_PER
							ORDER BY BW.BEGIN_PER;", TABLE_PREFIX, $p_seizoen);
		}
		
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function rooster($p_seizoen=-1, $p_week=-1) {
		if ($p_seizoen < 1) {
			$sel_sz = "";
			$sel_wk = "";
			$orderby = "BS.Begindatum DESC, BW.BEGIN_PER, BW.Post, F.Sorteringsvolgorde, L.Achternaam, L.Tussenv, L.Roepnaam";
		} else {	
			$sel_sz = sprintf("WHERE BW.SeizoenID=%d", $p_seizoen);
			if ($p_week > 0) {
				$sel_wk = sprintf("AND BW.Weeknr=%d", $p_week);
			} else {
				$sel_wk = "AND BW.Weeknr > 0";
			}
			$this->query = sprintf("SELECT ST_LEN_BP FROM %sBewseiz WHERE RecordID=%d;", TABLE_PREFIX, $p_seizoen);
			$result = $this->execsql();
			if ($result->fetchColumn() == 7) {
				$orderby = "BW.Weeknr, BW.Post, F.Sorteringsvolgorde, BW.BEGIN_PER, L.Achternaam, L.Tussenv, L.Roepnaam";
			} else {
				$orderby = "BW.BEGIN_PER, BW.Post, F.Sorteringsvolgorde, L.Achternaam, L.Tussenv, L.Roepnaam";
			}
		}
		$query = sprintf('SELECT BW.*, %1$s AS NaamBewaker, L.RecordID, F.Omschrijv AS OmsFunctie, F.Afkorting AS AfkFunctie, BS.Kode, BS.Lokatie, BS.ST_LEN_BP, BS.TOONERV,
						DATEDIFF(BW.EINDE_PER, BW.BEGIN_PER)+1 AS Dagen, BW.BEGIN_PER, L.GEBDATUM, 
						(SELECT SUM(DATEDIFF(BH.EINDE_PER, BH.BEGIN_PER)+1) FROM %2$sBewaking BH WHERE BH.Lid=BW.Lid AND BH.EINDE_PER < BW.BEGIN_PER) AS ErvaringDagen
						FROM ((%2$sBewaking AS BW INNER JOIN %2$sBewseiz AS BS ON BS.RecordID = BW.SeizoenID) INNER JOIN %2$sFunctie AS F ON BW.Functie=F.Nummer) INNER JOIN %2$sLid AS L ON L.RecordID=BW.Lid
						%3$s %4$s
						ORDER BY %5$s;', $this->selectnaam, TABLE_PREFIX, $sel_sz, $sel_wk, $orderby);
		return $this->execsql($query)->fetchAll();
	}
	
}  # cls_Bewaking

class cls_Bewakingsseizoen extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Bewseiz";
		$this->basefrom = $this->table . " AS BS";
	}
	
	public function lijst() {			
		$query = sprintf("SELECT BS.RecordID, BS.Kode, BS.Gewijzigd, BS.ST_LEN_BP FROM %s AS BS ORDER BY BS.Begindatum DESC, BS.Kode;", $this->table);
		$result = $this->execsql($query);
		return $result;
	}
	
	public function inschrijven() {
		$query = sprintf("SELECT DISTINCT BS.RecordID, BS.Kode, BS.Gewijzigd, BS.ST_LEN_BP
						  FROM %s AS BS INNER JOIN %sBewaking_Blok AS BB ON BS.RecordID = BB.SeizoenID
						  ORDER BY BS.Begindatum DESC, BS.Kode;", $this->table, TABLE_PREFIX);
		$result = $this->execsql($query);
		return $result;
	}
	
}  #cls_Bewakingsseizoen

class cls_Bewaking_blok extends cls_db_base {
	
	private $blokid = 0;
	private $omsblok = "";
	private $ingebruik = 0;
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Bewaking_Blok";
		$this->basefrom = $this->table . " AS BB";
		$this->ta = 11;
	}
	
	private function vulvars() {
		if ($this->blokid > 0) {
			$query = sprintf("SELECT IFNULL(Omschrijving, '') FROM %s AS BB WHERE BB.RecordID=%d;", $this->table, $this->blokid);
			$omsblok = $this->scalar($query);
			$query = sprintf("SELECT COUNT(*) FROM %sBewaking_Inschrijving AS BI WHERE BI.BlokID=%d;", TABLE_PREFIX, $this->blokid);
			$this->ingebruik = $this->scalar($query);
		} else {
			$this->omsblok = "";
			$this->ingebruik = 0;
		}
	}
	
	public function lijst($p_seizoen=-1, $p_filter=0) {
		
		$w = "";
		if ($p_seizoen > 0) {
			$w = sprintf("WHERE BB.SeizoenID=%d", $p_seizoen);
		}
		if ($p_filter == 1) {
			// alleen open blokken
			if (strlen($w) > 0) {
				$w .= " AND ";
			} else {
				$w = "WHERE ";
			}
			$w .= "BB.InschrijvingOpen=1 AND IFNULL(BB.Eind, CURDATE()) >= CURDATE()";
		}
		$query = sprintf("SELECT BB.*, BS.Kode AS KodeSeizoen, BS.Lokatie AS Locatie, BS.KeuzesBijInschrijving, BB.Kode AS Weeknr, BB.Kode AS intWeeknr,
						  IF(LENGTH(IFNULL(BB.Omschrijving, '')) > 0, BB.Omschrijving, CONCAT('Week ', BB.Kode)) AS OmsBlok,
						  (SELECT COUNT(*) FROM %2\$sBewaking_Inschrijving AS BI WHERE BI.BlokID=BB.RecordID) AS InGebruik
						  FROM %1\$s AS BB LEFT OUTER JOIN %2\$sBewseiz AS BS ON BB.SeizoenID=BS.RecordID
						  %3\$s
						  ORDER BY BS.Begindatum, BS.Kode, BB.Begin;", $this->table, TABLE_PREFIX, $w);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function add($p_seizoen) {
		$b = date("Y-m-d");
		$e = date("Y-m-d");
		$k = "";
		
		$query = sprintf("SELECT DATE_ADD(BB.Eind, INTERVAL 1 DAY) AS B, DATE_ADD(BB.Eind, INTERVAL BS.ST_LEN_BP DAY) AS E, BB.Kode
						  FROM %1\$sBewaking_Blok AS BB INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
						  WHERE BB.SeizoenID=%2\$d AND BB.Eind > '2001-01-01' ORDER BY BB.Eind DESC;", TABLE_PREFIX, $p_seizoen);
		$result = $this->execsql($query);
		$vb = $result->fetch();
		if (isset($vb->B)) {
			$b = $vb->B;
			$e = $vb->E;
			if (strlen($vb->Kode) > 0 and is_numeric($vb->Kode)) {
				$k = $vb->Kode + 1;
			}
		}
		$nrid = $this->nieuwrecordid();
		$this->query = sprintf("INSERT INTO %s (RecordID, SeizoenID, Kode, Begindatum, Eind) VALUES (%d, %d, '%s', '%s', '%s');", $this->table, $nrid, $p_seizoen, $k, $b, $e);
		if ($this->execsql() > 0) {
			$this->mess = sprintf("Bewakingsblok %d in seizoen %d is toegevoegd.", $nrid, $p_seizoen);
			$this->Log($nrid);
		}
	}
	
	public function update($p_blokid, $p_kolom, $p_waarde) {
		$this->blokid = $p_blokid;
		$this->vulvars();
		
		if ($this->pdoupdate($this->blokid, $p_kolom, $p_waarde) > 0) {
			$this->mess = sprintf("In bewakingsblok %d is kolom '%s' naar '%s' gewijzigd.", $this->blokid, $p_kolom, $p_waarde);
			$this->Log($p_blokid);
		}
	}
	
	public function delete($p_blokid) {
		$this->blokid = $p_blokid;
		$this->vulvars();
		
		if ($this->ingebruik > 0) {
			$this->mess = sprintf("Bewakingsblok %d (%s) wordt niet verwijderd, omdat het in gebruik is.", $this->blokid, $this->blokoms);
		} else {
			$this->pdodelete($this->blokid);
		}
		$this->Log($this->blokid);
	}

}  # cls_Bewaking_blok

class cls_InsBew extends cls_db_base {
	
	function __construct() {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Bewaking_Inschrijving";
		$this->basefrom = $this->table . " AS BI";
		$this->ta = 11;
	}
	
	public function record($p_ibid, $p_lidid=0, $p_bbid=0) {
		
		if ($p_ibid > 0) {
			$query = sprintf("SELECT BI.* FROM %s AS BI WHERE BI.RecordID=%d;", $this->table, $p_ibid);
		} else {
			$query = sprintf("SELECT BI.* FROM %s AS BI WHERE BI.Lid=%d AND BI.BlokID=%d;", $this->table, $p_lidid, $p_bbid);
		}
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
	public function overzicht($p_seizoen, $p_week) {
		$w = "";
		if ($p_seizoen > 0) {
			$w = sprintf("BB.SeizoenID=%d", $p_seizoen);
		}
		if ($p_week > 0) {
			if (strlen($w) > 0) {
				$w .= " AND ";
			}
			$w .= sprintf("BB.Kode='%d'", $p_week);
		}
		
		$query = sprintf("SELECT BI.Lid AS LidID, BB.Kode AS Week, BB.Begin AS dteBegin, BB.Eind AS dteEinde, %2\$s AS Bewaker, BI.Keuze,
						  IF(BI.Keuze IN (1, 3), 'Ja', '') AS `Keuze 1`, IF(BI.Keuze IN (2, 3), 'Ja', '') AS `Keuze 2`, BI.Opmerking, BI.Definitief, BI.Ingevoerd
						  FROM (%1\$sBewaking_Blok AS BB INNER JOIN (%4\$s AS BI INNER JOIN %1\$sLid AS L ON BI.Lid=L.RecordID) ON BB.RecordID=BI.BlokID) INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
						  WHERE %3\$s
						  ORDER BY BS.Begindatum, BS.Kode, L.Achternaam, BI.Lid, BB.Begin;", TABLE_PREFIX, $this->selectnaam, $w, $this->table);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function overzichtlid($p_lidid, $p_insnr=0) {
		if ($p_insnr > 0) {
			$xw = sprintf("BI.Nummer=%d", $p_insnr);
		} else {
			$xw = sprintf("BI.Lid=%d", $p_lidid);
		}
		$query = sprintf("SELECT BS.Kode AS Seizoen, BB.Kode AS Week, BB.Begin AS dteBegin, BB.Eind AS dteEinde,
								IF(BI.Keuze=3, 1, BI.Keuze) AS Keuze, BI.Opmerking, %2\$s AS NaamBewaker, L.Roepnaam AS RoepnaamBewaker,
								L.Email AS EmailBewaker, L.EmailOuders, L.GEBDATUM, L.Adres, L.Postcode, L.Woonplaats
								FROM (%1\$sBewaking_Blok AS BB INNER JOIN (%1\$sBewaking_Inschrijving AS BI INNER JOIN %1\$sLid AS L ON BI.Lid=L.RecordID) ON BB.RecordID = BI.BlokID) INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
								WHERE BB.Eind > CURDATE() AND %3\$s AND BI.Definitief > BI.Ingevoerd
								ORDER BY BS.Kode, BB.Kode, BB.Begin;", TABLE_PREFIX, $this->selectnaam, $xw);
								
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function laatstevanlid($p_lidid) {
		$query = sprintf("SELECT IFNULL(MAX(BI.Nummer), 0) FROM %s WHERE BI.Lid=%d;", $this->basefrom, $p_lidid);
		$result = $this->execsql($query);
		return $result->fetchColumn();
	}
	
	public function export() {
		$query = sprintf("SELECT %1\$s AS NaamLid, BI.Lid, BB.SeizoenID, BB.Begin, BB.Eind
							FROM (%2\$s AS BI INNER JOIN %3\$sLid AS L ON BI.Lid=L.RecordID) INNER JOIN %3\$sBewaking_Blok AS BB ON BI.BlokID = BB.RecordID
							WHERE IFNULL(BI.Afgemeld, '0000-00-00') < BI.Ingevoerd AND BI.Definitief > BI.Ingevoerd AND BI.Keuze IN (1, 3)
							ORDER BY L.Achternaam, L.Tussenv, L.Roepnaam, BB.Begin;", $this->selectnaam, $this->table, TABLE_PREFIX);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function matrixaantallen($p_seizoen) {
		$w = "";
		if ($p_seizoen > 0) {
			$w = sprintf("WHERE BB.SeizoenID=%d", $p_seizoen);
		}
		$query = sprintf("SELECT BS.Kode AS Seizoen, BB.Kode AS Week, BB.Begin AS dteBegin, BB.Eind AS dteEinde,
						  SUM(CASE WHEN BI.Definitief > BI.Ingevoerd AND BI.Keuze IN (1, 3) THEN 1 ELSE 0 END) AS `Keuze 1`,
						  SUM(CASE WHEN BI.Definitief > BI.Ingevoerd AND BI.Keuze=2 THEN 1 ELSE 0 END) AS `Keuze 2`,
						  SUM(IF(BI.Definitief <= BI.Ingevoerd, 1, 0)) AS `Niet definitief`,
						  IF(BB.InschrijvingOpen=1, 'Ja', 'Nee') AS `Open?`
						  FROM (%1\$sBewaking_Blok AS BB INNER JOIN (%1\$sBewaking_Inschrijving AS BI INNER JOIN %1\$sLid AS L ON BI.Lid=L.RecordID) ON BB.RecordID = BI.BlokID) INNER JOIN %1\$sBewseiz AS BS ON BB.SeizoenID = BS.RecordID
						  %2\$s
						  GROUP BY BS.Kode, BB.Kode, BB.Begin, BB.Eind, IF(BB.InschrijvingOpen=1, 'Ja', 'Nee')
						  ORDER BY BS.Kode, BB.Kode, BB.Begin;", TABLE_PREFIX, $w);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function add($p_lidid, $p_bbid) {
		$this->lidid = $p_lidid;
		$this->tas = 11;
		$nrid = $this->nieuwrecordid();
		$insnr = $this->max("BI.Nummer") + 3;
		$query = sprintf("INSERT INTO %s (RecordID, Lid, BlokID, Nummer, Ingevoerd) VALUES (%d, %d, %d, %d, CURDATE());", $this->table, $nrid, $this->lidid, $p_bbid, $insnr);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Inschrijving bewaking %d met nummer %d voor lid %d is toegevoegd.", $nrid, $insnr, $this->lidid);
			$this->Log($nrid);
		}
	}
	
	public function update($p_ibid, $p_kolom, $p_waarde) {
		$this->tas = 12;
		
		if ($this->pdoupdate($p_ibid, $p_kolom, $p_waarde) > 0) {
			$this->mess = sprintf("In record %s %d is kolom '%s' in '%s' gewijzigd.", $this->table, $p_ibid, $p_kolom, $p_waarde);
			$this->Log($p_ibid);
		}
	}
	
	public function defmaken($p_lidnr, $p_insnr) {
		$this->query = sprintf("UPDATE %s SET Definitief=SYSDATE() WHERE Nummer=%d AND Lid=%d;", $this->table, $p_insnr, $p_lidid);
		$result = $this->execsql();
		return $result;
	}
			
	public function afmelden() {
		$this->tas = 14;
		
		$this->query = sprintf("UPDATE %s AS BI SET Afgemeld=SYSDATE()
						  WHERE IFNULL(BI.Afgemeld, '0000-00-00') < BI.Ingevoerd AND BI.Definitief > BI.Ingevoerd AND BI.Keuze IN (1, 3);", $this->table);
		$result = $this->execsql();
		$this->mess = sprintf("Er zijn %d inschrijvingen voor bewaking afgemeld.", $result);
		$this->Log(0, 1);
	}
	
	public function delete($p_ibid) {
		$this->tas = 13;
		
		$this->pdodelete($p_ibid);
		$this->Log($p_ibid);
	}
			
}  # cls_InsBew

class cls_Onderdeel extends cls_db_base {
	public $oid = 0;
	public $naam = "";
	public $ondtype = "";
	public $ondtypeoms = "";
	public $alleenleden = 0;			// Mogen van dit onderdeel alleen mensen lid zijn, die ook lid van de vereniging zijn?
	public $iskader = false;			// Zijn de leden van dit onderdeel kaderlid?
	public $isautogroep = false;		// Wordt deze groep automatisch bijgewerkt op basis van MySQL-code of een Eigen query in de Access-database.
	public $magledenmuteren = false;	// Mag de huidige gebruiker de leden van dit onderdeel muteren, op basis van een persooonlijke groep
	private $mogelijketypes = "";
	
	function __construct($p_oid=-1) {
		$this->table = TABLE_PREFIX . "Onderdl";
		$this->basefrom = $this->table . " AS O";
		$this->ta = 20;
		if ($p_oid > 0) {
			$this->vulvars($p_oid);
		}
		foreach (ARRTYPEONDERDEEL as $k => $v) {
			$this->mogelijketypes .= $k;
		}
	}
	
	private function vulvars($p_oid) {
		$this->oid = $p_oid;
		
		$query = sprintf("SELECT O.* FROM %s WHERE O.RecordID=%d;", $this->basefrom, $this->oid);
		$result = $this->execsql($query);
		$row = $result->fetch();
		
		if (isset($row->RecordID)) {
			$this->naam = $row->Naam;
			$this->ondtype = $row->Type;
			$this->ondtypeoms = ARRTYPEONDERDEEL[$row->Type];
			$this->alleenleden = $row->{'Alleen leden'};
			if ($row->Kader == 1) {
				$this->iskader = true;
			} else {
				$this->iskader = false;
			}

			if ($row->GekoppeldAanQuery == 1 or strlen($row->MySQL) > 10) {
				$this->isautogroep = true;
			} else {
				$this->isautogroep = false;
			}
			
			if ($row->LedenMuterenDoor > 0 and in_array($row->LedenMuterenDoor, explode(",", $_SESSION['lidgroepen'])) == true) {
				$this->magledenmuteren = true;
			} else {
				$this->magledenmuteren = false;
			}
		
		} else {
			$this->oid = 0;
		}
	}
	
	public function naam($p_oid, $p_riz="") {
		$this->vulvars($p_oid);
		if (strlen($this->naam) > 0) {
			return $this->naam;
		} else {
			return $p_riz;
		}
	}
	
	public function autogroep($p_oid) {
		$this->vulvars($p_oid);
		return $this->isautogroep;
	}
	
	public function record($p_oid, $p_kode="") {
		if ($p_oid > 0) {
			$query = sprintf("SELECT O.* FROM %s AS O WHERE O.RecordID=%d;", $this->table, $p_oid);
		} else {
			$query = sprintf("SELECT O.* FROM %s AS O WHERE UPPER(O.Kode)='%s';", $this->table, strtoupper($p_kode));
		}
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
	public function lijst($p_ingebruik=1, $p_filter="", $p_per="", $p_lidid=0, $p_orderby="", $p_fetched=1) {
		
		if (strlen($p_per) < 8) {
			$p_per = date("Y-m-d");
		}
		$sqal = sprintf("SELECT COUNT(*) FROM %sLidond AS LO WHERE %s AND LO.OnderdeelID=O.RecordID", TABLE_PREFIX, str_replace("CURDATE()", "'" . $p_per . "'", cls_db_base::$wherelidond));
		
		$filter = "";
		if ($p_ingebruik > 0) {
			if ($_SESSION['webmaster'] == 0 and $p_lidid > 0) {
				$filter .= sprintf(" WHERE O.RecordID IN (SELECT LO.OnderdeelID FROM %sLidond AS LO WHERE %s AND LO.Lid=%d)", TABLE_PREFIX, cls_db_base::$wherelidond, $p_lidid);
			} else {
				$filter = sprintf(" WHERE (%s) > 0", $sqal);
			}
		}
		
		if (strlen($p_filter) > 0) {
			if (strlen($filter) > 0) {
				$filter .= " AND ";
			} else {
				$filter = " WHERE ";
			}
			$filter .= $p_filter;
		}
		
		if (strlen($p_orderby) > 0) {
			$orderby = $p_orderby . ", O.Naam";
		} else {
			$orderby = "O.Naam";
		}
		
		$query = sprintf("SELECT O.*, CONCAT(O.Kode, ' - ', O.Naam) AS Oms, (%s) AS AantalLeden FROM %s %s ORDER BY %s;", $sqal, $this->basefrom, $filter, $orderby);
		$result = $this->execsql($query);
		
		try {
			if ($p_fetched == 1) {
				return $result->fetchAll();
			} else {
				return $result;
			}
			
		} catch (Exception $e) {
			debug("Probleem met SQL/database: " . $e->getMessage() . "\n", 1, 1);
			return false;
		}
	}
	
	public function editlijst($p_ondtype, $p_fetched=1) {
		
		if ($p_ondtype == "P") {
			$w = sprintf("O.LedenMuterenDoor > 0 AND O.LedenMuterenDoor IN (%s)", $_SESSION['lidgroepen']);
		} elseif (strlen($p_ondtype) > 1) {
			$t = "";
			for ($i=0;$i<strlen($p_ondtype);$i++) {
				if (strlen($t) > 1) {
					$t .= ",";
				}
				$t .= sprintf("'%s'", substr($p_ondtype, $i, 1));
			}
			$w = sprintf("O.`Type` IN (%s)", $t);
		} else {
			$w = sprintf("O.`Type`='%s'", $p_ondtype);
		}
		
		$xsel = sprintf("(SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.OnderdeelID=O.RecordID AND IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE()) AS `aantalLeden`", TABLE_PREFIX);
		
		$query = sprintf("SELECT O.RecordID, O.Kode, O.Naam, O.Kader, %s FROM %s WHERE %s ORDER BY IF(IFNULL(O.VervallenPer, '9999-12-31') > CURDATE(), 0, 1), O.Naam;", $xsel, $this->basefrom, $w);
		$result = $this->execsql($query);
		
		if ($p_fetched == 1) {
			return $result->fetchAll();
		} else {
			return $result;
		}
	}
	
	public function htmloptions($p_cv=0, $p_ingebruik=1, $p_ondtype="") {
		
		if (strlen($p_ondtype) > 0) {
			$t = "";
			for ($i=0;$i<strlen($p_ondtype);$i++) {
				if (strlen($t) > 1) {
					$t .= ",";
				}
				$t .= sprintf("'%s'", substr($p_ondtype, $i, 1));
			}
			$f = sprintf("O.`Type` IN (%s)", $t);
		} else {
			$f = "";
		}
		
		$ret = "";
		$rows = $this->lijst($p_ingebruik, $f);
		foreach ($rows as $row) {
			$o = htmlentities($row->Naam);
			if ($row->AantalLeden > 1) {
				$o .= sprintf(" (%d leden)", $row->AantalLeden);
			}
			$ret .= sprintf("<option%s value=%d>%s</option>\n", checked($row->RecordID, "option", $p_cv), $row->RecordID, $o);
		}
		return $ret;
	}
	
	public function add($p_type="G", $p_code="") {
		$this->oid = 0;
		$this->tas = 1;
		$this->tm = 1;
		$nrid = $this->nieuwrecordid();
		if (strlen($p_code) < 3) {
			$p_code = sprintf("%s_%d", $p_type, $nrid);
		}
		$query = sprintf("SELECT COUNT(*) FROM %s WHERE Kode='%s';", $this->table, $p_code);
		if ((new cls_db_base())->scalar($query) > 0) {
			$this->mess = sprintf("De code '%s' is al in gebruik. Deze %s wordt niet toegevoegd.", $p_code, strtolower(ARRTYPEONDERDEEL[$p_type]));
			$nrid = 0;
			
		} elseif (strlen($p_type) == 0 or strpos($this->mogelijketypes, $p_type) === false) {
			$this->mess = sprintf("Type '%s' is niet correct. Deze %s wordt niet toegevoegd.", $p_type, strtolower(ARRTYPEONDERDEEL[$p_type]));
			$nrid = 0;
			
		} else {
			$query = sprintf("INSERT INTO %s (RecordID, Kode, Type, Naam, Kader,`Alleen leden`, GekoppeldAanQuery, Ingevoerd) VALUES (%d, '%s', '%s', '*** nieuw %s ***', 0, 0, 0, NOW());", $this->table, $nrid, $p_code, $p_type, $p_code);
			if ($this->execsql($query) > 0) {
				$this->mess = sprintf("Onderdeel %d met code '%s' is toegevoegd.", $nrid, $p_code);
				(new cls_Interface())->add($query, 0);
			}
		}
		$this->log($nrid, 1);
		
		return $nrid;
	}

	public function update($p_oid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_oid);
		$this->tas = 2;
		
		if ($p_kolom == "VervallenPer" and strlen($p_waarde) < 8) {
			$p_waarde = "";
		
		} elseif ($p_kolom == "Naam") {
			$p_waarde = str_replace("\"", "'", $p_waarde);
			
		} elseif ($p_kolom == "MySQL" and strlen($p_waarde) > 10) {
			if (startwith($p_waarde, "SELECT")) {
				$p_waarde = str_replace("\"", "'", $p_waarde);
			} else {
				$this->mess = sprintf("De MySQL-code moet met SELECT beginnen. Kolom '%s' is leeg gemaakt.", $p_kolom);
				$this->log($this->oid);
				$p_waarde = "";
			}
		}

		if ($p_kolom == "Type" and (strlen($p_waarde) == 0 or strpos($this->mogelijketypes, $p_waarde) === false)) {
			$this->mess = sprintf("Type wordt niet bijgewerkt, omdat de waarde (%s) niet juist is.", $p_waarde);
			
		} elseif ($this->pdoupdate($p_oid, $p_kolom, $p_waarde)) {
			if (strlen($p_reden) > 0) {
				$this->mess .= ", omdat " . $p_reden;
			}
		}

		$this->log($this->oid);
	}
	
	public function delete($p_oid, $p_reden="") {
		$this->vulvars($p_oid);
		$this->tas = 3;
		
		if ($this->pdodelete($this->oid) > 0) {
			$this->mess = sprintf("%s (%d) is verwijderd", $this->naam, $this->oid);
			if (strlen($p_reden) > 0) {
				$this->mess .= ", omdat " . $p_reden;
			}
			$this->log($this->oid);
		}
	}
	
	public function opschonen() {
		$query = sprintf("SELECT O.RecordID FROM %s WHERE IFNULL(O.VervallenPer, CURDATE()) < DATE_SUB(CURDATE(), INTERVAL 7 DAY) AND IFNULL(O.Gewijzigd, CURDATE()) < DATE_SUB(CURDATE(), INTERVAL 7 DAY) AND
								 O.RecordID NOT IN (SELECT LO.OnderdeelID FROM %2\$sLidond AS LO) AND
								 O.RecordID NOT IN (SELECT GR.OnderdeelID FROM %2\$sGroep AS GR) AND
								 O.RecordID NOT IN (SELECT AK.OnderdeelID FROM %2\$sAfdelingskalender AS AK);", $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$reden = "deze vervallen is en er geen leden en afdelingsgroepen meer aan zijn gekoppeld.";
			$this->delete($row->RecordID, $reden);
		}
	}
	
	public function controle() {
		
		$a['BST'] = "Bestuur";
		$a['FNK'] = "Functionarissen";
		$a['Kad'] = "Kader";
		foreach ($a as $c => $o) {
			$f = sprintf("O.Kode='%s'", $c);
			$oid = $this->max("RecordID", $f);
			if ($oid == 0) {
				if ($c == "Kader") {
					$t = "G";
				} else {
					$t = substr($c, 0, 1);
				}
				$oid = $this->add($t, $c);
				$this->update($oid, "Naam", $o);
			}
			if ($c == "Kad") {
				$this->update($oid, "Kader", 0);
				$this->update($oid, "HistorieOpschonen", 0);
				$s = sprintf("SELECT DISTINCT LO.Lid AS LidID FROM %1\$sLid AS L INNER JOIN ((%1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON O.RecordID=LO.OnderdeelID) INNER JOIN %1\$sFunctie AS F ON F.Nummer=LO.Functie) ON LO.Lid=L.RecordID WHERE LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE() AND (F.Kader=1 OR O.Kader=1);", TABLE_PREFIX);
				$this->update($oid, "MySQL", $s);
			} else {
				$this->update($oid, "Kader", 1);
			}
				
		}
		
		foreach ($this->basislijst() as $row) {
			if (strlen($row->Type) == 0 or strpos($this->mogelijketypes, $row->Type) === false) {
				$this->update($row->RecordID, "Type", "G", "type een ongeldige waarde had");
			} elseif (strlen($row->MySQL) > 0 and (strlen($row->MySQL) <= 10 or substr($row->MySQL, 0, 6) != "SELECT")) {
				$this->update($row->RecordID, "MySQL", "", "de mySQL-code niet correct is");
			} elseif (strpos("EOT", $row->Type) !== false and $row->Kader == 1) {
				$reden = "bij een eigenschap, onderscheiding of toestemming mag kader geen ja zijn";
				$this->update($row->RecordID, "Kader", 0, $reden);
			}
		}
	}  # controle
	
}  # cls_Onderdeel

class cls_Functie extends cls_db_base {
	
	private $fnkid = 0;
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Functie";
		$this->basefrom = $this->table . " AS F";
		$this->ta = 20;
		$this->pkkol = "Nummer";
	}
	
	public function selectlijst($p_soort, $p_per="", $p_asarray=0, $p_ondid=0) {
		
		$sqaantal = sprintf("SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE() AND F.Nummer=LO.Functie", TABLE_PREFIX);
		if ($p_ondid > 0) {
			$sqaantal .= sprintf(" AND LO.OnderdeelID=%d", $p_ondid);
		}
		
		if ($p_soort == "A") {
			$f = "WHERE F.Afdelingsfunctie=1";
		} elseif ($p_soort == "L") {
			$f = "WHERE F.Ledenadministratiefunctie=1";
		} else {
			$f = "";
		}
		
		if ($p_ondid > 0) {
			if (strlen($f) > 0) {
				$f .= " AND ";
			} else {
				$f = "WHERE ";
			}
			$f .= sprintf("(%s) > 0", $sqaantal);
		}
		
		if (strlen($p_per) == 10) {
			if (strlen($f) > 0) {
				$f .= " AND ";
			} else {
				$f = "WHERE ";
			}
			$f .= sprintf("IFNULL(F.`Vervallen per`, '9999-12-31') >= '%s'", $p_per);
		}
		
		$query = sprintf("SELECT F.Nummer, F.Omschrijv, IFNULL(F.`Vervallen per`, '9999-12-31') AS Vervallen, (%s) AS aantalMetFunctie FROM %s %s ORDER BY F.Sorteringsvolgorde, F.Omschrijv;", $sqaantal, $this->basefrom, $f);
		$result = $this->execsql($query);
		
		if ($p_asarray == 1) {
			foreach ($result->fetchAll() as $row) {
				$rv[$row->Nummer] = $row->Omschrijv;
			}
			return $rv;
		} else {
			return $result->fetchAll();
		}
	}
	
	public function add($p_nr=-1) {
		$this->tas = 1;
		if ($p_nr >= 0) {
			$nwpk = $p_nr;
		} else {
			$nwpk = $this->nieuwrecordid();
		}
		
		$this->query = sprintf("INSERT INTO %s (Nummer, Sorteringsvolgorde, Ingevoerd) VALUES (%d, 99, NOW());", $this->table, $nwpk);
		
		if ($this->execsql() > 0) {
			
			$this->mess = sprintf("%s: Record %d toegevoegd.", $this->table, $nwpk);
			$this->log($nwpk, 0);
			
			$sql = sprintf("INSERT INTO Functie (Nummer, Sorteringsvolgorde, Ingevoerd) VALUES (%d, 99, Now());", $nwpk);
			(new cls_Interface())->add($sql);

		}
		
	}
	
	public function update($p_fnkid, $p_kolom, $p_waarde) {
		$this->tas = 2;
		
		if ($this->pdoupdate($p_fnkid, $p_kolom, $p_waarde) > 0) {
			$this->log($p_fnkid);
		}
	}
	
	public function controle() {
		
		$this->update(0, "Afdelingsfunctie", 1);
		$this->update(0, "Ledenadministratiefunctie", 1);
		$this->update(0, "Bewakingsfunctie", 1);
		$this->update(0, "Kader", 0);
		$this->update(0, "Inval", 0);

	}  # controle
	
	public function opschonen() {
		
		$i_lo = new cls_lidond();
		$i_bw = new cls_Bewaking();
		
		foreach ($this->basislijst("IFNULL(`Vervallen per`, '9999-12-31') < CURDATE()") as $row) {
			$f = sprintf("Functie=%d", $row->Nummer);
			if ($i_lo->aantal($f) == 0) {
				if ($i_bw->aantal($f) == 0) {
					if ($this->pdodelete($row->Nummer, "de functie vervallen is en nergens meer wordt gebruikt.") > 0) {
						$this->log($row->Nummer);
					}
				}
			}
		}
	}  # opschonen
	
}  # cls_Functie

class cls_Afdelingskalender extends cls_db_base {
	private $ondid = 0;
	private $akid = 0;
	private $ondnaam = "";
	
	function __construct($p_ondid=-1) {
		$this->table = TABLE_PREFIX . "Afdelingskalender";
		$this->basefrom = $this->table . " AS AK";
		$this->ta = 21;
		$this->vulvars(-1, $p_ondid);
	}
	
	private function vulvars($p_akid=-1, $p_ondid=-1) {
		if ($p_akid >= 0) {
			$this->akid = $p_akid;
		}
		if ($p_ondid >= 0) {
			$this->ondid = $p_ondid;
		}
		if ($this->akid > 0) {
			$query = sprintf("SELECT IFNULL(AK.OnderdeelID, 0) FROM %s WHERE AK.RecordID=%d;", $this->basefrom, $this->akid);
			$this->ondid = $this->scalar($query);
		}
		if ($this->ondid > 0) {
			$this->ondnaam = (new cls_Onderdeel())->Naam($this->ondid);
		}
	}
	
	public function lijst($p_ondid=-1, $p_datum="", $p_filter="", $p_order="", $p_limiet=-1) {
		if ($p_ondid > 0) {
			$this->ondid = $p_ondid;
			$where = sprintf("AK.OnderdeelID=%d", $this->ondid);
		} else {
			$where = "O.`Type`='A'";
		}
		if (strlen($p_datum) > 5) {
			$where .= sprintf(" AND Datum='%s'", $p_datum);
		}
		if (strlen($p_filter) > 1) {
			$where .= sprintf(" AND %s", $p_filter);
		}
		
		if (strlen($p_order) == 0) {
			$p_order = "IF(AK.Datum > '1900-01-01', 1, 0), AK.Datum DESC, O.Naam";
		}
		
		$lm = "";
		if ($p_limiet > 0) {
			$lm = sprintf(" LIMIT %d", $p_limiet);
		}
		
		$sqstart = sprintf("SELECT MIN(GR.Starttijd) FROM %sGroep AS GR WHERE GR.Starttijd > '00:00' AND GR.OnderdeelID=AK.OnderdeelID", TABLE_PREFIX);
		
		$query = sprintf("SELECT AK.*, O.Kode, O.Naam, (%s) AS Begintijd FROM %s INNER JOIN %sOnderdl AS O ON O.RecordID=AK.OnderdeelID WHERE %s ORDER BY %s%s;", $sqstart, $this->basefrom, TABLE_PREFIX, $where, $p_order, $lm);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function record($p_ondid, $p_datum) {
		$query = sprintf("SELECT AK.*, O.Kode, O.Naam FROM %s AS AK INNER JOIN %sOnderdl AS O ON O.RecordID=AK.OnderdeelID WHERE AK.OnderdeelID=%d AND Datum='%s';", $this->table, TABLE_PREFIX, $p_ondid, $p_datum);
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
	public function htmloptions($p_onderdeelid, $p_cv, $p_filter="") {
		global $dtfmt;
		
		$rv = "";
		$dtfmt->setPattern(DTTEXT);
		foreach ($this->lijst($p_onderdeelid, "", $p_filter) as $row) {
			$o = $dtfmt->format(strtotime($row->Datum));
			if (strlen($row->Omschrijving) > 0) {
				$o .= " " . substr($row->Omschrijving, 0, 25);
			}
			$c = checked($row->RecordID, "option", $p_cv);
			$rv .= sprintf("<option value=%d %s>%s</option>\n", $row->RecordID, $c, $o);
		}
		return $rv;
	}
	
	public function add($p_ondid) {
		$this->tas = 1;
		$this->vulvars(-1, $p_ondid);
		$nrid = $this->nieuwrecordid();
		
		$query = sprintf("SELECT DATE_ADD(IFNULL(MAX(AK.Datum), CURDATE()), INTERVAL 7 DAY) FROM %s WHERE OnderdeelID=%d;", $this->basefrom, $p_ondid);
		$dat = $this->scalar($query);
		
		$query = sprintf("INSERT INTO %s (RecordID, OnderdeelID, Datum, IngevoerdDoor) VALUES (%d, %d, '%s', %d);", $this->table, $nrid, $p_ondid, $dat, $_SESSION['lidid']);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Record %d (%s) is voor afdeling %s in '%s' toegevoegd.", $nrid, $dat, $this->ondnaam, $this->table);
		}
		
		$this->log($nrid, 0);
		return $nrid;
	}
	
	public function update($p_akid, $p_kolom, $p_waarde) {
		$this->tas = 2;
		$this->vulvars($p_akid);
		$rv = "";
		
		$query = sprintf("SELECT COUNT(*) FROM %s WHERE AK.OnderdeelID=%d AND AK.Datum='%s' AND AK.RecordID<>%d;", $this->basefrom, $this->ondid, $p_waarde, $this->akid);
		if ($p_kolom == "Datum" and $this->scalar($query) > 0) {
			$this->mess = sprintf("Datum '%s' staat al in de afdelingskalender van %s. De wijziging wordt niet verwerkt.", $p_waarde, $this->ondnaam);
			$rv = $this->mess;
			$this->log($this->akid);
		} elseif ($this->pdoupdate($p_akid, $p_kolom, $p_waarde) > 0) {
			$this->log($p_akid);
		}
		
		return $rv;
	}
	
	public function delete($p_akid) {
		$this->tas = 3;
		$this->vulvars($p_akid);
		
		if ((new cls_Aanwezigheid())->aantal(sprintf("AfdelingskalenderID=%d", $p_akid)) == 0) {
			$this->pdodelete($p_akid);
		} else {
			$this->mess = sprintf("Record %d uit de afdelingskalender mag niet worden verwijderd, want die is nog in gebruik.", $p_akid);
		}
		$this->log($p_akid);
	}
	
}  # cls_Afdelingskalender

class cls_Aanwezigheid extends cls_db_base {
	
	private $loid = 0;
	private $aanwid = 0;
	private $akid = 0;
	private $akdatum  = "";
	private $lidvanaf = "";
	private $lidtm = "";
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Aanwezigheid";
		$this->basefrom = $this->table . " AS AW";
		$this->ta = 24;
	}
	
	private function vulvars($p_loid, $p_akid=-1) {
		$this->loid = $p_loid;
		if ($p_akid > 0) {
			$this->akid = $p_akid;
		}
		$query = sprintf("SELECT IFNULL(LO.Lid, 0) AS LidID, LO.Vanaf, IFNULL(LO.Opgezegd, '9999-12-31') AS TM FROM %sLidond AS LO WHERE LO.RecordID=%d;", TABLE_PREFIX, $this->loid);
		$row = $this->execsql($query)->fetch();
		$this->lidid  = $row->LidID;
		$this->lidvanaf = $row->Vanaf;
		$this->lidtm = $row->TM;
		
		$query = sprintf("SELECT IFNULL(RecordID, 0) FROM %s WHERE AW.LidondID=%d AND AW.AfdelingskalenderID=%d;", $this->basefrom, $this->loid, $this->akid);
		$this->aanwid = $this->scalar($query);
		
		$query = sprintf("SELECT IFNULL(AK.Datum, '') FROM %sAfdelingskalender AS AK WHERE AK.RecordID=%d;", TABLE_PREFIX, $this->akid);
		$this->akdatum = $this->scalar($query);
	}
	
	public function status($p_lidondid, $p_akid) {
		$query = sprintf("SELECT IFNULL(MAX(Status), '') FROM %s WHERE LidondID=%d AND AfdelingskalenderID=%d", $this->table, $p_lidondid, $p_akid);
		return $this->scalar($query);
	}
	
	public function aantalstatus($p_stat, $p_ondid) {
		$xw = "";
		if (strlen($p_stat) > 0 and $p_stat != "*") {
			$xw = sprintf("AND AW.Status='%s'", $p_stat);
		}
		$query = sprintf("SELECT COUNT(*) FROM %s INNER JOIN %sLidond AS LO ON AW.LidondID=LO.RecordID WHERE LO.OnderdeelID=%d %s;", $this->basefrom, TABLE_PREFIX, $p_ondid, $xw);
		return $this->scalar($query);
	}
	
	public function overzichtlid($p_lidid) {
		
		$query = sprintf("SELECT AK.Datum, AK.Omschrijving, AW.Status, F.OMSCHRIJV AS Functie
						  FROM %2\$sAfdelingskalender AS AK INNER JOIN (%1\$s INNER JOIN (%2\$sLidond AS LO INNER JOIN %2\$sFunctie AS F ON LO.Functie=F.Nummer) ON AW.LidondID=LO.RecordID) ON AW.AfdelingskalenderID=AK.RecordID
						  WHERE LO.Lid=%3\$d AND LENGTH(Status) > 0
						  ORDER BY AK.Datum DESC;", $this->basefrom, TABLE_PREFIX, $p_lidid);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function seizoenen($p_ondid) {
		
		$sq = sprintf("(SELECT COUNT(*) FROM %sAfdelingskalender AS AK WHERE AK.Datum >= SZ.Begindatum AND AK.Datum <= SZ.Einddatum AND Activiteit=1 AND AK.OnderdeelID=%d AND AK.Datum <= CURDATE())", TABLE_PREFIX, $p_ondid);
		$query = sprintf("SELECT SZ.Nummer, SZ.Begindatum, SZ.Einddatum FROM %2\$sSeizoen AS SZ WHERE %1\$s > 0 ORDER BY SZ.Begindatum DESC;", $sq, TABLE_PREFIX);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function perlidperperiode($p_loid, $p_vanaf, $p_tm) {
		$this->vulvars($p_loid);
		
		if ($this->lidvanaf > $p_vanaf) {
			$p_vanaf = $this->lidvanaf;
		}
		if ($this->lidtm < $p_tm) {
			$p_tm = $this->lidtm;
		}
		
		$query = sprintf("SELECT MAX(LO.Lid) AS LidID, SUM(IF(AW.Status='A', 1, 0)) AS aantAangemeld, IFNULL(SUM(IF(AW.Status IN ('A', 'L', 'V'), 0, 1)), 0) AS aantAfwezig,
						  IFNULL(SUM(IF(AW.Status='V', 1, 0)), 0) AS aantVervallen,
						  SUM(IF(AW.Status IN ('N', 'X'), 1, 0)) AS aantZonderReden,
						  SUM(IF(AW.Status='R', 1, 0)) AS aantMetReden,
						  SUM(IF(AW.Status='Z', 1, 0)) AS aantZiek, SUM(IF(AW.Status='L', 1, 0)) AS aantLaat
						  FROM (%1\$s INNER JOIN %2\$sLidond AS LO ON LO.RecordID=AW.LidondID) INNER JOIN %2\$sAfdelingskalender AS AK ON AW.AfdelingskalenderID=AK.RecordID
						  WHERE AK.Datum >= '%3\$s' AND AK.Datum <= '%4\$s' AND AK.Datum <= CURDATE() AND AW.LidondID=%5\$d;", $this->basefrom, TABLE_PREFIX, $p_vanaf, $p_tm, $p_loid);
		$result = $this->execsql($query);
		$row = $result->fetch();
		return $row;
	}
	
	private function add($p_loid, $p_akid, $p_waarde) {
		$this->vulvars($p_loid, $p_akid);
		$this->tas = 1;
		
		if (strlen($this->akdatum) >= 10) {
			$nrid = $this->nieuwrecordid();
			$query = sprintf("INSERT INTO %s (RecordID, LidondID, AfdelingskalenderID, Status, IngevoerdDoor) VALUES (%d, %d, %d, '%s', %d);", $this->table, $nrid, $p_loid, $p_akid, $p_waarde, $_SESSION['lidid']);
			if ($this->execsql($query) > 0) {
				$this->mess = sprintf("Record %d (%s) met status '%s' is aan tabel '%s' toegevoegd.", $nrid, $this->akdatum, $p_waarde, $this->table);
			} else {
				$this->mess = "Toevoegen record aanwezigheid is mislukt.";
				$nrid = 0;
			}
		} else {
			$nrid = 0;
			$this->mess = sprintf("Record %d bestaat niet in de afdelingskalender. Het record wordt niet toegevoegd.", $p_akid);
		}
		
		$this->aanwid = $nrid;
		$this->log($nrid);
		return $nrid;
		
	}
	
	public function update($p_loid, $p_akid, $p_kolom, $p_waarde) {
		$this->vulvars($p_loid, $p_akid);
		$this->tas = 2;
		
		if ($this->aanwid == 0 and strlen($p_waarde) > 0) {
			$this->add($this->loid, $this->akid, $p_waarde);
		} elseif ($this->aanwid > 0 and strlen($p_waarde) == 0) {
			$this->delete($this->loid, $this->akid);
		}
		
		if ($this->pdoupdate($this->aanwid, $p_kolom, $p_waarde) > 0) {
			$this->mess = sprintf("Kolom '%s' in record %d in tabel '%s' is in '%s' gewijzigd.", $p_kolom, $this->aanwid, $this->table, $p_waarde);
			$this->log($this->aanwid);
		}
	}
	
	private function delete($p_loid, $p_akid) {
		$this->vulvars($p_loid, $p_akid);
		$this->tas = 13;
		$this->pdodelete($this->aanwid);
		$this->log($aanwid);
	}
	
}  # cls_Aanwezigheid

class cls_Lidond extends cls_db_base {
	private $loid = 0;  // RecordID van het record in Lidond
	private $ondid = 0; // RecordID van het onderdeel
	public $ondnaam = ""; // Naam van het onderdeel
	private $ondtype = ""; // Type van het onderdeel
	private $ondkader = 0; // Is dit kader?
	private $lidnaam = "";  // De naam van het lid
	private $alleenleden = 0; // Mogen bij dit onderdeel alleen leden worden ingedeeld?
	public $organisatie = 0;  // Bij welke organisatie is dit onderdeel aangesloten?
	
	function __construct($p_ondid=-1, $p_lidid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Lidond";
		$this->basefrom = $this->table . " AS LO";
		if ($p_ondid > 0) {
			$this->ondid = $p_ondid;
			$ondrow = (new cls_Onderdeel())->record($this->ondid);
			if (isset($ondrow)) {
				$this->ondnaam = $ondrow->Naam;
				$this->organisatie = $ondrow->ORGANIS;
			}
		}
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		$this->ta = 6;
	}
	
	private function vulvars($p_loid=-1, $p_lidid=-1, $p_ondid=-1, $p_per="") {

		$this->loid = $p_loid;
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		if ($p_ondid >= 0) {
			$this->ondid = $p_ondid;
		}
		if (strlen($p_per) < 10) {
			$p_per = date("Y-m-d");
		}

		if ($this->loid > 0) {
			$this->query = sprintf("SELECT LO.Lid, LO.OnderdeelID FROM %s WHERE LO.RecordID=%d;", $this->basefrom, $this->loid);
			$row = $this->execsql()->fetch();
			if (isset($row->Lid)) {
				$this->lidid = $row->Lid;
				$this->ondid = $row->OnderdeelID;
			} else {
				$this->mess = sprintf("Record in lidond met ID %d bestaat niet.", $this->loid);
				$this->tm = 1;
			}
		} elseif ($this->lidid > 0 and $this->ondid > 0) {
			$query = sprintf("SELECT IFNULL(MAX(LO.RecordID), 0) FROM %s AS LO WHERE LO.Lid=%d AND LO.OnderdeelID=%d AND LO.Vanaf <= '%4\$s' AND IFNULL(LO.Opgezegd, '9999-12-31') >= '%4\$s';", $this->table, $this->lidid, $this->ondid, $p_per);
			$this->loid = $this->scalar($query);
		}
		
		if ($this->ondid > 0) {
			$query = sprintf("SELECT O.Naam, O.Type, O.Kader, O.`Alleen leden` AS AlleenLeden FROM %sOnderdl AS O WHERE O.RecordID=%d;", TABLE_PREFIX, $this->ondid);
			$row = $this->execsql($query)->fetch();
			if (isset($row)) {
				$this->ondnaam = $row->Naam;
				$this->ondtype = $row->Type;
				$this->ondkader = $row->Kader;
				$this->alleenleden = $row->AlleenLeden;
			}
		}
		if ($this->lidid > 0) {
			$query = sprintf("SELECT IFNULL(%1\$s, '%3\$d') FROM %2\$sLid AS L WHERE L.RecordID=%3\$d;", $this->selectnaam, TABLE_PREFIX, $this->lidid);
			$this->lidnaam = $this->scalar($query);
		} else {
			$this->lidnaam = "";
		}
		if ($this->ondtype == "T") {
			$this->ta = 16;
		} elseif ($this->ondtype == "E") {
			$this->ta = 17;
		}
	}
	
	public function record($p_lidid, $p_ondid=-1, $p_per="", $p_loid=0) {
		
		if (strlen($p_per) < 10) {
			$p_per = date("Y-m-d");
		}
		$this->vulvars($p_loid, $p_lidid, $p_ondid, $p_per);
		
		$this->query = sprintf("SELECT LO.*, GR.Starttijd, O.Naam AS AfdNaam, Act.Omschrijving AS GrActiviteit, Act.Contributie AS GrContributie, F.Omschrijv AS Functie, LO.Functie AS FunctieID, F.Kader
								FROM ((%s INNER JOIN (%2\$sGroep AS GR LEFT OUTER JOIN %2\$sActiviteit AS Act ON Act.RecordID=GR.ActiviteitID) ON LO.GroepID=GR.RecordID) INNER JOIN %2\$sFunctie AS F ON F.Nummer=LO.Functie) INNER JOIN %2\$sOnderdl AS O ON LO.OnderdeelID=O.RecordID WHERE LO.RecordID=%3\$d;", $this->basefrom, TABLE_PREFIX, $this->loid);
		$result = $this->execsql();
		return $result->fetch();
	}
	
	public function lijst($p_ondid, $p_filter="", $p_ord="GR.Volgnummer, GR.Kode", $p_per="", $p_limiet=0) {
		
		if (strlen($p_per) < 10) {
			$p_per = date("Y-m-d");
		}
		
		$w = sprintf("LO.Vanaf <= '%1\$s' AND IFNULL(LO.Opgezegd, '9999-12-31') >= '%1\$s'", $p_per);
		if ($p_ondid > 0) {
			$w .= sprintf(" AND LO.OnderdeelID=%d", $p_ondid);
		}
		if (strlen($p_filter) > 0) {
			$w .= " AND " . $p_filter;
		}
		
		if (strlen($p_ord) > 0) {
			$p_ord .= ", ";
		}
		
		$lm = "";
		if ($p_limiet > 0) {
			$lm = sprintf(" LIMIT %d", $p_limiet);
		}
		
		$sq = sprintf("(SELECT IFNULL(MAX(DatumTijd), '1900-01-01') FROM %sAdmin_activiteit AS A WHERE A.ReferID=LO.RecordID AND A.TypeActiviteit=19)", TABLE_PREFIX);
		
		$query = sprintf("SELECT LO.RecordID, LO.Lid AS LidID, %s AS NaamLid, L.Roepnaam, L.Achternaam, L.Tussenv, L.GEBDATUM, %s AS Leeftijd, F.Omschrijv AS Functie, F.Afkorting AS FunctAfk, F.Inval AS Invalfunctie, %s AS Groep, LO.Lid, O.Naam AS OndNaam, O.CentraalEmail, L.EmailVereniging, LO.Opmerk,
						  LO.Vanaf, LO.Opgezegd, LO.EmailFunctie, GR.Kode AS GrCode, GR.Omschrijving AS GrNaam, GR.Aanwezigheidsnorm, L.RelnrRedNed AS SportlinkID, LO.GroepID, LO.Functie AS FunctieID, %s AS LaatsteGroepMutatie
						  FROM %s
						  WHERE %s
						  ORDER BY %sL.Achternaam, L.Tussenv, L.Roepnaam%s;", $this->selectnaam, $this->selectleeftijd, $this->selectgroep, $sq, $this->fromlidond, $w, $p_ord, $lm);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function selectielijst($p_ondid, $p_fetched=1) {
		$this->ondid = $p_ondid;
		$this->vulvars();
		
		$this->query = sprintf("SELECT %s AS `Naam_lid`, LO.Vanaf, LO.Functie, LO.EmailFunctie, LO.Opmerk, LO.Opgezegd, LO.RecordID, L.GEBDATUM
								FROM %s
								WHERE LO.OnderdeelID=%d AND IFNULL(LO.Opgezegd, CURDATE()) >= LO.Vanaf
								ORDER BY IF(IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE(), 0, 1), L.Achternaam, L.TUSSENV, L.Roepnaam, LO.Vanaf DESC;", $this->selectnaam, $this->fromlidond, $p_ondid);
		$result = $this->execsql();
		
		if ($p_fetched == 1) {
			return $result->fetchAll();
		} else {
			return $result;
		}
	}
	
	public function aantallid($p_ondid, $p_filter="") {
		
		$w = cls_db_base::$wherelidond . sprintf(" AND LO.OnderdeelID=%d", $p_ondid);
		if (strlen($p_filter) > 0) {
			$w .= " AND " . $p_filter;
		}
		
		$query = sprintf("SELECT COUNT(DISTINCT Lid) FROM %s WHERE %s;", $this->fromlidond, $w);
		return $this->scalar($query);
	}
	
	public function editlijst($p_filter, $p_lidid) {
		
		$p_filter .= sprintf(" AND LO.Lid=%d", $p_lidid);
		$query = sprintf("SELECT LO.RecordID, O.Kode, O.Naam, LO.OPMERK, LO.Vanaf, LO.Opgezegd, LO.OnderdeelID, LO.Functie, LO.EmailFunctie, LO.GroepID FROM %s WHERE %s
							   ORDER BY IF(LO.Opgezegd > '1900-01-01', 1, 0), O.Kode;", $this->fromlidond, $p_filter);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function onderdeellijst($p_ondid, $p_filter=1, $p_extrafilter="", $p_sort="") {
		/*
			Uitleg p_filter
			- 1: huidige leden
			- 2: huidige en toekomstige leden
			- 3: leden zonder einde-datum
		*/
		
		$filter = sprintf("LO.OnderdeelID=%d", $p_ondid);
		if ($p_filter == 1) {
			$filter .= " AND " . cls_db_base::$wherelidond;
		} elseif ($p_filter == 2) {
			$filter .= " AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE()";
		} elseif ($p_filter == 3) {
			$filter .= " AND (LO.Opgezegd IS NULL)";
		}
		
		if (strlen($p_extrafilter) > 0) {
			$filter .= " AND " . $p_extrafilter;
		}
		
		$sel = sprintf("L.RecordID AS LidID, %s AS `Naam_lid`, CONCAT(%s, ' & ', %s) AS Bereiken, %s AS Email", $this->selectnaam, $this->selecttelefoon, $this->selectemail, $this->selectemail);
		$f_query = sprintf("SELECT MAX(LO.Functie) FROM %s WHERE %s;", $this->fromlidond, $filter);
		$gr_query = sprintf("SELECT MAX(LO.GroepID) FROM %s WHERE %s;", $this->fromlidond, $filter);
		$opm_query = sprintf("SELECT MAX(LO.OPMERK) FROM %s WHERE %s;", $this->fromlidond, $filter);
		$opg_query = sprintf("SELECT MAX(LO.Opgezegd) FROM %s WHERE %s;", $this->fromlidond, $filter);
		
		$sel .= ", CASE WHEN LO.GroepID > 0 AND LO.Functie > 0 THEN CONCAT(F.OMSCHRIJV, '/', IF(LENGTH(GR.Kode)=0, GR.RecordID, GR.Kode))
						WHEN LO.Functie > 0 THEN F.OMSCHRIJV
						WHEN LO.GroepID > 0 THEN IF(LENGTH(GR.Kode)=0, GR.RecordID, GR.Kode)
						ELSE '' END AS `Functie / Groep`";
		
		$sel .= ", F.Afkorting AS AfkFunc";
		if (strlen($this->scalar($opm_query)) > 0) {
			$sel .= ", LO.OPMERK AS Opmerking";
		}
		$sel .= ", LO.Vanaf";
		
		if ($this->scalar($opg_query) > '2001-01-01') {
			$sel .= ", LO.Opgezegd";
		}
		
		$sel .= sprintf(", L.GEBDATUM, %s AS Zoeknaam, LO.GroepID, LO.RecordID, %s AS Leeftijd, L.RelnrRedNed AS SportlinkID", $this->selectzoeknaam, $this->selectleeftijd);
		
		if (strlen($p_sort) > 0) {
			$p_sort .= ", ";
		}
		
		$query = sprintf("SELECT %s FROM %s WHERE %s ORDER BY %sL.Achternaam, L.TUSSENV, L.Roepnaam;", $sel, $this->fromlidond, $filter, $p_sort);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function lijstperlid($p_lidid, $p_type="*", $p_per="") {
		
		if (strlen($p_per) >= 8) {
			$w = sprintf("LO.Vanaf <= '%1\$s' AND IFNULL(LO.Opgezegd, '9999-12-31') >= '%1\$s'", $p_per);
		} else {
			$w = cls_db_base::$wherelidond;
		}
		$w .= sprintf(" AND LO.Lid=%d", $p_lidid);
		if ($p_type != "*") {
			$w .= sprintf(" AND O.`Type`='%s'", $p_type);
		}
		
		$query = sprintf("SELECT LO.*, O.Naam AS NaamOnderdeel, O.CentraalEmail, F.OMSCHRIJV AS FunctieOms, F.Kader, Act.Omschrijving AS GrActiviteit, Act.Code AS ActCode, Act.Contributie AS GrContributie,
						  O.LIDCB, O.JEUGDCB, O.FUNCTCB, O.Naam AS OndNaam, O.Kode AS OndCode
						  FROM %s WHERE %s ORDER BY LO.Vanaf;", $this->fromlidond, $w);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function groepsindeling($p_onderdeelid, $p_filter="") {
		
		if ($p_onderdeelid > 0) {
			$where = sprintf("AND LO.OnderdeelID=%d", $p_onderdeelid);
		} elseif (strlen($p_filter) > 0) {
			$where = "AND " . $p_filter;
		} else {
			$where = sprintf("AND LO.OnderdeelID IN (SELECT OnderdeelID FROM %sGroep)", TABLE_PREFIX);
		}
		$this->query = sprintf("SELECT DISTINCT O.Naam AS AfdNaam, CONCAT(GR.Starttijd, IF(LENGTH(GR.Eindtijd) > 3, CONCAT(' - ', GR.Eindtijd), '')) AS Tijdsblok, GR.Kode, GR.Omschrijving, L.Roepnaam,
					%1\$s AS NaamLid, %6\$s AS AVGnaam, %5\$s AS GroepOms, GR.Zwemzaal,
					LO.RecordID, LO.GroepID, LO.Lid, %2\$s AS Leeftijd, LO.Vanaf, IFNULL(LO.Opgezegd, '9999-12-31') AS Opgezegd
					FROM %3\$s
					WHERE IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE() AND (LO.Functie=0 OR LO.GroepID > 0) %4\$s
					ORDER BY O.Naam, IF(LO.GroepID=0, '99:99', GR.Starttijd), GR.Volgnummer, GR.Omschrijving, GR.RecordID, L.Achternaam, L.Roepnaam;", $this->selectnaam, $this->selectleeftijd, $this->fromlidond, $where, $this->selectgroep, $this->selectavgnaam);
		$result = $this->execsql();
		return $result->fetchAll();
	}
	
	public function lidgroepen() {
		$query = sprintf("SELECT GROUP_CONCAT(DISTINCT LO.OnderdeelID SEPARATOR ',') AS LG FROM %s WHERE Lid=%d AND %s;", $this->basefrom, $_SESSION['lidid'], cls_db_base::$wherelidond);
		$result = $this->execsql($query);
		$row = $result->fetch();
		if (strlen($row->LG) > 0) {
			$rv = "0," . $row->LG;
		} else {
			$rv = "0";
		}
		if ($_SESSION['webmaster'] == 1) {
			$rv = "-1," .$rv;
		}
		return $rv;
	}
	
	public function groepeningebruik($p_ondid=-1) {
		if ($p_ondid > 0) {
			$this->ondid = $p_ondid;
		}
		$query = sprintf("SELECT DISTINCT LO.GroepID, IF(LO.GroepID=0, 'Niet ingedeeld', GR.Omschrijving) AS Omschrijving, GR.Kode FROM %s AS LO LEFT OUTER JOIN %sGroep AS GR ON LO.GroepID=GR.RecordID WHERE (NOT LO.GroepID IS NULL) AND LO.OnderdeelID=%d AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE() ORDER BY IF(LO.GroepID=0, 999, GR.Volgnummer), GR.Omschrijving;", $this->table, TABLE_PREFIX, $this->ondid);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}  # groepeningebruik
	
	public function overzichtlid($p_lidid, $p_soort) {
		
		if ($p_soort == "T") {
			
			if ((new cls_Onderdeel())->max("O.MaximaleLengtePeriode", "Type='T' AND IFNULL(VervallenPer, CURDATE()) >= CURDATE()") > 0) {
				$sg = ", IF(O.MaximaleLengtePeriode > 0, CONCAT(O.MaximaleLengtePeriode, ' maanden'), 'tot intrekking') AS Geldigheid";
				$sq2 = sprintf(", (SELECT IFNULL(MAX(LO2.Vanaf), '') FROM %sLidond AS LO2 WHERE IFNULL(LO2.Opgezegd, '2999-12-31') < CURDATE() AND LO2.Lid=%d AND LO2.OnderdeelID=O.RecordID) AS `Laatste verlopen toestemming`", TABLE_PREFIX, $p_lidid);
			} else {
				$sg = "";
				$sq2 = "";
			}
							
			$sq = sprintf("(SELECT IFNULL(MAX(LO.Vanaf), 'Nee') FROM %s AS LO WHERE %s AND LO.Lid=%d AND LO.OnderdeelID=O.RecordID) AS Vanaf", $this->table, cls_db_base::$wherelidond, $p_lidid);
			$query = sprintf("SELECT O.Naam,
							  %1\$s
							  %2\$s
							  %3\$s
							  FROM %4\$sOnderdl AS O
							  WHERE IFNULL(O.VervallenPer, CURDATE()) >= CURDATE() AND O.Type='T';", $sq, $sg, $sq2, TABLE_PREFIX);
		} else {
			if ($p_soort == "K") {
				$xw = "(O.Kader=1 OR F.Kader=1)";
			} else {
				$xw = sprintf("O.`Type`='%s'", $p_soort);
			}
			$xw .= " AND IFNULL(LO.Opgezegd, '9999-12-31') >= LO.Vanaf";
		
			$query = sprintf("SELECT MAX(LO.Functie) FROM %s WHERE %s AND LO.Lid=%d;", $this->fromlidond, $xw, $p_lidid);
			if ($this->scalar($query) > 0) {
				$xtr_sel = "F.Omschrijv AS Functie, ";
			} else {
				$xtr_sel = "";
			}
		
			$query = sprintf("SELECT MAX(LO.OPMERK) FROM %s WHERE %2\$s AND LO.Lid=%3\$d;", $this->fromlidond, $xw, $p_lidid);
			if (strlen($this->scalar($query)) > 0) {
				$xtr_sel .= "LO.OPMERK AS Opmerking, ";
			}
		
			if ($p_soort != "K") {
				$query = sprintf("SELECT MAX(LO.GroepID) FROM %s WHERE %s AND LO.Lid=%d;", $this->fromlidond, $xw, $p_lidid);
				if ($this->scalar($query) > 0) {
					$xtr_sel .= sprintf("(SELECT CONCAT(IF(LENGTH(GR.Starttijd)=5, CONCAT(GR.Starttijd, ' - '), ''), IF(LENGTH(GR.Omschrijving) > 0, GR.Omschrijving, GR.Kode)) FROM %sGroep AS GR WHERE GR.RecordID=LO.GroepID) AS Groep,", TABLE_PREFIX);
				}
			}
			
			$query = sprintf("SELECT O.Naam,
							  F.OMSCHRIJV AS Functie,
							  LO.Vanaf,
							  %s
							  LO.Opgezegd,
							  IF(LO.Vanaf > DATE_SUB(CURDATE(), INTERVAL 6 MONTH), '', CONCAT(FORMAT(DATEDIFF(IF(LO.Opgezegd IS NULL, CURDATE(), LO.Opgezegd), LO.Vanaf)/365.25, 1, 'nl_NL'), ' jaar')) AS Duur
							  FROM %s
							  WHERE %s AND LO.Lid=%d
							  ORDER BY LO.Vanaf DESC, O.Naam;", $xtr_sel, $this->fromlidond, $xw, $p_lidid);
		}
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function islid($p_lidid, $p_ondid, $p_per="") {
		$this->vulvars(-1, $p_lidid, $p_ondid, $p_per);
		
		if ($this->loid == 0) {
			return false;
		} else {
			return true;
		}
	}
	
	public function loid($p_lidid, $p_ondid) {
		$this->vulvars(-1, $p_lidid, $p_ondid);
		return $this->loid;
	}
	
	public function bewaking($p_lidid) {
		$query  = sprintf("SELECT DISTINCT O.Kode FROM %s WHERE O.`Tonen in bewakingsadministratie`=1 AND LO.Lid=%d AND %s ORDER BY O.Kode;", $this->fromlidond, $p_lidid, cls_db_base::$wherelidond);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function onderscheiding($p_lidid) {
		$query = sprintf("SELECT O.Naam FROM %s AS LO INNER JOIN %sOnderdl AS O ON LO.OnderdeelID=O.RecordID
						  WHERE LO.Lid=%d AND O.`Type`='O' AND (LO.Opgezegd IS NULL) ORDER BY LO.Vanaf DESC;", $this->table, TABLE_PREFIX, $p_lidid);
		return $this->scalar($query);
	}
	
	public function add($p_ondid, $p_lidid, $p_reden="", $p_log=1, $p_vanaf="") {
		
		$i_lm = new cls_Lidmaatschap(0, $p_lidid);
		
		if ($this->tas != 31) {
			$this->tas = 11;
		}
		$rv = false;
		$this->vulvars(0, $p_lidid, $p_ondid);
		$nrid = 0;
		
		if (strlen($p_vanaf) < 8) {
			if ($this->ondtype == "A" and $this->alleenleden == 1) {
				$query = sprintf("SELECT IFNULL(MAX(LM.LIDDATUM), CURDATE()) FROM %sLidmaatschap AS LM WHERE LM.Lid=%d AND (LM.Opgezegd IS NULL) AND LM.LIDDATUM > DATE_SUB(CURDATE(), INTERVAL 1 MONTH);", TABLE_PREFIX, $this->lidid);
				$vanaf = $this->scalar($query);
			} elseif ($this->alleenleden == 1 and $i_lm->soortlid($this->lidid) == "Voormalig lid") {
				$vanaf = $i_lm->beginlidmaatschap($this->lidid);
			} else {
				$vanaf = date("Y-m-d");
			}
		} else {
			$vanaf = $p_vanaf;
		}
		
		$contrqry = sprintf("SELECT COUNT(*) FROM %s WHERE LO.Lid=%d AND LO.OnderdeelID=%d AND LO.Vanaf='%s';", $this->basefrom, $this->lidid, $this->ondid, $vanaf);
		
		if (strlen($this->ondnaam) == 0) {
			$this->mess = sprintf("OnderdeelID %d bestaat niet. Record wordt niet toegevoegd.", $this->ondid);
			
		} elseif (strlen($this->lidnaam) == 0) {
			$this->mess = sprintf("Lid %d bestaat niet. Dit record wordt niet toegevoegd.", $this->lidid);
			
		} elseif ($this->alleenleden == 1 and $i_lm->soortlid($this->lidid, $vanaf) !== "Lid") {
			$this->mess = sprintf("Dit record wordt niet toegevoegd, want de persoon is op %s geen lid.", $vanaf);
			
		} elseif ($this->scalar($contrqry) > 0) {
			$this->mess = sprintf("Dit record wordt niet toegevoegd, want de combinatie van Lid (%d), Onderdeel (%d) en vanaf (%s) al in de tabel staat.", $this->lidid, $this->ondid, $vanaf);
			
		} else {
			$nrid = $this->nieuwrecordid();
			$query = sprintf("INSERT INTO %s (RecordID, Lid, OnderdeelID, Vanaf, Functie, GroepID, Ingevoerd) VALUES (%d, %d, %d, '%s', 0, 0, NOW());", $this->table, $nrid, $this->lidid, $this->ondid, $vanaf);
			if ($this->execsql($query) > 0) {
				$this->mess = sprintf("%s is aan '%s' toegevoegd", $this->lidnaam, $this->ondnaam);
				if (strlen($p_reden) > 0) {
					$this->mess .= ", omdat " . $p_reden;
				}
				$this->loid = $nrid;
				(new cls_Interface())->add($query, $this->lidid);
				$rv = true;
			} else {
				$this->mess = sprintf("Geen record toegevoegd: %s", $query);
			}
		}
		$this->log($nrid, 0);
		return $rv;
	}
	
	public function update($p_loid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_loid);
		$rv = false;
		$this->tm = 0;
		
		if ($p_kolom == "GroepID") {
			$this->ta = 19;
			$this->tas = 0;
		} else {
			$this->ta = 6;
		}
		if ($this->tas != 32 and $this->ta == 6) {
			$this->tas = 12;
		}
		
		if (in_array($p_kolom, array("Lid", "OnderdeelID", "GroepID", "Functie"))) {
			if (strlen($p_waarde) == 0 or intval($p_waarde) <= 0) {
				$p_waarde = 0;
			} else {
				$p_waarde = intval($p_waarde);
			}
		}

		if ($p_kolom == "Lid" and $p_waarde <= 0) {
			$this->mess = sprintf("%s: De kolom 'Lid' mag geen 0 zijn, deze aanpassing wordt niet verwerkt.", $this->table);
			$this->tm = 1;
		} elseif ($p_kolom == "OnderdeelID" and $p_waarde <= 0) {
			$this->mess = sprintf("%s: De kolom 'OnderdeelID' mag geen 0 zijn, deze aanpassing wordt niet verwerkt.", $this->table);
			$this->tm = 1;
		} elseif ($p_kolom == "Vanaf" and strlen($p_waarde) < 6) {
			$this->mess = sprintf("%s: 'Vanaf' mag niet leeg zijn, deze aanpassing wordt niet verwerkt.", $this->table);
			$this->tm = 1;
		} elseif ($p_kolom == "Vanaf" and $this->alleenleden == 1 and (new cls_Lidmaatschap())->soortlid($this->lidid, $p_waarde) !== "Lid") {
			$this->mess = sprintf("De persoon is geen lid op %s, de aanpassing van vanaf wordt niet doorgevoerd.", $p_waarde);
		} elseif ($this->typekolom($p_kolom) == "date" and strlen($p_waarde) > 5 and $p_waarde <= '1970-01-01') {
			$this->mess = sprintf("De waarde '%s' voor kolom '%s' is ongeldig, deze aanpassing wordt niet verwerkt.", $p_waarde, $p_kolom);
			$this->tm = 1;
		} else {
			if ($this->pdoupdate($this->loid, $p_kolom, $p_waarde, $p_reden) > 0) {
				$rv = true;
			}
		}
		$this->log($this->loid);
		return $rv;
	}
	
	public function delete($p_loid, $p_lidid=-1, $p_ondid=-1, $p_reden="") {
		if ($this->tas != 33 and $this->tas != 39) {
			$this->tas = 13;
		}
		
		if ($p_loid > 0) {
			$this->loid = $p_loid;
			$query = sprintf("DELETE FROM %s WHERE RecordID=%d;", $this->table, $this->loid);
		} else {
			$this->lidid = $p_lidid;
			$this->ondid = $p_ondid;
			$query = sprintf("DELETE FROM %s WHERE Lid=%d AND OnderdeelID=%d;", $this->table, $this->lidid, $this->ondid);
		}
		$this->vulvars($this->loid, $this->lidid, $this->ondid);
		$rv = $this->execsql($query);
		if ($rv > 0) {
			$this->mess = sprintf("%s: %s '%s' is bij %s verwijderd", $this->table, ARRTYPEONDERDEEL[$this->ondtype], $this->ondnaam, $this->lidnaam);
			if (strlen($p_reden) > 0) {
				$this->mess .= ", omdat " . $p_reden;
			}
			
			(new cls_Interface())->add($query, $this->lidid);
		}
		$this->log($this->loid, 0);
		return $rv;
	}
	
	public function zeteigenschap($p_lidid, $p_ondid, $p_waarde) {
		//Deze functie wordt ook voor toestemmingen gebruikt, omdat dit technisch bijna hetzelfde werkt.
		
		$this->vulvars(-1, $p_lidid, $p_ondid);
		$rv = false;
		
		if ($p_waarde == 1) {
			if ($this->loid == 0) {
				if ($this->add($p_ondid, $p_lidid)) {
					$rv = true;
				}
			} elseif ($this->ondtype == "T") {
				if ($this->update($this->loid, "Vanaf", date("Y-m-d"))) {
					$rv = true;
				}
			}
		} else {
			$query = sprintf("SELECT LO.RecordID FROM %s WHERE LO.Lid=%d AND LO.OnderdeelID=%d;", $this->basefrom, $p_lidid, $p_ondid);
			$result = $this->execsql($query);
			foreach ($result->fetchAll() as $row) {
				$this->delete($row->RecordID);
				$rv = true;
			}
		}
		
		return $rv;
	}
	
	public function opschonen($p_lidid=-1) {
		$i_ond = new cls_Onderdeel();
		$this->tas = 39;
		
		if ($p_lidid > 0) {
			$wl = sprintf(" AND LO.Lid=%d", $p_lidid);
		} else {
			$wl = "";
		}
		
		$query = sprintf("SELECT LO.RecordID, O.HistorieOpschonen FROM %s AS LO INNER JOIN %sOnderdl AS O ON LO.OnderdeelID=O.RecordID
						  WHERE O.HistorieOpschonen > 1 AND IFNULL(LO.Opgezegd, '9999-12-31') < DATE_SUB(CURDATE(), INTERVAL O.HistorieOpschonen DAY)%s
						  ORDER BY LO.OnderdeelID;", $this->table, TABLE_PREFIX, $wl);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$reden = sprintf("op basis van HistorieOpschonen (%d dagen)", $row->HistorieOpschonen);
			$this->delete($row->RecordID, 0, 0, $reden);
		}
		
		$query = sprintf('SELECT LO.RecordID FROM %s WHERE LO.OnderdeelID NOT IN (SELECT O.RecordID FROM %sOnderdl AS O);', $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$i_ond->delete($row->RecordID, 0, 0, "het onderdeel niet (meer) bestaat");
		}
		
		$query = sprintf("SELECT LO.RecordID FROM %s WHERE LO.Vanaf > IFNULL(LO.Opgezegd, '9999-12-31')%s;", $this->basefrom, $wl);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, 0, 0, "de datum vanaf na de datum opgezegd ligt");
		}
		
		$query = sprintf("SELECT LO.RecordID, IFNULL(LO.Opgezegd, '9999-12-31') AS Opgezegd, IFNULL(L.RecordID, 0) AS LidID, IFNULL(L.Overleden, '9999-12-31') AS Overleden
						  FROM %s AS LO LEFT OUTER JOIN %sLid AS L ON LO.Lid=L.RecordID WHERE 1=1%s", $this->table, TABLE_PREFIX, $wl);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			if ($row->LidID == 0) {
				$this->delete($row->RecordID, 0, 0, "het record in de tabel 'Lid' niet (meer) bestaat.");
			} elseif ($row->Overleden < $row->Opgezegd and $row->Overleden <= date("Y-m-d")) {
				$this->update($row->RecordID, "Opgezegd", $row->Overleden, "het lid overleden is.");
			}
		}
		
		$query = sprintf("SELECT LO.RecordID FROM %s WHERE LO.GroepID > 0 AND IFNULL(LO.Opgezegd, '9999-12-31') < DATE_SUB(CURDATE(), INTERVAL 3 MONTH);", $this->basefrom);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $lorow) {
			if ($this->update($lorow->RecordID, "GroepID", 0, "de activiteit is beëindigd.")) {
				$this->log($lorow->RecordID);
			}
		}
		
	}  # opschonen
	
	public function controle() {

		$i_fnk = new cls_Functie();
		
		$lorows = $this->basislijst("LO.Functie > 0");
		
		foreach ($lorows as $lorow) {			
			$f = sprintf("F.Nummer=%d", $lorow->Functie);
			if ($lorow->Functie > 0 and $i_fnk->aantal($f) == 0) {
				$this->update($lorow->RecordID, "Functie", 0, "de functie niet bestaat.");
				$this->Log($lorow->RecordID);
			}
		}
		
	}  # controle
	
	public function autogroepenbijwerken($p_altijdlog=0, $p_interval=90, $p_ondid=-1) {
		
		$starttijd = microtime(true);
		$rv = 0;
		
		$f = sprintf("TypeActiviteit=%d AND TypeActiviteitSpecifiek=30", $this->ta);
		$lagb = (new cls_Logboek())->max("A.DatumTijd", $f);
		if ($p_interval < 1) {
			$p_interval = 45;
		}
		$t = sprintf("-%d minutes", $p_interval);
		if ($lagb < date("Y-m-d H:i:s", strtotime($t))) {
		
			$f = "LENGTH(O.MySQL) > 10 AND IFNULL(O.VervallenPer, '9999-12-31') >= CURDATE()";
			if ($p_ondid > 0) {
				$f .= sprintf(" AND O.RecordID=%d", $p_ondid);
			}
			$ondrows = (new cls_Onderdeel())->basislijst($f);
			foreach ($ondrows as $ondrow) {
				$reden = "";
				if (startwith($ondrow->MySQL, "SELECT ") == false) {
					$this->mess = sprintf("Deze MySQL-code voor onderdeel %d (%s) wordt niet uitgevoerd, omdat deze niet met SELECT begint.", $ondrow->RecordID, $ondrow->Naam);
				} elseif ($this->controleersql($ondrow->MySQL) == true) {
					$sourceres = $this->execsql($ondrow->MySQL);
					$pk = $sourceres->getColumnMeta(0)['name'];
					$sourcerows = $sourceres->fetchAll();
				
					$targetrows = $this->lijst($ondrow->RecordID);
					foreach ($targetrows as $targetrow) {
						$aanw = false;
						foreach ($sourcerows as $sourcerow) {
							if ($sourcerow->{$pk} == $targetrow->Lid) {
								$aanw = true;
							}
						}
						$reden = "op basis van de MySQL-code hoort dit lid niet in deze groep.";
						if ($aanw == false) {
							$this->tas = 32;
							$this->update($targetrow->RecordID, "Opgezegd", date("Y-m-d", strtotime("yesterday")), $reden);
							$rv++;
						}
					}
			
					foreach($sourcerows as $sourcerow) {
						$lidid = $sourcerow->{$pk};
						$aanwqry = sprintf("SELECT COUNT(*) FROM %s WHERE %s AND LO.OnderdeelID=%d AND LO.Lid=%d;", $this->basefrom, cls_db_base::$wherelidond, $ondrow->RecordID, $lidid);
						if ($this->scalar($aanwqry) == 0 and ($ondrow->{'Alleen leden'} == 0 or (new cls_Lidmaatschap())->soortlid($lidid) == "Lid")) {
							$f = sprintf("Lid=%d AND OnderdeelID=%d AND Vanaf <= CURDATE() AND IFNULL(Opgezegd, '9999-12-31') >= DATE_SUB(CURDATE(), INTERVAL 7 DAY)", $lidid, $ondrow->RecordID);
							$reden = "op basis van de MySQL-code dit lid in deze groep hoort.";
							$rec = $this->max("RecordID", $f);
							if ($rec > 0) {
								$this->tas = 32;
								$this->update($rec, "Opgezegd", '', $reden);
							} else {
								$this->tas = 31;
								$this->add($ondrow->RecordID, $lidid, $reden);
							}
							$rv++;
						}
					}
				} else {
					$this->tas = 30;
					$this->mess = sprintf("De MySQL-code van onderdeel %d (%s) is niet uitvoerbaar.", $ondrow->RecordID, $ondrow->Naam);
				}
				$this->log();
			}
		
			$query = sprintf("SELECT LO.RecordID, LO.Lid, LO.Vanaf, IFNULL(LO.Opgezegd, CURDATE()) AS Opgezegd FROM %1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON O.RecordID=LO.OnderdeelID WHERE O.`Alleen leden`=1 AND LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE();", TABLE_PREFIX);
			$result = $this->execsql($query);
			foreach ($result->fetchAll() as $row) {
				if ((new cls_Lidmaatschap())->soortlid($row->Lid, $row->Opgezegd) != "Lid") {
					$reden = "omdat de persoon geen lid van de vereniging (meer) is";
					$this->tas = 32;
					$this->update($row->RecordID, "Opgezegd", date("Y-m-d", strtotime("Yesterday")), $reden);
					$rv++;
				} elseif ((new cls_Lidmaatschap())->soortlid($row->Lid, $row->Vanaf) != "Lid") {
					$reden = "omdat de persoon toen nog geen lid van de vereniging was";
					$this->tas = 32;
					$this->update($row->RecordID, "Vanaf", date("Y-m-d", strtotime("Yesterday")), $reden);
					$rv++;
				}
			}
	
			$query = sprintf("SELECT LO.RecordID, O.VervallenPer FROM %1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON O.RecordID=LO.OnderdeelID WHERE IFNULL(O.VervallenPer, '9999-12-31') < IFNULL(LO.Opgezegd, '9999-12-31');", TABLE_PREFIX);
			$result = $this->execsql($query);
			$reden = "het onderdeel per die datum vervallen is.";
			foreach ($result->fetchAll() as $row) {
				$this->tas = 32;
				$this->update($row->RecordID, "Opgezegd", $row->VervallenPer, $reden);
				$rv++;
			}
		
			if ($rv > 0 or $p_altijdlog == 1) {
				$this->mess = sprintf("autogroepenbijwerken uitgevoerd, %d aanpassingen, in %.3f seconden, gedaan.", $rv, (microtime(true) - $starttijd));
				$this->tas = 30;
				$this->lidid = 0;
				$this->log();
			}
		}
		return $rv;
		
	} # autogroepenbijwerken
	
	public function auto_einde($p_ondid=-1) {
		$starttijd = microtime(true);
		
		$rv = 0;
		$i_ond = new cls_Onderdeel();
		$i_lm = new cls_Lidmaatschap();
		$this->tas = 38;
		
		$f = sprintf("TypeActiviteit=%d AND TypeActiviteitSpecifiek=%d", $this->ta, $this->tas);
		$lagb = (new cls_Logboek())->max("A.DatumTijd", $f);
		
		if (($lagb < date("Y-m-d H:i:s", strtotime("-90 minutes"))) or $p_ondid > 0) {
		
			$f = "`Alleen leden`=1";
			if ($p_ondid > 0) {
				$f .= sprintf(" AND O.RecordID=%d", $p_ondid);
			}
			$ondrows = $i_ond->lijst(1, $f);
			foreach ($ondrows as $ondrow) {
				$lorows = $this->onderdeellijst($ondrow->RecordID, 3);
				foreach ($lorows as $lorow) {
					$eindelm = $i_lm->eindelidmaatschap($lorow->LidID);
					if ($eindelm < "9999-12-31" and (($ondrow->Type == "A" and $eindelm <= date("Y-m-d", strtotime("+6 month"))) or $eindelm <= date("Y-m-d"))) {
						$this->update($lorow->RecordID, "Opgezegd", $eindelm, sprintf("het lidmaatschap per %s is beëindigd.", $eindelm));
						$rv++;
					}
				}
			}
			
			$exec_tijd = (microtime(true) - $starttijd);
			if (isset($_SESSION['settings']['performance_trage_select']) and $_SESSION['settings']['performance_trage_select'] > 0 and $exec_tijd >= $_SESSION['settings']['performance_trage_select']) {
				$this->mess = sprintf("cls_Lidond->auto_einde in %.3f seconden uitgevoerd.", $exec_tijd);
				$this->Log();
			}
		}
		
		$i_ond = null;
		$i_lm = null;
		
		return $rv;
	}
	
}  # cls_Lidond

class cls_Activiteit extends cls_db_base {
	private $actid = 0;  // RecordID van de activiteit
	
	function __construct($p_actid=-1) {
		$this->table = TABLE_PREFIX . "Activiteit";
		$this->basefrom = $this->table . " AS Act";
		$this->ta = 20;
		if ($p_actid > 0) {
			$this->actid = $p_actid;
		}
	}
	
	public function htmloptions($p_cv=-1) {
		
		$ret = "<option value=0>Geen</option>\n";
		$rows = $this->basislijst("", "Act.Code");
		foreach ($rows as $row) {
			$ret .= sprintf("<option%s value=%d>%s</option>\n", checked($row->RecordID, "option", $p_cv), $row->RecordID, $row->Code);
		}
		return $ret;
	}
	
	public function add() {
		$nrid = $this->nieuwrecordid();
		
		$this->query = sprintf("INSERT INTO %1\$s (RecordID, Omschrijving, Contributie, Ingevoerd) VALUES (%2\$d, 'Activiteit %2\$d', 0, NOW());", $this->table, $nrid);
		if ($this->execsql() > 0) {
			$this->mess = sprintf("Activiteit %d is toegevoegd.", $nrid);
			$this->Interface($this->query);
		}
		
		$this->log($nrid);
	}
	
	public function update($p_actid, $p_kolom, $p_waarde) {
		if ($p_kolom == "Contributie") {
			if (intval($p_waarde) <= 0 or strlen($p_waarde) == 0) {
				$p_waarde = 0;
			} else {
				$p_waarde = round($p_waarde, 2);
			}
		}
		$this->pdoupdate($p_actid, $p_kolom, $p_waarde);
		$this->log($p_actid);
	}
	
}  # cls_Activiteit

class cls_Groep extends cls_db_base {
	// Afdelingsgroepen
	
	private $afdid = 0;
	private $grid = -1;
	
	function __construct($p_afdid=-1, $p_grid=-1) {
		$this->table = TABLE_PREFIX . "Groep";
		$this->basefrom = $this->table . " AS GR";
		$this->ta = 19;
		if ($p_afdid >= 0) {
			$this->afdid = $p_afdid;
		}
		if ($p_grid >= 0) {
			$this->grid = $p_grid;
		}
	}
	
	public function record($p_grid=-1) {
		
		if ($p_grid >= 0) {
			$this->grid = $p_grid;
		}
		
		$query = sprintf("SELECT GR.* FROM %s WHERE GR.RecordID=%d;", $this->basefrom, $this->grid);
		$result = $this->execsql($query);
		
		return $result->fetch();
	}
	
	public function selectlijst($p_afdid=-1, $p_order="") {
		if ($p_afdid >= 0) {
			$this->afdid = $p_afdid;
		}
		if (strlen($p_order) > 0) {
			$p_order .= ", ";
		}
		
		$query = sprintf("SELECT GR.*,
						  IF(GR.RecordID=0, 'Niet ingedeeld', CONCAT(GR.Kode, ' - ', GR.Omschrijving)) AS GroepOms,
						  CASE 
							WHEN GR.RecordID=0 THEN 'Niet ingedeeld'
							WHEN LENGTH(Instructeurs) > 1 THEN CONCAT(GR.Kode, ' - ', GR.Omschrijving, ' | ', GR.Instructeurs)
							ELSE CONCAT(GR.Kode, ' - ', GR.Omschrijving)
						  END AS GroepOmsIns,
						  (SELECT COUNT(*) FROM %1\$sLidond AS LO WHERE LO.GroepID=GR.RecordID AND LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE() AND LO.OnderdeelID=%3\$d) AS aantalInGroep
						  FROM %2\$s
						  WHERE (GR.OnderdeelID=%3\$d OR GR.RecordID=0)
						  ORDER BY GR.Starttijd, GR.Volgnummer, GR.Omschrijving;", TABLE_PREFIX, $this->basefrom, $this->afdid);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function add($p_onderdeelid, $p_kode="") {
		$nrid = $this->nieuwrecordid();
		
		$this->query = sprintf("INSERT INTO %s (RecordID, OnderdeelID, Volgnummer, Kode, DiplomaID, Ingevoerd) VALUES (%d, %d, 1, '%s', 0, NOW());", $this->table, $nrid, $p_onderdeelid, $p_kode);
		if ($this->execsql() > 0) {
			$this->mess = sprintf("Groep %d is toegevoegd.", $nrid);
			$this->Interface($this->query);
		}
	}
	
	public function update($p_grid, $p_kolom, $p_waarde) {
		if (($p_kolom == "Volgnummer" or $p_kolom == "Aanwezigheidsnorm") and (intval($p_waarde) <= 0 or strlen($p_waarde) == 0)) {
			$p_waarde = 0;
		} elseif ($p_kolom == "Aanwezigheidsnorm" and intval($p_waarde) > 100) {
			$p_waarde = 100;
		} elseif (($p_kolom == "Starttijd" or $p_kolom == "Eindtijd") and strlen($p_waarde) == 4 and substr($p_waarde, 0, 1) != "0") {
			$p_waarde = "0" . $p_waarde;
		}
		$this->pdoupdate($p_grid, $p_kolom, $p_waarde);
		$this->log($p_grid);
	}
	
	private function delete($p_grid, $p_reden="") {
		$this->grid = $p_grid;
		$cntrqry = sprintf("SELECT COUNT(*) FROM %sLidond AS LO WHERE GroepID=%d", TABLE_PREFIX, $this->grid);
		if ($this->scalar($cntrqry) > 0) {
			$this->mess = sprintf("Groep %d wordt niet verwijderd, omdat deze nog in gebruik is.", $p_grid);
		} elseif ($this->pdodelete($p_grid) > 0) {
			if (strlen($p_reden) > 0) {
				$this->mess = sprintf("Groep %d is verwijderd, omdat %s.", $p_grid, $p_reden);
			}
		} else {
			$this->mess = sprintf("Groep %d is niet verwijderd, omdat deze niet bestaat.", $p_grid);
		}
		$this->log($p_grid);
	}
	
	public function opschonen() {
		$query = sprintf("SELECT GR.RecordID FROM %s WHERE DATE_ADD(GR.Ingevoerd, INTERVAL %d DAY) < CURDATE() AND (SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.GroepiD=GR.RecordID)=0;", $this->basefrom, BEWAARTIJDNIEUWERECORDS, TABLE_PREFIX);
		$result = $this->execsql($query);
		foreach($result->fetchAll() as $row) {
			$this->delete($row->RecordID, "deze groep niet meer wordt gebruikt.");
		}
	}
	
	public function controle() {
		
	}
	
}  # cls_Groep

class cls_Login extends cls_db_base {
	
	private $login = "";
	public $loginid = 0;
	public $beperkttotgroep;
	private $filteremail = "";
	
	function __construct() {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Admin_login";
		$this->basefrom = $this->table . " AS Login";
		$this->ta = 5;
		
		if (isset($_SESSION['settings']['login_beperkttotgroep']) and strlen($_SESSION['settings']['login_beperkttotgroep']) > 0) {
			$this->beperkttotgroep = explode(",", $_SESSION['settings']['login_beperkttotgroep']);
		} else {
			$this->beperkttotgroep = array(0);
		}
	}
	
	private function vulvars($p_lidid=0, $p_email="") {
		if ($p_lidid > 0) {
			$this->lidid = $p_lidid;
		}
		
		if ($this->lidid > 0) {
			$query = sprintf("SELECT RecordID, Login FROM %s WHERE LidID=%d;", $this->table, $this->lidid);
			$row = $this->execsql($query)->fetch();
			if (isset($row->RecordID)) {
				$this->loginid = $row->RecordID;
				$this->login = $row->Login;
			}
		}
		
		$p_email = trim(strtolower($p_email));
		if (isValidMailAddress($p_email, 0)) {
			$this->filteremail = sprintf("(LOWER(L.Email)='%1\$s' OR LOWER(L.EmailVereniging)='%1\$s' OR LOWER(L.EmailOuders)='%1\$s')", $p_email);
		} else {
			$this->filteremail = "";
		}
	}
		
	public function lijst($p_filter="", $p_orderby="", $p_email="") {
		$this->vulvars(0, $p_email);
		$filter = "";
		if (strlen($p_filter) > 0) {
			$p_filter = str_replace("Lidnr", "(" . $this->selectlidnr . ")", $p_filter);
			$filter = " AND " . $p_filter;
		}
		if (strlen($this->filteremail) > 0) {
			$filter .= " AND " . $this->filteremail;
		}
		
		if (strlen($p_orderby) > 0) {
			$p_orderby .= ", ";
		}
		
		$query = sprintf("SELECT Login.Login, %1\$s AS Naam_lid, L.Woonplaats, (%5\$s) AS Lidnr, IF(LENGTH(L.Email)=0, L.EmailOuders, L.Email) AS `E-mail`, Login.Ingevoerd, Login.LastLogin, 
					IF(LENGTH(Login.Wachtwoord) > 5 AND LENGTH(IFNULL(Login.ActivatieKey, ''))=0, IF(Login.Ingelogd=1, 'Ingelogd', 'Gevalideerd'), 'Niet gevalideerd') AS `Status`,
					IF(Login.FouteLogin > 0, Login.LidID, 0) AS `Unlock`, Login.LidID,
					IF(LENGTH(IFNULL(Login.ActivatieKey, ''))>0, Login.LidID, 0) AS ValLink,
					L.Roepnaam, L.Telefoon, L.Mobiel, L.Achternaam, L.Tussenv, L.Meisjesnm, L.GEBDATUM, L.EmailVereniging, Login.LidID,
					Login.Wachtwoord
					FROM %2\$sAdmin_login AS Login INNER JOIN %2\$sLid AS L ON Login.LidID=L.RecordID
					WHERE LENGTH(Login.Login) > 5 %3\$s
					ORDER BY %4\$sL.Achternaam, L.TUSSENV, L.Roepnaam;", $this->selectnaam, TABLE_PREFIX, $filter, $p_orderby, $this->selectlidnr);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function record($p_lidid, $p_email="") {
		$this->vulvars($p_lidid, $p_email);
		
		$query = sprintf("SELECT Login.*, %s AS Naam, L.Roepnaam, L.Email, Login.LidID, ", $this->selectnaam);
		
		$query .= sprintf("(SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.Lid=L.RecordID AND LO.OnderdeelID IN (%s) AND LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE()) AS AanvragenMag", TABLE_PREFIX, implode(", ", $this->beperkttotgroep));
		$query .= sprintf(" FROM %1\$sLid AS L LEFT JOIN %1\$sAdmin_login AS Login ON L.RecordID=Login.LidID WHERE ", TABLE_PREFIX);
		
		if (strlen($this->filteremail) > 0) {
			$query .= $this->filteremail;
		} else {
			$query .= sprintf("L.RecordID=%d;", $p_lidid);
		}
		
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
	public function lididherstel($p_login, $p_lidnr, $p_email) {
		$this->vulvars(0, $p_email);
		
		$query = sprintf("SELECT Login.LidID FROM (%1\$sAdmin_login AS Login INNER JOIN %1\$sLid AS L ON L.RecordID=Login.LidID) INNER JOIN %1\$sLidmaatschap AS LM ON LM.Lid=L.RecordID WHERE IFNULL(LM.Opgezegd, '9999-12-31') >= CURDATE() AND ", TABLE_PREFIX);
	
		if (strlen($p_login) > 5 or $p_lidnr > 0) {
			$query .= sprintf("(Login.Login='%s' OR LM.Lidnr=%d);", $p_login, $p_lidnr);
		} elseif (IsValidMailAddress($p_email, 0)) {
			$query .= $this->filteremail . ";";
		} else {
			$query .= "1=2;";
		}
		
		$result = $this->execsql($query);
		$rows = $result->fetchAll();
		if (count($rows) == 1) {
			return $rows[0]->LidID;
		} else {
			return 0;
		}
	}
	
	public function lidid($p_email, $p_lidnr=0) {
		$this->vulvars(0, $p_email);
		
		if (strlen($this->filteremail) > 0) {
			$w = $this->filteremail;
			if ($p_lidnr > 0) {
				$w .= sprintf(" AND LM.Lidnr=%d", $p_lidnr);
			}
		} else {
			$w = "1=2";
		}
		$query = sprintf("SELECT DISTINCT L.RecordID FROM %s WHERE %s;", $this->fromlid, $w);
		
		$result = $this->execsql($query);
		$rows = $result->fetchAll();
		if (count($rows) == 1) {
			return $rows[0]->RecordID;
		} elseif (count($rows) > 1) {
			return -1;
		} else {
			return 0;
		}
	}
	
	public function aanvragenmag($p_lidid) {
		global $lididtestusers;
		
		if (in_array($p_lidid, LIDIDWEBMASTERS) == true or in_array($p_lidid, $lididtestusers) == true) {
			return true;
		} else {
			$query = sprintf("SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE() AND LO.Lid=%d AND LO.OnderdeelID IN (%s);", TABLE_PREFIX, $p_lidid, implode(",", $this->beperkttotgroep));
			if ($this->scalar($query) > 0) {
				return true;
			} else {
				return false;
			}
		}
	}

	public function lididbijlogin($p_login) {
		$query = sprintf("SELECT IFNULL(MAX(LidID), 0) FROM %s WHERE Login='%s';", $this->table, $p_login);
		return $this->scalar($query);
	}

	public function controle($p_wachtwoord, $p_fromcookie=0) {
		
		$login = $_SESSION['username'];
		
		if ($_SESSION['settings']['login_autounlock'] > 0) {
			$query = sprintf("SELECT Login, LidID FROM %s WHERE Gewijzigd < DATE_SUB(SYSDATE(), INTERVAL %d MINUTE) AND FouteLogin > 0;", $this->table, $_SESSION['settings']['login_autounlock']);
			$result = $this->execsql($query);
			foreach ($result->fetchAll() as $row) {
				
				$updqry = sprintf("UPDATE %s SET FouteLogin=0, Gewijzigd=SYSDATE() WHERE Login='%s';", $this->table, $row->Login);
				$updres = $this->execsql($updqry);
				if ($updres > 0) {
					$this->mess = sprintf("Login '%s' is automatisch gedeblokkeerd.", $row->Login);
					$this->ta = 5;
					$this->tas = 5;
					$this->lidid = $row->LidID;
					$this->log();
				}
			}
		}
		
		// Iemand logt in met zijn/haar e-mailadres, dit mag alleen als deze uniek is in de database.
		if (isValidMailAddress($login, 0)) {
			$query = sprintf("SELECT Login.Login, Login.LidID FROM %1\$sLid AS L INNER JOIN %1\$sAdmin_login AS Login ON Login.LidID=L.RecordID WHERE (LOWER(L.Email) LIKE '%2\$s' OR LOWER(L.EmailVereniging) LIKE '%2\$s')", TABLE_PREFIX, strtolower($login));
			$result = $this->execsql($query);
			$rows = $result->fetchAll();
			if (count($rows) == 1) {
				$login = $rows[0]->Login;
			}
		}
		
		$xw = sprintf("AND Login.Login LIKE '%s'", cleanlogin($login));
		if (is_array($this->beperkttotgroep) and count($this->beperkttotgroep) > 0) {
			$xw .= sprintf(" AND (Login.LidID IN (SELECT Lid FROM %sLidond AS LO WHERE %s AND LO.OnderdeelID IN (%s)) OR Login.LidID IN (%s))", TABLE_PREFIX, cls_db_base::$wherelidond, implode(", ", $this->beperkttotgroep), implode(", ", LIDIDWEBMASTERS));
		} else {
			$xw .= sprintf(" AND Login.LidID IN (%s)", implode(", ", LIDIDWEBMASTERS));
		}
		$query = sprintf("SELECT Login.LidID, Login.Login, %3\$s AS NaamLid, L.Roepnaam, L.Email, L.EmailVereniging, L.EmailOuders, Login.Wachtwoord, Login.ActivatieKey,
					IFNULL((%4\$s), 0) AS Lidnr, Login.FouteLogin, Login.Gewijzigd
					FROM %1\$sAdmin_login AS Login INNER JOIN %1\$sLid AS L ON Login.LidID = L.RecordID
					WHERE LENGTH(Login.Wachtwoord) > 5 %2\$s
					ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", TABLE_PREFIX, $xw, $this->selectnaam, $this->selectlidnr);
		$result = $this->execsql($query);
		$row = $result->fetch();
		if (isset($row->LidID) and $row->LidID > 0) {
			$this->lidid = $row->LidID;
			if (password_verify($p_wachtwoord, $row->Wachtwoord)) {
				return $row;
			} else {
				if ($p_fromcookie == 0) {
					$query = sprintf("UPDATE %s SET FouteLogin=FouteLogin+1, Gewijzigd=SYSDATE() WHERE Login='%s';", $this->table, $login);
					$this->execsql($query);
				}
				return false;
			}
		} else {
			return false;
		}
	}

	public function valideerlogin($p_lidid, $p_key) {
		$this->lidid = $p_lidid;
		$query = sprintf("UPDATE %s SET ActivatieKey='', Gewijzigd=SYSDATE() WHERE ActivatieKey='%s' AND LidID=%d;", $this->table, $p_key, $p_lidid);
		$result = $this->execsql($query);
		if ($result == 1) {
			$this->mess = "Login is gevalideerd en kan worden gebruikt.";
		} else {
			$this->mess = sprintf("Er ging iets mis bij het valideren van de login voor %s is gevalideerd. Mogelijk is deze login al gevalideerd. Zo niet probeer het later opnieuw.", (new cls_Lid())->Naam($p_lidid));
		}
		$this->tas = 4;
		$this->log(0, 1);
		
		return $this->mess;
	}
	
	public function vernieuwactivatiekeys() {
		
		if ($_SESSION['settings']['login_geldigheidactivatie'] > 0) {
			$query = sprintf("SELECT Login.LidID FROM %s AS Login WHERE Login.Ingelogd=0 AND ((LENGTH(Login.ActivatieKey) > 5 AND IFNULL(Login.Gewijzigd, '1970-01-01') < ADDDATE(SYSDATE(), INTERVAL -%2\$d HOUR)) OR LENGTH(Login.Wachtwoord) < 5);", $this->table, $_SESSION['settings']['login_geldigheidactivatie']);
			$result = $this->execsql($query);
			foreach($result->fetchAll() as $row) {
				$this->nieuweactivitiekey($row->LidID);
			}
		}
	}
	
	public function nieuweactivitiekey($p_lidid) {
		$this->vulvars($p_lidid);
		$nk = newkey();
		$this->tas = 7;
		
		if (strlen($nk) > 5 and $this->loginid > 0) {
			if ($this->pdoupdate($this->loginid, "ActivatieKey", $nk) > 0) {
				$this->mess = "Er is een nieuwe activatiekey gemaakt voor de login.";
				$this->log($this->loginid);
				return $nk;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}
	
	public function wachtwoordreset($p_lidid) {
		$this->vulvars($p_lidid);
		$this->tas = 8;
		$nk = newkey();
		if (strlen($nk) > 0) {
			$query = sprintf("UPDATE %sAdmin_login SET ActivatieKey='%s', Wachtwoord='', LaatsteWachtwoordWijziging=SYSDATE(), Gewijzigd=SYSDATE(), GewijzigdDoor=%d WHERE LidID=%d;", TABLE_PREFIX, password_hash($nk, PASSWORD_DEFAULT), $_SESSION['lidid'], $p_lidid);
			if ($this->execsql($query) == 1) {
				$this->mess = "Het wachtwoord is leeg gemaakt, zodat het hersteld kan worden.";
				$this->log($this->loginid);
				return $nk;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}
	
	public function add($p_lidid, $p_login, $p_wachtwoord="") {
		$this->lidid = $p_lidid;
		$p_login = cleanlogin($p_login);
		$this->tm = 1;
		$this->tas = 1;
		$nrid = 0;
		
		$query = sprintf("SELECT Login FROM %sAdmin_login WHERE LidID=%d;", TABLE_PREFIX, $p_lidid);
		if ($this->aantal(sprintf("LidID=%d", $p_lidid)) > 0) {
			$this->mess = "Heeft al een login, er wordt geen nieuwe aangemaakt.";
		} elseif (strlen($p_login) < 6) {
			$this->mess = "De login moet minimaal 6 karakters lang zijn.";
		} elseif ($this->aantal(sprintf("Login='%s'", $p_login)) > 0) {
			$this->mess = sprintf("Login '%s' bestaat al en kan niet nogmaals worden aangemaakt.", $p_login);
		} elseif (strlen($p_wachtwoord) < $_SESSION['settings']['wachtwoord_minlengte']) {
			$this->mess = sprintf("Het wachtwoord moet minimaal %d karakters lang zijn.", $_SESSION['settings']['wachtwoord_minlengte']);
		} elseif ($this->lidid > 0) {
			$nrid = $this->nieuwrecordid();
			$nk = password_hash(newkey(), PASSWORD_DEFAULT);
			$query = sprintf("INSERT INTO %s (RecordID, LidID, Login, Wachtwoord, ActivatieKey, LaatsteWachtwoordWijziging) VALUES 
											 (%d, %d, '%s', '%s', '%s', SYSDATE());", $this->table, $nrid, $this->lidid, $p_login, password_hash($p_wachtwoord, PASSWORD_DEFAULT), $nk);
			if ($this->execsql($query) == 1) {
				$this->mess = sprintf("Login '%s' is aangemaakt.", $p_login);
				$this->mess .= fnValidatieLogin($this->lidid, $nk, "mail");
			} else {
				$this->mess = sprintf("Er ging iets mis bij het aanmaken van login '%s'.", $p_login);
				$nrid = 0;
			}
		}
		$this->log($nrid);
		return $nrid;
	}
	
	public function update($p_lidid, $p_kolom, $p_waarde, $p_opstart=0) {
		$this->tas = 2;
		
		if ($_SESSION['webmaster'] == 1 or $p_opstart == 1) {
			$this->vulvars($p_lidid);
			if ($this->pdoupdate($this->loginid, $p_kolom, $p_waarde) > 0) {
				$this->mess = sprintf("Kolom '%s' van login '%s' in '%s' gewijzigd.", $p_kolom, $this->login, $p_waarde);
			}
		} else {
			$this->mess = "Je bent niet bevoegd om logins te wijzigen.";
		}
		$this->log($this->loginid);
	}
	
	public function delete($p_lidid, $p_reden="") {
		$this->vulvars($p_lidid);
		$this->tas = 3;
		
		if ($this->pdodelete($this->loginid) > 0) {
			$this->mess = sprintf("Login %d (%s) is verwijderd", $this->loginid, $this->login);
			if (strlen($p_reden) > 0) {
				$this->mess .= ", omdat " . $p_reden;
			}
			$this->log($this->loginid);
		}
	}
	
	public function setlastactivity() {
		$query = sprintf("UPDATE %s SET LastActivity=SYSDATE() WHERE Ingelogd=1 AND LidID=%d;", $this->table, $_SESSION['lidid']);
		$this->execsql($query);
	}
	
	public function autounlock() {
		$rv = 0;
		if ($_SESSION['settings']['login_autounlock'] > 0) {
			$query = sprintf("SELECT Login, LidID FROM %s WHERE Gewijzigd < DATE_SUB(SYSDATE(), INTERVAL %d MINUTE) AND FouteLogin > 0;", $this->table, $_SESSION['settings']['login_autounlock']);
			$result = $this->execsql($query);
			foreach ($result->fetchAll() as $row) {
				$updqry = sprintf("UPDATE %s SET FouteLogin=0, Gewijzigd=SYSDATE() WHERE Login='%s';", $this->table, $row->Login);
				if ($this->execsql($updqry) > 0) {
					$this->lidid = $row->LidID;
					$this->mess = sprintf("Login %s is automatisch gedeblokkeerd.", $row->Login);
					$this->tas = 5;
					$this->log();
					$rv++;
				}
			}
		}
		return $rv;
	}

	public function setingelogd($p_lidid, $p_napost=0) {
		$this->lidid = $p_lidid;
		$this->ta = 1;
		if ($p_napost == 1) {
			$xw = "";
		} else {
			$xw = "AND Ingelogd=0";
		}
		
		$this->query = sprintf("UPDATE %s SET LastLogin=SYSDATE(), LastActivity=SYSDATE(), Ingelogd=1, FouteLogin=0 WHERE LidID=%d %s;", $this->table, $this->lidid, $xw);
		if ($this->execsql() > 0) {
			$this->tas = 1;
			$this->mess = sprintf("Heeft met '%s' ingelogd.", $_SESSION['username']);
			$f = sprintf("ReferLidID=%d AND IP_adres='%s' AND TypeActiviteit=1", $p_lidid, $_SERVER['REMOTE_ADDR']);
			if ((new cls_Logboek())->aantal($f) == 0) {
				$this->mess .= sprintf(" Dit is voor het eerst vanaf IP-adres %s.", $_SERVER['REMOTE_ADDR']);
				if ($_SESSION['settings']['mailing_meldingnieuwip'] > 0) {
				}
			}
			$this->log();
		} elseif ($_SESSION['lidid'] > 0) {
			$this->query = sprintf("UPDATE %s SET LastActivity=SYSDATE() WHERE LidID=%d;", $this->table, $_SESSION['lidid']);
			$this->execsql();
		}
	}
	
	public function uitloggen($p_lidid=0) {
		$this->ta = 1;
		$this->tas = 2;
		$rv = 0;
		if ($p_lidid > 0) {
			$this->lidid = $p_lidid;
			$query = sprintf("UPDATE %s SET Ingelogd=0 WHERE Ingelogd > 0 AND LidID=%d;", $this->table, $p_lidid);
			if ($this->execsql($query) > 0) {
				$this->mess = "Heeft uitgelogd.";
				$this->log();
				$_SESSION['lidid'] = 0;
				$rv++;
			}
		} else {
			$query = sprintf("SELECT LidID FROM %s WHERE Ingelogd > 0 AND (LastActivity < ADDDATE(SYSDATE(), INTERVAL -%d MINUTE));", $this->table, session_cache_expire());
			$result = $this->execsql($query);
			foreach($result->fetchAll() as $row) {
				$updqry = sprintf("UPDATE %s SET Ingelogd=0 WHERE LidID=%d;", $this->table, $row->LidID);
				if ($this->execsql($updqry) > 0) {
					$this->lidid = $row->LidID;
					$this->mess = "Is automatisch uitgelogd.";
					$this->log(0, 0, 1);
					$rv++;
				}
			}
		}
		if ($rv > 0) {
			fnMaatwerkNaUitloggen();
		}
		return $rv;
	}
	
	public function wijzigenwachtwoord($p_nieuw, $p_key, $p_nieuwherh, $p_lidid) {
		$this->tm = 1;
		$this->tas = 6;
		$this->lidid = $p_lidid;
		$this->mess = "";
		$alphabet = "abcdefghijklmnopqrstuvwxyz";
	
		if (strlen($p_nieuw) > $_SESSION['settings']['wachtwoord_maxlengte'] and $_SESSION['settings']['wachtwoord_maxlengte'] >= 7) {
			$this->mess = sprintf("Het wachtwoord is te lang, het mag maximaal %d karakters lang zijn.", $_SESSION['settings']['wachtwoord_maxlengte']);
		} elseif (strlen($p_nieuw) < $_SESSION['settings']['wachtwoord_minlengte']) {
			$this->mess = sprintf("Het wachtwoord is te kort, het moet minimaal %d karakters lang zijn.", $_SESSION['settings']['wachtwoord_minlengte']);
		} elseif (strpos($p_nieuw, "'") > 0 or strpos($p_nieuw, "\"") > 0) {
			$this->mess = "Er mogen geen aanhalingstekens in een wachtwoord zitten.";
		} elseif (strpos($p_nieuw, " ") !== false) {
			$this->mess = "Het wachtwoord mag geen spatie bevatten.";
		} elseif (strlen($_SESSION['username']) > 0 and strpos(strtolower($p_nieuw), strtolower($_SESSION['username'])) !== false) {
			$this->mess = "Het wachtwoord mag niet je login bevatten.";
		} elseif (strlen($_SESSION['roepnaamingelogde']) > 2 and strpos(strtolower($p_nieuw), strtolower($_SESSION['roepnaamingelogde'])) !== false) {
			$this->mess = "Het wachtwoord mag niet je roepnaam bevatten.";
		} elseif (strlen($p_nieuwherh) == 0 or $p_nieuw !== $p_nieuwherh) {
			$this->mess = "De nieuwe wachtwoorden zijn niet aan elkaar gelijk.";
		} else {
			$bevatc=0;
			$bevatk=0;
			$bevath=0;
			for ($i=0; $i < strlen($p_nieuw); $i++) {
				if (strpos("0123456789", substr($p_nieuw, $i, 1)) !== false) {
					$bevatc=1;
				} elseif (strpos($alphabet, substr($p_nieuw, $i, 1)) !== false) {
					$bevatk=1;
				} elseif (strpos(strtoupper($alphabet), substr($p_nieuw, $i, 1)) !== false) {
					$bevath=1;
				}
			}
			if ($bevatc == 0) {
				$this->mess = "Het wachtwoord moet minimaal 1 cijfer bevatten.";
			} elseif ($bevatk == 0) {
				$this->mess = "Het wachtwoord moet minimaal 1 kleine letter bevatten.";
			} elseif ($bevath == 0) {
				$this->mess = "Het wachtwoord moet minimaal 1 hoofdletter bevatten.";
			}
		}
		
		if (strlen($p_key) > 0 and strlen($this->mess) == 0) {
			$query = sprintf("SELECT ActivatieKey from %s WHERE LidID=%d;", $this->table, $p_lidid);
			if (password_verify($p_key, $this->scalar($query)) == false) {
				$this->mess = "De activivatiekey is niet correct, wachtwoord is niet gewijzigd.";	
			}
		}
		
		if (strlen($this->mess) == 0) {
			$query = sprintf("UPDATE %s SET Gewijzigd=SYSDATE(), GewijzigdDoor=%d, Wachtwoord='%s', LaatsteWachtwoordWijziging=SYSDATE(), ActivatieKey='' WHERE LidID=%d;", $this->table, $_SESSION['lidid'], password_hash($p_nieuw, PASSWORD_DEFAULT), $p_lidid);
			if ($this->execsql($query) > 0) {
				$this->mess = "Het wachtwoord is gewijzigd.";
				fnMaatwerkNaWijzigenWachtwoord($p_lidid, $p_nieuw);
			} else {
				$this->mess = "Het wachtwoord is niet gewijzigd.";
			}
		}
		$this->log(0, 1);
		return $this->mess;
	}  # wijzigenwachtwoord
	
	public function opschonen() {
		global $lididwebmasters;
		
		if ($_SESSION['settings']['login_bewaartijd'] > 0) {
			$reden = sprintf("er meer dan %d maanden niet mee is ingelogd", $_SESSION['settings']['login_bewaartijd']);
			$query = sprintf("SELECT LidID FROM %1\$s WHERE LastLogin < DATE_ADD(CURDATE(), INTERVAL -%2\$d MONTH) AND Ingevoerd < DATE_ADD(CURDATE(), INTERVAL -%2\$d MONTH);", $this->table, $_SESSION['settings']['login_bewaartijd']);
			$result = $this->execsql($query);
			foreach ($result->fetchAll() as $row) {
				$this->delete($row->LidID, $reden);
			}
		}
		
		$mess = "";
		if (is_array($this->beperkttotgroep) and count($this->beperkttotgroep) > 0) {
			$reden = "dit lid niet tot een groep behoort die mag inloggen";
			$where = sprintf("LidID NOT IN (SELECT Lid FROM %sLidond AS LO WHERE %s AND LO.OnderdeelID IN (%s))", TABLE_PREFIX, cls_db_base::$wherelidond, implode(", ", $this->beperkttotgroep));
			$where .= sprintf(" AND (LidID NOT IN (%s))", implode(", ", $lididwebmasters));
			$query = sprintf("SELECT LidID FROM %s WHERE LastLogin <= DATE_ADD(CURDATE(), INTERVAL -1 WEEK) AND %s;", $this->table, $where);
			$result = $this->execsql($query);
			foreach ($result->fetchAll() as $row) {
				$this->delete($row->LidID, $reden);
			}
		}
		
		if ($_SESSION['settings']['login_bewaartijdnietgebruikt'] > 0) {
			$reden = sprintf("deze niet binnen %d dagen na aanvragen is gebruikt.", $_SESSION['settings']['login_bewaartijdnietgebruikt']);
			$query = sprintf("SELECT LidID FROM %s WHERE ((LastLogin IS NULL) OR LastLogin < '2012-01-01') AND Ingevoerd < DATE_ADD(CURDATE(), INTERVAL -%d DAY);", $this->table, $_SESSION['settings']['login_bewaartijdnietgebruikt']);
			$result = $this->execsql($query);
			foreach ($result->fetchAll() as $row) {
				$this->delete($row->LidID, $reden);
			}
		}
	}  # opschonen
	
}  # cls_Login

class cls_Mailing extends cls_db_base {
	
	private $mid = 0;
	public $zichtbaarwhere = "";
	
	function __construct() {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Mailing";
		$this->basefrom = $this->table . " AS M";
		$this->ta = 4;
		if ($_SESSION['webmaster'] == 0 and in_array($_SESSION['settings']['mailing_alle_zien'], explode(",", $_SESSION['lidgroepen'])) == false) {
			$this->zichtbaarwhere = sprintf("M.ZichtbaarVoor IN (%s)", $_SESSION['lidgroepen'], $_SESSION['lidid']);
		}
	}
	
	public function record($p_mid) {
		$query = sprintf("SELECT M.*, MV.Vanaf_naam, MV.Vanaf_email FROM %s AS M LEFT OUTER JOIN %sMailing_vanaf AS MV ON M.MailingVanafID=MV.RecordID WHERE M.RecordID=%d;", $this->table, TABLE_PREFIX, $p_mid);
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
	public function lijst($p_filter="") {
		$xtra_sel = "";
		if (strlen($this->zichtbaarwhere) > 0) {
			$w = $this->zichtbaarwhere;
		} else {
			$w = "1=1";
		}
		
		$orderby = "M.RecordID DESC";
		if ($p_filter == "Templates") {
			$w .= " AND M.template=1 AND (M.deleted_on IS NULL)";
			$orderby = "M.subject";
			
		} elseif ($p_filter == "Prullenbak") {
			$w .= " AND (NOT M.deleted_on IS NULL)";
			$orderby = "M.deleted_on DESC";
			$xtra_sel = sprintf(", CONCAT((SELECT %s FROM %sLid AS L WHERE L.RecordID=M.DeletedBy), ' & ', DATE_FORMAT(M.deleted_on, %s)) AS `mrgVerwijderd`", $this->selectnaam, TABLE_PREFIX, $this->fdtlang);
		
		} elseif ($p_filter == "Muteren") {
			$w .= " AND (M.deleted_on IS NULL)";
			$orderby = "M.template DESC, M.RecordID DESC";
			
		} elseif (strlen($p_filter) > 0) {
			$w .= " AND " . $p_filter;
			$w .= " AND (M.deleted_on IS NULL)";
			
		} else {
			$w .= " AND (M.deleted_on IS NULL)";
			$orderby = "M.template DESC, M.RecordID DESC";
		}
		
		$query = sprintf("SELECT M.RecordID, CONCAT(M.subject, ' & ', IFNULL(M.Opmerking, '')) AS Onderwerp_Opmerking, IFNULL(MV.Vanaf_naam, '') AS Van,
					CONCAT(%1\$s, ' & ', DATE_FORMAT(M.Gewijzigd, %6\$s, 'nl_NL')) AS laatstGewijzigd, M.subject, M.Opmerking, M.OmschrijvingOntvangers
					%2\$s,
					IF((SELECT COUNT(*) FROM %3\$sMailing_hist AS MH WHERE MH.MailingID=M.RecordID) > 0, M.RecordID, 0) AS llkHist
					FROM (%3\$sMailing AS M LEFT JOIN %3\$sMailing_vanaf AS MV ON M.MailingVanafID=MV.RecordID) LEFT JOIN %3\$sLid AS L ON M.GewijzigdDoor=L.RecordID
					WHERE %4\$s ORDER BY %5\$s;", $this->selectnaam, $xtra_sel, TABLE_PREFIX, $w, $orderby, $this->fdlang);
		$result = $this->execsql($query);
		return $result->fetchAll();
		
	}  # lijst
	
	public function htmloptions($p_cv=-1) {
		$rv = "";
		$f = sprintf("(M.RecordID NOT IN (%d, %d, %d, %d, %d, %d, %d))", $_SESSION['settings']['mailing_lidnr'], $_SESSION['settings']['mailing_validatielogin'], $_SESSION['settings']['mailing_herstellenwachtwoord'],
						$_SESSION['settings']['mailing_bewakinginschrijving'], $_SESSION['settings']['mailing_bevestigingbestelling'], $_SESSION['settings']['mailing_bevestigingopzegging'], $_SESSION['settings']['mailingbijadreswijziging']);
		foreach ($this->lijst($f) as $row) {
			$o = $row->subject;
			if (strlen($row->Opmerking) > 0) {
				$o .= " - " . $row->Opmerking;
			}
			if (strlen($row->OmschrijvingOntvangers) > 0) {
				$o .= " - " . $row->OmschrijvingOntvangers;
			}
			$s = "";
			if ($p_cv == $row->RecordID) {
				$s = " selected";
			}
			$rv .= sprintf("<option value=%d%s>%s</option>\n", $row->RecordID, $s, $o);
		}
		return $rv;
	}
	
	public function bestaat($p_mid) {
		$query = sprintf("SELECT COUNT(*) FROM %s AS M WHERE M.RecordID=%d AND IFNULL(M.deleted_on, '1900-01-01') < '2000-01-01';", $this->table, $p_mid);
		$result = $this->execsql($query);
		if ($result->fetchColumn() == 1) {
			return true;
		} else {
			return false;
		}
	}
	
	public function mogelijkeontvangers($p_mid, $p_filter=0) {
		
		/*
			p_filter
				0 = alle personen in de tabel lid
				1 = alleen leden
		*/
		
		$query = sprintf("SELECT L.RecordID AS LidID, %1\$s AS Zoeknaam_lid FROM %2\$sLid AS L
					WHERE (L.Verwijderd IS NULL) AND (L.Overleden IS NULL) AND (LENGTH(L.Email) > 5 OR LENGTH(L.EmailVereniging) > 5 OR LENGTH(L.EmailOuders) > 5)
					AND (L.RecordID NOT IN (SELECT R.LidID FROM %2\$sMailing_rcpt AS R WHERE R.MailingID=%3\$d))", $this->selectzoeknaam, TABLE_PREFIX, $p_mid);
		if ($p_filter == 1) {
			$query .= sprintf(" AND L.RecordID IN (SELECT LM.Lid FROM %sLidmaatschap AS LM WHERE %s)", TABLE_PREFIX, $this->wherelid);
		}
		$query .= " ORDER BY L.Achternaam, L.Roepnaam;";
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function add($p_subject) {
		$nrid = 0;
		$this->tas = 1;
		
		if (strlen($p_subject) < 4) {
			$this->mess = "Het onderwerp moet uit minimaal 4 karakers bestaan. Deze mailing wordt niet toegevoegd.";
		} else {
			$p_subject = str_replace("\"", "'", $p_subject);
			$nrid = $this->nieuwrecordid();
		
			$query = sprintf("INSERT INTO %s (RecordID, MailingID, subject, OmschrijvingOntvangers, IngevoerdDoor) VALUES (%d, %d, \"%s\", '', %d);", $this->table, $nrid, $nrid, $p_subject, $_SESSION['lidid']);
			if ($this->execsql($query) > 0) {
				$this->mess = sprintf("Mailing %d (%s) is toegevoegd.", $nrid, $p_subject);
			} else {
				$this->mess = "Geen mailing toegevoegd.";
				$nrid = 0;
			}
		}
		
		$this->log($nrid);
		return $nrid;
	}
	
	public function update($p_mid, $p_kolom, $p_waarde) {
		$this->tas = 2;
		
		if ($p_kolom == "subject" and strlen($p_waarde) < 4) {
			$this->mess = "Het onderwerp moeten minimaal uit vier karakters bestaan. Deze aanpassing wordt niet verwerkt.";
			$this->log($p_mid);

		} elseif ($this->pdoupdate($p_mid, $p_kolom, $p_waarde) > 0) {
			if (strlen($p_waarde) > 125) {
				$p_waarde = substr($p_waarde, 0, 121) . " ...";
			}
			$this->mess = sprintf("In mailing %d is kolom '%s' is in '%s' gewijzigd.", $p_mid, $p_kolom, $p_waarde);
			$this->log($p_mid);
		}
	}
	
	public function delete($p_mid) {
		$this->tas = 3;
		
		if ($this->pdodelete($p_mid) > 0) {
			$this->log($p_mid);
			$query = sprintf("DELETE FROM %sMailing_rcpt WHERE MailingID=%d;", TABLE_PREFIX, $p_mid);
			$this->execsql($query);
		}
	}
	
	public function trash($p_mid, $p_direction="in") {
		$this->tas = 4;
		
		if ($p_direction == "in") {
			$this->query = sprintf("UPDATE %s SET deleted_on=SYSDATE(), DeletedBy=%d WHERE RecordID=%d AND (deleted_on IS NULL);", $this->table, $_SESSION['lidid'], $p_mid);
			if ($this->execsql() > 0) {
				$this->mess = sprintf("Mailing %d is naar de prullenbak verplaatst.", $p_mid);
			}
		} else {
			$this->query = sprintf("UPDATE %s SET deleted_on=NULL WHERE RecordID=%d AND (deleted_on IS NOT NULL);", $this->table, $p_mid);
			if ($this->execsql() > 0) {
				$this->mess = sprintf("Mailing %d is uit de prullenbak gehaald.", $p_mid);
			}
		}
		
		$this->log($p_mid);
	}
	
	public function opschonen() {
		if ($_SESSION['settings']['mailing_bewaartijd'] > 0) {
			$query = sprintf("SELECT M.RecordID FROM %s AS M WHERE (deleted_on IS NOT NULL) AND deleted_on < ADDDATE(CURDATE(), INTERVAL -%d MONTH);", $this->table, $_SESSION['settings']['mailing_bewaartijd']);
			$result = $this->execsql($query);
			foreach ($result->fetchAll() as $row) {
				$this->delete($row->RecordID);
			}
		}

		$query = sprintf('DELETE FROM %1$sMailing_rcpt
						  WHERE MailingID NOT IN (SELECT M.RecordID FROM %1$sMailing AS M);', TABLE_PREFIX);
		$this->execsql($query, 2);

		$query = sprintf('UPDATE %1$sMailing_hist SET MailingID=0
						  WHERE MailingID > 0 AND MailingID NOT IN (SELECT M.RecordID FROM %1$sMailing AS M);', TABLE_PREFIX);
		$this->execsql($query, 2);
	}
		
}  # cls_Mailing

class cls_Mailing_hist extends cls_db_base {
	
	private $mhid = 0;
	private $mid = 0;
	private $zichtbaarwhere = "";
	private $limit = 1500;
		
	function __construct($p_mhid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Mailing_hist";
		$this->basefrom = $this->table . " AS MH";
		$this->ta = 4;
		if ($_SESSION['webmaster'] == 0 and in_array($_SESSION['settings']['mailing_alle_zien'], explode(",", $_SESSION['lidgroepen'])) == false) {
			$this->zichtbaarwhere = sprintf(" AND (MH.ZichtbaarVoor IN (%s) OR MH.LidID=%d)", $_SESSION['lidgroepen'], $_SESSION['lidid']);
		}
		$this->vulvars($p_mhid);
	}
	
	private function vulvars($p_mhid=-1, $p_tas=-1) {
		if ($p_mhid >= 0) {
			$this->mhid = $p_mhid;
		}
		if ($p_tas >= 0) {
			$this->tas = $p_tas;
		}
		if ($this->mhid > 0) {
			$query = sprintf("SELECT MH.RecordID, MH.LidID, MH.MailingID FROM %s WHERE MH.RecordID=%d;", $this->basefrom, $this->mhid);
			$result = $this->execsql($query);
			$row = $result->fetch();
			if (isset($row->RecordID)) {
				$this->lidid = $row->LidID;
				$this->mid = $row->MailingID;
			}
		}
	}

	public function record($p_mhid=-1, $p_mid=0) {
		$this->vulvars($p_mhid);
		
		if ($this->mhid <= 0 and $p_mid > 0) {
			$w = sprintf("M.RecordID=%d", $p_mid);
		} else {
			$w = sprintf("MH.RecordID=%d", $this->mhid);
		}
		
		$query = sprintf("SELECT MH.Ingevoerd, MH.send_on, MH.from_addr, MH.from_name, M.to_name, MH.to_addr, MH.cc_addr, MH.subject, MH.message, MH.send_by, MH.LidID, MH.to_name AS Aan, MH.LidID, 
						  MH.MailingID, MH.Xtra_Char, MH.Xtra_Num, MH.ZichtbaarVoor, MH.ZonderBriefpapier, MH.IngevoerdDoor, M.CCafdelingen
						  FROM (%1\$s LEFT JOIN %2\$sLid AS L ON MH.IngevoerdDoor=L.RecordID) LEFT JOIN %2\$sMailing AS M ON MH.MailingID=M.RecordID
						  WHERE %3\$s
						  ORDER BY Ingevoerd DESC LIMIT 1;", $this->basefrom, TABLE_PREFIX, $w, $this->selectnaam);
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
	public function laatste($p_filter="", $p_aantal=1) {
		
		if (strlen($p_filter) > 0) {
			$p_filter = "WHERE " . $p_filter;
		}
	
		$query = sprintf("SELECT IF((MH.send_on IS NULL) AND IFNULL(MH.RecordID, 0) > 0, 'in outbox', DATE_FORMAT(MH.send_on, %s, %s)) AS Laatste, MH.to_addr AS Aan FROM %s %s ORDER BY MH.Ingevoerd DESC LIMIT %d;", $this->fdlang, $this->db_language, $this->basefrom, $p_filter, $p_aantal);
		$result = $this->execsql($query);
		if ($p_aantal > 1) {
			$rows = $result->fetchAll();
			if (count($rows) == 0) {
				return "Nee";
			} else {
				$rv = "";
				foreach ($rows as $row) {
					$rv .= $row->Laatste . ": " . $row->Aan . "<br>\n";
				}
				return $rv;
			}
		} else {
			$row = $result->fetch();
			if (isset($row->Laatste)) {
				return $row->Laatste;
			} else {
				return "Nee";
			}
		}
	}
	
	public function lijst($p_mid=-1) {
		if ($p_mid >= 0) {
			$this->mid = $p_mid;
		}
		
		if ($this->mid > 0) {
			$query = sprintf("SELECT MH.RecordID, IF(MH.send_on > '2000-01-01', MH.send_on, 'In outbox') AS Verzonden, %1\$s AS Aan, M.subject
						FROM (%2\$s LEFT OUTER JOIN %3\$sLid AS L ON MH.LidID=L.RecordID) LEFT OUTER JOIN %3\$sMailing AS M ON MH.MailingID=M.RecordID
						WHERE MH.MailingID=%4\$d %5\$s
						ORDER BY MH.RecordID DESC;", $this->selectmhaan, $this->basefrom, TABLE_PREFIX, $this->mid, $this->zichtbaarwhere);
		} else {
			$query = sprintf("SELECT MH.RecordID, MH.send_on, IF(LENGTH(MH.from_name)<3, MH.from_addr, MH.from_name) AS Van, %s AS Aan, MH.subject
						FROM %s LEFT OUTER JOIN %sLid AS L ON MH.LidID=L.RecordID
						WHERE MH.send_on > '2000-01-01' %s
						ORDER BY MH.RecordID DESC LIMIT %d;", $this->selectmhaan, $this->basefrom, TABLE_PREFIX, $this->zichtbaarwhere, $this->limit);
		}
		
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function outbox($p_srt=1, $p_mhid=-1) {
		/* uitleg p_srt
			1 = overzicht outbox
			2 = versturen in batchjob
			3 = direct versturen
			4 = speciale mailingen die direct verstuurd moeten worden
		*/
			
		if ($p_srt > 1) {
			$lm = $_SESSION['settings']['maxmailsperminuut'];
		} else {
			$lm = 1500;
		}
		
		if ($p_mhid > 0) {
			$xw = sprintf("AND MH.RecordID=%d", $p_mhid);
		} elseif ($p_srt == 2 or $p_srt == 3 or $p_srt == 4) {
			$xw = "AND MH.NietVersturenVoor < SYSDATE()";
		} else {
			$xw = $this->zichtbaarwhere;
		}
		$query = sprintf("SELECT MH.RecordID, MH.Ingevoerd, MH.subject, MH.to_name AS Aan, MH.from_name,
						MH.to_addr, MH.cc_addr, MH.NietVersturenVoor
						FROM (%1\$sMailing_hist AS MH LEFT OUTER JOIN %1\$sLid AS L ON MH.LidID=L.RecordID) LEFT JOIN %1\$sMailing AS M ON M.RecordID=MH.MailingID
						WHERE IFNULL(MH.send_on, '1970-01-01') <= '2000-01-01' %2\$s
						ORDER BY MH.NietVersturenVoor, MH.LidID, MH.RecordID LIMIT %3\$d;", TABLE_PREFIX, $xw, $lm);
		return $this->execsql($query);
	}
	
	public function aantaloutbox() {
		$query = sprintf("SELECT COUNT(*) FROM %s AS MH WHERE IFNULL(MH.send_on, '1970-01-01') <= '2000-01-01';", $this->table);
		return $this->scalar($query);
	}
		
	public function aantalverzonden($p_termijn) {
		/*
		Uitleg termijn:
			1: afgelopen minuut
			2: afgelopen uur
			3: afgelopen 24 uur
		*/
		
		if ($p_termijn == 2) {
			$tl = date("Y-m-d H:i:s", mktime(date("H")-1, date("i"), date("s"), date("m"), date("d"), date("Y")));
		} elseif ($p_termijn == 3) {
			$tl = date("Y-m-d H:i:s", mktime(date("H")-24, date("i"), date("s"), date("m"), date("d"), date("Y")));
		} else {
			$tl = date("Y-m-d H:i:s", mktime(date("H"), date("i")-1, date("s"), date("m"), date("d"), date("Y")));
		}
		
		$query = sprintf("SELECT COUNT(*) FROM %s AS MH WHERE IFNULL(MH.send_on, '1970-01-01') > '%s';", $this->table, $tl);
		return $this->scalar($query);
	}
	
	public function overzichtlid($p_lidid) {
		
		$query = sprintf("SELECT MH.RecordID AS lnkRecordID,
						  MH.send_on AS dteVerzonden,
						  IF(LENGTH(MH.from_name)<3, MH.from_addr, MH.from_name) AS Van,
						  MH.subject AS Onderwerp
						  FROM %s AS MH LEFT JOIN %sLid AS L ON MH.LidID=L.RecordID
						  WHERE MH.send_on > '2000-01-01' AND MH.LidID=%d
						  ORDER BY MH.RecordID DESC LIMIT 1000;", $this->table, TABLE_PREFIX, $p_lidid);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function lidbijemail($p_mhid) {
		$query = sprintf("SELECT IFNULL(MAX(LidID), 0) FROM %s AS MH WHERE MH.RecordID=%d;", $this->table, $p_mhid);
		return $this->scalar($query);
	}
	
	public function add($p_email) {
		$this->lidid = $p_email->lidid;
		$this->tas = 21;
		$nrid = $this->nieuwrecordid();
		
		if (strlen($p_email->aannaam) > $this->lengtekolom("to_name")) {
			$this->p_email = substr($p_email->aannaam, 0, $this->lengtekolom("to_name")-4) . " ...";
		}
		
		$p_email->aanadres = str_replace(";", ",", $p_email->aanadres);
		$p_email->aanadres = str_replace(" ", "", $p_email->aanadres);
		if (strlen($p_email->aanadres) > $this->lengtekolom("to_addr")) {
			$p_email->aanadres = substr($p_email->aanadres, 0, $this->lengtekolom("to_addr")-4) . " ...";
		}
		
		$p_email->cc = str_replace(";", ",", $p_email->cc);
		$p_email->cc = str_replace(" ", "", $p_email->cc);
		if (strlen($p_email->cc) > $this->lengtekolom("cc_addr")) {
			$p_email->cc = substr($p_email->cc, 0, $this->lengtekolom("cc_addr")-4) . " ...";
		}
		
		if (strlen($p_email->aannaam) == 0 and $p_email->mailingid > 0) {
			$p_email->aannaam = (new cls_mailing())->max("to_name", sprintf("RecordID=%d", $p_email->mailingid));
		}
		
		if (strlen($p_email->xtrachar) > $this->lengtekolom("Xtra_Char")) {
			$p_email->xtrachar = substr($p_email->xtrachar, 0, $this->lengtekolom("Xtra_Char"));
		}

		$p_email->vanafnaam = html_entity_decode(str_replace("\"", "'", $p_email->vanafnaam));
		$p_email->onderwerp = html_entity_decode(str_replace("\"", "'", $p_email->onderwerp));
		$p_email->aannaam = html_entity_decode(str_replace("\"", "'", $p_email->aannaam));
		$p_email->bericht = html_entity_decode(str_replace("\"", "'", $p_email->bericht));
	
		$query = sprintf("INSERT INTO %s SET RecordID=%d, LidID=%d, MailingID=%d, from_name=\"%s\", from_addr=\"%s\", subject=\"%s\", to_name=\"%s\", to_addr=\"%s\", cc_addr=\"%s\", message=\"%s\", ZonderBriefpapier=%d, ZichtbaarVoor=%d, Xtra_Char='%s', Xtra_Num=%d, NietVersturenVoor='%s', IngevoerdDoor=%d;",
						$this->table, $nrid, $p_email->lidid, $p_email->mailingid, $p_email->vanafnaam, $p_email->vanafadres, $p_email->onderwerp, $p_email->aannaam, $p_email->aanadres, $p_email->cc, $p_email->bericht, $p_email->zonderbriefpapier, $p_email->zichtbaarvoor, $p_email->xtrachar, $p_email->xtranum, $p_email->nietversturenvoor, $_SESSION['lidid']);
		if ($this->execsql($query) > 0) {
			return $nrid;
		} else {
			return 0;
		}
	}
	
	public function update($p_mhid, $p_kolom, $p_waarde) {
		$this->vulvars($p_mhid, 22);
		
		
		if ($this->pdoupdate($p_mhid, $p_kolom, $p_waarde) > 0) {
			if ($p_kolom != "send_on") {
				$this->log($p_mhid);
			}
		}
	}
	
	public function delete($p_mhid, $p_reden="", $p_log=1) {
		$this->vulvars($p_mhid, 23);
		$this->pdodelete($this->mhid, $p_reden);
		if ($p_log == 1) {
			$this->log($this->mhid);
		}
	}
	
	public function opschonen() {
		$this->tas = 23;
		$mho = $_SESSION['settings']['mailing_hist_opschonen'] ?? 18;
		if ($mho >= 3) {
			$query = sprintf("SELECT LM.Lid FROM %sLidmaatschap AS LM WHERE IFNULL(LM.Opgezegd, '9999-12-31') < DATE_SUB(CURDATE(), INTERVAL %d MONTH);", TABLE_PREFIX, $mho);
			$result = $this->execsql($query);
			foreach ($result->fetchAll() as $lmrow) {
				$this->lidid = $lmrow->Lid;
				if ((new cls_Lidmaatschap())->soortlid($lmrow->Lid) == "Voormalig lid") {
					$this->query = sprintf("DELETE FROM %s WHERE LidID=%d AND send_on < DATE_SUB(CURDATE(), INTERVAL %d MONTH);", $this->table, $lmrow->Lid, $mho);
					$aant = $this->execsql();
					if ($aant > 0) {
						$this->mess = sprintf("%s: %d records van voormalig lid %d verwijderd.", $this->table, $aant, $this->lidid);
						$this->Log(0, 1);
					}
				}
			}
		}
			
		$query = sprintf("SELECT MH.RecordID FROM %s WHERE MH.LidID > 0 AND (MH.LidID NOT IN (SELECT L.RecordID FROM %sLid AS L WHERE (L.Verwijderd IS NULL))) ORDER BY MH.LidID;", $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $mhrow) {
			$this->delete($mhrow->RecordID, "het lid niet (meer) bestaat.");
		}
		
		
		$mho = $_SESSION['settings']['mailing_verzonden_opschonen'] ?? 84;
		$aant = 0;
		$query = sprintf("SELECT MH.RecordID FROM %s WHERE MH.send_on < DATE_SUB(CURDATE(), INTERVAL %d MONTH) AND (MH.send_on IS NOT NULL);", $this->basefrom, $mho);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $mhrow) {
			$this->delete($mhrow->RecordID, "", 0);
			$aant++;
		}
		$this->lidid = 0;
		if ($aant > 0) {
			$this->mess = sprintf("%d verzonden e-mails verwijderd, omdat deze langer dan %d maanden geleden verzonden zijn.", $aant, $mho);
			$this->Log(0, 1);
		}
		
		$aant = 0;
		$query = sprintf("SELECT MH.RecordID FROM %s WHERE MH.Ingevoerd < DATE_SUB(CURDATE(), INTERVAL 15 DAY) AND (MH.send_on IS NULL);", $this->basefrom);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $mhrow) {
			$this->delete($mhrow->RecordID, "", 0);
			$aant++;
		}
		$this->lidid = 0;
		if ($aant > 0) {
			$this->mess = sprintf("%d e-mails uit de outbox verwijderd, omdat deze langer dan 2 weken niet verzonden zijn.", $aant, $mho);
			$this->Log(0, 1);
		}
	}
	
}  # cls_Mailing_hist

class cls_Mailing_rcpt extends cls_db_base {
	
	private $mid = 0;
	private $mrid = 0;
	private $email = "";
	
	function __construct($p_mid=-1) {
		$this->table = TABLE_PREFIX . "Mailing_rcpt";
		$this->basefrom = $this->table . " AS MR";
		if ($p_mid >= 0) {
			$this->mid = $p_mid;
		}
		$this->ta = 4;
		$this->tas = 10;
	}
	
	private function vulvars($p_mrid=-1) {
		if ($p_mrid >= 0) {
			$this->mrid = $p_mrid;
		}

		$this->lidid = 0;
		$this->email = "";
		if ($this->mrid > 0) {
			$query = sprintf("SELECT MR.LidID, MR.to_address, MR.MailingID FROM %s WHERE MR.RecordID=%d;", $this->basefrom, $this->mrid);
			$result = $this->execsql($query);
			$row = $result->fetch();
			if (isset($row->MailingID)) {
				$this->lidid = $row->LidID;
				$this->mid = $row->MailingID;
				$this->email = $row->to_address;
			} else {
				$this->mrid = 0;
			}
		}
	}
	
	public function lijst($p_mid=-1, $p_lidid=-1) {
		$this->mid = $p_mid;
		
		$w = sprintf("MR.MailingID=%d", $this->mid);
		if ($p_lidid > 0) {
			$w .= sprintf(" AND L.RecordID=%d", $p_lidid);
		}
		
		$query = sprintf("SELECT MR.LidID, MR.RecordID, %s AS Naam_lid, %s AS Zoeknaam_lid, L.Adres, L.Postcode, L.Woonplaats, L.Email, L.EmailVereniging, L.EmailOuders, L.GEBDATUM, MR.to_address, MR.MailingID
						  FROM %s LEFT OUTER JOIN %sLid AS L ON MR.LidID=L.RecordID
						  WHERE %s
						  ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, MR.to_address, MR.Ingevoerd;", $this->selectnaam, $this->selectzoeknaam, $this->basefrom, TABLE_PREFIX, $w);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function record($p_mid, $p_lidid) {
		$this->mid = $p_mid;
		$this->lidid = $p_lidid;
		$query = sprintf("SELECT MR.* FROM %s WHERE MR.MailingID=%d AND MR.LidID=%d;", $this->basefrom, $this->mid, $this->lidid);
		$result = $this->execsql($query);
		$row = $result->fetch();
		if (isset($row->RecordID)) {
			$this->mrid = $row->RecordID;
			return $row;
		} else {
			return false;
		}
	}
	
	public function aantalontvangers($p_mid=-1, $p_lidid=-1) {
		
		$query = sprintf("SELECT COUNT(*) FROM %s ", $this->basefrom);
		
		if ($p_mid > 0 and $p_lidid > 0) {
			$query .= sprintf(" WHERE MR.MailingID=%d AND MR.LidID=%d", $p_mid, $p_lidid);
		} elseif ($p_mid > 0) {
			$query .= sprintf(" WHERE MR.MailingID=%d", $p_mid);
		} elseif ($p_lidid > 0) {
			$query .= sprintf(" WHERE MR.LidID=%d", $p_lidid);
		}
		$query .= ";";
		
		return $this->scalar($query);
	}
	
	public function add($p_mid, $p_lidid, $p_email="", $p_geenlog=0, $p_xchar="", $p_xnum=0) {
		$this->lidid = $p_lidid;
		$this->mid = $p_mid;
		$this->tas = 11;
		$nrid = 0;
		
		if ($this->lidid == 0 and strlen($p_email) > 5 and isValidMailAddress($p_email)) {
			$mr = (new cls_Lid())->lidbijemail($p_email);
			if (count($mr) == 1) {
				$this->lidid = $mr[0]->LidID;
				$p_email = "";
			}
		}
		
		if ($this->lidid > 0 and $this->mid > 0) {
			$query = sprintf("SELECT COUNT(*) FROM %s AS MR WHERE MR.MailingID=%d AND MR.LidID=%d;", $this->table, $p_mid, $this->lidid);
		} elseif (strlen($p_email) > 5 and isValidMailAddress($p_email) and $this->mid > 0) {
			$query = sprintf("SELECT COUNT(*) FROM %s AS MR WHERE MR.MailingID=%d AND LOWER(MR.to_address) LIKE '%s';", $this->table, $p_mid, strtolower($p_email));
		} else {
			$query = "";
		}
		if (strlen($query) > 0) {
			$rl = (new cls_lid())->record($this->lidid);
			$r = $rl->NaamLid;
			if ($this->scalar($query) > 0) {
//				$this->mess = sprintf("%s is niet aan mailing %d toegevoegd, omdat hij/zij al ontvanger van deze mailing is.", $r, $p_mid);
			} else {
				$query = "";
				$nrid = $this->nieuwrecordid();
				if ($this->lidid > 0) {
					if (isValidMailAddress($rl->Email) or isValidMailAddress($rl->EmailOuders) or isValidMailAddress($rl->EmailVereniging)) {
						$query = sprintf("INSERT INTO %s (RecordID, MailingID, LidID, Xtra_Char, Xtra_Num) VALUES (%d, %d, %d, '%s', %d);", $this->table, $nrid, $p_mid, $this->lidid, $p_xchar, $p_xnum);
					} else {
						$this->mess = sprintf("%s is niet aan mailing %d toegevoegd, omdat er geen geldig emailadres bekend is.", $r, $p_mid);
						$this->tm = 1;
						$nrid = 0;
					}
					$rl = null;
				} elseif (isValidMailAddress($p_email)) {
					$query = sprintf("INSERT INTO %s (RecordID, MailingID, to_address) VALUES (%d, %d, '%s');", $this->table, $nrid, $p_mid, $p_email);
					$r = $p_email;
				} else {
					$this->mess = sprintf("%s is niet aan mailing %d toegevoegd, omdat dit geen correct emailadres is.", $p_email, $p_mid);
					$nrid = 0;
				}
				if (strlen($query) > 5 and $this->execsql($query) > 0 and $p_geenlog == 0) {
					$this->mess = sprintf("%s is aan mailing %d toegevoegd.", $r, $p_mid);
					$this->tm = 0;
				}
			}
		} else {
			$nrid = 0;
		}
		
		$this->log($nrid);
		return $nrid;
	}
	
	public function delete($p_mrid, $p_email="", $p_geenlog=0) {
		$this->vulvars($p_mrid);
		$this->tas = 13;

		$query = sprintf("DELETE FROM %s WHERE RecordID=%d;", $this->table, $this->mrid);
		if ($this->lidid > 0) {
			$r = (new cls_Lid())->Naam($this->lidid);
		} else {
			$r = $this->email;
		}
		$rv = $this->execsql($query);
		
		if ($p_geenlog == 0) {
			if ($rv == 0) {
				$this->mess = sprintf("%s is bij mailing %d niet verwijderd.", $r, $this->mid);
			} else {
				$this->mess = sprintf("%s is bij mailing %d verwijderd.", $r, $this->mid);
			}
			$this->log($this->mid);
		}
		
		return $rv;
	}
	
	public function delete_all($p_mid) {
		
		$query = sprintf("DELETE FROM %s WHERE MailingID=%d;", $this->table, $p_mid);
		$rv = $this->execsql($query);
		if ($rv > 0) {
			$this->mess = sprintf("Alle %d ontvangers zijn bij mailing %d verwijderd.", $rv, $p_mid);
			$this->log($p_mid);
		}
		
		return $rv;
	}  # delete_all
	
	public function opschonen() {
		
		$query = sprintf("DELETE FROM %s WHERE IFNULL(MailingID, 0)=0;", $this->table);
		$rv = $this->execsql($query);
		if ($rv > 0) {
			$this->mess = sprintf("Er zijn %d records uit %s verwijderd, omdat de MailingID 0 was.", $rv, $this->table);
			$this->log(0, 2);
		}
		
		$query = sprintf("DELETE FROM %s WHERE MailingID NOT IN (SELECT RecordID FROM %sMailing);", $this->table, TABLE_PREFIX);
		$rv = $this->execsql($query);
		if ($rv > 0) {
			$this->mess = sprintf("Er zijn %d records uit %s verwijderd, omdat de bijbehorende mailing niet (meer) bestaat.", $rv, $this->table);
			$this->log(0, 2);
		}
		
		if ($_SESSION['settings']['mailing_bewaartijd_ontvangers'] >= 3) {
			$query = sprintf("DELETE FROM %s WHERE Ingevoerd < DATE_SUB(CURDATE(), INTERVAL %d MONTH);", $this->table, $_SESSION['settings']['mailing_bewaartijd_ontvangers']);
			$rv = $this->execsql($query);
			if ($rv > 0) {
				$this->mess = sprintf("Er zijn %d records uit %s verwijderd, omdat ze langer dan %d maanden geleden zijn toegevoegd.", $rv, $this->table, $_SESSION['settings']['mailing_bewaartijd_ontvangers']);
				$this->log(0, 2);
			}
		}
		
	}
	
}  # cls_Mailing_rcpt

class cls_Mailing_vanaf extends cls_db_base {
	
	private $mvid = 0;
	public $vanaf_email = "";
	public $vanaf_naam = "";
	
	function __construct($p_mvid=0) {
		$this->table = TABLE_PREFIX . "Mailing_vanaf";
		$this->basefrom = $this->table . " AS MV";
		$this->ta = 4;
		$this->tas = 30;
		if ($p_mvid > 0) {
			$this->mvid = $p_mvid;
			$this->vulvars($this->mvid);
		}
	}
	
	private function vulvars($p_mvid) {
		$this->mvid = $p_mvid;
		$query = sprintf("SELECT MAX(IFNULL(MV.Vanaf_email, '')) AS VE, MAX(IFNULL(MV.Vanaf_naam, '')) AS VN FROM %s WHERE MV.RecordID=%d;", $this->basefrom, $this->mvid);
		$result = $this->execsql($query);
		$row = $result->fetch();
		$this->vanaf_email = $row->VE;
		$this->vanaf_naam = $row->VN;
	}
	
	public function lijst($p_fetched=1) {
		$query = sprintf("SELECT MV.RecordID, MV.Vanaf_email, MV.Vanaf_naam, MV.Ingevoerd, MV.Gewijzigd FROM %s ORDER BY MV.Vanaf_email;", $this->basefrom);
		$result = $this->execsql($query);
		if ($p_fetched == 1) {
			return $result->fetchAll();
		} else {
			return $result;
		}
	}
	
	public function zoekid($p_email) {
		if (strlen($p_email) > 0) {
			$query = sprintf("SELECT IFNULL(MV.RecordID, 0) FROM %s WHERE UPPER(Vanaf_email)=UPPER('%s');", $this->basefrom, $p_email);
			$this->mvid = $this->scalar($query);
		} else {
			$this->mvid = 0;
		}
		return $this->mvid;
	}
	
	public function vanafnaam($p_vanafadres="") {
		if (strlen($p_vanafadres) > 0) {
			$this->zoekid($p_vanafadres);
		}
		$query = sprintf("SELECT IFNULL(MV.Vanaf_naam, '') FROM %s WHERE MV.RecordID=%d;", $this->basefrom, $this->mvid);
		$this->vanaf_naam = $this->scalar($query);
		return $this->vanaf_naam;
	}
	
	public function htmloptions($p_cv="") {
		$rv = "";
		
		$query = sprintf("SELECT RecordID, Vanaf_email, Vanaf_naam FROM %s ORDER BY Vanaf_naam", $this->table);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$rv .= sprintf("<option value=%d %s>%s (%s)</option>\n", $row->RecordID, checked($row->RecordID, "option", $p_cv), $row->Vanaf_naam, $row->Vanaf_email);
		}
		return $rv;
	}
	
	public function curuser() {
		
		$query = sprintf("SELECT IFNULL(Vanaf_email, '') FROM %s WHERE LOWER(Vanaf_email)='%s';", $this->table, strtolower($_SESSION['emailingelogde']));
		$rv = $this->scalar($query);
		if (strlen($rv) == 0) {
			$query = sprintf("SELECT IFNULL(from_addr, '') FROM %sMailing AS M WHERE M.IngevoerdDoor=%d ORDER BY RecordID DESC LIMIT 1;", TABLE_PREFIX, $_SESSION['lidid']);
			$rv = $this->scalar($query);
		}
		return $rv;
	}
	
	public function add() {
		$this->tas = 31;
		$query = sprintf("INSERT INTO %s (Vanaf_email, Ingevoerd) VALUES ('** nieuw **', CURDATE());", $this->table);
		$result = $this->execsql($query);
		if ($result > 0) {
			$this->mess = sprintf("In tabel %s is record %d toegevoegd.", $this->table, $result);
			$this->log($result, 1);
		}
		return $result;
	}
	
	public function update($p_mvid, $p_kolom, $p_waarde) {
		$this->vulvars($p_mvid);
		$this->tas = 32;
		
		if ($this->pdoupdate($p_mvid, $p_kolom, $p_waarde) > 0) {
			$this->log($p_mvid);
		}
	}
	
	public function delete($p_mvid) {
		$this->vulvars($p_mvid);
		$this->tas = 33;
		$query = sprintf("SELECT COUNT(*) FROM %sMailing AS M WHERE MailingVanafID=%d;", TABLE_PREFIX, $this->mvid);
		if ($this->scalar($query) == 0) {
			$this->pdodelete($p_mvid);
		} else {
			$this->mess = sprintf("Record %d (%s) is nog in gebruik en mag niet verwijderd worden.", $this->mvid, $this->vanaf_email);
		}
		$this->log($p_mvid, 1);
	}
	
}  # cls_Mailing_vanaf

class cls_GBR extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "GBR";
		$this->basefrom = $this->table . " AS GBR";
	}
	
	public function htmloptions($p_cv) {
		$this->query = sprintf("SELECT DISTINCT GBR.Kode, CONCAT(GBR.Kode, ' - ', GBR.OMSCHRIJV) AS Oms FROM %s INNER JOIN %sMutatie AS MT ON MT.GBR=GBR.Kode ORDER BY GBR.Kode;", $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql();
		$ret = "";
		foreach ($result->fetchAll() as $row) {
			if ($p_cv == $row->Kode) {
				$s = " selected";
			} else {
				$s = "";
			}
			$ret .= sprintf('<option%s value="%s">%s</option>\n', $s, $row->Kode, $row->Oms);
		}
	}
	
}

class cls_Mutatie extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Mutatie";
		$this->basefrom = $this->table . " AS MT";
	}
	
	public function lijst($p_jaar, $p_gbr, $p_kpl) {
		$xs = "";
		$where = sprintf("M.BoekjaarID=%d", $p_jaar);
		if ($p_gbr != "*" and strlen($p_gbr) > 0) {
			$where .= sprintf(" AND M.GBR='%s'", $p_gbr);
		} else {
			$xs = ", CONCAT(GBR.Kode, ' - ', GBR.OMSCHRIJV) AS Grootboekrekening";
		}
		
		if ($p_kpl != "*" and strlen($p_kpl) > 0 and is_numeric($p_kpl)) {
			$where .= sprintf(" AND M.KostenplaatsID=%d", $p_kpl);
		} elseif ((new cls_Kostenplaats())->aantal() > 0) {
			$xs .= ", KPL.Kode AS Kostenplaats";
		}

		$query = sprintf("SELECT BJ.Kode AS Jaar%1\$s, Datum AS dteDatum, M.OMSCHRIJV AS Omschrijving, Debet-Credit AS curBedrag
						  FROM ((%2\$sMutatie AS M LEFT OUTER JOIN %2\$sKostenplaats AS KPL ON M.KostenplaatsID = KPL.RecordID) INNER JOIN %2\$sGBR AS GBR ON M.GBR = GBR.Kode) INNER JOIN %2\$sBoekjaar AS BJ ON M.BoekjaarID=BJ.RecordID
						  WHERE %3\$s
						  ORDER BY BJ.Kode, GBR.Kode, KPL.Kode, Datum;", $xs, TABLE_PREFIX, $where);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
}  # cls_Mutatie

class cls_Boekjaar extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Boekjaar";
		$this->basefrom = $this->table . " AS BJ";
	}
	
}  # cls_Boekjaar

class cls_Kostenplaats extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Kostenplaats";
		$this->basefrom = $this->table . " AS KP";
	}
	
}  # cls_Kostenplaats

class cls_Logboek extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Admin_activiteit";
		$this->basefrom = $this->table . " AS A";
	}
	
	private function script() {
		$script = $_SERVER['PHP_SELF'];
		if (strlen($_SERVER['QUERY_STRING']) > 0) {
			$script .= "?" . $_SERVER['QUERY_STRING'];
		}
		$ml = $this->lengtekolom("Script");
		if (strlen($script) > $ml) {
			$stript = substr($script, 0, $ml-4) . " ...";
		}
		
		return $script;
	}
	
	public function lijst($p_type, $p_smal=0, $p_lidid=0, $p_filter="", $p_sort="", $p_limiet=9999) {
		
		if ($p_type >= 0 and strlen($p_filter) > 5) {
			$w = sprintf("TypeActiviteit=%d AND %s", $p_type, $p_filter);
		} elseif ($p_lidid > 0 and strlen($p_filter) > 5) {
			$w = sprintf("LidID=%d AND %s", $p_lidid, $p_filter);
		} elseif ($p_type >= 0) {
			$w = sprintf("TypeActiviteit=%d", $p_type);
		} elseif (strlen($p_filter) > 5) {
			$w = $p_filter;
		} else {
			$w = "TypeActiviteit >= 0";
		}

		if ($p_type < 0 and $_SERVER["HTTP_HOST"] !== "phprbm.telling.nl") {
			$w .= " AND TypeActiviteit < 99";
		}
		
		$s = "A.DatumTijd, Omschrijving";
		if (in_array($p_type, array(-1, 1, 4, 5, 6, 7, 10, 12, 14, 16, 19, 98, 99))) {
			$s .= ", IF(A.ReferLidID > 0, A.ReferLidID, '') AS betreftLid";
		}
		$s .= ", CONCAT(TypeActiviteit, IF(TypeActiviteitSpecifiek > 0, CONCAT('-', TypeActiviteitSpecifiek), '')) AS `Type`";
		$s .= ", IF(A.LidID > 0, A.LidID, '') AS ingelogdLid";
		$s .= ", A.Getoond";
		
		if ($_SESSION['webmaster'] == 1 and $p_smal == 0) {
			$s .= ", CONCAT(Script, ' / ', RefFunction) AS scriptFunctie, `IP_adres`";
		}
		if ($p_lidid > 0) {
			$w .= sprintf(" AND (LidID=%1\$d OR ReferLidID=%1\$d) AND (TypeActiviteit NOT IN (2, 9, 98, 99))", $p_lidid);
		}
		if ($p_smal == 1) {
			$w .= " AND DatumTijd >= DATE_SUB(NOW(), INTERVAL 6 MONTH)";
		}
		
		if (strlen($p_sort) > 0) {
			$p_sort .= ", ";
		}
		
		if ($p_limiet > 1) {
			$lm = sprintf("LIMIT %d", $p_limiet);
		} else {
			$lm = "";
		}

		$query = sprintf("SELECT %s FROM %s AS A WHERE %s ORDER BY %sA.RecordID DESC %s;", $s, $this->table, $w, $p_sort, $lm);
		$result = $this->execsql($query);
		return $result->fetchAll();	
	}
	
	public function overzichtlid($p_lidid) {
		
		$query = sprintf("SELECT A.DatumTijd as `dtsDatum en tijd`, Omschrijving, CONCAT(TypeActiviteit, IF(TypeActiviteitSpecifiek > 0, CONCAT('-', TypeActiviteitSpecifiek), '')) AS `Type`, %1\$s AS ingelogdLid
								FROM %2\$sAdmin_activiteit AS A LEFT OUTER JOIN %2\$sLid AS L ON A.LidID=L.RecordID
								WHERE (TypeActiviteit NOT IN (2, 9, 98, 99)) AND ReferLidID=%3\$d
								ORDER BY A.RecordID DESC LIMIT 1500;", $this->selectnaam, TABLE_PREFIX, $p_lidid);
		$result = $this->execsql($query);
		return $result->fetchAll();	
	}
	
	public function lidlijst() {
		$this->query = sprintf("SELECT DISTINCT LidID, %1\$s AS Naam
							FROM %2\$sAdmin_activiteit AS A INNER JOIN %2\$sLid AS L ON A.LidID=L.RecordID
							ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", $this->selectzoeknaam, TABLE_PREFIX);
		$result = $this->execsql();
		return $result->fetchAll();
	}
			
	public function vorigelogin($p_opmaak=1) {
		global $dtfmt;
		
		if ($_SESSION['lidid'] > 0) {
			$this->lidid = $_SESSION['lidid'];
		}
		$query = sprintf("SELECT IFNULL(MAX(DatumTijd), 'Geen') FROM %s WHERE LidID=%d AND TypeActiviteit=1 AND DatumTijd < DATE_SUB(SYSDATE(), INTERVAL 15 MINUTE) AND Omschrijving LIKE '%%ingelogd%%';", $this->table, $this->lidid);
		
		if ($p_opmaak == 1) {
			$dtfmt->setPattern(DTLONG);
			return $dtfmt->format(strtotime($this->scalar($query)));
		} else {
			return $this->scalar($query);
		}
	}
	
	public function iplogincontrole() {
		
		if ($_SESSION['settings']['login_autounlock'] > 0) {
			$min = $_SESSION['settings']['login_autounlock'];
		} else {
			$min = 120;
		}
		$query = sprintf("SELECT COUNT(*) FROM %s WHERE TypeActiviteit=1 AND TypeActiviteitSpecifiek=4 AND IP_adres='%s' AND DatumTijd >= DATE_SUB(SYSDATE(), INTERVAL %d MINUTE);", $this->table, $_SERVER['HTTP_USER_AGENT'], $min);
		return $this->scalar($query);
	}
	
	public function add($p_oms, $p_ta=0, $p_lidid=-1, $p_tm=0, $p_referid=0, $p_tas=0, $p_reftable="", $p_refcolumn="", $p_autom=0) {
		$this->ta = $p_ta;
		$this->tas = $p_tas;
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		$this->tm = $p_tm;
		$p_reftable = str_replace(TABLE_PREFIX, "", $p_reftable);
		
		$nrid = $this->nieuwrecordid();
		
		/*
		$p_tm
			* 0: niet tonen
			* 1: aan iedereen tonen
			* 2: alleen tonen aan webmasters
			* 3: aan iedereen als alert tonen
		*/

		$bt = debug_backtrace(2, 4);
		$f = "";
		for ($t=count($bt)-1;$t >= 0;$t += -1) {
			if (isset($bt[$t]['function']) and strtolower($bt[$t]['function']) <> "log" and isset($bt[$t]['class']) and $bt[$t]['class'] <> "cls_Logboek") {
				if (strlen($f) > 0) {
					$f .= "=>";
				}
				if (strlen($bt[$t]['class']) > 0) {
					$f .= $bt[$t]['class'];
				}
				if (strlen($bt[$t]['function']) > 0) {
					$f .= $bt[$t]['type'] . $bt[$t]['function'];
				}
			}
		}
		$f = substr($f, 0, 75);
		
		$p_oms = str_replace("<p>", "", $p_oms);
		$p_oms = str_replace("</p>", "\n", $p_oms);
		$p_oms = str_replace("\"", "'", $p_oms);
		if (strlen($p_oms) > 64000) {
			$p_oms = substr($p_oms, 0, 64000);
		} elseif (strlen($p_oms) == 0) {
			$this->tm = 0;
		}
		
		$ua = $_SERVER['HTTP_USER_AGENT'];
		if (strlen($ua) > 125) {
			$ua = substr($ua, 0, 125);
		}
		
		if ($this->tm == 2 and $_SESSION['webmaster'] == 0) {
			$this->tm = 0;
		}
		
		if ($p_autom == 0) {
			$il = $_SESSION['lidid'];
		} else {
			$il = 0;
		}
		
		$query = sprintf("INSERT INTO %s (RecordID, DatumTijd, LidID, IP_adres, USER_AGENT, Omschrijving, ReferID, ReferLidID, TypeActiviteit, Script, Getoond, RefFunction, TypeActiviteitSpecifiek, RefTable, refColumn) VALUES 
				(%d, SYSDATE(), %d, '%s', '%s', \"%s\",%d, %d, %d, \"%s\", %d, '%s', %d, '%s', '%s');", $this->table, $nrid, $il, $_SERVER['REMOTE_ADDR'], $ua, $p_oms, $p_referid, $this->lidid, $this->ta, $this->script(), $this->tm, $f, $this->tas, $p_reftable, $p_refcolumn);
		$this->execsql($query);
		if ($this->tm == 1 or ($this->tm == 2 and $_SESSION['webmaster'] == 1)) {
			printf("<p class='mededeling'>%s</p>\n", $p_oms);
		} elseif ($this->tm == 3) {
			printf("<script>alert(\"%s\");</script>\n", $p_oms);
		}
		usleep(15);
		return $nrid;
	}
	
	public function opschonen() {
		
		if ($_SESSION['settings']['logboek_bewaartijd'] > 0) {
			$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL %d MONTH);", $this->table, $_SESSION['settings']['logboek_bewaartijd']);
			$this->execsql($query, 2);
		}

		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 7 DAY) AND LENGTH(Omschrijving)=0;", $this->table);
		$this->execsql($query, 2);

		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 7 DAY) AND TypeActiviteit IN (13, 98);", $this->table);
		$this->execsql($query, 2);

		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 7 DAY) AND TypeActiviteit=6 AND TypeActiviteitSpecifiek >= 30 AND TypeActiviteitSpecifiek <= 39;", $this->table);
		$this->execsql($query, 2);

// Backups
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 7 DAY) AND TypeActiviteit=3 AND TypeActiviteitSpecifiek > 1;", $this->table);
		$this->execsql($query, 2);
		
// Inloggen
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 7 MONTH) AND TypeActiviteit=1 AND TypeActiviteitSpecifiek=1;", $this->table);
		$this->execsql($query, 2);

// Uitloggen
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 7 DAY) AND TypeActiviteit=1 AND TypeActiviteitSpecifiek=2;", $this->table);
		$this->execsql($query, 2);
	}
	
	public function debugopschonen() {
		$query = sprintf("DELETE FROM %s WHERE TypeActiviteit=99 AND LidID=%d;", $this->table, $_SESSION['lidid']);
		$this->execsql($query, 2);
	}

}  # cls_Logboek

class cls_interface extends cls_db_base {
	
	public $maxrecords = 499;
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Admin_interface";
		$this->basefrom = $this->table . " AS I";
		$this->ta = 8;
	}

	public function lijst() {
		$query = sprintf("SELECT %s AS betreftLid, I.Ingevoerd, I.`SQL-statement` AS `SQL`, I.RecordID
						FROM %s LEFT OUTER JOIN %sLid AS L ON I.LidID=L.RecordID
						WHERE IFNULL(I.Afgemeld, '1970-01-01') < '2000-01-01'
						ORDER BY I.Ingevoerd, I.RecordID LIMIT %d;", $this->selectnaam, $this->basefrom, TABLE_PREFIX, $this->maxrecords);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function add($p_query, $p_lidid=0, $p_nip=0) {
		global $dbc;
		
		if ($_SESSION['settings']['interface_access_db'] == 1) {
			$msa_sql = str_ireplace("Now()", "#" . date("m/d/Y H:i:s") . "#", $p_query);
			if (strpos($p_query, '[MySQL]') === false) {
				$msa_sql = str_replace(TABLE_PREFIX, "", $msa_sql);
				$msa_sql = str_replace("SYSDATE()", "#" . date("m/d/Y H:i:s") . "#", $msa_sql);
				$msa_sql = str_replace("CURDATE()", "#" . date("m/d/Y") . "#", $msa_sql);
				$msa_sql = str_replace(" BINARY", "", $msa_sql);
				$msa_sql = str_replace("IFNULL(", "Nz(", $msa_sql);
			}
			if (substr($msa_sql, -1) != ";") {
				$msa_sql .= ";";
			}
		
			if ($p_nip == 1) {
				$ip = "";
			} else {
				$ip = $_SERVER['REMOTE_ADDR'];
			}
		
			$query = sprintf("INSERT INTO %s (`IP-adres`, LidID, `SQL-statement`, IngelogdLid) VALUES (?, ?, ?, ?)", $this->table);
			$stmt = $dbc->prepare($query);
			$stmt->bindParam(1, $ip);
			$stmt->bindParam(2, $p_lidid);
			$stmt->bindParam(3, $msa_sql);
			$stmt->bindParam(4, $_SESSION['lidid']);
			$stmt->execute();
			$stmt = null;
		}
	}
	
	public function delete($p_recid) {
		$query = sprintf("DELETE FROM %s WHERE RecordID=%d;", $this->table, $p_recid);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Record %d is uit tabel '%s' verwijderd.", $p_recid, $this->table);
			$this->log($p_recid);
		}
	}
	
	public function afmelden() {
		
		$query = sprintf("UPDATE %s SET Afgemeld=SYSDATE() WHERE IFNULL(Afgemeld, '1900-01-01') < '2011-01-01' ORDER BY Ingevoerd, RecordID LIMIT %d;", $this->table, $this->maxrecords);
		$result = $this->execsql($query);
		if ($result > 0) {
			$this->mess = sprintf("Er zijn %d wijzigingen uit de interface afgemeld.", $result);
			$this->tm = 1;
			$this->log();
		}
	}
	
	public function opschonen() {
		$query = sprintf("DELETE FROM %s WHERE Afgemeld < DATE_SUB(CURDATE(), INTERVAL 6 MONTH);", $this->table);
		$res = $this->execsql($query);
		if ($res > 0) {
			$this->mess = sprintf("%d records uit tabel '%s' verwijderd.", $res, $this->table);
			$this->Log();
		}
	}
	
}  # cls_interface

class cls_Diploma extends cls_db_base {
	
	private $dpnaam = "";
	private $dpcode = "";
	
	function __construct() {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Diploma";
		$this->basefrom = $this->table . " AS DP";
		$this->ta = 20;
	}
	
	private function vulvars($p_dpid=-1) {
		if ($p_dpid >= 0) {
			$this->dpid = $p_dpid;
		}
		if ($this->dpid > 0) {
			$row = $this->record($this->dpid);
			if ($row != false) {
				$this->dpnaam = $row->Naam;
				$this->dpcode = $row->Kode;
			} else {
				$this->dpid = 0;
			}
		}
	}
	
	public function lijst() {
		$query = sprintf("SELECT DP.* FROM %s ORDER BY DP.Kode;", $this->basefrom);
		$result = $this->execsql($query);
		
		debug("lijst in cls_diploma: vervangen door basislijst");
		
		return $result;
	}
	
	public function record($p_dpid) {
		$this->dpid = $p_dpid;
		
		$query = sprintf("SELECT DP.* FROM %s WHERE DP.RecordID=%d;", $this->basefrom, $p_dpid);
		$result = $this->execsql($query);
		$row = $result->fetch();
		if (isset($row->RecordID)) {
			return $row;
		} else {
			return false;
		}
	}
		
	public function lidmuteerlijst($p_lidid) {
		
		$query = sprintf("SELECT DP.*, LD.Lid, LD.DatumBehaald, LD.LicentieVervallenPer, LD.Diplomanummer, LD.RecordID AS LDID
						  FROM %s LEFT JOIN %sLiddipl AS LD ON LD.DiplomaID=DP.RecordID
						  WHERE ((LD.RecordID IS NULL) OR LD.Lid=%d);", $this->basefrom, TABLE_PREFIX, $p_lidid);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function htmloptions($p_cv=0, $p_afdfilter=-1, $p_zs=0, $p_inclvervallen=0, $p_filter="") {
		
		/*
			$p_afdfilter = door leden van deze afdeling gehaald.
		*/
		
		$rv = "";
		if ($p_afdfilter > 0) {
			$this->where = sprintf("WHERE DP.RecordID IN (SELECT DiplomaID FROM %1\$sLiddipl AS LD WHERE LD.Lid IN (SELECT Lid FROM %1\$sLidond AS LO WHERE LO.OnderdeelID=%2\$d AND IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE()))", TABLE_PREFIX, $p_afdfilter);
		}
		if ($p_zs == 1) {
			if (strlen($this->where) > 0) {
				$this->where .= " AND ";
			} else {
				$this->where = "WHERE ";
			}
			$this->where .= "DP.Zelfservice=1";
		}
		
		if ($p_inclvervallen == 0) {
			if (strlen($this->where) > 0) {
				$this->where .= " AND ";
			} else {
				$this->where = "WHERE ";
			}
			$this->where .= "IFNULL(DP.Vervallen, '9999-12-31') >= CURDATE()";
		}
		
		if (strlen($p_filter) > 0) {
			if (strlen($this->where) > 0) {
				$this->where .= " AND ";
			} else {
				$this->where = "WHERE ";
			}
			$this->where .= $p_filter;
		}
		
		$query = sprintf("SELECT DP.*, CONCAT(DP.Kode, ' - ', DP.Naam) AS ToonWaarde FROM %s %s ORDER BY IF(DP.EindeUitgifte IS NULL, 0, 1), DP.Volgnr, DP.Naam;", $this->basefrom, $this->where);
		foreach ($this->execsql($query) as $row) {
			$s = checked($row->RecordID, "option", $p_cv);
			$rv .= sprintf("<option value=%d %s>%s</option>\n", $row->RecordID, $s, $row->Naam);
		}
		return $rv;
		
	}  # htmloptions
	
	public function add() {
		$this->tas = 1;
		
		$nrid = $this->nieuwrecordid();
		
		$query = sprintf("INSERT INTO %s (RecordID, `Type`, Zelfservice, Ingevoerd) VALUES (%d, 'D', 0, SYSDATE());", $this->table, $nrid);
		if ($this->execsql($query) > 0) {
			$query = sprintf("INSERT INTO Diploma (RecordID, [Type], Zelfservice, Ingevoerd) VALUES (%d, 'D', 0, Now());", $nrid);
			(new cls_Interface())->add($query);
			$this->mess = sprintf("Diploma %d is toegevoegd.", $nrid);
			$this->Log($nrid);
		} else {
			$nrid = 0;
		}
		
		return $nrid;
		
	}  # add
	
	public function update($p_dpid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_dpid);
		$this->tm = 0;
		$this->tas = 2;
		
		$f = sprintf("DP.Kode='%s' AND DP.RecordID<>%d", $p_waarde, $p_dpid);
		if (strlen($p_kolom) == 0) {
			$this->mess = "Tabel Diploma: de kolom is leeg. Deze wijzging wordt niet doorgevoerd.";
		} elseif ($p_kolom == "Kode" and $this->aantal($f) > 0) {
			$this->mess = sprintf("Tabel Diploma: in record %d (%s) mag de waarde van Kode mag geen '%s' worden, want die is elders in gebruik.", $p_dpid, $this->dpnaam, $p_waarde);
		} else {
			if ($this->pdoupdate($p_dpid, $p_kolom, $p_waarde) and strlen($p_reden) > 0) {
				$this->mess .= ", omdat " . $p_reden;
			}
		}
		$this->log($p_dpid);
	}
	
	private function delete($p_dpid, $p_reden="") {
		$this->vulvars($p_dpid);
		$this->tas = 3;
		
		if ($this->pdodelete($this->dpid, $p_reden)) {
			$this->mess = sprintf("Tabel Diploma: record %d (%s) is verwijderd", $this->dpid, $this->dpnaam);
			if (strlen($p_reden) > 0) {
				$this->mess .= ", omdat " . $p_reden;
			}
			$this->log($p_dpid);
		}
	}
	
	public function controle() {
		
		$query = sprintf("SELECT DP.* FROM %s;", $this->basefrom);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			if (strlen($row->Vervallen) >= 10 and $row->EindeUitgifte > $row->Vervallen) {
				$this->update($row->RecordID, "EindeUitgifte", $row->Vervallen, "einde uitgifte niet na vervallen mag liggen.");
			} elseif (strlen($row->Vervallen) >= 10 and $row->Vervallen < date("Y-m-d") and $row->Zelfservice == 1) {
				$this->update($row->RecordID, "Zelfservice", 0, "het diploma is vervallen.");
			} elseif (array_key_exists($row->Type, ARRTYPEDIPLOMA) == false) {
				$this->update($row->RecordID, "Type", "D", "het diploma geen geldig type had.");
			}
		}
	}
	
	public function opschonen() {
		
		$query = sprintf("SELECT DP.RecordID FROM %s WHERE IFNULL(DP.Vervallen, '9999-12-31') < CURDATE() AND (SELECT COUNT(*) FROM %sLiddipl AS LD WHERE LD.DiplomaID=DP.RecordID)=0;", $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		$reden = "het diploma vervallen is en niemand dit diploma (meer) heeft.";
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, $reden);
		}
		
		$query = sprintf("SELECT DP.RecordID FROM %s WHERE IFNULL(DP.EindeUitgifte, '9999-12-31') < DATE_SUB(CURDATE(), INTERVAL 6 MONTH) AND (SELECT COUNT(*) FROM %sLiddipl AS LD WHERE LD.DiplomaID=DP.RecordID)=0;", $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		$reden = "het diploma niet meer wordt uitgegeven en niemand dit diploma (meer) heeft.";
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, $reden);
		}
	}  # opschonen
	
}  # cls_Diploma

class cls_Liddipl extends cls_db_base {
	
	private $ldid = 0;
	private $dpid = 0;
	private $datbehaald = "";
	private $dpnaam = "";
	
	function __construct() {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Liddipl";
		$this->basefrom = $this->table . " AS LD";
		$this->ta = 12;
	}
	
	private function vulvars($p_ldid=-1) {
		$this->ldid = $p_ldid;
		if ($this->ldid > 0) {
			$query = sprintf("SELECT LD.Lid, LD.DiplomaID, LD.DatumBehaald, DP.Naam FROM %s INNER JOIN %sDiploma AS DP ON DP.RecordID=LD.DiplomaID WHERE LD.RecordID=%d;", $this->basefrom, TABLE_PREFIX, $this->ldid);
			$row = $this->execsql($query)->fetch();
			$this->lidid = $row->Lid;
			$this->dpid = $row->DiplomaID;
			$this->datbehaald = $row->DatumBehaald;
			$this->dpnaam = $row->Naam;
		} elseif ($this->dpid > 0) {
			$query = sprintf("SELECT DP.Naam FROM %sDiploma AS DP WHERE DP.RecordID=%d;", TABLE_PREFIX, $this->dpid);
			$row = $this->execsql($query)->fetch();
			$this->dpnaam = $row->Naam;
		}
	}
	
	public function overzichtlid($p_lidid) {
		$query = sprintf("SELECT D.Naam,
						  O.Naam AS UitgegevenDoor,
						  LD.DatumBehaald,
						  LD.Diplomanummer,
						  IFNULL(LD.LicentieVervallenPer, IF(D.GELDIGH>0, DATE_ADD(LD.DatumBehaald, INTERVAL D.GELDIGH MONTH), IF((NOT D.Vervallen IS NULL), D.Vervallen, null))) AS GeldigTot
						  FROM %1\$sLiddipl AS LD INNER JOIN (%1\$sDiploma AS D INNER JOIN %1\$sOrganisatie AS O ON D.ORGANIS = O.Nummer) ON LD.DiplomaID = D.RecordID
						  WHERE LD.DatumBehaald <= CURDATE() AND LD.LaatsteBeoordeling=1 AND IFNULL(D.Vervallen, CURDATE()) >= CURDATE() AND LD.Lid=%2\$d
						  ORDER BY LD.DatumBehaald;", TABLE_PREFIX, $p_lidid);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function diplomasperlid($p_lidid) {
		$query = sprintf("SELECT LD.*, DP.Naam, DP.Kode, Org.Naam AS OrgNaam, DP.Zelfservice
						  FROM %1\$s INNER JOIN (%2\$sDiploma AS DP INNER JOIN %2\$sOrganisatie AS Org ON Org.Nummer=DP.ORGANIS) ON DP.RecordID=LD.DiplomaID WHERE LD.Lid=%3\$d;", $this->basefrom, TABLE_PREFIX, $p_lidid);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function overzichtperexamen($p_exdatum, $p_dpid) {
		
		$xw = "";
		if (strlen($p_exdatum) == 10) {
			$xw = sprintf("AND LD.DatumBehaald='%s'", $p_exdatum);
		}
		
		$query = sprintf("SELECT LD.*, %s AS NaamLid FROM %s INNER JOIN %sLid AS L ON LD.Lid=L.RecordID
							   WHERE LD.DiplomaID=%d %s ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, LD.DatumBehaald;", $this->selectnaam, $this->basefrom, TABLE_PREFIX, $p_dpid, $xw);
		$result = $this->execsql($query);
		
		return $result->fetchAll();
		
	}
	
	public function lidlaatstediplomas($p_lidid, $p_aant=2) {
		$rv = "";
		$a = 0;
		$query = sprintf("SELECT DP.Naam FROM %s AS LD INNER JOIN %sDiploma AS DP ON LD.DiplomaID=DP.RecordID
					WHERE LD.Lid=%d AND IFNULL(DP.Vervallen, CURDATE()) >= CURDATE() AND IFNULL(LD.LicentieVervallenPer, CURDATE()) >= CURDATE() AND DP.OpleidingInVereniging=1
					ORDER BY LD.DatumBehaald DESC, DP.Volgnr;", $this->table, TABLE_PREFIX, $p_lidid);
		$rows = $this->execsql($query)->fetchAll();
		
		foreach ($rows as $row) {
			if ($a < $p_aant) {
				if ($a > 0) {
					$rv .= ", ";
				}
				$rv .= $row->Naam;
				$a++;
			}
		}
		return $rv;
	}
	
	public function dubbelediplomas($p_lidid, $p_dpid, $p_orgldid) {
		
		$query = sprintf("SELECT LD.DatumBehaald FROM %s WHERE LD.Lid=%d AND LD.DiplomaID=%d AND LD.RecordID<>%d ORDER BY LD.DatumBehaald;", $this->basefrom, $p_lidid, $p_dpid, $p_orgldid);
		$result = $this->execsql($query);
		
		return $result->fetchAll();
		
	}
	
	public function vervallenbinnenkort($p_lidid=0) {
		$query = sprintf("SELECT D.Kode AS Code,
						  D.Naam AS Diploma,
						  CONCAT(D.Kode, ' ', D.Naam) AS DiplOms,
						  LD.DatumBehaald AS DatumBehaald,
						  LD.Diplomanummer,
						  IFNULL(LD.LicentieVervallenPer, IF(D.GELDIGH>0, DATE_ADD(LD.DatumBehaald, INTERVAL D.GELDIGH MONTH), NULL)) AS VervaltPer
						  FROM %1\$sLiddipl AS LD INNER JOIN %1\$sDiploma AS D ON LD.DiplomaID=D.RecordID
						  WHERE IFNULL(D.Vervallen, CURDATE()) >= CURDATE() AND ((NOT LD.LicentieVervallenPer IS NULL) OR D.GELDIGH > 0) AND LD.Lid=%2\$d
						  AND IFNULL(LD.LicentieVervallenPer, DATE_ADD(LD.DatumBehaald, INTERVAL D.GELDIGH MONTH)) < DATE_ADD(CURDATE(), INTERVAL %3\$d MONTH) AND LD.LicentieVervallenPer > DATE_SUB(CURDATE(), INTERVAL %3\$d MONTH)
						  ORDER BY D.Volgnr, D.Kode, LD.DatumBehaald DESC;", TABLE_PREFIX, $p_lidid, $_SESSION['settings']['termijnvervallendiplomasmelden']);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function add($p_lidid, $p_dpid, $p_exdatum="", $p_explaats="") {
		if (strlen($p_exdatum) < 8) {
			$p_exdatum = date("Y-m-d");
		}
		
		$this->lidid = $p_lidid;
		$this->dpid = $p_dpid;
		$this->vulvars();
		$this->tas = 1;
		$this->tm = 0;
		$nrid = $this->nieuwrecordid();
		$query = sprintf("INSERT INTO %s (RecordID, Lid, DiplomaID, DatumBehaald, EXPLAATS, LaatsteBeoordeling, Ingevoerd) VALUES (%d, %d, %d, '%s', '%s', 1, SYSDATE());", $this->table, $nrid, $this->lidid, $p_dpid, $p_exdatum, $p_explaats);
		if ($this->execsql($query) > 0) {
			$query = sprintf("INSERT INTO Liddipl (RecordID, Lid, DiplomaID, DatumBehaald, EXPLAATS, Ingevoerd) VALUES (%d, %d, %d, '%s', '%s', SYSDATE());", $nrid, $this->lidid, $p_dpid, $p_exdatum, $p_explaats);
			(new cls_Interface())->add($query, $this->lidid);
			$this->mess = sprintf("Diploma %d (%s) is toegevoegd.", $nrid, $this->dpnaam);
		} else {
			$nrid = 0;
		}
		$this->log($nrid);
		
		return $nrid;
	}
	
	public function update($p_ldid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_ldid);
		$gb = (new cls_Lid())->Geboortedatum($this->lidid);
		$this->tm = 1;
		$this->tas = 2;
		if ($p_kolom == "DatumBehaald" and $p_waarde < $gb) {
			$this->mess = "'Behaald op' mag niet voor de geboortedatum liggen, deze wijziging wordt niet verwerkt.";
		} elseif (($p_kolom == "DatumBehaald" or $p_kolom == "LicentieVervallenPer") and strtotime($p_waarde) == false and strlen($p_waarde) > 0) {
			$this->mess = sprintf("%s: %s is geen geldige datum, deze wijziging wordt niet verwerkt.", $p_kolom, $p_waarde);
		} elseif ($p_kolom == "DatumBehaald" and $p_waarde > date("Y-m-d")) {
			$this->mess = "'Behaald op' mag niet in de toekomst liggen, deze wijziging wordt niet verwerkt.";
		} elseif ($p_kolom == "LicentieVervallenPer" and $p_waarde < $this->datbehaald and strlen($p_waarde) > 0) {
			$this->mess = "'Vervallen per' moet na de datum van behalen liggen, deze wijziging wordt niet verwerkt.";
		} else {
			if ($this->pdoupdate($p_ldid, $p_kolom, $p_waarde) > 0) {
				$this->tm = 0;
				if (strlen($p_reden) > 0) {
					$this->mess .= ", omdat " . $p_reden;
				}
			}
		}
		$this->log($p_ldid);
	}
	
	public function delete($p_ldid, $p_reden="") {
		$this->vulvars($p_ldid);
		$this->tm = 0;
		$this->tas = 3;
		
		if ($this->pdodelete($this->ldid, $p_reden) > 0) {
			$this->mess = sprintf("Tabel Liddipl: record %d (%s) is verwijderd", $this->ldid, $this->dpnaam);
			if (strlen($p_reden) > 0) {
				$this->mess .= ", omdat " . $p_reden;
			}
			$this->log($this->ldid);
		}
	}
	
	public function controle() {
		$query = sprintf("SELECT LD.*, L.RecordID AS LidID, L.Overleden, DP.Vervallen, IF(DP.GELDIGH=0, '9999-12-31', DATE_ADD(LD.DatumBehaald, INTERVAL DP.GELDIGH MONTH)) AS GeldigTot
						  FROM (%1\$s INNER JOIN %2\$sDiploma AS DP ON LD.DiplomaID=DP.RecordID)INNER JOIN %2\$sLid AS L ON L.RecordID=LD.Lid;", $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			if (strlen($row->Overleden) >= 10 and $row->Overleden > $row->LicentieVervallenPer) {
				$this->update($row->RecordID, "LicentieVervallenPer", $row->Overleden, "het lid overleden is.");
			}  elseif (strlen($row->Vervallen) >= 10 and $row->Vervallen > $row->LicentieVervallenPer) {
				$this->update($row->RecordID, "LicentieVervallenPer", $row->Vervallen, "het diploma vervallen is.");
			}  elseif (strlen($row->LicentieVervallenPer) == 0 and $row->GeldigTot < date("Y-m-d")) {
				$this->update($row->RecordID, "LicentieVervallenPer", $row->GeldigTot, "de geldigheid is verlopen.");
			}
		}
	}
	
	public function opschonen() {
		$query = sprintf("SELECT LD.RecordID FROM %s WHERE LD.DiplomaID NOT IN (SELECT DP.RecordID FROM %sDiploma AS DP);", $this->basefrom, TABLE_PREFIX);
		$reden = " het diploma niet (meer) bestaat";
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, $reden);
		}		
		
		$query = sprintf("SELECT LD.RecordID FROM %s WHERE LD.Lid NOT IN (SELECT L.RecordID FROM %sLid AS L);", $this->basefrom, TABLE_PREFIX);
		$reden = " het lid niet (meer) bestaat";
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, $reden);
		}
		
		$query = sprintf("SELECT LD.RecordID FROM %s WHERE DatumBehaald > CURDATE();", $this->basefrom);
		$reden = " de datum van hehalen in de toekomst ligt";
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, $reden);
		}
		
		$query = sprintf("SELECT LD.RecordID FROM %s AS LD WHERE DatumBehaald > LicentieVervallenPer;", $this->table);
		$reden = " Vervallen per ligt voor datum behaald";
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, $reden);
		}
		
		$bt = $_SESSION['settings']['liddipl_bewaartermijn'] ?? 0;
		if ($bt > 3) {
			$reden = sprintf("de geldigheid langer dan %d maanden geleden is verlopen.", $bt);
			$query = sprintf("SELECT LD.RecordID FROM %s WHERE IFNULL(LD.LicentieVervallenPer, '9999-12-31') < DATE_SUB(CURDATE(), INTERVAL %d MONTH);", $this->basefrom, $bt);
			$result = $this->execsql($query);
			foreach ($result->fetchAll() as $row) {
				$this->delete($row->RecordID, $reden);
			}
		}
	}
	
}  #  cls_Liddipl

class cls_Examen extends cls_db_base {
	private $exid = 0;
	private $exdatum = "";
	private $explaats = "";
	
	function __construct($p_exid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Examen";
		$this->basefrom = $this->table . " AS EX";
		$this->ta = 12;
		$this->pkkol = "Nummer";
		if ($p_exid >= 0) {
			$this->vulvars($p_exid);
		}
	}
	
	private function vulvars($p_exid) {
		$this->exid = $p_exid;
		if ($this->exid > 0) {
			$query = sprintf("SELECT EX.* FROM %s WHERE EX.Nummer=%d;", $this->basefrom, $this->exid);
			$row = $this->execsql($query)->fetch();
			$this->exdatum = $row->Datum;
			$this->explaats = $row->Plaats;
		}
	}
	
	public function lijst() {
		$query = sprintf("SELECT EX.*, (SELECT COUNT(*) FROM %sLiddipl AS LD WHERE LD.Examen=EX.Nummer) AS AantalBehaald FROM %s ORDER BY EX.Datum DESC;", $this->basefrom, TABLE_PREFIX);
		$result = $thsis->execsql($query);
		return $result->fetchAll();
	}
	
	public function add() {
		$this->tas = 11;
		$nrid = $this->nieuwrecordid();
		$query = sprintf("INSERT INTO %s (Nummer, Datum, Ingevoerd) VALUES (%d CURDATE(), SYSDATE());", $this->table, $nrid);
		if ($this->execsql($query) > 0) {
			$query = sprintf("INSERT INTO Examen (Nummer, Datum, Ingevoerd) VALUES (%d, %s, Now());", $nrid, date("m/d/Y"));
			(new cls_Interface())->add($query);
			$this->mess = sprintf("Examen %d is toegevoegd.", $nrid);
			$this->Log($nrid);
		} else {
			$nrid = 0;
		}
		
		return $nrid;
	}

	public function update($p_exid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_exid);
		$this->tm = 0;
		$this->tas = 12;
		
		if ($this->pdoupdate($p_exid, $p_kolom, $p_waarde) and strlen($p_reden) > 0) {
			$this->mess .= ", omdat " . $p_reden;
		}
		$this->log($p_exid);
	}
	
	private function delete($p_exid, $p_reden="") {
		$this->vulvars($p_exid);
		$this->tas = 13;
		
		$contrqry = sprintf("SELECT COUNT(*) FROM %sLiddipl AS LD WHERE LD.Examen=%d;", TABLE_PREFIX, $p_exid);
		if ($this->scalar($contrqry) > 0) {
			$this->mess = sprintf("Examen %d wordt niet verwijderd, omdat er nog gerelateerde records in Liddipl zijn.", $p_exid);
			$this->log($p_exid);
		} else {
			if ($this->pdodelete($this->exid, $p_reden)) {
				$this->log($p_exid);
			}
		}
	}
	
	public function opschonen() {
		$query = sprintf("SELECT EX.Nummer FROM %s WHERE EX.Datum < CURDATE() AND EX.Ingevoerd < DATE_SUB(CURDATE(), INTERVAL 3 MONTH) AND (SELECT COUNT(*) FROM %sLiddipl AS LD WHERE LD.Examen=EX.Nummer)=0;", $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		$reden = "het examen in het verleden ligt en er een gekoppelde records (meer) zijn.";
		foreach ($result->fetchAll() as $exrow) {
			$this->delete($exrow->Nummer, $reden);
		}
	}
	
}  #cls_Examen

class cls_Organisatie extends cls_db_base {
	
	private $orgid = -1;
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Organisatie";
		$this->basefrom = $this->table . " AS Org";
		$this->pkkol = "Nummer";
		$this->ta = 20;
	}
	
	private function record($p_orgid) {
		$this->orgid = $p_orgid;
		$query = sprintf("SELECT Org.* FROM %s WHERE Org.Nummer=%d;", $this->basefrom, $this->orgid);
		$result = $this->execsql($query);
		$row = $result->fetch();
		if (isset($row->Nummer)) {
			return $row;
		} else {
			$this->orgid = -1;
			return false;
		}
	}
	
	public function naam($p_nr) {
		$query = sprintf("SELECT TRIM(Naam) FROM %s WHERE `%s`=%d;", $this->table, $this->pkkol, $p_nr);
		return $this->scalar($query);
	}
	
	public function lijst($p_filter=0, $p_fetched=1) {
		
		if ($p_filter == 1) {	
			// Organisaties gekoppeld aan een diploma
			$this->where = sprintf("WHERE Nummer IN (SELECT ORGANIS FROM %sDiploma)", TABLE_PREFIX);
		}
		
		$this->query = sprintf("SELECT Org.*, (SELECT IFNULL(COUNT(*), 0) FROM %1\$sDiploma AS DP WHERE DP.ORGANIS=Org.Nummer) + (SELECT IFNULL(COUNT(*), 0) FROM %1\$sOnderdl AS O WHERE O.ORGANIS=Org.Nummer) AS aantalLinked FROM %2\$s %3\$s ORDER BY 'Volledige naam';", TABLE_PREFIX, $this->basefrom, $this->where);
		$result = $this->execsql();
		
		if ($p_fetched == 1) {
			return $result->fetchAll();
		} else {
			return $result;
		}
	}
	
	public function htmloptions($p_filter=0, $p_cv=-1) {
		$rv = "";
		foreach ($this->lijst($p_filter) as $row) {
			$s = checked($row->Nummer, "option", $p_cv);
			$rv .= sprintf("<option value=%d %s>%s</option>\n", $row->Nummer, $s, $row->{'Volledige naam'});
		}
		return $rv;
	}
	
	public function add($p_orgid=-1) {
		$rv = -1;
		if ($p_orgid >= 0) {
			$nrid = $p_orgid;
		} else {
			$nrid = $this->nieuwrecordid();
		}
		
		$f = sprintf("Org.Nummer=%d", $nrid);
		if ($this->aantal($f) > 0) {
			$this->mess = sprintf("Organisatie met nummer %d mag niet worden toegevoegd, want dat nummer bestaat al.", $nrid);
			$this->log($nrid);
		} else {
		
			$query = sprintf("INSERT INTO %s (Nummer, Ingevoerd, Gewijzigd) VALUES (%d, CURDATE(), SYSDATE());", $this->table, $nrid);
			$rv = $this->execsql($query);
		
			if ($rv >= 0) {
				$this->mess = sprintf("Organisatie %d is toegevoegd.", $nrid);
				$this->log($nrid);
			
				$query = sprintf("INSERT INTO Organisatie (Nummer, Gewijzigd) VALUES (%d, Now());", $nrid);
				(new cls_Interface())->add($query);
			}
		}

		return $rv;
	}
	
	public function update($p_orgid, $p_kolom, $p_waarde) {
		$this->tm = 0;
		$this->tas = 2;
		
		if ($p_kolom == "Nummer") {
			$this->mess = "Het nummer mag niet worden gewijzigd. Deze wijziging wordt niet verwerkt.";
		} elseif ($p_kolom == "Naam" and strlen($p_waarde) == 0 and $p_orgid > 0) {
			$this->mess = "De afkoring mag niet leeg zijn. Deze wijziging wordt niet verwerkt.";
		} else {
			if ($this->pdoupdate($p_orgid, $p_kolom, $p_waarde) > 0) {
				$this->mess = sprintf("%s is bij organisatie %d in %s gewijzigd", $p_kolom, $p_orgid, $p_waarde);
			}
		}
		$this->log($p_orgid);
	}
	
	public function delete($p_orgid, $p_reden="") {
		$this->tas = 3;
		
		if ($this->pdodelete($p_orgid) > 0) {
			if (strlen($p_reden) > 0) {
				$this->mess .= ", omdat " . $p_reden;
			}
			$this->log($p_orgid);
		}
	}

	public function opschonen() {
		$query = sprintf("SELECT Nummer FROM %s WHERE Org.Gewijzigd < DATE_SUB(NOW(), INTERVAL 4 WEEK) AND Org.Nummer > 2", $this->basefrom);
		$query .= sprintf(" AND Org.Nummer NOT IN (SELECT ORGANIS FROM %sOnderdl)", TABLE_PREFIX);
		$query .= sprintf(" AND Org.Nummer NOT IN (SELECT ORGANIS FROM %sDiploma)", TABLE_PREFIX);
		$query .= ";";
		$res = $this->execsql($query);
		foreach ($res->fetchAll() as $row) {
			$this->delete($row->Nummer, "deze niet meer in gebruik is.");
		}
	}
	
	public function controle() {
		$a[0]['Naam'] = "";
		$a[0]['VolNaam'] = "";
		$a[1]['Naam'] = "KNBRD";
		$a[1]['VolNaam'] = "Reddinsbrigade Nederland";
		$a[2]['Naam'] = "NCS";
		$a[2]['VolNaam'] = "Nederlandse Culturele Sportbond";
		
		foreach ($a as $k => $v) {
			$row = $this->record($k);
			if ($row == false) {
				$this->add($k);
				$this->update($k, "Volledige naam", $v['VolNaam']);
			}
			$this->update($k, "Naam", $v['Naam']);			
		}
	}
	
}  # cls_Organisatie

class cls_Evenement extends cls_db_base {
	
	private $standaaarstatus;
	private $evid = 0;
	public $evoms = "";
	public $beperktotgroep = 0;
	private $sqlaantdln;
	private $sqlaantafgemeld;
	
	function  __construct($p_evid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Evenement";
		$this->basefrom = $this->table . " AS E";
		$this->ta = 7;
		$this->vulvars($p_evid);
		$this->sqlaantdln = sprintf("SELECT SUM(Aantal) FROM %sEvenement_Deelnemer AS ED WHERE ED.EvenementID=E.RecordID AND Status IN ('B', 'J', 'T')", TABLE_PREFIX);
		$this->sqlaantingeschreven = sprintf("SELECT COUNT(*) FROM %sEvenement_Deelnemer AS ED WHERE ED.EvenementID=E.RecordID AND Status='I'", TABLE_PREFIX);
		$this->sqlaantafgemeld = sprintf("SELECT COUNT(*) FROM %sEvenement_Deelnemer AS ED WHERE ED.EvenementID=E.RecordID AND Status='X'", TABLE_PREFIX);
	}
	
	private function vulvars($p_evid=-1) {
		global $dtfmt;
		
		if ($p_evid >= 0) {
			$this->evid = $p_evid;
		}
		
		if ($this->evid > 0) {
			$query = sprintf("SELECT E.* FROM %s WHERE E.RecordID=%d;", $this->basefrom, $p_evid);
			$evrow = $this->execsql($query)->fetch();
			$this->evoms = $evrow->Omschrijving;
			$this->evoms .= " " . $dtfmt->format(strtotime($evrow->Datum));
			$this->beperktotgroep = $evrow->BeperkTotGroep;
		}
	}
	
	public function lijst($p_soort=1, $p_datum="", $p_filter="") {
		/* p_soort:
			1=overzicht
			2=beheer
			3=inschrijving open
			4=persoonlijke agenda
			5=voor op de agenda
		*/

		$st = "DATE_FORMAT(E.Datum, '%H:%i', 'nl_NL')";
		$where = sprintf("E.Datum >= DATE_SUB(CURDATE(), INTERVAL 4 DAY) AND IFNULL(E.VerwijderdOp, '2000-01-01') < '2012-01-01' AND E.BeperkTotGroep IN (%1\$s)", $_SESSION["lidgroepen"]);

		$ord = "E.Datum DESC";
		if ($p_soort == 2) {
			$select = "E.RecordID, E.Datum, IF(RIGHT(E.Datum, 8)='00:00:00', '', DATE_FORMAT(E.Datum, '%H:%i', 'nl_NL')) AS Starttijd, E.Omschrijving, E.Locatie, (" . $this->sqlaantdln . ") AS Dln, E.Email, E.Eindtijd,
					   CASE E.InschrijvingOpen WHEN 0 THEN 'Nee' ELSE 'Ja' END AS insOpen, ET.Omschrijving AS typeOms, ET.Soort";
			$where = "IFNULL(E.VerwijderdOp, '1900-01-01') < '2012-01-01'";
			if (strlen($p_filter) > 0) {
				$where .= " AND " . $p_filter;
			}
			
		} elseif ($p_soort == 3) {
			$select = "E.*, ET.Omschrijving AS TypeEvenement";
			$where = sprintf("E.Datum > NOW() AND IFNULL(E.VerwijderdOp, '1900-01-01') < '2012-01-01' AND E.InschrijvingOpen=1 AND E.BeperkTotGroep IN (%s)", $_SESSION["lidgroepen"]);
			
		} elseif ($p_soort == 4) {
			$select = sprintf("E.Datum, E.Omschrijving, E.Locatie, E.MeerdereStartMomenten, E.RecordID, (%s) AS Dln, E.BeperkTotGroep", $this->sqlaantdln);
			$where = sprintf("E.Datum >= DATE_SUB(CURDATE(), INTERVAL 3 DAY) AND IFNULL(E.VerwijderdOp, '1900-01-01') < '2012-01-01' AND E.BeperkTotGroep IN (%s)", $_SESSION["lidgroepen"]);
			$ord = "E.Datum";
			
		} elseif ($p_soort == 5) {
			$select = sprintf("E.Datum, E.Omschrijving, E.Locatie, E.TypeEvenement, %s AS Starttijd, ET.Omschrijving AS OmsType, ET.Tekstkleur, ET.Achtergrondkleur, ET.Vet, ET.Cursief", $st);
			$where = sprintf("LEFT(E.Datum, 10)='%s' AND IFNULL(E.VerwijderdOp, '1900-01-01') < '2012-01-01'", $p_datum);
			$ord = "E.Datum";
			
		} else {
			$select = sprintf("E.RecordID, E.Datum, %s AS Starttijd, E.Omschrijving, E.Locatie, E.MeerdereStartMomenten,
								E.Email, E.Verzameltijd, E.Eindtijd, ET.Omschrijving AS OmsType, ET.Soort, ET.Tekstkleur, ET.Achtergrondkleur, ET.Vet, ET.Cursief,
								CASE E.InschrijvingOpen WHEN 0 THEN 'Nee' ELSE 'Ja' END AS `Ins. open?`,
								(%s) AS AantalDln, (%s) AS AantAfgemeld", $st, $this->sqlaantdln, $this->sqlaantafgemeld);
			$ord = "E.Datum";
		}
		$query = sprintf("SELECT %s
							FROM %s LEFT OUTER JOIN %sEvenement_Type AS ET ON E.TypeEvenement=ET.RecordID
							WHERE %s
							ORDER BY %s;", $select, $this->basefrom, TABLE_PREFIX, $where, $ord);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function record($p_evid) {
		$this->vulvars($p_evid);
		$query = sprintf("SELECT E.*, ET.Omschrijving AS OmsType, %s AS GewijzigdDoorNaam, (%s) AS AantDln, (%s) AantInschr, (%s) AS AantAfgemeld, ET.Tekstkleur, ET.Achtergrondkleur, ET.Vet, ET.Cursief
								FROM (%s LEFT JOIN %6\$sEvenement_Type AS ET ON ET.RecordID=E.TypeEvenement) LEFT JOIN %6\$sLid AS L ON E.GewijzigdDoor=L.RecordID
								WHERE E.RecordID=%7\$d;", $this->selectnaam, $this->sqlaantdln, $this->sqlaantingeschreven, $this->sqlaantafgemeld, $this->basefrom, TABLE_PREFIX, $this->evid);
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
	public function htmloptions($p_cv=-1, $p_filter="") {
		global $dtfmt;
		
		$evrows = $this->lijst(1, "". $p_filter);
		$rv = "";
		foreach ($evrows as $evrow) {
			$oms = $evrow->Omschrijving;
			$oms .= " " . $dtfmt->format(strtotime($evrow->Datum));
			$rv .= sprintf("<option value=%d%s>%s</option>\n", $evrow->RecordID, checked($p_cv, "option", $evrow->RecordID), $oms);
		}
		return $rv;
	}
			
	public function potdeelnemers($p_evid, $p_per="") {
		$where = "";
		$query = sprintf("SELECT BeperkTotGroep FROM %s AS E WHERE E.RecordID=%d;", $this->table, $p_evid);
		$groepid = $this->scalar($query);
		if (strlen($p_per) < 6) {
			$p_per = date("Y-m-d");
		}
		
		if ($groepid > 0) {
			$where = sprintf("AND LO.OnderdeelID=%d", $groepid);
		}
		$query = sprintf("SELECT DISTINCT L.RecordID AS LidID, %1\$s AS Naam
							FROM %2\$sLid AS L LEFT OUTER JOIN %2\$sLidond AS LO ON L.RecordID=LO.Lid
							WHERE (SELECT COUNT(*) FROM %2\$sEvenement_Deelnemer AS ED WHERE L.RecordID=ED.LidID AND ED.EvenementID=%3\$d)=0
							AND IFNULL(LO.Opgezegd, '9999-12-31') > '%5\$s' %4\$s
							ORDER BY %1\$s;", $this->selectzoeknaam, TABLE_PREFIX, $p_evid, $where, $p_per);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function standaardstatus($p_evid) {
		$this->evid = $p_evid;
		$query = sprintf("SELECT StandaardStatus FROM %s AS E WHERE E.RecordID=%d;", $this->table, $this->evid);
		$this->standaardstatus = $this->scalar($query);
		return $this->standaardstatus;
	}
	
	public function add($p_datum="") {
		if (strlen($p_datum) < 10) {
			$p_datum = date("Y-m-d");
		}
		$this->tas = 1;
		
		$nrid = $this->nieuwrecordid();
		$this->query = sprintf("INSERT INTO %s (RecordID, Datum, Omschrijving, TypeEvenement, InschrijvingOpen, StandaardStatus, IngevoerdDoor) VALUES (%d, '%s', '', 0, 0, 'B', %d);", $this->table, $nrid, $p_datum, $_SESSION['lidid']);
		if ($this->execsql() > 0) {
			$this->mess = sprintf("Evenement %d op %s is toegevoegd.", $nrid, $p_datum);
			$this->log($nrid);
			return $nrid;
		} else {
			return 0;
		}
	}
		
	public function update($p_evid, $p_kolom, $p_waarde) {
		$this->tas = 2;
		
		if ($this->pdoupdate($p_evid, $p_kolom, $p_waarde) > 0) {
			$this->log($p_evid);
		}
	}
	
	public function delete($p_evid) {
		$this->tas = 3;
		$this->vulvars($p_evid);
		
		if ($this->pdodelete($this->evid) > 0) {
			$this->mess = sprintf("Evenement %d (%s) is definitief verwijderd.", $this->evid, $this->evoms);
			$this->log($row->RecordID);		
		}
	}
	
	public function opschonen() {
		
		$this->query = sprintf("SELECT E.* FROM %s WHERE IFNULL(E.VerwijderdOp, '1900-01-01') > '2000-01-01';", $this->baasefrom);
		foreach($this->execsql()->fetchAll() as $row) {
			$contrqry = sprintf("SELECT COUNT(*) FROM %sEvenement_Deelnemer AS ED WHERE ED.EvenementID=%d;", TABLE_PREFIX, $row->RecordID);
			if ((new cls_db_base())->scalar($contrqry) ==  0) {
				$this->delete($row->RecordID);
			}
		}
	}
	
}  # cls_Evenement

class cls_Evenement_Deelnemer extends cls_db_base {
	
	private $edid = 0;
	private $evid = 0;
	private $omsEvenement = "";
	private $stdstatus = "";
	private $meerderestartmomenten = 0;
	
	function __construct($p_evid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Evenement_Deelnemer";
		$this->basefrom = $this->table . " AS ED";
		$this->ta = 7;
		$this->vulvars($p_evid);
	}
	
	private function vulvars($p_evid=-1, $p_lidid=-1) {
		if ($p_evid >= 0) {
			$this->evid = $p_evid;
		}
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		
		if ($this->edid > 0) {
			$this->query = sprintf("SELECT ED.LidID, ED.EvenementID FROM %s WHERE ED.RecordID=%d;", $this->basefrom, $this->edid);
			$row = $this->execsql()->fetch();
			$this->evid = $row->EvenementID;
			$this->lidid = $row->LidID;
		} elseif ($this->evid > 0 and $this->lidid > 0) {
			$query = sprintf("SELECT ED.RecordID FROM %s AS ED WHERE ED.LidID=%d AND ED.EvenementID=%d;", $this->table, $this->lidid, $this->evid);
			$this->edid = $this->scalar($query);
		}
		if ($this->evid > 0) {
			$query = sprintf("SELECT IFNULL(MAX(E.Omschrijving), '') AS OE, E.StandaardStatus, E.MeerdereStartMomenten FROM %sEvenement AS E WHERE E.RecordID=%d;", TABLE_PREFIX, $this->evid);
			$row = $this->execsql($query)->fetch();
			$this->OmsEvenement = $row->OE;
			$this->stdstatus = $row->StandaardStatus;
			$this->meerderestartmomenten = $row->MeerdereStartMomenten;
		}
	}
	
	public function record($p_edid, $p_lidid=-1, $p_evid=-1) {
		$this->edid = $p_edid;
		$this->vulvars($p_evid, $p_lidid);
		
		$query = sprintf("SELECT ED.*, E.Omschrijving AS OmsEvenement, E.Datum AS DatumEvenement, E.Email AS EmailEvenement FROM %s AS ED INNER JOIN %sEvenement AS E ON ED.EvenementID=E.RecordID WHERE ED.RecordID=%d;", $this->table, TABLE_PREFIX, $this->edid);
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
	public function overzichtevenement($p_evid, $p_stat="") {
		$this->vulvars($p_evid);
		if (strlen($p_stat) == 1) {
			$xw = sprintf("AND ED.Status='%s'", $p_stat);
		} elseif (strlen($p_stat) > 1) {
			$xw = sprintf("AND ED.Status IN (%s)", $p_stat);
		} else {
			$xw = "";
		}
		
		if ($this->meerderestartmomenten == 1) {
			$order = "ED.StartMoment, L.Achternaam, L.TUSSENV, L.Roepnaam";
		} else {
			$order = "L.Achternaam, L.TUSSENV, L.Roepnaam";
		}
		
		$query = sprintf("SELECT ED.RecordID, %1\$s AS NaamDeelnemer, %2\$s AS Telefoon, ED.StartMoment, IF(E.MeerdereStartMomenten=1, IFNULL(ED.StartMoment, ''), SUBSTRING(E.Datum, 12, 5)) AS Starttijd, E.MeerdereStartMomenten,
							ED.Status, ED.Opmerking, ED.Functie, ED.LidID, ED.Aantal, L.Geslacht, ED.Ingevoerd, ED.LidID, ED.IngevoerdDoor
							FROM (%3\$s INNER JOIN %4\$sLid AS L ON ED.LidID=L.RecordID) INNER JOIN %4\$sEvenement AS E ON E.RecordID=ED.EvenementID
							WHERE ED.EvenementID=%5\$d %6\$s
							ORDER BY %7\$s;", $this->selectnaam, $this->selecttelefoon, $this->basefrom, TABLE_PREFIX, $p_evid, $xw, $order);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}

	public function overzichtlid($p_lidid) {
		
		$query = "SELECT E.Datum, E.Omschrijving, E.Verzameltijd, DATE_FORMAT(E.Datum, '%H:%i') AS Starttijd, E.Locatie, E.Eindtijd, E.Email, ED.Opmerking, ED.Functie,
					IF(E.MeerdereStartMomenten=1, IFNULL(ED.StartMoment, ''), SUBSTRING(E.Datum, 12, 5)) AS Starttijd, E.MeerdereStartMomenten";
		$query .= ", CASE ED.Status";
		foreach (ARRDLNSTATUS as $s => $o) {
			$query .= sprintf(" WHEN '%s' THEN '%s'", $s, $o);
		}
		$query .= " END AS Status ";
		$query .= sprintf("FROM %1\$sEvenement AS E INNER JOIN %1\$sEvenement_Deelnemer AS ED ON E.RecordID = ED.EvenementID
								WHERE ED.LidID=%2\$d
								ORDER BY E.Datum DESC;", TABLE_PREFIX, $p_lidid);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function add($p_evid, $p_lidid, $p_status="") {
		$this->edid = 0;
		$this->vulvars($p_evid, $p_lidid);
		$nrid = $this->nieuwrecordid();
		$this->tas = 11;
		
		if (strlen($p_status) == 0){
			$p_status = $this->stdstatus;
		}
		
		$query = sprintf("SELECT COUNT(*) FROM %s AS ED WHERE ED.LidID=%d AND ED.EvenementID=%d;", $this->table, $this->lidid, $this->evid);
		if ($this->scalar($query) == 0) {
			$query = sprintf("INSERT INTO %sEvenement_Deelnemer (RecordID, LidID, EvenementID, Status, Opmerking, Functie, IngevoerdDoor) VALUES (%d, %d, %d, '%s', '', '', %d);", TABLE_PREFIX, $nrid, $this->lidid, $this->evid, $p_status, $_SESSION['lidid']);
			if ($this->execsql($query) > 0) {
				$this->edid = $nrid;
				$this->vulvars();
				$this->mess = sprintf("Is bij evenement %d (%s) met status '%s' toegevoegd.", $p_evid, $this->OmsEvenement, $p_status);
				$this->log($nrid);
				return $nrid;
			} else {
				return 0;
			}
		} else {
			$this->mess = sprintf("Lid %d is al deelnemer aan evenement %d (%s) en wordt niet toegevoegd.", $this->lidid, $this->evid, $this->omsEvenement);
			$this->log(0, 1);
			return 0;
		}
	}
	
	public function update($p_edid, $p_kolom, $p_waarde) {
		$this->edid = $p_edid;
		$this->vulvars();
		$this->tas = 12;
		$rv = 0;
		
		if ($this->pdoupdate($p_edid, $p_kolom, $p_waarde) > 0) {
			$this->log($p_edid);
			$rv = 1;
		}
		
		return $rv;
	}
	
	public function delete($p_edid) {
		$this->edid = $p_edid;
		$this->vulvars();
		$this->tas = 13;
		
		$query = sprintf("DELETE FROM %s WHERE RecordID=%d;", $this->table, $p_edid);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Is bij evenement %d (%s) verwijderd.", $this->evid, $this->OmsEvenement);
			$this->log($p_edid);
		}
	}
	
	public function opschonen() {
		
		$query = sprintf("DELETE FROM %s WHERE EvenementID NOT IN (SELECT RecordID FROM %sEvenement);", $this->table, TABLE_PREFIX);
		$this->execsql($query, 2);

		$query = sprintf("DELETE FROM %s WHERE LidID NOT IN (SELECT RecordID FROM %sLid);", $this->table, TABLE_PREFIX);
		$this->execsql($query, 2);
	}
	
}  #  cls_Evenement_Deelnemer

class cls_Evenement_Type extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Evenement_Type";
		$this->basefrom = $this->table . " AS ET";
		$this->ta = 7;
	}
	
	public function lijst() {
		$query = sprintf("SELECT ET.*, ET.Omschrijving AS OmsType FROM %s ORDER BY ET.Omschrijving;", $this->basefrom);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function htmloptions($cv) {
		$rv = "";
		foreach ($this->lijst() as $row) {
			$s = checked($cv, "option", $row->RecordID);
			$rv .= sprintf("<option value=%d %s>%s</option>\n", $row->RecordID, $s, $row->Omschrijving);
		}
		return $rv;
	}
	
	public function add($p_waarde) {
		$this->tas = 21;
		$nrid = $this->nieuwrecordid();
		
		$this->query = sprintf("INSERT INTO %s (RecordID, Omschrijving) VALUES (%d, '%s');", $this->table, $nrid, $p_waarde);
		if ($this->execsql() > 0) {
			$this->mess = sprintf("Evenement type %d (%s) is toegevoegd.", $nrid, $p_waarde);
			$this->log($nrid);
			return $nrid;
		} else {
			return 0;
		}
	}
	
	public function update($p_etid, $p_kolom, $p_waarde) {
		$this->tas = 22;
		if ($this->pdoupdate($p_etid, $p_kolom, $p_waarde) > 0) {
			$this->log($p_etid);
		}
	}
	
	public function delete($p_etid) {
		$this->tas = 23;
		
		$query = sprintf("SELECT COUNT(*) FROM %sEvenement AS E WHERE E.TypeEvenement=%d;", TABLE_PREFIX, $p_etid);
		if ((new cls_db_base())->scalar($query) > 0) {
			$this->mess = sprintf("Type %d is nog in gebruik en mag niet worden verwijderd.", $p_etid);
		} else {
			$this->pdodelete($p_etid);
		}
		$this->tm = 1;
		$this->log($p_etid);
	}
	
}  # cls_Evenement_Type

class cls_Artikel extends cls_db_base{
	
	function __construct() {
		$this->table = TABLE_PREFIX . "WS_Artikel";
		$this->basefrom = $this->table . " AS Art";
		$this->ta = 10;
	}
	
	public function lijst($p_type="bestellijst") {
		$query = sprintf("SELECT Art.*, CONCAT(Art.Omschrijving, IF(LENGTH(Art.Maat)>0, CONCAT(' (', Art.Maat, ')'), '')) AS OmsMaat, CONCAT(Art.Code, ' - ', Art.Omschrijving, ' ', IFNULL(Art.Maat, '')) AS CodeOmsMaat,
		IF((SELECT COUNT(*) FROM %1\$sWS_Orderregel AS Ord WHERE Ord.Artikel=Art.RecordID)+(SELECT COUNT(*) FROM %1\$sWS_Voorraadboeking AS VB WHERE VB.ArtikelID=Art.RecordID) > 0, 1, 0) AS InGebruik,
		(SELECT IFNULL(SUM(V.Aantal), 0) FROM %1\$sWS_Voorraadboeking AS V WHERE V.ArtikelID=Art.RecordID) AS Voorraad
		FROM %2\$s", TABLE_PREFIX, $this->basefrom);
		if ($p_type == "bestellijst") {
			$query .= sprintf(" WHERE IFNULL(Art.BeschikbaarTot, CURDATE()) >= CURDATE() AND IFNULL(Art.VervallenPer, CURDATE()) >= CURDATE() AND (Art.BeperkTotGroep IN (%s))", $_SESSION["lidgroepen"]);
		}
		$query .= " ORDER BY Art.Code, Art.Omschrijving, Art.RecordID;";
		
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function select($p_lidid) {
		$query = sprintf("SELECT A.*, CONCAT(Omschrijving, IF(LENGTH(Maat)>0, CONCAT(' (', Maat, ')'), '')) AS OmsMaat FROM %s
						WHERE (Art.RecordID NOT IN (SELECT Ord.Artikel FROM %sWS_Orderregel AS Ord WHERE Ord.Lid=%d))
						AND IFNULL(Art.VervallenPer, CURDATE()) >= CURDATE()
						ORDER BY Art.Code, Art.Omschrijving, Art.RecordID;", $this->basefrom, TABLE_PREFIX, $p_lidid);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function htmloptions($p_cv=-1, $p_filter="") {
		
		$query = sprintf("SELECT Art.RecordID, CONCAT(Art.Code, ' - ' , Art.Omschrijving, ' ', IFNULL(Art.Maat, '')) AS CodeOmsMaat FROM %s AS Art", $this->table);
		if ($p_filter == "bestelbaar") {
			$query .= sprintf(" WHERE IFNULL(Art.BeschikbaarTot, CURDATE()) >= CURDATE() AND IFNULL(Art.VervallenPer, CURDATE()) >= CURDATE() AND (Art.BeperkTotGroep IN (%s))", $_SESSION["lidgroepen"]);
			$query .= sprintf(" AND (IFNULL(Art.MaxAantalPerLid, 0)=0 OR Art.MaxAantalPerLid > (SELECT SUM(AantalBesteld) FROM %sWS_Orderregel WHERE Lid=%d))", TABLE_PREFIX, $_SESSION['lidid']);
		}
		$query .= " ORDER BY Art.Code, Art.Omschrijving;";
		
		$result = $this->execsql($query);
		$rv = "";
		foreach ($result->fetchAll() as $row) {
			$rv .= sprintf("<option value=%d %s>%s</option>\n", $row->RecordID, checked($p_cv, "option", $row->RecordID), $row->CodeOmsMaat);
		}
		return $rv;
	}
	
	public function add($p_code) {
		$nrid = $this->nieuwrecordid();
		
		$query = sprintf("INSERT INTO %s (RecordID, Code, IngevoerdDoor) VALUES  (%d, '%s', %d);", $this->table, $nrid, $p_code, $_SESSION['lidid']);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Artikel %d met code '%s' is toegevoegd.", $nrid, $p_code);
			$this->log($nrid);
		}
	}

	public function update($p_artid, $p_kolom, $p_waarde) {
		
		if ($this->pdoupdate($p_artid, $p_kolom, $p_waarde) > 0) {
			$this->tas = 12;
			$this->log($p_artid);
		}
	}
	
	public function delete($p_artikelid) {
		
		$f = sprintf("Artikel=%d", $p_artikelid);
		$f2 = sprintf("ArtikelID=%d", $p_artikelid);
		if ((new cls_Orderregel())->aantal($f) > 0 or (new cls_Voorraadboeking())->aantal($f2) > 0) {
			$this->mess = sprintf("Artikel %d is niet verwijderd, omdat het nog in gebruik is.", $p_artikelid);
		} else {
			$query = sprintf("DELETE FROM %s WHERE RecordID=%d;", $this->table, $p_artikelid);
			if ($this->execsql($query) > 0) {
				$this->mess = sprintf("Artikel %d is verwijderd.", $p_artikelid);
			}
		}
		$this->log($p_artikelid);
	}
	
	public function opschonen() {
		
		$query = sprintf("SELECT RecordID FROM %1\$s AS Art WHERE IFNULL(Art.BeschikbaarTot, CURDATE()) < CURDATE() AND Art.RecordID NOT IN (SELECT Ord.Artikel FROM %2\$sWS_Orderregel AS Ord) AND Art.RecordID NOT IN (SELECT VB.ArtikelID FROM %2\$sWS_Voorraadboeking AS VB);", $this->table, TABLE_PREFIX);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID);
		}
	}
	
}  # cls_Artikel

class cls_Orderregel extends cls_db_base {
	
	private $artid = 0;
	private $codeartikel = "";
	private $orid = 0;

	function __construct($p_lidid=0) {
		$this->table = TABLE_PREFIX . "WS_Orderregel";
		$this->basefrom = $this->table . " AS Ord";
		$this->lidid = $p_lidid;
		$this->ta = 10;
	}
	
	private function vulvars($p_orid=0) {
		$this->orid = $p_orid;
		if ($this->orid == 0 and $this->artid > 0 and $this->lidid > 0) {
			$query = sprintf("SELECT IFNULL(RecordID, 0) FROM %s WHERE Ord.Artikel=%d AND Ord.Lid=%d;", $this->basefrom, $this->artid, $this->lidid);
			$this->orid = $this->scalar($query);
			
		} elseif ($this->orid > 0 and ($this->artid == 0 or $this->lidid == 0)) {
			$query = sprintf("SELECT Ord.Artikel, Ord.Lid FROM %s WHERE Ord.RecordID=%d;", $this->basefrom, $this->orid);
			$row = $this->execsql($query)->fetch();
			$this->lidid = $row->Lid;
			$this->artid = $row->Artikel;
		}
		
		if ($this->artid > 0) {
			$query = sprintf("SELECT Art.Code FROM %sWS_Artikel AS Art WHERE Art.RecordID=%d;", TABLE_PREFIX, $this->artid);
			$this->codeartikel = $this->scalar($query);
		}
	}
	
	public function lijst($p_filter="", $p_eersterecord=0, $p_orderby="") {
		
		if (strlen($p_filter) > 0) {
			$p_filter = "WHERE " . $p_filter;
		}
		if (strlen($p_orderby) > 0 and substr(trim($p_orderby), -1) != ",") {
			$p_orderby = trim($p_orderby) . ",";
		}
		$query = sprintf("SELECT Ord.RecordID, Ord.Ordernr, Ord.Lid, Ord.Artikel, Ord.AantalBesteld, Ord.PrijsPerStuk, Ord.Opmerking,
					Ord.PrijsPerStuk * Ord.AantalBesteld AS Bedrag, Ord.Ingevoerd, Ord.BestellingDefinitief,
					%1\$s AS NaamLid, L.Roepnaam, L.Adres, L.Postcode, L.Woonplaats, L.Email, L.Email,
					Art.Code, Art.Omschrijving, Art.Maat, CONCAT(Art.Omschrijving, ' ', IFNULL(Art.Maat, '')) as ArtOms, CONCAT(Art.Code, ' - ', Art.Omschrijving, ' ', IFNULL(Art.Maat, '')) as CodeOmsMaat,
					(SELECT IFNULL(SUM(VB.Aantal), 0) * -1 FROM %3\$sWS_Voorraadboeking AS VB WHERE VB.OrderregelID=Ord.RecordID) AS AantalGeleverd,
					(SELECT IFNULL(SUM(Aantal), 0) * -1 FROM %3\$sWS_Voorraadboeking AS VB WHERE VB.ArtikelID=Art.RecordID) AS Voorraad, Art.MaxAantalPerLid
					FROM (%2\$s LEFT OUTER JOIN %3\$sWS_Artikel AS Art ON Art.RecordID=Ord.Artikel) INNER JOIN %3\$sLid AS L ON Ord.Lid=L.RecordID
					%4\$s
					ORDER BY %5\$s IF(Ord.Artikel > 0 AND Ord.AantalBesteld > 0, 1, 0), L.Achternaam, L.Roepnaam, Ord.Ordernr, Art.Code;", $this->selectnaam, $this->basefrom, TABLE_PREFIX, $p_filter, $p_orderby);
		$result = $this->execsql($query);
		if ($p_eersterecord == 1) {
			return $result->fetch();
		} else {
			return $result->fetchAll();
		}
	}
	
	public function winkelwagen($p_lidid){
		$f = sprintf("Ord.Lid=%d AND Ord.Ordernr=0", $p_lidid);
		return $this->lijst($f, 0, "Art.Code, Art.Omschrijving");
	}
	
	public function bestelling($p_lidid, $p_artikelid, $p_filter="") {
		
		if (strlen($p_filter) > 0) {
			$p_filter = "AND " . $p_filter;
		}
		
		$query = sprintf("SELECT * FROM %s WHERE Lid=%d AND Artikel=%d %s;", $this->table, $p_lidid, $p_artikelid, $p_filter);
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
	public function bevestiging($p_lidid, $p_filter) {
		if (strlen($p_filter) > 0) {
			$filter = "AND " . $p_filter;
		}
		$query = sprintf("SELECT Ord.Ordernr, Ord.AantalBesteld, Ord.Opmerking, Ord.PrijsPerStuk * Ord.AantalBesteld AS Bedrag,
					%1\$s AS NaamLid, L.Roepnaam, L.Adres, L.Postcode, L.Woonplaats, L.Email, L.GEBDATUM,
					A.Code, A.Omschrijving, A.Maat
					FROM (%2\$s INNER JOIN %3\$sWS_Artikel AS A ON A.RecordID=Ord.Artikel) INNER JOIN %3\$sLid AS L ON Ord.Lid=L.RecordID
					WHERE Ord.Lid=%4\$d AND (SELECT IFNULL(SUM(Aantal), 0) * -1 FROM %3\$sWS_Voorraadboeking AS VB WHERE VB.OrderregelID=Ord.RecordID) < Ord.AantalBesteld) AND (NOT Ord.BestellingDefinitief IS NULL) %4\$s
					ORDER BY A.Code;", $this->selectnaam, TABLE_PREFIX, $this->basefrom, $p_lidid, $p_filter);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function totalen() {
		$sqlag = sprintf("(SELECT IFNULL(SUM(Aantal), 0) * -1 FROM %sWS_Voorraadboeking AS VB WHERE VB.OrderregelID=Ord.RecordID)", TABLE_PREFIX);
		$query = sprintf("SELECT CONCAT(Art.Code, ' - ', Art.Omschrijving, ' ', IFNULL(Art.Maat, '')) AS `Artikel`,
					SUM(Ord.AantalBesteld) AS intBesteld, SUM(%2\$s) AS intGeleverd,
					(SELECT SUM(VB2.Aantal) FROM %1\$sWS_Voorraadboeking AS VB2 WHERE VB2.ArtikelID=Ord.Artikel) AS `intVoorraad`,
					SUM(Ord.AantalBesteld * Ord.PrijsPerStuk) AS `curBedrag besteld`,
					SUM(Ord.AantalBesteld-%2\$s) AS `intNog te leveren`, SUM(%2\$s * Ord.PrijsPerStuk) AS `curBedrag geleverd`
					FROM %1\$sWS_Orderregel AS Ord INNER JOIN %1\$sWS_Artikel AS Art ON Ord.Artikel = Art.RecordID
					WHERE Ord.Ordernr > 0 AND IFNULL(Art.VervallenPer, CURDATE()) >= CURDATE()
					GROUP BY Art.Code, Art.Omschrijving, Art.Maat
					ORDER BY Art.Code, Art.Omschrijving, Art.Maat;", TABLE_PREFIX, $sqlag);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function add($p_lidid, $p_artikelid, $p_incl_ordernr=0) {
		$nrid = $this->nieuwrecordid();
		$this->lidid = $p_lidid;
		$this->artid = $p_artikelid;
		$this->vulvars();
		
		if ($p_incl_ordernr == 0) {
			$ordnr = 0;
		} else {
			$ordnr = $this->max("Ordernr") + 1;
			if ($ordnr < (date("y") * 1000)) {
				$ordnr = (date("y") * 1000) + 1;
			}
		}
		
		$query = sprintf("INSERT INTO %s (RecordID, Lid, Artikel, AantalBesteld, Ordernr, IngevoerdDoor) VALUES (%d, %d, %d, 1, %d, %d);", $this->table, $nrid, $p_lidid, $p_artikelid, $ordnr, $_SESSION['lidid']);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Orderregel %d met artikel '%s' is toegevoegd.", $nrid, $this->codeartikel);
			$this->log($nrid, 1);
			$this->vulprijsperstuk();
			return $nrid;
		}
	}

	public function update($p_orid, $p_kolom, $p_waarde) {
		$this->vulvars($p_orid);
		
		if ($this->pdoupdate($p_orid, $p_kolom, $p_waarde) > 0) {
			$this->log($p_orid);
		}
	}
	
	public function delete($p_orid, $p_lidid=0, $p_artikelid=0) {
		$this->vulvars($p_orid);
		
		if ($p_orid > 0) {
			$query = sprintf("DELETE FROM %s WHERE RecordID=%d;", $this->table, $p_orid);
		} else {
			$query = sprintf("DELETE FROM %s WHERE Lid=%d AND Artikel=%d AND ISNULL(AantalGeleverd, 0)=0;", $this->table, $p_lidid, $p_artikelid);
		}
		if ($this->execsql($query) > 0) {
			if ($p_orid > 0) {
				$this->mess = sprintf("Orderregel %d met artike '%s' is verwijderd.", $p_orid, $this->codeartikel);
			} else {
				$this->mess = sprintf("De bestelling van artikel '%s' is verwijderd.", $this->codeartikel);
			}
			$this->log($this->orid);
		}
	}
	
	public function vulprijsperstuk() {
		$query = sprintf("SELECT Ord.RecordID, A.Verkoopprijs FROM %s INNER JOIN %sWS_Artikel AS A ON A.RecordID=Ord.Artikel WHERE (Ord.PrijsPerStuk IS NULL) AND A.Verkoopprijs > 0;", $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		foreach($result->fetchAll() as $row) {
			$this->update($row->RecordID, "PrijsPerStuk", $row->Verkoopprijs);
		}
	}
	
	public function definitiefmaken($p_lidid) {
		$this->lidid = $p_lidid;
		$non = $this->max("Ordernr") + 1;
		
		$query = sprintf("SELECT Ord.RecordID FROM %s WHERE Ord.Lid=%d AND Ord.Ordernr=0 AND Ord.AantalBesteld > 0 AND (BestellingDefinitief IS NULL);", $this->basefrom, $p_lidid);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$query = sprintf("UPDATE %s SET Ordernr=%d, BestellingDefinitief=SYSDATE(), GewijzigdDoor=%d WHERE RecordID=%d;", $this->table, $non, $_SESSION['lidid'], $row->RecordID);
			if ($this->execsql($query) > 0) {
				$this->mess = sprintf("Orderregel %d is op order %d geplaatst.", $row->RecordID, $non);
				$this->tas = 30;
				$this->log($row->RecordID);
			}
		}
	}
	
	public function opschonen() {
		
//		$query = sprintf("DELETE FROM %s WHERE AantalBesteld=0 AND AantalGeleverd=0 AND Ingevoerd <= DATE_ADD(CURDATE(), INTERVAL -1 WEEK);", $this->table);
//		$this->execsql($query, 2);
	}

}  # cls_Orderregel

class cls_Voorraadboeking extends cls_db_base {
	
	private $vbid = 0;
	
	function __construct() {
		parent::__construct();
		$this->table = TABLE_PREFIX . "WS_Voorraadboeking";
		$this->basefrom = $this->table . " AS VB";
		$this->ta = 10;
	}
	
	public function lijst($p_art=0) {
		$query = sprintf("SELECT VB.*, Ord.Ordernr, %s AS NaamLid FROM %s LEFT OUTER JOIN (%3\$sWS_Orderregel AS Ord LEFT OUTER JOIN %3\$sLid AS L ON L.RecordID=Ord.Lid) ON Ord.RecordID=VB.OrderregelID", $this->selectnaam, $this->basefrom, TABLE_PREFIX);
		if ($p_art > 0) {
			$query .= sprintf(" WHERE VB.ArtikelID=%d", $p_art);
		}
		$query .= " ORDER BY IFNULL(VB.Datum, CURDATE()), VB.RecordID;";
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function add($p_art, $p_orid=0) {
		$nrid = $this->nieuwrecordid();
		
		$query = sprintf("INSERT INTO %s (RecordID, ArtikelID, OrderregelID, IngevoerdDoor) VALUES (%d, %d, %d, %d);", $this->table, $nrid, $p_art, $p_orid, $_SESSION['lidid']);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Voorraadboeking %d voor artikel %d is toegevoegd.", $nrid, $p_art);
			$this->tas = 21;
			$this->log($nrid);
			return $nrid;
		} else {
			return false;
		}
	}
	
	public function update($p_vbid, $p_kolom, $p_waarde) {
		
		if ($this->pdoupdate($p_vbid, $p_kolom, $p_waarde) > 0) {
			$this->tas = 22;
			$this->log($p_vbid);
		}
	}
	
}  # cls_Voorraadboeking

class cls_Rekening extends cls_db_base {	
	public $rkid = 0;
	private $reksel = "";
	public $debnaam = "";
	public $datum="";
	public $seizoen = 0;
	public $bedrag = 0;
	public $betaald = 0;
	public $begindatumseizoen = "";
	public $adres= "";
	public $postcode = "";
	
	function __construct($p_rkid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Rekening";
		$this->basefrom = $this->table . " AS RK";
		$this->ta = 14;
		$this->pkkol = "Nummer";
		$this->vulvars($p_rkid);
	}
	
	private function vulvars($p_rkid=-1) {
		
		if ($p_rkid >= 0) {
			$this->rkid = $p_rkid;
		}
		
		$this->lidid = 0;
		$this->debnaam = "";
		$this->bedrag = 0;
		$this->betaald = 0;
		
		$this->reksel = sprintf("RK.*, (RK.Bedrag-RK.Betaald) AS Openstaand, %s AS NaamLid, L.Adres, L.Postcode, L.Woonplaats, L.`Machtiging afgegeven` AS Machtiging, L.Bankrekening AS Bankrekeningnummer, (%s) AS Lidnr,
								 L.RecordID AS LidID,
								 (SELECT COUNT(DISTINCT(RR.Lid)) FROM %3\$sRekreg AS RR WHERE RR.Rekening=RK.Nummer AND RR.Lid > 0) AS AantLid, (SELECT IFNULL(MIN(RR.Lid), 0) FROM %3\$sRekreg AS RR WHERE RR.Rekening=RK.Nummer AND RR.Lid > 0) AS EersteLid, 
								 DATE_ADD(RK.Datum, INTERVAL (RK.BETAALDAG*BET_TERM) DAY) AS UitersteBetaaldatum, DATE_ADD(RK.Datum, INTERVAL RK.BETAALDAG DAY) AS EindeEersteTermijn, RK.DEBNAAM AS Tenaamstelling", $this->selectnaam, $this->selectlidnr, TABLE_PREFIX);
		
		if ($this->rkid > 0) {
			$rkrow = $this->record($this->rkid);
			if (isset($rkrow->Nummer) and $rkrow->Nummer > 0) {
				$this->lidid = $rkrow->Lid;
				$this->datum = $rkrow->Datum;
				$this->seizoen = $rkrow->Seizoen;
				$this->debnaam = $rkrow->DEBNAAM;
				$this->bedrag = round($rkrow->Bedrag, 2);
				$this->betaald = round($rkrow->Betaald, 2);
			} else {
				$this->rkid = 0;
				$this->seizoen = 0;
			}
		}
		if ($this->lidid > 0) {
			$f = sprintf("L.RecordID=%d", $this->lidid);
			$this->adres = (new cls_Lid())->max("Adres", $f);
			$this->postcode = (new cls_Lid())->max("Postcode", $f);
		}
		if ($this->seizoen > 0) {
			$query = sprintf("SELECT MIN(SZ.Begindatum) FROM %sSeizoen AS SZ WHERE SZ.Nummer=%d;", TABLE_PREFIX, $this->seizoen);
			$this->begindatumseizoen = $this->scalar($query);
		}
	}
	
	public function lijst($p_filter, $p_lidid=-1) {
		if ($p_filter == "telaat") {
			$p_filter = "WHERE RK.Bedrag > RK.Betaald AND DATE_ADD(RK.Datum, INTERVAL (RK.BET_TERM * RK.BETAALDAG) DAY) < CURDATE()";
		} elseif (strlen($p_filter) > 0 and substr($p_filter, 0, 5) != "WHERE") {
			$p_filter = "WHERE " . $p_filter;
		}
		if ($p_lidid > 0) {
			if (strlen($p_filter) > 0) {
				$p_filter .= sprintf(" AND RK.Lid=%d", $p_lidid);
				
			} else {
				$p_filter = sprintf("WHERE RK.Lid=%d", $p_lidid);
			}
		}
		$query = sprintf("SELECT %s, DATE_ADD(RK.Datum, INTERVAL (RK.BET_TERM * RK.BETAALDAG) DAY) AS Betaaldatum, (RK.Bedrag-RK.Betaald) AS Openstaand FROM %s INNER JOIN %sLid AS L ON RK.Lid=L.RecordID %s ORDER BY RK.Nummer;", $this->reksel, $this->basefrom, TABLE_PREFIX, $p_filter);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function record($p_rkid=-1) {
		if ($p_rkid >= 0) {
			$this->rkid = $p_rkid;
		}
		
		$selbd = sprintf("(SELECT IFNULL(%s, 'lid zelf') FROM %sLid AS L WHERE L.RecordID=RK.BetaaldDoor) AS NaamBetaaldDoor, ", $this->selectnaam, TABLE_PREFIX);
		$selbd .= sprintf("(SELECT IFNULL(%s, '') FROM %sLid AS L WHERE L.RecordID=RK.BetaaldDoor) AS EmailBetaaldDoor", $this->selectemail, TABLE_PREFIX);
		
		$query = sprintf("SELECT %s, %s FROM %s LEFT JOIN %sLid AS L ON RK.Lid=L.RecordID WHERE RK.Nummer=%d;", $this->reksel, $selbd, $this->basefrom, TABLE_PREFIX, $this->rkid);
		$result = $this->execsql($query);
		return $result->fetch();
	}
		
	function overzichtbeheer($p_seizoen=-1, $p_naambevat="") {
		
		$rv = false;
		$w = "";
		if ($p_seizoen > 0) {
			$w = sprintf("WHERE RK.Seizoen=%d", $p_seizoen);
		}
		if (strlen($p_naambevat) > 0) {
			if (strlen($w) > 0) {
				$w .= " AND ";
			} else {
				$w = "WHERE ";
			}
			$w .= sprintf("RK.DEBNAAM LIKE '%%%s%%'", $p_naambevat);
		}
		
		$this->query = sprintf("SELECT RK.Nummer, RK.Datum, RK.OMSCHRIJV, RK.DEBNAAM, RK.Bedrag, RK.Betaald,
								IF((SELECT COUNT(*) FROM %2\$sRekreg AS RR WHERE RR.Rekening=RK.Nummer)=0, RK.Nummer, 0) AS linkDelete
								FROM %1\$s LEFT OUTER JOIN %2\$sLid AS L ON L.RecordID=RK.Lid %3\$s ORDER BY RK.Nummer;", $this->basefrom, TABLE_PREFIX, $w);
		
		try {
			
			$result = $this->execsql();
			$rv = $result->fetchAll();
			
		} catch (Exception $e) {
			debug("Probleem met SQL/database: " . $e->getMessage() . "\n", 1, 1);
		}
		
		return $rv;
		
	}
	
	public function overzichtlid($p_lidid) {
		$query = sprintf("SELECT IF ((SELECT COUNT(*) FROM %sRekreg AS RR WHERE RR.Rekening=RK.Nummer) > 0, RK.Nummer, 0) AS lnkNummer,
					RK.Nummer,
					RK.Seizoen,
					RK.Datum AS Datum,
					RK.OMSCHRIJV AS Omschrijving,
					RK.Bedrag AS Bedrag,
					RK.Betaald AS Betaald,
					(RK.Bedrag-RK.Betaald) AS Openstaand
					FROM %s
					WHERE RK.Lid=%d
					ORDER BY RK.Nummer DESC;", TABLE_PREFIX, $this->basefrom, $p_lidid);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function add($p_rkid, $p_seiz, $p_lidid) {
		$this->seizoen = $p_seiz;
		$this->lidid = $p_lidid;
		$this->tas = 1;
		$i_seiz = new cls_Seizoen($this->seizoen);
		$i_lid = new cls_Lid($p_lidid);
		
		if ($this->lidid <= 0) {
			$this->mess = "Er is geen (Klos)lid gespecificeerd. De rekening wordt niet toegevoegd.";
			$this->log(0, 1);
			return false;
			
		} else {
		
			$nwreknr = $this->nieuwrecordid($p_rkid);

			$this->query = sprintf("INSERT INTO %1\$s (%2\$s, Seizoen, Datum, Lid, BetaaldDoor, BET_TERM, Betaald, Ingevoerd) VALUES (%3\$d, %4\$d, CURDATE(), %5\$d, %5\$d, 1, 0, CURDATE());", $this->table, $this->pkkol, $nwreknr, $p_seiz, $this->lidid);
		
			if ($this->execsql() > 0) {
				$this->mess = sprintf("Rekening %d voor lid %d is toegevoegd.", $nwreknr, $this->lidid);
				$this->log($nwreknr);
			
				$sql = sprintf("INSERT INTO Rekening (%1\$s, Seizoen, Datum, Lid, BetaaldDoor, BET_TERM, Betaald, Ingevoerd) VALUES (%2\$d, %3\$d, CURDATE(), %4\$d, %4\$d, 1, 0, CURDATE());", $this->pkkol, $nwreknr, $p_seiz, $this->lidid);
				$this->Interface($sql);
			
				$this->update($nwreknr, "DEBNAAM", $i_lid->Naam());
			
				$szrow = $i_seiz->record($p_seiz);
				$this->update($nwreknr, "OMSCHRIJV", $szrow->Rekeningomschrijving);
				$this->update($nwreknr, "BETAALDAG", $szrow->BetaaldagenTermijn);
			
				return $nwreknr;
			} else {
				return false;
			}
		}
	}  # add
	
	public function update($p_rkid, $p_kolom, $p_waarde) {
		if ($p_rkid > 0) {
			$this->rkid = $p_rkid;
		}
		$this->tas = 2;
		$this->vulvars();
		
		if ($p_kolom == "Datum" and strlen($p_waarde) == 0) {
			$this->mess = "De datum is een verplicht veld en mag niet leeg zijn, deze wijziging wordt niet doorgevoerd.";
			$this->log($this->rkid);
		} elseif ($p_kolom == "Datum" and strtotime($p_waarde) === false) {
			$this->mess = sprintf("%s is geen geldige datum, deze wijziging wordt niet doorgevoerd", $p_waarde);
		} elseif ($p_kolom == "BETAALDAG" and is_numeric($p_waarde) === false) {
			$this->mess = sprintf("%s is geen geldige waarde voor betaaldagen, deze wijziging wordt niet doorgevoerd", $p_waarde);
			$this->log($this->rkid);
		} elseif ($this->pdoupdate($this->rkid, $p_kolom, $p_waarde) > 0) {
			$this->log($this->rkid);
		}
	}
	
	public function delete($p_rkid, $p_inclregels=0, $p_reden="") {
		$this->tas = 3;
		$this->vulvars($p_rkid);
		
		if ($p_inclregels == 1) {
			$query = sprintf("DELETE FROM %sRekreg WHERE Rekening=%d;", TABLE_PREFIX, $this->rkid);
			$this->execsql($query);
			
			if ($_SESSION['settings']['interface_access_db'] == 1) {
				$sql = sprintf("DELETE FROM Rekreg WHERE Rekening=%d;", $this->rkid);
				(new cls_Interface())->add($sql);
			}
		}
		
		$query = sprintf("SELECT COUNT(*) FROM %sRekreg AS RR WHERE RR.Rekening=%d;", TABLE_PREFIX, $this->rkid);
		if ($this->scalar($query) > 0) {
			$this->mess = sprintf("Rekening %d mag niet worden verwijderd, omdat deze regels heeft.", $this->rkid);
		} else {
			$this->pdodelete($p_rkid, $p_reden);
		}
		$this->log($p_rkid);
	}
	
	public function controle($p_rkid=-1, $p_seizoen=-1) {
		$starttijd = microtime(true);
		$i_rr = new cls_Rekeningregel();
		$i_lid = new cls_Lid();
		$i_seiz = new cls_Seizoen();
		$i_rb = new cls_RekeningBetaling();
		
		$f = "";
		if ($p_rkid > 0) {
			$f = sprintf("RK.Nummer=%d", $p_rkid);
		} elseif ($p_seizoen > 0) {
			$f = sprintf("RK.Seizoen=%d", $p_seizoen);
		}
		
		$rkrows = $this->basislijst($f);
		foreach ($rkrows as $rkrow) {
			$f = sprintf("RecordID=%d", $rkrow->Lid);
			if ($rkrow->Lid != 0 and $i_lid->aantal($f) == 0) {
				$this->update($rkrow->Nummer, "Lid", 0);
			} elseif ($rkrow->BetaaldDoor == 0 and $rkrow->Lid > 0) {
				$this->update($rkrow->Nummer, "BetaaldDoor", $rkrow->Lid);
			} elseif (strlen($rkrow->DEBNAAM) < 3 and $rkrow->Lid > 0) {
				$this->update($rkrow->Nummer, "DEBNAAM", $i_lid->Naam($rkrow->Lid));
			} elseif ($rkrow->Seizoen < 1) {
				$f = sprintf("Begindatum <= '%1\$s' AND Einddatum >= '%1\$s'", $rkrow->Datum);
				$s = $i_seiz->max("Nummer", $f);
				if ($s == 0) {
					$s = $i_seiz->max("Nummer");
				}
				$this->update($rkrow->Nummer, "Seizoen", $s);
			} elseif (strlen($rkrow->OMSCHRIJV) < 2 and $rkrow->Seizoen > 0) {
				$f = sprintf("SZ.Nummer=%d", $rkrow->Seizoen);
				$this->update($rkrow->Nummer, "OMSCHRIJV", $i_seiz->max("Rekeningomschrijving", $f));
			} elseif ($rkrow->BETAALDAG < 1 and $rkrow->Seizoen > 0) {
				$f = sprintf("SZ.Nummer=%d", $rkrow->Seizoen);
				$this->update($rkrow->Nummer, "BETAALDAG", $i_seiz->max("Betaaldagentermijn", $f));
			} elseif ($rkrow->BET_TERM < 1) {
				$this->update($rkrow->Nummer, "BET_TERM", 1);
			}

			$f = sprintf("RR.Rekening=%d", $rkrow->Nummer);
			if ($i_rr->aantal($f) > 0) {
				if ($rkrow->Lid == 0) {
					$f = sprintf("RR.Rekening=%d AND RR.Lid > 0", $rkrow->Nummer);
					$this->update($rkrow->Nummer, "Lid", $i_rr->min("RR.Lid", $f));
				}
			}
			
			$this->update($rkrow->Nummer, "Bedrag", $i_rr->totaalrekening($rkrow->Nummer));
			if ($i_rb->min("Datum") >= $rkrow->Datum) {
				$this->update($rkrow->Nummer, "Betaald", $i_rb->totaalrekening($rkrow->Nummer));
			}
		}
	
		$i_rr = null;
		$i_lid = null;
		$i_seiz = null;
		
		if ($p_rkid <= 0) {
			if ($p_seizoen > 0) {
				$this->mess = sprintf("De controle van de rekeningen van seizoen %d is in %.3f seconden uitgevoerd.", $p_seizoen, (microtime(true) - $starttijd));
			} else {
				$this->mess = sprintf("De controle van alle rekeningen is in %.3f seconden uitgevoerd.", (microtime(true) - $starttijd));
			}
			$this->rkid = 0;
			$this->tas = 19;
			$this->Log();
		}
		
	}  #  controle
	
	
	public function opschonen() {
		$this->tas = 18;
		
		if ($_SESSION['settings']['rekening_bewaartermijn'] > 3) {
			$query = sprintf("SELECT RK.Nummer FROM %s WHERE RK.Datum < DATE_SUB(CURDATE(), INTERVAL %d MONTH) LIMIT 50;", $this->basefrom, $_SESSION['settings']['rekening_bewaartermijn']);
			$result = $this->execsql($query);
			$reden = sprintf("de rekening meer dan %d maanden oud is.", $_SESSION['settings']['rekening_bewaartermijn']);
			$aantrek = 0;
			foreach ($result->fetchAll() as $rkrow) {
				$this->delete($rkrow->Nummer, 1, $reden);
				$aantrek++;
			}
		}
		
		if ($aantrek > 0) {
			$this->mess = sprintf("In totaal %d rekeningen verwijderd omdat deze ouder dan %d maanden waren.", $aantrek, $_SESSION['settings']['rekening_bewaartermijn']);
			$this->Log(0, 1);
		}
		
	}  # opschonen
	
}  # cls_Rekening

class cls_Rekeningregel extends cls_db_base {
	
	private $rrid = 0;
	private $rkid = 0;
	private $rekeningdatum = "";
	private $regelnr = 0;
	private $seizoen = 0;
	private $begindatumseizoen = "";
	
	function __construct($p_rkid=-1, $p_rrid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Rekreg";
		$this->basefrom = $this->table . sprintf(" AS RR INNER JOIN %sRekening AS RK ON RR.Rekening=RK.Nummer", TABLE_PREFIX);
		$this->ta = 14;
		if ($p_rrid >= 0) {
			$this->rrid = $p_rrid;
		}
		if ($p_rkid >= 0) {
			$this->rkid = $p_rkid;
			$this->seizoen = (new cls_Rekening())->max("Seizoen", sprintf("Nummer=%d", $this->rkid));
		}
	}
	
	private function vulvars($p_rrid=-1, $p_rkid=-1, $p_lidid=-1) {
		if ($p_rrid >= 0) {
			$this->rrid = $p_rrid;
		}
		
		if ($p_rkid >= 0) {
			$this->rkid = $p_rkid;
		}
		
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		
		if ($this->rrid > 0) {
			$query = sprintf("SELECT RR.Rekening, RR.Regelnr, RR.Lid, RK.Seizoen, RK.Datum, RK.Seizoen FROM %s WHERE RR.RecordID=%d;", $this->basefrom, $this->rrid);
			$result = $this->execsql($query);
			$row = $result->fetch();
			if (isset($row->Rekening)) {
				$this->rkid = $row->Rekening;
				$this->lidid = $row->Lid;
				$this->regelnr = $row->Regelnr;
				$this->seizoen = $row->Seizoen;
				$this->rekeningdatum = $row->Datum;
			}

		} elseif ($p_rkid >= 0) {
			$this->rkid = $p_rkid;
			$query = sprintf("SELECT RK.Seizoen, RK.Datum FROM %sRekening AS RK WHERE RK.Nummer=%d;", TABLE_PREFIX, $this->rkid);
			$result = $this->execsql($query);
			$row = $result->fetch();
			if (isset($row->Seizoen)) {
				$this->seizoen = $row->Seizoen;
				$this->rekeningdatum = $row->Datum;
			}
		}
		
		if ($this->seizoen > 0) {
			$query = sprintf("SELECT MIN(SZ.Begindatum) FROM %sSeizoen AS SZ WHERE SZ.Nummer=%d;", TABLE_PREFIX, $this->seizoen);
			$this->begindatumseizoen = $this->scalar($query);
		}
	}
	
	private function record($p_rrid, $p_zoek="") {
		$this->vulvars($p_rrid);
		
		if ($this->rrid <= 0 and strlen($p_zoek) > 0) {
			$w = $p_zoek;
		} else {
			$w = sprintf("RR.RecordID=%d", $this->rrid);
		}
		
		$query = sprintf("SELECT RR.* FROM %s WHERE %s;", $this->basefrom, TABLE_PREFIX, $w);
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
	function perrekening($p_rkid=-1, $p_xf="", $p_order="") {
		if ($p_rkid > 0) {
			$this->rkid = $p_rkid;
		}
		
		if (strlen($p_xf) > 0) {
			$p_xf = " AND " . $p_xf;
		}
		
		if (strlen($p_order) > 0) {
			$p_order = $p_order . ", ";
		}
		
		$query = sprintf("SELECT RR.*, %s AS NaamLid, L.GEBDATUM, L.EmailOuders FROM %s LEFT JOIN %sLid AS L ON RR.Lid=L.RecordID WHERE RR.Rekening=%d%s ORDER BY %sRegelnr;", $this->selectnaam, $this->basefrom, TABLE_PREFIX, $this->rkid, $p_xf, $p_order);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function overzichtlid($p_lidid) {
		$this->lidid = $p_lidid;
		$query = sprintf("SELECT DISTINCT IF((SELECT COUNT(*) FROM %1\$sRekreg AS RR WHERE RR.Rekening=RK.Nummer) > 0, RK.Nummer, 0) AS lnkNummer,
					RK.Nummer,
					RK.Seizoen,
					RK.Datum,
					RK.OMSCHRIJV AS Omschrijving,
					RK.Bedrag
					FROM %2\$s
					WHERE RR.Lid=%3\$d
					ORDER BY RK.Nummer DESC;", TABLE_PREFIX, $this->basefrom, $this->lidid);
		$result = $this->execsql($query);
		return $result->fetchAll();
		
	}  # overzichtlid

	public function standaardwaarde($p_rkid, $p_lidid) {
		$this->vulvars(0, $p_rkid, $p_lidid);
		
		$i_lo = new cls_Lidond(-1, $this->lidid);
		$i_lid = new cls_Lid($this->lidid);
		$i_sz = new cls_Seizoen();
		
		$szrow = $i_sz->record($this->seizoen);
		
		$jl = false;
		if ($i_lid->geboortedatum > $i_sz->peildatumjeugdlid) {
			$jl = true;
		}
		
		$is_lid = (new cls_Lidmaatschap())->islid($p_lidid, $this->rekeningdatum);
		
		$aantToegevoegd = 0;
		
		$oms = "";
		$cb = 0;
		
		$kpl = strtoupper($szrow->{'Verenigingscontributie kostenplaats'});
		$query = sprintf("SELECT COUNT(*) FROM %s WHERE RK.Seizoen=%d AND RR.KSTNPLTS='%s' AND RR.Lid=%d;", $this->basefrom, TABLE_PREFIX, $i_sz->szid, $kpl, $this->lidid);
		if ($this->scalar($query) == 0 and $is_lid) {
			$oms = $szrow->{'Verenigingscontributie omschrijving'};
			$onderscheiding = (new cls_lid())->onderscheiding($this->lidid);
			if (strlen($onderscheiding) > 0) {
				$cb = 0;
				$oms .= " " . $onderscheiding;
			} elseif ($i_lid->iskader($this->lidid, $this->rekeningdatum)) {
				$cb = $szrow->{'Contributie kader'};
				$oms .= " kader";
			} elseif ($jl) {
				$cb = $szrow->{'Contributie jeugdleden'};
				$oms .= " jeugdlid";
			} else {
				$cb = $szrow->{'Contributie leden'};
			}

			if ($i_lid->lidvanaf > $szrow->Begindatum) {
				$oms .= " vanaf " . date("d-m-Y", strtotime($i_lid->lidvanaf));
			}
			
			$rrid = $this->add($p_rkid, $this->lidid, $kpl);
			if ($rrid > 0) {
				$this->update($rrid, "OMSCHRIJV", $oms);
				$this->update($rrid, "Bedrag", $cb);
				$aantToegevoegd++;
			}
		}

		foreach ($i_lo->lijstperlid($this->lidid, "A", $this->rekeningdatum) as $lorow) {
				
			$query = sprintf("SELECT COUNT(*) FROM %s WHERE RK.Seizoen=%d AND RR.LidondID=%d;", $this->basefrom, TABLE_PREFIX, $i_sz->szid, $lorow->RecordID);
			if ($this->scalar($query) == 0) {
				$oms = $lorow->OndNaam;
				$kpl = $lorow->OndCode;
				
				if ($lorow->Kader == 1) {
					$cb = $lorow->FUNCTCB;
					if (strlen($lorow->FunctieOms) > 0) {
						$oms .= " (" . $lorow->FunctieOms . ")";
					}
				} elseif ($jl) {
					$cb = $lorow->JEUGDCB;
				} else {
					$cb = $lorow->LIDCB;
				}
				
				if (isset($lorow->GrActiviteit) and strlen($lorow->GrActiviteit) > 0 and isset($lorow->GrContributie) and $lorow->GrContributie > 0) {
					$oms .= " (" . $lorow->GrActiviteit . ")";
					$kpl .= "-" . $lorow->ActCode;
					$cb += $lorow->GrContributie;
				}
				if ($lorow->Vanaf > $szrow->Begindatum) {
					$oms .= " vanaf " . date("d-m-Y", strtotime($lorow->Vanaf));
				}
				
				if (round($cb, 2) <> 0) {
					$rrid = $this->add($p_rkid, $lorow->Lid, $kpl);
					if ($rrid > 0) {
						$this->update($rrid, "OMSCHRIJV", $oms);
						$this->update($rrid, "Bedrag", $cb);
						$this->update($rrid, "LidondID", $lorow->RecordID);
						$aantToegevoegd++;
					}
				}
			}
		}

		$i_lo = null;
		
		return $aantToegevoegd;
	}  # standaardwaarde
	
	public function totaalrekening($p_rkid) {
		$query = sprintf("SELECT IFNULL(SUM(RR.Bedrag), 0) FROM %s WHERE RR.Rekening=%d;", $this->basefrom, $p_rkid);
		return round($this->scalar($query), 2);
	}  # totaalrekening
	
	public function add($p_rkid, $p_lidid=-1, $p_kpl="") {
		$this->vulvars(0, $p_rkid, $p_lidid);
		$this->tas = 11;
		
		$nrid = $this->nieuwrecordid();
		$rnr = $this->max("Regelnr", sprintf("Rekening=%d", $this->rkid)) + 1;
		
		$query = sprintf("INSERT INTO %s (RecordID, Rekening, Lid, Regelnr, KSTNPLTS) VALUES (%d, %d, %d, %d, '%s');", $this->table, $nrid, $this->rkid, $this->lidid, $rnr, $p_kpl);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Regel %d aan rekening %d toegevoegd", $rnr, $this->rkid);
			$this->log($nrid);
			(new cls_Interface())->add($query);			
		} else {
			$nrid = 0;
		}
		
		return $nrid;
		
	}  # add
	
	public function update($p_rrid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_rrid);
		$this->tas = 12;
		
		if ($this->pdoupdate($this->rrid, $p_kolom, $p_waarde)) {
			$this->mess = sprintf("Op regel %d van rekening %d is kolom '%s' in '%s' gewijzigd", $this->regelnr, $this->rkid, $p_kolom, $p_waarde);
			if (strlen($p_reden) > 0) {
				$this->mess .= ", omdat " . $p_reden;
			}
			$this->log($this->rrid);
		}
	}
	
	public function delete($p_rrid, $p_reden="") {
		$this->vulvars($p_rrid);
		$this->tas = 13;
		
		if ($this->pdodelete($this->rrid, $p_reden)) {
			$this->mess = sprintf("Tabel Rekreg: record %d (%d-%d) is verwijderd", $this->rrid, $this->rkid, $this->regelnr);
			if (strlen($p_reden) > 0) {
				$this->mess .= ", omdat " . $p_reden;
			}
			$this->log($this->rrid);
		}
	}  # delete
	
	public function opschonen() {
		$query = sprintf("SELECT RR.RecordID FROM %s AS RR WHERE RR.Rekening NOT IN (SELECT RK.Nummer FROM %sRekening AS RK);", $this->table, TABLE_PREFIX);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, "de bijbehorende rekening niet (meer) bestaat.");
		}
	}  #  opschonen
	
}  # cls_Rekeningregel

class cls_RekeningBetaling extends cls_db_base {
	
	private $rbid = 0;
	private $rkid = 0;
	
	function __construct($p_rbid=-1) {
		$this->table = TABLE_PREFIX . "RekeningBetaling";
		$this->basefrom = $this->table . " AS RB";
		$this->pkkol = "RecordID";
		$this->vulvars($p_rbid);
		$this->ta = 14;
	}

	private function vulvars($p_rbid=-1) {
		if ($p_rbid >= 0) {
			$this->rbid = $p_rbid;
		}
		if ($this->rbid > 0) {
			$query = sprintf("SELECT RB.*, RK.Lid FROM %s INNER JOIN %sRekening AS RK ON RB.Rekening-RK.Nummer WHERE RB.RecordID=%d;", $this->basefrom, TABLE_PREFIX, $this->rbid);
			$result = $this->execsql($query);
			$row = $result->fetch();
			$this->rkid = $row->Rekening; 
			$this->lidid = $row->Lid;
		}
	}
	
	public function laatste($p_aantal=50) {
		$query = sprintf("SELECT RB.*, (RK.Bedrag-RK.Betaald) AS Openstaand FROM %s LEFT JOIN %sRekening AS RK ON RB.Rekening=RK.Nummer ORDER BY RB.Datum DESC, RB.RecordID DESC LIMIT %d;", $this->basefrom, TABLE_PREFIX, $p_aantal);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function totaalrekening($p_rkid) {
		$query = sprintf("SELECT IFNULL(SUM(RB.Bedrag), 0) FROM %s WHERE RB.Rekening=%d;", $this->basefrom, $p_rkid);
		return round($this->scalar($query), 2);
	}  # totaalrekening
	
	public function add($p_rkid, $p_datum, $p_bedrag) {
		$i_rk = new cls_Rekening($p_rkid);
		$nrid = 0;
		$this->tas = 21;
		
		if (strlen($p_bedrag) == 0) {
			$bedrag = 0;
		} else {
			$bedrag = round(floatval(str_replace(",", ".", $p_bedrag)), 2);
		}
		
		if ($bedrag == 0) {
			$this->mess = "Tabel RekeningBetaling: bedrag is 0, betaling wordt niet toegevoegd";

		} elseif ($i_rk->rkid == 0) {
			$this->mess = sprintf("Tabel RekeningBetaling: rekening %d bestaat niet, betaling wordt niet toegevoegd", $p_rkid);
		
		} elseif (strlen($p_datum) != 10) {
			$this->mess = "Tabel RekeningBetaling: de datum is niet correct, betaling wordt niet toegevoegd";

		} else {
			$nrid = $this->nieuwrecordid();
			$query = sprintf("INSERT INTO %s (RecordID, Rekening, Datum, Bedrag) VALUES (%d, %d, '%s', %.2F);", $this->table, $nrid, $p_rkid, $p_datum, $bedrag);
			if ($this->execsql($query) > 0) {
				$this->mess = sprintf("Tabel RekeningBetaling: betaling voor rekening %d van %.2f toegevoegd", $p_rkid, $bedrag);
				(new cls_Rekening())->controle($p_rkid);
			} else {
				$nrid = 0;
				$this->mess = "Tabel RekeningBetaling: toevoegen record mislukt";
			}
		}
		$this->log($nrid);
		$i_rk = null;
		return $nrid;

	}  # add
	
	public function delete($p_rbid) {
		$this->vulvars($p_rbid);
		$this->tas = 23;
		
		if ($this->pdodelete($this->rbid) > 0) {
			$this->log($this->rbid);
		}
	}
	
	public function controle() {
	}
	
	public function opschonen() {
	}
	
}  #  cls_RekeningBetaling

class cls_Seizoen extends cls_db_base {
	
	public $szid = 0;
	public $begindatum = "1900-01-01";
	public $einddatum = "9999-12-31";
	public $peildatumjeugdlid = "1900-01-01";
	
	function __construct($p_szid=-1) {
		$this->table = TABLE_PREFIX . "Seizoen";
		$this->basefrom = $this->table . " AS SZ";
		$this->pkkol = "Nummer";
		$this->vulvars($p_szid);
		$this->ta = 20;
	}
	
	private function vulvars($p_szid=-1) {
		if ($p_szid >= 0) {
			$this->szid = $p_szid;
		}
		if ($this->szid > 0) {
			$query = sprintf("SELECT SZ.Begindatum, SZ.Einddatum, DATE_SUB(SZ.Begindatum, INTERVAL SZ.`Leeftijdsgrens jeugdleden` YEAR) AS PDJL FROM %s WHERE SZ.Nummer=%d;", $this->basefrom, $this->szid);
			$result = $this->execsql($query);
			$szrow = $result->fetch();
			$this->begindatum = $szrow->Begindatum;
			$this->einddatum = $szrow->Einddatum;
			$this->peildatumjeugdlid = $szrow->PDJL;
		}
	}
	
	public function record($p_szid=-1) {
		$this->vulvars($p_szid);
		
		$this->query = sprintf("SELECT SZ.*, DATE_SUB(SZ.Begindatum, INTERVAL SZ.`Leeftijdsgrens jeugdleden` YEAR) AS PDJL FROM %s WHERE `%s`=%d;", $this->basefrom, $this->pkkol, $this->szid);
		$result = $this->execsql();
		return $result->fetch();
	}
	
	function htmloptions($p_cv=-1, $p_filter=0) {
		global $dtfmt;
		
		$dtfmt->setPattern(DTSHORT);
		$rv = "";
		foreach ($this->lijst(1, $p_filter) as $row) {
			$s = checked($row->Nummer, "option", $p_cv);
			$rv .= sprintf("<option value=%d %s>%s: %s t/m %s</option>\n", $row->Nummer, $s, $row->Nummer, $dtfmt->format(strtotime($row->Begindatum)), $dtfmt->format(strtotime($row->Einddatum)));
		}
		return $rv;
	}
	
	function lijst($p_fetched=1, $p_filter=0) {
		/*
			$p_filter
			0: alle seizoenen
			1: seizoenen met een rekening
		*/
		
		$this->query = sprintf("SELECT SZ.*, (SELECT COUNT(*) FROM %sRekening AS RK WHERE RK.Seizoen=SZ.Nummer) AS aantalRek FROM %s", TABLE_PREFIX, $this->basefrom);
		if ($p_filter == 1) {
			$this->query .= sprintf(" WHERE (SELECT COUNT(*) FROM %sRekening AS RK WHERE RK.Seizoen=SZ.Nummer) > 0", TABLE_PREFIX);
		}
		$this->query .= " ORDER BY SZ.Begindatum DESC;";
		$result = $this->execsql();
		if ($p_fetched == 1) {
			return $result->fetchAll();
		} else {
			return $result;
		}
	}
	
	public function eindehuidige($p_per="") {
		
		if (strlen($p_per) < 8) {
			$p_per = date("Y-m-d");
		}
		
		debug("Functie vervangen");
		
		$query = sprintf("SELECT SZ.Nummer FROM %1\$s WHERE SZ.Begindatum <= '%2\$s' AND SZ.Einddatum >= '%2\$s';", $this->basefrom, $p_per);
		$this->vulvars($this->scalar($query));
		return $this->einddatum;
	}
	
	public function zethuidige($p_per="", $p_szid=0) {
		
		if (strlen($p_per) == 10) {
			$query = sprintf("SELECT SZ.Nummer FROM %1\$s WHERE SZ.Begindatum <= '%2\$s' AND SZ.Einddatum >= '%2\$s';", $this->basefrom, $p_per);
		} elseif ($p_szid > 0) {
			$query = sprintf("SELECT IFNULL(SZ.Nummer, 0) FROM %s WHERE SZ.Nummer=%d;", $this->basefrom, $p_szid);
		} else {
			$query = sprintf("SELECT MAX(SZ.Nummer) FROM %s;", $this->basefrom, $p_szid);
		}
		$this->vulvars($this->scalar($query));
		return $this->szid;
	}
	
	public function add($p_nr=0) {
		$this->tas = 21;
		
		if ($p_nr > 0) {
			$nnr = $p_nr;
		} else {
			$nnr = $this->max("Nummer") + 1;
			if ($nnr < 2000) {
				$nnr = 2000;
			}
		}
		
		$bd = new datetime($this->max("Einddatum"));
		$bd->modify("+1 day");
		$ed = new datetime($bd->format("Y-M-d"));
		$ed->modify("+1 year");
		$ed->modify("-1 day");
		
		$query = sprintf("INSERT INTO %s (Nummer, Begindatum, Einddatum) VALUES (%d, '%s', '%s');", $this->table, $nnr, $bd->format("Y-m-d"), $ed->format("Y-m-d"));
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Seizoen %d is toegevoegd.", $nnr);
			$this->Log($nnr);
			(new cls_Interface())->add($query);
		}
	}
	
	public function update($p_szid, $p_kolom, $p_waarde) {
		$this->vulvars($p_szid);
		$this->tas = 22;
				
		if ($this->pdoupdate($this->szid, $p_kolom, $p_waarde)) {
			$this->Log($this->szid);
		}
	}

	public function delete($p_seiznr) {
		$this->tas = 23;
		$this->pdodelete($p_seiznr);
		$this->log($p_seiznr);
	}
	
}  # cls_Seizoen

class cls_Stukken extends cls_db_base {
	private $stid = 0;
	
	function __construct() {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Stukken";
		$this->basefrom = $this->table . " AS S";
		$this->ta = 22;
	}

	public function record($p_stid) {
		if ($p_stid > 0) {
			$this->stid = $p_stid;
		}
		$this->query = sprintf("SELECT S.* FROM %s WHERE S.RecordID=%d;", $this->basefrom, $this->stid);
		return $this->execsql()->fetch();
	}
	
	public function editlijst() {
		
		$query = sprintf("SELECT RecordID, S.Titel, S.`Type`, BestemdVoor, VastgesteldOp, Revisiedatum, VervallenPer FROM %s ORDER BY S.VervallenPer, S.Titel;", $this->basefrom);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function gewijzigdestukken() {
		$vl = (new cls_Logboek())->vorigelogin(0);
		if ($vl > date("Y-m-d H:i:s", mktime(date("h"), date("i"), 0, date("m")  , date("d")-7, date("Y")))) {
			$vl = date("Y-m-d H:i:s", mktime(date("h"), date("i"), 0, date("m")  , date("d")-7, date("Y")));
		}
		
		$query = sprintf("SELECT S.Titel FROM %s WHERE IFNULL(S.VervallenPer, CURDATE()) >= CURDATE()", $this->basefrom);
		if (strlen($vl) >= 10) {
			$query .= sprintf(" AND (S.VastgesteldOp >= '%1\$s' OR S.Ingangsdatum >= '%1\$s')", $vl);
		}
		$query .= " ORDER BY S.GewijzigdOp DESC;";
		
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function add() {
		$nrid = $this->nieuwrecordid();
		$query = sprintf("INSERT INTO %sStukken (RecordID, Titel, BestemdVoor, Link) VALUES (%d, '*** Nieuw stuk ***', 'Leden', '');", TABLE_PREFIX, $nrid);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Stuk met RecordID %d is toegevoegd.", $nrid);
			$this->tas = 1;
			$this->log($nrid);
		}
	}
	
	public function update($p_sid, $p_kolom, $p_waarde) {
		
		if ($this->pdoupdate($p_sid, $p_kolom, $p_waarde) > 0) {
			$this->tas = 2;
			$this->log($p_sid);
		}
	}

	public function delete($p_stid) {
		$this->pdodelete($p_stid);
		$this->log($p_stid, 1);
	}
	
}  # cls_Stukken

class cls_Eigen_lijst extends cls_db_base {
	
	public $elid = 0;
	private $default_waarde_params = "";
	public $sqlerror = "";
	public $elnaam = "";
	public $mysql = "";
	public $eigenscript = "";
	private $aantalkolommen = 0;
	private $kolomlidid = -1;
	
	function __construct($p_naam="", $p_elid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Eigen_lijst";
		$this->basefrom = $this->table . " AS EL";
		$this->ta = 23;
		if (strlen($p_naam) > 1) {
			$query = sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %s WHERE Naam='%s';", $this->table, $p_naam);
			$this->elid = $this->scalar($query);
		} elseif ($p_elid > 0) {
			$query = sprintf("SELECT IFNULL(MAX(RecordID), 0) FROM %s WHERE RecordID=%d;", $this->table, $p_elid);
			$this->elid = $this->scalar($query);
		}
		if ($this->elid > 0) {
			$this->vulvars($this->elid);
		}
	}
	
	private function vulvars($p_elid=-1) {
		if ($p_elid > 0) {
			$this->elid = $p_elid;
		}
		if ($this->elid > 0) {
			$query = sprintf("SELECT IFNULL(EL.Naam, '') AS Naam, MySQL, IFNULL(Default_value_params, '') AS Default_value_params, IFNULL(EigenScript, '') AS EigenScript, EL.AantalKolommen, EL.KolomLidID
							  FROM %s WHERE EL.RecordID=%d;", $this->basefrom, $this->elid);
			$elrow = $this->execsql($query)->fetch();
			$this->elnaam = $elrow->Naam;
			$this->mysql = $elrow->MySQL;
			$this->default_waarde_params = $elrow->Default_value_params;
			$this->eigenscript = $elrow->EigenScript;
			$this->aantalkolommen = $elrow->AantalKolommen;
			$this->kolomlidid = $elrow->KolomLidID;
		}
	}
	
	public function recordid($p_filter) {
		$query = sprintf("SELECT IFNULL(MIN(EL.RecordID), 0) FROM %s WHERE %s", $this->basefrom, $p_filter);
		$this->elid = $this->scalar($query);
		return $this->elid;
	}
	
	public function lijst($p_filter="") {
		
		$w = "";
		if (strlen($p_filter) > 0) {
			$w = "WHERE " . $p_filter;
		}
		$query = sprintf("SELECT * FROM %s %s ORDER BY EL.Naam;", $this->basefrom, $w);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function htmloptions($p_cv=-1, $p_filter=0) {
		
		$xw = "";
		if ($p_filter == 1) {
			// Alleen eigen lijsten met records
			$w = "WHERE AantalRecords > 0 ";
		} elseif ($p_filter == 2) {
			// Alleen eigen lijsten geschikt kunnen worden in een mailing
			$w = "WHERE KolomLidID >= 0 ";
		}
		
		$query = sprintf("SELECT * FROM %s WHERE KolomLidID >= 0 %sORDER BY Naam;", $this->table, $xw);
		$result = $this->execsql($query);
		
		$rv = "";
		foreach ($result->fetchAll() as $row) {
			$s = checked($row->RecordID, "option", $p_cv);
			$o = $row->Naam;
			if ($row->AantalRecords > 1) {
				$o .= sprintf(" (%d leden)", $row->AantalRecords);
			} elseif ($row->AantalRecords == 0) {
				$o .= " (geen records)";
			} else {
				$o .= " (1 lid)";
			}
			$rv .= sprintf("<option%s value=%d>%s</option>\n", $s, $row->RecordID, $o);
		}
		return $rv;
	}
	
	public function mysql($p_elid=-1, $p_as_sub=0) {
		$this->vulvars($p_elid);
		
		$rv = $this->mysql;
		if ($p_as_sub == 1) {
			if (substr($rv, -1) == ";") {
				$rv = substr($rv, 0, -1);
			}
			if ($this->aantalkolommen > 1 and $this->kolomlidid >= 0) {
				$rv = sprintf("SELECT SQ.LidID FROM (%s) AS SQ", $rv);
			}
			$rv = sprintf("(%s)", $rv);
		}
		
		return $rv;
	}
	
	public function controle($p_elid=-1, $p_altijd=0, $p_maxaantal=3) {
		global $dbc;
		
		$rv = 0;

		if ($p_elid > 0) {
			$this->elid = $p_elid;
		}
		if ($this->elid > 0 and $p_altijd == 1) {
			$w = sprintf("EL.RecordID=%d", $this->elid);
		} elseif ($this->elid > 0) {
			$w = sprintf("EL.RecordID=%d AND EL.LaatsteControle < DATE_SUB(NOW(), INTERVAL 5 MINUTE)", $this->elid);
		} else {
			$w = "EL.LaatsteControle < DATE_SUB(NOW(), INTERVAL 8 HOUR)";
		}
		
		$query = sprintf("SELECT * FROM %s WHERE %s LIMIT %d;", $this->basefrom, $w, $p_maxaantal);
		$result = $this->execsql($query);
		$elrows = $result->fetchAll();
		foreach ($elrows as $row) {
			$this->vulvars($row->RecordID);
			$cc = 0;
			$rc = 0;
			$kol_lidid = -1;
			$naameerstekolom = "";
			if (strlen($this->mysql) > 7) {
				$elres = $dbc->prepare($this->mysql);
				try {
					$elres->execute();
					$cc = $elres->ColumnCount();
					$rc = $elres->RowCount();
					$this->sqlerror = "";
		
				} catch (Exception $e) {
					$this->sqlerror = $e->getMessage();
				}
			
				if (strlen($this->sqlerror) == 0) {
					for ($i=0;$i<$cc;$i++) {
						if ($elres->getColumnMeta($i)['name'] == "LidID" or $elres->getColumnMeta($i)['name'] == "ndLidID") {
							$kol_lidid = $i;
						}
						if ($i == 0) {
							$naameerstekolom = $elres->getColumnMeta($i)['name'];
						}
					}
				}
				if (strlen($this->eigenscript) > 0) {
					$this->update($row->RecordID, "EigenScript", "", "de SQL-code is ingevuld.");
				}
			} elseif (strlen($this->eigenscript) > 4) {
				if (file_exists(BASEDIR . "/maatwerk/" . $this->eigenscript) == false) {
					$this->update($row->RecordID, "EigenScript", "", "het script niet bestaat.");
				}
			}
			
			$this->update($row->RecordID, "AantalKolommen", $cc);
			$this->update($row->RecordID, "AantalRecords", $rc);
			$this->update($row->RecordID, "KolomLidID", $kol_lidid);
			$this->update($row->RecordID, "NaamEersteKolom", $naameerstekolom);
			$this->update($row->RecordID, "LaatsteControle", date("Y-m-d H:i:s"));
			$rv++;
		}

		return $rv;

	}  # controle
	
	public function rowset($p_elid=-1, $p_waarde="") {
		global $dbc;
		
		if ($p_elid > 0) {
			$this->elid = $p_elid;
		}
		if (strlen($p_waarde) == 0) {
			$p_waarde = $this->default_waarde_params;
		}
		$this->query = $this->mysql($this->elid);
		
		if (strlen($p_waarde) > 0) {
			$arrpw = explode(";", $p_waarde);
			for ($i=0;$i<count($arrpw);$i++) {
				$pn = sprintf("@P%d", $i);
				$pw = $arrpw[$i];
				$pw = str_replace("@vandaag", date("Y-m-d"), $pw);
				$pw = str_replace("@nu", date("Y-m-d H:i:s"), $pw);
				$this->query = str_replace($pn, $pw, $this->query);
			}
		}
		
		if (strlen($this->query) < 9) {
			debug("Er is geen geldige SQL-code, vraag de webmaster dit probleem te verhelpen.", 1, 1);
			return false;
		} else {
		
			$result = $dbc->prepare($this->query);
			try {
				$result->execute();
				$this->sqlerror = "";
			
			} catch (Exception $e) {
				$this->sqlerror = $e->getMessage();
			}
		
			if (strlen($this->sqlerror) == 0) {
				$rows = $result->fetchAll();
				if (strlen($p_waarde) == 0) {
					$this->update($this->elid, "AantalRecords", count($rows));
				}
				return $rows;
			
			} else {
				debug($this->sqlerror . ". Vraag de webmaster dit te verhelpen.", 1, 1);
				return false;
			}
		}
	}  #  rowset

	public function kolomlidid($p_elid=-1) {
		if ($p_elid > 0) {
			$this->elid = $p_elid;
		}
		$query = sprintf("SELECT IFNULL(EL.KolomLidID, 0) FROM %s WHERE EL.RecordID=%d;", $this->basefrom, $this->elid);
		return $this->scalar($query);
	}
	
	public function record($p_elid=-1) {
		if ($p_elid > 0) {
			$this->elid = $p_elid; 
		}
		$query = sprintf("SELECT EL.* FROM %s WHERE EL.RecordID=%d;", $this->basefrom, $this->elid);
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
	public function add() {
		
		if ($_SESSION['webmaster'] == 1)  {
			$query = sprintf("INSERT INTO %s (RecordID, Naam, MySQL, AantalRecords, Ingevoerd, IngevoerdDoor, GewijzigdDoor) VALUES (%d, '*** Nieuwe lijst ***', '', 0, NOW(), %d, 0);", $this->table, $this->nieuwrecordid(), $_SESSION['lidid']);
			$elid = $this->execsql($query);
		} else {
			$elid = 0;
		}
		return $elid;
		
	}
	
	public function update($p_elid, $p_kolom, $p_waarde, $p_reden="") {
		$this->tas = 2;
		$this->vulvars($p_elid);
		
		if ($p_kolom == "Naam") {
			$p_waarde = str_replace("/", "_", $p_waarde);
		
		} elseif ($p_kolom == "MySQL") {
			$p_waarde = str_ireplace("[%LIDNAAM%]", $this->selectnaam, $p_waarde);
			$p_waarde = str_ireplace("[%TELEFOON%]", $this->selecttelefoon, $p_waarde);
			$p_waarde = str_ireplace("[%EMAIL%]", $this->selectemail, $p_waarde);
			$p_waarde = str_ireplace("[%LEEFTIJD%]", $this->selectleeftijd, $p_waarde);
			
		} elseif ($p_kolom == "Tabpage" and strlen($p_waarde) > 0)	{
			if (substr($p_waarde, -1) == "\\") {
				$p_waarde = substr($p_waard, 0, -1);
			}
			$i_auth = new cls_Authorisation();
			
			if ($i_auth->recordid($p_waarde) == 0) {
				$this->mess = sprintf("Tabpagina '%s' bestaat niet en kan dus niet aan een eigen lijst worden gekoppeld.", $p_waarde);
				$p_waarde = "";
				$this->log($p_elid, 1);
			}
			
			$i_auth = null;
		}

		if ($this->pdoupdate($this->elid, $p_kolom, $p_waarde) > 0) {
			if (strlen($p_reden) > 0) {
				$this->mess .= ", omdat " . $p_reden;
			}
			if ($p_kolom != "LaatsteControle") {
				$this->log($p_elid);
			}
		}
	}
	
	public function delete ($p_elid) {
		$this->vulvars($p_elid);
		$this->pdodelete($p_elid);
	}
	
} # cls_Eigen_lijst

class cls_Foto extends cls_db_base {
	
	private $ftid = 0;
	public $laatstgewijzigd = "";
	
	function __construct($p_ftid=-1) {
		$this->table = TABLE_PREFIX . "Foto";
		$this->basefrom = $this->table . " AS Foto";
		$this->ta = 6;
		$this->vulvars($p_ftid);
		$laatstgewijzigd = date("Y-m-d H:i:s");
	}
	
	private function vulvars($p_ftid) {
		if ($p_ftid >= 0) {
			$this->ftid = $p_ftid;
		}
		if ($this->ftid > 0) {
			$query = sprintf("SELECT Foto.* FROM %s WHERE Foto.RecordID=%d;", $this->frombase, $this->ftid);
			$result = $this->execsql($query);
			$frow = $result->fetch();
			$lidid = $frow->LidID;
		}
	}
	
	public function fotolid($p_lidid) {
		$query = sprintf("SELECT Foto.* FROM %s WHERE Foto.LidID=%d ORDER BY Ingevoerd DESC;", $this->basefrom, $p_lidid);
		$fr = $this->execsql($query)->fetch();
		
		if (isset($fr->RecordID)) {
			$this->laatstgewijzigd = $fr->FotoGewijzigd;
			return "data:image/jpg;charset=utf8;base64," . base64_encode($fr->FotoData);
		} else {
			return false;
		}
	}
	
	public function aantalwijzigingen($p_lidid, $p_duur=30) {
		$query = sprintf("SELECT IFNULL(COUNT(*), 0) FROM %s WHERE LidID=%d AND Ingevoerd > DATE_SUB(NOW(), INTERVAL %d MINUTE);", $this->table, $p_lidid, $p_duur);
		return $this->scalar($query);
	}
	
	public function add($p_file, $p_lidid, $p_ext) {
		$this->lidid = $p_lidid;
		$this->tas = 21;
		$nrid = 0;
		
		$maxwidth = 390;
		$maxheight = 500;
		
		if (file_exists($p_file)) {
			
			if ($p_ext == "jpg") {
				$gdimg = imagecreatefromjpeg($p_file);
			} elseif ($p_ext == "gif") {
				$gdimg = imagecreatefromgif($p_file);
			} elseif ($p_ext == "png") {
				$gdimg = imagecreatefrompng($p_file);
			} else {
				$gdimg = null;
			}
						
			if (imagesx($gdimg) > $maxwidth) {
				$scale = $maxwidth / imagesx($gdimg);
				if ($scale < 1) {
					$gdimg = imagescale($gdimg, imagesx($gdimg) * $scale);
				}
			}
			
			if (imagesy($gdimg) > $maxheight) {
				$scale = $maxheight / imagesy($gdimg);
				if ($scale < 1) {
					$gdimg = imagescale($gdimg, imagesx($gdimg) * $scale);
				}
			}

			ob_start();
			if ($p_ext == "jpg") {
				imagejpeg($gdimg);
			} elseif ($p_ext == "png") {
				imagepng($gdimg);
			} elseif ($p_ext == "gif") {
				imagegif($gdimg);
			}
			$imageData = addslashes(ob_get_contents());
			ob_end_clean();
			
			if ($p_ext == "jpg" and function_exists("exif_read_data")) {
				$exif = exif_read_data($p_file, 0, true);
				if ($exif === false) {
					$laatstgewijzigd = date("Y-m-d H:i:s", filectime($p_file));
				} else {
					$laatstgewijzigd = date("Y-m-d H:i:s", $exif['FILE']['FileDateTime']);
				}
			} else {
				$laatstgewijzigd = date("Y-m-d H:i:s", filectime($p_file));
			}
			
			$nrid = $this->nieuwrecordid();
			$this->query = sprintf("INSERT INTO %s (RecordID, FotoData, LidID, FotoGewijzigd) VALUES (%d, '%s', %d, '%s');", $this->table, $nrid, $imageData, $this->lidid, $laatstgewijzigd);
			if ($this->execsql()) {
				$this->mess = sprintf("Nieuwe pasfoto voor lid %d in de tabel gezet.", $this->lidid);
				$this->Log($nrid);
			} else {
				$nrid = 0;
			}

		} else {
			$this->mess = sprintf("Bestand '%s' bestaat niet, pasfoto voor lid %d is niet toegevoegd.", $p_file, $this->lidid);
			$this->Log();
		}
		return $nrid;
	}
	
	public function update($p_ftid, $p_kolom, $p_waarde) {
		$this->vulvars($p_ftid);
		$this->tas = 22;
		
		if ($this->pdoupdate($p_ftid, $p_kolom, $p_waarde) > 0) {
			$this->log($p_ftid);
		}
	}
	
	private function delete($p_ftid, $p_reden="") {
		$this->vulvars($p_ftid);
		$this->tas = 23;
		
		if ($this->pdodelete($this->ftid, $p_reden)) {
			$this->Log($this->ftid);
		}
	}
	
	public function opschonen() {
		$i_lm = new cls_Lidmaatschap();
		$hd = date("Y-m-d", strtotime("-3 month"));
		$reden = "de persoon langer dan 3 maanden geen lid meer is.";
		
		$frows = $this->basislijst();
		foreach ($frows as $frow) {
			if ($i_lm->islid($frow->LidID) and $i_lm->eindelidmaatschap($frow->LidID) < $hd and $i_lm->soortlid($frow->LidID) != "Kloslid") {
				$this->delete($this->ftid);
			}
		}
		
	}
	
}  # cls_Foto

class cls_dms extends cls_db_base {
}  # cls_dms

class cls_Parameter extends cls_db_base {
	
	private $arrParam;
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Admin_param";
		$this->basefrom = $this->table . " AS Param";
		$this->ta = 13;
		
		// Instellingen ledenadministratie
		
		$this->arrParam['agenda_url_feestdagen'] = array("Type" => "T", "Default" => 'https://calendar.google.com/calendar/ical/nl.dutch%23holiday%40group.v.calendar.google.com/public/basic.ics');
		$this->arrParam['agenda_verjaardagen'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['jubileumjaren'] = array("Type" => "T", "Default" => "12.5;25;50");
		$this->arrParam['kaderjubileumjaren'] = array("Type" => "T", "Default" => "5;12.5;25;50");
		$this->arrParam['kaderonderdeelid'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['mailingbijadreswijziging'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['meisjesnaamtonen'] = array("Type" => "B", "Default" => 1);
		$this->arrParam['menu_met_afdelingen'] = array("Type" => "T");
		$this->arrParam['liddipl_bewaartermijn'] = array("Type" => "I", "Default" => 84);
		$this->arrParam['muteerbarememos'] = array("Type" => "T", "Default" => "D,G");
		$this->arrParam['naamvereniging'] = array("Type" => "T");
		$this->arrParam['naamvereniging_afkorting'] = array("Type" => "T");
		$this->arrParam['rekening_groep_betaalddoor'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['rekening_bewaartermijn'] = array("Type" => "I", "Default" => 84);
		$this->arrParam['toonpasfotoindiennietingelogd'] = array("Type" => "B", "Default" => 0);
		$this->arrParam['uitleg_toestemmingen'] = array("Type" => "T");
		$this->arrParam['verjaardagenvooruit'] = array("Type" => "I", "Default" => 5);
		$this->arrParam['interface_access_db'] = array("Type" => "B", "Default" => 1);
		
		$this->arrParam['zs_incl_beroep'] = array("Type" => "B", "Default" => 1);
		$this->arrParam['zs_incl_vogafgegeven'] = array("Type" => "B", "Default" => 0);
		$this->arrParam['zs_incl_bsn'] = array("Type" => "B", "Default" => 0);
		$this->arrParam['zs_incl_emailouders'] = array("Type" => "B", "Default" => 0);
		$this->arrParam['zs_incl_emailvereniging'] = array("Type" => "B", "Default" => 0);
		$this->arrParam['zs_incl_iban'] = array("Type" => "B", "Default" => 0);
		$this->arrParam['zs_incl_legitimatie'] = array("Type" => "B", "Default" => 1);
		$this->arrParam['zs_incl_machtiging'] = array("Type" => "B", "Default" => 0);
		$this->arrParam['zs_incl_slid'] = array("Type" => "B", "Default" => 0);
		$this->arrParam['zs_muteerbarememos'] = array("Type" => "T");
		$this->arrParam['zs_opzeggingautomatischverwerken'] = array("Type" => "B", "Default" => 0);
		$this->arrParam['zs_opzegtermijn'] = array("Type" => "I", "Default" => 1);
		$this->arrParam['zs_voorwaardenbestelling'] = array("Type" => "T");
		$this->arrParam['zs_voorwaardeninschrijving'] = array("Type" => "T");
		// ***
		
		$this->arrParam['kaderoverzichtmetfoto'] = array("Type" => "B", "Default" => 1);
		$this->arrParam['toneninschrijvingenbewakingen'] = array("Type" => "B", "Default" => 1);
		$this->arrParam['tonentoekomstigebewakingen'] = array("Type" => "B", "Default" => 0);
		
		$this->arrParam['performance_trage_select'] = array("Type" => "F", "Default" => 0.5);
		
		$this->arrParam['logboek_bewaartijd'] = array("Type" => "I", "Default" => 6);
		$this->arrParam['login_geldigheidactivatie'] = array("Type" => "I", "Default" => 36);
		$this->arrParam['db_backup_type'] = array("Type" => "I", "Default" => 3);
		$this->arrParam['db_backupsopschonen'] = array("Type" => "I", "Default" => 11);
		$this->arrParam['login_autounlock'] = array("Type" => "I", "Default" => 120);
		
		$this->arrParam['mailing_alle_zien'] = array("Type" => "I", "Default" => -1);
		$this->arrParam['mailing_bevestigingbestelling'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['mailing_bevestigingopzegging'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['mailing_bevestigingdeelnameevenement'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['mailing_bewaartijd'] = array("Type" => "I", "Default" => 3);
		$this->arrParam['mailing_bewaartijd_ontvangers'] = array("Type" => "I", "Default" => 18);
		$this->arrParam['mailing_bewakinginschrijving'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['mailing_direct_verzenden'] = array("Type" => "B", "Default" => 0);
		$this->arrParam['mailing_extensies_toegestaan'] = array("Type" => "T", "Default" => "bmp, gif, jpeg, jpg, pdf, png, pps, rar, txt, zip");
		$this->arrParam['mailing_herstellenwachtwoord'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['mailing_hist_opschonen'] = array("Type" => "I", "Default" => 6);
		$this->arrParam['mailing_verzonden_opschonen'] = array("Type" => "I", "Default" => 84);
		$this->arrParam['mailing_meldingnieuwip'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['mailing_lidnr'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['mailing_mailopnieuw'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['mailing_rekening_stuurnaar'] = array("Type" => "I", "Default" => 1);
		$this->arrParam['mailing_rekening_valuta'] = array("Type" => "T", "Default" => "&euro&nbsp;");
		$this->arrParam['mailing_rekening_vanafid'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['mailing_rekening_zichtbaarvoor'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['mailing_sentoutbox_auto'] = array("Type" => "B", "Default" => 1);
		$this->arrParam['mailing_tinymce_apikey'] = array("Type" => "T", "Default" => "");
		$this->arrParam['mailing_type_editor'] = array("Type" => "I", "Default" => 1);
		$this->arrParam['mailing_validatielogin'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['mailing_wachttijdinoutbox'] = array("Type" => "I", "Default" => 15);
		$this->arrParam['max_grootte_bijlage'] = array("Type" => "I", "Default" => 2048);
		$this->arrParam['maxmailsperuur'] = array("Type" => "I", "Default" => 150);
		$this->arrParam['maxmailsperdag'] = array("Type" => "I", "Default" => 495);
		$this->arrParam['maxmailsperminuut'] = array("Type" => "I", "Default" => 360);
		
		$this->arrParam['login_bewaartijd'] = array("Type" => "I", "Default" => 6);
		$this->arrParam['login_bewaartijdnietgebruikt'] = array("Type" => "I", "Default" => 21);
		$this->arrParam['login_maxlengte'] = array("Type" => "I", "Default" => 12);
		$this->arrParam['wachtwoord_minlengte'] = array("Type" => "I", "Default" => 7);
		$this->arrParam['wachtwoord_maxlengte'] = array("Type" => "I", "Default" => 12);
		$this->arrParam['login_maxinlogpogingen'] = array("Type" => "I", "Default" => 4);
		$this->arrParam['termijnvervallendiplomasmailen'] = array("Type" => "I", "Default" => 3);
		$this->arrParam['termijnvervallendiplomasmelden'] = array("Type" => "I", "Default" => 6);
		$this->arrParam['verjaardagenaantal'] = array("Type" => "I", "Default" => 7);
		$this->arrParam['versie'] = array("Type" => "I");
		
		$this->arrParam['db_folderbackup'] = array("Type" => "T");
		$this->arrParam['emailwebmaster'] = array("Type" => "T");
		$this->arrParam['login_beperkttotgroep'] = array("Type" => "T");
		$this->arrParam['naamwebsite'] = array("Type" => "T", "Default" => "Naam website");
		$this->arrParam['path_attachments'] = array("Type" => "T");
		$this->arrParam['path_pasfoto'] = array("Type" => "T");
		$this->arrParam['path_templates'] = array("Type" => "T");
		$this->arrParam['title_head_html'] = array("Type" => "T", "");
		$this->arrParam['urlvereniging'] = array("Type" => "T");
		$this->arrParam['url_eigen_help'] = array("Type" => "T");	
	}
	
	public function lijst() {
		$query = sprintf("SELECT *, IFNULL(ValueNum, ValueChar) AS CurVal FROM %sAdmin_param ORDER BY Naam;", TABLE_PREFIX);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function controle() {
		foreach ($this->lijst() as $row) {
			if (isset($this->arrParam[$row->Naam]) == false) {
				$delqry = sprintf("DELETE FROM %s WHERE Naam='%s';", $this->table, $row->Naam);
				if ($this->execsql($delqry) > 0) {
					$this->mess = sprintf("Parameter '%s' is verwijderd.", $row->Naam);
					$this->log($row->RecordID);
				}
			}
		}
		
		foreach ($this->arrParam as $naam => $val) {
			$query = sprintf("SELECT COUNT(*) FROM %s WHERE Naam='%s';", $this->table, $naam);
			if ($this->scalar($query) == 0) {
				$this->add($naam, $val['Type']);
				if (isset($val['Default'])) {
					$this->update($naam, $val['Default']);
				}
			}
			$query = sprintf("SELECT * FROM %s WHERE Naam='%s' AND IFNULL(ParamType, '')<>'%s';", $this->table, $naam, $val['Type']);
			foreach ($this->execsql($query) as $row) {
				$updqry = sprintf("UPDATE %s SET ParamType='%s' WHERE RecordID=%d;", $this->table, $val['Type'], $row->RecordID);
				if ($this->execsql($updqry) > 0) {
					$this->mess = sprintf("Van parameter '%s' is het type in '%s' gewijzigd.", $naam, $val['Type']);
					$this->log($row->RecordID);
				}
			}
		}
	}
	
	public function vulsessie() {
		foreach ($this->lijst() as $row) {
			if (($row->ParamType == "F" or $row->ParamType == "I") and empty($row->ValueNum)) {
				$_SESSION['settings'][$row->Naam] = 0;
			} elseif ($row->ParamType == "F") {
				$_SESSION['settings'][$row->Naam] = $row->ValueNum;
			} elseif ($row->ParamType == "B" or $row->ParamType == "I") {
				$_SESSION['settings'][$row->Naam] = intval($row->ValueNum);
			} elseif (empty($row->ValueChar)) {
				$_SESSION['settings'][$row->Naam] = "";
			} else {
				$_SESSION['settings'][$row->Naam] = $row->ValueChar;
			}
		}
	}
	
	public function add($p_naam, $p_type) {
		$nrid = $this->nieuwrecordid();
		
		$query = sprintf("INSERT INTO %s (RecordID, Naam, ParamType, IngevoerdDoor) VALUES (%d, '%s', '%s', %d);", $this->table, $nrid, $p_naam, $p_type, $_SESSION['lidid']);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Parameter '%s', type '%s', is toegevoegd.", $p_naam, $p_type);
			$this->log($nrid);
		}
	}
	
	public function update($p_naam, $p_waarde, $p_reden="") {
		if (in_array($p_naam, array("db_folderbackup", "login_beperkttotgroep", "muteerbarememos", "menu_met_afdelingen", "path_attachments", "path_pasfoto", "path_templates", "urlvereniging", "url_eigen_help", "zs_muteerbarememos"))) {
			$p_waarde = str_replace(" ", "", $p_waarde);
		}
		if (in_array($p_naam, array("login_beperkttotgroep", "muteerbarememos", "menu_met_afdelingen", "zs_muteerbarememos"))) {
			$p_waarde = str_replace("'", "", $p_waarde);
			$p_waarde = str_replace("\"", "", $p_waarde);
		}
		$query = sprintf("SELECT * FROM %s WHERE Naam='%s';", $this->table, $p_naam);
		$cur = $this->execsql($query)->fetch();
		$set = "";
		if ($cur->ParamType == "B" or $cur->ParamType == "I") {
			if (strlen($p_waarde) == 0) {
				$p_waarde = 0;
			} else {
				$p_waarde = intval($p_waarde);
			}
			$set = sprintf("ValueNum=%d", $p_waarde);
			$xw = sprintf("IFNULL(ValueNum, 0)<>%d", $p_waarde);
		} elseif ($cur->ParamType == "F") {
			$p_waarde = sprintf("%F", $p_waarde);
			$set = sprintf("ValueNum=%.6F", $p_waarde);
			$xw = sprintf("IFNULL(ValueNum, 0)<>%.6F", $p_waarde);
		} elseif ($cur->ParamType == "T") {
			if (strlen($p_waarde) == 0) {
				$set = "ValueChar=NULL, ValueNum=NULL";
				$xw = "(ValueChar IS NOT NULL)";
			} else {
				$p_waarde = "\"" . str_replace("\"", "'", $p_waarde) . "\"";
				$set = sprintf("ValueChar=%s, ValueNum=NULL", $p_waarde);
				$xw = sprintf("IFNULL(ValueChar, '')<>%s", $p_waarde);
			}
		} else {
			$mess = sprintf("ParamType %s van %s is onbekend.", $cur->ParamType, $p_naam);
			debug($mess);
		}
		if (strlen($set) > 0) {
			$query = sprintf("UPDATE %s SET %s, GewijzigdDoor=%d WHERE RecordID=%d AND %s;", $this->table, $set, $_SESSION['lidid'], $cur->RecordID, $xw);
			if ($this->execsql($query) > 0) {
				if (strlen($p_reden) > 0) {
					$this->mess = sprintf("Parameter '%s' is in '%s' gewijzigd, omdat %s.", $p_naam, $p_waarde, $p_reden);
				} else {
					$this->mess = sprintf("Parameter '%s' is in '%s' gewijzigd.", $p_naam, $p_waarde);
				}
				$this->log($cur->RecordID);
			}
			$_SESSION['settings'][$p_naam] = str_replace("\"", "", $p_waarde);
		}
	}
	
}  # cls_Parameter

function db_delete_local_tables() {
	global $arrTables;
	
	foreach ($arrTables as $key => $tn) {
		if ($key >= 20) {
			(new cls_db_base())->execsql(sprintf("DELETE FROM %s%s;", TABLE_PREFIX, $tn), 2);
		}
	}
}

function db_onderhoud($type=9) {
	/* Type uitleg
		1 = na upload
		2 = zonder optimize tables
	*/
	global $arrTables, $db_name, $wherelid, $wherelidond, $lididwebmasters;
	
	(new cls_interface())->opschonen();
	(new cls_Onderdeel())->controle();
	
	$i_base = new cls_db_base();
	
	// Vaste aanpassingen aan de database na een upload.
	$i_base->execsql(sprintf("ALTER TABLE %sBewseiz CHANGE Begindatum Begindatum DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sBewseiz CHANGE `Einde` `Einde` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sBewseiz CHANGE `Geboren` `Geboren` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sDiploma CHANGE `Vervallen` `Vervallen` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sDiploma CHANGE `EindeUitgifte` `EindeUitgifte` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sFunctie CHANGE `Vervallen per` `Vervallen per` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sLid CHANGE `GEBDATUM` `GEBDATUM` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sLid CHANGE `Overleden` `Overleden` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sLid CHANGE `VOG afgegeven` `VOG afgegeven` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sLid CHANGE `Verwijderd` `Verwijderd` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sLiddipl CHANGE `DatumBehaald` `DatumBehaald` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sLiddipl CHANGE `LicentieVervallenPer` `LicentieVervallenPer` DATE DEFAULT NULL;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sLidmaatschap CHANGE `LIDDATUM` `LIDDATUM` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sLidmaatschap CHANGE `Opgezegd` `Opgezegd` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sLidond CHANGE `Vanaf` `Vanaf` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sLidond CHANGE `Opgezegd` `Opgezegd` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sMutatie CHANGE `Datum` `Datum` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sOnderdl CHANGE `VervallenPer` `VervallenPer` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sRekening CHANGE `Datum` `Datum` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sSeizoen CHANGE `Begindatum` `Begindatum` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sSeizoen CHANGE `Einddatum` `Einddatum` DATE;", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE %sLid CHANGE `BezwaarMachtiging` `BezwaarMachtiging` DATE NULL DEFAULT NULL;", TABLE_PREFIX));
	
	$i_base->execsql(sprintf("ALTER TABLE `%sLid` CHANGE `Geslacht` `Geslacht` CHAR(1) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT 'O';", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE `%sLid` CHANGE `Huisletter` `Huisletter` VARCHAR(2) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT '';", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE `%sLid` CHANGE `Toevoeging` `Toevoeging` VARCHAR(5) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT '';", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE `%sLid` CHANGE `Buitenland` `Buitenland` TINYINT(4) NULL DEFAULT '0';", TABLE_PREFIX));
	$i_base->execsql(sprintf("ALTER TABLE `%sLid` CHANGE `Legitimatietype` `Legitimatietype` CHAR(1) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT 'G';", TABLE_PREFIX));

	$query = sprintf("ALTER TABLE `%sMemo` CHANGE `Vertrouwelijk` `Vertrouwelijk` TINYINT(4) NULL DEFAULT '0'", TABLE_PREFIX);
	(new cls_db_base())->execsql($query);

	/***** Aanpassen lengte login in de database als deze kleiner is dan login_maxlengte  *****/
	$table = TABLE_PREFIX . "Admin_login";
	$col = "Login";
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE '%s';", DB_NAME, $table, $col);
	if ($i_base->scalar($query) < $_SESSION['settings']['login_maxlengte']) {
		$query = sprintf("ALTER TABLE `%1\$s` CHANGE `%2\$s` `%2\$s` VARCHAR( %3\$d ) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table, $col, $_SESSION['settings']['login_maxlengte']);
		(new cls_db_base())->execsql($query, 2);
	}
	
	/***** Aanpassen lengte wachtwoord in de database als deze kleiner is dan 255. Deze lengte is nodig voor de password_hash()  *****/
	$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE 'Wachtwoord';", DB_NAME, $table);
	if ($i_base->scalar($query) < 255) {
		$query = sprintf("ALTER TABLE `%s` CHANGE `Wachtwoord` `Wachtwoord` VARCHAR(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $table);
		(new cls_db_base())->execsql($query, 2);
	}
	
	/***** Kolommen/indexen die later zijn toegevoegd. *****/
	
	// Deze code kan na 1 mei 2023 worden verwijderd.
	$tab = TABLE_PREFIX . "Eigen_lijst";
	$col = "Default_value_params";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(100) NULL AFTER `MySQL`;", $tab, $col);
		$i_base->execsql($query, 2);	
	}
	
	$tab = TABLE_PREFIX . "Eigen_lijst";
	$col = "Aantal_params";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` TINYINT NOT NULL DEFAULT '0' AFTER `MySQL`;", $tab, $col);
		$i_base->execsql($query, 2);	
	}
	
	$tab = TABLE_PREFIX . "Mailing_rcpt";
	$col = "Rcpt_Type";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` CHAR(1) NOT NULL DEFAULT 'T' AFTER `to_address`;", $tab, $col);
		$i_base->execsql($query, 2);	
	}
	
	// Deze code kan na 1 juni 2023 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing";
	$col = "GroepOntvangers";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NOT NULL DEFAULT '0' AFTER `NietVersturenVoor`;", $tab, $col);
		$i_base->execsql($query, 2);	
	}
	
	$tab = TABLE_PREFIX . "Foto";
	$col = "FotoGewijzigd";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP AFTER `Type`;", $tab, $col);
		$i_base->execsql($query, 2);	
	}

	$tab = TABLE_PREFIX . "Eigen_lijst";
	$col = "NaamEersteKolom";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(25) NULL AFTER `KolomLidID`;", $tab, $col);
		$i_base->execsql($query, 2);	
	}
	
	$tab = TABLE_PREFIX . "Mailing";
	$col = "Opmerking";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(75) NULL AFTER `message`;", $tab, $col);
		$i_base->execsql($query, 2);	
	}
	
	// Deze code kan na 1 juli 2023 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing_rcpt";
	$col = "Xtra_Char";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` CHAR(5) NULL DEFAULT NULL after `Rcpt_Type`;", $tab, $col);
		$i_base->execsql($query, 2);	
	}
	
	$tab = TABLE_PREFIX . "Mailing_rcpt";
	$col = "Xtra_Num";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT(11) NOT NULL DEFAULT 0 after `Xtra_Char`;", $tab, $col);
		$i_base->execsql($query, 2);	
	}

	$tab = TABLE_PREFIX . "Mailing_hist";
	$col = "NietVersturenVoor";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` DATETIME NULL DEFAULT NULL after `send_on`;", $tab, $col);
		$i_base->execsql($query, 2);	
	}
	
	// Deze code kan na 1 september 2023 worden verwijderd.
	$tab = TABLE_PREFIX . "Eigen_lijst";
	$col = "Tabpage";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(75) NULL AFTER `AantalRecords`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Evenement_Type";
	$col = "Achtergrondkleur";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(12) NULL AFTER `Soort`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Evenement_Type";
	$col = "Tekstkleur";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(12) NULL AFTER `Soort`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Evenement_Type";
	$col = "Vet";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` TINYINT NOT NULL DEFAULT '0' AFTER `Tekstkleur`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Evenement_Type";
	$col = "Cursief";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` TINYINT NOT NULL DEFAULT '0' AFTER `Vet`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	// Deze code kan na 1 oktober 2023 worden verwijderd.
	$tab = TABLE_PREFIX . "Eigen_lijst";
	$col = "LaatsteControle";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP AFTER `Tabpage`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	// Deze code kan na 1 november 2023 worden verwijderd.
	$tab = TABLE_PREFIX . "Eigen_lijst";
	$col = "EigenScript";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(30) NULL AFTER `MySQL`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Admin_activiteit";
	$col = "refColumn";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(40) NULL AFTER `RefTable`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	// Deze code kan na 1 januari 2024 worden verwijderd.
	$tab = TABLE_PREFIX . "Onderdl";
	$col = "LedenMuterenDoor";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NULL AFTER `ORGANIS`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	
	/***** Velden die aangepast zijn *****/
	
	// Deze code mag na 1 november 2023 worden verwijderd.
	$query = sprintf("ALTER TABLE `%sEigen_lijst` CHANGE `Naam` `Naam` VARCHAR(40) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sAdmin_access` CHANGE `LaatstGebruikt` `LaatstGebruikt` DATE NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sAdmin_activiteit` CHANGE `refColumn` `refColumn` VARCHAR(40) NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sSeizoen` CHANGE `Verenigingscontributie kostenplaats` `Verenigingscontributie kostenplaats` VARCHAR(12) NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sSeizoen` CHANGE `Gezinskorting kostenplaats` `Gezinskorting kostenplaats` VARCHAR(12) NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	// Deze code mag na 1 januari 2024 worden verwijderd.
	$query = sprintf("ALTER TABLE `%sAdmin_login` CHANGE `Gewijzigd` `Gewijzigd` DATETIME NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	if ($i_base->bestaat_kolom("IngevoerdOp", "Stukken") == true) {
		$query = sprintf("ALTER TABLE `%sStukken` CHANGE `IngevoerdOp` `Ingevoerd` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
		$i_base->execsql($query);
	}
		
	foreach ($arrTables as $key => $tn) {
		if ($tn !== "Admin_activiteit") {
			$i_base->execsql(sprintf("ALTER TABLE %s%s CHANGE `Ingevoerd` `Ingevoerd` DATETIME NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX, $tn));
		}
	}
	
	/***** Velden die niet meer nodig zijn *****/
	
	// Deze code kan pas verwijderd worden als de kolom ook uit de Access-database is verwijderd.
	$tab = TABLE_PREFIX . "Lid";
	$col = "Nummer";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);	
	}
	
	// Deze code mag na 1 december 2022 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing";
	$col = "confidential";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);	
	}
	
	$tab = TABLE_PREFIX . "Mailing";
	$col = "verzamelen";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);	
	}
		
	$tab = TABLE_PREFIX . "Mailing";
	$col = "sent_on";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Mailing_hist";
	$col = "send_mt";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);	
	}

	/***** Opschonen database na een upload uit de Access-database.  *****/

	if ($type == 1) {		
		$query = sprintf("DELETE FROM %sGBR WHERE Balans=1;", TABLE_PREFIX);
		$i_base->execsql($query, 2);
		$query = sprintf('DELETE FROM %1$sMutatie WHERE GBR NOT IN (SELECT Kode FROM %1$sGBR);', TABLE_PREFIX);
		$i_base->execsql($query, 2);
	} else {
		(new cls_Eigen_lijst())->controle();
	}
	
	if ($type != 2) {
		foreach ($arrTables as $tn) {
			$i_base->execsql(sprintf("OPTIMIZE TABLE %s%s;", TABLE_PREFIX, $tn));
		}
	}
	
	if ($_SERVER["HTTP_HOST"] != "phprbm.telling.nl") {
		//  Deze tabellen moeten op dit moment alleen in de ontwikkelomgeving aanwezig zijn. 
		$query = sprintf("DROP TABLE IF EXISTS %sDMS_Folder;", TABLE_PREFIX);
		$i_base->execsql($query);
		$query = sprintf("DROP TABLE IF EXISTS %sDMS_Document;", TABLE_PREFIX);
		$i_base->execsql($query);
	}
}  # db_onderhoud

function db_backup($p_typebackup=3) {
	global $dbc, $db_name, $arrTables;
	
	$rv = false;
	
	$db_folderbackup = $_SESSION['settings']['db_folderbackup'];
	
	$buname = "backup_" . $p_typebackup . "_" . Date("Y-m-d-H-i-s") . "-" . rand(10001, 99998);
	
	if (strlen($db_folderbackup) < 5) {
		$db_folderbackup = str_replace($_SERVER['PHP_SELF'], "", $_SERVER["SCRIPT_FILENAME"]) . "/backups/";
		if (!file_exists($db_folderbackup)) {
			if (mkdir($db_folderbackup, 0755, true) == true) {
				$ret = sprintf("Directory '%s' is aangemaakt.", $db_folderbackup);
				(new cls_Logboek())->add($ret, 2, 0, 1);
			}
		}
	} elseif (substr($db_folderbackup, -1) != "/") {
		$db_folderbackup .= "/";
	}
	(new cls_Parameter())->update("db_folderbackup", $db_folderbackup);

	$mess = "";
	
	$laatstebackup = (new cls_Logboek())->max("DatumTijd", "TypeActiviteit=3 AND TypeActiviteitSpecifiek=1");
	if (strtotime($laatstebackup) < mktime(date("H")-1, date("m"), 0, date("m"), date("d"), date("Y")) or $_SERVER["HTTP_HOST"] == "phprbm.telling.nl") {
		
		
		$FileName = $db_folderbackup . $buname . ".sql";
		
		echo("<p class='mededeling'>Backup is gestart</p>");
		$buf = fopen($FileName, 'w');
				
		$aanttab = 0;
		$i_base = new cls_db_base();
		foreach ($arrTables as $tnr => $tnm) {
			set_time_limit(60);
			$table = TABLE_PREFIX . $tnm;
			$i_base->table = $table;
			$query = sprintf("SELECT COUNT(*) FROM %s;", $table);
			$a = $i_base->scalar($query);
//			debug($table . ": " . $a);
			
			if ($a > 0 and ($p_typebackup == 3 or ($p_typebackup == 1 and $tnr < 30) or ($p_typebackup == 2 and $tnr >= 30))) {
			
				$data = $i_base->exporttosql(2);
				fwrite($buf, $data);

				if ($a > 50000) {
					$query = sprintf("SELECT * FROM `%s` ORDER BY RecordID DESC LIMIT 50000;", $table);
				} else {
					$query = sprintf("SELECT * FROM `%s`;", $table);
				}
				$result = $dbc->prepare($query);
				$result->execute();
				$num_fields = $result->columnCount();
		
				while($row = $result->fetch(PDO::FETCH_NUM)) { 
					
					$data = sprintf("INSERT INTO `%s` VALUES(", $table);
					for($j=0; $j<$num_fields; $j++) {
						$meta = $result->GetColumnMeta($j);
						$row[$j] = addslashes($row[$j]); 
						$row[$j] = str_replace("\n","\\n",$row[$j]);
						if ($meta['native_type'] == "LONG" or $meta['native_type'] == "TINY"  or $meta['native_type'] == "SHORT") {
							if (isset($row[$j]) and strlen($row[$j]) > 0) {
								$data .= $row[$j];
							} else {
								$data .= "0";
							}
							
						} elseif ($meta['native_type'] == "DATE" or $meta['native_type'] == "DATETIME"  or $meta['native_type'] == "TIMESTAMP") {
							if (isset($row[$j]) and strlen($row[$j]) > 0) {
								$data .= "'" . $row[$j] . "'";
							} else {
								$data .= "NULL";
							}
							
						} elseif ($meta['native_type'] == "NEWDECIMAL") {
							if (isset($row[$j]) and strlen($row[$j]) > 0) {
								$data .= $row[$j];
							} else {
								$data .= "0";
							}
							
						} elseif (isset($row[$j])) {
							$data .= '"' . $row[$j] . '"' ;
//							debug($meta['name'] . ": " . $meta['native_type']);
						} else {
							$data .= '""';
						}
						if ($j < ($num_fields-1)) {
							$data .= ",";
						}
					}
					$data .= ");\n";
					
					fwrite($buf, $data);
				}
				$aanttab++;
				
				fwrite($buf, "\n\n");

				$resultcount = null;
				$result = null;
			}
		}
		fclose($buf);
		$i_base = null;

		if ($aanttab > 0) {
		
			$mess = sprintf("Backup %s (%d tabellen) is, in '%s', gemaakt.", ARRTYPEBACKUP[$p_typebackup], $aanttab, str_replace($db_folderbackup, "", $FileName));
			(new cls_Logboek())->add($mess, 3, 0, 1, 0, 1);
			$rv= true;

			if ($handle = opendir($db_folderbackup)) {
				while (false !== ($file = readdir($handle))) {
					sleep(2);
					if ($file != "." and $file != "..") {
						$vbn = $db_folderbackup . $file;
						if ($_SESSION['settings']['db_backupsopschonen'] > 1 and filemtime($vbn) < strtotime(sprintf("-%d days", $_SESSION['settings']['db_backupsopschonen'])) or filesize($db_folderbackup . $file) < 500 ) {
							unlink($vbn);
							$mess = sprintf("Backup-bestand %s is verwijderd. ", $file);
							(new cls_Logboek())->add($mess, 2, 0, 1);
						} elseif (fileperms($vbn) != 32768) {
							if (chmod($vbn, 0000) == true) {
								$mess = sprintf("Op bestand %s is chmod 0000 uitgevoerd.", $file);
							} else {
								$mess = sprintf("chmod 0000 op bestand %s is niet gelukt.", $vbn);
							}
							(new cls_Logboek())->add($mess, 2, 0, 1);
						}
					}
				}
			} else {
				$mess = "";
			}
		} else {
			(new cls_Logboek())->add("Het backup-script heeft gedraaid, maar de backup is niet gelukt.\n", 2, 0, 1, 0, 2);
		}
	} else {
//		(new cls_Logboek())->add("Het backupscript heeft korter dan 1 uur geleden met succes gedraaid. Het wordt niet nogmaals uitgevoerd.", 2, 0, 1, 0, 2);
	}
	return $rv;
}

function fnFreeBackupFiles() {
	
	$i_lb = new cls_Logboek();
	
	$mess = "Start fnFreeBackupFiles";
	$i_lb->add($mess, 3, 0, 1);
	
	$mess = "";
	if (is_dir($_SESSION['settings']['db_folderbackup'])) {
		if ($handle = opendir($_SESSION['settings']['db_folderbackup'])) {
			while (false !== ($file = readdir($handle))) {
				sleep(2);
				if ($file != "." and $file != "..") {
					$vbn = $_SESSION['settings']['db_folderbackup'] . $file;
					if (chmod($vbn, 0755) == true) {
						$mess = sprintf("Op bestand %s is chmod 0755 uitgevoerd.", $file);
					} else {
						$mess = sprintf("chmod 0755 op bestand %s is niet gelukt.", $vbn);
					}
					$i_lb->add($mess, 3, 0, 1);
				}
			}
		}
	} else {
		$mess = "De backup-directory bestaat niet.";
		$i_lb->add($mess, 3, 0, 1);
	}
	
	$i_lb = null;
	
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
/*
		$data .= sprintf("DROP TABLE IF EXISTS `%s`;", $table);

		$result2 = $dbc->prepare('SHOW CREATE TABLE '.$table);  
		$result2->execute();                            
		$row2 = $result2->fetch(PDO::FETCH_NUM);
		$data .= sprintf("\n\n%s;\n\n", $row2[1]);
*/
		
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
		$data .= "\n\n\n";
	}

	return $data;
}

function db_stats($lidid=0) {
	
	$i_base = new cls_db_base();

	$base = sprintf("SELECT COUNT(*) FROM %s WHERE %s", $i_base->fromlid, $i_base->wherelid);
	
	$stats['aantalleden'] = $i_base->scalar($base . ";");
	
	$stats['aantalvrouwen'] = $i_base->scalar($base . " AND L.Geslacht='V';");
	
	$stats['aantalmannen'] = $i_base->scalar($base . " AND L.Geslacht='M';");
	
	$query = sprintf("SELECT ROUND(AVG(DATEDIFF(CURDATE(), L.GEBDATUM))/365.25, 1) FROM %s WHERE %s AND (NOT L.GEBDATUM IS NULL);", $i_base->fromlid, $i_base->wherelid);
	$stats['gemiddeldeleeftijd'] = $i_base->scalar($query);
	
	$query = sprintf("SELECT COUNT(DISTINCT LO.Lid) FROM %s WHERE (O.Kader=1 OR F.Kader=1) AND %s;", $i_base->fromlidond, cls_db_base::$wherelidond);
	$stats['aantalkaderleden'] = $i_base->scalar($query);
	
	$query = sprintf("SELECT %1\$s AS LidNaam FROM %2\$sLid AS L INNER JOIN %2\$sAdmin_login AS Login ON L.RecordID = Login.LidID"
			 . " ORDER BY Login.Ingevoerd DESC LIMIT 1;", $i_base->selectnaam, TABLE_PREFIX);
	$stats['nieuwstelogin'] = $i_base->scalar($query);
	
	$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login AS Login;", TABLE_PREFIX);
	$stats['aantallogins'] = $i_base->scalar($query);
	
	$query = sprintf("SELECT MAX(DatumTijd) FROM %sAdmin_activiteit WHERE TypeActiviteit=9;", TABLE_PREFIX);
	$stats['laatsteupload'] = $i_base->scalar($query);
	
	$query = sprintf("SELECT %1\$s AS LidNaam FROM %2\$sLid AS L INNER JOIN %2\$sAdmin_login AS Login ON L.RecordID = Login.LidID"
			 . " WHERE Login.Ingelogd=1 ORDER BY Login.Ingevoerd DESC;", $i_base->selectnaam, TABLE_PREFIX);
	$result = $i_base->execsql($query);
	$stats['nuingelogd'] = "";
	foreach($result->fetchAll() as $row) {
		if (strlen($stats['nuingelogd']) > 1) { $stats['nuingelogd'] .= ", "; }
		$stats['nuingelogd'] .= $row->LidNaam;
	}
	
	if ($lidid > 0) {
		$query = sprintf("SELECT L.Roepnaam FROM %sLid AS L WHERE L.RecordID=%d;", TABLE_PREFIX, $lidid);
		$stats['roepnaamingelogde'] = $i_base->scalar($query);
		
		$query = sprintf("SELECT L.GEBDATUM FROM %sLid AS L WHERE L.RecordID=%d;", TABLE_PREFIX, $lidid);
		$stats['geboortedatumingelogde'] = $i_base->scalar($query);
		
		$query = sprintf("SELECT L.Gewijzigd FROM %sLid AS L WHERE L.RecordID=%d;", TABLE_PREFIX, $lidid);
		$filter = sprintf(" WHERE Lid=%d", $lidid);
	} else {
		$stats['roepnaamingelogde'] = "gast";
		
		$query = sprintf("SELECT MAX(L.Gewijzigd) FROM %sLid AS L;", TABLE_PREFIX);
		$filter = "";
	}
	$result = $i_base->execsql($query);
	$stats['laatstgewijzigd'] = $result->fetchColumn();
	
	$query = sprintf("SELECT MAX(BW.Gewijzigd) FROM %sBewaking AS BW%s;", TABLE_PREFIX, $filter);
	$result = $i_base->execsql($query);
	$lgw = $result->fetchColumn();
	if ($lgw > $stats['laatstgewijzigd']) {
		$stats['laatstgewijzigd'] = $lgw;
	}
	
	$query = sprintf("SELECT MAX(LO.Gewijzigd) FROM %sLidond AS LO%s;", TABLE_PREFIX, $filter);
	$lgw = $i_base->scalar($query);
	if ($lgw > $stats['laatstgewijzigd']) {
		$stats['laatstgewijzigd'] = $lgw;
	}
	
	if ($lidid > 0) {
		$query = sprintf("SELECT MAX(LD.Gewijzigd) FROM %sLiddipl AS LD%s;", TABLE_PREFIX, $filter);
		$lgw = $i_base->scalar($query);
		if ($lgw > $stats['laatstgewijzigd']) {
			$stats['laatstgewijzigd'] = $lgw;
		}
	}
	
	$query = sprintf("SELECT MAX(RK.Gewijzigd) FROM %sRekening AS RK%s;", TABLE_PREFIX, $filter);
	$lgw = $i_base->scalar($query);
	if ($lgw > $stats['laatstgewijzigd']) {
		$stats['laatstgewijzigd'] = $lgw;
	}	
	
	return $stats;
	
} # db_stats

function db_createtables() {

	$queries = sprintf("SET SQL_MODE = 'NO_AUTO_VALUE_ON_ZERO';
SET AUTOCOMMIT = 0;
START TRANSACTION;

CREATE TABLE IF NOT EXISTS `%1\$sAanwezigheid` (
  `RecordID` int(11) NOT NULL,
  `AfdelingskalenderID` int(11) NOT NULL,
  `LidondID` int(11) NOT NULL,
  `Status` char(1) DEFAULT NULL,
  `Opmerking` varchar(75) DEFAULT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sActiviteit` (
  `RecordID` int(11) NOT NULL,
  `Code` varchar(8) DEFAULT NULL,
  `Omschrijving` varchar(35) DEFAULT NULL,
  `Contributie` decimal(8,2) DEFAULT NULL,
  `Vervallen` datetime DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_access` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Tabpage` varchar(75) NOT NULL,
  `Toegang` int(11) NOT NULL DEFAULT -1,
  `LaatstGebruikt` date DEFAULT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_activiteit` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL COMMENT 'Wie heeft deze activiteit uitgevoerd?',
  `DatumTijd` datetime NOT NULL DEFAULT current_timestamp(),
  `Omschrijving` text NOT NULL,
  `ReferID` int(11) NOT NULL COMMENT 'Op welk RecordID heeft deze activiteit betrekking?',
  `ReferLidID` int(11) NOT NULL DEFAULT 0,
  `Script` varchar(100) NOT NULL,
  `TypeActiviteit` tinyint(4) DEFAULT 0,
  `TypeActiviteitSpecifiek` smallint(6) DEFAULT NULL,
  `IP_adres` varchar(45) NOT NULL,
  `USER_AGENT` varchar(125) DEFAULT NULL,
  `Getoond` tinyint(4) DEFAULT NULL COMMENT 'Is deze melding aan de gebruiker getoond?',
  `RefFunction` varchar(75) DEFAULT NULL,
  `RefTable` varchar(30) DEFAULT NULL,
  `refColumn` varchar(40) DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  KEY `TypeActiviteit` (`TypeActiviteit`),
  KEY `DatumTijd` (`DatumTijd`),
  KEY `LidID` (`LidID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_interface` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) DEFAULT NULL,
  `IP-adres` varchar(45) NOT NULL,
  `SQL-statement` text CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `IngelogdLid` int(11) DEFAULT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `Gedownload` datetime DEFAULT NULL,
  `Afgemeld` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_login` (
  `RecordID` int(11) DEFAULT NULL,
  `LidID` int(11) NOT NULL,
  `Login` varchar(15) NOT NULL,
  `Wachtwoord` varchar(255) NOT NULL,
  `HerinneringVervallenDiplomas` tinyint(4) NOT NULL DEFAULT 0,
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime NOT NULL,
  `GewijzigdDoor` int(11) NOT NULL DEFAULT 0,
  `LastLogin` datetime DEFAULT NULL,
  `LastActivity` datetime DEFAULT NULL,
  `Ingelogd` tinyint(4) NOT NULL DEFAULT 0,
  `FouteLogin` smallint(6) NOT NULL DEFAULT 0,
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
  `ValueChar` text COLLATE utf8_unicode_ci DEFAULT NULL,
  `ValueNum` decimal(16,6) DEFAULT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Naam` (`Naam`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci COMMENT='Parameters en instellingen';

CREATE TABLE IF NOT EXISTS `%1\$sAfdelingskalender` (
  `RecordID` int(11) NOT NULL,
  `OnderdeelID` int(11) NOT NULL,
  `Datum` date NOT NULL DEFAULT '0000-00-00',
  `Omschrijving` varchar(75) DEFAULT NULL,
  `Activiteit` tinyint(4) NOT NULL DEFAULT 1 COMMENT '1 = wel activiteit, 0 = geen activiteit',
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL DEFAULT 0,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sBewaking` (
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
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`SeizoenID`,`Lid`,`BEGIN_PER`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sBewaking_Blok` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `SeizoenID` int(11) NOT NULL,
  `Kode` char(2) DEFAULT NULL,
  `Omschrijving` varchar(40) DEFAULT NULL,
  `Begin` datetime NOT NULL,
  `Eind` datetime DEFAULT NULL,
  `InschrijvingOpen` tinyint(4) NOT NULL DEFAULT 1,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `SeizoenBegindatum` (`SeizoenID`,`Begin`),
  UNIQUE KEY `SeizoenKode` (`SeizoenID`,`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sBewaking_Inschrijving` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Lid` int(11) NOT NULL,
  `Nummer` int(11) NOT NULL DEFAULT 0,
  `BlokID` int(11) NOT NULL,
  `Keuze` tinyint(4) NOT NULL DEFAULT 1,
  `Opmerking` varchar(50) DEFAULT NULL,
  `Definitief` datetime DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `Afgemeld` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sBewseiz` (
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
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sBoekjaar` (
  `RecordID` int(11) NOT NULL DEFAULT 0,
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
  `Kode` varchar(10) DEFAULT NULL,
  `Naam` varchar(75) DEFAULT NULL,
  `Type` varchar(1) DEFAULT NULL,
  `ORGANIS` smallint(6) DEFAULT NULL,
  `MIN_LFT` smallint(6) DEFAULT NULL,
  `Volgnr` smallint(6) DEFAULT NULL,
  `OpleidingInVereniging` tinyint(4) DEFAULT NULL,
  `EXAM_OND` smallint(6) DEFAULT NULL,
  `GELDIGH` int(11) DEFAULT NULL,
  `AantalBeoordelingen` smallint(6) DEFAULT NULL,
  `Vervallen` date DEFAULT NULL,
  `EindeUitgifte` date DEFAULT NULL,
  `HistorieOpschonen` int(11) DEFAULT NULL,
  `VoorgangerID` int(11) DEFAULT NULL,
  `Alternatief` varchar(20) DEFAULT NULL,
  `Tonen in bewakingsadministratie` tinyint(4) DEFAULT NULL,
  `Afdelingsspecifiek` int(11) DEFAULT NULL,
  `Zelfservice` tinyint(4) DEFAULT NULL,
  `RolID` int(11) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sEigen_lijst` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Naam` varchar(40) NOT NULL,
  `MySQL` longtext NOT NULL,
  `EigenScript` varchar(30) DEFAULT NULL,
  `Aantal_params` tinyint(4) NOT NULL DEFAULT 0,
  `Default_value_params` varchar(100) DEFAULT NULL,
  `AantalKolommen` int(11) NOT NULL DEFAULT -1,
  `KolomLidID` int(11) NOT NULL DEFAULT -1,
  `NaamEersteKolom` varchar(25) DEFAULT NULL,
  `AantalRecords` int(11) NOT NULL DEFAULT 0,
  `Tabpage` varchar(75) DEFAULT NULL,
  `LaatsteControle` datetime NOT NULL DEFAULT current_timestamp(),
  `Ingevoerd` datetime NOT NULL,
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) NOT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sEvenement` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Datum` datetime NOT NULL,
  `Eindtijd` varchar(5) DEFAULT NULL,
  `Verzameltijd` varchar(5) DEFAULT NULL,
  `Omschrijving` varchar(50) NOT NULL,
  `Email` varchar(45) DEFAULT NULL,
  `TypeEvenement` int(11) NOT NULL,
  `InschrijvingOpen` tinyint(4) NOT NULL DEFAULT 1,
  `StandaardStatus` char(1) NOT NULL DEFAULT 'I',
  `MaxPersonenPerDeelname` int(11) NOT NULL DEFAULT 1,
  `BeperkTotGroep` int(11) NOT NULL DEFAULT 0 COMMENT 'Welke groep mag zich voor dit evenement inschrijven? 0 = iedereen.',
  `MeerdereStartMomenten` tinyint(4) NOT NULL DEFAULT 0,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL DEFAULT 0,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) DEFAULT NULL,
  `VerwijderdOp` date DEFAULT NULL,
  `Locatie` varchar(75) DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sExamen` (
  `Nummer` int(11) NOT NULL,
  `Datum` date DEFAULT NULL,
  `Plaats` varchar(30) DEFAULT NULL,
  `Begintijd` varchar(5) DEFAULT NULL,
  `Eindtijd` varchar(5) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`),
  UNIQUE KEY `DatumPlaats` (`Datum`,`Plaats`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sEvenement_Deelnemer` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL,
  `EvenementID` int(11) NOT NULL,
  `Functie` varchar(30) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `Opmerking` varchar(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `Status` char(1) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT 'I',
  `StartMoment` time DEFAULT NULL,
  `Aantal` int(11) NOT NULL DEFAULT 1,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL DEFAULT 0,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `LidEvenement` (`LidID`,`EvenementID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sEvenement_Type` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Omschrijving` varchar(30) CHARACTER SET utf8 COLLATE utf8_unicode_ci DEFAULT NULL,
  `Soort` char(1) NOT NULL DEFAULT 'W',
  `Tekstkleur` varchar(12) DEFAULT NULL,
  `Vet` tinyint(4) NOT NULL DEFAULT 0,
  `Cursief` tinyint(4) NOT NULL DEFAULT 0,
  `Achtergrondkleur` varchar(12) DEFAULT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sFoto` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL DEFAULT 0,
  `FotoData` longblob NOT NULL,
  `Type` char(1) NOT NULL DEFAULT 'P',
  `FotoGewijzigd` datetime NOT NULL DEFAULT current_timestamp(),
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

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
  `Inval` tinyint(4) DEFAULT NULL,
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
  `Kode` varchar(8) DEFAULT NULL,
  `Volgnummer` int(11) DEFAULT NULL,
  `Omschrijving` varchar(45) DEFAULT NULL,
  `Instructeurs` varchar(60) DEFAULT NULL,
  `Zwemzaal` varchar(15) DEFAULT NULL,
  `DiplomaID` int(11) DEFAULT NULL,
  `Aanwezigheidsnorm` smallint(6) DEFAULT NULL,
  `Starttijd` varchar(5) DEFAULT NULL,
  `Eindtijd` varchar(5) DEFAULT NULL,
  `ActiviteitID` int(11) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sKostenplaats` (
  `RecordID` int(11) NOT NULL DEFAULT 0,
  `Kode` varchar(5) DEFAULT NULL,
  `Naam` varchar(35) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLid` (
  `RecordID` int(11) NOT NULL,
  `Nummer` int(11) DEFAULT NULL,
  `Roepnaam` varchar(17) DEFAULT NULL,
  `Tussenv` varchar(7) DEFAULT NULL,
  `Achternaam` varchar(45) DEFAULT NULL,
  `Meisjesnm` varchar(25) DEFAULT NULL,
  `Voorletter` varchar(10) DEFAULT NULL,
  `Voornamen` varchar(40) DEFAULT NULL,
  `Geslacht` varchar(1) DEFAULT NULL,
  `GEBDATUM` date DEFAULT NULL,
  `Overleden` date DEFAULT NULL,
  `GEBPLAATS` varchar(22) DEFAULT NULL,
  `Adres` varchar(35) DEFAULT NULL,
  `Huisnr` int(11) DEFAULT NULL,
  `Huisletter` varchar(2) DEFAULT NULL,
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
  `RekeningBetaaldDoor` int(11) DEFAULT NULL,
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
  `GewijzigdDoor` int(11) DEFAULT NULL,
  `Verwijderd` date DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLiddipl` (
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
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLidmaatschap` (
  `RecordID` int(11) NOT NULL,
  `Lid` int(11) DEFAULT NULL,
  `LIDDATUM` date DEFAULT NULL,
  `Opgezegd` date DEFAULT NULL,
  `OpgezegdDoorVereniging` tinyint(4) DEFAULT NULL,
  `RedenOpzegging` longtext DEFAULT NULL,
  `Lidnr` int(11) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Lidnr` (`Lidnr`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sLidond` (
  `RecordID` int(11) NOT NULL,
  `Lid` int(11) DEFAULT NULL,
  `OnderdeelID` int(11) DEFAULT NULL,
  `Vanaf` date DEFAULT NULL,
  `Opgezegd` date DEFAULT NULL,
  `Functie` smallint(6) DEFAULT NULL,
  `EmailFunctie` varchar(45) DEFAULT NULL,
  `GroepID` int(11) DEFAULT NULL,
  `ActiviteitID` int(11) DEFAULT NULL,
  `Opmerk` varchar(30) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMailing` (
  `RecordID` int(11) NOT NULL DEFAULT 0,
  `MailingID` int(11) NOT NULL AUTO_INCREMENT,
  `from_name` varchar(50) DEFAULT '',
  `from_addr` varchar(50) DEFAULT '',
  `MailingVanafID` int(11) DEFAULT NULL,
  `to_name` varchar(50) DEFAULT '' COMMENT 'Omschrijving van de groep personen aan wie deze mailing gericht is',
  `OmschrijvingOntvangers` varchar(50) DEFAULT NULL COMMENT 'Omschrijving van de groep personen aan wie deze mailing gericht is',
  `cc_addr` varchar(50) DEFAULT '',
  `subject` varchar(75) DEFAULT '',
  `message` text DEFAULT NULL,
  `Opmerking` varchar(75) DEFAULT NULL,
  `NietVersturenVoor` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `GroepOntvangers` int(11) NOT NULL DEFAULT 0,
  `ZonderBriefpapier` tinyint(4) DEFAULT 0,
  `GebruikPlaatjeAlsBericht` tinyint(4) DEFAULT 0,
  `template` tinyint(4) NOT NULL DEFAULT 0,
  `CCafdelingen` tinyint(4) NOT NULL DEFAULT 0,
  `Concept` tinyint(4) DEFAULT 1,
  `ZichtbaarVoor` int(11) NOT NULL DEFAULT 0,
  `EvenementID` int(11) NOT NULL DEFAULT 0,
  `new_on` datetime NOT NULL DEFAULT current_timestamp(),
  `AddedBy` int(11) NOT NULL DEFAULT 0,
  `changed_on` datetime NOT NULL DEFAULT current_timestamp(),
  `ChangedBy` int(11) NOT NULL DEFAULT 0,
  `SentBy` int(11) NOT NULL DEFAULT 0,
  `deleted_on` datetime DEFAULT NULL,
  `DeletedBy` int(11) DEFAULT NULL,
  `HTMLdirect` tinyint(4) DEFAULT 0,
  `InterneOpmerking` text DEFAULT NULL COMMENT 'Voor uitleg over de mailing',
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) DEFAULT 0,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`MailingID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMailing_hist` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL DEFAULT 0,
  `MailingID` int(11) NOT NULL,
  `Xtra_Char` varchar(5) DEFAULT NULL,
  `Xtra_Num` int(11) DEFAULT NULL,
  `from_name` varchar(50) NOT NULL,
  `from_addr` varchar(50) NOT NULL,
  `to_name` varchar(100) NOT NULL,
  `subject` varchar(75) NOT NULL,
  `to_addr` varchar(255) NOT NULL,
  `cc_addr` varchar(255) NOT NULL,
  `message` text NOT NULL,
  `ZonderBriefpapier` tinyint(4) DEFAULT 0,
  `ZichtbaarVoor` int(11) NOT NULL DEFAULT 0,
  `send_by` int(11) DEFAULT NULL,
  `send_on` datetime DEFAULT NULL,
  `NietVersturenVoor` datetime DEFAULT NULL,
  `Successful` tinyint(4) NOT NULL DEFAULT 1,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`RecordID`),
  KEY `LidID` (`LidID`),
  KEY `MailingID` (`MailingID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMailing_rcpt` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `MailingID` int(11) NOT NULL DEFAULT 0,
  `LidID` int(11) NOT NULL DEFAULT 0,
  `to_address` varchar(50) DEFAULT NULL,
  `Rcpt_Type` char(1) NOT NULL DEFAULT 'T',
  `Xtra_Char` char(5) DEFAULT NULL,
  `Xtra_Num` int(11) NOT NULL DEFAULT 0,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMailing_vanaf` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Vanaf_email` varchar(30) NOT NULL,
  `Vanaf_naam` varchar(50) NOT NULL,
  `SMTP_server` tinyint(20) NOT NULL DEFAULT 0 COMMENT 'Indien 0, dan wordt de smtp server uit het config bestand gebruikt',
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `VanafEmail` (`Vanaf_email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMemo` (
  `RecordID` int(11) DEFAULT NULL,
  `Lid` int(11) NOT NULL,
  `Soort` varchar(1) NOT NULL,
  `Vertrouwelijk` tinyint(4) DEFAULT 0,
  `Memo` longtext DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Lid`,`Soort`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sMutatie` (
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
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`BoekjaarID`,`Periode`,`DagboekID`,`Stuknr`,`Regelnr`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sOnderdl` (
  `RecordID` int(11) NOT NULL,
  `Kode` varchar(8) DEFAULT NULL,
  `Naam` varchar(50) DEFAULT NULL,
  `LIDCB` decimal(8,2) DEFAULT NULL,
  `JEUGDCB` decimal(8,2) DEFAULT NULL,
  `FUNCTCB` decimal(8,2) DEFAULT NULL,
  `NIETLIDCB` decimal(8,2) DEFAULT NULL,
  `ContributiePerActiviteit` tinyint(4) DEFAULT NULL,
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
  `MySQL` longtext DEFAULT NULL,
  `Opmerking` longtext DEFAULT NULL,
  `Beschrijving` varchar(255) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sOrganisatie` (
  `Nummer` smallint(6) NOT NULL,
  `Naam` varchar(8) DEFAULT NULL,
  `Volledige naam` varchar(55) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sRekening` (
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
  `BetalingskortingDagen` int(11) DEFAULT NULL,
  `Bedrag` decimal(8,2) DEFAULT NULL,
  `Betaald` decimal(8,2) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sRekeningBetaling` (
  RecordID int(11) DEFAULT NULL,
  Rekening int(11) NOT NULL,
  Datum date DEFAULT NULL,
  Bedrag decimal(8,2) DEFAULT NULL,
  Mutatie varchar(25) DEFAULT NULL,
  Ingevoerd datetime NOT NULL DEFAULT current_timestamp(),
  Gewijzigd timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (RecordID)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sRekreg` (
  `RecordID` int(11) NOT NULL,
  `Rekening` int(11) DEFAULT NULL,
  `Regelnr` int(11) DEFAULT NULL,
  `Lid` int(11) DEFAULT NULL,
  `KSTNPLTS` varchar(12) DEFAULT NULL,
  `OMSCHRIJV` varchar(70) DEFAULT NULL,
  `Bedrag` decimal(8,2) DEFAULT NULL,
  `ToonOpRekening` tinyint(4) DEFAULT NULL,
  `LidondID` int(11) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sSeizoen` (
  `Nummer` int(11) NOT NULL,
  `Begindatum` date DEFAULT NULL,
  `Einddatum` date DEFAULT NULL,
  `Leeftijdsgrens jeugdleden` smallint(6) DEFAULT NULL,
  `Contributie leden` decimal(8,2) DEFAULT NULL,
  `Contributie jeugdleden` decimal(8,2) DEFAULT NULL,
  `Contributie kader` decimal(8,2) DEFAULT NULL,
  `Contributie Onderscheidingen` decimal(8,2) DEFAULT NULL,
  `RekeningenVerzamelen` tinyint(4) DEFAULT NULL,
  `Rekeningomschrijving` varchar(30) DEFAULT NULL,
  `BetaaldagenTermijn` int(11) DEFAULT NULL,
  `StandaardAantalTermijnen` int(11) DEFAULT NULL,
  `Verenigingscontributie omschrijving` varchar(50) DEFAULT NULL,
  `Verenigingscontributie kostenplaats` varchar(12) DEFAULT NULL,
  `Gezinskorting bedrag` decimal(8,2) DEFAULT NULL,
  `Maximale verenigingscontributie` decimal(8,2) DEFAULT NULL,
  `Gezinskorting omschrijving` varchar(50) DEFAULT NULL,
  `Gezinskorting kostenplaats` varchar(12) DEFAULT NULL,
  `Ingevoerd` date DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sStukken` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Titel` varchar(50) NOT NULL,
  `Type` char(1) NOT NULL DEFAULT 'R',
  `BestemdVoor` varchar(30) NOT NULL,
  `VastgesteldOp` date DEFAULT NULL,
  `Ingangsdatum` date DEFAULT NULL,
  `Revisiedatum` date DEFAULT NULL,
  `Link` varchar(150) NOT NULL,
  `VervallenPer` date DEFAULT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `GewijzigdOp` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sTaak` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Omschrijving` varchar(75) NOT NULL,
  `Beschrijving` text DEFAULT NULL,
  `Taakgroep` int(11) NOT NULL,
  `GeplandGereed` date DEFAULT NULL,
  `WerkelijkGereed` date DEFAULT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sTaakgroep` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Kode` varchar(5) NOT NULL,
  `Naam` varchar(50) DEFAULT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sTaakLid` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `TaakID` int(11) NOT NULL,
  `LidID` int(11) NOT NULL,
  `Eindverantwoordelijke` tinyint(4) NOT NULL DEFAULT 0,
  `Functie` varchar(50) NOT NULL,
  `Ingevoerd` int(11) NOT NULL,
  `IngevoerdDoor` datetime NOT NULL DEFAULT current_timestamp(),
  `Gewijzigd` int(11) NOT NULL,
  `GewijzigdDoor` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

CREATE TABLE IF NOT EXISTS `%1\$sVerdichting` (
  `RecordID` int(11) NOT NULL DEFAULT 0,
  `Kode` varchar(4) DEFAULT NULL,
  `Omschrijving` varchar(40) DEFAULT NULL,
  `DebetCredit` varchar(1) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT NULL,
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `%1\$sWS_Artikel` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Code` varchar(8) COLLATE utf8_unicode_ci DEFAULT NULL,
  `Omschrijving` varchar(50) COLLATE utf8_unicode_ci DEFAULT NULL,
  `Beschrijving` text COLLATE utf8_unicode_ci DEFAULT NULL,
  `Maat` varchar(6) COLLATE utf8_unicode_ci DEFAULT NULL,
  `Verkoopprijs` decimal(10,2) NOT NULL DEFAULT 0.00,
  `BeschikbaarTot` date DEFAULT NULL COMMENT 'Tot welke datum is dit artikel in de zelfservice beschikbaar?',
  `VervallenPer` date DEFAULT NULL COMMENT 'Na deze datum is dit artikel voor niemand meer beschikbaar.',
  `MaxAantalPerLid` smallint(6) DEFAULT NULL COMMENT 'Hoeveel mag één lid maximaal van dit product bestellen?',
  `BeperkTotGroep` int(11) NOT NULL DEFAULT 0 COMMENT 'Welke groep mag dit artikel in de zelfservice bestellen? 0 is iedereen.',
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

CREATE TABLE IF NOT EXISTS `%1\$sWS_Orderregel` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Ordernr` int(11) NOT NULL DEFAULT 0,
  `Artikel` int(11) NOT NULL DEFAULT 0,
  `Lid` int(11) NOT NULL DEFAULT 0,
  `AantalBesteld` smallint(6) NOT NULL DEFAULT 0,
  `PrijsPerStuk` decimal(10,2) DEFAULT NULL,
  `Opmerking` varchar(30) COLLATE utf8_unicode_ci DEFAULT NULL,
  `BestellingDefinitief` datetime DEFAULT NULL COMMENT 'Heeft het lid de bestelling bevestigd?',
  `Rekening` int(11) DEFAULT NULL,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

CREATE TABLE IF NOT EXISTS `%1\$sWS_Voorraadboeking` (
  `RecordID` int(11) DEFAULT NULL,
  `ArtikelID` int(11) DEFAULT NULL,
  `Datum` date DEFAULT NULL,
  `Omschrijving` varchar(50) DEFAULT NULL,
  `Aantal` smallint(6) NOT NULL DEFAULT 0,
  `OrderregelID` int(11) NOT NULL DEFAULT 0,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL DEFAULT 0,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewjizigdDoor` int(11) NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
COMMIT;", TABLE_PREFIX);

	(new cls_db_base())->execsql($queries);
	
}  # db_createtables

?>