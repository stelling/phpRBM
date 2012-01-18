<?php
include('./includes/standaard.inc');
		
if (toegang("Overzicht lid", 0)) {
	$ldl = "<a href='index.php?tp=Overzicht+lid&amp;lidid=%d'>%s</a>";
} else {
	$ldl="";
}

if ($_SESSION['aantallid'] == 0) {
	echo("<script>alert('Voordat deze website gebruikt kan worden moeten er eerst gegevens uit de Access-database ge-upload worden.');
		location.href='/admin.php?tp=Uploaden+data';</script>\n");
} elseif ((!isset($lidid) or $lidid == 0) and isset($_SESSION['lidid'])) {
	$lidid = $_SESSION['lidid'];
} else {
	$lidid = 0;
}
	
HTMLheader();

if ($currenttab == "Eigen gegevens" and toegang($_GET['tp'])) {
	if ($_SESSION['lidid'] > 0) {
		fnOverviewLid($_SESSION['lidid']);
	} else {
		echo("<p class='mededeling'>Er is geen lid ingelogd.</p>\n");
	}
} elseif ($currenttab == "Wijzigen" and toegang($_GET['tp'])) {
	if ($_SESSION['lidid'] > 0) {
		fnWijzigen($_SESSION['lidid']);
	} else {
		echo("<p class='mededeling'>Er is geen lid ingelogd.</p>\n");
	}	
} elseif ($currenttab == "Overzicht lid" and toegang($_GET['tp'])) {
	if (isset($_GET['lidid']) and is_numeric($_GET['lidid']) and $_GET['lidid'] > 0) {
		fnOverviewLid($_GET['lidid']);
	} else {
		fnOverviewLid();
	}
} elseif ($currenttab == "Login aanvragen" and $_SESSION['lidid'] == 0) {
	fnLoginAanvragen();
} elseif ($currenttab == "Wie is wie" and toegang($_GET['tp'])) { 
	fnWieiswie();
} elseif ($currenttab == "Ledenlijst" and toegang($_GET['tp'])) {
	fnLedenlijst();
} elseif ($currenttab == "Bewaking" and toegang($_GET['tp'])) {
	fnBewaking();
} elseif ($currenttab == "Kostenoverzicht" and toegang($_GET['tp'])) {
	fnKostenoverzicht();
} elseif ($currenttab == "Mailing" and toegang($_GET['tp'])) {
	fnMailing();
} else {
	$currenttab = "Verenigingsinfo";
	fnVoorblad();
}
	
HTMLfooter();

function fnVoorblad() {
	global $daysshowbirthdays;

	$myFile = 'templates/verenigingsinfo.html';
	$content = file_get_contents($myFile);
	$begin_blok = "<!-- Ingelogd -->";
	$einde_blok = "<!-- /Ingelogd -->";
	if ($content !== false) {
		if ($_SESSION['lidid'] == 0) {
			$content_uitgelogd = "";
			$offset = 0;
			while ($offset < strlen($content)) {
				if (strpos($content, $begin_blok, $offset) === FALSE) {
					$content_uitgelogd .= substr($content, $offset);
					$offset = strlen($content);
				} else {
					$eb = strpos($content, $begin_blok, $offset);
					$content_uitgelogd .= substr($content, $offset, $eb-$offset);
					$offset = strpos($content, $einde_blok, $offset) + strlen($einde_blok);
				}
			}
			$content = $content_uitgelogd;
		}
	
		// Algemene statistieken 
		$stats = db_stats();
		foreach (array('aantalleden', 'aantalvrouwen', 'aantalmannen', 'gemiddeldeleeftijd', 'aantalkaderleden', 'nieuwstelogin', 'aantallogins', 'nuingelogd') as $v) {
			$content = str_replace("[%" . strtoupper($v) . "%]", $stats[$v], $content);
		}
		$content = str_replace("[%LAATSTGEWIJZIGD%]", strftime("%e %B %Y (%H:%m)", strtotime($stats['laatstgewijzigd'])), $content);
		
		// Gebruiker-specifieke statistieken
		if (isset($_SESSION['lidid']) and $_SESSION['lidid'] > 0) {
			$stats = db_stats($_SESSION['lidid']);
			$content = str_replace("[%NAAMLID%]", $_SESSION['naamingelogde'], $content);
			$content = str_replace("[%LIDNR%]", $_SESSION['lidnr'], $content);
			$content = str_replace("[%INGELOGDEGEWIJZIGD%]", strftime("%e %B %Y (%H:%m)", strtotime($stats['laatstgewijzigd'])), $content);
		} else {
			$content = str_replace("[%INGELOGDEGEWIJZIGD%]", "", $content);
		}
		$content = str_replace("[%ROEPNAAM%]", $_SESSION['roepnaamingelogde'], $content);
	
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
					$t = sprintf("%s is vandaag %d jaar geworden", $row->Naam_lid, $lft);
				} elseif ($i == 1) {
// >= 5.3					$interval = date_diff(date_create($row->ndGEBDATUM), date_create("tomorrow"));
// >= 5.3					$t = sprintf("%s wordt morgen %d jaar", $row->Naam_lid, $interval->format('%y'));
					$lft = date("Y", strtotime("tomorrow")) - date("Y", strtotime($row->ndGEBDATUM));
					$t = sprintf("%s wordt morgen %d jaar", $row->Naam_lid, $lft);
				} else {
					$t = sprintf("Op %s is %s jarig", strftime("%e %B", strtotime($row->ndGEBDATUM)), $row->Naam_lid);
				}
				$fn = fotolid($row->lnkNummer);
				if (strlen($t) > 3) {
					$verj .= sprintf("%s. ", $t);
					if (strlen($fn) > 3) {
						$verjfoto .= sprintf("<div class='jarige'><img src='%s' alt='Pasfoto %s'><div class='tekstbijfoto'>%s.</div></div>\n", $fn, $row->Naam_lid, $t);
					} elseif (strlen($t) > 3) {
						$verjfoto .= sprintf("<p>%s.</p>\n", $t);
					}
				}
			}
		}
		
		$content = str_replace("[%VERJAARFOTO%]", $verjfoto, $content);
		$content = str_replace("[%VERJAARDAGEN%]", $verj, $content);

		printf("<div id='welkomstekst'>\n%s</div>  <!-- Einde welkomstekst -->\n", $content);
	} else {
		debug("Geen content voor het voorblad", 0, 0, 1);
	}
}

function fnWieiswie() {
	global $ldl, $currenttab2, $kaderoverzichtmetfoto;

	fnDispMenu(2);
	
	echo("<div id='wieiswie'>\n");
	$vo = "";
	$metfoto = 1;
	if ($currenttab2 == "Onderscheidingen") {
		$lijst = db_adressenlijst("O.TYPE='O'");
	} elseif ($currenttab2 == "Overige") {
	$lijst = db_adressenlijst("(O.TYPE='C' AND O.Kader=False)");
		$metfoto = $kaderoverzichtmetfoto;
	} else {
		$lijst = db_adressenlijst("O.Kader=True");
		$metfoto = $kaderoverzichtmetfoto;
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
			$fn = fotolid($row->Nummer);
			if ($currenttab2 == "Onderscheidingen" and strlen($fn) > 3) {
				printf("<div class='kaartje'><img src='%s'><p class='naamkaderlid'>%s</p>\n<p>vanaf %s</p>\n</div>\n", $fn, $ln, strftime("%B %Y", strtotime($row->Vanaf)));
			} elseif ($currenttab2 == "Onderscheidingen") {
				printf("<div class='kaartje'><p class='naamkaderlid'>%s</p>\n<p>vanaf %s</p>\n</div>\n", $ln, strftime("%B %Y", strtotime($row->Vanaf)));
			} elseif (strlen($fn) > 3) {
				printf("<div class='kaartje'><img src='%s'><p class='naamkaderlid'>%s</p>\n<p class='functiekaderlid'>%s</p>\n<p class='mailkaderlid'>%s</p>\n</div>\n", $fn, $ln, $func, $email);
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
		echo(fnDiplayTable($rows, $ldl));
		foreach ($rows as $row) {
			$sel_leden[] = $row->lnkNummer;
		}
		$_SESSION['sel_leden'] = $sel_leden;
	}
}

function fnOverviewLid($lidid=0) {
	global $curtab, $currenttab, $currenttab2;
	
	if ($lidid > 0) {
		fnDispMenu(2, "lidid=" . $lidid);

		$rows = db_gegevenslid($lidid, "Alg");
		if (count($rows) > 0) {
			$naamlid = $rows[0]->Naam;
		} else {
			$naamlid = "onbekend";
		}
			
		if ($currenttab2 == "Afdelingen" and toegang($_GET['tp'])) {
			$rows = db_gegevenslid($lidid, $currenttab2);
			if (count($rows) > 0) {
				echo(fnDiplayTable($rows, "", $currenttab2 . " " . $naamlid));
			} else {
				printf("<p class='mededeling'>%s heeft geen %s.</p>\n", $naamlid, $currenttab2);
			}
		} elseif ($currenttab2 == "Kader" and toegang($_GET['tp'])) {
			$rows = db_gegevenslid($lidid, $currenttab2);
			if (count($rows) > 0) {
				echo(fnDiplayTable($rows, "", $currenttab2 . " " . $naamlid));
			} else {
				printf("<p class='mededeling'>%s is niet ingedeeld (geweest) bij het kader.</p>\n", $naamlid);
			}
		} elseif ($currenttab2 == "Rollen" and toegang($_GET['tp'])) {
			$rows = db_gegevenslid($lidid, $currenttab2);
			if (count($rows) > 0) {
				echo(fnDiplayTable($rows, "", $currenttab2 . " " . $naamlid));
			} else {
				printf("<p class='mededeling'>%s heeft geen %s.</p>\n", $naamlid, $currenttab2);
			}
		} elseif ($currenttab2 == "Groepen" and toegang($_GET['tp'])) {
			$rows = db_gegevenslid($lidid, $currenttab2);
			if (count($rows) > 0) {
				echo(fnDiplayTable($rows, "", $currenttab2 . " " . $naamlid));
			} else {
				printf("<p class='mededeling'>%s is bij geen enkele groep ingedeeld.</p>\n", $naamlid);
			}
		} elseif ($currenttab2 == "Bewaking" and toegang($_GET['tp'])) {
			$rows = db_gegevenslid($lidid, "Bew");
			if (count($rows) > 0) {
				echo(fnDiplayTable($rows, "", "Bewaking " . $naamlid));
			} else {
				printf("<p class='mededeling'>%s heeft geen bewakingshistorie.</p>", $naamlid);
			}
			$rows = db_gegevenslid($lidid, "Lidbew");
			if (count($rows) > 0) {
				echo(fnDiplayForm($rows[0]));
			}
		} elseif ($currenttab2 == "Diplomas" and toegang($_GET['tp'])) {
			$rows = db_liddipl("lidgegevens", $lidid);
			if (count($rows) > 0) {
				echo(fnDiplayTable($rows, "", "Diploma's " . $naamlid));
			} else {
				printf("<p class='mededeling'>Bij %s zijn geen diploma's bekend.</p>", $naamlid);
			}
		} elseif ($currenttab2 == "Financieel" and toegang($_GET['tp'])) {
			if ($_SESSION['aantalrekeningen'] > 0) {
				$rows = db_gegevenslid($lidid, "Rekening");
				if (count($rows) > 0) {
					echo(fnDiplayTable($rows, "", "Rekeningen " . $naamlid));
				} else {
					printf("<p class='mededeling'>%s heeft geen rekeningen ontvangen.</p>", $naanmlid);
				}
			}
			$rows = db_gegevenslid($lidid, "Financieel");
			if (count($rows) > 0) {
				echo(fnDiplayForm($rows[0]));
			}
		} elseif ($currenttab2 == "Mailing" and toegang($_GET['tp'])) {
			if (isset($_GET['MailingID']) and $_GET['MailingID'] > 0) {
				$xtra = "<p class='mededeling'><input type='button' value='Terug' onclick='history.go(-1);'></p>\n";
				echo(fnDiplayForm(db_mailing("histdetails", 0, $_GET['MailingID'])));
			} else {
				$ld = sprintf("<a href='index.php?tp=%s&amp;lidid=%d&amp;MailingID=%%d'>%%s</a>", urlencode($_GET['tp']), $lidid);
				$rows = db_gegevenslid($lidid, "Mailing");
				if (count($rows) > 0) {
					echo(fnDiplayTable($rows, $ld, "Mailings " . $naamlid));
				} else {
					printf("<p class='mededeling'>%s heeft geen mailings ontvangen.</p>\n", $naamlid);
				}
			}
		} elseif (toegang($_GET['tp'])) {
			$rows = db_gegevenslid($lidid);
			if (count($rows) > 0) {
				$fn = fotolid($rows[0]->RecordID);
				if (strlen($fn) > 3) {
					$xtra = "<div id='pasfoto'>"
							. sprintf("<img src='%s' alt='Foto %s'>\n", $fn, $rows[0]->ndRoepnaam)
							. "</div>  <!-- Einde pasfoto -->\n";
				} else {
					$xtra = "";
				}
				echo(fnDiplayForm($rows[0], $xtra));
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

function fnWijzigen($lidid=0) {
	global $currenttab, $currenttab2, $pasfotoextenties, $emailledenadministratie, $emailnieuwepasfoto, $selfservicediplomas, $opzegtermijn;
	global $naamvereniging, $naamwebsite, $urlwebsite, $table_prefix, $emailsecretariaat, $gebruikopenid;
	global $muteerbarememos, $emailbevestiginginschrijving, $voorwaardeninschrijving;
	
	$fdlang = "%e %B %Y";
	
	$arrLegitimatie["G"] = "Geen";
	$arrLegitimatie["I"] = "Identiteitskaart";
	$arrLegitimatie["O"] = "Onbekend";
	$arrLegitimatie["P"] = "Paspoort";
	$arrLegitimatie["R"] = "Rijbewijs";
	
	$arrSoortMemo["A"] = "Algemeen";
	$arrSoortMemo["B"] = "Bewaking";
	$arrSoortMemo["D"] = "Dieet";
	$arrSoortMemo["E"] = "Examen";
	$arrSoortMemo["F"] = "Financiën";
	$arrSoortMemo["G"] = "Gezondheid/medisch";
	$arrSoortMemo["I"] = "Inschrijving bewaking";
	
	if ($lidid == 0) {
		$lidid = $_SESSION['lidid'];
	}
	
	$rows = db_gegevenslid($lidid, "Alg");
	$naamlid = $rows[0]->Naam;
	
	if (isset($_FILES['UploadFoto']['name']) and strlen($_FILES['UploadFoto']['name']) > 3) {
		$max_size_attachm = 2097152;  // 2MB

		$ad = $_SERVER["SCRIPT_FILENAME"];
		$ad = substr($ad, 0, strrpos($ad, "/")) . "/pasfoto/";
		chmod($ad, 0755);

		$target = $ad . sprintf("Pasfoto%d.jpg", $lidid);
		$ext = explode(".", $_FILES['UploadFoto']['name']);  
		$ext = strtolower($ext[count($ext) - 1]); 
		if (in_array($ext, $pasfotoextenties) === false) {
			printf("<script>\nalert(\"Het bestand met extensie %s kan niet worden bijgesloten, omdat de extensie niet toegestaan is. Alleen de volgende extensies zijn toegestaan: %s\")\n</script>\n", $ext, $pasfotoextenties);
		} elseif ($_FILES['UploadFoto']['size'] > $max_size_attachm) {
			printf("<script>\nalert(\"De foto kan niet worden ge-upload, omdat het groter is dan %d KB.\")\n</script>\n", $max_size_attachm / 1024);
		} else {
			$image = new SimpleImage();
			$image->load($target);
			if ($image->GetWidth() > 400) {
				$image->resizeToWidth(400);
				$image->save($target);
			} else {
				move_uploaded_file($_FILES['UploadFoto']["tmp_name"], $target);
			}
			$image = null;
			
			if (isValidMailAddress($emailledenadministratie, 0) or isValidMailAddress($emailnieuwepasfoto, 0)) {
				$mail = new RBMmailer();
				$mail->From = $_SESSION['emailingelogde'];
				$mail->FromName = $_SESSION['naamingelogde'];
				$mail->Subject = "Nieuwe pasfoto " . $_SESSION['naamingelogde'];
				if (isValidMailAddress($emailnieuwepasfoto, 0) and $emailnieuwepasfoto != $emailledenadministratie) {
					$mail->AddAddress($emailnieuwepasfoto);
				}
				$mail->AddAddress($emailledenadministratie);
				$mail->AddAttachment($target);
				if ($mail->Send()) {
					db_add_activiteit(sprintf("%s heeft een nieuwe pasfoto ingestuurd.", $_SESSION['naamingelogde']), 6);
				}
				$mail = null;
			}
		}  
	}
	
	if ($lidid > 0) {
//		$lidid = 32;
		fnDispMenu(2, "lidid=" . $lidid);

		if ($currenttab2 == "Algemene gegevens" and toegang($_GET['tp'])) {
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
							$mess .= "Achternaam mag niet leeg zijn, deze wijziging wordt niet verwerkt.<br>\n";
						} else {
							$v = "";
						}
					} elseif ($wijzvelden[$i]['label'] == "Legitimatietype") {
						$v = substr($_POST[$fn], 0 , 1);
					} elseif (stripos($wijzvelden[$i]['label'], "datum") !== FALSE) {
						$_POST[$fn] = change_month_to_uk($_POST[$fn]);
						if (isset($_POST[$fn]) and strlen($_POST[$fn]) > 0) {
							if (strtotime($_POST[$fn]) === FALSE) {
								$mess .= "Geboortedatum is niet correct, deze wordt niet verwerkt.<br>\n";
							} else {
								$v = strftime("%Y-%m-%d", strtotime($_POST[$fn]));
							}
						}
					} elseif (stripos($wijzvelden[$i]['label'], "e-mail") !== FALSE) {
						if (isset($_POST[$fn]) and strlen($_POST[$fn]) > 0) {
							if (isValidMailAddress($_POST[$fn], 0)) {
								$v = $_POST[$fn];
							} else {
								$mess .= "E-mailadres is niet correct, deze wijziging wordt niet verwerkt.<br>\n";
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
							$mess = sprintf("De wijziging in %s is correct verwerkt.", strtolower($wijzvelden[$i]['label']));
							db_add_activiteit($mess, 6);
						} else {
							$mess .= sprintf("De wijziging in %s is niet verwerkt.<br>\n", strtolower($wijzvelden[$i]['label']));
						}
					}
				}
				if (strlen($mess) > 1) {
					printf("<p class='mededeling'>%s</p>\n", $mess);
				}
			}
			
			$row = db_ledenwijzigingen($lidid);
			echo("<div id='wijzigengegevens'>\n");
			printf("<form method='post' action='/index.php?%s' name='frm_wijzigingen'>\n", urlencode($_SERVER['QUERY_STRING']));
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
			echo("<tr><th colspan=4><input type='submit' value='Verstuur' name='adreswijziging'></th><tr>\n");
			echo("</table>\n");
			echo($oldvals);
				
			echo("</form>\n");
			echo("</div>  <!-- Einde invulformulier -->\n");
		} elseif ($currenttab2 == "Diplomas" and toegang($_GET['tp'])) {
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
			printf("<form method='post' action='/index.php?%s' name='frm_diplwijz'>\n", $_SERVER['QUERY_STRING']);
			echo("<table>\n");
			printf("<tr><th colspan=6>Diploma's %s</th></tr>\n", $naamlid);
			printf("<tr><th>Code</th><th>Naam</th><th>Behaald op</th><th>Geldig tot</th><th>Diplomanummer</th><th>Verw?</th></tr>\n");
			foreach (db_diploma("selfservice_lijst") as $row) {
				$cv_rid = 0;
				$cv_behaald = "";
				$cv_vervaltop = "";
				$cv_diplomanr = "";
				$query = sprintf("SELECT * FROM Liddipl WHERE Lid=%d AND DiplomaID='%s' ORDER BY EXDATUM DESC LIMIT 1;", $lidid, $row->RecordID);
				$resld = fnQuery($query);
				$rowsld = $resld->fetchAll();
				if (count($rowsld) > 0) {
					$cv_rid = $rowsld[0]->RecordID;
					$cv_behaald = $rowsld[0]->EXDATUM;
					$cv_vervaltop = $rowsld[0]->LicentieVervallenPer;
					$cv_diplomanr = str_replace("\"", "'", $rowsld[0]->Diplomanummer);
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
			echo("<tr><th colspan=6><input type='submit' value='Verstuur' name='diplwijz'></th></tr>\n");
			echo("</table>\n");
			echo($oldvals);
			echo("</form>\n");
			echo("</div>  <!-- Einde wijzigendiplomas -->\n");
		} elseif ($currenttab2 == "Pasfoto" and toegang($_GET['tp'])) {
			$row = db_ledenwijzigingen($lidid);
			echo("<div id='nieuwepasfoto'>\n");
			
			$fn = fotolid($lidid);
			if (strlen($fn) > 4 and file_exists($fn)) {
				printf("<img src='%s'>\n", $fn);
			} else {
				echo("<p>Geen huidige pasfoto beschikbaar.</p>\n");
			}
			
			printf("<form method='post' action='/index.php?%s' name='frm_wijzigingen' enctype='multipart/form-data'>\n", urlencode($_SERVER['QUERY_STRING']));
			echo("<table>\n");
			printf("<tr><th colspan=2>Nieuwe pasfoto %s</th></tr>\n", $naamlid);
			echo("<tr><td class='label'>Bestand</td><td><input type='file' name='UploadFoto' id='UploadFoto'>&nbsp;");
			echo("<input type='submit' name='Upload' value='Insturen'></td></tr>\n");
			echo("</table>\n");
			echo("</form>\n");
			echo("</div>  <!-- Einde nieuwepasfoto -->\n");
			
			echo("<p>Het ideale formaat van de pasfoto is 390 pixels breed bij 500 pixels hoog.</p>\n");
		} elseif ($currenttab2 == "Bijzonderheden" and strlen($muteerbarememos) > 0 and toegang($_GET['tp'])) {
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
			printf("<form method='post' action='/index.php?%s' name='bijz_wijz'>\n", $_SERVER['QUERY_STRING']);
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
			echo("<tr><th colspan=2><input type='submit' value='Verstuur' name='wijziging'></th><tr>\n");
			echo("</table>\n");
			echo("</form>\n");
			echo("</div>  <!-- Einde bijzonderhedenwijzigen -->\n");
		} elseif ($currenttab2 == "Inschrijving bewaking" and toegang($_GET['tp'])) {
			if ($_SERVER['REQUEST_METHOD'] == "POST") {
				$geldig = false;
				$bevins = "";
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
					$ins = db_insbew("inschrijving", $lidid, $row->RecordID);
					if (isset($ins->RecordID)) {
						if ($kz == 0 and strlen($_POST[$opm]) == 0) {
							db_insbew("delete", $lidid, $row->RecordID);
						} elseif ($ins->Keuze != $kz or $ins->Opmerking != $_POST[$opm]) {
							db_insbew("update", $lidid, $row->RecordID, $kz, $_POST[$opm]);
						}
					} elseif ($kz > 0 or strlen($_POST[$opm]) > 0) {
						db_insbew("add", $lidid, $row->RecordID, $kz, $_POST[$opm]);
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
								printf("<script>alert('Een bevestiging van deze inschrijving is naar %s verzonden.');</script>\n", $_SESSION['emailingelogde']);			
								echo("<script>location.href='/index.php';</script>\n");	
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
			echo("<table>\n");
				
			$geldig = false;
			foreach (db_insbew("openblokken") as $row) {
				$ins = db_insbew("inschrijving", $lidid, $row->RecordID);
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
			}
			printf("<tr><td colspan=7>Opmerkingen:<br><textarea cols=90 rows=8 name='opmalg'>%s</textarea></td></tr>", db_memo($lidid, "I", "curval"));
			echo("<tr><th colspan=7>");
			printf("<p>%s</p>", $voorwaardeninschrijving);
			echo("<input type='submit' value='Wijzigen' name='Wijzigen'>");
			if ($geldig) {
				echo("&nbsp;<input type='submit' value='Definitief maken' name='Definitief'>");
			}
			echo("</th><tr>\n");
			echo("</table>\n");
			echo("</form>\n");
			echo("</div>  <!-- Einde inschrijvingbewaking -->\n");
		} elseif ($currenttab2 == "Opzegging" and toegang($_GET['tp'])) {
		
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
				<p>Hierbij zeg ik mijn lidmaatschap van de %s op per %s. Mijn lidnummer is %d.</p>\n
				%s
				<p>Met vriendelijke groeten,<br>
				<strong>%s</strong></p>\n", $naamvereniging, $opgezegdper, $_SESSION['lidnr'], $opm, $_SESSION['naamingelogde']);
				
				$body .= sprintf("\n<p>Dit formulier is verzonden met formulier %s?%s vanaf IP %s.</p>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING'], $_SERVER['REMOTE_ADDR']);
			
				$mail = new RBMmailer();
				$mail->From = $_SESSION['emailingelogde'];
				$mail->FromName = $_SESSION['naamingelogde'];
				$mail->AddAddress($emailledenadministratie);
				$mail->AddCC = $_SESSION['emailingelogde'];
				if (strlen($emailsecretariaat) > 5) {
					$mail->AddCC = $emailsecretariaat;
				}
				$mail->Subject = sprintf("Opzegging lidmaatschap %s per %s", $_SESSION['naamingelogde'], $opgezegdper);
				$mail->Body = $body;
				if ($mail->Send()) {
					db_add_activiteit(sprintf("%s heeft zijn lidmaatschap per %s opgezegd.", $_SESSION['naamingelogde'], $opgezegdper), 6);
					echo("<p class='mededeling'>De opzegging is verzonden.</p>");
				}
				$mail = null;
			} else {
				$form_opzegging = "<form name='Opzegging' action='./?tp=Wijzigen/Opzegging' method='post'>
<table>
<tr><th colspan=2>Opzegging lidmaatschap</th></tr>
<tr><td class='label'>Naam lid:</td><td>[%NAAMLID%]</td></tr>
<input type='hidden' name='NaamLid' value='[%NAAMLID%]'><tr>
<td class='label'>Lidnummer:</td><td>[%LIDNR%]</td></tr>
<input type='hidden' name='NaamLid' value='[%LIDNR%]'>
<td class='label'>Opzegging lidmaatschap per:</td><td><input type='text' name='OpzeggingPer' value='[%OPZEGGENVANAF%]' maxlength=18 size=20></td></tr>
<td class='label'>Reden en/of opmerkingen:</td><td><textarea name='RedenOpmerking' rows=8 cols=60></textarea></td></tr>
<tr><th colspan=2><input type='submit' name='VerstuurOpzegging' value='Verstuur opzegging'></th></tr>
</table>
</form>";
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
			
		} elseif ($currenttab2 == "Profiel" and toegang($_GET['tp'])) {
		
			if ($_SERVER['REQUEST_METHOD'] == "POST") {
				if (strlen($_POST['pw_nieuw']) < 6) {
					$mess = "Het nieuwe wachtwoord is te kort, het moet minimaal 6 karakters lang zijn.";
				} elseif ($_POST['pw_nieuw'] !== $_POST['pw_herhaal']) {
					$mess = "Nieuwe wachtwoorden zijn niet gelijk.";
				} else {
					$mess = db_change_password($_POST['pw_nieuw'], $_POST['pw_oud']);
				}
				printf("<p class='mededeling'>%s</p>", $mess);
				echo("<p><a href='/'>Klik hier om verder te gaan.</a></p>\n");
			} else {
				echo("<div id='wijzigengegevens'>\n");
				printf("<form name='ProfielWijzigen' action=\"%s?%s\" method='post'>", $_SERVER["PHP_SELF"], $_SERVER['QUERY_STRING']);
				echo("<table>
						<tr><th colspan=2>Wachtwoord wijzigen</th></tr>
						<tr><td class=label>Oude wachtwoord:</td><td><input type='password' name='pw_oud' size=10 maxlength=12></td></tr>
						<tr><td class=label>Nieuw wachtwoord:</td><td><input type='password' name='pw_nieuw' size=10 maxlength=12></td></tr>
						<tr><td class=label>Herhaal wachtwoord:</td><td><input type='password' name='pw_herhaal' size=10 maxlength=12></td></tr>
						<tr><th colspan=2><input type='submit' value='Wijzigen'>&nbsp;<input type=button onClick='history.go(-1);' value='Annuleren'></th></tr>
					</table>
				  </form>
				</div>  <!-- Einde invulformulier -->");
				
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
				echo(fnDiplayForm($rows[0], $xtra));
			}
		} else {
			echo("<p class='mededeling'>Je hebt geen toegang.</p>\n");
		}
	} else {
		echo("<p class='mededeling'>Er is geen lid geselecteerd.</p>\n4");
	}
}

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
	
	echo("<div id='bewakingsrooster'>\n");
	$st_len_bp = 1;
	
	echo("<div id='filter'>\n");
	printf("<form method='post' action='%s?%s'>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	echo("Bewakingseizoen: ");
	echo("<select name='seizoen' onchange='form.submit();'>\n");
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
	echo("</select>\n");
	echo("&nbsp;&nbsp;");
	
	if ($currenttab2 != "Aantallen") {
		echo("Bewakingsweek: ");
		echo("<select name='week' onchange='form.submit();'>\n");
		print("<option value='-1'>Alle</option>\n");
		if ($currenttab2 == "Overzicht inschrijvingen") {
			$rows = db_insbew("blokken", 0, 0, 0, "", $seizoen);
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
		echo("</select>\n");
	}
	echo("</form>\n");
	echo("</div>  <!-- Einde filter -->\n");

	if ($currenttab2 == "Bewakingsrooster" and toegang($_GET['tp'])) {
		$rooster = db_bewakingsrooster($seizoen, $week);
		$wk = "";
		$aantalbewdag = 0;
		$aantalond = db_Onderdelen("bw", 1);
		if ($aantalond > 1) {
			$bastotreg = "<tr>\n<td colspan='3'><b>Totaal: %d bewakers</b></td>\n<td class='number'><b>%s</b></td>\n<td>&nbsp;</td>\n</tr>\n";
		} else {
			$bastotreg = "<tr>\n<td colspan='3'><b>Totaal: %d bewakers</b></td>\n<td class='number'><b>%s</b></td>\n</tr>\n";
		}
		print("<table>\n");
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
	} elseif ($currenttab2 == "Postindeling" and toegang($_GET['tp'])) {
		$lijst = db_bewakingsrooster($seizoen, $week);
		
		$vw = -1;
		$vp = "";
		
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
		
	} elseif ($currenttab2 == "Aantallen" and toegang($_GET['tp'])) {
		$lijst = db_bewaking_aantallen($seizoen);
		echo(fnDiplayTable($lijst));

	} elseif ($currenttab2 == "Overzicht inschrijvingen" and toegang($_GET['tp'])) {
		$lijst = db_insbew("overzicht", 0, 0, 0, "", $seizoen, $week);
		echo(fnDiplayTable($lijst));
	}
	echo("</div>  <!-- Einde bewakingsrooster -->\n");
}

function fnLoginAanvragen() {
	global $naamvereniging, $urlwebsite, $beperktotgroep, $table_prefix, $lidnrversturenmogelijk;
	
	if (!isset($lidnrversturenmogelijk) or $lidnrversturenmogelijk != 1) {
		$lidnrversturenmogelijk = 0;
	}
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		if (is_numeric($_POST['Lidnummer']) and $_POST['Lidnummer'] > 0) {
			$query = sprintf("SELECT L.Login FROM %1\$sLidmaatschap AS LM INNER JOIN %1\$sAdmin_login AS L ON LM.Lid = L.LidID WHERE LM.Lidnr=%2\$d;"
					, $table_prefix, $_POST['Lidnummer']);
			$result = fnQuery($query);
			$login = $result->fetchColumn();
			if (strlen($login) > 0 and $_POST['login'] != $login) {
				if (strlen($_POST['login']) > 0) {
					printf("<p class='mededeling'>Lidnummer %d was al gekoppeld aan een login. Er wordt geen nieuwe login aangemaakt.</p>\n", $_POST['Lidnummer']);
				}
				$_POST['login'] = $login;
			}
		} elseif (strlen($_POST['login']) > 0) {
			$query = sprintf("SELECT LM.Lidnr FROM %1\$sLidmaatschap AS LM INNER JOIN %1\$sAdmin_login AS L ON LM.Lid = L.LidID" 
					 . " WHERE L.Login='%2\$s' ORDER BY LM.LIDDATUM DESC;", $table_prefix, $_POST['login']);
			$result = fnQuery($query);
			$lidnr = $result->fetchColumn();
			if ($lidnr > 0 and $_POST['Lidnummer'] != $lidnr) {
				$_POST['Lidnummer'] = $lidnr;
			}
		}

		$mess = "";
		if (strlen($_POST['email']) < 5 or !isValidMailAddress($_POST['email'])) {
			$mess = sprintf("Je hebt geen geldig e-mailadres (%s) opgegeven.", $_POST['email']);
		} elseif ((strlen($_POST['Lidnummer']) == 0 or $_POST['Lidnummer'] == 0) and $lidnrversturenmogelijk == 1) {
			$row = db_login_bevestiging(0, $_POST['email']);
			if ($row == false) {
				$mess = sprintf("Het emailadres '%s' is onbekend in de database.", $_POST['email']);
			} else {
				fnConfirmLidnr($row);
			}
		} elseif (strlen($_POST['Lidnummer']) == 0 or !is_numeric($_POST['Lidnummer'])) {
			$mess = "Je hebt geen (geldig) lidnummer opgegeven.";
		} elseif (strlen($_POST['login']) < 6 or strlen($_POST['login']) > 15) {
			$mess = "Je hebt geen geldige login opgegeven. Een login moet uit minimaal 6 en mag maximaal uit 15 karakters bestaan.";
		} else {
			$query = sprintf("SELECT L.Nummer FROM %1\$sLid AS L INNER JOIN %1\$sLidmaatschap AS LM ON L.Nummer = LM.Lid"
					 . " WHERE LM.Lidnr=%2\$d AND (LOWER(L.EMAIL)='%3\$s' OR LOWER(L.EmailVereniging)='%3\$s');", $table_prefix, $_POST['Lidnummer'], strtolower($_POST['email']));
			$result = fnQuery($query);
			$nummer = $result->fetchColumn();
			if ($nummer > 0) {
				$mess = db_add_login($_POST['login'], $nummer);
			} else {
				$mess = sprintf("De combinatie lidnummer (%d) en e-mailadres (%s) is niet bekend in onze database.", $_POST['Lidnummer'], $_POST['email']);
			}
		}
		printf("<p class='mededeling'>%s</p>\n", $mess);
		printf("<p><a href='http://%s' target='_top'>Klik hier om verder te gaan.</a></p>\n", $urlwebsite);

	} else {

		echo("<div id='invulformulier'>\n");
		printf("<form name=Loginaanvraag action='%s?%s' method='post'>", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
		echo("<table>
				<tr>
				<th colspan=2>Aanvragen login</th>
				</tr>
				<td class=label>Gewenste login:</td>
				<td><input type='text' name='login' size=15 maxlength=15> (alleen nodig bij het aanvragen van een nieuwe login)</td>
				</tr>
				<tr>
				<td class=label>Lidnummer:</td>
				<td><input type='number' name='Lidnummer' size=5></td>
				</tr>
				<tr>
				<td class=label>E-mailadres:</td>
				<td><input type='email' name='email' size=40></td>
				</tr>
				<tr>
				<td colspan=2><p>");
		if (isset($beperktotgroep) and $beperktotgroep > 0) {
			printf("Alleen leden van de groep '%s' van %s kunnen een login aanvragen. ", db_naam_onderdeel($beperktotgroep), $naamvereniging);
		} else {
			printf("Alleen leden van de %s kunnen een login aanvragen.");
		}
		echo("Je kan dit formulier ook gebruiken als je je login of wachtwoord vergeten bent.");
		if ($lidnrversturenmogelijk == 1) {
			echo(" Je lidnummer kan je opvragen door alleen je e-mailadres in te vullen.");
		}
		echo("</p>
				</td>
				</tr>
				<tr>
				<th colspan=2>
					<input type=submit name=aanmelden value=Aanvragen>&nbsp;
					<input type=button onClick='history.go(-1);' value='Annuleren'>
				</th>
				</tr>
			</table>
		  </form>
		</div>  <!-- Einde invulformulier -->");
	}
}

function fnKostenoverzicht() {
	global $table_prefix;

	$val_jaarfilter = "";
	$val_gbrfilter = "";
	$val_kplfilter = "";
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['lbJaarFilter'])) {
			$val_jaarfilter = $_POST['lbJaarFilter'];
		}
		if (isset($_POST['lbGBRFilter'])) {
			$val_gbrfilter = $_POST['lbGBRFilter'];
		}
		if (isset($_POST['lbKPLFilter'])) {
			$val_kplfilter = $_POST['lbKPLFilter'];
		}
	}
	
	echo("<div id='filter'>\n");
	printf("<form name='Filter' action='%s?%s' method='post'>", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
	echo("<table>\n");
	echo("<tr>\n");
	$ret = "";
	$query = "SELECT RecordID, Kode AS Jaar FROM Boekjaar ORDER BY Begindatum DESC;";
	$result = fnQuery($query);
	foreach ($result->fetchAll() as $row) {
		if ($val_jaarfilter == $row->RecordID or strlen($val_jaarfilter) == 0) {
			$s = " selected";
			$val_jaarfilter = $row->RecordID;
		} else {
			$s = "";
		}
		$ret .= sprintf('<option%s value="%2$s">%3$s</option>\n', $s, $row->RecordID, $row->Jaar);
	}
	printf("<td class='label'>Boekjaar</td><td><select name='lbJaarFilter' onchange='form.submit();'>%s</select></td>\n", $ret);
	
	$ret = "<option value='*'>Alle</option>\n";
	$query = sprintf("SELECT DISTINCT GBR.Kode, CONCAT(GBR.Kode, ' - ', GBR.OMSCHRIJV) AS Oms
				 FROM %1\$sMutatie AS M INNER JOIN %1\$sGBR AS GBR ON M.GBR = GBR.Kode
				 ORDER BY GBR.Kode;", $table_prefix);
	$result = fnQuery($query);
	foreach ($result->fetchAll() as $row) {
		if ($val_gbrfilter == $row->Kode) {
			$s = " selected";
		} else {
			$s = "";
		}
		$ret .= sprintf('<option%s value="%s">%s</option>\n', $s, $row->Kode, $row->Oms);
	}
	printf("<td class='label'>Grootboekrekening</td><td><select name='lbGBRFilter' onchange='form.submit();'>%s</select></td>\n", $ret);
	
	$ret = "<option value='*'>Alle</option>\n";
	$query = "SELECT DISTINCT KSTNPLTS AS Kode FROM Mutatie WHERE KostenplaatsID > 0 ORDER BY KSTNPLTS;";
	$result = fnQuery($query);
	foreach ($result->fetchAll() as $row) {
		if ($val_kplfilter == $row->Kode) {
			$s = " selected";
		} else {
			$s = "";
		}
		$ret .= sprintf('<option%s value="%s">%s</option>\n', $s, $row->Kode, $row->Kode);
	}
	printf("<td class='label'>Kostenplaats</td><td><select name='lbKPLFilter' onchange='form.submit();'>%s</select></td>\n", $ret);
	
	echo("</tr>\n");
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde filter -->\n");

	echo(fnDiplayTable(db_mutatie($val_jaarfilter, $val_gbrfilter, $val_kplfilter), "", "", 1));
}

function fnMailing() {
	global $ldl, $currenttab2;
		
	if (isset($_POST['Upload']) and $_POST['Upload'] == "Upload") {
		$op = "upload";
	} elseif (isset($_POST['action']) and $_POST['action'] == "Bewaren & sluiten") {
		$op = "save_close";
	} elseif (isset($_POST['action']) and $_POST['action'] == "Voorbeeld") {
		$op = "preview";
	} elseif (isset($_GET['op'])) {
		$op = $_GET['op'];
	} else {
		$op = "";
	}
	if ($op != "preview") {
		fnDispMenu(2);
	}

	$mailing = new mailing;
	if (isset($_GET['mid']) and $_GET['mid'] >= 0) {
		$mailing->mid = $_GET['mid'];
	} else {
		$mailing->mid = 0;
	}

	if ($op == "edit" and isset($_GET['mid'])) {
		$mailing->edit();
	} elseif ($op == "post" or $op == "save_close") {
		$mailing->post_form();
		$mailing->upload();
		if ($op == "save_close") {
			printf("<script>location.href='%s?tp=Mailing/%s';</script>\n", $_SERVER['PHP_SELF'], db_mailing("folder", $mailing->mid));
		} else {
			$mailing->edit();
		}
	} elseif ($currenttab2 == "Nieuw" and toegang($_GET['tp'])) {
		$mailing->mid = 0;
		$mailing->edit();
	} elseif ($op == "historie" and $mailing->mid > 0) {
		$rows = db_mailing("hist", $mailing->mid);
		if (count($rows) > 0) {
			$ld = sprintf("<a href='index.php?tp=%s&amp;op=histdetails&amp;rid=%%d'>%%s</a>", $_GET['tp']);
			echo(fnDiplayTable($rows, $ld, $rows[0]->ndOnderwerp, 0, 1));
		} else {
			echo("<p class='mededeling'>Deze mailing heeft geen historie.</p>\n");
		}
		echo("<div id='opdrachtknoppen'>\n
				<img src='images/back.png' alt='Terug' title='Terug' onclick='history.go(-1);'>\n
				</div>  <!-- Einde opdrachtknoppen -->\n");
	} elseif ($op == "histdetails" and $_GET['rid'] > 0) {
		echo(fnDiplayForm(db_mailing("histdetails", 0, $_GET['rid'])));
		echo("<div id='opdrachtknoppen'>\n
				<img src='images/back.png' alt='Terug' title='Terug' onclick='history.go(-1);'>\n
				</div>  <!-- Einde opdrachtknoppen -->\n");
	} elseif ($op == "upload") {
		$mailing->upload();
		$mailing->edit();
	} elseif ($op == "del_attach" and isset($_GET['attach'])) {
		$mailing->attach_delete($_GET['attach']);
		$mailing->edit();
	} elseif ($op == "add_lid" and isset($_GET['lidid']) and $_GET['lidid'] > 0) {
		$mailing->Add_lid($_GET['lidid']);
		$mailing->edit();
	} elseif ($op == "add_lid" and isset($_GET['to_address']) and strlen($_GET['to_address']) > 0) {
		$mailing->Add_lid(0, $_GET['to_address']);
		$mailing->edit();
	} elseif ($op == "del_lid" and isset($_GET['lidid']) and $_GET['lidid'] > 0) {
		$mailing->delete_lid($_GET['lidid']);
		$mailing->edit();
	} elseif ($op == "del_lid" and isset($_GET['addr']) and strlen($_GET['addr']) > 0) {
		$mailing->delete_lid(0, $_GET['addr']);
		$mailing->edit();
	} elseif ($op == "add_groep" and isset($_GET['groepid']) and $_GET['groepid'] > 0) {
		$mailing->add_groep($_GET['groepid']);
		$mailing->edit();
	} elseif ($op == "del_groep" and isset($_GET['groepid']) and $_GET['groepid'] != 0) {
		$mailing->delete_groep($_GET['groepid']);
		$mailing->edit();
	} elseif ($op == "preview") {
		$mailing->upload();
		$mailing->preview();
	} elseif ($op == "send") {
		$mailing->upload();
		$mailing->send();
		$mailing->lijst($currenttab2);
	} elseif ($op == "delete") {
		$mailing->delete();
		$mailing->lijst($currenttab2);
	} elseif ($op == "undelete") {
		$mailing->undelete();
		$mailing->lijst($currenttab2);
	} else {
		$mailing->lijst($currenttab2);
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

function fotolid($lidid) {
	global $pasfotoextenties;

	$fn = "";
	foreach($pasfotoextenties as $ext) {
		if (file_exists(sprintf("pasfoto/Pasfoto%d.%s", $lidid, $ext))) {
			$fn = sprintf("pasfoto/Pasfoto%d.%s", $lidid, $ext);
		} elseif (file_exists(sprintf("pasfoto/pasfoto%d.%s", $lidid, $ext))) {
			$fn = sprintf("pasfoto/pasfoto%d.%s", $lidid, $ext);
		} elseif (file_exists(sprintf("Pasfoto/Pasfoto%d.%s", $lidid, $ext))) {
			$fn = sprintf("Pasfoto/Pasfoto%d.%s", $lidid, $ext);
		}
		
	}
	return $fn;
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
