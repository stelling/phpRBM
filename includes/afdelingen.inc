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
	} elseif ($currenttab2 == "Examenresultaten") {
		fnExamenResultaten($afdid);
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
	$kols[1]['columnname'] = "Naam_lid";
	$kols[1]['sortcolumn'] = "L.Achternaam";

//	$kols[2]['headertext'] = "Bereiken";
	$kols[2]['headertext'] = "Email";
	$kols[2]['columnname'] = "Email";

	$f = sprintf("GR.OnderdeelID=%d", $afdid);
	if ((new cls_Groep())->aantal($f) == 0) {
		$kols[3]['headertext'] = "Functie";
	} else {
		$kols[3]['headertext'] = "Functie / groep";
	}
	$kols[3]['columnname'] = "Functie / Groep";

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
		printf("<p class='aantrecords'>%d rijen / %d leden</p>\n", count($rows), aantaluniekeleden($rows, "LidID"));
	}
	echo("</form>\n");
	echo("</div>  <!-- Einde filter -->\n");

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
			$dat = sprintf("<input type='date' id='Datum_%d' title='Datum' value='%s'>", $row->RecordID, $row->Datum);
		} else {
			$dtfmt->setPattern(DTLONGWD);
			$dat = $dtfmt->format(strtotime($row->Datum));
		}
		printf("<tr><td>%s</td><td><input type='text' id='Omschrijving_%d' title='Opmerking' value=\"%s\" maxlength=75 class='w75'></td>", $dat, $row->RecordID, str_replace("\"", "'", $row->Omschrijving));
		
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

function fnGroepsindeling($afdid, $p_muteren=0) {
		
	$arrToonLft[0] = "Nee";
	$arrToonLft[1] = "Ja";
	$arrToonLft[2] = "Tot 18 jaar";
	
	$hvtijd = "";
	$hvgroep = -1;
	$hvzaal = "";
	
	$filter = sprintf("O.RecordID=%d", $afdid);
	
	$afdnaam = (new cls_Onderdeel())->naam($afdid);
	printf("<div id='%s'>\n", strtolower(str_replace(" ", "", $afdnaam)));
	
	if ($p_muteren == 0) {
		
		echo("<div id='groepsindeling'>\n");
		printf("<h2>%s</h2>\n", $afdnaam);
		
		$toonleeftijd = $_POST['toonleeftijd'] ?? 2;
		$avg_naam = $_POST['avg_naam'] ?? 0;
		
		echo("<div id='filter'>\n");
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		$i_ak = new cls_afdelingskalender();
		$toonpresentie = 0;
		$f = sprintf("AK.OnderdeelID=%d AND (SELECT COUNT(*) FROM %sAanwezigheid AS AW WHERE AW.AfdelingskalenderID=AK.RecordID) > 0", $afdid, TABLE_PREFIX);
		if ($i_ak->aantal($f) > 0) {
			$toonpresentie = $_POST['toonpresentie'] ?? $i_ak->min("RecordID", "Datum >= CURDATE() AND " . $f);
			printf("<label>Inclusief afwezigheid</label><select name='toonpresentie' OnChange='this.form.submit();'>\n<option value=0>Geen</option>\n%s</select>", $i_ak->htmloptions($afdid, $toonpresentie, $f));

		}
		printf("<label>Toon leeftijden</label><select name='toonleeftijd' onChange='this.form.submit();'>");
		foreach ($arrToonLft as $k => $val) {
			printf("<option value=%d%s>%s</option>", $k, checked($k, "option", $toonleeftijd), $val);
		}
		echo("</select>\n");
		printf("<label>Zonder achternaam</label><input type='checkbox' name='avg_naam' title='Toon alleen de eerste letter van de achternaam' value=1 onClick='this.form.submit();' %s>", checked($avg_naam));
		echo("</form>\n");
		echo("</div> <!-- Einde filter -->\n");
		echo("<div class='clear'></div>\n");
	
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
			if ($avg_naam == 1) {
				$nm = $row->AVGnaam;
			} else {
				$nm = $row->NaamLid;
			}
			if (strlen($row->Leeftijd) > 5 and ($toonleeftijd == 1 or ($toonleeftijd == 2 and intval(substr($row->Leeftijd, 0, 2)) < 18))) {
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
		$i_act = new cls_Activiteit();

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
				$ld = (new cls_Liddipl())->lidlaatstediplomas($row->LidID, 3);
				printf("<tr><td%s>%s</td><td>%s</td><td>%s</td><td><select id='GroepID_%d'>%s</select></td></tr>\n", $cl, $nm, $row->Leeftijd, $ld, $row->RecordID, $options);
			}
			echo("</table>\n");
		}
		echo("</div>  <!-- Einde groepsindelingmuteren -->\n");
		
		echo("<div id='groepenmuteren'>\n");
		echo("<table>\n");
		echo("<caption>Muteren groepen</caption>\n");
		echo("<tr><th>#</th><th>Volgnr</th><th>Code</th><th>Omschrijving</th><th>Activiteit</th><th>Instructeurs</th><th>Starttijd</th><th>Eindtijd</th><th>Norm aanw.</th>");
		
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
				printf("<td><input type='number' id='Volgnummer_%d' title='Volgnummer groep' value='%s'></td>\n", $row->RecordID, $row->Volgnummer);
				printf("<td><input type='text' class='w8' id='Kode_%d' title='Code groep' maxlength=8 value=\"%s\"></td>\n", $row->RecordID, $row->Kode);
				printf("<td><input type='text' class='w45' id='Omschrijving_%d' title='Omschrijving groep' maxlength=45 value=\"%s\"></td>\n", $row->RecordID, $row->Omschrijving);
				
				printf("<td><select id='ActiviteitID_%d'>%s</td>\n", $row->RecordID, $i_act->htmloptions($row->ActiviteitID));
				
				printf("<td><input type='text' class='w55' id='Instructeurs_%d'  title='Instructeurs groep' maxlength=60 value=\"%s\"></td>\n", $row->RecordID, $row->Instructeurs);
				printf("<td><input type='time' id='Starttijd_%d'  title='Starttijd' value='%s'></td>\n", $row->RecordID, $row->Starttijd);
				printf("<td><input type='time' id='Eindtijd_%d'  title='Eindtijd' value='%s'></td>\n", $row->RecordID, $row->Eindtijd);
				printf("<td><input type='number' id='Aanwezigheidsnorm_%d'  title='Aanwezigheidsnorm' value=%d></td>\n", $row->RecordID, $row->Aanwezigheidsnorm);
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
		printf("<option value=%d %s>%s</option>\n", $row->RecordID, checked($row->RecordID, "option", $akid), $dtfmt->format(strtotime($row->Datum)));
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
	
	if ($akid > 0) {
	
		echo("<input type='text' placeholder='Naam of groep bevat' class='w30' OnKeyUp=\"fnFilter('presentiemuteren', this);\">\n");
		$row = null;
	
		echo("<div class='clear'></div>\n");
	
		printf("<input type='hidden' name='akid' value=%d>\n", $akid);
		$ro = "";
		$f = sprintf("AfdelingskalenderID=%d", $akid);
		if ($i_aanw->aantal($f) > 0) {
			$ro = "readonly";
		}
		
		echo("<table id='presentiemuteren'>\n");
		printf("<caption>Presentielijst %s</caption>\n", $dtfmt->format(strtotime($dat)));

		$gh = "";
		echo("<thead>\n");
		echo("<tr><th>Naam</th><th>Groep</th><th>Status presentie</th></tr>\n");
		echo("</thead>\n");
		
		foreach ($i_lo->lijst($p_onderdeelid, "", "GR.Volgnummer, GR.Kode", $dat) as $row) {
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
		$_POST['100aanwezigTonen'] = $_POST['100aanwezigTonen'] ?? 0;
	} else {
		$_POST['filterAanwezigheidsnormOnder'] = 1;
		$_POST['filterAanwezigheidsnormBoven'] = 1;
		$_POST['100aanwezigTonen'] = 0;
	}
	
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<label>Filter</label>\n");
	printf("<input type='checkbox' name='filterAanwezigheidsnormOnder'%s value=1 OnClick='this.form.submit();'><p>Onder norm</p>\n", checked($_POST['filterAanwezigheidsnormOnder']));
	printf("<input type='checkbox' name='filterAanwezigheidsnormBoven'%s value=1 OnClick='this.form.submit();'><p>Boven norm</p>\n", checked($_POST['filterAanwezigheidsnormBoven']));
	printf("<input type='checkbox' name='100aanwezigTonen'%s value=1 OnClick='this.form.submit();'><p>100%% aanwezig</p>\n", checked($_POST['100aanwezigTonen']));
	echo("</div> <!-- Einde filter -->\n");
	echo("</form>\n");
	
	echo("<table>\n");
	printf("<caption> Presentie per lid %s</caption>\n", $i_lo->ondnaam);
	foreach ($seizrows as $seizrow) {
		printf("<tr class='seizoentotaal'><th class='teken'>-</th><th>%d</th><th>%s t/m %s</th><th>Groep</th><th># act.</th>", $seizrow->Nummer, $dtfmt->format(strtotime($seizrow->Begindatum)), $dtfmt->format(strtotime($seizrow->Einddatum)));
		printf("%s<th>Afwezig</th><th>%% aanwezig</th><th>Ziek</th><th>Met reden</th><th>Zonder reden</th>%s</tr>\n", $kop_aangemeld, $kop_telaat);
		$lorows = $i_lo->lijst($p_ondid, "", "", $seizrow->Einddatum);
		foreach ($lorows as $lorow) {
			$aanwtotrow = $i_aw->perlidperperiode($lorow->RecordID, $seizrow->Begindatum, $seizrow->Einddatum);
			$lnklid = "";
			if ($lorow->Invalfunctie == 1) {
				$aa = $aanwtotrow->aantAangemeld + $aanwtotrow->aantLaat;
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
			if ($aa > 0 and (($aanwtotrow->aantAangemeld + $aanwtotrow->aantAfwezig) > 0 or $_POST['100aanwezigTonen'] == 1)) {
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
	$groepgewijzigdna = $_POST['groepgewijzigdna'] ?? "1900-01-01";
	if ($groepgewijzigdna < "2000-01-01") {
		$groepgewijzigdna = substr((new cls_Logboek())->min("DatumTijd", "TypeActiviteit=19"), 0, 10);
	}
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		if ($selmailing == -1) {
			$selmailing = $i_m->add("Afdelingsmailing " . $i_lo->ondnaam);
		}
		
		if (isset($_POST['btnOntvangersAanpassen']) and $selmailing > 0) {
			$i_mr->delete_all($selmailing);
			foreach ($i_lo->lijst($p_afdid) as $lorow) {
				$cn = sprintf("chkGroep_%d", $lorow->GroepID);
				$cnf = sprintf("chkFunctie_%d", $lorow->FunctieID);
				if (isset($_POST[$cn])) {
					if ($lorow->LaatsteGroepMutatie >= $groepgewijzigdna) {
						$i_mr->add($selmailing, $lorow->Lid, "", 0, "AfdGr", $lorow->GroepID);
					}
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
		echo("<li><button type='button' onClick='fnAlleGroepen();'>Alle groepen</button></li>\n");
		echo("</ul>\n");
		printf("<label>Groep gewijzigd op of na</label><input type='date' name='groepgewijzigdna' value='%s'>\n", $groepgewijzigdna);
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

function fnExamenResultaten($p_afdid) {
	
	$i_dp = new cls_Diploma();
	$i_org = new cls_Organisatie();
	$i_ld = new cls_Liddipl();
	$i_lid = new cls_Lid();
	
	$f = sprintf("DP.Afdelingsspecifiek=%d", $p_afdid);
	$dprows = $i_dp->basislijst($f, "DP.Kode");
	
	$dpid = $_POST['selecteerdiploma'] ?? 0;
	if ($dpid == 0) {
		$dpid = $i_dp->min("RecordID", $f . " AND (DP.EindeUitgifte IS NULL)");
	}
	$exdatum = $_POST['exdatum'] ?? date("Y-m-d");
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
	printf("<select name='selecteerdiploma' onChange='this.form.submit();'>%s</select>\n", $i_dp->htmloptions($dpid, -1, 0, 0, $f));
	echo("</div> <!-- Einde filter -->\n");
	
	echo("<div id='diplomamuteren'>\n");
	
	$dprow = $i_dp->record($dpid);
	
	printf("<label>RecordID</label><input type='text' id='RecordID' readonly disabled value=%d>\n", $dprow->RecordID);
	
	printf("<label>Naam</label><input type='text' id='Naam' class='w75' maxlength=75 value=\"%s\">\n", str_replace("\"", "'", $dprow->Naam));
	printf("<label class='k2'>Code</label><input type='text' id='Kode' class='w10' maxlength=10 value=\"%s\">\n", $dprow->Kode);
	
	printf("<label>Type</label><select id='Type'>%s</select>\n", fnOptionsFromArray(ARRTYPEDIPLOMA, $dprow->Type));
	printf("<label class='k2'>Uitgegeven door</label><select id='ORGANIS'>%s</select>\n", $i_org->htmloptions(1, $dprow->ORGANIS));
	
	printf("<label>Volgnummer</label><input type='number' id='Volgnr' value=%d class='d8'>\n", $dprow->Volgnr);
	printf("<label class='k2'>Einde uitgifte</label><input type='date' id='EindeUitgifte' name='EindeUitgifte' value='%s'>\n", $dprow->EindeUitgifte);
	
	echo("</div> <!-- Einde diplomamuteren -->\n");
	
	echo("<div class='clear'></div>\n");
	
	echo("<div id='examenresultaten'>\n");
	
	echo("<h2>Resultaat</h2>\n");
	printf("<label>Examendatum</label><input type='date' name='exdatum' value='%s' onBlur='this.form.submit();'>\n", $exdatum);
	printf("<label>Examenplaats</label><input type='text' name='explaats' value='%s' class='w22' onBlur='this.form.submit();'>\n", $explaats);
	
	echo("<div class='clear'></div>\n");
	
	echo("<table>\n");
	
	echo("<tr><th>Lid</th><th>Examenplaats</th><th>Diplomanr</th><th></th></tr>\n");
	
	$ldrows = $i_ld->overzichtperexamen($exdatum, $dpid);
	foreach ($ldrows as $ldrow) {
		printf("<tr><td id='naam_%2\$d'>%1\$s</td><td><input type='text' id='EXPLAATS_%2\$d' value='%3\$s'></td><td><input type='text' id='Diplomanummer_%2\$d' value='%4\$s'></td>", $ldrow->NaamLid, $ldrow->RecordID, $ldrow->EXPLAATS, $ldrow->Diplomanummer);
		$jsdo = sprintf("OnClick=\"liddipl_verw(%d);\"", $ldrow->RecordID);
		printf("<td><img src='%s' alt='Verwijderen' title='Verwijderen %s' %s></td>", BASE64_VERWIJDER_KLEIN, htmlentities($ldrow->NaamLid), $jsdo);
		echo("</tr>\n");
	}
	
	if ($eindeuitgifte >= $exdatum and $exdatum <= date("Y-m-d")) {
		printf("<tr><td><select name='ldtoevoegen' onChange='this.form.submit();'><option value=0>Toevoegen ....</option>\n%s</select></td></tr>\n", $i_lid->htmloptions(-1, 7, "", $exdatum, $p_afdid));
	}
	
	echo("</table>\n");
	echo("</form>\n");
	echo("</div> <!-- Einde examenresultaten -->\n");
?>
<script>

	$("#Naam, #Kode, #Volgnr").blur(function(){
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
