/* Tamedhon MUDLIB
 * Lehrlingsshell - Magier Stufe 10..19
 * Samtpfote@Tamedhon
 *
 * Diese Shell ist fuer Lehrlinge und Magier in Ruhestand gedacht,
 * die da eher eingeschraenkt werden sollen.
 *
 * Rechte:
 * 	- Leserecht auf /doc
 *  - ls, cat, more, head, tail, pwd, cd
 *  - goto, mschau, home
 *
 * $Date: 2001/01/28 19:03:14 $
 * $Revision: 1.2 $
 * $Log: lehrling.c,v $
 * Revision 1.2  2001/01/28 19:03:14  Samtpfote
 * home funzt jetzt bei Magiern > 20 endlich
 *
 * Revision 1.1  2001/01/13 13:49:48  Samtpfote
 * Initial revision
 *
 */

#include <properties.h>
#include <wizlevels.h>
#include <moving.h>

#define TO				this_object()
#define MAXIDLE			386400

inherit "/std/player/base";
inherit "/std/shells/filesys/filesys";

/* -- Kommandos ---------------------------------------------------- */
static int cmd_home()
{
  

  if(!IS_LEARNER(TO)) return 0;

  return 
  	move(
	    (IS_WIZARD(this_player())
		?"/players/"+getuid(this_player())+"/workroom"
		:"/room/lehrlingsbude"), 
	M_GO, "in die Lehrlingsbude");
}

static int cmd_teleport(string dest)
{
  object	ob;
  string	path;
  mixed		ret;

"/room/debug"->debug_message(TO, sprintf("%O",IS_LEARNER(TO)));  
  if(!IS_LEARNER(TO)) return 0;
  
  dest=_unparsed_args();
  if(!dest)
  {
	write("Wohin möchtest Du Dich teleportieren?\n");
	return 1;
  }
  
  if(ob=find_living(dest))
	if(ob=environment(ob))
	{
	  move(ob, M_TPORT|M_NOCHECK);
	  return 1;
	}
  
  path=MASTER->_get_path(dest, getuid(TO));
  ret=catch(call_other(path, "???"));
  if(ret && file_size(path) < 0)
  {	
	dest=match_living(dest, 1);
	if(stringp(dest) && (ob=environment(find_living(dest))))
	{
	  move(ob, M_TPORT|M_NOCHECK);
	  return 1;
	}
	
	printf("Fehler beim Teleport nach %O.\n", path);
	return 1;
  }
  
  if(ret)
  { printf("Fehler beim Teleport nach %O:\n%O\n", dest, ret); }
  
  ret=catch(move(path, M_TPORT|M_NOCHECK));
  if(ret)
  {	printf("Fehler beim Teleport nach %O:\n%O\n", dest, ret); }
  
  return 1;
}

int cmd_mschau(string str)
{
  int level;
  
  if(this_interactive() != TO) { return 0; }
  if( str == "ja" || str == "an" || str == "ein" )
  {
    if(QueryProp(P_WANTS_TO_LEARN))
	{
	  write("Du hast den Magier-Modus schon eingeschaltet!\n");
	}
    else
	{
	  write( "Du hast jetzt den Magier-Modus eingeschaltet.\n");
	}
    SetProp(P_WANTS_TO_LEARN, 1);
  }
  else if( str == "nein" || str == "aus")
  {
    if(QueryProp(P_WANTS_TO_LEARN))
	{
	  write("Ok, Magier-Mode aus.\n");
	}
	else
	{
	  write("Magier-Mode war schon aus.\n");
	}	  
    SetProp(P_WANTS_TO_LEARN, 0);
  }
  else
  {
	write("Was willst Du? 'mschau ein' oder 'mschau aus'?\n");
  }
  	
  return 1;
}


// ------------
static mixed *_query_localcmds()
{
  return
	({  /* kommando    funktion       ?   level       */
	  ({"localcmd",	"local_commands", 0, LEARNER_LVL}),
	  ({"goto", "cmd_teleport", 0, LEARNER_LVL}), 
	  ({"mschau", "cmd_mschau", 0, LEARNER_LVL}),
	  ({"home", "cmd_home", 0, LEARNER_LVL}),
	}) +
	base::_query_localcmds()+
	filesys::_query_localcmds();
}

/* -- Standardfunktionen ------------------------------------------- */
varargs void Reconnect(int silent, string myip)
{
  base::Reconnect(silent, myip);
  filesys::reconnect();
}

static void FinalSetup()
{
  SetProp(P_CURRENTDIR, "/doc");
}

varargs int remove()
{
    if(!IS_WIZARD(this_player()))
    {
        call_other("/room/lehrlingsbude", "???");
        this_player()->move("/room/lehrlingsbude", M_TPORT|M_NOCHECK);    
    }

    return(::remove());
}

void reset()
{
  int	idle;

  if(!query_once_interactive(this_object())) { return; }
  if(!interactive(this_object()))
  {
    quit();
    if(this_object()) { remove(); }
    return;
  }
  
  if(((idle=query_idle(this_object())) > MAXIDLE)&&!IS_ARCH(this_object()))
  {
    printf("%s war seit %s Minuten idle und wurde automatisch ausgeloggt.\n",
	  capitalize(getuid()),
	  time2string("%h %H und %m %M", query_idle(this_object())));
    tell_object(this_object(),
	  "Automatischer Logout wegen zu langer Idlezeit.\n");
    this_object()->quit();
    if (this_object()) remove();
    if (this_object()) destruct(this_object());
  }

}

void create()
{
  base::create();
  
  SetProp(P_RACE, "Magier");
  
  SetDefaultHome("/room/lehrlingsbude");
}
