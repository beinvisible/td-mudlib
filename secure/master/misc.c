/* SilberLand MUDlib
 *
 * /secure/master/misc.c -- miscellaneous functions of the master object
 * 
 * 2024/04/09 - inherit vor include gesetzt. inherits muessen immer VOR funktionsdekalration sein, PATH_ARRAY define entfernt, da bereits in master.h inkludiert. zeile 521 read_bytes gegen read_file ersetzt. bytes koennen nicht nach string direkt gehen - Aloy
 *
 * $Date: 2001/01/18 10:37:50 $
 * $Revision: 1.1 $
 * $Log: misc.c,v $
 * Revision 1.1  2001/01/18 10:37:50  Samtpfote
 * Initial revision
 *
 * Revision 1.4  1997/07/06 15:53:10  Woody
 * show_dir() ueberarbeitet, nimmt jetzt flags und arrays usw.
 *
 * Revision 1.3  1997/06/21 00:34:24  Woody
 * Bugfixed... jetzt sollte es klappen :)
 *
 * Revision 1.2  1997/06/21 00:08:26  Woody
 * Neues Format fuer ls -l, damit ftp auch mit mc, xftp usw. geht
 *
 * Revision 1.1  1997/03/16 00:02:51  Woody
 * prepared for revision control
 *
 * 1996/10/29  Woody  bei renew deep destruct von Autoloadern, weiters
 *                    Unterdruecken von 'XY verlaesst diese Welt.'
 *
 * 1996/09/23  Woody  bei renew silent move und umgek. Reihenfolge
 *
 * 1996/01/23  Woody  added UnbanishName()
 *
 */

// #pragma strong_types

inherit "/secure/master/fileaccess";

#include "/secure/master.h"

static mapping projects=([]);
static mixed *banished;
static mapping tbanished;

static string *explode_files(string file) {
  string data;
  mixed *exploded;
  int i;
  
  data=read_file(file);
  if (!data || !stringp(data) || data == "") return ({});
  exploded = efun::explode(data,"\n");
  for (i=sizeof(exploded);i--;)
    if (!stringp(exploded[i]) || exploded[i]=="" || exploded[i][0]=='#')
      exploded[i]=0;
  exploded-=({0});
  printf("%s%-30s%s: %s%3d%s %sObjekt%s%s\n", ANSI_YELLOW+ANSI_BOLD, file, ANSI_NORMAL, ANSI_GREEN+ANSI_ITALIC, i=sizeof(exploded), ANSI_NORMAL, ANSI_YELLOW+ANSI_BOLD,(i==1?"":"e"),ANSI_NORMAL);
  return exploded;
}

string *full_path_array(string path, string user) {
  string *strs;
  int p;
  
  if(!path) return ({""});

  if(!path)
    path="/";
  switch(path[0]) {
    case '/':
    if(!path[1]) return ({""});
    strs=PATH_ARRAY(path);
    break;
    case '+':
    if(!path[1]) return ({"d"});
    strs=({"d"})+PATH_ARRAY(path[1..<1]);
    break;
    case '~':
    if(user && !path[1])
      return ({"players",user});
    if(user && path[1]=='/')
      strs=({"players",user})+PATH_ARRAY(path[2..<1]);
    else
      strs=({"players"})+PATH_ARRAY(path[1..<1]);
    break;
    default:
    if(user && TP && getuid(TP) == user && TP->QueryProp(P_CURRENTDIR))
      strs=PATH_ARRAY(TP->QueryProp(P_CURRENTDIR)+"/"+path);
    else
      strs=PATH_ARRAY(path);
  }
  while((p=member(strs, "..")) != -1)
    strs = strs[0..p-2]+strs[p+1..];
  return strs;
}

string _get_path(string path, string user) {
  return "/"+implode(full_path_array(path, user),"/");
}

static void _cleanup_projects()
{
  int i;
  mixed *users;
  
  for (users=m_indices(projects),i=sizeof(users)-1;i>=0;i--)
    if((time()-projects[users[i]][1])>1800)
      projects=m_delete(projects,users[i]);
}

static int project_access(string user, string project)
{
  mixed *lines;
  string s;
  mapping tmp;

  if (!member(projects,project))
  {
    s=read_file(PROJECTDIR+"/"+project+"/ACCESS_RIGHTS");
    if(!s||s=="")
      return 0;
    tmp=([]);
    for (lines=explode(s,"\n")-({""});sizeof(lines);lines=lines[1..])
    {
      if (lines[0][0]=='*')
	tmp[lines[0][1..]]=2;
      else
	tmp[lines[0]]=1;
    }
    projects[project]=({tmp,time()});
    return tmp[user];
  }
  projects[project][1]=time();
  return projects[project][0][user];
}

void OudateProjectCache(string project)
{
  projects=m_delete(projects,project);
}

void UpdateTBanish();

mixed QueryBanished(string str){
  int i;

  if (!str) return 0;
  if (!pointerp(banished)) return 0;
  for (i=sizeof(banished)-1;i>=0;i--)
    if (sizeof(regexp(({str}),"^"+banished[i][0]+"$")))
    {
      if (!banished[i][1] || banished[i][1]=="")
	return "Dieser Name ist gesperrt.";
      else
	return banished[i][1];
    }
  return 0;
}

mixed QueryTBanished(string str) {
  int i;

  if (!str || !mappingp(tbanished) || !(i=tbanished[str]))
    return 0;

  if (i == -1 || i > time())
    return i;
    /*
    return sprintf("Es gibt schon einen Spieler diesen Namens.\n"
        +"Allerdings kann er/sie erst am %s wieder ins Mud kommen.\n",
          (i == -1 ? "St. Nimmerleinstag" :
           funcall(symbol_function('dtime),i)[0..16]));
    */

  // Ansonsten: die Zeit ist abgelaufen, Spieler darf wieder...
  efun::m_delete(tbanished, str);
  UpdateTBanish();
  return 0;
}

void ReloadBanishFile(){
  int i,t;
  string s1,s2,*s;
  
  banished=efun::explode(read_file("/secure/BANISH"),"\n");
  banished=banished-({""});
  for (i=sizeof(banished)-1;i>=0;i--)
  {
    s=explode(banished[i]," ");
    s1=s[0];
    s2=implode(s[1..]," ");
    banished[i]=({s1,s2});
  }
  if (!mappingp(tbanished)) {
    tbanished = ([]);

    if (file_size("/secure/TBANISH")>0) {
      s = efun::explode(read_file("/secure/TBANISH"),"\n");
      s-= ({""});
    }
    else
      s = ({});

    for (i=sizeof(s)-1; i>=0; i--) {
      sscanf(s[i],"%s:%d",s1,t);
      tbanished += ([ s1 : t ]);
    }
  }
}

void BanishName(string name, string reason)
{
  string *names;
  int i;
  
  if (!IS_LORD(TI)) return 0;
  if (QueryBanished(name)){
    write("Der Name ist schon gebannt.\n");
    return;
  }
  if (file_size(SAVEPATH+name[0..0]+"/"+name+".o")>0){
    write("Es existiert bereits ein Spieler dieses Namens.\n");
    return;
  }
/*  if (!("/secure/login"->valid_name(name))) return;*/
  if (!reason) reason="";
  names=({name+" "+reason});
  for (i=sizeof(banished)-1;i>=0;i--)
    names+=({banished[i][0]+" "+banished[i][1]});
  names=sort_array(names,#'>);
  rm("/secure/BANISH");
  write_file("/secure/BANISH",implode(names,"\n"));
  ReloadBanishFile();
  write("Ok.\n");  // woody@sl
}

void UpdateTBanish()
{
  int i;
  string *names;

  for (i=sizeof(names = sort_array(m_indices(tbanished), #'<))-1;i>=0;i--)
    names[i] = sprintf("%s:%d", names[i], tbanished[names[i]]);

  rm("/secure/TBANISH");
  write_file("/secure/TBANISH", implode(names, "\n"));
}

void TBanishName(string name, int days)
{
  int t;

  if (!IS_ARCH(TI) && (getuid(TI) != name)) return;
  if (QueryBanished(name)){
    write("Der Name ist schon gebannt.\n");
    return;
  }
  if (file_size(SAVEPATH+name[0..0]+"/"+name+".o")<=0){
    write("Es existiert kein Spieler dieses Namens!\n");
    return;
  }

  if (!days && member(tbanished, name))
    efun::m_delete(tbanished, name);
  else {
    if (!mappingp(tbanished))
      tbanished = ([]);
    if (days <= -1)
      t = -1;
    else
      t = (time()/86400)*86400 + days*86400;
    tbanished += ([ name : t ]);
  }
  UpdateTBanish();
}

void UnbanishName(string name)
{
  string *names;
  int i;

  if (!IS_LORD(TI)) return 0;
  if (!QueryBanished(name)){
    write("Der Name ist doch gar nicht gebannt.\n");
    return;
  }
/*  if (!("/secure/login"->valid_name(name))) return;*/
  names=({});
  for (i=0;i<=sizeof(banished)-1;i++)
  {
    if (banished[i][0]!=name)
      names+=({banished[i][0]+" "+banished[i][1]});
  }
  // names=sort_array(names,#'>);
  rm("/secure/BANISH");
  write_file("/secure/BANISH",implode(names,"\n"));
  ReloadBanishFile();
  write("Ok.\n");
}

static int create_home(string owner, int level)
{
  string def_castle;
  string dest, castle, wizard;
  object player;
  string *domains;
  int i;
  
  player = funcall(symbol_function('find_player),owner);
  if (!player)
    return -5;
  domains=get_domain_homes(owner);
  if (!sizeof(domains) && level >= DOMAINMEMBER_LVL) 
  {
    tell_object(player,"Du musst erst einer Region beitreten!\n");
    return -6;
  }
  tell_object(player,"Du bist Mitarbeiter der folgenden Regionen:\n");
  for (i=0;i<sizeof(domains);i++)
  {
    if (i) tell_object(player,", ");
    tell_object(player,domains[i]);
  }
  tell_object(player,".\n");
  update_wiz_level(owner, level);
  wizard = "/players/" + owner;
  castle = "/players/" + owner + "/workroom.c";
  if (file_size(wizard) == -1) {
    tell_object(player, "Verzeichnis " + wizard + " angelegt.\n");
    mkdir(wizard);
  }
  dest = object_name(environment(player));
  def_castle = read_file("/std/def_workroom.c");
  if (file_size(castle) > 0) {
    tell_object(player, "Du HATTEST ja schon ein Arbeitszimmer !\n");
  } else {
    if (write_file(castle, def_castle)) 
      tell_object(player, "Arbeitszimmer " + castle + " erzeugt.\n");
    else
      tell_object(player, "Arbeitszimmer konnte nicht erzeugt werden !\n");
  }
  return 1;
}

int allowed_advance_wizlevel(mixed ob)
{
  string what;
  
  if (objectp(ob) && geteuid(ob)==ROOTID) return 1;
  
  if (!stringp(ob))
    what=explode(object_name(ob),"#")[0];
  else
    what=ob;
  
  if (what=="/secure/merlin") return 1;
  
  return 0;
}

int advance_wizlevel(string name, int level)
{
  int answer;
  mixed *user;
  
  if (!allowed_advance_wizlevel(PO))
    return -1;
  
  if (level>80) return -2;
  
  if (!find_userinfo(name)) return -3;

  user=get_full_userinfo(name);
  
  if (user[USER_LEVEL+1]>level) return -4;
  
  if (user[USER_LEVEL+1]==level) return 1;
  
  if (level>=10 && level<20)
  {
    update_wiz_level(name, level);
    return 1;
  }
  if (level>=20 && user[USER_LEVEL+1]<21)
  {
    answer = create_home(name, level);
    if ( answer > 0 )
      return update_wiz_level(name, level);
    return answer;
  }

  update_wiz_level(name, level);

  return 1;
}

void redo_preload()
{
  mixed *to_preload;
  int i;
  
  to_preload=epilog(0);
  for (i=0;i<sizeof(to_preload);i++)
    catch(preload(to_preload[i]));
  to_preload=0;
}

void restart_heart_beat(object heart_beat)
{
  if (heart_beat) heart_beat->_restart_beat();
}

int renew_player_object(mixed who)
{
  object newob;
  object *obs;
  mixed err;
  string ob_name;
  object *armours, weapon;
  object tp;
  int i,active;

  if (stringp(who))
  {
    who=funcall(symbol_function('find_player),who);//'))
    if (!who)
    {
      who=funcall(symbol_function('find_netdead),who);//'))
      if (!who)
	return -1;
    }
  }
  if (!objectp(who))
    return -2;
  if (!query_once_interactive(who))
    return -3;
  active=interactive(who);
  printf("OK, renewing %O\n",who);
  seteuid(geteuid(who));
  err=catch(newob=clone_object(query_player_object(getuid(who))));
  seteuid(getuid(TO));
  if (err)
  {
    printf("%O\n",err);
    return -4;
  }
  if (!newob)
    return -5;
  /* Ok, the object is here now ... lets go for it ... */
  who->save_me(0);
  disable_commands();
  armours=who->QueryProp(P_ARMOURS);
  weapon=who->QueryProp(P_WEAPON);
  DEBUG_MSG(sprintf("RENEWING: %O %O\n",newob,who));
  ob_name=explode(object_name(newob),"#")[0];
  if (sizeof(ob_name)>11 && ob_name[0..11]=="/std/shells/")
    ob_name=ob_name[11..];
  ob_name=ob_name+":"+getuid(({object})who);
  if (active)
    exec(newob,who);
  if (active && (interactive(who)||!interactive(newob)))
  {
    DEBUG_MSG("ERROR: still active !\n");
    newob->remove();
    return 0;
  }
  newob->start_player(capitalize(getuid(who)),who->_query_my_ip());
  newob->move(environment(who),M_TPORT|M_NOCHECK|M_NO_SHOW|M_SILENT
	      |M_NO_ATTACK);
  obs=deep_inventory(who);
  for (err=sizeof(obs)-1;err>=0;err--)
    if (!obs[err]->QueryProp(P_AUTOLOADOBJ))
      if (environment(obs[err])==who)
        catch(obs[err]->move(newob,M_NOCHECK | M_SILENT));
      else ;
    else
      catch(destruct(obs[err]));
  who->SetProp(P_INVIS, 1);	// verhindert Meldung 'XY verlaesst diese Welt'
  who->remove(1);
  rename_object(newob,ob_name);
  tp=this_player();
  efun::set_this_player(newob);
  if (objectp(weapon))
    weapon->wield_me(1);
  if (pointerp(armours))
    for (i=sizeof(armours)-1;i>=0;i--)
      if (objectp(armours[i]))
	armours[i]->do_wear("alles", 1);
  efun::set_this_player(tp);
  return 1;
}

mixed __query_variable(object ob, string var)
{ 
  if (!PO || !IS_ELDER(geteuid(PO)))
  {
    write("ILLEGAL\n");
    return;
  }
  return
    funcall(bind_lambda(
			unbound_lambda(
				       ({}),
				       ({#'funcall,({#'symbol_variable,var})})),
			ob));
}

void RestartBeats()
{
  int i,size,counter;
  object ob;
  mixed *list;
  string file,obname,fun;
  
  "/secure/simul_efun"->StopCallOut(0);
  write("CALL_OUT-Restart in progress !\n");
  filter(users(),#'tell_object,"CALL_OUT-Restart in progress !\n");
  size=file_size("log/call_out_stop");
  if (size<=0)
    return;
  file="";
  counter=0;
  while (counter<size)
  {
    file+=read_file("log/call_out_stop",counter,
		     (size-(counter+=40000)>0?counter:size));
  }
  list=explode(file,"__(CUT HERE)__\n");
  list=list[<1..];
  list=explode(list[0],"\n")-({""});
  for (i=sizeof(list)-1;i>=0;i--)
    if (sscanf(list[i],"%s \"%s\"",obname,fun)==2 && ob=find_object(obname))
    {
      write(sprintf("%O -> %s\n",ob,fun));
      catch(ob->__restart(fun));
    }
  write("CALL_OUT-Restart completed !\n");
  filter(users(),#'tell_object,"CALL_OUT-Restart completed !\n");
  rename("log/call_out_stop","log/call_out_stop.old");
}

varargs string show_dir(mixed wo, string user, int flags)
{
  string ret;
  mixed *dir;
  int isdir,r,w,x,i,n;
  string tmp;
  object pl;
  int tim;
  string date,group,fname,*farr;

  if (pointerp(wo)) {
    ret="";
    if (sizeof(wo) > 1)
    {
      n=0;
      // files first, directories last
      wo=sort_array(wo, lambda( ({'wo}), ({#'==, ({#'file_size, 'wo}), -2}) ));
      for (i=0; i<sizeof(wo); i++) {
        n++;
        if (file_size(wo[i])==-2)
          ret += "\n"+wo[i]+":\n"+show_dir(wo[i], user, flags);
        else
          ret += show_dir(wo[i], user, flags | SHOWDIR_NOTOTAL);
      }
      if (flags & SHOWDIR_NOTOTAL)
        return ret;
      return "total "+n+"\n"+ret;
    }
    else if (sizeof(wo))
      return show_dir(wo[0], user, flags);
    else return "";
  }
  if (!user) user=geteuid(TI);
  if (!wo && pl=funcall(symbol_function('find_player),user))
    wo=pl->QueryProp(P_CURRENTDIR);
  tim=time();
  isdir=(file_size(wo)==-2);
  ret="";
  n=0;
  for (dir=get_dir(isdir?wo+"/*":wo,7);sizeof(dir)>2;dir=dir[3..])
  {
    if ((flags & SHOWDIR_NODOT) && (dir[0][0]=='.')) continue;
#ifdef OLD_LS_FORMAT
    if (member(dir[0],({".","..",".ftp"}))==-1)
#else
    if (dir[0] != ".ftp")
#endif
    {
      isdir=(dir[1]==-2);
      if (isdir)
        x='x';
      else if (dir[0][<2..<1]!=".c")
	x='-';
      else
	x=(find_object(wo+"/"+({string})dir[0]))?'x':'-';
      w=(valid_write(wo+"/"+dir[0],user,"write_file",0))?'w':'-';
      r=(valid_read(wo+"/"+dir[0],user,"read_file",0))?'r':'-';
      //date="secure/simul_efun"->dtime(dir[2]);
#ifdef OLD_LS_FORMAT
      date=funcall(symbol_function('dtime),dir[2]);
      if ((tim-dir[2])<31536000)
	date=date[9..11]+" "+date[5..6]+" "+date[19..<4];
      else
	date=date[9..11]+" "+date[5..6]+"  "+date[13..16];
#else
      date=funcall(symbol_function('ctime),dir[2]);
      if ((tim-dir[2])<31536000)
	date=date[4..9]+" "+date[11..15];
      else
	date=date[4..9]+"  "+date[<4..];
#endif
      tmp=creator_file(wo+"/"+dir[0]);
      if (tmp==ROOTID) 
	tmp="root";
      else if (tmp==BACKBONEID)
        tmp="daemon";
      else if (!tmp)
        tmp="none";
      else
        tmp=lower_case(tmp);
      fname=wo+"/"+({string})dir[0];
      farr=explode(fname,"/")-({".",""});
      if (!sizeof(farr)) continue;
      switch (farr[0])
      {
	case WIZARDDIR: 
          group="magier";
          break;
	case DOMAINDIR:
          if ((sizeof(farr)>2) || (isdir && (sizeof(farr)>1)))
            group=farr[1];
          else
            group="sys";
          break;
	case "news":
          group="news";
          break;
	case "mail":
          group="mail";
          break;
	case "open":
          group="public";
          break;
	case "p":
          group="project";
          break;
	default:
          group="sys";
      }
#ifdef OLD_LS_FORMAT
      ret+=sprintf("%c%c%c%c %-11s %-8s %8s %s %s%s\n",
                   isdir?'d':'-',r,w,x,tmp,group,
		   isdir?"-":sprintf("%d",dir[1]), date,dir[0],isdir?"/":"");
#else
      if (flags & SHOWDIR_SHORTPERM)
        ret+=sprintf("%c%c%c%c  %-11s %-8s %8s %s %s%s\n",
                     isdir ? 'd':'-',r,w,x,tmp,group,
                     isdir?"-":to_string(dir[1]), date,dir[0],
                     isdir && (flags & SHOWDIR_SLASHDIR) ? "/" : "");
      else
        ret+=sprintf("%c%c%c%c%c%c%c---   1 %-11s %-8s %8s %s %s%s\n",
                     isdir ? 'd':'-',r,w,x,r,w,x,tmp,group,
                     sprintf("%d",isdir?1024:dir[1]), date,dir[0],
                     isdir && (flags & SHOWDIR_SLASHDIR) ? "/" : "");
      n++;
#endif
    }
  }
#ifdef OLD_LS_FORMAT
  if (!n)
    ret="No files found.\n";
  return ret;
#else
  if (flags & SHOWDIR_NOTOTAL)
    return ret;
  return "total "+n+"\n"+ret;
#endif
}
