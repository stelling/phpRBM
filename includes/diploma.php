<?php

function fnDiplomazaken() {
	global $currenttab, $currenttab2;
	
	fnDispMenu(2);
	
	if ($currenttab2 == "Basislijst") {
		Diplomalijstmuteren();
	} elseif ($currenttab2 == "Leden per diploma") {
		fnExamenResultaten(-1, 0);
	} elseif ($currenttab2 == "Examens muteren") {
		examensMuteren();
	}
}  # fnDiplomazaken

function Diplomalijstmuteren() {
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
}  # Diplomalijstmuteren

function examensMuteren() {
	global $currenttab, $currenttab2;
	
	$i_ex = new cls_Examen();
	
	if (isset($_POST['nieuwExamen'])) {
		$i_ex->add();
	}
	
	$res = $i_ex->lijst(0);
	
	$kols[0]['columnname'] = "Nummer";
	$kols[0]['headertext'] = "Nummer";
	$kols[0]['type'] = "pk";
	$kols[0]['readonly'] = true;
	
	$kols[1]['columnname'] = "Datum";
	$kols[1]['type'] = "date";
	$kols[1]['cond_ro'] = "AantalBehaald";
	
	$kols[2]['columnname'] = "Omschrijving";
	$kols[2]['headertext'] = "Omschrijving";
	
	$kols[3]['columnname'] = "Plaats";
	$kols[3]['headertext'] = "Plaats";
	
	$kols[4]['columnname'] = "Begintijd";
	$kols[4]['type'] = "tijd";
	
	$kols[5]['columnname'] = "Eindtijd";
	$kols[5]['type'] = "tijd";
	
	$kols[6]['columnname'] = "AantalBehaald";
	$kols[6]['headertext'] = "Aantal";
	$kols[6]['type'] = "integer";
	$kols[6]['readonly'] = true;
	
	
	printf("<form method='post' action='%s?tp=%s/%s'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
	echo("<button name='nieuwExamen' type='submit'>Nieuw examen</button>\n");
	echo("</form>\n");
	echo(fnEditTable($res, $kols, "examensmuteren", "Muteren examens"));
	
}  # examensMuteren

function fnExamenResultaten($p_afdid=-1, $p_perexamen=1) {
	
	$i_dp = new cls_Diploma();
	$i_org = new cls_Organisatie();
	$i_ld = new cls_Liddipl();
	$i_lid = new cls_Lid();
	
	if ($p_afdid > 0) {
		$f = sprintf("DP.Afdelingsspecifiek=%d", $p_afdid);
	} else {
		$f = "";
	}
	$dprows = $i_dp->basislijst($f, "DP.Kode");
	
	$dpid = $_POST['selecteerdiploma'] ?? 0;

	if (isset($_POST['exdatum'])) {
		$exdatum = $_POST['exdatum'];
	} elseif ($dpid > 0) {
		$f2 = sprintf("LD.DiplomaID=%d", $dpid);
		$exdatum = $i_ld->max("DatumBehaald", $f2);
		if (strlen($exdatum) < 10) {
			$exdatum = date("Y-m-d");
		}
	} else {
		$exdatum = date("Y-m-d");
	}
	$explaats = $_POST['explaats'] ?? "";
	$eindeuitgifte = $_POST['eindeuitgifte'] ?? "9999-12-31";
	if (strlen($eindeuitgifte) < 8 or $eindeuitgifte < "1970-01-01") {
		$eindeuitgifte = "9999-31-12";
	}
	
	if (isset($_POST['ldtoevoegen']) and $_POST['ldtoevoegen'] > 0) {
		$i_ld->add($_POST['ldtoevoegen'], $dpid, $exdatum, $explaats);
	}
	
	echo("<div id=filter>\n");
	printf("<form action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
	echo("<label>Selecteer diploma</label>");
	printf("<select name='selecteerdiploma' onChange='this.form.submit();'>\n<option value=-1>Selecteer diploma ...</option>\n%s</select>\n", $i_dp->htmloptions($dpid, -1, 0, 0, $f));
	echo("</div> <!-- Einde filter -->\n");
	
	echo("<div id='diplomamuteren'>\n");
	
	if ($dpid > 0) {
	
		$dprow = $i_dp->record($dpid);
	
		printf("<label>RecordID</label><input type='text' id='RecordID' class='number' readonly disabled value=%d>\n", $dprow->RecordID);
	
		printf("<label>Naam</label><input type='text' id='Naam' class='w75' maxlength=75 value=\"%s\">\n", str_replace("\"", "'", $dprow->Naam));
		printf("<label class='k2'>Code</label><input type='text' id='Kode' class='w10' maxlength=10 value=\"%s\">\n", $dprow->Kode);
		printf("<label class='k2'>Volgnummer</label><input type='number' id='Volgnr' value=%d class='d8'>\n", $dprow->Volgnr);
	
		printf("<label>Type</label><select id='Type'>%s</select>\n", fnOptionsFromArray(ARRTYPEDIPLOMA, $dprow->Type));
		printf("<label class='k2'>Uitgegeven door</label><select id='ORGANIS'>%s</select>\n", $i_org->htmloptions(1, $dprow->ORGANIS));
	
		printf("<label>Einde uitgifte</label><input type='date' id='EindeUitgifte' name='EindeUitgifte' value='%s'>\n", $dprow->EindeUitgifte);
		printf("<label class='k2'>Geldigheid</label><input type='number' id='GELDIGH' value=%d class='num2' max=99><p>maanden (0 = onbeperkt)</p>\n", $dprow->GELDIGH);
		printf("<label class='k2'>Bewaren na verlopen geldigheid</label><input type='number' id='HistorieOpschonen' value=%d class='num3' max=999><p>maanden (0 = onbeperkt)</p>\n", $dprow->HistorieOpschonen);
	
		echo("</div> <!-- Einde diplomamuteren -->\n");
	
		echo("<div class='clear'></div>\n");
	
		echo("<div id='examenresultaten'>\n");
	
		if ($p_perexamen == 1) {
			echo("<h2>Geslaagden</h2>\n");
		} else {
			printf("<h2>Leden met %s</h2>\n", strtolower(ARRTYPEDIPLOMA[$dprow->Type]));
		}
		printf("<label>Examendatum</label><input type='date' name='exdatum' value='%s' onBlur='this.form.submit();'>\n", $exdatum);
		printf("<label>Examenplaats</label><input type='text' name='explaats' value='%s' class='w22' onBlur='this.form.submit();'>\n", $explaats);
	
		echo("<div class='clear'></div>\n");
	
		echo("<table>\n");
	
		echo("<tr><th>Lid</th>");
		if ($p_perexamen == 0) {
			echo("<th>Behaald op</th>");
			$ldrows = $i_ld->overzichtperexamen("", $dpid);
		} else {
			$ldrows = $i_ld->overzichtperexamen($exdatum, $dpid);
		}
		echo("<th>Examenplaats</th>");
		if ($p_perexamen == 0) {
			echo("<th>Diplomanr</th>");
			echo("<th>Geldig tot</th>");
		}
		echo("<th></th></tr>\n");
	
		foreach ($ldrows as $ldrow) {
			printf("<tr><td id='naam_%2\$d'>%1\$s</td>", $ldrow->NaamLid, $ldrow->RecordID);
			if ($p_perexamen == 0) {
				printf("<td><input type='date' id='DatumBehaald_%d' value='%s'></td>", $ldrow->RecordID, $ldrow->DatumBehaald);
			}
			printf("<td><input type='text' id='EXPLAATS_%d' value='%s'></td>", $ldrow->RecordID, $ldrow->EXPLAATS);
			if ($p_perexamen == 0) {
				printf("<td><input type='text' id='Diplomanummer_%d' value='%s'></td>", $ldrow->RecordID, $ldrow->Diplomanummer);
				printf("<td><input type='date' id='LicentieVervallenPer_%d' value='%s'></td>", $ldrow->RecordID, $ldrow->LicentieVervallenPer);
			}

			$jsdo = sprintf("OnClick=\"liddipl_verw(%d);\"", $ldrow->RecordID);
			printf("<td><img src='%s' alt='Verwijderen' title='Verwijderen %s' %s></td>", BASE64_VERWIJDER_KLEIN, htmlentities($ldrow->NaamLid), $jsdo);
		
			$dd = "";
			foreach ($i_ld->dubbelediplomas($ldrow->Lid, $ldrow->DiplomaID, $ldrow->RecordID) as $ddrow) {
				if (strlen($dd) > 0) {
					$dd .= ", ";
				} else {
					$dd = "ook op ";
				}
				$dd .= date("d-m-Y", strtotime($ddrow->DatumBehaald));
			}
			if (strlen($dd) > 0) {
				printf("<td>%s</td>", $dd);
			}
			echo("</tr>\n");
		}
		echo("</table>\n");
	
		if ($eindeuitgifte >= $exdatum and $exdatum <= date("Y-m-d")) {
			printf("<select name='ldtoevoegen' onChange='this.form.submit();'><option value=0>Lid toevoegen ....</option>\n%s</select>\n", $i_lid->htmloptions(-1, 1, "", $exdatum, $p_afdid));
		}
	}
	echo("</form>\n");
	echo("</div> <!-- Einde examenresultaten -->\n");
?>
<script>

	$("#Naam, #Kode, #Volgnr, #GELDIGH, #HistorieOpschonen").blur(function(){
		savedata("diplomaedit", $("#RecordID").val(), this);
	});
	
	$("#ORGANIS, #Type").blur(function(){
		savedata("diplomaedit", $("#RecordID").val(), this);
	});
	
	$("#EindeUitgifte").blur(function(){
		savedata("diplomaedit", $("#RecordID").val(), this);
		this.form.submit();
	});
	
	$("input[id^=EXPLAATS_], input[id^=Diplomanummer_]").blur(function(){
		savedata("liddipl", 0, this);
	});
	
	function liddipl_verw(p_ldid) {
		$("#naam_" + p_ldid).addClass("deleted");
		deleterecord("verw_liddipl", p_ldid);
	}

</script>
<?php	
}  # fnExamenResultaten

?>
