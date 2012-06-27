<?php
include('./includes/standaard.inc');

if ($_SESSION['aantallid'] == 0) {
	echo("<script>alert('Voordat deze website gebruikt kan worden moeten er eerst gegevens uit de Access-database ge-upload worden.');
		location.href='/admin.php?tp=Uploaden+data';</script>\n");
} elseif ((!isset($lidid) or $lidid == 0) and isset($_SESSION['lidid'])) {
	$lidid = $_SESSION['lidid'];
} else {
	$lidid = 0;
}

if (isset($_GET['op']) and $_GET['op'] == "exportins") {
	header("Content-type: text/plain");
	header("Content-Disposition: attachment; filename=inschrijvingen.sql");
	foreach(db_insbew("export") as $row) {
		echo(SQLexport($row) . "\n");
	}
	exit();
}
if ($currenttab != "Mailing") {
	HTMLheader();
}

if ($currenttab == "Eigen gegevens" and toegang($_GET['tp'])) {
	if ($_SESSION['lidid'] > 0) {
		fnOverviewLid($_SESSION['lidid'], $currenttab2);
	} else {
		echo("<p class='mededeling'>Er is geen lid ingelogd.</p>\n");
	}
} elseif ($currenttab == "Zelfservice" and toegang($_GET['tp'])) {
	if ($currenttab2 == "Inschrijving bewaking") {
		inschrijvenbewaking($_SESSION['lidid']);
	} elseif ($currenttab2 == "Inschrijving evenementen") {
		inschrijvenevenementen($_SESSION['lidid']);
	} elseif ($currenttab2 == "Bestellingen") {
		onlinebestellingen($_SESSION['lidid']);
	} else {
		fnWijzigen($_SESSION['lidid'], $currenttab2);
	}
} elseif ($currenttab == "Overzicht lid" and toegang("Overzicht lid")) {
	if (isset($_GET['lidid']) and is_numeric($_GET['lidid']) and $_GET['lidid'] > 0) {
		fnOverviewLid($_GET['lidid'], $currenttab2);
	} else {
		fnOverviewLid(0, $currenttab2);
	}
} elseif ($currenttab == "Verenigingsinfo" and $currenttab2 != "Introductie" and toegang($_GET['tp'])) {
	fnWieiswie($currenttab2, $kaderoverzichtmetfoto);
} elseif ($currenttab == "Ledenlijst" and toegang($_GET['tp'])) {
	fnLedenlijst();
} elseif ($currenttab == "Bewaking" and toegang($_GET['tp'])) {
	fnBewaking();
} elseif ($currenttab == "Kostenoverzicht" and toegang($_GET['tp'])) {
	fnKostenoverzicht();
} elseif ($currenttab == "Mailing" and toegang($_GET['tp'])) {
	fnMailing();
} elseif ($currenttab == "Evenementen" and toegang($_GET['tp'])) {
	fnEvenementen();
} elseif ($currenttab == "Bestellingen" and toegang($_GET['tp'])) {
	fnWebshop();
} else {
	$currenttab = "Verenigingsinfo";
	$currenttab2 = "Introductie";
	fnVoorblad();
	if (!isset($_SESSION['username']) or strlen($_SESSION['username']) <= 5) {
		echo("<div id='kolomrechts'>\n");
		fnLoginAanvragen();
		echo("</div>  <!-- Einde kolomrechts -->");
	}
}

if ($currenttab != "Mailing") {	
	HTMLfooter();
}

function fnVoorblad($metlogin=0) {

	fnDispMenu(2);

	$myFile = 'templates/verenigingsinfo.html';
	$content = file_get_contents($myFile);
	$begin_blok = "<!-- Ingelogd -->";
	$einde_blok = "<!-- /Ingelogd -->";
	if ($content !== false) {
		if ($_SESSION['lidid'] == 0) {
			$content_uitgelogd = "";
			$offset = 0;
			while ($offset < strlen($content)) {
				if (strpos($content, $begin_blok, $offset) === FALSE) {
					$content_uitgelogd .= substr($content, $offset);
					$offset = strlen($content);
				} else {
					$eb = strpos($content, $begin_blok, $offset);
					$content_uitgelogd .= substr($content, $offset, $eb-$offset);
					$offset = strpos($content, $einde_blok, $offset) + strlen($einde_blok);
				}
			}
			$content = $content_uitgelogd;
		}
	
		// Algemene statistieken 
		$stats = db_stats();
		foreach (array('aantalleden', 'aantalvrouwen', 'aantalmannen', 'gemiddeldeleeftijd', 'aantalkaderleden', 'nieuwstelogin', 'aantallogins', 'nuingelogd') as $v) {
			$content = str_replace("[%" . strtoupper($v) . "%]", htmlentities($stats[$v]), $content);
		}
		$content = str_replace("[%LAATSTGEWIJZIGD%]", strftime("%e %B %Y (%H:%M)", strtotime($stats['laatstgewijzigd'])), $content);
		$content = str_replace("[%LAATSTEUPLOAD%]", strftime("%e %B %Y (%H:%M)", strtotime($stats['laatsteupload'])), $content);
		
		// Gebruiker-specifieke statistieken
		if (isset($_SESSION['lidid']) and $_SESSION['lidid'] > 0) {
			$stats = db_stats($_SESSION['lidid']);
			$content = str_replace("[%NAAMLID%]", $_SESSION['naamingelogde'], $content);
			$content = str_replace("[%LIDNR%]", $_SESSION['lidnr'], $content);
			$content = str_replace("[%INGELOGDEGEWIJZIGD%]", strftime("%e %B %Y (%H:%m)", strtotime($stats['laatstgewijzigd'])), $content);
		} else {
			$content = str_replace("[%INGELOGDEGEWIJZIGD%]", "", $content);
		}
		$content = str_replace("[%KOMENDEEVENEMENTEN%]", ToekomstigeEvenementen(), $content);
		$content = str_replace("[%ROEPNAAM%]", $_SESSION['roepnaamingelogde'], $content);
		
		$content = str_replace("[%VERJAARFOTO%]", overzichtverjaardagen(1), $content);
		$content = str_replace("[%VERJAARDAGEN%]", overzichtverjaardagen(0), $content);

		printf("<div id='welkomstekst'>\n%s</div>  <!-- Einde welkomstekst -->\n", $content);
	} else {
		debug("Geen content voor het voorblad", 0, 0, 1);
	}
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
	$query = "SELECT RecordID, Kode AS Jaar FROM Boekjaar ORDER BY Begindatum DESC;";
	$result = fnQuery($query);
	foreach ($result->fetchAll() as $row) {
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
	$query = "SELECT DISTINCT KSTNPLTS AS Kode FROM Mutatie WHERE KostenplaatsID > 0 ORDER BY KSTNPLTS;";
	$result = fnQuery($query);
	foreach ($result->fetchAll() as $row) {
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
