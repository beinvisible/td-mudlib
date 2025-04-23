// SilberLand MUDlib
//
// std/armour/description.c -- armour description handling
//
// $Date: 1997/10/10 22:45:07 $
/* $Revision: 1.6 $
 * $Log: description.c,v $
 * Revision 1.6  1997/10/10 22:45:07  Woody
 * Bugfix in dam_desc()
 *
 * Revision 1.5  1997/10/08 13:29:15  Woody
 * dam_desc(), P_DAM_DESC in create() auf Default setzen
 *
 * Revision 1.4  1997/09/30 21:27:59  Woody
 * ::long() in long() verwenden (damit process_string() wieder geht)
 *
 * Revision 1.3  1997/09/25 15:05:09  Woody
 * _query_material(), "getragen" als einheitlichen Worn-Text
 *
 * Revision 1.2  1997/08/25 15:30:24  Feyaria
 * long ueberscheirben fuer anzeige der becshaedigung
 *
 * Revision 1.1  1997/06/25 11:48:04  Woody
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
  if (!x = QueryProp(P_AC)+(dam=QueryProp(P_DAMAGED)))
    return 0;
  if (stringp(desc))
    return (dam>(x/2)) ? desc : 0;
  if (!pointerp(desc) || !sizeof(desc)) return 0;
  x = ((sizeof(desc)-1)*dam/x);
  if (stringp(desc[x]))
    return desc[x];
  return 0;
}

varargs string short()
{
  string s;
  if (!(s=QueryProp(P_SHORT)) || QueryProp(P_INVIS)) return 0;
  return s+(QueryProp(P_WORN) ?
    " ("+(QueryProp(P_WORN_TEXT) || "getragen")+").\n":".\n");
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
  mixed res,at;

  if (mappingp(res=Query(P_MATERIAL)))
    return res;
  at=QueryProp(P_ARMOUR_TYPE);
  switch(at)
  {
    case AT_ARMOUR:
    case AT_HELMET:
    case AT_RING:
    case AT_SHIELD:
    case AT_BRACERS:
      return ([MAT_MISC_METAL:100]);
    case AT_AMULET:
      return ([MAT_MISC_METAL:100,MAT_MISC_MAGIC:100]);
    case AT_CLOAK:
    case AT_TROUSERS:
      return ([MAT_CLOTH:100]);
    case AT_MISC:
      return ([MAT_MISC:100]);
    case AT_GLOVE:
    case AT_BOOT:
    case AT_BELT:
    default:
      return ([MAT_LEATHER:100]);
  }
}

