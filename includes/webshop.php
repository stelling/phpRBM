<?php

function fnWebshop() {
	global $currenttab, $currenttab2;
	
	fnDispMenu(2);

	if ($currenttab2 == "Artikelbeheer" and toegang($_GET['tp'], 1, 1)) {
		artikelbeheer();
	} elseif ($currenttab2 == "Beheer" and toegang($_GET['tp'], 1, 1)) {
		bestellingbeheer();
	} elseif ($currenttab2 == "Voorraadbeheer" and toegang($_GET['tp'], 1, 1)) {
		voorraadbeheer();
	} elseif ($currenttab2 == "Logboek" and toegang($_GET['tp'], 1, 1)) {
		$lijst = (new cls_Logboek())->lijst(10, 1);
		echo(fnDisplayTable($lijst, fnStandaardKols("logboek"), "", 0, "", "", "logboek"));
	}
}

function winkelwagen($lidid) {
	
	// Bestellingen door het lid via de zelfservice

	fnDispMenu(2);
	
	echo("<div id='webshop'>\n");
	echo("<div id='winkelwagen'>\n");
	
	$i_or = new cls_Orderregel($lidid);
	$i_art = new cls_Artikel();
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['btnToevOrderregel']) and isset($_POST['Nieuw_OR']) and $_POST['Nieuw_OR'] > 0) {
			$i_or->add($lidid, $_POST['Nieuw_OR']);
		}
		
		$f = sprintf("Ord.Lid=%d AND Ord.Ordernr=0", $lidid);
		foreach ($i_or->lijst($f) as $ord) {
			$pnab = sprintf("AantalBesteld_%d", $ord->RecordID);
			$pnopm = sprintf("Opmerking_%d", $ord->RecordID);
			
			if (isset($_POST[$pnab])) {
				$i_or->update($ord->RecordID, "AantalBesteld", $_POST[$pnab]);
			}
			if (isset($_POST[$pnopm])) {
				$i_or->update($ord->RecordID, "Opmerking", $_POST[$pnopm]);
			}
		}
		
		if (isset($_POST['defmaken'])) {
			$ordernr = $i_or->definitiefmaken($lidid);
			if ($ordernr == 0) {
				$mess = "Er ging iets mis bij het defintief maken van de order. Neem contact op met de webmaster.";
			} elseif ($_SESSION['settings']['mailing_bevestigingbestelling'] > 0) {
				$mailing = new Mailing($_SESSION['settings']['mailing_bevestigingbestelling']);
				$mailing->xtranum = $ordernr;
				if ($mailing->send($lidid) > 0) {
					$mess = sprintf("Bevestiging bestelling %d is verzonden.", $ordernr);
				} else {
					$mess = sprintf("Fout bij het versturen van de bevestiging. Probeer het later nogmaals of neem contact op met de webmaster.");
				}
				$mailing = null;
			} else {
				$mess = "Er is geen mailing voor het bevestigingen van de bestelling beschikbaar.";
			}
			(new cls_Logboek())->add($mess, 10, $lidid, 0, $ordernr);
			printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		}
	}

	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	printf("<table class='%s'>\n", TABLECLASSES);
	echo("<caption>Winkelwagen</caption>\n");
	echo("<thead>\n<tr><th>Artikel</th><th>Prijs per stuk</th><th>Aantal</th><th>Bedrag</th><th>Beschikbaar</th><th>Opmerking</th></tr></thead>\n");
	$totaant = 0;
	$totbedrag = 0;
	$defmaken = false;
	echo("<tbody>\n");
	foreach ($i_or->winkelwagen($lidid) as $ord) {
		$i_or->vulvars($ord->RecordID);
		echo("<tr>\n");
		printf("<td>%s</td><td class='number'>&euro;&nbsp;%s</td>\n", $i_or->i_art->codeomsmaat, number_format($i_or->prijsperstuk, 2));
		printf("<td class='number'><input class='num2' type='number' name='AantalBesteld_%d' min=0 max=%d value=%d onChange='this.form.submit();'></td>\n", $ord->RecordID, $i_or->i_art->maxperlid, $i_or->aantalbesteld);
		printf("<td class='number'>&euro;&nbsp;%s</td>\n", number_format($i_or->bedrag, 2));
		printf("<td class='number'>%s</td>\n", number_format($i_or->i_art->vrijevoorraad, 0));
		printf("<td><input class='w30' type='text' name='Opmerking_%d' value='%s' maxlength=30 OnChange='this.form.submit();'></td>\n", $ord->RecordID, $i_or->opmerking);
		echo("</tr>\n");
		$totaant += $i_or->aantalbesteld;
		$totbedrag += $i_or->bedrag;
		if ($ord->AantalBesteld > 0) {
			$defmaken = true;
		}
	}
	echo("</tbody>\n");
	
	if ($totbedrag <> 0 or $totaant <> 0) {
		printf("<tr><td colspan=2>Totaal winkelwagen</td><td class='number'>%d</td><td class='number'>&euro;&nbsp;%s</td><td colspan=2></td></tr>\n", $totaant, number_format($totbedrag, 2));
	}
	
	echo("</table>\n");
//	echo("<div id='opdrachtknoppen'>\n");
	
	$opt = $i_art->htmloptions(-1, "bestelbaar");
	if (strlen($opt) > 0) {
		printf("<select class='form-select form-select-sm' name='Nieuw_OR'>%s</select>\n", $opt);
		printf("<button class='%s btn-sm' type='submit' name='btnToevOrderregel'>%s</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
	}

	if ($defmaken) {
		printf("<button class='%s' name='defmaken'>Definitief bestellen</button>\n", CLASSBUTTON);	
	}
	if (strlen($_SESSION['settings']['zs_voorwaardenbestelling']) > 0) {
		printf("<p>%s</p>\n", $_SESSION['settings']['zs_voorwaardenbestelling']);
	}
//	echo("</div>  <!-- Einde opdrachtknoppen -->");
	echo("</form>\n");
	echo("<div class='clear'></div>\n");
	
	$f = sprintf("Ord.Lid=%d AND LENGTH(IFNULL(Ord.BestellingDefinitief, '')) >= 10 AND Ord.AantalBesteld <> 0", $lidid);
	$rows = $i_or->lijst($f, 0, "Ord.BestellingDefinitief, Ord.Artikel");
	
	if (count($rows) > 0) {
		$kols[] = array('headertext' => "Order", 'columnname' => "Ordernr");
		$kols[] = array('headertext' => "Artikel", 'columnname' => "CodeOmsMaat");
		$kols[] = array('headertext' => "Aantal besteld", 'columnname' => "AantalBesteld", 'type' => "integer");
		$kols[] = array('headertext' => "Datum besteld", 'columnname' => "BestellingDefinitief", 'type' => "DTTEXT");
		$kols[] = array('headertext' => "Aantal geleverd", 'columnname' => "AantalGeleverd", 'type' => "integer");
		
		echo(fnDisplayTable($rows, $kols, "Eerdere bestellingen", 0, "", "bestellingenperlid"));
	}
	
	echo("</div>   <!-- Einde winkelwagen -->\n");
	echo("</div>   <!-- Einde webshop -->\n");
	
}  # winkelwagen

function artikelbeheer() {

	$i_art = new cls_Artikel();
	if (isset($_GET['op']) and $_GET['op'] == "delete" and $_GET['aid'] > 0) {
		$i_art->delete($_GET['aid']);
	}
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['Code_Nw'])) {
			$i_art->add();
		}
		$lijst = $i_art->lijst("beheer");
		foreach ($lijst as $row) {
			foreach ($row as $col => $val) {
				$pvn = sprintf("%s_%d", $col, $row->RecordID);
				if ($col == "Verkoopprijs" and isset($_POST[$pvn])) {
					$_POST[$pvn] = str_replace(",",".", $_POST[$pvn]);
					if (!is_numeric($_POST[$pvn])) {
//						$_POST[$pvn] = 0;
					}
				} elseif ($col == "BeschikbaarTot" and isset($_POST[$pvn])) {
					if ($_POST[$pvn] > $_POST[sprintf("VervallenPer_%d", $row->RecordID)]) {
						$_POST[$pvn] = $_POST[sprintf("VervallenPer_%d", $row->RecordID)];
					}
				}
				if (isset($_POST[$pvn])) {
					$i_art->update($row->RecordID, $col, $_POST[$pvn]);
				}
			}
		}
	}
	echo("<div id='webshop'>\n");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	printf("<table class='%s'>\n", TABLECLASSES);
	echo("<tr><th>Code</th><th>Omschrijving</th><th>Maat</th><th>Verkoopprijs</th><th>Beschikbaar tot</th><th>Vervallen per</th><th>Max per lid</th><th>Beschikbaar voor</th><th></th></tr>\n");
	$lijst = $i_art->lijst("beheer");
	foreach ($lijst as $row) {
		$i_art->vulvars($row->RecordID);
		echo("<tr>\n");
		printf("<td><input type='text' class='w8' name='Code_%d' value='%s' maxlength=8></td>\n", $row->RecordID, $i_art->code);
		printf("<td><input type='text' class='w50' name='Omschrijving_%d' value='%s' maxlength=50></td>\n", $row->RecordID, $i_art->omschrijving);
			
		$options = "";
		foreach(ARRKLEDINGMAAT as $m) {
			$options .= sprintf("<option value='%s' %s>%s</option>\n", $m, checked($m, "option", $row->Maat), $m);
		}
		printf("<td><select class='form-select form-select-sm' name='Maat_%d'>%s</select></td>\n", $row->RecordID, $options);
		printf("<td><input type='text' name='Verkoopprijs_%d' value='%s' class='d8'></td>\n", $row->RecordID, $row->Verkoopprijs);
		printf("<td><input type='date' name='BeschikbaarTot_%d' value='%s'></td>\n", $row->RecordID, $row->BeschikbaarTot);
		printf("<td><input type='date' name='VervallenPer_%d' value='%s'></td>\n", $row->RecordID, $row->VervallenPer);
		printf("<td><input type='number' name='MaxAantalPerLid_%d' value=%d max=999 class='num3'></td>\n", $row->RecordID, $row->MaxAantalPerLid);
		
		printf("<td><select class='form-select form-select-sm' name='BeperkTotGroep_%d'>\n", $row->RecordID);
		echo("<option value=-2>Niemand</option>\n");
		foreach((new cls_Onderdeel())->lijst(1) as $ond) {
			printf("<option value=%d%s>%s</option>\n", $ond->RecordID, checked($ond->RecordID, "option", $row->BeperkTotGroep), $ond->Naam);
		}
		echo("</select></td>\n");
		if ($row->InGebruik == 0) {
			$lnk = sprintf("%s?tp=%s&op=delete&aid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
			printf("<td><a href='%s'>%s</a></td>", $lnk, ICONVERWIJDER);
		} else {
			echo("<td></td>\n");
		}
		
		echo("</tr>\n");
	}
	echo("</table>\n");
	echo("<div id='opdrachtknoppen'>\n");
	printf("<button type='submit' class='%s' name='Code_Nw'>%s artikel</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
	if (count($lijst) > 0) {
		printf("<button type='submit' class='%s' name='action' value='save' title='Bewaren'>%s Bewaren</button>\n", CLASSBUTTON, ICONBEWAAR);
	}
	echo("</div> <!-- Einde opdrachtknoppen -->\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde artikelbeheer -->\n");
	
}  # artikelbeheer

function bestellingbeheer() {
	global $dtfmt;	

	$naamfilter="";
	$i_or = new cls_Orderregel();
	$i_vb = new cls_Voorraadboeking();
	$i_art = new cls_Artikel();
	$dtfmt->setPattern(DTTEXT);
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['Lid_0']) and is_numeric($_POST['Lid_0']) and $_POST['Lid_0'] > 0) {
			$i_or->add($_POST['Lid_0'], 0, 1);
		}
		foreach($i_or->lijst("Ordernr > 0") as $row) {
			$pnag = sprintf("AantalGeleverd_%d", $row->RecordID);
			$f = sprintf("OrderregelID=%d", $row->RecordID);
			$vbid = $i_vb->max("RecordID", $f);
			if ($vbid == 0 and isset($_POST[$pnag]) and intval($_POST[$pnag]) != 0) {
				$vbid = $i_vb->add($row->Artikel, $row->RecordID);
			} elseif ($vbid > 0 and isset($_POST[$pnag]) and intval($_POST[$pnag]) == 0) {
				$i_vb->delete($vbid);
			} else {
				$vbid = 0;
			}
			$i_or->lidid = $row->Lid;
			$pnart = sprintf("Artikel_%d", $row->RecordID);
			$pnab = sprintf("AantalBesteld_%d", $row->RecordID);
			$pnpps = sprintf("PrijsPerStuk_%d", $row->RecordID);
			if (isset($_POST[$pnab]) and strlen($_POST[$pnab]) > 0 and is_numeric($_POST[$pnab])) {
				$_POST[$pnab] = (int)$_POST[$pnab];
			} else {
				$_POST[$pnab] = 0;
			}
			if (isset($_POST[$pnpps]) and strlen($_POST[$pnpps]) > 0) {
				$_POST[$pnpps] = str_replace(",",".", $_POST[$pnpps]);
			} else {
				$_POST[$pnpps] = 0;
			}
			if (isset($_POST[$pnag]) and strlen($_POST[$pnag]) > 0 and is_numeric($_POST[$pnag])) {
				$_POST[$pnag] = (int)$_POST[$pnag];
			} else {
				$_POST[$pnag] = 0;
			}
			if (isset($_POST[$pnart]) and $_POST[$pnart] != $row->Artikel) {
				$i_or->update($row->RecordID, "Artikel", $_POST[$pnart]);
			}
			if (isset($_POST[$pnab]) and $_POST[$pnab] != $row->AantalBesteld and $row->Ordernr == 0) {
				$i_or->update($row->RecordID, "AantalBesteld", $_POST[$pnab]);
			}
			if ($_POST[$pnpps] != $row->PrijsPerStuk) {
				$i_or->update($row->RecordID, "PrijsPerStuk", $_POST[$pnpps]);
			}
			if ($_POST[$pnag] != $row->AantalGeleverd) {
				$w = intval($_POST[$pnag]) * -1;
				$i_vb->update($vbid, "Aantal", $w);
			}
		}
		$i_or->vulprijsperstuk();
		if (isset($_POST['tbNaamFilter']) and strlen($_POST['tbNaamFilter']) > 0) {
			$naamfilter = $_POST['tbNaamFilter'];
		}	
	} elseif (isset($_GET['op']) and $_GET['op'] == "delete" and $_GET['rid'] > 0) {
		(new cls_Orderregel())->delete($_GET['rid']);
	}
	
	echo("<div id='webshop'>\n");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<div id='filter'>\n");
	printf("<label>Naam bevat</label><input type='text' name='tbNaamFilter' size=30 value='%s' placeholder='Achter- of roepnaam' onblur='this.form.submit();'>\n", $naamfilter);
	echo("</div>  <!-- Einde filter -->\n");
	printf("<table class='%s'>\n", TABLECLASSES);
	echo("<thead>\n<tr><th>Ordernr</th><th>Lid</th><th>Artikel</th><th>Besteld</th><th>Prijs per stuk</th><th>Datum besteld</th><th>Bedrag</th><th>Geleverd</th><th>Opmerking bij bestellen</th><th></th></tr>\n</thead>\n");
	$nrmag = true;
	$f = "Ordernr > 0";
	if (strlen($naamfilter) > 0) {
		$f .= sprintf(" AND (L.Achternaam LIKE '%1\$s%%' OR L.Roepnaam LIKE '%1\$s%%' OR L.Meisjesnm LIKE '%1\$s%%')", $naamfilter);
	}
	$lijst = $i_or->lijst($f);
	$arthtmloptions = $i_art->htmloptions();
	foreach($lijst as $row) {
		$i_or->vulvars($row->RecordID);
		echo("<tr>\n");
		printf("<td class='number'>%d</td>", $row->Ordernr);
		printf("<td>%s</td>", $row->NaamLid);
		if ($i_or->artid > 0) {
			printf("<td>%s</td>", $i_or->i_art->omsmaat);
			printf("<td><input type='number' name='AantalBesteld_%d'class='num3' max=999 value=%d onBlur='form.submit();'></td>\n", $row->RecordID, $i_or->aantalbesteld);
		} else {
			printf("<td><select name='Artikel_%d' onChange='this.form.submit();'>\n", $row->RecordID);
			printf("<option value=0>Selecteer artikel</option>\n%s</select>\n", $arthtmloptions);
			echo("<td></td>");
		}
		
		if ($i_or->artid > 0) {
			printf("<td><input type='text' class='d8' name='PrijsPerStuk_%d' value='%s' maxlength=8></td>\n", $row->RecordID, number_format($i_or->prijsperstuk, 2));
		} else {
			echo("<td></td>\n");
		}
		
		if (strlen($i_or->bestellingdefinitief) >= 10) {
			printf("<td>%s</td>\n", $dtfmt->format(strtotime($i_or->bestellingdefinitief)));
		} else {
			echo("<td></td>\n");
		}
		printf("<td class='number'>&euro;&nbsp;%s</td>", number_format($i_or->bedrag, 2));
		if ($row->Artikel > 0 and $row->Lid > 0) {
			printf("<td><input type='number' name='AantalGeleverd_%d' class='num3' max=999 value=%d></td>\n", $row->RecordID, $i_or->aantalgeleverd);
		} else {
			echo("<td></td>\n");
		}
		printf("<td>%s</td>", $row->Opmerking);
		if ($i_or->aantalgeleverd == 0) {
			$lnk = sprintf("%s?tp=%s&op=delete&rid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
			printf("<td><a href='%s'>%s</i></a></td>", $lnk, ICONVERWIJDER);
		}
		echo("</tr>\n");
		if ($i_or->lidid == 0 or $i_or->artid == 0) {
			$nrmag = false;
		}
	}
	echo("</table>\n");
	
	echo("<div id='opdrachtknoppen'>\n");
	
	if ($nrmag) {
		$opt = $i_or->i_lid->htmloptions(-1, 8);
		if (strlen($opt) > 0) {
			echo("<select class='form-select' name='Lid_0' onChange='this.form.submit();'>\n");
			printf("<option value=0>Nieuwe regel ...</option>\n%s</select>\n", $opt);
		}
	}
	
	printf("<button type='submit' class='%s' name='action' value='save' title='Bewaren'>%s Bewaren</button>\n", CLASSBUTTON, ICONBEWAAR);
	echo("</div>  <!-- Einde opdrachtknoppen -->\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde bestellingbeheer -->\n");
	
	echo("<div class='clear' style='height: 30px;'></div>\n");
	
	$tots = $i_or->totalen();
	if (count($tots) > 0) {
		echo(fnDisplayTable($tots, null, "Totalen", 99, "", "", "totalenbestellingen"));
	}
	$i_or = null;
	
}  # bestellingbeheer

function Voorraadbeheer() {
	
	$i_art = new cls_Artikel();
	$i_vb = new cls_Voorraadboeking();
	
	echo("<div id='webshop'>\n");
	echo("<div id='voorraadbeheer'>\n");
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['Nieuw_vb']) and $_POST['Nieuw_vb'] > 0) {
			$i_vb->add($_POST['Nieuw_vb']);
		}
		$vbrows = $i_vb->lijst();
		foreach ($vbrows as $vbrow) {
			foreach (array("Omschrijving", "Datum", "Aantal") as $fld) {
				$cn = sprintf("%s_%d", $fld, $vbrow->RecordID);
				if (isset($_POST[$cn])) {
					$i_vb->update($vbrow->RecordID, $fld, $_POST[$cn]);
				}
			}
		}
	}
	
	$artrows = $i_art->lijst("voorraadbeheer");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	printf("<table class='%s'>\n", TABLECLASSES);
	echo("<tr><th>Artikel</th><th>Omschrijving boeking</th><th>Datum</th><th>Mutatie</th><th>Voorraad</th></tr>");
	foreach ($artrows as $artrow) {
		$i_art->vulvars($artrow->RecordID);
		printf("<tr><td>%s</td>\n", $i_art->omsmaat);
		$vbrows = $i_vb->lijst($artrow->RecordID);
		$aant = 0;
		$vrrd = 0;
		foreach ($vbrows as $vbrow) {
			$i_vb->vulvars($vbrow->RecordID);
			$vrrd += $i_vb->aantal;
			if ($aant > 0) {
				echo("</tr>\n<tr>\n");
				echo("<td></td>");
			}
			if ($i_vb->orderregelid > 0) {
				$nl = $i_vb->i_or->i_lid->naam;
				if (strlen($nl) > 0) {
					$nl = " (" . $nl . ")";
				}
				if ($i_vb->i_or->ordernr > 0) {
					printf("<td>Levering op order %d%s</td>", $i_vb->i_or->ordernr, $nl);
				} else {
					printf("<td>Levering op orderregel %d%s</td>", $i_vb->orderregelid, $nl);
				}
			} else {
				printf("<td><input type='text' value='%s' name='Omschrijving_%d' OnBlur='this.form.submit();'></td>\n", $vbrow->Omschrijving	, $vbrow->RecordID);
			}
			printf("<td><input type='date' value='%s' name='Datum_%d' OnBlur='this.form.submit();'></td>\n", $vbrow->Datum, $vbrow->RecordID);
			printf("<td><input type='number' value=%d name='Aantal_%d' max=999 OnChange='this.form.submit();' class='num3'></td>\n", $vbrow->Aantal, $vbrow->RecordID);
			printf("<td>%d</td>\n", $vrrd);
			$aant++;
		}
		echo("</tr>\n");
	}
	echo("</table>\n");
	
	echo("<div id='opdrachtknoppen'>\n");
	printf("<select class='form-select' name='Nieuw_vb' OnChange='this.form.submit();'><option value=0>Nieuwe boeking ....</option>%s</select>\n", $i_art->htmloptions());
	echo("</div>  <!-- Einde opdrachtknoppen -->\n");
	echo("</form>\n");
	
	echo("</div> <!-- Einde voorraadbeheer -->\n");
	echo("</div> <!-- Einde webshop -->\n");
}

?>
