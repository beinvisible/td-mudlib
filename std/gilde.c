// Tamedhon Mudlib
//
// std/gilde.c -- Standard Gilde
//
// $Date: 2010/08/14 00:00:00 $
/* $Revision: 4.0 $
 * $Log: gilde.c,v $
 *
 * Revision 4.0 2010/08/14 00:00:00  Grynnis
 * change() komplett ueberarbeitet.
 * v.a. veraenderte Berechnung der Min-Attr und Spellpause beim Wandeln.
 * Schummelbug "wandle int in int" abgefangen
 *
 * Revision 3.94 2008/09/06 17:00:00  TamTam
 * Formatierungs Unschaerfe bei optionalen Abenteuern in der Liste
 *
 * Revision 3.93 2008/07/05 11:00:00  TamTam
 * Attributsgrenze von 90 auf 100 angehoben (POTIONROOMS weg)
 *
 * Revision 3.92 2007/02/12 Gralkor
 * Queros Bug ausgebaut. Quero musste online sein, damit die advance-Func
 * funktioniert. Quero ist derzeit nicht da, also ist das auch ueberfluessig
 * also raus damit
 *
 * Revision 3.91 2005/07/10 15:49:00  Querolin
 * P_LAST_ADVANCE eingefuegt.
 *
 * Revision 3.9  2004/02/02 00:00:00  Serii/Gralkor
 * Testis koennen nun wieder einfacher Seher werden.
 *
 * Revision 3.8  1998/02/12 15:36:22  Woody
 * Spielerstufentitel in guild_title[P_DEFAULT_GUILD] sichern
 *
 * Revision 3.7  1998/02/05 01:10:03  Woody
 * Titel 'Druide/in' raus, stattdessen 'Haudegen/Mutige' rein
 *
 * Revision 3.6  1998/01/02 02:10:35  Feyaria
 * Zwingender GRund beim Banishen
 *
 * Revision 3.5  1997/11/05 16:00:48  Woody
 * Neue Berechnung der erhaltenen AP, Untergrenze beim Wandeln gelockert,
 * autom. save_me() nach Erhoehen der Stufe
 *
 * Revision 3.4  1997/06/25 11:17:29  Woody
 * Typofixing, fuer Levels > 20 ein AP pro 2 Level,
 * Testis duerfen gratis wandeln :)
 *
 * Revision 3.3  1997/06/12 12:15:33  Woody
 * liste und Kosten gebugfixt und an neue Version des QM angepasst
 *
 * Revision 3.2  1997/06/05 16:32:38  Feyaria
 * wandle erschwert
 *
 * Revision 3.1  1997/03/13 02:54:53  Woody
 * prepared for revision control
 *
 * Rev 3.02   96/10/09  Woody
 * "kosten" Befehl zeigt jetzt auch, wieviel Attributpunkte man erhalten wird
 * Die vielen kleinen Bugs und Feinheiten bei der AP-Berechnung raus
 *
 * Rev 3.01   96/10/06  Woody
 * Wandeln von Attributen fuer 50 MP
 *
 * Rev 3.0    96/09/30  Woody
 * neues Stufen- und Attributsystem reingetan
 *
 * Rev 2.40a  96/04/03  Woody
 * EP Stufen wieder normalisiert :)
 *
 * Revision 2.40  1996/02/19  23:15:00  Mupfel
 * keine Ausgabe ueber Info-Kanal fuer Testspieler
 *
 * Revision 2.39  1996/02/05  22:55:00  Mupfel
 * Ausgabe der neuen Spielerstufe auf den 'Info'-Kanal
 * ueber den Infomaster /obj/infomaster
 *
 * Revision 2.38b 96/01/23, Woody@SilberLand
 * command "unbanish"
 *
 * Revision 2.38a 96/01/16, Woody@SilberLand
 * EP-Stufen, Typo ("Willommen") raus
 *
 * Revision 2.38  1995/08/03  18:31:17  Rochus
 * Bugfix (hoffentlich)
 *
 * Revision 2.37  1995/08/01  12:28:43  Rochus
 * Rekursion vermieden.
 *
 * Revision 2.36  1995/08/01  12:20:17  Rochus
 * Titel-Behandlung.
 *
 * Revision 2.35  1995/07/18  12:07:08  Rumata
 * ein bisserl reformatierung der questliste
 *
 * Revision 2.34  1995/07/17  12:25:42  Wargon
 * _unparsed_args() in banish()
 * Variablen static gemacht
 *
 * Revision 2.33  1995/06/16  12:21:01  mud
 * added argument to QueryDontneed call
 *
 * Revision 2.32  1995/06/06  08:42:44  Jof
 * _notify_fail
 *
 * Revision 2.32  1995/06/05  14:56:00  Jof
 * _notify_fail used
 *
 * Revision 2.31  1995/05/22  19:33:11  Kirk
 * 1196 -> QP_MIN (defined in questmaster.h)
 *
 * Revision 2.30  1995/05/22  18:53:01  Kirk
 * QueryDontneed() called
 *
 * Revision 2.29  1995/03/30  13:48:43  Kirk
 * *** empty log message ***
 *
 * Revision 2.28  1995/02/10  16:30:14  Jof
 * *** empty log message ***
 *
 * Revision 2.27  1995/02/10  16:26:43  Jof
 * *** empty log message ***
 *
 * Revision 2.26  1995/02/07  17:02:09  Kirk
 * *** empty log message ***
 *
 * Revision 2.25  1995/02/06  19:44:14  Jof
 * RCS-Logs aufgeraeumt (leere Logs raus usw)
 *
 * Revision 2.24  1995/01/24  16:40:21  Jof
 * Revision 2.23  1994/12/01  13:29:59  Jof
 * Revision 2.22  1994/11/09  14:55:55  Kirk
 * Seerlevel now up to 35
 *
 * Revision 2.21  1994/10/07  14:58:38  Kirk
 * output of 'liste' changed
 *
 * Revision 2.20  1994/09/29  16:02:10  Rochus
 * notify_fail statt write und den nicht anwesenden Gnom entfernt.
 *
 * Revision 2.19  1994/09/26  16:03:12  Kirk
 * added playername argument to liste command
 *
 * Revision 2.18  1994/09/22  16:28:47  Kirk
 * Revision 2.17  1994/07/10  17:36:33  Jof
 * Weg mit "show"
 *
 * Revision 2.16  1994/07/05  10:41:20  Boing
 * asText() gab (sinngemaess) 'eine Abenteuer' zurueck.
 *
 * Revision 2.15  1994/06/30  11:17:54  Kirk
 * Revision 2.13  1994/06/30  10:00:40  Rumata
 * list() gruppiert nun die abenteuer nach QP-Gruppen.
 *
 * Revision 2.12  1994/06/16  21:25:53  Kirk
 * Revision 2.11  1994/06/15  14:48:17  Kirk
 * Revision 2.10  1994/06/13  23:18:16  Kirk
 * Revision 2.9  1994/05/16  09:11:43  Kirk
 * Revision 2.8  1994/03/23  17:31:55  Boing
 * Aenderung der Spielertitel
 *
 * Revision 2.8  1994/03/23  17:31:55  Boing
 * Revision 2.7  1994/03/21  16:48:13  Jof
 * Revision 2.6  1994/02/28  23:56:20  Kirk
 * Revision 2.5  1994/02/28  23:21:32  Kirk
 * Revision 2.4  1994/02/28  22:37:14  Kirk
 * new quest list and removed some superfluous trash
 *
 * Revision 2.3  1994/02/15  22:55:10  Kirk
 * made recommended player level in command 'liste' visible for all players
 *
 * Revision 2.2  1994/02/08  19:32:51  Kirk
 * Removed 'banish-message' in init()
 *
 * Revision 2.1  1994/01/18  01:01:41  mud
 * Revision 2.0  1994/01/13  16:59:38  mud
 * Revision 1.2  1993/12/30  20:42:25  mud
 */

#pragma strong_types

inherit "std/room";
//inherit "std/more";

//#define TW(x) if(find_player("samtpfote")) tell_object(find_player("samtpfote"),x)
//#define TW(x) "/room/debug"->debug_message(this_object(), x)
#define TW(x) /* x */

#include <properties.h>
#include <defines.h>
#include <rooms.h>
#include <wizlevels.h>
#include <language.h>
#include <new_skills.h>
#include <exploration.h>
#include <attributes.h>
#include <ansi.h>
#include "/secure/questmaster.h"

mixed get_next_exp(int lev);
varargs string get_new_title(int lev, object pl);

void create()
{
  room::create();
  SetProp(P_INDOORS,1);
  SetProp(P_INT_LONG,
      "Du befindest Dich in der berühmten Abenteurer-Gilde.\n"
      +"Hier kannst Du Deine Erfahrungsstufe erhöhen.\n"
      +"An der Wand siehst Du eine große Uhr.\n"
      +"Mögliche Befehle sind:\n"
      +"                   liste\n"
      +"                   erhöhe (stufe)\n"
      +"                   kosten\n");
  SetProp(P_INT_SHORT,"Abenteuer-Gilde");
  AddCmd("kosten","kosten");
  AddCmd("liste","liste");
  AddSpecialDetail("uhr","zeige_reboot");
  AddCmd(({"erhoehe", "erhöhe"}),"advance");
  AddCmd(({"wandle","wandele","wandel"}),"change");
  AddItem("/obj/zeitungsautomat",REFRESH_REMOVE);
  SetProp( P_LIGHT, 1 );
}

void init()
{
  room::init();
  if (IS_LORD(PL)) {
    add_action("banish","banish");
    add_action("unbanish","unbanish");
  }
  if (IS_ARCH(PL))
    add_action("mbanish","mbanish");
}

string GuildName()
{
    string      *path;

    path=explode(object_name(this_object()), "/");
    if((strstr(path[1], "files.") == 0))
    { path[1]=path[1][6..]; }    

    return lowerstring(path[1]);
}

string zeige_reboot()
{
  string str;
  int t,t2;

  t=time()-last_reboot_time();
  t2=t;
  str="Seit dem letzten Shutdown sind "+t+" Sekunden vergangen.\n";
  if (t<60) return str;
  str+="Das sind ";
  if (t>=86400)
  {
    str+=t/86400+" Tag";
    if (t>=86400*2)
      str+="e";
    str+=", ";
    t2=t2%86400;
  }
  if (t>=3600)
  {
    str+=t2/3600+" Stunde";
    if (t2/3600!=1)
      str+="n";
    str+=", ";
    t2=t2%3600;
  }
  if (t>=60)
  {
    str+=t2/60+" Minute";
    if (t2/60!=1)
      str+="n";
    str+=" und ";
    t2=t2%60;
  }
  str+=t2+" Sekunde";
  if (t2!=1)
    str+="n";
  str+=".\n";
  return str;
}

int banish(string str)
{
  string grund, name;

  if (!IS_LORD(this_interactive())) return 0;

  if (!str) {
    write("Syntax: banish <name> <grund>\n");
    return 1;
  }
  str = this_interactive()->_unparsed_args();

  if (sscanf(str,"%s %s",name,grund)!=2) name=str;
  if (!name || name =="")
  {
    write("Syntax: banish <name> <grund>\n");
    return 1;
  }
  if( !grund )
  {
    write("Du musst einen Grund angeben! Syntax: banish <name> <grund>\n");
    return 1;
  }
  name=lower_case(name);
  "/secure/master"->BanishName(name,capitalize(grund));
  return 1;
}

int unbanish(string str)
{
  string name;

  if (!IS_LORD(this_interactive())) return 0;

  if (!str) {
    write("Syntax: unbanish <name>\n");
    return 1;
  }
  str = this_interactive()->_unparsed_args();

  if (sscanf(str,"%s",name) != 1) name=str;
  if (!name || name =="") {
    write("Syntax: unbanish <name>\n");
    return 1;
  }
  name=lower_case(name);
  "/secure/master"->UnbanishName(name);
  return 1;
}

int mbanish(string str)
{
  string grund, name;

  if (!IS_ARCH(this_interactive())) return 0;

  _notify_fail("Syntax: mbanish <name> [<grund>]\n");

  if (!str || str=="")
    return 0;
  if (sscanf(str,"%s %s",name,grund)!=2) name=str;
  if (!name || name =="")
    return 0;
  "/secure/merlin"->MBanishInsert(lower_case(name), capitalize(grund),
      this_interactive());
  return 1;
}

#define male_title_str ({ \
      "der hoffnungsvolle Anfänger", \
      "der Landstreicher", \
      "der Streuner", \
      "der Pfadfinder", \
      "der unerfahrene Kämpfer", \
      "der Kämpfer", \
      "der erfahrene Kämpfer", \
      "der unerfahrene Krieger", \
      "der Krieger", \
      "der erfahrene Krieger", \
      "der heldenhafte Krieger", \
      "der unerfahrene Held", \
      "der Held", \
      "der erfahrene Held", \
      "der heldenhafte Abenteurer", \
      "der vorbildliche Abenteurer", \
      "der meisterhafte Abenteurer", \
      "der weithin bekannte Abenteurer", \
      "der angehende Seher", \
      "der Seher" })

#define fem_title_str ({ \
      "die hoffnungsvolle Anfängerin", \
      "die Landstreicherin", \
      "die Streunerin", \
      "die Pfadfinderin", \
      "die unerfahrene Kämpferin", \
      "die Kämpferin", \
      "die erfahrene Kämpferin", \
      "die unerfahrene Kriegerin", \
      "die Kriegerin", \
      "die erfahrene Kriegerin", \
      "die heldenhafte Kriegerin", \
      "die unerfahrene Heldin", \
      "die Heldinin", \
      "die erfahrene Heldin", \
      "die heldenhafte Abenteurerin",\
      "die vorbildliche Abenteurerin", \
      "die meisterhafte Abenteurerin", \
      "die weithin bekannte Abenteurerin", \
      "die angehende Seherin", \
      "die Seherin" })

#define elf_m_titles ({ \
        "der hoffnungsvolle Jungelf", \
        "der Elfenerkunder", \
        "der Elfenspäher", \
        "der Freund des Waldes", \
        "der Hüter des Waldes", \
        "der Protektor der Elfen", \
        "der Pfleger der Traditionen", \
        "der Weltenwanderer", \
        "der Erkenntnissucher", \
        "der Formsänger", \
        "der Wildnisentdecker", \
        "der Hüter der Traditionen", \
        "der Helfer in der Not", \
        "der Beschützer des Volkes", \
        "der heldenhafte Elf", \
        "der Elfenheld", \
        "der berühmte Elfenheld", \
        "der weit bekannte Elfenheld", \
        "der angehende Elfenseher", \
        "der Elfenseher" })

#define elf_f_titles ({ \
        "die hoffnungsvolle Jungelfe", \
        "die Elfenerkunderin", \
        "die Elfenspäherin", \
        "die Freundin des Waldes", \
        "die Hüterin des Waldes", \
        "die Protektorin der Elfen", \
        "die Pflegerin der Taditionen", \
        "die Weltenwanderin", \
        "die Erkenntnissucherin", \
        "die Formsängerin", \
        "die Wildnisentdeckerin", \
        "die Hüterin der Traditionen", \
        "die Helferin in der Not", \
        "die Beschützerin des Volkes", \
        "die heldenhafte Elfe", \
        "die Elfenheldin", \
        "die berühmte Elfenheldin", \
        "die weit bekannte Elfenheldin", \
        "die angehende Elfenseherin", \
        "die Elfenseherin" })

#define zw_m_titles ({ \
        "der hoffnungsvolle Jungzwerg", \
        "der Minenarbeiter", \
        "der Erzschürfer", \
        "der Tunnelgräber", \
        "der Stollenbauer", \
        "der Goldsucher", \
        "der Wandergeselle", \
        "der Schatzsucher", \
        "der Gesteinskundige", \
        "der Schmiedegeselle", \
        "der Hüttenkundige", \
        "der Grobschmied", \
        "der Waffenschmied", \
        "der Goldschmied", \
        "der Steinmetz", \
        "der Diamantschleifer", \
        "der Juwelier", \
        "der Meisterschmied", \
        "der angehende Zwergenseher", \
        "der Zwergenseher" })

#define zw_f_titles ({ \
        "die hoffnungsvolle Jungzwergin", \
        "die Minenarbeiterin", \
        "die Erzschürferin", \
        "die Tunnelgräberin", \
        "die Stollenbauerin", \
        "die Goldsucherin", \
        "die Wandergesellin", \
        "die Schatzsucherin", \
        "die Gesteinskundige", \
        "die Schmiedegesellin", \
        "die Hüttenkundige", \
        "die Grobschmiedin", \
        "die Waffenschmiedin", \
        "die Goldschmiedin", \
        "die Steinmetzin", \
        "die Diamantschleiferin", \
        "die Juwelierin", \
        "die Meisterschmiedin", \
        "die angehende Zwergenseherin", \
        "die Zwergenseherin" })

#define hob_m_titles ({ \
        "der hoffnungsvolle Junghobbit", \
        "der Herumtreiber", \
        "der kleine Hobbit", \
        "der pelzfüßige Hobbit", \
        "der Spaziergänger", \
        "der Wanderer", \
        "der Tourist", \
        "der Reisende", \
        "der Weltreisende", \
        "der Erforscher", \
        "der Weltenbummler", \
        "der Entdecker", \
        "der Reiseleiter", \
        "der Nachtschwärmer", \
        "der Draufgänger", \
        "der Hobbithexer", \
        "der Hobbitprophet", \
        "der Hobbitheld", \
        "der angehende Hobbitseher", \
        "der Hobbitseher" })

#define hob_f_titles ({ \
        "das hoffungsvolle Hobbitmädchen", \
        "die Herumtreiberin", \
        "die kleine Hobbitfrau", \
        "die pelzfüßige Hobbitfrau", \
        "die Spaziergängerin", \
        "die Wandersfrau", \
        "die Touristin", \
        "die Reisende", \
        "die Weltreisende", \
        "die Erforscherin", \
        "die Weltenbummlerin", \
        "die Entdeckerin", \
        "die Reiseleiterin", \
        "die Nachtschwärmerin", \
        "die Draufgängerin", \
        "die Hobbithexe", \
        "die Hobbitprophetin", \
        "die Hobbitheldin", \
        "die angehende Hobbitseherin", \
        "die Hobbitseherin" })

#define brownie_m_titles        ({ \
        "das kleine Krümelchen", \
        "der kleine Brownie", \
        "der Brownie", \
        "der Beerensammler", \
        "der Pilzsammler", \
        "der Bäckergeselle", \
        "der Wandervogel", \
        "der Biertrinker", \
        "der Schmetterlingsfänger", \
        "der Ameisenbändiger", \
        "der Wurmbezwinger", \
        "der Maulwurfreiter", \
        "der Froschwürger", \
        "der Brownieabenteurer", \
        "der Flammenbeschwörer", \
        "der Browniekämpfer", \
        "der Brownieheld", \
        "der Kristalldrechsler", \
        "der baldige Brownieseher", \
        "der Brownieseher" })

#define brownie_f_titles        ({ \
        "das kleine Krümelchen", \
        "die kleine Browniefrau", \
        "die Browniefrau", \
        "die Beerensammlerin", \
        "die Pilzsammlerin", \
        "die Bäckergesellin", \
        "die Wandersfrau", \
        "die Biertrinkerin", \
        "die Schmetterlingsfängerin", \
        "die Ameisenbändigerin", \
        "die Wurmbezwingerin", \
        "die Maulwurfreiterin", \
        "die Froschwürgerin", \
        "die Brownieabenteurerin", \
        "die Flammenhexe", \
        "die Browniekämpferin", \
        "die Brownieheldin", \
        "die Kristalldrechslerin", \
        "die baldige Brownieseherin", \
        "die Brownieseherin" })

varargs string get_new_title(int lev, object pl)
{
  string s;
  if (!pl) pl=PL;
  if (IS_SEER(pl) && (s=pl->Query(P_TITLE))) return s;
  if (lev<1) lev=1;
  else if (IS_SEER(pl))
    lev = 20;
  else if (lev >= 20)
    lev = 19;

  s=lower_case(pl->QueryProp(P_RACE));
  switch (s)
  {
    case "elf":
      if (pl->QueryProp(P_GENDER) == FEMALE) return elf_f_titles[lev-1];
      else return elf_m_titles[lev-1];
      break;
    case "zwerg":
      if (pl->QueryProp(P_GENDER) == FEMALE) return zw_f_titles[lev-1];
      else return zw_m_titles[lev-1];
      break;
    case "hobbit":
      if (pl->QueryProp(P_GENDER) == FEMALE) return hob_f_titles[lev-1];
      else return hob_m_titles[lev-1];
      break;
    case "brownie":
      if (pl->QueryProp(P_GENDER) == FEMALE) return brownie_f_titles[lev-1];
      else return brownie_m_titles[lev-1];
      break;
    default:
      if (pl->QueryProp(P_GENDER) == FEMALE) return fem_title_str[lev-1];
      else return male_title_str[lev-1];
  }
return "";
}

string UpdateTitle(int level, object pl)
{
  mapping ts;
  string title;

  title = get_new_title (level, pl);
  if (!mappingp(ts=pl->Query(P_GUILD_TITLE)))
      ts=([]);
  ts[pl->QueryProp(P_DEFAULT_GUILD) || DEFAULT_GUILD]=title;
  pl->Set(P_GUILD_TITLE,ts);
  if (!IS_SEER(pl))
    pl->SetProp(P_TITLE,0);
  return title;
}

mixed get_next_exp(int lev)
{
  if (lev < 1) return -1;
  if (lev >= 32) return 100000000+(lev-31)*25000000;
  return
    ({
      676, 1014, 1522, 2283, 3425,                              //  1-5
      5138, 7707, 11561, 17341, 26012,                          //  6-10
      39018, 58527, 77791, 97791, 131687,                       // 11-15
      197530, 296296, 444444, 666666, 1000000,                  // 16-20
      1500000, 2250000, 3375000, 5062500, 7600000,              // 21-25
      11000000, 17000000, 25000000, 40000000, 60000000,         // 26-30
      80000000})[lev];                                          // 31
}

int next_ap(int lev, object pl)
{
  int need;

  need=pl->QueryProp(P_NEEDED_QP);
  if (lev>=20) return need;     // zur Level-20-Werdung alle needed QP
  if (lev<4) return 0;          // spaeter mal, wenns mehr Quests gibt...
  if (lev<7) return need/20;
  if (lev<10) return need/10;
  if (lev<13) return need/5;
  if (lev<15) return need*7/20;
  if (lev<19) return need/2;
  if (lev<20) return need*13/20;
 
 return need;
}

int seer_cond(int silent)
{
  int unable, ep, ap;
  string sminfo;

  /* KEINE Ausnahmen fuer Testies. Samtpfote. */

  sminfo = "Um Seher"+(PL->QueryProp(P_GENDER) == FEMALE ? "in" : "")+
      " werden zu können, ";

  if (unable = (PL->QueryProp(P_LEVEL) < 20))
    sminfo += "reicht Deine Stufe noch nicht aus. ";

  ep = XP_NEEDED_FOR_WIZ - PL->QueryProp(P_XP);
  ap = PL->QueryProp(P_NEEDED_QP);
  if (ap < QM->QuerySeerQP())
    ap = QM->QuerySeerQP();
  ap = ap - PL->QueryProp(P_QP);

  if (unable && ((ep > 0) || (ap > 0))) sminfo += "Zudem ";
  if ((ep > 0) && (ap > 0))
    sminfo +=
      sprintf("fehlen Dir noch %d Erfahrungspunkt%s und %d Abenteuerpunkt%s.\n",
        ep, (ep>1 ? "e" : ""), ap, (ap>1 ? "e" : ""));
  else if (ep > 0)
    sminfo += ((ep == 1) ? "fehlt Dir noch 1 Erfahrungspunkt.\n" :
               "fehlen Dir noch "+ep+" Erfahrungspunkte.\n");
  else if (ap > 0)
    sminfo += ((ap == 1) ? "fehlt Dir noch 1 Abenteuerpunkt.\n" :
               "fehlen Dir noch "+ap+" Abenteuerpunkte.\n");

  unable = unable || (ep > 0) || (ap > 0);

  if (QM->QueryReadyForSeer(PL) == 0)
  {
    sminfo +=
      (unable ? "Du MUSST überdies noch einige Abenteuer lösen!" :
                "MUSST Du noch einige Abenteuer lösen!");
    sminfo += " Mit 'liste' erfährst Du, wieviele Abenteuer pro Gruppe Du "
      "auslassen kannst.\n";
    unable = 1;
  }
  ep = 10*EPMASTER->QueryAverage();
  if (11*EPMASTER->QueryExplorationPoints(this_player()) <= ep)
  {
    sminfo +=
      (unable ?
       "Außerdem solltest Du Dich besser auskennen als der Durchschnitt.\n" :
       "solltest Du Dich besser auskennen als der Durchschnitt.\n");
    unable = 1;
  }
   if((PL->QueryProp(P_TESTPLAYER))) unable=0;
  if (!unable)
    sminfo = "Du erfüllst alle Bedingungen, um Seher"+
      (PL->QueryProp(P_GENDER) == FEMALE ? "in" : "")+" zu werden.\n";

  if (!silent)
    write(break_string(sminfo, 78, 0, 1));

  return !unable;
}

// ApCost()
//  - ermittelt, wieviel EP fuer einen AP benoetigt werden
// nr: Die Nummer des APs (der wievielte durch EP bekommene AP?)
//
static int ApCost(int nr)
{
  int n;
  if (nr <= 10) return 10;      // konstante Kosten fuer die ersten 10 AP
  n = 10 + (nr-10) * (EPMASTER->QueryMaxEP() - 600) / 900;
  if (n<0) return 0; else return n;
}

// ApCosts()
//  - ermittelt die Gesamtkosten fuer (n) zu bekommende APs
// nr: Die Nummer des APs (der wievielte durch EP bekommene AP?)
// n: Die Anzahl an AP, die der Spieler bekommen soll
//
static int ApCosts(int nr, int n)
{
  int i,c;
  for (i=c=0; i<n; i++) c+=ApCost(nr+i);
  return c;
}

// ExploredAP()
//  - ermittelt die Anzahl der durch Forschen erlangten AP eines Spielers
// pl: Das Spielerobjekt
// level: Der Level des Spielers
//
static int ExploredAP(object pl, int level)
{
  int attrsum,ls;

  if (!level) level=pl->QueryProp(P_LEVEL);
  // Summe der jetzigen AP - 4 (man startet mit 1,1,1,1)
  attrsum=pl->QueryRealAttribute(A_STR)+pl->QueryRealAttribute(A_INT)+
          pl->QueryRealAttribute(A_DEX)+pl->QueryRealAttribute(A_CON)+
          pl->QueryProp(P_AP)-4;

  // gefundene ZTs abziehen
  attrsum -= 10 - sizeof(pl->QueryProp(P_POTIONROOMS));

  // Level beruecksichtigen
  if (level <= 10)
    attrsum -= (level-1)*2;
  else
    if (level <= 20)
      attrsum -= 18+(level-10);
    else
      attrsum -= 28+(level-20)/2;

  return attrsum;
}

// GivenAP()
//  - ermittelt, wieviele AP der Spieler beim Erhoehen auf Stufe 'level'
//    maximal erhaelt (also AP fuer Stufe und FP abzgl. Stufen- und ZT-AP)
//
static int GivenAP(object pl, int level)
{
  int ap, attrsum, i, nr, rest, maxsum;

  if (stringp(pl)) pl=find_player(pl);
  attrsum=pl->QueryRealAttribute(A_STR)+pl->QueryRealAttribute(A_INT)+
          pl->QueryRealAttribute(A_DEX)+pl->QueryRealAttribute(A_CON)+
          pl->QueryProp(P_AP);
  TW("attrsum: "+attrsum+".\n");
//  maxsum = A_SUM - sizeof(pl->QueryProp(P_POTIONROOMS)) - attrsum;
  maxsum = A_SUM - attrsum;
  if (maxsum <= 0) return 0;
  TW("maxsum: "+maxsum+".\n");
  if (level==1) ap=0;
  else if (level <= 10) ap=2;
  else if (level <= 20) ap=1;
  else ap=(level % 2) ? 0 : 1;  // jeden 2. Level einen weiteren AP
  TW("Für den Level: "+ap+" AP.\n");
  nr=ExploredAP(pl, level-1);
  TW("Bisher explored: "+nr+" AP.");
  rest=pl->QueryProp(P_EP_NUM)-pl->QueryProp(P_EP_NUM_FOR_AP);
  i=0;
  while ((rest > ApCost(nr+i+1)) && (ap+i+1 <= maxsum)) {
    if (!i) TW(sprintf("\nrest: %d (%d)", rest, ApCost(nr+i+1)));
    i++;
    rest-=ApCost(nr+i);
    TW(sprintf(" - %d (%d)",rest, ApCost(nr+i+1)));
  }
  ap+=i;
  TW("\nFürs Forschen: "+i+".\n");
  TW("Also insgesamt: "+ap+".\n");
  if ((20 <= level) && (level < 50)) maxsum -= (51 - level) / 2;
  if (level < 20) maxsum -= (20 - level);
  if (level < 10) maxsum -= (10 - level);
  if (ap > maxsum) {
    ap = (maxsum > 0 ? maxsum : 0);
    TW("Wurde begrenzt auf: "+ap+".\n");
  }
  return ap;
}

int kosten()
{
  int diff, level, ap;
  object pl;

  pl=PL;

  diff = 100 * (level=pl->QueryProp(P_LEVEL)+1) - pl->QueryProp(P_LP);
  if (diff <= 0)
    write("Du könntest Deine Stufe erhöhen!\n");
  else {
    write("Dir fehlen bis zur nächsten Stufe noch "+diff+" Stufenpunkt"+
        (diff == 1 ? "" : "e")+".\n");
    return 1;
  }
  if (ap = GivenAP (pl, level))
    write("Dabei wirst Du "+ap+" Attributpunkt"+
        (ap==1 ? "" : "e")+" erhalten.\n");

  if (IS_SEER(this_player()) || level < 20)
    return 1;

  seer_cond(0);
  return 1;
}

static string str2attr(string s)
{
  int i;
  if ((i=sizeof(s=lower_case(s))) < 3) return 0;
  if (s=="kraft"[0..i-1]) return A_STR;
  if (s=="intelligenz"[0..i-1]) return A_INT;
  if (s=="ausdauer"[0..i-1]) return A_CON;
  if (s=="geschicklichkeit"[0..i-1]) return A_DEX;
  return 0;
}

int change(string arg)
{
  string von, nach;
  int v, n, spellpause, sum, min;
  mapping attr;
  object pl;

  notify_fail(
    "Welches Attribut willst Du in welches wandeln?\n"+
    "Attribute: Kraft, Gesch(icklichkeit), Aus(dauer), Int(elligenz)\n");
  if (!arg) return 0;
  if (sscanf(arg, "%s in %s", von, nach) != 2) return 0;
  if (!(von=str2attr(von)) || !(nach=str2attr(nach))) return 0;

  if (von==nach)
    return notify_fail("Du kannst nur ein Attribut in ein anderes "
        "wandeln!\n"), 0;

  pl = this_player();
  if (pl->QueryProp(P_GHOST))
    return notify_fail("Als Geist kannst Du das nicht!\n"), 0;

  if (pl->QueryProp(P_FROG))
    return notify_fail("Als Frosch kannst Du das nicht!\n"), 0;

  if (pl->QueryProp(P_SP) < pl->QueryProp(P_MAX_SP) ||
      pl->QueryProp(P_HP) < pl->QueryProp(P_MAX_HP))
    return notify_fail("Dazu musst Du vollständig regeneriert sein!\n"),0;

  v = pl->QueryRealAttribute(von);
  n = pl->QueryRealAttribute(nach);

  spellpause = pl->QueryProp(P_NEXT_SPELL_TIME) - time();
  if (spellpause<0)
    spellpause=0;

  if (time() < pl->QueryProp(P_NEXT_SPELL_TIME))
    return notify_fail(break_string("Du bist noch zu erschöpft von Deiner "
        "letzten magischen Aktion. Warte noch "+
        (spellpause>=600?"mindestens "+spellpause/600+"0 Minuten!":
                         "ein paar Minuten!"))), 0;

  attr = pl->QueryProp(P_ATTRIBUTES);
  sum = attr[A_STR] + attr[A_INT] + attr[A_CON] + attr[A_DEX];
  min = sum/6;
  // Minimum ist: zwei Drittel vom Durchschnitt der Attribute (sum/4 * 2/3)
  if (min < 1)
    min = 1;

  if (v <= min)
    return notify_fail("Du kannst Deine "+ATTRIBUTE_STRING[von]+" nicht "+
                       "weiter verringern!\n"), 0;

  if (n >= A_MAX)
    return notify_fail("Du kannst Deine "+ATTRIBUTE_STRING[nach]+" nicht "+
                       "weiter erhöhen!\n"), 0;

  pl->SetProp(P_SP, 1+random(10));
  pl->SetProp(P_HP, pl->QueryProp(P_HP)/2);
  pl->SetProp(P_NEXT_SPELL_TIME, time() + n*10);
  // Spellpause abhaengig von der Hoehe des Atttributs in das gewandelt wird

  pl->SetRealAttribute(von, --v);
  pl->SetRealAttribute(nach, ++n);

  write (
    "Du konzentrierst Dich und murmelst ein paar magische Worte...\n"+
    "Deine "+ATTRIBUTE_STRING[von]+" hat sich auf "+v+" verringert.\n"+
    "Deine "+ATTRIBUTE_STRING[nach]+" hat sich auf "+n+" erhöht.\n"+
    "Diese Prozedur zehrt an Dir und Du bist danach sehr erschöpft.\n");
  say (pl->Name()+" murmelt einige magische Worte.\n");

  if (!pl->QueryProp(P_TESTPLAYER) &&
      !pl->QueryProp(P_WANTS_TO_LEARN) )
    log_file("WANDELN", dtime(time())+", "+getuid(pl)+", "+
                ATTRIBUTE_STRING[von]+": "+(v+1)+" --> "+v+", "+
                ATTRIBUTE_STRING[nach]+": "+(n-1)+" --> "+n+"\n");
  return 1;
}

int advance(string arg)
{
  string s, title, attr;
  int level, gender, ap, yet, attrsum;

  TW(sprintf("gilde::advance (%O) start\n", this_object()));

  notify_fail ("Was willst Du erhöhen?\n"+
    "Stufe, Gildenstufe, Kraft, Int(elligenz), "
    "Aus(dauer), Gesch(icklichkeit)?\n");
  if (!arg) return 0;
  attr=str2attr(arg);
  if (!attr && arg!="stufe" && arg!="gildenstufe") return 0;

  // Attribut-Handling
  attrsum=PL->QueryRealAttribute(A_STR)+PL->QueryRealAttribute(A_INT)+
          PL->QueryRealAttribute(A_DEX)+PL->QueryRealAttribute(A_CON);
  if (attr)
  {
    if (!(ap=PL->QueryProp(P_AP))) {
      write ("Du kannst Deine "+ATTRIBUTE_STRING[attr]+
             " im Moment leider nicht erhöhen.\n");
      return 1;
    }
    if (PL->QueryRealAttribute(attr) >= A_MAX) {
      write ("Du hast bereits die maximale "+ATTRIBUTE_STRING[attr]+
             "erreicht.\nVersuch doch, ein anderes Attribut zu erhöhen!\n"+
             "(Siehe dazu auch 'hilfe attribute').\n");
      return 1;
    }
    if (attrsum >= A_SUM) {
      write ("Du hast bereits die maximale Anzahl an Attributen erreicht.\n"+
             "Für Dich können wir leider nix mehr tun!\n");
      return 1;
    }
    yet=PL->QueryRealAttribute(attr);
    PL->SetRealAttribute(attr,yet+1);
    if (PL->QueryRealAttribute(attr) != (yet+1))
    {
      PL->SetRealAttribute(attr,yet);
      write ("Hoppla, irgendwas ist da schiefgelaufen. "+
             "Bitte einen Erzmagier verständigen!\n");
      return 1;
    }
    PL->SetProp(P_AP, --ap);
    write(sprintf("Deine %s hat sich von %d auf %d erhöht.\n",
      ATTRIBUTE_STRING[attr], yet, yet+1));
    PL->save_me(1);
    return 1;
  }

  if (PL->QueryProp(P_KILLS)) {
    notify_fail("Du hast einen Mitspieler umgebracht!\n"+
                "In diesem Fall kannst Du Deine Stufe nicht erhöhen.\n"+
                "Bitte geh zur Polizei und bring das in Ordnung.\n");
    say(PL->Name(WER)+" hat soeben auf schmerzliche Weise erfahren "+
        "müssen,\ndass es wirklich nicht förderlich ist, Mitspieler "+
        "umzubringen.\n", PL);
    return 0;
  }

  title = PL->QueryProp( P_TITLE );
  level = PL->QueryProp( P_LEVEL );
  gender = PL->QueryProp( P_GENDER );

  TW("PL->GUILD: "+PL->QueryProp(P_GUILD)+"\n");
  TW("GuildName(): "+GuildName()+"\n");
  if(lowerstring(PL->QueryProp(P_GUILD)) != lowerstring(GuildName()))
  {
/*
    tell_object(find_player("querolin"),break_string(
      (lowerstring(PL->QueryProp(P_GUILD)))+"!="+(lowerstring(GuildName()))+
      " in /std/gilde.c", 78) );

  if (PL->QueryProp(P_LEVEL)==-1)
  {
    tell_object(find_player("querolin"),break_string(
      (lowerstring(PL->QueryProp(P_GUILD)))+"!="+(lowerstring(GuildName()))+
      " level== -1 in /std/gilde.c", 78) );
  }

    TW("Gilde differieren!\n");
*/
    tell_object(PL, break_string(
        "Sorry, aber als Mitglied der Gilde der "+
        capitalize(PL->QueryProp(P_GUILD))+
        " kannst Du hier nicht Deine Stufe erhöhen."));
    return 0;
  }

  TW ("advance() 2 continued...\n");

  if (level == -1) level = 1;
  else if (100*(++level) > PL->QueryProp(P_LP))
    return kosten();

  TW ("advance() 3 continued...\n");

  if (level>=20 && !IS_SEER(PL) && seer_cond(1))
  {
    s=(gender==FEMALE ? "in" : "");
    write ("Du erfüllst die Bedingungen, um Seher"+s+" zu werden!\n"+
      "Suche dazu Merlin auf und sag 'merlin mach mich zu"+
      (gender==FEMALE ? "r" : "m")+" seher"+s+"'.\n");
  }

  PL->SetProp(P_LEVEL, level);
  title=UpdateTitle (level, PL);
  title=PL->QueryProp(P_TITLE) || "";
  if (sizeof(title)) title=" "+title;

  say (s=sprintf("%s%s hat jetzt Stufe %d erreicht.\n",
                 PL->Name(WER), title, level));
  if (!PL->QueryProp(P_TESTPLAYER))
    call_other("/obj/infomaster", "InfoMsg", s);

  TW ("advance() 4 continued...\n");

  if (level < 7)
    write(sprintf("Du bist jetzt %s%s (Stufe %d).\n",
        PL->name(WER), title, level));
  else if (level < 14)
    write(sprintf("Gut gemacht, %s%s (Stufe %d).\n",
        PL->name(WER), title, level));
  else
    write(sprintf("Willkommen auf Deiner neuen Stufe.\n"
        "Du bist jetzt %s%s (Stufe %d).\n", PL->name(WER), title, level));

  PL->Set(P_LAST_ADVANCE, SAVE, F_MODE_AS);
  PL->SetProp(P_LAST_ADVANCE, time() );
  if (lower_case(PL->QueryProp(P_GUILD))=="abenteurer")
  {
    PL->Set(P_LAST_ADVANCE_GUILD, SAVE, F_MODE_AS);
    PL->SetProp(P_LAST_ADVANCE_GUILD, time() );
  }

  // vergebene Attributpunkte
  // ---
  if (ap=GivenAP(PL,level))
  {
    int apnow, apforlev, epforap;

    if (level<=1) apforlev=0;
    else if (level <= 10) apforlev=2;
    else if (level <= 20) apforlev=1;
    else apforlev=(level % 2) ? 0 : 1;  // jeden 2. Level einen weiteren AP

    if (apforlev >= ap)
      apforlev=ap;
    else
    {
      epforap=ApCosts(ExploredAP(PL,level-1)+1, ap-apforlev);
      TW ("Ziehe "+epforap+" EP fuer AP ab.\n");
      PL->SetProp(P_EP_NUM_FOR_AP, PL->QueryProp(P_EP_NUM_FOR_AP)+epforap);
    }

    apnow = PL->SetProp(P_AP, ap+PL->QueryProp(P_AP));
    if ((level<=20) || (level % 2 == 0))
      s="des Erreichens Deiner Spielerstufe ";
    else
      s="";
    if (ap==apforlev)
      s="Aufgrund "+s;
    else
      s = "Aufgrund "+(sizeof(s) ? s+"und " : "")+
        "Deiner Erkundungen auf dieser Welt ";
    s+="erhältst Du "+ap+" Attributpunkt"+(ap==1 ? ", mit dem" :
        "e, mit denen")+" Du deine Attribute erhöhen kannst.";
    if (ap!=apnow) s+= " Insgesamt hast Du jetzt "+apnow+
        " Attributpunkt"+(apnow==1 ? "" : "e")+".";
    write("\n"+break_string(s, 78)+"\n");
  }
  TW ("advance() done.\n");
  PL->save_me(1);
  return 1;
}

static int compare( mixed *i, mixed *j )
{
  if( i[2] == j[2] )
    return i[1] > j[1];
  else
    return i[2] > j[2];
}

static string asText( int i )
{
  switch( i )
  {
    case 0: return "kann keines";
    case 1: return "kann eines";
    default:
      return sprintf( "können %d", i );
  }
}

varargs int liste(mixed pl)
{
  int qgroups, i, j, n, *dontneed;
  mixed *qlists, *quests;
  string str, bold, normal;

  if (!this_player()) return 0;

  if(!objectp(pl))
    if(stringp(pl))
      pl=find_player(pl)||find_netdead(pl);
  if(!objectp(pl))
    pl=PL;

  if ((this_player()->QueryProp(P_TTY) == "ansi") ||
      (this_player()->QueryProp(P_TTY) == "vt100"))
    bold=ANSI_BOLD, normal=ANSI_NORMAL;
  else
    bold=normal="";

  /*
  str = sprintf("                        Liste der Abenteuer: %s\n\n",
      (pl != PL ? "("+capitalize(getuid(pl))+")" : ""));
  str += sprintf("  %38s  AP  Stufe  geloest  verantw. Magier\n","");
  */
  str = sprintf("%s%-38s%s    AP  Stufe  gelöst  verantw. Magier\n",
      bold,"Liste der Abenteuer"+
      (pl != PL ? "("+capitalize(getuid(pl))+")" : "")+":", normal);
  for( i=0; i<39; i++, str+="--" );
  str += "\n";
  qgroups = sizeof(dontneed=QM->QueryDontneed(pl));
  qlists = allocate( qgroups );
  for( i=qgroups-1; i>=0; i-- )
    qlists[i] = ({});

  quests = QM->QueryQuests();
  for( i=sizeof(quests[0])-1; i>=0; i-- )
    if( quests[1][i][6] )
    {
      for( j=qgroups-1; QGROUPS[j]>quests[1][i][0]; j-- )
        ;
      qlists[j] += ({ ({
        quests[0][i], quests[1][i][0], quests[1][i][4], quests[1][i][7],
                quests[1][i][5]
      }) });
    }

  for( i=0; i<qgroups; i++ )
  {
    str += bold+"Gruppe "+(i+1)+":"+normal+"\n";
    if (!sizeof(qlists[i]))
      str += "  In dieser Gruppe gibt es derzeit keine Abenteuer.\n";
    else
    {
      qlists[i] = sort_array( qlists[i], "compare", ME );
      for( j=0; j<sizeof(qlists[i]); j++ )
      {
        if(qlists[i][j][4]==0)
          str +=
            sprintf( "  %-38s %3d   %2d     %-4s    %s\n",
              qlists[i][j][0], qlists[i][j][1], qlists[i][j][2],
              (pl->QueryQuest(qlists[i][j][0]) ? " ja": "nein"),
              capitalize(qlists[i][j][3])
        );
      }
      str += sprintf(
        "  (Von den Abenteuern dieser Gruppe %s ausgelassen werden.)\n",
        asText(dontneed[i]));
    }
    str += "\n";
  }
  str += bold+"Optionale Abenteuer:"+normal+"\n";
  n=0;
  for( i=0; i<qgroups; i++ )
  {
    for( j=0; j<sizeof(qlists[i]); j++ )
    {
      if(qlists[i][j][4]!=0) {
        n++;
        str += sprintf( "  %-38s %3d   %2d     %-4s    %s\n",
                       qlists[i][j][0], qlists[i][j][1], qlists[i][j][2],
                       (pl->QueryQuest(qlists[i][j][0]) ? " ja": "nein"),
                       capitalize(qlists[i][j][3])
                      );
      }
    }
  }
  if (!n)
    str += "  In dieser Gruppe gibt es derzeit keine Abenteuer.\n";

  i=j=pl->QueryProp(P_NEEDED_QP);
  if (QM->QuerySeerQP() > i) i = QM->QuerySeerQP();
  if (QM->QueryWizQP() > j) j = QM->QueryWizQP();
  if (i==j)
    str += sprintf(
      "\nUm "+bold+"Magier oder Seher"+normal+
      " zu werden, musst Du %d der %d Abenteuerpunkte\n"+
      "und %d Erfahrungspunkte erlangen.\n",
      i, QM->QueryMaxQP(), XP_NEEDED_FOR_WIZ);
  else
    str += sprintf(
      "\nUm "+bold+"Seher"+normal+
      " zu werden, musst Du %d der %d Abenteuerpunkte und\n"+
      "%d Erfahrungspunkte erlangen. Um "+bold+"Magier"+normal+
      " zu werden,\nbenötigst Du allerdings %d Abenteuerpunkte.\n",
      i, QM->QueryMaxQP(), XP_NEEDED_FOR_WIZ, j);
  this_player()->More(str, 0);
  return 1;
}
