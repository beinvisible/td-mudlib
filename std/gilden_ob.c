// Tamedhon MUDlib
//
// std/gilden_ob.c -- Basisfunktionen einer Gilde
//
// $Date: 2005/07/10 11:35:00 $
/* $Revision: 2.55 $
 * $Log: gilden_ob.c,v $
 * Revision 2.6  2005/07/10 11:35:00  Querolin
 * P_LAST_ADVANCE_GUILD eingefuegt
 *
 * Revision 2.55  2004/2/16 20:33:00  Serii
 * klitzekleine anpassung an austreten()
 *
 * Revision 2.5  2003/12/28 10:12:58  Gralkor
 * Infomeldung fuer Gildenstufenaufstieg
 * Ausgabe auch an Spieler
 *
 * Revision 2.4  1997/10/20 10:03:33  Feyaria
 * SI_SKILLLEARN_ATTRIBUTE in InitalSkillAbility
 *
 * Revision 2.3  1997/10/13 18:09:05  Woody
 * MaxGuildRating() eingefuehrt, gibt 10000 zurueck, fuer Gilden
 * mit anderen Maxima: ueberschreiben!
 *
 * Revision 2.2  1997/08/09 15:12:01  Crea
 * InitialSkillAbaility auf nicht-static ausgebessert, zum Ueberschreiben.
 *
 * Revision 2.1  1997/06/25 11:18:38  Woody
 * NotifyGiveQuest() definiert
 *
 * Revision 2.0  1997/03/13 02:55:26  Woody
 * prepared for revision control
 *
 * Revision 1.9  1996/09/30  Woody
 * GuildRating() eingebaut,
 * copy() in QuerySkill() statt direkter Rueckgabe.
 *
 * Revision 1.8  1995/07/28  10:47:46  Rochus
 * return-wert bei nicht erfolgreichem eintritt <0
 *
 * Revision 1.7  1995/07/24  11:02:30  Wargon
 * austreten() varargs gemacht; gibt Verluste an GILDENMASTER weiter.
 *
 * Revision 1.6  1995/06/06  08:42:44  Jof
 * _notify_fail
 *
 * Revision 1.6  1995/06/05  14:56:00  Jof
 * _notify_fail used
 *
 * Revision 1.5  1995/04/13  15:35:57  Rochus
 * Bugfix
 *
 * Revision 1.4  1995/02/06  19:40:05  Jof
 * Updated RCS Logs
 *
*/


// #define NEED_PROTOTYPES
inherit "/std/restriction_checker";
#include <properties.h>
#include <attributes.h>
#include <new_skills.h>
#include <daemon.h>
#include "/p/daemon/channel.h"

#pragma strong_types

/*
string GuildName() {
  return explode(object_name(this_object()),"#")[0][8..];
}
*/

string GuildName()
{
    string	*path;
    
    path=efun::explode(efun::explode(object_name(this_object()), "#")[0], "/");
    if((strstr(path[2], "files.") == 0) && (path[3] == "room"))
    { path[2]=path[2][6..]; }
/*
    if(find_player("samtpfote")) 
    { 
	tell_object(find_player("samtpfote"), 
		    sprintf("*** GuildName(): %s\npath=%O\n",
			path[2], path));
    }		    
*/    
    return lowerstring(path[2]);
}

varargs int
IsGuildMember(object pl) {
  string plg, gn;

  if (!pl && !(pl=this_player()))
    return 0;
  if (!(plg=pl->QueryProp(P_GUILD)))
    return 0;
    
    gn=GuildName(); plg=lowerstring(plg);

/*    
    if(find_player("samtpfote")) 
    { 
	tell_object(find_player("samtpfote"), 
		    sprintf("*** IsGuildMember(): "+
			    "ob=%O, plg=%s, GuildName()=%s\n",
			pl, plg, gn));
    }		    
*/    
    
  if (gn!=plg) {
    _notify_fail("Du gehörst dieser Gilde nicht an!\n");
    return 0;
  }
  return 1;
}

int check_cond(mixed cond) {
  string res;

  if (intp(cond)) {
    _notify_fail("Dir fehlt noch die nötige Erfahrung. "+
                "Komm später wieder.\n");
    return (this_player()->QueryProp(P_XP)>=cond);
  }
  if (mappingp(cond)) {
    res=check_restrictions(this_player(),cond);
    if (!stringp(res)) return 1;
    _notify_fail(res);
    return 0;
  }
  return 1;
}

int can_advance() {
  int lv;
  mapping lvs;

  if (!(lv=this_player()->QueryProp(P_GUILD_LEVEL))) return 1;
  if (!(lvs=QueryProp(P_GUILD_LEVELS))) return 0;
  return check_cond(lvs[lv+1]); // Bedingung fuer naechsten Level testen.
}

void adjust_title(object pl) {
  int lv;
  mixed ti;


  if (!pl ||
      !(lv=pl->QueryProp(P_GUILD_LEVEL)))
    return;
  switch(pl->QueryProp(P_GENDER)) {
  case MALE:
    ti=QueryProp(P_GUILD_MALE_TITLES);break;
  case FEMALE:
    ti=QueryProp(P_GUILD_FEMALE_TITLES);break;
  default:
    return;
  }
  if (mappingp(ti))
    ti=ti[lv];
  if (stringp(ti))
    pl->SetProp(P_GUILD_TITLE,ti);
}

void do_advance() {
  int lv;

  lv=this_player()->QueryProp(P_GUILD_LEVEL)+1;
  if (lv<1) lv=1;
  this_player()->SetProp(P_GUILD_LEVEL,lv);
  adjust_title(this_player());
  write(
  "Gut gemacht.\n"
  "Du bist jetzt "+this_player()->Name()+" "+
  this_player()->QueryProp(P_GUILD_TITLE)+".\n"
  "Das ist der "+
  this_player()->QueryProp(P_GUILD_LEVEL)+". Gildenlevel der "+
  capitalize(this_player()->QueryProp(P_GUILD))+".\n");                     
// Ausgabe aufm Info-Channel

  "/obj/infomaster.c"->InfoMsg(
  this_player()->Name()+" "+this_player()->QueryProp(P_GUILD_TITLE)+
  " ist gerade auf den "+this_player()->QueryProp(P_GUILD_LEVEL)+
  ". Gildenlevel der "+capitalize(this_player()->QueryProp(P_GUILD))+
  " aufgestiegen. ");

  this_player()->Set(P_LAST_ADVANCE_GUILD, SAVE, F_MODE_AS);
  this_player()->SetProp(P_LAST_ADVANCE_GUILD, time() );
}

int try_advance() {
  if (can_advance()) {
    if (IsGuildMember(this_player()))
      do_advance();
    return 1;
  }
  return 0;
}

int beitreten() {
  string res;
  int erg;

  if (res=check_restrictions(this_player(),QueryProp(P_GUILD_RESTRICTIONS))) {
    // Werden die Beitrittsbedingungen erfuellt?
    printf("Du kannst dieser Gilde nicht beitreten.\nGrund: %s",res);
    return -3;
  }
  if (erg=GUILDMASTER->beitreten()) {
    if (erg<0)
      return erg;
    if (!(this_player()->QueryProp(P_GUILD_LEVEL)))
      try_advance(); // Level 1 wird sofort vergeben
    return 1;
  }
  return 0;
}

varargs int austreten(int loss,object pl) {
    return GUILDMASTER->austreten(loss,pl);
}

int bei_oder_aus_treten(string str) {
  if (!str) return 0;
  if (sizeof(regexp(({lower_case(str)}),
                    "\\<aus\\>.*gilde\\>.*\\<aus\\>")))
    return this_object()->austreten();
  if (sizeof(regexp(({lower_case(str)}),
                     "(gilde\\>.*\\<bei\\>|\\<in\\>.*gilde\\>.*\\<ein\\>)")))
    return this_object()->beitreten();
  return 0;
}

varargs int
AddSkill(string sname, mapping ski) {
  mapping skills;

  if (!sname)
    return 0;
  if (!mappingp(skills=QueryProp(P_GUILD_SKILLS)))
    skills=([]);
  if (!mappingp(ski))
    ski=([]);
  if (!stringp(ski[SI_SKILLFUNC]))
    // Wenn keine Funktion angegeben ist, Funktionsname=Skillname
    ski[SI_SKILLFUNC]=sname;

  ski=AddSkillMappings(QueryProp(P_GLOBAL_SKILLPROPS),ski);

  skills[sname]=ski;
  SetProp(P_GUILD_SKILLS,skills);
  return 1;
}


varargs int
AddSpell(string verb, mapping ski) {
  mapping skills;

  if (!verb)
    return 0;
  if (!mappingp(ski))
    ski=([]);
  if (!stringp(ski[SI_SPELLBOOK]) &&
      !stringp(ski[SI_SPELLBOOK]=QueryProp(P_GUILD_DEFAULT_SPELLBOOK)))
    // Wenn kein Spellbook angegeben ist muss ein
    // Default-Spellbook angegeben sein, sonst Fehler
    return 0;
  if (file_size(SPELLBOOK_DIR+ski[SI_SPELLBOOK]+".c")<0)
    return 0; // Spellbook sollte auch existieren...

  return AddSkill(lower_case(verb),ski);
}

mapping QuerySkill(string skill) {
  mapping ski;

  if (!skill
      || !(ski=QueryProp(P_GUILD_SKILLS))
      || !(ski=ski[skill])) // Gildenspezifische Skilleigenschaften
    return 0;
  return copy(ski);
}

mapping QuerySpell(string spell) {
  mapping ski,ski2;
  string spellbook,sfunc;

  if (!spell
      || !(ski=QuerySkill(spell))
      || !(spellbook=ski[SI_SPELLBOOK]))
    return 0;
  if (!(sfunc=ski[SI_SKILLFUNC]))
    sfunc=spell;
  spellbook=SPELLBOOK_DIR+spellbook;
  if (!(ski2=(spellbook->QuerySpell(sfunc))))
    return 0;
  return AddSkillMappings(ski2,ski); // Reihenfolge wichtig!
  // Die Gilde kann Spelleigenschaften neu definieren!
}

varargs int
UseSpell(object caster, string spell, mapping sinfo) {
  mapping ski;
  string spellbook;

  if (!caster
      || !spell
      || !(ski=QuerySkill(spell)) // Existiert dieser Spell in dieser Gilde?
      || !(spellbook=ski[SI_SPELLBOOK])) // Spellbook muss bekannt sein
    return 0;
  if (sinfo)
    ski+=sinfo;
  spellbook=SPELLBOOK_DIR+spellbook;
  // printf("%O %O %O %O\n",spellbook,caster,spell,ski);
  return spellbook->UseSpell(caster,spell,ski);
}

int
InitialSkillAbility(mapping ski, object pl) {
  if (!ski || !pl) return 0;
  return (300*GetOffset(SI_SKILLLEARN,ski,pl)+
          (200*pl->QueryAttribute(ski[SI_SKILLLEARN_ATTRIBUTE])*
           GetFactor(SI_SKILLLEARN,ski,pl))/100);
}

int
SkillListe(int what) {
  mapping skills;
  string *ind,*info,*info2,s,sp;
  int i,res;
  
  if (!mappingp(skills=QueryProp(P_GUILD_SKILLS))) return 0;
  if (!sizeof(ind=m_indices(skills))) return 0;
  info=({});info2=({});
  for (i=sizeof(ind)-1;i>=0;i--) {
    if (stringp(s=skills[sp=ind[i]][SI_SKILLINFO]))
      s=sprintf("%-20s (%s)",sp,s);
    else
      s=sp;
    if (capitalize(ind[i][0..0])!=ind[i][0..0])
      // Spells werden klein geschrieben, Skills gross
      info+=({s});
    else
      info2+=({s});
  }
  info=sort_array(info,#'>);info2=sort_array(info2,#'>);
  res=0;
  if ((what & 0x01) && (i=sizeof(info))) {
    write("Du kannst versuchen, Folgendes zu lernen:\n");
    for (--i;i>=0;i--)
      printf("   %s\n",capitalize(info[i]));
    res=1;
  }
  if ((what & 0x02) && (i=sizeof(info2))) {
    write("Du kannst versuchen, folgende Fähigkeiten zu erwerben:\n");
    for (--i;i>=0;i--)
      printf("   %s\n",capitalize(info2[i]));
    res=1;
  }
  return res;
}

int
LearnSpell(string spell) {
  mapping ski,restr;
  object pl;
  string res;
  mixed learn_initfunc;
  int abil,diff;

  if (!IsGuildMember(pl=this_player())) {
    _notify_fail("Du gehörst dieser Gilde nicht an!\n");
    return 0;
  }
  _notify_fail("Was möchtest Du lernen?\n");
  if (!spell)
    return SkillListe(0x01);
  spell=lower_case(spell);
  if (!(ski=QuerySpell(spell)))
    return 0;
  if (pl->QuerySkill(spell)) {
    _notify_fail("Du kannst diese Fähigkeit doch schon!\n");
    return 0;
  }
  if ((restr=ski[SI_SKILLRESTR_LEARN])
      && (res=check_restrictions(pl,restr))) {
    printf("Du kannst diese Fähigkeit noch nicht lernen.\nGrund: %s",res);
    return 1;
  }
  abil=InitialSkillAbility(ski,pl);
  if (abil<1) abil=1;
  if (abil>7500) abil=7500;
  write("Du lernst etwas Neues.\n");
  if (!(diff=GetFValueO(SI_DIFFICULTY,ski,pl)))
    diff=GetFValueO(SI_SPELLCOST,ski,pl);
  pl->ModifySkill(spell,abil,diff);
  return 1;
}

varargs int
LearnSkill(string skill) {
  mapping ski,restr;
  object pl;
  string res;
  mixed learn_initfunc;
  int abil,diff;

  if (!IsGuildMember(pl=this_player())) {
    _notify_fail("Du gehörst dieser Gilde nicht an!\n");
    return 0;
  }
  _notify_fail("Was möchtest Du lernen?\n");
  if (!skill)
    return SkillListe(0x02);
  skill=capitalize(skill);
  if (!(ski=QuerySkill(skill)))
    return 0;
  if (pl->QuerySkill(skill)) {
    _notify_fail("Du hast diese Fähigkeit doch schon!\n");
    return 0;
  }
  if ((restr=ski[SI_SKILLRESTR_LEARN])
      && (res=check_restrictions(pl,restr))) {
    printf("Du kannst diese Fähigkeit noch nicht erwerben.\nGrund: %s",res);
    return 1;
  }
  abil=InitialSkillAbility(ski,pl);
  if (!abil) abil=1;
  if (abil>MAX_ABILITY) abil=MAX_ABILITY;
  if (abil<-MAX_ABILITY) abil=-MAX_ABILITY;
  write("Du erwirbst eine neue Fähigkeit.\n");
  diff=GetFValueO(SI_DIFFICULTY,ski,pl);
  pl->ModifySkill(skill,abil,diff);
  return 1;
}

int GuildRating(object pl)
{
  mapping ski;
  string *sk;
  int i, cnt, sum;
  closure qsa;

  if (!IsGuildMember(pl)||!query_once_interactive(pl))
    return 0;

  if (!(ski = QueryProp(P_GUILD_SKILLS)) ||
      !(qsa = symbol_function("QuerySkillAbility",pl)))
    return 0;

  sk = m_indices(ski);
  cnt = sizeof(ski);

  for (i=cnt-1, sum=0; i>=0; i--)
    sum += funcall(qsa, sk[i]);

  sum = sum/cnt;
  if (sum < 0)
    sum = 0;
  else if (sum > MAX_ABILITY)
    sum = MAX_ABILITY;

  return pl->SetProp(P_GUILD_RATING, sum);
}

int MaxGuildRating()
{
  return 10000;
}

// Wird von /std/player/quest.c aufgerufen, wenn Quest geloest.
// (Jedes mal - nicht nur beim ersten mal.)
// Ist zum selbst-ueberschreiben gedacht, sollte aber definiert sein.
void NotifyGiveQuest(object pl, string key){  }

