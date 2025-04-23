/* Tamedhon MUDLIB
 * Erzmagier - Magier Stufe 60+
 * Samtpfote@Tamedhon
 *
 * Diese Shell ist fuer die erweiterten Faehigkeiten der Erzmagier
 * gedacht, die auch Sicherheitsrelevantes angehen.
 *
 * $Date: 2001/01/19 20:02:09 $
 * $Revision: 1.3 $
 * $Log: erzmagier.c,v $
 * Revision 1.3  2001/01/19 20:02:09  Samtpfote
 * 'home' gefixt.
 *
 * Revision 1.2  2001/01/15 13:47:10  Samtpfote
 * kleiner bugfix
 *
 * Revision 1.1  2001/01/13 20:13:01  Samtpfote
 * Initial revision
 *
 *
 */

#include <properties.h>
#include <wizlevels.h>
#include <moving.h>

#define ME		this_player()

inherit "/std/shells/magier";

/* ------------------------------------------------------------- */
static int cmd_go_home() 
{
  if (!QueryProp(P_WANTS_TO_LEARN))
    return 0;
  return move("players/" + getuid() + "/workroom", 
                   M_GO, "nach Hause");
}


static int cmd_shut_down_game(string str)
{
  if (!IS_ARCH(ME) || ME!=this_interactive()) { return 0; }
  _notify_fail("Du musst einen Grund dafür angeben.\n");
  if (!str) return 0;
  shutdown(str);
  return 1;
}

static int cmd_add_domain_master(string str)
{
  string master, domain;

  if (this_object() != this_interactive() || 
      query_wiz_level(this_interactive())<60)
  {
    write("Das darfst Du nicht !\n");
    return 1;
  }
  _notify_fail("Syntax: addmaster <user> <domain>\n");
  if (!str) return 0;
  if (sscanf(str,"%s %s",master,domain)!=2) return 0;
  if (!master || !domain) return 0;
  if (!"/secure/master"->add_domain_master(master,domain))
    write("Hat nicht funktioniert.\n");
  else 
    write("Ok.\n");
  return 1;
}

static int cmd_remove_domain_master(string str)
{
  string master, domain;

  if (this_object() != this_interactive() || 
      query_wiz_level(this_interactive())<60)
  {
    write("Das darfst Du nicht !\n");
    return 1;
  }
  _notify_fail("Syntax: removemaster <user> <domain>\n");
  if (!str) return 0;
  if (sscanf(str,"%s %s",master,domain)!=2) return 0;
  if (!master || !domain) return 0;
  if (!"/secure/master"->remove_domain_master(master,domain))
    write("Hat nicht funktioniert.\n");
  else
    write("Ok.\n");
  return 1;
}

// ------------
static mixed *_query_localcmds()
{
  return
	magier::_query_localcmds()+
	({
		({"home", "cmd_go_home", 0, WIZARD_LVL }),
		({"shutdown", "cmd_shut_down_game", 0, ARCH_LVL }),
		({"addmaster", "cmd_add_domain_master", 0, ARCH_LVL }),
		({"removemaster", "cmd_remove_domain_master", 0, ARCH_LVL })		
	});
}

/* ------------------------------------------------------------- */
void create()
{
  magier::create();
}
