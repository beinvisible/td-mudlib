// SilberLand MUDlib
//
// std/weapon/description.c -- weapon description handling
//
// $Date: 1997/10/10 22:46:23 $
/* $Revision: 1.6 $
 * $Log: description.c,v $
 * Revision 1.7  2002/02/28 16:50:03  Gralkor
 *
 * Revision 1.6  1997/10/10 22:46:23  Woody
 * Bugfix in dam_desc()
 *
 * Revision 1.5  1997/10/08 13:46:20  Woody
 * dam_desc(), P_DAM_DESC in create() auf Default setzen
 *
 * Revision 1.4  1997/09/30 21:31:28  Woody
 * ::long() in long() verwenden (damit process_string() wieder geht)
 *
 * Revision 1.3  1997/09/25 15:13:37  Woody
 * _query_material()
 *
 * Revision 1.2  1997/08/25 15:31:04  Feyaria
 * long ueberscheirben fuer anzeige der becshaedigung
 *
 * Revision 1.1  1997/06/25 12:11:36  Woody
 * Initial revision
 *
 */

#pragma strong_types

inherit "/std/thing/description";

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <thing/description.h>
#include <properties.h>
#include <combat.h>

void create()
{
  ::create();
  SetProp (P_DAM_DESC, DFLT_DAM_DESC);
}

mixed dam_desc()
{
  string re;
  mixed desc;
  int x,dam;

  if (!QueryProp(P_NAME) || !QueryProp(P_SHORT) || !desc=QueryProp(P_DAM_DESC))
    return 0;
  if (!x = QueryProp(P_WC)+(dam=QueryProp(P_DAMAGED)))
    return 0;
  if (stringp(desc))
    return (dam>(x/2)) ? desc : 0;
  if (!pointerp(desc) || !sizeof(desc)) return 0;
  x = ((sizeof(desc)-1)*dam/x);
  if (stringp(desc[x]))
    return desc[x];
  return 0;
}

string short()
{
  string s;
  if (!(s=QueryProp(P_SHORT)) || QueryProp(P_INVIS)) return 0;
  return s+(QueryProp(P_WIELDED) ?
    " ("+(QueryProp(P_WIELDED_TEXT)||"gezückt")+").\n" : ".\n");
}

varargs string long()
{
  int dmg;
  string s;

  s=dam_desc();
  return ::long() +
    (s ? capitalize(QueryPronoun(WER))+" ist in einem "+s+"en Zustand.\n":"");
}

mapping _query_material() {
  mixed res,wt;

  if (mappingp(res=Query(P_MATERIAL)))
    return res;
  wt=QueryProp(P_WEAPON_TYPE);
  switch(wt) {
    case WT_KNIFE:
      return ([MAT_LEATHER:10,MAT_METAL:90]);
    case WT_AXE:
     return ([MAT_WOOD:80,MAT_IRON:20]);
    case WT_SLASHING:
     return ([MAT_LEATHER:10,MAT_IRON:90]);
    case WT_CLUB:
     return ([MAT_WOOD:100]);
    case WT_SWORD:
     return ([MAT_IRON:100]);
    case WT_INFANTRY:
     return ([MAT_WOOD:80,MAT_IRON:20]);
    case WT_STAFF:
     return ([MAT_WOOD:100]);
    case WT_SPEAR:
     return ([MAT_WOOD:100]);
    case WT_CHAIN:
     return ([MAT_WOOD:40,MAT_IRON:60]);
    case WT_THRUST:
     return ([MAT_IRON:100]);
    case WT_WHIP:
     return ([MAT_LEATHER:100]);
    case WT_SLING:
     return ([MAT_LEATHER:100]);
    case WT_BOW:
     return ([MAT_WOOD:80,MAT_CANNABIS:20]);
    case WT_XBOW:
     return ([MAT_IRON:40,MAT_WOOD:50,MAT_CANNABIS:10]);
    case WT_AMMU:
      return ([MAT_MISC:100]);
    case WT_MAGIC:
      return ([MAT_MAGIC:100]);  
    case WT_MISC:
      return ([MAT_MISC:100]);
    default:
      return ([MAT_MISC_METAL:100]);
  }
}

