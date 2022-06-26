<?php

$_SESSION['webmaster'] = 0;
if ($_SESSION['lidid'] == 0 or strlen($_SESSION['username']) == 0) {
	$_SESSION['lidid'] = 0;
	$_SESSION['username'] = "";
	$_SESSION['lidgroepen'] = "0";
} else {
	if (in_array($_SESSION['lidid'], $lididwebmasters)) {
		$_SESSION['webmaster'] = 1;
	}
	$_SESSION['lidgroepen'] = (new cls_Lidond())->lidgroepen();
}

$tabpages = null;
if ($_SERVER['PHP_SELF'] == "/admin.php") {
	addtp("Beheer logins");
	addtp("Autorisatie");
	addtp("Instellingen");
	addtp("Stamgegevens");
	addtp("Uploaden data");
	$f = "IFNULL(Afgemeld, '2000-01-01') < '2010-01-01'";
	if ((new cls_Interface())->aantal($f) > 0) {
		addtp("Downloaden wijzigingen");
	}
	addtp("Onderhoud");
	addtp("Logboek");
} else {
	$tabpages[] = "Vereniging";
	if ($_SESSION['lidid'] > 0) {
		addtp("Eigen gegevens");
	} elseif ((new cls_Login())->aantal() > 0) {
		addtp("Inloggen");
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
		addtp("Zelfservice/Algemene gegevens");
		$f = "Zelfservice=1";
		if ($gs != "B" and (new cls_diploma())->aantal($f) > 0) {
			addtp("Zelfservice/Diploma's");
		}
		addtp("Zelfservice/Pasfoto");
		$_SESSION['settings']['zs_muteerbarememos'] = $_SESSION['settings']['zs_muteerbarememos'] ?? "";
		if (strlen($_SESSION['settings']['zs_muteerbarememos']) > 0) {
			addtp("Zelfservice/Bijzonderheden");
		}
		if ($gs != "B" and (new cls_Bewaking_blok())->aantal("InschrijvingOpen=1 AND Eind >= CURDATE()") > 0) {
			addtp("Zelfservice/Inschrijven bewaking");
		}
		$f = sprintf("Datum >= CURDATE() AND IFNULL(VerwijderdOp, '1900-01-01') < '2012-01-01' AND InschrijvingOpen=1 AND BeperkTotGroep IN (%s)", $_SESSION["lidgroepen"]);
		if ((new cls_Evenement())->aantal($f) > 0) {
			addtp("Zelfservice/Evenementen");
		}
		
		$_SESSION['aantaltoestemmingen'] = $_SESSION['aantaltoestemmingen'] ?? (new cls_Onderdeel())->aantal("`Type`='T' AND IFNULL(VervallenPer, '9999-12-31') >= CURDATE()");
		if ($_SESSION['aantaltoestemmingen'] > 0 and $gs != "B") {
			addtp("Zelfservice/Toestemmingen");
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
		addtp("Ledenlijst/Voormalig leden");
		if ((new cls_Lidmaatschap())->aantal("LIDDATUM > CURDATE()")) {
			addtp("Ledenlijst/Toekomstige leden");
		}
		
		$_SESSION['aantalklosleden'] = $_SESSION['aantalklosleden'] ?? (new cls_Lid())->aantalklosleden();
		if ($_SESSION['aantalklosleden'] > 0) {
			addtp("Ledenlijst/Klosleden");
		}
		addtp("Ledenlijst/Nieuw (klos)lid");
		if ((new cls_Onderdeel())->aantal("`Type`='A' AND IFNULL(VervallenPer, '9999-12-31') >= CURDATE()") > 0) {
			addtp("Ledenlijst/Afdelingen");
		}
		addtp("Ledenlijst/Commissies");
		addtp("Ledenlijst/Groepen");
		if ((new cls_Onderdeel())->aantal("`Type`='R' AND IFNULL(VervallenPer, '9999-12-31') >= CURDATE()") > 0) {
			addtp("Ledenlijst/Rollen");
		}

		addtp("Ledenlijst/Eigen lijsten");
		if ($currenttab2 == "Eigen lijsten") {
			addtp("Ledenlijst/Eigen lijsten/Muteren");
			foreach((new cls_Eigen_lijst())->lijst() as $row) {
				if ($row->AantalRecords > 0 and $row->AantalKolommen > 1) {
					addtp("Ledenlijst/Eigen lijsten/" . $row->Naam);
				}
			}
		}
		
		$b = "Ledenlijst/Basisgegevens";
		addtp($b);
		if ($currenttab2 == "Basisgegevens") {
			addtp($b . "/Onderdelen");
			addtp($b . "/Functies");
			addtp($b . "/Diploma's");
			addtp($b . "/Seizoenen");
			addtp($b . "/Organisaties");
		}
	
		addtp("Ledenlijst/Instellingen");
		addtp("Ledenlijst/Mutaties");
	}
	
	addtp("Mailing");
	if ($currenttab == "Mailing") {
		$i_M = new cls_Mailing();
		addtp("Mailing/Muteren");
		addtp("Mailing/Nieuw");
		$_SESSION['aantalrekeningen'] = $_SESSION['aantalrekeningen'] ?? (new cls_Rekening())->aantal();
		if ($_SESSION['aantalrekeningen'] > 0) {
			addtp("Mailing/Rekeningen");
		}
		
		$f = "(deleted_on IS NOT NULL)";
		if ($i_M->aantal($f) > 0) {
			addtp("Mailing/Prullenbak");
		}
		addtp("Mailing/Outbox");
		addtp("Mailing/Verzonden mails");
		addtp("Mailing/Instellingen");
		addtp("Mailing/Logboek");
		$i_M = null;
		if ($currenttab2 == "Wijzigen mailing") {
			addtp($currenttab . "/" . $currenttab2);
		}
	}
	
	if (isset($_SESSION['settings']['menu_met_afdelingen']) and strlen($_SESSION['settings']['menu_met_afdelingen']) > 0) {
		// Tabblad per afdeling
		
		/* Voor later
		$arrafdinmenu = array();
		
		foreach ((new cls_Onderdeel())->lijst(1, "`Type`='A'") as $row) {
			if (in_array($row->RecordID, explode(",", $_SESSION['settings']['menu_met_afdelingen']))) {
				if (toegang($row->Naam, 0, 0)) {
					$arrafdinmenu[] = $row->Naam;
				}
			}
		}
		*/
		
		
		foreach ((new cls_Onderdeel())->lijst(1, "`Type`='A'") as $row) {
			if (in_array($row->RecordID, explode(",", $_SESSION['settings']['menu_met_afdelingen']))) {
//				debug($row->Naam);
				addtp($row->Naam, $row->RecordID);
				if ($currenttab == $row->Naam) {
					addtp($row->Naam . "/Afdelingslijst", $row->RecordID);
					addtp(trim($row->Naam) . "/Kalender");
					$f = sprintf("GroepID > 0 AND IFNULL(Opgezegd, CURDATE()) >= CURDATE() AND OnderdeelID=%d", $row->RecordID);
					if ((new cls_Lidond())->aantal($f) > 0) {
						addtp(trim($row->Naam) . "/Groepsindeling", $row->RecordID);
					}
					addtp(trim($row->Naam) . "/Groepsindeling muteren");
					$f = sprintf("OnderdeelID=%d AND Datum > DATE_SUB(CURDATE(), INTERVAL 9 MONTH) AND Activiteit=1", $row->RecordID);
					if ((new cls_Afdelingskalender())->aantal($f) > 0) {
						addtp(trim($row->Naam) . "/Presentie muteren");
						if ((new cls_Aanwezigheid())->aantalstatus("*", $row->RecordID) > 0) {
							addtp(trim($row->Naam) . "/Presentie per lid");
						}
					}
					
					if (toegang("Mailing/Muteren")) {
						$tp = trim($row->Naam) . "/Afdelingsmailing";
						addtp($tp);
					}
				}
			}
		}
	}

	if (!isset($_GET['lidid']) or strlen($_GET['lidid']) == 0) {
		$_GET['lidid'] = 0;
	}
	$gs = (new cls_Lid())->Geslacht($_GET['lidid']);

	if ($currenttab2 == "Wijzigen lid") {
		$b = $currenttab . "/" . $currenttab2;
		addtp($b);
		$b .= "/";
		addtp($b . "Algemene gegevens");
		if ((new cls_Onderdeel())->aantal("Type='A'") > 0 and (new cls_Lidmaatschap())->soortlid($_GET['lidid']) != "Kloslid") {
			addtp($b . "Afdelingen");
		}
		if ($gs != "B") {
			addtp($b . "B, C en F");
		}
		if ((new cls_Onderdeel())->aantal("Type='G'") > 0) {	
			addtp($b . "Groepen");
		}
		if ((new cls_Onderdeel())->aantal("Type='O'") > 0 and $gs != "B") {	
			addtp($b . "Onderscheidingen");
		}
		if ($gs != "B" and (new cls_Diploma())->aantal() > 0) {
			addtp($b . "Diploma's");
		}
		
		$_SESSION['aantaltoestemmingen'] = $_SESSION['aantaltoestemmingen'] ?? (new cls_Onderdeel())->aantal("`Type`='T' AND IFNULL(VervallenPer, '9999-12-31') >= CURDATE()");
		if ($gs != "B" and $_SESSION['aantaltoestemmingen'] > 0) {
			addtp($b . "Toestemmingen");
		}
		if (strlen($_SESSION['settings']['muteerbarememos']) > 0) {
			addtp($b . "Bijzonderheden");
		}
		if ($gs != "B") {
			addtp($b . "Lidmaatschap");
			addtp($b . "Pasfoto");
		}
	}
	
	if ((new cls_Mutatie())->aantal() > 0) {
		addtp("Kostenoverzicht");
	}
	
	addtp("Stukken");


	if ((new cls_Bewakingsseizoen())->aantal() > 0) {
		addtp("Bewaking");
		if ($currenttab == "Bewaking") {
			addtp("Bewaking/Bewakingsrooster");
			addtp("Bewaking/Postindeling");
			addtp("Bewaking/Aantallen");
			addtp("Bewaking/Nieuwe pasfoto");
			$f = "Zelfservice=1";
			if ((new cls_diploma())->aantal($f) > 0) {
				addtp("Bewaking/Diploma's muteren");
			}
			if ((new cls_Bewaking_blok())->aantal() > 0) {
				addtp("Bewaking/Overzicht inschrijvingen");
			}
			addtp("Bewaking/Blokken muteren");
			if (count((new cls_InsBew())->export()) > 0) {
				addtp("Bewaking/Downloaden inschrijvingen");
			}
			addtp("Bewaking/Logboek");
		}
	}
	
	addtp("Evenementen");
	if ($currenttab == "Evenementen") {
		$i_ev = new cls_Evenement();
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
		addtp("Evenementen/Types muteren");
		addtp("Evenementen/Logboek");
		$i_ev = null;
	}	
	
	addtp("Rekeningen");
	if ($currenttab == "Rekeningen") {
		$b = "Rekeningen/";
		addtp($b . "Beheer");
		addtp($b . "Nieuw");
		addtp($b . "Logboek");
	}
	
	addtp("Bestellingen");
	if ($currenttab == "Bestellingen") {
		if ((new cls_Artikel())->aantal() > 0) {
			addtp("Bestellingen/Bestellingen muteren");
		}
		addtp("Bestellingen/Artikelbeheer");
		if ((new cls_Artikel())->aantal() > 0) {
			addtp("Bestellingen/Voorraadbeheer");
		}
		if ((new cls_Logboek())->aantal("TypeActiviteit=10") > 0) {
			addtp("Bestellingen/Logboek");
		}
	}
	
	if ($_SERVER["HTTP_HOST"] == "phprbm.telling.nl") {
//		addtp("DMS");   Voorlopig geen tijd voor, dus uitgezet.
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
			$_SESSION['lidid'] = $row->LidID;
			$_SESSION['lidnr'] = $row->Lidnr;
			$_SESSION['naamingelogde'] = $row->NaamLid;
			if (strlen($row->Roepnaam) == 0) {
				$_SESSION['roepnaamingelogde'] = $_SESSION['naamingelogde'];
			} else {
				$_SESSION['roepnaamingelogde'] = trim($row->Roepnaam);
			}
			if (isValidMailAddress($row->EmailVereniging, 0)) {
				$_SESSION['emailingelogde'] = strtolower($row->EmailVereniging);
			} elseif (isValidMailAddress($row->Email, 0)) {
				$_SESSION['emailingelogde'] = strtolower($row->Email);
			} elseif (isValidMailAddress($row->EmailOuders, 0)) {
				$_SESSION['emailingelogde'] = strtolower($row->EmailOuders);
			} elseif (strlen($row->EmailOuders) > 5 and strpos($row->EmailOuders, ",") > 0) {
				$_SESSION['emailingelogde'] = strstr(strtolower($row->EmailOuders), ",", true);
			} else {
				$_SESSION['emailingelogde'] = "";
			}
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
	}
	
} # fnAuthenticatie
	
function toegang($soort, $melding=1, $p_log=1) {
	global $lididwebmasters, $tabpages, $currenttab, $currenttab2, $currenttab3;
	
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
	
	$_SESSION['webmaster'] = 0;
	if ($_SESSION['lidid'] > 0 and in_array($_SESSION['lidid'], $lididwebmasters)) {
		$_SESSION['webmaster'] = 1;
	}
	
	$soort = str_replace("'", "", $soort);
	$aid = $i_aut->recordid($soort);
//	debug($aid . " aid in toegang");
		
	if (strlen($soort) == 0 or in_array($soort, $arrAltijdToegang) === true or $_SESSION['webmaster'] == 1) {
		if (strlen($soort) > 0 and in_array($soort, $arrAltijdToegang) === false and in_array($soort, $tabpages) == false and $aid == 0) {
			$aid = $i_aut->add($soort);
		}
		$rv = true;
		
	} elseif ($_SESSION['lidid'] > 0 and isset($_SESSION['toegang'][$soort]) and $_SESSION['toegang'][$soort] == 1 and 1 == 2) {
		$rv = true;
		
	} else {
		$_SESSION['webmaster'] == 0;
		if ($i_aut->toegang($soort) == true) {
			$_SESSION['toegang'][$soort] = 1;
			$rv = true;
		} else {
			$mess = sprintf("Je hebt geen toegang tot '%s'.", $soort);
			if ($p_log == 1) {
				(new cls_Logboek())->add($mess, 15, 0, $melding);
				
			} elseif ($melding > 0) {
				printf("<p class='mededeling'>%s</p>\n", $mess);
			}
			$rv = false;
		}
	}
	
	
	if ($rv and $aid > 0) {
		$i_aut->update($aid, "LaatstGebruikt", date("Y-m-d"));
	}
	$i_aut = null;
	
	return $rv;
	
}  # toegang

function fnInloggen() {
	echo("<div id='login'>\n");
	echo("<h2>Inloggen</h2>\n");
	echo("<form action='/index.php' method='post'>\n");
	echo("<label>Login</label><input type='text' name='username'>\n");
	echo("<label>Wachtwoord</label><input type='password' name='password'>\n");
	echo("<label>Ingelogd blijven?</label><input type='checkbox' name='cookie' value=1 title='Ingelogd blijven'> <p>(werkt met cookies)</p>\n");
	echo("<p>Heb je nog geen login, dan kan je die <a href='\?tp=Login+aanvragen'>aanvragen</a>.</p>");
	echo("<p>Ben je je wachtwoord vergeten, dan kan je deze <a href='\?tp=Herstel+wachtwoord'>opnieuw instellen</a>.</p>\n");

	echo("<div id='opdrachtknoppen'>\n");
	echo("<input type='submit' value='Inloggen' name='Inloggen'>\n");
	echo("</div> <!-- Einde opdrachtknoppen -->\n");

	echo("</form>\n");

	echo("</div>  <!-- Einde login -->\n");

}  # fnInloggen

function fnHerstellenWachtwoord($stap="", $lidid=0) {
	
	$mess = "";
	$i_login = new cls_Login();
	
	if (strlen($stap) == 0 and isset($_POST['stap']) and strlen($_POST['stap']) > 0) {
		$stap = $_POST['stap'];
	}

	if ($stap == "mail" and $lidid > 0) {

		if ($_SESSION['settings']['mailing_herstellenwachtwoord'] == 0) {
			$i_M = new cls_Mailing();
			$mid = $inst_M->add("Herstellen wachtwoord");
			$i_M->update($mid, "from_addr", $_SESSION['settings']['emailwebmaster']);
			$i_M->update($mid, "message", "<p>Herstellen wachtwoord [%NAAMWEBSITE%]: https://[%URLRESETWW%]</p>\n");
			$i_M->update($mid, "template", 1);
			(new cls_Parameter())->update("mailing_herstellenwachtwoord", $mid);
			$i_M = null;
		}
		$row = $i_login->record($lidid);
		if ($_SESSION['settings']['login_maxinlogpogingen'] > 0 and $row->FouteLogin > $_SESSION['settings']['login_maxinlogpogingen']) {
			$mess = "Er is teveel foutief met deze login proberen in te loggen. Hierdoor is dit account geblokkeerd. ";
			if ($_SESSION['settings']['login_autounlock'] > 0 and $_SESSION['settings']['login_autounlock'] <= 24*60) {
				$mess .= sprintf("Uiterlijk om %s uur is deze blokkade verwijderd.", date("G:i", strtotime($row->Gewijzigd)+(60*$_SESSION['settings']['login_autounlock'])));
			} else {
				$mess .= sprintf("Vraag aan de <a href='mailto:%s'>webmaster</a> om deze vrij te geven.", $_SESSION['settings']['emailwebmaster']);
			}
			
		} elseif ($_SESSION['settings']['mailing_herstellenwachtwoord'] > 0) {
			
			$mailing = new Mailing($_SESSION['settings']['mailing_herstellenwachtwoord']);
			if ($mailing->send($row->LidID) > 0) {
				$mess = sprintf("De e-mail naar %s met de link om het wachtwoord te herstellen is verzonden.", $row->Naam);
			} else {
				$mess = "Fout bij het versturen van de e-mail. Probeer het later nogmaals of neem contact op met de webmaster.";
			}
			$mailing = null;

		} else {
			$mess = "Er is geen mailing voor het herstellen van het wachtwoord beschikbaar.";
		}
		
	} elseif ($_SERVER['REQUEST_METHOD'] == "POST" and isset($_POST['nieuwwachtwoord']) and strlen($_POST['nieuwwachtwoord']) > 0 and $_POST['lidid'] > 0) {
		$mess = (new cls_login())->wijzigenwachtwoord($_POST['nieuwwachtwoord'], $_POST["key"], $_POST['herhaalwachtwoord'], $_POST['lidid']);
		
		echo("<p><a href='/?tp=Inloggen'>Klik hier om verder te gaan.</a></p>\n");
	
	} elseif (isset($_GET["lidid"]) and $_GET["lidid"] > 0 and strlen($_GET["key"]) > 5) {
		$row = (new cls_Login())->record($_GET['lidid']);
		
		$login = $row->Login;
		
		echo("<div id='herstellenwachtwoord'>\n");
		if (strlen($login) > 5) {
			
			printf("<form name='HerstellenWachtwoord' action='%s?tp=Herstel+wachtwoord' method='post'>\n", $_SERVER["PHP_SELF"]);
			printf("<input type='hidden' name='key' value='%s'><input type='hidden' name='lidid' value=%d>\n
			<h3>Herstel wachtwoord</h3>
			<label>Login</label><input type=text value='%s' name='login' readonly>
			<label>Nieuw wachtwoord</label><input type='password' name='nieuwwachtwoord'>
			<label>Herhaal wachtwoord</label><input type='password' name='herhaalwachtwoord'>
			%s
			<div id='opdrachtknoppen'>\n
			<input type='submit' value='Bevestig'>\n
			</div> <!-- Einde opdrachtknoppen -->\n
			</form>", $_GET["key"], $_GET['lidid'], $login, fneisenwachtwoord());
		} else {
			echo("<h3>Deze link is niet correct, vraag een nieuwe aan.</h3>\n");
		}
		echo("</div>  <!-- Einde herstellenwachtwoord -->");
	
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
				$mess = "Er is geen uniek lid gevonden. Er wordt geen e-mail verstuurd om het wachtwoord te herstellen.";
			}
		}
		printf("<p class='mededeling'>%s</p>", htmlentities($mess));
		echo("<p><a href='/'>Klik hier om verder te gaan.</a></p>\n");
	} else {
		echo("<div id='herstellenwachtwoord'>\n");
		printf("<form name='HerstellenWachtwoord' action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER['QUERY_STRING']);
		echo("<fieldset>
		<h3>Herstellen wachtwoord</h3>
		<label>E-mailadres</label><input type='email' name='email'>
		<label>Lidnummer</label><input type='number' min=0 name='lidnummer' value=0>
		<label>Login</label><input type='text' name='login'>
		<input type='hidden' value='mail' name='stap'>
		</fieldset>
		<p>Na het versturen van deze link is je oude wachtwoord niet meer geldig.</p>
		<div id='opdrachtknoppen'>\n
		<input type='submit' value='Stuur herstellink'>\n
		</div> <!-- Einde opdrachtknoppen -->\n
		</form>
		</div>  <!-- Einde herstellenwachtwoord -->");
	}
	if (strlen($mess) > 0) {
		(new cls_Logboek())->add($mess, 5, $lidid, 0, 0, 8);
	}
	return $mess;
	
} # fnHerstellenWachtwoord

function fnValidatieLogin($lidid, $key, $stap) {
	
	$mess = "";
	$i_login = new cls_Login();
	if ($stap == "mail") {
	
		$row = $i_login->record($lidid);
		
		if ($_SESSION['settings']['mailing_validatielogin'] == 0) {
			$inst_M = new cls_Mailing();
			$content = "<p>Naam lid: [%Lidnaam%]</p>
						<p>Activatie url [%NAAMWEBSITE%]: https://[%URLACTIVATIE%]</p>\n";
			$mid = $inst_M->add("Validatielink nieuwe login");
			$inst_M->update($mid, "from_addr", $_SESSION['settings']['emailwebmaster']);
			$inst_M->update($mid, "template", 1);
			$inst_M->update($mid, "message", $content);
			(new cls_Parameter())->update("mailing_validatielogin", $mid);
			$inst_M = null;
		}
	
		if ($row->LidID > 0 and strlen($row->Wachtwoord) >= 5) {
			$mailing = new Mailing($_SESSION['settings']['mailing_validatielogin']);
			if ($mailing->send($row->LidID) == 0) {
				$mess = "Fout bij het versturen van de e-mail met de validatielink. Probeer het later nogmaals of neem contact op met de webmaster.";
			}
			$mailing = null;
		} else {
			$mess = sprintf("Login voor lidid %d bestaat niet, neem onctact op met de webmaster.", $lidid);
		}
		
		if (strlen($mess) > 2) {
			(new cls_Logboek())->add($mess, 5, $lidid, 1);
		}

	} else {
		$i_login->vernieuwactivatiekeys();
		$i_login->valideerlogin($lidid, $key);
	}
	
	$i_login = null;
	
	return $mess;
}  # fnValidatieLogin

function fnLoginAanvragen($stap="") {
	global $lididwebmasters, $lididtestusers;
	
	$i_login = new cls_Login();
	$login_verbodenkarakters = "@#$^&;*%éëèöôü'\"";
	
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
		
		$i_login = new cls_Login();
		$lidid = $i_login->lidid($_POST['emailadres'], $_POST['lidnummer']);
		if (!isValidMailAddress($_POST['emailadres'], 0)) {
			$mess = "Je hebt geen geldig e-mailadres opgegeven.";
		} elseif (strlen($_POST['gewenstelogin']) < 6) {
			$mess = "Je hebt geen geldige gewenste login opgegeven. Een login moet uit minimaal 6 karakters bestaan.";
		} elseif ($i_login->aantal(sprintf("Login='%s'", $_POST['gewenstelogin'])) > 0) {
			$ak = $i_login->record($lidid)->ActivatieKey;
			if (strlen($ak) > 5 and $lidid > 0) {
				fnValidatieLogin($lidid, $ak, "mail");
			} else {
				$mess = sprintf("Login '%s' is al in gebruik.", $_POST['gewenstelogin']);
			}
		} elseif (strlen($_POST['gewenstelogin']) > $_SESSION['settings']['login_maxlengte']) {
			$mess = sprintf("Je hebt geen geldige login opgegeven. Een login mag uit maximaal %d karakters bestaan.", $_SESSION['settings']['login_maxlengte']);
		} elseif (strpos($_POST['gewenstelogin'], " ") > 0) {
			$mess = "Je hebt geen geldige login opgegeven. Er mag geen spatie in een login zitten.";
		} elseif (strlen($gevondenverbodenkarakter) > 0) {
			$mess = sprintf("e hebt geen geldige login opgegeven. Er mag geen '%s' in een login zitten.", $gevondenverbodenkarakter);
		} elseif ($wwok !== "ok") {
			$mess = $wwok;
		} else {
			if ($lidid > 0) {
				if ($i_login->aanvragenmag($lidid)) {
					$i_login->add($lidid, $_POST['gewenstelogin'], $_POST['gewenstwachtwoord']);
				} else {
					if (count($i_login->beperkttotgroep) > 1) {
						$gt = "Webmasters";
						foreach ($i_login->beperkttotgroep as $gr) {
							$gt .= ", " . (new cls_Onderdeel())->naam($gr);
						}
						$mess = sprintf("Je behoort niet tot één van de groepen %s van %s. Een login mag niet aangevraagd worden.", $gt, $_SESSION['settings']['naamvereniging']);
					} else {
						$mess = sprintf("Je behoort niet tot de webmasters van %s. Een login mag niet aangevraagd worden.", $_SESSION['settings']['naamvereniging']);
					}
				}
			} elseif ($_POST['lidnummer'] > 0) {
				$mess = sprintf("De combinatie van lidnummer %d en e-mailadres '%s' is in de database onbekend.", $_POST['lidnummer'], $_POST['emailadres']);
			} elseif ($lidid == -1) {
				$mess = sprintf("E-mailadres '%s' komt meerdere keren voor, vul het lidnummer in bij de login aanvraag.", $_POST['emailadres']);
			} else {
				$mess = sprintf("E-mailadres '%s' is onbekend in de database.", $_POST['emailadres']);
			}
		}
		if (strlen($mess) > 0) {
			(new cls_Logboek())->add($mess, 5, $lidid, 1);
		}
		printf("<p><a href='%s' target='_top'>Klik hier om verder te gaan.</a></p>\n", BASISURL);
		
	} else {

		echo("<div id='loginaanvraag'>\n");
		printf("<form action='%s?tp=Bevestiging+login' method='post'>\n", $_SERVER["PHP_SELF"]);
		echo("<h3>Login aanvraag</h3>\n");
		
		echo("<label>E-mailadres</label><input type='email' name='emailadres' value=''>\n");
		echo("<label>Lidnummer</label><input type='number' name='lidnummer'>\n");
		printf("<label>Gewenste login</label><input type='text' name='gewenstelogin' maxlength=%d>\n", $_SESSION['settings']['login_maxlengte']);
		echo("<label>Gewenst wachtwoord</label><input type='password' name='gewenstwachtwoord' value=''>\n");

//		echo("</fieldset>\n");

		echo("<div class='clear'></div>\n");

		echo("<p>Je lidnummer kan je <a href='\?tp=Opvragen+lidnr'>opvragen</a>.</p>\n");
		echo("<p>Je ontvangt een e-mail met daarin een link om deze aanmelding te bevestigen. Pas na het bevestigen is de login bruikbaar om in te loggen.</p>\n");
		
		echo("<p>Voorwaarden aan een login:</p>\n<ul>\n");
		printf("<li>Minimaal 7 karakters en maximaal %d karakters lang</li>\n", $_SESSION['settings']['login_maxlengte']);
		echo("<li>Er mogen geen aanhalingstekens en geen spaties in zitten</li>\n");
		printf("<li>De karakters '%s' mogen er niet in zitten</li>\n", htmlent($login_verbodenkarakters));
		echo("</ul>\n");
		
		printf(fneisenwachtwoord());
		
		echo("<div id='opdrachtknoppen'>\n");
		echo("<input type='submit' name='loginaanvragen' value='Aanvragen'>\n");
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		
		echo("</form>\n");
		echo("</div>  <!-- Einde loginaanvraag -->\n");
	}
} # fnLoginAanvragen

function fnOpvragenLidnr($stap) {
	
	$mess = "";
			
	if ($stap == "mail") {
		$lidid = 0;
		if (!isValidMailAddress($_POST['emailvoorlidnr'], 0)) {
			$mess = "Je hebt geen geldig e-mailadres opgegeven.";
		} elseif ($_SESSION['settings']['mailing_lidnr'] > 0) {
			$rows = (new cls_Lid())->lidbijemail($_POST['emailvoorlidnr']);
			if (count($rows) > 0) {
				foreach ($rows as $row) {
					$mail = new email(0, $_SESSION['settings']['mailing_lidnr']);
					$mail->xtranum = $row->Lidnr;
					$mail->toevoegenlid($row->LidID);
					$mail->bericht = (new Mailing($_SESSION['settings']['mailing_lidnr']))->merge($row->LidID);
					if ($mail->to_outbox(1) > 0) {
						$mess = sprintf("Een e-mail aan %s met lidnummer %d is in de outbox geplaatst en wordt zo spoedig mogelijk verzonden. ", $row->NaamLid, $row->Lidnr);
					} else {
						$mess = sprintf("Fout bij het versturen van het lidnummer aan %s. Probeer het later nogmaals of neem contact met de webmaster op. ", $row->NaamLid);
					}
					(new cls_Logboek())->add($mess, 5, $row->LidID, 1, $row->LidID, 11);
					$mess = "";
					$mail = null;
					
				}
			} else {
				$mess = "Er is geen lid met dit e-mailadres in onze database bekend.";
			}
		} else {
			$mess = "Er is geen mailing beschikbaar voor het versturen van het lidnummer. Neem contact op met de webmaster.";
		}
		if (strlen($mess) > 0) {
			(new cls_Logboek())->add($mess, 5, 0, 1);
		}
		printf("<p><a href='%s' target='_top'>Klik hier om verder te gaan.</a></p>\n", BASISURL);
		
	} elseif ($_SESSION['settings']['mailing_lidnr'] > 0) {
		
		echo("<div id='opvragenlidnr'>\n");

		printf("<form action='%s?tp=Opvragen+lidnr' method='post'>\n", $_SERVER["PHP_SELF"]);
		
		echo("<h3>Opvragen Lidnummer</h3>\n");
		echo("<fieldset>\n");
		echo("<label>E-mailadres</label><input type='email' name='emailvoorlidnr'>\n");
		echo("</fieldset>\n");
		echo("<div id='opdrachtknoppen'>\n");
		echo("<input type='submit' name='opvragenlidnr' value='Opvragen'>\n");
		echo("</div> <!-- Einde opdrachtknoppen -->\n");
		
		echo("</form>\n");
		echo("</div>  <!-- Einde opvragenlidnr -->\n");
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
	} elseif (strlen($login) > 0 and strpos(strtolower($ww), strtolower($login)) !== false) {
		$mess = "Het wachtwoord mag niet je login bevatten.";
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
	
//	debug($tp, 1);
	
	$i_acc = new cls_Authorisation();
//	debug($tp . " in addtp");
	
	if ((strstr($tp, "/") === false or startwith($tp, $currenttab . "/")) and ($_SESSION['webmaster'] == 1 or toegang($tp, 0, 0))) {
//	if (toegang($tp, 0, 0)) {
//		debug($tp . " toegevoegd");
		$tabpages[] = $tp;
		$aid = $i_acc->recordid($tp);
		
		if ($_SESSION['webmaster'] == 1 and $aid == 0) {
			$i_acc->add($tp);
		}
	}
	
	$i_acc = null;
	
} # addtp

?>
