// Sonnenbrille by Kirk 
// (zugegebenermassen relativ ueberfluessig) ;-)
//
/* TAMEDHON: use it at your own caution!!!! Samtpfote.
 * 
 * $Date: 2001/01/13 13:19:39 $
 * $Revision: 1.1 $
 * $Log: sonnenbrille.c,v $
 * Revision 1.1  2001/01/13 13:19:39  Samtpfote
 * Initial revision
 *
 */

inherit "std/thing";

#include <properties.h>
#include <language.h>
#include <combat.h>
#include <moving.h>
#include <wizlevels.h>

#pragma strong_types

object owner;

string _query_short();
string _query_long();

void
create()
{
  if(!clonep(this_object()))
    return;
  owner = this_player();
  if (!IS_WIZARD(owner))
  {
    tell_object(owner, "Die Sonnenbrille zerfällt zu Staub.\n");
    call_out("remove",0);
    return;
  }
  ::create();
  AddId(({"sonnenbrille", "brille"}));
  SetProp(P_NAME, "Sonnenbrille");
  SetProp(P_SHORT, "Sonnenbrille");
  SetProp(P_LONG,
	  "Die Sonnenbrille schützt vor UV-Strahlen und verleiht Dir das "
	  "\n'gewisse Etwas`.\n");
  SetProp(P_GENDER, FEMALE);
  SetProp(P_NODROP, "Dann würdest Du sie sicher sofort vermissen.\n");
  SetProp(P_NOBUY, 1);
  SetProp(P_NEVERDROP, 1);
  SetProp(P_LIGHT, 1);
  SetProp(P_ARTICLE,1); 
  call_out("set_vars", 0);
}

void
init()
{
  ::init();
  if (IS_WIZARD(this_player())) 
  {
    add_action("ausruesten", "rueste", 1);
    add_action("ausruesten", "rüste", 1);
  }
  else 
  {
	call_out("remove", 0);
  }
}

void
InsertNotify(object ob)
{
  string *cl, cloner, str, arg1, sender;
  object obj;

  if (!IS_WIZARD(this_player()))
    return;
  
  if( ob->id("schnupfen") )
    return (void) call_out("destruct_it",0,ob);

  if (ob->id("death_mark"))
  {
    cl = explode((string)ob->QueryProp(P_CLONER),":");
    if(cl)
      if(sizeof(cl)>1)
	cloner = cl[1];
      else
	cloner = cl[0];
    else
      cloner = "UNKNOWN";
    tell_object(owner,sprintf(" *** [%O] vernichtet. (%s)\n", ob, cloner));
    call_out("destruct_it",0,ob);
    if(cloner!=getuid(owner) && (ob=find_player(cloner)))
      ob->disconnect();
    owner->SetProp(P_GHOST,0);
    if(object_name(environment(owner))=="/room/death/death_room")
      owner->move("/players/"+getuid(owner)+"/workroom",M_TPORT|M_SILENT);
    return;
  }

  if(ob->id("fluch"))
  {
    cl = explode((string)ob->QueryProp(P_CLONER),":");
    if(cl)
      if(sizeof(cl)>1)
	cloner = cl[1];
      else
	cloner = cl[0];
    else
      cloner = "UNKNOWN";
    tell_object(owner,sprintf(" *** [%O] vernichtet. (%s)\n", ob, cloner));
    call_out("destruct_it",0,ob);
    return;
  }
}

void
destruct_it(object ob)
{
  if(!ob) return;
  ob->remove();
  if(ob)
    destruct(ob);
}

#define GEBIRGE(x) "/d/gebirge/obj/"+x
#define SCHRECKEN(x) "/d/gebirge/boing/sm/obj/"+x
#define FRIEDHOF(x) "/players/boing/friedhof/obj/"+x

int
ausruesten(string str)
{
  object pl, ob;
  string wen;

  if (!IS_LEARNER(this_player()))
  {
    call_out("remove",0);
    return 1;
  }
  notify_fail("Verwende ein anderes tool!\n");
  return 0;
  
  notify_fail("syntax: rüste <testspieler> aus\n");

  if(!sscanf(str, "%s aus", wen)) return 0;
  if(!(pl=find_player(wen))) return 0;
  if(!(pl->QueryProp(P_TESTPLAYER))) return 0;

  (ob=clone_object(GEBIRGE("zmhelm")))->move(pl);
  pl->command_me("ziehe helm an");
  (ob=clone_object(SCHRECKEN("orkhose")))->move(pl);
  pl->command_me("ziehe hose an");
  (ob=clone_object(SCHRECKEN("steinring")))->move(pl);
  pl->command_me("ziehe ring an");
  (ob=clone_object(GEBIRGE("sbamulett")))->move(pl);
  pl->command_me("ziehe amulett an");
  (ob=clone_object(GEBIRGE("zmruestung")))->move(pl);
  pl->command_me("ziehe rüstung an");
  (ob=clone_object(FRIEDHOF("zomstiefel")))->move(pl);
  pl->command_me("ziehe stiefel an");
  (ob=clone_object(FRIEDHOF("sauger")))->move(pl);
  pl->command_me("zuecke schwert");
  (ob=clone_object(FRIEDHOF("vrobe")))->move(pl);
  pl->command_me("ziehe robe an");
  (ob=clone_object("/obj/fackel"))->move(pl);
  return 1;
}

static void
set_vars()
{
  object pl;

  pl = environment();
  if (!pl)
    return;

  if (!IS_LEARNER(pl))
  {
    call_out("remove",0);
    return;
  }

  /*
   * 19 oct 97: woody:
   * Query-Closures verwenden, da bei destruct der Brille ebenfalls weg
   */
  pl->Set(P_HANDS, lambda( ({}),
      ({#'return, '({" mit gewaltigen Händen",1000,DT_FIRE}) }) ),
      F_QUERY_METHOD );
  pl->Set(P_MAX_HANDS, lambda( ({}),
      ({#'return, 4 }) ), F_QUERY_METHOD );
  pl->Set(P_MAX_HP, lambda( ({}),
      ({#'return, 1000 }) ), F_QUERY_METHOD );
  pl->Set(P_MAX_SP, lambda( ({}),
      ({#'return, 1000 }) ), F_QUERY_METHOD );
  pl->Set(P_BODY, lambda( ({}),
      ({#'return, 1000 }) ), F_QUERY_METHOD );
  pl->heal_self(1000);

  /*
  pl->SetProp(P_AC, 5000);
  pl->SetProp(P_WC, 5000);
  pl->SetProp(P_MAX_HP, 10000);
  pl->SetProp(P_HP, 10000);
  pl->SetProp(P_MAX_SP, 10000);
  pl->SetProp(P_SP, 10000);
  pl->SetProp(P_MAX_HANDS, 6);
  pl->SetProp(P_HANDS, ({" mit blossen Haenden", 10000}));
  pl->SetProp(P_BODY, 10000);
  */
  pl->AddInsertHook(this_object());
}

int
_query_autoloadobj()
{
  return 1;
}

string
short()
{
  return _query_short();
}

string
_query_short()
{
  string s;

  s = "Eine Sonnenbrille";

  if(living(environment()))
    s += " (getragen)";
  return s;
}

string
long()
{
  return _query_long();
}

string
_query_long()
{
  string s;

  s = "Eine unheimlich coole Sonnenbrille mit sehr dunklen Gläsern.\n"
     +"Sie heilt auch lästigen Schnupfen ...\n"
     +"(Marke Blues-Brothers)\n";
  return s;
}

void
remove()
{
  if(this_interactive()!=owner) return;
  ::remove();
}

