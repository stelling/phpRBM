<?php

$_SESSION['lidid'] = $_SESSION['lidid'] ?? 0;
$_SESSION['webmaster'] = 0;
if ($_SESSION['lidid'] == 0 or strlen($_SESSION['username']) == 0) {
	$_SESSION['lidid'] = 0;
	$_SESSION['username'] = "";
	define("WEBMASTER", false);
} else {
	if (in_array($_SESSION['lidid'], $lididwebmasters)) {
		$_SESSION['webmaster'] = 1;
		define("WEBMASTER", true);
	} else {
		define("WEBMASTER", false);
	}
}

$tabpages = null;
if (substr($_SERVER['PHP_SELF'], -9) == "admin.php") {
	addtp("Beheer logins");
	addtp("Autorisatie");
	addtp("Eigen lijsten");
	addtp("Templates");
	addtp("Instellingen");
	addtp("Onderhoud");
	addtp("Logboek");
} else {
	addtp("Vereniging");
	addtp("Agenda");
	if ($_SESSION['lidid'] > 0) {
		addtp("Eigen gegevens");
	} elseif ((new cls_Login())->aantal() > 0) {
		addtp("Inloggen");
	} else {
		debug("Er zijn geen logins aanwezig.");
	}
	$i_lo = new cls_Lidond();

	if ($currenttab == "Eigen gegevens" or ($currenttab2 == "Overzicht lid")) {
		if ($currenttab == "Eigen gegevens") {
			$b = $currenttab . "/";
			$lidid = $_SESSION['lidid'];
		} elseif ($currenttab2 == "Overzicht lid") {
			$b = $currenttab . "/" . $currenttab2 . "/";
			if (!isset($_GET['lidid']) or strlen($_GET['lidid']) == 0) {
				$lidid = 0;
			} else {
				$lidid = $_GET['lidid'];
			}
		}
		$gs = (new cls_Lid())->Geslacht($lidid);
	}
	
	if ($_SESSION['lidid'] > 0) {
		addtp("Zelfservice");
	}
	if ($currenttab == "Zelfservice") {
		$gs = (new cls_Lid())->Geslacht($_SESSION['lidid']);
		
		$f = sprintf("Datum >= CURDATE() AND IFNULL(VerwijderdOp, '1900-01-01') < '2012-01-01' AND InschrijvingOpen=1 AND BeperkTotGroep IN (%s)", $_SESSION["lidgroepen"]);
		if ((new cls_Evenement())->aantal($f) > 0) {
			addtp("Zelfservice/Evenementen");
		}
		
		$i_ak = new cls_afdelingskalender();
		$i_ak->where = sprintf("AK.Datum >= CURDATE() AND AK.OnderdeelID IN (%s) AND AK.OnderdeelID IN (SELECT O.RecordID FROM %sOnderdl AS O WHERE O.AfmeldenMogelijk > 0)", $_SESSION['lidgroepen'], TABLE_PREFIX);
		if ($i_ak->aantal() > 0) {
			addtp("Zelfservice/Afmelden");
		}
		$i_ak = null;
		
		addtp("Zelfservice/Algemene gegevens");
		$f = "Zelfservice=1";
		$_SESSION['aantaltoestemmingen'] = $_SESSION['aantaltoestemmingen'] ?? (new cls_Onderdeel())->aantal("`Type`='T' AND IFNULL(VervallenPer, '9999-12-31') >= CURDATE()");
		if ($_SESSION['aantaltoestemmingen'] > 0 and $gs != "B") {
			addtp("Zelfservice/Toestemmingen");
		}
		if ($gs != "B" and (new cls_diploma())->aantal($f) > 0) {
			addtp("Zelfservice/Diploma's");
		}
		addtp("Zelfservice/Pasfoto");
		$_SESSION['settings']['zs_muteerbarememos'] = $_SESSION['settings']['zs_muteerbarememos'] ?? "";
		if (strlen($_SESSION['settings']['zs_muteerbarememos']) > 0) {
			addtp("Zelfservice/Bijzonderheden");
		}
		
		if (count((new cls_Artikel())->lijst("bestellijst")) > 0) {
			addtp("Zelfservice/Bestellingen");
		}
		if ((new cls_Lidmaatschap())->kanopzeggen($_SESSION['lidid']) == true) {
			addtp("Zelfservice/Opzegging");
		}
	}
	if ($_SESSION['lidid'] > 0) {
		addtp("Zelfservice/Wijzigen wachtwoord");
	}
	
	addtp("Ledenlijst");
	if ($currenttab == "Ledenlijst") {
		addtp("Ledenlijst/Leden");
		if ((new cls_Lidmaatschap())->aantal("IFNULL(LM.Opgezegd, '9999-12-31') < CURDATE()") > 0) {
			addtp("Ledenlijst/Voormalig leden");
		}
		if ((new cls_Lidmaatschap())->aantal("LIDDATUM > CURDATE()")) {
			addtp("Ledenlijst/Toekomstige leden");
		}
		
		$_SESSION['aantalklosleden'] = $_SESSION['aantalklosleden'] ?? (new cls_Lid())->aantalklosleden();
		if ($_SESSION['aantalklosleden'] > 0) {
			addtp("Ledenlijst/Klosleden");
		}
		addtp("Ledenlijst/Nieuw (klos)lid");
		if ((new cls_Inschrijving())->aantal("(Ins.Verwerkt IS NULL) AND (Ins.Verwijderd IS NULL)") > 0) {
			addtp("Ledenlijst/Wachtlijst");
		}
		if ((new cls_Onderdeel())->aantal("`Type`='G' AND IFNULL(VervallenPer, '9999-12-31') >= CURDATE()") > 0) {
			addtp("Ledenlijst/Groepen");
		}
		if ((new cls_Onderdeel())->aantal("`Type`='R' AND IFNULL(VervallenPer, '9999-12-31') >= CURDATE()") > 0) {
			addtp("Ledenlijst/Rollen");
		}
		if ((new cls_Onderdeel())->aantal("`Type`='M' AND IFNULL(VervallenPer, '9999-12-31') >= CURDATE()") > 0) {
			addtp("Ledenlijst/Materiaal");
		}
		

		addtp("Ledenlijst/Rapporten");
		if ($currenttab2 == "Rapporten") {
			addtp("Ledenlijst/Rapporten/Jubilarissen");
			addtp("Ledenlijst/Rapporten/Presentielijst");
			$f = "O.`Type`='T'";
			if ((new cls_Onderdeel())->aantal($f) > 0) {
				addtp("Ledenlijst/Rapporten/Toestemmingen");
			}
		}
		
		$b = "Ledenlijst/Basisgegevens";
		addtp($b);
		if ($currenttab2 == "Basisgegevens") {
			addtp($b . "/Onderdelen");
			if ((new cls_Onderdeel())->aantal("O.`Type`='A'") > 0) {
				addtp($b . "/Afdelingen");
			}
			addtp($b . "/Commissies");
			if ((new cls_Onderdeel())->aantal("`Type`='S'") > 0) {
				addtp($b . "/Selecties");
			}
			addtp($b . "/Functies");
			addtp($b . "/Activiteiten");
			addtp($b . "/Seizoenen");
			addtp($b . "/Organisaties");
		}
		
		if ((new cls_Onderdeel())->aantal("O.`ORGANIS`=1") > 0) {
			addtp("Ledenlijst/Sportlink");
		}
		addtp("Ledenlijst/Instellingen");
		addtp("Ledenlijst/Logboek");
	}
	
	addtp("Mailing");
	if ($currenttab == "Mailing") {
		$i_M = new cls_Mailing();
		$i_mv = new cls_Mailing_vanaf();
		$i_mh = new cls_Mailing_hist();
		$i_ond = new cls_Onderdeel();
		
		if ($i_M->aantal() > 0) {
			addtp("Mailing/Muteren");
		}
		
		if ($i_mv->aantal() > 0) {
			addtp("Mailing/Nieuw");
		}
		$_SESSION['aantalrekeningen'] = $_SESSION['aantalrekeningen'] ?? (new cls_Rekening())->aantal();
		if ($_SESSION['aantalrekeningen'] > 0) {
			addtp("Mailing/Rekeningen");
		}
		
		$f = "(deleted_on IS NOT NULL)";
		if ($i_M->aantal($f) > 0) {
			addtp("Mailing/Prullenbak");
		}
		
		if ($i_mh->aantal() > 0) {
			addtp("Mailing/Outbox");
			addtp("Mailing/Verzonden mails");
		}
		
		$f = sprintf("O.LedenMuterenDoor > 0 AND O.LedenMuterenDoor IN (%s)", $_SESSION['lidgroepen']);
		if ($i_ond->aantal($f) > 0) {
			addtp("Mailing/Groepen muteren");
		}
		
		addtp("Mailing/Instellingen");
		addtp("Mailing/Logboek");
		$i_M = null;
		$i_mv = null;
		$i_mh = null;
		$i_ond = null;
		if ($currenttab2 == "Wijzigen mailing") {
			addtp($currenttab . "/" . $currenttab2);
		}
	}
	
	$b = "Diplomazaken";
	addtp($b);
	if ($currenttab == $b) {
		$i_dp = new cls_Diploma();
		addtp($b . "/Beheer");
		if ($i_dp->aantal() > 0) {
			addtp($b . "/Examenonderdelen");
		}
//		addtp($b . "/Examens muteren");
	}
	
	$ondinmenu = $_SESSION['settings']['menu_met_afdelingen'] ?? "";
	if (strlen($ondinmenu) > 0) {
		// Tabblad per afdeling/onderdeel
		
		$i_ond = new cls_Onderdeel();
		$i_ond->where = sprintf("O.RecordID IN (%s)", $ondinmenu);
		foreach (explode(",", $ondinmenu) as $ondid) {
			$i_ond->vulvars($ondid);
			if (strlen($i_ond->naam) > 15) {
				$menukop = trim($i_ond->code);
			} else {
				$menukop = trim($i_ond->naam);
			}
			addtp($menukop, $ondid);
			if ($currenttab == $menukop) {
				if ($i_ond->type == "A") {
					addtp($menukop . "/Afdelingslijst", $ondid);
					addtp($menukop . "/Kalender");
					$f = sprintf("LO.GroepID > 0 AND IFNULL(LO.Opgezegd, '9999-12-31') >= CURDATE() AND LO.OnderdeelID=%d", $ondid);
					if ((new cls_Lidond())->aantal($f) > 0) {
						addtp($menukop . "/Groepsindeling", $ondid);
						addtp($menukop . "/Indeling muteren", $ondid);
					}
					addtp($menukop . "/Groepen muteren");
				}
				$f = sprintf("AK.OnderdeelID=%d AND AK.Datum > DATE_SUB(CURDATE(), INTERVAL 9 MONTH) AND AK.Activiteit=1", $ondid);
				if ((new cls_Afdelingskalender())->aantal($f) > 0) {
					addtp($menukop . "/Presentie muteren");
					if ((new cls_Aanwezigheid())->aantalstatus("*", $ondid) > 0) {
						addtp($menukop . "/Presentie per seizoen");
						addtp($menukop . "/Presentieoverzicht");
					}
				}
					
				$f = sprintf("DP.Afdelingsspecifiek=%d", $ondid);
				if ((new cls_Diploma())->aantal($f) > 0) {
					$tp = $menukop . "/Examens";
					addtp($tp);
					$tp = $menukop . "/Diploma's";
					addtp($tp);
				}
										
				$f = sprintf("Ins.OnderdeelID=%d AND (Ins.Verwerkt IS NULL) AND (Ins.Verwijderd IS NULL)", $ondid);
				if ((new cls_Inschrijving())->aantal($f) > 0) {
					addtp($menukop . "/Wachtlijst");
				}
				
				if ($i_ond->type == "A" and toegang("Mailing/Muteren", 0, 0)) {
					$tp = $menukop. "/Afdelingsmailing";
					addtp($tp);
				}
				
				$f = "O.`Type`='T'";
				if ((new cls_Onderdeel())->aantal($f) > 0) {
					addtp($menukop . "/Toestemmingen");
				}
				addtp($menukop . "/Logboek");
			}
		}
	}

	if (!isset($_GET['lidid']) or strlen($_GET['lidid']) == 0) {
		$_GET['lidid'] = 0;
	}
	$i_lid = new cls_Lid($_GET['lidid']);
	$i_ond = new cls_Onderdeel();
	$i_ond->where = "IFNULL(O.VervallenPer, '9999-12-31') > CURDATE()";

	if ($currenttab2 == "Wijzigen lid") {
		$b = $currenttab . "/" . $currenttab2;
		addtp($b);
		$b .= "/";
		addtp($b . "Algemene gegevens");
		addtp($b . "Financieel");
		if ($i_lid->geslacht != "B") {
			addtp($b . "Lidmaatschap");
		}
		if ($i_ond->aantal("Type='A'") > 0 and $i_lid->soortlid != "Kloslid") {
			addtp($b . "Afdelingen");
		}
		if ($i_lid->geslacht != "B" and $i_ond->aantal("Type='T'") > 0) {
			addtp($b . "Toestemmingen");
		}
		if ($i_lid->geslacht != "B") {
			addtp($b . "B, C en F");
		}
		if ((new cls_Onderdeel())->aantal("Type='G'") > 0) {	
			addtp($b . "Groepen");
		}
		if ((new cls_Onderdeel())->aantal("Type='O'") > 0 and $i_lid->geslacht != "B") {	
			addtp($b . "Onderscheidingen");
		}
		if ($i_lid->geslacht != "B" and (new cls_Diploma())->aantal() > 0) {
			addtp($b . "Diploma's");
		}

		if (strlen($_SESSION['settings']['muteerbarememos']) > 0) {
			addtp($b . "Bijzonderheden");
		}
		if ($i_lid->geslacht != "B") {
			addtp($b . "Pasfoto");
		}
	}
	
	addtp("Evenementen");
	if ($currenttab == "Evenementen") {
		$i_ev = new cls_Evenement();
		$i_ond = new cls_Onderdeel();
		$f = "IFNULL(VerwijderdOp, '1900-01-01') < '2012-01-01'";
		if ($i_ev->aantal($f) > 0) {
			if (count($i_ev->lijst(1)) > 0) {
				addtp("Evenementen/Overzicht");
			}
			addtp("Evenementen/Beheer");
		}
		if ((new cls_Evenement_Type())->aantal() > 0) {
			addtp("Evenementen/Nieuw");
		}
		
		if (count($i_ev->lijst(6)) > 0) {
			addtp("Evenementen/Presentielijst");
		}
		
		$f = sprintf("O.LedenMuterenDoor > 0 AND O.LedenMuterenDoor IN (%s)", $_SESSION['lidgroepen']);
		if ($i_ond->aantal($f) > 0) {
			addtp("Evenementen/Groepen muteren");
		}
		
		addtp("Evenementen/Types muteren");
		addtp("Evenementen/Logboek");
		$i_ev = null;
		$i_ond= null;
	}	
	
	if ((new cls_Seizoen())->aantal() > 0) {
		addtp("Rekeningen");
		if ($currenttab == "Rekeningen") {
			$b = "Rekeningen/";
			addtp($b . "Beheer");
			addtp($b . "Nieuw");
			addtp($b . "Aanmaken rekeningen");
			addtp($b . "Betalingen");
			addtp($b . "Instellingen");
			addtp($b . "Logboek");
		}
	}
	
	addtp("Bestellingen");
	if ($currenttab == "Bestellingen") {
		if ((new cls_Artikel())->aantal() > 0) {
			addtp("Bestellingen/Beheer");
		}
		addtp("Bestellingen/Artikelbeheer");
		if ((new cls_Artikel())->aantal() > 0) {
			addtp("Bestellingen/Voorraadbeheer");
		}
		if ((new cls_Logboek())->aantal("TypeActiviteit=10") > 0) {
			addtp("Bestellingen/Logboek");
		}
	}
	
	addtp("Website");
	if ($currenttab == "Website") {
		addtp("Website/Stukken");
		addtp("Website/Menu");
		addtp("Website/Inhoud");
		addtp("Website/Logboek");
	}
	
//		addtp("DMS");   Voorlopig geen tijd voor, dus uitgezet.
	
	if ($_SESSION['lidid'] > 0) {
		$i_el = new cls_Eigen_lijst();
		$i_el->where = "(EL.AantalKolommen > 0 OR LENGTH(EL.EigenScript) > 4) AND LENGTH(EL.Tabpage) > 0";
		foreach($i_el->basislijst("", "EL.Naam") as $row) {
			$i_el->vulvars($row->RecordID);
			if (($row->AantalKolommen > 0 and $i_el->aantalrijen > 0) or strlen($i_el->eigenscript) > 4) {
				addtp($row->Tabpage . "/" . $row->Naam);
			}
		}
		$i_el->where = sprintf("EL.GroepMelding > 0 AND EL.GroepMelding IN (%s)", $_SESSION["lidgroepen"]);
		foreach($i_el->basislijst() as $row) {
			$i_el->controle($row->RecordID, 30);
		}
	}
}

function fnAuthenticatie($melding=1, $password="", $p_napost=0) {
	/*
		melding: 1 = toon melding aan gebruiker
		p_napost: 1 = de gebruiker heeft op de knop inloggen geklikt en dus geen automatisch login.
	*/
	global $lididwebmasters;
	
	$i_login = new cls_Login();
	$i_lb = new cls_Logboek();
	
	if ((!isset($_SESSION['username']) or strlen($_SESSION['username']) == 0)) {
		if (isset($_COOKIE['username'])) {
			$_SESSION['username'] = $_COOKIE['username'];
		} else {
			$_SESSION['username'] = "";
		}
	}
	
	$pwfromcookie = 0;	
	if (strlen($password) == 0 and isset($_COOKIE['password']) and strlen($_COOKIE['password']) > 4) {
		$password = $_COOKIE['password'];
		$pwfromcookie = 1;
	}
		
	$mess = "";
	if ($_SESSION['settings']['login_maxinlogpogingen'] > 0 and $i_lb->iplogincontrole() > $_SESSION['settings']['login_maxinlogpogingen']) {
		$mess = sprintf("Inloggen vanaf IP-adres '%s' is geblokkeerd, omdat er teveel mislukte inlog-pogingingen mee gedaan zijn! Je bent niet ingelogd.", $_SERVER['HTTP_USER_AGENT']);
		$_SESSION['lidid'] = 0;
			
	} elseif (isset($_SESSION['username']) and strlen($_SESSION['username']) > 0 and strlen($password) > 4) {
		$row = $i_login->controle($password, $pwfromcookie);
		if ($_SESSION['settings']['login_maxinlogpogingen'] > 0 and isset($row->FouteLogin) and $row->FouteLogin > $_SESSION['settings']['login_maxinlogpogingen']) {
			if ($_SESSION['settings']['login_autounlock'] > 0 and $_SESSION['settings']['login_autounlock'] < 12*60) {
				$mess = sprintf("Login '%s' is tot uiterlijk %s uur geblokkeerd", $_SESSION['username'], date("G:i", strtotime($row->Gewijzigd)+(60*$_SESSION['settings']['login_autounlock'])));
			} else {
				$mess = sprintf("Login '%s' is geblokkeerd", $_SESSION['username']);
			}
			$mess .= ", omdat er teveel mislukte inlog-pogingingen mee gedaan zijn! Je bent niet ingelogd.";
			
			$_SESSION['lidid'] = 0;
			
		} elseif (isset($row->ActivatieKey) and strlen($row->ActivatieKey) > 5) {
			$mess = sprintf("Login '%s' is nog niet gevalideerd en kan nog niet worden gebruikt.", $row->Login);
			$_SESSION['lidid'] = 0;
			
		} elseif (isset($row->LidID) and $row->LidID > 0) {
			$i_lid = new cls_Lid($row->LidID);
			
			$_SESSION['lidid'] = $row->LidID;
			$_SESSION['lidnr'] = $i_lid->lidnr;
			$_SESSION['naamingelogde'] = $i_lid->naamlid;
			$_SESSION['roepnaamingelogde'] = $i_lid->roepnaam;
			$_SESSION['emailingelogde'] = $i_lid->email;
			
			$i_login->setingelogd($row->LidID, $p_napost);
			(new cls_Parameter())->vulsessie();
		} else {
			if ($pwfromcookie == 0) {
				$mess = sprintf("De combinatie van login '%s' en het ingevoerde wachtwoord is niet correct! Je bent niet ingelogd.", $_SESSION['username']);
			}
			setcookie("password", "", time()-60);
			$_SESSION['lidid'] = 0;
		}
	} else {
		$_SESSION['lidid'] = 0;
	}
	if ($melding == 1 and strlen($mess) > 0) {
		(new cls_Logboek())->add($mess, 1, 0, 3, 0, 4);
	} elseif (strlen($mess) > 0) {
		(new cls_Logboek())->add($mess, 1, 0, 0, 0, 4);
	}
	if ($_SESSION['lidid'] == 0 or strlen($_SESSION['username']) == 0) {
		$_SESSION['lidid'] = 0;
		$_SESSION['username'] = "";
		unset($_SESSION['lidgroepen']);
		unset($_SESSION['lidauth']);
	}
	
} # fnAuthenticatie
	
function toegang($soort, $melding=1, $p_log=1, $p_alleenmenu=0) {
	global $tabpages, $currenttab, $currenttab2, $currenttab3;
	
	$i_aut = new cls_Authorisation();
	$rv = false;

	$arrAltijdToegang[] = "Inloggen";
	$arrAltijdToegang[] = "Validatie login";
	$arrAltijdToegang[] = "Bevestiging login";
	if ($_SESSION['lidid'] == 0) {
		$arrAltijdToegang[] = "Login aanvragen";
		$arrAltijdToegang[] = "Herstel wachtwoord";
		$arrAltijdToegang[] = "Opvragen lidnr";
	}
	if (strlen($soort) == 0) {
		$soort = $currenttab;
		if (strlen($currenttab2) > 0) {
			$soort .= "/" . $currenttab2;
		}
		if (strlen($currenttab3) > 0) {
			$soort .= "/" . $currenttab3;
		}
	}
	
	$soort = str_replace("'", "", $soort);
		
	if (strlen($soort) == 0 or in_array($soort, $arrAltijdToegang) === true) {
		$rv = true;
		
	} else {
		if ($i_aut->toegang($soort, -1, $p_alleenmenu) == true) {
			$rv = true;
		} else {
			$mess = sprintf("Je hebt geen toegang tot '%s'.", $soort);
			if ($p_log == 1) {
				(new cls_Logboek())->add($mess, 15, 0, $melding, 0, 9);
				
			} elseif ($melding > 0) {
				printf("<p class='mededeling'>%s</p>\n", $mess);
			}
			$rv = false;
		}
	}
	
	$i_aut = null;
	
	return $rv;
	
}  # toegang

function fnInloggen() {
	printf("<form action='%s/index.php' id='login' class='form-check form-switch' method='post'>\n", BASISURL);
	echo("<h2>Inloggen</h2>\n");
	echo("<label class='form-label'>Login</label><input type='text' name='username' class='w15'>\n");
	printf("<label class='form-label'>Wachtwoord</label><input type='password' name='password' class='w%d'>\n", $_SESSION['settings']['wachtwoord_maxlengte']);
	echo("<label class='form-label'>Ingelogd blijven?</label><input type='checkbox' class='form-check-input' name='cookie' value=1 title='Ingelogd blijven'><p>(werkt met cookies)</p>\n");
	echo("<p>Heb je nog geen login, dan kan je die <a href='index.php?tp=Login+aanvragen'>aanvragen</a>.</p>");
	echo("<p>Ben je je wachtwoord vergeten, dan kan je deze <a href='\?tp=Herstel+wachtwoord'>opnieuw instellen</a>.</p>\n");

	echo("<div id='opdrachtknoppen'>\n");
	printf("<button type='submit' class='%s' name='Inloggen'>%s Inloggen</button>\n", CLASSBUTTON, ICONSUBMIT);
	echo("</div> <!-- Einde opdrachtknoppen -->\n");

	echo("</form>\n");

}  # fnInloggen

function fnHerstellenWachtwoord($stap="", $lidid=0) {
	
	$mess = "";
	$i_login = new cls_Login();
	
	if (strlen($stap) == 0 and isset($_POST['stap']) and strlen($_POST['stap']) > 0) {
		$stap = $_POST['stap'];
	}

	if ($stap == "mail" and $lidid > 0) {

		$i_login->vulvars($lidid);
		if ($_SESSION['settings']['login_maxinlogpogingen'] > 0 and $i_login->foutelogin > $_SESSION['settings']['login_maxinlogpogingen']) {
			$mess = "Er is teveel foutief met deze login proberen in te loggen. Hierdoor is dit account geblokkeerd. ";
			if ($_SESSION['settings']['login_autounlock'] > 0 and $_SESSION['settings']['login_autounlock'] <= 24*60) {
				$mess .= sprintf("Uiterlijk om %s uur is deze blokkade verwijderd.", date("G:i", strtotime($i_login->gewijzigd)+(60*$_SESSION['settings']['login_autounlock'])));
			} else {
				$mess .= sprintf("Vraag aan de <a href='mailto:%s'>webmaster</a> om deze vrij te geven.", $_SESSION['settings']['emailwebmaster']);
			}
			
		} elseif ((new cls_Mailing_vanaf())->min() > 0) {
			
			$mailing = new Mailing($_SESSION['settings']['mailing_herstellenwachtwoord']);
			
			if ($mailing->mid > 0) {
				if ($i_login->lidid > 0 and $mailing->send($i_login->lidid, 1, 1) > 0) {
					$mess = "E-mail met de link om het wachtwoord te herstellen is verzonden.";
				} else {
					$mess = "Fout bij het versturen van de e-mail. Probeer het later nogmaals of neem contact op met de webmaster.";
				}
			} else {
				$nk = (new cls_Login())->wachtwoordreset($lidid);
				$urlresetww = sprintf("%s/index.php?tp=Herstel+wachtwoord&lidid=%d&key=%s", BASISURL, $lidid, $nk);
				$email = new email();
				$email->onderwerp = $_SESSION['settings']['naamwebsite'] . " | Herstellen wachtwoord";
				$email->bericht = sprintf("<p>Link voor het opnieuw instellen van het wachtwoord: </p>\n", $urlresetww);
				$email->to_outbox(1);
			}
			$mailing = null;

		} else {
			$mess = "Er is geen vanaf e-mailadres beschikbaar, neem contact op met de webmaster.";
		}
		
	} elseif ($_SERVER['REQUEST_METHOD'] == "POST" and isset($_POST['nieuwwachtwoord']) and strlen($_POST['nieuwwachtwoord']) > 0 and $_POST['lidid'] > 0) {
		$mess = (new cls_login())->wijzigenwachtwoord($_POST['nieuwwachtwoord'], $_POST["key"], $_POST['herhaalwachtwoord'], $_POST['lidid']);
		
		echo("<p><a href='/?tp=Inloggen'>Klik hier om verder te gaan.</a></p>\n");
	
	} elseif (isset($_GET["lidid"]) and $_GET["lidid"] > 0 and strlen($_GET["key"]) > 5) {
		// Herstellen wachtwoord
		$i_login->vulvars($_GET['lidid']);
		
		if (strlen($i_login->login) > 5) {
			
			printf("<form id='herstellenwachtwoord' action='%s?tp=Herstel+wachtwoord' method='post'>\n", $_SERVER["PHP_SELF"]);
			printf("<input type='hidden' name='key' value='%s'><input type='hidden' name='lidid' value=%d>\n
			<h3>Herstel wachtwoord</h3>
			<label>Login</label><input type=text value='%s' name='login' readonly>
			<label>Nieuw wachtwoord</label><input type='password' name='nieuwwachtwoord'>
			<label>Herhaal wachtwoord</label><input type='password' name='herhaalwachtwoord'>
			%s
			<div id='opdrachtknoppen'>\n
			<button type='submit'>Bevestig</button>\n
			</div> <!-- Einde opdrachtknoppen -->\n
			</form>", $_GET["key"], $_GET['lidid'], $i_login->login, fneisenwachtwoord());
		} else {
			echo("<h3>Deze link is niet correct, vraag een nieuwe aan.</h3>\n");
		}
	
	} elseif ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (strlen($_POST['email']) > 0 and isValidMailAddress($_POST['email'], 0) == false) {
			$mess = "Dit is geen geldig e-mailadres.";
			
		} elseif (strlen($_POST['login']) < 5 and $_POST['lidnummer'] < 1 and strlen($_POST['email']) == 0) {
			$mess = "Vul minimaal 1 veld in.";
			
		} elseif (isset($_POST['login'])) {
			
			$lidid = $i_login->lididherstel($_POST['login'], $_POST['lidnummer'], $_POST['email']);
			if ($lidid > 0) {
				$mess = fnHerstellenWachtwoord("mail", $lidid);
			} else {
				$mess = $i_login->mess . " Er wordt geen e-mail, om het wachtwoord te herstellen, verstuurd.";
			}
		}
		printf("<p class='mededeling'>%s</p>", htmlentities($mess));
		echo("<p><a href='/'>Klik hier om verder te gaan.</a></p>\n");
	} else {
		printf("<form id='herstellenwachtwoord' action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER['QUERY_STRING']);
		echo("<h3>Herstellen wachtwoord</h3>
		<label class='form-label'>E-mailadres</label><input type='email' name='email'>
		<label class='form-label'>Lidnummer</label><input type='number' class='d8' min=0 name='lidnummer' value=0>\n");
		
		printf("<label>Login</label><input type='text' name='login' class='w%d'>\n", $_SESSION['settings']['login_maxlengte']);
		
		printf("<input type='hidden' value='mail' name='stap'>
		<p>Vul twee van de drie bovenstaande velden in.</p>
		<p>Je lidnummer kan je <a href='index.php?tp=Opvragen+lidnr'>hier</a> opvragen.</p>
		<p>Na het versturen van deze link is je oude wachtwoord niet meer geldig.</p>
		<div id='opdrachtknoppen'>\n
		<button class='%s' type='submit'>%s Stuur herstellink</button>\n
		</div> <!-- Einde opdrachtknoppen -->\n
		</form>", CLASSBUTTON, ICONSUBMIT);
	}
	if (strlen($mess) > 0) {
		(new cls_Logboek())->add($mess, 5, $lidid, 0, 0, 8);
	}
	return $mess;
	
} # fnHerstellenWachtwoord

function fnValidatieLogin($lidid, $key, $stap) {
	
	$mess = "";
	$i_login = new cls_Login();
	$i_m = new cls_Mailing();
	
	if ($stap == "mail") {
		$i_login->vulvars($lidid);
		if ($i_login->lidid > 0 and strlen($i_login->activatiekey) >= 5) {
			
			if ((new cls_Mailing_vanaf())->min() == 0) {
				$mess = "Er is geen adres beschikbaar om vanaf te e-mailen. Neem contact op met de webmaster.";
			
			} elseif ($_SESSION['settings']['mailing_validatielogin'] > 0 and $i_m->bestaat($_SESSION['settings']['mailing_validatielogin'])) {
				$mailing = new Mailing($_SESSION['settings']['mailing_validatielogin']);
				if ($mailing->send($i_login->lidid, 0, 1) == 0) {
					$mess = "Fout bij het versturen van de e-mail met de validatielink. Probeer het later nogmaals of neem contact op met de webmaster.";
				} else {
					$mess = "Een e-mail met validatielink is verzonden.";
				}
				$mailing = null;
				
			} else {
				$nak = $i_login->nieuweactivitiekey($lidid);
				$urlactivatie = sprintf("%s/index.php?tp=Validatie+login&lidid=%d&key=%s", BASISURL, $lidid, $nak);

				$email = new email();
				$email->toevoegenlid($i_login->lidid);
				$email->onderwerp = sprintf("%s | Activatie login", $_SESSION['settings']['naamwebsite']);
				$email->bericht = sprintf("<p>Naam lid: %s</p>
							<p>Activatie url %s: %s</p>\n
							<p>Deze link is %d uur geldig</p>", $email->aannaam, $_SESSION['settings']['naamwebsite'], $urlactivatie, $_SESSION['settings']['login_geldigheidactivatie']);
				if ($email->to_outbox(1) > 0) {
					$mess = "Een e-mail met validatielink is verzonden.";
				}
				$email = null;
			}
		} elseif ($i_login->lidid > 0) {
			$mess = sprintf("Login voor lidid %d heeft geen activicatie key, neem contact op met de webmaster.", $lidid);
		} else {
			$mess = sprintf("Login voor lidid %d bestaat niet, neem contact op met de webmaster.", $lidid);
		}
		
		if (strlen($mess) > 2) {
			(new cls_Logboek())->add($mess, 5, $lidid, 1);
		}

	} else {
		$i_login->vernieuwactivatiekeys();
		$i_login->valideerlogin($lidid, $key);
	}
	
	$i_login = null;
	$i_m = null;
	
	return $mess;
}  # fnValidatieLogin

function fnLoginAanvragen($stap="") {
	global $lididwebmasters;
	
	$i_login = new cls_Login();
	$i_lid = new cls_Lid();
	
	$login_verbodenkarakters = "@#$^&;*%éëèöôü'\"?!";
	
	if ($_SERVER['REQUEST_METHOD'] == "POST" and isset($_POST['loginaanvragen'])) {

		$mess = "";
		$lidid = 0;
		$_POST['emailadres'] = $_POST['emailadres'] ?? "";
		$_POST['gewenstelogin'] = $_POST['gewenstelogin'] ?? "";
		$_POST['gewenstwachtwoord'] = $_POST['gewenstwachtwoord'] ?? "";

		$wwok = checknewpassword($_POST['gewenstwachtwoord'], $_POST['gewenstelogin']);
		if (!isset($_POST['lidnummer']) or strlen($_POST['lidnummer']) == 0) {
			$_POST['lidnummer'] = 0;
		}
		
		$gevondenverbodenkarakter = "";
		for ($i=0; $i < strlen($login_verbodenkarakters); $i++) {
			if (strpos($_POST['gewenstelogin'], substr($login_verbodenkarakters, $i, 1)) > 0) {
				$gevondenverbodenkarakter = substr($login_verbodenkarakters, $i, 1);
			}
		}

		$i_lid->lidbijemail($_POST['emailadres'], sprintf("LM.Lidnr=%d", $_POST['lidnummer']));
		if (!isValidMailAddress($_POST['emailadres'], 0)) {
			$mess = "Je hebt geen geldig e-mailadres opgegeven.";
		} elseif ($i_lid->lidid <= 0) {
			$mess = "Deze combinatie van e-mailadres en lidnummer bestaat niet of is geen lid.";
		} elseif ($i_lid->inloggentoegestaan == false) {
			$mess = "Je behoort niet tot een groep, die mag inloggen. Een login mag niet worden aangevraagd.";
		} elseif ($i_login->aantal(sprintf("Login.LidID=%d", $i_lid->lidid)) > 0) {
			$i_login->vulvars($i_lid->lidid);
			if (strlen($i_login->activatiekey) > 5) {
				fnValidatieLogin($i_login->lidid, $i_login->activatiekey, "mail");
			} else {
				$mess = "Je hebt al een gevalideerde login.";
			}
		} elseif (strlen($_POST['gewenstelogin']) < 6) {
			$mess = "Je hebt geen geldige gewenste login opgegeven. Een login moet uit minimaal 6 karakters bestaan.";
		} elseif (strlen($_POST['gewenstelogin']) > $_SESSION['settings']['login_maxlengte']) {
			$mess = sprintf("Je hebt geen geldige login opgegeven. Een login mag uit maximaal %d karakters bestaan.", $_SESSION['settings']['login_maxlengte']);
		} elseif (strpos($_POST['gewenstelogin'], " ") > 0) {
			$mess = "Je hebt geen geldige login opgegeven. Er mag geen spatie in een login zitten.";
		} elseif (strlen($gevondenverbodenkarakter) > 0) {
			$mess = sprintf("e hebt geen geldige login opgegeven. Er mag geen '%s' in een login zitten.", $gevondenverbodenkarakter);
		} elseif ($i_login->aantal(sprintf("Login='%s'", $_POST['gewenstelogin'])) > 0) {
			$i_login->lididbijlogin($_POST['gewenstelogin']);
			if (strlen($i_login->activatiekey) > 5 and $i_lid->lidid == $i_login->lidid) {
				fnValidatieLogin($i_login->lidid, $i_login->activatiekey, "mail");
			} else {
				$mess = sprintf("Login '%s' is al in gebruik.", $_POST['gewenstelogin']);
			}
		} elseif ($wwok !== "ok") {
			$mess = $wwok;
		} else {
			$i_login->add($i_lid->lidid, $_POST['gewenstelogin'], $_POST['gewenstwachtwoord']);
		}
		if (strlen($mess) > 0) {
			(new cls_Logboek())->add($mess, 5, $i_lid->lidid, 1);
		}
		printf("<p><a href='%s' target='_top'>Klik hier om verder te gaan.</a></p>\n", BASISURL);
		
	} else {

		printf("<form action='%s?tp=Bevestiging+login' class='form' id='loginaanvraag' method='post'>\n", $_SERVER["PHP_SELF"]);
		echo("<h2>Login aanvraag</h2>\n");
		
		echo("<label>E-mailadres</label><input type='email' name='emailadres' value='' autocomplete='off'>\n");
		echo("<label>Lidnummer</label><input type='number' class='d8' name='lidnummer' value=0>\n");
		printf("<label>Gewenste login</label><input type='text' name='gewenstelogin' maxlength=%1\$d class='w%1\$d'>\n", $_SESSION['settings']['login_maxlengte']);
		printf("<label>Gewenst wachtwoord</label><input type='password' name='gewenstwachtwoord' value='' maxlength=%1\$d class='w%1\$d'>\n", $_SESSION['settings']['wachtwoord_maxlengte']);

		echo("<p>Je lidnummer kan je <a href='index.php?tp=Opvragen+lidnr'>hier</a> opvragen.</p>\n");
		echo("<p>Je ontvangt een e-mail met daarin een link om deze aanmelding te bevestigen. Na het bevestigen is de login te gebruiken al login.</p>\n");
		
		echo("<p>Voorwaarden aan een login:</p>\n<ul>\n");
		printf("<li>Minimaal 7 en maximaal %d karakters</li>\n", $_SESSION['settings']['login_maxlengte']);
		echo("<li>Er mogen geen aanhalingstekens en spaties in zitten</li>\n");
		printf("<li>De karakters '%s' mogen er niet in zitten</li>\n", htmlent($login_verbodenkarakters));
		echo("</ul>\n");
		
		printf(fneisenwachtwoord());
		
		echo("<div id='opdrachtknoppen'>\n");
		printf("<button type='submit' class='%s' name='loginaanvragen'>%s Aanvragen</button>\n", CLASSBUTTON, ICONSUBMIT);
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		
		echo("</form>\n");
	}
} # fnLoginAanvragen

function fnOpvragenLidnr($stap) {
	
	$mess = "";
	$mailing = new mailing($_SESSION['settings']['mailing_lidnr']);
	$i_lid = new cls_Lid();
			
	if ($stap == "mail") {
		if (!isValidMailAddress($_POST['emailvoorlidnr'], 0)) {
			$mess = "Je hebt geen geldig e-mailadres opgegeven.";
			
		} elseif ((new cls_Mailing_vanaf())->min() == 0) {
			$mess = "Er is geen adres bekend om vanaf te e-mailen. Neem contact op met de webmaster.";
			
		} else {
			$rows = $i_lid->lidbijemail($_POST['emailvoorlidnr']);
			if (count($rows) > 0) {
				foreach ($rows as $row) {
					$i_lid->vulvars($row->LidID);
					if ($mailing->mid > 0 ) {
						$mail = new email(0, $mailing->mid);
						$mail->toevoegenlid($row->LidID);
						$mail->bericht = $mailing->merge($row->LidID);
					} else {
						$mail = new email();
						$mail->toevoegenlid($row->LidID);
						$mail->onderwerp = "Lidnummer " . $_SESSION['settings']['naamvereniging_afkorting'];
						$mail->bericht = sprintf("<p>Je hebt je lidnummer bij de %s opgevraagd. Je lidnummer is %d.</p>
												  <p>Met dit lidnummer en e-mailadres '%s' kun je een login aanvragen.</p>\n", $_SESSION['settings']['naamvereniging_afkorting'], $row->Lidnr, $_POST['emailvoorlidnr']);
					}
					$mail->xtrachar = "LIDNR";
					$mail->xtranum = $row->Lidnr;
					
					if ($mail->to_outbox(1) > 0) {
						$mess = sprintf("Een e-mail met lidnummer %d is in de outbox geplaatst en wordt zo spoedig mogelijk verzonden.", $row->Lidnr);
					} else {
						$mess = sprintf("Fout bij het versturen van het lidnummer aan %s. Probeer het later nogmaals of neem contact met de webmaster op. ", $i_lid->naam);
					}
					(new cls_Logboek())->add($mess, 5, $row->LidID, 1, $row->LidID, 11);
					$mess = "";
					$mail = null;
				}
				
			} else {
				$mess = "Er is geen lid met dit e-mailadres in onze database bekend.";
			}
		} 
		if (strlen($mess) > 0) {
			(new cls_Logboek())->add($mess, 5, 0, 1);
		}
		printf("<p><a href='%s' target='_top'>Klik hier om verder te gaan.</a></p>\n", BASISURL);
		
	} else {
		
		echo("<div id='opvragenlidnr'>\n");
		printf("<form action='%s?tp=Opvragen+lidnr' method='post'>\n", $_SERVER["PHP_SELF"]);
		
		echo("<h3>Opvragen Lidnummer</h3>\n");
		echo("<label class='form-label'>E-mailadres</label><input type='email' name='emailvoorlidnr'>\n");
		echo("<div id='opdrachtknoppen'>\n");
		printf("<button type='submit' class='%s' name='opvragenlidnr' value='Opvragen'>Opvragen</button>\n", CLASSBUTTON);
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		
		echo("</form>\n");
		echo("</div>\n");
	}
	
} # fnOpvragenLidnr

function cleanlogin($login) {

	$login = trim($login);
	if (strlen($login) > 0 and isValidMailAddress($login) == false) {
		
		$ll = (new cls_Login())->lengtekolom("Login");
		
		if (strlen($login) > $ll) {
			$login = substr($login, 0, $ll);
		}
		$login = str_replace(" ", "_", $login);
		$login = str_replace("'", "", $login);
		$login = str_replace("\"", "", $login);
		$login = str_replace(";", "", $login);
	}
	
	return $login;
	
} # cleanlogin

function newkey() {
	
	$alphabet = "ABCDEFGHIJKLMNOPQRSTXYZabcdefghijklmnopqrstuwxyz()@$!0123456789";
	$ww = "";
	for ($i = 0; $i < 15; $i++) {
		$n = rand(0, strlen($alphabet)-1);
		$ww .= substr($alphabet, $n, 1);
	}
	$ww = strtoupper(substr($ww, 0, 2)) . substr($ww, 2, 8) . strtoupper(substr($ww, 10, 2));
	
	return $ww;
} # newkey

function checknewpassword($ww, $login="", $p_wwh="ghc") {
	
	$alphabet = "abcdefghijklmnopqrstuvwxyz";
	$mess = "ok";
	
	if (strlen($login) < 5) {
		$login = $_SESSION['username'];
	}
	
	if (strlen($ww) > $_SESSION['settings']['wachtwoord_maxlengte'] and $_SESSION['settings']['wachtwoord_maxlengte'] >= 7) {
		$mess = sprintf("Het wachtwoord is te lang, het mag maximaal %d karakters lang zijn.", $_SESSION['settings']['wachtwoord_maxlengte']);
	} elseif (strlen($ww) < $_SESSION['settings']['wachtwoord_minlengte']) {
		$mess = sprintf("Het wachtwoord is te kort, het moet minimaal %d karakters lang zijn.", $_SESSION['settings']['wachtwoord_minlengte']);
	} elseif (strpos($ww, "'") > 0 or strpos($ww, "\"") > 0) {
		$mess = "Er mogen geen aanhalingstekens in een wachtwoord zitten.";
	} elseif (strpos($ww, " ") !== false) {
		$mess = "Het wachtwoord mag geen spatie bevatten.";
	} elseif (strlen($login) > 0 and strpos($ww, $login) !== false) {
		$mess = sprintf("Je login (%s) mag geen onderdeel van je wachtwoord zijn.", $login);
	} elseif (strlen($_SESSION['roepnaamingelogde']) > 0 and strpos(strtolower($ww), strtolower($_SESSION['roepnaamingelogde'])) !== false) {
		$mess = "Het wachtwoord mag niet je roepnaam bevatten.";
	} elseif ($p_wwh !== "ghc" and (strlen($p_wwh) == 0 or $ww !== $p_wwh)) {
		$mess = "De nieuwe wachtwoorden zijn niet aan elkaar gelijk.";
	} else {
		$bevatc=0;
		$bevatk=0;
		$bevath=0;
		for ($i=0; $i < strlen($ww); $i++) {
			if (strpos("0123456789", substr($ww, $i, 1)) !== false) {
				$bevatc=1;
			} elseif (strpos($alphabet, substr($ww, $i, 1)) !== false) {
				$bevatk=1;
			} elseif (strpos(strtoupper($alphabet), substr($ww, $i, 1)) !== false) {
				$bevath=1;
			}
		}
		if ($bevatc == 0) {
			$mess = "Het wachtwoord moet minimaal 1 cijfer bevatten.";
		} elseif ($bevatk == 0) {
			$mess = "Het wachtwoord moet minimaal 1 kleine letter bevatten.";
		} elseif ($bevath == 0) {
			$mess = "Het wachtwoord moet minimaal 1 hoofdletter bevatten.";
		}
	}
	
	return $mess;
} # checknewpassword

function fneisenwachtwoord() {
	return sprintf("<p>Voorwaarden aan een wachtwoord:</p>
					<ul>
					<li>Minimaal %d karakters en maximaal %d karakters lang</li>
					<li>Er mogen geen aanhalingstekens en geen spaties in zitten</li>
					<li>Mag niet je login of roepnaam bevatten</li>
					<li>Moet minimaal 1 cijfer bevatten</li>
					<li>Moet minimaal 1 kleine letter bevatten</li>
					<li>Moet minimaal 1 hoofdletter bevatten</li>
					</ul>\n", $_SESSION['settings']['wachtwoord_minlengte'], $_SESSION['settings']['wachtwoord_maxlengte']);
} # fneisenwachtwoord

function addtp($tp, $afdnr=0) {
	global $tabpages, $currenttab;
	
	if ((strstr($tp, "/") === false or startwith($tp, $currenttab . "/")) and toegang($tp, 0, 0, 1)) {
		$tabpages[] = $tp;
	}
	
} # addtp

?>
