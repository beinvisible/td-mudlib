// Lapislazuli 

inherit "std/unit";

#include <properties.h>
#include <language.h>

string long()
{
  return break_string(parse_color_string("^W^GLapislazuli^N ist ein ätzend grüner Edelstein."));
}

string _query_long()
{
  return long();
}

void create()
{
  if (!clonep(this_object()) )  return;

  ::create();

  SetProp(P_SHORT,parse_color_string("Ein ^W^GLapislazuli^N"));
  SetProp(P_INFO,parse_color_string(
    "Dem ^W^GLapislazuli^N wird eine Beziehung zu ätzenden Säuren nachgesagt.\n"));
  Set(P_NAME, ({parse_color_string("^W^GLapislazuli^N"),parse_color_string("^W^GLapislazulis^N"),"Lapislazuli"}));
  SetProp(P_GENDER, MALE);

  AddSingularId( ({"lapislazuli", "edelstein"}) );
  AddPluralId( ({"lapislazulis", "edelsteine"}) );
  AddId( "EDELSTEIN" );
  SetCoinsPerUnits(170,1);
  SetGramsPerUnits(5,1);

  SetProp(P_MATERIAL, MAT_MISC_JEWEL );
  SetProp(P_NOBUY, 1);

}
