<?php
function fnBewaking($seizoen=0) {
	global $ldl, $currenttab2, $table_prefix;
	
	fnDispMenu(2);

	if (isset($_POST['seizoen']) and strlen($_POST['seizoen']) > 0) {
		$seizoen = $_POST['seizoen'];
		$_SESSION['actseizoen'] = $seizoen;
	} elseif (isset($_SESSION['actseizoen']) and strlen($_SESSION['actseizoen']) > 0) {
		$seizoen = $_SESSION['actseizoen'];
	} else {
		$seizoen = db_scalar(sprintf("SELECT MAX(SeizoenID) FROM %sBewaking;", $table_prefix));
	}
	if (isset($_POST['week']) and strlen($_POST['week']) > 0) {
		$week = $_POST['week'];
		$_SESSION['actweek'] = $week;
	} elseif (isset($_SESSION['actweek']) and strlen($_SESSION['actweek']) > 0) {
		$week = $_SESSION['actweek'];
	} else {
		$week = "*";
	}
	
	$st_len_bp = 1;
	
	echo("<div id='filter'>\n");
	printf("<form method='post' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	echo("<table>\n<tr>\n");
	echo("<td class='label'>Bewakingseizoen:</td>");
	echo("<td><select name='seizoen' onchange='form.submit();'>\n");
	if ($currenttab2 == "Overzicht inschrijvingen") {
		echo("<option value=-1>Alle</option>\n");
	}
	$bs_res = db_bewaking_seizoenen();
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
		echo("<td class='label'>Bewakingsweek:</td>");
		echo("<td><select name='week' onchange='form.submit();'>\n");
		print("<option value='-1'>Alle</option>\n");
		if ($currenttab2 == "Overzicht inschrijvingen") {
			$rows = db_insbew("blokken", 0, 0, 0, 0, "", $seizoen);
		} else {
			$rows = db_bewaking_aantallen($seizoen);
		}
		foreach($rows as $row) {
			if ($week == $row->Weeknr or strlen($week) == 0) {
				$week = $row->Weeknr;
				$sel = " selected";
			} else {
				$sel = "";
			}
			printf("<option value='%d'%s>%d</option>\n", $row->Weeknr, $sel, $row->Weeknr);
		}
		echo("</select>\n</td>\n");
	}
	echo("</tr>\n</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde filter -->\n");
	
	if ($currenttab2 == "Bewakingsrooster") {
		bew_rooster($seizoen, $week);
	} elseif ($currenttab2 == "Postindeling") {
		bew_postindeling($seizoen, $week=-1);
	} elseif ($currenttab2 == "Aantallen" and toegang($_GET['tp'])) {
		$lijst = db_bewaking_aantallen($seizoen);
		echo(fnDiplayTable($lijst));

	} elseif ($currenttab2 == "Overzicht inschrijvingen" and toegang($_GET['tp'])) {
		$lijst = db_insbew("matrixaantallen", 0, 0, 0, 0, "", $seizoen);
		echo(fnDiplayTable($lijst, "", "Totalen"));
		$lijst = db_insbew("overzicht", 0, 0, 0, 0, "", $seizoen, $week);
		echo(fnDiplayTable($lijst, "", "Details"));
	}
}

function bew_rooster($seizoen, $week=-1) {

	$rooster = db_bewakingsrooster($seizoen, $week);
	$wk = "";
	$aantalbewdag = 0;
	$aantalond = db_Onderdelen("bw", 1);
	if ($aantalond > 1) {
		$bastotreg = "<tr>\n<td colspan='3'><b>Totaal: %d bewakers</b></td>\n<td class='number'><b>%s</b></td>\n<td>&nbsp;</td>\n</tr>\n";
	} else {
		$bastotreg = "<tr>\n<td colspan='3'><b>Totaal: %d bewakers</b></td>\n<td class='number'><b>%s</b></td>\n</tr>\n";
	}
	echo("<div id='bewakingsrooster'>\n");
	echo("<table>\n");
	foreach($rooster->fetchAll() as $row) {
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
	}
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
}

function bew_postindeling($seizoen, $week=-1) {
	$lijst = db_bewakingsrooster($seizoen, $week);
		
	$vw = -1;
	$vp = "";
	
	echo("<div id='postindeling'>\n");
	echo("<table>\n");
		
	foreach ($lijst as $row) {
		if (($row->ST_LEN_BP == 7 and $vw != $row->Weeknr) or $row->ST_LEN_BP == 1 and $vw != strftime("%A %e %B %Y", strtotime($row->BEGIN_PER))) {
			echo("<div class='clear'></div>\n");
			if ($row->ST_LEN_BP == 7) {
				printf("<h2>Week %s-%d</h2>\n", $row->Seizoen, $row->Weeknr);
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
		
		$fn = fotolid($row->Nummer);
		if (strlen($fn) > 0) {
			$fn = sprintf("<img src='%s' alt='%s'>\n", $fn, $row->NaamBewaker);
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
	echo("</table>\n");
	echo("</div> <!-- Einde postindeling -->\n");
}

function inschrijvenbewaking($lidid) {
	global $naamvereniging, $naamwebsite, $urlwebsite, $voorwaardeninschrijving, $selfservicediplomas, $emailbevestiginginschrijving;

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
			if ($kz > 0) {
				$bevins .= sprintf("<li>%s - %s t/m %s", $row->Kode, strftime("%e %B %Y", strtotime($row->Begindatum)), strftime("%e %B %Y", strtotime($row->Einddatum)));
				if ($kz == 2) {
					$bevins .= " (2de keuze)";
				}
				if (strlen($_POST[$opm]) > 1) {
					$bevins .= sprintf(" (%s)", $_POST[$opm]);
				}
				$bevins .= "</li>\n";
			}
		}
		$opmcv = db_memo($lidid, "I", "curval");
		if (strlen($_POST['opmalg']) > 0 and strlen($opmcv) == 0) {
			db_memo($lidid, "I", "insert", $_POST['opmalg']);
		} elseif (strlen($_POST['opmalg']) == 0 and strlen($opmcv) > 0) {
			db_memo($lidid, "I", "delete");
		} elseif ($_POST['opmalg'] != $opmcv) {
			db_memo($lidid, "I", "update", $_POST['opmalg']);
		}
		if ($geldig) {
			$bevest_template = 'templates/bevestiging_inschrijving.html';
			if (isset($_POST['Definitief']) and file_exists($bevest_template)) {
				db_insbew("defmaken", $lidid, $insnr);
				$content = file_get_contents($bevest_template);
				$brief_template = 'templates/briefpapier.html';
				if (file_exists($brief_template)) {
					$briefpapier = file_get_contents($brief_template);
					if ($content == false) {
						$briefpapier = "[%MESSAGE%]";
					}
				} else {
					$briefpapier = "[%MESSAGE%]";
				}
				$subj = "Bevestiging inschrijving bewaking";
				$content = str_ireplace("[%MESSAGE%]", $content, $briefpapier);	
				$content = str_ireplace("[%FROM%]", $naamvereniging, $content);
				$content = str_ireplace("[%TO%]", $_SESSION['naamingelogde'], $content);
				$content = str_ireplace("[%SUBJECT%]", $subj, $content);
				$content = str_replace("[%NAAMVERENIGING%]", $naamvereniging, $content);
				$content = str_replace("[%NAAMWEBSITE%]", $naamwebsite, $content);
				$content = str_replace("[%URLWEBSITE%]", $urlwebsite, $content);
				$content = str_ireplace("[%NAAMLID%]", $_SESSION['naamingelogde'], $content);
				$content = str_ireplace("[%ROEPNAAM%]", $_SESSION['roepnaamingelogde'], $content);
				$content = str_ireplace("[%BEWINSCHRIJVING%]", $bevins, $content);
				$content = str_ireplace("[%BEWOPMERKING%]", db_memo($lidid, "I", "curval"), $content);
				$content = str_ireplace("[%DIEET%]", db_memo($lidid, "D", "curval"), $content);
				$content = str_ireplace("[%GEZONDHEID%]", db_memo($lidid, "G", "curval"), $content);
				
				$f = sprintf("D.Kode IN %s", $selfservicediplomas);
				$d = "";
				foreach (db_liddipl("lidgegevens", $lidid, 0, 0, "", "", "", $f) as $ld) {
					$d .= sprintf("<li>%s</li>\n", $ld->Diploma);
				}
				$content = str_ireplace("[%BEWDIPLOMAS%]", $d, $content);
				
				$mail = new RBMmailer();
				try {
					$mail->From = $emailbevestiginginschrijving;
					$mail->AddAddress($_SESSION['emailingelogde']);
					$mail->Subject = $subj;
					$mail->MsgHTML($content);
					if ($mail->Send()) {
						$oms = sprintf("Bevestiging inschrijving bewaking verzonden");
						db_add_activiteit($oms, 6);
						printf("<script>alert('Een bevestiging van deze inschrijving is naar %s verzonden.');</script>\n", $_SESSION['emailingelogde']);	echo("<script>location.href='/index.php';</script>\n");	
					} else {
						printf("<p>Error while sending message: %s</p>\n", $mail->ErrorInfo);
					}
				} catch (phpmailerException $e) {
					echo $e->errorMessage(); // Error messages from PHPMailer
				} catch (Exception $e) {
					echo $e->getMessage();
				}					
			}
		} else {
			echo("<p class='mededeling'>Deze inschrijving is niet geldig, omdat er geen enkele eerste keuze is aangegeven.</p>\n");
		}
	}

	echo("<div id='inschrijvingbewaking'>\n");
	printf("<form method='post' action='/index.php?%s' name='ins_bewaking'>\n", $_SERVER['QUERY_STRING']);
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
		if (isset($ins->Keuze) and $ins->Keuze != 2) {
			$c = "checked";
			$geldig = true;
		}
		if (isset($ins->Nummer) and $insnr == 0) {
			$insnr = $ins->Nummer;
		}
		printf("<td class='chk'><input type='checkbox' name='k1_%s' value=1 %s></input></td>\n", $row->RecordID, $c);
		if ($row->KeuzesBijInschrijving == 1) {
			$c = "";
			if (isset($ins->Keuze) and $ins->Keuze > 1) {
				$c = "checked";
			}
			printf("<td class='chk'><input type='checkbox' name='k2_%s' value=1 %s></input></td>\n", $row->RecordID, $c);
		}
		$v = "";
		if (isset($ins->Opmerking)) {
			$v = $ins->Opmerking;
		}
		printf("<td><input type='text' name='Opm_%d' size=60 maxlength=50 value=\"%s\"></td>\n", $row->RecordID, $v);
		echo("</tr>\n");
		if (isset($ins->Definitief) and $ins->Definitief < "2010-01-01") {
			$kandef = true;
		}
	}
	printf("<tr><td colspan=7>Opmerkingen:<br><textarea cols=90 rows=8 name='opmalg'>%s</textarea></td></tr>", db_memo($lidid, "I", "curval"));
	echo("<tr><th colspan=7>");
	printf("<p>%s</p>", $voorwaardeninschrijving);
	echo("<input type='submit' value='Wijzigen' name='Wijzigen'>");
	if ($geldig and $kandef) {
		echo("&nbsp;<input type='submit' value='Definitief maken' name='Definitief'>");
	}
	echo("</th><tr>\n");
	echo("</table>\n");
	printf("<input type='hidden' name='insnr' value=%d>\n", $insnr);
	echo("</form>\n");
	echo("</div>  <!-- Einde inschrijvingbewaking -->\n");
}

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
		$lo = db_bewaking_onderdelen_lid($row->Lid, $row->BEGIN_PER);
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
}

?>