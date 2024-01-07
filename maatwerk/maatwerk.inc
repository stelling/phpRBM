<?php

function fnMaatwerkNaUitloggen() {

}

function fnMaatwerkNaInloggen() {

}

function fnMaatwerkNaWijzigenWachtwoord($lidid=0, $ww="") {

}

function fnMaatwerkNaUpload() {

}
function fnMaatwerkNaToevoegenStandaardRegelOpRekening($p_rrid=0) {
	$i_rr = new cls_RekeningRegel(-1, $p_rrid);
	$i_lo = new cls_Lidond(-1, $i_rr->lidid, $i_rr->lidondid);
	$i_act = new cls_Activiteit($i_rr->activiteitid);
	$i_lm = new cls_Lidmaatschap(-1, $i_rr->lidid);
	
}  # fnMaatwerkNaToevoegenStandaardRegelOpRekening
?>