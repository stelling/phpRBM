<?php

function fnRekeningen() {
	global $currenttab, $currenttab2;
	
	fnDispMenu(2);
		
	if ($currenttab2 == "Muteren") {
		fnRekeningMuteren();
	} elseif ($currenttab2 == "Logboek") {
		$rows = (new cls_Logboek())->lijst(14);
		if (count($rows) > 0) {
			echo(fnDisplayTable($rows, "", "Logboek rekeningen", 0, "", "", "logboek"));
		} else {
			echo("<p>Er zijn geen mutaties bekend.</p>\n");
		}
	}
}

function fnRekeningMuteren() {
	$scherm = "M";
	$i_rk = new cls_Rekening();
	$i_rr = new cls_Rekeningregel();
	$i_lid = new cls_Lid();
	$i_seiz = new cls_Seizoen();
	
	$reknr = 2020000;
	
	echo("<div id='rekeningmuteren'>\n");
	
	if (isset($_GET['op']) and $_GET['op'] == "delete" and $_GET['rrid'] > 0) {
		$i_rr->delete($_GET['rrid']);
	}
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		$i_rk->update($reknr, "Seizoen", $_POST['seizoen']);
		$i_rk->update($reknr, "OMSCHRIJV", $_POST['omschrijving']);
		$i_rk->update($reknr, "DEBNAAM", $_POST['debnaam']);
		if (isset($_POST['lid'])) {
			$i_rk->update($reknr, "Lid", $_POST['lid']);
		}
		$i_rk->update($reknr, "Datum", $_POST['datum']);
		$i_rk->update($reknr, "BetaaldDoor", $_POST['betaalddoor']);
		
		$arrcols = ["Regelnr", "KSTNPLTS", "OMSCHRIJV", "Bedrag", "ToonOpRekening"];
		
		foreach($i_rr->perrekening($reknr) as $rrrow) {
			foreach ($arrcols as $col) {
				$cn = sprintf("%s_%d", $col, $rrrow->RecordID);
				if ($col == "Bedrag" and isset($_POST[$cn]) and strlen($_POST[$cn]) == 0) {
					$_POST[$cn] = 0;
					
				} elseif ($col == "Bedrag" and isset($_POST[$cn]) and floatval($_POST[$cn]) != 0) {
					$cnt = sprintf("ToonOpRekening_%d", $rrrow->RecordID);
					$_POST[$cnt] = 1;
				}
				if ($col == "ToonOpRekening" and !isset($_POST[$cn])) {
					$_POST[$cn] = 0;
				}
				if (isset($_POST[$cn])) {
					$i_rr->update($rrrow->RecordID, $col, $_POST[$cn]);
				}
			}
		}
				
		if (isset($_POST['RRnieuw']) and $_POST['RRnieuw'] >= 0) {
			$i_rr->add($reknr, $_POST['RRnieuw'], $_POST['KSTNPLTSnieuw']);
		}
		
		$f = sprintf("Rekening=%d", $reknr);
		$i_rk->update($reknr, "Bedrag", $i_rr->totaal("Bedrag", $f));
		
	}
	
	if ($scherm == "M")  {
		$row = $i_rk->record($reknr);
		
		$actionurl = sprintf("%s?tp=%s&amp;rekid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $reknr);
		printf("<form method='post' action='%s' name='wijzigenrekening'>\n", $actionurl);
		
		printf("<label>Rekeningnummer</label><p>%d</p>\n", $row->Nummer);
		
		echo("<div id='rekeninginfo'>\n");
		printf("<label>Totaal bedrag</label><p>&euro;&nbsp;%s</p>\n", fnBedrag($row->Bedrag));
		printf("<label>Betaald</label><p>&euro;&nbsp;%s</p>\n", fnBedrag($row->Betaald));
		$cl = "";
		if ($row->Bedrag > $row->Betaald and $row->UitersteBetaaldatum < date("Y-m-d")) {
			$cl = "class='telaat'";
		}
		printf("<label>Uiterste betaling op</label><p %s>%s</p>\n", $cl, strftime("%e %B %Y", strtotime($row->UitersteBetaaldatum)));
		printf("<label>Telefoon debiteur</label><p>%s</p>\n", $i_lid->telefoon($row->BetaaldDoor));
		printf("<label>E-mail debiteur</label><p>%s</p>\n", $i_lid->email($row->BetaaldDoor));
		echo("</div> <!-- Einde rekeninginfo -->\n");
		
		printf("<label>Seizoen</label><select name='seizoen'>%s</select>\n", $i_seiz->htmloptions($row->Seizoen));
		printf("<label>Omschrijving</label><input type='text' name='omschrijving' value='%s'>\n", $row->OMSCHRIJV);
		printf("<label>Tenaamstelling rekening</label><input type='text' name='debnaam' value='%s'>\n", $row->DEBNAAM);
		if ($row->AantLid == 1) {
			$f = sprintf("Lid > 0 AND Rekening=%d", $row->Nummer);
			printf("<input type='hidden' name='lid' value=%d>\n", $i_rr->min("Lid", $f));
		} elseif ($row->AantLid > 1) {
			$f = sprintf("L.RecordID IN (SELECT RR.Lid FROM %sRekreg AS RR WHERE RR.Rekening=%d)", TABLE_PREFIX, $row->Nummer);
			printf("<label>Gekoppeld aan lid</label><select name='lid'><option value=-1></option>%s</select>", $i_lid->htmloptions($row->Lid, 0, $f));
		}
		printf("<label>Betaald door</label><select name='betaalddoor'>\n%s</select>\n", $i_lid->htmloptions($row->BetaaldDoor));
		printf("<label>Rekeningdatum</label><input type='date' name='datum' value='%s'>\n", $row->Datum);
		printf("<label>Betaaltermijn in dagen</label><input type='number' name='betaaldag' value=%d min=1>\n", $row->BETAALDAG);
		printf("<label class='k2'>Aantal betaaltermijnen</label><input type='number' name='betaaldag' value=%d min=1>\n", $row->BET_TERM);
		
		echo("<div class='clear'></div>\n");
		echo("<div id='rekeningregelsmuteren'>\n");
		echo("<table>\n");
		echo("<tr><th>#</th><th>Kostenplaats</th><th>Lid</th><th>Omschrijving</th><th>Bedrag in &euro;</th><th>Zichtbaar</th><th>&nbsp;</th></tr>\n");
		foreach($i_rr->perrekening($reknr) as $rrrow) {
			echo("<tr>\n");
			printf("<td><input type='number' name='Regelnr_%d' step=1 value=%d min=1></td>\n", $rrrow->RecordID, $rrrow->Regelnr);
			printf("<td><input type='text' name='KSTNPLTS_%d' value='%s' class='kostenplaats' maxlength=5></td>\n", $rrrow->RecordID, $rrrow->KSTNPLTS);
			printf("<td>%s</td>\n", $rrrow->NaamLid);
			printf("<td><input type='text' name='OMSCHRIJV_%d' value='%s'></td>\n", $rrrow->RecordID, $rrrow->OMSCHRIJV);
			printf("<td><input type='number' name='Bedrag_%d' step='0.01' value='%s'></td>\n", $rrrow->RecordID, number_format($rrrow->Bedrag, 2, ".", ""));
			if ($rrrow->Bedrag == 0) {
				printf("<td><input type='checkbox' name='ToonOpRekening_%d' value=1%s readonly></td>\n", $rrrow->RecordID, checked($rrrow->ToonOpRekening));
			} else {
				printf("<td><input type='checkbox' name='ToonOpRekening_%d' value=1%s></td>\n", $rrrow->RecordID, checked($rrrow->ToonOpRekening));
			}
			printf("<td class='trash'><a href='%s?tp=%s&amp;op=delete&rrid=%d'>&nbsp&nbsp&nbsp</a></td>", $_SERVER['PHP_SELF'], $_GET['tp'], $rrrow->RecordID);
			
			echo("</tr>\n");
		}
		echo("</table>\n");
		printf("<select name='RRnieuw'>\n<option value=-1>Regel toevoegen ...</option><option value=0>Geen lid</option>%s</select>\n", $i_lid->htmloptions(0, 2));
		echo("</div> <!-- Einde rekeningregelsmuteren -->\n");
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<input type='submit' value='Bewaren'>\n");
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		
		echo("</form>\n");
		
	}
	echo("</div> <!-- einde rekeningmuteren  -->\n");
	
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
			if ($_SESSION['settings']['mailing_rekening_regelnrsweglaten'] == 0) {
				$rrtxt .= sprintf("<tr><td>%d</td><td>%s</td><td>%s</td><td class='number'>%s</td></tr>\n", $ireg, $regel->OMSCHRIJV, $nl, fnBedrag($regel->Bedrag));
			} else {
				$rrtxt .= sprintf("<tr><td>%s</td><td>%s</td><td class='number'>%s</td></tr>\n", $regel->OMSCHRIJV, $nl, $b);
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