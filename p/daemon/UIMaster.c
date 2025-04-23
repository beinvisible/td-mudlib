/* Tamedhon MUDlib 
 *
 * UIMaster.c - UniqueItemsMaster
 *            -> verwaltet die UniqueItems
 *
 * 2004/03/12 14:00:00 Serii@Tamedhon
 */

#include <properties.h>
#include <daemon.h>
#include <moving.h>
#include <wizlevels.h>

#include "/d/unterwelt/def.h"

#define SAVE_FILE "/p/daemon/save/UItems"

mapping ST_Aitems=([]);

static mapping Uitems=([]);

void create() {
  seteuid(getuid());
  restore_object(SAVE_FILE);
}

void Save() {
save_object(SAVE_FILE);
}

void reset() {
Save();
}

int remove() {return 0;}

static int secure()
{
  if (!previous_object()) return 0;
  if (geteuid(previous_object())==ROOTID) return 1;
  if (geteuid(previous_object()) != geteuid(this_interactive())) return 0;
  if (this_interactive() != this_player()) return 0;
  if (!IS_ARCH(this_interactive())) return 0;
  return 1;
}

varargs object UI_clone_object(string ui,string si,int bp)
{object ob;
  if(!ui||!si)return 0;
  if(ui==si)return 0;
  if(member(ST_Aitems,si))return 0;
  if(sizeof(Uitems[ui]))
    if(objectp(Uitems[ui][0]))return clone_object(si);
  if(!bp)ob=clone_object(ui);
  else
  {
    if(catch(call_other(ui,"???")))
      raise_error(sprintf("%O - failed to load\n",ui));
    ob=find_object(ui);
  }
  if(!mappingp(ST_Aitems[ui])){ST_Aitems[ui]=([0:ob->Name(WER),1:ob->QueryProp(P_ARMOUR_TYPE),2:ob->QueryProp(P_WEAPON_TYPE)]);Save();}
  Uitems[ui]=({ob,si});
  return ob;
}

string UI_get_si(string ui)
{
  return Uitems[ui][1];
}

int UI_is_ui(string sui)
{
  return member(ST_Aitems,sui);
}

void Dump()
{
if(!secure())return;
}
