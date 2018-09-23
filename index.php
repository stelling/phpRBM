<?php

if (!isset($_GET['tp'])) {
	$_GET['tp'] = "Vereniging/Introductie";
}

require('./includes/standaard.inc');

if ($_SESSION['aantallid'] == 0) {
	$query = sprintf("SELECT COUNT(*) FROM %sLid;", $table_prefix);
	$_SESSION['aantallid'] = db_scalar($query);
	if ($_SESSION['aantallid'] == 0) {
		echo("<script>alert('Voordat deze website gebruikt kan worden moeten er eerst gegevens uit de Access-database ge-upload worden.');
			location.href='./admin.php?tp=Uploaden+data';</script>\n");
	}
}

if (isset($_GET['op']) and $_GET['op'] == "exportins") {
	header("Content-type: text/plain");
	header("Content-Disposition: attachment; filename=inschrijvingen.sql");
	foreach(db_insbew("export") as $row) {
		echo(SQLexport($row) . "\n");
	}
	exit();
}

if ($currenttab !== "Mailing") {
	if ($currenttab == "Vereniging" or $currenttab == "Eigen gegevens"  or $currenttab == "Ledenlijst"  or $currenttab2 == "Logboek") {
		HTMLheader(1);
	} else {
		HTMLheader(0);
	}
}

if (toegang("", 0) == false) {
	$mess = sprintf("Je hebt tot '%s' geen toegang.", $_GET['tp']);
	db_logboek("add", $mess, 5, 0, 1);
	if ($_SESSION['lidid'] == 0) {
		fnLoginAanvragen();
	}
} elseif ($currenttab == "Herstellen wachtwoord") {
	fnHerstellenWachtwoord();
} elseif ($currenttab == "Eigen gegevens") {
	if ($_SESSION['lidid'] > 0) {
		fnOverviewLid($_SESSION['lidid'], $currenttab2);
	} else {
		echo("<p class='mededeling'>Er is geen lid ingelogd.</p>\n");
	}
} elseif ($currenttab == "Zelfservice") {
	if ($currenttab2 == "Inschrijving bewaking") {
		inschrijvenbewaking($_SESSION['lidid']);
	} elseif ($currenttab2 == "Inschrijving evenementen") {
		inschrijvenevenementen($_SESSION['lidid']);
	} elseif ($currenttab2 == "Bestellingen") {
		onlinebestellingen($_SESSION['lidid']);
	} else {
		fnWijzigen($_SESSION['lidid'], $currenttab2);
	}
} elseif ($currenttab == "Overzicht lid" and toegang("Overzicht lid", 0)) {
	if (isset($_GET['lidid']) and is_numeric($_GET['lidid']) and $_GET['lidid'] > 0) {
		fnOverviewLid($_GET['lidid'], $currenttab2);
	} else {
		fnOverviewLid(0, $currenttab2);
	}
} elseif ($currenttab == "Wijzigen lid") {
	if (isset($_GET['lidid']) and is_numeric($_GET['lidid']) and $_GET['lidid'] > 0) {
		fnWijzigen($_GET['lidid'], $currenttab2);
	}
} elseif ($currenttab == "Vereniging") {
	fnDispMenu(2);
	if ($currenttab2 == "Introductie") { 
		if (file_exists($fileverinfo)) {
			fnVoorblad();
			if (!isset($_SESSION['lidid']) or $_SESSION['lidid'] == 0) {
				echo("<div id='kolomrechts'>\n");
				fnLoginAanvragen();
				echo("</div>  <!-- Einde kolomrechts -->\n");
			}
		} else {
			fnLoginAanvragen();
		}
	} else {
		fnWieiswie($currenttab2, db_param("kaderoverzichtmetfoto"));
	}
} elseif ($currenttab == "Ledenlijst") {
	fnLedenlijst();
} elseif ($currenttab == "Bewaking") {
	fnBewaking();
} elseif ($currenttab == "Kostenoverzicht") {
	fnKostenoverzicht();
} elseif ($currenttab == "Mailing") {
	fnMailing();
} elseif ($currenttab == "Evenementen") {
	fnEvenementen();
} elseif ($currenttab == "Bestellingen") {
	fnWebshop();
} elseif (!isset($_SESSION['lidid']) or $_SESSION['lidid'] == 0) {
	fnLoginAanvragen();
} else {
	debug("Geen voorblad");
}

if ($currenttab != "Mailing") {	
	HTMLfooter();
}

function fnVoorblad($metlogin=0) {
	global $fileverinfo;

	if (file_exists($fileverinfo)) {
		$content = file_get_contents($fileverinfo);
	} else {
		$content = "";
	}
	if ($content !== false and strlen($content) > 0) {
		if ($_SESSION['lidid'] == 0) {
			$content = removetextblock($content, "<!-- Ingelogd -->", "<!-- /Ingelogd -->");
		}
	
		// Algemene statistieken
		$stats = db_stats();
		foreach (array('aantalleden', 'aantalvrouwen', 'aantalmannen', 'gemiddeldeleeftijd', 'aantalkaderleden', 'nieuwstelogin', 'aantallogins', 'nuingelogd') as $v) {
			$content = str_replace("[%" . strtoupper($v) . "%]", htmlentities($stats[$v]), $content);
		}
		$content = str_replace("[%LAATSTGEWIJZIGD%]", strftime("%e %B %Y (%H:%M)", strtotime($stats['laatstgewijzigd'])), $content);
		$content = str_replace("[%LAATSTEUPLOAD%]", strftime("%e %B %Y (%H:%M)", strtotime($stats['laatsteupload'])), $content);
		
		// Gebruiker-specifieke statistieken
		if ($_SESSION['lidid'] > 0) {
			$stats = db_stats($_SESSION['lidid']);
			$content = str_replace("[%NAAMLID%]", $_SESSION['naamingelogde'], $content);
			$content = str_replace("[%LIDNR%]", $_SESSION['lidnr'], $content);
			$content = str_replace("[%INGELOGDEGEWIJZIGD%]", strftime("%e %B %Y (%H:%m)", strtotime($stats['laatstgewijzigd'])), $content);
		} else {
			$content = str_replace("[%INGELOGDEGEWIJZIGD%]", "", $content);
		}
		if (strpos($content, "[%KOMENDEEVENEMENTEN%]") !== false) {
			$content = str_replace("[%KOMENDEEVENEMENTEN%]", ToekomstigeEvenementen(), $content);
		}
		$content = str_replace("[%ROEPNAAM%]", $_SESSION['roepnaamingelogde'], $content);
		if (strpos($content, "[%VERVALLENDIPLOMAS%]") !== false and $_SESSION['lidid'] > 0) {
			$strHV = "";
			foreach (db_liddipl("vervallenbinnenkort", $_SESSION['lidid']) as $row) {
				if ($row->VervaltPer <= date("Y-m-d")) {
					$strHV .= sprintf("<li>%s, gehaald op %s, is per %s vervallen.</li>\n", $row->Diploma, strftime("%e %h %Y", strtotime($row->DatumBehaald)), strftime("%e %h %Y", strtotime($row->VervaltPer)));
				} else {
					$strHV .= sprintf("<li>%s, gehaald op %s, vervalt op %s.</li>\n", $row->Diploma, strftime("%e %h %Y", strtotime($row->DatumBehaald)), strftime("%e %h %Y", strtotime($row->VervaltPer)));
				}
			}
			if (strlen($strHV) == 0) {
				$strHV = "<li>Geen</li>\n";
			}
			$content = str_replace("[%VERVALLENDIPLOMAS%]", $strHV, $content);
		}
	
		
		if (strpos($content, "[%VERJAARFOTO%]") !== false) {
			$content = str_replace("[%VERJAARFOTO%]", overzichtverjaardagen(1), $content);
		}
		if (strpos($content, "[%VERJAARDAGEN%]") !== false) {
			$content = str_replace("[%VERJAARDAGEN%]", overzichtverjaardagen(0), $content);
		}

		printf("<div id='welkomstekst'>\n%s</div>  <!-- Einde welkomstekst -->\n", $content);
		
	}
	?>
	<?php
}

function fnKostenoverzicht() {
	global $table_prefix;

	$val_jaarfilter = "";
	$val_gbrfilter = "";
	$val_kplfilter = "";
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['lbJaarFilter'])) {
			$val_jaarfilter = $_POST['lbJaarFilter'];
		}
		if (isset($_POST['lbGBRFilter'])) {
			$val_gbrfilter = $_POST['lbGBRFilter'];
		}
		if (isset($_POST['lbKPLFilter'])) {
			$val_kplfilter = $_POST['lbKPLFilter'];
		}
	}
	
	echo("<div id='filter'>\n");
	printf("<form name='Filter' action='%s?%s' method='post'>", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
	echo("<table>\n");
	echo("<tr>\n");
	$ret = "";
	foreach (db_boekjaar() as $row) {
		if ($val_jaarfilter == $row->RecordID or strlen($val_jaarfilter) == 0) {
			$s = " selected";
			$val_jaarfilter = $row->RecordID;
		} else {
			$s = "";
		}
		$ret .= sprintf('<option%s value="%2$s">%3$s</option>\n', $s, $row->RecordID, $row->Jaar);
	}
	printf("<td class='label'>Boekjaar</td><td><select name='lbJaarFilter' onchange='form.submit();'>%s</select></td>\n", $ret);
	
	$ret = "<option value='*'>Alle</option>\n";
	$query = sprintf("SELECT DISTINCT GBR.Kode, CONCAT(GBR.Kode, ' - ', GBR.OMSCHRIJV) AS Oms
				 FROM %1\$sMutatie AS M INNER JOIN %1\$sGBR AS GBR ON M.GBR = GBR.Kode
				 ORDER BY GBR.Kode;", $table_prefix);
	$result = fnQuery($query);
	foreach ($result->fetchAll() as $row) {
		if ($val_gbrfilter == $row->Kode) {
			$s = " selected";
		} else {
			$s = "";
		}
		$ret .= sprintf('<option%s value="%s">%s</option>\n', $s, $row->Kode, $row->Oms);
	}
	printf("<td class='label'>Grootboekrekening</td><td><select name='lbGBRFilter' onchange='form.submit();'>%s</select></td>\n", $ret);
	
	$ret = "<option value='*'>Alle</option>\n";
	foreach (db_kostenplaats("distinct") as $row) {
		if ($val_kplfilter == $row->Kode) {
			$s = " selected";
		} else {
			$s = "";
		}
		$ret .= sprintf('<option%s value="%s">%s</option>\n', $s, $row->Kode, $row->Kode);
	}
	printf("<td class='label'>Kostenplaats</td><td><select name='lbKPLFilter' onchange='form.submit();'>%s</select></td>\n", $ret);
	
	echo("</tr>\n");
	echo("</table>\n");
	echo("</form>\n");
	echo("</div>  <!-- Einde filter -->\n");

	echo(fnDisplayTable(db_mutatie($val_jaarfilter, $val_gbrfilter, $val_kplfilter), "", "", 1));
}

?>
