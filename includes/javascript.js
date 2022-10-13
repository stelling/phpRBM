function fnFilter(p_table, p_filtercontrol, p_skipkolom=-1) {
	var filter, table, tr, td, i, j, txtValue;
	
	filter = p_filtercontrol.value.toUpperCase();
	table = document.getElementById(p_table);
	tr = table.getElementsByTagName("tbody")[0].getElementsByTagName("tr");
	
	if (filter.length > 2) {

		for (i = 0; i < tr.length; i++) {
			var hideline = true;
			for (j = 0; j < tr[i].getElementsByTagName("td").length; j++) {
				td = tr[i].getElementsByTagName("td")[j];
				txtValue = td.textContent || td.innerText;
				if (txtValue.toUpperCase().indexOf(filter) > -1 && p_skipkolom != j) {
					hideline = false;
				}
			}
			if (hideline) {
				tr[i].style.display = "none";
			} else {
				tr[i].style.display = "";
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
	
	if (id.startsWith("Vanaf_naam") || id.startsWith("Vanaf_email")) {
		var split_id = id.split('_');
		var field_name = split_id[0] + '_' + split_id[1];
		var rid = split_id[2];
		
	} else if (id.indexOf("_") !== -1 && id !== "cc_addr" && id !== "BET_TERM") {
		var split_id = id.split('_');
		var field_name = split_id[0];
		var rid = split_id[1];
		
	} else {
		var field_name = control.id;
	}
	
	if (control.type == "checkbox") {
		if (control.checked == true) {
			var value = 1;
		} else {
			var value = 0;
		}

	} else if (field_name == "Bedrag") {
		var value = control.value.replace(",", ".");
		
	} else {
		var value = control.value;
	}

//	alert(entity + ' / field_name: ' + field_name + ' / value: ' + value + ' / rid: ' + rid);

	$.ajax({
		url: 'ajax_update.php?entiteit=' + entity,
		type: 'post',
		dataType: 'json',
		data: { field: field_name, value: value, id: rid },
		success: function(response){},
		fail: function( data, textStatus ) {
			alert(entity + 'update database is niet gelukt. ' + textStatus);
		}
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

function deleterecord(entity, rid) {
	
	$.ajax({
		url: 'ajax_update.php?entiteit=' + entity,
		type: 'post',
		dataType: 'json',
		data: { id:rid },
		success:function(response){}
	});	
}

/* Ledenadministratie specifiek */

function lidalgwijzprops() {
	
	var rn = $('#Roepnaam');
	var vl = $('#Voorletter');
	var gs = $('#Geslacht');
	
	if (vl.val().length == 0 && rn.val().length > 1 && gs.val() != "B") {
		vl.val(rn.val().substring(0, 1) + '.');
	}
	
	if (gs.val() == "V") {
		$('#lblMeisjesnm, #Meisjesnm, #uitleg_meisjesnm').show();
	} else {
		$('#lblMeisjesnm, #Meisjesnm, #uitleg_meisjesnm').hide();
	}
	
	var gbd = $('#GEBDATUM');
	if (gbd.val().length == 10 && gbd.val() > '1920-01-01') {
		const options = { year: 'numeric', month: 'long', day: 'numeric' };
		var dat = new Date(gbd.val());
		var today = new Date();
		var t;
		if (dat > today) {
			t = 'De geboortedatum mag niet in de toekomst liggen.';
			$('#uitleg_gebdatum').css({'color': 'red', 'weight': 'bolder'});
		} else {
			var lft = today.getYear() - dat.getYear();
			if (dat.getMonth() == today.getMonth() && dat.getDate() > today.getDate()) {
				lft--;
			} else if (dat.getMonth() > today.getMonth()) {
				lft--;
			}
			t = ' (' + dat.toLocaleDateString('nl-NL', options) + ')';
			if (lft > 1) {
				t = t.concat(' (', lft, ' jaar)');
			}
		}
		$('#uitleg_gebdatum').text(t);
	} else {
		$('#uitleg_gebdatum').text('');
	}
	
	var tel = $('#Telefoon').val();
	$('#uitleg_telefoon').text(fnControleTelefoon(tel));
	
	var tel = $('#Mobiel').val();
	$('#uitleg_mobiel').text(fnControleTelefoon(tel, "mobiel"));
	
	var e = $('#Email').val();
	$('#uitleg_email').text(fnControleEmail(e));
	
	var e = $('#EmailOuders').val();
	$('#uitleg_emailouders').text(fnControleEmail(e));
	
	if($("#EmailVereniging").length > 0) {
		var e = $('#EmailVereniging').val();
		$('#uitleg_emailvereniging').text(fnControleEmail(e));
	}
}

function loperlidprops() {
	
	var lidid = $('#lidid').val();
	var ondtype = $('#ondtype').val();
	
	$.ajax({
		url: 'ajax_update.php?entiteit=htmlloperlid',
		type: 'post',
		dataType: 'json',
		data: { lidid: lidid, ondtype: ondtype },
		success:function(reshtml){
			$('#tablelidond > tbody').empty();
			$('#tablelidond > tbody').append(reshtml);
		}
	});
}

function addlidond() {
	
	var lidid = $('#lidid').val();
	var ondid = $('#NieuwOnderdeel').val();
	
	if (ondid > 0) {
		$.ajax({
			url: 'ajax_update.php?entiteit=addlidond',
			type: 'post',
			async: false,
			dataType: 'json',
			data: { lidid: lidid, ondid: ondid }
		});
		loperlidprops();
	}
}

/* Mailing specifiek */

function mailingprops() {
	var mid = $('#recordid').text();
	
	$.ajax({
		url: 'ajax_update.php?entiteit=mailingcontrole',
		type: 'post',
		dataType: 'json',
		async: false,
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
		async: false,
		data: { mailingid: mid, selectie_vangebdatum: $('#selectie_vangebdatum').val(), selectie_temgebdatum: $('#selectie_temgebdatum').val(), selectie_groep: $('#selectie_groep').val() },
		success: function(response) {
			if (response['aantalontvangers'] > 1) {
				$('#lblOntvangers').html('Ontvangers (' + response['aantalontvangers'] + ')');
			} else {
				$('#lblOntvangers').html('Ontvangers');
			}
			document.getElementById('aantalpersoneningroep').innerHTML = response['aantalingroep'];
		},
		error: function(jqXHR, textStatus, errorThrown) {
			alert('Ophalen mislukt: ' + errorThrown);
		}
	});
	
	$.ajax({
		url: 'ajax_update.php?entiteit=mailing_html_ontvangers',
		type: 'post',
		dataType: 'json',
		async: false,
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
		async: false,
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
		async: false,
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
		async: false,
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
		async: false,
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
		async: false,
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
		async: false,
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
		async: false,
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

/* Rekening-specifiek */

function rekeningprops() {
	
	var euroformat = new Intl.NumberFormat('nl-NL', {
		style: 'decimal',
		minimumFractionDigits: 2,
		maximumFractionDigits: 2
	});
	
	var rkid = $('#reknr').text();
	rkbedrag = 0;
	table = document.getElementById("rekregels");
	tr = table.getElementsByTagName("tr");
	for (i = 0; i < tr.length; i++) {
		td = tr[i].getElementsByTagName("td")[4];
		 
		if (td) {
			cntr = td.getElementsByTagName("input")[0];
			if (cntr.disabled == false) {
				rkbedrag = rkbedrag + parseFloat(cntr.value.replace(",", "."));
			}
		}
	}
	
	$.ajax({
		url: 'ajax_update.php?entiteit=rekeningedit',
		type: 'post',
		dataType: 'json',
		data: { field: 'Bedrag', value: rkbedrag, id: rkid }
	});
	
	$("#rekeningbedrag").html(euroformat.format(rkbedrag));
	
	var rekdatum = new Date($('#Datum').val());
	var bedragbetaald = $('#bedragbetaald').val();
	days = parseInt($("#BETAALDAG").val());
	var today = new Date();
	const betaaldatum = new Date(rekdatum.getFullYear(), rekdatum.getMonth(), rekdatum.getDate());
	
	if (rkbedrag == 0 && bedragbetaald == 0) {
		$('#lblbedragbetaald').hide();
		$('#bedragbetaald').hide();
	} else {
		$('#lblbedragbetaald').show();
		$('#bedragbetaald').show();
	}
	
	betaaldatum.setDate(rekdatum.getDate() + days);
		
	if (rkbedrag != 0) {
		$("#lbluitersteBetaling").show();
		$("#uitersteBetaling").show();
		const options = { year: 'numeric', month: 'long', day: 'numeric' };
		$("#uitersteBetaling").html(betaaldatum.toLocaleDateString('nl-NL', options));
		if (betaaldatum.getTime() < today.getTime() && bedragbetaald < rkbedrag) {
			$("#uitersteBetaling").addClass("telaat");
		} else {
			$("#uitersteBetaling").removeClass("telaat");
		}
	} else {
		$("#lbluitersteBetaling").hide();
		$("#uitersteBetaling").hide();
	}
	
	var dn = $("#DEBNAAM");
	if (dn.val().length == 0) {
		var lid = $("#Lid").val();
		
		$.ajax({
			url: 'ajax_update.php?entiteit=naamlid',
			type: 'post',
			dataType: 'json',
			data: { id:lid },
			success: function(response){
				dn.val(response);
			}
			
		});
	}
	
	var lid = $("#BetaaldDoor").val();
	
	$.ajax({
		url: 'ajax_update.php?entiteit=telefoonlid',
		type: 'post',
		dataType: 'json',
		data: { id:lid },
		success: function(response){
			$("#telefoondebiteur").text(response);
		},
		error: function( jqXHR, textStatus, errorThrown) {
			alert("Ophalen telefoonnr mislukt: " + errorThrown);
		}
	});
	
	$.ajax({
		url: 'ajax_update.php?entiteit=emaillid',
		type: 'post',
		dataType: 'json',
		data: { id:lid },
		success: function(response){
			$("#emaildebiteur").text(response);
		}
	});
	
}

function blurkostenplaats(control) {
	
	var split_id = control.id.split('_');
	var rid = split_id[1];
	var oms = $("$OMSCHRIJV_" + rid);
	
	if (oms.val().length == 0) {
		oms.val(control.value);
	}
	
}

/* Algemeen */

function fnControleTelefoon(p_nr, p_srt="telefoon") {
	
	var rv;
	
	if (p_nr.length == 0) {
		rv = "";
	} else if (p_nr.substr(2, 1) == "-" && p_nr.length == 11 && p_srt == "mobiel") {
		rv = "";
	} else if (p_nr.substr(3, 1) == "-" && p_nr.length == 11 && p_srt == "telefoon") {
		rv = "";
	} else if (p_nr.substr(4, 1) == "-" && p_nr.length == 11 && p_srt == "telefoon") {
		rv = "";
	} else if (p_nr.lengte == 10 && p_nr.includes("-") === false) {
		rv = "";
	} else if (p_srt == "telefoon") {
		rv = "Formaat telefoonnummer is niet correct.";
	} else {
		rv = "Formaat mobiele nummer is niet correct.";
	}
	
	return rv;
}

function fnControleEmail(p_email, p_rvbijleeg="") {
	
	var emailReg = /^([\w-\.]+@([\w-]+\.)+[\w-]{2,4})?$/;
	
	if (p_email.length == 0) {
		rv = p_rvbijleeg;
	} else if (p_email.match(emailReg)) {
		rv = "";
	} else {
		rv = "Formaat e-mailadres is niet correct.";
	}
	
	return rv;
	
}
