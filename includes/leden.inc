<?php
		
if (toegang("Overzicht lid", 0)) {
	$ldl = "<a href='index.php?tp=Overzicht+lid&amp;lidid=%d'>%s</a>";
} else {
	$ldl="";
}

function fnWieiswie($actie, $metfoto=1) {
	global $ldl;

	fnDispMenu(2);
	
	echo("<div id='wieiswie'>\n");
	$vo = "";
	if ($actie == "Onderscheidingen") {
		$lijst = db_adressenlijst("O.TYPE='O'");
	} elseif ($actie == "Overige commissies") {
		$lijst = db_adressenlijst("(O.TYPE='C' AND O.Kader=False)");
	} else {
		$lijst = db_adressenlijst("O.Kader=True");
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
			$ln = htmlentities($row->LidNaam);
			if (isset($row->EmailFunctie) and isValidMailAddress($row->EmailFunctie, 0)) {
				$email = fnDispEmail($row->EmailFunctie, $row->LidNaam, 1);
			} elseif (isValidMailAddress($row->EmailVereniging, 0)) {
				$email = fnDispEmail($row->EmailVereniging, $row->LidNaam, 1);
			} else {
				$email = "";
			}
			if (strlen($row->OmsFunctie) > 1) {
				$func = $row->OmsFunctie;
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
				printf("<th colspan=4>%s</th>\n", $row->OndNaam);
				if (isValidMailAddress($row->CentraalEmail, 0)) {
					printf("<tr>\n<td>&nbsp;</td><td colspan='2'><strong>Centraal e-mailadres</strong></td>\n<td>%s</td>\n</tr>\n", fnDispEmail($row->CentraalEmail, $row->OndNaam, 0, 1));
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
	global $arrTL, $ldl, $table_prefix;

	$val_naamfilter="";
	$val_TL=$arrTL[1];
	$val_groep=0;
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['tbNaamFilter']) and strlen($_POST['tbNaamFilter']) > 0) {
			$val_naamfilter = $_POST['tbNaamFilter'];
		}
		if (isset($_POST['rbTL']) and strlen($_POST['rbTL']) > 0) {
			$val_TL = $_POST['rbTL'];
		}
		if (isset($_POST['lbGroepFilter'])) {
			$val_groep = $_POST['lbGroepFilter'];
		}
	}
	
	echo("<div id='filter'>\n");
	printf("<form name='Filter' action='%s?%s' method='post'>", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
	echo("<table>\n");
	echo("<tr>\n");
	printf("<td class='label'>Naam bevat</td><td><input type='text' name='tbNaamFilter' size=20 value='%s' placeholder='Achter- of roepnaam' onblur='form.submit();'></td>\n", $val_naamfilter);
	echo("<td>");
	foreach($arrTL as $tl) {
		if ($tl == $val_TL) {$c=" checked"; } else { $c=""; }
		if ($tl != "Klosleden" or $val_groep == 0) {
			printf('<input type="radio"%2$s name="rbTL" value="%1$s" onclick="form.submit();">%1$s', $tl, $c);
		}
	}
	echo("</td>\n");
	printf("<td class='label'>Groep</td><td><select name='lbGroepFilter' onchange='form.submit();'>%s</select></td>\n", fnSelectListGroepen($val_groep));
	echo("</tr>\n");
	echo("</table>\n");
	echo("</form>");
	echo("</div>  <!-- Einde filer -->\n");
	
	if (strlen($val_naamfilter) > 1) {
		$filter = sprintf('(L.Achternaam LIKE "%1$s" OR L.Roepnaam LIKE "%1$s")', "%" . strtoupper($val_naamfilter) . "%");
	} else {
		$filter = "";
	}
	
	if ($val_groep == 0) {
		if ($val_TL == "Leden") {
			if (strlen($filter) > 0) {$filter .= " AND "; }
			$filter .= "LM.Lidnr > 0 AND ((LM.Opgezegd IS NULL) OR LM.Opgezegd > CURDATE())";
		} elseif ($val_TL == "Klosleden") {
			if (strlen($filter) > 0) {$filter .= " AND "; }
			$filter .= "(LM.Lidnr IS NULL)";
		} elseif ($val_TL == "Voormalig leden") {
			if (strlen($filter) > 0) {$filter .= " AND "; }
			$filter .= "(NOT LM.Opgezegd IS NULL) AND LM.Opgezegd < CURDATE()";
		}
	} else {
		if (strlen($filter) > 0) {$filter .= " AND ";}
		$filter .= sprintf("L.Nummer IN (SELECT Lid FROM %sLidond AS LO WHERE L.Nummer = LO.Lid AND LO.OnderdeelID=%d", $table_prefix, $val_groep);
		if ($val_TL == "Leden") {
			$filter .= " AND (LO.Vanaf <= CURDATE() AND (LO.Opgezegd IS NULL) OR LO.Opgezegd >= CURDATE())";
		} elseif ($val_TL == "Voormalig leden") {
			$filter .= " AND (LO.Vanaf <= CURDATE() AND (NOT LO.Opgezegd IS NULL) OR LO.Opgezegd < CURDATE())";
		}
		$filter .= ")";
	}

	$rows = db_ledenlijst($filter);
	if (count($rows) > 0) {
		echo(fnDisplayTable($rows, $ldl));
		foreach ($rows as $row) {
			$sel_leden[] = $row->lnkNummer;
		}
		$_SESSION['sel_leden'] = $sel_leden;
	}
}

function fnOverviewLid($lidid=0, $actie="") {
	global $currenttab, $toneninschrijvingenbewakingen;
	
	if ($lidid > 0) {
		fnDispMenu(2, "lidid=" . $lidid);

		$rows = db_gegevenslid($lidid, "Alg");
		if (count($rows) > 0) {
			$naamlid = $rows[0]->Naam;
		} else {
			$naamlid = "onbekend";
		}
			
		if ($actie == "Afdelingen") {
			$rows = db_gegevenslid($lidid, $actie);
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", $actie . " " . $naamlid));
			} else {
				printf("<p class='mededeling'>%s heeft geen %s.</p>\n", $naamlid, $actie);
			}
		} elseif ($actie == "Kader") {
			$rows = db_gegevenslid($lidid, $actie);
			if (count($rows) > 0) {
				echo(fnDisplayTable($rows, "", $actie . " " . $naamlid));
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
			if ($_SESSION['aantalrekeningen'] > 0) {
				$rows = db_gegevenslid($lidid, "Rekening");
				if (count($rows) > 0) {
					echo(fnDisplayTable($rows, "", "Rekeningen " . $naamlid));
				} else {
					printf("<p class='mededeling'>%s heeft geen rekeningen ontvangen.</p>", $naamlid);
				}
			}
			$rows = db_gegevenslid($lidid, "Financieel");
			if (count($rows) > 0) {
				echo(fnDisplayForm($rows[0]));
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
					echo(fnDisplayTable($rows, $ld, "Ontvangen mails " . $naamlid));
				} else {
					printf("<p class='mededeling'>%s heeft geen mails vanaf deze website ontvangen.</p>\n", $naamlid);
				}
			}
		} elseif ($actie == "Logboek") {
			echo(fnDisplayTable(db_logboek("lijst", "", -1, $lidid)));
		} elseif (toegang($_GET['tp'])) {
			$rows = db_gegevenslid($lidid);
			if (count($rows) > 0) {
				$fn = fotolid($rows[0]->RecordID, 1);
				if (strlen($fn) > 3) {
					$xtra = "<div id='pasfoto'>"
							. sprintf("<img src='%s' alt='Foto %s'>\n", $fn, $rows[0]->ndRoepnaam)
							. "</div>  <!-- Einde pasfoto -->\n";
				} else {
					$xtra = "";
				}
				echo(fnDisplayForm($rows[0], $xtra));
			}
		} else {
			echo("<p class='mededeling'>Je hebt geen toegang.</p>\n");
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
	} else {
		printf("<p class='mededeling'>Er is geen lid geselecteerd. Selecteer een lid via de <a href='%s?tp=Ledenlijst'>ledenlijst</a>.</p>\n", $_SERVER['PHP_SELF']);
	}
}

function fnWijzigen($lidid=0, $actie="") {
	global $currenttab, $currenttab2, $pasfotoextenties, $emailledenadministratie, $emailnieuwepasfoto, $selfservicediplomas, $opzegtermijn;
	global $naamvereniging, $naamwebsite, $urlwebsite, $table_prefix, $emailsecretariaat, $gebruikopenid;
	global $muteerbarememos, $emailbevestiginginschrijving, $voorwaardeninschrijving;
	global $arrLegitimatie, $arrSoortMemo;
	
	if ($lidid == 0) {
		$lidid = $_SESSION['lidid'];
	}
	if (strlen($actie) == 0) {
		$actie = $currenttab2;
	}
	
	$naamlid = db_naamlid($lidid);
	if ($lidid > 0) {
//		$lidid = 32;
		fnDispMenu(2, "lidid=" . $lidid);

		if ($actie == "Algemene gegevens") {
			$wijzvelden[] = array('label' => "Roepnaam", 'naam' => "Roepnaam", 'lengte' => 17);
			$wijzvelden[] = array('label' => "Voorletters", 'naam' => "VOORLETTER", 'lengte' => 10);
			$wijzvelden[] = array('label' => "Tussenvoegsels", 'naam' => "Tussenv", 'lengte' => 7);
			$wijzvelden[] = array('label' => "Achternaam", 'naam' => "Achternaam", 'lengte' => 30);
			$wijzvelden[] = array('label' => "Meisjesnaam", 'naam' => "Meisjesnm", 'lengte' => 25);
			$wijzvelden[] = array('label' => "Adres", 'naam' => "Adres", 'lengte' => 30);
			$wijzvelden[] = array('label' => "Postcode", 'naam' => "Postcode", 'lengte' => 7);
			$wijzvelden[] = array('label' => "Woonplaats", 'naam' => "Woonplaats", 'lengte' => 22);
			$wijzvelden[] = array('label' => "Telefoon", 'naam' => "Telefoon", 'lengte' => 16);
			$wijzvelden[] = array('label' => "Mobiel", 'naam' => "Mobiel", 'lengte' => 16);
			$wijzvelden[] = array('label' => "E-mail", 'naam' => "EMAIL", 'lengte' => 45);
			$wijzvelden[] = array('label' => "Geboortedatum", 'naam' => "GEBDATUM", 'lengte' => 18);
			$wijzvelden[] = array('label' => "Geboorteplaats", 'naam' => "GEBPLAATS", 'lengte' => 22);
			$wijzvelden[] = array('label' => "Bankrekening", 'naam' => "BANKGIRO", 'lengte' => 10);
			$wijzvelden[] = array('label' => "Legitimatietype", 'naam' => "Legitimatietype");
			$wijzvelden[] = array('label' => "Legitimatienummer", 'naam' => "Legitimatienummer", 'lengte' => 15);
			$wijzvelden[] = array('label' => "Beroep", 'naam' => "Beroep", 'lengte' => 40);
			
			if ($_SERVER['REQUEST_METHOD'] == "POST") {
				$mess = "";
				for ($i=0; $i < count($wijzvelden); $i++) {
					$fn = $wijzvelden[$i]['naam'];
					$v = "__skip__";
					if (isset($_POST["chkLeeg_" . $fn]) and $_POST["chkLeeg_" . $fn] == "1") {
						if ($fn == "Achternaam") {
							$mess .= "Achternaam mag niet leeg zijn, deze wijziging wordt niet verwerkt. ";
						} else {
							$v = "";
						}
					} elseif ($wijzvelden[$i]['label'] == "Legitimatietype") {
						$v = substr($_POST[$fn], 0 , 1);
					} elseif (stripos($wijzvelden[$i]['label'], "datum") !== FALSE) {
						$_POST[$fn] = change_month_to_uk($_POST[$fn]);
						if (isset($_POST[$fn]) and strlen($_POST[$fn]) > 0) {
							if (strtotime($_POST[$fn]) === FALSE) {
								$mess .= "Geboortedatum is niet correct, deze wordt niet verwerkt. ";
							} else {
								$v = strftime("%Y-%m-%d", strtotime($_POST[$fn]));
							}
						}
					} elseif (stripos($wijzvelden[$i]['label'], "e-mail") !== FALSE) {
						if (isset($_POST[$fn]) and strlen($_POST[$fn]) > 0) {
							if (isValidMailAddress($_POST[$fn], 0)) {
								$v = $_POST[$fn];
							} else {
								$mess .= "E-mailadres is niet correct, deze wijziging wordt niet verwerkt. ";
							}
						}
					} elseif (isset($_POST[$fn]) and strlen($_POST[$fn]) > 0) {
						$v = $_POST[$fn];
					}
					if ($v != "__skip__" and $v != $_POST["old_" . $fn]) {
						$query = sprintf("UPDATE %sLid SET %s='%s', Gewijzigd=SYSDATE() WHERE Nummer=%d;", $table_prefix, $fn, $v, $lidid);
						$result = fnQuery($query);
						if ($result > 0) {
							db_interface("add", $query);
							$mess .= sprintf("%s van %s is in '%s' gewijzigd. ", $wijzvelden[$i]['label'], db_naamlid($lidid), $v);
						} else {
							$mess .= sprintf("De wijziging in %s bij %s is niet verwerkt. ", strtolower($wijzvelden[$i]['label']), db_naamlid($lidid));
						}
					}
				}
				if (strlen($mess) > 1) {
					printf("<p class='mededeling'>%s</p>\n", $mess);
					db_logboek("add", $mess, 6);
				}
			}
			
			$row = db_ledenwijzigingen($lidid);
			echo("<div id='wijzigengegevens'>\n");
			printf("<form method='post' action='%s?%s' name='frm_wijzigingen'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
			echo("<table>\n");
			echo("<tr><th>Veld</th><th>Huidige waarde</th><th>Nieuwe waarde</th><th>Leeg?</th></tr>\n");
			$oldvals = "";
			for ($i=0; $i < count($wijzvelden); $i++) {
				$dv = $row->$wijzvelden[$i]['naam'];
				$ph = $wijzvelden[$i]['label'];
				if ($wijzvelden[$i]['label'] == "Legitimatietype") {
					$t = "Legitimatie";
					$dv = $arrLegitimatie[$dv];
				} elseif (strpos($wijzvelden[$i]['label'], "datum") !== FALSE) {
					$t = "text";
					$dv = strftime("%e %B %Y", strtotime($dv));
				} elseif (stripos($wijzvelden[$i]['label'], "e-mail") !== FALSE) {
					$t = "email";
				} else {
					$t = "text";
				}
				if ($t == "Legitimatie") {
					$opt = "\n";
					foreach ($arrLegitimatie as $key => $val) {
						$sel = "";
						if ($key == $row->$wijzvelden[$i]['naam']) {
							$sel = " selected";
						}
						$opt .= sprintf("<option value='%s'%s>%s</option>\n", $key, $sel, $val);
					}
					$inp = sprintf("<select name='%s'>%s</select></td><td>", $wijzvelden[$i]['naam'], $opt);
				} else {
					$inp = sprintf("<input type='%s' name='%s' placeholder='%s' size=40 maxlength=%d></td><td class='chk'><input type='checkbox' name='chkLeeg_%s' value=1>", 
							$t, $wijzvelden[$i]['naam'], $ph, $wijzvelden[$i]['lengte'], $wijzvelden[$i]['naam']);
				}
				printf('<tr><td class="label">%1$s</td><td>%2$s</td><td>%3$s</td></tr>', $wijzvelden[$i]['label'], $dv, $inp);
				$oldvals .= sprintf("<input type='hidden' value='%s' name='old_%s'>\n", $row->$wijzvelden[$i]['naam'], $wijzvelden[$i]['naam']);
				echo("\n");
			}
			echo("<tr><th colspan=4><input type='submit' value='Bewaren' name='adreswijziging'></th><tr>\n");
			echo("</table>\n");
			echo($oldvals);
				
			echo("</form>\n");
			echo("</div>  <!-- Einde invulformulier -->\n");
		} elseif ($actie == "Diplomas") {
			if ($_SERVER['REQUEST_METHOD'] == "POST") {
				foreach (db_diploma("selfservice_lijst") as $row) {
					$fnRID = sprintf("RID_%d", $row->RecordID);
					$fnBehaald = sprintf("Behaald_%d", $row->RecordID);
					$fnVervaltPer = sprintf("VervaltPer_%d", $row->RecordID);
					$fnDiplnr = sprintf("Diplnr_%d", $row->RecordID);
					$fnVerw = sprintf("chkVerw_%d", $row->RecordID);
					if (isset($_POST[$fnVerw]) and $_POST[$fnVerw] == 1) {
						db_liddipl("delete", $lidid, 0, $row->RecordID, $_POST[$fnBehaald]);
					} elseif ($_POST[$fnRID] == 0) {
						db_liddipl("add", $lidid, 0, $row->RecordID, $_POST[$fnBehaald], $_POST[$fnVervaltPer], $_POST[$fnDiplnr]);
					} else {
						db_liddipl("update", 0, $_POST[$fnRID], 0, $_POST[$fnBehaald], $_POST[$fnVervaltPer], $_POST[$fnDiplnr]);
					}
				}
			}
			$oldvals = "";
			
			echo("<div id='wijzigendiplomas'>\n");
			printf("<form method='post' action='%s?%s' name='frm_diplwijz'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
			echo("<table>\n");
			printf("<tr><th colspan=6>Diploma's %s</th></tr>\n", $naamlid);
			printf("<tr><th>Code</th><th>Naam</th><th>Behaald op</th><th>Geldig tot</th><th>Diplomanummer</th><th>Verw?</th></tr>\n");
			foreach (db_diploma("selfservice_lijst") as $row) {
				$cv_rid = 0;
				$cv_behaald = "";
				$cv_vervaltop = "";
				$cv_diplomanr = "";
				$rowld = db_liddipl("diplomabehaald", $lidid, 0, $row->RecordID);
				if (isset($rowld->RecordID)) {
					$cv_rid = $rowld->RecordID;
					$cv_behaald = $rowld->EXDATUM;
					$cv_vervaltop = $rowld->LicentieVervallenPer;
					$cv_diplomanr = str_replace("\"", "'", $rowld->Diplomanummer);
				}
				if ($cv_rid == 0) {
					printf('<tr><td>%1$s</td><td>%2$s</td><td><input type=\'text\' name=\'Behaald_%6$d\' value="%3$s"></td><td><input type=\'text\' name=\'VervaltPer_%6$d\' value="%4$s"></td><td><input type=\'text\' name=\'Diplnr_%6$d\' value="%5$s" maxlength=14></td><td></td></tr>', $row->Kode, $row->Naam, $cv_behaald, $cv_vervaltop, $cv_diplomanr, $row->RecordID);
				} else {
					printf('<tr><td>%1$s</td><td>%2$s</td><td>%3$s</td><td><input type=\'text\' name=\'VervaltPer_%6$d\' value="%4$s"></td><td><input type=\'text\' name=\'Diplnr_%6$d\' value="%5$s" maxlength=14></td><td class=\'chk\'><input type=\'checkbox\' name=\'chkVerw_%6$d\' value=1></td></tr>', $row->Kode, $row->Naam, strftime("%e %B %Y", strtotime($cv_behaald)), $cv_vervaltop, $cv_diplomanr, $row->RecordID);
					$oldvals .= sprintf("<input type='hidden' name='Behaald_%d' value='%s'>", $row->RecordID, $cv_behaald);
				}
				$oldvals .= sprintf("\n<input type='hidden' name='RID_%d' value=%d>", $row->RecordID, $cv_rid);
				echo("\n\n");
			}
			echo("<tr><th colspan=6><input type='submit' value='Bewaren' name='diplwijz'></th></tr>\n");
			echo("</table>\n");
			echo($oldvals);
			echo("</form>\n");
			echo("</div>  <!-- Einde wijzigendiplomas -->\n");
			
		} elseif ($actie == "Bijzonderheden" and strlen($muteerbarememos) > 0) {
			if ($_SERVER['REQUEST_METHOD'] == "POST") {
				for ($iCounter=0; $iCounter < strlen($muteerbarememos); $iCounter++) {
					$kodesoort = substr($muteerbarememos, $iCounter, 1);
					$namevar = "Memo_" . $kodesoort;
					$curval = db_memo($lidid, $kodesoort, "curval");
					if (strlen($_POST[$namevar]) == 0) {
						db_memo($lidid, $kodesoort, "delete");
					} elseif (strlen($curval) == 0) {
						db_memo($lidid, $kodesoort, "insert", $_POST[$namevar]);
					} elseif ($curval != $_POST[$namevar]) {
						db_memo($lidid, $kodesoort, "update", $_POST[$namevar]);
					}
				}
			}
		
			echo("<div id='bijzonderhedenwijzigen'>\n");
			printf("<form method='post' action='%s?%s' name='bijz_wijz'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
			echo("<table>\n");
			for ($iCounter=0; $iCounter < strlen($muteerbarememos); $iCounter++) {
				$kodesoort = substr($muteerbarememos, $iCounter, 1);
				$namevar = "Memo_" . $kodesoort;
				if (array_key_exists($kodesoort, $arrSoortMemo)) {
					$naamsoort = $arrSoortMemo[$kodesoort];
				} else {
					$naamsoort = "Overig " . $kodesoort;
				}
				$curval = db_memo($lidid, $kodesoort, "curval");
				printf("<tr><td class='label'>%s</td><td><textarea cols=75 rows=10 name='%s'>%s</textarea></td></tr>\n", $naamsoort, $namevar, $curval);
			}
			echo("<tr><th colspan=2><input type='submit' value='Bewaren' name='wijziging'></th><tr>\n");
			echo("</table>\n");
			echo("</form>\n");
			echo("</div>  <!-- Einde bijzonderhedenwijzigen -->\n");
			
		} elseif ($actie == "Profiel") {
		
			if ($_SERVER['REQUEST_METHOD'] == "POST") {
				if (isset($_POST['pw_nieuw']) and strlen($_POST['pw_nieuw']) > 0) {
					if (strlen($_POST['pw_nieuw']) < 7) {
						$mess = "Het nieuwe wachtwoord is te kort, het moet minimaal 7 karakters lang zijn.";
					} elseif ($_POST['pw_nieuw'] != cleanlogin($_POST['pw_nieuw'])) {
						$mess = "Het nieuwe wachtwoord bevat ongeldige tekens.";
					} elseif ($_POST['pw_nieuw'] !== $_POST['pw_herhaal']) {
						$mess = "Nieuwe wachtwoorden zijn niet gelijk.";
					} else {
						$mess = db_change_password($_POST['pw_nieuw'], $_POST['pw_oud'], $lidid);
					}
					printf("<p class='mededeling'>%s</p>", $mess);
				}
				echo("<p><a href='/'>Klik hier om verder te gaan.</a></p>\n");
			} else {
				echo("<div id='profielwijzigen'>\n");
				printf("<form name='ProfielWijzigen' action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER['QUERY_STRING']);
				printf("<fieldset>
				<h3>Profiel wijzigen</h3>
				<label for='login'>Login:</label><input type='text' name='login' id='ingelogdlogin' value='%s' readonly='readonly'>
				<label for='pw_oud'>Oude wachtwoord:</label><input type='password' name='pw_oud' id='pw_oud' size=10 maxlength=12>
				<label for='pw_nieuw'>Nieuw wachtwoord:</label><input type='password' name='pw_nieuw' id='pw_nieuw' size=10 maxlength=12>
				<label for='pw_herhaal'>Herhaal wachtwoord:</label><input type='password' name='pw_herhaal' id='pw_herhaal' size=10 maxlength=12>
				<input type='submit' class='knop' value='Wijzigen'>\n", $_SESSION['username']);
				echo("</fieldset>
				</form>
				</div>  <!-- Einde profielwijzigen -->");
				
				if ($gebruikopenid == 1) {
					$row = db_logins("controle");
					echo("<div id='openid'>\n
					<h3>Ondersteuning OpenID</h3>
					<p>Deze website ondersteunt <a href='http://nl.wikipedia.org/wiki/OpenID'>OpenID</a>. Het voordeel van OpenID is dat je niet voor elke website een aparte login hoeft aan te maken en te onthouden. Veel bezoekers hebben al een OpenID, veelal zonder zich dit te realiseren. Kijk maar eens <a href='http://openid.net/get-an-openid/'>hier</a>. Dit zijn vooral Amerikaanse websites. Wil je liever een Nederlandse OpenID, dan kan je <a href='http://mijnopenid.nl/'>hier</a> terecht.</p>\n");
					if (strlen($row->openid_identity) > 0 and strlen($row->Login) >= 6) {
						printf("<p>Jij hebt zowel een gewone login als een login via OpenID. Je gewone login kan je het beste verwijderen. Mocht je deze weer nodig hebben, kan je hem altijd weer aanvragen.</p>
						<ul>
						<li>%s&nbsp;<img src='images/delete.png' alt='Delete login'></li>
						<li>%s&nbsp;<img src='images/delete.png' alt='Delete login'></li>
						</ul>", $row->Login, $row->openid_identity);
						
	//				} elseif () {
						
					}
					echo("</div>  <!-- Einde openid -->\n");
				}
			}

		} elseif (toegang($_GET['tp'])) {
			$rows = db_gegevenslid($lidid);
			if (count($rows) > 0) {
				$fn = fotolid($rows[0]->RecordID);
				$xtra = "<div id='pasfoto'>";
				if (strlen($fn) > 3) {
					$xtra .= sprintf("<img src='%s' alt='Foto %s'>\n", $fn, $rows[0]->ndRoepnaam);
				}
				$xtra .= "</div>  <!-- Einde pasfoto -->\n";
				echo(fnDisplayForm($rows[0], $xtra));
			}
		} else {
			echo("<p class='mededeling'>Je hebt geen toegang.</p>\n");
		}
	} else {
		echo("<p class='mededeling'>Er is geen lid geselecteerd.</p>\n4");
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
	global $daysshowbirthdays;

	$verj = "";
	$verjfoto = "";
	for ($i = 0; $i <= $daysshowbirthdays; $i++) {
		$dteHV = mktime(0, 0, 0, date("m"), date("d")+$i, date("Y"));
		$w = sprintf("DAY(L.GEBDATUM)=%d AND MONTH(L.GEBDATUM)=%d", date("d", $dteHV), date("m", $dteHV))
			. " AND LM.LIDDATUM <= CURDATE() AND ((LM.Opgezegd IS NULL) OR LM.Opgezegd > CURDATE())";
		foreach(db_ledenlijst($w) as $row) {
			if ($i == 0) {
// >= 5.3					$interval = date_diff(date_create($row->ndGEBDATUM), date_create("now"));
// >= 5.3					$t = sprintf("%s is vandaag %d jaar geworden", $row->Naam_lid, $interval->format('%y'));	
				$lft = date("Y", strtotime("today")) - date("Y", strtotime($row->ndGEBDATUM));
				$t = sprintf("%s is vandaag %d jaar geworden", htmlentities($row->Naam_lid), $lft);
			} elseif ($i == 1) {
// >= 5.3					$interval = date_diff(date_create($row->ndGEBDATUM), date_create("tomorrow"));
// >= 5.3					$t = sprintf("%s wordt morgen %d jaar", $row->Naam_lid, $interval->format('%y'));
				$lft = date("Y", strtotime("tomorrow")) - date("Y", strtotime($row->ndGEBDATUM));
				$t = sprintf("%s wordt morgen %d jaar", htmlentities($row->Naam_lid), $lft);
			} else {
				$t = sprintf("Op %s is %s jarig", strftime("%e %B", strtotime($row->ndGEBDATUM)), htmlentities($row->Naam_lid));
			}
			$fn = fotolid($row->lnkNummer, 1);
			if (strlen($t) > 3) {
				$verj .= sprintf("%s. ", $t);
				if (strlen($fn) > 3) {
					$verjfoto .= sprintf("<div class='jarige'><img src='%s' alt='Pasfoto %s'><div class='tekstbijfoto'>%s.</div></div>\n", $fn, htmlentities($row->Naam_lid), $t);
				} elseif (strlen($t) > 3) {
					$verjfoto .= sprintf("<p>%s.</p>\n", $t);
				}
			}
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

function nieuwepasfoto($lidid=0, $filterlid="") {
	global $pasfotoextenties, $emailledenadministratie, $emailnieuwepasfoto;
	
	fnDispMenu(2);

	$max_size_attachm = 2 * 1024 * 1024;  // 2MB
	
	if (isset($_POST['lidpasfoto']) and $_POST['lidpasfoto'] > 0) {
		$lidid = $_POST['lidpasfoto'];
	}
	$naamlid = db_naamlid($lidid);
	
	if (isset($_FILES['UploadFoto']['name']) and strlen($_FILES['UploadFoto']['name']) > 3 and $lidid > 0) {

		$ad = $_SERVER["SCRIPT_FILENAME"];
		$ad = substr($ad, 0, strrpos($ad, "/")) . "/pasfoto/";
		chmod($ad, 0755);

		$ext = explode(".", $_FILES['UploadFoto']['name']);
		$ext = strtolower($ext[count($ext) - 1]);
		$target = $ad . sprintf("Pasfoto%d.%s", $lidid, $ext);
		$mess = "";
		if (in_array($ext, $pasfotoextenties) === false) {
			$mess = sprintf("Het bestand met extensie %s is niet toegestaan. De volgende extensies zijn toegestaan: %s.", $ext, implode(", ", $pasfotoextenties));
		} elseif ($_FILES['UploadFoto']['size'] >= $max_size_attachm) {
			$mess = sprintf("De foto kan niet worden ge-upload, omdat het groter is dan %d KB.", $max_size_attachm / 1024);
		} else {
			if (file_exists($target)) {
				$bu_name = substr($target, 0, strrpos($target, ".")) . "_" . strftime("%Y%m%d", filectime($target)) . "." . $ext;
				rename($target, $bu_name);
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
		if (strlen($mess) > 0) {
			printf("<p class='mededeling'>%s</p>\n", $mess);
			db_logboek("add", $mess, 6);
		}
	}
	
	$optionslid = "<option value=-1>Selecteer lid ...</option>\n";
	foreach (db_lid($filterlid) as $row) {
		$optionslid .= sprintf("<option value=%1\$d>%2\$s (%1\$d)</option>\n", $row->Nummer, htmlentities($row->Zoeknaam));
	}

	echo("<div id='nieuwepasfoto'>\n");
	$fn = fotolid($lidid, 1);
	if (strlen($fn) > 4) {
		printf("<img src='%s' alt='Huidige pasfoto %s'>\n", $fn, $naamlid);
	} elseif ($lidid > 0) {
		printf("<p class='mededeling'>Geen huidige pasfoto van %s beschikbaar.</p>\n", $naamlid);
	}
			
	printf("<form method='post' action='%s?%s' name='frm_pasfoto' enctype='multipart/form-data'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	printf("<input type='hidden' name='MAX_FILE_SIZE' value=%d>\n", $max_size_attachm);
	if ($lidid > 0) {
		printf("<p><label for='UploadFoto'>Nieuwe pasfoto %s:&nbsp;</label>\n", $naamlid);
		printf("<input type='hidden' name='lidpasfoto' value=%d>\n", $lidid);
	} else {
		printf("<p><select name='lidpasfoto'>%s</select></p>\n", $optionslid);
	}
	echo("<input type='file' name='UploadFoto' id='UploadFoto'>&nbsp;");
	echo("<input type='submit' name='Upload' value='Insturen'>\n");
	printf("<p>Het ideale formaat van de pasfoto is 390 pixels breed bij 500 pixels hoog. De foto mag niet groter dan %d KB zijn.</p>", $max_size_attachm / 1024);
	echo("</form>\n");

	echo("</div>  <!-- Einde nieuwepasfoto -->\n");	
}

function opzegginglidmaatschap($lidid) {
	global $opzegtermijn, $naamvereniging, $naamwebsite, $urlwebsite, $emailledenadministratie, $emailsecretariaat;
	
	$fdlang = "%e %B %Y";
	
	if (!isset($opzegtermijn) or $opzegtermijn < 0) {
		$opzegtermijn = 1;
	}
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
			
		$body = sprintf("<p>Beste ledenadministratie,</p>\n
		<p>Hierbij zeg ik mijn lidmaatschap van de %s per %s op. Mijn lidnummer is %d.</p>\n
		%s
		<p>Met vriendelijke groeten,<br>
		<strong>%s</strong></p>\n", $naamvereniging, $opgezegdper, $_SESSION['lidnr'], $opm, $_SESSION['naamingelogde']);
				
		$body .= sprintf("\n<p>Dit formulier is verzonden met formulier %s?%s vanaf IP %s.</p>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING'], $_SERVER['REMOTE_ADDR']);
	
		$mail = new RBMmailer();
		$mail->From = $_SESSION['emailingelogde'];
		$mail->FromName = $_SESSION['naamingelogde'];
		$mail->AddAddress($emailledenadministratie);
		$mail->AddCC($_SESSION['emailingelogde']);
		if (strlen($emailsecretariaat) > 5) {
			$mail->AddCC($emailsecretariaat);
		}
		$mail->Subject = sprintf("Opzegging lidmaatschap %s per %s", $_SESSION['naamingelogde'], $opgezegdper);
		$mail->Body = $body;
		if ($mail->Send()) {
			db_logboek("add", sprintf("%s heeft zijn lidmaatschap per %s opgezegd.", $_SESSION['naamingelogde'], $opgezegdper), 6);
			echo("<p class='mededeling'>De opzegging is verzonden.</p>\n");
		}
		$mail = null;
	} else {
		$form_opzegging = sprintf("<div id='opzegformulier'>\n<form name='Opzegging' action='%s?%s' method='post'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
		$form_opzegging .= "<fieldset>
<h3>Opzegging lidmaatschap</h3>
<label for='NaamLid'>Naam lid:</label><input type='text' name='NaamLid' value='[%NAAMLID%]' readonly='readonly'>
<label for='Lidnr'>Lidnummer:</label><input type='text' name='Lidnr' value='[%LIDNR%]' readonly='readonly'>
<label for='OpzeggingPer'>Opzegging lidmaatschap per:</label><input type='text' name='OpzeggingPer' value='[%OPZEGGENVANAF%]' maxlength=18 size=20>
<textarea name='RedenOpmerking' rows=8 cols=50 title='Reden en/of opmerkingen'></textarea>
<input type='submit' name='VerstuurOpzegging' value='Verstuur opzegging'>
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
		$content = str_replace("[%NAAMVERENIGING%]", $naamvereniging, $content);
		$content = str_replace("[%NAAMWEBSITE%]", $naamwebsite, $content);
		$content = str_replace("[%URLWEBSITE%]", $urlwebsite, $content);
		
		echo($content);
	}
}

?>