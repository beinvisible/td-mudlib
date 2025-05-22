//  Topas

inherit "std/unit";

#include <properties.h>
#include <language.h>

varargs string long()
{
  return break_string(parse_color_string("Der ^LTopas^N ist ein sonnengelber Edelstein."));
}

string _query_long()
{
  return long();
}

void create()
{
  if (!clonep(this_object()) )  return;

  ::create();

  SetProp(P_SHORT,parse_color_string("Ein ^LTopas^N"));
  SetProp(P_INFO, 
    parse_color_string("^LTopase^N haben eine luftige Ausstrahlung.\n"));
  Set(P_NAME, ({parse_color_string("^LTopas^N"),parse_color_string("^LTopase^N"),"Topas"}) );
  SetProp(P_GENDER, MALE);

  AddSingularId( ({"topas", "edelstein"}) );
  AddPluralId( ({"topase", "edelsteine"}) );
  AddId( "EDELSTEIN" );
  SetCoinsPerUnits(180,1);
  SetGramsPerUnits(5,1);

  SetProp(P_MATERIAL, MAT_TOPAZ );
  SetProp(P_NOBUY, 1);

}
