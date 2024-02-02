<?php

$arrTables[0] = "Aanwezigheid";
$arrTables[] = "Admin_access";
$arrTables[] = "Admin_activiteit";
$arrTables[] = "Admin_interface";
$arrTables[] = "Admin_login";
$arrTables[] = "Admin_param";
$arrTables[] = "Admin_template";
$arrTables[] = "Afdelingskalender";
$arrTables[] = "Eigen_lijst";
$arrTables[] = "Evenement";
$arrTables[] = "Evenement_Deelnemer";
$arrTables[] = "Evenement_Type";
$arrTables[] = "Examen";
$arrTables[] = "Foto";
$arrTables[] = "Inschrijving";
$arrTables[] = "Mailing";
$arrTables[] = "Mailing_hist";
$arrTables[] = "Mailing_rcpt";
$arrTables[] = "Mailing_vanaf";
$arrTables[] = "RekeningBetaling";
$arrTables[] = "Stukken";
$arrTables[] = "Website_inhoud";
$arrTables[] = "Website_menu";
$arrTables[] = "WS_Artikel";
$arrTables[] = "WS_Orderregel";
$arrTables[] = "WS_Voorraadboeking";
// $arrTables[] = "DMS_Document";
// $arrTables[] = "DMS_Folder";

// Overgenomen uit de Access-database
$arrTables[30] = "Activiteit";
$arrTables[] = "Diploma";
$arrTables[] = "Functie";
$arrTables[] = "Groep";
$arrTables[] = "Lid";
$arrTables[] = "Liddipl";
$arrTables[] = "Lidmaatschap";
$arrTables[] = "Lidond";
$arrTables[] = "Memo";
$arrTables[] = "Onderdl";
$arrTables[] = "Organisatie";
$arrTables[] = "Rekening";
$arrTables[] = "Rekreg";
$arrTables[] = "Seizoen";

$TypeActiviteit[0] = "N.T.B.";
$TypeActiviteit[1] = "Inloggen/Uitloggen";
$TypeActiviteit[2] = "Beheer/onderhoud/jobs";
$TypeActiviteit[3] = "DB backup";
$TypeActiviteit[4] = "Mailing";
$TypeActiviteit[5] = "Authenticatie";
$TypeActiviteit[6] = "Lidgegevens";
$TypeActiviteit[7] = "Evenementen";
$TypeActiviteit[8] = "Interface";
$TypeActiviteit[9] = "Upload data";
$TypeActiviteit[10] = "Webshop";
$TypeActiviteit[12] = "Diploma's en examens";
$TypeActiviteit[13] = "Parameters";
$TypeActiviteit[14] = "Rekeningen en betalingen";
$TypeActiviteit[15] = "Autorisatie";
$TypeActiviteit[16] = "Toestemmingen per lid";
$TypeActiviteit[18] = "Taken";
$TypeActiviteit[20] = "Stamgegevens";
$TypeActiviteit[21] = "Afdelingskalender";
$TypeActiviteit[22] = "Stukken";
$TypeActiviteit[23] = "Eigen lijsten";
$TypeActiviteit[24] = "Afwezigheid/presentie";
$TypeActiviteit[25] = "Inschrijvingen";
$TypeActiviteit[97] = "Foutieve login";
$TypeActiviteit[98] = "Performance";
$TypeActiviteit[99] = "Debug- en foutmeldingen";
asort($TypeActiviteit);

// error_reporting(E_ALL);

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
	public string $table = "";				// Naam van de tabel met prefix
	private string $alias = "";			// Alias van de tabel
	public string $basefrom = "";			// Naam van de tabel met alias
	private $refcolumn = "";				// Naam van de kolom bij een update
	private $typecolumn = "";				// Type van de kolom
	private $nullablecolumn = false;		// Is de kolom nullable?
	public $pkkol = "RecordID";			// Naam van de kolom met de primary key
	public $naamlogging = "";				// De naam die, in de logging, wordt gebruikt om aan te geven welk record het betreft
	private $aantalkolommen = -1;			// Het aantal kolommen in het SQL-statement
	private $aantalrijen = -1;				// Het aantal rijen in het SQL-statement
	public $mess = "";						// Boodschap in de logging
	public $ta = 0;							// Type activiteit van de logging
	public $tas = 0;							// Type activiteit specifiek van de logging
	public $tm = 0; 							// Toon boodschap: 0=nee, 1=aan iedereen, 2=alleen voor webmasters, 3=aan iedereen, via popup (alert)
	public $lidid = 0;						// RecordID van het lid
	public $query = "";						// De SQL-code die moet worden uitgevoerd.
	public string $where = "";				// Dit filter wordt op diverse plaatsen gebruikt, als er geen ander filter is gespecificeerd.
	public string $per = "";				// Diverse functies worden per deze datum uitgevoerd.
	public string $ingevoerd = "";			// Datum/tijd dat het record is ingevoerd.
	public string $gewijzigd = "";			// De laatste wijzigingsdatum en tijd.
	
	public $fdlang = "'%e %M %Y'";
	public $fdtlang = "'%e %M %Y (%H:%i)'";
	public $fdkort = "'%e-%c-%Y'";
	
	public $db_language = "'nl_NL'";
	
	public $fromlid = TABLE_PREFIX . "Lid AS L INNER JOIN " . TABLE_PREFIX . "Lidmaatschap AS LM ON L.RecordID=LM.Lid";
	public $fromlidond = '(' . TABLE_PREFIX . 'Lid AS L INNER JOIN ((' . TABLE_PREFIX . 'Lidond AS LO INNER JOIN ' . TABLE_PREFIX . 'Functie AS F ON LO.Functie=F.Nummer) INNER JOIN ' . TABLE_PREFIX . 'Onderdl AS O ON LO.OnderdeelID=O.RecordID) ON L.RecordID=LO.Lid) LEFT JOIN (' . TABLE_PREFIX . 'Groep AS GR LEFT JOIN ' . TABLE_PREFIX . 'Activiteit AS Act ON Act.RecordID=GR.ActiviteitID) ON LO.GroepID=GR.RecordID';

	public $wherelid = "LM.LIDDATUM <= CURDATE() AND IFNULL(LM.Opgezegd, '9999-12-31') >= CURDATE()";
	public static $wherelidond = "LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE()";
	public $selectnaam = "CONCAT(IF(LENGTH(IFNULL(L.Roepnaam, ''))>1, TRIM(L.Roepnaam), IFNULL(L.Voorletter, '')), ' ', IF(IFNULL(L.Tussenv, '')>'', CONCAT(L.Tussenv, ' '), ''), L.Achternaam)";
	public $selectzoeknaam = "TRIM(CONCAT(L.Achternaam, ', ', IF(LENGTH(L.Roepnaam)>1, L.Roepnaam, L.Voorletter), IF(L.Tussenv>'', CONCAT(' ', L.Tussenv), '')))";	
	public $selectavgnaam = "CONCAT(IFNULL(L.Roepnaam, ''), ' ', IF(IFNULL(L.Tussenv, '')>'', CONCAT(REPLACE(REPLACE(REPLACE(L.Tussenv, 'van der', 'v/d'), 'van den', 'v/d'), 'van de', 'v/d'), ' '), ''), SUBSTRING(L.Achternaam, 1, 1), '.')";
	public $selectgeslacht = "";
	public $selectleeftijd = "IF(IFNULL(L.GEBDATUM, '1900-01-01') < '1910-01-01' OR (NOT ISNULL(L.Overleden)), NULL, CONCAT(TIMESTAMPDIFF(YEAR, L.GEBDATUM, CURDATE()), ' jaar'))";
	public $selectlidnr = "SELECT MAX(IFNULL(Lidnr, 0)) FROM " . TABLE_PREFIX . "Lidmaatschap AS LM WHERE LM.Lid=L.RecordID AND LM.LIDDATUM <= CURDATE() AND IFNULL(LM.Opgezegd, '9999-12-31') >= CURDATE()";
	public $selecttelefoon = "IF(LENGTH(IFNULL(L.Mobiel, '')) < 10, IFNULL(L.Telefoon, ''), L.Mobiel)";
	public $selectemail = "IF(LENGTH(L.Email) > 5, L.Email, IF(LENGTH(L.EmailOuders) > 5, L.EmailOuders, L.EmailVereniging))";
	public $selectgroep = "CASE 
					WHEN IFNULL(LO.GroepID, 0)=0 AND LO.Functie=0 THEN 'Niet ingedeeld'
					WHEN LENGTH(GR.Instructeurs) > 1 THEN CONCAT(GR.Omschrijving, ' | ', GR.Instructeurs)
					WHEN LENGTH(GR.Omschrijving) > 0 THEN GR.Omschrijving
					ELSE LO.GroepID
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
		$this->per = date("Y-m-d");
	}
		
	public function execsql($p_query="", $p_logtype=-1, $p_debug=0) {
		global $dbc;
		
		$starttijd = microtime(true);
		$mess = "";
		
		if (strlen($p_query) > 5) {
			$this->query = $p_query;
		}
		
		if ($p_debug == 1) {
			debug($this->query, 1);
		} elseif ($p_debug == 2) {
			debug($this->query, 0, 1);
		}
		
		if (strlen($this->query) <= 5 and isset($dbc)) {
			debug("Het uit te voeren SQL-statement is leeg.", 2, 1);
			return false;

		} elseif (isset($dbc)) {

			try {
				$dbc->query("SET lc_time_names='nl_NL';");
				$h = date("Z")/(60*60);
				$i = 60*($h-floor($h));
				$tzqry = sprintf("SET time_zone='%+d:%02d';", $h, $i);
				$dbc->query($tzqry);
//				$dbc->query("SET GLOBAL innodb_stats_on_metadata=0;");
				if (startwith($this->query, "SELECT ")) {
//					debug($this->query);
					$rv = $dbc->query($this->query);
					$exec_tijd = microtime(true) - $starttijd;
					if (isset($_SESSION['settings']['performance_trage_select']) and $_SESSION['settings']['performance_trage_select'] > 0 and $exec_tijd >= $_SESSION['settings']['performance_trage_select']) {
						$mess = sprintf("%.2f seconden query: %s", $exec_tijd, $this->query);
						$this->ta = 98;
					} elseif ($this->ta > 0) {
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
//					debug("$this->query, $rv");
					if (substr($this->query, 0, 6) == "UPDATE" or substr($this->query, 0, 7) == "DELETE ") {
						if ($rv > 0) {
							$mess = sprintf("Uitgevoerd: %s / records affected: %d", $this->query, $rv);
						} else {
//							$mess = sprintf("Uitgevoerd zonder resultaat: %s", $this->query);
						}
					} else {
						$exec_tijd = microtime(true) - $starttijd;
						$mess = sprintf("Uitgevoerd: %s in %.1f seconden", $this->query, $exec_tijd);
					}
					$result = null;
				}
				if ($p_logtype > 0 and strlen($mess) > 0) {
					(new cls_Logboek())->add($mess, $p_logtype, -1, 0, 0, $this->tas);
				}
				return $rv;
		
			} catch (Exception $e) {					
				$mess = sprintf("Error '%s' in SQL '%s': %s", $e->getCode(), $this->query, $e->getMessage());
				if ($e->getCode() == 2006) {
					debug($mess, 2, 0);
				} else {
					debug($mess, 2, 99);
				}
				return false;
			}
			
		} else {
			debug("Er is geen verbinding met de database-server.", 2, 0);
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
			$this->ta = 99;
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
	
	public function bestaat_index($p_table, $p_name) {
		if (strlen(TABLE_PREFIX) > 1 and startwith($p_table, TABLE_PREFIX) == FALSE) {
			$this->table = TABLE_PREFIX . $p_table;
		} else {
			$this->table = $p_table;
		}
	
		$query = sprintf("SELECT COUNT(*) FROM information_schema.statistics WHERE table_schema='%s' AND table_name='%s' AND index_name='%s';", DB_NAME, $this->table, $p_name);
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
		
		if (str_replace(TABLE_PREFIX, "", $this->table) == "Admin_access" and $p_kolom == "GewijzigdDoor") {
			return false;
		} elseif (str_replace(TABLE_PREFIX, "", $this->table) == "Admin_access" and $p_kolom == "Gewijzigd") {
			return true;
		} elseif (str_replace(TABLE_PREFIX, "", $this->table) == "Eigen_lijst" and $p_kolom == "Gewijzigd") {
			return true;
		} else {
			$query = sprintf("SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='%s' AND TABLE_NAME='%s' AND COLUMN_NAME='%s';", DB_NAME, $this->table, $p_kolom);
			if ($this->scalar($query) == 1) {
				return true;
			} else {
				return false;
			}
		}
	}
	
	public function typekolom($p_kolom, $p_table="") {
		
		$p = strpos($p_kolom, ".");
		if ($p !== false and $p > 0) {
			$p_kolom = substr($p_kolom, $p+1);
		}
		$this->typecolumn = "";
		$this->refcolumn = "";
		
		if (strlen($p_table) > 0) {
			$this->table = TABLE_PREFIX . $p_table;
		}
		
		$tn = str_replace(TABLE_PREFIX, "", $this->table);
		
		if ($p_kolom == "RecordID" or $p_kolom == "Nummer") {
			$this->typecolumn = "int";
			$this->refcolumn = $p_kolom;
			$this->nullablecolumn = false;

		} elseif ($p_kolom == "Regelnr" or $p_kolom == "Lid" or $p_kolom == "LidID" or $p_kolom == "LidondID" or $p_kolom == "ActiviteitID" or $p_kolom == "OnderdeelID" or $p_kolom == "Seizoen" or $p_kolom == "BETAALDAG") {
			$this->typecolumn = "int";
			$this->refcolumn = $p_kolom;
			$this->nullablecolumn = true;

		} elseif ($p_kolom == "Status" or $p_kolom == "OMSCHRIJV" or $p_kolom == "DEBNAAM" or $p_kolom == "KSTNPLTS") {
			$this->typecolumn = "text";
			$this->refcolumn = $p_kolom;
			$this->nullablecolumn = true;
			
		} elseif ($p_kolom == "Bedrag" or $p_kolom == "Betaald") {
			$this->typecolumn = "decimal";
			$this->refcolumn = $p_kolom;
			$this->nullablecolumn = true;
			
		} elseif ($p_kolom == "LIDDATUM" or $p_kolom == "Opgezegd" or $p_kolom == "Vanaf" or $p_kolom == "DatumBehaald" or $p_kolom == "Einddatum" or $p_kolom == "Datum" or $p_kolom == "LaatstGebruikt") {
			$this->typecolumn = "date";
			$this->nullablecolumn = true;
			$this->refcolumn = $p_kolom;
			
		} elseif ($p_kolom == "Ingevoerd" or $p_kolom == "Gewijzigd" or $p_kolom == "DatumTijd") {
			$this->typecolumn = "datetime";
			$this->refcolumn = $p_kolom;
			$this->nullablecolumn = true;
			
		} elseif (strlen($p_kolom) > 1) {
			$query = sprintf("SELECT TABLE_NAME, COLUMN_NAME, DATA_TYPE, IS_NULLABLE FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE '%s';", DB_NAME, $this->table, $p_kolom);
			$row = $this->execsql($query)->fetch();
			if (isset($row->COLUMN_NAME) and strlen($row->COLUMN_NAME) > 0) {
				$this->refcolumn = $row->COLUMN_NAME;
				$this->typecolumn = $row->DATA_TYPE;
				$this->nullablecolumn = $row->IS_NULLABLE;
			}
		}
		
		return $this->typecolumn;
		
	}
	
	public function lengtekolom($p_kolom) {		
		if ($p_kolom == "Script") {
			return 100;
		} else {
			$query = sprintf("SELECT CHARACTER_MAXIMUM_LENGTH FROM information_schema.COLUMNS WHERE TABLE_SCHEMA LIKE '%s' AND TABLE_NAME LIKE '%s' AND COLUMN_NAME LIKE '%s';", DB_NAME, $this->table, $p_kolom);
			return $this->scalar($query);
		}
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
	
		$rid = $this->scalar(sprintf("SELECT IFNULL(MAX(O.RecordID), 0) FROM %sOnderdl AS O;", TABLE_PREFIX));
		$r = $this->scalar(sprintf("SELECT IFNULL(MAX(DP.RecordID), 0) FROM %sDiploma AS DP;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		$r = $this->scalar(sprintf("SELECT IFNULL(MAX(EL.RecordID), 0) FROM %sEigen_lijst AS EL;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		$r = $this->scalar(sprintf("SELECT IFNULL(MAX(Act.RecordID), 0) FROM %sActiviteit AS Act;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		$r = $this->scalar(sprintf("SELECT IFNULL(MAX(F.Nummer), 0) FROM %sFunctie AS F;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		$r = $this->scalar(sprintf("SELECT IFNULL(MAX(DP.RecordID), 0) FROM %sDiploma AS DP;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		$r = $this->scalar(sprintf("SELECT IFNULL(MAX(ST.RecordID), 0) FROM %sStukken AS ST;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		$r = $this->scalar(sprintf("SELECT IFNULL(MAX(EX.Nummer), 0) FROM %sExamen AS EX;", TABLE_PREFIX));
		if ($r > $rid) {$rid = $r; }
		$r = $this->scalar(sprintf("SELECT IFNULL(MAX(EO.RecordID), 0) FROM %sExamenonderdeel AS EO;", TABLE_PREFIX));
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
			$rid = $this->scalar(sprintf("SELECT IFNULL(MAX(RK.Nummer), 0) FROM %sRekening AS RK WHERE RK.Nummer >= %d;", TABLE_PREFIX, $p_min-1));
		
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
		
		} elseif ($tabel == "Website_inhoud" or $tabel == "Website_menu") {
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(WI.RecordID), 0) FROM %sWebsite_inhoud AS WI;", TABLE_PREFIX));
			if ($r > $rid) {$rid = $r; }
			$r = $this->scalar(sprintf("SELECT IFNULL(MAX(WM.RecordID), 0) FROM %sWebsite_menu AS WM;", TABLE_PREFIX));
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
	
	public function bestaat_pk($p_id) {
		$query = sprintf("SELECT COUNT(*) FROM %s WHERE `%s`=%d;", $this->basefrom, $this->pkkol, $p_id);
		if ($this->scalar($query) > 0) {
			return true;
		} else {
			return false;
		}
	}
	
	public function waarde($p_recid, $p_kolom="") {
	}
	
	public function aantalkolommen($p_sql) {
		$this->aantalkolommen = $this->execsql($p_sql)->ColumnCount();
		return $this->aantalkolommen;
	}

	public function aantal($p_filter="", $p_distinct="") {
		
		if (strlen($p_filter) > 0 and strlen($this->where) > 0) {
			$p_filter = "WHERE " . $p_filter . " AND " . $this->where;
		} elseif (strlen($p_filter) > 0) {
			$p_filter = "WHERE " . $p_filter;
		} elseif (strlen($this->where) > 0) {
			$p_filter = "WHERE " . $this->where;
		}
	
		if ($this->bestaat_tabel($this->table) == false) {
			debug(sprintf("Tabel '%s' bestaat niet", $this->table), 2, 1);
			return false;
			
		} elseif (strlen($this->basefrom) > 0) {
			if (strlen($p_distinct) > 0) {
				$query = sprintf("SELECT COUNT(DISTINCT(%s)) FROM %s %s;", $p_distinct, $this->basefrom, $p_filter);
			} else {
				$query = sprintf("SELECT COUNT(*) FROM %s %s;", $this->basefrom, $p_filter);
			}
			return $this->scalar($query);
			
		} else {
			debug("Geen tabel bekend.", 2, 1);
			return false;
			
		}
	}  # aantal
	
	public function min($p_kolom="", $p_filter="") {
		if (strlen($p_kolom) == 0) {
			$p_kolom = $this->pkkol;
		}
		
		if ($this->is_kolom_numeriek($p_kolom) == true) {
			$query = sprintf("SELECT MIN(IFNULL(%1\$s, 0)) FROM %2\$s WHERE (%1\$s IS NOT NULL)", $p_kolom, $this->basefrom);
		} else {
			$query = sprintf("SELECT MIN(IFNULL(%1\$s, '')) FROM %2\$s WHERE (%1\$s IS NOT NULL)", $p_kolom, $this->basefrom);
		}
		if (strlen($p_filter) > 0 and strlen($this->where) > 0) {
			$query .= " AND " . $p_filter . " AND " . $this->where;
		} elseif (strlen($p_filter) > 0) {
			$query .= " AND " . $p_filter;
		} elseif (strlen($this->where) > 0) {
			$query .= " AND " . $this->where;
		}
		$query .= ";";
		$result = $this->execsql($query);
		$rv = $result->fetchColumn();
		if (strlen($rv) == 0 and $this->is_kolom_numeriek($p_kolom) == true) {
			return 0;
		} else {
			return $rv;
		}
	}
	
	public function max($p_kolom="", $p_filter="") {
		if (strlen($p_kolom) == 0) {
			$p_kolom = $this->pkkol;
		}
		
		if (substr($p_kolom, 0, 6) == "IFNULL") {
			$tk = "function";
		} else {
			$tk = $this->typekolom($p_kolom);
		}
		
//		debug("max function");
		
		if ($tk == "date" or $tk == "function") {
			$query = sprintf("SELECT IFNULL(MAX(%s), '') FROM %s", $p_kolom, $this->basefrom);
		} elseif ($this->is_kolom_numeriek($p_kolom, $tk) == true) {
			$query = sprintf("SELECT IFNULL(MAX(%s), 0) FROM %s", $p_kolom, $this->basefrom);
		} else {
			$query = sprintf("SELECT MAX(IFNULL(%s, '')) FROM %s", $p_kolom, $this->basefrom);
		}
		
		if (strlen($p_filter) > 0 and strlen($this->where) > 0) {
			$query .= " WHERE " . $p_filter . " AND " . $this->where;
		} elseif (strlen($p_filter) > 0) {
			$query .= " WHERE " . $p_filter;
		} elseif (strlen($this->where) > 0) {
			$query .= " WHERE " . $this->where;
		}			
		$query .= ";";
		$result = $this->execsql($query);
		return $result->fetchColumn();
	}
	
	public function laatste($p_kolom, $p_filter="", $p_sort="") {
		$query = sprintf("SELECT %s FROM %s", $p_kolom, $this->basefrom);
		if (strlen($p_filter) > 0) {
			$query .= sprintf(" WHERE %s", $p_filter);
		} elseif (strlen($this->where) > 0) {
			$query .= sprintf(" WHERE %s", $this->where);
		}
		$query .= " ORDER BY ";
		if (strlen($p_sort) > 0) {
			$query .= sprintf("%s, ", $p_sort);
		}			
		$query .= sprintf("%s DESC LIMIT 1;", $this->pkkol);
		
		return $this->scalar($query);
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
	
	public function pdoupdate($p_recid, $p_kolom, $p_waarde, $p_reden="") {
		global $dbc, $arrTables;
		
		$this->refcolumn = $p_kolom;
		$rv = false;
		$this->mess = "";
		
//		$mess = sprintf("%d / %s / %s / %s", $p_recid, $p_kolom, $p_waarde, $p_reden);
//		debug($mess, 0, 1);

		if ($p_recid >= 0 and strlen($p_kolom) > 0 and strlen($this->table) > 0) {
			
			$tk = $this->typekolom($p_kolom);
			if (($tk == "date" or $tk == "datetime") and (strlen($p_waarde) < 8 or $p_waarde <= "1900-01-01")) {
				$p_waarde = "NULL";
			} elseif ($tk == "time" and strlen($p_waarde) < 3) {
				$p_waarde = "NULL";
			} elseif ($this->is_kolom_numeriek("", $tk) and strlen(trim($p_waarde)) == 0) {
				$p_waarde = 0;
			} elseif ($tk == "decimal") {
				$s = $this->scalekolom($p_kolom);
				$p_waarde = str_replace(",", ".", $p_waarde);
				if ($s > 0) {
					$p_waarde = round(floatval($p_waarde), $s);
				} else {
					$p_waarde = floatval($p_waarde);
				}
			} elseif ($tk == "longblob") {
				$p_waarde = $p_waarde;
			} else {
				$p_waarde = trim($p_waarde);
			}
			
			if ($this->is_kolom_tekst("", $tk) and strlen($p_waarde) > $this->lengtekolom($p_kolom)) {
				$p_waarde = substr($p_waarde, 0, $this->lengtekolom($p_kolom));
				$this->mess = sprintf("%s: de nieuwe waarde voor kolom '%s' is groter dan erin past, deze waarde wordt afgekapt.", $this->table, $p_kolom);
				$this->log();
				$this->mess = "";
			}
		
			if ($this->bestaat_kolom("Gewijzigd")) {
				$gw = ", Gewijzigd=SYSDATE()";
			} else {
				$gw = "";
			}
			if ($p_waarde === "NULL") {
				$xw = sprintf("(`%s` IS NOT NULL)", $p_kolom);
			
			} elseif ($this->is_kolom_numeriek($p_kolom) == true) {
				$xw = sprintf("(IFNULL(`%1\$s`, 0)<>:nw OR (`%1\$s` IS NULL))", $p_kolom);

			} elseif ($tk == "date") {
				$xw = sprintf("IFNULL(`%s`, '')<>:nw", $p_kolom);
				
			} elseif($tk == "text") {
				$xw = sprintf("BINARY (IFNULL(`%s`, '')<>:nw)", $p_kolom);

			} elseif($tk == "longtext" or $tk == "longblob") {
				$xw = sprintf("(IFNULL(`%s`, '') NOT LIKE :nw)", $p_kolom);

			} else {
				$p_waarde = str_replace("\"", "'", trim($p_waarde));
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
							$p_waarde = str_replace("\n", " ", $p_waarde);
							$p_waarde = "\"" . str_replace("\"", "'", $p_waarde) . "\"";
							
						} elseif ($kt == "decimal" and (strlen($p_waarde) > 1)) {
							$p_waarde = str_replace(",", ".", $p_waarde);
						
						} elseif ($this->is_kolom_numeriek($p_kolom, $kt) == false and strpos($p_waarde, "'") !== false) {
							$p_waarde = "\"" . $p_waarde . "\"";
							
						} elseif ($this->is_kolom_numeriek($p_kolom, $kt) == false) {
							$p_waarde = "'" . $p_waarde . "'";
						}
					
						$sql = sprintf("UPDATE [%s] SET [%s]=%s, Gewijzigd=#%s# WHERE %s=%d;", $t, $p_kolom, $p_waarde, date("m/d/Y H:i:s"), $this->pkkol, $p_recid);
						$this->interface($sql);
					} else {
						if ($this->is_kolom_numeriek($p_kolom, $kt) == false) {
							$p_waarde = "'" . $p_waarde . "'";
						}
					}
					if ($kt == "date" or $kt == "datetime") {
						$p_waarde = str_replace("#", "'", $p_waarde);
					}
					if (strlen($this->naamlogging) > 0) {
						$nm = " (" . $this->naamlogging . ")";
					} else {
						$nm = "";
					}
					if (strlen($p_waarde) > 125) {
						$p_waarde = substr($p_waarde, 0, 121) . " ...";
					}
					$this->mess = sprintf("Tabel %s: van record %d%s is kolom '%s' in %s gewijzigd", str_replace(TABLE_PREFIX, "", $this->table), $p_recid, $nm, $p_kolom, $p_waarde);
					if (strlen($p_reden) > 0) {
						$this->mess .= ", omdat " . $p_reden;
					}
					$rv = true;
				}
			}
		}
		
		return $rv;
	}  # pdoupdate
	
	public function pdodelete($p_recid, $p_reden="") {
		global $dbc, $arrTables;
		$this->mess = "";
		$rc = 0;
		
		$query = sprintf("DELETE FROM `%s` WHERE `%s`=:id;", $this->table, $this->pkkol);
		$stmt = $dbc->prepare($query);
		$stmt->bindValue(":id", $p_recid);
		if ($stmt->execute()) {
			$rc = $stmt->rowCount();
			if ($rc > 0) {
				if (strlen($p_reden) > 0) {
					$p_reden = ", omdat " . $p_reden;
				}
			
				$t = str_replace(TABLE_PREFIX, "", $this->table);
				if (strlen($this->naamlogging) > 0) {
					$nm = " (" . $this->naamlogging . ")";
				} else {
					$nm = "";
				}
				$this->mess = sprintf("Tabel %s: Record %d%s is verwijderd%s", $t, $p_recid, $nm, $p_reden);

				if (array_search($t, $arrTables) >= 30 and $_SESSION['settings']['interface_access_db'] == 1) {
					$sql = sprintf("DELETE FROM %s WHERE %s=%d;", $t, $this->pkkol, $p_recid);
					$this->interface($sql);
				}
			}
		}
		
		return $rc;
	}  # pdodelete

	public function controleersql($p_query, $p_melding=0) {
		global $dbc;
		
		try {
			$dbc->query($p_query);
		} catch (Exception $e) {
			if ($p_melding == 1) {
				printf("<p class='waarschuwing'>Foutmelding in SQL: %s.</p>", $e->getMessage());
			}
			return false;
		}
		return true;
	}
	
	public function basislijst($p_filter="", $p_orderby="", $p_fetched=1, $p_limiet=-1) {
		
		if (strlen($p_filter) > 0 and strlen($this->where) > 0) {
			$p_filter = "WHERE " . $p_filter . " AND " . $this->where;
		} elseif (strlen($p_filter) > 0) {
			$p_filter = "WHERE " . $p_filter;
		} elseif (strlen($this->where) > 0) {
			$p_filter = "WHERE " . $this->where;
		}
		
		if (strlen($p_orderby) > 0) {
			$p_orderby = "ORDER BY " . $p_orderby;
		}
		
		if (strlen($this->basefrom) > 1) {
			$fr = $this->basefrom;
		} else {
			$fr = $this->table;
		}
		$lm = "";
		if ($p_limiet > 0) {
			$lm = sprintf(" LIMIT %d", $p_limiet);
		}

		$query = sprintf("SELECT * FROM %s %s %s %s;", $fr, $p_filter, $p_orderby, $lm);
		$result = $this->execsql($query);
		
		if ($p_fetched == 1) {
			return $result->fetchAll();
		} else {
			return $result;
		}
	}
	
	public function optimize() {
		$this->tas = 21;
		
		$this->execsql(sprintf("OPTIMIZE TABLE %s;", $this->table), 2);
		
	}  # optimize
	
	public function log($p_refID=0, $p_toonmess=-1, $p_refondid=0) {
		$lbid = 0;
		if ($p_toonmess >= 0) {
			$this->tm = $p_toonmess;
		}
		
		if (strlen($this->mess) > 0) {
			$lbid = (new cls_Logboek())->add($this->mess, $this->ta, $this->lidid, $this->tm, $p_refID, $this->tas, $this->table, $this->refcolumn, 0, $p_refondid);
			$this->mess = "";
		}
		
		return $lbid;
	}
	
	public function Interface($p_query) {
		if ($_SESSION['settings']['interface_access_db'] == 1) {
			$i_int = new cls_Interface();
			$i_int->lidid = $this->lidid;
			$i_int->add($p_query);
			$i_int = null;
		}
	}
	
	public function ingevoerdtekst() {
		global $dtfmt;
		
		if (strlen($this->ingevoerd) > 10 and substr($this->ingevoerd, 11, 5) > "00:00") {
			$dtfmt->setPattern(DTLONGSEC);
			return $dtfmt->format(strtotime($this->ingevoerd));
		} elseif (strlen($this->ingevoerd) >= 10) {
			$dtfmt->setPattern(DTTEXT);
			return $dtfmt->format(strtotime($this->ingevoerd));
		} else {
			return "";
		}
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
	public $adres = "";
	public $postcode = "";
	public $woonplaats = "";
	public $telefoon = "";
	public string $email = "";
	public string $emailouders = "";
	public string $emailvereniging = "";
	public $huisnr = 0;
	public string $huisletter = "";
	public string $toevoeging = "";
	public $lidnr = 0;
	public $iskader = false;
	public $islid = false;
	public $lidvanaf = "";
	public int $rekeningbetaalddoor = 0;
	public string $bankrekening = "";
	public int $machtigingafgegeven = 0;
	public string $opmerking = "";
	
	function __construct($p_lidid=-1, $p_per="") {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Lid";
		$this->basefrom = $this->table . " AS L";
		if (strlen($p_per) < 10) {
			$this->per = date("Y-m-d");
		} else {
			$this->per = $p_per;
		}
		$this->vulvars($p_lidid, $p_per);
		$this->ta = 6;
	}
	
	public function vulvars($p_lidid=-1, $p_per="") {
		if ($p_lidid < 0) {
			$p_lidid = $this->lidid;
		}
		if (strlen($p_per) == 10) {
			$this->per = $p_per;
		}
		if ($p_lidid != $this->lidid) {
			$this->lidid = $p_lidid;
			
			$this->naamlid = "";
			$this->zoeknaam = "";
			$this->roepnaam = "";
			$this->geslacht = "O";
			$this->geboortedatum = "1900-01-01";
			$this->adres = "";
			$this->huisnr = 0;
			$this->postcode = "";
			$this->woonplaats = "";
			$this->telefoon = "";
			$this->email = "";
			$this->emailouders = "";
			$this->emailvereniging = "";
			$this->lidvanaf = "";
			$this->iskader = false;
			$this->islid = false;
			$this->lidnr = 0;
			$this->rekeningbetaalddoor = 0;
		
			if ($this->lidid > 0) {
				$query = sprintf("SELECT *, %s AS NaamLid, %s AS Zoeknaam, %s AS Telefoon FROM %s WHERE L.RecordID=%d;", $this->selectnaam, $this->selectzoeknaam, $this->selecttelefoon, $this->basefrom, $this->lidid);
				$result = $this->execsql($query);
				$row = $result->fetch();
				if (isset($row->RecordID) and $row->RecordID > 0) {
					$this->naamlid = $row->NaamLid;
					$this->zoeknaam = $row->Zoeknaam;
					$this->roepnaam = $row->Roepnaam ?? "";
					$this->geslacht = $row->Geslacht ?? "O";
					$this->geboortedatum = $row->GEBDATUM ?? "";
					$this->adres = trim($row->Adres ?? "");
					$this->postcode = $row->Postcode ?? "";
					$this->woonplaats = trim($row->Woonplaats ?? "");
					
					if (strlen($row->Mobiel) >= 10) {
						$this->telefoon = $row->Mobiel;
					} else {
						$this->telefoon = $row->Telefoon;
					}
					
					if (isValidMailAddress($row->Email, 0)) {
						$this->email = $row->Email;
					} elseif (isValidMailAddress($row->EmailVereniging, 0)) {
						$this->email = $row->EmailVereniging;
					} elseif (isValidMailAddress($row->EmailOuders, 0) and $this->geboortedatum > date("Y-m-d", strtotime("-18 year"))) {
						$this->email = $row->EmailOuders;
					}
					$this->emailouders = $row->EmailOuders ?? "";
					$this->emailvereniging = $row->EmailVereniging ?? "";
					
					$this->huisnr = $row->Huisnr ?? 0;
					$this->huisletter = trim($row->Huisletter ?? "");
					$this->toevoeging = trim($row->Toevoeging ?? "");
					
					$this->rekeningbetaalddoor = $row->RekeningBetaaldDoor ?? 0;
					$this->bankrekening = $row->Bankrekening ?? "";
					$this->machtigingafgegeven = $row->{'Machtiging afgegeven'} ?? 0;
					$this->opmerking = $row->Opmerking ?? "";
					$this->ingevoerd = $row->Ingevoerd ?? "";
					$this->gewijzigd = $row->Gewijzigd ?? "";
					
					$lmqry = sprintf("SELECT LM.RecordID, LM.LIDDATUM, LM.Lidnr FROM %1\$sLidmaatschap AS LM WHERE LM.Lid=%2\$d AND LM.LIDDATUM <= '%3\$s' AND IFNULL(LM.Opgezegd, '9999-12-31') >= '%3\$s';", TABLE_PREFIX, $this->lidid, $this->per);
					$lmres = $this->execsql($lmqry);
					$lmrow = $lmres->fetch();
					if (isset($lmrow->RecordID)) {
						$this->islid = true;
						$this->lidvanaf = $lmrow->LIDDATUM;
						$this->lidnr = $lmrow->Lidnr;
					}
					
					$query = sprintf("SELECT COUNT(*) FROM %1\$s WHERE LO.Lid=%2\$d AND (O.Kader=1 OR F.Kader=1) AND LO.Vanaf <= '%3\$s' AND IFNULL(LO.Opgezegd, '9999-12-31') >= '%3\$s';", $this->fromlidond, $this->lidid, $this->per);
					if ($this->scalar($query) > 0) {
						$this->iskader = true;
					} else {
						$this->iskader = false;
					}
				} else {
					$this->lidid = 0;
				}
			}
		}
	}  # cls_Lid->vulvars

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
				$this->naamlid = $row->NaamLid;
				return $row;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}  # cls_Lid->record
	
	public function ledenlijst($p_soortlid=1, $p_ondfilter=-1, $p_ord="", $p_filter="", $p_metadres=0) {	
		/*
			p_soortlid:
			0 = geen filter
			1 = Lid
			2 = Toekomstig lid
			3 = Voormalig lid of Oud-lid
			4 = Kloslid
			5 = Huidige en toekomstige leden
		*/
		
		$i_ond = new cls_Onderdeel();
		$i_act = new cls_Activiteit();
		$i_el = new cls_Eigen_lijst();
		
		$xtraselect = "";
		
		if (toegang("Woonadres_tonen", 0, 0) or $p_metadres == 1) {
			$xtraselect .= ", L.Adres, L.Postcode, L.Woonplaats";
		}
		
		$xtraselect .= sprintf(", CONCAT(%s, ' & ', IFNULL(%s, '')) AS Bereiken", $this->selecttelefoon, $this->selectemail);
		$xtraselect .= sprintf(", IFNULL(%s, '') AS Telefoon", $this->selecttelefoon);
		$xtraselect .= sprintf(", IFNULL(%s, '') AS Email", $this->selectemail);
		$xtraselect .= ", MAX(LM.Lidnr) AS Lidnr";
		if ($p_soortlid != 4) {
			$xtraselect .= ", LM.LIDDATUM";
			$xtraselect .= ", LM.Opgezegd";
		}

		if ($p_soortlid >= 1 and $p_soortlid <= 3 and $p_ondfilter == 0) {
			$from = $this->fromlid;
		} else {
			$from = sprintf("%s LEFT JOIN %sLidmaatschap AS LM ON L.RecordID=LM.Lid", $this->basefrom, TABLE_PREFIX);
		}
		
		$filter = "WHERE (L.Verwijderd IS NULL)";
		if ($p_soortlid == 1) {
			$filter .= sprintf(" AND LM.LIDDATUM <= '%1\$s' AND IFNULL(LM.Opgezegd, '9999-12-31') >= '%1\$s'", $this->per);
		} elseif ($p_soortlid == 2) {
			$filter .= sprintf(" AND LM.LIDDATUM > '%1\$s'", $this->per);
		} elseif ($p_soortlid == 3) {
			$filter .= " AND IFNULL(LM.Opgezegd, '9999-12-31') < CURDATE()";
		} elseif ($p_soortlid == 4) {
			$filter .= " AND (LM.Lid IS NULL)";
		} elseif ($p_soortlid == 5) {
			$filter .= sprintf(" AND IFNULL(LM.Opgezegd, '9999-12-31') >= '%1\$s'", $this->per);
		}
		
		if ($p_ondfilter > 0 and $p_soortlid == 1) {
			$f = sprintf("O.RecordID=%d", $p_ondfilter);
			$f2 = sprintf("Act.RecordID=%d", $p_ondfilter);
			if ($i_ond->aantal($f) > 0) {
				$filter .= sprintf(" AND L.RecordID IN (SELECT LO.Lid FROM %sLidond AS LO WHERE LO.OnderdeelID=%d AND %s)", TABLE_PREFIX, $p_ondfilter, $this::$wherelidond);
			} elseif ($i_act->aantal($f2) > 0) {
				$filter .= sprintf(" AND L.RecordID IN (SELECT LO.Lid FROM %1\$sLidond AS LO INNER JOIN %1\$sGroep AS GR ON LO.GroepID=GR.RecordID WHERE ", TABLE_PREFIX);
				$filter .= sprintf("GR.ActiviteitID=%d AND %s)", $p_ondfilter, $this::$wherelidond);
			} else {
				$filter .= sprintf(" AND L.RecordID IN (%s)", $i_el->mysql($p_ondfilter, 1));
			}
		}
		if (strlen($p_filter) > 0) {
			$filter .= " AND " . $p_filter;
		}
		if (strlen($this->where) > 0) {
			$filter .= " AND " . $this->where;
		}
		
		if (strlen($p_ord) > 0) {
			$p_ord .= ", ";
		}
		
		$query = sprintf("SELECT DISTINCT L.RecordID, %s AS `NaamLid`%s, L.GEBDATUM, %s AS Zoeknaam, L.Postcode, L.RekeningBetaaldDoor, L.Achternaam, L.TUSSENV, L.Roepnaam, L.RecordID AS LidID, L.Opmerking
					FROM %s
					%s
					GROUP BY L.RecordID
					ORDER BY %sL.Achternaam, L.TUSSENV, L.Roepnaam, LM.LIDDATUM;", $this->selectnaam, $xtraselect, $this->selectzoeknaam, $from, $filter, $p_ord);
		$result = $this->execsql($query);
		return $result->fetchAll();
		
	}  # cls_Lid->ledenlijst
	
	public function stringnamen($p_soortlid=1) {
		$rv = "";
		$al = 0;
		$rows = $this->ledenlijst($p_soortlid);
		foreach ($rows as $row) {
			if ($al == 0) {
				$rv = $row->NaamLid;
			} elseif ($al == count($rows)-1) {
				$rv .= " en " . $row->NaamLid;
			} else {
				$rv .= ", " . $row->NaamLid;
			}
			$al++;
		}
		
		return $rv;
	}
	
	public function aantallid($p_soort="L", $p_geslacht="*") {
		
		if ($p_soort == "K") {
			// Klosleden
			$query = sprintf("SELECT COUNT(*) FROM %s LEFT OUTER JOIN %sLidmaatschap AS LM ON L.RecordID=LM.Lid WHERE (LM.Lid IS NULL)", $this->basefrom, TABLE_PREFIX);		
			if ($p_geslacht != "*") {
				$query .= sprintf(" AND L.Geslacht='%s'", $p_geslacht);
			}
		} elseif ($p_soort == "M") {
			// Kaderleden
			$query = sprintf("SELECT COUNT(DISTINCT LO.Lid) FROM %s WHERE (O.Kader=1 OR F.Kader=1) AND %s;", $this->fromlidond, cls_db_base::$wherelidond);
		} else {
			$query = sprintf("SELECT COUNT(*) FROM %s INNER JOIN %sLidmaatschap AS LM ON L.RecordID=LM.Lid WHERE ", $this->basefrom, TABLE_PREFIX);
			if ($p_soort == "L") {
				$query .= $this->wherelid;
			} elseif ($p_soort == "T") {
				$query .= "LM.LIDDATUM > CURDATE()";
			} else {
				$query .= "IFNULL(LM.Opgezegd, '9999-12-31') < CURDATE()";
			}
		}
		if (strlen($p_geslacht) == 1 and $p_geslacht != "*") {
			$query .= sprintf(" AND L.Geslacht='%s'", $p_geslacht);
		}
		$query .= ";";
		
		return $this->scalar($query);
	}
	
	public function jubilarissen($p_per) {
		
		$ondidkader = $_SESSION['settings']['kaderonderdeelid'] ?? 0;
		
		$sqlm = sprintf("SELECT SUM(DATEDIFF(IFNULL(LM2.Opgezegd, '%1\$s'), LM2.LIDDATUM)) FROM %2\$sLidmaatschap AS LM2 WHERE LM2.Lid=L.RecordID AND LM2.LIDDATUM < '%1\$s' GROUP BY LM2.Lid", $p_per, TABLE_PREFIX);
		if ($ondidkader > 0) {
			$sqkad = sprintf("SELECT IFNULL(SUM(DATEDIFF(IFNULL(LO.Opgezegd, '%1\$s'), LO.Vanaf)), 0) FROM %2\$sLidond AS LO WHERE LO.Lid=L.RecordID AND LO.OnderdeelID=%3\$d AND LO.Vanaf < IFNULL(LO.Opgezegd, '9999-12-31')", $p_per, TABLE_PREFIX, $ondidkader);
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
	}  # cls_lid->aantalklosleden
	
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
					IF(L.Burgerservicenummer > 1000, L.Burgerservicenummer, NULL) AS Burgerservicenummer,
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
					'Recordinformatie' as Kop7,
					L.RecordID, 
					L.Ingevoerd AS `dteIngevoerd op`,
					L.Gewijzigd AS `dteLaatst gewijzigd op`,
					L.Verwijderd AS 'dteVerwijderd op'
					FROM %s AS L LEFT OUTER JOIN %sLidmaatschap AS LM ON L.RecordID=LM.Lid 
					WHERE L.RecordID=%d 
					ORDER BY LM.LIDDATUM DESC;", $this->selectnaam, $this->selectgeslacht, $this->selectgebdatum, $this->selectleeftijd, $adresgegevens, $sqond, $sqlll, $sqllegitimatie, $sqllogin, $VOG, $this->table, TABLE_PREFIX, $p_lidid);
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
		
	}  # cls_Lid->baam
	
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
	
	public function zoeknaam($p_lidid=-1) {
		$thia->vulvars($p_lidid);
		return $this-zoeknaam;
	}
	
	public function geslacht($p_lidid=-1) {
		$this->vulvars($p_lidid);
		return $this->geslacht;
	}
	
	public function geboortedatum($p_lidid=-1) {
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
		if ($p_lidid > 0 and $p_lidid != $this->lidid) {
			$this->lidid = $p_lidid;
			$this->vulvars();
		}
		return $this->telefoon;
	}
	
	public function email($p_lidid=-1) {
		if ($p_lidid > 0 and $p_lidid != $this->lidid) {
			$this->vulvars($p_lidid);
		}
		return $this->email;
	}   # cls_Lid->email
	
	public function islid($p_lidid=-1, $p_per="") {
		$this->vulvars($p_lidid, $p_per);
		return $this->islid;
	}  # cls_Lid->islid
	
	public function iskader($p_lidid=-1, $p_per="") {
		$this->vulvars($p_lidid, $p_per);
		return $this->iskader;
	}  # cls_Lid->iskader
	
	public function onderscheiding($p_lidid=-1, $p_per="") {
		if ($p_lidid > 0 and $p_lidid != $this->lidid) {
			$this->lidid = $p_lidid;
			$this->vulvars();
		}
		if (strlen($p_per) < 10) {
			if (strlen($this->per) == 10) {
				$p_per = $this->per;
			} else {
				$p_per = date("Y-m-d");
			}
		}
		$query = sprintf("SELECT IFNULL(MAX(O.Naam), '') FROM %s WHERE LO.Lid=%d AND O.`Type`='O' AND LO.Vanaf <= '%3\$s' AND IFNULL(LO.Opgezegd, '9999-12-31') >= %3\$s;", $this->fromlidond, $this->lidid, $p_per);
		return $this->scalar($query);
	}  # cls_Lid->onderscheiding
	
	public function lijstselect($p_filter=0, $p_xf="", $p_per="", $p_ondid=-1) {
		
		if (strlen($p_per) < 10) {
			$p_per = $this->per;
		}
	
		$query = sprintf("SELECT L.RecordID, %s AS NaamLid, %s AS Zoeknaam ", $this->selectnaam, $this->selectzoeknaam);
		if ($p_filter == 1) {
			// Alleen leden
			$query .= sprintf("FROM %1\$s INNER JOIN %2\$sLidmaatschap AS LM ON L.RecordID=LM.Lid WHERE LM.LIDDATUM <= '%3\$s' AND IFNULL(LM.Opgezegd, '9999-12-31') >= '%3\$s'", $this->basefrom, TABLE_PREFIX, $p_per);

		} elseif ($p_filter == 2) {
			// Geen voormalige leden, Oud-leden, dus wel leden, klosleden en toekomstige leden
			$query .= sprintf("FROM %s LEFT OUTER JOIN %sLidmaatschap AS LM ON L.RecordID=LM.Lid WHERE (L.Verwijderd IS NULL) AND ((LM.Lid IS NULL) OR IFNULL(LM.Opgezegd, '9999-12-31') >= '%s')", $this->basefrom, TABLE_PREFIX, $p_per);

		} elseif ($p_filter == 3) {
			// Leden die aan een rekening zijn gekoppeld middels BetaaldDoor
			$query .= sprintf("FROM %s WHERE L.RecordID IN (SELECT RK.BetaaldDoor FROM %sRekening AS RK)", $this->basefrom, TABLE_PREFIX);

		} elseif ($p_filter == 5) {
			// Leden en toekomstige leden
			$query .= sprintf("FROM %s INNER JOIN %sLidmaatschap AS LM ON L.RecordID=LM.Lid WHERE IFNULL(LM.Opgezegd, '999-12-31') >= '%s'", $this->basefrom, TABLE_PREFIX, $p_per);
			
		} elseif ($p_filter == 6) {
			// Alleen Klosleden
			$query .= sprintf("FROM %s LEFT OUTER JOIN %sLidmaatschap AS LM ON L.RecordID=LM.Lid WHERE (LM.Lid IS NULL) AND (L.Verwijderd IS NULL)", $this->basefrom, TABLE_PREFIX, $p_per);
			
		} elseif ($p_filter == 7) {
			// Iedereen in de tabel Lid
			$query .= sprintf("FROM %s WHERE (L.Verwijderd IS NULL)", $this->basefrom);
			
		} else {
			$query .= sprintf("FROM %s WHERE L.RecordID > 0", $this->basefrom);
		}

		if ($p_ondid > 0) {
			$query .= sprintf(" AND L.RecordID IN (SELECT LO.Lid FROM %1\$sLidond AS LO WHERE LO.OnderdeelID=%2\$d AND LO.Vanaf <= '%3\$s' AND IFNULL(LO.Opgezegd, '9999-12-31') >= '%3\$s')", TABLE_PREFIX, $p_ondid, $p_per);
		}

		if (strlen($p_xf) > 0) {
			$query .= " AND " . $p_xf;
		}
		if (strlen($this->where) > 0) {
			$query .= " AND " . $this->where;
		}
		
		$query .= "	ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;";

		$result = $this->execsql($query);
		return $result->fetchAll();
		
	}  # cls_Lid->lijstselect
	
	public function htmloptions($p_cv=-1, $p_filter=0, $p_xf="", $p_per="", $p_ondid=-1) {
		if (strlen($p_per) < 10) {
			$p_per = $this->per;
		}
		$rv = "";
		
		foreach($this->lijstselect($p_filter, $p_xf, $p_per, $p_ondid) as $lid) {
			if ($lid->RecordID == $p_cv) {
				$s = " selected";
			} else {
				$s = "";
			}
			$tw = $lid->Zoeknaam;
			if ($p_filter == 0 or $p_filter == 2) {
				$sl = substr((new cls_Lidmaatschap())->soortlid($lid->RecordID, $p_per), 0, 1);
				if ($sl != "L") {
					$tw .= sprintf(" (%s)", $sl);
				}
			}
			$rv .= sprintf("<option%s value=%d>%s</option>\n", $s, $lid->RecordID, $tw);
		}
		
		return $rv;
		
	}  # cls_Lid->htmloptions
	
	public function lidbijemail($p_email, $p_xf="") {
		$p_email = strtolower($p_email);
		
		if (strlen($p_xf) > 0) {
			$p_xf = " AND " . $p_xf;
		}
		
		$query = sprintf("SELECT L.RecordID AS LidID, LM.Lidnr, %1\$s AS NaamLid FROM %2\$s INNER JOIN %3\$sLidmaatschap AS LM ON L.RecordID=LM.Lid
						  WHERE IFNULL(LM.Opgezegd, '9999-12-31') >= CURDATE() AND (LOWER(L.Email)='%4\$s' OR LOWER(L.EmailOuders)='%4\$s' OR LOWER(L.EmailVereniging)='%4\$s')%5\$s
						  ORDER BY LM.Lidnr;", $this->selectnaam, $this->basefrom, TABLE_PREFIX, $p_email, $p_xf);
		$rows = $this->execsql($query)->fetchAll();
		if (count($rows) == 1) {
			$this->vulvars($rows[0]->LidID);
		}
		return $rows;
	}
					
	public function verjaardagen($p_datum) {
		
		if (is_numeric($p_datum)) {
			$p_datum = date("Y-m-d", $p_datum);
		}
		
		if ($_SESSION['settings']['agenda_verjaardagen'] > 0) {
			$i_el = new cls_Eigen_lijst("", $_SESSION['settings']['agenda_verjaardagen']);
			if ($i_el->aantalkolommen == 1) {
				$verjqry = $i_el->mysql;
				if (substr($verjqry, -1) == ";") {
					$verjqry = substr($verjqry, 0, -1);
				}
				$wl = sprintf("L.RecordID IN (%s)", $verjqry);
			} else {
				(new cls_Parameter())->update("agenda_verjaardagen", 0, "deze eigen lijst niet geschikt is voor de verjaardagen");
				$wl = "1=2";
			}
			$i_el = null;
		} else {
			$wl = "1=2";
		}
		
		$df = sprintf("RIGHT(L.GEBDATUM, 5)='%s'", substr($p_datum, -5));
		$query = sprintf("SELECT L.RecordID, %1\$s AS `NaamLid`, L.GEBDATUM, (YEAR('%5\$s')-YEAR(L.GEBDATUM))-IF(RIGHT('%5\$s', 5)<RIGHT(L.GEBDATUM, 5), 1, 0) AS Leeftijd
					FROM %2\$s
					WHERE %3\$s AND %4\$s AND L.GEBDATUM > '1901-01-01'
					ORDER BY L.GEBDATUM DESC, L.RecordID;", $this->selectnaam, $this->basefrom, $wl, $df, $p_datum);

		return $this->execsql($query)->fetchAll();
	}  # cls_Lid->verjaardagen
	
	public function gemiddeldeleeftijd($p_geslacht="*") {
		$query = sprintf("SELECT ROUND(AVG(DATEDIFF(CURDATE(), L.GEBDATUM))/365.25, 1) FROM %s WHERE %s AND IFNULL(L.GEBDATUM, '9999-12-31') < CURDATE()", $this->fromlid, $this->wherelid);
		if (strlen($p_geslacht) == 1 and $p_geslacht != "*") {
			$query .= sprintf(" AND L.Geslacht='%s'", $p_geslacht);
		}
		$query .= ";";
		return $this->scalar($query);
	}
	
	public function add($p_achternaam, $p_postcode="") {
		global $dbc;
		$this->tas = 1;
		
		$data['recordid'] = $this->nieuwrecordid();
		$data['achternaam'] = $p_achternaam;
		$query = sprintf("INSERT INTO %s (RecordID, Achternaam, Ingevoerd) VALUES (:recordid, :achternaam, NOW());", $this->table);
		if ($this->lidid = $dbc->prepare($query)->execute($data) > 0) {
			$this->lidid = $data['recordid'];
			$this->mess = sprintf("Kloslid %d (%s) is toegevoegd", $this->lidid, $p_achternaam);
			$this->log($this->lidid);
			$this->interface($query);
			if (strlen($p_postcode) >= 6) {
				$this->update($this->lidid, "Postcode", $p_postcode);
			}
		}
		return $this->lidid;
	}  # cls_lid->add
	
	public function update($p_lidid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_lidid);
		$this->tas = 2;
		
		$p_waarde = ltrim(trim($p_waarde));
		if ($p_kolom == "Voorletter") {
			$p_waarde = trim(strtoupper($p_waarde));
			if (strlen($p_waarde) > 0 and substr($p_waarde, -1) != ".") {
				$p_waarde .= ".";
			}
			if (strlen($p_waarde) > 2) {
				$p_waarde = str_replace(" ", "", $p_waarde);
			}
		} elseif ($p_kolom == "Postcode" and $p_waarde != "NULL") {
			$p_waarde = trim(strtoupper($p_waarde));
			if (strlen($p_waarde) == 6) {
				$p_waarde = substr($p_waarde, 0, 4) . " " . substr($p_waarde, -2);
			}
		} elseif ($p_kolom == "Toevoeging" and strlen($p_waarde) > 1 and substr($p_waarde, 0, 1) == "-") {
			$p_waarde = substr($p_waarde, 1, 4);
		} elseif ($p_kolom == "Telefoon") {
			$p_waarde = str_replace(" ", "", $p_waarde);
		} elseif ($p_kolom == "Mobiel") {
			$p_waarde = str_replace(" ", "", $p_waarde);
			if (strlen($p_waarde) == 10 and substr($p_waarde, 0, 2) == "06") {
				$p_waarde = "06-" . substr($p_waarde, -8);
			}
		} elseif ($p_kolom == "EmailOuders" and strlen($p_waarde) > 5) {
			$p_waarde = str_replace(";", ",", $p_waarde);
		} elseif ($p_kolom == "Bankrekening") {
			$p_waarde = strtoupper($p_waarde);
		} elseif ($p_kolom == "Burgerservicenummer" and strlen($p_waarde) < 2) {
			$p_waarde = "NULL";
		}
		
		if ($p_kolom == "Postcode" and strlen($p_waarde) != 0 and strlen($p_waarde) != 7 and substr($p_waarde, 0, 4) < "9999" and substr($p_waarde, 0, 4) >= "1000") {
			$this->mess = "De postcode is niet correct, deze wijziging wordt niet verwerkt.";
			
		} elseif (($p_kolom == "Email" or $p_kolom == "EmailVereniging") and strlen($p_waarde) > 0 and isValidMailAddress($p_waarde, 0) == false) {
			$this->mess = sprintf("%s is niet correct, deze wijziging wordt niet verwerkt.", $p_kolom);
			
		} elseif ($p_kolom == "GEBDATUM" and $p_waarde > date("Y-m-d")) {
			$this->mess = "De geboortedatum kan niet in de toekomst liggen, deze wijziging wordt niet verwerkt.";
			
		} elseif ($p_kolom == "Bankrekening" and strlen($p_waarde) > 0 and $p_waarde != "NULL" and IsIBANgoed($p_waarde, 1) == false) {
			$this->mess = sprintf("Het formaat/controlegetal van de bankrekening %s is niet correct, deze wijziging wordt niet verwerkt.", $p_waarde);
				
		} elseif ($p_kolom == "Burgerservicenummer" and strlen($p_waarde) > 0 and $p_waarde != "NULL" and (!is_numeric($p_waarde) or $p_waarde < 100000000 or $p_waarde > 999999999)) {
			$this->mess = "Burgerservicenummer is niet correct, deze wijziging wordt niet verwerkt.";
			
		} elseif ($p_kolom == "RelnrRedNed" and strlen($p_waarde) != 7 and $p_waarde != "NULL" and strlen($p_waarde) > 0) {
			$this->mess = "Het Sportlink ID moet 7 karakters lang zijn, deze wijziging wordt niet verwerkt.";
			$this->tm = 1;
			
		} else {
			
			if ($this->is_kolom_tekst($p_kolom) == true and $p_waarde == "NULL") {
				$p_waarde = "";
			}
			
			$this->pdoupdate($this->lidid, $p_kolom, $p_waarde, $p_reden);
		}
		
		if ($this->tm > 0) {
			$rv = $this->mess;
		} else {
			$rv = "";
		}
		
		$this->log($this->lidid);
		
		return $rv;

	}  # update
	
	private function delete($p_lidid, $p_reden="") {
		$this->vulvars($p_lidid);
		
		if ($this->pdodelete($p_lidid, $p_reden)) {
			$this->log();
		}
		
	}  # cls_Lid->delete
	
	public function controle($p_lidid=-1) {
		if ($p_lidid > 0) {
			$f = sprintf("L.RecordID=%d", $p_lidid);
		} else {
			$f = "";
		}
		$lrows = $this->basislijst($f);
		foreach ($lrows as $lrow) {
			if (strlen($lrow->Email) > 0 and strlen($lrow->EmailOuders) > 0 and strtolower($lrow->Email) == strtolower($lrow->EmailOuders)) {
				$this->update($lrow->RecordID, "Email", "", "het emailadres gelijk is aan die van de ouders.");
			} elseif (array_key_exists($lrow->Geslacht, ARRGESLACHT) == false) {
				$this->update($lrow->RecordID, "Geslacht", "O", "geslacht een ongeldige waarde had.");
			} elseif (strlen($lrow->Roepnaam) > 1 and strlen($lrow->Voorletter) == 0 and $lrow->Geslacht != "B" and substr($lrow->Roepnaam, 0, 1) >= "A" and substr($lrow->Roepnaam, 0, 1) <= "Z") {
				$vl = strtoupper(substr($lrow->Roepnaam, 0, 1)) . ".";
				$this->update($lrow->RecordID, "Voorletter", $vl, "de voorletters leeg waren.");
			} elseif (array_key_exists($lrow->Legitimatietype, ARRLEGITIMATIE) == false) {
				$this->update($lrow->RecordID, "Legitimatietype", "G", "legitimatietype een ongeldige waarde had.");
			} elseif (strlen($lrow->Postcode) < 4 and (strlen($lrow->Adres) > 0 or strlen($lrow->Huisnr) > 0)) {
				$this->update($lrow->RecordID, "Adres", "", "de postcode leeg is");
				$this->update($lrow->RecordID, "Huisnr", "NULL", "de postcode leeg is");
			} elseif (strlen($lrow->Postcode) < 4 and strlen($lrow->Telefoon) > 0) {
				$this->update($lrow->RecordID, "Telefoon", "", "de postcode leeg is");
			} elseif (strlen($lrow->Toevoeging) > 1 and substr($lrow->Toevoeging, 0, 1) == "-") {
				$this->update($lrow->RecordID, "Toevoeging", substr($lrow->Toevoeging, 1, 4), "de toevoeging hoort niet met een streepje hoort te beginnen.");
			} elseif ($lrow->Overleden > "1900-01-01" and $lrow->Overleden < date("Y-m-d", strtotime("-3 month")) and (strlen($lrow->Postcode) > 0 or strlen($lrow->Email) > 0 or strlen($lrow->Mobiel) > 0)) {
				$reden = "de persoon langer dan 3 maanden geleden is overleden";
				$this->update($lrow->RecordID, "Postcode", "", $reden);
				$this->update($lrow->RecordID, "Woonplaats", "", $reden);
				$this->update($lrow->RecordID, "Email", "", $reden);
				$this->update($lrow->RecordID, "EmailVereniging", "", $reden);
				$this->update($lrow->RecordID, "EmailOuders", "", $reden);
				$this->update($lrow->RecordID, "Mobiel", "", $reden);
			}
		}
		
	}  # cls_Lid->controle
	
	public function opschonen() {
		$i_lm = new cls_Lidmaatschap();
		$i_lo = new cls_Lidond();

		// Definitief verwijderen leden met verwijder-markering
		$bt = $_SESSION['settings']['liddefinitiefverwijderen'] ?? 0;
		if ($bt > 0) {
			$reden = sprintf("de verwijdermarkering voor %s is gezet.", date("d-m-Y", strtotime(sprintf("-%d month", $bt))));
			$query = sprintf("SELECT L.RecordID FROM %s WHERE IFNULL(L.Verwijderd, '9999-12-31') < DATE_SUB(CURDATE(), INTERVAL %d MONTH)", $this->basefrom, $bt);
			foreach ($this->execsql($query)->fetchAll() as $lrow) {
				$delqry = sprintf("DELETE FROM %sLidmaatschap WHERE Lid=%d;", TABLE_PREFIX, $lrow->RecordID);
				$this->execsql($delqry, 6);
				
				$delqry = sprintf("DELETE FROM %sLidond WHERE Lid=%d;", TABLE_PREFIX, $lrow->RecordID);
				$this->execsql($delqry, 6);
				
				$delqry = sprintf("DELETE FROM %sFoto WHERE LidID=%d;", TABLE_PREFIX, $lrow->RecordID);
				$this->execsql($delqry, 6);
				
				$delqry = sprintf("DELETE FROM %sEvenement_Deelnemer WHERE LidID=%d;", TABLE_PREFIX, $lrow->RecordID);
				$this->execsql($delqry, 7);
				
				$this->delete($lrow->RecordID, $reden);
			}
		}
		
		$bt = $_SESSION['settings']['ledenopschonen'] ?? 0;
		if ($bt > 0 and 1 == 2) {
			$gd = date("Y-m-d", strtotime(sprintf("-%d month", $bt)));
			$query = sprintf("SELECT L.RecordID FROM %s INNER JOIN %sLidmaatschap AS LM ON LM.Lid=L.RecordID WHERE L.NietOpschonen=0 AND (L.Verwijderd IS NULL) AND IFNULL(LM.Opgezegd, '9999-12-31') < '%s';", $this->basefrom, TABLE_PREFIX, $gd);
			$reden = sprintf("het lidmaatschap voor %s is beëindigd.", date("d-m-Y", strtotime(sprintf("-%d month", $bt))));
			
			foreach ($this->execsql($query)->fetchAll() as $lrow) {
				
				if ($i_lm->eindelidmaatschap($lrow->RecordID) < $gd) {
					$i_lo->where = sprintf("LO.Lid=%d", $lrow->RecordID);
					foreach ($i_lo->basislijst() as $lorow) {
						$i_lo->delete($lorow->RecordID, $reden);
					}
				
					$i_lm->delete(-1, $lrow->RecordID, $reden);
					
					$this->update($lrow->RecordID, "Verwijderd", date("Y-m-d"), $reden);
				}
			}
		}
		
		$this->optimize();
	}  # cls_Lid->opschonen
	
}  # cls_Lid

class cls_Lidmaatschap extends cls_db_base {
	
	public int $lmid = 0;
	public int $lidnr = 0;
	public string $lidvanaf = "";
	public string $lidtm = "";
	public string $opgezegdper = "";
	public string $soortlid = "";
	
	public object $i_lid;
	
	function __construct($p_lmid=-1, $p_lidid=-1, $p_lidnr=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Lidmaatschap";
		$this->basefrom = $this->table . " AS LM";
		$this->per = date("Y-m-d");
		$this->vulvars($p_lmid, $p_lidid, $p_lidnr);
		$this->ta = 6;
		$this->tas = 8;
	}
	
	public function vulvars($p_lmid, $p_lidid=-1, $p_lidnr=-1) {
		global $dtfmt;
		
		if ($p_lmid >= 0) {
			$this->lmid = $p_lmid;
		}
		
		if ($this->lmid > 0) {
			$query = sprintf("SELECT LM.* FROM %s WHERE LM.RecordID=%d;", $this->basefrom, $this->lmid);
			$lmrow = $this->execsql($query)->fetch();
			if (!isset($lmrow->RecordID)) {
				$this->lmid = 0;
			}
		}
		
		if ($this->lmid <= 0 and $p_lidid >= 0) {
			$f = sprintf("LM.Lid=%d AND IFNULL(LM.Opgezegd, '9999-12-31') >= '%s'", $p_lidid, $this->per);
			$this->lmid = $this->max("RecordID", $f);
			if ($this->lmid <= 0) {
				$f = sprintf("LM.Lid=%d", $this->lidid);
				$this->lmid = $this->max("RecordID", $f);
			}
		} elseif ($p_lidnr > 0) {
			$this->lidnr = $p_lidnr;
			$query = sprintf("SELECT IFNULL(LM.RecordID, 0) FROM %s WHERE LM.Lidnr=%d;", $this->basefrom, $this->lidnr);
			$this->lmid = $this->scalar($query);
		}
		
		$this->lidnr = 0;
		$this->lidvanaf = "";
		if ($this->lmid > 0) {
			$query = sprintf("SELECT LM.* FROM %s WHERE LM.RecordID=%d;", $this->basefrom, $this->lmid);
			$row = $this->execsql($query)->fetch();
			if (isset($row->RecordID)) {
				$this->lidid = $row->Lid ?? 0;
				$this->lidnr = $row->Lidnr ?? 0;
				$this->lidvanaf = $row->LIDDATUM ?? "";
				if (strlen($row->Opgezegd) < 10) {
					$this->lidtm = "9999-12-31";
				} else {
					$this->lidtm = $row->Opgezegd;
				}
				$this->ingevoerd = $lmrow->Ingevoerd ?? "";
				$this->gewijzigd = $lmrow->Gewijzigd ?? "";
			}
		}
		
		if ($this->lidtm > "2000-01-01") {
			$dtfmt->setPattern(DTTEXT);
			$op = new datetime($this->lidtm);
			$op->modify("+1 day");
			$this->opgezegdper = $dtfmt->format($op);
		} else {
			$this->opgezegdper = "";
		}
		
		$this->naamlogging = $this->lidnr;
		
	}  # cls_Lidmaatschap->vulvars
	
	public function lidid($p_lmid, $p_lidnr=0) {
		$this->vulvars($p_lmid, -1, $p_lidnr);
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
	}  # cls_Lidmaatschap->islid
	
	public function soortlid($p_lidid=-1, $p_per="") {
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		if (strlen($p_per) < 10) {
			$p_per = $this->per;
		}
		
		if ($p_lidid > 0) {			
			$rv = "Lid";
			$query = sprintf("SELECT LM.* FROM %s WHERE LM.Lid=%d ORDER BY LM.LIDDATUM DESC;", $this->basefrom, $this->lidid);
			
			$result = $this->execsql($query);
			$row = $result->fetch();
			if (isset($row->Lid) and $row->Lid > 0) {
				$this->lmid = $row->RecordID;
				if ($row->Opgezegd > '1900-01-01' and $row->Opgezegd < $p_per) {
					$rv = "Voormalig lid";
				} elseif ($row->LIDDATUM > $p_per) {
					$rv = "Toekomstig lid";
				}
			} else {
				$query = sprintf("SELECT IFNULL(L.RecordID, 0) FROM %sLid AS L WHERE L.RecordID=%d AND (L.Verwijderd IS NULL);", TABLE_PREFIX, $this->lidid);
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
	}  # cls_Lidmaatschap->soortlid
	
	public function overzichtlid($p_lidid=-1) {
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		$this->query = sprintf("SELECT LM.Lidnr, LM.LIDDATUM, LM.Opgezegd, CONCAT(FORMAT(DATEDIFF(IFNULL(LM.Opgezegd, CURDATE()), LM.LIDDATUM)/365.25, 1, 'nl_NL'), ' jaar') AS Duur
										FROM %s WHERE LM.Lid=%d ORDER BY LM.LIDDATUM;", $this->basefrom, $this->lidid);
		$result = $this->execsql();
		return $result->fetchAll();
	}  # cls_Lidmaatschap->overzichtlid
	
	public function eindelidmaatschap($p_lidid=-1) {
		if ($p_lidid > 0) {
			$this->lidid = $p_lidid;
		}
		$query = sprintf("SELECT MAX(IFNULL(LM.Opgezegd, '9999-12-31')) FROM %s WHERE LM.Lid=%d;", $this->basefrom, $this->lidid);
		
		return $this->scalar($query);
		
	}  # cls_Lidmaatschap->eindelidmaatschap
	
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
		$query = sprintf("INSERT INTO %s (RecordID, Lid, Lidnr, LIDDATUM, OpgezegdDoorVereniging, Ingevoerd) VALUES (%d, %d, %d, CURDATE(), 0, NOW());", $this->table, $nrid, $this->lidid, $lidnr);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Lidmaatschap %d met lidnummer %d is toegevoegd.", $nrid, $lidnr);	

			$query = sprintf("INSERT INTO %s (RecordID, Lid, Lidnr, LIDDATUM, Ingevoerd) VALUES (%d, %d, %d, CURDATE(), NOW());", $this->table, $nrid, $this->lidid, $lidnr);
			$this->Interface($query);
		}
		$this->Log($nrid);
		
		if ($p_vanaf > "2000-01-01" and strlen($p_vanaf) == 10) {
			$this->update($nrid, "LIDDATUM", $p_vanaf);
		}
	}  # cls_Lidmaatschap->add

	public function update($p_lmid, $p_kolom, $p_waarde) {
		$this->vulvars($p_lmid);
		$this->tm = 0;

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
	
	public function delete($p_lmid=-1, $p_lidid=-1, $p_reden="") {
		$query = sprintf("SELECT LM.RecordID, LM.Lid FROM %s WHERE ", $this->basefrom);
		if ($p_lidid > 0) {
			$query .= sprintf("LM.Lid=%d;", $p_lidid);
		} else {
			$query .= sprintf("LM.RecordID=%d;", $p_lmid);
		}
		foreach ($this->execsql($query)->fetchAll() as $lmrow) {
			$this->lidid = $lmrow->Lid;
			if ($this->pdodelete($lmrow->RecordID, $p_reden)) {
				$this->log($lmrow->RecordID);
			}
		}

	}  # cls_Lidmaatschap->delete
	
	public function opzegging($p_lidid, $p_per) {
		$this->lidid = $p_lidid;
		
		if (strlen($p_per) > 8) {
			$this->query = sprintf("SELECT LM.RecordID FROM %s WHERE LM.Lid=%d AND (LM.Opgezegd IS NULL);", $this->basefrom, $this->lidid);
			foreach ($this->execsql()->fetchAll() as $row) {
				$this->update($row->RecordID, "Opgezegd", $p_per);
			}
		}
	}
	
	public function controle() {
	}  # cls_Lidmaatschap->controle
	
	public function opschonen() {
		
		$query = sprintf("SELECT LM.RecordID FROM %s WHERE (LM.Lid NOT IN (SELECT L.RecordID FROM %sLid AS L WHERE (L.Verwijderd IS NULL)));", $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->lidid = $row->Lid;
			$this->pdodelete($row->RecordID, "het gerelateerde record in de tabel Lid niet (meer) bestaat.");
			$this->Log($row->RecordID);
		}
		
		$query = sprintf("SELECT LM.RecordID, LM.Lid FROM %s WHERE LM.LIDDATUM > IFNULL(LM.Opgezegd, '9999-12-31');", $this->basefrom);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->lidid = $row->Lid;
			$this->pdodelete($row->RecordID, "de datum van lid worden na de datum van opzeggen ligt");
			$this->Log($row->RecordID);
		}
		
		$this->optimize();
		
	}  # cls_Lidmaatschap->opschonen
	
}  # cls_Lidmaatschap

class cls_Memo extends cls_db_base {
	private $mmid = 0;
	private $soort = "";
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Memo";
		$this->basefrom = $this->table . " AS M";
		$this->ta = 6;
		$this->tas = 7;
		$this->per = date("Y-m-d");
	}
	
	private function vulvars($p_lidid, $p_soort, $p_mmid=-1) {
		$this->lidid = $p_lidid;
		$this->soort = $p_soort;
		
		if ($p_mmid > 0) {
			$this->mmid = $p_mmid;
			$query = sprintf("SELECT M.* FROM %s WHERE M.RecordID=%d;", $this->basefrom, $this->mmid);
			$mrow = $this->execsql($query)->fetch();
			if (isset($mrow->Lid)) {
				$this->lidid = $mrow->Lid;
				$this->soort = $mrow->soort;
			} else {
				$this->mmid = 0;
			}
		
		} elseif ($this->lidid > 0 and strlen($this->soort) > 0) {
			$f = sprintf("Lid=%d AND Soort='%s'", $this->lidid, $this->soort);
			$this->mmid = $this->max("M.RecordID", $f);
		}
	}  # cls_Memo->vulvars

	public function inhoud($p_lidid, $p_soort) {
		$this->lidid = $p_lidid;
		$query = sprintf("SELECT Memo FROM %s WHERE Lid=%d AND Soort='%s';", $this->table, $p_lidid, $p_soort);
		return $this->scalar($query);
	}
	
	public function overzichtlid($p_lidid) {
		$this->lidid = $p_lidid;
		
		$query = sprintf("SELECT Soort, Memo FROM %s WHERE Lid=%d;", $this->table, $p_lidid);
		return $this->execsql($query)->fetchAll();		
	}  # cls_Memo->overzichtlid
	
	public function add($p_lidid, $p_soort, $p_waarde) {
		$this->lidid = $p_lidid;
		$nrid = $this->nieuwrecordid();
		
		$query = sprintf("INSERT INTO %s (RecordID, Lid, Soort, Memo, Ingevoerd) VALUES (%d, %d, '%s', \"%s\", NOW());", $this->table, $nrid, $p_lidid, $p_soort, $p_waarde);
		if ($this->execsql($query) > 0) {
			$query = sprintf("INSERT INTO Memo (RecordID, Lid, Soort, Memo, Ingevoerd) VALUES (%d, %d, '%s', \"%s\", NOW());", $nrid, $p_lidid, $p_soort, $p_waarde);
			$this->interface($query);
			$this->mess = sprintf("Memo '%s' met waarde '%s' is toegevoegd.", ARRSOORTMEMO[$p_soort], $p_waarde);
			$this->log($nrid);
		}
	}  # cls_Memo->add
	
	public function update($p_lidid, $p_soort, $p_waarde) {
		$this->vulvars($p_lidid, $p_soort);
		
		$p_waarde = str_replace("\"", "'", $p_waarde);
		
		if ($this->pdoupdate($this->mmid, "Memo", $p_waarde) > 0) {
			$this->log($this->mmid);
		}
	}
	
	public function delete($p_lidid, $p_soort, $p_mmid=-1, $p_reden="") {
		$this->vulvars($p_lidid, $p_soort, $p_mmid);
		
		if ($this->pdodelete($this->mmid, $p_reden)) {
			$this->log($this->mmid);
		}
	}  # cls_Memo->delete
	
	public function controle() {
	}
	
	public function opschonen() {
		
		$query = sprintf("SELECT M.RecordID FROM %s WHERE M.Ingevoerd < DATE_SUB(CURDATE(), INTERVAL %d DAY) AND LENGTH(M.Memo)=0;", $this->basefrom, BEWAARTIJDNIEUWERECORDS);
		foreach ($this->execsql($query)->fetchAll() as $mrow) {
			$this->delete(-1, -1, $mrow->RecordID, "het memo leeg is.");
		}
		
		$query = sprintf("SELECT M.RecordID FROM %s WHERE (M.Lid NOT IN (SELECT L.RecordID FROM %sLid AS L WHERE (L.Verwijderd IS NULL)));", $this->basefrom, TABLE_PREFIX);
		foreach ($this->execsql($query)->fetchAll() as $mrow) {
			$this->delete(-1, -1, $mrow->RecordID, "het lid niet (meer) bestaat.");
		}
		
		$this->optimize();
		
	}  # cls_Memo->opschonen
	
}  # cls_Memo

class cls_Authorisation extends cls_db_base {
	
	public $aid = 0;
	public $naamtp = "";
	public $ondid = 0;
	public $ondnaam = "";
	public $toegang = -1;
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Admin_access";
		$this->basefrom = $this->table . " AS AA";
		$this->per = date("Y-m-d");
		$this->ta = 15;
	}
	
	private function vulvars($p_aid=-1, $p_tabpage="") {
		$this->aid = $p_aid;
		
		if ($this->aid <= 0 and strlen($p_tabpage) > 0) {
			$query = sprintf("SELECT AA.RecordID FROM %s WHERE AA.Tabpage='%s';", $this->basefrom, $p_tabpage);
			$rows = $this->execsql($query)->fetchAll();
			if (count($rows) == 1) {
				$this->aid = $rows[0]->RecordID;
			}
		}

		if ($this->aid > 0) {	
			$query = sprintf("SELECT AA.* FROM %s WHERE AA.RecordID=%d;", $this->basefrom, $this->aid);
			$row = $this->execsql($query)->fetch();
			$this->naamtp = $row->Tabpage ?? "";
			$this->naamlogging = $this->naamtp;
			$this->toegang = $row->Toegang ?? 0;
			$this->ingevoerd = $row->Ingevoerd ?? "";
			if ($row->Toegang == -2) {
				$this->ondnaam = "Niemand";
			} elseif ($row->Toegang == -1) {
				$this->ondnaam = "Alleen webmasters";
			} elseif ($row->Toegang == 0) {
				$this->ondnaam = "Iedereen";
			} else {
				$this->ondid = $row->Toegang;
				$this->ondnaam = (new cls_Onderdeel())->Naam($row->Toegang);
			}
		}
	}  # cls_Authorisation->vulvars
	
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
		
		$w = "";
		if (strlen($this->where) > 0) {
			$w = sprintf(" WHERE %s", $this->where);
		}
		
		$query = sprintf("SELECT %s RecordID, Toegang, Tabpage, Ingevoerd, LaatstGebruikt FROM %s%s ORDER BY Tabpage;", $p_distinct, $this->basefrom, $w);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function toegang($p_tabpage, $p_aid=-1, $p_alleenmenu=0) {
		$rv = false;
		
		if ($p_aid > 0) {
			$this->vulvars($p_aid);
			$p_tabpage = $this->naamtp;
		} elseif (strlen($p_tabpage) > 0) {
			$this->vulvars(-1, $p_tabpage);
		}
		
		if (!isset($_SESSION['lidgroepen']) or strlen($_SESSION['lidgroepen']) <= 2) {
			$_SESSION['lidgroepen'] = (new cls_Lidond())->lidgroepen();
		}
		
		if (!isset($_SESSION['lidauth']) or !is_array($_SESSION['lidauth']) or count($_SESSION['lidauth']) < 5) {
			$query = sprintf("SELECT DISTINCT AA.Tabpage FROM %s WHERE AA.Toegang IN (%s);", $this->basefrom, $_SESSION['lidgroepen']);
			$_SESSION['lidauth'] = null;
			foreach ($this->execsql($query)->fetchAll() as $row) {
				$_SESSION['lidauth'][] = $row->Tabpage;
			}
		}
		
		if (WEBMASTER) {
			if (isset($_SESSION['lidauth']) and in_array($p_tabpage, $_SESSION['lidauth']) == false and $_SERVER['PHP_SELF'] != "/admin.php") {
				$query = sprintf("SELECT IFNULL(MIN(AA.RecordID), 0) FROM %s WHERE AA.Tabpage='%s';", $this->basefrom, $p_tabpage);
				$aid = $this->scalar($query);
				if (strlen($p_tabpage) > 0 and $aid == 0) {
					$this->aid = $this->add($p_tabpage);
				}
				$_SESSION['lidauth'][] = $p_tabpage;
			}
			$query = sprintf("SELECT MAX(AA.Toegang) FROM %s WHERE AA.Tabpage='%s';", $this->basefrom, $p_tabpage, $_SESSION['lidgroepen']);
			if ($this->scalar($query) == -2) {
				$rv = false;
			} else {
				$rv = true;
			}
		} else {
			if (in_array($p_tabpage, $_SESSION['lidauth'])) {
				$rv = true;
			} else {
				$query = sprintf("SELECT IFNULL(MIN(AA.RecordID), 0) FROM %s WHERE AA.Tabpage='%s' AND AA.Toegang IN (%s);", $this->basefrom, $p_tabpage, $_SESSION['lidgroepen']);
				$aid = $this->scalar($query);
				if ($aid > 0) {
					$rv = true;
				}
			}
		}
		
		if ($rv and $this->aid > 0 and $p_alleenmenu == 0) {
			$this->update($this->aid, "LaatstGebruikt", date("Y-m-d"));
		}
		
		return $rv;
		
	}  # cls_Authorisation->toegang
	
	public function autorisatiesperonderdeel() {
		$query = sprintf("SELECT O.Naam AS Onderdeel, A.Tabpage as `Toegang tot` FROM %sAdmin_access AS A INNER JOIN %1\$sOnderdl AS O ON A.Toegang=O.RecordID ORDER BY O.Naam, A.Tabpage;", TABLE_PREFIX);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function add($p_tabpage) {
		$this->tas = 1;
		
		if (WEBMASTER) {
			$p_tabpage = str_replace("'", "", $p_tabpage);
			$nrid = $this->nieuwrecordid();
			$query = sprintf("INSERT INTO %s (RecordID, Tabpage, Toegang) VALUES (%d, \"%s\", -1);", $this->table, $nrid, $p_tabpage);
			if ($this->execsql($query) > 0) {
				$this->mess = sprintf("Autorisatie %d (%s) is toegevoegd.", $nrid, $p_tabpage);
				$this->log($nrid);
			} else {
				$nrid = false;
			}
			$query = sprintf("UPDATE %s SET Toegang=0 WHERE Tabpage='Vereniging/Introductie' AND Toegang<>0;", $this->table);
			$this->execsql($query);
			return $nrid;
		} else {
			$this->mess = "Je bent niet bevoegd om autorisaties toe te voegen.";
			$this->log();
			return false;
		}
		
	}  # cls_Authorisation->add

	public function update($p_aid, $p_kolom, $p_waarde) {
		$this->vulvars($p_aid);
		$this->tas = 2;
		if (WEBMASTER == false and $p_kolom != "LaatstGebruikt") {
			$this->mess = "Je bent niet bevoegd om autorisaties aan te passen.";
		} elseif ($this->pdoupdate($p_aid, $p_kolom, $p_waarde)) {
			if ($p_kolom == "LaatstGebruikt") {
				$this->mess = "";
			}
		}
		$this->log($p_aid, 0, $this->ondid);
	}
	
	public function delete($p_aid, $p_reden="") {
		$this->tas = 3;
		$this->vulvars($p_aid);
		
		if (WEBMASTER) {
			$this->pdodelete($p_aid, $p_reden);
		} else {
			$this->mess = "Je bent niet bevoegd om autorisaties te verwijderen.";
		}
		$this->log($p_aid, 0, $this->ondid);
	}  # cls_Authorisation->delete
	
	public function controle() {
		
	}  # cls_Authorisation->controle
	
	public function opschonen() {
		$query = sprintf("SELECT RecordID FROM %s WHERE AA.Ingevoerd < DATE_SUB(CURDATE(), INTERVAL %d DAY) AND Toegang=-1 AND AA.LaatstGebruikt < DATE_SUB(CURDATE(), INTERVAL 3 MONTH);", $this->basefrom, BEWAARTIJDNIEUWERECORDS);
		$result = $this->execsql($query);
		$reden = "hij 3 maanden lang niet is gebruikt en toegang is alleen voor webmasters.";
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, $reden);
		}
		
		$this->optimize();
		
	}  # cls_Authorisation->opschonen
	
}  # cls_Authorisation

class cls_Onderdeel extends cls_db_base {
	public int $oid = 0;
	public string $naam = "";
	public string $code = "";
	public string $type = "";
	public string $typeoms = "";
	public string $email = "";
	public int $alleenleden = 0;		// Mogen van dit onderdeel alleen mensen lid zijn, die ook lid van de vereniging zijn?
	public $iskader = false;			// Zijn de leden van dit onderdeel kaderlid?
	public $isautogroep = false;		// Wordt deze groep automatisch bijgewerkt op basis van MySQL-code of een Eigen query in de Access-database.
	public int $ledenmuterendoor = 0;	// Door welke extra groep mogen de leden van dit onderdeel worden gemuteerd?
	private $mogelijketypes = "";
	public $aantalrijen = 0;			// Het aantal leden dat dit onderdeel op dit moment heeft
	public $organisatie = 0;

	function __construct($p_oid=-1) {
		$this->table = TABLE_PREFIX . "Onderdl";
		$this->basefrom = $this->table . " AS O";
		$this->per = date("Y-m-d");
		$this->vulvars($p_oid);
		foreach (ARRTYPEONDERDEEL as $k => $v) {
			$this->mogelijketypes .= $k;
		}
	}

	public function vulvars($p_oid) {
		$this->ta = 20;
		if ($p_oid >= 0) {
			$this->oid = $p_oid;
		}
		
		if ($this->oid > 0) {
			$query = sprintf("SELECT O.* FROM %s WHERE O.RecordID=%d;", $this->basefrom, $this->oid);
			$result = $this->execsql($query);
			$row = $result->fetch();
		
			if (isset($row->RecordID)) {
				$this->naam = trim($row->Naam ?? "");
				$this->naamlogging = $this->naam;
				$this->code = trim($row->Kode ?? "");
				$this->type = $row->Type ?? "G";
				$this->typeoms = ARRTYPEONDERDEEL[$this->type];
				$this->email = $row->CentraalEmail ?? "";
				$this->organisatie = $row->ORGANIS ?? 0;
				$this->alleenleden = $row->{'Alleen leden'} ?? 0;
				$this->ledenmuterendoor = $row->LedenMuterenDoor ?? 0;
				$this->ingevoerd = $row->Ingevoerd ?? "";
				$this->gewijzigd = $row->Gewijzigd ?? "";
				if ($row->Kader == 1) {
					$this->iskader = true;
				} else {
					$this->iskader = false;
				}

				if (strlen($row->MySQL) > 10) {
					$this->isautogroep = true;
				} else {
					$this->isautogroep = false;
				}
			
				$query = sprintf("SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.OnderdeelID=%d AND LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE();", TABLE_PREFIX, $this->oid);
				$this->aantalrijen = $this->scalar($query);
			} else {
				$this->oid = 0;
			}
		}
	}  # cls_Onderdeel->vulvars
	
	public function naam($p_oid, $p_riz="", $p_maxlen=99) {
		$this->vulvars($p_oid);
		
		if (strlen($this->naam) > 0) {
			$rv = $this->naam;
			if (strlen($rv) > $p_maxlen) {
				$rv = $this->code;
			}
		} else {
			$rv = $p_riz;
		}
		
		if (strlen($rv) > $p_maxlen) {
			$rv = substr($rv, 0, $p_maxlen-4) . " ...";
		}
		
		return $rv;
	}  # cls_Onderdeel->naam
	
	public function autogroep($p_oid) {
		$this->vulvars($p_oid);
		return $this->isautogroep;
	}
	
	public function record($p_oid=-1, $p_kode="") {
		if ($p_oid > 0) {
			$this->oid = $p_oid;
			
		} elseif (strlen($p_kode) > 0) {
			$query = sprintf("SELECT IFNULL(MIN(O.RecordID), 0) FROM %s WHERE UPPER(O.Kode)='%s';", $this->basefrom, strtoupper($p_kode));
			$id = $this->scalar($query);
			if ($id > 0) {
				$this->oid = $id;
			}
		}
		
		$query = sprintf("SELECT O.* FROM %s WHERE O.RecordID=%d;", $this->basefrom, $this->oid);
		
		$result = $this->execsql($query);
		$row = $result->fetch();
		if (isset($row->RecordID)) {
			$this->vulvars($this->oid);
			return $row;
		} else {
			return false;
		}
	}
	
	public function lijst($p_ingebruik=1, $p_filter="", $p_per="", $p_lidid=0, $p_orderby="", $p_fetched=1) {
		
		if (strlen($p_per) < 10) {
			$p_per = $this->per;
		}
		$sqal = sprintf("SELECT COUNT(*) FROM %sLidond AS LO WHERE %s AND LO.OnderdeelID=O.RecordID", TABLE_PREFIX, str_replace("CURDATE()", "'" . $p_per . "'", cls_db_base::$wherelidond));
		
		$filter = "";
		if ($p_ingebruik > 0) {
			if (WEBMASTER == false and $p_lidid > 0) {
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
		if (strlen($this->where) > 0) {
			if (strlen($filter) > 0) {
				$filter .= " AND ";
			} else {
				$filter = " WHERE ";
			}
			$filter .= $this->where;
		}
		
		if (strlen($p_orderby) > 0) {
			$orderby = $p_orderby . ", O.Naam";
		} else {
			$orderby = "O.Naam";
		}
		
		$query = sprintf("SELECT O.*, CONCAT(O.Kode, ' - ', O.Naam) AS Oms, (%s) AS AantalLeden FROM %s %s ORDER BY %s;", $sqal, $this->basefrom, $filter, $orderby);
		$result = $this->execsql($query);
		
		if ($p_fetched == 1) {
			return $result->fetchAll();
		} else {
			return $result;
		}

	}  # cls_Onderdeel->lijst
	
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
	}  # cls_Onderdeel->editlijst
	
	public function htmloptions($p_cv=-1, $p_ingebruik=1, $p_ondtype="", $p_aant=1) {
		/*
			$p_ingebruik: 1 = toon alleen onderdelen die op dit moment leden heeft
			$p_aant: 1 toon het aantal leden in het onderdeel
		*/
		
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
			if ($row->AantalLeden > 1 and $p_aant == 1) {
				$o .= sprintf(" (%d leden)", $row->AantalLeden);
			}
			$ret .= sprintf("<option%s value=%d>%s</option>\n", checked($row->RecordID, "option", $p_cv), $row->RecordID, $o);
		}
		return $ret;
	}  # cls_Onderdeel->htmloptions

	public function islid($p_ondid, $p_lidid) {
		$query = sprintf("SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.Lid=%d AND LO.OnderdeelID=%d AND LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE();", TABLE_PREFIX, $p_lidid, $p_ondid);
		if ($this->scalar($query) > 0) {
			return true;
		} else {
			return false;
		}
	}
	
	public function add($p_type="G", $p_code="") {
		global $dbc;
		
		$this->oid = 0;
		$this->tas = 11;
		$this->tm = 1;
		$nrid = $this->nieuwrecordid();
		if (strlen($p_type) == 0 or $p_type == "*") {
			$p_type = "G";
		}
		if (strlen($p_code) < 3) {
			$p_code = sprintf("%s_%d", $p_type, $nrid);
		}
		$query = sprintf("SELECT COUNT(*) FROM %s WHERE Kode='%s';", $this->table, $p_code);
		if ((new cls_db_base())->scalar($query) > 0) {
			$this->mess = sprintf("De code '%s' is al in gebruik. Deze %s wordt niet toegevoegd.", $p_code, strtolower(ARRTYPEONDERDEEL[$p_type]));
			$nrid = 0;
			
		} elseif (strpos($this->mogelijketypes, $p_type) === false) {
			$this->mess = sprintf("Type '%s' is niet correct. Dit onderdeel wordt niet toegevoegd.", $p_type);
			$nrid = 0;
			
		} else {
			$data['recordid'] = $nrid;
			$data['kode'] = $p_code;
			$data['type'] = $p_type;
			$data['naam'] = "*** Nieuw *" . $p_code . " ***";
			
			$query = sprintf("INSERT INTO %s (RecordID, Kode, Type, Naam, Kader, `Alleen leden`, GekoppeldAanQuery) VALUES (:recordid, :kode, :type, :naam, 0, 0, 0);", $this->table);
			if ($dbc->prepare($query)->execute($data) > 0) {	
				$this->mess = sprintf("Onderdeel %d met code '%s' is toegevoegd.", $nrid, $p_code);
				$this->interface($query);
			}
		}
		$this->log($nrid, 1);
		
		return $nrid;
	}  # cls_Onderdeel->add

	public function update($p_oid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_oid);
		$this->tas = 12;
		
		if ($p_kolom == "VervallenPer" and strlen($p_waarde) < 8) {
			$p_waarde = "";
		
		} elseif ($p_kolom == "Naam") {
			$p_waarde = str_replace("\"", "'", $p_waarde);
			
		} elseif ($p_kolom == "MySQL" and strlen($p_waarde) > 10) {
			if (startwith($p_waarde, "SELECT")) {
				$p_waarde = str_replace("\"", "'", $p_waarde);
				if ($this->pdoupdate($this->oid, "Alleen leden", 0, "de MySQL is ingevuld.")) {
					$this->log($this->oid);
				}
			} else {
				$this->mess = sprintf("De MySQL-code moet met SELECT beginnen. Kolom '%s' is leeg gemaakt.", $p_kolom);
				$this->log($this->oid);
				$p_waarde = "";
			}
		}

		if ($p_kolom == "Type" and (strlen($p_waarde) == 0 or strpos($this->mogelijketypes, $p_waarde) === false)) {
			$this->mess = sprintf("Type wordt niet bijgewerkt, omdat de waarde (%s) niet juist is.", $p_waarde);
			
		} else {
			$this->pdoupdate($this->oid, $p_kolom, $p_waarde, $p_reden);
		}

		$this->log($this->oid);
	}
	
	public function delete($p_oid, $p_reden="") {
		$this->vulvars($p_oid);
		$this->tas = 13;
		
		if ($this->pdodelete($this->oid) > 0) {
			$this->mess = sprintf("%s (%d) is verwijderd", $this->naam, $this->oid);
			if (strlen($p_reden) > 0) {
				$this->mess .= ", omdat " . $p_reden;
			}
			$this->log($this->oid);
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
				if ($c == "Kad") {
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
			} elseif ($row->VervallenPer > "2000-01-01" and $row->VervallenPer < date("Y-m-d") and strlen($row->CentraalEmail) > 0) {
				$this->update($row->RecordID, "CentraalEmail", "", "het onderdeel vervallen is.");
			} elseif (strlen($row->MySQL) > 0 and (strlen($row->MySQL) <= 10 or substr($row->MySQL, 0, 6) != "SELECT")) {
				$this->update($row->RecordID, "MySQL", "", "de mySQL-code niet correct is");
			} elseif (strpos("EOT", $row->Type) !== false and $row->Kader == 1) {
				$reden = "bij een eigenschap, onderscheiding of toestemming mag kader geen ja zijn";
				$this->update($row->RecordID, "Kader", 0, $reden);
			} elseif (strlen($row->MySQL) > 5 and $row->{'Alleen leden'} > 0) {
				$reden = "als er MySQL code is ingevuld, alleen leden niet aan mag staan.";
				$this->update($row->RecordID, "Alleen leden", 0, $reden);
			}
		}
	}  # cls_Onderdeel->controle
	
	public function opschonen() {
		$query = sprintf("SELECT O.RecordID FROM %1\$s WHERE IFNULL(O.VervallenPer, '9999-12-31') < CURDATE() AND IFNULL(O.Ingevoerd, CURDATE()) < DATE_SUB(CURDATE(), INTERVAL %2\$d DAY) AND
								(O.RecordID NOT IN (SELECT LO.OnderdeelID FROM %3\$sLidond AS LO));", $this->basefrom, BEWAARTIJDNIEUWERECORDS, TABLE_PREFIX);
		$result = $this->execsql($query);
		$reden = "deze vervallen is en nergens meer aan gekoppeld is.";
		foreach ($result->fetchAll() as $row) {
			$f = sprintf("DP.Afdelingsspecifiek=%d", $row->RecordID);
			if ((new cls_Diploma())->aantal($f) == 0) {
				$f = sprintf("EX.OnderdeelID=%d", $row->RecordID);
				if ((new cls_Examen())->aantal($f) == 0) {
					$f = sprintf("AK.OnderdeelID=%d", $row->RecordID);
					if ((new cls_Afdelingskalender())->aantal($f) == 0) {
						$f = sprintf("AA.Toegang=%d", $row->RecordID);
						if ((new cls_Authorisation())->aantal($f) == 0) {
							$f = sprintf("GR.OnderdeelID=%d", $row->RecordID);
							if ((new cls_Groep())->aantal($f) == 0) {
								$this->delete($row->RecordID, $reden);
							}
						}
					}
				}
			}
		}
		
		$this->optimize();
		
	}  # cls_Onderdeel->opschonen
	
}  # cls_Onderdeel

class cls_Functie extends cls_db_base {
	
	private int $fnkid = 0;
	public string $naam = "";
	public string $afkorting = "";
	
	function __construct($p_fnkid=-1) {
		$this->table = TABLE_PREFIX . "Functie";
		$this->basefrom = $this->table . " AS F";
		$this->ta = 20;
		$this->pkkol = "Nummer";
		$this->per = date("Y-m-d");
		$this->vulvars($p_fnkid);
	}
	
	private function vulvars($p_fnkid=-1) {
		if ($p_fnkid > 0) {
			$query = sprintf("SELECT * FROM %s WHERE F.Nummer=%d;", $this->basefrom, $p_fnkid);
			$row = $this->execsql($query)->fetch();
			if (isset($row->Nummer)) {
				$this->naam = $row->Omschrijv ?? "";
				$this->afkorting = $row->Afkorting ?? "";
				if (strlen($this->naam) > 20) {
					$this->naamlogging = $this->afkorting;
				} else {
					$this->naamlogging = $this->naam;
				}
				$this->ingevoerd = $row->Ingevoerd ?? "";
			}
		}
	}  # cls_Functie->vulvars
	
	public function selectlijst($p_soort, $p_per="", $p_asarray=0, $p_ondid=0) {
		
		$sqaantal = sprintf("SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE() AND F.Nummer=LO.Functie", TABLE_PREFIX);
		if ($p_ondid > 0) {
			$sqaantal .= sprintf(" AND LO.OnderdeelID=%d", $p_ondid);
		}
		
		if (strlen($p_per) == 10) {
			$f = sprintf("WHERE IFNULL(F.`Vervallen per`, '9999-12-31') >= '%s'", $p_per);
		} else {
			$f = sprintf("WHERE IFNULL(F.`Vervallen per`, '9999-12-31') >= '%s'", $this->per);
		}
		
		if ($p_soort == "A") {
			$f .= " AND F.Afdelingsfunctie=1";
		} elseif ($p_soort == "L" or $p_soort == "BCF") {
			$f .= " AND  F.Ledenadministratiefunctie=1";
		}
		
		if ($p_ondid > 0) {
			$f .= sprintf(" AND (%s) > 0", $sqaantal);
		}
		
		$query = sprintf("SELECT F.Nummer, F.Omschrijv, IFNULL(F.`Vervallen per`, '9999-12-31') AS Vervallen, (%s) AS aantalMetFunctie FROM %s %s ORDER BY F.Sorteringsvolgorde, F.Omschrijv;", $sqaantal, $this->basefrom, $f);
		$result = $this->execsql($query);
		$rows = $result->fetchAll();
		
		if ($p_asarray == 1 and count($rows) > 0) {
			foreach ($rows as $row) {
				$rv[$row->Nummer] = $row->Omschrijv;
			}
			return $rv;
		} else {
			return $rows;
		}
	}  # cls_Functie->selectlijst
	
	public function add($p_nr=-1) {
		$this->tas = 31;
		if ($p_nr >= 0) {
			$nwpk = $p_nr;
		} else {
			$nwpk = $this->nieuwrecordid();
		}
		
		$this->query = sprintf("INSERT INTO %s (Nummer, Sorteringsvolgorde, Ingevoerd) VALUES (%d, 99, NOW());", $this->table, $nwpk);
		if ($this->execsql() > 0) {
			
			$this->mess = sprintf("Functie %d is toegevoegd.", $nwpk);
			$this->log($nwpk, 0);
			
			$sql = sprintf("INSERT INTO Functie (Nummer, Sorteringsvolgorde, Ingevoerd) VALUES (%d, 99, Now());", $nwpk);
			$this->interface($sql);
		}
	}  # cls_Functie->add
	
	public function update($p_fnkid, $p_kolom, $p_waarde) {
		$this->vulvars($p_fnkid);
		$this->tas = 32;
		
		if (toegang("Ledenlijst/Basisgegevens/Functies")) {
			$this->pdoupdate($p_fnkid, $p_kolom, $p_waarde);
		} else {
			$this->mess = "Je hebt geen rechten om functies aan te passen.";
		}
		$this->log($p_fnkid);
	}  # cls_Functie->update
	
	private function delete($p_fnkid, $p_reden="") {
		$this->vulvars($p_fnkid);
		$this->tas = 33;
		
		if ($this->pdodelete($p_fnkid, $p_reden) > 0) {
			$this->log($p_fnkid);
		}
		
	}
	
	public function controle() {
		
		$f = "F.Nummer=0";
		if ($this->aantal($f) == 0) {
			$this->add(0);
		}

		$this->update(0, "Omschrijv", "");
		$this->update(0, "Afdelingsfunctie", 1);
		$this->update(0, "Ledenadministratiefunctie", 1);
		$this->update(0, "Kader", 0);
		$this->update(0, "Inval", 0);

	}  # cls_Functie->controle
	
	public function opschonen() {
		
		$i_lo = new cls_lidond();
		
		foreach ($this->basislijst("IFNULL(`Vervallen per`, '9999-12-31') < CURDATE()") as $row) {
			$f = sprintf("Functie=%d", $row->Nummer);
			if ($i_lo->aantal($f) == 0) {
				$this->delete($row->Nummer, "de functie vervallen is en wordt nergens meer gebruikt.");
			}
		}
		
		$this->optimize();
		
	}  # cls_Functie->opschonen
	
}  # cls_Functie

class cls_Afdelingskalender extends cls_db_base {
	private $ondid = 0;
	private $akid = 0;
	public $datum = "";
	
	public object $i_ond;
	
	function __construct($p_ondid=-1, $p_akid=-1) {
		$this->table = TABLE_PREFIX . "Afdelingskalender";
		$this->basefrom = sprintf("%s AS AK", $this->table);
		$this->per = date("Y-m-d");
		$this->vulvars($p_akid, $p_ondid);
	}
	
	public function vulvars($p_akid=-1, $p_ondid=-1) {
		$this->ta = 21;
		if ($p_akid >= 0) {
			$this->akid = $p_akid;
		}
		if ($p_ondid >= 0) {
			$this->ondid = $p_ondid;
		}
		if ($this->akid > 0) {
			$query = sprintf("SELECT AK.* FROM %s WHERE AK.RecordID=%d;", $this->basefrom, $this->akid);
			$row = $this->execsql($query)->fetch();
			if (isset($row->RecordID)) {
				$this->ondid = $row->OnderdeelID ?? 0;
				$this->datum = $row->Datum ?? "";
				$this->naamlogging = $this->datum;
				$this->ingevoerd = $row->Ingevoerd ?? "";
			}
		}
		
		$this->i_ond = new cls_Onderdeel($this->ondid);
		
	}  # cls_Afdelingskalender->vulvars
	
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
		
		if (strlen($this->where) > 1) {
			$where .= sprintf(" AND %s", $this->where);
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
	}  # cls_Afdelingskalender->lijst
	
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
				$o .= " " . substr($row->Omschrijving, 0, 35);
			}
			$c = checked($row->RecordID, "option", $p_cv);
			$rv .= sprintf("<option value=%d %s>%s</option>\n", $row->RecordID, $c, $o);
		}
		return $rv;
	}  # cls_Afdelingskalender->htmloptions
	
	public function komendeles() {
		$query = sprintf("SELECT AK.RecordID FROM %s WHERE AK.OnderdeelID=%d AND AK.Datum >= CURDATE() AND AK.Activiteit=1 ORDER BY AK.Datum;", $this->basefrom, $this->ondid);
		return $this->scalar($query);
	}
	
	public function datumeerstelesperiode($p_afdid, $p_aantal=13) {
		$query = sprintf("SELECT AK.Datum FROM %s WHERE AK.OnderdeelID=%d AND AK.Activiteit=1 AND AK.Datum <= CURDATE() ORDER BY AK.Datum DESC;", $this->basefrom, $p_afdid);
		$res = $this->execsql($query);
		$i = 0;
		$rv = "2000-01-01";
		foreach ($res->fetchAll() as $row) {
			if ($i < $p_aantal) {
				$rv = $row->Datum;
			}
			$i++;
		}
		
		return $rv;
	}
	
	public function add($p_ondid) {
		$this->tas = 1;
		$this->vulvars(-1, $p_ondid);
		$nrid = $this->nieuwrecordid();
		
		$query = sprintf("SELECT DATE_ADD(IFNULL(MAX(AK.Datum), CURDATE()), INTERVAL 7 DAY) FROM %s WHERE OnderdeelID=%d;", $this->basefrom, $p_ondid);
		$dat = $this->scalar($query);
		
		$query = sprintf("INSERT INTO %s (RecordID, OnderdeelID, Datum) VALUES (%d, %d, '%s');", $this->table, $nrid, $p_ondid, $dat);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Record %d (%s) is voor afdeling %s aan de afdelingskalender toegevoegd.", $nrid, $dat, $this->ondnaam);
		}
		
		$this->log($nrid, 0, $p_ondid);
		return $nrid;
	}  # cls_Afdelingskalender->add
	
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
			$this->log($p_akid, 0, $this->ondid);
		}
		
		return $rv;
	}  # cls_Afdelingskalender->update
	
	public function delete($p_akid) {
		$this->tas = 3;
		$this->vulvars($p_akid);
		
		if ((new cls_Aanwezigheid())->aantal(sprintf("AfdelingskalenderID=%d", $p_akid)) == 0) {
			$this->pdodelete($p_akid);
		} else {
			$this->mess = sprintf("Record %d uit de afdelingskalender mag niet worden verwijderd, want die is nog in gebruik.", $p_akid);
		}
		$this->log($p_akid, 0, $this->ondid);
	}
	
	public function controle() {
	}
	
	public function opschonen() {
		
		$this->optimize();
	
	}  # cls_Afdelingskalender->opschonen
	
}  # cls_Afdelingskalender

class cls_Aanwezigheid extends cls_db_base {
	
	private int $loid = 0;
	public int $aanwid = 0;
	private int $akid = 0;
	public string $status = "";
	public string $opmerking = "";
	public $isaanwezig = true;
	private string $selstat = "";
	
	public object $i_ak;
	public object $i_lo;
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Aanwezigheid";
		$this->basefrom = sprintf("%s AS AW", $this->table);
		$this->ta = 24;
		$this->per = date("Y-m-d");
		
		$this->selstat = "CASE AW.Status WHEN '' THEN 'Aanwezig' ";
		foreach	(ARRPRESENTIESTATUS as $s => $v) {
			$this->selstat .= sprintf(" WHEN '%s' THEN '%s'", $s, $v);
		}
		$this->selstat .= " ELSE 'Onbekend' END";
	}
	
	public function vulvars($p_loid, $p_akid=-1) {
		if ($p_loid >= 0) {	
			$this->loid = $p_loid;
		}
		if ($p_akid >= 0) {
			$this->akid = $p_akid;
		}
		
		$this->aanwid = 0;
		$this->status = "";
		$this->opmerking = "";
		if ($this->loid > 0 and $this->akid > 0) {
			$query = sprintf("SELECT AW.RecordID FROM %s WHERE AW.LidondID=%d AND AW.AfdelingskalenderID=%d;", $this->basefrom, $this->loid, $this->akid);
			$awrow = $this->execsql($query)->fetch();
			if (isset($awrow->RecordID)) {
				$this->aanwid = $awrow->RecordID;
				$this->status = $awrow->Status ?? "";
				$this->opmerking = $awrow->Opmerking ?? "";
			}
		}
		
		if ($this->status == "A" or $this->status == "J" or $this->status == "L" or strlen($this->status) == 0) {
			$this->isaanwezig = true;
		} else {
			$this->isaanwezig = false;
		}
		
		$this->i_ak = new cls_Afdelingskalender(-1, $this->akid);
		$this->i_lo = new cls_Lidond(-1, -1, $this->loid);

		$this->naamlogging = $this->i_ak->datum;
	}  # cls_Aanwezigheid->vulvars
	
	public function status($p_loid, $p_akid) {
		$query = sprintf("SELECT IFNULL(MAX(Status), '') FROM %s WHERE LidondID=%d AND AfdelingskalenderID=%d", $this->table, $p_loid, $p_akid);
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
	
	public function overzichtlid($p_lidid, $p_seizoen=-1) {
		$this->lidid = $p_lidid;
		
		$xw = "";
		if ($p_seizoen > 0) {
			$xw = sprintf(" AND (SELECT IFNULL(MAX(SZ.Nummer), 0) FROM %sSeizoen AS SZ WHERE SZ.Begindatum <= AK.Datum AND SZ.Einddatum >= AK.Datum)=%d", TABLE_PREFIX, $p_seizoen);
		}
		
		$query = sprintf("SELECT AK.Datum, AK.Omschrijving, F.OMSCHRIJV AS Functie, %1\$s AS Status, AW.Opmerking,
						  (SELECT IFNULL(MAX(SZ.Nummer), 0) FROM %3\$sSeizoen AS SZ WHERE SZ.Begindatum <= AK.Datum AND SZ.Einddatum >= AK.Datum) AS Seizoen
						  FROM %3\$sAfdelingskalender AS AK INNER JOIN (%2\$s INNER JOIN (%3\$sLidond AS LO INNER JOIN %3\$sFunctie AS F ON LO.Functie=F.Nummer) ON AW.LidondID=LO.RecordID) ON AW.AfdelingskalenderID=AK.RecordID
						  WHERE LO.Lid=%4\$d AND AK.Datum > DATE_SUB(CURDATE(), INTERVAL 12 MONTH) AND AK.Datum <= CURDATE()%5\$s
						  ORDER BY AK.Datum DESC;", $this->selstat, $this->basefrom, TABLE_PREFIX, $this->lidid, $xw);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}  # cls_Aanwezigheid->overzichtlid
	
	public function seizoenen($p_ondid) {
		
		$sq = sprintf("(SELECT COUNT(*) FROM %sAfdelingskalender AS AK WHERE AK.Datum >= SZ.Begindatum AND AK.Datum <= SZ.Einddatum AND Activiteit=1 AND AK.OnderdeelID=%d AND AK.Datum <= CURDATE())", TABLE_PREFIX, $p_ondid);
		$query = sprintf("SELECT SZ.Nummer, SZ.Begindatum, SZ.Einddatum FROM %2\$sSeizoen AS SZ WHERE %1\$s > 0 ORDER BY SZ.Begindatum DESC;", $sq, TABLE_PREFIX);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function perlidperperiode($p_loid, $p_vanaf, $p_tm="") {
		$this->vulvars($p_loid);
		
		if (strlen($p_tm) < 10) {
			$p_tm = date("Y-m-d");
		}
		
		if ($this->i_lo->vanaf > $p_vanaf) {
			$p_vanaf = $this->i_lo->vanaf;
		}
		if ($this->i_lo->lidtm < $p_tm) {
			$p_tm = $this->i_lo->lidtm;
		}
		
		$query = sprintf("SELECT MAX(LO.Lid) AS LidID,
						  SUM(IF(AW.Status IN ('A'), 1, 0)) AS aantAanwezig,
						  SUM(IF(AW.Status IN ('J'), 1, 0)) AS aantAangemeld,
						  IFNULL(SUM(IF(AW.Status IN ('H', 'N', 'R', 'X', 'Z'), 1, 0)), 0) AS aantAfwezig,
						  IFNULL(SUM(IF(AW.Status='V', 1, 0)), 0) AS aantVervallen,
						  SUM(IF(AW.Status IN ('N', 'X'), 1, 0)) AS aantZonderReden,
						  SUM(IF(AW.Status='R', 1, 0)) AS aantMetReden,
						  SUM(IF(AW.Status='Z', 1, 0)) AS aantZiek,
						  SUM(IF(AW.Status='L', 1, 0)) AS aantLaat
						  FROM (%1\$s INNER JOIN %2\$sLidond AS LO ON LO.RecordID=AW.LidondID) INNER JOIN %2\$sAfdelingskalender AS AK ON AW.AfdelingskalenderID=AK.RecordID
						  WHERE AK.Datum >= '%3\$s' AND AK.Datum <= '%4\$s' AND AW.LidondID=%5\$d;", $this->basefrom, TABLE_PREFIX, $p_vanaf, $p_tm, $p_loid);
		$result = $this->execsql($query);
		$row = $result->fetch();
		return $row;
	}
	
	public function beschikbarelessen($p_loid, $p_vanaf="1970-01-01", $p_tm="") {
		$this->vulvars($p_loid);
		
		if (strlen($p_tm) < 10) {
			$p_tm = date("Y-m-d");
		}

		if ($p_vanaf < $this->i_lo->vanaf) {
			$p_vanaf = $this->i_lo->vanaf;
		}
		
		if ($p_tm > $this->i_lo->lidtm) {
			$p_tm = $this->i_lo->lidtm;
		}
		
		$f = sprintf("AK.Activiteit=1 AND AK.OnderdeelID=%d AND AK.Datum >= '%s' AND AK.Datum <= '%s'", $this->i_lo->ondid, $p_vanaf, $p_tm);
		
		$rv = (new cls_Afdelingskalender())->aantal($f);
		
		$query = sprintf("SELECT COUNT(*) FROM %s INNER JOIN %sAfdelingskalender AS AK ON AW.AfdelingskalenderID=AK.RecordID WHERE AW.Status='V' AND AW.LidondID=%d", $this->basefrom, TABLE_PREFIX, $p_loid);
		$query .= sprintf(" AND AK.Datum >= '%s' AND AK.Datum <= '%s';", $p_vanaf, $p_tm);
		$rv = $rv - $this->scalar($query);
		
		return $rv;
	}  # cls_Aanwezigheid->beschikbarelessen
	
	public function gemistelessen($p_loid, $p_vanaf="1970-01-01") {
		
		$query = sprintf("SELECT COUNT(*) FROM %s INNER JOIN %sAfdelingskalender AS AK ON AW.AfdelingskalenderID=AK.RecordID
						  WHERE AW.LidondID=%d AND AK.Datum >= '%s' AND AK.Datum <= CURDATE() AND (AW.Status NOT IN ('A', 'L', 'V'));", $this->basefrom, TABLE_PREFIX, $p_loid, $p_vanaf);
		return $this->scalar($query);
		
	}  # cls_Aanwezigheid->gemistelessen
	
	private function add($p_loid, $p_akid) {
		$this->vulvars($p_loid, $p_akid);
		$this->tas = 1;
		
		if (strlen($this->i_ak->datum) >= 10) {
			$nrid = $this->nieuwrecordid();
			$query = sprintf("INSERT INTO %s (RecordID, LidondID, AfdelingskalenderID, Status, Opmerking) VALUES (%d, %d, %d, '', '');", $this->table, $nrid, $p_loid, $p_akid);
			if ($this->execsql($query) > 0) {
				$this->mess = sprintf("Tabel %s: Record %d (%s) is toegevoegd.", $this->table, $nrid, $this->naamlogging);
			} else {
				$this->mess = "Toevoegen record aanwezigheid is mislukt.";
				$nrid = 0;
			}
		} else {
			$nrid = 0;
			$this->mess = sprintf("Record %d bestaat niet in de afdelingskalender. Het record wordt niet toegevoegd.", $p_akid);
		}
		
		$this->aanwid = $nrid;
		$this->log($nrid, 0, $this->ondid);

		return $nrid;
	}  # cls_Aanwezigheid->add
	
	public function update($p_loid, $p_akid, $p_kolom, $p_waarde) {
		$this->vulvars($p_loid, $p_akid);
		$this->tas = 2;
		
		if ($this->loid > 0 and $this->aanwid == 0 and strlen($p_waarde) > 0) {
			$this->add($this->loid, $this->akid);
		}
		
		if ($this->aanwid > 0 and $this->pdoupdate($this->aanwid, $p_kolom, $p_waarde) > 0) {
			$this->mess = sprintf("Tabel Aanwezigheid: Kolom '%s' in record %d is in '%s' gewijzigd.", $p_kolom, $this->aanwid, $p_waarde);
			$this->log($this->aanwid, 0, $this->ondid);
		}
	}
	
	private function delete($p_loid, $p_akid, $p_reden="") {
		$this->vulvars($p_loid, $p_akid);
		$this->tas = 13;
		if ($this->pdodelete($this->aanwid, $p_reden) > 0) {
			$this->log($this->aanwid, 0, $this->ondid);
		}
	}  # cls_Aanwezigheid->delete
	
	public function controle() {
		$this->tas = 2;
		
		foreach($this->basislijst() as $row) {
			if ($row->Status == "N") {
				// Deze if mag na 1 januari 2024 weg
				if ($this->pdoupdate($row->RecordID, "Status", "X", "de status N is komen te vervallen.")) {
					$this->log($row->RecordID);
				}
			} elseif (strlen($row->Status) > 0 and array_key_exists($row->Status, ARRPRESENTIESTATUS) == false) {
				$this->pdoupdate($row->RecordID, "Status", "", "het een onbekende status is");
			}
		}
	}  # cls_Aanwezigheid->controle
	
	public function opschonen() {
		$i_lo = new cls_Lidond();
		$this->tas = 4;
		
		$tot = 0;
		$query = sprintf("SELECT O.RecordID, O.BewaartermijnPresentie FROM %sOnderdl AS O WHERE IFNULL(O.BewaartermijnPresentie, 0) > 0;", TABLE_PREFIX);
		foreach ($i_lo->execsql($query)->fetchAll() as $ondrow) {
			$f = sprintf("LO.OnderdeelID=%d AND IFNULL(LO.Opgezegd, '9999-12-31') < DATE_SUB(CURDATE(), INTERVAL %d MONTH)", $ondrow->RecordID, $ondrow->BewaartermijnPresentie);
			foreach ($i_lo->basislijst($f) as $lorow) {
				$this->lidid = $lorow->Lid;
				$this->ondid = $lorow->OnderdeelID;
				$delqry = sprintf("DELETE FROM %s WHERE LidondID=%d;", $this->table, $lorow->RecordID);
				$aant = $this->execsql($delqry);
				if ($aant > 0) {
					$this->mess = sprintf("Tabel %s: %d records verwijderd, omdat het lid geen lid van het onderdeel meer is.", $this->table, $aant);
					$this->log(0, 0, $this->ondid);
				}
				$tot += $aant;
			}
		}
		$i_lo = null;
		
		$query = sprintf("SELECT AW.LidondID, AW.AfdelingskalenderID FROM %s INNER JOIN %sAfdelingskalender AS AK ON AK.RecordID=AW.AfdelingskalenderID WHERE AK.Datum < DATE_SUB(CURDATE(), INTERVAL 7 YEAR);", $this->basefrom, TABLE_PREFIX);
		$rows = $this->execsql($query)->fetchAll();
		$reden = "de activiteit langer dan 7 jaar geleden was";
		
		foreach ($rows as $row) {
			$this->delete($row->LidondID, $row->AfdelingskalenderID, $reden);
			$tot++;
		}
		
		$query = sprintf("SELECT AW.LidondID, AW.AfdelingskalenderID FROM %s WHERE (SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.RecordID=AW.LidondID)=0;", $this->basefrom, TABLE_PREFIX);
		$rows = $this->execsql($query)->fetchAll();
		$reden = "het gerelateerde record in de tabel 'Lidond' niet (meer) bestaat.";
		
		foreach ($rows as $row) {
			$this->delete($row->LidondID, $row->AfdelingskalenderID, $reden);
			$tot++;
		}
		
		
		$this->optimize();
		
		return $tot;
		
	}  # cls_Aanwezigheid->opschonen
	
}  # cls_Aanwezigheid

class cls_Lidond extends cls_db_base {
	public int $loid = 0;				// RecordID van het record in Lidond
	public int $ondid = 0;				// RecordID van het onderdeel
	public string $vanaf = "";			// Wanneer startte dit lidmaatschap van dit onderdeel
	private string $opgezegd = "";		// Per wanneer is dit onderdeel opgezegd
	public string $lidtm;				// Wanneer eindigt dit lidmaatschap van dit onderdeel
	private int $groepid = 0;			// RecordID van de afdelingsgroep
	public string $email = "";			// E-mail behorende bij deze functie
	
	public $lidnaam = "";				// De naam van het lid
	
	private $omsfunctie = "";			// Omschrijving van de functie
	
	public int $actid = 0;				// RecordID van de activiteit van de groep
	
	public $magmuteren = false;			// Mag het ingelogde lid deze mutaties doen?
	private $sqlmgr = "";				// SQL-code om de laatste mutatie van afdelingsgroep te bepalen.
	
	public string $loclass = "";
	public string $lotitle = "";
	public int $suggestievolgendegroep = 0;
	
	public object $i_ond;
	public object $i_lid;
	public object $i_gr;
	
	function __construct($p_ondid=-1, $p_lidid=-1, $p_loid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Lidond";
		$this->basefrom = $this->table . " AS LO";
		$this->ta = 6;
		$this->per = date("Y-m-d");
		$this->vulvars($p_loid, $p_lidid, $p_ondid);
		$this->sqlmgr = sprintf("(SELECT IFNULL(MAX(DatumTijd), '1900-01-01') FROM %sAdmin_activiteit AS A WHERE A.ReferID=LO.RecordID AND (A.refTable='Lidond' OR A.refTable='rbm_Lidond') AND (A.refColumn='GroepID' OR A.TypeActiviteit=19))", TABLE_PREFIX);
		// Na 1 april 2025 mogen "OR A.refTable='rbm_Lidond'" en "OR A.TypeActiviteit=19" worden verwijderd.
	}
	
	public function vulvars($p_loid=-1, $p_lidid=-1, $p_ondid=-1, $p_per="") {

		$this->loid = $p_loid;
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		if ($p_ondid >= 0) {
			$this->ondid = $p_ondid;
		}
		if (strlen($p_per) == 10) {
			$this->per = $p_per;
		}
		
		if ($this->loid <= 0 and $this->lidid > 0 and $this->ondid > 0) {
			$query = sprintf("SELECT IFNULL(MAX(LO.RecordID), 0) FROM %1\$s WHERE LO.Lid=%2\$d AND LO.OnderdeelID=%3\$d AND LO.Vanaf <= '%4\$s' AND IFNULL(LO.Opgezegd, '9999-12-31') >= '%4\$s';", $this->basefrom, $this->lidid, $this->ondid, $this->per);
			$this->loid = $this->scalar($query);
		}

		$this->opgezegd = "";
		if ($this->loid > 0) {
			$this->query = sprintf("SELECT LO.* FROM %s WHERE LO.RecordID=%d;", $this->basefrom, $this->loid);
			$row = $this->execsql()->fetch();
			if (isset($row->Lid)) {
				$this->lidid = $row->Lid;
				$this->ondid = $row->OnderdeelID;
				$this->groepid = $row->GroepID ?? 0;
				$this->email = $row->EmailFunctie ?? "";
				$this->vanaf = $row->Vanaf;
				$this->opgezegd = $row->Opgezegd ?? "";
			} else {
				$this->mess = sprintf("Record in lidond met RecordID %d bestaat niet.", $this->loid);
				$this->tm = 1;
			}
		}
		
		if (strlen($this->opgezegd) == 10) {
			$this->lidtm = $this->opgezegd;
		} else {
			$this->lidtm = "9999-12-31";
		}
		
		$this->i_ond = new cls_Onderdeel($this->ondid);
		$this->i_lid = new cls_Lid($this->lidid);
		$this->i_gr = new cls_Groep(0, $this->groepid);
		
		if ($_SERVER['PHP_SELF'] == "/maatwerk/jobs.php") {
			$this->magmuteren = true;
		} elseif ($this->ondid > 0) {
			if ($this->i_ond->ledenmuterendoor > 0 and in_array($this->i_ond->ledenmuterendoor, explode(",", $_SESSION['lidgroepen'])) == true) {
				$this->magmuteren = true;
			} elseif ($_SESSION['lidid'] == $this->lidid and $this->i_ond->type == "T") {
				$this->magmuteren = true;
			} else {
				$this->magmuteren = false;
				if ($this->i_ond->type == "A" and toegang("Ledenlijst/Wijzigen lid/Afdelingen", 0, 0)) {
					$this->magmuteren = true;
				} elseif ($this->i_ond->type == "B" and toegang("Ledenlijst/Wijzigen lid/B, C en F", 0, 0)) {
					$this->magmuteren = true;
				} elseif ($this->i_ond->type == "C" and toegang("Ledenlijst/Commissies", 0, 0)) {
					$this->magmuteren = true;
				} elseif ($this->i_ond->type == "E" and toegang("Ledenlijst/Wijzigen lid/Eigenschappen", 0, 0)) {
					$this->magmuteren = true;
				} elseif ($this->i_ond->type == "F" and toegang("Ledenlijst/Wijzigen lid/B, C en F", 0, 0)) {
					$this->magmuteren = true;
				} elseif ($this->i_ond->type == "G" and toegang("Ledenlijst/Wijzigen lid/Groepen", 0, 0)) {
					$this->magmuteren = true;
				} elseif ($this->i_ond->type == "O" and toegang("Ledenlijst/Wijzigen lid/Onderscheidingen", 0, 0)) {
					$this->magmuteren = true;
				} elseif ($this->i_ond->type == "R" and toegang("Ledenlijst/Rollen", 0, 0)) {
					$this->magmuteren = true;
				} elseif ($this->i_ond->type == "S" and toegang("Ledenlijst/Selecties", 0, 0)) {
					$this->magmuteren = true;
				} elseif ($this->i_ond->type == "T" and toegang("Ledenlijst/Wijzigen lid/Toestemmingen", 0, 0)) {
					$this->magmuteren = true;
				}
			}
		}
		
		if ($this->lidid > 0) {
			$query = sprintf("SELECT IFNULL(%1\$s, '%3\$d') FROM %2\$sLid AS L WHERE L.RecordID=%3\$d;", $this->selectnaam, TABLE_PREFIX, $this->lidid);
			$this->lidnaam = $this->scalar($query);
		} else {
			$this->lidnaam = "";
		}

		$this->loclass = "";
		$this->lotitle = "";
		$this->suggestievolgendegroep = 0;
		
		if ($this->vanaf > date("Y-m-d")) {
			$this->loclass = "wordtlid";
		} elseif ($this->opgezegd > "1970-01-01" and $this->opgezegd < date("Y-m-d", strtotime("+3 month"))) {
			$this->loclass = "heeftopgezegd";
			$this->lotitle = sprintf("heeft per %s opgezegd", $this->opgezegd);
		}

		if ($this->groepid > 0 and $this->i_gr->dpid > 0) {
			$i_dp = new cls_Diploma($this->i_gr->dpid);
			$i_ld = new cls_Liddipl();
			
			if ($i_dp->voorgangerid > 0) {
				$f = sprintf("LD.Lid=%d AND LD.DiplomaID=%d AND LD.DatumBehaald <= CURDATE() AND LD.Geslaagd=1 AND LD.LaatsteBeoordeling=1", $this->lidid, $i_dp->voorgangerid);
				if ($i_ld->aantal($f) == 0) {
					$this->loclass .= " voorgangerontbreekt";
					$this->lotitle = sprintf("%s ontbreekt", $i_dp->naam($i_dp->voorgangerid));
				}
				$f = sprintf("LD.Lid=%d AND LD.DiplomaID=%d AND LD.DatumBehaald <= CURDATE() AND LD.Geslaagd=1 AND LD.LaatsteBeoordeling=1", $this->lidid, $i_dp->voorgangerid);
				$dvd = $i_ld->max("DatumBehaald", $f);
				if (strlen($dvd) == 10) {
					if ($i_dp->doorlooptijd > 0 and $dvd < date("Y-m-d", strtotime(sprintf("-%d month", $i_dp->doorlooptijd)))) {
						$this->loclass .= " voortgangsprobleem";
					}
					if (strlen($this->lotitle) > 0) {
						$this->lotitle .= ", ";
					}
					$this->lotitle .= sprintf("%s behaald op %s", $i_dp->naam($i_dp->voorgangerid), date("d-m-Y", strtotime($dvd)));
				}
			} else {
				if (strlen($this->lotitle) > 0) {
					$this->lotitle .= ", ";
				}
				$this->lotitle .= sprintf("lid vanaf %s", date("d-m-Y", strtotime($this->vanaf)));
				if ($i_dp->doorlooptijd > 0 and $this->vanaf < date("Y-m-d", strtotime(sprintf("-%d month", $i_dp->doorlooptijd)))) {
					$this->loclass .= " voortgangsprobleem";
				}
			}

			$f = sprintf("LD.Lid=%d AND LD.DiplomaID=%d AND LD.DatumBehaald <= CURDATE() AND LD.Geslaagd=1 AND LD.LaatsteBeoordeling=1", $this->lidid, $this->i_gr->dpid);
			$dh = $i_ld->max("DatumBehaald", $f);
			if (strlen($dh) > 0) {
				$this->loclass .= " dubbeldiploma";
				$this->lotitle = sprintf("%s is op %s behaald", $i_dp->naam($this->i_gr->dpid), date("d-m-Y", strtotime($dh)));
				$this->suggestievolgendegroep = 1;
			}
			
			$i_dp = null;
			$i_ld = null;
		}
		
	}  # cls_Lidond->vulvars
	
	public function record($p_lidid, $p_ondid=-1, $p_per="", $p_loid=0) {
		
		if (strlen($p_per) < 10) {
			$p_per = date("Y-m-d");
		}
		$this->vulvars($p_loid, $p_lidid, $p_ondid, $p_per);
		
		$this->query = sprintf("SELECT LO.*, GR.Starttijd, O.Naam AS AfdNaam, Act.Omschrijving AS GrActiviteit, Act.Contributie AS GrContributie, F.Omschrijv AS Functie, LO.Functie AS FunctieID, F.Kader, GR.Aanwezigheidsnorm
								FROM ((%s INNER JOIN (%2\$sGroep AS GR LEFT OUTER JOIN %2\$sActiviteit AS Act ON Act.RecordID=GR.ActiviteitID) ON LO.GroepID=GR.RecordID) INNER JOIN %2\$sFunctie AS F ON F.Nummer=LO.Functie) INNER JOIN %2\$sOnderdl AS O ON LO.OnderdeelID=O.RecordID WHERE LO.RecordID=%3\$d;", $this->basefrom, TABLE_PREFIX, $this->loid);
		$result = $this->execsql();
		return $result->fetch();
	}  # cls_Lidond->record
	
	public function lijst($p_ondid, $p_filter=1, $p_ord="GR.Volgnummer, GR.Kode", $p_per="", $p_extrafilter="", $p_limiet=0, $p_fetched=1) {
		
		if (strlen($p_per) < 10) {
			$p_per = $this->per;
		}
				
		if ($p_filter === 1) {
			// huidige leden
			$w = cls_db_base::$wherelidond;
		} elseif ($p_filter === 2) {
			// huidige en toekomstige leden
			$w = "IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE()";
		} elseif ($p_filter === 3) {
			// leden zonder einde-datum
			$w = "(LO.Opgezegd IS NULL)";
			
		} elseif ($p_filter === 4) {
			// zonder kader/functionarissen en met toekomstige leden
			$w = "IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE() AND LO.Functie=0";
			
		} elseif ($p_filter === 5) {
			// Verenigingskader
			$w = cls_db_base::$wherelidond . " AND (O.Kader=True OR (F.Kader=True AND O.Type='F'))";
			
		} elseif ($p_filter === 6) {
			// Afdelingskader
			$w = cls_db_base::$wherelidond . " AND O.`Type`='A' AND F.Kader=True";
			
		} elseif ($p_filter === 7) {
			// Commissieleden en functionarissen die niet onder het kader vallen, meestal aangesteld door de AV
			$w = cls_db_base::$wherelidond . " AND ((O.TYPE='C' AND O.Kader=False) OR (O.TYPE='F' AND F.Kader=False))";
			
		} elseif ($p_filter === 8) {
			// Geen filter
			$w = "(1 = 1)";
			
		} elseif (strlen($p_filter) > 1) {
			$w = $p_filter;
			
		} else {	
			// Leden op per datum
			$w = sprintf("LO.Vanaf <= '%1\$s' AND IFNULL(LO.Opgezegd, '9999-12-31') >= '%1\$s'", $p_per);
		}
		if ($p_ondid > 0) {
			$w .= sprintf(" AND LO.OnderdeelID=%d", $p_ondid);
		} else {
			if (strlen($p_ord) > 0) {
				$p_ord = ", " . $p_ord;
			}
			$p_ord = "IF(IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE(), 0, 1)" . $p_ord;
		}
		if (strlen($p_extrafilter) > 0) {
			$w .= " AND " . $p_extrafilter;
		}
		if (strlen($this->where) > 0) {
			$w .= " AND " . $this->where;
		}
		
		if (strlen($p_ord) > 0) {
			$p_ord .= ", ";
		}
		
		$lm = "";
		if ($p_limiet > 0) {
			$lm = sprintf(" LIMIT %d", $p_limiet);
		}
		
		$query = sprintf("SELECT LO.RecordID, LO.Lid AS LidID, LO.OnderdeelID, LO.Opmerk, LO.Vanaf, LO.Opgezegd, LO.EmailFunctie, LO.GroepID, LO.Functie, LO.Lid,
								%s AS NaamLid, L.Roepnaam, L.Achternaam, L.Tussenv, %s AS AVGnaam, L.GEBDATUM, %s AS Leeftijd, %s AS Email, L.EmailVereniging, 
								F.Omschrijv AS FunctieOms, F.Afkorting AS FunctAfk, F.Inval AS Invalfunctie, %s AS Groep, GR.DiplomaID,
								O.Kode, O.Naam AS OndNaam, O.CentraalEmail, IF(LO.Functie > 0, 0, 1) AS RO_Email,
								GR.Kode AS GrCode, GR.Omschrijving AS GrNaam, GR.Aanwezigheidsnorm, IFNULL(Act.BeperkingAantal, 0) AS BeperkingAantal, L.RelnrRedNed AS SportlinkID,
								CASE WHEN LO.GroepID > 0 AND LO.Functie > 0 THEN CONCAT(F.OMSCHRIJV, '/', IF(LENGTH(GR.Kode)=0, GR.RecordID, GR.Kode))
									WHEN LO.Functie > 0 THEN F.OMSCHRIJV
									WHEN LO.GroepID > 0 THEN IF(LENGTH(GR.Omschrijving)=0, IF(LENGTH(GR.Kode)=0, GR.RecordID, GR.Kode), GR.Omschrijving)
									ELSE '' END AS `FunctieGroep`,
								IF(IFNULL(LO.Opgezegd, '9999-12-31') > CURDATE(), LO.RecordID, 0) AS ridDelete
						  FROM %s
						  WHERE %s
						  ORDER BY %sL.Achternaam, L.Tussenv, L.Roepnaam%s;", $this->selectnaam, $this->selectavgnaam, $this->selectleeftijd, $this->selectemail, $this->selectgroep, $this->fromlidond, $w, $p_ord, $lm);
		$result = $this->execsql($query);
		if ($p_fetched == 1) {
			return $result->fetchAll();
		} else {
			return $result;
		}
	}  # cls_Lidond->lijst
	
	public function aantallid($p_ondid, $p_filter="") {
		
		$w = cls_db_base::$wherelidond . sprintf(" AND LO.OnderdeelID=%d", $p_ondid);
		if (strlen($p_filter) > 0) {
			$w .= " AND " . $p_filter;
		}
		
		$query = sprintf("SELECT COUNT(DISTINCT Lid) FROM %s WHERE %s;", $this->fromlidond, $w);
		return $this->scalar($query);
	}  # cls_Lidond->aantallid
	
	public function lijstperlid($p_lidid, $p_type="*", $p_per="", $p_incltoekomst=0) {
		
		if (strlen($p_per) < 10) {
			$p_per = $this->per;
		}
		
		if ($p_incltoekomst == 1) {
			$w = sprintf("IFNULL(LO.Opgezegd, '9999-12-31') >= '%1\$s' AND LO.Lid=%2\$d", $p_per, $p_lidid);
		} else {
			$w = sprintf("LO.Vanaf <= '%1\$s' AND IFNULL(LO.Opgezegd, '9999-12-31') >= '%1\$s' AND LO.Lid=%2\$d", $p_per, $p_lidid);
		}
		if ($p_type != "*") {
			$w .= sprintf(" AND O.`Type`='%s'", $p_type);
		}
		
		$query = sprintf("SELECT LO.*, O.Naam AS NaamOnderdeel, O.CentraalEmail, F.OMSCHRIJV AS FunctieOms, F.Kader,
						  Act.Omschrijving AS GrActiviteit, GR.ActiviteitID, Act.Code AS ActCode, Act.Contributie AS GrContributie, IFNULL(Act.GBR, '') AS ActGBR,
						  O.LIDCB, O.JEUGDCB, O.FUNCTCB, O.Naam AS OndNaam, O.Kode AS OndCode
						  FROM %s WHERE %s ORDER BY LO.Vanaf;", $this->fromlidond, $w);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}  # cls_Lidond->lijstperlid
	
	public function groepsindeling($p_onderdeelid, $p_filter="", $p_inclkader=0) {
		
		$where = sprintf("IFNULL(LO.Opgezegd, CURDATE()) >= '%s' ", $this->per);
		if ($p_onderdeelid > 0) {
			$where .= sprintf("AND LO.OnderdeelID=%d ", $p_onderdeelid);
		}
		if (strlen($p_filter) > 0) {
			$where .= "AND " . $p_filter;
		} else {
			$where .= sprintf("AND LO.OnderdeelID IN (SELECT OnderdeelID FROM %sGroep)", TABLE_PREFIX);
		}
		if ($p_inclkader == 0) {
			$where .= " AND (LO.Functie=0 OR LO.GroepID > 0)";
		}
		
		$this->query = sprintf("SELECT DISTINCT O.Naam AS AfdNaam, CONCAT(GR.Starttijd, IF(LENGTH(GR.Eindtijd) > 3, CONCAT(' - ', GR.Eindtijd), '')) AS Tijdsblok, GR.Kode, GR.Omschrijving,
					L.RecordID AS LidID, L.Roepnaam, %1\$s AS NaamLid, %6\$s AS AVGnaam,
					%5\$s AS GroepOms, GR.Zwemzaal, GR.DiplomaID, LO.Functie,
					LO.RecordID, LO.GroepID, LO.Lid, %2\$s AS Leeftijd, LO.Vanaf, IFNULL(LO.Opgezegd, '9999-12-31') AS Opgezegd, %7\$s AS LaatsteGroepMutatie
					FROM %3\$s
					WHERE %4\$s
					ORDER BY O.Naam, IF(LO.GroepID=0 OR LENGTH(TRIM(IFNULL(GR.Starttijd, '')))=0, '99:99', GR.Starttijd), GR.Volgnummer, GR.Omschrijving, GR.RecordID, L.Achternaam, L.Roepnaam;", $this->selectnaam, $this->selectleeftijd, $this->fromlidond, $where, $this->selectgroep, $this->selectavgnaam, $this->sqlmgr);
		$result = $this->execsql();
		return $result->fetchAll();
		
	}  # cls_Lidond->groepsindeling
	
	public function lidgroepen() {
		
		if ($_SESSION['lidid'] <= 0) {
			$rv = "0";
		} else {
			$query = sprintf("SELECT GROUP_CONCAT(DISTINCT LO.OnderdeelID SEPARATOR ',') AS LG FROM %s WHERE Lid=%d AND %s;", $this->basefrom, $_SESSION['lidid'], cls_db_base::$wherelidond);
			$result = $this->execsql($query);
			$row = $result->fetch();
			if (strlen($row->LG) > 0) {
				$rv = "0," . $row->LG;
			} else {
				$rv = "0";
			}
			if (WEBMASTER) {
				$rv = "-1," . $rv;
			}
		}
		
		return $rv;
	}  # cls_Lidond->lidgroepen
	
	public function groepeningebruik($p_ondid=-1) {
		if ($p_ondid > 0) {
			$this->ondid = $p_ondid;
		}
		$query = sprintf("SELECT DISTINCT LO.GroepID, IF(LO.GroepID=0, 'Niet ingedeeld', GR.Omschrijving) AS Omschrijving, GR.Kode FROM %s AS LO LEFT OUTER JOIN %sGroep AS GR ON LO.GroepID=GR.RecordID WHERE (NOT LO.GroepID IS NULL) AND LO.OnderdeelID=%d AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE() ORDER BY IF(LO.GroepID=0, 999, GR.Volgnummer), GR.Omschrijving;", $this->table, TABLE_PREFIX, $this->ondid);
		$result = $this->execsql($query);
		return $result->fetchAll();
		
	}  # cls_Lidond->groepeningebruik
	
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
			} elseif ($p_soort == "E") {
				$xw = sprintf("LO.Vanaf <= '%1\$s' AND IFNULL(LO.Opgezegd, '9999-12-31') >= '%1\$s' AND O.`Type`='E'", $this->per);
			} else {
				$xw = sprintf("O.`Type`='%s'", $p_soort);
			}
			$xw .= " AND IFNULL(LO.Opgezegd, '9999-12-31') >= LO.Vanaf";
			
			$query = sprintf("SELECT O.Naam,
							  F.OMSCHRIJV AS Functie,
							  LO.Vanaf,
							  LO.Opmerk,
							  (SELECT CONCAT(IF(LENGTH(GR.Starttijd)=5, CONCAT(GR.Starttijd, ' - '), ''), IF(LENGTH(GR.Omschrijving) > 0, GR.Omschrijving, GR.Kode)) FROM %sGroep AS GR WHERE GR.RecordID=LO.GroepID) AS Groep,
							  LO.Opgezegd,
							  IF(LO.Vanaf > DATE_SUB(CURDATE(), INTERVAL 6 MONTH), '', CONCAT(FORMAT(DATEDIFF(IF(LO.Opgezegd IS NULL, CURDATE(), LO.Opgezegd), LO.Vanaf)/365.25, 1, 'nl_NL'), ' jaar')) AS Duur
							  FROM %s
							  WHERE %s AND LO.Lid=%d
							  ORDER BY IF(IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE(), 0, 1), LO.Vanaf DESC, O.Naam;", TABLE_PREFIX, $this->fromlidond, $xw, $p_lidid);
		}
		$result = $this->execsql($query);
		
		return $result->fetchAll();
		
	}  # cls_Lidond->overzichtlid
	
	public function islid($p_lidid, $p_ondid, $p_per="") {
		$this->vulvars(-1, $p_lidid, $p_ondid, $p_per);
		
		if ($this->loid == 0) {
			return false;
		} else {
			return true;
		}
	}  # cls_Lidond->islid
	
	public function loid($p_lidid, $p_ondid) {
		$this->vulvars(-1, $p_lidid, $p_ondid);
		return $this->loid;
	}
	
	public function ondid($p_loid=-1) {
		$this->vulvars($p_loid);
		return $this->ondid;
	}
	
	public function onderscheiding($p_lidid) {
		$query = sprintf("SELECT O.Naam FROM %s INNER JOIN %sOnderdl AS O ON LO.OnderdeelID=O.RecordID
						  WHERE LO.Lid=%d AND O.`Type`='O' AND IFNULL(LO.Opgezegd, '9999-12-31') > '%s' ORDER BY LO.Vanaf DESC;", $this->basefrom, TABLE_PREFIX, $p_lidid, $this->per);
		return $this->scalar($query);
	}  #  cls_Lidond->onderscheiding
	
	public function add($p_ondid, $p_lidid, $p_reden="", $p_log=1, $p_vanaf="", $p_toonlog=0) {
		$this->vulvars(-1, $p_lidid, $p_ondid);
		$this->tas = 11;
		
		$i_lm = new cls_Lidmaatschap(0, $p_lidid);

		$rv = false;
		$nrid = 0;
		
		if (strlen($p_vanaf) < 8) {
			if ($this->i_ond->type == "A" and $this->i_ond->alleenleden == 1) {
				$query = sprintf("SELECT IFNULL(MAX(LM.LIDDATUM), CURDATE()) FROM %sLidmaatschap AS LM WHERE LM.Lid=%d AND (LM.Opgezegd IS NULL) AND LM.LIDDATUM > DATE_SUB(CURDATE(), INTERVAL 1 MONTH);", TABLE_PREFIX, $this->lidid);
				$vanaf = $this->scalar($query);
			} elseif ($this->i_ond->alleenleden == 1 and $i_lm->soortlid($this->lidid) == "Voormalig lid") {
				$i_lm->vulvars(-1, $this->lidid);
				$vanaf = $i_lm->lidvanaf;
			} elseif ($this->i_ond->alleenleden == 1 and $i_lm->soortlid($this->lidid) == "Toekomstig lid") {
				$i_lm->vulvars(-1, $this->lidid);
				$vanaf = $i_lm->lidvanaf;
				
			} else {
				$vanaf = date("Y-m-d");
			}
		} else {
			$vanaf = $p_vanaf;
		}
		
		$contrqry = sprintf("SELECT COUNT(*) FROM %s WHERE LO.Lid=%d AND LO.OnderdeelID=%d AND LO.Vanaf='%s';", $this->basefrom, $this->lidid, $this->ondid, $vanaf);
		
		if (strlen($this->i_ond->naam) == 0) {
			$this->mess = sprintf("OnderdeelID %d bestaat niet. Record wordt niet toegevoegd.", $this->ondid);
			
		} elseif ($this->magmuteren == false and debug_backtrace()[1]['function'] != "autogroepenbijwerken") {
			$this->mess = sprintf("Je bent niet bevoegd om van onderdeel '%s' de leden te muteren.", $this->ondnaam);
			
		} elseif (strlen($this->lidnaam) == 0) {
			$this->mess = sprintf("Lid %d bestaat niet. Dit record wordt niet toegevoegd.", $this->lidid);
			
		} elseif ($this->i_ond->alleenleden == 1 and $i_lm->soortlid($this->lidid, $vanaf) !== "Lid") {
			$this->mess = sprintf("Dit record wordt niet toegevoegd, want de persoon is op %s geen lid.", $vanaf);
			
		} elseif ($this->scalar($contrqry) > 0) {
			$this->mess = sprintf("Dit record wordt niet toegevoegd, want de combinatie van %s, %s en vanaf (%s) al in de tabel staat.", $this->lidnaam, $this->i_ond->naam, $vanaf);
			
		} else {
			$nrid = $this->nieuwrecordid();
			$query = sprintf("INSERT INTO %s (RecordID, Lid, OnderdeelID, Vanaf, Functie, GroepID, Ingevoerd) VALUES (%d, %d, %d, '%s', 0, 0, NOW());", $this->table, $nrid, $this->lidid, $this->ondid, $vanaf);
			if ($this->execsql($query) > 0) {
				$this->loid = $nrid;
				$this->mess = sprintf("Lidond: record %d is per '%s' toegevoegd", $this->loid, $vanaf);
				if (strlen($p_reden) > 0) {
					$this->mess .= ", omdat " . $p_reden;
				}
				$this->interface($query);
				$rv = true;
			} else {
				$this->mess = sprintf("Geen record toegevoegd: %s", $query);
			}
		}
		$this->log($nrid, $p_toonlog, $this->ondid);
		return $rv;
	} # cls_Lidond->add
	
	public function update($p_loid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_loid);
		$this->tas = 12;
		$rv = false;
		$this->tm = 0;
		$this->ta = 6;
		
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
		} elseif ($p_kolom == "Vanaf" and strlen($p_waarde) < 10) {
			$this->mess = sprintf("%s: 'Vanaf' mag niet leeg zijn, deze aanpassing wordt niet verwerkt.", $this->table);
			$this->tm = 1;

		} elseif ($this->magmuteren == false and $p_kolom != "GroepID" and debug_backtrace()[1]['function'] != "autogroepenbijwerken" and debug_backtrace()[1]['function'] != "auto_einde") {
			$this->mess = sprintf("Je bent niet bevoegd om van onderdeel '%s' de leden te muteren.", $this->ondnaam);
			$this->tm = 1;
			
		} elseif ($p_kolom == "Vanaf" and $this->i_ond->alleenleden == 1 and (new cls_Lidmaatschap())->soortlid($this->lidid, $p_waarde) !== "Lid") {
			$this->mess = sprintf("De persoon is geen lid op %s, de aanpassing van vanaf wordt niet doorgevoerd.", $p_waarde);
			$this->tm = 1;
			
		} elseif ($this->typekolom($p_kolom) == "date" and strlen($p_waarde) > 5 and $p_waarde <= '1970-01-01') {
			$this->mess = sprintf("De waarde '%s' voor kolom '%s' is ongeldig, deze aanpassing wordt niet verwerkt.", $p_waarde, $p_kolom);
			$this->tm = 1;
			
		} else {
			if ($this->pdoupdate($this->loid, $p_kolom, $p_waarde, $p_reden) > 0) {
				$rv = true;
				if ($p_waarde > 0 and $p_kolom == "GroepID") {
					$this->omsgroep = (new cls_Groep(-1, $p_waarde))->groms;
					$this->mess = sprintf("Tabel Lidond: van record %d is kolom 'GroepID' in %d (%s) gewijzigd.", $this->loid, $p_waarde, $this->omsgroep);
				} elseif ($p_waarde > 0 and $p_kolom == "Functie") {
					$this->omsfunctie = (new cls_Functie($p_waarde))->naam;
					$this->mess = sprintf("Tabel Lidond: van record %d is kolom 'Functie' in %d (%s) gewijzigd.", $this->loid, $p_waarde, $this->omsfunctie);
				}
			}
		}
		$this->log($this->loid, 0, $this->ondid);
		return $rv;
	}  # cls_Lidond->update
	
	public function delete($p_loid, $p_reden="") {
		if ($this->tas != 33 and $this->tas != 39) {
			$this->tas = 13;
		}
		$this->vulvars($p_loid);
		$rv = 0;
		
		if ($this->magmuteren == true) {
			$rv = $this->pdodelete($this->loid, $p_reden);
			$delqry = sprintf("DELETE FROM %sAanwezigheid WHERE LidondID=%d;", TABLE_PREFIX, $this->loid);
			$a = $this->execsql($delqry);
			if ($a > 0) {
				$this->mess .= sprintf(". Er zijn van dit lid ook %d presentie-records verwijderd.", $a);
			}
		} else {
			$this->mess = sprintf("Je bent niet bevoegd om leden bij onderdeel %s te verwijderen", $this->ondnaam);
		}
		$this->log($this->loid, 0, $this->ondid);
		
		return $rv;
		
	}  # cls_Lidond->delete
	
	public function zeteigenschap($p_lidid, $p_ondid, $p_waarde) {
		//Deze functie wordt ook voor toestemmingen gebruikt, omdat dit technisch bijna hetzelfde werkt.
		
		$this->vulvars(-1, $p_lidid, $p_ondid);
		$rv = false;
		
		if ($p_waarde == 1) {
			if ($this->loid == 0) {
				if ($this->add($p_ondid, $p_lidid)) {
					$rv = true;
				}
			} elseif ($this->i_ond->type == "T") {
				if ($this->update($this->loid, "Vanaf", date("Y-m-d"))) {
					$rv = true;
				}
			}
		} else {
			$query = sprintf("SELECT LO.RecordID FROM %s WHERE LO.Lid=%d AND LO.OnderdeelID=%d AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE();", $this->basefrom, $p_lidid, $p_ondid);
			$result = $this->execsql($query);
			foreach ($result->fetchAll() as $row) {
				$this->update($row->RecordID, "Opgezegd", date("Y-m-d", strtotime("-1 day")));
				$rv = true;
			}
		}
		
		return $rv;
	}  # cls_Lidond->zeteigenschap
	
	public function opschonen($p_lidid=-1) {
		$i_ond = new cls_Onderdeel();
		
		if ($p_lidid > 0) {
			$wl = sprintf(" AND LO.Lid=%d", $p_lidid);
		} else {
			$wl = "";
		}
		
		$query = sprintf("SELECT LO.RecordID, O.HistorieOpschonen FROM %s AS LO INNER JOIN %sOnderdl AS O ON LO.OnderdeelID=O.RecordID
						  WHERE O.HistorieOpschonen > 1 AND IFNULL(LO.Opgezegd, '9999-12-31') < DATE_SUB(CURDATE(), INTERVAL O.HistorieOpschonen DAY)%s
						  ORDER BY LO.Lid, LO.OnderdeelID;", $this->table, TABLE_PREFIX, $wl);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$reden = sprintf("op basis van HistorieOpschonen (%d dagen)", $row->HistorieOpschonen);
			$this->delete($row->RecordID, $reden);
		}
		
		$query = sprintf('SELECT LO.RecordID FROM %s WHERE LO.OnderdeelID NOT IN (SELECT O.RecordID FROM %sOnderdl AS O);', $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$i_ond->delete($row->RecordID, "het onderdeel niet (meer) bestaat");
		}
		
		$query = sprintf("SELECT LO.RecordID FROM %s WHERE LO.Vanaf > IFNULL(LO.Opgezegd, '9999-12-31')%s;", $this->basefrom, $wl);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, "de datum vanaf na de datum opgezegd ligt");
		}
		
		$query = sprintf("SELECT LO.RecordID, IFNULL(LO.Opgezegd, '9999-12-31') AS Opgezegd, IFNULL(L.RecordID, 0) AS LidID, IFNULL(L.Overleden, '9999-12-31') AS Overleden, IFNULL(L.Verwijderd, '9999-12-31') AS Verwijderd
								FROM %s AS LO LEFT OUTER JOIN %sLid AS L ON LO.Lid=L.RecordID WHERE 1=1%s", $this->table, TABLE_PREFIX, $wl);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			if ($row->LidID == 0) {
				$this->delete($row->RecordID, "het record in de tabel Lid niet (meer) bestaat.");
			} elseif ($row->Overleden < $row->Opgezegd and $row->Overleden < date("Y-m-d")) {
				$this->update($row->RecordID, "Opgezegd", $row->Overleden, "het lid overleden is.");
			} elseif ($row->Verwijderd < date("Y-m-d")) {
				$this->delete($row->RecordID, "het lid verwijderd is.");
			}
		}
		
		/*
		-- Ik wil hier nog even over nadenken, dus maar even uitgezet.
		$query = sprintf("SELECT LO.RecordID FROM %s INNER JOIN %sGroep AS GR ON LO.GroepID=GR.RecordID WHERE LO.GroepID > 0 AND GR.ActiviteitID=0 AND IFNULL(LO.Opgezegd, '9999-12-31') < DATE_SUB(CURDATE(), INTERVAL 3 MONTH);", $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $lorow) {
			if ($this->update($lorow->RecordID, "GroepID", 0, "de activiteit is beëindigd.")) {
				$this->log($lorow->RecordID, 0);
			}
		}
		*/
		if ($p_lidid <= 0) {
			$this->optimize();
		}
		
	}  # cls_Lidond->opschonen
	
	public function controle() {

		$i_fnk = new cls_Functie();
		
		$lorows = $this->basislijst("LO.Functie > 0");
		
		foreach ($lorows as $lorow) {			
			$f = sprintf("F.Nummer=%d", $lorow->Functie);
			if ($lorow->Functie > 0 and $i_fnk->aantal($f) == 0) {
				$this->update($lorow->RecordID, "Functie", 0, "de functie niet bestaat.");
				$this->Log($lorow->RecordID, 0, $lorow->OnderdeelID);
			}
		}
		
	}  # cls_Lidond->controle
	
	public function autogroepenbijwerken($p_altijdlog=0, $p_interval=30, $p_ondid=-1) {
		
		$starttijd = microtime(true);
		$rv = 0;
		if ($p_interval <= 1) {
			$p_interval = 45;
		}
		
		$t = sprintf("-%d minutes", $p_interval);
		if (!isset($_SESSION['laatste_autogroepenbijwerken']) or $_SESSION['laatste_autogroepenbijwerken'] < date("Y-m-d H:i:s", strtotime($t))) {
			$f = "A.TypeActiviteit=2 AND A.TypeActiviteitSpecifiek=61";
			$lb = (new cls_Logboek())->max("A.DatumTijd", $f);
			$_SESSION['laatste_autogroepenbijwerken'] = $lb;
		} else {
			$lb = $_SESSION['laatste_autogroepenbijwerken'];
		}

		if ($lb < date("Y-m-d H:i:s", strtotime($t))) {
		
			$f = "LENGTH(O.MySQL) > 10 AND LEFT(O.MySQL, 7)='SELECT '";
			if ($p_ondid > 0) {
				$f .= sprintf(" AND O.RecordID=%d", $p_ondid);
			} else {
				$f .= " AND IFNULL(O.VervallenPer, '9999-12-31') >= CURDATE()";
			}
			
			$ondrows = (new cls_Onderdeel())->basislijst($f);
			foreach ($ondrows as $ondrow) {
				$this->vulvars(-1, -1, $ondrow->RecordID);
				$reden = "";
				if ($this->controleersql($ondrow->MySQL) == true) {
					$sourceres = $this->execsql($ondrow->MySQL);
					$pk = $sourceres->getColumnMeta(0)['name'];
					$sourcerows = $sourceres->fetchAll();
				
					$f = sprintf("OnderdeelID=%d", $ondrow->RecordID);
					$targetrows = $this->basislijst($f);
					foreach ($targetrows as $targetrow) {
						$aanw = false;
						foreach ($sourcerows as $sourcerow) {
							if ($sourcerow->{$pk} == $targetrow->Lid) {
								$aanw = true;
							}
						}
						$reden = sprintf("op basis van de MySQL-code hoort dit lid niet in %s.", $ondrow->Naam);
						if ($aanw == false and (strlen($targetrow->Opgezegd) == 0 or $targetrow->Opgezegd > date("Y-m-d", strtotime("yesterday")))) {
							$this->tas = 32;
							$this->update($targetrow->RecordID, "Opgezegd", date("Y-m-d", strtotime("yesterday")), $reden);
							$rv++;
						}
					}
			
					foreach($sourcerows as $sourcerow) {
						$lidid = $sourcerow->{$pk};
						$aanwqry = sprintf("SELECT COUNT(*) FROM %s WHERE %s AND LO.OnderdeelID=%d AND LO.Lid=%d;", $this->basefrom, cls_db_base::$wherelidond, $ondrow->RecordID, $lidid);
						if ($this->scalar($aanwqry) == 0) {
							$f = sprintf("Lid=%d AND OnderdeelID=%d AND Vanaf <= CURDATE() AND IFNULL(Opgezegd, '9999-12-31') >= DATE_SUB(CURDATE(), INTERVAL 7 DAY)", $lidid, $ondrow->RecordID);
							$reden = "op basis van de MySQL-code dit lid in deze groep hoort.";
							$rec = $this->max("RecordID", $f);
							if ($rec > 0) {
								$this->update($rec, "Opgezegd", '', $reden);
							} else {
								$this->add($ondrow->RecordID, $lidid, $reden, 1, '', 0);
							}
							$rv++;
						}
					}
				} else {
					$this->mess = sprintf("De MySQL-code van onderdeel %d (%s) is niet uitvoerbaar.", $ondrow->RecordID, $ondrow->Naam);
				}
				$this->log();
			}
		
			$_SESSION['laatste_autogroepenbijwerken'] = date("Y-m-d H:i:s");
		
			$exec_tijd = (microtime(true) - $starttijd);
			$this->lidid = 0;
			if ($rv > 0 or $p_altijdlog == 1 or ($_SESSION['settings']['performance_trage_select'] > 0 and $exec_tijd > $_SESSION['settings']['performance_trage_select'])) {
				if ($rv == 0) {
					$this->ta = 2;
					$this->tas = 61;
				}
				$this->mess = sprintf("cls_Lidond->autogroepenbijwerken in %.1f seconden uitgevoerd", $exec_tijd);
				$this->log(0, 0);
			}
		}

		return $rv;
		
	} # cls_Lidond->autogroepenbijwerken
	
	public function auto_einde($p_ondid=-1, $p_interval=90) {
		$starttijd = microtime(true);
		
		$rv = 0;
		$i_ond = new cls_Onderdeel();
		$i_lm = new cls_Lidmaatschap();
		if ($p_interval < 1) {
			$p_interval = 45;
		}
		
		$f = "A.TypeActiviteit=2 AND A.TypeActiviteitSpecifiek=62";
		$lagb = (new cls_Logboek())->max("A.DatumTijd", $f);
		
		if (strlen($lagb) < 10 or $lagb < date("Y-m-d H:i:s", strtotime(sprintf("-%d minutes", $p_interval)))) {
			
			$f = "O.`Alleen leden`=1 AND O.Gewijzigd <= DATE_SUB(NOW(), INTERVAL 6 HOUR)";
			if ($p_ondid > 0) {
				$f .= sprintf(" AND O.RecordID=%d", $p_ondid);
			}
			$ondrows = $i_ond->lijst(1, $f);
			foreach ($ondrows as $ondrow) {
				$lorows = $this->lijst($ondrow->RecordID, 3);
				foreach ($lorows as $lorow) {
					$i_lm->vulvars(0, $lorow->LidID);
					
					if ($i_lm->lidtm < "9999-12-31" and (($ondrow->Type == "A" and $i_lm->lidtm <= date("Y-m-d", strtotime("+6 month"))) or $i_lm->lidtm <= date("Y-m-d"))) {
						$this->update($lorow->RecordID, "Opgezegd", $i_lm->lidtm, sprintf("het lidmaatschap per %s is beëindigd.", $i_lm->lidtm));
						$rv++;
					}
				}
			}
	
			$query = sprintf("SELECT LO.RecordID, O.VervallenPer, O.Naam AS OndNaam FROM %s INNER JOIN %sOnderdl AS O ON O.RecordID=LO.OnderdeelID WHERE IFNULL(O.VervallenPer, '9999-12-31') < IFNULL(LO.Opgezegd, '9999-12-31') AND O.Gewijzigd <= DATE_SUB(NOW(), INTERVAL 6 HOUR);", $this->basefrom, TABLE_PREFIX);
			$result = $this->execsql($query);
			foreach ($result->fetchAll() as $row) {
				$reden = sprintf("%s per die datum beëindigd is.", $row->OndNaam);
				$this->update($row->RecordID, "Opgezegd", $row->VervallenPer, $reden);
				$rv++;
			}
			
			$exec_tijd = (microtime(true) - $starttijd);
			$this->ta = 2;
			$this->tas = 62;
			$this->lidid = 0;
			$this->mess = sprintf("cls_Lidond->auto_einde in %.1f seconden uitgevoerd.", $exec_tijd);
			$this->Log();
		}
		
		$i_ond = null;
		$i_lm = null;
		
		return $rv;
	}  # auto_einde
	
}  # cls_Lidond

class cls_Activiteit extends cls_db_base {
	public int $actid = 0; 			// RecordID van de activiteit
	public string $omschrijving = "";
	public float $contributie = 0;
	
	function __construct($p_actid=-1) {
		$this->table = TABLE_PREFIX . "Activiteit";
		$this->basefrom = $this->table . " AS Act";
		$this->vulvars($p_actid);
		$this->ta = 20;
	}
	
	public function vulvars($p_actid=-1) {
		if ($p_actid > 0) {
			$this->actid = $p_actid;
		}
		if ($this->actid > 0) {
			$query = sprintf("SELECT * FROM %s WHERE Act.RecordID=%d;", $this->basefrom, $this->actid);
			$row = $this->execsql($query)->fetch();
			if (isset($row->RecordID)) {
				$this->omschrijving = $row->Omschrijving ?? "";
				$this->naamlogging = $this->omschrijving;
				$this->contributie = $row->Contributie ?? 0;
			} else {
				$this->actid = 0;
			}
		}
	}
	
	public function htmloptions($p_cv=-1) {
		
		$ret = "";
		$rows = $this->basislijst("", "Act.Code");
		foreach ($rows as $row) {
			$ret .= sprintf("<option%s value=%d>%s</option>\n", checked($row->RecordID, "option", $p_cv), $row->RecordID, $row->Omschrijving);
		}
		return $ret;
	}
	
	public function add() {
		$nrid = $this->nieuwrecordid();
		$this->tas = 51;
		
		$this->query = sprintf("INSERT INTO %1\$s (RecordID, Omschrijving, Contributie, Ingevoerd) VALUES (%2\$d, 'Activiteit %2\$d', 0, NOW());", $this->table, $nrid);
		if ($this->execsql() > 0) {
			$this->mess = sprintf("Activiteit %d is toegevoegd.", $nrid);
			$this->Interface($this->query);
		}
		
		$this->log($nrid, 0);
	}
	
	public function update($p_actid, $p_kolom, $p_waarde) {
		$this->vulvars($p_actid);
		$this->tas = 52;
		
		if ($p_kolom == "Contributie") {
			if (intval($p_waarde) <= 0 or strlen($p_waarde) == 0) {
				$p_waarde = 0;
			} else {
				$p_waarde = round($p_waarde, 2);
			}
		}
		if (toegang("Ledenlijst/Basisgegevens/Activiteiten", 0, 0)) {
			$this->pdoupdate($p_actid, $p_kolom, $p_waarde);
		} else {
			$this->mess = "Je bent niet bevoegd om activiteiten aan te passen.";
		}
		$this->log($p_actid);
	}
	
	private function delete ($p_actid, $p_reden="") {
		$this->vulvars($p_actid);
		$this->tas = 53;
		
		if ($this->pdodelete($p_actid, $p_reden)) {
			$this->log($this->actid);
		}
	}
	
	public function opschonen() {
		$query = sprintf("SELECT Act.RecordID FROM %s WHERE DATE_ADD(Act.Ingevoerd, INTERVAL %d DAY) < CURDATE() AND (SELECT COUNT(*) FROM %sGroep AS GR WHERE GR.ActiviteitID=Act.RecordID)=0;", $this->basefrom, BEWAARTIJDNIEUWERECORDS, TABLE_PREFIX);
		$result = $this->execsql($query);
		foreach($result->fetchAll() as $row) {
			$this->delete($row->RecordID, "deze activiteit niet meer gebruikt wordt");
		}
		
		$this->optimize();
		
	}
	
	public function controle() {
		
	}
	
}  # cls_Activiteit

class cls_Groep extends cls_db_base {
	// Afdelingsgroepen
	
	public $afdid = 0;
	public $grid = -1;
	public $actid = 0;
	public $naam = "";
	public $groms = "";
	public $dpid = 0;
	public $starttijd = "";
	public $eindtijd = "";
	public $tijden = "";
	public $instructeurs = "";
	public $aantalingroep = 0;		// Het aantal leden die op dit moment in deze groep zitten.
	public $aantalmetgroep = 0;	// Het aantal records waar deze groep aan gekoppeld is, ongeacht of deze nog actueel zijn.
	
	public object $i_dp;
	public object $i_act;
	
	function __construct($p_afdid=-1, $p_grid=-1) {
		$this->table = TABLE_PREFIX . "Groep";
		$this->basefrom = $this->table . " AS GR";
		$this->ta = 20;
		$this->per = date("Y-m-d");
		$this->vulvars($p_afdid, $p_grid);
	}
	
	public function vulvars($p_afdid=-1, $p_grid=-1) {
		
		if ($p_afdid >= 0) {
			$this->afdid = $p_afdid;
		}
		if ($p_grid >= 0) {
			$this->grid = $p_grid;
		}
		
		$this->actid = 0;
		$this->aantalingroep = 0;
		if ($this->grid > 0) {
			$query = sprintf("SELECT GR.* FROM %s WHERE GR.RecordID=%d;", $this->basefrom, $this->grid);
			$result = $this->execsql($query);
			$row = $result->fetch();
			if (isset($row->RecordID) and $row->RecordID > 0) {
				if (strlen(trim($row->Omschrijving)) > 0) {
					$this->naam = trim($row->Omschrijving ?? "");
				} elseif ($row->DiplomaID > 0) {
					$this->naam = (new cls_Diploma())->naam($row->DiplomaID);
				} else {
					$this->naam = "Groep " . $this->grid ?? 0;
				}
				$this->groms = $this->naam;
				$this->instructeurs = trim($row->Instructeurs ?? "");
				if (strlen($this->instructeurs) > 1) {
					$this->groms .= " | " . $this->instructeurs;
				}
				$this->naamlogging = $this->naam;
				
				$this->afdid = $row->OnderdeelID ?? 0;
				$this->dpid = $row->DiplomaID ?? 0;
				$this->actid = $row->ActiviteitID ?? 0;
				
				$this->starttijd = trim($row->Starttijd ?? "");
				$this->eindtijd = trim($row->Eindtijd ?? "");
				$this->tijden = $this->starttijd;
				if (strlen($this->eindtijd) > 3) {
					$this->tijden .= " - " . $this->eindtijd . " uur";
				}
				
				$alqry = sprintf("SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.GroepID=%d AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE();", TABLE_PREFIX, $this->grid);
				$this->aantalingroep = $this->scalar($alqry);
				
				$alqry = sprintf("SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.GroepID=%d;", TABLE_PREFIX, $this->grid);
				$this->aantalmetgroep = $this->scalar($alqry);
				
			} else {
				$this->grid = 0;
				$this->dpid = 0;
			}
		} else {
			$this->naam = "Niet ingedeeld";
			$this->groms = "Niet ingedeeld";
		}
		
		$this->i_dp = new cls_Diploma($this->dpid);
		$this->i_act = new cls_Activiteit($this->actid);
		
		if ($this->afdid > 0) {
			$query = sprintf("SELECT COUNT(*) FROM %sOnderdl AS O WHERE O.`Type`='A' AND O.RecordID=%d;", TABLE_PREFIX, $this->afdid);
			if ($this->scalar($query) == 0) {
				$this->afdid = 0;
			}
		}
				
	}  # cls_Groep->vulvars
	
	public function record($p_grid=-1) {
		$this->vulvars(-1, $p_grid);
		
		if ($this->grid > 0) {
			$query = sprintf("SELECT GR.* FROM %s WHERE GR.RecordID=%d;", $this->basefrom, $this->grid);
			$result = $this->execsql($query);
			return $result->fetch();
		} else {
			return false;
		}
	}  # cls_Groep->record
	
	public function selectlijst($p_afdid=-1, $p_order="") {
		$this->vulvars($p_afdid, 0);
		if (strlen($p_order) > 0) {
			$p_order .= ", ";
		}
		
		$query = sprintf("SELECT GR.*,
						  IF(GR.RecordID=0, 'Niet ingedeeld', CONCAT(IF(LENGTH(GR.Kode)>0, GR.Kode, GR.RecordID), ' - ', GR.Omschrijving)) AS GroepOms,
						  CASE 
							WHEN GR.RecordID=0 THEN 'Niet ingedeeld'
							WHEN LENGTH(Instructeurs) > 1 THEN CONCAT(GR.Kode, ' - ', GR.Omschrijving, ' | ', GR.Instructeurs)
							ELSE CONCAT(GR.Kode, ' - ', GR.Omschrijving)
						  END AS GroepOmsIns,
						  CONCAT(GR.Starttijd, IF(LENGTH(GR.Eindtijd) > 3, CONCAT(' - ', GR.Eindtijd, ' uur'), '')) as Tijden,
						  (SELECT COUNT(*) FROM %1\$sLidond AS LO WHERE LO.GroepID=GR.RecordID AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE() AND LO.OnderdeelID=%3\$d) AS aantalInGroep
						  FROM %2\$s
						  WHERE (GR.OnderdeelID=%3\$d OR GR.RecordID=0)
						  ORDER BY GR.Starttijd, GR.Volgnummer, GR.Omschrijving, GR.RecordID;", TABLE_PREFIX, $this->basefrom, $this->afdid);
		$result = $this->execsql($query);
		return $result->fetchAll();
		
	}  # cls_Groep->selectielijst
	
	public function htmloptions($p_cv=-1, $p_ondid=-1) {
		$rv = "";
		
		if ($p_ondid > 0) {
			$w = sprintf("(GR.OnderdeelID=%d OR GR.RecordID=0)", $p_ondid);
		} else {
			$w = "";
		}
		
		$grrows = $this->basislijst($w, "GR.Kode, GR.RecordID");
		foreach ($grrows as $grrow) {
			if (strlen(trim($grrow->Kode)) > 0) {
				$toon = $grrow->Kode;
			} elseif ($grrow->RecordID > 0) {
				$toon = $grrow->RecordID;
			} else {
				$toon = "Geen";
			}
			$rv .= sprintf("<option value=%d %s>%s</option>\n", $grrow->RecordID, checked($p_cv, "option", $grrow->RecordID), $toon);
		}
		
		return $rv;
	}  # cls_Groep->htmloptions
	
	public function add($p_afdid, $p_kode="") {
		$this->vulvars($p_afdid);
		$this->tas = 61;
		$nrid = 0;
		
		if ($this->afdid > 0) {
			$nrid = $this->nieuwrecordid();
		
			$this->query = sprintf("INSERT INTO %s (RecordID, OnderdeelID, Volgnummer, Kode, DiplomaID, Ingevoerd) VALUES (%d, %d, 1, '%s', 0, NOW());", $this->table, $nrid, $this->afdid, $p_kode);
			if ($this->execsql() > 0) {
				$this->mess = sprintf("Groep %d is toegevoegd.", $nrid);
				$this->Interface($this->query);
			}
		} else {
			$this->mess = "De groep is niet toegevoegd, omdat er geen (bestaande) afdeling is opgegeven.";
		}
		
		$this->log($nrid, 0, $this->afdid);
	}  # cls_Groep->add
	
	public function update($p_grid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars(-1, $p_grid);
		$this->tas = 62;
		
		if (($p_kolom == "Volgnummer" or $p_kolom == "Aanwezigheidsnorm") and (intval($p_waarde) <= 0 or strlen($p_waarde) == 0)) {
			$p_waarde = 0;
		} elseif ($p_kolom == "Aanwezigheidsnorm" and intval($p_waarde) > 100) {
			$p_waarde = 100;
		} elseif (($p_kolom == "Starttijd" or $p_kolom == "Eindtijd") and strlen($p_waarde) == 4 and substr($p_waarde, 0, 1) != "0") {
			$p_waarde = "0" . $p_waarde;
		}
		if ($this->pdoupdate($p_grid, $p_kolom, $p_waarde, $p_reden)) {
			$this->log($p_grid, 0, $this->afdid);
		}
	}  # cls_Groep->update
	
	public function delete($p_grid, $p_reden="") {
		$this->vulvars(-1, $p_grid);
		$this->tas = 63;
		
		$cntrqry = sprintf("SELECT COUNT(*) FROM %sLidond AS LO WHERE GroepID=%d", TABLE_PREFIX, $this->grid);
		if ($this->scalar($cntrqry) > 0) {
			$this->mess = sprintf("Groep %d wordt niet verwijderd, omdat deze nog in gebruik is.", $p_grid);
		} elseif ($this->pdodelete($p_grid, $p_reden) > 0) {

		} else {
			$this->mess = sprintf("Groep %d is niet verwijderd, omdat deze niet bestaat.", $p_grid);
		}
		$this->log($p_grid, 0, $this->afdid);
	}
	
	public function opschonen() {
		$query = sprintf("SELECT GR.RecordID FROM %s WHERE DATE_ADD(GR.Ingevoerd, INTERVAL %d DAY) < CURDATE() AND (SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.GroepiD=GR.RecordID)=0;", $this->basefrom, BEWAARTIJDNIEUWERECORDS, TABLE_PREFIX);
		$result = $this->execsql($query);
		foreach($result->fetchAll() as $row) {
			$this->delete($row->RecordID, "deze groep niet meer gebruikt wordt");
		}
		
		$this->optimize();
		
	}  # opschonen
	
	public function controle() {
		$i_dp = new cls_Diploma();
		
		$query = sprintf("SELECT GR.* FROM %s;", $this->basefrom);
		$result = $this->execsql($query);
		foreach($result->fetchAll() as $row) {
			if (strlen($row->Omschrijving) == 0 and $row->DiplomaID > 0) {
				$i_dp->vulvars($row->DiplomaID);
				$this->update($row->RecordID, "Omschrijving", $i_dp->naam);
			} elseif (strlen($row->Starttijd) > 0 and validTime($row->Starttijd) == false) {
				$this->update($row->RecordID, "Starttijd", "", "de starttijd geen geldige tijd is.");
			} elseif (strlen($row->Eindtijd) > 0 and validTime($row->Eindtijd) == false) {
				$this->update($row->RecordID, "Eindtijd", "", "de eindtijd geen geldige tijd is.");
			} elseif (strlen($row->Eindtijd) > 0 and $row->Eindtijd < $row->Starttijd) {
				$this->update($row->RecordID, "Eindtijd", "", "de eindtijd voor de starttijd ligt.");
			}
		}
	}  # cls_Groep->controle
	
}  # cls_Groep

class cls_Login extends cls_db_base {
	
	private $login = "";
	public $loginid = 0;
	public $beperkttotgroep;
	private $inloggentoegestaan = false;
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
	
	private function vulvars($p_lidid=-1, $p_email="", $p_lidnr=0) {
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		
		$p_email = trim(strtolower($p_email));
		if (isValidMailAddress($p_email, 0)) {
			$this->filteremail = sprintf("(LOWER(L.Email)='%1\$s' OR LOWER(L.EmailVereniging)='%1\$s' OR LOWER(L.EmailOuders)='%1\$s')", $p_email);
		} else {
			$this->filteremail = "";
		}
		
		if ($this->lidid <= 0 && strlen($this->filteremail) > 5) {
			$query = sprintf("SELECT L.RecordID FROM %sLid AS L WHERE %s;", TABLE_PREFIX, $this->filteremail);
			$rows = $this->execsql($query)->fetchAll();		
			if (count($rows) == 0) {
				$this->mess = sprintf("E-mailadres '%s' is onbekend in onze database.", $p_email);
				$this->lidid = 0;
			} elseif (count($rows) > 1) {
				$this->mess = sprintf("E-mailadres '%s' is aan meedere leden gekoppeld.", $p_email);
				$this->lidid = 0;
			} else {
				$this->lidid = $rows[0]->RecordID;
			}
		}
		
		if ($p_lidnr > 0 && $this->lidid <= 0) {
			$query = sprintf("SELECT IFNULL(MAX(LM.Lid), 0) FROM %sLidmaatschap AS LM WHERE LM.Lidnr=%d AND IFNULL(LM.Opgezegd, '9999-12-31') >= CURDATE();", TABLE_PREFIX, $p_lidnr);
			$this->lidid = $this->scalar($query);
			if ($this->lidid == 0) {
				$this->mess = sprintf("Lidnummer %d is onbekend in onze database.", $p_lidnr);
			}
		}
		
		if ($this->lidid > 0) {
			$query = sprintf("SELECT RecordID, Login FROM %s WHERE LidID=%d;", $this->table, $this->lidid);
			$row = $this->execsql($query)->fetch();
			if (isset($row->RecordID)) {
				$this->loginid = $row->RecordID;
				$this->login = $row->Login;
			}
			
			if (WEBMASTER) {
				$this->inloggentoegestaan = true;
			} else {
				$query = sprintf("SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE() AND LO.Lid=%d AND LO.OnderdeelID IN (%s);", TABLE_PREFIX, $this->lidid, implode(",", $this->beperkttotgroep));
				if ($this->scalar($query) > 0) {
					$this->inloggentoegestaan =  true;
				} else {
					$this->mess = "Dit lid mag niet inloggen.";
					$this->inloggentoegestaan = false;
				}
			}
		} else {
			$this->inloggentoegestaan = false;
		}
		
	}  # cls_Login->vulvars

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
		
		$query = sprintf("SELECT Login.Login, %s AS NaamLid, L.Woonplaats, (%s) AS Lidnr, IF(LENGTH(IFNULL(L.Email, ''))=0, L.EmailOuders, L.Email) AS `E-mail`, Login.Ingevoerd, Login.LastLogin, 
					IF(LENGTH(Login.Wachtwoord) > 5 AND LENGTH(IFNULL(Login.ActivatieKey, ''))=0, IF(Login.Ingelogd=1, 'Ingelogd', 'Gevalideerd'), 'Niet gevalideerd') AS `Status`,
					IF(Login.FouteLogin > 0, Login.LidID, 0) AS `Unlock`, Login.LidID,
					IF(LENGTH(IFNULL(Login.ActivatieKey, ''))>0, Login.LidID, 0) AS ValLink,
					L.Roepnaam, L.Telefoon, L.Mobiel, L.Achternaam, L.Tussenv, L.Meisjesnm, L.GEBDATUM, L.EmailVereniging, Login.LidID,
					Login.Wachtwoord
					FROM %s INNER JOIN %sLid AS L ON Login.LidID=L.RecordID
					WHERE LENGTH(Login.Login) > 5 %s
					ORDER BY %sL.Achternaam, L.TUSSENV, L.Roepnaam;", $this->selectnaam, $this->selectlidnr, $this->basefrom, TABLE_PREFIX, $filter, $p_orderby);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}  # cls_Login->lijst
	
	public function record($p_lidid, $p_email="") {
		$this->vulvars($p_lidid, $p_email);
		
		$query = sprintf("SELECT Login.*, %s AS Naam, L.Roepnaam, L.Email, Login.LidID, ", $this->selectnaam);
		
		$query .= sprintf("(SELECT COUNT(*) FROM %sLidond AS LO WHERE LO.Lid=L.RecordID AND LO.OnderdeelID IN (%s) AND LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE()) AS AanvragenMag", TABLE_PREFIX, implode(", ", $this->beperkttotgroep));
		$query .= sprintf(" FROM %sLid AS L LEFT JOIN %s ON L.RecordID=Login.LidID WHERE ", TABLE_PREFIX, $this->basefrom);
		
		if (strlen($this->filteremail) > 0) {
			$query .= $this->filteremail;
		} else {
			$query .= sprintf("L.RecordID=%d;", $p_lidid);
		}
		
		$result = $this->execsql($query);
		return $result->fetch();
	}  # cls_Login->record
	
	public function lididherstel($p_login, $p_lidnr, $p_email) {
		$this->vulvars(0, $p_email, $p_lidnr);
		$rv = false;
		
		if ($this->lidid <= 0 && strlen($p_login) >= 5) {
			$query = sprintf("SELECT IFNULL(MAX(Login.LidID), 0) FROM %s WHERE Login.Login='%s';", $this->basefrom, $p_login);
			$this->lidid = $this->scalar($query);
			if ($this->lidid == 0) {
				$this->mess = sprintf("Login '%s' is onbekend in onze database.", $p_login);
			}
		}
		
		if ($this->lidid > 0 && $this->inloggentoegestaan == true) {
			$rv = $this->lidid;
		} elseif ($this->lidid == 0 && strlen($this->filteremail) == 0 && strlen($p_login) == 0) {
			$this->mess = "Er is geen (geldig) e-mailadres ingevoerd.";
		} elseif ($this->lidid > 0 && strlen($p_login) == 0) {
			$this->mess = "Dit lid mag niet inloggen.";
		}
		
		return $rv;
	}  # lididherstel
	
	public function lidid($p_email, $p_lidnr=0) {
		$this->vulvars(0, $p_email, $p_lidnr);
		return $this->lidid;
	}
	
	public function nuingelogd() {
		
		$query = sprintf("SELECT %s AS NaamLid FROM %s INNER JOIN %sLid AS L ON L.RecordID=Login.LidID WHERE Login.Ingelogd=1;", $this->selectnaam, $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		$rv = "";
		foreach($result->fetchAll() as $row) {
			if (strlen($rv) > 1) {
				$rv .= ", ";
			}
			$rv .= $row->NaamLid;
		}

		return $rv;
	}
	
	public function aanvragenmag($p_lidid) {
		$this->vulvars($p_lidid);
		return $this->inloggentoegestaan;
	}

	public function lididbijlogin($p_login) {
		$query = sprintf("SELECT IFNULL(MAX(LidID), 0) FROM %s WHERE Login='%s';", $this->basefrom, $p_login);
		$this->lidid = $this->scalar($query);
		return $this->lidid;
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
		$query = sprintf("SELECT Login.LidID, Login.Login, Login.Wachtwoord, Login.ActivatieKey, Login.FouteLogin, Login.Gewijzigd
					FROM %s WHERE LENGTH(Login.Wachtwoord) > 5 %s;", $this->basefrom, $xw);
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
			$query = sprintf("UPDATE %sAdmin_login SET ActivatieKey='%s', Wachtwoord='', LaatsteWachtwoordWijziging=SYSDATE(), Gewijzigd=SYSDATE() WHERE LidID=%d;", TABLE_PREFIX, password_hash($nk, PASSWORD_DEFAULT), $p_lidid);
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
	}  # cls_Login->add
	
	public function update($p_lidid, $p_kolom, $p_waarde, $p_opstart=0) {
		$this->tas = 2;
		
		if (WEBMASTER or $p_opstart == 1) {
			$this->vulvars($p_lidid);
			if ($this->pdoupdate($this->loginid, $p_kolom, $p_waarde) > 0) {
				$this->mess = sprintf("Kolom '%s' van login '%s' in '%s' gewijzigd.", $p_kolom, $this->login, $p_waarde);
			}
		} else {
			$this->mess = "Je bent niet bevoegd om logins te wijzigen.";
		}
		$this->log($this->loginid);
	}  # cls_Login->update
	
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
	}  # cls_Login->autounlock

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
			$_SESSION['lidid'] = $this->lidid;
			$_SESSION['lidgroepen'] = (new cls_Lidond())->lidgroepen();
			$this->log();
		} elseif ($_SESSION['lidid'] > 0) {
			$this->query = sprintf("UPDATE %s SET LastActivity=SYSDATE() WHERE LidID=%d;", $this->table, $_SESSION['lidid']);
			$this->execsql();
		}
	}  # cls_Login->setingelogd
	
	public function uitloggen($p_lidid=0) {
		$this->lidid = $p_lidid;
		$this->ta = 1;
		$this->tas = 2;
		$rv = 0;
		$this->mess = "";
		if ($this->lidid > 0) {
			$query = sprintf("UPDATE %s SET Ingelogd=0 WHERE Ingelogd > 0 AND LidID=%d;", $this->table, $this->lidid);
			if ($this->execsql($query) > 0) {
				$this->mess = "Heeft uitgelogd.";
				$this->log();
				$_SESSION['lidid'] = 0;
				$_SESSION['lidgroepen'] = null;
				$_SESSION['lidauth'] = null;
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
					$this->log();
					$rv++;
				}
			}
			$this->lidid = 0;
		}
		if ($rv > 0) {
			(new cls_Lidond())->auto_einde();
			(new cls_Lidond())->autogroepenbijwerken(0);
			(new cls_Eigen_lijst())->controle(-1, 0, 1);
			sentoutbox(2);
			if (function_exists("fnMaatwerkNaUitloggen")) {
				fnMaatwerkNaUitloggen();
			}
		}
		return $rv;
		
	}  # cls_Login->uitloggen
	
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
			$query = sprintf("UPDATE %s SET Gewijzigd=SYSDATE(), Wachtwoord='%s', LaatsteWachtwoordWijziging=SYSDATE(), ActivatieKey='' WHERE LidID=%d;", $this->table, password_hash($p_nieuw, PASSWORD_DEFAULT), $p_lidid);
			if ($this->execsql($query) > 0) {
				$this->mess = "Het wachtwoord is gewijzigd.";
				if (function_exists("fnMaatwerkNaWijzigenWachtwoord")) {
					fnMaatwerkNaWijzigenWachtwoord($p_lidid, $p_nieuw);
				}
			} else {
				$this->mess = "Het wachtwoord is niet gewijzigd.";
			}
		}
		$this->log(0, 1);
		return $this->mess;
	}  # cls_Login->wijzigenwachtwoord
	
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
			$query = sprintf("SELECT LidID FROM %s WHERE IFNULL(LastLogin, '1970-01-01') < '2012-01-01' AND Ingevoerd < DATE_ADD(CURDATE(), INTERVAL -%d DAY);", $this->table, $_SESSION['settings']['login_bewaartijdnietgebruikt']);
			$result = $this->execsql($query);
			foreach ($result->fetchAll() as $row) {
				$this->delete($row->LidID, $reden);
			}
		}
		
		$this->optimize();
		
	}  # cls_Login->opschonen
	
}  # cls_Login

class cls_Mailing extends cls_db_base {
	
	private int $mid = 0;
	public string $zichtbaarwhere = "";
	
	function __construct($p_mid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Mailing";
		$this->basefrom = $this->table . " AS M";
		$this->ta = 4;
		$this->vulvars($p_mid);
		if (WEBMASTER == false and in_array($_SESSION['settings']['mailing_alle_zien'], explode(",", $_SESSION['lidgroepen'])) == false) {
			$this->zichtbaarwhere = sprintf("M.ZichtbaarVoor IN (%s)", $_SESSION['lidgroepen']);
		}
	}
	
	private function vulvars($p_mid=-1) {
		if ($p_mid >= 0) {
			$this->mid = $p_mid;
		}
		if ($this->mid > 0) {
			$query = sprintf("SELECT M.subject FROM %s WHERE M.RecordID=%d;", $this->basefrom, $this->mid);
			$row = $this->execsql($query)->fetch();
			if (isset($row->subject)) {
				$this->naamlogging = $row->subject ?? "";
			}
		}
	}  # cls_Mailing->vulvars
	
	public function record($p_mid) {
		$query = sprintf("SELECT M.*, MV.Vanaf_naam, MV.Vanaf_email FROM %s LEFT OUTER JOIN %sMailing_vanaf AS MV ON M.MailingVanafID=MV.RecordID WHERE M.RecordID=%d;", $this->basefrom, TABLE_PREFIX, $p_mid);
		$result = $this->execsql($query);
		return $result->fetch();
	}  # cls_Mailing->record
	
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
			$xtra_sel = sprintf(", M.deleted_on", $this->fdtlang);
		
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
		
		$query = sprintf("SELECT M.RecordID, CONCAT(M.subject, ' & ', IFNULL(M.Opmerking, '')) AS Onderwerp_Opmerking, MV.Vanaf_naam, M.subject, M.Opmerking, M.OmschrijvingOntvangers,
					CONCAT(MV.Vanaf_naam, ' & ', M.OmschrijvingOntvangers) AS Van_Aan,
					(SELECT IFNULL(COUNT(*), 0) FROM %3\$sMailing_rcpt AS MR WHERE MR.MailingID=M.RecordID) AS aantalOntvangers%2\$s,
					IF((SELECT COUNT(*) FROM %3\$sMailing_hist AS MH WHERE MH.MailingID=M.RecordID) > 0, M.RecordID, 0) AS linkHist
					FROM %3\$sMailing AS M LEFT OUTER JOIN %3\$sMailing_vanaf AS MV ON M.MailingVanafID=MV.RecordID
					WHERE %4\$s ORDER BY %5\$s;", $this->selectnaam, $xtra_sel, TABLE_PREFIX, $w, $orderby, $this->fdlang);
		$result = $this->execsql($query);
		return $result->fetchAll();
		
	}  # cls_Mailing->lijst
	
	public function htmloptions($p_cv=-1) {
		$rv = "";
		$f = sprintf("(M.RecordID NOT IN (%d, %d, %d, %d, %d, %d))", $_SESSION['settings']['mailing_lidnr'], $_SESSION['settings']['mailing_validatielogin'], $_SESSION['settings']['mailing_herstellenwachtwoord'],
						$_SESSION['settings']['mailing_bewakinginschrijving'], $_SESSION['settings']['mailing_bevestigingbestelling'], $_SESSION['settings']['mailing_bevestigingopzegging']);
		foreach ($this->lijst($f) as $row) {
			$o = $row->subject;
			if (strlen($row->Opmerking) > 0) {
				$o .= " - " . $row->Opmerking;
			}
			if (strlen($row->OmschrijvingOntvangers) > 0) {
				$o .= " - " . $row->OmschrijvingOntvangers;
			}
			if ($row->aantalOntvangers > 1) {
				$o .= sprintf(" (%d ontvangers)", $row->aantalOntvangers);
			}
			$s = "";
			if ($p_cv == $row->RecordID) {
				$s = " selected";
			}
			$rv .= sprintf("<option value=%d%s>%s</option>\n", $row->RecordID, $s, $o);
		}
		return $rv;
	}  # cls_Mailing->htmloptions
	
	public function bestaat($p_mid) {
		$query = sprintf("SELECT COUNT(*) FROM %s WHERE M.RecordID=%d AND IFNULL(M.deleted_on, '1900-01-01') < '2000-01-01';", $this->basefrom, $p_mid);
		$result = $this->execsql($query);
		if ($result->fetchColumn() == 1) {
			$this->mid = $p_mid;
			return true;
		} else {
			return false;
		}
	}
	
	public function mogelijkeontvangers($p_mid, $p_filter=0) {
		
		/*
			p_filter
				0 = alle personen in de tabel lid
				1 = leden en toekomstige leden
				2 = alleen klosleden
				3 = alleen voormarig leden
		*/
		
		$query = sprintf("SELECT L.RecordID AS LidID, %1\$s AS Zoeknaam_lid FROM %2\$sLid AS L
					WHERE (L.Verwijderd IS NULL) AND (L.Overleden IS NULL) AND (LENGTH(L.Email) > 5 OR LENGTH(L.EmailVereniging) > 5 OR LENGTH(L.EmailOuders) > 5)
					AND (L.RecordID NOT IN (SELECT R.LidID FROM %2\$sMailing_rcpt AS R WHERE R.MailingID=%3\$d))", $this->selectzoeknaam, TABLE_PREFIX, $p_mid);
		if ($p_filter == 1) {
			$query .= sprintf(" AND L.RecordID IN (SELECT LM.Lid FROM %sLidmaatschap AS LM WHERE IFNULL(LM.Opgezegd, '9999-12-31')  >= CURDATE())", TABLE_PREFIX);
		} elseif ($p_filter == 2) {
			$query .= sprintf(" AND L.RecordID NOT IN (SELECT LM.Lid FROM %sLidmaatschap AS LM)", TABLE_PREFIX);
		} elseif ($p_filter == 1) {
			$query .= sprintf(" AND L.RecordID IN (SELECT LM.Lid FROM %sLidmaatschap AS LM WHERE IFNULL(LM.Opgezegd, '9999-12-31') < CURDATE())", TABLE_PREFIX, $this->wherelid);
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
		
			$query = sprintf("INSERT INTO %s (RecordID, subject, OmschrijvingOntvangers) VALUES (%d, \"%s\", '');", $this->table, $nrid, $p_subject, $_SESSION['lidid']);
			if ($this->execsql($query) > 0) {
				$this->mess = sprintf("Mailing %d (%s) is toegevoegd.", $nrid, $p_subject);
			} else {
				$this->mess = "Geen mailing toegevoegd.";
				$nrid = 0;
			}
		}
		
		$this->log($nrid);
		return $nrid;
	}  # cls_Mailing->add
	
	public function update($p_mid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_mid);
		$this->tas = 2;
		
		if ($p_kolom == "cc_addr") {
			$p_waarde = str_replace(" ", "", trim($p_waarde));
			$p_waarde = str_replace(";", ",", $p_waarde);
		}
		
		if ($p_kolom == "subject" and strlen($p_waarde) < 4) {
			$this->mess = "Het onderwerp moeten minimaal uit vier karakters bestaan. Deze aanpassing wordt niet verwerkt.";

		} elseif (toegang("Mailing/Muteren")) {
			$this->pdoupdate($this->mid, $p_kolom, $p_waarde, $p_reden);

		}
		$this->log($this->mid);
	}  # cls_Mailing->update
	
	public function delete($p_mid, $p_reden="") {
		$this->vulvars($p_mid);
		$this->tas = 3;
		
		if ($this->pdodelete($this->mid, $p_reden) > 0) {
			$this->log($this->mid);
			$query = sprintf("DELETE FROM %sMailing_rcpt WHERE MailingID=%d;", TABLE_PREFIX, $this->mid);
			$this->execsql($query);
		}
	}  # cls_Mailing->delete
	
	public function trash($p_mid, $p_direction="in") {
		$this->vulvars($p_mid);
		$this->tas = 4;
		
		if ($p_direction == "in") {
			$this->query = sprintf("UPDATE %s SET deleted_on=NOW() WHERE RecordID=%d AND (deleted_on IS NULL);", $this->table, $this->mid);
			if ($this->execsql() > 0) {
				$this->mess = sprintf("Mailing %d (%s) is naar de prullenbak verplaatst.", $this->mid, $this->naamlogging);
			}
		} else {
			$this->query = sprintf("UPDATE %s SET deleted_on=NULL WHERE RecordID=%d AND (deleted_on IS NOT NULL);", $this->table, $this->mid);
			if ($this->execsql() > 0) {
				$this->mess = sprintf("Mailing %d (%s) is uit de prullenbak gehaald.", $this->mid, $this->naamlogging);
			}
		}
		$this->refcolumn = "deleted_on";
		
		$this->log($p_mid);
	}  # cls_Mailing->trash
	
	public function controle() {
		
		foreach($this->basislijst() as $mrow) {
			if (strlen($mrow->OmschrijvingOntvangers) == 0 and strlen($mrow->to_name) > 0) {
				$this->update($mrow->RecordID, "OmschrijvingOntvangers", $mrow->to_name, "de inhoud is overgezet");
				$this->log($mrow->RecordID);
			}
		}
		
	}  # cls_Mailing->controle
	
	public function opschonen() {
		if ($_SESSION['settings']['mailing_bewaartijd'] > 0) {
			$reden = sprintf("de mailing langer dan %d maanden geleden in de prullenbak is geplaatst", $_SESSION['settings']['mailing_bewaartijd']);
			$f = sprintf("IFNULL(M.deleted_on, '9999-12-31') < DATE_SUB(CURDATE(), INTERVAL %d MONTH)", $_SESSION['settings']['mailing_bewaartijd']);
			$f .= sprintf(" AND (M.RecordID NOT IN (SELECT MH.MailingID FROM %sMailing_hist AS MH))", TABLE_PREFIX);
			foreach ($this->basislijst($f) as $mrow) {
				$this->delete($mrow->RecordID, $reden);
			}
		}
		
		$this->optimize();

	}  # cls_Mailing->opschonen
		
}  # cls_Mailing

class cls_Mailing_hist extends cls_db_base {
	
	private $mhid = 0;
	private $mid = 0;
	private $subjectmailing = "";
	private $zichtbaarwhere = "";
	private $limit = 1500;
		
	function __construct($p_mhid=-1, $p_mid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Mailing_hist";
		$this->basefrom = $this->table . " AS MH";
		if (WEBMASTER == false and in_array($_SESSION['settings']['mailing_alle_zien'], explode(",", $_SESSION['lidgroepen'])) == false) {
			$this->zichtbaarwhere = sprintf(" AND (MH.ZichtbaarVoor IN (%s) OR MH.LidID=%d)", $_SESSION['lidgroepen'], $_SESSION['lidid']);
		}
		$this->ta = 4;
		$this->vulvars($p_mhid, $p_mid);
	}
	
	private function vulvars($p_mhid=-1, $p_mid=-1) {
		if ($p_mhid >= 0) {
			$this->mhid = $p_mhid;
		}
		if ($p_mid >= 0) {
			$this->mid = $p_mid;
		}
		if ($this->mhid > 0) {
			$query = sprintf("SELECT MH.RecordID, MH.LidID, MH.MailingID, MH.subject FROM %s WHERE MH.RecordID=%d;", $this->basefrom, $this->mhid);
			$result = $this->execsql($query);
			$row = $result->fetch();
			if (isset($row->RecordID)) {
				$this->lidid = $row->LidID;
				$this->mid = $row->MailingID;
				$this->naamlogging = $row->subject ?? "";
			}
		}
		if ($this->mid > 0) {
			$query = sprintf("SELECT IFNULL(MAX(M.subject), '') FROM %sMailing AS M WHERE M.RecordID=%d;", TABLE_PREFIX, $this->mid);
			$this->subjectmailing = $this->scalar($query);
			if (strlen($this->naamlogging) == 0) {
				$this->naamlogging = $this->subjectmailing;
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
		
		$query = sprintf("SELECT MH.*, M.CCafdelingen, M.OmschrijvingOntvangers, IF(MH.LidID > 0, %4\$s, MH.to_addr) AS AanNaam
						  FROM (%1\$s LEFT JOIN %2\$sLid AS L ON MH.LidID=L.RecordID) LEFT JOIN %2\$sMailing AS M ON MH.MailingID=M.RecordID
						  WHERE %3\$s
						  ORDER BY Ingevoerd DESC LIMIT 1;", $this->basefrom, TABLE_PREFIX, $w, $this->selectnaam);
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
	public function laatstemails($p_filter="", $p_aantal=1) {
		
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
			$query = sprintf("SELECT MH.RecordID, IF(MH.send_on > '2000-01-01', MH.send_on, 'In outbox') AS send_on, %1\$s AS Aan, M.subject
						FROM (%2\$s LEFT OUTER JOIN %3\$sLid AS L ON MH.LidID=L.RecordID) LEFT OUTER JOIN %3\$sMailing AS M ON MH.MailingID=M.RecordID
						WHERE MH.MailingID=%4\$d %5\$s
						ORDER BY MH.send_on DESC, MH.RecordID DESC;", $this->selectmhaan, $this->basefrom, TABLE_PREFIX, $this->mid, $this->zichtbaarwhere);
		} else {
			$query = sprintf("SELECT MH.RecordID, MH.send_on, MV.Vanaf_naam, %1\$s AS Aan, MH.subject
						FROM (%2\$s LEFT OUTER JOIN %3\$sMailing_vanaf AS MV ON MH.VanafID=MV.RecordID) LEFT OUTER JOIN %3\$sLid AS L ON MH.LidID=L.RecordID
						WHERE MH.send_on > '2000-01-01' %4\$s
						ORDER BY MH.send_on DESC, MH.RecordID DESC LIMIT %5\$d;", $this->selectmhaan, $this->basefrom, TABLE_PREFIX, $this->zichtbaarwhere, $this->limit);
		}
		
		$result = $this->execsql($query);
		return $result->fetchAll();
	}  # cls_Mailing_hist->lijst
	
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
			$xw = "AND MH.NietVersturenVoor <= NOW()";
		} else {
			$xw = $this->zichtbaarwhere;
		}
		$query = sprintf("SELECT MH.RecordID, MH.Ingevoerd, MH.subject, IF(MH.LidID > 0, %4\$s, MH.to_addr) AS Aan, MV.Vanaf_naam AS Vanaf,
						MH.to_addr, MH.cc_addr, MH.NietVersturenVoor
						FROM (%1\$sMailing_hist AS MH LEFT OUTER JOIN %1\$sLid AS L ON MH.LidID=L.RecordID) LEFT JOIN %1\$sMailing_vanaf AS MV ON MV.RecordID=MH.VanafID
						WHERE IFNULL(MH.send_on, '1970-01-01') <= '2000-01-01' %2\$s
						ORDER BY MH.NietVersturenVoor, MH.LidID, MH.RecordID LIMIT %3\$d;", TABLE_PREFIX, $xw, $lm, $this->selectnaam);
		return $this->execsql($query);
		
	}  # cls_Mailing_hist->outbox
	
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
			$tl = date("Y-m-d H:i:s", strtotime("-1 hour"));
		} elseif ($p_termijn == 3) {
			$tl = date("Y-m-d H:i:s", strtotime("-24 hour"));
		} else {
			$tl = date("Y-m-d H:i:s", strtotime("-1 minute"));
		}
		
		$query = sprintf("SELECT COUNT(*) FROM %s WHERE IFNULL(MH.send_on, '1970-01-01') > '%s';", $this->basefrom, $tl);
		return $this->scalar($query);
	}  # cls_Mailing_hist->aantalverzonden
	
	public function overzichtlid($p_lidid) {
		
		$query = sprintf("SELECT MH.RecordID,
						  IF(MH.send_on > '2000-01-01', MH.send_on, 'in outbox') AS Verzonden, MV.Vanaf_naam AS Van, MH.subject
						  FROM (%1\$s LEFT OUTER JOIN %2\$sMailing_vanaf AS MV ON MH.VanafID=MV.RecordID) LEFT JOIN %2\$sLid AS L ON MH.LidID=L.RecordID
						  WHERE MH.LidID=%3\$d
						  ORDER BY MH.send_on DESC, MH.RecordID DESC LIMIT 500;", $this->basefrom, TABLE_PREFIX, $p_lidid);
		$result = $this->execsql($query);
		return $result->fetchAll();
		
	}  # cls_Mailing_hist->overzichtlid
	
	public function lidbijemail($p_mhid) {
		$query = sprintf("SELECT IFNULL(MAX(LidID), 0) FROM %s AS MH WHERE MH.RecordID=%d;", $this->table, $p_mhid);
		return $this->scalar($query);
	}
	
	public function add($p_email) {
		global $dbc;
		
		$this->vulvars(-1, $p_email->mailingid);
		$this->tas = 21;
		$this->lidid = $p_email->lidid;
		$nrid = $this->nieuwrecordid();
		
		$data['nrid'] = $nrid;
		$data['lidid'] = $p_email->lidid;
		$data['mid'] = $p_email->mailingid;
		$data['vanafid'] = $p_email->vanafid;
		$data['subject'] = html_entity_decode(str_replace("\"", "'", $p_email->onderwerp));
		$data['to_name'] = $p_email->omsontvangers;
		if (strlen($p_email->aannaam) > 0) {
			$data['to_name'] = html_entity_decode(str_replace("\"", "'", $p_email->aannaam));
		}
		$data['to_addr'] = $p_email->aanadres;
		$data['cc_addr'] = $p_email->cc;
		$data['message'] = html_entity_decode(str_replace("\"", "'", $p_email->bericht));
		$data['replyid'] = $p_email->replyid;
		
		$data['xtra_char'] = $p_email->xtrachar;
		$data['zichtbaarvoor'] = $p_email->zichtbaarvoor;
		$data['zonderbriefpapier'] = $p_email->zonderbriefpapier;
		$lk = $this->lengtekolom("Xtra_Char");
		if (strlen($p_email->xtrachar) > $lk) {
			$data['xtra_char'] = substr($p_email->xtrachar, 0, $lk);
		}
		$data['xtra_num'] = $p_email->xtranum;
		$data['nietversturenvoor'] = $p_email->nietversturenvoor;
		$data['ingevoerddoor'] = $_SESSION['lidid'];
	
		$query = sprintf("INSERT INTO %s SET RecordID=:nrid, LidID=:lidid, MailingID=:mid, VanafID=:vanafid, subject=:subject, to_name=:to_name, to_addr=:to_addr, cc_addr=:cc_addr, message=:message, ZonderBriefpapier=:zonderbriefpapier, ZichtbaarVoor=:zichtbaarvoor, ReplyID=:replyid, Xtra_Char=:xtra_char, Xtra_Num=:xtra_num, NietVersturenVoor=:nietversturenvoor, IngevoerdDoor=:ingevoerddoor;", $this->table);
		if ($dbc->prepare($query)->execute($data) > 0) {
			$t = str_ireplace(TABLE_PREFIX, "", $this->table);
			if ($this->mid > 0) {
				$mess = sprintf("Record %d aan '%s' voor mailing %d (%s) toegevoegd.", $nrid, $t, $this->mid, $this->naamlogging);
				(new cls_logboek())->add($mess, 4, $p_email->lidid, 0, $nrid, 21, $t);
			}
			return $nrid;
		} else {
			return 0;
		}
		
	}  # cls_Mailing_hist->add
	
	public function update($p_mhid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_mhid);
		$this->tas = 22;
		
		if ($this->pdoupdate($this->mhid, $p_kolom, $p_waarde, $p_reden) > 0) {
			if ($p_kolom != "send_on") {
				$this->log($this->mhid);
			}
		}
	}
	
	public function delete($p_mhid, $p_reden="") {
		$this->vulvars($p_mhid);
		$this->tas = 23;

		if ($this->pdodelete($this->mhid, $p_reden)) {
			$this->log($this->mhid);
		}
	}
	
	public function outboxlegen() {
		$f = "(MH.send_on IS NULL)";
		foreach ($this->basislijst($f) as $mhrow) {
			$this->delete($mhrow->RecordID);
		}
	}
	
	public function controle() {
		$i_mv = new cls_Mailing_vanaf();
		
		$query = sprintf("SELECT MH.RecordID FROM %s WHERE MH.MailingID > 0 AND MH.MailingID NOT IN (SELECT M.RecordID FROM %sMailing AS M);", $this->basefrom, TABLE_PREFIX);
		$res = $this->execsql($query);
		foreach ($res->fetchAll() as $mhrow) {
			$this->update($mhrow->RecordID, "MailingID", 0, "de mailing niet (meer) bestaat");
		}
		
		$f = "IFNULL(MH.VanafID, 0)=0";
		foreach ($this->basislijst($f, "", 1, 2500) as $mhrow) {
			$mvid = $i_mv->min("RecordID");
			$this->update($mhrow->RecordID, "VanafID", $mvid, "", 1);
		}
		
		$i_mv = null;
		
	}  # controle
	
	public function opschonen() {
		$this->tas = 23;
		$i_lm = new cls_Lidmaatschap();
		
		$mho = $_SESSION['settings']['mailing_hist_opschonen'] ?? 84;
		if ($mho > 3) {
			$query = sprintf("SELECT LM.Lid FROM %sLidmaatschap AS LM WHERE IFNULL(LM.Opgezegd, '9999-12-31') < DATE_SUB(CURDATE(), INTERVAL %d MONTH);", TABLE_PREFIX, $mho);
			$result = $this->execsql($query);
			foreach ($result->fetchAll() as $lmrow) {
				$this->lidid = $lmrow->Lid;
				if ((new cls_Lidmaatschap())->eindelidmaatschap($lmrow->Lid) < date("Y-m-d", strtotime(sprintf("-%d month", $mho)))) {
					$this->query = sprintf("DELETE FROM %s WHERE LidID=%d AND send_on < DATE_SUB(CURDATE(), INTERVAL %d MONTH);", $this->table, $lmrow->Lid, $mho);
					$aant = $this->execsql();
					if ($aant > 0) {
						$this->mess = sprintf("%s: %d verzonden e-mails van voormalig lid %d verwijderd.", $this->table, $aant, $this->lidid);
						$this->log(0, 1);
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
		if ($mho > 3) {
			$aant = 0;
			$query = sprintf("SELECT MH.RecordID FROM %s WHERE MH.send_on < DATE_SUB(CURDATE(), INTERVAL %d MONTH) AND (MH.send_on IS NOT NULL);", $this->basefrom, $mho);
			$result = $this->execsql($query);
			$reden = sprintf("deze langer dan %d maanden geleden verzonden is.", $mho);
			foreach ($result->fetchAll() as $mhrow) {
				$this->delete($mhrow->RecordID, $reden);
			}
		}
		
		$query = sprintf("SELECT MH.RecordID FROM %s WHERE MH.Ingevoerd < DATE_SUB(CURDATE(), INTERVAL 15 DAY) AND (MH.send_on IS NULL);", $this->basefrom);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $mhrow) {
			$this->delete($mhrow->RecordID, "deze na 2 weken nog niet verzonden is");
		}
		
		$query = sprintf("DELETE FROM %s WHERE Xtra_char IN ('HERWW', 'ISF', 'LNR', 'NOTIF', 'VALLO') AND IFNULL(send_on, '9999-12-31') < DATE_SUB(CURDATE(), INTERVAL 1 MONTH);", $this->table);
		$this->execsql($query, 4);
		
		$query = sprintf("UPDATE %s SET message='' WHERE MailingID > 0 AND IFNULL(message, '') > '' AND IFNULL(send_on, '9999-12-31') < DATE_SUB(CURDATE(), INTERVAL 6 MONTH);", $this->table);
		$this->execsql($query, 4);
		
		$this->optimize();
		
	}  # opschonen
	
}  # cls_Mailing_hist

class cls_Mailing_rcpt extends cls_db_base {
	
	private int $mid = 0;
	private int $mrid = 0;
	private string $email = "";
	
	function __construct($p_mid=-1) {
		$this->table = TABLE_PREFIX . "Mailing_rcpt";
		$this->basefrom = $this->table . " AS MR";
		if ($p_mid >= 0) {
			$this->mid = $p_mid;
		}
		$this->ta = 4;
		$this->tas = 10;
	}
	
	private function vulvars($p_mrid=-1, $p_mid=-1) {
		if ($p_mrid >= 0) {
			$this->mrid = $p_mrid;
		}
		if ($p_mid >= 0) {
			$this->mid = $p_mid;
		}

		$this->email = "";
		if ($this->mrid > 0) {
			$query = sprintf("SELECT MR.* FROM %s WHERE MR.RecordID=%d;", $this->basefrom, $this->mrid);
			$result = $this->execsql($query);
			$row = $result->fetch();
			if (isset($row->RecordID)) {
				$this->lidid = $row->LidID;
				$this->mid = $row->MailingID;
				$this->email = $row->to_address ?? "";
			} else {
				$this->mrid = 0;
				$this->lidid = 0;
			}
		}
		
		if ($this->mid > 0) {
			$query = sprintf("SELECT M.subject FROM %sMailing AS M WHERE M.RecordID=%d;", TABLE_PREFIX, $this->mid);
			$result = $this->execsql($query);
			$row = $result->fetch();
			if (isset($row->subject)) {
				$this->naamlogging = $row->subject ?? "";
			} else {
				$this->mid = 0;
			}
		}
	}  # cls_Mailing_rcpt->vulvars
	
	public function lijst($p_mid=-1, $p_lidid=-1) {
		$this->mid = $p_mid;
		
		$w = sprintf("MR.MailingID=%d", $this->mid);
		if ($p_lidid > 0) {
			$w .= sprintf(" AND L.RecordID=%d", $p_lidid);
		}
		
		$query = sprintf("SELECT MR.LidID, MR.RecordID, %s AS NaamLid, %s AS Zoeknaam_lid, L.Adres, L.Postcode, L.Woonplaats, L.Email, L.EmailVereniging, L.EmailOuders, L.GEBDATUM, MR.to_address, MR.MailingID
						  FROM %s LEFT OUTER JOIN %sLid AS L ON MR.LidID=L.RecordID
						  WHERE %s
						  ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, MR.to_address, MR.Ingevoerd;", $this->selectnaam, $this->selectzoeknaam, $this->basefrom, TABLE_PREFIX, $w);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}  # cls_Mailing_rcpt->lijst
	
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
	}  # cls_Mailing_rcpt->aantalOntvangers
	
	public function add($p_mid, $p_lidid, $p_email="", $p_geenlog=0, $p_xchar="", $p_xnum=0, $p_ondid=0) {
		$this->vulvars(-1, $p_mid);
		$this->lidid = $p_lidid;
		$this->tas = 11;
		$nrid = 0;
		
		$p_email = str_replace(" ", "", $p_email);
		$p_email = str_replace(";", ",", $p_email);
		if ($this->lidid == 0 and strlen($p_email) > 5 and isValidMailAddress($p_email)) {
			$mr = (new cls_Lid())->lidbijemail($p_email);
			if (count($mr) == 1) {
				$this->lidid = $mr[0]->LidID;
				$p_email = "";
			}
		}
		
		if ($this->lidid > 0 and $this->mid > 0) {
			$query = sprintf("SELECT COUNT(*) FROM %s AS MR WHERE MR.MailingID=%d AND MR.LidID=%d;", $this->table, $this->mid, $this->lidid);
		} elseif (isValidMailAddress($p_email, 0) and $this->mid > 0) {
			$query = sprintf("SELECT COUNT(*) FROM %s AS MR WHERE MR.MailingID=%d AND LOWER(MR.to_address) LIKE '%s';", $this->table, $this->mid, strtolower($p_email));
		} else {
			$query = "";
		}
		if (strlen($query) > 0) {
			if ($this->scalar($query) > 0) {
//				$this->mess = sprintf("%s is niet aan mailing %d toegevoegd, omdat hij/zij al ontvanger van deze mailing is.", $r, $p_mid);
			} else {
				$query = "";
				$nrid = $this->nieuwrecordid();
				if ($this->lidid > 0) {
					$query = sprintf("INSERT INTO %s (RecordID, MailingID, LidID, Xtra_Char, Xtra_Num) VALUES (%d, %d, %d, '%s', %d);", $this->table, $nrid, $p_mid, $this->lidid, $p_xchar, $p_xnum);
				} elseif (isValidMailAddress($p_email)) {
					$query = sprintf("INSERT INTO %s (RecordID, MailingID, to_address) VALUES (%d, %d, '%s');", $this->table, $nrid, $this->mid, $p_email);
				} else {
					$this->mess = sprintf("%s is niet aan mailing %d toegevoegd, omdat dit geen correct emailadres is.", $p_email, $this->mid);
					$nrid = 0;
				}
				if (strlen($query) > 5 and $this->execsql($query) > 0 and $p_geenlog == 0) {
					$this->mess = sprintf("Mailing_rcpt: Record %d is aan mailing %d (%s) toegevoegd.", $nrid, $this->mid, $this->naamlogging);
					$this->tm = 0;
				}
			}
		} else {
			$nrid = 0;
		}
		
		$this->log($nrid, 0, $p_ondid);
		return $nrid;
	}  # cls_Mailing_rcpt->add
	
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
	}  # cls_Mailing_rcpt->delete
	
	public function delete_all($p_mid) {
		
		$query = sprintf("DELETE FROM %s WHERE MailingID=%d;", $this->table, $p_mid);
		$rv = $this->execsql($query);
		if ($rv > 0) {
			$this->mess = sprintf("Alle %d ontvangers zijn bij mailing %d verwijderd.", $rv, $p_mid);
			$this->log($p_mid);
		}
		
		return $rv;
	}  # cls_Mailing_rcpt->delete_all
	
	public function controle() {
	}  # cls_Mailing_rcpt->controle
	
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
		
		$this->optimize();
		
	}  # cls_Mailing_rcpt->opschonen
	
}  # cls_Mailing_rcpt

class cls_Mailing_vanaf extends cls_db_base {
	public $mvid = 0;
	public $vanaf_email = "";
	public $vanaf_naam = "";
	
	function __construct($p_mvid=-1, $p_email="") {
		$this->table = TABLE_PREFIX . "Mailing_vanaf";
		$this->basefrom = $this->table . " AS MV";
		$this->ta = 4;
		$this->tas = 30;
		$this->per = date("Y-m-d");
		$this->vulvars($p_mvid, $p_email);
	}
	
	public function vulvars($p_mvid=-1, $p_email="") {
		if ($p_mvid >= 0) {
			$this->mvid = $p_mvid;
		}
		
		if ($this->mvid <= 0 and strlen($p_email) > 5) {
			$query = sprintf("SELECT IFNULL(MV.RecordID, 0) FROM %s WHERE UPPER(Vanaf_email)='%s';", $this->basefrom, strtoupper($p_email));
			$this->mvid = $this->scalar($query);
		}
		
		if ($this->mvid > 0) {
			$query = sprintf("SELECT MV.* FROM %s WHERE MV.RecordID=%d;", $this->basefrom, $this->mvid);
			$result = $this->execsql($query);
			$row = $result->fetch();
			if (isset($row->RecordID)) {
				$this->vanaf_email = htmlentities($row->Vanaf_email);
				$this->vanaf_naam = $row->Vanaf_naam;
			} else {
				$this->mvid = 0;
			}
		} else {
			$this->vanaf_email = "";
			$this->vanaf_naam = "";
		}
		$this->naamlogging = $this->vanaf_email ?? "";
	}  # cls_Mailing_vanaf->vulvars
	
	public function default_vanaf() {
		$f = "LENGTH(Vanaf_email) > 5";
		$this->mvid = $this->min("RecordID", $f);
		$this->vulvars();
		return $this->vanaf_email;
	}  # cls_Mailing_vanaf->default_vanaf
	
	public function lijst($p_fetched=1) {
		$query = sprintf("SELECT MV.RecordID, MV.Vanaf_email, MV.Vanaf_naam, MV.Ingevoerd, MV.Gewijzigd FROM %s ORDER BY MV.Vanaf_email;", $this->basefrom);
		$result = $this->execsql($query);
		if ($p_fetched == 1) {
			$result = $result->fetchAll();
		}

		return $result;
	}  # cls_Mailing_vanaf->lijst
	
	public function htmloptions($p_cv="") {
		$rv = "";
		
		foreach ($this->lijst() as $row) {
			$rv .= sprintf("<option value=%d %s>%s (%s)</option>\n", $row->RecordID, checked($row->RecordID, "option", $p_cv), $row->Vanaf_naam, $row->Vanaf_email);
		}
		return $rv;
	}
	
	public function add() {
		$this->tas = 31;
		
		$query = sprintf("INSERT INTO %s (Vanaf_naam, Vanaf_email, Ingevoerd) VALUES ('** nieuw **', '', CURDATE());", $this->table);
		$result = $this->execsql($query);
		if ($result > 0) {
			$this->mess = sprintf("Mailing_vanaf: record %d is toegevoegd.", $result);
			$this->log($result, 1);
		}
		return $result;
	}
	
	public function update($p_mvid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_mvid);
		$this->tas = 32;
		
		if ($this->pdoupdate($this->mvid, $p_kolom, $p_waarde, $p_reden) > 0) {
			$this->log($this->mvid);
		}
	}
	
	public function delete($p_mvid, $p_reden="") {
		$this->vulvars($p_mvid);
		$this->tas = 33;
		
		$query = sprintf("SELECT COUNT(*) FROM %sMailing AS M WHERE M.MailingVanafID=%d;", TABLE_PREFIX, $this->mvid);
		if ($this->scalar($query) == 0) {
			$query = sprintf("SELECT COUNT(*) FROM %sMailing_hist AS MH WHERE MH.VanafID=%d;", TABLE_PREFIX, $this->mvid);
			if ($this->scalar($query) == 0) {
				$this->pdodelete($p_mvid, $p_reden);
			} else {
				$this->mess = sprintf("Record %d (%s) is nog een of meerdere verzonden e-mails gekoppeld en mag niet verwijderd worden.", $this->mvid, $this->naamlogging);
			}
		} else {
			$this->mess = sprintf("Record %d (%s) is nog een of meerdere mailings gekoppeld en mag niet verwijderd worden.", $this->mvid, $this->naamlogging);
		}
		$this->log($p_mvid, 1);
	}  # delete
	
	public function opschonen() {
		
		$this->optimize();
		
	}  # opschonen
	
}  # cls_Mailing_vanaf

class cls_Logboek extends cls_db_base {
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Admin_activiteit";
		$this->basefrom = $this->table . " AS A";
		$this->per = date("Y-m-d");
	}
	
	private function script() {
		$script = $_SERVER['PHP_SELF'];
		if (isset($_SERVER['QUERY_STRING']) and strlen($_SERVER['QUERY_STRING']) > 0) {
			$script .= "?" . str_replace("%20", " ", $_SERVER['QUERY_STRING']);
		}
		$ml = $this->lengtekolom("Script");
		if (strlen($script) > $ml) {
			$stript = substr($script, 0, $ml-4) . " ...";
		}
		
		return $script;
	}  # cls_Logboek->script
	
	public function lijst($p_type, $p_smal=0, $p_lidid=0, $p_filter="", $p_sort="", $p_limiet=9999, $p_fetch=1) {
		
		if ($p_type >= 0 and strlen($p_filter) > 5) {
			$w = sprintf("TypeActiviteit=%d AND %s", $p_type, $p_filter);
		} elseif ($p_lidid > 0 and strlen($p_filter) > 5) {
			$w = sprintf("LidID=%d AND %s", $p_lidid, $p_filter);
		} elseif ($p_type >= 0) {
			$w = sprintf("TypeActiviteit=%d", $p_type);
		} elseif (strlen($p_filter) > 5) {
			$w = $p_filter;
		} elseif (strlen($this->where) > 0) {
			$w = $this->where;
		} else {
			$w = "TypeActiviteit >= 0";
		}
		
		if (strlen($this->where) > 0) {
			$w .= " AND " . $this->where;
		}
		
		$s = "A.DatumTijd, Omschrijving";
		$s .= ", IF(A.ReferLidID > 0, A.ReferLidID, '') AS betreftLid";
		$s .= ", IF(IFNULL(A.ReferOnderdeelID, 0) > 0, A.ReferOnderdeelID, '') AS betreftOnderdeel";
		$s .= ", CONCAT(TypeActiviteit, IF(TypeActiviteitSpecifiek > 0, CONCAT('-', TypeActiviteitSpecifiek), '')) AS `Type`";
		$s .= ", IF(A.LidID > 0, A.LidID, '') AS ingelogdLid";
		$s .= ", IF(A.Getoond=1, 'Ja', '') AS Getoond";
		
		if (WEBMASTER and $p_smal == 0) {
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

		$query = sprintf("SELECT %s FROM %s WHERE %s ORDER BY %sA.RecordID DESC %s;", $s, $this->basefrom, $w, $p_sort, $lm);
		$result = $this->execsql($query);
		if ($p_fetch == 1) {
			return $result->fetchAll();
		} else {
			return $result;
		}
	}  # cls_Logboek->lijst
	
	public function overzichtlid($p_lidid) {
		
		$query = sprintf("SELECT A.DatumTijd, Omschrijving, %s AS ingelogdLid, IF(IFNULL(A.ReferOnderdeelID, 0) > 0, A.ReferOnderdeelID, '') AS betreftOnderdeel
								FROM %s LEFT OUTER JOIN %sLid AS L ON A.LidID=L.RecordID
								WHERE TypeActiviteit IN (1, 5, 6, 7, 12, 14, 15, 16) AND ReferLidID=%d
								ORDER BY A.RecordID DESC LIMIT 1500;", $this->selectnaam, $this->basefrom, TABLE_PREFIX, $p_lidid);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function lidlijst() {
		$this->query = sprintf("SELECT DISTINCT LidID, %s AS Naam
							FROM %s INNER JOIN %sLid AS L ON A.LidID=L.RecordID
							ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam;", $this->selectzoeknaam, $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql();
		return $result->fetchAll();
	}
			
	public function vorigelogin($p_opmaak=1) {
		global $dtfmt;
		
		if ($_SESSION['lidid'] > 0) {
			$this->lidid = $_SESSION['lidid'];
		}
		$query = sprintf("SELECT IFNULL(MAX(DatumTijd), 'Geen') FROM %s WHERE LidID=%d AND TypeActiviteit=1 AND DatumTijd < DATE_SUB(SYSDATE(), INTERVAL 15 MINUTE) AND Omschrijving LIKE '%%ingelogd%%';", $this->basefrom, $this->lidid);
		
		if ($p_opmaak == 1) {
			$dtfmt->setPattern(DTLONG);
			return $dtfmt->format(strtotime($this->scalar($query)));
		} else {
			return $this->scalar($query);
		}
	}  # cls_Logboek->vorigelogin
	
	public function iplogincontrole() {
		
		if ($_SESSION['settings']['login_autounlock'] > 0) {
			$min = $_SESSION['settings']['login_autounlock'];
		} else {
			$min = 120;
		}
		$query = sprintf("SELECT COUNT(*) FROM %s WHERE TypeActiviteit=1 AND TypeActiviteitSpecifiek=4 AND IP_adres='%s' AND DatumTijd >= DATE_SUB(SYSDATE(), INTERVAL %d MINUTE);", $this->basefrom, $_SERVER['HTTP_USER_AGENT'], $min);
		return $this->scalar($query);
	}  # cls_Logboek->iplogincontrole
	
	public function laatstgewijzigd() {
		
	}  # cls_Logboek->laatstgewijzigd
	
	public function add($p_oms, $p_ta=0, $p_lidid=-1, $p_tm=-1, $p_referid=0, $p_tas=-1, $p_reftable="", $p_refcolumn="", $p_autom=0, $p_refondid=0) {
		global $dbc;
		
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		if ($p_tm >= 0) {
			$this->tm = $p_tm;
		}
		if ($p_tas >= 0) {
			$this->tas = $p_tas;
		}
		
		$p_reftable = str_replace(TABLE_PREFIX, "", $p_reftable);
				
		if ($p_reftable == "Onderdl") {
			$refondid = $p_referid;

		} elseif ($p_refondid > 0) {
			$refondid = $p_refondid;
			
		} elseif ($p_reftable == "Lidond" and $p_referid > 0) {
			$f = sprintf("LO.RecordID=%d", $p_referid);
			$refondid = (new cls_Lidond())->max("OnderdeelID", $f);
			
		} elseif ($p_reftable == "Groep" and $p_referid > 0) {
			$f = sprintf("GR.RecordID=%d", $p_referid);
			$refondid = (new cls_Groep())->max("OnderdeelID", $f);
			
		} elseif ($p_reftable == "Diploma") {
			$f = sprintf("DP.RecordID=%d", $p_referid);
			$refondid = (new cls_Diploma())->max("Afdelingsspecifiek", $f);
			
		} elseif ($p_reftable == "Liddipl") {
			$f = sprintf("LD.RecordID=%d", $p_referid);
			$dpid = (new cls_Liddipl())->max("DiplomaID", $f);
			
			$f = sprintf("DP.RecordID=%d", $dpid);
			$refondid = (new cls_Diploma())->max("Afdelingsspecifiek", $f);
			
		} elseif ($p_reftable == "Mailing" and $p_refcolumn == "ZichtbaarVoor") {
			$f = sprintf("M.RecordID=%d", $p_referid);
			$refondid = (new cls_Mailing())->max("ZichtbaarVoor", $f);
			
		} elseif ($p_reftable == "Afdelingskalender" and $p_referid > 0) {
			$f = sprintf("AK.RecordID=%d", $p_referid);
			$refondid = (new cls_Afdelingskalender())->max("OnderdeelID", $f);
			
		} elseif ($p_reftable == "Aanwezigheid" and $p_referid > 0) {
			$f = sprintf("AW.RecordID=%d", $p_referid);
			$lo = (new cls_Aanwezigheid())->max("LidondID", $f);
			$f = sprintf("LO.RecordID=%d", $lo);
			$refondid = (new cls_Lidond())->max("OnderdeelID", $f);
			
		} elseif ($p_reftable == "Rekreg" and $p_referid > 0) {
			$f = sprintf("RR.RecordID=%d", $p_referid);
			$lo = (new cls_Rekeningregel())->max("LidondID", $f);
			
			$f = sprintf("LO.RecordID=%d", $lo);
			$refondid = (new cls_Lidond())->max("OnderdeelID", $f);
			
		} elseif ($p_reftable == "Admin_access" and $p_referid > 0) {
			$f = sprintf("AA.RecordID=%d", $p_referid);
			$refondid = (new cls_Authorisation())->max("Toegang", $f);
			
		} elseif ($p_reftable == "Inschrijving") {
			$f = sprintf("Ins.RecordID=%d", $p_referid);
			$refondid = (new cls_Inschrijving())->max("OnderdeelID", $f);
			
		} elseif ($p_reftable == "Examen") {
			$f = sprintf("EX.Nummer=%d", $p_referid);
			$refondid = (new cls_Examen())->max("OnderdeelID", $f);
			
		} elseif ($p_reftable == "Examenonderdeel") {
			$f = sprintf("EO.RecordID=%d", $p_referid);
			$dp = (new cls_Examenonderdeel())->max("DiplomaID", $f);
			
			$f = sprintf("DP.RecordID=%d", $dp);
			$refondid = (new cls_Diploma())->max("Afdelingsspecifiek", $f);
			
		} else {
			$refondid = $p_refondid;
		}

		/*
		$this->tm:
			* 0: niet tonen
			
			* 1: aan iedereen tonen (alert-info)
			* 2: alleen tonen aan webmasters (alert-info)
			* 3: aan iedereen via alert tonen
			
			* 11: aan iedereen tonen (warning-info)
			* 12: alleen tonen aan webmasters (warning-info)
		*/
		
		if (isset($_SERVER['REMOTE_ADDR'])) {
			$data['ipaddress'] = $_SERVER['REMOTE_ADDR'];
		} else {
			$data['ipaddress'] = "";
		}

		$bt = debug_backtrace(0, 4);
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
					if (isset($bt[$t]['args']) and count($bt[$t]['args']) > 0) {
						$a = "";
						for ($at=0; $at < count($bt[$t]['args']);$at++) {
							if ($at > 0) {
								$a .= ", ";
							}
							if (is_object($bt[$t]['args'][$at])) {
								$a .= "object";
							} elseif (is_numeric($bt[$t]['args'][$at])) {
								$a .= substr($bt[$t]['args'][$at], 0, 30);
							} elseif (strlen($bt[$t]['args'][$at]) > 30) {
								$a .= "'". substr($bt[$t]['args'][$at], 0, 26) . " ...'";
							} else {
								$a .= "'". substr($bt[$t]['args'][$at], 0, 30) . "'";
							}
						}
						$f .= " (" . str_replace("\"", "'", $a) . ")";
					}
				}
			}
		}
		$f = trim($f);
		if (strlen($f) > 125) {
			$f = substr($f, 0, 121) . " ...";
		}
		$data['reffunction'] = $f;
		
		$p_oms = str_replace("<p>", "", $p_oms);
		$p_oms = str_replace("</p>", "\n", $p_oms);
		$p_oms = str_replace("\"", "'", trim($p_oms));
		if (strlen($p_oms) > 0 and substr($p_oms, -1) != ".") {
			$p_oms .= ".";
		}
		
		if (strlen($p_oms) > 64000) {
			$p_oms = substr($p_oms, 0, 64000);
		} elseif (strlen($p_oms) == 0) {
			$this->tm = 0;
		}
		$data['omschrijving'] = $p_oms;
		$data['script'] = $this->script();
		$data['referid'] = $p_referid;
		if ($this->lidid == null) {
			$data['referlidid'] = 0;
		} else {
			$data['referlidid'] = $this->lidid;
		}
		$data['ta'] = $p_ta;
		$data['tas'] = $this->tas;
		$data['referonderdeelid'] = $refondid;

		if (!isset($_SERVER['HTTP_USER_AGENT'])) {
			$data['useragent'] = "";
		} elseif (strlen($_SERVER['HTTP_USER_AGENT']) > 125) {
			$data['useragent'] = substr($_SERVER['HTTP_USER_AGENT'], 0, 125);
		} else {
			$data['useragent'] = $_SERVER['HTTP_USER_AGENT'];
		}
		
		if ($this->tm == 2 and WEBMASTER == false) {
			$this->tm = 0;
		}
		$data['getoond'] = $this->tm;
		
		if ($p_autom == 0) {
			$data['ingelogdlid'] = $_SESSION['lidid'];
		} else {
			$data['ingelogdlid'] = 0;
		}
		$data['reftable'] = $p_reftable;
		$data['refcolumn'] = $p_refcolumn;

		$query = sprintf("INSERT INTO %s (LidID, IP_adres, USER_AGENT, Omschrijving, ReferID, ReferLidID, ReferOnderdeelID, TypeActiviteit, Script, Getoond, RefFunction, TypeActiviteitSpecifiek, RefTable, refColumn) VALUES 
				(:ingelogdlid, :ipaddress, :useragent, :omschrijving, :referid, :referlidid, :referonderdeelid, :ta, :script, :getoond, :reffunction, :tas, :reftable, :refcolumn);", $this->table);
		$nrid = $dbc->prepare($query)->execute($data);
		
		if ($this->tm == 1 or ($this->tm == 2 and WEBMASTER)) {
			printf("<p class='mededeling alert-info'>%s</p>\n", $p_oms);
		} elseif ($this->tm == 11 or ($this->tm == 12 and WEBMASTER)) {
			printf("<p class='mededeling warning-info'>%s</p>\n", $p_oms);
		} elseif ($this->tm == 3) {
			printf("<script>alert(\"%s\");</script>\n", $p_oms);
		}
		usleep(15);
		return $nrid;
	}  # cls_Logboek->add
	
	private function update($p_aid, $p_kolom, $p_waarde) {
		$this->pdoupdate($p_aid, $p_kolom, $p_waarde);
		// Bewust zonder logging
	}  # cls_Logboek->update
	
	private function delete($p_aid) {
		$this->pdodelete($p_aid);
		// Bewust zonder logging
	}  # cls_Logboek->delete
	
	public function controle() {
		
		$f = "A.TypeActiviteit=19";
		foreach ($this->basislijst($f) as $row) {
			if ($row->TypeActiviteit == 19) {  // Deze code kan weg na 1 april 2025
				$this->update($row->RecordID, "refColumn", "GroepID");
				$this->update($row->RecordID, "TypeActiviteit", 6);
			}
		}
		
	}  # cls_Logboek->controle
	
	public function opschonen() {
		
// Logging opschonen op basis van de instelling, geldt voor alle logging
		if ($_SESSION['settings']['logboek_bewaartijd'] > 0) {
			$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL %d MONTH);", $this->table, $_SESSION['settings']['logboek_bewaartijd']);
			$this->execsql($query, 2);
		}
		
// Logging, waar een lid aan is gekoppeld.
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL %d MONTH) AND ReferLidID > 0;", $this->table, $_SESSION['settings']['logboek_lid_bewaartijd']);
		$this->execsql($query, 2);

// Logging zonder tekst
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 7 DAY) AND LENGTH(Omschrijving)=0;", $this->table);
		$this->execsql($query, 2);

// Performance
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 7 DAY) AND TypeActiviteit=98;", $this->table);
		$this->execsql($query, 2);
		
// Automatisch bijwerken lidond en optimize tabellen
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 7 DAY) AND TypeActiviteit=2 AND TypeActiviteitSpecifiek >= 21;", $this->table);
		$this->execsql($query, 2);

// Backups
		$query = sprintf("DELETE FROM %s WHERE (DatumTijd < DATE_SUB(CURDATE(), INTERVAL 14 DAY) AND TypeActiviteit=3 AND TypeActiviteitSpecifiek > 1) OR (DatumTijd < DATE_SUB(CURDATE(), INTERVAL 3 MONTH) AND TypeActiviteit=3);", $this->table);
		$this->execsql($query, 2);
		
// Inloggen
		$f = "A.TypeActiviteit=1 AND A.TypeActiviteitSpecifiek=1";
		$aant = 0;
		foreach ($this->basislijst($f) as $row) {
			$query = sprintf("SELECT COUNT(*) FROM %s WHERE A.LidID=%d AND A.RecordID >= %d AND %s;", $this->basefrom, $row->LidID, $row->RecordID, $f);
			if ($this->scalar($query) > 10) {
				$this->delete($row->RecordID);
				$aant++;
			}
		}
		if ($aant > 0) {
			$this->mess = sprintf("Er zijn %d rijen uit de logging verwijderen van inloggen, omdat er meer dan 10 in de tabel, van het betreffende lid, stonden.", $aant);
			$this->ta = 2;
			$this->log(0);
		}

// Uitloggen
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 7 DAY) AND TypeActiviteit=1 AND TypeActiviteitSpecifiek=2;", $this->table);
		$this->execsql($query, 2);
		
// Mislukte logins
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 6 MONTH) AND TypeActiviteit=1 AND TypeActiviteitSpecifiek=4;", $this->table);
		$this->execsql($query, 2);
	
// Eigen debugging
		$query = sprintf("DELETE FROM %s WHERE TypeActiviteit=99 AND LidID=%d;", $this->table, $_SESSION['lidid']);
		$this->execsql($query, 2);

// Debugging
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 4 WEEK) AND TypeActiviteit IN (0, 99);", $this->table);
		$this->execsql($query, 2);
		
// Autorisatie
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 2 WEEK) AND TypeActiviteit=15;", $this->table);
		$this->execsql($query, 2);
		
// Ontvangers mailings
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 3 MONTH) AND TypeActiviteit=4 AND TypeActiviteitSpecifiek in (10, 11, 13);", $this->table);
		$this->execsql($query, 2);
		
// Outbox versturen
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 2 WEEK) AND TypeActiviteit=4 AND TypeActiviteitSpecifiek=50;", $this->table);
		$this->execsql($query, 2);
		
// Onderhoud/Jobs/Uploads
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 2 WEEK) AND TypeActiviteit IN (2, 9);", $this->table);
		$this->execsql($query, 2);
		
// Controle eigen lijsten
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 2 WEEK) AND TypeActiviteit=23 AND TypeActiviteitSpecifiek>1;", $this->table);
		$this->execsql($query, 2);
		
// Opschonen presentie
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 7 MONTH) AND TypeActiviteit=24;", $this->table);
		$this->execsql($query, 2);
		
// Toevoegen / Wijzigen / verwijderen parameter
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 2 YEAR) AND TypeActiviteit=13;", $this->table);
		$this->execsql($query, 2);
		
// Melding nieuwe versie
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 7 DAY) AND TypeActiviteit=13 AND TypeActiviteitSpecifiek=9;", $this->table);
		$this->execsql($query, 2);
		
// Stamgegevens en stukken
		$query = sprintf("DELETE FROM %s WHERE DatumTijd < DATE_SUB(CURDATE(), INTERVAL 3 MONTH) AND TypeActiviteit IN (20, 22);", $this->table);
		$this->execsql($query, 2);
		
		$this->optimize();
		
	}  #  cls_Logboek->opschonen

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
	
	public function add($p_query) {
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
		
			$query = sprintf("INSERT INTO %s (`IP-adres`, LidID, `SQL-statement`, IngelogdLid) VALUES (?, ?, ?, ?)", $this->table);
			$stmt = $dbc->prepare($query);
			$stmt->bindParam(1, $_SERVER['REMOTE_ADDR']);
			$stmt->bindParam(2, $this->lidid);
			$stmt->bindParam(3, $msa_sql);
			$stmt->bindParam(4, $_SESSION['lidid']);
			$stmt->execute();
			$stmt = null;
		}
	}
	
	public function delete($p_recid, $p_reden="") {
		$this->tas = 3;
		
		if ($this->pdodelete($p_recid, $p_reden)) {
			$this->log($p_recid);
		}
	}
	
	public function afmelden() {
		$this->tas = 2;
		
		$query = sprintf("UPDATE %s SET Afgemeld=SYSDATE() WHERE IFNULL(Afgemeld, '1900-01-01') < '2011-01-01' ORDER BY Ingevoerd, RecordID LIMIT %d;", $this->table, $this->maxrecords);
		$result = $this->execsql($query);
		if ($result > 0) {
			$this->mess = sprintf("Er zijn %d wijzigingen uit de interface afgemeld.", $result);
			$this->tm = 1;
			$this->log();
		}
		
		$this->opschonen();
	}
	
	public function opschonen() {
		$this->tas = 3;
		
		$query = sprintf("DELETE FROM %s WHERE Afgemeld < DATE_SUB(CURDATE(), INTERVAL 6 MONTH);", $this->table);
		$res = $this->execsql($query);
		if ($res > 0) {
			$this->mess = sprintf("%d records uit tabel '%s' verwijderd.", $res, $this->table);
			$this->Log();
		}
		
		$this->optimize();
		
	}  # opschonen
	
}  # cls_interface

class cls_Diploma extends cls_db_base {
	public int $dpid = 0;
	public string $naam = "";
	public string $code = "";
	public int $volgnr = 0;
	public string $dptype = "";
	public $organisatie = 0;					// Door welke organisatie wordt dit diploma uitgegeven?
	public $voorgangerid = 0;					// Wat is de logische voorganger van dit diploma?
	public $doorlooptijd = 0;					// in maanden, hoelang doet een leerling normaal over dit diploma?
	public $dpvolgende = 0;
	public $naamvolgende = "";					// Naam van het opvolgende diploma
	public $eindeuitgifte = "";
	public $geldigheid = 0;						// Hoelang is dit diploma na behalen geldig. In maanden.
	public $historieopschonen = 0;
	public $vervallen = "";
	public $zelfservice = 0;					// Is dit diploma muteerbaar in de zelfservice?
	public $afdelingsspecifiek = 0;				// Optie: de afdeling waar dit diploma bij hoort.
	public $aantalhouders = 0;					// Het aantal leden dat dit diploma nu nog heeft en waarvan het geldig is.
	public $aantalonderdelen = 0;				// Het aantal examenonderdelen
	
	public object $i_org;
	
	function __construct($p_dpid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Diploma";
		$this->basefrom = $this->table . " AS DP";
		$this->vulvars($p_dpid);
		$this->ta = 12;
	}
	
	public function vulvars($p_dpid=-1) {
		if ($p_dpid >= 0) {
			$this->dpid = $p_dpid;
		}
		
		$this->voorgangerid = 0;
		$this->dpvolgende = 0;
		$this->naamvolgende = "";
		if ($this->dpid > 0) {
			$query = sprintf("SELECT DP.* FROM %s WHERE DP.RecordID=%d;", $this->basefrom, $this->dpid);
			$row = $this->execsql($query)->fetch();
			if (isset($row->RecordID)) {
				$this->naam = trim(str_replace("\"", "'", $row->Naam));
				$this->code = $row->Kode ?? "";
				$this->volgnr = $row->Volgnr ?? 0;
				$this->dptype = $row->Type ?? "D";
				if (strlen($this->naam) > 20) {
					$this->naamlogging = $this->code;
				} else {
					$this->naamlogging = $this->naam;
				}
				
				$this->afdelingsspecifiek = $row->Afdelingsspecifiek ?? 0;
				$this->organisatie = $row->ORGANIS ?? 0;
				$this->voorgangerid = $row->VoorgangerID ?? 0;
				$this->doorlooptijd = $row->Doorlooptijd ?? 0;
				$this->eindeuitgifte = $row->EindeUitgifte ?? "";
				$this->geldigheid = $row->GELDIGH ?? 0;
				$this->historieopschonen = $row->HistorieOpschonen ?? 0;
				$this->vervallen = $row->Vervallen ?? "";
				$this->zelfservice = $row->Zelfservice ?? 0;
				
				$query = sprintf("SELECT DP.RecordID, DP.Naam FROM %s WHERE DP.VoorgangerID=%d AND IFNULL(DP.Vervallen, '9999-12-31') > CURDATE() AND IFNULL(DP.EindeUitgifte, '9999-12-31') > CURDATE();", $this->basefrom, $this->dpid);
				$this->dpvolgende = 0;
				$this->naamvolgende = "";
				foreach ($this->execsql($query)->fetchAll() as $volgrow) {
					if ($this->dpvolgende == 0) {
						$this->dpvolgende = $volgrow->RecordID;
					} else {
						$this->naamvolgende .= ", ";
					}
					$this->naamvolgende .= $volgrow->Naam ?? "";
				}
				
				$query = sprintf("SELECT COUNT(*) FROM %sLiddipl AS LD WHERE LD.DatumBehaald <= CURDATE() AND IFNULL(LD.LicentieVervallenPer, '9999-12-31') > CURDATE() AND LD.DiplomaID=%d;", TABLE_PREFIX, $this->dpid);
				$this->aantalhouders = $this->scalar($query);
				
				$query = sprintf("SELECT COUNT(*) FROM %sExamenonderdeel AS EO WHERE EO.DiplomaID=%d AND LENGTH(EO.Code) > 0;", TABLE_PREFIX, $this->dpid);
				$this->aantalonderdelen = $this->scalar($query);
				
			} else {
				$this->dpid = 0;
			}
		}
		
		$this->i_org = new cls_Organisatie($this->organisatie);
	}  # cls_Diploma->vulvars
	
	public function naam($p_dpid) {
		// Bewust p_dpid niet in this->dpid gezet.
		
		$query = sprintf("SELECT IFNULL(MAX(DP.Naam), '') FROM %s WHERE DP.RecordID=%d;", $this->basefrom, $p_dpid);
		return $this->scalar($query);
	}
		
	public function lidmuteerlijst($p_lidid) {
		$this->lidid = $p_lidid;
		
		$query = sprintf("SELECT DP.*, LD.Lid, LD.DatumBehaald, LD.LicentieVervallenPer, LD.Diplomanummer, LD.RecordID AS LDID
						  FROM %s LEFT JOIN %sLiddipl AS LD ON LD.DiplomaID=DP.RecordID
						  WHERE ((LD.RecordID IS NULL) OR LD.Lid=%d);", $this->basefrom, TABLE_PREFIX, $this->lidid);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function htmloptions($p_cv=0, $p_afdfilter=-1, $p_zs=0, $p_inclvervallen=0, $p_filter="", $p_kort=0, $p_opexamen=-1) {
		/*
			$p_afdfilter = door leden van deze afdeling gehaald.
			$p_kort = alleen de naam wordt weergegeven
		*/
		
		$rv = "";
		if ($p_afdfilter > 0) {
			$w = sprintf("WHERE DP.RecordID IN (SELECT DiplomaID FROM %1\$sLiddipl AS LD WHERE LD.Lid IN (SELECT Lid FROM %1\$sLidond AS LO WHERE LO.OnderdeelID=%2\$d AND IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE()))", TABLE_PREFIX, $p_afdfilter);
		} else {
			$w = "WHERE 1=1";
		}
		if ($p_zs == 1) {
			$w .= " AND DP.Zelfservice=1";
		}
		
		if ($p_inclvervallen == 0) {
			$w .= " AND IFNULL(DP.Vervallen, '9999-12-31') >= CURDATE()";
		}
		
		if (strlen($p_filter) > 0) {
			$w .= " AND " . $p_filter;
		}
		
		if (strlen($this->where) > 0) {
			$w .= " AND " . $this->where;
		}
		
		$sep = 0;
		
		if ($p_opexamen > 0) {
			$query = sprintf("SELECT DP.*, (SELECT COUNT(*) FROM %sLiddipl AS LD WHERE LD.Examen=%d AND LD.DiplomaID=DP.RecordID) AS aantal", TABLE_PREFIX, $p_opexamen);
		} else {
			$query = "SELECT DP.*";
		}
		$query .= sprintf(" FROM %s %s ORDER BY ", $this->basefrom, $w);
		
		if ($p_opexamen > 0) {
			$query .= sprintf("IF((SELECT COUNT(*) FROM %sLiddipl AS LD WHERE LD.Examen=%d AND LD.DiplomaID=DP.RecordID) > 0, 0, 1), ", TABLE_PREFIX, $p_opexamen);
		}
		$query .= "IF(DP.EindeUitgifte IS NULL, IF(DP.Vervallen IS NULL, 0, 1), 1), ";
		if ($p_kort == 1) {
			$query .= "DP.Naam;";
		} else {
			$query .= "DP.Kode, DP.Naam;";
		}
		foreach ($this->execsql($query) as $row) {
			if ($sep == 0 and ($row->EindeUitgifte > "2000-01-01" or $row->Vervallen > "2000-01-01")) {
				$rv .= "<option disabled>--- Wordt niet meer uitgegeven / Vervallen ---</option>\n";
				$sep = 1;
			}
			$s = checked($row->RecordID, "option", $p_cv);
			if ($p_kort == 1) {
				$rv .= sprintf("<option value=%d %s>%s</option>\n", $row->RecordID, $s, $row->Naam);
			} elseif ($p_opexamen > 0 and $row->aantal > 1) {
				$rv .= sprintf("<option value=%d %s>%s (%d kandidaten)</option>\n", $row->RecordID, $s, $row->Naam, $row->aantal);
			} else {
				$rv .= sprintf("<option value=%d %s>%s - %s</option>\n", $row->RecordID, $s, $row->Kode, $row->Naam);
			}
		}
		return $rv;
		
	}  # htmloptions
	
	public function add() {
		$this->vulvars();
		$this->tas = 41;
		
		$nrid = $this->nieuwrecordid();
		
		$query = sprintf("INSERT INTO %s (RecordID, `Type`, Zelfservice, Ingevoerd) VALUES (%d, 'D', 0, SYSDATE());", $this->table, $nrid);
		if ($this->execsql($query) > 0) {
			$query = sprintf("INSERT INTO Diploma (RecordID, [Type], Zelfservice, Ingevoerd) VALUES (%d, 'D', 0, Now());", $nrid);
			$this->interface($query);
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
		$this->tas = 42;
		
		$f = sprintf("DP.Kode='%s' AND DP.RecordID<>%d", $p_waarde, $p_dpid);
		if (toegang("Diplomabeheer", 1, 1) == false) {
			$this->mess = "Je hebt geen rechten om diploma's bij te werken.";
		} elseif (strlen($p_kolom) == 0) {
			$this->mess = "Tabel Diploma: de kolom is leeg. Deze wijzging wordt niet doorgevoerd.";
		} elseif ($p_kolom == "Kode" and $this->aantal($f) > 0) {
			$this->mess = sprintf("Tabel Diploma: in record %d (%s) mag de waarde van Kode mag geen '%s' worden, want die is elders in gebruik.", $p_dpid, $this->naamlogging, $p_waarde);
		} else {
			$this->pdoupdate($p_dpid, $p_kolom, $p_waarde, $p_reden);
		}
		$this->log($p_dpid);
	}
	
	private function delete($p_dpid, $p_reden="") {
		$this->vulvars($p_dpid);
		$this->tas = 43;
		
		if ($this->pdodelete($this->dpid, $p_reden)) {
			$this->log($p_dpid);
		}
	}  # cls_Diploma->delete
	
	public function controle() {
		
		$query = sprintf("SELECT DP.* FROM %s;", $this->basefrom);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			if (strlen($row->Volgnr) == 0) {
				$this->update($row->RecordID, "Volgnr", 0, "het volgnummer leeg was");
			} elseif (strlen($row->Vervallen) >= 10 and $row->EindeUitgifte > $row->Vervallen) {
				$this->update($row->RecordID, "EindeUitgifte", $row->Vervallen, "einde uitgifte niet na vervallen mag liggen");
			} elseif (strlen($row->Vervallen) >= 10 and $row->Vervallen < date("Y-m-d") and $row->Zelfservice == 1) {
				$this->update($row->RecordID, "Zelfservice", 0, "het diploma is vervallen");
			} elseif (array_key_exists($row->Type, ARRTYPEDIPLOMA) == false) {
				$this->update($row->RecordID, "Type", "D", "het diploma geen geldig type had");
			}
		}
	}  # cls_Diploma->controle
	
	public function opschonen() {
		
		$query = sprintf("SELECT DP.RecordID FROM %s WHERE IFNULL(DP.Vervallen, '9999-12-31') < CURDATE() AND (SELECT COUNT(*) FROM %sLiddipl AS LD WHERE LD.DiplomaID=DP.RecordID)=0;", $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		$reden = "het diploma vervallen is en niemand dit diploma (meer) heeft.";
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, $reden);
		}		
		
		$query = sprintf("SELECT DP.RecordID FROM %s WHERE IFNULL(DP.EindeUitgifte, '9999-12-31') < DATE_SUB(CURDATE(), INTERVAL 1 YEAR) AND (SELECT COUNT(*) FROM %sLiddipl AS LD WHERE LD.DiplomaID=DP.RecordID)=0;", $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		$reden = "het diploma het niet meer wordt uitgegeven en niemand dit diploma (meer) heeft.";
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, $reden);
		}
		
		$this->optimize();
		
	}  # cls_Diploma->opschonen
	
}  # cls_Diploma

class cls_Liddipl extends cls_db_base {
	
	private int $ldid = 0;
	private int $dpid = 0;
	private $datbehaald = "";
	private $examen = 0;
	private string $lidnaam = "";
	private string $lidgeboortedatum = "";
	
	private $geldigheid = 0;			// Hoeveel maanden is het diploma geldig na het behalen?
	private $licentievervaltper = "";
	public $vervaltper = "";			// Het resultaat bovenstaande twee variabelen
	
	public string $ldclass = "";
	public string $ldtitle = "";
	
	public object $i_dp;
	
	function __construct($p_ldid=-1, $p_lidid=-1, $p_dpid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Liddipl";
		$this->basefrom = $this->table . " AS LD";
		$this->per = date("Y-m-d");
		$this->vulvars($p_ldid, $p_lidid, $p_dpid);
		$this->ta = 12;
	}
	
	public function vulvars($p_ldid=-1, $p_lidid=-1, $p_dpid=-1) {
		
		if ($p_ldid >= 0) {
			$this->ldid = $p_ldid;	
		}
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		if ($p_dpid >= 0) {
			$this->dpid = $p_dpid;
		}
		if ($this->ldid > 0) {
			$query = sprintf("SELECT LD.* FROM %s WHERE LD.RecordID=%d;", $this->basefrom, $this->ldid);
			$row = $this->execsql($query)->fetch();
			if (isset($row->RecordID) and $row->RecordID > 0) {
				$this->lidid = $row->Lid;
				$this->dpid = $row->DiplomaID;
				$this->datbehaald = $row->DatumBehaald;
				$this->examen = $row->Examen;
				$this->licentievervaltper = $row->LicentieVervallenPer;
			} else {
				$this->ldid = 0;
			}	
		}
		
		$this->i_dp = new cls_Diploma($this->dpid);
		
		if (strlen($this->i_dp->naam) > 20) {
			$this->naamlogging = $this->i_dp->code;
		} else {
			$this->naamlogging = $this->i_dp->naam;
		}

		if ($this->lidid > 0) {
			$query = sprintf("SELECT L.RecordID, %s AS Naam, L.GEBDATUM FROM %sLid AS L WHERE L.RecordID=%d;", $this->selectnaam, TABLE_PREFIX, $this->lidid);
			$row = $this->execsql($query)->fetch();
			if (isset($row->RecordID)) {
				$this->lidnaam = $row->Naam ?? "";
				$this->lidgeboortedatum = $row->GEBDATUM ?? "";
			} else {
				$this->lidid = 0;
			}
		}
		
		$this->ldclass = "";
		$this->ldtitle = "";
		if ($this->dpid > 0 and $this->lidid > 0) {
			$f = sprintf("LD.Lid=%d AND LD.DiplomaID=%d AND LD.Geslaagd=1 AND LD.LaatsteBeoordeling=1 AND LD.DatumBehaald < '%s' AND IFNULL(LD.LicentieVervallenPer, '9999-12-31') >= CURDATE()", $this->lidid, $this->dpid, $this->datbehaald);
			$dat = $this->max("DatumBehaald", $f);
			if (strlen($dat) == 10) {
				$this->ldclass = "dubbeldiploma";
				$this->ldtitle = sprintf("ook behaald op %s", date("d-m-Y", strtotime($dat)));
			}
		}
		
		
		if ($this->i_dp->voorgangerid > 0 and $this->lidid > 0) {
			
			$f = sprintf("LD.Lid=%d AND LD.DiplomaID=%d AND LD.Geslaagd=1 AND LD.LaatsteBeoordeling=1", $this->lidid, $this->dpid);
			$datvg = $this->max("DatumBehaald", $f);
			if (strlen($datvg) < 10) {
				$this->ldclass .= " voorgangerontbreekt";
				if (strlen($this->ldtitle) > 0) {
					$this->ldtitle . ", ";
				}
				$this->ldtitle .= sprintf("%s ontbreekt", (new cls_diploma())->naam($this->i_dp->voorgangerid));
			}
		}
		
		if ($this->i_dp->doorlooptijd > 0 and $this->i_dp->voorgangerid > 0) {
			$f = sprintf("LD.Lid=%d AND LD.DiplomaID=%d AND LD.Geslaagd=1 AND LD.LaatsteBeoordeling=1", $this->lidid, $this->i_dp->voorgangerid);
			$datvg = $this->max("DatumBehaald", $f);
			$hd = new datetime($this->datbehaald);
			$hd->modify(sprintf("-%d month", $this->i_dp->doorlooptijd));
			if (strlen($datvg) == 10 and $datvg < $hd->format("Y-m-d")) {
				$this->ldclass .= " voortgangsprobleem";
				if (strlen($this->ldtitle) > 0) {
					$this->ldtitle .= ", ";
				}
				$this->ldtitle .= sprintf("%s behaald op %s", $this->i_dp->naam, date("d-m-Y", strtotime($datvg)));
			}
		}
		
		if (strlen($this->licentievervaltper) == 10) {
			$this->vervaltper = $this->licentievervaltper;
		} elseif ($this->geldigheid == 0) {
			$this->vervaltper = "9999-12-31";
		} else {
			$hd = new datetime($this->datbehaald);
			$hd->modify(sprintf("+%d", $this->geldigheid));
			$this->vervaltper = $hd->format("Y-m-d");
			$hd = null;
		}
		
	}  # cls_Liddipl->vulvars
	
	public function overzichtlid($p_lidid, $p_per="") {
		
		if (strlen($p_per) < 10) {
			$p_per = $this->per;
		}
		
		$query = sprintf("SELECT DP.Naam, DP.Kode, DP.Zelfservice,
							O.Naam AS UitgegevenDoor,
							CONCAT(IF(LENGTH(O.Naam) > 0, CONCAT(O.Naam, ' - ') , ''), DP.Naam) AS NaamLang,
							LD.RecordID,
							LD.DatumBehaald,
							EX.Plaats,
							LD.Examen,
							LD.Diplomanummer,
							LD.LicentieVervallenPer,
							IFNULL(LD.LicentieVervallenPer, IF(DP.GELDIGH>0, DATE_ADD(LD.DatumBehaald, INTERVAL DP.GELDIGH MONTH), IF((NOT DP.Vervallen IS NULL), DP.Vervallen, null))) AS GeldigTot
							FROM (%1\$sLiddipl AS LD LEFT OUTER JOIN %1\$sExamen AS EX ON LD.Examen=EX.Nummer) INNER JOIN (%1\$sDiploma AS DP INNER JOIN %1\$sOrganisatie AS O ON DP.ORGANIS=O.Nummer) ON LD.DiplomaID=DP.RecordID
							WHERE LD.DatumBehaald > '1900-01-01' AND LD.DatumBehaald <= '%3\$s' AND LD.LaatsteBeoordeling=1 AND LD.Geslaagd=1 AND LD.Lid=%2\$d
							ORDER BY LD.DatumBehaald;", TABLE_PREFIX, $p_lidid, $p_per);
		$result = $this->execsql($query);
		return $result->fetchAll();
		
	}  # cls_Liddipl->overzichtlid
	
	public function overzichtperdiploma($p_dpid) {
		
		$query = sprintf("SELECT LD.*, %s AS NaamLid, L.GEBDATUM FROM %s INNER JOIN %sLid AS L ON LD.Lid=L.RecordID
						  WHERE LD.DatumBehaald > '1900-01-01' AND LD.DiplomaID=%d AND LD.Geslaagd=1 AND LD.LaatsteBeoordeling=1 
						  ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, LD.DatumBehaald;", $this->selectnaam, $this->basefrom, TABLE_PREFIX, $p_dpid);
		$result = $this->execsql($query);

		return $result->fetchAll();
		
	}  # cls_Liddipl->overzichtperdiploma
	
	public function overzichtperexamen($p_examen=0, $p_diploma=-1, $p_lidid=-1, $p_filter="") {

		if ($p_lidid > 0) {
			$w = sprintf("LD.Lid=%d AND LD.Examen > 0", $p_lidid);
		}elseif ($p_diploma > 0) {
			$w = sprintf("LD.Examen=%d AND LD.DiplomaID=%d", $p_examen, $p_diploma);
		} elseif ($p_examen > 0) {
			$w = sprintf("LD.Examen=%d", $p_examen);
		} else {
			$w = "1=1";
		}
		if (strlen($p_filter) > 0) {
			$w .= sprintf(" AND %s", $p_filter);
		}
		
		$query = sprintf("SELECT LD.*, %1\%s AS NaamLid, %2\$s AS AVGnaam, L.GEBDATUM, EX.Datum, EX.Plaats, EX.Proefexamen, EX.OnderdeelID
						  FROM (%3\$s INNER JOIN %4\$sExamen AS EX ON LD.Examen=EX.Nummer) INNER JOIN %4\$sLid AS L ON LD.Lid=L.RecordID
						  WHERE %5\$s ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, LD.DatumBehaald;", $this->selectnaam, $this->selectavgnaam, $this->basefrom, TABLE_PREFIX, $w);
		$result = $this->execsql($query);
		
		return $result->fetchAll();
	}  # overzichtperexamen
	
	public function perexamendiploma($p_examen, $p_dpid, $p_alleengeslaagd=0) {
		
		$xw = "";
		if ($p_alleengeslaagd == 1) {
			$xw = " AND LD.Geslaagd=1";
		}
		
		$query = sprintf("SELECT LD.*, %s AS NaamLid, %s AS Zoeknaam, L.GEBDATUM, L.GEBPLAATS, L.RelnrRedNed FROM %s INNER JOIN %sLid AS L ON LD.Lid=L.RecordID
							   WHERE LD.Examen=%d AND LD.DiplomaID=%d%s
							   ORDER BY L.Achternaam, L.TUSSENV, L.Roepnaam, LD.DatumBehaald;", $this->selectnaam, $this->selectzoeknaam, $this->basefrom, TABLE_PREFIX, $p_examen, $p_dpid, $xw);
		$result = $this->execsql($query);
		
		return $result->fetchAll();
	}  # cls_Liddipl->perexamendiploma
	
	public function lidlaatstediplomas($p_lidid, $p_aant=2) {
		$rv = "";
		$a = 0;
		$query = sprintf("SELECT DISTINCT DP.Naam FROM %s INNER JOIN %sDiploma AS DP ON LD.DiplomaID=DP.RecordID
					WHERE LD.DatumBehaald > '1900-01-01' AND LD.Lid=%d AND IFNULL(LD.LicentieVervallenPer, '9999-12-31') >= CURDATE() AND LD.Geslaagd=1 AND LD.LaatsteBeoordeling=1
					ORDER BY LD.DatumBehaald DESC, DP.Volgnr;", $this->basefrom, TABLE_PREFIX, $p_lidid);
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
		
	}  # cls_Liddipl->lidlaatstediploms
	
	public function dubbelediplomas($p_orgldid) {
		
		$query = sprintf("SELECT LD.* FROM %s WHERE LD.RecordID=%d;", $this->basefrom, $p_orgldid);
		$row = $this->execsql($query)->fetch();
		if (isset($row->RecordID)) {
			$lidid = $row->Lid;
			$dpid = $row->DiplomaID;
			$behaaldvoor = $row->DatumBehaald;

			$query = sprintf("SELECT LD.DatumBehaald FROM %s WHERE LD.DatumBehaald < '%s' AND LD.Lid=%d AND LD.DiplomaID=%d AND IFNULL(LD.LicentieVervallenPer, '9999-12-31') >= CURDATE() ORDER BY LD.DatumBehaald;", $this->basefrom, $behaaldvoor, $lidid, $dpid);
			$result = $this->execsql($query);
		
			return $result->fetchAll();
		} else {
			return false;
		}
	} # dubbelediplomas
	
	public function vervallenbinnenkort($p_lidid=0) {
		// Functie kan na 1 januari 2024 vervallen
		
		if ($_SESSION['settings']['termijnvervallendiplomasmelden'] > 0) {
			$gd = date("Y-m-d", strtotime(sprintf("-%d month", $_SESSION['settings']['termijnvervallendiplomasmelden'])));
		} else {
			$gd = date("Y-m-d");
		}
		
		$query = sprintf("SELECT D.Kode AS Code,
						  D.Naam AS Diploma,
						  CONCAT(D.Kode, ' ', D.Naam) AS DiplOms,
						  LD.DatumBehaald,
						  LD.Diplomanummer,
						  IFNULL(LD.LicentieVervallenPer, IF(D.GELDIGH>0, DATE_ADD(LD.DatumBehaald, INTERVAL D.GELDIGH MONTH), NULL)) AS VervaltPer
						  FROM %1\$sLiddipl AS LD INNER JOIN %1\$sDiploma AS D ON LD.DiplomaID=D.RecordID
						  WHERE IFNULL(D.Vervallen, CURDATE()) >= CURDATE() AND ((NOT LD.LicentieVervallenPer IS NULL) OR D.GELDIGH > 0) AND LD.Lid=%2\$d
						  AND IFNULL(LD.LicentieVervallenPer, DATE_ADD(LD.DatumBehaald, INTERVAL D.GELDIGH MONTH)) < '%3\$s' AND LD.LicentieVervallenPer >= '%3\$s'
						  ORDER BY D.Volgnr, D.Kode, LD.DatumBehaald DESC;", TABLE_PREFIX, $p_lidid, $gd);
		$result = $this->execsql($query);
		
		return $result->fetchAll();
		
	}  # cls_Liddipl->vervallenbinnenkort
	
	public function add($p_lidid, $p_dpid, $p_exdatum="", $p_examen=0) {
		$this->vulvars(0, $p_lidid, $p_dpid);
		$nrid = 0;
		$this->tas = 1;
		$proef = 0;
		
		if (strlen($p_exdatum) < 10 and $p_examen <= 0) {
			$p_exdatum = date("Y-m-d");
		} elseif ($p_examen > 0) {
			$i_ex = new cls_Examen($p_examen);
			$proef = $i_ex->proef;
			$p_exdatum = $i_ex->exdatum;
			$i_ex = null;
		} else {
			$p_examen = 0;
		}
		
		if (strlen($p_exdatum) == 0) {
			$db = "NULL";
		} else {
			$db = sprintf("'%s'", $p_exdatum);
		}
		
		if ($p_examen == 0) {
			$gs = 1;
		} elseif (strlen($p_exdatum) == 10 and $p_exdatum <= date("Y-m-d")) {
			$gs = 1;
		} else {
			$gs = 0;
		}
		
		if ($proef == 0) {
			$lb = 1;
		} else {
			$lb = 0;
		}
		
		if ($this->lidid <= 0) {
			$this->mess = "Tabel Liddipl: record niet toegevoegd, omdat het lid niet bestaat.";
			$this->tm = 12;
			
		} elseif ($this->dpid <= 0) {
			$this->mess = "Tabel Liddipl: record niet toegevoegd, omdat het diploma niet bestaat.";
			$this->tm = 12;
			
		} else {
			$dubqry_ex = sprintf("SELECT COUNT(*) FROM %s WHERE LD.Lid=%d AND LD.Examen=%d AND LD.DiplomaID=%d;", $this->basefrom, $this->lidid, $p_examen, $this->dpid);
			if ($p_examen > 0 and $this->scalar($dubqry_ex) > 0)  {
				$this->mess = sprintf("%s wordt niet toegevoegd, omdat dit record voor dit lid bij dit examen al bestaat.", $this->i_dp->naam);
				$this->tm = 11;
			} else {
				$nrid = $this->nieuwrecordid();
				$query = sprintf("INSERT INTO %s (RecordID, Lid, DiplomaID, Examen, DatumBehaald, Geslaagd, LaatsteBeoordeling, Ingevoerd) VALUES (%d, %d, %d, %d, %s, %d, %d, SYSDATE());", $this->table, $nrid, $this->lidid, $this->dpid, $p_examen, $db, $gs, $lb);
				if ($this->execsql($query) > 0) {
					$query = sprintf("INSERT INTO Liddipl (RecordID, Lid, DiplomaID, DatumBehaald, Geslaagd, LaatsteBeoordeling, Ingevoerd) VALUES (%d, %d, %d, '%s', %d, %d, SYSDATE());", $nrid, $this->lidid, $this->dpid, $p_exdatum, $gs, $lb);
					$this->interface($query);
					$this->mess = sprintf("Liddipl: Record %d (%s) is toegevoegd.", $nrid, $this->i_dp->naam);
				} else {
					$nrid = 0;
					$this->mess = "Tabel Liddipl: record niet toegevoegd.";
					$this->tm = 11;
				}
			}
		}
		$this->log($nrid);
		
		return $nrid;
		
	}  # cls_Liddipl->add
	
	public function update($p_ldid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_ldid);
		$this->tm = 1;
		$this->tas = 2;
		if ($p_kolom == "Examen" and $p_waarde != 0 and (new cls_examen())->bestaat_pk($p_waarde) == false) {
			$this->mess = sprintf("Examen %d bestaat niet, deze wijziging wordt niet verwerkt.", $p_waarde);
			
		} elseif ($p_kolom == "DatumBehaald" and $p_waarde < $this->lidgeboortedatum and $p_waarde > "1900-01-01") {
			$this->mess = "'Behaald op' mag niet voor de geboortedatum liggen, deze wijziging wordt niet verwerkt.";
			
		} elseif (($p_kolom == "DatumBehaald" or $p_kolom == "LicentieVervallenPer") and strtotime($p_waarde) == false and strlen($p_waarde) > 0) {
			$this->mess = sprintf("%s: %s is geen geldige datum, deze wijziging wordt niet verwerkt.", $p_kolom, $p_waarde);
			
		} else {
			if ($this->pdoupdate($p_ldid, $p_kolom, $p_waarde, $p_reden) > 0) {
				$this->tm = 0;
			}
		}
		$this->log($p_ldid);
		
	}  # cls_Liddipl->update
	
	public function delete($p_ldid, $p_reden="") {
		$this->vulvars($p_ldid);
		$this->tm = 0;
		$this->tas = 3;
		
		if ($this->pdodelete($this->ldid, $p_reden) > 0) {
			$this->log($this->ldid);
		}
	}
	
	public function controle($p_exid=-1) {
		
		$i_ex = new cls_Examen();
		
		$w = "";
		if ($p_exid > 0) {
			$w = sprintf(" WHERE LD.Examen=%d", $p_exid);
		}
		$query = sprintf("SELECT LD.*, L.RecordID AS LidID, L.Overleden, DP.GELDIGH, DP.Vervallen, DP.AantalBeoordelingen
						  FROM (%1\$s INNER JOIN %2\$sDiploma AS DP ON LD.DiplomaID=DP.RecordID) INNER JOIN %2\$sLid AS L ON L.RecordID=LD.Lid%3\$s;", $this->basefrom, TABLE_PREFIX, $w);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$i_ex->vulvars($row->Examen);
			$geldigtot = date("Y-m-d", strtotime(sprintf("+%d month", $row->GELDIGH), strtotime($row->DatumBehaald)));
			if (strlen($row->Overleden) == 10 and ($row->LicentieVervallenPer < "1970-01-01" or $row->Overleden < $row->LicentieVervallenPer)) {
				$this->update($row->RecordID, "LicentieVervallenPer", $row->Overleden, "het lid overleden is");
			}  elseif (strlen($row->Vervallen) >= 10 and $row->Vervallen > $row->LicentieVervallenPer) {
				$this->update($row->RecordID, "LicentieVervallenPer", $row->Vervallen, "het diploma vervallen is.");
			}  elseif (strlen($row->LicentieVervallenPer) < 10 and $row->GELDIGH > 0 and $geldigtot < date("Y-m-d")) {
				$this->update($row->RecordID, "LicentieVervallenPer", $geldigtot, "de geldigheid van dit diploma beperkt is");
			} elseif ($row->Examen > 0 and $i_ex->exid == 0) {
				$this->update($row->RecordID, "Examen", 0, "het examen niet (meer) bestaat");
			} elseif ($row->Examen > 0 and $i_ex->proef == 0 and $row->LaatsteBeoordeling == 0) {
				$this->update($row->RecordID, "LaatsteBeoordeling", 1, "bij een examen is het altijd een laatste beoordeling.");
			} elseif ($row->Examen > 0 and $i_ex->exid > 0 and $i_ex->exdatum != $row->DatumBehaald and strlen($i_ex->exdatum) == 10 and $i_ex->exdatum <= date("Y-m-d") and $row->Geslaagd == 1 and $row->LaatsteBeoordeling == 1) {
				$this->update($row->RecordID, "DatumBehaald", $i_ex->exdatum);
			} elseif ($row->Examen == 0 and $row->Geslaagd == 0) {
				$this->update($row->RecordID, "Geslaagd", 1, "buiten een examen worden alleen behaalde diploma's ingevoerd.");
			} elseif ($row->Examen > 0 and $i_ex->proef == 1 and $row->LaatsteBeoordeling == 1) {
				$this->update($row->RecordID, "LaatsteBeoordeling", 0, "bij een proefexamen kan geen laatste beoordeling zijn.");
			}
		}
		
		$i_ex = null;
	}  # cls_Liddipl->controle
	
	public function opschonen() {
		$query = sprintf("SELECT LD.RecordID FROM %s WHERE LD.DiplomaID NOT IN (SELECT DP.RecordID FROM %sDiploma AS DP);", $this->basefrom, TABLE_PREFIX);
		$reden = "het diploma niet (meer) bestaat";
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, $reden);
		}
		
		$query = sprintf("SELECT LD.RecordID FROM %s WHERE LD.Lid NOT IN (SELECT L.RecordID FROM %sLid AS L WHERE (L.Verwijderd IS NULL));", $this->basefrom, TABLE_PREFIX);
		$reden = "het lid niet (meer) bestaat";
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, $reden);
		}
		
		$query = sprintf("SELECT LD.RecordID FROM %s WHERE LD.DatumBehaald > LD.LicentieVervallenPer;", $this->basefrom);
		$reden = "Vervallen per ligt voor datum behaald";
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, $reden);
		}
		
		$query = sprintf("SELECT LD.RecordID, DP.HistorieOpschonen FROM %s INNER JOIN %sDiploma AS DP ON LD.DiplomaID=DP.RecordID
						  WHERE IFNULL(LD.LicentieVervallenPer, '9999-12-31') < DATE_SUB(CURDATE(), INTERVAL DP.HistorieOpschonen MONTH) AND DP.HistorieOpschonen > 0;", $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$reden = sprintf("de geldigheid langer dan %d maanden is verlopen.", $row->HistorieOpschonen);
			$this->delete($row->RecordID, $reden);
		}
		
		$this->tm = 0;
		$i_lm = new cls_Lidmaatschap();
		$bt = $_SESSION['settings']['liddipl_bewaartermijn'] ?? 0;
		$a = 0;
		if ($bt > 0) {
			$f = sprintf("IFNULL(LM.Opgezegd, '9999-12-31') < DATE_SUB(CURDATE(), INTERVAL %d MONTH)", $bt);
			foreach ($i_lm->basislijst($f, "LM.Opgezegd") as $lmrow) {
				$this->lidid = $lmrow->Lid;
				if ($i_lm->eindelidmaatschap($lmrow->Lid) < date("Y-m-d", strtotime(sprintf("-%d month", $bt)))) {
					$query = sprintf("SELECT LD.* FROM %s WHERE LD.Lid=%d;", $this->basefrom, $lmrow->Lid);
					foreach ($this->execsql($query)->fetchAll() as $ldrow) {
						$reden = sprintf("omdat het lidmaatschap langer dan %d maanden geleden is beëindigd.", $bt);
						if ($a < 125) {
							if ($this->delete($ldrow->RecordID, $reden) > 0) {
								$this->log($ldrow->RecordID);
								$a++;
							}
						}
					}
				}
			}
		}
		$i_lm = null;
		
		$this->optimize();
		
	}  # opschonen
	
}  #  cls_Liddipl

class cls_Examen extends cls_db_base {
	public $exid = 0;
	public $exdatum = "";
	public $explaats = "";
	public $begintijd = "";
	public $onderdeelid = 0;
	public $aantalkandidaten = 0;
	public $proef = 0;
	public $examenoms = "Examen";
	public $examenomskort = "Examen";
	
	function __construct($p_exid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Examen";
		$this->basefrom = $this->table . " AS EX";
		$this->ta = 12;
		$this->pkkol = "Nummer";
		$this->vulvars($p_exid);
	}
	
	public function vulvars($p_exid, $p_datum="", $p_onderdeel=0) {
		if ($p_exid >= 0) {
			$this->exid = $p_exid;
		} elseif (strlen($p_datum) == 10 and $p_onderdeel > 0) {
			$query = sprintf("SELECT IFNULL(MAX(EX.Nummer), 0) FROM %s WHERE EX.Datum='%s' AND EX.OnderdeelID=%d;", $this->basefrom, $p_datum, $p_onderdeel);
			$this->exid = $this->scalar($query);
		}
		if ($this->exid > 0) {
			$query = sprintf("SELECT EX.* FROM %s WHERE EX.Nummer=%d;", $this->basefrom, $this->exid);
			$row = $this->execsql($query)->fetch();
			if (isset($row->Datum)) {
				$this->onderdeelid = $row->OnderdeelID;
				$this->exdatum = $row->Datum;
				$this->explaats = $row->Plaats;
				$this->begintijd = $row->Begintijd;
				$this->proef = $row->Proefexamen;
				if ($this->proef == 1) {
					$this->examenomskort = "Proefexamen";
				}
				if ($row->OnderdeelID > 0) {
					$query = sprintf("SELECT O.* FROM %sOnderdl AS O WHERE O.RecordID=%d;", TABLE_PREFIX, $row->OnderdeelID);
					$orow = $this->execsql($query)->fetch();
					if (isset($orow->RecordID)) {
						if ($orow->ORGANIS == 2) {
							if ($this->proef == 1) {
								$this->examenomskort = "Proefzwemmen";
							} else {
								$this->examenomskort = "Afzwemmen";
							}
						}
						$this->examenoms = $this->examenomskort . " " . $orow->Kode;
					}
				}
			} else {
				$this->exid = 0;
			}
			$query = sprintf("SELECT COUNT(*) FROM %sLiddipl AS LD WHERE LD.Examen=%d;", TABLE_PREFIX, $this->exid);
			$this->aantalkandidaten = $this->scalar($query);
		} else {
			$this->exdatum = "";
			$this->onderdeelid = 0;
			$this->aantalkandidaten = 0;
		}
	}
	
	public function lijst($p_fetched=1, $p_filter="") {
		
		$w = "1=1";
		if (strlen($p_filter) > 0) {
			$w .= " AND " . $p_filter;
		}
		if (strlen($this->where) > 0) {
			$w .= " AND " . $this->where;
		}
		
		$vn = "CONCAT(IF(O.ORGANIS=2, IF(EX.Proefexamen=1, 'Proefzwemmen', 'Áfzwemmen'), IF(EX.Proefexamen=1, 'Proefexamen', 'Examen')), IF(EX.OnderdeelID > 0, CONCAT(' ', O.Kode), ''))";
		
		$query = sprintf("SELECT EX.*, O.Naam AS OndNaam, %4\$s AS ExamenOms, (SELECT COUNT(*) FROM %1\$sLiddipl AS LD WHERE LD.Examen=EX.Nummer) AS AantalBehaald FROM %2\$s LEFT OUTER JOIN %1\$sOnderdl AS O ON EX.OnderdeelID=O.RecordID WHERE %3\$s ORDER BY EX.Datum DESC;", TABLE_PREFIX, $this->basefrom, $w, $vn);
		$result = $this->execsql($query);
		
		if ($p_fetched == 1) {
			return $result->fetchAll();
		} else {
			return $result;
		}
	}  # cls_Examen->lijst
	
	public function eerstediploma() {
		if ($this->aantalkandidaten > 0 and $this->exid > 0) {
			$query = sprintf("SELECT IFNULL(MIN(LD.DiplomaID), 0) FROM %sLiddipl AS LD WHERE LD.Examen=%d;", TABLE_PREFIX, $this->exid);
			return $this->scalar($query);
		} else {
			return 0;
		}
	}
	
	public function htmloptions($p_cv=-1, $p_filter="", $p_aantkand=1) {
		global $dtfmt;
		
		$rv = "";
		$dtfmt->setPattern(DTTEXT);
		
		foreach ($this->lijst(1, $p_filter) as $row) {
			$s = "";
			if ($p_cv == $row->Nummer) {
				$s = " selected";
			}
			$rv .= sprintf("<option%s value=%d>%s | %s", $s, $row->Nummer, $dtfmt->format(strtotime($row->Datum)), $row->Plaats);
			if ($p_aantkand == 1) {
				$rv .= sprintf(" | %d kandidaten", $row->AantalBehaald);
			}
			if ($row->Proefexamen == 1) {
				$rv .= " | Proefexamen";
			}
			$rv .= "</option>\n";
		}
		
		return $rv;
	}
	
	public function add() {
		$this->tas = 11;
		$nrid = $this->nieuwrecordid();
		$query = sprintf("INSERT INTO %s (Nummer, Datum, Ingevoerd) VALUES (%d, CURDATE(), SYSDATE());", $this->table, $nrid);
		if ($this->execsql($query) > 0) {
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
		
		if ($this->pdoupdate($this->exid, $p_kolom, $p_waarde, $p_reden) > 0) {
			$this->log($this->exid);
		}
	}
	
	private function delete($p_exid, $p_reden="") {
		$this->vulvars($p_exid);
		$this->tas = 13;
		
		$contrqry = sprintf("SELECT COUNT(*) FROM %sLiddipl AS LD WHERE LD.Examen=%d;", TABLE_PREFIX, $this->exid);
		if ($this->scalar($contrqry) > 0) {
			$this->mess = sprintf("Examen %d wordt niet verwijderd, omdat er nog gerelateerde records in Liddipl zijn.", $this->exid);
			$this->log($this->exid);
		} else {
			if ($this->pdodelete($this->exid, $p_reden)) {
				$this->log($this->exid);
			}
		}
	}
	
	public function controle() {
		
	}
	
	public function opschonen() {
		$query = sprintf("SELECT EX.Nummer, EX.Datum FROM %s WHERE EX.Datum < CURDATE() AND EX.Ingevoerd < DATE_SUB(CURDATE(), INTERVAL %d DAY) AND (SELECT COUNT(*) FROM %sLiddipl AS LD WHERE LD.Examen=EX.Nummer)=0;", $this->basefrom, BEWAARTIJDNIEUWERECORDS, TABLE_PREFIX);
		$result = $this->execsql($query);
		$reden = "het examen in het verleden ligt en er zijn geen gekoppelde records (meer).";
		foreach ($result->fetchAll() as $exrow) {
			$this->delete($exrow->Nummer, $reden);
		}
		
		$this->optimize();
		
	}  # opschonen
	
}  #cls_Examen

class cls_Examenonderdeel extends cls_db_base {
	
	private int $eoid = 0;
	private int $dpid = 0;
	private int $ondid = 0;
	private int $regelnr = 0;
	
	public object $i_dp;
	
	function __construct($p_eoid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Examenonderdeel";
		$this->basefrom = $this->table . " AS EO";
		$this->pkkol = "RecordID";
		$this->ta = 12;
		$this->vulvars($p_eoid);
	}
	
	private function vulvars($p_eoid) {
		if ($p_eoid >= 0) {
			$this->eoid = $p_eoid;
		}
		
		$this->dpid = 0;
		if ($this->eoid > 0) {
			$query = sprintf("SELECT EO.* FROM %s WHERE EO.RecordID=%d;", $this->basefrom, $this->eoid);
			$row = $this->execsql($query)->fetch();
			if (isset($row->RecordID)) {
				$this->dpid = $row->DiplomaID ?? 0;
				$this->regelnr = $row->Regelnr ?? 0;
			} else {
				$this->eoid = 0;
			}
		}
		
		$this->i_dp = new cls_Diploma($this->dpid);
		$this->naamlogging = $this->i_dp->naam . " / " . $this->regelnr;
		$this->ondid = $this->i_dp->afdelingsspecifiek;
		
	}  # cls_Examenonderdeel->vulvars
	
	function add($p_dpid) {
		$this->tas = 21;
		$nrid = $this->nieuwrecordid();
		
		$f = sprintf("EO.DiplomaID=%d", $p_dpid);
		$rnr = $this->max("Regelnr", $f) + 1;
		
		$insqry = sprintf("INSERT INTO %s (RecordID, DiplomaID, Regelnr, Code, Omschrijving) VALUES (%d, %d, %d, '', '');", $this->table, $nrid, $p_dpid, $rnr);
		$rv = $this->execsql($insqry);
		
		if ($rv >= 0) {
			$this->vulvars($nrid);
			$this->mess = sprintf("Examenonderdeel %d (%s) is toegevoegd.", $nrid, $this->naamlogging);
			$this->log($nrid);
		}
		
		return $rv;
	}  # cls_Examenonderdeel->add
	
	function update($p_eoid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_eoid);
		$this->tas = 22;
		
		if ($this->pdoupdate($p_eoid, $p_kolom, $p_waarde, $p_reden)) {
			$this->log($p_eoid);
		}
		
	}
	
	function delete($p_eoid, $p_reden="") {
		$this->vulvars($p_eoid);
		$this->tas = 23;
		
		if ($this->pdodelete($p_eoid, $p_reden) > 0) {
			$this->log($p_eoid);
		}
	}
	
}  # cls_Examenonderdeel

class cls_Organisatie extends cls_db_base {
	
	private int $orgid = -1;
	public string $naam = "";
	public string $volledigenaam = "";
	
	function __construct($p_orgid=-1) {
		$this->table = TABLE_PREFIX . "Organisatie";
		$this->basefrom = $this->table . " AS Org";
		$this->pkkol = "Nummer";
		$this->ta = 20;
		$this->vulvars($p_orgid);
	}
	
	private function vulvars($p_orgid) {
		if ($p_orgid >= 0) {
			$this->orgid = $p_orgid;
		}
		
		$query = sprintf("SELECT Org.* FROM %s WHERE Org.Nummer=%d;", $this->basefrom, $this->orgid);
		$orgrow = $this->execsql($query)->fetch();
		if (isset($orgrow->Nummer)) {
			$this->naam = $orgrow->Naam ?? "";
			$this->volledigenaam = $orgrow->{'Volledige naam'} ?? "";
		} else {
			$this->orgid = 0;
		}
		
	}  # cls_Organisatie->vulvars
	
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
		
		$this->query = sprintf("SELECT Org.*, IF((SELECT IFNULL(COUNT(*), 0) FROM %1\$sDiploma AS DP WHERE DP.ORGANIS=Org.Nummer) > 0, 1, IF((SELECT IFNULL(COUNT(*), 0) FROM %1\$sOnderdl AS O WHERE O.ORGANIS=Org.Nummer) > 0, 1, 0)) AS inGebruik FROM %2\$s %3\$s ORDER BY 'Volledige naam';", TABLE_PREFIX, $this->basefrom, $this->where);
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
				$this->interface($query);
			}
		}

		return $rv;
	}
	
	public function update($p_orgid, $p_kolom, $p_waarde, $p_reden="") {
		$this->tm = 0;
		$this->tas = 2;
		
		if ($p_kolom == "Nummer") {
			$this->mess = "Het nummer mag niet worden gewijzigd. Deze wijziging wordt niet verwerkt.";
		} elseif ($p_kolom == "Naam" and strlen($p_waarde) == 0 and $p_orgid > 0) {
			$this->mess = "De afkoring mag niet leeg zijn. Deze wijziging wordt niet verwerkt.";
		} elseif (toegang("Ledenlijst/Basisgegevens/Organisaties", 0, 0)) {
			$this->pdoupdate($p_orgid, $p_kolom, $p_waarde, $p_reden);
		} else {
			$this->mess = "Je bent niet bevoegd om organisaties aan te passen.";
		}
		$this->log($p_orgid, 1);
	}
	
	public function delete($p_orgid, $p_reden="") {
		$this->tas = 3;
		
		if ($this->pdodelete($p_orgid, $p_reden) > 0) {
			$this->log($p_orgid);
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
	
	public function opschonen() {
		$query = sprintf("SELECT Nummer FROM %s WHERE Org.Gewijzigd < DATE_SUB(NOW(), INTERVAL 4 WEEK) AND Org.Nummer > 2", $this->basefrom);
		$query .= sprintf(" AND Org.Nummer NOT IN (SELECT ORGANIS FROM %sOnderdl)", TABLE_PREFIX);
		$query .= sprintf(" AND Org.Nummer NOT IN (SELECT ORGANIS FROM %sDiploma)", TABLE_PREFIX);
		$query .= ";";
		$res = $this->execsql($query);
		foreach ($res->fetchAll() as $row) {
			$this->delete($row->Nummer, "deze niet meer in gebruik is.");
		}
		
		$this->optimize();
		
	}  # opschonen
	
}  # cls_Organisatie

class cls_Evenement extends cls_db_base {
	
	public int $evid = 0;
	public $datum = "";
	public string $datumtekst = "";
	public string $omschrijving = "";
	public string $locatie = "";
	public string $evoms = "";			// Combinatie van omschrijving en datum
	public string $evclass = "";		// De classes die aan het evenement voor de opmaak gekoppeld zijn
	public string $evstyle = "";		// De stylen die aan het evenement voor de opmaak gekoppeld zijn
	public string $starttijd = "";
	public $eindtijd = "";
	public $verzameltijd = "";
	public string $tijden = "";
	public string $verwijderdop = "";
	public int $typeevenement = 0;
	public string $typeomschrijving = "";
	public int $inschrijvingopen = 0;
	public string $standaardstatus = "I";
	public int $maxpersonenperdeelname = 1;
	public int $meerderestartmomenten = 0;
	public int $organisatie = 0;			// Welk orgaan organiseert dit evenement?
	public string $naamorganisatie = "";	// De naam van het organiserende orgaan
	public string $codeorganisatie = "";	// Afkortingscode van het organiserende orgaan
	public string $emailcontact = "";		// Emailadres van de organisatie
	public int $doelgroep = 0; 				// Kolom in de tabel heet 'BeperkTotGroep'
	
	public int $aantaldeelnemers = 0;
	public int $aantalinschreven = 0;
	public int $aantalafgemeld = 0;
	
	private string $sqlaantdln = "";
	private string $sqlaantingeschreven = "";
	private string $sqlaantafgemeld = "";
	
	public object $i_et;
	public object $i_orgond;
	
	function  __construct($p_evid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Evenement";
		$this->basefrom = $this->table . " AS E";
		$this->ta = 7;
		$this->vulvars($p_evid);
		$this->sqlaantdln = sprintf("SELECT IFNULL(SUM(Aantal), 0) FROM %sEvenement_Deelnemer AS ED WHERE ED.EvenementID=E.RecordID AND Status IN ('B', 'J', 'T') AND ED.LidID > 0", TABLE_PREFIX);
		$this->sqlaantingeschreven = sprintf("SELECT COUNT(*) FROM %sEvenement_Deelnemer AS ED WHERE ED.EvenementID=E.RecordID AND Status='I' AND ED.LidID > 0", TABLE_PREFIX);
		$this->sqlaantafgemeld = sprintf("SELECT COUNT(*) FROM %sEvenement_Deelnemer AS ED WHERE ED.EvenementID=E.RecordID AND Status='X' AND ED.LidID > 0", TABLE_PREFIX);
	}

	public function vulvars($p_evid=-1) {
		global $dtfmt;
		
		if ($p_evid >= 0) {
			$this->evid = $p_evid;
		}
		
		$dtfmt->setPattern(DTTEXTWD);
		
		$this->evclass = "";
		$this->evstyle = "";
		$this->typeomschrijving = "";
		$this->tijden = "";
		$this->organisatie = 0;
		$this->naamorganisatie = "";
		$this->emailcontact = "";
		
		if ($this->evid > 0) {
			$query = sprintf("SELECT E.* FROM %s WHERE E.RecordID=%d;", $this->basefrom, $this->evid);
			$evrow = $this->execsql($query)->fetch();
			if (isset($evrow->RecordID)) {
				$this->datum = substr($evrow->Datum, 0, 10);
				$this->datumtekst = $dtfmt->format(strtotime($this->datum));
				$this->omschrijving = str_replace("\"", "'", trim($evrow->Omschrijving));
				$this->locatie = str_replace("\"", "'", trim($evrow->Locatie));
				$this->evoms = $this->omschrijving . " " . $this->datumtekst;
				$this->organisatie = $evrow->Organisatie ?? 0;
				$this->doelgroep = $evrow->BeperkTotGroep ?? 0;
				$this->typeevenement = $evrow->TypeEvenement ?? 0;
				$this->inschrijvingopen = $evrow->InschrijvingOpen ?? 0;
				$this->standaardstatus = $evrow->StandaardStatus ?? "B";
				$this->starttijd = substr($evrow->Datum, 11, 5);
				$this->eindtijd = $evrow->Eindtijd;
				$this->verzameltijd = $evrow->Verzameltijd;
				$this->maxpersonenperdeelname = $evrow->MaxPersonenPerDeelname;
				$this->meerderestartmomenten = $evrow->MeerdereStartMomenten;
				$this->ingevoerd = $evrow->Ingevoerd ?? "";
				$this->gewijzigd = $evrow->Gewijzigd ?? "";
				$this->verwijderdop = $evrow->VerwijderdOp ?? "";
				
				if ($this->starttijd > "00:00" and $this->eindtijd > $this->starttijd) {
					$this->tijden = sprintf("van %s tot %s uur", $this->starttijd, $this->eindtijd);
				} elseif ($this->starttijd > "00:00") {
					$this->tijden = sprintf("vanaf %s uur", $this->starttijd);
				}
				
				if ($this->organisatie > 0) {
					$query = sprintf("SELECT O.* FROM %sOnderdl AS O WHERE O.RecordID=%d;", TABLE_PREFIX, $this->organisatie);
					$orgrow = $this->execsql($query)->fetch();
					if (isset($orgrow->RecordID)) {
						$this->naamorganisatie = $orgrow->Naam;
						$this->codeorganisatie = $orgrow->Kode;
						if (isValidMailAddress($orgrow->CentraalEmail, 0)) {
							$this->emailcontact = $orgrow->CentraalEmail;
						}
					}
				}
				

			} else {
				$this->evid = 0;
			}
		}
		$this->i_et = new cls_Evenement_Type($this->typeevenement);
		$this->i_orgond = new cls_Onderdeel($this->organisatie);
		
		$this->evclass = $this->i_et->evclass;
		if (strlen($this->locatie) > 1) {
			$this->evclass .= " " . str_replace("'", "", str_replace(" ", "_", strtolower($this->locatie)));
		}
		if (strlen($this->i_orgond->code) > 0) {
			$this->evclass .= " " . str_replace("'", "", str_replace(" ", "_", strtolower($this->i_orgond->code)));
		}
		
		$query = sprintf("SELECT COUNT(*) FROM %sEvenement_Deelnemer AS ED WHERE ED.Status IN ('B', 'J', 'T') AND ED.LidID > 0 AND ED.EvenementID=%d;", TABLE_PREFIX, $this->evid);
		$this->aantaldeelnemers = $this->scalar($query);
		
		$query = sprintf("SELECT COUNT(*) FROM %sEvenement_Deelnemer AS ED WHERE ED.Status='I' AND ED.LidID > 0 AND ED.EvenementID=%d;", TABLE_PREFIX, $this->evid);
		$this->aantalingeschreven = $this->scalar($query);
		
		$query = sprintf("SELECT COUNT(*) FROM %sEvenement_Deelnemer AS ED WHERE ED.Status='X' AND ED.LidID > 0 AND ED.EvenementID=%d;", TABLE_PREFIX, $this->evid);
		$this->aantalafgemeld = $this->scalar($query);
		
		
	}  # cls_Evenement->vulvars
	
	public function lijst($p_soort=1, $p_datum="", $p_filter="") {
		/* p_soort:
			1=overzicht
			2=beheer
			3=inschrijving open
			4=persoonlijke agenda
			5=voor op de agenda
			6=htmloptions select
		*/

		$st = "DATE_FORMAT(E.Datum, '%H:%i', 'nl_NL')";
		
		$select = sprintf("E.RecordID, E.Datum, %s AS Starttijd, E.Omschrijving, E.Locatie, E.MeerdereStartMomenten, O.Naam AS OrgNaam,
							O.CentraalEmail AS Email, E.Verzameltijd, E.Eindtijd, ET.Omschrijving AS OmsType, ET.Soort, ET.Tekstkleur, ET.Achtergrondkleur, ET.Vet, ET.Cursief,
							CASE E.InschrijvingOpen WHEN 0 THEN 'Nee' ELSE 'Ja' END AS `Ins. open?`,
							(%s) AS AantalDln, (%s) AS AantAfgemeld", $st, $this->sqlaantdln, $this->sqlaantafgemeld);
		$where = sprintf("E.Datum >= DATE_SUB(CURDATE(), INTERVAL 4 DAY) AND IFNULL(E.VerwijderdOp, '2000-01-01') < '2012-01-01' AND (%s) > 0", $this->sqlaantdln);
		if (WEBMASTER == false) {
			$where .= sprintf(" AND (E.BeperkTotGroep IN (%1\$s) OR E.Organisatie IN (%1\$s))", $_SESSION["lidgroepen"]);
		}
		$ord = "E.Datum";
		if ($p_soort == 2) {
			$select = sprintf("E.RecordID, E.Datum, IF(RIGHT(E.Datum, 8)='00:00:00', '', %s) AS Starttijd, E.Omschrijving, E.Locatie, (%s) AS Dln, O.Naam AS OrgNaam, O.CentraalEmail AS Email, E.Eindtijd,
					   E.InschrijvingOpen, ET.Omschrijving AS typeOms, ET.Soort", $st, $this->sqlaantdln);
			$where = "IFNULL(E.VerwijderdOp, '1900-01-01') < '2012-01-01'";		
			if (WEBMASTER == false) {
				$where .= sprintf(" AND E.Organisatie IN (%s)", $_SESSION["lidgroepen"]);
			}
			if (strlen($p_filter) > 0) {
				$where .= " AND " . $p_filter;
			}
			$ord = "E.Datum DESC";
			
		} elseif ($p_soort == 3) {
			$select = "E.*, ET.Omschrijving AS TypeEvenement, O.CentraalEmail AS EmailOrganisatie";
			$where = sprintf("E.Datum > NOW() AND IFNULL(E.VerwijderdOp, '1900-01-01') < '2012-01-01' AND E.InschrijvingOpen=1 AND E.BeperkTotGroep IN (%s)", $_SESSION["lidgroepen"]);
			
		} elseif ($p_soort == 4) {
			$select = sprintf("E.Datum, E.Omschrijving, E.Locatie, E.MeerdereStartMomenten, E.RecordID, (%s) AS Dln, E.BeperkTotGroep", $this->sqlaantdln);
			if (strlen($p_datum) == 10) {
				$where = sprintf("LEFT(E.Datum, 10)='%s'", $p_datum);
			} else {
				$where = "E.Datum > DATE_SUB(NOW(), INTERVAL 36 HOUR)";
			}
			$where .= sprintf(" AND IFNULL(E.VerwijderdOp, '1900-01-01') < '2012-01-01' AND E.BeperkTotGroep IN (%s)", $_SESSION["lidgroepen"]);
			$ord = "E.Datum";
			
		} elseif ($p_soort == 5) {
			$where = sprintf("LEFT(E.Datum, 10)='%s' AND IFNULL(E.VerwijderdOp, '1900-01-01') < '2012-01-01'", $p_datum);
			$ord = "E.Datum";
			
		} elseif ($p_soort == 6) {
			$where = "E.Datum >= DATE_SUB(CURDATE(), INTERVAL 1 MONTH) AND IFNULL(E.VerwijderdOp, '2000-01-01') < '2012-01-01'";
		}
		$query = sprintf("SELECT %1\$s
							FROM (%2\$s LEFT OUTER JOIN %3\$sOnderdl AS O ON E.Organisatie=O.RecordID) INNER JOIN %3\$sEvenement_Type AS ET ON E.TypeEvenement=ET.RecordID
							WHERE %4\$s
							ORDER BY %5\$s;", $select, $this->basefrom, TABLE_PREFIX, $where, $ord);
//							debug($query);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}  # cls_Evenement->lijst
	
	public function record($p_evid) {
		$this->vulvars($p_evid);
		$query = sprintf("SELECT E.*, ET.Omschrijving AS OmsType, (%s) AS AantDln, (%s) AantInschr, (%s) AS AantAfgemeld, ET.Tekstkleur, ET.Achtergrondkleur, ET.Vet, ET.Cursief
								FROM %s LEFT JOIN %sEvenement_Type AS ET ON ET.RecordID=E.TypeEvenement	WHERE E.RecordID=%d;", $this->sqlaantdln, $this->sqlaantingeschreven, $this->sqlaantafgemeld, $this->basefrom, TABLE_PREFIX, $this->evid);
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
	public function htmloptions($p_cv=-1, $p_filter="") {
		global $dtfmt;
		
		$evrows = $this->lijst(6, "". $p_filter);
		$rv = "";
		foreach ($evrows as $evrow) {
			$oms = $evrow->Omschrijving;
			$oms .= " " . $dtfmt->format(strtotime($evrow->Datum));
			$rv .= sprintf("<option value=%d%s>%s</option>\n", $evrow->RecordID, checked($p_cv, "option", $evrow->RecordID), $oms);
		}
		return $rv;
	}  # potdeelnemers->htmloptions
			
	public function potdeelnemers($p_evid) {
		$this->vulvars($p_evid);
		
		$where = "";		
		if ($this->doelgroep > 0) {
			$where = sprintf("AND LO.OnderdeelID=%1\$d AND LO.Vanaf <= '%2\$s' AND IFNULL(LO.Opgezegd, '9999-12-31') >= '%2\$s'", $this->doelgroep, $this->datum);
		}
		$query = sprintf("SELECT DISTINCT L.RecordID AS LidID, %1\$s AS Naam
							FROM %2\$sLid AS L LEFT OUTER JOIN %2\$sLidond AS LO ON L.RecordID=LO.Lid
							WHERE (SELECT COUNT(*) FROM %2\$sEvenement_Deelnemer AS ED WHERE L.RecordID=ED.LidID AND ED.EvenementID=%3\$d)=0
							%4\$s
							ORDER BY %1\$s;", $this->selectzoeknaam, TABLE_PREFIX, $this->evid, $where);
		$result = $this->execsql($query);
		return $result->fetchAll();
		
	}  # cls_Evenement->potdeelnemers
	
	public function add() {
		$this->tas = 1;
		
		$nrid = $this->nieuwrecordid();
		$this->query = sprintf("INSERT INTO %s (RecordID, Omschrijving, TypeEvenement, InschrijvingOpen, StandaardStatus) VALUES (%d, '', 0, 0, 'B');", $this->table, $nrid);
		if ($this->execsql() > 0) {
			$this->mess = sprintf("Evenement %d is toegevoegd.", $nrid);
			$this->log($nrid);
			return $nrid;
		} else {
			return 0;
		}
	}  # cls_Evenement->add
		
	public function update($p_evid, $p_kolom, $p_waarde, $p_reden="") {
		$this->tas = 2;
		
		if (toegang("Evenementen/Beheer", 0, 0)) {
			$this->pdoupdate($p_evid, $p_kolom, $p_waarde, $p_reden);
		} else {
			$this->mess = "Je bent niet bevoegd om evenementen aan te passen.";
		}
		$this->log($p_evid);
	}
	
	public function delete($p_evid, $p_reden="") {
		$this->tas = 3;
		$this->vulvars($p_evid);
		
		if ($this->pdodelete($this->evid, $p_reden) > 0) {
			$this->log($this->evid);
		}
	}  # cls_Evenement->delete
	
	public function controle() {
		
		$i_et = new cls_Evenement_Type();
		
		foreach($this->basislijst() as $row) {
			$f = sprintf("ET.RecordID=%d", $row->TypeEvenement);
			if ($i_et->aantal($f) == 0) {
				$reden = sprintf("het evenement type %d niet (meer) bestaat.", $row->TypeEvenement);
				$this->update($row->RecordID, "TypeEvenement", $i_et->min("RecordID"), $reden);
			}
		}
		
		$i_et = null;
		
	}  # cls_Evenement->controle
	
	public function opschonen() {
		
		$this->query = sprintf("SELECT E.* FROM %s WHERE IFNULL(E.VerwijderdOp, '1900-01-01') > '2000-01-01';", $this->basefrom);
		foreach($this->execsql()->fetchAll() as $row) {
			$contrqry = sprintf("SELECT COUNT(*) FROM %sEvenement_Deelnemer AS ED WHERE ED.EvenementID=%d;", TABLE_PREFIX, $row->RecordID);
			if ((new cls_db_base())->scalar($contrqry) ==  0) {
				$this->delete($row->RecordID, "omdat het al verwijderd is gemarkeerd en er geen deelnemers (meer) zijn.");
			}
		}
		
		$this->optimize();
		
	}  # cls_Evenement->opschonen
	
}  # cls_Evenement

class cls_Evenement_Deelnemer extends cls_db_base {
	
	public int $edid = 0;
	public int $evid = 0;
	public string $naamdln = "";
	public string $status = "G";
	public string $statusoms = "";
	public string $opmerking = "";
	public string $functie = "";
	public int $aantal= 1;
	public string $starttijd = "";
	public string $tijden = "";
	private $casestatus = "";
	private $magmuteren = false;
	public int $aanwezig = 0;
	
	public object $i_ev;
	
	function __construct($p_evid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Evenement_Deelnemer";
		$this->basefrom = $this->table . " AS ED";
		$this->ta = 7;
		$this->per = date("Y-m-d");
		$this->vulvars($p_evid);
		
		$this->casestatus = "CASE ED.Status ";
		foreach (ARRDLNSTATUS as $c => $o) {
			$this->casestatus .= sprintf("WHEN '%s' THEN '%s' ", $c, $o);
		}
		$this->casestatus .= "END";
	}
	
	public function vulvars($p_evid=-1, $p_lidid=-1, $p_edid=-1) {
		if ($p_evid >= 0) {
			$this->evid = $p_evid;
		}
		if ($p_lidid >= 0) {
			$this->lidid = $p_lidid;
		}
		
		if ($p_edid >= 0) {
			$this->edid = $p_edid;
		}
		
		if ($this->evid > 0 and $this->lidid > 0 and $this->edid <= 0) {
			$query = sprintf("SELECT IFNULL(ED.RecordID, 0) FROM %s WHERE ED.LidID=%d AND ED.EvenementID=%d;", $this->basefrom, $this->lidid, $this->evid);
			$this->edid = $this->scalar($query);
		}
		
		$this->naamdln = "";
		$this->opmerking = "";
		$this->functie = "";
		$this->status = "G";
		$this->statusoms = "";
		$this->aantal = 1;
		$this->starttijd = "";
		$this->aanwezig = 0;
		if ($this->edid > 0) {
			$this->evid = 0;
			$this->query = sprintf("SELECT ED.* FROM %s WHERE ED.RecordID=%d;", $this->basefrom, $this->edid);
			$row = $this->execsql()->fetch();
			if (isset($row->RecordID)) {
				$this->evid = $row->EvenementID ?? 0;
				$this->lidid = $row->LidID ?? 0;
				if ($this->lidid > 0) {
					$this->naamdln = (new cls_Lid())->naam($this->lidid);
				}
				$this->opmerking = str_replace("\"", "'", $row->Opmerking);
				$this->functie = str_replace("\"", "'", $row->Functie);
				$this->aantal = $row->Aantal ?? 1;
				$this->starttijd = $row->StartMoment ?? "";
				$this->status = $row->Status ?? "B";
				$this->statusoms = ARRDLNSTATUS[$this->status];
				if ($this->status == "B" or $this->status == "I" or $this->status == "J") {
					$this->aanwezig = 1;
				}
				$this->ingevoerd = $row->Ingevoerd ?? "";
				$this->gewijzigd = $row->Gewijzigd ?? "";
			} else {
				$this->edid = 0;
			}
		}
		
		$this->i_ev = new cls_Evenement($this->evid);
		
		$this->naamlogging = $this->i_ev->evoms;
		if ($this->i_ev->meerderestartmomenten == 0 and $this->i_ev->starttijd > "00:00") {
			$this->starttijd = $this->i_ev->starttijd;
		}
		if (WEBMASTER or in_array($this->organisatie, explode(",", $_SESSION['lidgroepen']))) {
			$this->magmuteren = true;
		}
		$this->onlineinschrijving = 0;
		if ($this->i_ev->inschrijvingopen == 1 and $this->i_ev->datum >= date("Y-m-d") and in_array($this->i_ev->doelgroep, explode(",", $_SESSION['lidgroepen']))) {
			$this->onlineinschrijving = 1;
		}
		
		$this->tijden = "";
		if ($this->starttijd > "00:00" and $this->i_ev->eindtijd > "00:00") {
			$this->tijden = sprintf("%s tot %s uur", $this->starttijd, $this->i_ev->eindtijd);
		} elseif ($this->starttijd > "00:00") {
			$this->tijden = sprintf("om %s uur", $this->starttijd);
		}
		$this->tijden = str_replace(" ", "&nbsp;", $this->tijden);
				
		if ($this->lidid > 0 and $this->lidid == $_SESSION['lidid']) {
			$this->magmuteren = true;
		}
		
	}  # cls_Evenement_Deelnemer->vulvars
	
	public function lijst($p_evid) {
		$this->evid = $p_evid;
		
		$query = sprintf("SELECT ED.*, (%s) AS NaamLid, (%s) AS Lidnr, %s AS StatusDln FROM %s INNER JOIN %sLid AS L ON ED.LidID=L.RecordID WHERE ED.EvenementID=%d;", $this->selectnaam, $this->selectlidnr, $this->casestatus, $this->basefrom, TABLE_PREFIX, $this->evid);
		$result = $this->execsql($query);
		
		return $result->fetchAll();
	}  # cls_Evenement_Deelnemer->lijst
	
	public function record($p_edid, $p_lidid=-1, $p_evid=-1) {
		if ($p_edid >= 0) {
			$this->edid = $p_edid;
		}
		$this->vulvars($p_evid, $p_lidid);
		
		$query = sprintf("SELECT ED.*, (%s) AS StatusDln, E.Omschrijving AS OmsEvenement, E.Datum AS DatumEvenement, E.Locatie, E.Email AS EmailEvenement
						  FROM %s AS ED INNER JOIN %sEvenement AS E ON ED.EvenementID=E.RecordID WHERE ED.RecordID=%d;", $this->casestatus, $this->table, TABLE_PREFIX, $this->edid);
		$result = $this->execsql($query);
		return $result->fetch();
	}
	
	public function overzichtevenement($p_evid, $p_stat="", $p_overzicht=0) {
		$this->vulvars($p_evid);
		if (strlen($p_stat) == 1) {
			$xw = sprintf("AND ED.Status='%s'", $p_stat);
		} elseif (strlen($p_stat) > 1) {
			$xw = sprintf("AND ED.Status IN (%s)", $p_stat);
		} else {
			$xw = "";
		}
		if ($p_overzicht == 1) {
			if (strlen($xw) > 0) {
				$xw .= " AND ";
			}
			$xw .= "ED.LidID > 0";
		}
		
		if ($this->i_ev->meerderestartmomenten == 1) {
			$order = "ED.StartMoment, L.Achternaam, L.TUSSENV, L.Roepnaam";
		} else {
			$order = "L.Achternaam, L.TUSSENV, L.Roepnaam";
		}
		
		$query = sprintf("SELECT ED.RecordID, %1\$s AS NaamDeelnemer, %2\$s AS Telefoon, ED.StartMoment, IF(E.MeerdereStartMomenten=1, IFNULL(ED.StartMoment, ''), SUBSTRING(E.Datum, 12, 5)) AS Starttijd, E.MeerdereStartMomenten,
							ED.Status, ED.Opmerking, ED.Functie, ED.LidID, ED.Aantal, L.Geslacht, ED.Ingevoerd
							FROM (%3\$s LEFT OUTER JOIN %4\$sLid AS L ON ED.LidID=L.RecordID) INNER JOIN %4\$sEvenement AS E ON E.RecordID=ED.EvenementID
							WHERE ED.EvenementID=%5\$d %6\$s
							ORDER BY %7\$s;", $this->selectnaam, $this->selecttelefoon, $this->basefrom, TABLE_PREFIX, $p_evid, $xw, $order);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}  #cls_Evenement_Deelnemer->overzichtevenement

	public function overzichtlid($p_lidid) {
		
		$query = "SELECT ED.RecordID, E.Datum, E.Omschrijving, E.Verzameltijd, DATE_FORMAT(E.Datum, '%H:%i') AS Starttijd, E.Locatie, E.Eindtijd, E.Email, ED.Opmerking, ED.Functie,
					IF(E.MeerdereStartMomenten=1, IFNULL(ED.StartMoment, ''), SUBSTRING(E.Datum, 12, 5)) AS Starttijd, E.MeerdereStartMomenten";
		$query .= ", CASE ED.Status";
		foreach (ARRDLNSTATUS as $s => $o) {
			$query .= sprintf(" WHEN '%s' THEN '%s'", $s, $o);
		}
		$query .= " END AS Status ";
		$query .= ", IF(Datum >= CURDATE(), IF(ED.Status IN ('A', 'B', 'I'), 1, 0), 0) AS inAgenda ";
		$query .= sprintf("FROM %1\$sEvenement AS E INNER JOIN %1\$sEvenement_Deelnemer AS ED ON E.RecordID = ED.EvenementID
								WHERE ED.LidID=%2\$d
								ORDER BY E.Datum DESC;", TABLE_PREFIX, $p_lidid);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}  # cls_Evenement_Deelnemer->overzichtlid
	
	public function agendaknop() {
		
		$o = urlencode($this->i_ev->evoms);
		$vn = $_SESSION['settings']['naamvereniging_afkorting'] ?? "";
		if (strlen($vn) > 0) {
			$o = $vn . ": " . $o;
		}
		$t = date("Ymd\THis", strtotime($this->i_ev->datum . " " . $this->starttijd)) . "/" . date("Ymd\T", strtotime($this->i_ev->datum . " "));
		if ($this->i_ev->eindtijd > $this->starttijd) {
			 $t .= str_replace(":", "", $this->i_ev->eindtijd) . "00";
		} else {
			$t .= "235959";
		}
		if (strlen($this->i_ev->verzameltijd) > 3) {
			$d = sprintf("Om %s verzamelen/inloop", $this->i_ev->verzameltijd);
		} else {
			$d = "";
		}
		$gu = sprintf("https://calendar.google.com/event?action=TEMPLATE&text=%s", $o);
		$gu .= sprintf("&#038;dates=%s", $t);
		$gu .= sprintf("&#038;details=%s", urlencode($d));
		$gu .= sprintf("&#038;location=%s", urldecode($this->i_ev->locatie));
		$gu .= "&#038;trp=false";
		$gu .= sprintf("&#038;sprop=website:%s", BASISURL);
			
		return sprintf("<a href='%s' target='_blank' rel='nofollow'><img src='https://img.icons8.com/plasticine/100/000000/google-logo.png' title='Zet in je Google-agenda'></a>", $gu);
	}  # cls_Evenement_Deelnemer->agendaknop
	
	public function add($p_evid, $p_lidid, $p_status="") {
		$this->edid = 0;
		$this->vulvars($p_evid, $p_lidid);
		$nrid = $this->nieuwrecordid();
		$this->tas = 11;
		
		if (strlen($p_status) == 0){
			$p_status = $this->i_ev->standaardstatus;
		}
		
		$query = sprintf("SELECT COUNT(*) FROM %s AS ED WHERE ED.LidID=%d AND ED.EvenementID=%d;", $this->table, $this->lidid, $this->evid);
		if ($this->scalar($query) == 0) {
			$query = sprintf("INSERT INTO %sEvenement_Deelnemer (RecordID, LidID, EvenementID, Status, Opmerking, Functie) VALUES (%d, %d, %d, '%s', '', '');", TABLE_PREFIX, $nrid, $this->lidid, $this->evid, $p_status);
			if ($this->execsql($query) > 0) {
				$this->edid = $nrid;
				$this->vulvars();
				$this->mess = sprintf("Tabel Evenement_Deelnemer: Record %d is bij evenement %d (%s) met status '%s' toegevoegd.", $this->edid, $p_evid, $this->naamlogging, $p_status);
				$this->log($nrid);
				return $nrid;
			} else {
				return 0;
			}
		} else {
			$this->mess = sprintf("Lid %d is al deelnemer aan evenement %d (%s) en wordt niet toegevoegd.", $this->lidid, $this->evid, $this->naamlogging);
			$this->log(0, 1);
			return 0;
		}
	}  # cls_Evenement_Deelnemer->add
	
	public function update($p_edid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars(-1, -1, $p_edid);
		$this->tas = 12;
		$rv = 0;
		
		if ($this->magmuteren) {
			if ($this->pdoupdate($this->edid, $p_kolom, $p_waarde, $p_reden)) {
				$rv = 1;
				$this->vulvars();
			}
		} else {
			$this->mess = "Je bent niet bevoegd om deelnemers bij dit evenement te muteren.";
		}
		$this->log($p_edid);
		
		return $rv;
	}  # cls_Evenement_Deelnemer->update
	
	public function delete($p_edid, $p_reden="") {
		$this->edid = $p_edid;
		$this->vulvars();
		$this->tas = 13;
		
		if (toegang("Evenementen/Beheer", 0, 0) and $this->magmuteren) {
			$this->pdodelete($this->edid, $p_reden);
		} else {
			$this->mess = "Je bent niet bevoegd om deelnemers bij dit evenement te verwijderen.";
		}
		$this->log($this->edid);
	}
	
	public function controle() {
	}
	
	public function opschonen($p_evid=-1) {
		
		if ($p_evid > 0) {
			$w = sprintf(" AND ED.EvenementID=%d", $p_evid);
		} else {
			$w = "";
		}
		
		$query = sprintf("SELECT ED.RecordID FROM %s WHERE ED.EvenementID NOT IN (SELECT E.RecordID FROM %sEvenement AS E)%s;", $this->basefrom, TABLE_PREFIX, $w);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, "het evenement niet (meer) bestaat.");
		}

		$query = sprintf("SELECT ED.RecordID FROM %s WHERE ED.LidID NOT IN (SELECT L.RecordID FROM %sLid AS L WHERE (L.Verwijderd IS NULL))%s;", $this->basefrom, TABLE_PREFIX, $w);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, "het lid niet (meer) bestaat.");
		}
		
		$query = sprintf("SELECT ED.RecordID FROM %s WHERE (ED.Status='G' OR LENGTH(TRIM(ED.Status))=0) AND LENGTH(IFNULL(TRIM(ED.Opmerking), ''))=0 AND LENGTH(IFNULL(TRIM(ED.Functie), ''))=0%s;", $this->basefrom, $w);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, "de status geen is en er geen opmerking en geen functie ingevuld is.");
		}
		
		$this->optimize();
		
	}  # opschonen
	
}  #  cls_Evenement_Deelnemer

class cls_Evenement_Type extends cls_db_base {
	public int $etid = 0;
	public string $omschrijving = "";
	public string $soort = "";
	public string $tekstkleur = "";
	public string $achtergrondkleur = "";
	public int $vet = 0;
	public int $cursief = 0;
	public string $style = "";
	public string $evclass = "";
	public int $aantalgekoppeldeevenementen = 0;
	
	function __construct() {
		$this->table = TABLE_PREFIX . "Evenement_Type";
		$this->basefrom = $this->table . " AS ET";
		$this->ta = 7;
	}
	
	public function vulvars($p_etid=-1) {
		if ($p_etid >= 0) {
			$this->etid = $p_etid;
		}
		
		$this->omschrijving = "";
		$this->style = "";
		$this->evclass = "";
		
		if ($this->etid > 0) {
			$query = sprintf("SELECT ET.* FROM %s WHERE ET.RecordID=%d;", $this->basefrom, $this->etid);
			$etrow = $this->execsql($query)->fetch();
			if (isset($etrow->RecordID)) {
				$this->omschrijving = $etrow->Omschrijving ?? "";
				$this->omschrijving = str_replace("\"", "'", trim($this->omschrijving));
				$this->naamlogging = $this->omschrijving;				
				$this->soort = $etrow->Soort ?? "";
				$this->tekstkleur = $etrow->Tekstkleur ?? "";
				$this->achtergrondkleur = $etrow->Achtergrondkleur ?? "";
				$this->vet = $etrow->Vet ?? 0;
				$this->cursief = $etrow->Cursief ?? 0;
				if (strlen($this->tekstkleur) > 2 or strlen($this->achtergrondkleur) > 2 or $this->vet == 1 or $this->cursief == 1) {
					$this->style = " style='";
					if (strlen($this->tekstkleur) > 2) {
						$this->style .= "color: " . $this->tekstkleur . "; ";
					}
					if (strlen($this->achtergrondkleur) > 2) {
						$this->style .= "background-color: " . $this->achtergrondkleur . "; ";
					}
					if ($this->vet == 1) {
						$this->style .= "font-weight: bold; ";
					}
					if ($this->cursief == 1) {
						$this->style .= "font-style: italic;";
					}
					$this->style .= "'";
				}
				
				$this->evclass = str_replace("'", "", str_replace(" ", "_", strtolower($this->omschrijving)));
				$f = sprintf("E.TypeEvenement=%d", $this->etid);
				$this->aantalgekoppeldeevenementen = (new cls_evenement())->aantal($f);
			} else {
				$this->etid = 0;
			}
		}
	}  # cls_Evenement_Type->vulvars
	
	public function htmloptions($cv) {
		$rv = "";
		foreach ($this->basislijst() as $row) {
			$s = checked($cv, "option", $row->RecordID);
			$rv .= sprintf("<option value=%d %s>%s</option>\n", $row->RecordID, $s, $row->Omschrijving);
		}
		return $rv;
	}  # cls_Evenement_Type->htmloptions
	
	public function add($p_waarde="*** Nieuw ***") {
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
		$this->vulvars($p_etid);
		$this->tas = 22;
		if ($this->pdoupdate($this->etid, $p_kolom, $p_waarde) > 0) {
			$this->log($p_etid);
		}
	}
	
	public function delete($p_etid) {
		$this->vulvars($p_etid);
		$this->tas = 23;
		
		$query = sprintf("SELECT COUNT(*) FROM %sEvenement AS E WHERE E.TypeEvenement=%d;", TABLE_PREFIX, $this->etid);
		if ($this->scalar($query) > 0) {
			$this->mess = sprintf("Type %d (%s) is nog in gebruik en mag niet worden verwijderd.", $this->etid, $this->naamlogging);
		} else {
			$this->pdodelete($this->etid);
		}
		$this->tm = 1;
		$this->log($p_etid);
	}
	
	public function controle() {
	}
	
	public function opschonen() {
		
		$query = sprintf("SELECT ET.RecordID FROM %s WHERE ET.Ingevoerd < DATE_SUB(CURDATE(), INTERVAL %d DAY) AND (ET.RecordID NOT IN (SELECT E.TypeEvenement FROM %sEvenement AS E))", $this->basefrom, BEWAARTIJDNIEUWERECORDS, TABLE_PREFIX);
		
		foreach ($this->execsql($query)->fetchAll() as $etrow) {
			$this->delete($etrow->RecordID, "het type niet (meer) in gebruik is.");
		}
		
		$this->optimize();
		
	}
	
}  # cls_Evenement_Type

class cls_Artikel extends cls_db_base{
	private $artid = 0;
	public string $code = "";
	public string $omschrijving = "";
	public string $maat = "";
	public float $verkoopprijs = 0;
	
	function __construct($p_artid=-1) {
		$this->table = TABLE_PREFIX . "WS_Artikel";
		$this->basefrom = $this->table . " AS Art";
		$this->per = date("Y-m-d");
		$this->ta = 10;
		$this->vulvars($p_artid);
	}
	
	private function vulvars($p_artid=-1) {
		if ($p_artid >= 0) {
			$this->artid = $p_artid;
		}
		if ($this->artid > 0) {
			$query = sprintf("SELECT Art.* FROM %s WHERE ART.RecordID=%d;", $this->basefrom, $this->artid);
			$artrow = $this->execsql($query)->fetch();
			if (isset($artrow->RecordID)) {
				$this->code = $artrow->Code ?? "";
				$this->omschrijving = $artrow->Omschrijving ?? "";
				$this->maat = $artrow->Maat ?? "";
				$this->verkoopprijs = round($artrow->Verkoopprijs ?? 0, 2);
			} else {
				$this->artid = 0;
			}
		}
	}
	
	public function lijst($p_type="bestellijst") {
		$query = sprintf("SELECT Art.*, CONCAT(IFNULL(Art.Omschrijving, ''), IF(LENGTH(Art.Maat)>0, CONCAT(' (', Art.Maat, ')'), '')) AS OmsMaat, CONCAT(Art.Code, ' - ', IFNULL(Art.Omschrijving, ''), ' ', IFNULL(Art.Maat, '')) AS CodeOmsMaat,
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
		
		$query = sprintf("SELECT Art.RecordID, CONCAT(Art.Code, ' - ' , IFNULL(Art.Omschrijving, ''), ' ', IFNULL(Art.Maat, '')) AS CodeOmsMaat FROM %s AS Art", $this->table);
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
	
	public function add($p_code="") {
		$this->tas = 11;
		$nrid = $this->nieuwrecordid();
		if (strlen($p_code) == 0) {
			$p_code = "Nw" . $nrid;
		}
		
		$query = sprintf("INSERT INTO %s (RecordID, Code) VALUES  (%d, '%s');", $this->table, $nrid, $p_code);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Artikel %d met code '%s' is toegevoegd.", $nrid, $p_code);
			$this->log($nrid);
		}
	}  # cls_Artikel->add

	public function update($p_artid, $p_kolom, $p_waarde) {
		$this->tas = 12;
		
		if ($this->pdoupdate($p_artid, $p_kolom, $p_waarde) > 0) {
			$this->log($p_artid);
		}
	}
	
	public function delete($p_artikelid) {
		$this->tas = 13;
		
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
	
	public function controle() {
	}
	
	public function opschonen() {
		
		$query = sprintf("SELECT RecordID FROM %1\$s AS Art WHERE IFNULL(Art.BeschikbaarTot, CURDATE()) < CURDATE() AND Art.RecordID NOT IN (SELECT Ord.Artikel FROM %2\$sWS_Orderregel AS Ord) AND Art.RecordID NOT IN (SELECT VB.ArtikelID FROM %2\$sWS_Voorraadboeking AS VB);", $this->table, TABLE_PREFIX);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID);
		}
		
		$this->optimize();
		
	}  # opschonen
	
}  # cls_Artikel

class cls_Orderregel extends cls_db_base {
	
	private $artid = 0;
	private $orid = 0;
	
	public object $i_art;

	function __construct($p_lidid=0) {
		$this->table = TABLE_PREFIX . "WS_Orderregel";
		$this->basefrom = $this->table . " AS Ord";
		$this->lidid = $p_lidid;
		$this->per = date("Y-m-d");
		$this->ta = 10;
	}
	
	private function vulvars($p_orid=0) {
		$this->orid = $p_orid;
		if ($this->orid <= 0 and $this->artid > 0 and $this->lidid > 0) {
			$query = sprintf("SELECT IFNULL(RecordID, 0) FROM %s WHERE Ord.Artikel=%d AND Ord.Lid=%d;", $this->basefrom, $this->artid, $this->lidid);
			$this->orid = $this->scalar($query);
			
		}
			
		if ($this->orid > 0) {
			$query = sprintf("SELECT Ord.* FROM %s WHERE Ord.RecordID=%d;", $this->basefrom, $this->orid);
			$row = $this->execsql($query)->fetch();
			if (isset($row->RecordID)) {
				$this->orid = $row->RecordID;
				$this->lidid = $row->Lid ?? 0;
				$this->artid = $row->Artikel ?? 0;
				$this->ingevoerd = $row->Ingevoerd ?? "";
				$this->gewijzigd = $row->Gewijzigd ?? "";
			} else {
				$this->orid = 0;
			}
		}
	
		$this->i_art = new cls_Artkel($this->artid);
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
					Art.Code, Art.Omschrijving, Art.Maat, CONCAT(Art.Omschrijving, ' ', IFNULL(Art.Maat, '')) as ArtOms, CONCAT(Art.Code, ' - ', Art.Omschrijving, ' ', IFNULL(Art.Maat, '')) AS CodeOmsMaat,
					(SELECT IFNULL(SUM(VB.Aantal), 0) * -1 FROM %3\$sWS_Voorraadboeking AS VB WHERE VB.OrderregelID=Ord.RecordID) AS AantalGeleverd,
					(SELECT IFNULL(SUM(Aantal), 0) FROM %3\$sWS_Voorraadboeking AS VB WHERE VB.ArtikelID=Art.RecordID) AS Voorraad, Art.MaxAantalPerLid
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
		
		$query = sprintf("INSERT INTO %s (RecordID, Lid, Artikel, AantalBesteld, Ordernr, IngevoerdDoor) VALUES (%d, %d, %d, 1, %d, %d);", $this->table, $nrid, $p_lidid, $this->artid, $ordnr, $_SESSION['lidid']);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Orderregel %d met artikel '%s' is toegevoegd.", $nrid, $this->codeartikel);
			$this->log($nrid, 1);
			$this->vulprijsperstuk();
			return $nrid;
		}
	}  # cls_Orderregel->add

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
				$this->mess = sprintf("Orderregel %d met artikel '%s' is verwijderd.", $p_orid, $this->i_art->code);
			} else {
				$this->mess = sprintf("De bestelling van artikel '%s' is verwijderd.", $this->i_art->code);
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
			$query = sprintf("UPDATE %s SET Ordernr=%d, BestellingDefinitief=SYSDATE() WHERE RecordID=%d;", $this->table, $non, $row->RecordID);
			if ($this->execsql($query) > 0) {
				$this->mess = sprintf("Orderregel %d is op order %d geplaatst.", $row->RecordID, $non);
				$this->tas = 30;
				$this->log($row->RecordID);
			}
		}
	}
	
	public function controle() {
		
	}
	
	public function opschonen() {
		
//		$query = sprintf("DELETE FROM %s WHERE AantalBesteld=0 AND AantalGeleverd=0 AND Ingevoerd <= DATE_ADD(CURDATE(), INTERVAL -1 WEEK);", $this->table);
//		$this->execsql($query, 2);

		$this->optimize();

	}  # opschonen

}  # cls_Orderregel

class cls_Voorraadboeking extends cls_db_base {
	
	private $vbid = 0;
	private $artid = 0;
	
	function __construct() {
		parent::__construct();
		$this->table = TABLE_PREFIX . "WS_Voorraadboeking";
		$this->basefrom = $this->table . " AS VB";
		$this->per = date("Y-m-d");
		$this->ta = 10;
	}
	
	private function vulvars($p_vbid=-1) {
		if ($p_vbid >= 0) {
			$this->vbid = $p_vbid;
		}
		
		if ($this->vbid > 0) {
			
		}
	}
	
	public function lijst($p_art=0) {
		$query = sprintf("SELECT VB.*, Ord.Ordernr, %s AS NaamLid FROM %s LEFT OUTER JOIN (%3\$sWS_Orderregel AS Ord LEFT OUTER JOIN %3\$sLid AS L ON L.RecordID=Ord.Lid) ON Ord.RecordID=VB.OrderregelID", $this->selectnaam, $this->basefrom, TABLE_PREFIX);
		if ($p_art > 0) {
			$query .= sprintf(" WHERE VB.ArtikelID=%d", $p_art);
		}
		$query .= " ORDER BY IFNULL(VB.Datum, CURDATE()), VB.RecordID;";
		$result = $this->execsql($query);
		return $result->fetchAll();
		
	}  # cls_Voorraadboeking->lijst
	
	public function add($p_art, $p_orid=0) {
		$this->tas = 21;
		$nrid = $this->nieuwrecordid();
		
		$query = sprintf("INSERT INTO %s (RecordID, ArtikelID, OrderregelID, IngevoerdDoor) VALUES (%d, %d, %d, %d);", $this->table, $nrid, $p_art, $p_orid, $_SESSION['lidid']);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Voorraadboeking %d voor artikel %d is toegevoegd.", $nrid, $p_art);
			$this->log($nrid);
			return $nrid;
		} else {
			return false;
		}
	}  # cls_Voorraadboeking->add
	
	public function update($p_vbid, $p_kolom, $p_waarde) {
		$this->vulvars($p_vbid);
		$this->tas = 22;
		
		if ($this->pdoupdate($this->vbid, $p_kolom, $p_waarde) > 0) {
			$this->log($this->vbid);
		}
	}
	
	public function delete($p_vbid, $p_reden="") {
		$this->vulvars($p_vbid);
		$this->tas = 23;
		
		if ($this->pdodelete($this->vbid, $p_reden)) {
			$this->log($this->vbid);
		}
	}
	
	public function controle() {
	}
	
	public function opschonen() {
		$this->optimize();
	}  # cls_Voorraadboeking->opschonen
	
}  # cls_Voorraadboeking

class cls_Rekening extends cls_db_base {	
	public $rkid = 0;
	private $reksel = "";
	public $debnaam = "";
	public $datum="";
	public $omschrijving = "";
	public $seizoen = 0;
	public $bankrekening = "";
	public $machtiging = 0;
	public $betaalddoor = 0;
	public $betaalddoornaam = "";
	public $betaalddooremail = "";
	public $bedrag = 0;
	public $betaald = 0;
	public $aantalbetaaltermijnen = 1;
	public $dagenperbetaaltermijn = 30;
	public $uiterstebetaaldatum = "";
	public $einde_eerstetermijn = "";
	
	public $adres= "";
	public $huisnr = "";
	public $postcode = "";
	public $woonplaats = "";
	public $lidnr = 0;
	
	public object $i_lid;
	public object $i_sz;
	
	function __construct($p_rkid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Rekening";
		$this->basefrom = $this->table . " AS RK";
		$this->pkkol = "Nummer";
		$this->per = date("Y-m-d");
		$this->vulvars($p_rkid);
		$this->ta = 14;
	}
	
	public function vulvars($p_rkid=-1) {
		global $dtfmt;
		
		if ($p_rkid >= 0) {
			$this->rkid = $p_rkid;
		}
		
		$this->seizoen = 0;
		$this->lidid = 0;
		$this->debnaam = "";
		$this->bedrag = 0;
		$this->betaald = 0;
		
		$dtfmt->setPattern(DTTEXT);
		
		$this->reksel = sprintf("RK.*, (RK.Bedrag-RK.Betaald) AS Openstaand, %s AS NaamLid, L.Adres, L.Postcode, L.Woonplaats, L.`Machtiging afgegeven` AS Machtiging, L.Bankrekening AS Bankrekeningnummer, (%s) AS Lidnr,
								 L.RecordID AS LidID,
								 (SELECT COUNT(DISTINCT(RR.Lid)) FROM %3\$sRekreg AS RR WHERE RR.Rekening=RK.Nummer AND RR.Lid > 0) AS AantLid,
								 (SELECT IFNULL(MIN(RR.Lid), 0) FROM %3\$sRekreg AS RR WHERE RR.Rekening=RK.Nummer AND RR.Lid > 0) AS EersteLid,
								 DATE_ADD(RK.Datum, INTERVAL (RK.BETAALDAG*RK.BET_TERM) DAY) AS UitersteBetaaldatum, DATE_ADD(RK.Datum, INTERVAL RK.BETAALDAG DAY) AS EindeEersteTermijn,
								 RK.DEBNAAM AS Tenaamstelling", $this->selectnaam, $this->selectlidnr, TABLE_PREFIX);
		
		if ($this->rkid > 0) {
			$query = sprintf("SELECT RK.* FROM %s WHERE RK.Nummer=%d;", $this->basefrom, $this->rkid);
			$rkrow = $this->execsql($query)->fetch();
			if (isset($rkrow->Nummer) and $rkrow->Nummer > 0) {
				$this->lidid = $rkrow->Lid;
				$this->datum = $rkrow->Datum ?? "";
				$this->omschrijving = $rkrow->OMSCHRIJV ?? "";
				$this->seizoen = $rkrow->Seizoen ?? 0;
				$this->debnaam = $rkrow->DEBNAAM ?? "";
				$this->betaalddoor = $rkrow->BetaaldDoor ?? 0;
				if ($this->betaalddoor == 0) {
					$this->betaalddoor = $this->lidid;
				}
				$this->aantalbetaaltermijnen = $rkrow->BET_TERM;
				$this->dagenperbetaaltermijn = $rkrow->BETAALDAG;
				$query = sprintf("SELECT IFNULL(SUM(RR.Bedrag), 0) FROM %sRekreg AS RR WHERE RR.Rekening=%d;", TABLE_PREFIX, $this->rkid);
				$this->bedrag = round($this->scalar($query), 2);
				$query = sprintf("SELECT IFNULL(SUM(RB.Bedrag), 0) FROM %sRekeningBetaling AS RB WHERE RB.Rekening=%d;", TABLE_PREFIX, $this->rkid);
				$this->betaald = round($this->scalar($query), 2);
				
				$ubd = new datetime($rkrow->Datum);
				$ubd->modify(sprintf("+%d day", $rkrow->BET_TERM * $rkrow->BETAALDAG));
				$this->uiterstebetaaldatum = $dtfmt->format($ubd);
				
				$eet = new datetime($rkrow->Datum);
				$eet->modify(sprintf("+%d day", $rkrow->BETAALDAG));
				$this->einde_eerstetermijn = $dtfmt->format($eet);
				
			} else {
				$this->rkid = 0;
			}
			$this->naamlogging = $this->debnaam;
		}
		
		$i_lid = new cls_Lid($this->lidid);
		$this->adres = $i_lid->adres;
		$this->postcode = $i_lid->postcode;
		$this->woonplaats = $i_lid->woonplaats;
		$this->lidnr = $i_lid->lidnr;
		
		$i_lid->vulvars($this->betaalddoor);
		$this->betaalddoornaam = $i_lid->naam();
		$this->betaalddooremail = $i_lid->email();
		$this->bankrekening = $i_lid->bankrekening;
		$this->machtiging = $i_lid->machtigingafgegeven;
		
		$l_lid = null;
		
		$this->i_lid = new cls_Lid($this->lidid);
		$this->i_sz = new cls_Seizoen($this->seizoen);
	}  # cls_Rekening->vulvars
	
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
		
		$this->query = sprintf("SELECT RK.Nummer, RK.Datum, CONCAT(RK.OMSCHRIJV, IF(LENGTH(IFNULL(RK.OpmerkingIntern, '')) > 0, ' *', '')) AS Omschrijving,
								RK.DEBNAAM, RK.Bedrag, RK.Betaald, CONCAT(RK.DEBNAAM, IF(RK.Lid <> RK.BetaaldDoor AND RK.BetaaldDoor > 0, ' *', '')) AS Tenaamstelling, RK.OpmerkingIntern,
								IF((SELECT COUNT(*) FROM %2\$sRekreg AS RR WHERE RR.Rekening=RK.Nummer)=0, RK.Nummer, 0) AS linkDelete
								FROM %1\$s LEFT OUTER JOIN %2\$sLid AS L ON L.RecordID=RK.Lid %3\$s ORDER BY RK.Nummer;", $this->basefrom, TABLE_PREFIX, $w);
		
		$result = $this->execsql();
		$rv = $result->fetchAll();
		
		return $rv;
	}  # cls_Rekening->overzichtbeheer
	
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
	
	public function nieuwrekeningnr($p_seiz=-1, $p_minrek=-1) {
		
		if ($p_seiz >= 0) {
			$this->seizoen = $p_seiz;
		}
		
		$nwreknr = $this->max("Nummer") + 1;
		if ($this->seizoen > 2000 and $nwreknr < ($this->seizoen*1000)) {
			$nwreknr = ($this->seizoen*1000) + 1;
		}
		if ($p_minrek > 0 and $nwreknr < $p_minrek) {
			$nwreknr = $p_minrek;
		}
		
		return $nwreknr;
	}  # cls_Rekening->nieuwrekeningnr
	
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
			

		} elseif ($this->seizoen <= 0) {
			$this->mess = "Er is geen seizoen gespecificeerd. De rekening wordt niet toegevoegd.";
			$this->log(0, 1);
			return false;
			
		} else {
			
			if ($p_rkid > 0)  {
				$nwreknr = $p_rkid;
				while ($this->aantal(sprintf("RK.Nummer=%d", $nwreknr)) > 0) {
					$nwreknr++;
				}
			} else {
				$nwreknr = $this->nieuwrekeningnr();
			}

			$query = sprintf("INSERT INTO %1\$s (%2\$s, Seizoen, Datum, Lid, BetaaldDoor, BETAALDAG, BET_TERM, Betaald, Ingevoerd) VALUES (%3\$d, %4\$d, CURDATE(), %5\$d, %6\$d, 30, 1, 0, CURDATE());", $this->table, $this->pkkol, $nwreknr, $p_seiz, $this->lidid, $i_lid->rekeningbetaalddoor);
			if ($this->execsql($query) > 0) {
				$this->mess = sprintf("Rekening %d voor lid %d is toegevoegd.", $nwreknr, $this->lidid);
				$this->log($nwreknr);
			
				$sql = sprintf("INSERT INTO Rekening (%1\$s, Seizoen, Datum, Lid, BetaaldDoor, BETAALDAG, BET_TERM, Betaald, Ingevoerd) VALUES (%2\$d, %3\$d, CURDATE(), %4\$d, %5\$d, 30, 1, 0, CURDATE());", $this->pkkol, $nwreknr, $p_seiz, $this->lidid, $i_lid->rekeningbetaalddoor);
				$this->Interface($sql);
			
				$this->update($nwreknr, "DEBNAAM", $i_lid->Naam());
			
				$this->update($nwreknr, "OMSCHRIJV", $i_seiz->rekeningomschrijving);
				$this->update($nwreknr, "BETAALDAG", $i_seiz->betaaldagentermijn);
			
				return $nwreknr;
			} else {
				return false;
			}
		}
	}  # cls_Rekening->add
	
	public function update($p_rkid, $p_kolom, $p_waarde) {
		$this->vulvars($p_rkid);
		$this->tas = 2;
		
		if ($p_kolom == "Datum" and strlen($p_waarde) == 0) {
			$this->mess = "De datum is een verplicht veld en mag niet leeg zijn, deze wijziging wordt niet doorgevoerd.";
		} elseif ($p_kolom == "Datum" and strtotime($p_waarde) === false) {
			$this->mess = sprintf("%s is geen geldige datum, deze wijziging wordt niet doorgevoerd", $p_waarde);
		} elseif ($p_kolom == "BETAALDAG" and is_numeric($p_waarde) === false) {
			$this->mess = sprintf("%s is geen geldige waarde voor betaaldagen, deze wijziging wordt niet doorgevoerd", $p_waarde);
		} elseif (toegang("Rekeningen/Muteren")) {
			$this->pdoupdate($this->rkid, $p_kolom, $p_waarde);
		} else {
			$this->mess = "Je bent niet bevoegd om rekeningen aan te passen.";
		}
		$this->log($this->rkid);
		
	}  # cls_Rekening->update
	
	public function delete($p_rkid, $p_inclregels=0, $p_reden="") {
		$this->tas = 3;
		$this->vulvars($p_rkid);
		
		if ($p_inclregels == 1) {
			$query = sprintf("DELETE FROM %sRekreg WHERE Rekening=%d;", TABLE_PREFIX, $this->rkid);
			$this->execsql($query);
			
			if ($_SESSION['settings']['interface_access_db'] == 1) {
				$sql = sprintf("DELETE FROM Rekreg WHERE Rekening=%d;", $this->rkid);
				$this->interface($sql);
			}
		}
		
		$query = sprintf("SELECT COUNT(*) FROM %sRekreg AS RR WHERE RR.Rekening=%d;", TABLE_PREFIX, $this->rkid);
		if ($p_inclregels == 0 and $this->scalar($query) > 0) {
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
		
		$rkrows = $this->basislijst($f, "RK.Gewijzigd DESC", 1, 750);
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
				$this->mess = sprintf("De controle van de rekeningen van seizoen %d is in %.1f seconden uitgevoerd.", $p_seizoen, (microtime(true) - $starttijd));
			} else {
				$this->mess = sprintf("De controle van %d rekeningen is in %.1f seconden uitgevoerd.", count($rkrows), (microtime(true) - $starttijd));
			}
			$this->rkid = 0;
			$this->tas = 19;
			$this->lidid = 0;
			$this->Log();
		}
		
	}  # cls_Rekening->controle
	
	public function opschonen() {
		$this->tas = 18;
		$aantrek = 0;
		
		if ($_SESSION['settings']['rekening_bewaartermijn'] > 3) {
			$query = sprintf("SELECT RK.Nummer FROM %s WHERE RK.Datum < DATE_SUB(CURDATE(), INTERVAL %d MONTH) LIMIT 250;", $this->basefrom, $_SESSION['settings']['rekening_bewaartermijn']);
			$result = $this->execsql($query);
			$reden = sprintf("de rekening meer dan %d maanden oud is.", $_SESSION['settings']['rekening_bewaartermijn']);
			foreach ($result->fetchAll() as $rkrow) {
				$this->delete($rkrow->Nummer, 1, $reden);
				$aantrek++;
			}
		}
		
		$query = sprintf("SELECT RK.Nummer FROM %s WHERE RK.Ingevoerd < DATE_SUB(NOW(), INTERVAL 6 HOUR) AND LENGTH(IFNULL(RK.OpmerkingIntern, '')) = 0 AND (RK.Nummer NOT IN (SELECT RR.Rekening FROM %sRekreg AS RR)) LIMIT 100;", $this->basefrom, TABLE_PREFIX);
		$result = $this->execsql($query);
		$reden = "de rekening geen regels en opmerking (meer) heeft.";
		foreach ($result->fetchAll() as $rkrow) {
			$this->delete($rkrow->Nummer, 0, $reden);
			$aantrek++;
		}
		
		if ($aantrek > 1) {
			$this->lidid = 0;
			$this->mess = sprintf("In totaal %d rekeningen verwijderd.", $aantrek);
			$this->Log(0, 1);
		}
		
		$this->optimize();
		
	}  # cls_Rekening->opschonen
	
}  # cls_Rekening

class cls_Rekeningregel extends cls_db_base {
	
	public int $rrid = 0;
	public int $rkid = 0;
	private int $regelnr = 0;
	public float $bedrag = 0;
	public int $lidondid = 0;
	public int $activiteitid = 0;
	
	public object $i_rk;
	
	function __construct($p_rkid=-1, $p_rrid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Rekreg";
		$this->basefrom = $this->table . sprintf(" AS RR INNER JOIN %sRekening AS RK ON RR.Rekening=RK.Nummer", TABLE_PREFIX);
		$this->vulvars($p_rrid, $p_rkid);
		$this->ta = 14;
	}
	
	private function vulvars($p_rrid=-1, $p_rkid=-1) {
		
		if ($p_rrid >= 0) {
			$this->rrid = $p_rrid;
		}
		
		if ($p_rkid >= 0) {
			$this->rkid = $p_rkid;
		}
		
		if ($this->rrid > 0) {
			$query = sprintf("SELECT RR.* FROM %s WHERE RR.RecordID=%d;", $this->basefrom, $this->rrid);
			$result = $this->execsql($query);
			$row = $result->fetch();
			if (isset($row->RecordID)) {
				$this->rkid = $row->Rekening ?? 0;
				$this->lidid = $row->Lid ?? 0;
				$this->regelnr = $row->Regelnr ?? 0;
				$this->bedrag = $row->Bedrag ?? 0;
				$this->lidondid = $row->LidondID ?? 0;
				$this->activiteitid = $row->ActiviteitID ?? 0;
				$this->naamlogging = sprintf("%d/%d", $this->rkid, $this->regelnr);
			} else {
				$this->rrid = 0;
			}
		}
		
		$this->i_rk = new cls_Rekening($this->rkid);
		
	}  # cls_Rekeningregel->vulvars
	
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
		
	}  # cls_Rekeningregel->record
	
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

	public function toevoegenstandaardregels($p_rkid, $p_lidid, $p_uitvoeren=1) {
		$this->lidid = $p_lidid;
		
		$this->vulvars(0, $p_rkid);
		
		$i_lo = new cls_Lidond(-1, $this->lidid);
		$i_lid = new cls_Lid($this->lidid, $this->i_rk->datum);
		$i_lm = new cls_Lidmaatschap(-1, $this->lidid);
		
		$jl = false;
		if ($i_lid->geboortedatum > $this->i_rk->i_sz->peildatumjeugdlid) {
			$jl = true;
		}
		
		$aantToegevoegd = 0;
		
		$oms = "";
		$cb = 0;
				
		if ($i_lid->islid or $i_lm->lidvanaf >= $this->i_rk->datum) {
		
			$query = sprintf("SELECT COUNT(*) FROM %s WHERE RK.Seizoen=%d AND RR.KSTNPLTS='%s' AND RR.Lid=%d;", $this->basefrom, $this->i_rk->seizoen, $this->i_rk->i_sz->verenigingscontributiekostenplaats, $this->lidid);
			if ($this->scalar($query) == 0) {
				$oms = $this->i_rk->i_sz->verenigingscontributieomschrijving;
				$onderscheiding = $i_lid->onderscheiding($this->lidid);
				if (strlen($onderscheiding) > 0) {
					$cb = 0;
					$oms .= " " . $onderscheiding;
				} elseif ($i_lid->iskader and $this->i_rk->i_sz->contributiekader != $this->i_rk->i_sz->contributielid) {
					$cb = $this->i_rk->i_sz->contributiekader;
					$oms .= " kader";
				} elseif ($jl and $this->i_rk->i_sz->contributiejeugdlid != $this->i_rk->i_sz->contributielid) {
					$cb = $this->i_rk->i_sz->contributiejeugdlid;
					$oms .= " jeugdlid";
				} else {
					$cb = $this->i_rk->i_sz->contributielid;
				}

				if ($i_lm->lidvanaf > $this->i_rk->i_sz->begindatum) {
					$oms .= " vanaf " . date("d-m-Y", strtotime($i_lm->lidvanaf));
				}

				if ($p_uitvoeren == 1) {
					$rrid = $this->add($p_rkid, $this->lidid, $this->i_rk->i_sz->verenigingscontributiekostenplaats);
					if ($rrid > 0) {
						$this->update($rrid, "OMSCHRIJV", $oms);
						$this->update($rrid, "Bedrag", $cb);
						if (function_exists("fnMaatwerkNaToevoegenStandaardRegelOpRekening")) {
							fnMaatwerkNaToevoegenStandaardRegelOpRekening($rrid);
						}
					}
				}
				$aantToegevoegd++;
			}

			foreach ($i_lo->lijstperlid($this->lidid, "A", $this->i_rk->datum, 1) as $lorow) {
				$cb = 0;
				$query = sprintf("SELECT COUNT(*) FROM %s WHERE RK.Seizoen=%d AND RR.LidondID=%d;", $this->basefrom, $this->i_rk->seizoen, $lorow->RecordID);
				if ($this->scalar($query) == 0) {
					if ($this->i_rk->i_sz->afdelingscontributieomschrijving == 2 and isset($lorow->GrActiviteit) and strlen($lorow->GrActiviteit) > 0) {
						$oms = $lorow->GrActiviteit;
					} else {
						$oms = $lorow->OndNaam;
					}
					$kpl = $lorow->OndCode;

					if (isset($lorow->GrActiviteit) and strlen($lorow->GrActiviteit) > 0 and isset($lorow->GrContributie) and $lorow->GrContributie > 0) {
						if ($this->i_rk->i_sz->afdelingscontributieomschrijving == 3) {
							$oms .= " (" . $lorow->GrActiviteit . ")";
						}
						if (strlen($lorow->ActGBR) > 0) {
							$kpl = $lorow->ActGBR;
						} else {
							$kpl .= "-" . $lorow->ActCode;
						}
						$cb = $lorow->GrContributie;
					}

					if ($lorow->Kader == 1) {
						$cb = $lorow->FUNCTCB;
					} elseif ($jl) {
						$cb += $lorow->JEUGDCB;
					} else {
						$cb += $lorow->LIDCB;
					}
					if ($lorow->Functie > 0 or $cb <> 0) {
						if (strlen($lorow->FunctieOms) > 0) {
							$oms .= " (" . $lorow->FunctieOms . ")";
						}

						if ($lorow->Vanaf > $this->i_rk->i_sz->begindatum) {
							$oms .= " vanaf " . date("d-m-Y", strtotime($lorow->Vanaf));
						}

						if ($p_uitvoeren == 1) {
							$rrid = $this->add($p_rkid, $lorow->Lid, $kpl);
							if ($rrid > 0) {
								$this->update($rrid, "LidondID", $lorow->RecordID);
								$this->update($rrid, "ActiviteitID", $lorow->ActiviteitID);
								$this->update($rrid, "OMSCHRIJV", $oms);
								$this->update($rrid, "Bedrag", $cb);
								if (function_exists("fnMaatwerkNaToevoegenStandaardRegelOpRekening")) {
									fnMaatwerkNaToevoegenStandaardRegelOpRekening($rrid);
								}
							}
						}
						$aantToegevoegd++;
					}
				}

			}
		}
		$f = sprintf("RR.Rekening=%d", $p_rkid);
		$tb = $this->totaal("RR.Bedrag", $f);
		(new cls_Rekening())->update($p_rkid, "Bedrag", $tb);

		$i_lo = null;
		$i_lid = null;
		$i_lm = null;
		
		return $aantToegevoegd;
	}  # cls_Rekeningregel->toevoegenstandaardregels
	
	public function totaalrekening($p_rkid) {
		$query = sprintf("SELECT IFNULL(SUM(RR.Bedrag), 0) FROM %s WHERE RR.Rekening=%d;", $this->basefrom, $p_rkid);
		return round($this->scalar($query), 2);
	}  # cls_Rekeningregel->totaalrekening
	
	public function add($p_rkid, $p_lidid=-1, $p_kpl="") {
		$this->vulvars(0, $p_rkid, $p_lidid);
		$this->tas = 11;
		
		$nrid = $this->nieuwrecordid();
		$rnr = $this->max("Regelnr", sprintf("Rekening=%d", $this->rkid)) + 1;
		
		if ($rnr < 1) {
			$this->mess = "Het regelnummer mag niet kleiner dan 1 zijn, de regel wordt niet toegevoegd.";
			$this->log();
		} elseif ($rnr > 99) {
			$this->mess = "Het regelnummer mag niet groter dan 99 zijn, de regel wordt niet toegevoegd.";
			$this->log();
		} elseif ($this->rkid < 1) {
			$this->mess = "Het rekeningnummer mag niet kleiner dan 1 zijn, de regel wordt niet toegevoegd.";
			$this->log();
		} elseif (toegang("Rekeningen/Muteren", 0, 0)) {
			$query = sprintf("INSERT INTO %s (RecordID, Rekening, Lid, Regelnr, KSTNPLTS, ToonOpRekening, LidondID, ActiviteitID) VALUES (%d, %d, %d, %d, '%s', 1, 0, 0);", $this->table, $nrid, $this->rkid, $this->lidid, $rnr, $p_kpl);
			if ($this->execsql($query) > 0) {
				$this->mess = sprintf("Regel %d aan rekening %d toegevoegd", $rnr, $this->rkid);
				$this->log($nrid);
				$this->Interface($query);
			} else {
				$nrid = 0;
			}
		} else {
			$this->mess = "Je bent niet bevoegd om rekeningregels te muteren.";
			$this->log();
		}
		
		return $nrid;
		
	}  # cls_Rekeningregel->add
	
	public function update($p_rrid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_rrid);
		$this->tas = 12;
		
		if (toegang("Rekeningen/Muteren", 0, 0)) {
			$this->pdoupdate($this->rrid, $p_kolom, $p_waarde, $p_reden);
		} else {
			$this->mess = "Je bent niet bevoegd om rekeningregels te muteren.";
		}
		
		$this->log($this->rrid);
	}  # cls_Rekeningregel->update
	
	public function delete($p_rrid, $p_reden="") {
		$this->vulvars($p_rrid);
		$this->tas = 13;
		
		if (toegang("Rekeningen/Muteren", 0, 0)) {
			$this->pdodelete($this->rrid, $p_reden);
		} else {
			$this->mess = "Je bent niet bevoegd om rekeningregels te verwijderen.";
		}
		$this->log($this->rrid);
	}  # cls_Rekeningregel->delete
	
	public function controle() {
	}  # cls_Rekeningregel->controle
	
	public function opschonen() {
		$query = sprintf("DELETE FROM %s WHERE Rekening=0;", $this->table, TABLE_PREFIX);
		$this->execsql($query, 2);
		
		$query = sprintf("SELECT RR.RecordID FROM %s AS RR WHERE RR.Rekening NOT IN (SELECT RK.Nummer FROM %sRekening AS RK) LIMIT 250;", $this->table, TABLE_PREFIX);
		$result = $this->execsql($query);
		foreach ($result->fetchAll() as $row) {
			$this->delete($row->RecordID, "de bijbehorende rekening niet (meer) bestaat.");
		}
		
		$this->optimize();
		
	}  #  cls_Rekeningregel->opschonen
	
}  # cls_Rekeningregel

class cls_RekeningBetaling extends cls_db_base {
	
	private $rbid = 0;
	private $rkid = 0;
	
	public object $i_rk;
	
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
		
		$this->i_rk = new cls_Rekening($this->rkid);
	}
	
	public function laatstebetalingen($p_aantal=50) {
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
		$this->optimize();
	}
	
}  #  cls_RekeningBetaling

class cls_Seizoen extends cls_db_base {
	
	public $szid = 0;
	public $begindatum = "1900-01-01";
	public $einddatum = "9999-12-31";
	public $peildatumjeugdlid = "1900-01-01";
	public $rekeningomschrijving = "";
	public $verenigingscontributieomschrijving = "";
	public $verenigingscontributiekostenplaats = "";
	public $afdelingscontributieomschrijving = 1;
	public $contributiekader = 0;
	public $contributielid = 0;
	public $contributiejeugdlid = 0;
	public $betaaldagentermijn = 30;
	
	function __construct($p_szid=-1) {
		$this->table = TABLE_PREFIX . "Seizoen";
		$this->basefrom = $this->table . " AS SZ";
		$this->pkkol = "Nummer";
		$this->per = date("Y-m-d");
		$this->vulvars($p_szid);
		$this->ta = 20;
	}
	
	private function vulvars($p_szid=-1) {
		if ($p_szid >= 0) {
			$this->szid = $p_szid;
		}
		if ($this->szid > 0) {
			$query = sprintf("SELECT SZ.*, DATE_SUB(SZ.Begindatum, INTERVAL SZ.`Leeftijdsgrens jeugdleden` YEAR) AS PDJL FROM %s WHERE SZ.Nummer=%d;", $this->basefrom, $this->szid);
			$result = $this->execsql($query);
			$szrow = $result->fetch();
			if (isset($szrow->Nummer)) {
				$this->begindatum = $szrow->Begindatum;
				$this->einddatum = $szrow->Einddatum;
				$this->peildatumjeugdlid = $szrow->PDJL ?? "1900-01-01";
				$this->rekeningomschrijving = $szrow->Rekeningomschrijving ?? "Contributie";
				$this->verenigingscontributieomschrijving = $szrow->{'Verenigingscontributie omschrijving'} ?? "Verenigingscontributie";
				$this->verenigingscontributiekostenplaats = $szrow->{'Verenigingscontributie kostenplaats'} ?? "";
				$this->afdelingscontributieomschrijving = $szrow->{'Afdelingscontributie omschrijving'} ?? 1;
				$this->contributiekader = $szrow->{'Contributie kader'} ?? 0;
				$this->contributielid = $szrow->{'Contributie leden'} ?? 0;
				$this->contributiejeugdlid = $szrow->{'Contributie jeugdleden'} ?? 0;
				$this->betaaldagentermijn = $szrow->BetaaldagenTermijn ?? 30;
			} else {
				$this->szid = 0;
			}
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
			$rv .= sprintf("<option value=%d%s>%s: %s t/m %s</option>\n", $row->Nummer, $s, $row->Nummer, $dtfmt->format(strtotime($row->Begindatum)), $dtfmt->format(strtotime($row->Einddatum)));
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
	
	public function zethuidige($p_per="", $p_szid=0) {
		
		if (strlen($p_per) == 10) {
			$query = sprintf("SELECT SZ.Nummer FROM %1\$s WHERE SZ.Begindatum <= '%2\$s' AND SZ.Einddatum >= '%2\$s';", $this->basefrom, $p_per);
		} elseif ($p_szid > 0) {
			$query = sprintf("SELECT IFNULL(SZ.Nummer, 0) FROM %s WHERE SZ.Nummer=%d;", $this->basefrom, $p_szid);
		} else {
			$query = sprintf("SELECT SZ.Nummer FROM %1\$s WHERE SZ.Begindatum <= '%2\$s' AND SZ.Einddatum >= '%2\$s';", $this->basefrom, $this->per);
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
		
		$query = sprintf("INSERT INTO %s (Nummer, Begindatum, Einddatum, BetaaldagenTermijn) VALUES (%d, '%s', '%s', 30);", $this->table, $nnr, $bd->format("Y-m-d"), $ed->format("Y-m-d"));
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Seizoen %d is toegevoegd.", $nnr);
			$this->Log($nnr);
			$this->interface($query);
		}
	}
	
	public function update($p_szid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_szid);
		$this->tas = 22;
				
		if ($this->pdoupdate($this->szid, $p_kolom, $p_waarde, $p_reden)) {
			$this->Log($this->szid);
		}
	}

	public function delete($p_seiznr, $p_reden="") {
		$this->tas = 23;
		$this->pdodelete($p_seiznr, $p_reden);
		$this->log($p_seiznr);
	}
	
	public function controle() {
		$aa = (new cls_Groep())->aantal("ActiviteitID > 0");
		
		foreach($this->basislijst() as $row) {
			if ($row->{'Afdelingscontributie omschrijving'} > 1 and $aa == 0) {
				$this->update($row->Nummer, "Afdelingscontributie omschrijving", 1, "er geen groepen met een activiteit zijn.");
			}
		}
	}
	
	public function opschonen() {
		
		$this->optimize();
		
	}  # opschonen
	
}  # cls_Seizoen

class cls_Stukken extends cls_db_base {
	public int $stid = 0;
	public string $title = "";
	public int $zichtbaarvoor = -1;
	public string $link = "";
	public string $url = "";
	public $magdownload = false;
	private string $folder = "";
	private $intern = true;
	
	function __construct($p_stid=-1, $p_stuknaam="") {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Stukken";
		$this->basefrom = $this->table . " AS S";
		$this->ta = 22;
		$this->vulvars($p_stid, $p_stuknaam);
	}
	
	public function vulvars($p_stid=-1, $p_stuknaam="") {
		if ($p_stid >= 0) {
			$this->stid = $p_stid;
		} elseif ($p_stid == -1 and strlen($p_stuknaam) > 0) {
			$query = sprintf("SELECT IFNULL(MIN(S.RecordID), 0) FROM %s WHERE S.Link='%s';", $this->basefrom, $p_stuknaam);
			$this->stid = $this->scalar($query);
		}
		$this->folder = BASEDIR . "/stukken/";
		$this->magdownload = false;
		
		if ($this->stid > 0) {
			$this->query = sprintf("SELECT S.* FROM %s WHERE S.RecordID=%d;", $this->basefrom, $this->stid);
			$row = $this->execsql()->fetch();
			if (isset($row->RecordID)) {
				$this->titel = $row->Titel;
				$this->naamlogging = $row->Titel;
				$this->zichtbaarvoor = $row->ZichtbaarVoor;
				$this->link = $row->Link;
				if ($this->zichtbaarvoor == 0 or WEBMASTER == 1) {
					$this->magdownload = true;
				} else {
					if (in_array($this->zichtbaarvoor, explode(",", $_SESSION['lidgroepen']))) {
						$this->magdownload = true;
					}
				}
			} else {
				$this->stid = 0;
			}
		}
		
		$this->url = "";
		if ($this->magdownload and $this->stid > 0) {
			if (substr($this->link, 0, 4) == "http") {
				$this->url = $this->link;
				$this->intern = false;
			} elseif (file_exists($this->folder . $this->link)) {
				$this->url = BASISURL . sprintf("/get_stuk.php?p_stukid=%d", $this->stid);
				$this->intern = true;
			}
		}
	}

	public function record($p_stid) {
		$this->vulvars($p_stid);
		
		$this->query = sprintf("SELECT S.* FROM %s WHERE S.RecordID=%d;", $this->basefrom, $this->stid);
		return $this->execsql()->fetch();
	}
	
	public function lijst() {
		$query = sprintf("SELECT S.*, IF(Ingangsdatum > DATE_SUB(CURDATE(), INTERVAL 1 MONTH), 'Gewijzigd',
				IF(S.Gewijzigd > DATE_SUB(CURDATE(), INTERVAL 1 MONTH), 'Gewijzigd', IF(Revisiedatum < CURDATE(), 'Overdue', ''))) AS Status,
				IF (S.ZichtbaarVoor > 0, (SELECT MAX(O.Naam) FROM %sOnderdl AS O WHERE O.RecordID=S.ZichtbaarVoor), 'Iedereen') AS Zichtbaar
				FROM %s
				WHERE IFNULL(S.VervallenPer, CURDATE()) >= CURDATE()", TABLE_PREFIX, $this->basefrom);
		if (WEBMASTER == false) {
			$query .= sprintf(" AND S.ZichtbaarVoor IN (%s)", $_SESSION['lidgroepen']);
		}
		$query .= " ORDER BY S.Type, S.Titel;";
		
		$result = $this->execsql($query);
		return $result->fetchAll();
		
	}
	
	public function editlijst() {
		
		$query = sprintf("SELECT S.RecordID, S.Titel, S.`Type`, BestemdVoor, VastgesteldOp, Revisiedatum, VervallenPer FROM %s ORDER BY S.VervallenPer, S.Titel;", $this->basefrom);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}
	
	public function gewijzigdestukken($p_vanaf="") {
		if (strlen($p_vanaf) < 10) {
			$p_vanaf = date("Y-m-d H:i:s", strtotime("-1 month"));
		}
		
		$vl = (new cls_Logboek())->vorigelogin(0);
		if ($vl > $p_vanaf) {
			$vl = $p_vanaf;
		}
		
		$query = sprintf("SELECT S.RecordID, S.Titel FROM %s WHERE IFNULL(S.VervallenPer, CURDATE()) >= CURDATE()", $this->basefrom);
		if (strlen($vl) >= 10) {
			$query .= sprintf(" AND (S.Ingevoerd >= '%1\$s' OR S.VastgesteldOp >= '%1\$s' OR S.Ingangsdatum >= '%1\$s')", $vl);
		}
		if (WEBMASTER == false) {
			$query .= sprintf(" AND S.ZichtbaarVoor IN (%s)", $_SESSION['lidgroepen']);
		}
		$query .= " ORDER BY S.Gewijzigd DESC;";
		
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
	
	public function update($p_stid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_stid);
		$this->tas = 2;
		
		if ($this->pdoupdate($this->stid, $p_kolom, $p_waarde, $p_reden) > 0) {
			$this->log($this->stid);
		}
	}

	public function delete($p_stid, $p_reden="") {
		$this->vulvars($p_stid);
		$this->tas = 3;
		
		$this->pdodelete($this->stid, $p_reden);
		$this->log($this->stid, 1);
	}
	
	public function controle() {
		
		foreach ($this->basislijst() as $row) {
			if (array_key_exists($row->Type, ARRTYPESTUK) == false) {
				if ($this->update($row->RecordID, "Type", "F", "type een ongeldige waarde had") > 0) {
					$this->log($row->RecordID);
				}
			}
		}
	}
	
	public function opschonen() {
		
		$this->optimize();
		
	}
	
}  # cls_Stukken

class cls_Website_menu extends cls_db_base {
	public int $wmid = 0;
	public ?string $titel = "";
	public int $vorigelaag = 0;
	public int $volgnr = 0;
	public int $inhoudid = 0;
	public ?string $externelink = "";
	public $gepubliceerd = "";
	private int $activelaag1 = -2;
	
	function __construct($p_wmid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Website_menu";
		$this->basefrom = $this->table . " AS WM";
		$this->ta = 22;
		$this->vulvars($p_wmid);
	}
	
	public function vulvars($p_wmid=-1) {
		if ($p_wmid >= 0) {
			$this->wmid = $p_wmid;
		}
		
		if ($this->wmid > 0) {
			$query = sprintf("SELECT * FROM %s WHERE WM.RecordID=%d;", $this->basefrom, $this->wmid);
			$row = $this->execsql($query)->fetch();
			if (isset($row->RecordID)) {
				$this->titel = $row->Titel;
				$this->vorigelaag = $row->VorigeLaag;
				$this->volgnr = $row->Volgnummer;
				if (strlen($row->InhoudID) > 0) {
					$this->inhoudid = $row->InhoudID;
				} else {
					$this->inhoudid = 0;
				}
				$this->externelink = $row->ExterneLink;
				$this->gepubliceerd = $row->Gepubliceerd;
				$this->naamlogging = $row->Titel;
			} else {
				$this->wmid = 0;
			}
		}
	}
	
	public function lijst($p_filter="") {
		$query = sprintf("SELECT WM.*, IF(WM.VorigeLaag > 0, (SELECT WM2.Titel FROM %s AS WM2 WHERE WM2.RecordID=WM.VorigeLaag), '') AS VorigMenu, IF(WM.InhoudID > 0, LEFT(WI.Titel, 40), LEFT(WM.ExterneLink, 40)) AS TitelInhoudKort", $this->table);
		$query .= sprintf(" FROM %s LEFT OUTER JOIN %sWebsite_inhoud AS WI ON WM.InhoudID=WI.RecordID", $this->basefrom, TABLE_PREFIX);
		if (strlen($p_filter) > 0) {
			$query .= sprintf(" WHERE %s", $p_filter);
		}
		$query .= " ORDER BY WM.VorigeLaag, WM.Volgnummer, WM.Titel;";
		$res = $this->execsql($query);
		return $res->fetchAll();
	}
	
	public function htmloptions($p_cv=-1, $p_filter="") {
		$rv = "";
		foreach ($this->lijst($p_filter) as $row) {
			$rv .= sprintf("<option value=%d%s>%s</option>\n", $row->RecordID, checked($row->RecordID, "option", $p_cv), $row->Titel);
		}
		
		return $rv;
	}
	
	public function htmlmenu($p_cv=-1, $p_ul=1) {
		$rv = "";
		$this->activelaag1 = $this->min("VorigeLaag", sprintf("WM.RecordID=%d", $p_cv));
		if ($p_ul == 1) {
			$ul_el = "<ul class='nav'>";
		} else {
			$ul_el = "";
		}
		
		$f_bas = "(WM.Gepubliceerd IS NOT NULL) AND WM.Gepubliceerd <= CURDATE() AND (IFNULL(WM.InhoudID, 0) > 0 OR LENGTH(IFNULL(WM.ExterneLink, '')) > 8) AND ";
		
		$f = $f_bas . "WM.VorigeLaag=0";
		foreach ($this->lijst($f) as $row_l1) {

			$rv .= $this->menuentry($row_l1, $p_cv, $p_ul);
			$f = sprintf("%sWM.VorigeLaag=%d", $f_bas, $row_l1->RecordID);
			$rows_l2 = $this->lijst($f);
			if (count($rows_l2) > 0) {
				$rv .= $ul_el; 
				foreach ($rows_l2 as $row_l2) {
					if ($row_l1->RecordID == $p_cv or ($row_l2->VorigeLaag == $this->activelaag1)) {
						$rv .= $this->menuentry($row_l2, $p_cv, $p_ul);
					}
				}
				if ($p_ul == 1) {
					$rv .= "</ul>\n";
				}
			}
			if ($p_ul == 1) {
				$rv .= "</li>\n";
			}
		}
		
		return $rv;
	}
	
	private function menuentry($p_row, $p_cv=-1, $p_li=1) {
		$rv = "";
		if ($p_cv == $p_row->RecordID) {
			$a_cl = "nav-link active";
		} else {
			$a_cl = "nav-link";
		}
		if ($p_li == 1) {
			$li_el = "<li class='nav-item'>";
		} else {
			$li_el = "";
		}

		if ($p_row->InhoudID > 0) {
			$rv .= sprintf("%s<a class='%s' href='%s?p_menu=%d'>%s</a>", $li_el, $a_cl, $_SERVER['PHP_SELF'], $p_row->RecordID, $p_row->Titel);
		} elseif (strlen($p_row->ExterneLink) > 8) {
			$rv .= sprintf("%s<a class='%s' href='%s'>%s</a>", $li_el, $a_cl, $p_row->ExterneLink, $p_row->Titel);
		} else {
			$rv .= sprintf("%s%s", $li_el, $p_row->Titel);
		}
		return $rv;
	}
	
	public function add() {
		$nrid = $this->nieuwrecordid();
		$this->tas = 11;
		
		$query = sprintf("INSERT INTO %s (RecordID, Titel) VALUES (%d, '*** Nieuw ***');", $this->table, $nrid);
		$this->execsql($query);
		$this->mess = sprintf("Website_menu: Record %d is toegevoegd.", $nrid);
		
		$this->log($nrid);
	}

	public function update($p_wmid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_wmid);
		$this->tas = 12;
		
		if ($this->pdoupdate($p_wmid, $p_kolom, $p_waarde) > 0) {
			$this->log($p_wmid);
			$this->vulvars();
		}
	}
	
}  # cls_Website_menu

class cls_Website_inhoud extends cls_db_base {
	public int $wiid = 0;
	public ?string $titel = "";
	public ?string $tekst = "";
	public int $htmldirect = 0;
	public ?string $inlinestyle = "";
	
	function __construct($p_wiid=-1) {
		parent::__construct();
		$this->table = TABLE_PREFIX . "Website_inhoud";
		$this->basefrom = $this->table . " AS WI";
		$this->ta = 22;
		$this->vulvars($p_wiid);
	}
	
	public function vulvars($p_wiid=-1) {
		if ($p_wiid >= 0) {
			$this->wiid = $p_wiid;
		}
		
		if ($this->wiid > 0) {
			$query = sprintf("SELECT WI.* FROM %s WHERE WI.RecordID=%d;", $this->basefrom, $this->wiid);
			$row = $this->execsql($query)->fetch();
			if (isset($row->RecordID)) {
				$this->titel = str_replace("\"", "'", $row->Titel);
				$this->tekst = $row->Tekst;
				$this->htmldirect = $row->HTMLdirect;
				$this->inlinestyle = str_replace("<style>", "", str_replace("</style>", "", $row->InlineStyle));
				$this->naamlogging = $row->Titel;
			} else {
				$this->wiid = 0;
			}
		}
	}
	
	public function setTekst($p_tekst) {
		
		if ($p_tekst !== $this->tekst) {
			$this->tekst = $p_tekst;
			$this->update($this->wiid, "Tekst", $this->tekst);
		}
	}
	
	public function htmloptions($p_cv) {
		$rv = "";
		
		foreach ($this->basislijst("", "WI.Titel") as $row) {
			$rv .= sprintf("<option value=%d%s>%s</option>\n", $row->RecordID, checked($row->RecordID, "option", $p_cv), $row->Titel);
		}
		
		return $rv;
	}
	
	public function add() {
		$nrid = $this->nieuwrecordid();
		$this->tas = 21;
		
		$query = sprintf("INSERT INTO %s (RecordID, Titel) VALUES (%d, '*** Nieuw ***');", $this->table, $nrid);
		$this->execsql($query);
		$this->mess = sprintf("Website_inhoud: Record %d is toegevoegd.", $nrid);
		
		$this->log($nrid);
		$this->wiid = $nrid;
		
		return $nrid;
	}
	
	public function update($p_wiid, $p_kolom, $p_waarde, $p_reden="") {
		$this->tas = 22;
		if ($this->pdoupdate($p_wiid, $p_kolom, $p_waarde) > 0) {
			$this->log($p_wiid);
		}
	}
	
}  # cls_Website_inhoud	

class cls_Eigen_lijst extends cls_db_base {
	public int $elid = 0;
	public int $aantal_params = 0;
	public string $waarde_params = "";
	public string $sqlerror = "";
	public string $naam = "";
	public string $uitleg = "";
	public string $mysql = "";
	public string $eigenscript = "";
	public int $groepmelding = 0;
	public int $aantalkolommen = 0;
	public int $aantalrijen = 0;
	public string $tabpage = "";
	private int $kolomlidid = -1;
	public string $laatstecontrole = "";
	
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
		$this->vulvars($this->elid);
	}
	
	public function vulvars($p_elid=-1) {
		if ($p_elid >= 0) {
			$this->elid = $p_elid;
		}
		
		if ($this->elid > 0) {
			$query = sprintf("SELECT EL.* FROM %s WHERE EL.RecordID=%d;", $this->basefrom, $this->elid);
			$elrow = $this->execsql($query)->fetch();

			if (isset($elrow->RecordID)) {
				$this->naam = $elrow->Naam ?? "";
				$this->naamlogging = $this->naam;
				$this->uitleg = $elrow->Uitleg ?? "";
				$this->mysql = $elrow->MySQL ?? "";
				$this->waarde_params = str_replace("\"", "'", $elrow->Default_value_params ?? "");
				$this->eigenscript = $elrow->EigenScript ?? "";
				$this->groepmelding = $elrow->GroepMelding ?? 0;
				$this->aantalkolommen = $elrow->AantalKolommen ?? 0;
				$this->aantalrijen = $elrow->AantalRecords ?? 0;
				$this->tabpage = $elrow->Tabpage ?? "";
				$this->kolomlidid = $elrow->KolomLidID ?? -1;
				$this->laatstecontrole = $elrow->LaatsteControle ?? "";
				$this->ingevoerd = $elrow->Ingevoerd ?? "";
				
				$this->aantal_params = 0;
				for ($p=0;$p<=9;$p++) {
					if (strpos($this->mysql, "@P" . $p) > 0) {
						$this->aantal_params++;
					}
				}
				
			} else {
				$this->elid = 0;
			}
		}
	}  # cls_Eigen_lijst->vulvars
	
	public function recordid($p_filter) {
		$query = sprintf("SELECT IFNULL(MIN(EL.RecordID), 0) FROM %s WHERE %s", $this->basefrom, $p_filter);
		$this->elid = $this->scalar($query);
		return $this->elid;
	}  # cls_Eigen_lijst->recordid
	
	public function lijst($p_filter="") {
		
		$w = "";
		if ($p_filter === 1) {
			// Alleen eigen lijsten met records
			$w = "WHERE EL.AantalRecords > 0";
		} elseif ($p_filter === 2) {
			// Alleen eigen lijsten die geschikt zijn voor selecties/filters/mailingen
			$w = "WHERE EL.KolomLidID >= 0 AND EL.Aantal_params=0";
		} elseif ($p_filter === 3) {
			// Alleen eigen lijsten die records hebben en geschikt zijn voor selecties/filters/mailingen
			$w = "WHERE EL.AantalRecords > 0 AND EL.KolomLidID >= 0 AND EL.Aantal_params=0";
		} elseif ($p_filter == 4) {
			// Persoonlijke meldingen op het voorblad
			$w = sprintf("WHERE EL.GroepMelding > 0 AND EL.GroepMelding IN (%s)", $_SESSION['lidgroepen']);
			
		} elseif (strlen($p_filter) > 1) {
			$w = "WHERE " . $p_filter;
		}
		$query = sprintf("SELECT EL.* FROM %s %s ORDER BY EL.Naam;", $this->basefrom, $w);
		$result = $this->execsql($query);
		return $result->fetchAll();
	}  # cls_Eigen_lijst->lijst
	
	public function htmloptions($p_cv=-1, $p_filter=0) {
	
		$rv = "";
		foreach ($this->lijst($p_filter) as $row) {
			$o = $row->Naam;
			if ($row->AantalRecords > 1) {
				$o .= sprintf(" (%d leden)", $row->AantalRecords);
			} elseif ($row->AantalRecords == 0) {
				$o .= " (geen records)";
			} else {
				$o .= " (1 lid)";
			}
			$rv .= sprintf("<option%s value=%d>%s</option>\n", checked($row->RecordID, "option", $p_cv), $row->RecordID, $o);
		}
		return $rv;
	}  # cls_Eigen_lijst->htmloptions
	
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
	
	public function rowset($p_elid=-1, $p_waarde="", $p_fetched=1) {
		global $dbc;
		
		if ($p_elid > 0) {
			$this->elid = $p_elid;
		}
		if (strlen($p_waarde) == 0) {
			$p_waarde = $this->waarde_params;
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
				if ($p_fetched == 1) {
					$rows = $result->fetchAll();
					$this->update($this->elid, "AantalRecords", count($rows));

					return $rows;
				} else {
					return $this->query;
				}
			
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
	}  # cls_Eigen_lijst->record
	
	public function add() {
		$this->tas = 1;
		
		if (WEBMASTER)  {
			$query = sprintf("INSERT INTO %s (RecordID, Naam, MySQL, AantalRecords, Ingevoerd) VALUES (%d, '*** Nieuwe lijst ***', '', 0, NOW());", $this->table, $this->nieuwrecordid());
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
				$p_waarde = substr($p_waarde, 0, -1);
			}
			$i_auth = new cls_Authorisation();
			
			if ($i_auth->recordid($p_waarde) == 0) {
				$this->mess = sprintf("Tabpagina '%s' bestaat niet en kan dus niet aan een eigen lijst worden gekoppeld.", $p_waarde);
				$p_waarde = "";
				$this->log($p_elid, 1);
			}
			
			$i_auth = null;
		}

		if ($this->pdoupdate($this->elid, $p_kolom, $p_waarde, $p_reden) > 0) {
			if ($p_kolom != "LaatsteControle") {
				$this->log($p_elid);
			}
		}
	}  # cls_Eigen_lijst->update
	
	public function delete($p_elid) {
		$this->tas = 3;
		$this->vulvars($p_elid);
		$this->pdodelete($p_elid);
	}
	
	public function controle($p_elid=-1, $p_altijd=0, $p_maxaantal=3) {
		global $dbc;
		$starttijd = microtime(true);
		
		if ($p_elid > 0) {
			$this->elid = $p_elid;
		}
		
		$rv = 0;

		if ($p_elid > 0) {
			$this->elid = $p_elid;
		}
		if ($this->elid > 0 and $p_altijd == 1) {
			$w = sprintf("EL.RecordID=%d", $this->elid);
		} elseif ($this->elid > 0) {
			$w = sprintf("EL.RecordID=%d AND EL.LaatsteControle < DATE_SUB(NOW(), INTERVAL 30 MINUTE)", $this->elid);
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
			if (strlen($this->mysql) > 7) {
				$elres = $dbc->prepare($this->rowset($row->RecordID, "", 0));
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
						if ($elres->getColumnMeta($i)['name'] == "LidID") {
							$kol_lidid = $i;
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
			$this->aantalkolommen = $cc;
			$this->update($row->RecordID, "AantalRecords", $rc);
			$this->aantalrijen = $rc;
			$this->update($row->RecordID, "KolomLidID", $kol_lidid);
			$this->update($row->RecordID, "LaatsteControle", date("Y-m-d H:i:s"));
			$rv++;
		}
		
		if ($rv > 1 or ((microtime(true) - $starttijd) > $_SESSION['settings']['performance_trage_select'] and $_SESSION['settings']['performance_trage_select'] > 0)) {
			$this->mess = sprintf("cls_Eigen_lijst->controle: er zijn in %.1f seconden %d lijsten gecontroleerd.", (microtime(true) - $starttijd), $rv);
			$this->tas = 10;
			$this->Log();
		}

		return $rv;

	}  # cls_Eigen_lijst->controle
	
	public function opschonen() {
		$this->optimize();
	}  # cls_Eigen_lijst->opschonen
	
} # cls_Eigen_lijst

class cls_Foto extends cls_db_base {
	
	private $ftid = 0;
	public $fotodata = "";
	public $laatstgewijzigd = "";
	
	function __construct($p_ftid=-1, $p_lidid=-1) {
		$this->table = TABLE_PREFIX . "Foto";
		$this->basefrom = $this->table . " AS Foto";
		$this->ta = 6;
		$laatstgewijzigd = date("Y-m-d H:i:s");
		$this->vulvars($p_ftid, $p_lidid);
	}

	private function vulvars($p_ftid=-1, $p_lidid=-1) {
		if ($p_ftid > 0) {
			$this->ftid = $p_ftid;
		} elseif ($p_lidid > 0) {
			$this->lidid = $p_lidid;
		}
		if ($this->lidid > 0) {
			$query = sprintf("SELECT IFNULL(MAX(Foto.RecordID), 0) FROM %s WHERE Foto.LidID=%d;", $this->basefrom, $p_lidid);
			$this->ftid = $this->scalar($query);
		}
		
		if ($this->ftid > 0) {
			$query = sprintf("SELECT Foto.* FROM %s WHERE Foto.RecordID=%d;", $this->basefrom, $this->ftid);
			$result = $this->execsql($query);
			$frow = $result->fetch();
			if (isset($frow->RecordID)) {
				$this->lidid = $frow->LidID;
				$this->fotodata = "data:image/jpg;charset=utf8;base64," . base64_encode($frow->FotoData);
				$this->laatstgewijzigd = $frow->FotoGewijzigd;
			} else {
				$this->ftid = 0;
			}
		}
	}  # cls_Foto->vulvars
	
	public function fotolid($p_lidid=-1) {
		if ($p_lidid > 0) {
			$this->vulvars(-1, $p_lidid);
		}
		
		if ($this->ftid > 0) {
			return $this->fotodata;
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
				$this->mess = "Nieuwe pasfoto in de tabel gezet";
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
	
	public function controle() {
		
	}
	
	public function opschonen() {
		$i_lm = new cls_Lidmaatschap();
		
		$bt = $_SESSION['settings']['fotosledenverwijderen'] ?? 0;
		
		if ($bt > 0) {
			$hd = date("Y-m-d", strtotime(sprintf("-%d month", $bt)));
			$reden = sprintf("de persoon langer dan %d maanden geen lid meer is.", $bt);

			$frows = $this->basislijst();
			foreach ($frows as $frow) {
				if ($i_lm->islid($frow->LidID) == false and $i_lm->eindelidmaatschap($frow->LidID) < $hd and $i_lm->soortlid($frow->LidID) != "Kloslid") {
					$this->delete($frow->RecordID, $reden);
				}
			}
		}
		
		$query = sprintf("SELECT Foto.RecordID FROM %s WHERE Foto.LidID NOT IN (SELECT L.RecordID FROM %sLid AS L WHERE (L.Verwijderd IS NULL));", $this->basefrom, TABLE_PREFIX);
		foreach ($this->execsql($query)->fetchAll() as $frow) {
			$this->delete($frow->RecordID, "het lid niet (meer) bestaat.");
		}
		
		$this->optimize();
		
	}  # cls_Foto->opschonen
	
}  # cls_Foto

class cls_Inschrijving extends cls_db_base {
	public int $insid = 0;
	public $naam = "";
	public $achternaam = "";
	public $geboortedatum = "";
	public $email = "";
	public $inschrijfdatum = "";
	public int $afdeling = 0;
	public int $pdfaanwezig = 0;
	public $opmerking = "";
	public $eersteles = "";
	public $xml = "";
	public string $verwerkt = "";
	public int $verwijderd = 0;
	
	function __construct($p_insid=-1) {
		$this->table = TABLE_PREFIX . "Inschrijving";
		$this->basefrom = $this->table . " AS Ins";
		$this->ta = 25;
		$this->per = date("Y-m-d");
		$this->vulvars($p_insid);
		$laatstgewijzigd = date("Y-m-d H:i:s");
	}
	
	public function vulvars($p_insid=-1) {
		if ($p_insid >= 0) {
			$this->insid = $p_insid;
		}
		if ($this->insid > 0) {
			$query = sprintf("SELECT Ins.* FROM %s WHERE Ins.RecordID=%d;", $this->basefrom, $this->insid);
			$result = $this->execsql($query);
			$row = $result->fetch();
			if (isset($row->Naam)) {
				$this->naam = str_replace("  ", " ", trim($row->Naam));
				$this->naamlogging = $this->naam;
				$this->achternaam = trim($row->Achternaam ?? "");
				$this->geboortedatum = $row->Geboortedatum ?? "";
				$this->email = $row->Email ?? "";
				$this->inschrijfdatum = $row->Datum ?? "";
				$this->afdeling = $row->OnderdeelID ?? 0;
				
				if ($row->PDF == null) {
					$this->pdfaanwezig = 0;
				} else {
					$this->pdfaanwezig = 1;
				}
				
				$this->lidid = $row->LidID ?? 0;
				$this->opmerking = $row->Opmerking ?? "";
				$this->eersteles = $row->EersteLes ?? "";
				$this->xml = $row->XML ?? "";
				$this->verwerkt = substr($row->Verwerkt, 0, 10);
				if ($row->Verwijderd > "1970-01-01") {
					$this->verwijderd = 1;
				} else {
					$this->verwijderd = 0;
				}
				$this->ingevoerd = $row->Ingevoerd ?? "";
			} else {
				$this->insid = 0;
				$this->lidid = 0;
			}
		} else {
			$this->lidid = 0;
		}
		
		if ($this->lidid > 0) {
			$query = sprintf("SELECT L.*, %s AS NaamLid FROM %sLid AS L WHERE L.RecordID=%d;", $this->selectnaam, TABLE_PREFIX, $this->lidid);
			$lrow = $this->execsql($query)->fetch();
			if (isset($lrow->RecordID)) {
				if (strlen($this->naam) <= 4) {
					$this->naam = $lrow->NaamLid;
				}
				if (strlen($this->achternaam) == 0) {
					$this->achternaam = $lrow->Achternaam;
				}
				if ($lrow->GEBDATUM > "1900-01-01" and strlen($this->geboortedatum) < 10) {
					$this->geboortedatum = $lrow->GEBDATUM;
				}
				
				if (strlen($this->email) < 5) {
					if (strlen($lrow->EmailOuders) > 5) {
						$this->email = $lrow->EmailOuders;
					} elseif (strlen($lrow->Email) > 5) {
						$this->email = $lrow->Email;
					}
				}
				
			}
		}
		
		if (strlen($this->achternaam) == 0 and strlen($this->naam) > 5 and $this->insid > 0) {
			$p = strrpos($this->naam, " ");
			if ($p > 1) {
				$this->achternaam = substr($this->naam, $p+1);
			}
		}
		
	}  # cls_Inschrijving->vulvars
	
	public function lijst($p_filter=1, $p_afd=-1, $p_fetched=1, $p_order=1) {
		
		$w = "(Ins.Verwijderd IS NULL)";
		if ($p_filter == 1) {
			// Alleen onverwerkte
			$w .= " AND (Ins.Verwerkt IS NULL)";
		} elseif ($p_filter == 2) {
			// Alleen verwerkte
			$w .= " AND (NOT Ins.Verwerkt IS NULL)";
		}
		if ($p_afd > 0) {
			$w .= sprintf(" AND OnderdeelID=%d", $p_afd);
		}
		if (strlen($this->where) > 0) {
			$w .= " AND " . $this->where;
		}
		
		if ($p_order === 2) {
			$ord = "IFNULL(Ins.EersteLes, '9999-12-31'), Ins.Naam";
		} elseif ($p_order === 3) {
			// Verwerkte inschrijvingen
			$ord = "Ins.Achternaam, Ins.Naam, Ins.RecordID";
		} elseif (strlen($p_order) >= 5) {
			$ord = $p_order;
		} else {
			$ord = "IFNULL(Ins.EersteLes, '9999-12-31'), Ins.Datum, Ins.Naam";
		}

		$query = sprintf("SELECT Ins.*, O.Naam AS Afdeling, IF(LENGTH(Ins.PDF) > 10, Ins.RecordID, 0) AS LnkPDF, CONCAT(%1\$s, ' (', L.RecordID, ')') AS GekoppeldLid, IF((Ins.EersteLes IS NULL), 0, Ins.RecordID) AS KanLidWorden,
						  (SELECT MAX(L2.Achternaam) FROM %3\$sLid AS L2 WHERE L2.GEBDATUM=Ins.Geboortedatum AND (LOWER(L2.Email)=LOWER(Ins.Email) OR LOWER(L2.EmailOuders)=LOWER(Ins.Email))) AS MogelijkAlInTabel
						  FROM (%2\$s LEFT OUTER JOIN %3\$sLid AS L ON Ins.LidID=L.RecordID) LEFT OUTER JOIN %3\$sOnderdl AS O ON Ins.OnderdeelID=O.RecordID
						  WHERE %4\$s
						  ORDER BY IF(Ins.Verwerkt IS NULL, 0, 1), %5\$s;", $this->selectnaam, $this->basefrom, TABLE_PREFIX, $w, $ord);
		$res = $this->execsql($query);
		if ($p_fetched == 1) {
			return $res->fetchAll();
		} else {
			return $res;
		}
	}
	
	public function htmloptions($p_cv=-1) {
		$rv = "";
		foreach ($this->lijst(-1, -1, 1, 2) as $row) {
			if ($row->RecordID == $p_cv) {
				$s = " selected";
			} else {
				$s = "";
			}
			if (strlen($row->EersteLes) == 10) {
				$rv .= sprintf("<option%s value=%d>%s - %s</option>\n", $s, $row->RecordID, $row->Naam, substr($row->EersteLes, 0, 10));
			} else {
				$rv .= sprintf("<option%s value=%d>%s</option>\n", $s, $row->RecordID, $row->Naam);
			}
		}
		
		return $rv;
	}
	
	public function pdf($p_insid=-1) {
		
		if ($p_insid > 0) {
			$this->insid = $p_insid;
		}
		
		$query = sprintf("SELECT PDF FROM %s WHERE Ins.RecordID=%d;", $this->basefrom, $this->insid);
		return $this->scalar($query);
	}
	
	public function add($p_onderdeelid=0) {
		$this->tas = 1;
		$nrid = $this->nieuwrecordid();
		
		$query = sprintf("INSERT INTO %s (RecordID, Datum, OnderdeelID, Naam, `XML`) VALUES (%d, CURDATE(), %d, '', '');", $this->table, $nrid, $p_onderdeelid);
		$this->insid = $this->execsql($query);
		
		if ($this->insid > 0) {
			$this->mess = sprintf("Inschrijving %d is toegevoegd.", $this->insid);
			$this->log($this->insid);
		}
		
		return $this->insid;
	}  # add
	
	public function addpdf($p_insid, $p_waarde, $p_tm=0) {
		$this->vulvars($p_insid);
		$this->tas = 2;
		
		if ($this->insid <= 0) {
			$this->mess = "Er is geen inschrijvingsnummer bekend.";
		} elseif ($this->pdoupdate($this->insid, "PDF", $p_waarde)) {
			$this->mess = sprintf("Aan inschrijving %d (%s) is een PDF toegevoegd.", $this->insid, $this->naamlogging);
		} else {
			$this->mess = sprintf("Toevoegen PDF-formulier aan inschrijving %d is mislukt.", $this->insid);
		}
		$this->log($this->insid, $p_tm);
		
	}  # addpdf
	
	public function update($p_insid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_insid);
		$this->tas = 2;
		
		if ($p_kolom == "XML" and 1 == 2) {
			// Moet nog getest worden voor het in productie kan
			$nm = "";
			$e = "";
			$xml = simplexml_load_string($p_waarde);
			foreach ($xml as $col => $val) {
				if ($col == "Roepnaam") {
					$nm = trim($val);
				} elseif ($col == "Tussenv" and strlen($val) > 0) {
					$nm .= " " . trim($val);
				} elseif ($col == "Achternaam") {
					$nm .= " " .  trim($val);
				} elseif ($col == "GEBDATUM") {
					$this->update($p_insid, "Geboortedatum", $val);
				} elseif ($col == "EmailOuders") {
					$e = $val;
				} elseif ($col == "Email" and strlen($e) == 0) {
					$e = $val;
				}
			}
			$this->update($p_insid, "Naam", $nm);
			$this->update($p_insid, "Email", $e);
		}
		
//		debug("$p_insid, $p_kolom, $p_waarde", 1, 1);
		
		if ($this->pdoupdate($this->insid, $p_kolom, $p_waarde, $p_reden) > 0) {
			$this->log($this->insid);
		}
	}
	
	public function toggle($p_insid, $p_kolom) {
		
		$f = sprintf("Ins.RecordID=%d", $p_insid);
		if ($this->max($p_kolom, $f) > "1970-01-01") {
			$this->update($p_insid, $p_kolom, "NULL");
		} else {
			$this->update($p_insid, $p_kolom, date("Y-m-d"));
		}
		
	}  # cls_Inschrijving->toggle
	
	public function delete($p_insid, $p_reden="") {
		$this->vulvars($p_insid);
		$this->tas = 3;
		
		if ($this->pdodelete($this->insid, $p_reden) > 0) {
			$this->log($this->insid);
		}
	}
	
	public function controle() {
		
		foreach ($this->basislijst() as $row) {
			if (strlen($row->Datum) < 10 or $row->Datum > substr($row->Ingevoerd, 0, 10)) {
			// Tijdelijke controle mag na 1/1/2024 weg
				$this->update($row->RecordID, "Datum", substr($row->Ingevoerd, 0, 10));
			} elseif (strlen($row->Achternaam) == 0 and strlen($row->Naam) > 5) {
				$p = strrpos($row->Naam, " ");
				if ($p > 1) {
					$this->update($row->RecordID, "Achternaam", substr($row->Naam, $p));
				}
			}
		}
		
	}  # cls_Inschrijving->controle
	
	public function opschonen() {
		$query = sprintf("SELECT Ins.RecordID FROM %s WHERE IFNULL(Ins.Verwerkt, '9999-12-31') < DATE_SUB(NOW(), INTERVAL 84 MONTH);", $this->basefrom);
		$res = $this->execsql($query);
		$reden = "de inschrijving langer dan 7 jaar geleden is verwerkt.";
		foreach ($res->fetchAll() as $row) {
			$this->delete($row->RecordID, $reden);
		}
		
		$this->optimize();
		
	}  # opschonen
	
}

class cls_dms extends cls_db_base {
}  # cls_dms

class cls_Template extends cls_db_base {
	
	private $tpid = 0;
	public $naam = "";
	public $inhoud = "";
	
	function __construct($p_tpid=-1, $p_naam="") {
		$this->table = TABLE_PREFIX . "Admin_template";
		$this->basefrom = $this->table . " AS TP";
		$this->ta = 20;
		$this->vulvars($p_tpid, $p_naam);
	}
	
	public function vulvars($p_tpid=-1, $p_naam="") {
		if ($p_tpid >= 0) {
			$this->tpid = $p_tpid;
		}
		
		if (strlen($p_naam) > 0) {
			$query = sprintf("SELECT TP.RecordID FROM %s WHERE TP.Naam='%s';", $this->basefrom, $p_naam);
			$row = $this->execsql($query)->fetch();
			if (isset($row->RecordID)) {
				$this->tpid = $row->RecordID;
			}
		}
		
		if ($this->tpid > 0) {
			$query = sprintf("SELECT TP.* FROM %s WHERE TP.RecordID=%d;", $this->basefrom, $this->tpid);
			$row = $this->execsql($query)->fetch();
			if (isset($row->RecordID)) {
				$this->naam = $row->Naam;
				$this->naamlogging = $row->Naam;
				$this->inhoud = $row->Inhoud;
			} else {
				$this->tpid = 0;
			}
		}
	}
	
	private function add($p_naam) {
		$this->tas = 71;
		$nrid = $this->nieuwrecordid();
		
		$query = sprintf("INSERT INTO %s (RecordID, Naam, Inhoud) VALUES (%d, '%s', '');", $this->table, $nrid, $p_naam);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Template %d (%s) is toegevoegd.", $nrid, $p_naam);
			$this->log($nrid);
		}
	}
	
	public function update($p_tpid, $p_kolom, $p_waarde, $p_reden="") {
		$this->vulvars($p_tpid);
		$this->tas = 72;

		if ($this->pdoupdate($this->tpid, $p_kolom, $p_waarde, $p_reden)) {
			$this->log($this->tpid);
		}
	}

	public function controle() {
		
		$i_rk = new cls_Rekening();
		
		$arrTP[] = "verenigingsinfo";
		$arrTP[] = "briefpapier";
		$arrTP[] = "opzegging";
		if ($i_rk->aantal() > 0) {
			$arrTP[] = "rekening";
			foreach ($i_rk->uniekelijst("Seizoen") as $row) {
				$arrTP[] = sprintf("rekening %d", $row->Seizoen);
			}
		}
		
		foreach ($arrTP as $tp) {
			$f = sprintf("Naam='%s'", $tp);
			if ($this->aantal($f) == 0) {
				$this->add($tp);
			}
		}
	}
	
	public function opschonen() {
		$this->optimize();
	}
	
}  # cls_Template

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
		$this->arrParam['meisjesnaamtonen'] = array("Type" => "B", "Default" => 1);
		$this->arrParam['menu_met_afdelingen'] = array("Type" => "T");
		$this->arrParam['liddipl_bewaartermijn'] = array("Type" => "I", "Default" => 84);
		$this->arrParam['muteerbarememos'] = array("Type" => "T", "Default" => "D,G");
		$this->arrParam['naamvereniging'] = array("Type" => "T");
		$this->arrParam['naamvereniging_afkorting'] = array("Type" => "T");
		$this->arrParam['naamvereniging_reddingsbrigade'] = array("Type" => "T");
		$this->arrParam['rekening_groep_betaalddoor'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['rekening_bewaartermijn'] = array("Type" => "I", "Default" => 84);
		$this->arrParam['sportlink_vereniging_relcode'] = array("Type" => "T");
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
//		$this->arrParam['toneninschrijvingenbewakingen'] = array("Type" => "B", "Default" => 1);
		$this->arrParam['tonentoekomstigebewakingen'] = array("Type" => "B", "Default" => 0);
		
		$this->arrParam['performance_trage_select'] = array("Type" => "F", "Default" => 0.5);
		
		$this->arrParam['logboek_bewaartijd'] = array("Type" => "I", "Default" => 48);
		$this->arrParam['logboek_lid_bewaartijd'] = array("Type" => "I", "Default" => 84);
		
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
		$this->arrParam['mailing_rekening_stuurnaar'] = array("Type" => "I", "Default" => 1);
		$this->arrParam['mailing_rekening_valuta'] = array("Type" => "T", "Default" => "&euro&nbsp;");
		$this->arrParam['mailing_rekening_vanafid'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['mailing_rekening_replyid'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['mailing_rekening_zichtbaarvoor'] = array("Type" => "I", "Default" => 0);
		$this->arrParam['mailing_sentoutbox_auto'] = array("Type" => "B", "Default" => 1);
		$this->arrParam['mailing_tinymce_apikey'] = array("Type" => "T", "Default" => "");
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
//		$this->arrParam['vanafid_webmaster'] = array("Type" => "I");
		$this->arrParam['verjaardagenaantal'] = array("Type" => "I", "Default" => 7);
		$this->arrParam['versie'] = array("Type" => "I");
		
		$this->arrParam['db_folderbackup'] = array("Type" => "T");
		$this->arrParam['login_beperkttotgroep'] = array("Type" => "T");
		$this->arrParam['naamwebsite'] = array("Type" => "T", "Default" => "Naam website");
		$this->arrParam['path_attachments'] = array("Type" => "T");
		$this->arrParam['path_pasfoto'] = array("Type" => "T");
		$this->arrParam['title_head_html'] = array("Type" => "T", "");
		$this->arrParam['urlvereniging'] = array("Type" => "T");
		
		$this->arrParam['website_mediabestanden'] = ['Type' => "T", 'Default' => "./www/media/"];  // Gebruik relatief pad tov root website
		
		// Retentie
		$this->arrParam['ledenopschonen'] = array("Type" => "I", "Default" => 84); // Op basis van einde laatste lidmaatschap
		$this->arrParam['liddefinitiefverwijderen'] = array("Type" => "I", "Default" => 24); // Op basis van de datum van de verwijdermarkering
		$this->arrParam['fotosledenverwijderen'] = array("Type" => "I", "Default" => 6); // maanden na einde lidmaatschap
	}
	
	public function lijst() {
		$query = sprintf("SELECT *, IFNULL(ValueNum, ValueChar) AS CurVal FROM %sAdmin_param ORDER BY Naam;", TABLE_PREFIX);
		$result = $this->execsql($query);
		return $result->fetchAll();
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
		$this->tas = 1;
		
		$query = sprintf("INSERT INTO %s (RecordID, Naam, ParamType) VALUES (%d, '%s', '%s');", $this->table, $nrid, $p_naam, $p_type);
		if ($this->execsql($query) > 0) {
			$this->mess = sprintf("Parameter '%s', type '%s', is toegevoegd.", $p_naam, $p_type);
			$this->log($nrid);
		}
	}
	
	public function update($p_naam, $p_waarde, $p_reden="") {
		$this->tas = 2;
		if ($p_naam == "versie") {
			$this->tas = 9;
		}

		if (in_array($p_naam, array("db_folderbackup", "login_beperkttotgroep", "muteerbarememos", "menu_met_afdelingen", "path_attachments", "path_pasfoto", "urlvereniging", "url_eigen_help", "zs_muteerbarememos"))) {
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
			$query = sprintf("UPDATE %s SET %s WHERE RecordID=%d AND %s;", $this->table, $set, $cur->RecordID, $xw);
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
	}  # update
	
	private function delete($p_naam) {
		$this->tas = 3;
		
		$delqry = sprintf("DELETE FROM %s WHERE Naam='%s';", $this->table, $p_naam);
		if ($this->execsql($delqry) > 0) {
			$this->mess = sprintf("Parameter '%s' is verwijderd.", $p_naam);
			$this->log($row->RecordID);
		}
	}
		
	public function controle() {
		
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
				if ($this->update($naam, "ParamType", $val['Type']) > 0) {
					$this->mess = sprintf("Van parameter '%s' is het type in '%s' gewijzigd.", $naam, $val['Type']);
					$this->tas = 12;
					$this->log($row->RecordID);
				}
			}
		}
	}  # controle
	
	public function opschonen() {
		
		foreach ($this->lijst() as $row) {
			if (isset($this->arrParam[$row->Naam]) == false) {
				$this->delete($row->Naam);
			}
		}
		
		$this->optimize();
	}  # opschonen
	
}  # cls_Parameter

function db_onderhoud($type=9) {
	/* Type uitleg
		1 = na upload
	*/
	global $arrTables, $db_name, $wherelid, $wherelidond, $lididwebmasters;
	
	$i_base = new cls_db_base();
	
	// Vaste aanpassingen
	if ($type == 1) {
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
		$i_base->execsql(sprintf("ALTER TABLE %sOnderdl CHANGE `VervallenPer` `VervallenPer` DATE;", TABLE_PREFIX));
		$i_base->execsql(sprintf("ALTER TABLE %sRekening CHANGE `Datum` `Datum` DATE;", TABLE_PREFIX));
		$i_base->execsql(sprintf("ALTER TABLE %sSeizoen CHANGE `Begindatum` `Begindatum` DATE;", TABLE_PREFIX));
		$i_base->execsql(sprintf("ALTER TABLE %sSeizoen CHANGE `Einddatum` `Einddatum` DATE;", TABLE_PREFIX));
	
		$i_base->execsql(sprintf("ALTER TABLE `%sLid` CHANGE `Geslacht` `Geslacht` CHAR(1) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT 'O';", TABLE_PREFIX));
		$i_base->execsql(sprintf("ALTER TABLE `%sLid` CHANGE `Huisletter` `Huisletter` VARCHAR(2) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT '';", TABLE_PREFIX));
		$i_base->execsql(sprintf("ALTER TABLE `%sLid` CHANGE `Toevoeging` `Toevoeging` VARCHAR(5) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT '';", TABLE_PREFIX));
		$i_base->execsql(sprintf("ALTER TABLE `%sLid` CHANGE `Buitenland` `Buitenland` TINYINT(4) NULL DEFAULT '0';", TABLE_PREFIX));
		$i_base->execsql(sprintf("ALTER TABLE `%sLid` CHANGE `Legitimatietype` `Legitimatietype` CHAR(1) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT 'G';", TABLE_PREFIX));

		$query = sprintf("ALTER TABLE `%sMemo` CHANGE `Vertrouwelijk` `Vertrouwelijk` TINYINT(4) NULL DEFAULT '0'", TABLE_PREFIX);
		$i_base->execsql($query);
	}

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
	$i_base->tas = 11;
	
	// Deze code kan na 1 januari 2024 worden verwijderd.
	$tab = TABLE_PREFIX . "Onderdl";
	$col = "LedenMuterenDoor";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NULL AFTER `ORGANIS`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Onderdl";
	$col = "BeschikbaarVoor";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NULL AFTER `ORGANIS`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	// Deze code kan na 1 maart 2024 worden verwijderd.
	$tab = TABLE_PREFIX . "Admin_activiteit";
	$idx = "TableColumn";
	if ($i_base->bestaat_index($tab, $idx) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD INDEX `%s` (`RefTable`, `refColumn`);", $tab, $idx);
		$i_base->execsql($query, 2);
	}

	$tab = TABLE_PREFIX . "Inschrijving";
	$col = "OnderdeelID";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NULL AFTER `XML`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Inschrijving";
	$col = "PDF";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` LONGBLOB AFTER `XML`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Inschrijving";
	$col = "LidID";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NULL AFTER `Verwerkt`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Groep";
	$idx = "OnderdeelID";
	if ($i_base->bestaat_index($tab, $idx) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD INDEX `%s` (`OnderdeelID`);", $tab, $idx);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Afdelingskalender";
	$idx = "OnderdeelID";
	if ($i_base->bestaat_index($tab, $idx) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD INDEX `%s` (`OnderdeelID`);", $tab, $idx);
		$i_base->execsql($query, 2);
	}

	$tab = TABLE_PREFIX . "Lidond";
	$idx = "LidOnderdeel";
	if ($i_base->bestaat_index($tab, $idx) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD INDEX `%s` (`Lid`, `OnderdeelID`);", $tab, $idx);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Rekreg";
	$idx = "Rekening";
	if ($i_base->bestaat_index($tab, $idx) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD INDEX `%s` (`Rekening`);", $tab, $idx);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Mailing";
	$idx = "PRIMARY";
	if ($i_base->bestaat_index($tab, $idx) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD PRIMARY KEY(`RecordID`);", $tab, $idx);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Mailing_rcpt";
	$idx = "MailingLid";
	if ($i_base->bestaat_index($tab, $idx) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD INDEX `%s` (`MailingID`, `LidID`);", $tab, $idx);
		$i_base->execsql($query, 2);
	}
	
	// Deze code kan na 1 april 2024 worden verwijderd.
	$tab = TABLE_PREFIX . "Inschrijving";
	$col = "Geboortedatum";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` DATE NULL AFTER `Naam`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Inschrijving";
	$col = "Email";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(45) NULL AFTER `Geboortedatum`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
		
	$tab = TABLE_PREFIX . "Mailing_hist";
	$col = "VanafID";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NULL AFTER `MailingID`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Afdelingskalender";
	$col = "Opmerking";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s`  VARCHAR(6) NULL AFTER `Activiteit`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Examen";
	$col = "OnderdeelID";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s`  INT NOT NULL DEFAULT '0' AFTER `Nummer`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Inschrijving";
	$col = "EersteLes";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` DATE NULL AFTER `Opmerking`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
		
	$tab = TABLE_PREFIX . "Liddipl";
	$col = "Examengroep";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NULL AFTER `Examen`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	// Deze code kan na 1 mei 2024 worden verwijderd.
	$tab = TABLE_PREFIX . "Liddipl";
	$idx = "Lid";
	if ($i_base->bestaat_index($tab, $idx) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD INDEX `%s` (`Lid`);", $tab, $idx);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Liddipl";
	$idx = "LidDiploma";
	if ($i_base->bestaat_index($tab, $idx) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD INDEX `%s` (`Lid`, `DiplomaID`);", $tab, $idx);
		$i_base->execsql($query, 2);
	}
	
// Deze code kan na 1 juni 2024 worden verwijderd.
	$tab = TABLE_PREFIX . "WS_Artikel";
	$idx = "Code";
	if ($i_base->bestaat_index($tab, $idx) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD UNIQUE INDEX `%s` (`Code`);", $tab, $idx);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "WS_Voorraadboeking";
	$idx = "PRIMARY";
	if ($i_base->bestaat_index($tab, $idx) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD PRIMARY KEY (`RecordID`);", $tab);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Stukken";
	$col = "ZichtbaarVoor";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NULL DEFAULT '0' AFTER `BestemdVoor`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Website_inhoud";
	$col = "HTMLdirect";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` TINYINT NOT NULL DEFAULT '0' AFTER `Tekst`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Website_menu";
	$col = "ExterneLink";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(150) NULL AFTER `InhoudID`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Website_inhoud";
	$col = "InlineStyle";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` TEXT NULL AFTER `HTMLdirect`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Evenement";
	$col = "Organisatie";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NULL AFTER `Locatie`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
		
	$tab = TABLE_PREFIX . "Activiteit";
	$col = "BeperkingAantal";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` SMALLINT NULL AFTER `Contributie`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	// Deze code kan na 1 juli 2024 worden verwijderd.
	$tab = TABLE_PREFIX . "Admin_activiteit";
	$col = "ReferOnderdeelID";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NULL AFTER `ReferLidID`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Seizoen";
	$col = "Afdelingscontributie omschrijving";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` TINYINT NOT NULL DEFAULT '1' AFTER `Verenigingscontributie kostenplaats`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	// Deze code kan na 1 september 2024 worden verwijderd.
	$tab = TABLE_PREFIX . "Eigen_lijst";
	$col = "Uitleg";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` TEXT NULL AFTER `Naam`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	// Deze code kan na 1 oktober 2024 worden verwijderd.
	$tab = TABLE_PREFIX . "Activiteit";
	$col = "GBR";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(4) NULL AFTER `Contributie`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Rekreg";
	$col = "ActiviteitID";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NULL AFTER `LidondID`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Rekening";
	$col = "OpmerkingIntern";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(255) NULL AFTER `Betaald`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Onderdl";
	$col = "BewaartermijnPresentie";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NULL AFTER `HistorieOpschonen`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Examen";
	$col = "Proefexamen";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` TINYINT NOT NULL DEFAULT '0' AFTER `Plaats`; ", $tab, $col);
		$i_base->execsql($query, 2);
	}

	$tab = TABLE_PREFIX . "Liddipl";
	$col = "Geslaagd";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` TINYINT NOT NULL DEFAULT '1' AFTER `DatumBehaald`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Inschrijving";
	$col = "Datum";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` DATE NOT NULL DEFAULT CURRENT_TIMESTAMP AFTER `RecordID`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Diploma";
	$col = "Doorlooptijd";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` TINYINT NULL AFTER `Volgnr`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Lid";
	$col = "NietOpschonen";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` TINYINT NOT NULL DEFAULT '0' AFTER `Opmerking`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Eigen_lijst";
	$col = "GroepMelding";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NOT NULL DEFAULT '0' AFTER `EigenScript`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
		
	$tab = TABLE_PREFIX . "Mailing_hist";
	$col = "ReplyID";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` INT NOT NULL DEFAULT '0' AFTER `ZichtbaarVoor`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
		
	$tab = TABLE_PREFIX . "Inschrijving";
	$col = "Verwijderd";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` DATE NULL;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Inschrijving";
	$col = "Achternaam";
	if ($i_base->bestaat_kolom($col, $tab) == false) {
		$query = sprintf("ALTER TABLE `%s` ADD `%s` VARCHAR(45) NULL AFTER `Naam`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	/***** Velden die aangepast zijn *****/
	$i_base->tas = 12;
	
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
	
	// Deze code mag na 1 juni 2024 worden verwijderd.
	
	if ($i_base->bestaat_kolom("GewijzigdOp", "Stukken") == true) {
		$i_base->execsql(sprintf("ALTER TABLE `%sStukken` CHANGE `GewijzigdOp` `Gewijzigd` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX));
	}
		
	// Deze code mag na 1 februari 2024 worden verwijderd.
	$query = sprintf("ALTER TABLE `%sAdmin_activiteit` CHANGE `RefFunction` `RefFunction` VARCHAR(125) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	// Deze code mag na 1 maart 2024 worden verwijderd.
	$tab = TABLE_PREFIX . "Eigen_lijst";
	$col = "Naam";
	$query = sprintf("ALTER TABLE `%1\$s` CHANGE `%2\$s` `%2\$s` VARCHAR(50) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL;", $tab, $col);
	$i_base->execsql($query);
	
	// Deze code mag na 1 april 2024 worden verwijderd.
	$query = sprintf("ALTER TABLE `%sMailing_hist` CHANGE `cc_addr` `cc_addr` VARCHAR(50) CHARACTER SET utf8 COLLATE utf8_general_ci NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sInschrijving` CHANGE `Opmerking` `Opmerking` VARCHAR(100) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sEvenement` CHANGE `Datum` `Datum` DATETIME NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sExamen` CHANGE `Datum` `Datum` DATE NOT NULL DEFAULT CURRENT_TIMESTAMP;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sLiddipl` CHANGE `Examen` `Examen` INT(11) NOT NULL DEFAULT '0';", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sLidmaatschap` CHANGE `RedenOpzegging` `RedenOpzegging` TEXT CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sEigen_lijst` CHANGE `MySQL` `MySQL` TEXT CHARACTER SET utf8 COLLATE utf8_general_ci NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sInschrijving` CHANGE `XML` `XML` TEXT CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sMemo` CHANGE `Memo` `Memo` TEXT CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
		
	$query = sprintf("ALTER TABLE `%sOnderdl` CHANGE `MySQL` `MySQL` TEXT CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sOnderdl` CHANGE `Opmerking` `Opmerking` TEXT CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sSeizoen` CHANGE `Rekeningomschrijving` `Rekeningomschrijving` VARCHAR(35) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	// Deze code mag na 1 mei 2024 worden verwijderd.
	$query = sprintf("ALTER TABLE `%sLiddipl` CHANGE `Lid` `Lid` INT(11) NOT NULL;", TABLE_PREFIX);
	$i_base->execsql($query);

	$query = sprintf("ALTER TABLE `%sLiddipl` CHANGE `DiplomaID` `DiplomaID` INT(11) NOT NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sWS_Voorraadboeking` CHANGE `RecordID` `RecordID` INT(11) NOT NULL AUTO_INCREMENT;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sEvenement` CHANGE `Organisatie` `Organisatie` INT(11) NOT NULL DEFAULT '0'; ", TABLE_PREFIX);
	$i_base->execsql($query);
	
	// Deze code mag na 1 oktober 2024 worden verwijderd.
	$query = sprintf("ALTER TABLE `%sAdmin_login` CHANGE `RecordID` `RecordID` INT(11) NOT NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sMemo` CHANGE `RecordID` `RecordID` INT(11) NOT NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sMailing_vanaf` CHANGE `Vanaf_email` `Vanaf_email` VARCHAR(50) CHARACTER SET utf8 COLLATE utf8_general_ci NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	$query = sprintf("ALTER TABLE `%sAdmin_activiteit` CHANGE `IP_adres` `IP_adres` VARCHAR(45) CHARACTER SET utf8 COLLATE utf8_general_ci NULL;", TABLE_PREFIX);
	$i_base->execsql($query);
	
	/***** Velden die niet meer nodig zijn *****/
	$i_base->tas = 13;
	
	// Deze code kan pas verwijderd worden als de kolom ook uit de Access-database is verwijderd.
	$tab = TABLE_PREFIX . "Lid";
	$col = "Nummer";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	// Deze code kan na 1 maart 2024 worden verwijderd.
	$tab = TABLE_PREFIX . "Functie";
	$col = "Oms_Vrouw";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Mailing";
	$col = "GebruikPlaatjeAlsBericht";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Mailing";
	$col = "SentBy";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Mailing";
	$col = "DeletedBy";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Mailing";
	$col = "InterneOpmerking";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
		
	$tab = TABLE_PREFIX . "Mailing";
	$col = "new_on";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Mailing";
	$col = "AddedBy";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Mailing";
	$col = "changed_on";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
		
	$tab = TABLE_PREFIX . "Mailing";
	$col = "ChangedBy";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Mailing_hist";
	$col = "Successful";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Mailing_hist";
	$col = "send_by";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Mailing";
	$col = "MailingID";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Mailing";
	$col = "from_name";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
		
	$tab = TABLE_PREFIX . "Mailing";
	$col = "from_addr";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}

	$tab = TABLE_PREFIX . "Examen";
	$col = "Omschrijving";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	// Deze code kan na 1 mei 2024 worden verwijderd.
	$tab = TABLE_PREFIX . "Liddipl";
	$col = "EXPLAATS";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	// Deze code kan na 1 september 2024 worden verwijderd.
	$tab = TABLE_PREFIX . "Mailing_hist";
	$col = "from_name";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Mailing_hist";
	$col = "from_addr";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Mailing";
	$col = "to_name";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Aanwezigheid";
	$col = "IngevoerdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Aanwezigheid";
	$col = "GewijzigdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Admin_login";
	$col = "GewijzigdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
		
	$tab = TABLE_PREFIX . "Admin_param";
	$col = "IngevoerdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
		
	$tab = TABLE_PREFIX . "Admin_param";
	$col = "GewijzigdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
			
	$tab = TABLE_PREFIX . "Admin_template";
	$col = "GewijzigdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Afdelingskalender";
	$col = "IngevoerdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
				
	$tab = TABLE_PREFIX . "Afdelingskalender";
	$col = "GewijzigdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Eigen_lijst";
	$col = "IngevoerdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
				
	$tab = TABLE_PREFIX . "Eigen_lijst";
	$col = "GewijzigdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Evenement";
	$col = "IngevoerdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
				
	$tab = TABLE_PREFIX . "Evenement";
	$col = "GewijzigdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
		
	$tab = TABLE_PREFIX . "Evenement_Type";
	$col = "GewijzigdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Lid";
	$col = "GewijzigdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
		
	$tab = TABLE_PREFIX . "Mailing";
	$col = "IngevoerdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
				
	$tab = TABLE_PREFIX . "Mailing";
	$col = "GewijzigdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "WS_Artikel";
	$col = "IngevoerdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
				
	$tab = TABLE_PREFIX . "WS_Artikel";
	$col = "GewijzigdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "WS_Orderregel";
	$col = "IngevoerdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
				
	$tab = TABLE_PREFIX . "WS_Orderregel";
	$col = "GewijzigdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "WS_Voorraadboeking";
	$col = "IngevoerdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
				
	$tab = TABLE_PREFIX . "WS_Voorraadboeking";
	$col = "GewijzigdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Evenement_Deelnemer";
	$col = "IngevoerdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	$tab = TABLE_PREFIX . "Evenement_Deelnemer";
	$col = "GewijzigdDoor";
	if ($i_base->bestaat_kolom($col, $tab) == true) {
		$query = sprintf("ALTER TABLE `%s` DROP `%s`;", $tab, $col);
		$i_base->execsql($query, 2);
	}
	
	/***** Opschonen database na een upload uit de Access-database.  *****/
	
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
	
	/*
	$p_typebackup
		1 = alleen bestanden niet uit Access-db naar bestand
		2 = alleen bestanden uit Access-db naar bestand
		3 = alle tabellen naar bestand
		4 = alle data naar scherm, zonder logins en logging
	*/
	$starttijd = microtime(true);
	
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
	if (strtotime($laatstebackup) < mktime(date("H")-1, date("m"), 0, date("m"), date("d"), date("Y")) or $p_typebackup >= 4 or $_SERVER["HTTP_HOST"] == "phprbm.telling.nl") {
		
		$volgnrbestand = 1;
		$FileName = $db_folderbackup . $buname . "_" . $volgnrbestand . ".sql";
		$buf = fopen($FileName, 'w');
		$data = "";
				
		$aanttab = 0;
		$arrbufiles = array();
		$i_base = new cls_db_base();
		foreach ($arrTables as $tnr => $tnm) {
			set_time_limit(60);
			$table = TABLE_PREFIX . $tnm;
			$i_base->table = $table;
			$query = sprintf("SELECT COUNT(*) FROM %s;", $table);
			$a = $i_base->scalar($query);
//			debug($table . ": " . $a);

			if ($a > 0 and ($p_typebackup == 3 or ($p_typebackup == 1 and $tnr < 30) or ($p_typebackup == 2 and $tnr >= 30) or ($p_typebackup == 4 and $tnm != "Foto"  and $tnm != "Inschrijving" and substr($tnm, 0, 6) != "Admin_"))) {
			
				if ($p_typebackup != 4) {
					$data = $i_base->exporttosql(2);
					fwrite($buf, $data);
					$data = "";
				}

				if ($p_typebackup == 4 and $a > 25000) {
					$query = sprintf("SELECT * FROM `%s` ORDER BY RecordID DESC LIMIT 35000;", $table);
				} else {
					$query = sprintf("SELECT * FROM `%s`;", $table);
				}
				$result = $dbc->prepare($query);
				$result->execute();
				$num_fields = $result->columnCount();		
				if ($p_typebackup == 4) {
					$data .= sprintf("TRUNCATE `%s`;\n", $table);
				}
		
				while($row = $result->fetch(PDO::FETCH_NUM)) {
					$data .= sprintf("INSERT INTO `%s` VALUES (", $table);
					for($j=0; $j<$num_fields; $j++) {
						$meta = $result->GetColumnMeta($j);
						$row[$j] = addslashes($row[$j]);
						if ($meta['native_type'] == "LONG" or $meta['native_type'] == "TINY"  or $meta['native_type'] == "SHORT") {
							if (isset($row[$j]) and strlen($row[$j]) > 0) {
								$data .= $row[$j];
							} else {
								$data .= "0";
							}
							
							
						} elseif ($meta['native_type'] == "DATE" or $meta['native_type'] == "DATETIME" or $meta['native_type'] == "TIMESTAMP" or $meta['native_type'] == "TIME") {
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
							
						} elseif ($meta['native_type'] == "BLOB" or $meta['native_type'] == "LONGBLOB") {
							if ($i_base->typekolom($meta['name'], $tnm) == "text") {
								$data .= '"' . $row[$j] . '"' ;
							} else {
								$data .= "0x" . bin2hex($row[$j]);
//								$data .= $row[$j];
							}
							
						} elseif (isset($row[$j])) {
							$data .= '"' . $row[$j] . '"' ;

						} else {
							$data .= '""';
						}
						if ($j < ($num_fields-1)) {
							$data .= ",";
						}
					}
					$data .= ");\n";
					
					fwrite($buf, $data);
					$data = "";
					
					$stat = fstat($buf);
					if (($stat['size']/1024) > (12 * 1024)) {
						fclose($buf);
						$mess = sprintf("Backup-bestand %s is bewaard.", str_replace($db_folderbackup, "", $FileName));
						$arrbufiles[] = str_replace($db_folderbackup, "", $FileName);
						(new cls_Logboek())->add($mess, 3, 0, 0, 0, 4);
					
						$volgnrbestand++;
						$FileName = $db_folderbackup . $buname . "_" . $volgnrbestand . ".sql";				
						$aantreginbestand = 0;
						$buf = fopen($FileName, 'w');
					}
				}
				$aanttab++;
				
				fwrite($buf, "\n");
				
				$stat = fstat($buf);
				if (($stat['size']/1024) > (9 * 1024)) {
					fclose($buf);
					$mess = sprintf("Backup-bestand %s is bewaard.", str_replace($db_folderbackup, "", $FileName));
					$arrbufiles[] = str_replace($db_folderbackup, "", $FileName);
					(new cls_Logboek())->add($mess, 3, 0, 0, 0, 4);
				
					$volgnrbestand++;
					$FileName = $db_folderbackup . $buname . "_" . $volgnrbestand . ".sql";				
					$aantreginbestand = 0;
					$buf = fopen($FileName, 'w');
				}
				
				$resultcount = null;
				$result = null;
			}
		}
		fclose($buf);
		
		$mess = sprintf("Backup-bestand %s is bewaard.", str_replace($db_folderbackup, "", $FileName));
		$arrbufiles[] = str_replace($db_folderbackup, "", $FileName);
		(new cls_Logboek())->add($mess, 3, 0, 0, 0, 4);
		
		$i_base = null;
		
		if ($p_typebackup == 4) {
			printf("<p class='mededeling'>De bestand staan hier.<ul>\n", BASISURL, str_replace(BASEDIR, "", $db_folderbackup), $buname);
			foreach ($arrbufiles as $bufile) {
				printf("<li><a href='%1\$s%2\$s%3\$s'>%3\$s</a></li>\n", BASISURL, str_replace(BASEDIR, "", $db_folderbackup), $bufile);
			}
			echo("</ul>\n</p>\n");
			
			echo("<p class='mededeling'>Deze export bevat geen Admin-tabellen.</p>\n");

		} elseif ($aanttab > 0) {
		
			$mess = sprintf("Backup %s (%d tabellen) is, in %d bestanden, gemaakt.", ARRTYPEBACKUP[$p_typebackup], $aanttab, count($arrbufiles));
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
	
	$exec_tijd = (microtime(true) - $starttijd);
	$mess = sprintf("Backup is in %.f seconden uitgevoerd.", $exec_tijd);
	(new cls_Logboek())->add($mess, 3, 0, 0, 0, 4);
	
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
}  # fnBackupTables

function db_createtables() {

	$queries = sprintf("SET SQL_MODE = 'NO_AUTO_VALUE_ON_ZERO';
START TRANSACTION;

CREATE TABLE IF NOT EXISTS `%1\$sAanwezigheid` (
  `RecordID` int(11) NOT NULL,
  `AfdelingskalenderID` int(11) NOT NULL,
  `LidondID` int(11) NOT NULL,
  `Status` char(1) DEFAULT NULL,
  `Opmerking` varchar(75) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sActiviteit` (
  `RecordID` int(11) NOT NULL,
  `Code` varchar(8) DEFAULT NULL,
  `Omschrijving` varchar(35) DEFAULT NULL,
  `Contributie` decimal(8,2) DEFAULT NULL,
  `GBR` varchar(4) DEFAULT NULL,
  `BeperkingAantal` smallint(6) DEFAULT NULL,
  `Vervallen` datetime DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_access` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Tabpage` varchar(75) NOT NULL,
  `Toegang` int(11) NOT NULL DEFAULT -1,
  `LaatstGebruikt` date DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_activiteit` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL COMMENT 'Wie heeft deze activiteit uitgevoerd?',
  `DatumTijd` datetime NOT NULL DEFAULT current_timestamp(),
  `Omschrijving` text NOT NULL,
  `ReferID` int(11) NOT NULL COMMENT 'Op welk RecordID heeft deze activiteit betrekking?',
  `ReferLidID` int(11) DEFAULT 0,
  `ReferOnderdeelID` int(11) DEFAULT NULL,
  `Script` varchar(100) NOT NULL,
  `TypeActiviteit` tinyint(4) DEFAULT 0,
  `TypeActiviteitSpecifiek` smallint(6) DEFAULT NULL,
  `IP_adres` varchar(45) DEFAULT NULL,
  `USER_AGENT` varchar(125) DEFAULT NULL,
  `Getoond` tinyint(4) DEFAULT NULL COMMENT 'Is deze melding aan de gebruiker getoond?',
  `RefFunction` varchar(125) DEFAULT NULL,
  `RefTable` varchar(30) DEFAULT NULL,
  `refColumn` varchar(40) DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  KEY `TypeActiviteit` (`TypeActiviteit`),
  KEY `DatumTijd` (`DatumTijd`),
  KEY `LidID` (`LidID`),
  KEY `TableColumn` (`RefTable`,`refColumn`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_interface` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) DEFAULT NULL,
  `IP-adres` varchar(45) NOT NULL,
  `SQL-statement` text CHARACTER SET utf8mb3 COLLATE utf8mb3_unicode_ci NOT NULL,
  `IngelogdLid` int(11) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gedownload` datetime DEFAULT NULL,
  `Afgemeld` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_login` (
  `RecordID` int(11) NOT NULL,
  `LidID` int(11) NOT NULL,
  `Login` varchar(15) NOT NULL,
  `Wachtwoord` varchar(255) NOT NULL,
  `HerinneringVervallenDiplomas` tinyint(4) NOT NULL DEFAULT 0,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime DEFAULT NULL,
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
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_param` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Naam` varchar(50) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL,
  `ParamType` char(2) DEFAULT NULL,
  `ValueChar` text DEFAULT NULL,
  `ValueNum` decimal(16,6) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Naam` (`Naam`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci COMMENT='Parameters en instellingen';

CREATE TABLE IF NOT EXISTS `%1\$sAdmin_template` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Naam` varchar(30) NOT NULL,
  `Inhoud` text DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sAfdelingskalender` (
  `RecordID` int(11) NOT NULL,
  `OnderdeelID` int(11) NOT NULL,
  `Datum` date NOT NULL DEFAULT '0000-00-00',
  `Omschrijving` varchar(75) DEFAULT NULL,
  `Activiteit` tinyint(4) NOT NULL DEFAULT 1,
  `Opmerking` varchar(6) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL DEFAULT 0,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`RecordID`),
  KEY `OnderdeelID` (`OnderdeelID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sDiploma` (
  `RecordID` int(11) NOT NULL,
  `Kode` varchar(10) DEFAULT NULL,
  `Naam` varchar(75) DEFAULT NULL,
  `Type` varchar(1) DEFAULT NULL,
  `ORGANIS` smallint(6) DEFAULT NULL,
  `MIN_LFT` smallint(6) DEFAULT NULL,
  `Volgnr` smallint(6) DEFAULT NULL,
  `Doorlooptijd` tinyint(4) DEFAULT NULL,
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
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sEigen_lijst` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Naam` varchar(50) NOT NULL,
  `Uitleg` text DEFAULT NULL,
  `MySQL` text DEFAULT NULL,
  `EigenScript` varchar(30) DEFAULT NULL,
  `GroepMelding` int(11) NOT NULL DEFAULT 0,
  `Aantal_params` tinyint(4) NOT NULL DEFAULT 0,
  `Default_value_params` varchar(100) DEFAULT NULL,
  `AantalKolommen` int(11) NOT NULL DEFAULT -1,
  `KolomLidID` int(11) NOT NULL DEFAULT -1,
  `AantalRecords` int(11) NOT NULL DEFAULT 0,
  `Tabpage` varchar(75) DEFAULT NULL,
  `LaatsteControle` datetime NOT NULL DEFAULT current_timestamp(),
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) NOT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sEvenement` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Datum` datetime DEFAULT NULL,
  `Eindtijd` varchar(5) DEFAULT NULL,
  `Verzameltijd` varchar(5) DEFAULT NULL,
  `Omschrijving` varchar(50) NOT NULL,
  `Locatie` varchar(75) DEFAULT NULL,
  `Organisatie` int(11) NOT NULL DEFAULT 0,
  `Email` varchar(45) DEFAULT NULL,
  `TypeEvenement` int(11) NOT NULL,
  `InschrijvingOpen` tinyint(4) NOT NULL DEFAULT 1,
  `StandaardStatus` char(1) NOT NULL DEFAULT 'I',
  `MaxPersonenPerDeelname` int(11) NOT NULL DEFAULT 1,
  `BeperkTotGroep` int(11) NOT NULL DEFAULT 0 COMMENT 'Welke groep mag zich voor dit evenement inschrijven? 0 = iedereen.',
  `ZichtbaarVoor` int(11) NOT NULL DEFAULT 0 COMMENT 'Voor welke groep is dit evenement zichtbaar? 0 = iedereen.',
  `MeerdereStartMomenten` tinyint(4) NOT NULL DEFAULT 0,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) DEFAULT NULL,
  `VerwijderdOp` date DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sEvenement_Deelnemer` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL,
  `EvenementID` int(11) NOT NULL,
  `Functie` varchar(30) CHARACTER SET utf8mb3 COLLATE utf8mb3_unicode_ci NOT NULL,
  `Opmerking` varchar(255) CHARACTER SET utf8mb3 COLLATE utf8mb3_unicode_ci NOT NULL,
  `Status` char(1) CHARACTER SET utf8mb3 COLLATE utf8mb3_unicode_ci DEFAULT 'I',
  `StartMoment` time DEFAULT NULL,
  `Aantal` int(11) NOT NULL DEFAULT 1,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL DEFAULT 0,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `LidEvenement` (`LidID`,`EvenementID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sEvenement_Type` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Omschrijving` varchar(30) CHARACTER SET utf8mb3 COLLATE utf8mb3_unicode_ci DEFAULT NULL,
  `Soort` char(1) NOT NULL DEFAULT 'W',
  `Tekstkleur` varchar(12) DEFAULT NULL,
  `Vet` tinyint(4) NOT NULL DEFAULT 0,
  `Cursief` tinyint(4) NOT NULL DEFAULT 0,
  `Achtergrondkleur` varchar(12) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sExamen` (
  `Nummer` int(11) NOT NULL,
  `OnderdeelID` int(11) NOT NULL DEFAULT 0,
  `Datum` date NOT NULL DEFAULT current_timestamp(),
  `Plaats` varchar(30) DEFAULT NULL,
  `Proefexamen` tinyint(4) NOT NULL DEFAULT 0,
  `Begintijd` varchar(5) DEFAULT NULL,
  `Eindtijd` varchar(5) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`),
  UNIQUE KEY `DatumPlaats` (`Datum`,`Plaats`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sExamenonderdeel` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `DiplomaID` int(11) NOT NULL,
  `Regelnr` int(11) NOT NULL,
  `Code` char(4) NOT NULL,
  `Omschrijving` varchar(50) NOT NULL,
  `VetGedrukt` tinyint(4) NOT NULL DEFAULT 0,
  `Ingevoerd` datetime NOT NULL DEFAULT current_timestamp(),
  `Gewijzigd` datetime NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sFoto` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL DEFAULT 0,
  `FotoData` longblob NOT NULL,
  `Type` char(1) NOT NULL DEFAULT 'P',
  `FotoGewijzigd` datetime NOT NULL DEFAULT current_timestamp(),
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sFunctie` (
  `Nummer` smallint(6) NOT NULL,
  `Omschrijv` varchar(35) DEFAULT NULL,
  `Afkorting` varchar(10) DEFAULT NULL,
  `Sorteringsvolgorde` smallint(6) DEFAULT NULL,
  `Afdelingsfunctie` tinyint(4) DEFAULT NULL,
  `Ledenadministratiefunctie` tinyint(4) DEFAULT NULL,
  `Bewakingsfunctie` tinyint(4) DEFAULT NULL,
  `Kader` tinyint(4) DEFAULT NULL,
  `Inval` tinyint(4) DEFAULT NULL,
  `Vervallen per` date DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

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
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  KEY `OnderdeelID` (`OnderdeelID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sInschrijving` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Datum` date NOT NULL DEFAULT current_timestamp(),
  `Naam` varchar(50) DEFAULT NULL,
  `Geboortedatum` date DEFAULT NULL,
  `Email` varchar(45) DEFAULT NULL,
  `Opmerking` varchar(100) DEFAULT NULL,
  `EersteLes` date DEFAULT NULL,
  `XML` text DEFAULT NULL,
  `PDF` longblob DEFAULT NULL,
  `OnderdeelID` int(11) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Verwerkt` datetime DEFAULT NULL,
  `LidID` int(11) DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sLid` (
  `RecordID` int(11) NOT NULL,
  `Roepnaam` varchar(17) DEFAULT NULL,
  `Tussenv` varchar(7) DEFAULT NULL,
  `Achternaam` varchar(45) DEFAULT NULL,
  `Meisjesnm` varchar(25) DEFAULT NULL,
  `Voorletter` varchar(10) DEFAULT NULL,
  `Voornamen` varchar(40) DEFAULT NULL,
  `Geslacht` char(1) DEFAULT 'O',
  `GEBDATUM` date DEFAULT NULL,
  `Overleden` date DEFAULT NULL,
  `GEBPLAATS` varchar(22) DEFAULT NULL,
  `Adres` varchar(35) DEFAULT NULL,
  `Huisnr` int(11) DEFAULT NULL,
  `Huisletter` varchar(2) DEFAULT '',
  `Toevoeging` varchar(5) DEFAULT '',
  `Postcode` varchar(7) DEFAULT NULL,
  `Woonplaats` varchar(22) DEFAULT NULL,
  `Buitenland` tinyint(4) DEFAULT 0,
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
  `Legitimatietype` char(1) DEFAULT 'G',
  `Legitimatienummer` varchar(15) DEFAULT NULL,
  `VOG afgegeven` date DEFAULT NULL,
  `LoginWebsite` varchar(20) DEFAULT NULL,
  `Wijknummer` int(11) DEFAULT NULL,
  `RelnrRedNed` varchar(8) DEFAULT NULL,
  `Beroep` varchar(40) DEFAULT NULL,
  `Opmerking` varchar(60) DEFAULT NULL,
  `NietOpschonen` tinyint(4) NOT NULL DEFAULT 0,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime DEFAULT NULL,
  `GewijzigdDoor` int(11) DEFAULT NULL,
  `Verwijderd` date DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sLiddipl` (
  `RecordID` int(11) NOT NULL,
  `Lid` int(11) NOT NULL,
  `DiplomaID` int(11) NOT NULL,
  `DatumBehaald` date DEFAULT NULL,
  `Geslaagd` tinyint(4) NOT NULL DEFAULT 1,
  `Beoordelaar` int(11) DEFAULT NULL,
  `LaatsteBeoordeling` tinyint(4) DEFAULT NULL,
  `Diplomanummer` varchar(25) DEFAULT NULL,
  `Examen` int(11) NOT NULL DEFAULT 0,
  `Examengroep` int(11) DEFAULT NULL,
  `LicentieVervallenPer` date DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  KEY `Lid` (`Lid`),
  KEY `LidDiploma` (`Lid`,`DiplomaID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sLidmaatschap` (
  `RecordID` int(11) NOT NULL,
  `Lid` int(11) DEFAULT NULL,
  `LIDDATUM` date DEFAULT NULL,
  `Opgezegd` date DEFAULT NULL,
  `OpgezegdDoorVereniging` tinyint(4) DEFAULT NULL,
  `RedenOpzegging` text DEFAULT NULL,
  `Lidnr` int(11) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Lidnr` (`Lidnr`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

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
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  KEY `LidOnderdeel` (`Lid`,`OnderdeelID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sMailing` (
  `RecordID` int(11) NOT NULL DEFAULT 0,
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
  `template` tinyint(4) NOT NULL DEFAULT 0,
  `CCafdelingen` tinyint(4) NOT NULL DEFAULT 0,
  `Concept` tinyint(4) DEFAULT 1,
  `ZichtbaarVoor` int(11) NOT NULL DEFAULT 0,
  `EvenementID` int(11) NOT NULL DEFAULT 0,
  `deleted_on` datetime DEFAULT NULL,
  `HTMLdirect` tinyint(4) DEFAULT 0,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) DEFAULT 0,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sMailing_hist` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `LidID` int(11) NOT NULL DEFAULT 0,
  `MailingID` int(11) NOT NULL,
  `VanafID` int(11) DEFAULT NULL,
  `Xtra_Char` varchar(5) DEFAULT NULL,
  `Xtra_Num` int(11) DEFAULT NULL,
  `to_name` varchar(100) NOT NULL,
  `subject` varchar(75) NOT NULL,
  `to_addr` varchar(255) NOT NULL,
  `cc_addr` varchar(50) DEFAULT NULL,
  `message` text NOT NULL,
  `ZonderBriefpapier` tinyint(4) DEFAULT 0,
  `ZichtbaarVoor` int(11) NOT NULL DEFAULT 0,
  `send_on` datetime DEFAULT NULL,
  `NietVersturenVoor` datetime DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`RecordID`),
  KEY `LidID` (`LidID`),
  KEY `MailingID` (`MailingID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sMailing_rcpt` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `MailingID` int(11) NOT NULL DEFAULT 0,
  `LidID` int(11) NOT NULL DEFAULT 0,
  `to_address` varchar(50) DEFAULT NULL,
  `Rcpt_Type` char(1) NOT NULL DEFAULT 'T',
  `Xtra_Char` char(5) DEFAULT NULL,
  `Xtra_Num` int(11) NOT NULL DEFAULT 0,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  PRIMARY KEY (`RecordID`),
  KEY `MailingLid` (`MailingID`,`LidID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sMailing_vanaf` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Vanaf_email` varchar(50) DEFAULT NULL,
  `Vanaf_naam` varchar(50) NOT NULL,
  `SMTP_server` tinyint(20) NOT NULL DEFAULT 0 COMMENT 'Indien 0, dan wordt de smtp server uit het config bestand gebruikt',
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `VanafEmail` (`Vanaf_email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sMemo` (
  `RecordID` int(11) NOT NULL,
  `Lid` int(11) NOT NULL,
  `Soort` varchar(1) NOT NULL,
  `Vertrouwelijk` tinyint(4) DEFAULT 0,
  `Memo` text DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Lid`,`Soort`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

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
  `BeschikbaarVoor` int(11) DEFAULT NULL,
  `LedenMuterenDoor` int(11) DEFAULT NULL,
  `Kader` tinyint(4) DEFAULT NULL,
  `Alleen leden` tinyint(4) DEFAULT NULL,
  `Tonen in bewakingsadministratie` tinyint(4) DEFAULT NULL,
  `CentraalEmail` varchar(45) DEFAULT NULL,
  `VervallenPer` date DEFAULT NULL,
  `HistorieOpschonen` int(11) DEFAULT NULL,
  `BewaartermijnPresentie` int(11) DEFAULT NULL,
  `MaximaleLengtePeriode` int(11) DEFAULT NULL,
  `GekoppeldAanQuery` int(11) DEFAULT NULL,
  `MySQL` text DEFAULT NULL,
  `Opmerking` text DEFAULT NULL,
  `Beschrijving` varchar(255) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Kode` (`Kode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sOrganisatie` (
  `Nummer` smallint(6) NOT NULL,
  `Naam` varchar(8) DEFAULT NULL,
  `Volledige naam` varchar(55) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

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
  `OpmerkingIntern` varchar(255) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sRekeningBetaling` (
  `RecordID` int(11) NOT NULL,
  `Rekening` int(11) NOT NULL,
  `Datum` date DEFAULT NULL,
  `Bedrag` decimal(8,2) DEFAULT NULL,
  `Mutatie` varchar(25) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

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
  `ActiviteitID` int(11) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  KEY `Rekening` (`Rekening`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

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
  `Rekeningomschrijving` varchar(35) DEFAULT NULL,
  `BetaaldagenTermijn` int(11) DEFAULT NULL,
  `StandaardAantalTermijnen` int(11) DEFAULT NULL,
  `Verenigingscontributie omschrijving` varchar(50) DEFAULT NULL,
  `Verenigingscontributie kostenplaats` varchar(12) DEFAULT NULL,
  `Afdelingscontributie omschrijving` tinyint(4) NOT NULL DEFAULT 1,
  `Gezinskorting bedrag` decimal(8,2) DEFAULT NULL,
  `Maximale verenigingscontributie` decimal(8,2) DEFAULT NULL,
  `Gezinskorting omschrijving` varchar(50) DEFAULT NULL,
  `Gezinskorting kostenplaats` varchar(12) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime DEFAULT NULL,
  PRIMARY KEY (`Nummer`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sStukken` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Titel` varchar(50) NOT NULL,
  `Type` char(1) NOT NULL DEFAULT 'R',
  `BestemdVoor` varchar(30) NOT NULL,
  `ZichtbaarVoor` int(11) DEFAULT 0,
  `VastgesteldOp` date DEFAULT NULL,
  `Ingangsdatum` date DEFAULT NULL,
  `Revisiedatum` date DEFAULT NULL,
  `Link` varchar(150) NOT NULL,
  `VervallenPer` date DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime NOT NULL DEFAULT current_timestamp(),
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

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
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

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
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

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
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=COMPACT;

CREATE TABLE IF NOT EXISTS `%1\$sWebsite_inhoud` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Titel` varchar(80) DEFAULT NULL,
  `Tekst` text DEFAULT NULL,
  `HTMLdirect` tinyint(4) NOT NULL DEFAULT 0,
  `InlineStyle` text DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime NOT NULL DEFAULT current_timestamp(),
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sWebsite_menu` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Titel` varchar(25) DEFAULT NULL,
  `Volgnummer` smallint(6) NOT NULL DEFAULT 1,
  `VorigeLaag` int(11) NOT NULL DEFAULT 0,
  `InhoudID` int(11) DEFAULT NULL,
  `ExterneLink` varchar(150) DEFAULT NULL,
  `Gepubliceerd` date DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `Gewijzigd` datetime NOT NULL DEFAULT current_timestamp(),
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;

CREATE TABLE IF NOT EXISTS `%1\$sWS_Artikel` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Code` varchar(8) DEFAULT NULL,
  `Omschrijving` varchar(50) DEFAULT NULL,
  `Beschrijving` text DEFAULT NULL,
  `Maat` varchar(6) DEFAULT NULL,
  `Verkoopprijs` decimal(10,2) NOT NULL DEFAULT 0.00,
  `BeschikbaarTot` date DEFAULT NULL COMMENT 'Tot welke datum is dit artikel in de zelfservice beschikbaar?',
  `VervallenPer` date DEFAULT NULL COMMENT 'Na deze datum is dit artikel voor niemand meer beschikbaar.',
  `MaxAantalPerLid` smallint(6) DEFAULT NULL COMMENT 'Hoeveel mag één lid maximaal van dit product bestellen?',
  `BeperkTotGroep` int(11) NOT NULL DEFAULT 0 COMMENT 'Welke groep mag dit artikel in de zelfservice bestellen? 0 is iedereen.',
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) DEFAULT NULL,
  PRIMARY KEY (`RecordID`),
  UNIQUE KEY `Code` (`Code`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci;

CREATE TABLE IF NOT EXISTS `%1\$sWS_Orderregel` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `Ordernr` int(11) NOT NULL DEFAULT 0,
  `Artikel` int(11) NOT NULL DEFAULT 0,
  `Lid` int(11) NOT NULL DEFAULT 0,
  `AantalBesteld` smallint(6) NOT NULL DEFAULT 0,
  `PrijsPerStuk` decimal(10,2) DEFAULT NULL,
  `Opmerking` varchar(30) DEFAULT NULL,
  `BestellingDefinitief` datetime DEFAULT NULL COMMENT 'Heeft het lid de bestelling bevestigd?',
  `Rekening` int(11) DEFAULT NULL,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewijzigdDoor` int(11) DEFAULT NULL,
  PRIMARY KEY (`RecordID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci;

CREATE TABLE IF NOT EXISTS `%1\$sWS_Voorraadboeking` (
  `RecordID` int(11) NOT NULL AUTO_INCREMENT,
  `ArtikelID` int(11) DEFAULT NULL,
  `Datum` date DEFAULT NULL,
  `Omschrijving` varchar(50) DEFAULT NULL,
  `Aantal` smallint(6) NOT NULL DEFAULT 0,
  `OrderregelID` int(11) NOT NULL DEFAULT 0,
  `Ingevoerd` datetime DEFAULT current_timestamp(),
  `IngevoerdDoor` int(11) NOT NULL DEFAULT 0,
  `Gewijzigd` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `GewjizigdDoor` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`RecordID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;
COMMIT;", TABLE_PREFIX);

	(new cls_db_base())->execsql($queries);
	
}  # db_createtables

?>
