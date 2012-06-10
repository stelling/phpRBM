<?php

function fnWebshop() {
	global $currenttab2, $table_prefix;
	
	fnDispMenu(2);

	if ($currenttab2 == "Artikelbeheer") {
		artikelbeheer();
	} elseif ($currenttab2 == "Bestellingen muteren") {
		bestellingbeheer();
	} elseif ($currenttab2 == "Logboek" and toegang($_GET['tp'])) {
		$lijst = db_logboek("lijst", "", 10);
		echo(fnDisplayTable($lijst, "", "", 0, "", "", "logboek"));
	}
}

function onlinebestellingen($lidid) {
	global $table_prefix, $voorwaardenbestelling;

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
			if (isset($ord->RecordID) and $_POST[$pnab] == 0) {
				$mess = db_orderregel("delete", $lidid, $art->RecordID);
			} elseif (!isset($ord->RecordID) and $_POST[$pnab] > 0) {
				$mess = db_orderregel("add", $lidid, $art->RecordID, $_POST[$pnab]);
			} elseif (isset($ord->RecordID)) {
				if (isset($_POST[$pnab]) and $ord->AantalBesteld != $_POST[$pnab]) {
					$set_clause = sprintf("AantalBesteld=%d, ", $_POST[$pnab]);
					$wijz = sprintf("Veld 'AantalBesteld' is %d geworden. ", $_POST[$pnab]);
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
						db_logboek("add", $mess, 10);
					}
				}
			}
		}
		if (isset($_POST['defmaken'])) {
			db_orderregel("defmaken", $lidid);
			bevestigingbestelling($lidid);
		}
		if (strlen($mess) > 0) {	
//			printf("<p class='mededeling'>%s</p>\n", $mess);
		}
	}

	echo("<div id='onlinebestellingen'>\n");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<table>\n");
	echo("<tr><th>Code</th><th>Omschrijving</th><th>Maat</th><th>Prijs per stuk</th><th>Bestelling</th><th>Bedrag</th><th>Geleverd</th><th>Opmerking</th></tr>\n");
	$defmaken = false;
	$totbedrag = 0;
	$lijst = db_artikel("artikelbeschikbaar");
	foreach ($lijst as $art) {
		$ord = db_orderregel("bestelling", $lidid, $art->RecordID);
		if (isset($ord->RecordID)) {
			$ab = $ord->AantalBesteld;
			$ag = $ord->AantalGeleverd;
			$pps = $ord->PrijsPerStuk;
			$opm = $ord->Opmerking;
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
			printf("<td><input type='number' name='AantalBesteld_%d' min=0 max=%d value=%d size=6></td>\n", $art->RecordID, $art->MaxAantalPerLid, $ab);
		}
		printf("<td class='number'>&euro;&nbsp;%s</td>\n", number_format($pps*$ab, 2));
		printf("<td class='number'>%d</td>\n", $ag);
		if ($ab > 0) {
			printf("<td><input type='text' name='Opmerking_%d' value='%s' size=30 maxlength=30></td>\n", $art->RecordID, $opm);
		} else {
			echo("<td>&nbsp;</td>\n");
		}
		echo("</tr>\n");
		$totbedrag += ($pps * $ab);
	}
	printf("<tr><td colspan=5>Totaalbedrag</td><td class='number'>&euro;&nbsp;%s</td><td colspan=2></td></tr>\n", number_format($totbedrag, 2));
	echo("<tr><th colspan=8>");
	printf("<p>%s</p>\n", $voorwaardenbestelling);
	echo("<input type='submit' value='Bestelling bijwerken'>\n");
	if ($defmaken) {
		echo("<button name='defmaken'>Bestelling definitief maken</button>\n");	
	}
	echo("</th></tr>\n");
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>   <!-- Einde onlinebestellingen -->\n");
}

function artikelbeheer() {
	global $table_prefix;

	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['NieuwArtikel'])) {
			db_artikel("add");
		}
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
				} elseif ($col == "BeschikbaarTot" and isset($_POST[$pvn])) {
					$_POST[$pvn] = change_month_to_uk($_POST[$pvn]);
					if (strlen($_POST[$pvn]) > 0 and strtotime($_POST[$pvn]) !== false) {
						$_POST[$pvn] = date("Y-m-d", strtotime($_POST[$pvn]));
					} else {
						$_POST[$pvn] = "9999-12-31";
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
				db_logboek("add", $mess, 10);
			}
		}
	}
	$lijst = db_artikel("overzichtbeheer");
	printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	echo("<table>\n");
	echo("<tr><th>RecordID</th><th>Code</th><th>Omschrijving</th><th>Maat</th><th>Verkoopprijs</th><th>Beschikbaar tot</th></tr>\n");
	foreach ($lijst as $row) {
		echo("<tr>\n");
		printf("<td class='number'>%d</td>\n", $row->RecordID);
		printf("<td><input type='text' name='Code_%d' value='%s' size=6 maxlength=6></td>\n", $row->RecordID, $row->Code);
		printf("<td><input type='text' name='Omschrijving_%d' value='%s' size=30 maxlength=30></td>\n", $row->RecordID, $row->Omschrijving);
		printf("<td><input type='text' name='Maat_%d' value='%s' size=6 maxlength=4></td>\n", $row->RecordID, $row->Maat);
		printf("<td><input type='text' name='Verkoopprijs_%d' value='%s' size=10 maxlength=8></td>\n", $row->RecordID, $row->Verkoopprijs);
		printf("<td><input type='date' name='BeschikbaarTot_%d' value='%s' size=20 maxlength=18></td>\n", $row->RecordID, $row->BeschikbaarTot);
			
		echo("</tr>\n");
	}
	echo("</table>\n");
	echo("<input type='submit' value='Artikelen bijwerken'>\n");
	echo("<button name='NieuwArtikel'>Nieuw artikel</button>\n");
	echo("</form>\n");
}

function bestellingbeheer() {
	global $table_prefix;

	$naamfilter="";
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['NieuweRegel'])) {
			db_orderregel("add");
		}
		$lijst = db_orderregel("overzichtbeheer");
		foreach($lijst as $row) {
			$pnlid = sprintf("Lid_%d", $row->RecordID);
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
			if (isset($_POST[$pnlid]) and $_POST[$pnlid] > 0) {
				$set_clause = sprintf("Lid=%d, ", $_POST[$pnlid]);
				$wijz = sprintf("Veld Lid is '%d' geworden. ", $_POST[$pnlid]);
			} else {
				$set_clause = "";
				$wijz = "";
			}
			if (isset($_POST[$pnart]) and $_POST[$pnart] > 0) {
				$set_clause = sprintf("Artikel=%d, ", $_POST[$pnart]);
				$wijz .= sprintf("Veld Artikel is '%d' geworden. ", $_POST[$pnart]);
			}
			if ($_POST[$pnab] != $row->AantalBesteld) {
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
					db_logboek("add", $mess, 10);
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
	echo("<tr><th>Lid</th><th>Artikel</th><th>Maat</th><th>Prijs per stuk</th><th>Besteld</th><th>Bedrag</th><th>Geleverd</th><th>Opmerking bij bestellen</th><th></th></tr>\n");
	$nrmag = true;
	if (strlen($naamfilter) > 0) {
		$filter = sprintf("(L.Achternaam LIKE '%1\$s%%' OR L.Roepnaam LIKE '%1\$s%%' OR L.Meisjesnm LIKE '%1\$s%%')", $naamfilter);
	} else {
		$filter = "";
	}
	$lijst = db_orderregel("overzichtbeheer", 0, 0, 0, $filter);
	foreach($lijst as $row) {
		echo("<tr>\n");
		if ($row->Lid > 0) {
			printf("<td>%s</td>", $row->NaamLid);
		} else {
			printf("<td><select name='Lid_%d' onChange='this.form.submit();'>\n", $row->RecordID);
			echo("<option value=0>&nbsp;</option>\n");
			foreach(db_lid("lijst") as $lid) {
				printf("<option value=%d>%s</option>\n", $lid->Nummer, $lid->Zoeknaam);
			}
			echo("</select></td>\n");
		}
		if ($row->Artikel > 0) {
			printf("<td>%s</td><td>%s</td>", $row->Omschrijving, $row->Maat);
			printf("<td class='number'><input type='text' name='PrijsPerStuk_%d' value='%s' size=10 maxlength=8></td>\n", $row->RecordID, number_format($row->PrijsPerStuk, 2));
		} else {
			printf("<td colspan=3><select name='Artikel_%d' onChange='this.form.submit();'>\n", $row->RecordID);
			echo("<option value=0>&nbsp;</option>\n");
			foreach(db_artikel("overzichtbeheer") as $art) {
				printf("<option value=%d>%s</option>\n", $art->RecordID, $art->OmsMaat);
			}
			echo("</select></td>\n");
		}
		printf("<td class='number'><input type='number' name='AantalBesteld_%d' value=%d size=6></td>\n", $row->RecordID, $row->AantalBesteld);
		printf("<td class='number'>&euro;&nbsp;%s</td>", number_format($row->Bedrag, 2));
		printf("<td class='number'><input type='number' name='AantalGeleverd_%d' value=%d size=6></td>\n", $row->RecordID, $row->AantalGeleverd);
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
	echo("<input type='submit' value='Bijwerken'>\n");
	if ($nrmag) {
		echo("<button name='NieuweRegel'>Nieuwe regel</button>\n");
	}
	echo("</div>  <!-- Einde bestellingbeheer -->\n");
}

function bevestigingbestelling($lidid) {
	global $naamvereniging, $naamwebsite, $urlwebsite, $emailbevestigingbestelling, $voorwaardenbestelling;

	$bevest_template = 'templates/bevestiging_bestelling.html';
	if (file_exists($bevest_template)) {
		$content = file_get_contents($bevest_template);
	} else {
		$content = "Hierbij bevestigen wij de volgende bestelling: [%BESTELLING%]";
	}

	$bestelling = "";
	$totbedrag = 0;
	foreach (db_orderregel("bevestiging", $lidid) as $row) {
		$naamlid = $row->NaamLid;
		$roepnaam = $row->Roepnaam;
		$email = $row->Email;
		$bestelling .= sprintf("<tr><td>%s</td><td>%s</td><td>%s</td><td class='number'>%d</td><td class='number'>&euro;&nbsp;%s</td><td>%s</td></tr>\n", $row->Code, $row->Omschrijving, $row->Maat, $row->AantalBesteld, number_format($row->Bedrag, 2), $row->Opmerking);
		$totbedrag += $row->Bedrag;
	}
	
	$content = str_ireplace("[%NAAMLID%]", $naamlid, $content);
	$content = str_ireplace("[%ROEPNAAM%]", $roepnaam, $content);
	$content = str_replace("[%NAAMVERENIGING%]", $naamvereniging, $content);
	$content = str_replace("[%NAAMWEBSITE%]", $naamwebsite, $content);
	$content = str_replace("[%URLWEBSITE%]", $urlwebsite, $content);
	$content = str_replace("[%VOORWAARDEN%]", $voorwaardenbestelling, $content);
	$content = str_ireplace("[%BESTELLING%]", $bestelling, $content);
	$content = str_ireplace("[%TOTAALBEDRAG%]", "&euro;&nbsp;" . number_format($totbedrag, 2), $content);

	$subj = "Bevestiging bestelling";
	$brief_template = "templates/briefpapier.html";
	if (file_exists($brief_template)) {
		$briefpapier = file_get_contents($brief_template);
		if ($content === false) {
			$htmlmessage = $content;
		} else {
			$htmlmessage = str_ireplace("[%MESSAGE%]", $content, $briefpapier);
			$htmlmessage = str_ireplace("[%FROM%]", $naamvereniging, $htmlmessage);
			$htmlmessage = str_ireplace("[%TO%]", $naamlid, $htmlmessage);
			$htmlmessage = str_ireplace("[%SUBJECT%]", $subj, $htmlmessage);
		}
	} else {
		$htmlmessage = $content;
	}
				
	$mail = new RBMmailer();
	$mail->From = $emailbevestigingbestelling;
	$mail->AddAddress($email, $naamlid);
//	$mail->AddCC($emailbevestigingbestelling);
	$mail->Subject = $subj;
	$mail->MsgHTML($htmlmessage);
	if ($mail->Send()) {
		db_add_mailing_hist($lidid, 0, $naamlid, $email, $content, $mail->FromName, $mail->From, $mail->Subject);
		$mess = sprintf("Bevestiging van deze bestelling is naar %s verzonden.", $email);
	} else {
		$mess = sprintf("Het versturen van de bevestiging van de bestelling aan %s is mislukt. Error: %s", db_naamlid($lidid), $mail->ErrorInfo);
	}
	db_logboek("add", $mess, 10);
	printf("<script>alert('%s');</script>\n", $mess);
	echo("<script>location.href='/index.php';</script>\n");
}

?>
