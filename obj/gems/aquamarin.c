// Aquamarin 

inherit "std/unit";

#include <properties.h>
#include <language.h>

string long()
{
  return break_string(parse_color_string("Dieser ^W^BAq^N^Buam^N^W^Bar^N^Bin^N schimmert in meerblauen Farben."));
}

string _query_long()
{
  return long();
}

void create()
{
  if (!clonep(this_object()) )  return;

  ::create();

  SetProp(P_SHORT,parse_color_string("Ein ^W^BAq^N^Buam^N^W^Bar^N^Bin^N"));
  SetProp(P_INFO, break_string(parse_color_string(
    "Der ^W^BAq^N^Buam^N^W^Bar^N^Bine^N hat eine wasserblaue Farbe. Scheinbar hat er eine enge "
    "Beziehung zu Wasser."),78) );
  Set(P_NAME, ({parse_color_string("^W^BAq^N^Buam^N^W^Bar^N^Bin^N"),parse_color_string("^W^BAq^N^Buam^N^W^Bar^N^Bine^N"),"Aquamarin"}) );
  SetProp(P_GENDER, MALE);

  AddSingularId( ({"aquamarin", "edelstein"}) );
  AddPluralId( ({"aquamarine", "edelsteine"}) );
  AddId( "EDELSTEIN" );
  SetCoinsPerUnits(200,1);
  SetGramsPerUnits(5,1);

  SetProp(P_MATERIAL, MAT_AQUAMARINE );
  SetProp(P_NOBUY, 1);

}
