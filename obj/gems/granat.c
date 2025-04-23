// Granat 

inherit "std/unit";

#include <properties.h>
#include <language.h>

string long()
{
  return break_string(parse_color_string("Der ^RGra^N^W^Sn^N^Rat^N ist ein dunkelroter Edelstein."));
}

string _query_long()
{
  return long();
}

void create()
{
  if (!clonep(this_object()) )  return;

  ::create();

  SetProp(P_SHORT,parse_color_string("Ein ^RGra^N^W^Sn^N^Rat^N"));
  SetProp(P_INFO,break_string(parse_color_string(
    "Manche Leute behaupten, ein ^RGra^N^W^Sn^N^Rat^N zieht Blitze an.\n"),78));
  Set(P_NAME, ({parse_color_string("^RGra^N^W^Sn^N^Rat^N"),parse_color_string("^RGra^N^W^Sn^N^Rate^N"),"Granat"}) );
  SetProp(P_GENDER, MALE);

  AddSingularId( ({"granat", "edelstein"}) );
  AddPluralId( ({"granate", "edelsteine"}) );
  AddId( "EDELSTEIN" );
  SetCoinsPerUnits(80,1);
  SetGramsPerUnits(5,1);

  SetProp(P_MATERIAL, MAT_MISC_JEWEL );
  SetProp(P_NOBUY, 1);

}
