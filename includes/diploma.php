<?php

function fnDiplomazaken() {
	global $currenttab, $currenttab2;
	
	fnDispMenu(2);
	
	if ($currenttab2 == "Basislijst") {
		fnDiplomalijstmuteren();
	} elseif ($currenttab2 == "Leden per diploma") {
		fnExamenResultaten(-1, 0);
	}
}  # fnDiplomazaken

function fnDiplomalijstmuteren() {
	global $currenttab, $currenttab2, $dtfmt;
	
	$i_dp = new cls_Diploma();
	
	if (isset($_POST['nieuwdiploma'])) {
		$i_dp->add();
	}
	
	$res = $i_dp->basislijst("", "DP.Kode", 0);
		
	$kols[0]['type'] = "pk";
	$kols[0]['readonly'] = true;
	$kols[1]['headertext'] = "Code";
		
	$kols[2]['headertext'] = "Naam";

//	$kols[3]['headertext'] = "Type";
//	$kols[3]['bronselect'] = ARRTYPEDIPLOMA;
		
	$kols[4]['headertext'] = "Uitgegeven door";
	foreach ((new cls_Organisatie())->lijst() as $row) {
		$arrOrg[$row->Nummer] = $row->Naam;
	}
	$kols[4]['bronselect'] = $arrOrg;
		
	$kols[6]['headertext'] = "Volgnr";
	$kols[6]['type'] = "integer";
	$kols[6]['max'] = 999;
	
	$kols[11]['headertext'] = "Vervallen per";
	$kols[11]['type'] = "date";
		
	$kols[12]['headertext'] = "Einde uitgifte";
	$kols[12]['type'] = "date";
		
	$kols[17]['headertext'] = "Zelfservice?";
	$kols[17]['columnname'] = "Zelfservice";
	$kols[17]['type'] = "checkbox";
		
	$kols[18]['headertext'] = "Afd. spec.";
	$kols[18]['columnname'] = "Afdelingsspecifiek";
	$arrAfd[0] = "Geen";
	foreach ((new cls_Onderdeel())->lijst(1, "O.`Type`='A'", "", 0, "O.Kode") as $row) {
		$arrAfd[$row->RecordID] = $row->Kode;
	}
	$kols[18]['bronselect'] = $arrAfd;
	
	echo("<div id='diplomasmuteren'>\n");
	printf("<form method='post' action='%s?tp=%s/%s'>", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
	echo(fnEditTable($res, $kols, "diplomaedit", "Muteren diploma's"));
	echo("<div id='opdrachtknoppen'>\n");
	echo("<button type='submit' name='nieuwdiploma'>Diploma toevoegen</buton>\n");
	echo("</div> <!-- Einde opdrachtknoppen -->\n");
	echo("</form>");
	echo("</div> <!-- Einde diplomasmuteren -->\n");
	
	
}

?>
