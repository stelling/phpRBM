<?php

function fnWijzigen($lidid, $actie="") {
	global $currenttab, $currenttab2, $currenttab3, $actionurl;
	
	if ($currenttab == "Zelfservice") {
		$xtra_param = "";
		$lidid = $_SESSION['lidid'];
		$actionurl = sprintf("%s?tp=%s", $_SERVER['PHP_SELF'], $_GET['tp']);
		
	} else {
		$xtra_param = sprintf("lidid=%d", $lidid);
		$actionurl = sprintf("%s?tp=%s&lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	}
	
	if ($lidid > 0) {
		$naamlid = (new cls_Lid())->Naam($lidid);
		fnDispMenu(2, $xtra_param);
		if ($currenttab != "Zelfservice") {
			fnDispMenu(3, $xtra_param);
		}

		if ($actie == "Algemene gegevens") {
			algemeenlidmuteren($lidid);
			
		} elseif ($currenttab3 == "Afdelingen") {
			onderdelenlidmuteren($lidid, "A");
			
		} elseif ($currenttab3 == "B, C en F") {
			onderdelenlidmuteren($lidid, "BCF");
			
		} elseif ($currenttab3 == "Groepen") {
			onderdelenlidmuteren($lidid, "G");
			
		} elseif ($currenttab3 == "Onderscheidingen") {
			onderdelenlidmuteren($lidid, "O");
			
		} elseif ($actie == "Diplomas" or $actie == "Diploma's") {
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
				<label>Herhaal wachtwoord:</label><input type='password' name='pw_herhaal' maxlength=12>", $_SESSION['username']);
				printf("</fieldset>
				%s
				<div id='opdrachtknoppen'>\n
				<input type='submit' value='Wijzigen'>\n
				</div> <!-- Einde opdrachtknoppen -->\n
				</form>\n
				</div>  <!-- Einde profielwijzigen -->\n", fneisenwachtwoord());
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
	
	$txt = "<div id='wieiswie'>\n";
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
					$txt .= "<div class='clear'></div>\n";
				}
				if (isValidMailAddress($row->CentraalEmail, 0)) {
					$txt .= sprintf('<h2 class="onderdeelnaam">%s </h2><h2 class="onderdeelemail">%s</h2>', $row->OndNaam, fnDispEmail($row->CentraalEmail, $row->OndNaam, 1));
				} else {
					$txt .= sprintf("<h2 class='onderdeelnaam'>%s</h2>\n", $row->OndNaam);
				}
				$txt .= "<div class='clear'></div>\n";
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
			
			$fd = fotolid($row->Lid, 1);
			if ($actie == "Onderscheidingen" and strlen($fd) > 3) {
				$txt .= sprintf("<div class='kaartje'><img src='%1\$s' alt='Pasfoto %2\$s'><p class='naamkaderlid'>%2\$s</p>\n<p>vanaf %3\$s</p>\n</div>\n", $fd, $ln, strftime("%B %Y", strtotime($row->Vanaf)));
			} elseif ($actie == "Onderscheidingen") {
				$txt .= sprintf("<div class='kaartje'><p class='naamkaderlid'>%s</p>\n<p>vanaf %s</p>\n</div>\n", $ln, strftime("%B %Y", strtotime($row->Vanaf)));
			} elseif (strlen($fd) > 3) {
				$txt .= sprintf("<div class='kaartje'><img src='%1\$s' alt='Pasfoto %2\$s'><p class='naamkaderlid'>%2\$s</p>\n<p class='functiekaderlid'>%3\$s</p>\n<p class='mailkaderlid'>%4\$s</p>\n</div>\n", $fd, $ln, $func, $email);
			} else {
				$txt .= sprintf("<div class='kaartje'><p class='naamkaderlid'>%s</p>\n<p class='functiekaderlid'>%s</p>\n<p class='mailkaderlid'>%s</p>\n</div>\n", $ln, $func, $email);	
			}
		}
	} else {	
		$txt .= "<table>\n";
		foreach ($lijst as $row) {
			if ($vo != $row->OndNaam) {
				$txt .= sprintf("<th colspan=5>%s</th>\n", $row->OndNaam);
				if (isValidMailAddress($row->CentraalEmail, 0)) {
					$txt .= sprintf("<tr>\n<td colspan=2><strong>Centraal e-mailadres</strong></td>\n<td>%s</td>\n<td><strong>Vanaf</strong></td></tr>\n", fnDispEmail($row->CentraalEmail, $row->OndNaam, 0, 1));
					$txt .= "<tr><td colspan=5>&nbsp</td></tr>\n";
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
			$txt .= sprintf("<tr>\n<td>%s</td>\n<td>%s</td>\n<td>%s</td>\n<td>%s</td>\n</tr>\n", $ln, $func, $email, strftime("%B %Y", strtotime($row->Vanaf)));
		}
		$txt .= "</table>\n";
	}
	$txt .= "</div>  <!-- Einde kaderoverzicht -->\n";
	
	return $txt;
	
} # fnWieiswie

function fnLedenlijst() {
	global $wherelidond, $currenttab2, $currenttab3;
	
	$i_lid = new cls_Lid();
	(new cls_lidond())->auto_einde();
	
	fnDispMenu(2);
	
	$_SESSION['val_groep'] = $_SESSION['val_groep'] ?? 0;
	if (isset($_POST['lbGroepFilter'])) {
		$_SESSION['val_groep'] = $_POST['lbGroepFilter'];
	}
	
	if ($currenttab2 == "Nieuw (klos)lid") {
		if ($_SERVER['REQUEST_METHOD'] == "POST") {
			if (isset($_POST['addkloslid']) and isset($_POST['achternaam']) and strlen($_POST['achternaam']) > 1) {
				$lidid = $i_lid->add($_POST['achternaam'], $_POST['geslacht'], $_POST['postcode']);
				if (isset($_POST['lidvanaf']) and $_POST['lidvanaf'] >= "1950-01-01" and strtotime($_POST['lidvanaf']) !== false) {
					(new cls_Lidmaatschap())->add($lidid, $_POST['lidvanaf']);
				}
			}
			$tp = sprintf("Ledenlijst/Wijzigen+lid/Algemene+gegevens&lidid=%d", $lidid);
			printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $tp);
			
		} else {
			
			echo("<div id='nieuwlid'>\n");
			printf("<form name='frmNieuwLid' action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
			echo("<label>Achternaam</label><input type='text' name='achternaam' maxlength=40><p>(zonder tussenvoegsels)</p>\n");
						
			$opt = "\n";
			foreach (constant("ARRGESLACHT") as $key => $val) {
				$opt .= sprintf("<option value='%s'>%s</option>\n", $key, $val);
			}
			printf("<label>Geslacht</label><select name='geslacht'>%s</select>", $opt);
			
			echo("<label>Postcode</label><input type='text' class='postcode' name='postcode' maxlength=7>\n");
			
			if (toegang("Ledenlijst/Wijzigen lid/Lidmaatschap", 0, 0) == true) {
				echo("<label>Lid vanaf</label><input type='date' name='lidvanaf'>\n");
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
			echo("<input id='tbOmsFilter' OnKeyUp=\"fnFilter('logboek', this);\">");
			printf("<p class='aantrecords'>%d rijen</p>\n", count($rows));
			echo("</div> <!-- Einde filter -->\n");
			echo(fnDisplayTable($rows, 'logboek', "Mutaties in lidgegevens", 0, "", "logboek"));
		} else {
			echo("<p>Er zijn geen mutaties bekend.</p>\n");
		}
		
	} else {
		
		if (toegang("Ledenlijst/Overzicht lid", 0, 0)) {
			$kols[0]['link'] = "<a href='index.php?tp=Ledenlijst/Overzicht+lid&lidid=%d'>%s</a>";
			$kols[0]['class'] = "details";
			$kols[0]['columnname'] = "RecordID";
			$kols[1]['headertext'] = "Naam";
			$kols[1]['sortcolumn'] = "L.Achternaam";
			$kols[2]['headertext'] = "Adres";
			$kols[2]['columnname'] = "Adres";
			$kols[3]['headertext'] = "Bereiken";
			$kols[3]['columnname'] = "Bereiken";
			$kols[3]['type'] = "combitext";
			$kn = 4;
			if ($currenttab2 == "Klosleden") {
				$kols[$kn]['headertext'] = "Opmerking";
				$kn++;
				$kols[$kn]['headertext'] = "Ond.";
				$kn++;
			} else {
				$kols[$kn]['headertext'] = "Lidnummer";
				$kols[$kn]['columnname'] = "Lidnr";
				$kols[$kn]['sortcolumn'] = "LM.Lidnr";
				$kols[$kn]['type'] = "integer";
				$kn++;
				$kols[$kn]['headertext'] = "Lid vanaf";
				$kols[$kn]['columnname'] = "LIDDATUM";
				$kols[$kn]['sortcolumn'] = "LM.LIDDATUM";
				$kols[$kn]['type'] = "date";
				$kn++;
				if ($currenttab2 != "Toekomstige leden") {
					$kols[$kn]['headertext'] = "Opgezegd per";
					$kols[$kn]['columnname'] = "Opgezegd";
					$kols[$kn]['sortcolumn'] = "LM.Opgezegd";
					$kols[$kn]['type'] = "date";
					$kn++;
				}
				$kols[$kn]['headertext'] = "Afd.";
				$kols[$kn]['columnname'] = "Afdelingen";
				$kn++;
			}
		}
		if (toegang("Ledenlijst/Wijzigen lid", 0, 0)) {
			$kols[$kn]['link'] = "<a href='index.php?tp=Ledenlijst/Wijzigen+lid/Algemene+gegevens&lidid=%d'>&nbsp;&nbsp;&nbsp;></a>";
			$kols[$kn]['columnname'] = "RecordID";
			$kols[$kn]['class'] = 'muteren';
			$kn++;
		}
	
		$ord = fnOrderBy($kols);
		
		if ($currenttab2 == "Klosleden") {
				$rows = $i_lid->klosledenlijst();
		
		} else {
			$sl = 0;
			if ($currenttab2 == "Leden") {
				$sl = 1;
			} elseif ($currenttab2 == "Voormalig leden") {
				$sl = 3;
			} elseif ($currenttab2 == "Toekomstige leden") {
				$sl = 2;
			}
			if ($sl > 0) {
				$rows = $i_lid->ledenlijst($sl, $_SESSION['val_groep'], $ord);
			}
		}
		
		echo("<div id='filter'>\n");
		echo("<input type='text' name='tbTekstFilter' id='tbTekstFilter' placeholder='Tekstfilter' OnKeyUp=\"fnFilter('ledenlijst', this);\">\n");
		printf("<form action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);

		if ($currenttab2 != "Klosleden") {
			printf("<select name='lbGroepFilter' onchange='this.form.submit();'>\n<option value=0>Filter op onderdeel</option>\n%s</select>\n", (new cls_Onderdeel())->htmloptions($_SESSION['val_groep']));
		}

		if (count($rows) > 1) {
			printf("<p class='aantrecords'>%d %s</p>\n", count($rows), $currenttab2);
		}
		echo("</form>\n");
		echo("</div>  <!-- Einde filter -->\n");
		
		if (count($rows) > 0) {
			echo(fnDisplayTable($rows, $kols, "", 0, "", "ledenlijst"));
			foreach ($rows as $row) {
				$sel_leden[] = $row->RecordID;
			}
			$_SESSION['sel_leden'] = $sel_leden;
		}
	}
	
} # fnLedenlijst

function fnOnderdelenmuteren($ondtype="G") {
	global $currenttab, $currenttab2;
	
	$i_lo = new cls_Lidond();
	$i_ond = new cls_Onderdeel();
	$i_lid = new cls_Lid();
	$i_auth = new cls_Authorisation();
	
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
					$contr_name = $bascn . "_" . $row->ndRecordID;
					if (isset($_POST[$contr_name])) {
						$i_lo->update($row->ndRecordID, $kolomnaam, $_POST[$contr_name]);
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
			} else {
				$scherm = "W";
			}
			$i_lo->opschonen();
		}
	} else {
		$i_ond->datacontrole();
	}
	
	if (isset($_GET['op']) and $_GET['op'] == "nieuwond") {
		$recid = $i_ond->add($ondtype);
		$i_ond->update($recid, "Type", $ondtype);
		$scherm = "S";
	}
	
	if (isset($_POST['add_lid']) and $_POST['add_lid'] > 0 and $muteerbaar) {
		$i_lo->add($onderdeelid, $lidid);
		$scherm = "L";
		$i_lo->autogroepenbijwerken(0, 1);
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
		$ondiskader = (new cls_Onderdeel())->max("Kader", sprintf("RecordID=%d", $onderdeelid));
		
		printf("<form method='post' action='%s?tp=%s&amp;OnderdeelID=%d&Scherm=L'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $onderdeelid);
		echo("<input type='hidden' name='formname' value='muteren_leden_onderdeel'>\n");
		if ($i_ond->autogroep($onderdeelid) == false) {
			$nl = sprintf("<select name='add_lid' onChange='this.form.submit();'>\n");
			$nl .= sprintf("<option value=0>Lid toevoegen ...</option>\n");
			$xf = sprintf("L.RecordID NOT IN (SELECT Lid FROM %sLidond AS LO WHERE LO.OnderdeelID=%d AND (LO.Opgezegd IS NULL))", TABLE_PREFIX, $onderdeelid);
			$nl .= $i_lid->htmloptions(-1, $alleenleden, $xf);
			$nl .= sprintf("</select>\n");
			
			if (!isset($_POST['selectie_status'])) {
				$_POST['selectie_status'] = "L";
			}
		} else {
			$nl = "";
		}
		
		$rows = $i_lo->selectielijst($onderdeelid, 0);
		$f = sprintf("OnderdeelID=%d", $onderdeelid);
		if ($ondtype == "A") {
			$rowsfunc = (new cls_Functie())->selectlijst($ondtype, $i_lo->min("Vanaf", $f), 1);
		} else {
			$rowsfunc = (new cls_Functie())->selectlijst("L", $i_lo->min("Vanaf", $f), 1);
		}
		$kols[0]['headertext'] = "Naam lid";
		$kols[0]['readonly'] = true;
		$kols[1]['headertext'] = "Vanaf";
		$kols[1]['type'] = "date";
		if ($ondiskader == 1 or $ondtype == "A" or $ondtype == "C" or $ondtype == "G") {
			$kols[2]['headertext'] = "Functie";
			$kols[2]['bronselect'] = $rowsfunc;
		}
		if ($ondiskader == 1) {
			$kols[3]['headertext'] = "Email bij functie";
			$kols[3]['type']= "email";
		}
		$kols[4]['headertext'] = "Opmerking";
		$kols[5]['headertext'] = "Tot en met";
		$kols[5]['type'] = "date";
		$kols[6]['type'] = "pk";
		
//		echo(fnDisplayTable($rows, $kols, $th, 0, "", "ledenperonderdeelmuteren", "", "", 1, "", $rowsfunc));
		echo(fnEditTable($rows, $kols, "ledenperonderdeelmuteren", $th));
		echo($nl);
	
		echo("<div id='opdrachtknoppen'>\n");
		$vor = 0;
		$vol = 0;
		for ($t=0;$t<count($ondrows);$t++) {
			if ($ondrows[$t]->RecordID == $onderdeelid) {
				if ($t > 0) {
					$vor = $ondrows[$t-1]->RecordID;
				}
				if ($t < (count($ondrows)-1)) {
					$vol = $ondrows[$t+1]->RecordID;
				}
			}
		}
		if ($vor > 0) {
			printf("<input type='button' onclick=\"location.href='%s?tp=%s&Scherm=L&OnderdeelID=%d';\" value='Vorige %s'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $vor, ARRTYPEONDERDEEL[$ondtype]);
		}
		printf("<input type='button' onclick=\"location.href='%s?tp=%s&Scherm=S';\" value='Overzicht %s'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $ondnaammv);
		
		if ($vol > 0) {
			printf("<input type='button' onclick=\"location.href='%s?tp=%s&Scherm=L&OnderdeelID=%d';\" value='Volgende %s'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $vol, ARRTYPEONDERDEEL[$ondtype]);
		}
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		
		echo("<div class='clear'></div>\n");
		
	} elseif ($scherm == "W" and $onderdeelid > 0) {
		
		$row = $i_ond->record($onderdeelid);
		
		echo("<div id='formulier'>\n");
		printf("<form method='post' action='%s?tp=%s&amp;OnderdeelID=%d&Scherm=W'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $onderdeelid);
		echo("<input type='hidden' name='formname' value='muteren_detail_onderdeel'>\n");
		
		printf("<label>RecordID</label><p>%s</p>\n", $row->RecordID);
		printf("<label>Code</label><input type='text' name='kode' class='w7' value='%s' maxlength=7>\n", $row->Kode);
		printf("<label>Naam</label><input type='text' name='naam' class='w50' value=\"%s\" maxlength=50>\n", $row->Naam);
		if ($ondtype != "T"  and $ondtype != "O") {
			printf("<label>Centraal e-mailadres</label><input type='email' name='centraalemail' class='w50' value='%s' maxlength=50>\n", $row->CentraalEmail);
			printf("<label>Is kader</label><input type='checkbox' name='kader' %s>\n", checked($row->Kader));
		}
		if ($ondtype == "A"  or $ondtype == "G") {
			printf("<label>Aangesloten bij</label><select name='ORGANIS'>\n%s</select>\n", (new cls_Organisatie())->htmloptions(0, $row->ORGANIS));
		}
		
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
		
		$igba = "";
		$f = sprintf("Toegang=%d", $onderdeelid);
		foreach ($i_auth->basislijst($f, "Tabpage") as $authrow) {
			if (strlen($igba) > 2) {
				$igba .= ", ";
			}
			$igba .= $authrow->Tabpage;
		}
		if (strlen($igba) > 2) {
			printf("<label>In gebruik bij autorisatie(s)</label><p>%s</p>\n", $igba);
		}
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<input type='submit' value='Bewaren' name='Bewaren'>\n");
		echo("<input type='submit' value='Bewaren en sluiten' name='Bewaren_Sluiten'>\n");
		echo("</div>\n");
		
		echo("</form>\n");
		echo("</div> <!-- Einde formulier -->\n");

	} else {

		$kols[0]['headertext'] = "&nbsp";
		$kols[0]['link'] = sprintf("%s?tp=%s&Scherm=W&OnderdeelID=%%d", $_SERVER['PHP_SELF'], $_GET['tp'], strtolower(ARRTYPEONDERDEEL[$ondtype]));
		$kols[0]['class'] = "muteren";
		$kols[0]['columnname'] = "RecordID";
		$kols[0]['type'] = "pk";
		
		$kols[1]['headertext'] = "Code";
		$kols[1]['columnname'] = "Kode";
		
		$kols[2]['headertext'] = "Naam";
		$kols[2]['columnname'] = "Naam";
		
		$kols[3]['headertext'] = "Kader?";
		$kols[3]['columnname'] = "Kader";
		$kols[3]['type'] = "checkbox";
				
		$kols[4]['headertext'] = "&nbsp";
		$kols[4]['link'] = sprintf("%s?tp=%s&Scherm=L&OnderdeelID=%%d", $_SERVER['PHP_SELF'], $_GET['tp'], strtolower(ARRTYPEONDERDEEL[$ondtype]));
		$kols[4]['class'] = "leden";
		$kols[4]['columnname'] = "RecordID";
		$kols[4]['title'] = "%d leden";
		$kols[4]['columntitle'] = 22;

		echo("<div class='clear'></div>\n");
	
		$res = $i_ond->editlijst($ondtype, 0);
		echo(fnEditTable($res, $kols, "onderdeeledit", $ondnaammv . " muteren"));
		
		$href = sprintf("%s?tp=%s&op=nieuwond", $_SERVER['PHP_SELF'], $_GET['tp']);
		printf("<a href='%s'><button>Nieuwe %s</button></a>\n", $href, strtolower(ARRTYPEONDERDEEL[$ondtype]));
	}
	
	echo("</div>  <!-- Einde onderdelenmuteren -->\n");	

} # fnOnderdelenmuteren

function fnEigenGegevens($lidid=0) {
	global $currenttab, $currenttab2, $currenttab3;
	
	$i_foto = new cls_Foto();
	
	if ($lidid == 0 and isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0) {
		$lidid = (new cls_Mailing_hist())->lidbijemail($_GET['MailingHistID']);
	}
	
	if ($currenttab == "Eigen gegevens") {
		$xtra_param = "";
		$lidid = $_SESSION['lidid'];
		$ct = $currenttab . "/";
	} else {
		$xtra_param = sprintf("lidid=%d", $lidid);
		$ct = $currenttab . "/" . $currenttab2 . "/";
		fnDispMenu(2);
	}
	
	if (isset($_GET['op'])) {
		$op = $_GET['op'];
	} else {
		$op = "";
	}
	$tabidx = 0;
	
	if ($lidid <= 0) {
		echo("<p class='mededeling'>Er is geen lid geselecteerd.</p>\n");
	} else {
		$naamlid = htmlentities((new cls_Lid())->Naam($lidid));
		$gs = (new cls_Lid())->Geslacht($lidid);
		
		$tn = "Algemeen";
		if (toegang($ct . $tn, 0, 0)) {
			$fn = fotolid($lidid, 1);
			$nl = (new cls_Lid())->Roepnaam($lidid);
			$fd = $i_foto->FotoLid($lidid);
			if ($fd !== false) {
				$xtra = sprintf("<div id='pasfoto'><img src='%s' title='Laatst gewijzigd op %s'></div>\n", $fd, strftime("%e %B %Y", strtotime($i_foto->laatstgewijzigd)));
			} elseif (strlen($fn) > 3) {
				$xtra = sprintf("<div id='pasfoto'><img src='%s' alt='Pasfoto %s' title='Laatst gewijzigd op %s'></div>\n", $fn, $nl, strftime("%e %B %Y", filectime(fotolid($lidid, 0))));
			} else {
				$xtra = "";
			}
			
			if ((new cls_Onderdeel())->Aantal("Type='E'") > 0 and toegang($ct . $tn, 0, 0)) {
				$rows = (new cls_Lidond())->overzichtlid($lidid, "E");
				if (count($rows) > 0) {
				}
			}
			
			$row = (new cls_Lid())->overzichtlid($lidid);
			$tabblad[$tn] = fnDisplayFormLabels($row, $xtra, "overzichtlid");
		}
		
		$tn = "Lidmaatschappen";
		if (toegang($ct . $tn, 0, 0)) {
			$rows = (new cls_Lidmaatschap())->overzichtlid($lidid);
			if (count($rows) > 1) {
				$tabblad[$tn] = fnDisplayTable($rows, null, $tn . " " . $naamlid, 0, "Lidmaatschappen");
			}
		}
		
		$tn = "Afdelingen";
		$kols[0]['headertext'] = "Afdeling";
		$kols[1]['headertext'] = "Vanaf";
		$kols[1]['type'] = "date";
		$kols[2]['headertext'] = "Functie";
		$kols[2]['columnname'] = "Functie";
		$kols[3]['headertext'] = "Opmerking";
		$kols[3]['columnname'] = "Opmerking";
		$kols[4]['headertext'] = "Groep";
		$kols[4]['columnname'] = "Groep";
		$kols[5]['headertext'] = "Opgezegd";
		$kols[5]['columnname'] = "Opgezegd";
		$kols[5]['type'] = "date";
		$kols[6]['headertext'] = "Duur";
		$kols[6]['columnname'] = "Duur";
		$rows = (new cls_Lidond())->overzichtlid($lidid, "A");
		if (count($rows) > 0 and toegang($ct . $tn, 0, 0)) {
			$rows = (new cls_Lidond())->overzichtlid($lidid, "A");
			if (count($rows) > 0) {
				$tabblad[$tn] = fnDisplayTable($rows, $kols, $tn . " " . $naamlid);
			} else {
				$tabblad[$tn] = sprintf("<p class='mededeling'>%s heeft geen %s.</p>\n", $naamlid, $tn);
			}
		}
		
		$kols[0]['headertext'] = "Groep";
		$kols[5]['headertext'] = "Tot en met";
		$tn = "Groepen";
		if ((new cls_Onderdeel())->Aantal("Type='G'") > 0 and toegang($ct . $tn, 0, 0)) {
			$rows = (new cls_Lidond())->overzichtlid($lidid, "G");
			if (count($rows) > 0) {
				$tabblad[$tn] = fnDisplayTable($rows, $kols, $tn . " " . $naamlid);
			}
		}
		
		$tn = "Kader";
		$kols[0]['headertext'] = "Naam";
		if (toegang($ct . $tn, 0, 0)) {
			$rows = (new cls_Lidond())->overzichtlid($lidid, "K");
			if (count($rows) > 0) {
				$tabblad[$tn] = fnDisplayTable($rows, null, $tn . " " . $naamlid, 0, "", "");
			}
		}

		$tn = "Rollen";
		if (toegang($ct . $tn, 0, 0)) {
			$rows = (new cls_Lidond())->overzichtlid($lidid, "R");
			if (count($rows) > 0) {
				$tabblad[$tn] = fnDisplayTable($rows, null, $tn . " " . $naamlid, 0, "", "Rollen");
			}
		}
		
		$tn = "Diploma's";
		$rows = (new cls_Liddipl())->overzichtlid($lidid);
		if (count($rows) > 0 and toegang($ct . $tn, 0, 0)) {
			if (count($rows) > 0) {
				$tabblad[$tn] = fnDisplayTable($rows, null, "Diploma's " . $naamlid);
			} else {
				$tabblad[$tn] = sprintf("<p class='mededeling'>Bij %s zijn geen diploma's geregistreerd.</p>", $naamlid);
			}
		}
		
		$tn = "Toestemmingen";
		if ($gs != "B" and (new cls_Onderdeel())->Aantal("Type='T'") > 0 and toegang($ct . $tn, 0, 0)) {
			$rows = (new cls_Lidond())->overzichtlid($lidid, "T");
			if (count($rows) > 0) {
				$tabblad[$tn] = fnDisplayTable($rows, null, $tn . " " . $naamlid, 0, "", "toestemmingen");
			} else {
				$tabblad[$tn] = sprintf("<p class='mededeling'>%s heeft geen toestemmingen verleend.</p>\n", $naamlid);
			}
			if (strlen($_SESSION['settings']['uitleg_toestemmingen']) > 0) {
				$tabblad[$tn] .= sprintf("<p>%s</p>\n", $_SESSION['settings']['uitleg_toestemmingen']);
			}
		}

		$tn = "Bijzonderheden";
		$rows = (new cls_Memo())->overzichtlid($lidid);
		if (count($rows) > 0 and toegang($ct . $tn, 0, 0)) {
			$tabblad[$tn] = "<div id='bijzonderheden'>\n";
			$tabblad[$tn] .= sprintf("<h2>%s %s</h2>\n", $tn, $naamlid);
			foreach ($rows as $row) {
				$tabblad[$tn] .= sprintf("<h3>%s</h3>\n", ARRSOORTMEMO[$row->Soort]);
				$tabblad[$tn] .= sprintf("<p>%s</p>\n", $row->Memo);
			}
			echo("</div> <!-- Einde bijzonderheden -->\n");
		}
			
		$tn = "Afwezigheden";
		$rows = (new cls_Aanwezigheid())->overzichtlid($lidid);
		if (count($rows) > 0 and toegang($ct . $tn, 0, 0)) {	
			if ($tn == $currenttab3) {
				$tabidx = count($tabblad);
			}			
			$tabblad[$tn] = "<table>\n";
			$tabblad[$tn] .= sprintf("<caption>Afwezigheden %s</caption>\n", $naamlid);
			$tabblad[$tn] .= "<tr><th>Datum</th><th>Status</th></tr>\n";
			foreach ($rows as $row) {
				$o = "";
				if (strlen($row->Functie) > 0) {
					$o = $row->Functie . ": ";
				}
				$o .= ARRPRESENTIESTATUS[$row->Status];
				$tabblad[$tn] .= sprintf("<tr><td>%s</td><td>%s</td></tr>\n", strftime("%A %e %B %Y", strtotime($row->Datum)), $o);
			}
			$tabblad[$tn] .= "</table>\n";
		}
			
		$tn = "Bewaking";
		if (toegang($ct . $tn, 0, 0)) {
			$rows = (new cls_Bewaking())->overzichtlid($lidid);
			if (count($rows) > 0) {
				$txt = fnDisplayTable($rows, null, "Bewaking " . $naamlid);
			} else {
				$txt = "";
			}
			
			$row = (new cls_Bewaking())->lidbew($lidid);
			if (isset($row->{'Totale bewaking'})) {
				$txt .= fnDisplayForm($row);
				debug("formulier");
			}
			$rows = (new cls_insbew())->overzichtlid($lidid);
			if ($_SESSION['settings']['toneninschrijvingenbewakingen'] == 1 and count($rows) > 0) {
				$txt .= fnDisplayTable($rows, null, "Ingeschreven voor bewakingen");
			}
			if (strlen($txt) > 0) {
				$tabblad[$tn] = $txt;
			}
		}
		
		$tn = "Rekeningen";
		$kols = null;
		$rows = (new cls_Rekeningregel())->overzichtlid($lidid);
		if (count($rows) > 0 and toegang($ct . $tn, 0, 0)) {
			if (count($rows) > 0) {
				$kols[0]['headertext'] = "&nbsp;";
				$kols[0]['link'] = sprintf("<a href='%s?tp=%s&op=details&lidid=%d&rid=%%d' title='Details rekening'>&nbsp;&nbsp;&nbsp;</a>", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
				$kols[0]['class'] = "details";
				$kols[1]['headertext'] = "Rekening";
				$kols[2]['headertext'] = "Seizoen";
				$kols[3]['headertext'] = "Datum";
				$kols[3]['type'] = "date";
				$kols[4]['headertext'] = "Omschrijving";
				$kols[5]['headertext'] = "Bedrag";
				$kols[5]['type'] = "currency";
				$tabblad[$tn] = fnDisplayTable($rows, $kols, "Rekeningen " . $naamlid);
			}
			
			$rid = $_GET['rid'] ?? 0;
			if ($op == "details" and $rid > 0) {
				$tabidx = count($tabblad);
				$tn = "Rekening";
				$tabblad[$tn] = RekeningDetail($rid);
			}
		}
		
		$tn = "Mailing";
		if (toegang($ct . $tn, 0, 0) and (new cls_Mailing_hist())->aantal() > 0) {
			$kols = null;
			$kols[0]['headertext'] = "&nbsp;";
			$kols[0]['link'] = sprintf("<a href='index.php?tp=%s&amp;MailingHistID=%%d'>%%s</a>", urlencode($_GET['tp']));
			$kols[0]['class'] = "details";
			$kols[1]['headertext'] = "Datum";
			$kols[1]['type'] = "date";
			$kols[2]['headertext'] = "Van";
			$kols[3]['headertext'] = "Onderwerp";
			$rows = (new cls_Mailing_hist())->overzichtlid($lidid);
			if (count($rows) > 0) {
				$tabblad[$tn] = fnDisplayTable($rows, $kols, sprintf("Aan %s verstuurde e-mails", $naamlid));
			}
			
			if (isset($_GET['MailingHistID']) and $_GET['MailingHistID'] > 0) {
				$tabidx = count($tabblad);
				$tn = "E-mail";
//				$xtra = "<p class='mededeling'><input type='button' value='Terug' onclick='history.go(-1);'></p>\n";
				$email = new email($_GET['MailingHistID']);
				$tabblad[$tn] = $email->toon(0);
				$mail = null;
			}
		}
			
		$tn = "Evenementen";
		$rows = (new cls_Evenement_Deelnemer())->overzichtlid($lidid);
		if (count($rows) > 0 and toegang($ct . $tn, 0, 0)) {
			$tabblad[$tn] = "<div id='evenementenperlid'>\n";
			$tabblad[$tn] .= "<table>\n";
			$tabblad[$tn] .= sprintf("<caption>Deelname evenementen %s</caption>\n", $naamlid);
			$tabblad[$tn] .= "<tr><th>Wanneer</th><th>Omschrijving</th><th>Contact</th><th>Opmerking / Functie</th><th>Status</th><th>In agenda</th></tr>\n";
			
			foreach($rows as $row) {
				$tabblad[$tn] .= "<tr>\n";
				$tijden = strftime("%a %e %B %Y", strtotime($row->dteDatum));
				
				if ($row->Starttijd > "00:00" and $row->Eindtijd > "00:00" and $row->MeerdereStartMomenten == 0) {
					$tijden .= sprintf("<br>\n%s tot %s uur", substr($row->Starttijd, 0, 5), substr($row->Eindtijd, 0, 5));
					
				} elseif ($row->Starttijd > "00:00") {
					$tijden .= sprintf("<br>\nom %s uur", substr($row->Starttijd, 0, 5));
				}
				$tijden = str_replace(" ", "&nbsp;", str_replace("  ", "&nbsp;", $tijden));
				if (strlen($row->Verzameltijd) >= 5 and $row->dteDatum >= date("Y-m-d H:i")) {
					$tijden .= sprintf("<br>\nVerzamelen om %s&nbsp;uur", substr($row->Verzameltijd, 0, 5));
				}
					
				$tabblad[$tn] .= sprintf("<td>%s</td>\n", $tijden);
				$oms = $row->Omschrijving;
				if (strlen($row->ndLocatie) > 1) {
					$oms .= "<br>\n" . $row->ndLocatie;
				}
				$tabblad[$tn] .= sprintf("<td>%s</td>\n", $oms);
				$tabblad[$tn] .= sprintf("<td>%s</td>\n", $row->emlContact);
				if (strlen($row->Opmerking) > 0 and strlen($row->Functie) > 0) {
					$of = sprintf("%s<br>%s", $row->Opmerking, $row->Functie);
				} elseif (strlen($row->Opmerking) > 0) {
					$of = $row->Opmerking;
				} else {
					$of = $row->Functie;
				}
				$tabblad[$tn] .= sprintf("<td>%s</td>\n", $of);
				$tabblad[$tn] .= sprintf("<td>%s</td>\n", $row->Status);
				if (($row->Status == "Aangemeld" or $row->Status == "Bevestigd" or $row->Status == "Ingeschreven") and $row->dteDatum >= date("Y-m-d H:i")) {
					$tabblad[$tn] .= sprintf("<td>%s</td>\n", fnAgendaKnop($row->dteDatum, $row->Eindtijd, $row->Verzameltijd, $row->Omschrijving, $row->ndLocatie));
				} else {
					$tabblad[$tn] .= "<td>&nbsp;</td>";
				}
				$tabblad[$tn] .= "</tr>\n";
			}
			
			$tabblad[$tn] .= "</table>\n";
			$tabblad[$tn] .= "</div>  <!-- einde evenementenperlid -->";
		}

		$tn = "Logboek";
		if ($lidid > 0 and toegang($ct . $tn, 0, 0)) {
			$rows = (new cls_Logboek())->overzichtlid($lidid);
			if (count($rows) > 0) {
				$tabblad[$tn] = fnDisplayTable($rows, null, $tn . " " .  $naamlid);
			} else {
				$tabblad[$tn] = sprintf("<p class='mededeling'>Er is geen logging van %s aanwezig.</p>\n", $naamlid);
			}
		}
		
		DisplayTabs($tabblad, $tabidx);
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
	
} # fnEigenGegevens

function overzichtverjaardagen($metfoto=1) {
	
	$i_lid = new cls_Lid();

	$verj = "";
	$verjfoto = "";
	$aantgetoond = 0;
	$dteHV = new DateTime();
	for ($i = 0; $i < $_SESSION['settings']['verjaardagenvooruit'] and $aantgetoond < $_SESSION['settings']['verjaardagenaantal']; $i++) {
		foreach($i_lid->verjaardagen($dteHV->format("Y-m-d")) as $row) {
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
				$t = sprintf("Op %s is %s jarig", strftime("%A %e %B", $dteHV->getTimestamp()), $nm);
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
		$dteHV->modify("+1 day");
	}
	if ($metfoto == 1) {
		return $verjfoto;
	} else {
		return $verj;
	}
	$i_lid = null;
} # overzichtverjaardagen

function fotolid($lidid, $metversie=0) {

	$rv = (new cls_Foto())->FotoLid($lidid);
	if ($rv == null) {
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
	}
	
	return $rv;
	
} # fotolid

function algemeenlidmuteren($lidid) {
	global $actionurl, $currenttab, $currenttab2;
	
	$i_lid = new cls_Lid();
	$row = $i_lid->record($lidid);
	$i_lo = new cls_Lidond();
	$i_ond = new cls_Onderdeel();
	$i_lm = new cls_Lidmaatschap();
	
	if ($currenttab == "Zelfservice") {
		$lidid = $_SESSION['lidid'];
	} elseif (isset($_POST['lidid'])) {
		$lidid = intval($_POST['lidid']);
	}
	
	$gs = $row->Geslacht;
	$sl = substr((new cls_Lidmaatschap())->soortlid($lidid), 0, 1);
		
	$wijzvelden[] = array('label' => "Roepnaam", 'naam' => "Roepnaam", 'lengte' => 17);
	$wijzvelden[] = array('label' => "Voorletters", 'naam' => "Voorletter", 'lengte' => 10, 'nietverw' => true, 'uitleg' => 'met puntjes');
	$wijzvelden[] = array('label' => "Tussenvoegsels", 'naam' => "Tussenv", 'lengte' => 7);
	$wijzvelden[] = array('label' => "Achternaam", 'naam' => "Achternaam", 'lengte' => 30, 'nietverw' => true, 'uitleg' => '');
	$wijzvelden[] = array('label' => "Meisjesnaam", 'naam' => "Meisjesnm", 'lengte' => 25, 'uitleg' => 'Wordt niet in de achternaam getoond.');
	$wijzvelden[] = array('label' => "Geslacht", 'naam' => "Geslacht", 'nietverw' => true);
	if ($gs != "B") {
		$wijzvelden[] = array('label' => "Geboortedatum", 'naam' => "GEBDATUM", 'type' => 'date', 'uitleg' => '');
		$wijzvelden[] = array('label' => "Geboorteplaats", 'naam' => "GEBPLAATS", 'lengte' => 22);
	}
	if ($sl == "K") {
		$wijzvelden[] = array('label' => "Opmerking", 'naam' => "Opmerking", 'lengte' => 60);
	}
	$wijzvelden[] = array('label' => "Adres", 'naam' => "Adres", 'lengte' => 30);
	$wijzvelden[] = array('label' => "Postcode", 'naam' => "Postcode", 'lengte' => 7);
	$wijzvelden[] = array('label' => "Woonplaats", 'naam' => "Woonplaats", 'lengte' => 22);
	$wijzvelden[] = array('label' => "Vast telefoonnummer", 'naam' => "Telefoon", 'lengte' => 21, 'uitleg' => "");
	$wijzvelden[] = array('label' => "Mobiel", 'naam' => "Mobiel", 'lengte' => 21, 'uitleg' => "");
	$wijzvelden[] = array('label' => "E-mail", 'naam' => "Email", 'lengte' => 45, 'uitleg' => "");
	if ($_SESSION['settings']['zs_incl_emailvereniging'] == 1 or $currenttab2 == "Wijzigen lid") {
		$wijzvelden[] = array('label' => "E-mail vereniging", 'naam' => "EmailVereniging", 'lengte' => 45, 'uitleg' => "");
	}
	if (($_SESSION['settings']['zs_incl_emailouders'] == 1 or $currenttab2 == "Wijzigen lid") and $gs != "B") {
		$wijzvelden[] = array('label' => "E-mail ouders", 'naam' => "EmailOuders", 'lengte' => 75, 'uitleg' => "");
		$wijzvelden[] = array('label' => "Namen ouders", 'naam' => "NamenOuders", 'lengte' => 90);
	}
	$wijzvelden[] = array('label' => "Waarschuwen bij nood", 'naam' => "Waarschuwen bij nood", 'lengte' => 254);
	
	if ($_SESSION['settings']['zs_incl_iban'] == 1 or $currenttab2 == "Wijzigen lid") {
		$wijzvelden[] = array('label' => "Bankrekening", 'naam' => "Bankrekening", 'lengte' => 18);
	}
	if ($_SESSION['settings']['zs_incl_machtiging'] == 1 or $currenttab2 == "Wijzigen lid") {
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
	echo("<div id='wijzigenlidgegevens'>\n");
	printf("<form method='post' action='%s' name='frm_wijzigingen'>\n", $actionurl);
	printf("<input type='hidden' name='lidid' value=%d>\n", $lidid);
	echo("<div id='algemenegegevenslidmuteren'>\n");
	printf("<label id='lblRecordID'>RecordID</label><p>%d</p>\n", $row->RecordID);
	$lidnr = $i_lm->lidnummer($row->RecordID);
	if ($lidnr > 0) {
		printf("<label id='lblLidnummer'>Lidnummer</label><p>%s</p>\n", $lidnr);
	}
	
	for ($i=0; $i < count($wijzvelden); $i++) {
		$jsoc = sprintf("OnBlur=\"savedata('lid', %d, this);\"", $row->RecordID, $wijzvelden[$i]['naam']);
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
			$inp = sprintf("<select id='%s'>%s</select>", $wijzvelden[$i]['naam'], $opt);
			
		} elseif (isset($wijzvelden[$i]['type']) and $wijzvelden[$i]['type'] == "cb") {
			if ($row->{$wijzvelden[$i]['naam']} == 1) {
				$dv = "Ja";
				$c = "checked";
			} else {
				$dv = "Nee";
				$c = "";
			}
			$inp = sprintf("<input type='checkbox' id='%s' value=1 %s>", $wijzvelden[$i]['naam'], $c);
		
		} else {
			if (isset($wijzvelden[$i]['type']) and $wijzvelden[$i]['type'] == "date") {
				$t = "date";
			} elseif (stripos($wijzvelden[$i]['label'], "e-mail") !== FALSE) {
				$t = "email";
			} else {
				$t = "text";
			}
			if (isset($wijzvelden[$i]['lengte'])) {
				$c = sprintf("class='w%d' ", $wijzvelden[$i]['lengte']);
			}
			
			if ($t == "text" and $wijzvelden[$i]['lengte'] > 100) {
				$inp = sprintf("<textarea id='%s' class='w90' placeholder='%s' rows=4>%s</textarea>", $wijzvelden[$i]['naam'], $ph, $dv);
			} elseif ($t == "date") {
				$inp = sprintf("<input type='date' id='%s' value='%s'>", $wijzvelden[$i]['naam'], $dv);
			} else {
				$inp = sprintf("<input type='%s' %sid='%s' value='%s' maxlength=%d>", $t, $c, $wijzvelden[$i]['naam'], $dv, $wijzvelden[$i]['lengte']);
			}
		}
		printf("<label id='lbl%s'>%s</label>%s", $wijzvelden[$i]['naam'], $wijzvelden[$i]['label'], $inp);
		
		if (isset($wijzvelden[$i]['uitleg'])) {
			printf("<p class='uitleg' id='uitleg_%s'>%s</p>", str_replace(" ", "_", strtolower($wijzvelden[$i]['naam'])), $wijzvelden[$i]['uitleg']);
		}
		
		echo("\n");
	}
	
	echo("</div> <!-- Einde algemenegegevenslidmuteren -->");
	
	$i_lo = new cls_Lidond(-1, $lidid);
	$f = "`Type`='E' AND IFNULL(VervallenPer, '9999-12-31') >= CURDATE()";
	$ondrows = $i_ond->lijst(0, $f);
	if (count($ondrows) > 0 and $currenttab != "Zelfservice") {
		echo("<div id='eigenschappenlidmuteren'>\n");
		echo("<h2>Eigenschappen</h2>\n");
		
		foreach($ondrows as $ondrow) {
			$c = "";
			if ($i_lo->islid($lidid, $ondrow->RecordID) == true) {
				$c = " checked";
			}
			$ro = "";
			if (strlen($ondrow->MySQL) > 10) {
				$ro = " disabled readonly";
			}
			printf("<input type='checkbox' id='eigenschap_%d'%s%s><p>%s</p>", $ondrow->RecordID, $c, $ro, $ondrow->Naam);
		}
		
		echo("</div> <!-- eigenschappenlidmuteren -->\n");
	}
	
	echo("</div>  <!-- Einde wijzigenlidgegevens -->\n");
	echo("<div id='opdrachtknoppen'>\n");
	echo("<input type='submit' value='Controleer gegevens'>\n");
	if ($sl == "K") {
		if ($row->Verwijderd > "1900-01-01") {
			echo("<button type='submit' name='Undo_Verwijderen'>Verwijderen ongedaan maken</button>\n");
		} else {
			echo("<button type='submit' name='Kloslid_Verwijderen'>Verwijderen</button>\n");
		}
	}
	echo("</div> <!-- Einde opdrachtknoppen -->\n");
	echo("</form>\n");

	printf("<script>
		\$( document ).ready(function() {
			lidalgwijzprops();
		});
		
		\$('input, select, textarea').blur(function(){
			savedata('lid', %1\$d, this);
		});
		
		$(\"[id^='eigenschap_']\").click(function(){
			
			var split_id = this.id.split('_');
			var ondid = split_id[1];
			
			if (this.checked == true) {
				var value = 1;
			} else {
				var value = 0;
			}

			$.ajax({
				url: 'ajax_update.php?entiteit=zeteigenschap',
				type: 'post',
				dataType: 'json',
				data: { lidid: %1\$d, ondid, value: value },
				success:function(response){}
			});
			
		});
		
		\$('input, #Geslacht').change(function(){
			lidalgwijzprops();
		});
		</script>\n", $lidid);

} # algemeenlidmuteren

function nieuwepasfoto($lidid, $filterlid="") {
	global $actionurl;
	
	$i_foto = new cls_Foto();
	
	$max_size_attachm = 3 * 1024 * 1024;  // 3MB
	$min_size_attachm = 4 * 1024;  // 4KB
	
	$naamlid = (new cls_Lid())->Naam($lidid);
	$ad = $_SESSION['settings']['path_pasfoto'] ?? "";
	
	if ($lidid == 0) {
		$mess = "Er is geen lid geselecteerd, neem hiervoor contact op met de webmaster.";
	
	} elseif (isset($_FILES['UploadFoto']['name']) and strlen($_FILES['UploadFoto']['name']) > 3) {

//		debug($_FILES['UploadFoto']);
	
		$ext = explode(".", $_FILES['UploadFoto']['name']);
		$ext = strtolower($ext[count($ext) - 1]);
		if ($ext == "jpeg") {
			$ext = "jpg";
		}
	
		$mess = "";
		if (in_array($ext, PASFOTOEXTENTIES) === false) {
			$mess = sprintf("Het bestand met extensie %s is niet toegestaan. De volgende extensies zijn toegestaan: %s.", $ext, implode(", ", PASFOTOEXTENTIES));

		} elseif ($_FILES['UploadFoto']['size'] > $max_size_attachm or $_FILES['UploadFoto']['error'] == 2) {
			$mess = sprintf("De foto kan niet ge-upload worden, omdat het bestand groter dan %dKB is.", $max_size_attachm / 1024);

		} elseif ($_FILES['UploadFoto']['size'] < $min_size_attachm) {
			$mess = sprintf("De foto kan niet worden ge-upload, omdat het bestand kleiner dan %d bytes is.", $min_size_attachm);

		} elseif (strlen($ad) > 0 and file_exists($ad) == false) {
			$mess = "De folder om de pasfoto op te slaan bestaat niet, neem hiervoor contact op met de webmaster.";

		} else {

			$target = $ad . sprintf("Pasfoto%d.%s", $lidid, $ext);
		
			$i_foto->add($_FILES['UploadFoto']['tmp_name'], $lidid, $ext);

			if (strlen($ad) > 0 and file_exists($ad)) {
				if (is_writable($ad) == false) {
					$mess = sprintf("Directory '%s' bestaat, maar de website heeft geen schrijfrechten.", $ad);
					(new cls_Logboek())->add($mess, 6, 0, 0, 0, 20);
					$mess = "";
					
				} else {
					$adv = $ad . "vervangen/";
					if (file_exists($adv) and is_writable($adv) == false) {
						$mess = sprintf("Directory '%s' bestaat, maar de website heeft geen schrijfrechten.", $adv);
						(new cls_Logboek())->add($mess, 6, 0, 0, 0, 20);
						$mess = "";
					}

					if (strlen($target) > 3 and file_exists($target) and file_exists($adv) and is_writable($adv) == true) {
						$of_ext = substr(strrchr($target, "."), 1);
						$bu_name = $adv . sprintf("Pasfoto%d_%d.%s", $lidid, strftime("%Y%m%d", filectime($target)), $of_ext);
						if (rename($target, $bu_name)) {
							$mess = sprintf("Pasfoto './%s' is hernoemd naar './%s'.", $target, $bu_name);
							(new cls_Logboek())->add($mess, 6, $lidid, 0, 0, 21);
						}
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
						$mess = "Er is een nieuwe pasfoto ge-upload.";
					
					} else {
						$mess = "Het is fout gegaan bij het uploaden van het bestand. Probeer het nog een keer of neem contact op met de webmaster.";
					}
				}
				(new cls_Logboek())->add($mess, 6, $lidid, 1, 0, 21);
			}
		}
	}

	echo("<div id='nieuwepasfoto'>\n");
	$fn = fotolid($lidid, 1);
	$fd = $i_foto->fotolid($lidid);
	if ($fd !== false) {
		printf("<img src='%s' alt='Huidige pasfoto %s'>\n", $fd, $naamlid);
		printf("<p>Deze foto is voor het laatst op %s gewijzigd.</p>", strftime("%e %B %Y", strtotime($i_foto->laatstgewijzigd)));
	} elseif (strlen($fn) > 4 and file_exists($fn)) {
		echo("<p>\n");
		printf("<img src='%s' alt='Huidige pasfoto %s'>\n", $fn, $naamlid);
		$fn = fotolid($lidid, 0);
		printf("Deze foto is voor het laatst op %s gewijzigd en is %dKB groot.", strftime("%e %B %Y", filectime($fn)), filesize($fn) / 1024);
		echo("</p>\n");
	} elseif ($lidid > 0) {
		printf("<p class='mededeling'>Geen huidige pasfoto van %s beschikbaar.</p>\n", $naamlid);
	}
	
	if ($i_foto->aantalwijzigingen($lidid) <= 3) {
		$actionurl = sprintf("%s?tp=%s&amp;lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
		printf("<form method='post' action='%s' name='frm_pasfoto' enctype='multipart/form-data'>\n", $actionurl);
		printf("<input type='hidden' name='MAX_FILE_SIZE' value=%d>\n", ($max_size_attachm * 2));
		printf("<label for='UploadFoto'>Nieuwe pasfoto %s:&nbsp;</label>\n", $naamlid);
		printf("<input type='hidden' name='lidid' value=%d>\n", $lidid);
		echo("<input type='file' name='UploadFoto' id='UploadFoto'>&nbsp;");
		echo("<input type='submit' name='Upload' value='Insturen'>\n");
		echo("<div class='clear'></div>\n");
		printf("<p>Het ideale formaat van de pasfoto is 390 pixels breed bij 500 pixels hoog. Het bestand moet minimaal %d&nbsp;bytes groot zijn en mag niet groter dan %d KB zijn.</p>\n", $min_size_attachm, $max_size_attachm / 1024);
		if ($lidid == $_SESSION['lidid']) {
			echo("<p>Met het uploaden van deze pasfoto geef je toestemming om deze foto aan bezoekers van deze website te tonen.</p>");
		}
		echo("</form>\n");
	} else {
		echo("<p>Je mag op dit moment geen pasfoto toevoegen, probeer het later nogmaals.</p>\n");
	}

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
		$mess = "";
		foreach ($i_ond->lijst(0, "`Type`='T'") as $ondrow) {
			if (isset($_POST['T_' . $ondrow->Kode])) {
				$loid = $i_lo->loid($lidid, $ondrow->RecordID);
				if ($loid == 0) {
					if ($i_lo->add($ondrow->RecordID, $lidid) == true) {
						$mess .= sprintf("%s is toegevoegd. ", $ondrow->Naam);
					}
				} elseif ($lidid == $_SESSION['lidid']) {
					if ($i_lo->update($loid, "Vanaf", date("Y-m-d"), "Bevestiging toestemming")) {
						$mess .= sprintf("%s is bevestigd. ", $ondrow->Naam);
					}
				}
				
			} else {
				if ($i_lo->delete(0, $lidid, $ondrow->RecordID) > 0) {
					$mess .= sprintf("%s is verwijderd. ", $ondrow->Naam);
				}
			}
		}
		if (strlen($mess) == 0) {
			$mess = "Er zijn geen toestemmingen aangepast.";
		}
		printf("<p class='mededeling'>%s</p>\n", $mess);
	}
	
	$f = "`Type`='T'";
	if ((new cls_Lidmaatschap())->soortlid($lidid) === "Lid") {
		$islid = true;
	} else {
		$islid = false;
	}
	
	echo("<div id='toestemmingenmuteren'>\n");
	printf("<h3>Toestemmingen %s muteren</h3>\n", (new cls_Lid())->Naam($lidid));
	$rows = $i_ond->lijst(0, $f, "", 0, "O.Kode");
	if (count($rows) > 0) {
		printf("<form method='post' action='%s'>\n", $actionurl);
		foreach ($rows as $row) {
			$d = "";
			if ($islid == false) {
				$d = " disabled";
			}
			printf("<label>%s</label><p><input type='checkbox' name='T_%s' value='1' %s%s></p>\n", $row->Naam, $row->Kode, checked($i_lo->islid($lidid, $row->RecordID)), $d);
		}
		echo("<div class='clear'></div>\n");
	} else {
		echo("<p>Er zijn geen toestemmingen die gegeven kunnen worden.</p>\n");
	}
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
	
	$i_lid = new cls_Lid($lidid);
	$naamlid = $i_lid->Naam();
	$minopzeggenper = new DateTime(sprintf("+%d month", $_SESSION['settings']['zs_opzegtermijn']));
	$eindeseizoen = new DateTime((new cls_seizoen())->eindehuidige());
	while ($eindeseizoen < $minopzeggenper) {
		$eindeseizoen->modify("+1 year");
	}
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$opgezegdper = new DateTime($_POST["OpzeggingPer"]);
		
		if ($opgezegdper < $minopzeggenper) {
			$opgezegdper = $minopzeggenper;
			$mess = sprintf("Er geldt een opzegtermijn van %d maand(en), hierdoor wordt de datum van opzegging %s.", $_SESSION['settings']['zs_opzegtermijn'], strftime("%e %B %Y", $opgezegdper->getTimestamp()));
			
			(new cls_Logboek())->add($mess, 6, $lidid, 1, 0, 8);
		}
		if (isset($_POST['RedenOpmerking']) and strlen($_POST['RedenOpmerking']) > 1) {
			$opm = "\n<p>" . $_POST['RedenOpmerking'] . "</p>\n";
		} else {
			$opm = "";
		}
			
		$mess = sprintf("Het lidmaatschap is per %s opgezegd.", $opgezegdper->format("d-m-Y"));
		(new cls_Logboek())->add($mess, 6, $lidid, 0, 0, 8);
			
		$body = sprintf("<p>Beste ledenadministratie,</p>\n
		<p>Hierbij zeg ik mijn lidmaatschap van de %s per %s op. Mijn lidnummer is %d.</p>\n
		%s
		<p>Met vriendelijke groeten,<br>
		<strong>%s</strong></p>\n", $_SESSION['settings']['naamvereniging'], strftime("%e %B %Y", $opgezegdper->getTimestamp()), $_SESSION['lidnr'], $opm, $naamlid);
				
		$body .= sprintf("\n<p>Dit formulier is verzonden vanaf IP-adres %s.</p>\n", $_SERVER['REMOTE_ADDR']);
	
		$mail = new email();
		$mail->toevoegenadres($_SESSION['settings']['emailledenadministratie'], "aan", "Ledenadministratie");
		$mail->toevoegenlid($lidid, "cc");
		$mail->onderwerp = sprintf("Opzegging lidmaatschap %s per %s", $naamlid, $opgezegdper->format("d-m-Y"));
		$mail->bericht = $body;
		$mail->to_outbox();
		$mess = "Een e-mail is klaar gezet om deze opzegging te bevestigen.";
		(new cls_Logboek())->add($mess, 6, $lidid, 1, 0, 8);
		$mail = null;
		if ($_SESSION['settings']['zs_opzeggingautomatischverwerken'] == 1) {
			(new cls_lidmaatschap())->opzegging($lidid, $opgezegdper->format("Y-m-d"));
		}
		
	} else {
		$form_opzegging = "<fieldset>
<h3>Opzegging lidmaatschap</h3>
<p><label for='NaamLid'>Naam lid</label><input type='text' name='NaamLid' value='[%NAAMLID%]' readonly='readonly'>
<label>Lidnummer</label><input type='number' name='Lidnr' value='[%LIDNR%]' readonly='readonly'>
<label>Opzegging per</label><input type='date' name='OpzeggingPer' value='[%OPZEGGENVANAF%]'>
<textarea name='RedenOpmerking' rows=8 cols=45 title='Reden en/of opmerkingen'></textarea>
</fieldset>
<div id='opdrachtknoppen'>\n
<input type='submit' name='VerstuurOpzegging' value='Verstuur opzegging'>\n
</div> <!-- Einde opdrachtknoppen -->\n";
		$myFile = 'templates/opzegging.html';
		if (file_exists($myFile)) {
			$content = file_get_contents($myFile);
			$content = str_replace("[%FORMOPZEGGING%]", $form_opzegging, $content);
		} else {
			$content = $form_opzegging;
		}
	
		$content = str_replace("[%OPZEGGENVANAF%]", $eindeseizoen->format("Y-m-d"), $content);
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
	
	$actionurl = sprintf("%s?tp=%s&lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	
	$i_lo = new cls_Lidond();
	$i_gr = new cls_Groep();
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$i_lo->opschonen($lidid);
		if (isset($_POST['Nieuw']) and $_POST['Nieuw'] > 0) {
			$i_lo->add($_POST['Nieuw'], $lidid);
		}
	}

	echo("<div id='onderdelenperlidmuteren'>\n");
	printf("<input type='hidden' id='lidid' value=%d>\n", $lidid);
	printf("<input type='hidden' id='ondtype' value='%s'>\n", $p_type);
	printf("<form method='post' action='%s'>\n", $actionurl);
	echo("<table id='tablelidond'>\n");
	$kf = "";
	if ($p_type == "A") {
		$typeoms = "Afdelingen";
		$kf = "<th>Functie</th><th>Functionele e-mail</th>";
		if ((new cls_Groep())->aantal() > 0) {
			$kf .= "<th>Groep</th>";
		}
	} elseif ($p_type == "BCF") {
		$typeoms = "Bestuur, commissies en functionarissen";
		$kf = "<th>Functie</th><th>Functionele e-mail</th>";
	} elseif ($p_type == "O") {
		$typeoms = "Onderscheidingen";
	} else {
		$typeoms = "Groepen";
	}
	
	printf("<caption>%s %s muteren</caption>\n", $typeoms, (new cls_Lid())->Naam($lidid));
	echo("<thead>\n");
	if ($p_type == "A") {
		printf("<tr><th>Afdeling</th><th>Vanaf</th>%s<th>Opmerking</th><th>Tot en met</th></tr>\n", $kf);
	} elseif ($p_type == "G") {
		printf("<tr><th>Groep</th><th>Vanaf</th>%s<th>Opmerking</th><th>Tot en met</th></tr>\n", $kf);
	} else {
		printf("<tr><th>Onderdeel</th><th>Vanaf</th>%s<th>Opmerking</th><th>Tot en met</th></tr>\n", $kf);
	}
	echo("</thead>\n");
	
	echo("<tbody>\n");
//	echo(htmlloperlid($lidid, $p_type));
	echo("</tbody>\n");

	echo("</table>\n");
	echo("</form>\n");
	echo("</div>\n <!-- Einde onderdelenperlidmuteren -->\n");
	$i_lo = null;
	
	echo("<script>
			$( document ).ready(function() {
				loperlidprops();
			});
			
		</script>\n");

}  # onderdelenlidmuteren

function htmlloperlid($p_lidid, $p_type) {
	
	$i_lo = new cls_Lidond();
	$i_gr = new cls_Groep();
	
	$rv = "";
	
	$kf = "";
	$tf = "";
	if ($p_type == "A") {
		$tf = "A";
	} elseif ($p_type == "BCF") {
		$tf = "L";
	}
	
	if ($p_type == "BCF") {
		$f = "(O.Type IN ('B', 'C', 'F') OR O.Kader=1)";
	} elseif ($p_type == "G") {
		$f = sprintf("O.Type='G' AND IFNULL(O.Kader, 0)=0");
	} else {
		$f = sprintf("O.Type='%s'", $p_type);
	}
	
	$frows = (new cls_functie())->selectlijst($tf);
	$jsoc = sprintf("OnChange=\"savedata('lidond', 0, this)\"");
	$jsob = sprintf("OnBlur=\"savedata('lidond', 0, this)\"");
	
	foreach ($i_lo->editlijst($f, $p_lidid) as $row) {
		$rv .= sprintf("<tr>\n<td>%s - %s</td>\n", $row->Kode, $row->Naam);
		$rv .= sprintf("<td><input type='date' id='Vanaf_%d' value='%s' required %s></td>\n", $row->RecordID, $row->Vanaf, $jsob);
		if ($p_type == "BCF" or $p_type == "A") {
			$options = "";
			foreach ($frows as $frow) {
				$options .= sprintf("<option value=%d %s>%s</option>\n", $frow->Nummer, checked($frow->Nummer, "option", $row->Functie), $frow->Omschrijv);
			}
			$rv .= sprintf("<td><select id='Functie_%d' %s>%s</select></td>\n", $row->RecordID, $jsoc, $options);
			$rv .= sprintf("<td><input type='email' id='EmailFunctie_%d' value='%s' class='w45' maxlength=45 %s></td>\n", $row->RecordID, $row->EmailFunctie, $jsob);
		}
		if ($p_type == "A" and $i_gr->aantal() > 0) {
			$f2 = sprintf("OnderdeelID=%d", $row->OnderdeelID);
			if ($i_gr->aantal($f2) > 0) {
				$options = "<option value=0></option>\n";
				foreach ((new cls_Groep())->selectlijst($row->OnderdeelID) as $grow) {
					$options .= sprintf("<option value=%d %s>%s</option>\n", $grow->RecordID, checked($grow->RecordID, "option", $row->GroepID), $grow->Kode);
				}
				$rv .= sprintf("<td><select id='GroepID_%d' %s>%s</select></td>\n", $row->RecordID, $jsoc, $options);
			} else {
				$rv .= "<td></td>\n";
			}
		}
		$rv .= sprintf("<td><input type='text' id='Opmerk_%d' class='w30' value='%s' maxlength=30 %s></td>\n", $row->RecordID, $row->OPMERK, $jsob);
		$rv .= sprintf("<td><input type='date' id='Opgezegd_%d' value='%s' %s></td>\n", $row->RecordID, $row->Opgezegd, $jsob);
		$rv .= "</tr>\n";
	}
	
	if ($p_lidid > 0) {
		$options = "<option value=0>Nieuw ....</option>\n";
		$f .= " AND IFNULL(O.VervallenPer, CURDATE()) >= CURDATE() AND IFNULL(O.GekoppeldAanQuery, 0)=0 AND LENGTH(IFNULL(O.MySQL, '')) < 10";
		foreach((new cls_Onderdeel())->lijst(0, $f) as $row) {
			$options .= sprintf("<option value=%d>%s - %s</option>\n", $row->RecordID, $row->Kode, $row->Naam);
		}
		$rv .= sprintf("<tr><td><select id='NieuwOnderdeel' OnChange='addlidond();'>%s</select>\n</td></tr>\n", $options);
	}
	
	return $rv;
}  # htmlloperlid

function diplomaslidmuteren($lidid, $td, $eenv=1) {
	global $actionurl;

	if ($td == "ZS") {
		$actie = "zelfservice_lijst";
		$zs = 1;
		$lidid = $_SESSION['lidid'] ?? 0;
	} else {
		$actie = "muteerlijst";
		$zs = 0;
	}
	
	$i_dp = new cls_Diploma();
	$i_ld = new cls_Liddipl();
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['Extra']) and $_POST['Extra'] > 0) {
			$nrid = $i_ld->add($lidid, $_POST['Extra']);
		}
		
		if (isset($_POST['inclVervallen'])) {
			$_SESSION['inclVervallen'] = $_POST['inclVervallen'];
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
	}
			
	echo("<div id='wijzigendiplomas'>\n");

//	echo("<p class='mededeling'>Werk in uitvoering, werkt dus niet zoals het zou moeten.</p>\n");
	$actionurl = sprintf("%s?tp=%s&lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	printf("<form method='post' action='%s'>\n", $actionurl);
	printf("<input type='hidden' id='lidid' value=%d>\n", $lidid);
	
	$ldrows = $i_ld->diplomasperlid($lidid);
	
	if (count($ldrows) > 8) {
		echo("<div id='filter'>\n");
		echo("<input id='tbFilterCodeNaam' placeholder='Code/naam bevat' OnKeyUp='fnFilterDiplomaLid();'>");
		echo("</div> <!-- Einde filter -->\n");
	}
	
	echo("<table id='diplomaslidmuteren'>\n");
	printf("<caption>Diploma's %s muteren</caption>\n", (new cls_Lid())->Naam($lidid));
	$minbehaald = date("d-m-Y", strtotime((new cls_Lid())->Geboortedatum($lidid)));
	echo("<thead>\n");
	printf("<tr><th>Code</th><th>Naam</th><th>Behaald op</th><th>Diplomanummer</th><th>Geldig tot</th></tr>\n");
	echo("</thead>\n");
	echo("<tbody>\n");
	$script = "";
	foreach ($ldrows as $ldrow) {
		if (strlen($ldrow->OrgNaam) > 4) {
			$dpnm = $ldrow->OrgNaam . " - " . $ldrow->Naam;
		} elseif (strlen($ldrow->OrgNaam) > 1) {
			$dpnm = $ldrow->OrgNaam . " " . $ldrow->Naam;
		} else {
			$dpnm = $ldrow->Naam;
		}
		printf("<tr>\n<td>%s</td>\n<td>%s</td>\n\n", $ldrow->Kode, $dpnm);
		$id = sprintf("DatumBehaald_%d", $ldrow->RecordID);
		printf("<td><input type='date' id='%s' value='%s' required></td>\n", $id, $ldrow->DatumBehaald);
		$script .= sprintf("$('#%1\$s').datepicker();\n$('#%1\$s').datepicker('dateFormat', 'yy-mm-dd').datepicker('maxDate', new Date());\n$('#%1\$s').datepicker('setDate', '%3\$s').datepicker('option', 'gotoCurrent', true);\n", $id, $minbehaald, $ldrow->DatumBehaald);
		printf("<td><input type='text' id='Diplomanummer_%d' value='%s' maxlength=30></td>\n", $ldrow->RecordID, $ldrow->Diplomanummer);
		printf("<td><input type='date' id='LicentieVervallenPer_%d' value='%s'></td>\n", $ldrow->RecordID, $ldrow->LicentieVervallenPer);
		echo("</tr>\n");
	}
	echo("</tbody>\n");
	echo("</table>\n");
	
	$f = "(EindeUitgifte IS NULL)";
	if ($zs == 1) {
		$f .= " AND Zelfservice=1";
	}
	$vf = "";
	if ($i_dp->aantal($f) > 0) {
		$vf = sprintf("<input type='checkbox' name='inclVervallen' value=1 %s  OnChange='this.form.submit();'><p>Inclusief vervallen</p>\n", checked(getvar("inclVervallen", 0)));
	}
	printf("<select name='Extra' OnChange='this.form.submit();'><option value=-1>Toevoegen ...</option>%s</select>%s\n", $i_dp->htmloptions(0, -1, $zs, getvar("inclVervallen", 0)), $vf);

//	echo("<div id='opdrachtknoppen'><input type='submit' value='Bewaren' name='diplwijz'></div>\n");

	echo("</form>\n");
	echo("</div>  <!-- Einde wijzigendiplomas -->\n");

	$i_dp = null;
	$i_ld = null;

	printf("<script>
				$(document).ready(function() {
					$('input').blur(function(){
						savedata('liddipl', 0, this);
					});
					
			%s
			
				});
		</script>\n", $script);
	
} # diplomaslidmuteren

function lidmaatschapmuteren($lidid) {
	
	$i_lm = new cls_Lidmaatschap();
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$omz['Lidnr'] = "Lidnr";
		$omz['LIDDATUM'] = "Vanaf";
		$omz['Opgezegd'] = "Opgezegd";
		$omz['OpgezegdDoorVereniging'] = "OpgezegdDoorVereniging";
		$f = sprintf("Lid=%d", $lidid);
		foreach ($i_lm->basislijst($f, "LIDDATUM") as $row) {
			foreach($omz as $kolomnaam => $bascn) {
				$contr_name = sprintf("%s_%d", $bascn, $row->RecordID);
				if ($bascn == "OpgezegdDoorVereniging") {
					$_POST[$contr_name] = $_POST[$contr_name] ?? 0;
				}
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
	echo("<tr><th>RecordID</th><th>Lidnummer</th><th>Lid vanaf</th><th>Opgezegd per</th><th>Door vereniging?</th><th></th><tr>\n");
	$toev = true;
	$aant = 0;
	$f = sprintf("Lid=%d", $lidid);
	foreach ($i_lm->basislijst($f, "LIDDATUM") as $row) {
		echo("<tr>\n");
		printf("<td>%d</td>\n", $row->RecordID);
		printf("<td><input type='number' name='Lidnr_%d' value=%d onblur='this.form.submit();'></td>\n", $row->RecordID, $row->Lidnr);
		printf("<td><input type='date' name='Vanaf_%d' value='%s' onblur='this.form.submit();'></td>\n", $row->RecordID, $row->LIDDATUM);
		printf("<td><input type='date' name='Opgezegd_%d' value='%s' onblur='this.form.submit();'></td>\n", $row->RecordID, $row->Opgezegd);
		
		if ($row->Opgezegd > "1900-01-01") {
			printf("<td><input type='checkbox' name='OpgezegdDoorVereniging_%d' value=1 %s onClick='this.form.submit();'></td>\n", $row->RecordID, checked($row->OpgezegdDoorVereniging));
		} else {
			echo("<td></td>");
			printf("<input type='hidden' name='OpgezegdDoorVereniging_%d' value=0>\n", $row->RecordID);
		}
		
		if ($row->Opgezegd >= "1901-01-01") {
			if ($row->Opgezegd >= date("Y-m-d")) {
				$toev = false;
			} elseif ($row->LIDDATUM >= date("Y-m-d")) {
				$toev = false;
			}
		} else {
			$toev = false;
		}
		if ($row->Opgezegd >= date("Y-m-d") and $_SESSION['settings']['mailing_bevestigingopzegging'] > 0) {
			printf("<td><input type='image' name='BO_%d' src='images/email.png' alt='Verstuur bevestiging' title='Verstuur bevestiging'></td>\n", $row->Lidnr);
		} else {
			echo("<td></td>\n");
		}
		echo("</tr>\n");
		$aant++;
	}
	echo("</table>\n");
	
	if ($toev) {
		echo("<button name='NieuwLidmaatschap' value=1>Nieuw lidmaatschap</button>\n");
	}
	
	echo("</form>\n");
	echo("</div>  <!-- Einde lidmaatschapmuteren -->\n");
	$i_lm = null;
} # lidmaatschapmuteren

function instellingenledenmuteren() {
	global $currenttab, $currenttab2;
	
	$i_p = new cls_Parameter();
	$i_ond = new cls_Onderdeel();
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		foreach (array("zs_incl_beroep", "zs_incl_bsn", "zs_incl_emailouders", "zs_incl_emailvereniging", "zs_incl_iban", "zs_incl_legitimatie", "zs_incl_slid", "zs_opzeggingautomatischverwerken") as $c) {
			if (isset($_POST[$c])) {
				$_POST[$c] = 1;
			} else {
				$_POST[$c] = 0;
			}
		}
		
		foreach ($i_p->lijst() as $row) {
			if (isset($_POST[$row->Naam])) {
//				debug($row->Naam . ": " . $_POST[$row->Naam]);
				$i_p->update($row->Naam, $_POST[$row->Naam]);
			}
		}
	}	
	$i_p->vulsessie();
	
	echo("<div id='instellingenmuteren'>\n");
	printf("<form method='post' action='%s?tp=%s/%s'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
	
	echo("<h2>Algemeen</h2>\n");
	$opt = $i_ond->htmloptions($_SESSION['settings']['woonadres_anderen_tonen']);
	printf("<label>Aan wie moet het woonadres aan andere leden worden getoond?</label><select name='woonadres_anderen_tonen' OnChange='this.form.submit();'><option value=-2>Niemand</option>\n%s</select>\n", $opt);
	printf("<label>Opzegtermijn in maanden</label><input type='number' name='zs_opzegtermijn' value=%d OnChange='this.form.submit();'>\n", $_SESSION['settings']['zs_opzegtermijn']);
	
	$options = "";
	foreach((new cls_Mailing())->lijst("Templates") as $row) {
		$options .= sprintf("<option%s value=%d>%s</option>", checked($row->RecordID, "option", $_SESSION['settings']['mailingbijadreswijziging']), $row->RecordID, $row->subject);
	}
	printf("<label>Mailing voor versturen e-mail naar ledenadministratie bij wijzigen postcode</label><select name='mailingbijadreswijziging' OnChange='this.form.submit();'>\n<Option value=0>Geen</option>%s</select>\n", $options);
	printf("<label>Moet een opzegging door een lid automatisch worden verwerkt?</label><input type='checkbox' name='zs_opzeggingautomatischverwerken' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_opzeggingautomatischverwerken']));
	
	echo("<h2>Rekeningen</h2>\n");
	printf("<label>Groep voor betaald door</label><select name='rekening_groep_betaalddoor' OnChange='this.form.submit();'><option value=0>Geen</option>%s</select>\n", $i_ond->htmloptions($_SESSION['settings']['rekening_groep_betaalddoor']));
		  
	echo("<h2>Agenda</h2>\n");
	printf("<label>Van wie moeten de verjaardagen op de agenda worden vermeld?</label><select name='agenda_verjaardagen' OnChange='this.form.submit();'>\n<option value=-1>Niemand</option>\n%s</select>\n", (new cls_eigen_lijst())->htmloptions($_SESSION['settings']['agenda_verjaardagen']));
	printf("<label>URL van de ICS voor feestdagen</label><input name='agenda_url_feestdagen' class='w150' value='%s'>\n", $_SESSION['settings']['agenda_url_feestdagen']);
	
	echo("<h2>Beschikbaar in de zelfservice</h2>\n");
	printf("<label>Beroep?</label><input type='checkbox' name='zs_incl_beroep' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_beroep']));
	printf("<label>Burgerservicenummer (BSN)?</label><input type='checkbox' name='zs_incl_bsn' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_bsn']));
	printf("<label>E-mail ouders?</label><input type='checkbox' name='zs_incl_emailouders' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_emailouders']));
	printf("<label>E-mail vereniging?</label><input type='checkbox' name='zs_incl_emailvereniging' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_emailvereniging']));
	printf("<label>Bankrekening / IBAN?</label><input type='checkbox' name='zs_incl_iban' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_iban']));
	printf("<label>Machtiging automatische incasso afgegeven?</label><input type='checkbox' name='zs_incl_machtiging' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_machtiging']));
	printf("<label>Legitimatie?</label><input type='checkbox' name='zs_incl_legitimatie' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_legitimatie']));
	printf("<label>Sportlink ID?</label><input type='checkbox' name='zs_incl_slid' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_slid']));
	
	echo("<h2>Overig in de zelfservice</h2>\n");
	printf("<label>Welke soorten memo's mogen leden zelf muteren?</label><input type='text' name='zs_muteerbarememos' value=\"%s\" OnChange='this.form.submit();'><p>(Scheiden met een komma)</p>\n", $_SESSION['settings']['zs_muteerbarememos']);
	printf("<label>Welke tekst moet er als uitleg bij de toestemmingen worden vermeld?</label><textarea name='uitleg_toestemmingen' rows=2 cols=68 OnChange='this.form.submit();'>%s</textarea>\n", $_SESSION['settings']['uitleg_toestemmingen']);
	
	echo("</form>\n");
	echo("</div> <!-- Einde instellingenmuteren -->\n");
	
}  # instellingenledenmuteren

function fnEigenlijstenmuteren() {
	
	if (isset($_GET['tp'])) {
		$tp = $_GET['tp'];
	}
	
	if (isset($_GET['paramID']) and $_GET['paramID'] > 0) {
		$elid = intval($_GET['paramID']);
	} else {
		$elid = 0;
	}
	$i_el = new cls_Eigen_lijst("", $elid);
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		if (isset($_POST['Toevoegen']) and $_POST['Toevoegen'] == "lijst_toevoegen") {
			$i_el->add();
		}
		
		if (isset($_POST['naam'])) {
			$i_el->update($elid, "Naam", $_POST['naam']);
		}
		
		if (isset($_POST['mysql'])) {
			$i_el->update($elid, "MySQL", $_POST['mysql']);
		}
		
		if (isset($_POST['default_waarde_params'])) {
			$i_el->update($elid, "Default_value_params", $_POST['default_waarde_params']);
		}

		$i_el->controle($elid);
		
		if (isset($_POST['BewarenSluiten'])) {
			printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $tp);
		} else {
			printf("<script>location.href='%s?tp=%s&paramActie=2&paramID=%d';</script>\n", $_SERVER['PHP_SELF'], $tp, $elid);
		}
		
	} elseif (isset($_GET['paramActie']) and $_GET['paramActie'] == 3 and $elid > 0) {
		
		$i_el->delete($elid);
		printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $tp);
		
	} elseif (isset($_GET['paramActie']) and $_GET['paramActie'] == 2 and $elid > 0) {
		
		$i_el->controle($elid);
		$row = $i_el->record();
		
		echo("<div id='eigenlijstmuteren'>\n");
		printf("<form method='post' action='%s?tp=%s&paramID=%d'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $elid);
		
		printf("<label>Naam eigen lijst</label><input type='text' name='naam' value='%s' maxlength=35>", $row->Naam);
		printf("<label>MySQL-code</label><textarea id='mysql' name='mysql' rows=16 cols=100>%s</textarea>\n", $row->MySQL);
		printf("<label>Standaard waarden parameter(s)</label><input type='text' name='default_waarde_params' value='%s' maxlength=100><p>(bij meedere scheiden met een ;)</p>\n", $row->Default_value_params);
		echo("<label>Beschikbare variabelen</label><p>[%LIDNAAM%], [%TELEFOON%], [%EMAIL%]</p>");
		if (strlen($i_el->sqlerror) == 0) {
			printf("<label>Aantal records</label><p>%d</p>\n", $row->AantalRecords);
			printf("<label>Aantal kolommen</label><p>%d</p>\n", $row->AantalKolommen);
		} else {
			$i_el->mess = sprintf("In Eigen_lijst %d is de MySQL-code niet correct. Foutmelding: %s", $elid, $i_el->sqlerror);
			printf("<p>%s</p>\n", $i_el->mess);
			$i_el->Log($elid);
		}
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<input type='submit' value='Bewaren' name='Bewaren'>\n");
		echo("<input type='submit' value='Bewaren & Sluiten' name='BewarenSluiten'>\n");
		echo("</div>\n");
		
		echo("</form>\n");
		echo("</div> <!-- Einde eigenlijstmuteren -->\n");
		
	} else {
		
		echo("<div id='overzichteigenlijsten'>\n");
//		$i_el->controle();
		$rows = $i_el->lijst();
		if (count($rows) > 0) {
			echo("<table>\n");
			echo("<caption>Overzicht Eigen lijsten</caption>\n");
			echo("<tr><th></th><th>Naam</th><th># records</th><th># kolommen</th><th>LidID</th><th></th></tr>\n");
			foreach($rows as $row) {
				$bl = sprintf("<a href='%s?tp=%s&paramID=%d&paramActie=%%d'>&nbsp;&nbsp;&nbsp;</a>", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
				printf("<tr><td>%s</td><td>%s</td><td class='number'>%d</td><td class='number'>%d</td><td class='number'>%d</td><td>%s</td></tr>\n", sprintf($bl, 2), $row->Naam, $row->AantalRecords, $row->AantalKolommen, $row->KolomLidID, sprintf($bl, 3));
			}
			echo("</table>\n");
		}
		echo("</div> <!-- Einde overzichteigenlijsten -->\n");
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		echo("<div id='opdrachtknoppen'>\n");
		echo("<button type='submit' name='Toevoegen' value='lijst_toevoegen'>Lijst toevoegen</button>\n");
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
	}
	
}  # fnEigenlijstenmuteren

function fnAgendaItems() {
	
	$i_ak = new cls_Afdelingskalender();
	$kalitem = null;
	$in = 0;
	
	if ($_SESSION['lidid'] > 0) {
		foreach ($i_ak->lijst(-1, "", "Datum >= CURDATE() AND (Activiteit=0 OR LENGTH(Omschrijving) > 0)") as $row) {
			if (in_array($row->OnderdeelID, explode(",", $_SESSION['lidgroepen']))) {
				$kalitem[$in]['datum'] = date("Y-m-d", strtotime($row->Datum));
				
				$lorow = (new cls_Lidond())->record($_SESSION['lidid'], $row->OnderdeelID);
				if (isset($lorow->Starttijd) and strlen($lorow->Starttijd) > 3 and $row->Activiteit == 1) {
					$kalitem[$in]['datum'] .= " " . $lorow->Starttijd;
				}
				
				if ($row->Activiteit == 1) {
					$oms = $lorow->AfdNaam;
				} else {
					$oms = "Geen " . $lorow->AfdNaam;
				}
				if (strlen($row->Omschrijving) > 1) {
					$oms .= " (" . $row->Omschrijving . ")";
				}
				$kalitem[$in]['oms'] = $oms;
				$in++;
			}
		}
	}
	
	foreach ((new cls_Evenement())->lijst(4) as $row) {
		if ($row->MeerdereStartMomenten == 0) {
			$kalitem[$in]['datum'] = date("Y-m-d H:i", strtotime($row->Datum));
		} else {
			$kalitem[$in]['datum'] = date("Y-m-d", strtotime($row->Datum));
			if ($_SESSION['lidid'] > 0) {
				$edrow = (new cls_Evenement_Deelnemer())->record(0, $_SESSION['lidid'], $row->RecordID);
				if (isset($edrow->RecordID) and strlen($edrow->StartMoment) > 2) {
					$kalitem[$in]['datum'] .= " " . $edrow->StartMoment;
				}
			}
		}
		$kalitem[$in]['oms']= $row->Omschrijving;
		$in++;
	}
	
	$rv = "";
	if (isset($kalitem) and count($kalitem) > 1) {
		usort($kalitem, function($a, $b) {
			return $a['datum'] <=> $b['datum'];
		});
	}
	for ($in=0;$in<=9;$in++) {
		if (isset($kalitem[$in]['datum'])) {
			$dat = strftime("%A %e %B", strtotime($kalitem[$in]['datum']));
			$tijd = date("H:i", strtotime($kalitem[$in]['datum']));
			if ($tijd > "00:00") {
				$dat .= " (" . $tijd . ")";
			}
			$rv .= sprintf("<li>%s: %s</li>\n", $dat, $kalitem[$in]['oms']);
		}
	}

	return $rv;
	
}  #fnAgendaItems

function fnBasisgegevens($p_type) {
	
	if ($p_type == "Onderdelen") {
		$i_ond = new cls_Onderdeel();
		
		if (isset($_POST['NieuwOnderdeel'])) {
			$i_ond->add();
		}
		$i_ond->opschonen();
		$i_ond->datacontrole();
		
		$ondrows = $i_ond->lijst(0, "", "", 0, "IF(O.VervallenPer IS NULL, 0, 1), O.`Type`", 0);
		
		$kols[0]['headertext'] = "#";
		$kols[0]['columnname'] = "RecordID";
		$kols[0]['type'] = "pk";
		$kols[0]['readonly'] = true;
		
		$kols[1]['headertext'] = "Code";
		$kols[1]['columnname'] = "Kode";
		
		$kols[2]['headertext'] = "Naam";
		$kols[2]['columnname'] = "Naam";
		
		$kols[4]['headertext'] = "Type";
		$kols[4]['bronselect'] = ARRTYPEONDERDEEL;
		$kols[4]['columnname'] = "Type";
		
		$kols[5]['headertext'] = "Kader?";
		$kols[5]['type'] = "checkbox";
		$kols[5]['columnname'] = "Kader";
		
		$kols[6]['headertext'] = "Alleen leden?";
		$kols[6]['type'] = "checkbox";
		$kols[6]['columnname'] = "Alleen leden";
		
		$kols[7]['headertext'] = "E-mailadres";
		$kols[7]['type'] = "email";
		$kols[7]['columnname'] = "CentraalEmail";
		
		$kols[8]['headertext'] = "Vervallen per";
		$kols[8]['type'] = "date";
		$kols[8]['columnname'] = "VervallenPer ";
		
		$kols[9]['headertext'] = "Opschonen na X dagen";
		$kols[9]['type'] = "integer";
		$kols[9]['columnname'] = "HistorieOpschonen";
		
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		echo(fnEditTable($ondrows, $kols, "onderdeeledit", "Muteren onderdelen"));
		echo("<div id='opdrachtknoppen'>\n");
		echo("<button name='NieuwOnderdeel' onClick='this.form.submit();'>Nieuw onderdeel</button>\n");
		echo("<input type='submit' value='Bewaren'>\n");
		echo("</div> <! =-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
		
		$i_ond = null;
		
	} elseif ($p_type == "Functies") {
		$i_fnk = new cls_Functie();
		$i_fnk->datacontrole();

		if (isset($_POST['NieuweFunctie'])) {
			$i_fnk->add();
		}
		
		$fnkres = $i_fnk->basislijst("", "IF(`Vervallen per` IS NULL, 0, 1), Omschrijv", 0);
		
		$kols = null;
		$kols[0]['headertext'] = "Nummer";
		$kols[0]['readonly'] = true;
		$kols[0]['type'] = "pk";
		
		$kols[1]['headertext'] = "Omschrijving";
		$kols[3]['headertext'] = "Afkorting";
		
		$kols[4]['headertext'] = "Volgorde";
		$kols[4]['type'] = "integer";
		
		$kols[5]['headertext'] = "Bij afdeling?";
		$kols[5]['type'] = "checkbox";
		
		$kols[6]['headertext'] = "Algmeen?";
		$kols[6]['type'] = "checkbox";
		
		$kols[7]['headertext'] = "Bij bewaking?";
		$kols[7]['type'] = "checkbox";
		
		$kols[8]['headertext'] = "Kader?";
		$kols[8]['type'] = "checkbox";
		
		$kols[9]['headertext'] = "Vervallen per";
		$kols[9]['type'] = "date";
		
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		
		echo(fnEditTable($fnkres, $kols, "functieedit", "Muteren functies"));
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<button name='NieuweFunctie' onClick='this.form.submit();'>Nieuwe functie</button>\n");
		echo("</div> <! =-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
		
		$i_ond = null;
		
	} elseif ($p_type == "Diplomas") {
		$i_dp = new cls_Diploma();
		$res = $i_dp->lijst();
		
		echo("<p class='mededeling'>Dit gedeelte is nog niet klaar.</p>\n");
		
		$kols[0]['type'] = "pk";
		$kols[0]['readonly'] = true;
		$kols[1]['headertext'] = "Code";
		$kols[2]['headertext'] = "Naam";
		
		
		$kols[3]['headertext'] = "Type";
		$kols[3]['bronselect'] = ARRTYPEDIPLOMA;
		
		$kols[4]['headertext'] = "Uitgegeven door";
		foreach ((new cls_Organisatie())->lijst() as $row) {
			$arrOrg[$row->Nummer] = $row->Naam;
		}
		$kols[4]['bronselect'] = $arrOrg;
		
		$kols[6]['headertext'] = "Volgnr";
		$kols[6]['type'] = "integer";
		
		$kols[11]['headertext'] = "Vervallen per";
		$kols[11]['type'] = "date";
		
		$kols[12]['headertext'] = "Einde uitgifte";
		$kols[12]['type'] = "date";
		
		$kols[17]['headertext'] = "Zelfservice?";
		$kols[17]['type'] = "checkbox";
		
		echo("<div id='diplomasmuteren'>\n");
		echo(fnEditTable($res, $kols, "diplomaedit", "Muteren diploma's"));
		echo("</div> <!-- Einde diplomasmuteren -->\n");
		
	} elseif ($p_type == "Organisaties") {
		$i_org = new cls_Organisatie();
		
		if (isset($_POST['NieuweOrganisatie'])) {
			$i_org->add();
		} elseif (isset($_GET['op']) and $_GET['op'] == "delete" and isset($_GET['OrgNr']) and $_GET['OrgNr'] > 0) {
			$i_org->delete($_GET['OrgNr']);
		}
		
		$orgres = $i_org->lijst(0, 0);
		
		$kols[0]['headertext'] = "Nummer";
		$kols[0]['type'] = "pk";
		$kols[0]['cond_ro'] = "aantalLinked";
		
		$kols[1]['headertext'] = "Afkorting";
		$kols[2]['headertext'] = "Volledige naam";


		$kols[3]['headertext'] = "&nbsp;";
		$kols[3]['columnname'] = "Nummer";
		$kols[3]['link'] = sprintf("%s?tp=%s&op=delete&OrgNr=%%d", $_SERVER['PHP_SELF'], $_GET['tp']);
		$kols[3]['cond_ro'] = "aantalLinked";
		$kols[3]['class'] = "trash";
		
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		
		echo(fnEditTable($orgres, $kols, "organisatieedit", "Muteren organisaties"));
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<button name='NieuweOrganisatie' onClick='this.form.submit();'>Nieuwe organisatie</button>\n");
		echo("</div> <! =-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
		
	} elseif ($p_type == "Seizoenen") {
		$i_sz = new cls_Seizoen();
		
		if (isset($_POST['NieuwSeizoen'])) {
			$i_sz->add();
		} elseif (isset($_GET['op']) and $_GET['op'] == "delete" and isset($_GET['SeizoenNr']) and $_GET['SeizoenNr'] > 0) {
			$i_sz->delete($_GET['SeizoenNr']);
		}
		
		$kols[0]['headertext'] = "Nummer";
		$kols[0]['columnname'] = "Nummer";
		$kols[0]['type'] = "pk";
		$kols[0]['cond_ro'] = "aantalRek";
		
		$kols[1]['headertext'] = "Begindatum";
		$kols[1]['columnname'] = "Begindatum";
		$kols[1]['type'] = "rqdate";
		
		$kols[2]['headertext'] = "Einddatum";
		$kols[2]['columnname'] = "Einddatum";
		$kols[2]['type'] = "rqdate";
		
		$kols[3]['headertext'] = "Leeftijdgrens jeugdleden";
		$kols[3]['columnname'] = "Leeftijdsgrens jeugdleden";
		$kols[3]['type'] = "integer";
		
		$kols[4]['headertext'] = "&nbsp;";
		$kols[4]['columnname'] = "Nummer";
		$kols[4]['class'] = "trash";
		$kols[4]['link'] = sprintf("%s?tp=%s&op=delete&SeizoenNr=%%d", $_SERVER['PHP_SELF'], $_GET['tp']);
		$kols[4]['cond_ro'] = "aantalRek";
		
		$seizres = $i_sz->lijst(0);
		
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		echo(fnEditTable($seizres, $kols, "seizoenedit", "Muteren seizoenen"));
		echo("<div id='opdrachtknoppen'>\n");
		echo("<button name='NieuwSeizoen' onClick='this.form.submit();'>Nieuw seizoen</button>\n");
		echo("</div> <! =-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
		
	} else {
		echo("<p class='mededeling'>Dit gedeelte is nog niet gebouwd.</p>\n");
	}
}  # fnBasisgegevens

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
      }
      $totaal = 98 - (int)bcmod($totaal, 97);
      if ($controle == $totaal) {
         $retval = true;
      }
   }
   
   return $retval;

} # IsIBANgoed

?>
