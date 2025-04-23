// Tamedhon MUDlib
//
// std/spellbook.c -- base spellbook object
//
// $Date: 2008/07/04 10:00:00  TamTam $
/* $Revision: 2.9 $
 * $Log: spellbook.c,v $
 *
 * Revision 2.9  2008/07/04 10:00:00  TamTam
 * NPC Angriffe sind 'legal' ->ATTACKS log angepasst
 *
 * Revision 2.8  2007/12/02 22:00:00  Gralkor
 * Spell-Counter (von TamTam)
 *
 * Revision 2.7  2004/03/30 22:00:00  Serii
 * pk-bugfix
 *
 * Revision 2.6  2004/02/02 19:00:00  Serii
 * Legale PK-attacken werden nun nichtmehr in /log/ATTACKS gelogt
 *
 * Revision 2.5  1997/11/05 16:08:05  Woody
 * SI_PREPARE_MSG, kein Zaubern waehrend des Rastens
 *
 * Revision 2.4  1997/10/06 12:23:58  Woody
 * Spruchvorbereitung (P_PREPARED_SPELL, SI_PREPARE_TIME)
 *
 * Revision 2.3  1997/09/17 14:19:12  Woody
 * P_NOATTACK auch hier schon abfragen und Attacke abfangen -> kein Ueben mehr
 *
 * Revision 2.2  1997/06/25 11:34:02  Woody
 * Try(Default)AttackSpell() gibt Schaden zurueck,
 * SI_SP_LOW_MSG, SI_TIME_MSG, SI_NOMAGIC
 *
 * Revision 2.1  1997/05/27 09:44:05  Feyaria
 * Learn abhaengig von SI_LEARN_ATTRIBUTE[D[D[D[D
 *
 * Revision 2.0  1997/03/13 03:13:55  Woody
 * prepared for revision control
 *
 * Revision 1.6  1996/10/19  Woody
 * Bugfix in Learn()
 *
 * Revision 1.5  1995/08/10  11:06:01  Rochus
 * Bugfix in FindGroup
 *
 * Revision 1.4  1995/07/25  12:51:05  Rochus
 * NEXT_SPELL_TIME wird gesaved.
 *
 * Revision 1.3  1995/04/21  16:03:39  Jof
 * seteuid(getuid())
 *
 * Revision 1.2  1995/03/28  08:31:19  Boing
 * Typo ausgemerzt
 *
 * Revision 1.1  1995/02/06  19:39:22  Jof
 * Initial revision
 *
 */

#define NEED_PROTOTYPES
inherit "/std/thing";
inherit "/std/restriction_checker";
#include <thing/properties.h>
#include <properties.h>
#include <wizlevels.h>
#include <new_skills.h>
#include <moving.h>
#include "/p/daemon/pkmaster/pkmaster.h"
#pragma strong_types

void create() {
  seteuid(getuid());
  ::create();
}

mapping QuerySpell(string spell) {
  mapping spi;

  if (!spell
      || !(spi=QueryProp(P_SB_SPELLS))
      || !(spi=spi[spell]))
    return 0;
  return copy(spi);
}

varargs int
AddSpell(string verb, int kosten, mixed ski) {
  int level;
  mapping spells;

  if (!verb || kosten<0)
    return 0;
  verb=lower_case(verb);
  level=(intp(ski))?ski:0;
  if (!mappingp(ski)) ski=([]);
  ski=AddSkillMappings(QueryProp(P_GLOBAL_SKILLPROPS),ski);
  ski+=([SI_SPELLCOST:kosten]);
  if (level)
    ski=AddSkillMappings(ski,([SI_SKILLRESTR_LEARN:([P_LEVEL:level])]));
  if (!(spells=QueryProp(P_SB_SPELLS)))
    spells=([]);
  SetProp(P_SB_SPELLS,spells+([verb:ski]));
  return 1;
}

int
TryAttackSpell(object victim, int damage, mixed dtypes,
               mixed is_spell, object caster, mapping sinfo) {
  mixed noattack;

  if (noattack = victim->QueryProp(P_NOATTACK)) {
    if (stringp(noattack))
      write(victim->Name(WER,1)+" "+noattack+"\n");
    else
      write(victim->Name(WER,1)+" springt geschickt aus Deiner Reichweite.\n");
    return 0;
  }

  damage=(damage*caster->QuerySkillAttribute(SA_DAMAGE))/100;

  if (query_once_interactive(victim) && query_once_interactive(caster)
                                     && !(IS_LEARNER(caster)
                                          && IS_LEARNER(victim))
				     && !(call_other(PKMASTER,"CheckForPkEnvironment",
				                               caster,victim)||
				          (call_other(PKMASTER,"CheckForHitList",
					                      caster,victim)==2)))
  {
    printf("Du kannst hier keine Spieler angreifen!\n");
    return 0;
  };

  if((sinfo[SI_NOMAGIC] < victim->QueryProp(P_NOMAGIC)) &&
      victim->QueryProp(P_NOMAGIC)*100 >
      random(100)*caster->QuerySkillAttribute(SA_ENEMY_SAVE)) {
    printf("%s wehrt Deinen Zauber ab.\n", victim->Name(WER, 1));
    return 0;
  }
  else {
    victim->Defend(damage, dtypes, is_spell, caster);
    return damage;
  }
}

varargs int
TryDefaultAttackSpell(object victim, object caster, mapping sinfo, mixed str) {
  if (!str) str=1;
  return TryAttackSpell(victim,
                        (GetOffset(SI_SKILLDAMAGE,sinfo,caster)+
                         random(GetFValue(SI_SKILLDAMAGE,sinfo,caster))),
                        GetData(SI_SKILLDAMAGE_TYPE,sinfo,caster),
                        str,
                        caster,
                        sinfo);
}

#define SMUL(x,y) ((x<0 && y<0)?(-1*x*y):(x*y))
int
SpellSuccess(object caster, mapping sinfo) {
  int cval,abil,res;

  abil=sinfo[SI_SKILLABILITY];
  cval=caster->UseSkill(SK_CASTING);
  res=abil + (SMUL(abil,cval)) / MAX_ABILITY - random(MAX_ABILITY);
  if (cval && res>MAX_ABILITY) // besonders gut gelungen?
    caster->LearnSkill(SK_CASTING,1+(res-MAX_ABILITY)/2000);
  return res;
}

int
CanTrySpell(object caster, mapping sinfo) {
  mapping rmap;
  string res;

  if (caster->QueryProp(P_RESTING)) {
    write("Während Du rastest, kannst Du nicht zaubern.\n");
    return 0;
  }
  if (caster->QueryProp(P_GHOST)) {
    write("Als Geist kannst Du nicht zaubern.\n");
    return 0;
  }
  if ((rmap=sinfo[SI_SKILLRESTR_USE])
      && (res=check_restrictions(caster,rmap))) {
    write(res);
    return 0;
  }
  return 1;
}

void
Learn(object caster, string spell, mapping sinfo) {
  int val,diff;
  string attrib;
  if(! attrib=sinfo[SI_SKILLLEARN_ATTRIBUTE])
    attrib=A_INT;
  val=((caster->QueryAttribute(attrib)/2)*GetFValue(SI_SKILLLEARN,sinfo,caster)+
       GetOffset(SI_SKILLLEARN,sinfo,caster));
  if (!(diff=GetFValueO(SI_DIFFICULTY,sinfo,caster)))
    diff=GetFValueO(SI_SPELLCOST,sinfo,caster);
  caster->LearnSkill(spell,val,diff);
}

void
Erfolg(object caster, string spell, mapping sinfo) {
  object env;
  int new_count;

  new_count=++sinfo[SI_USR];
  caster->ModifySkill(spell,([SI_USR:new_count]));
  if (env=environment(caster))
    env->SpellInform(caster,spell,sinfo);
}

void
Misserfolg(object caster, string spell, mapping sinfo)
{
    int		old_abil;
    string	msg;

    if(stringp(sinfo[SI_FAIL_MSG]))
    {
	write(sinfo[SI_FAIL_MSG]);
    }
    else
    {
	write("Der Zauberspruch ist Dir missglückt.\n");
    }

    old_abil = sinfo[SI_SKILLABILITY];
    Learn(caster, spell, sinfo);
    sinfo=caster->QuerySkill(spell);
    if (old_abil < (mappingp(sinfo) ? sinfo[SI_SKILLABILITY] : sinfo))
    {
	msg = "Du lernst aus Deinem Fehler.\n";
	if(stringp(sinfo[SI_LEARN_MSG]))
	{ msg=sinfo[SI_LEARN_MSG]; }
	else if(stringp(QueryProp(P_SB_SPELLS)[spell][SI_LEARN_MSG]))
	{ msg=QueryProp(P_SB_SPELLS)[spell][SI_LEARN_MSG]; }
	else if(stringp(QueryProp(P_GLOBAL_SKILLPROPS)[SI_LEARN_MSG]))
	{ msg=QueryProp(P_GLOBAL_SKILLPROPS)[SI_LEARN_MSG]; }

	write(msg);
    }
    else
    {
	msg = "Du lernst leider nichts aus Deinem Fehler.\n";
	if(stringp(sinfo[SI_FAILLEARN_MSG]))
	{ msg=sinfo[SI_FAILLEARN_MSG]; }
	else if(stringp(QueryProp(P_SB_SPELLS)[spell][SI_FAILLEARN_MSG]))
	{ msg=QueryProp(P_SB_SPELLS)[spell][SI_FAILLEARN_MSG]; }
	else if(stringp(QueryProp(P_GLOBAL_SKILLPROPS)[SI_FAILLEARN_MSG]))
	{ msg=QueryProp(P_GLOBAL_SKILLPROPS)[SI_FAILLEARN_MSG]; }

	write(msg);
    }

}

string
SelectSpell(string spell, mapping sinfo) {
  if (sinfo && sinfo[SI_SKILLFUNC])
    return sinfo[SI_SKILLFUNC];
  return spell;
}

void
prepare_spell(object caster, string spell, string msg) {
  if (!caster || !spell) return;
  tell_object(caster,
      msg || "Du bereitest Dich auf den Spruch ``"+spell+"'' vor.\n");
}

varargs int
UseSpell(object caster, string spell, mapping sinfo) {
  mapping ski;
  string spellbook,sname,fname,txt;
  int res,fat,cost;
  mixed ps;

  if (!caster || !spell)
    return 0;
  sname=SelectSpell(spell,sinfo);
  // Spell kann in der Gilde anderen Namen haben
  if (!(ski=QuerySpell(sname))) // Existiert dieser Spell?
    return 0;
  ski=AddSkillMappings(ski,sinfo);
  // Gildeneigenschaften sollen Spelleigenschaften ueberschreiben koennen
  ski=race_modifier(caster,ski);
  // Fuer verschiedene Rassen unterschiedliche Eigenschaften

  // printf("Spellinfo: %O\n",ski);

  if (!CanTrySpell(caster, ski))
    return 1;
  if (caster->QueryProp(P_SP) < (cost=GetFValueO(SI_SPELLCOST,ski,caster))) {
    if (txt=ski[SI_SP_LOW_MSG]) write(txt);
    else write("Du hast zu wenig Zauberpunkte für diesen Spruch.\n");
    return 1;
  }
  // printf("cost: %d\n",cost);
  if (time()<caster->QueryProp(P_NEXT_SPELL_TIME)) {
    if (txt=ski[SI_TIME_MSG]) write(txt);
    else write("Du bist noch zu erschöpft von Deinem letzten Spruch.\n");
    return 1;
  }

  // Spruchvorbereitung
  if (pointerp(ps=caster->QueryProp(P_PREPARED_SPELL)) // Ausstehender Spruch
      && (sizeof(ps)>=3) && intp(ps[0]) && stringp(ps[1])) {
    if (ps[1]==spell) { // Dieser Spruch wird noch vorbereitet
      if (time()<ps[0]) {
        write("Du bist noch mit der Spruchvorbereitung beschäftigt.\n");
        return 1;
      }
    } else {    // Andere Sprueche brechen die Vorbereitung ab
      write("Du brichst die Spruchvorbereitung für ``"+ps[1]+"'' ab.\n");
      ps=0;
    }
  }
  else ps=0;

  if ((fat=ski[SI_PREPARE_TIME]) && !ps)  // Spruch braucht Vorbereitungszeit
  {
    caster->SetProp(P_PREPARED_SPELL,({time()+fat,spell,ski[SI_SKILLARG]}));
    prepare_spell(caster,spell,ski[SI_PREPARE_MSG]);
    return 1;
  }
  else if (!ps)
    caster->SetProp(P_PREPARED_SPELL, 0);

  if (!(fname=sinfo[SI_SKILLFUNC]))
    fname=sname;
  // Funktion kann anderen Namen haben als Spell
  res=0;
  if((ski[SI_NOMAGIC] < environment(caster)->QueryProp(P_NOMAGIC)) &&
      random(100) < environment(caster)->QueryProp(P_NOMAGIC)) {
    write("Dein Zauberspruch verpufft im Nichts.\n");
    res=ERFOLG;
  } else {
    res=call_other(this_object(),fname,caster,ski);
  }
  if (!res)
    return 1;

  if ((fat=GetFValueO(SI_SPELLFATIGUE,ski,caster))<0)
    fat=1;
  caster->restore_spell_points(-1*cost);
  caster->SetProp(P_NEXT_SPELL_TIME,time()+fat);
  caster->Set(P_NEXT_SPELL_TIME,SAVE,F_MODE_AS);

  if (res>0)
    Erfolg(caster,spell,ski);
  else
    Misserfolg(caster,spell,ski);
  return 1;
}

static varargs object
find_victim(string wen, object pl) {
  object victim;

  if (!pl) return 0;
  if (!sizeof(wen)) {
    if (victim = pl->QueryEnemy())
      return victim;
    else
      return 0;
  }
  if (victim = present(wen, environment(pl)))
    return victim;
  else if (victim = present(wen, pl))
    return victim;
  else
    return 0;
}
varargs object
FindVictim(string wen, object pl, string msg) {
  object vic;

  if (!(vic=find_victim(wen,pl)) && msg)
    write(msg);
  return vic;
}
varargs object
FindLivingVictim(string wen, object pl, string msg) {
  object vic;

  if (!(vic=FindVictim(wen,pl,msg)))
    return 0;
  if (!living(vic)) {
    printf("%s lebt doch nicht!\n", capitalize(vic->name()));
    return 0;
  }
  return vic;
}
varargs object
FindEnemyVictim(string wen, object pl, string msg) {
  object vic;
  mixed noattack;

  if (!(vic=FindLivingVictim(wen,pl,msg))) {
    if ((stringp(wen) && wen!="") || !objectp(pl))
      return 0;
    if (!(vic=pl->SelectEnemy()))
      return 0;
  }
  if (noattack = vic->QueryProp(P_NOATTACK)) {
    if (stringp(noattack))
      write(vic->Name(WER,1)+" "+noattack+"\n");
    else
      write(vic->Name(WER,1)+" springt geschickt aus Deiner Reichweite.\n");
    return 0;
  }
  if (vic==pl) {
    write("Du könntest Dir dabei wehtun.\n");
    return 0;
  }
  if (!pl->IsEnemy(vic)) // War es bisher kein Feind?
    pl->Kill(vic);       // Dann ist es jetzt einer.
  return vic;
}

object *
FindGroup(object pl, int who) {
  object *res,ob,env;
  int p1,p2;

  res=({});
  if (!pl || !(env=environment(pl))) return res;
  p1=((query_once_interactive(pl)?1:-1));
  for (ob=first_inventory(env);ob;ob=next_inventory(ob)) {
    if (!living(ob)) continue;
    p2=(((query_once_interactive(ob)||ob->QueryProp(P_FRIEND))?1:-1));
    if (p2>0 && !interactive(ob) && query_once_interactive(ob))
      continue; // keine Netztoten.
    if (IS_LEARNING(ob) &&
        (ob->QueryProp(P_INVIS) || (who<0 && !pl->IsEnemy(ob))))
      continue;
    if (who<0 && ob->QueryProp(P_NO_GLOBAL_ATTACK))
      continue;
    if (p1*p2*who >=0)
      res+=({ob});
  }
  return res;
}

object *
FindGroupN(object pl, int who, int n) {
  if (!pl) return ({});
  n=(n*pl->QuerySkillAttribute(SA_EXTENSION))/100;
  if (n<1) n=1;n--;
  return FindGroup(pl,who)[0..n];
}

object *
FindGroupP(object pl, int who, int pr) {
  object *res,*nres;
  int i;

  nres=({});
  if (!pl) return nres;
  pr=(pr*pl->QuerySkillAttribute(SA_EXTENSION))/100;
  res=FindGroup(pl,who);
  for (i=sizeof(res)-1;i>=0;i--)
    if (pr>=random(100))
      nres+=({res[i]});
  return nres;
}
