/*
 * Tamedhon MUDlib
 * ---
 * /secure/npcmaster.c
 * NPC Master - verwaltet die Erst-Killer-Punkte
 * Maintained in Tamedhon by Querolin & TamTam
 * NPC Score Master, maintained by Woody
 * ---
 * $Date: 2008/03/09 20:00:00 $
 * $Revision: 2.0 $
 * $Log: npcmaster.c,v $
 *
 * Revision 2.0  2008/03/09 20:00:00  TamTam
 * SL und TD Versionen zusammengefuehrt wegen einigen Verbesserungen 1.6-1.10
 * Erst-Killer-Orakel eingebaut. Neues Savefileformat!
 * Kein Balance-Team Handling. *_SECURITY gibts hier nicht -> raus
 *
 * Revision 1.10  2002/04/30 15:45:00  Woody
 * Bugfixes in ChangeScore()
 *
 * Revision 1.9  2002/04/30 13:25:18  Feyaria
 * ChangeScore hinzugefuegt
 *
 * Revision 1.8  1999/05/22 01:36:59  Woody
 * Logging eingebaut
 *
 * Revision 1.7  1999/05/22 00:55:27  Woody
 * Balance-Team Handling, allowed(), QueryDump()
 *
 * Revision 1.6  1998/09/17 09:06:26  Woody
 * NPCs aus /players/ werden nicht mehr aufgenommen, QueryScore() neu
 *
 * Revision 1.5  1997/06/28 20:03:02  Woody
 * Bugfix, wenn this_player() in QueryNPC() nicht definiert war
 * (wie zum Beispiel beim Bumerang -> dann P_KILLER verwenden)
 *
 * Revision 1.4  1997/06/08 02:01:33  Woody
 * Bugfix in QueryNPC()
 *
 * Revision 1.3  1997/06/01 15:56:41  Woody
 * Queue Handling
 *
 * Revision 1.2  1997/03/15 23:27:11  Woody
 * prepared for revision control
 *
 * Revision 1.1  96-11-13  Woody
 * Abfrage, ob Testspieler, oder Filename /test/ enthaelt
 *
 * Revision 1.0  96-10-05  Woody
 * Erste Implementation - uebernommen von der MG-mudlib
 */

#include "npcmaster.h"
#include <wizlevels.h>
#include <properties.h>


// - prototypes --------------------------------------------------------------

void make_num(string key);
public string *QueryBalance();
public int AddToBalance(string who);
public int RemoveFromBalance(string who);
static int allowed();
varargs mixed QueryNPC(int score);
int SetScore(mixed key, int score);
mixed DelLast();
static int DumpNPCs();
mixed QueryDump();
mixed *QueryNPCbyNumber(int num);
mixed RemoveQueued(string key);
mixed RemoveQueuedRegexp(string pattern);
mixed ApproveQueued(string key);
mixed ApproveQueuedRegexp(string pattern);
varargs mapping QueryQueued(mixed pattern);
mapping QueryPending();
int Recalculate(string s);

// - data --------------------------------------------------------------------

// saved
private int lastNum;
private mapping npcs;                   // NPC Mapping
private mapping queued;                 // noch zu bestaetigende Scores
private mapping pending;                // noch zu vergebende Scores
private mapping oracle; // ([ "frage":"pfad",({ epnr1,epnr2...}) ])
private string *oracle_trusted;

// static (non-saved)
private static mapping by_num;          // Hilfsmapping (by number)
private static string dump;             // dump string
private static int ochanged;            // Orakel wurde geaendert
private static int oracle_changed;      // Orakel wird gerade geaendert
private static object oracle_ob;        // Orakel Daten


// - functions ---------------------------------------------------------------

void create()
{
  seteuid(getuid());
  npcs=m_allocate(0,2);
  queued=m_allocate(0,1);
  pending=m_allocate(0,1);
  oracle = ([]);
  oracle_trusted=({ });
  if (!restore_object(NPCSAVEFILE))
  {
    lastNum=0;
    npcs=m_allocate(0,2);
    queued=m_allocate(0,1);
    pending=m_allocate(0,1);
    oracle = ([]);
    oracle_trusted=({ });
  }
  by_num = m_allocate(0,2);
  dump = read_file(NPCDUMPFILE);
  filter_indices(npcs, #'make_num/*'*/);
}

void make_num(string key)
{
  by_num += ([ npcs[key,NPC_NUMBER] : key; npcs[key,NPC_SCORE] ]);
}

static int allowed()
{
  if ( previous_object() && this_interactive() && IS_ARCH(this_interactive()) )
    return 1;
  return 0;
}

void reset()
{
  string who;
  mixed u,ind;
  int i,j,changed;

  // check for all users logged in
  u=users();
  for (j=sizeof(u)-1; j>=0; j--)
  {
    who=getuid(u[j]);
    if (pointerp((pending[who])) && sizeof(pending[who]))
    {
      ind=pending[who];
      for (i=sizeof(ind)-1; i>=0; i--)
      {
        if (member(npcs,ind[i])) {              // has been confirmed
          u[j]->AddScore(npcs[ind[i],NPC_NUMBER]);
          pending[who] -= ({ ind[i] });
          changed=1;
        }
        else if (!member(queued,ind[i])) {      // has been discarded
          pending[who] -= ({ ind[i] });
          changed=1;
        }
      }
      if (!sizeof(pending[who]))                // nothing left
        efun::m_delete(pending,who);
    }
    else if (member(pending,who)) {
      efun::m_delete(pending,who);      // discard trash data
      changed=1;                        // (should never be executed)
    }
  }

  // check for non-existing players
  u=m_indices(pending);
  for (j=sizeof(u)-1; j>=0; j--)
    if (file_size("/save/"+u[j][0..0]+"/"+u[j]+".o")<=0) {
      efun::m_delete(pending,u[j]);
      changed=1;
    }

  // save if necessary
  if (changed) save_object(NPCSAVEFILE);

  // Orakel Fragen neu berechnen?
  if(ochanged)
  {
    call_out("CalcOracle",30,0);
    ochanged = 0;
  }
}

varargs mixed QueryNPC(int score)
{
  string key, who;
  object ob,killer;
  mixed ind;
  int i;

  if (!(ob=previous_object()))
    return NPC_INVALID_ARG;

  killer=this_player();
  if (!killer) killer=ob->QueryProp(P_KILLER);
  if (!killer || !interactive(killer)) return NPC_INVALID_ARG;

  // FIRST OF ALL process pending scores!
  who=getuid(killer);
  if (pointerp((ind=pending[who])) && sizeof(ind))
  {
    for (i=sizeof(ind)-1; i>=0; i--)
    {
      if (member(npcs,ind[i])) {
        killer->AddScore(npcs[ind[i],NPC_NUMBER]);
        pending[who] -= ({ ind[i] });
      }
      else if (!member(queued,ind[i]))
        pending[who] -= ({ ind[i] });
    }
    if (!sizeof(pending[who])) efun::m_delete(pending,who);
  }

  key=explode(object_name(previous_object()),"#")[0];
  if (member(npcs,key))
    return ({npcs[key,NPC_NUMBER],npcs[key,NPC_SCORE]});

  if (killer->QueryProp(P_TESTPLAYER) || IS_WIZARD(killer))
  {
    pending=m_delete(pending,who);      // evtl. aus mapping raus
    return NPC_INVALID_ARG;
  }
  if ((strstr(key, "/test/") != -1) || (key[0..8]=="/players/"))
    return NPC_INVALID_ARG;
  if (score<=0 ||
      member(inherit_list(previous_object()),"std/living/life.c") < 0)
    return NPC_INVALID_ARG;

  queued[key]=score;
  if (member(pending,who)) {
    if (member(pending[who],key) == -1)
      pending[who] += ({ key });
  } else
    pending[who] = ({ key });

  save_object(NPCSAVEFILE);
  return NPC_QUEUED;

  /*
  npcs[key,NPC_SCORE]=score;
  npcs[key,NPC_NUMBER]=++lastNum;
  by_num += ([lastNum:key;score]);
  save_object(NPCSAVEFILE);
  call_out("DumpNPCs",0);
  return ({lastNum,score});
  */
}

int QueryScore(mixed fn)
{
  if (objectp(fn)) fn=explode(object_name(fn),"#")[0];
  if (!stringp(fn)) return NPC_INVALID_ARG;
  if (member(queued,fn)) return NPC_QUEUED;
  if (!member(npcs,fn)) return 0;
  return npcs[fn,NPC_SCORE];
}

int SetScore(mixed key, int score)
{
  int x;
  if (!allowed()) return NPC_NO_PERMISSION;
  if (intp(key))
  {
    string *ind;
    ind=m_indices(npcs);
    for (x=sizeof(npcs)-1; x>=0; x--)
      if (npcs[ind[x],NPC_NUMBER] == key)
      {
        key=ind[x];
        break;
      }
      if (intp(key)) return NPC_INVALID_ARG;
  }
  if (!stringp(key)) return NPC_INVALID_ARG;
  if (!member(npcs,key))
    npcs[key,NPC_NUMBER]=++lastNum;
  npcs[key,NPC_SCORE]=score;
  by_num += ([npcs[key,NPC_NUMBER]:key;score]);

  log_file(NPCLOGFILE,
      "--- SetScore called by "+capitalize(getuid(this_interactive()))+" ("+
      dtime(time())+")\n"+
      sprintf(
        "#%04d  [%2d]  %-66s\n",
        npcs[key,NPC_NUMBER], score, key));

  queued = m_delete(queued,key);
  save_object(NPCSAVEFILE);
  DumpNPCs();
  ochanged = 1;  // Oraklefragen neu berechnen
  return 1;
}

int ChangeScore(mixed old, mixed new)
{
  int nr, score;

  if (!allowed()) return NPC_NO_PERMISSION;
  if (!member(npcs,old) || member(npcs,new) || (old == new))
    return NPC_INVALID_ARG;

  nr = npcs[old,NPC_NUMBER];
  score = npcs[old,NPC_SCORE];
  npcs = m_delete(npcs, old);
  npcs[new,NPC_NUMBER] = nr;
  npcs[new,NPC_SCORE] = score;
  by_num += ([ nr : new; score ]);

  log_file(NPCLOGFILE,
      "--- ChangeScore called by "+capitalize(getuid(this_interactive()))+" ("+
      dtime(time())+")\n"+ sprintf( "#%04d  %s -> %s\n", nr, old, new));
  save_object(NPCSAVEFILE);
  DumpNPCs();
  ochanged = 1;  // Oraklefragen neu berechnen
  return 1;
}

mixed DelLast()
{
  string s;
  if (!allowed()) return NPC_NO_PERMISSION;
  s="Deleted #"+lastNum+" ["+by_num[lastNum]+":"+
    by_num[lastNum,NPC_SCORE]+"]\n";
  npcs = m_delete (npcs, by_num[lastNum]);
  by_num = m_delete (by_num, lastNum);
  lastNum--;
  log_file(NPCLOGFILE,
      "--- DelLast called by "+capitalize(getuid(this_interactive()))+" ("+
      dtime(time())+")\n"+s);
  save_object(NPCSAVEFILE);
  call_out("DumpNPCs",0);
  ochanged = 1;  // Oraklefragen neu berechnen
  return s;
}

static int DumpNPCs()
{
  mixed *keys;
  int i, max;

  while (remove_call_out("DumpNPCs") != -1) /* continue */ ;

  dump="";
  keys=m_indices(npcs);
  keys=sort_array(keys, #'<);
  rm(NPCDUMPFILE);
  for (i=sizeof(keys)-1, max=0;i>=0;i--) {
    dump += sprintf("%5d %4d %s\n",
        npcs[keys[i],NPC_NUMBER], npcs[keys[i], NPC_SCORE], keys[i]);
    max += npcs[keys[i],NPC_SCORE];
  }
  dump += sprintf("====================\n"+"Gesamt: %d Punkte\n",max);
  write_file(NPCDUMPFILE, dump);
  return 1;
}

mixed QueryDump()
{
  if (!allowed()) return NPC_NO_PERMISSION;
  return dump;
}

mixed *QueryNPCbyNumber(int num)
{
  if (by_num[num])
    return ({num,by_num[num,NPC_SCORE],by_num[num,NPC_NUMBER]});

  return 0;
}

mixed RemoveQueued(string key)
{
  if (!allowed()) return NPC_NO_PERMISSION;
  log_file(NPCLOGFILE,
      "--- RemoveQueued called by "+
      capitalize(getuid(this_interactive()))+" ("+dtime(time())+")\n"+
      sprintf("#none  [%2d]  %-66s\n", queued[key], key));
  queued = m_delete(queued,key);
  save_object(NPCSAVEFILE);
  return copy(queued);
}

mixed RemoveQueuedRegexp(string pattern)
{
  int i;
  string *match;

  if (!allowed()) return NPC_NO_PERMISSION;
  log_file(NPCLOGFILE,
      "--- RemoveQueuedRegExp (\""+pattern+"\") called by "+
      capitalize(getuid(this_interactive()))+" ("+dtime(time())+")\n");
  match=regexp(m_indices(queued),pattern);
  for (i=sizeof(match)-1; i>=0; i--)
  {
    log_file(NPCLOGFILE,
        sprintf("#none  [%2d]  %-66s\n", queued[match[i]], match[i]));
    queued = m_delete(queued,match[i]);
  }
  save_object(NPCSAVEFILE);
  return copy(queued);
}

mixed ApproveQueued(string key)
{
  if (!allowed()) return NPC_NO_PERMISSION;
  if (!stringp(key)) return 0;
  ochanged = 1;  // Oraklefragen neu berechnen
  if (member(queued,key) && !member(npcs,key)) {
    npcs[key,NPC_SCORE]=queued[key];
    npcs[key,NPC_NUMBER]=++lastNum;
    by_num += ([lastNum:key;queued[key]]);
    queued = m_delete(queued,key);
    save_object(NPCSAVEFILE);
    call_out("DumpNPCs",0);
    log_file(NPCLOGFILE,
        "--- ApproveQueued called by "+
        capitalize(getuid(this_interactive()))+" ("+dtime(time())+")\n"+
        sprintf("#%04d  [%2d]  %-66s\n", lastNum, npcs[key,NPC_SCORE], key));
    return ({npcs[key,NPC_NUMBER],npcs[key,NPC_SCORE]});
  }
  else return NPC_INVALID_ARG;
}

mixed ApproveQueuedRegexp(string pattern)
{
  int i;
  string *match;
  mixed added;

  if (!allowed()) return NPC_NO_PERMISSION;
  if (!stringp(pattern)) return 0;
  match=regexp(m_indices(queued),pattern);
  added=({});
  log_file(NPCLOGFILE,
      "--- ApproveQueuedRegExp (\""+pattern+"\") called by "+
      capitalize(getuid(this_interactive()))+" ("+dtime(time())+")\n");
  for (i=sizeof(match)-1; i>=0; i--)
  {
    if (!member(npcs,match[i])) {
      npcs[match[i],NPC_SCORE]=queued[match[i]];
      npcs[match[i],NPC_NUMBER]=++lastNum;
      by_num += ([lastNum:match[i];queued[match[i]]]);
      queued = m_delete(queued,match[i]);
      added += ({ ({ match[i], npcs[match[i],NPC_NUMBER],
                               npcs[match[i],NPC_SCORE] }) });
      log_file(NPCLOGFILE,
          sprintf("#%04d  [%2d]  %-66s\n",
          lastNum, npcs[match[i],NPC_SCORE], match[i]));
    }
  }
  if (!sizeof(added)) return NPC_INVALID_ARG;
  save_object(NPCSAVEFILE);
  call_out("DumpNPCs",0);
  ochanged = 1;  // Oraklefragen neu berechnen
  return added;
}

varargs mapping QueryQueued(mixed pattern)
{
  string *match;
  mapping m;
  int i;

  if (!stringp(pattern)) return copy(queued);
  m=([]);
  match=regexp(m_indices(queued),pattern);
  for (i=sizeof(match)-1; i>=0; i--)
    m[match[i]]=queued[match[i]];
  return m;
}

mapping QueryPending()
{
  return copy(pending);
}

int Recalculate(string s)
{
  int i, j;

  if (!s || !sizeof(s))
    return 0;

  for (j=0, i=6*sizeof(s)-1; i>0; i--) {
    if (test_bit(s,i))
      j+=by_num[i,NPC_SCORE];
  }
  return j;
}

nomask varargs int QueryPercent(string was, mixed pl)
{
  string ek,str;
  int i,perc;
  int *all;
  object po;

  po=previous_object();
  if (po)
    str=explode(object_name(po),"#")[0]+".c";
  else
    str="\n";

  if (!allowed() && member(oracle_trusted,str) == -1)
    return NPC_NO_PERMISSION;

  if (oracle_changed)                       // derzeit keine Abfragen erlaubt
    return NPC_NO_PERMISSION;

  if(stringp(pl)) pl=find_player(pl) || find_netdead(pl);

  if(!objectp(pl))
    return NPC_NO_ENTRY;

  if(member(m_indices(oracle),was) < 0)
    return NPC_NO_ENTRY;

  all=oracle[was,1];

  if(sizeof(all) < 1)
    return NPC_NO_ENTRY;

  ek=pl->QueryScores();
  perc=0;
  for(i=0;i<sizeof(all);i++) {
    if(test_bit(ek,all[i])) {
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
    return NPC_NO_PERMISSION;

  if(mappingp(oracle) && sizeof(oracle))
    ss=m_indices(oracle);
  return ss;
}

int CalcOracle(int von) {
  int j,i,bis,bis_max;
  string *ora_ind;
  string *obs_ind;
  string ss;

  if(previous_object() != this_object() && !allowed())
    return NPC_NO_PERMISSION;

  if(this_player() && interactive(this_player()))
    oracle_ob=this_player();
  else
    oracle_ob=0;

  oracle_changed=1;  //waehrend der Berechnung keine Fragen an das Orakel

  while(find_call_out("CalcOracle") != -1)
    remove_call_out("CalcOracle");

  ora_ind=m_indices(oracle);
  bis_max=sizeof(ora_ind);
  obs_ind=m_indices(npcs);

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
        oracle[ora_ind[j],1]=oracle[ora_ind[j],1]+({ npcs[obs_ind[i],NPC_NUMBER] });
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
      tell_object(oracle_ob,"*** Berechnung für das EK-Orakel fertig!\n");
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
    return NPC_NO_PERMISSION;

  if(!stringp(frage) || !stringp(pfad))
    return NPC_NO_ENTRY;

  if(sizeof(frage) < 3)        // die frage muss mindestens 3 zeichen lang sein
    return NPC_NO_ENTRY;

  if(sizeof(pfad) < 6)        // der pfad muss mindestens 10 zeichen lang sein
    return NPC_NO_ENTRY;

  if(file_size(pfad) != -2)    // es muss ein pfad sein
    return NPC_NO_ENTRY;

  if(mappingp(oracle) && sizeof(oracle))
  {
    ora_ind=m_indices(oracle);

    if (member(ora_ind,frage) != -1)
      return NPC_NO_ENTRY;

    for(i=0;i<sizeof(ora_ind);i++) {
      if(strstr(oracle[ora_ind[i],0],pfad) != -1)
        return NPC_NO_ENTRY;
    }
  }
  oracle+=([ frage:pfad;({ }) ]);

  obs_ind=m_indices(npcs);
                                                  // gleich berechnen :-))
  for(i=0;i<sizeof(obs_ind);i++) {
    if(strstr(obs_ind[i],pfad) != -1) {
                                                  // gespeichert wird nr des FP
      oracle[frage,1]=oracle[frage,1]+({ npcs[obs_ind[i],NPC_NUMBER] });
    }
  }

  save_object (NPCSAVEFILE);
  return 1;
}

nomask mixed RemoveOracleString(string frage)
{
  string ss;

  if (!allowed())              // tja...
    return NPC_NO_PERMISSION;

  if(!stringp(frage))
    return NPC_NO_ENTRY;

  if (member(m_indices(oracle),frage) == -1)
    return NPC_NO_ENTRY;

  ss=oracle[frage,0];
  oracle=m_delete(oracle,frage);

  save_object (NPCSAVEFILE);
  return ss;
}

nomask mixed ShowOracleString()
{
  string *ora_ind;
  string *ss;
  int i;

  if (!allowed())              // tja...
    return NPC_NO_PERMISSION;

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
    return NPC_NO_PERMISSION;

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
    return NPC_NO_PERMISSION;

  if(!stringp(str))
    return NPC_NO_ENTRY;

  if(file_size(str) < 1)    // es muss ein pfad sein
    return NPC_NO_ENTRY;

  if (member(oracle_trusted,str) != -1)
    return NPC_NO_ENTRY;

  oracle_trusted+=({ str });

  save_object (NPCSAVEFILE);
  return sizeof(oracle_trusted);
}

nomask int RemoveOracleTrusted(string str)
{
  int i;
  string ss;

  if (!allowed())              // tja...
    return NPC_NO_PERMISSION;

  if(!stringp(str))
    return NPC_NO_ENTRY;

  if(file_size(str) < 1)    // es muss ein pfad sein
    return NPC_NO_ENTRY;

  if (member(oracle_trusted,str) == -1)
    return NPC_NO_ENTRY;

  oracle_trusted-=({ str });

  save_object (NPCSAVEFILE);
  return sizeof(oracle_trusted);
}
