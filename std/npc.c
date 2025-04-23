// Tamedhon MUDlib
//
// std/npc.c -- a generic npc (non player character)
//
// $Date: 2008/07/30 08:29:00 $ 
/* $Revision: 1.4 $       
 * $Log: npc.c,v $
 * Revision 1.4  2008/07/30 08:29:00  Querolin
 * Lastseen in init integriert
 *
 * Revision 1.3  2000/06/08 12:22:20  Samtpfote
 * ein paar kleinigkeiten angepasst
 *
 * Revision 1.2  2000/06/04 19:02:11  Samtpfote
 * npc/items.c eingebaut
 *
 * Revision 1.1  2000/06/04 19:01:11  Samtpfote
 * Initial revision
 *
 * Revision 3.2  1997/11/06 13:05:35  Woody
 * Temperaturbereich auf 0..40 ausgedehnt
 *
 * Revision 3.1  1997/06/25 11:25:52  Woody
 * inherit npc/attributes (statt living/attr...), description::init,
 * create_default_npc(): optionaler 2. Param, BODY auf level*4,
 * Random drin, usw.
 *
 * Revision 3.0  1997/03/13 03:07:22  Woody
 * prepared for revision control
 *
 * Revision 2.23  1996/06/22  16:45:00  Woody
 * Temperature Range fuer NPCs auf 0..30 gesetzt
 *
 * Revision 2.22  1996/03/12  16:45:00  Mupfel
 * Defines geaendert (/sys/npc.h included)
 *
 * Revision 2.21  1996/01/23  20:00:00  Hadra
 * P_NO_INVIS eingebaut
 *
 * Revision 2.20  1995/05/18  19:13:22  Rochus
 * Guard wird inherited.
 *
 * Revision 2.19  1995/04/26  15:44:17  Rochus
 * Skillsystem endgueltig eingebaut.
 *
 * Revision 2.18  1995/02/06  19:44:14  Jof
 * RCS-Logs aufgeraeumt (leere Logs raus usw)
 *
 * Revision 2.17  1995/01/16  09:03:13  Jof
 * Revision 2.16  1994/12/14  09:59:04  Rumata
 * verbessertes infosystem, jetzt kann bei AddInfo ein dritter parameter
 * uebergeben werden, der zur erzeugung mehrzeiliger infos dient.
 *
 * Revision 2.15  1994/10/06  11:20:49  Rochus
 * NPCs koennen jetzt die Spells benutzen.
 *
 * Revision 2.14  1994/09/08  17:46:13  Rochus
 * Skillsystem testweise eingebaut.
 *
 * Revision 2.13  1994/07/27  14:38:23  Jof
 * made suid
 *
 * Revision 2.12  1994/07/10  17:36:33  Jof
 * Weg mit "show"
 *
 * Revision 2.11  1994/07/10  17:23:38  Jof
 * Revision 2.10  1994/06/13  07:36:24  Jof
 * Removed SET_METHODs
 *
 * Revision 2.9  1994/05/31  08:56:33  Jof
 * catch_tell wieder rein
 *
 * Revision 2.8  1994/05/30  18:40:34  Jof
 * Revision 2.7  1994/05/15  18:40:56  mud
 * Revision 2.6  1994/05/15  16:48:12  mud
 * Revision 2.5  1994/03/21  16:48:13  Jof
 * Revision 2.4  1994/02/24  13:02:45  Jof
 * Revision 2.3  1994/01/25  12:09:31  jof
 * Revision 2.2  1994/01/17  15:44:39  mud
 * Revision 2.1  1994/01/17  12:29:16  mud
 * Revision 2.0  1994/01/13  16:51:57  mud
 */                            

#pragma strong_types

inherit "/std/thing/properties";
inherit "/std/living/description";
inherit "/std/living/life";
inherit "/std/living/moving";
inherit "/std/living/skills";
inherit "/std/npc/attributes";
inherit "/std/npc/chat";
inherit "/std/npc/combat";
inherit "/std/npc/comm";
inherit "/std/container/restrictions";
inherit "/std/thing/commands";
//inherit "/std/thing/util";
inherit "/std/thing/language";
inherit "/std/npc/info";
inherit "/std/npc/items";
inherit "/std/npc/put_and_get";
inherit "/std/npc/guard";

#include <config.h>
#include <properties.h>
#include <ansi.h>
#include <wizlevels.h>
#include <living.h>
#include <npc.h>
#include <language.h>
#include <attributes.h>
#include <moving.h>
#include <defines.h>

ENVCHECKPROC

static int _set_level(int arg);

#define DB(x)   "/room/debug"->debug_message(this_object(), x)

void create()
{ 
  seteuid(getuid());
  ENVCHECK
  restrictions::create();
  properties::create();
  description::create();
  commands::create();
  attributes::create();
  life::create();
  enable_commands();
  comm::create();
  combat::create();
  info::create();
  put_and_get::add_put_and_get_commands();
  add_action("UseSpell","",1);
  AddId("NPC");
  SetProp(P_NAME, "Niemand");
  SetProp(P_MSGIN, "kommt an");
  SetProp(P_MSGOUT, "geht");
  SetProp(P_MMSGIN, "erscheint in einer Rauchwolke");
  SetProp(P_MMSGOUT, "verschwindet mit Knall und Schwefelduft");
  SetProp(P_ALIGN, 0);
  SetProp(P_GENDER, NEUTER );
  SetProp(P_WEIGHT, 65000+random(25000));
  SetProp(P_AVERAGE_WEIGHT, 75000);
  SetProp(P_MAX_WEIGHT, 50000);
  SetProp(P_RACE, "Npc");
  SetProp(P_SIZE, 130+random(60)+random(2));
  SetProp(P_AVERAGE_SIZE, 150);
  SetProp(P_MAX_HP,100);
  SetProp(P_MAX_SP,100);
  SetProp(P_HP,50);
  SetProp(P_SP,50);
  SetProp(P_MAX_FOOD,100);
  SetProp(P_MAX_DRINK,100);
  SetProp(P_MAX_ALCOHOL,100);
  SetProp(P_DRINK, SetProp(P_MAX_DRINK,100));
  SetProp(P_FOOD, SetProp(P_MAX_FOOD,100));
  SetProp(P_ALCOHOL, 0);
  SetProp(P_HANDS, ({ "", 30 }) );
  SetProp(P_MAX_HANDS, 2);
  SetProp(P_USED_HANDS, 0);
  SetProp(P_ARMOURS,({}));
  SetProp(P_NPC, 1);
  SetProp(P_NO_INVIS, 1);
  SetProp(P_TEMPERATURE_RANGE, ([0:0, 1:40]));

  set_heart_beat(1);
  heartbeat=1;
}

void reset()
{
/* es ist besser dass der NPC destructet wird ....
 *
    if(!environment())
    {
        this_object()->move("/room/void", M_TPORT);
    }
 */
 if(!environment())
 {
	call_out("remove", 0);
 }

}

static int _set_level(int arg)
{
  Set(P_LEVEL, arg);
  SetAttr(A_CON, arg);
  SetAttr(A_DEX, arg);
  SetAttr(A_INT, arg);
  SetAttr(A_STR, arg);
  
  return 1;
}

varargs void create_default_npc(int level, int maxhp)
{
  int hands;
  if(level < 1) return;
  SetProp(P_LEVEL, level);
  if (maxhp<=0) maxhp=(10+level*level)*(95+random(11))/100;
  SetProp(P_MAX_HP, maxhp);
  SetProp(P_MAX_SP, maxhp);
  if (level<3)
    hands=30;
  else if (level<30)
    hands=10*level;
  else
    hands=300+(level-30)*5;
  SetProp(P_HANDS, ({" mit bloßen Händen", hands }) );
  SetProp(P_BODY, level*4);
  SetProp(P_XP, maxhp*hands*5);
}

void heart_beat()
{
	if (!environment())
		return;
  life::heart_beat();
  if (!this_object()) return; // Evtl. an Gift gestorben...
  combat::heart_beat();
  chat::heart_beat();
}

void give_notify(object ob)
{
  put_and_get::give_notify(ob);
}

// Force the monster to do a command.
int command_me(string cmd) { return command(cmd); }

void init()
{
  combat::init();
  info::init();
  commands::init();
  description::init();
  "/p/service/querolin/lastseen"->lsm_seen(this_object(),this_player());
}

void catch_tell(string str)
{}

varargs int remove()
{
  // DB(sprintf("PO=%O", previous_object()));
  // DB(sprintf("PO2=%O", previous_object(2)));
  // DB(sprintf("PO3=%O", previous_object(3)));
  return ::remove();
}
