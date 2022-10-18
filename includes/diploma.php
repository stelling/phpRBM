<?php

function fnDiplomazaken() {
	global $currenttab, $currenttab2;
	
	fnDispMenu(2);
	
	if ($currenttab2 == "Basislijst") {
		fnDiplomalijstmuteren();
	} elseif ($currenttab2 == "Examenresultaten") {
		fnExamenResultaten();
	}

}

function fnDiplomalijstmuteren() {
	$i_dp = new cls_Diploma();
	$res = $i_dp->basislijst("", "", 0);
		
	echo("<p class='mededeling'>Dit gedeelte is nog niet klaar.</p>\n");
		
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
	echo(fnEditTable($res, $kols, "diplomaedit", "Muteren diploma's"));
	echo("</div> <!-- Einde diplomasmuteren -->\n");		
}

?>
