// SilberLand MUDlib
//
// std/player/restrictions.c -- container aspect of players
//
// $Date: 1997/06/25 12:05:52 $                      
/* $Revision: 3.1 $       
 * $Log: restrictions.c,v $
 * Revision 3.1  1997/06/25 12:05:52  Woody
 * _query_max_weight() anders, Nachteile fuer Froesche
 *
 * Revision 3.0  1997/03/13 01:44:34  Woody
 * prepared for revision control
 *
 * Revision 2.11  1994/11/21  19:10:16  Jof
 * *** empty log message ***
 *
 * Revision 2.11  1994/11/21  19:10:16  Jof
 * *** empty log message ***
 *
 * Revision 2.10  1994/11/03  00:48:05  Jof
 * *** empty log message ***
 *
 * Revision 2.9  1994/10/04  09:54:21  Jof
 * *** empty log message ***
 *
 * Revision 2.9  1994/10/04  18:30:22  mud
 * *** empty log message ***
 *
 * Revision 2.8  1994/06/25  01:22:56  Jof
 * *** empty log message ***
 *
 * Revision 2.7  1994/06/17  09:12:20  Jof
 * ,
 *
 * Revision 2.6  1994/06/13  07:36:24  Jof
 * Removed SET_METHODs
 *
 * Revision 2.5  1994/05/26  19:31:28  Jof
 * Fehlende Includes (GRUMMEL)
 * ,#
 *
 * Revision 2.4  1994/05/26  19:27:37  Jof
 * GHOST-Abfrage ...
 *
 * Revision 2.3  1994/03/17  23:36:55  Jof
 * PROTECTED -> SECURED & Reindented
 *
 * Revision 2.2  1994/02/24  13:03:14  Jof
 * Private removed
 *
 * Revision 2.1  1994/01/17  23:35:05  mud
 * *** empty log message ***
 *
 * Revision 2.0  1994/01/13  16:59:09  mud
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/05  16:17:07  mud
 * changed save variables partly to mapping entry
 *             
 */                            

// This is a simple container to put objects in. It defines all functions
// which are necessary to describe an object which can be filled with
// other things.
//
// It will support restrictions for volume, weight etc.
//
// The following properties are defined:
// P_MAX_WEIGHT - maximum weight which container can carry
// P_WEIGHT_CONTENTS - current contents
// P_WEIGHT - builtin property: read->total weight, write->own weight
//
// Functions for manipulation of weight
// MayAddWeight(weight) - Can <weight> be inserted?
// AddWeight(weight) - Add an amount of <weight>
//
// IMPORTANT: unit should be interpreted as grams (g).

inherit "/std/container/restrictions";

#pragma strong_types

#define NEED_PROTOTYPES

#include "/sys/thing/properties.h"
#include <properties.h>
#include <wizlevels.h>
#include <container.h>
#include <attributes.h>
#include <defines.h>

// local properties prototypes
static int _query_max_weight();

void create()
{
  ::create();
  
  Set(P_MAX_WEIGHT, -1, F_SET_METHOD);
  Set(P_MAX_WEIGHT, SECURED, F_MODE);
}

// **** local property methods
static int _query_max_weight()
{
  int str,val;

  if (QueryProp(P_GHOST) && !IS_WIZARD(ME))
    return 0;
  str=QueryAttribute(A_STR);

  if (str<0) val=9200+str*160;
  else val=9200+str*800;

  if (val<3000) val=3000;
  return val;
}

static object *InsertHooks=({});

varargs int PreventInsert(object ob)
{
  int i;
  
  i=sizeof(InsertHooks)-1;

  while (i>=0)
	{
    if (!objectp(InsertHooks[i]) || environment(InsertHooks[i])!=ME)
      InsertHooks=InsertHooks[0..i-1]+InsertHooks[i+1..];
    else
      InsertHooks[i]->InsertNotify(ob);
		i--;
  } 
  return 0;
}

void AddInsertHook(object ob)
{
  if (member(InsertHooks,ob)!=-1 || environment(ob)!=this_object())
    return;
  InsertHooks+=({ob});
}

void RemoveInsertHook(object ob)
{
  InsertHooks-=({ob});
}

object *QueryInsertHooks()
{
  return InsertHooks;
}

static mixed _set_frog(mixed arg) {
  mixed res;

  res=Set(P_FROG,arg);
  if (res)
    SetProp(P_ATTRIBUTES_MODIFIER,({"#frosch",([A_STR:-20, A_INT:-10])}));
  else
    SetProp(P_ATTRIBUTES_MODIFIER,({"#frosch",0}));
  return res;
}
