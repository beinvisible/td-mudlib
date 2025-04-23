// Tamedhon MUDlib
//
// std/corpse.c -- corpse standard object
//
// $Date: 2012/06/17 14:25:00 $
/* $Revision: 1.63 $
 * $Log: corpse.c,v $
 *
 * Revision 1.63  2012/06/17 14:25:00  Serii
 * Fix falls Query(P_RACE)==0
 *
 * Revision 1.62  2012/06/17 20:50:00  Serii
 * QueryDecayState() gibt den Verwesungsgrad zurueck
 *
 * Revision 1.61  2012/06/15 09:04:00  Serii
 * P_WEIGHT wird nun auch gespeichert; P_ORIG_IDS
 * eventuelle P_NAME_ADJ die der leiche gesetzt werden, werden nun in den grossteil
 *   der verfaulenden P_SHORT mit eingefuegt (z.B. "gehaeutet")
 *
 * Revision 1.6  2008/06/30 14:12:00  TamTam
 * P_ORIG_HP - original P_MAX_HP (corpse)
 *
 * Revision 1.51  2004/01/03 19:11:00  Serii
 * id "\nleiche" eingefuegt. bei abfragen nach leichen im raum bitte diese id
 * benutzen, NICHT "leiche"
 *
 * Revision 1.5  2003/07/05 17:25:00  Querolin
 * P_RACE eingefuegt, damit der Bug vom killmsgd endlich aufhoert...
 *
 * Revision 1.4  2002/12/29 14:14:00  Serii
 * originales geschlecht nun in P_ORIG_GENDER
 *
 * Revision 1.3  2002/12/24 02:48:17  Serii
 * die komplette P_NAME des Npc wird nun in P_ORIG_FULL_NAME gespeichert, weils
 * bei manchen (z.b. bibliothekar) auch ein array sein kann
 *
 * Revision 1.2  2001/01/13 20:23:23  Samtpfote
 * die Zeile mit den ZT umgeaendert
 *
 * Revision 1.1  2001/01/13 20:21:35  Samtpfote
 * Initial revision
 *
 * Revision 3.12  1997/10/16 15:43:18  Woody
 * MayAddWeight() nimmt mixed als Parameter
 *
 * Revision 3.11  1997/10/10 14:31:46  Woody
 * Leichen saettigen (wenn sie kleiner als 10 cm sind) trotzdem um 1
 *
 * Revision 3.10  1997/09/28 14:22:02  Woody
 * Leiche besteht aus MAT_MISC_DEAD
 *
 * Revision 3.9  1997/09/17 14:10:06  Woody
 * Bei remove() inventory ausraeumen, do_damage() mit Objektuebergabe
 *
 * Revision 3.8  1997/07/11 00:09:09  Woody
 * Bugfix bei eigener MURDER_MSG (%s kam da...), weniger tolle Murdermsgs raus
 *
 * Revision 3.7  1997/06/25 11:14:40  Woody
 * Random anders, P_KILLER/P_ATTRIBUTES/P_LAST_DAMTYPES/P_SIZE setzen,
 * Leichen jetzt P_SIZE/10 (statt fix 20) Saettigung, Verspotten auf -M
 *
 * Revision 3.6  1997/06/05 13:04:44  Feyaria
 * typos in den moerdermeldungen entfernt
 *
 * Revision 3.5  1997/05/07 07:04:32  Feyaria
 * 140 neue moerdermeldungen :)))
 *
 * Revision 3.3  1997/03/19 16:24:32  Eros
 * Neue Moerdermeldung :)
 *
 * Revision 3.2  1997/03/13 02:51:47  Woody
 * prepared for revision control
 *
 * Revision 3.1  1996/11/28  Woody
 * in P_MURDER_MSG darf man jetzt auch %s verwenden (fuer den Killer)
 *
 * Revision 3.0  1996/10/06  Woody
 * Leichen werden (als 'Behaelter') nie voll
 *
 * Version AnderLand 24-06-95 by Mupfel
 * (leere) Funktion 'gegessen' eingefuehrt, die NACH der Essmeldung
 * und VOR dem removen der Leiche aufgerufen wird, um z.B. durch
 * Ueberschreiben eine Spielervergiftung herbeizufuehren.
 * Definition: void gegessen();
 *
 * Revision 2.7  1994/12/06  08:41:52  Boing
 * Ueber P_MURDER_MSG kann das gekillte Monster jetzt seinen Text auf
 * [Moerder] selber festlegen. Thanks to Silvana
 *
 * Revision 2.6  1994/10/06  16:35:45  Boing
 * Keine Moerder-Meldung wenn ein Magier ein Monster darniedermeuchelt. :)
 *
 * Revision 2.5  1994/08/19  08:25:22  Rochus
 * Logfile fuer Moerder verbessert...
 *
 * Revision 2.4  1994/08/18  18:57:51  Hate
 * bugfix (rochus changed the position of the 4)
 *
 * Revision 2.3  1994/08/18  18:56:42  Hate
 * Added Messages when a Monster dies (special formula used!)
 *
 * Revision 2.2  1994/06/13  18:46:03  Rochus
 * Property corpse_decay eingebaut, damit sich das auch von
 * aussen regeln laesst (z.b. fuer Aufgaben, bei denen man die
 * Leichen von bestimmten Monstern braucht...)
 *
 * Revision 2.1  1994/03/21  16:48:13  Jof
 * Reindented files using emacs - once again
 *
 * Revision 2.0  1994/01/13  16:59:41  mud
 * *** empty log message ***
 *
 * Revision 1.2  1993/12/30  20:42:25  mud
 * *** empty log message ***
 *
 */

// A corpse.
//
// Ekeltexte by Boing
//
// This is a decaying corpse. It is created automatically
// when a player or monster dies.

inherit "std/container";

#include <properties.h>
#include <language.h>
#include <moving.h>
#include <defines.h>
#include <wizlevels.h>

#define PROP_CORPSE_DECAY_TIME "corpse_decay"

#define DB(x)	"/room/debug"->debug_message(this_object(), x)

private static int _decay;
private static string _name;
private static int already_jeered=0;
private static string tempname=0;

ENVCHECKPROC

void create()
{
  ::create();
  if(clonep(this_object()))
  {
    AddId("leiche");
    AddId("rest");
    AddId("ueberrest");
    AddId("\nleiche");
    SetProp( P_GENDER, FEMALE );
    SetProp(P_NAME, "Leiche");
    SetProp(P_MATERIAL, MAT_MISC_DEAD);
    SetProp(PROP_CORPSE_DECAY_TIME,30);
    AddCmd(({"iss","verspeise"}),"mampf");
    AddCmd(({"spotte", "espotte", "gspotte"}),"spott");
  }
  else SetProp(P_ARTICLE, 0);

  SetProp(P_NOGET, 1);
}

varargs string name(int casus,int demon)
{
  return tempname || ::name(casus, demon);
}

void init()
{
  ::init();
}

/* Damit die Leiche nicht voll wird... */
varargs int MayAddWeight(mixed weight)
{
  return 0;
}

/* Uebernehme den Namen von ob */
void Identify(object ob )
{object race;
  _name = ob->name(WESSEN,0);
  SetProp(P_SHORT, "Die Leiche "+_name);
  SetProp(P_LONG, "Du siehst die sterblichen Überreste "+ _name + ".\n");
  _decay = 4;
  SetProp(P_LEVEL, ob->QueryProp(P_LEVEL));
  SetProp(P_ORIG_NAME, ob->name(RAW));
  SetProp(P_ORIG_FULL_NAME,ob->Query(P_NAME));
  SetProp(P_ORIG_GENDER,ob->QueryProp(P_GENDER));
  SetProp(P_ORIG_HP,ob->QueryProp(P_MAX_HP));
  SetProp(P_SIZE, ob->QueryProp(P_SIZE));
  SetProp(P_WEIGHT, ob->QueryProp(P_WEIGHT));
  SetProp(P_ORIG_IDS, ob->QueryProp(P_IDS));
  SetProp(P_KILLER, ob->QueryProp(P_KILLER));
  SetProp(P_ATTRIBUTES, ob->QueryProp(P_ATTRIBUTES));
  SetProp(P_LAST_DAMTYPES, ob->Query(P_LAST_DAMTYPES));
  race=ob->Query(P_RACE);
  if(!race)race=ob->QueryProp(P_RACE);
  SetProp(P_RACE,race);
  call_out("do_decay", QueryProp(PROP_CORPSE_DECAY_TIME));
  if(!query_once_interactive(ob)) "/std/corpse"->ChannelMessage(ob);
}

int QueryDecayState()
{
  return _decay;
}

/*
 * Prevents tampering with the master object
 */
query_prevent_shadow() { return 1; }

void do_decay()
{
  _decay -= 1;
  if (_decay > 0)
  {
    switch(_decay)
    {
      case 3: SetProp(P_SHORT,"Die bereits ziemlich stinkende"+
                      (sizeof(QueryProp(P_NAME_ADJ))?", "+QueryProp(P_NAME_ADJ)[0]+
                      "e":"")+" Leiche "+_name);
	      break;
      case 2: SetProp(P_SHORT,"Die schimmelnde und halbverweste"+
                      (sizeof(QueryProp(P_NAME_ADJ))?", "+QueryProp(P_NAME_ADJ)[0]+
                      "e":"")+" Leiche "+_name);
	      if ( QueryProp( P_HEAL ) >= -4 )
		SetProp( P_HEAL, -4 );
	      break;
      case 1: SetProp(P_SHORT,"Die verfaulten Einzelteile "+_name);
	      break;
	    }
    call_out("do_decay", QueryProp(PROP_CORPSE_DECAY_TIME));
    return;
  }
  ME->remove();
}

void gegessen() {}

varargs int remove()
{
  // units bekommen eine Chance zur Vereinigung...
  map( all_inventory(ME),
      #'call_other, "move", environment(ME), M_SILENT );
  return ::remove();
}

int mampf(string str)
{
  int heal;

  notify_fail("Was möchtest Du essen?\n");
  if (!str || !id(str)) return 0;
  if (!this_player()->eat_food((({int})QueryProp(P_SIZE)/10) || 1)) return 1;
  say(sprintf("%s wird von %s voll Hingebung verspeist.\n",
	      Name(WER), this_player()->name(WEM)));
  if ( ( heal = QueryProp(P_HEAL) ) < 0 )
  {
    this_player()->do_damage(random(-heal), this_object());
    write("Buah, diese Leiche war zweifellos nicht besonders gesund.\n");
  }
  else
  {
    this_player()->heal_self(random(heal));
    write("Hmmm, lecker!\n");
  }
  gegessen();
  ME->remove();
  return 1;
}

int spott(string str)
{
  object killobj;

  if (!str)
    return notify_fail("Syntax: [e|g]spotte <text>\n"), 0;

  if (!objectp(killobj=QueryProp("my_killer")))
    return notify_fail("Da gibt es nix zum verspotten.\n"), 0;

  if (PL!=killobj)
    return notify_fail("Du bist nicht der Mörder!\n"), 0;

  if (_decay < 4)
    return notify_fail("Dazu ist es jetzt zu spät.\n"), 0;

  notify_fail("Du kannst Dein Opfer nur einmal verspotten!\n");
  if (already_jeered)
    return 0;
  else {
    str = PL->_unparsed_args();
    switch (query_verb())
    {
      case "espotte":
        tempname=PL->Name();
        VOICEMASTER->SendToChannel("Mörder", ME, EMOTE, str);
        break;
      case "gspotte":
        tempname=PL->Name(WESSEN);
        VOICEMASTER->SendToChannel("Mörder", ME, GEMOTE, str);
        break;
      default:
        tempname=PL->Name();
        VOICEMASTER->SendToChannel("Mörder", ME, SAY, str);
        break;
    }
    tempname=0;
    already_jeered=1;
    write("Du verspottest Dein totes Opfer.\n");
  }
  return 1;
}

void ChannelMessage(object ob)
{
  int m_HP, m_WC, m_AC, s_HP, s_WC, s_AC;
  string msg;
  mixed msg_flags;
  int flag, mnr;
  string killer;

  if(explode(object_name(previous_object()), "#")[0] == "/std/corpse")
  {
    int i, x, y, z, nr;
    closure m_q, s_q;
    while(previous_object(i) && !query_once_interactive(previous_object(i)))
      i++;
    if(!previous_object(i)) return;
    if (IS_LEARNER(previous_object(i))) return;
    killer = previous_object(i)->name();
    if (lower_case(killer)!=getuid(previous_object(i)))
      killer=capitalize(getuid(previous_object(i)));
    m_q = symbol_function("QueryProp", ob);
    s_q = symbol_function("QueryProp", previous_object(i));
    if ((object_name(ob)=="obj/shut")||(nr=(random(100)<5)?1:0) ||
	(nr=((x = (m_HP = funcall(m_q, P_MAX_HP)) *
	      ((m_WC = funcall(m_q, P_TOTAL_WC)) +
	       (m_AC = funcall(m_q, P_TOTAL_AC)))) > 50000)?2:0) ||
	(nr=(((y = m_HP * (m_WC - (s_AC = funcall(s_q, P_TOTAL_AC)))) >
	      (z = 4 * (s_HP = funcall(s_q, P_MAX_HP)) *
	       ((s_WC = funcall(s_q, P_TOTAL_WC)) - m_AC))))?3:0))
    {
      if(!(msg=ob->QueryProp(P_MURDER_MSG)))
      {
        msg=({string})call_other("/p/daemon/killmsgd",
                               "generate_msg",
                               find_player(lowerstring(killer)));
      }

      flag = 0;
      msg_flags=ob->QueryProp(P_MSG_FLAGS);
      if (mappingp(msg_flags)) flag = msg_flags[P_MURDER_MSG];

      previous_object()->SetProp("my_killer", previous_object(i));
      //DB(sprintf("killer=%s", killer));
      //DB(sprintf("msg=%s", msg));
      switch (flag)
      {
        case 1:
          SetProp(P_NAME, "Der Geist "+ob->name(WESSEN, 0));
          VOICEMASTER->SendToChannel("Mörder", this_object(),
              EMOTE, msg);
          break;
        case 2:
          SetProp(P_NAME, ob->Name(WESSEN, 0)+" Geistes");
          VOICEMASTER->SendToChannel("Mörder", this_object(),
              GEMOTE, msg);
          break;
        default :
          SetProp(P_NAME, "Geist "+ob->name(WESSEN, 0));
          VOICEMASTER->SendToChannel("Mörder", this_object(),
              SAY, msg);
      }
    }
    log_file("moerder.log",
	     sprintf("MON(%O) COND(%d) NPC(%d), DIFF(%d,%d)\n",
		     ob,nr,x,y,z));
  }
}
