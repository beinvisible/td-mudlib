/* Tamedhon MUDLIB
 *
 * /secure/master.c -- master object
 * 
 * 2024/05/11 preload fkt vom WL
 * 2024/04/09 - diverse driverconfigs vom MG uebernommen, ausgabe location an neues tame angepasst. set_auto_include_string ersetzt. efun308 ersetzt - Aloy
 * 
 *
 * $Date: 2001/01/18 10:41:18 $
 * $Revision: 1.2 $
 * $Log: master.c,v $
 * Revision 1.2  2001/01/18 10:41:18  Samtpfote
 * die Dateizugriffsfunktionen auf master/fileaccess.c ausgelagert.
 *
 * Revision 1.1  2001/01/18 10:33:14  Samtpfote
 * Initial revision
 *
 * Revision 2.4  1998/02/01 23:52:15  Woody
 * /secure/LORD/ nur fuer Regionsmagier lesbar
 *
 * Revision 2.3  1998/01/26 10:32:00  Crea
 * added /event for access_rights handling
 *
 * Revision 2.2  1997/11/09 19:21:10  Woody
 * socket daemon hat erq privileges
 *
 * Revision 2.1  1997/03/19 19:50:45  Woody
 * Zugriffsrechte auf /d/ gefixt (valid_write())
 *
 * Revision 2.0  1997/03/15 23:12:51  Woody
 * prepared for revision control
 *
 * Revision 1.22 SL 6  1997/02/27  Woody@SilberLand
 * /ChangeLog auch fuer IS_ELDER schreibbar
 *
 * Revision 1.21 SL 5  1996/10/19  Woody@SilberLand
 * /log/report/*.ep nur mehr fuer Erzmagier lesbar
 *
 * Revision 1.20 SL 4  1996/09/30  Woody@SilberLand
 * Format der Laufzeitfehler geaendert.
 *
 * Revision 1.20 SL 3  1996/09/24  Woody@SilberLand
 * /secure/ARCH/ erzmagier-only gemacht.
 *
 * Revision 1.20 SL 2  1996/01/23  Woody@SilberLand
 * Automatischer Artikel bei selbstloeschung (master/userinfo)
 *
 * Revision 1.20 SL 1  1996/01/23  Woody@SilberLand
 * login text ("University...")
 *
 * Revision 1.20 AL 2 23.1.96 Gundur@anderland  Security :-))
 *                            Hadra             noch was erweitert :)
 *
 * Revision 1.20 AL 1  13-09-1995  Mupfel@Anderland
 * 'creator_file' auf /p/weapon und /p/armour vorbereitet
 *
 * Revision 1.20  1995/06/26  11:33:18  Jof
 * mofify_command ...
 *
 * Revision 1.19  1995/05/08  09:22:33  Jof
 * *** empty log message ***
 *
 * Revision 1.18  1995/05/05  18:06:15  Jof
 * *** empty log message ***
 *
 * Revision 1.17  1995/04/21  18:03:31  Jof
 * Debugging mail
 *
 * Revision 1.16  1995/04/10  15:28:51  Jof
 * removed some 'catch'es from runtime_error
 *
 * Revision 1.15  1995/03/30  13:46:11  Jof
 * Bugfixed.
 *
 * Revision 1.14  1995/03/06  21:10:02  Jof
 * ???
 *
 * Revision 1.13  1995/02/28  10:44:58  Jof
 * Timeouts for IPNAME-Cache
 *
 * Revision 1.13  1995/02/28  17:41:15  Jof
 * timeouts for ipnames
 *
 * Revision 1.12  1995/02/20  10:14:10  Jof
 * Bugs removed
 *
 * Revision 1.12  1995/02/20  17:51:43  Jof
 * Removed some logging
 *
 * Revision 1.11  1995/02/06  22:32:21  Jof
 * naseftpsupport -> network.c
 *
 * Revision 1.10  1995/01/03  12:31:35  Kirk
 * /secure/save_inactive now read-protected
 *
 * Revision 1.9  1994/12/23  12:20:22  Jof
 * *** empty log message ***
 *
 * Revision 1.8  1994/11/30  01:18:49  Jof
 * *** empty log message ***
 *
 * Revision 1.7  1994/11/14  08:12:14  Jof
 * *** empty log message ***
 *
 * Revision 1.6  1994/11/14  08:10:42  Jof
 * *** empty log message ***
 *
 * Revision 1.5  1994/11/07  22:09:04  Jof
 * *** empty log message ***
 *
 * Revision 1.4  1994/10/24  15:42:28  Kirk
 * added remove()
 *
 * Revision 1.3  1994/10/24  08:54:27  Jof
 * *** empty log message ***
 *
 * Revision 1.2  1994/10/07  18:15:48  Jof
 * *** empty log message ***
 *
 * Revision 1.1  1994/09/23  09:08:16  Jof
 * Initial revision
 *
 * Revision 1.29  1994/09/19  18:31:29  Jof
 * *** empty log message ***
 *
 * Revision 1.28  1994/09/12  08:49:24  Jof
 * ueberarbeitung
 *
 * Revision 1.28  1994/09/11  16:11:18  mud
 * *** empty log message ***
 *
 * Revision 1.27  1994/09/11  15:53:53  Jof
 * *** empty log message ***
 *
 *
 */

#pragma strong_types

inherit "/secure/master/misc";
inherit "/secure/master/userinfo";
inherit "/secure/master/network";
inherit "/secure/master/domain";
inherit "/secure/master/fileaccess";

#include "/secure/master.h"

static mapping ip_map;

string *define_include_dirs() {
  return ({ "secure/%s" , "sys/%s", "room/%s" });
}

void flag(string str) {}

object connect() { // Einloggende Spieler landen hier
  string err;
  object ob,bp;

#if defined (TLS_PORT)
  mixed errno;
  if (efun::interactive_info(efun::this_player(), II_MUD_PORT) == TLS_PORT) {
    // reject connection of TLS is not available
    if (!efun::tls_available())
      return 0;
    // establish TLS
    errno=efun::tls_init_connection(this_object());
    if (errno < 0) {
      // Fehler im Vebindungsaufbau
      printf("Can't establish a TLS/SSL encrypted connection: %s\n",
          tls_error(errno));
      return 0; // this will close the connection to the client.
    }
  }

#endif

  /*
  printf(
    "HTTP/1.0 302 Found\nLocation: http://atpforest.tuwien.ac.at/~mud/\n\n"
    "Vienna University of Technology, Austria.\nLocal time: %s MET\n\n"
    "SilberLand LPmud, SilberLand Mudlib, NATIVE mode, driver version %s\n\n",
    ctime(time()),__VERSION__);
  */

  printf("\nLokale Zeit: %s\n\n"+
         "Tamedhon LDmud driver %s.\n\n",
         ctime(time()),__VERSION__);

  /*
  write (
    "EMERGENCY CLOSING.\n---\n"
    "Due to severe problems, the mud will be down for some time.\n"
    "We try to do our best to get it up and running as soon as possible.\n"
    "Sorry for any inconvenience this causes to you. Please retry later!\n");
    return (object) 0;
  */

  if ((bp=find_object("/secure/login"))&&environment(bp)) catch(destruct(bp));
  err = catch(ob = clone_object("secure/login"));
  if (err) write("Fehler beim Laden von /secure/login.c\n"+err+"\n");
  return ob;
}

string get_master_uid()  {return ROOTID;}

string get_wiz_name(string file) {return creator_file(file);}

string *epilog(int eflag) {
  string *files, *domains;
  int i;

  efun::configure_object(this_object(), OC_EUID, ROOTID);
  
  seteuid(ROOTID);
  ReloadBanishFile();
  
  if (eflag) {
    write("-e angegeben -> Preloading unterbunden ...\n");
    return 0;
  }
  
  printf(ANSI_PURPLE+ANSI_BOLD"Preloading gestartet: %s\n\n"+ANSI_NORMAL,ctime(time()));
  files=explode_files("/std/preload_file")+explode_files("/d/preload_file");
  domains=get_domains();
  for (i=sizeof(domains);i--;)
    files+=explode_files("/d/"+domains[i]+"/preload_file");
  write("\n");
  return files;
}

void preload(string file) {

  // trace(TRACE_CALL|TRACE_CALL_OTHER|TRACE_RETURN|TRACE_ARGS);
  string err;
  string name;
  int *res, t1;
  
  if(!file || !file[0] || file[0] == ';') return;
  
  printf(ANSI_BLUE+ANSI_BOLD+"%-50s"+ANSI_NORMAL,file);
  if(file_size(file+".c") < 0) {
    write(ANSI_RED+"M I S S I N G\n"+ANSI_NORMAL);
    return;
  } 
  
  if (!(name=creator_file(file))) {
    write("\nno creator found for "+file+"\n");
    return;
  }
  printf(ANSI_YELLOW+ANSI_BOLD+"%-15s"+ANSI_NORMAL,name);
  if (!seteuid(name)) {
    write("\ncannot set euid to "+name+"\n"+"EUID is now "+geteuid(TO)+"\n");
    return;
  }
  res = rusage();
  t1 = res[0] + res[1];
  err = catch(call_other(file, "??"));
  if (err != 0) {
    write(ANSI_RED+ANSI_BOLD+"\nGot error " + err + " when loading " + file + "\n"+ANSI_NORMAL);
  } else {
    res=rusage();
    t1=res[0]+res[1]-t1;
    printf(ANSI_GREEN"(%2d.%:02d s)\n"+ANSI_NORMAL,t1/1000,t1%1000);
  }
  seteuid(ROOTID);
}

int valid_seteuid(object ob, string neweuid) {
  if (ob==this_object() ||
      getuid(ob) == ROOTID || getuid(ob) == neweuid ||
      creator_file(object_name(ob)) == neweuid)
    return 1;
  return 0;
}

static string get_simul_efun() {
  if (!catch(SIMUL_EFUN_FILE->start_simul_efun()))
    return SIMUL_EFUN_FILE;
  write("Failed to load " + SIMUL_EFUN_FILE + "\n");
  if (!catch(SPARE_SIMUL_EFUN_FILE->start_simul_efun()))
    return SPARE_SIMUL_EFUN_FILE;
  write("Failed to load " + SPARE_SIMUL_EFUN_FILE + "\n");
  efun::shutdown();
  return 0;
}

int valid_trace(object player, int trace_lvl) {
  return IS_WIZARD(player);
}

string make_path_absolute(string str) {
  return _get_path(str, getuid(TP));
}

int query_allow_shadow(object ob) {
	string f;

  if (ob==TO)
    return 0;
  if(getuid(ob) == ROOTID)
    return 0;
	f=explode(object_name(ob),"#")[0];
	if (f[-14..]=="/access_rights") return 0;
  return !({int})ob->query_prevent_shadow(PO);
}

string get_ed_buffer_save_object_name(string file) {
  string *file_ar;
  
  file_ar=efun::explode(file, "/");
  file=file_ar[sizeof(file_ar)-1];
  return "/players/"+getuid(TP)+"/.dead_ed_files/"+file;
}

int query_player_level(string what) {
  if(!TP) return 0;
  switch(what) {
    case "wizard":
    case "trace":
    return IS_ELDER(TP);
    case "error messages":
    return IS_LEARNER(TP);
  }
  return 0;
}

int log_error(string file,string message) {
  string lfile;
  string cr;
  
  cr=creator_file(file);
  if (!cr)
    cr="NOBODY";
  if (cr==ROOTID)
    lfile="/log/ROOT.err";
  else if (member(cr,' ')!=-1)
    lfile="/log/STD.err";
  if (!lfile)
  {
    if (!cr)
      lfile="/log/SYSERROR";
    else if (file_size("/players/"+cr) == -2)
      lfile = "/players/"+cr+"/.err";
    else if (file_size("/d/"+cr) == -2)
      lfile = "/d/"+cr+"/.err";
    else
      lfile="/log/"+cr+".err";
  }
  write_file(lfile,message);
  if (IS_LEARNER(TI))
    write(message);
  return 1;
}

void runtime_error(string err ,string prg, string curobj, int line) {
  string code;
  string mkzmsg;
  object ob;
  mixed trace;
  int n;

  // Fehlerquelle evt umbiegen
  if (strlen(err) > 1 && err[0] == '@') {
#if __EFUN_DEFINED__(debug_info)
    trace = debug_info(DINFO_TRACE, DIT_UNCAUGHT_ERROR) - ({ 0 });
#else
    trace = driver_info(DI_TRACE_LAST_UNCAUGHT_ERROR) - ({ 0 });
#endif
    if (err[1] >= '0' && err[1] <= '9') {
      n = err[1] - '0';
      err = err[2..];
    } else {
      n = 1;
      err = err[1..];
    }
    n = sizeof(trace) - 1 - n;
    if (n < 0) n = 0;

    if (sizeof(trace) > 0 && pointerp(trace = trace[n])) {
      prg    = trace[TRACE_PROGRAM];
      curobj = trace[TRACE_OBJECT];
      line   = trace[TRACE_LOC];
    }
  }
  
  if (err[<1] == '\n') err = err[0..<2];
  if (TI) mkzmsg=getuid(TI);
  else mkzmsg="noone";
  if (query_command()) mkzmsg+=":"+query_command();
  if (!curobj) curobj="";
  if (!prg) prg="<driver internal>";
  else if (prg[0]!='<') prg = "/" + prg;
  curobj = "/" + curobj;
  mkzmsg = sprintf("!<%s>::%s::%s::%d::%s",mkzmsg,prg,curobj,line,err);
  DEBUG_MSG(mkzmsg);  

  if (!TI) return;
  if (query_player_level("error messages")) {
    write("---------------------------------------------------------------"
      "---------------\n"+"file: "+prg+" line: "+line+" object: "+curobj+"\n");
    if (prg && code=read_file("/"+prg, line, 1))
      write("\n"+code+"\n");
    write("error: "+err+"\n-------------------------------------------------"
      "----------------------------\n");
  }
  else
    write("Du siehst einen Fehler im Raum-Zeit-Kontinuum.\n");

  if (err=="Object the closure was bound to has been destructed") {
    ob=find_object(curobj);
    if (ob && IS_LEARNER(ob) && query_once_interactive(ob))
    {
      ob->quit();
      if (ob)
	ob->remove();
      if (ob)
	destruct(ob);
    }
  }
}

//int valid_exec(string name) {return 1;}
int valid_exec(string name, object ob, object obfrom)
{
  // Ungueltige Parameter oder Aufruf durch process_string -> Abbruch
  if (!objectp(ob) || !objectp(obfrom) 
      || !stringp(name) || !sizeof(name)
      || funcall(symbol_function('process_call)) )
    return 0;

  // renew_player_object() darf ...
  if (name=="secure/master.c" || name=="/secure/master.c")
    return 1;

  // Ansonsten darf sich nur die Shell selber aendern ...
  if (previous_object() != obfrom) return 0;

  // Die Login-Shell zu jedem Objekt ...
  if (name=="/secure/login.c"
      || name=="secure/login.c")
    return 1;

  // Magier per exec nur, wenn sie damit keine Fremde uid/euid bekommen
  if (this_interactive() == obfrom && getuid(obfrom) == getuid(ob) 
      && geteuid(obfrom) == geteuid(ob)) 
      return 1;

  // Sonst darf niemand was
  return 0;
}

int save_ed_setup(object who, int code) {
  string file;
  
  if (!intp(code))
    return 0;
  file = "/players/" + geteuid(who) + "/.edrc";
  rm(file);
  return write_file(file, code + "");
}

int retrieve_ed_setup(object who) {
  string file;
  int code;
  
  file = "/players/" + getuid(who) + "/.edrc";
  if (file_size(file) <= 0)
    return 0;
  sscanf(read_file(file), "%d", code);
  return code;
}

void destruct_environment_of(object ob) {
  if (!query_once_interactive(ob) && object_name(ob)[0..10]!="/std/shells")
    return;
  tell_object(ob, "Du landest im unendlichen Nichts, nachdem die Welt um Dich zerbrochen ist\n");
  if(catch(ob->move("/room/void",M_TPORT|M_NOCHECK,0,"materialisiert")) && ob)
    destruct(ob);
}

void move_or_destruct(object what, object to) {
  object old_env;
  
  old_env=environment(what);
  if(catch(what->move(to, M_NOCHECK)))
    destruct(what);
  else if (what && environment(what)==old_env)
    destruct(what);
}

mixed prepare_destruct(object ob) {
  object super;
  mixed *errors;
  int i;
  
  if (!ob)
    return 1;
  super = environment(ob);
  if (!super) {
    object item;
    
    while ( item = first_inventory(ob) ) {
      destruct_environment_of(item);
      if (item && environment(item) == ob) destruct(item);
    }
  } else 
    while ( first_inventory(ob) )
      move_or_destruct(first_inventory(ob), super);
  return 0; /* success */
}

int valid_snoop(object me, object you) {
  return getuid(PO) == ROOTID;
}

int valid_query_snoop(object wiz) {
  return getuid(PO) == ROOTID;
}

/* privilege_violation is called when objects try to do illegal things,
 * or files being compiled request a privileged efun.
 *
 * return values: 
 *   1: The caller/file is allowed to use the privilege.
 *   0: The caller was probably misleaded; try to fix the error.
 *  -1: A real privilege violation. Handle it as error.
 */
int privilege_violation(string what, mixed who, mixed where) {
  object jof;

  if (objectp(who) && (geteuid(who)==ROOTID || who==this_object())) return 1;

  if(what == "send_udp") return 1; /* yet */
  if(what == "call_out_info") return 1; /* yet */

  if(what == "nomask simul_efun" &&
     (who == SIMUL_EFUN_FILE || who == SPARE_SIMUL_EFUN_FILE))
    return 1;

  if(what == "set_extra_wizinfo" || what == "get_extra_wizinfo")
    if (who == find_object(SIMUL_EFUN_FILE) ||
	who == find_object(SPARE_SIMUL_EFUN_FILE) || who == this_object())
      return 1;
      
  if(what == "rename_object" && 
      explode(object_name(who),"#")[0]=="/secure/login")
    return 1;

  if(what == "erq" && who == find_object(SOCKETD_FILE))
      return 1;

  if(what == "input_to") return 1;

  if(what == "configure_object") return 1;

  if(what == "set_this_object") return 1;

  if(what == "bind_lambda") return 1;

  if(what == "unbound_lambda") return 1;

  if(what == "restore_object") return 1;

  if(what == "save_object") return 1;

  return -1;
}

#define D2(x)        if (funcall(symbol_function('find_player),"debugger"))\
tell_object(funcall(symbol_function('find_player),"debugger"),x)

void receive_udp(string host, bytes message, int port)
{
  // Traditionell sind einkommende Nachrichten via UDP immer ASCII-pur
  // gewesen. Davon geht auch aller hier gerufener Code aus. Daher wird jetzt
  // hier zentral die Bytesequenz als ASCII interpretiert und in string
  // gewandelt, der dann wie bislang auch verarbeitet wird.
  string msg_text;

  // if (host=="127.0.0.1")
  // {
    if (catch(msg_text = to_text(message, UDP_ENCODING"//REPLACE");publish))
    {
      msg_text = to_text(message, UDP_ENCODING"//REPLACE");
      log_file("UDP_INVALID",sprintf(
        "Nachricht von %s:%d mit nicht-UTF8-Zeichen: %s\n",
        host, port, msg_text));
    }
  // }
  // else
  // {
  //   if (catch(msg_text = to_text(message, "ASCII");publish))
  //   {
  //     msg_text = to_text(message, "ISO8859-1");
  //     log_file("UDP_INVALID",sprintf(
  //       "Nachricht von %s:%d mit nicht-ASCII-Zeichen: %s\n",
  //       host, port, msg_text));
  //   }
  // }

  // Nicht mehr benutzt, zur Sicherheit abfangen vor dem inetd.
  if (strstr(msg_text, "EXTREQ:") == 0
      || strstr(msg_text, "IPNAME") == 0
      || strstr(msg_text, "AUTH") == 0)
  {
    return;
  }

  if (strstr(msg_text, "udp_query:") == 0)
  {
    return udp_query(msg_text[10..],host,port);
  }
  // Rest an inetd fuer Bearbeitung als Intermud-Nachricht.
  ({void})"secure/inetd"->receive_udp(host, msg_text);
}

//mixed *lall() { return get_extra_wizinfo(0); }

void slow_shut_down(int minutes)
{
  filter(users(),#'tell_object,
  "Der Gamedriver ruft: Der Speicher wird knapp ! Bereitet euch auf das Ende vor !\n");
  "/obj/shut"->shut(minutes*60);
}

void external_master_reload()
{
  int i;
  mixed *CALL_OUTS;

//  "/secure/simul_efun"->StopCallOut(1);
}

int heart_beat_error( object heart_beat, string error, string program,
		     string current_object, int line)
{
  if (!heart_beat)
    return 0;
  if ( interactive(heart_beat) ) {
    tell_object( heart_beat,
		"Der GameDriver teilt Dir mit: Dein Herzschlag hat ausgesetzt!\n");
    if (IS_LEARNER(heart_beat))
      tell_object(heart_beat, "Fehler: "+error+"Progamm: "+program+", CurrObj: "+current_object+", Zeile: "+line+"\n");
  }
  if (query_once_interactive(heart_beat))
    call_out("restart_heart_beat", 5, heart_beat);
  return 0; /* Don't restart */
}

void disconnect(object who)
{
  who->NetDead();
}

object compile_object(string st)
{
  int l;
  object ret;
  string *str;
  string compiler;
  
  if (!st || st=="")
    return 0;
  str=explode(st,"/")-({""});
  l=sizeof(str);
  if (l<2) return 0;
  compiler=implode(str[0..l-2],"/")+"/virtual_compiler";
  if (file_size(compiler+".c")>0)
  {
    if (catch(ret=({object})call_other(compiler,"compile_object",str[l-1])))
      return 0;
  }
  return ret;
}

void dangling_lfun_closure() {
  raise_error("dangling lfun closure\n");
}

void remove_player(object victim) {
  catch(victim->quit());
  if (victim)
    destruct(victim);
}

mixed give_uid_to_object(mixed what,object po, int clone)
{
  string creator,pouid;

  if ((!objectp(what) && !stringp(what)) || !objectp(po))
    return 1; // invalid parameters
  if (!(creator=creator_file(what)))
    return 1; // No creator, disallow
  if (creator==ROOTID && po==TO)
    return ROOTID; // Special hack for first load of simul_efun - ARGHH
  if (!(pouid=geteuid(po)))
    return 1; // Disallow if no euid in po
//  if (pouid==NOBODY)
//    log_file("NOBODY_CLONES",sprintf("%O (euid %O) cloned %O\n",po,pouid,what));
  if (creator==pouid || creator==BACKBONEID)
    return pouid;
  return ({creator,NOBODY});
}

void inaugurate_master(int arg) {

  efun::configure_object(this_object(), OC_EUID, ROOTID);

  // Driver konfigurieren
  // bei DC_SIGACTION_SIGUSR1 wird immer der Master neugeladen.
  configure_driver(DC_SIGACTION_SIGUSR1, DCS_RELOAD_MASTER);
  // bei SIGUSR2 wird der Master informiert, aber der Driver macht immer sein
  // Standardverhalten (Debuglog neu oeffnen).
  configure_driver(DC_SIGACTION_SIGUSR2, DCS_INFORM_MASTER);

  // Und Encoding fuer Dateinamen im Filesystem
  configure_driver(DC_FILESYSTEM_ENCODING, "UTF-8");
  // Standard-Encoding fuer Interactives
  // configure_interactive(0, IC_ENCODING, "ASCII//TRANSLIT");
  configure_interactive(0, IC_ENCODING, "UTF-8");
  // Standard-Encoding fuer Dateien
  set_driver_hook(H_FILE_ENCODING, "UTF-8");
  
  userinfo::create();
  set_driver_hook(H_AUTO_INCLUDE,"#pragma combine_strings\n#pragma verbose_errors\n");
  ReloadBanishFile();
  if (!arg)
    set_extra_wizinfo(0, allocate(BACKBONE_WIZINFO_SIZE));
  if (find_call_out("wiz_decay") < 0)
    call_out("wiz_decay", 3600);
    set_driver_hook(
		    H_MOVE_OBJECT0,
		    unbound_lambda( ({'item, 'dest}), ({#',,
		      ({#'?, ({#'!=, 'item, ({#'this_object})}),
			({#'raise_error,
			  "Illegal to move other object than this_object()\n"}) }),
		      ({#'efun::set_environment, 'item, 'dest}),
		      ({#'?, ({#'living, 'item}), ({#',,
			({#'efun::set_this_player, 'item}),
			({#'call_other, 'dest, "init"}),
			({#'?, ({#'!=, ({#'environment, 'item}), 'dest}), ({#'return})}),
		      }) }),
		      ({#'=, 'others, ({#'all_inventory, 'dest}) }),
		      ({#'=, ({#'[, 'others, ({#'member, 'others, 'item}) }), 0}),
		       ({#'filter, 'others,
			 ({#'bind_lambda,
			   unbound_lambda( ({'ob, 'item}),
					  ({#'?, ({#'living, 'ob}), ({#',,
					    ({#'efun::set_this_player, 'ob}),
					    ({#'call_other, 'item, "init"}),
					  }) })
					  )
			   }),
			 'item,
		       }),
		       ({#'?, ({#'living, 'item}), ({#',,
			 ({#'efun::set_this_player, 'item}),
			 ({#'filter_objects, 'others, "init"}),
		       }) }),
		       ({#'?, ({#'living, 'dest}), ({#',,
			 ({#'efun::set_this_player, 'dest}),
			 ({#'call_other, 'item, "init"}),
		       }) }),
		     }) )
				   );
        set_driver_hook(
             H_LOAD_UIDS,
             unbound_lambda( ({'object_name}), ({
	       #'give_uid_to_object,'object_name,({#'previous_object}),0
             }) )
            );
       set_driver_hook(
            H_CLONE_UIDS,
            unbound_lambda( ({'blueprint, 'new_name}), ({
	      #'give_uid_to_object,'new_name,({#'previous_object}),1
           }) )
        );
#if 1
      set_driver_hook(H_CREATE_OB,          "create");
#else
      set_driver_hook(H_CREATE_OB,
      unbound_lambda(({'ob}), ({#',,
          ({#'&&,
            ({#'>, ({#'sizeof, ({#'=, 'list, ({#'inherit_list, 'ob})})}), 1}),
            ({#'find_object, ({#'=, 'first, ({#'[, 'list, 1})})}),
            ({#'==,
              ({#'sizeof, 'list}),
              ({#'+, 1,
                ({#'sizeof, ({#'inherit_list, ({#'find_object, 'first})})})})}),
            ({#'==, 1, ({#'sizeof, ({#'=, 'list,
                ({#'functionlist, 'ob,
                  RETURN_FUNCTION_NAME|NAME_INHERITED})})})}),
            ({#'==, ({#'[, 'list, 0}), "create"}),
#if 1 
            ({#',, ({#'printf, "replacing %O\n", 'ob}), 1}),
#endif
            ({#'funcall,
              ({#'funcall, #'bind_lambda, #'replace_program, 'ob}),
              'first})
          }),
          ({#'call_other, 'ob, "create"})
        })
      ));
#endif
      set_driver_hook(H_INCLUDE_DIRS, map(
				define_include_dirs(),
				#'[..<],
				0, 3
			));
      // Standardincludeverzeichnisse fuer #include <>
      set_driver_hook(H_INCLUDE_DIRS, ({"/secure/","/sys/"}) );
      
      set_driver_hook(H_CREATE_CLONE,       "create");
      set_driver_hook(H_RESET,              "reset");
      set_driver_hook(H_CLEAN_UP,           "clean_up");
      set_driver_hook(H_MODIFY_COMMAND,     "modify_command");
      set_driver_hook(H_NOTIFY_FAIL,        "Wie bitte?\n");
      set_driver_hook(H_TELNET_NEG,0);

      set_driver_hook(H_MODIFY_COMMAND_FNAME, "modify_command");
#ifdef __EMACS__NERV__
		    ]
#endif
		      call_out("_cleanup_uinfo",3600);
			//call_out("start_simul_efun",0);
}

// WIZLIST
static void wiz_decay() {
  mixed *wl;
  int i;

  wl = wizlist_info();
  for (i=sizeof(wl); i--; ) {
    set_extra_wizinfo(wl[i][WL_NAME], wl[i][WL_EXTRA] * 99 / 100);
  }
  call_out("wiz_decay", 3600);
}

void save_wiz_file() {
  rm("/WIZLIST");
  write_file(
	     "/WIZLIST",
	     implode(
		     map(wizlist_info(),
			       lambda(({'a}),
				      ({#'sprintf, "%s %d %d\n",
					({#'[, 'a, WL_NAME}),
					 ({#'[/*]*/, 'a, WL_COMMANDS}),
					  ({#'[/*]*/, 'a, WL_EXTRA})
#ifdef __EMACS__NERV__
					   ]]]
#endif
					   })
				      )
			       ), ""
		     )
	     );
}

void notify_shutdown() {
  if (PO && PO != TO)
    return;
  filter(users(), #'tell_object, //#'
	       "Game driver shouts: LDmud shutting down immediately.\n");
  save_wiz_file();
}

int remove()
{
  write("The master object doesn't want to be removed!\n");
  return 0;
}

void telnet_neg(mixed cmd,mixed opt,mixed args)
{
  if (opt==34 && cmd==251)
        binary_message(({255,250,34,1,1,255,240}));
}

protected int handle_external_signal(int signal)
{
  switch (signal)
  {
    case SIGHUP:
    case SIGINT:
    case SIGUSR1:
      break;
    case SIGUSR2:
#ifdef __TLS__
      tls_refresh_certs();
#endif
      break;
  }

  // Standardfunktionalitaet des Drivers zusaetzlich ausfuehren.
  return 0;
}
