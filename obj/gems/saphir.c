// Saphir 

inherit "std/unit";

#include <properties.h>
#include <language.h>

string long()
{
  return break_string(parse_color_string("Der ^BSaphir^N ist ein eisblau glitzernder Edelstein."));
}

string _query_long()
{
  return long();
}

void create()
{
  if (!clonep(this_object()) )  return;

  ::create();

  SetProp(P_SHORT,parse_color_string("Ein ^BSaphir^N"));
  SetProp(P_INFO,parse_color_string(
    "Der ^BSaphir^N hat scheinbar eine enge Beziehung zum Eis.\n"));
  Set(P_NAME, ({parse_color_string("^BSaphir^N"),parse_color_string("^BSaphire^N"),"Saphir"}) );
  SetProp(P_GENDER, MALE);

  AddSingularId( ({"saphir", "edelstein"}) );
  AddPluralId( ({"saphire", "edelsteine"}) );
  AddId( "EDELSTEIN" );
  SetCoinsPerUnits(600,1);
  SetGramsPerUnits(5,1);

  SetProp(P_MATERIAL, MAT_SAPPHIRE);
  SetProp(P_NOBUY, 1);

}
