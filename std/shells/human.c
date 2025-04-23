// Tamedhon MUDlib
//
// std/shells/human.c -- Human Shell
//
// $Date: 2024/01/06 $
/* $Revision: 4.4 $
 * $Log: human.c,v $
 * 
 * Revision 4.4  2024/01/06 Gralkor
 * Prayroom ins neue Gidhrog gepackt
 *
 *
 * Revision 4.3  2023/07/19 Aloy
 * mehr Individualisierungsoptionen
 * av gewicht und groeße vereinheitlicht
 * av Gewicht für alle gleich! Auch Frauen können Muckis haben!
 * Frisuren und Rassemerkmale für alle gleich. Außer Bärte. Die nur für Männer.
 *
 * Revision 4.2  2009/08/01 16:00:00  Grynnis
 * Typo in _query_racestring ("Menschen" statt "Mensch")
 *
 * Revision 4.1  2008/01/14 22:00:00  TamTam
 * Vorbereitung fuer Augen-/Haar- und Hautfarbe+RassenSpecial
 *
 * Revision 1.2  2000/06/11 20:32:25  Samtpfote
 * TT_* eingebaut
 *
 * Revision 1.1  2000/06/11 20:24:58  Samtpfote
 * Initial revision
 *
 * Revision 3.5  1997/11/25 10:58:09  Angrox
 * Id "Menschenspieler" hinzugefuegt
 *
 * Revision 3.4  1997/11/05 16:25:26  Woody
 * neuer Temperaturbereich, P_MATERIAL_KNOWLEDGE setzen
 *
 * Revision 3.3  1997/10/07 09:24:14  Woody
 * neue Attribute, Food/Drink/Alc-Werte
 *
 * Revision 3.2  1997/09/17 14:42:32  Woody
 * neue Rassenbeschreibung
 *
 * Revision 3.1  1997/06/26 00:08:56  Woody
 * P_RACE_TT_MV gesetzt
 *
 * Revision 3.0  1997/03/13 02:14:14  Woody
 * prepared for revision control
 *
 * Revision 2.9  02-02-1996  00:00:08  Mupfel
 * P_TEMPERATURE_RANGE
 *
 * Revision 2.8  1995/04/24  11:58:28  Rochus
 * Groesse 170 cm
 *
 * Revision 2.7  1994/10/26  00:47:27  Rochus
 * Gilde verlegt.
 *
 * Revision 2.6  1994/09/13  11:45:56  Rochus
 * Int+1, Str+1, Con+1, Dex+3
 *
 * Revision 2.5  1994/06/24  00:11:14  Jof
 * *** empty log message ***
 *
 * Revision 2.4  1994/06/21  18:43:19  Jof
 * RACE... in _query umgewandelt (save mem)
 *
 * Revision 2.3  1994/03/21  16:48:13  Jof
 * Reindented files using emacs - once again
 *
 * Revision 2.2  1994/03/16  01:21:01  Rochus
 * Rassenbeschreibung
 *
 * Revision 2.1  1994/01/26  16:30:08  rochus
 * PrayRoom geaendert von /room/pray_room auf /d/ebene/room/PortVain/pray_room
 *
 * Revision 2.0  1994/01/13  17:00:15  mud
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/03  13:39:01  mud
 * *** empty log message ***
 *
 */

#pragma strong_types

inherit "/std/player/base";

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

void create() {
  base::create();

  AddClass(CL_HUMANOID);
  AddClass(CL_HUMAN);
  SetDefaultHome("/gilden/abenteurer");
  SetPrayRoom("/gilden/abenteurer");
  SetProp(P_AVERAGE_SIZE,170);
  SetProp(P_AVERAGE_WEIGHT,75000);
  SetProp(P_ATTRIBUTES_OFFSETS,
      ([ A_INT:1, A_STR:1, A_CON:2, A_DEX:2 ]) );
  SetProp(P_MAX_FOOD, 100);
  SetProp(P_MAX_DRINK, 100);
  SetProp(P_MAX_ALCOHOL, 100);
  SetProp(P_FOOD_DELAY, 10);
  SetProp(P_DRINK_DELAY, 10);
  SetProp(P_ALCOHOL_DELAY, 4);
  SetProp(P_GUILD, "abenteurer");
  SetProp(P_TEMPERATURE_RANGE,([T_RANGE_MIN:10,T_RANGE_MAX:30]));
  SetProp(P_LIGHT,0.0);
  SetProp(P_RACE_TT_MV,
      ([ TT_OTHER:      4,
         TT_CAVE:       11,
         TT_HOUSE:      3,
         TT_HILL:       9,
         TT_FOREST:     8,
         TT_TOWN:       3,
         TT_MOUNTAIN:   11,
         TT_DESERT:     10,
         TT_JUNGLE:     12,
         TT_SWAMP:      17,
         TT_PLAIN:      6,
         TT_ARCTIC:     13,
	 TT_TUNDRA:	10,
	 TT_WATER:	10,
         TT_UNDERWATER: 8,
         TT_ROAD:       5 ]) );
  SetProp(P_MATERIAL_KNOWLEDGE,
      ([ MATGROUP_WOOD:30,
         MATGROUP_METAL:20,
         MATGROUP_ELEMENTAL:20,
         MATGROUP_CLOTH:20 ]) );
  AddId("Menschenspieler");
}

string _query_race()
{
  return "Mensch";
}

string _query_racedescr()
{
  return break_string(
    "Ein Mensch kann nichts perfekt - dafür kann er von allem etwas. Er "
    "ist äußerst flexibel und kann sich den verschiedensten Situationen "
    "anpassen. Menschen sind etwa 1,70 m groß und fühlen sich am "
    "wohlsten in ihren Städten und unter ihresgleichen. Menschen haben "
    "einen sehr wechselhaften Charakter, sie handeln vielleicht nicht "
    "immer mit dem Kopf zuerst, beweisen in wichtigen Situationen aber "
    "durchaus einen gewissen Scharfsinn. Sie haben weder den Mut der "
    "Zwerge, noch den Feinsinn der Elfen, und auch die Gemütlichkeit "
    "eines Hobbits kann man ihnen nicht nachsagen, doch ein gewisser "
    "Instinkt leitet Menschen in ihren Entscheidungen - und diese sind "
    "zumeist richtig.", 78);
}

int QueryAllowSelect() { return 1; }

string *_query_racestring()
{
  if (QueryProp(P_GENDER) == FEMALE)
      return ({"Menschenfrau","Menschenfrau","Menschenfrau","Menschenfrau"});
        return ({"Mensch","Menschen","Menschen","Menschen"});
}

string *_query_eye_colors()
{
  return ({"blau", "grün", "braun", "schwarz", "eisblau"
           "grau", "blaugrau", "blaugrün", "graugrün", "blau-grün-heterochrom"});
}

string *_query_hair_styles()
{
  return ({"langes, gelocktes Haar",
           "mittellanges, glattes Haar",
           "mittellanges, gewelltes Haar",
           "langes, glattes Haar",
           "lange, geflochtene Zöpfe",
           "kurze Haare",
           "mittellanges, zerzaustes Haar",
           "langes gepflegtes Haar",
           "kein Haar"});
}

string *_query_hair_colors()
{
  return ({"hellblond", "blond", "dunkelblond", "schwarz",
           "rot", "hellbraun", "braun", "dunkelbraun"});
}

string *_query_skin_colors()
{
  return ({"hellbraun", "schokobraun", "blass", "kalkweiß",
           "sonnengebräunt", "dunkelbraun", "fast schwarz"});
}

string *_query_race_specials_male()
{
  return ({"trägt einen kleinen Spitzbart",
           "trägt einen struppigen Vollbart",
           "trägt einen Dreitagebart",
           "trägt keinen Bart"});
}

string *_query_race_specials()
{
  return ({"trägt silberne Ohrringe",
           "trägt eine Perle in jedem Ohr",
           "hat eine Narbe über dem Auge",
           "hat eine Narbe auf der Wange",
           "trägt ein Nasenpiercing"});
}

