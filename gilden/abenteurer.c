// Diese Datei ist Teil der Abenteurergilde (Gidhrog)
// ----------------------------------------------------------------------------
// Datei:        /gilden/abenteurer.c
// Autor:        Samtpfote
// Datum:        21.06.2000
// ----------------------------------------------------------------------------
// Beschreibung: Hauptstelle der ABler
// ----------------------------------------------------------------------------

#include <properties.h>
#include <rooms.h>
#include <wizlevels.h>
#include <language.h>
#include <defines.h>
#include <moving.h>
#include <new_skills.h>
#include <config.h>

inherit "/std/gilden_room";
inherit "/std/gilden_ob";
inherit "std/gilde";

void create()
{
  ::create();
  seteuid(getuid());


SetProp(P_GUILD_DEFAULT_SPELLBOOK,"ab_spells");
  SetProp(P_INDOORS, 1);
  SetProp(P_LIGHT, 1);
  SetProp(P_INT_SHORT, "Die Gilde der Abenteurer");
  SetProp(P_INT_LONG,
"Der Raum rings um Dich ist rustikal mit grob gezimmertem Eichenholz "
"bekleidet, das teilweise von einigen Gemälden verdeckt wird. Mehrere "
"Trophäen wurden unauffällig im Raum verteilt, so daß hier kaum noch "
"Platz für weiteres ist, ausser einigen Stühlen und einem Kamin am "
"Rande. Sonst fällt Dir noch eine Liste auf, die neben dem Eingang "
"hängt.\n");

  AddDetail("raum", "Du stehst mitten drin.");
  AddDetail(({"rand", "rande"}),
    "Der Kamin steht am Rand vor einer der Wände.");
  AddDetail("decke",
    "Die Decke ist aus groben Eichenbohlen gezimmert.");
  AddDetail(({"wand","waende","wände"}),
    "An den Wänden hängen verschiedene Gemälde, sonst sind "
    "sie bis auf eine Liste am Eingang kahl.");
  AddDetail("boden",
    "Der Boden besteht aus abgetretenen Eichenbohlen.");
  AddDetail(({"bohlen", "eichenbohlen"}),
    "Sie sind recht dunkel angelaufen. Das liegt wohl am Alter.");
  AddDetail("alter",
    "Jeder wird einmal alt.");
  AddDetail(({"gemälde","gemaelde"}),
    "Sie zeigen einige menschliche Helden. Neben einem Gemälde "
    "befindet sich - kaum sichtbar - eine Leiter, die nach "
    "oben führt.");
  AddDetail("leiter",
    "Wohin sie führen mag?");
  AddDetail("helden",
    "Sind alle mehr oder weniger schneidige Gesellen, die "
    "auf irgendeine Weise den Betrachter schief angrinsen "
    "und die Waffe stolz präsentieren.");
  AddDetail("gesellen",
    "Allesamt jung und mit diversen Narben uebersät.");
  AddDetail(({"narbe","narben"}),
    "Diese machen die Helden nicht hübscher.");
  AddDetail(({"betrachter"}),
    "Wer ist das wohl?");
  AddDetail(({"waffe", "schwerter", "schwert"}),
    "Verschiedene Schwerter, die blutbefleckt sind.");
  AddDetail(({"blut"}),
    "Rotes Blut!");
  AddDetail(({"tropäen","trophaeen"}),
    "Bei den Trophäen handelt es sich um mehrere angestaubte Pokale aus "
    "golden glänzendem Material, um verschiedene ausgestopfte Köpfe "
    "von Monstern und Ähnlichem. "
    "Daneben befindet sich eine kleine Vitrine.");
  AddDetail(({"pokale", "pokal"}),
    "Die Pokale wurden wohl in mehreren Wettbewerben gewonnen.");
  AddDetail(({"plaketten", "plakette"}),
    "Auf jedem Pokal prangt eine Plakette.");
  AddDetail("wettbewerbe",
    "Den Plaketten nach waren es wohl Wettbewerbe in der Art "
    "'Sauf-die-Gegnergilde-nieder', 'Metzelmaniac', "+
    "'Armdrücken' ...");
  AddDetail("material",
    "Bei genauerer Betrachtung zeigt sich, dass die Pokale leider nur "
    "aus vergoldetem Blech bestehen und kaum etwas wert sind.");
  AddDetail(({"köpfe","koepfe", "monster"}),
    "Du erblickst Köpfe von Löwen, Hirschen, Echsen, Riesenratten, "
    "Trollen und sogar einen von einem Drachen.");
  AddDetail(({"loewen", "loewe", "hirsche", "hirsch", "echsen", "echse",
    "ratte","ratten","riesenratte","riesenratten", "trolle", "troll", 
    "drachen", "drache"}),
    "Davon sind nur die Köpfe da, welche man einzeln betrachten kann.");
  AddDetail(({"loewenkopf","löwenkopf"}),
    "Was für eine zottelige Mähne der Löwenkopf hat. Irgendwer "
    "hat sich einen Spaß gemacht, und eine rosa Schleife reingebunden."
    "Allerdings sieht er nur entfernt aus wie ein Löwe. ");
  AddDetail(({"maehne","mähne"}),
    "Der Löwenkopf wirkt mit seiner Mähne fast lebendig."
    "Na sowas aber auch. Das ist doch Kunstfell! ");
  AddDetail(({"schleife", "stoff"}),
    "Eine zarte Schleife aus rosa Stoff.");
  AddDetail("hirschkopf",
    "Diese Hirsche wurden im Elfenwald erlegt und müssen im lebendigen "
    "Zustand echt riesig gewesen sein. Auf dem Geweih zählst Du "
    "satte sechs Verzweigungen.");
  AddDetail("verzweigungen",
    "Was für ein gewaltiger Hirsch!");
  AddDetail("elfenwald",
    "Der Elfenwald liegt nördlich von hier.");
  AddDetail("echsenkopf",
    "Es gibt nur einen kleinen, verschrumpelten Echsenkopf, der im "
    "Vergleich zum daneben hängenden Drachenkopf echt mickrig aussieht. "
    "Wahrscheinlich faellt er bald auseinander so vertrocknet ist er schon.");
  AddDetail(({"rattenkopf","riesenrattenkopf"}),
    "Dieser Kopf einer Ratte, wenn auch einer relativ großen Ratte. "
    "Da hat ein mutiger Abenteurer sich selbst übertroffen. ");
  AddDetail("trollkopf",
    "Vom Trollkopf ist nicht viel zu erkennen. Vor allem fallen Dir einige "
    "bräunlich-grüne Flechten auf, die über seine Hauer hängen.");
  AddDetail("hauer",
    "Der Trollkopf hat gewaltige Hauer.");
  AddDetail("menschenkopf",
    "Solche Köpfe werden hier in der Gilde niemals hängen.");
  AddDetail("flechten",
    "Sie wirken recht unspektakulär, aber unter ihnen entdeckst Du "
    "eine Reihe von Runen, die auf eine abgekratzte Schrift geschrieben "
    "wurden.");
  AddDetail("runen",
    ([0: "Du vermutest, dass sie zwergischen Ursprungs sind. "
         "Frag doch einen "
         "Zwerg, der sie fuer Dich übersetzt?",
      "zwerg": "Es handelt sich um zwergische Runen. "
               "Du liest: 'Dieser Troll "
      "wurde von Elrond Trollspaltersson erlegt'."
    ]));
  AddDetail(({"schrift","abgekratzte schrift"}),
  "Du kannst sie nicht mehr entziffern, aber es fing mit \"Cho\" "
  "an und hörte mit \"tät\" auf. ");
  AddDetail("drachenkopf",
    "Auch wenn der Drachenkopf neben dem mickrigen Echsenkopf hängt, "
    "bist Du Dir nicht ganz sicher, ob dieses grün glänzende, "
    "schuppige Ding wirklich einer ist. Besonders das kleine "
    "Schieferplättchen, dass am Halsansatz herunterhängt, irritiert "
    "Dich. Sonst wirkt es ziemlich authentisch: grüne Schuppen, "
    "große, weiße, blutbefleckte Zähne und eine heraushängende "
    "Züngelzunge.");
  AddDetail(({"plättchen","schieferplättchen","schieferplaettchen", 
              "plaettchen"}),
    "'Choppits -- Mythenplüschhändler erste Qualität'");
  AddDetail(({"zähne","drachenzähne","zaehne","drachenzaehne"}),
    "Du berührst Sie zögernd, und spürst, daß sie aus Ton sind!");
  AddDetail("schuppen", "Welche Schuppen meinst Du?\n");
  AddDetail("drachenschuppen",
    "Nach zögerlichen Berühren fühlen Sie sich lederartig an.\n");
  AddDetail("zunge", "Welche Zunge meinst Du?\n");
  AddDetail(({"drachenzunge", "züngelzunge","zuengelzunge"}),
    "Diese besteht aus gut verarbeiteter Drachenseide.\n");
  AddDetail(({"ton", "leder", "drachenseide"}),
    "Was für ein Schwindel!");
  AddDetail(({"stühle","stuhl", "stuehle"}),
    "Sie bestehen aus soliden Eichenholz und laden ein, es sich darauf "
    "bequem zu machen.");
  AddDetail("holz",
    "Aus Eiche.");
  AddDetail(({"eiche", "eichenholz"}),
    "Dunkles, fleckiges Holz, das ziemlich blank poliert wurde.");
  AddDetail("eingang",
    "Er führt hinaus auf die Straße.");
  AddDetail(({"straße","strasse"}),
    "Die Straße wirkt sehr belebt, wie offenbar ganz Gidhrog.");
  AddDetail("gidhrog",
    "Gidhrog ist eine Stadt der Menschen.");
  AddDetail(({"liste", "topliste"}),
    "Versuch doch die Topliste einmal zu lesen.\n");
  AddDetail("kamin",
    "Im Kamin knistert ein lustiges Feuer.\n");
  AddDetail("feuer",
    "Das Feuer macht es hier schön warm.");

  AddSmell(DEFAULT_SMELL,
    "Es riecht nach Schweiß der anwesenden Personen, harzigem "
    "Brennholz und Gebratenem.\n");
  AddSmell(({"holz", "brennholz"}),
    "Feines Brennholz aus dem Elfenwald, frisch gefällt.\n");
  AddSmell(({"schweiss","schweiß"}),
    ([0: "Du versuchst diesen Geruch zu verdrängen.\n",
      "zwerg": "Du erschnupperst den Geruch des Kampfes und des "
               "geselligen Biertrinkens.",
      "elf": "Widerlich! Menschenschweiß und Zwergen'duft' vermischen "
             "sich hier zu einer abscheulichen Kombination!",
      "hobbit": "Vielleicht würde etwas Parfum Abhilfe schaffen?",
      "mensch": "Er riecht nach Abenteurer, Aufregung und "
                "interessanten Geschichten.",
    ]));
  AddSmell("gebratenes",
    "Er rührt vom Kamin her.");

  AddSpell("blitz");
  AddSpell("identifiziere");
  AddSpell("schätz");
  AddSpell("pfeil");
  AddSpell("feuerball");
  AddSpell("schnell");

  AddExit("westen", 
         ([EX_DEST:"/room/kreuzung"]));
}
