<!DOCTYPE HTML>

<?php
date_default_timezone_set('Europe/Amsterdam');
setlocale(LC_ALL,'nl_NL@euro');

	if (isset($_GET['tp'])) {
		$currenttab = $_GET['tp'];
	} else {
		$currenttab = "Algemeen";
	}

	$tabpages[] = "Algemeen";
	$tabpages[] = "Mailing";
	$tabpages[] = "Templates";
	$tabpages[] = "Bestellingen";
	$tabpages[] = "Beheer";
	
	printf("<html lang='nl'>
<head>
<meta charset='ISO-8859-1'>
<meta name='robots' content='index,follow'>
<title>phpRBM | Handleiding | %s</title>
<link rel='stylesheet' href='default.css'>
</head>
<body>
<div id='container'>
<div id='header'>

<h1>phpRBM | Handleiding</h1>
<div class='clear'></div>
<ul class='tablist1'>", $currenttab);

	foreach($tabpages as $tp) {
		if ($currenttab == $tp) {
			printf("<li id='current'>%s</li>\n", $tp);
		} else {
			printf("<li><a href=\"%s?tp=%s\">%s</a></li>\n", $_SERVER['PHP_SELF'], urlencode($tp), $tp);
		}
	}
	echo("</ul>\n");
	echo("</div>  <!-- Einde header -->");
	
	echo("<div id='handleiding'>\n");
	
	if ($currenttab == "Mailing") {
?>
<h1>Handleiding mailings in phpRBM</h1>

<h2>Introductie</h2>
<p>Met deze module kunnen e-mails aan leden, groepen leden of externe contacten verstuurd worden. Hierbij kan bij leden of groepen leden gebruik gemaakt worden van mailmerge.
Ook zijn er een aantal mailings met een specifiek doel, zoals het versturen van een lidnummer of de bevestiging van een bewaking.</p>

<h2>Terminologie</h2>
<p>Er worden een aantal termen in de mailingsmodule gebruikt die wellicht handig zijn om even verder uit te leggen.</p>
<ul>
<li>Bijlagen: dit zijn documenten die bij een e-mail gevoegd kunnen worden. Ook wel attachments genoemd. Niet alle soorten bestanden mogen bijgevoegd worden. Sommige extensies worden door bepaalde virusscanners of spamfilters als een gevaar aangemerkt en hierdoor komt de e-mail wellicht niet of vertraagd aan.</li>
<li>Concept: een mailing die aangemaakt is, maar nog niet verzonden is.</li>
<li>Template: een mailing die je vaker wilt gaan gebruiken, je kan deze dan ook meerdere malen versturen.</li>
<li>Eén gezamenlijke e-mail: met deze optie worden geen afzonderlijke e-mails gestuurd, maar één e-mail naar alle ontvangers. Deze optie kan niet gebruikt worden in combinatie met mergevelden.</li>
<li>Variabelen: dit zijn velden die gebruikt kunnen worden in mailmerge. Feitelijk wordt dit veld vervangen door de waarde die in de database staat. Bij het gebruik van variabelen moet er goed op gelet worden dat deze letterlijk van de lijst overgenomen moeten worden. Inclusief blokhaken en %-tekens.</li>
<li>Vertrouwelijk: een mailing die, alleen door degene die hem gemaakt heeft en de webmasters, bekeken kan worden.</li>
</ul>

<h2>Versturen mailing of e-mails</h2>
<p>Zowel een bestaande als een nieuwe mailing kan verzonden worden. Een aantal zaken om rekening mee te houden.</p>
<ul>
<li>In het veld 'Aan' kan vrije tekst getypt worden. Hierin kan de groep beschreven worden die deze mail krijgt.</li>
<li>Er kunnen twee soorten ontvangers toegevoegd worden, te weten
	<ul>
	<li>Leden van de vereniging. In dat geval wordt het e-mailadres uit de database gehaald. Bij deze ontvangers zal de mailmerge werken.</li>
	<li>Losse e-mailadressen. Hiermee kan naar bijvoorbeeld externe contacten een mail gestuurd worden. Elk geldig e-mailadres mag worden toegevoegd.</li>
	</ul>
	In de lijst met toegevoegde ontvangers kan je het verschil zien doordat bij de eerste groep je de naam van het lid ziet en bij de tweede het e-mailadres.
</li>
<li>Cc: dit e-mailadres krijgt van elke mail een afschrift. Dus als je een mailing aan 250 leden stuurt en je vult hier een adres in, krijgt dit adres 250 mailtjes. Je kan hier meerdere adressen invullen, door ze middels een komma te scheiden.</li>
<li>Indien bij een lid het veld 'E-mail ouders' is ingevuld en het lid is jonger dan 18 jaar dan wordt er aan dit e-mailadres een cc gestuurd.</li>
<li>Voor het maken van het bericht maakt phpRBM gebruik van <a href='http://ckeditor.com/'>CKEditor</a>. CKEditor is een open source text editor met HTML-opmaak.</li>
<li>Wees spaarzaam met het aanpassen van kleuren in de editor. Veel kleuren worden namelijk door de template bepaald, echter zie je die niet in de editor. Hierdoor kan je bijvoorbeeld tekst rood maken, terwijl de achtergrond in de template ook rood is.</li>
<li>Bij twijfel hoe de mail eruit komt te zien, kan je de mail eerst aan jezelf versturen. Mocht hij dan naar wens zijn is het een kwestie van de ontvangers aanpassen en hij kan gewoon verstuurd worden. Je kan ook gebruik maken van de preview optie alvorens een mailing te versturen.</li>
<li>De layout van de uiteindelijke mail wordt bepaald door de template 'briefpapier'. Zie hiervoor de uitleg over templates.</li>
</ul>

<h2>Mailings met een specifiek doel</h2>
Er zijn op dit moment vier mailings met een specifiek doel mogelijk, te weten:
<ul>
<li>Bevestiging bestelling</li>
<li>Bevestiging inschrijving bewaking</li>
<li>Bevestiging opzegging lidmaatschap</li>
<li>Versturen lidnummer</li>
</ul>
<p>Feitelijk zijn dit gewone mailings, welke in de instellingen kunnen worden aangewezen voor één van bovenstaande doelen.
Verder hoeven er bij deze mailings geen ontvangers toegevoegd te worden en zijn er extra variabelen beschikbaar.</p>

<h2>Hoe worden de e-mail verstuurd?</h2>
<p>Voor het versturen zijn er twee technieken mogelijk, te weten de <a href='http://www.php.net/mail'>mail-functie van PHP</a> en <a href='http://nl.wikipedia.org/wiki/Smtp'>SMTP</a>. Mocht er een SMTP-server ingevuld zijn, dan wordt de mailing via SMTP verzonden, anders via de de mail-functie van PHP. Voor instellingen en eventuele aanvullende regels verwijs ik naar je webhoster.</p>
<p>Standaard wordt de mailing opsplitst in individuele mails, dit heeft als voordeel dat niet alle e-mailadressen voor alle ontvangers zichtbaar worden. Ook is zo de kans kleiner dat de mail als spam aangemerkt wordt. Als je de mails niet wilt opsplitsen, bijvoorbeeld omdat je de ontvangers de mogelijkheid van Reply-to-all wilt geven, kan je in een mailing 'Eén gezamenlijke e-mail' aanvinken.</p>
<p>In de instellingen kan een maximaal aantal mails per minuut worden opgegeven. Sommige hostingproviders hebben hier namelijk een beperking op. Als dit getal laag staat kan het wel betekenen dat het versturen van een mailing lang duurt.</p>

<?php
	
	} elseif ($currenttab == "Templates") {
?>

<h1>Handleiding templates in phpRBM</h1>

<h2>Introductie</h2>
<p>Templates zijn gemaakt om deze website verenigingsspecifieker te maken. Templates werken met velden, de velden zijn te herkennen aan: [% %]. Deze velden worden tijdens de uitvoering vervangen door de waarde, dus in feite mailmerge.</p>

De volgende templates zijn beschikbaar, deze staan in de folder 'templates'.
<ul>
<li>bevestiging_login.html</li>
<li>briefpapier.html</li>
<li>opzegging.html</li>
<li>rekening.html</li>
<li>verenigingsinfo.html</li>
</ul>

<h2>bevestiging_login</h2>
<p>Dit is de mail die gestuurd wordt als iemand zijn login aanvraagt.</p>
De volgende template-specifieke velden zijn beschikbaar.
<ul>
<li>[%LOGIN%]</li>
<li>[%PASSWORD%]</li>
<li>[%GEBLOKKEERD%]: hierin komt een tekst als de login geblokkeerd is</li>
<li>[%IPADDRESS%]: het ip-adres waarvan dit verzoek is gedaan</li>
</ul>
<p>Bij het versturen wordt de template 'briefpapier' gebruikt.</p>

<h2>briefpapier</h2>
<p>Dit is de template voor de mailings en deze wordt ook gebruikt voor het versturen van de aangevraagde logins.</p>

De volgende velden zijn beschikbaar.
<ul>
<li>[%FROM%]</li>
<li>[%TO%]</li>
<li>[%SUBJECT%]</li>
<li>[%MESSAGE%]</li>
</ul>
De namen spreken voor zich, bij from en to zijn het niet de e-mailadressen, maar de namen van de verzenders en ontvangers. Verder zijn er nog ruim 25 velden in de mailingsmodule zelf beschikbaar. Deze zijn op de ge-adresseerde ge&euml;nt.

<h2>opzegging</h2>
<p>Deze template wordt gebruikt om het formulier voor het opzeggen van het lidmaatschap van de vereniging te regelen. Mocht u deze niet beschikbaar hebben dan wordt een standaardformulier gebruikt.<p>

De volgende template-specifieke velden zijn beschikbaar.
<ul>
<li>[%FORMOPZEGGING%], mocht u deze niet gebruiken, zorg dan dat er een veld (input) in het formulier staat wat 'OpzeggingPer' heet die datum van opzeggen bevat.</li>
<li>[%OPZEGGENVANAF%]: rekening houdend met de opzegtermijn, vanaf welke datum mag het lid het lidmaatschap opzeggen.</li>
</ul>

<h2>rekening</h2>
<p>Deze template wordt gebruikt om de rekening op te maken, zowel in het scherm als in een mailing. De template voor rekeningen kan seizoen-specifiek worden gemaakt. Dit kan gedaan worden door in de naam van de template het nummer van seizoen op te nemen. Bijvoorbeeld: rekening 2012.html.</p>

De volgende template-specifieke velden zijn beschikbaar.
<ul>
<li>[%NAAMDEBITEUR%]</li>
<li>[%REKENINGDATUM%]: de datum van de rekening, opgemaakt als geschreven datum in het Nederlands</li>
<li>[%REKENINGOMSCHRIJVING%]</li>
<li>[%REKENINGNUMMER%]</li>
<li>[%SEIZOEN%]</li>
<li>[%REKENINGREGELS%]: de rekeningregels, opgemaakt als een rij in een tabel.</li>
<li>[%UITERSTEBETAALDATUM%]</li>
<li>[%VANAFADRES%]: het e-mailadres waarvan de rekeningen verzonden worden</li>
<li>[%VANAFNAAM%]: de naam die ook in de from van de e-mail gebruikt wordt</li>
<li>[%BANKREKENING%]: de bankrekening van het lid</li>
<li>[%REKENINGBEDRAG%]: het totaal bedrag van de rekening</li>
<li>[%BETAALD%]: het bedrag dat al betaald is op deze rekening</li>
<li>[%OPENSTAAND%]: het bedrag dat op deze rekening nog open staat</li>
</ul>
De tekst tussen &lt;!-- Geen machtiging --> en &lt;!-- /Geen machtiging --> wordt alleen getoond als het lid geen machtiging heeft afgegegeven. De tekst tussen &lt;!-- Wel machtiging --> en &lt;!-- /Wel machtiging --> wordt alleen getoond als het lid juist wel machtiging heeft afgegegeven. 

<h2>verenigingsinfo</h2>
<p>Dit is de inhoud van de introductiepagina. Hier kan bijvoorbeeld een vereniging iets over zichzelf of de website vertellen en hoe de ondersteuning is geregeld.</p>

<p>De volgende velden zijn beschikbaar.</p>
<ul>
	<li>Aantal leden: [%AANTALLEDEN%]</li>
	<li>Aantal vrouwen: [%AANTALVROUWEN%]</li>
	<li>Aantal mannen: [%AANTALMANNEN%]</li>
	<li>Gemiddelde leeftijd: [%GEMIDDELDELEEFTIJD%]</li>
	<li>Aantal kaderleden: [%AANTALKADERLEDEN%]</li>
	<li>Nieuwste login: [%NIEUWSTELOGIN%]</li>
	<li>Aantal aangemaakte logins: [%AANTALLOGINS%]</li>
	<li>Nu ingelogd: [%NUINGELOGD%]</li>
	<li>Database bijgewerkt t/m: [%LAATSTGEWIJZIGD%]</li>
	<li>De laatste keer dat de upload vanuit de Access-database is gedaan: [%LAATSTEUPLOAD%]</li>
	<li>Huidige gebruiker bijgewerkt t/m: [%INGELOGDEGEWIJZIGD%]</li>
	<li>Verjaardagen met foto's: [%VERJAARFOTO%]</li>
	<li>Verjaardagen zonder foto's: [%VERJAARDAGEN%]</li>
	<li>Komende evenementen: [%KOMENDEEVENEMENTEN%]</li>
	<li>Waarschuwing diploma's die recent vervallen zijn of binnenkort vervallen: [%VERVALLENDIPLOMAS%]</li>
</ul>
Bij verenigingsinfo is het mogelijk om bepaalde stukken tekst alleen te tonen als een gebruiker ingelogd is. Dit doe je door deze stukken tussen &lt;!--&nbsp;Ingelogd&nbsp;--&gt; en &lt;!--&nbsp;/Ingelogd&nbsp;--&gt; te plaatsen.

<h2>Overzicht meerdere keren gebruikte velden</h2>
Hieronder volgt een overzicht van de velden die in meerdere templates gebruikt worden.

<div id='overzichtvelden'>
<table>
<tr><th>Naam veld/template</th><th>bevestiging_login</th><th>opzegging</th><th>rekening</th><th>verenigingsinfo</th></tr>
<tr><th>[%NAAMLID%]</th><td>Ja</td><td>Ja</td><td></td><td>Ja</td></tr>
<tr><th>[%ADRES%]</th><td></td><td></td><td>Ja</td><td></td></tr>
<tr><th>[%POSTCODE%]</th><td></td><td></td><td>Ja</td><td></td></tr>
<tr><th>[%WOONPLAATS%]</th><td></td><td></td><td>Ja</td><td></td></tr>
<tr><th>[%ROEPNAAM%]</th><td>Ja</td><td></td><td>Ja</td><td>Ja</td></tr>
<tr><th>[%LIDNR%]</th><td>Ja</td><td>Ja</td><td>Ja</td><td>Ja</td></tr>
<tr><th>[%NAAMVERENIGING%]</th><td>Ja</td><td>Ja</td><td>Ja</td><td></td></tr>
<tr><th>[%NAAMWEBSITE%]</th><td>Ja</td><td>Ja</td><td>Ja</td><td></td></tr>
<tr><th>[%URLWEBSITE%]</th><td>Ja</td><td>Ja</td><td>Ja</td><td></td></tr>
</table>
</div> <!-- Einde overzichtvelden -->

<h2>Tot slot</h2>
<p>Mocht je meer velden nodig hebben, dan dit graag via <a href='https://github.com/stelling/phpRBM/issues'>GitHub</a> aangeven.</p>

<?php	
	} elseif ($currenttab == "Bestellingen") {
?>	
	<h1>Bestellingen / webshop</h1>
	
	<p>Het onderdeel bestellingen is feitelijk een eenvoudige webshop. Het geeft leden de mogelijkheid artikelen via de zelfservice te bestellen. Ook is het mogelijk om bestellingen door een beheerder laten invoeren en muteren. Er zit geen betaalmogelijkheid in deze webshop.</p>
	
	<ul>
	<li>Elk artikel kan slechts door één lid één maal worden besteld. Hij of zij kan wel meerdere stuks van dat artikel bestellen.</li>
	<li>Als een artikel in meerdere bestelperiodes besteld moet kunnen worden, moet het meerdere keren worden ingevoerd. Zorg ervoor dat het 'oude' artikel niet meer beschikbaar is.</li>
	<li>Alleen artikelen die via de zelfservice worden besteld krijgen een ordernummer. Bij artikelen met ordernummer kan het aantal besteld in 'Bestellingen muteren' niet worden aangepast.</li>
	<li>Als een lid iets heeft besteld, maar nog niet op definitief maken heeft geklikt, dan is de bestelling zichtbaar in 'Bestellingen muteren' met in de kolom 'Datum besteld' de letters ND. In de totalen wordt deze regel oek niet meegeteld.</li>
	<li>Als een lid op 'Bestelling definitief maken' klikt krijgt wordt er een bevestiging verstuurd. Hiervoor wordt gebruikt maakt van een mailing.</li>
	<li>Het veld 'Beschikbaar tot' zorgt ervoor dat een artikel tot een bepaalde datum in de zelfservice beschikbaar is.</li>
	<li>De datum in het veld 'Vervallen per' zorgt ervoor dat een artikel na deze datum helemaal niet meer beschikbaar of zichtbaar is.</li>
	<li>Als een artikel voor een beperkte groep beschikbaar is, geldt deze beperking alleen in de zelfservice. In 'Bestellingen muteren' geldt deze beperking niet.</li>
	</ul>
	
	<p>Een aantal zaken kunnen alleen door een beheerder van de website worden ingesteld. Deze opties zitten in de tab 'Instellingen' in het Admin-menu.</p>	
	<ul>
	<li>Welke mailing voor het bevestigingen van de bestelling moet worden gebruikt. Dit is de variabele 'mailing_bevestigingbestelling'.</li>
	<li>De zin die onderaan het bestelformulier in de zelfservice. Dit is de variabele 'zs_voorwaardenbestelling'.</li>
	</ul>

<?php
	} elseif ($currenttab == "Beheer") {
?>

<h1>Zaken voor beheerders</h1>

<h2>Minimale systeemeisen</h2>
De server waar je website op draait moet minimaal de volgende zaken beschikbaar hebbben.
<ul>
<li><a href='http://www.php.net'>PHP</a> 7.1.x</li>
<li><a href='http://www.mysql.com/'>MySQL</a>-server 5.5 of nieuwer of een gelijkwaardig alternatief, zoals MariaDB</li>
<li>Mogelijkheid om e-mails te kunnen versturen.</li>
</ul>

<h2>Alternatief voor MySQL</h2>
<p>MySQL kan ook vervangen worden door een compatibel product, deze alternatieven hebben vaak een betere performance. Het is voor mij niet doenlijk om deze allemaal te testen. Op MariaDB 5.5 is het pakket wel getest en daar werkt het prima op. Mocht je een foutmelding tegen komen op één van de alternatieven, dan kan je die uiteraard via GitHub melden. Graag met de SQL-code, die de fout veroorzaakt, deze code is vaak in de logging te vinden.</p>

<h2>Hoe tabellen uploaden?</h2>
Het uploaden de gegevens uit de Access-database naar de MySQL-database kan op verschillende manieren. Voor het pakket maakt het niet uit welke je kiest, zolang de beide structuren maar exact gelijk zijn.
<ul>
<li>Gebruik maken van <a href='http://software.telling.nl/#MSA2MySQL'>MSA2MySQL</a>. Dit tooltje kan onder andere vanuit een Access-database een export maken. In deze export staan SQL-statements die in een MySQL-database geïmporteerd kunnen worden. Voor dit importeren kan je gebruik maken van Uploaden data (Beheerdersmenu) of bijvoorbeeld <a href='http://www.phpmyadmin.net'>phpMyAdmin</a>.</li>
<li>Je kan gebruik maken van een sychronisatietool, zoals <a href='http://www.dbconvert.com/convert-access-to-mysql-pro.php'>DB Convert</a>.</li>
<li>Je kan zelf een export/import bouwen. De benodigde tabellen om deze gegevens in te importeren worden automatisch door phpRBM aangemaakt.</li>
</ul>

<h2>Welke gegevens uploaden?</h2>
De volgende tabellen kunnen vanuit de Access-database naar de MySQL-database ge-upload worden.
<ul>
<li>Bewaking</li>
<li>Bewseiz</li>
<li>Boekjaar</li>
<li>Diploma</li>
<li>Functie *</li>
<li>GBR</li>
<li>Groep</li>
<li>Lid *</li>
<li>Kostenplaats</li>
<li>Liddipl</li>
<li>Lidmaatschap</li>
<li>Lidond *</li>
<li>LidRedNed</li>
<li>Memo</li>
<li>Mutatie</li>
<li>Onderdl *</li>
<li>Rekening</li>
<li>Rekreg</li>
<li>Vereniging *</li>
</ul>
De tabellen met een sterretje moeten minimaal ge-upload worden. Deze zijn namelijk nodig om het inloggen en de autorisatie te regelen.
<p>Als de pasfoto's zichtbaar moeten zijn, dan moeten deze in de folder 'pasfoto' ge-upoad worden. De naamconventie is gelijk aan die van in de Access-database.</p>
<p>Mocht je bepaalde onderdelen (nog) niet gebruiken dan is het aan te raden om deze tabellen ook niet te uploaden.</p>

<h2>Gebruikersbeheer</h2>
<p>Het aanmaken van een login kan een gebruiker zelf regelen. Er zijn twee zaken waarbij actie van de webmaster noodzakelijk is.</p>
<ul>
<li>Een gebruiker wil een andere login. Hiervoor is de mogelijkheid gebouwd om een bestaand login te verwijderen (eerste kolom). Het betreffende lid kan dan zelf een nieuw login aanvragen.</li>
<li>Een login is geblokkeerd, omdat er te vaak foutief mee geprobeerd mee in te loggen. In Beheer logins kan een login gedeblokkeerd (laatste kolom) worden.</li>
</ul>

<h2>Autorisatiebeheer</h2>
<p>In 'Beheerdersmenu/Autorisatie zie je alle mogelijkheden (tabbladen) van het pakket. Per mogelijkheid moet je aangeven welke groep leden uit de Access-database wat mag. Indien je meerdere groepen tot hetzelfde tabblad toegang wilt geven kan je dit op twee manieren doen.</p>
<ul>
<li>Je kan onderaan het formulier het betreffende tabblad toevoegen en daaraan de tweede groep toevoegen. Deze optie is vooral handig als je een paar tabbladen heb waar meerdere groepen toegang toe moeten hebben.</li>
<li>Je kan in de Access-database een extra groep aanmaken, die je automatisch met de leden van de andere groepen vult. Voor meer uitleg zie de handleiding van de Access-database. Deze optie is vooral handig als je veel tabbladen voor meerdere groepen beschikbaar wilt maken.</li>
</ul>

<h2>Online wijzigingen lokaal verwerken</h2>
<p>Als gebruikers de mogelijkheid hebben om online wijzigingen door te voeren, dan moeten deze ook in de Access-database verwerkt worden. Hiervoor is de optie Downloaden wijzigingen gemaakt, deze optie is alleen zichtbaar als er wijzigingen te downloaden zijn. Download de wijzigingen als SQL-statements naar een bestand en importeer in het scherm Bestandsheer in de MS-Access database. Dit kan ook via een kopieer/plak actie. Het afmelden is zodat het pakket weet welke wijzigingen niet meer verwerkt hoeven te worden.</p>

<h2>Implementatie</h2>
<p>Hieronder volgt een lijstje van zaken die gedaan moeten worden cq. geregeld moeten worden bij de implementatie.</p>
<ul>
<li>Een URL en MySQL-database beschikbaar hebben. Voor de eisen, zie de minimale systeemeisen.</li>
<li>Bepalen welke onderdelen gebruiken gaan worden en dus welke tabellen ge&uuml;pload moeten worden.</li>
<li>Download het pakket, pak hem uit. Het pakket is te downloaden via <a href='https://github.com/stelling/phpRBM'>GitHub</a>. Upload alle bestanden naar een folder op je website.</li>
<li>Pas het bestand config.php aan. Vooral de inlog-gegevens voor de database moeten vanaf het begin goed staan.</li>
<li>Als de database ook voor andere zaken gebruikt wordt is het handig om een prefix voor de tabelnamen in te stellen. Zo weet je zeker dat er geen dubbele namen zijn. Ook staan ze dan allemaal bij elkaar.</li>
<li>Ga in een browser naar de root van de website. De eerste keer worden, mits de login-gegevens voor de database correct zijn, de benodigde tabellen automatisch aangemaakt. Ook wordt je gelijk naar de upload-mogelijkheid doorgestuurd.</li>
<li>Doe als eerste een upload. Voordat je een upload gedaan hebt, kan je feitelijk niets. Voor de eerste upload hoef je niet ingelogd te zijn.</li>
<li>Stukje schrijven met verenigingsinformatie voor in de tab Vereniging/Introductie. Dit stukje komt in de template 'verenigingsinfo.html'.</li>
<li>Middels autorisatiebeheer bepalen wie wat mag zien en/of doen.</li>
<li>De nieuwste versie van de Access-database ST-RBM moet in gebruik zijn. De website en database worden kwa databasestructuur met elkaar in lijn gehouden. Dus bij een upgrade: zorg dat je van beide de laatste hebt. Mocht je niet naar de laatste versie van de Access-database willen of kunnen upgraden, dan kan je ook de gegevens uit jou versie, voor het uploaden, importeren in een database, die wel de laatste versie heeft. Andersom kan helaas niet.</li>
<li>Optioneel: foto's van kaderleden updaten en aanvullen. Deze worden namelijk zichtbaar op de website.</li>
<li>Bepaal wie het beheer gaat doen, dit kunnen ook meerdere mensen zijn.</li>
<li>Optioneel: maak een eigen stylesheet om de website de look en feel van de vereniging te geven. Er zijn twee stylesheets, default.css en kleur.css. De gedachte is dat de default ongewijzigd gelaten wordt. Alle aanpassingen kunnen in kleur.css gedaan worden. Deze heet kleur, omdat dit voornamelijk aanpassingen in de kleuren zullen zijn. Het is technisch geen probleem om ook andere zaken, bijvoorbeeld de breedte van het scherm, hier aan te passen.</li>
<li>Maak een templates voor briefpapier en bevestigingsmail van de login.</li>
</ul>

<h2>Config.php</h2>
<p>In config.php kunnen de nodige zaken ingesteld worden. Een aantal zaken die hierin geregeld worden zijn:</p>
<ul>
<li>De encrypt_key voor de opslag van wachtwoorden. Wijzig deze gelijk, want later kan dit niet meer.</li>
<li>Host, login, wachtwoord van de MySQL-server en SMTP-server</li>
<li>Prefix voor tabelnamen, deze prefix moet op een underscore eindigen</li>
<li>De webmasters</li>
</ul>
<p>Verder is er in het beheerdersmenu een tab met instellingen beschikbaar. Hierin kunnen nog meer zaken worden ingesteld.</p>

<h2>Up-to-date houden van het pakket</h2>
<p>Er worden updates van dit pakket via <a href='https://github.com/'>GitHub</a> aangeboden. Er is (nog) geen automatische update of melding van een update beschikbaar. Wel is dit eenvoudig zelf te controleren. Rechtsonder op elke pagina staat de versie die je geïnstalleerd hebt. Bij de <a href='https://github.com/stelling/phpRBM/commits/master'>commits op GitHubs</a> kan je zien wat er na die datum is gewijzigd.
De beheerder kan dan beslissen of deze verbeteringen de moeite waard zijn om de updaten. Om de update uit te voeren download je het pakket in zijn geheel en upload je deze naar je webruimte. Indien nodig worden tabellen in de database automatisch aangepast. Kenners van GitHub kunnen ook die mogelijkheden voor updaten gebruiken, daarvoor verwijs ik naar de documentatie van GitHub zelf.</p>
<p>Net als bij ST-RBM is het aan te raden om om maximaal de versie niet ouder dan een jaar te laten worden. Er zit in phpRBM geen blokkade, maar om zeker te weten dat alle conversies het nog doen is dit zeker aan te raden.</p>

<h2>Database onderhoud</h2>
<p>In het admin-menu zit een tabblad 'Onderhoud'. Hierin kan een backup worden gemaakt. Ook kunnen diverse tabellen worden opgeschoond. Per onderdeel staat een korte uitleg vermeldt.</p>

<?php
	} else {
?>

<h1>Handleiding phpRBM</h1>

<h2>Introductie</h2>
<p>phpRBM is een website die gekoppeld is aan de <a href='http://software.telling.nl'>MS-Access database ST-RBM</a>. De gedachte achter het pakket is dat voor een aantal zaken het wel erg handig is, als deze online beschikbaar zijn. Bij phpRBM worden de meeste gegevens in de Access-database bijgehouden. Vervolgens worden deze gegevens naar de MySQL-database achter de website ge-upload.</p>

<h2>Welke mogelijkheden zijn er?</h2>
<p>Het pakket heeft de volgende mogelijkheden. Per mogelijkheid kan besloten worden om deze helemaal niet te gebruiken, voor een beperkte groep beschikbaar te maken of voor alle leden beschikbaar te stellen.</p>
<ul>
<li>Leden kunnen hun eigen gegevens bekijken. Inclusief afdelingen, rollen, diploma's, bewakingen, rekeningen, evenementen en ontvangen mailings.</li>
<li>Er zijn overzichten beschikbaar waarbij kaderleden, ereleden, leden van verdienste, allen eventueel met pasfoto, getoond kunnen worden.</li>
<li>Verjaardagen van leden kunnen getoond worden.</li>
<li>De complete ledenlijst met alle details per lid is beschikbaar.</li>
<li>Er kunnen mailings verstuurd worden, gebasseerd op groepen uit de database. Hierbij kan ook mailmerge toegepast worden.</li>
<li>Rekeningen kunnen per e-mail worden verzonden.</li>
<li>Het bewakingsrooster kan op verschillende manieren bekeken worden, ook kunnen hierbij de pasfoto's getoond worden.</li>
<li>Er kan een kostenoverzicht gemaakt worden, zodat commissies zelf kunnen kijken wat er op hun kostenplaats geboekt is en wat de laatste stand van zaken is.</li>
<li>Mogelijkheid voor leden hun eigen gegevens online te wijzigen. Het gaat hierbij om algemene gegevens, behaalde diploma, het kunnen uploaden van een (nieuwe) pasfoto en opzeggen van het lidmaatschap.</li>
<li>Muteren van een beperkt aantal gegevens, o.a. persoonlijke gegevens, diploma's en pasfoto.</li>
<li>Interface om de online wijzigingen in de Access-database te verwerken.</li>
<li>Online leden in laten schrijven voor een bewaking.</li>
<li>Beheren van evenementen, inclusief online inschrijving.</li>
<li>Beperkte webshop voor leden, zonder betalingsmogelijkheid.</li>
</ul>

<h2>Hoe werkt het voor een gebruiker?</h2>
<p>Om te beginen heeft een gebruiker een moderne browser nodig, het pakket is getest op <a href='http://www.mozilla.com/nl/firefox/'>Firefox >= 61</a> en <a href='http://www.google.com/chrome/'>Chrome >= 67</a>. Andere en/of oudere versies waarschijnlijk werken ook wel, maar een aantal zaken zien er minder fraai uit en daar heb ik de website niet in getest. Verder moet elke gebruiker een login aanvragen, dit kan op basis van het in database bekende e-mailadres. Hierna wordt de login per e-mail opgestuurd.<p>

<h2>Aanpassen aan de specifieke wensen van een reddingsbrigade</h2>
Elke vereniging heeft haar eigen huisstijl. Dit is prima inpasbaar in dit pakket gemaakt. Er zijn de volgende opties.
<ul>
<li>De opmaak wordt middels stylesheets geregeld.</li>
<li>Er zijn templates beschikbaar, voor meer uitleg zie het tabblad 'Templates'.</li>
<li>Onder andere de titel van de website en naam van de vereniging kunnen aangepast worden.</li>
</ul>

<h2>Ondersteuning</h2>
<p>Dit pakket is met veel plezier in mijn vrije tijd ontwikkeld en dus geldt hiervoor dezelfde ondersteuning als voor de Access-database. Officieel kan ik het niet garanderen, maar ik probeer iedereen altijd te helpen. Vragen, opmerkingen en verzoeken graag via <a href='https://github.com/stelling/phpRBM/issues'>GitHub</a>.</p>

<?php
	}
?>
	</div>  <!-- Einde handleiding -->

	<div id='opdrachtknoppen'><img src='images/back.png' alt='Terug' title='Terug' onclick='history.go(-1);'></div>  <!-- Einde opdrachtknoppen -->
	
	<div class='clear'></div>
<div id='footer'>
	<div id='footerleft'>&copy;&nbsp;<a href='http://www.telling.nl/' target='_top'>S. telling</a></div>
	<a href='http://http://phprbm.telling.nl/uitleg.php'>phpRBM</a>
	<div id='footerright'><? echo(strftime("%e %B %Y", filectime($_SERVER['SCRIPT_FILENAME']))); ?></div>
</div>  <!-- Einde footer  -->

</div>  <!-- Einde container -->
</body>
</html>
