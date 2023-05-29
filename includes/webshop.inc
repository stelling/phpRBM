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

function fnWinkelwagen($lidid) {
	
	// Bestellingen door het lid via de zelfservice

	fnDispMenu(2);
	
	echo("<div id='webshop'>\n");
	echo("<div id='winkelwagen'>\n");
	
	$i_or = new cls_Orderregel($lidid);
	$i_art = new cls_Artikel();
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['Nieuw_OR']) and $_POST['Nieuw_OR'] > 0) {
			$i_or->add($lidid, $_POST['Nieuw_OR']);
		}
		
		$f = sprintf("ORD.Lid=%d AND ORD.Ordernr=0", $lidid);
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
			$i_or->definitiefmaken($lidid);
			if ($_SESSION['settings']['mailing_bevestigingbestelling'] > 0) {
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
			(new cls_Logboek())->add($mess, 10, $lidid, $ordernr);
			printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		}
	}

	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	printf("<table class='%s'>\n", TABLECLASSES);
	echo("<caption>Winkelwagen</caption>\n");
	echo("<thead>\n<tr><th>Artikel</th><th>Prijs per stuk</th><th>Aantal</th><th>Bedrag</th><th>Voorraad</th><th>Opmerking</th></tr></thead>\n");
	$totaant = 0;
	$totbedrag = 0;
	$defmaken = false;
	echo("<tbody>\n");
	foreach ($i_or->winkelwagen($lidid) as $ord) {
		echo("<tr>\n");
		printf("<td>%s</td><td class='number'>&euro;&nbsp;%s</td>\n", $ord->CodeOmsMaat, number_format($ord->PrijsPerStuk, 2));
		printf("<td class='number'><input type='number' name='AantalBesteld_%d' min=0 max=%d value=%d onChange='this.form.submit();'></td>\n", $ord->RecordID, $ord->MaxAantalPerLid, $ord->AantalBesteld);
		printf("<td class='number'>&euro;&nbsp;%s</td>\n", number_format($ord->PrijsPerStuk*$ord->AantalBesteld, 2));
		printf("<td class='number'>%d</td>\n", $ord->Voorraad);
		printf("<td><input type='text' name='Opmerking_%d' value='%s' maxlength=30 OnChange='this.form.submit();'></td>\n", $ord->RecordID, $ord->Opmerking);
		echo("</tr>\n");
		$totaant += $ord->AantalBesteld;
		$totbedrag += ($ord->PrijsPerStuk * $ord->AantalBesteld);
		if ($ord->AantalBesteld > 0) {
			$defmaken = true;
		}
	}
	echo("</tbody>\n");
	
	
	if ($totbedrag <> 0 or $totaant <> 0) {
		printf("<tr><td colspan=2>Totaal winkelwagen</td><td class='number'>%d</td><td class='number'>&euro;&nbsp;%s</td><td colspan=2></td></tr>\n", $totaant, number_format($totbedrag, 2));
	}
	
	echo("</table>\n");
	echo("<div id='opdrachtknoppen'>\n");
	
	printf("<select name='Nieuw_OR' onChange='this.form.submit();'><option value=0>Artikel toevoegen ...</option>%s</select>\n", $i_art->htmloptions(-1, "bestelbaar"));
	

	if ($defmaken) {
		echo("<button name='defmaken'>Definitief bestellen</button>\n");	
	}
	if (strlen($_SESSION['settings']['zs_voorwaardenbestelling']) > 0) {
		printf("<p>%s</p>\n", $_SESSION['settings']['zs_voorwaardenbestelling']);
	}
	echo("</div>  <!-- Einde opdrachtknoppen -->");
	echo("</form>\n");
	
	echo("</div>   <!-- Einde winkelwagen -->\n");
	echo("</div>   <!-- Einde webshop -->\n");
	
}  # fnWinkelwagen

function artikelbeheer() {
	
	$arrMaat = array("", "XS", "S", "M", "L", "XL", "XXL");

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
	echo("<tr><th>Code</th><th>Omschrijving</th><th>Maat</th><th>Verkoopprijs</th><th>Beschikbaar tot</th><th>Vervallen per</th><th>Max per lid</th><th>Beschikbaar voor groep</th><th></th></tr>\n");
	$lijst = $i_art->lijst("beheer");
	foreach ($lijst as $row) {
		echo("<tr>\n");
		printf("<td><input type='text' class='tbsmal' name='Code_%d' value='%s' maxlength=8 class='w8' OnChange='this.form.submit();'></td>\n", $row->RecordID, $row->Code);
		printf("<td><input type='text' name='Omschrijving_%d' value='%s' maxlength=50 class='w50' OnChange='this.form.submit();'></td>\n", $row->RecordID, $row->Omschrijving);
			
		$options = "";
		foreach($arrMaat as $m) {
			$options .= sprintf("<option value='%s' %s>%s</option>\n", $m, checked($m, "option", $row->Maat), $m);
		}
		printf("<td><select name='Maat_%d'>%s</select></td>\n", $row->RecordID, $options);
		printf("<td><input type='text' name='Verkoopprijs_%d' value='%s' class='d8'></td>\n", $row->RecordID, $row->Verkoopprijs);
		printf("<td><input type='date' name='BeschikbaarTot_%d' value='%s'></td>\n", $row->RecordID, $row->BeschikbaarTot);
		printf("<td><input type='date' name='VervallenPer_%d' value='%s'></td>\n", $row->RecordID, $row->VervallenPer);
		printf("<td><input type='number' name='MaxAantalPerLid_%d' value=%d maxlength=3 class='inputnumber'></td>\n", $row->RecordID, $row->MaxAantalPerLid);
		
		printf("<td><select name='BeperkTotGroep_%d'>\n", $row->RecordID);
		echo("<option value=0>Geen beperking</option>\n");
		foreach((new cls_Onderdeel())->lijst(1) as $ond) {
			if ($ond->RecordID == $row->BeperkTotGroep) {
				$s = "selected";
			} else {
				$s = "";
			}
			printf("<option value=%d %s>%s</option>\n", $ond->RecordID, $s, $ond->Naam);
		}
		echo("</select></td>\n");
		if ($row->InGebruik == 0) {
			$lnk = sprintf("%s?tp=%s&op=delete&aid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
			printf("<td><a href='%s'><i class='bi bi-trash'></i></a></td>", $lnk);
		} else {
			echo("<td></td>\n");
		}
		
		echo("</tr>\n");
	}
	echo("</table>\n");
	echo("<div id='opdrachtknoppen'>\n");
	echo("<button type='submit' name='Code_Nw'><i class='bi bi-plus-circle'></i> Nieuw artikel</button>\n");
	if (count($lijst) > 0) {
		echo("<button type='submit' name='action' value='save' title='Bewaren'><i class='bi bi-save'></i> Bewaren</button>\n");
	}
	echo("</div> <!-- Einde opdrachtknoppen -->\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde artikelbeheer -->\n");
	
}  # artikelbeheer

function bestellingbeheer() {

	$naamfilter="";
	$i_or = new cls_Orderregel();
	$i_vb = new cls_Voorraadboeking();
	$i_art = new cls_Artikel();
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['Lid_0']) and is_numeric($_POST['Lid_0']) and $_POST['Lid_0'] > 0) {
			$i_or->add($_POST['Lid_0'], 0, 1);
		}
		foreach($i_or->lijst("Ordernr > 0") as $row) {
			$f = sprintf("OrderregelID=%d", $row->RecordID);
			$vbid = $i_vb->max("RecordID", $f);
			if ( $vbid == 0) {
				$vbid = $i_vb->add($row->Artikel, $row->RecordID);
			}
			$i_or->lidid = $row->Lid;
			$pnart = sprintf("Artikel_%d", $row->RecordID);
			$pnab = sprintf("AantalBesteld_%d", $row->RecordID);
			$pnpps = sprintf("PrijsPerStuk_%d", $row->RecordID);
			$pnag = sprintf("AantalGeleverd_%d", $row->RecordID);
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
	
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<div id='filter'>\n");
	printf("<label>Naam bevat</label><input type='text' name='tbNaamFilter' size=30 value='%s' placeholder='Achter- of roepnaam' onblur='this.form.submit();'>\n", $naamfilter);
	echo("</div>  <!-- Einde filter -->\n");
	echo("<div id='webshop'>\n");
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
		echo("<tr>\n");
		printf("<td class='number'>%d</td>", $row->Ordernr);
		printf("<td>%s</td>", $row->NaamLid);
		if ($row->Artikel > 0) {
			printf("<td>%s</td>", $row->ArtOms);
		} else {
			printf("<td colspan=2><select name='Artikel_%d' onChange='this.form.submit();'>\n", $row->RecordID);
			printf("<option value=0>Selecteer artikel</option>\n%s</select>\n", $arthtmloptions);
		}
		
		printf("<td class='number'><input type='number' name='AantalBesteld_%d' value=%d onBlur='form.submit();'></td>\n", $row->RecordID, $row->AantalBesteld);
		
		if ($row->Artikel > 0) {
			printf("<td><input type='text' class='tbsmal' name='PrijsPerStuk_%d' value='%s' maxlength=8></td>\n", $row->RecordID, number_format($row->PrijsPerStuk, 2));
		} else {
			echo("<td></td>\n");
		}
		
		if (!is_null($row->BestellingDefinitief)) {
			printf("<td>%s</td>\n", strftime("%e %B %Y", strtotime($row->BestellingDefinitief)));
		} else {
			echo("<td></td>\n");
		}
		printf("<td class='number'>&euro;&nbsp;%s</td>", number_format($row->Bedrag, 2));
		if ($row->Artikel > 0 and $row->Lid > 0) {
			printf("<td class='number'><input type='number' name='AantalGeleverd_%d' value=%d></td>\n", $row->RecordID, $row->AantalGeleverd);
		} else {
			echo("<td></td>\n");
		}
		printf("<td>%s</td>", $row->Opmerking);
		if ($row->AantalGeleverd == 0) {
			$lnk = sprintf("%s?tp=%s&amp;op=delete&amp;rid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
			printf("<td><a href='%s'><img src='./images/trash.png'></a></td>", $lnk);
		}
		echo("</tr>\n");
		if ($row->Lid == 0 or $row->Artikel == 0) {
			$nrmag = false;
		}
	}
	echo("</table>\n");
	
	echo("<div id='opdrachtknoppen'>\n");
	
	if ($nrmag) {
		echo("<select name='Lid_0' onChange='this.form.submit();'>\n");
		echo("<option value=0>Nieuwe regel ...</option>\n");
		echo((new cls_Lid())->htmloptions(-1, 1));
		echo("</select>\n");
	}
	
	echo("<button type='submit' name='action' value='save' title='Bewaren'><i class='bi bi-save'></i> Bewaren</button>\n");
	echo("</div>  <!-- Einde opdrachtknoppen -->\n");
	echo("</div>  <!-- Einde bestellingbeheer -->\n");
	
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
		printf("<tr><td>%s</td>\n", $artrow->CodeOmsMaat);
		$vbrows = $i_vb->lijst($artrow->RecordID);
		$aant = 0;
		$vrrd = 0;
		foreach ($vbrows as $vbrow) {
			$vrrd += $vbrow->Aantal;
			if ($aant > 0) {
				echo("</tr>\n<tr>\n");
				echo("<td></td>");
			}
			if ($vbrow->OrderregelID > 0) {
				$nl = $vbrow->NaamLid;
				if (strlen($nl) > 0) {
					$nl = " (" . $nl . ")";
				}
				if ($vbrow->Ordernr > 0) {
					printf("<td>Levering op order %d%s</td>", $vbrow->Ordernr, $nl);
				} else {
					printf("<td>Levering op orderregel %d%s</td>", $vbrow->OrderregelID, $nl);
				}
			} else {
				printf("<td><input type='text' value='%s' name='Omschrijving_%d' OnBlur='this.form.submit();'></td>\n", $vbrow->Omschrijving	, $vbrow->RecordID);
			}
			printf("<td><input type='date' value='%s' name='Datum_%d' OnBlur='this.form.submit();'></td>\n", $vbrow->Datum, $vbrow->RecordID);
			printf("<td><input type='number' value=%d name='Aantal_%d' OnChange='this.form.submit();'></td>\n", $vbrow->Aantal, $vbrow->RecordID);
			printf("<td>%d</td>\n", $vrrd);
			$aant++;
		}
		echo("</tr>\n");
	}
	echo("</table>\n");
	
	echo("<div id='opdrachtknoppen'>\n");
	printf("<select name='Nieuw_vb' OnChange='this.form.submit();'><option value=0>Nieuwe boeking ....</option>%s</select>\n", $i_art->htmloptions());
	echo("</div>  <!-- Einde opdrachtknoppen -->\n");
	echo("</form>\n");
	
	echo("</div> <!-- Einde voorraadbeheer -->\n");
	echo("</div> <!-- Einde webshop -->\n");
}

?>
