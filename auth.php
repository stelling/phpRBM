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
			setcookie("password", $_POST['password'], time()+(3600*24*30));
		}
		toegang("", 1, $_POST['password']);
		unset($_SESSION['toegang']);
		if ($_SESSION['lidid'] > 0) {
			printf("<script>\nlocation.href='%s';\n</script>\n", $_SERVER['HTTP_REFERER']);
		} else {
			printf("<p><a href='%s'>Klik hier om verder te gaan.</a></p>\n", $_SERVER['HTTP_REFERER']);
		}
	}
?>