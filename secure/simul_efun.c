//
// Wunderland Mudlib
//
// secure/simul_efun.c -- simulierte Efuns
//
// Basierend auf Morgengrauen Mudlib
// 
// Bedeutung von simul_efun.c <-> spare_simul_efun.c
// =================================================
//
// Wenn man an simul_efun.c etwas aendert, und das File buggt beim
// Neuladen, kann das ganze Mud abstuerzen. Um gefahrlos Aenderungen
// durchzufuehren, gibt es spare_simul_efun.c. Laesst sich simul_efun
// naemlich nicht laden, so wird es mit der spare-Version versucht.
// Wird die spare_simul_efun destructet wird wieder erstmal die simul_efun
// probiert, dann die spare_simul_efun.
// Bevor man etwas an simul_efun aendert, sollte man das File also einfach
// einmal nach spare_simul_efun.c umkopieren. Dann kann man gefahrlos
// alle Sachen ausprobieren. Nicht vergessen, auch die Spare immer
// upzudaten!
// Und natuerlich ist es zwecklos, die spare_simul_efun.c ins RCS
// aufzunehmen! Das ist wirklich nur ein Hilfsfile, meistens eine Kopie
// der letzten lauffaehigen Version von simul_efun.c.
//
// $Log: simul_efun.c,v $
// Revision 1.257  2021/04/17 19:18:38  Fuchur
// inut_to: binary_message will bytes, keine strings
//
// Revision 1.256  2021/04/02 21:21:00  Fuchur
// remove some alist functions
//
// Revision 1.255  2021/03/28 12:34:07  Fuchur
// `nosave` bei Variablen wiederhergestellt
//

#pragma strong_types
#pragma no_clone
#pragma no_inherit
#pragma no_shadow

// Absolute Pfade erforderlich - zum Zeitpunkt, wo der Master geladen
// wird, sind noch keine Include-Pfade da ...

#include <commands.h>
#include <driver_info.h>

#include "/secure/config.h"
#include "/secure/wizlevels.h"
#include "/sys/snooping.h"
#include "/sys/language.h"
#include "/sys/thing/properties.h"
#include "/sys/thing/description.h"
#include "/sys/wizlist.h"
#include "/sys/erq.h"
#include "/sys/lpctypes.h"
#include "/sys/daemon.h"
#include "/sys/properties.h"
#include "/sys/events.h"
#include "/sys/defines.h"
#include "/sys/telnet.h"
#include "/sys/terminal.h"
#include "/sys/files.h"
#include "/sys/time.h"
#include "/sys/input_to.h"
#include "/sys/notify_fail.h"
#include "/sys/ansi.h"
#include "/sys/strings.h"
#include "/sys/commands.h"

// Kompatibilitaetsfunktionen fuer ldmud 3.5
#include "/secure/sefun/bytes.c"
#include "/secure/sefun/query_once_interactive.c"
#include "/secure/sefun/query_ip_name.c"
#include "/secure/sefun/strlen.c"
#include "/secure/sefun/query_snoop.c"
#include "/secure/sefun/alist.c"
#include "/secure/sefun/set_heart_beat.c"
#include "/secure/sefun/shadow.c"
#include "/secure/sefun/query_input_pending.c"
#include "/secure/sefun/cat.c"
#include "/secure/sefun/debug_info.c"
#include "/secure/sefun/enable_commands.c"
#include "/secure/sefun/hash.c"
#include "/secure/sefun/query_editing.c"
#include "/secure/sefun/query_idle.c"
#include "/secure/sefun/query_load_average.c"
#include "/secure/sefun/query_mud_port.c"
#include "/secure/sefun/query_udp_port.c"
#include "/secure/sefun/seteuid.c"
#include "/secure/sefun/set_connection_charset.c"
#include "/secure/sefun/set_prompt.c"
#include "/secure/sefun/tail.c"
#include "/secure/sefun/terminal_colour.c"

#define TO       efun::this_object()
#define TI       efun::this_interactive()
#define TP       efun::this_player()
#define PO       efun::previous_object(0)
#define LEVEL(x) query_wiz_level(x)
#define NAME(x)  capitalize(getuid(x))

#define SHADOWLOGFILE "ARCH/SHADOWS"
#define RMILOGFILE    "ARCH/REMOVE_INTERACTIVE"

#define DEBUG(x) if (find_player("debugger")) tell_object(find_player("debugger"),x)

#define COLS (["^R":ANSI_RED,"^G":ANSI_GREEN,"^B":ANSI_BLUE,\
		"^Y":ANSI_YELLOW,"^W":ANSI_WHITE,"^P":ANSI_PURPLE,\
		"^C":ANSI_CYAN,"^I":ANSI_INVERS,"^F":ANSI_BLINK,\
		"^N":ANSI_NORMAL,"^D":ANSI_GREY,"^S":ANSI_BLACK,\
		"^A":ANSI_BOLD,"^L":ANSI_YELLOW+ANSI_WHITE])

// prototypes
varargs string     dtime(varargs mixed* args);
int                file_date(string file);
varargs int        log_file(string file, string txt, int size_to_break, int bakmode);
int                query_wiz_level(mixed player);
nomask varargs int snoop(object me, object you);
nomask object      query_snoop(object who);
string *           get_dir_ext(string path);
string *           new_explode(string str, string del);
varargs string     country(mixed ip, string num);
void               localcmd();
int                query_wiz_grp(mixed wiz);
void               set_living_name(string name);
object             find_living(string name);
object             find_player(string name);
varargs object     deep_present(mixed what, object ob);
string             mixed_to_string(mixed mix, int lvl);
varargs string     countup(string *foo, mixed i);
varargs string extract(string str, int from, int to);


//breakpoint hack, um den code irgendwie tracen zu können
void breakpoint(string message, int loopcount)
{
    for(int i=0;i<loopcount;i++)
    {
      write(message+"\n");
      efun::debug_message(message+"\n");
    }
} 

nomask void destruct(object ob)
{
  object tmp;

  if( member( (["/secure/master", "/obj/shut",
                "/room/void", "/room/netztot"]), object_name(ob) ) )
  {
    if( previous_object()!=ob && 
       (!this_interactive() || !IS_ARCH(this_interactive()) ))
    {
      write("You have no permission to destruct the specified object!\n");
      return;
    }
  }
  if (query_once_interactive(ob) && IS_ARCH(ob) && previous_object()!=ob &&
      (!previous_object() || !IS_ARCH(geteuid(previous_object()))) &&
      (!this_interactive() || !IS_ARCH(this_interactive())))
    return;
  tmp=ob;
  while (tmp=environment(tmp))
    tmp->Set(P_LAST_CONTENT_CHANGE,time());
  efun::destruct(ob);
}

void ___updmaster() {
  write("Removing old master ...\n");
  efun::destruct(efun::find_object(__MASTER_OBJECT__));
  write("Done. - Loading again ...\n");
  load_object(__MASTER_OBJECT__);
  write("Done.\n");
}

void ___updsimul() {
  object ob;
  write("Removing old simul ...\n");
  ob = find_object(SIMUL_EFUN_FILE);
  if (ob) destruct(ob);
  ob = find_object(SPARE_SIMUL_EFUN_FILE);
  if (ob) destruct(ob);
  write("Done. - Loading again ...\n");
  __MASTER_OBJECT__->get_simul_efun();
  write("Done.\n");
}

// * Herkunfts-Ermittlung

mapping ip_map;

varargs string query_ip_number(object ob)
{
  ob=ob||this_player();
  if (!interactive(ob)) return 0;
  return ob->_query_my_ip()||efun_query_ip_number(ob);
}

string query_ip_name(object ob)
{
  string ipnum;
  // mixed ipname;

  ipnum=query_ip_number(ob);
  if(ipnum == "127.0.0.1" || ipnum == "212.132.115.155")
    return "Tamedhon-HQ";
  else
    return efun_query_ip_name(ob) || ipnum;  // geht natuerlich nur mit ERQD
}

void __clear_ip_map()
{
  mixed *info;

  info = get_extra_wizinfo(0);
  ip_map=info[IP_NAMES]=m_allocate(0,2);
}

mapping dump_ip_mapping() {
  return copy(ip_map); 
}

varargs string country(mixed ip, string num)
{
  // if(objectp(ip)) return call_other("/p/daemon/townd","town",ip);
  return call_other("/p/daemon/townd","town",ip);
  // return call_other("/p/daemon/townd","town",num);
}

// * Snoopen und was dazugehoert

// Finde heraus ob (und wen ja wen) 'who' snoopt
static object find_snooped(object who) {
  object *u;
  int i;

  for (i=sizeof(u=users())-1;i>=0;i--)
    if (who==efun_query_snoop(u[i]))
      return u[i];
  return 0;
}

private string Lcut(string str) {
  return str[5..11]+str[18..];
}

nomask varargs int snoop(object me, object victim) {
  object old_victim, old_snooper;
  string txt, me_name, vic_name, os_name, *slist;
  int ret;
  
  if (!me || !interactive(me)) return 0;
  if (extern_call() && previous_object() != me) return 0;

  if (me == victim) {
    tell_object(me, "Du kannst Dich nicht selbst snoopen.\n");
    return 0;
  }

  me_name = capitalize(getuid(me));
  old_victim = find_snooped(me);
  if (old_victim) {
    vic_name = capitalize(getuid(old_victim));
    if (old_victim == victim) {
      tell_object(me, "Du snoopst " + vic_name + " bereits.\n");
      return 0;
    }
  }

  // Snoop ausschalten oder alten Snoop beenden
  if (!victim || old_victim) {
    if (!old_victim) tell_object(me, "Du snoopst niemanden.\n");
    else {
      if (!IS_ARCH(me) || !(me->QueryProp(P_SNOOPFLAGS) & SF_SILENT)) {
        log_file(SNOOPLOGFILE, sprintf("%s: %Q %Q %Q *OFF*\n", dtime(),
          me, old_victim, environment(old_victim)), 100000);
        VOICEMASTER->SendToChannel("Snoop", this_object(), SAY,
          sprintf("%s *OFF* %s (%Q).", me_name, vic_name,
          environment(old_victim)));
      }

      tell_object(old_victim, me_name + " beobachtet Dich nicht mehr.\n");
      txt = me_name + " beobachtet " + vic_name + " nicht mehr.\n";
      while (old_victim = find_snooped(old_victim))
        tell_object(old_victim, txt);
    }
    ret = efun::snoop(me);
    if (vic_name) tell_object(me, "*** Snoop auf " + vic_name + " beendet.\n");
    if (!victim) return ret;
  }

  // Start neuen Snoop
  vic_name = capitalize(getuid(victim));
  if (query_wiz_grp(me) <= query_wiz_grp(victim)
    && !victim->QueryAllowSnoop(me)) {
    tell_object(me, "Du kannst " + vic_name
      + " ohne Erlaubnis nicht snoopen.\n");
    tell_object(victim, "*** " + me_name + " versucht Dich zu snoopen.\n");
    return 0;
  }

  // Alten Snooper loswerden
  old_snooper = efun_query_snoop(victim);
  if (old_snooper) {
    if (!(me->QueryProp(P_SNOOPFLAGS) & SF_FORCE)) {
      tell_object(me, vic_name + " wird bereits von "
        + capitalize(getuid(old_snooper)) + " gesnoopt.\n");
      return 0;
    }
    if (query_wiz_grp(old_snooper) >= query_wiz_grp(me)
        && old_snooper->QueryProp(P_SNOOPFLAGS) & SF_LOCKED) {
      tell_object(me, "Du kannst " + vic_name + " nicht snoopen.\n");
      return 0;
    }

    efun::snoop(old_snooper);

    os_name = capitalize(getuid(old_snooper));
    tell_object(old_snooper, "*** " + me_name + " übernimmt den Snoop auf "
      + vic_name + ".\n");
    if (!IS_ARCH(old_snooper)
      || !(old_snooper->QueryProp(P_SNOOPFLAGS) & SF_SILENT)) {

      log_file(SNOOPLOGFILE, sprintf("%s: %Q %Q %Q *OFF*\n", dtime(),
        old_snooper, victim, environment(victim)), 100000);
      VOICEMASTER->SendToChannel("Snoop", this_object(), SAY,
        sprintf("%s *OFF* %s (%Q).", os_name, vic_name, environment(victim)));
    }

    tell_object(victim, os_name + " beobachtet Dich nicht mehr.\n");
    txt = os_name + " beobachtet " + vic_name + " nicht mehr.\n";

    old_victim = victim;
    while (old_victim = find_snooped(old_victim))
      tell_object(old_victim, txt);
  }

  // Jetzt neuen Snoop anlegen

  ret = efun::snoop(me, victim);
  if (ret < 1) {
    tell_object(me, "Fehler beim Snoopen aufgetreten.\n");
    return ret;
  }
  tell_object(me, "*** Snoop auf " + vic_name + " gestartet.\n");

  if (!IS_ARCH(me) || !(me->QueryProp(P_SNOOPFLAGS) & SF_SILENT)) {
    log_file(SNOOPLOGFILE, sprintf("%s: %Q %Q %Q\n", dtime(),
      me, victim, environment(victim)), 100000);
    VOICEMASTER->SendToChannel("Snoop", this_object(), SAY,
      sprintf("%s -> %s (%Q).", me_name, vic_name, environment(victim)));
  }
  slist = ({});
  old_victim = efun_query_snoop(me);
  if (old_victim) do {
    slist += ({ capitalize(getuid(old_victim)) });
  } while (old_victim = efun_query_snoop(old_victim));

  if (sizeof(slist))
    txt = " " + me_name+" wird wiederum von " + countup(slist)
      + " beobachtet.\n";
  else
    txt = "\n";

  tell_object(victim, "Du wirst jetzt von " + me_name + " beobachtet." + txt);
  txt = vic_name + " wird jetzt von " + me_name + " beobachtet." + txt;
  while (victim = find_snooped(victim))
    tell_object(victim, txt);

  if (old_snooper && (query_wiz_grp(me) < query_wiz_grp(old_snooper)
    || me->QueryAllowSnoop(old_snooper))) {
    snoop(old_snooper, me);
  }

  return ret;
}

nomask object query_snoop(object who) {
  object snooper;

  snooper=efun_query_snoop(who);
  if (!snooper) return 0;
  if (query_wiz_grp(snooper)>query_wiz_grp(getuid(previous_object())) &&
    IS_ARCH(snooper) && strstr(object_name(previous_object()), "/", 1)!=-1)
    return 0;
  return snooper;
}

// Flexiblere Kommandoverwaltung

string query_verb() {
  string verb;
  if (!this_player() || !(verb = this_player()->QueryCommand(1)))
    return efun::query_verb();
  return verb;
}

string query_command() {
  string cmd;
  if (!this_player() || !(cmd = this_player()->QueryCommand(0)))
    return efun::query_command();
  return cmd;
}

// * Emulation des 'alten' explode durch das neue

string *explode(string str, string del) {
  int s, t;
  string *strs;
  
  if(del == "")
    return ({str});
  strs=efun::explode(str, del);
  t=sizeof(strs)-1;
  while(s<=t && strs[s++] == "");s--;
  while(t>=0 && strs[t--] == "");t++;
  if(s<=t)
    return strs[s..t];
  return ({});
}

string *old_explode(string str, string del) {return explode(str,del);}

string *new_explode(string str, string del) {
  return efun::explode(str, del);
}

// * get_dir_ext: an directories einen /, an geladene Files einen * anhaengen

string *get_dir_ext(string path) {
  string *strs, file;
  mixed *finfo;
  int i, s, t;

  set_this_object(previous_object());
  if(s=sizeof(finfo=get_dir(path, GETDIR_NAMES|GETDIR_SIZES))) {
    strs=({string*})MASTER->full_path_array(path, TP ? getuid(TP) : 0);
    if(t=sizeof(strs))
      path="/"+(implode(strs[0..t-2], "/")+"/");
    else
      path="/";
    strs=({});
    while(s--,s-->0) {
      file=finfo[i++];
      switch(finfo[i++]) {
	case -2:
	file+="/";
	break;
	case -1:
	file+=" -?-";
	break;
	default:
	if(find_object(path+file))
	  file+="*";
      }
      strs+=({file});
    }
  }
  return strs;
}

// * Modifikations-Datum eines Files

int file_date(string file) {
  mixed *data;

  if(sizeof(data=get_dir(file, GETDIR_DATES)))
    return data[0];
  else
    return 0;
}

int file_time(string path)
{
  mixed *v;
  
  set_this_object(previous_object());
  if (sizeof(v=get_dir(path,4))) return v[0];
  return 0;
}

// * Bei 50k Groesse Log-File rotieren

varargs int log_file(string file, string txt, int size_to_break, int bakmode)
{
  string date,time;
  mixed *st;
  int i;

  file="/log/"+file;
  file=implode((efun::explode(file,"/")-({".."})),"/");

  if (!funcall(bind_lambda(#'efun::call_other,PO),"secure/master",//')
               "valid_write",file,geteuid(PO),"log_file",PO))
    return 0;

  if ( size_to_break >= 0 &&
      ( sizeof(st = efun::get_dir(file,2) ) &&
        st[0] >= (size_to_break || MAX_LOG_SIZE)) )
  {
    if (bakmode==1)
    {
      i=1;
      while (file_size(file+sprintf(".%03d",i)) != -1 && i<1000) i++;
      catch(rename(file, file+sprintf(".%03d",i)));
    }
    else if (bakmode==2)
    {
      date=ctime();
      time="."+date[11..12]+date[14..15];
      date="."+date[20..23]+( (["Jan":"01","Feb":"02","Mar":"03","Apr":"04",
            "May":"05","Jun":"06","Jul":"07","Aug":"08","Sep":"09",
            "Oct":"10","Nov":"11","Dec":"12"])[date[4..6]] || date[4..6] )+
           sprintf("%02d",to_int(date[8..9]));
      if (file_size(file+date) != -1)
        catch(rename(file, file+date+time));
      else
        catch(rename(file, file+date));
    }
    else
      catch(rename(file, file+".old"));
  }
  return write_file(file,txt);
}


// Fuer this_player() definierte Kommandos anzeigen

void localcmd() {
  string *verbs;
  int i, s;

  if(!TP) return;
  s=sizeof(verbs=query_actions(TP));
  write("Local commands:\n");
  while(s--)
    write("'"+verbs[i++]+"' ");
  write("\n");
}

// * Magier-Level abfragen

int query_wiz_level(mixed player) {
  return ({int})__MASTER_OBJECT__->query_wiz_level(player);
}

// * Teile aus einem Array entfernen *** OBSOLETE

mixed *exclude_array(mixed *arr,int from,int to)
{
  if (to<from)
    to = from;
  return arr[0..from-1]+arr[to+1..];
}

// * creator *** OBSOLETE

string creator(object ob)
{
  return ({string})__MASTER_OBJECT__->creator_file(ob);
  //return (string)"/secure/master"->creator_file(ob)+":"+getuid(ob);
}

// *** Notloesung fuer Sommerzeitkorrektur

private nosave int correktur;

void create() {
  string str;
  if (file_size("/etc/SOMMER")<=0) return;
  str=read_file("/etc/SOMMER");
  correktur=to_int(str);
}

varargs string ctime(int clock) {
  if (!clock) return efun::ctime(efun::time()+3600*correktur);
  return efun::ctime(clock);
}

int time() {
  return efun::time()+3600*correktur;
}

// * German version of ctime()
private nosave string* d_mon = ({ "Jan", "Feb", "Mar", "Apr", "Mai", "Jun",
                                  "Jul", "Aug" ,"Sep", "Okt", "Nov", "Dez" });
private nosave string* d_day = ({ "Son", "Mon", "Die", "Mit", "Don", "Fre",
                                  "Sam", "Son" });

varargs string dtime(varargs mixed* args) {
  int* lt;

  lt = apply(#'localtime, args);

  return sprintf("%s, %2d. %s %d, %02d:%02d:%02d",
    d_day[lt[TM_WDAY]], lt[TM_MDAY], d_mon[lt[TM_MON]], lt[TM_YEAR],
    lt[TM_HOUR], lt[TM_MIN], lt[TM_SEC]);
}

// * Shutdown mit zusaetzlichem logging

nomask int shutdown(string reason)
{
  string name;
  string obname;
  string output;

  if (!reason)
    return 0;
  if ((!this_interactive()||!IS_ARCH(this_interactive())) && 
       getuid(previous_object())!=ROOTID &&
	   object_name(previous_object())!="/obj/shut")
  {
    write("You have no permission to shut down the gamedriver!\n");
    return 0;
  }
  if ((this_interactive())&&(name=getuid(this_interactive())))
  {
    name=capitalize(name);
    filter(users(),#'tell_object,//'
		 capitalize(name)+" fährt das Spiel herunter!\n");
  }
  else
    name="ANONYMOUS";
  if (previous_object()) obname=capitalize(getuid(previous_object()));
  output=name;
  if (obname && name!=obname) output=output+" ("+obname+")";
  if (previous_object()&&object_name(previous_object())=="/obj/shut"){
    output+=" fährt das Spiel via Armageddon herunter.\n";
    output=dtime(time())+": "+output;
    log_file("GAME_LOG",output+"\n",-1);
    efun::shutdown();
    return 1;
  }
  output=ctime(time())+": "+output+" fährt das Spiel herunter.\n";
  output+="    Grund: "+reason;
  log_file("GAME_LOG",output+"\n",-1);
  efun::shutdown();
  return 1;
}

// * Interactive-Struct uebertragen - zus. checks

// OBSOLETE!
status is_clone(mixed ob) {
  return clonep(ob);
}

// * GD version

string version()
{
  return __VERSION__;
}

// * break_string

static int width,gCol,lfsig;
static string indstr;

#define __LFS "                                                          "
#define LFS (__LFS __LFS)

private string bnlfs(string word)  {
  int zz;

  zz=lfsig;
  lfsig=0;
  if ((gCol+strlen(word))>width) {
    gCol=strlen(indstr)+strlen(word)+1;
    return "\n"+indstr+word;
  }
  else  {
    gCol+=strlen(word)+1;
    return (zz ? "" : " ")+word;
  }
}

// Altes break_string() nur noch intern fuer BS_BLOCK
private varargs string
_break_string(string str, int _width, mixed indent, int flags) {
  string *tmp,*a,*b;
  int i,j,k,l;

  if (!str || str=="")
    return str;             /* Ein Scherzbold war am Werk... */

  if (flags&BS_LEAVE_MY_LFS)
  {
    flags&=~BS_LEAVE_MY_LFS;
    a=efun::explode(str,"\n");
    i = sizeof(a);
    if (a[<1]=="" && i>1)
    {
      a=a[0..<2];
      i--;
    }
    for (i--;i>=0;i--)
    {
      a[i]=_break_string(a[i],_width,indent,flags);
      if (a[i]=="") a[i]="\n";
    }
    return implode(a,"");
  }
  width=_width;
  lfsig=2;
  if (indent)  {
    if (intp(indent))       /* Soviel spaces sollten reichen! */
      indstr=LFS[1..indent];
    else if (stringp(indent))  {
      indstr=indent;
      indent=strlen(indstr);
    }
    else  {          /* Sicher ist sicher. Koennte ja auch ein obj sein */
      log_file(explode(object_name(previous_object()), "/")[1]+".rep", 
	       "BUG GAME: Illegal argument indent to break_string in file "+
	       object_name(previous_object())+"\n");
      return "";
    }
  }
  else
    indstr="";
  gCol=indent;
  for (i=strlen(str)-1;i>=0;i--)
    if (str[i]=='\n') str[i]=' ';
  while (str!="" && str[<1]==32)
    str=str[0..<2];
  if ((flags & BS_BLOCK) && !(flags & BS_NO_PARINDENT))
    if (strlen(str))
      if (str[0]==' ')
	str[0]='x';
      else
	str="x"+str;
  if (sizeof(tmp=map(efun::explode(str, " "),#'bnlfs))) //#')))
    tmp[0]=indstr+tmp[0];
	str=implode(efun::explode(implode(tmp,""),"\n")-({indstr}),"\n")+"\n";
  if (!(flags&BS_BLOCK))
    return str;
  if (!(flags & BS_NO_PARINDENT))
    str=str[1..];
  a=efun::explode(str,"\n"); // Einzelne Zeilen
  if (strlen(a[0])<_width && a[0][0..0]!=" " &&
      !(flags&BS_NO_PARINDENT))
    a[0]=" "+a[0];
  for (i=sizeof(a)-2;i>=0;i--) // der Reihe nach bearbeiten
  {
    l=strlen(a[i]);           // Laenge der akt. Zeile
    if (l<_width && member(a[i][1..],' ')>=0) // Kein Leerzeichen? Nichts tun
    {
      b=efun::explode(a[i]," "); // Fragmente erzeugen
      for (j=sizeof(b)-1;j>0;j--)
        if (l<_width && ((b[j]!="" && member(",.-()[]{}!?",b[j][0])>=0) || 
		    (j>0&&b[j-1]!="" &&
		     member(",.-()[]{}!?",b[j-1][<1])>=0)))
	  b[j]=" "+b[j],l++;
      if (l<_width)
        for (j=sizeof(b)-1;j>0;j--)
          if (l<_width &&
              ((b[j]=="" || b[j][0]!=' ') && (b[j-1]=="" || b[j-1][<1]!=' ')))
            b[j]=" "+b[j], l++;
      while (l<_width)
	for (j=sizeof(b)-1;j>0;j--)
	{
	  if (l<_width)
	    b[j]=" "+b[j], l++;
	  if (l<_width) 
	    b[sizeof(b)-j]=" "+b[sizeof(b)-j], l++;
	} 
      a[i]=implode(b," ");
    }
  }
  return implode(a,"\n")+"\n";
}

static mapping bscolors;

// * break_string
// stretch() -- stretch a line to fill a given width 
private string stretch(string s, int width) {
  int len=sizeof(s);
  if (len==width) return s;

  // reine Leerzeilen, direkt zurueckgeben
  string trimmed=trim(s,TRIM_LEFT," ");
  if (trimmed=="") return s; 
  int start_spaces = len - sizeof(trimmed);

  string* words = explode(trimmed, " ");
  // der letzte kriegt keine Spaces
  int word_count=sizeof(words) - 1;
  // wenn Zeile nur aus einem Wort, wird das Wort zurueckgegeben
  if (!word_count)
    return " "*start_spaces + words[0];

  int space_count = width - len;

  int space_per_word=(word_count+space_count) / word_count;
  // Anz.Woerter mit Zusatz-Space
  int rest=(word_count+space_count) % word_count; 
  // Rest-Spaces Verteilen
  foreach (int pos : rest) words[pos]+=" ";
  return (" "*start_spaces) + implode( words, " "*space_per_word );
}

// aus Geschwindigkeitsgruenden hat der Blocksatz fuer break_string eine
// eigene Funktion bekommen:
private varargs string block_string(string s, int width, int flags) {
  // wenn BS_LEAVE_MY_LFS, aber kein BS_NO_PARINDENT, dann werden Zeilen mit
  // einem Leerzeichen begonnen.
  // BTW: Wenn !BS_LEAVE_MY_LFS, hat der Aufrufer bereits alle \n durch " "
  // ersetzt.
  if ( (flags & BS_LEAVE_MY_LFS)
      && !(flags & BS_NO_PARINDENT))
  {
      s = " "+regreplace(s,"\n","\n ",1);
  }

  // sprintf fuellt die letzte Zeile auf die Feldbreite (hier also
  // Zeilenbreite) mit Fuellzeichen auf, wenn sie NICHT mit \n umgebrochen
  // ist. Es wird an die letzte Zeile aber kein Zeilenumbruch angehaengt.
  // Eigentlich ist das Auffuellen doof, aber vermutlich ist es unnoetig, es
  // wieder rueckgaengig zu machen.
  s = sprintf( "%-*=s", width, s);

  string *tmp=explode(s, "\n");
  // Nur wenn s mehrzeilig ist, Blocksatz draus machen. Die letzte Zeile wird
  // natuerlich nicht gedehnt. Sie ist dafuer schon von sprintf() aufgefuellt
  // worden. BTW: Die letzte Zeile endet u.U. noch nicht mit einem \n (bzw.
  // nur dann, wenn BS_LEAVE_MY_LFS und der uebergebene Text schon nen \n am
  // Ende der letzten Zeile hat), das macht der Aufrufer...
  if (sizeof(tmp) > 1)
    return implode( map( tmp[0..<2], #'stretch/*'*/, width ), "\n" ) 
      + "\n" + tmp[<1];

  return s;
}

public varargs string break_string(string s, int w, mixed indent, int flags)
{
    if ( !s || s == "" ) return "";

    if ( !w ) w=78;

    if( intp(indent) )
       indent = indent ? " "*indent : "";

    int indentlen=stringp(indent) ? sizeof(indent) : 0;

    if (indentlen>w) {
      set_this_object(previous_object());
      raise_error(sprintf("break_string: indent longer %d than width %d\n",
                  indentlen,w));
      // w=((indentlen/w)+1)*w;
    }

    if (!(flags & BS_LEAVE_MY_LFS)) 
      s=regreplace( s, "\n", " ", 1 );

    if ( flags & BS_SINGLE_SPACE )
       s = regreplace( s, "(^|\n| )  *", "\\1", 1 );
 
    string prefix="";
    if (indentlen && flags & BS_PREPEND_INDENT) {
      if (indentlen+sizeof(s) > w || 
         (flags & BS_LEAVE_MY_LFS) && strstr(s,"\n")>-1) {
       prefix=indent+"\n";
       indent=(flags & BS_NO_PARINDENT) ? "" : " ";
       indentlen=sizeof(indent);
      }
    }

    if ( flags & BS_BLOCK ) {
      /*
           s = implode( map( explode( s, "\n" ),
                               #'block_string, w, indentlen, flags),
                      "" );
      */
      s = block_string( s , w - indentlen, flags );
    }
    else {
      s = sprintf("%-1.*=s",w-indentlen,s);
    }
    if ( s[<1] != '\n' ) s += "\n";

    if ( !indentlen ) return prefix + s;
    
    string indent2 = ( flags & BS_INDENT_ONCE ) ? (" "*indentlen) :indent;
      
    return prefix + indent + 
      regreplace( s[0..<2], "\n", "\n"+indent2, 1 ) + "\n";
      /*
       string *buf;

       buf = explode( s, "\n" );
       return prefix + indent + implode( buf[0..<2], "\n"+indent2 ) + buf[<1] + "\n";
      */
}

string ansi2pinkfish(string str) {
// color and text formatting
//
// ESC[#(;#)m
// there can be multiple text formatting sequence numbers separated
// by a ; and ending with an m where the # is of the following values:
//
// attributes                        foreground colors     background colors
//   0  normal display                30  black             40  black
//   1  bold                          31  red               41  red
//   4  underline (mono only)         32  green             42  green
//   5  blink on                      33  yellow            43  yellow
//   7  reverse video on              34  blue              44  blue
//   8  nondisplayed (invisible)      35  magenta           45  magenta
//  10  default/reset                 36  cyan              46  cyan
//                                    37  white             47  white
//                                    39  default           49  default
//
// (Taken from http://www.bluesock.org/~willg/dev/ansi.html and
//  http://www.chiark.greenend.org.uk/~bjharris/all-escapes/all-escapes.txt)
//
//
// Pinkfish colour codes:
//
// RED     BLUE     ORANGE     YELLOW     GREEN     BLACK     WHITE     CYAN
// MAGENTA B_RED    B_BLUE     B_ORANGE   B_YELLOW  B_GREEN   B_MAGENTA B_BLACK
// B_WHITE B_CYAN   FLASH      BOLD       REVERSE   UNDERLINE INITTERM  RESET
// WINDOW  ENDTERM  STATUS     EBOLD
//
// (Taken from http://mud.stack.nl/intermud/pinkfish.html)

  string *parts, *codes, fish;
  int colour, i, j, n, m, rs;

  parts = regexplode(str,
    "\e\\[([01457]|[34][0-9]|10)(;([01457]|[34][0-9]|10))*m");

  for (n = sizeof(parts), i = 1; i < n; i += 2) {
    codes = efun::explode(parts[i][2..<2], ";");
    fish = "";
    for (m = sizeof(codes), j = 0; j < m; ++j) {
      colour = to_int(codes[j]);
      switch (colour) {
        case  0:
        case 39:
        case 49: if (!rs) fish += "%^RESET%^"; // suppress multiple RESET
                 rs = 1;
                 break;
        case  1: fish += "%^BOLD%^";      rs = 0; break;
        case  4: fish += "%^UNDERLINE%^"; rs = 0; break;
        case  5: fish += "%^FLASH%^";     rs = 0; break;
        case  7: fish += "%^REVERSE%^";   rs = 0; break;
       
        case 40..47:
          fish += "%^B_";
          colour %= 10;
          // fall thru
        case 30..37:
          if (colour > 10) {
            fish += "%^";
            colour %= 10;
          }
          rs = 0;
          switch (colour) {
            case 0: fish += "BLACK%^";   break;
            case 1: fish += "RED%^";     break;
            case 2: fish += "GREEN%^";   break;
            case 3: fish += "ORANGE%^";  break; // no YELLOW possible
            case 4: fish += "BLUE%^";    break;
            case 5: fish += "MAGENTA%^"; break;
            case 6: fish += "CYAN%^";    break;
            case 7: fish += "WHITE%^";   break;
          }
      }
    } // for (codes)
    parts[i] = fish;
  } // for (parts)

  str = implode(parts, "");
  return str;
}


// * times
static int last_reboot;

int last_reboot_time() {
  return last_reboot;
}

int first_boot_time() {
  return 766490400; // 16.4.1994
}

int exist_days() {
  return (((time()-first_boot_time())/8640)+5)/10;
}

// * uptime :)

string uptime() {
  int t;
  int tmp;
  string s;

  t=time()-last_reboot_time();
  s="";
  if (t>=86400)
    s+=sprintf("%d Tag%s, ",tmp=t/86400,(tmp==1?"":"e"));
  if (t>=3600)
    s+=sprintf("%d Stunde%s, ",tmp=(t=t%86400)/3600,(tmp==1?"":"n"));
  if (t>60)
    s+=sprintf("%d Minute%s und ",tmp=(t=t%3600)/60,(tmp==1?"":"n"));
  return s+sprintf("%d Sekunde%s",t=t%60,(t==1?"":"n"));
}

// * Was tun bei 'dangling' lfun-closures ?

void dangling_lfun_closure() {
  raise_error("dangling lfun closure\n");
}

// * Sperren ausser fuer master/simul_efun

// nomask void set_environment(object o1, object o2) {raise_error("@Available only for root\n");} 
// nomask void set_this_player() {raise_error("@Available only for root\n");}

// * Bisschen sperren
nomask varargs int set_next_reset(int x) {
  int alt;
  set_this_object(previous_object());
  alt = efun::set_next_reset(0);
  if (x >= 1800 || x < 1 || x > alt) return efun::set_next_reset(x);
  return alt;
}

nomask string process_string(mixed str)
{
// Serii
int lv;
string *inds,d1,d2,fg,bg;
// iireS
  set_this_object(previous_object());
  if (closurep(str))
    return funcall(str);
  if (!stringp(str))
    return str;
  str=efun::process_string(str);
// Serii
  if(sizeof(regexplode(str,"\\^[RGBYWPCIFNDSAL<]"))>1)
  {
    inds=m_indices(COLS);
    for(lv=0;lv<sizeof(COLS);lv++)
      str=efun::implode(efun::explode(str,inds[lv]),COLS[inds[lv]]);
    while(sscanf(str,"%s^<%s,%s>%s",d1,fg,bg,d2))
      str=efun::implode(efun::explode(str,sprintf("^<%s,%s>",fg,bg)),
                                          sprintf("\e[%s;%sm",fg,bg));
  }
// iireS
return str;
}

/// * Properties ggfs. mitspeichern

void save_object(string name)
{
  mapping properties;
  mapping save;
  mixed index;
  int i;

  save = m_allocate(0, 2);
  properties = previous_object()->QueryProperties();

  if(mappingp(properties))
  {
    // delete all entries in mapping properties without SAVE flag!
    index = m_indices(properties);
    for(i = sizeof(index)-1; i>=0;i--)
    {
      if(properties[index[i], F_MODE] & SAVE)
      {
        save[index[i]] = properties[index[i]];
        save[index[i], F_MODE] = 
          properties[index[i], F_MODE] & 
            (~(SETMNOTFOUND|QUERYMNOTFOUND|QUERYCACHED|SETCACHED));
      }
    }
  }
  else save = ([]);
  
  // save object!
  previous_object()->_set_save_data(save);
  funcall(bind_lambda(#'efun::save_object, previous_object()), name);
  previous_object()->_set_save_data(0);
}

// * Auch Properties laden

int restore_object(string name)
{
  int result;
  mixed index;
  mapping save;
  mapping properties;
  int i;
  closure cl;
  
  // get actual property settings (by create())
  properties = previous_object()->QueryProperties();

  // restore object
  result=funcall(bind_lambda(#'efun::restore_object, previous_object()), name);
  save = previous_object()->_get_save_data();
  if(mappingp(save))
  {
    index = m_indices(save);
    for(i = sizeof(index)-1; i>=0; i--)
    {
      properties[index[i]] = save[index[i]];
      properties[index[i], F_MODE] = save[index[i], F_MODE]
	&~(SETCACHED|QUERYCACHED);
    }
  }
  else properties = ([]);
  
  // restore properties
  cl=unbound_lambda(({'arg}),
	                  ({#'call_other,({#'this_object}), //'
			    "SetProperties",'arg})); //'})}))
  funcall(
		bind_lambda(
			    unbound_lambda(({'arg}), //'})
					   ({#'call_other,({#'this_object}),
					   "SetProperties",'arg})),//')
			    previous_object()),properties);
  previous_object()->_set_save_data(0);
  return result;
}

// * HB eines Objektes ein/ausschalten

int set_object_heart_beat(object ob, int flag)
{
  if (objectp(ob))
    return funcall(bind_lambda(#'set_heart_beat,ob),flag);
  return 0;
}

// * Magierlevelgruppen ermitteln

int query_wiz_grp(mixed wiz)
{
  int lev;

  lev=query_wiz_level(wiz);
  if (lev<SEER_LVL) return 0;
  if (lev>=GOD_LVL) return lev;
  if (lev>=ARCH_LVL) return ARCH_GRP;
  if (lev>=ELDER_LVL) return ELDER_GRP;
  if (lev>=LORD_LVL) return LORD_GRP;
  if (lev>=SPECIAL_LVL) return SPECIAL_GRP;
  if (lev>=DOMAINMEMBER_LVL) return DOMAINMEMBER_GRP;
  if (lev>=WIZARD_LVL) return WIZARD_GRP;
  if (lev>=LEARNER_LVL) return LEARNER_GRP;
  return SEER_LVL;
}

// * wizlist ausgeben

varargs void wizlist(string name) {
  int i, pos, total_cmd;
  int *cmds;
  mixed *a;
  mixed *b;

  if (!name) {
    if (this_player())
			name = getuid(this_player());
    if (!name)
      return;
  }
  a = transpose_array(wizlist_info());
  cmds = a[WL_COMMANDS];
  a[WL_COMMANDS] = a[0];
  a[0] = cmds;
  a = order_alist(a);
  cmds = a[0];
  a[0] = a[WL_COMMANDS];
  a[WL_COMMANDS] = cmds;
  if ((pos = member(a[WL_NAME], name)) < 0 && name != "ALL")
    return;
  b = allocate(sizeof(cmds));
  for (i = sizeof(cmds); i;) {
    b[<i] = i;
    total_cmd += cmds[--i];
  }
  a = transpose_array(a + ({b}) );
  if (name != "ALL") {
    if (pos + 18 < sizeof(cmds)) {
      a = a[pos-2..pos+2]+a[<15..];
    } else if (pos < sizeof(cmds) - 13) {
      a = a[pos-2..];
    } else {
      a = a[<15..];
    }
  }
  write("\nWizard top score list\n\n");
  if (total_cmd == 0)
    total_cmd = 1;
  for (i = sizeof(a); i; ) {
    b = a[<i--];
    printf("%-15s %5d %2d%% (%d)\t[%4dk,%5d] %6d %d\n",
	   b[WL_NAME], b[WL_COMMANDS], b[WL_COMMANDS] * 100 / total_cmd, b[<1],
	   b[WL_COST] / 1000,
	   b[WL_HEART_BEATS], b[WL_EXTRA], b[WL_ARRAY_TOTAL]
	   );
  }
  printf("\nTotal         %7d     (%d)\n\n", total_cmd, sizeof(cmds));
}

#ifndef TESTMUD
varargs void call_out(mixed what, int delay, varargs mixed* args) {
  mixed tmp;
  mixed *call_outs;
  int i;
  
  call_outs=call_out_info();
  i=sizeof(call_outs);
  if (geteuid(previous_object())!=ROOTID && i+1>600) {
    for (tmp=0;i--;) {
      if (call_outs[i][0]==previous_object() && call_outs[i][1]==what)
        tmp++;
    }
    if (tmp>=30) {
      VOICEMASTER->SendToChannel("Debug",this_object(),SAY,
        sprintf("CALL_OUT overflow by object %Q (%Q)\n",previous_object(),what));
      destruct(previous_object());
      return;
    }
  }
  tmp=this_object();
  set_this_object(previous_object());
  apply(#'efun::call_out, what ,delay, args);
  set_this_object(tmp);
  return;
}
#endif

// * Zu einer closure das Objekt, an das sie gebunden ist, suchen

mixed query_closure_object(closure c) {
  return
    CLOSURE_IS_UNBOUND_LAMBDA(get_type_info(c, 1)) ?
      0 :
  (to_object(c) || -1);
}

// * Wir wollen nur EIN Argument ... ausserdem checks fuer den Netztotenraum

varargs void move_object(mixed what, mixed where) {
  object po,*oldenv,*newenv;
  
  po=previous_object();
  if (!where) {
    where=what;
    what=po;
  }
  if (((stringp(where) && where==NETDEAD_ROOM ) || 
       (objectp(where) && where==find_object(NETDEAD_ROOM))) && 
       objectp(what)) {
    if (!query_once_interactive(what)) {
      what->remove();
      if (what) destruct(what);
      return;
    }
    if (living(what) || interactive(what)) {
      log_file("NDEAD2",sprintf("TRYED TO MOVE TO NETDEAD: %Q\n",what));
      return;
    }
    set_object_heart_beat(what,0);
  }
  oldenv=all_environment(what)||({});
  funcall(bind_lambda(#'efun::move_object,po),what,where);
  newenv=(what && all_environment(what))||({});
  map_objects((oldenv-newenv)+newenv, "ContentChanged");
}

void move_to_noenv() {
  set_environment(previous_object(), 0);
}

// * additional message handling

private int living_or_interactive(object o)
{
  return (living(o) || query_once_interactive(o));
}

void shout(string s) {
  filter(users(), lambda(({'u}),({#'&&,
				 ({#'environment, 'u}),
				 ({#'!=, 'u, ({#'this_player})}),
				   ({#'tell_object, 'u, s})
				   })));
}
				
mapping living_name_m, name_living_m, netdead;

// Wizinfo Bearbeitung

int ___query_actual_wizinfo_size() {
  mixed info;
  info=get_extra_wizinfo(0);
  if(pointerp(info))
    return sizeof(info);
  return -1;
}

void start_simul_efun() {
  mixed *info;
 
  if ( !(info = get_extra_wizinfo(0)) )
    set_extra_wizinfo(0, info = allocate(BACKBONE_WIZINFO_SIZE));
  if (!(living_name_m = info[LIVING_NAME]))
    living_name_m = info[LIVING_NAME] = m_allocate(0, 1);
  if (!(name_living_m = info[NAME_LIVING]))
    name_living_m = info[NAME_LIVING] = m_allocate(0, 1);
  if (!(last_reboot = info[LAST_REBOOT]))
    last_reboot = info[LAST_REBOOT] = time();
  if (!(netdead = info[NETDEAD_MAP]))
    netdead = info[NETDEAD_MAP] = ([]);
  if (!(ip_map = info[IP_NAMES]))
    ip_map = info[IP_NAMES] = m_allocate(0,2);
  if (find_call_out("clean_simul_efun") < 0)
    efun::call_out("clean_simul_efun", 10);
}

// * regelmaessig Listen von Ballast befreien

#define clean_log(s)
// #define clean_log(s) log_file("CLEAN_SIM",ctime(time())[4..18]+": "+(s));

static void clean_name_living_m(string *keys, int left, int num) 
{
  int i, j;
  mixed a;
  
  for (; left && num; num--) {
    left -= 1;
    a = name_living_m[keys[left]];
    if (pointerp(a) && member(a, 0) >= 0) {
      a -= ({ 0 });
      i = sizeof(a);
      if (i == 1) a = a[0];
    } else i = !!a;
    if (i > 0) {
      clean_log("Keeping "+keys[left]+" ("+
        funcall(#'mixed_to_string, a)+")\n");
      name_living_m[keys[left]] = a;
    } else {
      clean_log("Toasting "+keys[left]+"\n");
      m_delete(name_living_m, keys[left]);
    }
  }
  if (left)
    efun::call_out("clean_name_living_m", 1, keys, left, 80);
  else
    clean_log("Clean process finished\n");
}

static void clean_netdead() {
  int i;
  string *s;
  object ob;
  
  s=m_indices(netdead);
  for (i=sizeof(s)-1;i>=0;i--)
    if (!objectp(ob=netdead[s[i]]) || interactive(ob))
      m_delete(netdead,s[i]);
}

static void clean_simul_efun() {
  sizeof(living_name_m); // Zerstoerte Objekte entfernen
  while (remove_call_out("clean_simul_efun") >= 0);
  if (find_call_out("clean_name_living_m") < 0) {
    clean_log("Starting clean process\n");
    efun::call_out("clean_name_living_m", 1,
      m_indices(name_living_m), sizeof(name_living_m), 80);
  }
  efun::call_out("clean_simul_efun", 7200);
  efun::call_out("clean_netdead",2);
}

void set_object_living_name(string name, object obj) {
  string old;
  mixed a;
  int i;
 
  if (previous_object() == obj
      || previous_object() == this_object()
      || object_name(previous_object()) == __MASTER_OBJECT__) {
    // Zerstoerte Objekte entfernen (driver quirk)
    sizeof(living_name_m);

    // ggf alte Eintraege loeschen
    if (old = living_name_m[obj]) {
      a = name_living_m[old];
      if (pointerp(a)) {
        a -= ({ obj, 0 });
        i = sizeof(a);
        if (i > 1) name_living_m[old] = a;
        else if (i == 1) name_living_m[old] = a[0];
        else m_delete(name_living_m, old);
      } else if (a == obj) {
	m_delete(name_living_m, old);
      }
    }
    
    living_name_m[obj] = name;
    if (a = name_living_m[name]) {
      if (!pointerp(a)) {
	name_living_m[name] = ({a, obj});
	return;
      }
      a += ({ obj });
      a -= ({ 0 });
      name_living_m[name] = a;
      return;
    }
    name_living_m[name] = obj;
  }
}

void set_living_name(string name) {
  set_object_living_name(name, previous_object());
}

void remove_living_name() {
  string name;
  mixed a;
  int i;

  if (!previous_object()) return;

  name = living_name_m[previous_object()];
  if (name) {
    m_delete(living_name_m, previous_object());
    a = name_living_m[name];
    if (pointerp(a)) {
      a -= ({ previous_object(), 0 });
      i = sizeof(a);
      if (i > 1) name_living_m[name] = a;
      else if (i == 1) name_living_m[name] = a[0];
      else m_delete(name_living_m, name);
    } else if (previous_object() == a) {
      m_delete(name_living_m, name);
    }
  }
}

void _set_netdead()
{
  if (query_once_interactive(previous_object()))
    netdead[getuid(previous_object())]=previous_object();
}

void _remove_netdead()
{
  m_delete(netdead,getuid(previous_object()));
}

object find_netdead(string name)
{
  return netdead[name];
}

string *dump_netdead()
{
  return m_indices(netdead);
}

object find_living(string name) {
  mixed *a, r;
  int i;
  
  if (pointerp(r = name_living_m[name])) {
    if (member(r,0)>=0)
      r-=({0});
    if (!sizeof(r)){
      m_delete(name_living_m,name);
      clean_log(sprintf("find_living löscht %s\n",name));
      return 0;
    }
    if ( !living(r = (a = r)[0])) {
      for (i = sizeof(a); --i;) {
	if (living(a[<i])) {
	  r = a[<i];
	  a[<i] = a[0];
	  return a[0] = r;
	}
      }
    }
    return r;
  }
  return living(r) && r;
}

object *find_livings(string name)
{
  mixed r;

  if (pointerp(r=name_living_m[name])) {
    r-=({0});
    if (!sizeof(r)) {
      m_delete(name_living_m, name);
      clean_log(sprintf("find_livings löscht %s\n", name));
      return 0;
    }
  } else {
    if (objectp(r))
      r=({r});
    else return 0;
  }
  return filter(r, #'living);
}

object* users() {
  return filter(efun::users(), #'living);
}

object find_player(string name) {
  mixed *a, r;
  int i;
  
  if (pointerp(r = name_living_m[name])) {
    if (member(r,0)>=0)
      r-=({0});  
    if (!sizeof(r)){
      m_delete(name_living_m,name);
      clean_log(sprintf("find_player löscht %s\n",name));
      return 0;
    }
    if ( !(r = (a = r)[0]) || !query_once_interactive(r)) {
      for (i = sizeof(a); --i;) {
        // sorgt dafuer, dass der Spieler immer ganz vorne im Array steht
	if (a[<i] && query_once_interactive(a[<i])) {
	  r = a[<i];
	  a[<i] = a[0];
	  return a[0] = r;
	}
      }
    }
    return r && query_once_interactive(r) && r;
  }
  return r && query_once_interactive(r) && r;
}

int player_exists(string who)
{
  string *ret;

  if (!stringp(who)) raise_error("Bad argument 1 to player_exists()\n");

  if (!strlen(who)) raise_error("Leerer String bei player_exists()\n");

  // (Holger) *g* is a little bit quicker than:
  // if (member(who, '.')>-1 && member(who. '/')>-1)
  if (strlen(who&"./"))
    raise_error("Illegales Argument 1 zu player_exists()\n");

  who = lower_case(who);

  ret = get_dir("/save/" + who[0..0] + "/" + who + ".o");
  
  if (!sizeof(ret)) return 0;
  
  return 1;
}

int check_match(string str)
{
  mixed *match;
  
  if (!(match=name_living_m[str]))
    return 0;
  if (!pointerp(match) || member(match,0)<0)
    return 1;
  if (sizeof(match))
    return 1;
  m_delete(name_living_m,str);
  clean_log(sprintf("check_match löscht %s\n",str));
  return 0;
}

varargs mixed match_living(string str, int flag, object room) {
  int i, d;
  string* user;
  object* hits, *tmp;
  
  if (!stringp(str) || str == "") return 0;
  if (flag & MATCH_PLAYER_FIRST) flag |= MATCH_PLAYER;

  if (room) {
    d = 0;
    hits = all_inventory(room);
    if (flag & MATCH_PLAYER) hits = filter(hits, #'interactive);
    else hits = filter(hits, #'living);
    if (flag & MATCH_NO_INVIS)
      hits = filter(hits, (: !$1->QueryProp(P_INVIS) :));

    user = map(hits, (: living_name_m[$1] :));
    for (i = sizeof(user); i--;) {
      if (!user[i] || strstr(user[i], str)) hits[i] = 0;
      if (user[i] == str) d++; // direkte Treffer
    }
    if (d == 1) {
      if (flag & MATCH_OBJS) return ({ hits[member(user, str)] });
      else return str;
    }
    hits -= ({ 0 });
    i = sizeof(hits);

    if (flag & MATCH_PLAYER_FIRST && !i) {
      // direkten Treffer auf NPC suchen
      hits = all_inventory(room);
      hits = filter(hits, (: living($1) && !interactive($1) :));
      if (flag & MATCH_NO_INVIS)
        hits = filter(hits, (: !$1->QueryProp(P_INVIS) :));
      hits = filter(hits, (: living_name_m[$1] == $2 :), str);
      i = sizeof(hits);
    }

    if (flag & MATCH_OBJS) return hits;
    if (i > 1) return -1;
    if (!i) return -2;
    return living_name_m[hits[0]];
  }

  if (hits = find_livings(str)) { // Direkten Treffer suchen
    if (flag & MATCH_PLAYER) hits = filter(hits, #'interactive);
    if (flag & MATCH_NO_INVIS)
      hits = filter(hits, (: !$1->QueryProp(P_INVIS) :));
    i = sizeof(hits);
    if (flag & MATCH_OBJS) {
      if (i == 1) return hits;
    } else if (i > 1) {
      if (flag & MATCH_NO_DOUBLES) return -1;
      return str;
    } else if (i == 1) return str;
  }

  // Nichts gefunden, dann jetzt Teilstring suchen
  if (flag & MATCH_PLAYER) user = map(users(), #'getuid);
  else user = m_indices(name_living_m);
  user = filter(user, (: !strstr($1, $2) :), str) - ({ 0 });
  i = sizeof(user);
  for (hits = ({}); i--;) {
    tmp = find_livings(user[i]);
    if (!tmp) { user[i] = 0; continue; }
    if (flag & MATCH_NO_INVIS) {
      tmp = filter(tmp, (: !$1->QueryProp(P_INVIS) :));
      if (!sizeof(tmp)) { user[i] = 0; continue; }
    }
    hits += tmp;
  }
  i = sizeof(user -= ({ 0 }));
  if (i == 0) {
    if (!(flag & MATCH_PLAYER_FIRST))
      return (flag & MATCH_OBJS ? ({}) : -2);
    hits = find_livings(str);
    if (!hits) return (flag & MATCH_OBJS ? ({}) : -2);
    hits = filter(hits, (: living($1) && !interactive($1) :));
    if (flag & MATCH_NO_INVIS)
      hits = filter(hits, (: !$1->QueryProp(P_INVIS) :));
    if (flag & MATCH_OBJS) return hits;
    user = ({ str });
  } else {
    if (i > 1 && !(flag & MATCH_OBJS))
      return -1; // mehr als ein Livingname angesprochen
    if (flag & MATCH_PLAYER)
      hits = filter(hits, #'interactive);
  }
  i = sizeof(hits);
  if (flag & MATCH_OBJS) return hits;
  if (i == 0) return -2;
  if (i > 1 && flag & MATCH_NO_DOUBLES)
    return -1; // mehr als ein Living mit passendem Namen
  return user[0];
}

mixed *dump_livings()
{
  return sort_array(m_indices(name_living_m),#'>);
}

#if __EFUN_DEFINED__(send_erq)
void cindent(string name)
{
    string valid;

    valid = funcall(
      bind_lambda(#'call_other, previous_object()),
      __MASTER_OBJECT__,
      "valid_write",
      name,
      geteuid(previous_object()),
      "cindent", previous_object()
    );
    if (valid)
      send_erq(ERQ_FORK, "indent_wrapper " + (stringp(valid) ? valid : name));
}
#else
void cindent(string name)
{
  raise_error("Sorry, no CINDENT today.");
}
#endif

#if !__EFUN_DEFINED__(query_comm_stat)
mixed *query_comm_stat()
{
  return ({0,0});
}
#endif

#if !__EFUN_DEFINED__(absolute_hb_count)
int absolute_hb_count()
{
  return (time() - last_reboot_time()) / 2;
}
#endif

void __set_environment(object ob, mixed target)
{
  string path;
  object obj;

  if (!objectp(ob))
    return;
  // if (!IS_ARCH(geteuid(previous_object())) || !IS_ARCH(this_interactive()))
  if (!IS_ARCH(this_interactive()))
    return;
  if (objectp(target))
  {
    efun::set_environment(ob,target);
    return;
  }
  path=MASTER->_get_path(target,this_interactive());
  if (stringp(path) && file_size(path+".c")>=0 &&
      !catch(path->TU_GAR_NIX()))
  {
    obj=find_object(path);
    efun::set_environment(ob,obj);
    return;
  }
}

void _dump_wizlist(string file, int sortby) {
  int i;
  mixed *a;

  if (!(IS_LORD(this_interactive()) || IS_LORD(previous_object())))
    return;
  if (!__MASTER_OBJECT__->valid_write(file,geteuid(previous_object()),"write_file"))
  {
    write("NO WRITE PERMISSION\n");
    return;
  }
  a = wizlist_info();
  a = sort_array(a, lambda( ({'a,'b}), 
			   ({#'<, 
                             ({#'[,'a,sortby}),
                             ({#'[,'b,sortby})
                            })));
  rm(file);
  for (i=sizeof(a)-1;i>=0;i--)
    write_file(file,sprintf("%-11s: eval=%-8d cmds=%-6d HBs=%-5d array=%-5d mapping=%-7d\n",a[i][WL_NAME],a[i][WL_COST],a[i][WL_COMMANDS],a[i][WL_HEART_BEATS],a[i][WL_ARRAY_TOTAL],a[i][WL_CALL_OUT]));
}

varargs object present_living(mixed what, object where, int flag) {
  object* inv;
  int i, j;
  if (!objectp(where)) {
    where=previous_object();
    inv=all_inventory(where);
    where=environment(where);
    if (where) inv+=all_inventory(where);
  } else inv=all_inventory(where);
  if (flag) inv=filter(inv, #'interactive);
  else inv=filter(inv, #'living);
  if (objectp(what)) {
    if (member(inv, what)) return what;
    return 0;
  }
  for (i=sizeof(inv); j<i; j++) if (inv[j]->id(what)) return inv[j];
  return 0;
}

varargs object deep_present(mixed what, object ob) {
  object *inv;
  int cnt,i,size;
  string newwhat;
  
  if(!ob || !objectp(ob))
    ob=previous_object();
  if(objectp(what))
  {        
    for(i=sizeof(inv=deep_inventory(ob))-1;i>=0;i--)
      if(inv[i]==what)
	return what;
  }
  else
  {    
    if(stringp(what))
    {
      if(sscanf(what,"%s %d",newwhat,cnt)!=2)
        cnt=1;
      else
	what=newwhat;
      for(i=0,size=sizeof(inv=deep_inventory(ob));i<size;i++)
      {
	if(inv[i]->id(what))
	  if(!--cnt)
	    return inv[i];
      }
    }
  }
  return 0;
}

// Ich bin klein, mein Herz ist rein, ...
varargs object* present_plural(string was, object wo) {
  return
    filter_objects(all_inventory(wo || previous_object()), "id_plural", was);
}

int binary_message(mixed *message, int flags)
{
  return 0;
}

nomask void swap(object obj)
{
  write("Your are not allowed to swap objects by hand!\n");
  return;
}

nomask void garbage_collection()
{
  write("Call GC now and the mud will crash in 5-6 hours. DONT DO IT!\n");
  return;
}

object shadow(object ob, int flag) {
  if (flag && objectp(ob))
    log_file(SHADOWLOGFILE, sprintf("%s: %Q -> %Q TI: %Q\n",
      dtime(time())[5..], ob, previous_object(), this_interactive()));
#if __EFUN_DEFINED__(query_shadowing)
  set_this_object(previous_object());
#endif
  return efun_shadow(ob, flag);
}

// Da wollte sich jemand einen Spass machen...
void remove_interactive(object ob) {
  if(ob && query_once_interactive(ob) && this_interactive() &&
    ob!=this_interactive() && getuid(this_interactive())!=ROOTID)
      log_file(RMILOGFILE,
       sprintf("%s: %s removed from: %Q\n",
        dtime(time())[5..], capitalize(getuid(ob)), this_interactive()));
  efun::remove_interactive(ob);
}

string time2string( string format, int time )
{
  int days,hours,mins,secs,i,ch,max,abbr,dummy;
  string *parts, fmt;

  secs = time;
  mins = (time/60);
  hours = (time/3600);
  days = (time/86400);
  abbr = 0;

  parts = regexplode( format, "\(%\(-|\)[0-9]*[dhmsxDHMSX]\)|\(%%\)" );

  max = 0;
  for( i=1; i<sizeof(parts); i+=2 )
  {
    ch = parts[i][<1];
    switch( parts[i][<1] )
    {
    case 'x': case 'X':
        abbr = sscanf( parts[i], "%%%d", dummy ) && dummy==0;
        // NO break !
    case 'd': case 'D':
        max |= 7;
        break;
    case 'h': case 'H':
        max |= 3;
        break;
    case 'm': case 'M':
        max |= 1;
        break;
    }
  }
  if( max & 4 ) hours %= 24;
  if( max & 2 ) mins %= 60;
  if( max ) secs %= 60;

  for( i=1; i<sizeof(parts); i+=2 )
  {
    fmt = parts[i][0..<2];
    ch = parts[i][<1];
    if( ch=='x' )
    {
      if( days>0 ) ch='d';
      else if( hours>0 ) ch='h'; else ch = (mins>0) ? 'm' : 's';
    } else if( ch=='X' )
    {
      if( days>0 ) ch='D';
      else if( hours>0 ) ch='H'; else ch = (mins>0) ? 'M' : 'S';
    }
    switch( ch )
    {
      case 'd': parts[i] = sprintf( fmt+"d", days ); break;
      case 'h': parts[i] = sprintf( fmt+"d", hours ); break;
      case 'm': parts[i] = sprintf( fmt+"d", mins ); break;
      case 's': parts[i] = sprintf( fmt+"d", secs ); break;
      case 'D':
        if(abbr) parts[i] = "d"; else
        parts[i] = sprintf( fmt+"s", (days==1) ? "Tag" : "Tage" );
        break;
      case 'H':
        if(abbr) parts[i] = "h"; else
        parts[i] = sprintf( fmt+"s", (hours==1) ? "Stunde" : "Stunden" );
        break;
      case 'M':
        if(abbr) parts[i] = "m"; else
        parts[i] = sprintf( fmt+"s", (mins==1) ? "Minute" : "Minuten" );
        break;
      case 'S':
        if(abbr) parts[i] = "s"; else
        parts[i] = sprintf( fmt+"s", (secs==1) ? "Sekunde" : "Sekunden" );
        break;
      case '%':
        parts[i] = "%";
        break;
      }
    }
    return implode( parts, "" );
}

varargs string parse_mess(string str, object obj)
{
  int i;
  if (!stringp(str) || strstr(str,"&",0)<0) return str;

  if (!objectp(obj)) {
    obj = this_player();
    if (!objectp(obj)) return str;
  }

  if (strstr(str, "&&")>-1) {
    while ( (i=strstr(str, "&&WER&&")) > -1)
      str = str[0..i-1] + obj->name(WER,2) + str[i+7..];
    while ( (i=strstr(str, "&&WESSEN&&")) > -1)
      str = str[0..i-1] + obj->name(WESSEN,2) + str[i+10..];
    while ( (i=strstr(str, "&&WEM&&")) > -1)
      str = str[0..i-1] + obj->name(WEM,2) + str[i+7..];
    while ( (i=strstr(str, "&&WEN&&")) > -1)
      str = str[0..i-1] + obj->name(WEN,2) + str[i+7..];
    while ( (i=strstr(str, "&&")) > -1)
      str = str[0..i-1] + obj->name(WER,2) + str[i+2..];
  }
  if (strstr(str, "&!")>-1) {
    while ( (i=strstr(str, "&!WER!&")) > -1)
      str = str[0..i-1] + obj->QueryPronoun(WER) + str[i+7..];
    while ( (i=strstr(str, "&!WESSEN!&")) > -1)
      str = str[0..i-1] + obj->QueryPronoun(WESSEN) + str[i+10..];
    while ( (i=strstr(str, "&!WEM!&")) > -1)
      str = str[0..i-1] + obj->QueryPronoun(WEM) + str[i+7..];
    while ( (i=strstr(str, "&!WEN!&")) > -1)
      str = str[0..i-1] + obj->QueryPronoun(WEN) + str[i+7..];
    while ( (i=strstr(str, "&!")) > -1)
      str = str[0..i-1] + obj->QueryPronoun(WER) + str[i+2..];
  }
  if (strstr(str, "&%")>-1) {
    while ( (i=strstr(str, "&%WER%&")) > -1)
      str = str[0..i-1] + obj->QueryPossPronoun(MALE,WER) + str[i+7..];
    while ( (i=strstr(str, "&%WESSEN%&")) > -1)
      str = str[0..i-1] + obj->QueryPossPronoun(MALE,WESSEN) + str[i+10..];
    while ( (i=strstr(str, "&%WEM%&")) > -1)
      str = str[0..i-1] + obj->QueryPossPronoun(MALE,WEM) + str[i+7..];
    while ( (i=strstr(str, "&%WEN%&")) > -1)
      str = str[0..i-1] + obj->QueryPossPronoun(MALE,WEN) + str[i+7..];
    while ( (i=strstr(str, "&%FWER%&")) > -1)
      str = str[0..i-1] + obj->QueryPossPronoun(FEMALE,WER) + str[i+8..];
    while ( (i=strstr(str, "&%FWESSEN%&")) > -1)
      str = str[0..i-1] + obj->QueryPossPronoun(FEMALE,WESSEN) + str[i+11..];
    while ( (i=strstr(str, "&%FWEM%&")) > -1)
      str = str[0..i-1] + obj->QueryPossPronoun(FEMALE,WEM) + str[i+8..];
    while ( (i=strstr(str, "&%FWEN%&")) > -1)
      str = str[0..i-1] + obj->QueryPossPronoun(FEMALE,WEN) + str[i+8..];
    while ( (i=strstr(str, "&%NWER%&")) > -1)
      str = str[0..i-1] + obj->QueryPossPronoun(NEUTER,WER) + str[i+8..];
    while ( (i=strstr(str, "&%NWESSEN%&")) > -1)
      str = str[0..i-1] + obj->QueryPossPronoun(NEUTER,WESSEN) + str[i+11..];
    while ( (i=strstr(str, "&%NWEM%&")) > -1)
      str = str[0..i-1] + obj->QueryPossPronoun(NEUTER,WEM) + str[i+8..];
    while ( (i=strstr(str, "&%NWEN%&")) > -1)
      str = str[0..i-1] + obj->QueryPossPronoun(NEUTER,WEN) + str[i+8..];
    while ( (i=strstr(str, "&%")) > -1)
      str = str[0..i-1] + obj->QueryPossPronoun(MALE,WER) + str[i+2..];
  }
  return capitalize(str);
}


mixed summe(mixed *array)
{
  mixed sum;
  int i, size;

  if (!pointerp(array)) return array;
  if (!(size = sizeof(array))) return 0;
  for (sum = copy(array[0]), i = 1; i < size; sum += array[i++]);
  return sum;
}


// Diese Funktion wandelt ein Array of Strings in einen String um,
// wo die Elemente mit Kommata und das letzte mit 'und' voneinander
// getrennt sind.
varargs string countup(string *foo, mixed i)
{
  if(!pointerp(foo)) 
    raise_error("@Illegales Argument 1 zu countup()\n");
  switch (sizeof(foo))
  {
    case 0: return "";
    case 1: return foo[0];
  }
  if (i) {
    if (stringp(i))
      return implode(foo[0..<2], ", ")+" "+i+" "+foo[<1];
    return implode(foo[0..<2], ", ")+" oder "+foo[<1];
  }
  return implode(foo[0..<2], ", ")+" und "+foo[<1];
}

nomask varargs int command(string str, object ob) {
  if (objectp(ob) && query_once_interactive(ob) &&
    ob!=previous_object()) {
    raise_error("@command() to other interactive objects is not allowed!\n");
  }
  return funcall(bind_lambda(#'efun::command, previous_object()),
    str, (objectp(ob)?ob:previous_object()));
}

//////////////// Event sefuns ///////////////////


varargs int 
listen_event(mixed type, mixed prio, mixed callback)
{
	int* t;

	if (!callback) callback="receive_event";

	if (stringp(callback)) callback=funcall(bind_lambda(
		#'symbol_function,previous_object()),                             //'
		callback,previous_object());

	t = get_type_info(callback);
	if (t[0] != T_CLOSURE
		|| CLOSURE_IS_IDENTIFIER(t[1])
		|| CLOSURE_IS_UNBOUND_LAMBDA(t[1])
		|| CLOSURE_IS_OPERATOR(t[1]))
		raise_error("@Bad argument 3 to listen_event()\n");

	closure pocall = bind_lambda(#'call_other,previous_object());       //'

	// einfachen Typ in Array umwandeln
	if (stringp(type)) type = ({type});

	// einfache Prio in Array umwandeln
	if (intp(prio)) prio = ({prio});

	if (!pointerp(type) || !sizeof(type))
		raise_error("@Bad argument 1 to listen_event()\n");
	if (!pointerp(prio) || !sizeof(prio))
		raise_error("@Bad argument 2 to listen_event()\n");

	// Verschachtelte for () Indices:
	// ti = type-array-index		
	// tk = type-rollback-index		
	// pi = prio-array-index		
	// pk = prio-rollback-index		

	int ret = 0;

	// durch alle Typen loopen
	for (int ti=0;ti<sizeof(type);ti++)
	{
		// durch alle Prioritaeten loopen fuer diesen Typ
		for (int pi=0;pi<sizeof(prio);pi++)
		{
			ret = funcall(pocall, EVENTD, "listen", type[ti], prio[pi], callback);
			
			// Fehler aufgetreten	bei einer Prioritaet fuer diesen Typ
			if (ret!=1)
			{
				// Roll-Back aller bereits erfolgten Prio-Anmeldungen fuer diesen Typ
				for (int pk=pi-1;pk>=0;pk--)
				{
					funcall(pocall, EVENTD, "unlisten", type[ti], prio[pk], callback);
				}
				break;
			}
		}

		// Fehler aufgetreten	fuer diesen Typ
		if (ret!=1)
		{
			// Roll-Back aller bereits erfolgten Anmeldungen der anderen Typen
			for (int tk=ti-1;tk>=0;tk--)
			{
				for (int pk=sizeof(prio);pk--;)
				{
					funcall(pocall, EVENTD, "unlisten", type[tk], prio[pk], callback);
				}
			}
			break;
		}
	}

	return ret;
}

varargs int 
unlisten_event(mixed type, mixed prio, mixed callback)
{
	if (!callback) callback="receive_event";

	if (stringp(callback)) callback=funcall(bind_lambda(
		#'symbol_function,previous_object()),
		callback,previous_object());

	if (!closurep(callback)) return 0;

	closure pocall = bind_lambda(#'call_other,previous_object());       //'

	// einfachen Typ in Array umwandeln
	if (stringp(type)) type = ({type});

	// einfache Prio in Array umwandeln
	if (intp(prio)) prio = ({prio});

	if (!pointerp(type) || !sizeof(type))
		raise_error("@Bad argument 1 to unlisten_event()\n");
	if (!pointerp(prio) || !sizeof(prio))
		raise_error("@Bad argument 2 to unlisten_event()\n");

	// Verschachtelte for () Indices:
	// ti = type-array-index		
	// pi = prio-array-index		

	// durch alle Typen loopen
	for (int ti=0;ti<sizeof(type);ti++)
	{
		// durch alle Prioritaeten loopen fuer diesen Typ
		for (int pi=0;pi<sizeof(prio);pi++)
		{
			funcall(pocall, EVENTD, "unlisten", type[ti], prio[pi], callback);
		}
	}
	
	return 1;
}

void
set_global_listener(mixed type, status listening)
{
	closure pocall = bind_lambda(#'call_other,previous_object());       //'

	// einfachen Typ in Array umwandeln
	if (stringp(type)) type = ({type});

	if (!pointerp(type) || !sizeof(type))
		raise_error("@Bad argument 1 to set_global_listener()\n");

	// durch alle Typen loopen
	for (int ti=0;ti<sizeof(type);ti++)
	{
		funcall(pocall, EVENTD, "set_global", type[ti], listening);
	}
}

varargs mixed
send_event(string type, mapping data, mixed dest, int mode)
{
	dest=map((pointerp(dest)?dest:({dest})), lambda( ({'a}),
		({#'?, ({#'stringp, 'a}), ({#'load_object, 'a}), 'a }) ));
	return funcall(bind_lambda(#'call_other,previous_object()),
		EVENTD, "send", type, data, dest, mode);
}

varargs int
cancel_event(mixed info)
{
	return funcall(bind_lambda(#'call_other,previous_object()),
		EVENTD, "cancel_event", info);
}

varargs int
handle_event(mixed info)
{
	return funcall(bind_lambda(#'call_other,previous_object()),
		EVENTD, "handle_event", info);
}


varargs void input_to( mixed fun, int flags, varargs mixed *args )
{
    mixed *arr;
    int i;

    if ( !this_player() || !previous_object() )
       return;

    // TODO: input_to(...,INPUT_PROMPT, "", ...), wenn kein INPUT_PROMPT
    // vorkommt...
    if ( flags&INPUT_PROMPT ) {    
        arr = ({ fun, flags }) + args;
    }
    else {
        // ggf. ein INPUT_PROMPT hinzufuegen und nen Leerstring als Prompt.
        flags |= INPUT_PROMPT;
        arr = ({ fun, flags, "" }) + args;
    }

    // Arrays gegen flatten quoten.
    for ( i = sizeof(arr) - 1; i > 1; i-- )
       if ( pointerp(arr[i]) )
           arr[i] = quote(arr[i]);

    apply( bind_lambda( unbound_lambda( ({}),
                                     ({ #'efun::input_to/*'*/ }) + arr ),
                       previous_object() ) );
}

private nosave string *einer_strings =
  ({"","ein","zwei","drei","vier","fünf","sechs","sieben","acht","neun"});
private nosave string *zehner_strings =
  ({"","zehn","zwanzig","dreißig","vierzig","fünfzig","sechzig","siebzig",
      "achtzig","neunzig"});

varargs string
number2string(int number, int maxnumber_to_convert, int minnumber_very_much, string very_much_text)
{
  int einer, zehner, hunderter, tausender;
  string ret;

  if (minnumber_very_much < 1 || minnumber_very_much > 1000000)
    minnumber_very_much = 1000000;

  if (number < 0) raise_error("@Negatives Argument 1 bei number2string()\n");

  if (number >= minnumber_very_much)
    return (very_much_text || "ganz viele");

  if (maxnumber_to_convert && number > maxnumber_to_convert)
    return to_string(number);

  ret = "";
  tausender = number / 1000;
  if (tausender > 0)
  {
    if (tausender == 1) ret = "ein";
    else ret = number2string(tausender, 0, 0, 0);
    ret += "tausend";
  }

  number %= 1000;
  hunderter = number / 100;
  if (hunderter)
  {
    ret += einer_strings[hunderter] + "hundert";
  }

  number %= 100;
  switch (number)
  {
    // unregelmaessige formen
    case 0:  if (ret == "") ret = "null"; break;
    case 1:  ret += "eins"; break;
    case 11: ret += "elf"; break;
    case 12: ret += "zwölf"; break;
    case 16: ret += "sechzehn"; break;
    case 17: ret += "siebzehn"; break;

    default:
      zehner = number / 10;
      einer = number % 10;

      if (einer)
      {
        ret += einer_strings[einer];
        if (zehner > 1) ret += "und";
      }
      if (zehner)
        ret += zehner_strings[zehner];
  }

  return ret;
}

string digraph_filter(string str, int no_lfs) {
  int i, j, c;
  for (i = strlen(str)-1; i >= 0; --i) {
    if (str[i] >= ' ' && str[i] <= 127) continue;
    if (str[i] == '\n') {
      if (no_lfs) str[i] = ' ';
      continue;
    }

    // Bissl komplizierte Berechnung um mit aelteren und neueren Drivern
    // klar zu kommen, alte hatten signed chars
    c = (str[i] + 256) % 256;

    // UTF-8 Subset (nur erkennen der Umlaute)
    if (i && (j = member(UTF_8_FROM, c)) >= 0 && (str[i-1]+256) % 256 == 195) {
      --i;
      str[i..i+1] = UTF_8_TO[j*2..j*2+1];
      if (!(str[i] & 32) && str[i+2] && !(str[i+2] & 32))
        str[i+1] = str[i+1] & ~32;
      continue;
    }

    // 8 Bit Zeichensatz
    if ((j = member(DIGRAPH_FROM, c)) >= 0) {
      str[i..i] = DIGRAPH_TO[j*2..j*2+1];
      if (!(str[i] & 32) && str[i+2] && !(str[i+2] & 32))
        str[i+1] = str[i+1] & ~32;
      continue;
    }

    // unbekannt
    str[i] = ' ';
  }
  return str;
}

string bigraph_filter(string str, int no_lfs) {
  return digraph_filter(str, no_lfs);
}

mapping mts_cache; // Fuer rekursive Arrays/Mappings

// Wenn HIER (also im simul_efun) benutzt immer als closure verwenden!!
varargs string mixed_to_string(mixed mix, int lvl) {
  int i, j, s, t;
  mixed ret, *keys;

  if (extern_call()) {
    mts_cache = m_allocate(1, 0);
    if (lvl <= 0) lvl = (~0)>>>1; // Naja, ganz gross eben
  }
  if (lvl < 0) return "...";
  switch (typeof(mix)) {
    default:
    case T_INVALID:
      return "<invalid>";
    case T_LVALUE: // funzt leider nicht ;)
      return "&"+mixed_to_string(mix, lvl);
    case T_FLOAT:
      ret = sprintf("%Q", mix);
      if (member(ret, '.')<0) {
        i = member(ret, 'e');
        if (i >= 0) ret[i..i] = ".0e";
        else ret += ".0";
      }
      return ret;
    case T_NUMBER:
      return to_string(mix);
    case T_STRING:
      ret = implode(efun::explode(mix, "\\"), "\\\\");
      ret = implode(efun::explode(ret, "\0"), "\\0");
      ret = implode(efun::explode(ret, "\a"), "\\a");
      ret = implode(efun::explode(ret, "\b"), "\\b");
      ret = implode(efun::explode(ret, "\e"), "\\e");
      ret = implode(efun::explode(ret, "\f"), "\\f");
      ret = implode(efun::explode(ret, "\n"), "\\n");
      ret = implode(efun::explode(ret, "\r"), "\\r");
      ret = implode(efun::explode(ret, "\t"), "\\t");
      ret = implode(efun::explode(ret, "\""), "\\\"");
      ret = regreplace(ret, "[^ -~]", (: sprintf("\\\\%03d", $1[0]&255) :), 1);
      return "\""+ret+"\"";
    case T_POINTER:
      if (lvl<1 && sizeof(mix)) return "({...})";
      if (member(mts_cache, mix)) return "<recursive array>";
      mts_cache += ([ mix ]);
      i = sizeof(mix);
      ret = allocate(i);
      for (i=sizeof(mix); i--;) ret[i] = mixed_to_string(mix[i], lvl-1);
      return "({"+implode(ret, ",")+"})";
    case T_CLOSURE:
    case T_SYMBOL:
    case T_OBJECT:
      return sprintf("%Q", mix);
    case T_MAPPING:
      if (member(mts_cache, mix)) return "<recursive mapping>";
      mts_cache += ([ mix ]);
      s = sizeof(keys = m_indices(mix));
      t = widthof(mix);
      ret = "([";
      for (i = 0; i<s; i++) {
        if (lvl<1) {
          ret += "...";
          break;
        }
        ret += mixed_to_string(keys[i], lvl-1);
        if (t) {
          ret += ":" + mixed_to_string(mix[keys[i],0], lvl-1);
          for(j=1;j<t;j++)
            ret += ";" + mixed_to_string(mix[keys[i],j], lvl-1);
        }
        if (i<s-1) ret += ",";
      }
      return ret + "])";
    case T_QUOTED_ARRAY:
      return "'"*get_type_info(mix, 1)
        + mixed_to_string(to_array(mix), lvl);
  }
}

// Gibt eine Liste vertikal orientiert aus
varargs string listf(string* liste, int rowlen, int collen) {
  int size, i;
  int row, col, cols, rows;
  string ret;

  // Bestimmen des Modus
  if (!rowlen) cols = collen; // fuer vorbereitete Stringarrays
  else {
    if (!collen) { // collen selber bestimmen
      foreach (ret : liste) {
        size = strlen(ret);
        if (size > i) i = size;
      }
      collen = i + 1; // fuer 1 Leerzeichen
    }
    for (i = sizeof(liste); i--;) // aktuelle Laenge collen anpassen
      liste[i] = sprintf("%-:*s ", collen, liste[i]);
    cols = rowlen/(collen+1);
  }

  ret = "";
  if (cols < 1) cols = 1;
  size = sizeof(liste);
  rows = (--size/cols) + 1;

  do {
    i = col * rows + row;
    if (i <= size) ret += liste[i];
    if (++col >= cols) {
      if (ret[<1] == ' ') ret[<1] = '\n';
      else ret += "\n";
      col = 0;
      row++;
    }
  } while (row < rows);
  return ret;
}

void debug_msg(string uid, varargs mixed* args) {
  object pl;
  pl = find_player(uid);
  if (!pl) return;
  if (sizeof(args) && args[0] == "\b") { // interactive modus
    if (this_interactive() != pl) return;
    args = args[1..];
  }
  if (sizeof(args) && !stringp(args[0]))
    args = ({ "-> " + funcall(#'mixed_to_string, args[0]) + "\n" });

  tell_object(pl, apply(#'sprintf, args));
}

varargs object search_owner(object ob) {
  // Idee vom Unitopia
  if (!ob) ob = previous_object();
  do { ob = environment(ob); }
  while (ob && !living(ob));
  return ob;
}

mixed* generate_room_action(object room, mixed actor,
                                string txt, varargs mixed* args) {
  mixed owner;
  object *owners;
  string *parts, *otext, pre;
  int i, j, n_a, n_p, oi, sl;

  parts = regexplode(txt, "(^|[^%])(%%)*%[nN]");
  n_a = sizeof(args);
  n_p = sizeof(parts);

  if (n_a * 2 + 1 > n_p)
    raise_error("@Zu viele Argumente für tell_room_action()\n");
  if (n_a * 2 + 1 < n_p)
    raise_error("@Zu wenige Argumente für tell_room_action()\n");

  otext = ({ parts[0] });
  owners = ({ 0 });

  if (!pointerp(actor)) actor = ({ actor });

  for (i = 0; i < n_a; ++i) {
    sl = strlen(parts[i*2+1]);
    if (sl > 2) pre = parts[i*2+1][0..(sl-3)/2];
    else pre = "";
    owner = args[i][0]->QueryProp(P_NAME_OWNER);
    if (owner && (owner = owner[0]) && owner != actor[0]
        && present(owner, room)) {
      oi = member(owners, owner);
      if (oi < 0) {
        owners += ({ owner });
        otext += ({ otext[0] });
        oi = sizeof(owners) - 1;
      }
    }
    for (j = sizeof(otext); j--;) {
      otext[j] += pre + args[i][0]->name(args[i][1], args[i][2],
                  owners[j], parts[i*2+1][<1] == 'N' ? actor[0] : 0)
                  + parts[i*2+2];
    }
  }
  return ({ room, actor, owners, otext });
}

void tell_room_action(mixed room, mixed actor,
                                string txt, varargs mixed* args) {
  mixed* action;
  int i;

  if (pointerp(room)) action = room;
  else action = apply(#'generate_room_action, room, actor, txt, args);

  for (i = sizeof(action[3]); --i;) {
    tell_object(action[2][i], break_string(action[3][i]));
  }
  tell_room(action[0], break_string(action[3][0]), action[1] + action[2]);
}

mixed notify_fail(string str)
{
  if (!TP) return;
  TP->SetProp(P_ACTUAL_NOTIFY_FAIL,previous_object());
  return efun::notify_fail(str);
}

mixed _notify_fail(string str)
{
  object o;

  if (!TP) return;
  if ((o=TP->QueryProp(P_ACTUAL_NOTIFY_FAIL)) && o!=previous_object()) 
    return;
  TP->SetProp(P_ACTUAL_NOTIFY_FAIL,previous_object());
  return efun::notify_fail(str);
}

#define PP_PRE 1
#define PP_POST 2

private string pprocess_string(string str,int pp,mixed Cpos)
{string d1,d2,d3,*stra;
int pos,*ta,lv,lv2,tpos,rpos,atpos;
mapping temp;
  if(pp==PP_PRE)
  {
  str=implode(explode(str,"\n")," ");
  Cpos=({});
  pos=0;
    while((sscanf(str,"%s\e[%sm%s",d1,d2,d3)&&efun::sizeof(d2)<=2)||(sscanf(str,"%s\e[H%s",d1,d3))||(sscanf(str,"%s\e[2J%s")))
    {
      if(d2)pos=strstr(str,sprintf("\e[%sm",d2));
      else if((pos=strstr(str,"\e[H"))==-1)pos=strstr(str,"\e[2J");
      if(str[pos..pos]!="\\")
      {
        if(d2)
	{
          Cpos+=({({pos,sprintf("\e[%sm",d2)})});
          str=str[0..pos-1]+str[pos+sizeof(sprintf("\e[%sm",d2))..];
	}
	else
	{
	  if(str[pos..pos+2]=="\e[H")
	  {Cpos+=({({pos,"\e[H"})});str=str[0..pos-1]+str[pos+3..];}
	  else
	  {Cpos+=({({"\e[2J"})});str=str[0..pos-1]+str[pos+4..];}
	}
      }
      d2=0;
    }
  }
  if(pp==PP_POST)
  {
    stra=efun::explode(str,"\n");
    tpos=0;
    atpos=0;
    for(lv=0;lv<sizeof(Cpos);lv++)
    {
      rpos=0;
      lv2=0;
//      while((rpos+sizeof(stra[lv2])<(((int)(Cpos[lv][0]))+atpos))&&
      while(((rpos+sizeof(stra[lv2]))<(((Cpos[lv][0]))+atpos))&&
            (rpos<sizeof(str)))
      {
        rpos+=sizeof(stra[lv2])+1;
	lv2++;
      }
      stra[lv2]=stra[lv2][0..((((Cpos[lv][0]))+atpos)-rpos-1)]+(({string})(Cpos[lv][1]))+
                stra[lv2][((((Cpos[lv][0]))+atpos)-rpos)..];
      atpos+=sizeof(({string})(Cpos[lv][1]));
    }
    str=efun::implode(stra,"\n");
  }
return str;
}

//private mixed *C2pos;

private string pprocess_unprocessed_string(string str,int pp,mixed Cpos)
{string d1,d2,d3,*stra,*cols;
int pos,*ta,lv,lv2,tpos,rpos,atpos;
mapping temp;
  if(pp==PP_PRE)
  {
  cols=explode(implode(m_indices(COLS),""),"^");
  str=implode(explode(str,"\n")," ");
  Cpos=({});
  pos=0;
  while((sscanf(str,"%s^%s",d1,d3)&&efun::sizeof(d3)&&member(cols,d3[0..0])>=0)||
        (sscanf(str,"%s^<%s>%s",d1,d2,d3)))
    {
      if(d2)pos=strstr(str,sprintf("^<%s>",d2));
      else pos=strstr(str,sprintf("^%s",d3[0..0]));
//      if(str[pos..pos]!="\\")
      {
        if(d2)
	{
          Cpos+=({({pos,sprintf("^<%s>",d2)})});
          str=str[0..pos-1]+str[pos+sizeof(sprintf("^<%s>",d2))..];
	}
	else
	{
	  Cpos+=({({pos,sprintf("^%s",d3[0..0])})});
	  str=str[0..pos-1]+str[pos+2..];
	}
      }
      d2=0;
    }
  }
  if(pp==PP_POST)
  {
    stra=efun::explode(str,"\n");
    tpos=0;
    atpos=0;
    for(lv=0;lv<sizeof(Cpos);lv++)
    {
      rpos=0;
      lv2=0;
      while((rpos+sizeof(stra[lv2])<(((Cpos[lv][0]))+atpos))&&(rpos<sizeof(str)))
      {
        rpos+=sizeof(stra[lv2])+1;
	lv2++;
      }
      stra[lv2]=stra[lv2][0..((((Cpos[lv][0]))+atpos)-rpos-1)]+(({string})(Cpos[lv][1]))+
                stra[lv2][((((Cpos[lv][0]))+atpos)-rpos)..];
      atpos+=sizeof((({string})(Cpos[lv][1])));
    }
    str=efun::implode(stra,"\n");
  }
return str;
}

varargs string parse_color_string(string str,int bs,int sc)
{int lv;
string *inds,d1,d2,fg,bg;
if(!str)return str;
  if(sizeof(regexplode(str,"\\^[RGBYWPCIFNDSAL<]"))>1)
  {
    inds=m_indices(COLS);
    for(lv=0;lv<sizeof(COLS);lv++)
      str=efun::implode(efun::explode(str,inds[lv]),sc?"":COLS[inds[lv]]);
    while(sscanf(str,"%s^<%s,%s>%s",d1,fg,bg,d2))
      str=efun::implode(efun::explode(str,sprintf("^<%s,%s>",fg,bg)),
                                          sc?"":sprintf("\e[%s;%sm",fg,bg));
  }
  if(bs)str=break_string(str);
  return str;
} 

varargs string break_color_string(string s, int w, mixed indent, int flags)
{
  mixed Cpos;
  return pprocess_string(break_string(pprocess_string(s,PP_PRE,&Cpos),w,indent,flags,1),PP_POST,&Cpos);
}

varargs string extract(string str, int from, int to) {

  if(!stringp(str)) {
    set_this_object(previous_object());
    raise_error(sprintf("Bad argument 1 to extract(): %O",str));
  }
  if (intp(from) && intp(to)) {
    if (from>=0 && to>=0)
      return(str[from .. to]);
    else if (from>=0 && to<0)
      return(str[from .. <abs(to)]);
    else if (from<0 && to>=0)
      return(str[<abs(from) .. to]);
    else
      return(str[<abs(from) .. <abs(to)]);
  }
  else if (intp(from)) {
    if (from>=0)
      return(str[from .. ]);
    else
      return(str[<abs(from) .. ]);
  }
  else {
    return(str);
  }
}

varargs nomask float set_light(<float|int> i)
{
  object ob;

  if (!(ob=previous_object())) return 0; // ohne das gehts nicht.

  // aus kompatibilitaetsgruenden kann man auch den Lichtlevel noch setzen
  if (i!=0) ob->SetProp(P_LIGHT, ob->QueryProp(P_LIGHT)+i);

  float gesamtlicht = 0;
  
  if(interactive(ob))
    gesamtlicht += ({float})ob->QueryProp(P_LIGHT);

  object *envs = all_environment(ob);

  if(!sizeof(envs) == 0)
  {  
    for(int j=0;j < sizeof(envs);j++)
    {
      if(!stringp(envs[j]->QueryProp(P_LIGHT)))
        gesamtlicht += ({float})envs[j]->QueryProp(P_LIGHT);
      
      object *invs = deep_inventory(envs[0]);
      if(!sizeof(invs) == 0)
      {  
        for(int k=0;k < sizeof(invs);k++)
        {
          if(!interactive(invs[k]))
          {
            if(!stringp(invs[k]->QueryProp(P_LIGHT)))
              gesamtlicht += ({float})invs[k]->QueryProp(P_LIGHT);
          }
        }
      }
    }
  }

  return gesamtlicht;
}

string lowerstring(string s)
{
  if (!stringp(s)) return 0;
  return lower_case(s);
}

public varargs string CountUp( string *s, string sep, string lastsep )
{
    string ret;

    if ( !pointerp(s) )
       return "";
    
    if (!sep) sep = ", ";
    if (!lastsep) lastsep = " und ";

    switch (sizeof(s))  {
       case 0: ret=""; break;
       case 1: ret=s[0]; break;
       default:
              ret = implode(s[0..<2], sep);
              ret += lastsep + s[<1];
    }
    return ret;
}

nomask int colsizeof(string str)
{int len;
string d1,d2,d3,*tstr;
  tstr=explode(str,"\\");
  len=sizeof(tstr)-1;
  str=implode(tstr,"");
  while(sscanf(str,"%s\e[%sm%s",d1,d2,d3)&&sizeof(d2)<=2)
    str=implode(explode(str,sprintf("\e[%sm",d2)),"");
// so und noch 2 spezialfaelle... hoffendlich gibts da nich mehr von :)
  str=implode(explode(str,"\e[H"),"");
  str=implode(explode(str,"\e[2J"),"");
  return sizeof(str)+len;
} 

string upperstring(string s)
{
#if __EFUN_DEFINED__(upper_case)
  return(upper_case(s));
#else
  int i;
  if (!stringp(s)) return 0;
  for (i=sizeof(s)-1;i>=0;i--) s[i]=((s[i]<'a'||s[i]>'z')?s[i]:s[i]-32);
  return s;
#endif
}

private varargs void
assert_alist(mixed * alist, string context = "")
{
    if (!efun::sizeof(alist) || !efun::pointerp(alist[0]))
        // efun::raise_error(context + "Missing key array.\n");
        return;

    int keynum = efun::sizeof(alist[0]);
    for (int i = efun::sizeof(alist); i-->1;)
    {
        if (!efun::pointerp(alist[i]) || efun::sizeof(alist[i]) != keynum)
            // efun::raise_error(context + "Type or size mismatch of the data arrays.\n");
            return;
    }
}

varargs mixed
assoc(mixed key, mixed * keys_or_alist, mixed data_or_fail, mixed fail)
{
    // if (!efun::pointerp(keys_or_alist))
    //     raise_error("Bad argument 2 to assoc().\n");

    // case 3: mixed assoc(mixed key, mixed *keys, mixed *data [, mixed fail])
    if (efun::pointerp(data_or_fail))
    {
        // if (efun::sizeof(keys_or_alist) != efun::sizeof(data_or_fail))
        //     efun::raise_error("Number of keys and values differ.\n");

        int idx = efun::member(keys_or_alist, key);
        return idx < 0 ? fail : data_or_fail[idx];
    }

    // case 2: mixed assoc(mixed key, mixed *alist [, mixed fail])
    if (efun::sizeof(keys_or_alist) && efun::pointerp(keys_or_alist[0]))
    {
        assert_alist(keys_or_alist, "Bad argument 2 to assoc(): ");

        int idx = efun::member(keys_or_alist[0], key);
        return idx < 0 ? data_or_fail : keys_or_alist[1][idx];
    }

    // case 1: int assoc(mixed key, mixed *keys)
    // if (data_or_fail)
    //     efun::raise_error("Bad number of arguments to assoc().\n");

    return efun::member(keys_or_alist, key);
}

mixed
insert_alist(mixed key, varargs mixed * args)
{
    // if (!efun::sizeof(args))
    //     raise_error("Missing argument to insert_alist().\n");

    // case 2: int insert_alist(mixed key, mixed * keys)
    if (efun::sizeof(args) == 1)
    {
        // if (!efun::pointerp(args[0])
        //         || (efun::sizeof(args[0])
        //             && !efun::intp(args[0][0])
        //             && !efun::stringp(args[0][0])))
        //     efun::raise_error("Bad argument 2 to insert_alist().\n");

        int idx = efun::member(args[0], key);

        if (idx >= 0) return idx; // existing key

        // add new key, if key array was given by reference
        if (referencep(&(args[0])))
            args[0] += ({ key });
        return efun::sizeof(args[0]) - 1;
    }

    // case 1: mixed * insert_alist(mixed key, mixed data..., mixed * alist)
    assert_alist(args[<1],
            "Bad argument " + efun::sizeof(args) + " to insert_alist(): ");

    mixed * alist = efun::deep_copy(args[<1]);
    
    if(sizeof(alist) == 0)
      return;

    int idx = efun::member(alist[0], key);
    
    if (idx >= 0)
    {
        // existing key
        for (int i = efun::sizeof(alist); i-->1;)
        {
            alist[i][idx] = args[i-1];
        }
    }
    else
    {
        // new key
        alist[0] += ({ key });

        for (int i = efun::sizeof(alist); i-->1;)
        {
            alist[i] += ({ args[i-1] });
        }
    }

    /*
    // update arg if it was given by reference
    if (referencep(&(args[<1])))
        args[<1] = alist;
     */
    return alist;
}

public varargs string break_unprocessed_string(string s, int w, mixed indent, int flags)
{mixed Cpos;
return pprocess_unprocessed_string(_break_string(pprocess_unprocessed_string(s,PP_PRE,&Cpos),w,indent,flags),PP_POST,&Cpos);
} 

object set_modify_command(varargs <object|string|int>* arg)
{
    object ob = efun::previous_object();
    object|string modifier;
    object|string oldmodifier = efun::interactive_info(ob, IC_MODIFY_COMMAND);

    if(!sizeof(arg))
        modifier = ob;
    else if(!intp(arg[0]) || !arg[0])
        modifier = arg[0];
    else
        return oldmodifier;

    efun::configure_interactive(ob, IC_MODIFY_COMMAND, modifier);
    return oldmodifier;
}

varargs mixed slice_array(mixed array, int from, int to) {

  if(!pointerp(array)) {
    set_this_object(previous_object());
    raise_error(sprintf("Bad argument 1 to slice_array(): %O",array));
  }
  if (intp(from) && intp(to)) {
    if (from>=0 && to>=0)
      return(array[from .. to]);
    else if (from>=0 && to<0)
      return(array[from .. <abs(to)]);
    else if (from<0 && to>=0)
      return(array[<abs(from) .. to]);
    else
      return(array[<abs(from) .. <abs(to)]);
  }
  else if (intp(from)) {
    if (from>=0)
      return(array[from .. ]);
    else
      return(array[<abs(from) .. ]);
  }
  else {
    return(array);
  }
}

static int _isindomain(object ob, mixed d, int flag) {
  string *env;
  if (!environment(ob)) return 0;
  return flag ?
    sizeof(regexp(({object_name(environment(ob))}), d))
    : (sizeof(env=efun::explode(object_name(environment(ob)),"/")) < 3 ? 0 :
    member(d, env[2]));
}

varargs void domainshout(string s, mixed domain, int regexpflag) {
  if (!regexpflag=regexpflag&&stringp(domain)) {
    if (stringp(domain))
      domain=([ domain ]);
    else if (pointerp(domain))
      domain=mkmapping(domain);
    else {
      mixed ob;
      ob=all_environment(previous_object());
      if (!sizeof(ob)) ob=previous_object(); else ob=ob[<1];
      domain=([ efun::explode(object_name(ob),"/")[2] ]);
    }
  }
  filter(
      filter(users(), #'_isindomain, domain, regexpflag),
      lambda(({'u}),({#'&&, ({#'environment, 'u}),
        ({#'!=, 'u, ({#'this_player})}), //'
          ({#'tell_object, 'u, to_string(s)})
          })));
}

nomask int secure_level()
{
  int *level;
  //kette der Caller durchlaufen, den niedrigsten Level in der Kette
  //zurueckgeben. Zerstoerte Objekte (Selbstzerstoerer) fuehren zur Rueckgabe
  //von 0.
  //caller_stack(1) fuegt dem Rueckgabearray this_interactive() hinzu bzw. 0,
  //wenn es keinen Interactive gibt. Die 0 fuehrt dann wie bei zerstoerten
  //Objekten zur Rueckgabe von 0, was gewuenscht ist, da es hier einen
  //INteractive geben muss.
  level=map(caller_stack(1),function int (object caller)
      {if (objectp(caller))
       return(query_wiz_level(geteuid(caller)));
       return(0); // kein Objekt da, 0.
      } );
  return(min(level)); //den kleinsten Wert im Array zurueckgeben (ggf. 0)
}

nomask int colstrlen(string str)
{int len;
string d1,d2,d3,*tstr;
  tstr=explode(str,"\\");
  len=sizeof(tstr)-1;
  str=implode(tstr,"");
  while(sscanf(str,"%s\e[%sm%s",d1,d2,d3)&&strlen(d2)<=2)
    str=implode(explode(str,sprintf("\e[%sm",d2)),"");
// so und noch 2 spezialfaelle... hoffendlich gibts da nich mehr von :)
  str=implode(explode(str,"\e[H"),"");
  str=implode(explode(str,"\e[2J"),"");
  return strlen(str)+len;
}

int lowerchar(int char) {
  if (char<'A' || char>'Z') return char;
  return char+32;
}

mapping m_copy_delete(mapping m, mixed key) {
  return m_delete(copy(m), key);
}

/// This function maps an RGB value (each 0..255) to the nearest ANSI color.
string rgb_to_ansi(int r, int g, int b) {
  // Define an array of mappings.
  // The RGB values here are chosen as “representatives” of the ANSI colors.
  // You can adjust these if you have a different idea of what each ANSI code should represent.
  mapping *colors = ({
      ([ "ansi": ANSI_BLACK,  "r":   0, "g":   0, "b":   0 ]),
      ([ "ansi": ANSI_RED,    "r": 128, "g":   0, "b":   0 ]),
      ([ "ansi": ANSI_GREEN,  "r":   0, "g": 128, "b":   0 ]),
      ([ "ansi": ANSI_BROWN,  "r": 128, "g": 128, "b":   0 ]), // brown/dark yellow
      ([ "ansi": ANSI_BLUE,   "r":   0, "g":   0, "b": 128 ]),
      ([ "ansi": ANSI_PURPLE, "r": 128, "g":   0, "b": 128 ]),
      ([ "ansi": ANSI_CYAN,   "r":   0, "g": 128, "b": 128 ]),
      ([ "ansi": ANSI_GREY,   "r": 192, "g": 192, "b": 192 ]),
      ([ "ansi": ANSI_WHITE,  "r": 255, "g": 255, "b": 255 ])
  });
  
  // We'll use the squared Euclidean distance for comparison
  int best_dist = 1<<30; // a very high value
  string best_ansi = ANSI_WHITE; // default fallback
  
  foreach(mapping col in colors) {
      int dr = r - col["r"];
      int dg = g - col["g"];
      int db = b - col["b"];
      int dist = dr * dr + dg * dg + db * db;
      if(dist < best_dist) {
          best_dist = dist;
          best_ansi = col["ansi"];
      }
  }
  
  return best_ansi;
}

#define COLORS ([ "none": ANSI_NORMAL,     "bold": ANSI_BOLD, \
                  "blink": ANSI_BLINK,     "invers": ANSI_INVERS, \
                  "black": ANSI_BLACK,     "red": ANSI_RED, \
                  "green": ANSI_GREEN,     "yellow": ANSI_YELLOW, \
                  "blue": ANSI_BLUE,       "purple": ANSI_PURPLE, \
                  "cyan": ANSI_CYAN,       "white": ANSI_WHITE, \
                  "bblack": BACK_BLACK, "bred": BACK_RED, \
                  "bgreen": BACK_GREEN, "byellow": BACK_YELLOW, \
                  "bblue": BACK_BLUE,   "bpurple": BACK_PURPLE, \
                  "bcyan": BACK_CYAN])


private string do_getcolor(string color) { return COLORS[lower_case(color)]; }

string getcolor(string colorstring)
{
  return implode( map( explode(
          colorstring, "+" ), #'do_getcolor ), "" );
}

public varargs string getuuid(object ob)
{
  mixed *ret;
  
  if (!objectp(ob))
    ob = previous_object();

  if (!query_once_interactive(ob))
    return getuid(ob);

  ret = __MASTER_OBJECT__->get_userinfo( getuid(ob) );

  if (!pointerp(ret) || (sizeof(ret) < 5))
    return getuid(ob);

  // Username + "_" + CreationDate
  return sprintf("%s_%d", ret[0], ret[5]);
}
