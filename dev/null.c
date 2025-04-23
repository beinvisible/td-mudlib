/* /dev/null
 *
 * Wehe das ding loescht nochmal wer, dann loesch ich den loescher ;)
 *
 * 05/02/2004 Serii
 *
 */

inherit "std/room";

#include <properties.h>
#include <wizlevels.h>
#include <language.h>
#include <moving.h>
#include <player.h>

#include "/d/unterwelt/def.h"

void create()
{
  ::create();
  SetProp(P_LIGHT,0);
  SetProp(P_INT_SHORT, "/dev/null");
  SetProp(P_INT_LONG, " --- NIX --- ");
  SetProp(P_INDOORS,0);
}

void ca_d(object tp)
{
  tp->remove();
  if(tp)destruct(tp);
}

void init()
{object tp;
  ::init();
  tp=this_player();
  if(IS_GOD(tp))return;
  if(query_once_interactive(tp)){tp->move("/room/jail",M_GO|M_SILENT);return;}
  tp->move("/dev/tlg",M_NOCHECK);
  call_out("ca_d",0,tp);
}
