<?php

function fnMailing() {
	global $ldl, $currenttab, $currenttab2;

	if (!isset($_GET['mid'])) {
		$_GET['mid'] = 0;
	}
	if (!isset($_GET['mhid'])) {
		$_GET['mhid'] = 0;
	}
	if (isset($_GET['op'])) {
		$op = $_GET['op'];
	} else {
		$op = "";
	}
	
	if ($_SESSION['settings']['mailing_direct_verzenden'] == 1) {
		sentoutbox(2);
	}
	
	if (isset($_POST['UploadBijlage']) or isset($_POST['UploadMedia'])) {
		$op = "upload";
	} elseif ($_GET['tp'] == "Mailing/Historie" and $op != "histdetails" and $op != "opnieuw") {
		$op = "historie";
	} elseif (isset($_POST['action']) and $_POST['action'] == "Bewaren") {
		$op = "save";
	} elseif (isset($_POST['action']) and $_POST['action'] == "Bewaren & sluiten") {
		$op = "save_close";
	} elseif (isset($_POST['action']) and $_POST['action'] == "Verwijderen") {
		$op = "verwijderen";
	} elseif (isset($_POST['action']) and $_POST['action'] == "Inhoud versturen") {
		$op = "sentoutbox";
	} elseif (isset($_POST['LedenToevoegen']) and $_POST['LedenToevoegen'] == "Leden toevoegen") {
		$op = "add_selectie";
	} elseif (isset($_POST['LedenVerwijderen']) and $_POST['LedenVerwijderen'] == "Leden verwijderen") {
		$op = "del_selectie";
	} elseif (isset($_POST['OntvangersVerwijderen']) and $_POST['OntvangersVerwijderen'] == "Alle ontvangers verwijderen") {
		$op = "delete_all_rcpt";
	} elseif ($_SERVER['REQUEST_METHOD'] == "POST") {
		$op = "save";
	}

	$mailing = new Mailing($_GET['mid']);
	
		if ($op == "upload") {
			$mailing->upload();
		} elseif ($op == "del_attach" and isset($_GET['attach'])) {
			$mailing->attach_delete($_GET['attach'], "bijlage");
		} elseif ($op == "htmlmedia" and isset($_GET['attach'])) {
			$mailing->htmlmedia($_GET['attach']);
		} elseif ($op == "del_media" and isset($_GET['attach'])) {
			$mailing->attach_delete($_GET['attach'], "media");
		}
	
	if ($op != "preview") {
		HTMLheader();
	}
	
	if (($op == "historie" or $op == "histdetails") and $mailing->mid > 0) {
		fnDispMenu(2, sprintf("mid=%d", $mailing->mid));
	} elseif ($op != "preview") {
		fnDispMenu(2);
	}
	
//	echo("<h2>Werk in uitvoering. Graag niet gebruiken.</h2>");

	$f = sprintf("RecordID=%d", $_GET['mid']);
	$zv = (new cls_Mailing())->max("ZichtbaarVoor", $f);
	$lg = explode(",", $_SESSION['lidgroepen']);
	if ($_SESSION['webmaster'] == 0 and $zv != 0 and in_array($zv, $lg) === false) {
		$mess = "Je hebt geen rechten om deze mailing te bekijken of te bewerken.";
		printf("<h2>%s</h2>", $mess);
		debug($mess, 1, 0, 1);
		
	} elseif ($currenttab2 == "Nieuw" and toegang($_GET['tp'], 1, 1)) {
		$mailing->mid = 0;
		$mailing->edit();
		
	} elseif ($currenttab2 == "Instellingen" and toegang($_GET['tp'], 1, 1)) {
		fnMailingInstellingen();
		
	} elseif ($op == "opnieuw" and isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0) {
		sentfromhist($_GET['MailingHistID']);
		echo("<script>window.history.back();</script>\n");
		
	} elseif ($op == "sentmail" and isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0) {
		sentfromhist($_GET['MailingHistID'], 1);
		$mailing->lijst($currenttab2);
				
	} elseif ($op == "deletehist" and isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0) {
		(new cls_Mailing_hist())->delete($_GET['MailingHistID']);
		echo("<script>window.history.back();</script>\n");
		
	} elseif ($currenttab2 == "Rekeningen" and toegang($_GET['tp'], 1, 1)) {
		fnRekeningenMailen($op);
		
	} elseif ($currenttab2 == "Logboek" and toegang($_GET['tp'], 1, 1)) {
		$lijst = (new cls_Logboek())->lijst(4, 1);
		echo(fnDisplayTable($lijst));
		
	} elseif ($op == "historie" and $_GET['mid'] > 0) {
		$rows = (new cls_Mailing_hist())->lijst($_GET['mid']);
		if (count($rows) > 0) {
			$ld = sprintf("<a href='index.php?tp=%s&amp;op=histdetails&amp;mhid=%%d'>%%s</a>", $_GET['tp']);
			$th = sprintf("%d. %s", $mailing->mid, $rows[0]->ndOnderwerp);
			echo(fnDisplayTable($rows, $ld, $th, 0, 1));
		} else {
			echo("<p class='mededeling'>Deze mailing heeft geen historie.</p>\n");
		}
		
	} elseif ($op == "histdetails" and $_GET['mhid'] > 0) {
		$email = new email($_GET['mhid']);
		$email->toon();
		$mail = null;

	} elseif ($op == "sentoutbox") {
		sentoutbox();
		$mailing->lijst($currenttab2);
	} elseif ($op == "preview") {
		$mailing->preview();
	} elseif ($op == "send") {
		$mailing->post_form();
		$mailing->send();
		$mailing->lijst($currenttab2);
	} elseif ($op == "verwijderen") {
		$mailing->delete();
		$mailing->lijst("Muteren");
	} elseif ($op == "verwijderen ongedaan maken") {
		$mailing->undelete();
		$mailing->lijst("Muteren");
	} elseif ($currenttab2 == "Wijzigen mailing") {
		$mailing->post_form($op);
		if ($op == "Upload") {
			printf("<script>location.href='%s?%s';</script>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
		} elseif ($op == "save_close") {
			printf("<script>location.href='%s?tp=Mailing/Muteren';</script>\n", $_SERVER['PHP_SELF']);
		} elseif (isset($_POST['toevoegen'])) {
			printf("<script>location.href='%s?tp=Mailing/Wijzigen mailing&mid=%d';</script>\n", $_SERVER['PHP_SELF'], $mailing->mid);
		} else {
			$mailing->edit();
		}
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
	private $allowed_ext_afb = "bmp, gif, jpeg, jpg, mp3, mp4, png";
	private $dir_attachm = "";
	public $dir_media = "";
	private $max_size_attachm = 2097152; // 2MB
	private $vertraging_tussen_verzenden = 30; // de minimale tijd (in seconden) die er tussen het verzenden van dezelfde mailing moet zitten.
	private $smtpserver = 0; // De details van de server moeten in config.php gespecificeerd worden.
	
	private $MergeField;
	private $contains_mergefield = false;
	private $bevat_losse_email = false;
	private $merged_subject = "";
	private $merged_message = "";
	private $speciaal = "";
	
	private $mailingvanafid = 0;
	private $vanafnaam = "";
	private $vanafadres = "";
	private $to_name = "";
	private $cc_addr = "";
	private $subject = "";
	private $message = "";
	private $zichtbaarvoor = 0;
	private $template = 0;
	private $htmldirect = 0;
	private $zonderbriefpapier = 0;
	private $changed_on = "0000-00-00";
	private $Gewijzigd = "0000-00-00";
	private $IngevoerdDoor = 0;
	private $Ingevoerd = "0000-00-00";
	private $GewijzigdDoor = 0;
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
	private $emailsecretariaat = "";

	public $xtrachar = "";
	public $xtranum = 0;

	function __construct($p_mid=0) {
		global $selectnaam, $fdlang, $fdkort, $wherelid, $fromlidond, $wherelidond, $whereliddipl;

		$this->mid = $p_mid;
		$this->vuldbvars();
		
		if ($this->mid > 0) {
			$this->dir_attachm = $_SESSION['settings']['path_attachments'] . $this->mid . "/";
			$this->dir_media = substr($_SERVER["SCRIPT_FILENAME"], 0, strrpos($_SERVER["SCRIPT_FILENAME"], "/")) . "/media/" . $this->mid . "/";
		}
		
		if (strlen($_SESSION['settings']['mailing_extensies_toegestaan']) > 0) {
			$this->allowed_ext = $_SESSION['settings']['mailing_extensies_toegestaan'];
		}
		$this->max_size_attachm = $_SESSION['settings']['max_grootte_bijlage'] * 1024;
		$this->emailsecretariaat = $_SESSION['settings']['emailsecretariaat'];

		$sql = sprintf("SELECT Roepnaam FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Roepnaam", 'SQL' => $sql);
		
		$sql = sprintf("SELECT CASE
								WHEN L.GEBDATUM > DATE_SUB(CURDATE(), INTERVAL 18 YEAR) AND LENGTH(Email)>5 AND LENGTH(EmailOuders)>5 THEN CONCAT(IF(LENGTH(L.Roepnaam) < 2, 'leden', L.Roepnaam), ' of ouder/verzorger van')
								WHEN LENGTH(Email)>5 AND LENGTH(EmailOuders)=0 THEN IF(LENGTH(L.Roepnaam) < 2, 'leden', L.Roepnaam)
								WHEN LENGTH(Email)=0 AND LENGTH(EmailOuders)>5 THEN CONCAT('Ouder/verzorger van ', IF(LENGTH(L.Roepnaam) < 2, 'leden', L.Roepnaam))
								ELSE IF(LENGTH(L.Roepnaam) < 2, 'leden', L.Roepnaam) END
								FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "RoepnaamOfOuders", 'SQL' => $sql);
		
		$sql = sprintf("SELECT CONCAT(IF(LENGTH(L.Roepnaam) < 2, 'leden', L.Roepnaam), IF(L.GEBDATUM > DATE_SUB(CURDATE(), INTERVAL 18 YEAR), ' of ouders/verzorgers van', '')) FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "RoepnaamOfOuders18", 'SQL' => $sql);

		$sql = sprintf("SELECT Voorletter FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Voorletters", 'SQL' => $sql);

		$sql = sprintf("SELECT (CASE ISNULL(L.Tussenv) WHEN 0 THEN CONCAT(L.Tussenv, ' ', L.Achternaam)
				 ELSE L.Achternaam END) AS Achternaam FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Achternaam", 'SQL' => $sql);
		
		$sql = "SELECT CASE L.Geslacht";
		foreach (ARRGESLACHT as $key => $value) {
			$sql .= sprintf(" WHEN '%s' THEN '%s'", $key, $value);
		}
		$sql .= sprintf(" END FROM %sLid AS L WHERE L.RecordID=%%d", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Geslacht", 'SQL' => $sql);

		$sql = sprintf("SELECT L.Adres FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Adres", 'SQL' => $sql);

		$sql = sprintf("SELECT L.Postcode FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Postcode", 'SQL' => $sql);

		$sql = sprintf("SELECT L.Woonplaats FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);;
		$this->MergeField[]=array('Naam' => "Woonplaats_HL", 'SQL' => $sql);
		
		$sql = sprintf("SELECT UPPER(L.Woonplaats) FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);;
		$this->MergeField[]=array('Naam' => "Woonplaats", 'SQL' => $sql);
							 
		$sql = sprintf("SELECT %s AS Lidnaam FROM %sLid AS L WHERE L.RecordID=%%d;", $selectnaam, TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Lidnaam", 'SQL' => $sql);

		$sql = sprintf("SELECT Login FROM %sAdmin_login WHERE LidID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Login", 'SQL' => $sql);

		$sql = sprintf("SELECT LM.Lidnr FROM %sLidmaatschap AS LM WHERE %s AND LM.Lid=%%d;", TABLE_PREFIX, $wherelid);
		$this->MergeField[]=array('Naam' => "Lidnummer", 'SQL' => $sql);
		
		$sql = sprintf("SELECT FLOOR(SUM(TIMESTAMPDIFF(MONTH, LM.LIDDATUM, IF(ISNULL(LM.Opgezegd), CURDATE(), LM.Opgezegd)))/12)
							FROM %sLidmaatschap AS LM WHERE LM.LIDDATUM < CURDATE() AND LM.Lid=%%d LIMIT 1;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "LengteLidmaatschap", 'SQL' => $sql);

		$sql = sprintf("SELECT L.Telefoon FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Telefoon", 'SQL' => $sql);

		$sql = sprintf("SELECT L.Mobiel FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Mobiel", 'SQL' => $sql);

		$sql = sprintf("SELECT DATE_FORMAT(L.GEBDATUM, %s) AS Geboortedatum FROM %sLid AS L WHERE L.RecordID=%%d;", $fdlang, TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Geboortedatum", 'SQL' => $sql);

		$sql = sprintf("SELECT L.GEBPLAATS FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Geboorteplaats", 'SQL' => $sql);

		$sql = sprintf('SELECT O.Naam FROM %1$sLidond AS LO INNER JOIN %1$sOnderdl AS O ON O.RecordID = LO.OnderdeelID
				 WHERE %2$s AND O.GekoppeldAanQuery=0 AND LO.Lid=%%d ORDER BY O.Naam;', TABLE_PREFIX, $wherelidond);
		$this->MergeField[]=array('Naam' => "Onderdelen", 'SQL' => $sql);
	
		if ((new cls_db_base("Bewseiz"))->aantal() > 0) {
			$sql = sprintf('SELECT CONCAT(DATE_FORMAT(BEGIN_PER, %1$s), \' t/m \', DATE_FORMAT(EINDE_PER, %1$s)) AS Per'
				  . ' FROM %2$sBewaking AS BW INNER JOIN %2$sBewseiz AS BS ON BS.RecordID = BW.SeizoenID'
				  . ' WHERE EINDE_PER >= CURDATE() AND %3$s ORDER BY BEGIN_PER;', $fdlang, TABLE_PREFIX, "Lid=%d");
			$this->MergeField[]=array('Naam' => "BewakingToekomst", 'SQL' => $sql);
							 
			$Funct = "(CASE BW.Functie WHEN 0 THEN '' ELSE CONCAT(' (', F.Afkorting, ')') END)";
			$sql = sprintf('SELECT CONCAT(DATE_FORMAT(BEGIN_PER, %1$s), \' t/m \', DATE_FORMAT(EINDE_PER, %1$s), %2$s) AS Per
					 FROM (%3$sBewaking AS BW INNER JOIN %3$sBewseiz AS BS ON BS.RecordID = BW.SeizoenID) INNER JOIN %3$sFunctie AS F ON BW.Functie = F.Nummer
					 WHERE BW.EINDE_PER < CURDATE() AND %4$s ORDER BY BW.BEGIN_PER;', $fdkort, $Funct, TABLE_PREFIX, "BW.Lid=%d");
			$this->MergeField[]=array('Naam' => "BewakingHistorie", 'SQL' => $sql);
		}
							  
		$sql = sprintf("SELECT L.Bankrekening FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Bankrekening", 'SQL' => $sql);
		
		$sql = sprintf("SELECT L.Burgerservicenummer FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "BSN", 'SQL' => $sql);

		if ((new cls_Rekening())->aantal() > 0) {
			$sql = sprintf("SELECT RK.Nummer FROM %sRekening AS RK WHERE RK.Bedrag > 0 AND RK.Bedrag > RK.Betaald AND %s;", TABLE_PREFIX, "RK.Lid=%d");
			$this->MergeField[]=array('Naam' => "OpenstaandeRekeningen", 'SQL' => $sql);

			$sql = sprintf("SELECT ROUND(SUM(RK.Bedrag-RK.Betaald), 2) AS OpenstaandBedrag FROM %sRekening AS RK WHERE RK.Lid=%%d;", TABLE_PREFIX);
			$this->MergeField[]=array('Naam' => "OpenstaandBedrag", 'SQL' => $sql);
		}

		$sql = sprintf("SELECT (CASE L.`Machtiging afgegeven` WHEN 1 THEN 'Ja' ELSE 'Nee' END) AS MachtigingAfgegeven
							 FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "MachtigingAfgegeven", 'SQL' => $sql);
		
		$sql = sprintf('SELECT DISTINCT DP.Kode FROM %1$sLiddipl AS LD INNER JOIN %1$sDiploma AS DP ON LD.DiplomaID = DP.RecordID
				 WHERE %2$s AND LD.Lid=%%d ORDER BY DP.Volgnr, LD.DatumBehaald, DP.KODE;', TABLE_PREFIX, $whereliddipl);
		$this->MergeField[]=array('Naam' => "DiplomaKort", 'SQL' => $sql);

		$sql = sprintf('SELECT DISTINCT DP.Naam FROM %1$sLiddipl AS LD INNER JOIN %1$sDiploma AS DP ON LD.DiplomaID = DP.RecordID
				 WHERE %2$s AND LD.Lid=%%d ORDER BY DP.Volgnr, LD.DatumBehaald, DP.Naam;', TABLE_PREFIX, $whereliddipl);
		$this->MergeField[]=array('Naam' => "DiplomaLang", 'SQL' => $sql);
		
		if ((new cls_Diploma())->aantal("Zelfservice=1") > 0) {
			$sql = sprintf('SELECT DISTINCT DP.Naam FROM %1$sLiddipl AS LD INNER JOIN %1$sDiploma AS DP ON LD.DiplomaID = DP.RecordID
					WHERE %2$s AND LD.Lid=%%d AND DP.Zelfservice=1 ORDER BY DP.Volgnr, LD.DatumBehaald, DP.Naam;', TABLE_PREFIX, $whereliddipl);
			$this->MergeField[]=array('Naam' => "DiplomaZelfservice", 'SQL' => $sql);
		}
	
		$sql = sprintf("SELECT L.RelnrRedNed FROM %sLid AS L WHERE %s;", TABLE_PREFIX, "L.RecordID=%d");
		$this->MergeField[]=array('Naam' => "RelnrRedNed", 'SQL' => $sql);

		$sql = sprintf("SELECT DATE_FORMAT(LIDDATUM, %s) FROM %sLidmaatschap AS LM WHERE %s AND LM.Lid=%%d;", $fdlang, TABLE_PREFIX, $wherelid);
		$this->MergeField[]=array('Naam' => "LidVanaf", 'SQL' => $sql);

		$sql = sprintf("SELECT IF(LM.OPGEZEGD IS NULL, 'Niet', DATE_FORMAT(LM.Opgezegd, %s)) FROM %sLidmaatschap AS LM
							 WHERE LM.LIDDATUM < CURDATE() AND LM.Lid=%%d
							 ORDER BY LM.LIDDATUM DESC LIMIT 1;", $fdlang, TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "OpgezegdPer", 'SQL' => $sql);

		$sql = sprintf("SELECT IF((L.Email IS NULL), L.EmailOuders, L.Email) FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Email", 'SQL' => $sql);

		$query = sprintf("SELECT COUNT(*) FROM %1\$sLidRedNed AS RN JOIN %1\$sLidmaatschap AS LM ON LM.LIDNR = RN.Lidnr WHERE LENGTH(RN.Foutboodschap) > 0;", TABLE_PREFIX);
		if ((new cls_db_base())->scalar($query) > 0) {
			$sql = sprintf("SELECT RN.Foutboodschap FROM %1\$sLidRedNed AS RN JOIN %1\$sLidmaatschap AS LM ON LM.LIDNR = RN.Lidnr WHERE LM.LID=%%d;", TABLE_PREFIX);
			$this->MergeField[]=array('Naam' => "VerschilSportlink", 'SQL' => $sql);
		}
		
		$i_ond = new cls_Onderdeel();

		if (count($i_ond->lijst(1, "O.`Type`='A'")) > 0) {
			$sql = sprintf("SELECT CONCAT(O.Naam, IF(LO.Functie > 0, CONCAT(' (', F.Omschrijv , ')'), ''))
					FROM %s
					WHERE IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE() AND O.`Type`='A' AND LO.Lid=%%d ORDER BY O.Naam;", $i_ond->fromlidond);
			$this->MergeField[]=array('Naam' => "Afdelingen", 'SQL' => $sql);
			
			if ((new cls_Groep())->aantal() > 1) {				
				$sql = sprintf("SELECT CONCAT(O.Naam, IF(LO.GroepID > 0, CONCAT(' (', GR.Starttijd, ' ', GR.Omschrijving, ')'), IF(LO.Functie > 0, CONCAT(' (', F.Omschrijv , ')'), '')))
						FROM %s
						WHERE IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE() AND O.`Type`='A' AND LO.Lid=%%d ORDER BY O.Naam;", $i_ond->fromlidond);
				$this->MergeField[]=array('Naam' => "AfdelingenMetGroep", 'SQL' => $sql);
			}
		}
		
		if (count($i_ond->lijst(1, "O.`Type`='R'")) > 0) {
			$sql = sprintf("SELECT O.Naam FROM %sLidond AS LO INNER JOIN %s AS O ON O.RecordID = LO.OnderdeelID 
					 WHERE %s AND `Type`='R' AND LO.Lid=%%d ORDER BY O.Naam;", TABLE_PREFIX, $i_ond->table, $i_ond->wherelidond);
			$this->MergeField[]=array('Naam' => "Rollen", 'SQL' => $sql);
		}
		
				
		if (count($i_ond->lijst(1, "O.`Type`='T'")) > 0) {
			$sql = sprintf("SELECT O.Naam FROM %sLidond AS LO INNER JOIN %s AS O ON O.RecordID = LO.OnderdeelID 
					 WHERE %s AND `Type`='T' AND LO.Lid=%%d ORDER BY O.Naam;", TABLE_PREFIX, $i_ond->table, $i_ond->wherelidond);
			$this->MergeField[]=array('Naam' => "Toestemmingen", 'SQL' => $sql);
		}
		
		$i_ond =  null;

		$i_memo = new cls_Memo();
		if ($i_memo->aantal("Soort = 'D'") > 0) {
			$sql = sprintf("SELECT IF((Memo IS NULL), 'Geen', Memo) FROM %sMemo WHERE Lid=%%d AND Soort='D';", TABLE_PREFIX);
			$this->MergeField[]=array('Naam' => "Dieet", 'SQL' => $sql);
		}
		
		if ($i_memo->aantal("Soort = 'G'") > 0) {
			$sql = sprintf("SELECT IF((Memo IS NULL), 'Geen', Memo) FROM %sMemo WHERE Lid=%%d AND Soort='G';", TABLE_PREFIX);
			$this->MergeField[]=array('Naam' => "MemoGezondheid", 'SQL' => $sql);
		}
		
		if ($i_memo->aantal("Soort = 'I'") > 0) {
			$sql = sprintf("SELECT IF(Memo IS NULL, 'Geen', Memo) FROM %sMemo WHERE Lid=%%d AND Soort='I';", TABLE_PREFIX);
			$this->MergeField[]=array('Naam' => "OpmerkingInschrijvingBewaking", 'SQL' => $sql);
		}
		$i_memo = null;
		
		$this->MergeField[]=array('Naam' => "Naamvereniging", 'SQL' => "");
		$this->MergeField[]=array('Naam' => "Naamwebsite", 'SQL' => "");
		$this->MergeField[]=array('Naam' => "URLwebsite", 'SQL' => "");
		
		// Velden die bij specifieke mailings horen.
		if ($this->mid == $_SESSION['settings']['mailing_lidnr'] and $this->mid > 0) {
			$this->xtrachar = "LNR";
			$this->speciaal = "Versturen lidnummer";
			$this->vertraging_tussen_verzenden = 2;
			$this->MergeField[]=array('Naam' => "Geblokkeerd", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "IPaddress", 'SQL' => "");
			
		} elseif ($this->mid == $_SESSION['settings']['mailing_validatielogin'] and $this->mid > 0) {
			$this->xtrachar = "VALLOGIN";
			$this->speciaal = "Validatie login";
			$this->vertraging_tussen_verzenden = 5;
			$this->MergeField[]=array('Naam' => "Geblokkeerd", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "IPaddress", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "GELDIGHEIDACTIVATIE", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "URLACTIVATIE", 'SQL' => "");
			
		} elseif ($this->mid == $_SESSION['settings']['mailing_herstellenwachtwoord'] and $this->mid > 0) {
			$this->xtrachar = "HERWW";
			$this->speciaal = "Herstellen wachtwoord";
			$this->vertraging_tussen_verzenden = 5;
			$this->MergeField[]=array('Naam' => "Geblokkeerd", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "IPaddress", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "GELDIGHEIDACTIVATIE", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "URLRESETWW", 'SQL' => "");
			
		} elseif ($this->mid == $_SESSION['settings']['mailing_bewakinginschrijving'] and $this->mid > 0) {
			$this->xtrachar = "BW_I";
			$this->speciaal = "Bevestiging inschrijving bewaking";
			$this->vertraging_tussen_verzenden = 5;
			$this->MergeField[]=array('Naam' => "VoorwaardenInschrijving", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "InschrijvingBewaking", 'SQL' => "");
			
		} elseif ($this->mid == $_SESSION['settings']['mailing_bevestigingbestelling'] and $this->mid > 0) {
			$this->xtrachar = "BEST";
			$this->speciaal = "Bevestiging bestelling webshop";
			$this->vertraging_tussen_verzenden = 5;
			$this->MergeField[]=array('Naam' => "Ordernummer", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "Bestelling", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "Totaalbedrag", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "VoorwaardenBestelling", 'SQL' => "");
			
		} elseif ($this->mid == $_SESSION['settings']['mailing_bevestigingopzegging'] and $this->mid > 0) {
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
	
	public function lijst($p_filter="") {
		global $currenttab, $currenttab2;
		$i_M = new cls_Mailing();
	
		if (isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0 and $_GET['op'] == "bekijkmail") {
			$xtra = "<p class='mededeling'><input type='button' value='Terug' onclick='history.go(-1);'></p>\n";
			$email = new email($_GET['MailingHistID']);
			$email->toon();
			$mail = null;
			
		} elseif ($p_filter == "Verzonden mails" or $p_filter == "Verzonden e-mails" or $p_filter == "Outbox") {
			$i_mh = new cls_Mailing_hist();
			$ld = sprintf("<a href='index.php?tp=%s&amp;op=bekijkmail&amp;MailingHistID=%%d'>%%s</a>", urlencode($_GET['tp']));
			$f = "send_on > DATE_SUB(NOW(), INTERVAL 24 HOUR)";
			$f2 = "send_on > DATE_SUB(NOW(), INTERVAL 1 HOUR)";
			if ($_SESSION['settings']['maxmailsperdag'] > 0 and $i_mh->aantal($f) >= $_SESSION['settings']['maxmailsperdag']) {
				$rs = "";
			} elseif ($_SESSION['settings']['maxmailsperuur'] > 0 and $i_mh->aantal($f2) >= $_SESSION['settings']['maxmailsperuur']) {
				$rs = "";
			} elseif ($p_filter == "Outbox") {
				if ($_SESSION['settings']['mailing_direct_verzenden'] == 0) {
					$rs = sprintf("<a href='index.php?tp=%s&amp;op=sentmail&amp;MailingHistID=%%d'><img src='images/sendmail.png' alt='sent mail' title='Verstuur'></a>", urlencode($_GET['tp']));
				} else {
					$rs = "";
				}
				$rs .= sprintf("&nbsp;<a href='index.php?tp=%s&amp;op=deletehist&amp;MailingHistID=%%d'><img src='images/del.png' alt='Verwijder e-mail' title='Verwijderen'></a>", urlencode($_GET['tp']));
			} elseif ($_SESSION['settings']['mailing_mailopnieuw'] == 0 or ($_SESSION['settings']['mailing_mailopnieuw'] == 1 and $_SESSION['webmaster'] == 0)) {
				$rs = "";
			} else {
				$rs = sprintf("<a href='index.php?tp=%s&amp;op=sentoutbox&amp;MailingHistID=%%d'><img src='images/sendmail.png' alt='Sent e-mail' title='Opnieuw versturen'></a>", urlencode($_GET['tp']));
			}
			if ($p_filter == "Outbox") {
				$rows = $i_mh->outbox(1)->fetchAll();
				if (count($rows) > 0) {
					echo(fnDisplayTable($rows, $ld, "", 0, $rs));
					if ($_SESSION['settings']['mailing_direct_verzenden'] == 0 and count($rows) > 1) {
						printf("<form method='post' action='%s?tp=%s/%s'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
						echo("<div id='opdrachtknoppen'>\n");
						printf("<input type='submit' name='action' value='Inhoud versturen' onClick='this.form.submit();'>\n");
						echo("</div>  <!-- Einde opdrachtknoppen -->\n");
						echo("</form>\n");
					}
				} else {
					echo("<p>De outbox is leeg.</p>\n");
				}
			} else {
				echo(fnDisplayTable($i_mh->lijst(), $ld, "", 0, $rs));
			}
			$i_mh = null;
			
		} else {
			if (toegang("Mailing/Wijzigen mailing")) {
				$lnk = sprintf("<a href='%s?tp=%s/Wijzigen+mailing&amp;op=edit&amp;mid=%%d' title='Wijzigen mailing'><img src='images/edit.png' alt='Wijzig mailing'></a>", $_SERVER['PHP_SELF'], $currenttab);
			} else {
				$lnk = "";
			}
			
			echo("<div id='filter'>\n");
			echo("<label>Onderwerp bevat</label><input id='tbSubjectFilter' onKeyUp=\"fnFilter('lijstmailingen', 'tbSubjectFilter', 2);\">\n");
			echo("</div> <!-- Einde filter -->\n");
			
			$lnk_lk = sprintf("<a href='%s?tp=Mailing/Historie&amp;op=historie&amp;mid=%%d' title='Bekijk historie'><img src='images/object_10.png' alt='Historie'></a>", $_SERVER['PHP_SELF']);
			$rows = $i_M->lijst($p_filter);
			echo(fnDisplayTable($rows, $lnk, "", 0, $lnk_lk, "", "lijstmailingen"));
			if ($p_filter == "Prullenbak" and $_SESSION['settings']['mailing_bewaartijd'] > 0) {
				printf("<p>Mailings die langer dan %d maanden geleden verwijderd zijn, worden automatisch definitief opgeschoond.</p>\n", $_SESSION['settings']['mailing_bewaartijd']);
			}
		}
		$i_m = null;
	} # lijst
	
	public function edit() {
		global $currenttab, $currenttab2, $navpad, $fdlang;
		
		$i_m = new cls_Mailing();
		$i_mr = new cls_Mailing_rcpt();
		(new cls_Lidond())->autogroepenbijwerken();
		
		$f = sprintf("MailingID=%d", $this->mid);
		$this->aant_rcpt = $i_mr->aantal($f);
		
		if ($this->mid > 0) {
			$navpad[]['naam'] = sprintf("Details mailing %d", $this->mid);
		}
				
		printf("<div class='editmailingform'>\n");
//		printf("<form method='post' action='%s?tp=%s/%s&mid=%d' name='frm_mailedit' enctype='multipart/form-data'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2, $this->mid);
		printf("<form method='post' action='%s?tp=Mailing/Wijzigen+mailing&mid=%d' name='frm_mailedit' enctype='multipart/form-data'>\n", $_SERVER['PHP_SELF'], $this->mid);
		
		printf("<label>Van</label><select name='MailingVanafID'>\n<option value=''>Selecteer ...</option>%s</select>\n", (new cls_Mailing_vanaf())->htmloptions($this->mailingvanafid));
		printf("<label>Aan</label><input type='text' class='naam' name='to_name' value=\"%s\" maxlength=50 placeholder='Omschrijving groep personen aan wie de mailing is gericht'>\n", $this->to_name);
		printf("<label>Onderwerp</label><input type='text' class='naam' name='subject' value=\"%s\" maxlength=75 placeholder='Onderwerp'>\n", $this->subject);

		$this->verzendenmag = false;
		if ($this->mid > 0) {
			$rcpt_rows = $i_mr->lijst($this->mid);
			if (strlen($this->speciaal) > 0) {
				printf("<label>Specifiek doel</label><p>%s</p>\n", $this->speciaal);
				$i_mr->delete_all($this->mid);
			} else {
				$this->meldingen = "Er zijn nog geen ontvangers aan deze mailing toegevoegd.<br>\n";
				echo("<label>");
				if ($this->aant_rcpt == 1) {
					echo("Ontvanger</label>");
				} elseif ($this->aant_rcpt > 1) {
					printf("Ontvangers (%d)</label>", $this->aant_rcpt);
				} else {
					echo("Ontvangers</label>");
				}
				$aant_ze = 0;
				echo("<div class='invoerblok'>\n");
				$this_aant_rcpt = 0;
				echo("<ul class='listrecipients'>\n");
				foreach($rcpt_rows as $rcpt) {
					$ze = "";
					if (!isValidMailAddress($rcpt->Email) and !isValidMailAddress($rcpt->EmailVereniging) and !isValidMailAddress($rcpt->EmailOuders) and !isValidMailAddress($rcpt->to_address)) {
						$ze = " *";
						$aant_ze++;
					}
					if ($rcpt->LidID > 0) {
						$lnk = sprintf("%s?tp=%s&amp;op=del_rcpt&amp;mid=%d&amp;lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, $rcpt->LidID);
						printf("<li>%s&nbsp;<a href='%s'><img src='/images/delete.gif' alt='Verwijderen' title='Verwijderen'></a></li>\n", htmlentities($rcpt->Naam_lid . $ze), $lnk);
					} else {
						$lnk = sprintf("%s?tp=%s&amp;op=del_rcpt&amp;mid=%d&amp;addr=%s", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, $rcpt->to_address);
						printf("<li>%s&nbsp;<a href='%s'><img src='/images/delete.gif' alt='Verwijderen' title='Verwijderen'></a></li>\n", $rcpt->to_address, $lnk);
						$this->bevat_losse_email = true;
					}
					$this->aant_rcpt++;
				}
				echo("</ul>\n");
				if ($aant_ze > 0) {
					printf("<span>* = zonder e-mailadres, totaal %d stuks</span>", $aant_ze);
				}
				$this->verzendenmag = true;
				$this->meldingen = "";
				echo("</div> <!-- Einde invoerblok -->\n");

				echo("<label>Ontvanger toevoegen</label>\n");
				printf("<select name='add_lid' onChange='this.form.submit();'>\n");
				printf("<option value=0>Toevoegen lid ...</option>\n");
				$rcpt_rows = $i_m->mogelijkeontvangers($this->mid);
				foreach($rcpt_rows as $rcpt) {
					$rnm = $rcpt->Zoeknaam_lid;
					$sl = substr((new cls_Lidmaatschap())->soortlid($rcpt->LidID), 0, 1);
					if ($sl != "L") {
						$rnm .= sprintf(" (%s)", $sl);
					}
					printf("<option value='%d'>%s</option>\n", $rcpt->LidID, $rnm);
				}
				printf("</select>&nbsp;\n");
			
				echo("<input type='email' name='add_address' size=50 maxlength=50 placeholder='Toevoegen e-mailadres' onBlur='this.form.submit();'>");
				echo("<div class='clear'></div>\n");
				
				if (!isset($_POST['selectie_status'])) {
					$_POST['selectie_status'] = "L";
				}

				echo("<label>Selectie leden</label>\n");
				echo("<div class='invoerblok'>\n");
				echo("<div id='mailingselectieleden'>\n");
				echo("<table>\n");
				echo("<tr><td></td><td>Vanaf</td><td>Tot en met</td></tr>\n");

				$i_lid = new cls_Lid();
				$i_lm = new cls_Lidmaatschap();
				printf("<tr><td>Geboortedatum:</td><td><input type='date' value='%s' name='selectie_vangebdatum'></td><td><input type='date' value='%s' name='selectie_temgebdatum'></td></tr>\n", $i_lid->min("GEBDATUM"), $i_lid->max("GEBDATUM"));
				printf("<tr><td>Lid vanaf:</td><td><input type='date' name='selectie_vanliddatum' value='%s'></td><td><input type='date' name='selectie_temliddatum' value='%s'></td></tr>\n", $i_lm->min("LIDDATUM"), $i_lm->max("LIDDATUM"));
			
				$selgr = "<select name='selectie_groep'>\n
							<option value=0>&nbsp;</option>\n";
				foreach((new cls_Onderdeel())->lijst() as $grp) {
					$selgr .= sprintf("<option value=%d>%s</option>\n", $grp->RecordID, $grp->Naam);
				}
				$selgr .= "</select>\n";
				printf("<tr><td>Zit in groep:</td><td colspan=2>%s</td></tr>\n", $selgr);
				
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
		
		printf("<label>Cc</label><input type='email' name='cc_addr' value='%s'>\n", $this->cc_addr);
		$ondrows = (new cls_Onderdeel())->lijst(1, "`Type`<>'T'", "", $_SESSION['lidid']);
		if ($this->zichtbaarvoor == 0) {
				$select = "<option value=0 selected>Iedereen</option>\n";
		} else {
			$select = "<option value=0>Iedereen</option>\n";
		}
		if ($this->zichtbaarvoor == -1) {
			$select .= "<option value=-1 selected>Alleen webmasters</option>\n";
		} elseif ($_SESSION['webmaster'] == 1) {
			$select .= "<option value=-1>Alleen webmasters</option>\n";
		}
		foreach($ondrows as $ondrow) {
			if ($this->zichtbaarvoor == $ondrow->RecordID) {
				$select .= sprintf("<option value=%d selected>%s</option>\n", $ondrow->RecordID, $ondrow->Naam);
			} else {
				$select .= sprintf("<option value=%d>%s</option>\n", $ondrow->RecordID, $ondrow->Naam);
			}
		}	
		printf("<label>Zichtbaar voor</label><select name='ZichtbaarVoor'>\n%s</select>\n", $select);
		if ($this->mid > 0) {
			printf("<label>Opties</label><input type='checkbox' name='template' value=1%s><p>Template</p>", checked($this->template));
			printf("<input type='checkbox' name='HTMLdirect' value=1%s onChange='this.form.submit();'><p>HTML direct (zonder editor)</p>", checked($this->htmldirect));
			printf("<input type='checkbox' name='ZonderBriefpapier' value=1%s><p>Zonder briefpapier versturen</p>", checked($this->zonderbriefpapier));
		
			echo("<div class='clear'></div>\n");
			if ($this->htmldirect == 1) {
				printf("<textarea name='message' rows=15 cols=125>%s</textarea>\n", $this->message);
			} else {
				echo("</div> <!-- Einde editmailingform -->\n");
				if ($_SESSION['settings']['mailing_type_editor'] == 3) {
				
					printf("<textarea id='message' name='message'>%s</textarea>\n", $this->message);
				
				} elseif ($_SESSION['settings']['mailing_type_editor'] == 2) {
					printf("<textarea name='message'>%s</textarea>
					<script>
					CKEDITOR.replace( 'message' );
					</script>\n", $this->message);
			
				} else {
					printf("<textarea name='message' id='editor'>%s</textarea>\n", $this->message);
					echo("<script>\n
						ClassicEditor
							.create( document.querySelector( '#editor' )	)
							.catch( error => {
						console.error( error );
						} );n
						</script>\n");
				}
				printf("<div class='editmailingform'>\n");
			}

			$strHV = "";
			foreach ($this->MergeField as $v) {
				$strHV .= "<li>[%" . $v['Naam'] . "%]</li>\n";
			}
			$strHV .= "";
			echo("<div class='clear'></div>\n");
			printf("<label>Beschikbare variabelen</label><div class='invoerblok'><ul class='listmergefields'>\n%s</ul></div>\n", $strHV);

			echo("<label>Bijlagen:</label>\n<div class='invoerblok'>\n<table>\n");
			if (is_dir($this->dir_attachm)) {
				$d = dir($this->dir_attachm);
				while (false !== ($entry = $d->read())) {
					if ($entry != "." and $entry != "..") {
						echo("<tr>\n<td>" . $entry . "</td>\n");
						$stat = stat($this->dir_attachm . $entry);
						echo("<td>" . number_format(($stat['size'] / 1024), 0, ',', '.') . " KB</td>\n");
						$lnk = sprintf("%s?tp=%s&op=del_attach&mid=%d&attach=%s", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, str_replace(".", "!", $entry));
						printf("<td><a href='%s'><img src='/images/delete.gif' alt='Verwijderen' title='Verwijder'></a></td>\n", $lnk);
					}
				}
				$d->close();
			}
			echo("</table>\n");
			echo("<input type='file' name='UploadFile' size='40'>\n");
			echo("<input type='submit' name='UploadBijlage' value='Upload bijlage'>");
			printf("<p>(Extensies '%s' zijn toegestaan)</p>\n", $this->allowed_ext);
			echo("</div>  <!-- Einde invoerblok -->\n");	
			
			echo("<label>Media</label>\n<div class='invoerblok'>\n");
			echo("<table>\n");
			if (is_dir($this->dir_media)) {
				$d = dir($this->dir_media);
				while (false !== ($entry = $d->read())) {
					if ($entry != "." and $entry != "..") {
						echo("<tr>\n<td>" . $entry . "</td>\n");
						$stat = stat($this->dir_media . "/" . $entry);
						echo("<td>" . number_format(($stat['size'] / 1024), 0, ',', '.') . " KB</td>\n");
						$lnk = sprintf("%s?tp=%s&op=del_media&mid=%d&attach=%s", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, str_replace(".", "!", $entry));
						printf("<td><a href='%s'>Verwijder</a>&nbsp;", $lnk);
						if (strlen($this->message) == 0) {
							$lnk = sprintf("%s?tp=%s&op=afbeeldingalsbericht&mid=%d&attach=%s", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, str_replace(".", "!", $entry));
							printf("&nbsp;-&nbsp;<a href='%s'>Als bericht</a>", $lnk);
						}
						if ($this->htmldirect == 1) {
							$lnk = sprintf("%s?tp=%s&op=htmlmedia&mid=%d&attach=%s", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, str_replace(".", "!", $entry));
							printf("&nbsp;-&nbsp;<a href='%s'>HTML Media</a>", $lnk);
						}
					}
				}
				$d->close();
			}
			echo("</table>\n");

			echo("<input type='file' name='UploadMedia'>\n");
			echo("<input type='submit' name='UploadMedia' value='Upload mediabestand'>");
			printf("<p>(Extensies '%s' zijn toegestaan)</p>\n", $this->allowed_ext_afb);
			
			echo("</div>  <!-- Einde invoerblok -->\n");
		}
		
		$this->controle();
		if ($this->mid > 0) {
			$i_lid = new cls_Lid();
			if ($this->Ingevoerd > '1901-01-01') {
				printf("<label>Toegevoegd door/op</label><p>%s / %s</p>\n", $i_lid->naam($this->IngevoerdDoor), strftime("%e %B %Y (%H:%M)", strtotime($this->Ingevoerd)));
			}
			if ($this->Gewijzigd > '1901-01-01' and $this->GewijzigdDoor > 0) {
				printf("<label>Laatst gewijzigd door/op</label><p>%s / %s</p>\n", $i_lid->naam($this->GewijzigdDoor), strftime("%e %B %Y (%H:%M)", strtotime($this->Gewijzigd)));
			}
			
			if ($this->deleted_on > '1901-01-01') {
				printf("<label>Verwijderd door/op</label><p>%s / %s</p>\n", $i_lid->naam($this->DeletedBy), strftime("%e %B %Y (%H:%M)", strtotime($this->deleted_on)));
			}

			if ($this->verzendenmag == false) {
				$this->meldingen .= " Deze mailing mag niet verzonden worden.";
			}
			if (strlen($this->meldingen) > 0) {
				printf("<label>Meldingen</label><p>%s</p>\n", $this->meldingen);
			}
			$cls_lid = null;
		}
		
		echo("<div id='opdrachtknoppen'>\n");
		if ($this->mid > 0) {
			echo("<input type='submit' name='action' value='Bewaren'>\n");
			echo("<input type='submit' name='action' value='Bewaren & sluiten'>\n");
		} else {
			echo("<input type='submit' name='Toevoegen' value='Toevoegen'>\n");
		}
			
		if ($this->mid > 0 and $this->verzendenmag and $this->deleted_on < "2000-01-01") {
//			printf("<input type='submit' name='action' value='Verstuur mailing' title='Verstuur mailing'>\n");
			$lnk = sprintf("%s?tp=%s&op=send&mid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid);
			printf("<button onclick=\"window.open('%s','_blank')\">Verstuur mailing</button>", $lnk);	
		}
		if ($this->mid > 0  and $this->aant_rcpt > 0) {
			$lnk = sprintf("%s?tp=%s&op=preview&mid=%d", $_SERVER['PHP_SELF'], $currenttab, $this->mid);
			printf("<button onclick=\"window.open('%s','_blank')\">Bekijk voorbeeld</button>", $lnk);			
		}
		
		if ($this->mid > 0 and $this->deleted_on > "1901-01-01" and $_SESSION['webmaster'] == 1) {
			printf("<input type='submit' name='action' value='Verwijderen ongedaan maken' title='Verwijderen ongedaan maken'>\n");
		} elseif ($this->mid > 0 and $this->deleted_on < "1901-01-01" and ($_SESSION['webmaster'] == 1 or $_SESSION['lidid'] == $this->IngevoerdDoor)) {
			printf("<input type='submit' name='action' value='Verwijderen'>\n");
		}
		echo("</div>  <!-- Einde opdrachtknoppen -->\n");
		
		printf("</form>\n");
		echo("</div> <!-- Einde editmailingform -->\n");

		echo("</div>  <!-- Einde mailing -->\n");
	} # edit
	
	public function htmlmedia($p_bestand) {
		
		$url = BASISURL . str_replace(BASEDIR, "", $this->dir_media) . str_replace("!", ".", $p_bestand);
		$ext = fnExtBestand($p_bestand);
		if ($ext == "mp3") {
			$this->message .= "\n<audio controls>" . sprintf("<source src='%s' type='audio/mpeg'>Deze browser ondersteunt geen audio.</audio>", $url);
		} elseif ($ext == "mp4") {
			$this->message .= sprintf("<video width='800' height='600'><source src='%s' type='video/mp4'>Deze browser ondersteunt geen video's.</video> ", $url);
		} else {
			$this->message .= "\n" . sprintf("<img src='%s'>", $url);
		}
	}
	
	private function controle($enkellid=0) {
	
		if ($this->mid > 0) {
			$this->meldingen = "";
			$this->verzendenmag = true;

			if (strlen($this->vanafadres) == 0) {
				$this->meldingen = "De Van moet ingevuld zijn. \n";
				$this->verzendenmag = false;
			}
			
			if ($this->aant_rcpt == 0 and $enkellid == 0) {
				if (strlen($this->speciaal) > 0) {
					$this->meldingen .= "Een mailing met een speciaal doel, kan niet los worden verzonden.<br>\n";
				} else {
					$this->meldingen .= "Er zijn geen ontvangers aan deze mailing toegevoegd.<br>\n";
				}
				$this->verzendenmag = false;
			} else {
			}

			if (strlen($this->subject) == 0) {
				$this->meldingen .= "Het onderwerp moet ingevuld zijn. \n";
				$this->verzendenmag = false;
			}
			
			if (strlen($this->message) == 0) {
				$this->meldingen .= "Het bericht moet ingevuld zijn. \n";
				$this->verzendenmag = false;
			}
			if (strtotime($this->sent_on) > (time() - $this->vertraging_tussen_verzenden)) {
				if ($this->vertraging_tussen_verzenden < 210) {
					$this->meldingen .= sprintf("Er moet minimaal %d seconden zitten tussen het versturen van dezelfde mailing. \n", $this->vertraging_tussen_verzenden);
				} else {
					$this->meldingen .= sprintf("Er moet minimaal %d minuten zitten tussen het versturen van dezelfde mailing. \n", $this->vertraging_tussen_verzenden/60);
				}
				$this->verzendenmag = false;
				
			}
			
			foreach($this->MergeField as $fld) {
				$nm = "[%" . $fld['Naam'] . "%]";
				if (strpos($this->message, $nm) !== false) {
					$this->contains_mergefield = true;
				}
			}
			
			$hv = 0;
			while ($hv < strlen($this->message)) {
				$bp = strpos($this->message, "[%", $hv);
				if ($bp !== false) {
					$bp += 2;
					$ep = strpos($this->message, "%]", $bp);
					if ($ep !== false) {
						if (array_search (substr($this->message, $bp, ($ep-$bp)), array_column($this->MergeField, 'Naam')) == false) {
							$this->meldingen .= sprintf("%s is hier geen beschikbare variable.", substr($this->message, $bp-2, ($ep-$bp)+4));
						}
						$hv = $ep + 2;
					} else {
						$hv = $bp;
					}
				} else {
					$hv = strlen($this->message);
				}
			}
			
			if ($this->contains_mergefield == true and $this->bevat_losse_email == true) {
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
	} # controle
	
	public function post_form($p_actie="save") {
		
		$i_M = new cls_Mailing();
		$i_mr = new cls_Mailing_rcpt();
		
		// ** Gerelateerde tabellen bijwerken.
		if (isset($_POST['add_lid']) and $_POST['add_lid'] > 0) {
			$i_mr->add($this->mid, $_POST['add_lid']);
		} elseif (isset($_POST['add_address']) and strlen($_POST['add_address']) > 5) {
			$i_mr->add($this->mid, 0, $_POST['add_address']);
		}
		if ($p_actie == "add_selectie") {
			$this->add_del_selectie("add");
			
		} elseif ($p_actie == "del_rcpt" and isset($_GET['lidid']) and $_GET['lidid'] > 0) {
			$i_mr->delete($this->mid, $_GET['lidid']);
		} elseif ($p_actie == "del_rcpt" and isset($_GET['addr']) and strlen($_GET['addr']) > 0) {
			$i_mr->delete($this->mid, 0, $_GET['addr']);
		} elseif ($p_actie == "del_selectie") {
			$this->add_del_selectie("del");
		}  elseif ($p_actie == "delete_all_rcpt") {
			$i_mr->delete_all($this->mid);
		}
		
		/* Actie mbt uploads verwerken	
		if ($p_actie == "upload") {
			$this->upload();
		} elseif ($p_actie == "del_attach" and isset($_GET['attach'])) {
			$this->attach_delete($_GET['attach'], "bijlage");
		} elseif ($p_actie == "htmlmedia" and isset($_GET['attach'])) {
			$this->htmlmedia($_GET['attach']);
		} elseif ($p_actie == "del_media" and isset($_GET['attach'])) {
			$this->attach_delete($_GET['attach'], "media");
		}
		*/
			
		if ($p_actie == "afbeeldingalsbericht" and isset($_GET['attach'])) {
		
			$url = BASISURL . str_replace(BASEDIR, "", $this->dir_media) . "/" . $this->mid . "/" . str_replace("!", ".", $p_bestand);
							
			$m = sprintf("<html>\n<head>\n<title>%s</title>\n</head>\n<body>\n", $this->subject);
			$m .= sprintf("<img src='%1\$s' style='display:block;margin-right:auto;margin-left:auto;'>\n<br>\n<p style='text-align:center;font-size:9pt;'>Indien deze e-mail niet goed leesbaar is klik dan <a href='%1\$s'>hier</a>.</p>\n", $url);
			$m .= "</body>\n</html>\n";
			
			$_POST['message'] = $m;
			$_POST['ZonderBriefpapier'] = 1;
			$_POST['HTMLdirect'] = 1;
		}
		
		if (isset($_POST['subject']) and strlen($_POST['subject']) > 1) {
			$this->subject = html_entity_decode(str_replace("\"", "'", $_POST['subject']));
			$_POST['subject'] = $this->subject;
		}
		
		if ($this->mid == 0) {
			$this->mid = $i_M->add($this->subject);
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
		if (isset($_POST['HTMLdirect'])) {
			$_POST['HTMLdirect'] = 1;
		} else {
			$_POST['HTMLdirect'] = 0;
		}
		
		if (isset($_POST['ZonderBriefpapier'])) {
			$_POST['ZonderBriefpapier'] = 1;
		} else {
			$_POST['ZonderBriefpapier'] = 0;
		}
				
		if ($this->mid > 0) {
			$old = $i_M->record($this->mid);
			foreach($_POST as $key => $val) {
				if (array_key_exists($key, $old)) {
					$i_M->update($this->mid, $key, $val);
				}
			}
		}
		
		$this->vuldbvars();
		
		$i_M = null;
		$i_mr = null;
		
	} # post_form
	
	public function upload() {
		
		if ((isset($_FILES['UploadFile']['name']) and strlen($_FILES['UploadFile']['name']) > 3) or (isset($_FILES['UploadMedia']['name']) and strlen($_FILES['UploadMedia']['name']) > 3) and $this->mid > 0) {
			if (isset($_POST['UploadMedia'])) {
				$ad = $this->dir_media;
				$ae = $this->allowed_ext_afb;
				$target = $ad . $_FILES['UploadMedia']['name'];
			} else {
				$ad = $this->dir_attachm;
				$ae = $this->allowed_ext;
				$target = $ad . $_FILES['UploadFile']['name'];
			}
			
			if (!file_exists($ad)) {
				if (mkdir($ad, 0755, true) == true) {
					$mess = sprintf("Directory '%s' is aangemaakt.", $ad);
				} else {
					$mess = sprintf("Directory '%s' bestaat niet en kan niet aangemaakt worden. Probeer het later opnieuw of neem contact op met de webmaster.", $ad);
					$ad = "";
				}
				(new cls_Logboek())->add($mess, 4, 0, 0, $this->mid);
			} else {
				chmod($ad, 0755);
			}

			$ext = explode(".", $target);
			$ext = strtolower($ext[count($ext) - 1]);
			if (strpos($ae, $ext) === false) {
				$mess = sprintf("In mailing %d kan bestand '%s' niet worden ge-upload, omdat de extensie niet is toegestaan. Alleen de volgende extensies zijn toegestaan: %s", $this->mid, $_FILES['UploadFile']['name'], $ae);
			} elseif (isset($_POST['UploadFile']) and $_FILES['UploadFile']['size'] > $this->max_size_attachm) {
				$mess = sprintf("In mailing %d kan het bestand '%s' niet worden bijgesloten, omdat het te groot is.", $this->mid, $_FILES['UploadFile']['name']);
			} elseif (strlen($ad) > 0) {
				if (isset($_POST['UploadMedia'])) {
					if (move_uploaded_file($_FILES['UploadMedia']['tmp_name'], $target) == false) {
						$mess = sprintf("Fout %d is opgetreden bij het uploaden van bestand '%s'. Probeer het later opnieuw of neem contact op met de webmaster.", $_FILES['UploadMedia']['error'], $_FILES['UploadMedia']['name']);
					} else {
						$mess = sprintf("In mailing %d (%s) is afbeelding '%s' bijgesloten.", $this->mid, $this->subject, $_FILES['UploadMedia']['name']);
					}
				} else {
					if (move_uploaded_file($_FILES['UploadFile']['tmp_name'], $target) == false) {
						$mess = sprintf("Fout %d is opgetreden bij het uploaden van bestand '%s'. Probeer het later opnieuw of neem contact op met de webmaster.", $_FILES['UploadFile']['error'], $_FILES['UploadFile']['name']);
					} else {
						$mess = sprintf("In mailing %d (%s) is bestand '%s' bijgesloten.", $this->mid, $this->subject, $_FILES['UploadFile']['name']);
					}
				}
			}
			(new cls_Logboek())->add($mess, 4, 0, 0, $this->mid);
		}
	}
	
	public function attach_delete($p_bestand, $p_soort="bijlage") {
		
		if ($p_soort == "bijlage") {
			$ad = $this->dir_attachm;
		} else {
			$ad = $this->dir_media;
		}
		$p_bestand = str_replace("!", ".", $p_bestand);
		if (unlink($ad . $p_bestand)) {
			$mess = sprintf("Bestand '%s' bij mailing %d is verwijderd.", $p_bestand, $this->mid);
		} else {
			$mess = sprint("Het is niet gelukt om bestand '%s' uit directory '%s' te verwijderen.", $p_bestand, $ad);
		}
		(new cls_Logboek())->add($mess, 4, 0, 0, $this->mid);
	}
	
	public function add_del_selectie($actie) {
		global $fromlid, $wherelid, $fromlidond, $wherelidond;
		
		if ($actie == "add") {
			$w = " AND (L.Overleden IS NULL) AND (L.Verwijderd IS NULL)";
		} else {
			$w = "";
		}
		
		$mess = "";
		
		$i_lid = new cls_Lid();
		$i_lm = new cls_Lidmaatschap();
		
		if (strlen($_POST['selectie_vangebdatum']) == 10 and $_POST['selectie_vangebdatum'] > $i_lid->min("GEBDATUM") and $_POST['selectie_vangebdatum'] < $i_lid->max("GEBDATUM")) {
			$w .= sprintf(" AND L.GEBDATUM >= '%s'", $_POST['selectie_vangebdatum']);
		} 
		if ($_POST['selectie_temgebdatum'] > "1900-01-01" and $_POST['selectie_temgebdatum'] < $i_lid->max("GEBDATUM")) {
			$w .= sprintf(" AND L.GEBDATUM <= '%s'", $_POST['selectie_temgebdatum']);
		}
		
		if ($_POST['selectie_groep'] == 0) {
			if (strlen($_POST['selectie_vanliddatum']) == 10 and $_POST['selectie_vanliddatum'] > $i_lm->min("LIDDATUM")) {
				$w .= sprintf(" AND LM.LIDDATUM >= '%s'", $_POST['selectie_vanliddatum']);
			}
			if (strlen($_POST['selectie_temliddatum']) == 10 and $_POST['selectie_temliddatum'] < $i_lm->max("LIDDATUM")) {
				$w .= sprintf(" AND LM.LIDDATUM <= '%s'", $_POST['selectie_temliddatum']);
			}
			
			$query = sprintf("SELECT DISTINCT L.RecordID FROM %s WHERE (NOT LM.Lid IS NULL) AND IFNULL(LM.Opgezegd, '9999-12-31') >= CURDATE()  %s;", $fromlid, $w);
			
		} else {
			if (strlen($_POST['selectie_vanliddatum']) == 10 and $_POST['selectie_vanliddatum'] > $i_lm->max("LIDDATUM") and $_POST['selectie_vanliddatum'] <= $i_lm->max("LIDDATUM")) {
				$w .= sprintf(" AND LO.Vanaf >= '%s'", $_POST['selectie_vangebdatum']);
			}
			if (strlen($_POST['selectie_temliddatum']) == 10 and $_POST['selectie_temliddatum'] >= $i_lm->min("LIDDATUM") and $_POST['selectie_temliddatum'] < $i_lm->max("LIDDATUM")) {
				$w .= sprintf(" AND LO.Vanaf <= '%s'", $_POST['selectie_temgebdatum']);
			}
			$query = sprintf("SELECT DISTINCT L.RecordID FROM %s WHERE %s AND LO.OnderdeelID=%d %s;", $fromlidond, $wherelidond, $_POST['selectie_groep'], $w);
		}
		$i_lid = null;
		$i_lm = null;
		
		$aant = 0;
		$i_mr = new cls_Mailing_rcpt();
		if ($actie == "add") {
			foreach(fnQuery($query) as $row) {
				if ($i_mr->add($this->mid, $row->RecordID) > 0) {
					$aant++;
				}
			}
			$mess = sprintf("Er zijn %d leden, via een selectie, aan mailing %d (%s) toegevoegd.", $aant, $this->mid, $this->subject);
		} else {
			foreach(fnQuery($query) as $row) {
				if ($i_mr->delete($this->mid, $row->RecordID) > 0) {
					$aant++;
				}
			}
			$mess = sprintf("Er zijn %d leden, via een selectie, bij mailing %d (%s) verwijderd.", $aant, $this->mid, $this->subject);
		}
		$i_mr = null;
		(new cls_Logboek())->add($mess, 4, 0, 0, $this->mid);
		
	} # add_del_selectie
	
	public function preview() {
		$this->post_form();
		$this->send_mailing(1);
	}	
	
	public function send($lidid=0) {
		
		$this->controle($lidid);
		if ($this->verzendenmag == false) {
			(new cls_Logboek())->add($this->meldingen, 4, 0, 1, $this->mid);
			return false;
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
	
	private function send_mailing($preview=0, $p_lidid=0) {
		
		if ($p_lidid == 0) {
			$rcpts = (new cls_Mailing_rcpt())->lijst($this->mid);
		} else {
			$rcpts = db_lid("lijst", "", $p_lidid);
		}
		
		$aant_rcpts = 0;
		$aant_send = 0;
		foreach($rcpts as $rcpt) {
			$email = new email(0, $this->mid);
			if ($rcpt->RecordID > 0) {
				$this->merge($rcpt->RecordID);
				$email->Subject = $this->merged_subject;
				$email->toevoegenlid($rcpt->RecordID);
			} elseif (strlen($rcpt->to_address) > 5) {
				$this->merge(0);
				$email->toevoegenadres($rcpt->to_address);
				$this->bevat_losse_email = true;
			}
			$email->bericht = $this->merged_message;
			
			if ($preview == 1) {
				$mail = new RBMmailer();
				$mail->IsHTML(true);
				$mail->Subject = $this->merged_subject;
				$mail->Body = $this->merged_message;
				if (strlen($this->to_name) > 0) {
					$mail->addstationary($this->to_name, "", 0, $this->zonderbriefpapier);
				} elseif (isset($rcpt->Naam_lid)) {
					$mail->addstationary($rcpt->Naam_lid, "", 0, $this->zonderbriefpapier);
				}
				echo($mail->Body);
				$mail = null;
			} elseif ($email->to_outbox() == true) {
				$aant_send++;
			}
			$aant_rcpts++;
			$email = null;
		}
		
		if ($aant_send > 0) {
			$mess = sprintf("Via mailing %d (%s) zijn %d e-mails in de outbox geplaatst.", $this->mid, $this->subject, $aant_send);
			if ($_SESSION['lidid'] > 0) {
				db_logboek("add", $mess, 4, 0, 1, $this->mid);
			} else {
				db_logboek("add", $mess, 4, 0, 0, $this->mid);
			}
			if (strlen($this->speciaal) > 1) {
				sentoutbox(3);
			}
			(new cls_Mailing())->updatesent($this->mid);
		}

		return $aant_send;
	} # send_mailing
	
	private function merge($lidid=0) {
		
		$this->merged_subject = str_ireplace("[%Naamvereniging%]", $_SESSION['settings']['naamvereniging'], $this->subject);
		$this->merged_subject = str_ireplace("[%Naamwebsite%]", $_SESSION['settings']['naamwebsite'], $this->merged_subject);
		$this->merged_subject = str_ireplace("[%URLwebsite%]", BASISURL, $this->merged_subject);
		
		$this->merged_message = str_ireplace("[%Naamvereniging%]", $_SESSION['settings']['naamvereniging'], $this->message);
		$this->merged_message = str_ireplace("[%Naamwebsite%]", $_SESSION['settings']['naamwebsite'], $this->merged_message);
		$this->merged_message = str_ireplace("[%URLwebsite%]", BASISURL, $this->merged_message);
		
		if ($_SESSION['settings']['login_maxinlogpogingen'] > 0 and db_logins("foutelogin", "", "", $lidid) > $_SESSION['settings']['login_maxinlogpogingen']) {
			$geblokt = sprintf("Er is %d keer geprobeerd met deze login in te loggen. Hierdoor is dit account geblokkeerd. Vraag aan de webmaster om deze weer vrij te geven.", $db_logins("foutelogin", "", "", $lidid));
		} else {
			$geblokt = "";
		}
		$this->merged_message = str_ireplace("[%Geblokkeerd%]", $geblokt, $this->merged_message);
		$this->merged_message = str_ireplace("[%IPaddress%]", $_SERVER['REMOTE_ADDR'], $this->merged_message);
		$this->merged_message = str_ireplace("[%GELDIGHEIDACTIVATIE%]", intval($_SESSION['settings']['login_geldigheidactivatie']), $this->merged_message);
		
		if ($this->mid == $_SESSION['settings']['mailing_validatielogin'] and $lidid > 0) {
			$nk = db_logins("nieuweactivitiekey", "", "", $lidid);
			$urlactivatie = sprintf("%s/index.php?tp=Validatie+login&lidid=%d&key=%s", $_SERVER["HTTP_HOST"], $lidid, $nk);
			$this->merged_message = str_replace("[%URLACTIVATIE%]", $urlactivatie, $this->merged_message);
		
		} elseif ($this->mid == $_SESSION['settings']['mailing_herstellenwachtwoord'] and $lidid > 0) {
			$nk = db_logins("wachtwoordreset", "", "", $lidid);
			$urlresetww = sprintf("%s/index.php?tp=Herstellen+wachtwoord&lidid=%d&key=%s", $_SERVER["HTTP_HOST"], $lidid, $nk);
			$this->merged_message = str_replace("[%URLRESETWW%]", $urlresetww, $this->merged_message);
			
		} elseif ($this->mid == $_SESSION['settings']['mailing_bevestigingopzegging'] and $this->xtranum > 0) {
			$this->merged_message = str_ireplace("[%Lidnummer%]", $this->xtranum, $this->merged_message);
			$f = sprintf("Lidnr=%d", $this->xtranum);
			$row = (new cls_Lidmaatschap())->basislijst($f)->fetch;
			if (isset($row->Opgezegd)) {
				$this->merged_message = str_ireplace("[%OpgezegdPer%]", stftime("%e %B %Y",strtotime($row->Opgezegd)), $this->merged_message);
			}
			
		} elseif ($this->mid == $_SESSION['settings']['mailing_bewakinginschrijving']) {
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
			$this->merged_message = str_ireplace("[%VoorwaardenInschrijving%]", $_SESSION['settings']['zs_voorwaardeninschrijving'], $this->merged_message);
			$this->merged_message = str_ireplace("[%InschrijvingBewaking%]", $bevins, $this->merged_message);
			
		} elseif ($this->mid == $_SESSION['settings']['mailing_bevestigingbestelling']) {
			if ($this->xtranum > 0) {
				$filter = sprintf("ORD.Ordernr=%d", $this->xtranum);
			} else {
				$filter = "";
			}
			$bestelling = "<table>\n
							<tr><th>Code</th><th>Omschrijving</th><th>Maat</th><th class='number'>Aantal</th><th class='number'>Bedrag</th><th>Opmerking</th><tr>\n";
			
			$totbedrag = 0;
			foreach ((new cls_Orderregel())->bevestiging($lidid, $filter) as $row) {
				$ordernr = $row->Ordernr;
				$bestelling .= sprintf("<tr><td>%s</td><td>%s</td><td>%s</td><td class='number'>%d</td><td class='number'>&euro;&nbsp;%s</td><td>%s</td></tr>\n", $row->Code, $row->Omschrijving, $row->Maat, $row->AantalBesteld, number_format($row->Bedrag, 2), $row->Opmerking);
				$totbedrag += $row->Bedrag;
			}
			$bestelling .= "</table>\n";
			$this->merged_subject = str_ireplace("[%Ordernummer%]", $this->xtranum, $this->merged_subject);
			$this->merged_message = str_ireplace("[%Ordernummer%]", $this->xtranum, $this->merged_message);
			$this->merged_message = str_ireplace("[%Bestelling%]", $bestelling, $this->merged_message);
			$this->merged_message = str_ireplace("[%Totaalbedrag%]", number_format($totbedrag, 2), $this->merged_message);
			$this->merged_message = str_ireplace("[%VoorwaardenBestelling%]", $_SESSION['settings']['zs_voorwaardenbestelling'], $this->merged_message);
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
		(new cls_Mailing())->trash($this->mid, "in");
	}
	
	public function undelete() {
		(new cls_Mailing())->trash($this->mid, "out");
	}
		
	private function vuldbvars() {
			
		if ($this->mid > 0) {
			$ml = (new cls_Mailing())->record($this->mid);
			
			$this->mailingvanafid = $ml->MailingVanafID;
			$this->vanafnaam = $ml->Vanaf_naam;
			$this->vanafadres = $ml->Vanaf_email;
			
			$this->to_name = trim($ml->to_name);
			$this->cc_addr = trim($ml->cc_addr);
			$this->subject = trim($ml->subject);
			$this->message = $ml->message;
			$this->template = $ml->template;
			$this->zichtbaarvoor = $ml->ZichtbaarVoor;
			$this->htmldirect = $ml->HTMLdirect;
			$this->zonderbriefpapier = $ml->ZonderBriefpapier;
			$this->Ingevoerd = $ml->Ingevoerd;
			$this->IngevoerdDoor = $ml->IngevoerdDoor;
			$this->Gewijzigd = $ml->Gewijzigd;
			$this->GewijzigdDoor = $ml->GewijzigdDoor;
			$this->sent_on = $ml->sent_on;
			$this->SentBy = $ml->SentBy;
			$this->deleted_on = $ml->deleted_on;
			$this->DeletedBy = $ml->DeletedBy;
			$f = sprintf("MailingID=%d", $this->mid);
			$this->aant_rcpt = (new cls_Mailing_rcpt())->aantal($f);
		} else {
			$this->Ingevoerd = date("Y-m-d");
		}

	} # vuldbvars
} # class Mailing

class email {

	private $mhid = 0;
	public $mailingid = 0;
	public $lidid = 0;
	private $ingevoerd = "";
	private $ingevoerddoor = "";
	private $verstuurdop = "";
	public $vanafnaam = "";
	public $vanafadres = "";
	public $omsontvangers = "";
	private $aantalontvangers = 0;
	public $aannaam = "";
	public $aanadres = "";
	public $cc = "";
	public $zichtbaarvoor = 0;
	
	public $onderwerp = "";
	public $bericht = "";
	public $xtrachar = "";
	public $xtranum = 0;
	
	function __construct($mhid, $mid=0) {
		$this->ingevoerd = strftime('%e %B %Y (%H:%M:%S)');
		$this->ingevoerddoor = $_SESSION['naamingelogde'];
		
		if ($mhid > 0) {
			$this->mhid = $mhid;
			$mhrow = db_Mailing_hist("record", 0, $this->mhid);
			if (isset($mhrow->MailingID)) {
				$this->mailingid = $mhrow->MailingID;
				$this->ingevoerd = strftime('%e %B %Y (%H:%M:%S)', strtotime($mhrow->Ingevoerd));
				if ($mhrow->IngevoerdDoor > 0) {
					$this->ingevoerddoor = (new cls_Lid())->Naam($mhrow->IngevoerdDoor);
				}
				if ($mhrow->send_on > "2010-01-01") {
					$this->verstuurdop = strftime('%e %B %Y (%H:%M:%S)', strtotime($mhrow->send_on));
				}
				$this->lidid = $mhrow->LidID;
				$this->vanafnaam = $mhrow->from_name;
				$this->vanafadres = htmlentities($mhrow->from_addr);
				$this->omsontvangers = htmlentities($mhrow->to_name);
				$this->aannaam = htmlentities($mhrow->Aan);
				$this->aanadres = $mhrow->to_addr;
				$this->cc = $mhrow->cc_addr;
				$this->onderwerp = $mhrow->subject;
				$this->bericht = $mhrow->message;
				$this->zichtbaarvoor = $mhrow->ZichtbaarVoor;
				$this->xtrachar = $mhrow->Xtra_Char;
				$this->xtranum = $mhrow->Xtra_Num;
			}
			
		} elseif ($mid > 0) {	
			$this->mailingid = $mid;
			$mrow = (new cls_Mailing())->record($this->mailingid);
			$this->vanafnaam = $mrow->Vanaf_naam;
			$this->vanafadres = htmlentities($mrow->Vanaf_email);
			$this->omsontvangers = htmlentities($mrow->to_name);
			$this->cc = $mrow->cc_addr;
			$this->onderwerp = $mrow->subject;
			$this->zichtbaarvoor = $mrow->ZichtbaarVoor;
		}
	}  # __construct
	
	public function toon() {
		
		echo("<div id='verstuurdemail'>\n");

		printf("<label>Klaar gezet op:</label><div class='veld2k'>%s</div><label>door:</label><div class='veld2k'>%s</div>\n", $this->ingevoerd, $this->ingevoerddoor);
		
		if (strlen($this->verstuurdop) > 0) {
			printf("<label>Verzonden:</label><div class='veld'>%s</div>\n", $this->verstuurdop);
		} else {
			printf("<label>Verzonden:</label><div class='veld'>Wacht in outbox</div>\n");
		}
		
		if (strlen($this->vanafnaam) > 0) {
			printf("<label>Van:</label><div class='veld2k'>%s</div><label>E-mail:</label><div class='veld2k'>%s</div>\n", $this->vanafnaam, $this->vanafadres);
		} else {
			printf("<label>Van:</label><div class='veld'>%s</div>\n", $this->vanafadres);
		}
		
		if (strlen($this->omsontvangers) > 0) {
			printf("<label>Aan:</label><div class='veld'>%s</div>\n", $this->omsontvangers);
		}
		
		if (strlen($this->aannaam) > 0 and $this->aannaam != $this->aanadres) {
			printf("<label>Ontvanger:</label><div class='veld2k'>%s</div>\n", $this->aannaam);
			printf("<label>E-mail:</label><div class='veld2k'>%s</div>\n", $this->aanadres);
		} else {
			printf("<label>Aan e-mail:</label><div class='veld'>%s</div>\n", $this->aanadres);
		}
		if (strlen($this->cc) > 0) {
			printf("<label>CC:</label><div class='veld'>%s</div>\n", $this->cc);
		}
		
		printf("<label>Onderwerp:</label><div class='veld'>%s</div>\n", $this->onderwerp);
		
		$b = str_replace("<!DOCTYPE html>", "", $this->bericht);
		$b = str_replace("<html>", "", str_replace("</html>", "", $b));
		$b = str_replace("<body>", "", str_replace("</body>", "", $b));
		$b = removetextblock($b, "<head>", "</head>");
		
		printf("<label>Bericht:</label><div class='bericht'>%s</div>\n", $b);
		
		echo("</div>  <!-- Einde verstuurdemail -->\n");
		
	} # toon
	
	public function toevoegenlid($lidid) {
	
		$row = db_lid("record", "", $lidid);
		if ($this->lidid == 0) {
			$this->lidid = $lidid;
		}
		if (isValidMailAddress($row->Email)) {
			$this->toevoegenadres($row->Email);
		} elseif (strlen($row->EmailOuders) > 5) {
			$this->toevoegenadres($row->EmailOuders);
		} elseif (isValidMailAddress($row->EmailVereniging)) {
			$this->toevoegenadres($row->EmailVereniging);
		}
		
		if ($this->xtrachar != "REK") {
			$this->aannaam = $row->NaamLid;
		}
		
		if ($row->GEBDATUM > date("Y-m-d", mktime(0, 0, 0, date("m"), date("d"), date("Y")-18)) and strlen($row->EmailOuders) > 5) {
			$this->toevoegenadres($row->EmailOuders, "cc");
		}
		$this->aantalontvangers++;
	}  # toevoegenlid
	
	public function toevoegenadres($adres, $tp="aan") {
		
		foreach(explode(",", str_replace(" ", "", str_replace(";", ",", $adres))) as $e) {
			if ($tp == "aan") {
				if (isValidMailAddress($e) and strpos($this->aanadres, $e) === false) {
					if (strlen($this->aanadres) > 0) {
						$this->aanadres .= ", ";
					}
					$this->aanadres .= trim($e);
				}
			} elseif ($tp == "cc") {
				if (isValidMailAddress($e) and strpos($this->aanadres, $e) === false and strpos($this->cc, $e) === false) {
					if (strlen($this->cc) > 0) {
						$this->cc .= ", ";
					}
					$this->cc .= trim($e);
				}
			}
		}
	}  # toevoegenadres
	
	public function to_outbox() {
		
		$rv = false;
		if ($this->xtrachar == "REK" and $_SESSION['settings']['mailing_rekening_vanafid'] > 0) {
			$i_mv = new cls_Mailing_vanaf($_SESSION['settings']['mailing_rekening_vanafid']);
			$this->vanafadres = $i_mv->vanaf_email;
			$this->vanafnaam = $i_mv->vanaf_naam;
			$i_mv = null;
		}
		
		if (strlen($this->aanadres) > 5 and strlen($this->vanafadres) > 5) {
			$this->mhid = (new cls_Mailing_hist())->add($this);
			if ($this->mhid > 0) {
				$rv = true;
			}
		}
		
		return $rv;
		
	}  # to_outbox

}  # class email

function fnRekeningenMailen($op) {
	global $currenttab, $currenttab2;
	
	$i_rek = new cls_Rekening();
	$i_p = new cls_Parameter();

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
				$mindat = $i_rek->min("Datum");
			}
			if (strtotime($maxdat) !== FALSE) {
				$maxdat = date("Y-m-d", strtotime($maxdat));
			} else {
				$maxdat = $i_rek->max("Datum");
			}
			$filter = sprintf("RK.seizoen=%d AND RK.Nummer >= %d AND RK.Nummer <= %d AND RK.Datum >= '%s' AND RK.Datum <= '%s'", $_POST['rekseizoen'], $_POST['minreknr'], $_POST['maxreknr'], $mindat, $maxdat);
			if (isset($_POST['reklid']) and is_numeric($_POST['reklid']) and $_POST['reklid'] > 0) {
				$filter .= sprintf(" AND RK.Lid=%d", $_POST['reklid']);
			}
			if (isset($_POST['nulrekeningen'])) {
				$i_p->update("mailing_rekening_nulrekversturen", 1);
			} else {
				$filter .= " AND RK.Bedrag <> 0";
				$i_p->update("mailing_rekening_nulrekversturen", 0);
			}
			if (!isset($_POST['eerderverzonden'])) {
				$filter .= sprintf(" AND (RK.Nummer NOT IN (SELECT Xtra_Num FROM %sMailing_hist WHERE Xtra_Char='REK'))", TABLE_PREFIX);
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
				$i_p->update("mailing_rekening_stuurnaar", $_POST['rekverstuuraan']);
			}
			
			if (isset($_POST['vanafid'])) {
				$i_p->update("mailing_rekening_vanafid", $_POST['vanafid']);
			}
						
			if (isset($_POST['mailing_rekening_regelnrsweglaten']) and $_POST['mailing_rekening_regelnrsweglaten'] == 1) {
				$i_p->update("mailing_rekening_regelnrsweglaten", 1);
			} else {
				$i_p->update("mailing_rekening_regelnrsweglaten", 0);
			}

			$selrek = "";
			$aantrek = 0;
			foreach(db_rekening("selectie", 0, $filter) as $rk) {
				$selrek .= sprintf("<li>%d - %s - &euro;&nbsp;%03.2f</li>\n", $rk->Nummer, $rk->DEBNAAM, $rk->Bedrag);
				$aantrek += 1;
			}
			
			if (strlen($selrek) > 0) {
				printf("<form method='post' action='%s?tp=%s/%s&amp;op=selectieversturen' name='mailrek'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
				echo("De volgende rekeningen zijn geselecteerd.\n");
				printf("<ul>\n%s</ul>\n<div class='clear'></div>\n", $selrek);
				printf("<input type='hidden' name='rekfilter' value=\"%s\">\n", $filter);
				echo("<div id='opdrachtknoppen'>\n");
				echo("Rekeningen versturen?&nbsp<input type='checkbox' value='1' name='sure'>\n");
				echo("<input type='submit' value='Rekeningen versturen' name='StuurRek'>\n");
				echo("<input type='button' value='Terug' onClick='history.go(-1);'>\n");
				echo("</div>  <!-- Einde opdrachtknoppen -->\n");
				echo("</form>\n");
			} else {
				echo("<p class='mededeling'>Er zijn geen rekeningen geselecteerd.</p>");
			}
		} elseif ($op == "selectieversturen" and isset($_POST['sure'])) {
			$aant_ok = 0;
				
			foreach(db_rekening("selectie", 0, $_POST['rekfilter']) as $rk) {
				$email = new email(0, 0);
				$email->xtrachar = "REK";
				$email->xtranum = $rk->Nummer;
				$email->toevoegenlid($rk->Lid);
				
				if ($_SESSION['settings']['mailing_rekening_stuurnaar'] > 1) {
					$hd = new DateTime($rk->Datum);
					$hd->sub(new DateInterval('P18Y'));
					$rr = db_rekening("regels", $rk->Nummer);
					foreach($rr as $regel) {
						if ($regel->GEBDATUM > "1900-01-01" and $regel->GEBDATUM <= $hd->format("Y-m-d")) {
							$email->toevoegenlid($regel->Lid);
						} elseif ($_SESSION['settings']['mailing_rekening_stuurnaar'] == 2 and strlen($regel->EmailOuders) > 5) {
							$email->toevoegenadres($regel->EmailOuders, "cc");
						} elseif ($_SESSION['settings']['mailing_rekening_stuurnaar'] == 3) {
							$email->toevoegenlid($regel->Lid);
						}
					}
					$rr = null;
				}
				
				$email->bericht = RekeningDetail($rk->Nummer);
				if ($rk->Ouders == 1) {
					$email->aannaam = "Ouders/verzorgers van " . $rk->DEBNAAM;
				} else {
					$email->aannaam = $rk->DEBNAAM;
				}
				$email->onderwerp = $rk->OMSCHRIJV;
				
				if (strlen($email->aanadres) == 0) {
					$mess = sprintf("Rekening %d kon niet worden verzonden, omdat er van %s geen geldig e-mailadres in de database bekend is.", $rk->Nummer, $email->aannaam);
				} else {
					if ($email->to_outbox()) {
						$mess = sprintf("Rekening %d is naar %s (%s) verstuurd.", $rk->Nummer, $email->aannaam, $email->aanadres);
						$aant_ok++;
					} else {
						$mess = sprintf("Rekening %d kon niet worden verzonden.", $rk->Nummer);
					}
				}
				db_logboek("add", $mess, 4, 0, 0, $rk->Nummer);
				$email = null;
			}
		} elseif ($op == "selectieversturen") {
			$mess = "De rekeningen zijn niet verzonden, omdat het vinkje niet gezet was.";
			db_logboek("add", $mess, 4, 0, 1);
		}
	}
		
	if ($_SERVER['REQUEST_METHOD'] != "POST" or $op == "selectieversturen") {
		$i_p->vulsessie();
		printf("<form method='post' action='%s?tp=%s/%s&amp;op=selectierekeningen' name='mailrek'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
		echo("<table>\n");
		echo("<tr><th></th><th>Gelijk aan</th><th>Vanaf</th><th>Totenmet</th></tr>");
		printf("<tr><td class='label'>Seizoen</th><td><select name='rekseizoen'>\n%s</select></td><td></td><td></td></tr>\n", (new cls_Seizoen())->htmloptions());
		printf("<tr><td class='label'>Rekeningnummer</td><td><input type='number' name='reknr'></td><td><input type='number' value=%d name='minreknr'></td><td><input type='number' value=%d name='maxreknr'></td></tr>\n", $i_rek->min("Nummer"), $i_rek->max("Nummer"));
		printf("<tr><td class='label'>Rekeningdatum</td><td><input type='date' name='rekdatum'></td><td><input type='date' value='%s' name='mindatum'></td><td><input type='date' value='%s' name='maxdatum'></td></tr>\n", $i_rek->min("Datum"), $i_rek->max("Datum"));			
		printf("<tr><td class='label'>Betaald door (debiteur)</td><td><select name='reklid'>\n<option value='-1'>*** Iedereen ***</option>\n%s</select></td><td></td><td></td></tr>\n", (new cls_lid())->htmloptions(-1, 3));
		echo("<tr><td class='label'>Statussen</td><td colspan=3>");
		echo("<input type='checkbox' name='volbetaald' value=1 checked>Volledig betaald\n");
		echo("<input type='checkbox' name='deelbetaald' value=1 checked>Gedeeltelijk betaald\n");
		echo("<input type='checkbox' name='nietbetaald' value=1 checked>Niet betaald\n");
		printf("<input type='checkbox' name='nulrekeningen' value=1 %s>Nul rekeningen\n", checked($_SESSION['settings']['mailing_rekening_nulrekversturen']));
		echo("<input type='checkbox' name='eerderverzonden' value=1 checked>Eerder verzonden\n");
		echo("<input type='checkbox' name='geenemail' value=1>Geen e-mailadres\n");
		echo("</table>\n");
		
		echo("<div id='instellingenmuteren'>\n");
		echo("<h2>Instellingen</h2>\n");
		echo("<label>Rekening versturen aan:</label><select name='rekverstuuraan'>\n");
		$sn[1] = "Alleen gekoppeld lid";
		$sn[2] = "Gekoppeld lid en alle volwassenen op de rekening";
		$sn[3] = "Alle leden op de rekening";
		foreach($sn as $key => $val) {
			$s = "";
			if ($_SESSION['settings']['mailing_rekening_stuurnaar'] == $key) {
				$s = " selected";
			}
			printf("<option value=%d%s>%s</option>>\n", $key, $s, $val);
		}
		echo("</select>\n");
		
		$i_mv = new cls_Mailing_vanaf();
		printf("<label>Vanaf e-mailadres</label><select name='vanafid'>%s</select>\n", $i_mv->htmloptions($_SESSION['settings']['mailing_rekening_vanafid']));
		$i_mv = null;
		
		printf("<label>Regelnummers weglaten</label><input name='mailing_rekening_regelnrsweglaten' type='checkbox'%s value=1>\n", checked($_SESSION['settings']['mailing_rekening_regelnrsweglaten']));
		echo("</div> <!-- Einde instellingenmuteren -->\n");
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<input type='submit' value='Verder'>\n");
		echo("</div>  <!-- Einde opdrachtknoppen -->\n");
		
		echo("</form>\n");
	}
	echo("</div> <!-- Einde rekeningenmailen -->\n");
	
	$i_p = null;
} # fnRekeningenMailen

function fnMailingInstellingen() {
	global $currenttab, $currenttab2;
	
	echo("<div id='instellingenmuteren'>\n");
	$i_p = new cls_Parameter();
	$i_m = new cls_Mailing();
	$i_mv = new cls_Mailing_vanaf();
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		if (isset($_POST['mailing_direct_verzenden']) and $_POST['mailing_direct_verzenden'] == "1") {
			$i_p->update("mailing_direct_verzenden", 1);
		} else {
			$i_p->update("mailing_direct_verzenden", 0);
		}

		if (isset($_POST['mailing_sentoutbox_auto']) and $_POST['mailing_sentoutbox_auto'] == "1") {
			$i_p->update("mailing_sentoutbox_auto", 1);
		} else {
			$i_p->update("mailing_sentoutbox_auto", 0);
		}
		
		$i_p->update("mailing_alle_zien", $_POST['mailing_alle_zien']);
		$i_p->update("mailing_mailopnieuw", $_POST['mailing_mailopnieuw']);
		$i_p->update("mailing_bewaartijd", intval("0" . $_POST['mailing_bewaartijd']));
		$i_p->update("maxmailsperminuut", intval("0" . $_POST['maxmailsperminuut']));
		$i_p->update("maxmailsperuur", intval("0" . $_POST['maxmailsperuur']));
		$i_p->update("maxmailsperdag", intval("0" . $_POST['maxmailsperdag']));
		$i_p->update("path_attachments", $_POST['path_attachments']);
		$i_p->update("mailing_extensies_toegestaan", $_POST['mailing_extensies_toegestaan']);
		$i_p->update("max_grootte_bijlage", $_POST['max_grootte_bijlage']);
		
		$i_p->update("mailing_type_editor", $_POST['mailing_type_editor']);
		if (isset($_POST['mailing_tinymce_apikey'])) {
			if (strlen($_POST['mailing_tinymce_apikey']) < 8) {
				$_POST['mailing_tinymce_apikey'] = "no-api-key";
			}
			$i_p->update("mailing_tinymce_apikey", $_POST['mailing_tinymce_apikey']);
		}
		$i_p->update("mailing_lidnr", $_POST['mailing_lidnr']);
		$i_p->update("mailing_validatielogin", $_POST['mailing_validatielogin']);
		$i_p->update("mailing_herstellenwachtwoord", $_POST['mailing_herstellenwachtwoord']);
		$i_p->update("mailing_bevestigingopzegging", $_POST['mailing_bevestigingopzegging']);
		
		$kolomnamen = array("Vanaf_email", "Vanaf_naam", "SMTP_server");
		foreach ($_POST as $contr_name => $contr_val) {
			if (startwith($contr_name, "delete_vanaf")) {
				$rid = intval(str_replace("delete_vanaf_", "", $contr_name));
				$i_mv->delete($rid);
			} elseif ($contr_name == "vanaf_adres_toevoegen") {
				$i_mv->add();
			} else {
				foreach ($kolomnamen as $kolomnaam) {
					if (startwith($contr_name, $kolomnaam . "_")) {
						$rid = intval(str_replace($kolomnaam . "_", "", $contr_name));
						$i_mv->update($rid, $kolomnaam, $contr_val);
					}
				}
			}
		}
		
	}
	$i_p->vulsessie();
	
	$p = $_SESSION['settings']['path_attachments'];
	if (strlen($p) < 5 or !is_dir($p)) {
		$i_p->update("path_attachments", BASEDIR . "/attachments/");
	} elseif (substr($p, -1) != "/") {
		$i_p->update("path_attachments", $p . "/");
	}
	if (intval($_SESSION['settings']['max_grootte_bijlage']) < 5) {
		$i_p->update("max_grootte_bijlage", 2048);
	}

	$i_p = null;
	
	printf("<form method='post' action='%s?tp=%s/%s' name='mailinstellingen'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
	
	$options = sprintf("<option value=0%s>Nee</option>\n", checked($_SESSION['settings']['mailing_mailopnieuw'], "option", 0));
	$options .= sprintf("<option value=1%s>Alleen door webmasters</option>\n", checked($_SESSION['settings']['mailing_mailopnieuw'], "option", 1));
	$options .= sprintf("<option value=2%s>Ja</option>\n", checked($_SESSION['settings']['mailing_mailopnieuw'], "option", 2));
	
	echo("<h2>Algemeen</h2>\n");
	printf("<label>E-mails in de outbox direct probereren te versturen?</label><input type='checkbox' name='mailing_direct_verzenden' value=1%s>\n", checked($_SESSION['settings']['mailing_direct_verzenden']));
	printf("<label>E-mails uit de outbox automatisch in de achtergrond versturen?</label><input type='checkbox' name='mailing_sentoutbox_auto' value=1%s>\n", checked($_SESSION['settings']['mailing_sentoutbox_auto']));
	printf("<label>Mag een verzonden e-mail opnieuw worden verstuurd?</label><p><select name='mailing_mailopnieuw'>%s</select></p>\n", $options);
	
	$options = sprintf("<option value=-1%s>Webmasters</option>\n", checked($_SESSION['settings']['mailing_alle_zien'], "option", -1));
	
	foreach ((new cls_Onderdeel())->lijst(1) as $row) {
		$options .= sprintf("<option value=%d%s>%s</option>\n", $row->RecordID, checked($_SESSION['settings']['mailing_alle_zien'], "option", $row->RecordID), $row->Naam);
	}
	printf("<label>Wie mogen alle mailings zien en muteren?</label>\n<p><select name='mailing_alle_zien'>%s</select></p>\n", $options);

	printf("<label>Hoeveel maanden moeten mailings in de prullenbak bewaard blijven: (0 = altijd bewaren)</label><input type='number' name='mailing_bewaartijd' value=%d>\n", $_SESSION['settings']['mailing_bewaartijd']);
	printf("<label>Hoeveel mails mogen er per minuut verstuurd worden: (0 = onbeperkt)</label><input type='number' name='maxmailsperminuut' value=%d>\n", $_SESSION['settings']['maxmailsperminuut']);
	printf("<label>Hoeveel mails mogen er per uur verstuurd worden: (0 = onbeperkt)</label><input type='number' name='maxmailsperuur' value=%d>\n", $_SESSION['settings']['maxmailsperuur']);
	printf("<label>Hoeveel mails mogen er per 24 uur verstuurd worden: (0 = onbeperkt)</label><input type='number' name='maxmailsperdag' value=%d>\n", $_SESSION['settings']['maxmailsperdag']);
	$i_mh = new cls_Mailing_hist();
	printf("<label>Aantal verstuurde mails in de afgelopen uur / 24 uur:</label><p>%d / %d</p>\n", db_mailing_hist("aantalperuur"), db_mailing_hist("aantal24uur"));
	$i_mh = null;
	printf("<label>Waar worden de attachments bewaard?</label><input type='text' name='path_attachments' value='%s'>\n", $_SESSION['settings']['path_attachments']);
	printf("<label>Welke extensies zijn als attachment toegestaan: (leeg = standaard lijst)</label><input type='text' name='mailing_extensies_toegestaan' value='%s'>\n", $_SESSION['settings']['mailing_extensies_toegestaan']);
	printf("<label>Wat is de maximale groootte van één attachment?</label><input type='number' name='max_grootte_bijlage' value=%d><p>KB (nul = 2MB)</p>\n", $_SESSION['settings']['max_grootte_bijlage']);
	
	$options = "";
	foreach(ARRTYPEEDITOR as $key => $oms) {
		$options .= sprintf("<option%s value=%d>%s</option>\n", checked($key, "option", $_SESSION['settings']['mailing_type_editor']), $key, $oms);
	}
	printf("<label>Welke editor wil je gebruiken?</label><select name='mailing_type_editor'>%s</select>\n", $options);
	if ($_SESSION['settings']['mailing_type_editor'] == 3) {
		printf("<label>Wat is de API-key voor TinyMCE?</label><input type='text' name='mailing_tinymce_apikey' value='%s'>\n", $_SESSION['settings']['mailing_tinymce_apikey']);
	}
	
	echo("<h2>Mailings met een specifiek doel</h2>\n");
	
	$rows = $i_m->lijst("Templates");
	$options = "";
	foreach($rows as $row) {
		$options .= sprintf("<option%s value=%d>%s</option>", checked($row->lnkMailingID, "option", $_SESSION['settings']['mailing_lidnr']), $row->lnkMailingID, $row->Onderwerp);
	}
	printf("<label>Versturen lidnummer:</label><select name='mailing_lidnr'><Option value=0>Geen</option>%s</select>\n", $options);
	
	$options = "";
	foreach($rows as $row) {
		$options .= sprintf("<Option%s value=%d>%s</option>", checked($row->lnkMailingID, "option", $_SESSION['settings']['mailing_validatielogin']), $row->lnkMailingID, $row->Onderwerp);
	}
	printf("<label>Versturen validatie e-mail login:</label><select name='mailing_validatielogin'>%s</select>\n", $options);
	
	$options = "";
	foreach($rows as $row) {
		$options .= sprintf("<Option%s value=%d>%s</option>", checked($row->lnkMailingID, "option", $_SESSION['settings']['mailing_herstellenwachtwoord']), $row->lnkMailingID, $row->Onderwerp);
	}
	printf("<label>Versturen link herstellen wachtwoord:</label><select name='mailing_herstellenwachtwoord'>%s</select>\n", $options);

	$options = "<Option value=0>Geen</option>";
	foreach($rows as $row) {
		$options .= sprintf("<Option%s value=%d>%s</option>", checked($row->lnkMailingID, "option", $_SESSION['settings']['mailing_bevestigingopzegging']), $row->lnkMailingID, $row->Onderwerp);
	}
	$rows = null;
	printf("<label>Versturen bevestiging opzegging lidmaatschap:</label><select name='mailing_bevestigingopzegging'>%s</select>\n", $options);
	
	echo("<div class='clear'></div>\n");
	
	$link_lk = "<button type='submit' name='delete_vanaf_%d' title='Verwijder vanaf adres %d' onclick='this.submit();'><img src='./images/del.png'></button>";
	$xr = "<td colspan=6><button type='submit' name='vanaf_adres_toevoegen' title='Vanaf adres toevoegen' value='Vanaf adres toevoegen'><img src='./images/star.png'></button></td>";
	
	echo(fnDisplayTable(db_mailing_vanaf("editlijst"), "", "Vanaf adressen", 0, $link_lk, $xr, "editmailingvanaf", "", "Vanaf_naam, emlVanaf_email"));
	
	echo("<div id='opdrachtknoppen'>\n");
	echo("<input type='submit' value='Bewaren' name='InstellingenBewaren'>\n");
	echo("</div> <!-- Einde opdrachtknoppen -->\n");
	echo("</form>");
	echo("</div> <!-- Einde instellingenmuteren -->\n");
	
} # fnMailingInstellingen

function sentfromhist($p_mhid, $p_handm=0) {
	
	$rv = false;
	$mess = "";
	
	$row = (new cls_mailing_hist())->record($p_mhid);
	
	$mail = new RBMmailer();
	$mail->IsHTML(true);
	foreach (explode(",", $row->to_addr) as $e) {
		$e = trim($e);
		if (isValidMailAddress($e)) {
			$mail->AddAddress($e, $row->Aan);
		}
	}
	foreach (explode(",", $row->cc_addr) as $e) {
		$e = trim($e);
		if (isValidMailAddress($e)) {
			$mail->AddCC($e);
		}
	}
	$mail->From = $row->from_addr;
	$mail->FromName = $row->from_name;
	if ($row->send_on > "2000-01-01" and strpos($row->subject, "opnieuw verzonden") == false) {
		$mail->Subject = $row->subject . " (opnieuw verzonden)";
	} else {
		$mail->Subject = $row->subject;
	}
	$mail->Body = $row->message;
	
	if (strlen($row->to_name) > 0) {
		$a = $row->to_name;
	} else {
		$a = $row->Aan;
	}
	if ($mail->addstationary($a, "", 0, $row->ZonderBriefpapier)) {

		$ad = $_SESSION['settings']['path_attachments'] . $row->MailingID;
		if (is_dir($ad)) {
			$d = dir($ad);
			while (false !== ($entry = $d->read())) {
				if ($entry != "." and $entry != "..") {
					$mail->AddAttachment($ad . "/" . $entry);
				}
			}
			$d->close();
		}

		$error = "";
		try {
			$rv = $mail->Send();
		} catch (phpmailerException $e) {
			$error = $e->errorMessage();
			debug("phpmailerException: " . $error);
		} catch (Exception $e) {
			$error = $e->getMessage();
			debug("Exception: " . $error);
		}
		if (strlen($error) <4) {
			$error = $mail->ErrorInfo;
		}
		if ($rv) {
			(new cls_Mailing_hist())->update($p_mhid, "send_on", date("Y-m-d H:i:s"));
			if ($p_handm == 1) {
				$mess = sprintf("E-mail %d (%s) is aan %s verstuurd.", $p_mhid, $mail->Subject, $a);
			}
		} else {
			$mess = sprintf("Versturen van e-mail %d (%s) aan %s is mislukt. Foutboodschap: %s", $p_mhid, $mail->Subject, $a, $error);
		}
	}
	
	if (strlen($mess) > 0) {
		(new cls_Logboek())->add($mess, 4, $row->LidID, 1);
	}

	$mail = null;
	
	return $rv;
	
} # sentfromhist

function sentoutbox($mode=0) {
	
	/*
	Uitleg mode:
		0 = versturen middels knop door gebruiker
		1 = via batchjob
		2 = via optie 'mailing_direct_verzenden'
		3 = direct versturen in verband met speciaal soort
	*/
	
	$aantverzonden = 0;
	$mess = "";
	if ($mode == 1 and $_SESSION['settings']['mailing_sentoutbox_auto'] == 0) {
		$mess = sprintf("Het versturen in de achtergrond (via batchjob) is uitgeschakeld. Er staan %d e-mails in de outbox.", db_mailing_hist("aantaloutbox"));
		
	} elseif ($mode == 2 and $_SESSION['settings']['mailing_direct_verzenden'] == 0) {
		$mess = "Het direct versturen is uitgeschakeld.";
		
	} else {
		if ($mode == 1) {
			$a = "outboxbatch";
		} elseif ($mode == 3) {
			$a = "outboxspeciaal";
		} else {
			$a = "outbox";
		}
		set_time_limit(90);
	
		$stop = false;
		foreach(db_mailing_hist($a)->fetchAll() as $row) {
			if ($stop == false) {
				if (sentfromhist($row->lnkRecordID)) {
					$aantverzonden++;
				} else {
					$stop = true;
				}
			}
		}
	
		$aob = db_mailing_hist("aantaloutbox");
		if ($aantverzonden > 0 or $aob > 0) {
			if ($mode == 1) {
				$mess = sprintf("sentoutbox (mode:%d) is gestart door de batchjob", $mode);
			} else {
				$mess = sprintf("Versturen vanuit de outbox (mode: %d) is gereed", $mode);
			}
			$mess .= sprintf(", er zijn %d mails verzonden. Er staan nog %d mails te wachten in de outbox.", $aantverzonden, $aob);
		}
	}
	
	if (strlen($mess) > 0) {
		(new cls_Logboek())->add($mess, 4);
	}
	
	return $aantverzonden;
	
}  # sentoutbox

class RBMmailer extends PHPMailer\PHPMailer\PHPMailer {

	private $bestand_briefpapier;
	public $omsontvangers = "";

	function __construct() {
		global $smtphost, $smtpport, $smtpuser, $smtppw;
		
		if (!isset($smtphost)) {
			$mess = "Er is geen SMTP host in config.php gedefinieerd. Indien u geen SMTP wilt gebruiker, maak de variabele dan leeg.";
			(new cls_Logboek())->add($mess, 4);
			$this->IsMail(true);
			$this->IsSMTP(false);
		} elseif (strlen($smtphost) > 0) {
			$this->Host = $smtphost;
			$this->SMTPdebug = 4;
			if ($smtpport > 0) {
				$this->Port = $smtpport;
			}
			if ($smtpport == 587) {
				$this->SMTPSecure = "tls";
			}
			$this->IsSMTP(true);
			if (strlen($smtpuser) > 0) {
				$this->SMTPAuth = true;
				$this->Username = $smtpuser;
				$this->Password = $smtppw;
			} else {
				$this->SMTPAuth = false;
			}
		} elseif ($_SERVER["HTTP_HOST"] == "phprbm.telling.nl") {
			// Op het testsysteem alleen mails sturen als de smtp server is ingevuld. Dit om bij een storing te voorkomen dat er onnodige e-mails worden verstuurd.
		} else {
			$this->IsMail(true);
		}
		$this->CharSet = "UTF-8";
		$this->From = $_SESSION['settings']['emailwebmaster'];
		$this->FromName = $_SESSION['settings']['naamvereniging'];
		$this->IsHTML(true);
		$this->WordWrap = 110;
		$this->bestand_briefpapier = $_SESSION['settings']['path_templates'] . "briefpapier.html";
	}
	
	public function addstationary($to="", $from="", $verwijderaanhef=0, $p_zb=0) {
		
		if ($p_zb == 1) {
			if (startwith($this->Body, "<!DOCTYPE html>") == false) {
				$this->Body = "<!DOCTYPE html>\n" . $this->Body;
			}
			return true;
			
		} elseif (file_exists($this->bestand_briefpapier)) {
			if ($verwijderaanhef == 1) {
				$htmlmessage = removetextblock(file_get_contents($this->bestand_briefpapier), "<!-- Aanhef -->", "<!-- /Aanhef -->");
			} else {
				$htmlmessage = file_get_contents($this->bestand_briefpapier);
			}
			$htmlmessage = str_ireplace("[%MESSAGE%]", $this->Body, $htmlmessage);
			$htmlmessage = str_ireplace("[%FROM%]", $this->FromName, $htmlmessage);
			$htmlmessage = str_ireplace("[%TO%]", $to, $htmlmessage);
			$htmlmessage = str_ireplace("[%SUBJECT%]", $this->Subject, $htmlmessage);
			$this->Body = $htmlmessage;
			return true;
			
		} else {
			$mess = sprintf("Het bestand '%s' bestaat niet.", $this->bestand_briefpapier);
			db_logboek("add", $mess, 4, 0, 1);
			return false;
		}
	}
	
	public function Send() {
		
		$i_lb = new cls_Logboek();
				
		if ($_SESSION['settings']['maxmailsperminuut'] > 0 and db_mailing_hist("aantalperminuut") >= $_SESSION['settings']['maxmailsperminuut']) {
			$mess = sprintf("De limiet van aantal van %d te versturen mails per minuut is bereikt.", $_SESSION['settings']['maxmailsperminuut']);
			$i_lb->add($mess, 4);
			return false;
			
		} elseif ($_SESSION['settings']['maxmailsperdag'] > 0 and db_mailing_hist("aantal24uur") >= $_SESSION['settings']['maxmailsperdag']) {
			$mess = sprintf("De limiet van aantal van %d te versturen mails per 24 uur is bereikt.", $_SESSION['settings']['maxmailsperdag']);
			$i_lb->add($mess, 4);
			return false;
		
		} elseif ($_SESSION['settings']['maxmailsperuur'] > 0 and db_mailing_hist("aantalperuur") >= $_SESSION['settings']['maxmailsperuur']) {
			$mess = sprintf("De limiet van %d stuks te versturen mails per uur is bereikt.", $_SESSION['settings']['maxmailsperuur']);
			$i_lb->add($mess, 4);
			return false;
			
		} else {
			try {
				if(!$this->PreSend()) return false;
				return $this->PostSend();
			} catch (phpmailerException $e) {
				$this->SentMIMEMessage = '';
				$this->SetError($e->getMessage());
				if ($this->exceptions) {
					$i_lb->add($e->getMessage(), 4, 0, 1);
					throw $e;
				}
				return false;
			}
		}
	}
} # RBM_mailer

?>
