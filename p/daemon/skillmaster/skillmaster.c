// Tamedhon MUDlib
//
// /p/daemon/skillmaster/skillmaster.c -- (Tamedhon) SkillMaster
//
//
// 06/13/2012 Serii  etwas ueberarbeitet und skills werden nun mit der zeit wieder verlernt
//
// $Date: 2006/07/04 23:20:00 $
/* $Revision: 0.01 $
 * $Log: skillmaster.c,v $
 * Revision 0.01  2006/07/04 23:20:00  Serii
 * 
 *
 */


#include <defines.h>
#include <properties.h>
#include <wizlevels.h>
#include <config.h>

#include "/p/daemon/channel.h"

#define NEED_PROTOTYPES

#include "/sys/player/extra_skills.h"
#include "/p/daemon/skillmaster/skillmaster.h"

#define SAVEFILE "/p/daemon/save/skillmaster"

static mapping Learn(object pl, string skillname_int, int learn, mapping es_map, mapping skill);
private int UnlearnSkill(object pl, string skillname_int, string ap);

mapping E_Skills = ([ES_ACTIVE:([]),ES_PASSIVE:([])]);
mapping VerbToSkill=([]);


static int secure()
{
  if (!previous_object()) return 0;
  if (geteuid(previous_object())==ROOTID) return 1;
  if (geteuid(previous_object()) != geteuid(this_interactive())) return 0;
  if (this_interactive() != this_player()) return 0;
  if (!IS_ARCH(this_interactive())) return 0;
  return 1;
}


void Save(){save_object(SAVEFILE);}
private void Restore(){restore_object(SAVEFILE);}
void create(){seteuid(getuid());Restore();call_other(VOICEMASTER,"RegisterChannel","Skills",this_object());}

static void ca_lmsg(object pl, string str)
{
  tell_object(pl,str);
}

#define S_NAME "skill_name"
#define S_MAX_ABIL "skill_max_abil"
#define S_LEARN_MAX "skill_learn_max"
#define S_LEARN_MIN "skill_learn_min"
#define S_DESC "skill_desc"
#define S_VERBS "skill_verbs"
#define S_OBJ "skill_obj"
#define S_FUN "skill_fun"
#define S_USE_DIFF "skill_use_diff"
#define S_LEARN_DIFF "skill_learn_diff"
#define S_AUTO_LEARN "skill_auto_learn"

//
// NewSkill() - fuegt einen neuen skill hinzu (nur E+)
//
// skillname: diesen namen sieht evtl der spieler!
// skillname_int: interner name des skills, sollte dann einen extra eintrag in extra_skills.h kriegen
// skilldesc: beschreibung des skills
// abs_max: abselutes maximum fuer den skill (sollte std maessig 10000 sein)
// skill_conf: ([
//                 0: "A"||"P",      // "A"==active skill, "P"==passive skill
//                 1: ({ verben }),  // wenn 0 == "A" dann ist dies eine liste von verben
//                 2: learn_max,     // --> *_Skills
//                 3: learn_min,     // --> *_Skills
//                 4: 1|0,	     // nur fuer 0=="P": 1==lernt skill automatisch beim ersten benutzen
//                 5:"/path/to/object", // object in dem die funktion zum skill ist ("A")
//                 6:"function_name"  // funktion die den skill ausfuehrt ("A")
//                 7: use_diff       // siehe beschreibung fuer active skills oben!
//                 8: learn_diff     // siehe beschreibung fuer active skills oben!
//              ])

int NewSkill(string skillname_int, string skillname, string skilldesc, int abs_max, mapping skill_conf)
{
  int lv;
  mapping vmap;
  if(!secure()) return -1; // das darfste nicht ;)
  if(!sizeof(skillname_int)) return -2; // ohne namen geht nix
  if(!mappingp(skill_conf)) return -3; // das brauchen wir!
  if(skill_conf[0]!="A"&&skill_conf[0]!="P") return -4; // eins von beiden musses sein!
  if(skill_conf[0]=="A"&&member(E_Skills[ES_ACTIVE],skillname_int)) return -5; // gibts schon
  if(skill_conf[0]=="P"&&member(E_Skills[ES_PASSIVE],skillname_int)) return -5;

  if(skill_conf[0]=="A")
  {
    vmap=([]);
    for(lv=sizeof(skill_conf[1])-1;lv>=0;lv--)
    {
      if(stringp(VerbToSkill[skill_conf[1][lv]]))
        return -6;
      else
        vmap[skill_conf[1][lv]]=skillname_int;
    }
    VerbToSkill+=vmap;
  }

  if(abs_max<=0||!abs_max)abs_max=10000;

  if(skill_conf[0]=="A")
  {
    E_Skills[ES_ACTIVE]+=([skillname_int:
                           ([S_NAME:skillname,
                             S_MAX_ABIL:abs_max,
                             S_LEARN_MAX:skill_conf[2],
                             S_LEARN_MIN:skill_conf[3],
                             S_DESC:skilldesc,
                             S_VERBS:skill_conf[1],
                             S_OBJ:skill_conf[5],
                             S_FUN:skill_conf[6],
                             S_USE_DIFF:skill_conf[7],
                             S_LEARN_DIFF:skill_conf[8]
                            ])
                          ]);
  }
  else
  {
    E_Skills[ES_PASSIVE]+=([skillname_int:
                            ([S_NAME:skillname,
                              S_MAX_ABIL:abs_max,
                              S_LEARN_MAX:skill_conf[2],
                              S_LEARN_MIN:skill_conf[3],
                              S_DESC:skilldesc,
                              S_AUTO_LEARN:skill_conf[4],
                              S_USE_DIFF:skill_conf[7],
                              S_LEARN_DIFF:skill_conf[8]
                             ])
                           ]);
  }

  Save();
  return 0;
}

int DeleteSkill(string skillname_int, string pa)
{
  int lv;
  if(!secure()) return -1; // das darfste nicht ;)
  if(!sizeof(skillname_int)) return -2; // ohne namen geht nix
  if(pa!="A"&&pa!="P") return -4; // eins von beiden musses sein!
  if(pa=="A"&&!member(E_Skills[ES_ACTIVE],skillname_int)) return -5; // gibts nicht
  if(pa=="P"&&!member(E_Skills[ES_PASSIVE],skillname_int)) return -5;

  if(pa=="A")
  {
    for(lv=sizeof(E_Skills[ES_ACTIVE][skillname_int][S_VERBS])-1;lv>=0;lv--)
      VerbToSkill=m_delete(VerbToSkill,E_Skills[ES_ACTIVE][skillname_int][S_VERBS][lv]);
    E_Skills[ES_ACTIVE]=m_delete(E_Skills[ES_ACTIVE],skillname_int);
  }
  else
    E_Skills[ES_PASSIVE]=m_delete(E_Skills[ES_PASSIVE],skillname_int);

  Save();
  return 0;
}

varargs int LearnActiveSkill(object pl, string skillname_int, int max_abil)
{
  mapping skills;
  string *skillnames;
  mixed esa;
  int lv;
  int col,dem;
  if(!member(E_Skills[ES_ACTIVE],skillname_int)||!objectp(pl))
    return -1;
  skills=pl->QueryProp(P_EXTRA_SKILLS);
  if(!mappingp(skills)) // remove? (done in base.c)
  {
    skills=([ES_ACTIVE:([]),ES_PASSIVE:([])]);
    pl->SetProp(P_EXTRA_SKILLS,skills);
    pl->Set(P_EXTRA_SKILLS, SAVE, F_MODE_AS);
    pl->SetProp(P_EXTRA_SKILLS_ACTIVEx2,({}));
    pl->Set(P_EXTRA_SKILLS_ACTIVEx2, SAVE, F_MODE_AS);
  }

  if(member(skills[ES_ACTIVE],skillname_int))
    return -2;

  if(max_abil>E_Skills[ES_ACTIVE][skillname_int][S_MAX_ABIL]||max_abil<=0)
    max_abil=E_Skills[ES_ACTIVE][skillname_int][S_MAX_ABIL];

  skills[ES_ACTIVE][skillname_int]=
    ([ES_ABILITY:0,ES_MAX_ABILITY:max_abil,ES_USED_LAST:time(),ES_LEARNED_LAST:time(),ES_NR_USED_SUCC:0,ES_NR_USED:0]);

  skillnames=E_Skills[ES_ACTIVE][skillname_int][S_VERBS];
  esa=({});
  for(lv=sizeof(skillnames)-1;lv>=0;lv--)
    esa+=({({skillnames[lv],"exec_skill_call",0,0})});

  esa=pl->QueryProp(P_EXTRA_SKILLS_ACTIVEx2)+(esa-(pl->QueryProp(P_EXTRA_SKILLS_ACTIVEx2)&esa));

  pl->SetProp(P_EXTRA_SKILLS,skills);
  pl->SetProp(P_EXTRA_SKILLS_ACTIVEx2,esa);

  dem=pl->QueryProp(P_RACE)=="Dämon";

  col=pl->QueryProp(P_TTY)=="ansi"?1:0;
  call_out("ca_lmsg",0,pl,break_color_string(
    parse_color_string(sprintf("%s eine neue Fähigkeit gelernt: %s%s%s!\n",dem?"Ihr habt":"Du hast",
	      col?"^W":"'",capitalize(E_Skills[ES_ACTIVE][skillname_int][S_NAME]),col?"^N":"'"))));

  return 0;
}

varargs int LearnPassiveSkill(object pl, string skillname_int, int max_abil)
{
  mapping skills;
  string *skillnames;
  int col,dem;
  if(!member(E_Skills[ES_PASSIVE],skillname_int)||!objectp(pl))
    return -1;
  skills=pl->QueryProp(P_EXTRA_SKILLS);
  if(!mappingp(skills)) // remove? (done in base.c)
  {
    skills=([ES_ACTIVE:([]),ES_PASSIVE:([])]);
    pl->SetProp(P_EXTRA_SKILLS,skills);
    pl->Set(P_EXTRA_SKILLS, SAVE, F_MODE_AS);
  }

  if(member(skills[ES_PASSIVE],skillname_int))
    return -2;

  if(max_abil>E_Skills[ES_PASSIVE][skillname_int][S_MAX_ABIL]||max_abil<=0)
    max_abil=E_Skills[ES_PASSIVE][skillname_int][S_MAX_ABIL];

  skills[ES_PASSIVE][skillname_int]=
    ([ES_ABILITY:0,ES_MAX_ABILITY:max_abil,ES_USED_LAST:time(),ES_LEARNED_LAST:time(),ES_NR_USED_SUCC:0,ES_NR_USED:0]);

  pl->SetProp(P_EXTRA_SKILLS,skills);

  dem=pl->QueryProp(P_RACE)=="Dämon";

  col=pl->QueryProp(P_TTY)=="ansi"?1:0;
  call_out("ca_lmsg",0,pl,break_color_string(
    parse_color_string(sprintf("%s eine neue Fähigkeit gelernt: %s%s%s!\n",dem?"Ihr habt":"Du hast",
	      col?"^W":"'",capitalize(E_Skills[ES_PASSIVE][skillname_int][S_NAME]),col?"^N":"'"))));

  return 0;
}

varargs int LearnExtraSkill(object pl, string skillname_int, int max_abil)
{
  if(member(E_Skills[ES_PASSIVE],skillname_int))
    return LearnPassiveSkill(pl,skillname_int,max_abil);
  else if(member(E_Skills[ES_ACTIVE],skillname_int))
    return LearnActiveSkill(pl,skillname_int,max_abil);
  return -5;
}

int UseActiveSkill(object pl, string skillverb, string args)
{
  mapping skill,es_map;
  int succ,abil,use_diff,learn_diff,nolearn;
  string skillname_int;
  mixed averbs;
  if(previous_object()!=pl)return 0;
  if(!sizeof(skillverb)||!objectp(pl)) return 0;

  skillname_int=VerbToSkill[skillverb];

  if(!member(E_Skills[ES_ACTIVE],skillname_int)||!stringp(skillname_int)) // den skill gibts nichtmehr?
  {
    if(stringp(skillname_int))
    {
      es_map=pl->QueryProp(P_EXTRA_SKILLS);
      es_map[ES_ACTIVE]=m_delete(es_map[ES_ACTIVE],skillname_int);
      pl->SetProp(P_EXTRA_SKILLS,es_map);
    }
    averbs=pl->QueryProp(P_EXTRA_SKILLS_ACTIVEx2);
    averbs-=({skillverb,"exec_skill_call",0,0});
    pl->SetProp(P_EXTRA_SKILLS_ACTIVEx2,averbs);
    return 0;
  }

  es_map=pl->QueryProp(P_EXTRA_SKILLS);

  if(!mappingp(es_map[ES_ACTIVE][skillname_int]))
  {
    return 0;
  }

  if(UnlearnSkill(pl,skillname_int,ES_ACTIVE)<0) // verlernen falls zulang nichmehr benutzt (falls man den reset verpasst hat)
    return 1; // abbrechen - wir koennen den skill ja nun nichmehr!

  use_diff=E_Skills[ES_ACTIVE][skillname_int][S_USE_DIFF];
  learn_diff=E_Skills[ES_ACTIVE][skillname_int][S_LEARN_DIFF];

  if(use_diff>10000) use_diff=10000;
  if(use_diff<-10000) use_diff=-10000;
  if(learn_diff>10000) learn_diff=10000;
  if(learn_diff<-10000) learn_diff=-10000;

  skill=es_map[ES_ACTIVE][skillname_int];
  skill[ES_USED_LAST]=time();
  skill[ES_NR_USED]++;

  abil=skill[ES_ABILITY];

  succ=-random(E_Skills[ES_ACTIVE][skillname_int][S_MAX_ABIL])-use_diff+abil+random(1000);

  // die skillfunktion wird aufgerufen!
  // function muss 0 zurueck geben damit der spieler die chance hat was zu lernen!
  // die fun wird auch aufgerufen wenn der skill fehlschlaegt! dort muss dann eine entsprechende meldung ausgegeben werden!
  nolearn=call_other(E_Skills[ES_ACTIVE][skillname_int][S_OBJ],E_Skills[ES_ACTIVE][skillname_int][S_FUN],pl,succ,skill,skillverb,args);

  // fuer skills < 4000 wird gelernt wenn der skill gut geht
  // fuer skills > 6000 wird gelernt wenn der skill schief geht
  // fuer skills zwischen und einschliesslich 4000 und 6000 wird in jedem fall gelernt
  if(!nolearn&&((abil<4000&&succ>=0)||(abil>6000&&succ<=0)||(abil>=4000&&abil<=6000))&&
     random((int)(1.5*E_Skills[ES_ACTIVE][skillname_int][S_MAX_ABIL])-learn_diff)>=random(10*abil))
  {
    skill=Learn(pl,skillname_int,
          E_Skills[ES_ACTIVE][skillname_int][S_LEARN_MIN]+random(E_Skills[ES_ACTIVE][skillname_int][S_LEARN_MAX]-
                                                                 E_Skills[ES_ACTIVE][skillname_int][S_LEARN_MIN]),
	  E_Skills[ES_ACTIVE],skill);
  }

  if(succ>=0&&!nolearn)
    skill[ES_NR_USED_SUCC]++;

  if(!nolearn)
  {
    es_map[ES_ACTIVE][skillname_int]=skill;
    pl->SetProp(P_EXTRA_SKILLS,es_map);
  }

  if(nolearn>=0) // nolearn>0 --- wenigstens das kommando war richtig, nolearn=0 alles gutgegangen! --- jedenfalls ist hier ende...
    return 1;

  return 0; // nolearn<0 --- wir lassen das kommando weiter rutschen!
}

//
// *_diff: -10000=easy ... 0=normal ... 10000=hard
//
//ret: + success
//     0 error (most likely: skill not learned yet)
//     - failed
varargs int UsePassiveSkill(object pl, string skillname_int, int use_diff, int learn_diff, int nolearn)
{
  mapping skill,es_map;
  int succ,abil;
  if(!sizeof(skillname_int)||!objectp(pl)) return 0;//-1;
  if(!member(E_Skills[ES_PASSIVE],skillname_int)) { return 0;}//-2;

  es_map=pl->QueryProp(P_EXTRA_SKILLS);
  if(!mappingp(es_map[skillname_int])&&!E_Skills[ES_PASSIVE][skillname_int][S_AUTO_LEARN])
  {
    return 0;//-3;
  }
  else
    LearnPassiveSkill(pl,skillname_int);

  if(UnlearnSkill(pl,skillname_int,ES_PASSIVE)<0) // verlernen falls zulang nichmehr benutzt (falls man den reset verpasst hat)
    return 0;

//  if(!use_diff) use_diff=0;
  if(!use_diff) use_diff=E_Skills[ES_PASSIVE][skillname_int][S_USE_DIFF];
//  if(!learn_diff) learn_diff=0;
  if(!learn_diff) learn_diff=E_Skills[ES_PASSIVE][skillname_int][S_LEARN_DIFF];
  if(use_diff>10000) use_diff=10000;
  if(use_diff<-10000) use_diff=-10000;
  if(learn_diff>10000) learn_diff=10000;
  if(learn_diff<-10000) learn_diff=-10000;

  skill=es_map[ES_PASSIVE][skillname_int];
  skill[ES_NR_USED]++;

  abil=skill[ES_ABILITY];

  succ=-random(E_Skills[ES_PASSIVE][skillname_int][S_MAX_ABIL])-use_diff+abil+random(1000);

  if(!nolearn&&((abil<4000&&succ>=0)||(abil>6000&&succ<=0)||(abil>=4000&&abil<=6000))&&
     random((int)(1.5*E_Skills[ES_PASSIVE][skillname_int][S_MAX_ABIL])-learn_diff)>=random(10*abil))
  {
    skill=Learn(pl,skillname_int,
          E_Skills[ES_PASSIVE][skillname_int][S_LEARN_MIN]+random(E_Skills[ES_PASSIVE][skillname_int][S_LEARN_MAX]-
                                                                 E_Skills[ES_PASSIVE][skillname_int][S_LEARN_MIN]),
	  E_Skills[ES_PASSIVE],skill);
  }

  skill[ES_USED_LAST]=time();
  if(succ>=0)
    skill[ES_NR_USED_SUCC]++;

  es_map[ES_PASSIVE][skillname_int]=skill;
  pl->SetProp(P_EXTRA_SKILLS,es_map);

  if(succ==0)succ=-1;

  return succ;
}

static mapping Learn(object pl, string skillname_int, int learn, mapping es_map, mapping skill)
{
  int abil,old_abil;
  int col,dem;

  col=pl->QueryProp(P_TTY)=="ansi"?1:0;
  old_abil=abil=skill[ES_ABILITY];
  abil+=learn;
  if(abil>skill[ES_MAX_ABILITY])
    abil=skill[ES_MAX_ABILITY];
  else if(abil<0)
    abil=0;

  dem=pl->QueryProp(P_RACE)=="Dämon";

  if(abil>old_abil)
  {
    if(abil==skill[ES_MAX_ABILITY])
    {
      call_out("ca_lmsg",0,pl,break_color_string(
        parse_color_string(sprintf(
	    "%s Fähigkeit %s%s%s nicht mehr verbessern zu können!\n",dem?"Ihr scheint Eure":"Du scheinst Deine",
	      col?"^W":"'",capitalize(es_map[skillname_int][S_NAME]),col?"^N":"'"))));
    }
    else
    {
      call_out("ca_lmsg",0,pl,break_color_string(
        parse_color_string(sprintf(
	    "%s Fähigkeit %s%s%s etwas verbessert!\n",dem?"Ihr habt Eure":"Du hast Deine",
	      col?"^W":"'",capitalize(es_map[skillname_int][S_NAME]),col?"^N":"'"))));
    }
  }
  else if(abil<old_abil)
  {
    call_out("ca_lmsg",0,pl,break_color_string(
      parse_color_string(sprintf(
          "%s Fähigkeit %s%s%s hat sich etwas verschlechtert!\n",dem?"Eure":"Deine",
            col?"^W":"'",capitalize(es_map[skillname_int][S_NAME]),col?"^N":"'"))));
  }

  skill[ES_ABILITY]=abil;
  skill[ES_LEARNED_LAST]=time();

  return skill;
}

// ret: -1: no skillname / no player
//      -2: invalid skill
//      -3: player hasn't learned skill yet
//      0+: %skill ability
float QuerySkillAbility(object pl,string skillname_int)
{
  mapping es_map;
  string activepassive;
  if(!sizeof(skillname_int)||!objectp(pl)) return -1.0;

  activepassive=ES_PASSIVE;
  if(!member(E_Skills[activepassive],skillname_int))
  {
    activepassive=ES_ACTIVE;
    if(!member(E_Skills[activepassive],skillname_int))
      return -2.0;
  }

  es_map=pl->QueryProp(P_EXTRA_SKILLS);

  if(!mappingp(es_map[activepassive][skillname_int]))
  {
    return -3.0;
  }
  return es_map[activepassive][skillname_int][ES_ABILITY]*100.0/
         es_map[activepassive][skillname_int][ES_MAX_ABILITY];
}

void RemoveSkill(object pl, string skillname_int)
{
  string activepassive;
  mapping es_map;
  string *averbs;
  int siz,lv;

  activepassive=ES_PASSIVE;
  if(!member(E_Skills[activepassive],skillname_int))
  {
    activepassive=ES_ACTIVE;
    if(!member(E_Skills[activepassive],skillname_int))
      return;
  }

  es_map=pl->QueryProp(P_EXTRA_SKILLS);

  if(activepassive==ES_ACTIVE)
  {
    siz=sizeof(E_Skills[ES_ACTIVE][skillname_int][S_VERBS]);
    averbs=pl->QueryProp(P_EXTRA_SKILLS_ACTIVEx2);
    for(lv=0;lv<siz;lv++)
    {
      averbs-=({E_Skills[ES_ACTIVE][skillname_int][S_VERBS][lv],"exec_skill_call",0,0});
    }
    pl->SetProp(P_EXTRA_SKILLS_ACTIVEx2,averbs);
  }

  es_map[activepassive]=m_delete(es_map[activepassive],skillname_int);
  pl->SetProp(P_EXTRA_SKILLS,es_map);
}

#define UNLEARN_TIME		86400
#define UNLEARN_RATE_MULT	(1+random(15))

private int UnlearnSkill(object pl, string skillname_int, string ap)
{
  mapping skill,es_map;
  int nutime;
  int n_abil;
  int col,dem;

  es_map=pl->QueryProp(P_EXTRA_SKILLS);
  skill=es_map[ap][skillname_int];
  nutime=time()-skill[ES_USED_LAST];

  if(nutime>=UNLEARN_TIME)
  {
    col=pl->QueryProp(P_TTY)=="ansi"?1:0;
    dem=pl->QueryProp(P_RACE)=="Dämon";

    n_abil=skill[ES_ABILITY]-(nutime/UNLEARN_TIME)*(UNLEARN_RATE_MULT*E_Skills[ap][skillname_int][S_LEARN_MIN]);

    if(n_abil<=0)
    {
      RemoveSkill(pl,skillname_int);
      tell_object(pl,break_color_string(
      parse_color_string(sprintf(
          "%s Fähigkeit %s%s%s komplett verlernt!\n",dem?"Ihr habt Eure":"Du hast Deine",
            col?"^W":"'",capitalize(E_Skills[ap][skillname_int][S_NAME]),col?"^N":"'"))));
      return -1;
    }
    else
    {
      skill[ES_ABILITY]=n_abil;
      skill[ES_USED_LAST]=time();
      es_map[ap][skillname_int]=skill;
      pl->SetProp(P_EXTRA_SKILLS,es_map);
      tell_object(pl,break_color_string(
      parse_color_string(sprintf(
          "%s Fähigkeit %s%s%s etwas verlernt!\n",dem?"Ihr habt Eure":"Du hast Deine",
            col?"^W":"'",capitalize(E_Skills[ap][skillname_int][S_NAME]),col?"^N":"'"))));
      return 1;
    }
  }
  return 0;
}

void UnlearnSkills(object pl)
{
  int lv;
  string *inds;
  mixed tmp;
  tmp=pl->QueryProp(P_EXTRA_SKILLS);
  if(!pointerp(tmp))return;
  tmp=tmp[ES_PASSIVE];
  inds=m_indices(tmp);
  for(lv=0;lv<sizeof(inds);lv++)
    UnlearnSkill(pl,inds[lv],ES_PASSIVE);

  inds=m_indices(pl->QueryProp(P_EXTRA_SKILLS)[ES_ACTIVE]);
  for(lv=0;lv<sizeof(inds);lv++)
    UnlearnSkill(pl,inds[lv],ES_ACTIVE);
}

void All_UnlearnSkills()
{
  object *plist;
  int lv;
//  plist=filter(users(),lambda(({'u}),({#'<,({#'query_wiz_level,'u}),LEARNER_LVL})));
plist=users();
  for(lv=0;lv<sizeof(plist);lv++)
  {
    UnlearnSkills(plist[lv]);
  }
}

string Name(){return ME->query_real_name();}
string name(){return ME->Name();}
string query_real_name(){return sprintf("^W^G{%sSM}^N",MUDNAME[0..0]);}
int remove(){Save();call_other(VOICEMASTER,"RemoveChannel","Skills",this_object());destruct(this_object());return 1;}
void reset(){Save();All_UnlearnSkills();}
int query_prevent_shadow(){return 1;}

void DS()
{
}
