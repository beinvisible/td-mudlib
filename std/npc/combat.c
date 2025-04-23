// Tamedhon MUDlib
//
// std/npc/combat.c -- NPC combat/spells
//
// $Date: 2009/12/20 14:30:00 $
/* $Revision: 1.8 $
 * $Log: combat.c,v $
 *
 * Revision 1.8  2009/12/20 14:30:00  Grynnis
 * break_string in Spelltexten
 *
 * Revision 1.7  2003/01/15 11:46:04  Gralkor
 * P_AGGRESSIVE gilt nur fuer Spieler >15
 * AddDefaultSpell wird mit Schaden ausgegeben
 * DefaultSpells wurden mal debuggt
 * (wenn DS_DEAF aufgerufen wird, wurde man blind...)
 * Wurde auf Mapping umgestellt->BUG ;)
 *
 * Revision 1.6  1997/07/01 20:27:04  Feyaria
 * defaultspells
 *
 * Revision 1.5  1997/06/26 23:15:27  Woody
 * Bugfix: Autoattack NPCs griffen auch Mags an, P_NOAGGRESSIVE ging nicht
 *
 * Revision 1.4  1997/06/25 11:58:01  Woody
 * AutoAttack(), nun wesentlich flexibler (Wahrsch., Props...)
 *
 * Revision 1.3  1997/05/31 20:23:06  Feyaria
 * bug behoben .. Atack ohne enemy aufgerufen :)
 *
 * Revision 1.2  1997/05/30 19:03:59  Feyaria
 * spells jetzt im Attack statt im heartbeat
 *
 * Revision 1.1  1997/03/13 01:19:15  Woody
 * prepared for revision control
 *
*/

// neu ab 6.10.95 : Spieler mit gesetztem P_NOAGGRESSIVE
//                  werden nicht angegriffen
// neu ab 15.2.96 : AddSpell: Spell bei random(rate < gesamtrate) nicht
//                  bei random(kumulierte rate < gesamtrate)
//                  7. Parameter 'sp_info' wird, wenn gesetzt, an Defend als
//                  3. Parameter uebergeben.
//                  Hm.. die Ratenberechnung und das Drumrum ist irgendwie
//                  alles Quatsch
//                  Also mal was neues: in der Propertie P_NPC_SPELL_VECTOR
//                  wird die laufende Nummer (Index) des aktuellen Spells
//                  'rate'-mal als 'int' abgelegt. Also bei 3 Spells mit
//                  Raten 1:3:4 wird P_NPC_SPELL_VECTOR zu
//                  ({0,1,1,1,2,2,2,2}). Fuer den aktuellen Spell (in
//                  'heart_beat') wird dann zufaellig ein Eintrag
//                  ausgewaehlt und der zugehoerige Spell abgearbeitet.
// neu ab 28.2.96 : Der 6. Parameter bei AddSpell ist entweder ein Mapping,
//                  dann wird er 'Defend' als 3. Parameter uebergeben, oder
//                  aber eine Funktion (String), diese wird nach dem Spell
//                  mit dem aktuellen Schaden, dem Schadenstyp und dem
//                  Feindobjekt als Parameter aufgerufen.
// neu ab 12.3.96 : Parameterliste fuer AddSpell, Includes geaendert
//

inherit "std/living/combat";

#include <combat.h>
#include <npc/combat.h>
#include <language.h>
#include <properties.h>
#include <wizlevels.h>
#include <health.h>
#include <new_skills.h>

#define HB_CHECK 7

// Zur besseren Lesbarkeit:
#define DAMAGE   0
#define MSG_ENEMY 1
#define MSG_OTHER 2
#define DAM_TYPE  3
#define SP_INFO   4

static int heartbeat, beatcount;

void create()
{
  ::create();
  beatcount=1;
  heartbeat=1;
}

static void _set_max_hp(int i)
{
  Set(P_MAX_HP,i);
  SetProp(P_HP,i);
}

static void _set_max_sp(int i)
{
  Set(P_MAX_SP,i);
  SetProp(P_SP,i);
}

varargs int AddSpell(int rate, int damage, string TextForEnemy,
                                            string TextForOthers,
                                            mixed dam_type, mixed sp_info)
{
  mixed *spells;
  int   i, index, *rate_array;

  if (rate<=0 || damage<=0 || !stringp(TextForEnemy) || !stringp(TextForOthers))
    return 0;
  if (!dam_type)
    dam_type = DT_MAGIC;
  spells = Query(P_SPELLS);
  if (!pointerp(spells))
    spells = ({({damage, TextForEnemy, TextForOthers, dam_type, sp_info})});
  else
    spells += ({({damage, TextForEnemy, TextForOthers, dam_type, sp_info})});
  Set(P_SPELLS, spells);
  // Index-Array aufbauen: Jeden Index gemaess seiner Rate 'rate'-mal eintragen
  rate_array = Query(P_NPC_SPELL_VECTOR);
  if (!pointerp(rate_array)) rate_array = ({});  // Initialisieren
  // 'sizeof(spells)-1' ist der aktuelle Index (gibts kein 'fill_array'o.ae.??)
  index = sizeof(spells)-1;
  for (i=1;i<=rate;i++)
    rate_array += ({index});
  Set(P_NPC_SPELL_VECTOR, rate_array);
  return 1;
}

varargs void AddDefaultSpell(int rate, int damage,string type)
{
  switch (type)
  {
    case DS_POISON :
      AddSpell(rate,damage,
          capitalize(name(WER,1))+" vergiftet Dich.\n",
          capitalize(name(WER,1))+" vergiftet @WEN.\n",
          DT_POISON,"DefaultSpell_Poison",([SP_SHOW_DAMAGE:1]));
      break;
    case DS_FROG :
      AddSpell(rate,damage,
          capitalize(name(WER,1))+" verwandelt Dich in einen Frosch.\n",
          capitalize(name(WER,1))+" verwandelt @WEN in einen Frosch.\n",
          DT_WATER,"DefaultSpell_Frog",([SP_SHOW_DAMAGE:1]));
      break;
    case DS_BLIND :
      AddSpell(rate,damage,
          capitalize(name(WER,1))+" blendet Dich.\n",
          capitalize(name(WER,1))+" blendet @WEN.\n",
          DT_LIGHT,"DefaultSpell_Blind",([SP_SHOW_DAMAGE:1]));
      break;
    case DS_DEAF :
      AddSpell(rate,damage,
          capitalize(name(WER,1))+" lässt Dich ertauben.\n",
          capitalize(name(WER,1))+" lässt @WEN ertauben.\n",
          DT_SOUND,"DefaultSpell_Sound",([SP_SHOW_DAMAGE:1]));
      break;
    case DS_DISARM :
      AddSpell(rate,damage,
          capitalize(name(WER,1))+" entwaffnet Dich.\n",
          capitalize(name(WER,1))+" entwaffnet @WEN.\n",
          DT_MAGIC,"DefaultSpell_Disarm",([SP_SHOW_DAMAGE:1]));
      break;
    case DS_FEAR :
      AddSpell(rate,damage,
          capitalize(name(WER,1))+" erschreckt Dich.\n",
          capitalize(name(WER,1))+" erschreckt @WEN.\n",
          DT_TERROR,"DefaultSpell_Fear",([SP_SHOW_DAMAGE:1]));
      break;
    case DS_CURSE_WEAPON :
      AddSpell(rate,damage,
          capitalize(name(WER,1))+" verflucht Deine Waffe.\n",
          capitalize(name(WER,1))+" verflucht @WESSEN Waffe.\n",
          DT_UNHOLY,"DefaultSpell_CurseWeapon",([SP_SHOW_DAMAGE:1]));
      break;
    case DS_CURSE_ARMOUR :
      AddSpell(rate,damage,
          capitalize(name(WER,1))+" verflucht Deine Rüstung.\n",
          capitalize(name(WER,1))+" verflucht @WESSEN Rüstung.\n",
          DT_UNHOLY,"DefaultSpell_CurseArmour",([SP_SHOW_DAMAGE:1]));
      break;
    case DS_REDUCE_WC :
      AddSpell(rate,damage,
          capitalize(name(WER,1))+" zerstört Deine Waffe.\n",
          capitalize(name(WER,1))+" zerstört @WESSEN Waffe.\n",
          DT_UNHOLY,"DefaultSpell_ReduceWC",([SP_SHOW_DAMAGE:1]));
      break;
    case DS_REDUCE_AC :
      AddSpell(rate,damage,
          capitalize(name(WER,1))+" zerstört Deine Rüstung.\n",
          capitalize(name(WER,1))+" zerstört @WESSEN Rüstung.\n",
          DT_UNHOLY,"DefaultSpell_ReduceAC",([SP_SHOW_DAMAGE:1]));
      break;
  }
}

void DefaultSpell_Poison(object enemy, int damage, mixed dam_type)
{
  enemy->AddPoison(1);
}

void DefaultSpell_Frog(object enemy, int damage, mixed dam_type)
{
  enemy->SetProp(P_FROG, 1);
}

void DefaultSpell_Blind(object enemy, int damage, mixed dam_type)
{
  enemy->SetProp(P_BLIND, ([BLIND_TIME:10,
  			    BLIND_MSG:"Du wurdest geblendet.\n"]));
}

void DefaultSpell_Deaf(object enemy, int damage, mixed dam_type)
{
  enemy->SetProp(P_DEAF, ([DEAF_TIME:3000,
  			   DEAF_MSG:"%s kann Dich leider nicht hören.\n"]));
}

void DefaultSpell_Disarm(object enemy, int damage, mixed dam_type)
{
  object ob;
  if(ob=enemy->QueryProp(P_WEAPON))
    enemy->drop_obj(ob);
}

void DefaultSpell_Fear(object enemy, int damage, mixed dam_type)
{
  enemy->SetProp(P_WIMPY, enemy->QueryProp(P_HP));
}

void DefaultSpell_CurseWeapon(object enemy, int damage, mixed dam_type)
{
  object ob;
  if(ob=enemy->QueryProp(P_WEAPON))
    ob->SetProp(P_CURSED,
      ob->Name(WER,1)+" ist verflucht und lässt sich nicht mehr wegstecken!\n");
}

void DefaultSpell_CurseArmour(object enemy, int damage, mixed dam_type)
{
  object ob;
  object *armours;
  if( sizeof(armours=enemy->QueryProp(P_ARMOURS)) )
  {
    ob=armours[random(sizeof(armours))];
    ob->SetProp(P_CURSED,
      ob->Name(WER,1)+" ist verflucht und lässt sich nicht mehr ausziehen\n");
  }
}

void DefaultSpell_ReduceWC(object enemy, int damage, mixed dam_type)
{
  object ob;
  int wc;
  if(ob=enemy->QueryProp(P_WEAPON))
    wc=2*(ob->QueryProp(P_WC)+1)/3-1;
    ob->SetProp(P_WC,wc<0?0:wc);
}

void DefaultSpell_ReduceAC(object enemy, int damage, mixed dam_type)
{
  object ob;
  object *armours;
  int ac;
  if( sizeof(armours=enemy->QueryProp(P_ARMOURS)) )
  {
    ob=armours[random(sizeof(armours))];
    ac=2*(ob->QueryProp(P_AC)+1)/3-1;
    ob->SetProp(P_AC,ac<0?0:ac);
  }
}


int AutoAttack(object ob) {
  mixed m;

  if (!query_once_interactive(ob))
    return 0;
  if (mappingp(m=QueryProp(P_AGGRESSIVE))) {
    mixed *ind,x,z;
    float f;
    int i,n;

    ind=m_indices(m)-({0});n=0;f=0.0;
    for (i=sizeof(ind)-1;i>=0;i--) {
      x=ind[i];
      if ((z=m[x][ob->QueryProp(x)]) || (z=m[x][0])) {
        f=f+({float})z;
        n++;
      }
    }
    if (n)
      m=f/((float)n);
    else
      m=m[0];
  }
  if (((int)(100*(m+ob->QueryProp(P_AGGRESSIVE))))<=random(100))
    return 0;
  if (ob->QueryProp(P_NOAGGRESSIVE))
    return 0;
  if (IS_LEARNER(ob)
      && (ob->QueryProp(P_INVIS)
          || ob->QueryProp(P_WANTS_TO_LEARN)))
    return 0;
  if (ob->QueryProp(P_LEVEL)<15)
    return 0;
  return 1;
}

#if 0
TJ(string s)
{
  object o;
  if (o=find_player("debugger"))
    tell_object(o,sprintf("%O: %s\n",this_object(),s));
}
#else
#define TJ(x)
#endif

void heart_beat()
{
  int r,i;
  string akt_spell_mess;
  mixed env,*spells, sp_info;
  object enemy;

  beatcount--;
  if (!beatcount && !Query(P_HB))
  {
    if (!environment())
    {
      set_heart_beat(0);
      remove();
      return;
    }
    if (!QueryProp(P_POISON))
    {
      env=all_inventory(environment());
      i=sizeof(env)-1;
      while (i>=0 && !query_once_interactive(env[i]))
        i--;
      if (i<0)
      {
	 set_heart_beat(0);
	 heartbeat=0;
	 TJ("OFF\n");
	 beatcount=HB_CHECK;
	 Set(P_NPC_HB_OFF_COUNT,absolute_hb_count());
	 return;
      }
   }
 }
  ::heart_beat();
  enemy=QueryEnemy();
  if (QueryProp(P_AGGRESSIVE)
      && (!enemy || environment()!=environment(enemy))
      && !beatcount)
  {
    env=all_inventory(environment());
    beatcount=HB_CHECK;
    if (!sizeof(env))
      return;
    for (i=sizeof(env)-1;i>=0;i--)
      if (!AutoAttack(env[i]))
        env=env[0..i-1]+env[i+1..];
    if (!sizeof(env))
       return;
    i=random(sizeof(env));
    Kill(env[i]);
  }
  else if (!beatcount)
    beatcount=HB_CHECK;
}

void init()
{
  int hp, sp, alc, beat_off_num;
  TJ(sprintf("Goin', %O %O %O\n",this_object(),this_player(),this_interactive()));
  if (!heartbeat)
  {
    TJ("Continuin'");
    set_heart_beat(1);
    heartbeat=1;
    beat_off_num=-Query(P_NPC_HB_OFF_COUNT)+absolute_hb_count();
    if (beat_off_num>0) // Nicht ausgefuehrtes HEILEN nachholen
    {
      hp=QueryProp(P_HP);
      sp=QueryProp(P_SP);
      alc=QueryProp(P_ALCOHOL);
      TJ(sprintf("%O: HP=%d, SP=%d\n",this_object(),hp,sp));
      hp+=beat_off_num/HEAL_DELAY+alc/ALCOHOL_DELAY;
      sp+=beat_off_num/HEAL_DELAY+alc/ALCOHOL_DELAY;
      alc-=beat_off_num/ALCOHOL_DELAY;
      SetProp(P_HP,hp);
      SetProp(P_SP,sp);
      SetProp(P_ALCOHOL,alc);
      update_hunt_times(beat_off_num);
      Set(P_NPC_HB_OFF_COUNT,0);
      TJ(sprintf("%O: HP=%d, SP=%d\n",this_object(),hp,sp));
    }
  }

  if (AutoAttack(this_player()))
    Kill(this_player());
}

void Attack(object enemy)
{
  int r,i;
  string akt_spell_mess;
  mixed env,*spells, sp_info;

  ::Attack(enemy);

  if(time() > QueryProp(P_NEXT_SPELL_TIME))
  {
  spells=Query(P_SPELLS);
  if (!enemy
      || !pointerp(spells)
      || !sizeof(spells)
      || environment(enemy)!=environment()
      || QueryProp(P_DISABLE_ATTACK)
      || random(100)>Query(P_SPELLRATE))
    return;
  // Spell auswaehlen. Dazu zufaellig ein Element des Rate-Arrays
  // auswaehlen. Der Wert ist ein Index fuer das Spell-Array
  r = random(sizeof(Query(P_NPC_SPELL_VECTOR)));
  i = Query(P_NPC_SPELL_VECTOR)[r];
  tell_object(enemy, break_string(spells[i][MSG_ENEMY],78));
  if (stringp(akt_spell_mess=spells[i][MSG_OTHER]))
  {
     if (strstr(akt_spell_mess,"@WER",0))
	akt_spell_mess=
	   implode(efun::explode(akt_spell_mess,"@WER"),enemy->name(WER,1));
     if (strstr(akt_spell_mess,"@WESSEN",0))
	akt_spell_mess=
	   implode(efun::explode(akt_spell_mess,"@WESSEN"),
		   enemy->name(WESSEN,1));
     if (strstr(akt_spell_mess,"@WEM",0))
	akt_spell_mess=
	   implode(efun::explode(akt_spell_mess,"@WEM"),enemy->name(WEM,1));
     if (strstr(akt_spell_mess,"@WEN",0))
	akt_spell_mess=
	   implode(efun::explode(akt_spell_mess,"@WEN"),enemy->name(WEN,1));
     say(break_string(akt_spell_mess,78),({enemy, this_object()}));
  }
  if (!mappingp(sp_info = spells[i][SP_INFO]))
    sp_info = 1;
  enemy->Defend(r=random(spells[i][DAMAGE])+1,
                spells[i][DAM_TYPE],
                sp_info,
                this_object());

  if (stringp(sp_info = spells[i][SP_INFO]) &&
      function_exists(sp_info, this_object()))
     catch(call_other(this_object(),
                      sp_info,
                      enemy,
                      r, // Damage
                      spells[i][DAM_TYPE]));

 }
}

