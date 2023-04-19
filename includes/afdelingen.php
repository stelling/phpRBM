<?php

function fnAfdeling() {
	global $currenttab, $currenttab2;
	
	$f = sprintf("`Type`='A' AND Naam='%s'", $currenttab);
	$afdid = (new cls_Onderdeel())->max("RecordID", $f);
	
	if ($currenttab2 != "DL-lijst") {
		fnDispMenu(2);
	}
	
	if ($currenttab2 == "Afdelingslijst") {
		fnAfdelingslijst($afdid);
	} elseif ($currenttab2 == "Kalender") {
		fnAfdelingskalenderMuteren($afdid);
	} elseif ($currenttab2 == "Groepsindeling") {
		fnGroepsindeling($afdid);
	} elseif ($currenttab2 == "Groepsindeling muteren") {
		fnGroepsindeling($afdid, 1);
	} elseif ($currenttab2 == "Presentie muteren") {
		fnPresentieMuteren($afdid);
	} elseif ($currenttab2 == "Presentie per lid") {
		fnPresentiePerLid($afdid);
	} elseif ($currenttab2 == "Wachtlijst") {
		fnAfdelingswachtlijst($afdid);
	} elseif ($currenttab2 == "Afdelingsmailing") {
		fnAfdelingsmailing($afdid);
	} elseif ($currenttab2 == "Diploma's") {
		fnDiplomasMuteren($afdid);
	} elseif ($currenttab2 == "DL-lijst") {
		DL_lijst($_GET['p_examen'], $_GET['p_diploma']);
	} elseif ($currenttab2 == "Examens") {
		fnExamenResultaten($afdid);
	} else {
		debug($currenttab2);
	}
	
}  # fnAfdeling

function fnAfdelingslijst($afdid) {
	
	$i_dp = new cls_Diploma();
	$i_lo = new cls_Lidond($afdid);
	$afdnm = $i_lo->ondnaam;
	
	$diplfilter = $_POST['bezitdiploma'] ?? -1;
	$xf = "";
	if ($diplfilter > 0) {
		$xf = sprintf("L.RecordID IN (SELECT LD.Lid FROM %sLiddipl AS LD WHERE LD.DiplomaID=%d AND IFNULL(LD.LicentieVervallenPer, CURDATE()) >= CURDATE())", TABLE_PREFIX, $diplfilter);
	}
	
	$kols[1]['sortcolumn'] = "L.Achternaam";
	$kols[4]['sortcolumn'] = "LO.Vanaf";
	$kols[6]['sortcolumn'] = "LO.Opgezegd";
	
	$rows = $i_lo->lijst($afdid, 1, fnOrderBy($kols), "", $xf);
	
	if (toegang($afdnm . "/Overzicht lid", 0, 0)) {
		$kols[0]['headertext'] = "&nbsp;";
		$kols[0]['columnname']= "LidID";
		$kols[0]['link'] = "<a href='index.php?tp=" . $afdnm . "/Overzicht+lid&lidid=%d'>%s</a>";
		$kols[0]['class'] = "details";
	}
	$kols[1]['headertext'] = "Naam lid";
	$kols[1]['columnname'] = "NaamLid";

	$kols[2]['headertext'] = "Email";
	$kols[2]['columnname'] = "Email";

	$f = sprintf("GR.OnderdeelID=%d", $afdid);
	if ((new cls_Groep())->aantal($f) == 0) {
		$kols[3]['headertext'] = "Functie";
	} else {
		$kols[3]['headertext'] = "Functie / groep";
	}
	$kols[3]['columnname'] = "FunctieGroep";

	$kols[4]['headertext'] = "Vanaf";
	$kols[4]['columnname'] = "Vanaf";
	
	if (strlen(max(array_column($rows, "Opmerk"))) > 0) {
		$kols[4]['headertext'] = "Opmerking";
		$kols[4]['columnname'] = "Opmerk";
	}
	
	if (strlen(max(array_column($rows, "Opgezegd"))) > 0) {
		$kols[6]['headertext'] = "Tot en met";
		$kols[6]['columnname'] = "Opgezegd";
	}
	
	if ($i_lo->organisatie == 1) {
		$kols[7]['headertext'] = "Sportlink ID";
		$kols[7]['columnname'] = "SportlinkID";
	}
	
	printf("<form method='post' id='filter' action='%s?%s'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);

	print("<input type='text' name='tbTekstFilter' id='tbTekstFilter' placeholder='Tekstfilter' OnKeyUp=\"fnFilter('afdelingslijst', this);\">\n");
	printf("<select name='bezitdiploma' id='bezitdiploma' onchange='this.form.submit();'>\n<option value=-1>Filter op diploma</option>\n%s</select>\n", $i_dp->htmloptions($diplfilter, $afdid));
	if (count($rows) > 1) {
		printf("<p class='aantrecords'>%d rijen / %d leden</p>\n", count($rows), aantaluniekeleden($rows, "LidID"));
	}
	echo("</form>\n");

	if (count($rows) > 0) {
		echo(fnDisplayTable($rows, $kols, "", 0, "", "afdelingslijst"));
		echo("<script>fnFilterTwee('afdelingslijst', 'tbNaamFilter', 'tbFuncFilter', 1, 5);</script>\n");
		foreach ($rows as $row) {
			$sel_leden[] = $row->LidID;
		}
		$_SESSION['sel_leden'] = $sel_leden;
	}
	
	$i_dp = null;
	
} # fnAfdelingslijst

function fnAfdelingskalenderMuteren($p_onderdeelid){
	global $dtfmt;
	
	$i_ak = new cls_Afdelingskalender();
	$i_aanw = new cls_Aanwezigheid();
	
	$akid = -1;
	
	if (isset($_GET['op']) and $_GET['op'] == "delete" and isset($_GET['KalID']) and $_GET['KalID'] > 0) {
		$i_ak->delete($_GET['KalID']);
	}
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		if (isset($_POST['nieuw'])) {
			$akid = $i_ak->add($p_onderdeelid);
		} elseif (isset($_POST['nieuw7'])) {
			for ($i=1;$i<=7;$i++) {
				$akid = $i_ak->add($p_onderdeelid);
			}
		}
	}
	
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	
	echo("<button type='submit' name='nieuw'>Nieuw item</button>\n");
	echo("<button type='submit' name='nieuw7'>7 nieuwe items</button>\n");
	echo("<table id='afdelingskalendermuteren'>\n");
	$dat = "";
	$oms = "";
	$act = false;
	
	echo("<tr><th>Datum</th><th>Omschrijving</th><th>Opmerking</th><th>Activiteit?</th><th></th></tr>\n");
	$dtfmt->setPattern(DTTEXTWD);
	foreach ($i_ak->lijst($p_onderdeelid) as $row) {
		$aw = $i_aanw->aantal(sprintf("AfdelingskalenderID=%d", $row->RecordID));
		
		if ($aw == 0) {
			$dat = sprintf("<input type='date' id='Datum_%d' title='Datum' value='%s'>", $row->RecordID, $row->Datum);
		} else {
			$dat = $dtfmt->format(strtotime($row->Datum));
		}
		printf("<tr><td>%s</td><td><input type='text' id='Omschrijving_%d' title='Omschrijving' value=\"%s\" maxlength=75 class='w75'></td>", $dat, $row->RecordID, str_replace("\"", "'", $row->Omschrijving));
		printf("<td><input type='text' id='Opmerking_%d' title='Opmerking' value='%s' maxlength=6 class='w6'></td>", $row->RecordID, $row->Opmerking);
		
		printf("<td><input type='checkbox' id='Activiteit_%d' title='Is er zwemmen?' value=1 %s></td>", $row->RecordID, checked($row->Activiteit));
		if ($aw == 0) {
			printf("<td class='trash'><a href='%s?tp=%s&KalID=%d&op=delete'>&nbsp;&nbsp;&nbsp;</a></td>", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
		} else {
			echo("<td></td>");
		}
		echo("</tr>\n");
		if ($akid == $row->RecordID) {
			$dat = $row->Datum;
			$oms = $row->Omschrijving;
			if ($row->Activiteit == 1) {
				$act = true;
			}
		}
	}
	
	echo("</table>\n");
	
	echo("</form>\n");
	
	$i_ak = null;
	$i_lo = null;
	
	echo("<script>
			$( document ).ready(function() {
				\$('input').blur(function(){
					savedata('afdelingskalenderedit', 0, this);
				});
			});
		</script>\n");
}  # fnAfdelingskalenderMuteren

function fnGroepsindeling($afdid, $p_muteren=0) {
	
	$i_lo = new cls_Lidond($afdid);
	$i_ond = new cls_Onderdeel($afdid);
	$i_ak = new cls_afdelingskalender($afdid);
	$i_gr = new cls_Groep($afdid);
	$i_act = new cls_Activiteit();
	$i_dp = new cls_Diploma();
	$i_ld = new cls_Liddipl();
	$i_aanw = new cls_Aanwezigheid();

	$arrToonLft[0] = "Nee";
	$arrToonLft[1] = "Ja";
	$arrToonLft[2] = "Tot 18 jaar";
	
	$hvtijd = "";
	$hvgroep = -1;
	$hvzaal = "";
	
	$filter = sprintf("O.RecordID=%d", $afdid);
	
	$afdnaam = $i_ond->naam($afdid);
	printf("<div id='%s'>\n", strtolower(str_replace(" ", "", $afdnaam)));
	
	if ($p_muteren == 0) {
		
		echo("<div id='groepsindeling'>\n");
		printf("<h2>%s</h2>\n", $afdnaam);
		
		$toonleeftijd = $_POST['toonleeftijd'] ?? 2;
		$avg_naam = $_POST['avg_naam'] ?? 0;
		$toonopmerking = $_POST['toonopmerking'] ?? 0;
		
		printf("<form method='post' id='filter' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		
		$toonpresentie = 0;
		$f = sprintf("AK.OnderdeelID=%d AND (SELECT COUNT(*) FROM %sAanwezigheid AS AW WHERE AW.AfdelingskalenderID=AK.RecordID) > 0", $afdid, TABLE_PREFIX);
		if ($i_ak->aantal($f) > 0) {
			$toonpresentie = $_POST['toonpresentie'] ?? $i_ak->komendeles();
			printf("<label>Inclusief presentie</label><select name='toonpresentie' OnChange='this.form.submit();'>\n<option value=0>Geen</option>\n%s</select>", $i_ak->htmloptions($afdid, $toonpresentie, $f));
		}
		printf("<label>Toon leeftijden</label><select name='toonleeftijd' onChange='this.form.submit();'>");
		foreach ($arrToonLft as $k => $val) {
			printf("<option value=%d%s>%s</option>", $k, checked($k, "option", $toonleeftijd), $val);
		}
		echo("</select>\n");
		printf("<label>Zonder achternaam</label><input type='checkbox' name='avg_naam' title='Toon alleen de eerste letter van de achternaam' value=1 onClick='this.form.submit();' %s>", checked($avg_naam));
		$f = sprintf("AW.AfdelingskalenderID=%d AND AW.Status IN ('A', 'L') AND LENGTH(AW.Opmerking) > 0", $toonpresentie);
		if ($i_aanw->aantal($f) > 0) {
			printf("<label>Toon opmerkingen</label><input type='checkbox' name='toonopmerking' title='Toon de opmerking bij aanwezigen' value=1 onClick='this.form.submit();' %s>", checked($toonopmerking));
		}
		echo("</form>\n");
		echo("<div class='clear'></div>\n");
	
		foreach ($i_lo->groepsindeling($afdid) as $row) {		
			if ($hvgroep != $row->GroepID) {
				if ($hvgroep > -1) {
					echo("</ol>\n");
					echo("</div>  <!-- einde groepsindelingkolom -->\n");
					if ($hvtijd !== $row->Tijdsblok and strlen($hvtijd) > 0) {
						echo("<div class='clear'></div>\n");
					}
				}
			
				if (strlen($row->Tijdsblok) > 3 and $hvtijd !== $row->Tijdsblok) {
					printf("<h3>%s</h3>\n", $row->Tijdsblok);
				}
				$hvtijd = $row->Tijdsblok;
			
				echo("<div class='groepsindelingkolom'>\n");
				printf("<h4>%s</h4>\n", $row->GroepOms);
				echo("<ol>\n");
			}
			$cl = "";
			if ($toonpresentie > 0) {
//				$stat = (new cls_Aanwezigheid())->status($row->RecordID, $toonpresentie);
				$i_aanw->vulvars($row->RecordID, $toonpresentie);
				if (strlen($i_aanw->status) > 0) {
					$cl = sprintf("presstat_%s ", strtolower($i_aanw->status));
				}
			}
			if ($row->Vanaf > date("Y-m-d")) {
				$cl .= "wordtlid";
			} elseif ($row->Opgezegd < date("Y-m-d", strtotime("+3 month"))) {
				$cl .= "opgezegd";
			} elseif ($row->LaatsteGroepMutatie > date("Y-m-d", strtotime("-3 month"))) {
				$cl .= "gewijzigd";
			}
			$cl = trim($cl);
			if ($avg_naam == 1) {
				$nm = $row->AVGnaam;
			} else {
				$nm = $row->NaamLid;
			}
			if (strlen($row->Leeftijd) > 5 and ($toonleeftijd == 1 or ($toonleeftijd == 2 and intval(substr($row->Leeftijd, 0, 2)) < 18))) {
				$nm .= " (" .  $row->Leeftijd . ")";
			}
			if ($toonopmerking == 1 and strlen($i_aanw->opmerking) > 0 and ($i_aanw->status == "A" or $i_aanw->status == "L")) {
				$nm .= " (" .  $i_aanw->opmerking . ")";
			}
			if (strlen($cl) > 0) {
				$cl = sprintf(" class='%s'", $cl);
			}
			printf("<li%s>%s</li>\n", $cl, $nm);
			$hvgroep = $row->GroepID * 1;
			$hvzaal = $row->Zwemzaal;
		}
		echo("</ol>");
		echo("</div>  <!-- einde groepsindelingkolom -->\n");
		
		echo("</div>  <!-- Einde groepsindeling -->\n");
		
	} elseif ($afdid > 0) {
		
		$inclkader = $_POST['inclkader'] ?? 0;
		
		if (isset($_POST['NieuweGroep'])) {
			$i_gr->add($afdid);
		} else {
			$i_gr->controle();
		}
		
		$kols[0]['headertext'] = "Naam lid";
		$kols[1]['headertext'] = "Leeftijd";
		$kols[2]['headertext'] = "Laatst behaalde diploma's";
		$kols[4]['headertext'] = "Groep";
		
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		
		echo("<div id='filter'>\n");
		printf("<label>Inclusief kader?</label><input type='checkbox' name='inclkader' value=1%s>\n", checked($inclkader));
		echo("<button type='submit'>Ververs scherm</button>\n");
		echo("</div> <!-- Einde filter -->\n");
		
		$grrows = $i_gr->selectlijst($afdid);
		if (count($grrows) > 0) {
			echo("<table id='groepsindelingmuteren'>\n");
			echo("<tr><th>Naam</th><th>Leeftijd</th><th>Laatst behaalde diploma's</th><th>Groep</th></tr>\n\n");
			
			if ($inclkader == 1) {
				$f = 2;
			} else {
				$f = 4;
			}
			foreach ($i_lo->lijst($afdid, $f, "GR.Volgnummer, GR.Starttijd, GR.Omschrijving, F.Sorteringsvolgorde, F.Afkorting") as $row) {
				$i_dp->vulvars($row->DiplomaID);
				$cl = "";
				$t = "";
				if ($row->Vanaf > date("Y-m-d")) {
					$cl = "wordtlid";
				} elseif (isset($row->Opgezegd) and $row->Opgezegd > "1900-01-01" and $row->Opgezegd < date("Y-m-d", strtotime("+3 MONTH"))) {
					$cl = "opgezegd";
					$t = sprintf("heeft per %s opgezegd", $row->opgezegd);
				}
				if ($row->GroepID > 0 and $row->DiplomaID > 0) {
					if ($i_dp->dpvoorganger > 0) {
						$f = sprintf("LD.Lid=%d AND LD.DiplomaID=%d AND LD.DatumBehaald < CURDATE()", $row->LidID, $i_dp->dpvoorganger);
						if ($i_ld->aantal($f) == 0) {
							$cl .= " voorgangerontbreekt";
							$t = sprintf("%s ontbreekt", $i_dp->naam($i_dp->dpvoorganger));
						}
					}
					$f = sprintf("LD.Lid=%d AND LD.DiplomaID=%d", $row->LidID, $row->DiplomaID);
					$dh = $i_ld->max("DatumBehaald", $f);
					if (strlen($dh) > 0 and $dh < date("Y-m-d")) {
						$cl .= " dubbeldiploma";
						$t = sprintf("%s is al op %s behaald", $i_dp->naam($row->DiplomaID), $dh);
					}
				}
				if (strlen($cl) > 0) {
					$cl = sprintf(" class='%s'", trim($cl));
				}
				if (strlen($t) > 0) {
					$t = sprintf(" title='%s'", $t);
				}
				$options = "\n";
				foreach ($grrows as $grrow) {
					$options .= sprintf("<option value=%d%s>%s</option>\n", $grrow->RecordID, checked($row->GroepID, "option", $grrow->RecordID), $grrow->GroepOms);
				}
				$nm = $row->NaamLid;
				if (strlen($row->FunctAfk) > 0) {
					$nm .= " (" . $row->FunctAfk . ")";
				}
				$ld = $i_ld->lidlaatstediplomas($row->LidID, 5);
				if (strlen($ld) > 60) {
					$ld = substr($ld, 0, 56) . " ...";
				}
				printf("<tr><td%s%s>%s</td><td>%s</td><td>%s</td><td><select id='GroepID_%d'>%s</select></td></tr>\n", $cl, $t, $nm, $row->Leeftijd, $ld, $row->RecordID, $options);
			}
			echo("</table>\n");
		}
		
		echo("<div class='clear' style='height: 25px;'></div>\n");
		
		echo("<table id='groepenmuteren'>\n");
		echo("<caption>Muteren groepen</caption>\n");
		echo("<tr><th>#</th><th>Volgnr<br>Code</th><th>Omschrijving<br>Instructeurs</th><th>Activiteit<br>Diploma</th><th>Starttijd<br>Eindtijd</th><th>Norm<br>aanw.</th>");
		
		$f = sprintf("AK.OnderdeelID=%d AND AK.Datum >= CURDATE()", $afdid);
		if (file_exists("maatwerk/Presentielijst.php") and $i_ak->aantal($f) > 0) {
			$plmog = true;
			echo("<th></th>");
		} else {
			$plmog = false;
		}
		echo("</tr>\n");

		foreach ($i_gr->selectlijst($afdid) as $row) {
			if ($row->RecordID > 0) {
				echo("<tr>\n");
				printf("<td class='number'>%d</td>\n", $row->RecordID);
				printf("<td><input type='number' class='num3' id='Volgnummer_%d' title='Volgnummer groep' value='%s'>", $row->RecordID, $row->Volgnummer);
				printf("<br><input type='text' class='w8' id='Kode_%d' title='Code groep' placeholder='Code' maxlength=8 value=\"%s\"></td>\n", $row->RecordID, $row->Kode);
				
				printf("<td><input type='text' class='w45' id='Omschrijving_%d' title='Omschrijving groep' placeholder='Omschrijving' maxlength=45 value=\"%s\">", $row->RecordID, $row->Omschrijving);
				printf("<br><input type='text' class='w60' id='Instructeurs_%d' title='Instructeurs groep' placeholder='Instructeurs' maxlength=60 value=\"%s\"></td>\n", $row->RecordID, $row->Instructeurs);
				
				printf("<td><select id='ActiviteitID_%d'><option value=0>Geen</option>\n%s</select>", $row->RecordID, $i_act->htmloptions($row->ActiviteitID));
				$f = sprintf("DP.Afdelingsspecifiek=%d AND IFNULL(DP.EindeUitgifte, '9999-12-31') >= CURDATE()", $afdid);
				printf("<br><select id='DiplomaID_%d'><option value=0>Geen/Combinatie</option>\n%s</select></td>\n", $row->RecordID, $i_dp->htmloptions($row->DiplomaID, 0, 0, 0, $f, 1));
				
				printf("<td><input type='time' id='Starttijd_%d' title='Starttijd' value='%s'>", $row->RecordID, $row->Starttijd);
				printf("<br><input type='time' id='Eindtijd_%d' title='Eindtijd' value='%s'></td>\n", $row->RecordID, $row->Eindtijd);
				
				printf("<td><input type='number' class='num3' id='Aanwezigheidsnorm_%d'  title='Aanwezigheidsnorm' value=%d></td>\n", $row->RecordID, $row->Aanwezigheidsnorm);
				if ($plmog) {
					if ($row->aantalInGroep > 0) {
						printf("<td class='print'><a href='./maatwerk/Presentielijst.php?p_groep=%d' title='Presentielijst %d leden'>&nbsp;</a></td>", $row->RecordID, $row->aantalInGroep);
					} else {
						echo("<td></td>");
					}
				}
				echo("</tr>\n");
			}
		}
		
		echo("</table>\n");
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<input type='submit' value='Ververs scherm'>\n");
		echo("<button type='submit' name='NieuweGroep' value='NieuweGroep'>Nieuwe groep</button>\n");
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		
		echo("</form>\n");
/*		
		echo("<script src='//cdnjs.cloudflare.com/ajax/libs/timepicker/1.3.5/jquery.timepicker.min.js'>
		\$(document).ready(function() {
			\$(\"input[type='time']\").timepicker({
				timeFormat: 'HH:mm',
				dropdown: false,
				interval: 5
			});
		});
		</script>\n");
*/
		echo("<script>
		\$('select').change(function(){
			if (this.id.substring(0, 5) == 'Groep') {
				savedata('logroep', 0, this);
			} else {
				savedata('groepedit', 0, this);
			}
		});
		
		\$('input').change(function(){
			savedata('groepedit', 0, this);

		});
		</script>\n");
	}
	
	printf("</div> <!-- Einde %s -->\n", strtolower(str_replace(" ", "", $afdnaam)));
	
} # fnGroepsindeling

function fnPresentieMuteren($p_onderdeelid){
	global $dtfmt;
	
	$dtfmt->setPattern(DTTEXTWD);
	
	$i_ak = new cls_Afdelingskalender($p_onderdeelid);
	$i_aanw = new cls_Aanwezigheid();
	$i_lo = new cls_Lidond($p_onderdeelid);
	
	$akid = 0;
	$grid = -1;
	
	$f = sprintf("AK.OnderdeelID=%d AND AK.Datum >= CURDATE() AND AK.Activiteit=1", $p_onderdeelid); 
	$akid = $_POST['selecteerdatum'] ?? $i_ak->komendeles();
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		foreach ($_POST as $k => $v) {
			if (startwith($k, "aanw_")) {
				$loid = intval(str_replace("aanw_", "", $k));
				$i_aanw->update($loid, $_POST['akid'], "Status", $v);
			}
		}
		
		if (isset($_POST['selecteergroep']) and $_POST['selecteergroep'] >= 0) {
			$grid = intval($_POST['selecteergroep']);
		}
	}
	
	echo("<div id='aanwezigheidmuteren'>\n");
	echo("<div id='filter'>\n");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	
	echo("<label>Selecteer datum</label>");
	printf("<select name='selecteerdatum' OnChange='this.form.submit();'>%s</select>\n", $i_ak->htmloptions($p_onderdeelid, $akid, "Activiteit=1"));
	$dat = "";
	
	echo("<input type='text' placeholder='Naam of groep bevat' OnKeyUp=\"fnFilter('presentiemuteren', this);\">\n");
	echo("<button type='submit'>Ververs scherm</button>\n");
	
	echo("</div> <!-- Einde filter -->\n");
	echo("<div class='clear'></div>\n");
	
	if ($akid > 0) {
		$ro = "";
		$f = sprintf("AfdelingskalenderID=%d", $akid);
		if ($i_aanw->aantal($f) > 0) {
			$ro = "readonly";
		}
		
		echo("<table id='presentiemuteren'>\n");
		printf("<caption>Presentielijst %s</caption>\n", $dtfmt->format(strtotime($dat)));

		$gh = "";
		echo("<thead>\n");
		echo("<tr><th>Naam</th><th>Groep</th><th>Status presentie</th><th>Opmerking</th></tr>\n");
		echo("</thead>\n");
		
		foreach ($i_lo->lijst($p_onderdeelid, "", "GR.Volgnummer, GR.Kode", $dat) as $row) {
			$i_aanw->vulvars($row->RecordID, $akid);
			if (strlen($i_aanw->status) > 0) {
				$cl = sprintf("class='presstat_%s'", strtolower($i_aanw->status));
			} else {
				$cl = "";
			}
			$nm = $row->NaamLid;
			if (strlen($row->FunctAfk) > 0) {
				$nm .= sprintf(" (%s)", $row->FunctAfk);
			}
			$gr = "";
			if ($grid == -1) {
				$gr = sprintf("<td>%s</td>", $row->GrNaam);
			}
			printf("<tr><td %s>%s</td>%s\n", $cl, $nm, $gr);
			
			$options = "<option value=''>Geen - Aanwezig verondersteld</option>\n";
			foreach (ARRPRESENTIESTATUS as $k => $o) {
				$s = "";
				if ($k == $i_aanw->status) {
					$s = "selected";
				}				
				$options .= sprintf("<option value='%1\$s' %2\$s>%1\$s - %3\$s</option>\n", $k, $s, $o);
			}
			
			printf("<td><select id='status_%d'>%s</select></td>", $row->RecordID, $options);
			printf("<td><input type='text' id='opmerk_%d' class='w75' value=\"%s\" maxlength=75></td>\n", $row->RecordID, $i_aanw->opmerking);
			echo("</tr>\n");
		}
		echo("</table>\n");
	}
	echo("</form>\n");
	
	
	echo("</div> <!-- Einde aanwezigheidmuteren -->\n");
	
	printf("<script>
				$('select[id^=status]').change(function() {
					id = this.id;
					var split_id = id.split('_');
					var loid = split_id[1];
					var value = this.value;

					$.ajax({
						url: 'ajax_update.php?entiteit=lo_presentie',
						type: 'post',
						dataType: 'json',
						data: { loid: loid, field: 'Status', value: value, akid: %1\$d },
						success:function(response){}
					});
				});
				
				$('input[id^=opmerk]').blur(function() {
					id = this.id;
					var split_id = id.split('_');
					var loid = split_id[1];
					var value = this.value;

					$.ajax({
						url: 'ajax_update.php?entiteit=lo_presentie',
						type: 'post',
						dataType: 'json',
						data: { loid: loid, field: 'Opmerking', value: value, akid: %1\$d },
						success:function(response){}
					});
				});
			</script>\n", $akid);
	$i_ak = null;
	$i_aanw = null;
	$i_lo = null;
	
}  # fnAanwezigheidMuteren

function fnPresentiePerLid($p_ondid) {
	global $dtfmt;
	
	$dtfmt->setPattern(DTTEXT);
	
	$i_lo = new cls_Lidond($p_ondid);
	$i_aw = new cls_Aanwezigheid();
	$i_ak = new cls_Afdelingskalender();
	
	$seizrows = $i_aw->seizoenen($p_ondid);
	
	if ($i_aw->aantalstatus('J', $p_ondid) > 0) {
		$kop_aangemeld = "<th># Aangemeld</th>";
	} else {
		$kop_aangemeld = "";
	}
	
	if ($i_aw->aantalstatus('L', $p_ondid) > 0) {
		$kop_telaat = "<th>Te laat</th>";
	} else {
		$kop_telaat = "";
	}
	
	echo("<div id='presentieperlid'>\n");
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$_POST['filterAanwezigheidsnormOnder'] = $_POST['filterAanwezigheidsnormOnder'] ?? 0;
		$_POST['filterAanwezigheidsnormBoven'] = $_POST['filterAanwezigheidsnormBoven'] ?? 0;
		$_POST['100aanwezigTonen'] = $_POST['100aanwezigTonen'] ?? 0;
	} else {
		$_POST['filterAanwezigheidsnormOnder'] = 1;
		$_POST['filterAanwezigheidsnormBoven'] = 1;
		$_POST['100aanwezigTonen'] = 0;
	}
	
	printf("<form method='post' id='filter' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<label>Filter</label>\n");
	printf("<input type='checkbox' name='filterAanwezigheidsnormOnder'%s value=1 OnClick='this.form.submit();'><p>Onder norm</p>\n", checked($_POST['filterAanwezigheidsnormOnder']));
	printf("<input type='checkbox' name='filterAanwezigheidsnormBoven'%s value=1 OnClick='this.form.submit();'><p>Boven norm</p>\n", checked($_POST['filterAanwezigheidsnormBoven']));
	printf("<input type='checkbox' name='100aanwezigTonen'%s value=1 OnClick='this.form.submit();'><p>100%% aanwezig</p>\n", checked($_POST['100aanwezigTonen']));
	echo("</form>\n");
	
	echo("<table>\n");
	printf("<caption> Presentie per lid %s</caption>\n", $i_lo->ondnaam);
	foreach ($seizrows as $seizrow) {
		printf("<tr class='seizoentotaal'><th class='teken'>-</th><th>%d</th><th>%s t/m %s</th><th>Groep</th><th># Act.</th>", $seizrow->Nummer, $dtfmt->format(strtotime($seizrow->Begindatum)), $dtfmt->format(strtotime($seizrow->Einddatum)));
		printf("%s<th># Afwezig</th><th>%% Aanwezig</th><th>Ziek</th><th>Met reden</th><th>Zonder reden</th>%s</tr>\n", $kop_aangemeld, $kop_telaat);
		$lorows = $i_lo->lijst($p_ondid, "", "", $seizrow->Einddatum);
		foreach ($lorows as $lorow) {
			$aanwtotrow = $i_aw->perlidperperiode($lorow->RecordID, $seizrow->Begindatum, $seizrow->Einddatum);
			$lnklid = "";
			if ($lorow->Invalfunctie == 1) {
				$aa = $aanwtotrow->aantAanwezig + $aanwtotrow->aantAangemeld + $aanwtotrow->aantLaat;
			} else {
				if ($lorow->Vanaf > $seizrow->Begindatum) {
					$f = sprintf("AK.Datum >= '%s'", $lorow->Vanaf);
				} else {
					$f = sprintf("AK.Datum >= '%s'", $seizrow->Begindatum);
				}
				if ($lorow->Opgezegd > "1900-01-01" and ($lorow->Opgezegd < $seizrow->Einddatum or $lorow->Opgezegd < date("Y-m-d"))) {
					$f .= sprintf(" AND AK.Datum <= '%s'", $lorow->Opgezegd);
				} elseif (date("Y-m-d") < $seizrow->Einddatum) {
					$f .= sprintf(" AND AK.Datum <= '%s'", date("Y-m-d"));
				} else {
					$f .= sprintf(" AND AK.Datum <= '%s'", $seizrow->Einddatum);
				}
				$f .= sprintf(" AND AK.OnderdeelID=%s AND AK.Activiteit=1", $p_ondid);
				$aa = $i_ak->aantal($f);
				$aa -= $aanwtotrow->aantVervallen;
			}
			if ($aa > 0 and (($aanwtotrow->aantAanwezig + $aanwtotrow->aantAangemeld + $aanwtotrow->aantAfwezig) > 0 or $_POST['100aanwezigTonen'] == 1)) {
				$awperc = (($aa-$aanwtotrow->aantAfwezig)/$aa)*100;
				$toon = false;
				if ($_POST['filterAanwezigheidsnormBoven'] == 1 and $awperc >= $lorow->Aanwezigheidsnorm) {
					$toon = true;
				} elseif ($_POST['filterAanwezigheidsnormOnder'] == 1 and $awperc < $lorow->Aanwezigheidsnorm) {
					$toon = true;
				}
				if ($toon) {
					$tab = sprintf("%s/Overzicht lid/Presentie", $i_lo->ondnaam);
					if (toegang($tab, 0, 0)) {
						$lnklid = sprintf("%s?tp=%s&lidid=%d", $_SERVER['PHP_SELF'], $tab, $lorow->Lid);
						$kollid = sprintf("<a href='%s'>%s</a>", $lnklid, $lorow->NaamLid);
					} else {
						$kollid = $lorow->NaamLid;
					}
					if ($lorow->FunctieID > 0) {
						$kollid .= sprintf(" (%s)", $lorow->FunctAfk);
					}
					$cl = "";
					if ($lorow->Opgezegd < date("Y-m-d", strtotime("+3 MONTH")) and $lorow->Opgezegd > $seizrow->Begindatum) {
						$cl = " class='opgezegd'";
					}
					printf("<tr class='lidtotaal'><td></td><td colspan=2%s>%s</td><td>%s</td>", $cl, $kollid, $lorow->GrCode);
					printf("<td class='number'>%d</td>", $aa);
					if (strlen($kop_aangemeld) > 0) {
						printf("<td class='number'>%d</td>", $aanwtotrow->aantAangemeld);
					}
					printf("<td class='number'>%d</td>", $aanwtotrow->aantAfwezig);
					$cl = "OK";
					if ($lorow->Aanwezigheidsnorm > 0 and $lorow->Aanwezigheidsnorm > $awperc) {
						$cl = "NOK";
					}
					printf("<td class='number %s'>%.2f</td>", $cl, $awperc);
					printf("<td class='number'>%d</td>", $aanwtotrow->aantZiek);
					printf("<td class='number'>%d</td>", $aanwtotrow->aantMetReden);
					printf("<td class='number'>%d</td>", $aanwtotrow->aantZonderReden);
					if (strlen($kop_telaat) > 0) {
						printf("<td class='number'>%d</td>", $aanwtotrow->aantLaat);
					}
					echo("</tr>\n");
				}
			}
		}
	}
	echo("</table>\n");
	
	echo("</div> <!-- Einde presentieperlid -->\n");
	
	echo("<script>
		$('tr.seizoentotaal').click(function(){
			var \$this = \$(this);
			\$(this).nextUntil('tr.seizoentotaal').slideToggle(100).promise().done(function () {
				\$this.find('.teken').text(function (_, value) {
					return value == '-' ? '+' : '-'
				});
			});
		});
	</script>\n");
}  # fnPresentiePerLid

function fnAfdelingswachtlijst($p_afdid) {
	
	$i_ins = new cls_Inschrijving();
	
	if (isset($_GET['op']) and $_GET['op'] == "delete" and $_GET['RecordID'] > 0 and toegang("deleteinschrijving", 1, 1)) {
		$i_ins->delete($_GET['RecordID']);
	}
	
	$rows = $i_ins->lijst(1, $p_afdid, 0);
	
	$kols[0]['headertext'] = "";
	$kols[0]['columnname'] = "RecordID";
	$kols[0]['type'] = "pk";
	$kols[0]['readonly'] = true;
	
	$kols[1]['headertext'] = "Vanaf";
	$kols[1]['columnname'] = "Ingevoerd";
	$kols[1]['type'] = "date";
	$kols[1]['readonly'] = true;
	
	$kols[2]['headertext'] = "Naam & geboren";
	$kols[2]['columnname'] = "Naam";
	$kols[2]['secondcolumn'] = "Geboortedatum";
	$kols[2]['secondcolumntype'] = "geboren_leeftijd";
	$kols[2]['readonly'] = true;
	
	$kols[3]['headertext'] = "E-mail";
	$kols[3]['columnname'] = "Email";
	$kols[3]['type'] = "email";
	$kols[3]['readonly'] = true;
	
	$kols[4]['headertext'] = "Opmerking & eerste les";
	$kols[4]['columnname'] = "Opmerking";
	$kols[4]['secondcolumn'] = "EersteLes";
	$kols[4]['secondcolumntype'] = "date";

	$kols[6]['headertext'] = "&nbsp;";
	$kols[6]['columnname'] = "LnkPDF";
	$kols[6]['link'] = sprintf("%s/pdf.php?insid=%%d", BASISURL);
	$kols[6]['class'] = "pdf";
	
	if (toegang("deleteinschrijving", 0, 0)) {
		$kols[7]['headertext'] = "&nbsp;";
		$kols[7]['columnname'] = "RecordID";
		$kols[7]['link'] = sprintf("%s?tp=%s&op=delete&RecordID=%%d", $_SERVER['PHP_SELF'], $_GET['tp']);
		$kols[7]['class'] = "trash";
	}
	
//	echo(fnDisplayTable($rows, $kols, "Wachtlijst", 0, "", "wachtlijst"));
	echo(fnEditTable($rows, $kols, "wachtlijst", "Wachtlijst"));
}

function fnAfdelingsmailing($p_afdid) {
	global $selaant, $mingroepgewijzigd;
	
	$i_gr = new cls_Groep($p_afdid);
	$i_m = new cls_Mailing();
	$i_mr = new cls_Mailing_rcpt();
	$i_lo = new cls_Lidond($p_afdid);
	$i_f = new cls_Functie();
	$i_ex = new cls_examen();
	
	$selmailing = $_POST['SelecteerMailing'] ?? 0;
	$_POST['groepgewijzigdna'] = $_POST['groepgewijzigdna'] ?? "1900-01-01";
	$_POST['kandidaatopexamen'] = $_POST['kandidaatopexamen'] ?? -1;
	$mingroepgewijzigd = substr((new cls_Logboek())->min("DatumTijd", "refColumn='GroepID'"), 0, 10);
	if ($_POST['groepgewijzigdna'] < "2000-01-01") {
		$_POST['groepgewijzigdna'] = $mingroepgewijzigd;
	}
	$selontv = "";
	$selaant = 0;
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		if ($selmailing == -1) {
			$selmailing = $i_m->add("Afdelingsmailing " . $i_lo->ondnaam);
		}
		
		if (isset($_POST['btnOntvangersAanpassen']) and $selmailing > 0) {
			$selontv = fnOntvangersAfdelingsmailing($p_afdid, 1, $selmailing);
		} else {
			$selontv = fnOntvangersAfdelingsmailing($p_afdid, 0);
		}
	}

	$grrows = $i_gr->selectlijst();

	echo("<div id='afdelingsmailing'>\n");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	
	if (count($grrows) > 1) {
		echo("<h2>Selecteer groepen</h2>\n");
		echo("<ul>\n");
		foreach ($grrows as $grrow) {
			if ($grrow->aantalInGroep > 0) {
				$cn = sprintf("chkGroep_%d", $grrow->RecordID);
				if ($grrow->RecordID > 0) {
					$o = $grrow->Omschrijving;
				} else {
					$o = "Niet ingedeeld";
				}
				printf("<li><input type='checkbox' value=1 name='%s'%s title='%3\$s'><p>%3\$s (%4\$d)</p></li>\n", $cn, checked(getvar($cn)), $o, $grrow->aantalInGroep);
			}
		}
		$cn = "chkGroepAlle";
		echo("<li><button type='button' onClick='fnAlleGroepen();'>Alle groepen</button></li>\n");
		echo("</ul>\n");
		printf("<label for='groepgewijzigdna'>Groep gewijzigd op of na</label><input type='date' name='groepgewijzigdna' value='%s'>\n", $_POST['groepgewijzigdna']);
		// examen
		$f = sprintf("EX.OnderdeelID=%d", $p_afdid);
		if ($i_ex->aantal($f) > 0) {
			printf("<label>Kandidaat op examen</label><select name='kandidaatopexamen'><option value=-1>Niet van toepassing</option>\n%s</select>", $i_ex->htmloptions($_POST['kandidaatopexamen'], $f));
		}
	}

	echo("<h2>Selecteer functies</h2>");
	echo("<ul>\n");
	foreach ($i_f->selectlijst("A", "", 0, $p_afdid) as $frow) {
		$cn = sprintf("chkFunctie_%d", $frow->Nummer);
		if ($frow->Nummer > 0 and $frow->aantalMetFunctie > 0) {
			printf("<li><input type='checkbox' value=1 name='%s'%s><p>%s (%d)</p></li>\n", $cn, checked(getvar($cn)), $frow->Omschrijv, $frow->aantalMetFunctie);
		}
	}
	$cn = "chkFunctiesAlle";
	echo("<li><button type='button' onClick='fnAlleFuncties();'>Alle functies</button></li>\n");
	echo("</ul>\n");
	
	if (strlen($selontv) > 0) {
		if ($selaant > 3) {
			printf("<h2>Geselecteerde leden (%d)</h2>\n", $selaant);
		} else {
			echo("<h2>Geselecteerde leden</h2>\n");
		}
		printf("<ul>\n%s\n</ul>\n", $selontv);
	}
	
	echo("<h2>Selecteer mailing</h2>\n");
		
	printf("<select name='SelecteerMailing' title='Selecteer mailing' OnChange='this.form.submit();'>\n<option value=0>Selecteer ...</option>\n<option value=-1>*** Nieuwe mailing</option>\n%s</select>\n", $i_m->htmloptions($selmailing));
	
	echo("<div id='opdrachtknoppen'>\n");
	echo("<button type='submit' name='btnOntvangersBijwerken'>Ontvangers op scherm bijwerken</button>");
	if ($selmailing > 0) {
		if (toegang("Mailing/Muteren", 0)) {
			$d = "";
		} else {
			$d = " disabled";
		}
		printf("<button type='submit' name='btnOntvangersAanpassen'%s>Ontvangers in mailing aanpassen</button>", $d);
		printf("<button type='button' onClick=\"location.href='%s?tp=Mailing/Wijzigen mailing&mid=%d'\"%s>Ga naar mailing</button>", $_SERVER['PHP_SELF'], $selmailing, $d);
	}
	
	echo("</div> <!-- Einde opdrachtknoppen -->\n");
	echo("</form>\n");
	
	echo("</div> <!-- Einde afdelingsmailing -->\n");
	
?>

<script>
	function fnAlleGroepen() {
		$("input[name^='chkGroep_']").each(function() {
			if ($(this).attr("name") !== "chkGroep_0") {
				$(this).prop('checked', true);
			}
		});
	}

	function fnAlleFuncties() {
		$("input[name^='chkFunctie_']").each(function() {
			$(this).prop('checked', true);
		});
	}
</script>

<?php
}  # fnAfdelingsmailing

function fnOntvangersAfdelingsmailing($p_afdid, $p_uitvoeren=0, $p_mailing=-1) {
	global $selaant, $mingroepgewijzigd;
	
	$i_mr = new cls_Mailing_rcpt();
	$i_lid = new cls_Lid();
	$i_lo = new cls_Lidond();
	$i_ld = new cls_Liddipl();
	
	if ($p_mailing <= 0) {
		$p_uitvoeren = 0	;
	}
	
	$rv = "";
	$selaant = 0;
	
	if ($p_uitvoeren == 1) {
		$i_mr->delete_all($p_mailing);
	}
	foreach ($i_lo->groepsindeling($p_afdid, "", 1) as $lorow) {
		$cn = sprintf("chkGroep_%d", $lorow->GroepID);
		$cnf = sprintf("chkFunctie_%d", $lorow->Functie);
		if (isset($_POST[$cn])) {
			$toev = true;
			if ($_POST['kandidaatopexamen'] > 0) {
				$f = sprintf("LD.Examen=%d AND LD.Lid=%d", $_POST['kandidaatopexamen'], $lorow->LidID);
				if ($i_ld->aantal($f) == 0) {
					$toev = false;
				}
			}
			if ($toev and ($mingroepgewijzigd >= $_POST['groepgewijzigdna'] or $lorow->LaatsteGroepMutatie >= $_POST['groepgewijzigdna'])) {
				$rv .= sprintf("<li>%s</li>", $i_lid->naam($lorow->Lid));
				$selaant++;
				if ($p_uitvoeren == 1) {
					$i_mr->add($p_mailing, $lorow->Lid, "", 0, "AfdGr", $lorow->GroepID);
				}
			}
		} elseif (isset($_POST[$cnf])) {
			$rv .= sprintf("<li>%s</li>", $i_lid->naam($lorow->Lid));
			$selaant++;
			if ($p_uitvoeren == 1) {
				$i_mr->add($p_mailing, $lorow->Lid, "", 0);
			}
		}
	}
	
	$i_mr = null;
	$i_lid = null;
	$i_lo = null;
	
	return $rv;
}

?>
