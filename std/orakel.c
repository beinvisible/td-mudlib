// SilberLand MUDlib
//
// std/orakel.c -- Orakeleigenschaften fuer auskunftgebende Objekte/NPCs
//
// $Date: 1997/05/25 15:08:58 $
/* $Revision: 1.1 $
 * $Log: orakel.c,v $
 * Revision 1.1  1997/05/25 15:08:58  Woody
 * Initial revision
 *
 */


//
// string all_quests();
//   Gibt einen String (ohne abschliessendes \n) mit Infos ueber Anzahl
//   und Namen aller Quests zurueck, inkl. Info, ob this_player() die
//   Quest eventuell schon geloest hat.
//
// varargs string quest_info(string qid, string dontknow, string know);
//   Gibt zu einem gegebenen String qid Questinfos aus, wenn solche
//   vorhanden sind. Mit dontknow kann man einen String angeben, wenn
//   keine entsprechende Quest gefunden wird (der Standardwert ist hier
//   "%s? Kann ich Dir leider nix zu sagen."), wobei %s durch qid
//   ersetzt wird. Mit know kann man einen String angeben, der der
//   Questinfo vorgestellt wird. %s wird hier durch den kompletten
//   Namen der Quest ersetzt (zB. 'Hilf dem weisen Schriftgelehrten').
//   Standardwert ist hier "%s? *gruebel* Also... ".
//
// string *QuestIDs();
//   Gibt ein Array von sinnvollen IDs zurueck, die eine der Quests aus
//   der Questliste 'matchen'. Dies sind i.a. alle grossgeschriebenen
//   Woerter mit Ausnahme von "Hilf", "Der", "Die" und "Das".
//
// Beispielhafte Anwendung in einem NPC:
//
//   inherit "std/npc";
//   inherit "std/orakel";
//
// und in create():
//
//   AddInfo(({"aufgabe","aufgaben","abenteuer","quests","quest","questen"}),
//             "@@all_quests@@", "sagt: ");
//   AddInfo(ME->QuestIDs(), symbol_function("quest_info",ME), "sagt: ");
//
// symbol_function() ist notwendig, da der gefragte Text als Parameter
// an quest_info() uebergeben werden soll, was bei @@...@@ (process_string)
// aber nicht der Fall ist.
//
// Anmerkung: Es kann fuer NPCs sinn machen, die letztere AddInfo()
// Zeile in reset() nochmal aufzurufen, um das Info Mapping gegebenen-
// falls zu aktualisieren, falls eine neue Quest angehaengt wurde.
//


#include <config.h>
#include <questmaster.h>

static mapping quests;

static int _qsortfunc(mixed x, mixed y)
{
  return quests[x][4] > quests[y][4];   // nach Stufe sortieren
}

string all_quests()
{
  string s, *ind;
  int    i,n;

  s = "Also hier im "+MUDNAME+" gibt es zur Zeit %d "+
      "Aufgabe%s, auch Quest%s genannt. %s:";
  quests = QM->QueryQuests(1);          // ACHTUNG: QUESTMASTER ERWEITERN !!
  ind = QM->QueryQuests()[0];
  ind = sort_array(ind, #'_qsortfunc);  // nach Stufe sortieren
  for (i=n=0; i<sizeof(ind); i++)
  {
    if (quests[ind[i]][6])   // ist die Quest 'aktiviert' ?
    {
      n++;
      s += "\n"+ind[i];
      if (this_player()->QueryQuest(ind[i]))
        s += " (hast Du schon gelöst)";
    }
  }
  if (n==1)
    return sprintf (s, n, "", "", "Das wäre");
  else
    return sprintf (s, n, "n", "en", "Da hätten wir");
}

varargs string quest_info(string qid, string dontknow, string know)
{
  string s;
  int    i;
  mixed  *quests;
  string *tmp;

  s = sprintf(dontknow || "%s? Kann ich Dir leider nix zu sagen.", qid);
  quests = QM->QueryQuests();
  for (i=0;i<sizeof(quests[0]);i++)
  {
    if (((member(explode(lower_case(quests[0][i])," "), lower_case(qid))>=0)
       || (lower_case(quests[0][i])==lower_case(qid))) && quests[1][i][6])
    {
//      s = sprintf(know || "%s? *gruebel* Also... ", quests[0][i]);
      s = sprintf(know || "%s? Mal schaun was ich Dir da sagen kann... ", 
          quests[0][i]);
      s += quests[1][i][3];
      return s;
    }
  }
  return s;
}

string *QuestIDs()
{
  mixed  *quests;
  string *ids;
  int    i;

  quests = QM->QueryQuests();
  // Die ID's umfassen sowohl den 'ganzen Satz' als auch
  // Teile davon (naemlich was grossgeschrieben wurde)
  ids = quests[0];
  for (i=0;i<sizeof(quests[0]);i++)
    if (quests[1][i][6])
      // Auftrennen an den Leerzeichen (explode) und
      // Rauswerfen der kleingeschriebenen Woerter (filter)
      ids += filter(explode(quests[0][i], " "),
                          lambda(({'x}), ({#'==,'x,({#'capitalize, 'x})})))
           - ({"Hilf","Der","Die","Das"});
  return map(ids, #'lower_case);
}

