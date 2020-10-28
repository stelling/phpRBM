<?php

date_default_timezone_set('Europe/Amsterdam');
setlocale(LC_ALL,'nl_NL.utf8');

define("BASEDIR", $_SERVER['DOCUMENT_ROOT']);
define("ARRGESLACHT", ["O" => "Onbekend", "V" => "Vrouwelijk", "M" => "Mannelijk", "B" => "Bedrijf/Vereniging/Stichting"]);
define("ARRTL", ["Alle", "Leden", "Klosleden", "Voormalig leden"]);
define("ARRLEGITIMATIE", ["G" => "Geen", "I" => "Identiteitskaart", "P" => "Paspoort", "R" => "Rijbewijs"]);
define("ARRSOORTMEMO", ["A" => "Algemeen", "B" => "Bewaking", "D" => "Dieet", "E" => "Examen", "F" => "Financiën", "G" => "Gezondheid/medisch", "I" => "Inschrijving bewaking"]);
define("ARRTYPEONDERDEEL", ["A" => "Afdeling", "B" => "Bestuur", "C" => "Commissie", "G" => "Groep", "O" => "Onderscheiding", "R" => "Rol", "T" => "Toestemming"]);
define("ARRTYPEDIPLOMA", ["B" => "Brevet", "C" => "Competentie", "D" => "Diploma", "I" => "Insigne", "L" => "Licentie", "M" => "Module", "P" => "Proeve van bekwaamheid"]);
define("PASFOTOEXTENTIES", ["gif", "jpg", "jpeg", "png"]);
define("ARRTYPEEDITOR", [1 => "CKEditor 5 Classic editor", 2 => "CKEditor 4", 3 => "TinyMCE"]);
define("ARRSOORTEVENEMENT", ["B" => "Brigade-activiteit", "G" => "Gezellig/sociaal", "O" => "Opleiding", "V" => "Vergadering/overleg", "W" => "Wedstrijd"]);
define("ARRDLNSTATUS", ["A" => "Afgewezen", "B" => "Bevestigd", "I" => "Ingeschreven", "J" => "Aangemeld", "N" => "Niet geweest", "R" => "Reserve", "T" => "Onder voorbehoud", "X" => "Afgemeld/Afgezegd"]);
define("ARRPRESENTIESTATUS", ["A" => "Aangemeld/aanwezig", "R" => "Afgemeld/afwezig met bekende reden", "T" => "Onder voorbehoud", "X" => "Afgemeld/afwezig zonder bekende reden", "Z" => "Afgemeld/afwezig wegens ziekte"]);
define("ARRTYPEBACKUP", [1 => "phpRBM-tabellen", 2 => "Tabellen uit MS-Access", 3 => "Beide"]);
define("ARRTYPESTUK", ["B" => "Beleidsstuk", "F" => "Formulier", "H" => "Handleiding", "P" => "Promotiemateriaal", "R" => "Regeling", "V" => "Verklaring"]);
define("ARRTYPEMENU", [1 => "per niveau een aparte regel", 2 => "&eacute;&eacute;n menu met dropdown", 3 => "&eacute;&eacute;n menu met dropdown en extra menu voor niveau 2"]);

define("BASE64_TOEVOEGEN", "data:image/jpg;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAACDElEQVR42u2Xy2rCQBRAZxQfjWAE0YUiKN2ooLh200K39QtK/Y0u1WV/o6Vf0G4LdtO1IuhKEAVRQTEqEXz23vgg0eZRSaqLXLg4k4njMblzMqHkD8EwDIlEInGbzfYM3TvIq+3QFPJzsVg8NRqN6mQy0Twn1XwipSQWi8Xtdvs3dFmZ0ziASNdqtepyudQXwOPxkHA4/A7Ne8gV5NtqtarjmMViuYaPB2xCfrRarUy/39cXwO/3k0AgwJPNZX9tt9vZXq8njHm9XhIKhV6g+Qg57XQ6DKQhAOttN18ulwvr9Xo/nkwmc3Al8tiGH6e6A/h8PhIMBvcApVKpIB5PJBI5q9VqApgApwFA9QrrHCQjeyJa0O12awIAE1IlG87nczIcDgnKiiroVSkUAVRCom0ah1DRq94AuxC0TVOp1JFexYKRiWKlUvkSH4D/cQMAt0pfwufJobYR4Fe9GhWH2kYAWb0aFWJtXwTA/hbA2s12u11iFATWAPoEHmqSWyApQsi6hrmKsAokRQirQLUItyEtQtzlOByOsyxDEFKaOp1OQUQA8a8ims1mGxHhEbw3oFlFFbtcLtS1JhWPRiM6Ho9l50IVcxwn1NplPIxMABPgFIBzbcuPtL0bE+vVkBcTlmVRWHLalui12WxmBoOBvgAoq2g0qqZtQa/4cgobG30BMBS0LdErz/Oa5/wBboikeyFGSbsAAAAASUVORK5CYII=");
define("BASE64_VERWIJDER", "data:image/jpg;base64,AAABAAEAICAAAAEAIACoEAAAFgAAACgAAAAgAAAAQAAAAAEAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIBCgICAxICAgOyAgIDugICA7oCAgO6AgIDugICA7oCAgO6AgIDugICA7oCAgO6AgIDugICA7oCAgO6AgIDugICA7oCAgOyAgIDKgICATAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAQoCAgPqAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID8gICAUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIDIgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDYgICAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgPSAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIAAgICA+ICAgP+AgID/gICA/4CAgPyAgIDsgICA7ICAgPiAgID/gICA+oCAgOyAgIDsgICA+ICAgP+AgID4gICA7ICAgOyAgID8gICA/4CAgP+AgID/gICA/4CAgAwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgAKAgID4gICA/4CAgP+AgID/gICA6gAAAAAAAAAAgICApICAgP+AgIDGAAAAAAAAAACAgIC2gICA/4CAgLQAAAAAAAAAAICAgNqAgID/gICA/4CAgP+AgID/gICADgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgPqAgID/gICA/4CAgP+AgIDqAAAAAAAAAACAgICkgICA/4CAgMYAAAAAAAAAAICAgLaAgID/gICAtAAAAAAAAAAAgICA2oCAgP+AgID/gICA/4CAgP+AgIAOAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIACgICA+oCAgP+AgID/gICA/4CAgOoAAAAAAAAAAICAgKSAgID/gICAxgAAAAAAAAAAgICAtoCAgP+AgIC0AAAAAAAAAACAgIDagICA/4CAgP+AgID/gICA/4CAgBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgAKAgID8gICA/4CAgP+AgID/gICA6gAAAAAAAAAAgICApICAgP+AgIDGAAAAAAAAAACAgIC2gICA/4CAgLQAAAAAAAAAAICAgNqAgID/gICA/4CAgP+AgID/gICAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgPyAgID/gICA/4CAgP+AgIDqAAAAAAAAAACAgICkgICA/4CAgMYAAAAAAAAAAICAgLaAgID/gICAtAAAAAAAAAAAgICA2oCAgP+AgID/gICA/4CAgP+AgIAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIACgICA/ICAgP+AgID/gICA/4CAgOoAAAAAAAAAAICAgKSAgID/gICAxgAAAAAAAAAAgICAtoCAgP+AgIC0AAAAAAAAAACAgIDagICA/4CAgP+AgID/gICA/4CAgBIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgAKAgID8gICA/4CAgP+AgID/gICA6gAAAAAAAAAAgICApICAgP+AgIDGAAAAAAAAAACAgIC2gICA/4CAgLQAAAAAAAAAAICAgNqAgID/gICA/4CAgP+AgID/gICAEgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgPyAgID/gICA/4CAgP+AgIDqAAAAAAAAAACAgICkgICA/4CAgMYAAAAAAAAAAICAgLaAgID/gICAtAAAAAAAAAAAgICA2oCAgP+AgID/gICA/4CAgP+AgIASAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIACgICA/ICAgP+AgID/gICA/4CAgOoAAAAAAAAAAICAgKSAgID/gICAxgAAAAAAAAAAgICAtoCAgP+AgIC0AAAAAAAAAACAgIDagICA/4CAgP+AgID/gICA/4CAgBIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgAKAgID8gICA/4CAgP+AgID/gICA6gAAAAAAAAAAgICApICAgP+AgIDGAAAAAAAAAACAgIC2gICA/4CAgLQAAAAAAAAAAICAgNqAgID/gICA/4CAgP+AgID/gICAEgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgPyAgID/gICA/4CAgP+AgIDqAAAAAAAAAACAgICkgICA/4CAgMYAAAAAAAAAAICAgLaAgID/gICAtAAAAAAAAAAAgICA2oCAgP+AgID/gICA/4CAgP+AgIASAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIACgICA/ICAgP+AgID/gICA/4CAgOoAAAAAAAAAAICAgKSAgID/gICAxgAAAAAAAAAAgICAtoCAgP+AgIC0AAAAAAAAAACAgIDagICA/4CAgP+AgID/gICA/4CAgBIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgAKAgID8gICA/4CAgP+AgID/gICA6gAAAAAAAAAAgICApICAgP+AgIDGAAAAAAAAAACAgIC2gICA/4CAgLQAAAAAAAAAAICAgNqAgID/gICA/4CAgP+AgID/gICAEgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgPyAgID/gICA/4CAgP+AgIDqAAAAAAAAAACAgICkgICA/4CAgMYAAAAAAAAAAICAgLaAgID/gICAtAAAAAAAAAAAgICA2oCAgP+AgID/gICA/4CAgP+AgIASAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIACgICA/ICAgP+AgID/gICA/4CAgPCAgIBYgICAWICAgMSAgID/gICA2oCAgFiAgIBYgICA0ICAgP+AgIDOgICAWICAgFiAgIDmgICA/4CAgP+AgID/gICA/4CAgBIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgAKAgID8gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICAEgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgPyAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIASAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIACgICA/ICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgBIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgACAgIBUgICAVICAgFSAgIBUgICAVICAgFSAgIBUgICAVICAgFSAgIBUgICAVICAgFSAgIBUgICAVICAgFSAgIBUgICAVICAgFSAgIBUgICAVICAgFSAgIBUgICABgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgDaAgIBqgICAaoCAgGqAgIBqgICAaoCAgGqAgIBqgICAaoCAgGqAgIBqgICAaoCAgGqAgIBqgICAaoCAgGqAgIBqgICAaoCAgGqAgIBqgICAaoCAgGqAgIBqgICAaoCAgGqAgIBqgICAaoCAgDwAAAAAAAAAAAAAAAAAAAAAgICAfoCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICAjgAAAAAAAAAAAAAAAAAAAACAgIB+gICA/ICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgPyAgICOAAAAAAAAAAAAAAAAAAAAAICAgAKAgIAggICA9oCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID6gICALoCAgAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIAcgICAOICAgDyAgIA+gICAQICAgFyAgID/gICA/4CAgJaAgIBAgICAQICAgECAgIBAgICAQICAgECAgICIgICA/4CAgP+AgIBmgICAQICAgECAgIBAgICAQICAgCQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICACICAgPSAgID/gICA3oCAgJaAgICSgICAkoCAgJSAgICWgICAmoCAgNaAgID/gICA+oCAgBIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAjoCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgICgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIAEgICAgICAgNaAgIDmgICA6oCAgOyAgIDugICA7ICAgOiAgIDagICAiICAgAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA/gAAf/wAAD/4AAAf+AAAH/gAAB/4MYwf+DGMH/gxjB/4MYwf+DGMH/gxjB/4MYwf+DGMH/gxjB/4MYwf+DGMH/gxjB/4MYwf+DGMH/gxjB/4AAAf+AAAH/gAAB///////////+AAAAPgAAAD8AAAD//H4///wAP//8AD///gB/8=");
define("BASE64_VERWIJDER_KLEIN", "data:image/jpg;base64,R0lGODlhCQALAJEAAP///8zMzAAAAAAAACH5BAAAAAAALAAAAAAJAAsAAAIXBIRoyRkPA4qwtSOzWFpyoX0hFm2HBRQAOw==");
define("BASE64_DETAILS", "data:image/jpg;base64,AAABAAEAICAAAAEAIACoEAAAFgAAACgAAAAgAAAAQAAAAAEAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICANoCAgHyAgIB+gICAfoCAgH6AgIB+gICAfoCAgH6AgIB+gICAfoCAgH6AgIB+gICAfoCAgH6AgIB+gICAfoCAgH6AgIB+gICAfoCAgH6AgIB+gICAfoCAgH6AgIB+gICAfoCAgH6AgIB+gICAfoCAgH6AgIB+gICAfoCAgDSAgIDYgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA4ICAgOSAgID/gICA9ICAgOqAgIDsgICA/4CAgP+AgIDygICA6oCAgOqAgIDqgICA6oCAgOqAgIDqgICA6oCAgOqAgIDqgICA6oCAgOqAgIDqgICA6oCAgOqAgIDqgICA6oCAgOqAgIDqgICA6oCAgOqAgIDqgICA9ICAgP+AgIDugICA5oCAgP+AgIB8AAAAAICAgASAgID/gICA/4CAgGgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB8gICA/4CAgO6AgIDogICA/4CAgHwAAAAAgICABICAgP+AgID/gICAaAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgHyAgID/gICA7oCAgOqAgID/gICA0ICAgKSAgICmgICA/4CAgP+AgIDIgICApICAgKSAgICkgICApICAgKSAgICkgICApICAgKSAgICkgICApICAgKSAgICkgICApICAgKSAgICkgICApICAgKSAgICkgICApICAgKSAgICkgICA0ICAgP+AgIDugICA7ICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgO6AgIDsgICA/4CAgOqAgIDWgICA1oCAgP+AgID/gICA5oCAgNaAgIDWgICA1oCAgNaAgIDWgICA1oCAgNaAgIDWgICA1oCAgNaAgIDWgICA1oCAgNaAgIDWgICA1oCAgNaAgIDWgICA1oCAgNaAgIDWgICA1oCAgOqAgID/gICA7oCAgOyAgID/gICAfAAAAACAgIAEgICA/4CAgP+AgIBoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAfICAgP+AgIDugICA7ICAgP+AgIB8AAAAAICAgASAgID/gICA/4CAgGgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB8gICA/4CAgO6AgIDugICA/4CAgNyAgIC6gICAvICAgP+AgID/gICA1oCAgLqAgIC6gICAuoCAgLqAgIC6gICAuoCAgLqAgIC6gICAuoCAgLqAgIC6gICAuoCAgLqAgIC6gICAuoCAgLqAgIC6gICAuoCAgLqAgIC6gICAuoCAgNyAgID/gICA7oCAgO6AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDugICA7oCAgP+AgIDcgICAtICAgLKAgID/gICA/4CAgNCAgICwgICAsICAgLCAgICygICAsoCAgLKAgICygICAsoCAgLKAgICygICAsoCAgLSAgIC0gICAtICAgLSAgIC2gICAtoCAgLaAgIC2gICAuICAgLiAgIDcgICA/4CAgO6AgIDugICA/4CAgIIAAAAAgICABICAgP+AgID/gICAaAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgISAgID/gICA7oCAgO6AgID/gICAgAAAAACAgIAEgICA/4CAgP+AgIBoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAhICAgP+AgIDugICA7oCAgP+AgIDqgICA0ICAgNCAgID/gICA/4CAgOKAgIDQgICA0ICAgNCAgIDSgICA0oCAgNKAgIDSgICA0oCAgNKAgIDSgICA0oCAgNSAgIDUgICA1ICAgNSAgIDWgICA1oCAgNaAgIDWgICA2ICAgNiAgIDsgICA/4CAgO6AgIDugICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA7oCAgO6AgID/gICAyoCAgJqAgICcgICA/4CAgP+AgIDCgICAmoCAgJqAgICagICAmoCAgJqAgICagICAmoCAgJqAgICagICAmoCAgJqAgICagICAmoCAgJqAgICagICAmoCAgJqAgICagICAmoCAgJqAgICagICAyoCAgP+AgIDugICA7oCAgP+AgIB8AAAAAICAgASAgID/gICA/4CAgGgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB8gICA/4CAgO6AgIDugICA/4CAgHwAAAAAgICABICAgP+AgID/gICAaAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgHyAgID/gICA7oCAgO6AgID/gICA+oCAgPSAgID2gICA/4CAgP+AgID4gICA9ICAgPSAgID0gICA9ICAgPSAgID0gICA9ICAgPSAgID0gICA9ICAgPSAgID0gICA9ICAgPSAgID0gICA9ICAgPSAgID0gICA9ICAgPSAgID0gICA+oCAgP+AgIDsgICA7oCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOyAgIDugICA/4CAgMCAgICEgICAhoCAgP+AgID/gICAtoCAgISAgICEgICAhICAgISAgICEgICAhICAgISAgICEgICAhICAgISAgICEgICAhICAgISAgICEgICAhICAgISAgICEgICAhICAgISAgICEgICAhICAgMCAgID/gICA7ICAgO6AgID/gICAfAAAAACAgIAEgICA/4CAgP+AgIBoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAfICAgP+AgIDqgICA7oCAgP+AgICEgICADICAgBKAgID/gICA/4CAgHCAgIAMgICADICAgAyAgIAMgICADICAgAyAgIAMgICADICAgAyAgIAMgICADICAgAyAgIAMgICADICAgAyAgIAMgICADICAgAyAgIAMgICADICAgAyAgICEgICA/4CAgOiAgIDugICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA5oCAgNaAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDQgICAHoCAgGKAgIBkgICAZoCAgGaAgIBogICAaICAgGqAgIBqgICAaoCAgGqAgIBsgICAbICAgGyAgIBsgICAbICAgGyAgIBsgICAbICAgG6AgIBugICAboCAgG6AgIBugICAboCAgG6AgIBsgICAbICAgGqAgIBmgICAXICAgCIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA////////////////AAAAAAAAAAA5///8Of///AAAAAAAAAAAAAAAADn///w5///8AAAAAAAAAAAAAAAAGf//+Bn///gAAAAAAAAAAAAAAAA5///8Of///AAAAAAAAAAAAAAAADn///wZ///4AAAAAAAAAAD///////////////8=");
define("BASE64_DETAILREGELS", "data:image/jpg;base64,AAABAAEAICAAAAEAIACoEAAAFgAAACgAAAAgAAAAQAAAAAEAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICACoCAgFCAgICUgICAxoCAgOSAgIDugICA7oCAgOSAgIDKgICAnICAgFqAgIAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAHICAgJCAgIDwgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgPaAgICegICAKAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICABICAgHyAgID0gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID6gICAkICAgAwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgBiAgIDEgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA1oCAgCgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIAigICA3oCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgPCAgIDqgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA7ICAgDYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAGICAgN6AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDugICAKoCAgBKAgIDUgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA7ICAgCoAAAAAAAAAAAAAAAAAAAAAAAAAAICAgASAgIDAgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA+ICAgEIAAAAAAAAAAICAgCCAgIDogICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA2ICAgAwAAAAAAAAAAAAAAAAAAAAAgICAdICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIBiAAAAAAAAAAAAAAAAAAAAAICAgDiAgID2gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICAlgAAAAAAAAAAAAAAAICAgBaAgIDwgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICAhgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgFiAgID8gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID6gICALgAAAAAAAAAAgICAhICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgKyAgIACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgIKAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgICmAAAAAICAgASAgIDogICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDMgICACgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgKqAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgPiAgIAUgICAQoCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA5oCAgBwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICACoCAgM6AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgGSAgICEgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgPyAgIA6AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAIICAgO6AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICApoCAgLSAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgJ6AgICMgICAioCAgIqAgIAeAAAAAAAAAAAAAAAAAAAAAICAgAyAgICKgICAioCAgIyAgICUgICA8oCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDWgICA0ICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgDYAAAAAAAAAAAAAAAAAAAAAgICAFICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOyAgIDagICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICANgAAAAAAAAAAAAAAAAAAAACAgIAUgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA7oCAgNqAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIBkgICAMoCAgDCAgIAwgICAMICAgECAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDugICA0ICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgO6AgIC0gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA9oCAgPSAgID0gICA9ICAgPSAgID2gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA1oCAgISAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIA2AAAAAAAAAAAAAAAAAAAAAICAgBSAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgICkgICAQICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgDYAAAAAAAAAAAAAAAAAAAAAgICAFICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgGKAgIACgICA5oCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICApoCAgI6AgICOgICAjoCAgI6AgICWgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID4gICAFAAAAACAgICAgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgKQAAAAAAAAAAICAgBKAgIDugICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgLSAgICcgICAmoCAgJiAgICWgICAoICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID6gICAKgAAAAAAAAAAAAAAAICAgGyAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICAOgAAAAAAAAAAAAAAAAAAAACAgIAUgICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgJAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgLqAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIBQAAAAAAAAAAAAAAAAAAAAAICAgCiAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDUgICACgAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFICAgNiAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOaAgICKgICAhICAgIKAgICEgICA0oCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA6oCAgCYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAHoCAgNqAgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOqAgIAyAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFICAgL6AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDSgICAJAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgHSAgIDygICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID4gICAioCAgAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgBiAgICKgICA7ICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgP+AgIDygICAmICAgCIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIAIgICASoCAgIyAgIC8gICA3oCAgOyAgIDogICA3oCAgMKAgICSgICAUoCAgAoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA//AP//+AAf//AAB//AAAP/gAAB/wAYAP4APAB+AH4APAB/ADgA/wAYAf+AGAP/wBAH/+AAAH4AAAB+AAAAfgAAAH4AAAAAAAAAAAAAAH4ACAB+ABgAAAAYAAAAHAAAAD4AfgA+AH4AfwAAAP+AAAH/wAAD//AAB//4AB///wD/8=");
define("BASE64_MUTEER", "data:image/jpg;base64,AAABAAEAICAAAAEAIACoEAAAFgAAACgAAAAgAAAAQAAAAAEAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAfICAgOiAgIDugICA7oCAgO6AgIDugICA7oCAgO6AgIDugICA7oCAgO6AgIDugICA7oCAgO6AgIDugICA7oCAgO6AgIDugICA7oCAgO6AgIDugICA6oCAgIqAgIACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgEiAgID/gICA7oCAgNKAgIDSgICA0oCAgNKAgIDSgICAwoCAgHKAgICSgICAzoCAgNKAgIDSgICA0oCAgNKAgIDSgICA0oCAgNKAgIDSgICA0oCAgNKAgIDqgICA/4CAgFgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAeoCAgP+AgIBYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICADoCAgAYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgEaAgID/gICAigAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB6gICA/4CAgFIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgICEgICA7oCAgKKAgIBQgICACgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAPoCAgP+AgICKAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgHqAgID/gICAUgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgDqAgID/gICA/4CAgNyAgIDsgICArICAgFyAgIAOAAAAAAAAAAAAAAAAAAAAAAAAAACAgIA+gICA/4CAgIoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAeoCAgP+AgIBSAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAAoCAgOaAgIC6gICACoCAgAyAgIBmgICA/ICAgOCAgIAiAAAAAAAAAAAAAAAAAAAAAICAgD6AgID/gICAigAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB6gICA/4CAgFIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAloCAgGwAAAAAgICAEICAgMyAgID/gICA/4CAgOCAgIAiAAAAAAAAAAAAAAAAgICAPoCAgP+AgICKAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgHqAgID/gICAUgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIBGgICAvICAgBCAgIDMgICA/4CAgP+AgID/gICA/4CAgOCAgIAiAAAAAAAAAACAgIA+gICA/4CAgIoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAeoCAgP+AgIBSAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgAaAgIDqgICAzoCAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAiAAAAAICAgD6AgID/gICAigAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB6gICA/4CAgFIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgKCAgID/gICA/ICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAigICAHICAgOaAgICKAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgHqAgID/gICAUgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFoCAgNKAgIB4gICAgICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAigICAKICAgHQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAeoCAgP+AgIBSAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFoCAgMqAgIBIgICAgICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAigICAAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB6gICA/4CAgFIAAAAAAAAAAICAgA6AgICqgICAroCAgK6AgICugICAroCAgK6AgICugICAxICAgPaAgIBIgICAgICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAiAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgHqAgID/gICAUgAAAAAAAAAAgICABICAgGSAgIBqgICAaoCAgGqAgIBqgICAaoCAgGqAgIBqgICAaoCAgMqAgIBIgICAgICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAiAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAeoCAgP+AgIBSAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFoCAgMqAgIBIgICAgICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAiAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB6gICA/4CAgFIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFoCAgMqAgIBIgICAgICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAiAAAAAAAAAAAAAAAAAAAAAICAgHqAgID/gICAUgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFoCAgMqAgIBIgICAgICAgP+AgID/gICA/4CAgP+AgID/gICA/4CAgOCAgIAiAAAAAAAAAAAAAAAAgICAeoCAgP+AgIBSAAAAAAAAAACAgIAQgICAxICAgMqAgIDKgICAyoCAgMqAgIDKgICAyoCAgMqAgIDKgICAyoCAgMqAgIDKgICAiICAgMqAgIBIgICAgICAgP+AgID/gICA/4CAgP+AgID/gICA0oCAgBaAgIAOAAAAAAAAAACAgIB6gICA/4CAgFIAAAAAAAAAAICAgAKAgIBIgICAToCAgE6AgIBOgICAToCAgE6AgIBOgICAToCAgE6AgIBOgICAToCAgE6AgIAkgICAFoCAgMqAgIBIgICAgICAgP+AgID/gICA/4CAgNKAgIAWgICAdoCAgOCAgIAiAAAAAICAgHqAgID/gICAUgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFoCAgMqAgIBIgICAsICAgP+AgIDSgICAFoCAgHaAgID/gICA/4CAgOCAgIAcgICAeoCAgP+AgIBSAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAFoCAgNCAgID8gICA0oCAgBaAgIB2gICA/4CAgP+AgID/gICA/4CAgHqAgIB6gICA/4CAgFIAAAAAAAAAAICAgASAgIBegICAYoCAgGKAgIBigICAYoCAgGKAgIBigICAYoCAgGKAgIBigICAYoCAgGKAgIAwAAAAAAAAAAAAAAAAgICAFoCAgKiAgIAWgICAdoCAgP+AgID/gICA/4CAgP+AgIDugICALoCAgHqAgID/gICAUgAAAAAAAAAAgICADoCAgLCAgIC2gICAtoCAgLaAgIC2gICAtoCAgLaAgIC2gICAtoCAgLaAgIC2gICAtoCAgGYAAAAAAAAAAAAAAAAAAAAAAAAAAICAgGKAgID/gICA/4CAgP+AgID/gICA7oCAgDYAAAAAgICAeoCAgP+AgIBSAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgCKAgIAYgICAFoCAgNKAgID/gICA/4CAgO6AgIA2AAAAAAAAAACAgIB6gICA/4CAgFIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAPoCAgNSAgIAYgICAFoCAgMSAgIDmgICANgAAAAAAAAAAAAAAAICAgHqAgID/gICAUgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIA+gICA/4CAgIoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAeoCAgP+AgIBSAAAAAAAAAACAgIBUgICAGAAAAAAAAAAAgICADICAgGAAAAAAAAAAAICAgACAgIBkgICABgAAAAAAAAAAgICAMoCAgDgAAAAAAAAAAICAgD6AgID/gICAigAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAgIB4gICA/4CAgGAAAAAAAAAAAICAgNqAgIBaAAAAAAAAAACAgIA6gICA+gAAAAAAAAAAgICADICAgP+AgIAoAAAAAAAAAACAgICSgICAngAAAAAAAAAAgICATICAgP+AgICIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgDqAgID/gICA+oCAgO6AgIDugICA/ICAgPSAgIDugICA7oCAgPKAgID/gICA7oCAgO6AgIDugICA/4CAgPCAgIDugICA7oCAgPiAgID4gICA7oCAgO6AgID4gICA/4CAgEwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgGCAgIDQgICA3oCAgN6AgID6gICA6oCAgN6AgIDegICA5oCAgP+AgIDegICA3oCAgOCAgID/gICA4oCAgN6AgIDegICA8ICAgPKAgIDegICA3oCAgNKAgIBsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICAgNqAgIBaAAAAAAAAAACAgIA6gICA+gAAAAAAAAAAgICAEICAgP+AgIAoAAAAAAAAAACAgICSgICAngAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgICAsoCAgEAAAAAAAAAAAICAgCaAgIDKAAAAAAAAAACAgIAGgICA0oCAgBgAAAAAAAAAAICAgHKAgIB8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAwAAA/4BAAP+///5/v4/+f7/B/n+/zn5/v9w+f7/oHn+/4A5/v+AGf7/0A/+/+gH/vAEA/7/+gH+//0A/v/+gH7//0A+8AAgPv//0G7//+jG///xhv//+wbwAP4O///+Hv//+z7///n+///5/u93Of4AAAP/AAAH/+93P//vd//8=");

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

if ((isset($_SESSION['webmaster']) and $_SESSION['webmaster'] == 1) or strpos(BASISURL, "telling.nl") > 0) {
	error_reporting(E_ALL);
} else {
	error_reporting(0);
	error_reporting(E_ALL);
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
	$_GET['tp'] = $currenttab;
}

/* Included maatwerk */
$incl = BASEDIR . "/maatwerk/maatwerk.inc";
require_once($incl);
/* Einde includen maatwerk*/

/* Include Open source libaries */
require_once("phpmailer/PHPMailer.php");
require_once("phpmailer/SMTP.php");
require_once("phpmailer/Exception.php");
require_once("simpleimage.php");
/* Einde includen open source libs*/

foreach (array("database", "authentication", "leden", "bewaking", "evenement", "financieel", "mailing", "standaard", "webshop", "taken") as $incl) {
	$incl .= ".inc";
	if (file_exists(__DIR__ . "/" . $incl)) {
		if ($bestandsversie < filectime(__DIR__ . "/" . $incl)) {
			$bestandsversie = filectime(__DIR__ . "/" . $incl);
		}
		require_once($incl);
	}
}

$i_p = new cls_Parameter();

if (!isset($_SESSION['settings']['naamvereniging'])) {
	$i_p->vulsessie();
	$row = (new cls_db_base("Vereniging"))->basislijst()[0];
	$_SESSION['settings']['naamvereniging'] = $row->Naam;
	$_SESSION['settings']['naamvereniging_afkorting'] = $row->AfkortingNaam;
	$_SESSION['settings']['emailsecretariaat'] = $row->EmailSecretariaat;
	$_SESSION['settings']['emailledenadministratie'] = $row->EmailLedenadministratie;
}
if ($bestandsversie != $_SESSION['settings']['versie']) {
	debug(sprintf("Nieuwe versie: %s", strftime("%c")));
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
	
	if (isset($_SESSION['settings']['naamvereniging_afkorting']) and strlen($_SESSION['settings']['naamvereniging_afkorting']) > 0) {
		$title = $_SESSION['settings']['naamvereniging_afkorting'] . " | ";
	} else {
		$title = "";
	}
	
	if (strlen($currenttab) > 0 and strlen($currenttab2) > 0 and strlen($currenttab3) > 0) {
		$title .= $_SESSION['settings']['naamwebsite'] . " | " . $currenttab . " | " . $currenttab2 . " | " . $currenttab3;
	} elseif (strlen($currenttab) > 0 and strlen($currenttab2) > 0) {
		$title .= $_SESSION['settings']['naamwebsite'] . " | " . $currenttab . " | " . $currenttab2;
	} elseif (strlen($currenttab) > 0) {
		$title .= $_SESSION['settings']['naamwebsite'] . " | " . $currenttab;
	} else {
		$title .= $_SESSION['settings']['naamwebsite'];
	}

   if (!isset($_GET['header']) or $_GET['header'] != "no") {
		echo("<!DOCTYPE HTML>\n");
		echo("<html lang='nl'>\n");
		echo("<head>\n");
		printf("<script src='%s/includes/javascript.js' async></script>\n", BASISURL);
		if ($currenttab2 == "Wijzigen mailing") {
			if (!isset($_GET['mid']))  {
				$_GET['mid'] = 0;
			}
			if (file_exists(BASEDIR . '/maatwerk/email.css') and (new Mailing($_GET['mid']))->zonderbriefpapier == 0) {
				$ss = "content_css: 'maatwerk/email.css',";
			} else {
				$ss = "";
			}
			if ($_SESSION['settings']['mailing_type_editor'] == 3) {
				$img = "";
				if (isset($_GET['mid']) and $_GET['mid'] > 0) {
					$dir_media = (new Mailing($_GET['mid']))->dir_media;
					if (is_dir($dir_media)) {
						$md = dir($dir_media);
						$il = "";
						while (false !== ($entry = $md->read())) {
							if (endswith($entry, "jpg") or endswith($entry, "jepg") or endswith($entry, "bmp") or endswith($entry, "gif") or endswith($entry, "png")) {
								if (strlen($il) > 0) {
									$il .= ",\n";
								}
								$il .= sprintf("{title: '%s', value: '%s'}", $entry, BASISURL. '/media/' . intval($_GET['mid']) . '/' . $entry);
							}
						}
						if (strlen($il) > 0) {
							$img = sprintf("image_list: [\n%s],", $il);
					}
				}
				$src = "/includes/tinymce/js/tinymce/tinymce.min.js";
				if (file_exists(BASEDIR . $src)) {
					$src = BASISURL . $src;
				} else {
					$src = sprintf("https://cdn.tiny.cloud/1/%s/tinymce/5/tinymce.min.js", $_SESSION['settings']['mailing_tinymce_apikey']);
				}
				printf("<script src='%s' referrerpolicy='origin'></script>\n", $src);
				printf("<script>
		tinymce.init({
			selector: '#message',
			%s
			%s
			height: 600,
			plugins: 'hr link lists table image',
			menu: {
				table: { title: 'Table', items: 'inserttable | cell row column | tableprops deletetable' }
			},
			toolbar: 'undo redo | styleselect | bold italic | alignleft aligncenter alignright alignjustify | bullist numlist | link image | hr',
			menubar: 'edit insert format tools table'
		});
  </script>\n", $ss, $img);
				}
			} elseif ($_SESSION['settings']['mailing_type_editor'] == 2) {
				echo("<script src='https://cdn.ckeditor.com/4.13.0/standard/ckeditor.js'></script>\n");
			} else {
				echo("<script src='https://cdn.ckeditor.com/ckeditor5/12.4.0/classic/ckeditor.js'></script>\n");
			}
		}
		printf("<title>%s</title>\n", $title);
		if (file_exists("default.css")) {
			printf("<link rel='stylesheet' href='default.css?v%s'>\n", strftime("%Y%m%d%H%M", filectime("default.css")));
		}
		if (file_exists("maatwerk/kleur.css")) {
			printf("<link rel='stylesheet' href='maatwerk/kleur.css?v%s'>\n", strftime("%Y%m%d%H%M", filectime("maatwerk/kleur.css")));
		}
		echo("</head>\n");
		echo("<body>\n");
		if ($breed == 0) {
			echo("<div id='container'>\n");
		} else {
			echo("<div id='containerbreed'>\n");
		}
		echo("<header>\n");
		printf("<h1>%s</h1>\n", $_SESSION['settings']['naamwebsite']);
		
		if (strlen($_SESSION['settings']['url_eigen_help']) < 10) {
			$htmlhelp = "<a href='https://software.telling.nl/phpRBM/uitleg.php'><img src='images/question.png' title='Uitleg gebruik en installatie' alt='Question mark'></a>\n";
		} else {
			$htmlhelp = sprintf("<a href='%s'><img src='images/question.png' title='Helpfunctie' alt='Question mark'></a>\n", $_SESSION['settings']['url_eigen_help']);
		}
	
		echo("<div id='login'>\n");
		
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
		} elseif ((new cls_Login())->aantal() > 0) {
			echo("<form name='Login' action='/index.php' method='post'>\n");
			echo("<label>Login:</label><input type='text' name='username'>\n");
			echo("<label>Wachtwoord:</label><input type='password' name='password'>\n");
			echo("<input type='submit' value='Inloggen' name='Inloggen'>\n");
			echo("<input type='checkbox' name='cookie' value=1 title='Ingelogd blijven (werkt met cookies)'>\n");
			echo($htmlhelp);
			echo("</form>\n");
		}

		echo("</div>  <!-- Einde login -->\n");
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
		printf("<footer>\n
				 <div id='footerleft'>%s</div>\n
				 <a href='%s'>%s</a> %s\n
				 <div id='footerright'>%s</div>\n
				 </footer>\n", $cr, BASISURL, $_SESSION['settings']['naamwebsite'], $np, strftime("%e %B %Y", $bestandsversie));
		echo("</div>  <!-- Einde container of containerbreed -->\n");
		echo("</body>\n");
		echo("</html>\n");
	}
} # HTMLfooter

function fnDispMenu($toonnivo=1, $xtra="") {
	global $tabpages, $currenttab, $currenttab2, $currenttab3, $navpad;
	
	$tpd = ""; // Dit wordt getoond in het menu.
	$ulopen = false;

	if (strlen($xtra) > 0 and startwith($xtra, "&amp;") == false) {
		$xtra = "&amp;" . $xtra;
	}
	
	if ($_SESSION['settings']['typemenu'] == 1 or $_SESSION['settings']['typemenu'] == 3) {
		printf("<div id='menu_l%s'>\n", $toonnivo);
	}

	if (($_SESSION['settings']['typemenu'] == 2 or $_SESSION['settings']['typemenu'] == 3) and $toonnivo == 1) {  // Dit is het menu met hovering
		echo("<ul id='nav'>\n");
		$nivo = 0;
		foreach($tabpages as $tp) {
			$cl = "";
			if (strstr($tp, "/") === false) {
				if ($nivo > 0) {
					if ($ulopen) {
						echo("		</ul>\n	");
						$ulopen = false;
					}
					echo("</li>\n");
				}
				$nivo = 1;
				$tpd = $tp;
				$tp1 = $tp;
				$ulopen = false;
				if ($currenttab == str_replace("'", "", $tpd)) {
					$cl = " class='current'";
					$navpad[$nivo]['naam'] = $tpd;
					$navpad[$nivo]['url'] = sprintf("%s?tp=%s%s", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra);
				}
				printf("	<li%s><a href=\"%s?tp=%s%s\">%s</a>", $cl, $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra, $tpd);
			} elseif (strpos($tp, "/") > 1 and startwith($tp, $tp1) == true) {
				if ($nivo == 1) {
					echo("\n		<ul>\n");
					$ulopen = true;
				}
				$nivo = 2;
				$tpd = substr($tp, strpos($tp, "/")+1);
			
				if ($currenttab2 == str_replace("'", "", $tpd)) {
					$cl = " class='current'";
					$navpad[$nivo]['naam'] = $tpd;
					$navpad[$nivo]['url'] = sprintf("%s?tp=%s%s", $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra);
				}
				printf("			<li%s><a href=\"%s?tp=%s%s\">%s</a></li>\n", $cl, $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra, $tpd);
			}
		}
		if ($ulopen) {
			echo("		</ul>\n</li>\n");
		}
		echo("</ul>\n");
	}
	
	$nivo = $toonnivo;
//	debug($nivo);
//	print_r($tabpages);
	if ($_SESSION['settings']['typemenu'] == 1 or ($_SESSION['settings']['typemenu'] == 3 and $toonnivo == 2)) {
		printf("<ul class='tablist%d'>\n", $nivo);
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
				printf("<li%s><a href=\"%s?tp=%s%s\">%s</a></li>\n", $cl, $_SERVER['PHP_SELF'], urlencode(str_replace("'", "", $tp)), $xtra, $tpd);
			}
		}
		echo("</ul>\n");
	}
	
	if ($_SESSION['settings']['typemenu'] == 1 or $_SESSION['settings']['typemenu'] == 3) {
		printf("</div> <!-- Einde menu_l%s -->\n", $toonnivo);
	}
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
	} else {
		return $riz;
	}
}

function fnDisplayForm($row, $xtra_eind="", $id="formulier") {

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
					$dv = strftime('%e %B %Y', strtotime($value));
				}
				$ret .= sprintf("<tr><td class='label'>%s</td><td class='datum'>%s</td></tr>\n", substr($key, 3), $dv);
			} elseif (substr($key, 0, 3) == "dtt" and (!is_null($value))) {
				$ret .= sprintf("<tr><td class='label'>%s</td><td class='datumtijd'>%s</td></tr>\n", substr($key, 3), strftime('%e %B %Y (%H:%M)', strtotime($value)));
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

function fnDisplayFormLabels($row, $xtra_eind="", $id="formulier") {

	if (strlen($id) > 0) {
		$ret = sprintf("<div id='%s'>\n", $id);
	}
	if (isset($row) and $row != null) {
		foreach($row as $key => $value) {
			if (substr($key, 0, 3) == "Kop") {
				$ret .= sprintf("<h2>%s</h2>\n", $value);
			} elseif (substr($key, 0, 3) == "dte" and (!is_null($value))) {
				if (strtotime($value) == false) {
					$dv = $value;
				} else {
					$dv = strftime('%e %B %Y', strtotime($value));
				}
				$ret .= sprintf("<label>%s</label><span>%s</span>\n", substr($key, 3), $dv);
			} elseif (substr($key, 0, 3) == "dtt" and (!is_null($value))) {
				$ret .= sprintf("<label>%s<span>%s</span>\n", substr($key, 3), strftime('%e %B %Y (%H:%M)', strtotime($value)));
			} elseif (substr($key, 0, 3) == "boo" and $value == 1) {
				$ret .= sprintf("<label>%s</label><span>Ja</span>\n", substr($key, 3));
			} elseif (substr($key, 0, 3) == "boo" and $value != 1) {
				$ret .= sprintf("<label>%s</label><span>Nee</span>\n", substr($key, 3));
			} elseif (substr($key, 0, 3) == "mem" and strlen($value) > 1) {
				$ret .= sprintf("<value>%s</value>\n", htmlentities($value));
			} elseif (substr($key, 0, 3) == "eml" and strlen($value) > 1) {
				$ret .= sprintf("<label>%s</label><span>%s</span>\n", substr($key, 3), fnDispEmail($value, "", 1));
			} elseif (substr($key, 0, 2) != "nd" and strlen($value) > 0) {
				$ret .= sprintf("<label>%s</label><span>%s</span>\n", $key, htmlentities($value));
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

function fnDisplayTable($rows, $link="", $th="", $disptot=0, $link_lk="", $xtra_regel="", $class="", $xtra_kolom="", $editkols="", $autosubmit=0, $sortkols=null, $rowscbo=null) {
	global $currenttab;
	
	if (strlen($class) == 0) {
		$class = "lijst";
	}
	$ret = sprintf("<div class='%s'>\n", $class);
	$kol_pk = "";
	if ($autosubmit == 1) {
		$js = " OnBlur='this.form.submit();'";
	} else {
		$js = "";
	}
	if (isset($rows) and (count($rows) > 0 or strlen($xtra_regel) > 0)) {
		if (strlen($class) > 0) {
			$ret .= sprintf("<table id='%s'>\n", $class);
		} else {
			$ret .= "<table>\n";
		}
		$dispkop = 1;
		$totaantal = 0;
		foreach($rows as $row) {
			if ($dispkop == 1) {
				$hv = "<thead>\n<tr>";
				$kn = 0;
				$skn = 0;
				foreach($row as $key => $value) {
					$sortlink = "";
					$ht = "";
					if (strstr("cbo, chk, dte, dtt, dtl, dts, boo, int, cur, eml, num, mrg, pwd, lnm", substr($key, 0, 3)) !== false) {
						$ht = str_replace("_", " ", substr($key, 3));
					} elseif (startwith($key, "lnk")) {
						if (strlen($link) > 1) {
							$hv .= "<th>&nbsp;</th>";
							$ktot[$kn] = "geen";
						}
					} elseif (startwith($key, "llk")) {
						if (strlen($link_lk) > 1) {
							$hv .= sprintf("<th colspan=%d>&nbsp;</th>", substr_count($link_lk, "<td")+1);
							$ktot[$kn] = "geen";
						}
					} elseif (startwith($key, "lek")) {
						if (strlen($xtra_kolom) > 1) {
							$hv .= sprintf("<th colspan=%d>&nbsp;</th>", substr_count($link_lk, "<td")+1);
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
					} elseif ($key == "RecordID" or $key == "ndRecordID") {
						$ktot[$kn] = 0;
					} elseif (substr($key, 0, 2) != "nd" and substr($key, 0, 3) != "lnk" and substr($key, 0, 3) != "llk") {
						$ktot[$kn] = "geen";
					}
					if (strlen($kol_pk) == 0 and ($key == "RecordID" or $key == "ndRecordID" or startwith($key, "lnk") or startwith($key, "llk"))) {
						$kol_pk = $key;
					}
					$kn++;
				}
				if (strlen($th) > 1) {
					$ret .= sprintf("<caption>%s</caption>\n", $th);
				}
				$ret .= $hv . "</tr>\n</thead>\n<tbody>\n";
				$dispkop = 0;
			}
			$ret .= "<tr>";
			$kn = 0;
			foreach($row as $key => $value) {
				if (startwith($key, "chk")) {
					if (strstr($editkols, $key) === false) {
						if ($value == 1) {
							$ret .= "<td>Ja</td>";
						} else {
							$ret .= "<td>Nee</td>";
						}
					} else {
						$ret .= sprintf("<td><input type='checkbox'%s name='%s_%d' value=1%s></td>", $js, str_replace(" ", "_", substr($key, 3)), $row->$kol_pk, checked($value, "", 1));
					}
					
				} elseif ($key == "cboFunctie") {
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
					
				} elseif (startwith($key, "dte") or startwith($key, "rodte")) {
					if (startwith($key, "dte") and strstr($editkols, $key) !== false) {
						$ret .= sprintf("<td><input type='date' name='%s_%d' value='%s'%s></td>", str_replace(" ", "_", substr($key, 3)), $row->$kol_pk, $value, $js);
					} elseif ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td class='datum'>%s</td>", strftime('%e %B %Y', strtotime($value)));
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
				} elseif (substr($key, 0, 3) == "dtl") {
					if ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td class='datumlang'>%s</td>", strftime('%A %e %B %Y', strtotime($value)));
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
				} elseif (startwith($key, "dtt") or startwith($key, "rodtt")) {
					if (startwith($key, "dtt") and strstr($editkols, $key) !== false) {
						$ret .= sprintf("<td><input type='datetime' name='%s_%d' value='%s'></td>", substr($key, 3), $row->$kol_pk, $value);
					} elseif ((!is_null($value)) and $value > '1900-01-01') {
						$ret .= sprintf("<td class='datumtijd'>%s</td>", strftime('%e %B %Y (%H:%M)', strtotime($value)));
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
				} elseif (substr($key, 0, 3) == "dts") {
					if ((!is_null($value)) and $value > '1900-01-01' and strtotime($value) !== false) {
						$ret .= sprintf("<td class='datumtijd'>%s</td>", strftime('%e %B %Y (%H:%M:%S)', strtotime($value)));
					} elseif (strlen($value) > 0) {
						$ret .= sprintf("<td>%s</td>", $value);
					} else {
						$ret .= "<td>&nbsp;</td>";
					}
				} elseif (substr($key, 0, 3) == "int") {
					if (strstr($editkols, $key) === false) {
						$ret .= sprintf("<td class='number'>%d</td>", $value);
					} else {
						$ret .= sprintf("<td><input type='number' name='%s_%d' value='%s'></td>", substr($key, 3), $row->$kol_pk, $value);
					}
					$ktot[$kn] += intval($value);
				} elseif (substr($key, 0, 3) == "cur") {
					$ret .= sprintf("<td class='number'>%s</td>", number_format($value, 2, ",", "."));
					$ktot[$kn] += $value;
				} elseif (substr($key, 0, 3) == "boo" and $value == 1) {
					$ret .= "<td>Ja</td>";
				} elseif (substr($key, 0, 3) == "boo" and $value != 1) {
					$ret .= "<td>Nee</td>";
					
				} elseif (substr($key, 0, 3) == "lnm" and strlen($value) > 0) {
					if (is_numeric($value)) {
						$ret .= sprintf("<td>%s</td>", (new cls_lid())->Naam($value, "onbekend"));
					} else {
						$ret .= sprintf("<td>%s</td>", $value);
					}
					
				} elseif (startwith($key, "lnk")) {
					if (strlen($link) > 1) {
						if ($value > 0) {
							$href = str_replace("%d", $value, $link);
							$href = str_replace("%s", "<img src='" . BASE64_DETAILS . "' alt='Details' title='Details'>", $href);
							$ret .= "<td class='details'>" . $href . "</td>";
						} else {
							$ret .= "<td>&nbsp;</td>";
						}
					}
				} elseif (startwith($key, "llk")) {
					if ($value > 0 and strlen($link_lk) > 1) {
						$href = str_replace("%d", $value, $link_lk);
						$href = str_replace("%s", "<img src='images/zoom.png' alt='Details'>", $href);
						$ret .= "<td class='details'>" . $href . "</td>";
					} elseif (strlen($link_lk) > 1) {
						$ret .= "<td></td>\n";
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
					if (strstr($editkols, $key) === false) {
						$ret .= sprintf("<td>%s</td>", fnDispEmail($value, "", 1));
					} else {
						$ret .= sprintf("<td><input type='email' name='%s_%d' value='%s' class='email' %s></td>", str_replace(" ", "_", substr($key, 3)), $row->$kol_pk, $value, $js);
					}
				} elseif (startwith($key, "pwd")) {
					if (strstr($editkols, $key) !== false) {
						$ret .= "<td></td>";
					} else {
						$ret .= sprintf("<td><input type='password' name='%s_%d'></td>", substr($key, 3), $row->$kol_pk);
					}
				} elseif (substr($key, 0, 3) === "num") {
					$ret .= sprintf("<td class='number'>%s</td>", $value);
				} elseif (substr($key, 0, 3) == "nm2") {
					$ret .= sprintf("<td class='number'>%s</td>", number_format(floatval($value), 2, ",", "."));
				} elseif (substr($key, 0, 3) == "mrg") {
					$ret .= sprintf("<td>%s</td>", str_replace(" &amp; ", "<br>", htmlentities($value)));
				} elseif (substr($key, 0, 2) != "nd") {
					if (strstr($editkols, $key) === false or $row->$kol_pk == 0) {
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
			$ret .= sprintf("<tr>%s</tr>\n", $xtra_regel);
		}
		$ret .= "</tbody>\n";
		if ($disptot > 0 and $totaantal > 1) {
			$ret .= "<tfoot>\n<tr>";
			for ($kn=0; $kn < count($ktot); $kn++) {
				if ($kn+1 == $disptot and $totaantal > 1) {
					$ret .= sprintf("<th>%s records</th>", number_format($totaantal, 0, ",", "."));
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
	$ret .= sprintf("</div>  <!-- Einde %s -->\n", $class);
	return $ret;
	
} # fnDisplayTable

function fnOrderBy($p_arrSort) {
	$ord = "";
	
	if (isset($_GET['sort'])) {
		$_SESSION['huidsort'] = $_GET['sort'];
	} else {
		$_SESSION['huidsort'] = -1;
	}
	
	if ($_SESSION['huidsort'] >= 50) {
		$sk = $_SESSION['huidsort'] - 50;
	} else {
		$sk= $_SESSION['huidsort'];
	}
	
	$sw = "";
	if ($sk >= 0 and array_key_exists($sk, $p_arrSort)) {
		$sw = $p_arrSort[$sk];
		if (strpos($sw, ";") > 0) {
			$sw = explode(";", $sw)[1];
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

function debug($tekst, $alleenwm=1, $alert=0, $logact=0) {
	if (is_array($tekst) or is_object($tekst)) {
		$tekst = print_r($tekst, true);
	}
	if ($_SERVER["HTTP_HOST"] == "phprbm.telling.nl") {
		$alleenwm = 0;
	}
	if ($alleenwm != 1 or (isset($_SESSION['webmaster']) and $_SESSION['webmaster'] == 1)) {
		if ($alert == 1) {
			printf("<script>alert('Debug: %s');</script>\n", $tekst);
		} else {
			printf("<p class='debug'>Debug: %s</p>\n", $tekst);
		}
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
//	debug("startwith: ");
//	debug($haystack);
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
	} elseif ($tp === "checkbox" and ($val == 1 or $val === true)) {
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

?>
