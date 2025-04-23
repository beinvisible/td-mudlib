/* Tamedhon MUDLIB
 * Samtpfote@Tamedhon
 *
 * Standardgeldobjekt
 *
 * $Date: 2001/01/12 19:04:30 $
 * $Revision: 1.1 $
 * $Log: money.c,v $
 * Revision 1.1  2001/01/12 19:04:30  Samtpfote
 * Initial revision
 *
 */
// 20031023 Gralkor: Geld modifiziert. Beschreibung etc
//
// 20050528 Serii: Singular/Plural-Ids "taler" und "tametaler" eingefuegt
//

inherit "std/unit";

#pragma strong_types

#include <properties.h>
#include <language.h>

void _set_autoloadobj( mixed args )
{
  if (pointerp(args))
    args=args[0];
  if (stringp(args))
    sscanf(args,"%d",args);
  if (!intp(args))
    args=0;
  SetProp(P_AMOUNT,args);
}

int _query_autoloadobj()
{
  return QueryProp(P_AMOUNT);
}

void create()
{
  if (!clonep(this_object()))
    return;
  ::create();
  SetProp(P_NAME,({"Tametaler","Tametaler"}));
  SetProp(P_SHORT,"Tametaler");
  SetProp(P_LONG,
  "Jeder Tametaler ist ein kleines goldenes Blättchen. "
  "Auf der einen Seite ist das Gesicht des gütigen Querolin "
  "eingeprägt. "
  "Auf der anderen Seite siehst Du das düstere Gesicht von Serii, "
  "der Dich angrummelt. ");
  SetProp(P_MATERIAL, MAT_GOLD);
  Set(P_INFO, break_string(
  "Wenn Du dem MUD etwas Gutes tun willst, solltest Du etwas Geld spenden. "
  "'hilfe spenden' erklärt Dir, WIE Du es tun kannst. "));
  AddSingularId("muenze");
  AddPluralId("muenzen");
  AddSingularId("goldstueck");
  AddPluralId("goldstuecke");
  AddSingularId("münze");
  AddPluralId("münzen");
  AddSingularId("goldstück");
  AddPluralId("goldstücke");
  AddSingularId("taler");
  AddPluralId("taler");
  AddSingularId("tametaler");
  AddPluralId("tametaler");
  AddId("taler");
  AddId("tametaler");        
  AddId("gold");
  AddId("geld");
  AddId("\ngeld");
  SetProp(P_GENDER,MALE);
  SetCoinsPerUnits(1,1);
  SetGramsPerUnits(1,5);
}
