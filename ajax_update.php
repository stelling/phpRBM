<?php

require_once('./includes/standaard.inc');
toegang("", 0, 0);

$ent = $_GET['entiteit'] ?? "";
$rid = $_POST['id'] ?? 0;
$kolom = $_POST['field'] ?? "";
$newvalue = $_POST['value'] ?? 0;
	
if ($_SESSION['lidid'] > 0) {

	if ($ent === "lid") {
		$i_lid = new cls_Lid();
		$i_lid->update($rid, $kolom, $_POST['value']);
		
	} elseif ($ent === "liddipl") {
		$i_ld = new cls_Liddipl();
		$i_ld->update($rid, $kolom, $_POST['value']);
		
	} elseif ($ent === "lidond" or $ent === "logroep" or $ent === "ledenperonderdeelmuteren") {
		if ($rid > 0) {
			$i_lo = new cls_Lidond();
			$i_lo->update($rid, $kolom, $newvalue);
		}
		
	} elseif ($ent == "onderdeeledit" and toegang("Ledenlijst/Basisgegevens/Onderdelen")) {
		$i_ond = new cls_Onderdeel();
		$i_ond->update($rid, $kolom, $_POST['value']);
			
	} elseif ($ent == "functieedit" and toegang("Ledenlijst/Basisgegevens/Functies")) {
		$i_fnk = new cls_Functie();
		$i_fnk->update($rid, $kolom, $_POST['value']);
	
	} elseif ($ent == "diplomaedit" and toegang("Ledenlijst/Basisgegevens/Diplomas")) {
		$i_dp = new cls_Diploma();
		$i_dp->update($rid, $kolom, $_POST['value']);
		
	} elseif ($ent == "organisatieedit" and toegang("Ledenlijst/Basisgegevens/Organisaties")) {
		$i_org = new cls_Organisatie();
		$i_org->update($rid, $kolom, $_POST['value']);
		
	} elseif ($ent == "mailing" and toegang("Mailing/Wijzigen")) {
		$i_m = new cls_Mailing();
		$i_m->update($rid, $kolom, $_POST['value']);
		$i_m = null;
		
	} elseif ($ent == "mailingprops") {
		$rid = $_POST['mailingid'] ?? 0;
		$groep = $_POST['selectie_groep'] ?? 0;
		$vangebdatum = $_POST['selectie_vangebdatum'] ?? "1900-01-01";
		$temgebdatum = $_POST['selectie_temgebdatum'] ?? date("Y-m-d");
		$i_m = new Mailing($rid);
		$rv = $i_m->add_del_selectie('aantal', $groep, $vangebdatum, $temgebdatum);
		$i_m = null;
		
		echo(json_encode($rv));
		
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
//		$rid = $_GET['mailingid'] ?? 0;
		$mid = $_POST['mailingid'] ?? 0;
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
		$mid = $_POST['mid'] ?? 4778;
		$selgroep = $_POST['selgroep'] ?? 0;
		$vangebdatum = $_POST['vangebdatum'] ?? "1900-01-01";
		$temgebdatum = $_POST['temgebdatum'] ?? "9999-12-31";
		
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
		$mid = $_POST['mailingid'] ?? 0;
		$a = $_POST['alle'] ?? 0;
		$i_m = new Mailing();
		$rv = $i_m->options_mogelijke_ontvangers($mid, $a);
		$i_m = null;
		
		echo(json_encode($rv));
		
	} else {
		$mess = "Je roept deze procedure op een niet correcte manier aan.";
		(new cls_Logboek())->add($mess, 15, $_SESSION['lidid'], 1, 0, 9);
	}
	
	return true;
}

?>
