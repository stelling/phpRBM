<?php
header('Content-Type: application/json');

require_once('./includes/standaard.inc');
toegang("", 0, 0);

$ent = $_GET['entiteit'] ?? "";
$rid = $_POST['id'] ?? 0;
$kolom = $_POST['field'] ?? "";
$newvalue = $_POST['value'] ?? "";
$lidid = $_POST['lidid'] ?? 0;
$ondtype = $_POST['ondtype'] ?? "C";
$ondid = $_POST['ondid'] ?? 0;
	
if ($_SESSION['lidid'] > 0) {

	if ($ent === "lid") {
		$i_lid = new cls_Lid();
		$rv = $i_lid->update($rid, $kolom, $newvalue);
		$i_lid = null;
		echo(json_encode($rv));
		
	} elseif ($ent == "naamlid") {
		$i_lid = new cls_Lid($rid);
		echo(json_encode($i_lid->Naam()));
		
	} elseif ($ent == "telefoonlid") {
		$i_lid = new cls_Lid($rid);
		echo(json_encode($i_lid->telefoon()));
		
	} elseif ($ent == "emaillid") {
		$i_lid = new cls_Lid($rid);
		echo(json_encode($i_lid->email()));
		
	} elseif ($ent == "woonplaats") {
		$postcode = $_POST['postcode'] ?? "";
		$i_lid = new cls_Lid();
		echo(json_encode($i_lid->woonplaats($postcode)));
		
	} elseif ($ent === "lidond" or $ent === "logroep" or $ent === "ledenperonderdeelmuteren") {
		if ($rid > 0) {
			$i_lo = new cls_Lidond();
			$i_lo->update($rid, $kolom, $newvalue);
		}
				
	} elseif ($ent === "addlidond") {
//		$mess = sprintf("%d / %d", $ondid, $lidid);
//		debug($mess, 0, 1);
		$i_lo = new cls_Lidond();
		$i_lo->add($ondid, $lidid);
		
	} elseif ($ent === "lo_presentie") {
		$loid = $_POST['loid'] ?? 0;
		$akid = $_POST['akid'] ?? 0;
		
		if ($loid > 0 and $akid > 0) {
			$i_aanw = new cls_Aanwezigheid();
			$i_aanw->update($loid, $akid, "Status", $newvalue);
		}
		
	} elseif ($ent == "htmlloperlid") {
		echo(json_encode(htmlloperlid($lidid, $ondtype)));
		
	} elseif ($ent === "zeteigenschap") {
		$i_lo = new cls_Lidond();
		$value = $_POST['value'] ?? 1;
		$i_lo->zeteigenschap($lidid, $ondid, $value);
				
	} elseif ($ent === "liddipl") {
//		debug(sprintf("%s: %d / %s / %s", $ent, $rid, $kolom, $newvalue), 0, 1);
		if ($rid > 0) {
			$i_ld = new cls_Liddipl();
			$i_ld->update($rid, $kolom, $newvalue);
		}
		
	} elseif ($ent === "verw_liddipl") {
//		debug(sprintf("%s: %d / %s / %s", $ent, $rid, $kolom, $newvalue), 0, 1);
		if ($rid > 0) {
			$i_ld = new cls_Liddipl();
			$i_ld->delete($rid, $kolom, $newvalue);
		}
		
	} elseif ($ent == "onderdeeledit" and toegang("Ledenlijst/Basisgegevens/Onderdelen")) {
		
		$i_ond = new cls_Onderdeel();
		$i_ond->update($rid, $kolom, $newvalue);
		
	} elseif ($ent == "groepedit") {
		$i_gr = new cls_Groep();
		$i_gr->update($rid, $kolom, $newvalue);
			
	} elseif ($ent == "functieedit" and toegang("Ledenlijst/Basisgegevens/Functies")) {
		$i_fnk = new cls_Functie();
		$i_fnk->update($rid, $kolom, $newvalue);
		
	} elseif ($ent == "activiteitedit" and toegang("Ledenlijst/Basisgegevens/Activiteiten")) {
		$i_act = new cls_Activiteit();
		$i_act->update($rid, $kolom, $newvalue);
	
	} elseif ($ent == "diplomaedit" and toegang("Ledenlijst/Basisgegevens/Diplomas")) {
		$i_dp = new cls_Diploma();
		$i_dp->update($rid, $kolom, $newvalue);
		
	} elseif ($ent == "afdelingskalenderedit") {
		$i_ak = new cls_Afdelingskalender();
		$rv = $i_ak->update($rid, $kolom, $newvalue);
		$i_ak = null;
		echo(json_encode($rv));
		
	} elseif ($ent == "organisatieedit" and toegang("Ledenlijst/Basisgegevens/Organisaties")) {
		$i_org = new cls_Organisatie();
		$i_org->update($rid, $kolom, $newvalue);	
		
	} elseif ($ent == "seizoenedit" and (toegang("Ledenlijst/Basisgegevens/Seizoenen") or toegang("Rekeningen/Aanmaken rekeningen"))) {
		$i_sz = new cls_Seizoen();
		$i_sz->update($rid, $kolom, $newvalue);
		$i_sz = null;
		
	} elseif ($ent == "mailing" and toegang("Mailing/Muteren")) {
//		$mess = sprintf("%d / %s / %s", $rid, $kolom, $newvalue);
//		debug($mess, 0, 1);

		if ($rid > 0 and strlen($kolom) > 0) {
			$i_m = new cls_Mailing();
			$i_m->update($rid, $kolom, $newvalue);
			$i_m = null;
		}
		
	} elseif ($ent == "mailingprops") {
		$rid = $_POST['mailingid'] ?? 4;
		$groep = $_POST['selectie_groep'] ?? 350;
		$vangebdatum = $_POST['selectie_vangebdatum'] ?? "1920-01-01";
		$temgebdatum = $_POST['selectie_temgebdatum'] ?? date("Y-m-d");
		$i_m = new Mailing($rid);
		$rv = $i_m->add_del_selectie('aantal', $groep, $vangebdatum, $temgebdatum);
		$i_m = null;
		
		$rv = json_encode($rv);
		echo($rv);
		
	} elseif ($ent == "mailingcontrole") {
		$mid = $_POST['mailingid'] ?? 0;
		$i_m = new Mailing($mid);
		$rv = $i_m->controle();
		$i_m = null;
		
		echo(json_encode($rv));
		
	} elseif ($ent == "mailingaantontvangers") {
		$rid = $_POST['mailingid'] ?? 0;
		$i_mr = new cls_mailing_rcpt();
		$rv = $i_mr->aantalontvangers($rid);
		$i_mr = null;
		
		echo(json_encode($rv));
		
	} elseif ($ent == "mailing_html_ontvangers") {
		$mid = $_POST['mailingid'] ?? 4;
		$i_m = new Mailing($rid);
		$rv = $i_m->html_ontvangers($mid, false);
		$i_m = null;
		
		echo(json_encode($rv));
				
	} elseif ($ent == "mailing_add_ontvanger" and toegang("Mailing/Muteren")) {
		$mid = $_POST['mid'] ?? 0;
		$lidid = $_POST['lidid'] ?? 0;
		$email = $_POST['email'] ?? "";
		$i_mr = new cls_Mailing_rcpt();
		$rv = $i_mr->add($mid, $lidid, $email);
		$i_mr = null;
		
		echo(true);
		
	} elseif ($ent == "mailing_add_selectie_ontvangers" and toegang("Mailing/Muteren")) {
		$mid = $_POST['mid'] ?? 4;
		$selgroep = $_POST['selgroep'] ?? 0;
		$vangebdatum = $_POST['vangebdatum'] ?? "1900-01-01";
		$temgebdatum = $_POST['temgebdatum'] ?? "9999-12-31";
		
		$i_lo = new cls_lidond();
		$i_lo->auto_einde(0, 1, $selgroep);
		$i_lo->autogroepenbijwerken($selgroep);
		$i_lo = null;
		
		$i_m = new Mailing($mid);
		$rv = $i_m->add_del_selectie("add", $selgroep, $vangebdatum, $temgebdatum);
		$i_m = null;
		
		echo($rv);
		
	} elseif ($ent == "mailing_verw_ontvanger" and toegang("Mailing/Muteren")) {
		$mid = $_POST['mid'] ?? 0;
		$lidid = $_POST['lidid'] ?? 0;
		$email = $_POST['email'] ?? 0;
		$i_mr = new cls_Mailing_rcpt();
		$i_mr->delete($mid, $lidid, $email);
		$i_mr = null;
		
		echo(true);
		
	} elseif ($ent == "mailing_verw_selectie_ontvangers" and toegang("Mailing/Muteren")) {
		$mid = $_POST['mid'] ?? 0;
		$selgroep = $_POST['selgroep'] ?? 0;
		$vangebdatum = $_POST['vangebdatum'] ?? "1900-01-01";
		$temgebdatum = $_POST['temgebdatum'] ?? "9999-12-31";
		$i_m = new Mailing($mid);
		$rv = $i_m->add_del_selectie("delete", $selgroep, $vangebdatum, $temgebdatum);
		$i_m = null;
		
		echo(json_encode($rv));
				
	} elseif ($ent == "mailing_verw_alle_ontvangers" and toegang("Mailing/Muteren")) {
		$mid = $_POST['mid'] ?? 4778;
		$i_mr = new cls_Mailing_rcpt();
		$rv = $i_mr->delete_all($mid);
		$i_mr = null;
		
		echo(json_encode($rv));
		
	} elseif ($ent == "options_mogelijke_ontvangers") {
		$mid = $_POST['mailingid'] ?? 4;
		$a = $_POST['alle'] ?? 0;
		$i_m = new Mailing();
		$rv = $i_m->options_mogelijke_ontvangers($mid, $a);
		$i_m = null;
		
		echo(json_encode($rv));
		
	} elseif ($ent == "editmailingvanaf") {
		$i_mv = new cls_Mailing_vanaf();
		$i_mv->update($rid, $kolom, $newvalue);
		
	} elseif ($ent == "rekeningedit" and toegang("Rekeningen/Muteren")) {
//		$mess = sprintf("%d / %s /%s", $rid, $kolom, $newvalue);
//		debug($mess, 0, 1);
		$i_rk = new cls_Rekening();
		$i_rk->update($rid, $kolom, $newvalue);
		
	} elseif ($ent == "rekeningmail") {	
		$i_mh = new cls_Mailing_hist();
		$f = sprintf("Xtra_Char='REK' AND Xtra_Num=%d", $rid);
		$rv = $i_mh->laatste($f);
		$i_mh = null;
		
		echo(json_encode($rv));
		
	} elseif ($ent == "rekeningdetails") {
		
		$i_rk = new cls_Rekening($rid);
		
		$rv['rkid'] = $i_rk->rkid;
		$rv['debnaam'] = $i_rk->debnaam;
		$rv['bedrag'] = number_format(round($i_rk->bedrag, 2), 2, ",", "");
		$rv['open'] = number_format(round($i_rk->bedrag-$i_rk->betaald, 2), 2, ",", "");
		
		$i_rk = null;
		
		echo(json_encode($rv));
		
	} elseif ($ent == "rekregedit" and toegang("Rekeningen/Muteren")) {
//		$mess = sprintf("%d / %s /%s", $rid, $kolom, $newvalue);
//		debug($mess, 0, 1);
		$i_rr = new cls_Rekeningregel();
		$i_rr->update($rid, $kolom, $newvalue);
		
	} elseif ($ent == "verw_rekregel" and toegang("Rekeningen/Muteren")) {
		$i_rr = new cls_Rekeningregel();
		$i_rr->delete($rid);
		$i_rr = null;

	} elseif ($ent == "add_betaling") {
		$datum = $_POST['datum'] ?? "";
		$bedr = $_POST['bedr'] ?? 0;
		
		if ($rid > 0 and strlen($datum) == 10 and $bedr != 0) {
			$i_rb = new cls_RekeningBetaling();
			$i_rb->add($rid, $datum, $bedr);
			$i_rb = null;
		}
		
	} elseif ($ent == "del_betaling") {
		if ($rid > 0) {
			$i_rb = new cls_RekeningBetaling();
			$i_rb->delete($rid);
			$i_rb = null;
		}
		
	} elseif ($ent == "evenement" and toegang("Evenementen/Beheer")) {
		$i_ev = new cls_Evenement();
		$i_ev->update($rid, $kolom, $newvalue);
		$i_ev = null;

	} elseif ($ent == "evenementdln" and toegang("Evenementen/Beheer")) {
		$i_ed = new cls_Evenement_Deelnemer();
		$i_ed->update($rid, $kolom, $newvalue);
		$i_ed = null;
		
	} elseif ($ent == "verw_dln" and toegang("Evenementen/Beheer")) {
		$i_ed = new cls_Evenement_Deelnemer();
		$i_ed->delete($rid);
		$i_ed = null;
		
	} elseif ($ent == "add_autorisatie" and $_SESSION['webmaster'] == 1) {
		$i_aa = new cls_Authorisation();
		$i_aa->add($_POST['tabpage']);
		$i_aa = null;
		
	} elseif ($ent == "update_autorisatie" and $_SESSION['webmaster'] == 1) {
		$i_aa = new cls_Authorisation();
		$i_aa->update($rid, $kolom, $newvalue);
		
	} elseif ($ent === "delete_autorisatie" and $_SESSION['webmaster'] == 1) {
		$i_aa = new cls_Authorisation();
		$i_aa->delete($rid);
		
	} elseif ($ent == "updateparam") {
		$name = $_POST['name'] ?? "";
		$val = $_POST['value'] ?? "";
		if (strlen($name) > 1) {
			$i_param = new cls_Parameter();
			$i_param->update($name, $val);
			$i_param = null;
		}
		
	} elseif ($ent == "checkdnsrr") {
		$email = $_POST['email'] ?? "";
		$rv = fnControleEmail($email);
		echo(json_encode($rv));
		
	} else {
		$mess = sprintf("Entiteit '%s' bestaat niet in ajax_update.php of je hebt geen toegang.", $ent);
		(new cls_Logboek())->add($mess, 15, $_SESSION['lidid'], 1, 0, 9);
	}
	
	return true;
}

?>
