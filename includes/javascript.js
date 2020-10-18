function fnFilter(p_table, p_filterelement, p_kolom, p_kolom2) {
	var input, filter, table, tr, td, td2, i, txtValue, txtValue2;
	
	input = document.getElementById(p_filterelement);
	filter = input.value.toUpperCase();
	table = document.getElementById(p_table);
	tr = table.getElementsByTagName("tr");
	
	if (p_kolom2 == undefined) {
		p_kolom2 = -1;
	}

	for (i = 0; i < tr.length; i++) {
		td = tr[i].getElementsByTagName("td")[p_kolom];
		if (p_kolom2 >= 0) {
			td2 = tr[i].getElementsByTagName("td")[p_kolom2];	
		}
		if (td) {
			txtValue = td.textContent || td.innerText;
			if (p_kolom2 > -1) {
				txtValue2 = td2.textContent || td2.innerText;
			} else { 
				txtValue2 = "";
			}
			if (txtValue.toUpperCase().indexOf(filter) > -1) {
				tr[i].style.display = "";
			} else if (p_kolom2 > -1 && txtValue2.toUpperCase().indexOf(filter) > -1) {
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

function fnFilterTwee(p_table, p_input1, p_input2, p_col1, p_col2) {
	var filter1, filter2, input1, input2, table, tr, td1, td2, i, txtValue1, txtValue2;
	
	input1 = document.getElementById(p_input1);
	filter1 = input1.value.toUpperCase();
	input2 = document.getElementById(p_input2);
	filter2 = input2.value.toUpperCase();
	
	table = document.getElementById(p_table);
	tr = table.getElementsByTagName("tr");

	for (i = 0; i < tr.length; i++) {
		td1 = tr[i].getElementsByTagName("td")[p_col1];
		td2 = tr[i].getElementsByTagName("td")[p_col2];
		if (td1) {
			txtValue1 = td1.textContent || td1.innerText;
			txtValue2 = td2.textContent || td2.innerText;
			if (filter1.length == 0 && funcfilter.length == 0) {
				tr[i].style.display = "";
			} else if (filter1.length > 0 && txtValue1.toUpperCase().indexOf(filter1) > -1) {
				tr[i].style.display = "";
			} else if (filter2.length > 0 && txtValue2.toUpperCase().indexOf(filter2) > -1) {
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

function CopyFunction() {
	let textarea = document.getElementById("copywijzigingen");
	textarea.select();
	document.execCommand('copy');
	alert("De wijzigingen zijn naar het klembord gekopieerd.");
}
