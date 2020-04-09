<?php

function fnBewaking() {
	global $ldl, $currenttab2;
	
	fnDispMenu(2);
		
	if (isset($_POST['lidid']) and $_POST['lidid'] > 0) {
		$lidid = $_POST['lidid'];
	} else {
		$lidid = 0;
	}
	
	if (isset($_POST['dptype']) and strlen($_POST['dptype']) > 0) {
		$dptype = $_POST['dptype'];
	} else {
		$dptype = "";
	}

	if (isset($_POST['seizoen']) and strlen($_POST['seizoen']) > 0) {
		$seizoen = $_POST['seizoen'];
		$_SESSION['actseizoen'] = $seizoen;
	} elseif (isset($_SESSION['actseizoen']) and strlen($_SESSION['actseizoen']) > 0) {
		$seizoen = $_SESSION['actseizoen'];
	} elseif ($currenttab2 == "Overzicht inschrijvingen" or $currenttab2 == "Blokken muteren") {
		$seizoen = db_scalar(sprintf("SELECT MAX(SeizoenID) FROM %sBewaking_Blok;", TABLE_PREFIX));
	} else {
		$seizoen = db_scalar(sprintf("SELECT MAX(SeizoenID) FROM %sBewaking;", TABLE_PREFIX));
	}
	
	$hw = db_bewaking("huidigeweek", $seizoen);
	if (isset($_POST['week']) and strlen($_POST['week']) > 0) {
		$week = $_POST['week'];
		$_SESSION['actweek'] = $week;
	} elseif (isset($_SESSION['actweek']) and strlen($_SESSION['actweek']) > 0) {
		$week = $_SESSION['actweek'];
	} elseif ($hw > 0) {
		$week = $hw;
	} else {
		$week = "*";
	}
	
	$st_len_bp = 1;
	
	if ($currenttab2 == "Downloaden inschrijvingen") {
		$toonfilter = false;
	} else {
		echo("<div id='filter'>\n");
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		echo("<table>\n<tr>\n");
		$toonfilter = true;
	}	
	
	if ($toonfilter == false) {
		// Geen filter tonen
	} elseif ($currenttab2 == "Diploma's muteren" or $currenttab2 == "Nieuwe pasfoto") {
		$filterlid = sprintf("L.RecordID IN (SELECT Lid FROM %1\$sBewaking AS BW INNER JOIN %1\$sBewseiz AS BS ON BW.SeizoenID=BS.RecordID WHERE BS.Afgesloten=0)", TABLE_PREFIX);
		$optionslid = "<option value=-1>...</option>\n";
		foreach (db_lid("lijst", $filterlid) as $row) {
			if ($row->RecordID == $lidid) {
				$s = "selected";
			} else {
				$s = "";
			}
			$optionslid .= sprintf("<option value=%1\$d %3\$s>%2\$s</option>\n", $row->RecordID, htmlentities($row->Zoeknaam), $s);
		} # foreach
		echo("<td class='label'>Selecteer lid:</td>\n");
		printf("<td><select name='lidid' onChange='this.form.submit();'>%s</select></td>\n", $optionslid);
		if ($currenttab2 == "Diploma's muteren") {
			$dptypes['*'] = "Alle types";
			$dptypes['B'] = "Brevetten";
			$dptypes['D'] = "Diploma's";
			$dptypes['I'] = "Insignes";
			$dptypes['L'] = "Licenties";
			$dptypes['P'] = "Proeven van bekwaamheden";
			$dptypes['ZS'] = "Zelfservice";
			$options = "";
			foreach($dptypes as $key => $val) {
				if ($key == $dptype) {
					$s = "selected";
				} else {
					$s = "";
				}
				$options .= sprintf("<option value='%s' %s>%s</option>\n", $key, $s, $val);
			}
			echo("<td class='label'>Type diploma:</td>\n");
			printf("<td><select name='dptype' onChange='this.form.submit();'>%s</select></td>\n", $options);
		}
	
	} elseif ($currenttab2 == "Logboek") {
		if (!isset($_POST['lidfilter']) or strlen($_POST['lidfilter']) == 0) {
			$_POST['lidfilter'] = 0;
		}
		echo("<td class='label'>Filter op lid:</td><td><select name='lidfilter' id='lidfilter' onchange='this.form.submit();'>\n");
		echo("<option value=-1>Alle</option>\n");
		foreach (db_logboek("lidlijst", "", 6) as $row) {
			if ($row->LidID == $_POST['lidfilter']) {
				$s = "selected";
			} else {
				$s = "";
			}
			printf("<option %s value=%d>%s</option>\n", $s, $row->LidID, htmlentities($row->Naam));
		}
		echo("</select>\n</td>\n");
	} else {
		echo("<td class='label'>Bewakingseizoen:</td>");
		echo("<td><select name='seizoen' id='seizoen' onchange='form.submit();'>\n");
		if ($currenttab2 == "Overzicht inschrijvingen") {
			echo("<option value=-1>Alle</option>\n");
		}
		
		if ($currenttab2 == "Overzicht inschrijvingen") {
			$bs_res=db_bewaking("insseizoenen");
		} else {
			$bs_res=db_bewaking("seizoenen");
		}
		foreach($bs_res->fetchAll() as $row) {
			if ($seizoen == $row->RecordID or $seizoen == 0) {
				$seizoen = $row->RecordID;
				$sel = " selected";
				$st_len_bp = $row->ST_LEN_BP;
			} else {
				$sel = "";
			}
			printf("<option value=%d%s>%s</option>\n", $row->RecordID, $sel, $row->Kode);
		}
		echo("</select>\n</td>\n");
		
		if ($currenttab2 != "Aantallen" and $currenttab2 != "Blokken muteren") {
			echo("<td class='label'>Bewakingsweek:</td>\n");
			echo("<td><select name='week' id='week' onchange='this.form.submit();'>\n");
			print("<option value=-1>Alle</option>\n");
			if ($currenttab2 == "Overzicht inschrijvingen") {
				$rows = db_bewaking_blok("overzicht", $seizoen);
			} else {
				$rows = db_bewaking("aantallen", $seizoen);
			}
			foreach($rows as $row) {
				if ($week == $row->intWeeknr or strlen($week) == 0) {
					$week = $row->intWeeknr;
					$sel = " selected";
				} else {
					$sel = "";
				}
				printf("<option value=%1\$d%2\$s>%1\$d</option>\n", $row->intWeeknr, $sel);
			}
			echo("</select>\n</td>\n");
		}
	}
	if ($toonfilter) {
		echo("</tr>\n</table>\n");
		echo("</form>\n");
		echo("</div>  <!-- Einde filter -->\n");
	}
	
	if ($currenttab2 === "Bewakingsrooster" and toegang($_GET['tp'])) {
		bew_rooster($seizoen, $week);
	} elseif ($currenttab2 == "Postindeling") {
		bew_postindeling($seizoen, $week);
	} elseif ($currenttab2 == "Aantallen" and toegang($_GET['tp'])) {
		$lijst = db_bewaking("aantallen", $seizoen);
		echo(fnDisplayTable($lijst, "", "", 0, "", "", "bewakingsaantallen"));
	} elseif ($currenttab2 == "Nieuwe pasfoto" and $lidid > 0) {
		$filterlid = sprintf("L.RecordID IN (SELECT Lid FROM %1\$sBewaking AS BW INNER JOIN %1\$sBewseiz AS BS ON BW.SeizoenID=BS.RecordID WHERE BS.Afgesloten=0)", TABLE_PREFIX);
		nieuwepasfoto($lidid, $filterlid);
	} elseif ($currenttab2 == "Diploma's muteren" and toegang($_GET['tp']) and $lidid > 0) {
		diplomaslidmuteren($lidid, $dptype, 0);
	} elseif ($currenttab2 == "Blokken muteren" and toegang($_GET['tp'])) {
		blokkenmuteren($seizoen);
	} elseif ($currenttab2 == "Overzicht inschrijvingen" and toegang($_GET['tp'])) {
		$lijst = db_insbew("matrixaantallen", 0, 0, 0, 0, "", $seizoen);
		echo(fnDisplayTable($lijst, "", "Totalen"));
		$lijst = db_insbew("overzicht", 0, 0, 0, 0, "", $seizoen, $week);
		echo(DetailsInschrijvingBewaking($lijst));
	} elseif ($currenttab2 == "Downloaden inschrijvingen" and toegang($_GET['tp'])) {
		exportinschrijvingen();
	} elseif ($currenttab2 == "Logboek" and toegang($_GET['tp'])) {
		$lijst = db_logboek("lijst", "", 11, $_POST['lidfilter']);
		echo(fnDisplayTable($lijst));
	}
} # fnBewaking

function bew_rooster($seizoen, $week=-1) {

	$rooster = db_bewaking("rooster", $seizoen, 0, $week);
	$wk = "";
	$aantalbewdag = 0;
	$aantalond = db_onderdelen("aantal", "bw");
	if ($aantalond > 1) {
		$bastotreg = "<tr>\n<td colspan='3'><b>Totaal: %d bewakers</b></td>\n<td class='number'><b>%s</b></td>\n<td>&nbsp;</td>\n</tr>\n";
	} else {
		$bastotreg = "<tr>\n<td colspan='3'><b>Totaal: %d bewakers</b></td>\n<td class='number'><b>%s</b></td>\n</tr>\n";
	}
	echo("<div id='bewakingsrooster'>\n");
	echo("<table>\n");
	foreach($rooster as $row) {
		if (($wk != $row->Kode . "-" . $row->Weeknr and $row->ST_LEN_BP > 1) or ($wk != strftime("%A %e %B %Y", strtotime($row->BEGIN_PER)) and $row->ST_LEN_BP == 1)) {
			if ($aantalbewdag > 2) {
				if ($row->TOONERV == "W") {
					$erv = round($aantalervdag/$aantalbewdag, 2) . " weken";
				} else {
					$erv = round(($aantalervdag/$aantalbewdag)*7, 1) . " dagen";
				}
				printf($bastotreg, round($aantalbewdag, 0), $erv);
			}
			$aantalbewdag = 0;
			$aantalervdag = 0;
			if ($row->ST_LEN_BP > 1) {
				$wk = $row->Kode . "-" . $row->Weeknr;
			} else {
				$wk = strftime("%A %e %B %Y", strtotime($row->BEGIN_PER));
			}
			if (strlen($row->Lokatie) > 0) {
				$hdr = $row->Lokatie . ": " . $wk;
			} else {
				$hdr = $wk;
			}
			if ($aantalond > 1) {
				printf("<tr>\n<th>%s</th><th>Post</th><th>Functie</th><th>Ervaring</th><th>Rollen</th></tr>\n", $hdr);
			} else {
				printf("<tr>\n<th>%s</th><th>Post</th><th>Functie</th><th>Ervaring</th></tr>\n", $hdr);
			}
		}
		if ($aantalond > 1) {
			printf("<tr><td>%s</td><td>%s</td><td>%s</td><td class='number'>%s</td><td>%s</td></tr>\n",
					fnDispBW($row, "Naam"), $row->Post, $row->OmsFunctie, fnDispBW($row, "Erv"), fnDispBW($row, "Rollen"));
		} else {
			printf("<tr><td>%s</td><td>%s</td><td>%s</td><td class='number'>%s</td></tr>\n",
						fnDispBW($row, "Naam"), $row->Post, $row->OmsFunctie, fnDispBW($row, "Erv"));
		}
		$aantalbewdag += ($row->Dagen / $row->ST_LEN_BP);
		$aantalervdag += $row->ErvaringDagen/7 * ($row->Dagen / $row->ST_LEN_BP);
		$sel_leden[] = $row->RecordID;
	} # foreach
	if ($aantalbewdag > 2) {
		if ($row->TOONERV == "W") {
			$erv = round($aantalervdag/$aantalbewdag, 2) . " weken";
		} else {
			$erv = round(($aantalervdag/$aantalbewdag)*7, 1) . " dagen";
		}
		printf($bastotreg, round($aantalbewdag, 0), $erv);
	}
	echo("</table>\n");
	echo("</div>  <!-- Einde bewakingsrooster -->\n");
} # bew_rooster

function bew_postindeling($seizoen, $week=-1) {

	$lijst = db_bewaking("rooster", $seizoen, 0, $week);
		
	$vw = -1;
	$vp = "";
	
	echo("<div id='postindeling'>\n");
		
	foreach ($lijst as $row) {
		if (($row->ST_LEN_BP == 7 and $vw != $row->Weeknr) or $row->ST_LEN_BP == 1 and $vw != strftime("%A %e %B %Y", strtotime($row->BEGIN_PER))) {
			echo("<div class='clear'></div>\n");
			if ($row->ST_LEN_BP == 7) {
				printf("<h2>Week %s-%d</h2>\n", $row->Kode, $row->Weeknr);
				$vw = $row->Weeknr;
			} else {
				printf("<h2>%s</h2>\n", strftime("%A %e %B %Y", strtotime($row->BEGIN_PER)));
				$vw = strftime("%A %e %B %Y", strtotime($row->BEGIN_PER));
			}
		}
		if ($vp !== $row->Post) {
			echo("<div class='clear'></div>\n");
			if (strlen($row->Post) > 0) {
				printf("<h3>Post %s</h3>\n", $row->Post);
			}
			$vp = $row->Post;
		}
		
		$fn = fotolid($row->RecordID, 1);
		if (strlen($fn) > 0) {
			$fn = sprintf("<img src='%s' alt='%s'>\n", $fn, htmlentities($row->NaamBewaker));
		}
		if (strlen($row->OmsFunctie) > 0) {
			$of = $row->OmsFunctie;
		} else {
			$of = "&nbsp;";
		}
		$lft = date("Y", strtotime($row->BEGIN_PER)) - date("Y", strtotime($row->GEBDATUM));
		if (date("m-d", strtotime($row->BEGIN_PER)) < date("m-d", strtotime($row->GEBDATUM))) {
			$lft = $lft - 1;
		}
		printf("<div class='bewaker'>%s<p class='bewfunctie'>%s</p>\n<p class='bewnaam'>%s</p>\n<div class='bewleeftijd'>%d jaar</div>\n<div class='bewervaring'>%s ervaring</div>\n</div>\n",
				 $fn, $of, fnDispBW($row, "Naam"), $lft, fnDispBW($row, "Erv"));
		$sel_leden[] = $row->RecordID;
	}
	echo("</div> <!-- Einde postindeling -->\n");
} # bew_postindeling

function inschrijvenbewaking($lidid) {
	
	fnDispMenu(2);
	
	if (count(db_bewaking_blok("openblokken")) == 0) {
		echo("<p class='mededeling'>Er zijn op dit moment geen bewakingen waarvoor ingeschreven kan worden.</p>\n");
	} else {
		if ($_SERVER['REQUEST_METHOD'] == "POST") {
			$geldig = false;
			$bevins = "";
			if (isset($_POST['insnr']) and $_POST['insnr'] > 0) {
				$insnr = $_POST['insnr'];
			} else {
				$insnr = db_scalar(sprintf("SELECT MAX(BI.Nummer) FROM %sBewaking_Inschrijving AS BI;", TABLE_PREFIX)) + 3;
			}
			foreach (db_bewaking_blok("openblokken") as $row) {
				$k1 = sprintf("k1_%d", $row->RecordID);
				$k2 = sprintf("k2_%d", $row->RecordID);
				$opm = sprintf("Opm_%d", $row->RecordID);
				$kz = 0;
				if (isset($_POST[$k1]) and isset($_POST[$k2])) {
					$kz = 3;
					$geldig = true;
				} elseif (isset($_POST[$k1])) {
					$kz = 1;
					$geldig = true;
				} elseif (isset($_POST[$k2])) {
					$kz = 2;
				}
				$ins = db_insbew("inschrijving", $lidid, 0, $row->RecordID);
				if (isset($ins->RecordID)) {
					if ($kz == 0 and strlen($_POST[$opm]) == 0) {
						db_insbew("delete", $lidid, $insnr, $row->RecordID);
					} elseif ($ins->Keuze != $kz or $ins->Opmerking != $_POST[$opm] or $ins->Nummer != $insnr) {
						db_insbew("update", $lidid, $insnr, $row->RecordID, $kz, $_POST[$opm]);
					}
				} elseif ($kz > 0 or strlen($_POST[$opm]) > 0) {
					db_insbew("add", $lidid, $insnr, $row->RecordID, $kz, $_POST[$opm]);
				}
			}
			$opmcv = db_memo("curval", $lidid, "I");
			if (strlen($_POST['opmalg']) > 0 and strlen($opmcv) == 0) {
				db_memo("add", $lidid, "I", $_POST['opmalg']);
			} elseif (strlen($_POST['opmalg']) == 0 and strlen($opmcv) > 0) {
				db_memo("delete", $lidid, "I");
			} elseif ($_POST['opmalg'] != $opmcv) {
				db_memo("update", $lidid, "I", $_POST['opmalg']);
			}
			if ($geldig) {
				if (isset($_POST['Definitief'])) {
					db_insbew("defmaken", $lidid, $insnr);
					if (db_param("mailing_bewakinginschrijving") > 0) {
						BevestInschrBewaking($lidid, $insnr);
					}
				}
			}
		}

		echo("<div id='inschrijvingbewaking'>\n");
		printf("<form method='post' action='%s?tp=%s&amp;lidid=%d' name='ins_bewaking'>\n", $_SERVER['PHP_SELF'], urlencode($_GET['tp']), $lidid);
		$vs = -1;
		$insnr = 0;
		echo("<table>\n");
			
		$geldig = false;
		$kandef = false;
		foreach (db_bewaking_blok("openblokken") as $row) {
			$ins = db_insbew("inschrijving", $lidid, 0, $row->RecordID);
			if ($vs != $row->SeizoenID) {
				printf("<tr><th colspan=6>%s %s</th></tr>\n", $row->KodeSeizoen, $row->Locatie);
				if ($row->KeuzesBijInschrijving == 1) {
					$kk = "1ste keuze</th><th>2de keuze";
				} else {
					$kk = "Inschrijven";
				}
				printf("<tr><th>Week</th><th>Begindatum</th><th>Einddatum</th><th>%s</th><th>Opmerking</th></tr>\n", $kk);
				$vs = $row->SeizoenID;
			}
			printf("<tr><td>%s</td><td>%s</td><td>%s</td>\n", $row->Kode, strftime("%e %B %Y", strtotime($row->Begindatum)), strftime("%e %B %Y", strtotime($row->Einddatum)));
			$c = "";
			$v = "&nbsp;";
			if (isset($ins->Keuze) and ($ins->Keuze == 1 or $ins->Keuze == 3)) {
				$c = "checked";
				$v = "Ja";
				$geldig = true;
			}
			if (isset($ins->Nummer) and $insnr == 0) {
				$insnr = $ins->Nummer;
			}
			printf("<td class='chk'><input type='checkbox' onClick='this.form.submit();' name='k1_%s' value=1 %s></td>\n", $row->RecordID, $c);
			if ($row->KeuzesBijInschrijving == 1) {
				$c = "";
				$v = "&nbsp;";
				if (isset($ins->Keuze) and $ins->Keuze >= 2) {
					$c = "checked";
					$v = "Ja";
				}
				printf("<td class='chk'><input type='checkbox' onClick='this.form.submit();' name='k2_%s' value=1 %s></td>\n", $row->RecordID, $c);
			}
			$v = "";
			if (isset($ins->Opmerking)) {
				$v = $ins->Opmerking;
			}
			printf("<td><input type='text' onBlur='this.form.submit();' name='Opm_%d' size=60 maxlength=50 value=\"%s\"></td>\n", $row->RecordID, $v);
			echo("</tr>\n");
			if (isset($ins->RecordID) and is_null($ins->Definitief)) {
				$kandef = true;
			}
		}
		printf("<tr><td colspan=6>Opmerking(en) bij de inschrijving:<br><textarea onBlur='this.form.submit();' cols=90 rows=8 name='opmalg'>%s</textarea></td></tr>\n", db_memo("curval", $lidid, "I"));
		echo("<tr><th colspan=6>");
		printf("<p>%s</p>", db_param("zs_voorwaardeninschrijving"));
		if ($geldig and $kandef) {
			echo("&nbsp;<input type='submit' value='Definitief maken' name='Definitief'>");
			echo("<p>Zolang een inschrijving niet definitief gemaakt is, geldt deze niet als inschrijving.</p>\n");
		}
		echo("</th></tr>\n");
		echo("</table>\n");
		printf("<input type='hidden' name='insnr' value=%d>\n", $insnr);
		echo("</form>\n");
		echo("</div>  <!-- Einde inschrijvingbewaking -->\n");
	}
} # inschrijvenbewaking

function fnDispBW($row, $output) {
	global $ldl;

	if ($output == "Erv") {
		if ($row->ErvaringDagen == 0) {
			$rv = "geen";
		} elseif ($row->ErvaringDagen == 1) {
			$rv = "1 dag";
		} elseif ($row->ErvaringDagen >= 4 and $row->ErvaringDagen <= 10 and $row->TOONERV == "W") {
			$rv = "1 week";
		} elseif ($row->ErvaringDagen > 10 and $row->TOONERV == "W") {
			$rv = round($row->ErvaringDagen/7, 0) . " weken";
		} else {
			$rv = $row->ErvaringDagen . " dagen";
		}
	} elseif ($output == "Rollen") {
		$lo = db_lidond("bewaking", $row->Lid, $row->BEGIN_PER);
		$rv = "&nbsp;";
		foreach ($lo as $row_lo) {
			if ($rv == "&nbsp;") {
				$rv = $row_lo->Kode;
			} else {
				$rv .= ", " . $row_lo->Kode;
			}
		}
	} else {
		if (strlen($ldl) > 1) {
			$rv = sprintf($ldl, $row->Nummer, htmlentities($row->NaamBewaker));
		} else {
			$rv = htmlentities($row->NaamBewaker);
		}
		if ($row->ST_LEN_BP > 1) {
			if ($row->Dagen == 1) {
				$rv .= " (" . strftime("%a", strtotime($row->BEGIN_PER)) . ")";
			} elseif ($row->Dagen == 2) {
				$rv .= " (" . strftime("%a", strtotime($row->BEGIN_PER)) . " en " . strftime("%a", strtotime($row->EINDE_PER)) . ")";
			} elseif ($row->Dagen < 7) {
				$rv .= " (" . strftime("%a", strtotime($row->BEGIN_PER)) . " t/m " . strftime("%a", strtotime($row->EINDE_PER)) . ")";
			}
		}
	}
	
	return $rv;
} # fnDispBW

function BevestInschrBewaking($lidid, $insnr) {

	if (db_param("mailing_bewakinginschrijving") > 0) {
		$mailing = new Mailing(db_param("mailing_bewakinginschrijving"));
		$mailing->xtrachar = "BW_I";
		$mailing->xtranum = $insnr;
		$mailing->resultaatversturen = 0;
		if ($mailing->send($lidid) > 0) {
			$mess = sprintf("Inschrijving %d is aan %s verzonden.", $insnr, (new cls_Lid())->Naam($lidid));
		} else {
			$mess = sprintf("Fout bij het versturen van de e-mail. Probeer het later nogmaals of neem contact op met de webmaster.");
		}
		$mailing = null;
	} else {
		$mess = "Er is geen mailing voor het bevestigingen van de inschrijving gedefinieerd.";
	}
	db_logboek("add", $mess, 11, $lidid, 2);
	echo("<script>location.href='/index.php';</script>\n");
}

function DetailsInschrijvingBewaking($rows) {
	$ret = "<div id='lijst'>\n";
	
	if (isset($rows) and count($rows) > 0) {
		$ret .= "<table>\n";
		$ret .= "<tr><th colspan=8 class='tabelkop'>Details inschrijving bewaking</th></tr>\n";
		$ret .= "<tr><th colspan=2>Bewaker</th><th>Week</th><th>Periode</th><th>Keuze 1</th><th>Keuze 2</th><th>Opmerking (week)</th><th>Opmerking (Inschrijving)</th></tr>\n";
		$voriglid = -1;
		$hv = "";
		foreach($rows as $row) {
			if ($voriglid != $row->ndLidID) {
				if (strlen($hv) > 0) {
					$ret .= sprintf("<tr>%s</tr>\n", str_replace("[ROWSPAN]", $aantalreg, $hv));
				}
				$fn = fotolid($row->ndLidID);
				$voriglid = $row->ndLidID;
				$aantalreg = 0;
				$hv = "";
				$opmins = db_memo("curval", $row->ndLidID, "I");
			} elseif (strlen($hv) > 0) {
				$hv .= "</tr>\n<tr>";
			}
			$aantalreg++;
			if ($aantalreg == 1) {
				if (strlen($fn) > 0) {
					$hv .= sprintf("<td rowspan=[ROWSPAN] class='fotoinschrijving'><img src='%s' alt='Pasfoto %s'></td>\n", $fn, htmlentities($row->Bewaker));
				} else {
					$hv .= "<td rowspan=[ROWSPAN]>&nbsp;</td>\n";
				}
				$hv .= sprintf("<td rowspan=[ROWSPAN]>%s</td>", htmlentities($row->Bewaker));
			}
			$hv .= sprintf("<td>%s</td>", $row->Week);
			if (substr($row->dteBegin, 0, 7) == substr($row->dteEinde, 0, 7)) {
				$hv .= sprintf("<td>%s t/m %s</td>", strftime('%e', strtotime($row->dteBegin)), strftime('%e %b %Y', strtotime($row->dteEinde)));
			} else {
				$hv .= sprintf("<td>%s t/m %s</td>", strftime('%e %b', strtotime($row->dteBegin)), strftime('%e %b %Y', strtotime($row->dteEinde)));
			}
			$k = "";
			$cnd = "";
			if ($row->Keuze == 1 or $row->Keuze == 3) {
				if ($row->Definitief < $row->Ingevoerd) {
					$k = "ND";
					$cnd = " class='nietdefinitief'";
				} else {
					$k = "Ja";
				}
			}
			$hv .= sprintf("<td %s>%s</td>", $cnd, $k);
			
			$k = "";
			if ($row->Keuze == 2 or $row->Keuze == 3) {
				if ($row->Definitief < $row->Ingevoerd) {
					$k = "ND";
					$cnd = " class='nietdefinitief'";
				} else {
					$k = "Ja";
				}
			}
			$hv .= sprintf("<td %s>%s</td>", $cnd, $k);

			$hv .= sprintf("<td class='opmerkingweek'>%s</td>", $row->Opmerking);
			if ($aantalreg == 1) {
				$hv .= sprintf("<td rowspan=[ROWSPAN] class='opmerkinginschrijving'>%s</td>\n", $opmins);
			}
		}
		if (strlen($hv) > 0) {
			$ret .= sprintf("<tr>%s</tr>\n", str_replace("[ROWSPAN]", $aantalreg, $hv));
		}
		$ret .= "</table>\n";
	} else {
		$ret .= "<p class='mededeling'>Er zijn geen records beschikbaar.</p>\n";
	}
	$ret .= "</div>  <!-- Einde lijst -->\n";
	return $ret;
} # DetailsInschrijvingBewaking

function exportinschrijvingen() {

	if (isset($_GET['op']) and $_GET['op'] == "afmelden") {
		printf("<p class='mededeling'>%s</p>\n", db_insbew("afmelden"));
	}
	
	$rows = db_insbew("export");
	if (count($rows) > 0) {
		printf("<form name='formdownload' method='post' action='%s?%s&amp;op=exportins'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
		echo("<table>\n");
		echo("<tr><th>Naam lid</th><th>SQL</th></tr>\n");
		foreach($rows as $row) {
			printf("<tr><td>%s</td><td>%s</td></tr>\n", htmlentities($row->NaamLid), SQLexport($row));
		}
		echo("</table>\n");
		echo("<p class='mededeling'><input type='submit' value='Download SQL'>\n");
		printf("&nbsp;<input type='button' value='Afmelden' OnClick=\"location.href='%s?%s&amp;op=afmelden'\"></p>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
		echo("</form>\n");
	} else {
		echo("<p class='mededeling'>Er zijn geen wijzigingen die nog verwerkt moeten worden.</p>\n");
	}
} # exportinschrijvingen

function blokkenmuteren($seizoen) {
	global $fdsql;

	if (isset($_GET['op']) and $_GET['op'] == "new") {
		db_bewaking_blok("add", $seizoen);
	} elseif (isset($_GET['op']) and $_GET['op'] == "delete") {
		db_bewaking_blok("delete", $seizoen, $_GET['rid']);
	}
	if (isset($_POST['Bijwerken'])) {
		foreach (db_bewaking_blok("overzicht", $seizoen) as $row) {
			$kd = sprintf("Kode_%d", $row->RecordID);
			$f = "Kode";
			if ($_POST[$kd] != $row->$f) {
				db_bewaking_blok("edit", 0, $row->RecordID, $f, $_POST[$kd]);
			}
			
			$bd = sprintf("Begin_%d", $row->RecordID);
			$bd = change_month_to_uk($_POST[$bd]);
			if (strlen($bd) > 4 and $bd != "0000-00-00" and strtotime($bd) !== false) {
				$bd = strftime($fdsql, strtotime($bd));
			} else {
				$bd = "0000-00-00";
			}
			$f = "Begindatum";
			if ($bd != $row->$f) {
				db_bewaking_blok("edit", 0, $row->RecordID, $f, $bd);
			}
	
			$ed = sprintf("Einde_%d", $row->RecordID);
			$ed = change_month_to_uk($_POST[$ed]);
			if (strlen($ed) > 4 and $ed != "0000-00-00" and strtotime($ed) !== false) {
				$ed = strftime($fdsql, strtotime($ed));
			} else {
				$ed = "0000-00-00";
			}
			if ($ed < $bd) {
				$ed = $bd;
			}
			$f = "Einddatum";
			if ($ed != $row->$f) {
				db_bewaking_blok("edit", 0, $row->RecordID, $f, $ed);
			}
	
			$io = sprintf("Open_%d", $row->RecordID);
			if (isset($_POST[$io]) and $_POST[$io] == "1") {
				$io = 1;
			} else {
				$io = 0;
			}	
			$f = "InschrijvingOpen";
			if ($io != $row->$f) {
				db_bewaking_blok("edit", 0, $row->RecordID, $f, $io);
			}
		}
	}

	echo("<div id='muterenbewakingsblokken'>\n");
	printf("<form name='muterenbewakingsblokken' method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	$aant=0;
	echo("<table>\n");
	echo("<tr><th>ID</th><th>Kode</th><th>Begin</th><th>Einde</th><th>Inschrijving open?</th><th></th></tr>\n");
	foreach (db_bewaking_blok("overzicht", $seizoen) as $row) {
		echo("<tr>\n");
		printf("<td>%d</td>\n", $row->RecordID);
		printf("<td><input type='text' name='Kode_%d' value='%s'size=2 maxlength=2></td>", $row->RecordID, $row->Kode);
		printf("<td><input type='text' name='Begin_%d' value='%s'>%s</td>\n", $row->RecordID, $row->Begindatum, strftime("%a", strtotime($row->Begindatum)));
		printf("<td><input type='text' name='Einde_%d' value='%s'>%s</td>\n", $row->RecordID, $row->Einddatum, strftime("%a", strtotime($row->Einddatum)));
		printf("<td><input type='checkbox' name='Open_%d'%s value='1'></td>\n", $row->RecordID, checked($row->InschrijvingOpen));
		if (db_bewaking_blok("ingebruik", 0, $row->RecordID)) {
			echo("<td></td>\n");
		} else {
			printf("<td><a href='%s?tp=%s&amp;op=delete&amp;rid=%d'><img src='images/trash.png'></a></td>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
		}
		echo("</tr>\n");
		$aant++;
	}
	printf("<tr><td><a href='%s?%s&amp;op=new'><img src='images/star.png' alt='Nieuw'></a></td><td colspan=5>Nieuw blok</td></tr>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	if ($aant > 0) {
		echo("<tr><th colspan=6><input type='submit' name='Bijwerken' value='Bijwerken'></th></tr>\n");
	}
	echo("</table>\n");
	echo("</form>\n");
	echo("</div> <!-- Einde muterenbewakingsblokken -->\n");
	
} #blokkenmuteren

function SQLexport($row) {
	return sprintf("INSERT INTO Bewaking (Lid, SeizoenID, BEGIN_PER, EINDE_PER) VALUES (%d, %d, #%s#, #%s#);",
			$row->Lid, $row->SeizoenID, strftime("%m/%d/%Y", strtotime($row->Begindatum)), strftime("%m/%d/%Y", strtotime($row->Einddatum)));
}
?>
