#pragma strong_types

//inherit "std/room";

#include <properties.h>
#include <wizlevels.h>
#include <moving.h>
#include <environment.h>


#define LXP_LISTE "/p/service/querolin/listen/stat_lxp.c"
#define FP_LISTE  "/p/service/querolin/listen/stat_fp.c"
#define PLAYERCOUNT  "/p/service/querolin/playercount.c"

void create()
{
  //SetProp(P_ENVIRONMENT,([ENV_TEMPERATURE:20,ENV_TERRAIN:TT_CAVE]));
  call_out("invcheck",120);
  "/obj/sperrer"->NixGibts();

  call_other(FP_LISTE, "???");
  call_other(LXP_LISTE, "???");
  call_other(PLAYERCOUNT, "???");

}

int weg(object ob)
{
  if (!objectp(ob))
    return 0;
  ob->remove();
  if (ob)
    destruct(ob);
  return 1;
}

int wegraeumen(object ob)
{
  object *x;
  
  if (!objectp(ob))
    return 0;
  for (x=deep_inventory(ob);sizeof(x);x=x[1..])
    weg(x[0]);
  ob->move("/room/void",M_NOCHECK|M_SILENT);
  weg(ob);
  return 1;
}

void invcheck()
{
  object *inv;
  int i;
  
  while(remove_call_out("invcheck")!=-1);
  inv=all_inventory(this_object());
  for (i=sizeof(inv)-1;i>0;i--)
    if (interactive(inv[i]))
      set_object_heart_beat(inv[i],1);
    else
      if (!query_once_interactive(inv[i]) && object_name(inv[i])!="/obj/sperrer")
	call_out("wegraeumen",1,inv[i]);
  call_out("invcheck",120);
  "/obj/sperrer"->upd();
}

void init()
{
  if (!this_player())
    return;

  LXP_LISTE->Register(this_player());
  FP_LISTE->Register(this_player());
  PLAYERCOUNT->Register(this_player());
  tell_object(find_player("querolin"),break_string("NETZTOT->Register aufgerufen.\n",78));

  catch(this_player()->StopHuntingMode());
  if (interactive(this_player()))
    catch(this_player()->move("room/gilde",M_GO|M_NOCHECK));
  if (!query_once_interactive(this_player()))
  {
    this_player()->remove();
    if(this_player())
      destruct(this_player());
  }
  set_object_heart_beat(this_player(),0);
}

varargs string int_long()
{
  return "Dies ist der Netztotenraum. Es ist dunkel. Du siehst nichts.\n";
}

varargs string int_short()
{
  return "Nichts zu sehen.\n";
}

string QueryProp(string str)
{
  switch (str) {
    case "int_long": return int_long();
    case "int_short": return int_short();
  }
  return "";
}
