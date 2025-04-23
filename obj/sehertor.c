//
// Sehertore (c) by Rochus
//
// Wo diese Tore stehen, koennen sich Seher zu anderen Toren hin teleportieren.
// Wenn sie Aufgestellt wurden muessen sie noch von einem Magier mit
// Level 40 oder mehr in eine Liste eingetragen werden.
//
inherit "/std/thing";

#include <properties.h>
#include <language.h>
#pragma strong_types
#define SDMASTER "/obj/sehertormaster"
#define P_SEERDOOR_DISCOVER "rw_sehertor_disc"

string short() {
  if (!SDMASTER->DoorIsKnown()) return 0;
  return ::short();
}

varargs string long() {
  if (!SDMASTER->DoorIsKnown()) return 0;
  return ::long();
}

varargs int id(string str) {
  if (!SDMASTER->DoorIsKnown()) return 0;
  return ::id(str);
}

void create () {
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_NOGET,1);
  SetProp(P_NAME,"Portal");
  SetProp(P_GENDER,NEUTER);
  AddId ("sehertor");
  AddId ("tor");
  AddId ("portal");
  AddId ("teleporter");
  AddId ("teleportertor");
  SetProp(P_SEERDOOR_DISCOVER,1);
  SetProp(P_SHORT,"Ein magisches Portal");
  SetProp(P_LONG,
"Dieses magische Portal ermöglicht es Dir, Dich zu einem anderen magischen\n"+
"Portal, das Du schon kennst, zu teleportieren.\n");
  SetProp (P_MATERIAL, MAT_MAGIC);
  SetProp(P_INFO,
  "Du kannst den Befehl \"teleportiere\" benutzen, um andere, bekannte "
  "Portale zu besuchen. ");
 
  AddCmd(({"teleportier","teleportiere"}),"tele");
}

varargs int tele(string str) {
  return SDMASTER->Teleport(str);
}

void tor_message() {
  write("Du hast soeben "+name(WEN)+" entdeckt.\n");
}

void init() {
  ::init();
  if (!QueryProp(P_SEERDOOR_DISCOVER)) return;
  if (SDMASTER->DiscoverDoor())
    call_out("tor_message",0);
}
