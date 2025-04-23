// SilberLand MUDlib
//
// std/player/objects.c -- object handling for player
//
// $Date: 1997/06/25 12:04:19 $                      
/* $Revision: 3.3 $       
 * $Log: objects.c,v $
 * Revision 3.3  1997/06/25 12:04:19  Woody
 * Typofix, 'ME->' bei remove() in exec() raus
 *
 * Revision 3.2  1997/05/30 13:41:04  Woody
 * clone nun etwas logischer gestaltet (P_CLONE_MSG maessig)
 *
 * Revision 3.1  1997/05/13 11:13:29  mud
 * alles erst ab wizard_lvl
 *
 * Revision 3.0  1997/03/13 01:42:20  Woody
 * prepared for revision control
 *
 * Revision 2.11  1995/05/10  09:18:12  Rumata
 * P_CMSG -> P_CLONE_MSG, P_DMSG -> P_DESTRUCT_MSG
 *
 * Revision 2.10  1995/05/09  20:32:07  Hate
 * we now use P_DMSG and P_CMSG
 *
 * Revision 2.9  1995/04/26  16:29:34  Jof
 * fixed minor bug in load()
 *
 * Revision 2.9  1995/04/26  16:29:34  Jof
 * fixed minor bug in load()
 *
 * Revision 2.8  1995/01/13  17:09:45  Jof
 * ... unparsed_args
 * ,
 *
 * Revision 2.7  1994/07/14  15:15:51  Jof
 * Removed command mappings
 *
 * Revision 2.6  1994/07/12  18:09:27  Jof
 * *** empty log message ***
 *
 * Revision 2.5  1994/07/12  00:06:43  Jof
 * Weg mit AddAction
 *
 * Revision 2.4  1994/04/07  13:00:58  Jof
 * cloning message :)
 *
 * Revision 2.3  1994/03/17  23:36:55  Jof
 * PROTECTED -> SECURED & Reindented
 *
 * Revision 2.2  1994/03/14  15:57:38  Jof
 * Fixed cloning
 *
 * Revision 2.1  1994/02/23  08:47:05  Jof
 * Fixed some bugs in cloning
 *
 * Revision 2.0  1994/01/13  16:59:02  mud
 * *** empty log message ***
 *
 * Revision 1.3  1994/01/05  16:40:36  mud
 * Merged add_object_commands and add_object_commands2
 *
 * Revision 1.2  1994/01/05  16:17:07  mud
 * changed save variables partly to mapping entry
 *             
 */                            

#pragma strong_types

#define NEED_PROTOTYPES
#include "/sys/player/filesys.h"

#include <config.h>
#include <properties.h>
#include <language.h>
#include <moving.h>
#include <wizlevels.h>
#include <thing/moving.h>

static int update_object(string str) {
  object ob;
  string upd_file;
  if (!(str=_unparsed_args())) {
    notify_fail("Usage: Update <object_path>\n"); return 0;
  }
  upd_file = find_file(str,".c");
  if (!upd_file) upd_file=find_file(str);
  if (!upd_file) {
    notify_fail(str+": No such file.\n"); return 0;
  }
  ob = find_object(upd_file);
  if (!ob) {
    notify_fail(upd_file+": No such object.\n"); return 0;
  }
  destruct(ob);
  write(upd_file + ": will be reloaded at next reference.\n");
  return 1;
}

/*
 * "soft" means that the object is given the chance to self-destruct, thus
 * allowing it to do necessary cleanup like subtracting from the carried
 * weight of the environment(). We call remove() in the object to be
 * destructed.
 */
static int soft_update_object(string str) 
{
  object ob;
  string upd_file;
  if (!(str=_unparsed_args())) {
    notify_fail("Usage: update <object_path>\n"); return 0;
  }
  upd_file = find_file(str,".c");
  if (!upd_file) upd_file=find_file(str);
  if (!upd_file) {
    notify_fail(str+": No such file.\n"); return 0;
  }
  ob = find_object(upd_file);
  if (!ob) {
    notify_fail(upd_file+": No such object.\n"); return 0;
  }
  if (ob->remove() == 0) {
    notify_fail(upd_file+": doesn't want to be destructed!\n"); return 0;
  }
  write(upd_file + ": will be reloaded at next reference.\n");
  return 1;
}

int clone(string str) 
{
  object ob;
  string clone_file;

  if (!(str=_unparsed_args())){
    notify_fail("Usage: clone <object_path>\n"); return 0;
  }
  clone_file = find_file(str,".c");
  if (!clone_file) clone_file=find_file(str);
  if (!clone_file) {
    notify_fail(str+": No such file.\n"); return 0;
  }
  ob = clone_object(clone_file);
  if (!ob)
  {
    say(this_player()->Name(WER,1)+" malt wilde Zeichen in die Luft und "
        +"murmelt vor sich hin, aber nichts passiert...\n");
    write(clone_file+": Failed to clone.\n");
  } else {
    write("Cloned "+object_name(ob)+".\n");
    say(this_player()->Name(WER,1) + " "
                + this_player()->QueryProp(P_CLONE_MSG)+".\n");
    /* try to move the object to my environment */
    if (! ((ob->move(this_object(),M_GET)>0) || 
          (ob->move(environment(),M_NOCHECK)>0)))
    {
      say(ob->Name(WER,1) + " löst sich in Luft auf.\n");
      ob->remove();
      if (ob) destruct(ob);
      write(clone_file+": failed to move\n");
    }
  }
  return 1;
}

/*
 * "soft" means that the object is given the chance to self-destruct, thus
 * allowing it to do necessary cleanup like subtracting from the carried
 * weight of the environment(). We call remove() in the object to be
 * destructed.
 */
static int soft_destruct_object(string str)
{ 
  object ob;
  object *obs;
  string strWER,strWEN;

  if (!(str=_unparsed_args())){
    notify_fail("Usage: destruct <objectname>\n"); return 0;
  }
  strWER = lower_case(str);
  obs = this_player()->select_objects(strWER);
  if (!obs || !sizeof(obs)) {
    notify_fail("Kein \"" + str + "\" gefunden.\n");
    return 0;
  }
  ob=obs[0];
  strWER=ob->name(WER);
  strWEN=ob->name(WEN);
  if (!strWER)
    strWER="jemand";
  if (!strWEN)
    strWEN="jemanden";

  if (ob->remove() == 0) {
    notify_fail(strWER+" will nicht 'destructed' werden!\n");
    say(this_player()->name(WER,1)+" versucht vergeblich, "+strWEN+
        " zu atomisieren.\n");
    return 0;
  }
  say(capitalize(strWER)+" "+this_player()->QueryProp(P_DESTRUCT_MSG)+".\n");
  write(capitalize(strWER)+" wird von dir zerstäubt.\n");
  return 1;
}

static int destruct_object(string str)
{ 
  object ob;
  object *obs;
  string strWER,strWEN;

  if (!(str=_unparsed_args())) {
    notify_fail("Usage: Destruct <objectname>\n"); return 0;
  }
  strWER = lower_case(str);
  obs = this_player()->select_objects(strWER);
  if (!obs || !sizeof(obs)) {
    notify_fail("Kein \"" + str + "\" gefunden.\n"); return 0;
  }
  ob=obs[0];
  strWER=ob->name(WER);
  strWEN=ob->name(WEN);

  say(capitalize(strWER)+" "+this_player()->QueryProp(P_DESTRUCT_MSG)+".\n");
  destruct( ob );
  write(capitalize(strWER)+" wird von dir zerstäubt.\n");
  return 1;
}

/*
 * load object. If it cant be loaded errormsg is returned, else
 * 0 is returned.
 * We just call a non existing function in the object, and that way
 * force it to be loaded.
 */
string load_object(string file)
{
  return catch(call_other(file,"???"));
}

static int load(string str)
{ 
  object env;
  string file;
  string err;

  if (!(str=_unparsed_args())) {
    notify_fail("Usage: load <object_path>\n"); return 0;
  }
  file = find_file(str,".c");
  if (!file) file=find_file(str);
  if (!file) {
    notify_fail(str+": No such file.\n"); return 0;
  }
  if (err=load_object(file))
    printf("Cannot load %O, err = %O\n",file,err);
  else write(file+"\n");
  return 1;
}

static int exec_playerob(string name)
{
  object ob, *inv;
  int i;

  if (!IS_LORD(this_object())) return 0;
  if (this_player() != this_interactive()) return 0;
  if (this_player() != this_object()) return 0;
  if (!(name=_unparsed_args())) return 0;
  name="secure/master"->_get_path(name,getuid(this_object()));
  if (catch(call_other(name,"????"))) 
  {
    write("BUG in "+name+"\n");
    return 1;
  }
  ob=clone_object(name);
  if (!ob) return 0;
  if (getuid(ob) != getuid(this_object()))
  {
    write("UID conflict.\n");
    destruct(ob);
    return 1;
  }
  log_file("EXEC", getuid(this_object())+" "+name+" "+dtime(time()));
  disable_commands();
  exec(ob,this_object());
  if (interactive(this_object()) || !interactive(ob))
  {
    enable_commands();
    write("Fehler in exec\n");
    return 1;
  }
  inv=all_inventory(this_object());
  ob->start_player(capitalize(getuid(this_object())));
  remove();
  return 1;
}

static mixed _query_localcmds()
{
  return ({
           ({"clone","clone",0,WIZARD_LVL}),
	   ({"destruct","soft_destruct_object",0,WIZARD_LVL}),
	   ({"Destruct","destruct_object",0,WIZARD_LVL}),
	   ({"load","load",0,WIZARD_LVL}),
	   ({"update","soft_update_object",0,WIZARD_LVL}),
	   ({"Update","update_object",0,WIZARD_LVL}),
	   ({"exec","exec_playerob",0,WIZARD_LVL})
	 });
}
