function fnFilter() {
	var input, filter, table, tr, td, i, txtValue;
	input = document.getElementById("tbNaamFilter");
	filter = input.value.toUpperCase();
	table = document.getElementById("ledenlijst");
	tr = table.getElementsByTagName("tr");

	for (i = 0; i < tr.length; i++) {
		td = tr[i].getElementsByTagName("td")[1];
		if (td) {
			txtValue = td.textContent || td.innerText;
			if (txtValue.toUpperCase().indexOf(filter) > -1) {
				tr[i].style.display = "";
			} else {
				tr[i].style.display = "none";
			}
		}
	}
}

function fnFilterAfdelingslijst() {
	var naaminput, naamfilter, funcinput, funcfilter, table, tr, td1, td2, i, txtValue1, txtValue2;
	
	naaminput = document.getElementById("tbNaamFilter");
	naamfilter = naaminput.value.toUpperCase();
	funcinput = document.getElementById("tbFuncFilter");
	funcfilter = funcinput.value.toUpperCase();
	
	table = document.getElementById("afdelingslijst");
	tr = table.getElementsByTagName("tr");

	for (i = 0; i < tr.length; i++) {
		td1 = tr[i].getElementsByTagName("td")[1];
		td2 = tr[i].getElementsByTagName("td")[5];
		if (td1) {
			txtValue1 = td1.textContent || td1.innerText;
			txtValue2 = td2.textContent || td2.innerText;
			if (naamfilter.length == 0 && funcfilter.length == 0) {
				tr[i].style.display = "";
			} else if (naamfilter.length > 0 && txtValue1.toUpperCase().indexOf(naamfilter) > -1) {
				tr[i].style.display = "";
			} else if (funcfilter.length > 0 && txtValue2.toUpperCase().indexOf(funcfilter) > -1) {
				tr[i].style.display = "";
			} else {
				tr[i].style.display = "none";
			}
		}
	}
}

function fnFilterDiplomaLid() {
	var input, filter, table, tr, td1, td2, i, txtValue;
	input = document.getElementById("tbFilterCodeNaam");
	filter = input.value.toUpperCase();
	table = document.getElementById("diplomaslidmuteren");
	tr = table.getElementsByTagName("tr");

	for (i = 0; i < tr.length; i++) {
		td1 = tr[i].getElementsByTagName("td")[0];
		td2 = tr[i].getElementsByTagName("td")[1];
		if (td1) {
			txtValue1 = td1.textContent || td1.innerText;
			txtValue2 = td2.textContent || td2.innerText;
			if (txtValue1.toUpperCase().indexOf(filter) > -1 || txtValue2.toUpperCase().indexOf(filter) > -1) {
				tr[i].style.display = "";
			} else {
				tr[i].style.display = "none";
			}
		}
	}
}


