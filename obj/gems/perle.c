//  Perle

inherit "std/unit";

#include <properties.h>
#include <language.h>

string long()
{
  return break_string(parse_color_string("Diese Perle ist berauschend schön. Sie stammt aus dem Inneren einer "
    "Muschel."));
}

string _query_long()
{
  return long();
}

void create()
{
  if (!clonep(this_object()) )  return;

  ::create();

  SetProp(P_SHORT, "Eine Perle");
  SetProp(P_INFO, 
    "In Perlen sollen die Geräusche der Brandung gespeichert sein.");
  Set(P_NAME, ({"Perle", "Perlen", "Perle"}) );
  SetProp(P_GENDER, FEMALE);

  AddSingularId( ({"perle", "edelstein"}) );
  AddPluralId( ({"perlen", "edelsteine"}) );
  AddId( "EDELSTEIN" );
  SetCoinsPerUnits(270,1);
  SetGramsPerUnits(5,1);

  SetProp(P_MATERIAL, MAT_PEARL );
  SetProp(P_NOBUY, 1);

}
