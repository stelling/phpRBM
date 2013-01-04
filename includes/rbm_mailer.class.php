<?php

class RBMmailer extends PHPMailer {

	private $bestand_briefpapier = "templates/briefpapier.html";

	function __construct($lidid=-1) {
		global $table_prefix, $selectnaam;
		global $smtphost, $smtpport, $smtpuser, $smtppw;
	
		if (strlen($smtphost) > 0) {
			$this->Host = $smtphost;
			if ($smtpport > 0) {
				$this->Port = $smtpport;
			}
			$this->IsSMTP(true);
			if (strlen($smtpuser) > 0) {
				$this->SMTPAuth = true;
				$this->Username = $smtpuser;
				$this->Password = $smtppw;
			} else {
				$this->SMTPAuth = false;
			}
		} else {
			$this->IsMail(true);
		}
		$this->IsHTML(true);
		$this->From = db_param("emailwebmaster");
		$this->FromName = db_param("naamvereniging");
		$this->WordWrap = 110;
		
		if ($lidid > 0) {
			$this->ToevoegenLid($lidid);
		}
	}
	
	public function ToevoegenLid($lidid) {
	
		$rv = "";
		$row = db_lid("record", "", $lidid);
		if (strlen($row->Email) > 5 and self::ValidateAddress($row->Email)) {
			if (in_array($row->Email, $this->to) == false) {
				$this->AddAddress($row->Email, $row->NaamLid);
			}
			$rv = $row->Email;
		} elseif (strlen($row->EmailVereniging) > 5 and self::ValidateAddress($row->EmailVereniging)) {
			if (in_array($row->EmailVereniging, $this->to) == false) {
				$this->AddAddress($row->EmailVereniging, $row->NaamLid);
			}
			$rv = $row->EmailVereniging;
		}
		if (strlen($rv) > 0) {
			$this->InformerenOuders($row->GEBDATUM, $row->EmailOuders, "cc");
		} else {
			$rv = $this->InformerenOuders($row->GEBDATUM, $row->EmailOuders, "to");
		}
		
		return $rv;
	}
	
	public function InformerenOuders($geboren, $emailouders, $kind="cc") {
		if ($kind != "to" and count($this->to) == 0) {
			$kind = "to";
		}
		$rv = "";
		
		if ($geboren > date("Y-m-d", mktime(0, 0, 0, date("m"), date("d"), date("Y")-18)) and strlen($emailouders) > 5) {
			$emailouders = str_replace(" ", "", $emailouders);
			$emailouders = str_replace(";", ",", $emailouders);
			foreach(explode(",", $emailouders) as $e) {
				if (self::ValidateAddress($e) and in_array($e, $this->to) == false and in_array($e, $this->cc) == false) {
					$this->AddAnAddress($kind, $e);
					if (strlen($rv) > 0) {
						$rv .= ", ";
					}
					$rv .= $e;
				}
			}
		}
		return $rv;
	}
	
	public function ListAddresses($kind="to") {
		$rv = "";
		foreach($this->$kind as $e) {
			if (strlen($rv) > 0) {
				$rv .= ", ";
			}
			$rv .= $e[0];
		}
		return strtolower($rv);
	}
	
	public function hasstationary() {
		if (file_exists($this->bestand_briefpapier)) {
			return true;
		} else {
			return false;
		}
	}
	
	public function addstationary($to="", $from="") {
		if (strlen($to) == 0) {
			foreach($this->to as $e) {
				if (strlen($to) > 0) {
					$to .= ", ";
				}
				$to .= $e[1];
			}
		}
		if (strlen($from) == 0) {
			$from = $this->FromName;
		}
		if (file_exists($this->bestand_briefpapier)) {
			$htmlmessage = str_ireplace("[%MESSAGE%]", $this->Body, file_get_contents($this->bestand_briefpapier));
			$htmlmessage = str_ireplace("[%FROM%]", $from, $htmlmessage);
			$htmlmessage = str_ireplace("[%TO%]", $to, $htmlmessage);
			$htmlmessage = str_ireplace("[%SUBJECT%]", $this->Subject, $htmlmessage);
			$this->Body = $htmlmessage;
		} else {
			$mess = sprintf("Het bestand '%s' bestaat niet.", $this->bestand_briefpapier);
			db_logboek("add", $mess, 4);
		}
	}
	
	public function Send() {
		if (db_param("maxmailsperminuut") > 0 and isset($_SESSION['lastmailsend'])) {
			while ((microtime(true)-$_SESSION['lastmailsend']) < ((60 / db_param("maxmailsperminuut")) * 1.0005)) {
				usleep((60 / db_param("maxmailsperminuut")) * 1001000);
				set_time_limit(60);
			}
		}
		$_SESSION['lastmailsend'] = microtime(true);
	
		try {
			if(!$this->PreSend()) return false;
			return $this->PostSend();
		} catch (phpmailerException $e) {
			$this->SentMIMEMessage = '';
			$this->SetError($e->getMessage());
			if ($this->exceptions) {
				throw $e;
			}
			return false;
		}
	}

}

?>
