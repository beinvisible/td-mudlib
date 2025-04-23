/* SilberLand MUDlib
 *
 * /secure/parentmaster.c -- Verwaltet Verwandtschaften (unter Magiern)
 *
 * $Date: 1997/04/10 17:35:26 $
 * $Revision: 1.3 $
 * $Log: parentmaster.c,v $
 * Revision 1.3  1997/04/10 17:35:26  Woody
 * Kein std/thing mehr (security), remove() neu geschrieben
 *
 * Revision 1.2  1997/03/15 23:31:51  Woody
 * prepared for revision control
 *
 * 96/12/12  1.1  Woody
 * RemoveChild()
 *
 * 96/05/27  1.0  Woody
 * Initial Revision
 *
 */

#include <properties.h>
#include <daemon.h>
#include <wizlevels.h>
#include "parentmaster.h"

mapping relations;


void create() {
  if (clonep(this_object())) return destruct(this_object());
  relations = m_allocate(10,2);
  seteuid(getuid(this_object()));
  restore_object(PARENT_SAVEFILE);
}

nomask int remove()
{
  save_object(PARENT_SAVEFILE);
  destruct(this_object());
  return 1;
}

void SetParent (string wiz, string parent)
{
  if ((object_name(previous_object()) != "/secure/merlin")
      && !IS_ARCH(this_interactive()))
    return;
  if (!member(relations, wiz))
    relations += ([ wiz: ""; ({ }) ]);
  relations[wiz,REL_PARENT]=parent;
  save_object(PARENT_SAVEFILE);
}

void SetChild (string wiz, string child)
{
  if ((object_name(previous_object()) != "/secure/merlin")
      && !IS_ARCH(this_interactive()))
    return;
  if (!member(relations, wiz)) relations += ([ wiz: ""; ({ }) ]);
  relations[wiz,REL_CHILDREN]+=({child});
  save_object(PARENT_SAVEFILE);
}

void RemoveChild (string wiz, string child)
{
  if ((object_name(previous_object()) != "/secure/merlin")
      && !IS_ARCH(this_interactive()))
    return;
  if (!member(relations, wiz)) return;
  relations[wiz,REL_CHILDREN]-=({child});
  save_object(PARENT_SAVEFILE);
}

void RemoveWiz (string wiz)
{
  string p;
  if ((getuid(previous_object())!=wiz)
      && !IS_ARCH(this_interactive()))
    return;
  if (!member(relations, wiz)) return;
  if (member(relations, p=relations[wiz,REL_PARENT]))
    relations[p,REL_CHILDREN]-=({wiz});
  relations = m_delete (relations, wiz);
  save_object(PARENT_SAVEFILE);
  // Anmerkung: Die Nachkommen des entfernten Wizzards haben
  // sinnvollerweise weiterhin wiz als REL_PARENT gesetzt.
}

string GetParent (string wiz)
{
  if (!member(relations, wiz)) return 0;
  return relations[wiz,REL_PARENT];
}

string *GetChildren (string wiz)
{
  if (!member(relations, wiz)) return ({ });
  return relations[wiz,REL_CHILDREN];
}

