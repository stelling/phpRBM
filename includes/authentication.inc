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
	if (strlen($_SESSION['lidgroepen']) < 4) {
		$_SESSION['lidgroepen'] = (new cls_Lidond())->lidgroepen();;
	}
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
	addtp("Vereniging/Introductie");
	addtp("Vereniging/Kader");
	addtp("Vereniging/Overig");
	addtp("Vereniging/Onderscheidingen");
	if ($_SESSION['lidid'] > 0) {
		addtp("Eigen gegevens");
	}
	$i_lo = new cls_Lidond();
	
	if ($currenttab == "Eigen gegevens" or ($currenttab == "Ledenlijst" and $currenttab2 == "Overzicht lid")) {
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
		
		addtp($b . "Algemeen");
		$f = "`Type`='A' AND IFNULL(VervallenPer, CURDATE()) >= CURDATE()";
		if ((new cls_Onderdeel())->aantal($f) > 0) {
			addtp($b . "Afdelingen");
		}
		if (count($i_lo->overzichtlid($lidid, "K")) > 0) {
			addtp($b . "Kader");
		}
		if (count($i_lo->overzichtlid($lidid, "G")) > 0) {
			addtp($b . "Groepen");
		}
		if (count($i_lo->overzichtlid($lidid, "R")) > 0) {
			addtp($b . "Rollen");
		}
		
		$f = "`Type`='T' AND IFNULL(VervallenPer, CURDATE()) >= CURDATE()";
		if ($gs != "B" and (new cls_Onderdeel())->aantal($f) > 0) {
			addtp($b . "Toestemmingen");
		}
		if (count((new cls_Aanwezigheid())->overzichtlid($lidid)) > 0) {
			addtp($b . "Presentie");
		}
		
		if (count((new cls_Bewaking())->overzichtlid($lidid)) > 0 or count((new cls_InsBew())->overzichtlid($lidid)) > 0) {
			addtp($b . "Bewaking");
		}
		
		if (count((new cls_Liddipl())->overzichtlid($lidid)) > 0) {
			addtp($b . "Diploma's");
		}
		
		$f = sprintf("LidID=%d", $lidid);
		if ((new cls_Mailing_hist())->aantal($f) > 0) {
			addtp($b . "Mailing");
		}
		if (count((new cls_Evenement_Deelnemer())->overzichtlid($lidid)) > 0) {
			addtp($b . "Evenementen");
		}
		$f = sprintf("Lid=%d", $lidid);
		if ((new cls_Rekening())->aantal($f) > 0) {
			addtp($b . "Rekeningen");
		}
		if (count((new cls_Logboek())->overzichtlid($lidid)) > 0) {
			addtp($b . "Logboek");
		}
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
		if (strlen($_SESSION['settings']['zs_muteerbarememos']) > 0) {
			addtp("Zelfservice/Bijzonderheden");
		}
		if (count(db_bewaking_blok("openblokken")) > 0 and $gs != "B") {
			addtp("Zelfservice/Inschrijven bewaking");
		}
		if (count(db_evenement("open")) > 0) {
			addtp("Zelfservice/Evenementen");
		}
		if ((new cls_Onderdeel())->aantal(0, "`Type`='T'") > 0 and $gs != "B") {
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
		if ((new cls_db_base("Lidmaatschap"))->aantal("LIDDATUM > CURDATE()")) {
			addtp("Ledenlijst/Toekomstige leden");
		}
		
		if (count(db_lid("ledenlijst", "(LM.Lidnr IS NULL)")) > 0) {
			addtp("Ledenlijst/Klosleden");
		}
		addtp("Ledenlijst/Nieuw (klos)lid");
		addtp("Ledenlijst/Groepen muteren");
		addtp("Ledenlijst/Mutaties");
	}
	
	if (strlen($_SESSION['settings']['menu_met_afdelingen']) > 0) {
		// Tabblad per afdeling
		foreach ((new cls_Onderdeel())->lijst(1, "`Type`='A'") as $row) {
			if (in_array($row->RecordID, explode(",", $_SESSION['settings']['menu_met_afdelingen']))) {
				addtp($row->Naam, $row->RecordID);
				if ($currenttab == $row->Naam) {
					addtp($row->Naam . "/Afdelingslijst", $row->RecordID);
					if (count(db_lidond("actueleleden", 0, "", "LO.GroepID > 0", $row->RecordID)) > 0) {
						addtp(trim($row->Naam) . "/Groepsindeling", $row->RecordID);
					}
					addtp(trim($row->Naam) . "/Groepsindeling muteren");
//					addtp(trim($row->Naam) . "/Kalender muteren");
				}
			}
		}
	}

	if (!isset($_GET['lidid']) or strlen($_GET['lidid']) == 0) {
		$_GET['lidid'] = 0;
	}
	$gs = (new cls_Lid())->Geslacht($_GET['lidid']);

	if ($currenttab2 == "Wijzigen lid") {
		$b = "Ledenlijst/Wijzigen lid";
		addtp($b);
		$b .= "/";
		addtp($b . "Algemene gegevens");
		if ((new cls_Onderdeel())->aantal("Type='A'") > 0) {
			addtp($b . "Afdelingen");
		}
		addtp($b . "B, C en F");
		if ((new cls_Onderdeel())->aantal("Type='G'") > 0) {	
			addtp($b . "Groepen");
		}
		if ((new cls_Onderdeel())->aantal("Type='O'") > 0) {	
			addtp($b . "Onderscheidingen");
		}
		if ($gs != "B" and (new cls_Diploma())->aantal() > 0) {
			addtp($b . "Diploma's");
		}
		$f = "`Type`='T' AND IFNULL(VervallenPer, CURDATE()) >= CURDATE()";
		if ($gs != "B" and (new cls_Onderdeel())->aantal($f) > 0) {
			addtp($b . "Toestemmingen");
		}
		if (strlen($_SESSION['settings']['muteerbarememos']) > 0) {
			addtp($b . "Bijzonderheden");
		}
		addtp($b . "Lidmaatschap");
		addtp($b . "Pasfoto");
	}
	
	if ((new cls_Mutatie())->aantal() > 0) {
		addtp("Kostenoverzicht");
	}
	
	addtp("Mailing");
	$i_M = new cls_Mailing();
	if ($currenttab == "Mailing") {
		$f = "Template=1 AND (deleted_on IS NULL)";
		if ($i_M->aantal($f) > 0) {
			addtp("Mailing/Templates");
		}
		$f = "Template=0 AND (sent_on IS NULL) AND (deleted_on IS NULL)";
		if ($i_M->aantal($f) > 0) {
			addtp("Mailing/Concepten");
		}
		addtp("Mailing/Nieuw");
		if ((new cls_Rekening())->aantal() > 0) {
			addtp("Mailing/Rekeningen");
		}
		
		$f = "(sent_on IS NOT NULL) AND (deleted_on IS NULL)";
		if ($i_M->aantal($f) > 0) {
			addtp("Mailing/Verzonden");
		}
		
		$f = "(deleted_on IS NOT NULL)";
		if ($i_M->aantal($f) > 0) {
			addtp("Mailing/Prullenbak");
		}
		$f = "(send_on IS NULL)";
		if ((new cls_Mailing_hist())->aantal($f) > 0) {
			addtp("Mailing/Outbox");
		}
		addtp("Mailing/Verzonden mails");
		addtp("Mailing/Instellingen");
		if (db_logboek("aantal", "", 4) > 0) {
			addtp("Mailing/Logboek");
		}
	}
	$i_M = null;

	$i_db = new cls_db_base("Bewseiz");
	if ($i_db->aantal() > 0) {
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
		$f = "IFNULL(VerwijderdOp, '2000-01-01') < '2012-01-01'";
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
	
	addtp("Bestellingen");
	if ($currenttab == "Bestellingen") {
		if ((new cls_Artikel())->aantal() > 0) {
			addtp("Bestellingen/Bestellingen muteren");
		}
		addtp("Bestellingen/Artikelbeheer");
		addtp("Bestellingen/Logboek");
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
	
	if ((!isset($_SESSION['username']) or strlen($_SESSION['username']) == 0)) {
		if (isset($_COOKIE['username'])) {
			$_SESSION['username'] = $_COOKIE['username'];
		} else {
			$_SESSION['username'] = "";
		}
	}
	
	$pwfromcookie = 0;	
	if (strlen($password) == 0 and isset($_COOKIE['password']) and strlen($_COOKIE['password']) > 5) {
		$password = $_COOKIE['password'];
		$pwfromcookie = 1;
	}
		
	$mess = "";
	if ($_SESSION['settings']['login_maxinlogpogingen'] > 0 and db_logboek("iplogincontrole") > $_SESSION['settings']['login_maxinlogpogingen']) {
		$mess = sprintf("Inloggen vanaf IP-adres '%s' is geblokkeerd, omdat er teveel mislukte inlog-pogingingen mee gedaan zijn! Je bent niet ingelogd.", $_SERVER['HTTP_USER_AGENT']);
		$_SESSION['lidid'] = 0;
			
	} elseif (isset($_SESSION['username']) and strlen($_SESSION['username']) > 0 and strlen($password) > 0) {
		$row = db_logins("controle", "", $_SESSION['username'], 0, "", $password);
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
			(new cls_Login())->setingelogd($row->LidID, $p_napost);
		} else {
			if ($pwfromcookie == 0) {
				$mess = sprintf("De combinatie van login '%s' en het ingevoerde wachtwoord is niet correct! Je bent niet ingelogd.", $_SESSION['username']);
			}
			$_SESSION['lidid'] = 0;
		}
	} else {
		$_SESSION['lidid'] = 0;
	}
	if ($melding == 1 and strlen($mess) > 0) {
		(new cls_Logboek())->add($mess, 1, 0, 1, 0, 4);
	} elseif (strlen($mess) > 0) {
		(new cls_Logboek())->add($mess, 1, 0, 0, 0, 4);
	}
	if ($_SESSION['lidid'] == 0 or strlen($_SESSION['username']) == 0) {
		$_SESSION['lidid'] = 0;
		$_SESSION['username'] = "";
	}
	
} # fnAuthenticatie
	
function toegang($soort, $melding=1, $log=1) {
	global $lididwebmasters, $tabpages, $currenttab, $currenttab2, $currenttab3;

	$arrAltijdToeg1ang[] = "Validatie login";
	$arrAltijdToegang[] = "Bevestiging login";
	$arrAltijdToegang[] = "Opvragen lidnr";
	if ($_SESSION['lidid'] == 0) {
		$arrAltijdToegang[] = "Herstellen wachtwoord";
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
	$_SESSION['lidgroepen'] = "0";
	if ($_SESSION['lidid'] > 0 and in_array($_SESSION['lidid'], $lididwebmasters)) {
		$_SESSION['webmaster'] = 1;
	}
	$_SESSION['lidgroepen'] = (new cls_Lidond())->lidgroepen();
	
	$soort = str_replace("'", "", $soort);
		
	if (strlen($soort) == 0 or in_array($soort, $arrAltijdToegang) === true or $_SESSION['webmaster'] == 1) {
		return true;
	} elseif ($_SESSION['lidid'] > 0 and isset($_SESSION['toegang'][$soort]) and $_SESSION['toegang'][$soort] == 1) {
		return true;
	} else {
		$_SESSION['webmaster'] == 0;
		if ((new cls_Authorisation())->toegang($soort) == true) {
			$_SESSION['toegang'][$soort] = 1;
			return true;
		} else {
			$mess = sprintf("Je hebt geen toegang tot '%s' op %s.", $soort, $_SESSION['settings']['naamwebsite']);
			if ($log == 1) {
				(new cls_Logboek())->add($mess, 15, 0, $melding);
			} elseif ($melding > 0) {
				printf("<p class='mededeling'>%s</p>\n", $mess);
			}
			return false;
		}
	}
}  # toegang

function fnHerstellenWachtwoord($stap="", $lidid=0) {
	
	$mess = "";
	
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

		$row = db_logins("rowlogin", "", "", $lidid)[0];
		if ($_SESSION['settings']['login_maxinlogpogingen'] > 0 and $row->FouteLogin > $_SESSION['settings']['login_maxinlogpogingen']) {
			$mess = "Er is teveel foutief met deze login proberen in te loggen. Hierdoor is dit account geblokkeerd. ";
			if ($_SESSION['settings']['login_autounlock'] > 0 and $_SESSION['settings']['login_autounlock'] <= 24*60) {
				$mess .= sprintf("Uiterlijk om %s uur is deze blokkade verwijderd.", date("G:i", strtotime($row->Gewijzigd)+(60*$_SESSION['settings']['login_autounlock'])));
			} else {
				$mess .= sprintf("Vraag aan de <a href='mailto:%s'>webmaster</a> om deze vrij te geven.", $_SESSION['settings']['emailwebmaster']);
			}
			
		} elseif ($_SESSION['settings']['mailing_herstellenwachtwoord'] > 0) {
			
			$mailing = new Mailing($_SESSION['settings']['mailing_herstellenwachtwoord']);
			if ($mailing->send($row->RecordID) > 0) {
				return sprintf("De e-mail naar %s met de link om het wachtwoord te herstellen is verzonden.", $row->Naam);
			} else {
				return "Fout bij het versturen van de e-mail. Probeer het later nogmaals of neem contact op met de webmaster.";
			}
			$mailing = null;

		} else {
			$mess = "Er is geen mailing voor het herstellen van het wachtwoord beschikbaar.";
		}
		
		(new cls_Logboek())->add($mess, 5, $row->RecordID, 1);
		
	} elseif ($_SERVER['REQUEST_METHOD'] == "POST" and isset($_POST['nieuwwachtwoord']) and strlen($_POST['nieuwwachtwoord']) > 0 and $_GET['lidid'] > 0) {
		$mess = (new cls_login())->wijzigenwachtwoord($_POST['nieuwwachtwoord'], "", $_POST['herhaalwachtwoord'], $_GET['lidid']);
	
	} elseif (isset($_GET["lidid"]) and $_GET["lidid"] > 0 and strlen($_GET["key"]) > 5) {
		$row = (new cls_Login())->record($_GET['lidid']);
		
		$login = $row->Login;
		$dbkey = $row->ActivatieKey;
		
		echo("<div id='herstellenwachtwoord'>\n");
		if (strlen($login) > 5 and password_verify($_GET["key"], $dbkey)) {
			
			printf("<form name='HerstellenWachtwoord' action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
			printf("
			<h3>Herstellen wachtwoord</h3>
			<label>Login:</label><input type=text value='%s' name='login' readonly>
			<label>Nieuw wachtwoord:</label><input type='password' name='nieuwwachtwoord' size=25>
			<label>Herhaal wachtwoord:</label><input type='password' name='herhaalwachtwoord' size=25>
			<input type='submit' value='Bevestig'>\n
			</form>
			%s", $login, fneisenwachtwoord());
		} else {
			echo("<h3>Deze link is niet correct, vraag een nieuwe aan.</h3>\n");
		}
		echo("</div>  <!-- Einde herstellenwachtwoord -->");
	
	} elseif ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (isset($_POST['login']) and strlen($_POST['login']) < 7 and strlen($_POST['login']) > 0) {
			$mess = "Een login moet minimaal 7 karakers hebben.";
		} elseif (isset($_POST['email']) and strlen($_POST['email']) > 0 and !isValidMailAddress($_POST['email'], 0)) {
			$mess = sprintf("'%s' is geen geldig e-mailadres.", $_POST['email']);
		} elseif (isset($_POST['login']) and strlen($_POST['login']) < 7 and !isValidMailAddress($_POST['email'], 0)) {
			$mess = "Vul minimaal 2 van de 3 velden in.";
		} elseif (isset($_POST['lidnummer']) and $_POST['lidnummer'] < 1 and !isValidMailAddress($_POST['email'], 0)) {
			$mess = "Vul minimaal 2 van de 3 velden in.";
		} elseif (isset($_POST['login']) and strlen($_POST['login']) < 7 and $_POST['lidnummer'] < 1) {
			$mess = "Vul minimaal 2 van de 3 velden in.";
		} elseif (isset($_POST['login'])) {
			if (strlen($_POST['login']) > 5) {
				$w = sprintf("Login='%s'", $_POST['login']);
			} else {
				$w = "";
			}
			if (isValidMailAddress($_POST['email'], 0)) {
				if (strlen($w) > 0) {
					$w .= " AND ";
				}
				$w .= sprintf("(LOWER(L.Email)='%1\$s' OR LOWER(L.EmailVereniging)='%1\$s')", strtolower($_POST['email']));
			}
			if ($_POST['lidnummer'] > 0) {
				if (strlen($w) > 0) {
					$w .= " AND ";
				}
				$w .= sprintf("LM.Lidnr=%d", $_POST['lidnummer']);
			}
			$query = sprintf("SELECT MAX(Login.LidID)
									FROM (%1\$sAdmin_login AS Login INNER JOIN %1\$sLid AS L ON L.RecordID = Login.LidID) INNER JOIN %1\$sLidmaatschap AS LM ON LM.Lid = L.RecordID
									WHERE %2\$s;", TABLE_PREFIX, $w);
			$lidid = db_scalar($query);
			if ($lidid > 0) {
				$mess = fnHerstellenWachtwoord("mail", $lidid);
			} else {
				$mess = "De ingevulde combinatie is niet gevonden. Er wordt geen e-mail verstuurd om het wachtwoord te herstellen.";
			}
			printf("<p class='mededeling'>%s</p>", htmlentities($mess));
		}
		echo("<p><a href='/'>Klik hier om verder te gaan.</a></p>\n");
	} else {
		echo("<div id='herstellenwachtwoord'>\n");
		printf("<form name='HerstellenWachtwoord' action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
		echo("<fieldset>
		<h3>Herstellen wachtwoord</h3>
		<label>E-mailadres:</label><input type='text' name='email' size=50 maxlength=45>
		<label>Lidnummer:</label><input class='inputnumber' type='number' min=0 name='lidnummer' value=0>
		<label>Login:</label><input type='text' name='login' size=50 maxlength=15>
		<input type='hidden' value='mail' name='stap'>
		<input type='submit' value='Stuur herstellink'>\n
		</fieldset>
		</form>
		<p>Minimaal 2 van de 3 velden moeten correct zijn ingevuld. Na het versturen van deze link is je oude wachtwoord niet meer geldig.</p>
		</div>  <!-- Einde herstellenwachtwoord -->");
	}
	return $mess;
	
} # fnHerstellenWachtwoord

function fnValidatieLogin($lidid, $key, $stap) {
	
	$mess = "";
	if ($stap == "mail") {
	
		$row = db_logins("rowlogin", "", "", $lidid)[0];
		
		if ($_SESSION['settings']['mailing_validatielogin'] == 0) {
			$inst_M = new cls_Mailing();
			$content = "<p>Activatie url [%NAAMWEBSITE%]: https://[%URLACTIVATIE%]</p>\n";
			$mid = $inst_M->add("Validatielink nieuwe login");
			$inst_M->update($mid, "from_addr", $_SESSION['settings']['emailwebmaster']);
			$inst_M->update($mid, "template", 1);
			$inst_M->update($mid, "message", $content);
			(new cls_Parameter())->update("mailing_validatielogin", $mid);
			$inst_M = null;
		}
	
		if ($row->LidID > 0) {
			$mailing = new Mailing($_SESSION['settings']['mailing_validatielogin']);
			if ($mailing->send($row->LidID) == 0) {
				$mess = "Fout bij het versturen van de e-mail met de validatielink. Probeer het later nogmaals of neem contact op met de webmaster.";
			}
			$mailing = null;
		} else {
			$mess = sprintf("Login voor lidid %d bestaat niet, neem onctact op met de webmaster.", $lidid);
		}

	} else {
		$mess = db_logins("valideerlogin", "", "", $lidid);
	}
	if (strlen($mess) > 2) {
		(new cls_Logboek())->add("fnValidatieLogin: " . $mess, 5, $lidid, 1);
	}
	
	return $mess;
}  # fnValidatieLogin

function fnLoginAanvragen($stap="") {
	global $lididwebmasters, $lididtestusers, $selectlidnr;
	
	$login_verbodenkarakters = "@#$^&;*%éëèöôü";
	
	if ($_SERVER['REQUEST_METHOD'] == "POST" and isset($_POST['loginaanvragen'])) {

		$login = "";
		$mess = "";
		$lidid = 0;
		if (!isset($_POST['gewenstelogin'])) {
			$_POST['gewenstelogin'] = "";
		}
		if (!isset($_POST['gewenstwachtwoord'])) {
			$_POST['gewenstwachtwoord'] = "";
		}
		$wwok = checknewpassword($_POST['gewenstwachtwoord'], $_POST['gewenstelogin']);
		if (!isset($_POST['lidnummer']) or strlen($_POST['lidnummer']) == 0) {
			$_POST['lidnummer'] = 0;
		}
		$i_login = new cls_Login();
		$f = sprintf("(LidID=%d OR Login='%s')", $_POST['lidnummer'], $_POST['gewenstelogin']);
		if ($i_login->aantal($f) > 0) {
			$mess = "Deze login is al in gebruik en/of er is al een login aan dit lidnummer gekoppeld.";
		} elseif (strlen($_POST['email']) == 0) {
			$mess = "Je hebt geen e-mailadres opgegeven.";
		} elseif (!isValidMailAddress($_POST['email'], 0)) {
			$mess = sprintf("Je hebt geen geldig e-mailadres (%s) opgegeven.", $_POST['email']);
		} elseif ($_SESSION['settings']['login_lidnrnodigbijaanvraag'] == 1 and $_POST['lidnummer'] == 0) {
			$mess = "Je moet je lidnummer invullen om een login te kunnen aanvragen.";
		} else {
			if ($_POST['lidnummer'] > 0 and in_array($_POST['lidnummer'], $lididtestusers) == false) {
				$xf = sprintf("(%s)=%d", $selectlidnr, $_POST['lidnummer']);
			} else {
				$xf = "";
			}
			$gevondenverbodenkarakter = "";
			for ($i=0; $i < strlen($login_verbodenkarakters); $i++) {
				if (strpos($_POST['gewenstelogin'], substr($login_verbodenkarakters, $i, 1)) > 0) {
					$gevondenverbodenkarakter = substr($login_verbodenkarakters, $i, 1);
					break;
				}
			}
			$rows = db_logins("rowlogin", $_POST['email'], "", 0, $xf);
			if (count($rows) > 0) {
				$inst_Login = new cls_Login();
				foreach ($rows as $row) {
					if (isset($row->Login) and strlen($row->Login) > 5 and ($_SESSION['settings']['login_lidnrnodigbijaanvraag'] == 0 or $row->Lidnr == $_POST['lidnummer'])) {
						$lidid = $row->RecordID;
					} else {
						// E-mailadres is bekend, maar een aan dit e-mailadres gekoppelde login bestaat niet.
						if ($_SESSION['settings']['mailing_lidnr'] > 0 and $_POST['lidnummer'] == 0 and 1 == 2) { // Controleren of deze if mag vervallen
							$mess .= fnConfirmLidnr($row) . " ";
							$lidid = $row->RecordID;
						} elseif ($row->AanvragenMag == 0 and in_array($row->RecordID, $lididwebmasters) == false and in_array($row->RecordID, $lididtestusers) == false) {
							if (is_array($_SESSION['settings']['login_beperkttotgroep']) and count($_SESSION['settings']['login_beperkttotgroep']) > 1) {
								$gt = "";
								foreach ($_SESSION['settings']['login_beperkttotgroep'] as $gr) {
									if (strlen($gt) > 0) {$gt .= ", ";}
									$gt .= "'" . db_naam_onderdeel($gr) . "'";
								}
								$mess = sprintf("Je behoort niet tot één van de groepen %s van %s. Een login mag niet aangevraagd worden.", $gt, $_SESSION['settings']['naamvereniging']);
							} else {
								if (is_array($_SESSION['settings']['login_beperkttotgroep']) and count($_SESSION['settings']['login_beperkttotgroep']) == 1) {
									$gr = $_SESSION['settings']['login_beperkttotgroep'][0];
								} elseif (is_numeric($_SESSION['settings']['login_beperkttotgroep'])) {
									$gr = $_SESSION['settings']['login_beperkttotgroep'];
								} else {
									$gr = 0;
								}
								$mess = sprintf("Je behoort niet tot groep '%s' van %s. Een login mag niet aangevraagd worden.", db_naam_onderdeel($gr), $_SESSION['settings']['naamvereniging']);
							}
						} elseif (strlen($_POST['gewenstelogin']) == 0) {
							$mess = "Je hebt geen gewenste login opgegeven.";
						} elseif (strlen($_POST['gewenstelogin']) < 6) {
							$mess = "Je hebt geen geldige gewenste login opgegeven. Een login moet uit minimaal 6 karakters bestaan.";
						} elseif (strlen($_POST['gewenstelogin']) > $_SESSION['settings']['login_maxlengte']) {
							$mess = sprintf("Je hebt geen geldige login opgegeven. Een login mag uit maximaal %d karakters bestaan.", $_SESSION['settings']['login_maxlengte']);
						} elseif (strpos($_POST['gewenstelogin'], " ") > 0) {
							$mess = "Je hebt geen geldige login opgegeven. Er mag geen spatie in een login zitten.";
						} elseif (strpos($_POST['gewenstelogin'], ";") > 0) {
							$mess = "Je hebt geen geldige login opgegeven. Er mag geen puntkomma (;) in een login zitten.";
						} elseif (strpos($_POST['gewenstelogin'], "'") > 0 or strpos($_POST['gewenstelogin'], "\"") > 0) {
							$mess = "Je hebt geen geldige login opgegeven. Er mogen geen aanhalingstekens in een login zitten.";
						} elseif (strlen($gevondenverbodenkarakter) > 0) {
							$mess = sprintf("Je hebt geen geldige login opgegeven. Het karakter '%s' mag niet in de login zitten.", $gevondenverbodenkarakter);
						} elseif ($wwok !== "ok") {
							$mess = $wwok;
						} elseif (db_logins("controlebestaat", "", $_POST['gewenstelogin'], $row->RecordID) > 0) {
							$mess = sprintf("Login '%s' is al in gebruik.", $_POST['gewenstelogin']);
						} elseif ($_SESSION['settings']['login_lidnrnodigbijaanvraag'] == 0 or $row->Lidnr == $_POST['lidnummer'] or in_array($_POST['lidnummer'], $lididtestusers)) {
							if ($inst_Login->add($row->RecordID, $_POST['gewenstelogin'], $_POST['gewenstwachtwoord'])) {
								$rl = db_logins("rowlogin", "", "", $row->RecordID);
								$lidid = $rl[0]->RecordID;
							}
						} else {
							$mess = "Je mag geen login aanvragen.";
						}
					}
				}
				$inst_Login = null;
			} else {
				if ($_POST['lidnummer'] > 0) {
					$mess = sprintf("De combinatie van lidnummer %d en e-mailadres '%s' is in de database onbekend.", $_POST['lidnummer'], $_POST['email']);	
				} else {
					$mess = sprintf("E-mailadres '%s' is onbekend in de database.", $_POST['email']);
				}
			}
		}
		(new cls_Logboek())->add($mess, 5, $lidid, 1);
		printf("<p><a href='%s' target='_top'>Klik hier om verder te gaan.</a></p>\n", BASISURL);
		
	} else {

		echo("<div id='loginaanvraag'>\n");
		printf("<form action='%s?tp=Bevestiging+login' method='post'>\n", $_SERVER["PHP_SELF"]);
		echo("<h3>Aanvragen login</h3>\n");
		echo("<fieldset>\n");
				
		if ($_SESSION['settings']['login_lidnrnodigbijaanvraag'] == 1) {
			echo("<label>Lidnummer:</label><input class='inputnumber' type='number' name='lidnummer' min=0>\n");
		}
		echo("<label>E-mailadres:</label><input type='email' name='email'>\n");
		printf("<label>Gewenste login:</label><input type='text' name='gewenstelogin' maxlength=%d>\n", $_SESSION['settings']['login_maxlengte']);
		echo("<label>Gewenst wachtwoord:</label><input type='password' name='gewenstwachtwoord' maxlength=15>\n");

		echo("<input type='submit' name='loginaanvragen' value='Aanvragen'>\n");
		echo("</fieldset>\n");
		echo("</form>\n");

		echo("<p>");
		if ($_SESSION['settings']['login_lidnrnodigbijaanvraag'] == 1) {
			echo("Met de juiste combinatie van lidnummer en e-mailadres kan je een login aanvragen.");
		} else {
			echo("Met het, in de ledenadministratie bekende e-mailadres, kan je een login aanvragen.");
		}
		echo(" Je ontvangt een e-mail met daarin een link om deze aanmelding te bevestigen. Pas daarna is de login bruikbaar om in te loggen.</p>\n");
		
		echo("<p>Voorwaarden aan een login:</p>\n<ul>\n");
		printf("<li>Minimaal 7 karakters en maximaal %d karakters lang</li>\n", $_SESSION['settings']['login_maxlengte']);
		echo("<li>Er mogen geen aanhalingstekens en geen spaties in zitten</li>\n");
		printf("<li>De karakters '%s' mogen er niet in zitten</li>\n", htmlent($login_verbodenkarakters));
		echo("</ul>\n");
		
		printf(fneisenwachtwoord());
			
		echo("</div>  <!-- Einde loginaanvraag -->\n");
	}
} # fnLoginAanvragen

function fnOpvragenLidnr($stap) {
	
	if ($stap == "mail") {
		$lidid = 0;
		if (!isValidMailAddress($_POST['emailvoorlidnr'], 0)) {
			$mess = sprintf("Je hebt geen geldig e-mailadres (%s) opgegeven.", $_POST['emailvoorlidnr']);
		} elseif ($_SESSION['settings']['mailing_lidnr'] > 0) {
			$mess = "";
			$rows = db_logins("rowlogin", $_POST['emailvoorlidnr']);
			if (count($rows) > 0) {
				foreach ($rows as $row) {
					if ($row->Lidnr > 0) {
						$mailing = new Mailing($_SESSION['settings']['mailing_lidnr']);
						$mailing->xtranum = $row->Lidnr;
						if ($mailing->send($row->RecordID) > 0) {
							$mess .= sprintf("Een e-mail met het lidnummer van %s is verzonden. ", $row->Naam);
							$lidid = $row->RecordID;
						} else {
							$mess .= sprintf("Fout bij het versturen van het lidnummer aan %s. Probeer het later nogmaals of neem contact met de webmaster op. ", $row->Naam);
						}
						$mailing = null;
					} else {
						$mess .= sprintf("%s is geen lid, het lidnummer is daarom ook niet verzonden. ", $row->Naam);
					}
				}
			} else {
				$mess = sprintf("Dit e-mailadres (%s) is onbekend in onze database.", $_POST['emailvoorlidnr']);
			}
		} else {
			$mess = "Onbekende fout";
		}
		(new cls_Logboek())->add($mess, 5, $lidid, 1);
		printf("<p><a href='%s' target='_top'>Klik hier om verder te gaan.</a></p>\n", BASISURL);
		
	} elseif ($_SESSION['settings']['mailing_lidnr'] > 0) {

		echo("<div id='opvragenlidnr'>\n");
		printf("<form action='%s?tp=Opvragen+lidnr' method='post'>\n", $_SERVER["PHP_SELF"]);
		
		echo("<fieldset>\n");
		echo("<h3>Opvragen Lidnummer</h3>\n");
		echo("<label>E-mailadres:</label><input type='email' name='emailvoorlidnr'>\n");
		echo("<input type='submit' name='opvragenlidnr' value='Opvragen'>\n");
		echo("</fieldset>\n");
			
		echo("</form>\n
		</div>  <!-- Einde opvragenlidnr -->\n");
	}
} # fnOpvragenLidnr

function cleanlogin($login) {

	$login = trim($login);
	if (strlen($login) > 15) {
		$login = substr($login, 0, 15);
	}
	$login = str_replace(" ", "_", $login);
	$login = str_replace("'", "", $login);
	$login = str_replace("\"", "", $login);
	$login = str_replace(";", "", $login);
	
	return $login;
} # cleanlogin

function newpassword($lidid) {
	
	$alphabet = "abcdefghjklmnpqrstuwxyz@!%&#";
	$ww = "";
	for ($i = 0; $i < 10; $i++) {
		$n = rand(0, strlen($alphabet)-1);
		$ww .= substr($alphabet, $n, 1);
	}
	$ww = strtoupper(substr($ww, 0, 2)) . substr($ww, 2, 4). rand(100, 999);
	
	if ($lidid <= 0) {
		return false;
	} elseif (db_change_password($ww, "", $lidid) == "ok") {
		return $ww;
	} else {
		return false;
	}
} # newpassword

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

function checknewpassword($ww, $login="", $p_wwh="") {
	
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
	} elseif (strlen($p_wwh) == 0 or $ww !== $p_wwh) {
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
	return sprintf("<p>Voorwaarden aan een wachtwoord:</p>\n
					<ul>\n
					<li>Minimaal %d karakters en maximaal %d karakters lang</li>\n
					<li>Er mogen geen aanhalingstekens en geen spaties in zitten</li>\n
					<li>Mag niet je login of roepnaam bevatten</li>\n
					<li>Moet minimaal 1 cijfer bevatten</li>\n
					<li>Moet minimaal 1 kleine letter bevatten</li>\n
					<li>Moet minimaal 1 hoofdletter bevatten</li>\n
					</ul>\n", $_SESSION['settings']['wachtwoord_minlengte'], $_SESSION['settings']['wachtwoord_maxlengte']);
} # fneisenwachtwoord

function addtp($tp, $afdnr=0) {
	global $tabpages, $currenttab, $currenttab2;
	
	if ((strstr($tp, "/") === false or startwith($tp, $currenttab . "/")) and toegang($tp, 0, 0)) {
		$tabpages[] = $tp;
		if ($_SESSION['webmaster'] == 1) {
			$query = sprintf("SELECT COUNT(*) FROM %sAdmin_access WHERE Tabpage=\"%s\";", TABLE_PREFIX, str_replace("'", "", $tp));
			if (db_scalar($query) == 0) {
				(new cls_Authorisation())->add($tp);
			}
		}
	}
	
} # addtp

?>
