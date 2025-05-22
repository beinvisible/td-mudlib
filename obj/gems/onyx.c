// Onyx 

inherit "std/unit";

#include <properties.h>
#include <language.h>

varargs string long()
{
  return break_string(parse_color_string("Der ^S^WOnyx^N ist ein Edelstein von schwarzer Farbe."));
}

string _query_long()
{
  return long();
}

void create()
{
  if (!clonep(this_object()) )  return;

  ::create();

  SetProp(P_SHORT,parse_color_string("Ein ^S^WOnyx^N"));
  SetProp(P_INFO, 
    "Manche Leute haben furchbare Angst vor diesem schrecklichen, schwarzen "
    "Edelstein.\n");
  Set(P_NAME, ({parse_color_string("^S^WOnyx^N"),parse_color_string("^S^WOnyxe^N"),"Onyx"}) );
  SetProp(P_GENDER, MALE);

  AddSingularId( ({"onyx", "edelstein"}) );
  AddPluralId( ({"onyxe", "edelsteine"}) );
  AddId( "EDELSTEIN" );
  SetCoinsPerUnits(150,1);
  SetGramsPerUnits(5,1);

  SetProp(P_MATERIAL, MAT_MISC_JEWEL );
  SetProp(P_NOBUY, 1);

}
