//  Turmalin

inherit "std/unit";

#include <properties.h>
#include <language.h>

varargs string long()
{
  return break_string(parse_color_string("Der ^S^WTu^N^Grm^N^S^Wal^N^Gin^N ist ein grünschwarzer Edelstein. Irgendwie ist er etwas "
    "unheimlich.."));
}

string _query_long()
{
  return long();
}

void create()
{
  if (!clonep(this_object()) )  return;

  ::create();

  SetProp(P_SHORT,parse_color_string("Ein ^S^WTu^N^Grm^N^S^Wal^N^Gin^N"));
  SetProp(P_INFO,parse_color_string(
    "Die Mächte des Bösen benutzen oft einen ^S^WTu^N^Grm^N^S^Wal^N^Gin^N für ihre "
    "widerwärtigen Taten.\n"));
  Set(P_NAME, ({parse_color_string("^S^WTu^N^Grm^N^S^Wal^N^Gin^N"),parse_color_string("^S^WTu^N^Grm^N^S^Wal^N^Gin^N^S^We^N"),"Turmalin"}));
  SetProp(P_GENDER, MALE);

  AddSingularId( ({"turmalin", "edelstein"}) );
  AddPluralId( ({"turmaline", "edelsteine"}) );
  AddId( "EDELSTEIN" );
  SetCoinsPerUnits(200,1);
  SetGramsPerUnits(5,1);

  SetProp(P_MATERIAL, MAT_MISC_JEWEL );
  SetProp(P_NOBUY, 1);

}
