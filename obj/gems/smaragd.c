//  Smaragd

inherit "std/unit";

#include <properties.h>
#include <language.h>

varargs string long()
{
  return break_string(parse_color_string("Der ^GSmaragd^N ist ein giftgrüner Edelstein."));
}

string _query_long()
{
  return long();
}

void create()
{
  if (!clonep(this_object()) )  return;

  ::create();

  SetProp(P_SHORT,parse_color_string("Ein ^GSmaragd^N"));
  SetProp(P_INFO,parse_color_string(
    "^GSmaragde^N können Gifte heilen, aber auch giftige Wirkungen "
    "verstärken.\n"));
  Set(P_NAME, ({parse_color_string("^GSmaragd^N"),parse_color_string("^GSmaragde^N"),"Smaragd"}) );
  SetProp(P_GENDER, MALE);

  AddSingularId( ({"smaragd", "edelstein"}) );
  AddPluralId( ({"smaragde", "edelsteine"}) );
  AddId( "EDELSTEIN" );
  SetCoinsPerUnits(550,1);
  SetGramsPerUnits(5,1);

  SetProp(P_MATERIAL, MAT_EMERALD );
  SetProp(P_NOBUY, 1);

}
