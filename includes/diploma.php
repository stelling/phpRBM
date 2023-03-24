<?php

function fnDiplomazaken() {
	global $currenttab, $currenttab2;
	
	fnDispMenu(2);
	
	if ($currenttab2 == "Basislijst") {
		Diplomalijstmuteren();
	} elseif ($currenttab2 == "Details") {
		fnDiplomasMuteren();
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
	$kols[0]['columnname'] = "RecordID";
	$kols[0]['readonly'] = true;
	$kols[0]['headertext'] = "#";
	
	$kols[1]['headertext'] = "Code";
	$kols[1]['columnname'] = "Kode";
		
	$kols[2]['headertext'] = "Naam";
	$kols[2]['columnname'] = "Naam";

	$kols[6]['headertext'] = "Volgnr";
	$kols[6]['columnname'] = "Volgnr";
	$kols[6]['type'] = "integer";
	$kols[6]['max'] = 999;
		
	$kols[18]['headertext'] = "Afdelingsspec.";
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

function ExamensMuteren() {
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
	global $dtfmt;
	
	$i_dp = new cls_Diploma();
	$i_org = new cls_Organisatie();
	$i_ld = new cls_Liddipl();
	$i_lid = new cls_Lid();
	$i_gr = new cls_Groep($p_afdid);
	$i_lo = new cls_Lidond();
	$i_ex = new cls_Examen();
	
	if ($p_afdid > 0) {
		$f = sprintf("DP.Afdelingsspecifiek=%d", $p_afdid);
	} else {
		$f = "";
	}
	
	$dtfmt->setPattern(DTTEXT);
	
	$exid = $_POST['selecteerexamen'] ?? 0;
	$dpid = $_POST['selecteerdiploma'] ?? 0;
	
	if ($exid == -1) {
		$exid = $i_ex->add();
	}
		
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$i_ex->vulvars($exid);
		foreach ($_POST as $k => $v) {
			if (substr($k, 0, 12) == "ldtoevoegen_") {
				$dp = intval(str_replace("ldtoevoegen_", "", $k));
				if ($_POST[$k] > 0) {
					$i_ld->add($_POST[$k], $dp, "", $exid);
				}
			}
			
			if (substr($k, 0, 11) == "ledengroep_" and $exid > 0) {
				$gid = intval(str_replace("ledengroep_", "", $k));
				$i_gr->vulvars($p_afdid, $gid);
				if ($i_gr->diplomaid > 0) {
					$f_toev_groep = sprintf("LO.OnderdeelID=%1\$d AND LO.GroepID=%2\$d AND IFNULL(LO.Opgezegd, '9999-12-31') >= '%3\$s' AND LO.Lid NOT IN (SELECT LD.Lid FROM %4\$sLiddipl AS LD WHERE LD.Examen=%3\$d AND LD.DiplomaID=%5\$d)", $i_gr->afdid, $i_gr->grid, $i_ex->exid, TABLE_PREFIX, $i_gr->diplomaid);
					foreach ($i_lo->basislijst($f_toev_groep) as $lorow) {
						$i_ld->add($lorow->Lid, $i_gr->diplomaid, "", $i_ex->exid);
					}
					if ($dpid > 0) {
						$dpid = $i_gr->diplomaid;
					}
				}
			}
		}
	}
	
	echo("<div id=filter>\n");
	printf("<form action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
	if ($p_perexamen == 1) {
		echo("<label>Examen</label>");
		printf("<select name='selecteerexamen' onChange='this.form.submit();'>\n<option value=0>Selecteer examen ...</option>\n<option value=-1>*** Nieuw examen</option>\n%s</select>\n", $i_ex->htmloptions($exid));
	} else {
		$exid = 0;
		$i_ld->controle();
	}
	$i_ex->vulvars($exid);
	if ($dpid == 0) {
		$dpid = $i_ex->eerstediploma();
	}
	
	echo("<label>Diploma</label>");
	printf("<select name='selecteerdiploma' onChange='this.form.submit();'>\n<option value=-1>Selecteer diploma ...</option>\n%s</select>\n", $i_dp->htmloptions($dpid, -1, 0, 0, $f, 0, $i_ex->exid));
	if ($exid > 0 or $dpid > 0) {
		echo("<button type='submit'>Ververs scherm</button>\n");
	}
	echo("</div> <!-- Einde filter -->\n");
	
	if ($exid > 0) {
		echo("<div id='examenmuteren'>\n");
		printf("<label id='lblexamennummer'>Nummer</label><p id='examennummer'>%d</p>\n", $i_ex->exid);
		echo("<label>Examendatum</label>");
		if ($i_ex->aantalkandidaten > 0) {
			printf("<p>%s</p>\n", $dtfmt->format(strtotime($i_ex->exdatum)));
		} else {
			printf("<input type='date' id='Datum' value='%s' onBlur=\"savedata('examenmuteren', %d, this);\">\n", $i_ex->exdatum, $i_ex->exid);
		}
		printf("<label>Examenplaats</label><input type='text' id='Plaats' value='%s' onBlur=\"savedata('examenmuteren', %d, this);\" class='w30' maxlength=30>\n", $i_ex->explaats, $i_ex->exid);
		echo("</div> <!-- einde examenmuteren -->\n");
	}

	if (($exid > 0 or $p_perexamen == 0) and $dpid > 0) {
		
		$dprow = $i_dp->record($dpid);

		if (isset($_POST['nwe_groep']) and $_POST['nwe_groep'] > 0 and $p_afdid > 0) {
			$namen = "";
			$ldrows = $i_ld->perexamendiploma($exid, $i_dp->dpid);
			foreach ($ldrows as $ldrow) {
				$f2 = sprintf("LO.OnderdeelID=%d AND LO.Lid=%d AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE() AND LO.Functie=0", $p_afdid, $ldrow->Lid);
				$loid = $i_lo->max("RecordID", $f2);
				if ($loid > 0) {
					if ($i_lo->update($loid, "GroepID", $_POST['nwe_groep']) == true) {
						if (strlen($namen) > 0) {
							$namen .= ", ";
						}
						$namen .= $i_lo->lidnaam;
					}
				}
			}
			if (strlen($namen) > 0) {
				$i_gr->vulvars($p_afdid, $_POST['nwe_groep']);
				$mess = sprintf("%s zijn naar groep %s verplaatst", $namen, $i_gr->groms);
			} else {
				$mess = "Geen leden verplaatst.";
			}
			(new cls_Logboek())->add($mess, 19, 0, 1);
		}
	}
	
	if (($exid > 0 or $p_perexamen == 0)) {

		echo("<div id='examenresultaten'>\n");
		
		$query = sprintf("SELECT DISTINCT LD.DiplomaID AS DiplomaID FROM %1\$sLiddipl AS LD INNER JOIN %1\$sDiploma AS DP ON LD.DiplomaID=DP.RecordID WHERE ", TABLE_PREFIX);
		if ($p_perexamen == 1 and $dpid <= 0) {
			$query = sprintf("SELECT DISTINCT LD.DiplomaID FROM %1\$sLiddipl AS LD INNER JOIN %1\$sDiploma AS DP ON LD.DiplomaID=DP.RecordID WHERE ", TABLE_PREFIX);
			$query .= sprintf("LD.Examen=%d ORDER BY DP.Volgnr, DP.Naam;", $i_ex->exid);
		} else {		
			$query = sprintf("SELECT DP.RecordID AS DiplomaID FROM %sDiploma AS DP WHERE DP.RecordID=%d;", TABLE_PREFIX, $dpid);
		}

		foreach ($i_ld->execsql($query)->fetchAll() as $dipl) {
			$i_dp->vulvars($dipl->DiplomaID);
					
			$vg = 0; // Volgende groep
			$aant_vg = 0; // Aantal leden dat van groep verplaatst kan worden
			if ($i_dp->dpvolgende > 0) {
				$f = sprintf("GR.OnderdeelID=%d AND GR.DiplomaID=%d", $p_afdid, $i_dp->dpvolgende);
				$vg = $i_gr->max("RecordID", $f);
			}
	
			echo("<div class='kandidatengroep'>\n");
			echo("<table>\n");
			$t = "";
			if ($p_perexamen == 1) {
				$ldrows = $i_ld->perexamendiploma($exid, $i_dp->dpid);
				if (count($ldrows) > 3) {
					$t = sprintf(" title='%d rijen'", count($ldrows));
				}
				printf("<caption%s>%s</caption>\n", $t, $i_dp->dpnaam);
			} else {
				$ldrows = $i_ld->overzichtperdiploma($i_dp->dpid);
				if (count($ldrows) > 3) {
					$t = sprintf(" title='%d rijen'", count($ldrows));
				}
				printf("<caption%s>%s</caption>\n", $t, $i_dp->dpnaam);
				$exdatum = date("Y-m-d");
			}
			
			echo("<tr><th>Lid</th>");
			echo("<th>Geboortedatum</th>");
			if ($p_perexamen == 0) {
				echo("<th>Behaald op</th>");
			}
		
			if ($p_perexamen == 0) {
				echo("<th>Diplomanr</th>");
				echo("<th>Geldig tot</th>");
			}
			echo("<th></th><th></th></tr>\n");

			$naam_vg = "";
			$aant_vg = 0; // Aantal leden dat van groep verplaatst kan worden
			foreach ($ldrows as $ldrow) {
				$cl = "";
				$dd = "";
				if ($i_dp->dpvoorganger > 0) {
					$f = sprintf("LD.Lid=%d AND LD.DiplomaID=%d", $ldrow->Lid, $i_dp->dpvoorganger);
					if ($i_ld->aantal($f) == 0) {
						$dd = sprintf("%s ontbreekt", $i_dp->naam($i_dp->dpvoorganger));
						$cl = " voorgangerontbreekt";
					}
				}

				foreach ($i_ld->dubbelediplomas($ldrow->RecordID) as $ddrow) {
					if (strlen($dd) > 0) {
						$dd .= ", ";
					} else {
						$dd = "ook op ";
					}
					$dd .= date("d-m-Y", strtotime($ddrow->DatumBehaald));
					$cl .= " dubbeldiploma";
				}
			
				if (strlen($cl) > 0) {
					$cl = sprintf(" class='%s'", trim($cl));
				}
				printf("<tr><td id='naam_%2\$d'>%1\$s</td><td>%3\$s</td>", $ldrow->NaamLid, $ldrow->RecordID, $dtfmt->format(strtotime($ldrow->GEBDATUM)));
				if ($p_perexamen == 0) {
					printf("<td%s><input type='date' id='DatumBehaald_%d' value='%s'></td>", $cl, $ldrow->RecordID, $ldrow->DatumBehaald);
				}
				if ($p_perexamen == 0) {
					printf("<td><input type='text' id='Diplomanummer_%d' class='w25' value='%s' maxlength=25></td>", $ldrow->RecordID, $ldrow->Diplomanummer);
					printf("<td><input type='date' id='LicentieVervallenPer_%d' value='%s'></td>", $ldrow->RecordID, $ldrow->LicentieVervallenPer);
				}

				$jsdo = sprintf("OnClick=\"liddipl_verw(%d);\"", $ldrow->RecordID);
				printf("<td><img src='%s' alt='Verwijderen' title='Verwijderen %s' %s></td>", BASE64_VERWIJDER_KLEIN, htmlentities($ldrow->NaamLid), $jsdo);
		
				if (strlen($dd) > 0) {
					printf("<td>%s</td>", $dd);
				}
				echo("</tr>\n");
				$f = sprintf("LO.Lid=%d AND LO.OnderdeelID=%d AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE() AND Functie=0", $ldrow->Lid, $p_afdid);
				if ($p_afdid > 0 and $vg > 0 and $vg <> $i_lo->max("GroepID", $f)) {
					$aant_vg++;
					$naam_vg = $ldrow->NaamLid;
				}
			}
			echo("</table>\n");
	
			if ($i_dp->eindeuitgifte >= $i_ex->exdatum) {
				echo("<div class='clear'></div>\n");
				printf("<select name='ldtoevoegen_%d' onChange='this.form.submit();'><option value=0>Lid toevoegen ....</option>\n%s</select>\n", $i_dp->dpid, $i_lid->htmloptions(-1, 1, "", $i_ex->exdatum, $p_afdid));
			}

			if ($aant_vg > 0 and $vg > 0) {
				$i_gr->vulvars($p_afdid, $vg);
				if ($aant_vg == 1) {
					$ol = $naam_vg;
				} else {
					$ol = sprintf("%d leden", $aant_vg);
				}
				echo("<div class='clear'></div>\n");
				printf("<button type='submit' name='nwe_groep' value=%d>%s naar groep %s</button>\n", $vg, $ol, $i_gr->groms);
			}
			echo("</div> <!-- Einde kandidatengroep -->\n");
		}
	}
	echo("</div> <!-- Einde examenresultaten -->\n");
	
	if ($p_afdid > 0 and $exid > 0) {
		$f = sprintf("GR.OnderdeelID=%d AND GR.DiplomaID > 0", $p_afdid);
		echo("<div id='groepenaanexamentoevoegen'>\n");
		echo("<label>Groep toevoegen</label>\n");
		foreach ($i_gr->basislijst($f, "GR.Omschrijving") as $grrow) {
			$f_toev_groep = sprintf("LO.OnderdeelID=%1\$d AND LO.GroepID=%2\$d AND IFNULL(LO.Opgezegd, '9999-12-31') >= '%3\$s' AND LO.Lid NOT IN (SELECT LD.Lid FROM %4\$sLiddipl AS LD WHERE LD.Examen=%3\$d AND LD.DiplomaID=%5\$d)", $p_afdid, $grrow->RecordID, $exid, TABLE_PREFIX, $grrow->DiplomaID);
			$al = $i_lo->aantal($f_toev_groep);
			if ($al > 0) {
				printf("<button name='ledengroep_%d'>%s</button>", $grrow->RecordID, $grrow->Omschrijving);
			}
		}
		echo("</div> <!-- Einde groepenaanexamentoevoegen -->\n");
	}
	echo("</form>\n");
	
?>
<script>
		
	$("input[id^=Diplomanummer_]").blur(function(){
		savedata("liddipl", 0, this);
	});
	
	function liddipl_verw(p_ldid) {
		$("#naam_" + p_ldid).addClass("deleted");
		deleterecord("verw_liddipl", p_ldid);
	}

</script>
<?php
}  # fnExamenResultaten

function fnDiplomasMuteren($p_afdid=-1) {
	global $dtfmt;
	
	$i_dp = new cls_Diploma();
	$i_org = new cls_Organisatie();
	$i_ld = new cls_Liddipl();
	$i_lid = new cls_Lid();
	$i_gr = new cls_Groep($p_afdid);
	$i_lo = new cls_Lidond();
	
	if ($p_afdid > 0) {
		$f = sprintf("DP.Afdelingsspecifiek=%d", $p_afdid);
	} else {
		$f = "";
	}
	$dprows = $i_dp->basislijst($f, "DP.Kode");
	
	$dpid = $_POST['selecteerdiploma'] ?? 0;
	
	printf("<form action='%s?%s' id=filter method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
	echo("<label>Selecteer diploma</label>");
	printf("<select name='selecteerdiploma' onChange='this.form.submit();'>\n<option value=-1>Selecteer diploma ...</option>\n%s</select>\n", $i_dp->htmloptions($dpid, -1, 0, 1, $f));
	echo("</form>\n");
	
	if ($dpid > 0) {
		
		echo("<div id='diplomamuteren'>\n");
	
		$dprow = $i_dp->record($dpid);
	
		printf("<label>RecordID</label><input type='text' id='RecordID' class='number' readonly disabled value=%d>\n", $dprow->RecordID);
	
		printf("<label>Naam</label><input type='text' id='Naam' class='w75' maxlength=75 value=\"%s\">\n", str_replace("\"", "'", $dprow->Naam));
		printf("<label>Code</label><input type='text' id='Kode' class='w10' maxlength=10 value=\"%s\">\n", $dprow->Kode);
		printf("<label id='lblvolgnr'>Volgnummer</label><input type='number' id='Volgnr' value=%d class='num3'>\n", $dprow->Volgnr);
	
		printf("<label>Type</label><select id='Type'>%s</select>\n", fnOptionsFromArray(ARRTYPEDIPLOMA, $dprow->Type));
		printf("<label id='lbluitgegevendoor'>Uitgegeven door</label><select id='ORGANIS'>%s</select>\n", $i_org->htmloptions(1, $dprow->ORGANIS));
		$f = sprintf("DP.RecordID<>%d AND DP.Afdelingsspecifiek=%d AND IFNULL(DP.Vervallen, '9999-12-31') > CURDATE()", $dpid, $dprow->Afdelingsspecifiek);
		printf("<label id='lblvoorganger'>Voorganger</label><select id='VoorgangerID'><Option value=0>Geen</option>\n%s</select>\n", $i_dp->htmloptions($dprow->VoorgangerID, 0, 0, 0, $f, 1));
	
		printf("<label id='lbleindeuitgifte'>Einde uitgifte</label><input type='date' id='EindeUitgifte' value='%s'>\n", $dprow->EindeUitgifte);
		
		if ($p_afdid <= 0) {
			printf("<label id='lblgeldigheid'>Geldigheid</label><input type='number' id='GELDIGH' value=%d class='num2' max=99><p>maanden (0 = onbeperkt)</p>\n", $dprow->GELDIGH);
			printf("<label id='lblhistorie'>Bewaren na verlopen geldigheid</label><input type='number' id='HistorieOpschonen' value=%d class='num3' max=999><p>maanden (0 = onbeperkt)</p>\n", $dprow->HistorieOpschonen);
			printf("<label id='lblvervallenper'>Vervallen per</label><input type='date' id='Vervallen' value='%s'>\n", $dprow->Vervallen);
			printf("<label id='lblzelfservice'>Onderdeel van de zelfservice?</label><input type='checkbox' id='Zelfservice' value=1 %s title='Is dit diploma beschikbaar in de zelfservice?'>\n", checked($dprow->Zelfservice));
		}
	
		echo("</div> <!-- Einde diplomamuteren -->\n");
	
?>
<script>

	$("#Naam, #Kode, #Volgnr, #GELDIGH, #HistorieOpschonen").blur(function() {
		savedata("diplomaedit", $("#RecordID").val(), this);
	});
	
	$("#ORGANIS, #VoorgangerID, #Type").blur(function() {
		savedata("diplomaedit", $("#RecordID").val(), this);
	});
		
	$("#Zelfservice").change(function() {
		savedata("diplomaedit", $("#RecordID").val(), this);
	});
	
	$("#EindeUitgifte, #Vervallen").blur(function(){
		savedata("diplomaedit", $("#RecordID").val(), this);
		this.form.submit();
	});

</script>
<?php
	}
}  # fnDiplomasMuteren


?>
