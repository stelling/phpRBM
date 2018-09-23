<?php

if ((!isset($_SESSION['username']) or strlen($_SESSION['username']) == 0) and isset($_COOKIE['username'])) {
	$_SESSION['username'] = $_COOKIE['username'];
} elseif (!isset($_SESSION['username'])) {
	$_SESSION['username'] = "";
}

function toegang($soort="", $melding=1, $password="") {
	global $lididwebmasters, $table_prefix, $tabpages, $currenttab, $currenttab2;
	
	if (strlen($soort) == 0) {
		$soort = $currenttab;
		if (strlen($currenttab2) > 0) {
			$soort .= "/" . $currenttab2;
		}
	}

	if (isset($_SESSION['lastauthenication']) and $_SESSION['lastauthenication'] >= time()-7200 and $_SESSION['lidid'] > 0) {
		// Er hoeft niets te gebeuren.
	} else {
		db_logins("uitloggen");
		
		if (strlen($password) == 0 and isset($_COOKIE['password'])) {
			$password = $_COOKIE['password'];
		}
		if (strlen($password) > 12) {
			$password = substr($password, 0, 12);
		}
		
		if (isset($_SESSION['username']) and strlen($_SESSION['username']) > 0 and strlen($password) > 0) {
			$row = db_logins("controle", "", $_SESSION['username'], 0, "", $password);
			if (db_param("maxinlogpogingen") > 0 and isset($row->FouteLogin) and $row->FouteLogin > db_param("maxinlogpogingen")) {
				if (db_param("loginautounlock") > 0 and db_param("loginautounlock") < 12*60) {
				$mess = sprintf("Login '%s' is tot uiterlijk %s uur geblokkeerd", $_SESSION['username'], date("G:i", strtotime($row->Gewijzigd)+(60*db_param("loginautounlock"))));
				} else {
					$mess = sprintf("Login '%s' is geblokkeerd", $_SESSION['username']);
				}
				$mess .= ", omdat er teveel foute inlog-pogingingen mee gedaan zijn! Je bent niet ingelogd.";
				
				db_logboek("add", $mess, 1, 0, 2);
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
				if ($melding == 1) {
					$mess = sprintf("De combinatie van login '%s' en het ingevoerde wachtwoord is niet correct! Je bent niet ingelogd.", $_SESSION['username']);
					db_logboek("add", $mess, 1, 0, 2);
				}
				$_SESSION['lidid'] = 0;
			}
		} else {
			$_SESSION['lidid'] = 0;
		}
		if ($_SESSION['lidid'] == 0 or strlen($_SESSION['username']) == 0) {
			$_SESSION['lidid'] = 0;
			$_SESSION['username'] = "";
			$_SESSION['lidnr'] = 0;
			$_SESSION['naamingelogde'] = "gast";
			$_SESSION['roepnaamingelogde'] = "gast";
			$_SESSION['emailingelogde'] = "";
			unset($_SESSION['toegang']);
			if (isset($_COOKIE['password'])) {
				unset($_COOKIE['password']);
				setcookie("password", "", time()-3600);
			}
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
	
	if (strlen($soort) > 0 and $soort != "Herstellen wachtwoord") {
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
					db_logboek("add", $mess, 5, 0, 2);
				}
				$_SESSION['toegang'][$soort] = false;
			}
		}
		return $_SESSION['toegang'][$soort];
	} else {
		return true;
	}
}

function fnConfirmLogin($row, $xtratekst="", $pw="") {
	global $basisurl;
	
	$myFile = 'templates/bevestiging_login.html';
	if (file_exists($myFile)) {
		$content = file_get_contents($myFile);
	} else {
		$content = sprintf("<p>Beste [%ROEPNAAM%],</p>\n
<p>Je hebt een login voor de website <a href='%s'>[%NAAMWEBSITE%]</a> van %s aangevraagd. Hierbij je gegevens.</p>\n
<ul>\n
<li>Login: [%LOGIN%]</li>\n
<li>Wachtwoord: [%PASSWORD%]</li>\n
</ul>\n
<p>[%GEBLOKKEERD%]</p>
\n
<p>De letters i of o komen niet in een standaard wachtwoord voor.</p>
\n
<p>Met vriendelijke groeten,<br>\n
<strong>Webmaster [%NAAMWEBSITE%]</strong></p>\n", $basisurl, db_param("naamvereniging"));
	}
	
	$mail = new RBMmailer($row->Nummer);
	$mail->FromName = "Webmaster " . db_param("naamwebsite");
	$mail->Subject = sprintf("Bevestiging login %s voor %s", db_param("naamwebsite"), $row->Naam);

	if (db_param("maxinlogpogingen") > 0 and $row->FouteLogin > db_param("maxinlogpogingen")) {
		$geblokt = "Er is teveel foutief met deze login proberen in te loggen. Hierdoor is dit account geblokkeerd. ";
		if (db_param("loginautounlock") > 0 and db_param("loginautounlock") <= 24*60) {
			$geblokt .= sprintf("Uiterlijk om %s uur is deze blokkade verwijderd.", date("G:i", strtotime($row->Gewijzigd)+(60*db_param("loginautounlock"))));
		} else {
			$geblokt .= sprintf("Vraag aan de <a href='mailto:%s'>webmaster</a> om deze vrij te geven.", db_param("emailwebmaster"));
		}
	} else {
		$geblokt = "";
	}
	
	$content = str_replace("[%NAAMLID%]", $row->Naam, $content);
	$content = str_replace("[%LOGIN%]", $row->Login, $content);
	$content = str_replace("[%LIDNR%]", $row->Lidnr, $content);
	$content = str_replace("[%ROEPNAAM%]", $row->Roepnaam, $content);
	if (strlen($pw) > 5) {
		$content = str_replace("[%PASSWORD%]", $pw, $content);
	} else {
		$content = str_replace("[%PASSWORD%]", "Kan niet worden herverstrekt, wel gereset.", $content);
	}
	$content = str_replace("[%GEBLOKKEERD%]", $geblokt, $content);
	$content = str_replace("[%EXTRATEKST%]", $xtratekst, $content);
	$content = str_replace("[%NAAMVERENIGING%]", db_param("naamvereniging"), $content);
	$content = str_replace("[%NAAMWEBSITE%]", db_param("naamwebsite"), $content);
	$content = str_replace("[%URLWEBSITE%]", $_SERVER["HTTP_HOST"], $content);
	$content = str_replace("[%IPADDRESS%]", $_SERVER['REMOTE_ADDR'], $content);

	$mail->Body = $content;
	$mail->IsHTML(true);
	$mail->addstationary();
	if ($mail->Send()) {
		$mess = sprintf("Login '%s' aan %s (%s) verzonden.", $row->Login, $row->Naam, $mail->ListAddresses("to"));
	} else {
		$mess = sprintf("Fout tijdens het versturen van de mail: %s. Probeer later nogmaals of neem contact op met de webmaster.", $mail->ErrorInfo);
	}
	return $mess;
}

function fnHerstellenWachtwoord() {
	global $table_prefix;
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (strlen($_POST['login']) < 7 and !isValidMailAddress($_POST['email'], 0)) {
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
				$mess = fnPasswordReset($lidid);
			} else {
				$mess = "De ingevulde combinatie is niet gevonden. Het wachtwoord is niet hersteld.";
			}
			printf("<p class='mededeling'>%s</p>", htmlentities($mess));
		}
		echo("<p><a href='/'>Klik hier om verder te gaan.</a></p>\n");
	} else {
		echo("<div id='profielwijzigen'>\n");
		printf("<form name='HetstellenWachtwoord' action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
		echo("<fieldset>
		<h3>Herstellen wachtwoord</h3>
		<label>E-mailadres:</label><input type='text' name='email' size=50 maxlength=45>
		<label>Lidnummer:</label><input class='inputnumber' type='number' min=0 name='lidnummer' value=0>
		<label>Login:</label><input type='text' name='login' size=50 maxlength=15>
		<input type='submit' class='knop' value='Herstellen'>\n
		</fieldset>
		</form>
		<p>Minimaal 2 van de 3 velden moeten correct zijn ingevuld.</p>
		</div>  <!-- Einde profielwijzigen -->");
	}
} 

function fnPasswordReset($lidid) {
	
	$myFile = 'templates/Password_reset.html';
	if (file_exists($myFile)) {
		$content = file_get_contents($myFile);
	} else {
		$content = "<p>Beste [%ROEPNAAM%],</p>\n
<p>Je hebt een reset je wachtwoord voor <a href='http://[%URLWEBSITE%]'>[%NAAMWEBSITE%]</a> aangevraagd. Hierbij je login met je  nieuwe wachtwoord.</p>\n
<ul>\n
<li>Login: [%LOGIN%]</li>\n
<li>Wachtwoord: [%PASSWORD%]</li>\n
</ul>\n
<p>[%GEBLOKKEERD%]</p>
\n
<p>De letters i of o komen niet in een standaard wachtwoord voor.</p>
\n
<p>Met vriendelijke groeten,<br>\n
<strong>Webmaster [%NAAMWEBSITE%]</strong></p>\n";
	}
	
	$row = db_logins("rowlogin", "", "", $lidid)[0];
	
	$mail = new RBMmailer($row->Nummer);
	$mail->FromName = "Webmaster " . db_param("naamwebsite");
	$mail->Subject = "Nieuw wachtwoord";

	if (db_param("maxinlogpogingen") > 0 and $row->FouteLogin > db_param("maxinlogpogingen")) {
		$geblokt = "Er is teveel foutief met deze login proberen in te loggen. Hierdoor is dit account geblokkeerd. ";
		if (db_param("loginautounlock") > 0 and db_param("loginautounlock") <= 24*60) {
			$geblokt .= sprintf("Uiterlijk om %s uur is deze blokkade verwijderd.", date("G:i", strtotime($row->Gewijzigd)+(60*db_param("loginautounlock"))));
		} else {
			$geblokt .= sprintf("Vraag aan de <a href='mailto:%s'>webmaster</a> om deze vrij te geven.", db_param("emailwebmaster"));
		}
	} else {
		$geblokt = "";
	}
	
	$pw = newpassword($row->Nummer);
	if (strlen($pw) > 5) {
		$content = str_replace("[%NAAMLID%]", $row->Naam, $content);
		$content = str_replace("[%LOGIN%]", $row->Login, $content);
		$content = str_replace("[%ROEPNAAM%]", $row->Roepnaam, $content);
		$content = str_replace("[%PASSWORD%]", $pw, $content);
		$content = str_replace("[%GEBLOKKEERD%]", $geblokt, $content);
		$content = str_replace("[%NAAMVERENIGING%]", db_param("naamvereniging"), $content);
		$content = str_replace("[%NAAMWEBSITE%]", db_param("naamwebsite"), $content);
		$content = str_replace("[%URLWEBSITE%]", $_SERVER["HTTP_HOST"], $content);
		$content = str_replace("[%IPADDRESS%]", $_SERVER['REMOTE_ADDR'], $content);

		$mail->Body = $content;
		$mail->IsHTML(true);
		$mail->addstationary();
		if ($mail->Send()) {
			$mess = sprintf("Nieuw wachtwoord is aan %s (%s) verzonden.", $row->Naam, $mail->ListAddresses("to"));
		} else {
			$mess = sprintf("Fout tijdens het versturen van de het nieuwe wachtwoord: %s.<br> Probeer later nogmaals of neem contact op met de webmaster.", $mail->ErrorInfo);
		}
	} else {
		$mess = "Er ging iets mis, er is geen nieuw wachtwoord toegekend.";
	}
	db_logboek("add", $mess, 5, $row->Nummer);
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

function fnLoginAanvragen() {
	global $table_prefix, $lididwebmasters, $selectlidnr, $basisurl;
	
	$beperktotgroep = db_param("beperktotgroep");
	$lidnrnodigbijloginaanvraag = db_param("lidnrnodigbijloginaanvraag");
	$maxlengtelogin = db_param("maxlengtelogin");
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {

		$login = "";
		$mess = "";
		$lidid = 0;
		if (!isset($_POST['gewenstelogin'])) {
			$_POST['gewenstelogin'] = "";
		}
		if (!isset($_POST['lidnummer']) or strlen($_POST['lidnummer']) == 0) {
			$_POST['lidnummer'] = 0;
		}
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_login WHERE LidID=%d;", $table_prefix, $_POST['lidnummer']);
		if (db_scalar($query) > 0) {
			$mess = "Er is al een login aan dit lidnummer gekoppeld. Je kan er dus geen login op aanvragen.";
		} elseif (strlen($_POST['email']) == 0) {
			$mess = "Je hebt geen e-mailadres opgegeven.";
		} elseif (!isValidMailAddress($_POST['email'], 0)) {
			$mess = sprintf("Je hebt geen geldig e-mailadres (%s) opgegeven.", $_POST['email']);
		} elseif (strlen($_POST['gewenstelogin']) > 0 and $lidnrnodigbijloginaanvraag == 1 and $_POST['lidnummer'] == 0) {
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
					if (isset($row->Login) and strlen($row->Login) > 5 and ($lidnrnodigbijloginaanvraag == 0 or $row->Lidnr == $_POST['lidnummer'])) {
						$mess .= fnConfirmLogin($row) . " ";
						$lidid = $row->Nummer;
					} else {
						// E-mailadres is bekend, maar een aan dit e-mailadres gekoppelde login bestaat niet.
						if (db_param("mailing_lidnr") > 0 and $_POST['lidnummer'] == 0) {
							$mess .= fnConfirmLidnr($row) . " ";
							$lidid = $row->Nummer;
						} elseif ($row->AanvragenMag == 0 and in_array($row->Nummer, $lididwebmasters) == false) {
							if (is_array($beperktotgroep) and count($beperktotgroep) > 1) {
								$gt = "";
								foreach ($beperktotgroep as $gr) {
									if (strlen($gt) > 0) {$gt .= ", ";}
									$gt .= "'" . db_naam_onderdeel($gr) . "'";
								}
								$mess = sprintf("Je behoort niet tot één van de groepen %s van %s. Een login mag niet aangevraagd worden.", $gt, db_param("naamvereniging"));
							} else {
								if (is_array($beperktotgroep) and count($beperktotgroep) == 1) {
									$gr = $beperktotgroep[0];
								} elseif (is_numeric($beperktotgroep)) {
									$gr = $beperktotgroep;
								} else {
									$gr = 0;
								}
								$mess = sprintf("Je behoort niet tot groep '%s' van %s. Een login mag niet aangevraagd worden.", db_naam_onderdeel($gr), db_param("naamvereniging"));
							}
						} elseif (strlen($_POST['gewenstelogin']) == 0) {
							$mess = "Je hebt geen gewenste login opgegeven.";
						} elseif (strlen($_POST['gewenstelogin']) < 6) {
							$mess = "Je hebt geen geldige gewenste login opgegeven. Een login moet uit minimaal 6 karakters bestaan.";
						} elseif (strlen($_POST['gewenstelogin']) > $maxlengtelogin) {
							$mess = sprintf("Je hebt geen geldige login opgegeven. Een login mag uit maximaal %d karakters bestaan.", $maxlengtelogin);
						} elseif (strpos($_POST['gewenstelogin'], " ") > 0) {
							$mess = "Je hebt geen geldige login opgegeven. Er mag geen spatie in een login zitten.";
						} elseif (db_logins("controlebestaat", "", $_POST['gewenstelogin'], $row->Nummer) > 0) {
							$mess = sprintf("Login '%s' is al in gebruik.", $_POST['gewenstelogin']);
						} elseif ($lidnrnodigbijloginaanvraag == 0 or $row->Lidnr == $_POST['lidnummer']) {
							if (db_logins("add", "", $_POST['gewenstelogin'], $row->Nummer)) {
								$rl = db_logins("rowlogin", "", "", $row->Nummer);
								$lidid = $rl[0]->Nummer;
								$pw = newpassword($lidid);
								if (strlen($pw) > 5) {
									$mess = fnConfirmLogin($rl[0], "", $pw);
								} else {
									$mess = "Er is iets niet goed gegaan.";
								}
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
		db_logboek("add", $mess, 1, 0, 1, $lidid);
		printf("<p><a href='%s' target='_top'>Klik hier om verder te gaan.</a></p>\n", $basisurl);
		
	} else {

		echo("<div id='loginaanvraag'>\n");
		printf("<form name=Loginaanvraag action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
		echo("<fieldset>\n");
		echo("<h3>Aanvragen login</h3>\n");
		
		printf("<p><label>Gewenste login:</label><input type='text' name='gewenstelogin' maxlength=%d>\n", db_param("maxlengtelogin"));
		if (db_param("lidnrnodigbijloginaanvraag") == 1) {
			echo("<label>Lidnummer:</label><input class='inputnumber' type='number' name='lidnummer' min=0>\n");
		}
		echo("<label>E-mailadres:</label><input type='email' name='email'>\n");

		echo("<input class='knop' type='submit' name='loginaanvragen' value='Aanvragen'></p>\n");
		echo("</fieldset>\n");

		if (db_param("mailing_lidnr") > 0 and $lidnrnodigbijloginaanvraag == 1) {
			echo("<p>Door alleen je e-mailadres in te vullen word je lidnummer per e-mail verzonden.</p>\n");
		}
		echo("<p>");
		if ($lidnrnodigbijloginaanvraag == 1) {
			echo("Met de juiste combinatie van lidnummer en e-mailadres kan je een login aanvragen.");
		} else {
			echo("Met het, in de ledenadministratie bekende e-mailadres, kan je een login aanvragen.");
		}
		echo(" Je login en wachtwoord worden per e-mail naar je verzonden.</p>\n");
		
		printf("<p>Mocht je een login hebben, maar je wachtwoord niet meer weten, dan kan je dat <a href='%s?tp=Herstellen wachtwoord'>hier</a> herstellen.</p>", $_SERVER["PHP_SELF"]);
			
		echo("</form>
		</div>  <!-- Einde loginaanvraag -->\n");
	}
}

function cleanlogin($login) {

	$maxlengtelogin = db_param("maxlengtelogin");

	$login = trim($login);
	if (strlen($login) > $maxlengtelogin) {
		$login = substr($login, 0, $maxlengtelogin);
	}
	$login = str_replace(" ", "_", $login);
	$login = str_replace("'", "", $login);
	$login = str_replace("\"", "", $login);
	$login = str_replace(";", "", $login);
	
	return $login;
}

function newpassword($lidid) {
	global $lididwebmasters, $db_pass;
	
	if (in_array($lidid, $lididwebmasters)) {
		$ww = $db_pass;
		echo("<p>Het wachtwoord is gelijk aan het wachtwoord wat nodig is om in de database te kunnen inloggen. Graag dit direct wijzigen.</p>\n");
	} else {
		$alphabet = "abcdefghjklmnpqrstuwxyz@!%&#";
		$ww = "";
		for ($i = 0; $i < 10; $i++) {
			$n = rand(0, strlen($alphabet)-1);
			$ww .= substr($alphabet, $n, 1);
		}
		$ww = strtoupper(substr($ww, 0, 2)) . substr($ww, 2, 4). rand(100, 999);
	}
	
	if ($lidid <= 0) {
		return false;
	} elseif (db_change_password($ww, "", $lidid) == "ok") {
		return $ww;
	} else {
		return false;
	}
}

?>
