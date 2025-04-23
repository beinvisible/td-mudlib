// Tamedhon MUDlib
//
// std/room/exits.c -- room exits handling 
//
// $Date: 2004/01/10 18:45:00 $
/* $Revision: 1.45 $
 * $Log: exits.c,v $
 * Revision 1.45 2003/01/10 18:45:00  Serii
 * commobj-fix
 *
 * Revision 1.4 2003/12/27  11:31:07 Falgrim
 * umbauten am farbsystem (commobj)
 *
 * Revision 1.35a 2003/03/28  11:31:07 Falgrim
 * GetExit() bugfix ->this_player()
 *
 * Revision 1.35 2002/12/15  11:31:07 Falgrim
 * Bunt fuer die Richtungen der
 * sichtbaren Ausgaenge eingefuegt.
 *
 * Revision 1.3  2000/06/06 11:31:07  Samtpfote
 * *seufz* alles wieder rueckgaengig
 *
 * Revision 1.2  2000/06/04 18:24:35  Samtpfote
 * AddGuardedExit() hinzugefuegt.
 *
 * Revision 1.1  2000/06/04 18:24:06  Samtpfote
 * Initial revision
 *
 * Revision 3.3  1997/09/17 09:47:18  Woody
 * EX_PLMSG ausgeben, bevor EX_FUNC/EX_DEST ausgewertet wird
 *
 * Revision 3.2  1997/09/15 23:50:19  Woody
 * Neues Exit-System: P_NEW_EXITS. P_EXITS und P_SPECIAL_EXITS
 * bleibt bei Abfrage via QueryProp() abwaertskompatibel.
 *
 * Revision 3.1  1997/06/25 12:08:24  Woody
 * Forscherpunkt nur vergeben, wenn pl->move() > 0 retourniert
 *
 * Revision 3.0  1997/03/13 02:02:05  Woody
 * prepared for revision control
 *
 * Revision 2.25  1996/10/17  Woody
 * '_nach_ Nordwestoben' etc. eingefuehrt
 *
 * Revision 2.24  1996/09/26  05:00:00  Woody
 * Exploration Handling eingefuehrt
 *
 * Revision 2.23  1996/03/12  00:50:00  Mupfel
 * Includes geaendert: /sys/room/exits.h /sys/room/doors.h
 *
 * Revision 2.22  1996/03/05  00:15:00  Mupfel
 * AddGuardedExit laesst NPCs durch
 *
 * Revision 2.21  1996/02/29  00:15:00  Mupfel
 * AddGuardedExit, RemoveGuardedExit
 *
 * Revision 2.20  1995/10/08  14:30:00  Mupfel
 * P_EXIT_PREPOSITION fuer Ausgaenge
 *
 * Revision 2.19  1995/07/04  16:11:39  Rochus
 * Registrierung von Exits beim Routing Daemon
 *
 * Revision 2.18  1994/10/04  09:55:26  Jof
 * *** empty log message ***
 *
 * Revision 2.18  1994/10/04  14:41:59  mud
 * *** empty log message ***
 *
 * Revision 2.17  1994/08/22  17:56:39  Kirk
 * changed return type of AddSpecialExit to void
 *
 * Revision 2.16  1994/07/19  16:17:52  Hate
 * bugfix in AddExit()
 *
 * Revision 2.15  1994/07/19  15:11:57  Kirk
 * *** empty log message ***
 *
 * Revision 2.14  1994/07/12  00:09:10  Kirk
 * clean up
 *
 * Revision 2.13  1994/06/29  17:35:52  Jof
 * removed leftovers of old doors
 *
 * Revision 2.12  1994/06/29  13:44:14  Kirk
 * Removed old AddDoor/AddDoor2 stuff
 *
 * Revision 2.11  1994/06/22  18:55:42  Hate
 * made relative paths possible in _MakePath()
 *
 * Revision 2.10  1994/05/24  21:48:54  Kirk
 * removed debug output
 *
 * Revision 2.9  1994/05/24  19:14:26  Kirk
 * added support for NewDoors
 *
 * Revision 2.8  1994/03/21  16:48:13  Jof
 * Reindented files using emacs - once again
 *
 * Revision 2.7  1994/02/24  13:04:21  Jof
 * Private removed
 *
 * Revision 2.6  1994/02/24  09:44:27  Kirk
 * *** empty log message ***
 *
 * Revision 2.5  1994/02/21  12:18:49  Jof
 * checked for existance of exits in _normalfunction
 *
 * Revision 2.4  1994/02/09  05:06:56  Hate
 * bugfix: replaced QueryProp(P_EXITS) by Query(P_EXITS) because
 * Queryprop(P_EXITS) returns only NORMAL exits and cuts out Special exits
 *
 * Revision 2.3  1994/02/08  19:34:58  Kirk
 * removed debug message
 *
 * Revision 2.2  1994/01/25  15:50:34  Kirk
 * changed QueryProp(P_EXITS) to Query(P_EXITS) in RemoveExit()
 *
 * Revision 2.1  1994/01/16  14:38:46  mud
 * *** empty log message ***
 *
 * Revision 2.0  1994/01/13  17:00:23  mud
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/08  13:08:41  mud
 * changed to #pragma strong_types
 *             
 */                            

/*
 * Exits of the room (obvious ones, doors, and special ones)
 * we define the following function for easy reference:
 * GetExits() - return a string containing an "Obvious Exits" Statement
 *
 * The exits are implemented as properties P_EXITS
 * They are stored locally (_set_xx, _query_xx)
 * as mapping to speed up the routines in this module.
 *
 */

#pragma strong_types

#define NEED_PROTOTYPES

#include "/std/sys_debug.h"
#include <config.h>
#include <thing/properties.h>
#include <properties.h>
#include <moving.h>
#include <defines.h>
#include <room/exits.h>
#include <room/doors.h>
#include <routingd.h>
#include <wizlevels.h>
#include <new_skills.h>  // fuer P_TMP_MOVE_HOOK
#include <exploration.h>
//ansi-farben
#include <ansi.h>

 // global var's
private static string *explore;

// local prototypes
string _MakePath(string str);
string substitute_str(string str);
string GetExitMsg(string str);

//static object commobj;  <-- bugt!

#define commobj find_object("/std/player/base")

static mapping _query_new_exits()
{
  mapping exits;
  mixed key;
  int i;

  if((!previous_object() || object_name(previous_object())!=DOOR_MASTER) &&
     QueryProp(P_DOOR_INFOS))
    call_other(DOOR_MASTER,"init_doors");

  return Query(P_NEW_EXITS);
}

static mapping _query_exits()
{
  mapping exits;
  mixed key;
  int i;

  if((!previous_object() || object_name(previous_object())!=DOOR_MASTER) &&
     QueryProp(P_DOOR_INFOS))
    call_other(DOOR_MASTER,"init_doors");

  key = m_indices(exits = copy(Query(P_NEW_EXITS)));
  for (i=sizeof(key)-1;i>=0;i--)
    if ( exits[key[i]][EX_FUNC] || !stringp( exits[key[i]][EX_DEST] ) )
      exits = m_delete(exits, key[i]);
    else
      exits[key[i]] = exits[key[i]][EX_DEST];
  return exits;
}

static mapping _query_special_exits()
{
  mapping exits;
  mixed key;
  int i;

  key = m_indices(exits = copy(Query(P_NEW_EXITS)));
  for (i=sizeof(key)-1;i>=0;i--)
    if ( !closurep(exits[key[i]][EX_FUNC]) )
      exits = m_delete(exits, key[i]);
    else
      exits[key[i]] = exits[key[i]][EX_FUNC];
  return exits;
}

void reset() {}

void create()
{
  Set(P_NEW_EXITS,([]));
  Set(P_NEW_EXITS, NOSETMETHOD, F_MODE);
  Set(P_EXITS, NOSETMETHOD, F_MODE);
  Set(P_SPECIAL_EXITS, NOSETMETHOD, F_MODE);
  explore = EPMASTER->QueryExplore(EP_EXIT);
//  commobj=find_object("/std/player/base.c");
}

void AddExit(mixed cmd, mixed room)
{
  int i;
  mapping exita, exitb, exitc;
  int s;
  string msg;
  object router;

  exitb = ([]);

  exita=Query(P_NEW_EXITS);
  if (!mappingp(room))  // alte Version...
  {
    if (!closurep(room))        // Ahja... hoffentlich ist room ein String.
    {   
      if ((s = member(room, '#')) != -1)
      {
        exitb[EX_DEST] = room[s+1..];
        exitb[EX_MSG]  = room[0..(s-1)];
      }
      else
        exitb[EX_DEST] = room;

      exitb[EX_DEST] = _MakePath(exitb[EX_DEST]);

      if (!clonep(this_object()) && objectp(router=find_object(ROUTER)))
        router->RegisterExit(object_name(this_object()),cmd,exitb[EX_DEST]);
    }
    else
      exitb[EX_FUNC] = room;
  }
  else  // neue Version...
  {
    exitb = room;
    if (exitb[EX_DEST])
    {
      exitb[EX_DEST] = _MakePath(exitb[EX_DEST]);
      if (!clonep(this_object()) && objectp(router=find_object(ROUTER)))
        router->RegisterExit(object_name(this_object()),cmd,exitb[EX_DEST]);
    }
    if (exitb[EX_TESTFUNC])
    {
      if (stringp(exitb[EX_TESTFUNC]))
        exitb[EX_TESTFUNC] = symbol_function(exitb[EX_TESTFUNC], this_object());
      if (!closurep(exitb[EX_TESTFUNC])) return;
    }
    if (exitb[EX_FUNC])
    {
      if (stringp(exitb[EX_FUNC]))
        exitb[EX_FUNC] = symbol_function(exitb[EX_FUNC], this_object());
      if (!closurep(exitb[EX_FUNC])) return;
    }
  }

  if (!pointerp(cmd)) cmd = ({ cmd });

  for (i=sizeof(cmd)-1; i>=0; i--)
  {
    exitc = copy(exitb);
    if(!exitc[EX_MSG])
      exitc[EX_MSG] = GetExitMsg(cmd[i]);
    exita += ([ cmd[i] : exitc ]);
  }

  Set(P_NEW_EXITS, exita);
}

void RemoveExit(mixed cmd)
{
  mapping exita;
  
  if (!cmd)
    return Set(P_NEW_EXITS,([]));
  
  if (!pointerp(cmd))
    cmd = ({cmd});
  for(exita=Query(P_NEW_EXITS);sizeof(cmd);cmd=cmd[1..])
    exita = m_delete(exita,cmd[0]);
  
  Set(P_NEW_EXITS,exita);
}

void AddSpecialExit(mixed cmd, mixed room)
{
  if (stringp(room)) room=symbol_function(room, this_object());
  if (!closurep(room)) return;
  AddExit(cmd, room);
}

void RemoveSpecialExit(mixed cmd)
{
  RemoveExit(cmd);
}

void AddGuardedExit(mixed cmd, mixed room, string id, string fail_msg)
{
  AddExit(cmd, ([ EX_DEST:   room,
                  EXG_ID:    id,
                  EXG_PLMSG: fail_msg ]) );
}

void RemoveGuardedExit(mixed cmd)
{
  RemoveExit(cmd);
}

//Falgrim: ein bissl farbig und buuuuunt ;)
varargs string GetExits( object viewer )
{
  int n, i;
  string exits;
  string *numbers;
  string *indices;
  string *hidden;
  string color;

  if (QueryProp(P_DOOR_INFOS))
    call_other(DOOR_MASTER,"init_doors");

  numbers = ({"zwei","drei","vier","fünf","sechs","sieben","acht"});

  indices = m_indices(Query(P_NEW_EXITS));
  if (pointerp(hidden=QueryProp(P_HIDE_EXITS)))
    indices-=hidden;

  if (viewer->QueryProp(P_TTY)=="ansi"){
	color = commobj->_query_color(viewer, P_COLOR_EXIT);
  }else{
  	color = ANSI_PLAIN;
  }

	  n = sizeof(indices);
  	  switch (n)
  	  {
    		case 0:
    		return "Es gibt keine sichtbaren Ausgänge.\n";
    		case 1:
    		return "Es gibt "+color+"einen"+ANSI_PLAIN+" sichtbaren Ausgang: "+color+indices[0]+ANSI_PLAIN+".\n";
    		case 2: case 3: case 4: case 5: case 6: case 7: case 8:
    		exits = "Es gibt "+color+numbers[n-2]+ANSI_PLAIN+" sichtbare Ausgänge: ";
    		break;
    		default:
    		exits = "Es gibt viele sichtbare Ausgänge: ";
  	   }

	   for (i = 0; i < n-2; i++)
    	   	exits += color+indices[i]+ANSI_PLAIN+", ";
  	   exits += color+indices[n-2]+ANSI_PLAIN+" und "+color+indices[n-1]+ANSI_PLAIN+".";

	     return break_color_string(exits);
   return 0;
}

/*
 * init: Add the direction commands here. If there are doors, add
 * door manipulation commands.
 */
void init()
{
  add_action("_normalfunction","",1);
}

int _normalfunction() 
{
  string verb;
  mapping dest;
  mixed message, prep;
  mixed ret;

  verb = query_verb();
  if (!(dest=Query(P_NEW_EXITS)[verb])) return 0;

  // 1. EX_TESTFUNC
  // -------------------------------------------
  if (closurep(dest[EX_TESTFUNC]))
  {
    ret = funcall(dest[EX_TESTFUNC], verb);
    if (!ret) return 1;
  }

  // 2. GUARD
  // -------------------------------------------
  if (dest[EXG_ID] && present(dest[EXG_ID]) &&
      interactive(this_player()) && !IS_LEARNING(this_player()))
  {
    if (dest[EXG_PLMSG])
      write(process_string(dest[EXG_PLMSG]));
    else
      write("Eine unbekannte Kraft hält Dich zurück.\n");

    if (dest[EXG_RMMSG])
      say(capitalize(substitute_str(process_string(dest[EXG_RMMSG]))));

    return 1;
  }

  if (dest[EX_PLMSG]) write(break_string(dest[EX_PLMSG],78));

  // 3. EX_FUNC
  // -------------------------------------------
  if (closurep(dest[EX_FUNC]))
  {
    ret = funcall(dest[EX_FUNC], verb);
    if (!stringp(ret))
    {
      if (explore && ret && member(explore,verb)>=0)
        EPMASTER->GiveExplorationPoint(verb, EP_EXIT);
      return ret;
    }
  }
  else
    ret = dest[EX_DEST];

  if (!ret) return 0;
  ret = this_player()->move(ret, M_GO, dest[EX_MSG]);

  if (explore && (ret>0) && member(explore,verb)>=0)
    EPMASTER->GiveExplorationPoint(verb, EP_EXIT);

  return 1;
}

string _MakePath(string str)
{
  string pref,name,rest;
  string *comp; comp = explode(object_name(this_object()), "/");
  switch(str[0])
  {
    case '.':
      str = "/" + implode(comp[0..<2], "/") + "/" + str;
      break;
    case '~':
      str = "/" + comp[0] + "/" + (comp[0]=="d" ? comp[1] + "/" : "")
        +getuid(this_object())+str[1..];
      break;
  }
  return MASTER->_get_path(str, getuid(this_object()));
}

string GetExitMsg(string str)
{
  string s;

  if (!str) return "";

  if (str[<4..<3]=="ob") s = str[0..<5] + str[<2..];
  else if (str[<5..<3]=="unt") s = str[0..<6] + str[<2..];
  else s = str;

  if (member( ({ "süden","südwesten","westen","nordwesten",
                 "norden","nordosten","osten","südosten" }), s) != -1)
     return "nach " + capitalize(str);

  if (member(({"süden","südwesten","westen","nordwesten",
                         "norden","nordosten","osten","südosten",}), str)!=-1)
    return "nach "+capitalize(str);

  if (member( ({ "oben","unten", "links",
                 "rechts", "vorne", "hinten"}), str) != -1)
    return "nach "+str;

  if (QueryProp(P_EXIT_PREPOSITION))
    return QueryProp(P_EXIT_PREPOSITION)+" "+str;

  return str;
}

string substitute_str(string str)
{
  if (!str || !this_player()) return 0;
  str = implode(efun::explode(str, "@Wer@"), this_player()->Name() );
  str = implode(efun::explode(str, "@wer@"), this_player()->name() );
  str = implode(efun::explode(str, "@wessen@"), this_player()->name(WESSEN) );
  str = implode(efun::explode(str, "@wem@"), this_player()->name(WEM) );
  str = implode(efun::explode(str, "@wen@"), this_player()->name(WEN) );
  return str;
}
