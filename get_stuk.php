<?php
require("./includes/standaard.inc");

$i_lb = new cls_logboek();

$p_stukid = $_GET['p_stukid'] ?? 0;
$mess = "";
$ta = 22;
$tas = 8;

if ($p_stukid > 0) {
  
	$i_st = new cls_Stukken($p_stukid);

	if ($i_st->stid == 0) {
		$mess = sprintf("Stuk %d bestaat niet.", $p_stukid);
	} elseif (substr($i_st->link, 0, 4) == "http" or substr($i_st->link, 0, 5) == "https") {
		$mess = sprintf("Stuk '%1\$s' is een externe download of webpagina, ga naar <a href='%2\$s'>%1\$s</a>.", $i_st->titel, $i_st->link);
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
		}
	} else {
		$mess = sprintf("Je bent niet gerechtigd om stuk %d te downloaden.", $i_st->stid);
		$ta = 15;
		$tas = 9;
	}
	
} else {
	$mess = "Er is geen stuk gespecificeerd.";
}

if (strlen($mess) > 0 and ($_SESSION['lidid'] > 0 or $ta == 15)) {
	$i_lb->add($mess, $ta, 0, 1, $p_stukid, $tas, "Stukken");
} elseif (strlen($mess) > 0) {
	printf("<p class='mededeling'>%s</p>\n", $mess);
}

?>
