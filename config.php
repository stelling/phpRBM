<? 
// Gegevens om toegang te krijgen tot de MySQL-database. 
$db_host='';
$db_name='';
$db_user='';  // Deze gebruiker moet onder andere rechten hebben om tabellen te mogen aanmaken en te verwijderen. 
$db_pass='';
$db_folderbackup = "";  // Deze variabele is optioneel. Mocht deze niet ingevuld worden, dan wordt de standaard folder gebruikt.
$db_backupsopschonen = 11;  // Na hoeveel dagen moeten oude back-ups automatisch verwijderd worden? 0 = nooit.
$db_backuptarren = 0;  // Moet de backup gecomprimeerd worden? Let op, de webhost moet dit wel ondersteunen.
$table_prefix = "rbm_"; // Zorg dat je bij het uploaden van de gegevens uit MS-Access dezelfde prefix gebruikt.

// Authorisation data
$lididwebmasters = array(1);  // Dit is het interne nummer (RecordID) van het lid. Bij meerdere webmaster: scheiden met een komma.
$encript_key = "deze_wijzigen"; // Deze wordt gebruikt om de encryptie van de wachtwoorden te doen. Wijzig deze na de eerste installatie niet meer. Als je dit toch doet zijn alle wachtwoorden gewijzigd en moet iedereen een nieuwe login aanvragen.
$beperktotgroep = 0;  // Als je hier de RecordID van een groep (zie tabel ONDERDL) in vult, krijgt alleen deze groep toegang tot deze website. Vul 0 in als alle personen uit de database toegang moeten krijgen.
$lidnrnodigbijloginaanvraag = 0;  // Moet een lid zijn of haar lidnummer opgeven als er een login aangevraagd wordt?
$lidnrversturenmogelijk = 0;  // Hierbij geef je aan of het mogelijk moet zijn om vanaf deze website op basis van alleen een e-mailadres iemand zijn lidnummer per e-mail opgestuurd kan worden.

// General data
$naamwebsite = "phpRBM";  // Dit is de naam zoals deze in de titel en op elke pagina getoond wordt.
$urlwebsite = $_SERVER["HTTP_HOST"];   // Zonder http://
$naamvereniging = "";
$urlvereniging = "";   // Zonder http://
$emailwebmaster = "";
$emailledenadministratie = "ledenadm@trb.nu";
$daysshowbirthdays = 3;  // Het aantal dagen dat de verjaardagen vooruit getoond moeten worden.
$bewaartijdlogins = 6;  // Het aantal maanden dat niet gebruikte logins bewaard worden. 0 = altijd bewaren.
$kaderoverzichtmetfoto = 1;  // Moeten op het kaderoverzicht foto's getoond worden? (1 = ja, 0 = nee)
$rsswieiswie = 0;  // Moet er een rss van wie-is-wie gemaakt worden? Deze kan gebruikt worden om deze op een andere website te tonen, echter heeft deze geen beveiliging.
$scriptbijuitloggen = "";  // Dit script wordt automatisch gedraaid nadat iemand is uitgelogd.

//Mailingsmodule
$emailsecretariaat = "";  // Dit veld is niet verplicht, dit wordt gebruikt om het secretariaat op de hoogte te houden van verstuurde mailingen en opzeggingen.
$smtphost = "";
$smtpuser = "";
$smtppw = "";
$bewaartijdmailings = 3;  // Het aantal maanden dat verwijderde mailing bewaard worden. 0 = altijd bewaren.
$beperkfrom = "";  // Indien deze is ingevuld moet het from adres altijd vanaf dit domein zijn.
$max_grootte_bijlage = 0 * 1024 * 1024;  // Optioneel veld. Als je niets specificeerd dan is 2MB het maximum.  De waarde is in bytes.
$resultaatmailingversturen = 1; // Als hier een 1 staat wordt naar de ontvanger en het secretariaat een mail met het resultaat van deze mail verzonden.

// Self-service voor leden
$selfservicediplomas = "('ZB-A', 'ZB-B', 'ZS-A', 'ZS-B', 'ZIAb', 'ZIBb', 'ZIAs', 'ZIBs', 'EM', 'EHBO', 'BIG', 'RIJB', 'VA1', 'VA2', 'VB-O', 'VB-S', 'VS-O', 'VS-S')";  // Vul in deze lijst de codes in vam diploma's die leden zelf mogen wijzigen.
$emailnieuwepasfoto = "";
$opzegtermijn = 1;  // De opzegtermijn van de vereniging in maanden.
$muteerbarememos = "DG";   // Welke soorten memo's moeten leden zelf kunnen muteren?
$emailbevestiginginschrijving = "";   // Vanaf welk e-mailadres moet de bevestiging van de inschrijving voor de bewaking verzonden worden.
$voorwaardeninschrijving = "Met deze inschrijving verklaar je akkoord te zijn met de voorwaarden en verklaar je jezelf competent voor de bewaking.";  // Deze regel wordt bij de inschrijving vemeld als voorwaarde voor de inschrijving voor de bewaking.

?>
