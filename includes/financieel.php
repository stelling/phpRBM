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
	} elseif ($currenttab2 == "Muteren") {
		fnRekeningMuteren($reknr);
	} elseif ($currenttab2 == "Beheer") {
		rekeningbeheer();
		
	} elseif ($currenttab2 == "Nieuw") {
		
		$seizoen = $_POST['nwseizoen'] ?? -1;
		if ($seizoen <= 0) {
			$seizoen = (new cls_Seizoen())->zethuidige(date("Y-m-d"));
		}
		
		if ($_SERVER['REQUEST_METHOD'] == "POST") {
			if (isset($_POST['RekToevoegen'])) {
				if ($_POST['nwlid'] <= 0) {
					echo("<p class='mededeling'>Selecteer eerst een (Klos)lid.</p>\n");
				} else {
					$reknr = (new cls_Rekening())->add($_POST['nwrekening'], $seizoen, $_POST['nwlid']);
					printf("<script>location.href='%s?tp=%s/Muteren&p_reknr=%d'</script>\n", $_SERVER['PHP_SELF'], $currenttab, $reknr);
				}
			}
			
		} else {
			$nwreknr=(new cls_Rekening())->nieuwrekeningnr($seizoen);

			$actionurl = sprintf("%s?tp=%s", $_SERVER['PHP_SELF'], $_GET['tp']);
			printf("<form method='post' id='rekeningkopmuteren' action='%s'>\n", $actionurl);
		
			printf("<label class='form-label'>Seizoen</label><select name='nwseizoen' class='form-select form-select-sm'>\n%s</select>\n", (new cls_Seizoen())->htmloptions($seizoen));
			printf("<label class='form-label'>Rekeningnummer</label><input type='number' name='nwrekening' value=%d class='d8'>\n", $nwreknr);
			printf("<label class='form-label'>Lid</label><select name='nwlid' class='form-select form-select-sm'><option value=0>Selecteer lid ...</option>\n%s</select>\n", (new cls_Lid())->htmloptions(-1, 2));
			echo("<div class='clear'></div>\n");
			printf("<button type='submit' class='%s' name='RekToevoegen'>%s Toevoegen</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
			echo("</form>\n");
		}
		
	} elseif ($currenttab2 == "Aanmaken rekeningen") {
		rekeningenaanmaken();
		
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
}  # fnRekeningen

function rekeningbeheer() {
	global $currenttab;
	
	$i_rk = new cls_Rekening();
	$i_sz = new cls_Seizoen();
	
	if (isset($_GET['op']) and $_GET['op'] == "deleterekening" and $_GET['p_reknr'] > 0) {
		$i_rk->delete($_GET['p_reknr']);
	}

	$filterseizoen = intval($_GET['filterseizoen'] ?? $i_rk->Max("RK.Seizoen"));
	$filternaam = $_GET['tbFilterNaam'] ?? "";
	
	$rows = $i_rk->overzichtbeheer($filterseizoen);
	
	echo("<form method='GET' id='filter'>\n");
	printf("<input type='hidden' name='tp' value='%s'>\n", $_GET['tp']);
	printf("<select name='filterseizoen' class='form-select form-select-sm' title='Selecteer seizoen' onChange='this.form.submit();'>\n<option value=-1>Alle seizoenen</option>\n%s</select>\n", $i_sz->htmloptions($filterseizoen, 1));
	printf("<input type='text' id='tbFilterNaam' name='tbFilterNaam' placeholder='Tekstfilter' value='%s' OnKeyUp=\"fnFilter('%s', this);\">\n", $filternaam, __FUNCTION__);
	if (count($rows) > 1) {
		printf("<p class='aantrecords'>%d rekeningen</p>", count($rows));
	}
	echo("</form>\n");
	
	$l = sprintf("%s?tp=%s/Muteren&p_reknr=%%d", $_SERVER['PHP_SELF'], $currenttab);
	$kols[] = array('columnname' => "Nummer", 'link' => $l, 'class' => "muteren");
	$kols[] = array('columnname' => "Nummer", 'type' => "integer");
	$kols[] = array('columnname' => "Datum", 'type' => "date");
	$kols[] = array('columnname' => "Omschrijving", 'columntitle' => "OpmerkingIntern");
	$kols[] = array('columnname' => "Tenaamstelling");
	$kols[] = array('columnname' => "Bedrag", 'type' => "bedrag");

	$f = sprintf("RK.Seizoen=%d", $filterseizoen);
	if ($i_rk->max("RK.Betaald", $f) > 0) {
		$kols[] = array('columnname' => "Betaald", 'type' => "bedrag");
	}
	
	$adl = 0;
	foreach ($rows as $row) {
		if ($row->linkDelete > 0) {
			$adl++;
		}
	}
	
	if ($adl > 0) {
		$l = sprintf("%s?tp=%s/Beheer&op=deleterekening&p_reknr=%%d", $_SERVER['PHP_SELF'], $currenttab);
		$kols[7] = array('link' => $l, 'class' => 'trash', 'columnname' => "linkDelete");
	}
	
	if (count($rows) > 0) {
		echo(fnDisplayTable($rows, $kols, "", 0, "", __FUNCTION__));
		//overzichtrekeningen
	}
	
	printf("<script>
				$(document).ready(function() {
					fnFilter('%s', '%s');
				});
		  </script>\n", __FUNCTION__, $filternaam);
	
}  # rekeningbeheer

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
	$i_rb = new cls_RekeningBetaling();
	$i_lid = new cls_Lid($i_rk->lidid, $i_rk->datum);
	$i_seiz = new cls_Seizoen();
	$i_ond = new cls_Onderdeel();
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['btnStandaardRegels']) and $_POST['btnStandaardRegels'] > 0) {
			$at = $i_rr->toevoegenstandaardregels($reknr, $_POST['btnStandaardRegels']);
		} elseif (isset($_POST['NieuweRegelAnderLid']) and $_POST['NieuweRegelAnderLid'] > 0) {
			$i_rr->add($reknr, $_POST['NieuweRegelAnderLid']);
		} elseif (isset($_POST['NieuweRegel']) and $_POST['NieuweRegel'] >= 0) {
			$i_rr->add($reknr, $_POST['NieuweRegel']);
		} elseif (isset($_POST['verstuurrekening']) and $_POST['verstuurrekening'] > 0) {
			fnVerstuurRekening($_POST['verstuurrekening'], 1);
		}
	}
	
	if ($scherm == "M" and $reknr > 0)  {
		$i_rk->vulvars($reknr);
		if ($i_rk->aantalledenoprekening == 1) {
			$f = sprintf("RR.Rekening=%d AND RR.Lid > 0", $reknr);
			$i_rk->update($reknr, "Lid", $i_rr->min("RR.Lid", $f));
		}
		
		echo("<div id='rekeningmuteren'>\n");
		$actionurl = sprintf("%s?tp=%s&rkid=%d", $_SERVER['PHP_SELF'], $_GET['tp'], $reknr);
		printf("<form method='post' action='%s'>\n", $actionurl);
		echo("<div id='rekeningkopmuteren'>\n");
		
		printf("<label class='form-label'>Rekeningnummer</label><p id='reknr'>%d</p>\n", $i_rk->rkid);
		printf("<input type='hidden' name='rkid' value=%d>\n", $i_rk->rkid);
		
		echo("<div id='rekeninginfo'>\n");
		printf("<label class='form-label'>Totaal bedrag &euro;</label><p id='rekeningbedrag'>%03.2f</p>\n", $i_rk->bedrag);
		if ($i_rb->aantal() > 0) {
			printf("<label class='form-label' id='lblbedragbetaald'>Betaald &euro;</label><p id='bedragbetaald'>%03.2f</p>\n", $i_rk->betaald);
		}
		echo("<label class='form-label' id='lbluitersteBetaling'>Uiterste betaling op</label><p id='uitersteBetaling'></p>\n");
		echo("<label class='form-label'>Telefoon debiteur</label><p id='telefoondebiteur'></p>\n");
		echo("<label class='form-label'>E-mail debiteur</label><p id='emaildebiteur'></p>\n");
		
		echo("<label class='form-label'>Per E-mail verstuurd</label><p id='laatsteemail'></p>\n");
		echo("</div> <!-- Einde rekeninginfo -->\n");
		
		printf("<label class='form-label'>Seizoen</label><select id='Seizoen' class='form-select form-select-sm'>%s</select>\n", $i_seiz->htmloptions($i_rk->seizoen));
		printf("<label class='form-label'>Rekeningdatum</label><input type='date' id='Datum' value='%s' required>\n", $i_rk->datum);
		printf("<label class='form-label'>Omschrijving</label><input type='text' id='OMSCHRIJV' class='w35' value='%s' maxlength=35>\n", $i_rk->omschrijving);
		printf("<label class='form-label'>Tenaamstelling rekening</label><input type='text' id='DEBNAAM' class='w60' value='%s' maxlength=60>\n", $i_rk->debnaam);
		if ($i_rk->aantalledenoprekening > 1) {
			$d = "";
		} else {
			$d = " disabled";
		}
		$f = sprintf("(L.RecordID IN (SELECT RR.Lid FROM %sRekreg AS RR WHERE RR.Rekening=%d) OR L.RecordID=%d)", TABLE_PREFIX, $i_rk->rkid, $i_rk->lidid);
		printf("<label class='form-label'>Gekoppeld aan lid</label><select id='Lid' class='form-select form-select-sm'%s>\n%s</select>\n", $d, $i_lid->htmloptions($i_rk->lidid, 0, $f, $i_rk->datum));
		
		$f = sprintf("(L.RecordID IN (SELECT LO.Lid FROM %sLidond AS LO WHERE IFNULL(LO.Opgezegd, '9999-12-31') >= '%s' AND LO.OnderdeelID=%d)", TABLE_PREFIX, $i_rk->datum, $_SESSION['settings']['rekening_groep_betaalddoor']);
		$f .= sprintf(" OR L.RecordID IN (SELECT RR.Lid FROM %sRekreg AS RR WHERE RR.Rekening=%d)", TABLE_PREFIX, $reknr);
		$f .= sprintf(" OR L.RecordID=%d OR L.RecordID=%d)", $i_rk->betaalddoor, $i_rk->lidid);
		printf("<label class='form-label'>Betaald door / debiteur</label><select id='BetaaldDoor' class='form-select form-select-sm'>\n%s</select>\n", $i_lid->htmloptions($i_rk->betaalddoor, 7, $f, $i_rk->datum));
		if ($i_rk->dagenperbetaaltermijn < 1) {
			$bdt = $i_seiz->max("SZ.BetaaldagenTermijn", sprintf("SZ.Nummer=%d", $i_rk->seizoen));
		} else {
			$bdt = $i_rk->dagenperbetaaltermijn;
		}
		printf("<label class='form-label' id='lblBETAALDAG'>Aantal dagen per betaaltermijn</label><input type='text' id='BETAALDAG' value=%d class='w3' maxlength=3>\n", $bdt);
		
		if ($i_rk->aantalbetaaltermijnen < 1) {
			$bt = 1;
		} else {
			$bt = $i_rk->aantalbetaaltermijnen;
		}
		printf("<label class='form-label' id='lblBET_TERM'>Aantal betaaltermijnen</label><input type='text' id='BET_TERM' value=%d class='w2' maxlength=2>\n", $bt);
		
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
			echo("<tr><th>Regel</th><th>Kostenplaats</th><th>Lid</th><th>Omschrijving</th><th>Bedrag in &euro;</th><th>&nbsp;</th></tr>\n");
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
				printf("<td id='Delete_%1\$d' class='trash'><i class='bi bi-trash' onClick='verw_rekeningregel(%1\$d);'></i></td>", $rrrow->RecordID);
				echo("</tr>\n");
			}
			echo("</table>\n");
			$i_lid->per = $i_rk->datum;
			if ($i_rk->i_sz->gezinsrekening == 0) {
				$i_lid->where = sprintf("L.RecordID=%d", $i_rk->lidid);
			} else {
				$i_lid->where = sprintf("L.Postcode='%s' AND L.Adres='%s'", $i_rk->i_lid->postcode, $i_rk->i_lid->adres);
				if ($i_rk->betaalddoor == $i_rk->lidid) {
					$i_lid->where .= " AND IFNULL(L.RekeningBetaaldDoor, 0)=0";
				} else {
					$i_lid->where .= sprintf(" AND L.RekeningBetaaldDoor=%d", $i_rk->betaalddoor);
				}
			}
			foreach ($i_lid->ledenlijst(5) as $lrow) {
				$i_rr->where = sprintf("RR.Rekening=%d AND RR.Lid=%d", $i_rk->rkid, $lrow->RecordID);
				if ($i_rr->toevoegenstandaardregels($i_rk->rkid, $lrow->RecordID, 0) == 0) {
					printf("<button type='submit' class='%s btn-sm' name='NieuweRegel' value='%d'>%s Regel %s</button>\n", CLASSBUTTON, $lrow->RecordID, ICONTOEVOEGEN, $lrow->Roepnaam);
				} else {
					printf("<button type='submit' class='%s btn-sm' name='btnStandaardRegels' value=%d>%s Standaardregels %s</button>\n", CLASSBUTTON, $lrow->RecordID, ICONTOEVOEGEN, $lrow->Roepnaam);
				}
			}
			printf("<button type='submit' class='%s btn-sm' name='NieuweRegel' value=0 title='Regel, zonder lid, toevoegen'>%s Regel</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
			
			$i_lid->where = sprintf("L.Postcode<>'%s' AND L.Adres<>'%s'", $i_rk->i_lid->postcode, $i_rk->i_lid->adres);
			$opt = "<option value=0>Regel met lid toevoegen ...</option>\n" . $i_lid->htmloptions(0, 5);
			printf("<select name='NieuweRegelAnderLid' class='form-select' title='Regel met lid toevoegen' onChange='this.form.submit();'>\n%s</select>\n", $opt);
			
			echo("</div> <!-- Einde rekeningregelsmuteren -->\n");
		}
		
		echo("<div class='form-floating'>\n");
		printf("<textarea id='OpmerkingIntern' class='form-control' title='Interne opmerking' placeholder='Interne opmerking'>%s</textarea>\n", $i_rk->opmerkingintern);
		echo("<label for='OpmerkingIntern'>Interne opmerking</label>");
		echo("</div>\n");
		
		
		echo("<div id='opdrachtknoppen'>\n");
		printf("<button type='button' class='%s' name='Sluiten' onClick=\"location.href='%s?tp=%s/Beheer'\">%s Sluiten</button>\n", CLASSBUTTON, $_SERVER['PHP_SELF'], $currenttab, ICONSLUIT);
		
		$f = sprintf("RK.Nummer < %d", $reknr);
		$prev_rek = $i_rk->max("Nummer", $f);
		$f = sprintf("RK.Nummer > %d", $reknr);
		$next_rek = $i_rk->min("Nummer", $f);
		
		if ($prev_rek > 0) {
			printf("<button type='button' class='%s' name='VorigeRekening' onClick=\"location.href='%s?tp=%s/Muteren&p_reknr=%d'\">%s Vorige rekening</button>\n", CLASSBUTTON, $_SERVER['PHP_SELF'], $currenttab, $prev_rek, ICONVORIGE);
		}
		if ($next_rek > 0) {
			printf("<button type='button' class='%s' name='VolgendeRekening' onClick=\"location.href='%s?tp=%s/Muteren&p_reknr=%d'\">%s Volgende rekening</button>\n", CLASSBUTTON, $_SERVER['PHP_SELF'], $currenttab, $next_rek, ICONVOLGENDE);
		}
		
		$i_rr->where = sprintf("RR.Rekening=%d", $reknr);
		if ($i_rr->aantal() > 0) {
			printf("<button type='button' class='%s' onClick=\"location.href='%s?tp=%s&op=preview_rek&p_reknr=%d'\">%s Bekijk rekening</button>\n", CLASSBUTTON, $_SERVER['PHP_SELF'], $currenttab, $reknr, ICONVOORBEELD);
			printf("<button type='submit' class='%s' name='verstuurrekening' value=%d>%s Verstuur rekening</button>\n", CLASSBUTTON, $reknr, ICONVERSTUUR);
		}
		
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
		
		echo("</div> <!-- einde rekeningmuteren  -->\n");

		printf("<script>
			$( document ).ready(function() {
				rekeningprops();
			});

			\$(\"#rekeningkopmuteren > input, #rekeningkopmuteren > select, textarea\").on('blur', function(){
				savedata('rekeningedit', %1\$d, this);
				rekeningprops();
			});
			
			\$(\"#rekeningregelsmuteren tr td input\").on('blur', function(){
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

function rekeningenaanmaken() {
	
	$i_sz = new cls_Seizoen();
	$i_rk = new cls_Rekening();
	$i_lm = new cls_lidmaatschap();
	$i_lid = new cls_Lid();
	$i_rk = new cls_Rekening();
	$i_rr = new cls_Rekeningregel();
	
	$seizoen = $_POST['seizoen'] ?? -1;
	if ($seizoen <= 0) {
		$seizoen = $i_sz->zethuidige(date("Y-m-d"));
	}
	
	if (isset($_POST['rekeningdatum']) and strlen($_POST['rekeningdatum']) == 10) {
		$rekeningdatum = $_POST['rekeningdatum'];
	} else {
		$rekeningdatum = date("Y-m-d");
	}
	
	if (isset($_POST['eerstenummer']) and strlen($_POST['eerstenummer']) > 3) {
		$eerstenummer = intval($_POST['eerstenummer']);
	} else {
		$eerstenummer = $i_rk->nieuwrekeningnr($seizoen) + 1;
	}
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		$ontbrekende = $_POST['ontbrekende'] ?? 1;
	} else {
		$ontbrekende = 1;
	}

	$i_sz->vulvars($seizoen);
	if ($i_sz->gezinsrekening == 1) {
		$orderby = "L.Postcode, L.Adres, L.RekeningBetaaldDoor, L.GEBDATUM, L.Roepnaam";
	} else {
		$orderby = "L.Achternaam, L.Tussenv, L.RecordID";
	}
	if ($ontbrekende == 1) {
		$f = sprintf("(L.RecordID NOT IN (SELECT RR.Lid FROM %1\$sRekening AS RK INNER JOIN %1\$sRekreg AS RR ON RK.Nummer=RR.Rekening", TABLE_PREFIX);;
		$f .= sprintf(" WHERE RK.Seizoen=%d))", $seizoen);
	} else {
		$f = "";
	}
	$lidrows = $i_lid->ledenlijst(1, -1, $orderby, $f, 1);
	if ($_SERVER['REQUEST_METHOD'] == "POST" and isset($_POST['RekAanmaken'])) {
		if (isset($_POST['sure']) and $_POST['sure'] == "1") {
			set_time_limit(240);
			$vgezin = "0000 ZZ 999";
			$agl = 0;	// Aantal gezinsleden
			$reknr = $eerstenummer;
			$aantrek = 0;
			$aantregels = 0;
			foreach ($lidrows as $lidrow) {
				$i_lid->vulvars($lidrow->RecordID);
				$gezin = sprintf("%s %s %d", $i_lid->postcode, $i_lid->adres, $i_lid->rekeningbetaalddoor);
				if ($gezin != $vgezin and $agl > 1) {
					if ($i_sz->gezinskorting > 0) {
						$f = sprintf("RR.Rekening=%d AND RR.KSTNPLTS='%s' AND RR.Bedrag > 0", $reknr, $i_sz->verenigingscontributiekostenplaats);
						$aantal_gezinskorting = $i_rr->aantal($f, "RR.Lid");
						if ($aantal_gezinskorting > 1) {
							$rrid = $i_rr->add($reknr, 0, $i_sz->gezinskortingkostenplaats);
							$i_rr->update($rrid, "OMSCHRIJV", $i_sz->gezinskortingomschrijving);
							$i_rr->update($rrid, "Bedrag", $i_sz->gezinskorting * ($aantal_gezinskorting-1) * -1);
							$aantregels++;
						}
					}
					$i_rk->update($reknr, "DEBNAAM", $debnaam);
				}
				if ($gezin != $vgezin or $i_sz->gezinsrekening == 0) {
					$reknr = $i_rk->add($reknr, $seizoen, $lidrow->RecordID, $rekeningdatum);
					$agl = 1;
					$debnaam = $i_lid->naam;
					$vnm = $i_lid->achternaam . ", " . $i_lid->tussenvoegsels;
					$aantrek++;
				} else {
					if ($agl == 1) {
						$debnaam = " en " . $debnaam;
					} elseif ($agl > 1) {
						$debnaam = ", " . $debnaam;
					}
					if ($vnm == $i_lid->achternaam . ", " . $i_lid->tussenvoegsels) {
						$debnaam = $i_lid->roepnaam . $debnaam;
					} else {
						$debnaam = $i_lid->naam . $debnaam;
					}
					$agl++;
				}
				$aantregels += $i_rr->toevoegenstandaardregels($reknr, $lidrow->RecordID);
				$vgezin = $gezin;
			}
			$i_rk->controle(-1, $seizoen);
			printf("<p class='mededeling'>%d rekeningen aangemaakt met in totaal %d regels.<p>", $aantrek, $aantregels);
		} else {
			echo("<p class='mededeling'>Vink de checkbox 'Rekeningen aanmaken' aan, om de rekeningen aan te maken.</p>\n");
		}
	} elseif ($_SERVER['REQUEST_METHOD'] == "POST" and isset($_POST['Verder'])) {
		// Bevestigingsscherm
		if (count($lidrows) > 0) {
			printf("<form method='post' id='%s' class='form-check form-switch' action='%s?tp=%s'>\n", __FUNCTION__, $_SERVER['PHP_SELF'], $_GET['tp']);

			$toonleden = "";
			$aantleden = 0;
			foreach($lidrows as $lidrow) {
				$toonleden .= sprintf("<li>%s</li>\n", $lidrow->NaamLid);
				$aantleden++;
			}
			printf("Voor de volgende %d leden zal een rekening worden aangemaakt:\n<ul>\n%s</ul>\n", $aantleden, $toonleden);
			echo("<div class='clear'></div>\n");
			echo("<label class='form-check-label'><input type='checkbox' class='form-check-input' value='1' class='form-check-input' name='sure'>Rekeningen aanmaken?</label>\n");
			if ($ontbrekende == 1) {
				echo("<input type='hidden' name='ontbrekende' value=1>\n");
			}
			printf("<input type='hidden' name='rekeningdatum' value='%s'>\n", $rekeningdatum);
			printf("<input type='hidden' name='eerstenummer' value='%s'>\n", $eerstenummer);
			echo("<div id='opdrachtknoppen'>\n");
			printf("<button type='submit' class='%s' name='RekAanmaken'>%s Rekeningen aanmaken</button>\n", CLASSBUTTON, ICONVOLGENDE);
			printf("<button type='button' class='%s' onClick='history.go(-1);'>%s Terug</button>\n", CLASSBUTTON, ICONVORIGE);
			echo("</div>  <!-- Einde opdrachtknoppen -->\n");
			echo("</form>\n");
		} else {
			echo("<p class='mededeling'>Er zijn geen leden geselecteerd.</p>\n");
		}
	
	} else {
		// eerste selectiescherm
		printf("<form method='post' id='%s' class='form-check form-switch' action='%s?tp=%s'>\n", __FUNCTION__, $_SERVER['PHP_SELF'], $_GET['tp']);
		printf("<label class='form-label'>Seizoen</label><select name='seizoen' class='form-select form-select-sm' onChange='this.form.submit();'>%s</select>\n", $i_sz->htmloptions($seizoen));
		printf("<label class='form-label'>Omschrijving rekening</label><input type='text' id='Rekeningomschrijving' maxlength=35 class='w35' value='%s'>\n", $i_sz->rekeningomschrijving);
		printf("<label class='form-label'>Datum rekening</label><input type='date' name='rekeningdatum' value='%s'>\n", $rekeningdatum);
		printf("<label id='lblEersteRekeningNummer' class='form-label'>Eerste rekeningnummer</label><input type='number' class='d8' name='eerstenummer' value=%d>\n", $eerstenummer);
		printf("<label id='lblVerzamelenPerGezin' class='form-label'>Verzamelen per gezin?</label><input type='checkbox' class='form-check-input' id='RekeningenVerzamelen' %s>\n", checked($i_sz->gezinsrekening));
		printf("<label id='lblBetalingstermijn' class='form-label'>Betalingstermijn</label><input type='number' id='BetaaldagenTermijn' value=%d class='num2' min=0 max=999>\n", $i_sz->betaaldagentermijn);
		
		printf("<label class='form-label'>Alleen ontbrekende rekeningen</label><input type='checkbox' class='form-check-input' %s name='ontbrekende' value=1 onChange='this.form.submit();'>\n", checked($ontbrekende));
	
		printf("<label class='form-label'>Omschrijving verenigingscontributie</label><input type='text' id='Verenigingscontributie omschrijving' maxlength=50 class='w50' value='%s'>\n", $i_sz->verenigingscontributieomschrijving);
		printf("<label id='lblKostenplaats' class='form-label'>Kostenplaats</label><input type='text' id='Verenigingscontributie kostenplaats' maxlength=12 class='w8' value='%s'>\n", $i_sz->verenigingscontributiekostenplaats);
		
		$arrAO = [1 => "Alleen naam afdeling", 2 => "Alleen naam activiteit", 3 => "Combinatie namen afdeling en activiteit"];
		$options = "";
		foreach ($arrAO as $k => $o) {
			$options .= sprintf("<option value=%d%s>%s</option>\n", $k, checked($k, "option", $i_sz->afdelingscontributieomschrijving), $o);
		}
		printf("<label class='form-label'>Omschrijving afdelingscontributie</label><select id='Afdelingscontributie omschrijving' class='form-select form-select-sm'>%s</select>", $options);
		
		printf("<label class='form-label'>Omschrijving gezinskorting</label><input type='text' id='Gezinskorting omschrijving' maxlength=50 class='w50' value='%s'>\n", $i_sz->gezinskortingomschrijving);
		printf("<label id='lblKostenplaatsGezinskorting' class='form-label'>Kostenplaats gezinskorting</label><input type='text' id='Gezinskorting kostenplaats' maxlength=12 class='w8' value='%s'>\n", $i_sz->gezinskortingkostenplaats);
		printf("<label id='lblBedragGezinskorting' class='form-label'>Bedrag gezinskorting</label><input type='text' id='Gezinskorting bedrag' class='d8' value='%s'>\n", $i_sz->gezinskorting);
	
		echo("<div id='opdrachtknoppen'>\n");
		if (count($lidrows) > 0) {
			printf("<button type='submit' class='%s' name='Verder'><i class='bi bi-skip-forward-circle'></i> Verder (%d rekeningen)</button>\n", CLASSBUTTON, count($lidrows));
		} else {
			printf("<button type='button' class='%s'>Geen rekeningen beschikbaar</button>\n", CLASSBUTTON);
		}
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
	
		echo("</form>\n");
	
		printf("<script>
			$('input').on('blur', function() {
				if (this.id != null && this.id.length > 1) {
					savedata('seizoenedit', %1\$d, this);
				}
			});
			$('select').on('change', function() {
				savedata('seizoenedit', %1\$d, this);
			});
		</script>\n", $seizoen);
	}
	
}  # RekeningenAanmaken

function RekeningBetalingen() {
	$i_rb = new cls_RekeningBetaling();
	$i_rk = new cls_Rekening();
	
	echo("<div id='rekeningbetalingen'>\n");
	
	printf("<label class='form-label'>Rekeningnummer</label><input type='number' value=0 id='nieuw_rekening'>");
	echo("<p id='opmerking'></p>\n");
	printf("<label class='form-label'>Datum betaling</label><input type='date' id='nieuw_datum' value='%s'>\n", $i_rb->max("Datum"));
	echo("<label class='form-label'>Bedrag</label><input type='text' id='nieuw_bedrag' value='0,00' class='d8'>\n");
	printf("<button type='button' class='%s' id='btnBetalingToevoegen' onClick='fnBetalingToevoegen();' disabled>%s Betaling toevoegen</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
	printf("<button type='button' class='%s' onClick='location.reload();'>%s Ververs scherm</button>\n", CLASSBUTTON, ICONVERVERS);

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
	
	$('#nieuw_rekening').on('blur', function() {
		var rkid = $("#nieuw_rekening").val();
		if (rkid > 0) {
			$.ajax({
				url: 'ajax_update.php?entiteit=rekeningdetails',
				type: 'post',
				dataType: 'json',
				data: { id: rkid },
				success: function(response){
					if (response.rkid > 0) {
						$('#opmerking').html(response.debnaam);
						$('#nieuw_bedrag').val(response.open);
						
						$('#nieuw_datum').prop('disabled', false);
						$('#nieuw_bedrag').prop('disabled', false);
						$('#btnBetalingToevoegen').prop('disabled', false);
					} else {
						$('#opmerking').html('Rekening bestaat niet');
						$('#nieuw_datum').prop('disabled', true);
						$('#nieuw_bedrag').prop('disabled', true);
						$('#btnBetalingToevoegen').prop('disabled', true);
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
		
		$('#bedrag_' + rbid).addClass('deleted');
		$('#trash_' + rbid).hide();
		
		$.ajax({
			url: 'ajax_update.php?entiteit=del_betaling',
			type: 'post',
			dataType: 'json',
			data: { id: rbid }
		});
		
	}
	
	</script>
	<?php
}  # RekeningBetalingen

function RekeningInstellingen() {
	$i_ond = new cls_Onderdeel();
	$i_mv = new cls_Mailing_vanaf();
	
	echo("<div id='rekening_instellingen'>\n");
	
	echo("<h2>Algemene instellingen</h2>\n");
	printf("<label class='form-label'>Groep rekening betaald door</label><select id='rekening_groep_betaalddoor' class='form-select form-select-sm'>\n<option value=-1>Alleen webmasters</option>\n%s</select>\n", $i_ond->htmloptions($_SESSION['settings']['rekening_groep_betaalddoor'], 1));
	printf("<label class='form-label'>Bewaartermijn in maanden na rekeningdatum</label><input type='number' id='rekening_bewaartermijn' value=%d class='num2'>", $_SESSION['settings']['rekening_bewaartermijn']);
	
	echo("<h2>Instellingen voor mailen</h2>\n");
	echo("<label class='form-label'>Rekening versturen aan</label><select id='mailing_rekening_stuurnaar' class='form-select form-select-sm'>\n");
	$sn[1] = "Alleen betaald door";
	$sn[2] = "Betaald door en alle volwassenen op de rekening";
	$sn[3] = "Betaald door en alle leden op de rekening";
	$sn[4] = "Betaald door en het gekoppelde lid, indien ongelijk.";
	$sn[5] = "Alleen gekoppeld lid";
	$sn[6] = "Gekoppeld lid en alle volwassenen op de rekening";
	$sn[7] = "Alle leden op de rekening";
	foreach($sn as $key => $val) {
		printf("<option value=%d%s>%s</option>>\n", $key, checked($key, "option", $_SESSION['settings']['mailing_rekening_stuurnaar']), $val);
	}
	echo("</select>\n");
	printf("<label class='form-label'>Vanaf e-mailadres</label><select id='mailing_rekening_vanafid' class='form-select form-select-sm'>\n%s</select>\n", $i_mv->htmloptions($_SESSION['settings']['mailing_rekening_vanafid']));
	
	printf("<label class='form-label'>E-mailadres voor antwoorden</label><select id='mailing_rekening_replyid' class='form-select form-select-sm'>\n<option value=0>Gelijk aan vanaf</option>\n%s</select>\n", $i_mv->htmloptions($_SESSION['settings']['mailing_rekening_replyid']));
	
	printf("<label class='form-label'>Verstuurde e-mails alleen zichtbaar voor</label><select id='mailing_rekening_zichtbaarvoor' class='form-select form-select-sm'>\n<option value=-1>Alleen webmasters</option>\n%s</select>\n", $i_ond->htmloptions($_SESSION['settings']['mailing_rekening_zichtbaarvoor'], 1));
	echo("</div> <!-- Einde rekening_instellingen -->\n");
	
	echo("<script>
	$('select').on('change', function() {
		saveparam(this);
	});
	
	$('input').on('blur', function() {
		saveparam(this);
	});
	
</script>\n");
	
}  # RekeningInstellingen

function RekeningDetail($p_rkid, $p_metbriefpapier=0) {
	global $dtfmt;
	
	$i_rk = new cls_Rekening($p_rkid);
	$i_tp = new cls_Template();
	
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
	
	if ($i_rk->i_bd->machtigingafgegeven == 1) {
		$content = removetextblock($content, "<!-- Geen machtiging -->", "<!-- /Geen machtiging -->");
	} else {
		$content = removetextblock($content, "<!-- Wel machtiging -->", "<!-- /Wel machtiging -->");
	}
	
	if ($i_rk->bedrag == 0) {
		$content = removetextblock($content, "<!-- NietOpNulRekening -->", "<!-- /NietOpNulRekening -->");
	} elseif ($i_rk->bedrag < 0) {
		$content = removetextblock($content, "<!-- NietOpCredit -->", "<!-- /NietOpCredit -->");
	} else {
		$content = removetextblock($content, "<!-- NietOpDebet -->", "<!-- /NietOpDebet -->");
	}
	if ($i_rk->betaald == 0) {
		$content = removetextblock($content, "<!-- NietOpNulBetaald -->", "<!-- /NietOpNulBetaald -->");
	}
	
	if ($i_rk->lidid == $i_rk->betaalddoor) {
		$content = removetextblock($content, "<!-- AlleenOpBetaaldDoorAnder -->", "<!-- /AlleenOpBetaaldDoorAnder -->");
	}
	
	if (($i_rk->bedrag-$i_rk->betaald) < 0.1) {
		$content = removetextblock($content, "<!-- NietOpVolledigBetaald -->", "<!-- /NietOpVolledigBetaald -->");
	}
	
	if ($i_rk->aantalbetaaltermijnen > 1) {
		$content = removetextblock($content, "<!-- NietOpMeerdereTermijnen -->", "<!-- /NietOpMeerdereTermijnen -->");
	}
	
	if ($i_rk->aantalbetaaltermijnen < 2) {
		$content = removetextblock($content, "<!-- AlleenOpMeerdereTermijnen -->", "<!-- /AlleenOpMeerdereTermijnen -->");
	}
	
	$dtfmt->setPattern(DTTEXT);
	if ($i_rk->lidid == $i_rk->betaalddoor or $i_rk->betaalddoor == 0) {
		$content = str_ireplace("[%NAAMDEBITEUR%]", $i_rk->debnaam, $content);
	} else {
		$content = str_ireplace("[%NAAMDEBITEUR%]", $i_rk->i_bd->naam, $content);
	}
	$content = str_ireplace("[%NAAMLID%]", $i_rk->i_lid->naam, $content);
	$content = str_ireplace("[%ADRES%]", $i_rk->i_bd->adres, $content);
	$content = str_ireplace("[%POSTCODE%]", $i_rk->i_bd->postcode, $content);
	$content = str_ireplace("[%WOONPLAATS%]", $i_rk->i_bd->woonplaats, $content);
	$content = str_ireplace("[%LIDNR%]", $i_rk->i_lid->lidnr, $content);
	$content = str_ireplace("[%LIDID%]", $i_rk->lidid, $content);
	$content = str_ireplace("[%REKENINGDATUM%]", $dtfmt->format(strtotime($i_rk->datum)), $content);
	$content = str_ireplace("[%SEIZOEN%]", $i_rk->seizoen, $content);
	$content = str_ireplace("[%REKENINGOMSCHRIJVING%]", $i_rk->omschrijving, $content);
	$content = str_ireplace("[%REKENINGNUMMER%]", $i_rk->rkid, $content);
	$content = str_ireplace("[%REKENINGBEDRAG%]", fnBedrag($i_rk->bedrag), $content);
	$content = str_ireplace("[%BETAALD%]", fnBedrag($i_rk->betaald), $content);
	$content = str_ireplace("[%OPENSTAAND%]", fnBedrag($i_rk->bedrag-$i_rk->betaald), $content);
	$content = str_ireplace("[%BANKREKENING%]", $i_rk->i_bd->bankrekening, $content);
	$content = str_ireplace("[%UITERSTEBETAALDATUM%]", $i_rk->uiterstebetaaldatum, $content);
	$content = str_ireplace("[%EINDEEERSTETERMIJN%]", $i_rk->einde_eerstetermijn, $content);
	
	$content = str_ireplace("[%NAAMBETAALDDOOR%]", $i_rk->i_bd->naam, $content);
	$content = str_ireplace("[%EMAILBETAALDDOOR%]", $i_rk->i_bd->email, $content);
	
	$i_mv = new cls_Mailing_vanaf($_SESSION['settings']['mailing_rekening_vanafid']);
	$content = str_ireplace("[%VANAFADRES%]", $i_mv->vanaf_email, $content);
	$content = str_ireplace("[%VANAFNAAM%]", $i_mv->vanaf_naam, $content);
	$i_mv = null;
	$content = str_ireplace("[%NAAMVERENIGING%]", $_SESSION['settings']['naamvereniging'], $content);
	$content = str_ireplace("[%NAAMWEBSITE%]", $_SESSION['settings']['naamwebsite'], $content);
	$content = str_ireplace("[%URLWEBSITE%]", $_SERVER["HTTP_HOST"], $content);
	
	$rr = (new cls_Rekeningregel())->perrekening($i_rk->rkid);
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
	$bv = new DateTime($i_rk->datum);
	
	for ($bt=1;$bt<=$i_rk->aantalbetaaltermijnen;$bt++) {
		if ($bt < $i_rk->aantalbetaaltermijnen) {
			$bedr = round(($i_rk->bedrag / $i_rk->aantalbetaaltermijnen), 2);
			$tb += $bedr;
		} else {
			$bedr = round(($i_rk->bedrag - $tb), 2);
		}
		$bv->modify(sprintf("+%d day", $i_rk->dagenperbetaaltermijn));
		
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