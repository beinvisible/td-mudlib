// SilberLand MUDlib
//
// /p/service/obj/zettel/zettel.c -- Der Anfaengerzettel
//
// $Date: 2008/02/03 04:30:00 $
/* $Revision: 1.3 $
 * $Log: zettel.c,v $
 *
 * Revision 1.3  2008/02/03 04:30:00  TamTam
 * Gewicht reduziert
 *
 * Revision 1.2  1997/09/25 15:19:49  Woody
 * Material Papier
 *
 * Revision 1.1  1997/03/16 17:56:43  Woody
 * prepared for revision control
 *
 */

// Entstanden aus /obj/anfaengerbuch.c
// 23-04-95  - Mupfel@AnderLand -

inherit "std/thing";

#include <properties.h>
#include <defines.h>

void create()
{
  if(IS_BLUE(ME)) return;
  ::create();
  SetProp(P_SHORT, "Ein Zettel mit Hinweisen");
  SetProp(P_LONG,
    "Du hälst einen mittelgroßen Zettel in Deinen Händen, den Du mit\n"+
    "'lies zettel' lesen kannst.\n");
  SetProp(P_NAME,"Hinweiszettel");
  SetProp(P_GENDER, MALE);
  SetProp(P_WEIGHT, 50);
  SetProp(P_NOBUY, 1);
  SetProp(P_NOSELL, 1);
  SetProp(P_MATERIAL,MAT_PAPER);
  AddId(({"zettel","hinweiszettel","obj$zettel"}));
  AddCmd(({"lies","lese"}), "lies");
}

int lies(string str)
{
  notify_fail("Was willst Du lesen ? Syntax: lies <etwas>.\n");
  if (!str) return 0;
  if (!id(str)) return 0;
  say(capitalize(PL->name()) + " liest einen Zettel.\n", PL);
  PL->more("/p/service/loco/obj/zettel/zettel.text");
  return 1;
}

void reset()
{
  if(IS_BLUE(ME)) return;
  if(!environment(ME)){remove(); return;};
  if(!interactive(environment(ME)))
  {
    if(IS_BLUE(environment(ME)))
      tell_room(environment(ME), "Ein Zettel zerfällt zu Staub.\n");
      {remove(); return;}
  }
}
