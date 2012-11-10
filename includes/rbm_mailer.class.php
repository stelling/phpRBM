<?php

class RBMmailer extends PHPMailer {

	function __construct($lidid=-1) {
		global $table_prefix, $selectnaam;
	
		$smtphost = db_param("smtphost");
		if (strlen($smtphost) > 0) {
			$this->Host = $smtphost;
			if (db_param("smtpport") > 0) {
				$this->Port = db_param("smtpport");
			}
			$this->IsSMTP(true);
			$smtpuser = db_param("smtpuser");
			if (strlen($smtpuser) > 0) {
				$this->SMTPAuth = true;
				$this->Username = $smtpuser;
				$this->Password = db_param("smtppw");	
			} else {
				$this->SMTPAuth = false;
			}
		} else {
			$this->IsMail(true);
		}
		$this->IsHTML(true);
		$this->From = $_SESSION['emailwebmaster'];
		$this->FromName = $_SESSION['naamvereniging'];
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

}

?>
