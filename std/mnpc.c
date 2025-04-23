/*
 * SilberLand MUDlib
 * std/mnpc - standard moving npc
 * ---
 * NPC extensions (Woody)
 * make NPCs move via property definitions
 * ---
 * $Date: 1997/04/13 09:24:12 $
 * $Revision: 3.1 $
 * $Log: mnpc.c,v $
 * Revision 3.1  1997/04/13 09:24:12  Crea
 * einige bloede Bugs beseitigt.
 * MOV_SPECIAL geht jetzt endlich
 *
 * Revision 3.0  1997/04/13 09:23:15  Crea
 * prepared for revision control
 *
 * 1996-11-24  2.6  - P_NO_MNPC beruecksichtigt, MNPCs bleiben jetzt auch
 *                    stehen, wenn sie laenger keinem interactive begegnen
 * 1996-10-04  2.5  - wandern() gibt jetzt 1 im Erfolgsfall (when moved)
 *                    zurueck - praktisch beim Ueberschreiben
 * 1996-09-12  2.4  - Flag MOV_INFIGHT eingefuehrt
 * 1996-06-08  2.3  - MNPC kann jetzt auch durch Ausgaenge der Form
 *                    "durch einen Durchschlupf#/d/ebene/wald/roomxy"
 * 1996-05-28  2.2  - P_MNPC_AREA darf nun auch ein Array von Strings sein;
 *                    entsprechend darf der MNPC dann in allen Raeumen,
 *                    deren Filename so beginnt wie irgendein Element
 *                    des Arrays, rumrennen.
 * 1996-05-15  2.1  - P_MNPC_CMDS eingefuehrt (ein Mini-Sequencer)
 * 1996-04-13  2.0  - cond. symbol DEBUG
 *                  - Verwendung von mnpc.h und Properties !!!
 *                    ActivateMove() nicht mehr notwendig!!
 *                    Es reicht, MOV_MOVES in P_MNPC_FLAGS zu setzen!
 *                  - command() statt move() verwendet -> man sieht nun
 *                    auch eine ordentliche mout ("...geht nach Osten.")
 *                  - teleport nach MvDefault, wenn NPC in einer absoluten
 *                    Sackgasse steckt (also auch nicht rueckwaerts kann)
 * 1996-03-31  1.0  1st release
 *
 */

/* Doku:
**
** P_MNPC .......... = 1
** P_MNPC_AREA ..... = allowed area of movement given as either path to
**                     the filenames of the corresponding rooms or
**                     as array of filename initials
** P_MNPC_CMDS ..... = array of commands, where the elements are either
**                     a command (string), or a pause before the next
**                     move (int) - this is an _additional_ pause to the
**                     one defined by P_MNPC_WAIT
** P_MNPC_WAIT ..... = How long to wait before next move.
**                     Can be set as integer (means, wait X seconds
**                     before every move and add X/4 seconds as random
**                     pause per move) or as array of int [3] with meaning
**                     ({ WaitBeforeFirstMove, WaitBeforeMove, WaitRandom })
** P_MNPC_FLAGS .... = several Flags:
**                       MOV_BACKWARDS: MNPC is allowed to go back to the
**                                      previous room
**                       MOV_INDOORS: MNPC is allowed to go indoors
**                       MOV_OUTDOORS: MNPC is allowed to go outdoors
**                       MOV_SPECIAL: MNPC is allowed to use special exits
**                       MOV_MAYLOAD: MNPC is allowed to enter unloaded rooms
**                       MOV_MOVES: must be set to make MNPC really move
**                       MOV_CMDS: if set, P_MNPC_CMDS will be used as
**                                 action, otherwise normal random move
** P_MNPC_DEFAULTROOM ... = the room whereto the MNPC will be transed
**                          if he is absolutely lost
**
**
** ---
**
** Old style: (should not be used anymore)
**
**  SetMove (Where, Backwards, Indoors, Wait1st, WaitMin, WaitRnd, DefRoom);
**    Sets moving strategy of MNPC
**    Where: allowed area of movement given as path to the
**           corresponding rooms (e.g. "/d/ebene/Silberwald/")
**    Backwards (int): flag; 0 if MNPC should not move to the room
**                     he came from in the previous move, else != 0
**    Indoors: 0: don't care
**             1: only move indoors (it's generally a good idea to match
**                                   this with the startroom's property)
**             2: never move indoors (-"-)
**    Wait1st: time in seconds to wait before first move
**    WaitMin: minimum time to wait before every further move
**    WaitRnd: random time to wait -"- (eff.time = WaitMin+random(WaitRnd))
**    DefRoom: default room to move MNPC to if run into nowhere
**
**  ActivateMove (flag);
**    Activates MNPC's ability to move (if flag==1) or not (flag==0)
**    default is 0, even after call to SetMove() !!
**
*/

inherit "std/npc";

#include <properties.h>
#include <language.h>
#include <moving.h>
#include <combat.h>
#include <mnpc.h>

// #define DEBUG
// #define DEBUG2
#define TW(x) if (find_player("creative")) tell_object(find_player("creative"), x);

#define DEFAULTWAIT ({ 60, 60, 30 })
#define ACTIVE_TIME 300	// noch 300 Sekunden aktiv, nachdem der MNPC nen
			// Spieler getroffen hat

int CurrentCmd, PlayerMet;
string MvArea, MvDefault;
object prev_room;

void create()
{
  npc::create();
  SetProp (P_MNPC, 1);
  SetProp (P_MNPC_AREA, "/");
  SetProp (P_MNPC_FLAGS, 0);
  SetProp (P_MNPC_WAIT, DEFAULTWAIT );
  SetProp (P_MNPC_DEFAULTROOM, environment());
  PlayerMet=0;
}

void init()
{
  ::init();
  if (this_player() && query_once_interactive(this_player()))
  {
    PlayerMet=time();
    if ((QueryProp(P_MNPC_FLAGS)&MOV_MOVES) && (find_call_out("wandern")==-1))
      call_out ("wandern", QueryProp(P_MNPC_WAIT)[0]);
  }
}

static int *_set_mnpc_wait(mixed waits)
{
  int *MvWait;
  MvWait=({0,0,0});
  if (intp(waits))
  {
    MvWait[0]=MvWait[1]=waits;
    MvWait[2]=waits / 4;
  }
  else if (pointerp(waits))
  {
    int i;
    i=sizeof(waits);
    if ((i>0) && intp(waits[0])) MvWait[0]=waits[0]; else MvWait[0]=0;
    if ((i>1) && intp(waits[1])) MvWait[1]=waits[1]; else MvWait[1]=MvWait[0];
    if ((i>2) && intp(waits[2])) MvWait[2]=waits[2]; else MvWait[2]=MvWait[1] / 4;
  }
  return Set (P_MNPC_WAIT, MvWait);
}

static int _set_mnpc_flags(int flags)
{
  int oldf;
  if (intp(flags))
  {
    oldf=Query(P_MNPC_FLAGS);
    if (!(flags & MOV_MOVES))
      while (remove_call_out ("wandern") >= 0);
    else
      if (!(oldf & MOV_MOVES))
      {
        while (remove_call_out ("wandern") >= 0);
        call_out ("wandern", QueryProp(P_MNPC_WAIT)[0]);
      }
    return Set (P_MNPC_FLAGS, flags);
  }
  return 0;
}

void SetMove (string Where, int Backwards, int Indoors,
              int Wait1st, int WaitMin, int WaitRnd, string DefRoom)
{
  int flags;
  SetProp (P_MNPC_AREA, Where);
  flags = 0;
  if (Backwards) flags |= MOV_BACKWARDS;
  if (Indoors==1) flags |= MOV_INDOORS;
  else if (Indoors==2) flags |= MOV_OUTDOORS;
  else flags |= MOV_INDOORS | MOV_OUTDOORS;
  SetProp (P_MNPC_FLAGS, flags);
  SetProp (P_MNPC_WAIT, ({ Wait1st, WaitMin, WaitRnd }));
  SetProp (P_MNPC_DEFAULTROOM, DefRoom);
}

int ActivateMove (int flag)
{
  while (remove_call_out ("wandern") >= 0);
  if (flag)
  {
#ifdef DEBUG
    TW (sprintf("mnpc_flags: %O\n", QueryProp(P_MNPC_FLAGS)));
#endif
    SetProp (P_MNPC_FLAGS,
      (QueryProp(P_MNPC_FLAGS) & ~MOV_MOVES) | (flag ? MOV_MOVES : 0));
    // call_out ("wandern", QueryProp(P_MNPC_WAIT)[0]);
#ifdef DEBUG
    TW (sprintf("mnpc_flags: %O\n", QueryProp(P_MNPC_FLAGS)));
#endif
    return 1;
  }
  else
  return 0;
}

int wandern()  // groesstenteils von Merlin uebernommen
{
  mapping ex;
  mixed ziel, cmd, query_;
  object env, ob;
  string *ind, *rooms, fn, prev, *MvArea, MvDefault, path, *files, *rooms_;
  int i, j, flags, *wait;


  // wenn auf 2. pos ein string ist, dann muss es ja ein array sein. wenn nicht, dann pfad. anders ergibts keinen sinn.
  query_ = QueryProp(P_MNPC_AREA);

  if(!stringp(query_[1]))
  {
    rooms_ = ({});

    path = query_;
    if (path[<1] != '/')
          path += "/";
    files = get_dir(path + "*.c");
    for (i = 0; i < sizeof(files); i++)
    {
      rooms_+= ({path + files[i][0..<3]});
    }
    MvArea = rooms_;
  }
  else
  {
    MvArea = query_;
  }


  flags = QueryProp (P_MNPC_FLAGS);
  wait = QueryProp (P_MNPC_WAIT);
  MvDefault = QueryProp (P_MNPC_DEFAULTROOM);
  while (remove_call_out ("wandern") >= 0);
  if (!(flags & MOV_MOVES)) return 0;
  if (time()-PlayerMet >= ACTIVE_TIME) return 0;
  call_out ("wandern", wait[1] + random (wait[2]));

  if (!(env = environment()))
  {
    move (QueryProp(P_MNPC_DEFAULTROOM), 0);
    prev_room = 0;
    return 1;
  }

  /*
  // Nicht wandern, wenn nicht-idlende "Seher" im Raum sind
  for (raeume=all_inventory(env); sizeof(raeume); raeume=raeume[1..])
    if (interactive(raeume[0]) && query_idle(raeume[0]) < 180 &&
        raeume[0]->QueryProp (P_XP) > 999999)
      return 0;
  */

  if (flags & MOV_CMDS)
  {
    // tell_room (environment(), "Der doofe MNPC versucht sich in einem Command.\n");
    if (CurrentCmd > (sizeof(QueryProp(P_MNPC_CMDS))-1)) CurrentCmd=0;
    if (!pointerp(QueryProp(P_MNPC_CMDS))) return 0;
    cmd = QueryProp(P_MNPC_CMDS)[CurrentCmd++];
    if (intp(cmd)) {
      while (remove_call_out ("wandern") >= 0);
      call_out ("wandern", cmd);
      return 0;
    }
    if (!stringp(cmd)) return 0;
    if (cmd != "move")
    {
      // tell_room (environment(), "Command: '"+cmd+"'\n");
      command (cmd);
      return 1;
    }
  }
  if (InFight() && !(flags & MOV_INFIGHT)) return 0;

  ex = env->QueryProp(P_EXITS);
  if (flags & MOV_SPECIAL) ex += env->QueryProp(P_SPECIAL_EXITS);
  if (!mappingp(ex)) return 0;
#ifdef DEBUG2
  TW (sprintf("ex: %O\n", ex));
#endif
  ind = m_indices(ex);
#ifdef DEBUG
  TW (sprintf("ind: %O\n", ind));
#endif
  rooms = ({ });
  prev = 0;
  for (i = sizeof(ind)-1; i>=0; i--)
  {
    if (closurep(ex[ind[i]])) {
      rooms += ({ ind[i] });      // SpecialExit. Also moeglicher Exit.
      continue;
    }
    else
    if (!stringp(ziel=ex[ind[i]]))
      continue;                   // check exit, nur wenn nicht spec_ex
    if ((j=strstr(ziel, "#", 0)) != -1) ziel=ziel[j+1..];
#ifdef DEBUG
  TW (sprintf("*** Looking at room \"%s\".\n", ziel));
#endif
    if (!ob=find_object(ziel))
    {
#ifdef DEBUG
  TW (sprintf("would have to load room.\n"));
#endif
      if (flags & MOV_MAYLOAD) {
#ifdef DEBUG
  TW (sprintf("Lade Raum %O\n", ziel));
#endif
        call_other (ziel, "");  // lade Raum
        if (!ob=find_object(ziel)) continue;  // falls schiefgelaufen
      }
      else
        continue;      // nur in geladene Raeume rein
    }
    
    // if (ziel[0..sizeof(MvArea)-1] != MvArea) continue;
    // ^^^ alte Version ohne Arrays

/*  Achtung! MNPC's koennten via SpecialExits aus dem Gebiet hinaus!
 *  Da grundsaetzlich nicht checkbar ist, wohin das SpecExit fuehrt,
 *  ist hier Vorsicht geboten.
 *  Selbiges gilt auch fuer MOV_MAYLOAD: Es ist nicht sicher, ob der
 *  NPC bei einem SpecExit in einen geladenen Raum kommt, er laedt
 *  ihn aber auf alle Faelle beim Eintreten.
 *  Da in der Regel SpecialExits nicht sehr haeufig sind, ist das
 *  wohl vernachlaessigbar.
 *  -Crea-
 */
    
    j=sizeof(MvArea);

    while (j)
    {
#ifdef DEBUG
  TW (sprintf("ziel: '%s' vs. area: '%s'\n",
      ziel[0..sizeof(MvArea[j-1])-1], MvArea[j-1]
  ));
#endif
      if (ziel[0..sizeof(MvArea[j-1])-1] == MvArea[j-1]) break;
      j--;
    }
    if (!j) continue;  // j ist nur dann 0, wenn KEIN Raum matchte
    if (ob->QueryProp(P_NO_MNPC)) continue;
    if (!(flags & MOV_INDOORS) && (ob->QueryProp(P_INDOORS))) continue;
    if (!(flags & MOV_OUTDOORS) && (!ob->QueryProp(P_INDOORS))) continue;
    if (ob==env) continue;  // nicht in selben Raum rein
    if (ob==prev_room) {
      prev=ind[i];
      if (!(flags & MOV_BACKWARDS)) continue;  // nicht in vorigen Raum
    }
    rooms += ({ ind[i] });
  }
  prev_room = env;
#ifdef DEBUG
  TW (sprintf("rooms: %O\n", rooms));
#endif
  if (!(i=sizeof(rooms)))
    if (prev)
    {
#ifdef DEBUG
      TW ("no more possible rooms detected - moving backwards.\n");
#endif
      rooms+=({prev});
      i++;
    }
  if (i) command(rooms[random(i)]);
  if (environment() == env)
  {
#ifdef DEBUG
    TW (sprintf("move of %O failed, trying to move to MvDefault "+
    "instead.\n", this_object()));
#endif
    if (MvDefault && (MvDefault != object_name(env)) && find_object(MvDefault))
      move (MvDefault, M_TPORT);
    else return 0;
  }
  return 1;
}
