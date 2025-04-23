// Tamedhon MUDlib
//
// std/player/life.c -- player life handling
//
// $Date: 2008/05/11 20:00:00 $
/* $Revision: 4.2 $
 * $Log: life.c,v $
 *
 * Revision 4.2  2008/05/11 20:00:00  TamTam
 * StopHunt raus, passiert nochmal in die(), sonst Bug in P_TMP_DIE_HOOK
 *
 * Revision 4.1  2004/06/02 20:00:00  Serii
 * wenn man durch sich selber stirbt (daemonen durch ihr align etc ;) )
 * wirds nun nichtmehr als autorisierter PK registriert
 *
 * Revision 4.0  2004/03/30 22:00:00  Serii
 * pk-bugfix
 *
 * Revision 3.95  2004/02/02 19:00:00  Serii
 * kleiner fix an _set_authorizedplayerkills()
 *
 * Revision 3.9  2004/01/17 12:00:00  Querolin
 * second_life() und undie() auf P_LAST_LOST_XP angepasst.
 *
 * Revision 3.8  2003/10/02 23:35:00  Serii
 * P_PLAYERKILLS um ein paar kleine sachen erweitert
 *
 * Revision 3.7  2003/03/25 00:30:00  Serii
 * handling fuer P_PLAYERKILLS eingebaut
 *
 * Revision 3.6  2002/08/08 08:30:20  Querolin
 * undie fehler beseitigt
 *
 * Revision 3.5  2002/03/22 22:40:50  Querolin
 * undie ueberarbeitet
 *
 * Revision 3.4  1997/11/25 15:53:50  Feyaria
 * undie setzt xp, P_LOST_XP eingefuehrt
 *
 * Revision 3.3  1997/09/17 14:39:25  Woody
 * undie() eingebaut, Magierabfrage bei Tod etwas verschoben
 *
 * Revision 3.2  1997/07/18 21:26:08  Woody
 * Erschoepft Meldungen hier rausgenommen
 *
 * Revision 3.1  1997/06/25 12:02:02  Woody
 * P_MV hochidlen, bei Tod nicht mehr blind/taub/voll
 * Meldungen, wenn erschoepft
 *
 * Revision 3.0  1997/03/13 01:39:14  Woody
 * prepared for revision control
 *
 * Revision 2.26  1996/05/28  23:30:00  Woody
 * _set_align() nimm nun mixed statt int als Parameter
 *
 * Revision 2.25  1995/08/10  19:41:40  Rochus
 * NotifyPlayerDeath auch an Killer schicken.
 *
 * Revision 2.24  1995/07/20  15:47:47  Boing
 * _set_align() ist jetzt ueberladen, keine Beschraenkung mehr fuer Spieler.
 * Siehe dazu auch den direkten Kommentar zur Funktion.
 *
 * Revision 2.23  1995/07/16  13:40:25  Silvana
 * Dito in do_damage()
 *
 * Revision 2.22  1995/07/16  13:03:27  Silvana
 * Uebungsraum fuer Karateka eingetragen in _set_playerkills()
 *
 * Revision 2.21  1995/04/27  19:01:24  Jof
 * hook for _set_sp
 *
 * Revision 2.20  1995/03/01  15:58:09  Jof
 * P_LAST_KILLER
 * NotifyPlayerDeath
 *
 * Revision 2.19  1995/02/28  11:08:47  Jof
 * Moved Attack-Logging to living/combat
 *
 * Revision 2.19  1995/02/28  17:47:36  Jof
 * Dont set PK in our killer if we are the attacker
 *
 * Revision 2.18  1995/01/30  10:50:57  Jof
 * Dont LOG pks in new arena
 *
 * Revision 2.17  1994/12/23  12:24:39  Jof
 * *** empty log message ***
 *
 * Revision 2.16  1994/12/14  19:41:22  Jof
 * *** empty log message ***
 *
 * Revision 2.15  1994/12/13  14:06:51  Jof
 * *** empty log message ***
 *
 * Revision 2.14  1994/10/21  15:12:07  Rochus
 * Bug in do_damage() gefixed.
 *
 * Revision 2.13  1994/10/16  16:50:25  Boing
 * Zweitie-Tode werden geloggt
 *
 * Revision 2.12  1994/10/04  09:54:21  Jof
 * *** empty log message ***
 *
 * Revision 2.12  1994/10/04  15:10:54  mud
 * *** empty log message ***
 *
 * Revision 2.12  1994/10/04  15:10:54  mud
 * *** empty log message ***
 *
 * Revision 2.11  1994/09/22  09:40:26  Hate
 * (Rumata) selbstmord verhindern.
 *
 * Revision 2.10  1994/07/10  21:38:37  Jof
 * *** empty log message ***
 *
 * Revision 2.10  1994/07/10  21:38:37  Jof
 * *** empty log message ***
 *
 * Revision 2.9  1994/06/27  11:37:28  Jof
 * hp hooks
 *
 * Revision 2.8  1994/06/13  20:14:05  Rochus
 * /log/KILLS Eintrag geaendert.
 *
 * Revision 2.7  1994/06/13  07:36:24  Jof
 * Removed SET_METHODs
 *
 * Revision 2.6  1994/05/15  17:57:40  Jof
 * Some more checks ...
 *
 * Revision 2.5  1994/04/15  17:57:46  Rumata
 * Alle Gegner geben beim Tod des Spielers die Jagd auf ihn auf.
 *
 * Revision 2.4  1994/03/18  18:56:34  Jof
 * LEARNER koennen nicht mehr sterben
 *
 * Revision 2.3  1994/03/17  23:36:55  Jof
 * PROTECTED -> SECURED & Reindented
 *
 * Revision 2.2  1994/02/24  13:03:14  Jof
 * Private removed
 *
 * Revision 2.1  1994/02/08  17:59:54  Jof
 * *** empty log message ***
 *
 * Revision 2.0  1994/01/13  16:58:56  mud
 * *** empty log message ***
 *
 * Revision 1.3  1994/01/08  13:12:20  mud
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/05  16:17:07  mud
 * changed save variables partly to mapping entry
 *
 */

// Defines some things which are different than in living.c
// One example is the heart_beat().

inherit "std/living/life";

#pragma strong_types

#define NEED_PROTOTYPES

#include "/sys/thing/properties.h"
#include "/sys/player/base.h"

#include <properties.h>
#include <config.h>
#include <health.h>
#include <wizlevels.h>
#include <language.h>
#include <living/combat.h>

#include <defines.h>
#include <moving.h>

#include "/p/daemon/pkmaster/pkmaster.h"
static int _set_playerkills(int val);
static int _set_authorizedplayerkills(string *nara);
static int delay_mv;

void create()
{
  ::create();
  Set(P_KILLS, SAVE, F_MODE_AS);
  Set(P_KILLS, SECURED, F_MODE_AS);
  Set(P_PLAYERKILLS, SAVE, F_MODE_AS);
  Set(P_PLAYERKILLS, SECURED, F_MODE_AS);
  Set(P_GHOST, SAVE, F_MODE_AS);
  delay_mv=DELAY_MV;
}

static int CheckForArch()
{
  int i;
  object tmp,tmp2;

  i=1;
  while (tmp=previous_object(i))
  {
    i++;
    tmp2=tmp;
  }
  return (tmp2 && IS_ARCH(geteuid(tmp2)));
}

static int _set_authorizedplayerkills(string *nara)
{
  mixed setter;
  mapping pks;
  setter=RPL;
  if(!setter) setter=PL;
  if(!setter) setter="??";
  else setter=getuid(setter);
  pks=Query(P_PLAYERKILLS);
  if(!pks)pks=([]);
  if(CheckForArch()||
  (((previous_object()==this_object())&&
    ((previous_object(2)->query_real_name()==nara[0])&&
    (query_once_interactive(previous_object(2)))))||
    (previous_object()==find_player(nara[0]))&&
    (query_once_interactive(previous_object())))){
    if(!pks[PK_INFO]){pks[PK_INFO]=([PK_COUNT:1,PK_LAST:nara[0],PK_TIME:time()]);
      pks[PK_INFO][PK_RACE]=([]);}
    else {pks[PK_INFO][PK_COUNT]=pks[PK_INFO][PK_COUNT]+1;
          pks[PK_INFO][PK_LAST]=nara[0];pks[PK_INFO][PK_TIME]=time();}
    pks[PK_INFO][PK_RACE][nara[1]]=pks[PK_INFO][PK_RACE][nara[1]]+1;
    if(!pks[nara[0]])pks[nara[0]]=({1,nara[1],nara[2],nara[3],time()});
    else {pks[nara[0]][PKPI_KNR]=pks[nara[0]][PKPI_KNR]+1;pks[nara[0]]=pks[nara[0]]+({time()});}
  }
  else{
    log_file("SET_AUTHORIZEDPKILLS",setter+": "+getuid(ME)
	     +" ({"+nara[0]+","+nara[1]+"}) "+dtime(time())+" illegal!\n");
    return 0;}
    log_file("SET_AUTHORIZEDPKILLS",setter+": "+getuid(ME)
	     +" ({"+nara[0]+","+nara[1]+"}) "+dtime(time())+"\n");
  return Set(P_PLAYERKILLS,pks);
}

static int _set_playerkills(int val)
{
  mixed setter;
  string fn;
  int playerkills,is_arch,is_arena;
  object po,poe;

  setter=RPL;
  if(!setter) setter=PL;
  if(!setter) setter="??";
  else setter=getuid(setter);
  log_file("SET_KILLS", setter + ": " + getuid(ME)
	     +" " + (string)val + " "+dtime(time())+ "\n");

  is_arch=CheckForArch();
  playerkills = Query(P_KILLS);
  if( intp(val) && val>=0 )
    if(is_arch) playerkills = val;
    else  log_file("SET_KILLS", setter + ": " + getuid(ME)
		   +" " + (string)val + " "+dtime(time())+ " illegal!\n");
  else /*if(!call_other(PKMASTER,"CheckForPkEnvironment",previous_object(1)))*/
       playerkills++;
//    else{return 0;}
  return Set(P_KILLS, playerkills);
}

/* Hier wird _set_align() aus living/life.c ueberladen, damit das Spieler-
 * alignment nicht mehr auf +/- 1000 begrenzt ist. Durch die Beschraenkung
 * des Monster-alignments und durch die Formel zur Neuberechnung des
 * Alignments, bleibt der Wert fuer Spieler aber trotzdem beschraenkt.
 *
 * bugfix: es wird wieder darauf beschraenkt!
 */
static int _set_align(int a)
{
  if(a < -1000) a= (-1000);
  if(a > 1000) a= 1000;
  return Set(P_ALIGN, a);
}

static int time_to_save;	/* when to next save player */

static void heart_beat() {
  int mv;

  ::heart_beat();
  if (age > time_to_save) {
    save_me(1);
    time_to_save = age + 500;
  }
  if (--delay_mv < 0) {
    mv=QueryProp(P_MV);
    if (this_object()->InFight())
      mv+=MVP_GAIN_INFIGHT;
    else
      mv+=QueryProp(P_RESTING) ? MVP_GAIN_REST : MVP_GAIN_NORMAL;
    SetProp(P_MV, mv);
    delay_mv=DELAY_MV;
  }
}

void force_save() {
  time_to_save=0;
}

nomask int do_damage(int dam, object enemy) {
  string tmp,tmp2;
  int hit_point;

  hit_point = QueryProp(P_HP);
  if ( QueryProp(P_GHOST) || dam<=0 ) return 0;
  hit_point -= dam;
  if ( hit_point < 0 )
  {
    if (IS_LEARNING(ME) && query_once_interactive(ME))
    {
      tell_object(ME, "Deine magischen Kräfte verhindern Deinen Tod.\n" );
      return 0;
    }
    if (!interactive( ME ))
    {
      hit_point = 10;   // Tod bei Netztoten verhindern
    }
    else
    {
      if ( enemy && objectp(enemy) && interactive( enemy ) && enemy!=ME &&
           /*
	   object_name(environment(ME))[0..14]!="/d/schattenwelt/" &&
	   environment(ME)!=find_object("/d/wueste/room/arena/arena") &&
           environment(ME)!=find_object("/d/fernwest/li/xian/room/shinto") &&
           */
	   !QueryProp(P_TESTPLAYER)&&!(call_other(PKMASTER,"CheckForPkEnvironment",
				                               ME,enemy)||
				          (call_other(PKMASTER,"CheckForHitList",
					                      ME,enemy)==2)))
      {
	if (QueryPlAttacked(enemy))
	  hit_point=1;
	else
	  hit_point=0;
	if (!hit_point) enemy->SetProp( P_KILLS, -1 );
	log_file("KILLER",
		 sprintf("%s %s(%d/%d) toetete %s(%d/%d)%s\n",
			 ctime(time()),
			 getuid(enemy),query_wiz_level(enemy),
			 enemy->QueryProp(P_LEVEL),getuid(ME),
			 query_wiz_level(ME),QueryProp(P_LEVEL),
			 (hit_point?" NOTWEHR=>KEIN PK":"")));
      } else {
	tmp=dtime(time());
	tmp2=((objectp(enemy)?(explode(object_name(enemy),"#")[0]):""));
	if (sizeof(tmp2)>33) tmp2="..."+tmp2[<31..];
	if (!QueryProp(P_TESTPLAYER))
	  create_kill_log_entry(tmp2+" ("+
				((objectp(enemy)?getuid(enemy):"?"))
				+")");
      }
      if (enemy)
	enemy->StopHuntFor(this_object(),1);
      hit_point = -1;
//      map_objects( QueryEnemies()[0], "StopHuntFor", ME, 1 );
//      StopHuntingMode(1);
      Set(P_LAST_KILLER,enemy);
      Set(P_ALCOHOL,0);
      Set(P_DRINK,0);
      Set(P_FOOD,0);
      Set(P_BLIND,0);
      Set(P_DEAF,0);
      die();
    }
  }
  SetProp(P_HP, hit_point);
  return dam;
}

varargs int second_life(string arg) {
  int lost_exp, l;
  mixed killer;

  if (IS_LEARNING(this_object()) && query_once_interactive(this_object()))
  {
    tell_object(this_object(),
		"Sei froh, dass Du unsterblich bist, sonst wäre es eben zu Ende gewesen.\n");
    return 1;
  }
  if (!IS_SEER(ME) || (l=QueryProp(P_LEVEL))<20)
    lost_exp = QueryProp(P_XP)/3;
  else
//    lost_exp = QueryProp(P_XP)/(l-17);
     lost_exp= (QueryProp(P_XP)/10000)*(30000/l);

     // by Querolin: XP/10000 = 100.00 (2 Kommastellen)

  AddExp(-lost_exp);
  SetProp(P_LOST_XP, QueryProp(P_LOST_XP)+lost_exp);
  Set(P_LAST_LOST_XP, SAVE, F_MODE_AS);
  SetProp(P_LAST_LOST_XP, lost_exp);
  log_file("LAST_LOST_XP", QueryProp(P_NAME)+" has lost "+to_string(lost_exp)+".\n");
  SetProp( P_GHOST, 1 );
  if (objectp(killer=Query(P_LAST_KILLER)) && killer!=environment())
    catch(killer->NotifyPlayerDeath(ME,killer,lost_exp));
  if (environment())
    catch(environment()->NotifyPlayerDeath(ME,killer,lost_exp));
  clone_object( "room/death/death_mark" )->move( ME, M_NOCHECK );
  UpdateAttributes(); // Beim Tod werden Dinge entfernt, Attribute pruefen
  return 1;
}

int AddHpHook(object ob)
{
  object *hooks;

  if (!ob || !objectp(ob) || environment(ob)!=ME)
    return 0;
  hooks=Query(P_HP_HOOKS);
  if (!pointerp(hooks))
  {
    Set(P_HP_HOOKS,({ob}));
    return 1;
  }
  if (member(hooks,ob)>=0)
    return 0;
  Set(P_HP_HOOKS,hooks+({ob}));
  return 1;
}

int RemoveHpHook(object ob)
{
  object *hooks;

  if (!pointerp(hooks=Query(P_HP_HOOKS)))
    return 0;
  Set(P_HP_HOOKS,hooks-({ob}));
  return 1;
}

static int _set_hp(int hp)
{
  object *hooks;
  int ret,i,old;

  if ((old=Query(P_HP))==hp)
    return old;
  ret=life::_set_hp(hp);
  if (ret==old || !pointerp(hooks=Query(P_HP_HOOKS)))
    return ret;
  i=0;
  while (i<sizeof(hooks))
    if (!objectp(hooks[i]))
      hooks=hooks[0..i-1]+hooks[i+1..];
    else
    {
      hooks[i]->NotifyHpChange();
      i++;
    }
  Set(P_HP_HOOKS,hooks);
  return ret;
}

static int _set_sp(int sp)
{
  object *hooks;
  int ret,i,old;

  if ((old=Query(P_SP))==sp)
    return old;
  ret=life::_set_sp(sp);
  if (ret==old || !pointerp(hooks=Query(P_HP_HOOKS)))
    return ret;
  i=0;
  while (i<sizeof(hooks))
    if (!objectp(hooks[i]))
      hooks=hooks[0..i-1]+hooks[i+1..];
    else
    {
      hooks[i]->NotifyHpChange();
      i++;
    }
  Set(P_HP_HOOKS,hooks);
  return ret;
}

// #define TEST_MV
#ifdef TEST_MV
static int _set_mv(int mv)
{
  object *hooks;
  int ret,i,old;

  if ((old=Query(P_MV))==mv)
    return old;
  ret=life::_set_mv(mv);
  if (ret!=old)
    tell_object(this_object(), sprintf(" * %d MVP\n", ret));
  return ret;
}
#endif

int undie() {
  mixed x,di;

  if (!this_interactive())
    return 0;
  if(!IS_LORD(this_interactive())) return 0;
  if (QueryProp(P_LAST_LOST_XP)==-1)
  {
    return -1;
  }
  log_file("UNDIE",sprintf("%s %O -> %O\n",ctime(time())[4..15],
                             this_interactive(),this_object()));

  if ((x=QueryProp(P_DEADS)) > 0)
    x--;
  Set(P_DEADS, x);


/*
// ALTE METHODE!!! Nur zur Sicherheit noch hier...
  x=QueryProp(P_XP);
  if ((di=QueryProp(P_LEVEL))<20 || !IS_SEER(this_object()))
  {
    x=(x*3)/2;
  }
  else
  {
//  x=(x*(di-17))/(di-18); // ALT == SL
//  die() ->    lost_exp= (QueryProp(P_XP)/10000)*(30000/l);
    x=((x*100)/(10000-(30000/di)))*100;
    // Die Formel ist auf gut 5000 XP genau. wenn jemand da kleinlicher ist...
    // soll ers selber zurueckrechnen ;)  Quero
   }
  if(x > QueryProp(P_LOST_XP) )
    x = QueryProp(P_LOST_XP);
//  AddExp(x);
//  tell_object(find_player("querolin"),break_string("l:"+di+" x:"+x+".",78));

*/

  SetProp(P_XP, QueryProp(P_XP)+QueryProp(P_LAST_LOST_XP));
  SetProp(P_LAST_LOST_XP, -1);
  SetProp(P_LOST_XP, QueryProp(P_LOST_XP)-x);
  Set(P_GHOST,0);
  return 1;
}

void NotifyPlayerDeath(object vic,object killer,object lxp)
{if((killer==this_object())&&(killer!=vic))SetProp(P_PLAYERKILLS,({vic->query_real_name(),vic->QueryProp(P_RACE),vic->QueryProp(P_SECOND),killer->QueryProp(P_SECOND)}));}
