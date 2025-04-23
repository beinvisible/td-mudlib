// Tamedhon MUDlib
//
// LIVING/LIFE.C -- life variables
//
// $Date: 2009/04/23 19:00:00 $
/* $Revision: 4.3 $
 * $Log: life.c,v $
 *
 * Revision 4.3  2009/04/23  19:00:00  Grynnis
 * in do_damage() Berechnung der AddXP als Float statt Int
 * wegen Ueberlauf bei grossen Werten (Gervase-negative-XP-Bug).
 * Loggen aller Spieler die XP bekommen, statt nur Killer.
 *
 * Revision 4.2  2008/01/17 23:00:00  TamTam
 * AddHp, AddSp analog zu AddPoison
 * Add logging of negative XP
 *
 * In NPC_XP werden auch die Zeitstempel mitgeloggt
 * Gralkor
 *
 * Revision 4.1  2003/09/23 23:55:00  Serii
 * transfer_all_to() behandelt nun verfluchte, getragene, ruestungen wie
 * neverdrop-objecte
 *
 * Revision 4.00  2003/07/05 16:50:00  Querolin
 * P_HP_STORE und P_SP_STORE und die regeneration in heart_beat erweitert
 * heal_self() auf P_SP_STORE und P_HP_STORE geaendert
 * heal_self_direct() eingefuegt. Damit auch die alte Funktionalitaet
 * erhalten bleibt... falls mal gebraucht ;)
 *
 * Revision 3.95  2003/04/27 14:30:00  Querolin
 * Gralkors Aenderungen wieder raus - BUG
 *
 * Revision 3.94  2003/04/25 10:43:00  Gralkor
 * Bug mit uebermax Alkohol, Drink, Food behoben
 *
 * Revision 3.93  2002/04/09 19:50:00  Querolin
 * P_ALIGN Brechnung neu gestaltet (alte ausgeklammert!)
 *
 * Revision 3.92  2000/11/30 18:30:15  Querolin
 * Aendern von Woody und Dusty auf Tamedhon in den Textpassagen
 *
 * Revision 3.9  1998/01/24 20:30:42  Feyaria
 * Runteridlen von deaf, blind und paralyze, P_PARAYLZE beruecksichtigen
 *
 * Revision 3.8  1997/11/06 12:59:45  Woody
 * Schnelleres HP/SP Hochidlen waehrend des Rastens
 *
 * Revision 3.7  1997/10/09 00:52:08  Woody
 * P_ENVIRONMENT vor Verwendung auf mappingp() pruefen
 *
 * Revision 3.6  1997/10/08 15:18:29  Woody
 * Geister leiden nicht mehr unter Gift oder Temperatur
 *
 * Revision 3.5  1997/10/08 13:36:49  Woody
 * Temperatursystem implementiert
 *
 * Revision 3.4  1997/10/07 09:23:28  Woody
 * drink_alcohol(), drink_soft(), drink_drink(), eat_food() flexibler
 *
 * Revision 3.3  1997/09/17 14:30:49  Woody
 * P_*_DELAY als Properties, P_TMP_DIE_HOOK, NPCs ueberwinden Vergiftung IMMER
 *
 * Revision 3.2  1997/06/25 11:53:55  Woody
 * XP-Verlust nur bei Sehern niedriger, P_LAST_DAMTYPES Gift,
 * _set_mv(), do_exhaust(), Leiche per M_NOCHECK moven
 *
 * Revision 3.1  1997/04/17 16:12:26  Feyaria
 * AddPoison
 *
 * Revision 3.0  1997/03/13 00:46:04  Woody
 * prepared for revision control
 *
 * Revision 2.47  1997/02/02  Woody
 * P_ALIGN_STRING
 *
 * Revision 2.46  1996/10/12  Woody
 * Bug in die() raus - QueryNPC() bekam irrtuemlich P_XP uebergeben :(
 *
 * Revision 2.45  1996/10/05  Woody
 * die(): NPC-Master Aufruf, Hochidlen abhaengig von INDOORS und Rasse,
 * Killer-Bonus-XP werden unter den Anwesenden Mitkillern verteilt
 *
 * Revision 2.44  1996/09/23  17:30:00  Woody@Silberland
 * get_age()
 *
 * Revision 2.43  1996/04/16  00:50:00  Woody@Silberland
 * P_CORPSE_PROPS eingefuehrt
 *
 * Revision 2.42  1995/07/24  11:07:40  Wargon
 * Gift etwas logischer gemacht.
 * Zufallselement in Ueberwindung der Vergiftung eingebaut.
 *
 * Revision 2.41  1995/07/20  15:41:57  Boing
 * Kleine SChoenheitskorrektur an der alignment-Berechnung
 *
 * Revision 2.40  1995/07/19  12:37:44  Jof
 * Ghosts dont die
 *
 * Revision 2.39  1995/07/19  12:34:29  Wargon
 * Gift ein wenig geaendert.
 *
 * Revision 2.38  1995/07/19  11:46:19  Boing
 * Alignment-Veraenderung wird nach einer neuen Formel berechnet, die
 * alte Formel war viel zu krass.
 *
 * Revision 2.37  1995/05/22  07:16:27  Jof
 * Minor speedups
 *
 * Revision 2.37  1995/05/21  20:20:14  mud
 * Minor speedups
 *
 * Revision 2.36  1995/04/27  19:39:31  Jof
 * removed some unnecessary ME->'s
 *
 * Revision 2.35  1995/04/05  20:09:53  Jof
 * Dont give xp if we dont have xp
 *
 * Revision 2.34  1995/03/21  19:38:14  Boing
 * Magier koennen jetzt essen und trinken so viel sie wollen.
 *
 * Revision 2.33  1995/01/31  15:46:06  Jof
 * current_object d/gebirge/boing/sm/mon/killerpopel#268339 destructed beseitigt
 * SetProp(P_HP) in do_damage vorgezogen zu diesem zwecke
 *
 * Revision 2.32  1994/12/28  20:21:18  Jof
 * Revision 2.31  1994/12/06  10:11:38  Boing
 * Neue Prop P_KILLER wird im Todesfall auf das toetende Objekt gesetzt.
 *
 * Revision 2.30  1994/11/30  01:03:41  Kirk
 * Set(P_DEADS) in die()
 *
 * Revision 2.29  1994/11/30  00:34:17  Kirk
 * counting num of deads
 *
 * Revision 2.28  1994/10/04  09:45:53  Jof
 * Revision 2.29  1994/10/04  13:54:50  mud
 * Revision 2.28  1994/10/03  22:07:17  mud
 * Revision 2.27  1994/08/30  13:50:04  Jof
 * Revision 2.26  1994/08/24  17:56:49  Jof
 * fixed show_age
 *
 * Revision 2.25  1994/06/13  20:14:32  Rochus
 * /log/KILLS Eintrag geaendert.
 *
 * Revision 2.24  1994/06/13  07:36:24  Jof
 * Removed SET_METHODs
 *
 * Revision 2.23  1994/05/26  19:07:24  Rochus
 * Kein do_damage mehr, wenn der Gegner ein Geist ist.
 *
 * Revision 2.22  1994/05/15  18:11:04  Jof
 * Revision 2.21  1994/05/05  12:42:38  Highlander
 * Corpse nach wahl mit P_CORPSE
 *
 * Revision 2.20  1994/05/02  17:24:34  Hate
 * capitalized messages for poison
 *
 * Revision 2.19  1994/03/30  12:32:57  Hate
 * changed drink messages to tell_object(ME
 *
 * Revision 2.18  1994/03/25  12:48:29  Jof
 * Logging kills
 *
 * Revision 2.17  1994/03/21  16:48:13  Jof
 * Revision 2.16  1994/03/18  12:35:53  Jof
 * Revision 2.15  1994/02/25  09:58:15  Highlander
 * DAs miit dem alte Version holen muss ich mal ueben...
 *
 * Revision 2.14  1994/02/25  09:53:56  Highlander
 * der Koehler sagt: <Hick>!  - capitalize drumrumgemahcht auf Bitte Troy
 *
 * Revision 2.13  1994/02/24  13:04:42  Highlander
 * Endlich gefunden, warum das poisonlog so zugemluellt wurde. Beseitigt.
 *
 * Revision 2.12  1994/02/24  13:02:34  Jof
 * Private removed
 *
 * Revision 2.11  1994/02/23  12:52:00  Highlander
 * Bug inserted last time killed again
 *
 * Revision 2.10  1994/02/23  12:48:37  Highlander
 * Auf der Suche nach doofen loggingtexten...
 *
 * Revision 2.9  1994/02/17  17:57:18  Kirk
 * void varargs die() -> varargs void die()  ;)
 *
 * Revision 2.8  1994/02/17  16:39:57  Highlander
 * Poison geht erst mal
 *
 * Revision 2.3  1994/02/16  13:36:34  Highlander
 *
 * Revision 2.1  1994/01/17  22:18:10  mud
 * changed indentation
 *
 * Revision 2.0  1994/01/13  16:52:04  mud
 * Revision 1.4  1994/01/10  19:46:21  mud
 * Bug in HP fixed
 */

// living object life variables
//
//  P_ALIGN         -- alignment value
//  P_ALIGN_STRING  -- alignment string
//  P_NPC           -- if living is an NPC
//  P_HP            -- HitPoints
//  P_SP            -- SpellPoints
//  P_ALCOHOL       -- value of intoxication
//  P_DRINK         -- value of soakness
//  P_FOOD          -- value of stuffness
//  P_AGE           -- Age
//  P_XP            -- experience
//  P_POISON        -- level of poison
//  P_CORPSE        -- corpse-object
//  P_CORPSE_PROPS  -- corpse-object properties

#pragma strong_types

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <thing.h>
#include <properties.h>
#include <config.h>
#include <health.h>
#include <wizlevels.h>
#include <language.h>
#include <moving.h>
#include <defines.h>
#include <combat.h>
#include <new_skills.h>
#include <living.h>
#include <npcmaster.h>

#define ALCOHOL_VALUE(n) n


int age;                  /* Number of heart beats of this character. */

nosave int delay_alcohol; /* time until next alcohol effect */
nosave int delay_drink;   /* time until next drink effect */
nosave int delay_food;    /* time until next food effect */
nosave int delay_heal;    /* time until next heal effect */
nosave int delay_sp;      /* time until next sp regeneration */
nosave int delay_poison;  /* time until next poison effect */
nosave int delay_temp;    /* time until next temperature effect */
nosave int delay_theal;   /* time until next heal effect if out of temp */
nosave int delay_tsp;     /* time until next sp regeneration if out of temp */

nosave int htemp;         /* temperature difference */
nosave int lasthtemp;     /* to prevent NPC_TEMPERATURE logfile flooding... */
nosave int drop_poison;
nosave mapping enemy_damage;

// local properties prototypes
static int _set_align(int a);
static int _set_hp(int hp);
static int _set_sp(int sp);
static int _set_mv(int mv);
static int _set_alcohol(int n);
static int _set_drink(int n);
static int _set_food(int n);
static int _set_poison(int n);
static int _query_age();
static int _set_xp(int xp);
mixed _set_die_hook(mixed hook);
static int _set_blind(mixed bl);
static int _set_deaf(mixed taub);
static int _set_paralyzed(mixed para);

void create()
{
  Set(P_GHOST, SAVE, F_MODE);
  Set(P_FROG, SAVE, F_MODE);
  Set(P_ALIGN, SAVE, F_MODE);
  Set(P_ALIGN_STRING, SAVE, F_MODE);
  Set(P_HP, SAVE, F_MODE);
  Set(P_SP, SAVE, F_MODE);
  Set(P_HP_STORE, SAVE, F_MODE_AS);
  Set(P_SP_STORE, SAVE, F_MODE_AS);
  Set(P_HP_STORE_PERCENT, SAVE, F_MODE_AS);
  Set(P_SP_STORE_PERCENT, SAVE, F_MODE_AS);
  Set(P_MV, SAVE, F_MODE);
  Set(P_XP, SAVE, F_MODE);
  Set(P_TOTAL_TEMPERATURE_PROTECTION, PROTECTED, F_MODE);
  SetProp(P_TOTAL_TEMPERATURE_PROTECTION,
      ([ T_COLD_PROT:0,T_HEAT_PROT:0 ]) );

  Set(P_ALCOHOL, SAVE, F_MODE);
  Set(P_DRINK, SAVE, F_MODE);
  Set(P_FOOD, SAVE, F_MODE);
  Set(P_POISON, SAVE, F_MODE);

  SetProp(P_FOOD_DELAY, FOOD_DELAY);
  SetProp(P_DRINK_DELAY, DRINK_DELAY);
  SetProp(P_ALCOHOL_DELAY, ALCOHOL_DELAY);
  SetProp(P_HP_DELAY, HEAL_DELAY);
  SetProp(P_SP_DELAY, HEAL_DELAY);
  SetProp(P_POISON_DELAY, POISON_DELAY);

  Set(P_AGE, -1, F_SET_METHOD);
  Set(P_AGE, PROTECTED, F_MODE);
  SetProp(P_CORPSE, "/std/corpse");

  enemy_damage=([]);
}

/*
 * This function is called from other players when they want to make
 * damage to us. But it will only be called indirectly.
 * We return how much damage we received, which will
 * change the attackers score. This routine is probably called from
 * heart_beat() from another player.
 * Compare this function to reduce_hit_point(dam).
 */
int do_damage(int dam, mixed enemy)
{
  int hit_point;

  if (QueryProp(P_GHOST) || QueryProp(P_NOATTACK) || dam<=0 ||
      (objectp(enemy) &&
       (enemy->QueryProp(P_GHOST) || enemy->QueryProp(P_NOATTACK))))
    return 0;
  hit_point = QueryProp(P_HP)-dam;
  if (QueryProp(P_XP) && enemy)
  {
    enemy->AddExp(dam*({int})QueryProp(P_TOTAL_WC)/10);
    enemy_damage[enemy]+=dam;
  }
  SetProp(P_HP, hit_point);
  if (hit_point<0) {
    if (enemy)
    {
      int exp_to_give, total_damage, tmp, i, killexp;
      mixed *present_enemies, *inv;

      enemy->StopHuntFor(ME,1);
      present_enemies=({});
      exp_to_give=QueryProp(P_XP)/100;
      inv=all_inventory(environment());
      for (i=sizeof(inv)-1; i>=0; i--)
        if (tmp=enemy_damage[inv[i]])
        {
          total_damage+=tmp;
          present_enemies+=({({inv[i], tmp})});
        }

      if (!total_damage)
        enemy->AddExp(exp_to_give);
      else
        // der uebliche Fall, auch bei nur einem Enemy
        for (i=sizeof(present_enemies)-1; i>=0; i--)
        {
          killexp = to_int(to_float(exp_to_give) *
                (to_float(present_enemies[i][1]) / to_float(total_damage)));
          present_enemies[i][0]->AddExp(killexp);

          if (!query_once_interactive(ME))
            log_file ("NPC_XP",  ctime(time()) + ": " + object_name(ME) +
              ", XP: " +  exp_to_give +
              (exp_to_give==killexp?"":(", KillXP: " + killexp)) +
              ", HP*WC: " + QueryProp(P_TOTAL_WC)*QueryProp(P_MAX_HP)/10 +
              (getuid(present_enemies[i][0])==getuid(enemy) ?
                ", Killer: ":", Enemy: ") +
              capitalize(getuid(present_enemies[i][0])) + "\n");
        }

/*
      enemy->SetProp(P_ALIGN, enemy->QueryProp(P_ALIGN)*9/10 -
		     QueryProp(P_ALIGN)/4);
*/
      enemy->SetProp(P_ALIGN, (enemy->QueryProp(P_ALIGN) -
                     QueryProp(P_ALIGN)/10) + random(15) - random(15) );
    }
    SetProp(P_KILLER, enemy);
    StopHuntingMode(1);
    die();
  }
  return dam;
}

void transfer_all_to(mixed dest)
{
  object *ob;

  ob = all_inventory(ME);
  if(sizeof(all_inventory(ME)) && QueryProp(P_KILLER) &&
     QueryProp(P_KILLER)->QueryProp(P_LOOTING) )
  {
      tell_object(QueryProp(P_KILLER),"Du plünderst die Leiche.\n");
      tell_room(environment(ME), QueryProp(P_KILLER)->Name(WER,1)+
        " plündert die Leiche.\n", ({ME, QueryProp(P_KILLER) }) );
  }
  while(sizeof(ob))
  {
    if(
        (!ob[0]->QueryProp(P_NEVERDROP))&&
        (!
          (
            (ob[0]->QueryProp(P_CURSED)&&ob[0]->QueryProp(P_WORN))||
	    (ob[0]->QueryProp(P_CURSED)&&ob[0]->QueryProp(P_WIELDED))
	  )
        )
      )
    {
      ob[0]->move(dest,M_SILENT|M_NOCHECK); /* no unwield messages */
      if( QueryProp(P_KILLER) && QueryProp(P_KILLER)->QueryProp(P_LOOTING) )
        QueryProp(P_KILLER)->pick_obj( ob[0] );
    }
    ob = ob[1..];
  }
  SetProp(P_USED_HANDS,0);
}

static void create_kill_log_entry(string killer) {
  string tmp;
  int level,lost_exp;

  tmp=dtime(time());;
  if ((level=ME->QueryProp(P_LEVEL))<20 || !IS_SEER(ME))
    lost_exp = ME->QueryProp(P_XP)/3;
  else
    lost_exp = ME->QueryProp(P_XP)/(level-17);
  log_file("KILLS",
               tmp[5..11]+" "+tmp[19..23]+" "+
               capitalize(getuid(ME))+
                   "("+level+","+(lost_exp/1000)+") "+
               killer+"\n");
}

varargs void die(int poisondeath) {
  object corpse;
  string die_msg;
  string tmp, tmp2;
  mapping props;
  mixed *ind, res;
  int i;
  object  killer;

  if (QueryProp(P_GHOST)) return; // Ghosts dont die ...
  if (query_once_interactive(this_object()) && QueryProp(P_POISON))
    SetProp(P_POISON, 0); /* Don't die twice 'cause of the same poison */

  if (res=QueryProp(P_TMP_DIE_HOOK)){
    if (pointerp(res) && sizeof(res)>=3
        && intp(res[0]) && time()<res[0]
        && objectp(res[1]) && stringp(res[2])){
      if (res=call_other(res[1],res[2],poisondeath))
        return ;
    } else
      SetProp(P_TMP_DIE_HOOK,0);
  }

  map_objects( QueryEnemies()[0], "StopHuntFor", ME, 1 );
  StopHuntingMode(1);
  if (IS_LEARNING(ME) && query_once_interactive(ME))
  {
    tell_object(ME,
                "Sei froh, dass Du unsterblich bist, sonst wäre es eben Dein"
                " Ende gewesen.\n");
    return;
  }

  if (query_once_interactive(ME) && extern_call() &&
      previous_object()!=this_object())
  {
    tmp=dtime(time());
    corpse=previous_object()||this_interactive()||this_player();
    if (corpse)
    {
      tmp2=explode(object_name(corpse),"#")[0]+" (direkt !)";
      if (sizeof(tmp2)>33) tmp2="..."+tmp2[<31..];
      create_kill_log_entry(tmp2+" ("+getuid(corpse)+")");
      log_file("Kills",sprintf("%s %s\n",getuid(ME),tmp2));
    }
  }
  if (!query_once_interactive(ME)) {
    corpse = QueryProp(P_KILLER) || previous_object() ||
             this_interactive() || this_player();
    if (corpse && query_once_interactive(corpse)) {
      res = QueryProp(P_XP);
      if (res < NPC_EXP_MARK1) res = 0;
      else if (res < NPC_EXP_MARK2) res = 1;
      else if (res < NPC_EXP_MARK3) res = 2;
      else res = 3;
      if (pointerp(res = NPCMASTER->QueryNPC(res)))
        corpse->AddScore(res[0]);
    }
  }
  if (!die_msg=QueryProp(P_DIE_MSG))
    die_msg = " fällt tot zu Boden.\n";
  if (query_once_interactive(ME))
    Set(P_DEADS,QueryProp(P_DEADS)+1);
  if (poisondeath) {
    switch (poisondeath)
    {
      case 2:
        die_msg = " wird von Kälte hinweggerafft und kippt um.\n";
        Set(P_LAST_DAMTYPES, ({ DT_COLD }));
        break;
      case 3:
        die_msg = " wird von Hitze hinweggerafft und kippt um.\n";
        Set(P_LAST_DAMTYPES, ({ DT_FIRE }));
        break;
      default:
        die_msg = " wird von Gift hinweggerafft und kippt um.\n";
        Set(P_LAST_DAMTYPES, ({ DT_POISON }));
    }
  }
  say (ME->Name(WER,1) + die_msg);

  if (QueryProp(P_NOCORPSE))
    transfer_all_to(environment());
  else
  {
    corpse = clone_object(QueryProp(P_CORPSE));
    if (!(QueryProp(P_CORPSE_FLAGS) & COR_NOSETWEIGHT))
      corpse->SetProp(P_WEIGHT,QueryProp(P_WEIGHT));
    corpse->Identify(ME);
    corpse->SetProp(P_HEAL, QueryProp(P_HEAL));
    if (!(QueryProp(P_CORPSE_FLAGS) & COR_NOINVTRANSFER))
      transfer_all_to(corpse);
    else
      transfer_all_to(environment(ME));
    if (props=QueryProp(P_CORPSE_PROPS))
    {
      ind = m_indices(props);
      for (i=sizeof(ind); i>0; i)
        corpse->SetProp(ind[--i], props[ind[i]]);
    }
    if (QueryProp(P_CORPSE_FLAGS) & COR_MSGIN)
      corpse->move(environment(ME),M_GO|M_NOCHECK);
    else
      corpse->move(environment(ME),M_NOCHECK);
    if (QueryProp(P_CORPSE_FLAGS) & COR_KILLENEMY)
      corpse->Kill(QueryProp(P_KILLER));
  }
  SetProp(P_ALCOHOL, 0);
  if (!ME->second_life(corpse))
    remove();
}

void heal_self(int h) {
  if(h <= 0) return;
  SetProp(P_HP_STORE, QueryProp(P_HP_STORE)+h);
  SetProp(P_SP_STORE, QueryProp(P_SP_STORE)+h);
}

void heal_self_direct(int h) {
  if(h <= 0) return;
  SetProp(P_HP, QueryProp(P_HP)+h);
  SetProp(P_SP, QueryProp(P_SP)+h);
}

int AddSp(int h) {
  restore_spell_points(h);
  return QueryProp(P_SP);
}

void restore_spell_points(int h) {
  SetProp(P_SP, QueryProp(P_SP)+h);
}

int AddHp(int h) {
  // Falls value < 0 -> minimal auf 1 HP senken
  if(SetProp(P_HP, (QueryProp(P_HP) + h)) <= 0) SetProp(P_HP, 1);
  return QueryProp(P_HP);
}

/* Reduce hitpoints. Log who is doing it. */
int reduce_hit_point(int dam) {
  object o;
#ifdef LOG_REDUCE_HP
  if(this_player()!=ME) {
    log_file("REDUCE_HP", ME->name()+" by ");
    if(!this_player()) log_file("REDUCE_HP","?\n");
    else {
      log_file("REDUCE_HP",this_player()->name());
      o=previous_object();
      if (o)
        log_file("REDUCE_HP", " " + object_name(o) + ", " +
                 o->name(WER,0) + " (" + creator(o) + ")\n");
      else
        log_file("REDUCE_HP", " ??\n");
    }
  }
#endif
  return AddHp(-dam);
}

int AddPoison(int strength)
{
  return SetProp(P_POISON, QueryProp(P_POISON) + strength);
}

varargs int drink_alcohol(int strength, int silent)
{
  int alc;

  alc = QueryProp(P_ALCOHOL)+ALCOHOL_VALUE(strength);
  if ((alc > QueryProp(P_MAX_ALCOHOL)) &&
      !IS_LEARNER(this_object()))
  {
    if (!silent)
      tell_object(ME, "So ein Pech, Du hast alles verschüttet.\n");
    return 0;
  }
  if(alc < 0) alc = 0;
  if(!alc && !silent) tell_object(ME, "Du bist stocknüchtern.\n");
  SetProp(P_ALCOHOL, alc);
  return 1;
}

varargs int drink_soft(int strength, int silent)
{
  int soaked;

  soaked = QueryProp(P_DRINK)+DRINK_VALUE(strength);
  if((soaked > QueryProp(P_MAX_DRINK)) &&
     !IS_LEARNER(this_object()))
  {
    if (!silent)
      tell_object(ME, "Nee, so viel kannst Du momentan echt nicht trinken.\n");
    return 0;
  }
  if(soaked < 0) soaked = 0;
  if(!soaked && !silent) tell_object(ME, "Dir klebt die Zunge am Gaumen.\n");
  SetProp(P_DRINK, soaked);
  return 1;
}

varargs int drink_drink(int soak, int alc, int silent)
{
  int soaked, alcohol;
  alcohol = QueryProp(P_ALCOHOL)+ALCOHOL_VALUE(alc);
  soaked = QueryProp(P_DRINK)+DRINK_VALUE(soak);
  if (!IS_LEARNER(this_object()))
    if (alcohol > QueryProp(P_MAX_ALCOHOL)) {
      if (!silent)
        tell_object(ME,
            "So ein Pech, Du hast alles verschüttet.\n");
      return 0;
    }
    else if (soaked > QueryProp(P_MAX_DRINK)) {
      if (!silent)
        tell_object(ME,
            "Nee, so viel kannst Du momentan echt nicht trinken.\n");
      return 0;
    }
  if(alcohol < 0) alcohol = 0;
  if(soaked < 0) soaked = 0;
  if(!alcohol && !silent) tell_object(ME, "Du bist stocknüchtern.\n");
  SetProp(P_ALCOHOL, alcohol);
  SetProp(P_DRINK, soaked);
  return 1;
}

varargs int eat_food(int strength, int silent)
{
  int stuffed;

  stuffed = QueryProp(P_FOOD)+FOOD_VALUE(strength);
  if ((stuffed > QueryProp(P_MAX_FOOD)) &&
      !IS_LEARNER(this_object()))
  {
    if (!silent)
      tell_object(ME,
          "Das ist viel zu viel für Dich! Wie wärs mit etwas leichterem?\n");
    return 0;
  }
  if(stuffed < 0) stuffed = 0;
  if(!stuffed && !silent)
    tell_object(ME, "Was rumpelt denn da in Deinem Bauch?\n");
  SetProp(P_FOOD, stuffed);
  return 1;
}

varargs int do_exhaust(int exhaust, int silent)
{
  int mvs;

  mvs = QueryProp(P_MV)-exhaust;
  if ((mvs < 0) && !IS_LEARNER(this_object()))
  {
    if (!silent)
      tell_object(ME, "Dazu bist Du noch zu erschöpft!\n");
    return 0;
  }
  SetProp(P_MV, mvs);
  return 1;
}

static void heart_beat() {
  int hpoison, lhp, storei;
  mapping trange, tmod;

  if (!ME) return;

  hpoison=QueryProp(P_POISON);
  age++;
  if(QueryProp(P_ALCOHOL) && !random(40)) {
    int n;
    switch(random(4)) {
      case 0:
      say(ME->Name(WER,1) + " sagt: <Hick>!\n");
      write("<Hick>! Oh, Tschuldigung.\n");
      break;
      case 1:
      say(ME->Name(WER,1) + " stolpert über " +
          QueryPossPronoun(MALE,WEN,PLURAL) + " Füße.\n");
      write("Du stolperst.\n");
      break;
      case 2:
      write("Du fühlst Dich benommen.\n");
      say(ME->Name(WER,1) + " sieht betrunken aus.\n");
      break;
      case 3:
      say(ME->Name(WER,1) + " rülpst.\n");
      write("Du rülpst.\n");
      break;
    }
  }
  if(QueryProp(P_ALCOHOL) && --delay_alcohol < 0) {
    SetProp(P_ALCOHOL, QueryProp(P_ALCOHOL)-1);
    if (!hpoison && !EnemyPresent() ) {
      SetProp(P_HP, QueryProp(P_HP)+1);
      SetProp(P_SP, QueryProp(P_SP)+1);
    }
    delay_alcohol = QueryProp(P_ALCOHOL_DELAY);
  }

  if(--delay_drink < 0) {
    delay_drink = QueryProp(P_DRINK_DELAY);
    SetProp(P_DRINK, QueryProp(P_DRINK)-1);
  }

  if(--delay_food < 0) {
    delay_food = QueryProp(P_FOOD_DELAY);
    SetProp(P_FOOD, QueryProp(P_FOOD)-1);
  }

  /* temperature handling */
  if (--delay_temp < 0) {
    delay_temp = TEMP_DELAY;
    trange = QueryProp(P_TEMPERATURE_RANGE);
    if (mappingp(tmod=QueryProp(P_TOTAL_TEMPERATURE_PROTECTION))) {
      trange[T_COLD_PROT] -= tmod[T_COLD_PROT];
      trange[T_HEAT_PROT] += tmod[T_HEAT_PROT];
    }

    if(objectp(environment()))
    {
      htemp =  (mappingp(environment()->QueryProp(P_ENVIRONMENT)) ?
        environment()->QueryProp(P_ENVIRONMENT)[ENV_TEMPERATURE]:15)+
      environment()->QueryProp(P_TEMPERATURE_OFFSET)+
      QueryProp(P_TEMPERATURE_OFFSET);
    }

    if (intp(htemp) && intp(trange[0]) && (htemp < ({int})trange[0]))
      htemp -= trange[0];
    else if (intp(htemp) && intp(trange[1]) && (htemp > ({int})trange[1]))
      htemp -= trange[1];
    else htemp = 0;
  }

  /* normal regeneration */
  if(--delay_heal < 0) {
    delay_heal = QueryProp(P_HP_DELAY)-(QueryProp(P_RESTING) ? 2 : 0);
    if (!hpoison && !EnemyPresent() ) {  // Voraussetzung: nicht vergiftet
      if (!htemp)                               // Temperatur ok
        SetProp(P_HP, QueryProp(P_HP)+1);
      else if (intp(htemp) && (-5 < htemp) && (htemp < 5)) {   // Differenz < 5
        if(intp(delay_theal))
          if (--delay_theal < 0) {
          delay_theal = THEAL_DELAY;
          SetProp(P_HP, QueryProp(P_HP)+1);
        }
      }
    }
  }

  if(--delay_sp < 0) {
    delay_sp = QueryProp(P_SP_DELAY)-(QueryProp(P_RESTING) ? 2 : 0);
    if (!hpoison && !EnemyPresent() ) {   // Voraussetzung: nicht vergiftet
      if (!htemp)                               // Temperatur ok
        SetProp(P_SP, QueryProp(P_SP)+1);
      else if (intp(htemp) && (-5 < htemp) && (htemp < 5)) {   // Differenz < 5
        if(intp(delay_tsp))
          if (--delay_tsp < 0) {
          delay_tsp = THEAL_DELAY;
          SetProp(P_SP, QueryProp(P_SP)+1);
        }
      }
    }
  }

   /* HP_STORE regeneration */
   if (QueryProp(P_HP_STORE) > 0) {
      if (QueryProp(P_HP) < QueryProp(P_MAX_HP)) {
         switch(QueryProp(P_HP_STORE)) {
             case 50 .. 999:
               storei=6+random(6);
               break;
             case 30 .. 49:
               storei=5+random(5);
               break;
             case 20 .. 29:
               storei=4+random(4);
               break;
             case 10 .. 19:
               storei=3+random(3);
               break;
             case 0 .. 9:
               storei=2+random(2);
         }
         SetProp(P_HP, QueryProp(P_HP)+storei);
         SetProp(P_HP_STORE, QueryProp(P_HP_STORE)-storei);
      }
      else {
       // zuviel gefuttert...
         SetProp(P_HP_STORE, QueryProp(P_HP_STORE)-(2+random(3)));
      }
   }
//  tell_object(find_player("querolin"),
//   break_string("storei:"+storei+" HP_STORE:"+QueryProp(P_HP_STORE)+"\n",78));

   /* SP_STORE regeneration */
   if (QueryProp(P_SP_STORE) > 0) {
      if (QueryProp(P_SP) < QueryProp(P_MAX_SP)) {
         switch(QueryProp(P_SP_STORE)) {
             case 50 .. 999:
               storei=6+random(6);
               break;
             case 30 .. 49:
               storei=5+random(5);
               break;
             case 20 .. 29:
               storei=4+random(4);
               break;
             case 10 .. 19:
               storei=3+random(3);
               break;
             case 0 .. 9:
               storei=2+random(2);
         }
         SetProp(P_SP, QueryProp(P_SP)+storei);
         SetProp(P_SP_STORE, QueryProp(P_SP_STORE)-storei);
      }
      else {
       // zuviel gefuttert...
         SetProp(P_SP_STORE, QueryProp(P_SP_STORE)-(2+random(3)));
      }
   }

  if ((query_once_interactive(ME) && !interactive(ME)) ||
      QueryProp(P_GHOST))
  {
    hpoison = 0;
    htemp = 0;
  }

  if (hpoison)  {
    lhp=QueryProp(P_HP);
    hpoison--;
    if (--delay_poison < 0) {
      delay_poison = QueryProp(P_POISON_DELAY) + random(POISON_MERCY_DELAY);
      lhp -= hpoison;
      if (lhp<0) lhp=-1;
      SetProp(P_HP, lhp);
      if (lhp < 0)  {
        tell_object(this_object(), "Oh weh - das Gift war zuviel für Dich!\n"
                    +"Du stirbst.\n");
        if (query_once_interactive(this_object()))
        {
          create_kill_log_entry("Vergiftung");
          log_file("Kills", geteuid()+" GIFT\n");
        }
        die(1);
        return;
      }
      if (((hpoison < 3) || !query_once_interactive(ME)) &&
          (--drop_poison < 0)) {
        switch(hpoison) {
        case 1:
          drop_poison += (15+random(6));  // Das werden etwa 50...
        case 2:
          drop_poison += (25+random(10));
        case 0:
          Set(P_POISON, hpoison);
          if (!hpoison)
            tell_object(this_object(),
                "Du scheinst die Vergiftung überwunden zu haben.\n" );
          break;
        default:
          drop_poison += (20-2*hpoison + random(40-3*hpoison));
          Set(P_POISON, hpoison);
        }
      }
    }
    if (hpoison && !random(15))
      switch (QueryProp(P_HP)*100/QueryProp(P_MAX_HP))  {
        case 71..100 :
          write("Du fühlst Dich nicht gut.\n");
          say(ME->Name(WER)+" sieht etwas benommen aus.\n", ({ME}));
          break;
        case 46..70 :
          write("Dir ist schwindlig und Dein Magen revoltiert.\n");
          say(ME->Name(WER)+" taumelt ein wenig.\n", ({ME}));
          break;
        case 26..45 :
          write("Dir ist heiß. Du fühlst Dich schwach. Kopfweh hast Du "
                "auch.\n");
          say(ME->Name(WER)+" glüht direkt und scheint große "
              "Schwierigkeiten zu haben.\n", ({ME}));
          break;
        case 11..25 :
          write("Du fühlst Dich beschissen. Alles tut weh, und Du siehst "
                "nur noch unscharf.\n");
          say(ME->Name(WER)+" taumelt und stöhnt und kann gerade "
              "noch vermeiden, hinzufallen.\n", ({ME}));
          break;
        case 0..10 :
          write(break_string("Du siehst fast nichts mehr und kannst Dich nur "
                             "noch unter größten Schmerzen bewegen. Aber "
                             "bald tut nichts mehr weh...", 78));
          say(break_string(ME->Name(WER)+" glüht wie im Fieber, kann sich "
                           "kaum noch rühren und hat ein schmerzverzerrtes "
                           "Gesicht.\n", 78), ({ME}));
          break;
      }
  }

  if( QueryProp(P_BLIND) )
    if( !SetProp(P_BLIND, (QueryProp(P_BLIND)[BLIND_TIME])-1) )
      write((random(2)?"Querolin":"Serii")+"seidank! Du kannst wieder sehen!\n");
  if( QueryProp(P_DEAF) )
    if( !SetProp(P_DEAF, (QueryProp(P_DEAF)[DEAF_TIME])-1) )
      write((random(2)?"Querolin":"Serii")+"seidank! Du kannst wieder "
            "hören!\n");
  if( QueryProp(P_PARALYZED) )
    SetProp(P_PARALYZED, (QueryProp(P_PARALYZED)[PARALYZE_TIME])-1);

  if (htemp)
  {
    if (delay_temp==TEMP_DELAY)
    {
      if (!query_once_interactive(ME) && (lasthtemp != htemp)) {
        log_file("NPC_TEMPERATURE", sprintf("%s - %s in %O (Abweichung: %d)\n",
                 dtime(time())[5..],
                 object_name(ME), environment(), htemp));
        lasthtemp = htemp;
      }

      lhp = htemp<0 ? (-htemp+1)/4 : (htemp-1)/4;
      lhp = QueryProp(P_HP) - lhp;
      if (lhp<0) lhp=-1;
      SetProp(P_HP, lhp);
      if (lhp < 0)
      {
        tell_object(this_object(), "Oh weh - die "+
          (htemp<0 ? "Kälte" : "Hitze")+
          " war zuviel für Dich!\nDu stirbst.\n");
        if (query_once_interactive(this_object()))
        {
          create_kill_log_entry("Temperatur");
          log_file("Kills", geteuid()+" TEMP\n");
        }
        else
          log_file("NPC_TEMPERATURE", sprintf("%s - %s TOT (Abweichung: %d)\n",
                   dtime(time())[5..], object_name(ME), htemp));
        this_object()->die(htemp<0 ? 2 : 3);
        return;
      }
      if (random(30) < (htemp<0 ? -htemp : htemp))
      {
        if (htemp>19) {
          if (random(2)) {
            write("Die sengende Hitze macht Dir schwer zu schaffen.\n");
            say(ME->Name(WER)+
                " leidet arg unter der sengenden Hitze.\n",({ME}));
          } else {
            write("Was für eine Affenhitze! Dir rinnen schon "
                "Schweißperlen die Stirn herab...\n");
            say(ME->Name(WER)+
                " schwitzt und ächzt unter der Hitze.\n",({ME}));
          }
        }
        else if (htemp<-19)
        {
          if (random(2)) {
            write("Die klirrende Kälte macht Dir schwer zu schaffen.\n");
            say(ME->Name(WER)+
                " leidet arg unter der klirrenden Kälte.\n",({ME}));
          } else {
            write("Du bibberst am ganzen Leib, so kalt ist es.\n");
            say(ME->Name(WER)+
                " bibbert und friert am ganzen Leib.\n",({ME}));
          }
        }
        else
        switch (htemp)
        {
          case -19..-10:
            switch(random(3))
            {
              case 0:
                write("Du frierst am ganzen Leib ob der Kälte.\n");
                say(ME->Name(WER)+
                    " friert und zittert am ganzen Leib.\n", ({ME}));
                break;
              case 1:
                write("Du klapperst vor Kälte mit den Zähnen.\n");
                say(ME->Name(WER)+
                    " klappert vor Kälte mit den Zähnen.\n",({ME}));
                break;
              default:
                write("Du schlotterst vor lauter Kälte... brrrr!\n");
                say(ME->Name(WER)+
                    " schlottert ob der eisigen Kälte... brrrr!\n",({ME}));
            }
            break;
          case -9..-5:
            if (random(2)) {
              write("Du frierst etwas.\n");
              say(ME->Name(WER)+" scheint etwas zu frieren.\n", ({ME}));
            } else {
              write("Du spürst die Kälte ziemlich deutlich.\n");
            }
            break;
          case -4..-1:
            if (random(2)) {
              write("Dich fröstelt leicht.\n");
              say(ME->Name(WEN)+" scheint leicht zu frösteln.\n", ({ME}));
            } else {
              write("Dir ist ein wenig kalt... "
                  "Du spürst eine leichte Gänsehaut aufkommen.\n");
            }
            break;
          case 1..4:
            if (random(2)) {
              write("Dir ist ein wenig heiß, ufffz... "
                  "nicht sonderlich angenehm.\n");
            } else {
              write("Du schwitzt leicht... etwas warm hier!\n");
              say(ME->Name(WEM)+" scheint etwas heiß zu sein.\n", ({ME}));
            }
            break;
          case 5..9:
            if (random(2)) {
              write("Du wischt Dir den Schweiß von der Stirn. "
                  "Ganz schön warm hier!\n");
              say(ME->Name(WER)+
                  " wischt sich den Schweiß von der Stirn.\n", ({ME}));
            } else {
              write("Du bekommst die Hitze schon relativ "
                  "deutlich zu spüren...\n");
            }
            break;
          case 10..19:
            switch (random(3))
            {
              case 0:
                write("Du schwitzt ziemlich stark.\n");
                say(ME->Name(WER)+" schwitzt ziemlich stark.\n", ({ME}));
                break;
              case 1:
                write("Du stöhnst unter der Hitze.\n");
                say(ME->Name(WER)+" stöhnt unter der Hitze.\n", ({ME}));
                break;
              default:
                write("Die Hitze setzt Dir ziemlich zu... Du ächzt.\n");
                say(ME->Name(WER)+
                    " ächzt von den hohen Temperaturen geplagt.\n", ({ME}));
            }
        }

      }
    }
  }
}

string get_age()
{
  int i,j;
  string s;
  s="";
  i = QueryProp(P_AGE);
  if ((j=i/43200)) {
    s += j + " Tag"+(j==1?", ":"e, ");
    i %= 43200;
  }
  if ((j=i/1800)) {
    s += j + " Stunde"+(j==1?", ":"n, ");
    i %= 1800;
  }
  if ((j=i/30)) {
    s += j + " Minute"+(j==1?" und ":"n und ");
    i %= 30;
  }
  s += i*2 + " Sekunden.";
  return s;
}

void show_age()
{
  int i,j;

  write("Alter:\t");
  write(get_age()+"\n");
}

int AddExp(int e)
{
  int experience;

  experience = QueryProp(P_XP);
  if(QueryProp(P_KILLS)>1 && e>0) return experience;
  if((experience+=e) < 0) experience = 0;
  return SetProp(P_XP, experience);
}

int add_exp(int e)
{
  return AddExp(e);
}

static int _set_align(int a) {
  if (a<-1000) a = -1000;
  if (a>1000) a = 1000;
  return Set(P_ALIGN, a);
}

static int _set_hp_store(int hp_store) {
  SetProp(P_HP_STORE_PERCENT, 100 - (QueryProp(P_POISON)*3) +
                   (((QueryProp(P_MAX_FOOD) - QueryProp(P_FOOD)) +1 ) / 5));
  if(hp_store < 0) return Set(P_HP_STORE, 0);
  if(QueryProp(P_HP_STORE_PERCENT) > 175) Set(P_HP_STORE_PERCENT, 175);
  if(QueryProp(P_HP_STORE_PERCENT) <  25) Set(P_HP_STORE_PERCENT,  25);

  if(hp_store > (QueryProp(P_HP_STORE_PERCENT)*QueryProp(P_MAX_HP)/100))
              return Set(P_HP_STORE,
                      QueryProp(P_HP_STORE_PERCENT)*QueryProp(P_MAX_HP)/100);
  return Set(P_HP_STORE, hp_store);
}

static int _set_sp_store(int sp_store) {
  SetProp(P_SP_STORE_PERCENT, 100 - (QueryProp(P_POISON)*3) +
                (((QueryProp(P_MAX_ALCOHOL) - QueryProp(P_ALCOHOL)) +1 ) / 5));
  if(sp_store < 0) return Set(P_SP_STORE, 0);
  if(QueryProp(P_SP_STORE_PERCENT) > 175) Set(P_SP_STORE_PERCENT, 175);
  if(QueryProp(P_SP_STORE_PERCENT) <  25) Set(P_SP_STORE_PERCENT,  25);

  if(sp_store > (QueryProp(P_SP_STORE_PERCENT)*QueryProp(P_MAX_SP)/100))
              return Set(P_SP_STORE,
                      QueryProp(P_SP_STORE_PERCENT)*QueryProp(P_MAX_SP)/100);
  return Set(P_SP_STORE, sp_store);
}

static int _set_hp(int hp) {
  if(hp < 0) return Set(P_HP, 0);
  if(hp > QueryProp(P_MAX_HP)) return Set(P_HP, QueryProp(P_MAX_HP));
  return Set(P_HP, hp);
}

static int _set_sp(int sp) {
  if(sp < 0) return Set(P_SP, 0);
  if(sp > QueryProp(P_MAX_SP)) return Set(P_SP, QueryProp(P_MAX_SP));
  return Set(P_SP, sp);
}

static int _set_mv(int mv) {
  if(mv < 0) return Set(P_MV, 0);
  if(mv > QueryProp(P_MAX_MV)) return Set(P_MV, QueryProp(P_MAX_MV));
  return Set(P_MV, mv);
}

static int _set_alcohol(int n) {
  return Set(P_ALCOHOL, (n < 0 ? 0 : n));
}

static int _set_drink(int n) {
  return Set(P_DRINK, (n < 0 ? 0 : n));
}

static int _set_food(int n) {
  return Set(P_FOOD, (n < 0 ? 0 : n));
}

static int _set_poison(int n)  {
  n = (n<0 ? 0 : (n>MAX_POISON ? MAX_POISON : n));

  if (Query(P_POISON) == 0 && n==0)
    return 0;

  log_file("POISON", sprintf("%s - %s: %d von %O (%s)\n",
           dtime(time())[5..],
           (query_once_interactive(this_object()) ?
             capitalize(geteuid(this_object())) :
             capitalize(ME->name(WER))),
           n,
           (previous_object(2) ? previous_object(2) : previous_object(1)),
           (this_player() ? capitalize(geteuid(this_player())) : "???")));

  switch(n) {
  case 1:
    drop_poison = 40+random(16);
    break;
  case 2:
    drop_poison = 25+random(8);
    break;
  case 3:
    drop_poison = 18+random(4);
    break;
  }
  return Set(P_POISON, n);
}

static int _query_age() { return Set(P_AGE, age); }

static int _set_xp(int xp) { return Set(P_XP, xp < 0 ? 0 : xp); }

mixed _set_die_hook(mixed hook){
  if(hook && query_once_interactive(this_object()))
    log_file("DIE_HOOK",
       sprintf("%s : DIE_HOOK gesetzt von %O in %O (%s)\n",
               dtime(time())[5..],
               (previous_object(2) ? previous_object(2):previous_object(1)),
               ME,getuid(ME)));
  return Set(P_TMP_DIE_HOOK,hook);
}

static mapping _query_temperature_range()
{
  return copy(Query(P_TEMPERATURE_RANGE));
}

static int _set_blind(mixed bl)
{
  int ret;
  mixed standard;

  if( !mappingp(standard=QueryProp(P_BLIND)))
    standard=([BLIND_TIME:3600, BLIND_MSG:"Du bist blind!\n"]);
  if( mappingp(bl) )
    Set(P_BLIND, bl);
  if( stringp(bl) )
    Set(P_BLIND, standard + ([BLIND_MSG: bl ]) );
  if( intp(bl) )
    Set(P_BLIND, standard + ([BLIND_TIME: bl]) );
  if( !ret=QueryProp(P_BLIND)[BLIND_TIME] )
    Set(P_BLIND, 0);
  return ret;
}

static int _set_deaf(mixed taub)
{
  int ret;
  mixed standard;

  if( !mappingp(standard=QueryProp(P_DEAF)))
    standard=([DEAF_TIME:3600, DEAF_MSG:"%s ist momentan leider taub.\n"]);
  if(mappingp(taub))
    Set(P_DEAF, taub);
  else if(stringp(taub))
    Set(P_DEAF, standard + ([DEAF_MSG:taub]) );
  else if(intp(taub))
    Set(P_DEAF, standard + ([DEAF_TIME:taub]) );
  if( !ret=QueryProp(P_DEAF)[DEAF_TIME])
    Set(P_DEAF, 0);
  return ret;
}

static int _set_paralyzed(mixed para)
{
  int ret;
  string msg, end;
  mapping standard, old_para;

  if(IS_LEARNING(this_object()))
    return 0;
  standard= ([
    PARALYZE_TIME:1,
    PARALYZE_MSG:"Du bist paralysiert und kannst Dich nicht bewegen.\n",
    PARALYZE_MSG_OTHER:Name()+" ist paralysiert und kann sich nicht bewegen.\n",
    PARALYZE_END:(random(2)?"Serii":"Querolin")+"seidank! Du kannst Dich "
                 "wieder bewegen.\n",
    PARALYZE_END_OTHER:"%s kann sich wieder bewegen.\n"]);
  if( mappingp(para) )
    Set(P_PARALYZED, standard+para);
  else
  {
    if( mappingp(old_para=QueryProp(P_PARALYZED)))
      standard=old_para;
    if( intp(para) )
      Set(P_PARALYZED, standard+([PARALYZE_TIME:para]));
    else if( stringp(para) )
      Set(P_PARALYZED, standard+([PARALYZE_MSG:para]));
  }
  if( !ret=QueryProp(P_PARALYZED)[PARALYZE_TIME])
  {
    tell_object(ME, QueryProp(P_PARALYZED)[PARALYZE_END]);
    tell_room(environment(ME), capitalize(sprintf(
      QueryProp(P_PARALYZED)[PARALYZE_END_OTHER],name())),({ME}));
    Set(P_PARALYZED, 0);
  }
  SetProp(P_DISABLE_ATTACK, ret);
  if (ret>=10 && previous_object(1) && previous_object()!=this_player() )
    log_file("PARALYZE",sprintf("%s %d %O %O\n",dtime(time())[4..15],
                                ret,previous_object(1),this_player()));
  return ret;
}

