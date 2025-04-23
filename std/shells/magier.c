/* Tamedhon MUDLIB
 * Magier - Magier Stufe 20..55
 * Querolin@Tamedhon
 *
 * $Date: 2003/03/05 19:50:00$
 * $Revision: 1.3 $
 * $Log: magier.c,v $
 * Revision 1.3  2003/03/05 19:50:00  Querolin
 * verfolge wieder aktiviert
 * 
 * Revision 1.2  2001/01/18 09:38:11  Samtpfote
 * pwho & people wieder eingebaut
 *
 * Revision 1.1  2001/01/13 20:12:46  Samtpfote
 * Initial revision
 *
 *
 */

#include <properties.h>
#include <wizlevels.h>
#include <moving.h>
#include <udp.h>
#include <exploration.h>
#include <ansi.h>

inherit "/std/shells/lehrling";
inherit "/std/shells/filesys/filesys";
inherit "/std/player/objects";

#define ME		this_object()
#define TO		this_object()

static string my_snooper;
object	snoopee;

/* -- Hilfsfunktionen ----------------------------------------------- */
static string convert_message(string str, object obj)
{
  // efun:: damit @@foo@@ am Anfang kommen kann.
  str = implode(efun::explode(str, "@@wer@@"), obj->name(WER,2));
  str = implode(efun::explode(str, "@@wessen@@"), obj->name(WESSEN,2));
  str = implode(efun::explode(str, "@@wem@@"), obj->name(WEM,2));
  str = implode(efun::explode(str, "@@wen@@"), obj->name(WEN,2));
  str = implode(efun::explode(str, "@@ich@@"), this_object()->name(WER,2));
  return capitalize(str);
}

mixed verfolger()
{
  mixed *pur;

  if (!pointerp(pur=QueryProp(P_PURSUERS)))
    return 0;
  else
    return pur[0];
}

static void initialize() 
{
  filesys::initialize();
}

/* ----------------------------------------------------------------- */

static string prettyfile(string str) {
  if (!str) return 0;
  if (str[0..2] == "/d/") return "+"+str[3..<1];
  if (str[0..8] == "/players/") return "~"+str[9..<1];
  return str;
}

static int cmp_user(object i,object j)
{
  return query_ip_number(i) > query_ip_number(j);
}

static int cmd_list_peoples_wiz() 
{
  mixed *list, res;
  int i, a;
  string a_age,a_ipnum,a_name,a_level,a_idle,a_room,a_end, a_title;

  switch(QueryProp("tty")) {
    case "vt100":
    a_ipnum = ""; a_name = ANSI_BOLD; a_level = ANSI_NORMAL;
    a_idle = ANSI_BLINK; a_room = ANSI_NORMAL; a_end = ANSI_NORMAL;
    a_title = ANSI_INVERS; a_age = ANSI_NORMAL;
    break;
    case "ansi":
    a_ipnum = ANSI_GREEN; a_name = ANSI_BOLD; a_level = ANSI_RED;
    a_idle = ANSI_BLACK+ANSI_BOLD; a_room = ANSI_BOLD+ANSI_BLUE;
    a_end = ANSI_NORMAL;
    a_title = ANSI_INVERS; a_age = ANSI_PURPLE;
    break;
    default:
    a_title = a_ipnum = a_name = a_level = a_idle = a_room = a_end = "";
    a_age = "";
  }

  list = sort_array( users(), #'cmp_user );//');
  res = a_title+"There are now " + sizeof(list) + " players";
  for (i=0, a=0; i < sizeof(list); i++)
    if (query_idle(list[i]) >= 5 * 60)
      a++;
  if (a)
    res += " (" + (sizeof(list) - a) + " active)";
  res += ". " + query_load_average() + a_end+ "\n";
  for(i=0; i<sizeof(list); i++) {
    string name_;
    name_ = list[i]->query_real_name();
    if (!name_)
      name_ = "<logon>";
    name_ = capitalize(name_);
    if (list[i]->QueryProp(P_INVIS))
      name_ = "(" + name_ + ")";
    res += sprintf( "%s%-15s%s %s%-13s%s %s%3d%s %s",
	   a_ipnum, query_ip_number(list[i]),a_end,
	   a_name, name_,a_end,
	   a_level, MASTER->get_wiz_level(getuid(list[i])),a_end,
	   a_age
	   );
    res += time2string("%3x %0X",list[i]->QueryProp(P_AGE)*2);
    if (query_idle(list[i]) >= 5 * 60)
      res += a_idle+" I";
    else
      res += "  ";
    res += a_end;
    if (query_editing(list[i]))
      res += a_idle+"E"+a_end;
    else
      res += " ";

    if (query_input_pending(list[i]))
      res += a_idle+"M "+a_end;
    else
      res += "  ";

    if (environment(list[i]))
      res += a_room+prettyfile(object_name(environment(list[i])));
    res += a_end+"\n";
  }
  ME->More(res);

  return 1;
}


mixed level_sort(mixed list)
{
  mixed *sorted_arr;
  int i;
  sorted_arr = ({ ({ }),({ }) });
  for (i=sizeof(list)-1;i>=0;i--)
  {
    sorted_arr[0] += ({ list[i]->QueryProp(P_LEVEL) });
    sorted_arr[1] += ({ list[i] });
  }
  sorted_arr = order_alist(sorted_arr);
  return sorted_arr[1];
}

static int cmd_pwho()
{
  object* user;
  mixed* spieler, res;
  int i;
  mixed *hands;
  if (!IS_WIZARD(this_player()))
    return 0;
  user = users();
  spieler = ({ });
  for (i=sizeof(user)-1; i>=0; i--)
    if (!IS_LEARNER(user[i]))
      spieler += ({ user[i] });
  spieler = level_sort(spieler);

  res = "Lvl Name         Erfahrung   QP  Int Str Dex Con   WC   AC     FP   HP (max)\n";
  //     xxx xxxxxxxxxxxx xxxxxxxxx xxxx  xxx xxx xxx xxx xxxx xxxx  xxxxx  xxx (xxx)
  res += "------------------------------------------------------------------------------\n";
  for (i=sizeof(spieler)-1; i>=0; i--)
    res += sprintf("%3d %-12s %9d %4d  %3d %3d %3d %3d %4d %4d  %5d  %3d (%3d)\n",
	   spieler[i]->QueryProp(P_LEVEL),
	   capitalize(getuid(spieler[i])),
	   spieler[i]->QueryProp(P_XP),
	   spieler[i]->QueryProp(P_QP),
	   spieler[i]->QueryAttribute(A_INT),
	   spieler[i]->QueryAttribute(A_STR),
	   spieler[i]->QueryAttribute(A_DEX),
	   spieler[i]->QueryAttribute(A_CON),
	   spieler[i]->QueryProp(P_TOTAL_WC),
	   spieler[i]->QueryProp(P_TOTAL_AC),
	   spieler[i]->QueryProp(P_EP_NUM),
//	   (sizeof(hands=spieler[i]->QueryProp(P_HANDS))?hands[1]:0),
	   spieler[i]->QueryProp(P_HP),
	   spieler[i]->QueryProp(P_MAX_HP));
  ME->More(res);
  return 1;
}


static int cmd_zap(string str)
{
  object	opfer;
  string	*message;

  if(!str || !QueryProp(P_WANTS_TO_LEARN)) { return 0; }

  if(opfer=present(str, environment()))
  {
    if(living(opfer))
    {
      message = QueryProp(P_ZAP_MSG);
      if(!pointerp(message) || sizeof(message)!=3)
      {
		say(this_object()->Name() +" beschwört einen Blitz vom Himmel.\n");
		write("Du zappst "+opfer->name(WEN,2)+".\n");
      }
      else
      {
		write(convert_message(message[0], opfer));
		say(convert_message(message[1], opfer), ({this_player(), opfer}));
		tell_object(opfer, convert_message(message[2], opfer));
      }
      opfer->die();
      return 1;
    }
    else
    {
      write("Aber "+capitalize(str)+" lebt nicht.\n");
      return 1;
    }
  }
  else
  {
    write("Sowas siehst Du hier nicht.\n");
    return 1;
  }
}

static int cmd_peace(string sname)
{
  object	obj, *enem;
  string	myself, him;

  if(!QueryProp(P_WANTS_TO_LEARN)) { return 0; }
  myself=funcall(#'name, WER); 
  if (!sname)
  {
	say(myself+" stiftet Frieden.\n");
    for(obj=first_inventory(environment()); obj; obj=next_inventory(obj))
	{ obj->StopHuntingMode(); }
  }
  else
  {
    obj=find_living(sname);
    if(!obj)
    { 
      write("Kein solches Lebewesen im Spiel.\n");
      return 1;
    }
    him=obj->name(WEM);
    for(enem=({object *})obj->StopHuntingMode()[0];sizeof(enem);enem=enem[1..])
    {
      enem[0]->StopHuntFor(obj);
      tell_object(obj,myself+" beendet Deinen Kampf mit "+enem[0]->name(WEM)+".\n");
      tell_object(enem[0],myself+" beendet Deinen Kampf mit "+him+".\n");
    }
  }
  write("Ok.\n");
  return 1;
}

int cmd_verfolge(string str)
{
  mixed *pur, lv,lv2;
  
  if(!QueryProp(P_WANTS_TO_LEARN)) { return 0; }
  if(!str || str=="")
  {
    write("Verfolgungsmodus abgeschaltet.\n");
    if (!pointerp(pur=Query(P_PURSUERS)))
      return 1;
    if (pur[0])
      pur[0]->RemovePursuer(ME);
    return 1;
  }
  str=lower_case(str);
  if (intp(lv=match_living(str))||!lv)
  {
    write("Kein solches Wesen gefunden.\n");
    return 1;
  }
  if (objectp(lv))
    lv2=lv;
  else
  {
    lv2=find_player(lv);
    if (!lv2)
      lv2=find_living(lv);
  }
  if (!lv2)
  {
    write("Hm, irgendwas geht schief :(\n");
    return 1;
  }
  lv2->AddPursuer(ME);
  write("Ok, Du verfolgst jetzt "+lv2->name(WEN)+".\n");
  lv2->TakeFollowers();
  return 1;
}

static int cmd_allow_snoop(string str)
{ 
  object ob;
  
  if (!str)
  { 
    if (!my_snooper) return 0;
    str=my_snooper;
    my_snooper=0;
    write("Ok, "+capitalize(str)+" Erlaubnis zum Snoopen entzogen.\n");
    ob=query_snoop(this_player());
    if (!ob) return 1;
    if (getuid(ob) != str) return 1;
    tell_object(ob,
      ME->Name(WER)+" zwingt Dich, mit dem Snoopen aufzuhören.\n");
    snoop(ob);
    return 1;
  }
  if (my_snooper) cmd_allow_snoop(0); /* Remove the other first ! */
  str=lower_case(str);
  ob=find_player(str);
  if (!ob){
    write("Kein solcher Spieler.\n");
    return 1;
  }
  if (query_wiz_grp(ob)>query_wiz_grp(this_player())){
    write("Level des Snoopers größer als Deiner.\n");
    return 1;
  }
  write("Ok !\n");
  my_snooper=str;
  return 1;
}

nomask int QueryAllowSnoop(object who)
{
  return getuid(who) == my_snooper;
}

static int cmd_snoop_on(string str)
{
  object ob;
  string flags, plname;
  int fl;
  
  
  if ((!snoopee) && (!str))
  {
  	write("Du snoopst doch niemanden.\n");
  	return 1;
  }
  
  if (!str) {
    snoop(this_player());  /* Abort snoop */
    tell_object(snoopee, "*** DU WIRST VON "+this_player()->name(WEN)+" NICHT MEHR GESNOOPT! ***\n");
    write_file("/log/SNOOP", dtime(time())+" "+this_player()->name(WER)+
                           " snoopt "+snoopee->name()+" nun nicht mehr.\n");
    snoopee=0;
    return 1;
  }
  if (!QueryProp(P_WANTS_TO_LEARN))
    return 0;
  fl=0;
  if (sscanf(str,"%s %s", plname, flags)==2 && flags!="")
  {
    str=plname;
    while (sizeof(flags)) {
      switch (flags[0])
      {
	case 'l':
	  fl|=SF_LOCKED; 
	  break;
	case 'f':
	  fl|=SF_FORCE;
	  break;
	default:
	  printf("Unbekanntes Flag %s ignoriert.\n",flags[0..0]);
      }
      flags=flags[1..];
    }
  }
  SetProp(P_SNOOPFLAGS,fl);
  ob = find_player(str);
  if (!ob) {
    write("Spieler "+capitalize(str)+" nicht gefunden.\n");
    return 1;
  }
  if (!snoop(this_player(),ob)){
    write("Du kannst "+ob->name(WEN)+" nicht snoopen !\n");
    return 1;
  }
  write("Du snoopst jetzt "+ob->name(WEN)+".\n");
  snoopee=ob;
  write_file("/log/SNOOP",  dtime(time())+" "+this_player()->name(WER)
                           +" snoopt " + ob->name(WEN)+".\n");
  tell_object(ob, "*** DU WIRST VON "+this_player()->name(WEN)+" GESNOOPT! ***\n");
  return 1;
}

static int cmd_trans(string str) 
{
  object living;
  string oldenv;
  if (!QueryProp(P_WANTS_TO_LEARN))
    return 0;
  if (!str) {
    _notify_fail("Trans WEN?\n");
    return 0;
  }
  str=match_living(str,0);
  if (!stringp(str))
    switch (str)
    {
      case -1: write("Das war nicht eindeutig.\n"); return 1;
      case -2: write("So ein Wesen gibt es nicht.\n"); return 1;
    }
  if((living=find_living(str)))
  {
    if(!environment(living))
    {
	tell_object(living,
		"Keine Umgebung für "+living->Name()+" gefunden!\n");
	notify_fail("... failed.\n");
	return 0;
    }
    oldenv=object_name(environment(living));
    tell_object(living,
		"Ein seltsames Gefühl überkommt Dich ...\n"
		+"Du verlierst die Orientierung ...\n");
    if (living->move(object_name(environment(ME)),
        M_TPORT | (query_verb()=="Trans" ? M_NOCHECK : 0) ) <= 0)
    {
      write(living->Name(WER)+" will oder kann nicht kommen!\n");
      tell_object(living, Name(WER)+
		  " wollte Dich teleportieren, hat aber versagt!\n");
      return 1;
    }
    write("OK.\n");
    if (interactive(living) && !IS_LEARNER(living) &&
        !living->QueryProp(P_TESTPLAYER))
      log_file("TRANS", 
          sprintf("%s transt %s von %s nach %s %s\n",
	     capitalize(getuid(ME)), capitalize(getuid(living)),
             oldenv, object_name(environment(ME)), dtime(time())));
  }
  else
    write(capitalize(str)+" nicht gefunden!\n");
  return 1;
}

static int cmd_force_someone(string str)
{
  object living;
  string what, rest;
  string living_name;

  str=this_player()->_unparsed_args();
  if (!QueryProp(P_WANTS_TO_LEARN))
    return 0;
  if( !str || sscanf( str, "%s %s", living_name, what ) != 2 )
  {
    _notify_fail("Zwinge WEN zu WAS?\n");
    return 0;
  }
  if( sscanf( what, "zu %s", rest ) == 1 )
    what = rest;
  if (!(living = find_living(lower_case(living_name))))
  {
    write("Kein solches Lebewesen gefunden!\n");
    return 1;
  }
  if (living->command_me(what))
  {
    write("Hat geklappt!\n");
    if (!IS_LEARNER(living)&&!IS_ARCH(this_object()))
      if( find_living(lower_case(living_name)))
        tell_object(living,sprintf("%s zwingt Dich zu: %s\n",
				 this_object()->Name(WER), what));
    if (!living ||
        (!IS_LEARNER(living) && !living->QueryProp(P_TESTPLAYER)))
      log_file("ZWINGE", 
	     sprintf("%s zwingt %s %s\n", 
		     capitalize(geteuid(this_player())),str,
		     dtime(time())));
  }
  else
    write("Hat nicht geklappt!\n");
  return 1;
}

static int cmd_echo_all(string str)
{
  mixed *u;
  int i;

  if (!QueryProp(P_WANTS_TO_LEARN))
    return 0;
  if (!(str=_unparsed_args())) { // 
    write("Was möchtest Du 'echoallen'?\n");
    return 1;
  }
  u=users();
  for (i=sizeof(u)-1; i>=0; i--)
  {
    if (IS_ARCH(u[i]) && (u[i]->QueryProp(P_WANTS_TO_LEARN) >= 2))
      tell_object(u[i], "[Echoall:"+capitalize(getuid())+"] ");
  }
  shout( str + "\n");
  write( str + "\n");
  return 1;
}

static int cmd_echo_to(string str)
{
  object ob;
  string who;
  string msg;

  if (!QueryProp(P_WANTS_TO_LEARN))
    return 0;
  if (!(str=_unparsed_args()) || sscanf(str, "%s %s", who, msg) != 2) {
    write("Was möchtest Du 'echoen' ?\n");
    return 1;
  }
  ob = find_living(lower_case(who));
  if (!ob) {
    write("Kein Spieler mit diesem Namen eingeloggt.\n");
    return 1;
  }
  tell_object(ob, msg + "\n");
  write( ob->name(WEN)+"->" + msg + "\n");
  return 1;
}

static int cmd_mecho_all(string str)  {
  object *who;
  int i;

  if (!QueryProp(P_WANTS_TO_LEARN))
    return 0;
  _notify_fail("Was möchtest Du denn allen Magiern zukommen lassen?\n");
  if (!(str=_unparsed_args())) return 0;
  who=users();
  for (i=0; i<sizeof(who); i++)
    if (IS_LEARNER(who[i]))
      tell_object(who[i], break_string(str,78));
  return 1;
}

static int cmd_in_room(string str)
{
  string room;
  object old_room;
  string cmd,err;
  
  if (!(str=_unparsed_args()) || !QueryProp(P_WANTS_TO_LEARN))
    return 0;
  if (sscanf(str, "%s %s", room, cmd) != 2) {
    write("Usage: in ROOM CMD\n");
    return 1;
  }
  if (!room) {
    write("Fehlerhafter Dateiname.\n");
    return 1;
  }
  old_room = environment();
  room="/secure/master"->_get_path(room,getuid());
  if (file_size(room+".c")<0)
    write(room+".c: No such file - trying virtual room ...\n");
  if (err=catch(move_object(room)))
  {
    write("Error moving to room:"+err+"\n");
    return 1;
  }
  if (environment()!=find_object(room) && IS_ARCH(ME))
  {
    write("Move failed - trying efun308 ... ");
    __set_environment(ME,find_object(room));
    if (environment()!=find_object(room))
    {
      write("failed.\n");
      return 1;
    }
    write("suceeded.\n");
  } 
  command(cmd);
  if (old_room) move_object(old_room);
  else {write("Rückkehr nicht gelungen.\n");return 1;}
  if (environment()!=old_room && IS_ARCH(ME))
  {
    write("Move back failed, trying efun308 ... ");
    __set_environment(ME,old_room);
    if (environment()!=old_room)
      write("failed.\n");
    else
      write("suceeded.\n");
  }
  return 1;
}

static int cmd_at_player(mixed dest)
{
  object ob,old_room;
  mixed ret;
  string cmd;
  
  if (!QueryProp(P_WANTS_TO_LEARN))
    return 0;
  if (!(dest=_unparsed_args())) {
    write("Wohin möchtest Du Dich teleportieren?\n");
    return 1;
  }
  if (sscanf(dest, "%s %s", dest, cmd) != 2) {
    write("Usage: at LIVING CMD\n");
    return 1;
  }
  ob = find_living(dest);
  if (!ob)
  {
    dest=match_living(dest,1);
    if (stringp(dest))
      dest = find_living(dest);
    if (objectp(dest))
      ob=environment(dest);
    else
      ob=0;
  } else
    ob=environment(ob);
  if (!ob)
  {
    _notify_fail("Lebewesen nicht gefunden.\n");
    return 0;
  }
  old_room=environment(this_object());
  move_object(ob);
  if (environment()!=ob && IS_ARCH(ME))
  {
    write("Move failed, trying efun308 ... ");
    __set_environment(ME,ob);
    if (environment()!=ob)
    {
      write("failed.\n");
      return 1;
    }
    write("suceeded.\n");
  }
  command(cmd);
  if (old_room) move_object(old_room);
  else write("Rückkehr nicht gelungen.\n");
  if (environment()!=old_room && IS_ARCH(ME))
  {
    write("Move back failed, trying efun308 ... ");
    __set_environment(ME,old_room);
    if (environment()!=old_room)
      write("failed.\n");
    else
      write("suceeded.\n");
  }
  return 1;
}

static int cmd_teleport(string dest) {
  object ob;
  string path;
  mixed ret;

  if (!QueryProp(P_WANTS_TO_LEARN))
    return 0;
  if (!(dest=_unparsed_args())) {
    write("Wohin möchtest Du Dich teleportieren?\n");
    return 1;
  }
  if (verfolger())
    cmd_verfolge("");
  if ((ob=find_living(dest)) && (ob=environment(ob))) {
    move(ob,M_TPORT|M_NOCHECK);
    return 1;
  }
  path=MASTER->_get_path(dest,getuid(this_object()));
  ret=catch(call_other(path,"?"));
  if (ret && file_size(path)<0)
  {
    dest=match_living(dest,1);
    if (stringp(dest) && (ob=environment(find_living(dest)))) {
      move(ob,M_TPORT|M_NOCHECK);
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

static int cmd_heal(string str)
{
  object ob;
  string it;

  if (!QueryProp(P_WANTS_TO_LEARN))
    return 0;
  if (!str) return 0;
  it = lower_case(str);
  ob = present(it,environment());
  if (!ob || !living(ob))
    ob = find_living(it);
  if (!ob) {
    write("Dieser Spieler ist momentan nicht da.\n");
    return 1;
  }
  if (!IS_LEARNER(ob) && !ob->QueryProp(P_TESTPLAYER))
    log_file("HEAL", 
	     sprintf("%s heilt %s (%s) %s\n", 
		     capitalize(geteuid(ME)),
		     ob->name(WEN), capitalize(geteuid(ob)),
		     dtime(time())));
  ob->heal_self(1000000);
  tell_object(ob, QueryProp(P_NAME) + " heilt Dich.\n");
  write("Ok.\n");
  return 1;
}

static int cmd_go_home() 
{
  if (!QueryProp(P_WANTS_TO_LEARN))
    return 0;
  return move("players/" + getuid() + "/workroom", 
                   M_GO, "nach Hause");
}

static int cmd_go_wiz_home(string str)
{
  int f;

  if (!QueryProp(P_WANTS_TO_LEARN))
    return 0;
  str=query_verb();
  if (str[0]!='+') return 0;
  str=str[1..];
  str=explode(str," ")[0];
  if (catch(f=file_size("/players/"+str+"/workroom.c")) || f<=0)
  {
    printf("%shat keinen Workroom.\n",capitalize(str+" "));
    return 1;
  }
  if (catch(call_other("/players/"+str+"/workroom","???")))
  {
    write("Der Workroom hat Fehler.\n");
    return 1;
  }
  this_object()->move("/players/"+str+"/workroom",M_TPORT|M_NOCHECK);
  return 1;
}

static int cmd_protect(string str)
{
  if (!QueryProp(P_WANTS_TO_LEARN))
    return 0;
  if (this_object() != this_interactive())
    return 0;
  if (!str || str=="")
  {
    write("\"protect <propstring>\" toggelt das Protected-Flag.\n");
    return 1;
  }
  if (!Query(str) && !Query(str,F_MODE) && !Query(str,F_SET_METHOD) &&
      !Query(str,F_QUERY_METHOD))
  {
    write("Alle Werte dieser Property sind auf 0. Nichts gemacht.\n");
    return 1;
  }
  Set(str, PROTECTED, F_MODE);
  write(str+" ist jetzt "+(Query(str,F_MODE) & PROTECTED?"":"nicht ")+
	"protected.\n");
  return 1;
}

static int cmd_udpq(string str)
{
  string ret, mud, type;
  
  if (!QueryProp(P_WANTS_TO_LEARN))
    return 0;
  if (!(str=this_interactive()->_unparsed_args()) || str=="" ||
      sscanf(str,"%s %s",mud,type)<2)
  {
    write("Syntax: udpq mud type\n");
    return 1;
  }
  if (member(({"commands","email","encoding","hosts","inetd","list","mud_port","time",
	       "version"}),type)==-1)
    write("TYPEs: commands, email, encoding, hosts, inetd, list, "
        "mud_port, time, version\n");
  if (ret=INETD->send_udp(mud,
      ([SENDER:getuid(this_object()), REQUEST:QUERY, DATA:type]),1))
    write(ret);
  else
    write("Anfrage abgeschickt.\n");
  return 1;
}

static int cmd_ping(string str)
{
  object pl;

  if (!stringp(str)||!sizeof(str))
  {
    _notify_fail("Usage: ping <spielername>\n");
    return 0;
  }
  str=lower_case(str);
  if (!(pl=find_player(str)))
  {
    _notify_fail("Spieler "+capitalize(str)+" nicht gefunden.\n");
    return 0;
  }
  tell_object(pl,sprintf("%s pingt Dich an.\a\n",Name(WER)));
  write("Ok.\n");
  return 1;
}


// ---------
static mixed *_query_localcmds()
{
  return
	lehrling::_query_localcmds()
	+objects::_query_localcmds()
	+
	({
		({"zap", "cmd_zap", 0, WIZARD_LVL }),
		({"frieden", "cmd_peace", 0, WIZARD_LVL }),
		({"sallow", "cmd_allow_snoop", 0, FULL_WIZARD_LVL }),
		({"snoop", "cmd_snoop_on", 0, FULL_WIZARD_LVL }), 
		({"trans", "cmd_trans", 0, WIZARD_LVL }),
		({"zwinge", "cmd_force_someone", 0, FULL_WIZARD_LVL }),
		({"echoto", "cmd_echo_to", 0, WIZARD_LVL }),
		({"echoall", "cmd_echo_all", 0, WIZARD_LVL }),
		({"mecho", "cmd_mecho_all", 0, WIZARD_LVL }),
		({"in", "cmd_in_room", 0, FULL_WIZARD_LVL }),
		({"at", "cmd_at_player", 0, FULL_WIZARD_LVL }),
		({"heal", "cmd_heal", 0, WIZARD_LVL }),
		({"heile", "cmd_heal", 0, WIZARD_LVL }),
		({"home", "cmd_go_home", 0, WIZARD_LVL }),
		({"+", "cmd_go_wiz_home", 1, WIZARD_LVL }),
		({"protect", "cmd_protect", 0, FULL_WIZARD_LVL }),
		({"pwho", "cmd_pwho", 0, FULL_WIZARD_LVL }),
		({"people", "cmd_list_peoples_wiz", 0, QUESTWIZ_LVL }),
		({"goto", "cmd_teleport", 0, WIZARD_LVL}),
		({"udpq", "cmd_udpq", 0, WIZARD_LVL }),
		({"ping", "cmd_ping", 0, WIZARD_LVL }),
                ({"verfolge", "cmd_verfolge", 0, FULL_WIZARD_LVL })
	});
	
}

/* ---------------------------------------------------------------- */
static void FinalSetup()
{
  string err,filename;
  object ob;

  SetProp(P_CURRENTDIR,"/players/"+getuid());
  initialize();
}  

varargs int remove()
{
  string workroom;

  workroom = "/players/"+getuid()+"/workroom";
  if( !environment() || object_name(environment()) != workroom )
    catch(move(workroom, M_GO, "nach Hause"));
  return lehrling::remove();
}

void create()
{
  lehrling::create();
  
  snoopee=0;
  
  SetDefaultHome("gilden/abenteurer");
  SetProp(P_ENEMY_DEATH_SEQUENCE,
	([17:"Der Tod schüttelt verständnislos den Kopf.\n\n",
	  18:"Der Tod sagt: WIESO MUSSTEST DU DICH AUCH UNBEDINGT MIT EINEM "+
		 "MAGIER ANLEGEN?\n\n"]));							
}
