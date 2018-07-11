<?php

$db_host=''; // Server (host) naam van de MySQL-server.
$db_name=''; // Gebruikersnaam voor de MYSQL-server
$db_user=''; // Deze gebruiker moet onder andere rechten hebben om tabellen te mogen aanmaken en te verwijderen.
$db_pass=''; // Wachtwoord wat bij de gebruikersnaam van MySQL hoort. Het standaard wachtwoord van webmasters is gelijk aan dit wachtwoord.
$table_prefix = "rbm_"; // Zorg dat je bij het uploaden van de gegevens uit MS-Access dezelfde prefix gebruikt.
$lididwebmasters = array(1); // Dit is het interne nummer (RecordID) van het lid. Bij meerdere webmaster: scheiden met een komma.

$smtphost = ""; // De naam van de SMTP-server voor het versturen van e-mails. Indien deze niet wordt ingevuld, wordt van de mail-functie uit PHP gebruik gemaakt.
$smtpport = 0; //De poort die voor de SMTP-host gebruikt moet worden. 0 = gebruik default poort.
$smtpuser = ""; //De gebruiker om te kunnen inloggen op de SMTP-server.
$smtppw = ""; //Het wachtwoord dat bij de SMTP-user hoort, om te kunnen inloggen op de SMTP-server.

?>
