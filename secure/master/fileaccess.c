// Tamedhon MUDLIB
// Querolin@Tamedhon
//
// Routinen zur Zugriffsverwaltung
// 2024/04/09 - inherit vor include gesetzt. inherits muessen immer VOR funktionsdekalration sein - Aloy
//
/* $Revision: 1.5 $
 * $Date: 2010/09/02 16:00:00 $
 * $Log: fileaccess.c,v $
 *
 * Revision 1.5  2010/09/02 16:00:00  Grynnis
 * Umbau im valid_read: Mitarbeiter der Region duerfen die Region jetzt immer
 * lesen, egal welchen Level sie haben. Damit Objekte funktionieren, auch
 * wenn deren Magier im Ruhestand ist (Level 10) - Beispiel Stammbaum im Tal
 *
 * Revision 1.4  2010/03/02 22:30:00  Grynnis
 * beim Schreibzugriff auf /d/<REGION>/<IS_ARCH> keine Sonderbehandlung mehr
 *
 * Revision 1.3  2003/07/06 15:59:00  Querolin
 * /lib darf von jedem gelesen und geschrieben werden.
 *
 * Revision 1.2  2003/03/23 19:20:00  Serii
 * aenderung an valid_read() - /p/obj/xlogins/map/map.c darf auf /d/* lesen!
 *
 * Revision 1.1  2001/01/18 10:37:23  Samtpfote
 * Initial revision
 *
 */

inherit "/secure/master/domain";
 
#include "/secure/master.h"
#include "/secure/config.h"


string creator_file(mixed str) {
  string *strs,tmp;
  int s;

  if (objectp(str))
    strs = full_path_array(object_name(str), 0);
  else if (stringp(str))
    strs = full_path_array(str, 0);
  else return NOBODY;

  s = sizeof(strs);
  if (s<2) return NOBODY;

  switch (strs[0]) {
  case DOMAINDIR:
    if (s==2 || !IS_LEARNER(strs[2]))
      return strs[1];
    return strs[2];

  case PROJECTDIR:
    if (s==2 || !IS_LEARNER(strs[2]))
      return "p:"+strs[1];
    return strs[2];

  case WIZARDDIR:
    if (s>2)
      return strs[1];
    if (s==2 && file_size(strs[0]+"/"+strs[1])==-2)
      return strs[1];
    return NOBODY;

  case "secure":
    return ROOTID;

  case "gilden":
  case "spellbooks":
    tmp = strs[1];
    if (tmp[<2..<2]==".") tmp=tmp[0..<3];
    if ((s=member(tmp,'.'))>0) tmp=tmp[s+1..];
    return "GUILD."+tmp;

  case "std":
  case "room":
  case "obj":
    return BACKBONEID;

  case "doc":
    return "DOC";

  case "mail":
    return MAILID;

  case "news":
    return NEWSID;
  }
  return NOBODY;
}

mixed valid_read(string path, string euid, string fun, object obj) {
  string *strs;
  int s;
  string nuid;

  if (member(path,' ')!=-1)
    return 0;

  if (TI)
    nuid = getuid(TI);
  else
    nuid = "-";

  if (!euid) euid="-";

  strs = full_path_array(path, euid);
  path = implode(strs, "/");

  if (!sizeof(strs) || euid == ROOTID || obj==TO) return path;

  if (sizeof(strs) > 1 && strs[0] == "doc") return path;
  if (sizeof(strs) > 1 && strs[0] == "open") return path;

  if (sizeof(strs) > 1 && strs[0] == "wahl") return 0;
  if (sizeof(strs) > 1 && strs[0] == "news" &&
      (strs[<1][0..8]=="erzmagier" || strs[<1][0..12]=="OLD.erzmagier") &&
      !IS_ARCH(euid))
    return 0;
  if (sizeof(strs)>1 && strs[0]=="news" &&
      (strs[<1][0..12]=="regionsmagier" ||
       strs[<1][0..16]=="OLD.regionsmagier") &&
      !IS_LORD(euid))
    return 0;

  if (strs[0] == "mail" && euid == MAILID)
    return path;

  if (strs[0] == "mail")
  {
    if (sizeof(strs)>2&&strs[1]=="spool") return path;
    if (sizeof(strs)>2 && strs[2]!=euid+".o" && strs[2] !=euid &&
        strs[2]!=nuid+".o" && strs[2] !=nuid)
      return 0;
  }
  /* folgende Funktionen sind natuerlich voellig uninteressant */
  if (member(({"get_dir", "restore_object", "file_size", "read_file"}), fun)!=-1)
    return path;
    
  if (sizeof(strs)>1)
  {
    // "/secure/save/" und "/secure/save_incative/" fuer NIEMANDEN lesbar
    // "/log/ARCH/" und "/secure/ARCH/" nur fuer Erzmagier lesbar
    if ((strs[0] == "secure" &&
          (strs[1] == "save" || strs[1] == "save_inactive" )) ||
        ((strs[0] == "log" || strs[0] == "secure") &&
          strs[1] == "ARCH" && !IS_ARCH(euid)))
      return 0;

    // "/secure/LORD/" nur fuer Regionsmagier lesbar
    if (strs[0] == "secure" && strs[1] == "LORD" && !IS_LORD(euid))
      return 0;

    // "/log/report/*.ep" nur fuer Erzmagier lesbar
    if ((sizeof(strs)==3) && (strs[0]=="log") && (strs[1]=="report") &&
        (strs[2][<3..]==".ep") && !IS_ARCH(euid))
      return 0;
  }
  if (sizeof(strs)==1 && strs[0]=="core")
    return 0;

  /* /d/ handling */
  if (sizeof(strs) > 1 && strs[0] == "d")
  {
    /* RM koennen alles in /d/ lesen */
    if (IS_LORD(euid)) return path;

    /* Mitarbeiter in dieser Region duerfen diese Region lesen */
    if (domain_member(euid, strs[1]))
      return path;

    // ls-hack fuer ls /d/
    if (strs[1] == "*") return path;

    /* default wird geblockt */
    return 0;
  }

  /* gilden & spellbook */
  /*
  if (sizeof(strs) > 1 && strs[0] == "gilden")
  {
    if (euid == BACKBONEID) return path;

    if (euid == "GUILD."+strs[1])
    { return path; }

    if (interactive() && !IS_SPECIAL(euid))  return 0;
    return path;
  }
  if (sizeof(strs) > 1 && strs[0] == "spellbooks")
  {
    if (euid == "GUILD."+strs[1]) return path;
    if (!IS_SPECIAL(euid)) return 0;
    return path;
  }
 */

  return path;
}

int access_rights(string *p_arr, string euid) {
  int i;

  i = sizeof(p_arr) - 2;
  while (i >= 0 && file_size(implode(p_arr[0..i],"/")+"/access_rights.c") < 0)
    i--;
  if (i<0) return 0;
  if (!catch(i=call_other(implode(p_arr[0..i],"/")+"/access_rights",
                    "access_rights",euid,implode(p_arr[i+1..],"/"))))
    return i;
  return 0;
}

mixed valid_write(string path, string euid, string fun, object obj) {
  string *strs;
  int s;
  int x;
  string *pn;

  if (member(path,' ')!=-1)
    return 0;

  // tell_object(find_player("debugger"), sprintf("obj==%O\n", obj));
  if (fun=="log_file")
  {
    strs = full_path_array("/"+path, 0);
    if (sizeof(strs)>1 && strs[0]=="log") return implode(strs,"/");
  }
  if (!euid || euid=="NOBODY" || euid=="ftp" || euid=="anonymous") return 0;

  switch(fun) {
  case "save_object":
    if (!sizeof(path)) return 0;
      strs = full_path_array("/"+path, 0);
    break;

  case "ed_start":
    if (path && path[0])
      strs = full_path_array(path, euid);
    else
      strs = ({"players",euid,".err"});
    break;

  default:
    strs = full_path_array(path, euid);
  }

  path = implode(strs, "/");

  if (euid == ROOTID || obj==TO) return path;

  s = sizeof(strs);

  if ((s==1) && (strs[0]=="ChangeLog")) return IS_ELDER(euid);
  if (s>1) {
    switch(strs[0]) {
    case "wahl":
    case "secure":
    case "news":
    case "quests":
      return 0;

    case "ftp":
    case "open":
    case "tmp":
    case "lib":
      return 1;

    case "sys":
    case "std":
    case "event":
      return IS_ARCH(euid) || access_rights(strs,euid);

    case "gilden":
    case "spellbooks":
      return
            access_rights(strs, euid) ||
            euid == "GUILD."+strs[1] || IS_ARCH(euid);

    case "doc":
      return IS_SPECIAL(euid) || access_rights(strs,euid);

    case "mail":
      if (euid==MAILID || strs[1]=="spool")
        break;

    case "save":
      if (IS_ARCH(euid)) return 1;
      if (s != 3) return 0;
      if (extern_call() &&
          ((!TP || PO != TP) &&
           (geteuid(PO)!=getuid(PO) || geteuid(PO)!=euid)))
        return 0;
      if (strs[1] == euid[0..0] && (strs[2] == euid || strs[2]==euid+".o"))
        break;
      return 0;

    case PROJECTDIR:
      if (s<3 && !IS_ARCH(euid)) return 0;
      if (IS_ELDER(euid)) break;
      if (objectp(obj))
        pn = explode(object_name(obj),"/");
      else
        pn = ({});
      if (sizeof(pn)>2 && s>3 && implode(strs[0..1],":")==euid)
        return 1;
      if (s==3 && strs[2]=="ACCESS_RIGHTS")
        if (project_access(euid,strs[1])>1)
          break;
        else
          return 0;
      if (s>3 && euid==strs[2])
        break;
      if (project_access(euid,strs[1]))
        break;
      return 0;

    case DOMAINDIR:
      if (IS_ARCH(euid) || (s>2 && IS_ELDER(euid))) break;
      if (s<2) return 0;
      // if (s>2&&IS_ARCH(strs[2])) return 0;
      if (domain_master(euid,strs[1])) break;
      if (s==2) return 0;  // In /d/ selbst darf sonst keiner schreiben
      if (euid==strs[1]) break;
      if (s<=3 && (fun=="mkdir"||fun=="rmdir")) return 0;
      if (strs[2]==euid ||
          ((strs[2]=="common" || strs[2]=="alle") &&
           domain_member(euid, strs[1])))
        break;
      if (euid==strs[2]) break;
      if (access_rights(strs,euid)) break;
      return 0;

    case "log":
      if (sizeof(strs)==2 && (strs[1][0]<='Z' && strs[1][0]>='A'))
      {
        if (!IS_ARCH(euid) && fun != "write_file" && fun != "log_file")
          return 0;
      }
      else break;

    case WIZARDDIR:
      if (IS_ARCH(euid)) break;
      if (s>2 && IS_ELDER(euid)) break;
      if (IS_WIZARD(euid) && euid==strs[1]) break;
      return access_rights(strs,euid);

    default:
      return IS_ARCH(euid);
    }

    return path;
  }
  return IS_ARCH(euid);
}
