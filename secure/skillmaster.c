/* SilberLand MUDlib
 *
 * /secure/skillmaster.c -- skill handler
 *
 * $Date: 1997/03/15 23:45:29 $
 * $Revision: 1.1 $
 * $Log: skillmaster.c,v $
 * Revision 1.1  1997/03/15 23:45:29  Woody
 * prepared for revision control
 *
 */

/* Skillhandler fuer MorgenGrauen, 11/92 Jof */

/* Die Skills werden in einer AList gespeichert. Schluessel ist der Skill-
   name, die Eintraege sind wiederum Arrays mit 2 Elementen.

   Erstes Element ist die Beschreibung des Skills.

   Zweites Element ist wiederum eine AList.
   Die Eintraege in die 2. ALists haben als Schluessel die Verben, die zu
   dem entsprechenden Skill gehoeren, als Eintraege einen 4-elementigen
   Array, der als erstes Element den Filenamen eines Objektes, als 2.
   Element einen Funktionsnamen und als 3. Element eine Beschreibung des
   dem Verb zugeordneten Aktion enthaelt. 4. Element ist der Level, ab dem
   ein Spieler einen Skill erwerben kann.
*/

#include "/secure/wizlevels.h"

mixed *skills;

save_info()
{
    save_object("/etc/skills");
}

create()
{
    seteuid(getuid(this_object()));
    if (!restore_object("/etc/skills"))
        skills=({({}),({})});
		else
				reinitialize();
}

reinitialize()
{
		int i;

		for (i=0;i<sizeof(skills[1]);i++)
			skills[1][i][1]=order_alist(skills[1][i][1]);
		skills=order_alist(skills);
}

secure()
{
    if (!previous_object()) return 0;
    if (geteuid(previous_object())==ROOTID) return 1;
    if (geteuid(previous_object()) != geteuid(this_interactive())) return 0;
    if (this_interactive() != this_player()) return 0;
    if (!IS_ARCH(this_interactive())) return 0;
    return 1;
}

InsertSkill(string skillname, string skilldescr,
            string *verben, mixed objFunDescr)
{
    int i;

    if (!secure()) return 0;
    if (!stringp(skillname) || skillname=="") return -1;
    if (!pointerp(verben) || sizeof(verben)==0)
    	verben=({}); 
    if (!pointerp(objFunDescr) && sizeof(verben)!=0) return -2;
    if (!pointerp(objFunDescr)) objFunDescr=({});
    if (sizeof(verben) != sizeof(objFunDescr)) return -4;
    for (i=0;i<sizeof(verben);i++)
    {
        if (!stringp(verben[i]) || verben[i]=="") return -5;
        if (verben[i][0]!='*' &&
            (sizeof(objFunDescr[i])!=4 || !stringp(objFunDescr[i][0]) ||
             !stringp(objFunDescr[i][1]) || objFunDescr[i][0]=="" ||
             objFunDescr[i][1]=="" || !intp(objFunDescr[i][3]) ||
             objFunDescr[i][3]<1 || objFunDescr[i][3]>19)) return -6;
    }
    if (assoc(skillname, skills,-1)!=-1) return -7;
    verben=order_alist(verben, objFunDescr);
    skills=insert_alist(skillname, ({skilldescr, verben}), skills);
    save_info();
    return 1;
}

RemoveSkill(string skillname)
{
    if (!secure()) return 0;
    skills=exclude_alist(skillname, skills);
    save_info();
    return 1;
}

GetSkills()
{
    return skills[0..];
}

GetSkill(string skillname)
{
    return assoc(skillname,skills,-1);
}

GetFunctionAndDescription(string skillname, string verb)
{
    mixed *tmp;

    if ((tmp=assoc(skillname, skills,-1))==-1) return -1;
    if ((tmp=assoc(verb, tmp[1],-1))==-1) return -2;
    return tmp;
}

GetLevel(skillname, verb)
{
    mixed *tmp;

    tmp=GetFunctionAndDescription(skillname, verb);
    if (!pointerp(tmp)) return tmp;
    return tmp[3];
}

GetSkillDescription(string skillname)
{
    mixed *tmp;

    if ((tmp=assoc(skillname, skills,-1))==-1) return -1;
    return tmp[0];
}

AddVerbs(string skillname, mixed *verben, mixed *objFunDescr)
{
    mixed *tmp;
    int i;

    if (!secure()) return 0;
    if ((tmp=assoc(skillname, skills,-1))==-1) return -1;
    if (!pointerp(verben) || sizeof(verben)==0) return -2;
    if (!pointerp(objFunDescr)) return -3;
    if (sizeof(verben) != sizeof(objFunDescr)) return -4;

    for (i=0;i<sizeof(verben);i++)
    {
        if (!stringp(verben[i]) || verben[i]=="") return -5;
        if (verben[i][0]!='*' &&
            (sizeof(objFunDescr[i])!=4 || !stringp(objFunDescr[i][0]) ||
             !stringp(objFunDescr[i][1]) || objFunDescr[i][0]=="" ||
             objFunDescr[i][1]=="" || !intp(objFunDescr[i][3]) ||
             objFunDescr[i][3]<1 || objFunDescr[i][3]>19)) return -6;
    }
    verben+=tmp[1][0];
    objFunDescr+=tmp[1][1];
    verben=order_alist(verben, objFunDescr);
    skills=insert_alist(skillname, ({tmp[0],verben}), skills);
    save_info();
    return 1;
}
