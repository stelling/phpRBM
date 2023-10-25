<?php
require("./includes/standaard.inc");

if (isset($_GET['p_stukid']) and $_GET['p_stukid'] > 0) {
  
	$i_st = new cls_Stukken($_GET['p_stukid']);
	$i_lb = new cls_logboek();

	if ($i_st->stid == 0) {
		$mess = sprintf("Stuk %d bestaat niet.", $_GET['p_stukid']);
		$i_lb->add($mess, 22, 0, 1, $i_st->stid, 8, "Stukken");
	} elseif (substr($i_st->link, 0, 4) == "http") {
		$mess = sprintf("Stuk %d is een externe download.", $i_st->stid);
		$i_lb->add($mess, 22, 0, 1, $i_st->stid, 8, "Stukken");
	} elseif ($i_st->magdownload) {
		$filename = BASEDIR . "/stukken/" . $i_st->link;
		if (file_exists($filename)) {
			if (substr($i_st->link, -4) == ".pdf") {
				header('Content-type: application/pdf');
				header("Content-Length: " . filesize($filename));
			} else {
				header('Content-Type: application/zip');
				header("Content-Disposition: attachment; filename={$i_st->link}" );
			}
			
			header('Content-Transfer-Encoding: binary');
			readfile($filename);
			exit;
		} else {
			$mess = sprintf("Bestand '%s' bestaat niet.", $filename);
			$i_lb->add($mess, 22, 0, 1, $i_st->stid, 8, "Stukken");
		}
	} else {
		$mess = sprintf("Je bent niet gerechtigd om stuk %d te downloaden.", $i_st->stid);
		$i_lb->add($mess, 22, 0, 1, $i_st->stid, 8, "Stukken");
	}
	
} else {
	$mess = "Er is geen stuk gespecificeerd.";
	$i_lb->add($mess, 22, 0, 1, $i_st->stid, 8, "Stukken");
}

?>
