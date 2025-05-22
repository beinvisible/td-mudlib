// Diamant  

inherit "std/unit";

#include <properties.h>
#include <language.h>

varargs string long()
{
  return break_string(parse_color_string("^WDiamanten^N gelten nicht umsonst als die wertvollsten und schönsten "
    "Edelsteine. Der ^WDiamant^N funkelt und glitzert im Licht, als ob er "
    "selber eine Lichtquelle wäre. Außerdem gibt es kaum ein Material, "
    "das härter ist, als ein ^WDiamant^N."));
}

string _query_long()
{
  return long();
}

void create()
{
  if (!clonep(this_object()) )  return;

  ::create();

  SetProp(P_SHORT,parse_color_string("Ein ^WDiamant^N"));
  SetProp(P_INFO, break_string(parse_color_string(
    "Wenn ein Lichtstrahl auf einen ^WDiamanten^N trifft, kann man schon fast "
    "von einer Lichtexplosion sprechen. Und tatsächlich haben viele "
    "^WDiamanten^N eine starke Beziehung zu explosiven Kräften."),78));
  Set(P_NAME, ({parse_color_string("^WDiamant^N"),parse_color_string("^WDiamanten^N"),"Diamant"}) );
  SetProp(P_GENDER, MALE);

  AddSingularId( ({"diamant", "edelstein"}) );
  AddPluralId( ({"diamanten", "edelsteine"}) );
  AddId( "EDELSTEIN" );
  SetCoinsPerUnits(800,1);
  SetGramsPerUnits(5,1);

  SetProp(P_MATERIAL, MAT_DIAMOND );
  SetProp(P_NOBUY, 1);

}
