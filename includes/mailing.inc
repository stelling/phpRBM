<?php

function fnMailing() {
	global $ldl, $currenttab, $currenttab2;

	if (isset($_GET['op'])) {
		$op = $_GET['op'];
	} else {
		$op = "";
	}
	
	if (isset($_POST['Upload']) and $_POST['Upload'] == "Upload") {
		$op = "upload";
	} elseif ($_GET['tp'] == "Mailing/Historie" and $op != "histdetails" and $op != "opnieuw") {
		$op = "historie";
	} elseif (isset($_POST['action']) and $_POST['action'] == "Bewaren & sluiten") {
		$op = "save_close";
	} elseif (isset($_POST['LedenToevoegen']) and $_POST['LedenToevoegen'] == "Leden toevoegen") {
		$op = "add_selectie";
	} elseif (isset($_POST['LedenVerwijderen']) and $_POST['LedenVerwijderen'] == "Leden verwijderen") {
		$op = "del_selectie";
	} elseif (isset($_POST['OntvangersVerwijderen']) and $_POST['OntvangersVerwijderen'] == "Alle ontvangers verwijderen") {
		$op = "delete_all_rcpt";
		
	} elseif (isset($_POST['action'])) {
		$op = strtolower($_POST['action']);
	} elseif (isset($_GET['op'])) {
		$op = $_GET['op'];
	} elseif ($_SERVER['REQUEST_METHOD'] == "POST") {
		$op = "save";
	}

	if (isset($_GET['mid']) and $_GET['mid'] > 0) {
		$mailing = new Mailing($_GET['mid']);
	} else {
			$mailing = new Mailing();
	}
	
	if ($op != "preview") {
		HTMLheader();
	}
	
	if (($op == "historie" or $op == "histdetails") and $mailing->mid > 0) {
		fnDispMenu(2, sprintf("mid=%d", $mailing->mid));
	} elseif ($op != "preview") {
		fnDispMenu(2);
	}

	if ($op == "edit" and isset($_GET['mid'])) {
		$mailing->edit();
				
	} elseif ($currenttab2 == "Instellingen" and toegang($_GET['tp'], 1, 1)) {
		fnMailingInstellingen();
		
	} elseif ($op == "save" or $op == "save_close" or $op == "toevoegen") {
		$mailing->post_form();
		$mailing->upload();
		if ($op == "save_close") {
			printf("<script>location.href='%s?tp=Mailing/%s';</script>\n", $_SERVER['PHP_SELF'], $mailing->folder());
		} else {
			$mailing->edit();
		}
		
	} elseif ($op == "opnieuw" and isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0) {
		sendfromhist($_GET['MailingHistID'], 1);
		echo("<script>window.history.back();</script>\n");
		
	} elseif ($currenttab2 == "Nieuw" and toegang($_GET['tp'], 1, 1)) {
		$mailing->mid = 0;
		$mailing->edit();
		
	} elseif ($currenttab2 == "Rekeningen" and toegang($_GET['tp'], 1, 1)) {
		fnRekeningenMailen($op);
		
	} elseif ($currenttab2 == "Logboek" and toegang($_GET['tp'], 1, 1)) {
		$lijst = db_logboek("lijst", "", 4);
		echo(fnDisplayTable($lijst, "", "", 0, "", "", "logboek"));
	} elseif ($op == "historie" and $_GET['mid'] > 0) {
		$result = db_mailing_hist("lijst", 0, 0, $_GET['mid']);
		$rows = $result->fetchAll();
		if (count($rows) > 0) {
			$ld = sprintf("<a href='index.php?tp=%s&amp;op=histdetails&amp;rid=%%d&amp;mid=%d'>%%s</a>", $_GET['tp'], $_GET['mid']);
			$th = sprintf("%d. %s", $mailing->mid, $rows[0]->ndOnderwerp);
			echo(fnDisplayTable($rows, $ld, $th, 0, 1));
		} else {
			echo("<p class='mededeling'>Deze mailing heeft geen historie.</p>\n");
		}
	} elseif ($op == "histdetails" and $_GET['rid'] > 0) {
		toonverstuurdemail($_GET['rid']);
		
	} elseif ($op == "upload") {
		$mailing->upload();
		$mailing->edit();
	} elseif ($op == "del_attach" and isset($_GET['attach'])) {
		$mailing->attach_delete($_GET['attach']);
		$mailing->edit();
	} elseif ($op == "add_rcpt" and isset($_GET['lidid']) and $_GET['lidid'] > 0) {
		$mailing->Add_rcpt($_GET['lidid']);
		$mailing->edit();
	} elseif ($op == "add_rcpt" and isset($_GET['to_address']) and isValidMailAddress($_GET['to_address'], 0)) {
		$mailing->Add_rcpt(0, $_GET['to_address']);
		$mailing->edit();
	} elseif ($op == "del_rcpt" and isset($_GET['lidid']) and $_GET['lidid'] > 0) {
		$mailing->delete_rcpt($_GET['lidid']);
		$mailing->edit();
	} elseif ($op == "del_rcpt" and isset($_GET['addr']) and strlen($_GET['addr']) > 0) {
		$mailing->delete_rcpt(0, $_GET['addr']);
		$mailing->edit();
	} elseif ($op == "add_selectie") {
		$mailing->add_del_selectie("add");
		$mailing->edit();
	} elseif ($op == "del_selectie") {
		$mailing->add_del_selectie("del");
		$mailing->edit();
	} elseif ($op == "delete_all_rcpt") {
		$mailing->delete_all_rcpt();
		$mailing->edit();
	} elseif ($op == "preview") {
		$mailing->post_form();
		$mailing->upload();
		$mailing->preview();
	} elseif ($op == "send") {
		$mailing->post_form();
		$mailing->upload();
		$mailing->send();
		$mailing->lijst($currenttab2);
	} elseif ($op == "delete") {
		$mailing->delete();
		$mailing->lijst($currenttab2);
	} elseif ($op == "undelete") {
		$mailing->undelete();
		$mailing->lijst($currenttab2);
	} else {
		$mailing->lijst($currenttab2);
	}
		
	if ($op != "preview") {
		HTMLfooter();
	}
}

class Mailing {
	public $mid;
	
	private $allowed_ext = "bmp, gif, jpeg, jpg, pdf, png, pps, rar, txt, zip";
	private $dir_attachm = "/attachments";
	private $max_size_attachm = 2097152; // 2MB
	private $vertraging_tussen_verzenden = 30; // de minimale tijd (in seconden) die er tussen het verzenden van dezelfde mailing moet zitten.
	private $smtpserver = 0; // De details van de server moeten in config.php gespecificeerd worden.
	
	private $MergeField;
	private $contains_mergefield = false;
	private $contains_losse_email = false;
	private $merged_subject = "";
	private $merged_message = "";
	private $speciaal = "";
	
	private $from_name = "";
	private $from_addr = "";
	private $to_name = "";
	private $cc_addr = "";
	private $subject = "";
	private $message = "";
	private $template = 0;
	private $confidential = 0;
	private $new_on = "0000-00-00";
	private $AddedBy = 0;
	private $changed_on = "0000-00-00";
	private $ChangedBy = 0;
	private $sent_on = "0000-00-00";
	private $SentBy = 0;
	private $deleted_on = "0000-00-00";
	private $DeletedBy = 0;
	
	private $ok_send = "";
	private $nok_send = "";
	private $meldingen = "";
	private $aant_ok = 0;
	private $aant_nok = 0;
	private $aant_rcpt = 0;
	private $verzendenmag = false;
	public $resultaatversturen = 0;
	private $emailsecretariaat = "";
	
	public $xtrachar = "";
	public $xtranum = 0;

	function __construct($mid=0) {
		global $selectnaam, $table_prefix, $fdlang, $fdkort, $arrGeslacht, $wherelid, $fromlidond, $wherelidond, $whereliddipl;

		$this->mid = $mid;
		
		if ($this->mid > 0) {
			$ml = db_mailing("record", $this->mid);
			$this->from_name = trim($ml->from_name);
			$this->from_addr = trim($ml->from_addr);
			$this->to_name = trim($ml->to_name);
			$this->cc_addr = trim($ml->cc_addr);
			$this->subject = trim($ml->subject);
			$this->message = $ml->message;
			$this->template = $ml->template;
			$this->confidential = $ml->confidential;
			$this->new_on = $ml->new_on;
			$this->AddedBy = $ml->AddedBy;
			$this->changed_on = $ml->changed_on;
			$this->ChangedBy = $ml->ChangedBy;
			$this->sent_on = $ml->sent_on;
			$this->SentBy = $ml->SentBy;
			$this->deleted_on = $ml->deleted_on;
			$this->DeletedBy = $ml->DeletedBy;
			$this->aant_rcpt = db_mailing_rcpt("aantal", $this->mid);
		} else {
			$this->from_addr = $_SESSION['emailingelogde'];
			$this->new_on = date("Y-m-d");
		}
		
		$this->dir_attachm = substr($_SERVER["SCRIPT_FILENAME"], 0, strrpos($_SERVER["SCRIPT_FILENAME"], "/")) . "/attachments";
		
		if (strlen(db_param("mailing_extensies_toegestaan")) > 0) {
			$this->allowed_ext = db_param("mailing_extensies_toegestaan");
		}
		if (db_param("max_grootte_bijlage") > 0) {
			$this->max_size_attachm = db_param("max_grootte_bijlage");
		}
		$this->resultaatversturen = db_param("mailing_resultaatversturen");
		$this->emailsecretariaat = db_param("emailsecretariaat");

		$sql = sprintf("SELECT Roepnaam FROM %sLid AS L WHERE L.Nummer=%%d;", $table_prefix);
		$this->MergeField[]=array('Naam' => "Roepnaam", 'SQL' => $sql);
		
		$sql = sprintf("SELECT CONCAT(L.Roepnaam, IF(L.GEBDATUM > DATE_SUB(CURDATE(), INTERVAL 18 YEAR), ' of ouders/verzorgers van', '')) FROM %sLid AS L WHERE L.Nummer=%%d;", $table_prefix);
		$this->MergeField[]=array('Naam' => "Roepnaam_of_Ouder", 'SQL' => $sql);

		$sql = sprintf("SELECT Voorletter FROM %sLid AS L WHERE L.Nummer=%%d;", $table_prefix);
		$this->MergeField[]=array('Naam' => "Voorletters", 'SQL' => $sql);

		$sql = sprintf("SELECT (CASE ISNULL(L.Tussenv) WHEN 0 THEN CONCAT(L.Tussenv, ' ', L.Achternaam)
				 ELSE L.Achternaam END) AS Achternaam FROM %sLid AS L WHERE L.Nummer=%%d;", $table_prefix);
		$this->MergeField[]=array('Naam' => "Achternaam", 'SQL' => $sql);
		
		$sql = "SELECT CASE L.Geslacht";
		foreach ($arrGeslacht as $key => $value) {
			$sql .= sprintf(" WHEN '%s' THEN '%s'", $key, $value);
		}
		$sql .= sprintf(" END FROM %sLid AS L WHERE L.Nummer=%%d", $table_prefix);
		$this->MergeField[]=array('Naam' => "Geslacht", 'SQL' => $sql);

		$sql = sprintf("SELECT L.Adres FROM %sLid AS L WHERE L.Nummer=%%d;", $table_prefix);
		$this->MergeField[]=array('Naam' => "Adres", 'SQL' => $sql);

		$sql = sprintf("SELECT L.Postcode FROM %sLid AS L WHERE L.Nummer=%%d;", $table_prefix);
		$this->MergeField[]=array('Naam' => "Postcode", 'SQL' => $sql);

		$sql = sprintf("SELECT L.Woonplaats FROM %sLid AS L WHERE L.Nummer=%%d;", $table_prefix);;
		$this->MergeField[]=array('Naam' => "Woonplaats", 'SQL' => $sql);
							 
		$sql = sprintf("SELECT %s AS Lidnaam FROM %sLid AS L WHERE L.Nummer=%%d;", $selectnaam, $table_prefix);
		$this->MergeField[]=array('Naam' => "Lidnaam", 'SQL' => $sql);

		$sql = sprintf("SELECT Login FROM %sAdmin_login WHERE LidID=%%d;", $table_prefix);
		$this->MergeField[]=array('Naam' => "Login", 'SQL' => $sql);

		$sql = sprintf("SELECT LM.Lidnr FROM %sLidmaatschap AS LM WHERE %s AND LM.Lid=%%d;", $table_prefix, $wherelid);
		$this->MergeField[]=array('Naam' => "Lidnummer", 'SQL' => $sql);
		
		$sql = sprintf("SELECT FLOOR(SUM(TIMESTAMPDIFF(MONTH, LM.LIDDATUM, IF(ISNULL(LM.Opgezegd), CURDATE(), LM.Opgezegd)))/12)
							FROM %sLidmaatschap AS LM WHERE LM.LIDDATUM < CURDATE() AND LM.Lid=%%d LIMIT 1;", $table_prefix);
		$this->MergeField[]=array('Naam' => "LengteLidmaatschap", 'SQL' => $sql);

		$sql = sprintf("SELECT L.Telefoon FROM %sLid AS L WHERE L.Nummer=%%d;", $table_prefix);
		$this->MergeField[]=array('Naam' => "Telefoon", 'SQL' => $sql);

		$sql = sprintf("SELECT L.Mobiel FROM %sLid AS L WHERE L.Nummer=%%d;", $table_prefix);
		$this->MergeField[]=array('Naam' => "Mobiel", 'SQL' => $sql);

		$sql = sprintf("SELECT DATE_FORMAT(L.GEBDATUM, %s) AS Geboortedatum FROM %sLid AS L WHERE L.Nummer=%%d;", $fdlang, $table_prefix);
		$this->MergeField[]=array('Naam' => "Geboortedatum", 'SQL' => $sql);

		$sql = sprintf("SELECT L.GEBPLAATS FROM %sLid AS L WHERE L.Nummer=%%d;", $table_prefix);
		$this->MergeField[]=array('Naam' => "Geboorteplaats", 'SQL' => $sql);

		$sql = sprintf('SELECT O.Naam FROM %1$sLidond AS LO INNER JOIN %1$sOnderdl AS O ON O.RecordID = LO.OnderdeelID
				 WHERE %2$s AND O.GekoppeldAanQuery=0 AND LO.Lid=%%d ORDER BY O.Naam;', $table_prefix, $wherelidond);
		$this->MergeField[]=array('Naam' => "Onderdelen", 'SQL' => $sql);
	
		if (db_bewseiz("aantal") > 0) {
			$sql = sprintf('SELECT CONCAT(DATE_FORMAT(BEGIN_PER, %1$s), \' t/m \', DATE_FORMAT(EINDE_PER, %1$s)) AS Per'
				  . ' FROM %2$sBewaking AS BW INNER JOIN %2$sBewseiz AS BS ON BS.RecordID = BW.SeizoenID'
				  . ' WHERE EINDE_PER >= CURDATE() AND %3$s ORDER BY BEGIN_PER;', $fdlang, $table_prefix, "Lid=%d");
			$this->MergeField[]=array('Naam' => "BewakingToekomst", 'SQL' => $sql);
							 
			$Funct = "(CASE BW.Functie WHEN 0 THEN '' ELSE CONCAT(' (', F.Afkorting, ')') END)";
			$sql = sprintf('SELECT CONCAT(DATE_FORMAT(BEGIN_PER, %1$s), \' t/m \', DATE_FORMAT(EINDE_PER, %1$s), %2$s) AS Per
					 FROM (%3$sBewaking AS BW INNER JOIN %3$sBewseiz AS BS ON BS.RecordID = BW.SeizoenID) INNER JOIN %3$sFunctie AS F ON BW.Functie = F.Nummer
					 WHERE BW.EINDE_PER < CURDATE() AND %4$s ORDER BY BW.BEGIN_PER;', $fdkort, $Funct, $table_prefix, "BW.Lid=%d");
			$this->MergeField[]=array('Naam' => "BewakingHistorie", 'SQL' => $sql);
		}
							  
		$sql = sprintf("SELECT L.Bankrekening FROM %sLid AS L WHERE L.Nummer=%%d;", $table_prefix);
		$this->MergeField[]=array('Naam' => "IBAN", 'SQL' => $sql);
		
		$sql = sprintf("SELECT L.Burgerservicenummer FROM %sLid AS L WHERE L.Nummer=%%d;", $table_prefix);
		$this->MergeField[]=array('Naam' => "BSN", 'SQL' => $sql);

		if ($_SESSION['aantalrekeningen'] > 0) {
			$sql = sprintf("SELECT RK.Nummer FROM %sRekening AS RK WHERE RK.Bedrag > 0 AND RK.Bedrag > RK.Betaald AND %s;", $table_prefix, "RK.Lid=%d");
			$this->MergeField[]=array('Naam' => "OpenstaandeRekeningen", 'SQL' => $sql);

			$sql = sprintf("SELECT ROUND(SUM(RK.Bedrag-RK.Betaald), 2) AS OpenstaandBedrag FROM %sRekening AS RK WHERE RK.Lid=%%d;", $table_prefix);
			$this->MergeField[]=array('Naam' => "OpenstaandBedrag", 'SQL' => $sql);
		}

		$sql = sprintf("SELECT (CASE L.`Machtiging afgegeven` WHEN 1 THEN 'Ja' ELSE 'Nee' END) AS MachtigingAfgegeven
							 FROM %sLid AS L WHERE L.Nummer=%%d;", $table_prefix);
		$this->MergeField[]=array('Naam' => "MachtigingAfgegeven", 'SQL' => $sql);
		
		$sql = sprintf('SELECT DISTINCT DP.Kode FROM %1$sLiddipl AS LD INNER JOIN %1$sDiploma AS DP ON LD.DiplomaID = DP.RecordID
				 WHERE %2$s AND LD.Lid=%%d ORDER BY DP.Volgnr, LD.DatumBehaald, DP.KODE;', $table_prefix, $whereliddipl);
		$this->MergeField[]=array('Naam' => "DiplomaKort", 'SQL' => $sql);

		$sql = sprintf('SELECT DISTINCT DP.Naam FROM %1$sLiddipl AS LD INNER JOIN %1$sDiploma AS DP ON LD.DiplomaID = DP.RecordID
				 WHERE %2$s AND LD.Lid=%%d ORDER BY DP.Volgnr, LD.DatumBehaald, DP.Naam;', $table_prefix, $whereliddipl);
		$this->MergeField[]=array('Naam' => "DiplomaLang", 'SQL' => $sql);
		
		$sql = sprintf('SELECT DISTINCT DP.Naam FROM %1$sLiddipl AS LD INNER JOIN %1$sDiploma AS DP ON LD.DiplomaID = DP.RecordID
				 WHERE %2$s AND LD.Lid=%%d AND DP.Zelfservice=1 ORDER BY DP.Volgnr, LD.DatumBehaald, DP.Naam;', $table_prefix, $whereliddipl);
		$this->MergeField[]=array('Naam' => "DiplomaZelfservice", 'SQL' => $sql);
	
		$sql = sprintf("SELECT L.RelnrRedNed FROM %sLid AS L WHERE %s;", $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "RelnrRedNed", 'SQL' => $sql);

		$sql = sprintf("SELECT DATE_FORMAT(LIDDATUM, %s) FROM %sLidmaatschap AS LM WHERE %s AND LM.Lid=%%d;", $fdlang, $table_prefix, $wherelid);
		$this->MergeField[]=array('Naam' => "LidVanaf", 'SQL' => $sql);

		$sql = sprintf("SELECT IF(LM.OPGEZEGD IS NULL, 'Niet', DATE_FORMAT(LM.Opgezegd, %s)) FROM %sLidmaatschap AS LM
							 WHERE LM.LIDDATUM < CURDATE() AND LM.Lid=%%d
							 ORDER BY LM.LIDDATUM DESC LIMIT 1;", $fdlang, $table_prefix);
		$this->MergeField[]=array('Naam' => "OpgezegdPer", 'SQL' => $sql);

		$sql = sprintf("SELECT IF((L.Email IS NULL), L.EmailOuders, L.Email) FROM %sLid AS L WHERE Nummer=%%d;", $table_prefix);
		$this->MergeField[]=array('Naam' => "Email", 'SQL' => $sql);

		$query = sprintf("SELECT COUNT(*) FROM %1\$sLidRedNed AS RN JOIN %1\$sLidmaatschap AS LM ON LM.LIDNR = RN.Lidnr WHERE LENGTH(RN.Foutboodschap) > 0;", $table_prefix);
		if (TableExists("LidRedNed") and db_scalar($query) > 0) {
			$sql = sprintf("SELECT RN.Foutboodschap FROM %1\$sLidRedNed AS RN JOIN %1\$sLidmaatschap AS LM ON LM.LIDNR = RN.Lidnr WHERE LM.LID=%%d;", $table_prefix);
			$this->MergeField[]=array('Naam' => "VerschilSportlink", 'SQL' => $sql);
		}
				
		if (db_onderdelen("aantalingebruik", "A") > 0) {
			$sql = sprintf("SELECT CONCAT(O.Naam, IF(LO.GroepID > 0, CONCAT(' (', GR.Starttijd, ' ', (SELECT DP.Naam FROM %1\$sDiploma AS DP WHERE DP.RecordID=GR.DiplomaID), ')'), IF(LO.Functie > 0, CONCAT(' (', F.Omschrijv , ')'), '')))
					FROM ((%1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON O.RecordID = LO.OnderdeelID) LEFT JOIN %1\$sGroep AS GR ON LO.GroepID=GR.RecordID) INNER JOIN %1\$sFunctie AS F ON LO.Functie = F.Nummer
					WHERE ((LO.Opgezegd IS NULL) OR LO.Opgezegd > CURDATE()) AND `Type`='A' AND LO.Lid=%%d ORDER BY O.Naam;", $table_prefix);
			$this->MergeField[]=array('Naam' => "Afdelingen", 'SQL' => $sql);
		}
		
		if (db_onderdelen("aantalingebruik", "R") > 0) {
			$sql = sprintf("SELECT O.Naam FROM %1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON O.RecordID = LO.OnderdeelID 
					 WHERE %2\$s AND `Type`='R' AND LO.Lid=%%d ORDER BY O.Naam;", $table_prefix, $wherelidond);
			$this->MergeField[]=array('Naam' => "Rollen", 'SQL' => $sql);
		}
		
		if (db_memo("aantal", 0, "D") > 0) {
			$sql = sprintf("SELECT IF((Memo IS NULL), 'Geen', Memo) FROM %sMemo WHERE Lid=%%d AND Soort='D';", $table_prefix);
			$this->MergeField[]=array('Naam' => "Dieet", 'SQL' => $sql);
		}
		
		if (db_memo("aantal", 0, "G") > 0) {
			$sql = sprintf("SELECT IF((Memo IS NULL), 'Geen', Memo) FROM %sMemo WHERE Lid=%%d AND Soort='G';", $table_prefix);
			$this->MergeField[]=array('Naam' => "MemoGezondheid", 'SQL' => $sql);
		}
		
		if (db_memo("aantal", 0, "I") > 0) {
			$sql = sprintf("SELECT IF(Memo IS NULL, 'Geen', Memo) FROM %sMemo WHERE Lid=%%d AND Soort='I';", $table_prefix);
			$this->MergeField[]=array('Naam' => "OpmerkingInschrijvingBewaking", 'SQL' => $sql);
		}
		
		$this->MergeField[]=array('Naam' => "Naamvereniging", 'SQL' => "");
		$this->MergeField[]=array('Naam' => "Naamwebsite", 'SQL' => "");
		$this->MergeField[]=array('Naam' => "URLwebsite", 'SQL' => "");
		
		// Velden die bij specifieke mailings horen.
		if ($this->mid == db_param("mailing_lidnr") and $this->mid > 0) {
			$this->xtrachar = "LNR";
			$this->speciaal = "Versturen lidnummer";
			$this->vertraging_tussen_verzenden = 2;
			$this->MergeField[]=array('Naam' => "Geblokkeerd", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "IPaddress", 'SQL' => "");
			
		} elseif ($this->mid == db_param("mailing_validatielogin") and $this->mid > 0) {
			$this->xtrachar = "VALLOGIN";
			$this->speciaal = "Validatie login";
			$this->vertraging_tussen_verzenden = 5;
			$this->MergeField[]=array('Naam' => "Geblokkeerd", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "IPaddress", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "GELDIGHEIDACTIVATIE", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "URLACTIVATIE", 'SQL' => "");
			
		} elseif ($this->mid == db_param("mailing_herstellenwachtwoord") and $this->mid > 0) {
			$this->xtrachar = "HERWW";
			$this->speciaal = "Herstellen wachtwoord";
			$this->vertraging_tussen_verzenden = 5;
			$this->MergeField[]=array('Naam' => "Geblokkeerd", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "IPaddress", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "GELDIGHEIDACTIVATIE", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "URLRESETWW", 'SQL' => "");
			
		} elseif ($this->mid == db_param("mailing_bewakinginschrijving") and $this->mid > 0) {
			$this->xtrachar = "BW_I";
			$this->speciaal = "Bevestiging inschrijving bewaking";
			$this->vertraging_tussen_verzenden = 5;
			$this->MergeField[]=array('Naam' => "VoorwaardenInschrijving", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "InschrijvingBewaking", 'SQL' => "");
			
		} elseif ($this->mid == db_param("mailing_bevestigingbestelling") and $this->mid > 0) {
			$this->xtrachar = "BEST";
			$this->speciaal = "Bevestiging bestelling webshop";
			$this->vertraging_tussen_verzenden = 5;
			$this->MergeField[]=array('Naam' => "Ordernummer", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "Bestelling", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "Totaalbedrag", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "VoorwaardenBestelling", 'SQL' => "");
			
		} elseif ($this->mid == db_param("mailing_bevestigingopzegging") and $this->mid > 0) {
			$this->xtrachar = "BO_LM";
			$this->speciaal = "Bevestiging opzegging lidmaatschap";
		}

		sort($this->MergeField);

		foreach ($this->MergeField as $fld) {
			$nm = "[%" . $fld['Naam'] . "%]";
			if (strpos($this->message, $nm) !== false) {
				$this->contains_mergefield = true;
			}
		}

	} # __construct
	
	public function lijst($filter="") {
	
		if (isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0 and $_GET['op'] == "bekijkmail") {
			$xtra = "<p class='mededeling'><input type='button' value='Terug' onclick='history.go(-1);'></p>\n";
			toonverstuurdemail($_GET['MailingHistID']);
		} elseif ($filter == "Verzonden mails" or $filter == "Outbox") {
			$ld = sprintf("<a href='index.php?tp=%s&amp;op=bekijkmail&amp;MailingHistID=%%d'>%%s</a>", urlencode($_GET['tp']));
			if (db_mailing_hist("aantal24uur") >= db_param("maxmailsperdag")) {
				$rs = "";
			} elseif (db_param("mailing_mailopnieuw") == 0 or (db_param("mailing_mailopnieuw") == 1 and $_SESSION['webmaster'] == 0)) {
				$rs = "";
			} elseif ($filter == "Outbox") {
				$rs = "";
			} else {
				$rs = sprintf("<a href='index.php?tp=%s&amp;op=opnieuw&amp;MailingHistID=%%d'><img src='images/sendmail.png' alt='send mail' title='Opnieuw versturen'></a>", urlencode($_GET['tp']));
			}
			if ($filter == "Outbox") {
				echo(fnDisplayTable(db_mailing_hist("outbox"), $ld, "", 0, $rs));
			} else {
				echo(fnDisplayTable(db_mailing_hist("lijst"), $ld, "", 0, $rs));
			}
		} else {
			$lnk = sprintf("<a href='%s?tp=%s&amp;op=edit&amp;mid=%%d' title='Bewerk mailing'><img src='images/edit.png' alt='Edit boek'></a>", $_SERVER['PHP_SELF'], $_GET['tp']);
			if ($filter == "Concepten") {
				$lnk_lk = "";
			} else {
				$lnk_lk = sprintf("<a href='%s?tp=Mailing/Historie&amp;op=historie&amp;mid=%%d' title='Bekijk historie'><img src='images/object_10.png' alt='Historie'></a>", $_SERVER['PHP_SELF']);
			}
			echo(fnDisplayTable(db_mailing("lijst", 0, $filter), $lnk, "", 0, $lnk_lk));
			if ($filter == "Prullenbak" and db_param("mailing_bewaartijd") > 0) {
				printf("<p>Mailings die langer dan %d maanden geleden verwijderd zijn, worden automatisch definitief opgeschoond.</p>\n", db_param("mailing_bewaartijd"));
			}
		}
	} # lijst
	
	public function edit() {
		global $currenttab, $currenttab2, $navpad, $fdlang;

		$ad = $this->dir_attachm . "/" . $this->mid;
		
		if ($this->mid == 0) {
			$currenttab2 = "Concepten";
		} else {
			$navpad[]['naam'] = sprintf("Details mailing %d", $this->mid);
		}
		
		$selectfrom = "";
		foreach (db_mailing_vanaf("lijst") as $row) {
			$selectfrom .= sprintf("<option value='%s'%s>%s</option>\n", $row->Vanaf_email, checked(strtolower($row->Vanaf_email), "option", strtolower($this->from_addr)), $row->Vanaf_naam);
		}
		
		printf("<div id='editmailingform'>\n");
		printf("<form method='post' action='%s?tp=%s/%s&amp;mid=%d' name='frm_mailedit' enctype='multipart/form-data'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2, $this->mid);
		
		printf("<p><label>Van:</label><select name='from_addr'>\n%s</select></p>\n", $selectfrom);
		printf("<p><label>Vanaf e-mail:</label><input type='email' value=\"%s\" size=50 readonly></p>\n", $this->from_addr);
		printf("<p><label>Aan:</label><input type='text' name='to_name' value=\"%s\" size=50 maxlength=50> (Omschrijving van de groep personen aan wie deze mailing gericht is)</p>\n", $this->to_name);
		printf("<p><label>Onderwerp:</label><input type='text' name='subject' value=\"%s\" size=100 maxlength=75></p>\n", $this->subject);
			
		$this->verzendenmag = false;	
		if ($this->mid > 0) {
			$rcpt_rows = db_mailing("rcpt", $this->mid);
			$this->aant_rcpt = count($rcpt_rows);
			if (strlen($this->speciaal) > 0) {
				printf("<label>Specifiek doel:</label><p>%s</p>\n", $this->speciaal);
				if ($this->aant_rcpt > 0) {
					$this->delete_all_rcpt();
				}
			
			} else {
				$this->meldingen = "Er zijn nog geen ontvangers aan deze mailing toegevoegd.<br>\n";
			
				printf("<label>Ontvangers (%d):</label>", $this->aant_rcpt);
				$aant_ze = 0;
				if ($this->aant_rcpt > 0) {
					echo("<div class='invoerblok'>\n");
					echo("<ul class='listrecipients'>\n");
					foreach($rcpt_rows as $rcpt) {
						$ze = "";
						if (!isValidMailAddress($rcpt->Email) and !isValidMailAddress($rcpt->EmailVereniging) and !isValidMailAddress($rcpt->to_address)) {
							$ze = " *";
							$aant_ze++;
						}
						if ($rcpt->LidID > 0) {
							$lnk = sprintf("%s?tp=%s&amp;op=del_rcpt&amp;mid=%d&amp;lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, $rcpt->LidID);
							printf("<li>%s&nbsp;<a href='%s'><img src='/images/delete.gif' alt='Verwijderen' title='Verwijderen'></a></li>\n", htmlentities($rcpt->Naam_lid . $ze), $lnk);
						} else {
							$lnk = sprintf("%s?tp=%s&amp;op=del_rcpt&amp;mid=%d&amp;addr=%s", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, $rcpt->to_address);
							printf("<li>%s&nbsp;<a href='%s'><img src='/images/delete.gif' alt='Verwijderen' title='Verwijderen'></a></li>\n", $rcpt->to_address, $lnk);
							$this->contains_losse_email = true;
						}
					}
					echo("</ul>\n");
					if ($aant_ze > 0) {
						printf("<span>* = zonder e-mailadres, totaal %d stuks</span>", $aant_ze);
					}
					echo("</div> <!-- Einde invoerblok -->\n");
					$this->verzendenmag = true;
					$this->meldingen = "";
				}
				echo("<label>Ontvanger toevoegen:</label>\n");
				printf("<p>Lid: <select name='add_lid' onChange='this.form.submit();'></p>\n");
				printf("<option value=0>&nbsp;</option>\n");
				$rcpt_rows = db_mailing("potrcpt", $this->mid);
				foreach($rcpt_rows as $rcpt) {
					$rnm = $rcpt->Zoeknaam_lid;
					$sl = substr(db_lidmaatschap("soortlid", $rcpt->Nummer), 0, 1);
					if (strlen($sl) > 0) {
						$rnm .= sprintf(" (%s)", $sl);
					}
					printf("<option value='%d'>%s</option>\n", $rcpt->Nummer, $rnm);
				}
				printf("</select>&nbsp;\n");
			
				echo("E-mail: <input type='email' name='add_address' size=50 maxlength=50 onBlur='this.form.submit();'>");
				echo("</p>\n");
				
				if (!isset($_POST['selectie_status'])) {
					$_POST['selectie_status'] = "L";
				}

				echo("<label>Selectie leden:</label>\n");
				echo("<div class='invoerblok'>\n");
				echo("<div id='mailingselectieleden'>\n");
				echo("<table>\n");
				echo("<tr><td></td><td>Vanaf</td><td>Tot en met</td></tr>\n");
			
				printf("<tr><td>Geboortedatum:</td><td><input type='date' value='%s' name='selectie_vangebdatum'></td><td><input type='date' value='%s' name='selectie_temgebdatum'></td></tr>\n", db_lid("mingebdatum"), db_lid("maxgebdatum"));
				printf("<tr><td>Postcode:</td><td><input type='text' name='selectie_vanpostcode' value='%s' max-length=7></td><td><input type='text' name='selectie_tempostcode' value='%s' max-length=7></td></tr>\n", db_lid("minpostcode"), db_lid("maxpostcode"));
				printf("<tr><td>Lid vanaf:</td><td><input type='date' name='selectie_vanliddatum' value='%s'></td><td><input type='date' name='selectie_temliddatum' value='%s'></td></tr>\n", db_lid("minliddatum"), db_lid("maxliddatum"));
				printf("<tr><td>Opgezegd per:</td><td><input type='date' name='selectie_vanopgezegd'></td><td><input type='date' name='selectie_temopgezegd' value='%s'></td></tr>\n", db_lid("maxopgezegd"));
			
				echo("<tr><td>Lidstatus:</td><td colspan=2><select name='selectie_status' onChange='this.form.submit();'>\n");
				printf("<option value='L'%s>Lid</option>", checked("L", "option", $_POST['selectie_status']));
				printf("<option value='V'%s>Voormalig lid</option>", checked("V", "option", $_POST['selectie_status']));
				if (count(db_lidmaatschap("toekomstigeleden")) > 0) {
					printf("<option value='T'%s>Toekomstig lid</option>", checked("T", "option", $_POST['selectie_status']));
				}
				printf("<option value='K'>Kloslid</option>", checked("K", "option", $_POST['selectie_status']));
				echo("</select></td></tr>\n");
			
				$selgr = "<select name='selectie_groep'>\n
							<option value=0>&nbsp;</option>\n";
				if ($_POST['selectie_status'] == "L") {
					$rows = db_onderdelen("lijstingebruik");
				} else {
					$rows = db_onderdelen("lijst");
				}
				foreach($rows as $grp) {
					$selgr .= sprintf("<option value=%d>%s</option>\n", $grp->RecordID, $grp->Naam);
				}
				$selgr .= "</select>\n";
				printf("<tr><td>Zit in groep:</td><td colspan=2>%s (bijgewerkt t/m %s)</td></tr>\n", $selgr, strtolower(date("j F Y", strtotime(db_lidond("maxgewijzigd", 0, "", "O.Type<>'T'")))));
				
				echo("</table>\n");
				echo("</div> <!-- Einde mailingselectieleden -->\n");
			
				echo("<input type='submit' name='LedenToevoegen' value='Leden toevoegen'>\n");
				if ($this->aant_rcpt > 0) {
					echo("<input type='submit' name='LedenVerwijderen' value='Leden verwijderen'>\n");
					echo("<input type='submit' name='OntvangersVerwijderen' value='Alle ontvangers verwijderen'>\n");
				}
				echo("</div>  <!-- Einde invoerblok -->\n");
				echo("<div class='clear'></div>\n");
			}
		}
		
		printf("<p><label>Cc:</label><input type='text' name='cc_addr' value='%s' size=100></p>\n", $this->cc_addr);
		printf("<p><label>Opties:</label><input type='checkbox' name='template' value=1%s>Template&nbsp;", checked($this->template));
		printf("<input type='checkbox' name='confidential' value=1%s>Vertrouwelijk&nbsp;", checked($this->confidential));
		
		echo("<div class='clear'></div>\n");
		
		
		printf("</div> <!-- Einde editmailingform -->\n");
		printf("<textarea name='message' id='editor'>%s</textarea>\n", $this->message);
		
		echo("<script>
    ClassicEditor
	.create( document.querySelector( '#editor' )	)
	.catch( error => {
		console.error( error );
	} );
</script>\n");
	
		$strHV = "";
		foreach ($this->MergeField as $v) {
			$strHV .= "<li>[%" . $v['Naam'] . "%]</li>\n";
		}
		$strHV .= "";
		printf("<div id='editmailingform'>\n");
		echo("<div class='clear'></div>\n");
		printf("<label>Beschikbare variabelen:</label><div class='invoerblok'><ul class='listmergefields'>\n%s</ul></div>\n", $strHV);
		
		if ($this->mid > 0) {
			echo("<label>Bijlagen:</label><div class='invoerblok'>");
			
			echo("<table>\n");
			if (is_dir($ad)) {
				$d = dir($ad);
				while (false !== ($entry = $d->read())) {
					if ($entry != "." and $entry != "..") {
						echo("<tr>\n<td>" . $entry . "</td>\n");
						$stat = stat($ad . "/" . $entry);
						echo("<td>" . number_format(($stat['size'] / 1024), 0, ',', '.') . " KB</td>\n");
						$lnk = sprintf("%s?tp=%s&op=del_attach&mid=%d&attach=%s", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, str_replace(".", "!", $entry));
						printf("<td><a href='%s'><img src='/images/delete.gif' alt='Verwijderen' title='Verwijder'></a></td>\n", $lnk);
					}
				}
				$d->close();
			}
			echo("</table>\n");

			echo("<input type='file' name='UploadFile' id='UploadFile' size='40'>\n");
			echo("<input type='submit' name='Upload' value='Upload'>&nbsp;");
			printf("(Extensies '%s' zijn toegestaan)\n", $this->allowed_ext);
			echo("</div>  <!-- Einde invoerblok -->\n");
		}
		
		$this->controle();
		if ($this->mid > 0) {
			if ($this->new_on > '1901-01-01') {
				printf("<label>Toegevoegd door/op:</label><p>%s / %s</p>\n", db_naamlid($this->AddedBy), strftime("%e %B %Y (%H:%M)", strtotime($this->new_on)));
			}
			if ($this->changed_on > '1901-01-01' and $this->ChangedBy > 0) {
				printf("<label>Gewijzigd door/op:</label><p>%s / %s</p>\n", db_naamlid($this->ChangedBy), strftime("%e %B %Y (%H:%M)", strtotime($this->changed_on)));
			}
			if ($this->sent_on > '1901-01-01' and $this->SentBy > 0) {
				printf("<label>Verzonden door/op:</label><p>%s / %s</p>\n", db_naamlid($this->SentBy), strftime("%e %B %Y (%H:%M)", strtotime($this->sent_on)));
			}
			if ($this->deleted_on > '1901-01-01') {
				printf("<label>Verwijderd door/op:</label><p>%s / %s</p>\n", db_naamlid($this->DeletedBy), strftime("%e %B %Y (%H:%M)", strtotime($this->deleted_on)));
			}

			if ($this->verzendenmag == false) {
				$this->meldingen .= " Deze mailing mag niet verzonden worden.";
			}
			if (strlen($this->meldingen) > 0) {
				printf("<label>Meldingen:</label><p>%s</p>\n", $this->meldingen);
			}
		}
		echo("<div class='clear'></div>\n");
		
		echo("<div id='opdrachtknoppen'>\n");
		if ($this->mid > 0) {
			echo("<button type='submit' name='action' value='save' title='Bewaren'><img src='images/save.png' alt='Save'></button>\n");
			echo("<input class='knop' type='submit' name='action' value='Bewaren & sluiten'>\n");
		} else {
			echo("<input class='knop' type='submit' name='action' value='Toevoegen'>");
		}
			
		if ($this->mid > 0 and $this->verzendenmag and $this->deleted_on < "2000-01-01") {
			printf("<button type='submit' name='action' value='send' title='Verstuur mailing'><img src='images/sendmail.png' alt='Send'></button>\n");
		}
		if ($this->mid > 0) {
			printf("<button type='submit' name='action' value='preview' title='Voorbeeld'><img src='images/preview.png' alt='Preview'></button>\n");
		}
		if ($this->mid > 0 and $this->deleted_on > "1901-01-01" and $_SESSION['webmaster'] == 1) {
			printf("<button type='submit' name='action' value='undelete' title='Verwijderen ongedaan maken'><img src='images/undelete.png' alt='Undelete'></button>\n");
		} elseif ($this->mid > 0 and $this->deleted_on < "1901-01-01" and ($_SESSION['webmaster'] == 1 or $_SESSION['lidid'] == $this->AddedBy)) {
			printf("<button type='submit' name='action' value='delete' title='Verwijderen'><img src='images/delete.png' alt='Delete'></button>\n");
		}
		echo("</div>  <!-- Einde opdrachtknoppen -->\n");

		printf("</form>\n");
		echo("</div>  <!-- Einde mailing -->\n");
	} # edit
	
	private function controle($enkellid=0) {
	
		if ($this->mid > 0) {
			if ($this->aant_rcpt == 0 and $enkellid == 0) {
				if (strlen($this->speciaal) > 0) {
					$this->meldingen = "Een mailing met een speciaal doel, kan niet los worden verzonden.<br>\n";
				} else {
					$this->meldingen = "Er zijn geen ontvangers aan deze mailing toegevoegd.<br>\n";
				}
				$this->verzendenmag = false;
			} else {
				$this->meldingen = "";
				$this->verzendenmag = true;
			}

			if (strlen($this->subject) == 0) {
				$this->meldingen .= "Het onderwerp is leeg en dat moet ingevuld zijn. \n";
				$this->verzendenmag = false;
			}
			if (strlen($this->message) == 0) {
				$this->meldingen .= "Het bericht is leeg en dat moet ingevuld zijn. \n";
				$this->verzendenmag = false;
			}
			if (strtotime($this->sent_on) > (time() - $this->vertraging_tussen_verzenden)) {
				if ($this->vertraging_tussen_verzenden < 210) {
					$this->meldingen .= sprintf("Er moet minimaal %d seconden zitten tussen het versturen van dezelfde mailing. \n", $this->vertraging_tussen_verzenden);
				} else {
					$this->meldingen .= sprintf("Er moet minimaal %d minuten zitten tussen het versturen van dezelfde mailing. \n", $this->vertraging_tussen_verzenden/60);
				}
				$this->verzendenmag = false;
				
			} elseif ((db_mailing_hist("aantal24uur") + db_mailing_rcpt("aantal", $this->mid)) >= db_param("maxmailsperdag")) {
				$this->meldingen .= sprintf("Het maximaal aantal van %d verzonden mails per dag wordt overschreden.", db_param("maxmailsperdag"));
				$this->verzendenmag = false;
			}
			
			foreach($this->MergeField as $fld) {
				$nm = "[%" . $fld['Naam'] . "%]";
				if (strpos($this->message, $nm) !== false) {
					$this->contains_mergefield = true;
				}
			}
			if ($this->contains_mergefield == true and $this->contains_losse_email == true) {
				$this->meldingen .= "Er staan losse e-mailadressen en er wordt met mailmerge gewerkt. Bij de losse e-mailadressen werkt de mailmerge niet. \n";
			}
			if ($this->deleted_on > '1901-01-01') {
				$this->meldingen .= "Deze mailing is verwijderd. \n";
				$this->verzendenmag = false;
			}
		} else {
			$this->verzendenmag = false;
			$this->meldingen = "Er is geen mailing geselecteerd.<br>\n";
		}
	}
	
	public function post_form() {
		global $table_prefix;

		if (isset($_POST['subject']) and strlen($_POST['subject']) > 1) {
			$this->subject = html_entity_decode(str_replace("\"", "'", $_POST['subject']));
			$_POST['subject'] = $this->subject;
		}
		
		if ($this->mid == 0) {
			$this->mid = db_mailing("add", 0, "", $this->subject);
		}
		
		if (isset($_POST['from_addr']) and strlen($_POST['from_addr']) > 5) {
			$row = db_mailing_vanaf("record", $_POST['from_addr']);
			$_POST['from_name'] = $row->Vanaf_naam;
		}
		
		if (isset($_POST['cc_addr']) and strlen($_POST['cc_addr']) > 5) {
			$_POST['cc_addr'] = str_replace(" ", "", $_POST['cc_addr']);
			$_POST['cc_addr'] = str_replace(";", ",", $_POST['cc_addr']);
		} else {
			$_POST['cc_addr'] = "";
		}
		
		if (isset($_POST['message']) and strlen($_POST['message']) > 1) {
			$this->message = html_entity_decode(str_replace("\"", "'", $_POST['message']));
			$_POST['message'] = $this->message;
		}
		
		if (isset($_POST['template'])) {
			$_POST['template'] = 1;
		} else {
			$_POST['template'] = 0;
		}
		if (isset($_POST['confidential'])) {
			$_POST['confidential'] = 1;
		} else {
			$_POST['confidential'] = 0;
		}
		
		foreach($_POST as $key => $val) {
			if (bestaatkolom("Mailing", $key)) {
				$mess = db_mailing("update", $this->mid, "", $val, $key);
				if (strlen($mess) > 0) {
					db_logboek("add", $mess, 4);
				}
			}
		}
		
		if (isset($_POST['add_lid']) and $_POST['add_lid'] > 0) {
			$this->add_rcpt($_POST['add_lid']);
		}
		
		if (isset($_POST['add_address']) and strlen($_POST['add_address']) > 5) {
			$this->add_rcpt(0, $_POST['add_address']);
		}
	} # post_form
	
	public function upload() {
	
		if (isset($_FILES['UploadFile']['name']) and strlen($_FILES['UploadFile']['name']) > 3 and $this->mid > 0) {
			$ad = $this->dir_attachm . "/" . $this->mid . "/";
			if (!file_exists($ad)) {
				if (mkdir($ad, 0755, true) == true) {
					$mess = sprintf("Directory '%s' is aangemaakt.", $ad);
				} else {
					$mess = sprintf("Directory '%s' bestaat niet en kan niet aangemaakt worden. Probeer het later opnieuw of neem contact op met de webmaster.", $ad);
					$ad = "";
				}
				db_logboek("add", $mess, 4, 0, 0, $this->mid);
			} else {
				chmod($ad, 0755);
			}

			$target = $ad . $_FILES['UploadFile']['name'];
			$ext = explode(".", $_FILES['UploadFile']['name']);  
			$ext = strtolower($ext[count($ext) - 1]);
			if (strpos($this->allowed_ext, $ext) === false) {
				$mess = sprintf("In mailing %d kan het bestand '%s' niet worden bijgesloten, omdat de extensie niet toegestaan is. Alleen de volgende extensies zijn toegestaan: %s", $this->mid, $_FILES['UploadFile']['name'], $this->allowed_ext);
			} elseif ($_FILES['UploadFile']['size'] > $this->max_size_attachm) {
				$mess = sprintf("In mailing %d kan het bestand '%s' niet worden bijgesloten, omdat het te groot is.", $this->mid, $_FILES['UploadFile']['name']);
			} elseif (strlen($ad) > 0) {
				if (move_uploaded_file($_FILES['UploadFile']['tmp_name'], $target) == false) {
					$mess = sprintf("Fout %d is opgetreden bij het uploaden van bestand '%s'. Probeer het later opnieuw of neem contact op met de webmaster.", $_FILES['UploadFile']['error'], $_FILES['UploadFile']['name']);
				} else {
					$mess = sprintf("In mailing %d (%s) is bestand '%s' bijgesloten.", $this->mid, $this->subject, $_FILES['UploadFile']['name']);
				}
			}
			db_logboek("add", $mess, 4, 0, 2, $this->mid);
		}
	}
	
	public function attach_delete($attach) {
		$ad = $this->dir_attachm . "/" . $this->mid . "/";
		unlink($ad . str_replace("!", ".", $attach));
	}
	
	public function add_rcpt($lidid, $addr="") {
		$mess = db_mailing_rcpt("add", $this->mid, $lidid, $addr);
		if (strlen($mess) > 0) {
			db_logboek("add", $mess, 4, 0, 0, $this->mid);
		}
	}

	public function delete_rcpt($lidid, $addr="") {
		$mess = db_mailing_rcpt("del", $this->mid, $lidid, $addr);
		db_logboek("add", $mess, 4, 0, 0, $this->mid);
	}
	
	public function add_del_selectie($actie) {
		global $table_prefix, $fromlid, $wherelid, $fromlidond, $wherelidond;
		
		if ($actie == "add") {
			$w = " AND (L.Overleden IS NULL) AND (L.Verwijderd IS NULL)";
		} else {
			$w = "";
		}
		
		$mess = "";
		
		if (strlen($_POST['selectie_vangebdatum']) == 10 and $_POST['selectie_vangebdatum'] > db_lid("mingebdatum") and $_POST['selectie_vangebdatum'] < db_lid("maxgebdatum")) {
			$w .= sprintf(" AND L.GEBDATUM >= '%s'", $_POST['selectie_vangebdatum']);
		} 
		if ($_POST['selectie_temgebdatum'] > "1900-01-01" and $_POST['selectie_temgebdatum'] < db_lid("maxgebdatum")) {
			$w .= sprintf(" AND L.GEBDATUM <= '%s'", $_POST['selectie_temgebdatum']);
		}
		if (strlen($_POST['selectie_vanpostcode']) >= 4 and $_POST['selectie_vanpostcode'] > db_lid("minpostcode")) {
			$w .= sprintf(" AND L.Postcode >= '%s'", $_POST['selectie_vanpostcode']);
		}
		if (strlen($_POST['selectie_tempostcode']) >= 4 and $_POST['selectie_tempostcode'] < db_lid("maxpostcode")) {
			$w .= sprintf(" AND L.Postcode <= '%s'", $_POST['selectie_tempostcode']);
		}
		
		if ($_POST['selectie_groep'] == 0) {
			if (strlen($_POST['selectie_vanliddatum']) == 10 and $_POST['selectie_vanliddatum'] > db_lid("minliddatum")) {
				$w .= sprintf(" AND LM.LIDDATUM >= '%s'", $_POST['selectie_vanliddatum']);
			}
			if (strlen($_POST['selectie_temliddatum']) == 10 and $_POST['selectie_temliddatum'] < db_lid("maxliddatum")) {
				$w .= sprintf(" AND LM.LIDDATUM <= '%s'", $_POST['selectie_temliddatum']);
			}
			if (strlen($_POST['selectie_vanopgezegd']) == 10) {
				$w .= sprintf(" AND LM.Opgezegd >= '%s'", $_POST['selectie_vanopgezegd']);
			}
			if (strlen($_POST['selectie_temopgezegd']) == 10 and $_POST['selectie_temopgezegd'] < db_lid("maxopgezegd")) {
				$w .= sprintf(" AND LM.Opgezegd <= '%s'", $_POST['selectie_temopgezegd']);
			}
			
			if ($_POST['selectie_status'] == "L") {
				$query = sprintf("SELECT DISTINCT L.Nummer FROM %s WHERE %s %s;", $fromlid, $wherelid, $w);
			} elseif ($_POST['selectie_status'] == "V") {
				$query = sprintf("SELECT DISTINCT L.Nummer FROM %s WHERE (NOT LM.Opgezegd IS NULL) %s;", $fromlid, $w);
			} elseif ($_POST['selectie_status'] == "T") {
				$query = sprintf("SELECT DISTINCT L.Nummer FROM %s WHERE LM.LIDDATUM > CURDATE() %s;", $fromlid, $w);
			} else {
				$query = sprintf("SELECT DISTINCT L.Nummer FROM %s WHERE (LM.Lid IS NULL) %s;", $fromlid, $w);
			}
			
		} else {
			if (strlen($_POST['selectie_vanliddatum']) == 10 and $_POST['selectie_vanliddatum'] > db_lid("maxliddatum") and $_POST['selectie_vanliddatum'] <= db_lid("maxliddatum")) {
				$w .= sprintf(" AND LO.Vanaf >= '%s'", $_POST['selectie_vangebdatum']);
			}
			if (strlen($_POST['selectie_temliddatum']) == 10 and $_POST['selectie_temliddatum'] >= db_lid("minliddatum") and $_POST['selectie_temliddatum'] < db_lid("maxliddatum")) {
				$w .= sprintf(" AND LO.Vanaf <= '%s'", $_POST['selectie_temgebdatum']);
			}
			if (strlen($_POST['selectie_vanopgezegd']) == 10 and $_POST['selectie_vanopgezegd'] > db_lid("minopgezegd")) {
				$w .= sprintf(" AND LO.Opgezegd >= '%s'", $_POST['selectie_vanopgezegd']);
			}
			if (strlen($_POST['selectie_temopgezegd']) == 10 and $_POST['selectie_temopgezegd'] < db_lid("maxopgezegd")) {
				$w .= sprintf(" AND LO.Opgezegd <= '%s'", $_POST['selectie_temopgezegd']);
			}

			if ($_POST['selectie_status'] == "V") {
				$query = sprintf("SELECT DISTINCT L.Nummer FROM %s WHERE LO.OnderdeelID=%d AND (NOT LO.Opgezegd IS NULL) %s;", $fromlidond, $_POST['selectie_groep'], $w);
			} elseif ($_POST['selectie_status'] == "T") {
				$query = sprintf("SELECT DISTINCT L.Nummer FROM %s WHERE LO.OnderdeelID=%d AND LO.Vanaf > CURDATE() %s;", $fromlidond, $_POST['selectie_groep'], $w);
			} else {
				$query = sprintf("SELECT DISTINCT L.Nummer FROM %s WHERE %s AND LO.OnderdeelID=%d %s;", $fromlidond, $wherelidond, $_POST['selectie_groep'], $w);
			}
		}
		
		$aant = 0;
		if ($actie == "add") {
			foreach(fnQuery($query) as $row) {
				if (strlen(db_mailing_rcpt("add", $this->mid, $row->Nummer)) > 0) {
					$aant++;
				}
			}
			if ($aant > 0) {
				$mess = sprintf("Er zijn %d leden, via een selectie, aan mailing %d (%s) toegevoegd.", $aant, $this->mid, $this->subject);
			}
		} else {
			foreach(fnQuery($query) as $row) {
				if (strlen(db_mailing_rcpt("del", $this->mid, $row->Nummer)) > 0) {
					$aant++;
				}
			}
			if ($aant > 0) {
				$mess = sprintf("Er zijn %d leden, via een selectie, aan mailing %d (%s) verwijderd.", $aant, $this->mid, $this->subject);
			}
		}
		db_logboek("add", $mess, 4, 0, 0, $this->mid);
		
	} # add_del_selectie
		
	public function delete_all_rcpt() {
		global $table_prefix;
	
		$query = sprintf("DELETE FROM %sMailing_rcpt WHERE MailingID=%d;", $table_prefix, $this->mid);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Alle %d ontvangers zijn uit mailing %d verwijderd.", $result, $this->mid);
			db_logboek("add", $mess, 4, 0, 0, $this->mid);
		}
	} # delete_all_rcpt
	
	public function preview() {
		$this->send_mailing(1);
	}	
	
	public function send($lidid=0) {
		
		$this->controle($lidid);
		if ($this->verzendenmag == false) {
			db_logboek("add", $this->meldingen, 4, 0, 1, $this->mid);
		} else {
			return $this->send_mailing(0, $lidid);
		}
	}
	
	public function folder() {

		if ($this->deleted_on > '2001-01-01') {
			return "Prullenbak";
		} elseif ($this->template == 1) {
			return "Templates";
		} elseif ($this->sent_on == "") {
			return "Concepten";
		} else {
			return "Verzonden";
		}
	}
	
	private function send_mailing($preview=0, $lidid=0) {
		global $table_prefix;
		
		$mail = new RBMmailer();
		$mail->From = $this->from_addr;
		$mail->FromName = $this->from_name;
		$mail->IsHTML(true);
		
		if ($preview == 0) {
			$ad = $this->dir_attachm . "/" . $this->mid;
			if (is_dir($ad)) {
				$d = dir($ad);
				while (false !== ($entry = $d->read())) {
					if ($entry != "." and $entry != "..") {
						$mail->AddAttachment($ad . "/" . $entry);
					}
				}
				$d->close();
			}
		}
		
		if ($lidid == 0) {
			$rcpts = db_mailing("rcpt", $this->mid);
		} else {
			$rcpts = db_lid("lijst", "", $lidid);
		}
		if ($preview == 1 and count($rcpts) == 0) {
			$this->merge($_SESSION['lidid']);
			$mail->Subject = $this->merged_subject;
			$mail->Body = $this->merged_message;
			$mail->addstationary($_SESSION['naamingelogde']);
			echo($mail->Body);
		}
		
		foreach($rcpts as $rcpt) {
			if ($rcpt->Nummer > 0) {
				$this->merge($rcpt->Nummer);
				$mail->Body = $this->merged_message;
				$mail->Subject = $this->merged_subject;
				$e = $mail->ToevoegenLid($rcpt->Nummer);
			} else {
				$mail->Body = $this->message;
				$mail->Subject = $this->subject;
				$mail->AddAddress($rcpt->to_address);
				$e = $rcpt->to_address;
				$this->contains_losse_email = true;
			}
			if (isset($rcpt->Naam_lid)) {
				$to_name = $rcpt->Naam_lid;
			} else {
				$to_name = $this->to_name;
			}
			$mail->addstationary($to_name);
			if ($preview == 1) {
				echo("<br>\n" . $mail->Body);
			}
			if (strlen($mail->ListAddresses()) > 0 and $preview == 0) {
				foreach (explode(",", $this->cc_addr) as $v) {
					if (isValidMailAddress($v, 0)) {
						$mail->AddCC($v);
					}
				}
				if (db_param("maxmailsperdag") > 0 and db_mailing_hist("aantal24uur") >= db_param("maxmailsperdag")) {
					db_add_mailing_hist($rcpt->Nummer, $this->mid, $to_name, "", $this->merged_message, $mail, 0, $this->xtrachar, $this->xtranum);
					$this->nok_send .= sprintf("<li>%s {%s}</li>\n", $rcpt->Naam_lid, $e);
					$mess = sprintf("Het versturen van mailing %d (%s) aan %s is niet gelukt. Error: het maximum aantal van %d verstuurde mails per dag is bereikt.", $this->mid, $this->subject, $to_name, db_param("maxmailsperdag"));
					db_logboek("add", $mess, 4, 0, 0, $this->mid);
					$this->aant_nok++;
					
				} elseif ($mail->Send()) {
					db_add_mailing_hist($rcpt->Nummer, $this->mid, $to_name, "", $this->merged_message, $mail, 1, $this->xtrachar, $this->xtranum);
					if (isset($rcpt->Naam_lid)) {
						$this->ok_send .= sprintf("<li>%s {%s}</li>\n", $rcpt->Naam_lid, $e);
					} else {
						$this->ok_send .= sprintf("<li>%s</li>\n", $e);
					}
					$this->aant_ok++;
				} else {
					db_add_mailing_hist($rcpt->Nummer, $this->mid, $to_name, "", $this->merged_message, $mail, 0, $this->xtrachar, $this->xtranum);
					$this->nok_send .= sprintf("<li>%s {%s}</li>\n", $rcpt->Naam_lid, $e);
					$mess = sprintf("Het versturen van mailing %d (%s) aan %s is niet gelukt. Error: %s", $this->mid, $this->subject, $to_name, $mail->ErrorInfo);
					db_logboek("add", $mess, 4, 0, 0, $this->mid);
					$this->aant_nok++;
				}
				$mail->clearAllRecipients();
			} else {
				$this->nok_send .= sprintf("<li>%s {%s, %s&nbsp&nbsp;%s}</li>\n", $rcpt->Naam_lid, $rcpt->Adres, $rcpt->Postcode, $rcpt->Woonplaats);
				$this->aant_nok++;
			}
		}
		$summary = "";
		
		if ($preview == 0) {
			if ($this->aant_ok > 1) {
				$mess = sprintf("Mailing %d (%s) is door %s aan %d ontvangers verzonden.", $this->mid, $this->subject, db_naamlid($_SESSION['lidid'], "", "gast"), $this->aant_ok);
			} else {
				$mess = sprintf("Mailing %d (%s) is door %s verzonden.", $this->mid, $this->subject, db_naamlid($_SESSION['lidid'], "", "gast"));
			}
			if ($_SESSION['lidid'] > 0) {
				db_logboek("add", $mess, 4, 0, 1, $this->mid);
			} else {
				db_logboek("add", $mess, 4, 0, 0, $this->mid);
			}
			db_mailing("updatesent", $this->mid);
		}
		
		if ($this->resultaatversturen == 1 and $preview == 0) {
			if (strlen($this->ok_send) > 1) {
				$summary = sprintf("<p>Deze mailing is verzonden op %s om %s naar %d ontvangers:<ul>\n%s</ul>\n</p>\n", strftime("%e %B %Y"), strftime("%H:%M"), $this->aant_ok, $this->ok_send);
			}
			if (strlen($this->nok_send) > 1) {
				$summary .= sprintf("Deze mailing kon niet naar %d ontvangers verzonden worden: <ul>\n%s</ul>\n", $this->aant_nok, $this->nok_send);
			}
			$mail->clearAllRecipients();
			$ev = "E-mail";
			$mail->Body = $this->message . $summary;
			$mail->Subject = sprintf("Resultaat mailing %d: %s", $this->mid, $mail->Subject);
			if (isValidMailAddress($mail->From, 0)) {
				$mail->AddAddress($mail->From, $mail->FromName);
				if (strtolower($_SESSION['emailingelogde']) != strtolower($mail->From)) {
					$mail->AddAddress($_SESSION['emailingelogde']);
				}
			} else {
				$mail->AddAddress($_SESSION['emailingelogde']);
			}
			if (isValidMailAddress($this->emailsecretariaat, 0)) {
				$mail->AddCC($this->emailsecretariaat);
			}
			$mail->addstationary();
			if ($mail->Send()) {
				db_add_mailing_hist(0, 0, "", "", $this->message, $mail, 1, "MLRES", $this->mid);
			}
		}
		
		return $this->aant_ok;
	} # send_mailing
	
	private function merge($lidid=0) {
		global $table_prefix;
		
		$this->merged_subject = str_ireplace("[%Naamvereniging%]", db_param("naamvereniging"), $this->subject);
		$this->merged_subject = str_ireplace("[%Naamwebsite%]", db_param("naamwebsite"), $this->merged_subject);
		$this->merged_subject = str_ireplace("[%URLwebsite%]", $_SERVER["HTTP_HOST"], $this->merged_subject);
		
		$this->merged_message = str_ireplace("[%Naamvereniging%]", db_param("naamvereniging"), $this->message);
		$this->merged_message = str_ireplace("[%Naamwebsite%]", db_param("naamwebsite"), $this->merged_message);
		$this->merged_message = str_ireplace("[%URLwebsite%]", $_SERVER["HTTP_HOST"], $this->merged_message);
		
		if (db_param("login_maxinlogpogingen") > 0 and db_logins("foutelogin", "", "", $lidid) > db_param("login_maxinlogpogingen")) {
			$geblokt = sprintf("Er is %d keer geprobeerd met deze login in te loggen. Hierdoor is dit account geblokkeerd. Vraag aan de webmaster om deze weer vrij te geven.", $db_logins("foutelogin", "", "", $lidid));
		} else {
			$geblokt = "";
		}
		$this->merged_message = str_ireplace("[%Geblokkeerd%]", $geblokt, $this->merged_message);
		$this->merged_message = str_ireplace("[%IPaddress%]", $_SERVER['REMOTE_ADDR'], $this->merged_message);
		$this->merged_message = str_ireplace("[%GELDIGHEIDACTIVATIE%]", intval(db_param("login_geldigheidactivatie")), $this->merged_message);
		
		if ($this->mid == db_param("mailing_validatielogin") and $lidid > 0) {
			$nk = db_logins("nieuweactivitiekey", "", "", $lidid);
			$urlactivatie = sprintf("%s/index.php?tp=Validatie+login&lidid=%d&key=%s", $_SERVER["HTTP_HOST"], $lidid, $key);
			$this->merged_message = str_replace("[%URLACTIVATIE%]", $urlactivatie, $this->merged_message);
		
		} elseif ($this->mid == db_param("mailing_herstellenwachtwoord") and $lidid > 0) {
			$nk = db_logins("wachtwoordreset", "", "", $lidid);
			$urlresetww = sprintf("%s/index.php?tp=Herstellen+wachtwoord&lidid=%d&key=%s", $_SERVER["HTTP_HOST"], $lidid, $nk);
			$this->merged_message = str_replace("[%URLRESETWW%]", $urlresetww, $this->merged_message);
			
		} elseif ($this->mid == db_param("mailing_bevestigingopzegging")) {
			if ($this->xtranum > 0) {
				$this->merged_message = str_ireplace("[%Lidnummer%]", $this->xtranum, $this->merged_message);
				$row = db_lidmaatschap("record", 0, "", 0, "", $this->xtranum);
				if (isset($row->OpgezegdPer)) {
					$this->merged_message = str_ireplace("[%OpgezegdPer%]", $row->OpgezegdPer, $this->merged_message);
				}
			}
			
		} elseif ($this->mid == db_param("mailing_bewakinginschrijving")) {
			if ($this->xtranum == 0 and $lidid > 0) {
				$this->xtranum = db_insbew("laatstelid", $lidid);
			}
			$rows = db_insbew("overzichtlid", 0, $this->xtranum);
			$bevins = "<ul>\n";
			foreach ($rows as $row) {		
				$bevins .= sprintf("<li>%s - %s t/m %s", $row->Seizoen, strftime("%e %B %Y", strtotime($row->dteBegin)), strftime("%e %B %Y", strtotime($row->dteEinde)));
				if ($row->Keuze == 2) {
					$bevins .= " (2de keuze)";
				}
				if (strlen($row->Opmerking) > 1) {
					$bevins .= sprintf(" (%s)", $row->Opmerking);
				}
				$bevins .= "</li>\n";
			}
			$bevins .= "</ul>\n";
			$this->merged_message = str_ireplace("[%VoorwaardenInschrijving%]", db_param("zs_voorwaardeninschrijving"), $this->merged_message);
			$this->merged_message = str_ireplace("[%InschrijvingBewaking%]", $bevins, $this->merged_message);
			
		} elseif ($this->mid == db_param("mailing_bevestigingbestelling")) {
			if ($this->xtranum > 0) {
				$filter = sprintf("ORD.Ordernr=%d", $this->xtranum);
			} else {
				$filter = "";
			}
			$bestelling = "<table>\n
								<tr><th>Code</th><th>Omschrijving</th><th>Maat</th><th class='number'>Aantal</th><th class='number'>Bedrag</th><th>Opmerking</th><tr>\n";
			
			$totbedrag = 0;
			foreach (db_orderregel("bevestiging", $lidid, 0, 0, $filter) as $row) {
				$ordernr = $row->Ordernr;
				$bestelling .= sprintf("<tr><td>%s</td><td>%s</td><td>%s</td><td class='number'>%d</td><td class='number'>&euro;&nbsp;%s</td><td>%s</td></tr>\n", $row->Code, $row->Omschrijving, $row->Maat, $row->AantalBesteld, number_format($row->Bedrag, 2), $row->Opmerking);
				$totbedrag += $row->Bedrag;
			}
			$bestelling .= "</table>\n";
			$this->merged_subject = str_ireplace("[%Ordernummer%]", $this->xtranum, $this->merged_subject);
			$this->merged_message = str_ireplace("[%Ordernummer%]", $this->xtranum, $this->merged_message);
			$this->merged_message = str_ireplace("[%Bestelling%]", $bestelling, $this->merged_message);
			$this->merged_message = str_ireplace("[%Totaalbedrag%]", number_format($totbedrag, 2), $this->merged_message);
			$this->merged_message = str_ireplace("[%VoorwaardenBestelling%]", db_param("zs_voorwaardenbestelling"), $this->merged_message);
			
		}
		
		if ($lidid > 0) {
			foreach($this->MergeField as $fld) {
				$nm = "[%" . $fld['Naam'] . "%]";
				if (stripos($this->merged_message, $nm) !== false or stripos($this->merged_subject, $nm) !== false and isset($fld['SQL']) and strlen($fld['SQL']) > 0) {
					$query  = str_replace("%d", $lidid, $fld['SQL']);
					$result = fnQuery($query);
					$nv = "";
					foreach($result->fetchAll(PDO::FETCH_NUM) as $val) {
						if (strlen($nv) > 0) { $nv .= ", "; }
						$nv .= htmlent($val[0]);
					}
					$this->merged_subject = str_ireplace($nm, $nv, $this->merged_subject);
					$this->merged_message = str_ireplace($nm, $nv, $this->merged_message);
					$this->contains_mergefield = true;
					if ($this->xtrachar == "LNR" and $nm == "[%Lidnummer%]" and $this->xtranum == 0) {
						$this->xtranum = $nv;
					}
				}
			}
		}
	}
	
	public function delete() {
		global $table_prefix;
	
		$query = sprintf("UPDATE %sMailing SET deleted_on=NOW(), DeletedBy=%d WHERE MailingID=%d AND (deleted_on IS NULL);", $table_prefix, $_SESSION['lidid'], $this->mid);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Mailing %d (%s) is naar de prullenbak verplaatst.", $this->mid, $this->subject, $_SESSION['naamingelogde']);
			db_logboek("add", $mess, 4, 0, 1, $this->mid);
		}
	}
	
	public function undelete() {
		global $table_prefix;
		
		$query = sprintf("UPDATE %sMailing SET deleted_on=NULL WHERE MailingID=%d;", $table_prefix, $this->mid);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Mailing %d (%s) is door %s uit de prullenbak gehaald.", $this->mid, $this->subject, $_SESSION['naamingelogde']);
			db_logboek("add", $mess, 4, 0, 1, $this->mid);
		}
	}
		
	private function vulvars() {
	

	} # vulvars
} # class Mailing

function toonverstuurdemail($mhid) {
	global $navpad;
		
	$navpad[]['naam'] = sprintf("Verzonden mail %d", $mhid);
	
	echo("<div id='verstuurdemail'>\n");
	$row = db_mailing_hist("record", 0, $mhid);
	if ($_SESSION['webmaster'] == 1 and $row->send_by > 0) {
		$d = " door " . db_naamlid($row->send_by);
	} else {
		$d = "";
	}
	if ($row->Successful == 1) {
		printf("<div class='label'>Verzonden:</div><div class='veld'>%s%s</div>\n", strftime('%e %B %Y (%H:%M:%S)', strtotime($row->send_on)), $d);
	} elseif ($row->send_on <= '2000-01-01') {
		printf("<div class='label'>Verzonden:</div><div class='veld'>wacht in Outbox</div>\n");
	} else {
		printf("<div class='label'>Verzonden:</div><div class='veld'>Geprobeerd op %s%s (zie logboek)</div>\n", strftime('%e %B %Y (%H:%M:%S)', strtotime($row->send_on)), $d);
	}
	if (strlen($row->from_name) > 0) {
		printf("<div class='label'>Van:</div><div class='veld'>%s (%s)</div>\n", htmlentities($row->from_name), $row->from_addr);
	} else {
		printf("<div class='label'>Van:</div><div class='veld'>%s</div>\n", $row->from_addr);
	}
	if (strlen($row->to_name) > 0) {
		printf("<div class='label'>Aan:</div><div class='veld'>%s</div>\n", htmlentities($row->to_name));
	}
	printf("<div class='label'>E-mail:</div><div class='veld'>%s</div>\n", $row->to_addr);
	printf("<div class='label'>CC:</div><div class='veld'>%s</div>\n", $row->cc_addr);
	printf("<div class='label'>Onderwerp:</div><div class='veld'>%s</div>\n", $row->subject);
	printf("<div class='label'>Bericht:</div><div class='veld'>%s</div>\n", $row->message);
	echo("</div>  <!-- Einde verstuurdemail -->\n");
} # toonverstuurdemail

function fnRekeningenMailen($op) {
	global $currenttab, $currenttab2, $table_prefix;

	echo("<div id='rekeningenmailen'>\n");
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if ($op == "selectierekeningen") {
			if (isset($_POST['reknr']) and is_numeric($_POST['reknr'])) {
				$_POST['minreknr'] = $_POST['reknr'];
				$_POST['maxreknr'] = $_POST['reknr'];
			}
			if (isset($_POST['rekdatum']) and strlen($_POST['rekdatum']) > 3) {
				$_POST['mindatum'] = $_POST['rekdatum'];
				$_POST['maxdatum'] = $_POST['rekdatum'];
			}
			$mindat = change_month_to_uk($_POST['mindatum']);
			$maxdat = change_month_to_uk($_POST['maxdatum']);
			if (strtotime($mindat) !== FALSE) {
				$mindat = date("Y-m-d", strtotime($mindat));
			} else {
				$mindat = db_rekening("mindatum");
			}
			if (strtotime($maxdat) !== FALSE) {
				$maxdat = date("Y-m-d", strtotime($maxdat));
			} else {
				$maxdat = db_rekening("maxdatum");
			}
			$filter = sprintf("RK.seizoen = %d AND RK.Nummer >= %d AND RK.Nummer <= %d AND RK.Datum >= '%s' AND RK.Datum <= '%s'", $_POST['rekseizoen'], $_POST['minreknr'], $_POST['maxreknr'], $mindat, $maxdat);
			if (isset($_POST['reklid']) and is_numeric($_POST['reklid']) and $_POST['reklid'] > 0) {
				$filter .= sprintf(" AND RK.Lid=%d", $_POST['reklid']);
			}
			if (!isset($_POST['nulrekeningen'])) {
				$filter .= " AND RK.Bedrag <> 0";
			}
			if (!isset($_POST['eerderverzonden'])) {
				$filter .= sprintf(" AND (RK.Nummer NOT IN (SELECT Xtra_Num FROM %sMailing_hist WHERE Xtra_Char='REK'))", $table_prefix);
			}
			if (!isset($_POST['geenemail'])) {
				$filter .= " AND (LENGTH(L.EMAIL) > 5 OR LENGTH(L.EmailVereniging) > 5 OR LENGTH(L.EmailOuders) > 5)";
			}
			$filter .= " AND (";
			if (isset($_POST['nietbetaald'])) {
				$filter .= "(RK.Betaald=0 AND RK.Bedrag <> 0)";
			}
			if (isset($_POST['deelbetaald'])) {
				if (isset($_POST['nietbetaald'])) {
					$filter .= " OR ";
				}
				$filter .= "(RK.Betaald > 0 AND RK.Bedrag > RK.Betaald)";
			}
			if (isset($_POST['volbetaald'])) {
				if (isset($_POST['nietbetaald']) or isset($_POST['deelbetaald'])) {
					$filter .= " OR ";
				}
				$filter .= "RK.Bedrag <= RK.Betaald";
			}
			$filter .= ")";
			
			if (isset($_POST['rekverstuuraan'])) {
				db_param("mailing_rekening_stuurnaar", "updval", $_POST['rekverstuuraan']);
			}
			
			if (isset($_POST['vanafnaam'])) {
				db_param("mailing_rekening_from_naam", "updval", $_POST['vanafnaam']);
			}
			
			if (isset($_POST['mailing_rekening_nulregelsweglaten']) and $_POST['mailing_rekening_nulregelsweglaten'] == 1) {
				$val = 1;
			} else {
				$val = 0;
			}
			db_param("mailing_rekening_nulregelsweglaten", "updval", $val);

			$selrek = "";
			$aantrek = 0;
			foreach(db_rekening("selectie", 0, $filter) as $rk) {
				$selrek .= sprintf("<li>%d - %s - &euro;&nbsp;%03.2f</li>\n", $rk->Nummer, $rk->DEBNAAM, $rk->Bedrag);
				$aantrek += 1;
			}
			
			if (db_param("maxmailsperdag") > 0 and db_mailing_hist("aantal24uur") + $aantrek > db_param("maxmailsperdag")) {
				printf("<p class='mededeling'>Er mogen maximaal %d mails per 24 uur worden verstuurd. Op dit moment zou dit over het maximum heen gaan.</p>", db_param("maxmailsperdag"));
			} elseif (strlen($selrek) > 0) {
				printf("<form method='post' action='%s?tp=%s/%s&amp;op=selectieversturen' name='mailrek'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
				echo("De volgende rekeningen zijn geselecteerd.\n");
				printf("<ul>\n%s</ul>\n<div class='clear'></div>\n", $selrek);
				printf("<input type='hidden' name='rekfilter' value=\"%s\">\n", $filter);
				echo("Rekeningen versturen?&nbsp<input type='checkbox' value='1' name='sure'>\n");
				echo("<input type='submit' value='Rekeningen versturen' name='StuurRek'>\n");
				echo("<input type='button' value='Terug' onClick='history.go(-1);'>\n");
				echo("</form>\n");
			} else {
				echo("<p class='mededeling'>Er zijn geen rekeningen geselecteerd.</p>");
			}
		} elseif ($op == "selectieversturen" and isset($_POST['sure'])) {
			$aant_ok = 0;
			$mail = new RBMmailer();
			$mail->IsHTML(true);
			if (strlen(db_param("mailing_rekening_from_adres")) > 0) {
				$mail->From = db_param("mailing_rekening_from_adres");
			}
			if (strlen(db_param("mailing_rekening_from_naam")) > 0) {
				$mail->FromName = db_param("mailing_rekening_from_naam");
			}
				
			foreach(db_rekening("selectie", 0, $_POST['rekfilter']) as $rk) {
				$mail->ToevoegenLid($rk->Lid);
				
				$rr = db_rekening("regels", $rk->Nummer);
				$rrtxt = "";
				
				if (db_param("mailing_rekening_stuurnaar") > 1) {
					$hd = new DateTime($rk->Datum);
					$hd->sub(new DateInterval('P18Y'));
					foreach($rr as $regel) {
						$nl = db_naamlid($regel->Lid, "Naam", "");
						if ($regel->GEBDATUM > '1900-01-01' and $regel->GEBDATUM <= $hd->format('Y-m-d')) {
							$mail->ToevoegenLid($regel->Lid);
						} elseif (db_param("mailing_rekening_stuurnaar") == 3) {
							$mail->ToevoegenLid($regel->Lid);
						}
					}
				}
				
				$mail->Subject = sprintf("Rekening %d - %s", $rk->Nummer, $rk->OMSCHRIJV);
				$rt = RekeningDetail($rk->Nummer);
				$mail->Body = $rt;
				if ($rk->Ouders == 1) {
					$dn = "Ouders/verzorgers van " . $rk->DEBNAAM;
				} else {
					$dn = $rk->DEBNAAM;
				}
				$mail->addstationary($dn, "", 1);
				if (strlen($mail->ListAddresses) == 0) {
					$mess = sprintf("Rekening %d kon niet worden verzonden, omdat er van %s geen geldig e-mailadres in de database bekend is.", $rk->Nummer, $dn);
					
				} elseif (db_param("maxmailsperdag") > 0 and db_mailing_hist("aantal24uur") > db_param("maxmailsperdag")) {
					db_add_mailing_hist($rk->Lid, 0, $dn, "", $rt, $mail, 0);
					$mess = sprintf("Het versturen van rekening %d naar %s is niet gelukt. Error: het maximum aantal te versturen mails per dag (%d) is bereikt.", $rk->Nummer, $dn, db_param("maxmailsperdag"));
					
				} elseif ($mail->Send()) {
					db_add_mailing_hist($rk->Lid, 0, $dn, "", $rt, $mail, 1, "REK", $rk->Nummer);
					
					$mess = sprintf("Rekening %d is naar %s (%s) verstuurd.", $rk->Nummer, $dn, $mail->ListAddresses);
					$aant_ok++;
				} else {
					db_add_mailing_hist($rk->Lid, 0, $dn, "", $rt, $mail, 0);
					$mess = sprintf("Het versturen van rekening %d naar %s is niet gelukt. Error: %s", $rk->Nummer, $dn, $mail->ErrorInfo);
				}
				db_logboek("add", $mess, 4, 0, 0, $rk->Nummer);
				$mail->clearAllRecipients();
			}
		} elseif ($op == "selectieversturen") {
			$mess = "De rekeningen zijn niet verzonden, omdat het vinkje niet gezet was.";
			db_logboek("add", $mess, 4, 0, 1);
		}
	}
		
	if ($_SERVER['REQUEST_METHOD'] != "POST" or $op == "selectieversturen") {
		printf("<form method='post' action='%s?tp=%s/%s&amp;op=selectierekeningen' name='mailrek'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
		echo("<table>\n");
		echo("<tr><th></th><th>Gelijk aan</th><th>Vanaf</th><th>Totenmet</th></tr>");
		echo("<tr><td class='label'>Seizoen</th><td><select name='rekseizoen'>\n");
		foreach(db_rekening("seizoenen") as $row) {
			printf("<option value='%1\$d'>%1\$d</option>\n", $row->Seizoen);
		}
		echo("</select></td><td></td><td></td></tr>\n");
		printf("<tr><td class='label'>Rekeningnummer</th><td><input type='number' name='reknr'></td><td><input type='number' value=%d name='minreknr'></td><td><input type='number' value=%d name='maxreknr'></td></tr>\n", db_rekening("minnummer"), db_rekening("maxnummer"));
		printf("<tr><td class='label'>Rekeningdatum</th><td><input type='date' name='rekdatum'></td><td><input type='date' value='%s' name='mindatum'></td><td><input type='date' value='%s' name='maxdatum'></td></tr>\n", db_rekening("mindatum"), db_rekening("maxdatum"));			
		echo("<tr><td class='label'>Betaald door (debiteur)</th><td><select name='reklid'>\n<option value='-1'>*** Iedereen ***</option>>\n");
		foreach(db_lid("ledenmetrekening") as $row) {
			printf("<option value=%d>%s</option>\n", $row->Nummer, $row->Naam_lid);
		}
		echo("</select></td><td></td><td></td></tr>\n");
		echo("<tr><td class='label'>Statussen</th><td colspan=3>");
		echo("<input type='checkbox' name='volbetaald' value=1 checked>Volledig betaald\n");
		echo("<input type='checkbox' name='deelbetaald' value=1 checked>Gedeeltelijk betaald\n");
		echo("<input type='checkbox' name='nietbetaald' value=1 checked>Niet betaald\n");
		echo("<input type='checkbox' name='nulrekeningen' value=1 checked>Nul rekeningen\n");
		echo("<input type='checkbox' name='eerderverzonden' value=1 checked>Eerder verzonden\n");
		echo("<input type='checkbox' name='geenemail' value=1>Geen e-mailadres\n");
		
		echo("<tr><th colspan=4>Instellingen</th></tr>\n");
		echo("<tr><td class='label'>Rekening versturen aan:</th><td colspan=3><select name='rekverstuuraan'>\n");
		$sn[1] = "Alleen betaald door (debiteur)";
		$sn[2] = "Betaald door en alle volwassenen op de rekening";
		$sn[3] = "Betaald door en alle leden op de rekening";
		foreach($sn as $key => $val) {
			$s = "";
			if (db_param("mailing_rekening_stuurnaar") == $key) {
				$s = " selected";
			}
			printf("<option value=%d%s>%s</option>>\n", $key, $s, $val);
		}
		echo("</select></td></tr>\n");
		
		printf("<tr><td class='label'>Vanaf e-mailadres<td colspan=3><input name='vanafadres' type='email' value='%s' size=50></td></tr>\n", db_param("mailing_rekening_from_adres"));
		printf("<tr><td class='label'>Vanaf naam<td colspan=3><input name='vanafnaam' type='text' value='%s' size=50></td></tr>\n", db_param("mailing_rekening_from_naam"));
		printf("<tr><td class='label'>Nulregels weglaten<td colspan=3><input name='mailing_rekening_nulregelsweglaten' type='checkbox'%s value=1></td></tr>\n", checked(db_param("mailing_rekening_nulregelsweglaten")));
		
		echo("</td></tr>\n");
		echo("<tr><th colspan=4><input type='submit' value='Verder'></th></tr>\n");
		echo("</table>\n");
		echo("</form>\n");
	}
	echo("</div> <!== Einde rekeningenmailen -->\n");
} # fnRekeningenMailen

function fnMailingInstellingen() {
	global $currenttab, $currenttab2;
	
	echo("<div id='instellingenmuteren'>\n");
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		if (isset($_POST['mailing_resultaatversturen']) and $_POST['mailing_resultaatversturen'] == 1) {
			$val = 1;
		} else {
			$val = 0;
		}
		db_param("mailing_resultaatversturen", "updval", $val);
		
		db_param("mailing_mailopnieuw", "updval", $_POST['mailing_mailopnieuw']);
		
		if (strlen($_POST['mailing_bewaartijd']) == 0) {
			$_POST['mailing_bewaartijd'] = 0;
		}
		db_param("mailing_bewaartijd", "updval", $_POST['mailing_bewaartijd']);
		
		if (strlen($_POST['maxmailsperminuut']) == 0) {
			$_POST['maxmailsperminuut'] = 0;
		}
		db_param("maxmailsperminuut", "updval", $_POST['maxmailsperminuut']);
		
		if (strlen($_POST['maxmailsperdag']) == 0) {
			$_POST['maxmailsperdag'] = 0;
		}
		db_param("maxmailsperdag", "updval", $_POST['maxmailsperdag']);
		db_param("mailing_extensies_toegestaan", "updval", $_POST['mailing_extensies_toegestaan']);
		db_param("mailing_lidnr", "updval", $_POST['mailing_lidnr']);
		db_param("mailing_validatielogin", "updval", $_POST['mailing_validatielogin']);
		db_param("mailing_herstellenwachtwoord", "updval", $_POST['mailing_herstellenwachtwoord']);
		db_param("mailing_bevestigingopzegging", "updval", $_POST['mailing_bevestigingopzegging']);
		
		$kolomnamen = array("Vanaf_email", "Vanaf_naam", "SMTP_server");
		foreach ($_POST as $contr_name => $contr_val) {
			if (startwith($contr_name, "delete_vanaf")) {
				$rid = intval(str_replace("delete_vanaf_", "", $contr_name));
				db_mailing_vanaf("delete", "", $rid);
			} elseif ($contr_name == "vanaf_adres_toevoegen") {
				db_mailing_vanaf("add");
			} else {
				foreach ($kolomnamen as $kolomnaam) {
					if (startwith($contr_name, $kolomnaam . "_")) {
						$rid = intval(str_replace($kolomnaam . "_", "", $contr_name));
						db_mailing_vanaf("update", $contr_val, $rid, $kolomnaam);
					}
				}
			}
		}
	}
	printf("<form method='post' action='%s?tp=%s/%s' name='mailinstellingen'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
	
	$options = sprintf("<option value=0%s>Nee</option>\n", checked(db_param("mailing_mailopnieuw"), "option", 0));
	$options .= sprintf("<option value=1%s>Alleen door webmasters</option>\n", checked(db_param("mailing_mailopnieuw"), "option", 1));
	$options .= sprintf("<option value=2%s>Ja</option>\n", checked(db_param("mailing_mailopnieuw"), "option", 2));
	
	echo("<h2>Algemeen</h2>\n");
	printf("<label>Mag een verzonden e-mail opnieuw worden verstuurd?</label><p><select name='mailing_mailopnieuw'>%s</select></p>\n", $options);
	printf("<label>Versturen resultaat van de mailing naar de afzender en secretariaat:</label><p><input type='checkbox' value=1 name='mailing_resultaatversturen'%s></p>\n", checked(db_param("mailing_resultaatversturen")));
	printf("<label>Hoeveel maanden moeten mailings in de prullenbak bewaard blijven:</label><p><input class='inputnumber' type='number' name='mailing_bewaartijd' value=%d size=8> (0 = altijd bewaren)</p>\n", db_param("mailing_bewaartijd"));
	printf("<label>Hoeveel mails mogen er per minuut verstuurd worden:</label><p><input class='inputnumber' type='number' name='maxmailsperminuut' value=%d size=8> (0 = onbeperkt)</p>\n", db_param("maxmailsperminuut"));
	printf("<label>Hoeveel mails mogen er per 24 uur verstuurd worden:</label><p><input class='inputnumber' type='number' name='maxmailsperdag' value=%d size=8> (0 = onbeperkt)</p>\n", db_param("maxmailsperdag"));
	printf("<label>Welke extensies zijn als attachment toegestaan:</label><p><input class='inputtext' type='text' name='mailing_extensies_toegestaan' value='%s' size=30> (leeg = standaard lijst)</p>\n", db_param("mailing_extensies_toegestaan"));
	
	echo("<h2>Mailings met een specifiek doel</h2>\n");
	
	$rows = db_mailing("lijst", 0, "Templates");
	$options = "<Option value=0>Geen</option>";
	foreach($rows as $row) {
		$options .= sprintf("<Option%s value=%d>%s</option>", checked($row->lnkMailingID, "option", db_param("mailing_lidnr")), $row->lnkMailingID, $row->Onderwerp);
	}
	$rows = null;
	printf("<label>Versturen lidnummer:</label><p><select name='mailing_lidnr'>%s</select></p>\n", $options);
	
	$rows = db_mailing("lijst", 0, "Templates");
	$options = "";
	foreach($rows as $row) {
		$options .= sprintf("<Option%s value=%d>%s</option>", checked($row->lnkMailingID, "option", db_param("mailing_validatielogin")), $row->lnkMailingID, $row->Onderwerp);
	}
	$rows = null;
	printf("<label>Versturen validatie e-mail login:</label><p><select name='mailing_validatielogin'>%s</select></p>\n", $options);
	
	$rows = db_mailing("lijst", 0, "Templates");
	$options = "";
	foreach($rows as $row) {
		$options .= sprintf("<Option%s value=%d>%s</option>", checked($row->lnkMailingID, "option", db_param("mailing_herstellenwachtwoord")), $row->lnkMailingID, $row->Onderwerp);
	}
	$rows = null;
	printf("<label>Versturen link herstellen wachtwoord:</label><p><select name='mailing_herstellenwachtwoord'>%s</select></p>\n", $options);
	
	$rows = db_mailing("lijst", 0, "Templates");
	$options = "<Option value=0>Geen</option>";
	foreach($rows as $row) {
		$options .= sprintf("<Option%s value=%d>%s</option>", checked($row->lnkMailingID, "option", db_param("mailing_bevestigingopzegging")), $row->lnkMailingID, $row->Onderwerp);
	}
	$rows = null;
	printf("<label>Versturen bevestiging opzegging lidmaatschap:</label><p><select name='mailing_bevestigingopzegging'>%s</select></p>\n", $options);
	
	echo("<div class='clear'></div>\n");
	
	$link_lk = "<button type='submit' name='delete_vanaf_%d' title='Verwijder vanaf adres %d' onclick='this.submit();'><img src='./images/del.png'></button>";
	$xr = "<td colspan=6><button type='submit' name='vanaf_adres_toevoegen' title='Vanaf adres toevoegen' value='Vanaf adres toevoegen'><img src='./images/star.png'></button></td>";
	
	echo(fnDisplayTable(db_mailing_vanaf("editlijst"), "", "Vanaf adressen", 0, $link_lk, $xr, "editmailingvanaf", "", "Vanaf_naam, emlVanaf_email"));
	
	echo("<div class='clear'></div>\n");
	echo("<input class='knop' type='submit' value='Bewaren' name='InstellingenBewaren'>\n");
	echo("</form>");
	echo("</div> <!-- Einde instellingenmuteren -->\n");
	
} # fnMailingInstellingen

function sendfromhist($mhid, $opnieuw=0) {
	
	$row = db_mailing_hist("record", 0, $mhid);
	
	$mail = new RBMmailer($row->LidID);
	$mail->IsHTML(true);
	if ($row->LidID == 0) {
		foreach (explode(",", $row->to_addr) as $e) {
			if (isValidMailAddress($e)) {
				$mail->AddAddress($e);
			}
		}
		foreach (explode(",", $row->cc_addr) as $e) {
			if (isValidMailAddress($e)) {
				$mail->AddCC($e);
			}
		}
	}
	$mail->From = $row->from_addr;
	$mail->FromName = $row->from_name;
	if ($opnieuw == 1 and strpos($row->subject, "opnieuw verzonden") == false) {
		$mail->Subject = $row->subject . " (opnieuw verzonden)";
	} else {
		$mail->Subject = $row->subject;
	}
	$mail->Body = $row->message;
	$mail->addstationary($row->aan);
	if ($mail->Send()) {
		db_mailing_hist("updatesent", 0, $mhid, 0, $mail);
		$mess = sprintf("Mail '%s' is opnieuw verstuurd.", $mail->Subject);
	} else {
		$mess = sprintf("Het versturen van mail '%s' is niet gelukt.", $mail->Subject);
	}
	db_logboek("add", $mess, 4, 0, 1);
	
	$mail = null;
	$row = null;
	
} # sendfromhist

class RBMmailer extends PHPMailer\PHPMailer\PHPMailer {

	private $bestand_briefpapier = "templates/briefpapier.html";

	function __construct($lidid=-1) {
		global $table_prefix, $selectnaam;
		global $smtphost, $smtpport, $smtpuser, $smtppw;
		
		if (!isset($smtphost)) {
			$mess = "Er is geen SMTP host in config.php gedefinieerd. Indien u geen SMTP wilt gebruiker, maak de variabele dan leeg.";
			db_logboek("add", $mess, 4);
			$this->IsMail(true);
			$this->IsSMTP(false);
		} elseif (strlen($smtphost) > 0) {
			$this->Host = $smtphost;
			if ($smtpport > 0) {
				$this->Port = $smtpport;
				if ($smtpport == 587) {
					$this->SMTPSecure = "tls";
				}
			}
			$this->IsSMTP(true);
			if (strlen($smtpuser) > 0) {
				$this->SMTPAuth = true;
				$this->Username = $smtpuser;
				$this->Password = $smtppw;
			} else {
				$this->SMTPAuth = false;
			}
			$this->SMTPdebug = 2;
		} else {
			$this->IsMail(true);
		}
		$this->CharSet = "UTF-8";
		$this->From = db_param("emailwebmaster");
		$this->FromName = db_param("naamvereniging");
		$this->WordWrap = 110;
		
		if ($lidid > 0) {
			$this->ToevoegenLid($lidid);
		}
	}
	
	public function ToevoegenLid($lidid) {
	
		$rv = "";
		$row = db_lid("record", "", $lidid);
		if (strlen($row->Email) > 5 and self::ValidateAddress($row->Email)) {
			if (in_array($row->Email, $this->to) === false) {
				$this->AddAddress($row->Email, $row->NaamLid);
				$rv = $row->Email;
			}
		} elseif (strlen($row->EmailVereniging) > 5 and self::ValidateAddress($row->EmailVereniging) and in_array($row->EmailVereniging, $this->to) === false and in_array($row->EmailVereniging, $this->cc) === false) {
			$this->AddAddress($row->EmailVereniging, $row->NaamLid);
			$rv = $row->EmailVereniging;
		}
		
		$e = $this->InformerenOuders($row->GEBDATUM, $row->EmailOuders);
		if (strlen($e) > 5) {
			if (strlen($e) > 5) {
				$rv .= ", " . $e;
			} else {
				$rv = $e;
			}
		}
		return $rv;
	}
	
	public function InformerenOuders($geboren, $emailouders) {
		if (count($this->to) == 0) {
			$kind = "to";
		} else {
			$kind = "cc";
		}
		
		$rv = "";
		if ($geboren > date("Y-m-d", mktime(0, 0, 0, date("m"), date("d"), date("Y")-18)) and strlen($emailouders) > 5) {
			$emailouders = str_replace(" ", "", str_replace(";", ",", $emailouders));
			foreach(explode(",", $emailouders) as $e) {
				if (self::ValidateAddress($e) and in_array($e, $this->to) === false and in_array($e, $this->cc) === false) {
					$this->AddAnAddress($kind, $e);
					if (strlen($rv) > 0) {
						$rv .= ", ";
					}
					$rv .= $e;
				}
			}
		}
		return $rv;
	}
	
	public function ListAddresses($kind="to") {
		if (count($this->$kind) > 0) {
			$rv = "";
			foreach ($this->$kind as $e) {
				$rv .= $e[0] . ",";
			}
			return substr($rv, 0, -1);
		} else {
			return "";
		}
	}
	
	public function hasstationary() {
		if (file_exists($this->bestand_briefpapier)) {
			return true;
		} else {
			return false;
		}
	}
	
	public function addstationary($to="", $from="", $verwijderaanhef=0) {
		
		if (strlen($to) == 0) {
			foreach($this->to as $e) {
				if (strlen($to) > 0) {
					$to .= ", ";
				}
				$to .= $e[1];
			}
		}
		if (strlen($from) == 0) {
			$from = $this->FromName;
		}
		if (file_exists($this->bestand_briefpapier)) {
			if ($verwijderaanhef == 1) {
				$htmlmessage = removetextblock(file_get_contents($this->bestand_briefpapier), "<!-- Aanhef -->", "<!-- /Aanhef -->");
			} else {
				$htmlmessage = file_get_contents($this->bestand_briefpapier);
			}
			$htmlmessage = str_ireplace("[%MESSAGE%]", $this->Body, $htmlmessage);
			$htmlmessage = str_ireplace("[%FROM%]", $from, $htmlmessage);
			$htmlmessage = str_ireplace("[%TO%]", $to, $htmlmessage);
			$htmlmessage = str_ireplace("[%SUBJECT%]", $this->Subject, $htmlmessage);
			$this->Body = $htmlmessage;
		} else {
			$mess = sprintf("Het bestand '%s' bestaat niet.", $this->bestand_briefpapier);
			db_logboek("add", $mess, 4);
		}
	}
	
	public function Send() {
				
		if (db_param("maxmailsperminuut") > 0) {
			while ((microtime(true)-db_mailing("lastsendtime")) < (60 / db_param("maxmailsperminuut"))) {
				usleep((60000000 / db_param("maxmailsperminuut"))+50);
			}
			set_time_limit(60);
		}
		
		if (db_param("maxmailsperdag") > 0 and db_mailing_hist("aantal24uur") >= db_param("maxmailsperdag")) {
			$mess = sprintf("Het maximaal aantal van %d te versturen mail per dag is bereikt.", db_param("maxmailsperdag"));
			db_logboek("add", $mess, 4, 0, 1);
			return false;
			
		} else {
			try {
				if(!$this->PreSend()) return false;
				return $this->PostSend();
			} catch (phpmailerException $e) {
				$this->SentMIMEMessage = '';
				$this->SetError($e->getMessage());
				if ($this->exceptions) {
					throw $e;
				}
				return false;
			}
		}
	}
} # RBM_mailer

?>
