<?php
	include('./includes/standaard.inc');       
	
	if (!isset($_GET['url']) or strlen($_GET['url']) == 0) {
		$_GET['url'] = "/";
	}
   
	if(isset($_GET['openid_mode']) and $_GET['openid_mode'] == 'id_res' and $gebruikopenid == 1){     // Perform HTTP Request to OpenID server to validate key
		$openid = new SimpleOpenID;
		$openid->SetIdentity($_GET['openid_identity']);
		$openid_validation_result = $openid->ValidateWithServer();
		if ($openid_validation_result == true){         // OK HERE KEY IS VALID
			$_SESSION['username'] = $_GET['openid_identity'];
			$_SESSION['openid_ok'] = true;
			toegang($_GET['soort'], 0);
			printf("<script>\nlocation.href='%s?tp=%s';\n</script>\n", $_SERVER['PHP_SELF'], $_GET['soort']);
		} elseif ($openid->IsError() == true){            // ON THE WAY, WE GOT SOME ERROR
			$error = $openid->GetError();
			printf("<p class='mededeling'>OpenID: ERROR DESCRIPTION: %s</p>\n", $error['description']);
			$_SESSION['username'] = "";
			$_SESSION['openid_ok'] = false;
		} else {                                            // Signature Verification Failed
			echo("<p class='mededeling'>OpenID: INVALID AUTHORIZATION</p>\n");
			$_SESSION['username'] = "";
			$_SESSION['openid_ok'] = false;
		}
	} elseif (isset($_GET['actie']) and $_GET['actie'] == "uitloggen" and isset($_SESSION['username'])) {
		db_uitloggen();
		session_unset();
		setcookie("username", "", time()-3600);
		setcookie("password", "", time()-3600);
		echo("<script>\nlocation.href='/';\n</script>\n");
   } elseif (isset($_SESSION['username']) and strlen($_SESSION['username']) > 5 and strlen($_SESSION['password']) > 5) {
		header ("Location: " . $_GET['url']);
   } elseif ($_SERVER['REQUEST_METHOD'] == "POST") {
		$_SESSION['username'] = $_POST['username'];
		$_SESSION['password'] = $_POST['password'];
      if (isset($_POST['cookie']) and $_POST['cookie'] == 1) {                                    
			setcookie("username", $_SESSION['username'], time()+(3600*24*30*$bewaartijdlogins));
			setcookie("password", $_SESSION['password'], time()+(3600*24*30*$bewaartijdlogins));
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
			$_SESSION['username'] = $_COOKIE['username'];
			$_SESSION['password'] = $_COOKIE['password'];
		}
		echo("<script>\nhistory.go(-1);\n</script>\n");
	}
?>