<?php

require_once('./includes/standaard.inc');
toegang("", 0, 0);

if ($_SESSION['lidid'] > 0) {

	$ent = $_GET['entiteit'] ?? "";
	$id = $_POST['id'] ?? 0;
	$kolom = $_POST['field'] ?? "";

	if ($ent === "lid") {
		$i_lid = new cls_Lid();
		$i_lid->update($id, $kolom, $_POST['value']);
		
	} elseif ($ent === "liddipl") {
		$i_ld = new cls_Liddipl();
		$i_ld->update($id, $kolom, $_POST['value']);
		
	} elseif ($ent === "lidond") {
		$i_lo = new cls_Lidond();
		$i_lo->update($id, $kolom, $_POST['value']);
	
	} elseif ($ent == "diplomaedit") {
		$i_dp = new cls_Diploma();
		$i_dp->update($id, $kolom, $_POST['value']);
	}
	return true;
}

?>
