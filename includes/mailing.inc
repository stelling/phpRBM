<?php

class mailing {
	public $mid;
	
	public $allowed_ext = "gif, jpeg, jpg, pdf, png, pps, rar, txt, zip";
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
	
	private $ok_send = "";
	private $nok_send = "";

	function __construct() {
		global $selectnaam, $table_prefix, $fdlang, $fdkort, $max_grootte_bijlage;
	
		$WhereLidond = "WHERE LO.Vanaf <= CURDATE() AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd >= CURDATE())";
		$WhereLidmaatschap = "WHERE LM.LIDDATUM <= CURDATE() AND ((LM.Opgezegd IS NULL) OR LM.Opgezegd >= CURDATE())";
		$WhereLiddipl = "WHERE ((LD.LicentieVervallenPer IS NULL) OR LD.LicentieVervallenPer >= CURDATE())";
		
		$this->dir_attachm = substr($_SERVER["SCRIPT_FILENAME"], 0, strrpos($_SERVER["SCRIPT_FILENAME"], "/")) . "/attachments";
		
		if (isset($max_grootte_bijlage) and $max_grootte_bijlage > 0) {
			$max_size_attachm = $max_grootte_bijlage;
		}

		$sql = sprintf("SELECT Roepnaam FROM %sLid AS L WHERE %s;", $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "Roepnaam", 'SQL' => $sql);

		$sql = sprintf("SELECT Voorletter FROM %sLid AS L WHERE %s;", $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "Voorletters", 'SQL' => $sql);

		$sql = sprintf("SELECT (CASE ISNULL(L.Tussenv) WHEN 0 THEN CONCAT(L.Tussenv, ' ', L.Achternaam)"
			  . " ELSE L.Achternaam END) AS Achternaam FROM %sLid AS L WHERE %s;", $table_prefix, "L.Nummer=%d");
		$this->MergeField[]=array('Naam' => "Achternaam", 'SQL' => $sql);

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
	}

	public function lijst($filter="") {
		global $bewaartijdmailings;
	
		$lnk = sprintf("<a href='%s?tp=%s&amp;op=edit&amp;mid=%s' title='Bewerk mailing'><img src='images/zoom.png'></a>", $_SERVER['PHP_SELF'], $_GET['tp'], "%d");
		$lnk_lk = sprintf("<a href='%s?tp=Mailing/Historie&amp;op=historie&amp;mid=%s' title='Bekijk historie'><img src='images/object_10.png'></a>", $_SERVER['PHP_SELF'], "%d");
		echo(fnDiplayTable(db_mailing("lijst", 0, 0, $filter), $lnk, "", 0, $lnk_lk));
		if ($filter == "Prullenbak") {
			printf("<p>Mailings die langer dan %d maanden geleden verwijderd zijn, worden automatisch definitief opgeschoond.</p>\n", $bewaartijdmailings);
		}
	}
	
	public function toonverstuurdemail ($rid) {
		echo("<div id='verstuurdemail'>\n");
		$row = db_mailing("histdetails", 0, $rid);
		printf("<label>Verzonden:</label><field>%s</field>\n", strftime('%e %B %Y (%H:%M)', strtotime($row->send_on)));
		printf("<label>Van:</label><field>%s</field>\n", $row->Van);
		printf("<label>Aan:</label><field>%s</field>\n", $row->to_name);
		printf("<label>E-mail:</label><field>%s</field>\n", $row->to_addr);
		printf("<label>CC:</label><field>%s</field>\n", $row->cc_addr);
		printf("<label>Onderwerp:</label><field>%s</field>\n", $row->subject);
		printf("<label>Bericht:</label><field>%s</field>\n", $row->message);
		echo("</div>  <!-- Einde verstuurdemail -->\n");
	}
	
	public function edit() {
		global $currenttab, $currenttab2, $beperkfrom, $naamvereniging;

		$ad = $this->dir_attachm . "/" . $this->mid;

		if ($this->mid > 0) {
			$ml = db_mailing("", $this->mid);
			$from_name = $ml->from_name;
			$from_addr = $ml->from_addr;
			$to_name = $ml->to_name;
			$cc_addr = $ml->cc_addr;
			$subject = $ml->subject;
			$message = $ml->message;
		} else {
			$lid = db_gegevenslid($_SESSION['lidid']);
			$from_name = $naamvereniging;
			$from_addr = $this->detailssender("emlE-mail");
			$to_name = "";
			$cc_addr = "";
			$subject = "";
			$message = "";
		}

		if ($this->mid == 0) {
			$currenttab2 = "Concepten";
		}
		printf("<div id='mailing'>\n");
		printf("<form method='post' action='%s?tp=%s/%s&amp;op=post&amp;mid=%d' name='frm_mailedit' enctype='multipart/form-data'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2, $this->mid);
		printf("<input type='hidden' name='submit' value='1'>\n");
		printf("<table>\n");
		printf("<tr><td class='label'>Van:</td><td><input type='text' name='from_name' value='%s' size=50 maxlength=50></td></tr>\n", $from_name);
		printf("<tr><td class='label'>Vanaf e-mail:</td><td><input type='email' name='from_addr' value='%s' size=50 maxlength=50></td></tr>\n", $from_addr);
		printf("<tr><td class='label'>Aan:</td><td><input type='text' name='to_name' value='%s' size=50 maxlength=50></td></tr>\n", $to_name);
		
		$meldingen = "Er zijn nog geen ontvangers aan deze mailing toegevoegd.<br>\n";
		$verzendenmag = false;
		if ($this->mid > 0) {
			echo("<tr><td class='label'>Ontvangers met e-mail:");
			echo("<td><ul class='listrecipients'>\n");
			
			$rcpt_rows = db_rcpt_mailing($this->mid);
			$aant_ze = 0;
			$text_ze = "";
			if (count($rcpt_rows) > 0) {
				foreach($rcpt_rows as $rcpt) {
					if ($rcpt->Nummer > 0) {
						$lnk = sprintf("%s?tp=%s&op=del_lid&mid=%d&lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, $rcpt->Nummer);
						$reg = sprintf("<li>%s&nbsp;<a href='%s'><img src='/images/delete.gif' alt='Verwijderen' title='Verwijderen'></a></li>\n", $rcpt->Naam_lid, $lnk);
					} else {
						$lnk = sprintf("%s?tp=%s&op=del_lid&mid=%d&addr=%s", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid, $rcpt->to_address);
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
			$rcpt_rows = db_pot_rcpt_mailing($this->mid);
			foreach($rcpt_rows as $rcpt) {
				printf("<option value='%d'>%s</option>\n", $rcpt->Nummer, $rcpt->Zoeknaam_lid);
			}
			printf("</select>");
			$lnk = sprintf("%s?tp=%s&op=add_lid&mid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid);
			printf("&nbsp;<img src='/images/move.gif' alt='move' title='Lid toevoegen' onclick=\"elem=document.getElementById('add_lid'); val=elem.options[elem.selectedIndex].value; if (val!=0) { location.href='%s&lidid=' + val }\">&nbsp;\n", $lnk);
			echo("E-mail: <input type='email' name='add_address' id='add_address' size=50 maxlength=50>");
			printf("&nbsp;<img src='/images/move.gif' alt='move' title='E-mail toevoegen' onclick=\"elem=document.getElementById('add_address'); val=elem.value; location.href='%s&to_address=' + val\">&nbsp;\n", $lnk);
			echo("</td></tr>\n<tr>");
			
			echo("<td class='label'>Groep toevoegen of verwijderen:</td>");
			printf("<td><select name='sel_group_add' id='sel_group'>\n");
			printf("<option value=0>&nbsp;</option>\n");
			printf("<option value=-1>Iedereen</option>\n");
			printf("<option value=0>&nbsp;</option>\n");
			$rows = db_Onderdelen();
			foreach($rows as $grp) {
				printf("<option value=%d>%s</option>\n", $grp->RecordID, $grp->Oms);
			}
			printf("</select>\n");
			$lnk = sprintf("%s?tp=%s&op=add_groep&mid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid);
			printf("<img src='/images/move.gif' alt='move' title='groep toevoegen' onclick=\"elem=document.getElementById('sel_group'); val=elem.options[elem.selectedIndex].value; if (val>0) { location.href='%s&groepid=' + val }\">&nbsp;\n", $lnk);
			$lnk = sprintf("%s?tp=%s&op=del_groep&mid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid);
			printf("<img src='/images/delete.gif' alt='kruis' title='groep verwijderen' onclick=\"elem=document.getElementById('sel_group'); val=elem.options[elem.selectedIndex].value; if (val!=0) { location.href='%s&groepid=' + val }\">&nbsp;\n", $lnk);
			$result = fnQuery("SELECT Max(Gewijzigd) FROM Lidond;");
			printf("(groepen zijn t/m %s bijgewerkt)", strftime("%e %B %Y", strtotime($result->fetchColumn())));
			printf("</td></tr>\n");
		}
		printf("<tr><td class='label'>Cc:</td><td><input type='text' name='cc_addr' value='%s' size=100></td></tr>\n", $cc_addr);
		printf("<tr><td class='label'>Onderwerp:</td><td><input type='text' name='subject' value='%s' size=100></td></tr>\n", $subject);
		if (isset($ml->template) and $ml->template == 1) {
			$templ_chk = " checked";
		} else {
			$templ_chk = "";
		}
		if (isset($ml->confidential) and $ml->confidential == 1) {
			$conf_chk = " checked";
		} else {
			$conf_chk = "";
		}
		
		printf("<tr><td class='label'>Opties:</td><td><input type='checkbox' name='templ' value=1%s>Template&nbsp;<input type='checkbox' name='confidential' value=1%s>Vertrouwelijk</td></tr>\n", $templ_chk, $conf_chk);
		echo("</table>\n");
		
		printf("<textarea name='message'>%s</textarea>\n", $message);
		
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
			['Styles','Format','Font','FontSize'],
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
			if (strlen($subject) == 0) {
				$meldingen .= "Het onderwerp is leeg en dat moet ingevuld zijn.<br>\n";
				$verzendenmag = false;
			}
			if (strtotime($ml->sent_on) > (time() - $this->vertraging_tussen_verzenden)) {
				$meldingen .= sprintf("Deze mailing is korter dan %d minuten geleden verzonden.<br>\n", $this->vertraging_tussen_verzenden/60);
				$verzendenmag = false;
			}
			
			if (!file_exists($this->bestand_briefpapier)) {
				$meldingen .= sprintf("Het bestand %s bestaat niet. Neem hierover contact op met de beheerder.<br>\n", $this->bestand_briefpapier);
				$verzendenmag = false;
			}
			foreach($this->MergeField as $fld) {
				$nm = "[%" . $fld['Naam'] . "%]";
				if (strpos($message, $nm) !== false) {
					$this->contains_mergefield = true;
				}
			}
			if ($this->contains_mergefield == true and $this->contains_losse_email == true) {
				$meldingen .= "Er staan losse e-mailadressen en er wordt met mailmerge gewerkt. Bij de losse e-mailadressen werkt de mailmerge niet.<br>\n";
			}
			if ($ml->new_on > '0000-00-00') {
				if ($ml->AddedBy > 0) {
					$usr = db_gegevenslid($ml->AddedBy);
				} else {
					$usr[0]->Naam = "Onbekend";
				}
				printf("<tr><td class='label'>Toegevoegd door/op:</td><td>%s / %s</td></tr>\n", $usr[0]->Naam, strftime("%e %B %Y (%H:%M)", strtotime($ml->new_on)));
			}
			if ($ml->changed_on > '0000-00-00' and $ml->ChangedBy > 0) {
				$usr = db_gegevenslid($ml->ChangedBy);
				printf("<tr><td class='label'>Gewijzigd door/op:</td><td>%s / %s</td></tr>\n", $usr[0]->Naam, strftime("%e %B %Y (%H:%M)", strtotime($ml->changed_on)));
			}
			if ($ml->sent_on > '0000-00-00' and $ml->SentBy > 0) {
				$usr = db_gegevenslid($ml->SentBy);
				printf("<tr><td class='label'>Verzonden door/op:</td><td>%s / %s</td></tr>\n", $usr[0]->Naam, strftime("%e %B %Y (%H:%M)", strtotime($ml->sent_on)));
			}
			if ($ml->deleted_on > '1901-01-01') {
				if ($ml->DeletedBy > 0) {
					$usr = db_gegevenslid($ml->DeletedBy);
				} else {
					$usr[0]->Naam = "Onbekend";
				}
				printf("<tr><td class='label'>Verwijderd door/op:</td><td>%s / %s</td></tr>\n", $usr[0]->Naam, strftime("%e %B %Y (%H:%M)", strtotime($ml->deleted_on)));
				$verzendenmag = false;
			}
		} else {
			$verzendenmag = false;
		}
		
		if ($this->mid > 0) {
			if (!isValidMailAddress($ml->from_addr)) {
				$verzendenmag = false;
				$meldingen .= "Er moet een geldig e-mailadres bij 'Vanaf e-mail' ingevuld worden.<br>\n";
			} elseif (strlen($beperkfrom) > 2) {
				if (substr($beperkfrom, 0, 1) != "@") {
					$beperkfrom = "@" . $beperkfrom;
				}
				if (strpos($ml->from_addr, $beperkfrom) === false) {
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
		}
		
		printf("</table>\n");
		
		echo("<div id='opdrachtknoppen'>\n");
		if ($this->mid > 0) {
			printf("<input type='image' src='images/save.png' name='action' title='Wijzigingen bewaren' value='Wijzigingen bewaren'>\n");
			printf("<input type='submit' name='action' value='Bewaren & sluiten'>\n");
		} else {
			printf("<input type='submit' name='action' value='Toevoegen'>");
		}
			
		if ($this->mid > 0 and $verzendenmag and $ml->deleted_on < "1901-01-01") {
			$lnk = sprintf("%s?tp=%s&op=send&mid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid);
			printf("<img src='images/sendmail.png' title='Verstuur mailing' onclick=\"location.href='%s'\">", $lnk);
			$lnk = sprintf("%s?tp=%s&op=preview&header=no&mid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid);
			printf("<img src='images/preview.png' title='Voorbeeld' onclick=\"location.href='%s'\">\n", $lnk);	
		}
		if ($this->mid > 0 and $ml->deleted_on > "1901-01-01" and $_SESSION['webmaster'] == 1) {
			$lnk = sprintf("%s?tp=%s&op=undelete&mid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid);
			printf("<img src='images/undelete.png' title='Verwijderen ongedaan maken' onclick=\"location.href='%s'\">\n", $lnk);		
		} elseif ($this->mid > 0 and $ml->deleted_on < "1901-01-01" and ($_SESSION['webmaster'] == 1 or $_SESSION['lidid'] == $ml->AddedBy)) {
			$lnk = sprintf("%s?tp=%s&op=delete&mid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $this->mid);
			printf("<img src='images/delete.png' title='Verwijderen' onclick=\"location.href='%s'\">\n", $lnk);				
		}
		echo("</div>  <!-- Einde opdrachtknoppen -->\n");

		printf("</form>\n");
		echo("</div>  <!-- Einde mailing -->\n");
	}
	
	public function post_form() {
		global $nameorganisation, $beperkfrom;
	
		if (!isset($_POST['from_name']) or strlen($_POST['from_name']) == 0) {
			$_POST['from_name'] = $nameorganisation;
		}
		$setclause = sprintf("from_name=\"%s\"", str_replace("\"", "'", $_POST['from_name']));
	
		if (isset($_POST['from_addr']) and isValidMailAddress($_POST['from_addr'], 1)) {
			$setclause .= sprintf(", from_addr='%s'", strtolower($_POST['from_addr']));
		}
		if (isset($_POST['cc_addr']) and strlen($_POST['cc_addr']) > 5) {
			$cc = str_replace(" ", "", strtolower($_POST['cc_addr']));
			$cc = str_replace(";", ",", $cc);
			$setclause .= sprintf(", cc_addr='%s'", $cc);
		} else {
			$setclause .= ", cc_addr=''";
		}
		if (isset($_POST['templ']) and $_POST['templ'] == '1') {
			$setclause .= ", template=1";
		} else {
			$setclause .= ", template=0";
		}
		if (isset($_POST['confidential']) and $_POST['confidential'] == '1') {
			$setclause .= ", confidential=1";
		} else {
			$setclause .= ", confidential=0";
		}
		
		$to_name = str_replace("\"", "'", $_POST['to_name']);
		$subject = str_replace("\"", "'", $_POST['subject']);
		$message = str_replace("\"", "'", $_POST['message']);
		$setclause .= sprintf(", to_name=\"%s\", subject=\"%s\", message=\"%s\", changed_on=SYSDATE(), ChangedBy=%d", $to_name, $subject, $message, $_SESSION['lidid']);
		if ($this->mid > 0) {
			$query  = sprintf("UPDATE Mailing SET %s WHERE MailingID=%d;", $setclause, $this->mid);
			fnQuery($query);
			$oms = sprintf("Mailing %d is door %s aangepast.", $this->mid, $_SESSION['naamingelogde']);
			db_add_activiteit($oms, 4);
		} else {
			$query = sprintf("INSERT INTO Mailing SET %s, new_on=SYSDATE(), AddedBy=%d;", $setclause, $_SESSION['lidid']);
			$this->mid = fnQuery($query);
			$oms = sprintf("Mailing %d is door %s toegevoegd.", $this->mid, $_SESSION['naamingelogde']);
			db_add_activiteit($oms, 4);
		}
		
		if (isset($_POST['add_address']) and strlen($_POST['add_address']) > 5) {
			$this->add_lid(0, $_POST['add_address']);
		}
	}
	
	public function upload() {
	
		if (isset($_FILES['UploadFile']['name']) and strlen($_FILES['UploadFile']['name']) > 3 and $this->mid > 0) {
			$ad = $this->dir_attachm . "/" . $this->mid . "/";
			if (!file_exists($ad)) {
				if (mkdir($ad, 0755, true) == true) {
					$ret = sprintf("Directory '%s' is aangemaakt.", $ad);
					db_add_activiteit($ret, 3);
				} else {
					$ret = sprintf("Directory '%s' bestaat niet en kan niet aangemaakt worden. Waarschijnlijk een rechtenprobleem.", $ad);
					db_add_activiteit($ret, 3);
					$ad = "";
				}
			} else {
				chmod($ad, 0755);
			}

			$target = $ad . $_FILES['UploadFile']['name'];
			$ext = explode(".", $_FILES['UploadFile']['name']);  
			$ext = strtolower($ext[count($ext) - 1]);
			if (strlen($ad) == 0) {
				printf("<script>\nalert(\"%s\")\n</script>\n", $ret);
			} elseif (strpos($this->allowed_ext, $ext) === false) {
				printf("<script>\nalert(\"Het bestand met extensie %s kan niet worden bijgesloten, omdat de extensie niet toegestaan is. Alleen de volgende extensies zijn toegestaan: %s\")\n</script>\n", $ext, $this->allowed_ext);
			} elseif ($_FILES['UploadFile']['size'] > $this->max_size_attachm) {
				echo("<script>\nalert(\"Het bestand kan niet worden bijgesloten, omdat het te groot is.\")\n</script>\n");
			} else {
				move_uploaded_file($_FILES['UploadFile']["tmp_name"], $target);
			} 
		}
	}
	
	public function attach_delete($attach) {
		$ad = $this->dir_attachm . "/" . $this->mid . "/";
		unlink($ad . str_replace("!", ".", $attach));
	}
	
	public function add_lid($lidid, $addr="") {
		if ($lidid > 0) {
			$query = sprintf("SELECT COUNT(*) FROM Mailing_rcpt WHERE MailingID=%d AND LidID=%d;", $this->mid, $lidid);
			$result = fnQuery($query);
			if ($result->fetchColumn() == 0) {
				$query = sprintf("INSERT INTO Mailing_rcpt SET MailingID=%d, LidID=%d, Ingevoerd=SYSDATE();", $this->mid, $lidid);
				fnQuery($query);
			}
		}
		if (strlen($addr) > 5 and isValidMailAddress($addr, 0)) {
		
			$query = sprintf("SELECT COUNT(*) FROM Mailing_rcpt WHERE MailingID=%d AND to_address='%s';", $this->mid, strtolower($addr));
			$result = fnQuery($query);
			if ($result->fetchColumn() == 0) {
				$query = sprintf("INSERT INTO Mailing_rcpt SET MailingID=%d, to_address='%s', Ingevoerd=SYSDATE();", $this->mid, strtolower($addr));
				fnQuery($query);
			}
			
		}
	}
		
	public function delete_lid($lidid, $addr="") {
		if ($lidid > 0) {
			$query = sprintf("DELETE FROM Mailing_rcpt WHERE MailingID=%d AND LidID=%d;", $this->mid, $lidid);
			fnQuery($query);
		} elseif (strlen($addr) > 0) {
			$query = sprintf("DELETE FROM Mailing_rcpt WHERE MailingID=%d AND to_address='%s';", $this->mid, $addr);
			fnQuery($query);
		}
	}
	
	public function add_groep($groepid) {
		$query = sprintf("SELECT Lid FROM Lidond AS LO"
				 . " WHERE OnderdeelID=%d AND LO.Lid NOT IN (SELECT R.LidID FROM Mailing_rcpt AS R WHERE R.LidID=LO.Lid AND R.MailingID=%d)"
				 . " AND LO.Vanaf <= SYSDATE() AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd > SYSDATE());", $groepid, $this->mid);
		$result = fnQuery($query);
		foreach($result->fetchAll() as $row) {
			$this->Add_lid($row->Lid);
		}
	}
		
	public function delete_groep($groepid) {
	
		if ($groepid == -1) {
			$query = sprintf("DELETE FROM Mailing_rcpt WHERE MailingID=%d;", $this->mid);
			fnQuery($query);
		} else {
			$query = sprintf("SELECT Lid FROM Lidond AS LO"
					 . " WHERE OnderdeelID=%d AND LO.Lid IN (SELECT R.LidID FROM Mailing_rcpt AS R WHERE R.LidID=LO.Lid AND R.MailingID=%d)"
					 . " AND LO.Vanaf <= SYSDATE() AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd > SYSDATE());", $groepid, $this->mid);
			$result = fnQuery($query);
			foreach($result->fetchAll() as $row) {
				$this->Delete_lid($this->mid, $row->Lid);
			}
		}
	}
	
	public function preview() {
		$this->send_mailing(1);
	}	
	
	public function send() {
	
		$ml = db_mailing("", $this->mid);
		if (strtotime($ml->sent_on) > (time() - $this->vertraging_tussen_verzenden)) {
			printf("<p class='Mededeling'>Deze mailing is korter dan %d minuten geleden verzonden en wordt daarom niet nogmaals verzonden.</p>\n", $this->vertraging_tussen_verzenden/60);
		} else {
			$this->send_mailing(0);
		}
	}
	
	private function send_mailing($preview=0) {
		global $emailsecretariaat;
	
		$query = sprintf("SELECT from_addr, from_name, to_name, subject, cc_addr FROM Mailing WHERE MailingID=%d;", $this->mid);
		$result = fnQuery($query);
		$row = $result->fetch();
		
		$mail = new RBMmailer();
		$mail->From = $row->from_addr;
		$mail->FromName = $row->from_name;
		$mail->Subject = $row->subject;
		foreach (explode(",", $row->cc_addr) as $v) {
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
		
		if ($preview == 0) {
			$query = sprintf("UPDATE Mailing SET sent_on=SYSDATE(), SentBy=%d WHERE MailingID=%d;", $_SESSION['lidid'], $this->mid);
			fnQuery($query);
		}
		
		foreach($this->MergeField as $fld) {
			$nm = "[%" . $fld['Naam'] . "%]";
			if (strpos($this->mailbericht, $nm) !== false) {
				$this->contains_mergefield = true;
			}
		}
		
		foreach(db_rcpt_mailing($this->mid) as $rcpt) {
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
					} else {
						$mail->AddAddress($e);
						$this->mailbericht = str_ireplace("[%TO%]", $e, $this->mailbericht);
					}
					$mail->MsgHTML($this->mailbericht);
					if ($mail->Send()) {
						db_add_mailing_hist($rcpt->Nummer, $this->mid, $row->to_name, $e, $this->merged_message);
					}
					$mail->ClearAddresses();
				}
			} else {
				$this->nok_send .= sprintf("<li>%s {%s, %s&nbsp&nbsp;%s}</li>\n", $rcpt->Naam_lid, $rcpt->Adres, $rcpt->Postcode, $rcpt->Woonplaats);
			}
		}
		$mail->ClearCCs();
		$summary = "";
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
			$this->create_mess(0);
			$rows = db_gegevenslid($_SESSION['lidid']);
			$ev = "E-mail";
			$this->mailbericht = str_ireplace("[%TO%]", "", $this->mailbericht);
			$mail->MsgHTML($this->mailbericht . $summary);
			$mail->Subject = "Resultaat mailing: " . $mail->Subject;
			if (isValidMailAddress($mail->From, 0)) {
				$mail->AddAddress($mail->From, $mail->FromName);
			} else {
				$mail->AddAddress($this->detailssender("emlE-mail"));
			}
			if (isset($emailsecretariaat) and isValidMailAddress($emailsecretariaat, 0)) {
				$mail->AddCC($emailsecretariaat);
			}
			$mail->Send();
			
			$oms = sprintf("Mailing %d is door %s verzonden.", $this->mid, $_SESSION['naamingelogde']);
			db_add_activiteit($oms, 4);
		}
	}
	
	private function create_mess($lidid=0) {
		global $table_prefix;
		
		$query = sprintf("SELECT from_name AS `from`, to_name AS `to`, subject, message
								FROM %sMailing WHERE MailingID=%d;", $table_prefix, $this->mid);
		$result = fnQuery($query);
		$row = $result->fetch();
		$this->merged_message = $row->message;
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
					$this->merged_message = str_ireplace($nm, $nv, $this->merged_message);
					$this->contains_mergefield = true;
				}
			}
		}
		
		if (file_exists($this->bestand_briefpapier)) {
			$this->mailbericht = file_get_contents($this->bestand_briefpapier);
			if ($this->mailbericht !== false) {
				$this->mailbericht = str_ireplace("[%FROM%]", $row->from, $this->mailbericht);
				if (strlen($row->to) > 0) {
					$this->mailbericht = str_ireplace("[%TO%]", $row->to, $this->mailbericht);
				}
				$this->mailbericht = str_ireplace("[%SUBJECT%]", $row->subject, $this->mailbericht);
				$this->mailbericht = str_ireplace("[%MESSAGE%]", $this->merged_message, $this->mailbericht);
			} else {
				printf("<p class='mededeling'>Er gaat iets mis met het laden van bestand '%s'. Controleer of dit bestand niet beschadigd en benaderbaar is.</p>\n", $this->bestand_briefpapier);
			}
		} else {
			printf("<p class='mededeling'>Bestand '%s' bestaat niet.</p>\n", $this->bestand_briefpapier);
		}
	}
	
	public function delete() {
		$query = sprintf("UPDATE Mailing SET deleted_on=NOW(), DeletedBy=%d WHERE MailingID=%d;", $_SESSION['lidid'], $this->mid);
		fnQuery($query);
	}
	
	public function undelete() {
		$query = sprintf("UPDATE Mailing SET deleted_on=NULL WHERE MailingID=%d;", $this->mid);
		fnQuery($query);
	}
	
	private function detailssender($rv="Naam") {
		$lid = db_gegevenslid($_SESSION['lidid']);
		if ($rv == "emlE-mail") {
			$ev = "emlE-mail vereniging";
			if (strlen($lid[0]->$ev) > 5) {
				return $lid[0]->$ev;
			} else {
				return $lid[0]->$rv;
			}
		} else {
			return $lid[0]->$rv;
		}
	}
	
}

?>