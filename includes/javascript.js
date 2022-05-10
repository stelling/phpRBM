function fnFilter(p_table, p_filterelement, p_kolom, p_kolom2, p_kolom3, p_kolom4) {
	var input, filter, table, tr, td, td2, td3, td4, i, txtValue, txtValue2, txtValue3, txtValue4;
	
	input = document.getElementById(p_filterelement);
	filter = input.value.toUpperCase();
	table = document.getElementById(p_table);
	tr = table.getElementsByTagName("tr");
	
	if (filter.length > 2) {
	
		if (p_kolom2 == undefined) {
			p_kolom2 = -1;
		}
		if (p_kolom3 == undefined) {
			p_kolom3 = -1;
		}
		if (p_kolom4 == undefined) {
			p_kolom4 = -1;
		}

		for (i = 0; i < tr.length; i++) {
			td = tr[i].getElementsByTagName("td")[p_kolom];
			if (p_kolom2 >= 0) {
				td2 = tr[i].getElementsByTagName("td")[p_kolom2];	
			}
			if (p_kolom3 >= 0) {
				td3 = tr[i].getElementsByTagName("td")[p_kolom3];	
			}
			if (p_kolom4 >= 0) {
				td4 = tr[i].getElementsByTagName("td")[p_kolom4];	
			}
			if (td) {
				txtValue = td.textContent || td.innerText;
				if (p_kolom2 > -1) {
					txtValue2 = td2.textContent || td2.innerText;
				} else { 
					txtValue2 = "";
				}
				if (p_kolom3 > -1) {
					txtValue3 = td3.textContent || td3.innerText;
				} else { 
					txtValue3 = "";
				}
				if (p_kolom4 > -1) {
					txtValue4 = td4.textContent || td4.innerText;
				} else { 
					txtValue4 = "";
				}
				if (txtValue.toUpperCase().indexOf(filter) > -1) {
					tr[i].style.display = "";
				} else if (p_kolom2 > -1 && txtValue2.toUpperCase().indexOf(filter) > -1) {
					tr[i].style.display = "";
				} else if (p_kolom3 > -1 && txtValue3.toUpperCase().indexOf(filter) > -1) {
					tr[i].style.display = "";
				} else if (p_kolom4 > -1 && txtValue4.toUpperCase().indexOf(filter) > -1) {
					tr[i].style.display = "";
				} else {
					tr[i].style.display = "none";
				}
			}
		}
	} else {
		for (i = 0; i < tr.length; i++) {
			tr[i].style.display = "";
		}
	}
}

function fnFilterTwee(p_table, p_input1, p_input2, p_col1, p_col2, p_input3, p_col3) {
	var filter1, filter2, filter3, input1, input2, input3, table, tr, td1, td2, td3, i, txtValue1, txtValue2, txtValue3;
	
	if (p_col3 == undefined) {
		p_col3 = -1;
	}

	input1 = document.getElementById(p_input1);
	filter1 = input1.value.toUpperCase();
	input2 = document.getElementById(p_input2);
	filter2 = input2.value.toUpperCase();
	if (p_col3 >= 0) {
		input3 = document.getElementById(p_input3);
		filter3 = input3.value.toUpperCase();
	} else {
		filter3 = "";
	}
	
	table = document.getElementById(p_table);
	tr = table.getElementsByTagName("tr");

	for (i = 0; i < tr.length; i++) {
		td1 = tr[i].getElementsByTagName("td")[p_col1];
		td2 = tr[i].getElementsByTagName("td")[p_col2];
		td3 = tr[i].getElementsByTagName("td")[p_col3];
		if (td1) {
			txtValue1 = td1.textContent || td1.innerText;
			txtValue2 = td2.textContent || td2.innerText;
			if (filter3.length > 0) {
				txtValue3 = td3.textContent || td3.innerText;
			} else {
				txtValue3 = ""
			}
			if (filter1.length == 0 && filter2.length == 0 && filter3.length == 0) {
				tr[i].style.display = "";
			} else if (filter1.length > 0 && txtValue1.toUpperCase().indexOf(filter1) > -1) {
				tr[i].style.display = "";
			} else if (filter2.length > 0 && txtValue2.toUpperCase().indexOf(filter2) > -1) {
				tr[i].style.display = "";
			} else if (filter3.length > 0 && txtValue3.toUpperCase().indexOf(filter3) > -1) {
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

function savedata(entity, id, value, field_name='') {

  // Get edit id, field name and value
	if (field_name == "") {
		var split_id = id.split('_');
		var field_name = split_id[0];
		var edit_id = split_id[1];
	} else {
		var edit_id = id;
	}

	// Sending AJAX request
	$.ajax({
		url: 'ajax_update.php?entiteit=' + entity,
		type: 'post',
		data: { field:field_name, value:value, id:edit_id },
		success:function(response){}
	});
}
