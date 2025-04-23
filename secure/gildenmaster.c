/* Tamedhon MUDlib
 * Samtpfote@Tamedhon
 *
 * $Revision: 1.2 $
 * $Date: 2004/2/16 20:33:00 $
 * $Log: gildenmaster.c,v $
 * Revision 1.2  2004/2/16 20:33:00  Serii
 * kleine anpassung in austreten() - man kann nun auch aus ner Gilde
 * rausgeschmissen werden. das war vorher nicht moeglich...
 *
 * Revision 1.1  2001/01/12 18:52:16  Samtpfote
 * Initial revision
 *
 */

#define NEED_PROTOTYPES
inherit "/std/thing";
#include <thing/properties.h>
#include <properties.h>
#include <wizlevels.h>
#include <new_skills.h>
#pragma strong_types

#define DB(x)	\
	if(find_player("debugger")) { \
		tell_object(find_player("debugger"), \
			sprintf("[GILDENMASTER] %O\n",x)); }

void create() {
  if (clonep(this_object())) {
	destruct(this_object());
	return;
  }
  ::create();
  
  seteuid(getuid());
  
  restore_object(GUILD_SAVEFILE);
  if (!QueryProp(P_VALID_GUILDS)) {
	SetProp(P_VALID_GUILDS,({}));
	Set(P_VALID_GUILDS,SAVE|SECURED,F_MODE);
  }
}

nomask int beitreten() {
  object pl,gilde;
  string gname,ogname,dfguild;
  if (!(pl=this_player()) || !(gilde=previous_object()))
	return 0;
  gname=explode(object_name(gilde),"#")[0];
  ogname=pl->QueryProp(P_GUILD);
  dfguild=(pl->QueryProp(P_DEFAULT_GUILD)||"abenteurer");
  
  DB(sprintf("pl=%O, gname=%s, ogname=%s, dfguild=%s", 
	pl, gname, ogname, dfguild));

//  if ((ogname=pl->QueryProp(P_GUILD)) && ogname!=DEFAULT_GUILD) { //die zeile gegen die folgende ausgetauscht (Serii@Tamedhon 13.08.2002)
  if ((ogname=pl->QueryProp(P_GUILD)) && ogname!=dfguild) {
    DB(sprintf("ogname=%s", ogname));
    if ((GUILD_DIR+ogname)==gname) {
      write("Du bist schon in dieser Gilde.\n");
      DB(" Bereits in Gilde.");
      return -4;
    }
    DB(" In anderer Gilde.");
    write("Du bist noch in einer anderen Gilde.\n");
    return -1;
  }
  if (gname[0..7]!=GUILD_DIR ||
	  member(QueryProp(P_VALID_GUILDS),(gname=gname[8..]))<0) {
	write("Diese Gilde ist leider nicht zugelassen.\n"+
		  "Bitte verständige einen Erzmagier.\n");
        DB(" * * * * * * UNGÜLTIGE GILDE * * * * *");
	return -2;
  }

  pl->SetProp(P_GUILD,gname);
  DB("Beitritt OK.");
  return 1;
}

static nomask void loose_ability(mixed key, mixed dat, int loss) {
  mixed val;

  val=mappingp(dat)?dat[SI_SKILLABILITY]:dat;
  if (!intp(val) || val<=0) return;
  val-=(val*loss)/100;if (val<=0) val=1;
  if (mappingp(dat))
	dat[SI_SKILLABILITY]=val;
  else
	dat=val;
}

varargs nomask int austreten(int loss, object tp) {
  object pl,gilde;
  string gname;
  mapping skills;

  if (!(pl=this_player()) || !(gilde=previous_object()))
	return 0;

//--serii
  if((!query_once_interactive(pl))&&
     (explode(object_name(pl),"/")[0]=="gilden")&&
     (objectp(tp)))pl=tp;
//--iireS

  gname=explode(object_name(gilde),"#")[0];
  DB(sprintf("pl=%O, gname=%s, plguild=%s",
	pl, gname, pl->QueryProp(P_GUILD))); 
  DB("Austritt");
  if (gname[0..7]!=GUILD_DIR ||
	  pl->QueryProp(P_GUILD)!=gname[8..]) {
	write("Du kannst hier nicht aus einer anderen Gilde austreten.\n");
        DB("failed.");
	return -1;
  }
  if (loss<=0) loss=20;
  skills=pl->QueryProp(P_NEWSKILLS);
  walk_mapping(skills,"loose_ability",this_object(),loss);
  pl->SetProp(P_NEWSKILLS,skills);
  pl->SetProp(P_GUILD,0);
  DB("Ok.");
  return 1;
}

nomask static int security_check() {
  object tp;

  if (!objectp(tp=this_player()) ||
	  tp!=this_interactive() ||
	  !interactive(tp) ||
	  !IS_ARCH(tp))
	return 0;
  return 1;
}
  

nomask int AddGuild(string gildob) {
  object tp;

  if (!gildob || !security_check() || file_size(GUILD_DIR + gildob + ".c")<0)
	return 0;
  SetProp(P_VALID_GUILDS,QueryProp(P_VALID_GUILDS)+({gildob}));
  save_object(GUILD_SAVEFILE);
  return 1;
}

nomask int RemoveGuild(string gildob) {
  object tp;

  if (!gildob || !security_check())
	return 0;
  SetProp(P_VALID_GUILDS,QueryProp(P_VALID_GUILDS)-({gildob}));
  save_object(GUILD_SAVEFILE);
  return 1;
}

nomask int ValidGuild(string gildenob) {
  if (!gildenob) return 0;
  return (member(QueryProp(P_VALID_GUILDS),gildenob)>=0);
}

nomask int SaveGuild()
{
  save_object(GUILD_SAVEFILE);
  return 1;
}

