// Tamedhon MUDlib
//
// std/shells/dwarf.c -- Dwarven Shell
//
// $Date: 2023/07/19 $
/* $Revision: 4.2 $
 * $Log: dwarf.c,v $
 *
 * Revision 4.2  2023/07/19 Aloy
 * av Gewicht für alle gleich! Auch Frauen können Muckis haben!
 * Frisuren und Rassemerkmale für alle gleich
 * mehr Individualisierungsoptionen
 *
 * Revision 4.1  2008/01/14 22:00:00  TamTam
 * Vorbereitung fuer Augen-/Haar- und Hautfarbe+RassenSpecial
 *
 * Revision 1.3 2007/12/18 16:15:00 Gralkor
 * Die Zwerge haben eine eigene Gilde und nen Prayroom
 *
 * Revision 1.2  2000/06/11 20:28:34  Samtpfote
 * TT_* eingebaut
 *
 * Revision 1.1  2000/06/11 20:24:31  Samtpfote
 * Initial revision
 *
 * Revision 3.6  1997/11/25 10:00:13  Angrox
 * Id "Zwergenspieler" hinzugefuegt
 *
 * Revision 3.5  1997/11/05 16:25:26  Woody
 * neuer Temperaturbereich
 *
 * Revision 3.4  1997/10/09 15:13:22  Woody
 * P_MATERIAL_KNOWLEDGE gesetzt
 *
 * Revision 3.3  1997/10/07 09:24:14  Woody
 * neue Attribute, Food/Drink/Alc-Werte
 *
 * Revision 3.2  1997/09/17 14:41:00  Woody
 * neue Rassenbeschreibung, _query_hp/sp_delay()
 *
 * Revision 3.1  1997/06/26 00:00:29  Woody
 * P_RACE_TT_MV gesetzt
 *
 * Revision 3.0  1997/03/13 02:11:58  Woody
 * prepared for revision control
 *
 * Revision 2.8  01-24-1997  13:00:00  Woody
 * DefaultHome und PrayRoom gesetzt
 *
 * Revision 2.7  02-02-1996  00:00:08  Mupfel
 * P_TEMPERATURE_RANGE
 *
 * Revision 2.6  1995/04/24  11:58:41  Rochus
 * Groesse 120 cm
 *
 * Revision 2.5  1995/03/21  18:29:01  Boing
 * _query_racedescr -> return hat gefehlt, ausserdem ist die funktion
 * jetzt auf string getyped
 *
 * Revision 2.4  1994/09/13  11:45:29  Rochus
 * Str+2, Dex+1, Con+3
 *
 * Revision 2.3  1994/06/24  00:11:14  Jof
 * *** empty log message ***
 *
 * Revision 2.2  1994/06/21  18:43:19  Jof
 * RACE... in _query umgewandelt (save mem)
 *
 * Revision 2.1  1994/03/21  16:48:13  Jof
 * Reindented files using emacs - once again
 *
 * Revision 2.0  1994/01/13  17:00:17  mud
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/03  13:39:01  mud
 * *** empty log message ***
 *
 */

inherit "std/player/base";

#include <properties.h>
#include <attributes.h>
#include <environment.h>
#include <moving.h>
#include <class.h>
#include <combat.h>

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

  SetDefaultHome("/gilden/abenteurer");
  SetPrayRoom("/gilden/abenteurer");

  AddClass(CL_HUMANOID);
  AddClass(CL_DWARF);

  SetProp(P_AVERAGE_SIZE,120);
  SetProp(P_AVERAGE_WEIGHT,75000);
  SetProp(P_MSGIN, "stapft herein");
  SetProp(P_MSGOUT, "stapft");
  SetProp(P_MMSGIN, "bildet sich aus einem Haufen Stein");
  SetProp(P_MMSGOUT, "zerfällt zu einem Steinhaufen");
  SetProp(P_ATTRIBUTES_OFFSETS,
      ([ A_STR:3, A_DEX:1, A_CON:4, A_INT:-2 ]) );
  SetProp(P_MAX_FOOD, 80);
  SetProp(P_MAX_DRINK, 90);
  SetProp(P_MAX_ALCOHOL, 130);
  SetProp(P_FOOD_DELAY, 12);
  SetProp(P_DRINK_DELAY, 11);
  SetProp(P_ALCOHOL_DELAY, 3);
  SetProp(P_GUILD, "abenteurer");
  SetProp(P_TEMPERATURE_RANGE,([T_RANGE_MIN:10,T_RANGE_MAX:30]));
  SetProp(P_LIGHT,0.6);
  SetProp(P_RACE_TT_MV,
      ([ TT_OTHER:      4,
         TT_CAVE:       3,
         TT_HOUSE:      4,
         TT_HILL:       7,
         TT_FOREST:     9,
         TT_TOWN:       9,
         TT_MOUNTAIN:   3,
         TT_DESERT:     9,
         TT_JUNGLE:     14,
         TT_SWAMP:      20,
         TT_PLAIN:      6,
         TT_ARCTIC:     12,
	 TT_TUNDRA:	11,
	 TT_WATER:	20,
         TT_UNDERWATER: 15,
         TT_ROAD:       5 ]) );
  SetProp(P_RESISTANCE_STRENGTHS,
	  ([ DT_FIRE : -0.2,
	     DT_WATER : 0.4,
	     DT_UNHOLY: 0.2 ]));
  SetProp(P_MATERIAL_KNOWLEDGE,
      ([ MATGROUP_STONE:30,
         MATGROUP_METAL:30,
         MATGROUP_PRECIOUS_METAL: 40,
         MAT_GOLD:100 ]) );
  AddId("Zwergenspieler");
}

string _query_race()
{
  return "Zwerg";
}

string _query_racedescr()
{
  return break_string(
    "Ein Zwerg ist nur im Gebirge so richtig zu Hause. Dort, in seinen "
    "kühlen, dunklen Stollen und Höhlen fühlt er sich wohl, das ist "
    "seine Welt. "
    "Zwerge sind etwas größer als Hobbits, etwa 1,20 m. Diese geringe "
    "Größe gleichen sie mit ungewöhnlicher Ausdauer, unbeirrbarer "
    "Kampfkraft und Mut sowie einer gehörigen Portion roher Kraft "
    "spielend wieder aus. Hat er sich erstmal etwas in den Kopf gesetzt, "
    "hat es keinen Sinn zu versuchen, es ihm wieder auszureden. Vielmehr "
    "läuft man in diesen Momenten Gefahr, den Jähzorn und das Misstrauen "
    "eines Zwerges kennenzulernen. Besonderes Geschick wird Zwergen auch "
    "beim Anfertigen und im Umgang mit Waffen nachgesagt - sie sind nicht "
    "nur gute Handwerker sondern auch geschickte wenn auch nicht sonderlich "
    "intelligente Kämpfer. Hat ein Zwerg einmal Freundschaft geschlossen, "
    "so kann man sich felsenfest auf ihn verlassen. Doch auch Feiern ist "
    "er nicht abgeneigt - seine Ausdauer betrifft nicht nur Kampfkraft "
    "sondern auch Trinkfestigkeit.", 78);
}

int QueryAllowSelect() { return 1; }

int _query_hp_delay() {
  int re;
  re = Query(P_HP_DELAY);
  if (environment() && environment()->QueryProp(P_INDOORS))
    re--;
  else
    re++;
  return re;
}

int _query_sp_delay() {
  int re;
  re = Query(P_SP_DELAY);
  if (environment() && environment()->QueryProp(P_INDOORS))
    re--;
  else
    re++;
  return re;
}


string *_query_racestring()
{
  if (QueryProp(P_GENDER) == FEMALE)
      return ({"Zwergin","Zwergin","Zwergin","Zwergin"});
        return ({"Zwerg","Zwergs","Zwerg","Zwerg"});
}

string *_query_eye_colors()
{
  return ({"stahlblau", "schwarz", "grau", "blaugrau",
           "blau", "dunkelblau", "steingrau"});
}

string *_query_hair_styles()
{
  return ({"langes, wildgelocktes Haar",
           "mittellanges, lockiges Haar",
           "langes, gelocktes Haar",
           "kurzgeschnittenes Haar",
           "mittellanges, wuschiges Haar",
           "mittellanges, glattes Haar"});
}

string *_query_hair_colors()
{
  return ({"rostrot", "aschgrau", "kohleschwarz", "staubgrau",
           "feuerrot", "schiefergrau", "schwarzgrau", "flammenrot"});
}

string *_query_skin_colors()
{
  return ({"rostbraun", "aschgrau", "staubgrau",
           "schiefergrau", "hellgrau", "sandighell"});
}

string *_query_race_specials()
{
  return ({"trägt einen kleinen Spitzbart",
           "trägt einen struppigen Kurzbart",
           "trägt einen rauschenden Kurzbart",
           "trägt keinen Bart",
           "trägt voller Stolz einen kleinen Spitzbart",
           "trägt voller Stolz einen langen, struppigen Vollbart",
           "trägt voller Stolz einen langen, rauschenden Vollbart",
           "trägt voller Stolz einen zu einem Zopf geflochteten Vollbart",
           "trägt voller Stolz einen zu zwei Zöpfen geflochteten Vollbart",
           "trägt voller Stolz einen zu drei Zöpfen geflochteten Vollbart",
           "trägt voller Stolz einen zu einigen Zöpfen geflochteten",
           "Vollbart"});
}

