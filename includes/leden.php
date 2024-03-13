<?php

function fnLedenlijst() {
	global $wherelidond, $currenttab2, $currenttab3;
	
	$starttijd = microtime(true);
	$i_lid = new cls_Lid();
	$i_lo = new cls_Lidond();
	$i_ins = new cls_Inschrijving();
	$i_ond = new cls_Onderdeel();
	$i_act = new cls_Activiteit();
	$i_el = new cls_Eigen_lijst();
	$i_gr = new cls_Groep();

	$i_lo->auto_einde(-1, 480);
	$i_lo->autogroepenbijwerken(-1, 480);
	
	fnDispMenu(2);
	
	$_SESSION['val_groep'] = $_SESSION['val_groep'] ?? 0;
	if (isset($_POST['lbGroepFilter'])) {
		$_SESSION['val_groep'] = $_POST['lbGroepFilter'];
	}
	
	if (isset($_GET['p_lidid']) and $_GET['p_lidid'] > 0 and isset($_GET['op']) and $_GET['op'] == "verwijder") {
		$i_lid->update($_GET['p_lidid'], "Verwijderd", date("Y-m-d"));
	}
	
	if ($currenttab2 == "Nieuw (klos)lid") {
		fnNieuwLid();
		
	} elseif ($currenttab2 == "Wachtlijst") {
		wachtlijst();
		
	} elseif ($currenttab2 == "Instellingen") {
		instellingenledenmuteren();

	} elseif ($currenttab2 == "Logboek") {

		$rows = (new cls_Logboek())->lijst(6, 1, 0, "TypeActiviteitSpecifiek < 30", "", 750);

		if (count($rows) > 0) {
			echo("<div id='filter'>\n");
			echo("<input id='tbOmsFilter' placeholder='Tekstfilter' OnKeyUp=\"fnFilter('logboek', this);\" title='Tekstfilter'>");
			printf("<p class='aantrecords'>%d rijen</p>\n", count($rows));
			echo("</div> <!-- Einde filter -->\n");
			echo(fnDisplayTable($rows, 'logboek', "Logboek lidgegevens", 0, "", "logboek"));
		} else {
			echo("<p>Er zijn geen mutaties bekend.</p>\n");
		}
		
	} else {
		
		if ($currenttab2 == "Klosleden") {
			$sl = 4;
		} elseif ($currenttab2 == "Leden") {
			$sl = 1;
		} elseif ($currenttab2 == "Voormalig leden" or $currenttab2 == "Oud-leden") {
			$sl = 3;
		} elseif ($currenttab2 == "Toekomstige leden") {
			$sl = 2;
		} else {
			$sl = 0;
		}
		
		$kols[1]['sortcolumn'] = "L.Achternaam";
		$kols[3]['sortcolumn'] = "CONCAT(L.Postcode, CONVERT(L.Huisnr, VARCHAR(8)), L.Huisletter)";
		$kols[7]['sortcolumn'] = "L.GEBDATUM";
		$kols[10]['sortcolumn'] = "LM.Lidnr";
		$kols[11]['sortcolumn'] = "LM.LIDDATUM";
		$kols[12]['sortcolumn'] = "LM.Opgezegd";
			
		$rows = $i_lid->ledenlijst($sl, $_SESSION['val_groep'], fnOrderBy($kols));
		
		$arrCB = array("telefoon", "email", "geboren");
		if (toegang("Woonadres_tonen", 0, 0)) {
			$arrCB[] = "adres";
		}
		if ($currenttab2 != "Klosleden") {
			$arrCB[] = "lidnummer";
			$arrCB[] = "vanaf";
			if (count($rows) > 0 and strlen(max(array_column($rows, "Opgezegd"))) > 0) {
				$arrCB[] = "opgezegd";
			}
		}
		if (count($rows) > 0 and strlen(max(array_column($rows, "Opmerking"))) > 0) {
			$arrCB[] = "opmerking";
		}
		
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
		
		printf("<form method='post' id='filter' class='form-check form-switch' action='%s?%s'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
		echo("<input type='text' name='tbTekstFilter' id='tbTekstFilter' placeholder='Tekstfilter' OnKeyUp=\"fnFilter('ledenlijst', this);\" title='Tekstfilter'>\n");

		if ($currenttab2 == "Leden") {
			$options = $i_ond->htmloptions($_SESSION['val_groep']);
			if ($i_act->aantal() > 0) {
				$options .= "<option value=0 disabled>--- Activiteiten --</option>\n";
				$options .= $i_act->htmloptions($_SESSION['val_groep']);
			}
			if (count($i_el->lijst(3)) > 0) {
				$options .= "<option value=0 disabled>--- Eigen lijsten --</option>\n";
				$options .= $i_el->htmloptions($_SESSION['val_groep'], 3);
			}
			printf("<select id='lbgroepfilter' name='lbGroepFilter' class='form-select form-select-sm' title='Filter op onderdeel' onchange='this.form.submit();'>\n<option value=0>Filter op onderdeel</option>\n%s</select>\n", $options);
		}
//		echo("<div class='form-check form-switch'>\n");
		if (in_array("adres", $arrCB)) {
			printf("<label class='form-check-label'><input type='checkbox' class='form-check-input'  name='toonadres' title='Toon adres'%s onClick='this.form.submit();'>Adres</label>\n", checked($toonadres));
		} else {
			$toonadres = 0;
		}
		printf("<label class='form-check-label'><input type='checkbox' class='form-check-input' name='toontelefoon' title='Toon telefoon'%s onClick='this.form.submit();'>Telefoon</label>\n", checked($toontelefoon));
		printf("<label class='form-check-label'><input type='checkbox' class='form-check-input' name='toonemail' title='Toon e-mail'%s onClick='this.form.submit();'>E-mail</label>\n", checked($toonemail));
		
		printf("<label class='form-check-label'><input type='checkbox' class='form-check-input' name='toongeboren' title='Toon e-mail'%s onClick='this.form.submit();'>Geboren</label>\n", checked($toongeboren));
		
		if (in_array("lidnummer", $arrCB)) {
			printf("<label class='form-check-label'><input type='checkbox' class='form-check-input' name='toonlidnummer' title='Toon lidnummer'%s onClick='this.form.submit();'>Lidnr</label>\n", checked($toonlidnummer));
		} else {
			$toonlidnummer = 0;
		}
		if (in_array("vanaf", $arrCB)) {
			printf("<label class='form-check-label'><input type='checkbox' class='form-check-input' name='toonvanaf' title='Toon vanaf'%s onClick='this.form.submit();'>Vanaf</label>\n", checked($toonvanaf));
		} else {
			$toonvanaf = 0;
		}
		if (in_array("opgezegd", $arrCB)) {
			printf("<label class='form-check-label'><input type='checkbox' class='form-check-input' name='toonopgezegd' title='Toon opgezegd'%s onClick='this.form.submit();'>Opgezegd</label>\n", checked($toonopgezegd));
		} else {
			$toonopgezegd = 0;
		}
		
		if (in_array("opmerking", $arrCB)) {
			printf("<label class='form-check-label'><input type='checkbox' class='form-check-input' name='toonopmerking' title='Toon opmerking'%s onClick='this.form.submit();'><p>Opmerking</p></label>\n", checked($toonopmerking));
		} else {
			$toonopmerking = 0;
		}
		
		if (toegang("Ledenlijst/Overzicht lid", 0, 0)) {
			$l = "index.php?tp=Ledenlijst/Overzicht+lid&lidid=%d";
			$kols[0] = array('columnname' => "RecordID", 'link' => $l, 'class' => "detailslid");
		}
		$kols[1]['columnname'] = "NaamLid";
		$kols[1]['headertext'] = "Naam";
		
		if ($toonadres == 1) {
			$kols[2]['columnname'] = "Adres";
			$kols[3]['columnname'] = "Postcode";
			$kols[4]['columnname'] = "Woonplaats";
		}
		
		if ($toontelefoon == 1) {
			$kols[5]['columnname'] = "Telefoon";
		}
		if ($toonemail == 1) {
			$kols[6] = ['columnname' => "Email", 'headertext' => "E-mail", 'type' => "email"];
		}
		if ($toongeboren == 1) {
			$kols[7] = ['columnname' => "GEBDATUM", 'headertext' => "Geb. datum", 'type' => "date", 'sortcolumn' => "L.GEBDATUM"];
		}
		if ($toonopmerking == 1) {
			$kols[8]['columnname'] = "Opmerking";
		}
		if ($currenttab2 == "Klosleden") {
			$sq = sprintf("SELECT GROUP_CONCAT(DISTINCT O.Kode SEPARATOR '/') FROM %1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON O.RecordID=LO.OnderdeelID WHERE LO.Lid=%%d AND IFNULL(LO.Opgezegd, CURDATE()) >= CURDATE()", TABLE_PREFIX);
			$kols[] = array('columnname' => "RecordID", 'headertext' => "Ond.", 'type' => "subqry", 'class' => "onderdelen", 'subqry' => $sq);
			
			$l = sprintf("%s?tp=%s&op=verwijder&p_lidid=%%d", $_SERVER['PHP_SELF'], $_GET['tp']);
			$kols[] = array('columnname' => "RecordID", 'headertext' => "&nbsp;", 'link' => $l, 'class' => "trash");
			
		} else {
			if ($toonlidnummer == 1) {
				$kols[10] = ['columnname' => "Lidnr", 'headertext' => "Lidnummer", 'type' => "integer", 'sortcolumn' => "LM.Lidnr"];
			}
			if ($toonvanaf == 1) {
				$kols[11] = ['columnname' => "LIDDATUM", 'headertext' => "Lid vanaf", 'type' => "date", 'sortcolumn' => "LM.LIDDATUM"];
			}
			if ($toonopgezegd == 1) {
				$kols[12] = ['columnname' => "Opgezegd", 'headertext' => "Opgezegd per", 'type' => "date", 'sortcolumn' => "LM.Opgezegd"];
			}
			if ($currenttab2 == "Leden") {
				$sq = sprintf("SELECT GROUP_CONCAT(DISTINCT O.Kode SEPARATOR '/') FROM %1\$sLidond AS LO INNER JOIN %1\$sOnderdl AS O ON O.RecordID=LO.OnderdeelID WHERE (O.Type='A' OR O.Kader=1) AND LO.Lid=%%d AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE()", TABLE_PREFIX);
				$kols[] = ['columnname' => "RecordID", 'headertext' => "Afd./Kader", 'type' => "subqry", 'subqry' => $sq, 'class' => "afdkader"];
			}
		}
		if (toegang("Ledenlijst/Wijzigen lid", 0, 0)) {
			$kols[] = ['columnname' => "RecordID", 'link' => "index.php?tp=Ledenlijst/Wijzigen+lid/Algemene+gegevens&lidid=%d", 'class' => 'muteren'];
		}
				
		if (count($rows) > 1) {
			printf("<p class='aantrecords'>%d %s</p>\n", count($rows), $currenttab2);
		}
//		echo("</div>  <!-- Einde form-check form-switch -->\n");
		echo("</form>\n");
		
		if (count($rows) > 0) {		
			echo(fnDisplayTable($rows, $kols, "", 0, "", "ledenlijst"));
			foreach ($rows as $row) {
				$sel_leden[] = $row->RecordID;
			}
			$_SESSION['sel_leden'] = $sel_leden;
			echo("<script>
					$(document).ready(function() {
						el = $('#tbTekstFilter');
						fnFilter('ledenlijst', el.val());
					});
				  </script>\n");
		} else {
			echo("</form>\n");
		}
	}

// debug(microtime(true) - $starttijd);
	
} # fnLedenlijst

function fnWijzigen($p_lidid, $actie="") {
	global $currenttab, $currenttab2, $currenttab3, $actionurl;
	
	$i_lid = new cls_Lid($p_lidid);
	
	if ($currenttab == "Zelfservice") {
		$lidid = $_SESSION['lidid'];
	} elseif ($p_lidid <= 0) {
		$mess = "Er is geen lid geselecteerd.";		
		$lidid = 0;
	} elseif ($i_lid->lidid == 0) {
		$mess = sprintf("LidID %d bestaat niet.", $p_lidid);		
		$lidid = 0;
	} else {
		$lidid = $i_lid->lidid;
	}
	
	if ($currenttab == "Zelfservice") {
		$xtra_param = "";
		$actionurl = sprintf("%s?tp=%s", $_SERVER['PHP_SELF'], $_GET['tp']);
	} else {
		$xtra_param = sprintf("lidid=%d", $lidid);
		$actionurl = sprintf("%s?tp=%s&lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	}
	
	if ($lidid > 0) {
		$naamlid = $i_lid->naam();
		fnDispMenu(2, $xtra_param);
		if ($currenttab != "Zelfservice") {
			fnDispMenu(3, $xtra_param);
		}

		if ($actie == "Algemene gegevens") {
			algemeenlidmuteren($lidid);
			
		} elseif ($currenttab3 == "Financieel") {
			financieellidmuteren($lidid);
			
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
			
			if (strlen($mm) > 0) {
				printf("<h3>Bijzonderheden %s</h3>\n", (new cls_Lid())->Naam($lidid));
				printf("<form method='post' id='bijzonderhedenmuteren' action='%s'>\n", $actionurl);
				foreach (explode(",", $mm) as $kodesoort) {
					$namevar = "Memo_" . $kodesoort;
					if (isset(ARRSOORTMEMO[$kodesoort])) {
						$curval = $i_Mm->inhoud($lidid, $kodesoort);
						echo("<div class='form-floating'>\n");
						printf("<textarea id='%1\$s' name='%1\$s' class='form-control' placeholder='%3\$s' onChange='this.form.submit();'>%2\$s</textarea>\n", $namevar, $curval, ARRSOORTMEMO[$kodesoort]);
						printf("<label for='%s'>%s</label>", $namevar, ARRSOORTMEMO[$kodesoort]);
						echo("</div>\n");
					}
				}
				echo("</form>\n");
			} else {
				echo("<p class='mededeling'>Er zijn geen bijzonderheden die hier gemuteerd kunnen worden.</p>");
			}
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
				} elseif ($_POST['pw_nieuw'] != $_POST['pw_herhaal']) {
					$mess = "De nieuwe wachtwoorden zijn niet aan elkaar gelijk.";
				} else {
					(new cls_Login())->wijzigenwachtwoord($_POST['pw_nieuw'], $_POST['pw_oud'],$_POST['pw_herhaal'], $lidid);
				}
				if (strlen($mess) > 0) {
					printf("<p class='mededeling'>%s</p>\n", $mess);
				}
				echo("<p><a href='/'>Klik hier om verder te gaan.</a></p>\n");
				
			} else {
				printf("<form method='post' id='profielwijzigen' action='%s'>\n", $actionurl);
				printf("<fieldset>
				<h3>Wijzigen wachtwoord</h3>
				<label class='form-label'>Login:</label><input type='text' title='Login' name='ingelogdlogin' value='%1\$s' readonly='readonly'>
				<label class='form-label'>Oude wachtwoord:</label><input type='password' class='w%2\$d' name='pw_oud' title='Huidige wachtwoord' maxlength=%2\$d>
				<label class='form-label'>Nieuw wachtwoord:</label><input type='password' class='w%2\$d' name='pw_nieuw' title='Nieuw wachtwoord' maxlength=%2\$d>
				<label class='form-label'>Herhaal wachtwoord:</label><input type='password' class='w%2\$d' name='pw_herhaal' title='Herhaal nieuw wachtwoord' maxlength=%2\$d>", $_SESSION['username'], $_SESSION['settings']['wachtwoord_maxlengte']);
				
				printf("</fieldset>
				%s
				<div id='opdrachtknoppen'>\n
				<button type='submit' class='%s'>Wijzigen</button>\n
				</div> <!-- Einde opdrachtknoppen -->\n
				</form>\n", fneisenwachtwoord(), CLASSBUTTON);
			}	
		} else {
			$mess = sprintf("fnWijzigen: actie '%s' bestaat niet", $actie);
			debug($mess);
		}
	} else {
		printf("<p class='mededeling'>%s</p>\n", $mess);
	}
} # fnWijzigen

function fnWieiswie($actie, $metfoto=1) {
	global $ldl, $lididtestusers, $dtfmt;
	
	$i_lo = new cls_Lidond();
	
	$txt = "<div class='wieiswie'>\n";
	$vo = "";
	if ($actie == "Onderscheidingen") {
		$lijst = $i_lo->lijst(0, 1, "O.Naam, LO.Vanaf", "", "O.TYPE='O'");
		
	} elseif ($actie == "Afdelingskader") {
		$lijst = $i_lo->lijst(0, 6, "O.Naam, F.Sorteringsvolgorde, F.Omschrijv");
		
	} elseif ($actie == "Overig") {
		$lijst = $i_lo->lijst(0, 7, "O.Type, O.Naam");
		
	} else {
		// Kader of Verenigingskader
		$lijst = $i_lo->lijst(-1, 5, "O.Type, O.Naam, F.Sorteringsvolgorde, F.Omschrijv");
	}
	
	$dtfmt->setPattern("MMMM yyyy");
	if (($metfoto == 1) and ($_SESSION['settings']['toonpasfotoindiennietingelogd'] == 1 or strlen($_SESSION['username']) > 5)) {
		foreach ($lijst as $row) {
			$i_lo->vulvars($row->RecordID);
			if ($vo != $row->OndNaam) {
				if (strlen($vo) > 0) {
					$txt .= "</div> <!-- Einde row onderdeel -->\n";
				}
				$txt .= "<div class='row'>\n";
				if (isValidMailAddress($i_lo->i_ond->email, 0)) {
					$txt .= sprintf("<h2 class='onderdeelnaam'>%s </h2><h2 class='onderdeelemail'>%s</h2>\n", $i_lo->i_ond->naam, fnDispEmail($i_lo->i_ond->email, $i_lo->i_ond->naam, 1));
				} else {
					$txt .= sprintf("<h2 class='onderdeelnaam'>%s</h2><h2 class='onderdeelemail'></h2>\n", $i_lo->i_ond->naam);
				}
				$vo = $i_lo->i_ond->naam;
			} 
			$ln = htmlentities($row->NaamLid);
			if (isValidMailAddress($i_lo->email, 0)) {
				$email = fnDispEmail($i_lo->email, $row->NaamLid, 1);
			} elseif (isValidMailAddress($i_lo->i_lid->emailvereniging, 0)) {
				$email = fnDispEmail($row->EmailVereniging, $row->NaamLid, 1);
			} else {
				$email = "";
			}
			if (strlen($row->FunctieOms) > 1) {
				$func = $row->FunctieOms;
				if (strlen($row->Opmerk) > 0) {
					$func .= " &amp; " .  $row->Opmerk;
				}
			} else {
				$func = $row->Opmerk;
			}
			
			$fd = (new cls_Foto(-1, $row->LidID))->fotodata;
			if ($actie == "Onderscheidingen" and strlen($fd) > 3) {
				$txt .= sprintf("<div class='kaartje'><img class='rounded-circle' src='%1\$s' alt='Pasfoto %2\$s'>\n<p class='naamkaderlid'>%2\$s</p>\n<p>vanaf %3\$s</p>\n", $fd, $ln, $dtfmt->format(strtotime($row->Vanaf)));
			} elseif ($actie == "Onderscheidingen") {
				$txt .= sprintf("<div class='kaartje'><p class='naamkaderlid'>%s</p>\n<p>vanaf %s</p>\n", $ln, $dtfmt->format(strtotime($row->Vanaf)));
			} elseif (strlen($fd) > 3) {
				$txt .= sprintf("<div class='kaartje col-md-auto'><img class='rounded-circle' src='%1\$s' alt='Pasfoto %2\$s'><p class='naamkaderlid'>%2\$s</p>\n<p class='functiekaderlid'>%3\$s</p>\n<p class='mailkaderlid'>%4\$s</p>\n", $fd, $ln, $func, $email);
			} else {
				$txt .= sprintf("<div class='kaartje col-md-auto'><p class='naamkaderlid'>%s</p>\n<p class='functiekaderlid'>%s</p>\n<p class='mailkaderlid'>%s</p>\n", $ln, $func, $email);	
			}
			$txt .= "</div> <!-- Einde kaartje -->\n";
		}
		$txt .= "</div> <!-- Einde row onderdeel -->\n";
	} else {	
		$txt .= sprintf("<table class='%s'>\n", TABLECLASSES);
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
			if (strlen($row->FunctieOms) > 1) {
				$func = $row->FunctieOms;
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
	
	$txt .= "</div> <!-- Einde wieiswie -->\n";
	
	return $txt;
	
} # fnWieiswie

function fnNieuwLid() {
	$i_lid = new cls_Lid();
	$i_ins = new cls_Inschrijving();
	$i_lo = new cls_Lidond();

	if (isset($_GET['op']) and $_GET['op'] == "delete" and $_GET['RecordID'] > 0 and toegang("deleteinschrijving", 1, 1)) {
		$i_ins->update($_GET['RecordID'], "Verwijderd", date("Y-m-d"));
	}

	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$lidid = 0;
		$eersteles = "";
		if (isset($_POST['addkloslid'])) {
			$anm = "";
			$lidid = $i_lid->add($_POST['achternaam']);
			if ($lidid > 0 and isset($_POST['lidvanaf']) and $_POST['lidvanaf'] > "1970-01-01" and strtotime($_POST['lidvanaf']) !== false) {
				(new cls_Lidmaatschap())->add($lidid, $_POST['lidvanaf']);
			}
			
			printf("<script>\nlocation.href='%s?tp=Ledenlijst/Wijzigen+lid/Algemene+gegevens&lidid=%d';\n</script>\n", $_SERVER['PHP_SELF'], $lidid);
		}
	}
		
	if (isset($_GET['op']) and $_GET['op'] == "edit" and $_GET['RecordID'] > 0 and toegang("editinschrijving", 1, 1)) {
		editinschrijving($_GET['RecordID']);
		
	} else {
		printf("<form method='post' id='nieuwlid' action='%s?%s'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
		echo("<label class='form-label'>Achternaam</label><input type='text' name='achternaam' title='Achternaam, zonder tussenvoegsels' maxlength=40>\n");
			
		if (toegang("Ledenlijst/Wijzigen lid/Lidmaatschap", 0, 0) == true) {
			echo("<label class='form-label'>Lid vanaf</label><input type='date' name='lidvanaf' title='Indien ingevuld, dan wordt het lidmaatschap toegevoegd'>\n");
		}
			
		printf("<button type='submit' class='%s' name='addkloslid' value='Toevoegen'>%s (Klos)lid</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
			
		echo("</form>\n");
				
		$insrows = $i_ins->lijst(2, 0, 1, 3);
		if (count($insrows) > 0) {
			
			$kols = null;
			
			if (toegang("editinschrijving", 0, 0)) {
				$l = sprintf("%s?tp=%s&op=edit&RecordID=%%d", $_SERVER['PHP_SELF'], $_GET['tp']);
				$kols[] = array('headertext' => "&nbsp;", 'columnname' => "RecordID", 'link' => $l, 'class' => "muteren");
			} else {
				$kols[] = array('headertext' => "#", 'columnname' => "RecordID");
			}
			
			$kols[] = array('headertext' => "Naam", 'columnname' => "Naam");
			$kols[] = array('columnname' => "Datum", 'headertext' => "Inschrijfdatum", 'type' => "date");
			$kols[] = array('headertext' => "Verwerkt", 'columnname' => "Verwerkt", 'type' => "date");
			$kols[] = array('headertext' => "&nbsp;", 'columnname' => "LnkPDF", 'link' => sprintf("%s/pdf.php?insid=%%d", BASISURL), 'class' => "pdf");
			$kols[] = array('headertext' => "Gekoppeld lid", 'columnname' => "GekoppeldLid");
			
			if (toegang("deleteinschrijving", 0, 0)) {
				$l = sprintf("%s?tp=%s&op=delete&RecordID=%%d", $_SERVER['PHP_SELF'], $_GET['tp']);
				$kols[] = array('headertext' => "&nbsp;", 'columnname' => "RecordID", 'link' => $l, 'class' => "trash");
			}
			
			echo("<div class='clear'></div>\n");
			echo("<h2>Archief</h2>\n");
			echo("<div id='filter'>\n");
			
			echo("<input placeholder='Tekstfilter' OnKeyUp=\"fnFilter('inschrijvingen', this);\" title='Filter op tekst'>");
			printf("<p class='aantrecords'>%d rijen</p>\n", count($insrows));
			
			echo("</div> <!-- Einde filter -->\n");
			echo(fnDisplayTable($insrows, $kols, "Verwerkte inschrijvingen", 0, "", "inschrijvingen"));
		}
	}
	
}  # fnNieuwLid

function editinschrijving($p_insid) {
	global $dtfmt, $currenttab;
	
	$i_ins = new cls_Inschrijving($p_insid);
	$i_lid = new cls_Lid();
	$i_ond = new cls_Onderdeel();
	$i_lb = new cls_Logboek();
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
				
		if (isset($_FILES['UploadFile']['name']) and strlen($_FILES['UploadFile']['name']) > 3) {
						
			$ext = explode(".", $_FILES['UploadFile']['name']);
			$ext = strtolower($ext[count($ext) - 1]);
			$mess = "";
			if ($ext != "pdf") {
				$mess = sprintf("Bestand '%s' kan niet worden ge-upload, omdat de extensie niet is toegestaan. Alleen PDF is toegestaan.", $_FILES['UploadFile']['name']);
				$i_lb->add($mess, 25, -1, 1, $i_ins->insid, 2);
			} elseif (isset($_POST['UploadFile']) and $_FILES['UploadFile']['size'] > (512 * 1024)) {
				$mess = "Het bestand mag niet worden toegevoegd, omdat het te groot is.";
				$i_lb->add($mess, 25, -1, 1, $i_ins->insid, 2);
			} else {
				$fp = fopen($_FILES['UploadFile']['tmp_name'], 'r');
				$content = fread($fp, filesize($_FILES['UploadFile']['tmp_name']));
				fclose($fp);
				$i_ins->addpdf(-1, $content, 1);
			}
		}
	}
	
	printf("<form method='post' action='%s?tp=%s&op=edit&RecordID=%d' id='%s' class='form-check form-switch' enctype='multipart/form-data'>\n", $_SERVER["PHP_SELF"], $_GET['tp'], $i_ins->insid, __FUNCTION__);
	echo("<h2>Inschrijving muteren</h2>\n");

	printf("<label class='form-label'>RecordID</label><p>%d</p>\n", $i_ins->insid);
	printf("<label class='form-label'>Datum inschrijving</label><input type='date' id='Datum' name='Datum' value='%s'>\n", $i_ins->inschrijfdatum);
	printf("<label class='form-label'>Volledige naam</label><input type='text' id='Naam' name='Naam' value='%s' class='w50' maxlength=50>", $i_ins->naam);
	printf("<label class='form-label'>Achternaam</label><input type='text' id='Achternaam' value='%s' title='Achternaam, zonder tussenvoegels' class='w45' maxlength=45>", $i_ins->achternaam);
	printf("<label class='form-label'>Geboortedatum</label><input type='date' id='Geboortedatum' value='%s'>", $i_ins->geboortedatum);
	printf("<label class='form-label'>E-mail</label><input type='email' id='Email' value='%s' class='w45'>", $i_ins->email);

	if ($currenttab == "Ledenlijst") {
		$i_ond->where = "O.`Type`='A' AND IFNULL(O.VervallenPer, '9999-12-31') > CURDATE()";
		printf("<label class='form-label'>Afdeling</label><select id='OnderdeelID' class='form-select form-select-sm'>\n<option value=0>Geen</option>\n%s</select>\n", $i_ond->htmloptions($i_ins->afdeling, 0, "", 0));
	}

	printf("<label class='form-label'>Opmerking</label><input type='text' id='Opmerking' name='Opmerking' value='%s' class='w100' maxlength=100>\n", $i_ins->opmerking);
	
	echo("<label class='form-label'>PDF inschrijfformulier</label>\n");
	
	if ($i_ins->pdfaanwezig == 1) {
		printf("<p><a href='%s/pdf.php?insid=%d'>%s huidige PDF</a></p>\n", BASISURL, $i_ins->insid, ICONPDF);
	}
	
	echo("<input type='file' name='UploadFile'>\n");
	echo("<input type='submit' name='UploadFile' value='Upload inschrijfformulier'>");
	printf("<p>Alleen PDF's zijn toegestaan</p>\n");
	
	printf("<label class='form-label'>Eerste les</label><input type='date' id='EersteLes' name='EersteLes' value='%s'>\n", $i_ins->eersteles);

	if ($currenttab == "Ledenlijst") {
		printf("<label class='form-label'>Gekoppeld aan lid</label><select id='LidID' name='LidID' class='form-select form-select-sm' onChange='this.form.submit();'>\n<option value=0>Geen</option>\n%s</select>\n", $i_lid->htmloptions($i_ins->lidid, 7));
		printf("<label class='form-label'>Verwerkt</label><input type='checkbox' class='form-check-input' id='chkverwerkt' value=1%s>\n", checked($i_ins->verwerkt));
	}
	
	printf("<label class='form-label'>Verwijderd</label><input type='checkbox' class='form-check-input' id='chkverwijderd' value=1%s>\n", checked($i_ins->verwijderd));
	
	printf("<label class='form-label'>Ingevoerd op</label><p>%s</p>\n", $i_ins->ingevoerdtekst());

	echo("<div id='opdrachtknoppen'>\n");
	printf("<button type='button' class='%s' onClick=\"location.href='%s?tp=%s'\">%s Sluiten</button>\n", CLASSBUTTON, $_SERVER['PHP_SELF'], $_GET['tp'], ICONSLUIT);
	echo("</div>\n");

	echo("</form>\n");
	
	printf("<script>
		\$('input').on('blur', function() {
				if (this.id == 'chkverwijderd' || this.id == 'chkverwerkt') {
					savecheckasdate('%1\$s', %2\$d, this)
				} else {
					savedata('%1\$s', %2\$d, this);
				}
			});

		\$('select').on('change', function() {
			savedata('%1\$s', %2\$d, this);
		});

	</script>\n", __FUNCTION__, $i_ins->insid);
	
	$i_ond = null;
	$i_lid = null;
	$i_ins = null;
	$i_lb = null;
	
}  # editinschrijving

function wachtlijst() {
	$i_lid = new cls_Lid();
	$i_ins = new cls_Inschrijving();
	$i_lo = new cls_Lidond();
	
	$op = $_GET['op'] ?? "";
	$rid = $_GET['RecordID'] ?? 0;
	
	if ($op == "edit" and $rid > 0) {
		editinschrijving($rid);

	} elseif ($op == "add" and $rid > 0) {
		$i_ins->vulvars($rid);
		$lidid = $i_lid->add($i_ins->achternaam);
		if ($lidid > 0) {
			$i_lid->update($lidid, "GEBDATUM", $i_ins->geboortedatum);
			if ($i_ins->geboortedatum <= date("Y-m-d", strtotime("-18 year"))) {
				$i_lid->update($lidid, "Email", $i_ins->email);
			} else {
				$i_lid->update($lidid, "EmailOuders", $i_ins->email);
			}
			if (strlen($i_ins->xml) >= 100) {
				$xml = simplexml_load_string($i_ins->xml);
				foreach ($xml as $col => $val) {
					if ($col == "Achternaam" or $col == "GEBDATUM" or $col == "Email") {
						// Deze uit de XML overslaan, omdat ze ook in de tabel staan.
					} elseif ($col == "Telefoon" and substr($val, 0, 2) == "06") {
						$i_lid->update($lidid, "Mobiel", $val);
					} elseif ($i_lid->bestaat_kolom($col)) {
						$i_lid->update($lidid, $col, $val);
					} elseif ($col == "Lidond") {
						$nieuwlo = $val;
					}
				}
			}
			$i_ins->update($i_ins->insid, "Verwerkt", date("Y-m-d H:i:s"));
			$i_ins->update($i_ins->insid, "LidID", $lidid);
			if($i_ins->eersteles > "1970-01-01") {
				(new cls_Lidmaatschap())->add($lidid, $i_ins->eersteles);
			} else {
				(new cls_Lidmaatschap())->add($lidid, date("Y-m-d"));
			}
			
			if ($i_ins->afdeling > 0) {
				$i_lo->add($i_ins->afdeling, $lidid, "opgegeven via inschrijfformulier", 0);
			}
			
			if (isset($nieuwlo) and count($nieuwlo) > 0) {
				foreach ($nieuwlo as $lo => $objlo) {
					$ondid = intval($objlo);
					if ($ondid > 0 and $ondid != $i_ins->afdeling) {
						$i_lo->add($ondid, $lidid, "opgegeven via inschrijfformulier", 0);
					}
				}
			}

			printf("<script>\nlocation.href='%s?tp=Ledenlijst/Wijzigen+lid/Algemene+gegevens&lidid=%d';\n</script>\n", $_SERVER['PHP_SELF'], $lidid);
		}
				
	} elseif ($op == "nieuw") {
		$rid = $i_ins->add();
		editinschrijving($rid);
	}

	if ($op != "nieuw" and $op != "edit") {
		$insres = $i_ins->lijst(1, -1, 0);

		$kols = null;
		$l = sprintf("%s?tp=%s&op=edit&RecordID=%%d", $_SERVER['PHP_SELF'], $_GET['tp']);
		$kols[] = array('headertext' => "&nbsp;", 'columnname' => "RecordID", 'link' => $l, 'class' => "muteren");

		$kols[] = array('columnname' => "Datum", 'headertext' => "Inschrijfdatum", 'type' => "date", 'readonly' => true);
		$kols[] = array('columnname' => "Naam", 'readonly' => true);
		$kols[] = array('columnname' => "Geboortedatum", 'headertext' => "Geboren", 'type' => "DTSHORT", 'readonly' => true);
		$kols[] = array('columnname' => "Afdeling", 'readonly' => true);
		$kols[] = array('headertext' => "Eerste les", 'columnname' => "EersteLes", 'type' => "date");
		$kols[] = array('headertext' => "Opmerking", 'columnname' => "Opmerking", 'collen' => 40);
		$l = sprintf("%s/pdf.php?insid=%%d", BASISURL);
		$kols[] = array('headertext' => "&nbsp;", 'columnname' => "LnkPDF", 'link' => $l, 'class' => "pdf");
		if (toegang("Ledenlijst/Nieuw (klos)lid", 0, 0)) {
			$l = sprintf("%s?tp=%s&op=add&RecordID=%%d", $_SERVER['PHP_SELF'], $_GET['tp']);
			$kols[] = array('headertext' => "&nbsp;", 'columnname' => "KanLidWorden", 'link' => $l, 'class' => "toevoegen");
		}
		$kols[] = array('columnname' => "MogelijkAlInTabel", 'headertext' => "Aanwezig?", 'readonly' => true);

		echo("<div id='filter'>\n");
		echo("<input type='text' name='tbTekstfilter' placeholder='Tekstfilter' onkeyup=\"fnFilter('inschrijvingen', this);\" title='Tekstfilter'>\n");
		$l = sprintf("%s?tp=%s&op=nieuw", $_SERVER['PHP_SELF'], $_GET['tp']);
		printf("<button class='%s btn-sm' onClick=\"location.href='%s'\">%s Inschrijving</button>\n", CLASSBUTTON, $l, ICONTOEVOEGEN);
		echo("</div> <!-- Einde filter -->\n");
		
		echo(fnEditTable($insres, $kols, "", "Inschrijvingen"));
		
	}
				
}  # wachtlijst

function fnOnderdelenmuteren($ondtype="G") {
	global $currenttab, $currenttab2;
	
	$i_lo = new cls_Lidond();
	$i_lid = new cls_Lid();
	$i_auth = new cls_Authorisation();
	
	if ($ondtype == "C") {
		$ondnaammv = "Commissies";
	} else	if ($ondtype == "R") {
		$ondnaammv = "Rollen";
	} else	if ($ondtype == "S") {
		$ondnaammv = "Selecties";
	} else {
		$ondnaammv = ARRTYPEONDERDEEL[$ondtype] . "en";
	}
	
	fnDispMenu(2);
	
	echo("<div id='onderdelenmuteren'>\n");
	
//	echo("<h2>Werk in uitvoering, er mag getest worden.</h2>\n");
	
	$scherm = $_GET['Scherm'] ?? "O";
	$onderdeelid = 0;
	if (isset($_GET['OnderdeelID']) and $_GET['OnderdeelID'] > 0) {
		$onderdeelid = intval($_GET['OnderdeelID']);
	} elseif (isset($_POST['OnderdeelID']) and $_POST['OnderdeelID'] > 0) {
		$onderdeelid = intval($_POST['OnderdeelID']);
	} elseif (isset($_POST['muteerleden'])) {
		$onderdeelid = intval(str_replace("muteerleden_", "", $_POST['muteerleden']));
		$scherm = "L";
	}
	$i_ond = new cls_Onderdeel($onderdeelid);
	
	$ondrows = $i_ond->editlijst($ondtype);
	
	if ($scherm == "L") {
		LedenOnderdeelMuteren($onderdeelid, 0);
		
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
			printf("<button type='button' class='%s' onclick=\"location.href='%s?tp=%s&Scherm=L&OnderdeelID=%d';\"><i class='bi bi-skip-backward-circle'></i> Vorige %s</button>\n", CLASSBUTTON, $_SERVER['PHP_SELF'], $_GET['tp'], $vor, ARRTYPEONDERDEEL[$ondtype]);
		}
		printf("<button type='button' class='%s' onclick=\"location.href='%s?tp=%s&Scherm=S';\">%s Overzicht</button>\n", CLASSBUTTON, $_SERVER['PHP_SELF'], $_GET['tp'], ICONOVERZICHT);
	
		if ($vol > 0) {
			printf("<button type='button' class='%s' onclick=\"location.href='%s?tp=%s&Scherm=L&OnderdeelID=%d';\"><i class='bi bi-skip-forward-circle'></i> Volgende %s</button>\n", CLASSBUTTON, $_SERVER['PHP_SELF'], $_GET['tp'], $vol, ARRTYPEONDERDEEL[$ondtype]);
		}
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		echo("<div class='clear'></div>\n");
		
	} elseif ($scherm == "W" and $onderdeelid > 0) {
		
		detailsonderdeelmuteren($onderdeelid);

	} else {

		$kols[0]['headertext'] = "&nbsp;";
		$kols[0]['link'] = sprintf("%s?tp=%s&Scherm=W&OnderdeelID=%%d", $_SERVER['PHP_SELF'], $_GET['tp'], strtolower(ARRTYPEONDERDEEL[$ondtype]));
		$kols[0]['class'] = "muteren";
		$kols[0]['columnname'] = "RecordID";
		
		$kols[1]['headertext'] = "Code";
		$kols[1]['columnname'] = "Kode";
		
		$kols[2]['headertext'] = "Naam";
		$kols[2]['columnname'] = "Naam";
		
		$kols[3]['headertext'] = "Kader?";
		$kols[3]['columnname'] = "Kader";
		$kols[3]['type'] = "checkbox";
		$kols[3]['readonly'] = true;
		
		$kols[4]['headertext'] = "# leden";
		$kols[4]['columnname'] = "aantalLeden";
		$kols[4]['type'] = "integer";
		$kols[4]['readonly'] = true;
				
		$kols[5]['headertext'] = "&nbsp;";
		$kols[5]['link'] = sprintf("%s?tp=%s&Scherm=L&OnderdeelID=%%d", $_SERVER['PHP_SELF'], $_GET['tp'], strtolower(ARRTYPEONDERDEEL[$ondtype]));
		$kols[5]['class'] = "leden";
		$kols[5]['columnname'] = "RecordID";
		$kols[5]['title'] = "Leden muteren";
		$kols[5]['columntitle'] = 4;
	
		$rows = $i_ond->editlijst($ondtype, 1);
		echo(fnDisplayTable($rows, $kols, $ondnaammv . " muteren"));
	}
	
	echo("</div>  <!-- Einde onderdelenmuteren -->\n");	

} # fnOnderdelenmuteren

function detailsonderdeelmuteren($p_ondid) {
	global $dtfmt;
	
	$i_ond = new cls_Onderdeel($p_ondid);
	$i_lo = new cls_Lidond();
	$i_auth = new cls_Authorisation();
		
	$i_lo->autogroepenbijwerken(0, 1, $i_ond->oid);
	$i_lo->auto_einde($i_ond->oid);

	$row = $i_ond->record();

	printf("<form method='post' class='form-check form-switch' id='%s' action='%s?tp=%s&OnderdeelID=%d&Scherm=W'>\n", __FUNCTION__, $_SERVER['PHP_SELF'], $_GET['tp'], $i_ond->oid);
	echo("<input type='hidden' name='formname' value='muteren_detail_onderdeel'>\n");
		
	printf("<label class='form-label'>RecordID</label><p>%s</p>\n", $row->RecordID);
	printf("<label class='form-label'>Code</label><input type='text' id='Kode' class='w7' value='%s' maxlength=7>\n", $row->Kode);
	printf("<label class='form-label'>Naam</label><input type='text' id='Naam' class='w50' value=\"%s\" maxlength=50>\n", $row->Naam);
	printf("<label class='form-label'>Type onderdeel</label><p>%s</p>\n", $i_ond->typeoms);
	
	if (($i_ond->type == "G" or $i_ond->type == "R" or $i_ond->type == "S")) {
		printf("<label>Opmerking / uitleg</label><textarea id='Opmerking' name='Opmerking'>%s</textarea>\n", $i_ond->opmerking);
	}
	
	if ($i_ond->type != "T" and $i_ond->type != "O") {
		printf("<label class='form-label'>Centraal e-mailadres</label><input type='email' id='centraalemail' class='w50' value='%s' maxlength=50>\n", $row->CentraalEmail);
		printf("<label class='form-label'>Is kader</label><input type='checkbox' class='form-check-input' id='Kader' %s>\n", checked($row->Kader));
	}
	if ($i_ond->type == "A"  or $i_ond->type == "G") {
		printf("<label class='form-label'>Aangesloten bij</label><select id='ORGANIS' class='form-select form-select-sm'>\n%s</select>\n", (new cls_Organisatie())->htmloptions(0, $row->ORGANIS));
	}
		
	if ($i_ond->aantal("O.`Type` IN ('B', 'C', 'R')") > 0 and strlen($i_ond->mysql) < 5) {
		printf("<label for='ledenmuterendoor' class='form-label'>Leden muteerbaar door</label><select id='ledenmuterendoor' class='form-select form-select-sm'>\n<option value=0>Geen extra groep</option>\n%s</select>\n", $i_ond->htmloptions($row->LedenMuterenDoor, 1, "BCR"));
	}
		
	printf("<label for='Alleen leden' class='form-label'>Alleen leden</label><input type='checkbox' class='form-check-input' id='Alleen leden' %s>\n", checked($i_ond->alleenleden));
		
	if ($i_ond->type == "A") {
		printf("<label for='LIDCB' class='form-label'>Contributie lid</label><input id='LIDCB' type='text' class='bedrag' value='%.2F'>", $row->LIDCB);
		printf("<label for='JEUGDCB' class='form-label'>Contributie jeugdlid</label><input id='JEUGDCB' type='text' class='bedrag' value='%.2F'>", $row->JEUGDCB);
		printf("<label for='FUNCTCB' class='form-label'>Contributie afdelingsfunctionaris</label><input id='FUNCTCB' class='bedrag' type='text' value='%.2F'>", $row->FUNCTCB);
		printf("<label for='NIETLIDCB' class='form-label'>Contributie niet-lid</label><input id='NIETLIDCB' type='text' class='bedrag' value='%.2F'>", $row->NIETLIDCB);
	}
		
	printf("<label for='vervallenper' class='form-label'>Vervallen per</label><input type='date' id='VervallenPer' value='%s'>\n", $row->VervallenPer);
	printf("<label class='form-label'>Historie opschonen</label><input type='number' class='num3' id='HistorieOpschonen' value=%d><p>dagen na einde lidmaatschap %s</p>\n", $row->HistorieOpschonen, strtolower($i_ond->typeoms));
	if ($i_ond->type == "A") {
		printf("<label class='form-label'>Bewaartermijn presentie</label><input type='number' class='num3' class='num3' id='BewaartermijnPresentie' value=%d><p>maanden na einde lidmaatschap %s</p>\n", $i_ond->bewaartermijnpresentie, strtolower($i_ond->typeoms));
		printf("<label class='form-label'>Afmelden via de zelfservice</label><input type='number' class='num3' class='num3' id='AfmeldenMogelijk' value=%d><p>aantal lessen vooruit</p>\n", $i_ond->afmeldenmogelijk);
	}
	printf("<label class='form-label'>Maximale periode</label><input type='number' class='num3' id='MaximaleLengtePeriode' value=%d><p>dagen</p>\n", $row->MaximaleLengtePeriode);

	if (($i_ond->type == "G" or $i_ond->type == "R" or $i_ond->type == "S") and WEBMASTER) {
		printf("<label>MySQL-code automatisch bijwerken</label><textarea id='mysql' name='mysql' %s>%s</textarea>\n", ATTRCODEELEMENT, $i_ond->mysql);
		if (strlen($row->MySQL) > 10 and $i_ond->controleersql($row->MySQL, 1) == false) {
			echo("<p class='waarschuwing'>Deze code kan niet worden uitgevoerd.</p>");
		} elseif (strlen($row->MySQL) > 10) {
			$st = microtime(true);
			$result = (new cls_db_base())->execsql($row->MySQL);
			printf("<label class='form-label'>Duur query in seconden</label><p>%f</p>\n", microtime(true) - $st);
			printf("<label class='form-label'>Aantal rijen</label><p>%s</p>\n", count($result->fetchAll()));			
		}
	}
		
	$igba = "";
	$f = sprintf("Toegang=%d", $i_ond->oid);
	foreach ($i_auth->basislijst($f, "Tabpage") as $authrow) {
		if (strlen($igba) > 2) {
			$igba .= ", ";
		}
		$igba .= $authrow->Tabpage;
	}
	if (strlen($igba) > 2) {
		printf("<label class='form-label'>In gebruik bij autorisatie(s)</label><p>%s</p>\n", $igba);
	}
	
	$dtfmt->setPattern(DTLONG);
	printf("<label clas='form-label'>Laatst bijgewerkt</label><p>%s</p>", $dtfmt->format(strtotime($row->Gewijzigd)));

	echo("<div id='opdrachtknoppen'>\n");
	printf("<button type='button' class='%s' onClick=\"location.href='%s?tp=%s&Scherm=W';\">%s Sluiten</button>\n", CLASSBUTTON, $_SERVER['PHP_SELF'], $_GET['tp'], ICONSLUIT);
	echo("</div>\n");
		
	echo("</form>\n");
		
	printf("<script>
				
		\$('input, textarea').on('blur', function(){
			savedata('onderdeeledit', %1\$d, this);
		});
			
		\$(\"input[type='number']\").on('change', function(){
			savedata('onderdeeledit', %1\$d, this);
		});
		
		\$('select').on('change', function(){
			savedata('onderdeeledit', %1\$d, this);
		});
				
		</script>", $i_ond->oid);
	
}  # detailsonderdeelmuteren

function LedenOnderdeelMuteren($p_ondid) {
	// Dit is leden per onderdeel
	
	$i_ond = new cls_Onderdeel($p_ondid);
	$i_lo = new cls_Lidond($p_ondid);
	$i_lid = new cls_Lid();
	$i_fnk = new cls_Functie();
	$i_lo->autogroepenbijwerken(0, 5, $p_ondid);
	
	if ($i_ond->oid <= 0) {
		$mess = sprintf("Onderdeel %d bestaat niet.", $p_ondid);
		(new cls_Logboek())->add($mess, 15, -1, 1, 0, 9);
		
	} elseif ($i_lo->magmuteren == false) {
		$mess = sprintf("Je hebt geen rechten om de leden van %s te muteren.", $i_ond->naam);
		(new cls_Logboek())->add($mess, 15, -1, 1, 0, 9);
		
	} else {
	
		if (isset($_POST['add_lid']) and $_POST['add_lid'] > 0) {
			$i_lo->add($i_ond->oid, $_POST['add_lid']);

		} elseif (isset($_GET['op']) and $_GET['op'] == "delete" and isset($_GET['RecordID']) and $_GET['RecordID'] > 0) {
			if ($i_lo->ondid($_GET['RecordID']) == $i_ond->oid) {
				$i_lo->update($_GET['RecordID'], "Opgezegd", date("Y-m-d", strtotime("-1 day")));
				printf("<script>
					var url = '%s?tp=%s&OnderdeelID=%d';
					location.href=url;
					</script>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $_GET['OnderdeelID']);
			} else {
				$mess = "Dit record bestaat niet bij dit onderdeel. Het wordt niet verwijderd.";
				debug($mess, 1, 1);
			}
		} elseif (WEBMASTER) {
			$i_lo->auto_einde($p_ondid, 15);
		}
		
		printf("<form method='post' action='%s?tp=%s&OnderdeelID=%d&Scherm=L'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $i_ond->oid);
		echo("<input type='hidden' name='formname' value='muteren_leden_onderdeel'>\n");
		if ($i_ond->isautogroep == false) {
			$nl = sprintf("<select name='add_lid' class='form-select form-select-sm' onChange='this.form.submit();'>\n");
			if ($i_ond->alleenleden == 1) {
				$nl .= sprintf("<option value=0>Lid toevoegen ...</option>\n");
			} else {
				$nl .= sprintf("<option value=0>Persoon toevoegen ...</option>\n");
			}
			$xf = sprintf("L.RecordID NOT IN (SELECT Lid FROM %sLidond AS LO WHERE LO.OnderdeelID=%d AND (LO.Opgezegd IS NULL))", TABLE_PREFIX, $i_ond->oid);
			$nl .= $i_lid->htmloptions(-1, $i_ond->alleenleden, $xf);
			$nl .= sprintf("</select>\n");
		
			if (!isset($_POST['selectie_status'])) {
				$_POST['selectie_status'] = "L";
			}
		} else {
			$i_lo->autogroepenbijwerken(0, 3, $p_ondid);
			$nl = "";
		}

		if ($i_ond->type == "E" or $i_ond->type == "T") {
			$f = 1;
		} else {
			$f = 8;
		}
		$res = $i_lo->lijst($i_ond->oid, $f, "IF(IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE(), 0, 1)", "", "", 0, 0);
		
		$f = sprintf("OnderdeelID=%d", $i_ond->oid);
		if ($i_ond->type == "A") {
			$rowsfunc = $i_fnk->selectlijst($i_ond->type, $i_lo->min("Vanaf", $f), 1);
		} else {
			$rowsfunc = $i_fnk->selectlijst("L", $i_lo->min("Vanaf", $f), 1);
		}
		asort($rowsfunc);
		$kols = null;
		$kols[0] = array('headertext' => "Naam lid", 'columnname' => "NaamLid", 'readonly' => true);
		$kols[1] = array('headertext' => "Vanaf", 'columnname' => "Vanaf", 'type' => "date");
		
		if (count($rowsfunc) > 1 and ($i_ond->iskader or $i_ond->type == "A" or $i_ond->type == "C")) {
			$kols[2] = array('headertext' => "Functie", 'columnname' => "Functie", 'bronselect' => $rowsfunc);
		}
		if ($i_ond->iskader) {
			$kols[3] = array('headertext' => "Email bij functie", 'columnname' => "EmailFunctie", 'type' => "email");
		}
		$kols[4] = ['headertext' => "Opmerking", 'columnname' => "Opmerk"];
		
		if ($i_ond->type != "E" and $i_ond->type != "T") {
			$kols[] = array('headertext' => "Tot en met", 'type' => "date");
		} else {
			$kols[1]['readonly'] = true;
		}
		
		if (($i_ond->type == "E" or $i_ond->type == "G"  or $i_ond->type == "R" or $i_ond->type == "S" or $i_ond->type == "T") and $i_ond->isautogroep == false) {
			$l = sprintf("%s?tp=%s&op=delete&OnderdeelID=%d&RecordID=%%d", $_SERVER['PHP_SELF'], $_GET['tp'], $i_ond->oid);
			$kols[] = array('headertext' => "&nbsp;", 'columnname' => "ridDelete", 'link' => $l, 'class' => "trash");
		}

		echo(fnEditTable($res, $kols, "ledenperonderdeelmuteren", $i_ond->naam));
		echo($nl);
		
		if (strlen($i_ond->opmerking) > 0) {
			printf("<p>%s</p>\n", $i_ond->opmerking);
		}
	
		if ($i_ond->isautogroep) {
			echo("<p>Deze groep wordt automatisch bijgewerkt.</p>\n");
		}
	}
	
}  # LedenOnderdeelMuteren

function persoonlijkeGroepMuteren() {
	
	$i_ond = new cls_Onderdeel();

	$l = sprintf("%s?tp=%s&OnderdeelID=%%d", $_SERVER['PHP_SELF'], $_GET['tp']);
	$kols[] = array('headertext' => "&nbsp", 'link' => $l, 'class' => "leden", 'columnname' => "RecordID", 'title' => "%d leden");
	$kols[] = array('headertext' => "Code", 'columnname' => "Kode");
	$kols[] = array('headertext' => "Naam", 'columnname' => "Naam");
		
	$rows = $i_ond->editlijst("P", 1);
	echo(fnDisplayTable($rows, $kols, "Groepen muteren"));
	
}  # persoonlijkeGroepMuteren

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
		
		if ($currenttab == "Eigen gegevens") {
			$th = "";
		} else {
			$th = $naamlid;
		}
		
		$tn = "Algemeen";
		if (toegang($ct . $tn, 0, 0)) {
			$nl = (new cls_Lid())->Roepnaam($lidid);
			$fd = (new cls_Foto(-1, $lidid))->fotodata;
			if ($fd != null) {
				$xtra = sprintf("<div id='pasfoto'><img src='%s' title='Laatst gewijzigd op %s'></div>\n", $fd, $dtfmt->format(strtotime($i_foto->laatstgewijzigd)));
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
		$kols[] = array('headertext' => "Lidnummer", 'columnname' => "Lidnr");
		$kols[] = array('headertext' => "Vanaf", 'columnname' => "LIDDATUM", 'type' => "DTTEXT");
		$kols[] = array('headertext' => "Opgezegd", 'columnname' => "Opgezegd", 'type' => "DTTEXT");
		$kols[] = array('headertext' => "Duur", 'columnname' => "Duur");
		
		if (toegang($ct . $tn, 0, 0)) {
			$rows = (new cls_Lidmaatschap())->overzichtlid($lidid);
			if (count($rows) > 1) {
				$tabblad[$tn] = fnDisplayTable($rows, $kols, $tn . " " . $th);
			}
		}
		
		$tn = "Afdelingen";
		$kols = null;
		$rows = (new cls_Lidond())->overzichtlid($lidid, "A");
		
		if (count($rows) > 0 and toegang($ct . $tn, 0, 0)) {
			$kols[] = array('headertext' => "Afdeling", 'columnname' => "Naam");
			if (strlen(max(array_column($rows, "Functie"))) > 0) {
				$kols[] = array('headertext' => "Functie", 'columnname' => "Functie");
			}
		
			$kols[] = array('columnname' => "Vanaf", 'headertext' => "Vanaf", 'type' => "DTTEXT");
			$kols[] = array('headertext' => "Opmerking", 'columnname' => "Opmerk");
		
			if (strlen(max(array_column($rows, "Groep"))) > 0) {
				$kols[] = array('headertext' => "Groep", 'columnname' => "Groep");
			}

			if (strlen(max(array_column($rows, "Opgezegd"))) > 0) {
				$kols[] = array('headertext' => "Tot en met", 'columnname' => "Opgezegd", 'type' => "DTTEXT");
			}
		
			$kols[] = array('headertext' => "Duur", 'columnname' => "Duur");
			
			$tabblad[$tn] = fnDisplayTable($rows, $kols, $th, 0, "", "", "onderdelenlid");
		}
		
		$tn = "Groepen";
		$rows = (new cls_Lidond())->overzichtlid($lidid, "G");
		$kols = null;
		if (count($rows) > 0 and toegang($ct . $tn, 0, 0)) {
			$kols[0]['headertext'] = "Groep";
			$kols[0]['columnname'] = "Naam";
		
			$kols[1]['headertext'] = "Vanaf";
			$kols[1]['columnname'] = "Vanaf";
			$kols[1]['type'] = "DTTEXT";
			
			$kols[2] = array('headertext' => 'Opmerking', 'columnname' => 'Opmerk');
		
			if (strlen(max(array_column($rows, "Opgezegd"))) > 0) {
				$kols[3] = array('headertext' => "Tot en met", 'columnname' => "Opgezegd", 'type' => "DTTEXT");
			}
			$kols[4] = array('headertext' => "Duur", 'columnname' => "Duur");
			
			$tabblad[$tn] = fnDisplayTable($rows, $kols, $th, 0, "", "", "onderdelenlid");
		}
		
		$tn = "Kader";
		$kols = null;
		$kols[0] = array('headertext' => "Onderdeel", 'columnname' => "Naam");
		$kols[1] = array('headertext' => "Functie", 'columnname' => "Functie");
		$kols[2] = array('headertext' => "Vanaf", 'columnname' => "Vanaf", 'type' => "DTTEXT");
		$kols[3] = array('headertext' => "Tot en met", 'columnname' => "Opgezegd", 'type' => "DTTEXT");
		$kols[4] = array('headertext' => "Duur", 'columnname' => "Duur");
		
		if (toegang($ct . $tn, 0, 0)) {
			if ($tn == $currenttab3) {
				$tabidx = count($tabblad);
			}	
			$rows = (new cls_Lidond())->overzichtlid($lidid, "K");
			if (count($rows) > 0) {
				$tabblad[$tn] = fnDisplayTable($rows, $kols, $th, 0, "", "", "onderdelenlid");
			}
		}

		$tn = "Rollen";
		$rows = $i_lo->overzichtlid($lidid, "R");
		$kols = null;
		if (count($rows) > 0 and toegang($ct . $tn, 0, 0)) {
			$kols[0]['headertext'] = "Rol";
			$kols[0]['columnname'] = "Naam";
		
			$kols[1]['headertext'] = "Vanaf";
			$kols[1]['columnname'] = "Vanaf";
			$kols[1]['type'] = "DTTEXT";
		
			if (strlen(max(array_column($rows, "Opgezegd"))) > 0) {
				$kols[2]['headertext'] = "Tot en met";
				$kols[2]['columnname'] = "Opgezegd";
				$kols[2]['type'] = "DTTEXT";
			}

			$kols[3]['headertext'] = "Duur";
			$kols[3]['columnname'] = "Duur";
			
			$tabblad[$tn] = fnDisplayTable($rows, $kols, $th, 0, "", $tn);
		}
		
		$tn = "Diploma's";
		$rows = (new cls_Liddipl())->overzichtlid($lidid);
		if (count($rows) > 0 and toegang($ct . $tn, 0, 0)) {
			$kols[0]['headertext'] = "Diploma";
			$kols[0]['columnname'] = "NaamLang";
			$kols[1]['headertext'] = "Behaald op";
			$kols[1]['columnname'] = "DatumBehaald";
			$kols[1]['type'] = "DTTEXT";	
			
			if (strlen(max(array_column($rows, "Plaats"))) > 0) {
				$kols[3]['headertext'] = "Plaats";
				$kols[3]['columnname'] = "Plaats";
			}
			
			if (strlen(max(array_column($rows, "Diplomanummer"))) > 0) {
				$kols[3]['headertext'] = "Gegevens";
				$kols[3]['columnname'] = "Diplomanummer";
			}
			if (max(array_column($rows, "GeldigTot")) > "1900-01-01") {
				$kols[4]['headertext'] = "Geldig tot";
				$kols[4]['columnname'] = "GeldigTot";
				$kols[4]['type'] = "DTTEXT";
			}
			if (count($rows) > 0) {
				$tabblad[$tn] = fnDisplayTable($rows, $kols, $th);
			} else {
				$tabblad[$tn] = sprintf("<p class='mededeling'>Bij %s zijn geen diploma's geregistreerd.</p>", $naamlid);
			}
		}
		
		$kols=null;
		$tn = "Toestemmingen";
		$kols[] = array('headertext' => "Toestemming", 'columnname' => "Naam");
		$kols[] = array('headertext' => "Afgegeven", 'columnname' => "Vanaf");

		if ($gs != "B" and count($i_lo->lijstperlid($lidid, "T")) > 0 and toegang($ct . $tn, 0, 0)) {
			$rows = $i_lo->overzichtlid($lidid, "T");
			$tabblad[$tn] = fnDisplayTable($rows, $kols, $th, 0, "", "toestemmingen");
			if (strlen($_SESSION['settings']['uitleg_toestemmingen']) > 0) {
				$tabblad[$tn] .= sprintf("<p>%s</p>\n", $_SESSION['settings']['uitleg_toestemmingen']);
			}
		}
		
		$kols=null;
		$tn = "Eigenschappen";
		$kols[] = array('headertext' => "Eigenschap", 'columnname' => "Naam");
		$kols[] = array('headertext' => "Toegevoegd op", 'columnname' => "Vanaf");

		if ($gs != "B" and count($i_lo->lijstperlid($lidid, "E")) > 0 and toegang($ct . $tn, 0, 0)) {
			$rows = $i_lo->overzichtlid($lidid, "E");
			if (count($rows) > 0) {
				$tabblad[$tn] = fnDisplayTable($rows, $kols, $th, 0, "", "eigenschappen");
			} else {
				$tabblad[$tn] = sprintf("<p class='mededeling'>%s heeft geen toestemmingen verleend.</p>\n", $naamlid);
			}
		}

		$tn = "Bijzonderheden";
		$rows = (new cls_Memo())->overzichtlid($lidid);
		if (count($rows) > 0 and toegang($ct . $tn, 0, 0)) {
			$tabblad[$tn] = sprintf("<h2>%s %s</h2>\n", $tn, $naamlid);
			foreach ($rows as $row) {
				$tabblad[$tn] .= sprintf("<h3>%s</h3>\n", ARRSOORTMEMO[$row->Soort]);
				$tabblad[$tn] .= sprintf("<p>%s</p>\n", $row->Memo);
			}
		}
			
		$tn = "Presentie";
		$kols = null;
		$rows = (new cls_Aanwezigheid())->overzichtlid($lidid);
		if (count($rows) > 0 and toegang($ct . $tn, 0, 0)) {
			$kols[] = array('headertext' => "Seizoen", 'columnname' => "Seizoen");
			$kols[] = array('headertext' => "Datum", 'columnname' => "Datum");
			$kols[] = array('headertext' => "Omschrijving", 'columnname' => "Omschrijving");
			if (strlen(max(array_column($rows, "Functie"))) > 0) {
				$kols[] = array('headertext' => "Functie", 'columnname' => "Functie");
			}
			$kols[] = array('headertext' => "Status", 'columnname' => "Status");
		
			if (strlen(max(array_column($rows, "Opmerking"))) > 0) {
				$kols[] = array('headertext' => "Opmerking`", 'columnname' => "Opmerking");
			}
			$tabblad[$tn] = fnDisplayTable($rows, $kols, $th, 0, "", "", "table-sm");
		
			if ($tn == $currenttab3) {
				$tabidx = count($tabblad)-1;
			}
		}
			
		$tn = "Rekeningen";
		$kols = null;
		$rows = (new cls_Rekeningregel())->overzichtlid($lidid);
		if (count($rows) > 0 and toegang($ct . $tn, 0, 0)) {
			$l = sprintf("%s?tp=%s&op=details&lidid=%d&rid=%%d' title='Details rekening", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
			$kols[0] = array('columnname' => "Nummer", 'link' => $l, 'class' => "details");
			
			$kols[1]['columnname'] = "Nummer";
			$kols[2]['columnname'] = "Seizoen";
			$kols[3] = array('columnname' => "Datum", 'type' => "date");
			$kols[4]['columnname'] = "Omschrijving";
			$kols[5] = array('columnname' => "Bedrag", 'type' => "bedrag");
			$tabblad[$tn] = fnDisplayTable($rows, $kols, $th);
			
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
			$l = sprintf("index.php?tp=%s&MailingHistID=%%d", urlencode($_GET['tp']));
			$kols[0] = array('columnname' => "RecordID", 'link' => $l, 'class' => "details");
			$kols[1] = array('columnname' => "Verzonden", 'headertext' => "Datum", 'type' => "date");
			$kols[2]['columnname'] = "Van";
			$kols[3] = array('columnname' => "subject", 'headertext' => "Onderwerp");
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
		$i_ed = new cls_Evenement_Deelnemer();
		$rows = $i_ed->overzichtlid($lidid);
		if (count($rows) > 0 and toegang($ct . $tn, 0, 0)) {
			$tabblad[$tn] = sprintf("<table class='%s'>\n", TABLECLASSES);
			if ($currenttab != "Eigen gegevens") {
				$tabblad[$tn] .= sprintf("<caption>Deelname evenementen %s</caption>\n", $naamlid);
			}
			$tabblad[$tn] .= "<tr><th>Wanneer</th><th>Omschrijving</th><th class='contact'>Contact</th><th class='opmerking'>Opmerking / Functie</th><th>Status</th>";
			if ($lidid == $_SESSION['lidid']) {
				$tabblad[$tn] .= "<th class='inagenda'>In agenda</th>";
			}
			$tabblad[$tn] .= "</tr>\n";
			
			foreach($rows as $row) {
				$i_ed->vulvars(-1, -1, $row->RecordID);
				$tabblad[$tn] .= "<tr>\n";
				$wanneer = $i_ed->i_ev->datumtekst;
				if (strlen($i_ed->tijden) > 0) {
					$wanneer .= "<br>" . $i_ed->tijden;
				}
				if (strlen($i_ed->i_ev->verzameltijd) >= 5 and $i_ed->i_ev->datum >= date("Y-m-d")) {
					$wanneer .= sprintf("<br>Verzamelen om %s&nbsp;uur", $i_ed->i_ev->verzameltijd);
				}
					
				$tabblad[$tn] .= sprintf("<td>%s</td>\n", $wanneer);
				$tabblad[$tn] .= sprintf("<td>%s<br>%s</td>\n", $i_ed->i_ev->omschrijving, $i_ed->i_ev->locatie);
				$tabblad[$tn] .= sprintf("<td class='contact'>%s</td>\n", $i_ed->i_ev->emailcontact);
				$tabblad[$tn] .= sprintf("<td class='opmerking'>%s<br>%s</td>\n", $i_ed->opmerking, $i_ed->functie);
				$tabblad[$tn] .= sprintf("<td>%s</td>\n", $i_ed->statusoms);
				if ($i_ed->lidid == $_SESSION['lidid']) {
					if ($i_ed->i_ev->datum >= date("Y-m-d") and $i_ed->aanwezig == 1) {
						$tabblad[$tn] .= sprintf("<td class='inagenda'>%s</td>\n", $i_ed->agendaknop());
					} else {
						$tabblad[$tn] .= "<td class='inagenda'></td>";
					}
				}
				$tabblad[$tn] .= "</tr>\n";
			}
			
			$tabblad[$tn] .= "</table>\n";
		}
		$i_ed = null;
		$i_ev = null;

		$tn = "Logboek";
		$rows = (new cls_Logboek())->overzichtlid($lidid);
		if ($lidid > 0 and toegang($ct . $tn, 0, 0) and count($rows) > 0) {
			$kols = fnStandaardKols("logboek", 0, $rows);
			$tabblad[$tn] = fnDisplayTable($rows, $kols, $i_ond->naam);
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
		
		if ($currenttab == "Ledenlijst") {
			$lnk = sprintf("<button type='button' class='%s' OnClick=\"location.href='%s?tp=%%s&lidid=%%d';\">%%s lid</button>\n", CLASSBUTTON, $_SERVER['PHP_SELF']);
			echo("<div id='opdrachtknoppen'>\n");
			if ($current_key > 0) {
				printf($lnk, $_GET['tp'], $_SESSION['sel_leden'][0], "<i class='bi bi-skip-start-circle'></i> Eerste");
				printf($lnk, $_GET['tp'], $_SESSION['sel_leden'][$current_key-1], "<i class='bi bi-skip-backward-circle'></i> Vorige");
			}
			if ($_SESSION['sel_leden'][count($_SESSION['sel_leden'])-1] != $lidid) {
				printf($lnk, $_GET['tp'], $_SESSION['sel_leden'][$current_key+1], "<i class='bi bi-skip-forward-circle'></i> Volgende");
				printf($lnk, $_GET['tp'], $_SESSION['sel_leden'][count($_SESSION['sel_leden'])-1], "<i class='bi bi-skip-end-circle'></i> Laatste");
			}
			printf("<p>Lid %d van de %d</p>\n", $current_key+1, count($_SESSION['sel_leden']));
			echo("</div>  <!-- Einde opdrachtknoppen -->\n");
		}
	}
	
} # fnEigenGegevens

function overzichtverjaardagen($metfoto=1) {
	global $dtfmt;

	if ($metfoto != 0 and $_SESSION['settings']['toonpasfotoindiennietingelogd'] == 0 and $_SESSION['lidid'] == 0) {
		$metfoto = 0;
	}
	
	$i_lid = new cls_Lid();
	$i_foto = new cls_foto();

	$verj = "";
	$verjfoto = "";
	$aantgetoond = 0;
	$dteHV = new DateTime();
	for ($i = 0; $i < $_SESSION['settings']['verjaardagenvooruit'] and $aantgetoond < $_SESSION['settings']['verjaardagenaantal']; $i++) {
		foreach($i_lid->verjaardagen($dteHV->format("Y-m-d")) as $row) {
			$i_lid->vulvars($row->RecordID);
			$o = (new cls_Lidond())->onderscheiding($row->RecordID);
			if (strlen($o) > 0) {
				$nm = $o . " ";
			} else {
				$nm = "";
			}
			$nm .= htmlentities($row->NaamLid);
			if ($i == 0) {
				$t = sprintf("%s is vandaag %d jaar geworden", $nm, $row->Leeftijd);
			} elseif ($i == 1) {
				$t = sprintf("%s wordt morgen %d jaar", $nm, $row->Leeftijd);
			} else {
				$dtfmt->setPattern("EEEE d MMMM");
				$t = sprintf("Op %s is %s jarig", $dtfmt->format($dteHV), $nm);
			}
			if (strlen($t) > 3) {
				$fd = $i_foto->fotolid($row->RecordID);
				$verj .= sprintf("<li>%s</li> ", $t);
				if (strlen($fd) > 0) {
					$verjfoto .= sprintf("<div class='jarige'><img src='%s' class='rounded-circle' alt='Pasfoto %s'><div class='tekstbijfoto'>%s</div></div>\n", $fd, htmlentities($row->NaamLid), $t);
				} elseif (strlen($t) > 3) {
					$verjfoto .= sprintf("<p>%s</p>\n", $t);
				}
			}
			$aantgetoond++;
		}
		$dteHV->modify("+1 day");
	}
	$i_lid = null;
	if (strlen($verj) > 0) {
		$verj = "<div id='voorblad_verjaardagen'>\n<h3>Verjaardagen</h3>\n<ul>" . $verj . "</ul>\n</div>\n";
		$verjfoto = "<div id='voorblad_verjaardagen'>\n<h3>Verjaardagen</h3>\n" . $verjfoto . "</div>\n";
	}
	if ($metfoto == 1) {
		return $verjfoto;
	} else {
		return $verj;
	}
	
} # overzichtverjaardagen

function fotolid($lidid, $metversie=0) {
	
	$i_foto = new cls_Foto(-1, $lidid);

	$rv = $i_foto->fotolid();
	
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
	
	$i_lid = new cls_Lid($lidid);
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

	$sl = substr((new cls_Lidmaatschap())->soortlid($lidid), 0, 1);
		
	$wijzvelden[] = array('label' => "Roepnaam", 'naam' => "Roepnaam", 'lengte' => 17);
	$wijzvelden[] = array('label' => "Voorletters", 'naam' => "Voorletter", 'lengte' => 10, 'nietverw' => true);
	$wijzvelden[] = array('label' => "Tussenvoegsels", 'naam' => "Tussenv", 'lengte' => 7);
	$wijzvelden[] = array('label' => "Achternaam", 'naam' => "Achternaam", 'lengte' => 45, 'nietverw' => true);
	if (strlen($i_lid->meisjesnaam) > 0) {
		$wijzvelden[] = array('label' => "Meisjesnaam", 'naam' => "Meisjesnm", 'lengte' => 25);
	}
	$wijzvelden[] = array('label' => "Gender", 'naam' => "Geslacht", 'nietverw' => true);
	if ($i_lid->geslacht != "B") {
		$wijzvelden[] = array('label' => "Geboortedatum", 'naam' => "GEBDATUM", 'type' => 'date', 'titel' => "Geboortedatum");
		$wijzvelden[] = array('label' => "Geboorteplaats", 'naam' => "GEBPLAATS", 'lengte' => 22);
	}
	if ($sl == "K" or strlen($i_lid->opmerking) > 0) {
		$wijzvelden[] = array('label' => "Opmerking", 'naam' => "Opmerking", 'lengte' => 60);
	}
	
	$u = "";
	if (strlen($i_lid->postcode) == 0) {
		$u = "";
	} elseif (strlen($i_lid->postcode) < 6) {
		$u = "Geen juiste postcode ingevoerd";
	} else {
		$pdok = pdok($i_lid->postcode, $i_lid->huisnr, $i_lid->huisletter, $i_lid->toevoeging);
		if (!isset($pdok->numFound) or $pdok->numFound == 0) {
			if ($i_lid->huisnr > 0) {
				$u = "Adres bestaat niet";
			}
		} elseif (isset($pdok->docs[0]->huis_nlt)) {
			$adr = $pdok->docs[0]->straatnaam . " " . $pdok->docs[0]->huis_nlt;
			if ($adr != trim($i_lid->adres) and $pdok->numFound == 1) {
				$i_lid->update($lidid, "Adres", $adr);
				$row = $i_lid->record($lidid);
			}
			if (isset($pdok->docs[0]->huisletter) and $pdok->numFound == 1) {
				$i_lid->update($lidid, "Huisletter", $pdok->docs[0]->huisletter);
			}
			if (isset($pdok->docs[0]->huisnummertoevoeging) and $pdok->numFound == 1) {
				$i_lid->update($lidid, "Toevoeging", $pdok->docs[0]->huisnummertoevoeging);
			}
		}
	}
	$wijzvelden[] = array('label' => "Postcode", 'naam' => "Postcode", 'lengte' => 7);
	$wijzvelden[] = array('label' => "Huisnummer", 'naam' => "Huisnr", 'lengte' => 4, 'titel' => "Huisnummer");
	$wijzvelden[] = array('label' => "Letter", 'naam' => "Huisletter", 'lengte' => 2);
	$wijzvelden[] = array('label' => "Toevoeging", 'naam' => "Toevoeging", 'lengte' => 5, 'titel' => "Huisnummertoevoeging");
	$wijzvelden[] = array('label' => "Adres", 'naam' => "Adres", 'lengte' => 35, 'uitleg' => $u, 'readonly' => 0);
	$wijzvelden[] = array('label' => "Woonplaats", 'naam' => "Woonplaats", 'lengte' => 22, 'readonly' => 0);
	$wijzvelden[] = array('label' => "Vast telefoonnummer", 'naam' => "Telefoon", 'lengte' => 21);
	$wijzvelden[] = array('label' => "Mobiel", 'naam' => "Mobiel", 'lengte' => 21);
	if ($row->GEBDATUM < date("Y-m-d", strtotime("-8 year"))) {
		$wijzvelden[] = array('label' => "E-mail", 'naam' => "Email", 'lengte' => 45, 'titel' => "Persoonlijke e-mailadres van het lid");
	}
	if (($_SESSION['settings']['zs_incl_emailvereniging'] == 1 or $currenttab2 == "Wijzigen lid") and $row->GEBDATUM < date("Y-m-d", strtotime("-12 year")) and $sl != "K") {
		$wijzvelden[] = array('label' => "E-mail vereniging", 'naam' => "EmailVereniging", 'lengte' => 45, 'titel' => "Emailadres voor het verenigingswerk");
	}
	if (($_SESSION['settings']['zs_incl_emailouders'] == 1 or $currenttab2 == "Wijzigen lid") and $i_lid->geslacht != "B") {
		$wijzvelden[] = array('label' => "E-mail ouders", 'naam' => "EmailOuders", 'lengte' => 75);
		$wijzvelden[] = array('label' => "Namen ouders", 'naam' => "NamenOuders", 'lengte' => 90);
	}
	$wijzvelden[] = array('label' => "Waarschuwen bij nood", 'naam' => "Waarschuwen bij nood", 'lengte' => 254);
	
	if ($_SESSION['settings']['zs_incl_iban'] == 1 and $currenttab == "Zelfservice") {
		$wijzvelden[] = array('label' => "Bankrekening", 'naam' => "Bankrekening", 'lengte' => 18);
	}
	if ($_SESSION['settings']['zs_incl_machtiging'] == 1 and $currenttab == "Zelfservice") {
		$wijzvelden[] = array('label' => "Machtiging incasso", 'naam' => "Machtiging afgegeven", 'lengte' => 1, 'type' => 'cb');
	}
	
	if ($i_lid->geslacht != "B") {
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
		
		if ($currenttab2 == "Wijzigen lid") {
			$wijzvelden[] = array('label' => "Overleden", 'naam' => "Overleden", 'type' => 'date');
			$wijzvelden[] = array('label' => "Niet opschonen", 'naam' => "NietOpschonen", 'type' => "cb");
		}
	}
		
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
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

	$i_lm->vulvars(-1, $lidid);
//	printf("<form method='post' id='%s' class='form-check form-switch' action='%s'>\n", __FUNCTION__, $actionurl);
	printf("<div id='%s' class='form-check form-switch'>\n", __FUNCTION__);
	printf("<label id='lblRecordID' class='form-label'>RecordID/LidID</label><input name='lidid' class='w10' value=%d readonly>\n", $i_lid->lidid);
	if ($i_lm->lidnr > 0) {
		printf("<label id='lblLidnummer' class='form-label'>Lidnummer</label><input name='lidnr' class='w10' value=%d readonly></p>\n", $i_lm->lidnr);
	}
	
	for ($i=0; $i < count($wijzvelden); $i++) {
		
		$jsoc = sprintf("onBlur=\"savedata('lid', %d, this);\"", $row->RecordID, $wijzvelden[$i]['naam']);
		$dv = str_replace("\n", "<br>\n", $row->{$wijzvelden[$i]['naam']});
		if (isset($wijzvelden[$i]['readonly']) and $wijzvelden[$i]['readonly'] == 1) {
			$ro = " readonly";
		} else {
			$ro = "";
		}
			
		if (isset($wijzvelden[$i]['titel']) and strlen($wijzvelden[$i]['titel']) > 0) {
			$titel = trim($wijzvelden[$i]['titel']);
		} else {
			$titel = trim($wijzvelden[$i]['naam']);
		}

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
		if ($wijzvelden[$i]['naam'] == "Geslacht" or $wijzvelden[$i]['naam'] == "Legitimatietype") {
			if ($wijzvelden[$i]['naam'] == "Geslacht") {
				$an = constant("ARRGESLACHT");
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
			$inp = sprintf("<select id='%s' class='form-select form-select-sm' title='%s'>%s</select>", $wijzvelden[$i]['naam'], $wijzvelden[$i]['label'], $opt);
			
		} elseif (isset($wijzvelden[$i]['type']) and $wijzvelden[$i]['type'] == "cb") {
			if ($row->{$wijzvelden[$i]['naam']} == 1) {
				$dv = "Ja";
				$c = "checked";
			} else {
				$dv = "Nee";
				$c = "";
			}
			$inp = sprintf("<input type='checkbox' class='form-check-input' id='%s' value=1 title='%s' %s>", $wijzvelden[$i]['naam'], $titel, $c);
		
		} else {
			if (isset($wijzvelden[$i]['type']) and $wijzvelden[$i]['type'] == "date") {
				$t = "date";
			} elseif ($wijzvelden[$i]['naam'] == "Email" or $wijzvelden[$i]['naam'] == "EmailVereniging") {
				$t = "email";
			} else {
				$t = "text";
			}
			if (isset($wijzvelden[$i]['lengte'])) {
				$c = sprintf("class='w%d' ", $wijzvelden[$i]['lengte']);
			}
			
			if ($t == "text" and $wijzvelden[$i]['lengte'] > 100) {
				$inp = sprintf("<textarea id='%s' class='w90' placeholder='%s' rows=4 title='%4\$s'>%s</textarea>", $wijzvelden[$i]['naam'], $ph, $dv, $titel);
			} elseif ($t == "date") {
				$inp = sprintf("<input type='date' id='%1\$s' value='%2\$s'%3\$s title=\"%4\$s\">", $wijzvelden[$i]['naam'], $dv, $ro, $titel);
			} else {
				$inp = sprintf("<input type='%1\$s' %2\$sid='%3\$s' value='%4\$s' maxlength=%5\$d%6\$s title=\"%3\$s\">", $t, $c, $titel, $dv, $wijzvelden[$i]['lengte'], $ro);
			}
		}
		printf("<label id='lbl%s' class='form-label'>%s</label>%s", $wijzvelden[$i]['naam'], $wijzvelden[$i]['label'], $inp);
		
		$u = $wijzvelden[$i]['uitleg'] ?? "";
		printf("<p class='uitleg' id='uitleg_%s'>%s</p>", str_replace(" ", "_", strtolower($wijzvelden[$i]['naam'])), $u);
				
		echo("\n");
	}
	
	printf("<label class='form-label'>Ingevoerd op</label><p>%s</p>", $i_lid->ingevoerdtekst());
	
	printf("</div> <!-- Einde %s -->\n", __FUNCTION__);
	
	
	if ($currenttab != "Zelfservice" and toegang("Ledenlijst/Wijzigen lid/Eigenschappen", 0, 0)) {
		eigenschappenlidmuteren($lidid);
	}

	printf("<script>
		\$( document ).ready(function() {
			lidalgwijzprops();
		});

		\$('input, select, textarea').on('blur', function(){
			savedata('lid', %1\$d, this);
			if (this.id == 'Huisnr' || this.id == 'Huisletter' || this.id == 'Toevoeging') {
				$('#Adres').trigger('blur');
				$('#Woonplaats').trigger('blur');
			}
		});
		
		\$('input, #Geslacht').on('change', function(){
			lidalgwijzprops();
		});
		
		\$('#Overleden').blur('change', function(){
			if (\$('#Overleden').val().length >= 10) {
				alert('Door het invullen van de overlijdensdatum wordt het lid automatisch opgezegd en worden meerdere gegegevens leeg gemaakt.');
			}
		});
		
		
		</script>\n", $lidid);

} # algemeenlidmuteren

function eigenschappenlidmuteren($lidid) {
	
	$i_lo = new cls_Lidond(-1, $lidid);
	$i_ond = new cls_Onderdeel();	
	$i_ond->where = "`Type`='E' AND IFNULL(VervallenPer, '9999-12-31') >= CURDATE()";
	
	$ondrows = $i_ond->lijst();
	if (count($ondrows) > 0) {
		printf("<div id='%s' class='form-check form-switch'>\n", __FUNCTION__);
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
			printf("<input type='checkbox' class='form-check-input' id='eigenschap_%d'%s%s>", $ondrow->RecordID, $c, $ro);
			printf("<label for='eigenschap_%d'>%s</label>\n", $ondrow->RecordID, $ondrow->Naam);
		}
		echo("</div> <!-- Einde eigenschappenlidmuteren -->\n");
	}
	
	printf("<script>
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
		</script>\n", $lidid);
	
	$i_lo = null;
	$i_ond = null;
	
} # eigenschappenlidmuteren

function financieellidmuteren($lidid) {
	global $actionurl, $currenttab, $currenttab2;
	
	$i_lid = new cls_Lid($lidid);
	$i_lid->controle($lidid);
	$i_lo = new cls_Lidond();
	$i_lo->where = sprintf("LO.OnderdeelID=%d AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE()", $_SESSION['settings']['rekening_groep_betaalddoor']);
	
	printf("<h3>Financi&euml;le gegevens %s</h3>\n", (new cls_Lid())->Naam($lidid));
	printf("<form method='post' id='%s' class='form-check form-switch' action='%s'>\n", __FUNCTION__, $actionurl);
	
	printf("<label class='form-label'>Bankrekeningnummer</label><input type='text' id='Bankrekening' value='%s' class='w18'><p id='uitleg_bankrekening' class='uitleg'>IBAN</p>\n", $i_lid->bankrekening);
	
	if ($currenttab2 == "Wijzigen lid" and $_SESSION['settings']['rekening_groep_betaalddoor'] > 0 and $i_lo->aantal() > 0) {
		$opt = "<option value=0>Lid zelf</option>\n";
		foreach ($i_lo->lijst($_SESSION['settings']['rekening_groep_betaalddoor']) as $lorow) {
			$opt .= sprintf("<option value=%d%s>%s</option>\n", $lorow->LidID, checked($i_lid->rekeningbetaalddoor, "option", $lorow->LidID), $lorow->NaamLid);
		}
		printf("<label class='form-label'>Rekening betaald door</label><select class='form-select form-select-sm' id='RekeningBetaaldDoor'>\n%s</select>\n", $opt);
	}
	printf("<label class='form-label'>Machtiging incasso afgegeven</label><input type='checkbox' id='Machtiging afgegeven' class='form-check-input' value=1%s>\n", checked($i_lid->machtigingafgegeven));
	echo("</form>\n");
	
	printf("<script>
		\$('input, select').on('blur', function(){
			savedata('lid', %1\$d, this);
			lidfinwijzprops();
		});
		
		\$('input').on('click', function(){
			savedata('lid', %1\$d, this);
		});
		</script>\n", $lidid);
	
}  # financieellidmuteren

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
		}
	}

	printf("<div id='%s'>\n", __FUNCTION__);
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
		$actionurl = sprintf("%s?tp=%s&lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
		printf("<form method='post' id='%s' action='%s' enctype='multipart/form-data'>\n", __FUNCTION__, $actionurl);
		printf("<input type='hidden' name='MAX_FILE_SIZE' value=%d>\n", ($max_size_attachm * 2));
		printf("<label for='UploadFoto' class='form-label'>Nieuwe pasfoto %s</label>\n", $naamlid);
		printf("<input type='hidden' name='lidid' value=%d>\n", $lidid);
		echo("<input type='file' name='UploadFoto' id='UploadFoto'>&nbsp;");
		echo("<input type='submit' name='Upload' value='Insturen'>\n");
		echo("</form>\n");
		echo("<br>\n<ul>\n");
		echo("<li>Het ideale formaat van de pasfoto is 390&nbsp;pixels breed bij 500&nbsp;pixels hoog.</li>\n");
		printf("<li>De volgende extensies '%s' van het bestand zijn toegestaan.</li>\n", implode(",&nbsp;", PASFOTOEXTENTIES));
		printf("<li>Het bestand moet minimaal %d&nbsp;bytes groot zijn en mag niet groter dan %d&nbsp;KB zijn.</li>\n", $min_size_attachm, $max_size_attachm / 1024);
		if ($lidid == $_SESSION['lidid']) {
			echo("<li>Met het uploaden van deze pasfoto geef je toestemming om deze foto aan bezoekers van deze website te tonen.</li>\n");
		}
		echo("</ul>");
	} else {
		echo("<p>Je mag op dit moment geen pasfoto toevoegen, probeer het later nogmaals.</p>\n");
	}

	printf("</div>  <!-- Einde %s -->\n", __FUNCTION__);
} # nieuwepasfoto

function toestemmingenmuteren($lidid) {
	global $currenttab;
	
	$i_ond = new cls_Onderdeel();
	$i_lo = new cls_Lidond();
	
	if ($lidid != $_SESSION['lidid']) {
		$js = "OnChange='this.form.submit();'";
	} else {
		$js = "";
	}
	
	$actionurl = sprintf("%s?tp=%s&lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$mess = "";
		foreach ($i_ond->lijst(-1, "`Type`='T'") as $ondrow) {
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
	
	$i_lm = new cls_Lidmaatschap(-1, $lidid);
	$sl = $i_lm->soortlid($lidid);
	
	printf("<h3>Toestemmingen %s</h3>\n", (new cls_Lid())->Naam($lidid));
	printf("<form method='post' id='%s' action='%s'>\n", __FUNCTION__, $actionurl);
	
	$i_ond->where = "O.`Type`='T'";
	$rows = $i_ond->lijst(0, "", "", 0, "O.Kode");
	if (count($rows) > 0) {
		$auc = 0;
		$d = "";
		if ($sl == "Toekomstig lid") {
			$i_lo->per = $i_lm->lidvanaf;
		} elseif ($sl !== "Lid") {
			$d = " disabled";
		}
		echo("<div class='form-switch'>\n");
		foreach ($rows as $row) {
			if ($i_lo->islid($lidid, $row->RecordID)) {
				$c = " checked";
			} else {
				$c = "";
				$auc++;
			}
			printf("<label class='form-check-label' for='T_%s'>%s\n", $row->Kode, $row->Naam);
			printf("<input class='form-check-input' type='checkbox' id='T_%1\$s' name='T_%1\$s' value=1 %2\$s%3\$s>", $row->Kode, $c, $d);
			echo("</label>\n");
		}
		echo("</div>\n");
		if (strlen($_SESSION['settings']['uitleg_toestemmingen']) > 0) {
			printf("<p>%s</p>\n", $_SESSION['settings']['uitleg_toestemmingen']);
		}
	} else {
		echo("<p>Er zijn geen toestemmingen die gegeven kunnen worden.</p>\n");
	}
	
	if ($lidid == $_SESSION['lidid']) {
		$tb = "Bevestigen";
	} else {
		$tb = "Bewaren";
	}
	
	echo("<div id='opdrachtknoppen'>\n");
	if ($currenttab != "Zelfservice" and ($sl == "Lid" or $sl == "Toekomstig lid") and $auc > 1) {
		printf("<button type='button' class='%s' onClick=\"AlleToestemming();\">%s Alle toestemmingen</button>", CLASSBUTTON, ICONCHECK);
	}
	printf("<button type='submit' class='%s' name='Bevestigen'>%s %s</button>\n", CLASSBUTTON, ICONBEWAAR, $tb);
	echo("</div> <!-- Einde opdrachtknoppen -->\n");

	echo("</form>\n");
	
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
	$i_tp = new cls_Template();
	
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
<h2>Opzegging lidmaatschap</h2>
<p><label for='NaamLid' class='form-label'>Naam lid</label><input type='text' name='NaamLid' value='[%NAAMLID%]' readonly='readonly'>
<label class='form-label'>Lidnummer</label><input type='number' name='Lidnr' value='[%LIDNR%]' readonly='readonly'>
<label class='form-label'>Opzegging per</label><input type='date' name='OpzeggingPer' value='[%OPZEGGENVANAF%]'>
<textarea name='RedenOpmerking' title='Reden en/of opmerkingen' placeholder='Reden en/of opmerkingen'></textarea>
</fieldset>
<div id='opdrachtknoppen'>\n
<button type='submit' name='VerstuurOpzegging'>Verstuur opzegging</button>\n
</div> <!-- Einde opdrachtknoppen -->\n";
		
		$i_tp->vulvars(-1, "opzegging");
		if (strlen($i_tp->inhoud) > 0) {
			$content = $i_tp->inhoud;
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
		printf("<form method='post' id='%s' action='%s'>\n", __FUNCTION__, $actionurl);
		echo($content);
		echo("</form>\n");
		echo("</div>  <!-- Einde opzegformulier -->\n");
	}
} # opzegginglidmaatschap

function onderdelenlidmuteren($lidid, $p_type="G") {
	//Dit is onderdelen per lid

//	debug(__FUNCTION__ . " / type: " . $p_type);
	
	$i_f = new cls_functie();
	$i_lo = new cls_Lidond(-1, $lidid);
	$i_gr = new cls_Groep();
	$i_ond = new cls_Onderdeel();
	
	$actionurl = sprintf("%s?tp=%s&lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	
	$i_lo->opschonen($lidid);
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['btnToevoegenOnderdeel']) and isset($_POST['NieuwOnderdeel']) and $_POST['NieuwOnderdeel'] > 0) {
			$i_lo->add($_POST['NieuwOnderdeel'], $lidid);
		}
	}

	/*
	printf("<input type='hidden' id='lidid' value=%d>\n", $lidid);
	printf("<input type='hidden' id='ondtype' value='%s'>\n", $p_type);
	*/
	printf("<form method='post' id='%s' action='%s'>\n", __FUNCTION__, $actionurl);
	$kf = "";
	if ($p_type == "A") {
		$typeoms = "Afdelingen";
	} elseif ($p_type == "BCF") {
		$typeoms = "Bestuur, commissies en functionarissen";
		$kf = "<th>Functie</th><th>Functionele e-mail</th>";
	} elseif ($p_type == "O") {
		$typeoms = "Onderscheidingen";
	} else {
		$typeoms = "Groepen";
	}
	
	if ($p_type == "BCF") {
		$ond_f = "(O.Type IN ('B', 'C', 'F') OR O.Kader=1)";
	} elseif ($p_type == "G") {
		$ond_f = sprintf("O.Type='G' AND IFNULL(O.Kader, 0)=0");
	} else {
		$ond_f = sprintf("O.Type='%s'", $p_type);
	}
	$f = $ond_f . sprintf(" AND LO.Lid=%d", $lidid);
	
	$lorows = $i_lo->lijst(-1, $f, "", "", "", 0);
	
	$kols = null;
	if ($p_type == "A") {
		$kols[] = array('columnname' => "OndNaam", 'headertext' => "Afdeling", 'readonly' => true);
	} else {
		$kols[] = array('columnname' => "OndNaam", 'headertext' => "Onderdeel", 'readonly' => true);
	}
	$kols[] = array('columnname' => "Vanaf", 'type' => "date");
	if ($p_type == "A" or $p_type == "BCF") {
		$frows = (new cls_functie())->selectlijst($p_type, "", 1);
		$kols[] = array('columnname' => "Functie", 'headertext' => "Functie", 'bronselect' => $frows);

		if (count($lorows) > 0 and max(array_column($lorows, "Functie")) > 0) {
			$kols[] = array('columnname' => "EmailFunctie", 'headertext' => "E-mail bij functie", 'type' => "email", 'cond_ro' => "RO_Email");
		}
		if ($p_type == "A" and (new cls_Groep())->aantal() > 0) {
			$kols[] = array('columnname' => "GroepID", 'headertext' => "Groep", 'type' => "groep", 'columnfilter' => "OnderdeelID");
		}
	}
	
	if ($p_type != "O") {
		$kols[] = array('columnname' => "Opmerk", 'headertext' => "Opmerking");
		$kols[] = array('columnname' => "Opgezegd", 'headertext' => "Tot en met", 'type' => "date");
	}
	
	if ($p_type == "G") {
		$oc = sprintf("verwijderlo(%%d);");
		$kols[] = array('columnname' => "RecordID", 'headertext' => "&nbsp;", 'onclick' => $oc, 'class' => "trash");
	}
	
	$res = $i_lo->lijst(-1, $f, "", "", "", 0, 0);
	echo(fnEditTable($res, $kols, "lidond", $typeoms . " " . $i_lo->i_lid->naam));

	if ($lidid > 0) {
		$i_ond->where = $ond_f . " AND IFNULL(O.VervallenPer, '9999-12-31') >= CURDATE()";
		printf("<select name='NieuwOnderdeel' class='form-select'>\n<option value=0>Nieuw ....</option>\n%s</select>\n", $i_ond->htmloptions(-1, 0, "", 0));
		printf("<button type='submit' class='%s btn-sm' name='btnToevoegenOnderdeel'>%s</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
	}
	echo("</form>\n");
	
	echo("<script>
		function verwijderlo(loid) {
			$.ajax({
				url: 'ajax_update.php?entiteit=deletelidond',
				type: 'post',
				dataType: 'json',
				data: { loid: loid }
			});
			
			$('#trash_' + loid).hide();
			$('#OndNaam_' + loid).addClass('deleted');
			
		}
	</script>\n");
	
	$i_lo = null;
	$i_ond = null;

}  # onderdelenlidmuteren

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
	
	$inclVervallen = $_POST['inclVervallen'] ?? 0;
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['Extra']) and $_POST['Extra'] > 0) {
			$nrid = $i_ld->add($lidid, $_POST['Extra']);
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

//	echo("<p class='mededeling'>Werk in uitvoering, werkt dus niet zoals het zou moeten.</p>\n");
	printf("<div id='%s'>\n", __FUNCTION__);
	$actionurl = sprintf("%s?tp=%s&lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	printf("<form method='post' class='form-check form-switch' action='%s'>\n", $actionurl);
	
	$ldrows = $i_ld->overzichtlid($lidid);
	
	if (count($ldrows) > 8) {
		echo("<div id='filter'>\n");
		printf("<input id='tbFilterCodeNaam' placeholder='Code/naam bevat' OnKeyUp=\"fnFilter('%s', this);\">", __FUNCTION__);
		echo("</div> <!-- Einde filter -->\n");
	}
	
	printf("<table id='%s' class='%s'>\n", __FUNCTION__, TABLECLASSES);
	printf("<caption>Diploma's %s muteren</caption>\n", (new cls_Lid())->Naam($lidid));
	$minbehaald = date("d-m-Y", strtotime((new cls_Lid())->Geboortedatum($lidid)));
	echo("<thead>\n");
	printf("<tr><th>Code</th><th>Naam</th><th>Behaald op</th><th class='diplomanummer'>Diplomanummer</th><th class='geldigtot'>Geldig tot</th></tr>\n");
	echo("</thead>\n");
	echo("<tbody>\n");
	$script = "";
	$dtfmt->setPattern(DTSHORT);
	foreach ($ldrows as $ldrow) {
		printf("<tr>\n<td>%s</td>\n<td>%s</td>\n\n", $ldrow->Kode, $ldrow->NaamLang);
		if ($ldrow->Examen > 0) {
			$d = " disabled";
		} else {
			$d = "";
		}
		if ($ldrow->Zelfservice == 0 and $td == "ZS") {
			printf("<td>%s</td><td class='diplomanummer'>%s</td><td class='geldigtot'>%s</td>", $dtfmt->format(strtotime($ldrow->DatumBehaald)), $ldrow->Diplomanummer, $dtfmt->format(strtotime($ldrow->LicentieVervallenPer)));
		} else {
			$id = sprintf("DatumBehaald_%d", $ldrow->RecordID);
			printf("<td><input type='date' id='%s' value='%s' max='%s' required%s></td>\n", $id, $ldrow->DatumBehaald, date("Y-m-d"), $d);
//			$script .= sprintf("$('#%1\$s').datepicker();\n$('#%1\$s').datepicker('dateFormat', 'yy-mm-dd').datepicker('maxDate', new Date());\n$('#%1\$s').datepicker('setDate', '%3\$s').datepicker('option', 'gotoCurrent', true);\n", $id, $minbehaald, $ldrow->DatumBehaald);
			printf("<td class='diplomanummer'><input type='text' id='Diplomanummer_%d' value='%s' maxlength=30></td>\n", $ldrow->RecordID, $ldrow->Diplomanummer);
			printf("<td class='geldigtot'><input type='date' id='LicentieVervallenPer_%d' value='%s'></td>\n", $ldrow->RecordID, $ldrow->LicentieVervallenPer);
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
		$vf = sprintf("<label class='form-check-label'><input type='checkbox' class='form-check-input' name='inclVervallen' value=1 %s  OnChange='this.form.submit();'>Inclusief vervallen</label>\n", checked($inclVervallen));
	}
	printf("<select name='Extra' class='form-select-sm' OnChange='this.form.submit();'><option value=-1>Toevoegen ...</option>%s</select>%s\n", $i_dp->htmloptions(0, -1, $zs, getvar("inclVervallen", 0)), $vf);

	echo("</form>\n");
	
	if ($zs == 1) {
		echo("<div class='clear'></div>\n");
		echo("<p>Een diploma kan worden verwijderd door bij 'Geldig tot' een datum in te vullen die voor 'Behaald op' ligt.</p>\n");
	}
	
	printf("</div> <!-- Einde %s -->\n", __FUNCTION__);

	$i_dp = null;
	$i_ld = null;

	printf("<script>
				$(document).ready(function() {
					$('input').on('blur', function(){
						savedata('liddipl', 0, this);
					});
					
			%s
			
				});
		</script>\n", $script);
	
} # diplomaslidmuteren

function lidmaatschapmuteren($lidid) {
	
	$i_lm = new cls_Lidmaatschap(-1, $lidid);
	
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
		}
		
		if (isset($_POST['verstuurbevestiging']) and $_POST['verstuurbevestiging'] > 0 and $_SESSION['settings']['mailing_bevestigingopzegging'] > 0) {

			$mailing = new Mailing($_SESSION['settings']['mailing_bevestigingopzegging']);
			$mailing->xtranum = $_POST['verstuurbevestiging'];
			$mailing->send($i_lm->lidid(-1, $_POST['verstuurbevestiging']));
			$mailing = null;
		}
			
		if (isset($_POST['NieuwLidmaatschap'])) {
			$i_lm->add($lidid);
		} elseif (isset($_POST['verwijderlidmaatschap']) and $_POST['verwijderlidmaatschap'] > 0) {
			$i_lm->delete($_POST['verwijderlidmaatschap']);
		}
	} else {
		$i_lm->controle($lidid);
		$i_lm->opschonen($lidid);
	}
	
	$actionurl = sprintf("%s?tp=%s&lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	printf("<form method='post' action='%s'>\n", $actionurl);
	printf("<table id='%s' class='%s'>\n", __FUNCTION__, TABLECLASSES);
	printf("<caption>Lidmaatschappen %s</caption>\n", (new cls_Lid())->Naam($lidid));
	echo("<tr><th>RecordID</th><th>Lidnummer</th><th>Lid vanaf</th><th>Opgezegd per</th><th>Door vereniging?</th><th></th><th></th></tr>\n");
	$toev = true;
	$aant = 0;
	$i_lm->where = sprintf("LM.Lid=%d", $lidid);
	foreach ($i_lm->basislijst("", "LIDDATUM") as $row) {
		echo("<tr>\n");
		printf("<td>%d</td>\n", $row->RecordID);
		printf("<td><input type='number' class='d8' name='Lidnr_%d' value=%d onblur='this.form.submit();'></td>\n", $row->RecordID, $row->Lidnr);
		printf("<td><input type='date' name='Vanaf_%d' value='%s' onblur='this.form.submit();'></td>\n", $row->RecordID, $row->LIDDATUM);
		printf("<td><input type='date' name='Opgezegd_%d' value='%s' onblur='this.form.submit();'></td>\n", $row->RecordID, $row->Opgezegd);
		
		if ($row->Opgezegd > "1900-01-01") {
			printf("<td><input type='checkbox' class='form-check-input' name='OpgezegdDoorVereniging_%d' title='Heeft de vereniging het lidmaatschap opgezegd?' value=1 %s onClick='this.form.submit();'></td>\n", $row->RecordID, checked($row->OpgezegdDoorVereniging));
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
		
		if ($row->Opgezegd >= date("Y-m-d", strtotime("-18 month")) and $_SESSION['settings']['mailing_bevestigingopzegging'] > 0) {
			$i_mh = new cls_Mailing_hist();
			$i_mh->where = sprintf("MH.MailingID=%d AND MH.Xtra_Num=%d", $_SESSION['settings']['mailing_bevestigingopzegging'], $row->Lidnr);
			if ($i_mh->aantal() == 0) {
				printf("<td><button type='submit' class='%s btn-sm' name='verstuurbevestiging' value=%d title='Verstuur bevestiging'>%s bevestiging</td>\n", CLASSBUTTON, $row->Lidnr, ICONVERSTUUR);
			} else {
				echo("<td></td>\n");
			}
			$i_mh = null;
		} else {
			echo("<td></td>\n");
		}
		
		if ($row->Opgezegd > "1970-01-01" and $row->Opgezegd < date("Y-m-d")) {
			printf("<td><button type='submit' name='verwijderlidmaatschap' value=%d title='Verwijder lidmaatschap'>%s</td>\n", $row->RecordID, ICONVERWIJDER);
		} else {
			echo("<td></td>\n");
		}
		
		echo("</tr>\n");
		$aant++;
	}
	echo("</table>\n");
	
	if ($toev) {
		printf("<button type='submit' class='%s' name='NieuwLidmaatschap' value=1>%s Lidmaatschap</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
	}
	
	echo("</form>\n");
	$i_lm = null;
} # lidmaatschapmuteren

function instellingenledenmuteren() {
	global $currenttab, $currenttab2;
	
	$i_p = new cls_Parameter();
	$i_p->controle();
	$i_ond = new cls_Onderdeel();
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		foreach (array("zs_incl_beroep", "zs_incl_bsn", "zs_incl_emailouders", "zs_incl_emailvereniging", "zs_incl_iban", "zs_incl_machtiging", "zs_incl_legitimatie", "zs_incl_slid", "zs_opzeggingautomatischverwerken") as $c) {
			if (isset($_POST[$c])) {
				$_POST[$c] = 1;
			} else {
				$_POST[$c] = 0;
			}
		}
		
		foreach ($i_p->lijst() as $row) {
			if (isset($_POST[$row->Naam])) {
//				debug($row->Naam . ": " . $_POST[$row->Naam]);
				if ($row->Naam == "muteerbarememos") {
					$_POST[$row->Naam] = strtoupper($_POST[$row->Naam]);
				}
				$i_p->update($row->Naam, $_POST[$row->Naam]);
			}
		}
	}	
	$i_p->vulsessie();
	
	if ($_SESSION['settings']['logboek_lid_bewaartijd'] > $_SESSION['settings']['logboek_bewaartijd']) {
		$i_p->update("logboek_lid_bewaartijd", $_SESSION['settings']['logboek_bewaartijd'], "deze waarde mag niet groter zijn dan de algemene bewaartijd van het logboek.");
	}
	
	$sm = "";
	foreach(ARRSOORTMEMO as $k => $v) {
		if (strlen($sm) > 0) {
			$sm .= ", ";
		}
		$sm .= $k;
	}
	
	printf("<form method='post' id='ledenlijst_instellingen' class='form-check form-switch' action='%s?tp=%s/%s'>\n", $_SERVER['PHP_SELF'], $currenttab, $currenttab2);
	
	echo("<h2>Algemeen</h2>\n");

	printf("<label class='form-label'>Naam reddingsbrigade</label><input type='text' name='naamvereniging_reddingsbrigade' class='w50' value=\"%s\" OnChange='this.form.submit();'>\n", $_SESSION['settings']['naamvereniging_reddingsbrigade']);
	printf("<label class='form-label'>Sportlink relatienummer</label><input type='text' name='sportlink_vereniging_relcode' value=\"%s\" class='w7' maxlength=7 OnChange='this.form.submit();'>\n", $_SESSION['settings']['sportlink_vereniging_relcode']);
	
	printf("<label class='form-label'>Opzegtermijn in maanden</label><input type='number' class='num2' name='zs_opzegtermijn' value=%d OnChange='this.form.submit();'>\n", $_SESSION['settings']['zs_opzegtermijn']);
	printf("<label class='form-label'>Opzegging automatisch verwerken</label><input type='checkbox' class='form-check-input' name='zs_opzeggingautomatischverwerken' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_opzeggingautomatischverwerken']));
	
	printf("<label class='form-label'>Welke soorten memo's zijn in gebruik?</label><input type='text' name='muteerbarememos' value='%s' class='w21' onChange='this.form.submit();'><p>(scheiden met een komma) (%s)</p>", $_SESSION['settings']['muteerbarememos'], $sm);
	
	echo("<h2>Rekeningen</h2>\n");
	printf("<label class='form-label'>Groep voor betaald door</label><select name='rekening_groep_betaalddoor' class='form-select form-select-sm	' OnChange='this.form.submit();'><option value=0>Geen</option>%s</select>\n", $i_ond->htmloptions($_SESSION['settings']['rekening_groep_betaalddoor']));
		  
	echo("<h2>Agenda</h2>\n");
	printf("<label class='form-label'>Groep leden met verjaardag op de agenda</label><select name='agenda_verjaardagen' class='form-select form-select-sm' OnChange='this.form.submit();'>\n<option value=-1>Niemand</option>\n%s</select>\n", (new cls_eigen_lijst())->htmloptions($_SESSION['settings']['agenda_verjaardagen']));
	printf("<label class='form-label'>URL van de ICS voor feestdagen</label><input name='agenda_url_feestdagen' class='w110' value='%s'>\n", $_SESSION['settings']['agenda_url_feestdagen']);
	
	echo("<h2>Beschikbaar in de zelfservice</h2>\n");
	
	printf("<label class='form-label' for='zs_incl_beroep'>Beroep</label><input type='checkbox' class='form-check-input' id='zs_incl_beroep' name='zs_incl_beroep' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_beroep']));

	printf("<label class='form-label'>Burgerservicenummer (BSN)</label><input type='checkbox' class='form-check-input' name='zs_incl_bsn' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_bsn']));
	printf("<label class='form-label'>E-mail ouders</label><input type='checkbox' class='form-check-input' name='zs_incl_emailouders' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_emailouders']));
	printf("<label class='form-label'>E-mail vereniging</label><input type='checkbox' class='form-check-input' name='zs_incl_emailvereniging' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_emailvereniging']));
	printf("<label class='form-label'>Bankrekening / IBAN</label><input type='checkbox' class='form-check-input' name='zs_incl_iban' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_iban']));
	printf("<label class='form-label'>Machtiging incasso afgegeven</label><input type='checkbox' class='form-check-input' name='zs_incl_machtiging' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_machtiging']));
	printf("<label class='form-label'>Legitimatie</label><input type='checkbox' class='form-check-input' name='zs_incl_legitimatie' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_legitimatie']));
	printf("<label class='form-label'>Sportlink ID</label><input type='checkbox' class='form-check-input' name='zs_incl_slid' OnChange='this.form.submit();'%s>\n", checked($_SESSION['settings']['zs_incl_slid']));
	if (strlen($_SESSION['settings']['muteerbarememos']) > 0) {
		printf("<label class='form-label'>Soorten memo's</label><input type='text' name='zs_muteerbarememos' class='w21' value=\"%s\" OnChange='this.form.submit();'><p>(Scheiden met een komma) (%s)</p>\n", $_SESSION['settings']['zs_muteerbarememos'], $_SESSION['settings']['muteerbarememos']);
	} else {
		echo("<input type='hidden' name=zs_muteerbarememos' value=''>\n");
	}
	
	echo("<h2>Overig in de zelfservice</h2>\n");
	printf("<label class='form-label'>Tekst als uitleg bij de toestemmingen</label><textarea name='uitleg_toestemmingen' rows=2 cols=68 OnChange='this.form.submit();'>%s</textarea>\n", $_SESSION['settings']['uitleg_toestemmingen']);
	
	echo("<h2>Retentie / opschonen</h2>\n");
	printf("<label class='form-label'>Leden, onderdelen per lid en lidmaatschappen</label><input type='number' class='num3' value=%d><p>in maanden na einde laatste lidmaatschap</p>\n", $_SESSION['settings']['ledenopschonen']);
	printf("<label class='form-label'>Lid definitief verwijderen</label><input type='number' class='num2' name='liddefinitiefverwijderen' value=%d OnChange='this.form.submit();'><p>aantal maanden na verwijdermarkering</p>\n", $_SESSION['settings']['liddefinitiefverwijderen']);
	printf("<label class='form-label'>Diploma's van lid</label><input type='number' class='num2' name='liddipl_bewaartermijn' value=%d OnChange='this.form.submit();'><p>aantal maanden na einde lidmaatschap</p>\n", $_SESSION['settings']['liddipl_bewaartermijn']);
	printf("<label class='form-label'>Aan een lid gekoppelde logging</label><input type='number' class='num2' name='logboek_lid_bewaartijd' value=%d OnChange='this.form.submit();'><p>aantal maanden</p>\n", $_SESSION['settings']['logboek_lid_bewaartijd']);
	printf("<label class='form-label'>Foto's van leden</label><input type='number' class='num2' name='fotosledenverwijderen' value=%d OnChange='this.form.submit();'><p>aantal maanden na beëindiging lidmaatschap</p>\n", $_SESSION['settings']['fotosledenverwijderen']);
	echo("</form>\n");
	
}  # instellingenledenmuteren

function fnPersoonlijkeAgenda() {
	global $dtfmt;
	
	$i_ak = new cls_Afdelingskalender();
	$i_lo = new cls_Lidond();
	$i_dp = new cls_Diploma();
	$i_ld = new cls_Liddipl();
	$i_ond = new cls_Onderdeel();
	$i_ex = new cls_Examen();
	$i_ed = new cls_Evenement_Deelnemer();
	$kalitem = null;
	$in = 0;
	
	$dtStart = strtotime("-1 day");
	
	for ($sw=$dtStart;$sw <= strtotime("+29 day");$sw=strtotime("+1 day", $sw)) {
		$hd = date("Y-m-d", $sw);
		
		// Afdelingskalender
		if ($sw < strtotime("+14 day")) {
			$sq = sprintf("SELECT LO.OnderdeelID FROM %sLidond AS LO WHERE IFNULL(LO.Opgezegd, '9999-12-31') >= '%s' AND LO.Lid=%d", TABLE_PREFIX, date("Y-m-d", $sw), $_SESSION['lidid']);
			$f = sprintf("AK.Datum='%s' AND AK.OnderdeelID IN (%s)", date("Y-m-d", $sw), $sq);
			foreach ($i_ak->lijst(-1, "", $f, "AK.Datum", 3) as $row) {
				$kalitem[$in]['datum'] = $hd;

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
				$i_ex->vulvars(-1, $hd, $row->OnderdeelID);
				if ($i_ex->exid > 0) {
					$f = sprintf("LD.Lid=%d AND LD.Examen=%d", $_SESSION['lidid'], $i_ex->exid);
					$dpid = $i_ld->max("DiplomaID", $f);
					if ($dpid > 0) {
						$oms .= " (" . $i_ex->examenomskort . " " . $i_dp->naam($dpid) . ")";
					}
				}
				$kalitem[$in]['oms'] = $oms;
				$in++;
			}
		}
	
		// Evenementen
		foreach ((new cls_Evenement())->lijst(4, $hd) as $evrow) {
			$i_ed->vulvars($evrow->RecordID, $_SESSION['lidid'], 0);
			$o = $i_ed->i_ev->omschrijving;
			if ($i_ed->status == "B" or $i_ed->status == "J" or $i_ed->status == "I") {
				if ($i_ed->onlineinschrijving == 1 and $i_ed->i_ev->datum >= date("Y-m-d") and toegang("Zelfservice/Evenementen", 0, 0)) { 
					$o = sprintf("<a href='%s/index.php?tp=Zelfservice/Evenementen'>%s</a>", BASISURL, $o);
				} elseif (toegang("Evenementen/Overzicht", 0) and $evrow->Dln > 0) {
					$o = sprintf("<a href='%s/index.php?tp=Evenementen/Overzicht'>%s</a>", BASISURL, $o);
				}
				$kalitem[$in]['datum'] = $i_ed->i_ev->datum . " " . trim($i_ed->starttijd);
					
				if ($i_ed->edid > 0) {
					if ($i_ed->status != "B") {
						$o .= ": " . $i_ed->statusoms;
					}
					if (strlen($i_ed->functie) > 0) {
						$o .= sprintf(" als %s", $i_ed->functie);
					}
				}
				$kalitem[$in]['oms'] = $o;
				$in++;
			}
		}

		$f = sprintf("EX.Datum='%s' AND EX.OnderdeelID=0", $hd);
		$ldrows = $i_ld->overzichtperexamen(-1, -1, $_SESSION['lidid'], $f);	
		foreach ($ldrows as $ldrow) {
			if ($ldrow->Proefexamen == 1) {
				$o = "Proefexamen";
			} else {
				$o = "Examen";
			}
			$r = array_search($ldrow->Datum, array_column($kalitem, "datum"));
			if ($r === false) {
				$kalitem[$in]['datum'] = $ldrow->Datum;
				$kalitem[$in]['oms'] = $o;
				$in++;
			} else {
				$kalitem[$r]['oms'] .= " / " . $o;
			}
		}
	}
	
	$rv = "";
	if (isset($kalitem) and count($kalitem) > 0) {
		$rv = "<div id='persoonlijkeagenda'>\n<h3>Persoonlijke agenda</h3>\n<ul>\n";
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
		$rv .= "</ul>\n</div> <!-- Einde persoonlijkeagenda -->\n";
	}

	return $rv;
	
}  # fnPersoonlijkeAgenda

function fnBasisgegevens($p_type) {
	
	if ($p_type == "Onderdelen") {
		$i_ond = new cls_Onderdeel();
		
		$filtertypeonderdeel = $_POST['filtertypeonderdeel'] ?? "*";
		$inclvervallen = $_POST['inclvervallen'] ?? 0;
		
		if (isset($_POST['NieuwOnderdeel'])) {
			$i_ond->add($filtertypeonderdeel);
		} else {
			$i_ond->controle();
		}
		
		printf("<form method='post' class='form-check form-switch' id='filter' action='%s?%s'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
		$opt = "<option value='*'>Filter op type ...</option>\n";
		foreach (ARRTYPEONDERDEEL as $k => $v) {
			$s = "";
			$i_ond->where = sprintf("O.`Type`='%s'", $k);
			if ($i_ond->aantal() > 1) {
				if ($filtertypeonderdeel == $k) {
					$s = " selected";
				}
				$opt .= sprintf("<option value='%s'%s>%s</option>\n", $k, $s, $v);
			}
		}
		$i_ond->where = "";
		printf("<select name='filtertypeonderdeel' class='form-select form-select-sm' onChange='this.form.submit();'>\n%s</select>\n", $opt);
		printf("<label class='form-check-label'><input type='checkbox' class='form-check-input' name='inclvervallen' id='inclvervallen' value=1%s onClick='this.form.submit();'>Inclusief vervallen</label>\n", checked($inclvervallen));
		echo("</form>\n");

		$f = "";
		if (strlen($filtertypeonderdeel) > 0 and $filtertypeonderdeel != "*") {
			$f = sprintf("O.`Type`='%s'", $filtertypeonderdeel);
		}
		if (intval($inclvervallen) != 1) {
			if (strlen($f) > 1) {
				$f .= " AND ";
			}
			$f .= "IFNULL(O.VervallenPer, '9999-12-31') >= CURDATE()";
		}
		
		$ondrows = $i_ond->lijst(0, $f, "", 0, "O.`Type`, O.Naam", 0);
		
		$l = sprintf("%s?tp=%s&Scherm=W&OnderdeelID=%%d", $_SERVER['PHP_SELF'], $_GET['tp']);
		$kols[] = array('headertext' => "&nbsp;", 'columnname' => "RecordID", 'link' => $l, 'type' => "link", 'class' => 'muteren');
		$kols[] = array('headertext' => "Code", 'columnname' => "Kode");
		$kols[] = array('headertext' => "Naam", 'columnname' => "Naam");
		if ($filtertypeonderdeel == "*") {
			$kols[] = array('headertext' => "Type", 'bronselect' => ARRTYPEONDERDEEL, 'columnname' => "Type");
		}
		$kols[] = array('headertext' => "Kader", 'type' => "checkbox", 'columnname' => "Kader");
		$kols[] = array('headertext' => "Alleen leden", 'type' => "checkbox", 'columnname' => "Alleen leden");
		$kols[] = array('headertext' => "Vervallen per", 'type' => "date", 'columnname' => "VervallenPer");
		
		echo(fnEditTable($ondrows, $kols, "onderdeeledit", "Muteren onderdelen"));
		
		echo("<div id='opdrachtknoppen'>\n");
		printf("<form method='post' action='%s?%s'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
		printf("<button type='submit' class='%s' name='NieuwOnderdeel' onClick='this.form.submit();'>%s Onderdeel</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
		echo("</form>\n");
		echo("</div> <! =-- Einde opdrachtknoppen -->\n");
		
		
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
		
		echo(fnEditTable($ondrows, $kols, "onderdeeledit", "Muteren onderdelen"));
		
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
		printf("<button type='submit' class='%s' name='NieuweFunctie'>%s Functie</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
		
		$i_ond = null;
		
	} elseif ($p_type == "Activiteiten") {
		$i_act = new cls_Activiteit();
		$i_act->controle();

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
		
		$kols[4]['headertext'] = "GBR";
		$kols[4]['columnname'] = "GBR";
		$kols[4]['type'] = "text";
		
		$kols[5] = ['headertext' => "Max aantal", 'columnname' => "BeperkingAantal", 'type' => "integer", 'title' => "Hoeveel mag er per seizoen worden gezwommen. 0=alle keren."];
				
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']); 
		
		echo(fnEditTable($res, $kols, "activiteitedit", "Muteren activiteiten"));
		
		echo("<div id='opdrachtknoppen'>\n");
		printf("<button type='submit' class='%s' name='NieuweActiviteit'>%s Activiteit</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
		
		$i_ond = null;

	} elseif ($p_type == "Organisaties") {
		$i_org = new cls_Organisatie();
		$i_org->controle();
		
		if (isset($_POST['NieuweOrganisatie'])) {
			$i_org->add();
		} elseif (isset($_GET['op']) and $_GET['op'] == "delete" and isset($_GET['OrgNr']) and $_GET['OrgNr'] > 0) {
			$i_org->delete($_GET['OrgNr']);
		}
		
		$orgres = $i_org->lijst(0, 0);
		
		$kols[] = array('columnname' => "Nummer", 'type' => "pk", 'cond_ro' => "inGebruik");
		$kols[] = array('columnname' => "Naam", 'headertext' => "Afkorting");
		$kols[]['columnname'] = "Volledige naam";

		$l = sprintf("%s?tp=%s&op=delete&OrgNr=%%d", $_SERVER['PHP_SELF'], $_GET['tp']);
		$kols[] = array('headertext' => "&nbsp;", 'columnname' => "Nummer", 'link' => $l, 'cond_ro' => "inGebruik", 'class' => "trash");
		
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		
		echo(fnEditTable($orgres, $kols, "organisatieedit", "Muteren organisaties"));
		
		echo("<div id='opdrachtknoppen'>\n");
		printf("<button type='submit' class='%s' name='NieuweOrganisatie'>%s Organisatie</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
		
	} elseif ($p_type == "Seizoenen") {
		$i_sz = new cls_Seizoen();
		$i_sz->controle();
		
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
		printf("<button type='submit' class='%s' name='NieuwSeizoen'>%s Seizoen</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
		echo("</div> <! =-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
		
	} else {
		echo("<p class='mededeling'>Dit gedeelte is nog niet gebouwd.</p>\n");
	}
}  # fnBasisgegevens

function presentielijst($p_evenement=0) {
	
	$ondid = $_POST['onderdeelid'] ?? 0;
	$titellijst = $_POST['titellijst'] ?? "";
	$eventid = $_POST['eventid'] ?? 0;
	
	$i_ond = new cls_Onderdeel();
	$i_ev = new cls_Evenement($eventid);
	
	if (strlen($titellijst) == 0 and $eventid > 0) {
		$titellijst = $i_ev->evoms;
	}
	
	$actionurl = sprintf("%s?tp=%s", $_SERVER['PHP_SELF'], $_GET['tp']);
	printf("<form method='post' id='filter' action='%s'>\n", $actionurl);
	if ($p_evenement == 0) {
		printf("<select name='onderdeelid' class='form-select form-select-sm' onChange='this.form.submit();'><option value=0>Selecteer groep ...</option>\n%s</select>\n", $i_ond->htmloptions($ondid, 1));
	}
	printf("<select name='eventid' class='form-select form-select-sm' title='Selecteer evenement' onChange='this.form.submit();'><option value=0>Selecteer evenement ...</option>\n%s</select>\n", $i_ev->htmloptions($eventid));
	printf("<input type='text' name='titellijst' id='titellijst' placeholder='Titel presentielijst' value='%s' onBlur='this.form.submit();'>", $titellijst);
	echo("</form>\n");
	
	if ($ondid > 0 or $eventid > 0) {
		$i_lid = new cls_Lid();
		$i_ed = new cls_Evenement_Deelnemer();
		
		$aant = 0;
		printf("<table id='%s' class='%s'>\n", __FUNCTION__, TABLECLASSES);
		if (strlen($titellijst) > 1)  {
			printf("<thead>\n<tr><th colspan=2>%s</th></tr></thead>\n", $titellijst);
		}
		echo("<tbody>\n");
		if ($ondid > 0) {
			$rows = $i_lid->ledenlijst(1, $ondid);
		} else {
			$rows = $i_ed->lijst($eventid);
		}
		foreach ($rows as $row) {
			if ($aant == 0) {
				echo("<tr>");
			} elseif ($aant/2 == intval($aant/2)) {
				echo("</tr>\n<tr>");
			}
			$nm = $row->NaamLid;
			$stat = "";
			if ($eventid > 0) {
				$i_ed->vulvars($eventid, $row->RecordID, 0);
				if (strlen($i_ed->functie) > 0) {
					$nm .= " (" . $i_ed->functie . ")";
				}
				if (strlen($i_ed->statusoms) > 0) {
					$stat = ": " . $i_ed->statusoms;
				}
			}
			
			printf("<td>%d %s%s</td>", $row->Lidnr, $nm, $stat);
			$aant++;
		}
		
		echo("</tbody>\n");
		echo("</table>\n");	
	}
	
}  # presentielijst

function overzichttoestemmingen($p_ondid=0) {		
	$i_ond = new cls_Onderdeel();
	$i_lid = new cls_Lid();
	$i_lo = new cls_Lidond();

	$aant = 0;
	printf("<table id='overzichttoestemmingen' class='%s'>\n", TABLECLASSES);
	echo("<thead>\n<tr><th>Naam lid</th>");
	$f = "O.`Type`='T' AND IFNULL(O.VervallenPer, '9999-12-31') >= CURDATE()";
	foreach($i_ond->basislijst($f, "O.Kode") as $row) {
		printf("<th title=\"%s\">%s</th>", $row->Naam, $row->Kode);
	}
	echo("</tr></thead>\n");
	echo("<tbody>\n");
	foreach ($i_lid->ledenlijst(1, $p_ondid) as $row) {
		$rij = sprintf("<tr><td>%s</td>", $row->NaamLid);
		$toon = false;
		foreach($i_ond->basislijst($f, "O.Kode") as $ondrow) {
			$f2 = sprintf("LO.OnderdeelID=%d AND LO.Lid=%d AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE()", $ondrow->RecordID, $row->RecordID);
			if ($i_lo->aantal($f2) > 0) {
				$r = "Ja";
				$toon = true;
			} else {
				$r = "";
			}
			$rij .= sprintf("<td>%s</td>", $r);
		}
		$rij .= "</tr>\n";
		if ($toon) {
			echo($rij);
		}
	}
		
	echo("</tbody>\n");
	echo("</table>\n");
	
}  # overzichttoestemmingen

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
	$query .= " AND (L.Verwijderd IS NULL) ORDER BY L.Achternaam, L.Voorletter;";
   
	$rows = $i_ond->execsql($query)->fetchAll();
	
	$actionurl = sprintf("%s?tp=%s", $_SERVER['PHP_SELF'], $_GET['tp']);
	printf("<form method='post' id='filter' class='form-check form-switch' action='%s'>\n", $actionurl);
	printf("<label>Ledenopgave per</label><input type='date' name='opgaveper' value='%s' onBlur='this.form.submit();'>\n", $opgaveper);
	if ($i_ond->aantal("O.ORGANIS=2") > 0) {
		printf("<label>Alleen leden van NCS-afdelingen?</label><input type='checkbox' name='ncsafd' class='form-check-input' value=1%s onClick='this.form.submit();'>\n", checked($ncsafd));
	}
	printf("<p class='aantrecords'>%d Leden</p>\n", count($rows));
	echo("</form>\n");

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
	printf("<form method='post' id='sportlink' action='%s'>\n", $actionurl);
	echo("<label class='form-label'>Export uit Sportlink</label><textarea name='exportsl' id='sportlinkdata'></textarea>\n");
	printf("<button type='submit' class='%s'>%s Toon resultaat</button>\n", CLASSBUTTON, ICONLIJST);
	echo("<p>Plak hierboven de export uit Sportlink, puntkomma-gescheiden en met onderstaande kolommen.</p>\n");
	echo("</form>\n");
	
	printf("<table class='%s'>\n<tr>", TABLECLASSES);
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
					$i_lid->controle($r->RecordID);
					$i_lid->vulvars($r->RecordID);
					if (trim($kolommen[1]) !== trim($r->TUSSENV)) {
						$verschil = $Kolomhoofd[1] . " lokaal: " . $r->TUSSENV;
					
					} elseif ($kolommen[2] !== $r->Achternaam) {
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
	
	$datumvanaf = $_POST['datumvanaf'] ?? date("Y-m-d", strtotime("-1 year"));
	$datumtot = $_POST['datumtot'] ?? date("Y-m-d");
	
	$maandenterug = $_POST['maandenterug'] ?? 12;
	$jubileumjaren = $_SESSION['settings']['jubileumjaren'] ?? "12.5;25;50";
	$kaderjubileumjaren = $_SESSION['settings']['kaderjubileumjaren'] ?? "5;12.5;25;50";
	$jubileumjaren = str_replace(",", ".", $jubileumjaren);
	$kaderjubileumjaren = str_replace(",", ".", $kaderjubileumjaren);

	printf("<div id='%s'>\n", __FUNCTION__);
	$actionurl = sprintf("%s?tp=%s", $_SERVER['PHP_SELF'], $_GET['tp']);
	printf("<form method='post' id='filter' action='%s'>\n", $actionurl);
	printf("<label class='form-label'>Periode vanaf</label><input type='date' value='%s' name='datumvanaf'>", $datumvanaf);
	printf("<label class='form-label'>tot en met</label><input type='date' value='%s' name='datumtot'>", $datumtot);
	echo("<div class='clear'></div>\n");
	printf("<label class='form-label'>Jubileumjaren</label><input type='text' name='jubileumjaren' id='jubileumjaren' value='%s' class='w60' onBlur='saveparam(this);'><p>Puntkomma-gescheiden</p>\n", $jubileumjaren);
	echo("<div class='clear'></div>\n");
	printf("<label class='form-label'>Groep met kader</label><select id='kaderonderdeelid' class='form-select form-select-sm' onChange='saveparam(this);'><option value=-1>Geen</option>\n%s</select>\n", $i_ond->htmloptions($_SESSION['settings']['kaderonderdeelid'], 1, "G"));
	printf("<label class='form-label'>Jubileumjaren voor kader</label><input type='text' name='jubileumjaren' id='kaderjubileumjaren' value='%s' class='w60' onBlur='saveparam(this);'><p>Puntkomma-gescheiden</p>\n", $kaderjubileumjaren);

	echo("<div id='opdrachtknoppen'>\n");
	printf("<button type='submit' class='%s'>%s Toon resultaat</button>\n", CLASSBUTTON, ICONLIJST);
	echo("</div> <!-- Einde opdrachtknoppen -->\n");
	echo("</form>\n");
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
	
		$dagenterug = (strtotime($datumtot) - strtotime($datumvanaf)) / (60 * 60 *24);
		$rows = $i_lid->jubilarissen($datumtot);
		
		printf("<table class='%s'>\n", TABLECLASSES);
		echo("<thead><tr><th>Naam lid</th><th>Lidmaatschap</th><th>Kaderlidmaatschap</th></tr></thead>\n");
		foreach($rows as $row) {
			$jub = "";
			$kadjub = "";
			foreach (explode(";", $jubileumjaren) as $j) {
				$ad = floatval($j) * 365.25;
				if ($row->LengteLidmaatschap >= $ad and $row->LengteLidmaatschap < ($ad+$dagenterug)) {
					$jub = str_replace(".", ",", $j);
				}
			}
			if ($row->EindeKaderlidmaatschap > date("Y-m-d", strtotime($datumvanaf))) {
				foreach (explode(";", $kaderjubileumjaren) as $j) {
					$ad = floatval($j) * 365.25;
					if ($row->LengteKaderlidmaatschap >= $ad and $row->LengteKaderlidmaatschap < ($ad+$dagenterug)) {
						$kadjub = str_replace(".", ",", $j);
					}
				}
			}
			if (strlen($jub) > 0 or strlen($kadjub) > 0) {
				$nm = $row->Naam;
				$tab = "Ledenlijst/Overzicht lid";
				if (strlen($kadjub) > 0 and toegang($tab . "/Kader", 0, 0)) {
					$tab .= "/Kader";
				}
				if (toegang($tab, 0, 0)) {
					$nm = sprintf("<a href='/index.php?tp=%s&lidid=%d'>%s</a>", $tab, $row->RecordID, $row->Naam);
				}
				printf("<tr><td>%s</td><td class='number'>%s</td><td class='number'>%s</td></tr>\n", $nm, $jub, $kadjub);
			}
		}
		echo("</table>\n");
	}
	echo("</div> <!-- jubilarisoverzicht -->\n");

}  # jubilarissen

function pdok($p_postcode, $p_huisnr=0, $p_letter="", $p_toev="") {
	
	$p_postcode = strtoupper(trim($p_postcode));
	$p_postcode = str_replace(" ", "", $p_postcode);
	$p_letter = trim($p_letter);
	$p_toev = trim($p_toev);

	if (strlen($p_postcode) == 6) {
		$curl= curl_init();
		curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
		$url = sprintf("https://api.pdok.nl/bzk/locatieserver/search/v3_1/free?fq=postcode:%s", $p_postcode);
		if ($p_huisnr > 0) {
			$url .= sprintf("&fq=huisnummer:%d", $p_huisnr);
			if (strlen($p_letter) > 0) {
				$url .= "&fq=huisletter:" . $p_letter;
			}
			if (strlen($p_toev) > 0) {
				$url .= "&fq=huisnummertoevoeging:" . $p_toev;
			}
		}
//		debug($url);
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
   
   $arrCountry = array('al'=>28,'ad'=>24,'at'=>20,'az'=>28,'bh'=>22,'be'=>16,'ba'=>20,'br'=>29,'bg'=>22,'cr'=>21,'hr'=>21,'cy'=>28,'cz'=>24,'dk'=>18,'do'=>28,'ee'=>20,'fo'=>18,'fi'=>18,'fr'=>27,'ge'=>22,'de'=>22,'gi'=>23,'gr'=>27,'gl'=>18,'gt'=>28,'hu'=>28,'is'=>26,'ie'=>22,'il'=>23,'it'=>27,'jo'=>30,'kz'=>20,'kw'=>30,'lv'=>21,'lb'=>28,'li'=>21,'lt'=>20,'lu'=>20,'mk'=>19,'mt'=>31,'mr'=>27,'mu'=>30,'mc'=>27,'md'=>24,'me'=>22,'nl'=>18,'no'=>15,'pk'=>24,'ps'=>29,'pl'=>28,'pt'=>25,'qa'=>29,'ro'=>24,'sm'=>27,'sa'=>24,'rs'=>22,'sk'=>24,'si'=>19,'es'=>24,'se'=>24,'ch'=>21,'tn'=>24,'tr'=>26,'ae'=>23,'gb'=>22,'vg'=>24);
    
   $IBAN = strtoupper(str_replace(" ", "", $IBAN));
   if (strlen($IBAN) == 0 and $LeegIsGoed == 1) {
		$retval = true;
		
   } elseif (array_key_exists(substr(strtolower($IBAN), 0, 2), $arrCountry) === false) {
	   $retval = false;
	  
   } elseif (strlen($IBAN) == $arrCountry[substr(strtolower($IBAN), 0, 2)]) {
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
