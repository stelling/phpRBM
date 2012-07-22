<?php
	include('./includes/standaard.inc');       
	
	if (!isset($_GET['url']) or strlen($_GET['url']) == 0) {
		$_GET['url'] = "/";
	}
   
	if (isset($_GET['actie']) and $_GET['actie'] == "uitloggen" and isset($_SESSION['username'])) {
		db_logins("uitloggen", "", "", $_SESSION['lidid']);
		$_SESSION['username'] = "";
		$_SESSION['password'] = "";
		$_SESSION['lidgroepen'] = "(0)";
		setcookie("username", "", time()-3600);
		setcookie("password", "", time()-3600);
		echo("<script>\nlocation.href='/';\n</script>\n");
   } elseif (isset($_SESSION['username']) and strlen($_SESSION['username']) > 5 and strlen($_SESSION['password']) > 5) {
		header ("Location: " . $_GET['url']);
   } elseif ($_SERVER['REQUEST_METHOD'] == "POST") {
		$_SESSION['username'] = cleanlogin($_POST['username']);
		if (strlen($_POST['password']) > 12) {
			$_SESSION['password'] = substr($_POST['password'], 0, 12);
		} else {
			$_SESSION['password'] = $_POST['password'];
		}
      if (isset($_POST['cookie']) and $_POST['cookie'] == 1) {                                    
			setcookie("username", $_SESSION['username'], time()+(3600*24*30*$bewaartijdlogins));
			if (strlen($_SESSION['password']) > 5) {
				setcookie("password", $_SESSION['password'], time()+(3600*24*30*$bewaartijdlogins));
			}
      } else {
			setcookie("username", "", time()-3600);
			setcookie("password", "", time()-3600);
		}
		if (isset($_GET['url']) and strlen($_GET['url']) > 0) {
			$href = $_GET['url'];
		} else {
			$href = $_SERVER['HTTP_REFERER'];
		}
		printf("<script>\nlocation.href='%s';\n</script>\n", $href);
   } else {
		if (!isset($_SESSION['username']) and isset($_COOKIE['username'])) {
			$_SESSION['username'] = cleanlogin($_COOKIE['username']);
			if (strlen($_COOKIE['password']) > 12) {
				$_SESSION['password'] = substr($_COOKIE['password'], 0, 12);
			} elseif (strlen($_COOKIE['password']) > 5) {
				$_SESSION['password'] = $_COOKIE['password'];
			}
		}
		echo("<script>\nhistory.go(-1);\n</script>\n");
	}
?>