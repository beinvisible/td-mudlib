/*
 *  Tool um Forscherpunkte zu verwalten - Woody@SL
 *  ---
 *  1998/02/11  Feyaria 1.8 IS_LORD abfrage im init
 *  1998/01/15  Feyaria 1.7 npcset auch mit nummer
 *  1997/06/01  Woody  1.6  npcmaster Queue Handling
 *  1997/05/28  Woody  1.5  Bugfix in eps <nr> (wenn !IS_ARCH())
 *  1997/05/27  Woody  1.4  epc(hange)
 *  1996/10/13  Woody  1.3  NPC-Master Interface
 *  1996/10/13  Woody  1.2  clone# wird nicht mehr angezeigt in epshow
 *  1996/10/10  Woody  1.1  epdump, epshow erweitert
 *  1996/09/27  Woody  1.0  Release
 *
 * $Date: 2001/01/13 13:04:34 $
 * $Revision: 1.1 $
 * $Log: eptool.c,v $
 * Revision 1.1  2001/01/13 13:04:34  Samtpfote
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
#include <exploration.h>
#include <npcmaster.h>

#define NF notify_fail

static object owner;

void create()
{
  string str;

  if (IS_BLUE(ME)) return;
  ::create();
  owner = PL;

  AddId("eptool");
  SetProp(P_ARTICLE,0);
  SetProp(P_GENDER, NEUTER);

  str=capitalize(owner->name(WESSEN));
  SetProp(P_NAME, str+" EP-Tool");
  SetProp(P_SHORT, str+" EP-Tool");

  SetProp(P_LONG,

   "Das EP-Tool dient der Administration von Exploration Points.\n\n"

   "Implementierte Kommandos (für Erzmagier++):\n"
   "  epa(dd) <objekt>:<typ>:<eps>:<keys>     Exploration Point(s) hinzufügen\n"
   "  epc(hange) <objekt>:<typ>:<eps>:<keys>  Exploration Point(s) ändern\n"
   "  epd(el) <objekt>:<typ>                  EP entfernen\n"
   "  eps(how) <nummer> | <objekt>[:<typ>]    EP #x oder eines Objekts anzeigen\n"
   "  epi(nfo) [<spieler>]                    Diverse Infos anzeigen\n"
   "  epdump                                  Sämtliche EP anzeigen\n\n"
   "  npcs(et) <objekt>|<nummer>:<bonus>      NPC-Killerbonus hinzufügen/ändern\n"
   "  npcp(ending)                            Bonus-Spielerqueue anzeigen\n"
   "  npcq(ueue) [<regexp>]                   Bonus-Queue anzeigen\n"
   "  npca(pprove) <dateiname>                Bonus bestätigen\n"
   "  npcar <regexp>                          Bonus nach Regexp bestätigen\n"
   "  npcr(emove) <dateiname>                 Eintrag aus der Queue löschen\n"
   "  npcrr <regexp>                          Einträge -\"- nach Regexp löschen\n"
   "  npcdump                                 Sämtliche NPC-Boni anzeigen\n\n"

   "<objekt> ist eine Objektangabe, wobei auch der Suffix 'im raum' bzw.\n"
   "'in mir' erlaubt ist, um genauer anzugeben, ob das Objekt im Raum oder\n"
   "im eigenen Inventory zu finden ist. Die Angabe 'hier' steht für den\n"
   "Raum selbst, in dem man grade steht.\n"
   "<typ> ist 'det' (Detail, Read-Detail, Special-Detail), 'sens' (ein raum-\n"
   "spezifischer Geruch oder Geräusch (eines Raums, NICHT eines Objekts!))\n"
   "'exit' (Ausgang), 'cmd' (ein AddCmd-Kommand), 'info' (Infos von einem NPC)\n"
   "oder 'misc' (eigene GiveExplorationPoint()-Aufrufe im Objekt).\n"
   "<keys> sind, durch Kommas getrennt, sämtliche (!!) mögliche Schlüssel,\n"
   "also das Detail, der Ausgang, das Kommando-Verb oder das Schlüsselwort\n"
   "des NPCs. Dabei sind wirklich sämtliche möglichen Schlüssel anzugeben,\n"
   "zB. 'kletter' und auch 'klettere' (\"kletter, klettere\"), wenn beides\n"
   "dieselbe Aktion auslöst.\n"
  );

  SetProp(P_NEVERDROP, 1);
  SetProp(P_NODROP, "Lass das lieber nicht wo rumliegen.\n");
  SetProp(P_NOBUY, 1);
  AddCmd(({ "epa", "epadd" }),   "epAdd");
  AddCmd(({ "epc", "epchange" }),"epChange");
  AddCmd(({ "epd", "epdel" }),   "epDel");
  AddCmd(({ "eps", "epshow" }),  "epShow");
  AddCmd(({ "epi", "epinfo" }),  "epInfo");
  AddCmd("epdump", "epDump");
  AddCmd(({ "npcs", "npcset" }),   "npcSet");
  AddCmd(({ "npcp", "npcpending" }),   "npcPending");
  AddCmd(({ "npcq", "npcqueue" }),     "npcQueue");
  AddCmd(({ "npca", "npcapprove" }),   "npcApprove");
  AddCmd(({ "npcar" }),                "npcApproveRegexp");
  AddCmd(({ "npcr", "npcremove" }),    "npcRemove");
  AddCmd(({ "npcrr" }),                "npcRemoveRegexp");
  AddCmd("npcdump", "npcDump");
  // "  npcs(how) <objekt>                      NPC-Killerbonus anzeigen\n"
  // AddCmd(({ "npcs", "npcshow" }),  "npcShow");
}

void init()
{
  if( !IS_LORD(environment(this_object())))
    call_out("remove",0);
  ::init();
}

int _query_autoloadobj() { return 1; }

object GetObj (string s)
{
  object ob;
  string s1, s2;
  if ((s=="hier") || (s=="here")) return environment(this_player());
  if (sscanf(s, "%s in mir", s1)) return present(s1, this_player());
  if (sscanf(s, "%s im raum", s1))
    return present(s1, environment(this_player()));
  if (!(ob=present(s, environment(this_player()))))
    ob=present(s, this_player());
  return ob;
}

int epAddChange(string str, int changeflag)
{
  string s_ob,s_typ,s_keys, *keys;
  int i, type, eps;
  object ob;

  str=this_player()->_unparsed_args();
  NF("Nicht genug Parameter angegeben!\n"
     "Syntax: epa(dd) <objekt>:<typ>:<eps>:<keys>\n"
     "bzw. epc(hange) <objekt>:<typ>:<eps>:<keys>\n");
  if (!str) return 0;
  if (sscanf(str, "%s:%s:%d:%s", s_ob, s_typ, eps, s_keys) < 4)
    return 0;

  if ((type=member(EP_TYP_STR, s_typ)) < 0)
    return NF("Ungültiger EP-Typ!\n"), 0;

  ob = GetObj (s_ob);
  if (!ob)
    return NF("Objekt nicht gefunden!\n"), 0;

  if ((eps<0) || (eps>EPS_MAX))
    return NF("Ungültige EP-Anzahl!\n"), 0;

  // keys = explode (implode(explode(s_keys," "),""), ",");  // spaces raus
  keys = explode (s_keys, ",");  // nee, spaces drin sind schon o.k.
  for (i=sizeof(keys)-1; i>=0; i--)                 // nur leading/trailing
    keys[i] = implode (explode(keys[i], " "), " "); // spaces wolln wir nicht

  i = changeflag ? EPMASTER->ChangeEPObject(ob, type, keys, eps)
                 : EPMASTER->AddEPObject (ob, type, keys, eps);
  if (i<0)
  {
    switch (i)
    {
      case EPERR_NOT_ARCH:
        return NF("Funktion ist Erzmagiern vorbehalten!\n"), 0;
      case EPERR_INVALID_OB:
        return NF("Das angegebene Objekt ist für EPs nicht zulässig!\n"), 0;
      case EPERR_INVALID_ARG:
        return NF("Ungültiges Argument!\n"), 0;
      default:
        return NF("Unbekannter Fehler!\n"), 0;
    }
  }
  write ("Ok, EP hinzugefügt. Insgesamt gibt es nun "+i+" EPs.\n");
  write (sprintf("%-4s  [%2d]  %:-40s  %:-20s\n",
         s_typ, eps, object_name(ob), implode(keys, ",") ));
  return 1;
}

int epAdd(string str)
{
  return epAddChange(str,0);
}

int epChange(string str)
{
  return epAddChange(str,1);
}

int npcSet(string str)
{
  string s_ob;
  int i, bonus, num;
  object ob;

  str=this_player()->_unparsed_args();
  NF("Nicht genug Parameter angegeben!\n"
     "Syntax: npcs(et) <objekt>:<bonus> oder <nummer>:<bonus>\n");
  if (!str) return 0;
  if (sscanf(str, "%d:%d", num, bonus) == 2)
    s_ob=NPCMASTER->QueryNPCbyNumber(num)[2];
  else if (sscanf(str, "%s:%d", s_ob, bonus) == 2)
  {
    ob = GetObj (s_ob);
    if (!ob) return NF("Objekt nicht gefunden!\n"), 0;
    s_ob = explode(object_name(ob),"#")[0];
  }   
  if (bonus<0) return NF("Ungültiger Bonus!\n"), 0;

  i = NPCMASTER->SetScore(s_ob, bonus);

  if (i<1)
  {
    switch (i)
    {
      case NPC_NO_PERMISSION:
        return NF("Funktion ist Erzmagiern vorbehalten!\n"), 0;
      case NPC_INVALID_ARG:
        return NF("Ungültiges Argument!\n"), 0;
      default:
        return NF("Unbekannter Fehler!\n"), 0;
    }
  }
  write ("Ok, Bonus gesetzt:\n");
  write (sprintf("[%d] %s\n", bonus, s_ob));
  return 1;
}

int npcPending()
{
  mapping m;
  mixed ind;
  int i;

  m=NPCMASTER->QueryPending();
  ind=sort_array(m_indices(m), #'<);
  if (!sizeof(ind))
    write("Derzeit keine Spieler in der Pending Queue.\n");
  else
  for (i=sizeof(ind)-1; i>=0; i--)
    write(ind[i]+": "+implode(m[ind[i]], ", ")+"\n");
  return 1;
}

int npcQueue(string str)
{
  mapping m;
  mixed ind;
  int i;

  if (str) str=this_player()->_unparsed_args();
  m=NPCMASTER->QueryQueued(str);
  ind=sort_array(m_indices(m), #'<);
  if (!sizeof(ind))
    write("Derzeit keine NPCs"+
        (str ? ", die '"+str+"' matchen" : "")+" in der Queue.\n");
  else
  for (i=sizeof(ind)-1; i>=0; i--)
    write(sprintf("%-40s: Bonus nach P_XP wäre %d\n", ind[i], m[ind[i]]));
  return 1;
}

int npcApprove(string str)
{
  int i;
  mixed x;

  str=this_player()->_unparsed_args();
  NF("Nicht genug Parameter angegeben!\n"
     "Syntax: npca(pprove) <dateiname>\n");
  if (!str) return 0;

  if (str[<2..] == ".c") str=str[0..<3];
  if (file_size(str+".c") <= 0)
    return write(str+".c nicht gefunden oder ungültig!\n"), 1;

  x = NPCMASTER->ApproveQueued(str);
  if (pointerp(x)) {
    write(sprintf("Killbonus (%d) für %s bestätigt [#%d]\n",
        x[1], str, x[0]));
    return 1;
  }
  else switch (x)
    {
      case NPC_NO_PERMISSION:
        return NF("Funktion ist Erzmagiern vorbehalten!\n"), 0;
      case NPC_INVALID_ARG:
        return NF("Ungültiges Argument!\n"), 0;
      default:
        return NF("Unbekannter Fehler!\n"), 0;
    }
}

int npcApproveRegexp(string str)
{
  int i;
  mixed x;

  str=this_player()->_unparsed_args();
  NF("Nicht genug Parameter angegeben!\n"
     "Syntax: npcar <regexp>\n");
  if (!str) return 0;

  x = NPCMASTER->ApproveQueuedRegexp(str);
  if (pointerp(x)) {
    write("Killbonus bestätigt für:\n");
    for (i=0; i<sizeof(x); i++)
      write(sprintf("#%4d  [%3d]  %-40s\n",
          x[i][1], x[i][2], x[i][0]));
    write(sprintf("-----------\nInsgesamt %d Einträge bestätigt.\n",
        sizeof(x)));
  }
  else if (x==NPC_INVALID_ARG)
    write("Keine passenden Einträge gefunden!\n");
  else switch (x)
    {
      case NPC_NO_PERMISSION:
        return NF("Funktion ist Erzmagiern vorbehalten!\n"), 0;
      /*
      case NPC_INVALID_ARG:
        return NF("Ungueltiges Argument!\n"), 0;
      */
      default:
        return NF("Unbekannter Fehler!\n"), 0;
    }
  return 1;
}

int npcRemove(string str)
{
  int i,n;
  mixed x;

  str=this_player()->_unparsed_args();
  NF("Nicht genug Parameter angegeben!\n"
     "Syntax: npcr(emove) <dateiname>\n");
  if (!str) return 0;

  n = sizeof(NPCMASTER->QueryQueued(str));
  if (str[<2..] == ".c") str=str[0..<3];
  x=NPCMASTER->RemoveQueued(str);
  if (mappingp(x))
    if (sizeof(x) != n)
      write("Ok, Eintrag '"+str+"' aus Queue entfernt.\n");
    else
      write("Kein Eintrag dieses Namens in der Queue!\n");
  else switch (x)
    {
      case NPC_NO_PERMISSION:
        return NF("Funktion ist Erzmagiern vorbehalten!\n"), 0;
      case NPC_INVALID_ARG:
        return NF("Ungültiges Argument!\n"), 0;
      default:
        return NF("Unbekannter Fehler!\n"), 0;
    }
  return 1;
}

int npcRemoveRegexp(string str)
{
  int i,n;
  mixed x;

  str=this_player()->_unparsed_args();
  NF("Nicht genug Parameter angegeben!\n"
     "Syntax: npcrr <regexp>\n");
  if (!str) return 0;

  n = sizeof(NPCMASTER->QueryQueued(str));
  x=NPCMASTER->RemoveQueuedRegexp(str);
  if (mappingp(x))
    if (sizeof(x) != n)
      write("Ok, "+(sizeof(x)-n)+" Einträge aus Queue entfernt.\n");
    else
      write("Keine passenden Einträge in der Queue!\n");
  else switch (x)
    {
      case NPC_NO_PERMISSION:
        return NF("Funktion ist Erzmagiern vorbehalten!\n"), 0;
      case NPC_INVALID_ARG:
        return NF("Ungültiges Argument!\n"), 0;
      default:
        return NF("Unbekannter Fehler!\n"), 0;
    }
  return 1;
}

int npcDump()
{
  if (!IS_ARCH(this_player()))
    return NF("Funktion ist Erzmagiern vorbehalten!\n"), 0;
  this_player()->More(read_file(NPCDUMPFILE));
  return 1;
}

int epDel(string str)
{
  string s_ob, s_typ;
  int i, type;
  object ob;

  str=this_player()->_unparsed_args();
  NF("Nicht genug Parameter angegeben!\n"
     "Syntax: epd(el) <objekt>:<typ>\n");
  if (!str) return 0;
  if ((i=sscanf(str, "%s:%s", s_ob, s_typ)) < 2)
    return 0;
  if ((type=member(EP_TYP_STR, s_typ)) < 0)
    return NF("Ungültiger EP-Typ!\n"), 0;

  ob = GetObj (s_ob);
  if (!ob)
    return NF("Objekt nicht gefunden!\n"), 0;

  i = EPMASTER->RemoveEPObject (ob, type);
  if (i<0)
  {
    switch (i)
    {
      case EPERR_NOT_ARCH:
        return NF("Funktion ist Erzmagiern vorbehalten!\n"), 0;
      case EPERR_INVALID_OB:
        return NF("Das angegebene Objekt ist für EPs nicht zulässig!\n"), 0;
      case EPERR_INVALID_ARG:
        return NF("Ungültiges Argument!\n"), 0;
      case EPERR_NO_ENTRY:
        return NF("Objekt hatte keinen FP!\n"), 0;
      default:
        return NF("Unbekannter Fehler!\n"), 0;
    }
  }
  write (sprintf("%-4s  %:-70s\n", s_typ, object_name(ob) ));
  write ("Ok, EP gelöscht. Insgesamt gibt es nun "+i+" EPs.\n");
  return 1;
}

int epShow(string str)
{
  string s_ob, s_typ, s;
  mixed result;
  int i, type;
  object ob;

  str=this_player()->_unparsed_args();
  NF("Nicht genug Parameter angegeben!\n"
     "Syntax: eps(how) <nummer> | <objekt>[:<typ>]\n");
  if (!str) return 0;
  if ((i=sscanf(str, "%s:%s", s_ob, s_typ)) < 2)
  {
    s_ob=str;
    s_typ="";
  }
  ob = GetObj (s_ob);
  if (!ob)
    if (s_ob==to_string(i=to_int(s_ob)))
    {
      result = ({ "", 0, ({}), 0 });
      result = EPMASTER->QueryEPEntry(i);
      if (!result)
        return NF("Die EP-Tabelle hat keinen Eintrag #"+i+".\n"), 0;
      else if (intp(result))
        switch(result)
        {
          case EPERR_NOT_ARCH:
            write("Funktion ist Erzmagiern vorbehalten!\n");
            break;
          default:
            write("Unbekannter Fehler!\n");
        }
      else
      {
        write (sprintf("EP-Nr: #%d, Objekt: \"%s\", Typ: \"%s\"\n"
                       "EPs: %d, Keys: ({ %s })\n",
               i, result[0], EP_TYP_STR[result[1]],
               result[3], implode(result[2], ", ") ));
      }
      return 1;
    }
    else
      return NF("Objekt nicht gefunden!\n"), 0;
  s="";
  s_ob = explode(object_name(ob), "#")[0];
  result = ({ ({}), 0, 0 });
  if (s_typ!="")
  {
    if ((type=member(EP_TYP_STR, s_typ)) < 0)
      return NF("Ungültiger EP-Typ!\n"), 0;
    if ((result=EPMASTER->QueryEPObject (ob, type) ))
      s = sprintf ("%-4s  #%04d  %:-25s  [%2d]  ({ %.25s })\n",
                   s_typ, result[EPM_NUM], s_ob[<25..],
                   result[EPM_EPS], implode(result[EPM_KEY], ",") );
  }
  else
  {
    for (type=0; type<=EP_TYP_MAX; type++)
    {
      result = ({ ({}), 0, 0 });
      if (pointerp(result=EPMASTER->QueryEPObject (ob, type)))
        s += sprintf ("%-4s  #%04d  %:-25s  [%2d]  ({ %.25s })\n",
                     EP_TYP_STR[type], result[EPM_NUM], s_ob[<25..],
                     result[EPM_EPS], implode(result[EPM_KEY], ",") );
    }
  }
  if (s!="")
    write (
    "type  bitnr  file name of object        EPs   Keys\n"+
    "-----------------------------------------------------------------------------\n"+
    s);
  else
    if (intp(result) && (result < 0))
      switch(result)
      {
        case EPERR_NOT_ARCH:
          write("Funktion ist Erzmagiern vorbehalten!\n");
          break;
        case EPERR_INVALID_OB:
          write("Das angegebene Objekt ist für EPs nicht zulässig!\n");
          break;
        default:
          write("Unbekannter Fehler!\n");
      }
    else
      write ("Das Objekt "+s_ob+" hat keine EP"+
        (s_typ != "" ? " vom Typ \""+s_typ+"\".\n" : ".\n"));
  return 1;
}

int epInfo(string str)
{
  object ob;

  if (!str)
  {
    write (
      sprintf ("Anzahl der EP-Objekte: %d\n"+
               "Maximum an erreichbaren EPs: %d\n"+
               "Aktueller EP-Durchschnitt: %d\n",
               EPMASTER->QueryMaxEPObjects(),
               EPMASTER->QueryMaxEP(),
               EPMASTER->QueryAverage() ));
  }
  else
  {
    ob=find_player(str);
    if (!ob)
      return NF("Spieler "+capitalize(str)+" nicht gefunden!\n"), 0;
    write (
      sprintf ("%s hat %d EP gefunden.\n",
        capitalize(getuid(ob)), EPMASTER->QueryExplorationPoints(ob) ));
  }
  return 1;
}

int epDump(string str)
{
  int result;
  result = EPMASTER->dumpEP();
  if (result < 0) switch(result)
  {
    case EPERR_NOT_ARCH:
      write("Funktion ist Erzmagiern vorbehalten!\n");
      break;
    default:
      write("Unbekannter Fehler!\n");
  }
  return 1;
}

