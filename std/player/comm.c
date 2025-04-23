// Tamedhon MUDlib
//
// std/player/comm.c -- basic player communiction commands
//
// $Date: 2004/01/10 18:52:00 $
/* $Revision: 1.46 $
 * $Log: comm.c,v $
 * Revision 1.46 2004/01/10 18:52:00  Serii
 * commobj-fix
 *
 * Revision 1.45 2003/12/27 00:00:00 Falgrim
 * umbauten am farbsystem
 *
 * Revieion 1.4  2003/07/05 15:15:00  Querolin
 * MAX_HIST fuer thist von 10 auf 15 erweitert.
 * *tellallhist, tellhist, GetTellAllHist eingebaut.
 *
 * Revieion 1.3  2003/02/23 15:00:00  Querolin
 * Betrunkenen-Modus eingebaut (sage, rufe, teile mit)
 * 
 * Revieion 1.2  2001/06/02 16:35:00  Querolin
 * Einbau von _query_color und div. Farbumsetzungen
 *
 * Revision 1.1  2001/01/13 12:16:54  Samtpfote
 * Initial revision
 *
 * Revision 3.8  1998/01/24 20:39:18  Feyaria
 * ausgeben von DEAF_MSG
 *
 * Revision 3.7  1997/10/01 08:54:15  Woody
 * Bugfix in _whisper()
 *
 * Revision 3.6  1997/09/17 14:38:04  Woody
 * P_EARMUFFS um 1 erhoeht
 *
 * Revision 3.5  1997/06/25 12:01:15  Woody
 * RCS logs aufgeraeumt, neues Message() System mit Flags,
 * Teile-Mit History vereinfacht, Earmuffs ueberarbeitet
 *
 * Revision 3.4  1997/06/13 10:21:01  Woody
 * name() wieder auf ME->name() (fuer Shadowobjekte)
 *
 * Revision 3.3  1997/05/22 15:34:50  Feyaria
 * erwider mit unpsred_args :)
 *
 * Revision 3.2  1997/04/24 08:41:07  Feyaria
 * presay weg beim mitteilen von unsichtbaren magiern
 *
 * Revision 3.1  1997/04/04 13:35:54  Woody
 * jemandEM was zufluestern
 *
 * Revision 3.0  1997/03/13 01:34:27  Woody
 * prepared for revision control
 *
 * Revision 2.68  1997/01/06  Woody
 * tellhist ueberarbeitet, um doppelte teilemits auszufiltern
 *
 * Revision 2.67  1996/11/17  Woody
 * 'thist' eingebaut
 *
 * Revision 2.66  1996/10/03  Woody
 * P_PERM_STRING fuer Sprachflueche, erwieder -> erwider
 *
 * Revision 2.65  1996/01/11  18:30:00  Hadra
 * _tell(): teil mit Fehlermeldung fuer unsichtbare Magier gefixt
 * Message(): Koboldmeldung fuer unsichtbare Magier gefixt
 * Presays bei teil mit rausgeworfen
 *
 * Revision 2.64  1995/10/09  22:21:00  Mupfel
 * Neuer Befehl 'rede' fuer das Sprechen in einer Rassensprache
 *
 * Revision 2.63  1995/07/25  09:39:57  Wargon
 * Handling von P_DEAF
 *
 * Revision 2.62  1995/06/13  07:28:15  Rumata
 * Bisher wirkte oropax auf Spieler wie ignoriere. DAS WILL MAN NICHT!
 * Ausserdem ist der Code in Message wie Kraut und Rueben :-(
 * Deshalb nur: Buxfix.
 * 
 */

#pragma strong_types

inherit "/std/player/channel";
inherit "/std/thing/properties";

#define NEED_PROTOTYPES

#include "/std/sys_debug.h"

#include <thing/properties.h>
#include <player/comm.h>
#include <player/base.h>

#include <properties.h>
#include <config.h>
#include <ansi.h>
#include <wizlevels.h>
#include <language.h>
#include <udp.h>
#include <defines.h>
#include <daemon.h>
#include <ansi.h>

#define ECHO_COST 50
#define PERMUTATE(from,to) \
  if (objectp(QueryProp(P_PERM_STRING))) \
    to=call_other(QueryProp(P_PERM_STRING), "permutate_string", (from))||""; \
  else \
    to=from;

#define DRUNKDAEMON             "/p/daemon/drunkd.c"

static string *last_tell=({});
static string last_received;

#define MAX_HIST 15
static mixed *tellhist=({});
static mixed *tellallhist=({});

varargs string name(int casus, int demonst);
varargs string Name(int casus, int demonst);
static void AddToTellHist(string was);
static void AddToTellAllHist(string was);

//local property prototypes
static int _set_earmuffs(int level);
static int _query_intermud();

//static object commobj; <-- BUGT!
#define commobj find_object("/std/player/base")

private static string *buffer = ({});

void create()
{
  ::create();
  Set(P_EARMUFFS, 0);
  Set(P_EARMUFFS, SAVE, F_MODE);
  Set(P_EARMUFFS, SECURED, F_MODE);
  Set(P_INTERMUD, SAVE, F_MODE);
  Set(P_IGNORE, ({}), F_VALUE);
  Set(P_IGNORE, SAVE, F_MODE);
  Set(P_BUFFER, SAVE, F_MODE);
  tellhist=({});
  tellallhist=({});
//  commobj=find_object("/std/player/base.c");
}

private int _send(object ob, string message, int flag)
{
  mixed res;
  string color;

  color="";
  if (interactive(ob)) { color=commobj->_query_color(ob, P_COLOR_COMMUNICATION);}

  if(!call_resolved(&res, ob, "Message", message, flag))
    tell_object(ob, color+message+ANSI_NORMAL);
  return res;
}

varargs static int _flush_cache(string arg)
{
  string color;
//  DIE FARBE GEHT HIER NOCH NET!!! 
//  is wohl noch ein bug... naja mal spaeter schaun... 24.06.01 querolin
//  color=_query_color(this_player());
//  if (color) { sprintf(color); }
  switch(arg)
  {
    case "ein":
      SetProp(P_BUFFER, 1); 
      printf("Der Kobold merkt sich jetzt alles!\n"); break;
    case "aus":
      SetProp(P_BUFFER, 0); 
      printf("Der Kobold wird Dich nicht stören!\n"); break;
    default: if(arg) printf("Der Kobold sagt: kobold ein oder kobold aus\n");
  }
  if(sizeof(buffer))
  {
    tell_object(this_object(),
		sprintf("Ein kleiner Kobold teilt Dir folgendes mit:\n%s",
			implode(buffer, "")));
    buffer = ({});
  }     
//  if (color) { sprintf(ANSI_NORMAL); }
  return 1;
}

private int check_ignore(mixed ignore, string verb, string name)
{
  return (ignore == verb) ||
    ((sizeof(ignore = explode(ignore, ".")) > 1) &&
     (name == ignore[0] && member(ignore[1..], verb) != -1));
}

varargs int Message(string msg, int flag)
{
  object ti;
  string verb, reply, *ignore;
  int em, te;
  mixed deaf;

  ti = this_interactive();
  verb = query_verb();
  te = flag & (MSGFLAG_TELL | MSGFLAG_WHISPER);

  // fuer erwidern
  if (ti && (flag & MSGFLAG_TELL))
    if (!ti->QueryProp(P_INVIS)||IS_LEARNER(ME))
      last_received=getuid(ti);

  // Hoert der Spieler nicht?
  em = (ti && (te || flag & MSGFLAG_SHOUT) &&
        (QueryProp(P_EARMUFFS) &&
         (query_wiz_level(ti) < QueryProp(P_EARMUFFS))));
  ignore = (pointerp(ignore = QueryProp(P_IGNORE)) ? ignore : ({}));

  // Werden Sender oder Verb ignoriert?
  if (ti && (member(ignore, getuid(ti)) != -1)) {
    if(te && (IS_WIZARD(ti) || !QueryProp(P_INVIS)))
      tell_object(ti, Name()+" hört gar nicht zu, was Du sagst.\n");
    return MESSAGE_IGNORE_YOU;
  }
  if (ti && verb &&
     sizeof(filter(ignore, #'check_ignore/*'*/, verb, getuid(ti))))
  {
    if ((verb[0..2] != "ruf" && verb[0..3] != "mruf" &&
         verb[0..3] != "echo" && verb[0] != '-')
        && (IS_WIZARD(ti) || !QueryProp(P_INVIS)))
      tell_object(ti, Name()+" wehrt \""+verb+"\" ab.\n");
    return MESSAGE_IGNORE_VERB;
  }
  if (flag & MSGFLAG_RTELL) {
    int at;

    verb = lower_case(explode(msg, " ")[0][1..]);
    at = member(verb, '@');

    if ((member(ignore, verb) >= 0) || (member(ignore,verb[0..at]) >= 0))
      return MESSAGE_IGNORE_YOU;
    else if (at > 0 && member(ignore, verb[at..]) >= 0)
      return MESSAGE_IGNORE_MUD;
  }

  // Spieler taub?
  te |= (flag & MSGFLAG_SAY);

  if ((deaf=QueryProp(P_DEAF)) && (flag & MSGFLAG_DEAFCHK)
      && !(flag & MSGFLAG_CHIST))
  {
    if (te)
    {
      if( mappingp(deaf) )
        deaf=deaf[DEAF_MSG];
      if(!stringp(deaf))
        deaf = "%s ist momentan leider taub.\n";
      reply = capitalize(sprintf(deaf, name()));
    }
  }
  else if (em)
    reply = Name()+" hat Oropax in den Ohren.\n";

  if(QueryProp(P_BUFFER) &&
     (deaf ||
      query_editing(this_object()) ||
      query_input_pending(this_object())))
  {
    deaf = MESSAGE_DEAF;
    if(flag & MSGFLAG_CACHE)
    {
      AddToTellHist (msg);
      AddToTellAllHist (msg);
      if (!stringp(reply))
        reply = Name()+" möchte gerade nicht gestört werden.\n";

      if(sizeof(buffer) < 20)
      {
        buffer += ({ msg[0..<2]+" ["+dtime(time())[19..26]+"]\n" });
	reply += "Die Mitteilung wurde von einem kleinen Kobold in Empfang "
	  + "genommen.\nEr wird sie später weiterleiten!";
        deaf = MESSAGE_CACHE;
      }
      else {
	reply += "Die Mitteilung ging verloren, denn "
	  + "der Kobold kann sich nichts mehr merken!";
        deaf = MESSAGE_CACHE_FULL;
      }
      if (ti && (IS_WIZARD(ti) || !QueryProp(P_INVIS))) 
        tell_object(ti, reply+"\n");
    }
    return deaf;
  }
  else if ((deaf || em) && ((flag & MSGFLAG_RTELL) ||
            (ti && (IS_WIZARD(ti) || !QueryProp(P_INVIS))))) {
    if (te && ti)
      tell_object(ti, reply);
    return MESSAGE_DEAF;
  }

  _flush_cache();
  if (flag & MSGFLAG_CACHE) { AddToTellHist(msg); AddToTellAllHist(msg); }
  if(te && QueryProp(P_AWAY)) msg = msg[0..<2]+" ["+dtime(time())[19..23]+"]\n";
  if (this_object()->QueryProp(P_TTY)=="ansi")
  { msg=commobj->_query_color(this_object(), P_COLOR_COMMUNICATION)+msg+ANSI_NORMAL; } 
  tell_object(this_object(), msg);
  return MESSAGE_OK;
}

static int ignoriere(string str)
{
  mixed ignore;
  int i;
  string verb;
  string color;

  color=commobj->_query_color(this_player(), P_COLOR_COMMUNICATION);

  if (verb && verb!="ignorier" && verb!="ignoriere")
    return 0;
  if(!(ignore = Query(P_IGNORE))) ignore = ({});
  
  if (!str)
  {
    write(color+"Du ignorierst:\n"+implode(ignore, ", ")+"\n"+ANSI_NORMAL);
    return 1;
  }
  if (member(ignore,lower_case(str))!=-1)
  {
    Set(P_IGNORE, ignore -= ({ lower_case(str) }));
    printf(color+"Du ignorierst %s nicht mehr.\n"+ANSI_NORMAL, 
           capitalize(str));
    return 1;
  }
  Set(P_IGNORE, ignore += ({ lower_case(str) }));
  printf(color+"Du ignorierst jetzt folgende Mitteilungen: %s.\n"+
         ANSI_NORMAL, str);
  return 1;
}

int earmuffs(string s)
{
  int             x;
  string color;

  color=commobj->_query_color(this_player(), P_COLOR_COMMUNICATION);
  
  if (!s || s == "") {
    write(color+"Ok, Oropax aus.\n"+ANSI_NORMAL);
    SetProp(P_EARMUFFS, 0);
    return 1;
  }
  if (sscanf(s, "%d", x) == 0 || x < 0) {
    write(color+"Verwendung: oropax <level>\n"+ANSI_NORMAL);
    return 1;
  }
  if (SetProp(P_EARMUFFS, x) == x)
    printf(color+"Ok, Oropaxlevel ist jetzt %d.\n"+ANSI_NORMAL, x);
  else
    printf(color+"So hoch geht es nicht. Oropaxlevel ist jetzt %d.\n"+
           ANSI_NORMAL,
	   QueryProp(P_EARMUFFS));
  return 1;
}

private static string _my_race = "";

private int same_race(object pl)
{
  // alle NPC's bekommen die Meldung im Klartext
  return (!interactive(pl) || (pl->QueryProp(P_RACE) == _my_race)
          || IS_LEARNING(pl));
}

private int not_same_race(object pl)
{
  return (interactive(pl) && pl->QueryProp(P_RACE) != _my_race && !IS_LEARNING(pl));
}

static int _communicate(mixed str, int silent)
{
  string  verb;
  string  myname;
  string  msg,tmpmsg, newmsg;
  object *taub, *same, *not_same;
  int     i;
  string  color;

  //color=_query_color(this_player());
  color="";  //Solange 'say' nicht farblich unterstuetzt.... :(

  if (extern_call()) str=_unparsed_args()||"";
  verb = query_verb();
  if(stringp(verb) && verb[0] == '\'') str = verb[1..] + " " + str;
  if (str==""||str==" "||!str)
  {
    write(color+"Was willst Du sagen?\n"+ANSI_NORMAL);
    return 1;
  }

  /*
      Querolin:
      Betrunken? Dann sollt mans auch sehen ;)
  */
  newmsg=str;

     if (QueryProp(P_ALCOHOL))
     {
      tmpmsg = call_other(DRUNKDAEMON, "drunkspeach", str, this_player());
      if (tmpmsg) newmsg=tmpmsg;
     }

  PERMUTATE(newmsg,msg);
  
  myname = (((QueryProp(P_INVIS) || !IS_WIZARD(ME)) ||
            !(QueryProp(P_CAN_FLAGS) & CAN_PRESAY) ?
	      "" : QueryProp(P_PRESAY))+ME->name()) || "";
  taub = filter(all_inventory(environment()||this_object()),
                      #'interactive/*'*/);
  for (i=sizeof(taub)-1; i>=0; i--) 
    if (!(taub[i]->QueryProp(P_DEAF)))
      taub[i] = 0;
  taub -= ({ 0 });
  taub += ({ this_object() });
  if (verb == "rede" && !IS_WIZARD(ME))
  {
    _my_race = QueryProp(P_RACE);
    same     = filter(all_inventory(environment()||this_object()),
                            #'same_race);
    not_same = filter(all_inventory(environment()||this_object()),
                            #'not_same_race);
    // das hoeren alle meiner Rasse, NPC's und Magier mit 'mschau an':
    say(break_string(newmsg, 78, capitalize(myname)+" sagt: "), taub+not_same);
    // das hoeren alle anderen:
    switch (capitalize(_my_race))
    {
      case "Zwerg":
        say(capitalize(myname)+" murmelt etwas in "+QueryPossPronoun()+
            "en Bart, was Du nicht verstehst.\n", taub+same);
        break;
      case "Elf":
        say(capitalize(myname)+" spricht etwas in einem elfischen Dialekt.\n", taub+same);
        break;
      case "Mensch":
        say(capitalize(myname)+
            " sagt etwas in einer fremdartigen Sprache.\n", taub+same);
        break;
      case "Hobbit":
        say(capitalize(myname)+
            " scheint noch an der letzten Mahlzeit zu kauen.\n", taub+same);
        break;
      case "Troll":
        say(capitalize(myname)+
            " kauderwelscht herum.\n", taub+same);
        break;
      case "Ork":
        say(capitalize(myname)+
            " bricht sich fast die Zunge beim Husten.\n", taub+same);
        break;
      case "Goblin":
        say(capitalize(myname)+
            " klingt so, als wäre Fell in den Rachen gekommen.\n", taub+same);
        break;
      default:
        say(capitalize(myname)+" brummelt vor sich hin.\n", taub+same);
    }
  }
  else
    say(break_string(msg, 78, capitalize(myname)+" sagt: "), taub);
  if(!silent) write(break_string(newmsg, 78, "Du sagst: "));
  
  return 1;
}

static int _shout_to_all(mixed str)
{
  string pre, myname, realname, str1, str2, tmpmsg;
  int i,chars;
  object *u;
  string color, qtmpmsg, newmsg;

  color=commobj->_query_color(this_player(), P_COLOR_COMMUNICATION);

  
  if (!(str=_unparsed_args()))
  {
    write(color+"Was willst Du rufen?\n"+ANSI_NORMAL);
    return 1;
  }
  chars=sizeof(str)/2;
  if (chars<4) chars=4;
  pre = (!IS_WIZARD(ME) ||
         QueryProp(P_INVIS) ||
         !(QueryProp(P_CAN_FLAGS) & CAN_PRESAY)) ? "" : QueryProp(P_PRESAY);
  realname = capitalize((pre + capitalize(getuid()))||"");
  myname = capitalize(pre + ME->name()||"");
  if (QueryProp(P_INVIS))
    realname = "("+realname+")";

  /*
      Querolin:
      Betrunken? Dann sollt mans auch sehen ;)
  */
  newmsg=str;
    if (QueryProp(P_ALCOHOL))
    {
      qtmpmsg = call_other(DRUNKDAEMON, "drunkspeach", str, this_player());
      if (qtmpmsg) newmsg=qtmpmsg;
    }

  PERMUTATE(newmsg,str1);

  if(QueryProp(P_FROG))
    str2 = break_string("Quaaak, quaaaaaak, quuuuaaaaaaaaaaaaaaaaaaaak !!",
		        78, myname+" quakt: ");
  else
    str2 = break_string(str1, 78, myname+" ruft: ");

  // if(!IS_LEARNER(this_player()))
  if(!IS_SEER(this_player()))  // Seher++ duerfen kostenfrei rufen
  {
    if (QueryProp(P_SP) <(chars+20)) 
    {
      write("Du must erst wieder magische Kräfte sammeln.\n");
      write("Tip: Benutz doch mal die Kanäle (Hilfe dazu mit 'hilfe ebenen').\n");
      return 1;
    }
    SetProp(P_SP, QueryProp(P_SP) - chars - 20);
  }
    
  write(color+break_string(newmsg, 78, "Du rufst: ")+ANSI_NORMAL);
  str1=break_string(str1, 78, realname+" ruft: ");

  u=users()-({this_object()});
  for (i=sizeof(u)-1;i>=0;i--)
    if (IS_WIZARD(u[i]))
      _send(u[i],str1,MSGFLAG_SHOUT);
    else
      _send(u[i],str2,MSGFLAG_SHOUT);
  return 1;
}

varargs int _tell(mixed str,int intcall)
{
  object          ob;
  string          who;
  mixed           msg;
  string          myname;
  mixed           it;
  string          away;
  mixed           ignore;
  int             i;
  mixed          *xname;
  int             visflag;
  string          ret, newmsg, tmpmsg;
  string	  color;

  color=commobj->_query_color(this_player(), P_COLOR_COMMUNICATION);

  if (extern_call()&&this_interactive()!=ME) return 1;
  if (!(str=(intcall?str:_unparsed_args())) ||
      sscanf(str, "%s %s", who, msg) != 2) {
    write(color+"Was willst Du mitteilen?\n"+ANSI_NORMAL);
    return 1;
  }
  if (who == ".") who = ".1";
  if (sscanf(who, ".%d", i))
  {
    if (i > 0 && i <= sizeof(last_tell)) who = last_tell[i-1];
    else {
      _notify_fail(color+"Mit so vielen Leuten hast Du nicht gesprochen!\n"
                   +ANSI_NORMAL);
      return 0;
    }
  }
  xname = explode(who, "@");
  if (sizeof(xname) == 2) {
    if (ret=({string})INETD->send_udp(xname[1],([ REQUEST:   "tell",
						RECIPIENT: xname[0],
						SENDER:	   getuid(ME),
						DATA:	   msg ]), 1))
      write(ret);
    else
    {
      write(color+"Nachricht abgeschickt.\n"+ANSI_NORMAL);
      if (sizeof(last_tell))
      {
      	if (last_tell[0]!=who)
	  last_tell=(({who})+(last_tell-({who})))[0..19];
      } else last_tell=({who});
    }
    return 1;
  }
  if (!ob=find_player(it = lower_case(who)))
  {
    it = match_living(it, 0);
    if (!stringp(it))
      switch (it) {
	case -1:
	write(color+"Das war nicht eindeutig!\n"+ANSI_NORMAL);
	return 1;
	case -2:
	write(color+"Kein solcher Spieler!\n"+ANSI_NORMAL);
	return 1;
      }
    ob = find_player(it);
    if (!ob)
      ob = find_living(it);
    if (!ob)
    {
      write(color+"Kein solcher Spieler!\n"+ANSI_NORMAL);
      return 1;
    }
  }
  if (sizeof(last_tell))
  {
    if(last_tell[0]!=it)
      last_tell=(({it})+(last_tell-({it})))[0..19];
  } else last_tell=({it});
  if (QueryProp(P_INVIS))
  {
    if (!IS_WIZARD(ob))
      myname = ME->name();
    else
      myname = "("+capitalize(getuid())+")";
  }
  else
    myname = ((IS_WIZARD(ME) && (QueryProp(P_CAN_FLAGS) & CAN_PRESAY)) ?
              QueryProp(P_PRESAY):"") + ME->name();
  if (myname && sizeof(myname)) myname=capitalize(myname);

  visflag = (!ob->QueryProp(P_INVIS)) || IS_WIZARD(this_player());

  /*
      Querolin:
      Betrunken? Dann sollt mans auch sehen ;)
  */
  newmsg=msg;

//  if (QueryProp(P_NAME)=="Brownie")
     if (QueryProp(P_ALCOHOL))
     {
      tmpmsg = call_other(DRUNKDAEMON, "drunkspeach", msg, this_player());
      if (tmpmsg) newmsg=tmpmsg;
     }

  PERMUTATE(newmsg,str);

  _send(ob, break_string(str, 78, myname+" teilt Dir mit: "),MSGFLAG_TELL);

  if (visflag) 
    write(color+break_string(str, 78, "Du teilst "+capitalize(it)+" mit: ")
               +ANSI_NORMAL);

  if (!visflag&&interactive(ob))
    write(color+"Kein solcher Spieler!\n"+ANSI_NORMAL);
  else if (away = ({string}) ob->QueryProp(P_AWAY))
    printf(color+"%s ist gerade nicht da: %s\n"+ANSI_NORMAL, 
           capitalize(it), away);
  return 1;
}

static void AddToTellHist(string was)
{
  was=implode(explode(was,"\n")-({""}),"\n")+"\n";  // Leerzeilen raus
  if (sizeof(tellhist))
  {
    // check ob's evtl. bloss ein timeout-bedingter resend war (UDP/cross-mud)
    // (oder ob einfach jemand mit lauter gleichen mehrfach-tm's nervt)
    if ((tellhist[<1][1]==was) && (time()-tellhist[<1][0] <= 30))
    {
      tellhist[<1][0]=time();   // Zeitpunkt updaten
      return;
    }
  }
  tellhist += ({ ({ time(), was }) });
  while (sizeof(tellhist) > MAX_HIST) tellhist = tellhist[1..];
}

static void AddToTellAllHist(string was)
{
  was=implode(explode(was,"\n")-({""}),"\n")+"\n";  // Leerzeilen raus
  // und nochmal fuer tellallhist
  if (sizeof(tellallhist))
  {
    // check ob's evtl. bloss ein timeout-bedingter resend war (UDP/cross-mud)
    // (oder ob einfach jemand mit lauter gleichen mehrfach-tm's nervt)
      if ((tellallhist[<1][1]==was) && (time()-tellallhist[<1][0] <= 30))
    {
      tellallhist[<1][0]=time();   // Zeitpunkt updaten
      return;
    }
  }
  tellallhist += ({ ({ time(), was }) });
  while (sizeof(tellallhist) > MAX_HIST) tellallhist = tellallhist[1..];
}

static string GetTellHist()
{
  string str;
  int i;
  string b, n;
  string color;

  color=commobj->_query_color(this_player(), P_COLOR_COMMUNICATION);

  str=this_player()->QueryProp(P_TTY);
  if (str=="ansi")
    { b=ANSI_BOLD, n=ANSI_NORMAL+color; } //original n=ANSI_NORMAL; 
  else
  {
  if (str=="vt100")
  { b=ANSI_BOLD, n=ANSI_NORMAL; }
  else
    { b=n=""; }
  }
  if ((previous_object()!=ME) || (this_interactive()!=ME))
    return "Ungültiger Aufruf!\n";
  if (!sizeof(tellhist))
    return color+"Noch niemand hat Dir etwas mitgeteilt!\n"+ANSI_NORMAL;
  str="";
  for (i=0; i<sizeof(tellhist); i++)
    str += b+dtime(tellhist[i][0])[19..26]+" Uhr:"+n+"\n"+tellhist[i][1]+
    ANSI_NORMAL;
  return str+ANSI_NORMAL;
}

static string GetTellAllHist()
{
  string str;
  int i;
  string b, n;
  string color;

  color=commobj->_query_color(this_player(), P_COLOR_COMMUNICATION);

  str=this_player()->QueryProp(P_TTY);
  if (str=="ansi")
    { b=ANSI_BOLD, n=ANSI_NORMAL+color; } //original n=ANSI_NORMAL;
  else
  {
  if (str=="vt100")
  { b=ANSI_BOLD, n=ANSI_NORMAL; }
  else
    { b=n=""; }
  }
  if ((previous_object()!=ME) || (this_interactive()!=ME))
    return "Ungültiger Aufruf!\n";
  if (!sizeof(tellallhist))
    return color+"Keine Mitteilungen von Dir oder an Dich vorhanden!\n"+
                 ANSI_NORMAL;
  str="";
  for (i=0; i<sizeof(tellallhist); i++)
    str += b+dtime(tellallhist[i][0])[19..26]+" Uhr:"+n+"\n"+tellallhist[i][1]+
    ANSI_NORMAL;
  return str+ANSI_NORMAL;
}


static int _teile(string str)
{
  string          who, message, name;
   
  if (!(str=_unparsed_args())) return 0;
  if (sscanf(str, "%s mit %s", who, message) == 2) {
    if (match_living(who)==-1 || match_living(who)==-2) { 
       name=who; 
    }
    else { 
       name=match_living(who);
    }
    AddToTellAllHist(break_string(message+ANSI_NORMAL, 78, 
         "Du teilst "+capitalize(name)+" mit: "));
    return _tell(who + " " + message,1);
  }
  return 0;
}

static int _whisper(string str)
{
  object          ob;
  string          who;
  string          msg;
  string          myname;
  string          color;

  color=commobj->_query_color(this_player(), P_COLOR_COMMUNICATION);

  if (!(str=_unparsed_args()) || 
       (sscanf(str, "%s zu %s", who, msg) != 2 &&
        sscanf(str, "%s %s", who, msg) != 2)) {
    write(color+"Was willst Du wem zuflüstern?\n"+ANSI_NORMAL);
    return 1;
  }
  ob = present(lower_case(who), environment(this_player()));
  if (!ob || !living(ob)) {
    write(color+"Kein solcher Spieler in diesem Raum.\n"+ANSI_NORMAL);
    return 1;
  }
  PERMUTATE(msg,str);
  myname = capitalize((((IS_WIZARD(ME) && !QueryProp(P_INVIS) &&
                         (QueryProp(P_CAN_FLAGS) & CAN_PRESAY))?
                        QueryProp(P_PRESAY) : "") + ME->name()) || "");
  _send(ob, sprintf(commobj->_query_color(ob, P_COLOR_COMMUNICATION)
       +"%s flüstert Dir zu: %s\n"+ANSI_NORMAL,
        myname,str), MSGFLAG_WHISPER);
  write(sprintf(color+"Du flüsterst %s zu: %s\n"+ANSI_NORMAL, 
        ob->name(WEM), msg));
  say(sprintf("%s flüstert %s etwas zu.\n", myname, ob->name(WEM, 1)), ob);
  return 1;
}

static int _converse()
{
  write("Mit '**' wird das Gespräch beendet.\n");
  write("]");
  input_to("_converse_more");
  return 1;
}

static int _converse_more(mixed str)
{
  string          cmd;
  int             i;
  string          myname;

  if (str == "**") {
    write("Ok.\n");
    return 0;
  }
  myname=capitalize((((IS_WIZARD(ME)&&!QueryProp(P_INVIS) &&
                     (QueryProp(P_CAN_FLAGS) & CAN_PRESAY))?
		      QueryProp(P_PRESAY):"")+ME->name())||"");
  if(str != "") _communicate(str, 1); 
  write("]");
  input_to("_converse_more");
  return 0;
}

private int is_learner(object o) { return IS_LEARNER(o); }

static int _shout_to_wizards(mixed str)
{
  int             i, j;
  string          myname;
  object         *u;
  string	 color;

  color=commobj->_query_color(this_player(), P_COLOR_COMMUNICATION);
 
  str = _unparsed_args();
  if (!str||!sizeof(str)) {
    write(color+"Was willst Du den Magiern zurufen?\n"+ANSI_NORMAL);
    return 1;
  }
  myname = capitalize(getuid(this_object()));
  if (!IS_WIZARD(this_object())) write("Ok.\n");
  map(filter(users(), #'is_learner), //')))
#'_send, break_string(str, 78, myname+" an alle Magier: "),MSGFLAG_MECHO);//'
  return 1;
}

static int _echo(string str) {
  if (!IS_SEER(ME) || (!IS_WIZARD(ME) &&
		       !(QueryProp(P_CAN_FLAGS)&CAN_ECHO)))
    return 0;
  if (!(str=_unparsed_args())) {
    write ("Was möchtest Du 'echoen'?\n");
    return 1;
  }
  if (!IS_WIZARD(this_interactive()))
  {
    if (QueryProp(P_SP)<ECHO_COST)
    {
      _notify_fail("Du musst erst wieder magische Kräfte sammeln.\n");
      return 0;
    }
    SetProp(P_SP,QueryProp(P_SP)-ECHO_COST);
    str=">\b"+str;
  }
  say ( str + "\n");
  write ( str + "\n");
  return 1;
}

//****local property methods
static int _set_earmuffs(int level)
{
  int max;
  
  max=1+query_wiz_level(this_object());

  if (max>99)
    max=99;

  return Set(P_EARMUFFS, level > max ? max : level);
}

static mixed _set_ignore(mixed arg)
{
  if(stringp(arg)) arg = ({ arg });
  if(pointerp(arg)) return Set(P_IGNORE, arg);
  return Query(P_IGNORE);
}

static int _query_intermud()
{
  mixed tmp;
  return member(pointerp(tmp=Query(P_CHANNELS))?tmp:({}), "Intermud") > -1;
}

int erwidere(string str)
{
  if (last_received)
    return _tell(last_received+" "+(_unparsed_args() || ""), 1);

  _notify_fail("Du hast aber noch keine Mitteilungen erhalten, "
               "auf die Du was erwidern\nkönntest.\n");
  return 0;
}

static int tmhist()
{
  int i;
  
  if (!sizeof(last_tell))
  {
    _notify_fail("Du hast noch keinem etwas mitgeteilt.\n");
    return 0;
  }
  for (i=sizeof(last_tell)-1;i>=0;i--)
    printf(".%-2d %s\n",i+1,last_tell[i]);
  return 1;
}

static int thist()
{
  write (GetTellHist());
  return 1;
}

static int tellhist()
{
  write (GetTellAllHist());
  return 1;
} 

static string *_query_localcmds()
{
  return ({
     ({"kobold", "_flush_cache",1,0}),
     ({"sag","_communicate",1,0}),
     ({"'","_communicate",1,0}),
     ({"rede","_communicate",1,0}),
     ({"mruf","_shout_to_wizards",1,0}),
     ({"ruf","_shout_to_all",1,0}),
     ({"erzaehl","_tell",1,0}),
     ({"erzähl","_tell",1,0}),
     ({"teil","_teile",1,0}),
     ({"fluester","_whisper",1,0}),
     ({"flüster","_whisper",1,0}),
     ({"gespraech","_converse",1,0}),
     ({"gespräch","_converse",1,0}),
     ({"echo","_echo",0,0}),
     ({"oropax","earmuffs",0,WIZARD_LVL}),
     ({"ignorier","ignoriere",1,0}),
     ({"tmhist","tmhist",1,0}),
     ({"thist","thist",1,0}),
     ({"tellhist","tellhist",1,0}),
     ({"erwider","erwidere",1,0}),
    })+channel::_query_localcmds();
}

