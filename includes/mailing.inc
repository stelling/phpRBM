<?php

function fnMailing() {
	global $ldl, $currenttab2;
		
	if (isset($_POST['Upload']) and $_POST['Upload'] == "Upload") {
		$op = "upload";
	} elseif (isset($_POST['action']) and $_POST['action'] == "Bewaren & sluiten") {
		$op = "save_close";
	} elseif (isset($_POST['action']) and $_POST['action'] == "Voorbeeld") {
		$op = "preview";
	} elseif (isset($_GET['op'])) {
		$op = $_GET['op'];
	} elseif ($_GET['tp'] == "Mailing/Historie") {
		$op = "historie";
	} else {
		$op = "";
	}

	if (isset($_GET['mid']) and $_GET['mid'] > 0) {
		$mailing = new mailing($_GET['mid']);
	} else {
		$mailing = new mailing();
	}
	
	if (($op == "historie" or $op == "histdetails") and $mailing->mid > 0) {
		fnDispMenu(2, sprintf("mid=%d", $mailing->mid));
	} elseif ($op != "preview") {
		fnDispMenu(2);
	}

	if ($op == "edit" and isset($_GET['mid'])) {
		$mailing->edit();
	} elseif ($op == "post" or $op == "save_close") {
		$mailing->post_form();
		$mailing->upload();
		if ($op == "save_close") {
			printf("<script>location.href='%s?tp=Mailing/%s';</script>\n", $_SERVER['PHP_SELF'], db_mailing("folder", $mailing->mid));
		} else {
			$mailing->edit();
		}
	} elseif ($currenttab2 == "Nieuw" and toegang($_GET['tp'])) {
		$mailing->mid = 0;
		$mailing->edit();
	} elseif ($currenttab2 == "Logboek" and toegang($_GET['tp'])) {
		$lijst = db_logboek("lijst", "", 4);
		echo(fnDisplayTable($lijst, "", "", 0, "", "", "logboek"));
	} elseif ($op == "historie" and $mailing->mid > 0) {
		$rows = db_mailing("hist", $mailing->mid);
		if (count($rows) > 0) {
			$ld = sprintf("<a href='index.php?tp=%s&amp;op=histdetails&amp;rid=%%d&amp;mid=%d'>%%s</a>", $_GET['tp'], $mailing->mid);
			echo(fnDisplayTable($rows, $ld, $rows[0]->ndOnderwerp, 0, 1));
		} else {
			echo("<p class='mededeling'>Deze mailing heeft geen historie.</p>\n");
		}
	} elseif ($op == "histdetails" and $_GET['rid'] > 0) {
		$mailing->toonverstuurdemail($_GET['rid']);
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
	} elseif ($op == "add_groep" and isset($_GET['groepid']) and $_GET['groepid'] > 0) {
		$mailing->add_groep($_GET['groepid']);
		$mailing->edit();
	} elseif ($op == "del_groep" and isset($_GET['groepid']) and $_GET['groepid'] != 0) {
		$mailing->delete_groep($_GET['groepid']);
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
}

class mailing {
	public $mid;
	
	public $allowed_ext = "bmp, gif, jpeg, jpg, pdf, png, pps, rar, txt, zip";
	public $dir_attachm = "/attachments";
	public $max_size_attachm = 2097152; // 2MB
	public $template_bestand = "";
	public $bestand_briefpapier = "templates/briefpapier.html";
	public $vertraging_tussen_verzenden = 120; // in seconden
	
	private $MergeField;
	private $contains_mergefield = false;
	private $contains_losse_email = false;
	private $mailbericht;
	private $merged_message = "";
	
	private $from_name = "";
	private $from_addr = "";
	private $to_name = "";
	private $cc_addr = "";
	private $subject = "";
	private $message = "";
	private $template = 0;
	private $confidential = 0;
	private $verzamelen = 0;
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
	private $resultaatversturen = 1;

	function __construct($mid=0) {
		global $selectnaam, $table_prefix, $fdlang, $fdkort, $max_grootte_bijlage, $resultaatmailingversturen, $arrGeslacht;

		$this->mid = $mid;
	
		$WhereLidond = "WHERE LO.Vanaf <= CURDATE() AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd >= CURDATE())";
		$WhereLidmaatschap = "WHERE LM.LIDDATUM <= CURDATE() AND ((LM.Opgezegd IS NULL) OR LM.Opgezegd >= CURDATE())";
		$WhereLiddipl = "WHERE ((LD.LicentieVervallenPer IS NULL) OR LD.LicentieVervallenPer >= CURDATE())";
		
		$this->dir_attachm = substr($_SERVER["SCRIPT_FILENAME"], 0, strrpos($_SERVER["SCRIPT_FILENAME"], "/")) . "/attachments";
		
		if (isset($max_grootte_bijlage) and $max_grootte_bijlage > 0) {
			$max_size_attachm = $max_grootte_bijlage;
		}
		if (isset($resultaatmailingversturen) and $resultaatmailingversturen == 1) {
			$this->resultaatversturen = 1;
		} else {
			$this->resultaatversturen = 0;
		}

		$sql = sprintf("SELECT Roepnaam FROM %sLid AS L WHERE %s;", $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "Roepnaam", 'SQL' => $sql);

		$sql = sprintf("SELECT Voorletter FROM %sLid AS L WHERE %s;", $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "Voorletters", 'SQL' => $sql);

		$sql = sprintf("SELECT (CASE ISNULL(L.Tussenv) WHEN 0 THEN CONCAT(L.Tussenv, ' ', L.Achternaam)"
			  . " ELSE L.Achternaam END) AS Achternaam FROM %sLid AS L WHERE %s;", $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "Achternaam", 'SQL' => $sql);
		
		$sql = "SELECT CASE L.Geslacht";
		foreach ($arrGeslacht as $key => $value) {
			$sql .= sprintf(" WHEN '%s' THEN '%s'", $key, $value);
		}
		$sql .= sprintf(" END FROM %sLid AS L WHERE %s", $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "Geslacht", 'SQL' => $sql);

		$sql = sprintf("SELECT L.Adres FROM %sLid AS L WHERE %s;", $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "Adres", 'SQL' => $sql);

		$sql = sprintf("SELECT L.Postcode FROM %sLid AS L WHERE %s;", $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "Postcode", 'SQL' => $sql);

		$sql = sprintf("SELECT L.Woonplaats FROM %sLid AS L WHERE %s;", $table_prefix, "L.Nummer=%d");;
		$this->MergeField[]=array('Naam' => "Woonplaats", 'SQL' => $sql);
							 
		$sql = sprintf("SELECT %s AS Lidnaam FROM %sLid AS L WHERE %s;", $selectnaam, $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "Lidnaam", 'SQL' => $sql);

		$sql = sprintf("SELECT LM.Lidnr FROM %sLidmaatschap AS LM %s AND %s;", $table_prefix, $WhereLidmaatschap, "LM.Lid=%d");
		$this->MergeField[]=array('Naam' => "Lidnummer", 'SQL' => $sql);
		
		$sql = sprintf("SELECT FLOOR(DATEDIFF(IF(ISNULL(LM.Opgezegd), CURDATE(), LM.Opgezegd), LM.LIDDATUM)/365.25)
							FROM %sLidmaatschap AS LM WHERE LM.LIDDATUM < CURDATE() AND %s
							ORDER BY LM.LIDDATUM DESC LIMIT 1;", $table_prefix, "LM.Lid=%d");
		$this->MergeField[]=array('Naam' => "LengteLidmaatschap", 'SQL' => $sql);

		$sql = sprintf("SELECT L.Telefoon FROM %sLid AS L WHERE %s;", $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "Telefoon", 'SQL' => $sql);

		$sql = sprintf("SELECT L.Mobiel FROM %sLid AS L WHERE %s;", $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "Mobiel", 'SQL' => $sql);

		$sql = sprintf("SELECT DATE_FORMAT(L.GEBDATUM, %s) AS Geboortedatum FROM %sLid AS L WHERE %s;", $fdlang, $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "Geboortedatum", 'SQL' => $sql);

		$sql = sprintf("SELECT L.GEBPLAATS FROM %sLid AS L WHERE %s;", $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "Geboorteplaats", 'SQL' => $sql);

		$sql = sprintf('SELECT O.Naam FROM %1$sLidond AS LO INNER JOIN %1$sOnderdl AS O ON O.RecordID = LO.OnderdeelID'
			  . ' %2$s AND O.GekoppeldAanQuery=0 AND %3$s ORDER BY O.Naam;', $table_prefix, $WhereLidond, "LO.Lid=%d");
		$this->MergeField[]=array('Naam' => "Onderdelen", 'SQL' => $sql);
	
		if ($_SESSION['aantalbewaking'] > 0) {
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
							  
		$sql = sprintf("SELECT L.BANKGIRO FROM %sLid AS L WHERE %s;", $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "Bankrekening", 'SQL' => $sql);

		if ($_SESSION['aantalrekeningen'] > 0) {
			$sql = sprintf("SELECT RK.Nummer FROM %sRekening AS RK WHERE RK.Bedrag > 0 AND RK.Bedrag > RK.Betaald AND %s;", $table_prefix, "RK.Lid=%d");
			$this->MergeField[]=array('Naam' => "OpenstaandeRekeningen", 'SQL' => $sql);

			$sql = sprintf("SELECT ROUND(SUM(RK.Bedrag-RK.Betaald), 2) AS OpenstaandBedrag FROM %sRekening AS RK WHERE %s;", $table_prefix, "RK.Lid=%d");
			$this->MergeField[]=array('Naam' => "OpenstaandBedrag", 'SQL' => $sql);
		}

		$sql = sprintf("SELECT (CASE L.`Machtiging afgegeven` WHEN 1 THEN 'Ja' ELSE 'Nee' END) AS MachtigingAfgegeven"
											  . " FROM %sLid AS L WHERE %s;", $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "MachtigingAfgegeven", 'SQL' => $sql);
		
		$sql = sprintf('SELECT DISTINCT DP.Kode FROM %1$sLiddipl AS LD INNER JOIN %1$sDiploma AS DP ON LD.DiplomaID = DP.RecordID'
			  . ' %2$s AND %3$s ORDER BY DP.Volgnr, LD.EXDATUM, DP.KODE;', $table_prefix, $WhereLiddipl, "LD.Lid=%d");
		$this->MergeField[]=array('Naam' => "DiplomaKort", 'SQL' => $sql);

		$sql = sprintf('SELECT DISTINCT DP.Naam FROM %1$sLiddipl AS LD INNER JOIN %1$sDiploma AS DP ON LD.DiplomaID = DP.RecordID'
			  . ' %2$s AND %3$s ORDER BY DP.Volgnr, LD.EXDATUM, DP.Naam;', $table_prefix, $WhereLiddipl, "LD.Lid=%d");
		$this->MergeField[]=array('Naam' => "DiplomaLang", 'SQL' => $sql);
	
		$sql = sprintf("SELECT L.RelnrRedNed FROM %sLid AS L WHERE %s;", $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "RelnrRedNed", 'SQL' => $sql);

		$sql = sprintf("SELECT DATE_FORMAT(LIDDATUM, %s) FROM %sLidmaatschap AS LM %s AND %s;", $fdlang, $table_prefix, $WhereLidmaatschap, "LM.Lid=%d");
		$this->MergeField[]=array('Naam' => "LidVanaf", 'SQL' => $sql);

		$sql = sprintf("SELECT IF(LM.OPGEZEGD IS NULL, 'Niet', DATE_FORMAT(LM.Opgezegd, %s)) FROM %sLidmaatschap AS LM
							 WHERE LM.LIDDATUM < CURDATE() AND %s
							 ORDER BY LM.LIDDATUM DESC LIMIT 1;", $fdlang, $table_prefix, "LM.Lid=%d");
		$this->MergeField[]=array('Naam' => "OpgezegdPer", 'SQL' => $sql);

		$sql = sprintf("SELECT L.EMAIL FROM %sLid AS L WHERE %s;", $table_prefix, "Nummer=%d");
		$this->MergeField[]=array('Naam' => "Email", 'SQL' => $sql);

		if (TableExists("LidRedNed")) {
			$sql = sprintf("SELECT RN.Foutboodschap FROM %1\$sLidRedNed AS RN JOIN %1\$sLidmaatschap AS LM ON LM.LIDNR = RN.Lidnr WHERE %2\$s;", $table_prefix, "LM.LID=%d");
			$this->MergeField[]=array('Naam' => "VerschilSportlink", 'SQL' => $sql);
		}
		
		if ($_SESSION['aantalrollen'] > 0) {
			$sql = sprintf("SELECT O.Naam FROM %1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON O.RecordID = LO.OnderdeelID "
				  . "%2\$s AND `Type`='R' AND %3\$s ORDER BY O.Naam;", $table_prefix, $WhereLidond, "LO.Lid=%d");
			$this->MergeField[]=array('Naam' => "Rollen", 'SQL' => $sql);
		}
			 
		sort($this->MergeField);
		
		$this->vulvars();
	}
	
	public function lijst($filter="") {
		global $bewaartijdmailings;
	
		$lnk = sprintf("<a href='%s?tp=%s&amp;op=edit&amp;mid=%s' title='Bewerk mailing'><img src='images/zoom.png'></a>", $_SERVER['PHP_SELF'], $_GET['tp'], "%d");
		$lnk_lk = sprintf("<a href='%s?tp=Mailing/Historie&amp;op=historie&amp;mid=%s' title='Bekijk historie'><img src='images/object_10.png'></a>", $_SERVER['PHP_SELF'], "%d");
		echo(fnDisplayTable(db_mailing("lijst", 0, 0, $filter), $lnk, "", 0, $lnk_lk));
		if ($filter == "Prullenbak") {
			printf("<p>Mailings die langer dan %d maanden geleden verwijderd zijn, worden automatisch definitief opgeschoond.</p>\n", $bewaartijdmailings);
		}
	}
	
	public function toonverstuurdemail($rid) {
		global $navpad;
		
		$navpad[]['naam'] = sprintf("Verzonden mail %d", $rid);
	
		echo("<div id='verstuurdemail'>\n");
		$row = db_mailing("histdetails", 0, $rid);
		printf("<div class='label'>Verzonden:</div><div class='veld'>%s</div>\n", strftime('%e %B %Y (%H:%M)', strtotime($row->send_on)));
		if (strlen($row->from_name) > 0) {
			printf("<div class='label'>Van:</div><div class='veld'>%s (%s)</div>\n", htmlentities($row->from_name), $row->from_addr);
		} else {
			printf("<div class='label'>Van:</div><div class='veld'>%s</div>\n", $row->from_addr);
		}
		if (strlen($row->to_name) > 0) {
			printf("<div class='label'>Aan:</div><div class='veld'>%s</div>\n", htmlentities($row->to_name));
		}
		printf("<div class='label'>E-mail:</div><div class='veld'>%s</div>\n", $row->to_addr);
		if (strlen($row->cc_addr) > 5) {
			printf("<div class='label'>CC:</div><div class='veld'>%s</div>\n", $row->cc_addr);
		}
		printf("<div class='label'>Onderwerp:</div><div class='veld'>%s</div>\n", $row->subject);
		printf("<div class='label'>Bericht:</div><div class='veld'>%s</div>\n", $row->message);
		echo("</div>  <!-- Einde verstuurdemail -->\n");
	}
	
	public function edit() {
		global $currenttab, $currenttab2, $beperkfrom, $naamvereniging, $navpad;
		
		$this->vulvars();

		$ad = $this->dir_attachm . "/" . $this->mid;
		
		if ($this->mid == 0) {
			$currenttab2 = "Concepten";
		} else {
			$navpad[]['naam'] = sprintf("Details mailing %d", $this->mid);
		}
		printf("<div id='mailing'>\n");
		printf("<form method='post' action='%s?tp=%s/%s&amp;op=post&amp;mid=%d' name='frm_mailedit' enctype='multipart/form-data'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2, $this->mid);
		printf("<table>\n");
		printf("<tr><td class='label'>Van:</td><td><input type='text' name='from_name' value='%s' size=50 maxlength=50></td></tr>\n", $this->from_name);
		printf("<tr><td class='label'>Vanaf e-mail:</td><td><input type='email' name='from_addr' value='%s' size=50 maxlength=50></td></tr>\n", $this->from_addr);
		printf("<tr><td class='label'>Aan:</td><td><input type='text' name='to_name' value='%s' size=50 maxlength=50></td></tr>\n", $this->to_name);
		
		$meldingen = "Er zijn nog geen ontvangers aan deze mailing toegevoegd.<br>\n";
		$verzendenmag = false;
		if ($this->mid > 0) {
			echo("<tr><td class='label'>Ontvangers met e-mail:");
			echo("<td><ul class='listrecipients'>\n");
			
			$rcpt_rows = db_mailing("rcpt", $this->mid);
			$aant_ze = 0;
			$text_ze = "";
			if (count($rcpt_rows) > 0) {
				foreach($rcpt_rows as $rcpt) {
					if ($rcpt->Nummer > 0) {
						$lnk = sprintf("%s?tp=%s&op=del_rcpt&mid=%d&lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, $rcpt->Nummer);
						$reg = sprintf("<li>%s&nbsp;<a href='%s'><img src='/images/delete.gif' alt='Verwijderen' title='Verwijderen'></a></li>\n", htmlentities($rcpt->Naam_lid), $lnk);
					} else {
						$lnk = sprintf("%s?tp=%s&op=del_rcpt&mid=%d&addr=%s", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, $rcpt->to_address);
						$reg = sprintf("<li>%s&nbsp;<a href='%s'><img src='/images/delete.gif' alt='Verwijderen' title='Verwijderen'></a></li>\n", $rcpt->to_address, $lnk);
						$this->contains_losse_email = true;
					}
					if (isValidMailAddress($rcpt->EMAIL) or isValidMailAddress($rcpt->EmailVereniging) or isValidMailAddress($rcpt->to_address)) {
						echo($reg);
					} else {
						$aant_ze++;
						$text_ze .= $reg;
					}
				}
				echo("</ul>\n");
				if (count($rcpt_rows)-$aant_ze > 1) {
					echo("<div class='clear'></div>\n");
					printf("Totaal %d ontvangers\n", count($rcpt_rows)-$aant_ze);
				}
				if ($aant_ze > 0) {
					echo("<tr><td class='label'>Ontvangers zonder e-mail:");
					echo("<td><ul class='listrecipients'>\n");
					echo($text_ze);
					echo("</ul>\n");
					echo("<div class='clear'></div>\n");
					printf("Totaal %d ontvangers\n", $aant_ze);
				}
				$verzendenmag = true;
				$meldingen = "";
			} else {
				echo("<p>Nog niemand!</p>");
			}
			echo("</td></tr>\n");
			
			echo("<tr><td class='label'>Ontvanger toevoegen:</td>\n");
			printf("<td>Lid: <select name='add_lid' id='add_lid'>\n");
			printf("<option value=0>&nbsp;</option>\n");
			$rcpt_rows = db_mailing("potrcpt", $this->mid);
			foreach($rcpt_rows as $rcpt) {
				printf("<option value='%d'>%s</option>\n", $rcpt->Nummer, $rcpt->Zoeknaam_lid);
			}
			printf("</select>");
			$lnk = sprintf("%s?tp=%s&op=add_rcpt&mid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid);
			printf("&nbsp;<img src='/images/move.gif' alt='move' title='Lid toevoegen' onclick=\"elem=document.getElementById('add_lid'); val=elem.options[elem.selectedIndex].value; if (val!=0) { location.href='%s&lidid=' + val }\">&nbsp;\n", $lnk);
			echo("E-mail: <input type='email' name='add_address' id='add_address' size=50 maxlength=50>");
			printf("&nbsp;<img src='/images/move.gif' alt='move' title='E-mail toevoegen' onclick=\"elem=document.getElementById('add_address'); val=elem.value; location.href='%s&to_address=' + val\">&nbsp;\n", $lnk);
			echo("</td></tr>\n<tr>");
			
			echo("<td class='label'>Groep toevoegen of verwijderen:</td>");
			printf("<td><select name='sel_group_add' id='sel_group'>\n");
			printf("<option value=0>&nbsp;</option>\n");
			printf("<option value=-1>Iedereen</option>\n");
			printf("<option value=0>&nbsp;</option>\n");
			foreach(db_onderdelen() as $grp) {
				printf("<option value=%d>%s</option>\n", $grp->RecordID, $grp->Oms);
			}
			printf("</select>\n");
			$lnk = sprintf("%s?tp=%s&op=add_groep&mid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid);
			printf("<img src='/images/move.gif' alt='move' title='groep toevoegen' onclick=\"elem=document.getElementById('sel_group'); val=elem.options[elem.selectedIndex].value; if (val>0) { location.href='%s&groepid=' + val }\">&nbsp;\n", $lnk);
			$lnk = sprintf("%s?tp=%s&op=del_groep&mid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid);
			printf("<img src='/images/delete.gif' alt='kruis' title='groep verwijderen' onclick=\"elem=document.getElementById('sel_group'); val=elem.options[elem.selectedIndex].value; if (val!=0) { location.href='%s&groepid=' + val }\">&nbsp;\n", $lnk);
			printf("(groepen zijn t/m %s bijgewerkt)", strftime("%e %B %Y", strtotime(db_lidond("laatstgewijzigd"))));
			printf("</td></tr>\n");
		}
		printf("<tr><td class='label'>Cc:</td><td><input type='text' name='cc_addr' value='%s' size=100></td></tr>\n", $this->cc_addr);
		printf("<tr><td class='label'>Onderwerp:</td><td><input type='text' name='subject' value='%s' size=100 onBlur='this.form.submit();'></td></tr>\n", $this->subject);
		if ($this->template == 1) {
			$templ_chk = " checked";
		} else {
			$templ_chk = "";
		}
		if ($this->confidential == 1) {
			$conf_chk = " checked";
		} else {
			$conf_chk = "";
		}
		if ($this->verzamelen == 1) {
			$verz_chk = " checked";
		} else {
			$verz_chk = "";
		}
		
		printf("<tr><td class='label'>Opties:</td><td><input type='checkbox' name='template' value=1%s>Template&nbsp;", $templ_chk);
		printf("<input type='checkbox' name='confidential' value=1%s>Vertrouwelijk&nbsp;", $conf_chk);
		printf("<input type='checkbox' name='verzamelen' value=1%s>E&eacute;n gezamenlijke e-mail</td>", $verz_chk);
		echo("</tr>\n</table>\n");
		
		printf("<textarea name='message' onBlur='this.form.submit();'>%s</textarea>\n", $this->message);
		
		echo("<script>CKEDITOR.replace('message',{
        height : 450,
        uiColor : '#9AB8F3',
		  toolbar :
        [
			['Bold','Italic','Underline','Strike','-','Subscript','Superscript'],
			['Cut','Copy','Paste','PasteText','PasteFromWord','-','Print', 'SpellChecker', 'Scayt'],
			['Undo','Redo','-','Find','Replace','-','SelectAll','RemoveFormat'],
			['NumberedList','BulletedList','-','Outdent','Indent','Blockquote'],
			'/',
			['JustifyLeft','JustifyCenter','JustifyRight','JustifyBlock'],
			['Link','Unlink'],
			['Image','Table','HorizontalRule','Smiley','SpecialChar'],
			['Styles','Format','FontSize'],
			['TextColor'],
			['Maximize', 'ShowBlocks','-','About']
        ]
		});</script>\n");
	
		$strHV = "";
		foreach ($this->MergeField as $v) {
			$strHV .= "<li>[%" . $v['Naam'] . "%]</li>\n";
		}
		$strHV .= "";
		echo("<table>\n");
		printf("<tr><td class='label'>Beschikbare variabelen:</td>\n<td><ul class='listmergefields'>\n%s</ul></td>\n", $strHV);
		
		if ($this->mid > 0) {
			echo("<tr>\n<td class='label'>Bijlagen:</td>\n<td>\n");
			
			echo("<table>\n");
			if (is_dir($ad)) {
				$d = dir($ad);
				while (false !== ($entry = $d->read())) {
					if ($entry != "." and $entry != "..") {
						echo("<tr>\n<td>" . $entry . "</td>\n");
						$stat = stat($ad . "/" . $entry);
						echo("<td>" . number_format(($stat['size'] / 1024), 0, ',', '.') . " KB</td>\n");
						$lnk = sprintf("%s?tp=%s&op=del_attach&mid=%d&attach=%s", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, str_replace(".", "!", $entry));
						printf("<td><a href='%s'><img src='/images/delete.gif' alt='kruis' title='Verwijder'></a></td>\n", $lnk);
						echo("</tr>\n");
					}
				}
				$d->close();
			}
			echo("</table>\n");
			
			echo("<input type='file' name='UploadFile' id='UploadFile' size='40'>\n");
			echo("<input type='submit' name='Upload' value='Upload'>&nbsp;");
			printf("(Extensies '%s' zijn toegestaan)\n", $this->allowed_ext);
		}
			
		if ($this->mid > 0) {
			if (strlen($this->subject) == 0) {
				$meldingen .= "Het onderwerp is leeg en dat moet ingevuld zijn.<br>\n";
				$verzendenmag = false;
			} 
			if (strlen($this->message) == 0) {
				$meldingen .= "Het bericht is leeg en dat moet ingevuld zijn.<br>\n";
				$verzendenmag = false;
			}
			if (strtotime($this->sent_on) > (time() - $this->vertraging_tussen_verzenden)) {
				$meldingen .= sprintf("Deze mailing is korter dan %d minuten geleden verzonden.<br>\n", $this->vertraging_tussen_verzenden/60);
				$verzendenmag = false;
			}
			
			if (!file_exists($this->bestand_briefpapier)) {
				$meldingen .= sprintf("Het bestand %s bestaat niet. Neem hierover contact op met de beheerder.<br>\n", $this->bestand_briefpapier);
				$verzendenmag = false;
			}
			foreach($this->MergeField as $fld) {
				$nm = "[%" . $fld['Naam'] . "%]";
				if (strpos($this->message, $nm) !== false) {
					$this->contains_mergefield = true;
				}
			}
			if ($this->contains_mergefield == true and $this->contains_losse_email == true) {
				$meldingen .= "Er staan losse e-mailadressen en er wordt met mailmerge gewerkt. Bij de losse e-mailadressen werkt de mailmerge niet.<br>\n";
			}
			if ($this->contains_mergefield and $this->verzamelen) {
				$meldingen .= "Een mailing met variabelen kan niet als &eacute;&eacute;n gezamenlijke e-mail verzonden worden.<br>\n";
				$verzendenmag = false;
			}
			if ($this->new_on > '1901-01-01') {
				printf("<tr><td class='label'>Toegevoegd door/op:</td><td>%s / %s</td></tr>\n", db_naamlid($this->AddedBy), strftime("%e %B %Y (%H:%M)", strtotime($this->new_on)));
			}
			if ($this->changed_on > '1901-01-01' and $this->ChangedBy > 0) {
				printf("<tr><td class='label'>Gewijzigd door/op:</td><td>%s / %s</td></tr>\n", db_naamlid($this->ChangedBy), strftime("%e %B %Y (%H:%M)", strtotime($this->changed_on)));
			}
			if ($this->sent_on > '1901-01-01' and $this->SentBy > 0) {
				printf("<tr><td class='label'>Verzonden door/op:</td><td>%s / %s</td></tr>\n", db_naamlid($this->SentBy), strftime("%e %B %Y (%H:%M)", strtotime($this->sent_on)));
			}
			if ($this->deleted_on > '1901-01-01') {
				printf("<tr><td class='label'>Verwijderd door/op:</td><td>%s / %s</td></tr>\n", db_naamlid($this->DeletedBy), strftime("%e %B %Y (%H:%M)", strtotime($this->deleted_on)));
				$verzendenmag = false;
			}
			if (isValidMailAddress($this->from_addr, 0) == false) {
				$verzendenmag = false;
				$meldingen .= "Er moet een geldig e-mailadres bij 'Vanaf e-mail' ingevuld worden.<br>\n";
			} elseif (strlen($beperkfrom) > 2) {
				if (substr($beperkfrom, 0, 1) != "@") {
					$beperkfrom = "@" . $beperkfrom;
				}
				if (strpos($this->from_addr, $beperkfrom) === false) {
					$verzendenmag = false;
					$meldingen .= sprintf("Er mag alleen vanaf een e-mailadres uit het domein %s een mailing verzonden worden.<br>\n", substr($beperkfrom, 1));
				}
			}
			if ($verzendenmag == false) {
				$meldingen .= "<br>\nDeze mailing mag (nog) niet verzonden worden.\n";
			}
			if (strlen($meldingen) > 0) {
				printf("<tr><td class='label'>Meldingen:</td><td>%s</td></tr>\n", $meldingen);
			}
		} else {
			$verzendenmag = false;
		}
		echo("</table>\n");
		
		echo("<div id='opdrachtknoppen'>\n");
		if ($this->mid > 0) {
			printf("<input type='image' src='images/save.png' name='action' title='Wijzigingen bewaren' value='Wijzigingen bewaren'>\n");
			printf("<input type='submit' name='action' value='Bewaren & sluiten'>\n");
		} else {
			printf("<input type='submit' name='action' value='Toevoegen'>");
		}
			
		if ($this->mid > 0 and $verzendenmag and $this->deleted_on < "1901-01-01") {
			$lnk = sprintf("%s?tp=%s&op=send&mid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid);
			printf("<img src='images/sendmail.png' title='Verstuur mailing' onclick=\"location.href='%s'\">\n", $lnk);
		}
		if ($this->mid > 0) {
			$lnk = sprintf("%s?tp=%s&op=preview&header=no&mid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid);
			printf("<img src='images/preview.png' title='Voorbeeld' onclick=\"location.href='%s'\">\n", $lnk);	
		}
		if ($this->mid > 0 and $this->deleted_on > "1901-01-01" and $_SESSION['webmaster'] == 1) {
			$lnk = sprintf("%s?tp=%s&op=undelete&mid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid);
			printf("<img src='images/undelete.png' title='Verwijderen ongedaan maken' onclick=\"location.href='%s'\">\n", $lnk);		
		} elseif ($this->mid > 0 and $this->deleted_on < "1901-01-01" and ($_SESSION['webmaster'] == 1 or $_SESSION['lidid'] == $this->AddedBy)) {
			$lnk = sprintf("%s?tp=%s&op=delete&mid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid);
			printf("<img src='images/delete.png' title='Verwijderen' onclick=\"location.href='%s'\">\n", $lnk);				
		}
		echo("</div>  <!-- Einde opdrachtknoppen -->\n");

		printf("</form>\n");
		echo("</div>  <!-- Einde mailing -->\n");
	}
	
	public function post_form() {
		global $naamvereniging, $beperkfrom, $table_prefix;
		
		if ($this->mid == 0) {
			$this->mid = db_mailing("add", 0, 0, "", $_POST['subject']);
		}
	
		if (!isset($_POST['from_name']) or strlen($_POST['from_name']) == 0) {
			$_POST['from_name'] = $naamvereniging;
		}
		
		if (isset($_POST['cc_addr']) and strlen($_POST['cc_addr']) > 5) {
			$_POST['cc_addr'] = str_replace(" ", "", strtolower($_POST['cc_addr']));
			$_POST['cc_addr'] = str_replace(";", ",", $_POST['cc_addr']);
		} else {
			$_POST['cc_addr'] = "";
		}
		if (isset($_POST['template']) and $_POST['template'] == '1') {
			$_POST['template'] = 1;
		} else {
			$_POST['template'] = 0;
		}
		if (isset($_POST['confidential']) and $_POST['confidential'] == '1') {
			$_POST['confidential'] = 1;
		} else {
			$_POST['confidential'] = 0;
		}
		if (isset($_POST['verzamelen']) and $_POST['verzamelen'] == '1') {
			$_POST['verzamelen'] = 1;
		} else {
			$_POST['verzamelen'] = 0;
		}
		
		$old = db_mailing("record", $this->mid);
		$setclause = "";
		$oms = "";
		foreach($_POST as $key => $val) {
			$_POST[$key] = str_replace("\"", "'", $_POST[$key]);
			if (array_key_exists($key, $old) and $old->$key != $_POST[$key]) {
				if (is_numeric($_POST[$key])) {
					$setclause .= sprintf("%s=%d, ", $key, $_POST[$key]);
					$oms .= sprintf("Veld '%s' is %d geworden. ", $key, $_POST[$key]);
				} else {
					$setclause .= sprintf("%s=\"%s\", ", $key, $_POST[$key]);
					if (strlen($_POST[$key]) > 79) {
						$oms .= sprintf("Veld '%s' is '%s ...' geworden. ", $key, substr($_POST[$key], 0, 75));
					} else {
						$oms .= sprintf("Veld '%s' is '%s' geworden. ", $key, $_POST[$key]);
					}
				}
			}
		}
		if (strlen($setclause) > 0) {
			$setclause .= sprintf("changed_on=SYSDATE(), ChangedBy=%d", $_SESSION['lidid']);
			$query  = sprintf("UPDATE %sMailing SET %s WHERE MailingID=%d;", $table_prefix, $setclause, $this->mid);
			fnQuery($query);
			$oms = sprintf("In mailing %d zijn de volgende velden aangepast. %s", $this->mid, $oms);
			db_logboek("add", $oms, 4);
		}
		
		if (isset($_POST['add_address']) and strlen($_POST['add_address']) > 5) {
			$this->add_rcpt(0, $_POST['add_address']);
		}
	}
	
	public function upload() {
	
		if (isset($_FILES['UploadFile']['name']) and strlen($_FILES['UploadFile']['name']) > 3 and $this->mid > 0) {
			$ad = $this->dir_attachm . "/" . $this->mid . "/";
			if (!file_exists($ad)) {
				if (mkdir($ad, 0755, true) == true) {
					$mess = sprintf("Directory '%s' is aangemaakt.", $ad);
					db_logboek("add", $mess, 4);
				} else {
					$mess = sprintf("Directory '%s' bestaat niet en kan niet aangemaakt worden. Probeer het later opnieuw of neem contact op met de webmaster.", $ad);
					db_logboek("add", $mes, 4);
					$ad = "";
				}
			} else {
				chmod($ad, 0755);
			}

			$target = $ad . $_FILES['UploadFile']['name'];
			$ext = explode(".", $_FILES['UploadFile']['name']);  
			$ext = strtolower($ext[count($ext) - 1]);
			if (strlen($ad) == 0) {
				printf("<script>\nalert(\"%s\")\n</script>\n", $mess);
			} elseif (strpos($this->allowed_ext, $ext) === false) {
				$mess = sprintf("Het bestand '%s' kan niet worden bijgesloten, omdat de extensie niet toegestaan is. Alleen de volgende extensies zijn toegestaan: %s", $_FILES['UploadFile']['name'], $this->allowed_ext);
				printf("<script>\nalert(\"%s\")\n</script>\n", $mess);
				db_logboek("add", $mess, 4);
			} elseif ($_FILES['UploadFile']['size'] > $this->max_size_attachm) {
				$mess = sprintf("Het bestand '%s' kan niet worden bijgesloten, omdat het te groot is.", $_FILES['UploadFile']['name']);
				printf("<script>\nalert(\"%s\")\n</script>\n", $mess);
				db_logboek("add", $mess, 4);
			} else {
				if (move_uploaded_file($_FILES['UploadFile']['tmp_name'], $target) == false) {
					$mess = sprintf("Fout %d is opgetreden bij het uploaden van bestand '%s'. Probeer het later opnieuw of neem contact op met de webmaster.", $_FILES['UploadFile']['error'], $_FILES['UploadFile']['name']);
				} else {
					$mess = sprintf("Het bestand '%s' is bijgesloten.", $_FILES['UploadFile']['name']);
				}
				printf("<script>\nalert(\"%s\")\n</script>\n", $mess);
				db_logboek("add", $mess, 4);
			}
		}
	}
	
	public function attach_delete($attach) {
		$ad = $this->dir_attachm . "/" . $this->mid . "/";
		unlink($ad . str_replace("!", ".", $attach));
	}
	
	public function add_rcpt($lidid, $addr="") {
		if (db_mailing("addrcpt", $this->mid, 0, "", "", $lidid, $addr)) {
			if ($lidid > 0) {
				$r = db_naamlid($lidid);
			} else {
				$r = $addr;
			}
			$mess = sprintf("%s is aan mailing %d (%s) toegevoegd.", $r, $this->mid, $this->subject);
			db_logboek("add", $mess, 4);
		}
	}

	public function delete_rcpt($lidid, $addr="") {
		$mess = db_mailing("delrcpt", $this->mid, 0, "", "", $lidid, $addr);
		if (strlen($mess) > 0) {
			db_logboek("add", $mess, 4);
		}
	}
	
	public function add_groep($groepid) {
		$query = sprintf("SELECT Lid FROM Lidond AS LO"
				 . " WHERE OnderdeelID=%d AND LO.Lid NOT IN (SELECT R.LidID FROM Mailing_rcpt AS R WHERE R.LidID=LO.Lid AND R.MailingID=%d)"
				 . " AND LO.Vanaf <= SYSDATE() AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd > SYSDATE());", $groepid, $this->mid);
		$result = fnQuery($query);
		foreach($result->fetchAll() as $row) {
			db_mailing("addrcpt", $this->mid, 0, "", "", $row->Lid);
		}
		$mess = sprintf("Groep '%s' is aan mailing %d toevoegd.", db_naam_onderdeel($groepid), $this->mid);
		db_logboek("add", $mess, 4);
	}
		
	public function delete_groep($groepid) {
		global $table_prefix;
	
		if ($groepid == -1) {
			$query = sprintf("DELETE FROM %sMailing_rcpt WHERE MailingID=%d;", $table_prefix, $this->mid);
			$result = fnQuery($query);
			if ($result > 0) {
				$mess = sprintf("Alle %d ontvangers zijn uit mailing %d verwijderd.", $result, $this->mid);
				db_logboek("add", $mess, 4);
				$query = sprintf("OPTIMIZE TABLE %sMailing_rcpt;", $table_prefix);
				fnQuery($query);
			}
		} else {
			$rows = db_lidond("actueleleden", 0, "", sprintf("LO.OnderdeelId=%d", $groepid));
			foreach($rows as $row) {
				db_mailing("delrcpt", $this->mid, 0, "", "", $row->Lid);
			}
			$mess = sprintf("De leden van groep '%s' zijn bij mailing %d verwijderd.", db_naam_onderdeel($groepid), $this->mid);
			db_logboek("add", $mess, 4);
		}
	}
	
	public function preview() {
		$this->send_mailing(1);
	}	
	
	public function send() {
	
		if (strtotime($this->sent_on) > (time() - $this->vertraging_tussen_verzenden)) {
			printf("<p class='Mededeling'>Deze mailing is korter dan %d minuten geleden verzonden en wordt daarom niet nogmaals verzonden.</p>\n", $this->vertraging_tussen_verzenden/60);
		} elseif (isValidMailAddress($this->from_addr, 0) == false) {
			printf("<p class='mededeling'>Er is geen geldig 'Vanaf e-mail' ingevuld en daarom wordt deze e-mail niet verzonden.</p>\n");
		} else {
			$this->send_mailing(0);
		}
	}
	
	private function send_mailing($preview=0) {
		global $emailsecretariaat, $table_prefix;
		
		if ($preview == 0) {
			$mail = new RBMmailer();
			$mail->From = $this->from_addr;
			$mail->FromName = $this->from_name;
			$mail->Subject = $this->subject;
			foreach (explode(",", $this->cc_addr) as $v) {
				if (isValidMailAddress($v, 0)) {
					$mail->AddCC($v);
				}
			}
		
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
		
		$rcpts = db_mailing("rcpt", $this->mid);
		if ($preview == 1 and count($rcpts) == 0) {
			$this->create_mess($_SESSION['lidid']);
			echo("<br>\n" . $this->mailbericht);
		}
		
		foreach($rcpts as $rcpt) {
			$this->create_mess($rcpt->Nummer);
			if ($preview == 1) {
				echo("<br>\n" . $this->mailbericht);
			}
			if (isValidMailAddress($rcpt->EMAIL, 0) or isValidMailAddress($rcpt->EmailVereniging, 0) or isValidMailAddress($rcpt->to_address, 0)) {
				if (isValidMailAddress($rcpt->EMAIL, 0)) {
					$e = strtolower($rcpt->EMAIL);
				} elseif (isValidMailAddress($rcpt->EmailVereniging, 0)) {
					$e = strtolower($rcpt->EmailVereniging);
				} else {
					$e = strtolower($rcpt->to_address);
					$this->contains_losse_email = true;
				}
				if (isset($rcpt->Naam_lid)) {
					$this->ok_send .= sprintf("<li>%s {%s}</li>\n", $rcpt->Naam_lid, $e);
				} else {
					$this->ok_send .= sprintf("<li>%s</li>\n", $e);
				}
				if ($preview == 0) {
					if (isset($rcpt->Naam_lid)) {
						$mail->AddAddress($e, $rcpt->Naam_lid);
						$this->mailbericht = str_ireplace("[%TO%]", $rcpt->Naam_lid, $this->mailbericht);
						$to_name = $rcpt->Naam_lid;
					} else {
						$mail->AddAddress($e);
						$this->mailbericht = str_ireplace("[%TO%]", $e, $this->mailbericht);
						$to_name = $this->to_name;
					}
					$mail->MsgHTML($this->mailbericht);
					if ($this->verzamelen) {
						db_add_mailing_hist($rcpt->Nummer, $this->mid, $to_name, $e, $this->merged_message);
					} else {
						if ($mail->Send()) {
							db_add_mailing_hist($rcpt->Nummer, $this->mid, $to_name, $e, $this->merged_message);
						}
						$mail->ClearAddresses();
					}
				}
			} else {
				$this->nok_send .= sprintf("<li>%s {%s, %s&nbsp&nbsp;%s}</li>\n", $rcpt->Naam_lid, $rcpt->Adres, $rcpt->Postcode, $rcpt->Woonplaats);
			}
		}
		if ($this->verzamelen and $preview == 0) {
			$mail->Send();
			$mail->ClearAddresses();
		}
		$summary = "";
		
		if ($preview == 0 and strlen($this->ok_send) > 1) {
			$query = sprintf("UPDATE %sMailing SET sent_on=SYSDATE(), SentBy=%d WHERE MailingID=%d;", $table_prefix, $_SESSION['lidid'], $this->mid);
			fnQuery($query);
			$oms = sprintf("Mailing %d (%s) is door %s verzonden.", $this->mid, $this->subject, $_SESSION['naamingelogde']);
			db_logboek("add", $oms, 4);
		}
		if ($this->resultaatversturen == 1) {
			if (strlen($this->ok_send) > 1 and $preview == 1) {
				$summary = sprintf("<p>De volgende ontvangers hebben een correct e-mailadres:<ul>\n%s</ul>\n</p>\n", $this->ok_send);
			} elseif (strlen($this->ok_send) > 1) {
				$summary = sprintf("<p>Deze mailing is verzonden op %s om %s naar:<ul>\n%s</ul>\n</p>\n", strftime("%e %B %Y"), strftime("%H:%M"), $this->ok_send);
			}
			if (strlen($this->nok_send) > 1 and $preview == 1) {
				$summary .= sprintf("De volgende ontvangers hebben geen (correct) e-mailadres in de database: <ul>\n%s</ul>\n", $this->nok_send);
			} elseif (strlen($this->nok_send) > 1) {
				$summary .= sprintf("Deze mailing kon niet verzonden worden naar: <ul>\n%s</ul>\n", $this->nok_send);
			}
			if ($preview == 1) {
				printf("<div id='container'>%s</div>\n", $summary);
			} else {
				$this->create_mess();
				$ev = "E-mail";
				$this->mailbericht = str_ireplace("[%TO%]", "", $this->mailbericht);
				$mail->MsgHTML($this->mailbericht . $summary);
				$mail->Subject = "Resultaat mailing: " . $mail->Subject;
				if (isValidMailAddress($mail->From, 0)) {
					$mail->AddAddress($mail->From, $mail->FromName);
				} else {
					$mail->AddAddress($_SESSION['emailingelogde']);
				}
				$mail->ClearCCs();
				if (isset($emailsecretariaat) and isValidMailAddress($emailsecretariaat, 0)) {
					$mail->AddCC($emailsecretariaat);
				}
				$mail->Send();
			}
		}
	}
	
	private function create_mess($lidid=0) {
		global $table_prefix;
		
		$this->merged_message = $this->message;
		if ($lidid > 0) {
			foreach($this->MergeField as $fld) {
				$nm = "[%" . $fld['Naam'] . "%]";
				if (strpos($this->merged_message, $nm) !== false) {
					$query  = str_replace("%d", $lidid, $fld['SQL']);
					$result = fnQuery($query);
					$nv = "";
					foreach($result->fetchAll(PDO::FETCH_NUM) as $val) {
						if (strlen($nv) > 0) { $nv .= ", "; }
						$nv .= $val[0];
					}
					$this->merged_message = str_ireplace($nm, htmlentities($nv), $this->merged_message);
					$this->contains_mergefield = true;
				}
			}
		}
		
		if (file_exists($this->bestand_briefpapier)) {
			$this->mailbericht = file_get_contents($this->bestand_briefpapier);
			if ($this->mailbericht !== false) {
				$this->mailbericht = str_ireplace("[%FROM%]", $this->from_name, $this->mailbericht);
				if (strlen($this->to_name) > 0) {
					$this->mailbericht = str_ireplace("[%TO%]", $this->to_name, $this->mailbericht);
				}
				$this->mailbericht = str_ireplace("[%SUBJECT%]", $this->subject, $this->mailbericht);
				$this->mailbericht = str_ireplace("[%MESSAGE%]", $this->merged_message, $this->mailbericht);
			} else {
				printf("<p class='mededeling'>Er gaat iets mis met het laden van bestand '%s'. Controleer of dit bestand niet beschadigd en benaderbaar is.</p>\n", $this->bestand_briefpapier);
			}
		} else {
			printf("<p class='mededeling'>Bestand '%s' bestaat niet.</p>\n", $this->bestand_briefpapier);
		}
	}
	
	public function delete() {
		global $table_prefix;
	
		$query = sprintf("UPDATE %sMailing SET deleted_on=NOW(), DeletedBy=%d WHERE MailingID=%d;", $table_prefix, $_SESSION['lidid'], $this->mid);
		$result = fnQuery($query);
		if ($result > 0) {
			$mess = sprintf("Mailing %d (%s) is door %s naar de prullenbak verplaatst.", $this->mid, $this->subject, $_SESSION['naamingelogde']);
			db_logboek("add", $mess, 4);
		}
	}
	
	public function undelete() {
		$query = sprintf("UPDATE Mailing SET deleted_on=NULL WHERE MailingID=%d;", $this->mid);
		fnQuery($query);
	}
		
	private function vulvars() {
		global $naamvereniging;
	
		if ($this->mid > 0) {
			$ml = db_mailing("", $this->mid);
			$this->from_name = trim($ml->from_name);
			$this->from_addr = trim($ml->from_addr);
			$this->to_name = trim($ml->to_name);
			$this->cc_addr = trim($ml->cc_addr);
			$this->subject = trim($ml->subject);
			$this->message = $ml->message;
			$this->template = $ml->template;
			$this->confidential = $ml->confidential;
			$this->verzamelen = $ml->verzamelen;
			$this->new_on = $ml->new_on;
			$this->AddedBy = $ml->AddedBy;
			$this->changed_on = $ml->changed_on;
			$this->ChangedBy = $ml->ChangedBy;
			$this->sent_on = $ml->sent_on;
			$this->SentBy = $ml->SentBy;
			$this->deleted_on = $ml->deleted_on;
			$this->DeletedBy = $ml->DeletedBy;
				
			foreach ($this->MergeField as $fld) {
				$nm = "[%" . $fld['Naam'] . "%]";
				if (strpos($this->mailbericht, $nm) !== false) {
					$this->contains_mergefield = true;
				}
			}
			
		} else {
			$this->from_name = $naamvereniging;
			$this->from_addr = $_SESSION['emailingelogde'];
			$this->new_on = date("Y-m-d");
		}
	}
}

?>