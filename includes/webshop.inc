<?php

function fnWebshop() {
	global $currenttab2, $table_prefix;
	
	fnDispMenu(2);

	if ($currenttab2 == "Artikelbeheer") {
		if ($_SERVER['REQUEST_METHOD'] == "POST") {
			if (isset($_POST['NieuwArtikel'])) {
				db_artikel("add");
			}
			$lijst = db_artikel("overzichtbeheer");
			foreach ($lijst as $row) {
				$set_clause = "";
				foreach ($row as $col => $val) {
					$pvn = sprintf("%s_%d", $col, $row->RecordID);
					if ($col == "Verkoopprijs" and !is_numeric($_POST[$pvn])) {
						$_POST[$pvn] = 0;
					} elseif ($col == "BeschikbaarTot") {
						$_POST[$pvn] = change_month_to_uk($_POST[$pvn]);
						if (strlen($_POST[$pvn]) > 0 and strtotime($_POST[$pvn]) !== false) {
							$_POST[$pvn] = date("Y-m-d", strtotime($_POST[$pvn]));
						} else {
							$_POST[$pvn] = "9999-12-31";
						}
					}
					if (isset($_POST[$pvn]) and $_POST[$pvn] != $val) {
						$set_clause .= sprintf("%s='%s', ", $col, $_POST[$pvn]);
					}
				}
				if (strlen($set_clause) > 0) {
					$set_clause .= sprintf("Gewijzigd=SYSDATE(), GewijzigdDoor=%d", $_SESSION['lidid']);
					$query = sprintf("UPDATE %sWS_Artikel SET %s WHERE RecordID=%d;", $table_prefix, $set_clause, $row->RecordID);
					fnQuery($query);
				}
			}
		}
		$lijst = db_artikel("overzichtbeheer");
		printf("<form method='post' action='%s?tp=%s'>\n", $_SERVER['PHP_SELF'], $_GET['tp']);
		echo("<table>\n");
		echo("<tr><th>RecordID</th><th>Code</th><th>Omschrijving</th><th>Verkoopprijs</th><th>Beschikbaar tot</th></tr>\n");
		foreach ($lijst as $row) {
			echo("<tr>\n");
			printf("<td class='number'>%d</td>\n", $row->RecordID);
			printf("<td><input type='text' name='Code_%d' value='%s' size=6 maxlength=6></td>\n", $row->RecordID, $row->Code);
			printf("<td><input type='text' name='Omschrijving_%d' value='%s' size=30 maxlength=30></td>\n", $row->RecordID, $row->Omschrijving);
			printf("<td><input type='text' name='Verkoopprijs_%d' value='%s' size=10 maxlength=8></td>\n", $row->RecordID, $row->Verkoopprijs);
			printf("<td><input type='date' name='BeschikbaarTot_%d' value='%s' size=20 maxlength=18></td>\n", $row->RecordID, $row->BeschikbaarTot);
			
			echo("</tr>\n");
		}
		echo("</table>\n");
		echo("<input type='submit' value='Artikelen bijwerken'>\n");
		echo("<button name='NieuwArtikel'>Nieuw artikel</button>\n");
		echo("</form>\n");
	}
}

class webshop {
	public $orderid;

	function __construct() {
		global $table_prefix;
	}

	public function lijst($filter="") {
	}
	
	public function edit() {
	}
	
	public function post() {
	}
	
	public function upload() {
	}
	
	public function delete() {
	}
	
	public function undelete() {
	}
	
}

?>