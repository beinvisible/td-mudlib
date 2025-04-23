// SilberLand MUDlib
//
// std/thing/moving.c -- object moving
//
// $Date: 2008/06/15 02:00:00 $
/* $Revision: 3.5 $
 * $Log: moving.c,v $
 *
 * Revision 3.5  2008/06/15 02:00:00  TamTam
 * Bugfix bei Aufruf von TryMove() (if .. if .. else)
 *
 * Revision 3.4  2004/09/03 22:00:00  Serii
 * Bugfix beim move-hook
 *
 * Revision 3.3  2004/04/15 08:00:00  Serii
 * P_TMP_MOVE_HOOK gibts jetz auch fuer 'tote' sachen :)
 *
 * Revision 3.2  2001/02/10 12:07:59  Samtpfote
 * prepared for revision control
 *
 * Revision 3.2  1997/10/16 15:49:30  Woody
 * move() uebergibt jetzt this_object() an MayAddWeight(), statt Gewicht
 *
 * Revision 3.1  1997/06/25 12:10:06  Woody
 * PreventLeave() aufrufen (1 -> ME_CANT_LEAVE_ENV)
 *
 * Revision 3.0  1997/03/13 02:31:21  Woody
 * prepared for revision control
 *
 * Revision 2.9  Feb 97  Woody
 * Korrekte Reihenfolge bei RemoveSensitiveObject(),
 * Zudem (wenn sensitiv) bei remove() erstmal in room/void moven
 *
 * Revision 2.8  1995/11/09 14:35:23  Rochus
 * Objekte, bei denen P_SENSITIVE gesetzt ist, werden
 * an/abgemeldet, damit z.B. ein Spieler-Objekt weiss,
 * dass es etwas feuerempfindliches im Inventory hat :-)
 *
 * Revision 2.7  1995/06/02 12:58:05  Rumata
 * move made varargs
 *
 * Revision 2.6  1994/10/04  09:24:02  Jof
 * *** empty log message ***
 *
 * Revision 2.6  1994/10/03  21:46:25  mud
 * Optimizations
 *
 * Revision 2.5  1994/07/10  17:03:08  Jof
 * rmoved _filter_remove
 *
 * Revision 2.4  1994/06/25  01:23:10  Kirk
 * PreventInsert() called in every case in move() to supply InsertHook list
 *
 * Revision 2.3  1994/03/18  11:56:25  Jof
 * Fixed indentation
 *
 * Revision 2.2  1994/03/07  17:15:49  Jof
 * some includes added
 *
 * Revision 2.1  1994/03/07  17:12:55  Jof
 * removed some this_object()-> from move()
 *
 * Revision 2.0  1994/01/13  17:00:31  mud
 * *** empty log message ***
 *
 * Revision 1.4  1994/01/04  10:46:50  mud
 * NEED_PROTOTYPES added
 *
 * Revision 1.3  1994/01/03  18:31:21  mud
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/03  12:36:47  mud
 * *** empty log message ***
 *
 */

#pragma strong_types

#include <properties.h>
#include <moving.h>
#include <new_skills.h> //fuer P_TMP_MOVE_HOOK

#define NEED_PROTOTYPES

#include <thing/properties.h>

mixed _query_move_hook()
{
  return mappingp(Query(P_TMP_MOVE_HOOK))?m_values(Query(P_TMP_MOVE_HOOK)):({});
}

varargs void _set_move_hook(mixed h)
{mapping mh;
string *inds;
int lv;
  mh=Query(P_TMP_MOVE_HOOK);
  if(!mappingp(mh))mh=([]);
  if(objectp(h))
  {
    if(mh[h]==h)
      mh=m_delete(mh,h);
    else
      mh[h]=h;
    Set(P_TMP_MOVE_HOOK,mh);
    return;
  }
  if(pointerp(h)&&(sizeof(h)>=3))
  {
    mh[h[1]]=h;
    Set(P_TMP_MOVE_HOOK,mh);
  }
}

// move()
//
// This is called every time someone wants to move us. No message is
// given currently (this behaviour can be changed in livng objects
// that want to be noisy when leaving aroom and entering another or
// the source or destination object can give messages (dropping or
// taking things)
// a retvalue of 1 means success to move, <=0 means failure.

static int TryMove(mixed dest, int method)
{
    if (QueryProp(P_NODROP) &&  (method & (M_PUT|M_GIVE)))
      return ME_CANT_BE_DROPPED;
    if (QueryProp(P_NOGET) &&  (method & M_GET))
      return ME_CANT_BE_TAKEN;
    if (dest->MayAddWeight(this_object()))
      return ME_TOO_HEAVY;
    if (dest->PreventInsert(this_object()))
      return ME_CANT_BE_INSERTED;
    if (environment() && environment()->PreventLeave(this_object(),dest))
      return ME_CANT_LEAVE_ENV;
  return 1;
}

varargs int move(mixed dest,int method) {
int lv;
  object source;
  mixed sens;
  mixed res,res2;

  if (!(method & M_NOCHECK))
  {
    if((res=TryMove(dest,method))<0)return res;
  }
  else {
          dest->PreventInsert(this_object());
          if (environment())
            environment()->PreventLeave(this_object(),dest);
       }

  if (sizeof(res=QueryProp(P_TMP_MOVE_HOOK)))
  {
for(lv=0;lv<sizeof(res);lv++)
{
    if (pointerp(res[lv]) && sizeof(res[lv])>=3
        && intp(res[lv][0]) && time()<res[lv][0]
        && objectp(res[lv][1]) && stringp(res[lv][2]))
    {
      if ((res2=call_other(res[lv][1],res[lv][2],dest,method)) && pointerp(res2) && sizeof(res2)==2)
      {
        dest=res2[0];
        method=res2[1];
      }
    }
    else if(objectp(res[lv]))
      SetProp(P_TMP_MOVE_HOOK,res[lv]);
    else
      SetProp(P_TMP_MOVE_HOOK,res[lv][1]);
}
  }

  source = environment();
  move_object( dest );
  if (source != environment())
  {
    sens=QueryProp(P_SENSITIVE);
    if (sens && source)
      source->RemoveSensitiveObject(this_object());
    if (sens && environment())
      environment()->InsertSensitiveObject(this_object(),sens);
  }

  return 1;
}

// remove()
//
// This is called when someone wants to destruct us. Objects normally
// can be destructed - well, this is only a simulation, someone could
// leave a nice message "The xxx turns onto a terrible blast of Energy"
// if he wants :)
// You also can redefine it if you want...
// 1 means success, destruction complete, 0 is failure.

varargs int remove() {
  object ob;
  ob=environment();
  if (ob && QueryProp(P_SENSITIVE)) {
    move_object("/room/void");
    ob->RemoveSensitiveObject(this_object());
  }
  destruct(this_object());
  return 1;
}
