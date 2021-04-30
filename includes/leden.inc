<?php

if (toegang("Ledenlijst/Overzicht lid", 0, 0)) {
	$ldl = "<a href='index.php?tp=Ledenlijst/Overzicht+lid&amp;lidid=%d'>%s</a>";
} else {
	$ldl = "";
}

if (toegang("Ledenlijst/Wijzigen lid", 0, 0)) {
	$llw = "<a href='index.php?tp=Ledenlijst/Wijzigen+lid/Algemene+gegevens&amp;lidid=%d'><img src='" . BASE64_MUTEER . "' alt='Wijzigen' title='Wijzigen lidgegevens'></a>";
} else {
	$llw = "";
}

if (isset($_POST['AutoBewaren']) and $_POST['AutoBewaren'] == "1") {
	$_SESSION['AutoBewaren'] = 1;
} elseif (!isset($_SESSION['AutoBewaren'])) {
	$_SESSION['AutoBewaren'] = 0;
}

function fnWijzigen($lidid, $actie="") {
	global $currenttab, $currenttab2, $currenttab3, $actionurl;
	
	if ($currenttab == "Zelfservice") {
		$xtra_param = "";
		$lidid = $_SESSION['lidid'];
		$actionurl = sprintf("%s?tp=%s", $_SERVER['PHP_SELF'], $_GET['tp']);
	} else {
		$xtra_param = sprintf("lidid=%d", $lidid);
		$actionurl = sprintf("%s?tp=%s&amp;lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	}
	
	if ($lidid > 0) {
		$naamlid = (new cls_Lid())->Naam($lidid);
		fnDispMenu(2, $xtra_param);
		if ($currenttab != "Zelfservice") {
			fnDispMenu(3, $xtra_param);
		}

		if ($actie == "Algemene gegevens") {
			algemeenlidmuteren($lidid, $_SESSION['AutoBewaren']);
			
		} elseif ($currenttab3 == "Afdelingen") {
			onderdelenlidmuteren($lidid, "A");
			
		} elseif ($currenttab3 == "B, C en F") {
			onderdelenlidmuteren($lidid, "BCF");
			
		} elseif ($currenttab3 == "Groepen") {
			onderdelenlidmuteren($lidid, "G");
			
		} elseif ($currenttab3 == "Onderscheidingen") {
			onderdelenlidmuteren($lidid, "O");
			
		} elseif ($actie == "Diplomas") {
			if ($currenttab == "Zelfservice") {
				diplomaslidmuteren($_SESSION['lidid'], "ZS");
			} else {
				diplomaslidmuteren($lidid, "*");
			}
			
		} elseif ($actie == "Pasfoto") {
			nieuwepasfoto($lidid);
			
		} elseif ($actie == "Toestemmingen") {
			toestemmingenmuteren($lidid);
			
		} elseif ($actie == "Bijzonderheden") {
			$i_Mm = new cls_Memo();
			if ($currenttab == "Zelfservice") {
				$mm = $_SESSION['settings']['zs_muteerbarememos'];
			} else {
				$mm = $_SESSION['settings']['muteerbarememos'];
			}
			if ($_SERVER['REQUEST_METHOD'] == "POST") {
				foreach (explode(",", $mm) as $kodesoort) {
					$namevar = "Memo_" . $kodesoort;
					$curval = $i_Mm->inhoud($lidid, $kodesoort);
					if (strlen($_POST[$namevar]) == 0) {
						$i_Mm->delete($lidid, $kodesoort);
					} elseif (strlen($curval) == 0 and isset(ARRSOORTMEMO[$kodesoort])) {
						$i_Mm->add($lidid, $kodesoort, $_POST[$namevar]);
					} elseif ($curval != $_POST[$namevar]) {
						$i_Mm->update($lidid, $kodesoort, $_POST[$namevar]);
					}
				}
			}
			
			echo("<div id='bijzonderhedenwijzigen'>\n");
			if (strlen($mm) > 0) {
				printf("<form method='post' action='%s'>\n", $actionurl);
				foreach (explode(",", $mm) as $kodesoort) {
					$namevar = "Memo_" . $kodesoort;
					if (isset(ARRSOORTMEMO[$kodesoort])) {
						$curval = $i_Mm->inhoud($lidid, $kodesoort);
						printf("<label>%s</label><textarea cols=85 rows=10 name='%s' OnChange='this.form.submit();'>%s</textarea>\n", ARRSOORTMEMO[$kodesoort], $namevar, $curval);
					}
				}
				echo("</form>\n");
			} else {
				echo("<p class='mededeling'>Er zijn geen bijzonderheden die hier gemuteerd mogen worden.</p>");
			}
			echo("</div>  <!-- Einde bijzonderhedenwijzigen -->\n");
			$i_Mm = null;

		} elseif ($actie == "Opzegging") {
			opzegginglidmaatschap($_SESSION['lidid']);
				
		} elseif ($actie == "Lidmaatschap") {
			lidmaatschapmuteren($lidid);
			
		} elseif ($actie == "Wijzigen wachtwoord") {
		
			if ($_SERVER['REQUEST_METHOD'] == "POST") {
				$mess = "";
				if (strlen($_POST['pw_oud']) < 5) {
					$mess = "Het oude wachtwoord is een verplicht veld.";
				} elseif (strlen($_POST['pw_nieuw']) < 5) {
					$mess = "Het nieuwe wachtwoord is een verplicht veld.";
				} elseif (strlen($_POST['pw_herhaal']) < 5) {
					$mess = "Herhalen van het wachtwoord is verplicht.";
				} else {
					(new cls_Login())->wijzigenwachtwoord($_POST['pw_nieuw'], $_POST['pw_oud'],$_POST['pw_herhaal'], $lidid);
				}
				if (strlen($mess) > 0) {
					printf("<p class=mededeling>%s</p>\n", $mess);
				}
				echo("<p class='mededeling'><a href='/'>Klik hier om verder te gaan.</a></p>\n");
				
			} else {
				echo("<div id='profielwijzigen'>\n");
				printf("<form action='%s' method='post'>\n", $actionurl);
				printf("<fieldset>
				<h3>Wijzigen wachtwoord</h3>
				<label>Login:</label><input type='text' name='ingelogdlogin' value='%s' readonly='readonly'>
				<label>Oude wachtwoord:</label><input type='password' name='pw_oud' maxlength=12>
				<label>Nieuw wachtwoord:</label><input type='password' name='pw_nieuw' maxlength=12>
				<label>Herhaal wachtwoord:</label><input type='password' name='pw_herhaal' maxlength=12>
				<input type='submit' value='Wijzigen'>\n", $_SESSION['username']);
				printf("</fieldset>
				</form>
				%s
				</div>  <!-- Einde profielwijzigen -->", fneisenwachtwoord());
			}	
		} else {
			printf("<p class='mededeling'>fnWijzigen: actie '%s' bestaat niet.</p>\n", $actie);
		}
	} else {
		echo("<p class='mededeling'>Er is geen lid geselecteerd.</p>\n");
	}
} # fnWijzigen

function fnWieiswie($actie, $metfoto=1) {
	global $ldl, $lididtestusers;
	
	$i_lo = new cls_Lidond();
	
	echo("<div id='wieiswie'>\n");
	$vo = "";
	if (strlen(implode(",", $lididtestusers)) > 1) {
		$geentest = sprintf(" AND (LO.Lid NOT IN (%s))", implode(",", $lididtestusers));
	} else {
		$geentest = "";
	}
	if ($actie == "Onderscheidingen") {
		$lijst = $i_lo->lijst(0, "O.TYPE='O'", "O.Naam, LO.Vanaf, LO.Lid");
		
	} elseif ($actie == "Afdelingskader") {
		$lijst = $i_lo->lijst(0, "O.TYPE='A' AND F.Kader=1", "O.Naam, F.Sorteringsvolgorde, F.Omschrijv");
		
	} elseif ($actie == "Overig") {
		$lijst = $i_lo->lijst(0, "((O.TYPE='C' AND O.Kader=False) OR (O.TYPE='F' AND F.Kader=False))", "O.Type, O.Naam");
		
	} else {
		// Kader of Verenigingskader
		$lijst = $i_lo->lijst(0, "(O.Kader=True OR (F.Kader=True AND O.Type='F'))", "O.Type, O.Naam, F.Sorteringsvolgorde, F.Omschrijv");
	}

	if (($metfoto == 1) and ($_SESSION['settings']['toonpasfotoindiennietingelogd'] == 1 or strlen($_SESSION['username']) > 5)) {
		foreach ($lijst as $row) {
			if ($vo != $row->OndNaam) {
				if (strlen($vo) > 0) {
					echo("<div class='clear'></div>\n");
				}
				if (isValidMailAddress($row->CentraalEmail, 0)) {
					printf('<div class="wieiswie-onderdeelnaam">%s </div><div class="wieiswie-onderdeelemail">%s</div>', $row->OndNaam, fnDispEmail($row->CentraalEmail, $row->OndNaam, 1));
				} else {
					printf("<div class='wieiswie-onderdeelnaam'>%s</div>\n", $row->OndNaam);
				}
				echo("<div class='clear'></div>\n");
				$vo = $row->OndNaam; 
			} 
			$ln = htmlentities($row->NaamLid);
			if (isset($row->EmailFunctie) and isValidMailAddress($row->EmailFunctie, 0)) {
				$email = fnDispEmail($row->EmailFunctie, $row->NaamLid, 1);
			} elseif (isValidMailAddress($row->EmailVereniging, 0)) {
				$email = fnDispEmail($row->EmailVereniging, $row->NaamLid, 1);
			} else {
				$email = "";
			}
			if (strlen($row->Functie) > 1) {
				$func = $row->Functie;
				if (strlen($row->Opmerk) > 0) {
					$func .= " " .  $row->Opmerk;
				}
			} else {
				$func = $row->Opmerk;
			}
			$fn = fotolid($row->Lid, 1);
			if ($actie == "Onderscheidingen" and strlen($fn) > 3) {
				printf("<div class='kaartje'><img src='%1\$s' alt='Pasfoto %2\$s'><p class='naamkaderlid'>%2\$s</p>\n<p>vanaf %3\$s</p>\n</div>\n", $fn, $ln, strftime("%B %Y", strtotime($row->Vanaf)));
			} elseif ($actie == "Onderscheidingen") {
				printf("<div class='kaartje'><p class='naamkaderlid'>%s</p>\n<p>vanaf %s</p>\n</div>\n", $ln, strftime("%B %Y", strtotime($row->Vanaf)));
			} elseif (strlen($fn) > 3) {
				printf("<div class='kaartje'><img src='%1\$s' alt='Pasfoto %2\$s'><p class='naamkaderlid'>%2\$s</p>\n<p class='functiekaderlid'>%3\$s</p>\n<p class='mailkaderlid'>%4\$s</p>\n</div>\n", $fn, $ln, $func, $email);
			} else {
				printf("<div class='kaartje'><p class='naamkaderlid'>%s</p>\n<p class='functiekaderlid'>%s</p>\n<p class='mailkaderlid'>%s</p>\n</div>\n", $ln, $func, $email);	
			}
		}
	} else {	
		echo("<table>\n");
		foreach ($lijst as $row) {
			if ($vo != $row->OndNaam) {
				printf("<th colspan=5>%s</th>\n", $row->OndNaam);
				if (isValidMailAddress($row->CentraalEmail, 0)) {
					printf("<tr>\n<td colspan=2><strong>Centraal e-mailadres</strong></td>\n<td>%s</td>\n<td><strong>Vanaf</strong></td></tr>\n", fnDispEmail($row->CentraalEmail, $row->OndNaam, 0, 1));
					echo("<tr><td colspan=5>&nbsp</td></tr>\n");
				}
				$vo = $row->OndNaam;
			}
			if (strlen($ldl) > 1) {
				$ln = sprintf($ldl, $row->RecordID, htmlentities($row->NaamLid));
			} else {
				$ln = htmlentities($row->NaamLid);
			}
			if (isset($row->EmailFunctie) and isValidMailAddress($row->EmailFunctie, 0)) {
				$email = fnDispEmail($row->EmailFunctie, $row->NaamLid, 0);
			} elseif (isValidMailAddress($row->EmailVereniging, 0)) {
				$email = fnDispEmail($row->EmailVereniging, $row->NaamLid, 0);
			} else {
				$email = "";
			}
			if (strlen($row->Functie) > 1) {
				$func = $row->Functie;
				if (strlen($row->Opmerk) > 0) {
					$func .= " " .  $row->Opmerk;
				}
			} else {
				$func = $row->Opmerk;
			}
			printf("<tr>\n<td>%s</td>\n<td>%s</td>\n<td>%s</td>\n<td>%s</td>\n</tr>\n", $ln, $func, $email, strftime("%B %Y", strtotime($row->Vanaf)));
		}
		echo("</table>\n");
	}
	echo("</div>  <!-- Einde kaderoverzicht -->\n");	
} # fnWieiswie

function fnLedenlijst() {
	global $wherelidond, $ldl, $llw, $currenttab2, $currenttab3;
	
	$i_lid = new cls_Lid();
	
	$arrSort[] = "Lidnr;Lidnr";
	$arrSort[] = "Naam lid;L.Achternaam";
	$arrSort[] = "Lid vanaf;LM.LIDDATUM";
	$arrSort[] = "Opgezegd;LM.Opgezegd";
	
	$ord = fnOrderBy($arrSort);
	
	fnDispMenu(2);

	if ($_SERVER['REQUEST_METHOD'] == "POST" and isset($_POST['lbGroepFilter'])) {
		if (isset($_POST['lbGroepFilter'])) {
			$_SESSION['val_groep'] = $_POST['lbGroepFilter'];
		} else {
			$_SESSION['val_groep'] = 0;
		}
	} elseif (!isset($_SESSION['val_groep'])) {
		$_SESSION['val_groep'] = 0;
	}
	
	$filter = "(L.Verwijderd IS NULL)";
	$xtraselect = ", LM.LIDDATUM AS `dteLid vanaf`, LM.Opgezegd AS `dteOpgezegd per`";
	
	if ($currenttab2 == "Klosleden") {
		$filter .= " AND (LM.Lidnr IS NULL)";
		$rows = $i_lid->ledenlijst($filter);
	} elseif ($currenttab2 != "Mutaties" and $currenttab2 != "Instellingen") {
		if ($_SESSION['val_groep'] > 0) {
			if (strlen($filter) > 0) {$filter .= " AND ";}
			$filter .= sprintf("L.RecordID IN (SELECT LO.Lid FROM %sLidond AS LO WHERE ", TABLE_PREFIX);
			$filter .= sprintf("LO.OnderdeelID=%d", $_SESSION['val_groep']);
			if ($currenttab2 == "Leden") {
				$filter .= " AND " . $i_lid->wherelidond . ")";
			} elseif ($currenttab2 == "Voormalig leden") {
				$filter .= " AND (LO.Vanaf <= CURDATE() AND ((NOT LO.Opgezegd IS NULL) OR LO.Opgezegd < CURDATE())))";
			} else {
				$filter .= ")";
			}
		}
		if ($currenttab2 == "Leden") {
			$filter .= " AND LM.Lidnr > 0 AND IFNULL(LM.Opgezegd, CURDATE()) >= CURDATE()";
			$xtraselect = ", LM.LIDDATUM AS `dteLid vanaf`";
		} elseif ($currenttab2 == "Voormalig leden") {
			$filter .= " AND IFNULL(LM.Opgezegd, CURDATE()) < CURDATE()";
		} elseif ($currenttab2 == "Toekomstige leden") {
			$filter .= " AND LM.LIDDATUM > CURDATE()";
		} else {
			$filter .= " AND 1=2";
		}
		$rows = $i_lid->ledenlijst($filter, $xtraselect, $ord);
	}
	
	if ($currenttab2 == "Nieuw (klos)lid") {
		if ($_SERVER['REQUEST_METHOD'] == "POST") {
			if (isset($_POST['addkloslid']) and isset($_POST['achternaam']) and strlen($_POST['achternaam']) > 1) {
				$lidid = $i_lid->add($_POST['achternaam'], $_POST['geslacht'], $_POST['postcode']);
				if (isset($_POST['lidmtoevoegen'])) {
					(new cls_Lidmaatschap())->add($lidid);
				}
			}
			$tp = sprintf("Ledenlijst/Wijzigen+lid/Algemene+gegevens&lidid=%d", $lidid);
			printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $tp);
			
		} else {
			
			echo("<div id='nieuwlid'>\n");
			printf("<form name='frmNieuwLid' action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
			echo("<label>Achternaam</label><input type='text' name='achternaam' maxlength=40>\n");
						
			$opt = "\n";
			foreach (constant("ARRGESLACHT") as $key => $val) {
				$opt .= sprintf("<option value='%s'>%s</option>\n", $key, $val);
			}
			printf("<label>Geslacht</label><select name='geslacht'>%s</select>", $opt);
			
			echo("<label>Postcode</label><input type='text' class='postcode' name='postcode' maxlength=7>\n");
			
			if (toegang("Ledenlijst/Wijzigen lid/Lidmaatschap", 0, 0) == true) {
				echo("<label>Lidmaatschap toevoegen?</label><input type='checkbox' name='lidmtoevoegen'>\n");
			}
			
			echo("<div id='opdrachtknoppen'>\n");
			echo("<input type='submit' name='addkloslid' value='Toevoegen'>\n");
			echo("</div> <!-- Einde opdrachtknoppen -->\n");
			
			echo("</form>\n");
			echo("</div> <!-- Einde nieuwlid -->\n");
		}
		
	} elseif ($currenttab2 == "Instellingen") {
		instellingenledenmuteren();
		
	} elseif ($currenttab2 == "Mutaties") {
		
		$rows = (new cls_Logboek())->lijst(6, 1, 0, "TypeActiviteitSpecifiek < 30", "", 750);
		
		if (count($rows) > 0) {
			echo("<div id='filter'>\n");
			echo("<label>Omschrijving bevat</label><input id='tbOmsFilter' OnKeyUp=\"fnFilter('logboek', 'tbOmsFilter', 1);\">");
			printf("<p class='aantrecords'>%d rijen</p>\n", count($rows));
			echo("</div> <!-- Einde filter -->\n");
			echo(fnDisplayTable($rows, "", "Mutaties in lidgegevens", 0, "", "", "logboek"));
		} else {
			echo("<p>Er zijn geen mutaties bekend.</p>\n");
		}
		
	} else {
		echo("<div id='filter'>\n");
		$zk2 = 3;
		if ($_SESSION['settings']['woonadres_anderen_tonen'] == 0) {
			$zk2 = 2;
		}
		printf("<label>Naam/E-mail bevat</label><input type='text' name='tbNaamFilter' id='tbNaamFilter' placeholder='Achter-, roepnaam of e-mail' OnKeyUp=\"fnFilter('ledenlijst', 'tbNaamFilter', 1, %d);\">\n", $zk2);
		printf("<form action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
		
		if ($currenttab2 != "Klosleden") {
			printf("<label>Onderdeel</label><select name='lbGroepFilter' onchange='this.form.submit();'>\n<option value=0>** Iedereen **</option>\n%s</select>\n", (new cls_Onderdeel())->htmloptions($_SESSION['val_groep']));
		}

		if (count($rows) > 1) {
			printf("<p class='aantrecords'>%d %s</p>\n", count($rows), $currenttab2);
		}
		echo("</form>\n");
		echo("</div>  <!-- Einde filter -->\n");
		
		if (count($rows) > 0) {
			echo(fnDisplayTable($rows, $ldl, "", 0, $llw, "", "ledenlijst", "", "", 0, $arrSort));
			foreach ($rows as $row) {
				$sel_leden[] = $row->lnkNummer;
			}
			$_SESSION['sel_leden'] = $sel_leden;
		}
	}
	
} # fnLedenlijst

function fnAfdeling() {
	global $currenttab, $currenttab2;
	
	$f = sprintf("`Type`='A' AND Naam='%s'", $currenttab);
	$afdid = (new cls_Onderdeel())->max("RecordID", $f);
	
	fnDispMenu(2);
	
	if ($currenttab2 == "Afdelingslijst") {
		fnAfdelingslijst($afdid);
	} elseif ($currenttab2 == "Groepsindeling") {
		fnGroepsindeling("", $afdid);
	} elseif ($currenttab2 == "Groepsindeling muteren") {
		fnGroepsindeling("", $afdid, 1);
	} elseif ($currenttab2 == "Kalender muteren") {
		fnAfdelingskalenderMuteren($afdid);
	}
	
}  # fnAfdeling

function fnAfdelingslijst($afdid) {
	global $ldl;
	
	$i_dp = new cls_Diploma();
	
	$arrSort[] = "Naam lid;L.Achternaam";
	$arrSort[] = "Woonplaats;L.Woonplaats";
	$arrSort[] = "Functie;F.Omschrijv";
	$arrSort[] = "Groep;GR.Kode";
	$arrSort[] = "Opmerking;LO.Opmerk";
	$arrSort[] = "Vanaf;LO.Vanaf";
	$arrSort[] = "Totenmet;LO.Opgezegd";
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$naamfilter = $_POST['tbNaamFilter'];
		$funcfilter = $_POST['tbFuncFilter'];
		$diplfilter = $_POST['bezitdiploma'];
	} else {
		$naamfilter = "";
		$funcfilter = "";
		$diplfilter = -1;
	}
	
	$xf = "";
	if ($diplfilter > 0) {
		$xf = sprintf("L.RecordID IN (SELECT LD.Lid FROM %sLiddipl AS LD WHERE LD.DiplomaID=%d AND IFNULL(LD.LicentieVervallenPer, CURDATE()) >= CURDATE())", TABLE_PREFIX, $diplfilter);
	}
	
	$rows = (new cls_Lidond())->onderdeellijst($afdid, 1, $xf, fnOrderBy($arrSort));
	
	echo("<div id='filter'>\n");
	printf("<form action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);

	printf("<label>Naam bevat</label><input type='text' name='tbNaamFilter' id='tbNaamFilter' value='%s' placeholder='Achter- of roepnaam' OnKeyUp=\"fnFilterTwee('afdelingslijst', 'tbNaamFilter', 'tbFuncFilter', 1, 5);\">\n", $naamfilter);
	printf("<label>Functie bevat</label><input type='text' name='tbFuncFilter' id='tbFuncFilter' value='%s' placeholder='Functie' OnKeyUp=\"fnFilterTwee('afdelingslijst', 'tbNaamFilter', 'tbFuncFilter', 1, 5)\">\n", $funcfilter);
	printf("<label>Bezit diploma</label><select name='bezitdiploma' onchange='this.form.submit();'>\n<option value=-1>Geen filter</option>\n%s</select>\n", $i_dp->htmloptions($diplfilter, -1, $afdid));
	if (count($rows) > 1) {
		printf("<p class='aantrecords'>%d rijen</p>\n", count($rows));
	}
	echo("</form>\n");
	echo("</div>  <!-- Einde filter -->\n");

	if (count($rows) > 0) {
		echo(fnDisplayTable($rows, $ldl, "", 0, "", "", "afdelingslijst", "", "", 0, $arrSort));
		echo("<script>fnFilterTwee('afdelingslijst', 'tbNaamFilter', 'tbFuncFilter', 1, 5);</script>\n");
		foreach ($rows as $row) {
			$sel_leden[] = $row->lnkNummer;
		}
		$_SESSION['sel_leden'] = $sel_leden;
	}
	
	$i_dp = null;
	
} # fnAfdelingslijst

function fnOnderdelenmuteren($ondtype="G") {
	global $currenttab, $currenttab2;
	
	$i_lo = new cls_Lidond();
	$i_ond = new cls_Onderdeel();
	$i_lid = new cls_Lid();
	
	if ($ondtype == "C") {
		$ondnaammv = "Commissies";
	} else	if ($ondtype == "R") {
		$ondnaammv = "Rollen";
	} else {
		$ondnaammv = ARRTYPEONDERDEEL[$ondtype] . "en";
	}
	
	fnDispMenu(2);
	
	echo("<div id='onderdelenmuteren'>\n");
	
//	echo("<h2>Werk in uitvoering, er mag getest worden.</h2>\n");
	
	if (isset($_GET['Scherm'])) {
		$scherm = $_GET['Scherm'];
	} else {
		$scherm = "O";
	}
	$onderdeelid = 0;
	if (isset($_GET['OnderdeelID']) and $_GET['OnderdeelID'] > 0) {
		$onderdeelid = intval($_GET['OnderdeelID']);
	} elseif (isset($_POST['OnderdeelID']) and $_POST['OnderdeelID'] > 0) {
		$onderdeelid = intval($_POST['OnderdeelID']);
	} elseif (isset($_POST['muteerleden'])) {
		$onderdeelid = intval(str_replace("muteerleden_", "", $_POST['muteerleden']));
		$scherm = "L";
	}
	
	if (isset($_POST['del_lid']) and $_POST['del_lid'] > 0) {
		$lidid = intval($_POST['del_lid']);
	} elseif (isset($_POST['add_lid']) and strlen($_POST['add_lid']) > 0) {
		$lidid = intval($_POST['add_lid']);
	} elseif (isset($_GET['lidid']) and $_GET['lidid'] > 0) {
		$lidid = intval($_GET['lidid']);
	} else {
		$lidid = 0;
	}
	
	if (isset($_GET['recid']) and strlen($_GET['recid']) > 0 and is_numeric($_GET['recid'])) {
		$f = sprintf("RecordID=%d", $_GET['recid']);
		$lidid = $i_lo->max("Lid", $f);
	}
	
	$alleenleden = 1;
	$f = sprintf("RecordID=%d", $onderdeelid);
	if ($i_ond->max("GekoppeldAanQuery", $f) == 0) {
		$muteerbaar = true;
		$alleenleden = $i_ond->max("Alleen leden", $f);
	} else {
		$muteerbaar = false;
	}

	$ondrows = $i_ond->editlijst($ondtype);
	if ($_SERVER['REQUEST_METHOD'] == "POST" and isset($_POST['formname'])) {
		if ($_POST['formname'] == "muteren_onderdelen") {
			$omz["Kode"] = "Code";
			$omz["Naam"] = "Naam";
			$omz["CentraalEmail"] = "Centraal_E-mail";
			foreach ($ondrows as $row) {
				foreach ($omz as $kolomnaam => $bascn) {
					$contr_name = $bascn . "_" . $row->lnkRecordID;
					if ($kolomnaam == "Alleen leden" and isset($_POST[$contr_name])) {
						$val = 1;
					} elseif ($kolomnaam == "Alleen leden") {
						$val = 0;
					} else {
						$val = $_POST[$contr_name];
					}
					$i_ond->update($row->lnkRecordID, $kolomnaam, $val);
				}
			}
			
		} elseif ($_POST['formname'] == "muteren_leden_onderdeel") {
			$omz["Functie"] = "Functie";
			$omz["EmailFunctie"] = "Email";
			$omz["Opmerk"] = "Opmerking";
			$omz["Vanaf"] = "Vanaf";
			$omz["Opgezegd"] = "Tot_en_met";
			$rows = $i_lo->selectielijst($onderdeelid);
			foreach ($rows as $row) {
				foreach ($omz as $kolomnaam => $bascn) {
					$contr_name = $bascn . "_" . $row->llkNummer;
					if (isset($_POST[$contr_name])) {
						$i_lo->update($row->llkNummer, $kolomnaam, $_POST[$contr_name]);
					}
				}
			}
		} elseif ($_POST['formname'] == "muteren_detail_onderdeel") {
			$oid = $_GET['OnderdeelID'];
			
			foreach (array("Kode", "Naam", "CentraalEmail", "Kader", "Alleen Leden", "VervallenPer", "HistorieOpschonen", "MaximaleLengtePeriode", "MySQL") as $key) {
				$cn = str_replace(" ", "", strtolower($key));
				if ($key == "Kader" or $key == "Alleen Leden") {
					if (isset($_POST[$cn])) {
						$_POST[$cn] = 1;
					} else {
						$_POST[$cn] = 0;
					}
				}
				if (isset($_POST[$cn])) {
					$i_ond->update($oid, $key, $_POST[$cn]);
				}
			}
			if (isset($_POST['Bewaren_Sluiten'])) {
				$scherm = "B";
				$i_lo->autogroepenbijwerken(0, 1);
			} else {
				$scherm = "W";
			}
		}

		if (isset($_POST['NieuwOnderdeel']) and strlen($_POST['NieuwOnderdeel']) > 1) {
			$i_ond->add($ondtype, $_POST['NieuwOnderdeel']);
			$scherm = "S";
		}
	} else {
		$i_ond->datacontrole();
	}
	
	if (isset($_POST['add_lid']) and $_POST['add_lid'] > 0 and $muteerbaar) {
		$i_lo->add($onderdeelid, $lidid);
		$scherm = "L";
	}
	
	if ($muteerbaar) {
		if (isset($_POST['LedenToevoegen']) and $_POST['LedenToevoegen'] == "Leden toevoegen") {
			add_del_selectie("add", $onderdeelid);
			$scherm = "L";
		} elseif (isset($_POST['LedenVerwijderen']) and $_POST['LedenVerwijderen'] == "Leden verwijderen") {
			add_del_selectie("del", $onderdeelid);
			$scherm = "L";
		} elseif (isset($_POST['AlleLedenVerwijderen'])) {
			add_del_selectie("deleteall", $onderdeelid);
			$scherm = "L";
		}
	}
	if ($scherm == "L") {
		$th = sprintf("Leden %s '%s' muteren", strtolower(ARRTYPEONDERDEEL[$ondtype]), $i_ond->naam($onderdeelid));
		
		printf("<form method='post' action='%s?tp=%s&amp;OnderdeelID=%d&Scherm=L'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $onderdeelid);
		echo("<input type='hidden' name='formname' value='muteren_leden_onderdeel'>\n");
		if ($i_ond->autogroep($onderdeelid) == false) {
			$nl = sprintf("<td colspan=9><select name='add_lid' onChange='this.form.submit();'>\n");
			$nl .= sprintf("<option value=0>Lid toevoegen ...</option>\n");
			$xf = sprintf("L.RecordID NOT IN (SELECT Lid FROM %sLidond AS LO WHERE LO.OnderdeelID=%d AND (LO.Opgezegd IS NULL))", TABLE_PREFIX, $onderdeelid);
			$nl .= $i_lid->htmloptions(-1, $alleenleden, $xf);
			$nl .= sprintf("</select></td>&nbsp;\n");
			
			if (!isset($_POST['selectie_status'])) {
				$_POST['selectie_status'] = "L";
			}
		} else {
			$nl = "";
		}
		
		$rows = $i_lo->selectielijst($onderdeelid);
		$f = sprintf("OnderdeelID=%d", $onderdeelid);
		$rowsfunc = (new cls_Functie())->selectlijst("L", $i_lo->min("Vanaf", $f));
		echo(fnDisplayTable($rows, "", $th, 0, "", $nl, "ledenperonderdeelmuteren", "", "dteVanaf, emlEmail, cboFunctie, Opmerking, dteTot en met", 1, "", $rowsfunc));
		
		if (($ondtype == "G") and $i_ond->autogroep($onderdeelid) == false and 1 == 2) {
			echo("<table>\n");
			echo("<tr><th colspan=3>Meerdere leden toevoegen of verwijderen</th></tr>\n");
			echo("<tr><td></td><td>Vanaf</td><td>Tot en met</td></tr>\n");
			
			$i_lid = new cls_Lid();
			$i_lm = new cls_Lidmaatschap();
			printf("<tr><th>Geboortedatum:</th><td><input type='date' value='%s' name='selectie_vangebdatum'></td><td><input type='date' value='%s' name='selectie_temgebdatum'></td></tr>\n", $i_lid->min("GEBDATUM"), $i_lid->max("GEBDATUM"));
			printf("<tr><th>Postcode:</th><td><input type='text' name='selectie_vanpostcode' value='%s' maxlength=7></td><td><input type='text' name='selectie_tempostcode' value='%s' maxlength=7></td></tr>\n", $i_lid->min("Postcode"), $i_lid->max("Postcode"));
			printf("<tr><th>Lidnummer:</th><td><input type='number' name='selectie_vanlidnr' value='%s'></td><td><input type='number' name='selectie_temlidnr' value='%s'></td></tr>\n", $i_lm->minlidnr(), $i_lm->maxlidnr());
			printf("<tr><th>Lid vanaf:</th><td><input type='date' name='selectie_vanliddatum' value='%s'></td><td><input type='date' name='selectie_temliddatum' value='%s'></td></tr>\n", $i_lm->min("LIDDATUM"), $i_lm->max("LIDDATUM"));
			printf("<tr><th>Opgezegd per:</th><td><input type='date' name='selectie_vanopgezegd'></td><td><input type='date' name='selectie_temopgezegd' value='%s'></td></tr>\n", $i_lm->max("Opgezegd"));
			
			echo("<tr><th>Lidstatus:</th><td colspan=2><select name='selectie_status' onChange='this.form.submit();'>\n");
			printf("<option value='L'%s>Lid</option>", checked("L", "option", $_POST['selectie_status']));
			printf("<option value='V'%s>Voormalig&nbsp;lid</option>", checked("V", "option", $_POST['selectie_status']));
			if ($i_lm->aantal("LIDDATUM > CURDATE()") > 0) {
				printf("<option value='T'%s>Toekomstig lid</option>", checked("T", "option", $_POST['selectie_status']));
			}
			printf("<option value='K'>Kloslid</option>", checked("K", "option", $_POST['selectie_status']));
			echo("</select></td></tr>\n");
			
			$i_lid = null;
			$i_lm = null;
		
			$selgr = "";
			if ($_POST['selectie_status'] == "L") {
				$rows = $i_ond->lijst(1);
			} else {
				$rows = $i_ond->lijst(0);
			}
			foreach($rows as $grp) {
				if ($grp->RecordID != $onderdeelid) {
					$selgr .= sprintf("<option value=%d>%s</option>\n", $grp->RecordID, $grp->Naam);
				}
			}
			printf("<tr><th>Zit in groep:</th><td colspan=2><select name='selectie_groep'>\n
						<option value=0>&nbsp;</option>\n%s</select></td></tr>\n", $selgr);
			
			printf("<tr><th>Zit niet in groep:</th><td colspan=2><select name='selectie_niet_groep'>\n
					<option value=0>&nbsp;</option>\n%s</select></td></tr>\n", $selgr);
				
			$seldp = (new cls_diploma())->htmloptions(0);
						
			printf("<tr><th>In het bezit van diploma:</th><td colspan=2><select name='selectie_diploma'>\n
						<option value=0>&nbsp;</option>\n%s</select></td></tr>\n", $seldp);
			
			printf("<tr><th>Niet in bezit van diploma:</th><td colspan=2><select name='selectie_niet_diploma'>\n
					<option value=0>&nbsp;</option>\n%s</select></td></tr>\n", $seldp);
			
			echo("</table>\n");
		
			printf("<input type='hidden' name='OnderdeelID' value=%d>\n", $onderdeelid);
			echo("<input type='submit' name='LedenToevoegen' value='Leden toevoegen'>\n");
			if ($aant_lid > 0) {
				echo("<input type='submit' name='LedenVerwijderen' value='Leden verwijderen'>\n");
				echo("<input type='submit' name='AlleLedenVerwijderen' value='Alle leden verwijderen'>\n");
			}
			echo("</form>\n");
		}
	
		echo("<div id='opdrachtknoppen'>\n");
		$vor = 0;
		$vol = 0;
		for ($t=0;$t<count($ondrows);$t++) {
			if ($ondrows[$t]->llkRecordID == $onderdeelid) {
				if ($t > 0) {
					$vor = $ondrows[$t-1]->llkRecordID;
				}
				if ($t < (count($ondrows)-1)) {
					$vol = $ondrows[$t+1]->llkRecordID;
				}
			}
		}
		if ($vor > 0) {
			printf("<input type='button' onclick=\"location.href='%s?tp=%s&amp;Scherm=L&amp;OnderdeelID=%d';\" value='Vorige %s'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $vor, ARRTYPEONDERDEEL[$ondtype]);
		}
		printf("<input type='button' onclick=\"location.href='%s?tp=%s&amp;Scherm=S';\" value='Overzicht %s'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $ondnaammv);
		if ($vol > 0) {
			printf("<input type='button' onclick=\"location.href='%s?tp=%s&amp;Scherm=L&amp;OnderdeelID=%d';\" value='Volgende %s'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $vol, ARRTYPEONDERDEEL[$ondtype]);
		}
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		
		echo("<div class='clear'></div>\n");
		
	} elseif ($scherm == "W" and $onderdeelid > 0) {
		
		$row = $i_ond->record($onderdeelid);
		
		echo("<div id='formulier'>\n");
		printf("<form method='post' action='%s?tp=%s&amp;OnderdeelID=%d&Scherm=W'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $onderdeelid);
		echo("<input type='hidden' name='formname' value='muteren_detail_onderdeel'>\n");
		
		printf("<label>RecordID</label><input type='text' value='%s' readonly>\n", $row->RecordID);
		printf("<label>Code</label><input type='text' name='kode' value='%s' maxlength=5>\n", $row->Kode);
		printf("<label>Naam</label><input type='text' name='naam' value='%s' maxlength=50>\n", $row->Naam);
		printf("<label>Centraal e-mailadres</label><input type='email' name='centraalemail' value='%s' maxlength=50>\n", $row->CentraalEmail);
		printf("<label>Is kader</label><input type='checkbox' name='kader' %s>\n", checked($row->Kader));
		printf("<label for='alleenleden'>Alleen leden</label><input type='checkbox' name='alleenleden' id='alleenleden' %s>\n", checked($row->AlleenLeden));
		printf("<label for='vervallenper'>Vervallen per</label><input type='date' name='vervallenper' id='vervallenper' value='%s'>\n", $row->VervallenPer);
		printf("<label>Historie opschonen</label><input type='number' name='historieopschonen' value=%d><p>in dagen</p>\n", $row->HistorieOpschonen);
		printf("<label>Maximale periode</label><input type='number' name='maximalelengteperiode' value=%d><p>in dagen</p>\n", $row->MaximaleLengtePeriode);
		
		if (($ondtype == "G"  or $ondtype == "R") and $_SESSION['webmaster'] == 1) {
			printf("<label>MySQL-code automatisch bijwerken</label><textarea id='mysql' name='mysql' rows=5 cols=100>%s</textarea>\n", $row->MySQL);
			if (strlen($row->MySQL) > 10 and (new cls_db_base())->controleersql($row->MySQL, 1) == false) {
				echo("<p class='mededeling'>Deze code kan niet worden uitgevoerd.</p>");
			} elseif (strlen($row->MySQL) > 10) {
				$st = microtime(true);
				$result = (new cls_db_base())->execsql($row->MySQL);
				printf("<label>Duur query in seconden</label><p>%f</p>\n", microtime(true) - $st);
				printf("<label>Aantal rijen</label><p>%s</p>\n", count($result->fetchAll()));			
			}
		}
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<input type='submit' value='Bewaren' name='Bewaren'>\n");
		echo("<input type='submit' value='Bewaren en sluiten' name='Bewaren_Sluiten'>\n");
		echo("</div>\n");
		
		echo("</form>\n");
		echo("</div> <!-- Einde formulier -->\n");

	} else {

		$ondrows = $i_ond->editlijst($ondtype);
		$link_lk = sprintf("<a href='%s?tp=%s&amp;Scherm=L&amp;OnderdeelID=%%d'><img src='./images/users-group.png' title='Leden %s' alt='Leden'></a>\n", $_SERVER['PHP_SELF'], $_GET['tp'], strtolower(ARRTYPEONDERDEEL[$ondtype]));
		$link_ek = sprintf("<a href='%s?tp=%s&amp;Scherm=W&amp;OnderdeelID=%%d'><img src='%s' title='muteren %s' alt='muteren'></a>\n", $_SERVER['PHP_SELF'], $_GET['tp'], BASE64_MUTEER, strtolower(ARRTYPEONDERDEEL[$ondtype]));

		echo("<div class='clear'></div>\n");
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		echo("<input type='hidden' name='formname' value='muteren_onderdelen'>\n");
		$xr = "<tr><td></td><td><input type='text' class='code' OnChange='this.form.submit();' name='NieuwOnderdeel' value=''></td><td>Vul de code in om een nieuwe toe te voegen</td></tr>\n";
		echo(fnDisplayTable($ondrows, $link_ek, $ondnaammv . " muteren", 0, $link_lk, $xr, "editonderdelen", "", "Code, Naam", 1));
		
		echo("</form>\n");
	}
	
	echo("</div>  <!-- Einde onderdelenmuteren -->\n");	

} # fnOnderdelenmuteren

function fnGroepsindeling($afdkode="", $afdid=0, $p_muteren=0, $p_toonleeftijd=2) {
	/* p_toonleeftijd
	0: nee
	1: ja, alle
	2: alleen bij jonger dan 18 jaar
	*/
	
	$hvafd = "";
	$hvtijd = "";
	$hvgroep = "";
	
	if (strlen($afdkode) > 0 and $afdid == 0) {
		$f = sprintf("Kode='%s'", $afdkode);
		$afdid = (new cls_Onderdl())->max($f);
	}
	
	if ($afdid > 0) {
		$filter = sprintf("O.RecordID=%d", $afdid);
	} else {
		$filter = "";
	}
	
	echo("<div id='groepsindeling'>\n");
	
	if ($p_muteren == 0) {
		
		$i_ak = new cls_afdelingskalender();
		$toonpresentie = 0;
		$f = sprintf("OnderdeelID=%d", $afdid);
		if ($i_ak->aantal($f) > 0) {
			if (isset($_POST['toonpresentie'])) {
				$toonpresentie = $_POST['toonpresentie'];
			} else {
				$f = "Datum >= CURDATE()";
				$toonpresentie = $i_ak->min("RecordID", $f);
			}
			printf("<form name='grpfilter' method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
			printf("<label>Inclusief aanwezigheid: </label><p><select name='toonpresentie' OnChange='this.form.submit();'>\n<option value=0>Geen</option>\n%s</select></p>", $i_ak->htmloptions($afdid, $toonpresentie));
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
		
			if ($hvtijd !== $row->Tijdsblok or $hvgroep !== $row->GroepOms) {
				if (strlen($hvgroep) > 0) {
					echo("</ol>");
					echo("</div>  <!-- einde groepsindelingkolom -->\n");
					if ($hvtijd !== $row->Tijdsblok) {
						echo("<div class='clear'></div>\n");
					}
				}
				echo("<div class='groepsindelingkolom'>\n");
				$volgnr = 1;
			
				$hvtijd = $row->Tijdsblok;
				if (strlen($hvtijd) > 3) {
					printf("<h3>%s</h3>\n", $hvtijd);
				}
			
				$hvgroep = $row->GroepOms;
				printf("<h4>%s</h4>\n", $row->GroepOms);
				echo("<ol>\n");
			}
			$cl = "";
			if ($toonpresentie > 0) {
				$stat = (new cls_Aanwezigheid())->status($row->RecordID, $toonpresentie);
				if (strlen($stat) > 0) {
					$cl = sprintf("class='presstat_%s'", strtolower($stat));
				}
			}
			$nm = $row->NaamLid;
			if (strlen($row->Leeftijd) > 5 and ($p_toonleeftijd == 1 or ($p_toonleeftijd == 2 and intval(substr($row->Leeftijd, 0, 2)) < 18))) {
				$nm .= " (" .  $row->Leeftijd . ")";
			}
			printf("<li %s>%s</li>\n", $cl, $nm);
			$volgnr++;
		}
		
	} elseif ($afdid > 0) {
		
		$kn[0] = "RecordID";
		$kn[] = "Kode";
		$kn[] = "Omschrijving";
		$kn[] = "Instructeurs";
		$kn[] = "Starttijd";
		$kn[] = "Eindtijd";
		
		$kl[1] = 6;
		$kl[] = 45;
		$kl[] = 45;
		$kl[] = 5;
		$kl[] = 5;
		
		$i_lo = new cls_Lidond();
		$i_gr = new cls_Groep();
			
		if ($_SERVER['REQUEST_METHOD'] == "POST") {
			//Bijwerken groepsindeling
			foreach ($i_lo->groepsindeling($afdid) as $row) {
				$p = sprintf("mg_%d", $row->RecordID);
				if (isset($_POST[$p]) and $_POST[$p] != $row->GroepID) {
					$i_lo->update($row->RecordID, "GroepID", $_POST[$p]);
				}
			}
			//Bijwerken afdelingsgroepen
			foreach ($i_gr->selectlijst($afdid) as $row) {
				for ($i=1;$i<=5; $i++) {
					$pn = sprintf("%s_%d", strtolower($kn[$i]), $row->RecordID);
					if (isset($_POST[$pn])) {
						$i_gr->update($row->RecordID, $kn[$i], $_POST[$pn]);
					}
				}
			}
			
			if (isset($_POST['kode_nw']) and strlen($_POST['kode_nw']) > 0) {
				$i_gr->add($afdid, $_POST['kode_nw']);
			}
		}
		
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		
		$grrows = $i_gr->selectlijst($afdid);
		echo("<table>\n");
		echo("<tr><th>Naam</th><th>Leeftijd</th><th>Laatst behaalde diploma's</th><th>Groep</th></tr>\n\n");
		foreach ($i_lo->onderdeellijst($afdid, 1, "", "GR.Kode, L.Achternaam, L.Tussenv, L.Roepnaam") as $row) {
			$options = "<option value=0>Niet ingedeeld</option>\n";
			foreach ($grrows as $grrow) {
				$options .= sprintf("<option value=%d %s>%s</option>\n", $grrow->RecordID, checked($row->ndGroepID, "option", $grrow->RecordID), $grrow->GroepOms);
			}
			$nm = $row->Naam_lid;
			if (strlen($row->ndAfkFunc) > 0) {
				$nm .= " (" . $row->ndAfkFunc . ")";
			}
			$ld = (new cls_Liddipl())->lidlaatstediplomas($row->llkNummer, 3);
			printf("<tr><td>%s</td><td>%s</td><td>%s</td><td><select name='mg_%d' OnChange='this.form.submit();'>%s</select></td></tr>\n", $nm, $row->ndLeeftijd, $ld, $row->ndRecordID, $options);
		}
		echo("</table>\n");
		
		echo("<table>\n");
		echo("<caption>Muteren groepen</caption>\n");
		echo("<tr><th>Code</th><th>Omschrijving</th><th>Instructeurs</th><th>Starttijd</th><th>Eindtijd</th></tr>\n");
		
		$f = sprintf("OnderdeelID=%d", $afdid);
		foreach ($i_gr->basislijst($f, "Kode") as $row) {
			if ($row->RecordID > 0) {
				echo("<tr>\n");
				for ($i=1;$i<=5; $i++) {
					if ($i >= 4) {
						$tp = "time";
					} else {
						$tp = "text";
					}
					printf("<td><input type='%s' name='%s_%d' size=%d maxlength=%d value='%s'></td>\n", $tp, strtolower($kn[$i]), $row->RecordID, $kl[$i], $kl[$i], $row->{$kn[$i]});
				}
			
				echo("</tr>\n");
			}
		}
		
		printf("<tr><td>Nieuw</td><td colspan=4><input type='text' name='kode_nw' max-length=%d placeholder='Code'></td></tr>\n", $kl[1]);
		echo("</table>\n");
		
		echo("<div id='knoppenbalk'>\n");
		echo("<input type='submit' value='Groepen bewaren'>\n");
		echo("</div> <!-- Einde knoppenbalk -->\n");
		
		echo("</form>\n");
	}
	
	echo("</div>  <!-- Einde groepsindeling -->\n");
	
} # fnGroepsindeling

function fnAfdelingskalenderMuteren($p_onderdeelid){
	
	$i_ak = new cls_Afdelingskalender();
	$i_aanw = new cls_Aanwezigheid();
	
	$akid = 0;
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		foreach ($_POST as $k => $v) {
			if (startwith($k, "aanw_")) {
				$loid = intval(str_replace("aanw_", "", $k));
				$i_aanw->update($loid, $_POST['akid'], "Status", $v);
			}
		}
	
		if (isset($_POST['selecteerdatum'])) {
			if ($_POST['selecteerdatum'] == 0) {
				$akid = $i_ak->add($p_onderdeelid);
			} else {
				$akid = $_POST['selecteerdatum'];
			}
		}
			
		if (isset($_POST['akid']) and $_POST['akid'] > 0) {
			if (isset($_POST['Datum']) and strlen($_POST['Datum']) == 10) {
				$i_ak->update($_POST['akid'], "Datum", $_POST['Datum']);
			}
			
			if (isset($_POST['Omschrijving'])) {
				$i_ak->update($_POST['akid'], "Omschrijving", $_POST['Omschrijving']);
			}
			
			if (isset($_POST['Activiteit'])) {
				$i_ak->update($_POST['akid'], "Activiteit", 1);
			} else {
				$i_ak->update($_POST['akid'], "Activiteit", 0);
			}
		}
	}
	
	echo("<div id='afdelingskalendermuteren'>\n");
	printf("<form name='kalmut' method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	
	echo("<label>Selecteer dag:</label><p>");
	echo("<select name='selecteerdatum' OnChange='this.form.submit();'>\n");
	echo("<option value=-1>...</option>\n");
	$dat = "";
	$oms = "";
	$act = false;
	foreach ($i_ak->lijst($p_onderdeelid) as $row) {
		printf("<option value=%d %s>%s</option>\n", $row->RecordID, checked($row->RecordID, "option", $akid), strftime("%A %e %B %Y", strtotime($row->Datum)));
		if ($akid == $row->RecordID) {
			$dat = $row->Datum;
			$oms = $row->Omschrijving;
			if ($row->Activiteit == 1) {
				$act = true;
			}
		}
	}
	echo("<option value=0>Nieuwe datum</option>\n");
	echo("</select></p>\n");
	echo("<div class='clear'></div>\n");
	
	if ($akid > 0) {
		printf("<input type='hidden' name='akid' value=%d>\n", $akid);
		$ro = "";
		$f = sprintf("AfdelingskalenderID=%d", $akid);
		if ($i_aanw->aantal($f) > 0) {
			$ro = "readonly";
		}
		printf("<label>Datum:</label><p><input type='date' name='Datum' value='%s' %s></p>\n", $dat, $ro);
		printf("<label>Omschrijving:</label><p><input type='text' name='Omschrijving' value='%s' size=75 max-length=75 OnChange='this.form.submit();'></p>\n", $oms);
//		printf("<label>Activiteit gepland?</label><p><input type='checkbox' name='Activiteit' %s  OnChange='this.form.submit();'></p>\n", checked($act));
		echo("<div class='clear'></div>\n");
		
		$i_lo = new cls_Lidond();
		
		echo("<table>\n");
		printf("<tr><th colspan=5>Presentielijst %s</th></tr>\n", strftime("%A %e %B %Y", strtotime($dat)));
		echo("<tr><th>Naam</th><th>Functie</th><th>Groep</th><th>Aanwezigheid</th></tr>\n");
		
		foreach ($i_lo->lijst($p_onderdeelid) as $row) {
			$stat = $i_aanw->status($row->RecordID, $akid);
			if (strlen($stat) > 0) {
				$cl = sprintf("class='presstat_%s'", strtolower($stat));
			} else {
				$cl = "";
			}
			printf("<tr><td %s>%s</td><td>%s</td><td>%s</td>\n", $cl, $row->NaamLid, $row->Functie, $row->Groep);
			
			
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
	
	echo("</div> <!-- Einde afdelingskalendermuteren -->\n");
	$i_ak = null;
	$i_lo = null;
	
}  # fnAfdelingskalenderMuteren

function fnEigenGegevens($lidid=0, $actie="") {
	global $currenttab, $currenttab2, $currenttab3;
	
	if ($lidid == 0 and isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0) {
		$lidid = (new cls_Mailing_hist())->lidbijemail($_GET['MailingHistID']);
	}
	
	if ($currenttab == "Eigen gegevens") {
		$xtra_param = "";
		$lidid = $_SESSION['lidid'];
	} else {
		$xtra_param = sprintf("lidid=%d", $lidid);
	}
	
	if (isset($_GET['op'])) {
		$op = $_GET['op'];
	} else {
		$op = "";
	}
	
	if ($lidid <= 0) {
		echo("<p class='mededeling'>Er is geen lid geselecteerd.</p>\n");
	} elseif (toegang($_GET['tp'], 0) == false) {
		$mess = sprintf("Je hebt tot tabblad '%s' geen toegang.", $actie);
		(new cls_Logboek())->add($mess, 5, $lidid, 1);
	} else {
	
		fnDispMenu(2, $xtra_param);
		if ($currenttab != "Eigen gegevens") {
			fnDispMenu(3, $xtra_param);
		}
		$naamlid = htmlentities((new cls_Lid())->Naam($lidid));
				
		if ($actie == "Lidmaatschappen") {
			$rows = (new cls_Lidmaatschap())->overzichtlid($lidid);
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", $actie . " " . $naamlid, 0));
			} else {
				printf("<p class='mededeling'>%s heeft geen andere %s.</p>\n", $naamlid, $actie);
			}
			
		} elseif ($actie == "Afdelingen") {
			$rows = (new cls_Lidond())->overzichtlid($lidid, "A");
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", $actie . " " . $naamlid, 0, "", "", "onderdelenperlid"));
			} else {
				printf("<p class='mededeling'>%s heeft geen %s.</p>\n", $naamlid, $actie);
			}
			
		} elseif ($actie == "Groepen") {
			$rows = (new cls_Lidond())->overzichtlid($lidid, "G");
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", $actie . " " . $naamlid, 0, "", "", "onderdelenperlid"));
			} else {
				printf("<p class='mededeling'>%s is bij geen enkele groep ingedeeld.</p>\n", $naamlid);
			}
		} elseif ($actie == "Kader") {
			$rows = (new cls_Lidond())->overzichtlid($lidid, "K");
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", $actie . " " . $naamlid, 0, "", "", "onderdelenperlid"));
			} else {
				printf("<p class='mededeling'>%s is niet ingedeeld (geweest) bij het kader.</p>\n", $naamlid);
			}
		} elseif ($actie == "Rollen") {
			$rows = (new cls_Lidond())->overzichtlid($lidid, "R");
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", $actie . " " . $naamlid, 0, "", "", "onderdelenperlid"));
			} else {
				printf("<p class='mededeling'>%s heeft geen %s.</p>\n", $naamlid, $actie);
			}
		} elseif ($actie == "Toestemmingen") {
			$rows = (new cls_Lidond())->overzichtlid($lidid, "T");
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", $actie . " " . $naamlid, 0, "", "", "onderdelenperlid"));
			} else {
				printf("<p class='mededeling'>Er zijn geen toestemmingen bekend van %s.</p>\n", $naamlid);
			}
			if (strlen($_SESSION['settings']['uitleg_toestemmingen']) > 0) {
				printf("<p>%s</p>\n", $_SESSION['settings']['uitleg_toestemmingen']);
			}
			
		} elseif ($actie == "Bijzonderheden") {
			$rows = (new cls_Memo())->overzichtlid($lidid);
			echo("<div id='bijzonderheden'>\n");
			printf("<h2>Bijzonderheden %s</h2>\n", $naamlid);
			foreach ($rows as $row) {
				printf("<h3>%s</h3>\n", ARRSOORTMEMO[$row->Soort]);
				printf("<p>%s</p>\n", $row->Memo);
			}
			echo("</div> <!-- Einde bijzonderheden -->\n");
		} elseif ($actie == "Presentie") {
			$rows = (new cls_Aanwezigheid())->overzichtlid($lidid);
			if (count($rows) > 0) {				
				echo("<table>\n");
				printf("<tr><th colspan=3>Presentie %s</th></tr>\n", $naamlid);
				echo("<tr><th>Datum</th><th>Omschrijving</th><th>Status</th></tr>\n");
				foreach ($rows as $row) {
					printf("<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n", strftime("%A %e %B %Y", strtotime($row->Datum)), $row->Omschrijving, ARRPRESENTIESTATUS[$row->Status]);
				}
				echo("</table>\n");
			} else {
				printf("<p class='mededeling'>Bij %s is geen presentie bekend.</p>", $naamlid);
			}
			
		} elseif ($actie == "Bewaking") {
			$rows = (new cls_Bewaking())->overzichtlid($lidid);
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", "Bewaking " . $naamlid));
			} else {
				printf("<p class='mededeling'>%s heeft geen bewakingshistorie.</p>", $naamlid);
			}
			$row = (new cls_Bewaking())->lidbew($lidid);
			if (isset($row) and $row != null) {
				echo(fnDisplayForm($row));
			}
			$rows = db_insbew("overzichtlid", $lidid);
			if ($_SESSION['settings']['toneninschrijvingenbewakingen'] == 1 and count($rows) > 0) {
				echo(fnDisplayTable($rows, "", "Ingeschreven voor bewakingen"));
			}
			
		} elseif ($actie == "Diplomas") {
			$rows = (new cls_Liddipl())->overzichtlid($lidid);
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", "Diploma's " . $naamlid));
			} else {
				printf("<p class='mededeling'>Bij %s zijn geen diploma's bekend.</p>", $naamlid);
			}
			
		} elseif ($actie == "Rekeningen") {
			if ($op == "details" and isset($_GET['rid']) and $_GET['rid'] > 0) {
				echo(RekeningDetail($_GET['rid']));
			} else {
				$rows = (new cls_Rekeningregel())->overzichtlid($lidid);
				if (count($rows) > 0) {
					$lnk = sprintf("<a href='%s?tp=%s&amp;op=details&amp;lidid=%d&amp;rid=%%d' title='Details rekening'><img src='%s' alt='Zoom'></a>", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid, BASE64_DETAILS);
					echo(fnDisplayTable($rows, $lnk, "Rekeningen " . $naamlid));
				} else {
					printf("<p class='mededeling'>%s heeft geen rekeningen.</p>", $naamlid);
				}
			}
			
		} elseif ($actie == "Mailing") {
			if (isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0) {
				$xtra = "<p class='mededeling'><input type='button' value='Terug' onclick='history.go(-1);'></p>\n";
				$email = new email($_GET['MailingHistID']);
				$email->toon();
				$mail = null;
			} else {
				$ld = sprintf("<a href='index.php?tp=%s&amp;MailingHistID=%%d'>%%s</a>", urlencode($_GET['tp']));
				$rows = (new cls_Mailing_hist())->overzichtlid($lidid);
				if (count($rows) > 0) {
					echo(fnDisplayTable($rows, $ld, sprintf("Aan %s verstuurde e-mails", $naamlid)));
				} else {
					printf("<p class='mededeling'>Er zijn vanaf deze website geen mailings naar %s verstuurd.</p>\n", $naamlid);
				}
			}
			
		} elseif ($actie == "Evenementen") {
			$rows = (new cls_Evenement_Deelnemer())->overzichtlid($lidid);
			if (count($rows) > 0) {
				echo("<div id='evenementenperlid'>\n");
				echo("<table>\n");
				printf("<tr><th colspan=6>Deelname evenementen %s</th></tr>\n", $naamlid);
				echo("<tr><th>Wanneer</th><th>Omschrijving</th><th>Contact</th><th>Opmerking / Functie</th><th>Status</th><th>In agenda</th></tr>\n");
				
				foreach($rows as $row) {
					echo("<tr>\n");
					if (strlen($row->Verzameltijd) >= 5 and $row->dteDatum >= date("Y-m-d H:i")) {
						$v = sprintf("<br>Verzamelen om %s&nbsp;uur", $row->Verzameltijd);
					} else {
						$v = "";
					}
					printf("<td>%s&nbsp;tot&nbsp;%s&nbsp;uur%s</td>\n", str_replace(" ", "&nbsp;", str_replace("  ", "&nbsp;", strftime("%A %e %B %Y van %R", strtotime($row->dteDatum)))), $row->Eindtijd, $v);
					$oms = $row->Omschrijving;
					if (strlen($row->ndLocatie) > 1) {
						$oms .= "<br>\n" . $row->ndLocatie;
					}
					printf("<td>%s</td>\n", $oms);
					printf("<td>%s</td>\n", $row->emlContact);
					if (strlen($row->Opmerking) > 0 and strlen($row->Functie) > 0) {
						$of = sprintf("%s<br>%s", $row->Opmerking, $row->Functie);
					} elseif (strlen($row->Opmerking) > 0) {
						$of = $row->Opmerking;
					} else {
						$of = $row->Functie;
					}
					printf("<td>%s</td>\n", $of);
					printf("<td>%s</td>\n", $row->Status);
					if (($row->Status == "Aangemeld" or $row->Status == "Bevestigd" or $row->Status == "Ingeschreven") and $row->dteDatum >= date("Y-m-d H:i")) {
						printf("<td>%s</td>\n", fnAgendaKnop($row->dteDatum, $row->Eindtijd, $row->Verzameltijd, $row->Omschrijving, $row->ndLocatie));
					} else {
						echo("<td>&nbsp;</td>");
					}
					echo("</tr>\n");
				}
				
				echo("</table>\n");
				echo("</div>  <!-- einde evenementenperlid -->");
				
			} else {
				printf("<p class='mededeling'>Bij %s zijn geen deelnames aan evenementen bekend.</p>", $naamlid);
			}
		} elseif ($actie == "Logboek") {
			if ($lidid > 0) {
				$rows = (new cls_Logboek())->overzichtlid($lidid);
				if (count($rows) > 0) {
					echo(fnDisplayTable($rows, "", sprintf("Logboek %s", $naamlid)));
				} else {
					printf("<p class='mededeling'>Er is geen logging bij %s bekend.</p>\n", $naamlid);
				}
			} else {
				echo("<p class='mededeling'>Er is geen lidid bekend.</p>\n");
			}
		} else {
			$fn = fotolid($lidid, 1);
			$nl = (new cls_Lid())->Roepnaam($lidid);
			if (strlen($fn) > 3) {
				$xtra = sprintf("<div id='pasfoto'><img src='%s' alt='Pasfoto %s' title='Laatst gewijzigd op %s'></div>\n", $fn, $nl, strftime("%e %B %Y", filectime(fotolid($lidid, 0))));
			} else {
				$xtra = "";
			}
			$row = (new cls_Lid())->overzichtlid($lidid);
			echo(fnDisplayFormLabels($row, $xtra, "overzichtlid"));
		}
		if (isset($_SESSION['sel_leden']) and count($_SESSION['sel_leden']) > 1 and $currenttab != "Eigen gegevens" and !isset($_GET['MailingID'])) {
			$current_key = -1;
			foreach ($_SESSION['sel_leden'] as $key => $val) {
				if ($val == $lidid) {
					$current_key = $key;
				}
			}
			
			$lnk = sprintf("<input type='button' OnClick=\"location.href='%s?tp=%s&amp;lidid=%s';\" value='%s lid'>&nbsp;\n", $_SERVER['PHP_SELF'], "%s", "%d", "%s");
			echo("<div id='opdrachtknoppen'>\n");
			if ($current_key > 0) {
				printf($lnk, $_GET['tp'], $_SESSION['sel_leden'][0], "Eerste");
				printf($lnk, $_GET['tp'], $_SESSION['sel_leden'][$current_key-1], "Vorige");
			}
			if ($_SESSION['sel_leden'][count($_SESSION['sel_leden'])-1] != $lidid) {
				printf($lnk, $_GET['tp'], $_SESSION['sel_leden'][$current_key+1], "Volgende");
				printf($lnk, $_GET['tp'], $_SESSION['sel_leden'][count($_SESSION['sel_leden'])-1], "Laatste");
			}
			printf("<p>Lid %d van de %d</p>\n", $current_key+1, count($_SESSION['sel_leden']));
			echo("</div>  <!-- Einde opdrachtknoppen -->\n");
		}
	}
} # fnEigenGegevens

function fnSelectListGroepen($cv=0) {
	
	debug("fnSelectListGroepen todo: vervangen door class");

	$ret = "<option value=0>** Iedereen **</option>\n";
	$rows = (new cls_Onderdeel())->lijst(1);
	foreach ($rows as $row) {
		$ret .= sprintf("<option%s value=%d>%s</option>\n", checked($row->RecordID, "option", $cv), $row->RecordID, htmlentities($row->Naam));
	}
	return $ret;
} # fnSelectListGroepen

function overzichtverjaardagen($metfoto=1) {

	$verj = "";
	$verjfoto = "";
	$aantgetoond = 0;
	for ($i = 0; $i < $_SESSION['settings']['verjaardagenvooruit'] and $aantgetoond < $_SESSION['settings']['verjaardagenaantal']; $i++) {
		$dteHV = mktime(0, 0, 0, date("m"), date("d")+$i, date("Y"));
		foreach(db_lid("verjaardagen", date("Y-m-d", $dteHV)) as $row) {
			$o = (new cls_Lidond())->onderscheiding($row->RecordID);
			if (strlen($o) > 0) {
				$nm = $o . " ";
			} else {
				$nm = "";
			}
			$nm .= htmlentities($row->Naam_lid);
			if ($i == 0) {
				$t = sprintf("%s is vandaag %d jaar geworden", $nm, $row->Leeftijd);
			} elseif ($i == 1) {
				$t = sprintf("%s wordt morgen %d jaar", $nm, $row->Leeftijd);
			} else {
				$t = sprintf("Op %s is %s jarig", strftime("%A %e %B", $dteHV), $nm);
			}
			$fn = fotolid($row->RecordID, 1);
			if (strlen($t) > 3) {
				$verj .= sprintf("%s. ", $t);
				if (strlen($fn) > 3) {
					$verjfoto .= sprintf("<div class='jarige'><img src='%s' alt='Pasfoto %s'><div class='tekstbijfoto'>%s.</div></div>\n", $fn, htmlentities($row->Naam_lid), $t);
				} elseif (strlen($t) > 3) {
					$verjfoto .= sprintf("<p>%s.</p>\n", $t);
				}
			}
			$aantgetoond++;
		}
	}
	if ($metfoto == 1) {
		return $verjfoto;
	} else {
		return $verj;
	}
} # overzichtverjaardagen

function fotolid($lidid, $metversie=0) {

	$rv = "";
	foreach(PASFOTOEXTENTIES as $ext) {
		$fn = sprintf("%sPasfoto%d.%s", $_SESSION['settings']['path_pasfoto'], $lidid, $ext);
		if (file_exists($fn)) {
			$rv = $fn;
		} else {
			$fn = sprintf("%sPasfoto%d.%s", $_SESSION['settings']['path_pasfoto'], $lidid, strtoupper($ext));
			if (file_exists($fn)) {
				$rv = $fn;
			}
		}
	}
	if (strlen($rv) > 3 and $metversie == 1) {
		$rv .= "?v" . strftime("%Y%m%d%H%M", filectime($rv));
	}
	$rv = str_replace(BASEDIR, ".", $rv);
	
	return $rv;
	
} # fotolid

function algemeenlidmuteren($lidid) {
	global $actionurl, $currenttab, $currenttab2;
	
	$jsoc = "";
	if ($currenttab == "Zelfservice" or $_SESSION['AutoBewaren'] == 1) {
		$jsoc = "OnChange='this.form.submit();'";
	}
	
	$i_lid = new cls_Lid();
	
	if ($currenttab == "Zelfservice") {
		$lidid = $_SESSION['lidid'];
	} elseif (isset($_POST['lidid'])) {
		$lidid = intval($_POST['lidid']);
	}
	
	$gs = $i_lid->Geslacht($lidid);
	$sl = substr((new cls_Lidmaatschap())->soortlid($lidid), 0, 1);
		
	$wijzvelden[] = array('label' => "Roepnaam", 'naam' => "Roepnaam", 'lengte' => 17);
	if ($sl != "K") {
		$wijzvelden[] = array('label' => "Voorletters", 'naam' => "Voorletter", 'lengte' => 10);
	}
	$wijzvelden[] = array('label' => "Tussenvoegsels", 'naam' => "Tussenv", 'lengte' => 7);
	$wijzvelden[] = array('label' => "Achternaam", 'naam' => "Achternaam", 'lengte' => 30, 'nietverw' => true);
	if ($gs == "V") {
		$wijzvelden[] = array('label' => "Meisjesnaam", 'naam' => "Meisjesnm", 'lengte' => 25);
	}
	$wijzvelden[] = array('label' => "Geslacht", 'naam' => "Geslacht", 'nietverw' => true);
	if ($gs != "B") {
		$wijzvelden[] = array('label' => "Geboortedatum", 'naam' => "GEBDATUM", 'type' => 'date');
		$wijzvelden[] = array('label' => "Geboorteplaats", 'naam' => "GEBPLAATS", 'lengte' => 22);
	}
	if ($sl == "K") {
		$wijzvelden[] = array('label' => "Opmerking", 'naam' => "Opmerking", 'lengte' => 60);
	}
	$wijzvelden[] = array('label' => "Adres", 'naam' => "Adres", 'lengte' => 30);
	$wijzvelden[] = array('label' => "Postcode", 'naam' => "Postcode", 'lengte' => 7);
	$wijzvelden[] = array('label' => "Woonplaats", 'naam' => "Woonplaats", 'lengte' => 22);
	$wijzvelden[] = array('label' => "Vast telefoonnummer", 'naam' => "Telefoon", 'lengte' => 16);
	$wijzvelden[] = array('label' => "Mobiel", 'naam' => "Mobiel", 'lengte' => 16);
	$wijzvelden[] = array('label' => "E-mail", 'naam' => "Email", 'lengte' => 45);
	if ($_SESSION['settings']['zs_incl_emailvereniging'] == 1 or $currenttab2 == "Wijzigen lid") {
		$wijzvelden[] = array('label' => "E-mail vereniging", 'naam' => "EmailVereniging", 'lengte' => 45);
	}
	if (($_SESSION['settings']['zs_incl_emailouders'] == 1 or $currenttab2 == "Wijzigen lid") and $gs != "B") {
		$wijzvelden[] = array('label' => "E-mail ouders", 'naam' => "EmailOuders", 'lengte' => 45);
		$wijzvelden[] = array('label' => "Namen ouders", 'naam' => "NamenOuders", 'lengte' => 90);
	}
	$wijzvelden[] = array('label' => "Waarschuwen bij een noodgeval", 'naam' => "Waarschuwen bij nood", 'lengte' => 254);
	
	if ($_SESSION['settings']['zs_incl_iban'] == 1) {
		$wijzvelden[] = array('label' => "Bankrekening", 'naam' => "Bankrekening", 'lengte' => 18);
	}
	if ($_SESSION['settings']['zs_incl_machtiging'] == 1 or $currenttab == "Wijzigen lid") {
		$wijzvelden[] = array('label' => "Machtiging incasso afgegeven", 'naam' => "Machtiging afgegeven", 'lengte' => 1, 'type' => 'cb');
	}
	if ($gs != "B") {
		if ($_SESSION['settings']['zs_incl_vogafgegeven'] == 1 or $currenttab2 == "Wijzigen lid") {
			$wijzvelden[] = array('label' => "VOG afgegeven op", 'naam' => "VOG afgegeven", 'lengte' => 10, 'type' => 'date');
		}
		if ($_SESSION['settings']['zs_incl_bsn'] == 1 or $currenttab2 == "Wijzigen lid") {
			$wijzvelden[] = array('label' => "Burgerservicenummer", 'naam' => "Burgerservicenummer", 'lengte' => 9);
		}
		if ($_SESSION['settings']['zs_incl_slid'] == 1 or $currenttab2 == "Wijzigen lid") {
			$wijzvelden[] = array('label' => "Sportlink ID", 'naam' => "RelnrRedNed", 'lengte' => 7, 'nietverw' => false);
		}
		if ($_SESSION['settings']['zs_incl_legitimatie'] == 1 or $currenttab2 == "Wijzigen lid") {
			$wijzvelden[] = array('label' => "Legitimatietype", 'naam' => "Legitimatietype", 'nietverw' => true);
			$wijzvelden[] = array('label' => "Legitimatienummer", 'naam' => "Legitimatienummer", 'lengte' => 15);
		}
		if ($_SESSION['settings']['zs_incl_beroep'] == 1 or $currenttab2 == "Wijzigen lid") {
			$wijzvelden[] = array('label' => "Beroep", 'naam' => "Beroep", 'lengte' => 40);
		}
	}
		
	if (isset($_POST['Kloslid_Verwijderen'])) {
		$i_lid->update($lidid, "Verwijderd", date("Y-m-d"));
	} elseif (isset($_POST['Undo_Verwijderen'])) {
		$i_lid->update($lidid, "Verwijderd", "NULL");
	} elseif ($_SERVER['REQUEST_METHOD'] == "POST") {
		for ($i=0; $i < count($wijzvelden); $i++) {
			$fn = $wijzvelden[$i]['naam'];
			$contrname = str_replace(" ", "_", $fn);
			$v = "__skip__";
			if ($fn == "Machtiging afgegeven") {
				if (isset($_POST[$contrname])) {
					$v = 1;
				} else {
					$v = 0;
				}
				
			} elseif (isset($_POST[$contrname])) {
				$v = $_POST[$contrname];
			}
			if ($v !== "__skip__") {
				$i_lid->update($lidid, $fn, $v);
			}
		}
		
		if (isset($_POST['Sluiten'])) {
			if ($sl == "K") {
				printf("<script>location.href='%s?tp=Ledenlijst/Klosleden';</script>\n", $_SERVER['PHP_SELF']);
			} elseif ($sl == "V") {
				printf("<script>location.href='%s?tp=Ledenlijst/Voormalig+leden';</script>\n", $_SERVER['PHP_SELF']);
			} else {
				printf("<script>location.href='%s?tp=Ledenlijst/Leden';</script>\n", $_SERVER['PHP_SELF']);
			}
		}
	}

	$row = $i_lid->record($lidid);
	echo("<div id='wijzigengegevens'>\n");
	printf("<form method='post' action='%s' name='frm_wijzigingen'>\n", $actionurl);
	printf("<input type='hidden' name='lidid' value=%d>\n", $lidid);
	for ($i=0; $i < count($wijzvelden); $i++) {
		$dv = str_replace("\n", "<br>\n", $row->{$wijzvelden[$i]['naam']});
		if ($wijzvelden[$i]['naam'] == "Woonplaats" and strlen($dv) == 0 and strlen($row->Postcode) >= 4) {
			$dv = $i_lid->max("Woonplaats", sprintf("LEFT(Postcode, 4)='%s'", substr($row->Postcode, 0, 4)));
		} elseif ($wijzvelden[$i]['naam'] == "Adres" and strlen($dv) == 0 and strlen($row->Postcode) == 7) {
			$dv = $i_lid->max("Adres", sprintf("LEFT(Postcode, 7)='%s'", substr($row->Postcode, 0, 7)));
			if (strlen($dv) > 2 and strrpos($dv, " ") > 1) {
				$dv = substr($dv, 0, strrpos($dv, " "));
			}
		}
		$ph = $wijzvelden[$i]['label'];
		if ($wijzvelden[$i]['label'] == "Geslacht" or ($wijzvelden[$i]['label'] == "Legitimatietype")) {
			if ($wijzvelden[$i]['label'] == "Geslacht") {
				$an = "ARRGESLACHT";
			} else {
				$an = "ARRLEGITIMATIE";
			}
			$opt = "\n";
			foreach (constant($an) as $key => $val) {
				$sel = "";
				if ($key == $row->{$wijzvelden[$i]['naam']}) {
					$sel = " selected";
					$dv = $val;
				}
				$opt .= sprintf("<option value='%s'%s>%s</option>\n", $key, $sel, $val);
			}
			$inp = sprintf("<select name='%s' %s>%s</select>", $wijzvelden[$i]['naam'], $jsoc, $opt);
			
		} elseif (isset($wijzvelden[$i]['type']) and $wijzvelden[$i]['type'] == "cb") {
			if ($row->{$wijzvelden[$i]['naam']} == 1) {
				$dv = "Ja";
				$c = "checked";
			} else {
				$dv = "Nee";
				$c = "";
			}
			$inp = sprintf("<input type='checkbox' name='%s' value=1 %s %s>", $wijzvelden[$i]['naam'], $c, $jsoc);
		
		} else {
			if (isset($wijzvelden[$i]['type']) and $wijzvelden[$i]['type'] == "date") {
				$t = "date";
			} elseif (stripos($wijzvelden[$i]['label'], "e-mail") !== FALSE) {
				$t = "email";
			} else {
				$t = "text";
			}
			if ($t == "text" and $wijzvelden[$i]['lengte'] > 100) {
				$inp = sprintf("<textarea name='%s' placeholder='%s' cols=50 rows=4 %s>%s</textarea>", str_replace(" ", "_", $wijzvelden[$i]['naam']), $ph, $jsoc, $dv);
			} elseif ($t == "date") {
				$inp = sprintf("<input type='date' name='%s' value='%s' %s>", $wijzvelden[$i]['naam'], $dv, $jsoc);
			} else {
				$inp = sprintf("<input type='%s' name='%s' placeholder='%s' value='%s' maxlength=%d %s>", $t, $wijzvelden[$i]['naam'], $ph, $dv, $wijzvelden[$i]['lengte'], $jsoc);
			}
		}
		if ($row->Verwijderd > "2010-01-01") {
			printf('<tr><td class="label">%1$s</td><td>%2$s</td></tr>', $wijzvelden[$i]['label'], $dv);
		} else {
//			printf('<tr><td class="label">%1$s</td><td>%2$s</td><td>%3$s</td></tr>', $wijzvelden[$i]['label'], $dv, $inp);
			printf('<label>%1$s</label>%2$s', $wijzvelden[$i]['label'], $inp);
		}
		echo("\n");
	}
	echo("</div>  <!-- Einde wijzigengegevens -->\n");
//	echo("</table>\n");

	$c = "";
	$d = "";
	if ($_SESSION['AutoBewaren'] == 1) {
		$c = "checked";
		$d = "disabled";
	}
	if ($currenttab == "Ledenlijst") {
		echo("<div id='opdrachtknoppen'>\n");
		printf("<input type='submit' value='Bewaren' name='Bewaren' %s>\n", $d);
		echo("<input type='submit' value='Sluiten' name='Sluiten'>\n");
		if ($sl == "K" and $row->Verwijderd < "2010-01-01") {
			echo("<input type='submit' value='Verwijderen' name='Kloslid_Verwijderen'>\n");
		}	 elseif ($row->Verwijderd > "2010-01-01") {
			echo("<input type='submit' value='Verwijderen terugdraaien' name='Undo_Verwijderen'>\n");
		}
		printf("<input type='checkbox' name='AutoBewaren' value='1' %s OnChange='this.form.submit();'> Automatisch bewaren", $c);
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
	}

	echo("</form>\n");

} # algemeenlidmuteren

function nieuwepasfoto($lidid, $filterlid="") {
	global $actionurl;
	
	$max_size_attachm = 2 * 1024 * 1024;  // 2MB
	$min_size_attachm = 4 * 1024;  // 4KB
	
	if (isset($_POST['lidid']) and $_POST['lidid'] > 0) {
		$lidid = $_POST['lidid'];
	}
	$naamlid = (new cls_Lid())->Naam($lidid);
	
	if (isset($_FILES['UploadFoto']['name']) and strlen($_FILES['UploadFoto']['name']) > 3 and $lidid > 0) {
		$ad = $_SESSION['settings']['path_pasfoto'];
		if (file_exists($ad)) {
			chmod($ad, 0755);
		} elseif (strlen($ad) > 3) {
			if (mkdir($ad, 0755, true) == true) {
				$mess = sprintf("Directory '%s' is aangemaakt.", $ad);
				(new cls_Logboek())->add($mess, 6, 0, 0, 0, 20);
			}
		}
		if (file_exists($ad . "vervangen/")) {
			chmod($ad . "vervangen/", 0755);
		} elseif (file_exists($ad)) {
			if (mkdir($ad . "vervangen/", 0755, true) == true) {
				$mess = sprintf("Directory '%s' is aangemaakt.", $ad . "vervangen/");
				(new cls_Logboek())->add($mess, 6, 0, 0, 0, 20);
			}
		}

		$ext = explode(".", $_FILES['UploadFoto']['name']);
		$ext = strtolower($ext[count($ext) - 1]);
		if ($ext == "jpeg") {
			$ext = "jpg";
		}
		$target = $ad . sprintf("Pasfoto%d.%s", $lidid, $ext);
		$mess = "";
		if (in_array($ext, PASFOTOEXTENTIES) === false) {
			$mess = sprintf("Het bestand met extensie %s is niet toegestaan. De volgende extensies zijn toegestaan: %s.", $ext, implode(", ", PASFOTOEXTENTIES));
		} elseif ($_FILES['UploadFoto']['size'] > $max_size_attachm) {
			$mess = sprintf("De foto kan niet ge-upload worden, omdat het bestand groter dan %dKB is.", $max_size_attachm / 1024);
		} elseif ($_FILES['UploadFoto']['size'] < $min_size_attachm) {
			$mess = sprintf("De foto kan niet worden ge-upload, omdat het bestand kleiner dan %d bytes is.", $min_size_attachm);
		} elseif (file_exists($ad) == false) {
			$mess = "De folder om de pasfoto op te staan bestaat niet, neem hiervoor contact op met de websmaster.";
		} else {
			$of = fotolid($lidid, 0);
			while (strlen($of) > 3 and file_exists($of)) {
				$of_ext = substr(strrchr($of, "."), 1);
				$bu_name = "vervangen/" . sprintf("Pasfoto%d_%d.%s", $lidid, strftime("%Y%m%d", filectime($of)), $of_ext);
				if (rename($of, $ad . $bu_name)) {
					$mess = sprintf("Pasfoto './%s' is verplaatst en hernoemd naar './%s'.", $of, $bu_name);
					db_logboek("add", $mess, 6, $lidid);
					(new cls_Logboek())->add($mess, 6, $lidid, 0, 0, 21);
				}
				$of = fotolid($lidid, 0);
			}
			if (move_uploaded_file($_FILES['UploadFoto']["tmp_name"], $target)) {
				$image = new SimpleImage();
				$image->load($target);
				if ($image->GetWidth() > 390) {
					$image->resizeToWidth(390);
					$image->save($target);
				}
				if ($image->GetHeight() > 500) {
					$image->resizeToHeight(500);
					$image->save($target);
				}
				$image = null;
			}
			
			if ($lidid == $_SESSION['lidid'] and isValidMailAddress($_SESSION['settings']['zs_emailnieuwepasfoto'], 0) and (file_exists($target))) {
				$mail = new RBMmailer();
				$mail->From = $_SESSION['emailingelogde'];
				$mail->FromName = $naamlid;
				$mail->Subject = "Nieuwe pasfoto " . $naamlid;
				if (isValidMailAddress($_SESSION['settings']['zs_emailnieuwepasfoto'], 0) and $_SESSION['settings']['zs_emailnieuwepasfoto'] != $_SESSION['settings']['emailledenadministratie']) {
					$mail->AddAddress($_SESSION['settings']['zs_emailnieuwepasfoto']);
				}
				if (isValidMailAddress($_SESSION['settings']['emailledenadministratie'], 0)) {
					$mail->AddAddress($_SESSION['settings']['emailledenadministratie']);
				}
				$mail->AddCC($_SESSION['emailingelogde']);
				$body = sprintf("<p>Beste ledenadministratie,</p>
										<p>Bijgevoegd is mijn nieuwe pasfoto.</p>
										<p>Met vriendelijke groeten,<br>
										<strong>%s<strong></p>\n", $naamlid);
				$mail->MsgHTML($body);
				$mail->AddAttachment($target);
				if ($mail->Send()) {
					$mess = sprintf("%s heeft een nieuwe pasfoto ingestuurd.", $naamlid);
				} else {
					$mess = sprintf("Fout bij het e-mailen van de pasfoto: %s.", $mail->ErrorInfo);
				}
				$mail = null;
			} elseif (file_exists($target)) {
				$mess = "Er is een nieuwe pasfoto ge-upload.";
			} else {
				$mess = "Het is fout gegaan bij het uploaden van het bestand. Probeer het nog een keer of neem contact op met de webmaster.";
			}
		}
		(new cls_Logboek())->add($mess, 6, $lidid, 1, 0, 22);
	}

	echo("<div id='nieuwepasfoto'>\n");
	$fn = fotolid($lidid, 1);
	if (strlen($fn) > 4) {
		echo("<p>\n");
		printf("<img src='%s' alt='Huidige pasfoto %s'>\n", $fn, $naamlid);
		$fn = fotolid($lidid, 0);
		printf("Deze foto is voor het laatst op %s gewijzigd en is %dKB groot.", strftime("%e %B %Y", filectime($fn)), filesize($fn) / 1024);
		echo("</p>\n");
	} elseif ($lidid > 0) {
		printf("<p class='mededeling'>Geen huidige pasfoto van %s beschikbaar.</p>\n", $naamlid);
	}
			
	$actionurl = sprintf("%s?tp=%s&amp;lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	printf("<form method='post' action='%s' name='frm_pasfoto' enctype='multipart/form-data'>\n", $actionurl);
	printf("<input type='hidden' name='MAX_FILE_SIZE' value=%d>\n", $max_size_attachm);
	printf("<label for='UploadFoto'>Nieuwe pasfoto %s:&nbsp;</label>\n", $naamlid);
	printf("<input type='hidden' name='lidid' value=%d>\n", $lidid);
	echo("<input type='file' name='UploadFoto' id='UploadFoto'>&nbsp;");
	echo("<input type='submit' name='Upload' value='Insturen'>\n");
	printf("<p>Het ideale formaat van de pasfoto is 390 pixels breed bij 500 pixels hoog. Het bestand moet minimaal %d bytes groot zijn en mag niet groter dan %d KB zijn.</p>\n", $min_size_attachm, $max_size_attachm / 1024);
	if ($lidid == $_SESSION['lidid']) {
		echo("<p>Met het uploaden van deze pasfoto geef je toestemming om deze foto aan bezoekers van deze website te tonen.</p>");
	}
	echo("</form>\n");

	echo("</div>  <!-- Einde nieuwepasfoto -->\n");	
} # nieuwepasfoto

function toestemmingenmuteren($lidid) {
	
	$i_ond = new cls_Onderdeel();
	$i_lo = new cls_Lidond();
	
	if ($lidid != $_SESSION['lidid']) {
		$js = "OnChange='this.form.submit();'";
	} else {
		$js = "";
	}
	
	$actionurl = sprintf("%s?tp=%s&amp;lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		foreach ($i_ond->lijst(0, "`Type`='T'") as $ondrow) {
			if (isset($_POST['T_' . $ondrow->Kode])) {
				if ($i_lo->islid($lidid, $ondrow->RecordID) == false) {
					$i_lo->add($ondrow->RecordID, $lidid);
				}
			} else {
				$i_lo->delete(0, $lidid, $ondrow->RecordID);
			}
		}
	}
	
	echo("<div id='toestemmingenmuteren'>\n");
	printf("<h3>Toestemmingen %s muteren</h3>\n", (new cls_Lid())->Naam($lidid));
	printf("<form method='post' name='frmToestemming' action='%s'>\n", $actionurl);
	foreach ($i_ond->lijst(0, "`Type`='T'") as $row) {
		printf("<label>%s</label><p><input type='checkbox' name='T_%s' value='1' %s></p>\n", $row->Naam, $row->Kode, checked($i_lo->islid($lidid, $row->RecordID)));
	}
	echo("<div class='clear'></div>\n");
	if (strlen($_SESSION['settings']['uitleg_toestemmingen']) > 0) {
		printf("<p>%s</p>\n", $_SESSION['settings']['uitleg_toestemmingen']);
	}
	
	if ($lidid == $_SESSION['lidid']) {
		$tb = "Bevestigen";
	} else {
		$tb = "Bewaren";
	}
	
	echo("<div id='opdrachtknoppen'>\n");
	printf("<input class='knop' type='submit' value='%s' name='Bevestigen'>\n", $tb);
	echo("</div> <!-- Einde opdrachtknoppen -->\n");
	
	echo("</form>\n");
	echo("</div> <!-- Einde toestemmingenmuteren -->\n");
	
	$i_ond = null;
	$i_lo = null;
	
} # toestemmingenmuteren

function opzegginglidmaatschap($lidid) {
	global $actionurl;
	
	$fdlang = "%e %B %Y";
	
	$lid = (new cls_Lid())->record($lidid);
	$naamlid = $lid->NaamLid;
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST["OpzeggingPer"]) and strlen($_POST["OpzeggingPer"]) > 3) {
			$_POST["OpzeggingPer"] = change_month_to_uk($_POST["OpzeggingPer"]);
		} else {
			$_POST["OpzeggingPer"] = "";
		}
		if (strlen($_POST["OpzeggingPer"]) > 3 and strtotime($_POST["OpzeggingPer"]) !== FALSE) {
			$opgezegdper = change_month_to_uk($_POST["OpzeggingPer"]);
			if (strtotime($opgezegdper) < mktime(0, 0, 0, date("m")+$_SESSION['settings']['zs_opzegtermijn'], date("d"), date("Y"))) {
				$opgezegdper = strftime($fdlang, mktime(0, 0, 0, date("m")+$_SESSION['settings']['zs_opzegtermijn'], date("d"), date("Y")));
				$mess = sprintf("Er geldt een opzegtermijn van %d maand(en), hierdoor wordt de datum van opzegging %s.", $_SESSION['settings']['zs_opzegtermijn'], $opgezegdper);
				(new cls_Logboek())->add($mess, 6, $lidid, 1, 0, 8);
			} else {
				$opgezegdper = strftime($fdlang, strtotime($opgezegdper));
			}
		} else {
			$opgezegdper = strftime($fdlang, mktime(0, 0, 0, date("m")+$_SESSION['settings']['zs_opzegtermijn'], date("d"), date("Y")));
			printf("<p class='mededeling'>Er geldt een opzegtermijn van %d maand(en), hierdoor wordt de datum van opzeggen %s.</p>", $_SESSION['settings']['zs_opzegtermijn'], $opgezegdper);
		}
		if (isset($_POST['RedenOpmerking']) and strlen($_POST['RedenOpmerking']) > 1) {
			$opm = "\n<p>" . $_POST['RedenOpmerking'] . "</p>\n";
		} else {
			$opm = "";
		}
			
		$mess = sprintf("Het lidmaatschap is per %s opgezegd.", $opgezegdper);
		(new cls_Logboek())->add($mess, 6, $lidid, 0, 0, 8);
			
		$body = sprintf("<p>Beste ledenadministratie,</p>\n
		<p>Hierbij zeg ik mijn lidmaatschap van de %s per %s op. Mijn lidnummer is %d.</p>\n
		%s
		<p>Met vriendelijke groeten,<br>
		<strong>%s</strong></p>\n", $_SESSION['settings']['naamvereniging'], $opgezegdper, $_SESSION['lidnr'], $opm, $naamlid);
				
		$body .= sprintf("\n<p>Dit formulier is verzonden vanaf IP-adres %s.</p>\n", $_SERVER['REMOTE_ADDR']);
	
		$mail = new email();
		$mail->toevoegenadres($_SESSION['settings']['emailledenadministratie'], "aan", "Ledenadministratie");
		$mail->toevoegenlid($lidid, "cc");
		$mail->onderwerp = sprintf("Opzegging lidmaatschap %s per %s", $naamlid, $opgezegdper);
		$mail->bericht = $body;
		$mail->to_outbox();
		$mess = "Een e-mail is klaar gezet om deze opzegging te bevestigen.";
		(new cls_Logboek())->add($mess, 6, $lidid, 1, 0, 8);
		$mail = null;
		if ($_SESSION['settings']['zs_opzeggingautomatischverwerken'] == 1) {
			(new cls_lidmaatschap())->opzegging($lidid, $opgezegdper);
		}
		
	} else {
		$form_opzegging = "<fieldset>
<h3>Opzegging lidmaatschap</h3>
<p><label for='NaamLid'>Naam lid:</label><input type='text' name='NaamLid' value='[%NAAMLID%]' readonly='readonly'>
<label>Lidnummer:</label><input type='number' name='Lidnr' value='[%LIDNR%]' readonly='readonly'>
<label>Opzegging lidmaatschap per:</label><input type='date' name='OpzeggingPer' value='[%OPZEGGENVANAF%]'>
<textarea name='RedenOpmerking' rows=8 cols=48 title='Reden en/of opmerkingen'></textarea>
<input type='submit' name='VerstuurOpzegging' value='Verstuur opzegging'>
</fieldset>";
		$myFile = 'templates/opzegging.html';
		if (file_exists($myFile)) {
			$content = file_get_contents($myFile);
			$content = str_replace("[%FORMOPZEGGING%]", $form_opzegging, $content);
		} else {
			$content = $form_opzegging;
		}
	
		$content = str_replace("[%OPZEGGENVANAF%]", strftime($fdlang, mktime(0, 0, 0, date("m")+$_SESSION['settings']['zs_opzegtermijn'], date("d")+1, date("Y"))), $content);
		$content = str_replace("[%NAAMLID%]", $_SESSION['naamingelogde'], $content);
		$content = str_replace("[%LIDNR%]", $_SESSION['lidnr'], $content);
		$content = str_replace("[%NAAMVERENIGING%]", $_SESSION['settings']['naamvereniging'], $content);
		$content = str_replace("[%NAAMWEBSITE%]", $_SESSION['settings']['naamwebsite'], $content);
		$content = str_replace("[%URLWEBSITE%]", $_SERVER["HTTP_HOST"], $content);
		
		echo("<div id='opzegformulier'>\n");
		printf("<form method='post' name='Opzegging' action='%s'>\n", $actionurl);
		echo($content);
		echo("</form>\n");
		echo("</div>  <!-- Einde opzegformulier -->\n");
	}
} # opzegginglidmaatschap

function onderdelenlidmuteren($lidid, $p_type="G") {
	
	$actionurl = sprintf("%s?tp=%s&amp;lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	
	$i_lo = new cls_Lidond();
	$i_gr = new cls_Groep();
	
	$kf = "";
	$tf = "";
	if ($p_type == "A") {
		$typeoms = "Afdelingen";
		$kf = "<th>Functie</th><th>Functionele e-mail</th>";
		if ($i_gr->aantal() > 0) {
			$kf .= "<th>Groep</th>";
		}
		$tf = "A";
	} elseif ($p_type == "BCF") {
		$typeoms = "Bestuur, commissies en functionarissen";
		$kf = "<th>Functie</th><th>Functionele e-mail</th>";
		$tf = "L";
	} elseif ($p_type == "O") {
		$typeoms = "Onderscheidingen";
	} else {
		$typeoms = "Groepen";
	}
	
	if ($p_type == "BCF") {
		$f = "(O.Type IN ('B', 'C', 'F') OR O.Kader=1)";
	} elseif ($p_type == "G") {
		$f = sprintf("O.Type='G' AND O.Kader=0");
	} else {
		$f = sprintf("O.Type='%s'", $p_type);
	}

	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['Nieuw']) and $_POST['Nieuw'] > 0) {
			$i_lo->add($_POST['Nieuw'], $lidid);
		}
		foreach ($i_lo->editlijst($f, $lidid) as $row) {
			$vn = sprintf("Vanaf_%d", $row->RecordID);
			if (isset($_POST[$vn]) and strlen($_POST[$vn]) >= 10 and $_POST[$vn] != $row->Vanaf) {
				$i_lo->update($row->RecordID, "Vanaf", $_POST[$vn]);
			}
			$vn = sprintf("Functie_%d", $row->RecordID);
			if (isset($_POST[$vn])) {
				$i_lo->update($row->RecordID, "Functie", $_POST[$vn]);
			}
			$vn = sprintf("Email_%d", $row->RecordID);
			if (isset($_POST[$vn])) {
				$i_lo->update($row->RecordID, "EmailFunctie", $_POST[$vn]);
			}
			$vn = sprintf("GroepID_%d", $row->RecordID);
			if (isset($_POST[$vn])) {
				$i_lo->update($row->RecordID, "GroepID", $_POST[$vn]);
			}
			$vn = sprintf("Opmerking_%d", $row->RecordID);
			if (isset($_POST[$vn])) {
				$i_lo->update($row->RecordID, "OPMERK", $_POST[$vn]);
			}
			$vn = sprintf("Opgezegd_%d", $row->RecordID);
			if (isset($_POST[$vn])) {
				$i_lo->update($row->RecordID, "Opgezegd", $_POST[$vn]);
			}
		}
	}

	echo("<div id='onderdelenperlidmuteren'>\n");
	printf("<form method='post' name='frmOnderdelenPerLidMuteren' action='%s'>\n", $actionurl);
	echo("<table>\n");
	printf("<caption>%s %s muteren</caption>\n", $typeoms, (new cls_Lid())->Naam($lidid));
	$frows = (new cls_functie())->selectlijst($tf);
	if ($p_type == "A") {
		printf("<tr><th>Afdeling</th><th>Vanaf</th>%s<th>Opmerking</th><th>Tot en met</th></tr>\n", $kf);
	} elseif ($p_type == "G") {
		printf("<tr><th>Groep</th><th>Vanaf</th>%s<th>Opmerking</th><th>Tot en met</th></tr>\n", $kf);
	} else {
		printf("<tr><th>Onderdeel</th><th>Vanaf</th>%s<th>Opmerking</th><th>Tot en met</th></tr>\n", $kf);
	}
	foreach ($i_lo->editlijst($f, $lidid) as $row) {
		printf("<tr>\n<td>%s - %s</td>\n", $row->Kode, $row->Naam);
		printf("<td><input type='date' name='Vanaf_%d' value='%s' OnBlur='this.form.submit();'></td>\n", $row->RecordID, $row->Vanaf);
		if (strlen($kf) > 0) {
			$options = "";
			foreach ($frows as $frow) {
				$options .= sprintf("<option value=%d %s>%s</option>\n", $frow->Nummer, checked($frow->Nummer, "option", $row->Functie), $frow->Omschrijv);
			}
			printf("<td><select name='Functie_%d' OnChange='this.form.submit();'>%s</select></td>\n", $row->RecordID, $options);
			printf("<td><input type='email' name='Email_%d' value='%s' OnChange='this.form.submit();'></td>\n", $row->RecordID, $row->EmailFunctie);
		}
		if ($p_type == "A" and $i_gr->aantal() > 0) {
			$f2 = sprintf("OnderdeelID=%d", $row->OnderdeelID);
			if ($i_gr->aantal($f2) > 0) {
				$options = "<option value=0></option>\n";
				foreach ((new cls_Groep())->selectlijst($row->OnderdeelID) as $grow) {
					$options .= sprintf("<option value=%d %s>%s</option>\n", $grow->RecordID, checked($grow->RecordID, "option", $row->GroepID), $grow->Kode);
				}
				printf("<td><select name='GroepID_%d' OnChange='this.form.submit();'>%s</select></td>\n", $row->RecordID, $options);
			} else {
				echo("<td></td>\n");
			}
		}
		printf("<td><input type='text' name='Opmerking_%d' value='%s' OnBlur='this.form.submit();'></td>\n", $row->RecordID, $row->OPMERK);
		printf("<td><input type='date' name='Opgezegd_%d' value='%s' OnBlur='this.form.submit();'></td>\n", $row->RecordID, $row->Opgezegd);
		echo("</tr>\n");
	}
	
	$options = "<option value=0>Nieuw ....</option>\n";
	$f .= " AND IFNULL(O.VervallenPer, CURDATE()) >= CURDATE() AND O.GekoppeldAanQuery=0 AND LENGTH(IFNULL(O.MySQL, '')) < 10";
	foreach((new cls_Onderdeel())->lijst(0, $f) as $row) {
		$options .= sprintf("<option value=%d>%s - %s</option>\n", $row->RecordID, $row->Kode, $row->Naam);
	}
	printf("<tr><td><select name='Nieuw' OnChange='this.form.submit();'>%s</select>\n</td></tr>\n", $options);
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>\n <!-- Einde onderdelenperlidmuteren -->\n");
	$i_lo = null;

}  # onderdelenlidmuteren

function diplomaslidmuteren($lidid, $td, $eenv=1) {
	global $actionurl;

	if ($td == "ZS") {
		$actie = "zelfservice_lijst";
		$lidid = $_SESSION['lidid'];
	} else {
		$actie = "muteerlijst";
	}
	
	if (!isset($_SESSION['dipltextfilter'])) {
		$_SESSION['dipltextfilter'] = "";
	}
	
	if (!isset($_SESSION['orgfilter'])) {
		$_SESSION['orgfilter'] = -1;
	}
	
	$i_dp = new cls_Diploma();
	$i_ld = new cls_Liddipl();
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		foreach ($_POST as $k => $v) {
			if (startwith($k, "Nieuw_") and $_POST[$k] > "1900-01-01") {
				$dpid = intval(str_replace("Nieuw_", "", $k));
				$i_ld->add($lidid, $dpid, $_POST[$k]);
			} elseif ($k == "Extra" and $_POST[$k] > 0) {
				$i_ld->add($lidid, $_POST[$k], date("Y-m-d"));
			}
		}
		
		$omz["DatumBehaald"] = "Behaald";
		$omz["LicentieVervallenPer"] = "VervaltPer";
		$omz["Diplomanummer"] = "Diplnr";
		
		$f = sprintf("Lid=%d", $lidid);
		foreach ($i_ld->basislijst($f) as $ld) {
			$contr_name = "Behaald_" . $ld->RecordID;
			if (isset($_POST[$contr_name]) and strlen($_POST[$contr_name]) == 0) {
				$i_ld->delete($ld->RecordID);
			} else {
				foreach ($omz as $kolomnaam => $bascn) {
					$contr_name = $bascn . "_" . $ld->RecordID;
					if (isset($_POST[$contr_name])) {
						$i_ld->update($ld->RecordID, $kolomnaam, $_POST[$contr_name]);
					}
				}
			}
		}
		
		if (isset($_POST['orgfilter'])) {
			$_SESSION['orgfilter'] = intval($_POST['orgfilter']);
		}
	}
			
	echo("<div id='wijzigendiplomas'>\n");
	
//	echo("<p class='mededeling'>Werk in uitvoering, werkt dus niet zoals het zou moeten.</p>\n");
	$actionurl = sprintf("%s?tp=%s&amp;lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	printf("<form method='post' action='%s' name='frm_wijzigendiplomas'>\n", $actionurl);
	
	$f = "";
	if ($td == "ZS") {
		$f = "Zelfservice=1";
	} else {
		echo("<div id='filter'>\n");
		echo("<label>Code/naam bevat</label><input id='tbFilterCodeNaam' OnKeyUp='fnFilterDiplomaLid();'>");
		printf("<label>Uitgegeven door: </label><select name='orgfilter' OnChange='this.form.submit();'>\n<option value=-1>Geen filter</option>\n%s</select>\n", (new cls_Organisatie())->htmloptions(1, $_SESSION['orgfilter']));
		echo("</div> <!-- Einde filter -->\n");
		
		if (strlen($_SESSION['dipltextfilter']) > 0) {
			$f = sprintf("(Kode LIKE '%%%1\$s%%' OR Naam LIKE '%%%1\$s%%')", $_SESSION['dipltextfilter']);
			if ($_SESSION['orgfilter'] >= 0) {
				$f .= sprintf("AND ORGANIS=%d", $_SESSION['orgfilter']);
			}
		} elseif ($_SESSION['orgfilter'] >= 0) {
			$f = sprintf("ORGANIS=%d", $_SESSION['orgfilter']);
		}
	}
	
	echo("<table id='diplomaslidmuteren'>\n");
	printf("<caption>Diploma's %s muteren</caption>\n", (new cls_Lid())->Naam($lidid));
	printf("<tr><th>Code</th><th>Naam</th><th>Geldigheid</th><th>Behaald op</th><th>Diplomanummer</th><th>Geldig tot</th></tr>\n");
	foreach ($i_dp->basislijst($f, "kode") as $dp) {
		$orgcode = (new cls_Organisatie())->naam($dp->ORGANIS);
		if (strlen($orgcode) > 4) {
			$dpnm = $orgcode . " - " . $dp->Naam;
		} elseif (strlen($orgcode) > 1) {
			$dpnm = $orgcode . " " . $dp->Naam;
		} else {
			$dpnm = $dp->Naam;
		}
		if ($dp->GELDIGH > 0) {
			$geldig = sprintf("%d maanden", $dp->GELDIGH);
		} else {
			$geldig = "";
		}
		printf("<tr>\n<td>%s</td>\n<td>%s</td>\n<td>%s</td>\n", $dp->Kode, $dpnm, $geldig);
		
		$f = sprintf("Lid=%d AND DiplomaID=%d", $lidid, $dp->RecordID);
		$aant = 0;
		foreach ($i_ld->basislijst($f) as $ld) {
			if ($aant > 0) {
				echo("</tr>\n<tr><td></td><td></td><td></td>");
			}
			printf("<td><input type='date' name='Behaald_%d' value='%s' OnBlur='this.form.submit();'></td>\n", $ld->RecordID, $ld->DatumBehaald);
			printf("<td><input type='text' name='Diplnr_%d' value='%s' maxlength=30 OnBlur='this.form.submit();'></td>\n", $ld->RecordID, $ld->Diplomanummer);
			printf("<td><input type='date' name='VervaltPer_%d' value='%s' OnBlur='this.form.submit();'></td>\n", $ld->RecordID, $ld->LicentieVervallenPer);
			$aant++;
		}
		if ($aant == 0) {
			printf("<td><input type='date' name='Nieuw_%d' OnBlur='this.form.submit();'></td><td></td><td></td>\n", $dp->RecordID);
		}
		echo("</tr>\n");
	}
	if ($td != "ZS") {
		printf("<tr><td colspan=2><select name='Extra' OnChange-='this.form.submit();'><option value=-1>Toevoegen ...</option>%s</select></td></tr>\n", $i_dp->htmloptions(0, $_SESSION['orgfilter']));
	}
	
	echo("</table>\n");
	
//	echo("<div id='opdrachtknoppen'><input type='submit' value='Bewaren' name='diplwijz'></div>\n");
	
	echo("</form>\n");
	echo("</div>  <!-- Einde wijzigendiplomas -->\n");
	
	$i_dp = null;
	$i_ld = null;
	
} # diplomaslidmuteren

function lidmaatschapmuteren($lidid) {
	
	$i_lm = new cls_Lidmaatschap();
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$omz['Lidnr'] = "Lidnr";
		$omz['LIDDATUM'] = "Vanaf";
		$omz['Opgezegd'] = "Opgezegd";
		$f = sprintf("Lid=%d", $lidid);
		foreach ($i_lm->basislijst($f, "LIDDATUM") as $row) {
			foreach($omz as $kolomnaam => $bascn) {
				$contr_name = sprintf("%s_%d", $bascn, $row->RecordID);
				if (isset($_POST[$contr_name])) {
					$i_lm->update($row->RecordID, $kolomnaam, $_POST[$contr_name]);
				}
			}
			
			$nm = sprintf("BO_%d_x", $row->Lidnr);
			if (isset($_POST[$nm]) and $_POST[$opgezegd] > "2010-01-01" and $_SESSION['settings']['mailing_bevestigingopzegging'] > 0) {
				$mailing = new Mailing($_SESSION['settings']['mailing_bevestigingopzegging']);
				$mailing->xtrachar = "BO_LM";
				$mailing->xtranum = $row->Lidnr;
				$mailing->send($row->Lid);
				$mailing = null;
			}
		}
			
		if (isset($_POST['NieuwLidmaatschap'])) {
			$i_lm->add($lidid);
		}
	}
	
	echo("<div id='lidmaatschapmuteren'>\n");
	$actionurl = sprintf("%s?tp=%s&amp;lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	printf("<form method='post' action='%s'>\n", $actionurl);
	echo("<table>\n");
	printf("<caption>Lidmaatschap %s muteren</caption>\n", (new cls_Lid())->Naam($lidid));
	echo("<tr><th>RecordID</th><th>Lidnummer</th><th>Lid vanaf</th><th>Opgezegd per</th><th></th><tr>\n");
	$toev = true;
	$aant = 0;
	$f = sprintf("Lid=%d AND LIDDATUM <= IFNULL(Opgezegd, CURDATE())", $lidid);
	foreach ($i_lm->basislijst($f, "LIDDATUM") as $row) {
		echo("<tr>\n");
		printf("<td>%d</td>\n", $row->RecordID);
		printf("<td><input type='number' name='Lidnr_%d' value=%d onchange='this.form.submit();'></td>\n", $row->RecordID, $row->Lidnr);
		printf("<td><input type='date' name='Vanaf_%d' value='%s' onchange='this.form.submit();'></td>\n", $row->RecordID, $row->LIDDATUM);
		if ($row->Opgezegd >= "1901-01-01") {
			if ($row->Opgezegd >= date("Y-m-d")) {
				$toev = false;
			} elseif ($row->LIDDATUM >= date("Y-m-d")) {
				$toev = false;
			}
		} else {
			$toev = false;
		}
		printf("<td><input type='date' name='Opgezegd_%d' value='%s' onchange='this.form.submit();'></td>\n", $row->RecordID, $row->Opgezegd);
		if ($row->Opgezegd >= date("Y-m-d") and $_SESSION['settings']['mailing_bevestigingopzegging'] > 0) {
			printf("<td><input type='image' name='BO_%d' src='images/email.png' alt='Verstuur bevestiging' title='Verstuur bevestiging'></td>\n", $row->Lidnr);
		} else {
			echo("<td></td>\n");
		}
		echo("</tr>\n");
		$aant++;
	}
	
	if ($toev) {
		echo("<tr><td><button name='NieuwLidmaatschap' value=1>Nieuw lidmaatschap</button></td></tr>\n");
	}
	
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde lidmaatschapmuteren -->\n");
	$i_lm = null;
} # lidmaatschapmuteren

function instellingenledenmuteren() {
	global $currenttab, $currenttab2;
	
	$i_p = new cls_Parameter();
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		foreach (array("meisjesnaamtonen", "woonadres_anderen_tonen", "zs_incl_beroep", "zs_incl_bsn", "zs_incl_emailouders", "zs_incl_emailvereniging", "zs_incl_iban", "zs_incl_legitimatie", "zs_incl_slid", "zs_opzeggingautomatischverwerken") as $c) {
			if (isset($_POST[$c])) {
				$_POST[$c] = 1;
			} else {
				$_POST[$c] = 0;
			}
		}
		
		foreach ($i_p->lijst() as $row) {
			if (isset($_POST[$row->Naam])) {
				$i_p->update($row->Naam, $_POST[$row->Naam]);
			}
		}
	}	
	$i_p->vulsessie();
	
	echo("<div id='instellingenmuteren'>\n");
	printf("<form method='post' action='%s?tp=%s/%s'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
	
	echo("<h2>Algemeen</h2>\n");
	printf("<label>Moet de naam van een lid de meisjesnaam bevatten?</label><input type='checkbox' name='meisjesnaamtonen' OnChange='this.form.submit();' %s>\n", checked($_SESSION['settings']['meisjesnaamtonen']));
	printf("<label>Moet het woonadres van een lid ook aan andere leden worden getoond?</label><input type='checkbox' name='woonadres_anderen_tonen' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['woonadres_anderen_tonen']));
	printf("<label>Opzegtermijn van de vereniging in maanden</label><input type='number' name='zs_opzegtermijn' value=%d OnChange='this.form.submit();'>\n", $_SESSION['settings']['zs_opzegtermijn']);
	
	$options = "";
	foreach((new cls_Mailing())->lijst("Templates") as $row) {
		$options .= sprintf("<option%s value=%d>%s</option>", checked($row->lnkMailingID, "option", $_SESSION['settings']['mailingbijadreswijziging']), $row->lnkMailingID, $row->Onderwerp);
	}
	printf("<label>Mailing voor versturen e-mail naar ledenadministratie bij wijzigen postcode</label><select name='mailingbijadreswijziging' OnChange='this.form.submit();'>\n<Option value=0>Geen</option>%s</select>\n", $options);
	
	printf("<label>Moet een opzegging door een lid automatisch worden verwerkt?</label><input type='checkbox' name='zs_opzeggingautomatischverwerken' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_opzeggingautomatischverwerken']));	
	
	echo("<h2>Beschikbaar in de zelfservice</h2>\n");
	printf("<label>Beroep?</label><input type='checkbox' name='zs_incl_beroep' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_beroep']));
	printf("<label>Burgerservicenummer (BSN)?</label><input type='checkbox' name='zs_incl_bsn' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_bsn']));
	printf("<label>E-mail ouders?</label><input type='checkbox' name='zs_incl_emailouders' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_emailouders']));
	printf("<label>E-mail vereniging?</label><input type='checkbox' name='zs_incl_emailvereniging' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_emailvereniging']));
	printf("<label>Bankrekening / IBAN?</label><input type='checkbox' name='zs_incl_iban' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_iban']));
	printf("<label>Legitimatie?</label><input type='checkbox' name='zs_incl_legitimatie' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_legitimatie']));
	printf("<label>Sportlink ID?</label><input type='checkbox' name='zs_incl_slid' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_slid']));
	echo("<h2>Overig in de zelfservice</h2>\n");
	printf("<label>Welke soorten memo's mogen leden zelf muteren?</label><input type='text' name='zs_muteerbarememos' value=\"%s\" OnChange='this.form.submit();'><p>(Scheiden met een komma)</p>\n", $_SESSION['settings']['zs_muteerbarememos']);
	printf("<label>Welke tekst moet er als uitleg bij de toestemmingen worden vermeld?</label><textarea name='uitleg_toestemmingen' rows=2 cols=68 OnChange='this.form.submit();'>%s</textarea>\n", $_SESSION['settings']['uitleg_toestemmingen']);
	
	echo("</form>\n");
	echo("</div> <!-- Einde instellingenmuteren -->\n");
	
}  # instellingenledenmuteren

function fnEigenlijstenmuteren() {
	
	$i_el = new cls_Eigen_lijst();
	if (isset($_GET['paramID']) and $_GET['paramID'] > 0) {
		$elid = intval($_GET['paramID']);
		$i_el->elid = $elid;
	} else {
		$elid = 0;
	}
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		if (isset($_POST['Toevoegen']) and $_POST['Toevoegen'] == "lijst_toevoegen") {
			$i_el->add();
		}
		
		if (isset($_POST['naam'])) {
			$i_el->update($elid, "Naam", $_POST['naam']);
		}
		
		if (isset($_POST['mysql'])) {
			if ($i_el->controleersql($_POST['mysql']) == false) {
				$i_el->update($elid, "AantalRecords", -1);
			} else {
				$result = $i_el->execsql($_POST['mysql']);
				$rows = $result->fetchAll();
				$i_el->update($elid, "AantalRecords", count($rows));
			}
			$i_el->update($elid, "MySQL", $_POST['mysql']);
		}
		
	} elseif (isset($_GET['paramActie']) and $_GET['paramActie'] == 2 and $elid > 0) {
		
		$row = $i_el->record();
		
		echo("<div id='formulier'>\n");
		printf("<form method='post' action='%s?tp=%s&paramID=%d'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $elid);
		
		printf("<label>Naam eigen lijst</label><input type='text' name='naam' value='%s'>", $row->Naam);
		printf("<label>MySQL-code</label><textarea id='mysql' name='mysql' rows=10 cols=100>%s</textarea>\n", $row->MySQL);
		echo("<label>Aantal records</label>");
		if ($row->AantalRecords >= 0) {
			printf("<p>%d</p>\n", $row->AantalRecords);
		} else {
			echo("<p>De MySQL-code is niet uitvoerbaar</p>\n");
		}
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<input type='submit' value='Bewaren' name='Bewaren'>\n");
		echo("</div>\n");
		
		echo("</form>\n");
		echo("</div> <!-- Einde formulier -->\n");
		
	}
		
	$rows = $i_el->lijst();
	if (count($rows) > 0) {
		echo("<table>\n");
		echo("<caption>Overzicht Eigen lijsten</caption>\n");
		foreach($rows as $row) {
			printf("<tr><td><a href='%s?tp=%s&paramID=%d&paramActie=2'><img src='%s'></a></td><td>%s</td><td>%s</td></tr>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID, BASE64_MUTEER, $row->Naam, $row->MySQL);
		}
		echo("</table>\n");
	}
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<div id='opdrachtknoppen'>\n");
	echo("<button type='submit' name='Toevoegen' value='lijst_toevoegen'>Lijst toevoegen</button>\n");
	echo("</div>\n");
	echo("</form\n");
	
}  # fnEigenlijstenmuteren

function IsIBANgoed($IBAN, $LeegIsGoed=1) {
   $retval = false;
   
   if (strlen($IBAN) == 0 and $LeegIsGoed == 1) {
		$retval = true;
      
   } elseif (strlen($IBAN) == 18) {
      $IBAN = strtoupper($IBAN);
      $controle = intval(substr($IBAN, 2, 2));
      $IBAN = substr($IBAN, 4, 14) . substr($IBAN, 0, 2) . "00";
      $totaal = "";
      for ($teller=0; $teller < strlen($IBAN); $teller++) {
         if (ord(substr($IBAN, $teller, 1)) < 65) {
            $totaal = $totaal . substr($IBAN, $teller, 1);
         } else {
            $totaal = $totaal . strval(ord(substr($IBAN, $teller, 1)) - 55);
         }
      } # for
      $totaal = 98 - (int)bcmod($totaal, 97);
      if ($controle == $totaal) {
         $retval = true;
      }
   }
   
   return $retval;

} # IsIBANgoed

function MailVervallenDiplomas() {
	
	// Deze functie is nog niet klaar en wordt nog nergens aangeroepen.
	
	$termijnvervallendiplomasmailen = $_SESSION['settings']['termijnvervallendiplomasmailen'];

	if ((date('H') == 8 or ($_SESSION['webmaster'] == 1 and date('H') > 7)) and $termijnvervallendiplomasmailen > 0) {
		$ed = date('Y-m-d', mktime(0, 0, 0, date("m")+$termijnvervallendiplomasmailen, date("d"), date("Y")));
		foreach((new cls_Login())->lijst("Login.HerinneringVervallenDiplomas=1") as $lid) {
			$query = sprintf("SELECT IFNULL(MAX(LEFT(DATE_ADD(send_on, INTERVAL %d MONTH), 10)), '2012-01-01') FROM %sMailing_hist WHERE Xtra_Char='H_DP' AND LidID=%d;", $termijnvervallendiplomasmailen, TABLE_PREFIX, $lid->lnkNummer);
			$bd = db_scalar($query);
			if ($bd < date('Y-m-d')) {
				$bd = date('Y-m-d');
			}
			
			$herdipl = "<tr><th>Code</th><th>Naam diploma</th><th>Behaald op</th><th>Vervalt op</th></tr>\n";
			foreach(db_liddipl("lidgegevens", $lid->lnkNummer) as $ld) {
				$vp = "dteVervalt per";
				if ($ld->$vp >= $bd and $ld->$vp < $ed) {
					$herdipl .= sprintf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", $ld->Code, $ld->Diploma, strftime('%e %B %Y', strtotime($ld->dteDatum)), strftime('%e %B %Y', strtotime($ld->$vp)));
				}
			}
			print("<table border=1>\n" . $herdipl . "</table>\n");
		}
	}
} # MailVervallenDiplomas

?>
