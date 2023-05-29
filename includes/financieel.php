<?php

function fnRekeningen() {
	global $currenttab, $currenttab2;
	
	$reknr = $_GET['p_reknr'] ?? 0;
	$op = $_GET['op'] ?? "";
	if ($op != "preview_rek") {
		fnDispMenu(2);
	}
	
	if ($op == "preview_rek") {
		echo(RekeningDetail($reknr, 1));
	} elseif ($op == "send_rek") {
		fnVerstuurRekening($reknr, 1);
		fnRekeningMuteren($reknr);
	} elseif ($currenttab2 == "Muteren") {
		fnRekeningMuteren($reknr);
	} elseif ($currenttab2 == "Beheer") {
		fnRekeningbeheer();
		
	} elseif ($currenttab2 == "Nieuw") {
		
		if ($_SERVER['REQUEST_METHOD'] == "POST") {
			if (isset($_POST['RekToevoegen'])) {
				if ($_POST['nwlid'] <= 0) {
					echo("<p class='mededeling'>Selecteer eerst een (Klos)lid.</p>\n");
				} else {
					$reknr = (new cls_Rekening())->add($_POST['nwrekening'], $_POST['nwseizoen'], $_POST['nwlid']);
					printf("<script>location.href='%s?tp=%s/Muteren&p_reknr=%d'</script>\n", $_SERVER['PHP_SELF'], $currenttab, $reknr);
				}
			}
			
		} else {
		
			$nwreknr = (new cls_rekening())->max("RK.Nummer") + 1;
			if (($nwreknr / 2) == intval($nwreknr / 2)) {
				$nwreknr++;
			}

			$actionurl = sprintf("%s?tp=%s", $_SERVER['PHP_SELF'], $_GET['tp']);
			printf("<form method='post' id='rekeningmuteren' action='%s'>\n", $actionurl);
		
			printf("<label>Rekeningnummer</label><input type='number' name='nwrekening' value=%d class='d8'>\n", $nwreknr);
			printf("<label>Seizoen</label><select name='nwseizoen'>%s</select>\n", (new cls_Seizoen())->htmloptions());
			printf("<label>Lid</label><select name='nwlid'><option value=0>Selecteer lid ...</option>\n%s</select>\n", (new cls_Lid())->htmloptions(-1, 2));
			echo("<div class='clear'></div>\n");
			echo("<button type='submit' name='RekToevoegen'><i class='bi bi-plus-circle'></i> Toevoegen</button>\n");
			echo("</form>\n");
		}
		
	} elseif ($currenttab2 == "Aanmaken rekeningen") {
		RekeningenAanmaken();
		
	} elseif ($currenttab2 == "Betalingen") {
		RekeningBetalingen();
		
	} elseif ($currenttab2 == "Instellingen") {
		RekeningInstellingen();

	} elseif ($currenttab2 == "Logboek") {
		$rows = (new cls_Logboek())->lijst(14);
		if (count($rows) > 0) {
			echo("<div id='filter'>\n");
			echo("<input type='text' id='tbFilterNaam' placeholder='Tekstfilter' OnKeyUp=\"fnFilter('logboek', this);\">\n");
			echo("</div> <!-- Einde filter -->\n");
			
			$kols = fnStandaardKols("logboek");
			echo(fnDisplayTable($rows, $kols, "Logboek rekeningen", 0, "", "logboek"));
		} else {
			echo("<p>Er zijn geen mutaties bekend.</p>\n");
		}
	}
}

function fnRekeningbeheer() {
	global $currenttab;
	
	$i_rk = new cls_Rekening();
	$i_sz = new cls_Seizoen();
	
	if (isset($_GET['op']) and $_GET['op'] == "deleterekening" and $_GET['p_reknr'] > 0) {
		$i_rk->delete($_GET['p_reknr']);
	}

	if (isset($_POST['filterseizoen'])) {
		$filterseizoen = intval($_POST['filterseizoen']);
	} else {
		$filterseizoen = $i_rk->Max("RK.Seizoen");
	}
	$filternaam = $_POST['tbFilterNaam'] ?? "";
	
	$rows = $i_rk->overzichtbeheer($filterseizoen, $filternaam);
	
	if ($filterseizoen > 0 and strlen($filternaam) == 0) {
		$js = "OnKeyUp=\"fnFilter('overzichtrekeningen', this);\"";
	} else {
		$js = "Onblur=\"this.form.submit();\"";
	}
	
	printf("<form method='post' id='filter' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	printf("<label>Seizoen</label><select name='filterseizoen' onChange='this.form.submit();'>\n<option value=-1>Alle seizoenen</option>\n%s</select>\n", $i_sz->htmloptions($filterseizoen, 1));
	printf("<input type='text' id='tbFilterNaam' name='tbFilterNaam' placeholder='Tekstfilter' value='%s' %s>\n", $filternaam, $js);
	printf("<p class='aantrecords'>%d rekeningen</p>", count($rows));
	echo("</form>\n");
	
	$kols[0]['link'] = sprintf("%s?tp=%s/Muteren&p_reknr=%%d", $_SERVER['PHP_SELF'], $currenttab);
	$kols[0]['columnname'] = "Nummer";
	$kols[0]['class'] = "muteren";
	
	$kols[1]['headertext'] = "Nummer";
	$kols[1]['columnname'] = "Nummer";
	
	$kols[2]['headertext'] = "Datum";
	$kols[2]['columnname'] = "Datum";
	$kols[2]['type'] = "date";
	
	$kols[3]['headertext'] = "Omschrijving";
	$kols[3]['columnname'] = "OMSCHRIJV";
	
	$kols[3]['headertext'] = "Tenaamstelling";
	$kols[3]['columnname'] = "DEBNAAM";
	
	$kols[4]['headertext'] = "Bedrag";
	$kols[4]['columnname'] = "Bedrag";
	$kols[4]['type'] = "bedrag";

	$f = sprintf("RK.Seizoen=%d", $filterseizoen);
	if ($i_rk->max("RK.Betaald", $f) > 0) {
		$kols[5]['headertext'] = "Betaald";
		$kols[5]['columnname'] = "Betaald";
		$kols[5]['type'] = "bedrag";
	}
	
	$adl = 0;
	foreach ($rows as $row) {
		if ($row->linkDelete > 0) {
			$adl++;
		}
	}
	
	if ($adl > 0) {
		$kols[7]['link'] = sprintf("%s?tp=%s/Beheer&op=deleterekening&p_reknr=%%d", $_SERVER['PHP_SELF'], $currenttab);
		$kols[7]['class'] = 'trash';
		$kols[7]['columnname'] = "linkDelete";
	}
	
	if (count($rows) > 0) {
		echo(fnDisplayTable($rows, $kols, "", 0, "", "overzichtrekeningen"));
	}
}

function fnRekeningMuteren($p_rkid=-1) {
	global $currenttab;

	$scherm = "M";
	if (isset($_POST['rkid']) and $_POST['rkid'] > 0) {
		$reknr = $_POST['rkid'];
	} elseif (isset($_GET['rkid']) and $_GET['rkid'] > 0) {
		$reknr = $_GET['rkid'];
	} elseif ($p_rkid > 0) {
		$reknr = $p_rkid;
	} else {
		$reknr = 0;
	}
	
	$i_rk = new cls_Rekening($reknr);
	$i_rr = new cls_Rekeningregel($reknr);
	$i_lid = new cls_Lid();
	$i_seiz = new cls_Seizoen();
	$i_ond = new cls_Onderdeel();
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['RRnieuw']) and $_POST['RRnieuw'] >= 0) {
			$at = $i_rr->standaardwaarde($reknr, $_POST['RRnieuw']);
			if ($at == 0) {
				$i_rr->add($reknr, $_POST['RRnieuw']);
			}
		}
	}
	
	if ($scherm == "M" and $reknr > 0)  {
		$row = $i_rk->record($reknr);
		if ($row->AantLid == 1) {
			$i_rk->update($reknr, "Lid", $row->EersteLid);
		}
		$tb = $i_rr->totaal("RR.Bedrag", sprintf("Rekening=%d", $reknr));
		$i_rk->update($reknr, "Bedrag", $tb);
		
		echo("<div id='rekeningmuteren'>\n");
		$actionurl = sprintf("%s?tp=%s&rkid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $reknr);
		printf("<form method='post' action='%s'>\n", $actionurl);
		echo("<div id='rekeningkopmuteren'>\n");
		
		printf("<label>Rekeningnummer</label><p id='reknr'>%d</p>\n", $row->Nummer);
		printf("<input type='hidden' name='rkid' value=%d>\n", $row->Nummer);
		
		echo("<div id='rekeninginfo'>\n");
		echo("<label>Totaal bedrag &euro;</label><p id='rekeningbedrag'></p>\n");
		printf("<label id='lblbedragbetaald'>Betaald &euro;</label><p id='bedragbetaald'>%03.2f</p>\n", $row->Betaald);
		echo("<label id='lbluitersteBetaling'>Uiterste betaling op</label><p id='uitersteBetaling'></p>\n");
		echo("<label>Telefoon debiteur</label><p id='telefoondebiteur'></p>\n");
		echo("<label>E-mail debiteur</label><p id='emaildebiteur'></p>\n");
		
		echo("<label>Per E-mail verstuurd</label><p id='laatsteemail'></p>\n");
		echo("</div> <!-- Einde rekeninginfo -->\n");
		
		printf("<label>Seizoen</label><select id='Seizoen'>%s</select>\n", $i_seiz->htmloptions($row->Seizoen));
		printf("<label>Rekeningdatum</label><input type='date' id='Datum' value='%s' required>\n", $row->Datum);
		printf("<label>Omschrijving</label><input type='text' id='OMSCHRIJV' class='w35' value='%s' maxlength=35>\n", $row->OMSCHRIJV);
		printf("<label>Tenaamstelling rekening</label><input type='text' id='DEBNAAM' class='w60' value='%s' maxlength=60>\n", $row->DEBNAAM);
		if ($row->AantLid > 1) {
			$d = "";
		} else {
			$d = " disabled";
		}
		$f = sprintf("L.RecordID IN (SELECT RR.Lid FROM %sRekreg AS RR WHERE RR.Rekening=%d)", TABLE_PREFIX, $row->Nummer);
		printf("<label>Gekoppeld aan lid</label><select id='Lid'%s><option value=-1></option>%s</select>\n", $d, $i_lid->htmloptions($row->Lid, 0, $f, $row->Datum));
		
		$f = sprintf("(L.RecordID IN (SELECT LO.Lid FROM %sLidond AS LO WHERE IFNULL(LO.Opgezegd, '9999-12-31') >= '%s' AND LO.OnderdeelID=%d)", TABLE_PREFIX, $row->Datum, $_SESSION['settings']['rekening_groep_betaalddoor']);
		$f .= sprintf(" OR L.RecordID IN (SELECT RR.Lid FROM %sRekreg AS RR WHERE RR.Rekening=%d)", TABLE_PREFIX, $reknr);
		$f .= sprintf(" OR L.RecordID=%d OR L.RecordID=%d)", $row->BetaaldDoor, $row->Lid);
		printf("<label>Betaald door / debiteur</label><select id='BetaaldDoor'>\n%s</select>\n", $i_lid->htmloptions($row->BetaaldDoor, 7, $f, $row->Datum));
		if ($row->BETAALDAG < 1) {
			$bdt = $i_seiz->max("SZ.BetaaldagenTermijn", sprintf("SZ.Nummer=%d", $row->Seizoen));
		} else {
			$bdt = $row->BETAALDAG;
		}
		printf("<label id='lblBETAALDAG'>Betaaltermijn in dagen</label><input type='text' id='BETAALDAG' value=%d class='w3' maxlength=3>\n", $bdt);
		
		if ($row->BET_TERM < 1) {
			$bt = 1;
		} else {
			$bt = $row->BET_TERM;
		}
		printf("<label id='lblBET_TERM'>Aantal betaaltermijnen</label><input type='text' id='BET_TERM' value=%d class='w2' maxlength=2>\n", $bt);
		
		echo("</div> <!-- einde rekeningkopmuteren  -->\n");
		
		echo("<div class='clear'></div>\n");
		
		if ($reknr > 0) {
			foreach($i_rr->perrekening($reknr) as $rrrow) {
				if (strlen($rrrow->KSTNPLTS) > 0 and strlen($rrrow->OMSCHRIJV) == 0) {
					$ondrow = $i_ond->record(0, $rrrow->KSTNPLTS);
					if (isset($ondrow->Naam)) {
						$i_rr->update($rrrow->RecordID, "OMSCHRIJV", $ondrow->Naam);
						$i_rr->update($rrrow->RecordID, "KSTNPLTS", $ondrow->Kode);
					}
				}
			}
			echo("<div id='rekeningregelsmuteren'>\n");
			printf("<table id='rekregels' class='%s'>\n", TABLECLASSES);
			echo("<tr><th>#</th><th>Kostenplaats</th><th>Lid</th><th>Omschrijving</th><th>Bedrag in &euro;</th><th>&nbsp;</th></tr>\n");
			foreach($i_rr->perrekening($reknr) as $rrrow) {
				echo("<tr>\n");
				printf("<td><input type='number' id='Regelnr_%d' class='num2' step=1 value=%d min=1></td>\n", $rrrow->RecordID, $rrrow->Regelnr);
				$jsob = "";
				if (strlen($rrrow->OMSCHRIJV) == 0) {
					$jsob = " onBlur='this.form.submit();'";
				}
				printf("<td><input type='text' id='KSTNPLTS_%d' value='%s' class='w7' maxlength=7%s></td>\n", $rrrow->RecordID, $rrrow->KSTNPLTS, $jsob);
				printf("<td id='Naam_%d'>%s</td>\n", $rrrow->RecordID, $rrrow->NaamLid);
				printf("<td><input type='text' id='OMSCHRIJV_%d' class='w70' value=\"%s\" maxlength=70></td>\n", $rrrow->RecordID, $rrrow->OMSCHRIJV);
				printf("<td><input type='text' id='Bedrag_%d' value='%s' class='d8'></td>\n", $rrrow->RecordID, number_format($rrrow->Bedrag, 2, ",", ""));
				printf("<td id='Delete_%1\$d'><i class='bi bi-trash' onClick='verw_rekeningregel(%1\$d);'></i></td>", $rrrow->RecordID);
				echo("</tr>\n");
			}
			echo("</table>\n");
			$opt = "<option value=-1>Regel(s) toevoegen ...</option><option value=0>Zonder lid</option>\n";
			$opt .= sprintf("<option value=-1 disabled>-- Gezin --</option>\n%s", $i_lid->htmloptions(0, 5, sprintf("L.Postcode='%s' AND L.Adres='%s'", $i_rk->postcode, $i_rk->adres)));
			$opt .= "<option value=-1 disabled>-- Alle leden --</option>\n";
			$opt .= $i_lid->htmloptions(0, 5);
			printf("<select name='RRnieuw' onChange='this.form.submit();'>\n%s</select>\n", $opt);
			echo("</div> <!-- Einde rekeningregelsmuteren -->\n");
		}
		echo("</form>\n");
		
		echo("<div id='opdrachtknoppen'>\n");
		printf("<button type='button' name='Sluiten' onClick=\"location.href='%s?tp=%s/Beheer'\"><i class='bi bi-door-closed'></i> Sluiten</button>\n", $_SERVER['PHP_SELF'], $currenttab);
		
		$f = sprintf("Nummer < %d", $reknr);
		$prev_rek = $i_rk->max("RK.Nummer", $f);
		$f = sprintf("Nummer > %d", $reknr);
		$next_rek = $i_rk->min("RK.Nummer", $f);
		
		if ($prev_rek > 0) {
			printf("<button type='button' name='VorigeRekening' onClick=\"location.href='%s?tp=%s/Muteren&p_reknr=%d'\"><i class='bi bi-skip-backward-circle'></i> Vorige rekening</button>\n", $_SERVER['PHP_SELF'], $currenttab, $prev_rek);
		}
		if ($next_rek > 0) {
			printf("<button type='button' name='VolgendeRekening' onClick=\"location.href='%s?tp=%s/Muteren&p_reknr=%d'\"><i class='bi bi-skip-forward-circle'></i> Volgende rekening</button>\n", $_SERVER['PHP_SELF'], $currenttab, $next_rek);
		}
		
		$f = sprintf("RR.Rekening=%d", $reknr);
		if ($i_rr->aantal($f) > 0) {
			printf("<button type='button' onClick=\"location.href='%s?tp=%s&op=preview_rek&p_reknr=%d'\"><i class='bi bi-eye'></i> Bekijk rekening</button>\n", $_SERVER['PHP_SELF'], $currenttab, $reknr);
			printf("<button type='button' onClick=\"location.href='%s?tp=%s&op=send_rek&p_reknr=%d'\"><i class='bi bi-envelope-at'></i> Verstuur rekening</button>\n", $_SERVER['PHP_SELF'], $currenttab, $reknr);
		}
		
		
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		
		echo("</div> <!-- einde rekeningmuteren  -->\n");

		printf("<script>
			$( document ).ready(function() {
				rekeningprops();
			});

			\$(\"#rekeningkopmuteren > input, #rekeningkopmuteren > select\").blur(function(){
				savedata('rekeningedit', %1\$d, this);
				rekeningprops();
			});
			
			\$(\"#rekeningregelsmuteren tr td input\").blur(function(){
				savedata('rekregedit', 0, this);
				rekeningprops();
			});
			
			\$(\"#rekeningregelsmuteren tr td input[type='checkbox']\").click(function(){
				savedata('rekregedit', 0, this);
				rekeningprops();
			});
			
			function verw_rekeningregel(rrid) {
				$('#Regelnr_' + rrid).prop('disabled', true);
				$('#KSTNPLTS_' + rrid).prop('disabled', true);
				$('#Naam_' + rrid).addClass('deleted');
				$('#OMSCHRIJV_' + rrid).prop('disabled', true);
				$('#Bedrag_' + rrid).prop('disabled', true);
				$('#Delete_' + rrid).hide();
				deleterecord('verw_rekregel', rrid);
				rekeningprops();
			}
			
		</script>\n", $reknr);
	}

}  # fnRekeningMuteren

function RekeningenAanmaken() {
	
	$i_sz = new cls_Seizoen();
	$i_rk = new cls_Rekening();
	$i_lm = new cls_lidmaatschap();
	$i_lid = new cls_Lid();
	$i_rk = new cls_Rekening();
	$i_rr = new cls_Rekeningregel();
	
	$seizoen = $_POST['seizoen'] ?? $i_sz->max("Nummer");
	if (isset($_POST['eerstenummer']) and strlen($_POST['eerstenummer']) > 3) {
		$eerstenummer = $_POST['eerstenummer'];
		if ($eerstenummer <= $i_rk->max("Nummer")) {
			$eerstenummer = $i_rk->max("Nummer")+2;
		}
	} else {
		$eerstenummer = $i_rk->max("Nummer")+2;
		if ($eerstenummer < ($seizoen * 1000) + 1) {
			$eerstenummer = ($seizoen * 1000) + 1;
		}
	}
	
	echo("<div id='aanmakenrekeningen'>\n");
	$szrow = $i_sz->record($seizoen);
	if ($szrow->RekeningenVerzamelen == 1) {
		$orderby = "L.Postcode, L.Adres, L.RekeningBetaaldDoor, L.GEBDATUM, L.Roepnaam";
	} else {
		$orderby = "L.RecordID";
	}
	if ($_SERVER['REQUEST_METHOD'] == "POST" and isset($_POST['RekAanmaken'])) {
		if (isset($_POST['sure']) and $_POST['sure'] == "1") {
			$f = sprintf("LM.Ingevoerd >= '%s'", $_POST['lmingevoerdna']);
			$lidrows = $i_lid->ledenlijst(1, -1, $orderby, $f, 1);
			$vgezin = "0000 ZZ";
			$agl = 0;
			$reknr = intval($_POST['eerstenummer']);
			if ($reknr <= $i_rk->max("Nummer")) {
				$i_rk->max("Nummer") + 1;
			}
			if (($reknr / 2) == intval($reknr / 2)) {
				$reknr++;
			}
			$aantrek = 0;
			$aantregels = 0;
			foreach ($lidrows as $lidrow) {
				$gezin = sprintf("%s %s %d", $lidrow->Postcode, $lidrow->Adres, $lidrow->RekeningBetaaldDoor);
				if ($gezin != $vgezin and $agl > 1) {
					if ($szrow->{'Gezinskorting bedrag'} > 0) {
						$rrid = $i_rr->add($reknr, 0, $szrow->{'Gezinskorting kostenplaats'});
						$i_rr->update($rrid, "OMSCHRIJV", $szrow->{'Gezinskorting omschrijving'});
						$i_rr->update($rrid, "Bedrag", $szrow->{'Gezinskorting bedrag'} * ($agl-1));
						$aantregels++;
					}
					$i_rk->update($reknr, "DEBNAAM", $debnaam);
				}
				if ($gezin != $vgezin or $szrow->RekeningenVerzamelen == 0) {
					$reknr = $i_rk->add($reknr, $seizoen, $lidrow->RecordID);
					$agl = 1;
					$debnaam = $i_lid->naam($lidrow->RecordID);
					$vnm = $lidrow->Achternaam . ", " . $lidrow->TUSSENV;
					$aantrek++;
				} else {
					if ($agl == 1) {
						$debnaam = " en " . $debnaam;
					} elseif ($agl > 1) {
						$debnaam = ", " . $debnaam;
					}
					if ($vnm == $lidrow->Achternaam . ", " . $lidrow->TUSSENV) {
						$debnaam = $lidrow->Roepnaam . $debnaam;
					} else {
						$debnaam = $lidrow->NaaLid . $debnaam;
					}
					$agl++;
				}
				$aantregels += $i_rr->standaardwaarde($reknr, $lidrow->RecordID);
				$vgezin = $gezin;
			}
			$i_rk->controle(-1, $seizoen);
			printf("<p class='mededeling'>%d rekeningen aangemaakt met in totaal %d regels.<p>", $aantrek, $aantregels);
		} else {
			echo("<p class='mededeling'>Vink de checkbox voor 'Rekeningen aanmaken' aan, om de rekeningen aan te maken.</p>\n");
		}
	} elseif ($_SERVER['REQUEST_METHOD'] == "POST") {
		$f = sprintf("LM.Ingevoerd >= '%s'", $_POST['lmingevoerdna']);
		$lidrows = $i_lid->ledenlijst(1, -1, $orderby, $f);
		if (count($lidrows) > 0) {
			printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
			printf("Voor de volgende %d leden zal een rekening worden aangemaakt:\n", count($lidrows));
				
			echo("<ul>\n");
			foreach($lidrows as $lidrow) {
				printf("<li>%s</li>\n", $lidrow->NaamLid);
			}
			echo("</ul>\n<div class='clear'></div>\n");
			echo("<div id='opdrachtknoppen'>\n");
			echo("<input type='checkbox' value='1' name='sure'>\n");
			echo("<input type='submit' value='Rekeningen aanmaken' name='RekAanmaken'>\n");
			printf("<input type='hidden' name='lmingevoerdna' value='%s'>\n", $_POST['lmingevoerdna']);
			printf("<input type='hidden' name='eerstenummer' value='%s'>\n", $_POST['eerstenummer']);
			echo("<input type='button' value='Terug' onClick='history.go(-1);'>\n");
			echo("</div>  <!-- Einde opdrachtknoppen -->\n");
			echo("</form>\n");
		} else {
			echo("<p class='mededeling'>Er zijn geen leden geselecteerd.</p>\n");
		}
	
	} else {
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
	
		printf("<label>Seizoen</label><select name='seizoen' onChange='this.form.submit();'>%s</select>\n", $i_sz->htmloptions($seizoen));

		printf("<label>Omschrijving rekening</label><input type='text' id='Rekeningomschrijving' maxlength=35 class='w35' value='%s'>\n", $szrow->Rekeningomschrijving);
	
		printf("<label>Datum rekening</label><input type='date' name='Rekeningdatum' value='%s'>\n", date("Y-m-d"));
		printf("<label class='k2'>Eerste nummer rekening</label><input type='number' name='eerstenummer' value=%d>\n", $eerstenummer);
		printf("<label class='k2'>Verzamelen per gezin?</label><input type='checkbox' id='RekeningenVerzamelen' %s>\n", checked($szrow->RekeningenVerzamelen));
		
		printf("<label>Lidmaatschap ingevoerd na</label><input type='date' name='lmingevoerdna' value='%s'>\n", substr($i_lm->min("Ingevoerd"), 0, 10));
	
		printf("<label>Omschrijving verenigingscontributie</label><input type='text' id='Verenigingscontributie omschrijving' maxlength=50 class='w50' value='%s'>\n", $szrow->{'Verenigingscontributie omschrijving'});
		printf("<label class='k2'>Kostenplaats</label><input type='text' id='Verenigingscontributie kostenplaats' maxlength=12 class='w12' value='%s'>\n", $szrow->{'Verenigingscontributie kostenplaats'});
		
		printf("<label>Omschrijving gezinskorting</label><input type='text' id='Gezinskorting omschrijving' maxlength=50 class='w50' value='%s'>\n", $szrow->{'Gezinskorting omschrijving'});
		printf("<label class='k2'>Kostenplaats gezinskorting</label><input type='text' id='Gezinskorting kostenplaats' maxlength=12 class='w12' value='%s'>\n", $szrow->{'Gezinskorting kostenplaats'});
		printf("<label class='k2'>Bedrag gezinskorting</label><input type='text' id='Gezinskorting bedrag' class='d8' value='%s'>\n", $szrow->{'Gezinskorting bedrag'});
	
		echo("<div id='opdrachtknoppen'>\n");
		echo("<button type='submit'>Verder</button>\n");
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
	
		echo("</form>\n");
	
		printf("<script>
			$('input').blur(function() {
				savedata('seizoenedit', %1\$d, this);
			});
		</script>\n", $seizoen);
	}
	echo("</div> <!-- Einde aanmakenrekeningen -->\n");
	
}  # RekeningenAanmaken

function RekeningBetalingen() {
	$i_rb = new cls_RekeningBetaling();
	$i_rk = new cls_Rekening();
	
	echo("<div id='rekeningbetalingen'>\n");
	
	printf("<label>Rekeningnummer</label><input type='number' value=0 id='nieuw_rekening'>");
	echo("<p id='opmerking'></p>\n");
	printf("<label>Datum betaling</label><input type='date' id='nieuw_datum' value='%s'>\n", $i_rb->max("Datum"));
	echo("<label>Bedrag</label><input type='text' id='nieuw_bedrag' value='0,00' class='d8'>\n");
	echo("<button type='button' id='btnBetalingToevoegen' onClick='fnBetalingToevoegen();' disabled>Betaling toevoegen</button>\n");
	echo("<button type='button' onClick='location.reload();'>Ververs scherm</button>\n");

	echo("<div class='clear'></div>\n");

	printf("<table id='betalingen' class='%s'>\n", TABLECLASSES);
	echo("<caption>Laatste betalingen</caption>\n");
	echo("<thead><tr><th>Rekening</th><th>Datum</th><th>Bedrag</th><th></th></tr></thead>\n");
	echo("<tbody>\n");
	foreach ($i_rb->laatstebetalingen() as $row) {
		$del = sprintf("<i class='bi bi-trash' onClick='verw_betaling(%d);'></i>", $row->RecordID);
		printf("<tr><td class='number'>%1\$d</td><td>%2\$s</td><td id='bedrag_%3\$d'>%4\$.2f</td><td id='trash_%3\$d'>%5\$s</td></tr>\n", $row->Rekening, $row->Datum, $row->RecordID, $row->Bedrag, $del);
	}
	echo("</tbody>\n");	
	echo("</table>\n");
	echo("</div> <!-- Einde rekeningbetalingen -->\n");
	
	?>
	<script>
	
	$("#nieuw_rekening").blur(function() {
		var rkid = $("#nieuw_rekening").val();
		if (rkid > 0) {
			$.ajax({
				url: 'ajax_update.php?entiteit=rekeningdetails',
				type: 'post',
				dataType: 'json',
				data: { id: rkid },
				success: function(response){
					if (response.rkid > 0) {
						$("#opmerking").html(response.debnaam);
						$("#nieuw_bedrag").val(response.open);
						
						$("#nieuw_datum").prop('disabled', false);
						$("#nieuw_bedrag").prop('disabled', false);
						$("#btnBetalingToevoegen").prop('disabled', false);
					} else {
						$("#opmerking").html("Rekening bestaat niet");
						$("#nieuw_datum").prop('disabled', true);
						$("#nieuw_bedrag").prop('disabled', true);
						$("#btnBetalingToevoegen").prop('disabled', true);
					}
				},
				fail: function( data, textStatus ) {
					alert('Rekeningdetails ophalen is mislukt. ' + textStatus);
				}
			});
		} else {
			$("#opmerking").html("");
			$("#nieuw_datum").prop('disabled', true);
			$("#nieuw_bedrag").prop('disabled', true);
			$("#btnBetalingToevoegen").prop('disabled', true);
		}
	});
	
	function fnBetalingToevoegen() {
		var rkid = $("#nieuw_rekening").val();
		var btdat = $("#nieuw_datum").val();
		var bedr = $("#nieuw_bedrag").val().replace(",", ".");
		
		let cur = Intl.NumberFormat('nl-NL', {
			minimumFractionDigits: 2,      
			maximumFractionDigits: 2,
		});
		
		$("#betalingen > tbody > tr:first").before("<tr><td class='number'>" + rkid + "</td><td>" + btdat + "</td><td class='number'>" + cur.format(bedr) + "</td></tr>");
		
		$.ajax({
			url: 'ajax_update.php?entiteit=add_betaling',
			type: 'post',
			dataType: 'json',
			data: { id: rkid, datum: btdat, bedr: bedr }
		});
	}
	
	function verw_betaling(rbid) {
		
		$("#bedrag_" + rbid).addClass('deleted');
		$("#trash_" + rbid).hide();
		
		$.ajax({
			url: 'ajax_update.php?entiteit=del_betaling',
			type: 'post',
			dataType: 'json',
			data: { id: rbid }
		});
		
	}
	
	</script>
	<?php
}

function RekeningInstellingen() {
	$i_ond = new cls_Onderdeel();
	$i_mv = new cls_Mailing_vanaf();
	
	echo("<div id='instellingenmuteren'>\n");
	
	echo("<h2>Algemene instellingen</h2>\n");
	printf("<label>Groep rekening betaald door</label><select id='rekening_groep_betaalddoor'>\n<option value=-1>Alleen webmasters</option>\n%s</select>\n", $i_ond->htmloptions($_SESSION['settings']['rekening_groep_betaalddoor'], 1));
	printf("<label>Bewaartermijn (na rekeningdatum)</label><input type='number' id='rekening_bewaartermijn' value=%d class='num2'><p>(maanden)</p>", $_SESSION['settings']['rekening_bewaartermijn']);
	

	echo("<h2>Instellingen voor mailen</h2>\n");
	echo("<label>Rekening versturen aan</label><select id='mailing_rekening_stuurnaar'>\n");
	$sn[1] = "Alleen betaald door";
	$sn[2] = "Betaald door en alle volwassenen op de rekening";
	$sn[3] = "Betaald door en alle leden op de rekening";
	$sn[4] = "Betaald door en het gekoppelde lid, indien ongelijk.";
	foreach($sn as $key => $val) {
		printf("<option value=%d%s>%s</option>>\n", $key, checked($key, "option", $_SESSION['settings']['mailing_rekening_stuurnaar']), $val);
	}
	echo("</select>\n");
		
	printf("<label>Vanaf e-mailadres</label><select id='mailing_rekening_vanafid'>%s</select>\n", $i_mv->htmloptions($_SESSION['settings']['mailing_rekening_vanafid']));

	printf("<label>Verstuurde e-mails alleen zichtbaar voor</label><select id='mailing_rekening_zichtbaarvoor'>\n<option value=-1>Alleen webmasters</option>\n%s</select>\n", $i_ond->htmloptions($_SESSION['settings']['mailing_rekening_zichtbaarvoor'], 1));
	echo("</div> <!-- Einde instellingenmuteren -->\n");
?>
<script>
	$("select").change(function() {
		saveparam(this);
	});
	
	$("input").focusout(function() {
		saveparam(this);
	});
	
</script>
<?php
	
}  # RekeningInstellingen

function RekeningDetail($p_rkid, $p_metbriefpapier=0) {
	global $dtfmt;
	
	$i_rk = new cls_Rekening($p_rkid);
	$i_tp = new cls_Template();
	
	$i_rk->controle($p_rkid);
	$rk = $i_rk->record($p_rkid);
	
	$i_tp->vulvars(-1, sprintf("rekening %d", $i_rk->seizoen));
	if (strlen($i_tp->inhoud) == 0) {
		$i_tp->vulvars(-1, "rekening");
	}
	
	if (strlen($i_tp->inhoud) >  0) {
		$content = $i_tp->inhoud;
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
<!-- MetRegelnummers=0 -->
<!-- MetNulRegels=1 -->
<table>
<tr><th>Regel</th><th>Omschrijving</th><th>Lid</th><th class='number'>Bedrag in &euro;</th></tr>
[%REKENINGREGELS%]
<tr><th colspan=3>Totaal</th><th class='number'>[%REKENINGBEDRAG%]</th><tr>
</table>
</div> <!-- Einde rekeningregels -->

<!-- NietOpCredit -->
<!-- NietOpNulRekening -->
<p>Deze rekening dient op [%UITERSTEBETAALDATUM%], onder vermelding van rekeningnummer [%REKENINGNUMMER%], volledig betaald te zijn.</p>

<!-- AlleenOpMeerdereTermijnen -->
<p>U mag deze rekening in meerdere termijnen betalen volgens onderstaand scherm.</p>
<table id='betaaltermijnen'>
<tr><th>#</th><th>Betalen voor</th><th>Bedrag in &euro;</th></tr>
[%BETAALTERMIJNEN%]
</table>
<!-- /AlleenOpMeerdereTermijnen -->

<p>Indien u een andere betalingsregeling wenst of een vraag over deze rekening heeft kunt u contact met de <a href='mailto:[%VANAFADRES%]'>[%VANAFNAAM%]</a> opnemen.</p>
<!-- /NietOpNulRekening -->
<!-- /NietOpCredit -->

<!-- NietOpDebet -->
<!-- NietOpNulRekening -->
<p>Bovenstaand bedrag wordt zo spoedig mogelijk aan u overgemaakt.</p>
<!-- /NietOpNulRekening -->
<!-- /NietOpDebet -->

<!-- NietOpDebet -->
<p>Indien u een vraag over deze rekening heeft kunt u contact met de <a href='mailto:[%VANAFADRES%]'>[%VANAFNAAM%]</a> opnemen.</p>
<!-- /NietOpDebet -->

<p><strong>Het bestuur van [%NAAMVERENIGING%]</strong></p>\n";
	}
	
	$n = "<!-- MetRegelnummers=";
	$p = strpos($content, $n);
	$metregelnr = 1;
	if ($p > 1) {
		$p += strlen($n);
		$metregelnr = intval(substr($content, $p, 1));
	}
	
	$n = "<!-- MetNulRegels=";
	$p = strpos($content, $n);
	$metnulregels = 1;
	if ($p > 1) {
		$p += strlen($n);
		$metnulregels = intval(substr($content, $p, 1));
	}
	
	if ($rk->Machtiging == 1) {
		$content = removetextblock($content, "<!-- Geen machtiging -->", "<!-- /Geen machtiging -->");
	} else {
		$content = removetextblock($content, "<!-- Wel machtiging -->", "<!-- /Wel machtiging -->");
	}
	
	if ($rk->Bedrag == 0) {
		$content = removetextblock($content, "<!-- NietOpNulRekening -->", "<!-- /NietOpNulRekening -->");
	} elseif ($rk->Bedrag < 0) {
		$content = removetextblock($content, "<!-- NietOpCredit -->", "<!-- /NietOpCredit -->");
	} else {
		$content = removetextblock($content, "<!-- NietOpDebet -->", "<!-- /NietOpDebet -->");
		if ($rk->Betaald == 0) {
			$content = removetextblock($content, "<!-- NietOpNulBetaald -->", "<!-- /NietOpNulBetaald -->");
		}
	}
	
	if ($rk->Bedrag <= 0 or $rk->Lid == $rk->BetaaldDoor) {
		$content = removetextblock($content, "<!-- AlleenOpBetaaldDoorAnder -->", "<!-- /AlleenOpBetaaldDoorAnder -->");
	}
	
	if ($rk->Openstaand < 0.20) {
		$content = removetextblock($content, "<!-- NietOpVolledigBetaald -->", "<!-- /NietOpVolledigBetaald -->");
	}
	
	if ($rk->BET_TERM > 1) {
		$content = removetextblock($content, "<!-- NietOpMeerdereTermijnen -->", "<!-- /NietOpMeerdereTermijnen -->");
	}
	
	if ($rk->BET_TERM < 2) {
		$content = removetextblock($content, "<!-- AlleenOpMeerdereTermijnen -->", "<!-- /AlleenOpMeerdereTermijnen -->");
	}
	
	$dtfmt->setPattern(DTTEXT);
	$content = str_ireplace("[%NAAMDEBITEUR%]", $rk->Tenaamstelling, $content);
	$content = str_ireplace("[%ADRES%]", $rk->Adres, $content);
	$content = str_ireplace("[%POSTCODE%]", $rk->Postcode, $content);
	$content = str_ireplace("[%WOONPLAATS%]", $rk->Woonplaats, $content);
	$content = str_ireplace("[%LIDNR%]", $rk->Lidnr, $content);
	$content = str_ireplace("[%LIDID%]", $rk->LidID, $content);
	$content = str_ireplace("[%REKENINGDATUM%]", $dtfmt->format(strtotime($rk->Datum)), $content);
	$content = str_ireplace("[%SEIZOEN%]", $rk->Seizoen, $content);
	$content = str_ireplace("[%REKENINGOMSCHRIJVING%]", $rk->OMSCHRIJV, $content);
	$content = str_ireplace("[%REKENINGNUMMER%]", $rk->Nummer, $content);
	$content = str_ireplace("[%REKENINGBEDRAG%]", fnBedrag($rk->Bedrag), $content);
	$content = str_ireplace("[%BETAALD%]", fnBedrag($rk->Betaald), $content);
	$content = str_ireplace("[%OPENSTAAND%]", fnBedrag($rk->Openstaand), $content);
	$content = str_ireplace("[%BANKREKENING%]", $rk->Bankrekeningnummer, $content);
	$content = str_ireplace("[%UITERSTEBETAALDATUM%]", $dtfmt->format(strtotime($rk->UitersteBetaaldatum)), $content);
	$content = str_ireplace("[%EINDEEERSTETERMIJN%]", $dtfmt->format(strtotime($rk->EindeEersteTermijn)), $content);
	
	$content = str_ireplace("[%NAAMBETAALDDOOR%]", $rk->NaamBetaaldDoor, $content);
	$content = str_ireplace("[%EMAILBETAALDDOOR%]", $rk->EmailBetaaldDoor, $content);
	
	$i_mv = new cls_Mailing_vanaf($_SESSION['settings']['mailing_rekening_vanafid']);
	$content = str_ireplace("[%VANAFADRES%]", $i_mv->vanaf_email, $content);
	$content = str_ireplace("[%VANAFNAAM%]", $i_mv->vanaf_naam, $content);
	$i_mv = null;
	$content = str_ireplace("[%NAAMVERENIGING%]", $_SESSION['settings']['naamvereniging'], $content);
	$content = str_ireplace("[%NAAMWEBSITE%]", $_SESSION['settings']['naamwebsite'], $content);
	$content = str_ireplace("[%URLWEBSITE%]", $_SERVER["HTTP_HOST"], $content);
	
	$rr = (new cls_Rekeningregel())->perrekening($rk->Nummer);
	$rrtxt = "";
	$ireg = 1;
	foreach($rr as $regel) {
		if ($regel->Lid > 0) {
			$nl = (new cls_Lid())->Naam($regel->Lid, "");
		} else {
			$nl = "";
		}
		if ($regel->Bedrag <> 0 or $metnulregels == 1) {
			$rrtxt .= "<tr>";
			if ($metregelnr == 1) {
				$rrtxt .= sprintf("<td>%d</td>", $ireg);
			}
			$rrtxt .= sprintf("<td>%s</td><td>%s</td><td class='number'>%s</td></tr>\n", $regel->OMSCHRIJV, $nl, fnBedrag($regel->Bedrag));
			$ireg += 1;
		}
	}
	$content = str_ireplace("[%REKENINGREGELS%]", $rrtxt, $content);
	
	$bttxt = "";
	$tb = 0;
	$bv = new DateTime($rk->Datum);
	
	for ($bt=1;$bt<=$rk->BET_TERM;$bt++) {
		if ($bt < $rk->BET_TERM) {
			$bedr = round(($rk->Bedrag / $rk->BET_TERM), 2);
			$tb += $bedr;
		} else {
			$bedr = round(($rk->Bedrag - $tb), 2);
		}
		$bv->modify(sprintf("+%d day", $rk->BETAALDAG));
		
		$bttxt .= sprintf("<tr><td>%d</td><td>%s</td><td class='number'>%s</td></tr>\n", $bt, $dtfmt->format($bv), fnBedrag($bedr));
	}
	$content = str_ireplace("[%BETAALTERMIJNEN%]", $bttxt, $content);
	
	$bestand_briefpapier = $_SESSION['settings']['path_templates'] . "briefpapier.html";
	$i_tp->vulvars(-1, "briefpapier");
	if ($p_metbriefpapier == 1 and strlen($i_tp->inhoud) > 0) {
		
		$rv = removetextblock($i_tp->inhoud, "<!-- Aanhef -->", "<!-- /Aanhef -->");
		$rv = str_ireplace("[%SUBJECT%]", "Rekening " . $p_rkid, $rv);
		$rv = str_ireplace("[%MESSAGE%]", $content, $rv);
		
	} else {
		$rv = $content;
	}
	
	return $rv;

} # RekeningDetail

function fnBedrag($bedrag) {
	if (strlen($_SESSION['settings']['mailing_rekening_valuta']) == 0) {
		return str_replace(",00", ",=&nbsp;", sprintf("%03.2f", $bedrag));
	} else {
		return $_SESSION['settings']['mailing_rekening_valuta'] . "&nbsp;" . str_replace(",00", ",=&nbsp;", sprintf("%03.2f", $bedrag));
	}	
}  #fnBedrag

?>