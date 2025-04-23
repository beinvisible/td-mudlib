// Bergkristall 

inherit "std/unit";

#include <properties.h>
#include <language.h>

string long()
{
  return break_string(parse_color_string("Ein wunderbar funkelnder ^WBer^N^S^Wgkr^N^Wist^N^S^Wall^N. Er reflektiert das Licht und "
    "und scheint dadurch in den verschiedensten Farben zu glänzen. Du "
    "kannst kaum den Blick von solcher Schönheit wenden. Er ist etwa 20 cm "
    "lang und 5 cm breit. Seine fünf Kanten laufen vorne zu einem "
    "gleichmäßigen Spitz zusammen."));
}

string _query_long()
{
  return long();
}

void create()
{
  if (!clonep(this_object()) )  return;

  ::create();

  SetProp(P_SHORT,parse_color_string("Ein ^WBer^N^S^Wgkr^N^Wist^N^S^Wall^N"));
  SetProp(P_INFO,parse_color_string(
    "Der ^WBer^N^S^Wgkr^N^Wist^N^S^Wall^N ist ein mystischer Stein mit Leben spendender Energie.\n"));
  Set(P_NAME, ({parse_color_string("^WBer^N^S^Wgkr^N^Wist^N^S^Wall^N"),parse_color_string("^WBer^N^S^Wgkr^N^Wist^N^S^Walle^N"),"Bergkristall"}));
  SetProp(P_GENDER, MALE);

  AddSingularId( ({"bergkristall", "kristall", "edelstein"}) );
  AddPluralId( ({"bergkristalle", "kristalle", "edelsteine"}) );
  AddId( "EDELSTEIN" );
  SetCoinsPerUnits(90,1);
  SetGramsPerUnits(5,1);

  SetProp(P_MATERIAL, MAT_QUARTZ );
  SetProp(P_NOBUY, 1);

}
