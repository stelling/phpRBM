<? 
// *** Standaard config.php voor GitHub ***

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
$beperktotgroep = array();  // Vul hier de RecordID's van de groepen (zie tabel ONDERDL) in die toegang hebben. Als je geen groepen invult hebben alleen webmasters toegang.
$lidnrnodigbijloginaanvraag = 0;  // Moet een lid zijn of haar lidnummer opgeven als er een login aangevraagd wordt?
$lidnrversturenmogelijk = 0;  // Hierbij geef je aan of het mogelijk moet zijn om vanaf deze website op basis van alleen een e-mailadres iemand zijn lidnummer per e-mail opgestuurd kan worden.
$maxinlogpogingen = 4; // Na hoeveel foutieve inlogpogingen moet het account geblokkeerd worden? (0 = nooit)
$maxlengtelogin = 12;  // De maximale lengte die een login mag zijn.

// General data
$naamwebsite = "phpRBM";  // Dit is de naam zoals deze in de titel en op elke pagina getoond wordt.
$urlwebsite = $_SERVER["HTTP_HOST"];   // Zonder http://
$naamvereniging = "";
$urlvereniging = "";   // Zonder http://
$emailwebmaster = "";
$emailledenadministratie = "ledenadm@";
$daysshowbirthdays = 3;  // Het aantal dagen dat de verjaardagen vooruit getoond moeten worden.
$numbershowbirthdays = 5;  // Het maximaal aantal verjaardagen getoond moeten worden. Als er meerdere leden op dezelfde dag jarig zijn, wordt deze dag wel compleet getoond. 
$bewaartijdlogins = 6;  // Het aantal maanden dat niet gebruikte logins bewaard worden. 0 = altijd bewaren.
$kaderoverzichtmetfoto = 1;  // Moeten op het kaderoverzicht foto's getoond worden? (1 = ja, 0 = nee)
$rsswieiswie = 0;  // Moet er een rss van wie-is-wie gemaakt worden? Deze kan gebruikt worden om deze op een andere website te tonen, echter heeft deze geen beveiliging.
$tonentoekomstigebewakingen = 1; // Moeten bij de gegevens van een lid ook toekomstige bewakingen getoond worden (1 = ja, 0 = alleen historie)
$toneninschrijvingenbewakingen = 1; // Moeten bij de gegevens van een lid ook inschrijvingen voor bewakingen getoond worden (1 = ja, 0 = nee)
$scriptbijuitloggen = "";  // Dit script wordt automatisch gedraaid nadat iemand is uitgelogd.
$typemenu = 3; // 1 = per niveau een aparte regel, 2 = één menu met dropdown, 3 = één menu met dropdown en extra menu voor niveau 2.
$bewaartijdlogging = 13; // Hoelang in maanden moet logging bewaard blijven. 0 = altijd.
$bewaartijdinloggen = 6; // Hoelang in maanden moet logging van het in- en uitloggen bewaard blijven. 0 = gelijk aan bewaartijdlogging.

//Mailingsmodule
$emailsecretariaat = "";  // Dit veld is niet verplicht, dit wordt gebruikt om het secretariaat op de hoogte te houden van verstuurde mailingen en opzeggingen.
$smtphost = "";
$smtpport = 0;  // De poort die voor de SMTP-host gebruikt moet worden.
$smtpuser = "";
$smtppw = "";
$bewaartijdmailings = 3;  // Het aantal maanden dat verwijderde mailing bewaard worden. 0 = altijd bewaren.
$beperkfrom = "";  // Indien deze is ingevuld moet het from adres altijd vanaf dit domein zijn.
$max_grootte_bijlage = 0 * 1024 * 1024;  // Optioneel veld. Als je niets specificeerd dan is 2MB het maximum.  De waarde is in bytes.
$resultaatmailingversturen = 1; // Als hier een 1 staat wordt naar de ontvanger en het secretariaat een mail met het resultaat van deze mail verzonden.
$maxmailsperminuut = 250;  // Het maximaal aantal e-mails dat via een mailing per minuut verzonden mag worden.  0 = onbeperkt.

// Self-service voor leden
$selfservicediplomas = "('ZB-A', 'ZB-B', 'ZS-A', 'ZS-B')";  // Vul in deze lijst de codes in vam diploma's die leden zelf mogen wijzigen.
$emailnieuwepasfoto = "";
$opzegtermijn = 1;  // De opzegtermijn van de vereniging in maanden.
$muteerbarememos = array('WN', 'D', 'G');   // Welke soorten memo's moeten leden zelf kunnen muteren?
$emailbevestiginginschrijving = "";   // Vanaf welk e-mailadres moet de bevestiging van de inschrijving voor de bewaking verzonden worden.
$voorwaardeninschrijving = "Met deze inschrijving verklaar je akkoord te zijn met de voorwaarden en verklaar je jezelf competent voor de bewaking.";  // Deze regel wordt bij de inschrijving vemeld als voorwaarde voor de inschrijving voor de bewaking.
$emailbevestigingbestelling = "";   // Vanaf welk e-mailadres moet de bevestiging van een bestelling verzonden worden.
$voorwaardenbestelling = "De kosten van een bestelling worden middels een automatische incasso afgeschreven.";  // Deze regel wordt bij de bestellingen in de zelfservice vermeld.
$termijnvervallendiplomasmailen = 0;  // Hoeveel maanden vooruit moeten leden een herinnering krijgen als een diploma gaat vervallen. 0 = geen herinnering sturen.

?>
