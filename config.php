<?php

$db_host=''; // Server (host) naam van de MariaDB-server.
$db_name=''; // Gebruikersnaam voor MariaDB.
$db_user=''; // Deze gebruiker moet onder andere rechten hebben om tabellen te mogen aanmaken en te verwijderen.
$db_pass=''; // Wachtwoord wat bij de gebruikersnaam van MariaDB hoort. Het standaard wachtwoord van webmasters is gelijk aan dit wachtwoord.

$table_prefix = "rbm_"; // Zorg dat je bij het uploaden van de gegevens uit MS-Access dezelfde prefix gebruikt.
$lididwebmasters = array(0); // Dit zijn de interne nummers (RecordID's) van de webmasters. Bij meerdere webmaster: scheiden met een komma.
$lididtestusers = array(0); // Dit zijn interne nummers (RecordID's) van het testgebruikers. Bij meerdere testgebruikers: scheiden met een komma.
$salt2FA = ""; // Als je Two Factor Authentication wilt gebruiken vul dan hier een ongeveer 40 karakters lange code in.

$smtphost = ""; // De naam van de SMTP-server voor het versturen van e-mails. Indien deze niet wordt ingevuld, wordt van de mail-functie uit PHP gebruik gemaakt.
$smtpport = 0; //De poort die voor de SMTP-host gebruikt moet worden. 0 = gebruik default poort.
$smtpuser = ""; //De gebruiker om te kunnen inloggen op de SMTP-server.
$smtppw = ""; //Het wachtwoord dat bij de SMTP-user hoort, om te kunnen inloggen op de SMTP-server.

$httpsverplicht = 1; // Als waarde is 1 dan kan je alleen via https deze website bereiken.

$lididwebmasters = array(0);

$smtphost = "";
$smtpport = 465;
$smtpuser = "";
$smtppw = "";

?>
