// Tamedhon MUDlib
//
// std/room/items.c -- creating extra items in room
//
// $Date: 2006/06/01 12:14:00 $                      
/* $Revision: 3.3 $       
 * $Log: items.c,v $
 * Revision 3.3  2006/06/01 12:14:00  Serii
 * random bei AddUniqueItem()
 *
 * Revision 3.2  2004/03/12 14:00:00  Serii
 * AddUniqueItem() eingefuegt
 *
 * Revision 3.1  1997/06/25 12:08:53  Woody
 * REFRESH_INIT in init() behandeln, RemoveItem() eingefuehrt
 *
 * Revision 3.0  1997/03/13 02:02:45  Woody
 * prepared for revision control
 *
 * Revision 2.25  1996/03/11  02:05:00  Mupfel
 * Konstanten aus /sys/room/items.h statt /sys/rooms.h
 *
 * Revision 2.24  1995/02/08  02:05:26  Rochus
 * Mit 1 anstelle des Property-Mappings kann man jetzt
 * die Blueprint anstelle eines gecloneten Objektes nehmen
 * (1==UNIQUE)
 *
 * Revision 2.23  1995/02/06  18:49:04  Jof
 * remove REF_NONE entries w/ destructed obj from list
 *
 * Revision 2.22  1994/12/01  13:29:02  Jof
 * clean_double
 *
 * Revision 2.21  1994/11/28  09:17:37  Jof
 * cleaner ...
 *
 * Revision 2.20  1994/10/04  09:55:26  Jof
 * *** empty log message ***
 *
 * Revision 2.20  1994/10/04  14:41:59  mud
 * *** empty log message ***
 *
 * Revision 2.19  1994/08/17  09:56:43  Jof
 * *** empty log message ***
 *
 * Revision 2.18  1994/08/16  17:35:10  Jof
 * *** empty log message ***
 *
 * Revision 2.17  1994/07/19  16:14:08  Kirk
 * *** empty log message ***
 *
 * Revision 2.16  1994/07/12  00:09:10  Kirk
 * clean up
 *
 * Revision 2.15  1994/03/21  16:48:13  Jof
 * Reindented files using emacs - once again
 *
 * Revision 2.14  1994/03/20  17:05:51  Hate
 * #include <daemon.h> and OBJECTD instead of direct strings
 *
 * Revision 2.13  1994/03/14  22:35:54  Hate
 * Added a call to the persisten object daemon in create()
 * changed AddItem() to be able to configure object via 3rd argument
 *
 * Revision 2.12  1994/03/10  19:39:58  Jof
 * Bug in create fixed.
 *
 * Revision 2.11  1994/03/10  19:22:52  Jof
 * /
 *
 * Revision 2.10  1994/03/10  18:38:29  Hate
 * AddItem() now returns the object it cloned and "/players/jonas/schilderm"
 * is added.
 *
 * Revision 2.9  1994/02/24  13:04:21  Jof
 * Private removed
 *
 * Revision 2.8  1994/02/18  12:11:35  Jof
 * another bug in do_refresh fixed ...
 *
 * Revision 2.7  1994/02/17  13:21:01  Jof
 * check for pointerp in do_refresh
 *
 * Revision 2.6  1994/02/16  16:42:00  Jof
 * Fixed nasty refresh-bug
 *
 * Revision 2.3  1994/02/15  17:46:34  Hate
 * changed item implementation to array from mapping
 *
 * Revision 2.2  1994/01/19  20:32:54  hate
 * *** empty log message ***
 *
 * Revision 2.0  1994/01/13  17:00:24  mud
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/08  13:08:41  mud
 * changed to #pragma strong_types
 *             
 */                            

// extra items handling
//
//	AddItem(string filename, int refresh)
//	  Clones an item and puts it into the room. <refresh> may be
//	  on of the following:
//	  REFRESH_NONE: No refresh done until reboot.
//	  REFRESH_DESTRUCT: Refresh on reset if item was destructed.
//	  REFRESH_REMOVE: Refresh on reset if item was removed from room.
//	  REFRESH_ALWAYS: Create a new clone on every reset.
//	  REFRESH_INIT:   Create a new clone every time a living enters
//
// The commands are implemented as properties P_ITEMS as mapping. They are 
// stored locally (_set_xx) as mapping to speed up the routines 
// in this module.
// 
// Format of key and data in the P_ITEMS mapping:
//
// ([ key1 : refresh1; obp1; arr1, ..., keyn : refreshn; obpn; arrn ])

#include "/std/sys_debug.h"
#pragma strong_types

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <room/items.h>
#include <defines.h>
#include <config.h>
#include <properties.h>
#include <moving.h>
#include <daemon.h>

void create()
{ 
  SetProp(P_ITEMS,({}));
  OBJECTD->QueryObject();  // querying general objects
}

/* Item handling */

varargs object AddItem(mixed filename, int refresh, mixed props) 
{
  string file,fn1,fn2;
  object ob;
  int i;
//UI
if(objectp(filename))
{
  ob=filename;
//  if(call_other("/p/daemon/UIMaster","UI_is_ui"))
    filename=({"###",explode(object_name(ob),"#")[0],call_other("/p/daemon/UIMaster","UI_get_si",explode(object_name(ob),"#")[0])});
//  else filename=explode(object_name(ob),"#")[0];
}
//IU
  else if(pointerp(filename)) 
  {
    for(i = 0; i < sizeof(filename); i++)
      filename[i] = MASTER->_get_path(filename[i],"?");
    file = filename[random(sizeof(filename))];
  }
  else 
    file = filename = MASTER->_get_path(filename,"?");
  if ((props==1)&&(!ob)) {
    if(catch(call_other(file,"???")))
      raise_error(sprintf("AddItem(): %O does not exist or is not loadable\n",
                          file));
    ob=find_object(file);
  } else if(!ob) {
    if(catch(ob=clone_object(file)))
      raise_error(sprintf("AddItem(): %O does not exist or is not loadable\n",
                          file));
  }
  ob->move(this_object(), M_NOCHECK | M_NO_ATTACK);
  SetProp(P_ITEMS, QueryProp(P_ITEMS)
	  +({ ({ ob,        // RITEM_OBJECT
                 filename,  // RITEM_FILE
                 refresh    // RITEM_REFRESH
		 }) 
		+ ((mappingp(props) || props==1)?({ props }):({}))
		}));
  if(mappingp(props)) 
    walk_mapping(props, symbol_function("SetProp", ob)); 
  return ob;
}

varargs object AddUniqueItem(string *filenames, int refresh, int rand ,mixed props)
{
  if(!filenames)
  {
    raise_error("Wrong arguments to AddUniqueItem()\n");
    return 0;
  }
  if(sizeof(filenames)!=2)
  {
    raise_error("Wrong number of filenames in AddUniqueItem()\n");
    return 0;
  }
  if(!rand||rand<2)return AddItem(call_other("/p/daemon/UIMaster","UI_clone_object",filenames[0],filenames[1],(props==1?1:0)),refresh,props);
  if(!random(rand))return AddItem(call_other("/p/daemon/UIMaster","UI_clone_object",filenames[0],filenames[1],(props==1?1:0)),refresh,props);
  return AddItem(filenames[1],(props==1?1:0),refresh,props);
}

static void ri_rem_ob(object ob)
{
  object *inv;
  int i;

  if (objectp(ob) && present(ob)) {
    inv = deep_inventory(ob);
    for (i=sizeof(inv)-1; i>=0; i--) {
      if (inv[i]) {
        inv[i]->remove(1);
        if (inv[i])
          destruct(inv[i]);
      }
    }
    ob->remove(1);
    if (ob)
      destruct(ob);
  }
}

static int ri_filter(mixed *ritem, mixed file)
{
  object ob, *inv;
  int i;

  ob = ritem[RITEM_OBJECT];

  if (stringp(file)) {
    if (stringp(ritem[RITEM_FILE]) && ritem[RITEM_FILE] == file) {
      ri_rem_ob(ob);
      return 0;
    }
  }
  else {
    if (pointerp(ritem[RITEM_FILE]) &&
        ((sizeof(file&ritem[RITEM_FILE]) == sizeof(ritem[RITEM_FILE]))
//UI
	||(ritem[RITEM_FILE][0]=="###")
//IU
	)) {
      ri_rem_ob(ob);
      return 0;
    }
  }
  return 1;
}

void RemoveItem(mixed filename)
{
  mixed *items;
  int i;

  if (!pointerp(items=QueryProp(P_ITEMS)) || !sizeof(items))
    return;

  if (pointerp(filename)) {
    for(i = sizeof(filename)-1; i >= 0; i--)
      filename[i] = MASTER->_get_path(filename[i],"?");
  }
  else
    filename = MASTER->_get_path(filename, "?");

  items=filter(items, #'ri_filter/*'*/, filename);
  SetProp(P_ITEMS, items || ({}));
}

static mixed _do_refresh(mixed item)
{
  string file;
  object ob;
  ob=0;
  if (!pointerp(item) || item[RITEM_REFRESH] == REFRESH_NONE)
    return item;

//UI
  if(pointerp(item[RITEM_FILE])&&(item[RITEM_FILE][0]=="###"))
      ob=call_other("/p/daemon/UIMaster","UI_clone_object",item[RITEM_FILE][1],item[RITEM_FILE][2],(sizeof(item)>3?item[RITEM_PROPS]==1?1:0:0));
//IU
  else if(pointerp(item[RITEM_FILE]))
    file = item[RITEM_FILE][random(sizeof(item[RITEM_FILE]))];
  else file = item[RITEM_FILE];

  switch(item[RITEM_REFRESH])
  {
    case REFRESH_NONE:
    case REFRESH_INIT:
    if (!item[RITEM_OBJECT]) return 0;
    break;
    case REFRESH_DESTRUCT:
    if(objectp(item[RITEM_OBJECT]))
      break; // else FALL THROUGH
    case REFRESH_REMOVE:
    if(objectp(item[RITEM_OBJECT]) && environment(item[RITEM_OBJECT]) == ME)
      break; // else FALL THROUGH
    default:
    if ((!ob) && (sizeof(item)>RITEM_PROPS) && (item[RITEM_PROPS]==1)) {
      call_other(file,"???");
      ob=find_object(file);
    } else if(!ob) {
      ob=clone_object(file);
    }
    ob->move(ME,M_NOCHECK|M_NO_ATTACK);
    break;
  }
  if (ob)
  {
    item[RITEM_OBJECT] = ob;
    if(sizeof(item) > RITEM_PROPS && mappingp(item[RITEM_PROPS]))
      walk_mapping(item[RITEM_PROPS], symbol_function("SetProp", ob));
  }
  return item;
}

static mixed removeable_ob(object ob)
{
  if (!interactive(ob) && (!living(ob)
			   && !query_once_interactive(ob) &&
			   stringp(ob->short())))
    return ob;
  else
    return 0;
}

static int _sortfunc(mixed *a, mixed *b)
{
  return a[0]<b[0];
}

void clean_double()
{
  mixed *inh,*items;
  int i,j;
  string ident;
  object ob;
  
  inh=all_inventory(this_object());
  if (!pointerp(inh) || !sizeof(inh)) return;
  inh=filter(inh,#'removeable_ob)-({0}); //')
  if (sizeof(inh)<10) return;
  items=Query(P_ITEMS);
  for (i=sizeof(items)-1;i>=0;i--) inh-=({items[i][0]});
  for (i=sizeof(inh)-1;i>=0;i--)
  {
    if (interactive(inh[i])) return;  // Nichts machen, falls Spieler anwesend
    ob=inh[i];
    inh[i]=({sprintf("%s%s%s",explode(object_name(ob),"#")[0],
		     ob->short(),ob->long()),ob});
  }
  inh=sort_array(inh,#'_sortfunc); //')
  ident="";
  for (i=sizeof(inh)-1;i>=0;i--)
    if (ident!=inh[i][0])
    {
      ident=inh[i][0];
      j=1;
    } else 
    {
      j++;
      if (j>3 && inh[i][1])
      {
	log_file("CLEAN_ROOM",sprintf("%s: %O in %O\n",dtime(time())[5..],inh[i][1],this_object()));
	catch(inh[i][1]->remove());
	if (inh[i][1])
	  destruct(inh[i][1]);
      }
    }
}

// reset handling: check how the items should be refreshed.
//
void reset() 
{
  if(object_name(ME)=="/std/room/items") return;
  if (!pointerp(QueryProp(P_ITEMS))) SetProp(P_ITEMS, ({}));
  SetProp(P_ITEMS, map(QueryProp(P_ITEMS), #'_do_refresh)-({0}));//'))
  funcall(#'clean_double);//')
}

void init()
{
  mixed file;
  mixed item;
  object ob;
  int i;

  item=QueryProp(P_ITEMS);
  for (i=sizeof(item)-1; i>=0; i--)
    if (item[i][RITEM_REFRESH]==REFRESH_INIT)
    {
      ob=0;
//UI
      if(pointerp(item[i][RITEM_FILE])&&(item[i][RITEM_FILE][0]=="###"))
        ob=call_other("/p/daemon/UIMaster","UI_clone_object",item[i][RITEM_FILE][1],item[i][RITEM_FILE][2],(sizeof(item[i])>3?item[i][RITEM_PROPS]==1?1:0:0));
//IU
      else if(pointerp(item[i][RITEM_FILE])) 
        file = item[i][RITEM_FILE][random(sizeof(item[RITEM_FILE]))];
      else file = item[i][RITEM_FILE];
      if (!ob && sizeof(item[i])>RITEM_PROPS && item[i][RITEM_PROPS]==1) {
        call_other(file,"???");
        ob=find_object(file);
      } else if(!ob) {
        ob=clone_object(file);
      }
      if (ob)
      {
        ob->move(ME,M_NOCHECK|M_NO_ATTACK);
        item[i][RITEM_OBJECT] = ob;
        if(sizeof(item[i]) > RITEM_PROPS && mappingp(item[i][RITEM_PROPS]))
          walk_mapping(item[i][RITEM_PROPS], symbol_function("SetProp", ob));
      }
    }
}

// local property methods
static mixed _set_items(mixed arg) 
{
  if(pointerp(arg)) return Set(P_ITEMS, arg);
  return 0;
}
