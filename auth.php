<?php
	include('./includes/standaard.inc');
	
	if (!isset($_GET['url']) or strlen($_GET['url']) == 0) {
		$_GET['url'] = "/";
	}
	
	$bewaartijdlogins = db_param("bewaartijdlogins");
   
	if (isset($_GET['actie']) and $_GET['actie'] == "uitloggen" and $_SESSION['lidid'] > 0) {
		db_logins("uitloggen", "", "", $_SESSION['lidid']);
		$_SESSION['username'] = "";
		$_SESSION['lidid'] = 0;
		$_SESSION['lidgroepen'] = "(0)";
		setcookie("username", "", time()-3600);
		setcookie("password", "", time()-3600);
		echo("<script>\nlocation.href='/';\n</script>\n");
   } elseif ($_SESSION['lidid'] > 0) {
		header ("Location: " . $_GET['url']);
   } elseif ($_SERVER['REQUEST_METHOD'] == "POST") {
		$_SESSION['username'] = cleanlogin($_POST['username']);
      if (isset($_POST['cookie']) and $_POST['cookie'] == 1) {                                    
			setcookie("username", $_SESSION['username'], time()+(3600*24*30*$bewaartijdlogins));
			setcookie("password", $_POST['password'], time()+(3600*24*30*$bewaartijdlogins));
      } else {
			setcookie("username", "", time()-3600);
			setcookie("password", "", time()-3600);
		}
		toegang("", 1, $_POST['password']);
		printf("<script>\nlocation.href='%s';\n</script>\n", $_SERVER['HTTP_REFERER']);
   } else {
		if (!isset($_SESSION['username']) and isset($_COOKIE['username'])) {
			$_SESSION['username'] = cleanlogin($_COOKIE['username']);
		}
		echo("<script>\nhistory.go(-1);\n</script>\n");
	}
?>