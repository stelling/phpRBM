<?php

require('./includes/standaard.inc');

$insid = $_GET['insid'] ?? 18;

if ($_SESSION['lidid'] > 0) {
	$i_ins = new cls_Inschrijving($insid);
	
	if ($i_ins->insid == 0) {
		HTMLheader();
		printf("<p class='mededeling'>Inschrijving %d bestaat niet.</p>\n", $insid);
		
	} elseif (strlen($i_ins->pdf()) == 0) {
		HTMLheader();
		printf("<p class='mededeling'>Aan inschrijving %d is geen PDF gekoppeld.</p>\n", $i_ins->insid);
		
	} else {

		header("Content-type: application/pdf");
		header("Content-Disposition: inline; filename=Inschrijfformulier $i_ins->naam $insid.pdf");
		header('Pragma: public');
	
		echo($i_ins->pdf());
	}
	
} else {
	HTMLheader();
	echo("<p class='mededeling'>Je moet zijn ingelogd om dit te kunnen bekijken.</p>\n");
}

?>