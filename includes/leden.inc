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
				<label>Login:</label><input type='text' title='Login' name='ingelogdlogin' value='%s' readonly='readonly'>
				<label>Oude wachtwoord:</label><input type='password' name='pw_oud' title='Huidige wachtwoord' maxlength=12>
				<label>Nieuw wachtwoord:</label><input type='password' name='pw_nieuw' title='Nieuw wachtwoord' maxlength=12>
				<label>Herhaal wachtwoord:</label><input type='password' name='pw_herhaal' title='Herhaal nieuw wachtwoord' maxlength=12>", $_SESSION['username']);
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
	global $ldl, $lididtestusers, $dtfmt;
	
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
	
	$dtfmt->setPattern("MMMM yyyy");
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
					$func .= " &amp; " .  $row->Opmerk;
				}
			} else {
				$func = $row->Opmerk;
			}
			
			$fd = fotolid($row->Lid, 1);
			if ($actie == "Onderscheidingen" and strlen($fd) > 3) {
				$txt .= sprintf("<div class='kaartje'><img src='%1\$s' alt='Pasfoto %2\$s'><p class='naamkaderlid'>%2\$s</p>\n<p>vanaf %3\$s</p>\n</div>\n", $fd, $ln, $dtfmt->format(strtotime($row->Vanaf)));
			} elseif ($actie == "Onderscheidingen") {
				$txt .= sprintf("<div class='kaartje'><p class='naamkaderlid'>%s</p>\n<p>vanaf %s</p>\n</div>\n", $ln, $dtfmt->format(strtotime($row->Vanaf)));
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
			$txt .= sprintf("<tr>\n<td>%s</td>\n<td>%s</td>\n<td>%s</td>\n<td>%s</td>\n</tr>\n", $ln, $func, $email, $dtfmt->format(strtotime($row->Vanaf)));
		}
		$txt .= "</table>\n";
	}
	$txt .= "</div>  <!-- Einde kaderoverzicht -->\n";
	
	return $txt;
	
} # fnWieiswie

function fnLedenlijst() {
	global $wherelidond, $currenttab2, $currenttab3;
	
	$starttijd = microtime(true);
	$i_lid = new cls_Lid();
	$i_lo = new cls_Lidond();

	$i_lo->auto_einde();
	$i_lo->autogroepenbijwerken(1);
	(new cls_Eigen_lijst())->controle();
	
	fnDispMenu(2);
	
	$_SESSION['val_groep'] = $_SESSION['val_groep'] ?? 0;
	if (isset($_POST['lbGroepFilter'])) {
		$_SESSION['val_groep'] = $_POST['lbGroepFilter'];
	}
	
	if ($currenttab2 == "Nieuw (klos)lid") {
		if ($_SERVER['REQUEST_METHOD'] == "POST") {
			if (isset($_POST['addkloslid']) and isset($_POST['achternaam']) and strlen($_POST['achternaam']) > 1) {
				$lidid = $i_lid->add($_POST['achternaam'], $_POST['postcode']);
				if (isset($_POST['lidvanaf']) and $_POST['lidvanaf'] >= "1950-01-01" and strtotime($_POST['lidvanaf']) !== false) {
					(new cls_Lidmaatschap())->add($lidid, $_POST['lidvanaf']);
				}
			}
			$tp = sprintf("Ledenlijst/Wijzigen+lid/Algemene+gegevens&lidid=%d", $lidid);
			printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $tp);
			
		} else {
			
			echo("<div id='nieuwlid'>\n");
			printf("<form name='frmNieuwLid' action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
			echo("<label>Achternaam</label><input type='text' name='achternaam' title='Achternaam' maxlength=40><p>(zonder tussenvoegsels)</p>\n");

			echo("<label>Postcode</label><input type='text' class='postcode w7' name='postcode' maxlength=7>\n");
			
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
		
		$arrCB = array("adres", "telefoon", "email", "lidnummer", "vanaf", "opgezegd");
		
		foreach ($arrCB as $k) {
			$vn = "toon" . $k;
			$cn = "ledenlijst_" . $vn;
			if ($_SERVER['REQUEST_METHOD'] == "POST") {
				if (isset($_POST[$vn]) and $_POST[$vn] == "on") {
					$$vn = 1;
				} else {
					$$vn = 0;
				}
				setcookie($cn, $$vn, time()+(3600*24*180));
			} elseif (isset($_COOKIE[$cn])) {
				$$vn = intval($_COOKIE[$cn]);
			} else {
				$$vn = 0;
			}
			$$vn = intval($$vn);
		}
		
		echo("<div id='filter'>\n");
		echo("<input type='text' name='tbTekstFilter' id='tbTekstFilter' placeholder='Tekstfilter' OnKeyUp=\"fnFilter('ledenlijst', this);\">\n");
		printf("<form action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);

		if ($currenttab2 != "Klosleden") {
			printf("<select name='lbGroepFilter' onchange='this.form.submit();'>\n<option value=0>Filter op onderdeel</option>\n%s</select>\n", (new cls_Onderdeel())->htmloptions($_SESSION['val_groep']));
		}
		if (toegang("Woonadres_tonen", 0, 0)) {
			printf("<span><input type='checkbox' name='toonadres' title='Toon adres'%s onClick='this.form.submit();'><p>Adres</p></span>", checked($toonadres));
		}
		printf("<span><input type='checkbox' name='toontelefoon' title='Toon telefoon'%s onClick='this.form.submit();'><p>Telefoon</p></span>", checked($toontelefoon));
		printf("<span><input type='checkbox' name='toonemail' title='Toon e-mail'%s onClick='this.form.submit();'><p>E-mail</p></span>", checked($toonemail));
		printf("<span><input type='checkbox' name='toonlidnummer' title='Toon lidnummer'%s onClick='this.form.submit();'><p>Lidnr</p></span>", checked($toonlidnummer));
		printf("<span><input type='checkbox' name='toonvanaf' title='Toon vanaf'%s onClick='this.form.submit();'><p>Vanaf</p></span>", checked($toonvanaf));
		if ($currenttab2 != "Toekomstige leden") {
			printf("<span><input type='checkbox' name='toonopgezegd' title='Toon opgezegd'%s onClick='this.form.submit();'><p>Opgezegd</p></span>", checked($toonopgezegd));
		}
		
		if (toegang("Ledenlijst/Overzicht lid", 0, 0)) {
			$kols[0]['link'] = "<a href='index.php?tp=Ledenlijst/Overzicht+lid&lidid=%d'>%s</a>";
			$kols[0]['class'] = "details";
			$kols[0]['columnname'] = "RecordID";
		} else {
			$kols[0]['skip'] == true;
		}
		$kols[1]['headertext'] = "Naam";
		$kols[1]['sortcolumn'] = "L.Achternaam";
		$kn = 2;
		if ($toonadres == 1) {
			$kols[$kn]['headertext'] = "Adres";
			$kols[$kn]['columnname'] = "Adres";
			$kn++;
			
			$kols[$kn]['headertext'] = "Postcode";
			$kols[$kn]['columnname'] = "Postcode";
			$kn++;
			
			$kols[$kn]['headertext'] = "Woonplaats";
			$kols[$kn]['columnname'] = "Woonplaats";
			$kn++;
		}
		
		if ($toontelefoon == 1) {
			$kols[$kn]['headertext'] = "Telefoon";
			$kols[$kn]['columnname'] = "Telefoon";
			$kn++;
		}
		if ($toonemail == 1) {
			$kols[$kn]['headertext'] = "E-mail";
			$kols[$kn]['columnname'] = "Email";
			$kn++;
		}
		if ($currenttab2 == "Klosleden") {
			$kols[$kn]['headertext'] = "Opmerking";
			$kols[$kn]['columnname'] = "Opmerking";
			$kn++;
			$kols[$kn]['headertext'] = "Ond.";
			$kols[$kn]['type'] = "subqry";
			$kols[$kn]['subqry'] = sprintf("SELECT GROUP_CONCAT(DISTINCT O.Kode SEPARATOR '/') FROM %1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON O.RecordID=LO.OnderdeelID WHERE LO.Lid=%%d AND IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE()", TABLE_PREFIX);
			$kols[$kn]['columnname'] = "RecordID";
			$kn++;
		} else {
			if ($toonlidnummer == 1) {
				$kols[$kn]['headertext'] = "Lidnummer";
				$kols[$kn]['columnname'] = "Lidnr";
				$kols[$kn]['sortcolumn'] = "LM.Lidnr";
				$kols[$kn]['type'] = "integer";
				$kn++;
			}
			if ($toonvanaf == 1) {
				$kols[$kn]['headertext'] = "Lid vanaf";
				$kols[$kn]['columnname'] = "LIDDATUM";
				$kols[$kn]['sortcolumn'] = "LM.LIDDATUM";
				$kols[$kn]['type'] = "date";
				$kn++;
			}
			if ($currenttab2 != "Toekomstige leden" and $toonopgezegd == 1) {
				$kols[$kn]['headertext'] = "Opgezegd per";
				$kols[$kn]['columnname'] = "Opgezegd";
				$kols[$kn]['sortcolumn'] = "LM.Opgezegd";
				$kols[$kn]['type'] = "date";
				$kn++;
			}
			if ($currenttab2 == "Leden") {
				$kols[$kn]['headertext'] = "Afd.";
				$kols[$kn]['type'] = "subqry";
				$kols[$kn]['subqry'] = sprintf("SELECT GROUP_CONCAT(DISTINCT O.Kode SEPARATOR '/') FROM %1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON O.RecordID=LO.OnderdeelID WHERE O.Type='A' AND LO.Lid=%%d AND IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE()", TABLE_PREFIX);
				$kols[$kn]['columnname'] = "RecordID";
			}
			$kn++;
		}
		if (toegang("Ledenlijst/Wijzigen lid", 0, 0)) {
			$kols[$kn]['link'] = "<a href='index.php?tp=Ledenlijst/Wijzigen+lid/Algemene+gegevens&lidid=%d'>&nbsp;&nbsp;&nbsp;></a>";
			$kols[$kn]['columnname'] = "RecordID";
			$kols[$kn]['class'] = 'muteren';
			$kn++;
		}
		
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
				$rows = $i_lid->ledenlijst($sl, $_SESSION['val_groep'], fnOrderBy($kols));
			}
		}
		
		if (count($rows) > 0) {
			if (count($rows) > 1) {
				printf("<p class='aantrecords'>%d %s</p>\n", count($rows), $currenttab2);
			}
			echo("</form>\n");
			echo("</div>  <!-- Einde filter -->\n");
			
			echo(fnDisplayTable($rows, $kols, "", 0, "", "ledenlijst"));
			foreach ($rows as $row) {
				$sel_leden[] = $row->RecordID;
			}
			$_SESSION['sel_leden'] = $sel_leden;
		} else {
			echo("</form>\n");
			echo("</div>  <!-- Einde filter -->\n");
		}
	}

// debug(microtime(true) - $starttijd);
	
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
		$alleenleden = $i_ond->max("O.`Alleen leden`", $f);
	} else {
		$muteerbaar = false;
	}

	$ondrows = $i_ond->editlijst($ondtype);
	if ($_SERVER['REQUEST_METHOD'] == "POST" and isset($_POST['formname'])) {
		if ($_POST['formname'] == "muteren_leden_onderdeel") {
			$omz["Functie"] = "Functie";
			$omz["EmailFunctie"] = "Email";
			$omz["Opmerk"] = "Opmerking";
			$omz["Vanaf"] = "Vanaf";
			$omz["Opgezegd"] = "Tot_en_met";
			$rows = $i_lo->selectielijst($onderdeelid);
			foreach ($rows as $row) {
				foreach ($omz as $kolomnaam => $bascn) {
					$contr_name = $bascn . "_" . $row->RecordID;
					if (isset($_POST[$contr_name])) {
						$i_lo->update($row->RecordID, $kolomnaam, $_POST[$contr_name]);
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
			$i_lo->autogroepenbijwerken(0, 3, $oid);
			$i_lo->auto_einde($oid);
		}
	} else {
		$i_ond->controle();
	}
	
	if (isset($_GET['op']) and $_GET['op'] == "nieuwond") {
		$recid = $i_ond->add($ondtype);
		$i_ond->update($recid, "Type", $ondtype);
		$scherm = "S";
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
			if (strlen($row->MySQL) > 10 and $i_ond->controleersql($row->MySQL, 1) == false) {
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
		$kols[4]['columntitle'] = 4;

		echo("<div class='clear'></div>\n");
	
		$res = $i_ond->editlijst($ondtype, 0);
		echo(fnEditTable($res, $kols, "onderdeeledit", $ondnaammv . " muteren"));
		
		$href = sprintf("%s?tp=%s&op=nieuwond", $_SERVER['PHP_SELF'], $_GET['tp']);
		printf("<a href='%s'><button>Nieuwe %s</button></a>\n", $href, strtolower(ARRTYPEONDERDEEL[$ondtype]));
	}
	
	echo("</div>  <!-- Einde onderdelenmuteren -->\n");	

} # fnOnderdelenmuteren

function fnEigenGegevens($lidid=0) {
	global $currenttab, $currenttab2, $currenttab3, $dtfmt;
	
	$i_foto = new cls_Foto();
	$i_ond = new cls_Onderdeel();
	$i_lo = new cls_Lidond();
	
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
		$dtfmt->setPattern(DTTEXT);
		$naamlid = htmlentities((new cls_Lid())->Naam($lidid));
		$gs = (new cls_Lid())->Geslacht($lidid);
		
		$tn = "Algemeen";
		if (toegang($ct . $tn, 0, 0)) {
			$fn = fotolid($lidid, 1);
			$nl = (new cls_Lid())->Roepnaam($lidid);
			$fd = $i_foto->FotoLid($lidid);
			if ($fd !== false) {
				$xtra = sprintf("<div id='pasfoto'><img src='%s' title='Laatst gewijzigd op %s'></div>\n", $fd, $dtfmt->format(strtotime($i_foto->laatstgewijzigd)));
			} elseif (strlen($fn) > 3) {
				$xtra = sprintf("<div id='pasfoto'><img src='%s' alt='Pasfoto %s' title='Laatst gewijzigd op %s'></div>\n", $fn, $nl, $dtfmt->format(filectime(fotolid($lidid, 0))));
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
		$kols[0]['headertext'] = "Lidnummer";
		$kols[0]['columnname'] = "Lidnr";
		
		$kols[1]['headertext'] = "Vanaf";
		$kols[1]['columnname'] = "LIDDATUM";
		$kols[1]['type'] = "DTTEXT";
		
		$kols[2]['headertext'] = "Opgezegd";
		$kols[2]['columnname'] = "Opgezegd";
		$kols[2]['type'] = "DTTEXT";
		
		$kols[3]['headertext'] = "Duur";
		$kols[3]['columnname'] = "Duur";
		
		if (toegang($ct . $tn, 0, 0)) {
			$rows = (new cls_Lidmaatschap())->overzichtlid($lidid);
			if (count($rows) > 1) {
				$tabblad[$tn] = fnDisplayTable($rows, $kols, $tn . " " . $naamlid);
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
				$tabblad[$tn] = fnDisplayTable($rows, $kols, $tn . " " . $naamlid, 0, "");
			} else {
				$tabblad[$tn] = sprintf("<p class='mededeling'>%s heeft geen %s.</p>\n", $naamlid, $tn);
			}
		}
		
		$kols[0]['headertext'] = "Groep";
		$kols[0]['columnname'] = "Naam";
		
		$kols[1]['headertext'] = "Vanaf";
		$kols[1]['columnname'] = "Vanaf";
		$kols[1]['type'] = "DTTEXT";
		
		$kols[2]['headertext'] = "Tot en met";
		$kols[2]['columnname'] = "Opgezegd";
		$kols[2]['type'] = "DTTEXT";
		
		$kols[3]['headertext'] = "Duur";
		$kols[3]['columnname'] = "Duur";
		
		$tn = "Groepen";
		if ((new cls_Onderdeel())->Aantal("Type='G'") > 0 and toegang($ct . $tn, 0, 0)) {
			$rows = (new cls_Lidond())->overzichtlid($lidid, "G");
			if (count($rows) > 0) {
				$tabblad[$tn] = fnDisplayTable($rows, $kols, $tn . " " . $naamlid);
			}
		}
		
		$tn = "Kader";
		$kols[0]['headertext'] = "Onderdeel";
		if (toegang($ct . $tn, 0, 0)) {
			$rows = (new cls_Lidond())->overzichtlid($lidid, "K");
			if (count($rows) > 0) {
				$tabblad[$tn] = fnDisplayTable($rows, $kols, $tn . " " . $naamlid, 0, "", "");
			}
		}

		$tn = "Rollen";
		if (toegang($ct . $tn, 0, 0)) {
			$rows = $i_lo->overzichtlid($lidid, "R");
			if (count($rows) > 0) {
				$tabblad[$tn] = fnDisplayTable($rows, $kols, $tn . " " . $naamlid, 0, "", "Rollen");
			}
		}
		
		$tn = "Diploma's";
		$rows = (new cls_Liddipl())->overzichtlid($lidid);
		if (count($rows) > 0 and toegang($ct . $tn, 0, 0)) {
			$kols[0]['headertext'] = "Diploma";
			$kols[0]['columnname'] = "Naam";
			$kols[1]['headertext'] = "Uitgegeven door";
			$kols[1]['columnname'] = "UitgegevenDoor";
			$kols[2]['headertext'] = "Behaald op";
			$kols[2]['columnname'] = "DatumBehaald";
			$kols[2]['type'] = "DTTEXT";
			$kols[3]['headertext'] = "Gegevens";
			$kols[3]['columnname'] = "Diplomanummer";
			$kols[4]['headertext'] = "Geldig tot";
			$kols[4]['columnname'] = "GeldigTot";
			$kols[4]['type'] = "DTTEXT";
			if (count($rows) > 0) {
				$tabblad[$tn] = fnDisplayTable($rows, $kols, "Diploma's " . $naamlid);
			} else {
				$tabblad[$tn] = sprintf("<p class='mededeling'>Bij %s zijn geen diploma's geregistreerd.</p>", $naamlid);
			}
		}
		
		$tn = "Toestemmingen";
		$kols[0]['headertext'] = "Omschrijving";
		$kols[0]['columnname'] = "Naam";
		
		$kols[1]['headertext'] = "Afgegeven / Laatst bevestigd";
		$kols[1]['columnname'] = "Vanaf";
		if ($gs != "B" and $i_ond->Aantal("Type='T'") > 0 and toegang($ct . $tn, 0, 0)) {
			$rows = $i_lo->overzichtlid($lidid, "T");
			if (count($rows) > 0) {
				$tabblad[$tn] = fnDisplayTable($rows, $kols, $tn . " " . $naamlid, 0, "", "toestemmingen");
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
			
		$tn = "Presentie";
		$rows = (new cls_Aanwezigheid())->overzichtlid($lidid);
		if (count($rows) > 0 and toegang($ct . $tn, 0, 0)) {	
			if ($tn == $currenttab3) {
				$tabidx = count($tabblad);
			}			
			$tabblad[$tn] = "<table>\n";
			$tabblad[$tn] .= sprintf("<caption>Presentie %s</caption>\n", $naamlid);
			$tabblad[$tn] .= "<tr><th>Datum</th><th>Status</th></tr>\n";
			foreach ($rows as $row) {
				$o = "";
				if (strlen($row->Functie) > 0) {
					$o = $row->Functie . ": ";
				}
				$o .= ARRPRESENTIESTATUS[$row->Status];
				$dtfmt->setPattern(DTTEXTWD);
				$tabblad[$tn] .= sprintf("<tr><td>%s</td><td>%s</td></tr>\n", $dtfmt->format(strtotime($row->Datum)), $o);
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
				$kols[1]['columnname'] = "Nummer";
				$kols[2]['headertext'] = "Seizoen";
				$kols[3]['headertext'] = "Datum";
				$kols[3]['type'] = "date";
				$kols[4]['headertext'] = "Omschrijving";
				$kols[5]['headertext'] = "Bedrag";
				$kols[5]['type'] = "bedrag";
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
			$tabblad[$tn] .= "<tr><th>Wanneer</th><th>Omschrijving</th><th>Contact</th><th>Opmerking / Functie</th><th>Status</th>";
			if ($lidid == $_SESSION['lidid']) {
				$tabblad[$tn] .= "<th>In agenda</th>";
			}
			$tabblad[$tn] .= "</tr>\n";
			$dtfmt->setPattern("EEE d MMMM yyyy");
			
			foreach($rows as $row) {
				$tabblad[$tn] .= "<tr>\n";
				$tijden = $dtfmt->format(strtotime($row->Datum));
				
				if ($row->Starttijd > "00:00" and $row->Eindtijd > "00:00" and $row->MeerdereStartMomenten == 0) {
					$tijden .= sprintf("<br>\n%s tot %s uur", substr($row->Starttijd, 0, 5), substr($row->Eindtijd, 0, 5));
					
				} elseif ($row->Starttijd > "00:00") {
					$tijden .= sprintf("<br>\nom %s uur", substr($row->Starttijd, 0, 5));
				}
				$tijden = str_replace(" ", "&nbsp;", str_replace("  ", "&nbsp;", $tijden));
				if (strlen($row->Verzameltijd) >= 5 and $row->Datum >= date("Y-m-d H:i")) {
					$tijden .= sprintf("<br>\nVerzamelen om %s&nbsp;uur", substr($row->Verzameltijd, 0, 5));
				}
					
				$tabblad[$tn] .= sprintf("<td>%s</td>\n", $tijden);
				$oms = $row->Omschrijving;
				if (strlen($row->Locatie) > 1) {
					$oms .= "<br>\n" . $row->Locatie;
				}
				$tabblad[$tn] .= sprintf("<td>%s</td>\n", $oms);
				$tabblad[$tn] .= sprintf("<td>%s</td>\n", $row->Email);
				if (strlen($row->Opmerking) > 0 and strlen($row->Functie) > 0) {
					$of = sprintf("%s<br>%s", $row->Opmerking, $row->Functie);
				} elseif (strlen($row->Opmerking) > 0) {
					$of = $row->Opmerking;
				} else {
					$of = $row->Functie;
				}
				$tabblad[$tn] .= sprintf("<td>%s</td>\n", $of);
				$tabblad[$tn] .= sprintf("<td>%s</td>\n", $row->Status);
				if ($lidid == $_SESSION['lidid']) {
					if (($row->Status == "Aangemeld" or $row->Status == "Bevestigd" or $row->Status == "Ingeschreven") and $row->Datum >= date("Y-m-d H:i")) {
						$tabblad[$tn] .= sprintf("<td>%s</td>\n", fnAgendaKnop($row->Datum, $row->Eindtijd, $row->Verzameltijd, $row->Omschrijving, $row->Locatie));
					} else {
						$tabblad[$tn] .= "<td>&nbsp;</td>";
					}
				}
				$tabblad[$tn] .= "</tr>\n";
			}
			
			$tabblad[$tn] .= "</table>\n";
			$tabblad[$tn] .= "</div>  <!-- einde evenementenperlid -->";
		}

		$tn = "Logboek";
		$rows = (new cls_Logboek())->overzichtlid($lidid);
		if ($lidid > 0 and toegang($ct . $tn, 0, 0) and count($rows) > 0) {
			$kols = fnStandaardKols("logboek", 0);
			$kols[4]['headertext'] = "Uitgevoerd door";
			$tabblad[$tn] = fnDisplayTable($rows, $kols, $tn . " " .  $naamlid);
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
	global $dtfmt;

	if ($metfoto != 0 and $_SESSION['settings']['toonpasfotoindiennietingelogd'] == 0 and $_SESSION['lidid'] == 0) {
		$metfoto = 0;
	}
	
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
				$dtfmt->setPattern("EEEE d MMMM");
				$t = sprintf("Op %s is %s jarig", $dtfmt->format($dteHV), $nm);
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
	$i_lid = null;
	if (strlen($verj) > 0) {
		$verj = "<h3>Verjaardagen</h3>\n" . $verj;
		$verjfoto = "<h3>Verjaardagen</h3>\n" . $verjfoto;
	}
	if ($metfoto == 1) {
		return $verjfoto;
	} else {
		return $verj;
	}
	
} # overzichtverjaardagen

function fotolid($lidid, $metversie=0) {

	$rv = (new cls_Foto())->FotoLid($lidid);
	if ($rv == null and strlen($_SESSION['settings']['path_pasfoto']) > 5 and is_dir($_SESSION['settings']['path_pasfoto'])) {
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
			$rv .= "?v" . date("YmdHi", filectime($rv));
		}
		$rv = str_replace(BASEDIR, ".", $rv);
	}
	
	return $rv;
	
} # fotolid

function algemeenlidmuteren($lidid) {
	global $actionurl, $currenttab, $currenttab2;
	
	$i_lid = new cls_Lid();
	$i_lid->controle($lidid);
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
	
	$u = "";
	if (strlen($row->Adres) > 3 and $row->Huisnr == 0) {
		$u = "Geen huisnummer ingevoerd";
	} else {
		$pdok = pdok($row->Postcode, $row->Huisnr, $row->Huisletter, $row->Toevoeging);
		if (!isset($pdok->numFound) or $pdok->numFound == 0) {
			$u = "Adres bestaat niet";
		} elseif (isset($pdok->docs[0]->huis_nlt)) {
			$adr = $pdok->docs[0]->straatnaam . " " . $pdok->docs[0]->huis_nlt;
			if ($adr != trim($row->Adres)) {
				$i_lid->update($lidid, "Adres", $adr);
				$row = $i_lid->record($lidid);
			}
		}
	}
	$wijzvelden[] = array('label' => "Adres", 'naam' => "Adres", 'lengte' => 30, 'uitleg' => $u);
	$wijzvelden[] = array('label' => "Postcode", 'naam' => "Postcode", 'lengte' => 7);
	$wijzvelden[] = array('label' => "Woonplaats", 'naam' => "Woonplaats", 'lengte' => 22);
	$wijzvelden[] = array('label' => "Vast telefoonnummer", 'naam' => "Telefoon", 'lengte' => 21, 'uitleg' => "");
	$wijzvelden[] = array('label' => "Mobiel", 'naam' => "Mobiel", 'lengte' => 21, 'uitleg' => "");
	if ($row->GEBDATUM < date("Y-m-d", strtotime("-8 year"))) {
		$wijzvelden[] = array('label' => "E-mail", 'naam' => "Email", 'lengte' => 45, 'uitleg' => "");
	}
	if (($_SESSION['settings']['zs_incl_emailvereniging'] == 1 or $currenttab2 == "Wijzigen lid") and $row->GEBDATUM < date("Y-m-d", strtotime("-12 year")) and $sl != "K") {
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
	$f = sprintf("LO.OnderdeelID=%d", $_SESSION['settings']['rekening_groep_betaalddoor']);
	if ($currenttab2 == "Wijzigen lid" and $_SESSION['settings']['rekening_groep_betaalddoor'] > 0 and $i_lo->aantal($f) > 0) {
		$wijzvelden[] = array('label' => "Rekening betaald door", 'naam' => "RekeningBetaaldDoor", 'lengte' => 18);
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
		$i_lid->controle($lidid);
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
	printf("<label id='lblRecordID'>RecordID/LidID</label><p>%d</p>\n", $row->RecordID);
	$lidnr = $i_lm->lidnummer($row->RecordID);
	if ($lidnr > 0) {
		printf("<label id='lblLidnummer'>Lidnummer</label><p>%s</p>\n", $lidnr);
	}
	
	for ($i=0; $i < count($wijzvelden); $i++) {
		
		$jsoc = sprintf("OnBlur=\"savedata('lid', %d, this);\"", $row->RecordID, $wijzvelden[$i]['naam']);
		$dv = str_replace("\n", "<br>\n", $row->{$wijzvelden[$i]['naam']});
		
		if (($wijzvelden[$i]['naam'] == "Adres" or $wijzvelden[$i]['naam'] == "Woonplaats") and strlen($row->Postcode) >= 6) {
			
			$pdok = pdok($row->Postcode);
			
			if ($wijzvelden[$i]['naam'] == "Woonplaats" and strlen($dv) == 0 and isset($pdok->docs[0]->woonplaatsnaam)) {
				$dv = $pdok->docs[0]->woonplaatsnaam;
				$i_lid->update($lidid, "Woonplaats", $dv);
				
			} elseif ($wijzvelden[$i]['naam'] == "Adres" and strlen($dv) == 0 and isset($pdok->docs[0]->straatnaam)) {
				$dv = $pdok->docs[0]->straatnaam . " ";
			}
		}
		
		$ph = $wijzvelden[$i]['label'];
		if ($wijzvelden[$i]['label'] == "Geslacht" or $wijzvelden[$i]['label'] == "Legitimatietype" or $wijzvelden[$i]['label'] == "Rekening betaald door") {
			if ($wijzvelden[$i]['label'] == "Geslacht") {
				$an = constant("ARRGESLACHT");
				
			} elseif ($wijzvelden[$i]['label'] == "Rekening betaald door") {
				unset($an);
				$an[0] = "Lid zelf";
				foreach ($i_lo->lijst($_SESSION['settings']['rekening_groep_betaalddoor']) as $lorow) {
					$an[$lorow->Lid] = $lorow->NaamLid;
				}
			} else {
				$an = constant("ARRLEGITIMATIE");
			}
			$opt = "\n";
			foreach ($an as $key => $val) {
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
	global $actionurl, $dtfmt;;
	
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

		} else {
		
			$i_foto->add($_FILES['UploadFoto']['tmp_name'], $lidid, $ext);
/*

			$target = $ad . sprintf("Pasfoto%d.%s", $lidid, $ext);
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
						$bu_name = $adv . sprintf("Pasfoto%d_%d.%s", $lidid, date("Ymd", filectime($target)), $of_ext);
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
			*/
		}
	}

	echo("<div id='nieuwepasfoto'>\n");
	$fn = fotolid($lidid, 1);
	$fd = $i_foto->fotolid($lidid);
	$dtfmt->setPattern(DTTEXT);
	if ($fd !== false) {
		printf("<img src='%s' alt='Huidige pasfoto %s'>\n", $fd, $naamlid);
		printf("<p>Deze foto is voor het laatst op %s gewijzigd.</p>", $dtfmt->format(strtotime($i_foto->laatstgewijzigd)));
	} elseif (strlen($fn) > 4 and file_exists($fn)) {
		echo("<p>\n");
		printf("<img src='%s' alt='Huidige pasfoto %s'>\n", $fn, $naamlid);
		$fn = fotolid($lidid, 0);
		printf("Deze foto is voor het laatst op %s gewijzigd en is %dKB groot.", $dtfmt->format(filectime($fn)), filesize($fn) / 1024);
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
				if ($i_lo->zeteigenschap($lidid, $ondrow->RecordID, 1) == true) {
					$mess .= sprintf("%s is toegevoegd of opnieuw bevestigd. ", $ondrow->Naam);
				}
			} else {
				if ($i_lo->zeteigenschap($lidid, $ondrow->RecordID, 0) == true) {
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
	echo("<button type='button' onClick=\"AlleToestemming();\">Alle toestemmingen</button>");
	printf("<button type='submit' name='Bevestigen'>%s</button>\n", $tb);
	echo("</div> <!-- Einde opdrachtknoppen -->\n");

	echo("</form>\n");
	echo("</div> <!-- Einde toestemmingenmuteren -->\n");
	
	echo("<script>
			function AlleToestemming() {
				$('input[type=checkbox]').each(function () {
					\$(this).prop('checked', true);
				});
			};
	</script>\n");

	$i_ond = null;
	$i_lo = null;

} # toestemmingenmuteren

function opzegginglidmaatschap($lidid) {
	global $actionurl, $dtfmt;
	
	$i_lid = new cls_Lid($lidid);
	$i_sz = new cls_Seizoen();
	$naamlid = $i_lid->Naam();
	$minopzeggenper = new DateTime(sprintf("+%d month", $_SESSION['settings']['zs_opzegtermijn']));
	$i_sz->zethuidige(date("Y-m-d", strtotime("+4 week")));
	$eindeseizoen = new DateTime($i_sz->einddatum);
	while ($eindeseizoen < $minopzeggenper) {
		$eindeseizoen->modify("+1 year");
	}

	$dtfmt->setPattern(DTTEXT);
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$opgezegdper = new DateTime($_POST["OpzeggingPer"]);
		
		if ($opgezegdper < $minopzeggenper) {
			$opgezegdper = $minopzeggenper;
			$mess = sprintf("Er geldt een opzegtermijn van %d maand(en), hierdoor wordt de datum van opzegging %s.", $_SESSION['settings']['zs_opzegtermijn'], $dtfmt->format($opgezegdper));
			
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
		<strong>%s</strong></p>\n", $_SESSION['settings']['naamvereniging'], $dtfmt->format($opgezegdper), $_SESSION['lidnr'], $opm, $naamlid);
				
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
<textarea name='RedenOpmerking' title='Reden en/of opmerkingen' placeholder='Reden en/of opmerkingen'></textarea>
</fieldset>
<div id='opdrachtknoppen'>\n
<button type='submit' name='VerstuurOpzegging'>Verstuur opzegging</button>\n
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
		$content = str_replace("[%LIDID%]", $_SESSION['lidid'], $content);
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
	global $actionurl, $dtfmt;

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
	$dtfmt->setPattern(DTSHORT);
	foreach ($ldrows as $ldrow) {
		if (strlen($ldrow->OrgNaam) > 4) {
			$dpnm = $ldrow->OrgNaam . " - " . $ldrow->Naam;
		} elseif (strlen($ldrow->OrgNaam) > 1) {
			$dpnm = $ldrow->OrgNaam . " " . $ldrow->Naam;
		} else {
			$dpnm = $ldrow->Naam;
		}
		printf("<tr>\n<td>%s</td>\n<td>%s</td>\n\n", $ldrow->Kode, $dpnm);
		if ($ldrow->Zelfservice == 0 and $td == "ZS") {
			printf("<td>%s</td><td>%s</td><td>%s</td>", $dtfmt->format(strtotime($ldrow->DatumBehaald)), $ldrow->Diplomanummer, $dtfmt->format(strtotime($ldrow->LicentieVervallenPer)));
		} else {
			$id = sprintf("DatumBehaald_%d", $ldrow->RecordID);
			printf("<td><input type='date' id='%s' value='%s' required></td>\n", $id, $ldrow->DatumBehaald);
			$script .= sprintf("$('#%1\$s').datepicker();\n$('#%1\$s').datepicker('dateFormat', 'yy-mm-dd').datepicker('maxDate', new Date());\n$('#%1\$s').datepicker('setDate', '%3\$s').datepicker('option', 'gotoCurrent', true);\n", $id, $minbehaald, $ldrow->DatumBehaald);
			printf("<td><input type='text' id='Diplomanummer_%d' value='%s' maxlength=30></td>\n", $ldrow->RecordID, $ldrow->Diplomanummer);
			printf("<td><input type='date' id='LicentieVervallenPer_%d' value='%s'></td>\n", $ldrow->RecordID, $ldrow->LicentieVervallenPer);
		}
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
		echo("<button type='submit' name='NieuwLidmaatschap' value=1>Nieuw lidmaatschap</button>\n");
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

function fnPersoonlijkeAgenda() {
	global $dtfmt;
	
	$i_ak = new cls_Afdelingskalender();
	$i_lo = new cls_Lidond();
	$kalitem = null;
	$in = 0;
	
	$f = sprintf("AK.Datum >= CURDATE() AND AK.OnderdeelID IN (%s) AND AK.Datum <= DATE_ADD(CURDATE(), INTERVAL 5 WEEK)", $_SESSION['lidgroepen']);
	foreach ($i_ak->lijst(-1, "", $f) as $row) {
		$kalitem[$in]['datum'] = date("Y-m-d", strtotime($row->Datum));
			
		$lorow = $i_lo->record($_SESSION['lidid'], $row->OnderdeelID);
		if (isset($lorow->Starttijd) and strlen($lorow->Starttijd) > 3 and $row->Activiteit == 1) {
			$kalitem[$in]['datum'] .= " " . $lorow->Starttijd;
		}
				
		if ($row->Activiteit == 1) {
			$oms = $row->Naam;
		} else {
			$oms = "Geen " . $row->Naam;
		}
		if (strlen($row->Omschrijving) > 1) {
			$oms .= " (" . $row->Omschrijving . ")";
		}
		$kalitem[$in]['oms'] = $oms;
		$in++;
	}
	
	foreach ((new cls_Evenement())->lijst(4) as $row) {
		$i_ed = new cls_Evenement_Deelnemer();
		$edrow = $i_ed->record(0, $_SESSION['lidid'], $row->RecordID);
		if ($row->Dln == 0 or isset($edrow->RecordID)) {
			if ($row->MeerdereStartMomenten == 0) {
				$kalitem[$in]['datum'] = date("Y-m-d H:i", strtotime($row->Datum));
			} else {
				$kalitem[$in]['datum'] = date("Y-m-d", strtotime($row->Datum));
				if (isset($edrow->RecordID) and strlen($edrow->StartMoment) > 2) {
					$kalitem[$in]['datum'] .= " " . $edrow->StartMoment;
				}
			}
			$o = $row->Omschrijving;
			
			if (toegang("Evenementen", 0)) {
				$o = sprintf("<a href='%s/index.php?tp=Evenementen'>%s</a>", BASISURL, $o);
			}
			
			if ($row->Dln > 1) {
				$o .= sprintf(" (%d deelnemers)", $row->Dln);
			}
			if (isset($edrow->RecordID)) {
				$o .= ": " . ARRDLNSTATUS[$edrow->Status];
			}
			$kalitem[$in]['oms'] = $o;
			$in++;
		}
	}
	
	if ($_SESSION['lidid'] > 0 and  $_SESSION['settings']['termijnvervallendiplomasmelden'] > 0) {
		$rows = (new cls_Liddipl())->vervallenbinnenkort($_SESSION['lidid']);
		foreach ($rows as $row) {
			if ($row->VervaltPer >= date("Y-m-d", strtotime("-7 days"))) {
				$kalitem[$in]['datum'] = date("Y-m-d H:i", strtotime($row->VervaltPer));
				$kalitem[$in]['oms'] = sprintf("%s vervalt", $row->DiplOms);
				$in++;
			}
		}
	}
	
	$rv = "";
	if (isset($kalitem) and count($kalitem) > 0) {
		$rv = "<h3>Persoonlijke agenda</h3>\n<ul>\n";
		usort($kalitem, function($a, $b) {
			return $a['datum'] <=> $b['datum'];
		});
		$dtfmt->setPattern(DTPERSCAL);
		for ($in=0;$in<=9;$in++) {
			if (isset($kalitem[$in]['datum'])) {
				$dat = $dtfmt->format(strtotime($kalitem[$in]['datum']));
				$tijd = date("H:i", strtotime($kalitem[$in]['datum']));
				if ($tijd > "00:00") {
					$dat .= " (" . $tijd . ")";
				}
				$rv .= sprintf("<li>%s: %s</li>\n", $dat, $kalitem[$in]['oms']);
			}
		}
		$rv .= "</ul>\n";
	}

	return $rv;
	
}  # fnPersoonlijkeAgenda

function fnBasisgegevens($p_type) {
	
	if ($p_type == "Onderdelen") {
		$i_ond = new cls_Onderdeel();
		
		if (isset($_POST['NieuwOnderdeel'])) {
			$i_ond->add();
		}
		$i_ond->opschonen();
		$i_ond->controle();
		
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
		$kols[8]['columnname'] = "VervallenPer";
		
		$kols[9]['headertext'] = "Opschonen na";
		$kols[9]['type'] = "dagen";
		$kols[9]['columnname'] = "HistorieOpschonen";
		
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		echo(fnEditTable($ondrows, $kols, "onderdeeledit", "Muteren onderdelen"));
		echo("<div id='opdrachtknoppen'>\n");
		echo("<button name='NieuwOnderdeel' onClick='this.form.submit();'>Nieuw onderdeel</button>\n");
		echo("<input type='submit' value='Bewaren'>\n");
		echo("</div> <! =-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
		
		$i_ond = null;
		
	} elseif ($p_type == "Afdelingen") {
		$i_ond = new cls_Onderdeel();
		
		$ondrows = $i_ond->lijst(0, "O.`Type`='A'", "", 0, "IF(O.VervallenPer IS NULL, 0, 1), O.`Type`", 0);
		
		$kols[0]['headertext'] = "#";
		$kols[0]['columnname'] = "RecordID";
		$kols[0]['type'] = "pk";
		$kols[0]['readonly'] = true;
		
		$kols[2]['headertext'] = "Naam";
		$kols[2]['columnname'] = "Naam";
		
		$kols[3]['headertext'] = "Contributie<br>lid";
		$kols[3]['columnname'] = "LIDCB";
		$kols[3]['type'] = "bedrag";
	
		$kols[4]['headertext'] = "Contributie<br>jeugdlid";
		$kols[4]['columnname'] = "JEUGDCB";
		$kols[4]['type'] = "bedrag";

		$kols[5]['headertext'] = "Contributie<br>kader";
		$kols[5]['columnname'] = "FUNCTCB";
		$kols[5]['type'] = "bedrag";
	
		$kols[6]['headertext'] = "Alleen leden?";
		$kols[6]['type'] = "checkbox";
		$kols[6]['columnname'] = "Alleen leden";
		
		$kols[7]['headertext'] = "E-mailadres";
		$kols[7]['type'] = "email";
		$kols[7]['columnname'] = "CentraalEmail";
		
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		echo(fnEditTable($ondrows, $kols, "onderdeeledit", "Muteren onderdelen"));
		echo("<div id='opdrachtknoppen'>\n");
		echo("<input type='submit' value='Bewaren'>\n");
		echo("</div> <! =-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
		
		$i_ond = null;
		
	} elseif ($p_type == "Functies") {
		$i_fnk = new cls_Functie();
		$i_fnk->controle();

		if (isset($_POST['NieuweFunctie'])) {
			$i_fnk->add();
		}
		
		$fnkres = $i_fnk->basislijst("", "IF(`Vervallen per` IS NULL, 0, 1), Omschrijv", 0);
		
		$kols = null;
		$kols[0]['headertext'] = "Nummer";
		$kols[0]['readonly'] = true;
		$kols[0]['type'] = "pk";
		
		$kols[1]['headertext'] = "Omschrijving";
		$kols[1]['columnname'] = "Omschrijv";
		
		$kols[2]['headertext'] = "Afkorting";
		$kols[2]['columnname'] = "Afkorting";
		
		$kols[3]['headertext'] = "Volgnr";
		$kols[3]['columnname'] = "Sorteringsvolgorde";
		$kols[3]['type'] = "integer";
		$kols[3]['max'] = 99;
		
		if ((new cls_Onderdeel())->aantal("O.`Type`='A'") > 0) {
			$kols[4]['headertext'] = "Bij afdeling?";
			$kols[4]['columnname'] = "Afdelingsfunctie";
			$kols[4]['type'] = "checkbox";
		}
		
		$kols[5]['headertext'] = "Algemeen?";
		$kols[5]['columnname'] = "Ledenadministratiefunctie";
		$kols[5]['type'] = "checkbox";
		
		if ((new cls_Bewaking())->aantal() > 0) {
			$kols[6]['headertext'] = "Bij bewaking?";
			$kols[6]['columnname'] = "Bewakingsfunctie";
			$kols[6]['type'] = "checkbox";
		}
		
		$kols[7]['headertext'] = "Kader?";
		$kols[7]['columnname'] = "Kader";
		$kols[7]['type'] = "checkbox";
		
		$kols[8]['headertext'] = "Inval?";
		$kols[8]['columnname'] = "Inval";
		$kols[8]['type'] = "checkbox";
		
		$kols[9]['headertext'] = "Vervallen per";
		$kols[9]['columnname'] = "Vervallen per";
		$kols[9]['type'] = "date";
		
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		
		echo(fnEditTable($fnkres, $kols, "functieedit", "Muteren functies"));
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<button name='NieuweFunctie' onClick='this.form.submit();'>Nieuwe functie</button>\n");
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
		
		$i_ond = null;
		
		
	} elseif ($p_type == "Activiteiten") {
		$i_act = new cls_Activiteit();

		if (isset($_POST['NieuweActiviteit'])) {
			$i_act->add();
		}
		
		$res = $i_act->basislijst("", "Omschrijving", 0);
		
		$kols = null;
		$kols[0]['headertext'] = "#";
		$kols[0]['readonly'] = true;
		$kols[0]['type'] = "pk";
		
		$kols[1]['headertext'] = "Code";
		$kols[2]['headertext'] = "Omschrijving";
		
		$kols[3]['headertext'] = "Contributie";
		$kols[3]['type'] = "bedrag";
				
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		
		echo(fnEditTable($res, $kols, "activiteitedit", "Muteren activiteiten"));
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<button name='NieuweActiviteit' onClick='this.form.submit();'>Nieuwe activiteit</button>\n");
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
		
		$i_ond = null;

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
		
		$kols[3]['headertext'] = "Jeugdlid tot";
		$kols[3]['columnname'] = "Leeftijdsgrens jeugdleden";
		$kols[3]['type'] = "leeftijd";
		
		$kols[4]['headertext'] = "Contributie<br>lid";
		$kols[4]['columnname'] = "Contributie leden";
		$kols[4]['type'] = "bedrag";
		
		$kols[5]['headertext'] = "Contributie<br>jeugdlid";
		$kols[5]['columnname'] = "Contributie jeugdleden";
		$kols[5]['type'] = "bedrag";
		
		$kols[6]['headertext'] = "Contributie<br>kader";
		$kols[6]['columnname'] = "Contributie kader";
		$kols[6]['type'] = "bedrag";
		
		$kols[7]['headertext'] = "Betaaltermijn";
		$kols[7]['columnname'] = "BetaaldagenTermijn";
		$kols[7]['type'] = "dagen";
		
		$kols[8]['headertext'] = "&nbsp;";
		$kols[8]['columnname'] = "Nummer";
		$kols[8]['class'] = "trash";
		$kols[8]['link'] = sprintf("%s?tp=%s&op=delete&SeizoenNr=%%d", $_SERVER['PHP_SELF'], $_GET['tp']);
		$kols[8]['cond_ro'] = "aantalRek";
		
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

function presentielijst() {
	
	$ondid = $_POST['onderdeelid'] ?? 0;
	$titellijst = $_POST['titellijst'] ?? "";
	$eventid = $_POST['eventid'] ?? 0;
	
	$i_ond = new cls_Onderdeel();
	$i_ev = new cls_Evenement($eventid);
	
	if ($ondid == 0 and $eventid > 0) {
		$ondid = $i_ev->beperktotgroep;
	}
	if (strlen($titellijst) == 0 and $eventid > 0) {
		$titellijst = $i_ev->evoms;
	}
	
	echo("<div id='filter'>\n");
	$actionurl = sprintf("%s?tp=%s", $_SERVER['PHP_SELF'], $_GET['tp']);
	printf("<form method='post' action='%s'>\n", $actionurl);
	printf("<select name='onderdeelid' onChange='this.form.submit();'><option value=0>Selecteer groep ...</option>\n%s</select>\n", $i_ond->htmloptions($ondid, 1));
	printf("<input type='text' name='titellijst' class='w60' placeholder='Titel presentielijst' value='%s' onBlur='this.form.submit();'>", $titellijst);
	printf("<select name='eventid' onChange='this.form.submit();'><option value=0>Gekoppeld aan ...</option>\n%s</select>\n", $i_ev->htmloptions($eventid));
	echo("</form>\n");
	echo("</div> <!-- Einde filter -->\n");
	
	if ($ondid > 0) {
		$i_lid = new cls_Lid();
		$i_ed = new cls_Evenement_Deelnemer($eventid);
		echo("<div id='presentielijst'>\n");
		
		$aant = 0;
		echo("<table>\n");
		if (strlen($titellijst) > 1)  {
			printf("<thead>\n<tr><th colspan=2>%s</th></tr></thead>\n", $titellijst);
		}
		echo("<tbody>\n");
		$rows = $i_lid->ledenlijst(1, $ondid);
		foreach ($rows as $row) {
			if ($aant == 0) {
				echo("<tr>");
			} elseif ($aant/2 == intval($aant/2)) {
				echo("</tr>\n<tr>");
			}
			$nm = $row->Naam_lid;
			$stat = "";
			$edrow = $i_ed->record(-1, $row->RecordID, $eventid);
			if (isset($edrow->Functie) and strlen($edrow->Functie) > 0) {
				$nm .= " (" . $edrow->Functie . ")";
			}
			if (isset($edrow->Status)) {
				$stat = ": " . ARRDLNSTATUS[$edrow->Status];
			}
			
			printf("<td>%d %s%s</td>", $row->Lidnr, $nm, $stat);
			$aant++;
		}
		
		echo("</tbody>\n");
		echo("</table>\n");
		echo("</div> <!-- Einde presentielijst -->\n");
		
	}
	
}  # presentielijst

function ncsopgave() {
	$i_ond = new cls_Onderdeel();
	
	$opgaveper = $_POST['opgaveper'] ?? date("Y") . "-01-01";
	if (isset($_POST['ncsafd']) and $_POST['ncsafd'] == "1") {
		$ncsafd = 1;
	} else {
		$ncsafd = 0;
	}
	
	$query = "SELECT DISTINCT L.Voorletter, L.Roepnaam, CONCAT(IF(Length(L.TUSSENV) > 0, CONCAT(L.TUSSENV,  ' '), ''), L.Achternaam) AS Achternm, L.Adres, L.Postcode, L.Woonplaats, L.GEBDATUM ";
	if ($ncsafd == 0) {
		$query .= sprintf("FROM %1\$sLid AS L INNER JOIN %1\$sLidmaatschap AS LM ON L.RecordID=LM.Lid WHERE LM.LIDDATUM <= '%2\$s' AND IFNULL(LM.Opgezegd, '9999-12-31') >= '%2\$s'", TABLE_PREFIX, $opgaveper);
	} else {
		$query .= sprintf("FROM %1\$sLid AS L INNER JOIN (%1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON LO.OnderdeelID=O.RecordID) ON LO.Lid=L.RecordID
						   WHERE LO.Vanaf <= '%2\$s' AND IFNULL(LO.Opgezegd, '9999-12-31') >= '%2\$s' AND (O.ORGANIS=2 OR O.TYPE='B')", TABLE_PREFIX, $opgaveper);
	}
	$query .= " ORDER BY L.Achternaam, L.Voorletter;";
   
	$rows = $i_ond->execsql($query)->fetchAll();
	
	echo("<div id='filter'>\n");
	$actionurl = sprintf("%s?tp=%s", $_SERVER['PHP_SELF'], $_GET['tp']);
	printf("<form method='post' action='%s'>\n", $actionurl);
	printf("<label>Ledenopgave per</label><input type='date' name='opgaveper' value='%s' onBlur='this.form.submit();'>\n", $opgaveper);
	if ($i_ond->aantal("O.ORGANIS=2") > 0) {
		printf("<label>Alleen leden van NCS-afdelingen?</label><input type='checkbox' name='ncsafd' value='1'%s onClick='this.form.submit();'>\n", checked($ncsafd));
	}
	printf("<p class='aantrecords'>%d Leden</p>\n", count($rows));
	echo("</form>\n");
	echo("</div> <!-- Einde filter -->\n");

	$kols[0]['columnname'] = "Voorletter";
	$kols[0]['headertext'] = "Voorletters";
	$kols[1]['columnname'] = "Roepnaam";
	$kols[1]['headertext'] = "Voornaam";
	$kols[2]['columnname'] = "Achternm";
	$kols[2]['headertext'] = "Achternm";
	$kols[3]['columnname'] = "Adres";
	$kols[3]['headertext'] = "Adres";
	$kols[4]['columnname'] = "Postcode";
	$kols[4]['headertext'] = "Postcode";
	$kols[5]['columnname'] = "Woonplaats";
	$kols[5]['headertext'] = "Woonplaats";
	$kols[6]['columnname'] = "GEBDATUM";
	$kols[6]['headertext'] = "Geboortedatum";
	$kols[6]['type'] = "dateshort";
	
	echo(fnDisplayTable($rows, $kols));
	
}  # ncsopgave

function sportlink() {
	$i_lid = new cls_Lid();
	fnDispMenu(2);
	
	$Kolomhoofd = array("Relatiecode", "Tussenvoegsels", "Achternaam", "Roepnaam", "Voorletters", "Geslacht", "Geboortedatum", "Geboorteplaats", "Postcode", "Huisnummer", "Toevoeging");
	
	$actionurl = sprintf("%s?tp=%s", $_SERVER['PHP_SELF'], $_GET['tp']);
	printf("<form method='post' action='%s'>\n", $actionurl);
	echo("<label>Export uit Sportlink</label><textarea name='exportsl' id='sportlinkdata'></textarea><p>Plak hierboven de export uit Sportlink, puntkomma-gescheiden en met onderstaande kolommen.</p>\n");
	
	echo("<button type='submit'>Toon resultaat</button>\n");
	echo("</form>\n");
	
	echo("<table>\n<tr>");
	for ($knr=0;$knr < count($Kolomhoofd);$knr++) {
		printf("<th>%s</th>", $Kolomhoofd[$knr]);
	}
	echo("<th>Verschil</th></tr>\n");
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$data = $_POST['exportsl'];
		
		$separator = "\r\n";
		$line = strtok($data, $separator);
		
		while ($line !== false) {
			$kolommen = explode(";", $line);
			if (strlen(trim($kolommen[0])) == 7) {
				echo("<tr>");
				for ($knr=0;$knr < count($Kolomhoofd);$knr++) {
					printf("<td>%s</td>", $kolommen[$knr]);
				}
			
				$lidrows = $i_lid->lidredned($kolommen[0]);
				$verschil = "";
				$arrSLID[] = $kolommen[0];
				if (count($lidrows) == 0) {
					$verschil = "Lokaal: geen lid RedNed";
				} else {
					$r = $lidrows[0];
	//				debug($r->RecordID);
					$i_lid->controle($r->RecordID);
					if (trim($kolommen[1]) !== trim($r->TUSSENV)) {
						$verschil = $Kolomhoofd[1] . " lokaal: " . $r->TUSSENV;
					
					} elseif ($kolommen[2] !== $r->Achternaam and $kolommen[2] !== trim($r->Anaam2)) {
						$verschil = $Kolomhoofd[2] . " lokaal: " . $r->Achternaam;
					
					} elseif (trim($kolommen[3]) !== trim($r->Roepnaam)) {
						$verschil = $Kolomhoofd[3] . " lokaal: " . $r->Roepnaam;
					
					} elseif (trim($kolommen[4]) !== trim($r->Voorletter)) {
						$verschil = $Kolomhoofd[4] . " lokaal: " . $r->Voorletter;
					
					} elseif ($kolommen[5] !== $r->Geslacht) {
						$verschil = $Kolomhoofd[5] . " is ongelijk";
					
					} elseif (trim($kolommen[6]) !== date("d-m-Y", strtotime($r->GEBDATUM))) {
						$verschil = $Kolomhoofd[6] . " lokaal: " . $r->GEBDATUM;
					
					} elseif (strtoupper(trim($kolommen[7])) !== strtoupper(trim($r->GEBPLAATS))) {
						$verschil = $Kolomhoofd[7] . " lokaal: " . $r->GEBPLAATS;
					
					} elseif (strtoupper(trim($kolommen[8])) !== strtoupper(trim($r->Postcode))) {
						$verschil = $Kolomhoofd[8] . " lokaal: " . $r->Postcode;
					
					} elseif (intval($kolommen[9]) !== intval($r->Huisnr)) {
						$verschil = $Kolomhoofd[9] . " lokaal: " . $r->Huisnr;
					
					} elseif (strtoupper(trim($kolommen[10])) !== strtoupper(trim($r->Toev))) {
						$verschil = $Kolomhoofd[10] . " lokaal: " . $r->Toev;
					}
				}
				printf("<td>%s</td>", $verschil);
				echo("</tr>\n");
			}
			$line = strtok( $separator );
		}
		
		$lidrows = $i_lid->lidredned();
		foreach ($lidrows as $lidrow) {
			if (in_array($lidrow->RelnrRedNed, $arrSLID) == false) {
				$i_lid->controle($lidrow->RecordID);
				printf("<tr><td></td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>Onbekend in Sportlink</td></tr>\n", 				
				$lidrow->TUSSENV, $lidrow->Achternaam, $lidrow->Roepnaam, $lidrow->Voorletter, $lidrow->Geslacht, date("d-m-Y", strtotime($lidrow->GEBDATUM)), $lidrow->GEBPLAATS, $lidrow->Postcode, $lidrow->Huisnr, $lidrow->Toev);
			}
		}
		
	}
	echo("</table>\n");
	
}  # Sportlink

function jubilarissen() {
	
	$i_lid = new cls_Lid();
	$i_ond = new cls_Onderdeel();
	
	$datumper = $_POST['datumper'] ?? date("Y-m-d");
	$maandenterug = $_POST['maandenterug'] ?? 12;
	$jubileumjaren = $_SESSION['settings']['jubileumjaren'] ?? "12.5;25;50";
	$kaderjubileumjaren = $_SESSION['settings']['kaderjubileumjaren'] ?? "5;12.5;25;50";
	$jubileumjaren = str_replace(",", ".", $jubileumjaren);
	$kaderjubileumjaren = str_replace(",", ".", $kaderjubileumjaren);

	echo("<div id='jubilarisoverzicht'>\n");
	echo("<div id='filter'>\n");
	$actionurl = sprintf("%s?tp=%s", $_SERVER['PHP_SELF'], $_GET['tp']);
	printf("<form method='post' action='%s'>\n", $actionurl);
	printf("<label>Datum per</label><input type='date' value='%s' name='datumper'>", $datumper);
	printf("<label>Maanden terug</label><input type='number' value='%s' name='maandenterug' min=3 max=99 class='num2'>", $maandenterug);
	echo("<div class='clear'></div>\n");
	printf("<label>Jubileumjaren</label><input type='text' name='jubileumjaren' id='jubileumjaren' value='%s' onBlur='saveparam(this);'><p>Puntkomma-gescheiden</p>\n", $jubileumjaren);
	echo("<div class='clear'></div>\n");
	printf("<label>Groep met kader</label><select id='kaderonderdeelid' onChange='saveparam(this);'><option value=-1>Geen</option>\n%s</select>\n", $i_ond->htmloptions($_SESSION['settings']['kaderonderdeelid'], 1, "G"));
	printf("<label>Jubileumjaren voor kader</label><input type='text' name='jubileumjaren' id='kaderjubileumjaren' value='%s' onBlur='saveparam(this);'><p>Puntkomma-gescheiden</p>\n", $kaderjubileumjaren);
	
	echo("<div class='clear'></div>\n");
	
	echo("<button type='submit'>Toon resultaat</button>\n");
	echo("</form>\n");
	echo("</div> <!-- Einde filter -->\n");
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
	
		$rows = $i_lid->jubilarissen($datumper);
		echo("<table>\n");
		echo("<thead><tr><th>Naam lid</th><th>Lidmaatschap</th><th>Kaderlidmaatschap</th></tr></thead>\n");
		foreach($rows as $row) {
			$jub = "";
			$kadjub = "";
			foreach (explode(";", $jubileumjaren) as $j) {
				$m = floatval($j) * 12;
				if ($row->LengteLidmaatschap >= $m and $row->LengteLidmaatschap < ($m+$maandenterug)) {
					$jub = $j;
				}
			}
			if ($row->EindeKaderlidmaatschap > date("Y-m-d", strtotime("-12 month"))) {
				foreach (explode(";", $kaderjubileumjaren) as $j) {
					$m = floatval($j) * 12;
					if ($row->LengteKaderlidmaatschap >= $m and $row->LengteKaderlidmaatschap < ($m+$maandenterug)) {
						$kadjub = $j;
					}
				}
			}
			if (strlen($jub) > 0 or strlen($kadjub) > 0) {
				printf("<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n", $row->Naam, $jub, $kadjub);
			}
		}
		echo("</table>\n");
	}
	echo("</div> <!-- jubilarisoverzicht -->\n");

}  # jubilarissen

function pdok($p_postcode, $p_huisnr=0, $p_letter="", $p_toev="") {
	
	$p_postcode = strtoupper(trim($p_postcode));
	$p_postcode = str_replace(" ", "", $p_postcode);
	$p_letter = strtoupper(trim($p_letter));

	if (strlen($p_postcode) == 6) {
		$curl= curl_init();
		curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
		$url = sprintf("https://geodata.nationaalgeoregister.nl/locatieserver/free?fq=postcode:%s", $p_postcode);
		if ($p_huisnr > 0) {
			$url .= sprintf("&fq=huisnummer:%d", $p_huisnr);
			if (strlen($p_letter) > 0) {
				$url .= "&fq=huisletter:" . $p_letter;
			}
			if (strlen($p_toev) > 0) {
				$url .= "&fq=huisnummertoevoeging:" . $p_toev;
			}
		}
		curl_setopt($curl, CURLOPT_URL, $url);
		$respdok = curl_exec($curl);
		if (isset(json_decode($respdok)->response)) {
			$pdok = json_decode($respdok)->response;
		} else {
			$pdok = false;
		}
		curl_close($curl);
		
		return $pdok;
	} else {
		return false;
	}
	
}  # pdok

function IsIBANgoed($IBAN, $LeegIsGoed=1) {
   $retval = false;
   
   if (strlen($IBAN) == 0 and $LeegIsGoed == 1) {
		$retval = true;
      
   } elseif (strlen($IBAN) == 18) {
		$IBAN = strtoupper(str_replace(" ", "", $IBAN));
		$landcode = substr($IBAN, 0, 2);
		$controlegetal = intval(substr($IBAN, 2, 2));
		$reknr = substr($IBAN, 4);
		
		$tecontroleren = $reknr . $landcode . "00";
		
		$controlestring = "";
		for ($t=0; $t < strlen($tecontroleren); $t++) {
			$c = substr($tecontroleren, $t, 1); 
			if ($c >= "A" and $c <= "Z") {
				$o = ord($c) - 55;
			} else {
				$o = $c;
			}
			$controlestring .= "" . $o;
		}
		
		$checksum = intval(substr($controlestring, 0, 1));
		for ($pos = 1; $pos < strlen($controlestring); $pos++) {
			$checksum *= 10;
			$checksum += intval(substr($controlestring, $pos,1));
			$checksum %= 97;
		}

		$retval = ((98-$checksum) == $controlegetal);
   }
   
   return $retval;

} # IsIBANgoed

?>
