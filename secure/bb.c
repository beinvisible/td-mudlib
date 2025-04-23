#include <properties.h>
#include <moving.h>
#include <wizlevels.h>
#include <config.h>

inherit "std/thing";

#define DB(x) tell_object(find_player("samtpfote"), "[bb] "+x+"\n")

object	pl;

varargs int id() { return 0; }
varargs int remove() { return 0; }

int alles(string str)
{

/*	DB(sprintf("pl=%O", pl));
 *	DB("str="+str);
 */

	if(!pl) return 0;
	//if(!str) return 0;
	write_file("/log/ARCH/bibr/"+getuid(pl),
		   ctime(time())+" "+query_verb()+" "+pl->_unparsed_args()+"\n");
	return 0;
}


object activate()
{
	pl=environment(this_object());
	seteuid(ROOTID);

        // Benutzung des BB-Tools verzeichnen [morpheus]
        write_file("/log/LORD/bblog", ctime(time()) + ": " + getuid(pl) +
                                      " hat einen BigBrother bekommen.\n");
	return pl;
}

void create()
{
	if(!clonep(this_object())) return 0;
	::create();

	AddId("tamebb");
	SetProp(P_SHORT, 0);
	SetProp(P_NEVERDROP, 1);
	SetProp(P_NODROP, 1);
	SetProp(P_AUTOLOADOBJ, 1);

	pl=0;

	AddCmd("", "alles", 1);
}
