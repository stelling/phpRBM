<?php

function fnStukken($p_scherm="") {
	global $dtfmt;
	
	$scherm = $p_scherm;
	
	if ($scherm != "O") {
		fnDispMenu(2);
	}
	
	$i_stuk = new cls_Stukken();
	$i_lb = new cls_Logboek();
	$i_ond = new cls_Onderdeel();
	
	if (isset($_GET['p_scherm']) and strlen($_GET['p_scherm']) > 0) {
		$scherm = $_GET['p_scherm'];
	}
	
	if (isset($_POST['stid']) and $_POST['stid'] > 0) {
		$stid = $_POST['stid'];
	} elseif (isset($_GET['p_stid']) and $_GET['p_stid'] > 0) {
		$stid = $_GET['p_stid'];
	} else {
		$stid = 0;
	}
	
	if (isset($_GET['op']) and $_GET['op'] == "delete" and $stid > 0) {
		$i_stuk->delete($stid);
	}
	
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['Toevoegen'])) {
			$i_stuk->add();
		}
		
		if ($_POST['naamdoc'] == "extlink") {
			if (isset($_POST['extlink'])) {
				$_POST['Link'] = $_POST['extlink'];
			} else {
				$_POST['Link'] = "";
			}
		} else {
			$_POST['Link'] = $_POST['naamdoc'];
		}
		
		if ($stid > 0) {
			$row = $i_stuk->record($stid);
			foreach ($row as $col => $val){
				if (isset($_POST[$col])) {
					$i_stuk->update($row->RecordID, $col, $_POST[$col]);
				}
			}
		}
	}
	
	if ($scherm == "F" and $stid > 0) {
		
		$row = $i_stuk->record($stid);
		if (substr($i_stuk->link, 0, 4) == "http") {
			$s = " selected";
		} else {
			$s = "";
		}
		$optlocalfiles = sprintf("<option value='extlink'%s>Externe link</option>\n", $s);
		
		if (is_dir(BASEDIR . "/stukken/")) {
			$d = dir(BASEDIR . "/stukken/");
			while (false !== ($entry = $d->read())) {
				if (substr($entry, 0, 1) != "." and $entry != "..") {
					if ($i_stuk->link == $entry) {
						$s = " selected";
					} else {
						$s = "";
					}
					$optlocalfiles .= sprintf("<option value='%1\$s'%2\$s>%1\$s", $entry, $s);
					$stat = stat(BASEDIR . "/stukken/" . $entry);
					$optlocalfiles .= sprintf(" (%s KB)</option>\n", number_format(($stat['size'] / 1024), 0, ',', '.'));
				}
			}
			$d->close();
		}
		
		printf("<form method='post' id='stukkenmuteren' action='%s?tp=%s&p_scherm=F&p_stid=%d'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $row->RecordID);
		printf("<label>RecordID</label><p>%d</p>\n", $row->RecordID);
		printf("<input type='hidden' name='stid' value=%d>\n", $row->RecordID);
		printf("<label>Titel</label><input type='text' name='Titel' value=\"%s\" class='w50' maxlength=50>\n", str_replace("\"", "'", $row->Titel));
		printf("<label>Bestemd voor</label><input type='text' name='BestemdVoor' value=\"%s\" class='w30'>\n", str_replace("\"", "'", $row->BestemdVoor));
		printf("<label>Zichtbaar voor</label><select name='ZichtbaarVoor'><option value=0>Iedereen</option>%s</select>\n", $i_ond->htmloptions($row->ZichtbaarVoor));
		printf("<label>Vastgesteld op</label><input type='date' name='VastgesteldOp' value='%s'>\n", $row->VastgesteldOp);
		printf("<label>Ingangsdatum</label><input type='date' name='Ingangsdatum' value='%s'>\n", $row->Ingangsdatum);
		printf("<label>Revisiedatum</label><input type='date' name='Revisiedatum' value='%s'>\n", $row->Revisiedatum);
		printf("<label>Vervallen per</label><input type='date' name='VervallenPer' value='%s'>\n", $row->VervallenPer);
			
		$options = "";
		foreach (ARRTYPESTUK as $k => $v) {
			$options .= sprintf("<option value='%s' %s>%s</option>\n", $k, checked($k, "option", $row->Type), $v);
		}
		printf("<label>Type</label><select name='Type'>%s</select>\n", $options);
		printf("<label>Naam document</label><select name='naamdoc' onChange='this.form.submit();'>%s</select>", $optlocalfiles);
		if (substr($row->Link, 0, 4) != "http") {
			$u = BASISURL . sprintf("/get_stuk.php?p_stukid=%d", $row->RecordID);
			printf("<p id='ganaarurl'><a href='%1\$s'>Ga naar</a></p>\n", $u);
		}
		
		if (substr($i_stuk->link, 0, 4) == "http" or (isset($_POST['naamdoc']) and $_POST['naamdoc'] == "extlink")) {
			printf("<label>Externe link</label><input type='url' name='extlink' value='%s'>\n", $row->Link);
			if (substr($i_stuk->link, 0, 4) == "http") {
				printf("<p id='ganaarurl'><a href='%1\$s'>Ga naar</a></p>\n", $row->Link);
			}
		}
		printf("<label>Ingevoerd op</label><p>%s</p>\n", $dtfmt->format(strtotime($row->Ingevoerd)));
		echo("<div class='clear'></div>\n");
		
		$f = sprintf("ReferID=%d", $row->RecordID);
		$lbrows = $i_lb->lijst(22, 0, 0, $f, "DatumTijd DESC", 8);
		if (count($lbrows) > 0) {
			echo(fnDisplayTable($lbrows, fnStandaardKols("logboek"), "Laatste mutaties"));
		}
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<button type='submit'><i class='bi bi-save'></i> Bewaren</button>\n");
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		
		echo("</form>\n");
		
	} elseif ($scherm == "O") {
		
		$rv = sprintf("<table class='%s'>\n", TABLECLASSES);
		$rv .= "<thead>\n";
		$rv .= "<tr><th>Titel</th><th>Bestemd voor</th><th>Zichtbaar voor</th><th>Ingangsdatum/versie</th><th>Revisiedatum</th></tr>\n";
		$rv .= "</thead>\n";
		
		$vc = "ZZ";
		$dtfmt->setPattern(DTTEXT);
		
		foreach ($i_stuk->lijst() as $row) {
			if ($vc != $row->Type) {
				$rv .= sprintf("<tr class='subkop'><td colspan=5>%s</td></tr>\n", ARRTYPESTUK[$row->Type]);
			}
			if (strlen($row->Status) > 0) {
				$rv .= sprintf("<tr class='%s'>", strtolower($row->Status));
			} else {
				$rv .= "<tr>";
			}
			$i_stuk->vulvars($row->RecordID);
			if (strlen($i_stuk->url) > 0) {
				$t = sprintf("<a href='%s'>%s</a>", $i_stuk->url, $row->Titel);
			} else {
				$t = $row->Titel;
			}
			$rv .= sprintf("<td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", $t, $row->BestemdVoor, $row->Zichtbaar, $dtfmt->format(strtotime($row->Ingangsdatum)), $dtfmt->format(strtotime($row->Revisiedatum)));
			$vc = $row->Type;
		}
		
		$rv .= "</table>\n";
		
		return $rv;
		
	} else {
	
		$rows = $i_stuk->lijst();
		
		$kols[0]['link'] = sprintf("%s?tp=%s&p_scherm=F&p_stid=%%d", BASISURL, $_GET['tp']);
		$kols[0]['columnname'] = "RecordID";
		$kols[0]['class'] = "muteren";
		
		$kols[1]['headertext'] = "Titel";
		$kols[2]['headertext'] = "Type";
		$kols[3]['headertext'] = "Bestemd voor";
		
		$kols[4]['headertext'] = "Zichtbaar voor";
		$kols[4]['columnname'] = "Zichtbaar";
		
		$kols[5]['headertext'] = "Vastgesteld op";
		$kols[5]['columnname'] = "VastgesteldOp";
		$kols[5]['type'] = "dateshort";
		
		$kols[6]['headertext'] = "Revisiedatum";
		$kols[6]['columnname'] = "Revisiedatum";
		$kols[6]['type'] = "dateshort";
		
		$kols[7]['headertext'] = "Vervallen per";
		$kols[7]['columnname'] = "VervallenPer";
		$kols[7]['type'] = "dateshort";
		
		$kols[8]['link'] = sprintf("%s?tp=%s&op=delete&p_stid=%%d", BASISURL, $_GET['tp']);
		$kols[8]['columnname'] = "RecordID";
		$kols[8]['class'] = "trash";
		
		echo(fnDisplayTable($rows, $kols));
		
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		echo("<div id='opdrachtknoppen'>\n");
		echo("<button type='submit' name='Toevoegen' title='Stuk toevoegen'><i class='bi bi-plus-circle'></i> Stuk toevoegen</button>\n");
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		echo("</form>\n");
		
	}
	$i_stuk = null;
}  # fnStukken

function fnGewijzigdeStukken() {

	$rv = "";
	if ($_SESSION['lidid'] > 0) {
		$rows = (new cls_Stukken())->gewijzigdestukken(date("Y-m-d", strtotime("-7 day")));
		if (count($rows) > 0) {
			$rv = "<h3>Gewijzigde stukken</h3>\n";
			$rv .= sprintf("<p>Onderstaande stukken gewijzigd sinds je laatste login of korter dan een week geleden.</p>\n<ul>\n", count($rows));
			foreach($rows as $row) {
				$rv .= sprintf("<li>%s</li>\n", $row->Titel);
			}
			$rv .= "</ul>\n";
		}
	}
	
	return $rv;

}  # fnGewijzigdeStukken

public function fnWebsiteMenu() {
	
	fnDispMenu(2);
	
	
}

public function fnWebsiteInhoud() {
	
	fnDispMenu(2);
}

?>
