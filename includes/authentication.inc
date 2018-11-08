<?php

if ((!isset($_SESSION['username']) or strlen($_SESSION['username']) == 0) and isset($_COOKIE['username'])) {
	$_SESSION['username'] = $_COOKIE['username'];
} elseif (!isset($_SESSION['username'])) {
	$_SESSION['username'] = "";
}

function toegang($soort="", $melding=1, $password="") {
	global $lididwebmasters, $table_prefix, $tabpages, $currenttab, $currenttab2;
	
	$arrAltijdToegang[] = "Herstellen wachtwoord";
	$arrAltijdToegang[] = "Validatie login";
	$arrAltijdToegang[] = "Bevestiging login";
	$arrAltijdToegang[] = "Opvragen lidnr";
	
	if (strlen($soort) == 0) {
		$soort = $currenttab;
		if (strlen($currenttab2) > 0) {
			$soort .= "/" . $currenttab2;
		}
	}

	$pwfromcookie = 0;
	if (isset($_SESSION['lastauthenication']) and $_SESSION['lastauthenication'] >= time()-7200 and $_SESSION['lidid'] > 0) {
		// Er hoeft niets te gebeuren.
	} else {
		db_logins("uitloggen");
		
		if (strlen($password) == 0 and isset($_COOKIE['password']) and strlen($_COOKIE['password']) >= db_param("wachtwoord_minlengte")) {
			$password = $_COOKIE['password'];
			$pwfromcookie = 1;
		}
		
		$mess = "";
		if (isset($_SESSION['username']) and strlen($_SESSION['username']) > 0 and strlen($password) > 0) {
			$row = db_logins("controle", "", $_SESSION['username'], 0, "", $password);
			if (db_param("login_maxinlogpogingen") > 0 and isset($row->FouteLogin) and $row->FouteLogin > db_param("login_maxinlogpogingen")) {
				if (db_param("login_autounlock") > 0 and db_param("login_autounlock") < 12*60) {
					$mess = sprintf("Login '%s' is tot uiterlijk %s uur geblokkeerd", $_SESSION['username'], date("G:i", strtotime($row->Gewijzigd)+(60*db_param("login_autounlock"))));
				} else {
					$mess = sprintf("Login '%s' is geblokkeerd", $_SESSION['username']);
				}
				$mess .= ", omdat er teveel mislukte inlog-pogingingen mee gedaan zijn! Je bent niet ingelogd.";
				
				$_SESSION['lidid'] = 0;
				
			} elseif (db_param("login_maxinlogpogingen") > 0 and db_logboek("iplogincontrole") > db_param("login_maxinlogpogingen")) {
				$mess = sprintf("Login van IP-adres '%s' is geblokkeerd, omdat er teveel mislukte inlog-pogingingen mee gedaan zijn! Je bent niet ingelogd.", $_SERVER['HTTP_USER_AGENT']);
				$_SESSION['lidid'] = 0;
				
			} elseif (isset($row->ActivatieKey) and strlen($row->ActivatieKey) > 5) {
				$mess = sprintf("Login '%s' is nog niet gevalideerd en kan nog niet worden gebruikt.", $row->Login);
				$_SESSION['lidid'] = 0;
				
			} elseif (isset($row->LidID) and $row->LidID > 0) {
				$_SESSION['lastauthenication'] = time();
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
				db_logins("setingelogd", "", "", $_SESSION['lidid']);
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
			db_logboek("add", $mess, 97, 0, 2);
		} elseif (strlen($mess) > 0) {
			db_logboek("add", $mess, 97);
		}
		if ($_SESSION['lidid'] == 0 or strlen($_SESSION['username']) == 0) {
			$_SESSION['lidid'] = 0;
			$_SESSION['username'] = "";
			$_SESSION['lidnr'] = 0;
			$_SESSION['naamingelogde'] = "gast";
			$_SESSION['roepnaamingelogde'] = "gast";
			$_SESSION['emailingelogde'] = "";
			unset($_SESSION['toegang']);
		}
	}
	if (in_array($_SESSION['lidid'], $lididwebmasters)) {
		$_SESSION['webmaster'] = 1;
	} else {
		$_SESSION['webmaster'] = 0;
	}
	
	if ($_SESSION['lidid'] == 0) {
		$_SESSION['lidgroepen'] = "(0)";
	} elseif (!isset($_SESSION['lidgroepen']) or $_SESSION['lidgroepen'] == "(0)") {
		$query = sprintf("SELECT OnderdeelID FROM %sLidond AS LO WHERE LO.Vanaf <= CURDATE() AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd >= CURDATE()) AND LO.Lid=%d;", $table_prefix, $_SESSION['lidid']);
		$result = fnQuery($query);
		$rows = $result->fetchAll(PDO::FETCH_COLUMN);
		if (count($rows) > 0) {
			$_SESSION['lidgroepen'] = "(0, " . implode(", ", $rows) . ")";
		} else {
			$_SESSION['lidgroepen'] = "(0)";
		}
	}
	
	if ($_SESSION['lidid'] > 0) {
		db_logins("updateactiviteit", "", "", $_SESSION['lidid']);
	}
	
	if (strlen($soort) > 0 and in_array($soort, $arrAltijdToegang) == false) {
		$soort = str_replace("'", "", $soort);
		if ($_SESSION['webmaster'] == 1) {
			if (!isset($_SESSION['toegang'][$soort])) {
				$query = sprintf("SELECT COUNT(*) FROM %sAdmin_access WHERE Tabpage=\"%s\";", $table_prefix, $soort);
				$result = db_scalar($query);
				if ($result == 0) {
					db_authorisation("add", 0, $soort);
				}
			}
			$_SESSION['toegang'][$soort] = true;
		} else {
			if (!isset($_SESSION['toegang'][$soort])) {
				$query = sprintf("SELECT COUNT(*) FROM %sAdmin_access WHERE Tabpage=\"%s\" AND Toegang IN %s;", $table_prefix, $soort, $_SESSION['lidgroepen']);
				$result = fnQuery($query);
				if ($result->fetchColumn() > 0) {
					$_SESSION['toegang'][$soort] = true;
				} else {
					$_SESSION['toegang'][$soort] = false;
				}
			}
			if ($_SESSION['toegang'][$soort] == false) {
				if ($melding == 1) {
					if ($_SESSION['lidid'] > 0) {
						$mess = sprintf("%s heeft geen toegang tot het onderdeel '%s' op %s.", $_SESSION['naamingelogde'], $soort, db_param("naamwebsite"));
					} else {
						$mess = sprintf("Je bent niet ingelogd en hierdoor hebt geen toegang tot '%s' op %s.", $soort, db_param("naamwebsite"));
					}
					db_logboek("add", $mess, 15, 0, 2);
				}
				$_SESSION['toegang'][$soort] = false;
			}
		}
		return $_SESSION['toegang'][$soort];
	} else {
		return true;
	}
}

function fnHerstellenWachtwoord($stap="", $lidid=0) {
	global $table_prefix;
	
	$mess = "";
	
	if (strlen($stap) == 0 and isset($_POST['stap']) and strlen($_POST['stap']) > 0) {
		$stap = $_POST['stap'];
	}
		
	if ($stap == "mail" and $lidid > 0) {
		$myFile = 'templates/Password_reset.html';
		if (file_exists($myFile)) {
			$content = file_get_contents($myFile);
		} else {	
			$content = "<p>Beste [%ROEPNAAM%],</p>\n
<p>Je hebt een reset je wachtwoord voor [%NAAMWEBSITE%] aangevraagd. Hierbij de link om je nieuwe wachtwoord op te geven.</p>\n
<ul>\n
<li>Login: [%LOGIN%]</li>\n
<li><a href='https://[%URLWEBSITE%][%URLRESETWW%]'>Klik op om nieuw wachtwoord in te geven</a></li>\n
<li>Deze link is [%GELDIGHEIDACTIVATIE%] uur geldig</li>
</ul>\n
<p>[%GEBLOKKEERD%]</p>
\n
<p>Met vriendelijke groeten,<br>\n
<strong>Webmaster [%NAAMWEBSITE%]</strong></p>\n";
		}
	
		$nk = db_logins("wachtwoordreset", "", "", $lidid);
		$row = db_logins("rowlogin", "", "", $lidid)[0];
	
		$mail = new RBMmailer($row->Nummer);
		$mail->FromName = "Webmaster " . db_param("naamwebsite");
		$mail->Subject = "Herstellen wachtwoord";

		if (db_param("login_maxinlogpogingen") > 0 and $row->FouteLogin > db_param("login_maxinlogpogingen")) {
			$geblokt = "Er is teveel foutief met deze login proberen in te loggen. Hierdoor is dit account geblokkeerd. ";
			if (db_param("login_autounlock") > 0 and db_param("login_autounlock") <= 24*60) {
				$geblokt .= sprintf("Uiterlijk om %s uur is deze blokkade verwijderd.", date("G:i", strtotime($row->Gewijzigd)+(60*db_param("login_autounlock"))));
			} else {
				$geblokt .= sprintf("Vraag aan de <a href='mailto:%s'>webmaster</a> om deze vrij te geven.", db_param("emailwebmaster"));
			}
		} else {
			$geblokt = "";
		}
	
		$urlresetww = sprintf("/index.php?tp=Herstellen+wachtwoord&lidid=%d&key=%s", $lidid, $nk);
	
		if (strlen($urlresetww) > 5) {
			$content = str_replace("[%NAAMLID%]", $row->Naam, $content);
			$content = str_replace("[%URLRESETWW%]", $urlresetww, $content);
			$content = str_replace("[%LOGIN%]", $row->Login, $content);
			$content = str_replace("[%ROEPNAAM%]", $row->Roepnaam, $content);
			$content = str_replace("[%GEBLOKKEERD%]", $geblokt, $content);
			$content = str_replace("[%GELDIGHEIDACTIVATIE%]", intval(db_param("login_geldigheidactivatie")), $content);
			$content = str_replace("[%NAAMVERENIGING%]", db_param("naamvereniging"), $content);
			$content = str_replace("[%NAAMWEBSITE%]", db_param("naamwebsite"), $content);
			$content = str_replace("[%URLWEBSITE%]", $_SERVER["HTTP_HOST"], $content);
			$content = str_replace("[%IPADDRESS%]", $_SERVER['REMOTE_ADDR'], $content);

			$mail->Body = $content;
			$mail->IsHTML(true);
			$mail->addstationary();
			if ($mail->Send()) {
				$mess = sprintf("Link om wachtwoord te herstellen is aan %s (%s) verzonden.", $row->Naam, $mail->ListAddresses("to"));
			} else {
				$mess = sprintf("Fout tijdens het versturen van de mail: %s.<br> Probeer later nogmaals of neem contact op met de webmaster.", $mail->ErrorInfo);
			}
		} else {
			$mess = "Er ging iets mis, het wachtwoord is niet gereset.";
		}
		db_logboek("add", $mess, 5, $row->Nummer);
		
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
		
		$query = sprintf("SELECT Login, ActivatieKey FROM %sAdmin_login WHERE LidID=%d;", $table_prefix, $_GET["lidid"]);
		$result = fnQuery($query);
		$row = $result->fetch();
		
		$login = $row->Login;
		$dbkey = $row->ActivatieKey;
		
		echo("<div id='profielwijzigen'>\n");
		if (strlen($login) > 5 and password_verify($_GET["key"], $dbkey)) {
			
			printf("<form name='HerstellenWachtwoord' action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
			printf("<fieldset>
			<h3>Resetten wachtwoord</h3>
			<label>Login:</label><input type=text value='%s' name='login' readonly>
			<label>Nieuw wachtwoord:</label><input type='password' name='nieuwwachtwoord' size=25>
			<label>Herhaal wachtwoord:</label><input type='password' name='herhaalwachtwoord' size=25>
			<input type='submit' class='knop' value='Bevestig'>\n
			</fieldset>
			</form>", $login, $_GET["lidid"]);
		} else {
			echo("<h3>Deze link is niet correct, vraag een nieuwe aan.</h3>\n");
		}
		echo("</div>  <!-- Einde profielwijzigen -->");
	
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
									WHERE %2\$s;", $table_prefix, $w);
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
		echo("<div id='profielwijzigen'>\n");
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
		</div>  <!-- Einde profielwijzigen -->");
	}
	return $mess;
} # fnHerstellenWachtwoord

function fnValidatieLogin($lidid, $key, $stap) {
	global $basisurl;
	
	if ($stap == "mail") {
		$content = "<p>Beste [%ROEPNAAM%],</p>\n
<p>Je hebt een nieuwe login voor [%NAAMWEBSITE%] aangevraagd. Hierbij de link om je login te bevestigen. Pas na activatie zal deze login werken.</p>\n
<ul>\n
<li>Login: [%LOGIN%]</li>\n
<li><a href='https://[%URLWEBSITE%][%URLACTIVATIE%]'>Klik hierop om de login te bevestigen.</a></li>\n
<li>Deze link is [%GELDIGHEIDACTIVATIE%] uur geldig</li>
</ul>\n
<p>[%GEBLOKKEERD%]</p>
\n
<p>Met vriendelijke groeten,<br>\n
<strong>Webmaster [%NAAMWEBSITE%]</strong></p>\n";
	
		$row = db_logins("rowlogin", "", "", $lidid)[0];
	
		$mail = new RBMmailer($row->Nummer);
		$mail->FromName = "Webmaster " . db_param("naamwebsite");
		$mail->Subject = "Link bevestiging nieuwe login";

		if (db_param("login_maxinlogpogingen") > 0 and $row->FouteLogin > db_param("login_maxinlogpogingen")) {
			$geblokt = "Er is teveel foutief met deze login proberen in te loggen. Hierdoor is dit account geblokkeerd. ";
			if (db_param("login_autounlock") > 0 and db_param("login_autounlock") <= 24*60) {
				$geblokt .= sprintf("Uiterlijk om %s uur is deze blokkade verwijderd.", date("G:i", strtotime($row->Gewijzigd)+(60*db_param("login_autounlock"))));
			} else {
				$geblokt .= sprintf("Vraag aan de <a href='mailto:%s'>webmaster</a> om deze vrij te geven.", db_param("emailwebmaster"));
			}
		} else {
			$geblokt = "";
		}
	
		$urlactivatie = sprintf("/index.php?tp=Validatie+login&lidid=%d&key=%s", $lidid, $key);
	
		if (strlen($urlactivatie) > 5) {
			$content = str_replace("[%NAAMLID%]", $row->Naam, $content);
			$content = str_replace("[%URLACTIVATIE%]", $urlactivatie, $content);
			$content = str_replace("[%LOGIN%]", $row->Login, $content);
			$content = str_replace("[%ROEPNAAM%]", $row->Roepnaam, $content);
			$content = str_replace("[%GEBLOKKEERD%]", $geblokt, $content);
			$content = str_replace("[%GELDIGHEIDACTIVATIE%]", intval(db_param("login_geldigheidactivatie")), $content);
			$content = str_replace("[%NAAMVERENIGING%]", db_param("naamvereniging"), $content);
			$content = str_replace("[%NAAMWEBSITE%]", db_param("naamwebsite"), $content);
			$content = str_replace("[%URLWEBSITE%]", $_SERVER["HTTP_HOST"], $content);
			$content = str_replace("[%IPADDRESS%]", $_SERVER['REMOTE_ADDR'], $content);

			$mail->Body = $content;
			$mail->IsHTML(true);
			$mail->addstationary();
			if ($mail->Send()) {
				$mess = sprintf(" Link om de nieuwe login te bevestigen is aan %s (%s) verzonden.", $row->Naam, $mail->ListAddresses("to"));
			} else {
				$mess = sprintf(" Fout tijdens het versturen van de mail: %s.<br> Probeer later nogmaals of neem contact op met de webmaster.", $mail->ErrorInfo);
			}
		} else {
			$mess = "Er ging iets mis, er is geen activatielink verzonden.";
		}
		db_logboek("add", $mess, 5, $row->Nummer);
	} else {
		$mess = db_logins("valideerlogin", "", "", $lidid);
		printf("<p>%s</p>\n", $mess);
	}
	return $mess;
}

function fnConfirmLidnr($row) {

	$mailing = new Mailing(db_param("mailing_lidnr"));
	$mailing->xtrachar = "LNR";
	$mailing->xtranum = $row->Lidnr;
	$mailing->resultaatversturen = 0;
	if ($mailing->send($row->Nummer) > 0) {
		return sprintf("Het lidnummer is aan %s verzonden.", $row->Naam);
	} else {
		return "Fout bij het versturen van de e-mail. Probeer het later nogmaals of neem contact op met de webmaster.";
	}
	$mailing = null;
}

function fnLoginAanvragen($stap="") {
	global $table_prefix, $lididwebmasters, $selectlidnr, $basisurl;
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {

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
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login WHERE (LidID=%d OR Login='%s');", $table_prefix, $_POST['lidnummer'], $_POST['gewenstelogin']);
		if (db_scalar($query) > 0) {
			$mess = "Deze login is al in gebruik en/of er is al een login aan dit lidnummer gekoppeld.";
		} elseif (strlen($_POST['email']) == 0) {
			$mess = "Je hebt geen e-mailadres opgegeven.";
		} elseif (!isValidMailAddress($_POST['email'], 0)) {
			$mess = sprintf("Je hebt geen geldig e-mailadres (%s) opgegeven.", $_POST['email']);
		} elseif (strlen($_POST['gewenstelogin']) > 0 and db_param("login_lidnrnodigbijaanvraag") == 1 and $_POST['lidnummer'] == 0) {
			$mess = "Je moet je lidnummer invullen om een login te kunnen aanvragen.";
		} else {
			if ($_POST['lidnummer'] > 0) {
				$xf = sprintf("(%s)=%d", $selectlidnr, $_POST['lidnummer']);
			} else {
				$xf = "";
			}
			$rows = db_logins("rowlogin", $_POST['email'], "", 0, $xf);
			if (count($rows) > 0) {
				foreach ($rows as $row) {
					if (isset($row->Login) and strlen($row->Login) > 5 and (db_param("login_lidnrnodigbijaanvraag") == 0 or $row->Lidnr == $_POST['lidnummer'])) {
						$lidid = $row->Nummer;
					} else {
						// E-mailadres is bekend, maar een aan dit e-mailadres gekoppelde login bestaat niet.
						if (db_param("mailing_lidnr") > 0 and $_POST['lidnummer'] == 0) {
							$mess .= fnConfirmLidnr($row) . " ";
							$lidid = $row->Nummer;
						} elseif ($row->AanvragenMag == 0 and in_array($row->Nummer, $lididwebmasters) == false) {
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
						} elseif ($wwok !== "ok") {
							$mess = $wwok;
						} elseif (db_logins("controlebestaat", "", $_POST['gewenstelogin'], $row->Nummer) > 0) {
							$mess = sprintf("Login '%s' is al in gebruik.", $_POST['gewenstelogin']);
						} elseif (db_param("login_lidnrnodigbijaanvraag") == 0 or $row->Lidnr == $_POST['lidnummer']) {
							if (db_logins("add", "", $_POST['gewenstelogin'], $row->Nummer, "", $_POST['gewenstwachtwoord'])) {
								$rl = db_logins("rowlogin", "", "", $row->Nummer);
								$lidid = $rl[0]->Nummer;
							}
						} else {
							$mess = "Je mag geen login aanvragen.";
							debug($mess, 0);
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
		printf("<p><a href='%s' target='_top'>Klik hier om verder te gaan.</a></p>\n", $basisurl);
		
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
		
		printf("<p>Mocht je een login hebben, maar je wachtwoord niet meer weten, dan kan je dat <a href='%s?tp=Herstellen wachtwoord'>hier</a> herstellen.</p>", $_SERVER["PHP_SELF"]);
			
		echo("</form>
		</div>  <!-- Einde loginaanvraag -->\n");
	}
} # fnLoginAanvragen

function fnOpvragenLidnr() {
	global $table_prefix, $basisurl;
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (!isValidMailAddress($_POST['emailvoorlidnr'], 0)) {
			$mess = sprintf("Je hebt geen geldig e-mailadres (%s) opgegeven.", $_POST['emailvoorlidnr']);
		} elseif (db_param("mailing_lidnr") > 0) {
			$mess = "";
			$rows = db_logins("rowlogin", $_POST['emailvoorlidnr']);
			if (count($rows) > 0) {
				foreach ($rows as $row) {
					$mailing = new Mailing(db_param("mailing_lidnr"));
					$mailing->xtrachar = "LNR";
					$mailing->xtranum = $row->Lidnr;
					$mailing->resultaatversturen = 0;
					if ($mailing->send($row->Nummer) > 0) {
						$mess .= sprintf("Het lidnummer is aan %s verzonden. ", $row->Naam);
					} else {
						$mess .= sprintf("Fout bij het versturen van het lidnummer aan %s. Probeer het later nogmaals of neem contact met de webmaster op. ", $row->Naam);
					}
					$mailing = null;
				}
			} else {
				$mess = sprintf("Dit e-mailadres (%s) is onbekend in onze database.", $_POST['emailvoorlidnr']);
			}		
			
		} else {
			$mess = "Onbekende fout in fnOpvragenLidnr";
		}
		db_logboek("add", $mess, 5, 0, 1);
		printf("<p><a href='%s' target='_top'>Klik hier om verder te gaan.</a></p>\n", $basisurl);
		
	} elseif (db_param("mailing_lidnr") > 0 and db_param("login_lidnrnodigbijaanvraag") == 1) {

		echo("<div id='opvragenlidnr'>\n");
		printf("<form name='OpvragenLidnr' action='%s?tp=Opvragen+lidnr' method='post'>\n", $_SERVER["PHP_SELF"]);
		
		echo("<fieldset>\n");
		echo("<h3>Opvragen Lidnummer</h3>\n");
		echo("<label>E-mailadres:</label><input type='email' name='emailvoorlidnr'>\n");
		echo("<input class='knop' type='submit' name='opvragenlidnr' value='Opvragen'></p>\n");
		echo("</fieldset>\n");
			
		echo("</form>
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
}

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
}

function newkey() {
	
	$alphabet = "ABCDEFGHIJKLMNOPQRSTXYZabcdefghijklmnopqrstuwxyz()@$!0123456789";
	$ww = "";
	for ($i = 0; $i < 15; $i++) {
		$n = rand(0, strlen($alphabet)-1);
		$ww .= substr($alphabet, $n, 1);
	}
	$ww = strtoupper(substr($ww, 0, 2)) . substr($ww, 2, 8) . strtoupper(substr($ww, 10, 2));
	
	return $ww;
}

function checknewpassword($ww, $login="") {
	
	$alphabet = "abcdefghijklmnopqrstuvwxyz";
	$mess = "ok";
	
	if (strlen($ww) > db_param('wachtwoord_maxlengte') and db_param('wachtwoord_maxlengte') >= 7) {
		$mess = sprintf("Het wachtwoord is te lang, het mag maximaal %d karakters lang zijn.", db_param('wachtwoord_maxlengte'));
	} elseif (strlen($ww) < db_param('wachtwoord_minlengte') and db_param('wachtwoord_minlengte') > 0) {
		$mess = sprintf("Het wachtwoord is te kort, het moet minimaal %d karakters lang zijn.", db_param('wachtwoord_minlengte'));
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
}

?>
