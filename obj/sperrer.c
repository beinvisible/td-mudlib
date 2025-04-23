#include <properties.h>
#include <moving.h>
#include <defines.h>
#include <config.h>
#include <wizlevels.h>

#define PO previous_object()
#define TI this_interactive()

void upd();

void create()
{
	if (clonep(ME))
	{
		write("Autodest\n");
		// this_object()->move("/dev/null", M_NOCHECK);
		destruct(this_object());
	}
	upd();
}

void upd()
{
  this_object()->move("/room/netztot", M_NOCHECK);
}

void debug(mixed what, mixed arg)
{
	string log;

	if (!environment())
		return;
	log=sprintf("%s %s %O PO=%O PPO=%O TO=%O TI=%O verb=%O\n",dtime(time()),what,arg,previous_object(),previous_object(1),this_object(),this_interactive(),query_verb());
	log_file("NDEAD",log);
	if (find_player("debugger"))
		tell_object(find_player("debugger"),log);
	if (extern_call() && previous_object() && 
	    (!interactive(PO)||IS_LEARNER(PO)) && 
	    getuid(PO)!=ROOTID && PO!=this_object())
	{
		log_file("NDEAD",sprintf("DELETED %O\n",PO));
		catch(PO->remove());
		if (PO && !(PO==this_object()) && object_name(PO)!="/secure/simul_efun")
			destruct(PO);
		if (TI && IS_LEARNER(TI))
		{
			tell_object(TI,"MESSING WITH NETDEAD ROOM - STOP\n");
			TI->remove();
			if (TI)
				destruct(TI);
			log_file("NDEAD",sprintf("STOPPED %O\n",TI));
		}
         raise_error("DONT MESS WITH NETDEAD\n");
	}
}

void reset()
{
	upd();
}

varargs void id(string str)
{
	debug("id",str);
}

void Query(mixed what)
{
	debug("query",what);
}

void remove()
{
	debug("remove",0);
}

varargs string long()
{
	debug("long",0);
	return "";
}

varargs string short()
{
	debug("short",0);
	return "";
}
