<?php

function fnAfdeling() {
	global $currenttab, $currenttab2;
	
	$f = sprintf("`Type`='A' AND Naam='%s'", $currenttab);
	$afdid = (new cls_Onderdeel())->max("RecordID", $f);
	
	fnDispMenu(2);
	
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
	} elseif ($currenttab2 == "Afdelingsmailing") {
		fnAfdelingsmailing($afdid);
	}
	
}  # fnAfdeling

function fnAfdelingslijst($afdid) {
	
	$i_dp = new cls_Diploma();
	$i_lo = new cls_Lidond($afdid);
	$afdnm = $i_lo->ondnaam;
	$i_lo->auto_einde($afdid);
	
	if (toegang($afdnm . "/Overzicht lid", 0, 0)) {
		$kols[0]['headertext'] = "&nbsp;";
		$kols[0]['link'] = "<a href='index.php?tp=" . $afdnm . "/Overzicht+lid&lidid=%d'>%s</a>";
		$kols[0]['class'] = "details";
	}
	$kols[1]['headertext'] = "Naam lid";
	$kols[1]['sortcolumn'] = "L.Achternaam";
	$kols[2]['headertext'] = "Bereiken";
	$kols[3]['headertext'] = "Functie / groep";
	$kols[4]['headertext'] = "Vanaf";
	$kols[4]['sortcolumn'] = "LO.Vanaf";
	$kols[4]['columnname'] = "Vanaf";
	$kols[5]['headertext'] = "Tot en met";
	$kols[5]['columnname'] = "Opgezegd";
	$kols[5]['sortcolumn'] = "LO.Opgezegd";
	if ($i_lo->organisatie == 1) {
		$kols[6]['headertext'] = "Sportlink ID";
		$kols[6]['columnname'] = "SportlinkID";
	}
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$diplfilter = $_POST['bezitdiploma'];
	} else {
		$diplfilter = -1;
	}
	
	$xf = "";
	if ($diplfilter > 0) {
		$xf = sprintf("L.RecordID IN (SELECT LD.Lid FROM %sLiddipl AS LD WHERE LD.DiplomaID=%d AND IFNULL(LD.LicentieVervallenPer, CURDATE()) >= CURDATE())", TABLE_PREFIX, $diplfilter);
	}
	
	$rows = $i_lo->onderdeellijst($afdid, 1, $xf, fnOrderBy($kols));
	
	echo("<div id='filter'>\n");
	printf("<form action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);

	print("<input type='text' name='tbTekstFilter' id='tbTekstFilter' placeholder='Tekstfilter' OnKeyUp=\"fnFilter('afdelingslijst', this);\">\n");
	printf("<select name='bezitdiploma' id='bezitdiploma' onchange='this.form.submit();'>\n<option value=-1>Filter op diploma</option>\n%s</select>\n", $i_dp->htmloptions($diplfilter, $afdid));
	if (count($rows) > 1) {
		printf("<p class='aantrecords'>%d rijen / %d leden</p>\n", count($rows), aantaluniekeleden($rows, "ndLidID"));
	}
	echo("</form>\n");
	echo("</div>  <!-- Einde filter -->\n");

	if (count($rows) > 0) {
		echo(fnDisplayTable($rows, $kols, "", 0, "", "afdelingslijst"));
		echo("<script>fnFilterTwee('afdelingslijst', 'tbNaamFilter', 'tbFuncFilter', 1, 5);</script>\n");
		foreach ($rows as $row) {
			$sel_leden[] = $row->ndLidID;
		}
		$_SESSION['sel_leden'] = $sel_leden;
	}
	
	$i_dp = null;
	
} # fnAfdelingslijst

function fnAfdelingskalenderMuteren($p_onderdeelid){
	
	$i_ak = new cls_Afdelingskalender();
	$i_aanw = new cls_Aanwezigheid();
	
	$akid = -1;
	
	if (isset($_GET['op']) and $_GET['op'] == "delete" and isset($_GET['KalID']) and $_GET['KalID'] > 0) {
		$i_ak->delete($_GET['KalID']);
	}
	
	echo("<div id='afdelingskalendermuteren'>\n");
	
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
	echo("<table>\n");
	$dat = "";
	$oms = "";
	$act = false;
	
	echo("<tr><th>Datum</th><th>Opmerking</th><th>Activiteit?</th><th></th></tr>\n");
	
	foreach ($i_ak->lijst($p_onderdeelid) as $row) {
		$aw = $i_aanw->aantal(sprintf("AfdelingskalenderID=%d", $row->RecordID));
		
		if ($aw == 0) {
			$dat = sprintf("<input type='date' id='Datum_%d' value='%s'>", $row->RecordID, $row->Datum);
		} else {
			$dat = strftime("%A %e %B %Y", strtotime($row->Datum));
		}
		printf("<tr><td>%s</td><td><input type='text' id='Omschrijving_%d' value=\"%s\" maxlength=75 class='w75'></td>", $dat, $row->RecordID, str_replace("\"", "'", $row->Omschrijving));
		
		printf("<td><input type='checkbox' id='Activiteit_%d' value=1 %s></td>", $row->RecordID, checked($row->Activiteit));
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
	
	echo("</div> <!-- Einde afdelingskalendermuteren -->\n");
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

function fnGroepsindeling($afdid, $p_muteren=0, $p_toonleeftijd=2) {
	/* p_toonleeftijd
		0: nee
		1: ja, alle
		2: alleen bij jonger dan 18 jaar
	*/
	
	$hvtijd = "";
	$hvgroep = -1;
	$hvzaal = "";
	
	if ($afdid > 0) {
		$filter = sprintf("O.RecordID=%d", $afdid);
	} else {
		$filter = "";
	}
	
	$afdnaam = (new cls_Onderdeel())->naam($afdid);
	printf("<div id='%s'>\n", strtolower(str_replace(" ", "", $afdnaam)));
	
	if ($p_muteren == 0) {
		
		echo("<div id='groepsindeling'>\n");
		printf("<h2>%s</h2>\n", $afdnaam);
		
		$i_ak = new cls_afdelingskalender();
		$toonpresentie = 0;
		$f = sprintf("AK.OnderdeelID=%d AND (SELECT COUNT(*) FROM %sAanwezigheid AS AW WHERE AW.AfdelingskalenderID=AK.RecordID) > 0", $afdid, TABLE_PREFIX);
		if ($i_ak->aantal($f) > 0) {
			$toonpresentie = $_POST['toonpresentie'] ?? $i_ak->min("RecordID", "Datum >= CURDATE() AND " . $f);
			printf("<form name='grpfilter' method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
			printf("<label>Inclusief afwezigheid</label><select name='toonpresentie' OnChange='this.form.submit();'>\n<option value=0>Geen</option>\n%s</select>", $i_ak->htmloptions($afdid, $toonpresentie, $f));
			echo("</form>\n");
			echo("<div class='clear'></div>\n");
		}
	
		foreach ((new cls_Lidond())->groepsindeling($afdid) as $row) {		
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
				$stat = (new cls_Aanwezigheid())->status($row->RecordID, $toonpresentie);
				if (strlen($stat) > 0) {
					$cl = sprintf("presstat_%s ", strtolower($stat));
				}
			}
			if ($row->Vanaf > date("Y-m-d")) {
				$cl .= "wordtlid";
			} elseif ($row->Opgezegd < date("Y-m-d", strtotime("+3 MONTH"))) {
				$cl .= "opgezegd";
			}
			$cl = trim($cl);
			$nm = $row->NaamLid;
			if (strlen($row->Leeftijd) > 5 and ($p_toonleeftijd == 1 or ($p_toonleeftijd == 2 and intval(substr($row->Leeftijd, 0, 2)) < 18))) {
				$nm .= " (" .  $row->Leeftijd . ")";
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
		
		$i_lo = new cls_Lidond();
		$i_gr = new cls_Groep();
		$i_ak = new cls_Afdelingskalender($afdid);

		if (isset($_POST['NieuweGroep'])) {
			$i_gr->add($afdid);
		}
		
		$kols[0]['headertext'] = "Naam lid";
		$kols[1]['headertext'] = "Leeftijd";
		$kols[2]['headertext'] = "Laatst behaalde diploma's";
		$kols[4]['headertext'] = "Groep";
		
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		echo("<div id='groepsindelingmuteren'>\n");
		
		$grrows = $i_gr->selectlijst($afdid);
		if (count($grrows) > 0) {
			echo("<table>\n");
			echo("<tr><th>Naam</th><th>Leeftijd</th><th>Laatst behaalde diploma's</th><th>Groep</th></tr>\n\n");
			foreach ($i_lo->onderdeellijst($afdid, 2, "", "GR.Volgnummer, GR.Starttijd, GR.Omschrijving, F.Sorteringsvolgorde, F.Afkorting") as $row) {
				$cl = "";
				if ($row->Vanaf > date("Y-m-d")) {
					$cl = " class='wordtlid'";
				} elseif (isset($row->Opgezegd) and $row->Opgezegd > "1900-01-01" and $row->Opgezegd < date("Y-m-d", strtotime("+3 MONTH"))) {
					$cl = " class='opgezegd'";
				}
				$options = "\n";
				foreach ($grrows as $grrow) {
					$options .= sprintf("<option value=%d%s>%s</option>\n", $grrow->RecordID, checked($row->GroepID, "option", $grrow->RecordID), $grrow->GroepOms);
				}
				$nm = $row->Naam_lid;
				if (strlen($row->AfkFunc) > 0) {
					$nm .= " (" . $row->AfkFunc . ")";
				}
				$ld = (new cls_Liddipl())->lidlaatstediplomas($row->ndLidID, 3);
				printf("<tr><td%s>%s</td><td>%s</td><td>%s</td><td><select id='GroepID_%d'>%s</select></td></tr>\n", $cl, $nm, $row->Leeftijd, $ld, $row->RecordID, $options);
			}
			echo("</table>\n");
		}
		echo("</div>  <!-- Einde groepsindelingmuteren -->\n");
		
		echo("<div id='groepenmuteren'>\n");
		echo("<table>\n");
		echo("<caption>Muteren groepen</caption>\n");
		echo("<tr><th>#</th><th>Volgnr</th><th>Code</th><th>Omschrijving</th><th>Instructeurs</th><th>Starttijd</th><th>Eindtijd</th><th>Norm aanw.</th>");
		
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
				printf("<td><input type='number' id='Volgnummer_%d' value='%s'></td>\n", $row->RecordID, $row->Volgnummer);
				printf("<td><input type='text' class='w8' id='Kode_%d' maxlength=8 value=\"%s\"></td>\n", $row->RecordID, $row->Kode);
				printf("<td><input type='text' class='w45' id='Omschrijving_%d' maxlength=45 value=\"%s\"></td>\n", $row->RecordID, $row->Omschrijving);
				printf("<td><input type='text' class='w60' id='Instructeurs_%d' maxlength=60 value=\"%s\"></td>\n", $row->RecordID, $row->Instructeurs);
				printf("<td><input type='time' id='Starttijd_%d' value='%s'></td>\n", $row->RecordID, $row->Starttijd);
				printf("<td><input type='time' id='Eindtijd_%d' value='%s'></td>\n", $row->RecordID, $row->Eindtijd);
				printf("<td><input type='number' id='Aanwezigheidsnorm_%d' value=%d></td>\n", $row->RecordID, $row->Aanwezigheidsnorm);
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
		
		echo("</div>  <!-- Einde groepenmuteren -->\n");
		
		echo("</form>\n");
		
		echo("<script>
		\$('select').change(function(){
			savedata('logroep', 0, this);
		});
		
		\$('input').change(function(){
			savedata('groepedit', 0, this);
		});
		</script>\n");
	}
	
	printf("</div> <!-- Einde %s -->\n", strtolower(str_replace(" ", "", $afdnaam)));
	
} # fnGroepsindeling

function fnPresentieMuteren($p_onderdeelid){
	
	$i_ak = new cls_Afdelingskalender($p_onderdeelid);
	$i_aanw = new cls_Aanwezigheid();
	$i_lo = new cls_Lidond($p_onderdeelid);
	
	$akid = 0;
	$grid = -1;
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		foreach ($_POST as $k => $v) {
			if (startwith($k, "aanw_")) {
				$loid = intval(str_replace("aanw_", "", $k));
				$i_aanw->update($loid, $_POST['akid'], "Status", $v);
			}
		}
	
		if (isset($_POST['selecteerdatum'])) {
			$akid = $_POST['selecteerdatum'];
		}
		
		if (isset($_POST['selecteergroep']) and $_POST['selecteergroep'] >= 0) {
			$grid = intval($_POST['selecteergroep']);
		}
	}
	
	echo("<div id='aanwezigheidmuteren'>\n");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	
	echo("<label>Selecteer datum</label><p>");
	echo("<select name='selecteerdatum' OnChange='this.form.submit();'>\n");
	echo("<option value=-1>...</option>\n");
	$dat = "";
	$oms = "";
	$act = false;
	foreach ($i_ak->lijst($p_onderdeelid, "", "Activiteit=1") as $row) {
		printf("<option value=%d %s>%s</option>\n", $row->RecordID, checked($row->RecordID, "option", $akid), strftime("%A %e %B %Y", strtotime($row->Datum)));
		if ($akid == $row->RecordID) {
			$dat = $row->Datum;
			$oms = $row->Omschrijving;
			if ($row->Activiteit == 1) {
				$act = true;
			}
		}
	}
	echo("</select></p>\n");
	$row = null;
	
	echo("<label>Selecteer groep</label><p>");
	echo("<select name='selecteergroep' OnChange='this.form.submit();'>\n");
	echo("<option value=-1>...</option>\n");
	foreach ($i_lo->groepeningebruik() as $row) {
		printf("<option value=%d %s>%s</option>\n", $row->GroepID, checked($row->GroepID, "option", $grid), $row->Omschrijving);
	}
	echo("</select></p>\n");
	$row = null;
	
	echo("<div class='clear'></div>\n");
	
	if ($akid > 0) {
		printf("<input type='hidden' name='akid' value=%d>\n", $akid);
		$ro = "";
		$f = sprintf("AfdelingskalenderID=%d", $akid);
		if ($i_aanw->aantal($f) > 0) {
			$ro = "readonly";
		}
		
		echo("<table>\n");
		printf("<caption>Presentielijst %s</caption>\n", strftime("%A %e %B %Y", strtotime($dat)));

		$gh = "";
		$f = "";
		if ($grid >= 0) {
			$f = sprintf("LO.GroepID=%d", $grid);
		} else {
			$gh = "<th>Groep</th>";
		}
		echo("<thead>\n");
		printf("<tr><th>Naam</th>%s<th>Status presentie</th></tr>\n", $gh);
		echo("</thead>\n");
		
		foreach ($i_lo->lijst($p_onderdeelid, $f, "L.Achternaam, L.Tussenv, L.Roepnaam", $dat) as $row) {
			$stat = $i_aanw->status($row->RecordID, $akid);
			if (strlen($stat) > 0) {
				$cl = sprintf("class='presstat_%s'", strtolower($stat));
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
			
			$options = "<option value=''>Aanwezig</option>\n";
			foreach (ARRPRESENTIESTATUS as $k => $o) {
				$s = "";
				if ($k == $stat) {
					$s = "selected";
				}				
				$options .= sprintf("<option value='%1\$s' %2\$s>%1\$s - %3\$s</option>\n", $k, $s, $o);
			}
			
			printf("<td><select id='Status_%d'>%s</select></td>", $row->RecordID, $options);
			echo("</tr>\n");
		}
		echo("</table>\n");
	}
	echo("</form>\n");
	
	echo("</div> <!-- Einde aanwezigheidmuteren -->\n");
	
	printf("<script>
				$( document ).ready(function() {
					$('select[id^=Status]').change(function() {
						id = this.id;
						var split_id = id.split('_');
						var loid = split_id[1];
						var value = this.value;

						$.ajax({
							url: 'ajax_update.php?entiteit=lo_presentie',
							type: 'post',
							dataType: 'json',
							data: { loid: loid, value: value, akid: %d },
							success:function(response){}
						});
					});
				});
			</script>\n", $akid);
	$i_ak = null;
	$i_aanw = null;
	$i_lo = null;
	
}  # fnAanwezigheidMuteren

function fnPresentiePerLid($p_ondid) {
	$i_lo = new cls_Lidond($p_ondid);
	$i_aw = new cls_Aanwezigheid();
	
	$seizrows = $i_aw->seizoenen($p_ondid);
	
	if ($i_aw->aantalstatus('A', $p_ondid) > 0) {
		$kop_aangemeld = "<th>Aangemeld</th>";
	} else {
		$kop_aangemeld = "";
	}
	
	if ($i_aw->aantalstatus('L', $p_ondid) > 0) {
		$kop_telaat = "<th>Te laat</th>";
	} else {
		$kop_telaat = "";
	}
	
	echo("<div id='presentieperlid'>\n");
	
	echo("<div id='filter'>\n");
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$_POST['filterAanwezigheidsnormOnder'] = $_POST['filterAanwezigheidsnormOnder'] ?? 0;
		$_POST['filterAanwezigheidsnormBoven'] = $_POST['filterAanwezigheidsnormBoven'] ?? 0;
	} else {
		$_POST['filterAanwezigheidsnormOnder'] = 1;
		$_POST['filterAanwezigheidsnormBoven'] = 1;
	}
	
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<label>Filter aanwezigheidsnorm</label>\n");
	printf("<input type='checkbox' name='filterAanwezigheidsnormOnder'%s value=1 OnClick='this.form.submit();'><p>Onder</p>\n", checked($_POST['filterAanwezigheidsnormOnder']));
	printf("<input type='checkbox' name='filterAanwezigheidsnormBoven'%s value=1 OnClick='this.form.submit();'><p>Boven</p>\n", checked($_POST['filterAanwezigheidsnormBoven']));
	echo("</div> <!-- Einde filter -->\n");
	echo("</form>\n");
	
	echo("<table>\n");
	printf("<caption> Presentie per lid %s</caption>\n", $i_lo->ondnaam);
	foreach ($seizrows as $seizrow) {
		printf("<tr class='seizoentotaal'><th class='teken'>-</th><th>%d</th><th>%s t/m %s</th><th>Groep</th><th># act.</th>", $seizrow->Nummer, strftime('%e %B %Y', strtotime($seizrow->Begindatum)), strftime('%e %B %Y', strtotime($seizrow->Einddatum)));
		printf("%s<th>Afwezig</th><th>%% aanwezig</th><th>Ziek</th><th>Met reden</th><th>Zonder reden</th>%s</tr>\n", $kop_aangemeld, $kop_telaat);
		$lorows = $i_lo->lijst($p_ondid, "", "", $seizrow->Einddatum);
		foreach ($lorows as $lorow) {
			$aanwtotrow = $i_aw->perlidperperiode($lorow->RecordID, $seizrow->Begindatum, $seizrow->Einddatum);
			$lnklid = "";
			if (($aanwtotrow->aantAangemeld + $aanwtotrow->aantAfwezig) > 0) {
				$aa = $seizrow->aantalAct - $aanwtotrow->aantVervallen;
				$awperc = (($aa-$aanwtotrow->aantAfwezig)/$aa)*100;
				$toon = false;
				if ($_POST['filterAanwezigheidsnormBoven'] == 1 and $awperc >= $lorow->Aanwezigheidsnorm) {
					$toon = true;
				} elseif ($_POST['filterAanwezigheidsnormOnder'] == 1 and $awperc < $lorow->Aanwezigheidsnorm) {
					$toon = true;
				}
				if ($toon) {
					$tab = sprintf("%s/Overzicht lid/Afwezigheden", $i_lo->ondnaam);
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

function fnAfdelingsmailing($p_afdid) {
	$i_gr = new cls_Groep($p_afdid);
	$i_m = new cls_Mailing();
	$i_mr = new cls_Mailing_rcpt();
	$i_lo = new cls_Lidond($p_afdid);
	$i_f = new cls_Functie();
	
	$selmailing = $_POST['SelecteerMailing'] ?? 0;
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		if ($selmailing == -1) {
			$selmailing = $i_m->add("Afdelingsmailing " . $i_lo->ondnaam);
		}
		
		if (isset($_POST['btnOntvangersAanpassen']) and $selmailing > 0) {
			$i_mr->delete_all($selmailing);
			foreach ($i_lo->lijst($p_afdid) as $lorow) {
				$cn = sprintf("chkGroep_%d", $lorow->GroepID);
				$cnf = sprintf("chkFunctie_%d", $lorow->FunctieID);
				if (isset($_POST[$cn]) or (isset($_POST['chkGroepAlle']) and $lorow->GroepID > 0)) {
					$i_mr->add($selmailing, $lorow->Lid, "", 0, "AfdGr", $lorow->GroepID);
				} elseif ((isset($_POST[$cnf]) or isset($_POST['chkFunctiesAlle'])) and $lorow->FunctieID > 0) {
					$i_mr->add($selmailing, $lorow->Lid, "", 0);
				}
			}
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
				printf("<li><input type='checkbox' value=1 name='%s'%s><p>%s (%d)</p></li>\n", $cn, checked(getvar($cn)), $o, $grrow->aantalInGroep);
			}
		}
		$cn = "chkGroepAlle";
		printf("<li><input type='checkbox' value=1 name='%s'%s><p>Alle groepen</p></li>\n", $cn, checked(getvar($cn)));
		echo("</ul>\n");
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
	printf("<li><input type='checkbox' value=1 name='%s'%s><p>Alle functies</p></li>\n", $cn, checked(getvar($cn)));
	echo("</ul>\n");
	
	echo("<h2>Selecteer mailing</h2>");
		
	$txtao = "";
	if ($selmailing > 0) {
		$ao = $i_mr->aantalontvangers($selmailing);
		if ($ao == 1) {
			$txtao = " (1 ontvanger)";
		} elseif ($ao == 0) {
			$txtao = " (geen ontvangers)";
		} else {
			$txtao = sprintf(" (%d ontvangers)", $ao);
		}
	}
	printf("<select name='SelecteerMailing' OnChange='this.form.submit();'>\n<option value=0>Selecteer ...</option>\n<option value=-1>*** Nieuwe mailing</option>\n%s</select><p>%s</p>\n", $i_m->htmloptions($selmailing), $txtao);
	
	echo("<div id='opdrachtknoppen'>\n");
	if ($selmailing > 0) {
		echo("<button type='submit' name='btnOntvangersAanpassen'>Ontvangers in mailing aanpassen</button>");
		printf("<button type='button' OnClick=\"location.href='%s?tp=Mailing/Wijzigen mailing&mid=%d'\">Ga naar mailing</button>", $_SERVER['PHP_SELF'], $selmailing);
	}
	
	echo("</div> <! Einde opdrachtknoppen -->\n");
	echo("</form>\n");
	
	echo("</div> <!-- Einde afdelingsmailing -->\n");


}  # fnAfdelingsmailing

?>
