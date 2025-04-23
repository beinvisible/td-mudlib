// Tamedhon MUDlib
//
// LIVING/COMBAT.C -- living combat
//

// Gralkor	2024-07-13 	Entfernung des RCS
// Gralkor	2024-07-12 	Ueberarbeitung der Schadensmeldungen
//		                detaillierter im Bereich des P_WC

/* 
 *
 * Revision 3.4  2008/05/11 16:00:00  TamTam
 * P_LAST_DAMAGE
 *
 * Revision 3.3  2004/03/30 22:00:00  Serii
 * pk-bugfix
 *
 * Revision 3.2  2004/02/02 19:00:00  Serii
 * Legale PK-attacken werden nun nichtmehr in /log/ATTACKS gelogt
 *
 * Revision 3.17  2002/08/08 14:25:59  Serii
 * pkmaster von /obj -> /p/daemon
 *
 * Revision 3.16  2002/08/06 20:00:00  Serii
 * In Kill() alles auf /obj/pkmaster fuer PKs umgestellt
 *
 * Revision 3.15  2002/08/04 09:36:00  Querolin
 * Kampfmeldungen (alt/neu) koennen nun ausgewaehlt werden. -> _whathow()
 *
 * Revision 3.14  2002/02/27 17:36:20  Gralkor
 * habe mal die dummen alten Schadensmeldungen durch 'realistischere' ersetzt
 *
 * Revision 3.13  1998/01/24 20:30:12  Feyaria
 * Bei flucht auf opjectp pruefen, P_DISABLE_ATTACK anders mitloggen
 *
 * Revision 3.12  1997/12/01 14:48:15  Woody
 * Sicherheitschecks bei Ausgabe der Fluchtrichtung beim Fluechten
 *
 * Revision 3.11  1997/11/10 00:07:40  Woody
 * Klammern um Blind/Taubabfrage in CheckResistance, das buggte (seltsam)
 *
 * Revision 3.10  1997/11/05 16:17:01  Woody
 * Bei Blindheit Trefferquote senken, Blinde sind
 * zudem gegen DT_LIGHT und Taube gegen DT_SOUND resistent
 *
 * Revision 3.9  1997/10/29 13:07:32  Woody
 * Meldung wohin man gefluechtet ist, wenn eingestellte flu'ri scheitert
 *
 * Revision 3.8  1997/10/08 13:36:30  Woody
 * _query_total_ac() vereinfacht und dadurch hofftl. beschleunigt
 *
 * Revision 3.7  1997/10/06 12:21:21  Woody
 * P_PREPARED_SPELL auswerten und gg.falls per UseSpell() casten
 * (im heart_beat())
 *
 * Revision 3.6  1997/09/17 14:24:58  Woody
 * P_LAST_COMBAT_TIME setzen, Attack-Msg nur bei Attacke und/oder Aenderung,
 * Setzen von P_DISABLE_ATTACK mitloggen, wenn > 10 Sekunden
 *
 * Revision 3.5  1997/08/25 15:29:45  Feyaria
 * quality_decay
 *
 * Revision 3.4  1997/06/25 11:52:55  Woody
 * Netztote sind keine Feinde mehr, P_LAST_DAMTYPES,
 * Fluchtrichtung/Attackbusy auf Seher u.max. 7 beschraenkt,
 * neues Handshandling (P_HANDS_USED_BY, Use/FreeHands())
 *
 * Revision 3.3  1997/06/02 15:29:34  Feyaria
 * kampfskill deaktiviert... da buggte irgendwas
 *
 * Revision 3.2  1997/06/02 10:17:46  Feyaria
 * eigene schadensmeldungen in showdamage
 *
 * Revision 3.1  1997/05/30 19:12:54  Feyaria
 * fluchtrichtung und attack_busy umgestellt
 *
 * Revision 3.0  1997/03/13 00:51:43  Woody
 * prepared for revision control
 *
 * Revision 2.70  1997/02/27  Woody
 * erweiterte Abfrage von P_BRIEF
 *
 * Revision 2.69  1997/02/10  Woody
 * inherit inventory, CheckSensitiveAttack fuer angriffsempfindliche Objekte,
 * Angriffs/Schadensmeldungen bei 'kurz' und 'ultrakurz' eingeschraenkt
 *
 * Revision 2.68  1997/02/01  Woody
 * enemies auf mapping umgestellt, P_RESISTANCE_STRENGTHS, etc.
 *
 * Revision 2.67  1997/01/08  Woody
 * Statt Arrestieren nun Aufruf des PK-Masters: /obj/pkmaster::TriedPK(wer,wen)
 *
 * Revision 2.66  1997/01/06  Woody
 * automatisches Arrestieren bei PK-Versuch
 *
 * Revision 2.65  1996/11/19  Woody
 * neue Schadensmeldungen [von Feyaria]
 *
 * Revision 2.64  1996/11/18  Woody
 * P_NOATTACK ist nun nicht mehr durch Feuerball umgehbar
 *
 * Revision 2.63c 1996/10/10  Woody
 * Bug in StopHuntID() raus (for schleife ab sizeof()-1)
 *
 * Revision 2.63b 1996/10/06  Woody
 * 100% Fluchtrichtung erst ab Level 35
 *
 * Revision 2.63a 1996/04/04  Woody
 * "Jajaja"-Text: Punkt hintendran
 *
 * Revision 2.63  1996/03/24  03:50:00  Woody
 * Bugfix in do_my_heart_beat() falls Gegner per wimpy getoetet wird
 *
 * Revision 2.62  1996/02/29  01:10:00  Mupfel
 * Schadensmeldungen: 'zerschmetter*' jetzt von 31-40 LP Schaden,
 *                    darueber: 'pulverisier*'
 *
 * Revision 2.61  1996/02/28  20:25:00  Mupfel
 * Bugfix bei P_NOATTACK, keine Meldung wenn das Monster
 * unsichtbar ist
 *
 * Revision 2.60 20.1.95 oder so
 * P_NOATTACK eingebaut   Gundur
 *
 * Revision 2.59  1995/07/26  14:35:00  Rumata
 * environment(ME)->environment() after Flee()
 * Flee can remove the player object.
 *
 * Revision 2.58  1995/07/19  17:09:10  Jof
 * Yeah
 *
 * Revision 2.56  1995/07/16  13:58:53  Silvana
 * Und naochmal: Trainigsraum fuer Karateka.
 *
 * Revision 2.55  1995/07/13  10:29:51  Rochus
 * Mega-BUG-Fix.
 *
 * Revision 2.54  1995/07/11  15:35:56  Rochus
 * *** empty log message ***
 *
 * Revision 2.52  1995/07/07  14:22:03  Rochus
 * Auch Body hilft gegen Physical Attacks.
 *
 * Revision 2.51  1995/06/16  21:55:42  Silvana
 * P_TMP_DEFEND_HOOK with same args as Defend()
 *
 * Revision 2.50  1995/05/22  07:17:12  Jof
 * Flee even if attack is still disabled
 *
 * Revision 2.50  1995/05/21  20:20:14  mud
 * Flee even if attack is still disables
 *
 * Revision 2.49  1995/05/10  19:29:31  Rochus
 * SI_ENEMY wurde nicht gesetzt.
 *
 * Revision 2.48  1995/04/26  17:23:11  Jof
 * callothers to funcalls
 *
 * Revision 2.47  1995/03/22  19:10:01  Catweazle
 * *** empty log message ***
 *
 * Revision 2.46  1995/03/22  18:44:56  Catweazle
 * Arenalogging repariert
 *
 * Revision 2.45  1995/03/02  20:16:52  Jof
 * check for player exists in plattacked
 *
 * Revision 2.44  1995/03/01  20:15:56  Jof
 * Log arena kills
 *
 * Revision 2.43  1995/03/01  18:10:47  Rochus
 * bugfix
 *
 * Revision 2.42  1995/02/28  11:09:48  Jof
 * Try to remember ATTACKER when PKing ...
 *
 * Revision 2.42  1995/02/28  17:46:32  Jof
 * Attack-Logging moved HERE
 * Remember Player-Player-Attacks
 *
 * Revision 2.41  1995/02/23  16:40:08  Rochus
 * Bei Spells kann mit SP_SHOW_DAMAGE angegeben werden,
 * dass der Treffer gezeigt werden soll, und in SP_REDUCE_ARMOUR
 * koennen bestimmte Armourtypen abgewertet werden.
 *
 * Revision 2.40  1995/02/20  19:36:26  Rochus
 * SI_SPELL kann bei Kampfspells gesetzt werden
 *
 * Revision 2.39  1995/02/20  19:11:40  Rochus
 * Feind wird bei Kampfskills mit uebergeben
 * Waffentyp im Skill setzbar
 *
 * Revision 2.38  1995/02/20  09:59:59  Jof
 * Optimizations
 *
 * Revision 2.37  1995/01/25  22:26:23  Jof
 * Bugfixes
 *
 * Revision 2.36  1995/01/24  23:36:56  Jof
 * no_more_attacks
 *
 * Revision 2.35  1995/01/24  16:19:23  Kirk
 * attack_busy added
 *
 * Revision 2.34  1995/01/17  18:19:27  Rochus
 * Noch ne Fehlerkorrektur
 *
 * Revision 2.33  1995/01/17  17:33:10  Rochus
 * Fehler bei InformDefend behoben.
 *
 * Revision 2.32  1994/11/15  14:45:24  Jof
 * Revision 2.31  1994/10/07  17:04:29  Jof
 * Revision 2.30  1994/10/04  10:06:08  Jof
 * Revision 2.29  1994/10/04  09:45:53  Jof
 * Revision 2.29  1994/10/04  13:54:50  mud
 * Revision 2.28  1994/09/30  14:23:34  Rochus
 * Fehler in letzter Aenderung behoben.
 *
 * Revision 2.27  1994/09/30  14:01:46  Rochus
 * Beliebige Befehle zur Flucht zugelassen.
 *
 * Revision 2.26  1994/09/30  11:30:37  Rochus
 * Fehler in Flee() behoben.
 *
 * Revision 2.25  1994/09/29  20:46:37  Rochus
 * P_WIMPY_DIRECTION fuer bevorzugte fluchtrichtung
 *
 * Revision 2.24  1994/09/29  16:36:10  Rochus
 * Revision 2.23  1994/09/23  11:12:30  Rochus
 * Fehler in AddDefender behoben.
 *
 * Revision 2.22  1994/09/22  13:40:25  Rochus
 * Moeglichkeiten von TMP_ATTACK/DEFEND_HOOK erweitert.
 *
 * Revision 2.21  1994/09/15  14:16:36  Rochus
 * P_DEFENDER, P_TMP_ATTACK_HOOK und P_TMP_DEFEND_HOOK eingefuehrt.
 * TMP-Hooks fuer kurzzeitiges Ueberschreiben von Attack/Defend
 * Defender werden ueber Angriff informiert, einer kann Werte aendern.
 *
 * Revision 2.20  1994/09/14  16:29:42  Rochus
 * MagicAttack/Defense Fight-Skills
 *
 * Revision 2.19  1994/09/12  18:30:11  Rochus
 * Skill-Attribut SPEED beim heart_beat beruecksichtigt.
 *
 * Revision 2.18  1994/08/23  23:47:53  Kirk
 * no damage by netdeads
 *
 * Revision 2.17  1994/08/02  15:27:53  Jof
 * Revision 2.16  1994/06/20  09:48:05  Jof
 * Inserted update_hunt_times
 *
 * Revision 2.15  1994/06/18  11:05:44  Boing
 * Fixed InsertEnemy
 *
 * Revision 2.14  1994/06/13  07:36:24  Jof
 * Removed SET_METHODs
 *
 * Revision 2.13  1994/05/26  19:13:16  Rochus
 * Geister koennen kein Attack(enemy) mehr ausfuehren.
 *
 * Revision 2.12  1994/05/15  18:11:04  Jof
 * Clean-Ups
 *
 * Revision 2.11  1994/04/14  23:02:15  Kirk
 * added set_heart_beat(1) to InsertEnemy
 *
 * Revision 2.10  1994/03/21  16:48:13  Jof
 * Reindented files using emacs - once again
 *
 * Revision 2.9  1994/03/20  17:35:54  Hate
 * Optimized QueryProp(P_HANDS) during combat
 *
 * Revision 2.8  1994/03/18  12:35:53  Jof
 * Fixed indentation
 *
 * Revision 2.7  1994/02/24  13:02:34  Jof
 * Private removed
 *
 * Revision 2.6  1994/02/01  18:14:41  Rumata
 * P_DISABLE_ATTACK testet auf integer.
 *
 * Revision 2.2  1994/01/27  22:36:09  Kirk
 * initial whimpy value set to 20
 *
 * Revision 2.1  1994/01/27  15:33:48  jof
 * livings cannot attack themselves.
 *
 * Revision 1.3  1994/01/10  20:48:10  mud
 * Fixed Bug in wimpy-code
 */

inherit "/std/living/inventory";

#include "/std/sys_debug.h"

#pragma strong_types

#define NEED_PROTOTYPES

#include <thing/properties.h>

#include <config.h>
#include <properties.h>
#include <language.h>
#include <wizlevels.h>
#include <attributes.h>
#include <living.h>
#include <combat.h>
#include <moving.h>
#include <doors.h>
#include <defines.h>
#include <new_skills.h>
#include <sensitive.h>
#include <ansi.h>

#include "/p/daemon/pkmaster/pkmaster.h"
#define HUNTTIME 300 //300 HBs sind 10 Minuten
#define RNAME(x) capitalize(getuid(x))

static mapping enemies;
static string magic_attack;
static int attack_busy;
static int no_more_attacks;
static string *plAttacked;
static string last_attack_msg;

// local properties prototypes
static int _query_total_ac();
static mixed _set_hands(mixed h);
static mixed _query_hands();
static int _set_wimpy(int i);

static void create()
{
  enemies=m_allocate(0,1);
  Set(P_WIMPY, SAVE, F_MODE_AS);
  Set(P_TOTAL_AC, PROTECTED, F_MODE_AS);
  Set(P_HANDS, SAVE, F_MODE_AS);
  Set(P_RESISTANCE, ({}));
  Set(P_VULNERABILITY, ({}));
  Set(P_RESISTANCE_STRENGTHS,([DT_FIRE:0]));
  attack_busy=0;
  plAttacked=({});
}

mixed _set_resistance(mixed arg) {
  int i;
  mapping map;
  mixed old;

  if (!pointerp(arg)) arg=({arg});
  if (!mappingp(map=Query(P_RESISTANCE_STRENGTHS)))
    map=([]);
  if (pointerp(old=QueryProp(P_RESISTANCE)))
    for (i=sizeof(old)-1;i>=0;i--)
      map[old[i]]=(1.0+(({float})map[old[i]]))*2.0-1.0;
  for (i=sizeof(arg)-1;i>=0;i--)
    map[arg[i]]=(1.0+(({float})map[arg[i]]))*0.5-1.0;
  SetProp(P_RESISTANCE_STRENGTHS,map);
  return Set(P_RESISTANCE,arg);
}

mixed _set_vulnerability(mixed arg) {
  int i;
  mapping map;
  mixed old;

  if (!pointerp(arg)) arg=({arg});
  if (!mappingp(map=Query(P_RESISTANCE_STRENGTHS)))
    map=([]);
  if (pointerp(old=QueryProp(P_VULNERABILITY)))
    for (i=sizeof(old)-1;i>=0;i--)
      map[old[i]]=(1.0+(({float})map[old[i]]))*0.5-1.0;
  for (i=sizeof(arg)-1;i>=0;i--)
    map[arg[i]]=(1.0+(({float})map[arg[i]]))*2.0-1.0;
  SetProp(P_RESISTANCE_STRENGTHS,map);
  return Set(P_VULNERABILITY,arg);
}

int QueryPlAttacked(object pl)
{
  int i;

  if (!objectp(pl)) return 0;
  for (i=sizeof(plAttacked)-1;i>=0;i--)
    if (!(pl=find_player(plAttacked[i])||find_netdead(plAttacked[i])) ||
	(!IsEnemy(pl) && !(pl->IsEnemy(ME))))
      plAttacked=plAttacked[0..i-1]+plAttacked[i+1..];
  if (!pl) return 0;
  return (member(plAttacked,getuid(pl))>=0);
}

int check_attack(object me, object en)
{
  mixed noattack;

  if ((noattack=en->QueryProp(P_NOATTACK)) && !query_once_interactive(en))
  {
    if (!en->QueryProp(P_SHORT) || en->QueryProp(P_INVIS))
      return 0;             // keine Meldung bei unsichtbaren NPCs
    if (stringp(noattack))
      tell_object(me, capitalize(en->name(WER, 1))+" "+noattack+"\n");
    else
      tell_object(me, capitalize(en->name(WER, 1))+
                      " springt geschickt aus Deiner Reichweite.\n");
    return 0;
  }
  return 1;
}

/*
 * kill - start combat
 * Sets the enemy thus starting combat
 */
int Kill(object ob)
{
  int res;
//  int arena;

  if (!objectp(ob)) return 0;
  if (!check_attack(ME, ob)) return 0;
  if (ob->QueryProp(P_GHOST))
  {
    tell_object(ME,ob->Name(WER)+" ist doch schon tot!\n");
    return 1;
  }
  if (query_once_interactive(ob) && query_once_interactive(ME) &&
      !IsEnemy(ob))
  {
    QueryPlAttacked(ME);
    ob->QueryPlAttacked(ob); // aktualisieren ...
  }
  res=InsertEnemy(ob);
  tell_object(ME,(res?"Ok":"Jajaja, machst Du doch schon.")+"\n");
  if (!res) return 1;
/* 06.08. 2002 - Serii@Tamedhon - arenen gibts nicht, wird zukuenftig vom
 *                                /obj/pkmaster.c uebernommen.
 */
//  arena=0;
  if (query_once_interactive(ME) && query_once_interactive(ob) &&
      !(IS_LEARNER(ME) && IS_LEARNER(ob)) /*&&
      !(ob->QueryProp(P_TESTPLAYER))*/)
  {
    object *u;
    string message;
    int i;

    if (this_interactive()!=ME)
      message="ACHTUNG: TI="+getuid(this_interactive());
    else
      message="";
    if((message!="")||!(call_other(PKMASTER,"CheckForPkEnvironment",ME,ob)||
                        (call_other(PKMASTER,"CheckForHitList",ME,ob)==2)))
    log_file("ATTACKS",dtime(time())+", "+RNAME(ME)+" greift "+RNAME(ob)+
             " an"+message/*+(arena?" (ARENA)":"")*/+"\n");
/*    if (!arena)
    {
      message="**** "+RNAME(ME)+" greift "+RNAME(ob)+" an ****\n";
      u=users();
      for (i=sizeof(u)-1;i>=0;i--)
	if (IS_LORD(u[i]))
	  tell_object(u[i],message);*/
//    call_out("tried_pk", 0, ob);
  call_other(this_object(),"tried_pk",ob);
//    }
    plAttacked+=({getuid(ob)});
  }
  return 1;
}

void tried_pk(object enemy)
{
  while (remove_call_out("arrest") != -1);
// 08.08. 2002 - Serii@Tamedhon - ab jetzt ist der pkmaster in /p/daemon
  catch (call_other ("/p/daemon/pkmaster", "TriedPK", ME, enemy));
}

int InsertEnemy(object ob)
{
  if (ob == ME
      || !objectp(ob)
      || ob->QueryProp(P_GHOST)
      || ob->QueryProp(P_NOATTACK)
      || member(enemies,ob))
    return 0;
  set_heart_beat(1);
  last_attack_msg=0;
  if (!living(ob))
    return 1;
  enemies[ob,ENEMY_HUNTTIME]=HUNTTIME;
  Set(P_LAST_COMBAT_TIME,time());
  return 1;
}

object QueryPreferedEnemy() {
  int sz,r;
  mixed pref;
  object enemy;

  enemy=0;
  if (pointerp(pref=QueryProp(P_PREFERED_ENEMY))
      && (sz=sizeof(pref))>1
      && intp(r=pref[0])
      && random(100)<r) {
    enemy=pref[1+random(sz-1)];
    if (!objectp(enemy)) {
      pref-=({enemy});
      if (sizeof(pref)<2) pref=0;
      SetProp(P_PREFERED_ENEMY,pref);
      return 0;
    }
    if (!IsEnemy(enemy))
      return 0;
  }
  return enemy;
}

object *PresentEnemies() {
  int i;
  object *here, *netdead, *all, pl;

  all=m_indices(enemies);
  here=({}); netdead=({});
  for (i=sizeof(all)-1;i>=0;i--) {
    pl=all[i];
    if (!objectp(pl) || pl->QueryProp(P_GHOST) || pl->QueryProp(P_NOATTACK))
      efun::m_delete(enemies,pl);
    else
      if (environment()==environment(pl)) {
        if (interactive(pl) || !query_once_interactive(pl))
          here+=({pl});
        else
          netdead+=({pl});
      }
  }
  if (!sizeof(here)) // Netztote sind nur Feinde, falls keine anderen da sind
    return netdead;
  return here;
}

varargs object SelectEnemy(object *here) {
  object enemy;

  if (!pointerp(here))
    here=PresentEnemies();
  if (!sizeof(here))
    return 0;
  if (!objectp(enemy=QueryPreferedEnemy())
      || environment(enemy)!=environment())
    enemy=here[random(sizeof(here))];
  return enemy;
}

void do_my_heart_beat() {
  int i, c;
  object *here, *netdead, enemy, *all;

  if (QueryProp(P_RESTING)) return;
  no_more_attacks=0,here=({}),netdead=({});
  all=m_indices(enemies);
  for (i=sizeof(all)-1;i>=0;i--) {
    enemy=all[i];
    if (!objectp(enemy)
        || enemy->QueryProp(P_GHOST)
        || enemy->QueryProp(P_NOATTACK))
      efun::m_delete(enemies,enemy);
    else
      if (environment()==environment(enemy)) {
        if (interactive(enemy) || !query_once_interactive(enemy))
          here+=({enemy});
        else
          netdead+=({enemy});
        enemies[enemy,ENEMY_HUNTTIME]=HUNTTIME;
      } else
        if((enemies[enemy,ENEMY_HUNTTIME]--)<=0)
          StopHuntFor(enemy);
  }
  if (!sizeof(here))
    here=netdead;
  if (objectp(enemy=SelectEnemy(here))) { // Feind anwesend?
    if (QueryProp(P_WIMPY) > QueryProp(P_HP)) {
      Flee();
      if (enemy && (environment(enemy) != environment()))
	enemy=0;
    }
  } else
    enemy=0;

  c=QueryProp(P_DISABLE_ATTACK);
  if (intp(c) && c>0) {
    SetProp(P_DISABLE_ATTACK, --c);
    return;
  }
  if (enemy) Attack(enemy);
}

static int remaining_heart_beats;
void heart_beat()
{
  int i,hbs;
  mixed ps;

  attack_busy=0;
  hbs=remaining_heart_beats+QuerySkillAttribute(SA_SPEED);
  if (hbs<=0) hbs=100;
  remaining_heart_beats=hbs%100;
  hbs/=100;
  if (hbs>10) hbs=10;
  hbs-=no_more_attacks;
  for (i=0;i<hbs;i++) do_my_heart_beat();
  no_more_attacks=0;

  if (pointerp(ps=QueryProp(P_PREPARED_SPELL))
      && sizeof(ps)>=3 && intp(ps[0]) && stringp(ps[1])) {
    if (time()>=ps[0])  {    // Kann der Spruch jetzt ausgefuehrt werden?
      UseSpell(ps[2],ps[1]); // Dann los
      SetProp(P_PREPARED_SPELL,0);
    }
  } else if (ps) { // Unbrauchbarer Wert
    SetProp(P_PREPARED_SPELL,0);
  }
}

int update_hunt_times(int beats)
{
  int i;
  mixed *all;

  all=m_indices(enemies);
  for (i=sizeof(all)-1;i>=0;i--)
    if (!objectp(all[i])||all[i]->QueryProp(P_GHOST))
      efun::m_delete(enemies,all[i]);
    else
    {
      enemies[all[i],ENEMY_HUNTTIME]-=beats;
      if (enemies[all[i],ENEMY_HUNTTIME]<=0)
        efun::m_delete(enemies,all[i]);
    }
  return 0;
}

int IsEnemy(object wer)
{
  return (member(enemies,wer));
}

void StopHuntText(object arg)
{
  string cnorm,color;

  if (arg->QueryProp(P_TTY)=="ansi")
  { cnorm=ANSI_NORMAL; color=ANSI_GREEN; }
  else { cnorm=""; color=""; }

  if (!objectp(arg)) return;
  tell_object(arg, color+ME->Name(WER)+" jagt Dich nicht mehr.\n"+cnorm);

  if (QueryProp(P_TTY)=="ansi")
  { cnorm=ANSI_NORMAL; color=ANSI_GREEN; }
  else { cnorm=""; color=""; }

  tell_object(ME,color+"Du jagst "+arg->name(WEN)+" nicht mehr.\n"+cnorm);
}

varargs int StopHuntFor(object arg, int silent)
{
  if (!objectp(arg) || !IsEnemy(arg)) return 0;
  if (!silent)
    StopHuntText(arg);
  efun::m_delete(enemies,arg);
  last_attack_msg=0;
  return 1;
}

void Attack(object enemy)
{
  int i,damage;
  object weaponob;
  string damage_msg, damage_msg2, wtype;
  mixed res,spell,damage_type;
  mapping ainfo;

  if (no_more_attacks || QueryProp(P_GHOST) ||
      (objectp(enemy) && enemy->QueryProp(P_NOATTACK)) ||
      (query_once_interactive(this_object()) && !interactive(this_object())))
    return;
  Set(P_LAST_COMBAT_TIME,time());

  if (QueryProp(P_NOATTACK))
  {
    StopHuntFor(enemy, 1);     // 1='silent'
    enemy->StopHuntFor(ME, 1);
    return;
  }
  no_more_attacks=1,spell=0;
  if (res=QueryProp(P_TMP_ATTACK_HOOK))
  {
    if (pointerp(res) && sizeof(res)>=3 && intp(res[0]) &&
        time()<res[0] && objectp(res[1]) && stringp(res[2]))
    {
      if (!(res=call_other(res[1],res[2],enemy))) return;
    }
    else
      SetProp(P_TMP_ATTACK_HOOK, 0);
  }

  if (objectp(weaponob = QueryProp(P_WEAPON)))
  {
    closure cl;

    weaponob->TakeFlaw();
    if (weaponob)
    {
      cl=symbol_function("name",weaponob);
      damage_msg = " mit "+funcall(cl,WEM,0);
      damage_msg2 =  " mit "+funcall(cl,WEM,1);
      cl=symbol_function("QueryProp",weaponob);
      damage = weaponob->QueryDamage(enemy);
      damage_type=funcall(cl,P_DAM_TYPE);
      wtype=funcall(cl,P_WEAPON_TYPE);
      // Zweihaendige Waffe?
      if (funcall(cl,P_NR_HANDS)==2 &&
          mappingp(res=UseSkill(SK_TWOHANDED,([SI_SKILLDAMAGE:damage]))))
        damage=(({int})(res[SI_SKILLDAMAGE]));
    }
  }
  else
  {
    // Check if there is a magical attack
    if (mappingp(res=UseSkill(SK_MAGIC_ATTACK,([SI_ENEMY:enemy]))))
    {
      damage=res[SI_SKILLDAMAGE];
      damage_type=res[SI_SKILLDAMAGE_TYPE];
      if (stringp(res[SI_SKILLDAMAGE_MSG]))
        damage_msg=damage_msg2=" mit "+res[SI_SKILLDAMAGE_MSG];
      else
        damage_msg=damage_msg2=" mit magischen Fähigkeiten";
      if (!(wtype=res[P_WEAPON_TYPE]))
	wtype=WT_MAGIC;
      spell=res[SI_SPELL];
    }
    else
    {
      res = QueryProp(P_HANDS);
      damage = random( res[1]+1 );
      damage = (2*damage + 10*QueryAttribute(A_STR))/3;
      damage_type = res[2];
      damage_msg = damage_msg2 = res[0];
      wtype=WT_HANDS;
    }
  }

  ainfo=([SI_ENEMY:enemy,
	  SI_SKILLDAMAGE:damage,
	  SI_SKILLDAMAGE_TYPE:damage_type,
          SI_SKILLDAMAGE_MSG:damage_msg,
          SI_SKILLDAMAGE_MSG2:damage_msg2,
          SI_SPELL:spell,
	  P_WEAPON:weaponob,
	  P_WEAPON_TYPE:wtype]);

  if (mappingp(res=UseSkill(FIGHT(wtype),ainfo))) {
    // besondere Faehigkeiten mit diesem Waffentyp?
    damage=ainfo[SI_SKILLDAMAGE]=res[SI_SKILLDAMAGE];
    damage_msg=ainfo[SI_SKILLDAMAGE_MSG]=res[SI_SKILLDAMAGE_MSG];
    damage_msg2=ainfo[SI_SKILLDAMAGE_MSG2]=res[SI_SKILLDAMAGE_MSG2];
    if(res[SI_SPELL])spell=ainfo[SI_SPELL]=res[SI_SPELL];
  }
  if (mappingp(res=UseSkill(SK_FIGHT,ainfo))) {
    // besondere allgemeine Kampffaehigkeiten?
    damage=ainfo[SI_SKILLDAMAGE]=res[SI_SKILLDAMAGE];
    damage_msg=ainfo[SI_SKILLDAMAGE_MSG]=res[SI_SKILLDAMAGE_MSG];
    damage_msg2=ainfo[SI_SKILLDAMAGE_MSG2]=res[SI_SKILLDAMAGE_MSG2];
    damage_type=ainfo[SI_SKILLDAMAGE_TYPE]=res[SI_SKILLDAMAGE_TYPE];
    if(res[SI_SPELL])spell=ainfo[SI_SPELL]=res[SI_SPELL];
  }

  if (damage_msg2!=last_attack_msg)
  {
    string cnorm,color;

    if (QueryProp(P_TTY)=="ansi")
    { cnorm=ANSI_NORMAL; color=ANSI_GREEN; }
    else { cnorm=""; color=""; }

    last_attack_msg=damage_msg2;

    if (!(QueryProp(P_BRIEF) & BRIEF_FNOATTACK))
      tell_object( ME, color+"  Du greifst " + enemy->name(WEN,1) +
                   damage_msg2 + " an.\n"+cnorm );

    say( "  "+ME->Name(WER,2) + " greift " + enemy->name(WEN,1)
         + damage_msg + " an.\n", enemy );

    if (enemy->QueryProp(P_TTY)=="ansi")
    { cnorm=ANSI_NORMAL; color=ANSI_RED; }
    else { cnorm=""; color=""; }

    if (!(enemy->QueryProp(P_BRIEF) & BRIEF_FNOATTACK))
      tell_object( enemy, color+"  "+ME->Name(WER,1) + " greift Dich" +
                   damage_msg2 + " an.\n"+cnorm );
  }

  // Bei Blindheit mit 80% Wahrscheinlichkeit Schaden fuenfteln
  if (QueryProp(P_BLIND) && (random(100) > 20))
    damage /= 5;

  enemy->Defend(damage,damage_type,spell,this_object());
  /* Done attacking */
}

void AddDefender(object friend) {
  object *defs;

  if (!objectp(friend)) return;
  if (!pointerp(defs=QueryProp(P_DEFENDERS))) defs=({});
  if (member(defs,friend)>=0) return;
  defs+=({friend});
  SetProp(P_DEFENDERS,defs);
}

void InformDefend(object enemy) {}

mixed DefendOther(int dam, mixed dam_type, mixed spell, object enemy) {
  return 0;
}

void CheckWimpyAndFlee() {
  if (QueryProp(P_WIMPY) > QueryProp(P_HP)
      && find_call_out("Flee")<0)
    call_out("Flee",1,environment());
}

string mess(string msg,object me,object enemy)
{
  closure mname, ename;

  mname = symbol_function("name", me);
  ename = symbol_function("name", enemy);

  msg=implode(efun::explode(msg,"@WER1"),funcall(mname,WER,1));
  msg=implode(efun::explode(msg,"@WESSEN1"),funcall(mname,WESSEN,1));
  msg=implode(efun::explode(msg,"@WEM1"),funcall(mname,WEM,1));
  msg=implode(efun::explode(msg,"@WEN1"),funcall(mname,WEN,1));
  msg=implode(efun::explode(msg,"@WER2"),funcall(ename,WER,1));
  msg=implode(efun::explode(msg,"@WESSEN2"),funcall(ename,WESSEN,1));
  msg=implode(efun::explode(msg,"@WEM2"),funcall(ename,WEM,1));
  msg=implode(efun::explode(msg,"@WEN2"),funcall(ename,WEN,1));
  return break_string(capitalize(msg),78,"  ",1);
}

string* _whathow(string who_s, int dam)
{
/*
    Ausgelagert aus Defend() damit je nach Spieler Prop entschieden werden
    kann welche Schadensmeldung angzeigt wird. Default sind die neuen
    Meldungen.
                                 Querolin@Tamedhon 2002 08 02
*/
   string *answer;
   string what, how;
   object who;

  who=find_player(lower_case(who_s));
/*
  if (who) {
  tell_object(find_player("querolin"),break_string(who->Name()+"->"+
      who->QueryProp(P_FIGHTMESSAGES), 78));
   }
*/
  if ( (!who) ||
       (who->QueryProp(P_FIGHTMESSAGES)==0) ||
       (who->QueryProp(P_FIGHTMESSAGES)=="0") )
  {
    switch (dam)
    {
    case 0:
      what = " verfehl";
      how = "";
      break;
    case 1:
      what = " kitzel";
      how = " am Bauch";
      break;
    case 2:
      what = " kratz";
      how =  " leicht";
      break;
    case 3:
      what = " kratz";
      how  = " etwas";
      break;
    case 4:
      what = " kratz";
      how  = " ziemlich";
      break;
    case 5:
      what = " zerkratz";
      how = "";
      break;
    case 6:
       what = " zerkratz";
       how  = " leicht";
       break;
    case 7:
       what = " kratz";
       how  = " etwas";
       break;
    case 8:
       what = " kratz";
       how  = " ziemlich";
       break;
    case 9:
      what = " triff";
      how = " gerade so";
      break;
    case 10:
      what = " triff";
      how = " leicht";
      break;
     case 11:
      what = " triff";
      how  = "";
      break;
     case 12:
      what = " triff";
      how  = " stärker";
      break;
     case 13:
      what = " triff";
      how  = " härter";
      break;      
     case 14:
      what = " triff";
      how  = " hart";
      break;  
    case 15:
      what = " triff";
      how  = " sehr hart";
      break;
    case 16:
      what = " triff";
      how  = " wirklich hart";
      break;
    case 17:
      what = " triff";
      how  = " ziemlich übel";
      break;  
    case 18:
      what = " triff";
      how = " übel";
      break;
    case 19:
      what = " triff";
      how  = " sehr übel";
      break;
    case 20:
      what = " triff";
      how  = " extrem übel";
      break;
    case 21:
      what = " triff";
      how = " absolut superübel";
      break;
    case 22:
     what = " triff";
     how  = " krachend";
     break;
    case 23:
     what = " triff";
     how  = " mit dem Krachen brechender Knochen";
     break;
    case 24:
     what = " triff";
     how  = " verheerend";
     break;
    case 25..26:
     what = " triff";
     how  = " extrem";
     break; 
    case 27..28:
      what = " schläg";
      how = "";
      break;
    case 29..30:
      what = " schläg";
      how = " mäßig";
      break;
    case 31..32:
      what = " schläg";
      how = " hart";
      break;
    case 33..36:
      what = " schläg";
      how = " sehr hart";
      break;
    case 37..41:
      what = " schläg";
      how = " wirklich hart";
      break;
    case 42..46:
      what = " schläg";
      how = " extrem hart";
      break;
    case 47..50:
      what = " schlaeg";
      how = " schon uebel";
      break;
    case 51..55:
      what = " schläg";
      how = " übel";
      break;
    case 56..60:
      what = " schläg";
      how = " extrem übel";
      break;
    case 61..65:
      what = " schlaeg";
      how = " ziemlich zusammen";
      break;
    case 66..70:
      what = " schläg";
      how  = " zusammen";
      break;  
    case 71..75:
      what = " schläg";
      how  = " schwer zusammen";
      break;   
    case 76..80:
      what = " schläg";
      how  = " übel zusammen";
      break;
    case 81..85:
      what = " schläg";
      how  = " richtig übel zusammen";
      break;
    case 86..90:
      what = " schläg";
      how  = " wirlich richtig übel zusammen";
      break;
    case 91..99:
      what = " zerschmetter";
      how = " am Boden";
      break;
    case 100..109:
      what = " zerpflück";
      how = " in der Luft";
      break;
    case 110..119:
      what = " verletz";
      how = "";
      break;
    case 120..129:
      what = " verletz";
      how = " schwer";
      break;
    case 130..139:
      what = " verletz";
      how = " lebensgefährlich";
      break;
    case 140..149:
      what = " verletz";
      how = " tödlich";
      break;
    case 150..159:
      what = " hau";
      how = " von den Füßen";
      break;
    case 160..169:
      what = " ramm";
      how = " ungespitzt in den Boden";
      break;
    case 170..179:
      what = " zerleg";
      how = " in kleine Häppchen";
      break;
    case 180..189:
      what = " hau";
      how = " zu Klump";
      break;
    case 190..200:
      what = " zerfetz";
      how = "";
      break;
    case 201..259:
      what = " zerbrösel";
      how = "";
      break;
    case 260..299:
      what = " pulverisier";
      how = "";
      break;
    case 300..599:
      what = " katapultier";
      how = " in die Umlaufbahn";
      break;
    default:
      what = " vernichte";
      how = "";
   }
  }
  else
  {
    switch (dam)
    {
    case 0:
      what = " verfehl";
      how = "";
      break;
    case 1:
      what = " kitzel";
      how = " am Bauch";
      break;
    case 2..3:
      what = " kratz";
      how = "";
      break;
    case 4..5:
      what = " triff";
      how = "";
      break;
    case 6..10:
      what = " triff";
      how = " hart";
     break;
    case 11..15:
      what = " triff";
      how = " sehr hart";
      break;
    case 16..20:
      what = " triff";
      how = " extrem hart";
      break;
    case 21..30:
      what = " schläg";
      how = " mit dem Krachen brechender Knochen";
      break;
    case 31..40:
      what = " zerschmetter";
      how = " in kleine Stückchen";
      break;
    case 41..50:
      what = " schläg";
      how = " zu Brei";
      break;
    case 51..60:
      what = " pulverisier";
      how = "";
      break;
    case 61..70:
      what = " hau";
      how = " zu Klump";
      break;
    case 71..80:
      what = " zerstäub";
      how = "";
      break;
    case 81..90:
      what = " zerfetz";
      how = " in kleinste Teilchen";
      break;
    case 91..100:
      what = " zerbrösel";
      how = " zu feinem Staub";
      break;
    case 101..130:
      what = " atomisier";
      how = "";
      break;
    case 131..160:
      what = " schleuder";
      how = " einmal rund ums Tamedhon";
      break;
    case 161..199:
      what = " katapultier";
      how = " in die Umlaufbahn";
      break;
    default:
      what = " vernichte";
      how = "";
    }
  }
  answer=({ what, how });
  return answer;
}

int Defend(int dam, mixed dam_type, mixed spell, object enemy)  {
  int i,new_enemy;
  string what, how;
  string *whathow;
  object *armours;
  mixed res,res2;

  if (!(enemy=(enemy||this_player())) ||
      (query_once_interactive(enemy) && ! interactive(enemy)))
    return 0;
  if (!check_attack(enemy, ME)) return 0;  // Der Angreifer ist hier 'enemy'
  if (stringp(dam_type)) dam_type = ({ dam_type });
  if (new_enemy=(!IsEnemy(enemy))) InsertEnemy(enemy);

  if (res=QueryProp(P_DEFENDERS)) {
    object *defs;

    defs=({});if (!pointerp(res)) res=({res});
    for (i=sizeof(res)-1;i>=0;i--)
      if (objectp(res[i]) && member(defs,res[i])<0 &&
          environment(res[i])==environment()) {
        defs+=({res[i]});
	call_other(res[i],"InformDefend",enemy);
      }
    SetProp(P_DEFENDERS,defs);
    if (!sizeof(defs)) {
      SetProp(P_DEFENDERS,0);
    } else {
      res=call_other(defs[random(i)],"DefendOther",dam,dam_type,spell,enemy);
      if (pointerp(res) && sizeof(res)>=3 && intp(res[0]) && pointerp(res[1])){
        // Helfer koennen den Schaden oder Schadenstyp aendern,
        // z.B. Umwandlung von Feuer nach Eis oder so...
        dam=res[0];
        dam_type=res[1];
        spell=res[2];
      }
    }
  }

  if (res=QueryProp(P_TMP_DEFEND_HOOK)) {
    if (pointerp(res) && sizeof(res)>=3
        && intp(res[0]) && time()<res[0]
        && objectp(res[1]) && stringp(res[2])) {
      if (!(res=call_other(res[1],res[2],dam,dam_type,spell,enemy))) {
        return (int) CheckWimpyAndFlee();
      } else {
        if (pointerp(res) && sizeof(res)>=3 &&
            intp(res[0] && pointerp(res[1]))) {
          dam=res[0];
          dam_type=res[1];
          spell=res[2];
        }
      }
    } else {
      SetProp(P_TMP_DEFEND_HOOK,0);
    }
  }

  armours=QueryProp(P_ARMOURS);
  if( i=sizeof(armours) )
  {
    object tmp;

    // Abnutzung eines zufaellig gewaehlten Ruestungsteils
    tmp = armours[random(i)];
    if (tmp) tmp->TakeFlaw(dam_type);
    // Abnutzung kann Ruestungsteil zerstoeren!
    if (!objectp(tmp)) armours -= ({tmp});
  }
  if (mappingp(spell) && mappingp(res=spell[SP_REDUCE_ARMOUR]))
  {
    for (i=sizeof(armours)-1;i>=0;i--)
      if (armours[i]) {
	if (intp(res2=res[armours[i]->QueryProp(P_ARMOUR_TYPE)]) && res2>0)
	  dam-=(res2*armours[i]->QueryDefend(dam_type, spell, enemy))/100;
	else
	  dam-=armours[i]->QueryDefend(dam_type, spell, enemy);
      }
  } else { // Normale Ruestungberechnung
    for (i=sizeof(armours)-1;i>=0;i--)
      if (armours[i])
	dam -= armours[i]->QueryDefend(dam_type, spell, enemy);
  }

  if (mappingp(res=UseSkill(SK_MAGIC_DEFENSE,
                            ([SI_ENEMY:enemy,
			      SI_SKILLDAMAGE:dam,
			      SI_SKILLDAMAGE_TYPE:dam_type,
			      SI_SPELL:spell])))) {
    dam=res[SI_SKILLDAMAGE];
    dam_type=res[SI_SKILLDAMAGE_TYPE]; // Skill kann Typ umwandeln
  }
  CheckSensitiveAttack(dam,dam_type,spell,enemy);
  if (!enemy) return 0;
  enemy->SetProp(P_LAST_COMBAT_TIME, time());

  dam = (int)(CheckResistance(dam_type)*dam);
  // if (!spell || (mappingp(spell) && spell[SP_PHYSICAL_ATTACK]))
  //   dam = dam - random (QueryProp(P_BODY) + QueryAttribute(A_DEX)) - 1;
  if (!spell || (mappingp(spell) && spell[SP_PHYSICAL_ATTACK])) {
    res2=random(QueryProp(P_BODY)+QueryAttribute(A_DEX))-1;
    if (mappingp(spell)
        && mappingp(res=spell[SP_REDUCE_ARMOUR])
        && intp(res=res[P_BODY])
        && res>0)
      res2=(res2*res)/100; // Reduzierbare Wirksamkeit des Bodies
    dam-=res2;
  }

  if (dam < 0) dam = 0;
  dam = dam / 10;

  if (!spell || (mappingp(spell) && spell[SP_SHOW_DAMAGE] &&
                 !pointerp(spell[SP_SHOW_DAMAGE])))
  {
/*
    if ((dam > 10) || !(enemy->QueryProp(P_BRIEF) & BRIEF_FNODAMAGE))
      if (what[<1] != 'z')
        tell_object( enemy, "  Du"+what+"st "+ME->name(WEN,1) + how + ".\n");
      else
        tell_object( enemy, "  Du"+what+"t "+ME->name(WEN,1) + how + ".\n");
    if ((dam > 10) || !(QueryProp(P_BRIEF) & BRIEF_FNODAMAGE))
      tell_object( ME, "  " + enemy->Name(WER,1) + what +
                   "t Dich" + how + ".\n" );
 */

  whathow=_whathow(enemy->Name(),dam);
  what=whathow[0];
  how=whathow[1];

  if (enemy->QueryProp(P_TTY)=="ansi")
  {
    if ((dam > 10) || !(enemy->QueryProp(P_BRIEF) & BRIEF_FNODAMAGE))
      if (what[<1] != 'z')
        tell_object( enemy, ANSI_GREEN+"  Du"+what+"st "+ME->name(WEN,1) +
                     how + ".\n" + ANSI_NORMAL);
      else
        tell_object( enemy, ANSI_GREEN+"  Du"+what+"t "+ME->name(WEN,1) +
                     how + ".\n" + ANSI_NORMAL);
   }
   else
   {
    if ((dam > 10) || !(enemy->QueryProp(P_BRIEF) & BRIEF_FNODAMAGE))
      if (what[<1] != 'z')
        tell_object( enemy, "  Du"+what+"st "+ME->name(WEN,1) + how + ".\n");
      else
        tell_object( enemy, "  Du"+what+"t "+ME->name(WEN,1) + how + ".\n");
   }

  whathow=_whathow(ME->Name(),dam);
  what=whathow[0];
  how=whathow[1];

  if (QueryProp(P_TTY)=="ansi")
  {
    if ((dam > 10) || !(QueryProp(P_BRIEF) & BRIEF_FNODAMAGE))
      tell_object( ME, ANSI_RED+"  " + enemy->Name(WER,1) + what +
                   "t Dich" + how + ".\n" + ANSI_NORMAL );
  }
  else
  {
    if ((dam > 10) || !(QueryProp(P_BRIEF) & BRIEF_FNODAMAGE))
      tell_object( ME, "  " + enemy->Name(WER,1) + what +
                   "t Dich" + how + ".\n" );
  }

    whathow=_whathow("",dam);
    what=whathow[0];
    how=whathow[1];

    say( "  " + enemy->Name(WER,1) + what + "t "
    + ME->name(WEN,1) + how + ".\n", enemy );
  }
  else if(mappingp(spell) && (res=spell[SP_SHOW_DAMAGE]) && pointerp(res))
  {
    for(i=sizeof(res)-1;i>=0;i--)
      if(dam > res[i][0])
      {
        tell_object(ME,mess(res[i][1],ME,enemy));
        tell_object(enemy,mess(res[i][2],ME,enemy));
        say(mess(res[i][3],ME,enemy), enemy);
        break;
      }
  }

  Set(P_LAST_DAMTYPES, stringp(dam_type) ? ({ dam_type }) : dam_type);
  Set(P_LAST_DAMAGE, dam);

  do_damage(dam,enemy);
  CheckWimpyAndFlee();
  return dam;
}

float CheckResistance(string *dam_type)
{
  mapping map;
  float faktor, n;
  int i, b, d;

  b=QueryProp(P_BLIND); d=QueryProp(P_DEAF);
  if (!mappingp(map=QueryProp(P_RESISTANCE_STRENGTHS)))
    if (!b && !d)
      return 1.0;
    else
      map = (b ? ([ DT_LIGHT : -0.5 ]) : ([])) +
            (d ? ([ DT_SOUND : -0.5 ]) : ([]));
  else if (b || d) {
    map = copy(map);
    if (b) map[DT_LIGHT]=map[DT_LIGHT]-0.5;
    if (d) map[DT_SOUND]=map[DT_SOUND]-0.5;
  }
  if (!pointerp(dam_type)) {
    log_file("bad_damage",
             sprintf ("in %O\n\t%O\n",this_object(),dam_type));
    // dam_type=({dam_type});
    dam_type=({});
  }
  i=sizeof(dam_type);
  if (i<1)
    return 1.0;
  n=((float)i);
  faktor=0.0;
  for (--i;i>=0;i--)
    faktor=faktor+(({float})map[dam_type[i]]);
  return 1.0+(faktor/n);
}

varargs mixed StopHuntingMode(int silent)
{
  mapping save_enemy;
  int i;

  save_enemy=enemies;
  if (!silent) walk_mapping(enemies, #'StopHuntText); //#');
  enemies=m_allocate(0,1);
  last_attack_msg=0;
  return save_enemy;
}

object QueryEnemy() { /* Spellhandler needs it */
  return SelectEnemy();
}

mixed QueryEnemies() {
  return ({m_indices(enemies),m_values(enemies)});
}

mapping GetEnemies() {
  return enemies;
}

mapping SetEnemies(<object*|int*>* *myenemies) {
  enemies=mkmapping(myenemies[0],myenemies[1]);
  return enemies;
}

varargs void Flee(object oldenv) {
  mixed *exits,exit,dex;
  mapping tmp;
  int i;
  object env;

  if (extern_call() && previous_object() != this_object()) return;
  if (oldenv && oldenv!=environment()) return; // Fuer call_outs
  if (IS_LEARNING(ME) && query_once_interactive(ME)) return;
  if (QueryProp(P_GHOST)) return;
  tell_object(ME,
    "Die Angst ist stärker als Du ... Du willst nur noch weg hier.\n");
  if (!environment()) return;
  exits=({});
  env=environment();
  tmp=environment()->QueryProp(P_EXITS);
  exits = m_indices(tmp);
  tmp=environment()->QueryProp(P_SPECIAL_EXITS);
  exits += m_indices(tmp);

  tmp=environment()->_query_doors();
  if (sizeof(tmp)==2) {
    i=sizeof(tmp[0]);
    while (i)
      if (tmp[1][--i][DOOR_STATUS]<DOOR_STATUS_CLOSED)
        exits+=({tmp[0][i]});
  }
  if (IS_SEER(ME)
      && interactive(ME)
      && dex=QueryProp(P_WIMPY_DIRECTION)) {
//  i=80+3*(QueryProp(P_LEVEL)-20)/2;
    i=QueryProp(P_LEVEL)+2*QueryAttribute(A_DEX);
    exits+=({dex}); // bevorzugte Fluchtrichtung mindestens einmal
  }
  if (!sizeof(exits)) {
    tell_object(ME,"Du versuchst zu fliehen, schaffst es aber nicht.\n");
    return;
  }
  while (sizeof(exits) && environment()==env) {
    if (dex                       // Vorzugsweise Fluchtrichtung?
        && (member(exits,dex)>=0) // moeglich?
        && (random(100)<=i))      // und Wahrscheinlichkeit gross genug?
      exit=dex;
    else
      exit=exits[random(sizeof(exits))];
    catch(command(exit));
    exits-=({exit});
  }
  if (environment()==env)
    tell_object(ME,"Dein Fluchtversuch ist gescheitert.\n");
  else if (exit!=dex)
  {
    tmp=env->QueryProp(P_NEW_EXITS);
    if (mappingp(tmp) && member(tmp,exit) && mappingp(tmp[exit]))
      tell_object(ME,"In panischer Angst bist Du "+
          (tmp[exit][EX_MSG] || "irgendwohin")+" geflüchtet.\n");
    else
    {
      if( objectp(ME) )
        tell_object(ME,"In panischer Angst bist Du irgendwohin geflüchtet.\n");
    }
  }
}

mixed EnemyPresent()
{
  int i;
  mixed *all;

  all=m_indices(enemies);
  for (i=sizeof(all)-1;i>=0;i--)
    if (all[i] && environment()==environment(all[i]))
      return all[i];
  return 0;
}

mixed InFight()
{
  return EnemyPresent();
}

varargs int StopHuntID(string str, int silent)
{
  int i,j;
  object *obs;

  if(!stringp(str)) return 0;
  obs=m_indices(enemies);
  for (i=sizeof(obs)-1;i>=0;i--)
    if (objectp(obs[i]) && obs[i]->id(str))
      StopHuntFor(obs[i],silent),j++;
  return j;
}

// **** this is property-like
int _set_attack_busy(int dummy)
{
  return (attack_busy++);
}

int _query_attack_busy()
{
  int lvl;

  lvl=(QueryProp(P_LEVEL)-14)/7; // Bonus ab 21 ...
  if (lvl<0 || !IS_SEER(ME)) lvl=0;
  if (lvl>7) lvl=7;
  return (attack_busy>lvl);
}

// **** local property methods
static int _set_wimpy(int i)
{
  if(!intp(i) || i>QueryProp(P_MAX_HP) || i<0) return 0;
  return Set(P_WIMPY, i);
}

static mixed _set_hands(mixed h)
{
  if(!QueryProp(P_WEAPON)) SetProp(P_TOTAL_WC, h[1]);
  if(sizeof(h)==2) h += ({ DT_BLUDGEON });
  return Set(P_HANDS, h);
}

static mixed _query_hands()
{
  if(!Query(P_HANDS) )
    return Set(P_HANDS, ({ " mit bloßen Händen", 30, DT_BLUDGEON }));
  return Query(P_HANDS);
}

static int _query_total_ac()
{
  return Query(P_TOTAL_AC)+QueryProp(P_BODY)+QueryAttribute(A_DEX);
}

int _set_disable_attack(int val) {
  if (val>30)
    val=30;
  if (val>=10 && previous_object(1) && previous_object()!=this_object() )
    log_file("PARALYSE",sprintf("%s %d %O %O\n",ctime(time())[4..15],
                                val,previous_object(1),this_player()));
  return Set(P_DISABLE_ATTACK,val);
}

// Neue Verwaltung der Haende:

// P_HANDS_USED_BY enhaelt ein Array mit allen Objekten, die Haende
// belegen, jedes kommt so oft vor wie Haende belegt werden.

mixed *_query_hands_used_by() {
  return ((Query(P_HANDS_USED_BY)||({}))-({0}));
}

int _query_used_hands() {
  return sizeof(QueryProp(P_HANDS_USED_BY));
}

int _query_free_hands() {
  return QueryProp(P_MAX_HANDS)-QueryProp(P_USED_HANDS);
}

varargs int UseHands(object ob, int num) {
  mixed *h;
  int i;

  if (!ob && !(ob=previous_object(1)))
    return 0;
  if (num<=0 && (num=ob->QueryProp(P_HANDS))<=0)
    return 0;
  h=QueryProp(P_HANDS_USED_BY)-({ob});
  if (sizeof(h)+num>QueryProp(P_MAX_HANDS))
    return 0;
  for (i=0;i<num;i++)
    h+=({ob});
  SetProp(P_HANDS_USED_BY,h);
  return 1;
}

varargs int FreeHands(object ob) {
  if (!ob && !(ob=previous_object(1)))
    return 0;
  SetProp(P_HANDS_USED_BY,QueryProp(P_HANDS_USED_BY)-({ob}));
  return 1;
}

// Kompatibilitaetsfunktionen:

int _set_used_hands(int new_num) {
  int old_num, dif;
  object ob;

  old_num=QueryProp(P_USED_HANDS);
  if (!objectp(ob=previous_object(1)))
    return old_num;
  if (!(dif=new_num-old_num))
    return new_num;
  if (dif>0)
    UseHands(ob,dif);
  else
    FreeHands(ob);
  return QueryProp(P_USED_HANDS);
}
