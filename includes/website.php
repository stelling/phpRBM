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
		
		if (isset($_POST['naamdoc']) and $_POST['naamdoc'] == "extlink") {
			if (isset($_POST['extlink'])) {
				$_POST['Link'] = $_POST['extlink'];
			} else {
				$_POST['Link'] = "";
			}
		} elseif (isset($_POST['naamdoc'])) {
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
		
		if (substr($i_stuk->link, 0, 4) != "http" and file_exists(BASEDIR . "/stukken/" . $i_stuk->link) == false) {
			$optlocalfiles .= sprintf("<option value='%1\$s' selected>%1\$s (bestaat niet)", $i_stuk->link);
		}
		
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
		printf("<label class='form-label'>RecordID</label><p>%d</p>\n", $row->RecordID);
		printf("<input type='hidden' name='stid' value=%d>\n", $row->RecordID);
		printf("<label class='form-label'>Titel</label><input type='text' name='Titel' value=\"%s\" class='w50' maxlength=50>\n", str_replace("\"", "'", $row->Titel));
		printf("<label class='form-label'>Bestemd voor</label><input type='text' name='BestemdVoor' value=\"%s\" class='w30'>\n", str_replace("\"", "'", $row->BestemdVoor));
		printf("<label class='form-label'>Zichtbaar voor</label><select name='ZichtbaarVoor' class='form-select'><option value=0>Iedereen</option>%s</select>\n", $i_ond->htmloptions($row->ZichtbaarVoor));
		printf("<label class='form-label'>Vastgesteld op</label><input type='date' name='VastgesteldOp' value='%s'>\n", $row->VastgesteldOp);
		printf("<label class='form-label'>Ingangsdatum</label><input type='date' name='Ingangsdatum' value='%s'>\n", $row->Ingangsdatum);
		printf("<label class='form-label'>Revisiedatum</label><input type='date' name='Revisiedatum' value='%s'>\n", $row->Revisiedatum);
		printf("<label class='form-label'>Vervallen per</label><input type='date' name='VervallenPer' value='%s'>\n", $row->VervallenPer);
			
		$options = "";
		foreach (ARRTYPESTUK as $k => $v) {
			$options .= sprintf("<option value='%s' %s>%s</option>\n", $k, checked($k, "option", $row->Type), $v);
		}
		printf("<label class='form-label'>Type</label><select name='Type' class='form-select'>%s</select>\n", $options);
		printf("<label class='form-label'>Naam document</label><select name='naamdoc' class='form-select' onChange='this.form.submit();'>\n%s</select>", $optlocalfiles);
		if (substr($row->Link, 0, 4) != "http" and file_exists(BASEDIR . "/stukken/" . $i_stuk->link)) {
			$u = BASISURL . sprintf("/get_stuk.php?p_stukid=%d", $row->RecordID);
			printf("<p id='ganaarurl'><a href='%1\$s'>Ga naar</a></p>\n", $u);
		}
		
		if (substr($i_stuk->link, 0, 4) == "http" or (isset($_POST['naamdoc']) and ($_POST['naamdoc'] == "extlink") or substr($row->Link, 0, 4) == "http")) {
			printf("<label class='form-label'>Externe link</label><input type='url' name='extlink' value='%s'>\n", $row->Link);
			if (substr($i_stuk->link, 0, 4) == "http") {
				printf("<p id='ganaarurl'><a href='%1\$s'>Ga naar</a></p>\n", $row->Link);
			}
		}
		printf("<label class='form-label'>Ingevoerd op</label><p>%s</p>\n", $dtfmt->format(strtotime($row->Ingevoerd)));
		echo("<div class='clear'></div>\n");
		
		$f = sprintf("ReferID=%d", $row->RecordID);
		$lbrows = $i_lb->lijst(22, 0, 0, $f, "DatumTijd DESC", 8);
		if (count($lbrows) > 0) {
			echo(fnDisplayTable($lbrows, fnStandaardKols("logboek"), "Laatste mutaties"));
		}
		
		echo("<div id='opdrachtknoppen'>\n");
		printf("<button type='submit' class='%s'>%s Bewaren</button>\n", CLASSBUTTON, ICONBEWAAR);
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		
		echo("</form>\n");
		
	} elseif ($scherm == "O") {
		
		$rv = sprintf("<table class='%s' id='stukkenlijst'>\n", TABLECLASSES);
		$rv .= "<thead>\n";
		$rv .= "<tr><th>Titel</th><th>Bestemd voor</th><th class='zichtbaarvoor'>Zichtbaar voor</th><th class='ingangsdatum'>Ingangsdatum/versie</th><th class='revisiedatum'>Revisiedatum</th></tr>\n";
		$rv .= "</thead>\n";
		
		$vc = "ZZ";
		$dtfmt->setPattern(DTTEXT);
		
		foreach ($i_stuk->lijst() as $row) {
			$i_stuk->vulvars($row->RecordID);
			if ($vc != $row->Type) {
				$rv .= sprintf("<tr class='subkop'><td colspan=5>%s</td></tr>\n", ARRTYPESTUK[$row->Type]);
			}
			if (strlen($row->Status) > 0) {
				$rv .= sprintf("<tr class='%s'>", strtolower($row->Status));
			} else {
				$rv .= "<tr>";
			}
			if (strlen($i_stuk->url) > 0) {
				$t = sprintf("<a href='%s'>%s</a>", $i_stuk->url, $row->Titel);
			} else {
				$t = $row->Titel;
			}
			$rv .= sprintf("<td>%s</td><td>%s</td><td class='zichtbaarvoor'>%s</td><td class='ingangsdatum'>%s</td><td class='revisiedatum'>%s</td></tr>\n", $t, $row->BestemdVoor, $row->Zichtbaar, $dtfmt->format(strtotime($row->Ingangsdatum)), $dtfmt->format(strtotime($row->Revisiedatum)));
			$vc = $row->Type;
		}
		
		$rv .= "</table>\n";
		
		return $rv;
		
	} else {
	
		$rows = $i_stuk->lijst();
		
		$l = sprintf("%s?tp=%s&p_scherm=F&p_stid=%%d", BASISURL, $_GET['tp']);
		$kols[0] = ['columnname' => "RecordID", 'headertext' => "&nbsp;", 'class' => "muteren", 'link' => $l];

		$kols[1]['headertext'] = "Titel";
		$kols[2]['headertext'] = "Type";
		$kols[3] = ['columnname' => "BestemdVoor", 'headertext' => "Bestemd voor"];
		
		$kols[4] = ['columnname' => "Zichtbaar", 'headertext' => "Zichtbaar voor"];
		$kols[5] = ['columnname' => "VastgesteldOp", 'headertext' => "Vastgesteld op", 'type' => "dateshort"];
		$kols[6] = ['columnname' => "Revisiedatum", 'headertext' => "Revisiedatum", 'type' => "dateshort"];
		$kols[7] = ['columnname' => "VervallenPer", 'headertext' => "Vervallen per", 'type' => "dateshort"];
		
		$l = sprintf("%s?tp=%s&op=delete&p_stid=%%d", BASISURL, $_GET['tp']);
		$kols[8] = ['columnname' => "RecordID", 'headertext' => "&nbsp;", 'class' => "trash", 'link' => $l];
		
		echo(fnDisplayTable($rows, $kols));
		
		printf("<form method='post' id='opdrachtknoppen' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		echo("<button type='submit' name='Toevoegen' title='Stuk toevoegen'><i class='bi bi-plus-circle'></i> Stuk toevoegen</button>\n");
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

function fnWebsiteMenu() {
	
	fnDispMenu(2);
	
	$scherm = $_GET['p_scherm'] ?? "B";
	$wmid = $_GET['p_wmid'] ?? 0;
	
	$i_wm = new cls_Website_menu($wmid);
	$i_wi = new cls_website_inhoud();
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['Toevoegen'])) {
			$i_wm->add();
		}
		
		$i_wm->titel = $_POST['Titel'] ?? "";
		$i_wm->update($i_wm->wmid, "Titel", $i_wm->titel);
		
		$i_wm->vorigelaag = $_POST['VorigeLaag'] ?? 0;
		$i_wm->update($i_wm->wmid, "VorigeLaag", $i_wm->vorigelaag);
		
		$i_wm->volgnr = $_POST['Volgnummer'] ?? 1;
		$i_wm->update($i_wm->wmid, "Volgnummer", $i_wm->volgnr);
		
		$i_wm->inhoudid = $_POST['InhoudID'] ?? 0;
		$i_wm->update($i_wm->wmid, "InhoudID", $i_wm->inhoudid);
		
		$i_wm->externelink = $_POST['ExterneLink'] ?? "";
		$i_wm->update($i_wm->wmid, "ExterneLink", $i_wm->externelink);
		
		$i_wm->gepubliceerd = $_POST['Gepubliceerd'] ?? "";
		$i_wm->update($i_wm->wmid, "Gepubliceerd", $i_wm->gepubliceerd);
		
		if (isset($_POST['BewaarSluit'])) {
			$scherm = "B";
		}
	}
	
	if ($scherm == "B") {
		$l = sprintf("%s?tp=%s&p_scherm=F&p_wmid=%%d", BASISURL, $_GET['tp']);
		$kols[] = ['columnname' => "RecordID", 'class' => "muteren", 'link' => $l];
		$kols[] = ['columnname' => "VorigMenu", 'headertext' => "Vorige laag"];
		$kols[] = ['columnname' => "Volgnummer", 'headertext' => "Volgnr", 'type' => "integer"];
		$kols[] = ['columnname' => "Titel", 'headertext' => "Titel"];
		$kols[] = ['columnname' => "TitelInhoudKort", 'headertext' => "Inhoud"];
		$kols[] = ['columnname' => "Gepubliceerd", 'headertext' => "Gepubliceerd", 'type' => "date"];
		
		$rows = $i_wm->lijst();
		echo(fnDisplayTable($rows, $kols));
		
		printf("<form method='post' id='opdrachtknoppen' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		echo("<button type='submit' name='Toevoegen' title='Menu-item toevoegen'><i class='bi bi-plus-circle'></i> Menu-item toevoegen</button>\n");
		echo("</form>\n");
		
	} elseif ($scherm == "F") {
		
		printf("<form method='post' id='website_menu' action='%s?tp=%s&p_scherm=F&p_wmid=%d'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $wmid);
		
		printf("<label class='form-label'>RecordID</label><p>%d</p>\n", $i_wm->wmid);
		printf("<label class='form-label'>Titel</label><input type='text' name='Titel' value=\"%s\" class='w20' maxlength=20>\n", $i_wm->titel);
		
		$f = sprintf("WM.VorigeLaag=0 AND WM.RecordID<>%d", $i_wm->wmid);
		printf("<label class='form-label'>Vorige laag</label><select name='VorigeLaag' class='form-select'><option value=0>Geen</option>\n%s</select>\n", $i_wm->htmloptions($i_wm->vorigelaag, $f));
		
		printf("<label class='form-label'>Volgnummer</label><input type='number' name='Volgnummer' value=\"%s\" class='num2'>\n", $i_wm->volgnr);
		
		printf("<label class='form-label'>Inhoud</label><select name='InhoudID' class='form-select'><option value=0>Geen</option>\n%s</select>\n", $i_wi->htmloptions($i_wm->inhoudid));
		
		printf("<label class='form-label'>Directe link</label><input type='url' name='ExterneLink' value=\"%s\" title='Directe link naar een andere website'>\n", $i_wm->externelink);
		
		printf("<label class='form-label'>Gepubliceerd</label><input type='date' name='Gepubliceerd' value=\"%s\" title='Zichtbaar vanaf'>\n", $i_wm->gepubliceerd);
		
		echo("<div id='opdrachtknoppen'>\n");
		printf("<button type='submit' class='%s' name='Bewaar' title='Bewaar menu-item'>%s Bewaren</button>\n", CLASSBUTTON, ICONBEWAAR);
		printf("<button type='submit' class='%s' name='BewaarSluit' title='Bewaar menu-item'>%s Bewaren en sluiten</button>\n", CLASSBUTTON, ICONSLUIT);
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		
		echo("</form>\n");
	}
	
}

function fnWebsiteInhoud() {
	
	fnDispMenu(2);
	
	$scherm = $_GET['p_scherm'] ?? "B";
	$wiid = $_GET['p_wiid'] ?? 0;
	$i_wi = new cls_Website_inhoud($wiid);
	$ext_toegestaan = "pdf, gif, jpg, jpeg, png";
	if ($i_wi->wiid > 0) {
		$ad = $_SESSION['settings']['website_mediabestanden'] . $i_wi->wiid . "/";
	} else {
		$ad = "";
	}
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		
		if (isset($_POST['Toevoegen'])) {
			$wiid = $i_wi->add();
			$scherm = "F";
		}
		
		$i_wi->titel = $_POST['Titel'] ?? "";
		$i_wi->update($i_wi->wiid, "Titel", $i_wi->titel);
		if (isset($_POST['HTMLdirect'])) {
			$i_wi->htmldirect = 1;
		} else {
			$i_wi->htmldirect = 0;
		}
		$i_wi->update($i_wi->wiid, "HTMLdirect", $i_wi->htmldirect);
		
		if (isset($_POST['tekst'])) {
			$i_wi->setTekst($_POST['tekst']);
		}
		
		if (isset($_FILES['UploadFile']['name']) and strlen($_FILES['UploadFile']['name']) > 3) {
			$max_size_attachm = $_SESSION['settings']['max_size_attachm'] ?? 2048;
			$target = $ad . $_FILES['UploadFile']['name'];
			
			if (!file_exists($ad)) {
				if (mkdir($ad, 0755, true) == true) {
					$mess = sprintf("Folder '%s' is aangemaakt.", $ad);
				} else {
					$mess = sprintf("Folder '%s' bestaat niet en kan niet worden aangemaakt. Probeer het later opnieuw of neem contact op met de webmaster.", $ad);
					$ad = "";
				}
				(new cls_Logboek())->add($mess, 22, 0, 2, $i_wi->wiid, 29);
			} else {
				chmod($ad, 0755);
			}

			$ext = explode(".", $target);
			$ext = strtolower($ext[count($ext) - 1]);
			if (strpos($ext_toegestaan, $ext) === false) {
				$mess = sprintf("Bestand '%s' niet worden ge-upload, omdat de extensie niet is toegestaan.", $_FILES['UploadFile']['name']);
			} elseif (isset($_POST['UploadFile']) and $_FILES['UploadFile']['size'] > $max_size_attachm) {
				$mess = sprintf("Bestand '%s' niet worden bijgesloten, omdat het te groot is.", $_FILES['UploadFile']['name']);
			} elseif (strlen($ad) > 0) {
				if (move_uploaded_file($_FILES['UploadFile']['tmp_name'], $target) == false) {
					$mess = sprintf("Fout %d is opgetreden bij het uploaden van bestand '%s'. Probeer het later opnieuw of neem contact op met de webmaster.", $_FILES['UploadFile']['error'], $_FILES['UploadFile']['name']);
				} else {
					$mess = sprintf("Aan Website_inhoud %d is bestand '%s' toegevoegd.", $i_wi->wiid, $_FILES['UploadFile']['name']);
				}
			}
			(new cls_Logboek())->add($mess, 22, 0, 1, $i_wi->wiid, 25);
		}
		
		if (isset($_POST['BewaarSluit'])) {
			$scherm = "B";
		}
	}
	
	if ($scherm == "B") {
		
		$l = sprintf("%s?tp=%s&p_scherm=F&p_wiid=%%d", BASISURL, $_GET['tp']);
		$kols[] = ['columnname' => "RecordID", 'class' => "muteren", 'link' => $l];
		$kols[] = ['columnname' => "Titel", 'headertext' => "Titel"];
		
		$rows = $i_wi->basislijst("", "WI.Titel, WI.RecordID");
		echo(fnDisplayTable($rows, $kols));
		
		printf("<form method='post' id='opdrachtknoppen' action='%s?tp=%s&p_scherm=B'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		printf("<button type='submit' class='%s' name='Toevoegen' title='Inhoud toevoegen'>%s Inhoud</button>\n", CLASSBUTTON, ICONTOEVOEGEN);
		
		echo("</form>\n");
		
	} elseif ($scherm == "F" and $wiid > 0) {
		$tabbijlagefiles = "";
		$imglist = "";
		if (strlen($ad) > 0 and is_dir($ad)) {
			$d = dir($ad);
			while (false !== ($entry = $d->read())) {
				if ($entry != "." and $entry != "..") {
					$tabbijlagefiles .= sprintf("<tr><td><a href='%s%s'>%s</a></td>", $ad, $entry, $entry);
					$stat = stat($ad . $entry);
					$tabbijlagefiles .= sprintf("<td>%s KB</td>", number_format(($stat['size'] / 1024), 0, ',', '.'));
//					$tabbijlagefiles .= sprintf("<td><button type='submit' name='del_attach_%d' alt='Verwijderen' title='Verwijder %s'><i class='bi bi-trash'></i></button></td>\n", $bnr, $entry);
//					$tabbijlagefiles .= sprintf("<input type='hidden' name='name_attach_%d' value='%s'>\n", $bnr, str_replace(".", "!", $entry));
					$tabbijlagefiles .= "</tr>\n";
					if (strlen($imglist) > 0) {
						$imglist .= ",\n";
					}
					if (in_array(substr($entry, -4), array(".gif", ".jpg", ".jpeg", ".png"))) {
						$imglist .= sprintf("{ title: '%s', value: '%s' }", $entry, $ad . $entry);
					}
				}
			}
			$d->close();
		}
		
		printf("<form method='post' id='website_inhoud' action='%s?tp=%s&p_scherm=F&p_wiid=%d' enctype='multipart/form-data'>\n", $_SERVER['PHP_SELF'], $_GET['tp'], $wiid);
		
		printf("<label class='form-label'>RecordID</label><p>%d</p>\n", $i_wi->wiid);
		printf("<label class='form-label'>Titel</label><input type='text' name='Titel' value=\"%s\" class='w80' maxlength=80>\n", $i_wi->titel);
		echo("<div class='form-switch'>");
		printf("<label class='form-label'>HTML direct</label><input type='checkbox' class='form-check-input' name='HTMLdirect'%s title='Zonder editor' onClick='this.form.submit();'>\n", checked($i_wi->htmldirect));
		echo("</div>\n");
		
		$stylesheettekst = "";
		if ($i_wi->htmldirect == 0) {
			if (file_exists(BASEDIR . '/www/default.css')) {
				$stylesheet = "www/default.css";
			} else {
				$stylesheet = "";
			}
			if (file_exists(BASEDIR . '/www/kleur.css')) {
				if (strlen($stylesheet) > 0) {
					$stylesheet .= ", ";
				}
				$stylesheet .= "www/kleur.css";
			}
			if (strlen($stylesheet) > 0) {
				$stylesheettekst = sprintf("Stylesheet(s) %s is/zijn aan de editor gekoppeld.", $stylesheet);
				$stylesheet = sprintf("content_css: '%s',", $stylesheet);
			} else {
				$stylesheettekst = "Er is geen stylesheet aan de editor gekoppeld.";
			}
			if (strlen($imglist) > 0) {
				$imglist = sprintf("image_list: [\n%s\n],", $imglist);
			}			
			printf("<script>
			tinymce.init({
				selector: '#tekst',
				placeholder: 'Tekst hier ...',
				theme: 'silver',
				mobile: { theme: 'silver' },
				menubar: true,
				height: 500,
				relative_urls: false,
				remove_script_host: false,
				convert_urls: true,
				remove_script_host: false,
				plugins: 'link lists table paste image importcss',
				paste_data_images: false,
				paste_as_text: true,
				%s
				%s
				importcss_append: true,
				menu: {
					table: { title: 'Table', items: 'inserttable | cell row column | tableprops deletetable' }
				},
				toolbar: 'undo redo | styles | bold italic | alignleft aligncenter alignright alignjustify | bullist numlist | link image ',
				menubar: 'edit insert format tools table',
			});
			</script>\n", $stylesheet, $imglist);
		}
		
		echo("<div class='clear'></div>\n");
		printf("<textarea name='tekst' id='tekst'>%s</textarea>\n", $i_wi->tekst);	
		if (strlen($stylesheettekst) > 0) {
			printf("<p>%s</p>\n", $stylesheettekst);
		}
		
		echo("<label class='form-label'>Bijbehorende bestanden</label>");
		echo("<div id='website_bestanden'>\n");
		
			if (strlen($tabbijlagefiles) > 0) {
				printf("<table>%s</table>\n", $tabbijlagefiles);
			}
		
			echo("<input type='file' name='UploadFile'>\n");
			echo("<input type='submit' name='UploadBijlage' value='Upload bijlage'>");
			printf("<p>(Extensies '%s' zijn toegestaan)</p>\n", $ext_toegestaan);
		
		echo("</div> <!-- Einde website_bestanden -->\n");

		echo("<div id='opdrachtknoppen'>\n");
		printf("<button type='submit' class='%s' name='Bewaar' title='Bewaar inhoud'>%s Bewaren</button>\n", CLASSBUTTON, ICONBEWAAR);
		printf("<button type='submit' class='%s' name='BewaarSluit' title='Bewaar menu-item'>%s Bewaren en sluiten</button>\n", CLASSBUTTON, ICONSLUIT);
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		
		echo("</form>\n");
		
	}
}

?>
