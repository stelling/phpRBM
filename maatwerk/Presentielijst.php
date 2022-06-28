<?php

include "../includes/standaard.inc";

$maxlessen = 22;
$maxleerlingen = 25;

error_reporting(E_ALL);

$grid = $_GET['p_groep'] ?? 0;
$i_gr = new cls_Groep(0, $grid);

echo("<!DOCTYPE html>
		<html lang='nl'>
			<head>
			<title>Presentielijst</title>
			<link rel='stylesheet' href='../default.css'>
				<link rel='stylesheet' href='kleur.css'>
				<style>
				header {
					height: 90px;
				}
				
				img, h1 {
					float: left;
				}
				
				header h1 {
					clear: right;
					width: 80%;
					text-align: center;
					margin-top: 6px;
					margin-bottom: 2px;
					font-size: 24pt;
				}
				
				h2.groepsnaam {
					float: left;
					margin-left: 200px;
				}
				
				h2.tijden {
					float: right;
					margin-right: 8px;
				}
				
				#presentielijst {
					border-width: 1px;
					border-style: none;
					margin-top: 16px;
					clear: both;
					overflow: visible;
				}
				
				#presentielijst table {
					width: 100%;
				}
				
				th {
					background-color: white;
					color: black;
				}
				
				th.rotate {
					height: 225px;
					width: 40px;
					padding-top: 1px;
					padding-bottom: 1px;
					padding-left: 1px;
					padding-right: 1px;
					text-align: left;
					white-space: nowrap;
				}
				
				th.rotate > div {
					transform:
						translate(-10px, 72px)
						rotate(275deg);
					width: 45px;
					margin-left: 0;
					margin-right: 0;
					padding-left: 0;
					border-width: 1px;
					border-style: none;
					font-size: 12pt;
				}
				
				td {
					height: 45px;
				}
				
				th:first-child, td:first-child {
					text-align: right;
							width: 130px;
				}

				</style>
			</head>\n");

if ($grid > 0) {
	$grrow = $i_gr->record();
	$afdid = $grrow->OnderdeelID ?? 0;
	
	$i_lo = new cls_Lidond($afdid);
	
	$tp = $i_lo->ondnaam . "/Groepsindeling muteren";

	$i_ak = new cls_Afdelingskalender();

	if (toegang($tp, 0, 0)) {
		
		$einddatum = (new cls_Seizoen())->eindehuidige();
		
		$f = sprintf("GroepID=%d", $grid);
		$lorows = $i_lo->lijst(-1, $f, "", "", $maxleerlingen);
		if (count($lorows) > 0) {
				
			$htmlkop = "";
			$o = $grrow->Omschrijving;
			if (strlen($grrow->Instructeurs) > 0) {
				$o .= " - " . $grrow->Instructeurs; 
			}
			$t = $grrow->Starttijd;
			if (strlen($grrow->Eindtijd) > 0 and $grrow->Eindtijd > $grrow->Starttijd) {
				$t .= " - " . $grrow->Eindtijd;
			}

			$htmlkop = sprintf("<body>
				<div id='container'>
				<header>
				<h1>Presentielijst %s</h1>\n
				<h2 class='groepsnaam'>%s</h2>
				<h2 class='tijden'>%s</h2>
				</header>\n", $i_lo->ondnaam, $o, $t);
			$htmlll = "";
			foreach ($lorows as $lorow) {
				$htmlll .= sprintf("<th class='rotate'><div>%s</div></th>", $lorow->NaamLid);
			}

			echo($htmlkop);

			echo("<div id='presentielijst'>\n");
			echo("<table>");
			echo("<thead><tr><th></th>");
			echo($htmlll);
			echo("<th></th></tr>\n</thead>\n");

			$f = sprintf("AK.Datum >= CURDATE() AND AK.Datum <= '%s' AND AK.Activiteit=1", $einddatum);
			$f = sprintf("AK.Activiteit=1 AND AK.Datum <= '%s'", $einddatum);
			$akrows = $i_ak->lijst($afdid, "", $f, "AK.Datum", $maxlessen);
			foreach($akrows as $akrow) {
				printf("<tr><td>%s</td>", strftime("%e %B %Y", strtotime($akrow->Datum)));
				for ($i=1;$i<=count($lorows);$i++) {
					echo("<td>&nbsp;</td>");
				}
				printf("<td>%s</td></tr>\n", $akrow->Omschrijving);
			}

			echo("</table>\n");

			echo("<table>\n");
			echo("<caption>Wat is er in de les behandeld?</captio>\n");
			foreach($akrows as $akrow) {
				printf("<tr><td>%s</td><td></td></tr>\n", strftime("%e %B %Y", strtotime($akrow->Datum)));
			}
			echo("</table>\n");
		} else {
			$mess = "Deze groep is niet ing eebruik.";
			debug($mess, 1, 0);
		}

		echo("</div> <!-- Einde presentielijst -->\n");
	} else {
		$mess = "Je hebt geen rechten om deze lijst te bekijken.";
		debug($mess, 1, 1);
	}
} else {
	$mess = "Er is geen afdelingsgroep geselecteerd.";
	debug($mess, 1, 1);
}

echo("</div> <!--Einde container -->
			</body>
		</html>\n");
?>
