// Tamedhon MUDlib
//
// shells/elf.c -- Elven Shell
//
// $Date: 2023/07/19 $
/* $Revision: 4.2 $
 * $Log: elf.c,v $
 *
 * Revision 4.2  2023/07/19 Aloy
 * mehr Individualisierungsoptionen
 * av Gewicht für alle gleich! Auch Frauen können Muckis haben!
 * Frisuren und Rassemerkmale für alle gleich
 *
 * Revision 4.1  2008/01/14 22:00:00  TamTam
 * Vorbereitung fuer Augen-/Haar- und Hautfarbe+RassenSpecial
 *
 * Revision 1.3  2001/01/12 18:57:50  Samtpfote
 * Eintragung von Start/Pray-Room
 *
 * Revision 1.2  2000/06/11 20:30:44  Samtpfote
 * TT_* eingebaut
 *
 * Revision 1.1  2000/06/11 20:24:39  Samtpfote
 * Initial revision
 *
 * Revision 3.8  1997/11/25 10:56:07  Angrox
 * Id "Elfenspieler" hinzugefuegt
 *
 * Revision 3.7  1997/11/05 16:25:26  Woody
 * neuer Temperaturbereich
 *
 * Revision 3.6  1997/10/09 15:16:54  Woody
 * P_MATERIAL_KNOWLEDGE gesetzt
 *
 * Revision 3.5  1997/10/07 09:24:14  Woody
 * neue Attribute, Food/Drink/Alc-Werte
 *
 * Revision 3.4  1997/09/17 14:41:40  Woody
 * neue Rassenbeschreibung, _query_hp/sp_delay()
 *
 * Revision 3.3  1997/06/26 00:08:56  Woody
 * P_RACE_TT_MV gesetzt
 *
 * Revision 3.2  1997/05/07 17:10:30  Feyaria
 * startpunkt im elfendorf
 *
 * Revision 3.1  1997/04/08 20:29:11  Woody
 * Typo in der Rassenbeschreibung raus
 *
 * Revision 3.0  1997/03/13 02:12:31  Woody
 * prepared for revision control
 *
 * Revision 2.9  02-02-1996  00:00:08  Mupfel
 * P_TEMPERATURE_RANGE
 *
 * Revision 2.8  1995/04/26  15:19:42  Jof
 * Typo fixed
 *
 * Revision 2.7  1995/04/26  15:18:27  Jof
 * Toeters Rassenbeschreibung
 *
 * Revision 2.6  1995/04/24  11:58:11  Rochus
 * Groesse 195 cm.
 *
 * Revision 2.5  1994/09/13  11:44:37  Rochus
 * Int+3, Dex+2, Con+1
 *
 * Revision 2.4  1994/06/24  00:11:14  Jof
 * *** empty log message ***
 *
 * Revision 2.3  1994/06/21  18:43:19  Jof
 * RACE... in _query umgewandelt (save mem)
 *
 * Revision 2.2  1994/03/21  16:48:13  Jof
 * Reindented files using emacs - once again
 *
 * Revision 2.1  1994/01/26  16:31:23  Freund
 * Aufgrund neuer Elfensiedlung wurde Prayroom geaendert. -Freund.
 *
 * Revision 2.0  1994/01/13  17:00:18  mud
 * *** empty log message ***
 *
 * Revision 1.3  1994/01/03  13:39:01  mud
 * *** empty log message ***
 *
 */

#pragma strong_types

inherit "std/player/base";

#include <properties.h>
#include <attributes.h>
#include <class.h>
#include <environment.h>
#include <moving.h>

int _query_new_size()
{
  int avs;
  avs=QueryProp(P_AVERAGE_SIZE);
  return avs + random((avs/5)) - (avs/10);
}
int _query_new_weight()
{
  int avw;
  avw=QueryProp(P_AVERAGE_WEIGHT);
  return avw + random((avw/5))-(avw/10);
}

void create()
{
  base::create();
  AddClass(CL_HUMANOID);
  AddClass(CL_ELF);
  SetDefaultHome("/gilden/abenteurer");
  SetPrayRoom("/gilden/abenteurer");
  SetProp(P_AVERAGE_SIZE, 195);
  SetProp(P_AVERAGE_WEIGHT, 70000);
  SetProp(P_ATTRIBUTES_OFFSETS,
      ([ A_INT:4, A_DEX:3, A_CON:1, A_STR:-2 ]) );
  SetProp(P_MSGIN, "wandelt leichten Fußes herein");
  SetProp(P_MSGOUT, "wandelt leichten Fußes");
  SetProp(P_MMSGIN, "verdichtet sich aus einer magischen Wolke");
  SetProp(P_MMSGOUT, "löst sich in einer duftenden Wolke auf");
  SetProp(P_MAX_FOOD, 100);
  SetProp(P_MAX_DRINK, 130);
  SetProp(P_MAX_ALCOHOL, 70);
  SetProp(P_FOOD_DELAY, 10);
  SetProp(P_DRINK_DELAY, 8);
  SetProp(P_ALCOHOL_DELAY, 5);
  SetProp(P_GUILD, "abenteurer");
  SetProp(P_TEMPERATURE_RANGE,([T_RANGE_MIN:10,T_RANGE_MAX:30]));
  SetProp(P_LIGHT,0.3);
  SetProp(P_RACE_TT_MV,
      ([ TT_OTHER:      4,
         TT_CAVE:       13,
         TT_HOUSE:      6,
         TT_HILL:       8,
         TT_FOREST:     3,
         TT_TOWN:       11,
         TT_MOUNTAIN:   13,
         TT_DESERT:     9,
         TT_JUNGLE:     6,
         TT_SWAMP:      15,
         TT_PLAIN:      6,
         TT_ARCTIC:     12,
	 TT_TUNDRA:	11,
         TT_UNDERWATER: 9,
	 TT_WATER:	9,
         TT_ROAD:       5 ]) );
  SetProp(P_MATERIAL_KNOWLEDGE,
      ([ MATGROUP_WOOD:30,
         MATGROUP_HERBAL:30,
         MATGROUP_LIVING:20 ]) );
  AddId("Elfenspieler");
}

string _query_race()
{
  return "Elf";
}

string _query_racedescr()
{
  return break_string(
    "Elfen sind große, hagere Wesen, etwa 1,90 m von Statur. Ihr am "
    "meisten charakteristisches Merkmal sind wohl ihre langen, spitzen "
    "Ohren. Sie haben einen beeindruckenden Feinsinn für die Natur, "
    "der sie auch entstammen und der sie sich auch stark verbunden "
    "fühlen. Daher verwundert es auch nicht weiter, dass sie sich am "
    "liebsten in ruhige Wälder zurückziehen, um sich Dingen wie der "
    "Kunst und der Musik zu widmen. Größere Dörfer oder Städte "
    "hingegen werden von Elfen meist gemieden. Ein besonderer Wesenszug "
    "der Elfen ist wohl ihre außerordentliche Klugheit und Weisheit. "
    "Mit ihr haben sie schon so manchen überlegen geglaubten Feind "
    "besiegt. Ihre Intelligenz verleitet Elfen nur allzuoft zu "
    "hemmungsloser Arroganz - und wenn sie erstmal begonnen haben, "
    "über etwas zu philosophieren, kann man ihnen mitunter noch "
    "stundenlang zuhören...", 78);
}

int QueryAllowSelect() { return 1; }

int _query_hp_delay() {
  int re;
  re = Query(P_HP_DELAY);
  if (environment() && environment()->QueryProp(P_INDOORS))
    re++;
  else
    re--;
  return re;
}
int _query_sp_delay() {
  int re;
  re = Query(P_SP_DELAY);
  if (environment() && environment()->QueryProp(P_INDOORS))
    re++;
  else
    re--;
  return re;
}

string *_query_racestring()
{
  if (QueryProp(P_GENDER) == FEMALE)
      return ({"Elfe","Elfe","Elfe","Elfe"});
        return ({"Elf","Elf","Elf","Elf"});
}

string *_query_eye_colors()
{
  return ({"smaragdgrün", "saphirblau", "rubinrot", "amethystviolett",
           "goldgesprenkelt", "obsidianschwarz", "bernsteinfarben",
           "silbern"});
}

string *_query_hair_styles()
{
  return ({"langes, wallend-gelocktes Haar",
           "mittellanges, glattes Haar",
           "langes, glattes Haar",
           "lange geflochtene Zöpfe",
           "lange umwickelte Zöpfe",
           "wildes Haar"});
}

string *_query_hair_colors()
{
  return ({"goldblond", "nachtschwarz", "eichenbraun", "mondweiß",
           "blauschwarz", "sonnengelb", "hellblond", "silbern",
           "mallorngolden", "flammenrot", "blattgrün", "mystisch lila"});
}

string *_query_skin_colors()
{
  return ({"goldbraun", "bronzeschimmernd", "eichenbraun", "mondweiß",
           "platingrau", "nächtlich lila", "eisblau", "sonnig hell"});
}

string *_query_race_specials()
{
  return ({"hat lange, spitze Elfenohren",
           "hat lange Elfenohren",
           "hat spitze Elfenohren",
           "hat lange, spitze gepiercte Elfenohren",
           "hat lange gepiercte Elfenohren",
           "hat spitze gepiercte Elfenohren",
           "hat lange, spitze Elfenohren mit Ohrschmuck",
           "hat lange Elfenohren mit Ohrschmuck",
           "hat spitze Elfenohren mit Ohrschmuck",});
}

