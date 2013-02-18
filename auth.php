<?php
	include('./includes/standaard.inc');

	if (isset($_GET['actie']) and $_GET['actie'] == "uitloggen") {
		db_logins("uitloggen", "", "", $_SESSION['lidid']);
		session_destroy();
		setcookie("username", "", time()-3600);
		setcookie("password", "", time()-3600);
		echo("<script>\nlocation.href='/';\n</script>\n");
   } elseif ($_SERVER['REQUEST_METHOD'] == "POST") {
		$_SESSION['username'] = cleanlogin($_POST['username']);
      if (isset($_POST['cookie']) and $_POST['cookie'] == 1) {                                    
			setcookie("username", $_SESSION['username'], time()+(3600*24*30));
			if (isset($_POST['password']) and strlen($_POST['password']) > 5) {
				setcookie("password", $_POST['password'], time()+(3600*24*30));
			}
		}
		unset($_SESSION['toegang']);
		toegang("", 1, $_POST['password']);
		if ($_SESSION['lidid'] > 0) {
			printf("<script>\nlocation.href='http://%s';\n</script>\n", $_SERVER["HTTP_HOST"]);
		} else {
			printf("<p><a href='%s'>Klik hier om verder te gaan.</a></p>\n", $_SERVER['HTTP_REFERER']);
		}
	}
?>