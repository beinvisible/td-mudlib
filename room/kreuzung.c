#include <properties.h>

inherit "std/room";

void create()
{
    ::create();
    
    SetProp(P_INT_SHORT, "Eine Kreuzung im Untergrund von Tamedhon");
    SetProp(P_INT_LONG, break_string(
	"Du befindest Dich irgendwo im Untergrund von Tamedhon auf "+
	"einer ausgelatschten Kreuzung. Das einzige was hier wirklich "+
	"ist, ist ein großer Wegweiser, der in alle möglichen "+
	"Richtungen zeigt."));
    SetProp(P_ENVIRONMENT, ([ENV_TERRAIN: TT_HOUSE, ENV_TEMPERATURE: 15]));
    SetProp(P_LIGHT, 1);
    
    AddDetail("wegweiser", "Lesen bildet.\n");
    AddReadDetail("wegweiser",
	" <--- DEBUG (Osten) --\n"+
	" -- RAUM DER ENTSCHEIDUNG (Westen) --->\n"+
	" <--- LEHRLINGSBUDE (Norden) --\n"+
	" -- MAGIEKONTROLLZENTRUM (Süden) --->\n"+
	" <--- DAS NICHTS (unten) --\n"+
	" -- GEFÄNGNIS (rechts) --->\n\n"+
	"Darunter, auf einer Tafel.\n"+
	" ABENTEURERGILDE .... hinten\n"+
	" BALANCEOFFICE ...... oben\n");
	
    AddExit("osten", "/room/debug");
    AddExit("westen", "/room/entscheidung");
    AddExit("norden", "/room/lehrlingsbude");
    AddExit("süden", "/room/mkz");
    AddExit("unten", "/room/void");
    AddExit("rechts", "/room/jail");
    AddExit("oben", "/room/balance");
    AddExit("hinten", "/gilden/abenteurer");
}
