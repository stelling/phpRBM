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
	} elseif ($currenttab2 == "Aanwezigheid muteren") {
		fnAanwezigheidMuteren($afdid);
	}
	
}  # fnAfdeling

function fnAfdelingslijst($afdid) {
	
	$afdnm = (new cls_Onderdeel())->naam($afdid);
	(new cls_lidond())->auto_einde($afdid);
	$i_dp = new cls_Diploma();
	
	$arrSort[] = "Naam lid;L.Achternaam";
	$arrSort[] = "Woonplaats;L.Woonplaats";
	$arrSort[] = "Functie;F.Omschrijv";
	$arrSort[] = "Groep;GR.Volgnummer";
	$arrSort[] = "Opmerking;LO.Opmerk";
	$arrSort[] = "Vanaf;LO.Vanaf";
	$arrSort[] = "Totenmet;LO.Opgezegd";
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$diplfilter = $_POST['bezitdiploma'];
	} else {
		$diplfilter = -1;
	}
	
	$xf = "";
	if ($diplfilter > 0) {
		$xf = sprintf("L.RecordID IN (SELECT LD.Lid FROM %sLiddipl AS LD WHERE LD.DiplomaID=%d AND IFNULL(LD.LicentieVervallenPer, CURDATE()) >= CURDATE())", TABLE_PREFIX, $diplfilter);
	}
	
	$rows = (new cls_Lidond())->onderdeellijst($afdid, 2, $xf, fnOrderBy($arrSort));
	
	echo("<div id='filter'>\n");
	printf("<form action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);

	print("<input type='text' name='tbTekstFilter' id='tbTekstFilter' placeholder='Tekstfilter' OnKeyUp=\"fnFilter('afdelingslijst', 'tbTekstFilter', 1, 2, 3);\">\n");
	printf("<select name='bezitdiploma' id='bezitdiploma' onchange='this.form.submit();'>\n<option value=-1>Filter op diploma</option>\n%s</select>\n", $i_dp->htmloptions($diplfilter, -1, $afdid));
	if (count($rows) > 1) {
		printf("<p class='aantrecords'>%d rijen / %d leden</p>\n", count($rows), aantaluniekeleden($rows, "ndLidID"));
	}
	echo("</form>\n");
	echo("</div>  <!-- Einde filter -->\n");
	
		
	$kols=null;
	if (toegang($afdnm . "/Overzicht lid", 0, 0)) {
		$kols[0]['link'] = "<a href='index.php?tp=" . $afdnm . "/Overzicht+lid&lidid=%d'>%s</a>";
		$kols[0]['class'] = "details";
	}

	if (count($rows) > 0) {
		echo(fnDisplayTable($rows, $kols, "", 0, "", "afdelingslijst", "", "", 0, $arrSort));
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
		
		foreach ($i_ak->lijst($p_onderdeelid) as $row) {
			$cntr = sprintf("Activiteit_%d", $row->RecordID);
			if (isset($_POST[$cntr])) {
				$_POST[$cntr] = 1;
			} else {
				$_POST[$cntr] = 0;
			}
			foreach (array("Datum", "Omschrijving", "Activiteit") as $fld) {
				$cntr = sprintf("%s_%d", $fld, $row->RecordID);
				if (isset($_POST[$cntr])) {
					$i_ak->update($row->RecordID, $fld, $_POST[$cntr]);
				}
			}
		}
		
		if (isset($_POST['nieuw'])) {
			$akid = $i_ak->add($p_onderdeelid);
		}
	}
	
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	
	echo("<table>\n");
	$dat = "";
	$oms = "";
	$act = false;
	echo("<tr><th>Datum</th><th>Opmerking</th><th>Activiteit?</th><th># Aanwezig</th><th></th></tr>\n");
	
	foreach ($i_ak->lijst($p_onderdeelid) as $row) {
		$aw = $i_aanw->aantal(sprintf("AfdelingskalenderID=%d", $row->RecordID));
		
		if ($aw == 0) {
			$dat = sprintf("<input type='date' name='Datum_%d' value='%s'>", $row->RecordID, $row->Datum);
		} else {
			$dat = strftime("%A %e %B %Y", strtotime($row->Datum));
		}
		
		$d = "<td></td>";
		if ($aw == 0) {
			$d = sprintf("<td class='trash'><a href='%s?tp=%s&KalID=%d&op=delete'>&nbsp;&nbsp;&nbsp;</a></td>", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
		}
		
		$chkAct = sprintf("<input type='checkbox' name='Activiteit_%d' value=1 %s onChange='this.form.submit();'>", $row->RecordID, checked($row->Activiteit));
		
		printf("<tr><td>%2\$s</td><td><input type='text' name='Omschrijving_%1\$d' value='%3\$s' onblur='this.form.submit();'></td><td>%4\$s</td><td>%5\$d</td>%6\$s</tr>\n", $row->RecordID, $dat, $row->Omschrijving, $chkAct, $aw, $d);
		if ($akid == $row->RecordID) {
			$dat = $row->Datum;
			$oms = $row->Omschrijving;
			if ($row->Activiteit == 1) {
				$act = true;
			}
		}
	}
	
	echo("</table>\n");
	
	echo("<button type='submit' name='nieuw'>Nieuw item</button>\n");
	echo("</form>\n");
	
	echo("</div> <!-- Einde afdelingskalendermuteren -->\n");
	$i_ak = null;
	$i_lo = null;
	
}  # fnAfdelingskalenderMuteren

function fnGroepsindeling($afdid, $p_muteren=0, $p_toonleeftijd=2) {
	/* p_toonleeftijd
		0: nee
		1: ja, alle
		2: alleen bij jonger dan 18 jaar
	*/
	
	$hvafd = "";
	$hvtijd = "";
	$hvgroep = -1;
	$hvzaal = "";
	
	if ($afdid > 0) {
		$filter = sprintf("O.RecordID=%d", $afdid);
	} else {
		$filter = "";
	}
	
	if ($p_muteren == 0) {
		
		echo("<div id='groepsindeling'>\n");
		
		$i_ak = new cls_afdelingskalender();
		$toonpresentie = 0;
		$f = sprintf("OnderdeelID=%d AND Activiteit=1", $afdid);
		if ($i_ak->aantal($f) > 0) {
			if (isset($_POST['toonpresentie'])) {
				$toonpresentie = $_POST['toonpresentie'];
			} else {
				$f = "Datum >= CURDATE()";
				$toonpresentie = $i_ak->min("RecordID", $f);
			}
			printf("<form name='grpfilter' method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
			printf("<label>Inclusief aanwezigheid</label><select name='toonpresentie' OnChange='this.form.submit();'>\n<option value=0>Geen</option>\n%s</select>", $i_ak->htmloptions($afdid, $toonpresentie, "Activiteit=1"));
			echo("</form>\n");
			echo("<div class='clear'></div>\n");
		}
	
		foreach ((new cls_Lidond())->groepsindeling($afdid) as $row) {
			if ($hvafd !== $row->AfdNaam) {
				if (strlen($hvafd) > 0) {
					echo("<div class='clear'></div>\n");
				}
				$hvafd = $row->AfdNaam;
				printf("<h2>%s</h2>\n", $hvafd);
				
			}
		
			if ($hvgroep != $row->GroepID) {
				if ($hvgroep > -1) {
					echo("</ol>\n");
					echo("</div>  <!-- einde groepsindelingkolom -->\n");
					if ($hvtijd !== $row->Tijdsblok) {
//						echo("<div class='clear'></div>\n");
					}
				}
				echo("<div class='groepsindelingkolom'>\n");
			
				if (strlen($row->Tijdsblok) > 3 and $hvtijd !== $row->Tijdsblok) {
					printf("<h3>%s</h3>\n", $row->Tijdsblok);
				}
				$hvtijd = $row->Tijdsblok;
			
				printf("<h4>%s</h4>\n", $row->GroepOms);
				echo("<ol>\n");
			}
			$cl = "";
			if ($toonpresentie > 0) {
				$stat = (new cls_Aanwezigheid())->status($row->RecordID, $toonpresentie);
				if (strlen($stat) > 0) {
					$cl = sprintf("presstat_%s", strtolower($stat));
				}
			}
			if ($row->Opgezegd < date("Y-m-d", strtotime("+3 MONTH"))) {
				$cl .= " opgezegd";
			}
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
		
		$kn[] = "Volgnummer";
		$kn[] = "Kode";
		$kn[] = "Omschrijving";
		$kn[] = "Instructeurs";
		$kn[] = "Starttijd";
		$kn[] = "Eindtijd";
		$kn[] = "Activiteit";
		
		$i_lo = new cls_Lidond();
		$i_gr = new cls_Groep();
			
		if ($_SERVER['REQUEST_METHOD'] == "POST") {
			//Bijwerken groepsindeling
			foreach ($i_lo->groepsindeling($afdid) as $row) {
				$p = sprintf("mg_%d", $row->RecordID);
				if (isset($_POST[$p])) {
					$i_lo->update($row->RecordID, "GroepID", $_POST[$p]);
				}
			}
			
			//Bijwerken afdelingsgroepen
			foreach ($i_gr->selectlijst($afdid) as $row) {
				foreach ($kn as $key => $vn) {
					$pn = sprintf("%s_%d", $vn, $row->RecordID);
					if (isset($_POST[$pn])) {
						$i_gr->update($row->RecordID, $vn, $_POST[$pn]);
					}
				}
			}
			
			// Afdelingsgroep toevoegen
			if (isset($_POST['NieuweGroep'])) {
				$i_gr->add($afdid);
			}
		}
		
		echo("<div id='groepsindelingmuteren'>\n");
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		
		$grrows = $i_gr->selectlijst($afdid);
		if (count($grrows) > 0) {
			echo("<table>\n");
			echo("<tr><th>Naam</th><th>Leeftijd</th><th>Laatst behaalde diploma's</th><th>Groep</th></tr>\n\n");
			foreach ($i_lo->onderdeellijst($afdid, 2, "", "GR.Volgnummer, GR.Starttijd, GR.Omschrijving, L.Achternaam, L.Tussenv, L.Roepnaam") as $row) {
				$options = "<option value=0>Niet ingedeeld</option>\n";
				foreach ($grrows as $grrow) {
					$options .= sprintf("<option value=%d %s>%s</option>\n", $grrow->RecordID, checked($row->ndGroepID, "option", $grrow->RecordID), $grrow->Omschrijving);
				}
				$nm = $row->Naam_lid;
				if (strlen($row->ndAfkFunc) > 0) {
					$nm .= " (" . $row->ndAfkFunc . ")";
				}
				$ld = (new cls_Liddipl())->lidlaatstediplomas($row->ndLidID, 3);
				printf("<tr><td>%s</td><td>%s</td><td>%s</td><td><select name='mg_%d' OnChange='this.form.submit();'>%s</select></td></tr>\n", $nm, $row->ndLeeftijd, $ld, $row->ndRecordID, $options);
			}
			echo("</table>\n");
			echo("</div>  <!-- Einde groepsindelingmuteren -->\n");
		}
		
		echo("<a name='groepenmuteren'>\n");
		echo("<div id='groepenmuteren'>\n");
		echo("<table>\n");
		echo("<caption>Muteren groepen</caption>\n");
		echo("<tr><th>#</th><th>Volgnr</th><th>Code</th><th>Omschrijving</th><th>Instructeurs</th><th>Starttijd</th><th>Eindtijd</th><th>Activiteit</th></tr>\n");
		
	
		$f = sprintf("OnderdeelID=%d", $afdid);
		foreach ($i_gr->basislijst($f, "Volgnummer, Omschrijving") as $row) {
			if ($row->RecordID > 0) {
				echo("<tr>\n");
				printf("<td>%d</td>\n", $row->RecordID);
				printf("<td><input type='number' name='Volgnummer_%d' value='%s'></td>\n", $row->RecordID, $row->Volgnummer);
				printf("<td><input type='text' class='w8' name='Kode_%d' maxlength=8 value=\"%s\"></td>\n", $row->RecordID, $row->Kode);
				printf("<td><input type='text' class='w45' name='Omschrijving_%d' maxlength=45 value=\"%s\"></td>\n", $row->RecordID, $row->Omschrijving);
				printf("<td><input type='text' class='w60' name='Instructeurs_%d' maxlength=60 value=\"%s\"></td>\n", $row->RecordID, $row->Instructeurs);
				printf("<td><input type='time' name='Starttijd_%d' value='%s'></td>\n", $row->RecordID, $row->Starttijd);
				printf("<td><input type='time' name='Eindtijd_%d' value='%s'></td>\n", $row->RecordID, $row->Eindtijd);
				printf("<td>%s</td>\n", $row->Activiteit);
				
				echo("</tr>\n");
			}
		}
		
		echo("</table>\n");
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<input type='submit' value='Groepen bewaren'>\n");
		echo("<button name='NieuweGroep' type='submit' value='NieuweGroep'>Nieuwe groep</button>\n");
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		
		echo("</div>  <!-- Einde groepenmuteren -->\n");
		
		echo("</form>\n");
	}
	
} # fnGroepsindeling

function fnAanwezigheidMuteren($p_onderdeelid){
	
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
	echo("<select name='selecteerdatum' Onblur='this.form.submit();'>\n");
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
		printf("<tr><th colspan=2>Presentielijst %s</th></tr>\n", strftime("%A %e %B %Y", strtotime($dat)));
		echo("<tr><th>Naam</th><th>Aanwezigheid</th></tr>\n");
		
		$f = "";
		if ($grid >= 0) {
			$f = sprintf("LO.GroepID=%d", $grid);
		}
		
		foreach ($i_lo->lijst($p_onderdeelid, $f, "L.Achternaam, L.Tussenv, L.Roepnaam", $dat) as $row) {
			$stat = $i_aanw->status($row->RecordID, $akid);
			if (strlen($stat) > 0) {
				$cl = sprintf("class='presstat_%s'", strtolower($stat));
			} else {
				$cl = "";
			}
			$nm = $row->NaamLid;
			if (strlen($row->GrCode) > 0 and $grid < 0) {
				$nm .= sprintf(" (%s)", $row->GrCode);
			} elseif (strlen($row->FunctAfk) > 0) {
				$nm .= sprintf(" (%s)", $row->FunctAfk);
			}
			printf("<tr><td %s>%s</td>\n", $cl, $nm);
			
			$options = "<option value=''>Onbekend</option>\n";
			foreach (ARRPRESENTIESTATUS as $k => $o) {
				$s = "";
				if ($k == $stat) {
					$s = "selected";
				}				
				$options .= sprintf("<option value='%s' %s>%s</option>\n", $k, $s, $o);
			}
			
			printf("<td><select name='aanw_%d' OnChange='this.form.submit();'>%s</select></td>", $row->RecordID, $options);
			echo("</tr>\n");
		}
		echo("</table>\n");
	}
	echo("</form>\n");
	
	echo("</div> <!-- Einde aanwezigheidmuteren -->\n");
	$i_ak = null;
	$i_aanw = null;
	$i_lo = null;
	
}  # fnAanwezigheidMuteren

?>