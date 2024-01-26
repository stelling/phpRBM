function fnFilter(p_table, p_filtercontrol, p_skipkolom=-1) {
	var filter, table, tr, td, i, j, txtValue;
	
	if (p_filtercontrol.value == undefined) {
		filter = p_filtercontrol.toUpperCase();
	} else {
		filter = p_filtercontrol.value.toUpperCase().trim();
	}
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

function CopyFunction(p_id="copywijzigingen") {
	let el = document.getElementById(p_id);
	el.select();
	document.execCommand('copy');
	alert("De inhoud is naar het klembord gekopieerd.");
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
	} else {
		var value = control.value;
	}

//	alert(entity + ' / field_name: ' + field_name + ' / value: ' + value + ' / rid: ' + rid);

	$.ajax({
		url: 'ajax_update.php?entiteit=' + entity,
		type: 'post',
		data: { field: field_name, value: value, id: rid },
        dataType: "json",
        success: function(data) {
			if (data.length > 4) {
				alert(data);
			}
			return data;
        },
		fail: function( data, textStatus ) {
			alert(entity + ': update database is niet gelukt. ' + textStatus);
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

function saveparam(control) {
	
	var pn = control.id;
	
	if (control.type == "checkbox") {
		if (control.checked == true) {
			var value = 1;
		} else {
			var value = 0;
		}
	} else {
		var value = control.value;
	}
	
	$.ajax({
		url: 'ajax_update.php?entiteit=updateparam',
		type: 'post',
		dataType: 'json',
		data: { name:pn, value:value }
	});
}

function savecheckasdate(entity, rid, control) {
	
	if (entity == "editinschrijving" && control.id == "chkverwerkt") {
		field_name = "Verwerkt";
	} else if (entity == "editinschrijving" && control.id == "chkverwijderd") {
		field_name = "Verwijderd";
	}
		
	$.ajax({
		url: 'ajax_update.php?entiteit=' + entity,
		type: 'post',
		dataType: 'json',
		data: {field: field_name, id: rid },
		fail: function( data, textStatus ) {
			alert(entity + ': update database is niet gelukt. ' + textStatus);
		}
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
	var ov = $('#Overleden');
	
	if (typeof vl !== 'undefined' && typeof rn !== 'undefined') {
		if (vl.val().length == 0 && rn.val().length > 1 && gs.val() != "B") {
			vl.val(rn.val().substring(0, 1) + '.');
		}
	}
	
	if (gs.val() == "V") {
		$('#lblMeisjesnm, #Meisjesnm, #uitleg_meisjesnm').show();
	} else {
		$('#lblMeisjesnm, #Meisjesnm, #uitleg_meisjesnm').hide();
	}
	
	var gbd = $('#GEBDATUM');
	var t = "";
	if (gbd.val().length == 10 && gbd.val() > '1920-01-01') {
		const options = { year: 'numeric', month: 'long', day: 'numeric' };
		var dat = new Date(gbd.val());
		var today = new Date();
		var t;
		if (dat > today) {
			t = 'De geboortedatum mag niet in de toekomst liggen.';
			$('#uitleg_gebdatum').addClass('error');
		} else {
			var lft = today.getYear() - dat.getYear();
			if (dat.getMonth() == today.getMonth() && dat.getDate() > today.getDate()) {
				lft--;
			} else if (dat.getMonth() > today.getMonth()) {
				lft--;
			}
			t = dat.toLocaleDateString('nl-NL', options);
			if (lft > 1 && ov.val() == "") {
				t = t.concat(' (', lft, ' jaar)');
			}
		}
	}
	$('#uitleg_gebdatum').text(t);
	
	adresvullen();
	
	var tel = $('#Telefoon').val();
	if (typeof tel !== 'undefined') {
		$('#uitleg_telefoon').text(fnControleTelefoon(tel));
	}
	
	var tel = $('#Mobiel').val();
	if (typeof tel !== 'undefined') {
		if (tel.length == 10 && tel.substr(0, 2) == "06") {
			tel = tel.substr(0, 2) + "-" + tel.substr(2);
		}
		$('#uitleg_mobiel').text(fnControleTelefoon(tel, "mobiel"));
	}
	
	var e = $('#Email').val();
	$.ajax({
		url: 'ajax_update.php?entiteit=checkdnsrr',
		type: 'post',
		dataType: 'json',
		data: { email: e },
		success: function(response){
			$('#uitleg_email').text(response);
		}
	});
	
	e = $('#EmailOuders').val();
	if (typeof e !== 'undefined') {
		if (e.length > 0) {
			$.ajax({
				url: 'ajax_update.php?entiteit=checkdnsrr',
				type: 'post',
				dataType: 'json',
				data: { email: e },
				success: function(response){
					$('#uitleg_emailouders').text(response);
				}
			});
		} else {
			$('#uitleg_emailouders').text("");
		}
	}

	e = $('#EmailVereniging').val();
	if (typeof e !== 'undefined') {
		if (e.length > 0) {
			$.ajax({
				url: 'ajax_update.php?entiteit=checkdnsrr',
				type: 'post',
				dataType: 'json',
				data: { email: e },
				success: function(response){
					$('#uitleg_emailvereniging').text(response);
				}
			});
		} else {
			$('#uitleg_emailvereniging').text("");
		}
	}
	
	lidfinwijzprops();
	
	s = $('#Burgerservicenummer').val();
	if (typeof s !== 'undefined') {
		$('#uitleg_burgerservicenummer').text("");
		if (s.length > 0) {
			if (isNaN(s)) {
				$('#uitleg_burgerservicenummer').text("Dit is geen nummer, deze wijziging wordt neit verwerkt.");
			} else if (s < 100000000) {
				$('#uitleg_burgerservicenummer').text("Deze BSN is te klein, deze wijziging wordt neit verwerkt.");
			}
		}
	}
}

function lidfinwijzprops() {
	b = $('#Bankrekening').val();
	if (typeof b !== 'undefined') {
		$('#uitleg_bankrekening').text("IBAN");
		if (b.length > 0) {
			$.ajax({
				url: 'ajax_update.php?entiteit=checkiban',
				type: 'post',
				dataType: 'json',
				data: { iban: b },
				success: function(response){
					if (response == false) {
						$('#uitleg_bankrekening').text('Het formaat/controlegetal van deze bankrekening is niet correct. De Bankrekening wordt niet gewijzigd.');
					}
				}
			});
		}
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

function verwijderkloslid(p_lidid, p_undo=0) {
	
	if (p_undo == 0) {
		const date = new Date();

		$.ajax({
			url: 'ajax_update.php?entiteit=lid',
			type: 'post',
			dataType: 'json',
			data: { field: 'Verwijderd', value: date, id: p_lidid },
			fail: function( data, textStatus ) {
				alert(entity + ': update database is niet gelukt. ' + textStatus);
			}	
		});
	}
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
				$('#btnverstuurmailing').addClass("disbabled");
				
				document.getElementById('btnbekijkvoorbeeld').disabled = true;
//				document.getElementById('btnbekijkvoorbeeld').style.color = "red";
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
	
	/*
	
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
	*/
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
	mailingprops();
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
		data: { mid: mid, selgroep: groepid, vangebdatum: vangebdatum, temgebdatum: temgebdatum },
		success: function(response){
			mailingprops();
		}
	});
}

function mailing_verw_ontvanger(p_rid, p_email) {
	$.ajax({
		url: 'ajax_update.php?entiteit=mailing_verw_ontvanger',
		type: 'post',
		dataType: 'json',
		data: { id: p_rid, email: p_email },
		success: function(response){
			$("#ontvanger_" + p_rid + " > img").hide();
			$("#ontvanger_" + p_rid).addClass("deleted");
		}
	});
	
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
			mailingprops();
		},
		error: function(jqXHR, textStatus, errorThrown) {
			alert('Mislukt: ' + errorThrown);
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
			
function mailing_savemessage(p_hist=0) {
	var rid = $('#recordid').text();
	
	if (p_hist == 1) {
		var e = "email";
	} else {
		var e = "mailing";
	}
	
	var value = tinymce.get('message').getContent();
	$.ajax({
		url: 'ajax_update.php?entiteit=' + e,
		type: 'post',
		dataType: 'json',
		data: { field:'message', value:value, id:rid },
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

function mailtooutbox (p_rid) {
	
	$.ajax({
		url: 'ajax_update.php?entiteit=email',
		type: 'post',
		dataType: 'json',
		data: {field: 'send_on', id: p_rid, value: ''},
		fail: function( data, textStatus ) {
			alert(entity + ': update database is niet gelukt. ' + textStatus);
		}
	});
	
	var nvv = new Date();
	nvv.setMinutes(nvv.getMinutes() + 20);
	nvv = nvv.toISOString().slice(0, 19).replace('T', ' ');
	
	$.ajax({
		url: 'ajax_update.php?entiteit=email',
		type: 'post',
		dataType: 'json',
		data: {field: 'NietVersturenVoor', id: p_rid, value: '+20 minute'},
		fail: function( data, textStatus ) {
			alert(entity + ': update database is niet gelukt. ' + textStatus);
		}
	});
	
	$('#send_on_' + p_rid).addClass('deleted');
	$('#mislukt_' + p_rid).hide();
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
	bt = parseInt($("#BET_TERM").val());
	var today = new Date();
	const betaaldatum = new Date(rekdatum.getFullYear(), rekdatum.getMonth(), rekdatum.getDate());
	
	if (rkbedrag == 0 && bedragbetaald == 0) {
		$('#lblbedragbetaald').hide();
		$('#bedragbetaald').hide();
	} else {
		$('#lblbedragbetaald').show();
		$('#bedragbetaald').show();
	}
	
	betaaldatum.setDate(rekdatum.getDate() + (days * bt));
		
	if (rkbedrag != 0) {
		$("#lbluitersteBetaling").show();
		$("#uitersteBetaling").show();
		$("#lblBETAALDAG").show();
		$("#BETAALDAG").show();
		$("#lblBET_TERM").show();
		$("#BET_TERM").show();
		const options = { year: 'numeric', month: 'long', day: 'numeric' };
		$("#uitersteBetaling").html(betaaldatum.toLocaleDateString('nl-NL', options));
		if (betaaldatum.getTime() < today.getTime() && bedragbetaald < rkbedrag && bedragbetaald !== 'undefined') {
			$("#uitersteBetaling").addClass("telaat");
		} else {
			$("#uitersteBetaling").removeClass("telaat");
		}
	} else {
		$("#lbluitersteBetaling").hide();
		$("#uitersteBetaling").hide();
		$("#lblBETAALDAG").hide();
		$("#BETAALDAG").hide();
		$("#lblBET_TERM").hide();
		$("#BET_TERM").hide();
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
	
	$.ajax({
		url: 'ajax_update.php?entiteit=rekeningmail',
		type: 'post',
		dataType: 'json',
		data: { id: rkid },
		success: function(response){
			$("#laatsteemail").html(response);
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
		
		$.ajax({
			url: 'ajax_update.php?entiteit=checkdnsrr',
			type: 'post',
			dataType: 'json',
			async: false,
			data: { id: rkid },
			success: function(response){
				if (response == 0) {
					rv = "Domein bestaat niet of er mag niet naar gemailed worden.";
					alert(rv);
				}
			}
		});
		
	} else {
		rv = "Formaat e-mailadres is niet correct.";
	}
	
	return rv;
}

function adresvullen() {

	var pc = $('#Postcode').val();
	var	zpc = pc.replace(' ', '');
	var hn = $('#Huisnr').val();
	var hl = $('#Huisletter').val();
	var tv = $('#Toevoeging').val();
	var ad = $('#Adres');
	var wp = $('#Woonplaats');
	
	if (zpc.length >= 6 && hn.length > 0) {
			
		var url = 'https://api.pdok.nl/bzk/locatieserver/search/v3_1/free?fq=postcode:' + zpc + '&fq=huisnummer:' + hn;
		if (hl.length > 0) {
			url = url + '&fq=huisletter:' + hl;
		}
		if (tv.length > 0) {
			url = url + '&fq=huisnummertoevoeging:' + tv;
		}
		$.ajax({
			url: url,
			dataType: 'json',
			type: 'get',
			success: (data) => {
				if (data.response.numFound == 0) {
					$('#uitleg_adres').html('Adres bestaat niet');
					ad.val('');
				} else {
					var nad = data.response.docs[0].straatnaam + ' ' + data.response.docs[0].huis_nlt;
					ad.val(nad);
					wp.val(data.response.docs[0].woonplaatsnaam);
					$("#uitleg_adres").html('');
				}
			}
		});
	} else {
		if (hn.length > 0 && hn > "0") {
			ad.val('');
			wp.val('');
		}
	}
}

function verw_auth(rid) {
	deleterecord('delete_autorisatie', rid);
	$('#name_' + rid).addClass('deleted');
	$('#delete_' + rid).hide();
}

function add_auth(tabpage) {
	$.ajax({
		url: 'ajax_update.php?entiteit=add_autorisatie',
		type: 'post',
		async: false,
		dataType: 'json',
		data: { tabpage:tabpage },
		success:function(response) {
			window.location.reload();
		}
	});
}