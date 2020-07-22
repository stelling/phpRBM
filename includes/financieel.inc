<?php

function fnRekeningen() {
	global $currenttab, $currenttab2;
	
	fnDispMenu(2);
	
	if ($currenttab2 == "Muteren") {
		fnRekeningMuteren();
	}
}

function fnRekeningMuteren() {
	$scherm = "M";
	$i_rk = new cls_Rekening();
	$i_lid = new cls_Lid();
	
	echo("<div id='rekeningmuteren'>\n");
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
	} elseif ($scherm == "M")  {
		$reknr = 2020010;
		$row = $i_rk->record($reknr);
		
		printf("<label>Rekeningnummer</label><p>%d</p>\n", $row->Nummer);
		printf("<label class='k2'>Seizoen</label>");
		printf("<label>Omschrijving</label><input type='text' name='omschrijving' value='%s'>\n", $row->OMSCHRIJV);
		printf("<label>Tenaamstelling rekening</label><input type='text' name='debnaam' value='%s'>\n", $row->DEBNAAM);
		printf("<label>Gekoppeld aan lid</label><select name='lid'><option value=-1></option>%s</select>", $i_lid->htmloptions($row->Lid));
		printf("<label>Betaald door</label><select name='lid'><option value=-1>(gekoppeld lid)</option>%s</select>", $i_lid->htmloptions($row->Lid));
		printf("<label>Rekeningdatum</label><input type='date' name='datum' value='%s'>", $row->Datum);
		printf("<label>Betaaltermijn</label><input type='number' name='betaaldag' value=%d>", $row->BETAALDAG);
		printf("<label>Betaaltermijn</label><input type='number' name='betaaldag' value=%d>", $row->BETAALDAG);
	}
	echo("</div> <!-- einde rekeningmuteren  -->\n");
}

function RekeningDetail($nr) {
	
	$rk = db_rekening("hoofd", $nr);
	
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
	
	if (($rk->Bedrag - $rk->Betaald) < 0.50) {
		$content = removetextblock($content, "<!-- NietOpVolledigBetaald -->", "<!-- /NietOpVolledigBetaald -->");
	}
	
	$content = str_ireplace("[%NAAMDEBITEUR%]", $rk->DEBNAAM, $content);
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
	$content = str_ireplace("[%OPENSTAAND%]", fnBedrag($rk->Bedrag - $rk->Betaald), $content);
	$content = str_ireplace("[%BANKREKENING%]", $rk->Bankrekeningnummer, $content);
	$content = str_ireplace("[%UITERSTEBETAALDATUM%]", strftime("%e %B %Y", strtotime($rk->UitersteBetaaldatum)), $content);
	$content = str_ireplace("[%VANAFADRES%]", $_SESSION['settings']['mailing_rekening_from_adres'], $content);
	$content = str_ireplace("[%VANAFNAAM%]", $_SESSION['settings']['mailing_rekening_from_naam'], $content);
	$content = str_ireplace("[%NAAMVERENIGING%]", $_SESSION['settings']['naamvereniging'], $content);
	$content = str_ireplace("[%NAAMWEBSITE%]", $_SESSION['settings']['naamwebsite'], $content);
	$content = str_ireplace("[%URLWEBSITE%]", $_SERVER["HTTP_HOST"], $content);
	
	$rr = (new cls_Rekeningregel())->perrekening($nr);
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
	
	$ret = "<option value='*'>Alle</option>\n";
	$query = sprintf("SELECT DISTINCT GBR.Kode, CONCAT(GBR.Kode, ' - ', GBR.OMSCHRIJV) AS Oms
				 FROM %1\$sMutatie AS M INNER JOIN %1\$sGBR AS GBR ON M.GBR = GBR.Kode
				 ORDER BY GBR.Kode;", TABLE_PREFIX);
	$result = fnQuery($query);
	foreach ($result->fetchAll() as $row) {
		if ($val_gbrfilter == $row->Kode) {
			$s = " selected";
		} else {
			$s = "";
		}
		$ret .= sprintf('<option%s value="%s">%s</option>\n', $s, $row->Kode, $row->Oms);
	}
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