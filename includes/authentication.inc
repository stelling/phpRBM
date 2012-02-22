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
	global $lididwebmasters, $table_prefix, $urlwebsite, $gebruikopenid, $naamwebsite;

	if (isset($_SESSION['lidid']) and $_SESSION['lidid'] > 0 and isset($_SESSION['lastauthenication']) and $_SESSION['lastauthenication'] >= time()-(10*60)) {
		// Er hoeft niets te gebeuren.
	} else {
		$query = sprintf("UPDATE %sAdmin_login SET Ingelogd=0 WHERE Ingelogd=1 AND LastLogin < ADDDATE(SYSDATE(), INTERVAL -25 MINUTE);", $table_prefix);
		fnQuery($query);
		$_SESSION['lidid'] = 0;
		$_SESSION['lidnr'] = 0;
		$_SESSION['naamingelogde'] = "gast";
		$_SESSION['roepnaamingelogde'] = "gast";
		if (isset($_SESSION['username']) and strlen($_SESSION['username']) >= 6) {
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
				$_SESSION['naamingelogde'] = $row->Naam_lid;
				if (strlen($row->ndRoepnaam) == 0) {
					$_SESSION['roepnaamingelogde'] = $_SESSION['naamingelogde'];
				} else {
					$_SESSION['roepnaamingelogde'] = trim($row->ndRoepnaam);
				}
				$e = "E-mail";
				$_SESSION['emailingelogde'] = $row->$e;
				$query = sprintf("UPDATE %sAdmin_login SET LastLogin=SYSDATE(), Ingelogd=1 WHERE Login='%s';", $table_prefix, $_SESSION['username']);
				$result = fnQuery($query);
				if ($result > 0) {
					$mess = sprintf("%s is ingelogd met login %s. Zijn/haar LidID is %d.", $_SESSION['naamingelogde'], $_SESSION['username'], $_SESSION['lidid']);
					db_add_activiteit($mess, 1);
				}
			} else {
				if (isset($_SESSION['password']) and strlen($_SESSION['password']) > 1) {
					$mess = sprintf("Er is geprobleerd in te loggen met %s.", $_SESSION['username']);
					db_add_activiteit($mess, 1);
				}
				$_SESSION['username'] = "";
				$_SESSION['password'] = "";
				setcookie("username", "", time()-3600);
				setcookie("password", "", time()-3600);
				if (count(db_logins()) > 0) {
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
	
	if (in_array($soort, array('Verenigingsinfo', 'Login aanvragen', 'Beginpagina')) === TRUE) {
		return true;
	} elseif (strlen($soort) > 0) {
		$soort = str_replace("'", "", $soort);
		$query = sprintf("SELECT COUNT(*) FROM %sAdmin_access WHERE Tabpage=\"%s\";", $table_prefix, $soort);
		$result = fnQuery($query);
		if ($result->fetchColumn() == 0) {
			$query = sprintf("INSERT INTO %sAdmin_access SET Tabpage=\"%s\", Ingevoerd=CURDATE();", $table_prefix, $soort);
			fnQuery($query);
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
						printf("<script>alert('%s heeft geen toegang tot het onderdeel '%s' op %s.');</script>\n", $_SESSION['naamingelogde'], $soort, $naamwebsite);
					} else {
						printf("<script>alert('Je bent niet ingelogd en hierdoor hebt geen toegang tot '%s' op %s.');</script>\n", $soort, $naamwebsite);
					}
				}
				return false;
			}
		}
	} else {
		return true;
	}	
}

function fnConfirmLogin($row) {
	global $urlwebsite, $naamwebsite, $naamvereniging;
	
	$def_bevestiging = "<p>Beste [%ROEPNAAM%],</p>\n
<p>Je hebt een login voor de website <a href='http://[%URLWEBSITE%]'>[%NAAMWEBSITE%]</a> aangevraagd. Hierbij je gegevens.</p>\n
<ul>\n
<li>Login: [%LOGIN%]</li>\n
<li>Wachtwoord: [%PASSWORD%]</li>\n
</ul>\n
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
	
	$content = str_replace("[%NAAMLID%]", $row->Naam, $content);
	$content = str_replace("[%LOGIN%]", $row->Login, $content);
	$content = str_replace("[%LIDNR%]", $row->Lidnr, $content);
	$content = str_replace("[%ROEPNAAM%]", $row->Roepnaam, $content);
	$content = str_replace("[%PASSWORD%]", encript_decript($row->Wachtwoord, 0), $content);
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

	try {
		$mail->FromName = $from;
		if (strlen($row->EmailVereniging) > 5 and isValidMailAddress($row->EmailVereniging)) {
			$mail->AddAddress($row->EmailVereniging);
		} else {
			$mail->AddAddress($row->EMAIL);
		}
		$mail->Subject = $subj;
		$mail->MsgHTML($content);
		if ($mail->Send()) {
			$oms = sprintf("Login '%s' verzonden aan %s", $row->Login, $row->Naam);
			db_add_activiteit($oms, 5);
			return true;
		} else {
			printf("<p>Error while sending message: %s</p>\n", $mail->ErrorInfo);
			return false;
		}
	} catch (phpmailerException $e) {
		echo $e->errorMessage(); // Error messages from PHPMailer
	} catch (Exception $e) {
		echo $e->getMessage();
	}
}

function fnConfirmLidnr($row) {
	global $urlwebsite, $naamwebsite, $naamvereniging;
	
	$content = sprintf("<p>Beste %s,</p>\n
<p>Je hebt je lidnummer van %s opgevraagd. Je lidnummer is: %d. Met dit lidnummer kan je <a href='http://%s/index.php?tp=Login+aanvragen'>hier</a> een login aanvragen.</p>\n
\n
<p>Met vriendelijke groeten,<br>\n
<strong>Webmaster %s</strong></p>\n", $row->Roepnaam, $naamvereniging, $row->Lidnr, $urlwebsite, $naamwebsite);

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
			$email = $row->EMAIL;
		}
		$mail->AddAddress($email);
		$mail->Subject = $subj;
		$mail->MsgHTML($content);
		if ($mail->Send()) {
			$oms = sprintf("Lidnr %d verzonden aan %s (%s).", $row->Lidnr, $row->Naam, $email);
			db_add_activiteit($oms);
			printf("<p class='mededeling'>Lidnummer is naar %s verzonden.</p>\n", $email);
			return true;
		} else {
			printf("<p>Error while sending message: %s</p>\n", $mail->ErrorInfo);
			return false;
		}
	} catch (phpmailerException $e) {
		echo $e->errorMessage(); // Error messages from PHPMailer
	} catch (Exception $e) {
		echo $e->getMessage();
	}
}

function encript_decript($text, $encr = 1) {
	global $encript_key;
	$retval = "";

	if (strlen($encript_key) < 3) {
		exit("Invalid encript_key");
	} elseif (strlen($text) == 0) {
		exit("No text supplied");
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

?>
