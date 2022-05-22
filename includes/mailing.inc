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
	
	if ($_SESSION['settings']['mailing_direct_verzenden'] > 0) {
		sentoutbox(3);
	}
	
	$mailing = new Mailing($_GET['mid']);
	
	if ($_GET['tp'] == "Mailing/Historie" and $op != "histdetails" and $op != "opnieuw") {
		$op = "historie";
	} elseif (isset($_POST['action']) and $_POST['action'] == "Bewaren") {
		$op = "save";
	} elseif (isset($_POST['action']) and $_POST['action'] == "Bewaren & sluiten") {
		$op = "save_close";
	} elseif (isset($_POST['action']) and $_POST['action'] == "Verstuur mailing") {
		$op = "send";
	} elseif (isset($_POST['action']) and $_POST['action'] == "Bekijk voorbeeld") {
		$op = "preview";
	} elseif (isset($_POST['action']) and $_POST['action'] == "Verwijderen") {
		$op = "verwijderen";
	} elseif (isset($_POST['action']) and $_POST['action'] == "Inhoud versturen") {
		$op = "sentoutbox";
	} elseif (isset($_POST['LedenToevoegen'])) {
		$op = "add_selectie";
	} elseif (isset($_POST['LedenVerwijderen'])) {
		$op = "del_selectie";
	} elseif (isset($_POST['OntvangersVerwijderen'])) {
		$op = "delete_all_rcpt";
		
	}
	
	if ($op == "htmlmedia" and isset($_GET['attach'])) {
		$mailing->htmlmedia($_GET['attach']);
	} elseif ($op == "del_media" and isset($_GET['attach'])) {
		$mailing->attach_delete($_GET['attach'], "media");
	}
	
	if ($currenttab2 != "previewwindow") {
		HTMLheader();
	}
	
	if (($op == "historie" or $op == "histdetails") and $mailing->mid > 0) {
		fnDispMenu(2, sprintf("mid=%d", $mailing->mid));
	} elseif ($currenttab2 != "previewwindow") {
		fnDispMenu(2);
	}
	
//	echo("<h2>Werk in uitvoering. Graag niet gebruiken.</h2>");

	$f = sprintf("RecordID=%d", $_GET['mid']);
	$zv = (new cls_Mailing())->max("ZichtbaarVoor", $f);
	$lg = explode(",", $_SESSION['lidgroepen']);
	if ($_SESSION['webmaster'] == 0 and $zv != 0 and in_array($zv, $lg) === false and in_array($_SESSION['settings']['mailing_alle_zien'], $lg) === false) {
		$mess = "Je hebt geen rechten om deze mailing te bekijken of te bewerken.";
		printf("<p class='mededeling'>%s</p>", $mess);
		
	} elseif ($currenttab2 == "Nieuw" and toegang($_GET['tp'], 1, 1)) {
		$mailing->mid = 0;
		$mailing->edit();
		
	} elseif ($op == "send") {
		$mailing->post_form();
		$mailing->send();
		$mailing->edit();
		
	} elseif ($currenttab2 == "Instellingen" and toegang($_GET['tp'], 1, 1)) {
		fnMailingInstellingen();
		
	} elseif ($op == "opnieuw" and isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0) {
		sentfromhist($_GET['MailingHistID']);
		echo("<script>window.history.back();</script>\n");
		
	} elseif ($op == "sentmail" and isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0) {
		sentfromhist($_GET['MailingHistID'], 1);
		lijstmailings($currenttab2);
				
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
			$kols[0]['link'] = sprintf("<a href='index.php?tp=%s&amp;op=histdetails&mhid=%%d'>%%s</a>", $_GET['tp']);
			$kols[0]['class'] = "details";
			$th = sprintf("%d. %s", $mailing->mid, $rows[0]->ndOnderwerp);
			echo(fnDisplayTable($rows, $kols, $th, 1));
		} else {
			echo("<p class='mededeling'>Deze mailing heeft geen historie.</p>\n");
		}
		
	} elseif ($op == "histdetails" and $_GET['mhid'] > 0) {
		$email = new email($_GET['mhid']);
		$email->toon();
		$mail = null;

	} elseif ($op == "sentoutbox") {
		sentoutbox(1);
		lijstmailings($currenttab2);
	} elseif ($op == "preview") {
		$mailing->post_form();
		$lnk = sprintf("%s?tp=%s/previewwindow&mid=%d", $_SERVER['PHP_SELF'], $currenttab, $mailing->mid);
		printf("<script>window.open('%s','_blank')</script>\n", $lnk);
		$mailing->edit();
	} elseif ($currenttab2 == "previewwindow") {
		$mailing->preview();
	} elseif ($op == "verwijderen") {
		$mailing->delete();
		lijstmailings("Muteren");
	} elseif ($op == "verwijderen ongedaan maken") {
		$mailing->undelete();
		lijstmailings("Muteren");
	} elseif ($currenttab2 == "Wijzigen mailing") {
		if ($_SERVER['REQUEST_METHOD'] == "POST" or strlen($op) > 0) {
			$mailing->post_form($op);
		}
		if ($op == "save_close") {
			printf("<script>location.href='%s?tp=Mailing/Muteren';</script>\n", $_SERVER['PHP_SELF']);
		} elseif (isset($_POST['toevoegen'])) {
			printf("<script>location.href='%s?tp=Mailing/Wijzigen mailing&mid=%d';</script>\n", $_SERVER['PHP_SELF'], $mailing->mid);
		} else {
			$mailing->edit();
		}
	} else {
		lijstmailings($currenttab2);
	}
		
	if ($currenttab2 != "previewwindow") {
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
	private $CCafdelingen = 0;
	private $subject = "";
	private $message = "";
	private $NietVersturenVoor = "0000-00-00";
	private $zichtbaarvoor = 0;
	private $template = 0;
	private $htmldirect = 0;
	public $zonderbriefpapier = 0;
	private $changed_on = "0000-00-00";
	private $Gewijzigd = "0000-00-00";
	private $IngevoerdDoor = 0;
	private $Ingevoerd = "0000-00-00";
	private $GewijzigdDoor = 0;
	private $SentBy = 0;
	private $deleted_on = "0000-00-00";
	private $DeletedBy = 0;
	private $evenementid = 0;
	
	private $ok_send = "";
	private $nok_send = "";
	private $meldingen = "";
	private $aant_ok = 0;
	private $aant_nok = 0;
	private $aant_rcpt = 0;
	private $aant_ontvangers_zonder_email = 0;
	private $verzendenmag = false;
	private $emailsecretariaat = "";
	
	private $sl_aantingroep = 0;
	private $sl_aanttoevoegen = 0;
	private $sl_aantverwijderen = 0;

	public $xtrachar = "";
	public $xtranum = 0;

	function __construct($p_mid=0) {
		
		$i_base = new cls_db_base();

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
								WHEN L.GEBDATUM > DATE_SUB(CURDATE(), INTERVAL 18 YEAR) AND LENGTH(IFNULL(L.Email, '')) > 5 AND LENGTH(IFNULL(L.EmailOuders, '')) > 5 THEN CONCAT(IF(LENGTH(L.Roepnaam) < 2, 'leden', L.Roepnaam), ' of ouder/verzorger van')
								WHEN LENGTH(IFNULL(L.Email, '')) > 5 AND LENGTH(IFNULL(L.EmailOuders, ''))=0 THEN IF(LENGTH(L.Roepnaam) < 2, 'leden', L.Roepnaam)
								WHEN LENGTH(IFNULL(L.Email, ''))=0 AND LENGTH(IFNULL(L.EmailOuders, '')) > 5 THEN CONCAT('ouder/verzorger van ', IF(LENGTH(L.Roepnaam) < 2, 'leden', L.Roepnaam))
								ELSE IF(LENGTH(IFNULL(L.Roepnaam, '')) < 2, 'leden', L.Roepnaam) END
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
							 
		$sql = sprintf("SELECT %s AS Lidnaam FROM %sLid AS L WHERE L.RecordID=%%d;", (new cls_db_base())->selectnaam, TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Lidnaam", 'SQL' => $sql);

		$sql = sprintf("SELECT Login FROM %sAdmin_login WHERE LidID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Login", 'SQL' => $sql);

		$sql = sprintf("SELECT LM.Lidnr FROM %sLidmaatschap AS LM WHERE IFNULL(LM.Opgezegd, '9999-12-31') >= CURDATE() AND LM.Lid=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Lidnummer", 'SQL' => $sql);
		
		$sql = sprintf("SELECT FLOOR(SUM(TIMESTAMPDIFF(MONTH, LM.LIDDATUM, IF(ISNULL(LM.Opgezegd), CURDATE(), LM.Opgezegd)))/12)
							FROM %sLidmaatschap AS LM WHERE LM.LIDDATUM < CURDATE() AND LM.Lid=%%d LIMIT 1;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "LengteLidmaatschap", 'SQL' => $sql);

		$sql = sprintf("SELECT IF(LENGTH(IFNULL(L.Telefoon, '')) < 10, L.Mobiel, IF(LENGTH(IFNULL(L.Mobiel, '')) < 10, L.Telefoon, CONCAT(L.Telefoon, ' / ', L.Mobiel))) FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Telefoon", 'SQL' => $sql);

		$sql = sprintf("SELECT DATE_FORMAT(L.GEBDATUM, %s, 'nl_NL') AS Geboortedatum FROM %sLid AS L WHERE L.RecordID=%%d;", $i_base->fdlang, TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Geboortedatum", 'SQL' => $sql);

		$sql = sprintf("SELECT L.GEBPLAATS FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Geboorteplaats", 'SQL' => $sql);

		$sql = sprintf('SELECT O.Naam FROM %1$sLidond AS LO INNER JOIN %1$sOnderdl AS O ON O.RecordID = LO.OnderdeelID
				 WHERE %2$s AND O.GekoppeldAanQuery=0 AND LO.Lid=%%d ORDER BY O.Naam;', TABLE_PREFIX, cls_db_base::$wherelidond);
		$this->MergeField[]=array('Naam' => "Onderdelen", 'SQL' => $sql);
	
		if ((new cls_db_base("Bewseiz"))->aantal() > 0) {
			$sql = sprintf('SELECT CONCAT(DATE_FORMAT(BEGIN_PER, %1$s, \'nl_NL\'), \' t/m \', DATE_FORMAT(EINDE_PER, %1$s, \'nl_NL\')) AS Per'
				  . ' FROM %2$sBewaking AS BW INNER JOIN %2$sBewseiz AS BS ON BS.RecordID = BW.SeizoenID'
				  . ' WHERE EINDE_PER >= CURDATE() AND %3$s ORDER BY BEGIN_PER;', $i_base->fdlang, TABLE_PREFIX, "Lid=%d");
			$this->MergeField[]=array('Naam' => "BewakingToekomst", 'SQL' => $sql);
							 
			$Funct = "(CASE BW.Functie WHEN 0 THEN '' ELSE CONCAT(' (', F.Afkorting, ')') END)";
			$sql = sprintf('SELECT CONCAT(DATE_FORMAT(BEGIN_PER, %1$s, \'nl_NL\'), \' t/m \', DATE_FORMAT(EINDE_PER, %1$s, \'nl_NL\'), %2$s) AS Per
					 FROM (%3$sBewaking AS BW INNER JOIN %3$sBewseiz AS BS ON BS.RecordID = BW.SeizoenID) INNER JOIN %3$sFunctie AS F ON BW.Functie = F.Nummer
					 WHERE BW.EINDE_PER < CURDATE() AND %4$s ORDER BY BW.BEGIN_PER;', $i_base->fdkort, $Funct, TABLE_PREFIX, "BW.Lid=%d");
			$this->MergeField[]=array('Naam' => "BewakingHistorie", 'SQL' => $sql);
		}
							  
		$sql = sprintf("SELECT L.Bankrekening FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Bankrekening", 'SQL' => $sql);
		
		$sql = sprintf("SELECT L.Burgerservicenummer FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "BSN", 'SQL' => $sql);

		if ((new cls_Rekening())->aantal() > 0 and toegang('Ledenlijst/Overzicht lid/Rekeningen', 0, 0)) {
			$sql = sprintf("SELECT RK.Nummer FROM %sRekening AS RK WHERE RK.Bedrag > 0 AND RK.Bedrag > RK.Betaald AND %s;", TABLE_PREFIX, "RK.Lid=%d");
			$this->MergeField[]=array('Naam' => "OpenstaandeRekeningen", 'SQL' => $sql);

			$sql = sprintf("SELECT ROUND(SUM(RK.Bedrag-RK.Betaald), 2) AS OpenstaandBedrag FROM %sRekening AS RK WHERE RK.Lid=%%d;", TABLE_PREFIX);
			$this->MergeField[]=array('Naam' => "OpenstaandBedrag", 'SQL' => $sql);
		}

		$sql = sprintf("SELECT (CASE L.`Machtiging afgegeven` WHEN 1 THEN 'Ja' ELSE 'Nee' END) AS MachtigingAfgegeven
							 FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "MachtigingAfgegeven", 'SQL' => $sql);
		
		$sql = sprintf("SELECT DISTINCT DP.Kode FROM %1\$sLiddipl AS LD INNER JOIN %1\$sDiploma AS DP ON LD.DiplomaID=DP.RecordID
				 WHERE IFNULL(LD.LicentieVervallenPer, '9999-12-31') >= CURDATE() AND LD.Lid=%%d ORDER BY DP.Volgnr, LD.DatumBehaald, DP.KODE;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "DiplomaKort", 'SQL' => $sql);

		$sql = sprintf("SELECT DISTINCT DP.Naam FROM %1\$sLiddipl AS LD INNER JOIN %1\$sDiploma AS DP ON LD.DiplomaID = DP.RecordID
				 WHERE IFNULL(LD.LicentieVervallenPer, '9999-12-31') >= CURDATE() AND LD.Lid=%%d ORDER BY DP.Volgnr, LD.DatumBehaald, DP.Naam;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "DiplomaLang", 'SQL' => $sql);
		
		if ((new cls_Diploma())->aantal("Zelfservice=1") > 0) {
			$sql = sprintf("SELECT DISTINCT DP.Naam FROM %1\$sLiddipl AS LD INNER JOIN %1\$sDiploma AS DP ON LD.DiplomaID = DP.RecordID
					WHERE IFNULL(LD.LicentieVervallenPer, '9999-12-31') >= CURDATE() AND LD.Lid=%%d AND DP.Zelfservice=1 ORDER BY DP.Volgnr, LD.DatumBehaald, DP.Naam;", TABLE_PREFIX);
			$this->MergeField[]=array('Naam' => "DiplomaZelfservice", 'SQL' => $sql);
		}
	
		$sql = sprintf("SELECT L.RelnrRedNed FROM %sLid AS L WHERE %s;", TABLE_PREFIX, "L.RecordID=%d");
		$this->MergeField[]=array('Naam' => "RelnrRedNed", 'SQL' => $sql);

		$sql = sprintf("SELECT DATE_FORMAT(LIDDATUM, %s, 'nl_NL') FROM %sLidmaatschap AS LM WHERE IFNULL(LM.Opgezegd, '9999-12-31') >= CURDATE() AND LM.Lid=%%d;", $i_base->fdlang, TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "LidVanaf", 'SQL' => $sql);

		$sql = sprintf("SELECT IF(LM.OPGEZEGD IS NULL, 'Niet', DATE_FORMAT(LM.Opgezegd, %s, 'nl_NL')) FROM %sLidmaatschap AS LM
							 WHERE LM.LIDDATUM < CURDATE() AND LM.Lid=%%d
							 ORDER BY LM.LIDDATUM DESC LIMIT 1;", $i_base->fdlang, TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "OpgezegdPer", 'SQL' => $sql);

		$sql = sprintf("SELECT IFNULL(L.Email, 'Geen') FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Email", 'SQL' => $sql);
		
		$sql = sprintf("SELECT IFNULL(L.EmailOuders, '') FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "EmailOuders", 'SQL' => $sql);

		$query = sprintf("SELECT COUNT(*) FROM %1\$sLidRedNed AS RN JOIN %1\$sLidmaatschap AS LM ON LM.LIDNR = RN.Lidnr WHERE LENGTH(RN.Foutboodschap) > 0;", TABLE_PREFIX);
		if ((new cls_db_base())->scalar($query) > 0) {
			$sql = sprintf("SELECT RN.Foutboodschap FROM %1\$sLidRedNed AS RN JOIN %1\$sLidmaatschap AS LM ON LM.LIDNR = RN.Lidnr WHERE LM.LID=%%d;", TABLE_PREFIX);
			$this->MergeField[]=array('Naam' => "VerschilSportlink", 'SQL' => $sql);
		}
		
		$i_ond = new cls_Onderdeel();

		if (count($i_ond->lijst(1, "O.`Type`='A'")) > 0) {
			$sql = sprintf("SELECT CONCAT(O.Naam, IF(LO.Functie > 0, CONCAT(' (', F.Omschrijv , ')'), ''), IF(LO.Opgezegd >= CURDATE(), CONCAT(' (eindigt per ', DATE_FORMAT(LO.Opgezegd, %s, 'nl_NL'), ')'), ''))
					FROM %s
					WHERE LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE() AND O.`Type`='A' AND LO.Lid=%%d ORDER BY LO.Vanaf;", $i_base->fdlang, $i_ond->fromlidond);
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
					 WHERE %s AND `Type`='R' AND LO.Lid=%%d ORDER BY O.Naam;", TABLE_PREFIX, $i_ond->table, cls_db_base::$wherelidond);
			$this->MergeField[]=array('Naam' => "Rollen", 'SQL' => $sql);
		}
		
				
		if (count($i_ond->lijst(1, "O.`Type`='T'")) > 0) {
			$sql = sprintf("SELECT O.Naam FROM %sLidond AS LO INNER JOIN %s AS O ON O.RecordID = LO.OnderdeelID 
					 WHERE %s AND `Type`='T' AND LO.Lid=%%d ORDER BY O.Naam;", TABLE_PREFIX, $i_ond->table, cls_db_base::$wherelidond);
			$this->MergeField[]=array('Naam' => "Toestemmingen", 'SQL' => $sql);
			$this->MergeField[]=array('Naam' => "Toestemmingen_UL", 'SQL' => $sql);
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
			$this->MergeField[]=array('Naam' => "OpmInschrijvingBewaking", 'SQL' => $sql);
		}
		$i_memo = null;
		
		if ($this->evenementid > 0) {
			
			$sql = "SELECT CASE ";
			foreach (ARRDLNSTATUS as $k => $v) {
				$sql .= sprintf("WHEN ED.Status='%s' THEN '%s' ", $k, $v);
			}
			$sql .= "END";
			$sql .= sprintf(" FROM %1\$sEvenement_Deelnemer AS ED WHERE ED.LidID=%%d AND ED.EvenementID=%2\$d;", TABLE_PREFIX, $this->evenementid);
			$this->MergeField[]=array('Naam' => "DeelnemerStatus", 'SQL' => $sql);
			
			$sql = sprintf("SELECT IF(E.MeerdereStartMomenten=1, LEFT(ED.StartMoment, 5), SUBSTRING(E.Datum, 12, 5)) FROM %1\$sEvenement_Deelnemer AS ED INNER JOIN %1\$sEvenement AS E ON E.RecordID=ED.EvenementID WHERE ED.LidID=%%d AND ED.EvenementID=%2\$d;", TABLE_PREFIX, $this->evenementid);
			$this->MergeField[]=array('Naam' => "DeelnemerStarttijd", 'SQL' => $sql);
			
			$sql = sprintf("SELECT IFNULL(ED.Functie, '') FROM %1\$sEvenement_Deelnemer AS ED WHERE ED.LidID=%%d AND ED.EvenementID=%2\$d;", TABLE_PREFIX, $this->evenementid);
			$this->MergeField[]=array('Naam' => "DeelnemerFunctie", 'SQL' => $sql);
			
			$sql = sprintf("SELECT IFNULL(ED.Opmerking, '') FROM %1\$sEvenement_Deelnemer AS ED WHERE ED.LidID=%%d AND ED.EvenementID=%2\$d;", TABLE_PREFIX, $this->evenementid);
			$this->MergeField[]=array('Naam' => "DeelnemerOpmerking", 'SQL' => $sql);
		}
		
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
	
	public function edit() {
		global $currenttab, $currenttab2, $navpad;
		
		$i_m = new cls_Mailing();
		$i_mr = new cls_Mailing_rcpt();
		$i_el = new cls_Eigen_lijst();
		$i_ond = new cls_Onderdeel();
		$ob = "OnBlur='this.form.submit();'";
		$jstb = sprintf("OnBlur=\"savedata('mailing', %d, this);\"", $this->mid);
		$jscb = sprintf("OnClick=\"savecb('mailing', %d, this);\"", $this->mid);
		$jsoc = sprintf("OnChange=\"savedata('mailing', %d, this);\"", $this->mid);
		
		if (file_exists(BASEDIR . '/maatwerk/email.css') and $this->zonderbriefpapier == 0) {
			$ss = "content_css: 'maatwerk/email.css',";
		} else {
			$ss = "";
		}
		$bnr = 1;
		$tabbijlagefiles = "";
		if (is_dir($this->dir_attachm)) {
			$d = dir($this->dir_attachm);
			while (false !== ($entry = $d->read())) {
				if ($entry != "." and $entry != "..") {
					$tabbijlagefiles .= sprintf("<tr>\n<td>%s</td>\n", $entry);
					$stat = stat($this->dir_attachm . $entry);
					$tabbijlagefiles .= sprintf("<td>%s KB</td>\n", number_format(($stat['size'] / 1024), 0, ',', '.'));
					$tabbijlagefiles .= sprintf("<td><input type='image' src='%s' name='del_attach_%d' alt='Verwijderen' title='Verwijder %s'></td>\n", BASE64_VERWIJDER_KLEIN, $bnr, $entry);
					$tabbijlagefiles .= sprintf("<input type='hidden' name='name_attach_%d' value='%s'>\n", $bnr, str_replace(".", "!", $entry));
					$bnr++;
				}
			}
			$d->close();
		}
		
		$img = "";
		$tabmediafiles = "";
		if (is_dir($this->dir_media)) {
			$md = dir($this->dir_media);
			$il = "";
			while (false !== ($entry = $md->read())) {
				if ($entry != "." and $entry != "..") {
					if (strlen($il) > 0) {
						$il .= ",\n";
					}
					$il .= sprintf("{title: '%s', value: '%s'}", $entry, './media/' . intval($this->mid) . '/' . $entry);
			
					$tabmediafiles .= sprintf("<tr>\n<td>%s</td>\n", $entry);
					$stat = stat($this->dir_media . "/" . $entry);
					$tabmediafiles .= sprintf("<td>%s KB</td>\n", number_format(($stat['size'] / 1024), 0, ',', '.'));
					if ($this->htmldirect == 1) {
						$lnk = sprintf("%s?tp=%s&op=htmlmedia&mid=%d&attach=%s", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, str_replace(".", "!", $entry));
						$tabmediafiles .= sprintf("<td><a href='%s'>HTML Media</a></td>\n", $lnk);
					}
					if (strlen($this->message) == 0) {
						$lnk = sprintf("%s?tp=%s&op=afbeeldingalsbericht&mid=%d&attach=%s", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, str_replace(".", "!", $entry));
						$tabmediafiles .= sprintf("<td><a href='%s'>Als bericht</a></td>\n", $lnk);
					}
					$lnk = sprintf("%s?tp=%s&op=del_media&mid=%d&attach=%s", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, str_replace(".", "!", $entry));
					$tabmediafiles .= sprintf("<td><input type='image' src='%s' name='del_media_%d' alt='Verwijderen' title='Verwijder %s'></td>\n", BASE64_VERWIJDER_KLEIN, $bnr, $entry);
					$tabmediafiles .= sprintf("<input type='hidden' name='name_attach_%d' value='%s'>\n", $bnr, str_replace(".", "!", $entry));
					$bnr++;
				}
			}
			if (strlen($il) > 0) {
				$img = sprintf("image_list: [\n%s],", $il);
			}
			$md->close();
		}
				
		if ($_SESSION['settings']['mailing_type_editor'] == 3) {
				$src = "/includes/tinymce/js/tinymce/tinymce.min.js";
				if (file_exists(BASEDIR . $src)) {
					$src = BASISURL . $src;
				} else {
					$src = sprintf("https://cdn.tiny.cloud/1/%s/tinymce/6/tinymce.min.js", $_SESSION['settings']['mailing_tinymce_apikey']);
				}
				printf("<script src='%s' referrerpolicy='origin'></script>\n", $src);
				printf("<script>
		tinymce.init({
			selector: '#message',
			%s
			height: 650,
			image_prepend_url: '%s/',
			relative_urls: false,
			convert_urls: false,
			remove_script_host : false,
			plugins: 'hr link lists table image paste importcss',
			paste_as_text: true,
			%s
			importcss_append: true,
			menu: {
				table: { title: 'Table', items: 'inserttable | cell row column | tableprops deletetable' }
			},
			toolbar: 'undo redo | styleselect | bold italic | alignleft aligncenter alignright alignjustify | bullist numlist | link image | hr | removeformat',
			menubar: 'edit insert format tools table'
		});
  </script>\n", $img, BASISURL, $ss);
  
		} elseif ($_SESSION['settings']['mailing_type_editor'] == 2) {
			echo("<script src='https://cdn.ckeditor.com/4.13.0/standard/ckeditor.js'></script>\n");
		} else {
			echo("<script src='https://cdn.ckeditor.com/ckeditor5/12.4.0/classic/ckeditor.js'></script>\n");
		}
		
		(new cls_Lidond())->autogroepenbijwerken();
		foreach ($i_el->lijst("AantalRecords=0 AND KolomLidID >= 0") as $elrow) {
			$i_el->controle($elrow->RecordID);
		}
		
		$f = sprintf("MailingID=%d", $this->mid);
		$this->aant_rcpt = $i_mr->aantal($f);
		
		if ($this->mid > 0) {
			$navpad[]['naam'] = sprintf("Details mailing %d", $this->mid);
		}
		
		if ($this->mid > 0) {
			$m = sprintf("&mid=%d", $this->mid);
		} else {
			$m = "";
		}	
		printf("<div class='editmailingform'>\n");
		printf("<form method='post' action='%s?tp=Mailing/Wijzigen+mailing%s' name='frm_mailedit' enctype='multipart/form-data'>\n", $_SERVER['PHP_SELF'], $m);
		
		if ($this->mid > 0) {
			printf("<label class='k2'>RecordID</label><p id='recordid'>%d</p>\n", $this->mid);
		}
		printf("<input type='hidden' name='mailingid' value=%d>\n", $this->mid);
		printf("<label>Van</label><select id='MailingVanafID' %s>\n<option value=''>Selecteer ...</option>%s</select>\n", $jsoc, (new cls_Mailing_vanaf())->htmloptions($this->mailingvanafid));
		printf("<label>Aan</label><input type='text' id='OmschrijvingOntvangers' value=\"%s\" maxlength=50 placeholder='Omschrijving groep personen aan wie de mailing is gericht' %s>\n", $this->to_name, $jstb);
		printf("<label>Onderwerp</label><input type='text' id='subject' value=\"%s\" maxlength=75 placeholder='Onderwerp' %s>\n", $this->subject, $jstb);

		$this->verzendenmag = false;
		if ($this->mid > 0) {
			if (strlen($this->speciaal) > 0) {
				printf("<label>Specifiek doel</label><p>%s</p>\n", $this->speciaal);
				$i_mr->delete_all($this->mid);
			} else {
				echo("<label id='lblOntvangers'>Ontvangers</label>");
				echo("<div class='invoerblok'>\n");
				echo("<ul class='lijstontvangers'>\n");
				echo("<div id='lijstontvangers'>\n");
//				echo($this->html_ontvangers());
				echo("</div> <!-- Einde lijstontvangers -->\n");
				echo("</ul> <!-- Einde lijstontvangers -->\n");
				echo("</div> <!-- Einde invoerblok -->\n");
				$this->verzendenmag = true;
				$this->meldingen = "";

				echo("<label>Ontvanger toevoegen</label>\n");
				printf("<select id='add_lid' onChange=\"mailing_add_ontvanger($(this).val(), '');\">\n");
//				echo($this->options_mogelijke_ontvangers($this->mid, 0));
				
				echo("<input type='checkbox' id='cb_alle_personen' OnClick=\"options_mogelijke_ontvangers();\"><p>Alle</p>");
			
				$_POST['selecteer_groep'] = $_POST['selecteer_groep'] ?? 0;
				echo("<input type='email' name='add_address' maxlength=50 placeholder='Toevoegen e-mailadres' onBlur='this.form.submit();'>");
				echo("<div class='clear'></div>\n");
				
				if (!isset($_POST['selectie_status'])) {
					$_POST['selectie_status'] = "L";
				}

				echo("<label>Selectie leden</label>\n");
				echo("<div id='mailingselectieleden'>\n");

				$i_lid = new cls_Lid();
				$i_lm = new cls_Lidmaatschap();
				
				$_POST['selectie_vangebdatum'] = $_POST['selectie_vangebdatum'] ?? $i_lid->min("GEBDATUM");
				$_POST['selectie_temgebdatum'] = $_POST['selectie_temgebdatum'] ?? $i_lid->max("GEBDATUM");
				
				printf("<label>Vanaf geboortedatum</label><input type='date' value='%s' name='selectie_vangebdatum' id='selectie_vangebdatum' OnBlur='mailingprops();'><p id='tekst_vangebdatum'></p>\n", $_POST['selectie_vangebdatum']);
				printf("<label>T/m geboortedatum</label><input type='date' value='%s' name='selectie_temgebdatum' id='selectie_temgebdatum' OnBlur='mailingprops();'><p id='tekst_temgebdatum'></p>\n", $_POST['selectie_temgebdatum']);

				$_POST['selectie_groep'] = $_POST['selectie_groep'] ?? 0;
				$selgr = sprintf("<option value=0>&nbsp;</option>\n%s<option disabled>-- Eigen lijsten --</option>\n%s</select>\n", $i_ond->htmloptions($_POST['selectie_groep'], 1), $i_el->htmloptions($_POST['selectie_groep'], 0));
				printf("<label>Zit in groep</label><select name='selectie_groep' id='selectie_groep' OnChange='mailingprops();'>%s</selectie>\n", $selgr);
				
				//$this->add_del_selectie("aantal");
				echo("<label>Aantal personen in groep</label><p id='aantalpersoneningroep'></p>\n");
				echo("<button type='submit' name='LedenToevoegen' id='LedenToevoegen'>Groepsleden toevoegen</button>\n");
				echo("<button type='submit' name='LedenVerwijderen' id='LedenVerwijderen'>Groepsleden verwijderen</button>\n");
				echo("<button type='submit' name='OntvangersVerwijderen' id='OntvangersVerwijderen'>Alle ontvangers verwijderen</button>\n");
				echo("</div> <!-- Einde mailingselectieleden -->\n");
				echo("<div class='clear'></div>\n");
			}
		}
		
		printf("<label>Cc</label><input type='text' id='cc_addr' value='%s' %s>\n", $this->cc_addr, $jstb);
		
		if ((new cls_Onderdeel())->aantal("`Type`='A' AND LENGTH(CentraalEmail) > 4 AND IFNULL(VervallenPer, CURDATE()) >= CURDATE()") > 0) {
			printf("<label>Cc aan afdelingen</label><input type='checkbox' name='CCafdelingen' id='CCafdelingen' value=1 %s %s>\n", checked($this->CCafdelingen), $jscb);
		}

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
			printf("<label>Opties</label><input type='checkbox' name='template' id='template' value=1%s %s><p>Template</p>", checked($this->template), $jscb);
			printf("<input type='checkbox' name='HTMLdirect' value=1%s onChange='this.form.submit();'><p>HTML direct (zonder editor)</p>", checked($this->htmldirect));
			printf("<input type='checkbox' name='ZonderBriefpapier' value=1%s  onChange='this.form.submit();'><p>Zonder briefpapier versturen</p>", checked($this->zonderbriefpapier));

			echo("<div class='clear'></div>\n");
			if ($this->htmldirect == 1) {
				printf("<textarea name='message' rows=15 cols=125 %s>%s</textarea>\n", $ob, $this->message);
			} else {
				echo("</div> <!-- Einde editmailingform -->\n");
				if ($_SESSION['settings']['mailing_type_editor'] == 3) {
				
					printf("<textarea id='message' name='message'>%s</textarea>\n", $this->message);
				
				} elseif ($_SESSION['settings']['mailing_type_editor'] == 2) {
					printf("<textarea name='message' %s>%s</textarea>
					<script>
					CKEDITOR.replace( 'message' );
					</script>\n", $this->message, $ob);
			
				} else {
					printf("<textarea name='message' id='editor' %s>%s</textarea>\n", $ob, $this->message);
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

			echo("<div class='clear'></div>\n");
			echo("<label>Beschikbare variabelen</label><div class='invoerblok'>\n<ul class='lijstvariabelen'>\n");
			foreach ($this->MergeField as $v) {
				echo("<li>[%" . $v['Naam'] . "%]</li>\n");
			}
			echo("</ul></div>\n");

			echo("<label>Bijlagen</label>\n<div class='invoerblok'>\n");
			if (strlen($tabbijlagefiles) > 0) {
				echo("<table>\n");
				echo($tabbijlagefiles);
				echo("</table>\n");
			}
			echo("<input type='file' name='UploadFile'>\n");
			echo("<input type='submit' name='UploadBijlage' value='Upload bijlage'>");
			printf("<p>(Extensies '%s' zijn toegestaan)</p>\n", $this->allowed_ext);
			echo("</div>  <!-- Einde invoerblok -->\n");	
			
			echo("<label>Media</label>\n<div class='invoerblok'>\n");
			if (strlen($tabmediafiles) > 0) {
				echo("<table>\n");
				echo($tabmediafiles);
				echo("</table>\n");
			}

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
			printf("<input type='submit' name='action' value='Verstuur mailing' title='Verstuur mailing'>\n");
		}
		if ($this->mid > 0 and $this->aant_rcpt > 0 and strlen($this->message) > 4) {
			$lnk = sprintf("%s?tp=%s&op=savepreview&mid=%d", $_SERVER['PHP_SELF'], $currenttab, $this->mid);
			echo("<input type='submit' name='action' value='Bekijk voorbeeld'>\n");
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
		
		$i_el = null;
		
		printf("
		<script>
			$(document).ready(function() {
				mailingprops();
			});
			
			function mailingprops() {
				$.ajax({
					url: 'ajax_update.php?entiteit=mailingprops',
					type: 'post',
					dataType: 'json',
					data: { mailingid: %1\$d, selectie_vangebdatum: $('#selectie_vangebdatum').val(), selectie_temgebdatum: $('#selectie_temgebdatum').val(), selectie_groep: $('#selectie_groep').val() },
					success: function(response) {
						retvals = response.split(',');
						document.getElementById('aantalpersoneningroep').innerHTML = retvals[0];
						document.getElementById('tekst_vangebdatum').innerHTML = ' (' + retvals[3] + ')';
						document.getElementById('tekst_temgebdatum').innerHTML = ' (' + retvals[4] + ')';
					}
				});
				
				$.ajax({
					url: 'ajax_update.php?entiteit=mailingaantontvangers',
					type: 'post',
					dataType: 'json',
					data: { mailingid: %1\$d },
					success: function(response) {
						if (response > 1) {
							document.getElementById('lblOntvangers').innerHTML = 'Ontvangers (' + response + ')';
						} else {
							document.getElementById('lblOntvangers').innerHTML = 'Ontvangers';
						}
					}
				});
				
				$.ajax({
					url: 'ajax_update.php?entiteit=mailing_html_ontvangers',
					type: 'post',
					dataType: 'json',
					data: { mailingid: %1\$d },
					success: function(response) {
						document.getElementById('lijstontvangers').innerHTML = response;
					}
				});
				options_mogelijke_ontvangers();
			}
			
			function mailing_verw_ontvanger(p_lidid, p_email) {
				$.ajax({
					url: 'ajax_update.php?entiteit=mailing_verw_ontvanger',
					type: 'post',
					dataType: 'json',
					data: { mid: %1\$d, lidid: p_lidid, email: p_email }
				});
				mailingprops();
			}
			
			function mailing_add_ontvanger(p_lidid, p_email) {
				$.ajax({
					url: 'ajax_update.php?entiteit=mailing_add_ontvanger',
					type: 'post',
					dataType: 'json',
					data: { mid: %1\$d, lidid: p_lidid, email: p_email }
				});
				document.getElementById('add_lid').value = 0;
				mailingprops();
			}
			
			function options_mogelijke_ontvangers() {
				if (document.getElementById('cb_alle_personen').checked == true) {
					f = 1;
				} else {
					f = 0;
				}
				
				$.ajax({
					url: 'ajax_update.php?entiteit=options_mogelijke_ontvangers',
					type: 'post',
					dataType: 'json',
					data: { mailingid: %1\$d, alle: f },
					success: function(response) {
						document.getElementById('add_lid').innerHTML = response;
					}
				});
			}
		</script>\n", $this->mid);
	} # edit
	
	public function options_mogelijke_ontvangers($p_mid, $p_alle=0) {
		$i_m = new cls_Mailing();
		
		
		if ($p_mid > 0) {
			$this->mid = $p_mid;
		}
		
		if ($p_alle == 0) {
			$f = 1;
		} else {
			$f = 0;
		}
		
		if ($p_alle == 1) {
			$rv = sprintf("<option value=0>Toevoegen ontvanger ...</option>\n");
		} else {
			$rv = sprintf("<option value=0>Toevoegen lid ...</option>\n");
		}
		$rcpt_rows = $i_m->mogelijkeontvangers($this->mid, $f);
		foreach($rcpt_rows as $rcpt) {
			$rnm = $rcpt->Zoeknaam_lid;
			if ($f == 0) {
				$sl = substr((new cls_Lidmaatschap())->soortlid($rcpt->LidID), 0, 1);
				if ($sl != "L") {
					$rnm .= sprintf(" (%s)", $sl);
				}
			}
			$rv .= sprintf("<option value='%d'>%s</option>\n", $rcpt->LidID, $rnm);
		}
		
		$i_m = null;
		
		
		return $rv;
	}
	
	public function html_ontvangers($p_mid=-1) {
		
		if ($p_mid > 0) {
			$this->mid = $p_mid;
		}
		
		$i_mr = new cls_Mailing_rcpt();
		
		$rcpt_rows = $i_mr->lijst($this->mid);
		$this->meldingen = "Er zijn nog geen ontvangers aan deze mailing toegevoegd.<br>\n";
		
		$this->aant_rcpt = 0;
		$this->aant_ontvangers_zonder_email = 0;
		$rv = "";
		foreach($rcpt_rows as $rcpt) {
			$ze = "";
			if (!isValidMailAddress($rcpt->Email) and !isValidMailAddress($rcpt->EmailVereniging) and !isValidMailAddress($rcpt->EmailOuders) and !isValidMailAddress($rcpt->to_address)) {
				$ze = " *";
				$this->aant_ontvangers_zonder_email++;
			}
			$jsdo = sprintf("OnClick=\"mailing_verw_ontvanger(%d, '%s');\"", $rcpt->LidID, $rcpt->to_address);
			if ($rcpt->LidID > 0) {
//				$lnk = sprintf("%s?tp=Mailing&op=del_rcpt&mid=%d&lidid=%d", $_SERVER['PHP_SELF'], $this->mid, $rcpt->LidID);
				$rv .= sprintf("<li>%s&nbsp;<img src='%s' alt='Verwijderen' title='Verwijderen' %s></li>\n", htmlentities($rcpt->Naam_lid . $ze), BASE64_VERWIJDER_KLEIN, $jsdo);
			} else {
				$lnk = sprintf("%s?tp=Mailing&op=del_rcpt&amp;mid=%d&amp;addr=%s", $_SERVER['PHP_SELF'], $this->mid, $rcpt->to_address);
				$rv .= sprintf("<li>%s&nbsp;<img src='%s' alt='Verwijderen' title='Verwijderen' %s></li>\n", $rcpt->to_address, BASE64_VERWIJDER_KLEIN, $jsdo);
				$this->bevat_losse_email = true;
			}
			$this->aant_rcpt++;
		}

		return $rv;		
	}
	
	private function htmlmedia($p_bestand) {
		
		$url = BASISURL . str_replace(BASEDIR, "", $this->dir_media) . str_replace("!", ".", $p_bestand);
		$ext = fnExtBestand($p_bestand);
		if ($ext == "mp3") {
			$this->message .= "\n<audio controls>" . sprintf("<source src='%s' type='audio/mpeg'>Deze browser ondersteunt geen audio.</audio>", $url);
		} elseif ($ext == "mp4") {
			$this->message .= sprintf("<video width='800' height='600'><source src='%s' type='video/mp4'>Deze browser ondersteunt geen video's.</video> ", $url);
		} else {
			$this->message .= "\n" . sprintf("<img src='%s'>", $url);
		}
	}  # htmlmedia
	
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
			}

			if (strlen($this->subject) == 0) {
				$this->meldingen .= "Het onderwerp moet ingevuld zijn. \n";
				$this->verzendenmag = false;
			}
			
			if (strlen($this->message) == 0) {
				$this->meldingen .= "Het bericht moet ingevuld zijn. \n";
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
		$i_el = new cls_Eigen_lijst();
		$i_lo = new cls_Lidond();
		
		if ($p_actie == "add_selectie") {
			$this->add_del_selectie("add", $_POST['selectie_groep']);
		} elseif ($p_actie == "del_selectie") {
			$this->add_del_selectie("delete");
		}  elseif ($p_actie == "delete_all_rcpt") {
			$i_mr->delete_all($this->mid);
		} 
		
		// ** Bijlagen en media bijwerken.
		$this->upload();
		
		$sw = "del_attach_";
		$swm = "del_media_";
		foreach ($_POST as $key => $val) {
			if (substr($key, -2) == ".x" or substr($key, -2) == "_x") {
				$key = substr($key, 0, -2);
			}
			if ((startwith($key, $sw) or startwith($key, $swm)) and endswith($key, "y") == false) {
				if (startwith($key, $swm)) {
					$fnr = intval(str_replace($swm, "", $key));
					$fn = $_POST[sprintf("name_attach_%d", $fnr)];
					$this->attach_delete($fn, "media");
				} else {
					$fnr = intval(str_replace($sw, "", $key));
					$fn = $_POST[sprintf("name_attach_%d", $fnr)];
					$this->attach_delete($fn);
				}
			}
		}
		
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
		}
	
		if (isset($_POST['message']) and strlen($_POST['message']) > 1) {
			$this->message = html_entity_decode(str_replace("\"", "'", $_POST['message']));
			$_POST['message'] = $this->message;
		}
		
		if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
			if (isset($_POST['CCafdelingen'])) {
				$_POST['CCafdelingen'] = 1;
			} else {
				$_POST['CCafdelingen'] = 0;
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
		}
				
		if ($this->mid > 0) {
			$old = $i_M->record($this->mid);
			foreach($_POST as $key => $val) {
				if (property_exists($old, $key)) {
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
				(new cls_Logboek())->add($mess, 4, 0, 2, $this->mid);
			} else {
				chmod($ad, 0755);
			}

			$ext = explode(".", $target);
			$ext = strtolower($ext[count($ext) - 1]);
			if (strpos($ae, $ext) === false) {
				$mess = sprintf("In mailing %d kan bestand '%s' niet worden ge-upload, omdat de extensie niet is toegestaan. Alleen de volgende extensies zijn toegestaan: %s.", $this->mid, $_FILES['UploadFile']['name'], $ae);
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
			(new cls_Logboek())->add($mess, 4, 0, 1, $this->mid, 41);
		}
	}
	
	public function attach_delete($p_bestand, $p_soort="bijlage") {
		
		$p_bestand = str_replace("!", ".", $p_bestand);
		if ($p_soort == "bijlage") {
			$ad = $this->dir_attachm;
		} else {
			$ad = $this->dir_media;
		}
		if (file_exists($ad . $p_bestand)) {
			if (unlink($ad . $p_bestand)) {
				$mess = sprintf("Bestand '%s' bij mailing %d is verwijderd.", $p_bestand, $this->mid);
			} else {
				$mess = sprintf("Het is niet gelukt om bestand '%s' uit directory '%s' te verwijderen.", $p_bestand, $ad);
			}
		} else {
			$mess = sprintf("Het bestand '%s' is niet aanwezig in directory '%s'.", $p_bestand, $ad);
		}
		(new cls_Logboek())->add($mess, 4, 0, 1, $this->mid, 43);
	}
	
	public function add_del_selectie($actie="aantal", $p_ondid=0) {
		$_POST['selectie_vangebdatum'] = $_POST['selectie_vangebdatum'] ?? "1900-01-01";
		$_POST['selectie_temgebdatum'] = $_POST['selectie_temgebdatum'] ?? date("Y-m-d");
		
		$lidqry = sprintf("SELECT L.RecordID FROM %sLid AS L WHERE L.GEBDATUM >= '%s'", TABLE_PREFIX, $_POST['selectie_vangebdatum']);
		if ($_POST['selectie_temgebdatum'] >= $_POST['selectie_vangebdatum']) {
			$lidqry .= sprintf(" AND L.GEBDATUM <= '%s'", $_POST['selectie_temgebdatum']);
		}
		$lidqry .= " AND (L.Overleden IS NULL) AND (L.Verwijderd IS NULL)";
		
		$ondid = $_POST['selectie_groep'] ?? $p_ondid;
		
		/*
		if (strlen($_POST['selectie_vanliddatum']) == 10 and $_POST['selectie_vanliddatum'] > $i_lm->max("LIDDATUM") and $_POST['selectie_vanliddatum'] <= $i_lm->max("LIDDATUM")) {
			$lidqry .= sprintf(" AND LO.Vanaf >= '%s'", $_POST['selectie_vangebdatum']);
		}
		if (strlen($_POST['selectie_temliddatum']) == 10 and $_POST['selectie_temliddatum'] >= $i_lm->min("LIDDATUM") and $_POST['selectie_temliddatum'] < $i_lm->max("LIDDATUM")) {
			$lidqry .= sprintf(" AND LO.Vanaf <= '%s'", $_POST['selectie_temgebdatum']);
		}
		*/
		
		$i_el = new cls_Eigen_lijst();
		$i_base = new cls_db_base();
		if ($ondid > 0) {
			$elid = $i_el->controle($ondid);
			if ($elid > 0) {
				$subqry = $i_el->mysql($elid, 1);
			} else {
				$subqry = sprintf("(SELECT DISTINCT LO.Lid AS LidID FROM %sLidond AS LO WHERE %s AND LO.OnderdeelID=%d)", TABLE_PREFIX, cls_db_base::$wherelidond, $ondid);
			}
		} else {
			$subqry = sprintf("(SELECT LM.Lid AS LidID FROM %sLidmaatschap AS LM WHERE IFNULL(LM.Opgezegd, CURDATE()) >= CURDATE())", TABLE_PREFIX);
		}
		
		$lidqry .= sprintf(" AND L.RecordID IN %s;", $subqry);
		
		$i_mr = new cls_Mailing_rcpt();
		
		$selrows = $i_base->execsql($lidqry)->fetchAll();
		
//	debug($lidqry, 1, 0, 1);
//	debug($lidqry . ": ". count($selrows), 1, -1, 1);
		
		$this->sl_aantingroep = 0;
		$this->sl_aanttoevoegen = 0;
		$this->sl_aantverwijderen = 0;
		if ($actie == "aantal") {
			$this->sl_aantingroep = count($selrows);
		} else {
			
			foreach($selrows as $row) {
				$this->sl_aantingroep++;
				$amr = $i_mr->aantalontvangers($this->mid, $row->RecordID);
				if ($amr > 0 and $actie == "delete") {
					$this->sl_aantverwijderen++;
					$i_mr->delete($this->mid, $row->RecordID, "", 1);
				} elseif ($actie == "add") {
					$this->sl_aanttoevoegen++;
					$i_mr->add($this->mid, $row->RecordID, "", 1);
				}
			}
		}
		$mess = "";
		if ($actie == "add" and $this->sl_aanttoevoegen > 0) {
			$mess = sprintf("Er zijn %d ontvangers via een selectie aan mailing %d (%s) toegevoegd.", $this->sl_aanttoevoegen, $this->mid, $this->subject);
			(new cls_Logboek())->add($mess, 4, 0, 0, $this->mid, 11);
		} elseif ($actie == "delete" and $this->sl_aantverwijderen > 0) {
			$mess = sprintf("Er zijn %d ontvangers via een selectie bij mailing %d (%s) verwijderd.", $this->sl_aantverwijderen, $this->mid, $this->subject);
			(new cls_Logboek())->add($mess, 4, 0, 0, $this->mid, 13);
		}
		
		$rv = sprintf("%d,%d,%d",$this->sl_aantingroep, $this->sl_aanttoevoegen, $this->sl_aantverwijderen);
		$rv .= "," . trim(strftime('%e %B %Y', strtotime($_POST['selectie_vangebdatum'])));
		$rv .= "," . trim(strftime('%e %B %Y', strtotime($_POST['selectie_temgebdatum'])));
		
		return $rv;
		
	} # add_del_selectie
	
	public function preview() {
		$this->send_mailing(1);
	}	
	
	public function send($lidid=0) {
		
		$this->controle($lidid);
		if ($this->verzendenmag == false) {
			(new cls_Logboek())->add($this->meldingen, 4, 0, 1, $this->mid);
			return false;
		} else {
			if ($_SESSION['settings']['mailing_sentoutbox_auto'] == 0 and $_SESSION['settings']['mailing_direct_verzenden'] == 0) {
				$mess = "Het automatisch versturen staat uit, neem contact op met een beheeder om dit aan te laten zetten.";
				(new cls_Logboek())->add($mess, 4, 0, 1, $this->mid);
			}
			return $this->send_mailing(0, $lidid);
		}
	}  # send
	
	public function folder() {

		if ($this->deleted_on > '2001-01-01') {
			return "Prullenbak";
		} elseif ($this->template == 1) {
			return "Templates";
		} else {
			return "Verzonden";
		}
	}
	
	private function send_mailing($preview=0, $p_lidid=0) {
		
		if ($p_lidid == 0) {
			$rcpts = (new cls_Mailing_rcpt())->lijst($this->mid);
			$pk = "RecordID";
		} else {
			$f = sprintf("L.RecordID=%d", $p_lidid);
			$rcpts = (new cls_Lid())->ledenlijst(1, 0, "", $f);
			$pk = "lnkNummer";
		}
		
		$aant_rcpts = 0;
		$aant_send = 0;
		foreach($rcpts as $rcpt) {
			$email = new email(0, $this->mid);
			if ($rcpt->$pk > 0) {
				$this->merge($rcpt->$pk);
				$email->Subject = $this->merged_subject;
				$email->toevoegenlid($rcpt->$pk);
			} elseif (strlen($rcpt->to_address) > 5) {
				if ($this->mid != $_SESSION['settings']['mailingbijadreswijziging']) {
					$this->merge(0);
				}
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
			if ($aant_send == 1) {
				$mess = sprintf("Via mailing %d (%s) is een e-mail in de outbox geplaatst.", $this->mid, $this->subject);
			} else {
				$mess = sprintf("Via mailing %d (%s) zijn %d e-mails in de outbox geplaatst.", $this->mid, $this->subject, $aant_send);
			}
			(new cls_Logboek())->add($mess, 4, 0, 1, $this->mid);
			if (strlen($this->speciaal) > 1) {
				sentoutbox(4);
			}
		}

		return $aant_send;
	} # send_mailing
	
	public function merge($lidid=0) {
		
		$this->merged_subject = str_ireplace("[%Naamvereniging%]", $_SESSION['settings']['naamvereniging'], $this->subject);
		$this->merged_subject = str_ireplace("[%Naamwebsite%]", $_SESSION['settings']['naamwebsite'], $this->merged_subject);
		$this->merged_subject = str_ireplace("[%URLwebsite%]", BASISURL, $this->merged_subject);
		
		$this->merged_message = str_ireplace("[%Naamvereniging%]", $_SESSION['settings']['naamvereniging'], $this->message);
		$this->merged_message = str_ireplace("[%Naamwebsite%]", $_SESSION['settings']['naamwebsite'], $this->merged_message);
		$this->merged_message = str_ireplace("[%URLwebsite%]", BASISURL, $this->merged_message);
		
		$fl = (new cls_Login())->max("FouteLogin", sprintf("LidID=%d", $lidid));
		if ($_SESSION['settings']['login_maxinlogpogingen'] > 0 and $fl > $_SESSION['settings']['login_maxinlogpogingen']) {
			$geblokt = sprintf("Er is %d keer geprobeerd met deze login in te loggen. Hierdoor is dit account geblokkeerd. Vraag aan de webmaster om deze weer vrij te geven.", $fl);
		} else {
			$geblokt = "";
		}
		$this->merged_message = str_ireplace("[%Geblokkeerd%]", $geblokt, $this->merged_message);
		$this->merged_message = str_ireplace("[%IPaddress%]", $_SERVER['REMOTE_ADDR'], $this->merged_message);
		$this->merged_message = str_ireplace("[%GELDIGHEIDACTIVATIE%]", intval($_SESSION['settings']['login_geldigheidactivatie']), $this->merged_message);
				
		if ($this->mid == $_SESSION['settings']['mailing_validatielogin'] and $lidid > 0) {
			$nak = (new cls_Login())->nieuweactivitiekey($lidid);
			$urlactivatie = sprintf("%s/index.php?tp=Validatie+login&lidid=%d&key=%s", $_SERVER["HTTP_HOST"], $lidid, $nak);
			$this->merged_message = str_replace("[%URLACTIVATIE%]", $urlactivatie, $this->merged_message);
		
		} elseif ($this->mid == $_SESSION['settings']['mailing_herstellenwachtwoord'] and $lidid > 0) {
			$nk = (new cls_Login())->wachtwoordreset($lidid);
			$urlresetww = sprintf("%s/index.php?tp=Herstel+wachtwoord&lidid=%d&key=%s", $_SERVER["HTTP_HOST"], $lidid, $nk);
			$this->merged_message = str_replace("[%URLRESETWW%]", $urlresetww, $this->merged_message);
			
		} elseif ($this->mid == $_SESSION['settings']['mailing_bevestigingopzegging'] and $this->xtranum > 0) {
			$this->merged_message = str_ireplace("[%Lidnummer%]", $this->xtranum, $this->merged_message);
			$f = sprintf("Lidnr=%d", $this->xtranum);
			$row = (new cls_Lidmaatschap())->basislijst($f)->fetch;
			if (isset($row->Opgezegd)) {
				$this->merged_message = str_ireplace("[%OpgezegdPer%]", stftime("%e %B %Y",strtotime($row->Opgezegd)), $this->merged_message);
			}
			
		} elseif ($this->mid == $_SESSION['settings']['mailing_bewakinginschrijving']) {
			$i_ib = new cls_InsBew();
			if ($this->xtranum == 0 and $lidid > 0) {
				$this->xtranum = $i_ib->laatstevanlid($lidid);
			}
			$rows = $i_ib->overzichtlid(0, $this->xtranum);
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
			
			$i_ib = null;
			
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
					$result = (new cls_db_base())->execsql($query);
					$nv = "";
					foreach($result->fetchAll(PDO::FETCH_NUM) as $val) {
						if (substr($fld['Naam'], -3) == "_UL") {
							$nv .= "<li>";
						} elseif (strlen($nv) > 0) {
							$nv .= ", ";
						}
						$nv .= htmlentities($val[0]);
						if (substr($fld['Naam'], -3) == "_UL") {
							$nv .= "</li>\n";
						}
					}
					if (strlen($nv) == 0 and ($fld['Naam'] == "Afdelingen" or $fld['Naam'] == "Toestemmingen" or $fld['Naam'] == "Toestemmingen_UL")) {
						$nv = "Geen";
					} elseif (substr($fld['Naam'], -3) == "_UL") {
						$nv = "<ul>\n" . $nv . "</ul>";
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
		return $this->merged_message;
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
			$this->NietVersturenVoor = $ml->NietVersturenVoor;
			$this->CCafdelingen = $ml->CCafdelingen;
			$this->template = $ml->template;
			$this->zichtbaarvoor = $ml->ZichtbaarVoor;
			$this->htmldirect = $ml->HTMLdirect;
			$this->zonderbriefpapier = $ml->ZonderBriefpapier;
			$this->Ingevoerd = $ml->Ingevoerd;
			$this->IngevoerdDoor = $ml->IngevoerdDoor;
			$this->Gewijzigd = $ml->Gewijzigd;
			$this->GewijzigdDoor = $ml->GewijzigdDoor;
			$this->SentBy = $ml->SentBy;
			$this->deleted_on = $ml->deleted_on;
			$this->DeletedBy = $ml->DeletedBy;
			$this->evenementid = $ml->EvenementID;
			$f = sprintf("MailingID=%d", $this->mid);
			$this->aant_rcpt = (new cls_Mailing_rcpt())->aantal($f);
		} else {
			$this->Ingevoerd = date("Y-m-d");
		}

	} # vuldbvars
} # class Mailing

function lijstmailings($p_filter="") {
	global $currenttab, $currenttab2;
	
	$i_M = new cls_Mailing();

	if (isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0 and $_GET['op'] == "bekijkmail") {
		$xtra = "<p class='mededeling'><input type='button' value='Terug' onclick='history.go(-1);'></p>\n";
		$email = new email($_GET['MailingHistID']);
		$email->toon();
		$mail = null;
			
	} elseif ($p_filter == "Verzonden mails" or $p_filter == "Verzonden e-mails" or $p_filter == "Outbox") {
		$i_mh = new cls_Mailing_hist();
		$kols[0]['link'] = sprintf("<a href='index.php?tp=%s&op=bekijkmail&amp;MailingHistID=%%d'>%%s</a>", urlencode($_GET['tp']));
		if ($_SESSION['settings']['maxmailsperdag'] > 0 and $i_mh->aantalverzonden(1) >= $_SESSION['settings']['maxmailsperdag']) {
			
		} elseif ($_SESSION['settings']['maxmailsperuur'] > 0 and $i_mh->aantalverzonden(2) >= $_SESSION['settings']['maxmailsperuur']) {
			
		} elseif ($p_filter == "Verzonden mails" or $p_filter == "Verzonden e-mails") {
			
		} elseif ($p_filter == "Outbox") {
			if ($_SESSION['settings']['mailing_direct_verzenden'] == 0) {
				$kols[6]['link'] = sprintf("<a href='index.php?tp=%s&amp;op=sentmail&amp;MailingHistID=%%d'>&nbsp;&nbsp;&nbsp;</a>", urlencode($_GET['tp']));
				$kols[6]['class'] = "sendmail";
			}
			$kols[7]['link'] = sprintf("<a href='index.php?tp=%s&amp;op=deletehist&amp;MailingHistID=%%d'>&nbsp;&nbsp;&nbsp;</a>", urlencode($_GET['tp']));
		}
		if ($p_filter == "Outbox") {
			if ($_SESSION['settings']['mailing_sentoutbox_auto'] == 0 and $_SESSION['settings']['mailing_direct_verzenden'] == 0) {
				$mess = "Het automatisch versturen staat uit, neem contact op met een beheeder om dit aan te laten zetten.";
				(new cls_Logboek())->add($mess, 4, 0, 1);
			}
			
			$rows = $i_mh->outbox(1)->fetchAll();
			if (count($rows) > 0) {
				if (count($rows) > 2) {
					echo("<div id='filter'>\n");
					printf("<p class='aantrecords'>%d e-mails</p>\n", count($rows));
					echo("</div> <!-- Einde filter -->\n");
				}
				echo(fnDisplayTable($rows, $kols, "", 0, "", "lijstoutbox"));
			} else {
				echo("<p class='mededeling'>Er staan geen e-mails te wachten om verzonden te worden.</p>\n");
			}
		} else {
			
			$rows = $i_mh->lijst();
			echo("<div id='filter'>\n");
			echo("<input id='tbTekstFilter' placeholder='Tekstfilter' onKeyUp=\"fnFilter('lijstverzondenmails', 'tbTekstFilter', 2, 3, 4);\">\n");
			if (count($rows) > 2) {
				printf("<p class='aantrecords'>%d Verzonden e-mails</p>\n", count($rows));
			}
			echo("</div> <!-- Einde filter -->\n");
			
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, $kols, "", 0, "", "lijstverzondenmails"));
			} else {
				echo("<p class='mededeling'>Er zijn geen verzonden e-mails.</p>\n");
			}
		}
		$i_mh = null;
			
	} else {
		$kols = null;
		if (toegang("Mailing/Wijzigen mailing")) {
			$kols[0]['link'] = sprintf("<a href='%s?tp=%s/Wijzigen+mailing&mid=%%d' title='Wijzigen mailing'>&nbsp;&nbsp;&nbsp;</a>", $_SERVER['PHP_SELF'], $currenttab);
			$kols[0]['class'] = "muteren";
		}
		$rows = $i_M->lijst($p_filter);
		echo("<div id='filter'>\n");
		echo("<input id='tbTekstFilter' placeholder='Tekstfilter' onKeyUp=\"fnFilter('lijstmailingen', 'tbTekstFilter', 1, 2, 3);\">\n");
		printf("<p class='aantrecords'>%d mailings</p>\n", count($rows));
		echo("</div> <!-- Einde filter -->\n");
			
		$kols[-1]['link'] = sprintf("<a href='%s?tp=Mailing/Historie&amp;op=historie&amp;mid=%%d' title='Verstuurde e-mails'>&nbsp;&nbsp;&nbsp;</a>", $_SERVER['PHP_SELF']);
		$kols[-1]['class'] = "detailregels";
		
		echo(fnDisplayTable($rows, $kols, "", 0, "", "lijstmailingen"));
		if ($p_filter == "Prullenbak" and $_SESSION['settings']['mailing_bewaartijd'] > 0) {
			printf("<p>Mailings die langer dan %d maanden geleden verwijderd zijn, worden automatisch definitief opgeschoond.</p>\n", $_SESSION['settings']['mailing_bewaartijd']);
		}
	}
	$i_m = null;
} # lijstmailings

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
	private $CCafdelingen = 0;
	public $zichtbaarvoor = 0;
	
	public $onderwerp = "";
	public $bericht = "";
	public $zonderbriefpapier = 0;
	public $xtrachar = "";
	public $xtranum = 0;
	
	function __construct($mhid=0, $mid=0) {
		$this->ingevoerd = strftime('%e %B %Y (%H:%M:%S)');
		$this->ingevoerddoor = $_SESSION['naamingelogde'];
		
		if ($mhid > 0) {
			$this->mhid = $mhid;
			$mhrow = (new cls_Mailing_hist())->record($this->mhid);
			if (isset($mhrow->MailingID)) {
				$this->mailingid = $mhrow->MailingID;
				$this->CCafdelingen = (new cls_Mailing())->min("CCafdelingen", sprintf("MailingID=%d", $this->mailingid));
				$this->ingevoerd = strftime('%e %B %Y (%H:%M:%S)', strtotime($mhrow->Ingevoerd));
				$this->ingevoerddoor = (new cls_Lid())->Naam($mhrow->IngevoerdDoor);
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
				if ($this->CCafdelingen == 1 and $this->lidid > 0) {
					foreach((new cls_Lidond())->lijstperlid($this->lidid, "A") as $afdrow) {
						if (strlen($afdrow->CentraalEmail) > 4) {
							if (strlen($this->cc) > 4) {
								$this->cc .= ",";
							}
							$this->cc .= $afdrow->CentraalEmail;
						}
					}
				}
				$this->onderwerp = $mhrow->subject;
				$this->bericht = $mhrow->message;
				$this->zonderbriefpapier = $mhrow->ZonderBriefpapier;
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
			$this->zonderbriefpapier = $mrow->ZonderBriefpapier;
		} else {
			$this->vanafadres = $_SESSION['settings']['emailwebmaster'];
		}
	}  # __construct
	
	public function toon($p_direct=1) {
		
		$txt = "<div id='verstuurdemail'>\n";

		$txt .= sprintf("<label>Klaar gezet op</label><p>%s</p><label class='k2'>door</label><p>%s</p>\n", $this->ingevoerd, $this->ingevoerddoor);
		
		if (strlen($this->verstuurdop) > 0) {
			$txt .= sprintf("<label>Verzonden</label><p>%s</p>\n", $this->verstuurdop);
		} else {
			$txt .= sprintf("<label>Verzonden</label><p>Wacht in outbox</p>\n");
		}
		
		if (strlen($this->vanafnaam) > 0) {
			$txt .= sprintf("<label>Van</label><p>%s</p><label class='k2'>E-mail</label><p>%s</p>\n", $this->vanafnaam, $this->vanafadres);
		} else {
			$txt .= sprintf("<label>Van</label><p>%s</p>\n", $this->vanafadres);
		}
		
		if (strlen($this->omsontvangers) > 0) {
			$txt .= sprintf("<label>Aan</label><p>%s</p>\n", $this->omsontvangers);
		}
		
		if (strlen($this->aannaam) > 0 and $this->aannaam != $this->aanadres) {
			$txt .= sprintf("<label>Ontvanger</label><p>%s</p>", $this->aannaam);
			$txt .= sprintf("<label class='k2'>E-mail</label><p>%s</p>\n", $this->aanadres);
		} else {
			$txt .= sprintf("<label>Aan e-mail</label><p>%s</p>\n", $this->aanadres);
		}
		if (strlen($this->cc) > 0) {
			$txt .= sprintf("<label>CC</label><p>%s</p>\n", $this->cc);
		}
		
		$txt .= sprintf("<label>Onderwerp</label><p>%s</p>\n", $this->onderwerp);
		
		$txt .= "</div>  <!-- Einde verstuurdemail -->\n";
		
		$b = str_replace("<!DOCTYPE html>", "", $this->bericht);
		$b = str_replace("<html>", "", str_replace("</html>", "", $b));
		$b = str_replace("<body>", "", str_replace("</body>", "", $b));
		$b = removetextblock($b, "<head>", "</head>");
		
		$txt .= sprintf("<div class='bericht'>\n%s\n</div> <!-- Einde bericht -->\n", $b);
		
		if ($p_direct == 1) {
			echo($txt);
		}

		return $txt;
		
	} # toon
	
	public function toevoegenlid($p_lidid, $p_tp="aan") {
	
		$row = (new cls_Lid())->record($p_lidid);
		if ($this->lidid == 0) {
			$this->lidid = $p_lidid;
		}
		if (isValidMailAddress($row->Email)) {
			$this->toevoegenadres($row->Email, $p_tp);
		} elseif (strlen($row->EmailOuders) > 5) {
			$this->toevoegenadres($row->EmailOuders, $p_tp);
		} elseif (isValidMailAddress($row->EmailVereniging)) {
			$this->toevoegenadres($row->EmailVereniging, $p_tp);
		}
		
		if ($this->xtrachar != "REK" and $p_tp == "aan") {
			$this->aannaam = $row->NaamLid;
		}
		
		if ($row->GEBDATUM > date("Y-m-d", mktime(0, 0, 0, date("m"), date("d"), date("Y")-18)) and strlen($row->EmailOuders) > 5) {
			$this->toevoegenadres($row->EmailOuders, "cc");
		}
		$this->aantalontvangers++;
	}  # toevoegenlid
	
	public function toevoegenadres($adres, $tp="aan", $p_naam="") {
		
		foreach(explode(",", str_replace(" ", "", str_replace(";", ",", $adres))) as $e) {
			if ($tp == "aan" or $tp == "to") {
				if (isValidMailAddress($e) and strpos($this->aanadres, $e) === false) {
					if (strlen($this->aanadres) > 0) {
						$this->aanadres .= ", ";
					}
					$this->aanadres .= trim($e);
					$this->aannaam = $p_naam;
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
	
	public function to_outbox($p_direct=0) {
		
		$rv = 0;
		if ($this->xtrachar == "REK" and $_SESSION['settings']['mailing_rekening_vanafid'] > 0) {
			$i_mv = new cls_Mailing_vanaf($_SESSION['settings']['mailing_rekening_vanafid']);
			$this->vanafadres = $i_mv->vanaf_email;
			$this->vanafnaam = $i_mv->vanaf_naam;
			$i_mv = null;
		}
		
		if (strlen($this->aanadres) < 5) {
			debug("geen aanadres aanwezig");
		} elseif (strlen($this->vanafadres) > 5) {
			$this->mhid = (new cls_Mailing_hist())->add($this);
			if ($this->mhid > 0) {
				$rv = $this->mhid;
				if ($p_direct == 1) {
					sentoutbox(0, $this->mhid);
				}
			}
		}
		
		return $rv;
		
	}  # to_outbox

}  # class email

function fnRekeningenMailen($op) {
	global $currenttab, $currenttab2;
	
	$i_rk = new cls_Rekening();
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
				$mindat = $i_rk->min("Datum");
			}
			if (strtotime($maxdat) !== FALSE) {
				$maxdat = date("Y-m-d", strtotime($maxdat));
			} else {
				$maxdat = $i_rk->max("Datum");
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
			
			$i_p->update("mailing_rekening_zichtbaarvoor", $_POST['zichtbaarvoor']);

			$selrek = "";
			$aantrek = 0;
			foreach($i_rk->lijst($filter) as $rk) {
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
				
			foreach($i_rk->lijst($_POST['rekfilter']) as $rk) {
				$email = new email(0, 0);
				$email->xtrachar = "REK";
				$email->xtranum = $rk->Nummer;
				$email->toevoegenlid($rk->Lid);
				$email->zichtbaarvoor = $_SESSION['settings']['mailing_rekening_zichtbaarvoor'];
				$tm = 0;

				if ($_SESSION['settings']['mailing_rekening_stuurnaar'] > 1) {
					$hd = new DateTime($rk->Datum);
					$hd->sub(new DateInterval('P18Y'));
					foreach((new cls_Rekeningregel())->perrekening($rk->Nummer) as $regel) {
						if ($regel->GEBDATUM > "1900-01-01" and $regel->GEBDATUM <= $hd->format("Y-m-d")) {
							$email->toevoegenlid($regel->Lid);
						} elseif ($_SESSION['settings']['mailing_rekening_stuurnaar'] == 2 and strlen($regel->EmailOuders) > 5) {
							$email->toevoegenadres($regel->EmailOuders, "cc");
						} elseif ($_SESSION['settings']['mailing_rekening_stuurnaar'] == 3) {
							$email->toevoegenlid($regel->Lid);
						}
					}
				}
				
				$email->bericht = RekeningDetail($rk->Nummer);
				$email->aannaam = $rk->Tenaamstelling;
				$email->onderwerp = $rk->OMSCHRIJV;
				
				if (strlen($email->aanadres) == 0) {
					$mess = sprintf("Rekening %d kan niet worden verzonden, omdat er van %s geen geldig e-mailadres in de database bekend is.", $rk->Nummer, $email->aannaam);
					$tm = 1;
				} else {
					if ($email->to_outbox(0)) {
						$mess = sprintf("Rekening %d van %s (%s) is in de outbox geplaatst.", $rk->Nummer, $email->aannaam, $email->aanadres);
						$aant_ok++;
					} else {
						$mess = sprintf("Rekening %d kon niet in de outbox worden geplaatst.", $rk->Nummer);
						$tm = 1;
					}
				}
				(new cls_Logboek())->add($mess, 4, 0, $tm, $rk->Nummer);
				$email = null;
			}
		} elseif ($op == "selectieversturen") {
			$mess = "De rekeningen zijn niet verzonden, omdat het vinkje niet gezet was.";
			(new cls_Logboek())->add($mess, 4, 0, 1);
		}
	}

	if ($_SERVER['REQUEST_METHOD'] != "POST" or $op == "selectieversturen") {
		$i_p->vulsessie();
		printf("<form method='post' action='%s?tp=%s/%s&amp;op=selectierekeningen' name='mailrek'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
		echo("<h2>Filters</h2>\n");
		echo("<table>\n");
		echo("<tr><th></th><th>Gelijk aan</th><th>Vanaf</th><th>Totenmet</th></tr>");
		printf("<tr><td class='label'>Seizoen</th><td><select name='rekseizoen'>\n%s</select></td><td></td><td></td></tr>\n", (new cls_Seizoen())->htmloptions());
		printf("<tr><td class='label'>Rekeningnummer</td><td><input type='number' name='reknr'></td><td><input type='number' value=%d name='minreknr'></td><td><input type='number' value=%d name='maxreknr'></td></tr>\n", $i_rk->min("Nummer"), $i_rk->max("Nummer"));
		printf("<tr><td class='label'>Rekeningdatum</td><td><input type='date' name='rekdatum'></td><td><input type='date' value='%s' name='mindatum'></td><td><input type='date' value='%s' name='maxdatum'></td></tr>\n", $i_rk->min("Datum"), $i_rk->max("Datum"));			
		printf("<tr><td class='label'>Betaald door (debiteur)</td><td><select name='reklid'>\n<option value=-1>*** Iedereen ***</option>\n%s</select></td><td></td><td></td></tr>\n", (new cls_lid())->htmloptions(-1, 3));
		echo("<tr><td class='label'>Statussen</td><td colspan=3>\n");
		echo("<input type='checkbox' name='volbetaald' value=1 checked><p>Volledig betaald</p>\n");
		echo("<input type='checkbox' name='deelbetaald' value=1 checked><p>Gedeeltelijk betaald</p>\n");
		echo("<input type='checkbox' name='nietbetaald' value=1 checked><p>Niet betaald</p>\n");
		printf("<input type='checkbox' name='nulrekeningen' value=1 %s><p>Nul rekeningen</p>\n", checked($_SESSION['settings']['mailing_rekening_nulrekversturen']));
		echo("<input type='checkbox' name='eerderverzonden' value=1 checked><p>Eerder verzonden</p>\n");
		echo("</table>\n");
		
		echo("<h2>Instellingen</h2>\n");
		echo("<label>Rekening versturen aan</label><select name='rekverstuuraan'>\n");
		$sn[1] = "Alleen gekoppeld lid";
		$sn[2] = "Gekoppeld lid en alle volwassenen op de rekening";
		$sn[3] = "Alle leden op de rekening";
		foreach($sn as $key => $val) {
			printf("<option value=%d%s>%s</option>>\n", $key, checked($key, "option", $_SESSION['settings']['mailing_rekening_stuurnaar']), $val);
		}
		echo("</select>\n");
		
		$i_mv = new cls_Mailing_vanaf();
		printf("<label>Vanaf e-mailadres</label><select name='vanafid'>%s</select>\n", $i_mv->htmloptions($_SESSION['settings']['mailing_rekening_vanafid']));
		$i_mv = null;
		
		printf("<label>Alleen zichtbaar voor</label><select name='zichtbaarvoor'>\n<option value=-1>Alleen webmasters</option>\n%s</select>\n", (new cls_Onderdeel())->htmloptions($_SESSION['settings']['mailing_rekening_zichtbaarvoor'], 1));
		
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
		if (strlen($_POST['mailing_bewaartijd']) == 0 or $_POST['mailing_bewaartijd'] < 1) {
			$_POST['mailing_bewaartijd'] = 999;
		}
		if (strlen($_POST['maxmailsperminuut']) == 0 or $_POST['maxmailsperminuut'] < 1) {
			$_POST['maxmailsperminuut'] = 9999;
		}
		if (strlen($_POST['maxmailsperuur']) == 0 or $_POST['maxmailsperuur'] < 1) {
			$_POST['maxmailsperuur'] = 9999;
		}
		if (strlen($_POST['maxmailsperdag']) == 0 or $_POST['maxmailsperdag'] < 1) {
			$_POST['maxmailsperdag'] = 9999;
		}
		
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
		
		$i_p->update("mailing_wachttijdinoutbox", $_POST['mailing_wachttijdinoutbox']);
		$i_p->update("mailing_alle_zien", $_POST['mailing_alle_zien']);
		$i_p->update("mailing_mailopnieuw", $_POST['mailing_mailopnieuw']);
		$i_p->update("mailing_bewaartijd", intval("0" . $_POST['mailing_bewaartijd']));
		$i_p->update("mailing_hist_opschonen", intval("0" . $_POST['mailing_hist_opschonen']));
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

	printf("<form method='post' action='%s?tp=%s/%s'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
	
	$options = sprintf("<option value=0%s>Nee</option>\n", checked($_SESSION['settings']['mailing_mailopnieuw'], "option", 0));
	$options .= sprintf("<option value=1%s>Alleen door webmasters</option>\n", checked($_SESSION['settings']['mailing_mailopnieuw'], "option", 1));
	$options .= sprintf("<option value=2%s>Ja</option>\n", checked($_SESSION['settings']['mailing_mailopnieuw'], "option", 2));
	
	echo("<h2>Algemeen</h2>\n");
	printf("<label>E-mails in de outbox direct probereren te versturen?</label><input type='checkbox' name='mailing_direct_verzenden' value='1'%s>\n", checked($_SESSION['settings']['mailing_direct_verzenden']));
	printf("<label>E-mails uit de outbox automatisch in de achtergrond versturen?</label><input type='checkbox' name='mailing_sentoutbox_auto' value='1'%s>\n", checked($_SESSION['settings']['mailing_sentoutbox_auto']));
	printf("<label>Hoeveel minuten moeten mails minimaal wachten totdat ze automatisch worden verstuurd?</label><input type='number' name='mailing_wachttijdinoutbox' value=%d>\n", $_SESSION['settings']['mailing_wachttijdinoutbox']);
	
	printf("<label>Mag een verzonden e-mail opnieuw worden verstuurd?</label><select name='mailing_mailopnieuw'>%s</select></p>\n", $options);
	
	$options = sprintf("<option value=-1%s>Webmasters</option>\n", checked($_SESSION['settings']['mailing_alle_zien'], "option", -1));
	
	foreach ((new cls_Onderdeel())->lijst(1) as $row) {
		$options .= sprintf("<option value=%d%s>%s</option>\n", $row->RecordID, checked($_SESSION['settings']['mailing_alle_zien'], "option", $row->RecordID), $row->Naam);
	}
	printf("<label>Wie mogen alle mailings zien en muteren?</label>\n<select name='mailing_alle_zien'>%s</select>\n", $options);

	printf("<label>Hoeveel mails mogen er per minuut verstuurd worden</label><input type='number' name='maxmailsperminuut' value=%d min=1 max=9999>\n", $_SESSION['settings']['maxmailsperminuut']);
	printf("<label>Hoeveel mails mogen er per uur verstuurd worden</label><input type='number' name='maxmailsperuur' value=%d min=1 max=9999>\n", $_SESSION['settings']['maxmailsperuur']);
	printf("<label>Hoeveel mails mogen er per 24 uur verstuurd worden</label><input type='number' name='maxmailsperdag' value=%d min=1 max=9999>\n", $_SESSION['settings']['maxmailsperdag']);
	$i_mh = new cls_Mailing_hist();
	printf("<label>Aantal verstuurde mails in de afgelopen uur / 24 uur:</label><p>%d / %d</p>\n", $i_mh->aantalverzonden(2), $i_mh->aantalverzonden(3));
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
	
	echo("<h2>Opschonen</h2>\n");
	printf("<label>Hoelang, in maanden, moeten mailings in de prullenbak bewaard blijven?</label><input type='number' name='mailing_bewaartijd' value=%d min=1 max=999>\n", $_SESSION['settings']['mailing_bewaartijd']);
	printf("<label>Hoelang, in maanden, moeten verzonden mails van voormalig leden bewaard blijven?</label><input type='number' name='mailing_hist_opschonen' value=%d min=3 max=999>\n", $_SESSION['settings']['mailing_hist_opschonen']);
	
	echo("<h2>Mailings met een specifiek doel</h2>\n");
	
	$rows = $i_m->lijst("Templates");

	$options = "";
	foreach($rows as $row) {
		$options .= sprintf("<option%s value=%d>%s</option>", checked($row->lnkMailingID, "option", $_SESSION['settings']['mailing_lidnr']), $row->lnkMailingID, $row->Onderwerp);
	}
	printf("<label>Versturen lidnummer</label><select name='mailing_lidnr'>\n<Option value=0>Geen</option>\n%s</select>\n", $options);
	
	$options = "";
	foreach($rows as $row) {
		$options .= sprintf("<Option%s value=%d>%s</option>", checked($row->lnkMailingID, "option", $_SESSION['settings']['mailing_validatielogin']), $row->lnkMailingID, $row->Onderwerp);
	}
	printf("<label>Versturen validatie e-mail login</label><select name='mailing_validatielogin'><Option value=0>Geen</option>\n%s</select>\n", $options);
	
	$options = "";
	foreach($rows as $row) {
		$options .= sprintf("<Option%s value=%d>%s</option>", checked($row->lnkMailingID, "option", $_SESSION['settings']['mailing_herstellenwachtwoord']), $row->lnkMailingID, $row->Onderwerp);
	}
	printf("<label>Versturen link herstellen wachtwoord</label><select name='mailing_herstellenwachtwoord'><Option value=0>Geen</option>\n%s</select>\n", $options);

	$options = "<Option value=0>Geen</option>";
	foreach($rows as $row) {
		$options .= sprintf("<Option%s value=%d>%s</option>", checked($row->lnkMailingID, "option", $_SESSION['settings']['mailing_bevestigingopzegging']), $row->lnkMailingID, $row->Onderwerp);
	}
	$rows = null;
	printf("<label>Versturen bevestiging opzegging lidmaatschap</label><select name='mailing_bevestigingopzegging'>%s</select>\n", $options);
	
	echo("<div class='clear'></div>\n");
	
	$kols[-1]['link'] = "<button type='submit' name='delete_vanaf_%d' title='Verwijder vanaf adres %d' onclick='this.submit();'>&nbsp;&nbsp;&nbsp;</button>";
	$kols[-1]['class'] = "trash";
	$xr = "<td colspan=6><button type='submit' name='vanaf_adres_toevoegen' title='Vanaf adres toevoegen' value='Vanaf adres toevoegen'><img src='./images/star.png'></button></td>";
	
	$rows = (new cls_Mailing_vanaf())->lijst(); 
	echo(fnDisplayTable($rows, $kols, "Vanaf adressen", 0, "", "editmailingvanaf", "", "Vanaf_naam, emlVanaf_email"));
	
	echo("<div id='opdrachtknoppen'>\n");
	echo("<button type='submit' name='vanaf_adres_toevoegen' value='Vanaf adres toevoegen'>Vanaf adres toevoegen</button>");
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
				$mess = sprintf("E-mail %d (%s) is naar %s verstuurd.", $p_mhid, $mail->Subject, $row->to_addr);
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

function sentoutbox($p_mode, $p_mhid=-1) {
	
	/*
	Uitleg p_mode:
		1 = versturen middels knop door gebruiker
		2 = via batchjob
		3 = direct versturen
		4 = direct versturen in verband met speciaal soort
	*/
	
	$i_mh = new cls_Mailing_hist();
	
	$aantverzonden = 0;
	$mess = "";
	if ($p_mode == 2 and $_SESSION['settings']['mailing_sentoutbox_auto'] == 0) {
		$mess = sprintf("Het versturen in de achtergrond (via batchjob) is uitgeschakeld. Er staan %d e-mails in de outbox.", $i_mh->aantaloutbox());
		
	} elseif ($p_mode == 3 and $_SESSION['settings']['mailing_direct_verzenden'] == 0) {
		$mess = sprintf("Het direct versturen is uitgeschakeld. Er staan %d e-mails in de outbox.", $i_mh->aantaloutbox());
		
	} elseif ($i_mh->aantalverzonden(3) >= $_SESSION['settings']['maxmailsperdag']) {
		$mess = "Het versturen is niet gestart, omdat het maximale aantal ter versturen e-mails per 24 uur al is bereikt.";
		
	} elseif ($i_mh->aantalverzonden(2) >= $_SESSION['settings']['maxmailsperuur']) {
		$mess = "Het versturen is niet gestart, omdat het maximale aantal ter versturen e-mails per uur al is bereikt.";
		
	} elseif ($i_mh->aantalverzonden(1) >= $_SESSION['settings']['maxmailsperminuut']) {
		$mess = "Het versturen is niet gestart, omdat het maximale aantal ter versturen e-mails per minuut al is bereikt.";
		
	} else {
		foreach($i_mh->outbox($p_mode)->fetchAll() as $mhrow) {
			if ($mhrow->ndNietVersturenVoor < date("Y-m-d h:i:s") and $i_mh->aantalverzonden(1) < $_SESSION['settings']['maxmailsperminuut']) {
				if (sentfromhist($mhrow->lnkRecordID)) {
					$aantverzonden++;
				}
			}
		}
	
		$aob = $i_mh->aantaloutbox();
		if ($aantverzonden > 0 or $aob > 0) {
			if ($p_mode == 2) {
				$mess = sprintf("sentoutbox (mode: %d) is gestart door de batchjob", $p_mode);
			} else {
				$mess = sprintf("Versturen vanuit de outbox (mode: %d) is gereed", $p_mode);
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
	public $zonderbriefpapier = 0;

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
	
	public function addstationary($to="", $from="", $verwijderaanhef=0, $p_zb=-1) {
		
		if ($p_zb >= 0) {
			$this->zonderbriefpapier = $p_zb;
		}
			
		if ($this->zonderbriefpapier == 1) {
			if (startwith($this->Body, "<!DOCTYPE html>") == false) {
				$this->Body = "<!DOCTYPE html>\n" . $this->Body;
			}
			return true;
			
		} elseif (file_exists($this->bestand_briefpapier)) {
//			debug($this->bestand_briefpapier, 1, 0, 1);
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
			(new cls_Logboek())->add($mess, 4, 0, 1);
			return false;
		}
	}
	
	public function Send() {
		
		$i_lb = new cls_Logboek();
		$i_mh = new cls_Mailing_hist();

		if ($i_mh->aantalverzonden(1) >= $_SESSION['settings']['maxmailsperminuut']) {
			$mess = sprintf("De limiet van aantal van %d te versturen mails per minuut is bereikt.", $_SESSION['settings']['maxmailsperminuut']);
			$i_lb->add($mess, 4);
			return false;
			
		} elseif ($i_mh->aantalverzonden(3) >= $_SESSION['settings']['maxmailsperdag']) {
			$mess = sprintf("De limiet van aantal van %d te versturen mails per 24 uur is bereikt.", $_SESSION['settings']['maxmailsperdag']);
			$i_lb->add($mess, 4);
			return false;
		
		} elseif ($i_mh->aantalverzonden(2) >= $_SESSION['settings']['maxmailsperuur']) {
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
