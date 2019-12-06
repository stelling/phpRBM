<?php

$tabpages = null;
if ($_SERVER['PHP_SELF'] == "/admin.php") {
	addtp("Beheer logins");
	addtp("Autorisatie");
	addtp("Instellingen");
	addtp("Stamgegevens");
	addtp("Uploaden data");
	if (db_interface("aantalopenstaand") > 0) {
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
	if ($currenttab == "Eigen gegevens") {
		addtp("Eigen gegevens/Algemeen");
		if (count(db_gegevenslid($_SESSION['lidid'], "Afdelingen")) > 0) {
			addtp("Eigen gegevens/Afdelingen");
		}
		if (count(db_gegevenslid($_SESSION['lidid'], "Kader")) > 0) {
			addtp("Eigen gegevens/Kader");
		}
		if (count(db_gegevenslid($_SESSION['lidid'], "Rollen")) > 0) {
			addtp("Eigen gegevens/Rollen");
		}
		if (count(db_gegevenslid($_SESSION['lidid'], "Groepen")) > 0) {
			addtp("Eigen gegevens/Groepen");
		}
		if (count(db_gegevenslid($_SESSION['lidid'], "Bew")) > 0 or count(db_insbew("overzichtlid", $_SESSION['lidid'])) > 0) {
			addtp("Eigen gegevens/Bewaking");
		}
		if (count(db_liddipl("lidgegevens", $_SESSION['lidid'])) > 0) {
			addtp("Eigen gegevens/Diploma's");
		}
		if (db_rekening("aantal", 0, "", $_SESSION['lidid']) > 0) {
			addtp("Eigen gegevens/Financieel");
		}
		addtp("Eigen gegevens/Mailing");
		if (db_onderdeel("aantal", "T") > 0) {
			addtp("Eigen gegevens/Toestemmingen");
		}
		if (count(db_gegevenslid($_SESSION['lidid'], "Evenementen")) > 0) {
			addtp("Eigen gegevens/Evenementen");
		}
		addtp("Eigen gegevens/Logboek");
	}
	
	if ($_SESSION['lidid'] > 0) {
		addtp("Zelfservice");
	}
	if ($currenttab == "Zelfservice") {
		addtp("Zelfservice/Algemene gegevens");
		if (count(db_diploma("zelfservice_lijst")) > 0) {
			addtp("Zelfservice/Diploma's");
		}
		addtp("Zelfservice/Pasfoto");
		if (db_onderdeel("aantal", "T") > 0) {
			addtp("Zelfservice/Toestemmingen");
		}
		if (count($muteerbarememos) > 0) { 
			addtp("Zelfservice/Bijzonderheden");
		}
		if (count(db_bewaking_blok("openblokken")) > 0) {
			addtp("Zelfservice/Inschrijven bewaking");
		}
		if (count(db_evenement("open")) > 0) {
			addtp("Zelfservice/Inschrijven evenementen");
		}
		if (count(db_artikel("artikelbeschikbaar")) > 0) {
			addtp("Zelfservice/Bestellingen");
		}
		if (db_lidmaatschap("kanopzeggen", $_SESSION['lidid']) == true) {
			addtp("Zelfservice/Opzegging");
		}
	}
	if ($_SESSION['lidid'] > 0) {
		addtp("Zelfservice/Wijzigen wachtwoord");
	}
	
	if (count(db_lidond("actueleleden", 0, "", "LO.GroepID > 0")) > 0) {
		addtp("Groepsindeling");
		if ($currenttab == "Groepsindeling") {
			foreach(db_onderdeel("lijstopkode", "A") as $row) {
				if (count(db_lidond("actueleleden", 0, "", "LO.GroepID > 0", $row->RecordID)) > 0) {
					addtp("Groepsindeling/" . trim($row->Kode));
				}
			}
		}
	}
	
	addtp("Ledenlijst");
	if ($currenttab == "Overzicht lid") {
		if (!isset($_GET['lidid']) or strlen($_GET['lidid']) == 0) {
			$_GET['lidid'] = 0;
		}
		addtp("Overzicht lid");
		addtp("Overzicht lid/Algemeen");
		if (count(db_gegevenslid($_GET['lidid'], "Afdelingen")) > 0) {
			addtp("Overzicht lid/Afdelingen");
		}
		if (count(db_gegevenslid($_GET['lidid'], "Kader")) > 0) {
			addtp("Overzicht lid/Kader");
		}
		if (count(db_gegevenslid($_GET['lidid'], "Rollen")) > 0) {
			addtp("Overzicht lid/Rollen");
		}
		if (count(db_gegevenslid($_GET['lidid'], "Groepen")) > 0) {
			addtp("Overzicht lid/Groepen");
		}
		if (count(db_gegevenslid($_GET['lidid'], "Bew")) > 0) {
			addtp("Overzicht lid/Bewaking");
		}
		if (count(db_liddipl("lidgegevens", $_GET['lidid'])) > 0) {
			addtp("Overzicht lid/Diploma's");
		}
		if (db_rekening("aantal") > 0) {
			addtp("Overzicht lid/Financieel");
		}
		if (count(db_gegevenslid($_GET['lidid'], "Mailing")) > 0) {
			addtp("Overzicht lid/Mailing");
		}
		if (db_onderdeel("aantal", "T") > 0) {
			addtp("Overzicht lid/Toestemmingen");
		}
		if (db_evenement("aantaldlnrs", $_GET['lidid']) > 0) {
			addtp("Overzicht lid/Evenementen");
		}
		if (count(db_logboek("lijst", "", -1, $_GET['lidid'])) > 0) {
			addtp("Overzicht lid/Logboek");
		}
	}
	if ($currenttab == "Wijzigen lid") {
		addtp("Wijzigen lid");
		addtp("Wijzigen lid/Algemene gegevens");
		addtp("Wijzigen lid/Diploma's");
		addtp("Wijzigen lid/Pasfoto");
		addtp("Wijzigen lid/Toestemmingen");
		addtp("Wijzigen lid/Bijzonderheden");
		addtp("Wijzigen lid/Lidmaatschap");
	}
	if (db_mutatie("aantal") > 0) {
		addtp("Kostenoverzicht");
	}
	
	addtp("Selecties");
	
	addtp("Mailing");
	if ($currenttab == "Mailing") {
		addtp("Mailing/Templates");
		addtp("Mailing/Concepten");
		addtp("Mailing/Nieuw");
		if (db_rekening("aantal") > 0) {
			addtp("Mailing/Rekeningen");
		}
		addtp("Mailing/Verzonden");
		if (db_mailing("aantalprullenbak") > 0) {
			addtp("Mailing/Prullenbak");
		}
		if (count(db_mailing_hist("outbox")->fetchAll()) > 0) {
			addtp("Mailing/Outbox");
		}
		addtp("Mailing/Verzonden mails");
		addtp("Mailing/Instellingen");
		if (db_logboek("aantal", "", 4) > 0) {
			addtp("Mailing/Logboek");
		}
	}

	if (db_bewseiz("aantal") > 0) {
		addtp("Bewaking");
		if ($currenttab == "Bewaking") {
			addtp("Bewaking/Bewakingsrooster");
			addtp("Bewaking/Postindeling");
			addtp("Bewaking/Aantallen");
			addtp("Bewaking/Nieuwe pasfoto");
			addtp("Bewaking/Diploma's muteren");
			if (count(db_bewaking_blok("overzicht")) > 0) {
				addtp("Bewaking/Overzicht inschrijvingen");
			}
			addtp("Bewaking/Blokken muteren");
			if (count(db_insbew("export")) > 0) {
				addtp("Bewaking/Downloaden inschrijvingen");
			}
			addtp("Bewaking/Logboek");
		}
	}
	
	addtp("Evenementen");
	if ($currenttab == "Evenementen") {
		if (db_evenement("aantal") > 0) {
			addtp("Evenementen/Overzicht");
			addtp("Evenementen/Beheer");
		}
		if (db_Evenement_Type("aantal") > 0) {
			addtp("Evenementen/Nieuw");
		}
		addtp("Evenementen/Types muteren");
		addtp("Evenementen/Logboek");
	}
	
	addtp("Bestellingen");
	if ($currenttab == "Bestellingen") {
		if (count(db_artikel("overzichtbeheer")) > 0) {
			addtp("Bestellingen/Bestellingen muteren");
		}
		addtp("Bestellingen/Artikelbeheer");
		addtp("Bestellingen/Logboek");
	}
	
	if ($_SERVER["HTTP_HOST"] == "phprbm.telling.nl") {
//		addtp("DMS");   Voorlopig geen tijd voor, dus uitgezet.
	}
}

function fnAuthenticatie($melding=1, $password="") {
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
	if (db_param("login_maxinlogpogingen") > 0 and db_logboek("iplogincontrole") > db_param("login_maxinlogpogingen")) {
		$mess = sprintf("Inloggen vanaf IP-adres '%s' is geblokkeerd, omdat er teveel mislukte inlog-pogingingen mee gedaan zijn! Je bent niet ingelogd.", $_SERVER['HTTP_USER_AGENT']);
		$_SESSION['lidid'] = 0;
			
	} elseif (isset($_SESSION['username']) and strlen($_SESSION['username']) > 0 and strlen($password) > 0) {
		$row = db_logins("controle", "", $_SESSION['username'], 0, "", $password);
		if (db_param("login_maxinlogpogingen") > 0 and isset($row->FouteLogin) and $row->FouteLogin > db_param("login_maxinlogpogingen")) {
			if (db_param("login_autounlock") > 0 and db_param("login_autounlock") < 12*60) {
				$mess = sprintf("Login '%s' is tot uiterlijk %s uur geblokkeerd", $_SESSION['username'], date("G:i", strtotime($row->Gewijzigd)+(60*db_param("login_autounlock"))));
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
		db_logboek("add", $mess, 97, 0, 1);
	} elseif (strlen($mess) > 0) {
		db_logboek("add", $mess, 97);
	}
	if ($_SESSION['lidid'] == 0 or strlen($_SESSION['username']) == 0) {
		$_SESSION['lidid'] = 0;
		$_SESSION['username'] = "";
		$_SESSION['webmaster'] = 0;
	} else {
		if (in_array($_SESSION['lidid'], $lididwebmasters)) {
			$_SESSION['webmaster'] = 1;
		}
		db_logins("setingelogd", "", "", $_SESSION['lidid']);
	}
} # fnAuthenticatie
	
function toegang($soort, $melding=1, $log=1) {
	global $lididwebmasters, $tabpages, $currenttab, $currenttab2, $muteerbarememos;
	
	$arrAltijdToegang[] = "Validatie login";
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
	}
	$soort = str_replace("'", "", $soort);
	
	if (strlen($soort) > 0 and in_array($soort, $arrAltijdToegang) === false) {
		if ($_SESSION['lidid'] > 0 and in_array($_SESSION['lidid'], $lididwebmasters) === true) {
			$_SESSION['webmaster'] == 1;
			return true;
		} elseif ($_SESSION['lidid'] > 0 and isset($_SESSION['toegang'][$soort]) and $_SESSION['toegang'][$soort] == 1) {
			return true;
		} else {
			$_SESSION['webmaster'] == 0;
			if (db_authorisation("heefttoegang", 0, $soort) == true) {
				$_SESSION['toegang'][$soort] = 1;
				return true;
			} else {
				$mess = sprintf("Je hebt geen toegang tot '%s' op %s.", $soort, db_param("naamwebsite"));
				if ($log == 1) {
					db_logboek("add", $mess, 15, 0, $melding);
				} elseif ($melding > 0) {
					printf("<p class='mededeling'>%s</p>\n", $mess);
				}
				return false;
			}
		}
	} else {
		return true;
	}
}

function fnHerstellenWachtwoord($stap="", $lidid=0) {
	
	$mess = "";
	
	if (strlen($stap) == 0 and isset($_POST['stap']) and strlen($_POST['stap']) > 0) {
		$stap = $_POST['stap'];
	}
		
	if ($stap == "mail" and $lidid > 0) {

		if (db_param("mailing_herstellenwachtwoord") == 0) {
			$content = "<p>Herstellen wachtwoord [%NAAMWEBSITE%]: https://[%URLRESETWW%]</p>\n";
			$mid = db_mailing("add", 0, 0, "", "Herstellen wachtwoord", 0, db_param("emailwebmaster"), "");
			db_mailing("update", $mid, "", "message", $content);
			db_mailing("update", $mid, "", "template", 1);
			fnQuery($query);
			db_param("mailing_herstellenwachtwoord", "updval", $mid);
		}

		$row = db_logins("rowlogin", "", "", $lidid)[0];
		if (db_param("login_maxinlogpogingen") > 0 and $row->FouteLogin > db_param("login_maxinlogpogingen")) {
			$mess = "Er is teveel foutief met deze login proberen in te loggen. Hierdoor is dit account geblokkeerd. ";
			if (db_param("login_autounlock") > 0 and db_param("login_autounlock") <= 24*60) {
				$mess .= sprintf("Uiterlijk om %s uur is deze blokkade verwijderd.", date("G:i", strtotime($row->Gewijzigd)+(60*db_param("login_autounlock"))));
			} else {
				$mess .= sprintf("Vraag aan de <a href='mailto:%s'>webmaster</a> om deze vrij te geven.", db_param("emailwebmaster"));
			}
			
		} elseif (db_param("mailing_herstellenwachtwoord") > 0) {
			
			$mailing = new Mailing(db_param("mailing_herstellenwachtwoord"));
			if ($mailing->send($row->Nummer) > 0) {
				return sprintf("De e-mail naar %s met de link om het wachtwoord te herstellen is in de outbox geplaatst.", $row->Naam);
			} else {
				return "Fout bij het versturen van de e-mail. Probeer het later nogmaals of neem contact op met de webmaster.";
			}
			$mailing = null;

		} else {
			$mess = "Er is geen mailing voor het herstellen van het wachtwoord beschikbaar.";
		}
		
		db_logboek("add", $mess, 5, 0, 1, $row->Nummer);
		
	} elseif ($_SERVER['REQUEST_METHOD'] == "POST" and isset($_POST['nieuwwachtwoord']) and strlen($_POST['nieuwwachtwoord']) > 0 and $_GET['lidid'] > 0) {
		
		$wwok = checknewpassword($_POST['nieuwwachtwoord'], $_POST['login']);
		if ($wwok !== "ok") {
			$mess = $wwok;
		} elseif (strlen($_POST['nieuwwachtwoord']) != strlen($_POST['herhaalwachtwoord']) or $_POST['nieuwwachtwoord'] !== $_POST['herhaalwachtwoord']) {
			$mess = "De twee ingevoerde wachtwoorden zijn niet exact gelijk aan elkaar. Het wachtwoord wordt niet gewijzigd.";
		} else {
			$mess = db_change_password($_POST['nieuwwachtwoord'], "", $_GET['lidid']);
		}
		db_logboek("add", $mess, 5, $_GET['lidid'], 1);
	
	} elseif (isset($_GET["lidid"]) and $_GET["lidid"] > 0 and strlen($_GET["key"]) > 5) {
		
		$query = sprintf("SELECT Login, ActivatieKey FROM %sAdmin_login WHERE LidID=%d;", TABLE_PREFIX, $_GET["lidid"]);
		$result = fnQuery($query);
		$row = $result->fetch();
		
		$login = $row->Login;
		$dbkey = $row->ActivatieKey;
		
		echo("<div id='herstellenwachtwoord'>\n");
		if (strlen($login) > 5 and password_verify($_GET["key"], $dbkey)) {
			
			printf("<form name='HerstellenWachtwoord' action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
			printf("<fieldset>
			<h3>Herstellen wachtwoord</h3>
			<label>Login:</label><input type=text value='%s' name='login' readonly>
			<label>Nieuw wachtwoord:</label><input type='password' name='nieuwwachtwoord' size=25>
			<label>Herhaal wachtwoord:</label><input type='password' name='herhaalwachtwoord' size=25>
			<input type='submit' class='knop' value='Bevestig'>\n
			</fieldset>
			</form>
			%s", $login, fneisenwachtwoord());
		} else {
			echo("<h3>Deze link is niet correct, vraag een nieuwe aan.</h3>\n");
		}
		echo("</div>  <!-- Einde herstellenwachtwoord -->");
	
	} elseif ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (strlen($_POST['login']) < 7 and strlen($_POST['login']) > 0) {
			$mess = "Eem login moet minimaal 7 karakers hebben.";
		} elseif (strlen($_POST['email']) > 0 and !isValidMailAddress($_POST['email'], 0)) {
			$mess = sprintf("'%s' is geen geldig e-mailadres.", $_POST['email']);
		} elseif (strlen($_POST['login']) < 7 and !isValidMailAddress($_POST['email'], 0)) {
			$mess = "Vul minimaal 2 van de 3 velden in.";
		} elseif ($_POST['lidnummer'] < 1 and !isValidMailAddress($_POST['email'], 0)) {
			$mess = "Vul minimaal 2 van de 3 velden in.";
		} elseif (strlen($_POST['login']) < 7 and $_POST['lidnummer'] < 1) {
			$mess = "Vul minimaal 2 van de 3 velden in.";
		} else {
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
									FROM (%1\$sAdmin_login AS Login INNER JOIN %1\$sLid AS L ON L.Nummer = Login.LidID) INNER JOIN %1\$sLidmaatschap AS LM ON LM.Lid = L.Nummer
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
		<input type='submit' class='knop' value='Stuur mail met herstellink'>\n
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
		
		if (db_param("mailing_validatielogin") == 0) {
			$content = "<p>Activatie url [%NAAMWEBSITE%]: https://[%URLACTIVATIE%]</p>\n";
			$mid = db_mailing("add", 0, 0, "", "Validatielink nieuwe login", 0, db_param("emailwebmaster"), "");
			$query  = sprintf("UPDATE %sMailing SET message=\"%s\", template=1 WHERE MailingID=%d;", TABLE_PREFIX, $content, $mid);
			fnQuery($query);
			db_param("mailing_validatielogin", "updval", $mid);
		}
	
		$mailing = new Mailing(db_param("mailing_validatielogin"));
		if ($mailing->send($row->Nummer) == 0) {
			$mess = "Fout bij het versturen van de e-mail met de validatielink. Probeer het later nogmaals of neem contact op met de webmaster.";
		}
		$mailing = null;

	} else {
		$mess = db_logins("valideerlogin", "", "", $lidid);
	}
	if (strlen($mess) > 2) {
		db_logboek("add", $mess, 5, 0, 1, $lidid);
	}
	
	return $mess;
}

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
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login WHERE (LidID=%d OR Login='%s');", TABLE_PREFIX, $_POST['lidnummer'], $_POST['gewenstelogin']);
		if (db_scalar($query) > 0) {
			$mess = "Deze login is al in gebruik en/of er is al een login aan dit lidnummer gekoppeld.";
		} elseif (strlen($_POST['email']) == 0) {
			$mess = "Je hebt geen e-mailadres opgegeven.";
		} elseif (!isValidMailAddress($_POST['email'], 0)) {
			$mess = sprintf("Je hebt geen geldig e-mailadres (%s) opgegeven.", $_POST['email']);
		} elseif (db_param("login_lidnrnodigbijaanvraag") == 1 and $_POST['lidnummer'] == 0) {
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
				foreach ($rows as $row) {
					if (isset($row->Login) and strlen($row->Login) > 5 and (db_param("login_lidnrnodigbijaanvraag") == 0 or $row->Lidnr == $_POST['lidnummer'])) {
						$lidid = $row->Nummer;
					} else {
						// E-mailadres is bekend, maar een aan dit e-mailadres gekoppelde login bestaat niet.
						if (db_param("mailing_lidnr") > 0 and $_POST['lidnummer'] == 0 and 1 == 2) { // Controleren of deze if mag vervallen
							$mess .= fnConfirmLidnr($row) . " ";
							$lidid = $row->Nummer;
						} elseif ($row->AanvragenMag == 0 and in_array($row->Nummer, $lididwebmasters) == false and in_array($row->Nummer, $lididtestusers) == false) {
							if (is_array(db_param("login_beperkttotgroep")) and count(db_param("login_beperkttotgroep")) > 1) {
								$gt = "";
								foreach (db_param("login_beperkttotgroep") as $gr) {
									if (strlen($gt) > 0) {$gt .= ", ";}
									$gt .= "'" . db_naam_onderdeel($gr) . "'";
								}
								$mess = sprintf("Je behoort niet tot één van de groepen %s van %s. Een login mag niet aangevraagd worden.", $gt, db_param("naamvereniging"));
							} else {
								if (is_array(db_param("login_beperkttotgroep")) and count(db_param("login_beperkttotgroep")) == 1) {
									$gr = db_param("login_beperkttotgroep")[0];
								} elseif (is_numeric(db_param("login_beperkttotgroep"))) {
									$gr = db_param("login_beperkttotgroep");
								} else {
									$gr = 0;
								}
								$mess = sprintf("Je behoort niet tot groep '%s' van %s. Een login mag niet aangevraagd worden.", db_naam_onderdeel($gr), db_param("naamvereniging"));
							}
						} elseif (strlen($_POST['gewenstelogin']) == 0) {
							$mess = "Je hebt geen gewenste login opgegeven.";
						} elseif (strlen($_POST['gewenstelogin']) < 6) {
							$mess = "Je hebt geen geldige gewenste login opgegeven. Een login moet uit minimaal 6 karakters bestaan.";
						} elseif (strlen($_POST['gewenstelogin']) > db_param("login_maxlengte")) {
							$mess = sprintf("Je hebt geen geldige login opgegeven. Een login mag uit maximaal %d karakters bestaan.", db_param("login_maxlengte"));
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
						} elseif (db_logins("controlebestaat", "", $_POST['gewenstelogin'], $row->Nummer) > 0) {
							$mess = sprintf("Login '%s' is al in gebruik.", $_POST['gewenstelogin']);
						} elseif (db_param("login_lidnrnodigbijaanvraag") == 0 or $row->Lidnr == $_POST['lidnummer'] or in_array($_POST['lidnummer'], $lididtestusers)) {
							if (db_logins("add", "", $_POST['gewenstelogin'], $row->Nummer, "", $_POST['gewenstwachtwoord'])) {
								$rl = db_logins("rowlogin", "", "", $row->Nummer);
								$lidid = $rl[0]->Nummer;
							}
						} else {
							$mess = "Je mag geen login aanvragen.";
						}
					}
				}
			} else {
				if ($_POST['lidnummer'] > 0) {
					$mess = sprintf("De combinatie van lidnummer %d en e-mailadres '%s' is in de database onbekend.", $_POST['lidnummer'], $_POST['email']);	
				} else {
					$mess = sprintf("E-mailadres '%s' is onbekend in de database.", $_POST['email']);
				}
			}
		}
		db_logboek("add", $mess, 5, 0, 1, $lidid);
		printf("<p><a href='%s' target='_top'>Klik hier om verder te gaan.</a></p>\n", BASISURL);
		
	} else {

		echo("<div id='loginaanvraag'>\n");
		printf("<form name=Loginaanvraag action='%s?tp=Bevestiging+login' method='post'>\n", $_SERVER["PHP_SELF"]);
		echo("<fieldset>\n");
		echo("<h3>Aanvragen login</h3>\n");
		
		if (db_param("login_lidnrnodigbijaanvraag") == 1) {
			echo("<label>Lidnummer:</label><input class='inputnumber' type='number' name='lidnummer' min=0>\n");
		}
		echo("<label>E-mailadres:</label><input type='email' name='email'>\n");
		printf("<p><label>Gewenste login:</label><input type='text' name='gewenstelogin' maxlength=%d>\n", db_param("login_maxlengte"));
		echo("<p><label>Gewenst wachtwoord:</label><input type='password' name='gewenstwachtwoord' maxlength=15>\n");

		echo("<input class='knop' type='submit' name='loginaanvragen' value='Aanvragen'></p>\n");
		echo("</fieldset>\n");

		echo("<p>");
		if (db_param("login_lidnrnodigbijaanvraag") == 1) {
			echo("Met de juiste combinatie van lidnummer en e-mailadres kan je een login aanvragen.");
		} else {
			echo("Met het, in de ledenadministratie bekende e-mailadres, kan je een login aanvragen.");
		}
		echo(" Je ontvangt een e-mail met daarin een link om deze aanmelding te bevestigen. Pas daarna is de login bruikbaar om in te loggen.</p>\n");
		
		echo("<p>Voorwaarden aan een login:<ul>\n");
		printf("<li>Minimaal 7 karakters en maximaal %d karakters lang</li>\n", db_param("login_maxlengte"));
		echo("<li>Er mogen geen aanhalingstekens en geen spaties in zitten</li>\n");
		printf("<li>De karakters '%s' mogen er niet in zitten</li>\n", htmlent($login_verbodenkarakters));
		echo("</ul>\n</p>\n");
		
		printf(fneisenwachtwoord());
			
		echo("</form>
		</div>  <!-- Einde loginaanvraag -->\n");
	}
} # fnLoginAanvragen

function fnOpvragenLidnr($stap) {
	
	if ($stap == "mail") {
		if (!isValidMailAddress($_POST['emailvoorlidnr'], 0)) {
			$mess = sprintf("Je hebt geen geldig e-mailadres (%s) opgegeven.", $_POST['emailvoorlidnr']);
		} elseif (db_param("mailing_lidnr") > 0) {
			$mess = "";
			$rows = db_logins("rowlogin", $_POST['emailvoorlidnr']);
			if (count($rows) > 0) {
				foreach ($rows as $row) {
					if ($row->Lidnr > 0) {
						$mailing = new Mailing(db_param("mailing_lidnr"));
						$mailing->xtranum = $row->Lidnr;
						if ($mailing->send($row->Nummer) > 0) {
							$mess .= sprintf("Een e-mail met Het lidnummer van %s is in de outbox geplaatst. ", $row->Naam);
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
		db_logboek("add", $mess, 5, 0, 1);
		printf("<p><a href='%s' target='_top'>Klik hier om verder te gaan.</a></p>\n", BASISURL);
		
	} elseif (db_param("mailing_lidnr") > 0) {

		echo("<div id='opvragenlidnr'>\n");
		printf("<form name='OpvragenLidnr' action='%s?tp=Opvragen+lidnr' method='post'>\n", $_SERVER["PHP_SELF"]);
		
		echo("<fieldset>\n");
		echo("<h3>Opvragen Lidnummer</h3>\n");
		echo("<label>E-mailadres:</label><input type='email' name='emailvoorlidnr'>\n");
		echo("<input class='knop' type='submit' name='opvragenlidnr' value='Opvragen'></p>\n");
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

function checknewpassword($ww, $login="") {
	
	$alphabet = "abcdefghijklmnopqrstuvwxyz";
	$mess = "ok";
	
	if (strlen($ww) > db_param('wachtwoord_maxlengte') and db_param('wachtwoord_maxlengte') >= 7) {
		$mess = sprintf("Het wachtwoord is te lang, het mag maximaal %d karakters lang zijn.", db_param('wachtwoord_maxlengte'));
	} elseif (strlen($ww) < db_param('wachtwoord_minlengte') and db_param('wachtwoord_minlengte') > 0) {
		$mess = sprintf("Het wachtwoord is te kort, het moet minimaal %d karakters lang zijn.", db_param('wachtwoord_minlengte'));
	} elseif (strpos($ww, "'") > 0 or strpos($ww, "\"") > 0) {
		$mess = "Er mogen geen aanhalingstekens in een wachtwoord zitten.";
	} elseif (strpos($ww, " ") !== false) {
		$mess = "Het wachtwoord mag geen spatie bevatten.";
	} elseif (strlen($login) > 0 and strpos(strtolower($ww), strtolower($login)) !== false) {
		$mess = "Het wachtwoord mag uw login niet bevatten.";
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
					<li>Mag niet je login bevatten</li>\n
					<li>Moet minimaal 1 cijfer bevatten</li>\n
					<li>Moet minimaal 1 kleine letter bevatten</li>\n
					<li>Moet minimaal 1 hoofdletter bevatten</li>\n
					</ul>\n", db_param("wachtwoord_minlengte"), db_param("wachtwoord_maxlengte"));
} # fneisenwachtwoord

function addtp($tp) {
	global $tabpages, $currenttab;
	
	if ((strstr($tp, "/") === false or startwith($tp, $currenttab . "/")) and toegang($tp, 0, 0)) {
		$tabpages[] = $tp;
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_access WHERE Tabpage=\"%s\";", TABLE_PREFIX, str_replace("'", "", $tp));
		if (db_scalar($query) == 0) {
			db_authorisation("add", 0, $tp);
		}
	}
	
} # addtp

?>
