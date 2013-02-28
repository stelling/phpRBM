<?php
	include('./includes/standaard.inc');

	if (isset($_GET['actie']) and $_GET['actie'] == "uitloggen") {
		db_logins("uitloggen", "", "", $_SESSION['lidid']);
		session_destroy();
		setcookie("username", "", time()-3600);
		setcookie("password", "", time()-3600);
		echo("<script>\nlocation.href='/';\n</script>\n");
	} elseif ($_SERVER['REQUEST_METHOD'] == "POST") {
		if (strlen($_POST['password']) < 7) {
			$mess = "Om in te loggen is het invullen van een wachtwoord van minimaal 7 karakters vereist.";
			db_logboek("add", $mess, 1, 0, 2);
		} else {
			$_SESSION['username'] = cleanlogin($_POST['username']);
			if (isset($_POST['cookie']) and $_POST['cookie'] == 1) {                                    
				setcookie("username", $_SESSION['username'], time()+(3600*24*30));
				if (isset($_POST['password']) and strlen($_POST['password']) > 5) {
					setcookie("password", $_POST['password'], time()+(3600*24*30));
				}
			}
			unset($_SESSION['toegang']);
			toegang("", 1, $_POST['password']);
		}
		printf("<script>\nlocation.href='http://%s';\n</script>\n", $_SERVER["HTTP_HOST"]);
	}
?>