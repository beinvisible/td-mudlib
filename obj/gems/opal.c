// Opal 

inherit "std/unit";

#include <properties.h>
#include <language.h>

varargs string long()
{
  return break_string(parse_color_string("Der ^PO^N^Yp^N^Ga^N^Cl^N schimmert magisch in allen Farben."));
}

string _query_long()
{
  return long();
}

void create()
{
  if (!clonep(this_object()) )  return;

  ::create();

  SetProp(P_SHORT,parse_color_string("Ein ^PO^N^Yp^N^Ga^N^Cl^N"));
  SetProp(P_INFO,parse_color_string(
    "Es gibt Gerüchte, die ^PO^N^Yp^N^Ga^N^Cl^N^Re^N mit Magie in Zusammenhang bringen.\n"));
  Set(P_NAME, ({parse_color_string("^PO^N^Yp^N^Ga^N^Cl^N"),parse_color_string("^PO^N^Yp^N^Ga^N^Cl^N^Re^N"),"Opal"}) );
  SetProp(P_GENDER, MALE);

  AddSingularId( ({"opal", "edelstein"}) );
  AddPluralId( ({"opale", "edelsteine"}) );
  AddId( "EDELSTEIN" );
  SetCoinsPerUnits(250,1);
  SetGramsPerUnits(5,1);

  SetProp(P_MATERIAL, MAT_OPAL );
  SetProp(P_NOBUY, 1);

}
