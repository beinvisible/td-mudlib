// SilberLand MUDlib
//
// std/living/skills.c -- Skill/Spell Handling for Livings
//
// $Date: 1998/02/12 15:38:28 $
/* $Revision: 1.6 $
 * $Log: skills.c,v $
 * Revision 1.6  1998/02/12 15:38:28  Woody
 * P_DEFAULT_GUILD Handling
 *
 * Revision 1.5  1997/09/17 14:35:28  Woody
 * ModifySkillAttribute() hat sinnvollen Returnwert
 *
 * Revision 1.4  1997/06/25 11:56:14  Woody
 * QuerySkillAbility() Parameter flexibler (wie UseSkill())
 *
 * Revision 1.3  1997/06/02 17:08:38  Feyaria
 * grrr falshe klammernung in der max_ability berchnung
 *
 * Revision 1.2  1997/03/13 00:21:23  Woody
 * Levelanforderungen in LimitAbility() um 5 Level raufgeschraubt
 * Jetzt kann man erst mit Level 35 volle Skill/Spell Ability haben...
 *
 * Revision 1.1  1997/03/13 00:20:55  Woody
 * Initial revision
 *
 */

inherit "/std/living/std_skills";

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <properties.h>
#include <new_skills.h>
#include <wizlevels.h>

#pragma strong_types

nomask static int security_check() {
  object tp;

  if (!objectp(tp=this_player())
      || tp!=this_interactive()
      || !interactive(tp)
      || !IS_ARCH(tp))
    return 0;
  return 1;
}

string _set_guild(string gildenname) {
  object pre;

  if (!objectp(pre=previous_object())) return 0;
  if (pre!=this_object() // Das Lebewesen selber darf die Gilde setzen,
      && object_name(pre)!=GUILDMASTER  // der Gildenmaster auch
      && !security_check())            // und auch Erzmagier.
    return 0;
  return Set(P_GUILD, gildenname);
}

string _query_guild() {
  string res;

  if (!(res=Query(P_GUILD)) && query_once_interactive(this_object()))
    return QueryProp(P_DEFAULT_GUILD) || DEFAULT_GUILD;
  // Spieler, die keiner Gilde angehoeren, gehoeren zur Abenteurergilde
  return res;
}

int _set_guild_level(int num) {
  string gilde;
  mapping levels;

  if (!(gilde=QueryProp(P_GUILD)))  // Defaultgilden haben keine Gildenlevel
    return 0;
  if (!mappingp(levels=Query(P_GUILD_LEVEL)))
    levels=([]);
  levels[gilde]=num;
  Set(P_GUILD_LEVEL,levels);
  return num;
}

int _query_guild_level() {
  string gilde;
  mapping levels;

  if (!(gilde=QueryProp(P_GUILD)))
    return 0;
  if ((gilde==QueryProp(P_DEFAULT_GUILD)) || (gilde==DEFAULT_GUILD))
    return QueryProp(P_LEVEL);  // Defaultgilden: Gildenlevel = Spielerlevel
  if (!mappingp(levels=Query(P_GUILD_LEVEL)))
    return 0;
  return levels[gilde];
}


string _set_guild_title(string t) {
  string gilde;
  mapping titles;

  if (!(gilde=QueryProp(P_GUILD)))
    return 0;
  if (!mappingp(titles=Query(P_GUILD_TITLE)))
    titles=([]);
  titles[gilde]=t;
  Set(P_GUILD_TITLE,titles);
  return t;
}

string _query_guild_title() {
  string gilde,t;
  object g;
  mapping titles;

  if (!(gilde=QueryProp(P_GUILD)))
    return 0;
  if (!mappingp(titles=Query(P_GUILD_TITLE)))
    titles=([]);
  t=titles[gilde];
  if (!t
      && query_once_interactive(this_object())
      && objectp(g=find_object("/gilden/"+gilde))) {
    g->adjust_title(this_object());
    SetProp(P_TITLE,0);
    if (!mappingp(titles=Query(P_GUILD_TITLE))) return 0;
    t=titles[gilde];
  }
  return t;
}

string _query_title() {
  string t1,t2;

  t2=QueryProp(P_GUILD_TITLE);
  if (t1=Query(P_TITLE)) return t1;
  return t2;
}

static mixed execute_anything(mixed fun, mixed args) {
  object ob;

  if (closurep(fun) && objectp(query_closure_object(fun)))
    return funcall(fun,args);
  if (!pointerp(fun) || sizeof(fun)<2) return 0;
  if (stringp(fun[0]))
    ob=find_object(fun[0]);
  else
    ob=fun[0];
  if (!objectp(ob) || !stringp(fun[1])) return 0;
  return call_other(ob,fun[1],args);
}

/*
   Skill-Attribute:
   Attribute, die sich eine bestimmte Zeit lang auf alle Skills auswirken
   (allgemeine Qualitaet, Geschwindigkeit, Dauer, Reichweite, Ausdehnung,
    Abwehrwahrscheinlichkeit ...)

   P_SKILL_ATTRIBUTES:
     Mapping attributname:modifier
   modifier: Array mit 3 Eintraegen:
     - eigener modifier
     - vom feind gesetzter modifier (< 100%)
     - vom freund gesetzter modifier (> 100%)
   Jeder dieser Eintraege ist wiederrum ein Array mit 3-4 Eintraegen:
     - Wert (Prozentangabe, 100% bewirkt keine Aenderung)
     - Dauer der Wirksamkeit (Endzeitpunkt)
     - Level desjenigen, der diesen Wert zuletzt gesetzt hat
       Der Wert wird (bei Freund und Feind) nur dann gesetzt,
       wenn der Setzende einen hoeheren Level hat.
     [ - Funktion, die aufgerufen werden soll um value zu ermitteln. ]
*/

varargs int
QuerySkillAttribute(string atrname) {
  mixed attrs,attr;
  int i,tm,val,changed,cval;

  if (!atrname || atrname==SA_QUALITY)
    val=100;
  else
    val=QuerySkillAttribute(SA_QUALITY);
  // Die allgemeine Qualitaet wirkt sich auf alle Skill-Attribute aus.

  if (!mappingp(attrs=Query(P_SKILL_ATTRIBUTES)) ||
      !pointerp(attr=attrs[atrname]) || !(i=sizeof(attr)))
    return val;

  tm=time();
  changed=0;
  for (--i;i>=0;i--) {
    if (!pointerp(attr[i]) || sizeof(attr[i])<3 ||
        attr[i][SA_CHANGE_UNTIL]<tm) {
      // Ungueltig oder Zeit der Wirksamkeit abgelaufen?
      attr[i]=0;
      changed=1;
      continue;
    }
    if (sizeof(attr[i])<4 ||
        !intp(cval=execute_anything(attr[i][SA_CHANGE_FUNCTION], attr[i])) ||
        // Wenn eine Funktion angegeben ist, deren Ergebnis nehmen
        // (Falls der Rueckgabewert gueltig ist)
        cval<10 || cval>1000)
      cval=attr[i][SA_CHANGE_VALUE];
    val=(val*attr[i][SA_CHANGE_VALUE])/100;
  }

  if (changed) {
    attrs[atrname]=attr;
    Set(P_SKILL_ATTRIBUTES,attrs);
    // Veraenderungen speichern, spart einiges an Testerei ein
  }

  if (val<10) val=10;
  if (val>1000) val=1000;
  return val;
}

varargs int
ModifySkillAttribute(object caster, string atrname, int value, int duration,
                     mixed fun) {
  // Modifiziert ein Attribut fuer nachfolgende Skills
  mixed attr,attrs,change;
  int changer_pos,level,endtime,res;

  duration=(duration*QuerySkillAttribute(SA_DURATION))/100;
  // Hier wirkt sich schon die Modifikation der Dauer aus

  if (!objectp(caster) || value<10 || value>1000 || duration<0)
    return 0;
  // gueltige Werte?
  if (!mappingp(attrs=QueryProp(P_SKILL_ATTRIBUTES)))
    attrs=([]);
  if (!pointerp(attr=attrs[atrname]) || sizeof(attr)<3)
    attr=({0,0,0});

  if (caster==this_object())
    changer_pos=SA_FROM_PLAYER;
  else {
    if (value<=100)
      changer_pos=SA_FROM_ENEMY;
    else
      changer_pos=SA_FROM_FRIEND;
  }
  res=changer_pos+1;
  endtime=time()+duration;
  level=(caster==this_object())?QueryProp(P_LEVEL):caster->QueryProp(P_LEVEL);
  if (!(change=attr[changer_pos]) || !pointerp(change) || sizeof(change)<3 ||
      changer_pos==SA_FROM_PLAYER || level>change[SA_CHANGE_LEVEL]) {
    // Aenderung akzeptieren falls sie von diesem Objekt selber kommt oder
    // der Aendernde einen hoeheren Level hat als der, der die letzte
    // noch gueltige Aenderung vorgenommen hat.
    if (fun)
      change=({value,endtime,level,fun});
    else
      change=({value,endtime,level});
  } else {
    res*=-1;
  }

  attr[changer_pos]=change;
  attrs[atrname]=attr;
  Set(P_SKILL_ATTRIBUTES,attrs);
  return res;
}

varargs mapping _set_newskills(mapping value, string gilde) {
  // Eigentlich sollte man den _set-Funktionen keine weiteren Parameter geben
  mapping skills;

  if (!gilde && !(gilde=QueryProp(P_GUILD))) gilde="ANY";
  if (!(skills=Query(P_NEWSKILLS))) skills=([]);
  if (!value) value=([]);

  // Security check
  if (this_interactive() &&
      (getuid(this_interactive()) != getuid(this_object())))
    log_file("ARCH/X_SKILL_SET",
        sprintf("%s: TI=%O, TO=%O\n",
            ctime()[4..18], this_interactive(), this_object()));

  skills[gilde]=value;
  Set(P_NEWSKILLS,skills);
  return value;
}

varargs mapping _query_newskills(string gilde) {
  // Eigentlich sollte man den _query-Funktionen keine Parameter geben...
  mapping skills;

  if (!gilde && !(gilde=QueryProp(P_GUILD))) gilde="ANY";
  skills=Query(P_NEWSKILLS);
  if (!mappingp(skills) ||
      !mappingp(skills=skills[gilde]))
    return ([]);
  return skills;
}


varargs mapping QuerySkill(string sname) {
  mixed res;
  string gilde;

  if (!sname)
    return 0;
  if (!(res=_query_newskills()[sname]) && // Skill existiert nicht in Gilde
      !((gilde=QueryProp(P_GUILD)) &&     // Gilde ist gesetzt...
        (res=_query_newskills("ANY")[sname])
        // und Skill existiert auch nicht Gildenunabhaengig?
      ))
    return 0;
  if (!mappingp(res))
    res=([SI_SKILLABILITY:res]);
  if (gilde)
    // Wenn "gilde" gesetzt ist muss dieser Skill bei der zweiten
    // Zuweisung "res=..." gesetzt worden, also Gildenunabhaengig sein.
    res+=([SI_GUILD:"ANY"]);
  return res;
  // Uebrigens, _query- und _set-Funktionen sollten eigentlich nicht
  // direkt aufgerufen werden... Hier hat es einiges vereinfacht...
}

#define SMUL(x,y) ((x<0 && y<0)?(-1*x*y):(x*y))
varargs int QuerySkillAbility(mixed sname, mixed argl) {
  mapping res;
  string skill2;
  mixed val;

  if (objectp(sname) && stringp(argl)) sname = argl;
  if (!(res=QuerySkill(sname))) return 0;
  if (!intp(val=res[SI_SKILLABILITY])) return val;
  if (skill2=res[SI_INHERIT]) {
    int val2;
    val2=QuerySkillAbility(skill2);
    val=(val+SMUL(val,val2))/(2*MAX_ABILITY);
  }
  return val;
}

static varargs mixed
LimitAbility(mixed sinfo, int diff) {
  mixed abil;
  int max,old,d2;

  abil=(mappingp(sinfo)?sinfo[SI_SKILLABILITY]:sinfo);
  if (!intp(abil)) return sinfo;
  old=abil;

  if (mappingp(sinfo) && (d2=sinfo[SI_DIFFICULTY]))
    // Beim Spieler eingetragene Schwierigkeit gilt vor angegebener.
    diff=d2;

  if(QueryProp(P_LEVEL)<30)
    max=MAX_ABILITY-(diff+100)*(35-QueryProp(P_LEVEL));
  else
    max=MAX_ABILITY-(diff+100)*(40-QueryProp(P_LEVEL))/2;
  // Diff 0     1: 66%   10: 75%   15: 80%   20: 85%   25: 90%   30: 95%
  // Diff 100   1: 32%   10: 50%   15: 60%   20: 70%   25: 80%   30: 90%
  // Diff 200   1: -2%   10: 25%   15: 40%   20: 55%   25: 70%   30: 85%
  // Diff 300   1:-36%   10:  0%   15: 20%   20: 40%   25: 60%   30: 80%
  // Diff 400   1:-70%   10:-25%   15:  0%   20: 25%   25: 50%   30: 75%
  //
  // Das entspricht einer direkten Erhoehung saemtlicher Anforderungen
  // um 5 Level!! D.h. es trifft schon die Anfaenger. Wenn der Level
  // spaeter einmal noch hoeher geschraubt wird, sollte evtl. ein
  // 'Streckungsfaktor' im Bereich knapp unter 1 (*80/100 o.ae.)
  // eingerechnet werden, um die Max-Ability ein bisschen besser
  // ueber die Level zu 'streuen'... [Anm. Woody]

  // Hier noch eine etwas komplettere Tabelle:
  //
  // diff|lvl 1:|   3:|   7:| 10:| 13:| 16:| 19:| 22:| 25:| 28:| 31:| 34:|
  // ----+------+-----+-----+----+----+----+----+----+----+----+----+----+
  //  -50|   83%|  84%|  86%| 87%| 89%| 90%| 92%| 93%| 95%| 96%| 98%| 99%|
  //  -10|   69%|  72%|  74%| 77%| 80%| 82%| 85%| 88%| 91%| 93%| 96%| 99%|
  //    0|   66%|  69%|  72%| 75%| 78%| 81%| 84%| 87%| 90%| 93%| 96%| 99%|
  //   10|   62%|  65%|  69%| 72%| 75%| 79%| 82%| 85%| 89%| 92%| 95%| 98%|
  //   20|   59%|  62%|  66%| 70%| 73%| 77%| 80%| 84%| 88%| 91%| 95%| 98%|
  //   30|   55%|  59%|  63%| 67%| 71%| 75%| 79%| 83%| 87%| 90%| 94%| 98%|
  //   40|   52%|  56%|  60%| 65%| 69%| 73%| 77%| 81%| 86%| 90%| 94%| 98%|
  //   50|   49%|  53%|  58%| 62%| 67%| 71%| 76%| 80%| 85%| 89%| 94%| 98%|
  //  100|   32%|  38%|  44%| 50%| 56%| 62%| 68%| 74%| 80%| 86%| 92%| 98%|
  //  150|   15%|  22%|  30%| 37%| 45%| 52%| 60%| 67%| 75%| 82%| 90%| 97%|
  //  200|   -2%|   7%|  16%| 25%| 34%| 43%| 52%| 61%| 70%| 79%| 88%| 97%|
  //  250|  -19%|  -8%|   2%| 12%| 23%| 33%| 44%| 54%| 65%| 75%| 86%| 96%|
  //  300|  -36%| -24%| -12%|  0%| 12%| 24%| 36%| 48%| 60%| 72%| 84%| 96%|
  //  400|  -70%| -55%| -40%|-25%|-10%|  5%| 20%| 35%| 50%| 65%| 80%| 95%|
  //  500| -104%| -86%| -68%|-50%|-32%|-14%|  4%| 22%| 40%| 58%| 76%| 94%|
  //  600| -138%|-117%| -96%|-75%|-54%|-33%|-12%|  9%| 30%| 51%| 72%| 93%|

  if (abil>max) abil=max;
  if (abil>MAX_ABILITY) abil=MAX_ABILITY;
  if (abil<-MAX_ABILITY) abil=-MAX_ABILITY;
  if (old && !abil) abil=1;
  // Faehigkeiten sollen nicht durch die Begrenzung verschwinden

  if (!mappingp(sinfo)) return abil;
  sinfo[SI_SKILLABILITY]=abil;
  return sinfo;
}

varargs void ModifySkill(string sname, mixed val, int diff, string gilde) {
  mapping skills;
  mixed skill;

  if (!sname)
    return;
  skills=_query_newskills(gilde);
  skill=skills[sname];
  if (mappingp(skill)) {
    if (mappingp(val))
      skill+=val;
    else
      skill[SI_SKILLABILITY]=val;
  } else {
    if (mappingp(val))
      skill=([SI_SKILLABILITY:skill])+val;
    else
      skill=val;
  }
  skill=LimitAbility(skill,diff);
  skills[sname]=skill;
  _set_newskills(skills,gilde);
  // Uebrigens, _query- und _set-Funktionen sollten eigentlich nicht
  // direkt aufgerufen werden... Hier hat es einiges vereinfacht...
}

varargs int LearnSkill(string sname, int add, int diff) {
  mapping res;
  string skill2,gilde;
  int val;

  if (add>MAX_ABILITY/50) add=MAX_ABILITY/50;
  if (add<1) add=1;
  res=QuerySkill(sname);
  if (res) {
    val=res[SI_SKILLABILITY];
    gilde=res[SI_GUILD];
  } else {
    val=0;
    gilde=0;
  }
  val+=add;
  if (!val && add) val=1;
  // Faehigkeit soll durch Erhoehung nicht verschwinden.
  ModifySkill(sname,val,diff,gilde);
  if (mappingp(res) && skill2=res[SI_INHERIT])
    LearnSkill(skill2,add/3,diff);
  return 1;
}

varargs int return0() {return 0;}

static mixed _load_object(string s) {
  object ob;
  
  call_other(s,"???");
  if (ob=find_object(s)) return ob;
  return s;
}

varargs int UseSpell(string str, string spell) {
  string gilde,sbook;
  mapping sinfo;
  closure cl;
  
  if (!spell && !(spell=query_verb()))
    return 0;
  spell=lower_case(spell);
  if (!(sinfo=QuerySkill(spell)))
    return 0;
  sinfo[SI_SKILLARG]=str; // Argument eintragen

  if (!objectp(get_type_info((cl=sinfo[SI_CLOSURE]),2))) {
    if (stringp(sbook=sinfo[SI_SPELLBOOK]))
      // Wenn ein Spellbook angegeben ist wird der Spell direkt ausgefuehrt
      cl=symbol_function("UseSpell",_load_object(SPELLBOOK_DIR+sbook));
    else if (gilde=QueryProp(P_GUILD))
    // Wenn der Spieler in einer Gilde ist, so weiss diese, in welchem
    // Spellbook der Spell zu finden ist...
      cl=symbol_function("UseSpell",_load_object(GUILD_DIR+gilde));
    else
      cl=symbol_function("return0",this_object());
    sinfo[SI_CLOSURE]=cl;
    ModifySkill(spell,([SI_CLOSURE:cl]),0,sinfo[SI_GUILD]);
  }
  return funcall(cl,this_object(),spell,sinfo);
}

varargs mixed UseSkill(string skill, mapping args) {
  mapping sinfo;
  string gilde, func,skill2;
  mixed res;
  closure cl;
  
  if (!skill) return 0;
  skill=capitalize(skill);
  if (!(sinfo=QuerySkill(skill))) return 0;
  if (args) sinfo+=args;

  if (!objectp(get_type_info((cl=sinfo[SI_CLOSURE]),2))) {
    if (!(func=sinfo[SI_SKILLFUNC]) || // Keine Funktion angegeben?
        !(gilde=QueryProp(P_GUILD)))   // Keine Gilde angegeben?
    {
      // Dann Standard-Funktion nehmen
      if (function_exists("StdSkill_"+skill,this_object()))
        cl=symbol_function("StdSkill_"+skill,this_object());
      else // Falls es die nicht gibt den Ability-Wert liefern
        cl=symbol_function("QuerySkillAbility",this_object());
    } else {
      // Sonst diese Funktion im Gildenobjekt aufrufen
      cl=symbol_function(func,_load_object(GUILD_DIR+gilde));
    }
    sinfo[SI_CLOSURE]=cl;
    ModifySkill(skill,([SI_CLOSURE:cl]),0,sinfo[SI_GUILD]);
  }
  res=funcall(cl,this_object(),skill,sinfo);
  if ((skill2=sinfo[SI_INHERIT]) && mappingp(res))
    res=UseSkill(skill2,res); // Fuer Skills, die von anderen abhaengen
  return res;
}
