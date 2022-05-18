<?php

require_once('./includes/standaard.inc');
toegang("", 0, 0);

$ent = $_GET['entiteit'] ?? "";
$rid = $_POST['id'] ?? 0;
$kolom = $_POST['field'] ?? "";
	
if ($_SESSION['lidid'] > 0) {

	if ($ent === "lid") {
		$i_lid = new cls_Lid();
		$i_lid->update($rid, $kolom, $_POST['value']);
		
	} elseif ($ent === "liddipl") {
		$i_ld = new cls_Liddipl();
		$i_ld->update($rid, $kolom, $_POST['value']);
		
	} elseif ($ent === "lidond") {
		$i_lo = new cls_Lidond();
		$i_lo->update($rid, $kolom, $_POST['value']);
	
	} elseif ($ent == "diplomaedit") {
		$i_dp = new cls_Diploma();
		$i_dp->update($rid, $kolom, $_POST['value']);
		
	} elseif ($ent == "mailing") {
		$i_m = new cls_Mailing();
		$i_m->update($rid, $kolom, $_POST['value']);
		
	}
	return true;
}

?>
