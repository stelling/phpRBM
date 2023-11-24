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
	} elseif ($currenttab2 == "Examenonderdelen") {
		fnExamenonderdelen();
	}
}  # fnDiplomazaken

function Diplomalijstmuteren() {
	global $currenttab, $currenttab2, $dtfmt;

	$i_dp = new cls_Diploma();
	$i_ond = new cls_Onderdeel();
	
	if (isset($_POST['nieuwdiploma'])) {
		$i_dp->add();
	}
	
	$res = $i_dp->basislijst("", "DP.Kode", 0);
		
	$kols[] = array ('type' => "pk", 'columnname' => "RecordID", 'readonly' => true, 'headertext' => "#");
	$kols[]['columnname'] = "Kode";
	$kols[]['columnname'] = "Naam";
	$kols[] = array('columnname' => "Volgnr", 'type' => "integer", 'max' => 999);
	
	$f = "O.`Type`='A'";
	if ($i_ond->aantal($f) > 0) {
		$arrAfd[0] = "Geen";
		foreach ($i_ond->lijst(1, $f, "", 0, "O.Kode") as $row) {
			$arrAfd[$row->RecordID] = $row->Kode;
		}
		$kols[] = array('headertext' => "Afdelingsspec.", 'columnname' => "Afdelingsspecifiek", 'bronselect' => $arrAfd);
	}
	
	echo("<div id='diplomasmuteren'>\n");
	printf("<form method='post' action='%s?tp=%s/%s'>", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
	echo(fnEditTable($res, $kols, "diplomaedit", "Muteren diploma's"));
	echo("<div id='opdrachtknoppen'>\n");
	printf("<button type='submit' class='%s' name='nieuwdiploma'>%s Diploma</buton>\n", CLASSBUTTON, ICONTOEVOEGEN);
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
	echo("<button name='nieuwExamen' type='submit' class='btn btn-light'>Nieuw examen</button>\n");
	echo("</form>\n");
	echo(fnEditTable($res, $kols, "examensmuteren", "Muteren examens"));
	
}  # examensMuteren

function fnExamenResultaten($p_afdid=-1, $p_perexamen=1) {
	global $dtfmt, $currenttab;
	
	$i_ld = new cls_Liddipl();
	$i_lid = new cls_Lid();
	$i_gr = new cls_Groep($p_afdid);
	$i_lo = new cls_Lidond();
	$i_ex = new cls_Examen();
	$i_eo = new cls_Examenonderdeel();
	
	$isrn = false;
	if ($p_afdid > 0) {
		$i_o = new cls_Onderdeel($p_afdid);
		if ($i_o->organisatie == 1) {
			$isrn = true;
		}
		$i_o = null;
		$f = sprintf("DP.Afdelingsspecifiek=%d", $p_afdid);
	} else {
		$f = "";
	}
	
	$exid = $_POST['selecteerexamen'] ?? 0;
	$dpid = $_POST['selecteerdiploma'] ?? 0;
	$i_dp = new cls_Diploma($dpid);

	$i_ex->vulvars($exid);
	if ($i_ex->exid > 0) {
		$i_lid->per = $i_ex->exdatum;
	}

	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['btnExamenToevoegen'])) {
			$exid = $i_ex->add();
			$f = sprintf("AK.OnderdeelID=%d AND AK.Datum >= CURDATE() AND AK.Activiteit=1", $p_afdid);
			$i_ex->update($exid, "Datum", (new cls_Afdelingskalender())->min("Datum", $f));
			$i_ex->update($exid, "OnderdeelID", $p_afdid);
		} elseif (isset($_POST['btnAllenGeslaagd'])) {
			$s = explode("-", $_POST['btnAllenGeslaagd']);
			$f = sprintf("LD.Examen=%d AND LD.DiplomaID=%d AND LD.Geslaagd=0", $s[0], $s[1]);
			foreach ($i_ld->basislijst($f) as $ldrow) {
				$i_ld->update($ldrow->RecordID, "Geslaagd", 1);
			}
		}
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
			
			if (substr($k, 0, 12) == "proefexamen_" and $i_ex->exid > 0) {
				$id = intval(str_replace("proefexamen_", "", $k));
				$f = sprintf("LD.Examen=%d AND LD.Geslaagd=1", $id);
				foreach ($i_ld->basislijst($f) as $ldrow) {
					$f = sprintf("LD.Examen=%d AND LD.Lid=%d AND LD.DiplomaID=%d", $i_ex->exid, $ldrow->Lid, $ldrow->DiplomaID);
					if ($i_ld->aantal($f) == 0) {
						$i_ld->add($ldrow->Lid, $ldrow->DiplomaID, "", $i_ex->exid);
					}
				}
			}
			
		}
	}
	
	echo("<div id='filter'>\n");
	printf("<form action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
	if ($p_perexamen == 1) {
		echo("<label class='form-label'>Examen</label>");
		$fex = sprintf("(EX.OnderdeelID=0 OR EX.OnderdeelID=%d)", $p_afdid);
		printf("<select name='selecteerexamen' class='form-select' onChange='this.form.submit();'>\n<option value=0>Selecteer examen ...</option>\n%s</select>\n", $i_ex->htmloptions($exid, $fex));
	} else {
		$exid = 0;
		$i_ld->controle();
	}
	$i_ex->vulvars($exid);
	
	printf("<select name='selecteerdiploma' class='form-select' onChange='this.form.submit();'>\n<option value=-1>Selecteer diploma ...</option>\n%s</select>\n", $i_dp->htmloptions($dpid, -1, 0, 0, "", 0, $i_ex->exid));
	if ($exid > 0) {
		printf("<button type='submit' class='%s'>%s Ververs scherm</button>\n", CLASSBUTTON, ICONVERVERS);
	}
	echo("</div> <!-- Einde filter -->\n");
	$dtfmt->setPattern(DTTEXT);
	
	if ($exid > 0) {
		$i_ld->controle($exid);
		echo("<div id='examenmuteren'>\n");
		printf("<label id='lblexamennummer' class='form-label'>Nummer</label><p id='examennummer'>%d</p>\n", $i_ex->exid);
		echo("<label class='form-label'>Examendatum</label>");
		if ($i_ex->aantalkandidaten > 0 and 1 == 2) {
			$dtfmt->setPattern(DTTEXTWD);
			printf("<p>%s</p>\n", $dtfmt->format(strtotime($i_ex->exdatum)));
		} else {
			printf("<input type='date' id='Datum' value='%s' title='Examendatum' onBlur=\"savedata('examenmuteren', %d, this);\">\n", $i_ex->exdatum, $i_ex->exid);
		}
		printf("<label class='form-label'>Examenplaats</label><input type='text' id='Plaats' value='%s' placeholder='Examenplaats' onBlur=\"savedata('examenmuteren', %d, this);\" class='w30' maxlength=30>\n", $i_ex->explaats, $i_ex->exid);
		$f = sprintf("LD.Examen=%d", $exid);
		if ($i_ld->aantal($f) > 0 and 1 == 2) {
			$d = " disabled";
		} else {
			$d = "";
		}
		printf("<label class='form-check-label'>Proefexamen</label><input type='checkbox' class='form-check-input' id='Proefexamen' value=1%s%s title='Proefexamen?' onBlur=\"savedata('examenmuteren', %d, this);\">\n", checked($i_ex->proef), $d, $i_ex->exid);
		if ($isrn) {
			printf("<label class='form-label'>Starttijd</label><input type='text' id='Begintijd' value='%s' onBlur=\"savedata('examenmuteren', %d, this);\" class='w5' maxlength=5>\n", $i_ex->begintijd, $i_ex->exid);
		}
		echo("</div> <!-- einde examenmuteren -->\n");
		$i_ld->controle($exid);
	}

	if ($exid > 0) {
		/*
		-- Verplaatsen geslaagden naar een nieuwe groep is naar 'Groepsindeling muteren' verplaatst
		if (isset($_POST['nwe_groep']) and strlen($_POST['nwe_groep']) > 0 and $p_afdid > 0) {
			$grid = intval(explode("-", $_POST['nwe_groep'])[0]);
			$vdpid = intval(explode("-", $_POST['nwe_groep'])[1]);
			$namen = "";
			$i_gr->vulvars($p_afdid, $grid);
			$i_dp->vulvars($vdpid);
			$ldrows = $i_ld->perexamendiploma($exid, $vdpid, 1);
			foreach ($ldrows as $ldrow) {
				$f = sprintf("LO.Lid=%d AND LO.OnderdeelID=%d AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE() AND LO.Functie=0", $ldrow->Lid, $p_afdid);
				$loid = $i_lo->max("RecordID", $f);
				$huid_gr = $i_lo->max("GroepID", $f);
				$f = sprintf("GR.RecordID=%d", $huid_gr);
				$huid_gr_dipl = $i_gr->max("DiplomaID", $f);
				if ($huid_gr_dipl <> $i_dp->dpvolgende and $i_dp->dpvolgende > 0) {
					if ($i_lo->update($loid, "GroepID", $grid) == true) {
						if (strlen($namen) > 0) {
							$namen .= ", ";
						}
						$namen .= $i_lo->lidnaam;
					}
				}
			}
		}
		*/
	} elseif ($p_perexamen == 1) {
		echo("<div id='opdrachtknoppen'>\n");
		printf("<button type='submit' class='%s' name='btnExamenToevoegen'>%s Examen</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
	}
	
	if (($exid > 0 or $p_perexamen == 0)) {

		echo("<div id='examenresultaten' class='row'>\n");
		
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
	
			if ($p_perexamen == 1) {
				echo("<div class='kandidatengroep col'>\n");
			}
			printf("<table class='%s'>\n", TABLECLASSES);
			$t = "";
			if ($p_perexamen == 1) {
				$ldrows = $i_ld->perexamendiploma($exid, $i_dp->dpid);
				if (count($ldrows) > 1) {
					$t = sprintf(" title='%d rijen'", count($ldrows));
				}
				printf("<caption%s>%s</caption>\n", $t, $i_dp->dpnaam);
			} else {
				$ldrows = $i_ld->overzichtperdiploma($i_dp->dpid);
				if (count($ldrows) > 1) {
					$t = sprintf(" title='%d rijen'", count($ldrows));
				}
				printf("<caption%s>%s</caption>\n", $t, $i_dp->dpnaam);
				$exdatum = date("Y-m-d");
			}
			
			echo("<tr><th>Lid</th>");
			echo("<th>Geboren</th>");
			if ($isrn) {
				echo("<th>Sportlink</th>");
			}
			if ($p_perexamen == 0) {
				echo("<th>Behaald op</th>");
			}
		
			if ($p_perexamen == 0) {
				echo("<th>Diplomanr</th>");
				echo("<th>Geldig tot</th>");
			}
			if ($i_ex->exdatum <= date("Y-m-d")) {
				echo("<th>G</th>");
			}
			echo("<th></th></tr>\n");

			$naam_vg = "";
			$aant_vg = 0; // Aantal kandidaten dat van groep verplaatst kan worden
			$aant_ng = 0; // Aantal kandidaten die (nog) niet op geslaagd staan.
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
				$gb = $dtfmt->format(strtotime($ldrow->GEBDATUM));
				if ($isrn == 1 and strlen($ldrow->GEBPLAATS) > 1) {
					$gb .= " te " . $ldrow->GEBPLAATS;
				}
				$t = "";
				if (strlen($dd) > 0) {
					$t = sprintf(" title='%s'", $dd);
				}
				printf("<tr><td%1\$s id='naam_%3\$d'%5\$s>%2\$s</td><td>%4\$s</td>", $cl, $ldrow->NaamLid, $ldrow->RecordID, $gb, $t);
				
				if ($isrn) {
					printf("<td>%s</td>", $ldrow->RelnrRedNed);
				}
				
				if ($p_perexamen == 0) {
					printf("<td><input type='date' id='DatumBehaald_%d' value='%s'></td>", $ldrow->RecordID, $ldrow->DatumBehaald);
				}
				if ($p_perexamen == 0) {
					printf("<td><input type='text' id='Diplomanummer_%d' class='w25' value='%s' maxlength=25></td>", $ldrow->RecordID, $ldrow->Diplomanummer);
					printf("<td><input type='date' id='LicentieVervallenPer_%d' value='%s'></td>", $ldrow->RecordID, $ldrow->LicentieVervallenPer);
				} elseif ($i_ex->exdatum <= date("Y-m-d")) {
					printf("<td><input type='checkbox' id='Geslaagd_%d' title='Geslaagd?' value=1%s></td>", $ldrow->RecordID, checked($ldrow->Geslaagd));
				}

				$jsdo = sprintf("OnClick=\"liddipl_verw(%d);\"", $ldrow->RecordID);
				printf("<td title='Verwijderen %s' %s>%s</td>", htmlentities($ldrow->NaamLid), $jsdo, ICONVERWIJDER);
				
				echo("</tr>\n");
				$f = sprintf("LO.Lid=%d AND LO.OnderdeelID=%d AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE() AND LO.Functie=0", $ldrow->Lid, $p_afdid);
				$huid_gr = $i_lo->max("GroepID", $f);
				$f = sprintf("GR.RecordID=%d", $huid_gr);
				$huid_gr_dipl = $i_gr->max("DiplomaID", $f);
				if ($p_afdid > 0 and $huid_gr_dipl == $i_dp->dpid and $i_dp->dpvolgende > 0) {
					if ($ldrow->Geslaagd == 1) {
						if ($aant_vg > 0) {
							$naam_vg .= ", ";
						}
						$naam_vg .= $ldrow->NaamLid;
						$aant_vg++;
					}
				}
				if ($ldrow->Geslaagd == 0) {
					$aant_ng++;
				}
			}
			echo("</table>\n");

			if ($i_dp->eindeuitgifte >= $i_ex->exdatum) {
				echo("<div class='clear'></div>\n");
				$xf = sprintf("(L.RecordID NOT IN (SELECT LD.Lid FROM %sLiddipl AS LD WHERE LD.DiplomaID=%d AND LD.Examen=%d))", TABLE_PREFIX, $i_dp->dpid, $i_ex->exid);
				printf("<select name='ldtoevoegen_%d' class='form-select' onChange='this.form.submit();'><option value=0>Lid toevoegen ....</option>\n%s</select>\n", $i_dp->dpid, $i_lid->htmloptions(-1, 1, $xf, "", $p_afdid));
			}
			
			if ($aant_ng > 1 and $i_ex->exdatum <= date("Y-m-d")) {
				printf("<button type='submit' class='%s' name='btnAllenGeslaagd' value='%d-%d' title='Allemaal geslaagd'>%s</button>\n", CLASSBUTTON, $exid, $dipl->DiplomaID, ICONCHECK);
			}
			
			$f = sprintf("EO.DiplomaID=%d", $i_dp->dpid);
			if ($i_eo->aantal($f) > 1 and count($ldrows) > 0) {
				printf("<button type='button' class='%s' title='Print aftekenlijst' onClick=\"window.open('%s?tp=%s/Aftekenlijst&p_examen=%d&p_diploma=%d', '_blank')\">%s Afteken\n", CLASSBUTTON, $_SERVER['PHP_SELF'], $currenttab, $exid, $i_dp->dpid, ICONPRINT);
			}
			if ($i_dp->organisatie == 1 and count($ldrows) > 0) {
				printf("<button type='button' class='%s' title='Print DL-lijst' onClick=\"window.open('%s?tp=%s/DL-lijst&p_examen=%d&p_diploma=%d', '_blank')\">%s DL\n", CLASSBUTTON, $_SERVER['PHP_SELF'], $currenttab, $exid, $i_dp->dpid, ICONPRINT);
			}

			if ($aant_vg > 0 and $i_ex->exdatum <= date("Y-m-d") and $i_ex->proef == 0) {
				$i_gr->vulvars($p_afdid, $vg);
				$t = "";
				if ($aant_vg == 1) {
					$ol = $naam_vg;
				} else {
					$ol = sprintf("%d leden", $aant_vg);
					$t = sprintf(" title='%s'", $naam_vg);
				}
				
				/*
				-- Verplaatsen geslaagden naar een nieuwe groep is naar 'Groepsindeling muteren' verplaatst
				echo("<div class='clear'></div>\n");
				
				$vdps = "-1";
				$f = sprintf("VoorgangerID=%d", $i_dp->dpid);
				foreach ($i_dp->basislijst($f) as $dprow) {
					$vdps .= "," . $dprow->RecordID;
				}
				$f = sprintf("GR.OnderdeelID=%d AND GR.DiplomaID IN (%s)", $p_afdid, $vdps);
				foreach ($i_gr->basislijst($f) as $vgrow) {
					printf("<button type='submit' class='%s' name='nwe_groep' value='%d-%d'%s>%s naar groep %s</button>\n", CLASSBUTTON, $vgrow->RecordID, $i_dp->dpid, $t, $ol, $vgrow->Omschrijving);
				}
				*/
			}
			if ($p_perexamen == 1) {
				echo("</div> <!-- Einde kandidatengroep col -->\n");
//				echo("</div> <!-- Einde col -->\n");
			}
		}
		echo("</div> <!-- Einde examenresultaten -->\n");
	}
	
	if ($p_afdid > 0 and $exid > 0) {
		$f = sprintf("GR.OnderdeelID=%d AND GR.DiplomaID > 0", $p_afdid);
		echo("<div id='knoppenbalk'>\n");
		echo("<label class='form-label'>Groep toevoegen</label>\n");
		foreach ($i_gr->basislijst($f, "GR.Omschrijving") as $grrow) {
			$f_toev_groep = sprintf("LO.OnderdeelID=%1\$d AND LO.GroepID=%2\$d AND IFNULL(LO.Opgezegd, '9999-12-31') >= '%3\$s' AND LO.Lid NOT IN (SELECT LD.Lid FROM %4\$sLiddipl AS LD WHERE LD.Examen=%3\$d AND LD.DiplomaID=%5\$d)", $p_afdid, $grrow->RecordID, $exid, TABLE_PREFIX, $grrow->DiplomaID);
			$al = $i_lo->aantal($f_toev_groep);
			if ($al > 0) {
				printf("<button type='submit' class='%s' name='ledengroep_%d'>%s</button>", CLASSBUTTON, $grrow->RecordID, $grrow->Omschrijving);
			}
		}
		$f = sprintf("EX.Proefexamen=1 AND EX.OnderdeelID=%d AND EX.Datum < '%s' AND EX.Nummer IN (SELECT LD.Examen FROM %sLiddipl AS LD WHERE LD.Geslaagd=1)", $p_afdid, $i_ex->exdatum, TABLE_PREFIX);
		$potexrows = $i_ex->basislijst($f, "Datum DESC", 1, 5);
		if (count($potexrows) > 0 and $i_ex->proef == 0) {
			echo("<label class='form-label'>Proefexamen toevoegen</label>\n");
			foreach ($potexrows as $potexrow) {
				printf("<button type='submit' class='%s' name='proefexamen_%d'>Proefexamen %s</button>", CLASSBUTTON, $potexrow->Nummer, date("d-m-Y", strtotime($potexrow->Datum)));
			}
		}
		
		echo("</div> <!-- Einde knoppenbalk -->\n");
	}
	echo("</form>\n");
	
?>
<script>
		
	$("input[id^=DatumBehaald_], input[id^=Diplomanummer_], input[id^=LicentieVervallenPer_]").on('blur', function() { 
		savedata("liddipl", 0, this);
	});
	
	$("input[type='checkbox'").click(function() {
		savedata("liddipl", 0, this);
	});
	
	function liddipl_verw(p_ldid) {
		$("#naam_" + p_ldid).addClass("deleted");
		deleterecord("verw_liddipl", p_ldid);
	}

</script>
<?php
}  # fnExamenResultaten

function fnExamenonderdelen() {
	global $currenttab, $currenttab2;
	
	$i_eo = new cls_Examenonderdeel();
	$i_dp = new cls_diploma();
	
	$dpid = $_POST['selecteerdiploma'] ?? 0;
	
	if (isset($_POST['nieuwExamenonderdeel']) and $dpid > 0) {
		$i_eo->add($dpid);
	} elseif (isset($_GET['op']) and $_GET['op'] == "verwijder" and $_GET['p_eoid'] > 0) {
		$i_eo->delete($_GET['p_eoid']);
	}
	
	printf("<form action='%s?tp=%s/%s' method='post'>\n", $_SERVER["PHP_SELF"], $currenttab, $currenttab2);
	echo("<div id='filter'>\n");
	printf("<select name='selecteerdiploma' class='form-select' onChange='this.form.submit();'>\n<option value=0>Selecteer diploma ...</option>\n%s</select>\n", $i_dp->htmloptions($dpid, -1, 0, 1));
	echo("</div> <!-- Einde filter -->\n");
	
	$f = sprintf("EO.DiplomaID=%d", $dpid);
	$eores = $i_eo->basislijst($f, "EO.Regelnr, EO.Code", 0);
	
	$kols[]['headertext'] = "";
	$kols[]['headertext'] = "";
	$kols[] = array('colomnname' => "Regelnr", 'type' => "integer", 'max' => 99);
	$kols[] = array('colomnname' => "Code", 'collen' => 4);
	$kols[] = array('colomnname' => "Omschrijving", 'collen' => 45);
	$kols[] = array('columnname' => "VetGedrukt", 'headertext' => "Vet?", 'type' => "checkbox");
	$l = sprintf("%s?%s&op=verwijder&p_eoid=%%d", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	$kols[] = array('columnname' => "RecordID", 'headertext' => "&nbsp;", 'link' => $l, 'class' => "trash");
	
	if ($dpid > 0) {
		echo(fnEditTable($eores, $kols, "examenonderdeel"));
	
		echo("<div id='opdrachtknoppen'>\n");
		printf("<button name='nieuwExamenonderdeel' type='submit' class='%s'>%s Regel</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
	}
	
}  # fnExamenonderdelen

function fnDiplomasMuteren($p_afdid=-1) {
	global $dtfmt;
	
	$i_dp = new cls_Diploma();
	$i_org = new cls_Organisatie();
	$i_ld = new cls_Liddipl();
	$i_ond = new cls_Onderdeel();
	$i_eo = new cls_Examenonderdeel();
	
	if ($p_afdid > 0) {
		$f = sprintf("DP.Afdelingsspecifiek=%d", $p_afdid);
	} else {
		$f = "";
	}
	$dprows = $i_dp->basislijst($f, "DP.Kode");
	
	$i_ond->where = sprintf("O.`Type`='A' AND (IFNULL(O.VervallenPer, '9999-12-31') >= CURDATE() OR O.RecordID IN (SELECT DP.Afdelingsspecifiek FROM %sDiploma AS DP))", TABLE_PREFIX);
	
	$dpid = $_POST['selecteerdiploma'] ?? 0;
	
	printf("<form action='%s?%s' id=filter method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
	printf("<select name='selecteerdiploma' class='form-select' onChange='this.form.submit();'>\n<option value=-1>Selecteer diploma ...</option>\n%s</select>\n", $i_dp->htmloptions($dpid, -1, 0, 1, $f));
	echo("</form>\n");
	
	if ($dpid > 0) {
		$dprow = $i_dp->record($dpid);
		
		echo("<div id='diplomamuteren'>\n");
	
		printf("<label class='form-label'>RecordID</label><p id='RecordID'>%d</p>\n", $dprow->RecordID);
	
		printf("<label class='form-label'>Naam</label><input type='text' id='Naam' class='w75' maxlength=75 value=\"%s\">\n", str_replace("\"", "'", $dprow->Naam));
		printf("<label class='form-label'>Code</label><input type='text' id='Kode' class='w10' maxlength=10 value=\"%s\">\n", $dprow->Kode);
		printf("<label class='form-label' id='lblvolgnr'>Volgnummer</label><input type='number' id='Volgnr' value=%d class='num3'>\n", $dprow->Volgnr);
		printf("<label class='form-label'>Type</label><select id='Type' class='form-select'>%s</select>\n", fnOptionsFromArray(ARRTYPEDIPLOMA, $dprow->Type));
		
		if ($p_afdid <= 0) {
			printf("<label class='form-label'>Afdelingsspecifiek</label><select name='Afdelingsspecifiek' class='form-select'>\n<option value=0>Geen</option>\n%s</select>\n", $i_ond->htmloptions($dprow->Afdelingsspecifiek, 0, "", "", 0));
		}
		
		printf("<label class='form-label' id='lbluitgegevendoor'>Uitgegeven door</label><select id='ORGANIS' class='form-select'>%s</select>\n", $i_org->htmloptions(1, $dprow->ORGANIS));
		$f = sprintf("DP.RecordID<>%d AND DP.Afdelingsspecifiek=%d AND IFNULL(DP.Vervallen, '9999-12-31') > CURDATE()", $dpid, $dprow->Afdelingsspecifiek);
		printf("<label id='lblvoorganger' class='form-label'>Voorganger</label><select id='VoorgangerID' class='form-select'><Option value=0>Geen</option>\n%s</select>\n", $i_dp->htmloptions($dprow->VoorgangerID, 0, 0, 0, $f, 1));
		
		printf("<label id='lbldoorlooptijd' class='form-label'>Doorlooptijd</label><input type='number' class='num3' id='Doorlooptijd' value=%d><p>in maanden</p>\n", $dprow->Doorlooptijd);
		
		if (strlen($i_dp->naamvolgende) > 0) {
			printf("<label class='form-label' id='lblvolgende'>Volgende diploma('s)</label><p>%s</p>\n", $i_dp->naamvolgende);
		}
	
		printf("<label class='form-label' id='lbleindeuitgifte'>Einde uitgifte</label><input type='date' id='EindeUitgifte' value='%s'>\n", $dprow->EindeUitgifte);
		
		if ($p_afdid <= 0) {
			printf("<label class='form-label' id='lblgeldigheid'>Geldigheid</label><input type='number' id='GELDIGH' value=%d class='num2' max=99><p>maanden (0 = onbeperkt)</p>\n", $dprow->GELDIGH);
			printf("<label class='form-label' id='lblhistorie'>Bewaren na verlopen geldigheid</label><input type='number' id='HistorieOpschonen' value=%d class='num3' max=999><p>maanden (0 = onbeperkt)</p>\n", $dprow->HistorieOpschonen);
			printf("<label class='form-label' id='lblvervallenper'>Vervallen per</label><input type='date' id='Vervallen' value='%s'>\n", $dprow->Vervallen);
			printf("<label class='form-label' id='lblzelfservice'>Zelfservice?</label><input type='checkbox' id='Zelfservice' value=1 %s title='Is dit diploma beschikbaar in de zelfservice?'>\n", checked($dprow->Zelfservice));
		}
		printf("<label class='form-label'>Aantal leden</label><p>%d</p>\n", $i_dp->aantalhouders);
		$f_eo = sprintf("EO.DiplomaID=%d AND LENGTH(EO.Code) > 0", $i_dp->dpid);
		printf("<label>Aantal examenonderdelen</label><p>%d</p>\n", $i_eo->aantal($f_eo));
		
		echo("</div> <!-- Einde diplomamuteren -->\n");
		printf("<script>
	$('#Naam, #Kode, #Volgnr, #GELDIGH, #HistorieOpschonen').on('blur', function() {
		savedata('diplomaedit', %1\$d, this);
	});
	
	$('#ORGANIS, #VoorgangerID, #Type').on('blur', function() {
		savedata('diplomaedit', %1\$d, this);
	});
		
	$('#Zelfservice, #Doorlooptijd').on('change', function() {
		savedata('diplomaedit', %1\$d, this);
	});
	
	$('#EindeUitgifte, #Vervallen').on('blur', function(){
		savedata('diplomaedit', %1\$d, this);
		this.form.submit();
	});
</script>\n", $i_dp->dpid);
	}
}  # fnDiplomasMuteren

function DL_lijst($p_exid, $p_dpid) {
	global $dtfmt;
	
	$i_ex = new cls_Examen($p_exid);
	$i_dp = new cls_Diploma($p_dpid);
	$i_ld = new cls_Liddipl();
	
	echo("<!DOCTYPE HTML>
<html lang='nl'>
<head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1.0'>
<title>phpRBM | Zwemmend Redden | DL-lijst</title>
<link rel='stylesheet' href='default.css'>
</head>
<body>");

	echo("<div id='dl_lijst'>\n");
	echo("<img src='data:image/png;base64,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
JiaeKUxSf2JiYmJi4pnCJPUnJiYmJiaeKUxSf2JiYmJi4pnCJPUnJiYmJiaeKUxSf2JiYmJi4pnCJPUnJiYmJiaeKUxSf2JiYmJi4pnCJPUnJiYmJiaeKUxSf2JiYmJi4pnCJPUnJiYmJiaeKUxSf2Ji
YmJi4pnCJPUnJiYmJiaeKUxSf2JiYmJi4pnCJPUnJiYmJiaeKUxSf2JiYmJi4pnCJPUnJiYmJiaeKUxSf2JiYmJi4pnCJPUnJiYmJiaeKUxSf2JiYmJi4pnCJPUnJiYmJiaeKUxSf2JiYmJi4pnCJPUn
JiYmJiaeKcQv9AX8V8NBnuBrK+SSL8nBvwI2/ks/l1eemJiYmJi4/HnaS/3DovrAvw9/o/yrSHJz3L1SSd2yihUeEBDrMYjJbU2AjJm5a5gdejWeQHc49AUHMT7rj+sTvsLExMTExMRTgLj77/1VvrA4
FIEuAHqJTPXRai8S1z0bgqgXlcdJiy7OarxDnW5JCHSJ2QwVy6g2yGE5Pb7c4QtYLegk9ScmJiYmLlOe5ra+AyYY4hmKCFXZ98kb5pjgCrjhiPVByFolVxdVQdZqMJKwe5EPvANVvuSr6QP13A/a8mJF
t/Dx3wAo6L6bofzNdf/7I5Nwn5iYmJj4gvM0l/qsRKwpbk/0fQEFQXDFQRQB8yooBoLhIS25eJYPvvsjP/LPk/Wv2b3AV34j2aTeOCCuFYrnQDgkxW24BjikD0xMTExMTFxmPM2l/uB7V1xFUihfPOA/
DzL+nK8+aYUTVHGKBA+SSdu892du/9f//Mq8k80+8UPf9VI6vuoPqwhOHiIHiKvIGEFwwMb3GvUNpcQOVlfHJQkHh740MTExMTHxlPI0l/pwKOi+72M//C058MNZCILjjlurYvQ7+Vd+9p4f+RdXbz+i
0tWzZmuvv+0Hvvf5rfB1f4C14x6qfTHt439y0JNvAGJF5NtUEDkxMTExcVny9Jf6g/AtchdQBBultKIHRbI4GQ+DaM4eEv053v7Tn/7R77t2eWEjBuvc9pZHq42623vgx/7362LiTX9EwgktL7Yy9Fcx
fXAB9JLgwiHB/5n5/0O2QMlAnDSEiYmJiYmniP9GRI5hhtvwFwAdM/sNDLfRCR+CYmqeF2mRAew//bt/s5n2Yr9Lt6eVRoNuIe3Furvw6//3D2F9IAsuK8WiZAgoWchChnzABfCfZ3LvT0xMTEx8gfhv
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
b3JTcGFjZQA2NTUzNTN7AG4AAAAhdEVYdGV4aWY6RGF0ZVRpbWUAMjAwNzoxMjoxMCAxNzowNDo1
NppouWAAAAATdEVYdGV4aWY6RXhpZk9mZnNldAAxNjTMeysUAAAAGHRFWHRleGlmOlBpeGVsWERp
bWVuc2lvbgA2Nzl14HiLAAAAGHRFWHRleGlmOlBpeGVsWURpbWVuc2lvbgAyMDdHH4rhAAAAK3RF
WHRleGlmOlNvZnR3YXJlAEFkb2JlIFBob3Rvc2hvcCBDUzIgTWFjaW50b3NoZRUOrwAAABx0RVh0
ZXhpZjp0aHVtYm5haWw6Q29tcHJlc3Npb24ANvllcFcAAAAodEVYdGV4aWY6dGh1bWJuYWlsOkpQ
RUdJbnRlcmNoYW5nZUZvcm1hdAAzMDJFJGpdAAAAL3RFWHRleGlmOnRodW1ibmFpbDpKUEVHSW50
ZXJjaGFuZ2VGb3JtYXRMZW5ndGgAMjMxM9iGNNcAAAAfdEVYdGV4aWY6dGh1bWJuYWlsOlJlc29s
dXRpb25Vbml0ADIlQF7TAAAAH3RFWHRleGlmOnRodW1ibmFpbDpYUmVzb2x1dGlvbgA3Mi8x2ocY
LAAAAB90RVh0ZXhpZjp0aHVtYm5haWw6WVJlc29sdXRpb24ANzIvMXTvib0AAAAASUVORK5CYII='>\n");
	
	echo("<p id='naamformulier'>Rayon Examen Commissie<br>Deelnemerslijst Lifesaving</p>\n");
	printf("<p id='naamdiploma'>%s</p>\n", $i_dp->dpnaam);
	
	echo("<div class='clear' style='height: 60px;'></div>\n");
	
	printf("<label>Datum</label><p>%s</p>\n", $dtfmt->format(strtotime($i_ex->exdatum)));
	printf("<label>Tijd</label><p>%s</p>\n", $i_ex->begintijd);
	printf("<label>Locatie</label><p>%s</p>\n", $i_ex->explaats);
	printf("<label>Brigade</label><p>%s (%s)</p>\n", $_SESSION['settings']['naamvereniging_reddingsbrigade'], $_SESSION['settings']['sportlink_vereniging_relcode']);
	
	echo("<table>\n");
	echo("<thead>\n");
	echo("<tr><th></th><th>Relatienr</th><th>Naam</th><th>Geboortedatum</th><th>Geboorteplaats</th><th>G*</th><th>A*</th></tr>");
	echo("</thead>\n");
	
	echo("<tbody>\n");
	
	$volgnr = 1;
	foreach($i_ld->perexamendiploma($i_ex->exid, $i_dp->dpid) as $ldrow) {
		printf("<tr><td>%d</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>&nbsp;</td><td>&nbsp;</td></tr>\n", $volgnr, $ldrow->RelnrRedNed, $ldrow->Zoeknaam, date("d/m/Y", strtotime($ldrow->GEBDATUM)), $ldrow->GEBPLAATS);
		$volgnr++;
	}
	echo("</tbody>\n");
	echo("</table>\n");
	
	$h = 600 - (26 * ($volgnr-1));
	if ($h > 0) {
		printf("<div class='clear' style='height: %dpx;'></div>\n", $h);
	}

	echo("<div class='onderblok'><p>NAAM<br>VOORZITTER /TOEZICHTHOUDER</p></div>");
	echo("<div class='onderblok'><p>HANDTEKENING<br>VOORZITTER /TOEZICHTHOUDER</p></div>");
	echo("<div class='onderblok stempel'><p>STEMPEL<br>VOORZITTER /TOEZICHTHOUDER</p></div>");
	
	echo("<div class='clear'></div>\n");
	
	echo("<p id='voetregel1'>(de reddingsbrigade van) elke kandidaat wordt geacht kennis te hebben genomen van het geldende examenreglement.</p>\n");
	echo("<p id='voetregel2'>*G = geslaagd / A = afgewezen (aankruisen wat van toepassing is!)</p>\n");
	
	echo("</div> <!-- Einde dl_lijst -->\n");
	
	echo("</body>
	\n");
	
}

?>
