// Tamedhon MUDlib
//
// SHELLS/HOBBIT.C -- Hobbit Shell
//
// $Date: 2023/07/19 $
/* $Revision: 4.3 $
 * $Log: hobbit.c,v $
 *
 * Revision 4.3  2023/07/19 Aloy
 * av Gewicht für alle gleich! Auch Frauen können Muckis haben!
 * Frisuren und Rassemerkmale für alle gleich
 * mehr Individualisierungsoptionen
 *
 * Revision 4.2  2008/09/06 17:00:00  TamTam
 * P_MMSGIN + P_MMSGOUT richtig gesetzt
 *
 * Revision 4.1  2008/01/14 22:00:00  TamTam
 * Vorbereitung fuer Augen-/Haar- und Hautfarbe+RassenSpecial
 *
 * Revision 1.3  2000/09/04 18:10:00  Querolin
 * PrayRoom und DefaultHome ins neue Hobbitdorf gelegt.
 *
 * Revision 1.2  2000/06/11 20:31:44  Samtpfote
 * TT_* eingebaut
 *
 * Revision 1.1  2000/06/11 20:24:48  Samtpfote
 * Initial revision
 *
 * Revision 3.7  1997/11/25 10:57:16  Angrox
 * Id "Hobbitspieler" hinzugefuegt
 *
 * Revision 3.6  1997/11/05 16:25:26  Woody
 * neuer Temperaturbereich
 *
 * Revision 3.5  1997/11/03 18:26:57  Feyaria
 * Startraum in hobbingen
 *
 * Revision 3.4  1997/10/09 15:18:11  Woody
 * P_MATERIAL_KNOWLEDGE gesetzt
 *
 * Revision 3.3  1997/10/07 09:24:14  Woody
 * neue Attribute, Food/Drink/Alc-Werte
 *
 * Revision 3.2  1997/09/17 14:42:08  Woody
 * neue Rassenbeschreibung
 *
 * Revision 3.1  1997/06/26 00:08:56  Woody
 * P_RACE_TT_MV gesetzt
 *
 * Revision 3.0  1997/03/13 02:13:39  Woody
 * prepared for revision control
 *
 * 2.Februar 1996  P_TEMPERATURE_RANGE gesetzt von Mupfel
 * 17.August wg. Umstellung auf Lib-1.6 setzen von P_SIZE = 100 cm
 * 15.Juni prayroom und defHome auf Hobbitdorf gesetzt von Gundur
 * 9.April 1995  V1.0 Gundur
 */

#pragma strong_types

inherit "std/player/base";

#include <properties.h>
#include <attributes.h>
#include <environment.h>
#include <moving.h>
#include <class.h>

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
  AddClass(CL_HOBBIT);
  SetDefaultHome("/gilden/abenteurer");
  SetPrayRoom("/gilden/abenteurer");
  SetProp(P_AVERAGE_SIZE, 100);
  SetProp(P_AVERAGE_WEIGHT, 60000);
  SetProp(P_MSGIN, "schlendert gemütlich herein");
  SetProp(P_MSGOUT, "schlendert gemütlich");
  SetProp(P_MMSGOUT, "schnippt mit den Fingern und ist weg");
  SetProp(P_MMSGIN, "erscheint ganz unspektakulär");
  SetProp(P_ATTRIBUTES_OFFSETS,
      ([ A_INT:2, A_DEX:4, A_CON:1, A_STR:-1 ]) );
  SetProp(P_MAX_FOOD, 130);
  SetProp(P_MAX_DRINK, 70);
  SetProp(P_MAX_ALCOHOL, 100);
  SetProp(P_FOOD_DELAY, 8);
  SetProp(P_DRINK_DELAY, 11);
  SetProp(P_ALCOHOL_DELAY, 4);
  SetProp(P_GUILD, "abenteurer");
  SetProp(P_TEMPERATURE_RANGE,([T_RANGE_MIN:8,T_RANGE_MAX:28]));
  SetProp(P_LIGHT,0.0);
  SetProp(P_RACE_TT_MV,
      ([ TT_OTHER:      4,
         TT_CAVE:       6,
         TT_HOUSE:      6,
         TT_HILL:       3,
         TT_FOREST:     11,
         TT_TOWN:       8,
         TT_MOUNTAIN:   11,
         TT_DESERT:     13,
         TT_JUNGLE:     8,
         TT_SWAMP:      13,
         TT_PLAIN:      5,
         TT_ARCTIC:     12,
	 TT_TUNDRA:	11,
	 TT_WATER:	17,
         TT_UNDERWATER: 15,
         TT_ROAD:       5 ]) );
  SetProp(P_MATERIAL_KNOWLEDGE,
      ([ MATGROUP_EATABLE:30,
         MATGROUP_DRUG:30,
         MATGROUP_POISONOUS:10]) );
  AddId("Hobbitspieler");
}

string _query_race()
{
  return "Hobbit";
}

string _query_racedescr()
{
  return break_string(
    "Hobbits sind kleine Wesen, sogenannte Halblinge, etwa einen Meter von "
    "Statur. Sie sind relativ stark behaart und zeichnen sich durch ihre "
    "außerordentliche Geschicklichkeit, ihren Mut und ihre bewunderswerte "
    "Standfestigkeit aus. Ihre Heimat sind hügelige Täler und Ebenen, "
    "denn ebenso wie die Elfen wissen auch sie die Natur zu schätzen, "
    "ohne jedoch eine derart starke Bindung mit ihr einzugehen. Wenngleich "
    "Hobbits unermüdliche und vor allem auch kluge Kämpfer sein können, "
    "so ist das kriegerische Abenteurerleben doch nicht so recht ihre Sache. "
    "Viel lieber sitzen sie zu Hause in ihrer warmen, gemütlichen Stube vor "
    "dem Kamin, um ihr Pfeifchen zu rauchen oder sich die köstlichsten "
    "Speisen einzuverleiben. Denn Freundlichkeit und Gemütlichkeit sind "
    "Eigenschaften, die allen Hobbits gemeinsam sind.", 78);
}

int QueryAllowSelect() { return 1; }

string *_query_racestring()
{
  if (QueryProp(P_GENDER) == FEMALE)
      return ({"Hobbitfrau","Hobbitfrau","Hobbitfrau","Hobbitfrau"});
        return ({"Hobbit","Hobbits","Hobbit","Hobbit"});
}

string *_query_eye_colors()
{
  return ({"braun", "hellbraun", "mittelbraun", "dunkelbraun",
           "schwarz", "blau", "hellblau", "dunkelblau"});
}

string *_query_hair_styles()
{
  return ({"langes, wildes Haar",
           "mittellanges, stubbliges Haar",
           "langes, gelocktes Haar",
           "kurze Haare",
           "mittellanges, wildes Haar"});
}

string *_query_hair_colors()
{
  return ({"hellbraun", "braun", "dunkelbraun", "schwarz",
           "schwarzbraun", "mittelbraun", "braunschwarz", "dunkelblond"});
}

string *_query_skin_colors()
{
  return ({"hellbraun", "braun", "blass", "cremefarben"});
}

string *_query_race_specials()
{
  return ({"hat eine wohlgenährte Figur",
           "hat einen Kugelbauch",
           "ist etwas füllig",
           "hat haarige Füße",
           "hat eine Pfeife"});
}

