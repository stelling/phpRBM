<?php


require_once('./includes/standaard.inc');

$ent = $_GET['entiteit'] ?? "";
$id = $_POST['id'] ?? 0;
$kolom = $_POST['field'] ?? "";

if ($ent == "lidedit") {
	$i_dp = new cls_Diploma();
	$i_dp->update($id, $kolom, $_POST['value']);
	return true;
	
} elseif ($ent == "diplomaedit") {
	$i_dp = new cls_Diploma();
	$i_dp->update($id, $kolom, $_POST['value']);
	return true;
}

?>
