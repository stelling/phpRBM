<?php
	include('./includes/standaard.inc');

	if ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (strlen($_POST['password']) < 6) {
			$mess = "Om in te loggen is het invullen van een wachtwoord van minimaal 6 karakters vereist.";
			db_logboek("add", $mess, 1, 0, 2);
		} else {
			$_SESSION['username'] = cleanlogin($_POST['username']);
			if (isset($_POST['cookie']) and $_POST['cookie'] == 1) {                                    
				setcookie("username", $_SESSION['username'], time()+(3600*24*30));
				if (isset($_POST['password']) and strlen($_POST['password']) > 6) {
					setcookie("password", $_POST['password'], time()+(3600*24*30));
				}
			}
			unset($_SESSION['toegang']);
			toegang("", 1, $_POST['password']);
		}
		printf("<script>\nlocation.href='%s';\n</script>\n", $basisurl);
	}
?>