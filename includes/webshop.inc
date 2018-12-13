<?php

function fnWebshop() {
	global $currenttab, $currenttab2, $table_prefix;
	
	fnDispMenu(2);

	if ($currenttab2 == "Artikelbeheer" and toegang($_GET['tp'], 1, 1)) {
		artikelbeheer();
	} elseif ($currenttab2 == "Bestellingen muteren" and toegang($_GET['tp'], 1, 1)) {
		bestellingbeheer();
	} elseif ($currenttab2 == "Logboek" and toegang($_GET['tp'], 1, 1)) {
		$lijst = db_logboek("lijst", "", 10);
		echo(fnDisplayTable($lijst, "", "", 0, "", "", "logboek"));
	}
}

function onlinebestellingen($lidid) {
	global $table_prefix;

	fnDispMenu(2);
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$lijst = db_artikel("artikelbeschikbaar");
		foreach ($lijst as $art) {
			$ord = db_orderregel("bestelling", $lidid, $art->RecordID);
			$mess = "";
			$set_clause = "";
			$wijz = "";
			$pnab = sprintf("AantalBesteld_%d", $art->RecordID);
			if (isset($_POST[$pnab])) {
				if (strlen($_POST[$pnab]) == 0 or !is_numeric($_POST[$pnab]) or $_POST[$pnab] < 0) {
					$_POST[$pnab] = 0;
				} elseif ($_POST[$pnab] > $art->MaxAantalPerLid) {
					$_POST[$pnab] = $art->MaxAantalPerLid;
				}
			}
			if (isset($_POST['Ordernr']) and is_numeric($_POST['Ordernr']) and $_POST['Ordernr'] > 0) {
				$ordernr = $_POST['Ordernr'];
			} else {
				$ordernr = db_orderregel("maxordernr") + 1;
				$mon = (((int)date("y")) * 1000) + 1;
				if ($ordernr < $mon) {
					$ordernr = $mon;
				}
			}
			if (!isset($ord->RecordID) and $_POST[$pnab] > 0 and $ordernr > 0) {
				$mess = db_orderregel("add", $lidid, $art->RecordID, $_POST[$pnab], "", 0, $ordernr);
			} elseif (isset($ord->RecordID)) {
				if ($ordernr > 0 and $ord->Ordernr != $ordernr) {
					$set_clause = sprintf("Ordernr=%d, ", $ordernr);
					$wijz = sprintf("Veld 'Ordernr' is %d geworden. ", $ordernr);
				}
				if (isset($_POST[$pnab]) and $ord->AantalBesteld != $_POST[$pnab]) {
					$set_clause .= sprintf("AantalBesteld=%d, ", $_POST[$pnab]);
					$wijz .= sprintf("Veld 'AantalBesteld' is %d geworden. ", $_POST[$pnab]);
				}
				$pnmt = sprintf("Maat_%d", $art->RecordID);
				if (isset($_POST[$pnmt]) and $_POST[$pnmt] != $ord->Maat) {
					$set_clause .= sprintf("Maat='%s', ", $_POST[$pnmt]);
					$wijz .= sprintf("Veld 'Maat' is '%s' geworden. ", $_POST[$pnmt]);
				}
				$pnopm = sprintf("Opmerking_%d", $art->RecordID);
				if (isset($_POST[$pnopm]) and $_POST[$pnopm] != $ord->Opmerking) {
					$set_clause .= sprintf("Opmerking='%s', ", $_POST[$pnopm]);
					$wijz .= sprintf("Veld 'Opmerking' is '%s' geworden. ", $_POST[$pnopm]);
				}
				if (strlen($set_clause) > 0) {
					$query = sprintf("UPDATE %sWS_Orderregel SET %s BestellingDefinitief=NULL, GewijzigdDoor=%d
								WHERE RecordID=%d;", $table_prefix, $set_clause, $_SESSION['lidid'], $ord->RecordID);
					$result = fnQuery($query);
					if ($result > 0) {
						$mess = sprintf("Orderregel van artikel %d voor %s is gewijzigd. %s", $ord->Artikel, db_naamlid($lidid), $wijz);
						db_logboek("add", $mess, 10, 0, 0, $ord->RecordID);
					}
				}
			}
		}
		if (isset($_POST['defmaken'])) {
			db_orderregel("defmaken", $lidid);
			if (db_param("mailing_bevestigingbestelling") > 0) {
				bevestigingbestelling($lidid, $ordernr);
			}
		}
	}

	echo("<div id='onlinebestellingen'>\n");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<table>\n");
	echo("<tr><th>Code</th><th>Omschrijving</th><th>Maat</th><th>Prijs per stuk</th><th>Bestelling</th><th>Bedrag</th><th>Geleverd</th><th>Opmerking</th></tr>\n");
	$defmaken = false;
	$totbedrag = 0;
	$ordnr = 0;
	$lijst = db_artikel("artikelbeschikbaar");
	foreach ($lijst as $art) {
		$ord = db_orderregel("bestelling", $lidid, $art->RecordID);
		if (isset($ord->RecordID)) {
			$ab = $ord->AantalBesteld;
			$ag = $ord->AantalGeleverd;
			$pps = $ord->PrijsPerStuk;
			$opm = $ord->Opmerking;
			if ($ord->Ordernr > 0) {
				$ordnr = $ord->Ordernr;
			}
			if (is_null($ord->BestellingDefinitief)) {
				$defmaken = true;
			}
		} else {
			$ab = 0;
			$ag = 0;
			$pps = $art->Verkoopprijs;
			$opm = "";
		}
		echo("<tr>\n");
		printf("<td>%s</td><td>%s</td><td>%s</td><td class='number'>&euro;&nbsp;%s</td>\n", $art->Code, $art->Omschrijving, $art->Maat, number_format($pps, 2));
		if ($ag > 0) {
			printf("<td class='number'>%d</td>\n", $ab);
		} else {
			printf("<td><input type='number' name='AantalBesteld_%d' min=0 max=%d value=%d  class='inputnumber'></td>\n", $art->RecordID, $art->MaxAantalPerLid, $ab);
		}
		printf("<td class='number'>&euro;&nbsp;%s</td>\n", number_format($pps*$ab, 2));
		printf("<td class='number'>%d</td>\n", $ag);
		if ($ab > 0 or strlen($opm) > 0) {
			printf("<td><input type='text' name='Opmerking_%d' value='%s' size=30 maxlength=30></td>\n", $art->RecordID, $opm);
		} else {
			printf("<td>%s</td>\n", $opm);
		}
		echo("</tr>\n");
		$totbedrag += ($pps * $ab);
	}
	if ($ordnr > 0) {
		$on = $ordnr;
	} else {
		$on = "";
	}
	printf("<tr><td colspan=5>Totaalbedrag order %s</td><td class='number'>&euro;&nbsp;%s</td><td colspan=2></td></tr>\n", $on, number_format($totbedrag, 2));
	echo("<tr><th colspan=8>");
	printf("<p>%s</p>\n", db_param("zs_voorwaardenbestelling"));
	echo("<input type='submit' value='Bestelling bijwerken'>\n");
	if ($defmaken) {
		echo("<button name='defmaken'>Bestelling definitief maken</button>\n");	
	}
	echo("</th></tr>\n");
	echo("</table>\n");
	printf("<input type='hidden' name='Ordernr' value=%d>\n", $ordnr);
	echo("</form>\n");
	echo("</div>   <!-- Einde onlinebestellingen -->\n");
}

function artikelbeheer() {
	global $table_prefix;

	if (isset($_GET['op']) and $_GET['op'] == "nieuw") {
		db_artikel("add");
	} elseif (isset($_GET['op']) and $_GET['op'] == "delete" and $_GET['aid'] > 0) {
		db_artikel("delete", $_GET['aid']);
	}
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$lijst = db_artikel("overzichtbeheer");
		$wijz = "";
		foreach ($lijst as $row) {
			$set_clause = "";
			foreach ($row as $col => $val) {
				$pvn = sprintf("%s_%d", $col, $row->RecordID);
				if ($col == "Verkoopprijs" and isset($_POST[$pvn])) {
					$_POST[$pvn] = str_replace(",",".", $_POST[$pvn]);
					if (!is_numeric($_POST[$pvn])) {
						$_POST[$pvn] = 0;
					}
				} elseif (($col == "BeschikbaarTot" or $col == "VervallenPer") and isset($_POST[$pvn])) {
					$_POST[$pvn] = change_month_to_uk($_POST[$pvn]);
					if (strlen($_POST[$pvn]) > 3 and strtotime($_POST[$pvn]) !== false) {
						$_POST[$pvn] = date("Y-m-d", strtotime($_POST[$pvn]));
					} else {
						$_POST[$pvn] = "9999-12-31";
					}
					if ($col == "BeschikbaarTot") {
						if ($_POST[$pvn] > $_POST[sprintf("VervallenPer_%d", $row->RecordID)]) {
							$_POST[$pvn] = $_POST[sprintf("VervallenPer_%d", $row->RecordID)];
						}
					}
				}
				if (isset($_POST[$pvn]) and $_POST[$pvn] != $val) {
					$set_clause .= sprintf("%s='%s', ", $col, $_POST[$pvn]);
					$wijz .= sprintf("Veld %s is '%s' geworden. ", $col, $_POST[$pvn]);
				}
			}
			if (strlen($set_clause) > 0) {
				$set_clause .= sprintf("Gewijzigd=SYSDATE(), GewijzigdDoor=%d", $_SESSION['lidid']);
				$query = sprintf("UPDATE %sWS_Artikel SET %s WHERE RecordID=%d;", $table_prefix, $set_clause, $row->RecordID);
				fnQuery($query);
				$mess = sprintf("Artikel %d is aangepast. %s", $row->RecordID, $wijz);
				db_logboek("add", $mess, 10, 0, 0, $row->RecordID);
			}
		}
	}
	echo("<div id='artikelbeheer'>\n");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<table>\n");
	echo("<tr><th>RecordID</th><th>Code</th><th>Omschrijving</th><th>Maat</th><th>Verkoopprijs</th><th>Beschikbaar tot</th><th>Vervallen per</th><th>Max aantal per lid</th><th>Beschikbaar voor groep</th><th></th></tr>\n");
	$lijst = db_artikel("overzichtbeheer");
	foreach ($lijst as $row) {
		echo("<tr>\n");
		printf("<td class='number'>%d</td>\n", $row->RecordID);
		printf("<td><input type='text' name='Code_%d' value='%s' size=6 maxlength=6></td>\n", $row->RecordID, $row->Code);
		printf("<td><input type='text' name='Omschrijving_%d' value='%s' size=35 maxlength=50></td>\n", $row->RecordID, $row->Omschrijving);
		printf("<td><input type='text' name='Maat_%d' value='%s' size=4 maxlength=4></td>\n", $row->RecordID, $row->Maat);
		printf("<td><input type='text' name='Verkoopprijs_%d' value='%s' maxlength=8 class='inputnumber'></td>\n", $row->RecordID, $row->Verkoopprijs);
		printf("<td><input type='date' name='BeschikbaarTot_%d' value='%s' size=10 maxlength=18></td>\n", $row->RecordID, $row->BeschikbaarTot);
		printf("<td><input type='date' name='VervallenPer_%d' value='%s' size=10 maxlength=18></td>\n", $row->RecordID, $row->VervallenPer);
		printf("<td><input type='number' name='MaxAantalPerLid_%d' value=%d maxlength=3 class='inputnumber'></td>\n", $row->RecordID, $row->MaxAantalPerLid);
		
		printf("<td><select name='BeperkTotGroep_%d'>\n", $row->RecordID);
		echo("<option value=0>Geen beperking</option>\n");
		foreach(db_onderdelen() as $ond) {
			if ($ond->RecordID == $row->BeperkTotGroep) {
				$s = "selected";
			} else {
				$s = "";
			}
			printf("<option value=%d %s>%s</option>\n", $ond->RecordID, $s, $ond->Naam);
		}
		echo("</select></td>\n");
		if (db_artikel("ingebruik", $row->RecordID) == 0) {
			$lnk = sprintf("%s?tp=%s&amp;op=delete&amp;aid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
			printf("<td><a href='%s'><img src='./images/trash.png'></a></td>", $lnk);
		} else {
			echo("<td></td>\n");
		}
		
		echo("</tr>\n");
	}
	printf("<tr><td class='number'><a href='%s?%s&amp;op=nieuw' title='Nieuw artikel'><img src='images/star.png' alt='Nieuw'></a></td><td colspan=9>Nieuw artikel</td></tr>\n", $_SERVER['PHP_SELF'], $_SERVER['QUERY_STRING']);
	if (count($lijst) > 0) {
		echo("<tr>\n<th colspan=10>");
		echo("<button type='submit' name='action' value='save' title='Bewaren'><img src='images/save.png' alt='Save'></button>\n");
		echo("</th>\n</tr>\n");
	}
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde artikelbeheer -->\n");
}

function bestellingbeheer() {
	global $table_prefix, $fdlang;

	$naamfilter="";
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['Lid_0']) and is_numeric($_POST['Lid_0']) and $_POST['Lid_0'] > 0) {
			db_orderregel("add", $_POST['Lid_0']);
		}
		$lijst = db_orderregel("overzichtbeheer");
		foreach($lijst as $row) {
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
				$set_clause = sprintf("Artikel=%d, ", $_POST[$pnart]);
				$wijz = sprintf("Veld Artikel is '%d' geworden. ", $_POST[$pnart]);
			} else {
				$set_clause = "";
				$wijz = "";
			}
			if (isset($_POST[$pnab]) and $_POST[$pnab] != $row->AantalBesteld and $row->Ordernr == 0) {
				$set_clause .= sprintf("AantalBesteld=%d, ", $_POST[$pnab]);
				$wijz .= sprintf("Veld AantalBesteld is '%d' geworden. ", $_POST[$pnab]);
			}
			if ($_POST[$pnpps] != $row->PrijsPerStuk) {
				$set_clause .= sprintf("PrijsPerStuk='%s', ", number_format($_POST[$pnpps], 2, ".", ""));
				$wijz .= sprintf("Veld PrijsPerStuk is '%s' geworden. ", number_format($_POST[$pnpps], 2));
			}
			if ($_POST[$pnag] != $row->AantalGeleverd) {
				$set_clause .= sprintf("AantalGeleverd=%d, ", $_POST[$pnag]);
				$wijz .= sprintf("Veld AantalGeleverd is '%d' geworden. ", $_POST[$pnag]);
			}
			if (strlen($set_clause) > 0) {
				$set_clause .= sprintf("GewijzigdDoor=%d", $_SESSION['lidid']);
				$query = sprintf("UPDATE %sWS_Orderregel SET %s WHERE RecordID=%d;", $table_prefix, $set_clause, $row->RecordID);
				$result = fnQuery($query);
				if ($result > 0) {
					$mess = sprintf("Orderregel %d is gewijzigd. %s", $row->RecordID, $wijz);
					db_logboek("add", $mess, 10, $row->Lid);
					db_orderregel("vulprijsperstuk");
				}
			}
		}
		if (isset($_POST['tbNaamFilter']) and strlen($_POST['tbNaamFilter']) > 0) {
			$naamfilter = $_POST['tbNaamFilter'];
		}	
	} elseif (isset($_GET['op']) and $_GET['op'] == "delete" and $_GET['rid'] > 0) {
		db_orderregel("delete", 0, 0, 0, "", $_GET['rid']);
	}
	
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<div id='filter'>\n");
	echo("<table>\n");
	echo("<tr>\n");
	printf("<td class='label'>Naam bevat</td><td><input type='text' name='tbNaamFilter' size=30 value='%s' placeholder='Achter- of roepnaam' onblur='this.form.submit();'></td>\n", $naamfilter);
	echo("</tr>");
	echo("</table>\n");
	echo("</div>  <!-- Einde filter -->\n");
	echo("<div id='bestellingbeheer'>\n");
	echo("<table>\n");
	echo("<tr><th>Ordernr</th><th>Lid</th><th>Artikel</th><th>Maat</th><th>Besteld</th><th>Prijs per stuk</th><th>Datum besteld</th><th>Bedrag</th><th>Geleverd</th><th>Opmerking bij bestellen</th><th></th></tr>\n");
	$nrmag = true;
	if (strlen($naamfilter) > 0) {
		$filter = sprintf("(L.Achternaam LIKE '%1\$s%%' OR L.Roepnaam LIKE '%1\$s%%' OR L.Meisjesnm LIKE '%1\$s%%')", $naamfilter);
	} else {
		$filter = "";
	}
	$lijst = db_orderregel("overzichtbeheer", 0, 0, 0, $filter);
	foreach($lijst as $row) {
		echo("<tr>\n");
		printf("<td class='number'>%d</td>", $row->Ordernr);
		printf("<td>%s</td>", $row->NaamLid);
		if ($row->Artikel > 0) {
			printf("<td>%s</td><td>%s</td>", $row->Omschrijving, $row->Maat);
		} else {
			printf("<td colspan=2><select name='Artikel_%d' onChange='this.form.submit();'>\n", $row->RecordID);
			echo("<option value=0>&nbsp;</option>\n");
			foreach(db_artikel("select", 0, $row->Lid) as $art) {
				printf("<option value=%d>%s</option>\n", $art->RecordID, $art->OmsMaat);
			}
			echo("</select></td>\n");
		}
		if ($row->Ordernr > 0) {
			printf("<td class='number'>%d</td>\n", $row->AantalBesteld);
		} else {
			printf("<td class='number'><input type='number' name='AantalBesteld_%d' value=%d  class='inputnumber' onBlur='form.submit();'></td>\n", $row->RecordID, $row->AantalBesteld);
		}

		if ($row->Artikel > 0) {
			printf("<td class='number'><input type='text' name='PrijsPerStuk_%d' value='%s' maxlength=8 class='inputnumber'></td>\n", $row->RecordID, number_format($row->PrijsPerStuk, 2));
		} else {
			echo("<td></td>\n");
		}
		
		if (is_null($row->BestellingDefinitief) and $row->AantalBesteld > 0 and $row->Ordernr > 0) {
			echo("<td class='nietdefinitief'>ND</td>\n");
		} elseif (!is_null($row->BestellingDefinitief)) {
			printf("<td>%s</td>\n", strftime("%e %B %Y", strtotime($row->BestellingDefinitief)));
		} else {
			echo("<td></td>\n");
		}
		printf("<td class='number'>&euro;&nbsp;%s</td>", number_format($row->Bedrag, 2));
		if ($row->Artikel > 0 and $row->Lid > 0) {
			printf("<td class='number'><input type='number' name='AantalGeleverd_%d' value=%d  class='inputnumber'></td>\n", $row->RecordID, $row->AantalGeleverd);
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
	if ($nrmag) {
		echo("<tr>\n");
		$w = sprintf("L.Nummer IN (SELECT Lid FROM %sLidmaatschap AS LM WHERE ((LM.Opgezegd IS NULL) OR LM.Opgezegd >= CURDATE()))", $table_prefix);
		echo("<td colspan=11><select name='Lid_0' onChange='this.form.submit();'>\n");
		echo("<option value=0>Nieuwe regel ...</option>\n");
		foreach(db_lid("lijst", $w) as $lid) {
			printf("<option value=%d>%s</option>\n", $lid->Nummer, $lid->Zoeknaam);
		}
		echo("</select></td>\n");
		echo("</tr>\n");
	}
	
	echo("<tr>\n<th colspan=11>");
	echo("<button type='submit' name='action' value='save' title='Bewaren'><img src='images/save.png' alt='Save'></button>\n");
	echo("</th>\n</tr>\n");
	echo("</table>\n");
	echo("</div>  <!-- Einde bestellingbeheer -->\n");
	
	$tots = db_orderregel("totalen");
	if (count($tots) > 0) {
		echo(fnDisplayTable($tots, "", "Totalen", 99, "", "", "totalenbestellingen"));
	}
	
}

function bevestigingbestelling($lidid, $ordernr=0) {

	if (db_param("mailing_bevestigingbestelling") > 0) {
		$mailing = new Mailing(db_param("mailing_bevestigingbestelling"));
		$mailing->xtranum = $ordernr;
		$mailing->resultaatversturen = 0;
		if ($mailing->send($lidid) > 0) {
			$mess = sprintf("Bevestiging bestelling %d is aan %s verzonden.", $ordernr, db_naamlid($lidid));
		} else {
			$mess = sprintf("Fout bij het versturen van de bevestiging. Probeer het later nogmaals of neem contact op met de webmaster.");
		}
		$mailing = null;
	} else {
		$mess = "Er is geen mailing voor het bevestigingen van de bestelling gedefinieerd.";
	}
	db_logboek("add", $mess, 10, $lidid, 2, $ordernr);
	
	printf("<script>location.href='%s?tp=%s';</script>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
}

?>
