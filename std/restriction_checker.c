// MorgenGrauen MUDlib
//
// restriction_checker.c -- Prueft Einschraenkungen bei Gildenwechsel
//
// $Date :$
/* $Revision: 1.2 $
 * $Log: restriction_checker.c,v $
 * Revision 1.2  2001/01/15 13:17:12  Samtpfote
 * SR_INCLUDE/EXCLUDE_RACE auf lowerstring()
 *
 * Revision 1.1  2001/01/15 13:10:16  Samtpfote
 * Initial revision
 *
 * Revision 2.4  1997/09/17 14:17:04  Woody
 * Crashfix (wer weiss... :))
 *
 * Revision 2.3  1997/06/17 10:32:28  Feyaria
 * typo entfernt
 *
 * Revision 2.2  1997/05/28 15:22:59  Feyaria
 * is_seer aus interactive abfragen (npcs imit seherstatus geibts ja nicht)
 *
 * Revision 2.1  1997/04/19 20:40:50  Feyaria
 * zusaetzliche Restrikitionen ei (MAX_LEVEL, SIZEm , GENDER, FROG)
 *
 * Revision 2.0  1997/03/13 03:11:09  Woody
 * prepared for revision control
 *
 * Revision 1.6  1995/07/25  13:32:48  Rochus
 * Wenn keine Alkohol-Einschraenkung da war durfte
 * man NICHTS getrunken haben...
 *
 * Revision 1.5  1995/07/24  16:47:36  Silvana
 * Typo fixed (< statt > )
 *
 * Revision 1.4  1995/06/13  13:38:38  Rochus
 * Abfrage auf P_ALCOHOL/DRINK/FOOD
 *
 * Revision 1.3  1995/02/06  19:44:14  Jof
 * RCS-Logs aufgeraeumt (leere Logs raus usw)
 *
*/

#define NEED_PROTOTYPES
#include <thing/properties.h>
#include <properties.h>
#include <new_skills.h>
#include <wizlevels.h>
#pragma strong_types

static mixed execute_anything(mixed fun, mixed args) {
  if (!fun) return 0;
  if (closurep(fun)
      && objectp(query_closure_object(fun)))
    return funcall(fun,args);
  if (stringp(fun))
    return call_other(this_object(),fun,args);
  if (pointerp(fun)
      && sizeof(fun)>=2
      && stringp(fun[1])
      && (stringp(fun[0]) || objectp(fun[0])))
    return call_other(fun[0],fun[1],args);
  return 0;
}

mapping race_modifier(object pl, mapping map) {
  string race;
  mapping rmap;

  if (!map || !pl)
	return map;
  if ((rmap=map[SM_RACE])
	  && (race=pl->QueryProp(P_RACE))
	  && (rmap=rmap[race]))
	return map+rmap;
  return map;
}

// #define PropertyCheck(x) (restr[x] && pl->QueryProp(x)<restr[x])
// #define AttributeCheck(x) (restr[x] && pl->QueryAttribute(x)<restr[x])

string
check_restrictions(object pl, mapping restr) {
  /* Folgende Einschraenkungen koennen geprueft werden:
   * - Mindestwerte bei allen Attributen und Properties Level, QP, XP
   * - bestimmte Rassen koennen ausgeschlossen werden bzw. erlaubt werden
   *   (SR_EXCLUDE_RACE ist ein Array von ausgeschlossenen Rassen,
   *    SR_INCLUDE_RACE eines mit eingeschlossenen)
   * - SR_RACE kann ein Mapping sein, dass Rassen ein eigenes
   *   Restriction-mapping zuordnet. "*" steht fuer alle anderen Rassen.
   */
  string race,res;
  mixed newres;
  string *genderstr;

  genderstr= ({"Etwas", "Mann", "Frau"});
    
  if (!mappingp(restr)) return 0; // Keine Einschraenkungen
  if (!objectp(pl)) return "";
  restr=race_modifier(pl,restr);
  if ((pl->QueryProp(P_LEVEL) < restr[P_LEVEL]) ||
	  (pl->QueryProp(P_GUILD_LEVEL) < restr[P_GUILD_LEVEL]))
    return "Deine Stufe reicht dafür nicht aus.\n";
  if ((restr[SR_MAX_LEVEL] && pl->QueryProp(P_LEVEL) > restr[SR_MAX_LEVEL]) )
    return "Deine Stufe ist dafür schon zu hoch.\n";
  if ( restr[SR_SEER] && !IS_SEER(pl) && interactive(pl) )
    return "Dafür musst Du Seher sein.\n";
  if( pl->QueryProp(P_SIZE) < restr[P_SIZE] )
    return "Dafür bist Du zu klein.\n";
  if( restr[SR_MAX_SIZE] && pl->QueryProp(P_SIZE) > restr[SR_MAX_SIZE] )
    return "Dafür bist Du zu groß.\n";
  if( restr[P_GENDER] && pl->QueryProp(P_GENDER) != restr[P_GENDER] )
    return "Das kannst Du als "+genderstr[pl->QueryProp(P_GENDER)]+" nicht.\n";
  if( restr[P_FROG] && pl->QueryProp(P_FROG) )
    return "Das kann ein Frosch nicht.\n";
  if (pl->QueryAttribute(A_INT) < restr[A_INT])
    return "Deine Intelligenz reicht dafür nicht aus.\n";
  if (pl->QueryProp(P_XP) < restr[P_XP])
    return "Du hast nicht genügend Erfahrung dafür.\n";
  if (pl->QueryProp(P_QP) < restr[P_QP])
    return "Du hast nicht genügend Aufgaben gelöst.\n";
  if (restr[P_ALCOHOL] && pl->QueryProp(P_ALCOHOL) >= restr[P_ALCOHOL])
    return "Du bist zu besoffen.\n";
  if (restr[P_DRINK] && pl->QueryProp(P_DRINK) >= restr[P_DRINK])
    return "Du hast zuviel getrunken.\n";
  if (restr[P_FOOD] && pl->QueryProp(P_FOOD) >= restr[P_FOOD])
    return "Du hast zuviel gegessen.\n";
  if (pl->QueryAttribute(A_DEX) < restr[A_DEX])
    return "Deine Geschicklichkeit reicht dafür nicht aus.\n";
  if (pl->QueryAttribute(A_CON) < restr[A_CON])
    return "Deine Ausdauer reicht dafür nicht aus.\n";
  if (pl->QueryAttribute(A_STR) < restr[A_STR])
    return "Deine Stärke reicht dafür nicht aus.\n";
  if (restr[SR_BAD] && pl->QueryProp(P_ALIGN)<restr[SR_BAD])
    return "Du bist zu böse.\n";
  if (restr[SR_GOOD] && pl->QueryProp(P_ALIGN)>restr[SR_GOOD])
    return "Du bist nicht böse genug.\n";
  if (restr[SR_FREE_HANDS] >
	  pl->QueryProp(P_MAX_HANDS)-pl->QueryProp(P_USED_HANDS))
	return "Du hast nicht genug freie Hände dafür.\n";

 /* 
  if(pointerp(restr[SR_INCLUDE_RACE]))
  {
	"/room/debug"->debug_message(previous_object(), 
	  sprintf("%O (%s) SR_INCLUDE: %O\n\n",
		pl, pl->QueryProp(P_RACE), restr[SR_INCLUDE_RACE]));
  }
 
 if(pointerp(restr[SR_EXCLUDE_RACE]))
  {
        "/room/debug"->debug_message(previous_object(), 
          sprintf("%O (%s) SR_EXCLUDE: %O\n\n",
                pl, pl->QueryProp(P_RACE), restr[SR_EXCLUDE_RACE]));
  }
*/	
  race=lowerstring(pl->QueryProp(P_RACE));
  if ((pointerp(restr[SR_EXCLUDE_RACE]) &&
       member(restr[SR_EXCLUDE_RACE],race)>=0) ||
      (pointerp(restr[SR_INCLUDE_RACE]) &&
       member(restr[SR_INCLUDE_RACE],race)<0))
    return ("Als "+capitalize(race)+" kannst Du das nicht.\n");
  return execute_anything(restr[SR_FUN],pl);
}

varargs mixed
GetData(string dname, mapping map, object pl) {
  mixed dat,res;

  if (!dname || !map) return 0;
  if (closurep(dat=map[dname]) && (res=funcall(dat,pl)))
	return res;
  return dat;
  
}

varargs int
GetValue(string vname, mapping map, object pl) {
  mixed dat,res;

  // printf("GetValue(%O): %O\n",vname,map);
  if (!vname || !map) return 0;
  if ((dat=map[vname]) && (res=execute_anything(dat,pl)))
	return res;
  // printf("Value: %O\n",dat);
  return intp(dat) ? dat : 0;
}

varargs int
GetFactor(string fname, mapping map, object pl) {
  mixed res;

  // printf("GetFactor(%O):\n",fname);
  if (!fname  || !(res=GetValue(FACTOR(fname),map,pl)))
	return 100;
  if (res<10) res=10;
  if (res>1000) res=1000;
  // printf("Factor: %O\n",res);
  return res;
}

varargs int
GetOffset(string oname, mapping map, object pl) {
  mixed res;

  // printf("GetOffset(%O):\n",oname);
  if (!oname  || !(res=GetValue(OFFSET(oname),map,pl)))
	return 0;
  if (res<-10000) res=-10000;
  if (res>10000) res=10000;
  // printf("Offset: %O\n",res);
  return res;
}

varargs int
GetFValue(string vname, mapping map, object pl) {
  return (GetValue(vname,map,pl)*GetFactor(vname,map,pl))/100;
}

varargs int
GetValueO(string vname, mapping map, object pl) {
  return GetValue(vname,map,pl)+GetOffset(vname,map,pl);
}

varargs int
GetFValueO(string vname, mapping map, object pl) {
  return ((GetValue(vname,map,pl)*GetFactor(vname,map,pl))/100
		  + GetOffset(vname,map,pl));
}

mapping AddSkillMappings(mapping oldmap, mapping newmap) {
  mapping res,t1,t2;
  mixed *inx,ind;
  int i;

  if (!oldmap) return newmap;
  if (!newmap) return oldmap;
  inx=({SI_SKILLRESTR_LEARN,SI_SKILLRESTR_USE,SM_RACE});
  res=oldmap+newmap;
  for (i=sizeof(inx)-1;i>=0;i--) {
	ind=inx[i];
	t1=oldmap[ind];
	t2=newmap[ind];
	if (t1) {
	  if (t2)
		res[ind]=t1+t2;
	  else
		res[ind]=t1;
	} else {
	  if (t2)
		res[ind]=t2;
	}
  }
  return res;
}

