<?php
		
if (toegang("Overzicht lid", 0)) {
	$ldl = "<a href='index.php?tp=Overzicht+lid&amp;lidid=%d'>%s</a>";
} else {
	$ldl = "";
}

if (toegang("Wijzigen lid", 0)) {
	$llw = "<a href='index.php?tp=Wijzigen+lid&amp;lidid=%d'>%s</a>";
} else {
	$llw = "";
}

function fnWijzigen($lidid=0, $actie="") {
	global $currenttab, $currenttab2, $table_prefix;
	global $muteerbarememos, $arrLegitimatie, $arrSoortMemo, $arrGeslacht, $actionurl;
	
	$emailledenadministratie = db_param("emailledenadministratie");
	$opzegtermijn = db_param("zs_opzegtermijn");
	
	if ($lidid == 0) {
		$lidid = $_SESSION['lidid'];
	}
	if (strlen($actie) == 0) {
		$actie = $currenttab2;
	}
	
	if ($lidid != $_SESSION['lidid']) {
		$xtra_param = sprintf("lidid=%d", $lidid);
		$actionurl = sprintf("%s?tp=%s&amp;lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	} else {
		$xtra_param = "";
		$actionurl = sprintf("%s?tp=%s", $_SERVER['PHP_SELF'], $_GET['tp']);
	}
	
	$naamlid = db_naamlid($lidid);
	if ($lidid > 0) {
		fnDispMenu(2, $xtra_param);

		if ($actie == "Algemene gegevens") {
			algemeenlidmuteren($lidid);
			
		} elseif ($actie == "Diplomas") {
			if ($currenttab == "Zelfservice") {
				diplomaslidmuteren($lidid, "ZS");
			} else {
				diplomaslidmuteren($lidid, "*");
			}
			
		} elseif ($currenttab2 == "Pasfoto") {
			nieuwepasfoto($lidid);
			
		} elseif ($actie == "Bijzonderheden" and count($muteerbarememos) > 0) {
			if ($_SERVER['REQUEST_METHOD'] == "POST") {
				foreach ($muteerbarememos as $kodesoort) {
					$namevar = "Memo_" . $kodesoort;
					$curval = db_memo("curval", $lidid, $kodesoort);
					if (strlen($_POST[$namevar]) == 0 and $kodesoort != "WN") {
						db_memo("delete", $lidid, $kodesoort);
					} elseif (strlen($curval) == 0 and $kodesoort != "WN") {
						db_memo("add", $lidid, $kodesoort, $_POST[$namevar]);
					} elseif ($curval != $_POST[$namevar]) {
						db_memo("update", $lidid, $kodesoort, $_POST[$namevar]);
					}
				}
			}
			echo("<div id='bijzonderhedenwijzigen'>\n");
			printf("<form method='post' action='%s' name='bijz_wijz'>\n", $actionurl);
			echo("<table>\n");
			foreach ($muteerbarememos as $kodesoort) {
				$namevar = "Memo_" . $kodesoort;
				if (array_key_exists($kodesoort, $arrSoortMemo)) {
					$naamsoort = $arrSoortMemo[$kodesoort];
				} else {
					$naamsoort = "Overig " . $kodesoort;
				}
				$curval = db_memo("curval", $lidid, $kodesoort);
				printf("<tr><td class='label'>%s</td><td><textarea cols=75 rows=10 name='%s'>%s</textarea></td></tr>\n", $naamsoort, $namevar, $curval);
			}
			echo("<tr><th colspan=2><input type='submit' value='Bewaren' name='wijziging'></th></tr>\n");
			echo("</table>\n");
			echo("</form>\n");
			echo("</div>  <!-- Einde bijzonderhedenwijzigen -->\n");

		} elseif ($actie == "Opzegging") {
				opzegginglidmaatschap($_SESSION['lidid']);
				
		} elseif ($actie == "Lidmaatschap") {
				lidmaatschapmuteren($lidid);
			
		} elseif ($actie == "Profiel") {
		
			if ($_SERVER['REQUEST_METHOD'] == "POST") {
				if (isset($_POST['pw_nieuw']) and strlen($_POST['pw_nieuw']) > 0) {
					if (strlen($_POST['pw_nieuw']) < 7) {
						$mess = "Het nieuwe wachtwoord is te kort, het moet minimaal 7 karakters lang zijn.";
					} elseif (strlen($_POST['pw_nieuw']) > 12) {
						$mess = "Het nieuwe wachtwoord is te lang, het mag maximaal 12 karakters lang zijn.";
					} elseif ($_POST['pw_nieuw'] != cleanlogin($_POST['pw_nieuw'])) {
						$mess = "Het nieuwe wachtwoord bevat ongeldige tekens.";
					} elseif(!preg_match("#[0-9]+#", $_POST['pw_nieuw']) ) {
						$mess = "Het nieuwe wachtwoord moet minimaal één getal bevatten.";
					} elseif(!preg_match("#[a-z]+#", $_POST['pw_nieuw']) ) {
						$mess = "Het nieuwe wachtwoord moet minimaal één letter bevatten.";
					} elseif(!preg_match("#[A-Z]+#", $_POST['pw_nieuw']) ) {
						$mess = "Het nieuwe wachtwoord moet minimaal één hoofdletter bevatten.";
					} elseif($_SESSION['username'] == $_POST['pw_nieuw']) {
						$mess = "Het nieuwe wachtwoord mag niet gelijk aan de login zijn.";
					} elseif($_SESSION['emailingelogde'] == $_POST['pw_nieuw']) {
						$mess = "Het nieuwe wachtwoord mag niet gelijk aan de e-mail zijn.";
					} elseif($_SESSION['roepnaamingelogde'] == $_POST['pw_nieuw']) {
						$mess = "Het nieuwe wachtwoord mag niet gelijk aan de roepnaam zijn.";
					} elseif ($_POST['pw_nieuw'] !== $_POST['pw_herhaal']) {
						$mess = "Nieuwe wachtwoorden zijn niet gelijk.";
					} else {
						$mess = db_change_password($_POST['pw_nieuw'], $_POST['pw_oud'], $lidid);
					}
					printf("<p class='mededeling'>%s</p>", htmlentities($mess));
				}
				echo("<p><a href='/'>Klik hier om verder te gaan.</a></p>\n");
			} else {
				echo("<div id='profielwijzigen'>\n");
				printf("<form name='ProfielWijzigen' action='%s' method='post'>\n", $actionurl);
				printf("<fieldset>
				<h3>Profiel wijzigen</h3>
				<label for='ingelogdlogin'>Login:</label><input type='text' name='ingelogdlogin' id='ingelogdlogin' value='%s' readonly='readonly'>
				<label for='pw_oud'>Oude wachtwoord:</label><input type='password' name='pw_oud' id='pw_oud' size=10 maxlength=12>
				<label for='pw_nieuw'>Nieuw wachtwoord:</label><input type='password' name='pw_nieuw' id='pw_nieuw' size=10 maxlength=12>
				<label for='pw_herhaal'>Herhaal wachtwoord:</label><input type='password' name='pw_herhaal' id='pw_herhaal' size=10 maxlength=12>
				<input type='submit' class='knop' value='Wijzigen'>\n", $_SESSION['username']);
				echo("</fieldset>
				</form>
				</div>  <!-- Einde profielwijzigen -->");
			}
			
		} else {
			echo("<p class='mededeling'>Deze optie bestaat niet.</p>\n");
		}
	} else {
		echo("<p class='mededeling'>Er is geen lid geselecteerd.</p>\n4");
	}
} # fnWijzigen

function fnWieiswie($actie, $metfoto=1) {
	global $ldl;
	
	echo("<div id='wieiswie'>\n");
	$vo = "";
	if ($actie == "Onderscheidingen") {
		$lijst = db_lidond("actueleleden", 0, "", "O.TYPE='O'");
	} elseif ($actie == "Overige commissies") {
		$lijst = db_Lidond("actueleleden", 0, "", "(O.TYPE='C' AND O.Kader=False)");
	} else {
		$lijst = db_lidond("actueleleden", 0, "", "O.Kader=True");
	}

	if ($metfoto == 1) {
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
			} else {
				$func = $row->Opmerk;
			}
			$fn = fotolid($row->Nummer, 1);
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
					printf("<tr>\n<td>&nbsp;</td><td colspan=2><strong>Centraal e-mailadres</strong></td>\n<td>%s</td>\n</tr>\n", fnDispEmail($row->CentraalEmail, $row->OndNaam, 0, 1));
					if (strlen($vo) == 0) {
						echo("<tr><td colspan=4>&nbsp;</td>\n<td><strong>Vanaf</strong></td>\n</tr>\n");
					} else {
						echo("<tr><td>&nbsp;</td>\n\n\n</tr>\n");
					}
				}
				$vo = $row->OndNaam;
			}
			if (strlen($ldl) > 1) {
				$ln = sprintf($ldl, $row->Nummer, htmlentities($row->LidNaam));
			} else {
				$ln = htmlentities($row->LidNaam);
			}
			if (isset($row->EmailFunctie) and isValidMailAddress($row->EmailFunctie, 0)) {
				$email = fnDispEmail($row->EmailFunctie, $row->LidNaam, 0);
			} elseif (isValidMailAddress($row->EmailVereniging, 0)) {
				$email = fnDispEmail($row->EmailVereniging, $row->LidNaam, 0);
			} else {
				$email = "";
			}
			if (strlen($row->OmsFunctie) > 1) {
				$func = $row->OmsFunctie;
			} else {
				$func = $row->Opmerk;
			}
			printf("<tr>\n<td>&nbsp;</td>\n<td>%s</td>\n<td>%s</td>\n<td>%s</td>\n<td>%s</td>\n</tr>\n", $ln, $func, $email, strftime("%B %Y", strtotime($row->Vanaf)));
		}
		echo("</table>\n");
	}
	echo("</div>  <!-- Einde kaderoverzicht -->\n");
	
}

function fnLedenlijst() {
	global $arrTL, $table_prefix, $wherelidond, $ldl;

	if (toegang("Wijzigen lid", 0)) {
		$llw = "<a href='index.php?tp=Wijzigen+lid/Algemene+gegevens&amp;lidid=%1\$d'><img src='./images/edit.png' alt='Wijzigen' title='Wijzigen lidgegevens'></a>";
		if (toegang("Wijzigen lid/Pasfoto", 0)) {
			$llw .= "</td><td class='details'><a href='index.php?tp=Wijzigen+lid/Pasfoto&amp;lidid=%1\$d'><img src='./images/profile.png' alt='Pasfoto' title='Wijzigen pasfoto'></a>";
		}
	} else {
		$llw = "";
	}

	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$_SESSION['val_naamfilter'] = $_POST['tbNaamFilter'];
		$_SESSION['val_TL'] = $_POST['rbTL'];
		$_SESSION['val_groep'] = $_POST['lbGroepFilter'];
	} elseif (!isset($_SESSION['val_naamfilter'])) {
		$_SESSION['val_naamfilter'] = "";
		$_SESSION['val_TL'] = $arrTL[1];
		$_SESSION['val_groep'] = 0;
	}
	
	echo("<div id='filter'>\n");
	printf("<form name='Filter' action='%s?%s' method='post'>", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
	echo("<table>\n");
	echo("<tr>\n");
	printf("<td class='label'>Naam bevat</td><td><input type='text' name='tbNaamFilter' size=20 value='%s' placeholder='Achter- of roepnaam' onblur='form.submit();'></td>\n", $_SESSION['val_naamfilter']);
	echo("<td>");
	foreach($arrTL as $tl) {
		if ($tl == $_SESSION['val_TL']) {$c=" checked"; } else { $c=""; }
		if ($tl != "Klosleden" or $_SESSION['val_groep'] == 0) {
			printf("<input type='radio'%2\$s name='rbTL' value='%1\$s' onclick='this.form.submit();'>%1\$s\n", $tl, $c);
		}
	}
	echo("</td>\n");
	printf("<td class='label'>Groep</td><td><select name='lbGroepFilter' onchange='this.form.submit();'>%s</select></td>\n", fnSelectListGroepen($_SESSION['val_groep']));
	echo("</tr>\n");
	echo("</table>\n");
	echo("</form>");
	echo("</div>  <!-- Einde filter -->\n");
	
	if (strlen($_SESSION['val_naamfilter']) > 1) {
		$filter = sprintf("(L.Achternaam LIKE '%%%1\$s%%' OR L.Roepnaam LIKE '%%%1\$s%%' OR L.MEISJESNM LIKE '%%%1\$s%%')", strtoupper($_SESSION['val_naamfilter']));
	} else {
		$filter = "";
	}
	
	if ($_SESSION['val_groep'] == 0) {
		if ($_SESSION['val_TL'] == "Leden") {
			if (strlen($filter) > 0) {$filter .= " AND "; }
			$filter .= "LM.Lidnr > 0 AND ((LM.Opgezegd IS NULL) OR LM.Opgezegd > CURDATE())";
		} elseif ($_SESSION['val_TL'] == "Klosleden") {
			if (strlen($filter) > 0) {$filter .= " AND "; }
			$filter .= "(LM.Lidnr IS NULL)";
		} elseif ($_SESSION['val_TL'] == "Voormalig leden") {
			if (strlen($filter) > 0) {$filter .= " AND "; }
			$filter .= "(NOT LM.Opgezegd IS NULL) AND LM.Opgezegd < CURDATE()";
		}
		$rows = db_lid("ledenlijst", $filter);
	} else {
		if (strlen($filter) > 0) {$filter .= " AND ";}
		$filter .= sprintf("LO.OnderdeelID=%d", $_SESSION['val_groep']);
		if ($_SESSION['val_TL'] == "Leden") {
			$filter .= " AND " . $wherelidond;
		} elseif ($_SESSION['val_TL'] == "Voormalig leden") {
			$filter .= " AND (LO.Vanaf <= CURDATE() AND ((NOT LO.Opgezegd IS NULL) OR LO.Opgezegd < CURDATE()))";
		}
		$rows = db_lidond("onderdeellijst", 0, "", $filter);
	}

	if (count($rows) > 0) {
		echo(fnDisplayTable($rows, $ldl, "", 3, $llw, "", "ledenlijst"));
		foreach ($rows as $row) {
			$sel_leden[] = $row->lnkNummer;
		}
		$_SESSION['sel_leden'] = $sel_leden;
	}
}

function fnOverviewLid($lidid=0, $actie="") {
	global $currenttab;
	
	if ($currenttab == "Eigen gegevens") {
		$xtra_param = "";
	} else {
		$xtra_param = sprintf("lidid=%d", $lidid);
	}
	
	if (isset($_GET['op'])) {
		$op = $_GET['op'];
	} else {
		$op = "";
	}
	
	$toneninschrijvingenbewakingen = db_param("toneninschrijvingenbewakingen");
	
	if ($lidid <= 0) {
		printf("<p class='mededeling'>Er is geen lid geselecteerd. Selecteer een lid via de <a href='%s?tp=Ledenlijst'>ledenlijst</a>.</p>\n", $_SERVER['PHP_SELF']);
	} elseif (toegang($_GET['tp'], 0) == false) {
		$mess = sprintf("Je hebt tot tabblad '%s' geen toegang.", $actie);
		db_logboek("add", $mess, 5, 0, 1);
	} else {
	
		fnDispMenu(2, $xtra_param);
		$naamlid = htmlentities(db_naamlid($lidid));
		
		if ($actie == "Afdelingen") {
			$rows = db_gegevenslid($lidid, $actie);
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", $actie . " " . $naamlid, 0, "", "", "onderdelenperlid"));
			} else {
				printf("<p class='mededeling'>%s heeft geen %s.</p>\n", $naamlid, $actie);
			}
		} elseif ($actie == "Kader") {
			$rows = db_gegevenslid($lidid, $actie);
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", $actie . " " . $naamlid, 0, "", "", "onderdelenperlid"));
			} else {
				printf("<p class='mededeling'>%s is niet ingedeeld (geweest) bij het kader.</p>\n", $naamlid);
			}
		} elseif ($actie == "Rollen") {
			$rows = db_gegevenslid($lidid, $actie);
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", $actie . " " . $naamlid));
			} else {
				printf("<p class='mededeling'>%s heeft geen %s.</p>\n", $naamlid, $actie);
			}
		} elseif ($actie == "Groepen") {
			$rows = db_gegevenslid($lidid, $actie);
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", $actie . " " . $naamlid));
			} else {
				printf("<p class='mededeling'>%s is bij geen enkele groep ingedeeld.</p>\n", $naamlid);
			}
		} elseif ($actie == "Bewaking") {
			$rows = db_gegevenslid($lidid, "Bew");
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", "Bewaking " . $naamlid));
			} else {
				printf("<p class='mededeling'>%s heeft geen bewakingshistorie.</p>", $naamlid);
			}
			$rows = db_gegevenslid($lidid, "Lidbew");
			if (count($rows) > 0) {
				echo(fnDisplayForm($rows[0]));
			}
			$rows = db_insbew("overzichtlid", $lidid);
			if (isset($toneninschrijvingenbewakingen) and $toneninschrijvingenbewakingen == 1 and count($rows) > 0) {
				echo(fnDisplayTable($rows, "", "Ingeschreven voor bewakingen"));
			}
		} elseif ($actie == "Diplomas") {
			$rows = db_liddipl("lidgegevens", $lidid);
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", "Diploma's " . $naamlid));
			} else {
				printf("<p class='mededeling'>Bij %s zijn geen diploma's bekend.</p>", $naamlid);
			}
		} elseif ($actie == "Financieel") {
			if ($op == "details" and isset($_GET['rid']) and $_GET['rid'] > 0) {
				echo(RekeningDetail($_GET['rid']));
			} else {
				if ($_SESSION['aantalrekeningen'] > 0) {
					$rows = db_gegevenslid($lidid, "Rekening");
					if (count($rows) > 0) {
						$lnk = sprintf("<a href='%s?tp=%s&amp;op=details&amp;lidid=%d&amp;rid=%%d' title='Details rekening'><img src='images/zoom.png' alt='Zoom'></a>", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
						echo(fnDisplayTable($rows, $lnk, "Rekeningen " . $naamlid));
					} else {
						printf("<p class='mededeling'>%s heeft geen rekeningen ontvangen.</p>", $naamlid);
					}
				}
				$rows = db_gegevenslid($lidid, "Financieel");
				if (count($rows) > 0) {
					echo(fnDisplayForm($rows[0]));
				}
			}
		} elseif ($actie == "Mailing") {
			if (isset($_GET['MailingID']) and $_GET['MailingID'] > 0) {
				$xtra = "<p class='mededeling'><input type='button' value='Terug' onclick='history.go(-1);'></p>\n";
				$mailing = new mailing;
				$mailing->toonverstuurdemail($_GET['MailingID']);
				$mailing = null;
			} else {
				$ld = sprintf("<a href='index.php?tp=%s&amp;lidid=%d&amp;MailingID=%%d'>%%s</a>", urlencode($_GET['tp']), $lidid);
				$rows = db_gegevenslid($lidid, "Mailing");
				if (count($rows) > 0) {
					echo(fnDisplayTable($rows, $ld, sprintf("Aan %s verstuurde mailings", $naamlid)));
				} else {
					printf("<p class='mededeling'>Er zijn vanaf deze website geen mailings naar %s verstuurd.</p>\n", $naamlid);
				}
			}
		} elseif ($actie == "Evenementen") {
			$rows = db_gegevenslid($lidid, $actie);
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", "Deelname evenementen " . $naamlid));
			} else {
				printf("<p class='mededeling'>Bij %s zijn geen deelnames aan evenementen bekend.</p>", $naamlid);
			}
		} elseif ($actie == "Logboek") {
			echo(fnDisplayTable(db_logboek("lijst", "", -1, $lidid)));
		} else {
			$fn = fotolid($lidid, 1);
			if (strlen($fn) > 3) {
				$xtra = sprintf("<div id='pasfoto'><img src='%s' alt='Pasfoto %s' title='Laatst gewijzigd op %s'></div>\n", $fn, db_naamlid($lidid, "Roepnaam"), strftime("%e %B %Y", filectime(fotolid($lidid, 0))));
			} else {
				$xtra = "";
			}
			$rows = db_gegevenslid($lidid);
			echo(fnDisplayForm($rows[0], $xtra, "overzichtlid"));
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
			printf("Lid %d van de %d", $current_key+1, count($_SESSION['sel_leden']));
			echo("</div>  <!-- Einde opdrachtknoppen -->\n");
		}
	}
}

function fnSelectListGroepen($cv=0) {

	$ret = "<option value=0>** Iedereen **</option>\n";
	$rows = db_Onderdelen();
	foreach ($rows as $row) {
		if ($cv == $row->RecordID) {$s = " selected";} else {$s = "";}
		$ret .= sprintf("<option%s value=%d>%s</option>\n", $s, $row->RecordID, htmlentities($row->Oms));
	}
	return $ret;
}

function overzichtverjaardagen($metfoto=1) {

	$verj = "";
	$verjfoto = "";
	$aantgetoond = 0;
	for ($i = 0; $i <= db_param("verjaardagenvooruit") and $aantgetoond < db_param("verjaardagenaantal"); $i++) {
		$dteHV = mktime(0, 0, 0, date("m"), date("d")+$i, date("Y"));
		foreach(db_lid("verjaardagen", date("Y-m-d", $dteHV)) as $row) {
			if ($i == 0) {
				$t = sprintf("%s is vandaag %d jaar geworden", htmlentities($row->Naam_lid), $row->Leeftijd);
			} elseif ($i == 1) {
				$t = sprintf("%s wordt morgen %d jaar", htmlentities($row->Naam_lid), $row->Leeftijd);
			} else {
				$t = sprintf("Op %s is %s jarig", strftime("%A %e %B", $dteHV), htmlentities($row->Naam_lid));
			}
			$fn = fotolid($row->Nummer, 1);
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
}

function fotolid($lidid, $metversie=0) {
	global $pasfotoextenties;

	$fn = "";
	foreach($pasfotoextenties as $ext) {
		if (file_exists(sprintf("pasfoto/Pasfoto%d.%s", $lidid, $ext))) {
			$fn = sprintf("pasfoto/Pasfoto%d.%s", $lidid, $ext);
		} elseif (file_exists(sprintf("pasfoto/pasfoto%d.%s", $lidid, $ext))) {
			$fn = sprintf("pasfoto/pasfoto%d.%s", $lidid, $ext);
		} elseif (file_exists(sprintf("pasfoto/Pasfoto%d.%s", $lidid, strtoupper($ext)))) {
			$fn = sprintf("pasfoto/Pasfoto%d.%s", $lidid, strtoupper($ext));
		} elseif (file_exists(sprintf("pasfoto/pasfoto%d.%s", $lidid, strtoupper($ext)))) {
			$fn = sprintf("pasfoto/pasfoto%d.%s", $lidid, strtoupper($ext));
		}
	}
	if (strlen($fn) > 3 and $metversie == 1) {
		$fn .= "?v" . strftime("%Y%m%d%H%M", filectime($fn));
	}
	return $fn;
}

function algemeenlidmuteren($lidid) {
	global $arrGeslacht, $arrLegitimatie, $actionurl, $table_prefix, $fdsql, $fdaccess, $currenttab;
		
	$wijzvelden[] = array('label' => "Roepnaam", 'naam' => "Roepnaam", 'lengte' => 17);
	$wijzvelden[] = array('label' => "Voorletters", 'naam' => "VOORLETTER", 'lengte' => 10);
	$wijzvelden[] = array('label' => "Tussenvoegsels", 'naam' => "Tussenv", 'lengte' => 7);
	$wijzvelden[] = array('label' => "Achternaam", 'naam' => "Achternaam", 'lengte' => 30, 'nietverw' => true);
	$wijzvelden[] = array('label' => "Meisjesnaam", 'naam' => "Meisjesnm", 'lengte' => 25);
	$wijzvelden[] = array('label' => "Geslacht", 'naam' => "Geslacht", 'nietverw' => true);
	$wijzvelden[] = array('label' => "Adres", 'naam' => "Adres", 'lengte' => 30);
	$wijzvelden[] = array('label' => "Postcode", 'naam' => "Postcode", 'lengte' => 7);
	$wijzvelden[] = array('label' => "Woonplaats", 'naam' => "Woonplaats", 'lengte' => 22);
	$wijzvelden[] = array('label' => "Telefoon", 'naam' => "Telefoon", 'lengte' => 16);
	$wijzvelden[] = array('label' => "Mobiel", 'naam' => "Mobiel", 'lengte' => 16);
	$wijzvelden[] = array('label' => "E-mail", 'naam' => "Email", 'lengte' => 45);
	if (db_param("zs_incl_emailvereniging") == 1 or $currenttab == "Wijzigen lid") {
		$wijzvelden[] = array('label' => "E-mail vereniging", 'naam' => "EmailVereniging", 'lengte' => 45);
	}
	if (db_param("zs_incl_emailouders") == 1 or $currenttab == "Wijzigen lid") {
		$wijzvelden[] = array('label' => "E-mail ouders", 'naam' => "EmailOuders", 'lengte' => 45);
	}
	$wijzvelden[] = array('label' => "Geboortedatum", 'naam' => "GEBDATUM", 'lengte' => 18);
	$wijzvelden[] = array('label' => "Geboorteplaats", 'naam' => "GEBPLAATS", 'lengte' => 22);
	$wijzvelden[] = array('label' => "Bankrekening", 'naam' => "BANKGIRO", 'lengte' => 10);
	if (db_param("zs_incl_iban") == 1) {
		$wijzvelden[] = array('label' => "IBAN (International Bank Account Number)", 'naam' => "Bankrekening", 'lengte' => 18);
	}
	if (db_param("zs_incl_bezwmachtiging") == 1 or $currenttab == "Wijzigen lid") {
		$wijzvelden[] = array('label' => "Bezwaar machtiging incasso", 'naam' => "BezwaarMachtiging", 'lengte' => 1);
	}
	if (db_param("zs_incl_bsn") == 1) {
		$wijzvelden[] = array('label' => "Burgerservicenummer", 'naam' => "Burgerservicenummer", 'lengte' => 9);
	}
	if (db_param("zs_incl_slid") == 1 or $currenttab == "Wijzigen lid") {
		$wijzvelden[] = array('label' => "Sportlink ID", 'naam' => "RelnrRedNed", 'lengte' => 7, 'nietverw' => true);
	}
	if (db_param("zs_incl_legitimatie") == 1 or $currenttab == "Wijzigen lid") {
		$wijzvelden[] = array('label' => "Legitimatietype", 'naam' => "Legitimatietype", 'nietverw' => true);
		$wijzvelden[] = array('label' => "Legitimatienummer", 'naam' => "Legitimatienummer", 'lengte' => 15);
	}
	if (db_param("zs_incl_beroep") == 1 or $currenttab == "Wijzigen lid") {
		$wijzvelden[] = array('label' => "Beroep", 'naam' => "Beroep", 'lengte' => 40);
	}

	$naamlid = db_naamlid($lidid);
		
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$mess = "";
		$toonmess = 0;
		$old = db_lid("wijzigingen", "", $lidid);
		for ($i=0; $i < count($wijzvelden); $i++) {
			$fn = $wijzvelden[$i]['naam'];
			$v = "__skip__";
			if ($fn == "BezwaarMachtiging") {
				if (isset ($_POST[$fn]) and $_POST[$fn] == "1" and $old->$fn < "2000-01-01") {
					$v = date("Y-m-d");
				} elseif (!isset ($_POST[$fn]) and $old->$fn >= "2000-01-01") {
					$v = "NULL";
				}
				
			} elseif ($fn == "Bankrekening" and isset($_POST[$fn]) and strlen($_POST[$fn]) > 0 and IsIBANgoed($_POST[$fn], 1) == false) {
				$mess .= "IBAN is niet correct, deze wijziging wordt niet verwerkt. ";
				$toonmess = 1;
				$v = "";
				
			} elseif ($fn == "Burgerservicenummer" and isset($_POST[$fn]) and strlen($_POST[$fn]) > 0 and (!is_numeric($_POST[$fn]) or $_POST[$fn] < 100000000 or $_POST[$fn] > 999999999)) {
				$mess .= "Burgerservicenummer is niet correct, deze wijziging wordt niet verwerkt. ";
				$toonmess = 1;
				$v = "";
				
			} elseif (isset($_POST["chkLeeg_" . $fn]) and $_POST["chkLeeg_" . $fn] == "1") {
				if (stripos($wijzvelden[$i]['label'], "datum") === FALSE) {
					$v = "";
				} else {
					$v = "NULL";
				}
			} elseif (stripos($wijzvelden[$i]['label'], "datum") !== FALSE) {
				$_POST[$fn] = change_month_to_uk($_POST[$fn]);
				if (isset($_POST[$fn]) and strlen($_POST[$fn]) > 0) {
					if (strtotime($_POST[$fn]) === FALSE) {
						$mess .= "Geboortedatum is niet correct, deze wijziging wordt niet verwerkt. ";
						$toonmess = 1;
					} else {
						$v = strftime($fdsql, strtotime($_POST[$fn]));
					}
				}
			} elseif (stripos($wijzvelden[$i]['label'], "e-mail") !== FALSE) {
				if (isset($_POST[$fn]) and strlen($_POST[$fn]) > 0) {
					if (isValidMailAddress($_POST[$fn], 0)) {
						$v = $_POST[$fn];
					} else {
						$mess .= "E-mailadres is niet correct, deze wijziging wordt niet verwerkt. ";
						$toonmess = 1;
					}
				}
			} elseif ($fn == "RelnrRedNed" and strlen($_POST[$fn]) != 7 and strlen($_POST[$fn]) > 0) {
				$mess .= "Het Sportlink ID moet 7 karakters lang zijn, deze wijziging wordt niet verwerkt. ";
				$toonmess = 1;
				$v = "";
				
			} elseif (isset($_POST[$fn]) and strlen($_POST[$fn]) > 0) {
				$v = $_POST[$fn];
			}
			if ($v != "__skip__" and $v != $old->$fn) {
				if ($v == "NULL") {
					$query = sprintf("UPDATE %sLid SET %s=NULL, Gewijzigd=SYSDATE() WHERE Nummer=%d;", $table_prefix, $fn, $lidid);
				} else {
					$query = sprintf("UPDATE %sLid SET %s='%s', Gewijzigd=SYSDATE() WHERE Nummer=%d;", $table_prefix, $fn, $v, $lidid);
				}
				$result = fnQuery($query);
				if ($result > 0) {
					if (($fn == "GEBDATUM" or $fn == "BezwaarMachtiging") and $v != "NULL") {
						$query = sprintf("UPDATE Lid SET %s=#%s#, Gewijzigd=SYSDATE() WHERE Nummer=%d;", $fn, strftime($fdaccess, strtotime($v)), $lidid);
					}
					db_interface("add", $query, $lidid);
					$mess .= sprintf("%s van %s is in '%s' gewijzigd. ", $wijzvelden[$i]['label'], $naamlid, $v);
				} else {
					$mess .= sprintf("De wijziging in %s bij %s is niet verwerkt. ", strtolower($wijzvelden[$i]['label']), $naamlid);
					$toonmess = 1;
				}
			}
		}
		db_logboek("add", $mess, 6, 0, $toonmess);
	} # if post

	$row = db_lid("wijzigingen", "", $lidid);
	echo("<div id='wijzigengegevens'>\n");
	printf("<form method='post' action='%s' name='frm_wijzigingen'>\n", $actionurl);
	echo("<table>\n");
	echo("<tr><th>Veld</th><th>Huidige waarde</th><th>Nieuwe waarde</th><th>Leeg</th></tr>\n");
	for ($i=0; $i < count($wijzvelden); $i++) {
		$dv = $row->$wijzvelden[$i]['naam'];
		$ph = $wijzvelden[$i]['label'];
		if ($wijzvelden[$i]['label'] == "Geslacht" or ($wijzvelden[$i]['label'] == "Legitimatietype")) {
			if ($wijzvelden[$i]['label'] == "Geslacht") {
				$an = "arrGeslacht";
			} else {
				$an = "arrLegitimatie";
			}
			$opt = "\n";
			foreach ($$an as $key => $val) {
				$sel = "";
				if ($key == $row->$wijzvelden[$i]['naam']) {
					$sel = " selected";
					$dv = $val;
				}
				$opt .= sprintf("<option value='%s'%s>%s</option>\n", $key, $sel, $val);
			}
			$inp = sprintf("<select name='%s'>%s</select></td><td>", $wijzvelden[$i]['naam'], $opt);
			
		} elseif ($wijzvelden[$i]['naam'] == "BezwaarMachtiging") {
			if ($row->$wijzvelden[$i]['naam'] > "2000-01-01") {
				$dv = strftime("%e %B %Y", strtotime($dv));
				$c = "checked";
			} else {
				$dv = "Nee";
				$c = "";
			}
			$inp = sprintf("<input type='checkbox' name='%s' value=1 %s>", $wijzvelden[$i]['naam'], $c);
			$t = "text";
		
		} else {
			if (strpos($wijzvelden[$i]['label'], "datum") !== FALSE) {
				$t = "text";
				if ($dv > "1900-01-01") {
					$dv = strftime("%e %B %Y", strtotime($dv));
				}
			} elseif (stripos($wijzvelden[$i]['label'], "e-mail") !== FALSE) {
				$t = "email";
			} else {
				$t = "text";
			}
			$inp = sprintf("<input type='%s' name='%s' placeholder='%s' size=40 maxlength=%d></td><td class='chk'>", 
					$t, $wijzvelden[$i]['naam'], $ph, $wijzvelden[$i]['lengte']);
			if (strlen($dv) > 0 and (!isset($wijzvelden[$i]['nietverw']) or $wijzvelden[$i]['nietverw'] == false)) {
				$inp .= sprintf("<input type='checkbox' name='chkLeeg_%s' value=1>", $wijzvelden[$i]['naam']);
			}
		}
		printf('<tr><td class="label">%1$s</td><td>%2$s</td><td>%3$s</td></tr>', $wijzvelden[$i]['label'], $dv, $inp);
		echo("\n");
	}
	echo("<tr><th colspan=4><input type='submit' value='Bewaren' name='adreswijziging'></th></tr>\n");
	echo("</table>\n");

	echo("</form>\n");
	echo("</div>  <!-- Einde wijzigengegevens -->\n");	
}

function nieuwepasfoto($lidid, $filterlid="") {
	global $pasfotoextenties, $actionurl;
	
	$emailledenadministratie = db_param("emailledenadministratie");
	$emailnieuwepasfoto = db_param("zs_emailnieuwepasfoto");

	$max_size_attachm = 2 * 1024 * 1024;  // 2MB
	$min_size_attachm = 4 * 1024;  // 4KB
	
	if (isset($_POST['lidid']) and $_POST['lidid'] > 0) {
		$lidid = $_POST['lidid'];
	}
	$naamlid = db_naamlid($lidid);
	
	if (isset($_FILES['UploadFoto']['name']) and strlen($_FILES['UploadFoto']['name']) > 3 and $lidid > 0) {
	
		$ad = $_SERVER["SCRIPT_FILENAME"];
		$ad = substr($ad, 0, strrpos($ad, "/")) . "/pasfoto/";
		if (file_exists($ad)) {
			chmod($ad, 0755);
		} else {
			if (mkdir($ad, 0755, true) == true) {
				$ret = sprintf("Directory '%s' is aangemaakt.", $ad);
				db_logboek("add", $ret, 6);
			}
		}
		if (file_exists($ad . "vervangen/")) {
			chmod($ad . "vervangen/", 0755);
		} else {
			if (mkdir($ad . "vervangen/", 0755, true) == true) {
				$ret = sprintf("Directory '%s' is aangemaakt.", $ad . "vervangen/");
				db_logboek("add", $ret, 6);
			}
		}

		$ext = explode(".", $_FILES['UploadFoto']['name']);
		$ext = strtolower($ext[count($ext) - 1]);
		if ($ext == "jpeg") {
			$ext = "jpg";
		}
		$target = $ad . sprintf("Pasfoto%d.%s", $lidid, $ext);
		$mess = "";
		if (in_array($ext, $pasfotoextenties) === false) {
			$mess = sprintf("Het bestand met extensie %s is niet toegestaan. De volgende extensies zijn toegestaan: %s.", $ext, implode(", ", $pasfotoextenties));
		} elseif ($_FILES['UploadFoto']['size'] > $max_size_attachm) {
			$mess = sprintf("De foto kan niet ge-upload worden, omdat het bestand groter dan %dKB is.", $max_size_attachm / 1024);
		} elseif ($_FILES['UploadFoto']['size'] < $min_size_attachm) {
			$mess = sprintf("De foto kan niet worden ge-upload, omdat het bestand kleiner dan %d bytes is.", $min_size_attachm);
		} else {
			$of = fotolid($lidid, 0);
			while (strlen($of) > 3 and file_exists($of)) {
				$of_ext = substr(strrchr($of, "."), 1);
				$bu_name = "vervangen/" . sprintf("Pasfoto%d_%d.%s", $lidid, strftime("%Y%m%d", filectime($of)), $of_ext);
				if (rename($of, $ad . $bu_name)) {
					$mess = sprintf("Pasfoto './%s' is verplaatst en hernoemd naar './%s'.", $of, $bu_name);
					db_logboek("add", $mess, 6);
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
			
			if ($lidid == $_SESSION['lidid'] and isValidMailAddress($emailnieuwepasfoto, 0) and (file_exists($target))) {
				$mail = new RBMmailer();
				$mail->From = $_SESSION['emailingelogde'];
				$mail->FromName = $naamlid;
				$mail->Subject = "Nieuwe pasfoto " . $naamlid;
				if (isValidMailAddress($emailnieuwepasfoto, 0) and $emailnieuwepasfoto != $emailledenadministratie) {
					$mail->AddAddress($emailnieuwepasfoto);
				}
				if (isValidMailAddress($emailledenadministratie, 0)) {
					$mail->AddAddress($emailledenadministratie);
				}
				$mail->AddCC($_SESSION['emailingelogde']);
				$body = sprintf("<p>Beste ledenadministratie,</p>
										<p>Bijgevoegd is mijn nieuwe pasfoto.</p>
										<p>Met vriendelijke groeten,<br>
										<strong>%s (LidID: %d)<strong></p>\n", $naamlid, $lidid);
				$mail->MsgHTML($body);
				$mail->AddAttachment($target);
				if ($mail->Send()) {
					$mess = sprintf("%s heeft een nieuwe pasfoto ingestuurd.", $naamlid);
				} else {
					$mess = sprintf("Fout bij het e-mailen van de pasfoto: %s.", $mail->ErrorInfo);
				}
				$mail = null;
			} elseif (file_exists($target)) {
				$mess = sprintf("Er is van %s een nieuwe pasfoto ge-upload.", $naamlid);
			} else {
				$mess = "Het is fout gegaan bij het uploaden van het bestand. Probeer het nog een keer of neem contact op met de webmaster.";
			}
		}
		db_logboek("add", $mess, 6, 0, 1);
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
	if ($lidid > 0) {
		printf("<p><label for='UploadFoto'>Nieuwe pasfoto %s:&nbsp;</label>\n", $naamlid);
		printf("<input type='hidden' name='lidid' value=%d>\n", $lidid);
	} else {
		$optionslid = "<option value=-1>Selecteer lid ...</option>\n";
		foreach (db_lid("lijst", $filterlid) as $row) {
			$optionslid .= sprintf("<option value=%1\$d>%2\$s (%1\$d)</option>\n", $row->Nummer, htmlentities($row->Zoeknaam));
		}
		printf("<p><select name='lidid'>%s</select></p>\n", $optionslid);
	}
	echo("<input type='file' name='UploadFoto' id='UploadFoto'>&nbsp;");
	echo("<input type='submit' name='Upload' value='Insturen'>\n");
	printf("<p>Het ideale formaat van de pasfoto is 390 pixels breed bij 500 pixels hoog. Het bestand moet minimaal %d bytes groot zijn en mag niet groter dan %d KB zijn.</p>\n", $min_size_attachm, $max_size_attachm / 1024);
	echo("</form>\n");

	echo("</div>  <!-- Einde nieuwepasfoto -->\n");	
}

function opzegginglidmaatschap($lidid) {
	global $actionurl;
	
	$opzegtermijn = db_param("zs_opzegtermijn");
	$emailledenadministratie = db_param("emailledenadministratie");
	$fdlang = "%e %B %Y";
	
	$lid = db_lid("record", "", $lidid);
	$naamlid = $lid->NaamLid;
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST["OpzeggingPer"]) and strlen($_POST["OpzeggingPer"]) > 3) {
			$_POST["OpzeggingPer"] = change_month_to_uk($_POST["OpzeggingPer"]);
		} else {
			$_POST["OpzeggingPer"] = "";
		}
		if (strlen($_POST["OpzeggingPer"]) > 3 and strtotime($_POST["OpzeggingPer"]) !== FALSE) {
			$opgezegdper = change_month_to_uk($_POST["OpzeggingPer"]);
			if (strtotime($opgezegdper) < mktime(0, 0, 0, date("m")+$opzegtermijn, date("d"), date("Y"))) {
				$opgezegdper = strftime($fdlang, mktime(0, 0, 0, date("m")+$opzegtermijn, date("d"), date("Y")));
				printf("<p class='mededeling'>Er geldt een opzegtermijn van %d maand(en), hierdoor wordt de datum van opzegging %s.</p>", $opzegtermijn, $opgezegdper);
			} else {
				$opgezegdper = strftime($fdlang, strtotime($opgezegdper));
			}
		} else {
			$opgezegdper = strftime($fdlang, mktime(0, 0, 0, date("m")+$opzegtermijn, date("d"), date("Y")));
			printf("<p class='mededeling'>Er geldt een opzegtermijn van %d maand(en), hierdoor wordt de datum van opzeggen %s.</p>", $opzegtermijn, $opgezegdper);
		}
		if (isset($_POST['RedenOpmerking']) and strlen($_POST['RedenOpmerking']) > 1) {
			$opm = "\n<p>" . $_POST['RedenOpmerking'] . "</p>\n";
		} else {
			$opm = "";
		}
			
		$mess = sprintf("%s heeft haar/zijn lidmaatschap per %s opgezegd.", $naamlid, $opgezegdper);
		db_logboek("add", $mess, 6);
			
		$body = sprintf("<p>Beste ledenadministratie,</p>\n
		<p>Hierbij zeg ik mijn lidmaatschap van de %s per %s op. Mijn lidnummer is %d.</p>\n
		%s
		<p>Met vriendelijke groeten,<br>
		<strong>%s</strong></p>\n", $_SESSION['naamvereniging'], $opgezegdper, $_SESSION['lidnr'], $opm, $naamlid);
				
		$body .= sprintf("\n<p>Dit formulier is verzonden vanaf IP-adres %s.</p>\n", $_SERVER['REMOTE_ADDR']);
	
		$mail = new RBMmailer();
		$mail->From = $_SESSION['emailingelogde'];
		$mail->FromName = $naamlid;
		$mail->AddAddress($emailledenadministratie);
		$mail->InformerenOuders($lid->GEBDATUM, $lid->EmailOuders);
		$mail->AddCC($_SESSION['emailingelogde']);
		if (isValidMailAddress($_SESSION['emailsecretariaat'], 0)) {
			$mail->AddCC($_SESSION['emailsecretariaat']);
		}
		$mail->Subject = sprintf("Opzegging lidmaatschap %s per %s", $naamlid, $opgezegdper);
		$mail->Body = $body;
		if ($mail->Send()) {
			$mess = sprintf("De opzegging per '%s' is verzonden.", $opgezegdper);
			$success = 1;
		} else {
			$mess = "Het versturen van de opzegging is mislukt. Probeer het later nogmaals.";
			$success = 0;
		}
		db_add_mailing_hist(0, 0, "Ledenadministratie", $emailledenadministratie, $body, $mail, $success);
		db_logboek("add", $mess, 6, 0, 1);
		$mail = null;
		if (db_param("zs_opzeggingautomatischverwerken") == 1) {
			$mess = db_lidmaatschap("opzegging", $lidid, $opgezegdper);
			db_logboek("add", $mess, 6);
		}
		
	} else {
		$form_opzegging = sprintf("<div id='opzegformulier'>\n<form name='Opzegging' action='%s' method='post'>\n", $actionurl);
		$form_opzegging .= "<fieldset>
<h3>Opzegging lidmaatschap</h3>
<p><label for='NaamLid'>Naam lid:</label><input type='text' name='NaamLid' value='[%NAAMLID%]' readonly='readonly'>
<label for='Lidnr'>Lidnummer:</label><input type='text' class='inputnumber' name='Lidnr' value='[%LIDNR%]' readonly='readonly'>
<label for='OpzeggingPer'>Opzegging lidmaatschap per:</label><input type='text' name='OpzeggingPer' id='OpzeggingPer' value='[%OPZEGGENVANAF%]' maxlength=18 size=20></p>
<textarea name='RedenOpmerking' rows=8 cols=48 title='Reden en/of opmerkingen'></textarea>
<input type='submit' class='knop' name='VerstuurOpzegging' value='Verstuur opzegging'>
</fieldset>
</form>
</div>  <!-- Einde opzegformulier -->";
		$myFile = 'templates/opzegging.html';
		if (file_exists($myFile)) {
			$content = file_get_contents($myFile);
			$content = str_replace("[%FORMOPZEGGING%]", $form_opzegging, $content);
		} else {
			$content = $form_opzegging;
		}
	
		$content = str_replace("[%OPZEGGENVANAF%]", strftime($fdlang, mktime(0, 0, 0, date("m")+$opzegtermijn, date("d")+1, date("Y"))), $content);
		$content = str_replace("[%NAAMLID%]", $_SESSION['naamingelogde'], $content);
		$content = str_replace("[%LIDNR%]", $_SESSION['lidnr'], $content);
		$content = str_replace("[%NAAMVERENIGING%]", $_SESSION['naamvereniging'], $content);
		$content = str_replace("[%NAAMWEBSITE%]", $_SESSION['naamwebsite'], $content);
		$content = str_replace("[%URLWEBSITE%]", $_SESSION['urlwebsite'], $content);
		
		echo($content);
	}
} # opzegginglidmaatschap

function diplomaslidmuteren($lidid, $td, $eenv=1) {
	global $actionurl;

	if ($td == "ZS") {
		$actie = "zelfservice_lijst";
	} else {
		$actie = "muteerlijst";
	}
	
	if ($_SERVER['REQUEST_METHOD'] == "POST" and isset($_POST['diplwijz'])) {
		foreach (db_diploma($actie, 0, $td) as $dp) {
			foreach (db_liddipl("diplomalid", $lidid, 0, $dp->RecordID) as $ld) {
				$fnBehaald = sprintf("Behaald_%d_%d", $dp->RecordID, $ld->RecordID);
				$fnVervaltPer = sprintf("VervaltPer_%d_%d", $dp->RecordID, $ld->RecordID);
				$fnDiplnr = sprintf("Diplnr_%d_%d", $dp->RecordID, $ld->RecordID);
				$fnVerw = sprintf("chkVerw_%d_%d", $dp->RecordID, $ld->RecordID);
				if (strlen($_POST[$fnBehaald]) == 0 and strlen($_POST[$fnDiplnr]) > 0) {
					echo("<p class='mededeling'>Er moet altijd een datum van behalen ingevuld worden. Diplomanummer wordt niet toegevoegd.</p>");
				} elseif (isset($_POST[$fnVerw])) {
					db_liddipl("delete", $ld->Lid, 0, $dp->RecordID, $ld->EXDATUM);
				} elseif ($_POST[$fnVervaltPer] != $ld->LicentieVervallenPer or $_POST[$fnDiplnr] != $ld->Diplomanummer) {
					db_liddipl("update", 0, $ld->RecordID, 0, $ld->EXDATUM, $_POST[$fnVervaltPer], $_POST[$fnDiplnr]);
				}
			}
			$fnBehaald = sprintf("Behaald_%d_%d", $dp->RecordID, 0);
			$fnVervaltPer = sprintf("VervaltPer_%d_%d", $dp->RecordID, 0);
			$fnDiplnr = sprintf("Diplnr_%d_%d", $dp->RecordID, 0);
			if (isset($_POST[$fnBehaald]) and strlen($_POST[$fnBehaald]) > 4 and $lidid > 0) {
				db_liddipl("add", $lidid, 0, $dp->RecordID, $_POST[$fnBehaald], $_POST[$fnVervaltPer], $_POST[$fnDiplnr]);				
			}
		}
	}
			
	$oldvals = "";
	echo("<div id='wijzigendiplomas'>\n");
	
	$actionurl = sprintf("%s?tp=%s&amp;lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	printf("<form method='post' action='%s' name='frm_pasfoto' enctype='multipart/form-data'>\n", $actionurl);
	if ($lidid > 0) {
		printf("<input type='hidden' name='lidid' value=%d>\n", $lidid);
	}
	printf("<input type='hidden' name='dptype' value='%s'>\n", $td);
	
	echo("<table>\n");
	if ($_SESSION['lidid'] != $lidid) {
		printf("<tr><th class='tabelkop' colspan=7>Diploma's %s muteren</th></tr>\n", db_naamlid($lidid));
	}
	printf("<tr><th>Code</th><th>Naam</th><th>Geldigheid</th><th>Behaald op</th><th>Geldig tot</th><th>Diplomanummer</th><th>Verw?</th></tr>\n");
	foreach (db_diploma($actie, 0, $td) as $dp) {
		$aant = 0;
		if ($dp->GELDIGH <= 0) {
			$geldig = "";
		} elseif ($dp->GELDIGH == 1) {
			$geldig = "1 maand";
		} elseif (($dp->GELDIGH/12) == intval($dp->GELDIGH/12)) {
			$geldig = sprintf("%d jaar", $dp->GELDIGH/12);
		} else {
			$geldig = sprintf("%d maanden", $dp->GELDIGH);
		}
		$vast = sprintf("<td>%s</td>\n<td>%s</td>\n<td>%s</td>\n", $dp->Kode, $dp->Naam, $geldig);
		$var = "<td><input type='text' name='VervaltPer_%3\$d_%4\$d' value='%1\$s'></td>\n
				  <td><input type='text' name='Diplnr_%3\$d_%4\$d' value='%2\$s' maxlength=14></td>\n";
		foreach (db_liddipl("diplomalid", $lidid, 0, $dp->RecordID) as $ld) {
			if ($eenv == 0 or $aant == 0) {
				echo("<tr>\n");
				echo($vast);
				printf("<td>%s</td>\n", strftime("%e %B %Y", strtotime($ld->EXDATUM)));
				printf($var, $ld->LicentieVervallenPer, $ld->Diplomanummer, $dp->RecordID, $ld->RecordID);
				printf("<td class='chk'><input type='checkbox' name='chkVerw_%d_%d' value=1></td>\n", $dp->RecordID, $ld->RecordID);
				echo("</tr>\n");
				$oldvals .= sprintf("<input type='hidden' name='Behaald_%d_%d' value='%s'>", $dp->RecordID, $ld->RecordID, $ld->EXDATUM);
				$oldvals .= sprintf("\n<input type='hidden' name='RID_%d' value=%d>\n", $dp->RecordID, $ld->RecordID);
			}
			$aant++;
		}
		if ($aant == 0 or ($eenv == 0 and $dp->GELDIGH > 0)) {
			echo("<tr>\n");
			echo($vast);
			printf("<td><input type='text' name='Behaald_%d_0'></td>\n", $dp->RecordID);
			printf($var, "", "", $dp->RecordID, 0);
			echo("<td></td></tr>\n");
		}
	}
	echo("<tr><th colspan=7><input type='submit' value='Bewaren' name='diplwijz'></th></tr>\n");
	echo("</table>\n");
	echo($oldvals);
	echo("</form>\n");
	echo("</div>  <!-- Einde wijzigendiplomas -->\n");
}

function lidmaatschapmuteren($lidid) {
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
	
		foreach (db_lidmaatschap("overzichtlid", $lidid) as $row) {
			$lidnr = sprintf("Lidnr_%d", $row->RecordID);
			$vanaf = sprintf("Vanaf_%d", $row->RecordID);
			$opgezegd = sprintf("Opgezegd_%d", $row->RecordID);
			db_lidmaatschap("update", $row->Lid, $_POST[$opgezegd], $row->RecordID, $_POST[$vanaf], $_POST[$lidnr]);
		}
			
		if (isset($_POST['NieuwLidmaatschap'])) {
			db_lidmaatschap("add", $lidid);
		}
		
	} # if post
	
	echo("<div id='lidmaatschapmuteren'>\n");
	$actionurl = sprintf("%s?tp=%s&amp;lidid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $lidid);
	printf("<form method='post' action='%s' name='frm_lidmaatschapmuteren'>\n", $actionurl);
	echo("<table>\n");
	echo("<tr><th>RecordID</th><th>Lidnummer</th><th>Lid vanaf</th><th>Opgezegd per</th><tr>\n");
	$toev = true;
	$aant = 0;
	foreach (db_lidmaatschap("overzichtlid", $lidid) as $row) {
		echo("<tr>\n");
		printf("<td>%d</td>\n", $row->RecordID);
		printf("<td><input type='number' class='inputnumber' name='Lidnr_%d' value=%d></td>\n", $row->RecordID, $row->Lidnr);
		printf("<td><input name='Vanaf_%d' value='%s'></td>\n", $row->RecordID, strftime('%e %B %Y', strtotime($row->LIDDATUM)));
		if ($row->Opgezegd >= "1901-01-01") {
			$v = strftime('%e %B %Y', strtotime($row->Opgezegd));
			if ($row->Opgezegd >= date("Y-m-d")) {
				$toev = false;
			} elseif ($row->LIDDATUM >= date("Y-m-d")) {
				$toev = false;
			}
		} else {
			$v = "";
			$toev = false;
		}
		printf("<td><input name='Opgezegd_%d' value='%s'></td>\n", $row->RecordID, $v);
		echo("</tr>\n");
		$aant++;
	} # foreach
	
	echo("<tr>\n<th colspan=4>\n");
	if ($aant > 0) {
		echo("<input type='submit' value='Bijwerken'>\n");
	}
	if ($toev) {
		echo("<button name='NieuwLidmaatschap' value=1>Nieuw lidmaatschap</button>");
	}
	echo("</th></tr>\n");
	
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde lidmaatschapmuteren -->\n");	
}

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

function RekeningDetail($nr) {
	
	$rk = db_rekening("hoofd", $nr);
	
	$rekening_template = 'templates/rekening2.html';
	if (file_exists($rekening_template)) {
		$content = file_get_contents($rekening_template);
	} else {
		$content = "<p>[%NAAMDEBITEUR%]<br>
[%ADRES%]<br>
[%POSTCODE%]  [%WOONPLAATS%]</p>
<br>
<p>Mijdrecht, [%REKENINGDATUM%]</p>
<br>
<p><u>Betreft: [%REKENINGOMSCHRIJVING%] [%REKENINGNUMMER%]</u></p>
<br>
<div id='rekeningregels'>
<table>
<tr><th>Regel</th><th>Omschrijving</th><th>Lid</th><th class='number'>Bedrag in &euro;</th></tr>
[%REKENINGREGELS%]
<tr><th colspan=3>Totaal</th><th class='number'>[%REKENINGBEDRAG%]</th><tr>
</table>
</div> <!-- Einde rekeningregels -->
<p>Deze rekening wordt rond [%UITERSTEBETAALDATUM%] middels automatische incasso van bankrekening [%BANKREKENING%] afgeschreven, tenzij je hiervoor een bezwaar hebt ingediend.</p>

<p>Indien u een andere betalingsregeling wenst of een vraag over deze rekening heeft kunt u contact met de penningmeester opnemen.</p>

<p><strong>Het bestuur van [%NAAMVERENIGING%]</strong></p>\n";
	}
	
	$content = str_ireplace("[%NAAMDEBITEUR%]", $rk->DEBNAAM, $content);
	$content = str_ireplace("[%ADRES%]", $rk->Adres, $content);
	$content = str_ireplace("[%POSTCODE%]", $rk->Postcode, $content);
	$content = str_ireplace("[%WOONPLAATS%]", $rk->Woonplaats, $content);
	$content = str_ireplace("[%LIDNR%]", $rk->Lidnr, $content);
	$content = str_ireplace("[%REKENINGDATUM%]", strftime("%e %B %Y", strtotime($rk->Datum)), $content);
	$content = str_ireplace("[%SEIZOEN%]", $rk->Seizoen, $content);
	$content = str_ireplace("[%REKENINGOMSCHRIJVING%]", $rk->OMSCHRIJV, $content);
	$content = str_ireplace("[%REKENINGNUMMER%]", $rk->Nummer, $content);
	$content = str_ireplace("[%REKENINGBEDRAG%]", $rk->Bedrag, $content);
	$content = str_ireplace("[%BANKREKENING%]", $rk->Bankrekeningnummer, $content);
	$content = str_ireplace("[%UITERSTEBETAALDATUM%]", strftime("%e %B %Y", strtotime($rk->UitersteBetaaldatum)), $content);
	$content = str_ireplace("[%NAAMVERENIGING%]", $_SESSION['naamvereniging'], $content);
	$content = str_ireplace("[%NAAMWEBSITE%]", $_SESSION['naamwebsite'], $content);
	$content = str_ireplace("[%URLWEBSITE%]", $_SESSION['urlwebsite'], $content);
	
	$rr = db_rekening("regels", $nr);
	$rrtxt = "";
	foreach($rr as $regel) {
		$nl = db_naamlid($regel->Lid, "Naam", "");
		$rrtxt .= sprintf("<tr><td>%d</td><td>%s</td><td>%s</td><td class='number'>%03.2f</td></tr>\n", $regel->Regelnr, $regel->OMSCHRIJV, $nl, $regel->Bedrag);
	}
	$content = str_ireplace("[%REKENINGREGELS%]", $rrtxt, $content);
	
	return $content;

} # RekeningDetail

?>
