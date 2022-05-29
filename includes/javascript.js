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
	var td1, td2, i, txtValue;
	var input = document.getElementById("tbFilterCodeNaam");
	var filter = input.value.toUpperCase();
	var table = document.getElementById("diplomaslidmuteren");
	var tr = table.getElementsByTagName("tr");

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

function savedata(entity, rid, control) {

	id = control.id;
	if (id.indexOf("_") !== -1 && id !== "cc_addr") {
		var split_id = id.split('_');
		var field_name = split_id[0];
		var rid = split_id[1];
	} else {
		var field_name = control.id;
	}
	var value = control.value;

	$.ajax({
		url: 'ajax_update.php?entiteit=' + entity,
		type: 'post',
		dataType: 'json',
		data: { field:field_name, value:value, id:rid },
		success:function(response){}
	});
}

function savecb(entity, rid, control) {
	
	if (control.checked == true) {
		value = 1;
	} else {
		value = 0;
	}

	$.ajax({
		url: 'ajax_update.php?entiteit=' + entity,
		type: 'post',
		dataType: 'json',
		data: { field:control.id, value:value, id:rid },
		success:function(response){}
	});
}

/* Mailing specifiek */
function mailingprops() {
	var mid = $('#recordid').text();
	
	$.ajax({
		url: 'ajax_update.php?entiteit=mailingcontrole',
		type: 'post',
		dataType: 'json',
		data: { mailingid: mid },
		success: function(response) {
			$('#meldingen').html(response['meldingen']);
			if (response['verzendenmag'] == true) {
				document.getElementById('btnverstuurmailing').disabled = false;
				document.getElementById('btnverstuurmailing').style.color = "";
				
				document.getElementById('btnbekijkvoorbeeld').disabled = false;
				document.getElementById('btnbekijkvoorbeeld').style.color = "";
			} else {
				document.getElementById('btnverstuurmailing').disabled = true;
				document.getElementById('btnverstuurmailing').style.color = "red";
				
				document.getElementById('btnbekijkvoorbeeld').disabled = true;
				document.getElementById('btnbekijkvoorbeeld').style.color = "red";
			}
		}
	});
	
	const options = { year: 'numeric', month: 'long', day: 'numeric' };
	var dat = new Date($('#selectie_vangebdatum').val());
	document.getElementById('tekst_vangebdatum').innerHTML = ' (' + dat.toLocaleDateString('nl-NL', options) + ')';
	
	dat = new Date($('#selectie_temgebdatum').val());
	document.getElementById('tekst_temgebdatum').innerHTML = ' (' + dat.toLocaleDateString('nl-NL', options) + ')';

	$.ajax({
		url: 'ajax_update.php?entiteit=mailingprops',
		type: 'post',
		dataType: 'json',
		data: { mailingid: mid, selectie_vangebdatum: $('#selectie_vangebdatum').val(), selectie_temgebdatum: $('#selectie_temgebdatum').val(), selectie_groep: $('#selectie_groep').val() },
		success: function(response) {
			if (response['aantalontvangers'] > 1) {
				$('#lblOntvangers').html('Ontvangers (' + response['aantalontvangers'] + ')');
			} else {
				$('#lblOntvangers').html('Ontvangers');
			}
			
			document.getElementById('aantalpersoneningroep').innerHTML = response['aantalingroep'];
		}
	});
	
	$.ajax({
		url: 'ajax_update.php?entiteit=mailing_html_ontvangers',
		type: 'post',
		dataType: 'json',
		data: { mailingid: mid },
		success: function(response) {
			if (response.length > 12) {
				$('#lblOntvangers').show();
				$('#lijstontvangers').show();
			} else {
				$('#lijstontvangers').hide();
				$('#lblOntvangers').hide();
			}
			$('#lijstontvangers').html(response);
		}
	});
	
	
	if (document.getElementById('cb_alle_personen').checked == true) {
		f = 1;
	} else {
		f = 0;
	}
	$.ajax({
		url: 'ajax_update.php?entiteit=options_mogelijke_ontvangers',
		type: 'post',
		dataType: 'json',
		data: { mailingid: mid, alle: f },
		success: function(response) {
			document.getElementById('add_lid').innerHTML = response;
		}
	});
}

function mailing_add_ontvanger(p_mid, p_lidid, p_email) {
	var mid = $('#recordid').text();
	
	$.ajax({
		url: 'ajax_update.php?entiteit=mailing_add_ontvanger',
		type: 'post',
		dataType: 'json',
		data: { mid: mid, lidid: p_lidid, email: p_email }
	});
	document.getElementById('add_lid').value = 0;
	mailingprops(mid);
}

function mailing_add_selectie_ontvangers() {
	var mid = $('#recordid').text();
	var groepid = $('#selectie_groep').val();
	var vangebdatum = $('#selectie_vangebdatum').val();
	var temgebdatum = $('#selectie_temgebdatum').val();
	
	$.ajax({
		url: 'ajax_update.php?entiteit=mailing_add_selectie_ontvangers',
		type: 'post',
		dataType: 'json',
		data: { mid: mid, selgroep: groepid, vangebdatum: vangebdatum, temgebdatum: temgebdatum }
	});
	mailingprops();
}

function mailing_verw_ontvanger(p_mid, p_lidid, p_email) {
	var mid = $('#recordid').text();
	$.ajax({
		url: 'ajax_update.php?entiteit=mailing_verw_ontvanger',
		type: 'post',
		dataType: 'json',
		data: { mid: mid, lidid: p_lidid, email: p_email }
	});
	mailingprops();
}

function mailing_verw_selectie_ontvangers() {
	var mid = $('#recordid').text();
	var groepid = document.getElementById('selectie_groep').value;
	var vangebdatum = $('#selectie_vangebdatum').val();
	var temgebdatum = $('#selectie_temgebdatum').val();
	
	$.ajax({
		url: 'ajax_update.php?entiteit=mailing_verw_selectie_ontvangers',
		type: 'post',
		dataType: 'json',
		data: { mid: mid, selgroep: groepid, vangebdatum: vangebdatum, temgebdatum: temgebdatum },
		success: function(response){
			if (response > 0) {
				mailingprops();
			}
		}
	});
}

function mailing_verw_alle_ontvangers() {
	var mid = $('#recordid').text();
	
	$.ajax({
		url: 'ajax_update.php?entiteit=mailing_verw_alle_ontvangers',
		type: 'post',
		dataType: 'json',
		data: { mid: mid },
		success: function(response){
			if (response > 0) {
				mailingprops();
			}
		}
	});
	
}
			
function mailing_savemessage() {
	var mid = $('#recordid').text();
	
	var value = tinymce.get('message').getContent();
	$.ajax({
		url: 'ajax_update.php?entiteit=mailing',
		type: 'post',
		dataType: 'json',
		data: { field:'message', value:value, id:mid },
		success: function(response){}
	});
}

function togglevariabelen() {
	
	if ($('#lblbeschikbarevariabelen > span').text() == "+") {
		$("#lblbeschikbarevariabelen > span").text("-");
		$("#lijstvariabelen").height("auto");
	} else {
		$("#lblbeschikbarevariabelen > span").text("+");
		$("#lijstvariabelen").height($("#lblbeschikbarevariabelen").height());
	}

}
