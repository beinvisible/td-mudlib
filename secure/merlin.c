/* Tamedhon MUDlib
 *
 * /secure/merlin.c -- Der gute alte Merlin
 *
 * 2024-09-07 Gralkor Komplette Typisierung und Deklaration
 * 2024/05/23 wo befindet sich merlin ausgelagert - Aloy
 *
 * 2024/05/11 - pragmas vom MG merlin, typisierungen, datentypanpassungen - Aloy
 *
 * $Date: 2008/09/10 12:00:00 $
 * $Revision: 1.95 $
 * $Log: merlin.c,v $
 *
 * Revision 1.95 2008/09/10 12:00:00  TamTam
 * create_wizard() Stufe 2 - Lehrling->Neumagierstatt Vollmagier
 * Magierstufe von FULL_WIZARD_LVL auf WIZARD_LVL angepasst
 *
 * Revision 1.94 2008/07/18 11:18:00  Querolin
 * TOPLISTINPUT angepasst an Seher und Erzseher
 *
 * Revision 1.93 2008/03/21 14:00:00  TamTam
 * Erzseher eingefuehrt
 *
 * Revision 1.92 2005/03/10 20:25:00  Gralkor
 * Modifikation an der Merlin-Shouterei (Steppe und Nordmeer ;) )
 *
 * Revision 1.91 2004/08/20 18:32:00  Querolin
 * Drei Statistiken von Gidhrog hierher verschoeben auf Grund Nekura ;)
 *
 * Revision 1.9  2003/02/26 11:22:00  Querolin
 * _checkplayercount erweitert
 *
 * Revision 1.8  2003/01/01 11:00:00  Querolin
 * degradiere ueberarbeitet um Erzmagier abstufen zu koennen.
 *
 * Revision 1.7  2001/09/09 10:31:00  Querolin
 * PLAYERSTAT_AGE eingefueft. Top-Liste nach Alter.
 *
 * Revision 1.6  2001/06/25 07:35:00  Querolin
 * Login-Renew eingefuegt. Sonst muss ich x Spieler man. renewn *G*
 *
 * Revision 1.5  2001/04/15 16:00:00  Querolin
 * Osterei eingebaut. (Am 17.4. wieder entfernt)
 *
 * Revision 1.4  2001/01/22 17:30:00  Querolin
 * PlayerCount eingebaut
 *
 * Revision 1.3  2000/06/12 09:28:56  Samtpfote
 * merlin weiss auch nun, wo die brownies zuhause sind
 *
 * Revision 1.2  2000/06/11 20:09:06  Samtpfote
 * Anpassung der Pfade hinsichtlich Tamedhon,
 * Magier werden nimmer verletzt, Einbau von Debugmeldungen
 *
 * Revision 1.1  2000/06/11 19:43:54  Samtpfote
 * Initial revision
 *
 * Revision 1.9  1997/11/26 13:04:31  Feyaria
 * No_attack, no_global_attack, Name(), degradiere, spielerloeschung
 *
 * Revision 1.8  1997/10/05 22:22:45  Feyaria
 * magierwerdung ab 30 statt ab 31 :)
 *
 * Revision 1.7  1997/09/18 09:55:14  Woody
 * QueryMaxUsersToday() funktioniert nun wie es sollte
 *
 * Revision 1.6  1997/08/21 14:04:32  Woody
 * QueryMaxUsers(), QueryMaxUsersToday()
 *
 * Revision 1.5  1997/07/05 14:55:47  Woody
 * Magier_und_Seher_Text() gebugfixt, jetzt flexibler
 * (kneisst selber, wieviel Questgruppen es gibt :))
 *
 * Revision 1.4  1997/06/12 12:26:10  Woody
 * Ein bissl Debugging, und QueryReadyForSeer() (statt Wiz)
 *
 * Revision 1.3  1997/06/05 12:47:02  Feyaria
 * huegelland beim rufen dazugefuegt
 *
 * Revision 1.2  1997/03/29 00:06:12  Feyaria
 * Sponsoring erst ab Questmagier-Level
 *
 * Revision 1.1  1997/03/15 23:15:06  Woody
 * prepared for revision control
 *
 * Jan 06, 1997  Woody  Befoerderung ueberarbeitet, ausserdem wandert Merlin
 *                      jetzt wenns sein muss auch durch Special Exits.
 */

/*
 * Der gute alte Merlin...
 * ...macht Spieler zu Sehern, Seher zu Magiern, befoerdert Magier,
 * registriert Spieler, die ins Spiel kommen, und vieles mehr.
 */

//_types
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma verbose_errors
#pragma combine_strings
//#pragma pedantic
//#pragma range_check
#pragma warn_deprecated

#include <properties.h>
#include <language.h>
#include <moving.h>
#include <defines.h>
#include <news.h>
#include "/secure/wizlevels.h"
#include "/secure/config.h"
#include "/secure/questmaster.h"
#include "/secure/exploration.h"
#include "/secure/parentmaster.h"
#include "/secure/merlin_dest.h"

#ifndef DEBUG
#define DEBUG(x) if (find_player("samtpfote")) tell_object(find_player("samtpfote"),x)
#endif

#define SAVEFILE "/secure/ARCH/merlin"
#define PLAYERCOUNT "/p/service/querolin/playercount.c"
#define PLAYERSTAT_AGE "/p/service/querolin/listen/stat_alter.c"
#define SCHEIDUNGSMASTER "/secure/scheidung.c"

#define LXP_LISTE "/p/service/querolin/listen/stat_lxp.c"
#define FP_LISTE  "/p/service/querolin/listen/stat_fp.c"
#define AGE_LISTE "/p/service/querolin/listen/stat_alter.c"

mixed QueryProp(string prop);
int move(mixed dest, int method);
string query_g_suffix(int gen, int casus, int anzahl);
string Magier_und_Seher_Text(object pl);
string make_invlist(object viewer,object *inv);
static mixed determine_action(mixed mess,mixed name);
static int create_wizard(mixed who, mixed promoter);
int goto(mixed dest);

static string GetCurrentDay();
int QueryMaxUsers();
int QueryMaxUsersToday();

static string *whitespaces=({",",".","?",";",":","-","!","\n","\t"});
static object prev_room;
static int delay,max,maxtoday;
static string flag,currentday;
string *login_renew_list;

mapping MBanishListe;


void create()
{
  string s;
  login_renew_list=({});
  seteuid(ROOTID);
  if (file_size("/log/statistik/max.users")<=0)
    max=sizeof(users());
  else {
    s=read_file("/log/statistik/max.users",0,1);
    sscanf(s,"%d",max);
  }
  if (file_size("/log/statistik/max.users.today")<=0) {    // no info yet...
    maxtoday=0;
    currentday=0;
  } else {                                      // read /etc/maxusers.today
    s=explode(read_file("/log/statistik/max.users.today",0,1),"\n")[0];
    sscanf(s,"%d|%s|",maxtoday,currentday);
  }
  /*(void)*/ QueryMaxUsersToday();          // write out to /etc/maxusers.today

  set_living_name("merlin");
  enable_commands();
  call_out("wandern",25);
  // call_out("load_pwand",1);  // hamwer nich
  move("/gilden/abenteurer",0);
  MBanishListe = m_allocate(0,2);
  restore_object(SAVEFILE);
}

void load_pwand()
{
//  catch(call_other("/players/cymonk/obj/pwand","???"));
}

string _query_kill_name()
{
  return "Ein genervter Merlin";
}

int move(mixed ob,int methods)
{
  object tmp;

  if (methods&M_GET)
    return ME_CANT_BE_TAKEN;

  if (stringp(ob))
  {
    if (!(tmp=find_object(ob)))
    {
      call_other(ob,"?");
      tmp=find_object(ob);
    }
    ob=tmp;
  }
  if (!objectp(ob))
    return 0;
  if (environment())
    tell_room(environment(),"Merlin zieht weiter.\n");
  tell_room(ob,"Merlin kommt an.\n");
  move_object(ob);
  return 1;
}

mixed QueryProp(string str)
{
  if (!stringp(str)||str=="")
    return 0;
  return call_other(this_object(),"_query_"+str);
}

int id(string str)
{
  return (str=="merlin");
}

string name(int casus, int demon)
{
  if (casus!=WESSEN)
    return "Merlin";
  return "Merlins";
}

string QueryDu(int casus, int gender, int zahl)
{
  return
    ({ ({ ({ "du", "ihr"}), ({ "du", "ihr"}), ({ "du", "ihr"}) }),
	 ({({"deines","deiner"}),({"deines","deiner"}),({"deiner","deiner"})}),
	 ({({"dir","euch"}),({"dir","euch"}),({"dir","euch"})}),
	 ({({"dich","euch"}),({"dich","euch"}),({"dich","euch"})})
       })[casus][gender][zahl];
}

string long()
{
  return "Das ist Merlin, der mächtige Urvater aller Magier.\n"
    "Er ist unangreifbar, denn er steht über solchen Dingen.\n";
}

string short()
{
  return "Merlin, der Urvater aller Magier.\n";
}

string _query_race()
{
  return "Magier";
}

int _query_gender()
{
  return MALE;
}

int _query_no_global_attack()
{
  return 1;
}

mixed _query_noattack()
{
  write("Merlin gibt Dir eine schallende Ohrfeige.\n");
  say(sprintf("Merlin gibt %s eine schallende Ohrfeige.\n",
             this_player()->name(WEM)), this_player());
  return "schimpft: Lass das!!!";
}

string Name(int casus, int demon)
{
  return name(casus, demon);
}

string QueryPronoun(int casus)
{
  switch (casus)
  {
    case WER: return "er";
    case WEM: return "ihm";
    default:  return "ihn";
  }
}

string QueryPossPronoun( mixed what, int casus, int number )
{
  int gen2;
  gen2 = ( intp( what ) ) ? what : what->QueryProp(P_GENDER);
  return "sein" + query_g_suffix( gen2, casus, number );
}

string query_g_suffix( int gen, int casus, int anzahl )
{
  return ({ ({ ({"","e"}), ({"es","er"}), ({"em","en"}), ({"","e"}) }),
	      ({ ({"","e"}), ({"es","er"}), ({"em","en"}), ({"en","e"}) }),
	      ({ ({"e","e"}), ({"er","er"}), ({"er","en"}), ({"e","e"}) }) })
    [gen][casus][anzahl];
}

string _query_short()
{
  return short();
}

string _query_long()
{
  return long();
}

string _query_name()
{
  return "Merlin";
}

int _query_weight()
{
  return 75000;
}

static void PostSeher(string who, int g)
{
  mixed *art;

  art=({"chronik","Merlin",0,0,
      sprintf("%s ist jetzt ein%s Seher%s!",
        capitalize(who),(g==2?"e":""),(g==2?"in":"")),
      sprintf("\nAm %s hat %s die nötigen Bedingungen\n"
        "erfüllt und ist vom Spielerstatus zum Seherstatus aufgestiegen.\n"
        "\n Herzlichen Glückwunsch, %s!\n",dtime(time()),
        capitalize(who),capitalize(who))});

  "/secure/news"->WriteNote(art,1);
}

static void PostErzseher(string who, int g)
{
  mixed *art;

  art=({"chronik","Merlin",0,0,
      sprintf("%s ist jetzt ein%s Erzseher%s!",
        capitalize(who),(g==2?"e":""),(g==2?"in":"")),
      sprintf("\nAm %s hat %s die nötigen Bedingungen\n"
        "erfüllt und ist vom Seherstatus zum Erzseherstatus aufgestiegen.\n"
        "\n Herzlichen Glückwunsch, %s!\n",dtime(time()),
        capitalize(who),capitalize(who))});

  "/secure/news"->WriteNote(art,1);
}

static void PostMagier(string who, string prom, int gender)
{
  mixed *art;

  art = ({"chronik","Merlin",0,0,
    sprintf("%s ist jetzt ein%s Magier%s!",
      capitalize(who),(gender==2?"e":""),(gender==2?"in":"")),
    sprintf("\nAm %s hat %s %s zu%s Magier%s\nberufen. "
      "Herzlichen Glückwunsch, %s. Glückwünsche auch an Dich,\n"
      " %s, zu Deine%s neuen %s!\n",dtime(time()),prom,who,
      (gender==2?"r":"m"),(gender==2?"in":""),who,prom,
      (gender==2?"r":"m"),(gender==2?"Tochter":"Sohn"))+
      "\n\nMerlin.\n"
     });

  "/secure/news"->WriteNote(art,1);
}

static void PostMagierLevelChange
	(string who, string prom, int gender, int degrad, int level)
{
  mixed *art;

  art = ({"chronik","Merlin",0,0,
  		sprintf("%s wurde "+(degrad?"degradiert":"befördert")+"!"
  			,capitalize(who)),

  		sprintf("\nAm %s hat %s %s auf Stufe %i "+
  			(degrad?"degradiert.\n\nHerzliches Beileid, das wird schon wieder":
  			 "befördert.\n\nHerzlichen Glückwunsch zur neuen Stufe")+".",
  			dtime(time()), capitalize(prom), capitalize(who),
  			level)+
  		"\n\nMerlin.\n"
  	});

  "/secure/news"->WriteNote(art,1);
}


void notify_player_change(string who,int rein)
{
  int i, mag, invis;
  object *u,ob;
  string wer;

  if (!stringp(who))
    return;
  wer=capitalize(who);

  if (file_size("/log/statistik/max.users")<=0)
    max=0;
  if (sizeof(users())>=max)
  {
    max=sizeof(users());
    catch(rm("/log/statistik/max.users"));
    write_file("/log/statistik/max.users",sprintf("%d user: %s (%s)\n",
        max,dtime(time()),query_load_average()));
  }
  QueryMaxUsersToday();  // Update maxtoday
  PLAYERCOUNT->Register(find_player(lower_case(who)));  //zaehlen
  PLAYERSTAT_AGE->Register(find_player(lower_case(who))); // Altersliste
  u=users();
  mag=IS_LEARNER(who);
  if(!(ob=find_player(who)) && !(ob=find_netdead(who)))
  {
  if (find_player("querolin")) { tell_object(find_player("querolin"),
      break_string(
       who+" nicht gefunden!!!.\n",78) ); }

    return;
  }
  invis=ob->QueryProp(P_INVIS);
  for (i=sizeof(u)-1;i>=0;i--)
    if (!(mag&&invis&&!IS_LEARNER(u[i]))&&u[i]!=ob)
      u[i]->notify_player_change(wer,rein,invis);
}

int loginrenew(string who)
{
  // oben ist noch der string *login_renew_list definiert und im
  // create auf ({}) gesetzt!!

  string err;
  object ob;
  int         is, i;
  string      fmtstr;
  if (member(login_renew_list, who) != -1) { return 1; }
  ob=find_player(who);
  if (!ob)
    return 1;
  if (!interactive(ob))
  {
    err="This is not an interactive player";
    return 1;
  }

  if ((err="/secure/master"->renew_player_object(ob))<0)
    err="error "+err+" when renewing "+who;
   tell_object(find_player("querolin"),break_string(this_player()->name()+
      " --> LOGINRENEW! : "+err+"\n"));

  login_renew_list+=({who});

  is=sizeof(login_renew_list);
    if(!is) return 1;

    fmtstr="%s   %s\n";
    rm("/log/statistik/login_renew_merlin");
    for(i=0; i<is; i++)
    {
        write_file("/log/statistik/login_renew_merlin",
            sprintf(fmtstr,
                    ctime(time()),
                    capitalize(login_renew_list[i])
                   ));
    }
  return 1;
}

int ostereicheck()
{
  object ob;
  //if (find_player("querolin"))
  //{
  //   tell_object(find_player("querolin"),break_string(this_player()->name()+
  //    " --> Ostereicheck!\n"));
  //}
  if (!present("\nosterei", this_player()))
  {
    tell_object(this_player(),break_string("Der Osterhase hoppelt vorbei und steckt Dir schnell "
     +"ein kleines Osterei zu.\n"));
    //if (find_player("querolin"))
    //{
    //   tell_object(find_player("querolin"),break_string(this_player()->name()+
    //    " hat grad ein Osterei bekommen.\n"));
    //}
    ob=clone_object("/d/gidhrog/gralkor/gidhrog-stadt/obj/osterei");
    ob->move(this_player(), M_NOCHECK);
  }
  return 1;
}

int _checkplayercount(string who)
{
   object ob;
   string wiz_sec;
   int seerstatus;

   ob=find_player(lower_case(who));
   PLAYERCOUNT->Register(ob);
   PLAYERSTAT_AGE->Register(ob);
   if (!ob) return 1;
   if (ob->QueryProp(P_SECOND))
   {
      wiz_sec=to_string(query_wiz_level(lower_case(ob->QueryProp(P_SECOND))));
   }
   else
   {
      wiz_sec="0";
   }

//   if(IS_LEARNER(ob)) return 1;
   if(!ob) return 1;
   if(ob->QueryProp(P_TESTPLAYER)) return 1;
   if(lower_case(who[0..3]) == "gast") return 1;

   seerstatus=0;
   if (IS_SEER(ob)) { seerstatus=1; }
   if (ob->QueryProp(P_CAN_FLAGS)&CAN_ERZ) { seerstatus=2; }

   log_file("ARCH/TOPLISTINPUT.TXT",capitalize(lower_case(who))+";"+
            IS_LEARNER(ob)+";"+
	    query_wiz_level(ob)+";"+
	    to_string(seerstatus)+";"+
            ob->QueryProp(P_AGE)+";"+
//            ob->QueryProp(P_ATTRIBUTES)+";"+
	    ob->QueryProp(P_DEADS)+";"+
            ob->QueryProp("ep_num")+";"+
	    ob->QueryProp(P_GENDER)+";"+
            ob->QueryProp("guild_level")+";"+
            ob->QueryProp(P_LEVEL)+";"+
            ob->QueryProp(P_LOST_XP)+";"+
	    ob->QueryProp(P_SECOND)+";"+
            ob->QueryProp(P_QP)+";"+
            ob->QueryProp(P_XP)+";"+
            capitalize(lower_case(ob->QueryProp(P_GUILD)))+";"+
            capitalize(lower_case(ob->QueryProp(P_RACE)))+";"+
            ob->QueryProp("guild_rating")+";"+
	    wiz_sec+";\n" );

       call_other(LXP_LISTE, "???");
       call_other(FP_LISTE, "???");
       call_other(AGE_LISTE, "???");

       LXP_LISTE->Register(ob);
       FP_LISTE->Register(ob);
       AGE_LISTE->Register(ob);

   return 1;
}


void notify_player_enter(string who)
{
//  if (find_player("querolin")) { tell_object(find_player("querolin"),
//      break_string(
//       who+" in notify_player_enter() in /secure/merlin.c aufgerufen.\n",78) ); }

//call_out("ostereicheck",1);
//  call_out("loginrenew",2, who);  // deaktiviert - querolin!
  /* Da die Sache mitm Playercount nicht immer 100% funktioniert hat
     mal versuchen obs so geht... *gruebel*
     quero
  */
  call_out("_checkplayercount",1, who);
  SCHEIDUNGSMASTER->_login_check(find_player(who));
  //PLAYERCOUNT->Register(find_player(lower_case(who)));
  notify_player_change(who,1);
}

void notify_player_leave(string who)
{
  PLAYERCOUNT->Register(find_player(lower_case(who)));
  PLAYERSTAT_AGE->Register(find_player(lower_case(who)));
  notify_player_change(who,0);
}

static string GetCurrentDay()
{
  return ctime()[0..9] + ctime()[<5..];
}

int QueryMaxUsers()
{
  return max;
}

int QueryMaxUsersToday()
{
  if ((sizeof(users()) > maxtoday) || (currentday != GetCurrentDay()))
  {
    maxtoday=sizeof(users());
    currentday=GetCurrentDay();
    catch(rm("/log/statistik/max.users.today"));
    write_file("/log/statistik/max.users.today",
        sprintf("%d|%s|\n", maxtoday, currentday));
  }
  return maxtoday;
}

void Defend(int dam, string dam_type, int spell, object enemy)
{
  object en;
  if (!en=previous_object() || !living(en))
    en = this_player();
  tell_room(environment(), "Merlin schimpft: Lass das!!!\n");
  write("Merlin gibt Dir eine schallende Ohrfeige.\n");
  say(sprintf("Merlin gibt %s eine schallende Ohrfeige.\n",
	      this_player()->name(WEM)), this_player());
  en->StopHuntFor(this_object(), 1);
}

void shoutansw()
{
  object env,ti;
  string *path;
  string ans;

  if (!(env=environment()))
    return;
  // if (!(ti=this_interactive())||(int)ti->QueryProp(P_LEVEL)<19||ti->QueryProp(P_GHOST))
  if (!(ti=this_interactive()) || !query_wiz_level(ti) || ti->QueryProp(P_GHOST))
    return;
  if(!IS_LEARNER(ti))
  {
    tell_object(ti,"Du spürst einen sengenden Schmerz.\n");
    ti->do_damage(ti->QueryProp(P_HP)/2+5);
  }
  if(ti->QueryProp(P_GHOST)) return;
  ans="Ich hab nicht die leiseste Idee, wo ich hier bin ...\n";
  path=explode(object_name(env),"/");

  DEBUG(sprintf("*** Merlins Raum: %s\n", explode(object_name(env), "#")[0]));
  ans=merlin_dest(path);

  ans="Merlin ruft: "+ans+"\n";
  call_out("myshout",0,ans);
}

void myshout(string s)
{
  write(s);
  shout(s);
}

void catch_tell(string str)
{
  string name, dummy;
  mixed message;
  int i,s;

  if (!this_player())
    return;

  if (environment()==environment(this_player()) && previous_object()!=ME)
    tell_room(this_object(),"*"+str);

  if (this_player() != this_interactive())
    return;

  str=lower_case(str);

  if ((strstr(str,"merlin")!=-1 || strstr(str,"teilt")!=-1) &&
      strstr(str,"wo")!=-1 && strstr(str,"bist")!=-1)
    return shoutansw();

  if (sscanf(str,"%s sagt: %s",name,message)!=2)
    return;
  name=explode(name," ")[<1];

  if (!name || name=="" ||
	    catch(s=file_size("save/"+name[0..0]+"/"+name+".o")) || s<=0 )
    return;

  if (name!=getuid(this_interactive()) && !IS_ARCH(this_interactive()))
  {
    if (flag==getuid(this_interactive())+"##"+name)
      return;
    call_out("do_say",0,"Merlin sagt: "+
        capitalize(getuid(this_interactive()))+
        " hat gerade so getan, als würde "+
        capitalize(name)+" etwas sagen!\n");
    flag=getuid(this_interactive())+"##"+name;
    call_out("clear_flag",0);
    return;
  }
  flag=0;
  for (i=0;i<sizeof(whitespaces);i++)
  {
    message=old_explode(message,whitespaces[i]);
    if (!pointerp(message)||sizeof(message)==0)
      return;
    message=implode(message," ");
  }

  message=explode(message," ");

  if (!pointerp(message) || !sizeof(message))
    return;

  message-=({0})-({""});

  if (!message || sizeof(message)==0)
    return;

  if (member(message,"merlin")==-1)
    return;

  message-=({"merlin"});

  message=implode(message," ");

  if (!message || message=="")
    return;

  determine_action(message, name);
}

static mixed determine_action(mixed mess,mixed name)
{
  string str;

  if ((strstr(mess,"mach mich zum seher")!=-1) ||
      (strstr(mess,"mach mich zur seherin")!=-1))
    return call_out("create_seer", 0, this_player());

  if ((strstr(mess,"mach mich zum erzseher")!=-1) ||
      (strstr(mess,"mach mich zur erzseherin")!=-1))
    return call_out("create_archseer", 0, this_player());

  if (mess=="mach mich zum magier" || mess=="mach mich zur magierin")
    return create_wizard(name,name);

  if (sscanf(mess,"mach %s zum magier",str)==1||sscanf(mess,"mach %s zur magierin",str)==1)
    return create_wizard(lower_case(str),name);
  
  return 1;
}

#define Say(str) say(str,who)
#define Write(str) tell_object(who,str)

int QueryConditions(object pl, int wiz)
{
  int ret,i,gen;

  if (IS_SEER(pl))
    return 1;
  if (pl->QueryProp(P_TESTPLAYER)) return 1;
  gen=pl->QueryProp(P_GENDER);
  if (pl->QueryProp(P_XP)<XP_NEEDED_FOR_WIZ)
  {
    tell_object(pl,"Dir fehlt noch die nötige Erfahrung.\n");
    say(pl->name(WER)+" hat noch nicht genug Erfahrung für den "+
        (wiz ? "Magier" : "Seher")+"status.\n",pl);
    return 0;
  }
  ret = wiz ? QM->QueryReadyForWiz(pl) : QM->QueryReadyForSeer(pl);
  if (ret==-1)
  {
    tell_object(pl,"Dir fehlen noch Abenteuerpunkte. "
        "Lös doch erstmal noch ne Quest, ok?\n");
    say(pl->name(WER)+" hat noch nicht genug Abenteuerpunkte für den "+
        (wiz ? "Magier" : "Seher")+"status.\n", pl);
    return 0;
  }

  if (ret!=1)
  {
    tell_object(pl,
      "Du hast zwar genug Abenteuerpunkte, aber es gibt Abenteuer, die man\n"+
      "einfach lösen MUSS, um " + (wiz ? "Magier" : "Seher") +
      (gen != 2 ? "" : "in") + " zu werden.\n");
    tell_object(pl,Magier_und_Seher_Text(pl));
    tell_object(pl,"\nLöse die fehlenden Abenteuer und komm dann wieder.\n");
    return 0;
  }
 
  return 1;
}

int QueryArchseerConditions(object pl)
{
  if (IS_LEARNER(pl) || pl->QueryProp(P_TESTPLAYER))
  {
    tell_object(pl,"Haha. Sehr witzig.\n");
    say(pl->name(WER)+" versuchte als "+(IS_LEARNER(pl)?"Magier":"Testie")+
      " Erzseher zu werden.\n",pl);
    tell_room(environment(),"Merlin lacht sich schier tot.\n");
    return 0;
  }
  if (!IS_SEER(pl))
  {
    tell_room(environment(),"Merlin sagt: Werd doch erst mal Seher, "+pl->name()+"!\n");
    return 0;
  }
  if (pl->QueryProp(P_LEVEL)<50)
  {
    tell_object(pl,"Dir fehlt noch die nötige Spielerstufe.\n");
    say(pl->name(WER)+" hat noch nicht die nötige Spielerstufe für den "+
        "Erzseherstatus.\n",pl);
    return 0;
  }
  if (pl->QueryProp(P_QP)<QM->QueryMaxQP())
  {
    tell_object(pl,"Dir fehlen noch die nötigen Questpunkte.\n");
    say(pl->name(WER)+" hat noch nicht genug Questpunkte für den "+
        "Erzseherstatus.\n",pl);
    return 0;
  }
  if (pl->QueryProp(P_EP_NUM)<(EPMASTER->QueryMaxEP()/2))
  {
    tell_object(pl,"Dir fehlen noch die nötigen Forscherpunkte.\n");
    say(pl->name(WER)+" hat noch nicht genug Forscherpunkte für den "+
        "Erzseherstatus.\n",pl);
    return 0;
  }
  return 1;
}

static string _astext(int i, string ex)
{
  if ( (i>12) || (i<0) ) return to_string(i);
  return ({ "kein"+ex, "ein"+ex, "zwei", "drei", "vier", "fünf",
            "sechs", "sieben", "acht", "neun", "zehn", "elf", "zwölf"}) [i];
}

string Magier_und_Seher_Text(object pl)
{
  int *dn,i;
  string str;

  // QGROUPS is defined in /secure/questmaster.h
  dn = QM->QueryDontneed(pl);

  str="";
  for (i=sizeof(QGROUPS)-1; i>=1; i--)
    str+="von den Abenteuern ab "+QGROUPS[i]+" Punkten darfst Du "+
      _astext(dn[i],"es")+" auslassen,\n";
  str="\n"+capitalize(str)+"und von den ganz leichten Abenteuern darfst Du "+
    _astext(dn[0],"es")+" auslassen.\n\n"+
"Falls Du in einer Gruppe mit schwereren Abenteuern mehr gelöst haben\n"+
"solltest, als erforderlich war, so kannst Du Dir dafür eine leichtere\n"+
"Aufgabe ersparen.\n";

  return str;
}

static int create_wizard(mixed who, mixed promoter)
{
  mixed *domains;
  object vertrag;
  int ret;
  string gen;

  who=find_player(who);
  promoter=find_player(promoter);
  if (!who || !promoter)
    return 0;
  //
  // player not interactive?
  //
  if (!interactive(who))
  {
    write(capitalize(getuid(who))+" ist nicht aktiv.\n");
    return 0;
  }
  //
  // wants to advance himself?
  //
  if (who==promoter)
  {
    write("Du kannst Dich nicht selber befördern. Such Dir dazu bitte einen "+
	      "Vollmagier,\nder Dir einen Vertrag gibt.\n");
    return 1;
  }
  //
  // banished?
  //
  if (MBanishListe[getuid(who),1] && !IS_ARCH(promoter))
  {
    tell_object(promoter,capitalize(getuid(who))+
        " darf nur von einem Erzmagier zum Magier gemacht werden.\n");
    say(promoter->Name(WER)+" ist kein Erzmagier und darf "+
        capitalize(getuid(who))+" nicht zum Magier machen.\n", promoter);
    return 1;
  }
  //
  // player has PK?
  //
  if (who->QueryProp("playerkills")) {
     tell_room(environment(who),who->name()+
         " hat noch einen Playerkill und kann somit kein Magier werden.\n"
         "Am besten den Sheriff oder einen Erzmagier verständigen.\n");
     return 1;
  }
  //
  // already dom_member?
  //
  if (IS_DOMAINMEMBER(who))
  {
    Write("Für Dich kann ich nichts mehr tun. Du bist doch schon Magier!\n");
    return 1;
  }
  //
  // already wizard?
  //
  if (IS_WIZARD(who))
  {
    domains="secure/master"->get_domain_homes(getuid(who));
    if (!domains || !pointerp(domains) || !sizeof(domains))
    {
      Say(who->Name(WER)+" gehört noch keiner Region an und kann daher noch "+
	      "kein Voll-\nmagier werden.\n");
      Write("Du gehörst noch keiner Region an. Schließ Dich zunächst einer "+
	        "an und komm\ndann wieder !\n");
      return 0;
    }
    ret="secure/master"->advance_wizlevel(geteuid(who),25);
    if(ret>0)
    {
      shout(who->Name(WER)+" arbeitet jetzt an einer Region mit !\n");
      "secure/master"->renew_player_object(who);
      return 1;
    }
    write("RETURNVALUE "+ret+"\n");
    say("RETURNVALUE "+ret+"\n");
    return 0;
  }
  //
  // promoter's level is not >DOMAINMEMBER_LVL+1
  if (query_wiz_level(promoter)<QUESTWIZ_LVL)
  {
    tell_object(promoter,"Du musst derzeit mindestens Level "+
	            (QUESTWIZ_LVL)+" haben, um jemanden beförden zu "+
				"können.\n");
    say(promoter->name(WER)+" ist kein Vollmagier und darf daher niemanden "+
	    "zum Vollmagier machen.\n");
    return 1;
  }
  if (IS_LEARNER(who))
  {
    ret="secure/master"->advance_wizlevel(geteuid(who), WIZARD_LVL);
    if (ret<=0)
    {
      say("Merlin: error "+ret+"\n");
      write("Merlin: error "+ret+"\n");
    }
    who->SetProp (P_START_HOME, "players/"+getuid(who)+"/workroom");
    write("Merlin ruft: "+who->name(WER)+" ist in den Kreis der Magier "+
	      "aufgenommen worden!\n");
    shout("Merlin ruft: "+who->name(WER)+" ist in den Kreis der Magier "+
	      "aufgenommen worden!\n");
    if ("secure/master"->set_player_object(geteuid(who),"/std/shells/magier")
	    <=0)
    {
      say("MERLIN: konnte Magiershell nicht einstellen.\n");
      write("MERLIN: konnte Magiershell nicht einstellen.\n");
    }

    "secure/master"->renew_player_object(who);
    return 1;
  }
  if (!(vertrag=present("unterschriebenvertrag",who)))
  {
    Write("Du hast ja gar keinen unterschriebenen Vertrag bei Dir. Besorg Dir "+
	      "einen Ver-\ntrag, unterschreibe ihn und komm dann wieder !\n");
    Say(who->name(WER)+" hat keinen unterschriebenen Vertrag.\n");
    return 1;
  }
  if (geteuid(vertrag)!=geteuid(promoter))
  {
    tell_object(promoter,"Das geht nicht, er hat einen Vertrag von "+
	            capitalize(geteuid(vertrag))+" bei sich.\n");
    say(promoter->name(WER)+" versucht, "+who->name(WER)+" zum Magier zu "+
	    "machen, doch "+who->name(WER)+" hat einen Vertrag\nvon "+
		capitalize(geteuid(vertrag))+" bei sich.\n",promoter);
    return 0;
  }
  if (!QueryConditions(who,1))
    return 0;
  ret="secure/master"->advance_wizlevel(geteuid(who), LEARNER_LVL);
  if (ret>0)
  {
    PARENTMASTER->SetParent(getuid(who), getuid(promoter));
    PARENTMASTER->SetChild(getuid(promoter), getuid(who));

    who->SetProp (P_START_HOME, "room/lehrlingsbude");
    PostMagier(who->name(WER),promoter->name(WER),who->QueryProp(P_GENDER));
    write_file("/log/SPONSOR",dtime(time())+": "+promoter->name(WER)+" macht "+
             who->name(WER)+" zum Learner.\n");
    write(who->name(WER)+" ist in den Kreis der Magier aufgenommen worden!\n");
    shout(who->name(WER)+" ist in den Kreis der Magier aufgenommen worden!\n");
    if ("secure/master"->set_player_object(geteuid(who),"/std/shells/lehrling")
	    <=0)
    {
      say("MERLIN: konnte Magiershell nicht einstellen.\n");
      write("MERLIN: konnte Magiershell nicht einstellen.\n");
    }
    "secure/master"->renew_player_object(who);
    return 1;
  }
  write(" --RETURNVALUE IS "+ret+"\n");
  say(" --RETURNVALUE IS "+ret+"\n");
  
  return 0;
}

void clear_flag()
{
  flag=0;
}

void do_say(string str)
{
  say(str);
}

int create_seer(mixed player)
{
  string playername, was;
  int ret, gender;

  if (!objectp(player)) return 0;
  if (IS_SEER(player))
  {
    tell_room (environment(), "Merlin sagt: Du bist doch schon kein "+
      "normaler Spieler mehr, "+player->name()+"!\n");
    return 1;
  }

  if (!("std/gilde"->seer_cond(1)))
  {
    tell_room (environment(), "Merlin sagt: Du erfüllst noch nicht alle "+
      "Bedingungen, um Seher zu werden!\n");
    return -1;
  }

  if (!(ret=QueryConditions(player,0)))
    return -1;
  /* Samtpfote 6.4.2002 -- wer hat das denn da eingebaut?
   *                       MagierBanish hat ja nix mit Seher
   *                       zu tun ....
  if (MBanishListe[getuid(player),1])
  {
   write("Du kannst hier nicht zum Seher werden.\n");
    return 1;
  }
  */


  tell_room (environment(), "Merlin sagt: Also gut, dann wollen wir mal...\n");
  if ("secure/master"->advance_wizlevel(geteuid(player),1)<=0)
  {
    tell_room (environment(), "Merlin flucht: Da geht was schief ... sag mal "+
      "einem Erzmagier Bescheid, sag nur: "+ret+" ...\n");
    return -1;
  }

  playername = geteuid(player);
  "secure/master"->renew_player_object(player);
  player = find_player(playername);
  gender=player->QueryProp(P_GENDER);
  was = gender==FEMALE ? "eine Seherin" : "ein Seher";

  "std/gilde"->UpdateTitle(player->QueryProp(P_LEVEL), player);
  write("Merlin ruft: "+player->name(WER)+" ist jetzt "+was+" !\n");
  shout("Merlin ruft: "+player->name(WER)+" ist jetzt "+was+" !\n");
  PostSeher(player->name(WER), gender);

  return 1;
}

int create_archseer(mixed player)
{
  string playername, was;
  int ret, gender;

  if (!objectp(player)) return 0;
  if (player->QueryProp(P_CAN_FLAGS)&CAN_ERZ)
  {
    tell_room (environment(), "Merlin sagt: Du bist doch schon ein "+
      "Erzseher, "+player->name()+"!\n");
    return 1;
  }

  if (!(ret=QueryArchseerConditions(player)))
    return -1;

  tell_room (environment(), "Merlin sagt: Also gut, dann wollen wir mal...\n");
  player->SetProp(P_CAN_FLAGS, player->QueryProp(P_CAN_FLAGS)|CAN_ERZ);

  gender=player->QueryProp(P_GENDER);
  was = gender==FEMALE ? "eine Erzseherin" : "ein Erzseher";

  write("Merlin ruft: "+player->name(WER)+" ist jetzt "+was+" !\n");
  shout("Merlin ruft: "+player->name(WER)+" ist jetzt "+was+" !\n");
  PostErzseher(player->name(WER), gender);

  return 1;
}

void reset()
{
}

void wandern()
{
  mapping ex,rooms;
  mixed ziel;
  object *raeume,*laeden,env,ob;
  string *ind,fn;
  int i;

  while (remove_call_out("wandern")>=0);
  call_out("wandern",45+random(100));
  if (delay)
  {
    delay=0;
    return;
  }
  if (!(env=environment()))
  {
    move("/gilden/abenteurer",0);
	env=environment();
  }
  for(raeume=all_inventory(env);sizeof(raeume);raeume=raeume[1..])
    if (interactive(raeume[0]) && query_idle(raeume[0])<180 &&
	raeume[0]->QueryProp(P_XP)>999999)
      return;
  ex=env->QueryProp(P_EXITS);
  if (!mappingp(ex))
    return;
  ind=m_indices(ex);rooms=([]);laeden=({});
  for (i=sizeof(ind)-1;i>=0;i--)
  {
    if (!stringp(ziel=ex[ind[i]]))
      continue;
    if (!ob=find_object(ziel))
      continue;
    rooms+=([ob]);
  }
  rooms-=([env]);
  if (prev_room && sizeof(m_indices(rooms))>1)
    rooms-=([prev_room]);
  prev_room=env;
  raeume=m_indices(rooms);
  if (!i=sizeof(raeume)) {
    // Verloren? Nicht ganz!
    ex=env->QueryProp(P_SPECIAL_EXITS);
    ind=m_indices(ex);
    while ((environment()==env) && (i=sizeof(ind))) {
      command(fn=ind[random(i)]);
      ind-=({fn});
    }
    if (environment()==env)
      move("/gilden/abenteurer", 0);  // Sackgasse? -> Gilde.
  }
  else {
    ob=raeume[random(i)];
    move(ob,0);
  }
}

string int_short()
{
  return "Du bist im allseits beliebten Wizclub \"Zum lustigen Merlin\".\n";
}

string int_long(object who,mixed where)
{
  string descr;

  if( IS_LEARNER(who) && who->QueryProp( P_WANTS_TO_LEARN ) )
    descr = "[" + object_name(ME) + "]\n";
  else
    descr = "";
  descr+="Du bist im Wizclub 'Zum lustigen Merlin'. Dieser Wizclub ist etwas "
    "besonderes -\ner hat keinen festen Standort, sondern wandert im Mud "
    "herum. Außerdem kann er\nSpieler zu Magiern machen - aber nur, wenn "
    "man ihn von außen aus darauf\nanspricht.\n";
  if ( who->QueryProp(P_SHOW_EXITS) )
    descr += "Es gibt 2 sichtbare Ausgänge: raus und gilde.\n";
  descr += make_invlist(who, all_inventory(ME) - ({ who }));
  if( environment())
    descr += "Außerhalb siehst Du:\n"
      + environment()->int_short(who,ME);
  return descr;
}

string make_invlist(object viewer,object *inv)
{
  int i, iswiz;
  string descr;

  descr = "";
  iswiz = IS_LEARNER( viewer ) && viewer->QueryProp(P_WANTS_TO_LEARN);

  for (i = 0; i < sizeof(inv); i++)
  {
    string sh;
    sh = inv[i]->short();
    if (iswiz)
    {
      if (sh) sh = sh[0..<2] + " ["+object_name(inv[i])+"]\n";
      else sh = "["+object_name(inv[i])+"]\n";
    }
    if (sh) descr += sh;
  }
  return descr;
}

void init()
{
  if (interactive(this_player()) && environment(this_player())==ME)
    add_action("gehen","",1);
  add_action("befoerdere","befoerder",1);
  add_action("befoerdere","beförder",1);
  add_action("degradiere", "degradiere",1);
  add_action("spielerloeschung", "spielerloeschung",1);
  add_action("spielerloeschung", "spielerlöschung",1);
}

int befoerdere(string str)
{
  string wen;
  int ret,stufe,l;

  if (!IS_LORD(this_player())) return 1;
  if (this_player() != this_interactive()) return 1;
  if (!str||sscanf(str,"%s auf stufe %d",wen,stufe)<2)
  {
    notify_fail("Syntax: befördere <name> auf stufe <stufe>\n");
    return 0;
  }
  if (stufe>99)
  {
    write("Maximum ist Stufe 99!\n");
    return 1;
  }
  if ((stufe>30) && !IS_ARCH(this_player()))
  {
    write("Maximum ist Stufe 30!\n");
    return 1;
  }
  l=query_wiz_level(wen);
  if (l<10)
  {
    printf("%s muss zwecks Weiterbeförderung mindestens Stufe 10 haben.\n",capitalize(wen));
    return 1;
  }
  if (l>=stufe)
  {
    printf("%s ist schon Stufe %d.\n",capitalize(wen),l);
    return 1;
  }
  // ret="secure/master"->advance_wizlevel(wen,stufe);
  ret=MASTER->advance_wizlevel(wen,stufe);
  if (ret<=0)
    printf("Errorcode %d.\n", ret);
  else {
    printf("Ok, %s auf Stufe %d befördert.\n", capitalize(wen), stufe);
    if (find_player(wen)) {
      tell_object (find_player(wen), sprintf(
        "Du wurdest von Merlin auf Stufe %d befördert.\n", stufe));
      "/obj/infomaster"->InfoMsg(sprintf(
        "%s wurde von Merlin auf Magierlevel %d befördert.\n",
        capitalize(wen), stufe));
    }
  }

  PostMagierLevelChange(
  	wen, this_player()->name(), find_player(wen)->QueryProp(P_GENDER),
  	0, stufe);

  return 1;
}

int degradiere(string str)
{
  string wen, mshell;
  int ret,stufe,l;

  if (!IS_ARCH(this_player())) return 1;
  if (this_player() != this_interactive()) return 1;
  if (!str||sscanf(str,"%s auf stufe %d",wen,stufe)<2)
  {
    notify_fail("Syntax: degradiere <name> auf stufe <stufe>\n");
    return 0;
  }
  if (stufe<10)
  {
    write("Minimum ist Stufe 10!\n");
    return 1;
  }
  l=query_wiz_level(wen);
  if (l<=stufe)
  {
    printf("%s ist schon Stufe %d.\n",capitalize(wen),l);
    return 1;
  }
/*
  if(l>=ELDER_LVL )
  {
    printf("%s kann so nicht degradiert werden!.\n",capitalize(wen));
    return 1;
  }
*/
  ret=MASTER->update_wiz_level(wen,stufe);
  if (ret<=0)
    printf("Errorcode %d.\n", ret);
  else {
    printf("Ok, %s auf Stufe %d degradiert.\n", capitalize(wen), stufe);
    if (find_player(wen)) {
      tell_object (find_player(wen), sprintf(
        "Du wurdest von Merlin auf Stufe %d degradiert.\n", stufe));
      "/obj/infomaster"->InfoMsg(sprintf(
        "%s wurde von Merlin auf Magierlevel %d degradiert.\n",
        capitalize(wen), stufe));
    }
  }

    PostMagierLevelChange(
  	wen, this_player()->name(), 1, //find_player(wen)->QueryProp(P_GENDER),
  	1, stufe);


  if (find_player(wen))
  {
  mshell="/std/shells/magier";

  if (stufe >= ARCH_LVL)
  {
    mshell="/std/shells/erzmagier";
  }

  if (stufe <= LEARNER_LVL)
  {
    mshell="/std/shells/lehrling";
  }

  if ("secure/master"->set_player_object(geteuid(find_player(wen)),mshell)
            <=0)
    {
      say("MERLIN: konnte Magiershell nicht einstellen.\n");
      write("MERLIN: konnte Magiershell nicht einstellen.\n");
    }

    "secure/master"->renew_player_object(wen);
  }
  else
  {
    write("MERLIN: Magier nicht gefunden - Shell nicht aktualisiert!\n");
  }

  return 1;
}

int spielerloeschung(string str)
{
  object ob;

  if (!IS_ARCH(this_player())) return 0;
  if (this_player() != this_interactive()) return 0;
  if( ob=find_player(str) )
  {
    ob->remove();
    if(ob) destruct( ob );
  }
  if( ob=find_netdead(str) )
  {
    ob->remove();
    if(ob) destruct( ob );
  }
  MASTER->playerdel(str);
  return 1;
}


int sag(mixed str)
{
  int i;

  if (str==0)
    str="";
  str = explode(break_string(str, 60 ), "\n");
  for (i = 0; i < sizeof(str); i++)
  {
    if (!str[i])
      str[i]="";
    tell_room(environment(),"Merlin sagt: "+str[i]+"\n");
    tell_room(ME,"*Merlin sagt: "+str[i]+"\n");
  }
  return 1;
}

void show_exits (mixed ex)
{
  printf("%s",environment()->GetExits());
}

gehen(string str)
{
  string verb;
  int i;
  mapping exits;
  string *ex;

  verb=query_verb();str=this_interactive()->_unparsed_args();
  switch (verb)
  {
    case "gilde":
      this_player()->move("/gilden/abenteurer",M_GO,"in die Gilde");
      return 1;
    case "raus":
      this_player()->move(environment(),M_GO,"raus");
      return 1;
  }
  if (!IS_WIZARD(this_interactive()))
    return;
  if (verb!="merlin")
    return;
  delay=1;
  exits=environment()->QueryProp(P_EXITS);
  if (!str||str=="")
  {
    ex=m_indices(exits);
    printf(environment()->int_short(ME,ME));
    show_exits();
    return 1;
  }
  if (explode(str," ")[0]=="sag")
    return sag(implode(explode(str," ")[1..]," "));
  if (str[0]==';')
  {
    tell_room(environment(ME),"Merlins "+str[1..]+"\n");
    tell_room(ME,"*Merlins "+str[1..]+"\n");
    return 1;
  }
  if (str[0]==':')
  {
    tell_room(environment(ME),"Merlin "+str[1..]+"\n");
    tell_room(ME,"*Merlin "+str[1..]+"\n");
    return 1;
  }
  if (str[0]=='*')
  {
    goto(str[1..]);
    printf(environment()->int_short(ME,ME));
    show_exits();
    return 1;
  }
  if (!exits[str])
    return 0;
  move(exits[str],0);
  write("Merlin sagt: Jawoll, Chef.\n");
  printf(environment()->int_short(ME,ME));
  show_exits();
  return 1;
}

int goto(mixed dest)
{
  object ob;
  string path;
  mixed ret;

  if (!dest) {
    write("Wohin möchtest Du Merlin bringen ?\n");
    return 1;
  }
  ob = find_living(dest);
  if (ob) {
    ob = environment(ob);
    move(object_name(ob),0);
    return 1;
  }
  path="secure/master"->_get_path(dest,getuid(this_player()));
  ret=catch(call_other(path,"?"));
  if (ret && file_size(path)<0)
  {
    dest=match_living(dest,1);
    if (stringp(dest))
    {
      ob = environment(find_living(dest));
      move(object_name(ob),M_TPORT|M_NOCHECK);
      return 1;
    }
    printf("Fehler beim Teleport nach %O: No such file.\n",path);
    return 1;
  }
  if (ret)
    printf("Fehler beim Teleport nach %O: %O\n",dest,ret);
  ret=catch(move(path,M_TPORT|M_NOCHECK));
  if (stringp(ret))
    ret=implode(explode(ret,"\n"),"");
  if (ret)
    printf("Fehler beim Teleport nach %O: %O\n",dest,ret);
  return 1;
}

mixed locate_objects(string complex_desc)
{
  object ob;

  if (complex_desc=="alles" || complex_desc=="alle")
    return filter_objects(all_inventory(this_object()),"short");
  if (ob = present(complex_desc,this_object()) )
    return ({ ob });
  return ({}) ;
}

void MBanishInsert(string name, string grund, object wer)
{
  if (!name || name=="" || !grund || grund=="")
    return;
  if (wer!=this_interactive())
    return;
  if (!wer||!objectp(wer)||!IS_ARCH(wer))
    return;
  MBanishListe += ([name:grund;getuid(wer)]);
  save_object(SAVEFILE);
}

void MBanishDelete(string name)
{
  if (!name || name=="")
    return 0;
  if (!IS_ARCH(this_interactive()))
    return 0;
  MBanishListe = m_delete(MBanishListe,name);
  save_object(SAVEFILE);
}

string _query_noget()
{
  return "Merlin mag das nicht!\n";
}
