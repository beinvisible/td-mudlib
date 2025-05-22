// Amethyst 

inherit "std/unit";

#include <properties.h>
#include <language.h>

varargs string long()
{
  return break_string(parse_color_string("Ein kleiner, lila ^PAmethyst^N."));
}

string _query_long()
{
  return long();
}

void create()
{
  if (!clonep(this_object()) )  return;

  ::create();

  SetProp(P_SHORT,parse_color_string("Ein ^PAmethyst^N"));
  SetProp(P_INFO,break_string(parse_color_string(
  "Der ^PAmethyst^N ist relativ häufig und daher nicht so viel wert wie andere "
    "Edelsteine. Es heißt, er schützt vor Schlangenbiss und habe eine "
    " ernüchternde Wirkung.")));
  Set(P_NAME, ({parse_color_string("^PAmethyst^N"),parse_color_string("^PAmethyste^N"),"Amethyst"}) );
  SetProp(P_GENDER, MALE);

  AddSingularId( ({"amethyst", "edelstein"}) );
  AddPluralId( ({"amethyste", "edelsteine"}) );
  AddId( "EDELSTEIN" );
  SetCoinsPerUnits(50,1);
  SetGramsPerUnits(5,1);

  SetProp(P_MATERIAL, MAT_AMETHYST );
  SetProp(P_NOBUY, 1);

}
