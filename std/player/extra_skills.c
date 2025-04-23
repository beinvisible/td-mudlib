// Tamedhon MUDlib
//
// std/player/extra_skills.c -- skills modul
//
// $Date: 2006/07/08 14:17:00 $
/* $Revision: 0.1 $
 * $Log: extra_skills.c,v $
 * Revision 0.1  2006/07/08 14:17:00  Serii
 *
 * 05/20/2012 Serii  UseExtraSkill eingebaut
 *
 */

#pragma strong_types
#pragma save_types

#define NEED_PROTOTYPES

#include "/sys/player/extra_skills.h"
#include "/p/daemon/skillmaster/skillmaster.h"
#include <thing/properties.h>
//#include <wizlevels.h>
//#include <defines.h>
#include <properties.h>
//#include <daemon.h>
//#include "/std/sys_debug.h"

void create()
{
  Set(P_EXTRA_SKILLS, SAVE, F_MODE);
  Set(P_EXTRA_SKILLS, ([ES_ACTIVE:([]),ES_PASSIVE:([])]));

  Set(P_EXTRA_SKILLS_ACTIVEx2, SAVE, F_MODE);
  Set(P_EXTRA_SKILLS_ACTIVEx2, ({}));
}

string *_set_extra_skills_activex2(string *list)
{
//  DB(previous_object());
  return Set(P_EXTRA_SKILLS_ACTIVEx2,list);
}

mapping _set_extra_skills(mapping es)
{
//  DB(previous_object());
  return Set(P_EXTRA_SKILLS,es);
}

varargs static int exec_skill_call(string args)
//varargs int exec_skill_call(string args)
{
  return SKILLMASTER->UseActiveSkill(this_player(),query_verb(),args);
}

varargs int UseExtraSkill(string skillname_int, int use_diff, int learn_diff, int nolearn)
{
  if(!sizeof(skillname_int))
  {
    return 0;
  }
//  if(!use_diff)use_diff=0;
//  if(!learn_diff)learn_diff=0;
//  if(!nolearn)nolearn=0;
  return SKILLMASTER->UsePassiveSkill(this_object(),skillname_int, use_diff, learn_diff, nolearn);
}

varargs float QueryExtraSkillAbility(string skillname_int)
{
  return SKILLMASTER->QuerySkillAbility(this_object(),skillname_int);
}

varargs int LearnExtraSkill(string skillname_int, int max_abil)
{
  return SKILLMASTER->LearnExtraSkill(this_object(),skillname_int,max_abil);
}

static string *_query_localcmds()
{
  string *cmds;
  int lv;

  cmds=QueryProp(P_EXTRA_SKILLS_ACTIVEx2);
  if(!pointerp(cmds))
  {
    Set(P_EXTRA_SKILLS, SAVE, F_MODE);
    Set(P_EXTRA_SKILLS, ([ES_ACTIVE:([]),ES_PASSIVE:([])]));
    Set(P_EXTRA_SKILLS_ACTIVEx2, SAVE, F_MODE);
    Set(P_EXTRA_SKILLS_ACTIVEx2, ({}));
    return ({});
  }

/*                       // P_EXTRA_SKILLS_ACTIVEx2 should already be in this format!
  cmds=({});
  for(lv=sizeof(skills)-1;lv>=0;lv--)
  {
    cmds+=({({skills[lv],"exec_skill_call",0,0})});
  }
*/

  return cmds;
}

