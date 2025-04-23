// Tamedhon MUDlib
//
// std/ROOM.C -- room base object
//
// $Date: 1997/06/25 11:31:25 $                      
/* $Revision: 3.1 $       
 * $Log: room.c,v $
 * Revision 3.2  2005/11/07 14:22:00  Querolin
 * ROOMCOUNTER->__count_me() eingefuegt.
 *
 * Revision 3.1  1997/06/25 11:31:25  Woody
 * items::init() aufrufen, P_HIDE_EXITS beruecksichtigen
 *
 * Revision 3.0  1997/03/13 03:11:46  Woody
 * prepared for revision control
 *
 * Revision 2.25  1997/02/10  Woody
 * inherit container/inventory
 *
 * Revision 2.24  1996/05/13  11:12:00  Woody
 * "Set(P_IDS, ({}));" aus create() raus (wg. AddId Raum)
 *
 * Revision 2.23  1996/03/11  02:46:00  Mupfel
 * Includes geaendert
 *
 * Revision 2.22  1995/03/23  10:18:28  Rumata
 * ausgaenge werden fuer blinde und dunkle unsichtbar.
 *
 * Revision 2.21  1995/02/06  19:44:14  Jof
 * RCS-Logs aufgeraeumt (leere Logs raus usw)
 *
 * Revision 2.20  1994/12/23  12:21:07  Rumata
 * Revision 2.19  1994/10/24  09:02:40  Rumata
 * Revision 2.18  1994/10/18  09:28:21  Jof
 * Revision 2.16  1994/10/07  12:55:12  Jof
 * replace_program-hack
 *
 * Revision 2.14  1994/07/10  17:23:38  Jof
 * Revision 2.13  1994/07/10  00:01:07  Kirk
 * removed MayAddWeight (is defined in restrcitions.c)
 *
 * Revision 2.12  1994/06/29  17:58:41  Jof
 * Revision 2.11  1994/06/29  13:45:30  Kirk
 * Removed old AddDoor/AddDoor2 stuff
 *
 * Revision 2.10  1994/06/15  15:23:40  Hate
 * Bugfix in show() (Jof's work)
 *
 * Revision 2.9  1994/06/09  16:45:46  Jof
 * " clean count " Logik eingebaut
 *
 * Revision 2.8  1994/05/24  19:16:18  Kirk
 * added support for NewDoors
 *
 * Revision 2.7  1994/03/21  16:48:13  Jof
 * Reindented files using emacs - once again
 *
 * Revision 2.6  1994/03/10  20:12:58  Jof
 * Noch ein paar Aenderungen an clean_up
 *
 * Revision 2.5  1994/03/10  19:55:26  Jof
 * removed clean_up - Logging
 *
 * Revision 2.4  1994/02/18  12:28:31  Jof
 * clean_up didnt work as it should
 *
 * Revision 2.3  1994/02/15  17:45:42  Hate
 * changed clean_up to meet array implementation of items
 *
 * Revision 2.2  1994/02/02  21:24:48  hate
 * debug_log for clean_up
 *
 * Revision 2.2  1994/02/02  21:24:48  hate
 * debug_log for clean_up
 *
 * Revision 2.1  1994/02/02  21:19:16  hate
 * clean_up fixes
 *
 * Revision 2.0  1994/01/13  16:52:24  mud
 */                            

#pragma strong_types

inherit "std/thing/properties";
//inherit "std/thing/util";
inherit "std/thing/language";
inherit "std/room/moving";
inherit "std/room/restrictions";
inherit "std/room/description";
inherit "std/room/exits";
inherit "std/room/commands";
inherit "std/room/items";
inherit "std/room/doors";
inherit "std/container/inventory";

#define ROOMCOUNTER "/p/service/querolin/roomcounter.c"

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <config.h>
#include <properties.h>
#include <language.h>
#include <wizlevels.h>
#include <moving.h>
#include <defines.h>
#include <functionlist.h>
#include <room.h>

void reset()
{
  items::reset();
  exits::reset();
  doors::reset();
}

static int
_query_noget()
{
  return 1;
}

void _create()
{
  /* Set effective userid to userid */
  /* so that we may clone other things */
  seteuid(getuid(this_object()));
  properties::create();
  restrictions::create();
  description::create();
  exits::create();
  items::create();
  commands::create();
  doors::create();
  AddId("Raum");
  SetProp(P_MAGIC,1);		/* all kind of magic is allowed */
  SetProp(P_NAME,0);
  SetProp(P_NAME_ADJ,({}));
  Set(P_SHORT,0);
  Set(P_LONG,0);
  Set(P_TRANSPARENT,0);
  Set(P_ADJECTIVES,({}));
  // Set(P_IDS,({}));
  Set(P_WEIGHT,PROTECTED,F_MODE);
  Set(P_TOTAL_WEIGHT,PROTECTED,F_MODE);
  Set(" clean counter ",2);

  ROOMCOUNTER->__count_me(object_name(this_object() ) );
}

void maybe_replace_program()
{
  string *list, first;
  object first_ob;

  first_ob=find_object(first=(list=inherit_list(this_object()))[1]);
  if (object_name(this_object())=="/std/room" ||
      !(first_ob=find_object(first=(list=inherit_list(this_object()))[1])) ||
      (sizeof(list)!=1+sizeof(inherit_list(first_ob))) ||
      (1!=sizeof(list=functionlist(this_object(),
				   RETURN_FUNCTION_NAME|NAME_INHERITED))) ||
      list[0]!="create")
    return;
  replace_program(first);
}

void create()
{
  maybe_replace_program();
  _create();
}

static int check_clean_count()
{
  int cc;

  cc=Query(" clean counter ");
  if (--cc<=0)
    return 1;
  Set(" clean counter ",cc);
  return 0;
}
  
int clean_up(int arg)
{
  mixed itema;
	object me;
  
  if(arg>1) return 1; // better not ;)

  if (Query(" never clean ")) return 0;

  // if there are any item we have produced ourselfes check them
  if(pointerp(itema = QueryProp(P_ITEMS)))
  {
    mixed names;
    int i;
    i = sizeof(names = all_inventory(this_object()));
    while(i--)
    {
      if (query_once_interactive(names[i]))
      {
	Set(" clean counter ",2);
	return 1;
      }
      if(objectp(names[i])) names[i] = explode(object_name(names[i]),"#")[0];
    }
    
    for(i = sizeof(itema)-1; i >= 0; i--)
    {
      // Semantik:
      // 1. Wenn RITEM_OBJECT ein array ist, kann evtl ge'clean'ed werden.
      // 2. Wenn es ein REFRESH_NONE Item ist und es entweder nicht mehr existiert oder nicht
      //    in diesem Raum ist, kein clean_up (es wuerde beim neuladen des Raumes ja wieder erzeugt;
      //    falls es aber hier ist, wird es mitvernichtet, dann ists ok)
      // 3. Wenn es ein REFRESH_DESTRUCT ist und noch existiert, aber nicht hier ist, KEIN 
      //    clean_up.
      if (!pointerp(itema[i][RITEM_OBJECT]) &&
	  ((itema[i][RITEM_REFRESH] == REFRESH_NONE &&
	    (!itema[i][RITEM_OBJECT] || environment(itema[i][RITEM_OBJECT])!=this_object())) ||
	   (itema[i][RITEM_REFRESH] == REFRESH_DESTRUCT &&
	    itema[i][RITEM_OBJECT] && environment(itema[i][RITEM_OBJECT]) != this_object())))
	return 1;
      names -= (pointerp(itema[i][RITEM_FILE]) ?
		itema[i][RITEM_FILE] : ({ itema[i][RITEM_FILE] }));
    }
    // if there are objects left in the room do not clean up but try again later
    if(sizeof(names) && !check_clean_count()) return 1;
  }
  else
    // is there any object lying around?
    if(first_inventory(this_object()) && !check_clean_count()) return 2;
  
  // do clean_up
  //    log_file("clean_up_log",sprintf("%s:%s: %O\n",ctime(time())[11..18],__HOST_NAME__,this_object()));
	me = this_object();
	me->remove(); // mit late binding !
	if (me) destruct(me); // auf jeden fall sterben !
  return 0;
}

/* Instead of printing the exits with the long description, we implement */
/* the command "exits" to show them. */
int show_exits() {
  string ex;
  if( this_player()->CannotSee() ) return 1;
  if (QueryProp(P_HIDE_EXITS) && !pointerp(QueryProp(P_HIDE_EXITS))) return 1;
  if (ex = GetExits(this_player())) write(ex);
  return 1;
}

int toggle_exits(string str)
{
  int ex;
  
  if (!str) return show_exits();
  if (str!="auto") return 0;
  ex = this_player()->QueryProp(P_SHOW_EXITS);
  this_player()->SetProp(P_SHOW_EXITS, !ex);
  if (ex) write("Ausgänge werden nicht mehr automatisch angezeigt.\n");
  else write("Ausgänge werden automatisch angezeigt.\n");
  return 1;
}

void init()
{
  Set(" clean counter ",2);
  items::init();
  exits::init();
  commands::init();
  description::init();
  doors::init();
  if(environment(this_player())!=this_object()) return;
  add_action("toggle_exits", "exits");
  add_action("toggle_exits", "ausgang");
  add_action("toggle_exits", "ausgaenge");
  add_action("toggle_exits", "ausgänge");
  add_action("toggle_exits", "aus");
}

int is_player_here()
{
  foreach(object obj:all_inventory())
  {
    if(interactive(obj) && !IS_LEARNER(obj))
      return 1;
  }
  return 0;
}
