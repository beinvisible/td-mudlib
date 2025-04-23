//  Zirkon

inherit "std/unit";

#include <properties.h>
#include <language.h>

string long()
{
  return break_string(parse_color_string("Der ^YZirkon^N ist ein Edelstein von brauner Farbe."));
}

string _query_long()
{
  return long();
}

void create()
{
  if (!clonep(this_object()) )  return;

  ::create();

  SetProp(P_SHORT,parse_color_string("Ein ^YZirkon^N"));
  SetProp(P_INFO, break_string(parse_color_string(
    "^YZirkone^N sollen angeblich sogar elektrischen Strom leiten können, "
    "aber was ist Strom überhaupt?."), 78));
  Set(P_NAME, ({parse_color_string("^YZirkon^N"),parse_color_string("^YZirkone^N"),"Zirkon"}) );
  SetProp(P_GENDER, MALE);

  AddSingularId( ({"zirkon", "edelstein"}) );
  AddPluralId( ({"zirkone", "edelsteine"}) );
  AddId( "EDELSTEIN" );
  SetCoinsPerUnits(350,1);
  SetGramsPerUnits(5,1);

  SetProp(P_MATERIAL, MAT_MISC_JEWEL );
  SetProp(P_NOBUY, 1);

}
