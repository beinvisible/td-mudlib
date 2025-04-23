/* SilberLand MUDlib
 *
 * /secure/master/userinfo.c -- secure save file handling
 * 
 * 2024/04/09 - m_allocate durch m_allocate ersetzt, (void) bei rm entfernt - Aloy
 *
 * $Date: 2001/01/18 10:37:57 $
 * $Revision: 1.1 $
 * $Log: userinfo.c,v $
 * Revision 1.1  2001/01/18 10:37:57  Samtpfote
 * Initial revision
 *
 * Revision 1.7  1997/11/30 16:00:49  Woody
 * Bugfix in delete_player (Zeitungsartikel buggte)
 *
 * Revision 1.6  1997/11/25 11:39:26  Feyaria
 * playerdelete
 *
 * Revision 1.5  1997/04/09 20:50:55  Woody
 * Bugfix: dtime() is ne simulefun -> Aufruf per funcall() *argl*
 *
 * Revision 1.4  1997/04/08 16:23:01  Woody
 * Selbstloeschungsartikel ueberarbeitet
 *
 * Revision 1.3  1997/04/08 15:19:22  Woody
 * Meldung auf [Info:] bei Selbstloeschung
 *
 * Revision 1.2  1997/03/17 19:30:22  Woody
 * Bei finger neue /p Verzeichnisstruktur beruecksichtigt
 *
 * Revision 1.1  1997/03/16 00:05:15  Woody
 * prepared for revision control
 *
 */

#pragma strong_types

#include "/secure/master.h"

#define HBINT    2   /* interval between two heart_beats in seconds */

static mapping userlist;
string name, password;
int level;
string *domains, shell;
int creation_date;

void create()
{
  userlist=m_allocate(0,6);
}

int clear_cache() // Vergiss ALLE infos im Cache
{
  userlist=m_allocate(0,6);
  return 1;
}

string secure_savefile(string name)
{
  if(!name||name=="")
    return "";
  name=explode(name,".")[0];
  if (file_size(SECURESAVEPATH+name[0..0]+"/"+name+".o")>=0)
    return SECURESAVEPATH+name[0..0]+"/"+name;
  else if (file_size("secure/save_inactive/"+name[0..0]+"/"+name+".o")>=0)
    return "secure/save_inactive/"+name[0..0]+"/"+name;
  return "";
}

string secure_isavefile(string name)
{
  if(!name||name=="")
    return "";
  name=explode(name,".")[0];
  if (file_size("secure/save_inactive/"+name[0..0]+"/"+name+".o")>=0)
    return "secure/save_inactive/"+name[0..0]+"/"+name;
  return "";
}

mixed *show_cache(){return m_indices(userlist);}

int find_userinfo(string user) {
  string file;
  
  if (!user||user==""||member(user,' ')!=-1||member(user,':')!=-1)
    return 0;
  if (!member(userlist,user)) {
    if ((file=secure_savefile(user))=="")
    {
      userlist+=([user: "NP"; -1; ({}); "LOCKED"; -1; time() ]);
      return 0;
    }  
    password="";
    if (!restore_object(file)) return 0;
    userlist+=([user: password; level; domains; shell; creation_date; time() ]);
  }
  userlist[user,USER_TOUCH]=time();
  if (userlist[user,USER_LEVEL]==-1) return 0;
  return 1;
}

void RemoveFromCache(string user)
{
  efun::m_delete(userlist,user);
}

mixed *get_userinfo(string user) {
  if(!user||user=="")
    return 0;
  user=explode(user,".")[0];
  if (!member(userlist,user) && !find_userinfo(user))
    return 0;
  level=userlist[user,USER_LEVEL];
  if (level==-1) return 0;
  domains=userlist[user,USER_DOMAIN];
  shell=userlist[user,USER_OBJECT];
  creation_date=userlist[user,USER_CREATION_DATE];
  return ({user,"##"+user,level,domains,shell,creation_date,0});
}

static mixed *get_full_userinfo(string user) {
  if(!user||user=="")
    return 0;
  user=explode(user,".")[0];
  if (!member(userlist,user) && !find_userinfo(user))
    return 0;
  password=userlist[user,USER_PASSWORD];
  level=userlist[user,USER_LEVEL];
  domains=userlist[user,USER_DOMAIN];
  shell=userlist[user,USER_OBJECT];
  creation_date=userlist[user,USER_CREATION_DATE];
  return ({user,password,level,domains,shell,creation_date,0});
}

static void save_userinfo(string user) {
  if(!user||user=="")
    return;
  user=explode(user,".")[0];
  if (!member(userlist,user)) return;
  name = user;
  level = userlist[name,USER_LEVEL];
  domains = userlist[name,USER_DOMAIN];
  shell = userlist[name,USER_OBJECT];
  password = userlist[name,USER_PASSWORD];
  creation_date = userlist[name,USER_CREATION_DATE];
  if (!creation_date) creation_date = -1;
  save_object(SECURESAVEPATH+name[0..0]+"/"+name);
}

int update_wiz_level(string user,int lev) {
  if (getuid(PO) != ROOTID) return 0;
  if (!find_userinfo(user)) return 0;
  userlist[user,USER_LEVEL] = lev;
  save_userinfo(user);
  return 1;
}

int set_player_object(string user, string objectname)
{
  mixed *path;
  string prev;
  
  if (objectname=="") objectname=0;
  if (!stringp(user) || user=="") return -6;
  if (!stringp(objectname))
  {
    if (!find_userinfo(user)) return -4;
    userlist[user,USER_OBJECT]=0;
    save_userinfo(user);
    return 1;
  }
  if (catch(call_other(objectname,"??")))
  {
    write("Fehler in "+objectname+"!\n");
    return -2;
  }
  objectname=_get_path(objectname,0);
  path=(explode(objectname,"/")-({""}))-({0});
  if (sizeof(path)<3 || path[0]!="std" || path[1]!="shells") return -3;
  if (!find_userinfo(user)) return -4;
  userlist[user,USER_OBJECT]=objectname;
  save_userinfo(user);
  return 1;
}

string query_player_object( string name )
{
  mixed *userentry;
  if( !find_userinfo(name) ) return "";
  return userlist[name,USER_OBJECT];
}

int update_password(string old,string new) {
  string user;
  
  if (!find_userinfo(user=getuid(PO))) return 0;
  password = userlist[user,USER_PASSWORD];
  if (password && password != "" && crypt(old,password) != password)
    return 0;
  userlist[user,USER_PASSWORD] = crypt(new,0);
  save_userinfo(user);
  return 1;
}

string get_wiz_name(string file) {return creator_file(file);}

int get_wiz_level(string user) {
  if (user && find_userinfo(user)) return userlist[user,USER_LEVEL];
  return 0;
}

int query_wiz_level(mixed player) {
  if (objectp(player) && query_once_interactive(player))
    return get_wiz_level(getuid(player));
  else
    if (stringp(player))
      return get_wiz_level(player);
  return 0;
}

static void set_domains(string player, mixed *domains)
{
  userlist[player, USER_DOMAIN]=domains;
  save_userinfo(player);
}

void _cleanup_uinfo() {
  mixed users;
  int i;

  remove_call_out ("_cleanup_uinfo");
  call_out("_cleanup_uinfo", 3600);
  for (users=m_indices(userlist),i=sizeof(users)-1; i>=0;i--)
    if ((time()-userlist[users[i],USER_TOUCH]) > 1800
	&& !funcall(symbol_function('find_player),users[i]))
      userlist=m_delete(userlist,users[i]);
  _cleanup_projects();
}

int delete_player(string passwd, string real_name)
{
  mixed *userentry, *userinfo, *art, h;
  mapping props;
  string part_filename, text, wbl, guild;
  int wiz;
  object tp;

  userinfo=MASTER->get_userinfo(real_name);
  tp = TP;
  wiz = IS_WIZARD(tp);
  if (wiz) wiz = query_wiz_level(tp);
  props = tp->QueryProperties();
  guild = props["guild"];
  if (!guild || guild="") guild = "abenteurer";
  text = sprintf ("%s:\n%s hat sich soeben selbst entfernt.\n\n",
      funcall(symbol_function('dtime),time()), capitalize(real_name));

  if (props["gender"] == 2) {
    text += "Sie"; wbl = "in"; }
  else {
    text += "Er"; wbl = ""; }
  text += " war zu diesem Zeitpunkt ";
  text += wiz ?
    FINGERMASTER->wiz_level_name(wiz, props["gender"], real_name)+
    " (Magierlevel: "+wiz+")" :
    ((IS_SEER(tp) ? "Seher" : "Spieler")+wbl);

  text += ".\nVoller Name: "+(((h = props["presay"]) && h!="") ? h+" " : "")+
          capitalize(stringp(h=props["name"]) ? h : real_name)+
          (stringp(h=tp->QueryProp("title")) && h!="" ? " "+h : "");

  text += sprintf ("\nRasse: %s, Geschlecht: %s, Alter: %s\n",
      tp->QueryProp("race") || "unbekannt",
      ([0:"neutral?!", 1:"maennlich",
        2:"weiblich"]) [props["gender"]] || "buggy",
      FINGERMASTER->timediff(tp->QueryProp("age")*HBINT));

  if (wiz)
  {
    mixed data;

    data=userinfo[3];
    if (sizeof(data)) 
      text+="Regionsmagier von     : "+
        implode(map(data,#'capitalize),", ")+".\n";
    data="/secure/master"->get_domain_homes(real_name);
    if (sizeof(data))
      text+="Regionsmitarbeiter von: "+
        implode(map(data,#'capitalize),", ")+".\n";
  }
  else
    text += sprintf("Gilde: %s, Gildenlevel: %d, Spielerlevel: %d\n",
        capitalize(guild), tp->QueryProp("guild_level"), props["level"]);

  if (stringp(h=props["married"]))
    text += "Verheiratet mit: "+capitalize(h)+"\n";
  if (h=props["testplayer"])
    text+=capitalize(real_name)+" war Testspieler"+wbl+
      (stringp(h) ? " von "+h : "")+".\n";
  if (h=props["second"])
    text+=capitalize(real_name)+" war Zweitspieler"+wbl+
      (stringp(h) ? " von "+h : "")+".\n";
  text += ((h=props["deads"]) ? h+" mal" : "Nie")+" gestorben!\n";
  text += ((h=props["playerkills"]) ? h+" PLAYERKILLS!!\n" : "")+"\n";

  art = ({"chronik", "TDP", 0, 0,
      sprintf ("Selbstentfernung von %s",capitalize(real_name)), text});

  if (!PO || PO!=TP || PO!=TI || real_name != getuid(PO) ||
      !find_userinfo(real_name))
    return 0;
  password = userlist[real_name,USER_PASSWORD];
  if (crypt(passwd, password) != password) return 0;

  if (props["level"] > 1)
    NEWSSERVER->WriteNote(art,1);

  call_other("/obj/infomaster", "InfoMsg",
      capitalize(real_name)+" hat sich soeben selbst entfernt.");

  part_filename="/"+real_name[0..0]+"/"+real_name+".o";
  rm("secure/save"+part_filename);
  rm("save"+part_filename);
  rm("mail"+part_filename);
  userlist=m_delete(userlist,real_name);
  log_file("USERDELETE",sprintf("%s: %s\n",ctime(time()),real_name));
  return 1;
}

int playerdel(string real_name)
{
  string part_filename;
  object ob;

  if (getuid(PO) != ROOTID) return 0;
  if (!IS_ARCH(TI)) return 0;

  if (!find_userinfo(real_name))
  {
    write("User "+real_name+" nicht gefunden!\n");
    return 0;
  }
  part_filename="/"+real_name[0..0]+"/"+real_name+".o";
  rm("secure/save"+part_filename);
  rm("save"+part_filename);
  rm("mail"+part_filename);
  userlist=m_delete(userlist,real_name);
  log_file("USERDELETE",sprintf("%s: %s entfernt von %s\n",
    ctime(time()),real_name,TI->query_real_name()));
  call_other("/obj/infomaster", "InfoMsg",
      capitalize(real_name)+" wurde gerade von "+
      capitalize(TI->query_real_name())+" entfernt.");

  return 1;
}

