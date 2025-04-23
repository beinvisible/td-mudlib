/*
 *  Tool um Sehertore zu verwalten - Woody@SilberLand
 *  ---
 *  1996/06/04  Woody  1.0  Release
 *  1996/02/11  Feyaria     Abfrage auf IS_WIZARD
 *
 * $Date: 2001/01/13 13:25:22 $
 * $Revision: 1.2 $
 * $Log: tortool.c,v $
 * Revision 1.2  2001/01/13 13:25:22  Samtpfote
 * Abfrage auf IS_ELDER
 *
 * Revision 1.1  2001/01/13 13:24:52  Samtpfote
 * Initial revision
 *
 */

inherit "std/thing";

#include <defines.h>
#include <properties.h>
#include <language.h>
#include <wizlevels.h>
#include <moving.h>
#include <ansi.h>

#define TM "/obj/sehertormaster"
#define DOORFILE "/obj/sehertore"
//#define DOORFILE "/secure/ARCH/sehertore"

static object owner;
mapping doors;

void create()
{
  string str;

  if (IS_BLUE(ME)) return;
  ::create();
  owner = PL;
  restore_object(DOORFILE);
  if (!doors) doors=([]);

  AddId("tortool");
  AddId("sehertortool");
  SetProp(P_ARTICLE,0);
  SetProp(P_GENDER, NEUTER);

  str=capitalize(owner->name(WESSEN));
  SetProp(P_NAME, str+" Sehertor-Tool");
  SetProp(P_SHORT, str+" Sehertor-Tool");
  SetProp(P_LONG,
   "Das Sehertor-Tool dient der Administration von Sehertoren.\n\n"
  +"Implementierte Kommandos:\n"
  +"  sts(how)                      : Liste aller Sehertore anzeigen\n"
  +"  stg(oto)    <n>               : trans zu Sehertor\n"
  +"Und für Erzmagier++:\n"
  +"  sta(dd)     <room> [<n>]      : Neues Sehertor Nr. n einfügen\n"
  +"  std(el)     <n>               : Sehertor entfernen\n"
  +"  stc(hange)  <n> <newroom>     : Sehertor verlegen\n\n"
  +" <room> ist der komplette Pfadname des Raumes, der das Sehertor\n"
  +" enthält, oder \"here\", d.h. der aktuelle Raum.\n");

  SetProp(P_NEVERDROP, 1);
  SetProp(P_NODROP, "Lass das lieber nicht wo rumliegen.\n");
  SetProp(P_NOBUY, 1);
  AddCmd(({ "sts", "stshow" }),   "stShow");
  AddCmd(({ "stg", "stgoto" }),   "stGoto");
  if (!IS_ARCH(this_interactive())) return;
  AddCmd(({ "sta", "stadd" }),    "stAdd");
  AddCmd(({ "std", "stdel" }),    "stDel");
  AddCmd(({ "stc", "stchange" }), "stChange");
}

void init()
{
  if( !IS_ELDER(environment(this_object())))
    call_out("remove",0);
  ::init();
}

int _query_autoloadobj() { return 1; }

int stShow(string str)
{
  int i, n;
  string *ind, lst;
  mixed *inf, ix;

  inf=({});
  ind=m_indices(doors);
  if (sizeof(ind)) for (i=sizeof(ind)-1; i>=0; i--) {
    ix=ind[i];
    if (!n=doors[ix]) continue;
    inf+=({({n,doors[ix,1],ix})});
  }
  inf=sort_array(inf,lambda(({'x,'y}),
                            ({#'<,({#'[, 'x, 0}),({#'[, 'y, 0})})));
  lst = ANSI_BOLD+"Liste aller Sehertore:"+ANSI_NORMAL+"\n\n";
  for (i=sizeof(inf)-1;i>=0;i--)
    lst += sprintf(" %s%2d.%s %s\n     [%s]\n",
      ANSI_BOLD, inf[i][0], ANSI_NORMAL, inf[i][1], inf[i][2]);
  this_player()->More(lst);

  return 1;
}

int stGoto(string str)
{
  int i, n;
  string *ind;
  // "  stg(oto)    <n>               : trans zu Sehertor\n\n"
  notify_fail (
    "syntax: stgoto <n>\n"+
    "  <n> ........ Nummer des Sehertors, zu dem teleportiert werden soll.\n");
  if (!stringp(str)) return 0;
  if (sscanf(str, "%d", n) != 1) return 0;
  ind=m_indices(doors);
  if (sizeof(ind)) for (i=sizeof(ind)-1; i>=0; i--) {
    if (doors[ind[i]]==n) {
      write ("Ok, teleportiere zu Sehertor "+n+".\n");
      this_player()->move(ind[i], M_TPORT);
      return 1;
    }
  }
  write ("Kein Sehertor mit dieser Nummer gefunden!\n");
  return 1;
}

int stAdd(string str)
{
  string s, *ind;
  int n, m, i;

  if (!IS_ARCH(this_interactive())) return 0;
  notify_fail (
    "syntax: stadd <room> [<n>]\n"+
    "  <room> ..... Raum in dem sich das Sehertor befindet (oder \"here\").\n"+
    "  <n> ........ Nummer, die das Tor bekommen soll.\n"+
    "               (falls ausgelassen, wird die Nummer automatisch vergeben)\n");
  if (!stringp(str)) return 0;
  str=this_player()->_unparsed_args()+" ";
  if (sscanf(str, "%s %d", s, n) == 1) n=0;
  else
    if (sscanf(str, "%s %d", s, n) != 2) return 0;
  if (s=="here") s=object_name(environment(this_player()));
  ind=m_indices(doors);
  if (sizeof(ind)) for (i=sizeof(ind)-1; i>=0; i--)
  {
    if (doors[ind[i]]==n) {
      write ("Ein Sehertor mit dieser Nummer gibt es bereits!\n  - "+
        doors[ind[i],1]+" ["+ind[i]+"]\n");
      return 1;
    }
    if (doors[ind[i]]>m) m=doors[ind[i]];
  } else m=0;
  if (n==0) n=m+1;
  write (sprintf("Ok, füge Raum \"%d. %s\" hinzu.\n"+
    "Beschreibung (~q Abbruch): ", n, s));
  input_to ("stAdd2", 0, s, n);
  return 1;
}

void stAdd2(string str, string s, int n)
{
  if (!IS_ARCH(this_interactive())) return 0;
  if (str=="~q") { write("Ok, Abbruch!\n"); return; }
  doors += ([ s: n; str ]);
  write ("Ok, eingetragen.\n");
  save_object (DOORFILE);
  TM->ReloadDoorFile();
}

int stDel(string str)
{
  int i, n;
  string *ind;

  if (!IS_ARCH(this_interactive())) return 0;
  notify_fail (
    "syntax: stdel <n>\n"+
    "  <n> ..... Nummer des Sehertors, das entfernt werden soll (oder \"here\").\n");
  if (!stringp(str)) return 0;
  if (str=="here")
  {
    str=object_name(environment(this_player()));
    if (!member(doors, str))
      write ("Hier ist kein Sehertor (oder es ist nicht in der Liste)!\n");
    else {
      write ("Sehertor \""+doors[str, 1]+"\" wirklich loeschen? (j/n) ");
      input_to ("stDel2", 0, str);
    }
    return 1;
  }
  if (sscanf(str, "%d", n) != 1) return 0;
  ind=m_indices(doors);
  if (sizeof(ind)) for (i=sizeof(ind)-1; i>=0; i--)
    if (doors[ind[i]]==n) {
      write ("Sehertor \""+doors[ind[i], 1]+"\" wirklich löschen? (j/n) ");
      input_to ("stDel2", 0, ind[i]);
      return 1;
    }
  write ("Kein Sehertor mit dieser Nummer gefunden!\n");
  return 1;
}

int stDel2(string str, string ind)
{
  if (!IS_ARCH(this_interactive())) return 0;
  if ((str=="j") || (str=="ja")) {
    write ("Ok, Sehertor #"+doors[ind]+" (\""+doors[ind, 1]+"\") gelöscht.\n");
    doors -= ([ind]);
    save_object (DOORFILE);
    TM->ReloadDoorFile();
  } else {
    write ("Ok, Abbruch.\n");
  }
 return 1;
}

int stChange(string str)
{
  int i, n;
  string s, *ind;

  if (!IS_ARCH(this_interactive())) return 0;
  // "  stc(hange)  <n> <newroom>     : Sehertor verlegen\n"
  notify_fail (
    "syntax: stchange <n> <room>\n"+
    "  <n> ........ Nummer, des zu verlegenden Sehertors.\n"+
    "  <room> ..... Raum, in den das Sehertor verlegt werden soll (oder \"here\").\n");
  if (!stringp(str)) return 0;
  str=this_player()->_unparsed_args();
  if (sscanf(str, "%d %s", n, s) != 2) return 0;
  if (s=="here") s=object_name(environment(this_player()));
  ind=m_indices(doors);
  if (sizeof(ind)) for (i=sizeof(ind)-1; i>=0; i--)
    if (doors[ind[i]]==n) {
      write ("Sehertor \""+doors[ind[i], 1]+"\" wirklich verlegen? (j/n) ");
      input_to ("stChange2", 0, ind[i], s);
      return 1;
    }
  write ("Kein Sehertor mit dieser Nummer gefunden!\n");
  return 1;
}

int stChange2(string str, string ind, string s)
{
  if (!IS_ARCH(this_interactive())) return 0;
  if ((str=="j") || (str=="ja")) {
    write ("Ok, Sehertor #"+doors[ind]+" (\""+doors[ind, 1]+"\") verlegt.\n");
    doors += ([s: doors[ind,0]; doors[ind, 1]]);
    doors -= ([ind]);
    save_object (DOORFILE);
    TM->ReloadDoorFile();
  } else {
    write ("Ok, Abbruch.\n");
  }
 return 1;
}

