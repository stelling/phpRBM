<?php

function fnMailing() {
	global $ldl, $currenttab, $currenttab2;

	$_GET['mid'] = $_GET['mid'] ?? 0;
	$_GET['mhid'] = $_GET['mhid'] ?? 0;
	$op = $_GET['op'] ?? "";

	if ($_SESSION['settings']['mailing_direct_verzenden'] == 1) {
		sentoutbox(3);
	} elseif (isset($_POST['outboxlegen']) and WEBMASTER) {
		(new cls_mailing_hist())->outboxlegen();
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
	}

	if (($op == "historie" or $op == "histdetails") and $mailing->mid > 0) {
		fnDispMenu(2, sprintf("mid=%d", $mailing->mid));
		
	} elseif ($currenttab2 != "previewwindow" and $op != "preview_hist") {
		fnDispMenu(2);
	}

//	echo("<h2>Werk in uitvoering. Graag niet gebruiken.</h2>");

	$f = sprintf("RecordID=%d", $_GET['mid']);
	$zv = (new cls_Mailing())->max("ZichtbaarVoor", $f);
	$lg = explode(",", $_SESSION['lidgroepen']);
	if (WEBMASTER == false and $zv != 0 and in_array($zv, $lg) === false and in_array($_SESSION['settings']['mailing_alle_zien'], $lg) === false) {
		$mess = "Je hebt geen rechten om deze mailing te bekijken of te bewerken.";
		printf("<p class='waarschuwing'>%s</p>", $mess);
		
	} elseif ($currenttab2 == "Nieuw" and toegang($_GET['tp'], 1, 1)) {
		$mailing->mid = 0;
		$mailing->edit();
		
	} elseif ($op == "send") {
		$mailing->post_form();
		$mailing->send();
		$mailing->edit();
		
	} elseif ($currenttab2 == "Groepen muteren") {
		if (isset($_GET['OnderdeelID']) and $_GET['OnderdeelID'] > 0) {
			LedenOnderdeelMuteren($_GET['OnderdeelID'], 1);
		} else {
			persoonlijkeGroepMuteren();
		}
		
	} elseif ($currenttab2 == "Instellingen" and toegang($_GET['tp'], 1, 1)) {
		fnMailingInstellingen();
		
	} elseif ($op == "opnieuw" and isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0) {
		sentfromhist($_GET['MailingHistID']);
		echo("<script>window.history.back();</script>\n");
		
	} elseif ($op == "sentmail" and isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0) {
		sentfromhist($_GET['MailingHistID'], 1);
		lijstmailings($currenttab2);
				
	} elseif ($op == "deletehist" and isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0) {
		$i_mh = new cls_Mailing_hist();
		$i_mh->delete($_GET['MailingHistID']);
		$i_mh = null;
		echo("<script>window.history.back();</script>\n");
		
	} elseif ($currenttab2 == "Rekeningen" and toegang($_GET['tp'], 1, 1)) {
		fnRekeningenMailen($op);
		
	} elseif ($currenttab2 == "Logboek" and toegang($_GET['tp'], 1, 1)) {
		$lijst = (new cls_Logboek())->lijst(4, 1, 0, "", "", 1500);
		$kols = fnStandaardKols("logboek", 1, $lijst);
		
		echo(fnDisplayTable($lijst, $kols));
		
	} elseif ($op == "historie" and $_GET['mid'] > 0) {
		$rows = (new cls_Mailing_hist())->lijst($_GET['mid']);
		if (count($rows) > 0) {
			$l = sprintf("index.php?tp=%s&op=histdetails&mhid=%%d", $_GET['tp']);
			$kols[] = array('columnname' => "RecordID", 'link' => $l, 'class' => "details");
			$kols[] = array('headertext' => "Verzonden", 'columnname' => "send_on", 'type' => "datetime");
			$kols[] = array('headertext' => "Aan", 'columnname' => "Aan", 'type' => "combitext");
			if (WEBMASTER) {
				$kols[] = array('columnname' => "RecordID", 'headertext' => "&nbsp;", 'type' => "click", 'class' => "mislukt", 'onclick' => "mailtooutbox(%d)", 'title' => "Terugzetten in de outbox.");
			}
			
			$th = sprintf("%d. %s", $mailing->mid, $rows[0]->subject);
			echo(fnDisplayTable($rows, $kols, $th, 1));
		} else {
			echo("<p class='mededeling'>Deze mailing heeft geen historie.</p>\n");
		}
		
	} elseif ($op == "histdetails" and $_GET['mhid'] > 0) {
		$email = new email($_GET['mhid']);
		$email->toon();
		$mail = null;
		
	} elseif ($op == "edit_email" and $_GET['mhid'] > 0) {
		$email = new email($_GET['mhid']);
		$email->edit();
		$mail = null;

	} elseif ($op == "preview_hist" and $_GET['mhid'] > 0) {
		
		echo($mailing->preview_hist($_GET['mhid']));

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
		
	if ($currenttab2 != "previewwindow" and $op != "preview_hist") {
		HTMLfooter();
	}
}  # fnMailing

class Mailing {
	public $mid;
	
	private string $allowed_ext = "bmp, gif, jpeg, jpg, pdf, png, pps, rar, txt, zip";
	private $allowed_ext_afb = "bmp, gif, jpeg, jpg, mp3, mp4, png";
	private $dir_attachm = "";
	private $max_size_attachm = 2097152; // 2MB
	private $vertraging_tussen_verzenden = 30; // de minimale tijd (in seconden) die er tussen het verzenden van dezelfde mailing moet zitten.
	private $smtpserver = 0; // De details van de server moeten in config.php gespecificeerd worden.
	
	private $MergeField;
	private $contains_mergefield = false;
	private $bevat_losse_email = false;
	private $merged_subject = "";
	private $merged_message = "";
	private $speciaal = "";
	private $opmerking = "";
	
	public $mailingvanafid = 0;
	private $OmschrijvingOntvangers = "";
	public $cc_addr = "";
	private $CCafdelingen = 0;
	private $subject = "";
	private $message = "";
	private $NietVersturenVoor = "0000-00-00";
	private $zichtbaarvoor = 0;
	private $template = 0;
	private $htmldirect = 0;
	public $zonderbriefpapier = 0;
	public $gewijzigd = "";
	public $ingevoerd = "";
	private $deleted_on = "0000-00-00";
	private $evenementid = 0;
	
	private $ok_send = "";
	private $nok_send = "";
	private $meldingen = "";
	private $aant_ok = 0;
	private $aant_nok = 0;
	public $aant_rcpt = 0;
	private $verzendenmag = false;
	private $automontvanger = false;
	
	private $sl_huidigegroep = 0;
	private $sl_aantingroep = 0;
	private $sl_aanttoevoegen = 0;
	private $sl_aantverwijderen = 0;

	public $xtrachar = "";
	public $xtranum = 0;

	function __construct($p_mid=0) {
		
		$i_base = new cls_db_base();
		
		$this->vuldbvars($p_mid);
		
		if ($this->mid > 0) {
			$this->dir_attachm = $_SESSION['settings']['path_attachments'] . $this->mid . "/";
		}
		
		if (strlen($_SESSION['settings']['mailing_extensies_toegestaan']) > 0) {
			$this->allowed_ext = $_SESSION['settings']['mailing_extensies_toegestaan'];
		}
		$this->max_size_attachm = $_SESSION['settings']['max_grootte_bijlage'] * 1024;

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

		$sql = sprintf("SELECT UPPER(L.Postcode) FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Postcode", 'SQL' => $sql);

		$sql = sprintf("SELECT UPPER(L.Woonplaats) FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Woonplaats_HL", 'SQL' => $sql);
		
		$sql = sprintf("SELECT L.Woonplaats FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Woonplaats", 'SQL' => $sql);
							 
		$sql = sprintf("SELECT %s AS Lidnaam FROM %sLid AS L WHERE L.RecordID=%%d;", (new cls_db_base())->selectnaam, TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Lidnaam", 'SQL' => $sql);

		$sql = sprintf("SELECT IFNULL(`Login`, 'Geen') FROM %sAdmin_login WHERE LidID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Login", 'SQL' => $sql);

		$sql = sprintf("SELECT IFNULL(LM.Lidnr, 0) FROM %sLidmaatschap AS LM WHERE IFNULL(LM.Opgezegd, '9999-12-31') >= CURDATE() AND LM.Lid=%%d;", TABLE_PREFIX);
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
		
		$query = sprintf("SELECT MAX(L.Bankrekening) FROM %sLid AS L;", TABLE_PREFIX);
		if (strlen($i_base->scalar($query)) >= 15) {
			$sql = sprintf("SELECT L.Bankrekening FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
			$this->MergeField[]=array('Naam' => "Bankrekening", 'SQL' => $sql);
		}
		
		$query = sprintf("SELECT MAX(L.Burgerservicenummer) FROM %sLid AS L;", TABLE_PREFIX);
		if (strlen($i_base->scalar($query)) >= 9) {
			$sql = sprintf("SELECT L.Burgerservicenummer FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
			$this->MergeField[]=array('Naam' => "BSN", 'SQL' => $sql);
		}

		if ((new cls_Rekening())->aantal() > 0 and toegang('Ledenlijst/Overzicht lid/Rekeningen', 0, 0)) {
			$sql = sprintf("SELECT RK.Nummer FROM %sRekening AS RK WHERE RK.Bedrag > 0 AND RK.Bedrag > RK.Betaald AND %s;", TABLE_PREFIX, "RK.Lid=%d");
			$this->MergeField[]=array('Naam' => "OpenstaandeRekeningen", 'SQL' => $sql);
			
			$this->MergeField[]=array('Naam' => "OpenstaandeRekeningenTabel", 'SQL' => "");

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
		
		$sql = sprintf("SELECT IFNULL(DP.Naam, 'Geen') FROM %1\$sLiddipl AS LD INNER JOIN %1\$sDiploma AS DP ON LD.DiplomaID=DP.RecordID WHERE LD.Lid=%%d AND LD.DatumBehaald > '2000-01-01' AND LD.DatumBehaald <= CURDATE() ORDER BY LD.DatumBehaald DESC LIMIT 1;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "DiplomaLaatste", 'SQL' => $sql);
		
		if ((new cls_Diploma())->aantal("Zelfservice=1") > 0) {
			$sql = sprintf("SELECT DISTINCT DP.Naam FROM %1\$sLiddipl AS LD INNER JOIN %1\$sDiploma AS DP ON LD.DiplomaID = DP.RecordID
					WHERE IFNULL(LD.LicentieVervallenPer, '9999-12-31') >= CURDATE() AND LD.Lid=%%d AND DP.Zelfservice=1 ORDER BY DP.Volgnr, LD.DatumBehaald, DP.Naam;", TABLE_PREFIX);
			$this->MergeField[]=array('Naam' => "DiplomaZelfservice", 'SQL' => $sql);
		}
	
		$sql = sprintf("SELECT L.RelnrRedNed FROM %sLid AS L WHERE %s;", TABLE_PREFIX, "L.RecordID=%d");
		$this->MergeField[]=array('Naam' => "RelnrRedNed", 'SQL' => $sql);

		$sql = sprintf("SELECT DATE_FORMAT(LIDDATUM, %s, 'nl_NL') FROM %sLidmaatschap AS LM WHERE IFNULL(LM.Opgezegd, '9999-12-31') >= CURDATE() AND LM.Lid=%%d;", $i_base->fdlang, TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "LidVanaf", 'SQL' => $sql);

		$sql = sprintf("SELECT IF(LM.OPGEZEGD IS NULL, '', DATE_FORMAT(DATE_ADD(LM.Opgezegd, INTERVAL 1 DAY), %s, 'nl_NL')) FROM %sLidmaatschap AS LM
							 WHERE LM.LIDDATUM < CURDATE() AND LM.Lid=%%d
							 ORDER BY LM.LIDDATUM DESC LIMIT 1;", $i_base->fdlang, TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "OpgezegdPer", 'SQL' => $sql);

		$sql = sprintf("SELECT IFNULL(L.Email, '') FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "Email", 'SQL' => $sql);
		
		$sql = sprintf("SELECT IF(LENGTH(IFNULL(L.Email, '')) < 6, IFNULL(L.EmailOuders, ''), L.Email) FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "EmailLidOfOuders", 'SQL' => $sql);
		
		$sql = sprintf("SELECT IFNULL(L.EmailOuders, '') FROM %sLid AS L WHERE L.RecordID=%%d;", TABLE_PREFIX);
		$this->MergeField[]=array('Naam' => "EmailOuders", 'SQL' => $sql);
		
		$i_ond = new cls_Onderdeel();

		if (count($i_ond->lijst(1, "O.`Type`='A'")) > 0) {
			$sql = sprintf("SELECT CONCAT(O.Naam, IF(LO.Functie > 0, CONCAT(' (', F.Omschrijv , ')'), ''), IF(LO.Opgezegd >= DATE_SUB(CURDATE(), INTERVAL 6 MONTH), CONCAT(' (eindigt per ', DATE_FORMAT(DATE_ADD(LO.Opgezegd, INTERVAL 1 DAY), %s, 'nl_NL'), ')'), ''))
								 FROM %s
								 WHERE LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, '9999-12-31') >= DATE_SUB(CURDATE(), INTERVAL 6 MONTH) AND O.`Type`='A' AND LO.Lid=%%d ORDER BY LO.Vanaf;", $i_base->fdlang, $i_ond->fromlidond);
			$this->MergeField[]=array('Naam' => "Afdelingen", 'SQL' => $sql);
			
			$sql = sprintf("SELECT CONCAT(O.Naam, IF(LO.Functie > 0, CONCAT(' (', F.Omschrijv , ')'), ''), IF(LO.Opgezegd >= CURDATE(), CONCAT(' (eindigt per ', DATE_FORMAT(LO.Opgezegd, %s, 'nl_NL'), ')'), ''))
					FROM %s
					WHERE IFNULL(LO.Opgezegd, '9999-12-31') >= DATE_SUB(CURDATE(), INTERVAL 6 MONTH) AND O.`Type`='A' AND LO.Lid=%%d ORDER BY LO.Vanaf;", $i_base->fdlang, $i_ond->fromlidond);
//			$this->MergeField[]=array('Naam' => "AfdelingenOpgezegd", 'SQL' => $sql);
			
			if ((new cls_Groep())->aantal() > 1) {
						
				$sql = sprintf("SELECT CONCAT(O.Naam, IF(LO.GroepID > 0, CONCAT(' (', GR.Starttijd, ' ', GR.Omschrijving, ')'), IF(LO.Functie > 0, CONCAT(' (', F.Omschrijv , ')'), '')))
						FROM %s
						WHERE IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE() AND O.`Type`='A' AND LO.Lid=%%d ORDER BY O.Naam;", $i_ond->fromlidond);
				$this->MergeField[]=array('Naam' => "AfdelingenMetGroep", 'SQL' => $sql);
				
				if ((new cls_Activiteit())->aantal() > 1) {

					$sql = sprintf("SELECT CONCAT(O.Naam, IF(LO.GroepID > 0, IF(GR.ActiviteitID>0, CONCAT(' (', Act.Omschrijving, ')'), ''), IF(LO.Functie > 0, CONCAT(' (', F.Omschrijv , ')'), '')), IF(LO.Opgezegd >= DATE_SUB(CURDATE(), INTERVAL 6 MONTH), CONCAT(' (eindigt per ', DATE_FORMAT(DATE_ADD(LO.Opgezegd, INTERVAL 1 DAY), %s, 'nl_NL'), ')'), ''))
										 FROM %s
										 WHERE IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE() AND O.`Type`='A' AND LO.Lid=%%d ORDER BY O.Naam;", $i_base->fdlang, $i_ond->fromlidond);
					$this->MergeField[]=array('Naam' => "AfdelingenMetActiviteit", 'SQL' => $sql);
					
					$sql = sprintf("SELECT Act.Omschrijving
							FROM %s
							WHERE IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE() AND O.`Type`='A' AND LO.GroepID > 0 AND GR.ActiviteitID > 0 AND LO.Lid=%%d ORDER BY Act.Omschrijving;", $i_ond->fromlidond);
					$this->MergeField[]=array('Naam' => "Activiteiten", 'SQL' => $sql);
				}
			}
		}
		
		$sql = sprintf("SELECT CONCAT(IF(LENGTH(F.OMSCHRIJV) > 0, F.OMSCHRIJV, 'Lid'), IF(O.`TYPE` IN ('A', 'C', 'G'), CONCAT(' ', O.Naam), '')) FROM (%1\$sLidond AS LO INNER JOIN %1\$sFunctie AS F ON LO.Functie=F.Nummer) INNER JOIN %2\$s AS O ON O.RecordID=LO.OnderdeelID 
						WHERE %3\$s AND (O.Kader=1 OR F.Kader=1) AND LO.Lid=%%d ORDER BY LO.Vanaf, O.Naam;", TABLE_PREFIX, $i_ond->table, cls_db_base::$wherelidond);
		$this->MergeField[]=array('Naam' => "Kaderact.", 'SQL' => $sql);
		
		if (count($i_ond->lijst(1, "O.`Type`='R'")) > 0) {
			$sql = sprintf("SELECT O.Naam FROM %sLidond AS LO INNER JOIN %s AS O ON O.RecordID = LO.OnderdeelID 
					 WHERE %s AND `Type`='R' AND LO.Lid=%%d ORDER BY O.Naam;", TABLE_PREFIX, $i_ond->table, cls_db_base::$wherelidond);
			$this->MergeField[]=array('Naam' => "Rollen", 'SQL' => $sql);
		}
		
		if (count($i_ond->lijst(1, "O.`Type`='T'")) > 0) {
			$sql = sprintf("SELECT CONCAT(O.Naam, ': ', IF((SELECT COUNT(*) FROM %1\$sLidond AS LO WHERE LO.OnderdeelID=O.RecordID AND LO.Lid=%%d AND IFNULL(LO.Opgezegd, '9999-2-31') >= CURDATE()) > 0, 'Ja', 'Nee')) FROM %1\$sOnderdl AS O WHERE O.`Type`='T' ORDER BY O.Naam;", TABLE_PREFIX);
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
		
		$i_mr = new cls_Mailing_rcpt();
		$f = sprintf("MR.MailingID=%d AND Xtra_Char='AfdGr' AND Xtra_Num > 0", $this->mid);
		if ($i_mr->aantal($f) > 0) {
			$sql = sprintf("SELECT (SELECT IFNULL(GR.Omschrijving, '') FROM %1\$sGroep AS GR WHERE GR.RecordID=Xtra_Num) FROM %1\$sMailing_rcpt AS MR WHERE MR.LidID=%%d AND MR.MailingID=%2\$d;", TABLE_PREFIX, $this->mid);
			$this->MergeField[]=array('Naam' => "AfdelingsgroepOmschrijving", 'SQL' => $sql);
			
			$sql = sprintf("SELECT (SELECT IFNULL(GR.Starttijd, '') FROM %1\$sGroep AS GR WHERE GR.RecordID=Xtra_Num) FROM %1\$sMailing_rcpt AS MR WHERE MR.LidID=%%d AND MR.MailingID=%2\$d;", TABLE_PREFIX, $this->mid);
			$this->MergeField[]=array('Naam' => "AfdelingsgroepStarttijd", 'SQL' => $sql);

			$sql = sprintf("SELECT (SELECT CONCAT(IFNULL(GR.Starttijd, ''), ' - ', GR.Eindtijd) FROM %1\$sGroep AS GR WHERE GR.RecordID=Xtra_Num) FROM %1\$sMailing_rcpt AS MR WHERE MR.LidID=%%d AND MR.MailingID=%2\$d;", TABLE_PREFIX, $this->mid);
			$this->MergeField[]=array('Naam' => "AfdelingsgroepTijden", 'SQL' => $sql);
			
			$sql = sprintf("SELECT (SELECT IFNULL(GR.Instructeurs, '') FROM %1\$sGroep AS GR WHERE GR.RecordID=Xtra_Num) FROM %1\$sMailing_rcpt AS MR WHERE MR.LidID=%%d AND MR.MailingID=%2\$d;", TABLE_PREFIX, $this->mid);
			$this->MergeField[]=array('Naam' => "AfdelingsgroepInstructeurs", 'SQL' => $sql);
			
			$sql = sprintf("SELECT (SELECT IFNULL(DP.Naam, '') FROM %1\$sGroep AS GR INNER JOIN %1\$sDiploma AS DP ON DP.RecordID=GR.DiplomaID WHERE GR.RecordID=Xtra_Num) FROM %1\$sMailing_rcpt AS MR WHERE MR.LidID=%%d AND MR.MailingID=%2\$d;", TABLE_PREFIX, $this->mid);
			$this->MergeField[]=array('Naam' => "AfdelingsgroepDiploma", 'SQL' => $sql);
			
		}
		$i_mr = null;
		
		$this->MergeField[]=array('Naam' => "Naamvereniging", 'SQL' => "");
		$this->MergeField[]=array('Naam' => "Naamwebsite", 'SQL' => "");
		$this->MergeField[]=array('Naam' => "URLwebsite", 'SQL' => "");
		$this->MergeField[]=array('Naam' => "FROMNAME", 'SQL' => "");
		
		// Velden die bij specifieke mailings horen.
		if ($this->mid == $_SESSION['settings']['mailing_lidnr'] and $this->mid > 0) {
			$this->xtrachar = "LNR";
			$this->speciaal = "Versturen lidnummer";
			$this->automontvanger = true;
			$this->vertraging_tussen_verzenden = 2;
			$this->MergeField[]=array('Naam' => "Geblokkeerd", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "IPaddress", 'SQL' => "");
			
		} elseif ($this->mid == $_SESSION['settings']['mailing_validatielogin'] and $this->mid > 0) {
			$this->xtrachar = "VALLOGIN";
			$this->speciaal = "Validatie login";
			$this->automontvanger = true;
			$this->vertraging_tussen_verzenden = 5;
			$this->MergeField[]=array('Naam' => "Geblokkeerd", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "IPaddress", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "GELDIGHEIDACTIVATIE", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "URLACTIVATIE", 'SQL' => "");
			
		} elseif ($this->mid == $_SESSION['settings']['mailing_herstellenwachtwoord'] and $this->mid > 0) {
			$this->xtrachar = "HERWW";
			$this->speciaal = "Herstellen wachtwoord";
			$this->automontvanger = true;
			$this->vertraging_tussen_verzenden = 5;
			$this->MergeField[]=array('Naam' => "Geblokkeerd", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "IPaddress", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "GELDIGHEIDACTIVATIE", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "URLRESETWW", 'SQL' => "");
			
		} elseif ($this->mid == $_SESSION['settings']['mailing_bevestigingdeelnameevenement'] and $this->mid > 0) {
			$this->xtrachar = "EVD";
			$this->speciaal = "Bevestiging evenement";
			$this->automontvanger = true;
			$this->vertraging_tussen_verzenden = 5;
			$this->MergeField[]=array('Naam' => "OmsEvenement", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "DatumEvenement", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "LocatieEvenement", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "StatusEvenement", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "AantalPersonenEvenement", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "OpmEvenement", 'SQL' => "");
			
		} elseif ($this->mid == $_SESSION['settings']['mailing_bevestigingbestelling'] and $this->mid > 0) {
			$this->xtrachar = "BEST";
			$this->speciaal = "Bevestiging bestelling webshop";
			$this->automontvanger = true;
			$this->vertraging_tussen_verzenden = 5;
			$this->MergeField[]=array('Naam' => "Ordernummer", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "Bestelling", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "Totaalbedrag", 'SQL' => "");
			$this->MergeField[]=array('Naam' => "VoorwaardenBestelling", 'SQL' => "");
			
		} elseif ($this->mid == $_SESSION['settings']['mailing_bevestigingopzegging'] and $this->mid > 0) {
			$this->xtrachar = "BO_LM";
			$this->speciaal = "Bevestiging opzegging lidmaatschap";
			$this->automontvanger = true;

		} elseif ($this->mid == $_SESSION['settings']['mailingbijadreswijziging'] and $this->mid > 0) {
			$this->xtrachar = "LIDAW";
			$this->speciaal = "Melding wijziging postcode";
		}

		sort($this->MergeField);

		foreach ($this->MergeField as $fld) {
				$nm = "[%" . $fld['Naam'] . "%]";
			if (strpos($this->message, $nm) !== false) {
				$this->contains_mergefield = true;
			}
		}

	}  # Mailing->__construct

	private function vuldbvars($p_mid=-1) {
		if ($p_mid > 0) {
			$this->mid = $p_mid;
		}
		
		if ($this->mid > 0) {
			$ml = (new cls_Mailing())->record($this->mid);
			
			if (isset($ml->RecordID)) {
				$this->mailingvanafid = $ml->MailingVanafID;
				$this->OmschrijvingOntvangers = trim($ml->OmschrijvingOntvangers);
				$this->cc_addr = trim($ml->cc_addr);
				$this->subject = trim($ml->subject);
				$this->Opmerking = trim($ml->Opmerking);
				$this->message = $ml->message;
				$this->NietVersturenVoor = $ml->NietVersturenVoor;
				$this->CCafdelingen = $ml->CCafdelingen;
				$this->template = $ml->template;
				$this->zichtbaarvoor = $ml->ZichtbaarVoor;
				$this->htmldirect = $ml->HTMLdirect;
				$this->zonderbriefpapier = $ml->ZonderBriefpapier;
				$this->ingevoerd = $ml->Ingevoerd;
				$this->gewijzigd = $ml->Gewijzigd;
				$this->deleted_on = $ml->deleted_on;
				$this->evenementid = $ml->EvenementID;
				$this->aant_rcpt = (new cls_Mailing_rcpt())->aantalontvangers($this->mid);
			} else {
				$this->mid = 0;
			}
		} else {
			$this->Ingevoerd = date("Y-m-d");
		}

	}  # Mailing->vuldbvars
	
	public function edit() {
		global $currenttab, $currenttab2, $navpad, $dtfmt;
		
		$i_m = new cls_Mailing();
		$i_mr = new cls_Mailing_rcpt();
		$i_el = new cls_Eigen_lijst();
		$i_ond = new cls_Onderdeel();
		$ob = "OnBlur='this.form.submit();'";
		if ($this->mid > 0) {
			$jstb = sprintf("OnBlur=\"savedata('mailing', %d, this);\"", $this->mid);
			$jscb = sprintf("OnClick=\"savecb('mailing', %d, this);\"", $this->mid);
			$jsoc = sprintf("OnChange=\"savedata('mailing', %d, this);\"", $this->mid);
		} else {
			$jstb = "";
			$jscb = "";
			$jsoc = "";
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
					$tabbijlagefiles .= sprintf("<td><button type='submit' name='del_attach_%d' alt='Verwijderen' title='Verwijder %s'><i class='bi bi-trash'></i></button></td>\n", $bnr, $entry);
					$tabbijlagefiles .= sprintf("<input type='hidden' name='name_attach_%d' value='%s'>\n", $bnr, str_replace(".", "!", $entry));
					$bnr++;
				}
			}
			$d->close();
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
		printf("<div id='editmailingform'>\n");
		printf("<form method='post' id='editmailingform' class='form-check form-switch' action='%s?tp=Mailing/Wijzigen+mailing%s' enctype='multipart/form-data'>\n", $_SERVER['PHP_SELF'], $m);
		
		if ($this->mid > 0) {
			printf("<p id='recordid'>%d</p><label id='lblrecordid'>RecordID</label>\n", $this->mid);
		}
		printf("<input type='hidden' name='mailingid' value=%d>\n", $this->mid);
		printf("<label class='form-label'>Van</label><select name='MailingVanafID' id='MailingVanafID' class='form-select form-select-sm' %s>\n<option value=''>Selecteer ...</option>%s</select>\n", $jsoc, (new cls_Mailing_vanaf())->htmloptions($this->mailingvanafid));
		printf("<label class='form-label'>Aan</label><input type='text' name='OmschrijvingOntvangers' id='OmschrijvingOntvangers' class='w50' value=\"%s\" maxlength=50 placeholder='Omschrijving groep personen aan wie de mailing is gericht' %s>\n", $this->OmschrijvingOntvangers, $jstb);
		printf("<label class='form-label'>Onderwerp</label><input type='text' name='subject' id='subject' class='w75' value=\"%s\" maxlength=75 placeholder='Onderwerp' %s>\n", $this->subject, $jstb);

		if ($this->mid > 0) {
			printf("<label class='form-label'>Opmerking (intern)</label><input type='text' id='Opmerking' class='w75' value=\"%s\" maxlength=75 placeholder='Extra verduidelijking' %s>\n", $this->Opmerking, $jstb);
			if (strlen($this->speciaal) > 0) {
				printf("<label class='form-label'>Specifiek doel</label><p>%s</p>\n", $this->speciaal);
			}
			if ($this->automontvanger == true) {
				$i_mr->delete_all($this->mid);
			} else {
				echo("<label id='lblOntvangers' class='form-label'>Ontvangers</label>\n");
				echo("<div id='lijstontvangers'>\n");
				echo("</div> <!-- Einde lijstontvangers -->\n");
				$this->verzendenmag = true;
				$this->meldingen = "";

				echo("<label class='form-label'>Ontvanger toevoegen</label>\n");
				printf("<select id='add_lid' class='form-select form-select-sm' onChange=\"mailing_add_ontvanger(%d, $(this).val(), '');\">%s</select>\n", $this->mid, $this->options_mogelijke_ontvangers());
			
				$_POST['selecteer_groep'] = $_POST['selecteer_groep'] ?? 0;
				
				printf("<input type='email' maxlength=50 placeholder='Toevoegen e-mailadres' onBlur=\"mailing_add_ontvanger(%d, 0, $(this).val());\">", $this->mid);
				printf("<button type='button' id='OntvangersVerwijderen' class='%s btn-sm' OnClick='mailing_verw_alle_ontvangers(%d);'>%s Ontvangers</button>\n", CLASSBUTTON, $this->mid, ICONVERWIJDER);

				echo("<label>Selectie leden</label>\n");
				echo("<div id='mailingselectieleden'>\n");

				$i_lid = new cls_Lid();
				$i_lm = new cls_Lidmaatschap();
				
				$_POST['selectie_vangebdatum'] = $_POST['selectie_vangebdatum'] ?? $i_lid->min("GEBDATUM");
				$_POST['selectie_temgebdatum'] = $_POST['selectie_temgebdatum'] ?? $i_lid->max("GEBDATUM");
				
				printf("<label class='form-label'>Vanaf geboortedatum</label><input type='date' value='%s' name='selectie_vangebdatum' id='selectie_vangebdatum' OnBlur='mailingprops(%d);'><p id='tekst_vangebdatum'></p>\n", $_POST['selectie_vangebdatum'], $this->mid);
				printf("<label class='form-label'>T/m geboortedatum</label><input type='date' value='%s' name='selectie_temgebdatum' id='selectie_temgebdatum' OnBlur='mailingprops(%d);'><p id='tekst_temgebdatum'></p>\n", $_POST['selectie_temgebdatum'], $this->mid);

				$this->sl_huidigegroep = $_POST['selectie_groep'] ?? 0;
				$selgr = sprintf("<option value=0>&nbsp;</option>\n%s<option disabled>-- Eigen lijsten --</option>\n%s</select>\n", $i_ond->htmloptions($this->sl_huidigegroep, 1), $i_el->htmloptions($this->sl_huidigegroep, 2));
				printf("<label class='form-label'>Zit in groep</label><select name='selectie_groep' id='selectie_groep' class='form-select form-select-sm' OnChange='mailingprops(%d);'>%s</selectie>\n", $this->mid, $selgr);
				
				echo("<label class='form-label'>Aantal personen in groep</label><p id='aantalpersoneningroep'></p>\n");
				printf("<button type='button' id='LedenToevoegen' class='%s btn-sm' OnClick='mailing_add_selectie_ontvangers();'>%s Groepsleden</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
//				printf("<button type='submit' name='LedenToevoegen' class='%s btn-sm' OnClick='mailing_add_selectie_ontvangers();'>%s Groepsleden</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
				printf("<button type='button' class='%s btn-sm' id='GroepsledenVerwijderen' OnClick='mailing_verw_selectie_ontvangers();'>%s Groepsleden</button>\n", CLASSBUTTON, ICONVERWIJDER);
				echo("</div> <!-- Einde mailingselectieleden -->\n");
//				echo("<div class='clear'></div>\n");
			}
			printf("<label class='form-label'>Cc</label><input type='text' id='cc_addr' class='w50' maxlength=50 value='%s' %s>\n", $this->cc_addr, $jstb);

			if ((new cls_Onderdeel())->aantal("`Type`='A' AND LENGTH(CentraalEmail) > 4 AND IFNULL(VervallenPer, CURDATE()) >= CURDATE()") > 0) {
				printf("<label class='form-label' for='CCafdelingen' id='ccafdelingen'>Cc aan afdelingen</label><input type='checkbox' id='CCafdelingen' class='form-check-input' value=1 %s %s>\n", checked($this->CCafdelingen), $jscb);
			}

			$ondrows = (new cls_Onderdeel())->lijst(1, "`Type`<>'T'", "", $_SESSION['lidid']);
			if ($this->zichtbaarvoor == 0) {
				$select = "<option value=0 selected>Iedereen</option>\n";
			} else {
				$select = "<option value=0>Iedereen</option>\n";
			}
			foreach($ondrows as $ondrow) {
				if ($this->zichtbaarvoor == $ondrow->RecordID) {
					$select .= sprintf("<option value=%d selected>%s</option>\n", $ondrow->RecordID, $ondrow->Naam);
				} else {
					$select .= sprintf("<option value=%d>%s</option>\n", $ondrow->RecordID, $ondrow->Naam);
				}
			}
			printf("<label class='form-label'>Zichtbaar voor</label><select id='ZichtbaarVoor' class='form-select form-select-sm' %s>\n%s</select>\n", $jsoc, $select);
			printf("<label for='template' class='form-check'>Template</label><input type='checkbox' id='template' class='form-check-input' value=1%s %s>", checked($this->template), $jscb);
			printf("<label for='HTMLdirect' id='htmldirect' class='form-check'>HTML direct</label><input type='checkbox' id='HTMLdirect' name='HTMLdirect' class='form-check-input' title='Zonder editor'value=1%s onChange='this.form.submit();'>", checked($this->htmldirect));
			printf("<label for='ZonderBriefpapier' id='zonderbriefpapier' class='form-check'>Zonder briefpapier</label><input type='checkbox' id='ZonderBriefpapier' class='form-check-input' value=1%s %s>", checked($this->zonderbriefpapier), $jscb);

			echo("<div class='clear'></div>\n");
			
			foreach($this->MergeField as $fld) {
				$nm = "[%" . $fld['Naam'] . "%]";
				if (stripos($this->message, $nm) !== false) {
					$this->contains_mergefield = true;
					$this->message = str_ireplace($nm, $nm, $this->message);
				}
			}
			
			if ($this->htmldirect == 1) {
				printf("<textarea id='message' onBlur=\"savedata('mailing', %d, this);\">%s</textarea>\n", $this->mid, $this->message);
			} else {
				printf("<textarea id='message' name='message'>%s</textarea>\n", $this->message);
			}

			echo("<div class='clear'></div>\n");
			echo("<label id='lblbeschikbarevariabelen' class='form-label' OnClick=\"togglevariabelen();\">Beschikbare variabelen <span>+</span></label>");
			echo("<ul id='lijstvariabelen'>\n");
			foreach ($this->MergeField as $v) {
				echo("<li>[%" . $v['Naam'] . "%]</li>\n");
			}
			echo("</ul>");

			echo("<label id='lblbijlagen' class='form-label'>Bijlagen</label>\n<div id='bijlagen'>\n");
			if (strlen($tabbijlagefiles) > 0) {
				echo("<table class='table table-bordered'>\n");
				echo($tabbijlagefiles);
				echo("</table>\n");
			}
			echo("<input type='file' name='UploadFile'>\n");
			echo("<input type='submit' name='UploadBijlage' value='Upload bijlage'>");
			printf("<p>(Extensies '%s' zijn toegestaan)</p>\n", $this->allowed_ext);
			echo("</div>  <!-- Einde bijlagen -->\n");
		}
		
		$this->controle();
		if ($this->mid > 0) {
			$i_lid = new cls_Lid();
			$dtfmt->setPattern(DTLONG);
			$lm = laatstemutatie("Mailing", $this->mid, 1, " / ");
			if (strlen($lm) > 10) {
				printf("<label class='form-label'>Ingevoerd door / op</label><p>%s</p>\n", $lm);
			} else {
				printf("<label class='form-label'>Ingevoerd op</label><p>%s</p>\n", $dtfmt->format(strtotime($this->ingevoerd)));
			}
			printf("<label class='form-label'>Laatst gewijzigd door / op</label><p>%s</p>\n", laatstemutatie("Mailing", $this->mid, 2, " / "));
			
			if ($this->deleted_on > '1901-01-01') {
				printf("<label class='form-label'>Verwijderd op</label><p>%s</p>\n", $dtfmt->format(strtotime($this->deleted_on)));
			}

			echo("<label id='lblmeldingen' class='form-label'>Meldingen</label><ul id='meldingen'></ul>\n");
			$cls_lid = null;
		}
		
		echo("<div id='opdrachtknoppen'>\n");
		if ($this->mid > 0) {
			printf("<button type='submit' class='%s' name='action' value='Bewaren'>%s Bewaren</button>\n", CLASSBUTTON, ICONBEWAAR);
			printf("<button type='submit' class='%s' name='action' value='Bewaren & sluiten'>%s Bewaren & sluiten</button>\n", CLASSBUTTON, ICONSLUIT);
		} else {
			printf("<button type='submit' class='%s' name='Toevoegen'>%s Toevoegen</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
		}

		printf("<button type='submit' class='%s' name='action' value='Verstuur mailing' title='Verstuur mailing' id='btnverstuurmailing'>%s Verstuur mailing</button>\n", CLASSBUTTON, ICONVERSTUUR);
		
		$lnk = sprintf("%s?tp=%s&op=savepreview&mid=%d", $_SERVER['PHP_SELF'], $currenttab, $this->mid);
		printf("<button type='submit' class='%s' name='action' value='Bekijk voorbeeld' id='btnbekijkvoorbeeld'>%s Bekijk voorbeeld</button>\n", CLASSBUTTON, ICONVOORBEELD);

		if ($this->mid > 0 and $this->deleted_on > "1901-01-01" and WEBMASTER) {
			printf("<button type='submit' name='action' value='Verwijderen ongedaan maken' title='Verwijderen ongedaan maken'>Verwijderen ongedaan maken</button>\n");
		} elseif ($this->mid > 0 and $this->deleted_on < "1901-01-01" and (WEBMASTER or $_SESSION['lidid'] == $this->IngevoerdDoor)) {
			printf("<button type='submit' class='%s' name='action' value='Verwijderen'>%s Verwijderen</button>\n", CLASSBUTTON, ICONVERWIJDER);
		}
		echo("</div>  <!-- Einde opdrachtknoppen -->\n");
		
		printf("</form>\n");
		echo("</div> <!-- Einde editmailingform -->\n");
		
		$i_el = null;
		
		echo("<script>
			$(document).ready(function() {
				mailingprops();
			});
		</script>\n");
		if ($this->htmldirect == 0) {
			js_editor(0);
		}
	}  # Mailing->edit
	
	public function options_mogelijke_ontvangers($p_mid=-1) {
		$i_m = new cls_Mailing();
		
		if ($p_mid > 0) {
			$this->mid = $p_mid;
		}
		
		$rv = sprintf("<option value=0>Toevoegen lid ...</option>\n");
		$rcpt_rows = $i_m->mogelijkeontvangers($this->mid, 1);
		foreach($rcpt_rows as $rcpt) {
			$rv .= sprintf("<option value='%d'>%s</option>\n", $rcpt->LidID, $rcpt->Zoeknaam_lid);
		}
		$rv .= sprintf("<option value=0 disabled>Toevoegen kloslid ...</option>\n");
		$rcpt_rows = $i_m->mogelijkeontvangers($this->mid, 2);
		foreach($rcpt_rows as $rcpt) {
			$rv .= sprintf("<option value='%d'>%s</option>\n", $rcpt->LidID, $rcpt->Zoeknaam_lid);
		}
		
		$rv .= sprintf("<option value=0 disabled>Toevoegen voormalig lid ...</option>\n");
		$rcpt_rows = $i_m->mogelijkeontvangers($this->mid, 3);
		foreach($rcpt_rows as $rcpt) {
			$rv .= sprintf("<option value='%d'>%s</option>\n", $rcpt->LidID, $rcpt->Zoeknaam_lid);
		}
		
		$i_m = null;
		
		return $rv;
	}  # Mailing->options_mogelijke_ontvangers
	
	public function html_ontvangers($p_mid=-1, $p_asarray=false) {
		
		$this->mid = $p_mid;
		
		$i_mr = new cls_Mailing_rcpt();
		
		$rcpt_rows = $i_mr->lijst($this->mid);

		if ($p_asarray) {
			$rv = array();
		} else {
			$rv = "<ul>\n";
		}
		foreach($rcpt_rows as $rcpt) {
			$jsdo = sprintf("OnClick=\"mailing_verw_ontvanger(%d, '%s');\"", $rcpt->RecordID, $rcpt->to_address);
			if ($rcpt->LidID > 0) {
				$item = sprintf("%s&nbsp;<i class='bi bi-trash' %s alt='Verwijderen' title='Verwijderen'></i>", htmlentities($rcpt->NaamLid), $jsdo);
			} else {
				$item = sprintf("%s&nbsp;<i class='bi bi-trash' %s alt='Verwijderen' title='Verwijderen'></i>", $rcpt->to_address, $jsdo);
				$this->bevat_losse_email = true;
			}
			if ($p_asarray) {
				$rv[] = $item;
			} else {
				$rv .= sprintf("<li id='ontvanger_%d'>%s</li>\n", $rcpt->RecordID, $item);
			}
		}
		
		if (!$p_asarray) {
			$rv .= "</ul>\n";
		}
		$this->aant_rcpt = count($rcpt_rows);

		return $rv;		
	}  # Mailing->html_ontvangers
	
	public function controle($enkellid=0) {
	
		if ($this->mid > 0) {
			
			if ($this->deleted_on > "2000-01-01") {
				$this->meldingen = "<li>Deze mailing staat op verwijderen en mag daarom niet worden verzonden.</li>\n";
				$this->verzendenmag = false;
			} else {
				$this->meldingen = "";
				$this->verzendenmag = true;
			}

			if ($this->mailingvanafid == 0) {
				$this->meldingen .= "<li>De Van moet ingevuld zijn.</li>\n";
				$this->verzendenmag = false;
			}
			
			if ($this->aant_rcpt == 0 and $enkellid == 0) {
				$this->meldingen .= "<li>Er zijn geen ontvangers aan deze mailing toegevoegd.</li>\n";
				$this->verzendenmag = false;
			}

			if (strlen($this->subject) == 0) {
				$this->meldingen .= "<li>Het onderwerp moet ingevuld zijn.</li>\n";
				$this->verzendenmag = false;	
			}
			
			if (strlen($this->message) == 0) {
				$this->meldingen .= "<li>Het bericht moet ingevuld zijn.</li>\n";
				$this->verzendenmag = false;
			}
			
			foreach($this->MergeField as $fld) {
				$nm = "[%" . $fld['Naam'] . "%]";
				if (stripos($this->message, $nm) !== false) {
					$this->contains_mergefield = true;
					$this->message = str_ireplace($fld['Naam'], $fld['Naam'], $this->message);
				}
			}
			
			$hv = 0;
			while ($hv < strlen($this->message)) {
				$bp = strpos($this->message, "[%", $hv);
				if ($bp !== false) {
					$bp += 2;
					$ep = strpos($this->message, "%]", $bp);
					if ($ep !== false) {
						$mm = substr($this->message, $bp, ($ep-$bp));
						$lp = strpos($mm, "#");
						if ($lp !== false) {
							$mm = substr($mm, 0, $lp);
						}
						$as = array_search($mm, array_column($this->MergeField, 'Naam'));
						if ($as === false) {
							$this->meldingen .= sprintf("<li>%s is geen beschikbare variable.</li>\n", $mm);
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
				$this->meldingen .= "<li>Er staan losse e-mailadressen en er wordt met mailmerge gewerkt. Bij de losse e-mailadressen werkt de mailmerge niet.</li>\n";
			}
			if ($this->deleted_on > '1901-01-01') {
				$this->meldingen .= "<li>Deze mailing is verwijderd.</li>";
				$this->verzendenmag = false;
			}
		} else {
			$this->verzendenmag = false;
			$this->meldingen = "<li>Er is geen mailing geselecteerd.</li>\n";
		}
		
		if ($this->verzendenmag) {
			$this->meldingen .= "<li>Deze mailing kan verzonden worden.</li>";
		} else {
			$this->meldingen .= "<li>Deze mailing mag niet verzonden worden.</li>\n";
		}
		
		$rv['verzendenmag'] = $this->verzendenmag;
		$rv['meldingen'] = $this->meldingen;
		
		return $rv;
		
	}  # Mailing->controle
	
	public function post_form($p_actie="save") {
		
		$i_M = new cls_Mailing();
		$i_mr = new cls_Mailing_rcpt();
		$i_el = new cls_Eigen_lijst();
		$i_lo = new cls_Lidond();
		
		if (isset($_POST['mailingid']) and $_POST['mailingid'] > 0) {
			$this->mid = $_POST['mailingid'];
		} elseif (isset($_GET['mailingid']) and $_GET['mailingid'] > 0) {
			$this->mid = $_GET['mailingid'];
		}
		
		if ($this->mid == 0) {
			$this->mid = $i_M->add($_POST['subject']);
			if ($this->mid > 0) {
				$i_M->update($this->mid, "MailingVanafID", $_POST['MailingVanafID']);
				$i_M->update($this->mid, "OmschrijvingOntvangers", $_POST['OmschrijvingOntvangers']);
			}
		} else {
			
			if (isset($_POST['message'])) {
				$i_M->update($this->mid, "message", $_POST['message']);
			}
			
			// ** Bijlagen bijwerken.
			$this->upload();
		
			$sw = "del_attach_";
			foreach ($_POST as $key => $val) {
				if (substr($key, -2) == ".x" or substr($key, -2) == "_x") {
					$key = substr($key, 0, -2);
				}
				if (startwith($key, $sw) and endswith($key, "y") == false) {
					$fnr = intval(str_replace($sw, "", $key));
					$fn = $_POST[sprintf("name_attach_%d", $fnr)];
					$this->attach_delete($fn);
				}
			}
		
			if (isset($_POST['HTMLdirect'])) {
				$_POST['HTMLdirect'] = 1;
			} else {
				$_POST['HTMLdirect'] = 0;
			}
			$i_M->update($this->mid, "HTMLdirect", $_POST['HTMLdirect']);
		}
		
		$this->vuldbvars();
		
		$i_M = null;
		$i_mr = null;
		
	}  # Mailing->post_form
	
	public function upload() {
		
		if (isset($_FILES['UploadFile']['name']) and strlen($_FILES['UploadFile']['name']) > 3 and $this->mid > 0) {
			$ad = $this->dir_attachm;
			$ae = $this->allowed_ext;
			$target = $ad . $_FILES['UploadFile']['name'];
			
			if (!file_exists($ad)) {
				if (mkdir($ad, 0755, true) == true) {
					$mess = sprintf("Folder '%s' is aangemaakt.", $ad);
				} else {
					$mess = sprintf("Folder '%s' bestaat niet en kan niet aangemaakt worden. Probeer het later opnieuw of neem contact op met de webmaster.", $ad);
					$ad = "";
				}
				(new cls_Logboek())->add($mess, 4, 0, 2, $this->mid, 44);
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
				if (move_uploaded_file($_FILES['UploadFile']['tmp_name'], $target) == false) {
					$mess = sprintf("Fout %d is opgetreden bij het uploaden van bestand '%s'. Probeer het later opnieuw of neem contact op met de webmaster.", $_FILES['UploadFile']['error'], $_FILES['UploadFile']['name']);
				} else {
					$mess = sprintf("In mailing %d (%s) is bestand '%s' bijgesloten.", $this->mid, $this->subject, $_FILES['UploadFile']['name']);
				}
			}
			(new cls_Logboek())->add($mess, 4, 0, 1, $this->mid, 41);
		}
	}  # Mailing->upload
	
	public function attach_delete($p_bestand) {
		
		$p_bestand = str_replace("!", ".", $p_bestand);
		$ad = $this->dir_attachm;
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
	}  # Mailing->attach_delete
	
	public function add_del_selectie($actie="aantal", $p_ondid=0, $p_vangebdatum="", $p_temgebdatum="") {
		
		$ondid = $_POST['selectie_groep'] ?? $p_ondid;
		$vangebdatum = $_POST['selectie_vangebdatum'] ?? $p_vangebdatum;
		$temgebdatum = $_POST['selectie_temgebdatum'] ?? $p_temgebdatum;
		
		$lidqry = sprintf("SELECT L.RecordID FROM %sLid AS L WHERE (L.Overleden IS NULL) AND (L.Verwijderd IS NULL)", TABLE_PREFIX);
		if (strlen($vangebdatum) == 10) {
			$lidqry .= sprintf(" AND L.GEBDATUM >= '%s'", $vangebdatum);
		}
		if ($temgebdatum >= $vangebdatum and strlen($temgebdatum) == 10) {
			$lidqry .= sprintf(" AND IFNULL(L.GEBDATUM, '') <= '%s'", $temgebdatum);
		}

		$i_el = new cls_Eigen_lijst("", $ondid);
		$i_base = new cls_db_base();
		$selnaam = "";
		if ($ondid > 0) {
			if ($i_el->elid > 0) {
				$subqry = $i_el->mysql(-1, 1);
				$selnaam = $i_el->naam;
			} else {
				$subqry = sprintf("(SELECT DISTINCT LO.Lid AS LidID FROM %sLidond AS LO WHERE %s AND LO.OnderdeelID=%d)", TABLE_PREFIX, cls_db_base::$wherelidond, $ondid);
				$selnaam = (new cls_Onderdeel())->naam($ondid);
			}
		} else {
			$subqry = sprintf("(SELECT LM.Lid AS LidID FROM %sLidmaatschap AS LM WHERE IFNULL(LM.Opgezegd, '9999-12-31') >= CURDATE())", TABLE_PREFIX);
			$selnaam = "alle leden";
		}
		
		$lidqry .= sprintf(" AND L.RecordID IN %s;", $subqry);
		
		$i_mr = new cls_Mailing_rcpt();
		
		$selrows = $i_base->execsql($lidqry)->fetchAll();
		
		$this->sl_aantingroep = count($selrows);
		$this->sl_aanttoevoegen = 0;
		$this->sl_aantverwijderen = 0;
		if ($actie == "add" or $actie == "delete") {
			foreach($selrows as $row) {
				if ($actie == "delete") {
					$mrrow = $i_mr->record($this->mid, $row->RecordID);
					if ($mrrow != false) {
						$mrid = $mrrow->RecordID;
						if ($i_mr->delete($mrid, 1) > 0) {
							$this->sl_aantverwijderen++;
						}
					}
				} elseif ($actie == "add" and $this->mid > 0) {
					if ($i_mr->add($this->mid, $row->RecordID, "", 1) > 0) {
						$this->sl_aanttoevoegen++;
					}
				}
			}
			
		}
		$mess = "";
		if ($actie == "add" and $this->sl_aanttoevoegen > 0) {
			$mess = sprintf("Er zijn %d ontvangers via een selectie (%s) aan mailing %d (%s) toegevoegd.", $this->sl_aanttoevoegen, $selnaam, $this->mid, $this->subject);
			(new cls_Logboek())->add($mess, 4, 0, 0, $this->mid, 11);
		} elseif ($actie == "delete" and $this->sl_aantverwijderen > 0) {
			$mess = sprintf("Er zijn %d ontvangers via een selectie (%s) bij mailing %d (%s) verwijderd.", $this->sl_aantverwijderen, $selnaam, $this->mid, $this->subject);
			(new cls_Logboek())->add($mess, 4, 0, 0, $this->mid, 13);
		}

		if ($actie == "add") {
			$rv = $this->sl_aanttoevoegen;
		} elseif ($actie == "delete") {
			$rv = $this->sl_aantverwijderen;
		} else {
			$rv['aantalingroep'] = $this->sl_aantingroep;
			$rv['aantalontvangers'] = $this->aant_rcpt;
		}
		
		return $rv;
		
	}   # Mailing->add_del_selectie
	
	public function setVanaf($p_mvid, $p_incc=0) {
		
		$i_mv = new cls_Mailing_vanaf($p_mvid);
		if ($i_mv->mvid > 0) {
			$this->mailingvanafid = $i_mv->mvid;
			if ($p_incc == 1) {
				$this->cc_addr = $i_mv->vanaf_email;
			}
		}
		$i_mv = null;
	}  # mailing->setVanaf
	
	public function preview() {
		$this->send_mailing(1);
	}
	
	public function send($lidid=0, $p_melding=1, $p_direct=0) {
		
		if ($this->mid > 0) {
			$this->controle($lidid);
			if ($this->verzendenmag == false) {
				(new cls_Logboek())->add($this->meldingen, 4, 0, $p_melding, $this->mid);
				return false;
			} else {
				if ($_SESSION['settings']['mailing_sentoutbox_auto'] == 0 and $_SESSION['settings']['mailing_direct_verzenden'] == 0) {
					$mess = "Het automatisch versturen staat uit, neem contact op met een beheerder om dit aan te laten zetten.";
					(new cls_Logboek())->add($mess, 4, 0, $p_melding, $this->mid);
				}
				return $this->send_mailing(0, $lidid, $p_melding, $p_direct);
			}
		}  else {
			$this->meldingen = "Er is geen mailing geselecteerd.";
			(new cls_Logboek())->add($this->meldingen, 4, 0, $p_melding);
			return false;
		}
	}  # Mailing->send
	
	private function send_mailing($preview=0, $p_lidid=0, $p_melding=1, $p_direct=0) {
		
		if ($p_lidid == 0) {
			$rcpts = (new cls_Mailing_rcpt())->lijst($this->mid);
			$pk = "RecordID";
		} else {
			$f = sprintf("L.RecordID=%d", $p_lidid);
			$rcpts = (new cls_Lid())->ledenlijst(0, 0, "", $f);
			$pk = "RecordID";
		}
		
		$aant_rcpts = 0;
		$aant_send = 0;
		foreach($rcpts as $rcpt) {
			$email = new email(0, $this->mid);
			if ($rcpt->LidID > 0) {
				$this->merge($rcpt->LidID);
				$email->Subject = $this->merged_subject;
				$email->lidid = $rcpt->LidID;
				$email->toevoegenlid($rcpt->LidID);
			} elseif (strlen($rcpt->to_address) > 5) {
				$email->lidid = 0;
				$email->toevoegenadres($rcpt->to_address);
				$this->bevat_losse_email = true;
			}
			$email->vanafid = $this->mailingvanafid;
			if (isValidMailAddress($this->cc_addr, 0)) {
				$email->toevoegenadres($this->cc_addr, "cc");
			}
			$email->bericht = $this->merged_message;
			$email->onderwerp = $this->merged_subject;
			$email->xtrachar = $this->xtrachar;
			$email->xtranum = $this->xtranum;
			
			if ($preview == 1) {
				$mail = new RBMmailer();
				$mail->IsHTML(true);
				$mail->Subject = $this->merged_subject;
				$mail->Body = $this->merged_message;
				if (strlen($this->OmschrijvingOntvangers) > 0) {
					$mail->addstationary($this->OmschrijvingOntvangers, "", 0, $this->zonderbriefpapier);
				} elseif (isset($rcpt->NaamLid)) {
					$mail->addstationary($rcpt->NaamLid, "", 0, $this->zonderbriefpapier);
				}
				echo($mail->Body);
				$mail = null;
			} elseif ($email->to_outbox($p_direct, $this->speciaal) == true) {
				$aant_send++;
			}
			$aant_rcpts++;
			$email = null;
		}
		
		if ($aant_send > 0) {
			if ($aant_send > 1) {
				$mess = sprintf("Via mailing %d (%s) zijn %d e-mails in de outbox geplaatst.", $this->mid, $this->subject, $aant_send);
				(new cls_Logboek())->add($mess, 4, 0, $p_melding, $this->mid, 24);
			}
		}

		return $aant_send;
		
	}   # Mailing->send_mailing
	
	public function preview_hist($p_mhid, $p_verwijderaanhef=1) {
		$i_mh = new cls_mailing_hist($p_mhid);
		$mhrow = $i_mh->record();
		$i_tp = new cls_Template(-1, "briefpapier");

		if ($mhrow->ZonderBriefpapier == 1) {
			return $mhrow->message;
		} elseif (strlen($i_tp->inhoud) > 0) {
			if ($p_verwijderaanhef == 1) {
				$htmlmessage = removetextblock($i_tp->inhoud, "<!-- Aanhef -->", "<!-- /Aanhef -->");
			} else {
				$htmlmessage = $i_tp->inhoud;
			}
			$htmlmessage = str_ireplace("[%MESSAGE%]", $mhrow->message, $htmlmessage);
			$htmlmessage = str_ireplace("[%FROM%]", $mhrow->from_name, $htmlmessage);
			$htmlmessage = str_ireplace("[%TO%]", $mhrow->AanNaam, $htmlmessage);
			$htmlmessage = str_ireplace("[%SUBJECT%]", $mhrow->subject, $htmlmessage);
			return $htmlmessage;
		} else {
			debug("De template briefpapier is niet ingevuld.", 1, 1);
			return false;
		}
	}  # Mailing->preview_hist
	
	public function merge($lidid=0) {
		global $dtfmt;
		
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
			$urlactivatie = sprintf("%s/index.php?tp=Validatie+login&lidid=%d&key=%s", BASISURL, $lidid, $nak);
			$this->merged_message = str_replace("[%URLACTIVATIE%]", $urlactivatie, $this->merged_message);
		
		} elseif ($this->mid == $_SESSION['settings']['mailing_herstellenwachtwoord'] and $lidid > 0) {
			$nk = (new cls_Login())->wachtwoordreset($lidid);
			$urlresetww = sprintf("%s/index.php?tp=Herstel+wachtwoord&lidid=%d&key=%s", BASISURL, $lidid, $nk);
			$this->merged_message = str_replace("[%URLRESETWW%]", $urlresetww, $this->merged_message);
			
		} elseif ($this->mid == $_SESSION['settings']['mailing_bevestigingopzegging'] and $this->xtranum > 0) {
			$this->merged_message = str_ireplace("[%Lidnummer%]", $this->xtranum, $this->merged_message);
			$i_lm = new cls_Lidmaatschap(-1, -1, $this->xtranum);
			$this->merged_message = str_ireplace("[%OpgezegdPer%]", $i_lm->opgezegdper, $this->merged_message);
			$i_lm = null;
			
		} elseif ($this->mid > 0 and $this->mid == $_SESSION['settings']['mailing_bevestigingdeelnameevenement'] and $this->xtranum > 0) {
			$dtfmt->setPattern(DTTEXT);
			$row = (new cls_Evenement_Deelnemer())->record($this->xtranum);
			$this->merged_message = str_ireplace("[%OMSEVENEMENT%]", $row->OmsEvenement, $this->merged_message);
			$this->merged_subject = str_ireplace("[%OMSEVENEMENT%]", $row->OmsEvenement, $this->merged_subject);
			
			$this->merged_message = str_ireplace("[%DATUMEVENEMENT%]", $dtfmt->format(strtotime($row->DatumEvenement)), $this->merged_message);
			$this->merged_subject = str_ireplace("[%DATUMEVENEMENT%]", $dtfmt->format(strtotime($row->DatumEvenement)), $this->merged_subject);
			
			$this->merged_message = str_ireplace("[%LOCATIEEVENEMENT%]", $row->Locatie, $this->merged_message);
			$this->merged_subject = str_ireplace("[%LOCATIEEVENEMENT%]", $row->Locatie, $this->merged_subject);
			
			$this->merged_message = str_ireplace("[%STATUSEVENEMENT%]", ARRDLNSTATUS[$row->Status], $this->merged_message);
			$this->merged_subject = str_ireplace("[%STATUSEVENEMENT%]", ARRDLNSTATUS[$row->Status], $this->merged_subject);
			
			$this->merged_message = str_ireplace("[%AANTALPERSONENEVENEMENT%]", $row->Aantal, $this->merged_message);
			if (strlen($row->Opmerking) > 0) {
				$opm = $row->Opmerking;
			} else {
				$opm = 'Geen';
			}
			$this->merged_message = str_ireplace("[%OPMEVENEMENT%]", $opm, $this->merged_message);

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
			$dtfmt->setPattern(DTTEXT);
			foreach($this->MergeField as $fld) {
				$nm = "[%" . $fld['Naam'] . "%]";
				$znm = trim("[%" . $fld['Naam']);
				$nmml = "";
				if ((stripos($this->merged_message, $znm) !== false or stripos($this->merged_subject, $nm)) !== false and isset($fld['SQL']) and strlen($fld['SQL']) > 5) {
					if ($fld['Naam'] == "OpenstaandeRekeningenTabel") {
						$rkrows = (new cls_Rekening())->lijst("telaat", $lidid);
						$nv = "";
						foreach ($rkrows as $rkrow) {
							$nv .= sprintf("<tr><td>%d</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", $rkrow->Nummer, $dtfmt->format(strtotime($rkrow->Datum)), $dtfmt->format(strtotime($rkrow->Betaaldatum)), number_format($rkrow->Bedrag, 2, ",", "."), number_format($rkrow->Betaald, 2, ",", "."), number_format($rkrow->Openstaand, 2, ",", "."));
						}
					} else {
						$query  = str_replace("%d", $lidid, $fld['SQL']);
						$result = (new cls_db_base())->execsql($query);
						$nv = "";
						foreach($result->fetchAll(PDO::FETCH_NUM) as $val) {
							if (substr($fld['Naam'], -3) == "_UL") {
								$nv .= "<li>";
							} elseif (strlen($nv) > 0) {
								$nv .= " en ";
							}
							$nv .= htmlentities($val[0]);
							if (substr($fld['Naam'], -3) == "_UL") {
								$nv .= "</li>\n";
							}
						}
						$ml = stripos($this->merged_message, $znm . "#");
						if ($ml !== false) {
							$bpl = $ml + strlen($znm) + 1;
							$epl = strpos($this->merged_message, "%]", $bpl);
							$lab = trim(substr($this->merged_message, $bpl, ($epl-$bpl)));
							$lab = str_replace("</li>", "", $lab);
							if (strlen($nv) > 0 and substr($lab, 0, 4) == "[li]") {
								$nv = str_replace("[li]", "<li>", $lab) . " " . $nv . "</li>";
							} elseif (strlen($nv) > 0 and substr($lab, 0, 4) == "[p]") {
								$nv = str_replace("[p]", "<p>", $lab) . " " . $nv . ".</p>";
							} elseif (strlen($nv) > 0) {
								$nv = $lab . " " . $nv;
							}
							$nmml = $znm . "#". $lab . "%]";
						} elseif ((strlen($nv) == 0 or $nv == "9999-12-31") and $fld['Naam'] == "OpgezegdPer") {
							$nv = "Niet";
						} elseif (strlen($nv) == 0 and ($fld['Naam'] == "Afdelingen" or $fld['Naam'] == "Toestemmingen" or $fld['Naam'] == "Toestemmingen_UL")) {
							$nv = "Geen";
						} elseif (substr($fld['Naam'], -3) == "_UL") {
							$nv = "<ul>\n" . $nv . "</ul>";
						}
					}
					$this->merged_subject = str_ireplace($nm, $nv, $this->merged_subject);
					$this->merged_message = str_ireplace($nm, $nv, $this->merged_message);
					$this->merged_message = str_ireplace($nmml, $nv, $this->merged_message);
					$this->contains_mergefield = true;
					if ($this->xtrachar == "LNR" and $nm == "[%Lidnummer%]" and $this->xtranum == 0) {
						$this->xtranum = $nv;
					}
				}
			}
		}
		return $this->merged_message;
	}  # Mailing->merge
	
	public function delete() {
		(new cls_Mailing())->trash($this->mid, "in");
	}  # Mailing->delete
	
	public function undelete() {
		(new cls_Mailing())->trash($this->mid, "out");
	}  # Mailing->undelete
	
} # class Mailing

function lijstmailings($p_filter="") {
	global $currenttab, $currenttab2;
	
	$i_m = new cls_Mailing();

	if (isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0 and $_GET['op'] == "bekijkmail") {
		$xtra = "<p class='mededeling'><input type='button' value='Terug' onclick='history.go(-1);'></p>\n";
		$email = new email($_GET['MailingHistID']);
		$email->toon();
		$mail = null;
		
	} elseif (isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0 and $_GET['op'] == "edit_email") {
		$xtra = "<p class='mededeling'><input type='button' value='Terug' onclick='history.go(-1);'></p>\n";
		$email = new email($_GET['MailingHistID']);
		$email->edit();
		$mail = null;
			
	} elseif ($p_filter == "Verzonden mails" or $p_filter == "Verzonden e-mails" or $p_filter == "Outbox") {
		$i_mh = new cls_Mailing_hist();
		$l = sprintf("index.php?tp=%s&op=bekijkmail&MailingHistID=%%d", urlencode($_GET['tp']));
		$kols[] = array('link' => $l, 'headertext' => "&nbsp;", 'columnname' => "RecordID", 'class' => "viewmail");
		
		$kols[] = array('headertext' => "Verzonden", 'columnname' => "send_on", 'type' => "DTLONG");
		$kols[] = array('columnname' => "Vanaf_naam", 'headertext' => "Vanaf");
		$kols[] = array('columnname' => "Aan", 'type' => "combitext");
		$kols[] = array('headertext' => "Onderwerp", 'columnname' => "subject");
		
		if ($_SESSION['settings']['maxmailsperdag'] > 0 and $i_mh->aantalverzonden(1) >= $_SESSION['settings']['maxmailsperdag']) {
			
		} elseif ($_SESSION['settings']['maxmailsperuur'] > 0 and $i_mh->aantalverzonden(2) >= $_SESSION['settings']['maxmailsperuur']) {
			
		} elseif ($p_filter == "Verzonden mails" or $p_filter == "Verzonden e-mails") {
			
		}
		if ($p_filter == "Outbox") {
			$kols[1] = array('headertext' => "Aangemaakt op/om", 'columnname' => "Ingevoerd", 'type' => "DTHMS");
			
			$l = sprintf("index.php?tp=%s&op=edit_email&MailingHistID=%%d", urlencode($_GET['tp']));
			$kols[5] = array('columnname' => "RecordID", 'link' => $l, 'class' => "muteren");

			$kols[6]['link'] = sprintf("index.php?tp=%s&op=sentmail&MailingHistID=%%d'", urlencode($_GET['tp']));
			$kols[6]['columnname'] = "RecordID";
			$kols[6]['class'] = "sendmail";
			
			$kols[7] = array('headertext' => "Versturen na", 'columnname' => "NietVersturenVoor", 'type' => "DTHMS");
			
			$kols[8]['link'] = sprintf("index.php?tp=%s&op=deletehist&MailingHistID=%%d", urlencode($_GET['tp']));
			$kols[8]['columnname'] = "RecordID";
			$kols[8]['class'] = "trash";
			
			if ($_SESSION['settings']['mailing_sentoutbox_auto'] == 0 and $_SESSION['settings']['mailing_direct_verzenden'] == 0) {
				$mess = "Het automatisch versturen staat uit, neem contact op met een beheerder om dit aan te laten zetten.";
				(new cls_Logboek())->add($mess, 4, 0, 1);
			}
			
			$rows = $i_mh->outbox(1)->fetchAll();
			if (count($rows) > 0) {
				if (count($rows) > 1) {
					echo("<div id='filter'>\n");
					printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
					if (WEBMASTER) {
						printf("<button class='%s btn-sm' name='outboxlegen'>%s Outbox legen</button>\n", CLASSBUTTON, ICONVERWIJDER);
					}
					printf("<p class='aantrecords'>%d e-mails</p>\n", count($rows));
					echo("</form>\n");
					echo("</div> <!-- Einde filter -->\n");
				}
				echo(fnDisplayTable($rows, $kols, "", 0, "", "lijstoutbox"));
			} else {
				echo("<p class='mededeling'>Er staan geen e-mails te wachten om verzonden te worden.</p>\n");
			}
		} else {
			$kols[] = array('columnname' => "RecordID", 'type' => "pk", 'skip' => true);
			if (WEBMASTER) {
				$kols[] = array('columnname' => "RecordID", 'headertext' => "&nbsp;", 'type' => "click", 'class' => "mislukt", 'onclick' => "mailtooutbox(%d)", 'title' => "Versturen is mislukt, verplaatsen naar de outbox.");
			}
			
			$rows = $i_mh->lijst();
			echo("<div id='filter'>\n");
			echo("<input id='tbTekstFilter' placeholder='Tekstfilter' onKeyUp=\"fnFilter('lijstverzondenmails', this);\">\n");
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
		if (toegang("Mailing/Wijzigen mailing")) {
			$l = sprintf("%s?tp=%s/Wijzigen+mailing&mid=%%d", $_SERVER['PHP_SELF'], $currenttab);
			$kols[] = array('columnname' => "RecordID", 'link' => $l, 'class' => "muteren", 'title' => "Wijzigen mailing");
		}
		$kols[] = array('headertext' => "Onderwerp / Opmerking", 'columnname' => "Onderwerp_Opmerking", 'type' => "mrg");
		$kols[] = array('headertext' => "Van / Aan", 'columnname' => "Van_Aan", 'type' => "mrg");
		$kols[] = array('headertext' => "Laatst gewijzigd", 'columnname' => "RecordID", 'type' => "laatsteMutatie_Naam", 'table' => "Mailing");
		$kols[] = array('headertext' => "Verwijderd op", 'columnname' => "deleted_on", 'type' => "date");

		$l = sprintf("%s?tp=Mailing/Historie&op=historie&mid=%%d", $_SERVER['PHP_SELF']);
		$kols[] = array('columnname' => "linkHist", 'link' => $l, 'class' => "detailregels", 'title' => "Verstuurde e-mails");
		
		$rows = $i_m->lijst($p_filter);
		echo("<div id='filter'>\n");
		echo("<input id='tbTekstFilter' placeholder='Tekstfilter' onKeyUp=\"fnFilter('lijstmailingen', this);\">\n");
		printf("<p class='aantrecords'>%d mailings</p>\n", count($rows));
		echo("</div> <!-- Einde filter -->\n");
		
		echo(fnDisplayTable($rows, $kols, "", 0, "", "lijstmailingen"));
		if ($p_filter == "Prullenbak" and $_SESSION['settings']['mailing_bewaartijd'] > 0) {
			printf("<p>Na %d maanden worden deze mailings definitief verwijderd.</p>\n", $_SESSION['settings']['mailing_bewaartijd']);
		}
	}
	$i_m = null;
} # lijstmailings

class email {
	private $mhid = 0;
	public $mailingid = 0;
	public $lidid = 0;
	private $ingevoerd = "";
	private string $ingevoerddoor = "";	// Naam van de verstuurder
	private string $verstuurd_dt = "";
	private string $verstuurdop = "";	// Datum voluit geschreven
	public $vanafnaam = "";
	public $vanafadres = "";
	public $vanafid = 0;
	public $replyid = 0;
	public $omsontvangers = "";
	public $aannaam = "";
	public $aanadres = "";
	public $cc = "";
	private $CCafdelingen = 0;
	public $zichtbaarvoor = -1;
	private $zichtbaar = false;
	
	public $onderwerp = "";
	public $bericht = "";
	public $zonderbriefpapier = 0;
	public string $nietversturenvoor = "";
	public $xtrachar = "";
	public $xtranum = 0;
	
	function __construct($p_mhid=0, $p_mid=-1) {
		global $dtfmt;
		
		$dtfmt->setPattern(DTLONGSEC);
		$this->ingevoerd = $dtfmt->format(time());
		$this->ingevoerddoor = $_SESSION['naamingelogde'];

		$this->vulvars($p_mhid, $p_mid);
		
	}  # __construct
	
	public function vulvars($p_mhid, $p_mid=-1) {
		global $dtfmt;
		
		$this->mhid = $p_mhid;
		if ($p_mid > 0) {
			$this->mailingid = $p_mid;
		}
		
		if ($this->mhid > 0) {
			$mhrow = (new cls_Mailing_hist())->record($this->mhid);
			if (isset($mhrow->MailingID)) {
				$this->mailingid = $mhrow->MailingID;
				$dtfmt->setPattern(DTLONG);
				$this->ingevoerd = $dtfmt->format(strtotime($mhrow->Ingevoerd));
				$this->ingevoerddoor = (new cls_Lid())->Naam($mhrow->IngevoerdDoor);
				$this->verstuurd_dt = $mhrow->send_on ?? "";
				if ($mhrow->send_on > "2010-01-01") {
					$this->verstuurdop = $dtfmt->format(strtotime($this->verstuurd_dt));
				}
				$this->lidid = $mhrow->LidID;
				$this->vanafid = $mhrow->VanafID;
				$this->omsontvangers = htmlentities($mhrow->OmschrijvingOntvangers);
				$this->aannaam = htmlentities($mhrow->AanNaam);
				$this->aanadres = $mhrow->to_addr;
				$this->cc = $mhrow->cc_addr ?? "";
				$this->CCafdelingen = $mhrow->CCafdelingen ?? 0;
				$this->onderwerp = $mhrow->subject;
				$this->bericht = $mhrow->message;
				$this->zonderbriefpapier = $mhrow->ZonderBriefpapier ?? 0;
				$this->nietversturenvoor = $mhrow->NietVersturenVoor ?? "";
				$this->zichtbaarvoor = $mhrow->ZichtbaarVoor ?? 0;
				$this->xtrachar = $mhrow->Xtra_Char ?? "";
				$this->xtranum = $mhrow->Xtra_Num;
			}
			
		} elseif ($this->mailingid > 0) {	
			$mrow = (new cls_Mailing())->record($this->mailingid);
			$this->vanafid = $mrow->MailingVanafID;
			$this->omsontvangers = htmlentities($mrow->OmschrijvingOntvangers);
			$this->cc = $mrow->cc_addr;
			$this->CCafdelingen = $mrow->CCafdelingen;
			$this->onderwerp = $mrow->subject;
			$this->bericht = $mrow->message;
			$this->zichtbaarvoor = $mrow->ZichtbaarVoor ?? 0;
			$this->zonderbriefpapier = $mrow->ZonderBriefpapier ?? 0;
		} else {
			$i_mv = new cls_Mailing_vanaf($this->vanafid);
			$this->vanafid = $i_mv->min();
			$i_mv = null;			
		}
		
		if ($this->vanafid > 0) {
			$i_mv = new cls_Mailing_vanaf($this->vanafid);
			$this->vanafnaam = $i_mv->vanaf_naam;
			$this->vanafadres = $i_mv->vanaf_email;
			$i_mv = null;
		}
		
		if (WEBMASTER) {
			$this->zichtbaar = true;
		} elseif ($this->zichtbaarvoor == 0) {
			$this->zichtbaar = true;
		} elseif (in_array($_SESSION['settings']['mailing_alle_zien'], explode(",", $_SESSION['lidgroepen'])) == true) {
			$this->zichtbaar = true;
		} elseif ($this->zichtbaarvoor > 0 and in_array($this->zichtbaarvoor, explode(",", $_SESSION['lidgroepen'])) == true) {
			$this->zichtbaar = true;
		} else {
			$this->zichtbaar = false;
		}
	}  # email->vulvars
	
	public function toon($p_direct=1) {
		
		if ($this->zichtbaar) {

			$txt = "<div id='verstuurdemail'>\n";
			$txt .= sprintf("<label class='form-label'>Klaar gezet op</label><p>%s</p><label id='lblIngevoerdDoor'>door</label><p>%s</p>\n", $this->ingevoerd, $this->ingevoerddoor);
		
			if (strlen($this->verstuurdop) > 0) {
				$txt .= sprintf("<label class='form-label'>Verzonden</label><p>%s</p>\n", $this->verstuurdop);
			} else {
				$txt .= sprintf("<label class='form-label'>Verzonden</label><p>Wacht in outbox</p>\n");
			}
		
			if ($this->zonderbriefpapier == 0 and strlen($this->bericht) > 10) {
				$txt .= sprintf("<label id='lblGaNaar' class='form-label'>Ga naar</label><p><a href='%s?tp=Mailing&op=preview_hist&mhid=%d'>preview</a></p>", $_SERVER['PHP_SELF'], $this->mhid);
			}
		
			if (strlen($this->vanafnaam) > 0) {
				$txt .= sprintf("<label class='form-label'>Van</label><p>%s</p><label id='lblEmail'>E-mail</label><p>%s</p>\n", $this->vanafnaam, $this->vanafadres);
			} else {
				$txt .= sprintf("<label class='form-label'>Van</label><p>%s</p>\n", $this->vanafadres);
			}
		
			if (strlen($this->omsontvangers) > 0) {
				$txt .= sprintf("<label class='form-label'>Aan</label><p>%s</p>\n", $this->omsontvangers);
			}
		
			if (strlen($this->aannaam) > 0 and $this->aannaam != $this->aanadres) {
				$txt .= sprintf("<label class='form-label'>Ontvanger</label><p>%s</p>", $this->aannaam);
				$txt .= sprintf("<label id='lblEmailOntvanger' class='form-label'>E-mail</label><p>%s</p>\n", $this->aanadres);
			} else {
				$txt .= sprintf("<label class='form-label'>Aan e-mail</label><p>%s</p>\n", $this->aanadres);
			}
			if (strlen($this->cc) > 0) {
				$txt .= sprintf("<label class='form-label'>CC</label><p>%s</p>\n", $this->cc);
			}
		
			$txt .= sprintf("<label class='form-label'>Onderwerp</label><p>%s</p>\n", $this->onderwerp);
		
			$txt .= "</div>  <!-- Einde verstuurdemail -->\n";
			
			if (strlen($this->bericht) < 10 and $this->mailingid > 0) {
				$b = (new Mailing($this->mailingid))->merge($this->lidid);
			} else {
		
				$b = str_replace("<!DOCTYPE html>", "", $this->bericht);
				$b = str_replace("<html>", "", str_replace("</html>", "", $b));
				$b = str_replace("<body>", "", str_replace("</body>", "", $b));
				$b = removetextblock($b, "<head>", "</head>");
			}

			$txt .= sprintf("<div class='bericht'>\n%s\n</div> <!-- Einde bericht -->\n", $b);
		} else {
			$mess = sprintf("Je bent niet bevoegd om e-mail %d te bekijken.", $this->mhid);
			$txt = sprintf("<p class='mededeling'>%s</p>\n", $mess);
			(new cls_Logboek())->add($mess, 4, -1, -1, $this->mhid, 26);
		}
		
		if ($p_direct == 1) {
			echo($txt);
		}

		return $txt;
		
	} # email->toon
	
	public function edit($p_mhid=-1, $p_lidid=-1) {
		global $currenttab, $currenttab2;
		
		if ($this->verstuurdop > "2000-01-01") {
			
			$mess = sprintf("Email %d is al verzonden en mag niet meer bewerkt worden.", $this->mhid);
			$txt = sprintf("<p class='mededeling'>%s</p>\n", $mess);
			(new cls_Logboek())->add($mess, 4, -1, -1, $this->mhid, 22);
				
		} elseif ($this->zichtbaar) {

			$txt = sprintf("<form method='post' id='verstuurdemail' action='%s?tp=%s/%s&op=edit_email'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
			$txt .= sprintf("<label class='form-label'>RecordID</label><p id='recordid'>%d</p>\n", $this->mhid);
			$txt .= sprintf("<label class='form-label'>Klaar gezet op</label><p>%s</p><label id='lblIngevoerdDoor'>door</label><p>%s</p>\n", $this->ingevoerd, $this->ingevoerddoor);
		
			if (strlen($this->vanafnaam) > 0) {
				$txt .= sprintf("<label class='form-label'>Van</label><p>%s</p><label id='lblEmail'>E-mail</label><p>%s</p>\n", $this->vanafnaam, $this->vanafadres);
			} else {
				$txt .= sprintf("<label class='form-label'>Van</label><p>%s</p>\n", $this->vanafadres);
			}
		
			if (strlen($this->aannaam) > 0 and $this->aannaam != $this->aanadres) {
				$txt .= sprintf("<label class='form-label'>Ontvanger</label><p>%s</p>", $this->aannaam);
				$txt .= sprintf("<label class='form-label' id='lblEmailOntvanger'>E-mail</label><p>%s</p>\n", $this->aanadres);
			} else {
				$txt .= sprintf("<label class='form-label'>Aan e-mail</label><p>%s</p>\n", $this->aanadres);
			}
			
			$txt .= sprintf("<label class='form-label'>CC</label><input type='text' class='w50' maxlength=50 id='cc_addr' value='%s'>\n", $this->cc);
		
			$txt .= sprintf("<label class='form-label'>Onderwerp</label><input type='text' class='w75' maxlength=75 id='subject' value='%s'>\n", $this->onderwerp);
			if (strlen($this->verstuurd_dt) < 10) {
				$txt .= sprintf("<label class='form-label'>Niet versturen voor</label><input type='datetime-local' id='NietVersturenVoor' value='%s'>\n", $this->nietversturenvoor);
			}
		
			$txt .= "<div class='clear'></div>\n";
			$txt .= sprintf("<textarea id='message'>%s</textarea>\n", $this->bericht);
			
			$txt .= "</form>\n";
			
		} else {
			$mess = sprintf("Je bent niet bevoegd om e-mail %d te bewerken.", $this->mhid);
			$txt = sprintf("<p class='mededeling'>%s</p>\n", $mess);
			(new cls_Logboek())->add($mess, 4, -1, -1, $this->mhid, 22);
		}
		
		echo($txt);
		js_editor(1);
		printf("<script>
		\$('input').on('blur', function(){
			savedata('email', %1\$d, this);
		});
		</script>", $this->mhid);
		
	}  # email->edit
	
	public function toevoegenlid($p_lidid, $p_tp="aan") {
	
		$this->lidid = $p_lidid;
		$row = (new cls_Lid())->record($this->lidid);
		
		if (isValidMailAddress($row->Email, 0)) {
			$this->toevoegenadres($row->Email, $p_tp, $row->NaamLid);
		} elseif (strlen($row->EmailOuders) > 5) {
			$this->toevoegenadres($row->EmailOuders, $p_tp);
		} elseif (isValidMailAddress($row->EmailVereniging, 0)) {
			$this->toevoegenadres($row->EmailVereniging, $p_tp, $row->NaamLid);
		}
		
		if ($this->xtrachar != "REK" and $p_tp == "aan") {
			$this->aannaam = $row->NaamLid;
		}
		
		if ($row->GEBDATUM > date("Y-m-d", strtotime("-18 year")) and strlen($row->EmailOuders) > 5) {
			$this->toevoegenadres($row->EmailOuders, "cc");
		}
		
	}  # email->toevoegenlid
	
	public function toevoegenadres($p_adres, $tp="aan", $p_naam="") {
		
		$adres = str_replace(" ", "", str_replace(";", ",", $p_adres));
		foreach(explode(",", $adres) as $e) {
			if ($tp == "aan" or $tp == "to") {
				if (isValidMailAddress($e, 0) and strpos($this->aanadres, $e) === false) {
					if (strlen($this->aanadres) > 0) {
						$this->aanadres .= ",";
					}
					$this->aanadres .= trim($e);
					$this->aannaam = $p_naam;
				}
			} elseif ($tp == "cc") {
				if (isValidMailAddress($e, 0) and strpos($this->aanadres, $e) === false and strpos($this->cc, $e) === false) {
					if (strlen($this->cc) > 0) {
						$this->cc .= ",";
					}
					$this->cc .= trim($e);
				}
			}
		}
		
	}  # email->toevoegenadres
	
	public function to_outbox($p_direct=0, $p_spec="") {
		
		$rv = 0;
		
		if ($this->vanafid <= 0 and strlen($this->vanafadres) > 5) {
			$i_mv = new cls_Mailing_vanaf();
			$i_mv->vulvars(-1, $this->vanafadres);
			$this->vanafid = $i_mv->mvid;
			$i_mv = null;
		}
		
		$naamlid = "";
		if ($this->lidid > 0) {
			$i_lid = new cls_Lid($this->lidid);
			$naamlid = $i_lid->naamlid;
			$this->aanadres = $i_lid->email;
			$i_lid = null;
		}
		
		$i_mv = new cls_Mailing_vanaf($this->vanafid);
		$this->vanafadres = $i_mv->vanaf_email;
		$this->vanafnaam = $i_mv->vanaf_naam;
		$i_mv = null;
			
		$this->bericht = str_ireplace("[%FROMNAME%]", $this->vanafnaam, $this->bericht);
		
		if ($this->vanafid <= 0) {
			$mess = "Er is geen Van ingevuld.";
			debug($mess, 1, 1);
			
		} elseif (strlen($this->aanadres) == 0 and $this->lidid > 0) {
			$mess = sprintf("Er is geen e-mailadres van %s bekend. De e-mail kan niet worden verzonden.", $naamlid);
			(new cls_logboek())->add($mess, 4, $this->lidid, 11, 0, 21);
			
		} elseif(isValidMailAddress($this->aanadres) == false and strpos($this->aanadres, ",") == false) {
			$mess = sprintf("E-mailadres '%s' is geen geldig e-mailadres.", $this->aanadres);
			(new cls_logboek())->add($mess, 4, $this->lidid, 11, 0, 21);
			
		} else {
			
			if ($this->CCafdelingen == 1 and $this->lidid > 0) {
				foreach((new cls_Lidond())->lijstperlid($this->lidid, "A") as $afdrow) {
					if (isValidMailAddress($afdrow->CentraalEmail) and strpos($this->cc, $afdrow->CentraalEmail) === false) {
						if (strlen($this->cc) > 4) {
							$this->cc .= ",";
						}
						$this->cc .= $afdrow->CentraalEmail;
					}
				}
			}
			
			$wt = $_SESSION['settings']['mailing_wachttijdinoutbox'] ?? 0;
			if ($p_direct == 1 or $_SESSION['settings']['mailing_direct_verzenden'] == 1) {
				$this->nietversturenvoor = date("Y-m-d H:i:s");
			} elseif ($p_direct > 1 and $wt > $p_direct) {
				$this->nietversturenvoor = date("Y-m-d H:i:s", strtotime(sprintf("+%d minute", intval($wt/$p_direct))));
			} else {
				$this->nietversturenvoor = date("Y-m-d H:i:s", strtotime(sprintf("+%d minute", $wt)));
			}
			$this->mhid = (new cls_Mailing_hist())->add($this);
			if ($this->mhid > 0) {
				$rv = $this->mhid;
			}
		}
		if (strlen($p_spec) > 0) {
			sentoutbox(4);
		} elseif ($p_direct == 1) {
			sentoutbox(3);
		}
		
		return $rv;
		
	}  # email->to_outbox

}  # class email

function fnRekeningenMailen($op) {
	global $currenttab, $currenttab2;
	
	$i_rk = new cls_Rekening();
	$i_p = new cls_Parameter();
 
	echo("<div id='rekeningenmailen'>\n");
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if ($op == "selectierekeningen") {
			$mindat = $_POST['mindatum'] ?? "1970-01-01";
			$maxdat = $_POST['maxdatum'] ?? "9999-12-31";
			if ($mindat > $maxdat) {
				$maxdat = $mindat;
			}
			$filter = sprintf("RK.seizoen=%d AND RK.Nummer >= %d AND RK.Nummer <= %d AND RK.Datum >= '%s' AND RK.Datum <= '%s'", $_POST['rekseizoen'], $_POST['minreknr'], $_POST['maxreknr'], $mindat, $maxdat);
			if (isset($_POST['reklid']) and is_numeric($_POST['reklid']) and $_POST['reklid'] > 0) {
				$filter .= sprintf(" AND RK.Lid=%d", $_POST['reklid']);
			}
			if (isset($_POST['nulrekeningen'])) {
				$_POST['nulrekeningen'] = 1;
			} else {
				$filter .= " AND RK.Bedrag <> 0";
				$_POST['nulrekeningen'] = 0;
			}
			if (!isset($_POST['eerderverzonden'])) {
				$filter .= sprintf(" AND (RK.Nummer NOT IN (SELECT Xtra_Num FROM %sMailing_hist WHERE Xtra_Char='REK'))", TABLE_PREFIX);
				$_POST['eerderverzonden'] = 0;
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

			$selrek = "";
			$aantrek = 0;
			$rkrows = $i_rk->lijst($filter);
			foreach($rkrows as $rk) {
				$selrek .= sprintf("<li>%d - %s - &euro;&nbsp;%03.2f</li>\n", $rk->Nummer, $rk->DEBNAAM, $rk->Bedrag);
				$aantrek += 1;
			}
			
			if (strlen($selrek) > 0) {
				printf("<form method='post' action='%s?tp=%s/%s&op=selectieversturen' name='mailrek'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
				printf("De volgende %d rekeningen zijn geselecteerd.\n", count($rkrows));
				printf("<ul>\n%s</ul>\n<div class='clear'></div>\n", $selrek);
				printf("<input type='hidden' name='rekfilter' value=\"%s\">\n", $filter);
				echo("<div id='opdrachtknoppen'>\n");
				echo("<input type='checkbox' value='1' name='sure'>\n");
				echo("<input type='submit' value='Rekeningen versturen' name='StuurRek'>\n");
				echo("<input type='button' value='Terug' onClick='history.go(-1);'>\n");
				echo("</div>  <!-- Einde opdrachtknoppen -->\n");
				echo("</form>\n");
			} else {
				echo("<p class='mededeling'>Er zijn geen rekeningen geselecteerd.</p>");
			}
		} elseif ($op == "selectieversturen" and isset($_POST['sure'])) {
			$aant_ok = 0;
			$_SESSION['settings']['mailing_rekening_stuurnaar'] = $_SESSION['settings']['mailing_rekening_stuurnaar'] ?? 1;
				
			foreach($i_rk->lijst($_POST['rekfilter']) as $rk) {
				$aant_ok += fnVerstuurRekening($rk->Nummer);
			}
		} elseif ($op == "selectieversturen") {
			$mess = "De rekeningen zijn niet verzonden, omdat het vinkje niet gezet was.";
			(new cls_Logboek())->add($mess, 4, 0, 1);
		}
	}

	if ($_SERVER['REQUEST_METHOD'] != "POST" or $op == "selectieversturen") {
		$i_p->vulsessie();
		printf("<form method='post' action='%s?tp=%s/%s&op=selectierekeningen'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
		$rekseizoen = (new cls_Seizoen())->max("Nummer");
		printf("<label class='form-label'>Seizoen</label><select name='rekseizoen' class='form-select form-select-sm'>\n%s</select>\n", (new cls_Seizoen())->htmloptions($rekseizoen, 1));
		printf("<label class='form-label'>Rekeningnummer</label><input type='number' class='d8' value=%d name='minreknr'><label class='form-label totenmet'>tot en met</label><input type='number' class='d8' value=%d name='maxreknr'>\n", $i_rk->min("Nummer"), $i_rk->max("Nummer"));

		$f = sprintf("RK.Seizoen=%d", $rekseizoen);
		printf("<label class='form-label'>Rekeningdatum</label><input type='date' value='%s' name='mindatum'><label class='totenmet'>tot en met</label><input type='date' value='%s' name='maxdatum'>\n", $i_rk->min("Datum", $f), $i_rk->max("Datum", $f));
		printf("<label class='form-label'>Betaald door (debiteur)</label><select name='reklid' class='form-select form-select-sm'>\n<option value=-1>*** Iedereen ***</option>\n%s</select>\n", (new cls_lid())->htmloptions(-1, 3));
		
		echo("<div class='form-check form-switch'>\n");
		echo("<label class='form-label'>Volledig betaald</label><input type='checkbox' class='form-check-input' name='volbetaald' value=1>\n");
		echo("<label class='form-label'>Gedeeltelijk betaald</label><input type='checkbox' name='deelbetaald' class='form-check-input' value=1 checked>\n");
	
		echo("<label class='form-label'>Niet betaald</label><input type='checkbox' name='nietbetaald' class='form-check-input' value=1 checked>\n");
		echo("<label class='form-label'>Nul rekeningen</label><input type='checkbox' name='nulrekeningen' class='form-check-input' value=1>\n");
		echo("<label class='form-label'>Eerder verzonden</label><input type='checkbox' name='eerderverzonden' class='form-check-input' value=1>\n");

		echo("</div>\n");

		echo("<div id='opdrachtknoppen'>\n");
		printf("<button type='submit' class='%s'>%s Verder</button>\n", CLASSBUTTON, ICONSUBMIT);
		echo("</div>  <!-- Einde opdrachtknoppen -->\n");
		
		echo("</form>\n");
	}
	echo("</div> <!-- Einde rekeningenmailen -->\n");
	
	$i_p = null;
} # fnRekeningenMailen

function fnVerstuurRekening($p_rkid, $p_toonmelding=0) {
	
	$i_rk = new cls_Rekening();
	
	$i_rk->controle($p_rkid);
	$rkrow = $i_rk->record($p_rkid);
	$stuurnaar = $_SESSION['settings']['mailing_rekening_stuurnaar'];
	
	$email = new email(0, 0);
	$email->xtrachar = "REK";
	$email->xtranum = $rkrow->Nummer;
	if ($stuurnaar == 5 or $stuurnaar == 6) {
		$email->toevoegenlid($rkrow->Lid);
	} else {
		$email->toevoegenlid($rkrow->BetaaldDoor);
	}
	$email->zichtbaarvoor = $_SESSION['settings']['mailing_rekening_zichtbaarvoor'];
	$email->vanafid = $_SESSION['settings']['mailing_rekening_vanafid'];
	if ($_SESSION['settings']['mailing_rekening_replyid'] > 0 and $_SESSION['settings']['mailing_rekening_replyid'] != $email->vanafid) {
		$email->replyid = $_SESSION['settings']['mailing_rekening_replyid'];
	}
	$tm = $p_toonmelding;
	$rv = 0;

	if ($stuurnaar == 2 or $stuurnaar == 3) {
		$hd = new DateTime($rkrow->Datum);
		$hd->sub(new DateInterval('P18Y'));
		foreach((new cls_Rekeningregel())->perrekening($rkrow->Nummer) as $regel) {
			if ($regel->GEBDATUM > "1900-01-01" and $regel->GEBDATUM <= $hd->format("Y-m-d") and ($stuurnaar == 2 or $stuurnaar == 6)) {
				$email->toevoegenlid($regel->Lid);
			} elseif ($stuurnaar == 3 or $stuurnaar == 7) {
				$email->toevoegenlid($regel->Lid);
			}
		}
	}
	if ($_SESSION['settings']['mailing_rekening_stuurnaar'] == 4 and $rkrow->BetaaldDoor != $rkrow->Lid) {
		$email->toevoegenlid($rkrow->Lid);
	}
				
	$email->bericht = RekeningDetail($rkrow->Nummer);
	if ($rkrow->BetaaldDoor != $rkrow->Lid) {
		$email->aannaam = (new cls_Lid())->naam($rkrow->BetaaldDoor);
	} else {
		$email->aannaam = $rkrow->Tenaamstelling;
	}
	$email->onderwerp = $rkrow->Nummer . " ". $rkrow->OMSCHRIJV;
				
	if (strlen($email->aanadres) == 0) {
		$mess = sprintf("Rekening %d kan niet worden verzonden, omdat er van %s geen geldig e-mailadres in de database bekend is.", $rkrow->Nummer, $email->aannaam);
		$tm = 1;
	} else {
		if ($email->to_outbox(0)) {
			$mess = sprintf("Rekening %d van %s (%s) is in de outbox geplaatst.", $rkrow->Nummer, $email->aannaam, $email->aanadres);
			$rv = 1;
		} else {
			$mess = sprintf("Rekening %d kon niet in de outbox worden geplaatst.", $rkrow->Nummer);
			$tm = 1;
		}
	}
	(new cls_Logboek())->add($mess, 4, $rkrow->Lid, $tm, $rkrow->Nummer, 21);
	$email = null;	
	
	return $rv;

}  # fnVerstuurRekening

function fnMailingInstellingen() {
	global $currenttab, $currenttab2;
	
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
		$i_p->update("mailing_bewaartijd", intval("0" . $_POST['mailing_bewaartijd']));
		$i_p->update("mailing_bewaartijd_ontvangers", intval("0" . $_POST['mailing_bewaartijd_ontvangers']));
		$i_p->update("mailing_verzonden_opschonen", intval("0" . $_POST['mailing_verzonden_opschonen']));
		$i_p->update("mailing_hist_opschonen", intval("0" . $_POST['mailing_hist_opschonen']));
		$i_p->update("maxmailsperminuut", intval("0" . $_POST['maxmailsperminuut']));
		$i_p->update("maxmailsperuur", intval("0" . $_POST['maxmailsperuur']));
		$i_p->update("maxmailsperdag", intval("0" . $_POST['maxmailsperdag']));
		$i_p->update("path_attachments", $_POST['path_attachments']);
		$i_p->update("mailing_extensies_toegestaan", $_POST['mailing_extensies_toegestaan']);
		$i_p->update("max_grootte_bijlage", $_POST['max_grootte_bijlage']);
		
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
		$i_p->update("mailing_bevestigingdeelnameevenement", $_POST['mailing_bevestigingdeelnameevenement']);
		$i_p->update("mailing_bevestigingbestelling", $_POST['mailing_bevestigingbestelling']);
	}
		
	if (isset($_GET['delete_vanaf']) and $_GET['delete_vanaf'] > 0) {
		$i_mv->delete($_GET['delete_vanaf']);
	} elseif (isset($_POST['vanaf_adres_toevoegen'])) {
		$i_mv->add();
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

	printf("<form method='post' id='mailing_instellingen' class='form-check form-switch' action='%s?tp=%s/%s'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
	
	echo("<h2>Algemeen</h2>\n");
	printf("<label class='form-label'>E-mails in de outbox direct probereren te versturen?</label><input type='checkbox' class='form-check-input' name='mailing_direct_verzenden' value='1'%s>\n", checked($_SESSION['settings']['mailing_direct_verzenden']));
	printf("<label class='form-label'>E-mails uit de outbox automatisch in de achtergrond versturen?</label><input type='checkbox' class='form-check-input' name='mailing_sentoutbox_auto' value='1'%s>\n", checked($_SESSION['settings']['mailing_sentoutbox_auto']));
	printf("<label class='form-label'>Wachttijd voor automatisch versturen?</label><input type='number' class='num3' name='mailing_wachttijdinoutbox' value=%d><p>minuten</p>\n", $_SESSION['settings']['mailing_wachttijdinoutbox']);

	$options = sprintf("<option value=-1%s>Webmasters</option>\n", checked($_SESSION['settings']['mailing_alle_zien'], "option", -1));
	
	foreach ((new cls_Onderdeel())->lijst(1) as $row) {
		$options .= sprintf("<option value=%d%s>%s</option>\n", $row->RecordID, checked($_SESSION['settings']['mailing_alle_zien'], "option", $row->RecordID), $row->Naam);
	}
	printf("<label class='form-label'>Wie mogen alle mailings zien en muteren?</label>\n<select name='mailing_alle_zien' class='form-select form-select-sm'>%s</select>\n", $options);

	printf("<label class='form-label'>Hoeveel mails mogen er per minuut verstuurd worden</label><input type='number' class='num5' name='maxmailsperminuut' value=%d min=1 max=9999>\n", $_SESSION['settings']['maxmailsperminuut']);
	printf("<label class='form-label'>Hoeveel mails mogen er per uur verstuurd worden</label><input type='number' class='num5' name='maxmailsperuur' value=%d min=1 max=9999>\n", $_SESSION['settings']['maxmailsperuur']);
	printf("<label class='form-label'>Hoeveel mails mogen er per 24 uur verstuurd worden</label><input type='number' class='num5' name='maxmailsperdag' value=%d min=1 max=9999>\n", $_SESSION['settings']['maxmailsperdag']);
	$i_mh = new cls_Mailing_hist();
	printf("<label class='form-label'>Aantal verstuurde mails in de afgelopen uur / 24 uur:</label><p>%d / %d</p>\n", $i_mh->aantalverzonden(2), $i_mh->aantalverzonden(3));
	$i_mh = null;
	printf("<label class='form-label'>Waar worden de bijlagen bewaard?</label><input type='text' name='path_attachments' value='%s'>\n", $_SESSION['settings']['path_attachments']);
	printf("<label class='form-label'>Welke extensies zijn als bijlage toegestaan: (leeg = standaard lijst)</label><input type='text' name='mailing_extensies_toegestaan' value='%s'>\n", $_SESSION['settings']['mailing_extensies_toegestaan']);
	printf("<label class='form-label'>Wat is de maximale groootte van één bijlage?</label><input type='number' class='num5' name='max_grootte_bijlage' value=%d><p>KB (nul = 2MB)</p>\n", $_SESSION['settings']['max_grootte_bijlage']);
	
	printf("<label class='form-label'>Wat is de API-key voor TinyMCE?</label><input type='text' name='mailing_tinymce_apikey' value='%s'>\n", $_SESSION['settings']['mailing_tinymce_apikey']);
		
	echo("<h2>Retentie / Opschonen</h2>\n");
	printf("<label class='form-label'>Hoe lang moeten mailings in de prullenbak bewaard blijven?</label><input type='number' class='num3' name='mailing_bewaartijd' value=%d min=1 max=999><p>maanden</p>\n", $_SESSION['settings']['mailing_bewaartijd']);
	printf("<label class='form-label'>Hoe lang moeten verzonden e-mails bewaard blijven?</label><input type='number' class='num3' name='mailing_verzonden_opschonen' value=%d min=6 max=999><p>maanden</p>\n", $_SESSION['settings']['mailing_verzonden_opschonen']);
	printf("<label class='form-label'>Hoe lang moeten ontvangers bij een mailing worden bewaard?</label><input type='number' class='num3' name='mailing_bewaartijd_ontvangers' value=%d min=3 max=999><p>maanden</p>\n", $_SESSION['settings']['mailing_bewaartijd_ontvangers']);
	printf("<label class='form-label'>Bewaartermijn verzonden e-mails na beïndiging lidmaatschap</label><input type='number' class='num3' name='mailing_hist_opschonen' value=%d min=3 max=999><p>maanden</p>\n", $_SESSION['settings']['mailing_hist_opschonen']);
	
	echo("<h2>Mailings met een specifiek doel</h2>\n");
	
	$rows = $i_m->lijst("Templates");

	$options = "";
	foreach($rows as $row) {
		$options .= sprintf("<option%s value=%d>%s</option>\n", checked($row->RecordID, "option", $_SESSION['settings']['mailing_lidnr']), $row->RecordID, $row->subject);
	}
	printf("<label class='form-label'>Versturen lidnummer</label><select name='mailing_lidnr' class='form-select form-select-sm'>\n<Option value=0>Geen</option>\n%s</select>\n", $options);
	
	$options = "";
	foreach($rows as $row) {
		$options .= sprintf("<Option%s value=%d>%s</option>\n", checked($row->RecordID, "option", $_SESSION['settings']['mailing_validatielogin']), $row->RecordID, $row->subject);
	}
	printf("<label class='form-label'>Versturen validatie e-mail login</label><select name='mailing_validatielogin' class='form-select form-select-sm'><Option value=0>Geen</option>\n%s</select>\n", $options);
	
	$options = "";
	foreach($rows as $row) {
		$options .= sprintf("<Option%s value=%d>%s</option>", checked($row->RecordID, "option", $_SESSION['settings']['mailing_herstellenwachtwoord']), $row->RecordID, $row->subject);
	}
	printf("<label class='form-label'>Versturen link herstellen wachtwoord</label><select name='mailing_herstellenwachtwoord' class='form-select form-select-sm'><Option value=0>Geen</option>\n%s</select>\n", $options);

	$options = "<Option value=0>Geen</option>\n";
	foreach($rows as $row) {
		$options .= sprintf("<Option%s value=%d>%s</option>\n", checked($row->RecordID, "option", $_SESSION['settings']['mailing_bevestigingopzegging']), $row->RecordID, $row->subject);
	}
	printf("<label class='form-label'>Versturen bevestiging opzegging lidmaatschap</label><select name='mailing_bevestigingopzegging' class='form-select form-select-sm'>%s</select>\n", $options);
	
	$options = "<Option value=0>Geen</option>";
	foreach($rows as $row) {
		$options .= sprintf("<Option%s value=%d>%s</option>", checked($row->RecordID, "option", $_SESSION['settings']['mailing_bevestigingdeelnameevenement']), $row->RecordID, $row->subject);
	}
	printf("<label class='form-label'>Versturen bevestiging inschrijven bij evenement</label><select name='mailing_bevestigingdeelnameevenement' class='form-select form-select-sm'>%s</select>\n", $options);
	
	
	$options = "<Option value=0>Geen</option>\n";
	foreach($rows as $row) {
		$options .= sprintf("<Option%s value=%d>%s</option>\n", checked($row->RecordID, "option", $_SESSION['settings']['mailing_bevestigingbestelling']), $row->RecordID, $row->subject);
	}
	$rows = null;
	printf("<label class='form-label'>Versturen bevestiging bestelling</label><select name='mailing_bevestigingbestelling' class='form-select form-select-sm'>%s</select>\n", $options);
	
	$rows = null;
	echo("<div class='clear'></div>\n");
	
	$kols = null;
	$kols[] = array('headertext' => "#", 'type' => "pk", 'readonly' => true);
	$kols[] = array('headertext' => "Vanaf e-mail", 'columnname' => "Vanaf_email", 'type' => "email");
	$kols[] = array('headertext' => "Vanaf naam", 'columnname' => "Vanaf_naam");
	$kols[] = array('headertext' => "Ingevoerd", 'columnname' => "Ingevoerd", 'type' => "date", 'readonly' => true);
	$kols[] = array('headertext' => "Gewijzigd", 'columnname' => "Gewijzigd", 'type' => "datetime", 'readonly' => true, 'type' => "datetime");
	
	$l = sprintf("%s?tp=%s/%s&delete_vanaf=%%d", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
	$kols[] = array('headertext' => "&nbsp;", 'link' => $l, 'columnname' => "RecordID", 'columntitle' => "Verwijder record", 'class' => "trash");
	
	$rows = $i_mv->lijst(0);
	echo(fnEditTable($rows, $kols, "editmailingvanaf", "Vanaf adressen"));
	
	echo("<div id='opdrachtknoppen'>\n");
	printf("<button type='submit' name='vanaf_adres_toevoegen' class='%s'>%s Vanaf adres toevoegen</button>", CLASSBUTTON, ICONTOEVOEGEN);
	
	printf("<button type='submit' name='InstellingenBewaren' class='%s'>%s Bewaren</button>\n", CLASSBUTTON, ICONBEWAAR);
	echo("</div> <!-- Einde opdrachtknoppen -->\n");
	echo("</form>");
	
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
			$mail->AddAddress($e, $row->AanNaam);
		}
	}
	foreach (explode(",", $row->cc_addr) as $e) {
		$e = trim($e);
		if (isValidMailAddress($e)) {
			$mail->AddCC($e);
		}
	}
	
	if ($row->VanafID > 0) {
		$i_mv = new cls_Mailing_vanaf($row->VanafID);
		$mail->From = $i_mv->vanaf_email;
		$mail->FromName = $i_mv->vanaf_naam;
		$i_mv = null;
	} else {
		$mail->From = $row->from_addr;
		$mail->FromName = $row->from_name;
	}
	
	if ($row->ReplyID > 0 and $row->ReplyID != $row->VanafID) {
		$i_mv = new cls_Mailing_vanaf($row->ReplyID);
		$mail->addReplyTo($i_mv->vanaf_email, $i_mv->vanaf_naam);
		$i_mv = null;
	}
	
	if ($row->send_on > "2000-01-01" and strpos($row->subject, "opnieuw verzonden") == false) {
		$mail->Subject = $row->subject . " (opnieuw verzonden)";
	} else {
		$mail->Subject = $row->subject;
	}
	$mail->Body = $row->message;
	
	if ($mail->addstationary($row->AanNaam, "", 0, $row->ZonderBriefpapier)) {

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
		} elseif ($_SERVER['HTTP_HOST'] != "phprbm.telling.nl") {
			$mess = sprintf("Versturen van e-mail %d (%s) aan %s is mislukt. Foutboodschap: %s", $p_mhid, $mail->Subject, $row->to_addr, $error);
		}
	}
	
	if (strlen($mess) > 0) {
		(new cls_Logboek())->add($mess, 4, $row->LidID, 1, $p_mhid, 25);
	}

	$mail = null;
	
	return $rv;
	
} # sentfromhist

function sentoutbox($p_mode) {
	
	/*
	Uitleg p_mode:
		1 = versturen middels knop door gebruiker
		2 = via batchjob of na uitloggen
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
			if ($i_mh->aantalverzonden(1) < $_SESSION['settings']['maxmailsperminuut']) {
				if (sentfromhist($mhrow->RecordID)) {
					$aantverzonden++;
				}
			}
		}
	
		if ($aantverzonden > 0) {
			$aob = $i_mh->aantaloutbox();
			if ($p_mode == 2) {
				$mess = "sentoutbox is gestart door de batchjob";
			} else {
				$mess = sprintf("Versturen vanuit de outbox (mode: %d) is gereed", $p_mode);
			}
			$mess .= sprintf(", er zijn %d e-mails verzonden. Er staan nog %d e-mails te wachten in de outbox.", $aantverzonden, $aob);
		}
	}
	
	if (strlen($mess) > 0) {
		(new cls_Logboek())->add($mess, 4, 0, 0, 0, 50);
	}

	return $aantverzonden;
	
}  # sentoutbox

class RBMmailer extends PHPMailer\PHPMailer\PHPMailer {
	// Regelt het feitelijk versturen van de e-mail

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
		} elseif ( function_exists("mail") ) {
			$this->IsMail(true);
		}
		$this->CharSet = "UTF-8";
		$this->From = (new cls_Mailing_vanaf())->default_vanaf();
		$this->FromName = $_SESSION['settings']['naamvereniging'];
		$this->IsHTML(true);
		$this->WordWrap = 110;
	}
	
	public function addstationary($to="", $from="", $verwijderaanhef=0, $p_zb=-1) {
		$i_tp = new cls_Template(-1, "briefpapier");

		if ($p_zb >= 0) {
			$this->zonderbriefpapier = $p_zb;
		}
			
		if ($this->zonderbriefpapier == 1) {
			if (startwith($this->Body, "<!DOCTYPE html>") == false) {
				$this->Body = "<!DOCTYPE html>\n" . $this->Body;
			}
			return true;
			
		} elseif (strlen($i_tp->inhoud) > 0) {
//			debug($this->bestand_briefpapier, 0, 1);
			if ($verwijderaanhef == 1) {
				$htmlmessage = removetextblock($i_tp->inhoud, "<!-- Aanhef -->", "<!-- /Aanhef -->");
			} else {
				$htmlmessage = $i_tp->inhoud;
			}
			$htmlmessage = str_ireplace("[%MESSAGE%]", $this->Body, $htmlmessage);
			$htmlmessage = str_ireplace("[%FROM%]", $this->FromName, $htmlmessage);
			$htmlmessage = str_ireplace("[%TO%]", $to, $htmlmessage);
			$htmlmessage = str_ireplace("[%SUBJECT%]", $this->Subject, $htmlmessage);
			$this->Body = $htmlmessage;
			return true;
			
		} else {
			$mess = "De template 'briefpapier' is niet ingevuld.";
			(new cls_Logboek())->add($mess, 4, 0, 1);
			return false;
		}
	}  # RBMmailer->addstationary
	
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
	}  # RBMmailer->Send
	
} # RBM_mailer

function eigennotificatie($p_ondid, $p_aanadres, $p_tas=-1, $p_interval=48, $p_cc="", $p_onderwerp="", $p_vanafid=-1) {
	
	if ($p_tas < 0) {
		$p_tas = $p_ondid;
	}
	
	if ($p_interval < 4) {
		$p_interval = 4;
	}
	
	$i_ond = new cls_Onderdeel($p_ondid);
	$i_lo = new cls_Lidond(-1, -1, $p_ondid);
	$i_lo->autogroepenbijwerken(0, 10, $p_ondid);
	$i_el = new cls_Eigen_lijst("", $p_ondid);
	$i_lb = new cls_Logboek();
	$i_mh = new cls_mailing_hist();
	$i_email = new email();
	
	if ($p_vanafid > 0) {
		$i_email->vanafid = $p_vanafid;
	}
	
	$ar = 0;
	$ng = "";
	$res = "";
	
	$f = sprintf("MH.Xtra_Char='NOTIF' AND MH.Xtra_Num=%d AND LOWER(MH.to_addr)=LOWER('%s')", $p_ondid, $p_aanadres);
	$laatste = $i_mh->max("IFNULL(MH.send_on, MH.Ingevoerd)", $f);
	if ($laatste < date("Y-m-d H:i:s", strtotime(sprintf("-%d hour", $p_interval)))) {
		if ($i_ond->oid > 0) {
			$ng = $i_ond->naam;
			$query = sprintf("SELECT DISTINCT L.RecordID AS LidID, %1\$s AS Naam
							  FROM %2\$sLidond AS LO INNER JOIN %2\$sLid AS L ON LO.Lid=L.RecordID
							  WHERE LO.Vanaf <= CURDATE() AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE() AND LO.OnderdeelID=%3\$d;", $i_ond->selectnaam, TABLE_PREFIX, $i_ond->oid);
			$rows = $i_ond->execsql($query)->fetchAll();
			if ($rows !== false) {
				$ar = count($rows);
				$res = fnDisplayTable($rows);
			}
		} elseif ($i_el->elid > 0) {
			if ($i_el->aantalrijen > 0) {
				$i_el->controle($i_el->elid);
			}
			$ng = $i_el->naam;
			$ar = $i_el->aantalrijen;
			
			if ($i_el->aantalrijen > 0) {
				$rows = $i_el->rowset();
				if ($rows !== false) {
					$res = fnDisplayTable($rows);
				}
				$res .= sprintf("<p>%s</p>\n", $i_el->uitleg);
			}
		}
		if ($ar > 0 and isValidMailAddress($p_aanadres, 0)) {
			$i_email->aanadres = $p_aanadres;
			if (isValidMailAddress($p_cc, 0)) {
				$i_email->cc = $p_cc;
			} elseif (strlen($p_cc) > 10) {
				$p_cc = str_replace(";", ",", $p_cc);
				if (strpos($p_cc, ",") !== false) {
					$i_email->cc = $p_cc;	
				}
			}
			if (strlen($p_onderwerp) > 0) {
				$i_email->onderwerp = $p_onderwerp;
			} elseif ($ar > 1) {
				$i_email->onderwerp = sprintf("%s (%d rijen)", $ng, $ar);
			} else {
				$i_email->onderwerp = $ng;
			}
			$i_email->bericht = "<!DOCTYPE html>
									<html lang='nl'>
									<head>\n";
			$i_email->bericht .= sprintf("<title>%s</title>\n", $i_email->onderwerp);
			if (file_exists(BASEDIR . "/maatwerk/email.css")) {
				$i_email->bericht .= "<link rel='stylesheet' href='" . BASISURL . "/maatwerk/email.css'>\n";
			}
			$i_email->bericht .= "</head>\n<body>\n";
										
			$i_email->bericht .= $res;
			$i_email->bericht .= "</body>\n</html>\n";
			$i_email->zonderbriefpapier = 1;
			$i_email->xtrachar = "NOTIF";
			$i_email->xtranum = $p_ondid;
			if ($i_email->to_outbox() > 0) {
				$mess = sprintf("Notificatie %s in de outbox geplaatst", $ng);
				$i_lb->add($mess, 4, 0, 0, 0, 21);
			}
		}
	}
	
	$i_ond = null;
	$i_lo = null;
	$i_el = null;
	$i_lb = null;
	$i_email = null;
	
}  # eigennotificatie

function js_editor($p_hist=0) {
	
	if (file_exists(BASEDIR . '/maatwerk/email.css')) {
		$stylesheet = "content_css: 'maatwerk/email.css',";
	} else {
		$stylesheet = "";
	}
	printf("<script>
		tinymce.init({
			selector: '#message',
			placeholder: 'Bericht hier ...',
			theme: 'silver',
			mobile: { theme: 'silver' },
			menubar: true,
			height: 650,
			relative_urls: false,
			convert_urls: false,
			remove_script_host : false,
			plugins: 'link lists table image importcss',
			paste_as_text: true,
			%s
			importcss_append: true,
			menu: {
				table: { title: 'Table', items: 'inserttable | cell row column | tableprops deletetable' }
			},
			toolbar: 'undo redo | styleselect | bold italic | alignleft aligncenter alignright alignjustify | bullist numlist | link image | removeformat',
			menubar: 'edit insert format tools table',
			
			setup: function(editor) {
				editor.on('blur', function(e) {
					mailing_savemessage(%d);
				});
			}
		});
  </script>\n", $stylesheet, $p_hist);
}  # js_editor

?>
