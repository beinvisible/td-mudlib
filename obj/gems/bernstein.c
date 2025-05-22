// Bernstein 

inherit "std/unit";

#include <properties.h>
#include <language.h>

varargs string long()
{
  return break_string(parse_color_string("In diesem leuchtend gelben ^YBer^N^Ln^N^Yste^N^Li^N^Yn^N ist ein winziges Käferchen "
    "eingeschlossen."));
}

string _query_long()
{
  return long();
}

void create()
{
  if (!clonep(this_object()) )  return;

  ::create();

  SetProp(P_SHORT,parse_color_string("Ein ^YBer^N^Ln^N^Yste^N^Li^N^Yn^N"));
  SetProp(P_INFO, break_string(parse_color_string(
    "In manchen Sagen heißt es, ^YBer^N^Ln^N^Yste^N^Li^N^Yn^N sei eingefrorenes Licht, aber "
    "die Ähnlichkeit zu Baumharz ist kaum von der Hand zu weisen. Trotzdem "
    "besitzen ^YBer^N^Ln^N^Yste^N^Li^N^Yne^N eine sonderbare Beziehung zu Licht."),78));
  Set(P_NAME, ({parse_color_string("^YBer^N^Ln^N^Yste^N^Li^N^Yn^N"),parse_color_string("^YBer^N^Ln^N^Yste^N^Li^N^Yne^N"),"Bernstein"}) );
  SetProp(P_GENDER, MALE);

  AddSingularId( ({"bernstein", "edelstein"}) );
  AddPluralId( ({"bernsteine", "edelsteine"}) );
  AddId( "EDELSTEIN" );
  SetCoinsPerUnits(180,1);
  SetGramsPerUnits(5,1);

  SetProp(P_MATERIAL, MAT_AMBER );
  SetProp(P_NOBUY, 1);

}
