//  Rubin

inherit "std/unit";

#include <properties.h>
#include <language.h>
#include <ansi.h>

varargs string long()
{
  return break_string(parse_color_string("Der ^RRubin^N ist ein wertvoller Edelstein und hat eine feuerrote Farbe."));
}

string _query_long()
{
  return long();
}

void create()
{
  if (!clonep(this_object()) )  return;

  ::create();

  SetProp(P_SHORT,parse_color_string("Ein ^RRubin^R"));
  SetProp(P_INFO, 
    "Rubine haben eine eigenartige Beziehung zu Feuer.\n");
  Set(P_NAME, ({parse_color_string("^RRubin^N"),parse_color_string("^RRubine^N"),"Rubin"}));
  SetProp(P_GENDER, MALE);

  AddSingularId( ({"rubin", "edelstein"}) );
  AddPluralId( ({"rubine", "edelsteine"}) );
  AddId( "EDELSTEIN" );
  SetCoinsPerUnits(650,1);
  SetGramsPerUnits(5,1);

  SetProp(P_MATERIAL, MAT_RUBY );
  SetProp(P_NOBUY, 1);

}
