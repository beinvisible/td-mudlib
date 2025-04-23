// SilberLand MUDlib
//
// std/thing/commands.c -- thing command handling
//
// $Date: 2001/02/10 12:05:58 $                      
/* $Revision: 3.0 $       
 * $Log: commands.c,v $
 * Revision 3.0  2001/02/10 12:05:58  Samtpfote
 * prepared for revision control
 *
 * Revision 3.0  1997/03/13 02:19:43  Woody
 * prepared for revision control
 *
 * Revision 2.14  1996/09/26  05:13:00  Woody
 * Exploration handling
 *
 * Revision 2.13  1994/10/04  09:24:02  Jof
 * *** empty log message ***
 *
 * Revision 2.13  1994/10/04  09:24:02  Jof
 * *** empty log message ***
 *
 * Revision 2.13  1994/10/03  21:45:11  mud
 * Minor changes
 *
 * Revision 2.12  1994/07/12  00:08:43  Jof
 * *** empty log message ***
 *
 * Revision 2.10  1994/07/10  22:42:04  Jof
 * add_action("_cl","") instead of many add_actions - save mem
 *
 * Revision 2.9  1994/07/10  22:33:04  Jof
 * *** empty log message ***
 *
 * Revision 2.8  1994/07/06  16:44:43  Rochus
 * add_commands sicherer gemacht
 *
 * Revision 2.7  1994/06/13  07:36:24  Jof
 * Removed SET_METHODs
 *
 * Revision 2.6  1994/06/06  09:40:31  Jof
 * All commands stored as closures
 *
 * Revision 2.5  1994/03/21  16:48:13  Jof
 * Reindented files using emacs - once again
 *
 * Revision 2.4  1994/03/18  11:56:25  Jof
 * Fixed indentation
 *
 * Revision 2.3  1994/03/09  21:52:55  Hate
 * changes made and commands.h created
 *
 * Revision 2.2  1994/03/07  15:23:02  Jof
 * *** empty log message ***
 *
 * Revision 2.1  1994/03/07  13:09:36  Kirk
 * *** empty log message ***
 *
 * Revision 2.0  1994/01/13  17:00:29  mud
 * *** empty log message ***
 *
 * Revision 1.4  1994/01/08  13:13:21  mud
 * *** empty log message ***
 *
 * Revision 1.3  1994/01/04  10:46:50  mud
 * NEED_PROTOTYPES added
 */                            

// Defining extra commands for objects
//
//	AddCmd(string|string* cmd, string fun|closure cl, int flag)
//	   Adds a verb with associated function to the object. When
//         <cmd> or one of the elements of the array <cmd> is typed,
//	   the function <fun> in the object is invoked. The function
// 	   should return 1 in success or 0 in case the object doesn't
// 	   accept the combination <cmd> and arguments to <cmd> (See
//	   "commands" in the concepts documentation). Note that you
//	   can query the verb typed by using the efun query_verb().
//
// The commands are implemented as property P_COMMANDS as mapping.

#pragma strong_types

#define NEED_PROTOTYPES 1

#include <thing/properties.h>
#include <thing/commands.h>
#include <properties.h>
#include <exploration.h>

// prototype for local property methods
static mapping _set_commands(mapping commands);

private static string *explore;

void create()
{
  Set(P_COMMANDS, ([]));
  explore = EPMASTER->QueryExplore(EP_CMD);
}

varargs void AddCmd(mixed cmd, mixed func, int flag)
{
  int i;
  mapping cmds;
  
  cmds = Query(P_COMMANDS);

  if (!pointerp(cmd)) cmd=({cmd});
  
  for (i = sizeof(cmd)-1; i>=0; i--)
    cmds += ([ cmd[i] : func; flag ]);

  Set(P_COMMANDS, cmds);			// setting new old+actions
}

varargs void RemoveCmd(mixed cmd)
{
  int i;
  mapping cmds;
  
  cmds = Query(P_COMMANDS);
  if (!cmd) Set(P_COMMANDS, ([]));
  else
  {
    if(stringp(cmd)) cmd = ([cmd]); 
    else if(!mappingp(cmd) && pointerp(cmd)) cmd = mkmapping(cmd);
    else return;
    Set(P_COMMANDS, Query(P_COMMANDS) - cmd);
  }
}

int auswerten(mixed cmd, string str)
{
  if (closurep(cmd))
    return funcall(cmd,str);
  return call_other(this_object(),cmd,str);
}

int _cl(string str)
{
  string verb;
  mixed cmd, cmds;
  int i,ret;
  string *keys;
  
  if(!verb = query_verb()) return 0;
  cmds=Query(P_COMMANDS);
  if(cmd = cmds[verb])
    if ((ret=auswerten(cmd, str))) {
      if (explore && member(explore,verb)>=0)
        EPMASTER->GiveExplorationPoint(verb, EP_CMD);
      return ret;
    }

  keys=m_indices(cmds);					// incomplete cmd
  
  for(i = sizeof(keys)-1; i>=0; i--)
  {
    string key;

    key=keys[i];
    if(cmds[key, 1] && key == verb[0..sizeof(key)-1])
      if ((ret=auswerten(cmds[key], str))) {
        if (explore && member(explore,key)>=0)
          EPMASTER->GiveExplorationPoint(key, EP_CMD);
        return ret;
      }
  }
  return 0;
}

void init() {
  mixed cmds;
  
  if (mappingp(cmds=QueryProp(P_COMMANDS)))
    add_action("_cl","",1);
}

// **** local property methods
static mapping _set_commands(mapping commands)
{
  if(mappingp(commands)) Set(P_COMMANDS, commands);
  return Query(P_COMMANDS);
}
