/*
 * Tamedhon MUDlib
 * ---
 * /secure/explorationmaster.c
 * Exploration Master - verwaltet die Forscherpunkte
 * Maintained in Tamedhon by Querolin & TamTam
 * Maintained by Woody - Credits to Wargon@MorgenGrauen
 * ---
 * $Date: 2008/03/09 20:00:00 $
 * $Revision: 2.1 $
 * $Log: explorationmaster.c,v $
 *
 * Revision 2.1  2008/08/16 15:00:00  TamTam
 * Schwankungen im Schnitt der FPs etwas stabilisiert
 * Konstanten ausgelagert nach exploration.h
 *
 * Revision 2.0  2008/03/09 20:00:00  TamTam
 * SL und TD Versionen zusammengefuehrt wegen EP Orakel
 * ARCH_SECURITY gibts hier nicht -> raus
 *
 * Revision 1.x  2002/11/10 18:20:00  Querolin
 * RecalcAverage wieder aktiviert und Aenderungen im reset
 *
 * Revision 1.13  2002/09/20 20:22:10  Feyaria
 * Gesamten Pfad mitloggen
 *
 * Revision 1.12  2001/10/24 14:04:48  Woody
 * Beim Dump zusaetzlich komplette Liste erstellen, More() raus
 *
 * Revision 1.11  2001/01/03 18:30:37  Zordan
 * FP-Orakel eingebaut. Neues Savefileformat!
 *
 * Revision 1.10  1999/05/26 13:56:52  Zordan
 * Neues Loghandling
 *
 * Revision 1.9  1998/09/20 13:29:02  Woody
 * ARCH_SECURITY verwendet
 *
 * Revision 1.8  1997/10/02 14:30:40  Woody
 * RemoveEPEntry()
 *
 * Revision 1.7  1997/08/29 18:59:53  Woody
 * FPs auch in VC-Raeumen moeglich! Anmerkungen weiter unten lesen!!!
 *
 * Revision 1.6  1997/08/27 14:30:05  Woody
 * ChangeEpObject(), FP-Scripter werden geloggt, QueryApCost()
 *
 * Revision 1.5  1997/03/15 22:53:43  Woody
 * prepared for revision control
 *
 * Revision 1.4  96-10-14  Woody
 * Mitloggen von Add/Remove
 *
 * Revision 1.3  96-10-12  Woody
 * dump schreibt auch ins Dumpfile hinaus
 *
 * Revision 1.2  96-10-10  Woody
 * QueryEPEntry()
 *
 * Revision 1.1  96-10-06  Woody
 * Auslesen des Average Wertes aus nem File
 *
 * Revision 1.0  96-09-25  Woody
 * Erste Implementation
 *
 */


/*
 *
 * Forscherpunkte in VC-Raeumen (virtual compiler):
 * ------------------------------------------------
 *
 * Es ist moeglich, in VC-Raeumen FPs zu verteilen, sogar wenn der
 * Filename spielerabhaengig ist. Allerdings sind hierbei unbedingt
 * einige Dinge zu beachten:
 *
 * 1. In der FP-Liste steht der Filename des VIRTUELLEN Raums, nicht
 *    der des Standard-Objekts, das gecloned wird. Ist ja auch sinnvoll,
 *    schliesslich kommt es ja oft genug vor, dass EIN Standardobjekt
 *    dutzendfach fuer verschiedenste Raeume gecloned wird. Es KANN zwar
 *    auch Sinn machen, das Std-Objekt in der FP-Liste zu haben, aber man
 *    sollte UNBEDINGT DAVON ABSEHEN, es sei denn man hat echt Durchblick
 *    bei der Vorgangsweise des Masters bei VCs. So - don't do it.
 *
 * 2. Ist der Filename des VC-Raums vom Spieler abhaengig, so kann man
 *    NUR DANN Forscherpunkte vergeben, wenn die getuid() des Spielers
 *    das Unterscheidungsmerkmal fuer den Filenamen bildet und zudem
 *    dieser einer bestimmten Richtlinie folgt, die besagt, dass die
 *    getuid die laengste alphanumerische Folge nach dem ersten Doppel-
 *    punkt im Filenamen ist - kurzum, dem Muster .*:[a-z]{3,}.*
 *    genuegt (mind. 3 Zeichen lang und aus a..z bestehend).
 *
 *    Die (potentielle) Spieler-UID wird dann durch den String @@tp_uid@@
 *    ersetzt - in dieser Form sollte der Raum bzw. das Objekt auch in der
 *    FP-Liste eingetragen sein, also beispielsweise:
 *    "/d/ebene/woody/vwelt/room/irrgarten:@@tp_uid@@:4,6"
 *
 *    Es ist WICHTIG, dass man in allen anderen Faellen AUF KEINEN FALL
 *    einen Doppelpunkt als Teil des Filenamens verwenden sollte - es
 *    gibt genug andere potentielle Trennzeichen (nichtsdestotrotz ist
 *    die Verwendung unbedenklich, solange der Filename nicht der oben
 *    angegebenen regular expression matched.
 *
 *    Hinzufuegen und Entfernen von FPs in solchen VCs geht wie gewohnt
 *    mit dem EP-Tool, allerdings NUR, wenn man sich in seinem 'eigenen'
 *    VC-Raum befindet (zB. "/d/ebene/woody/vwelt/room/irrgarten:woody:4,6",
 *    nicht aber ".....:avalon:4,6"). Am besten verifiziert man den eben
 *    eingetragenen FP mit 'eps' anhand seiner eindeutigen Nummer (hier
 *    sollte sich im Filenamen dann @@tp_uid@@ wiederfinden) und loescht
 *    ihn bei Irrtum gegebenenfalls sofort wieder.
 *
 *    Woody.- (fuer etwaige Rueckfragen zur Verfuegung stehend)
 *
 */


#include <config.h>
#include <wizlevels.h>
#include "exploration.h"
#include <properties.h>

/* Im save-file gespeicherte Var's */

private mapping obs;
private int cnt, epmax;
private string alloc;
private mapping oracle; // ([ "frage":"pfad",({ epnr1,epnr2...}) ])
private string *oracle_trusted;

/* private lokale Var's */

private static int average;
private static mapping lastfound;
private static string vc_ob, vc_ob1;
private static string vc_name;

private static int changed;          // Orakel wurde geaendert
private static int oracle_changed;   // Orakel wird gerade geaendert
private static object oracle_ob;     // Orakel Daten

private int allowed()
{
  if ( previous_object() && this_interactive() && IS_ARCH(this_interactive()) )
    return 1;
  return 0;
}

void reset()
{
  // ::reset();
  if (find_call_out("RecalcAverage") == -1 )
  {
    call_out("RecalcAverage", 1);
  }
  if(changed)
  {
    call_out("CalcOracle",30,0);
    changed = 0;
  }
}

void create()
{
  // ::create();
  seteuid( getuid( this_object() ));
  lastfound = ([]);
  obs = ([]);
  oracle = ([]);
  cnt = epmax = 0;
  alloc = "";
  oracle_trusted=({ });
  if (!restore_object(EPSAVEFILE)) {
    lastfound = ([]);
    obs = ([]);
    oracle = ([]);
    cnt = epmax = 0;
    alloc = "";
    oracle_trusted=({ });
    save_object(EPSAVEFILE);
  }
  average=vAverage; // Default-Wert wird mittels RecalcAverage aktualisiert...
  reset();
}

nomask remove()
{
  save_object(EPSAVEFILE);
  destruct( this_object() );
  return 1;
}

nomask void RecalcAverage()
{
  int i, avg;
  object *u;

  while (remove_call_out ("RecalcAverage") >= 0);
  call_out ("RecalcAverage", TIME_UPD_AVG);

  u=users();
  if (!sizeof(u)) return 0;
  avg=vPlayerNum*vAverage;  // Grundwert initialisieren (vPlayer)
  for (i=sizeof(u)-1; i>=0; i--)
  {
    avg += u[i]->QueryProp(P_EP_NUM);
  }
  avg /= (sizeof(u)+vPlayerNum); // Anzahl Spieler um vPlayer erhoehen
  average = avg;
}

// prueft ob ein Objekt als FP-Objekt gueltig ist,
// und gibt wenn ja dessen Filename zurueck, ansonsten 0.
private string validOb (mixed ob)
{
  string fn, fpart;

  if (!objectp(ob)) return 0;

  fn = explode(object_name(ob),"#")[0];
  fpart = explode(fn,"/")[<1];
  // Beispiel: "/d/ebene/obj/ding#123" -> fn="/d/ebene/obj/ding", fpart="ding"

  // Keine FPs an Spielern
  if (query_once_interactive(ob)) return 0;

  // VC-Sonderbehandlung
  if ((file_size(fn+".c") < 0) && this_interactive() &&
      (strstr(fpart, ":"+getuid(this_interactive())) >= 0) )
    fn = fn[0..<sizeof(fpart)+1] +
      regreplace(fpart, ":"+getuid(this_interactive()), ":@@tp_uid@@", 0);
  else
    if ((file_size(fn+".c") <= 0) &&
        this_player() && (strstr(fpart, getuid(this_player())) >= 0))
      return 0;

  return fn;
}

nomask int AddEPObject (object ob, int type, mixed keys, int eps)
{
  string fn, s;

  // nur Erzmagier duerfen EP-Objekte hinzufuegen
  if (!allowed())
    return EPERR_NOT_ARCH;

  // und selbst die duerfen nur gueltige Objekte verwenden
  if (!(fn = validOb(ob)))
    return EPERR_INVALID_OB;

  // 1. Der Typ muss gueltig sein, und 2. Zuviele oder auch
  // zuwenige (i.e., negative) EP darf man auch nicht vergeben, ganz klar.
  if ( (type < 0) || (type > EP_TYP_MAX) ||
       (eps < 0)  || (eps > EPS_MAX) )
    return EPERR_INVALID_ARG;

  // Vielleicht gibt man uns ja nen String. Aber wir brauchen nen Array...
  if (stringp(keys))
    keys = ({ keys });

  fn = type + ":" + fn;

  // Hmm... gab's die Kombination Typ:Objekt vielleicht schon?
  if (member(obs, fn))
  {
    // Jau... neue Keys hinzufuegen, wenn eps != 0, diese neu setzen.
    obs[fn, EPM_KEY] += keys;
    if (eps) {
      epmax -= obs[fn, EPM_EPS];
      obs[fn, EPM_EPS] = eps;
      epmax += eps;
    }
  }
  else
  {
    int nr, i;

    nr = 0;
    while (test_bit(alloc, nr)) nr++;

    obs += ([ fn : keys; nr; eps ]);
    cnt++;
    epmax += eps;
    alloc = set_bit (alloc, nr);
  }
  changed = 1;
  log_file (EPLOGFILE,
    "--- Added EP by "+capitalize(getuid(this_interactive()))+" ("+
    dtime(time())+")\n"+
    sprintf(
       "%-4s  #%04d  %-40s  [%2d]  %s\n",
        EP_TYP_STR[to_int(fn[0..0])], obs [fn,EPM_NUM], fn,
        obs [fn,EPM_EPS], implode( obs [fn,EPM_KEY], ", ")));
  save_object (EPSAVEFILE);
  return cnt;
}

nomask int RemoveEPObject (object ob, int type)
{
  string fn, fpart;

  if (!allowed())
    return EPERR_NOT_ARCH;

  if (!objectp(ob))
    return EPERR_INVALID_OB;

  if ((type < 0) || (type > EP_TYP_MAX))
    return EPERR_INVALID_ARG;

  fn = explode(object_name(ob),"#")[0];
  fpart = explode(fn,"/")[<1];

  if (this_interactive() &&
      (strstr(fpart, ":"+getuid(this_interactive())) >= 0) )
    fn = fn[0..<sizeof(fpart)+1] +
      regreplace(fpart, ":"+getuid(this_interactive()), ":@@tp_uid@@", 0);

  fn = type + ":" + fn;

  if (!member (obs, fn))
    return EPERR_NO_ENTRY;

  alloc = clear_bit (alloc, obs [fn, EPM_NUM]);

  log_file (EPLOGFILE,
    "--- Removed EP by "+capitalize(getuid(this_interactive()))+" ("+
    dtime(time())+")\n"+
    sprintf(
       "%-4s  #%04d  %-40s  [%2d]  %s\n",
        EP_TYP_STR[to_int(fn[0..0])], obs [fn,EPM_NUM], fn,
        obs [fn,EPM_EPS], implode( obs [fn,EPM_KEY], ", ")));

  cnt--;
  epmax -= obs [fn, EPM_EPS];
  obs = m_delete (obs, fn);
  changed = 1;
  save_object (EPSAVEFILE);
  return cnt;
}

nomask int ChangeEPObject (object ob, int type, mixed keys, int eps)
{
  string fn;

  if (!allowed())
    return EPERR_NOT_ARCH;

  if (!objectp(ob))
    return EPERR_INVALID_OB;

  if ((type < 0) || (type > EP_TYP_MAX))
    return EPERR_INVALID_ARG;

  fn = explode(object_name(ob),"#")[0];
  fn = type + ":" + fn;

  if (!member (obs, fn))
    return EPERR_NO_ENTRY;

  // Vielleicht gibt man uns ja nen String. Aber wir brauchen nen Array...
  if (stringp(keys))
    keys = ({ keys });

  if (!pointerp(keys))
    return EPERR_INVALID_ARG;

  epmax -= obs[fn,EPM_EPS];
  obs[fn,EPM_KEY] = keys;
  obs[fn,EPM_EPS] = eps;
  epmax += obs [fn, EPM_EPS];

  log_file (EPLOGFILE,
    "--- Changed EP by "+capitalize(getuid(this_interactive()))+" ("+
    dtime(time())+")\n"+
    sprintf(
       "%-4s  #%04d  %-40s  [%2d]  %s\n",
        EP_TYP_STR[to_int(fn[0..0])], obs [fn,EPM_NUM], fn,
        obs [fn,EPM_EPS], implode( obs [fn,EPM_KEY], ", ")));

  changed = 1;
  save_object (EPSAVEFILE);
  return cnt;
}

nomask mixed QueryEPObject (object ob, int type)
{
  string fn;

  if (!allowed())
    return EPERR_NOT_ARCH;

  if (!(fn = validOb(ob)))
    return EPERR_INVALID_OB;

  fn = type + ":" + fn;

  if (!member(obs, fn))
    return 0;

  return ({ obs [fn,EPM_KEY], obs [fn,EPM_NUM], obs [fn,EPM_EPS] });
}

nomask mixed QueryEPEntry (int n)
{
  int i;
  string fn;
  mixed *keys;

  if (!allowed())
    return EPERR_NOT_ARCH;

  keys=m_indices(obs);
  for (i=sizeof(keys)-1; i>=0; i--)
  {
    if (obs[keys[i],EPM_NUM]==n)
      return ({ keys[i][2..], to_int(keys[i][0..0]),
                obs [keys[i],EPM_KEY], obs [keys[i],EPM_EPS] });
  }
  return 0;
}

nomask mixed RemoveEPEntry (int n)
{
  int i;
  string fn;
  mixed *keys;

  if (!allowed())
    return EPERR_NOT_ARCH;

  fn=0;
  keys=m_indices(obs);
  for (i=sizeof(keys)-1; i>=0; i--)
    if (obs[keys[i],EPM_NUM]==n)
      fn = keys[i];

  if (!fn)
    return EPERR_NO_ENTRY;

  alloc = clear_bit (alloc, obs [fn, EPM_NUM]);

  log_file (EPLOGFILE,
    "--- Removed EP by "+capitalize(getuid(this_interactive()))+" ("+
    dtime(time())+")\n"+
    sprintf(
       "%-4s  #%04d  %-40s  [%2d]  %s\n",
        EP_TYP_STR[to_int(fn[0..0])], obs [fn,EPM_NUM], fn,
        obs [fn,EPM_EPS], implode( obs [fn,EPM_KEY], ", ")));

  cnt--;
  epmax -= obs [fn, EPM_EPS];
  obs = m_delete (obs, fn);
  changed = 1;
  save_object (EPSAVEFILE);
  return cnt;
}

nomask void PrepareVCQuery(string file)
{
  string path, *parts, fname;

  vc_ob = 0;

  if (!previous_object() || !stringp(file))
    return;

  parts = efun::explode(object_name(previous_object()),"/");

  if (parts[<1] == "virtual_compiler") {
    path = implode(parts[0..<2],"/")+"/";
    if (file_size(path+file+".c") < 0) {
      file=regreplace (file, "(.*:)[A-Za-z][A-Za-z][A-Za-z][A-Za-z]*(.*)",
          "\\1@@tp_uid@@\\2", 0);
      vc_ob = path+file;
    }
  }
}

nomask string *QueryExplore (int type)
{
  string fn, po;

  if (!previous_object())
    return 0;

  fn = type + ":" +
       explode( po=object_name( previous_object()), "#") [0];

  if (!member (obs, fn))        // Kein FP im previous_object()
  {
    if (!vc_ob)                 // VC steht auch keiner zu pruefen an
      return 0;                    // -> Keine FPs
    else {
      if (!vc_name) {           // Hmmm. Erste Anfrage des VC-Objekts
        vc_name=po;                // -> Name des eigtl. Objekts merken
        vc_ob1=vc_ob;              //    ausserdem noch vc_ob merken
      } else if (vc_name!=po) { // Erneute Anfrage - evtl. neues Objekt
        if (vc_ob!=vc_ob1) {       // jetzt anderer VC ?
          vc_name=po;                 // JA -> neuen Namen merken
          vc_ob1=vc_ob;
        } else {
          vc_ob=vc_ob1=vc_name=0;     // NEIN -> Alles loeschen
          return 0;
        }
      }
      if (!member(obs, fn = type+":"+vc_ob))
        return 0;
    }
  }
  else
    vc_ob=vc_ob1=vc_name=0;     // FP im previous_object() -> kein VC

  return obs [fn,EPM_KEY];
}

nomask int QueryMaxEP()
{
  return epmax;
}

nomask int QueryMaxEPObjects()
{
  return cnt;
}

nomask int QueryAverage()
{
  return average;
}

nomask int QueryApCost()
{
  return EP_PER_AP;
}

static int check_arch(object u)
{
  return query_wiz_level(u) >= ARCH_LVL;
}

nomask int GiveExplorationPoint (string key, int type)
{
  string fn;
  string ep;

  if (!previous_object() || !this_interactive() || !this_player() ||
       this_player() != this_interactive() ||
       this_player()->QueryProp(P_KILLS))
    return 0;

  fn = type + ":" +
       explode( object_name( previous_object()), "#") [0];

  fn = regreplace(fn, ":"+getuid(this_player()), ":@@tp_uid@@", 0);

  if (!member(obs,fn))
    return 0;

  if (member(obs[fn,EPM_KEY], key) < 0)
    return 0;

  ep = this_player()->QueryProp(P_EP);
  if (!stringp(ep)) ep = "";

  // Pruefen, ob Spieler diesen FP schon vorher mal gefunden hat.
  if (test_bit (ep, obs[fn,EPM_NUM]))
    return 0;

  if (!member(lastfound,getuid(this_player())))
    lastfound += ([ getuid(this_player()) : time(); 1 ]);
  else if (time() <= lastfound[getuid(this_player()),0]+LF_TIME)
    lastfound[getuid(this_player()),1]++;
  else
    lastfound[getuid(this_player()),1]=1;
  lastfound[getuid(this_player()),0]=time();

  if (lastfound[getuid(this_player()),1] >= LF_WARN)
  {
    object *u;
    u=filter(users(), "check_arch");
    map(u, #'tell_object,
        "**** FP-LISTEN/SCRIPTVERDACHT: "+
        capitalize(getuid(this_player()))+" ****\n");
    write_file (LF_LOG,
      sprintf("%s:\n"
          "    %s: %s (%s)\n",
          dtime(time()), capitalize(getuid(this_player())), fn[2..], key));
    write_file (LF_LOG_NAME+getuid(this_player()),
      sprintf("%s: EP_TOO_FAST\n    %s  (%s)\n",
      dtime(time()), fn[2..], key));
  }

  ep = set_bit (ep, obs[fn,EPM_NUM]);
  this_player()->SetProp(P_EP, ep);
  this_player()->AddEP( obs[fn,EPM_EPS] );
  return 1;
}

nomask int QueryExplorationPoints (object ob)
{
  return ob->QueryProp(P_EP_NUM);
}

int compare(string s1, string s2)
{
  return s1[2..] < s2[2..];
}

nomask int dumpEP ()
{
  string fn, fn2, s, s2;
  mixed *keys;
  int i;

  if (!allowed())
    return EPERR_NOT_ARCH;

  // keys=sort_array(m_indices(obs), #'<);
  keys=sort_array(m_indices(obs), "compare");
  s = "type bit#  file name of object          EPs   Keys\n"
      "----------------------------------------------------------------------------\n";
  s2= "type bit#  file name of object                                 EPs   Keys\n"
      "----------------------------------------------------------------------------------------------------------\n";
  for (i=sizeof(keys)-1; i>=0; i--)
  {
    fn = keys[i];
    fn2 = fn[2..];
    if (sizeof(fn2) > 27) fn2=".."+fn2[<25..];
    s += sprintf(
       // "%-4s  %3d  %:-27s  [%2d]  ({ %.25s })\n",
       "%-4s %4d  %:-27s  [%2d]  %.31s\n",
        EP_TYP_STR[to_int(fn[0..0])], obs[fn,EPM_NUM], fn2,
        obs[fn,EPM_EPS], implode( obs[fn,EPM_KEY], ", "));
    s2 += sprintf(
       "%-4s %4d  %-50s  [%2d]  %s\n",
        EP_TYP_STR[to_int(fn[0..0])], obs[fn,EPM_NUM], fn[2..],
        obs[fn,EPM_EPS], implode( obs[fn,EPM_KEY], ", "));
  }
  catch(rm(EPDUMPFILE));
  write_file(EPDUMPFILE, s);
  catch(rm(EPDUMPFILE + ".all"));
  write_file(EPDUMPFILE + ".all", s2);
  return 1;
}

nomask varargs int QueryPercent(string was, mixed pl)
{
  string ep,str;
  int i,perc;
  int *all;
  object po;

  po=previous_object();
  if (po)
    str=explode(object_name(po),"#")[0]+".c";
  else
    str="\n";

  if (!allowed() && member(oracle_trusted,str) == -1)
    return EPERR_NOT_ARCH;

  if (oracle_changed)                       // derzeit keine Abfragen erlaubt
    return EPERR_NOT_ARCH;

  if(stringp(pl)) pl=find_player(pl) || find_netdead(pl);

  if(!objectp(pl))
    return EPERR_INVALID_ARG;

  if(member(m_indices(oracle),was) < 0)
    return EPERR_NO_ENTRY;

  all=oracle[was,1];

  if(sizeof(all) < 1)
    return EPERR_NO_ENTRY;

  ep=pl->QueryProp(P_EP);
  perc=0;
  for(i=0;i<sizeof(all);i++) {
    if(test_bit(ep,all[i])) {
      perc++;
    }
  }
  return perc*100/sizeof(all);
}

nomask mixed QueryQuestions()
{
  object po;
  string *ss,str;

  po=previous_object();
  if (po)
    str=explode(object_name(po),"#")[0]+".c";
  else
    str="\n";

  if (!allowed() && member(oracle_trusted,str) == -1)
    return EPERR_NOT_ARCH;

  ss=m_indices(oracle);
  return ss;
}

int CalcOracle(int von) {
  int j,i,bis,bis_max;
  string *ora_ind;
  string *obs_ind;
  string ss;

  if(previous_object() != this_object() && !allowed())
    return EPERR_NOT_ARCH;

  if(this_player() && interactive(this_player()))
    oracle_ob=this_player();
  else
    oracle_ob=0;

  oracle_changed=1;  //waehrend der Berechnung keine Fragen an das Orakel

  while(find_call_out("CalcOracle") != -1)
    remove_call_out("CalcOracle");

  ora_ind=m_indices(oracle);
  bis_max=sizeof(ora_ind);
  obs_ind=m_indices(obs);

  if(von==0) {
    for(i=0;i<bis_max;i++) {
      oracle[ora_ind[i],1]=({ });
    }
  }
  bis=von+5;
  if(bis > bis_max)
    bis=bis_max;
  for(j=von;j<bis;j++) {
    ss=oracle[ora_ind[j],0];                // pfadname der frage
    for(i=0;i<sizeof(obs_ind);i++) {
      if(strstr(obs_ind[i],ss) != -1) {
                                            // gespeichert wird nr des FP
        oracle[ora_ind[j],1]=oracle[ora_ind[j],1]+({ obs[obs_ind[i],EPM_NUM] });
      }
    }
  }
  if(bis < bis_max)
  {
    call_out("CalcOracle",10,bis);
  }
  else
  {
    oracle_changed=0;
    if(objectp(oracle_ob) && interactive(oracle_ob))
      tell_object(oracle_ob,"*** Berechnung für das FP-Orakel fertig!\n");
  }
  return 1;
}

nomask int AddOracleString(string frage, string pfad)
{
  int i;
  string *ora_ind;
  string *obs_ind;
  string ss;

  if (!allowed())              // tja...
    return EPERR_NOT_ARCH;

  if(!stringp(frage) || !stringp(pfad))
    return EPERR_INVALID_ARG;

  if(sizeof(frage) < 3)        // die frage muss mindestens 3 zeichen lang sein
    return EPERR_INVALID_ARG;

  if(sizeof(pfad) < 6)        // der pfad muss mindestens 10 zeichen lang sein
    return EPERR_INVALID_ARG;

  if(file_size(pfad) != -2)    // es muss ein pfad sein
    return EPERR_INVALID_ARG;

  ora_ind=m_indices(oracle);

  if (member(ora_ind,frage) != -1)
    return EPERR_INVALID_ARG;

  for(i=0;i<sizeof(ora_ind);i++) {
    if(strstr(oracle[ora_ind[i],0],pfad) != -1)
      return EPERR_INVALID_ARG;
  }
  oracle+=([ frage:pfad;({ }) ]);

  obs_ind=m_indices(obs);
                                                  // gleich berechnen :-))
  for(i=0;i<sizeof(obs_ind);i++) {
    if(strstr(obs_ind[i],pfad) != -1) {
                                                  // gespeichert wird nr des FP
      oracle[frage,1]=oracle[frage,1]+({ obs[obs_ind[i],EPM_NUM] });
    }
  }

  save_object (EPSAVEFILE);
  return 1;
}

nomask mixed RemoveOracleString(string frage)
{
  string ss;

  if (!allowed())              // tja...
    return EPERR_NOT_ARCH;

  if(!stringp(frage))
    return EPERR_INVALID_ARG;

  if (member(m_indices(oracle),frage) == -1)
    return EPERR_INVALID_ARG;

  ss=oracle[frage,0];
  oracle=m_delete(oracle,frage);

  save_object (EPSAVEFILE);
  return ss;
}

nomask mixed ShowOracleString()
{
  string *ora_ind;
  string *ss;
  int i;

  if (!allowed())              // tja...
    return EPERR_NOT_ARCH;

  ora_ind=m_indices(oracle);
  ss=({ });

  for(i=0;i<sizeof(ora_ind);i++)
  {
    ss+=({ sprintf("%-15s: (%3d) %s",ora_ind[i],sizeof(oracle[ora_ind[i],1]),
                                     oracle[ora_ind[i],0] ) });
  }

  return ss;
}

nomask mixed ShowOracleTrusted()
{
  string *ss;
  int i;

  if (!allowed())              // tja...
    return EPERR_NOT_ARCH;

  ss=({ });

  for(i=0;i<sizeof(oracle_trusted);i++)
  {
    ss+=({ sprintf("%s",oracle_trusted[i]) });
  }

  return ss;
}

nomask int AddOracleTrusted(string str)
{
  int i;
  string ss;

  if (!allowed())              // tja...
    return EPERR_NOT_ARCH;

  if(!stringp(str))
    return EPERR_INVALID_ARG;

  if(file_size(str) < 1)    // es muss ein pfad sein
    return EPERR_INVALID_ARG;

  if (member(oracle_trusted,str) != -1)
    return EPERR_INVALID_ARG;

  oracle_trusted+=({ str });

  save_object (EPSAVEFILE);
  return sizeof(oracle_trusted);
}

nomask int RemoveOracleTrusted(string str)
{
  int i;
  string ss;

  if (!allowed())              // tja...
    return EPERR_NOT_ARCH;

  if(!stringp(str))
    return EPERR_INVALID_ARG;

  if(file_size(str) < 1)    // es muss ein pfad sein
    return EPERR_INVALID_ARG;

  if (member(oracle_trusted,str) == -1)
    return EPERR_INVALID_ARG;

  oracle_trusted-=({ str });

  save_object (EPSAVEFILE);
  return sizeof(oracle_trusted);
}
