<?php

function fnRekeningen() {
	global $currenttab, $currenttab2;
	
	fnDispMenu(2);
	
	$reknr = 0;
	if (isset($_GET['p_reknr']) and $_GET['p_reknr'] > 0) {
		$reknr = $_GET['p_reknr'];
	}
		
	if ($currenttab2 == "Muteren") {
		fnRekeningMuteren($reknr);
	} elseif ($currenttab2 == "Beheer") {
		fnRekeningbeheer();
		
	} elseif ($currenttab2 == "Nieuw") {
		
		if ($_SERVER['REQUEST_METHOD'] == "POST") {
			if (isset($_POST['RekToevoegen'])) {
				$reknr = (new cls_Rekening())->add($_POST['nwrekening'], $_POST['nwseizoen'], $_POST['nwlid']);
				printf("<script>location.href='%s?tp=%s/Muteren&p_reknr=%d'</script>\n", $_SERVER['PHP_SELF'], $currenttab, $reknr);
			}
			
		} else {
		
			$nwreknr = (new cls_rekening())->max("Nummer") + 1;
			if (($nwreknr / 2) == intval($nwreknr / 2)) {
				$nwreknr++;
			}
		
			echo("<div id='rekeningmuteren'>\n");
			$actionurl = sprintf("%s?tp=%s", $_SERVER['PHP_SELF'], $_GET['tp']);
			printf("<form method='post' action='%s'>\n", $actionurl);
		
			printf("<label>Rekeningnummer</label><input type='number' name='nwrekening' value=%d>\n", $nwreknr);
			printf("<label>Seizoen</label><select name='nwseizoen'>%s</select>\n", (new cls_Seizoen())->htmloptions());
			printf("<label>Lid</label><select name='nwlid'><option value=0>Selecteer lid ...</option>\n%s</select>\n", (new cls_Lid())->htmloptions(-1, 2));
			echo("<div class='clear'></div>\n");
			echo("<input type='submit' value='Toevoegen' name='RekToevoegen'>\n");
			echo("</form>\n");
			echo("</div> <!-- Einde rekeningregelsmuteren -->\n");
		}
		
	} elseif ($currenttab2 == "Logboek") {
		$rows = (new cls_Logboek())->lijst(14);
		if (count($rows) > 0) {
			echo(fnDisplayTable($rows, "logboek", "Logboek rekeningen", 0, "", "", "logboek"));
		} else {
			echo("<p>Er zijn geen mutaties bekend.</p>\n");
		}
	}
}

function fnRekeningbeheer() {
	global $currenttab;
	
	$i_rk = new cls_Rekening();
	$i_sz = new cls_Seizoen();
	
	if (isset($_GET['op']) and $_GET['op'] == "deleterekening" and $_GET['p_reknr'] > 0) {
		$i_rk->delete($_GET['p_reknr']);
	}

	if (isset($_POST['filterseizoen'])) {
		$filterseizoen = $_POST['filterseizoen'];
	} else {
		$filterseizoen = $i_rk->Max("Seizoen");
	}
	
	echo("<div id='filter'>\n");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	printf("<label>Filter op seizoen</label><select name='filterseizoen' onChange='this.form.submit();'>\n<option value=-1>Alle seizoenen</option>\n%s</select>\n", $i_sz->htmloptions($filterseizoen));
	echo("<input type='text' id='tbFilterNaam' placeholder='Tekstfilter' OnKeyUp=\"fnFilter('overzichtrekeningen', this);\">\n");
	echo("</form>\n");
	echo("</div> <!-- Einde filter -->\n");
	
	$rows = $i_rk->overzichtbeheer($filterseizoen);
	$kols[0]['link'] = sprintf("%s?tp=%s/Muteren&p_reknr=%%d", $_SERVER['PHP_SELF'], $currenttab);
	$kols[0]['columnname'] = "Nummer";
	
	$kols[1]['headertext'] = "Nummer";
	$kols[1]['columnname'] = "Nummer";
	
	$kols[2]['headertext'] = "Datum";
	$kols[2]['columnname'] = "Datum";
	
	$kols[3]['headertext'] = "Omschrijving";
	$kols[3]['columnname'] = "OMSCHRIJV";
	
	$kols[3]['headertext'] = "Tenaamstelling";
	$kols[3]['columnname'] = "DEBNAAM";
	
	$kols[4]['headertext'] = "Bedrag";
	$kols[4]['columnname'] = "Bedrag";
	$kols[4]['type'] = "currency";
		
	$kols[5]['headertext'] = "Betaald";
	$kols[5]['columnname'] = "Betaald";
	$kols[5]['type'] = "currency";
	
	$kols[7]['link'] = sprintf("%s?tp=%s/Beheer&op=deleterekening&p_reknr=%%d", $_SERVER['PHP_SELF'], $currenttab);
	$kols[7]['class'] = 'trash';
	$kols[7]['columnname'] = "linkDelete";
	
	echo(fnDisplayTable($rows, $kols, "", 0, "", "overzichtrekeningen"));
}

function fnRekeningMuteren($p_rkid=-1) {
	global $currenttab;

	$scherm = "M";
	if (isset($_POST['rkid']) and $_POST['rkid'] > 0) {
		$reknr = $_POST['rkid'];
	} elseif (isset($_GET['rkid']) and $_GET['rkid'] > 0) {
		$reknr = $_GET['rkid'];
	} elseif ($p_rkid > 0) {
		$reknr = $p_rkid;
	} else {
		$reknr = 0;
	}
	
	$i_rk = new cls_Rekening($reknr);
	$i_rr = new cls_Rekeningregel($reknr);
	$i_lid = new cls_Lid();
	$i_seiz = new cls_Seizoen();
	$i_ond = new cls_Onderdeel();
	
	if (isset($_GET['op']) and $_GET['op'] == "deleteregel" and $_GET['rrid'] > 0) {
		$i_rr->delete($_GET['rrid']);
	}
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {

		$arrcols = ["Regelnr", "KSTNPLTS", "OMSCHRIJV", "Bedrag"];
		
		foreach($i_rr->perrekening($reknr) as $rrrow) {
			foreach ($arrcols as $col) {
				$cn = sprintf("%s_%d", $col, $rrrow->RecordID);
				if ($col == "Bedrag" and isset($_POST[$cn]) and strlen($_POST[$cn]) == 0) {
					$_POST[$cn] = 0;
					
				} elseif ($col == "Bedrag" and isset($_POST[$cn]) and floatval($_POST[$cn]) != 0) {
					$cnt = sprintf("ToonOpRekening_%d", $rrrow->RecordID);
					$_POST[$cnt] = 1;
				}
				if (isset($_POST[$cn])) {
					$i_rr->update($rrrow->RecordID, $col, $_POST[$cn]);
				}
			}
		}

		if (isset($_POST['RRnieuw']) and $_POST['RRnieuw'] >= 0) {
			$at = $i_rr->standaardwaarde($reknr, $_POST['RRnieuw']);
			if ($at == 0) {
				$i_rr->add($reknr, $_POST['RRnieuw']);
			}
		}

		if (isset($_POST['BewarenSluiten'])) {
			printf("<script>location.href='%s?tp=%s/Beheer'</script>\n", $_SERVER['PHP_SELF'], $currenttab);
		}
	
	}
	$f = sprintf("Rekening=%d", $reknr);
	$i_rk->update($reknr, "Bedrag", $i_rr->totaal("Bedrag", $f));
	
	if ($scherm == "M")  {
		$row = $i_rk->record($reknr);
		if ($row->AantLid == 1) {
			$i_rk->update($reknr, "Lid", $row->EersteLid);
		}
		$tb = $i_rr->totaal("Bedrag", sprintf("Rekening=%d", $reknr));
		$i_rk->update($reknr, "Bedrag", $tb);
		
		echo("<div id='rekeningmuteren'>\n");
		$actionurl = sprintf("%s?tp=%s&rkid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $reknr);
		printf("<form method='post' action='%s'>\n", $actionurl);
		echo("<div id='rekeningkopmuteren'>\n");
		
		printf("<label>Rekeningnummer</label><p id='reknr'>%d</p>\n", $row->Nummer);
		printf("<input type='hidden' name='rkid' value=%d>\n", $row->Nummer);
		
		echo("<div id='rekeninginfo'>\n");
		echo("<label>Totaal bedrag &euro;</label><p id='rekeningbedrag'></p>\n");
		printf("<label id='lblbedragbetaald'>Betaald &euro;</label><p id='bedragbetaald'>%03.2f</p>\n", $row->Betaald);
		echo("<label id='lbluitersteBetaling'>Uiterste betaling op</label><p id='uitersteBetaling'></p>\n");
		$t = $i_lid->telefoon($row->BetaaldDoor);
		if (strlen($t) > 9) {
			printf("<label>Telefoon debiteur</label><p>%s</p>\n", $t);
		}
		$e = $i_lid->email($row->BetaaldDoor);
		if (strlen($e) > 5) {
			printf("<label>E-mail debiteur</label><p>%s</p>\n", $e);
		}
		echo("</div> <!-- Einde rekeninginfo -->\n");
		
		printf("<label>Seizoen</label><select id='Seizoen'>%s</select>\n", $i_seiz->htmloptions($row->Seizoen));
		printf("<label>Rekeningdatum</label><input type='date' id='Datum' value='%s' required>\n", $row->Datum);
		printf("<label>Omschrijving</label><input type='text' id='OMSCHRIJV' class='w30' value='%s' maxlength=30>\n", $row->OMSCHRIJV);
		printf("<label>Tenaamstelling rekening</label><input type='text' id='DEBNAAM' class='w60' value='%s' maxlength=60>\n", $row->DEBNAAM);
		if ($row->AantLid > 1) {
			$f = sprintf("L.RecordID IN (SELECT RR.Lid FROM %sRekreg AS RR WHERE RR.Rekening=%d)", TABLE_PREFIX, $row->Nummer);
			printf("<label>Gekoppeld lid</label><select id='Lid'><option value=-1></option>%s</select>\n", $i_lid->htmloptions($row->Lid, 0, $f, $row->Datum));
		} else {
			printf("<input type='hidden' id='Lid' value=%d>\n", $row->Lid);
		}
		$f = sprintf("(L.RecordID IN (SELECT LO.Lid FROM %sLidond AS LO WHERE IFNULL(LO.Opgezegd, '9999-12-31') >= '%s' AND LO.OnderdeelID=%d)", TABLE_PREFIX, $row->Datum, $_SESSION['settings']['rekening_groep_betaalddoor']);
		$f .= sprintf(" OR L.RecordID IN (SELECT RR.Lid FROM %sRekreg AS RR WHERE RR.Rekening=%d)", TABLE_PREFIX, $reknr);
		$f .= sprintf(" OR L.RecordID=%d OR L.RecordID=%d)", $row->BetaaldDoor, $row->Lid);
		printf("<label>Betaald door / debiteur</label><select id='BetaaldDoor'>\n%s</select>\n", $i_lid->htmloptions($row->BetaaldDoor, 7, $f, $row->Datum));
		if ($row->BETAALDAG < 1) {
			$bdt = $i_seiz->max("BetaaldagenTermijn", sprintf("Nummer=%d", $row->Seizoen));
		} else {
			$bdt = $row->BETAALDAG;
		}
		printf("<label id='lblBETAALDAG'>Betaaltermijn in dagen</label><input type='number' id='BETAALDAG' value=%d min=1>\n", $bdt);
		printf("<label id='lblBET_TERM'>Aantal betaaltermijnen</label><input type='number' id='BET_TERM' value=%d min=1>\n", $row->BET_TERM);
		echo("</div> <!-- einde rekeningkopmuteren  -->\n");
		
		echo("<div class='clear'></div>\n");
		
		if ($reknr > 0) {
			foreach($i_rr->perrekening($reknr) as $rrrow) {
				if (strlen($rrrow->KSTNPLTS) > 0 and strlen($rrrow->OMSCHRIJV) == 0) {
					$ondrow = $i_ond->record(0, $rrrow->KSTNPLTS);
					if (isset($ondrow->Naam)) {
						$i_rr->update($rrrow->RecordID, "OMSCHRIJV", $ondrow->Naam);
						$i_rr->update($rrrow->RecordID, "KSTNPLTS", $ondrow->Kode);
					}
				}
			}
			echo("<div id='rekeningregelsmuteren'>\n");
			echo("<table id='rekregels'>\n");
			echo("<tr><th>#</th><th>Kostenplaats</th><th>Lid</th><th>Omschrijving</th><th>Bedrag in &euro;</th><th>Zichtbaar</th><th>&nbsp;</th></tr>\n");
			foreach($i_rr->perrekening($reknr) as $rrrow) {
				echo("<tr>\n");
				printf("<td><input type='number' id='Regelnr_%d' step=1 value=%d min=1></td>\n", $rrrow->RecordID, $rrrow->Regelnr);
				$jsob = "";
				if (strlen($rrrow->OMSCHRIJV) == 0) {
					$jsob = " onBlur='this.form.submit();'";
				}
				printf("<td><input type='text' id='KSTNPLTS_%d' value='%s' class='w7' maxlength=7%s></td>\n", $rrrow->RecordID, $rrrow->KSTNPLTS, $jsob);
				printf("<td>%s</td>\n", $rrrow->NaamLid);
				printf("<td><input type='text' id='OMSCHRIJV_%d' class='w60' value=\"%s\" maxlength=60></td>\n", $rrrow->RecordID, $rrrow->OMSCHRIJV);
				printf("<td><input type='text' id='Bedrag_%d' value='%s' class='d8'></td>\n", $rrrow->RecordID, number_format($rrrow->Bedrag, 2, ",", ""));
				printf("<td><input type='checkbox' id='ToonOpRekening_%d' value=1%s></td>\n", $rrrow->RecordID, checked($rrrow->ToonOpRekening));
				printf("<td class='trash'><a href='%s?tp=%s&op=deleteregel&rrid=%d&rkid=%d'>&nbsp;&nbsp;&nbsp;</a></td>", $_SERVER['PHP_SELF'], $_GET['tp'], $rrrow->RecordID, $rrrow->Rekening);
				echo("</tr>\n");
			}
			echo("</table>\n");
			$opt = "<option value=-1>Regel(s) toevoegen ...</option><option value=0>Zonder lid</option>\n";
			$opt .= sprintf("<option value=-1 disabled>-- Gezin --</option>\n%s", $i_lid->htmloptions(0, 5, sprintf("L.Postcode='%s' AND L.Adres='%s'", $i_rk->postcode, $i_rk->adres)));
			$opt .= "<option value=-1 disabled>-- Alle leden --</option>\n";
			$opt .= $i_lid->htmloptions(0, 5);
			printf("<select name='RRnieuw' onChange='this.form.submit();'>\n%s</select>\n", $opt);
			echo("</div> <!-- Einde rekeningregelsmuteren -->\n");
		}
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<input type='submit' value='Bewaren' name='Bewaren'>\n");
		echo("<button type='submit' name='BewarenSluiten'>Bewaren en sluiten</button>\n");
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		
		echo("</form>\n");
		echo("</div> <!-- einde rekeningmuteren  -->\n");
		
		printf("<script>
			$( document ).ready(function() {
				rekeningprops();
			});

			\$(\"#rekeningkopmuteren > input, #rekeningkopmuteren > select\").blur(function(){
				savedata('rekeningedit', %1\$d, this);
				rekeningprops();
			});
			
			\$(\"#rekeningregelsmuteren tr td input\").blur(function(){
				savedata('rekregedit', 0, this);
				rekeningprops();
			});
			
			\$(\"#rekeningregelsmuteren tr td input[type='checkbox']\").click(function(){
				savedata('rekregedit', 0, this);
				rekeningprops();
			});
			
		</script>\n", $reknr);
	}

}  # fnRekeningMuteren

function RekeningDetail($p_rkid) {
	
	$rk = (new cls_Rekening())->record($p_rkid);
	
	$rekening_template = $_SESSION['settings']['path_templates'] . sprintf("rekening %d.html", $rk->Seizoen);
	if (file_exists($rekening_template)) {
		$content = file_get_contents($rekening_template);
	} else {
		$rekening_template = $_SESSION['settings']['path_templates'] . "rekening.html";
		if (file_exists($rekening_template)) {
			$content = file_get_contents($rekening_template);	
		} else {
			$content = "<p>[%NAAMDEBITEUR%]<br>
[%ADRES%]<br>
[%POSTCODE%]  [%WOONPLAATS%]</p>
<br>
<p>Mijdrecht, [%REKENINGDATUM%]</p>
<br>
<p><u>Betreft: [%REKENINGOMSCHRIJVING%] [%REKENINGNUMMER%]</u></p>
<br>
<div id='rekeningregels'>
<!-- MetRegelnummers=0 -->
<table>
<tr><th>Regel</th><th>Omschrijving</th><th>Lid</th><th class='number'>Bedrag in &euro;</th></tr>
[%REKENINGREGELS%]
<tr><th colspan=3>Totaal</th><th class='number'>[%REKENINGBEDRAG%]</th><tr>
</table>
</div> <!-- Einde rekeningregels -->

<!-- NietOpCredit -->
<!-- NietOpNulRekening -->
<p>Deze rekening dient op [%UITERSTEBETAALDATUM%], onder vermelding van rekeningnummer [%REKENINGNUMMER%], volledig betaald te zijn.</p>
<p>Indien u een andere betalingsregeling wenst of een vraag over deze rekening heeft kunt u contact met de <a href='mailto:[%VANAFADRES%]'>[%VANAFNAAM%]</a> opnemen.</p>
<!-- /NietOpNulRekening -->
<!-- /NietOpCredit -->

<!-- NietOpDebet -->
<!-- NietOpNulRekening -->
<p>Bovenstaand bedrag wordt zo spoedig mogelijk aan u overgemaakt.</p>
<!-- /NietOpNulRekening -->
<!-- /NietOpDebet -->

<!-- NietOpDebet -->
<p>Indien u een vraag over deze rekening heeft kunt u contact met de <a href='mailto:[%VANAFADRES%]'>[%VANAFNAAM%]</a> opnemen.</p>
<!-- /NietOpDebet -->

<p><strong>Het bestuur van [%NAAMVERENIGING%]</strong></p>\n";
		}
	}
	
	$n = "<!-- MetRegelnummers=";
	$p = strpos($content, $n);
	$metregelnr = 1;
	if ($p > 1) {
		$p += strlen($n);
		$metregelnr = intval(substr($content, $p, 1));
	}
	
	if ($rk->Machtiging == 1) {
		$content = removetextblock($content, "<!-- Geen machtiging -->", "<!-- /Geen machtiging -->");
	} else {
		$content = removetextblock($content, "<!-- Wel machtiging -->", "<!-- /Wel machtiging -->");
	}
	
	if ($rk->Bedrag == 0) {
		$content = removetextblock($content, "<!-- NietOpNulRekening -->", "<!-- /NietOpNulRekening -->");
	} elseif ($rk->Bedrag < 0) {
		$content = removetextblock($content, "<!-- NietOpCredit -->", "<!-- /NietOpCredit -->");
	} else {
		$content = removetextblock($content, "<!-- NietOpDebet -->", "<!-- /NietOpDebet -->");
		if ($rk->Betaald == 0) {
			$content = removetextblock($content, "<!-- NietOpNulBetaald -->", "<!-- /NietOpNulBetaald -->");
		}
	}
	
	if (($rk->Openstaand) < 0.50) {
		$content = removetextblock($content, "<!-- NietOpVolledigBetaald -->", "<!-- /NietOpVolledigBetaald -->");
	}
	$content = str_ireplace("[%NAAMDEBITEUR%]", $rk->Tenaamstelling, $content);
	$content = str_ireplace("[%ADRES%]", $rk->Adres, $content);
	$content = str_ireplace("[%POSTCODE%]", $rk->Postcode, $content);
	$content = str_ireplace("[%WOONPLAATS%]", $rk->Woonplaats, $content);
	$content = str_ireplace("[%LIDNR%]", $rk->Lidnr, $content);
	$content = str_ireplace("[%REKENINGDATUM%]", strftime("%e %B %Y", strtotime($rk->Datum)), $content);
	$content = str_ireplace("[%SEIZOEN%]", $rk->Seizoen, $content);
	$content = str_ireplace("[%REKENINGOMSCHRIJVING%]", $rk->OMSCHRIJV, $content);
	$content = str_ireplace("[%REKENINGNUMMER%]", $rk->Nummer, $content);
	$content = str_ireplace("[%REKENINGBEDRAG%]", fnBedrag($rk->Bedrag), $content);
	$content = str_ireplace("[%BETAALD%]", fnBedrag($rk->Betaald), $content);
	$content = str_ireplace("[%OPENSTAAND%]", fnBedrag($rk->Openstaand), $content);
	$content = str_ireplace("[%BANKREKENING%]", $rk->Bankrekeningnummer, $content);
	$content = str_ireplace("[%UITERSTEBETAALDATUM%]", strftime("%e %B %Y", strtotime($rk->UitersteBetaaldatum)), $content);
	$i_mv = new cls_Mailing_vanaf($_SESSION['settings']['mailing_rekening_vanafid']);
	$content = str_ireplace("[%VANAFADRES%]", $i_mv->vanaf_email, $content);
	$content = str_ireplace("[%VANAFNAAM%]", $i_mv->vanaf_naam, $content);
	$i_mv = null;
	$content = str_ireplace("[%NAAMVERENIGING%]", $_SESSION['settings']['naamvereniging'], $content);
	$content = str_ireplace("[%NAAMWEBSITE%]", $_SESSION['settings']['naamwebsite'], $content);
	$content = str_ireplace("[%URLWEBSITE%]", $_SERVER["HTTP_HOST"], $content);
	
	$rr = (new cls_Rekeningregel())->perrekening($rk->Nummer);
	$rrtxt = "";
	$ireg = 1;
	foreach($rr as $regel) {
		if ($regel->Lid > 0) {
			$nl = (new cls_Lid())->Naam($regel->Lid, "");
		} else {
			$nl = "";
		}
		if ($regel->ToonOpRekening == 1) {
			if ($metregelnr == 1) {
				$rrtxt .= sprintf("<tr><td>%d</td><td>%s</td><td>%s</td><td class='number'>%s</td></tr>\n", $ireg, $regel->OMSCHRIJV, $nl, fnBedrag($regel->Bedrag));
			} else {
				$rrtxt .= sprintf("<tr><td>%s</td><td>%s</td><td class='number'>%s</td></tr>\n", $regel->OMSCHRIJV, $nl, fnBedrag($regel->Bedrag));
			}
			$ireg += 1;
		}
	}
	$content = str_ireplace("[%REKENINGREGELS%]", $rrtxt, $content);
	
	return $content;

} # RekeningDetail

function fnKostenoverzicht() {
	
	$i_mut = new cls_Mutatie();
	$i_gbr = new cls_GBR();

	$val_jaarfilter = "";
	$val_gbrfilter = "";
	$val_kplfilter = "";
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['lbJaarFilter'])) {
			$val_jaarfilter = $_POST['lbJaarFilter'];
		}
		if (isset($_POST['lbGBRFilter'])) {
			$val_gbrfilter = $_POST['lbGBRFilter'];
		}
		if (isset($_POST['lbKPLFilter'])) {
			$val_kplfilter = $_POST['lbKPLFilter'];
		}
	}
	
	echo("<div id='filter'>\n");
	printf("<form name='Filter' action='%s?%s' method='post'>", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
	echo("<table>\n");
	echo("<tr>\n");
	$ret = "";
	foreach ((new cls_Boekjaar())-basislijst("", "Begindatum") as $row) {
		if ($val_jaarfilter == $row->RecordID or strlen($val_jaarfilter) == 0) {
			$s = " selected";
			$val_jaarfilter = $row->RecordID;
		} else {
			$s = "";
		}
		$ret .= sprintf('<option%s value="%2$s">%3$s</option>\n', $s, $row->RecordID, $row->Jaar);
	}
	printf("<td class='label'>Boekjaar</td><td><select name='lbJaarFilter' onchange='form.submit();'>%s</select></td>\n", $ret);
	
	$ret = "<option value='*'>Alle</option>\n" . $i_gbr->htmloptions($val_gbrfilter);
	printf("<td class='label'>Grootboekrekening</td><td><select name='lbGBRFilter' onchange='form.submit();'>%s</select></td>\n", $ret);
	
	$ret = "<option value='*'>Alle</option>\n";
	foreach ((new cls_Kostenplaats())->basislijst("", "Kode") as $row) {
		if ($val_kplfilter == $row->Kode) {
			$s = " selected";
		} else {
			$s = "";
		}
		$ret .= sprintf('<option%s value="%s">%s</option>\n', $s, $row->Kode, $row->Kode);
	}
	printf("<td class='label'>Kostenplaats</td><td><select name='lbKPLFilter' onchange='form.submit();'>%s</select></td>\n", $ret);
	
	echo("</tr>\n");
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde filter -->\n");

	$lijst = (new cls_Mutatie())->lijst($val_jaarfilter, $val_gbrfilter, $val_kplfilter);
	echo(fnDisplayTable($lijst, "", "", 1));
}  # fnKostenoverzicht

function fnBedrag($bedrag) {
	if (strlen($_SESSION['settings']['mailing_rekening_valuta']) == 0) {
		return str_replace(",00", ",=&nbsp;", sprintf("%03.2f", $bedrag));
	} else {
		return $_SESSION['settings']['mailing_rekening_valuta'] . "&nbsp;" . str_replace(",00", ",=&nbsp;", sprintf("%03.2f", $bedrag));
	}	
}  #fnBedrag

?>