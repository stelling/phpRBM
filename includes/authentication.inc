<?php

// require_once("includes/class.openid.v3.php");

if (!isset($_SESSION['username']) or !isset($_SESSION['password'])) {
	if (isset($_COOKIE['username']) and strlen($_COOKIE['username']) >= 5) {
		$_SESSION['username'] = $_COOKIE['username'];
		$_SESSION['password'] = $_COOKIE['password'];
	}
	toegang("", 0);
}

function toegang($soort, $melding=1) {
	global $lididwebmasters, $table_prefix, $urlwebsite, $gebruikopenid, $naamwebsite, $maxinlogpogingen, $tabpages;

	if (isset($_SESSION['lidid']) and $_SESSION['lidid'] > 0 and isset($_SESSION['lastauthenication']) and $_SESSION['lastauthenication'] >= time()-60) {
		// Er hoeft niets te gebeuren.
	} else {
		db_logins("uitloggen");
		$_SESSION['lidid'] = 0;
		$_SESSION['lidnr'] = 0;
		$_SESSION['naamingelogde'] = "gast";
		$_SESSION['roepnaamingelogde'] = "gast";
		$_SESSION['emailingelogde'] = "";
		if (isset($_SESSION['username']) and strlen($_SESSION['username']) > 5 and strlen($_SESSION['password']) > 5) {
			$row = db_logins("controle");
			if (strlen($_SESSION['password']) == 0 and isset($row->LidID) and $row->LidID > 0 and (!isset($_SESSION['openid_ok']) or $_SESSION['openid_ok'] != true) and $gebruikopenid == 1) {
				$openid = new SimpleOpenID;
				$openid->SetIdentity($row->openid_identity);
				$openid->SetTrustRoot('http://' . $urlwebsite);
				$appr_url = sprintf("http://%s/auth.php?soort=%s", $urlwebsite, urlencode($soort));
				if ($openid->GetOpenIDServer()){
					$openid->SetApprovedURL($appr_url);      // Send Response from OpenID server to this script
					$openid->Redirect();     // This will redirect user to OpenID Server
				} else {
					$error = $openid->GetError();
					printf("<p class='mededeling'>OpenID fout: %s</p>\n", $error['description']);
				}
				exit;
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
					$_SESSION['emailingelogde'] = $row->EmailVereniging;
				} elseif (isValidMailAddress($row->Email, 0)) {
					$_SESSION['emailingelogde'] = $row->Email;
				} else {
					$_SESSION['emailingelogde'] = "";
				}
				
				db_logins("setingelogd", "", "", $_SESSION['lidid']);

			} else {
				if (isset($_SESSION['password']) and strlen($_SESSION['password']) > 1) {
					$mess = sprintf("Er is geprobeerd met login '%s' in te loggen.", $_SESSION['username']);
					db_logboek("add", $mess, 1);
				}
				$_SESSION['username'] = "";
				$_SESSION['password'] = "";
				setcookie("username", "", time()-3600);
				setcookie("password", "", time()-3600);
				if (count(db_logins("lijst")) > 0) {
					echo("<script>alert('Login en/of wachtwoord is/zijn niet correct of login is niet geldig! Je bent niet ingelogd.');</script>\n");
				}
			}
		} else {
			$_SESSION['username'] = "";
			$_SESSION['password'] = "";
		}
	}
	if (in_array($_SESSION['lidid'], $lididwebmasters)) {
		$_SESSION['webmaster'] = 1;
	} else {
		$_SESSION['webmaster'] = 0;
	}
	if ($_SESSION['lidid'] > 0 and (!isset($_SESSION['lidgroepen']) or $_SESSION['lidgroepen'] = "(0)")) {
		$query = sprintf("SELECT OnderdeelID FROM %sLidond AS LO WHERE LO.Vanaf <= CURDATE() AND ((LO.Opgezegd IS NULL) OR LO.Opgezegd >= CURDATE()) AND LO.Lid=%d;", $table_prefix, $_SESSION['lidid']);
		$result = fnQuery($query);
		$rows = $result->fetchAll(PDO::FETCH_COLUMN);
		if (count($rows) > 0) {
			$_SESSION['lidgroepen'] = "(0, " . implode(", ", $rows) . ")";
		} else {
			$_SESSION['lidgroepen'] = "(0)";
		}
	} elseif (!isset($_SESSION['lidgroepen'])) {
		$_SESSION['lidgroepen'] = "(0)";
	}
	
	if ($_SESSION['lidid'] > 0) {
		db_logins("updateactiviteit", "", "", $_SESSION['lidid']);
	}
	
	if (in_array($soort, array('Verenigingsinfo', 'Beginpagina')) === TRUE) {
		return true;
	} elseif (strlen($soort) > 0) {
		if ($_SESSION['lidid'] > 0) {
			$soort = str_replace("'", "", $soort);
			$query = sprintf("SELECT COUNT(*) FROM %sAdmin_access WHERE Tabpage=\"%s\";", $table_prefix, $soort);
			$result = db_scalar($query);
			if ($result == 0) {
				db_authorisation("add", 0, $soort);
			}
		}
		if ($_SESSION['webmaster'] == 1) {
			return true;
		} else {
			$query = sprintf("SELECT COUNT(*) FROM %sAdmin_access WHERE Tabpage=\"%s\" AND Toegang IN %s;", $table_prefix, $soort, $_SESSION['lidgroepen']);
			$result = fnQuery($query);
			if ($result->fetchColumn() > 0) {
				return true;
			} else {
				if ($melding == 1) {
					if ($_SESSION['lidid'] > 0) {
						$mess = sprintf("%s heeft geen toegang tot het onderdeel '%s' op %s.", $_SESSION['naamingelogde'], $soort, $naamwebsite);
					} else {
						$mess = sprintf("Je bent niet ingelogd en hierdoor hebt geen toegang tot '%s' op %s.", $soort, $naamwebsite);
					}
					db_logboek("add", $mess, 5);
					printf("<script>alert('%s');</script>\n", $mess);
				}
				return false;
			}
		}
	} else {
		return true;
	}	
}

function fnConfirmLogin($row, $xtratekst="") {
	global $urlwebsite, $naamwebsite, $naamvereniging, $maxinlogpogingen;
	
	db_logins("wachtwoordenvullen");
	
	$def_bevestiging = "<p>Beste [%ROEPNAAM%],</p>\n
<p>Je hebt een login voor de website <a href='http://[%URLWEBSITE%]'>[%NAAMWEBSITE%]</a> aangevraagd. Hierbij je gegevens.</p>\n
<ul>\n
<li>Login: [%LOGIN%]</li>\n
<li>Wachtwoord: [%PASSWORD%]</li>\n
</ul>\n
<p>[%GEBLOKKEERD%]</p>
\n
<p>Met vriendelijke groeten,<br>\n
<strong>Webmaster [%NAAMWEBSITE%]</strong></p>\n";

	$myFile = 'templates/bevestiging_login.html';
	if (file_exists($myFile)) {
		$content = file_get_contents($myFile);
		if ($content == false) {
			$content = $def_bevestiging;
		}
	} else {
		$content = $def_bevestiging;
	}
	$myFile = 'templates/briefpapier.html';
	if (file_exists($myFile)) {
		$briefpapier = file_get_contents($myFile);
		if ($content == false) {
			$briefpapier = "[%MESSAGE%]";
		}
	} else {
		$briefpapier = "[%MESSAGE%]";
	}
	if (isset($maxinlogpogingen) and $maxinlogpogingen > 0 and $row->FouteLogin > $maxinlogpogingen) {
		$geblokt = sprintf("Er is %d keer geprobeerd met deze login in te loggen. Hierdoor is dit account geblokkeerd. Vraag aan de webmaster om deze weer vrij te geven.", $row->FouteLogin);
	} else {
		$geblokt = "";
	}
	
	$content = str_replace("[%NAAMLID%]", $row->Naam, $content);
	$content = str_replace("[%LOGIN%]", $row->Login, $content);
	$content = str_replace("[%LIDNR%]", $row->Lidnr, $content);
	$content = str_replace("[%ROEPNAAM%]", $row->Roepnaam, $content);
	$content = str_replace("[%PASSWORD%]", encript_decript($row->Wachtwoord, 0), $content);
	$content = str_replace("[%GEBLOKKEERD%]", $geblokt, $content);
	$content = str_replace("[%EXTRATEKST%]", $xtratekst, $content);
	$content = str_replace("[%NAAMVERENIGING%]", $naamvereniging, $content);
	$content = str_replace("[%NAAMWEBSITE%]", $naamwebsite, $content);
	$content = str_replace("[%URLWEBSITE%]", $urlwebsite, $content);
		
	$content = str_ireplace("[%MESSAGE%]", $content, $briefpapier);

	$subj = sprintf("Bevestiging login %s voor %s", $naamwebsite, $row->Naam);
	$from = "Webmaster " . $naamwebsite;
	
	$content = str_ireplace("[%FROM%]", $from, $content);
	$content = str_ireplace("[%TO%]", $row->Naam, $content);
	$content = str_ireplace("[%SUBJECT%]", $subj, $content);

	$mail = new RBMmailer();

	$mail->FromName = $from;
	if (isValidMailAddress($row->EmailVereniging, 0)) {
		$email = $row->EmailVereniging;
	} else {
		$email = $row->Email;
	}
	$mail->AddAddress($email);
	$mail->Subject = $subj;
	$mail->MsgHTML($content);
	if ($mail->Send()) {
		return sprintf("Login '%s' aan %s (%s) verzonden.", $row->Login, $row->Naam, $email);
	} else {
		return sprintf("Fout tijdens het versturen van de mail: %s. Probeer later nogmaals of neem contact op met de webmaster.", $mail->ErrorInfo);
	}
}

function fnConfirmLidnr($row) {
	global $urlwebsite, $naamwebsite, $naamvereniging;
	
	$content = sprintf("<p>Beste %s,</p>\n
<p>Je lidnummer is: %d.</p>
<p>Met dit lidnummer kan je <a href='http://%s/index.php?tp=Login+aanvragen'>hier</a> een login aanvragen.</p>\n
\n
<p>Met vriendelijke groeten,<br>\n
<strong>Webmaster %s</strong></p>\n", $row->Roepnaam, $row->Lidnr, $urlwebsite, $naamwebsite);

	$myFile = 'templates/briefpapier.html';
	if (file_exists($myFile)) {
		$briefpapier = file_get_contents($myFile);
		if ($content == false) {
			$briefpapier = "[%MESSAGE%]";
		}
	} else {
		$briefpapier = "[%MESSAGE%]";
	}
		
	$content = str_ireplace("[%MESSAGE%]", $content, $briefpapier);

	$subj = sprintf("Lidnummer %s", $naamvereniging);
	$from = "Webmaster " . $naamwebsite;
	
	$content = str_ireplace("[%FROM%]", $from, $content);
	$content = str_ireplace("[%TO%]", $row->Naam, $content);
	$content = str_ireplace("[%SUBJECT%]", $subj, $content);

	$mail = new RBMmailer();

	try {
		$mail->FromName = $from;
		if (strlen($row->EmailVereniging) > 5 and isValidMailAddress($row->EmailVereniging)) {
			$email = $row->EmailVereniging;
		} else {
			$email = $row->Email;
		}
		$mail->AddAddress($email);
		$mail->Subject = $subj;
		$mail->MsgHTML($content);
		if ($mail->Send()) {
			return sprintf("Lidnummer %d is aan %s (%s) verzonden.", $row->Lidnr, $row->Naam, $email);
		} else {
			return sprintf("Fout bij het versturen van de e-mail: %s. Probeer het later nogmaals of neem contact op met de webmaster.", $mail->ErrorInfo);
		}
	} catch (phpmailerException $e) {
		return $e->errorMessage(); // Error messages from PHPMailer
	} catch (Exception $e) {
		return $e->getMessage();
	}
}

function fnLoginAanvragen() {
	global $naamvereniging, $urlwebsite, $beperktotgroep, $table_prefix, $lidnrversturenmogelijk, $lidnrnodigbijloginaanvraag;
	global $lididwebmasters, $maxlengtelogin, $selectlidnr;

	if (!isset($_POST['lidnummer']) or strlen($_POST['lidnummer']) == 0) {
		$_POST['lidnummer'] = 0;
	}
	
	if ($_SERVER['REQUEST_METHOD'] == "POST") {

		$login = "";
		$mess = "";
		if (strlen($_POST['email']) == 0) {
			$mess = "Je hebt geen e-mailadres opgegeven.";
		} elseif (!isValidMailAddress($_POST['email'], 0)) {
			$mess = sprintf("Je hebt geen geldig e-mailadres (%s) opgegeven.", $_POST['email']);
		} elseif ($lidnrversturenmogelijk == 0 and $lidnrnodigbijloginaanvraag == 1 and $_POST['lidnummer'] == 0) {
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
					} else {
						// E-mailadres is bekend, maar er is geen login aangemaakt
						if ($lidnrversturenmogelijk == 1 and $_POST['lidnummer'] == 0) {
							$mess .= fnConfirmLidnr($row) . " ";
						} elseif ($row->AanvragenMag == 0 and in_array($row->Nummer, $lididwebmasters) == false) {
							if (is_array($beperktotgroep) and count($beperktotgroep) > 1) {
								$gt = "";
								foreach ($beperktotgroep as $gr) {
									if (strlen($gt) > 0) {$gt .= ", ";}
									$gt .= "'" . db_naam_onderdeel($gr) . "'";
								}
								$mess = sprintf("Je behoort niet tot één van de groepen %s van %s. Een login mag niet aangevraagd worden.", $gt, $naamvereniging);
							} else {
								if (is_array($beperktotgroep) and count($beperktotgroep) == 1) {
									$gr = $beperktotgroep[0];
								} else {
									$gr = $beperktotgroep;
								}
								$mess = sprintf("Je behoort niet tot groep '%s' van %s. Een login mag niet aangevraagd worden.", db_naam_onderdeel($gr), $naamvereniging);
							}
						} elseif (strlen($_POST['gewenstelogin']) < 6 or strlen($_POST['gewenstelogin']) > $maxlengtelogin) {
							$mess = sprintf("Je hebt geen geldige login opgegeven. Een login moet uit minimaal 6 en maximaal %d karakters bestaan.", $maxlengtelogin);
						} elseif (strpos($_POST['gewenstelogin'], " ") > 0) {
							$mess = "Je hebt geen geldige login opgegeven. Er mag geen spatie in een login zitten.";
						} elseif (db_logins("controlebestaat", "", $_POST['gewenstelogin'], $row->Nummer) > 0) {
							$mess = sprintf("Login '%s' is al in gebruik.", $_POST['gewenstelogin']);
						} elseif ($lidnrnodigbijloginaanvraag == 0 or $row->Lidnr == $_POST['lidnummer']) {
							if (db_logins("add", "", $_POST['gewenstelogin'], $row->Nummer)) {
								$rl = db_logins("rowlogin", "", "", $row->Nummer);
								$mess = fnConfirmLogin($rl[0]);
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
		if (strlen($mess) > 0) {
			db_logboek("add", $mess, 1);
			printf("<p class='mededeling'>%s</p>\n", $mess);
		}
		printf("<p><a href='http://%s' target='_top'>Klik hier om verder te gaan.</a></p>\n", $urlwebsite);
		
	} else {

		echo("<div id='loginaanvraag'>\n");
		printf("<form name=Loginaanvraag action='%s?%s' method='post'>\n", $_SERVER["PHP_SELF"], $_SERVER["QUERY_STRING"]);
		echo("<fieldset>\n");
		echo("<h3>Aanvragen login</h3>\n");
		
		echo("<p><label for='gewenstelogin'>Gewenste login:</label>\n
				<input type='text' name='gewenstelogin' id='gewenstelogin'>\n");
				
		if ($lidnrnodigbijloginaanvraag == 1) {
			echo("<label for='lidnummer'>Lidnummer:</label>\n
					<input type='number' name='lidnummer' id='lidnummer'>\n");
		}
				
		echo("<label for='email'>E-mailadres:</label>\n
				<input type='email' name='email' id='email'>\n");
				
		echo("<input class='knop' type='submit' name='loginaanvragen' value='Aanvragen'></p>\n");
		echo("</fieldset>\n");
		
		if ($lidnrversturenmogelijk == 1) {
			echo("<p>Door alleen je e-mailadres in te vullen wordt je lidnummer per e-mail verzonden.\n");
		} else {
			echo("<p>");
		}
		if ($lidnrnodigbijloginaanvraag == 1) {
			echo("Met de juiste combinatie van lidnummer en e-mailadres kan je een login aanvragen.</p>\n");
		} else {
			echo("Je kan een login aanvragen op je in de ledenadministratie bekende e-mailadres.</p>\n");
		}
			
		echo("</form>
		</div>  <!-- Einde loginaanvraag -->\n");
	}
}

function encript_decript($text, $encr=1) {
	global $encript_key;
	$retval = "";

	if (strlen($encript_key) < 3) {
		exit("Invalid encript_key");
	} elseif (strlen($text) == 0) {
		$retval = "";
	} else {
		while (strlen($encript_key) < strlen($text)) {
			$encript_key .= $encript_key;
		}
		for ($i=0; $i<strlen($text); $i++) {
			if ($encr == 1) {
				$retval .= chr(ord(substr($text, $i, 1)) + ord(substr($encript_key, $i, 1)));
			} else {
				$retval .= chr(ord(substr($text, $i, 1)) - ord(substr($encript_key, $i, 1)));
			}
		}
	}
	return $retval;
}

function cleanlogin($login) {
	global $maxlengtelogin;

	$login = trim($login);
	if (strlen($login) > $maxlengtelogin) {
		$login = substr($login, 0, $maxlengtelogin);
	}
	$login = str_replace(" ", "_", trim($login));
	$login = str_replace("'", "", trim($login));
	$login = str_replace("\"", "", trim($login));
	$login = str_replace(";", "", trim($login));
	
	return $login;
}
?>
