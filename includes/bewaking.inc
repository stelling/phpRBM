<?php

function fnBewaking() {
	global $ldl, $currenttab2, $table_prefix;
	
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
	} elseif ($currenttab2 == "Overzicht inschrijvingen") {
		$seizoen = db_scalar(sprintf("SELECT MAX(SeizoenID) FROM %sBewaking_Blok WHERE InschrijvingOpen=1;", $table_prefix));
	} else {
		$seizoen = db_scalar(sprintf("SELECT MAX(SeizoenID) FROM %sBewaking;", $table_prefix));
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
	
	echo("<div id='filter'>\n");
	printf("<form method='post' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	echo("<table>\n<tr>\n");
	
	
	if ($currenttab2 == "Downloaden inschrijvingen") {
		// Geen filter tonen
	} elseif ($currenttab2 == "Diploma's muteren" or $currenttab2 == "Nieuwe pasfoto") {
		$filterlid = sprintf("L.Nummer IN (SELECT Lid FROM %1\$sBewaking AS BW INNER JOIN %1\$sBewseiz AS BS ON BW.SeizoenID=BS.RecordID WHERE BS.Afgesloten=0)", $table_prefix);
		$optionslid = "<option value=-1>...</option>\n";
		foreach (db_lid("lijst", $filterlid) as $row) {
			if ($row->Nummer == $lidid) {
				$s = "selected";
			} else {
				$s = "";
			}
			$optionslid .= sprintf("<option value=%1\$d %3\$s>%2\$s</option>\n", $row->Nummer, htmlentities($row->Zoeknaam), $s);
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
		
		if ($currenttab2 != "Aantallen") {
			echo("<td class='label'>Bewakingsweek:</td>\n");
			echo("<td><select name='week' id='week' onchange='this.form.submit();'>\n");
			print("<option value=-1>Alle</option>\n");
			if ($currenttab2 == "Overzicht inschrijvingen") {
				$rows = db_insbew("blokken", 0, 0, 0, 0, "", $seizoen);
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
	echo("</tr>\n</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde filter -->\n");
	
	if ($currenttab2 === "Bewakingsrooster" and toegang($_GET['tp'])) {
		bew_rooster($seizoen, $week);
	} elseif ($currenttab2 == "Postindeling") {
		bew_postindeling($seizoen, $week);
	} elseif ($currenttab2 == "Aantallen" and toegang($_GET['tp'])) {
		$lijst = db_bewaking("aantallen", $seizoen);
		echo(fnDisplayTable($lijst, "", "", 0, "", "", "bewakingsaantallen"));
	} elseif ($currenttab2 == "Nieuwe pasfoto" and $lidid > 0) {
		$filterlid = sprintf("L.Nummer IN (SELECT Lid FROM %1\$sBewaking AS BW INNER JOIN %1\$sBewseiz AS BS ON BW.SeizoenID=BS.RecordID WHERE BS.Afgesloten=0)", $table_prefix);
		nieuwepasfoto($lidid, $filterlid);
	} elseif ($currenttab2 == "Diploma's muteren" and toegang($_GET['tp']) and $lidid > 0) {
		diplomaslidmuteren($lidid, $dptype, 0);
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
	$aantalond = db_onderdelen("bw", 1);
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
		$sel_leden[] = $row->Nummer;
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
		
		$fn = fotolid($row->Nummer, 1);
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
		$sel_leden[] = $row->Nummer;
	}
	echo("</div> <!-- Einde postindeling -->\n");
} # bew_postindeling

function inschrijvenbewaking($lidid) {
	
	fnDispMenu(2);
	
	if (count(db_insbew("openblokken")) == 0) {
		echo("<p class='mededeling'>Er zijn op dit moment geen bewakingen waarvoor ingeschreven kan worden.</p>\n");
	} else {
		if ($_SERVER['REQUEST_METHOD'] == "POST") {
			$geldig = false;
			$bevins = "";
			if (isset($_POST['insnr']) and $_POST['insnr'] > 0) {
				$insnr = $_POST['insnr'];
			} else {
				$insnr = db_scalar(sprintf("SELECT MAX(Nummer) FROM %sBewaking_Inschrijving;", $table_prefix)) + 3;
			}
			foreach (db_insbew("openblokken") as $row) {
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
					BevestInschrBewaking($lidid, $insnr);
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
		foreach (db_insbew("openblokken") as $row) {
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
	
	$emailbevestiginginschrijving = db_param("zs_emailbevestiginginschrijving");
	if (isValidMailAddress($emailbevestiginginschrijving, 0) == false) {
		$emailbevestiginginschrijving = db_param("emailledenadministratie");
	}
	$voorwaardeninschrijving = db_param("zs_voorwaardeninschrijving");

	$bevest_template = 'templates/inschrijving.html';
	if (file_exists($bevest_template)) {
		$content = file_get_contents($bevest_template);
	} else {
		$content = "Hierbij bevestigen wij de volgende inschrijving voor de bewaking: [%BEWINSCHRIJVING%]";
	}
	$subj = sprintf("Bevestiging inschrijving bewaking %d", $insnr);
	$rows = db_insbew("overzichtlid", 0, $insnr);
	$bevins = "";
	foreach ($rows as $row) {
		$naambewaker = $row->ndNaamBewaker;
		$roepnaambewaker = $row->ndRoepnaamBewaker;
		$adres = $row->ndAdres;
		$postcode = $row->ndPostcode;
		$woonplaats = $row->ndWoonplaats;
		$email = $row->ndEmailBewaker;
		$emailouders = $row->ndEmailOuders;
		$geboren = $row->ndGeboren;
		
		$bevins .= sprintf("<li>%s - %s t/m %s", $row->Seizoen, strftime("%e %B %Y", strtotime($row->dteBegin)), strftime("%e %B %Y", strtotime($row->dteEinde)));
		if ($row->Keuze == 2) {
			$bevins .= " (2de keuze)";
		}
		if (strlen($row->Opmerking) > 1) {
			$bevins .= sprintf(" (%s)", $row->Opmerking);
		}
		$bevins .= "</li>\n";
	}
	$content = str_ireplace("[%NAAMLID%]", $naambewaker, $content);
	$content = str_ireplace("[%ROEPNAAM%]", $roepnaambewaker, $content);
	$content = str_ireplace("[%ADRES%]", $adres, $content);
	$content = str_ireplace("[%POSTCODE%]", $postcode, $content);
	$content = str_ireplace("[%WOONPLAATS%]", $woonplaats, $content);
	$content = str_replace("[%NAAMVERENIGING%]", $_SESSION['naamvereniging'], $content);
	$content = str_replace("[%NAAMWEBSITE%]", $_SESSION['naamwebsite'], $content);
	$content = str_replace("[%URLWEBSITE%]", $_SESSION['urlwebsite'], $content);
	$content = str_replace("[%VOORWAARDEN%]", $voorwaardeninschrijving, $content);
	$content = str_ireplace("[%BEWINSCHRIJVING%]", $bevins, $content);
	$content = str_ireplace("[%BEWOPMERKING%]", db_memo("curval", $lidid, "I"), $content);
	$content = str_ireplace("[%DIEET%]", db_memo("curval", $lidid, "D"), $content);
	$content = str_ireplace("[%GEZONDHEID%]", db_memo("curval", $lidid, "G"), $content);
				
	$d = "";
	foreach (db_liddipl("lidgegevens", $lidid, 0, 0, "", "", "", "D.Zelfservice=1") as $ld) {
		$d .= sprintf("<li>%s</li>\n", $ld->Diploma);
	}
	$content = str_ireplace("[%BEWDIPLOMAS%]", $d, $content);

	$mail = new RBMmailer($lidid);
	try {
		$mail->From = $emailbevestiginginschrijving;
		$mail->Subject = $subj;
		$mail->Body = $content;
		$mail->addstationary();
		if ($mail->Send()) {
			db_add_mailing_hist($lidid, 0, $naambewaker, $email, $content, $mail, 1, "BW_I", $insnr);
			$mess = sprintf("Bevestiging van deze inschrijving is naar %s verzonden.", $email);
		} else {
			db_add_mailing_hist($lidid, 0, $naambewaker, $email, $content, $mail, 0);
			$mess = sprintf("Het versturen van de bevestiging van de inschrijving voor de bewaking aan %s is mislukt. Error: %s.", db_naamlid($lidid), $mail->ErrorInfo);
		}
		db_logboek("add", $mess, 11);
		printf("<script>alert('%s');</script>\n", $mess);
		echo("<script>location.href='/index.php';</script>\n");
	} catch (phpmailerException $e) {
		echo $e->errorMessage(); // Error messages from PHPMailer
	} catch (Exception $e) {
		echo $e->getMessage();
	}
} # BevestInschrBewaking

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

function SQLexport($row) {
	return sprintf("INSERT INTO Bewaking (Lid, SeizoenID, BEGIN_PER, EINDE_PER) VALUES (%d, %d, #%s#, #%s#);",
			$row->Lid, $row->SeizoenID, strftime("%m/%d/%Y", strtotime($row->Begindatum)), strftime("%m/%d/%Y", strtotime($row->Einddatum)));
}
?>
