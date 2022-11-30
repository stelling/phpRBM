<?php

date_default_timezone_set('Europe/Amsterdam');
setlocale(LC_ALL,'nl_NL.utf8');
$dtfmt = new IntlDateFormatter(
    'nl_NL',
    IntlDateFormatter::LONG,
    IntlDateFormatter::LONG,
    'Europe/Amsterdam'
);

define("BASEDIR", $_SERVER['DOCUMENT_ROOT']);
define("ARRGESLACHT", ["O" => "Onbekend", "V" => "Vrouwelijk", "M" => "Mannelijk", "N" => "Neutraal", "B" => "Bedrijf/Vereniging/Stichting"]);
define("ARRTL", ["Alle", "Leden", "Klosleden", "Voormalig leden"]);
define("ARRLEGITIMATIE", ["G" => "Geen", "I" => "Identiteitskaart", "P" => "Paspoort", "R" => "Rijbewijs"]);
define("ARRSOORTMEMO", ["A" => "Algemeen", "B" => "Bewaking", "D" => "Dieet", "E" => "Examen", "F" => "Financiën", "G" => "Gezondheid/medisch", "I" => "Inschrijving bewaking"]);
define("ARRTYPEONDERDEEL", ["A" => "Afdeling", "B" => "Bestuur", "C" => "Commissie", "E" => "Eigenschap", "F" => "Functionarissen", "G" => "Groep", "O" => "Onderscheiding", "R" => "Rol", "T" => "Toestemming"]);
define("ARRTYPEDIPLOMA", ["B" => "Brevet", "C" => "Competentie", "D" => "Diploma", "I" => "Insigne", "L" => "Licentie", "M" => "Module", "P" => "PvB"]);
define("PASFOTOEXTENTIES", ["gif", "jpg", "jpeg", "png"]);
define("ARRTYPEEDITOR", [1 => "CKEditor 5 Classic editor", 2 => "CKEditor 4", 3 => "TinyMCE"]);
define("ARRSOORTEVENEMENT", ["B" => "Brigade-activiteit", "G" => "Gezellig/sociaal", "O" => "Opleiding", "V" => "Vergadering/overleg", "W" => "Wedstrijd"]);
define("ARRDLNSTATUS", ["A" => "Afgewezen", "B" => "Bevestigd", "I" => "Ingeschreven", "J" => "Aangemeld", "N" => "Niet geweest", "R" => "Reserve", "T" => "Onder voorbehoud", "X" => "Afgemeld/Afgezegd"]);
define("ARRPRESENTIESTATUS", ["A" => "Aangemeld/Aanwezig", "L" => "Aanwezig, maar te laat", "H" => "Aangemeld, door vereniging afgezegd", "N" => "Niet aanwezig (onafgemeld)", "R" => "Afwezig met bekende reden", "T" => "Onder voorbehoud", "V" => "Les is vervallen", "X" => "Afgemeld zonder bekende reden", "Z" => "Ziek"]);
define("ARRTYPEBACKUP", [1 => "phpRBM-tabellen", 2 => "Tabellen uit MS-Access", 3 => "Beide"]);
define("ARRTYPESTUK", ["A" => "Algemene Vergadering", "B" => "Beleidsstuk", "F" => "Formulier", "H" => "Handleiding", "P" => "Promotiemateriaal", "R" => "Regeling", "V" => "Verklaring"]);
define("ARRTYPEMENU", [1 => "per niveau een aparte regel", 2 => "&eacute;&eacute;n menu met dropdown", 3 => "&eacute;&eacute;n menu met dropdown en extra menu voor niveau 2"]);
define("BEWAARTIJDNIEUWERECORDS", 120);

define("BASE64_TOEVOEGEN", "data:image/jpg;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAACDElEQVR42u2Xy2rCQBRAZxQfjWAE0YUiKN2ooLh200K39QtK/Y0u1WV/o6Vf0G4LdtO1IuhKEAVRQTEqEXz23vgg0eZRSaqLXLg4k4njMblzMqHkD8EwDIlEInGbzfYM3TvIq+3QFPJzsVg8NRqN6mQy0Twn1XwipSQWi8Xtdvs3dFmZ0ziASNdqtepyudQXwOPxkHA4/A7Ne8gV5NtqtarjmMViuYaPB2xCfrRarUy/39cXwO/3k0AgwJPNZX9tt9vZXq8njHm9XhIKhV6g+Qg57XQ6DKQhAOttN18ulwvr9Xo/nkwmc3Al8tiGH6e6A/h8PhIMBvcApVKpIB5PJBI5q9VqApgApwFA9QrrHCQjeyJa0O12awIAE1IlG87nczIcDgnKiiroVSkUAVRCom0ah1DRq94AuxC0TVOp1JFexYKRiWKlUvkSH4D/cQMAt0pfwufJobYR4Fe9GhWH2kYAWb0aFWJtXwTA/hbA2s12u11iFATWAPoEHmqSWyApQsi6hrmKsAokRQirQLUItyEtQtzlOByOsyxDEFKaOp1OQUQA8a8ims1mGxHhEbw3oFlFFbtcLtS1JhWPRiM6Ho9l50IVcxwn1NplPIxMABPgFIBzbcuPtL0bE+vVkBcTlmVRWHLalui12WxmBoOBvgAoq2g0qqZtQa/4cgobG30BMBS0LdErz/Oa5/wBboikeyFGSbsAAAAASUVORK5CYII=");
define("BASE64_VERWIJDER", "data:image/jpg;base64,AAABAAEAICAAAAEAIACoEAAAFgAAACgAAAAgAAAAQAAAAAEAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIBCgICAxICAgOyAgIDugICA7oCAgO6AgIDugICA7oCAgO6AgIDugICA7oCAgO6AgIDugICA7oCAgO6AgIDugICA7oCAgOyAgIDKgICATAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAQoCAgPqAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID8gICAUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIDIgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDYgICAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgPSAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIAAgICA+ICAgP+AgID/gICA/4CAgPyAgIDsgICA7ICAgPiAgID/gICA+oCAgOyAgIDsgICA+ICAgP+AgID4gICA7ICAgOyAgID8gICA/4CAgP+AgID/gICA/4CAgAwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgAKAgID4gICA/4CAgP+AgID/gICA6gAAAAAAAAAAgICApICAgP+AgIDGAAAAAAAAAACAgIC2gICA/4CAgLQAAAAAAAAAAICAgNqAgID/gICA/4CAgP+AgID/gICADgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgPqAgID/gICA/4CAgP+AgIDqAAAAAAAAAACAgICkgICA/4CAgMYAAAAAAAAAAICAgLaAgID/gICAtAAAAAAAAAAAgICA2oCAgP+AgID/gICA/4CAgP+AgIAOAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIACgICA+oCAgP+AgID/gICA/4CAgOoAAAAAAAAAAICAgKSAgID/gICAxgAAAAAAAAAAgICAtoCAgP+AgIC0AAAAAAAAAACAgIDagICA/4CAgP+AgID/gICA/4CAgBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgAKAgID8gICA/4CAgP+AgID/gICA6gAAAAAAAAAAgICApICAgP+AgIDGAAAAAAAAAACAgIC2gICA/4CAgLQAAAAAAAAAAICAgNqAgID/gICA/4CAgP+AgID/gICAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgPyAgID/gICA/4CAgP+AgIDqAAAAAAAAAACAgICkgICA/4CAgMYAAAAAAAAAAICAgLaAgID/gICAtAAAAAAAAAAAgICA2oCAgP+AgID/gICA/4CAgP+AgIAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIACgICA/ICAgP+AgID/gICA/4CAgOoAAAAAAAAAAICAgKSAgID/gICAxgAAAAAAAAAAgICAtoCAgP+AgIC0AAAAAAAAAACAgIDagICA/4CAgP+AgID/gICA/4CAgBIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgAKAgID8gICA/4CAgP+AgID/gICA6gAAAAAAAAAAgICApICAgP+AgIDGAAAAAAAAAACAgIC2gICA/4CAgLQAAAAAAAAAAICAgNqAgID/gICA/4CAgP+AgID/gICAEgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgPyAgID/gICA/4CAgP+AgIDqAAAAAAAAAACAgICkgICA/4CAgMYAAAAAAAAAAICAgLaAgID/gICAtAAAAAAAAAAAgICA2oCAgP+AgID/gICA/4CAgP+AgIASAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIACgICA/ICAgP+AgID/gICA/4CAgOoAAAAAAAAAAICAgKSAgID/gICAxgAAAAAAAAAAgICAtoCAgP+AgIC0AAAAAAAAAACAgIDagICA/4CAgP+AgID/gICA/4CAgBIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgAKAgID8gICA/4CAgP+AgID/gICA6gAAAAAAAAAAgICApICAgP+AgIDGAAAAAAAAAACAgIC2gICA/4CAgLQAAAAAAAAAAICAgNqAgID/gICA/4CAgP+AgID/gICAEgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgPyAgID/gICA/4CAgP+AgIDqAAAAAAAAAACAgICkgICA/4CAgMYAAAAAAAAAAICAgLaAgID/gICAtAAAAAAAAAAAgICA2oCAgP+AgID/gICA/4CAgP+AgIASAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIACgICA/ICAgP+AgID/gICA/4CAgOoAAAAAAAAAAICAgKSAgID/gICAxgAAAAAAAAAAgICAtoCAgP+AgIC0AAAAAAAAAACAgIDagICA/4CAgP+AgID/gICA/4CAgBIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgAKAgID8gICA/4CAgP+AgID/gICA6gAAAAAAAAAAgICApICAgP+AgIDGAAAAAAAAAACAgIC2gICA/4CAgLQAAAAAAAAAAICAgNqAgID/gICA/4CAgP+AgID/gICAEgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgPyAgID/gICA/4CAgP+AgIDqAAAAAAAAAACAgICkgICA/4CAgMYAAAAAAAAAAICAgLaAgID/gICAtAAAAAAAAAAAgICA2oCAgP+AgID/gICA/4CAgP+AgIASAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIACgICA/ICAgP+AgID/gICA/4CAgPCAgIBYgICAWICAgMSAgID/gICA2oCAgFiAgIBYgICA0ICAgP+AgIDOgICAWICAgFiAgIDmgICA/4CAgP+AgID/gICA/4CAgBIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgAKAgID8gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICAEgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgPyAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIASAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIACgICA/ICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgBIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgACAgIBUgICAVICAgFSAgIBUgICAVICAgFSAgIBUgICAVICAgFSAgIBUgICAVICAgFSAgIBUgICAVICAgFSAgIBUgICAVICAgFSAgIBUgICAVICAgFSAgIBUgICABgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgDaAgIBqgICAaoCAgGqAgIBqgICAaoCAgGqAgIBqgICAaoCAgGqAgIBqgICAaoCAgGqAgIBqgICAaoCAgGqAgIBqgICAaoCAgGqAgIBqgICAaoCAgGqAgIBqgICAaoCAgGqAgIBqgICAaoCAgDwAAAAAAAAAAAAAAAAAAAAAgICAfoCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICAjgAAAAAAAAAAAAAAAAAAAACAgIB+gICA/ICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgPyAgICOAAAAAAAAAAAAAAAAAAAAAICAgAKAgIAggICA9oCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID6gICALoCAgAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIAcgICAOICAgDyAgIA+gICAQICAgFyAgID/gICA/4CAgJaAgIBAgICAQICAgECAgIBAgICAQICAgECAgICIgICA/4CAgP+AgIBmgICAQICAgECAgIBAgICAQICAgCQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICACICAgPSAgID/gICA3oCAgJaAgICSgICAkoCAgJSAgICWgICAmoCAgNaAgID/gICA+oCAgBIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAjoCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgICgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIAEgICAgICAgNaAgIDmgICA6oCAgOyAgIDugICA7ICAgOiAgIDagICAiICAgAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA/gAAf/wAAD/4AAAf+AAAH/gAAB/4MYwf+DGMH/gxjB/4MYwf+DGMH/gxjB/4MYwf+DGMH/gxjB/4MYwf+DGMH/gxjB/4MYwf+DGMH/gxjB/4AAAf+AAAH/gAAB///////////+AAAAPgAAAD8AAAD//H4///wAP//8AD///gB/8=");
define("BASE64_VERWIJDER_KLEIN", "data:image/jpg;base64,R0lGODlhCQALAJEAAP///8zMzAAAAAAAACH5BAAAAAAALAAAAAAJAAsAAAIXBIRoyRkPA4qwtSOzWFpyoX0hFm2HBRQAOw==");
define("BASE64_DETAILS", "data:image/jpg;base64,AAABAAEAICAAAAEAIACoEAAAFgAAACgAAAAgAAAAQAAAAAEAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICANoCAgHyAgIB+gICAfoCAgH6AgIB+gICAfoCAgH6AgIB+gICAfoCAgH6AgIB+gICAfoCAgH6AgIB+gICAfoCAgH6AgIB+gICAfoCAgH6AgIB+gICAfoCAgH6AgIB+gICAfoCAgH6AgIB+gICAfoCAgH6AgIB+gICAfoCAgDSAgIDYgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA4ICAgOSAgID/gICA9ICAgOqAgIDsgICA/4CAgP+AgIDygICA6oCAgOqAgIDqgICA6oCAgOqAgIDqgICA6oCAgOqAgIDqgICA6oCAgOqAgIDqgICA6oCAgOqAgIDqgICA6oCAgOqAgIDqgICA6oCAgOqAgIDqgICA9ICAgP+AgIDugICA5oCAgP+AgIB8AAAAAICAgASAgID/gICA/4CAgGgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB8gICA/4CAgO6AgIDogICA/4CAgHwAAAAAgICABICAgP+AgID/gICAaAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgHyAgID/gICA7oCAgOqAgID/gICA0ICAgKSAgICmgICA/4CAgP+AgIDIgICApICAgKSAgICkgICApICAgKSAgICkgICApICAgKSAgICkgICApICAgKSAgICkgICApICAgKSAgICkgICApICAgKSAgICkgICApICAgKSAgICkgICA0ICAgP+AgIDugICA7ICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgO6AgIDsgICA/4CAgOqAgIDWgICA1oCAgP+AgID/gICA5oCAgNaAgIDWgICA1oCAgNaAgIDWgICA1oCAgNaAgIDWgICA1oCAgNaAgIDWgICA1oCAgNaAgIDWgICA1oCAgNaAgIDWgICA1oCAgNaAgIDWgICA1oCAgOqAgID/gICA7oCAgOyAgID/gICAfAAAAACAgIAEgICA/4CAgP+AgIBoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAfICAgP+AgIDugICA7ICAgP+AgIB8AAAAAICAgASAgID/gICA/4CAgGgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB8gICA/4CAgO6AgIDugICA/4CAgNyAgIC6gICAvICAgP+AgID/gICA1oCAgLqAgIC6gICAuoCAgLqAgIC6gICAuoCAgLqAgIC6gICAuoCAgLqAgIC6gICAuoCAgLqAgIC6gICAuoCAgLqAgIC6gICAuoCAgLqAgIC6gICAuoCAgNyAgID/gICA7oCAgO6AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDugICA7oCAgP+AgIDcgICAtICAgLKAgID/gICA/4CAgNCAgICwgICAsICAgLCAgICygICAsoCAgLKAgICygICAsoCAgLKAgICygICAsoCAgLSAgIC0gICAtICAgLSAgIC2gICAtoCAgLaAgIC2gICAuICAgLiAgIDcgICA/4CAgO6AgIDugICA/4CAgIIAAAAAgICABICAgP+AgID/gICAaAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgISAgID/gICA7oCAgO6AgID/gICAgAAAAACAgIAEgICA/4CAgP+AgIBoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAhICAgP+AgIDugICA7oCAgP+AgIDqgICA0ICAgNCAgID/gICA/4CAgOKAgIDQgICA0ICAgNCAgIDSgICA0oCAgNKAgIDSgICA0oCAgNKAgIDSgICA0oCAgNSAgIDUgICA1ICAgNSAgIDWgICA1oCAgNaAgIDWgICA2ICAgNiAgIDsgICA/4CAgO6AgIDugICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA7oCAgO6AgID/gICAyoCAgJqAgICcgICA/4CAgP+AgIDCgICAmoCAgJqAgICagICAmoCAgJqAgICagICAmoCAgJqAgICagICAmoCAgJqAgICagICAmoCAgJqAgICagICAmoCAgJqAgICagICAmoCAgJqAgICagICAyoCAgP+AgIDugICA7oCAgP+AgIB8AAAAAICAgASAgID/gICA/4CAgGgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB8gICA/4CAgO6AgIDugICA/4CAgHwAAAAAgICABICAgP+AgID/gICAaAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgHyAgID/gICA7oCAgO6AgID/gICA+oCAgPSAgID2gICA/4CAgP+AgID4gICA9ICAgPSAgID0gICA9ICAgPSAgID0gICA9ICAgPSAgID0gICA9ICAgPSAgID0gICA9ICAgPSAgID0gICA9ICAgPSAgID0gICA9ICAgPSAgID0gICA+oCAgP+AgIDsgICA7oCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOyAgIDugICA/4CAgMCAgICEgICAhoCAgP+AgID/gICAtoCAgISAgICEgICAhICAgISAgICEgICAhICAgISAgICEgICAhICAgISAgICEgICAhICAgISAgICEgICAhICAgISAgICEgICAhICAgISAgICEgICAhICAgMCAgID/gICA7ICAgO6AgID/gICAfAAAAACAgIAEgICA/4CAgP+AgIBoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAfICAgP+AgIDqgICA7oCAgP+AgICEgICADICAgBKAgID/gICA/4CAgHCAgIAMgICADICAgAyAgIAMgICADICAgAyAgIAMgICADICAgAyAgIAMgICADICAgAyAgIAMgICADICAgAyAgIAMgICADICAgAyAgIAMgICADICAgAyAgICEgICA/4CAgOiAgIDugICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA5oCAgNaAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDQgICAHoCAgGKAgIBkgICAZoCAgGaAgIBogICAaICAgGqAgIBqgICAaoCAgGqAgIBsgICAbICAgGyAgIBsgICAbICAgGyAgIBsgICAbICAgG6AgIBugICAboCAgG6AgIBugICAboCAgG6AgIBsgICAbICAgGqAgIBmgICAXICAgCIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA////////////////AAAAAAAAAAA5///8Of///AAAAAAAAAAAAAAAADn///w5///8AAAAAAAAAAAAAAAAGf//+Bn///gAAAAAAAAAAAAAAAA5///8Of///AAAAAAAAAAAAAAAADn///wZ///4AAAAAAAAAAD///////////////8=");
define("BASE64_DETAILREGELS", "data:image/jpg;base64,AAABAAEAICAAAAEAIACoEAAAFgAAACgAAAAgAAAAQAAAAAEAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICACoCAgFCAgICUgICAxoCAgOSAgIDugICA7oCAgOSAgIDKgICAnICAgFqAgIAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAHICAgJCAgIDwgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgPaAgICegICAKAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICABICAgHyAgID0gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID6gICAkICAgAwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgBiAgIDEgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA1oCAgCgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIAigICA3oCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgPCAgIDqgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA7ICAgDYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAGICAgN6AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDugICAKoCAgBKAgIDUgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA7ICAgCoAAAAAAAAAAAAAAAAAAAAAAAAAAICAgASAgIDAgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA+ICAgEIAAAAAAAAAAICAgCCAgIDogICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA2ICAgAwAAAAAAAAAAAAAAAAAAAAAgICAdICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIBiAAAAAAAAAAAAAAAAAAAAAICAgDiAgID2gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICAlgAAAAAAAAAAAAAAAICAgBaAgIDwgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICAhgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgFiAgID8gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID6gICALgAAAAAAAAAAgICAhICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgKyAgIACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgIKAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgICmAAAAAICAgASAgIDogICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDMgICACgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgKqAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgPiAgIAUgICAQoCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA5oCAgBwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICACoCAgM6AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgGSAgICEgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgPyAgIA6AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAIICAgO6AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICApoCAgLSAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgJ6AgICMgICAioCAgIqAgIAeAAAAAAAAAAAAAAAAAAAAAICAgAyAgICKgICAioCAgIyAgICUgICA8oCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDWgICA0ICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgDYAAAAAAAAAAAAAAAAAAAAAgICAFICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOyAgIDagICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICANgAAAAAAAAAAAAAAAAAAAACAgIAUgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA7oCAgNqAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIBkgICAMoCAgDCAgIAwgICAMICAgECAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDugICA0ICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgO6AgIC0gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA9oCAgPSAgID0gICA9ICAgPSAgID2gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA1oCAgISAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIA2AAAAAAAAAAAAAAAAAAAAAICAgBSAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgICkgICAQICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgDYAAAAAAAAAAAAAAAAAAAAAgICAFICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgGKAgIACgICA5oCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICApoCAgI6AgICOgICAjoCAgI6AgICWgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID4gICAFAAAAACAgICAgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgKQAAAAAAAAAAICAgBKAgIDugICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgLSAgICcgICAmoCAgJiAgICWgICAoICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID6gICAKgAAAAAAAAAAAAAAAICAgGyAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICAOgAAAAAAAAAAAAAAAAAAAACAgIAUgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgJAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgLqAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIBQAAAAAAAAAAAAAAAAAAAAAICAgCiAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDUgICACgAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFICAgNiAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOaAgICKgICAhICAgIKAgICEgICA0oCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA6oCAgCYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAHoCAgNqAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOqAgIAyAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFICAgL6AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDSgICAJAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgHSAgIDygICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID4gICAioCAgAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgBiAgICKgICA7ICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDygICAmICAgCIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIAIgICASoCAgIyAgIC8gICA3oCAgOyAgIDogICA3oCAgMKAgICSgICAUoCAgAoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA//AP//+AAf//AAB//AAAP/gAAB/wAYAP4APAB+AH4APAB/ADgA/wAYAf+AGAP/wBAH/+AAAH4AAAB+AAAAfgAAAH4AAAAAAAAAAAAAAH4ACAB+ABgAAAAYAAAAHAAAAD4AfgA+AH4AfwAAAP+AAAH/wAAD//AAB//4AB///wD/8=");
define("BASE64_MUTEER", "data:image/jpg;base64,AAABAAEAICAAAAEAIACoEAAAFgAAACgAAAAgAAAAQAAAAAEAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAfICAgOiAgIDugICA7oCAgO6AgIDugICA7oCAgO6AgIDugICA7oCAgO6AgIDugICA7oCAgO6AgIDugICA7oCAgO6AgIDugICA7oCAgO6AgIDugICA6oCAgIqAgIACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgEiAgID/gICA7oCAgNKAgIDSgICA0oCAgNKAgIDSgICAwoCAgHKAgICSgICAzoCAgNKAgIDSgICA0oCAgNKAgIDSgICA0oCAgNKAgIDSgICA0oCAgNKAgIDqgICA/4CAgFgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAeoCAgP+AgIBYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICADoCAgAYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgEaAgID/gICAigAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB6gICA/4CAgFIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgICEgICA7oCAgKKAgIBQgICACgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAPoCAgP+AgICKAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgHqAgID/gICAUgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgDqAgID/gICA/4CAgNyAgIDsgICArICAgFyAgIAOAAAAAAAAAAAAAAAAAAAAAAAAAACAgIA+gICA/4CAgIoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAeoCAgP+AgIBSAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgOaAgIC6gICACoCAgAyAgIBmgICA/ICAgOCAgIAiAAAAAAAAAAAAAAAAAAAAAICAgD6AgID/gICAigAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB6gICA/4CAgFIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAloCAgGwAAAAAgICAEICAgMyAgID/gICA/4CAgOCAgIAiAAAAAAAAAAAAAAAAgICAPoCAgP+AgICKAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgHqAgID/gICAUgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIBGgICAvICAgBCAgIDMgICA/4CAgP+AgID/gICA/4CAgOCAgIAiAAAAAAAAAACAgIA+gICA/4CAgIoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAeoCAgP+AgIBSAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgAaAgIDqgICAzoCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAiAAAAAICAgD6AgID/gICAigAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB6gICA/4CAgFIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgKCAgID/gICA/ICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAigICAHICAgOaAgICKAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgHqAgID/gICAUgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFoCAgNKAgIB4gICAgICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAigICAKICAgHQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAeoCAgP+AgIBSAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFoCAgMqAgIBIgICAgICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAigICAAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB6gICA/4CAgFIAAAAAAAAAAICAgA6AgICqgICAroCAgK6AgICugICAroCAgK6AgICugICAxICAgPaAgIBIgICAgICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAiAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgHqAgID/gICAUgAAAAAAAAAAgICABICAgGSAgIBqgICAaoCAgGqAgIBqgICAaoCAgGqAgIBqgICAaoCAgMqAgIBIgICAgICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAiAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAeoCAgP+AgIBSAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFoCAgMqAgIBIgICAgICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAiAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB6gICA/4CAgFIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFoCAgMqAgIBIgICAgICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAiAAAAAAAAAAAAAAAAAAAAAICAgHqAgID/gICAUgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFoCAgMqAgIBIgICAgICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAiAAAAAAAAAAAAAAAAgICAeoCAgP+AgIBSAAAAAAAAAACAgIAQgICAxICAgMqAgIDKgICAyoCAgMqAgIDKgICAyoCAgMqAgIDKgICAyoCAgMqAgIDKgICAiICAgMqAgIBIgICAgICAgP+AgID/gICA/4CAgP+AgID/gICA0oCAgBaAgIAOAAAAAAAAAACAgIB6gICA/4CAgFIAAAAAAAAAAICAgAKAgIBIgICAToCAgE6AgIBOgICAToCAgE6AgIBOgICAToCAgE6AgIBOgICAToCAgE6AgIAkgICAFoCAgMqAgIBIgICAgICAgP+AgID/gICA/4CAgNKAgIAWgICAdoCAgOCAgIAiAAAAAICAgHqAgID/gICAUgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFoCAgMqAgIBIgICAsICAgP+AgIDSgICAFoCAgHaAgID/gICA/4CAgOCAgIAcgICAeoCAgP+AgIBSAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFoCAgNCAgID8gICA0oCAgBaAgIB2gICA/4CAgP+AgID/gICA/4CAgHqAgIB6gICA/4CAgFIAAAAAAAAAAICAgASAgIBegICAYoCAgGKAgIBigICAYoCAgGKAgIBigICAYoCAgGKAgIBigICAYoCAgGKAgIAwAAAAAAAAAAAAAAAAgICAFoCAgKiAgIAWgICAdoCAgP+AgID/gICA/4CAgP+AgIDugICALoCAgHqAgID/gICAUgAAAAAAAAAAgICADoCAgLCAgIC2gICAtoCAgLaAgIC2gICAtoCAgLaAgIC2gICAtoCAgLaAgIC2gICAtoCAgGYAAAAAAAAAAAAAAAAAAAAAAAAAAICAgGKAgID/gICA/4CAgP+AgID/gICA7oCAgDYAAAAAgICAeoCAgP+AgIBSAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgCKAgIAYgICAFoCAgNKAgID/gICA/4CAgO6AgIA2AAAAAAAAAACAgIB6gICA/4CAgFIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAPoCAgNSAgIAYgICAFoCAgMSAgIDmgICANgAAAAAAAAAAAAAAAICAgHqAgID/gICAUgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIA+gICA/4CAgIoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAeoCAgP+AgIBSAAAAAAAAAACAgIBUgICAGAAAAAAAAAAAgICADICAgGAAAAAAAAAAAICAgACAgIBkgICABgAAAAAAAAAAgICAMoCAgDgAAAAAAAAAAICAgD6AgID/gICAigAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB4gICA/4CAgGAAAAAAAAAAAICAgNqAgIBaAAAAAAAAAACAgIA6gICA+gAAAAAAAAAAgICADICAgP+AgIAoAAAAAAAAAACAgICSgICAngAAAAAAAAAAgICATICAgP+AgICIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgDqAgID/gICA+oCAgO6AgIDugICA/ICAgPSAgIDugICA7oCAgPKAgID/gICA7oCAgO6AgIDugICA/4CAgPCAgIDugICA7oCAgPiAgID4gICA7oCAgO6AgID4gICA/4CAgEwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgGCAgIDQgICA3oCAgN6AgID6gICA6oCAgN6AgIDegICA5oCAgP+AgIDegICA3oCAgOCAgID/gICA4oCAgN6AgIDegICA8ICAgPKAgIDegICA3oCAgNKAgIBsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgNqAgIBaAAAAAAAAAACAgIA6gICA+gAAAAAAAAAAgICAEICAgP+AgIAoAAAAAAAAAACAgICSgICAngAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAsoCAgEAAAAAAAAAAAICAgCaAgIDKAAAAAAAAAACAgIAGgICA0oCAgBgAAAAAAAAAAICAgHKAgIB8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAwAAA/4BAAP+///5/v4/+f7/B/n+/zn5/v9w+f7/oHn+/4A5/v+AGf7/0A/+/+gH/vAEA/7/+gH+//0A/v/+gH7//0A+8AAgPv//0G7//+jG///xhv//+wbwAP4O///+Hv//+z7///n+///5/u93Of4AAAP/AAAH/+93P//vd//8=");

define("DTSHORT", "dd-MM-yyyy");
define("DTDAYMONTH", "d MMMM");
define("DTTEXT", "d MMMM yyyy");
define("DTPERSCAL", "EEEE d MMMM");
define("DTTEXTWD", "EEEE d MMMM yyyy");
define("DTLONG", "d MMMM yyyy (HH:mm)");
define("DTLONGWD", "EEEE d MMMM yyyy (HH:mm)");
define("DTLONGSEC", "d MMMM yyyy (HH:mm:ss)");
define("DTLONGWDSEC", "EEEE d MMMM yyyy (HH:mm:ss)");

require(BASEDIR . '/config.php');
define("LIDIDWEBMASTERS", $lididwebmasters);
$bestandsversie = filectime(BASEDIR . "/index.php");
if (strlen($table_prefix) > 0 and substr($table_prefix, -1) != "_") {
	$table_prefix .= "_";
}
define("TABLE_PREFIX", $table_prefix);

if (isset($httpsverplicht) and $httpsverplicht == 1 and !isset($_SERVER['HTTPS'])) {
	header(sprintf("location: https:%s", $_SERVER['HTTP_HOST']));
}

if (isset($_SERVER['HTTPS'])){
	define("BASISURL", "https://" . $_SERVER['HTTP_HOST']);
} else {
	define("BASISURL", "http://" . $_SERVER['HTTP_HOST']);
}

session_cache_expire(25);
if (isset($_SESSION['lidid'])) {
	// Sessie bestaat al
} elseif (session_start() === FALSE or strlen(session_id()) == 0) {
	$mess = "De sessie kan niet gestart worden, hierdoor kan er op dit moment niet ingelogd worden.";
	(new cls_Logboek())->add($mess, 1, 0, 1);
}

if ($_SERVER['PHP_SELF'] == "/index.php") {
	if (isset($_COOKIE["pagbreedte"]) and $_COOKIE["pagbreedte"] > 0) {
		$_SESSION['pagbreedte'] = $_COOKIE["pagbreedte"];
	} elseif ($_SESSION['pagbreedte'] == 0) {
		$_SESSION['pagbreedte'] = $_GET['p_pagbreedte'] ?? 0;
		if ($_SESSION['pagbreedte'] > 0) {
			setcookie("pagbreedte", $_SESSION['pagbreedte'], time()+(3600*24*90));
		}
	}
	if ($_SESSION['pagbreedte'] == 0) {
		printf("<script>location.href=\"%s?p_pagbreedte=\" + screen.width + \"\"</script>\n", BASISURL);
	}
}

if ((isset($_SESSION['webmaster']) and $_SESSION['webmaster'] == 1) or strpos(BASISURL, "telling.nl") > 0) {
	error_reporting(E_ALL);
} else {
	error_reporting(0);
//	error_reporting(E_ALL);
}

if (!isset($_SESSION['lidid']) or $_SESSION['lidid'] == 0) {
	$_SESSION['lidid'] = 0;
	$_SESSION['lidnr'] = 0;
	$_SESSION['emailingelogde'] = "";
	$_SESSION['naamingelogde'] = "gast";
	$_SESSION['roepnaamingelogde'] = "gast";
	$_SESSION['webmaster'] = 0;
	$_SESSION['lidgroepen'] = "0";
}

$currenttab2 = "";
$currenttab3 = "";
if (isset($_GET['tp']) and mb_substr_count($_GET['tp'], "/") == 2) {
	$currenttab = explode("/", $_GET['tp'])[0];
	$currenttab2 = explode("/", $_GET['tp'])[1];
	$currenttab3 = explode("/", $_GET['tp'])[2];
	
} elseif (isset($_GET['tp']) and mb_substr_count($_GET['tp'], "/") == 1) {
	$currenttab = explode("/", $_GET['tp'])[0];
	$currenttab2 = explode("/", $_GET['tp'])[1];
	
} elseif (isset($_GET['tp']) and strlen($_GET['tp']) > 1) {
	$currenttab = $_GET['tp'];
	
} else {
	if ($_SERVER['PHP_SELF'] == "/admin.php") {
		$currenttab = "Beheer logins";
	} else {
		$currenttab = "Vereniging";
	}
}

/* Include maatwerk */
$incl = BASEDIR . "/maatwerk/maatwerk";
if (file_exists($incl . ".inc")) {
	require_once($incl . ".inc");
} elseif (file_exists($incl . ".php")) {
	require_once($incl . ".php");
}
/* Einde includen maatwerk*/

/* Include Open source libaries */
require_once("phpmailer/PHPMailer.php");
require_once("phpmailer/SMTP.php");
require_once("phpmailer/Exception.php");
require_once("simpleimage.php");
/* Einde includen open source libs*/

foreach (array("database", "authentication", "afdelingen", "leden", "bewaking", "evenement", "financieel", "mailing", "diploma", "standaard", "webshop", "taken") as $incl) {
	$inclpath = __DIR__ . "/" . $incl . ".inc";
	if (file_exists($inclpath)) {
		if ($bestandsversie < filectime($inclpath)) {
			$bestandsversie = filectime($inclpath);
		}
		require_once($inclpath);
	} else {
		$inclpath = __DIR__ . "/" . $incl . ".php";
		if (file_exists($inclpath)) {
			if ($bestandsversie < filectime($inclpath)) {
				$bestandsversie = filectime($inclpath);
			}
			require_once($inclpath);
		}
	}
}

$i_p = new cls_Parameter();

if (!isset($_SESSION['settings']['naamvereniging'])) {
	$i_p->vulsessie();
}
if ($bestandsversie != $_SESSION['settings']['versie']) {
	debug(sprintf("Nieuwe versie: %s", $dtfmt->format(time())), 0);
	$i_p->controle();
	db_createtables();
	db_onderhoud(2);
	$i_p->update("versie", $bestandsversie);
	$i_p->vulsessie();
}
$i_p = null;

if ($_SESSION['lidid'] > 0) {
	(new cls_Login())->setlastactivity();
}
$dtfmt->setPattern(DTTEXT);
$fileverinfo = 'templates/verenigingsinfo.html';

foreach ($tabpages as $tp) {
	$arrTP = explode("/", $tp);
	if (strlen($currenttab2) == 0 and count($arrTP) > 1 and $arrTP[0] == $currenttab and toegang($tp, 0)) {
		$currenttab2 = $arrTP[1];
	}
	if (strlen($currenttab3) == 0 and count($arrTP) > 2 and $arrTP[0] == $currenttab and $arrTP[1] == $currenttab2 and toegang($tp, 0)) {
		$currenttab3 = $arrTP[2];
	}
}

function HTMLheader($breed=0) {
	global $currenttab, $currenttab2, $currenttab3;
	
	if (isset($_SESSION['settings']['title_head_html']) and strlen($_SESSION['settings']['title_head_html']) > 0) {
		$title = $_SESSION['settings']['title_head_html'];
	} else {
		if (isset($_SESSION['settings']['naamvereniging_afkorting']) and strlen($_SESSION['settings']['naamvereniging_afkorting']) > 0) {
			$title = $_SESSION['settings']['naamvereniging_afkorting'] . " | ";
		} else {
			$title = "";
		}
		$title .= $_SESSION['settings']['naamwebsite'];
	}

	if (strlen($currenttab) > 0) {
		$title .= " | " . $currenttab;
	}
	if (strlen($currenttab2) > 0) {
		$title .= " | " . $currenttab2;
	}
	if (strlen($currenttab3) > 0) {
		$title .= " | " . $currenttab3;
	}

   if (!isset($_GET['header']) or $_GET['header'] != "no") {
		echo("<!DOCTYPE HTML>\n");
		echo("<html lang='nl'>\n");
		echo("<head>\n");
		if (file_exists("includes/javascript.js")) {
			printf("<script src='includes/javascript.js?v%d'></script>\n", filectime("includes/javascript.js"));
		}
		echo("<meta charset='UTF-8'>\n");
		echo("<meta http-equiv='Cache-control' content='public'>\n");
		echo("<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n");
		printf("<title>%s</title>\n", $title);
		echo("<link rel='stylesheet' href='//code.jquery.com/ui/1.13.1/themes/base/jquery-ui.css'>\n");
//		echo("<link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap-icons@1.8.1/font/bootstrap-icons.css'>\n");
		echo("<script src='https://code.jquery.com/jquery-3.6.0.js'></script>\n");
		echo("<script src='https://code.jquery.com/ui/1.13.1/jquery-ui.js'></script>\n");
		
		if (file_exists("default.css")) {
			printf("<link rel='stylesheet' href='default.css?v%d'>\n", filectime("default.css"));
		}
		if (file_exists("maatwerk/kleur.css")) {
			printf("<link rel='stylesheet' href='maatwerk/kleur.css?v%d'>\n", filectime("maatwerk/kleur.css"));
		}
		
		echo("</head>\n");
		echo("<body>\n");
		if ($breed == 0) {
			echo("<div id='container'>\n");
		} else {
			echo("<div id='containerbreed'>\n");
		}
		echo("<header>\n");
		
		if (strlen($_SESSION['settings']['url_eigen_help']) < 10) {
			$htmlhelp = "<a href='https://software.telling.nl/phpRBM/uitleg.php'><img src='images/question.png' title='Uitleg gebruik en installatie' alt='Question mark'></a>\n";
		} else {
			$htmlhelp = sprintf("<a href='%s'><img src='images/question.png' title='Helpfunctie' alt='Question mark'></a>\n", $_SESSION['settings']['url_eigen_help']);
		}
	
		echo("<div id='welkom'>\n");
		
		if ($_SESSION['lidid'] > 0) {
			printf("<p>Welkom %s</p>\n", htmlentities($_SESSION['roepnaamingelogde']));
			echo("<a href='/?tp=Zelfservice/Wijzigen+wachtwoord'><img src='images/user.png' title='Wijzigen wachtwoord' alt='Wijzigen wachtwoord'></a>\n");
			echo("<a href='index.php?actie=uitloggen'><img src='images/power.png' title='Uitloggen' alt='Uitloggen'></a>\n");
			if ($_SESSION['webmaster'] == 1) {
				if ($_SERVER['PHP_SELF'] != "/admin.php") {
					echo("<a href='admin.php'><img src='images/tool.png' title='Beheerdersmenu' alt='Tool'></a>\n");
				} elseif ($_SERVER['PHP_SELF'] == "/admin.php") {
					echo("<a href='index.php'><img src='images/home.png' title='Beginpagina' alt='Home'></a>\n");
				}
			}
			echo($htmlhelp);
		} else {
			echo($htmlhelp);
		}

		echo("</div>  <!-- Einde welkom -->\n");
		
		printf("<h1>%s</h1>\n", $_SESSION['settings']['naamwebsite']);
		echo("<div class='clear'></div>\n");
		
		$xtra = "";
		if ($currenttab == "Overzicht lid" and isset($_GET['lidid']) and $_GET['lidid'] > 0) {
			$xtra = sprintf("lidid=%d", $_GET['lidid']);
		}
		fnDispMenu(1, $xtra);
		
		echo("</header>\n");
		echo("<div class='clear'></div>\n");
	}
} # HTMLheader

function HTMLfooter() {
	global $navpad, $bestandsversie, $currenttab, $currenttab2, $currenttab3;

	if (!isset($_GET['header']) or $_GET['header'] != "no") {
		if (isset($_SESSION['settings']['naamvereniging']) and strlen($_SESSION['settings']['naamvereniging']) > 2) {
			if (strlen($_SESSION['settings']['urlvereniging']) > 3) {
				$cr = sprintf("<a href='%s'>%s</a>", $_SESSION['settings']['urlvereniging'], $_SESSION['settings']['naamvereniging']);
			} else {
				$cr = $_SESSION['settings']['naamvereniging'];
			}
		} else {
			$cr = "<a href='https://www.telling.nl'>&copy;&nbsp;S.Telling</a>";
		}
		echo("<div class='clear'></div>\n");
		
		$np = "";
		if (isset($navpad) and is_array($navpad)) {
			$i = 1;
			foreach ($navpad as $npl) {
				if (isset($npl['url']) and strlen($npl['url']) > 0 and $i < count($navpad)) {
					$np .= " | " . sprintf("<a href='%s'>%s</a>\n", $npl['url'], $npl['naam']);
				} else {
					$np .= " | " . $npl['naam'] . "\n";
				}
				$i++;
			}
		}
		echo("<footer>\n");
		printf("<a href='%s'>%s</a> %s\n", BASISURL, $_SESSION['settings']['naamwebsite'], $np);
		echo("</footer>\n");
		echo("</div>  <!-- Einde container of containerbreed -->\n");
		echo("</body>\n");
		echo("</html>\n");
	}
} # HTMLfooter

function fnDispMenu($toonnivo=1, $xtra="") {
	global $tabpages, $currenttab, $currenttab2, $currenttab3, $navpad;
	
	echo("<nav>\n");
	$tpd = ""; // Dit wordt getoond in het menu.
	$ulopen = false;
	$_SESSION['settings']['typemenu'] = 1;
	$_SESSION['pagbreedte'] = $_SESSION['pagbreedte'] ?? 900;
	if ($_SESSION['pagbreedte'] > 2500) {
		$mamk = 12;
	} elseif ($_SESSION['pagbreedte'] > 1250) {
		$mamk = 11;
	} else {
		$mamk = 4;
	}

	if (strlen($xtra) > 0 and startwith($xtra, "&amp;") == false) {
		$xtra = "&" . $xtra;
	}
	
	$nivo = $toonnivo;
	if ($_SESSION['settings']['typemenu'] == 1 or ($_SESSION['settings']['typemenu'] == 3 and $nivo == 2)) {
		printf("<ul id='menu_l%d'>\n", $toonnivo);
		$itp = 1;
		foreach($tabpages as $tp) {
			if ($nivo == 1 and strstr($tp, "/") === false) {
				$tpd = $tp;
			} elseif ($nivo == 2 and mb_substr_count($tp, "/") == 1 and startwith($tp, $currenttab . "/")) {
				$tpd = str_replace($currenttab . "/", "", $tp);
			
			} elseif ($nivo == 3 and mb_substr_count($tp, "/") == 2 and startwith($tp, $currenttab . "/") and strpos($tp, $currenttab2) > 1) {
				$tpd = str_replace($currenttab2 . "/", "", str_replace($currenttab . "/", "", $tp));
				
//			} elseif ($currenttab2 == "Overzicht lid") {
				
			} else {
				$tpd = "";
			}
			if (strlen($tpd) > 1) {
				$cl = "";
				if ($nivo == 1 and $currenttab == str_replace("'", "", $tp)) {
					$cl = " class='current'";
					$navpad[$nivo]['naam'] = $tpd;
					$navpad[$nivo]['url'] = sprintf("%s?tp=%s%s", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra);
				} elseif ($nivo == 2 and ($currenttab2 == str_replace("'", "", $tpd) or strlen($currenttab2) == 0)) {
					$cl = " class='current'";
					if (toegang($currenttab . "/" . str_replace("'", "", $tpd), 0)) {
						$currenttab2 = $tpd;
					}
					$navpad[$nivo]['naam'] = $tpd;
					$navpad[$nivo]['url'] = sprintf("%s?tp=%s%s", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra);
				} elseif ($nivo == 3 and ($currenttab3 == str_replace("'", "", $tpd) or strlen($currenttab3) == 0)) {
					$cl = " class='current'";
					$navpad[$nivo]['naam'] = $tpd;
					$navpad[$nivo]['url'] = sprintf("%s?tp=%s%s", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra);
				}
				if ($itp == $mamk and 1 == 2) {
					echo("<li class='dropdown'>&nbsp;\n");
					echo("<ul>\n");
				}
				printf("<li%s><a href=\"%s?tp=%s%s\">%s</a></li>\n", $cl, $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra, $tpd);
				$itp++;
			}
		}
		if ($itp > $mamk and 1 == 2) {
			echo("</ul> <!-- Einde dropdown -->\n");
			echo("</li>\n");
		}
		echo("</ul>\n");
	}
	echo("</nav>\n");
	if ($nivo > 1) {
		echo("<div class='clear'></div>\n");
	}
}  # fnDispMenu

function isValidMailAddress($address, $LeegIsGoed=0){

  	if ($LeegIsGoed == 1 and strlen($address) == 0) {
		return true;
	} elseif (strlen(trim($address)) < 5) {
		return false;
   } else {
		return filter_var($address, FILTER_VALIDATE_EMAIL);
	}
} # isValidMailAddress

function fnDispEmail($strEmail, $strNaam="", $booInclLink=1) {
	if (strpos($strEmail, "<") > 2 and strlen($strnaam) < 3) {
   	$strNaam = trim(substr(trim($strEmail), 0, strpos($strEmail, "<")));
		$strEmail = substr(trim($strEmail), strpos($strEmail, "<")+1, -1);
   }
	$strRet = "";
	if (isValidMailAddress($strEmail, 0)) {
		if ($_SESSION['lidid'] > 0) {
			$strNaam = str_replace(" ", "%20", htmlentities($strNaam));
			if (strlen($strNaam) > 3 and $booInclLink == 1) {
				$strRet = sprintf('<a href="mailto:%1$s%%20<%2$s>">%2$s</a>', $strNaam, $strEmail);
			} elseif ($booInclLink == 1) {
				$strRet = sprintf('<a href="mailto:%1$s">%1$s</a>', $strEmail);
			} elseif (strlen($strNaam) > 3 and $booInclLink == 2) {
				$strRet = sprintf('<a href="mailto:%1$s%%20<%2$s>">%1$s</a>', $strNaam, $strEmail);
			} else {
				$strRet = $strEmail;
			}
		} else {
			$strHV = "";
			$iCounter=0;
			while ($iCounter < strlen($strEmail)) {
				$strHV .= chr(ord(substr($strEmail, $iCounter, 1))+1);
				$iCounter++;
			}
			$strRet = "<script>\n";
			$strRet .= "a0 = \"" . $strHV . "\"\n";

			$strRet .= "s0=\"\"\n";
			$strRet .= "for (i=0;i<a0.length; i++) {
					n=a0.charCodeAt(i)-1;
					s0+=String.fromCharCode(n);
					}\n";
			if (strlen($strNaam) > 3 and $booInclLink == 1) {
				$strRet .= "document.write('<a href=\"mailto: " . $strNaam . " <'+s0+'" . ">\">'+s0+'</a>');\n";
			} elseif ($booInclLink == 1) {
				$strRet .= "document.write('<a href=\"mailto: '+s0+'\">'+s0+'</a>');\n";
			} elseif (strlen($strNaam) > 3 and $booInclLink == 2) {
				$strRet .= "document.write('<a href=\"mailto: " . $strNaam . " <'+s0+'" . ">\">" . $strNaam . "</a>');\n";
			} else {
				$strRet .= "document.write(s0);\n";
			}
			$strRet .= "</script>\n";
		}
   } elseif (strlen($strEmail) > 0) {
   	$strRet = "Geen (geldig) e-mailadres";
   }
	return $strRet;
}

function getvar($nm, $riz="") {
	if (isset($_GET[$nm])) {
		return $_GET[$nm];
	} elseif (isset($_POST[$nm])) {
		return $_POST[$nm];
	} elseif (isset($_SESSION[$nm])) {
		return $_SESSION[$nm];
	} else {
		return $riz;
	}
}

function fnDisplayForm($row, $xtra_eind="", $id="formulier") {
	global $dtfmt;

	if (strlen($id) > 0) {
		$ret = sprintf("<div id='%s'>\n", $id);
	}
	if (isset($row) and $row != null) {
		$ret .= "<table>\n";
		foreach($row as $key => $value) {
			if (substr($key, 0, 3) == "Kop") {
				$ret .= sprintf("<tr><th colspan=2>%s</th></tr>\n", $value);
			} elseif (substr($key, 0, 3) == "dte" and (!is_null($value))) {
				if (strtotime($value) == false) {
					$dv = $value;
				} else {
					$dtfmt->setPattern(DTTEXT);
					$dv = $dtfmt->format(strtotime($value));
				}
				$ret .= sprintf("<tr><td class='label'>%s</td><td class='datum'>%s</td></tr>\n", substr($key, 3), $dv);
			} elseif (substr($key, 0, 3) == "dtt" and (!is_null($value))) {
				$dtfmt->setPattern(DTLONG);
				$ret .= sprintf("<tr><td class='label'>%s</td><td class='datumtijd'>%s</td></tr>\n", substr($key, 3), $dtfmt->format(strtotime($value)));
			} elseif (substr($key, 0, 3) == "boo" and $value == 1) {
				$ret .= sprintf("<tr><td class='label'>%s</td><td>Ja</td></tr>\n", substr($key, 3));
			} elseif (substr($key, 0, 3) == "boo" and $value != 1) {
				$ret .= sprintf("<tr><td class='label'>%s</td><td>Nee</td></tr>\n", substr($key, 3));
			} elseif (substr($key, 0, 3) == "mem" and strlen($value) > 1) {
				$ret .= sprintf("<tr><td colspan=2>%s</td></tr\n>", htmlentities($value));
			} elseif (substr($key, 0, 3) == "eml" and strlen($value) > 1) {
				$ret .= sprintf("<tr><td class='label'>%s</td><td>%s</td></tr>\n", substr($key, 3), fnDispEmail($value, "", 1));
			} elseif (substr($key, 0, 2) != "nd" and strlen($value) > 0) {
				$ret .= sprintf("<tr><td class='label'>%s</td><td>%s</td></tr>\n", $key, str_replace("\n", "<br>\n", htmlentities($value)));
			}
		}
		$ret .= "</table>\n";
	} else {
		$ret .= "Er is geen record beschikbaar.";
	}
	if (strlen($xtra_eind) > 0) {
		$ret .= $xtra_eind;
	}
	if (strlen($id) > 0) {
		$ret .= sprintf("</div>  <!-- Einde %s -->\n", $id);
	}
	return $ret;
}

function fnDisplayFormLabels($row, $xtra_eind="", $id="") {
	global $dtfmt;
	
	$dtfmt->setPattern(DTTEXT);

	if (strlen($id) > 0) {
		$ret = sprintf("<div id='%s'>\n", $id);
	}
	if (isset($row) and $row != null) {
		foreach($row as $key => $value) {
			if (substr($key, 0, 3) == "Kop") {
				$ret .= sprintf("<h2>%s</h2>\n", $value);
			} elseif (substr($key, 0, 3) == "dte" and (!is_null($value))) {
				if ($value <= '1900-01-01') {
					$dv = "";
				} elseif (strtotime($value) == false) {
					$dv = $value;
				} else {
					$dv = $dtfmt->format(strtotime($value));
				}
				$ret .= sprintf("<label>%s</label><p>%s</p>\n", substr($key, 3), $dv);
			} elseif (substr($key, 0, 3) == "dtt" and (!is_null($value))) {
				$dtfmt->setPattern(DTLONG);
				$ret .= sprintf("<label>%s<p>%s</p>\n", substr($key, 3), $dtfmt->format(strtotime($value)));
			} elseif (substr($key, 0, 3) == "boo" and $value == 1) {
				$ret .= sprintf("<label>%s</label><p>Ja</p>\n", substr($key, 3));
			} elseif (substr($key, 0, 3) == "boo" and $value != 1) {
				$ret .= sprintf("<label>%s</label><p>Nee</p>\n", substr($key, 3));
			} elseif (substr($key, 0, 3) == "mem" and strlen($value) > 1) {
				$ret .= sprintf("<value>%s</value>\n", htmlentities($value));
			} elseif (substr($key, 0, 3) == "eml" and strlen($value) > 1) {
				$ret .= sprintf("<label>%s</label><p>%s</p>\n", substr($key, 3), fnDispEmail($value, "", 1));
			} elseif (substr($key, 0, 2) != "nd" and strlen($value) > 0) {
				$ret .= sprintf("<label>%s</label><p>%s</p>\n", $key, htmlentities($value));
			}
		}
	} else {
		$ret .= "Er is geen record beschikbaar.";
	}
	if (strlen($xtra_eind) > 0) {
		$ret .= $xtra_eind;
	}
	if (strlen($id) > 0) {
		$ret .= sprintf("</div>  <!-- Einde %s -->\n", $id);
	}
	return $ret;
}  # fnDisplayFormLabels

function fnDisplayTable($rows, $kols=null, $th="", $disptot=0, $xtra_regel="", $class="lijst", $xtra_kolom="") {
	global $currenttab, $currenttab2, $dtfmt;
	
	$ret = "";
	if (strlen($class) > 0) {
		$ret = sprintf("<div class='%s'>\n", $class);
	}
	$kol_pk = "";
	if (isset($rows) and (count($rows) > 0 or strlen($xtra_regel) > 0)) {
		if (strlen($class) > 0) {
			$ret .= sprintf("<table id='%1\$s' name='%1\$s'>\n", $class);
		} else {
			$ret .= "<table>\n";
		}
		$dispkop = 1;
		$totaantal = 0;
		
		if (strlen($th) > 1) {
			$ret .= sprintf("<caption>%s</caption>\n", $th);
		}
		$ret .= "<thead>\n<tr>";
		
		if ($kols == "logboek") {
			$kols = fnStandaardKols($kols);
		}
			
		if ($kols == null) {
//			debug("fnDisplayTable zonder kolommen", 2, 1);
			$kn = 0;
			$skn = 0;
			$hv = "";
			foreach($rows[0] as $key => $value) {
				$kols[$kn]['columnname'] = $key;
				$sortlink = "";
				$ht = "";
				if (strstr("cbo, chk, dte, dtk, dtt, dtl, dts, boo, int, cur, eml, num, mrg, pwd, lnm", substr($key, 0, 3)) !== false) {
					$ht = str_replace("_", " ", substr($key, 3));
					
				} elseif (isset($kols[$kn]['link'])) {
					$hv .= "<th>&nbsp;</th>";
					$ktot[$kn] = "geen";

				} elseif (startwith($key, "llk") and isset($kols[-1]['link'])) {
					$hv .= "<th>&nbsp;</th>";
					$ktot[$kn] = "geen";
						
				} elseif (isset($kols[$kn]['skip']) and $kols[$kn]['skip'] == true) {
					// Deze kolom moet worden overgeslagen.
					
				} elseif (isset($kols[$kn]['headertext'])) {
					$hv .= sprintf("<th>%s</th>", $kols[$kn]['headertext']);
					
				} elseif (startwith($key, "lek")) {
					if (strlen($xtra_kolom) > 1) {
						$hv .= "<th>&nbsp;</th>";
						$ktot[$kn] = "geen";
					}
				} elseif (substr($key, 0, 2) != "nd") {
					$ht = str_replace("_", " ", $key);
				}
					
				if (empty($sortkols) == false) {
					$skn = -1;
					foreach ($sortkols as $k => $v) {
						if (startwith($v, $ht)) {
							$skn = $k;
						}
					}
					if (isset($_SESSION['huidsort']) and $_SESSION['huidsort'] < 50 and $_SESSION['huidsort'] == $skn and $skn >= 0) {
						$skn += 50;
					}
					if ($skn >= 0) {
						$sortlink = sprintf("%s?tp=%s&sort=%d", $_SERVER["PHP_SELF"], $currenttab, $skn);
					}
				}
					
				if (strlen($ht) > 0 and strlen($sortlink) > 0) {
					$hv .= sprintf("<th><a href='%s'>%s</a></th>", $sortlink, $ht);
				} elseif (strlen($ht) > 0) {
					$hv .= sprintf("<th>%s</th>", $ht);
				}
					
				if (startwith($key, "cur")) {
					$ktot[$kn] = 0.00;
				} elseif (substr($key, 0, 3) == "int") {
					$ktot[$kn] = 0;
				} elseif ($key == "RecordID") {
					$ktot[$kn] = 0;
				} elseif (substr($key, 0, 2) != "nd" and substr($key, 0, 3) != "lnk" and substr($key, 0, 3) != "llk") {
					$ktot[$kn] = "geen";
				}
				if (strlen($kol_pk) == 0 and ($key == "RecordID" or startwith($key, "lnk") or startwith($key, "llk"))) {
					$kol_pk = $key;
				}
				$kn++;
			}
			
			$ret .= $hv;
			
		} else {
		
			foreach ($kols as $k => $kol) {
				$iq = false;
				if (isset($kol['headertext']) or isset($kol['link'])) {
					if (isset($kol['subqry']) and strlen($kol['subqry']) > 6) {
						$iq = true;
					} elseif (isset($kol['columnname']) and strlen($kol['columnname']) > 0) {
						foreach($rows[0] as $key => $val) {
							if ($key == $kol['columnname']) {
								$iq = true;
							}
						}
					} else {
						$iq = true;
						$kn = 0;
						foreach($rows[0] as $key => $val) {
							if ($k == $kn) {
								$kols[$k]['columnname'] = $key;
							}
							$kn++;
						}
					}
				}
				
				if ($iq) {
					if (!isset($kol['headertext'])) {
						$kols[$k]['headertext'] = "&nbsp;";
					}
					if (isset($kol['sortcolumn'])) {
						if (isset($_SESSION['huidsort']) and $_SESSION['huidsort'] < 50 and $_SESSION['huidsort'] == $k and $k >= 0) {
							$skn = $k + 50;
						} else {
							$skn = $k;
						}
						if (strlen($currenttab2) > 0) {
							$ct = $currenttab . "/" . $currenttab2;
						} else {
							$ct = $currenttab;
						}
						$sortlink = sprintf("%s?tp=%s&sort=%d", $_SERVER["PHP_SELF"], $ct, $skn);
						$ret .= sprintf("<th><a href='%s'>%s</a></th>", $sortlink, $kols[$k]['headertext']);
					} else {
						$ret .= sprintf("<th>%s</th>", $kols[$k]['headertext']);
					}
//					debug($k . ": " . $kols[$k]['headertext'] . ": " . $kols[$k]['columnname']);
				} else {
					$kols[$k]['skip'] = true;
				}
				$ktot[$k] = 0;
			}
		}
		$ret .= "</tr>\n</thead>\n<tbody>\n";
		
		foreach($rows as $row) {
			$kn = 0;
			$ret .= "<tr>";
//			foreach($row as $key => $value) {
			foreach ($kols as $kn => $kol) {
				if (isset($kol['columnname'])) {
					$key = $kol['columnname'];
					if (isset($kol['skip']) and $kol['skip'] == true) {
						$value = null;
					} elseif (isset($kol['subqry']) and strlen($kol['subqry']) > 6) {
						$sq = str_replace("%d", $row->{$key}, $kol['subqry']);
						$value = (new cls_db_base())->scalar($sq);
						$kol['skip'] = false;
					} else {
						$value = $row->{$key};
						$kol['skip'] = false;
					}
				} else {
					$key = "";
					$kol['skip'] = true;
					$value = null;
				}
				$kol_edit = 0;
				$kol_link = "";
				$kol_class = "";
				if (isset($kols[$kn]['edit'])) {
					$kol_edit = $kols[$kn]['edit'];
				}
				
				if ($kol_edit == 1) {
					debug("Edit in display");
				}
				
				if (isset($kols[$kn]['type'])) {
					$kol_type = $kols[$kn]['type'];
				} elseif (strlen($key) >= 3 and strstr("cbo, chk, dte, dtk, dtt, dtl, dts, boo, int, cur, eml, num, mrg, pwd, lnm", substr($key, 0, 3)) !== false) {
					$kol_type = substr($key, 0, 3);
				} else {
					$kol_type = "text";
				}
				
				if (isset($kols[$kn]['link'])) {
					$kol_type = "link";
					if (startwith($kols[$kn]['link'], "<a href") == true) {
						$kol_link = $kols[$kn]['link'];
					} else {
						$kol_link = "<a href='" . $kols[$kn]['link'] . "'";
						if (isset($kols[$kn]['title'])) {
							$kol_link .= sprintf(" title='%s'", $kols[$kn]['title']);
						}
						$kol_link .= ">&nbsp;&nbsp;&nbsp;</a>";
					}
				}
				
				if (isset($kols[$kn]['class'])) {
					$kol_class = sprintf("class='%s'", $kols[$kn]['class']);
				}
				
				if ($kol['skip'] == true) {
					// Deze kolom moet worden overgeslagen.
						
				} elseif ($kol_type == "chk") {
					if ($kol_edit == 0) {
						if ($value == 1) {
							$ret .= "<td>Ja</td>";
						} else {
							$ret .= "<td>Nee</td>";
						}
					} else {
						$ret .= sprintf("<td><input type='checkbox'%s name='%s_%d' value=1%s></td>", $js, str_replace(" ", "_", substr($key, 3)), $row->$kol_pk, checked($value, "", 1));
					}
					
				} elseif ($key == "cboFunctie") {
					debug("fnDisplayTable/cboFunctie", 2, 1);
					$ret .= "<td>";
					if ($rowscbo != null) {
						$ret .= sprintf("<select name='%s_%d' %s>", substr($key, 3), $row->$kol_pk, $js);
						foreach ($rowscbo as $rowcbo) {
							if ($rowcbo->Vervallen >= $row->dteVanaf) {
								$ret .= sprintf("<option value=%d %s>%s</option>", $rowcbo->Nummer, checked($rowcbo->Nummer, "option", $value), $rowcbo->Omschrijv);
							}
						}
						$ret.= "</select>";
					}
					$ret .= "</td>\n";
					
				} elseif ($kol_type == "dte" or $kol_type == "rodte" or $kol_type == "date" or $kol_type == "DTTEXT") {
					// 1 januari 2020
					if ((!is_null($value)) and $value > '1900-01-01' and strtotime($value) !== false) {
						$dtfmt->setPattern(DTTEXT);
						$ret .= sprintf("<td>%s</td>", $dtfmt->format(strtotime($value)));
					} elseif ((!is_null($value)) and strlen($value) > 0) {
						$ret .= sprintf("<td>%s</td>", $value);
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
					
				} elseif ($kol_type == "dtk" or $kol_type == "dateshort") {
					if ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td>%s</td>", date('d/m/Y', strtotime($value)));
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
				} elseif ($kol_type == "dtl" or $kol_type == "DTTEXTWD") {
					$dtfmt->setPattern(DTTEXTWD);
					if ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td>%s</td>", $dtfmt->format(strtotime($value)));
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
				} elseif ($kol_type == "dtt" or $kol_type == "rodtt" or $kol_type == "datetime" or $kol_type == "DTLONG") {
					$dtfmt->setPattern(DTLONG);
					if ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td>%s</td>", $dtfmt->format(strtotime($value)));
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
					
				} elseif ($kol_type == "dts" or $kol_type == "datetimesecond" or $kol_type == "DTLONGSEC") {
					if ((!is_null($value)) and $value > '1900-01-01' and strtotime($value) !== false) {
						$dtfmt->setPattern(DTLONGSEC);
						$ret .= sprintf("<td>%s</td>", $dtfmt->format(strtotime($value)));
					} elseif (strlen($value) > 0) {
						$ret .= sprintf("<td>%s</td>", $value);
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
				} elseif ($kol_type == "int" or $kol_type == "integer") {
					if ($kol_edit == 0) {
						$ret .= sprintf("<td class='number'>%d</td>", $value);
					} else {
						$ret .= sprintf("<td><input type='number' name='%s_%d' value='%s'></td>", substr($key, 3), $row->$kol_pk, $value);
					}
					$ktot[$kn] += intval($value);
					
				} elseif ($kol_type == "cur" or $kol_type == "currency" or $kol_type == "bedrag") {
					$ret .= sprintf("<td class='number'>%s</td>", number_format($value, 2, ",", "."));
					$ktot[$kn] += $value;

				} elseif ($key == "Getoond") {
					if ($value == 0) {
						$ret .= "<td>Nee</td>";
					} else {
						$ret .= "<td>Ja</td>";
					}

				} elseif ($kol_type == "boo" or $kol_type == "boolean") {
					if ($value == 1) {
						$ret .= "<td>Ja</td>";
					} elseif ($value == 0 or $value == -1) {
						$ret .= "<td>Nee</td>";
					} else {
						$ret .= "<td>Onbekend</td>";
					}
					
				} elseif (($kol_type == "lnm" or $kol_type == "lidnaam") and strlen($value) > 0) {
					if (is_numeric($value)) {
						$ret .= sprintf("<td>%s</td>", (new cls_lid())->Naam($value, "onbekend"));
					} else {
						$ret .= sprintf("<td>%s</td>", $value);
					}
					
				} elseif (strlen($kol_link) > 1) {
					if ($value > 0) {
						$href = str_replace("%d", $value, $kol_link);
						$href = str_replace("%s", "&nbsp;&nbsp;&nbsp;", $href);
						$ret .= sprintf("<td %s>%s</td>", $kol_class, $href);
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
						
				} elseif ($kol_type == "llk" or $kol_type == "link") {
					if ($value > 0 and isset($kols[-1]['link'])) {
						if (startwith($kols[-1]['link'], "<a href") == true) {
							$kol_link = $kols[-1]['link'];
						} else {
							$kol_link = "<a href='" . $kols[-1]['link'] . "'>&nbsp;&nbsp;&nbsp;</a>";
						}
						$href = str_replace("%d", $value, $kol_link);
						$kol_class = "";
						if (isset($kols[-1]['class'])) {
							$kol_class = sprintf("class='%s'", $kols[-1]['class']);
						}
						$ret .= sprintf("<td %s>%s</td>", $kol_class, $href);
					} elseif (isset($kols[-1]['link'])) {
						$ret .= "<td>&nbsp;</td>\n";
					}
					
				} elseif (startwith($key, "lek")) {
					if ($value > 0 and strlen($xtra_kolom) > 1) {
						$href = str_replace("%d", $value, $xtra_kolom);
						$href = str_replace("%s", "<img src='images/zoom.png' alt='Details'>", $href);
						$ret .= "<td class='details'>" . $href . "</td>";
					} elseif (strlen($xtra_kolom) > 1) {
						$ret .= "<td></td>\n";
					}
					
				} elseif (startwith($key, "eml")) {
					if ($kol_edit == 0) {
						$ret .= sprintf("<td>%s</td>", fnDispEmail($value, "", 1));
					} else {
						$ret .= sprintf("<td><input type='email' name='%s_%d' value='%s' class='email' %s></td>", str_replace(" ", "_", substr($key, 3)), $row->$kol_pk, $value, $js);
					}
				} elseif (startwith($key, "pwd")) {
					if ($kol_edit == 0) {
						$ret .= "<td></td>";
					} else {
						$ret .= sprintf("<td><input type='password' name='%s_%d'></td>", substr($key, 3), $row->$kol_pk);
					}
				} elseif (substr($key, 0, 3) === "num") {
					$ret .= sprintf("<td class='number'>%s</td>", $value);
				} elseif (substr($key, 0, 3) == "nm2" or $kol_type == "num2") {
					$ret .= sprintf("<td class='number'>%s</td>", number_format(floatval($value), 2, ",", "."));
				} elseif ($kol_type == "mrg" or $kol_type == "combitext") {
					$ret .= sprintf("<td>%s</td>", str_replace(" &amp; ", "<br>", htmlentities($value, ENT_HTML5)));
					
				} elseif ($kol_type == "subqry") {
					$ret .= sprintf("<td>%s</td>", $value);
					
				} else {
					if ($kol_edit == 0 or $row->$kol_pk == 0) {
						$ret .= sprintf("<td>%s</td>", htmlentities($value));
					} else {
						$ret .= sprintf("<td><input name='%s_%d' value='%s' class='%s'%s></td>", $key, $row->$kol_pk, $value, str_replace(" ", "", strtolower($key)), $js);
					}
				}
				$kn++;
			}
			$ret .= "</tr>\n";
			$totaantal++;
		}
		if (strlen($xtra_regel) > 0) {
			$ret .= sprintf("<tr><td colspan=%d>%s</td></tr>\n", count($kols), $xtra_regel);
		}
		$ret .= "</tbody>\n";
		if ($disptot > 0 and $totaantal > 1) {
			$ret .= "<tfoot>\n<tr>";
			for ($kn=0; $kn < count($ktot); $kn++) {
				if ($kn+1 == $disptot and $totaantal > 1) {
					$ret .= sprintf("<th colspan=2>%s records</th>", number_format($totaantal, 0, ",", "."));
					$kn++;
				} elseif (gettype($ktot[$kn]) == "double") {
					$ret .= sprintf("<th class='number'>%s</th>", number_format($ktot[$kn], 2, ",", "."));
				} elseif (gettype($ktot[$kn]) == "integer") {
					$ret .= sprintf("<th class='number'>%s</th>", number_format($ktot[$kn], 0, ",", "."));
				} else {
					$ret .= "<th></th>";
				}
			}
			$ret .= "</tr>\n</tfoot>\n";
		}
		$ret .= "</table>\n";
	} else {
		$ret .= "<p class='mededeling'>Er zijn geen records beschikbaar.</p>\n";
	}
	if (strlen($class) > 0) {
		$ret .= sprintf("</div>  <!-- Einde %s -->\n", $class);
	}
	return $ret;
	
} # fnDisplayTable

function fnEditTable($res, $kols, $id, $th) {
	global $dtfmt;
	
	$rows = $res->fetchAll(PDO::FETCH_NUM);
	
	$ret = sprintf("<table id='%s'>\n", $id);
	if (strlen($th) > 0) {
		$ret .= sprintf("<caption>%s</caption>\n", $th);
	}
	$ret .= "<thead>\n";
	$ret .= "<tr>";
	$colpk = -1;
	foreach ($kols as $kn => $kol) {
		$kols[$kn]['colnum'] = $kn;
		$kols[$kn]['col_cond_ro'] = -1;
		if (count($rows) > 0) {
			foreach ($rows[0] as $k => $v) {
				if ($kn == $k and !isset($kol['columnname'])) {
					$kols[$kn]['columnname'] = $res->getColumnMeta($k)['name'];
					$kols[$kn]['colnum'] = $k;
				} elseif (isset($kol['columnname']) and $res->getColumnMeta($k)['name'] == $kol['columnname']) {
					$kols[$kn]['colnum'] = $k;
				}
				if (isset($kol['link']) and strlen($kol['link']) > 1) {
					$kols[$kn]['type'] = "link";
				} elseif (!isset($kol['type'])) {
					$kols[$kn]['type'] = strtolower($res->getColumnMeta($kn)['native_type']);
				}
				if (isset($kol['cond_ro']) and strlen($kol['cond_ro']) > 0 and $res->getColumnMeta($k)['name'] == $kol['cond_ro']) {
					$kols[$kn]['col_cond_ro'] = $k;
				}
			}
		}
		
		if (isset($kol['headertext']) and strlen($kol['headertext']) > 0) {
			$ret .= sprintf("<th>%s</th>", $kol['headertext']);
		}
		if (isset($kol['type']) and $kol['type'] == "pk") {
			$colpk = $kn;
		}
	}
	
//	debug($kols);
	$ret .= "</tr>\n";
	$ret .= "</thead>\n";
	
	$ret .= "<tbody>\n";
	$dispkop = 1;
	$totaantal = 0;
	foreach($rows as $row) {
		$ret .= "<tr>";
		$kn = 0;
		$valpk = $row[$colpk];
		foreach ($kols as $key => $kol) {
			$tp = $kol['type'] ?? "text";
			$kol_class = "";
			if (isset($kol['class'])) {
				$kol_class = sprintf("class='%s'", $kol['class']);
			}
			if (isset($kol['link'])) {
				$tp = "link";
				if (substr($kol['link'], 0, 7) == "<a href") {
					$kol_link = $kol['link'];
				} else {
					$kol_link = "<a href='" . $kol['link'] . "'";
					if (isset($kol['title'])) {
						if (isset($kol['columntitle']) and strpos($kol['title'], "%") !== false) {
							$t = sprintf($kol['title'], $row[$kol['columntitle']]);
						} else {
							$t = $kol['title'];
						}
						$kol_link .= sprintf(" title='%s'", $t);
					}
					$kol_link .= ">&nbsp;&nbsp;&nbsp;</a>";
				}
				
			}
			$colnum = $kol['colnum'];
			$col_cond_ro = $kol['col_cond_ro'];
			$ro = $kol['readonly'] ?? false;
			if ($col_cond_ro >= 0 and $row[$col_cond_ro] > 0) {
				$ro = true;
			}
			$value = $row[$colnum];
			$colname = $res->getColumnMeta($colnum)['name'];
			$collen = intval($res->getColumnMeta($colnum)['len'] / 3);
			
			if ($tp == "link") {
				if ($value > 0 and $ro == false) {
					$href = str_replace("%d", $value, $kol_link);
					$href = str_replace("%s", "&nbsp;&nbsp;&nbsp;", $href);
					$ret .= sprintf("<td %s>%s</td>", $kol_class, $href);
				} else {
					$ret .= "<td>&nbsp;</td>";
				}	
			
			} elseif ($tp == "checkbox") {
				$ret .= "<td class='checkbox'>";
				if ($ro) {
					if ($value == 1) {
						$ret .= "Ja</td>";
					} else {
						$ret .= "Nee</td>";
					}
				} else {
					$ret .= sprintf("<input type='checkbox' id='%s_%d' value=1%s></td>", $colname, $valpk, checked($value));
				}
				
			} elseif (isset($kol['bronselect']) and count($kol['bronselect']) > 0) {
				$opt = "";
				foreach ($kol['bronselect'] as $k => $v) {
					$opt .= sprintf("<option%s value=%s>%s</option>\n", checked($k, "option", $value), $k, $v);
				}
				$ret .= sprintf("<td><select id='%s_%d'>\n%s</select></td>\n", $colname, $valpk, $opt);

			} elseif ($tp == "date" or $tp == "rqdate" or $tp == "DTTEXT") {
				if ($ro == false) {
					$rq = "";
					if ($tp == "rqdate") {
						$rq = " required";
					}
					$ret .= sprintf("<td><input type='date' id='%s_%d' value='%s'%s></td>", $colname, $valpk, $value, $rq);
				} elseif ((!is_null($value)) and $value > '1900-01-01') {
					$dtfmt->setPattern(DTTEXT);
					$ret .= sprintf("<td>%s</td>", $dtfmt->format(strtotime($value)));
				} else {
					$ret .= "<td>&nbsp;</td>";
				}
				
			} elseif ($tp == "datetime" or $tp == "DTLONG") {
				$dtfmt->setPattern(DTLONG); // 1 januari 2020 (14:12)
				if ($ro == false) {
					// Nog aanpassen, klopt niet
					$ret .= sprintf("<td><input type='date' id='%s_%d' value='%s'></td>", $colname, $valpk, $value);
					
				} elseif ((!is_null($value)) and $value > '1900-01-01') {
					$ret .= sprintf("<td>%s</td>", $dtfmt->format(strtotime($value)));
				} else {
					$ret .= "<td>&nbsp;</td>";
				}

			} elseif ($tp == "integer" or ($tp == "pk" and isset($kols[$colpk]['headertext']))) {
				if ($ro) {
					$ret .= sprintf("<td class='number'>%d</td>", $value);
				} else {
					if (isset($kol['max']) and $kol['max'] <= 99) {
						$c = "num2";
						$m = sprintf(" max=%d", $kol['max']);
					} elseif (isset($kol['max']) and $kol['max'] <= 999) {
						$c = "num3";
						$m = sprintf(" max=%d", $kol['max']);
					} else {
						$c = "w10";
						$m = "";
					}
					$ret .= sprintf("<td><input type='number' id='%s_%d' value=%d class='%s'%s></td>", $colname, $valpk, $value, $c, $m);
				}

			} elseif ($tp == "leeftijd") {
				if ($ro) {
					$ret .= sprintf("<td class='number'>%d</td>", $value);
				} else {
					$ret .= sprintf("<td><input type='number' id='%s_%d' value=%d class='num2' min=0 max=99></td>", $colname, $valpk, $value);
				}
				
			} elseif ($tp == "dagen") {
				if ($ro) {
					if ($value == 1) {
						$ret .= "<td class='number'>1 dag</td>";
					} else {
						$ret .= sprintf("<td class='number'>%d dagen</td>", $value);
					}
				} else {
					$ret .= sprintf("<td><input type='number' id='%s_%d' value=%d class='num2' min=0 max=999>dagen</td>", $colname, $valpk, $value);
				}

			} elseif ($tp == "bedrag") {
				if ($ro) {
					$ret .= sprintf("<td class='number'>%.2F</td>", $value);
				} else {
					$ret .= sprintf("<td><input type='text' id='%s_%d' value=%.2F class='d8'></td>", $colname, $valpk, $value);
				}				
				
			} elseif ($tp == "email") {
				if ($ro) {
					$ret .= sprintf("<td>%s</td>", fnDispEmail($value, "", 1));
				} else {
					$ret .= sprintf("<td><input type='email' id='%s_%d' value='%s'></td>", $colname, $valpk, $value);
				}
				
			} elseif ($tp == "pwd") {
				if ($kol_edit == 0) {
					$ret .= "<td></td>";
				} else {
					$ret .= sprintf("<td><input type='password' name='%s_%d'></td>", $colname, $valpk);
				}

			} elseif (isset($kol['headertext'])) {
				if ($ro) {
					$ret .= sprintf("<td>%s</td>", htmlentities($value));
				} else {
					$xc = sprintf("w%d", $collen);
					$ret .= sprintf("<td><input type='text' id='%s_%d' value=\"%s\" class='%s' maxlength=%d></td>", $colname, $valpk, $value, $xc, $collen);
				}
			}
		}
		$ret .= "</tr>\n";
	}
	$ret .= "</tbody>\n";
	$ret .= "</table>\n";
	
	$ret .= sprintf("<script>
	
	$('select').change(function() {
		savedata('%1\$s', 0, this);
	});
	
	$('input').blur(function() {
		savedata('%1\$s', 0, this);
	});
	
	$(\"input[type='number']\").change(function() {
		savedata('%1\$s', 0, this);
	});

</script>\n", $id);

	return $ret;
	
} # fnEditTable

function fnStandaardKols($p_ent, $p_sort=1) {
	
	if ($p_ent == "logboek") {
		$kols = null;
		$kols[0]['headertext'] = "Datum en tijd";
		$kols[0]['type'] = "DTLONGSEC";
		if ($p_sort == 1) {
			$kols[0]['sortcolumn'] = "RecordID";
		}
		$kols[1]['headertext'] = "Omschrijving";
		if ($p_sort == 1) {
			$kols[1]['sortcolumn'] = "Omschrijving";
		}
		$kols[2]['headertext'] = "Betreft lid";
		$kols[2]['columnname'] = "betreftLid";
		$kols[2]['type'] = "lidnaam";
		$kols[3]['headertext'] = "Type";
		$kols[3]['columnname'] = "Type";
		if ($p_sort == 1) {
			$kols[3]['sortcolumn'] = "Type";
		}
		$kols[4]['headertext'] = "Ingelogd lid";
		$kols[4]['columnname'] = "ingelogdLid";
		if ($p_sort == 1) {
			$kols[4]['sortcolumn'] = "ingelogdLid";
		}
		$kols[4]['type'] = "lidnaam";
		$kols[5]['headertext'] = "Getoond?";
		$kols[5]['columnname'] = "Getoond";
		$kols[5]['type'] = "boolean";
		if ($_SESSION['webmaster'] == 1) {
			$kols[6]['headertext'] = "Script / functie";
			$kols[6]['columnname'] = "scriptFunctie";
			if ($p_sort == 1) {
				$kols[6]['sortcolumn'] = "scriptFunctie";
			}
			$kols[7]['headertext'] = "IP adres";
			$kols[7]['columnname'] = "IP_adres";
			if ($p_sort == 1) {
				$kols[7]['sortcolumn'] = "A.IP_adres";
			}
		}
	}
	
	return $kols;
	
}  # fnStandaardKols

function fnOrderBy($p_arrSort) {
	$ord = "";
	
	$_SESSION['huidsort'] = $_GET['sort'] ?? -1;
		
	if ($_SESSION['huidsort'] >= 50) {
		$sk = $_SESSION['huidsort'] - 50;
	} else {
		$sk= $_SESSION['huidsort'];
	}
	
	$sw = "";
	if ($sk >= 0 and isset($p_arrSort[$sk])) {
		//$sw = $p_arrSort[$sk];
		if (isset($p_arrSort[$sk]['sortcolumn'])) {
			$sw = $p_arrSort[$sk]['sortcolumn'];
		} else {
			$sw = $p_arrSort[$sk];
			if (strpos($sw, ";") > 0) {
				$sw = explode(";", $sw)[1];
			}
		}
	}
	
	if (strlen($sw) > 0 and (strpos($sw, " ") !== false or strpos($sw, "-") !== false)) {
		if (strpos($sw, ".") !== false) {
			$ord = explode(".", $sw)[0] . ".`" . explode(".", $sw)[1] . "`";
		} else {
			$ord = "`" . $sw . "`";
		}
	} else {
		$ord = $sw;
	}
	
	if ($_SESSION['huidsort'] >= 50 and strlen($ord) > 0) {
		$ord .= " DESC";
	}
	
	return $ord;

}  # fnOrderBy

function debug($tekst, $p_tm=2, $logact=0) {
	
	/*
	$p_tm
		* 0: nooit tonen
		* 1: aan iedereen tonen
		* 2: alleen tonen aan webmasters
		* 3: aan iedereen als alert tonen
	*/
	
	if (is_array($tekst) or is_object($tekst) and $p_tm > 0) {
		$tekst = print_r($tekst, true);
	}
	if ($_SERVER["HTTP_HOST"] == "phprbm.telling.nl" and $p_tm == 2) {
		$p_tm = 1;
	}
	
	$_SESSION['webmaster'] = $_SESSION['webmaster'] ?? 0;
	$f = "";
	if ($_SESSION['webmaster'] == 1 or $_SERVER["HTTP_HOST"] == "phprbm.telling.nl") {
		$bt = debug_backtrace(DEBUG_BACKTRACE_PROVIDE_OBJECT, 2);
		for ($t=count($bt)-1;$t >= 0;$t--) {
			if (strlen($f) > 0) {
				$f .= "=>";
			}
			if (isset($bt[$t]['class']) and strlen($bt[$t]['class']) > 0) {
				$f .= $bt[$t]['class'];
			}
			if (isset($bt[$t]['file']) and strlen($bt[$t]['file']) > 0) {
				$f .= str_replace(BASEDIR, "", $bt[$t]['file']);
				if (isset($bt[$t]['line'])) {
					$f .= sprintf(" (%d)", $bt[$t]['line']);
				}
			}
		}
		$f .= ":";
	}
	
	if ($p_tm == 3) {
		printf("<script>alert('Debug: %s');</script>\n", $tekst);
	} elseif ($p_tm == 1 or ($p_tm == 2 and $_SESSION['webmaster'] == 1)) {
		if ($_SESSION['webmaster'] == 0) {
			$et = ". Neem contact op met de webmaster om dit op te laten lossen.";
		} else {
			$et = "";
		}
		printf("<p class='debug'>%s %s%s</p>\n", $f, $tekst, $et);
	}

	if ($logact == 1) {
		(new cls_Logboek())->add("Debug: " . $tekst, 99);
	}
	
} # debug

function accessdate($sqldate) {
	if (strlen($sqldate) == 0) {
		return "NULL";
	} elseif (strtotime($sqldate) != false) {
		return "#" . date("m/d/Y", strtotime($sqldate)) . "#";
	} else {
		return false;
	}
} # accessdate

function startwith($haystack, $start) {
	if (strlen($haystack) < strlen($start)) {
		return false;
	} elseif (substr($haystack, 0, strlen($start)) == $start) {
		return true;
	} else {
		return false;
	}
} # startwith

function endswith($haystack, $end) {
    $length = strlen($end);
	if (strlen($haystack) == 0) {
		 return false;
    } elseif ($length == 0) {
		return true;
    } else {
		return (substr($haystack, -$length) === $end);
	}
} # endswith

function change_month_to_uk($strdate) {

	$arrMonth[]=array("From" => "januari", "To" => "January");
	$arrMonth[]=array("From" => "februari", "To" => "February");
	$arrMonth[]=array("From" => "maart", "To" => "March");
	$arrMonth[]=array("From" => "april", "To" => "April");
	$arrMonth[]=array("From" => "mei", "To" => "May");
	$arrMonth[]=array("From" => "juni", "To" => "June");
	$arrMonth[]=array("From" => "juli", "To" => "July");
	$arrMonth[]=array("From" => "augustus", "To" => "August");
	$arrMonth[]=array("From" => "september", "To" => "September");
	$arrMonth[]=array("From" => "oktober", "To" => "October");
	$arrMonth[]=array("From" => "november", "To" => "November");
	$arrMonth[]=array("From" => "december", "To" => "December");
	$arrMonth[]=array("From" => "morgen", "To" => "tomorrow");
	$arrMonth[]=array("From" => "vandaag", "To" => "today");
	$arrMonth[]=array("From" => "gisteren", "To" => "yesterday");
	
	if (strlen($strdate) > 4) {
		foreach ($arrMonth as $key => $value) {
			$strdate = str_ireplace(" " . substr($value["From"], 0, 3) . " ", " " . $value["To"] . " ", $strdate);
			$strdate = str_ireplace(" " . $value["From"], " " . $value["To"], $strdate);
		}
	}
	return $strdate;
} # change_month_to_uk

function removetextblock($content, $start_block="<!-- Ingelogd -->", $end_block="<!-- /Ingelogd -->") {

	while (strpos($content, $start_block) !== false and strpos($content, $end_block) !== false) {
		$stb = strpos($content, $start_block);
		$etb = strpos($content, $end_block) + strlen($end_block);
		$content = substr($content, 0, $stb) . substr($content, $etb);
	}
	return $content;
	
} # removetextblock

function checked($val, $tp="checkbox", $sv=1) {
	if ($tp === "option" and $val == $sv) {
		return " selected";
	} elseif ($tp === "checkbox" and (intval($val) == 1 or $val === true)) {
		return " checked";
	} else {
		return "";
	}
} #checked

function htmlent($text) {
	return htmlentities($text, ENT_COMPAT,'ISO-8859-1', true);
} # htmlent

function fnExtBestand($p_fn) {
	
	$p_fn = str_replace("!", ".", $p_fn);
	$hv_ext = explode(".", $p_fn);
	return $hv_ext[count($hv_ext)-1];
	
}  # extbestand

function pv($p_control, $p_ookget=1) {
	if (isset($_POST[$p_control])) {
		return $_POST[$p_control];
	} elseif ($p_ookget == 1 and isset($_GET[$p_control])) {
		return $_GET[$p_control];
	} else {
		return "";
	}
}  # pv

function aantaluniekeleden($rows, $veldnm) {
	
	$aant = 0;
	$vw = -1;
	
	asort($rows);
	
	foreach ($rows as $row) {
		if ($vw != $row->$veldnm) {
			$aant++;
		}
		$vw = $row->$veldnm;
	}
	
	return $aant;
	
}  # aantaluniekeleden

function fnControleTelefoon($tn, $srt="telefoon") {
	
	if (strlen(trim($tn)) == 0) {
		$rv = "";
	} elseif (substr($tn, 2, 1) == "-" and strlen($tn) == 11 and $srt == "mobiel") {
		$rv = "";
	} elseif (substr($tn, 3, 1) == "-" and strlen($tn) == 11 and $srt == "telefoon") {
		$rv = "";
	} elseif (substr($tn, 4, 1) == "-" and strlen($tn) == 11 and $srt == "telefoon") {
		$rv = "";
	} elseif (strlen($tn) == 10 and strstr($tn, "-") === false) {
		$rv = "";
	} elseif ($srt == "telefoon") {
		$rv = "Formaat telefoonnummer is niet correct.";
	} else {
		$rv = "Formaat mobiele nummer is niet correct.";
	}
	
	return $rv;
	
}  # fnControleTelefoon

function fnControleEmail($p_email) {

	if (strlen($p_email) == 0 or strpos($p_email, ",") !== false) {
		$rv = "";
	} elseif (isValidMailAddress($p_email) == false) {
		$rv = "Het formaat van het e-mailadres is niet correct.";
	} else {
		$domein = explode("@", $p_email)[1];
		if (checkdnsrr($domein, "MX") == false) {
			$rv = "Het domein van dit e-mailadres bestaat niet of kan geen e-mail ontvangen.";
		} else {
			$rv = "";
		}
	}
	
	return $rv;
	
}  # fnControleEmail

function DisplayTabs($tabblad, $inittab=0) {
	
	printf("<script>
		$( function() {
			$('#tabs').tabs({
				active: %d
			});
		} );
	</script>\n", $inittab);	
		
	echo("<div id='tabs'>\n");
	echo("<ul>\n");
	$tn = 1;
	foreach ($tabblad as $nm=>$inhoud) {
		printf("<li><a href='#tab-%s'>%s</a></li>\n", str_replace("'", "", strtolower($nm)), $nm);
		$tn++;
	}
	echo("</ul>\n");
		
	$tn = 1;
	foreach ($tabblad as $nm=>$inhoud) {
		printf("<div id='tab-%1\$s'>\n%2\$s</div> <!-- Einde tab-%1\$s -->\n", str_replace("'", "", strtolower($nm)), $inhoud);
		$tn++;
	}
	
	echo("</div> <!-- Einde tabs -->\n");
	
}  # DisplayTabs

function fnOptionsFromArray($p_array, $p_cv=-1, $p_geenwaarde="") {
	
	if (strlen($p_geenwaarde) > 0) {
		$rv = sprintf("<option value=0%s>%s</option>\n", checked($p_cv, "option", 0), $p_geenwaarde);
	} else {
		$rv = "";
	}
	
	foreach ($p_array as $k => $v) {
		$rv .= sprintf("<option value='%s'%s>%s</option>\n", $k, checked($p_cv, "option", $k), $v);
	}
	
	return $rv;
	
}  # fnOptionsFromArray

?>
