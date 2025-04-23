// SilberLand MUDlib
//
// std/room/description.c -- room description handling
//
// $Date: 2000/06/11 20:12:02 $
/* $Revision: 1.1 $
 * $Log: description.c,v $
 * Revision 1.1  2000/06/11 20:12:02  Samtpfote
 * Initial revision
 *
 * Revision 3.10  1998/01/13 07:47:44  Crea
 * added new wiz_header handling via P_WANTS_TO_LEARN Flags
 *
 * Revision 3.9  1997/12/28 09:07:14  Crea
 * implemented wiz_header, for new room-header.
 *
 * Revision 3.8  1997/11/08 20:10:41  Angrox
 * TT_CAVE bleibt TT_CAVE, auch wenn P_INDOORS == 1
 *
 * Revision 3.6  1997/10/14 12:04:59  Woody
 * QueryExhaustion() eingefuehrt
 *
 * Revision 3.5  1997/09/30 10:22:52  Crea
 * INDOORS kann jetzt immer gesetzt werden.
 *
 * Revision 3.4  1997/08/29 21:44:16  Woody
 * Im Rassenmapping der Sounds und Smells darf man auch ne Closure angeben
 *
 * Revision 3.3  1997/07/22 16:28:31  Woody
 * int_short() und int_long() varargs gemacht
 *
 * Revision 3.2  1997/06/25 12:07:29  Woody
 * P_HIDE_EXITS (wenn kein Array) beruecksichtigt
 *
 * Revision 3.1  1997/05/13 10:30:44  Crea
 * added terraintype-handling, fixed some bad bugs in set_environment
 * rooms without terrain will automatically get "correct" value
 *
 * Revision 3.0  1997/03/13 02:00:09  Woody
 * prepared for revision control
 *
 * Revision 2.14  1997/02/27  Woody
 * P_BRIEF & BRIEF_VMASK
 *
 * Revision 2.13  1996/10/10  Woody
 * private mapping details rausgenommen, isja in std/thing... *argl*
 *
 * Revision 2.12  1996/10/10  Woody
 * Handling von Exploration Points eingefuehrt
 *
 * Revision 2.11  09-02-1996  21:00:00  Woody
 * AddSmell() und AddSound() mit autom. Zeilenumbruch
 *
 * Revision 2.10  05-26-1996  16:30:00  Woody
 * riech und lausch Methoden hier rausgenommen (nach std/player/sense moved)
 *
 * Revision 2.00  04-17-1996  01:50:00  Woody
 * removed detail handling (now in std/thing/description.c)
 * added init() description::init()
 * added id's "raum", "hier"
 *
 * Revision 1.08  12-03-1996  23:00:00  Mupfel
 * Includes geaendert
 *
 * Revision 1.07  22-02-1996  23:00:00  Hadra
 * int_short() und int_long() geaendert
 *
 * Revision 1.06  22-02-1996  03:10:00  Mupfel
 * Neue (Default-)Ausgaben fuer DEFAULT_SMELL und DEFAULT_SOUND
 *
 * Revision 1.05  30-01-1996  02:25:00  Mupfel
 * ENV_TEMP => ENV_TEMPERATURE (hat eh' noch keiner benutzt)
 *
 * Revision 1.04  27-01-1996  02:55:00  Mupfel
 * AddReadDetail kann jetzt wie AddDetail auch Mappings (Rassen-
 * abhaengigkeit) und Closures als Argumente haben.
 *
 * Revision 1.03  22-12-1995  23:30:00  Mupfel
 * P_ENVIRONMENT: ENV_TEMP standardmaessig 15 Grad
 *                Flag EF_INDOORS wird mit P_INDOORS gesetzt
 *
 * Revision 1.02  13-10-1995  16:03:00  Mupfel
 * statt lausch(e) geht jetzt auch horch(e)
 * statt rieche geht jetzt auch rieche an
 *
 * Revision 1.01  13-10-1995  02:05:00  Mupfel
 * wer taub ist (P_DEAF gesetzt), kann nicht lauschen
 *
 * Revision 1.00  12-10-1995  20:15:00  Mupfel
 * AddSound, AddSmell, GetSmell, GetSound
 * Angeregt von Silvana@MorgenGrauen und Caradhras
 */                            

//_types

inherit "std/container/description";

#include <properties.h>
#include <defines.h>
#include <wizlevels.h>
#include <wizard.h>
#include <std_headers.h>
#include <room/description.h>
#include <room/doors.h>
#include <environment.h>
#include <exploration.h>
#include <moving.h>
#include <vt100.h>

private static mapping smells;
private static mapping sounds;

private static string *explore;

void create()
{
  string domain, subdomain, dummy;
  int tt;

  ::create();
  smells = ([DEFAULT_SMELL:"Du schnupperst, riechst aber nichts.\n"]);
  sounds = ([DEFAULT_SOUND:"Du lauschst, hörst aber nichts.\n"]);
  SetProp(P_INT_SHORT,"<namenloser Raum>");
  SetProp(P_INT_LONG,0);
  SetProp(P_ROOM_MSG, ({}) );
  SetProp(P_FUNC_MSG, 0);
  SetProp(P_MSG_PROB, 30);
  tt=0;
  if (sscanf(object_name(), "/d/%s/%s/%s", domain, subdomain, dummy)==3) {
    switch(domain) {
      case "dschungel":
        tt=TT_JUNGLE; break;
      case "sumpf":
        tt=TT_SWAMP; break;
      case "ebene":
        tt=TT_PLAIN; break;
      case "gebirge":
        tt=TT_MOUNTAIN; break;
      case "huegelland":
        tt=TT_HILL; break;
      case "wald":
        tt=TT_FOREST; break;
      case "polar":
        tt=TT_ARCTIC; break;
      case "tundra":
        tt=TT_TUNDRA; break;
      case "unterwelt":
        tt=TT_CAVE; break;
      case "wasser":
      case "meer":
      case "see":
        tt=TT_WATER; break;
      case "wueste":
        tt=TT_DESERT; break;
    }
  }
  SetProp(P_ENVIRONMENT,
      ([ ENV_TEMPERATURE:15, ENV_TERRAIN:tt||TT_OTHER ]) );
  AddId(({"raum","hier"}));
  explore = EPMASTER->QueryExplore(EP_SENSE);
}

void init()
{
  mixed roommsg;

  ::init();
  if (find_call_out("WriteRoomMessage")==-1)
    if ((roommsg=QueryProp(P_ROOM_MSG))&&sizeof(roommsg))
      call_out("WriteRoomMessage", random(QueryProp(P_MSG_PROB)));
}

void AddRoomMessage(string *mesg, int prob, mixed *func)
{
  if (prob>0)
    SetProp(P_MSG_PROB, prob);

  SetProp(P_ROOM_MSG, mesg);
  SetProp(P_FUNC_MSG, func);
}

static void WriteRoomMessage()
{
  int i,tim;
  string *room_msg,func;
	mixed *func_msg;

  room_msg = ({string *})QueryProp(P_ROOM_MSG);
  func_msg = QueryProp(P_FUNC_MSG);
  if ((!room_msg || !sizeof(room_msg)) && !func_msg)
    return;

	if (room_msg&&sizeof(room_msg))
	{
    i = random(sizeof(room_msg));
    tell_room(this_object(), room_msg[i]);
  }

  if (func_msg)
	{
		if (stringp(func_msg))
			func=(string)func_msg;
		else
			func=func_msg[random(sizeof(func_msg))];
		if (func && function_exists(func))
      call_other (this_object(), func, i);
	}

  while (remove_call_out("WriteRoomMessage")!=-1);
  tim=QueryProp(P_MSG_PROB);
  if(sizeof(filter(
			 deep_inventory(this_object()), #'interactive))) //')))
    call_out("WriteRoomMessage", (tim<15 ? 15 : tim));
}

mapping _set_smells(mapping arg)
{
  if (mappingp(arg))
    return smells = arg;
  return 0;
}

mapping _query_smells()
{
  return smells;
}

mapping _set_sounds(mapping arg)
{
  if (mappingp(arg))
    return sounds = arg;
  return 0;
}

mapping _query_sounds()
{
  return sounds;
}

void AddSmell(mixed *keys, mixed descr)
{
  int i;

  // autom. break_string()
  if (stringp(descr))
    descr=brstr(descr);
  else
    if (mappingp(descr))
    {
      string *k;
      k=m_indices(descr);
      for (i=sizeof(k)-1; i>=0; i--)
        descr[k[i]]=brstr(descr[k[i]]);
    }

  if (!pointerp(keys))
    smells += ([ lower_case((string)keys) : descr ]);
  else
    for (i = sizeof(keys) - 1; i >= 0; i--)
      smells += ([ lower_case(({string})keys[i]) : descr ]);
}

void RemoveSmell(mixed *keys)
{
  int i;

  if (!keys)
    smells = ([DEFAULT_SMELL:"Du schnupperst, riechst aber nichts.\n"]);
  else if (stringp(keys))
    smells = m_delete(smells, keys);
  else
    for (i = sizeof(keys) - 1; i >= 0; i--)
      smells = m_delete(smells, keys[i]);
}

void AddSound(mixed *keys, mixed descr) 
{
  int i;

  // autom. break_string()
  if (stringp(descr))
    descr=brstr(descr);
  else
    if (mappingp(descr))
    {
      string *k;
      k=m_indices(descr);
      for (i=sizeof(k)-1; i>=0; i--)
        descr[k[i]]=brstr(descr[k[i]]);
    }

  if (!pointerp(keys))
    sounds += ([ lower_case((string)keys) : descr ]);
  else
    for (i = sizeof(keys) - 1; i >= 0; i--)
      sounds += ([ lower_case(({string})keys[i]) : descr ]);
}

void RemoveSound(mixed *keys)
{
  int i;

  if (!keys)
    sounds = ([DEFAULT_SOUND:"Du lauschst, hörst aber nichts.\n"]);
  else if (stringp(keys))
    sounds = m_delete(sounds, keys);
  else
    for (i = sizeof(keys) - 1; i >= 0; i--)
      sounds = m_delete(sounds, keys[i]);
}

varargs string GetSmell(mixed key, mixed race) 
{
  mixed smell;

  if (stringp(race)) race=lower_case(race);
  smell = smells[key];
  if (closurep(smell)) smell = funcall(smell,key);
  if (mappingp(smell)) smell = smell[race] || smell[0];
  if (closurep(smell)) smell = funcall(smell,key);

  if (smell && explore && member(explore,key) >= 0)
    EPMASTER->GiveExplorationPoint(key, EP_SENSE);

  return smell;
}

varargs string GetSound(mixed key, mixed race) 
{
  mixed sound;

  if (stringp(race)) race=lower_case(race);
  sound = sounds[key];
  if (closurep(sound)) sound = funcall(sound,key);
  if (mappingp(sound)) sound = sound[race] || sound[0];
  if (closurep(sound)) sound = funcall(sound,key);

  if (sound && explore && member(explore,key) >= 0)
    EPMASTER->GiveExplorationPoint(key, EP_SENSE);

  return sound;
}

/* 
 * new id handling. Details must be handled here, too, so
 * they are found from the player ob.
 * But only if the player is IN the room...
 */
varargs int id(string str, int lvl) 
{
  if (::id(str))
    return 1;
  if (this_player() && environment(this_player()) != this_object()) 
    return 0;
  if (Query(P_DETAILS)[str])
    return 1;
  return 0;
}

string wiz_header(mixed viewer)
{
  string descr, ter, fnam, nc, sc, lc, tc, cc, ec, ic, dc, coff, *fn;
  int fs, mlev;

  mlev=viewer->QueryProp(P_WANTS_TO_LEARN);
  descr="";
  switch (viewer->QueryProp(P_TTY)) {
    case "vt100":
    case "ansi": coff=VT_TA_AO; nc=VT_GREEN; sc=VT_TA_B+VT_BLUE;
                 lc=VT_TA_B+VT_YELLOW; tc=VT_TA_B+VT_WHITE;
                 ec=VT_TA_B+VT_GREEN; ic=VT_TA_B+VT_RED;
                 cc=VT_CYAN; dc=VT_TA_B+VT_WHITE; break;
    default: coff=nc=sc=cc=dc="";
  }
  descr+="["+nc;
  fnam=object_name(ME);
  fn=explode(fnam, "/");
  if (mlev&RH_NFILENAME && sizeof(fn)) {
    switch (fn[0]) {
      case "players": descr+="~"+fn[1]+"/";
                      if (sizeof(fn)>3)
                        descr+=".../";
                      descr+=fn[<1]; break;
      case "d": descr+="+"+capitalize(fn[1][0..1])+"/"+fn[2]+"/.../"+fn[<1]; break;
      default: descr+="/"+implode(fn, "/");
    }
  }
  else
    descr+=fnam;
  if (mlev&RH_SIZE) {
    descr+=coff+"|sz:"+sc;
    fs=file_size(fnam+".c");
    if (fs<1024)
      descr+=fs;
    else
      descr+=(fs/1024)+"k";
  }
  if (mlev&RH_LIGHT)
  {
    if(intp(ME->QueryProp(P_LIGHT)))
      descr+=sprintf("%s|li:%s%d", coff, lc, ME->QueryProp(P_LIGHT));
    if(floatp(ME->QueryProp(P_LIGHT)))
      descr+=sprintf("%s|li:%s%f", coff, lc, ME->QueryProp(P_LIGHT));
  }
  if (mlev&RH_TEMPERATURE)
    descr+=sprintf("%s|tmp:%s%d", coff, tc, ME->QueryProp(P_ENVIRONMENT)[ENV_TEMPERATURE]);
  if (mlev&RH_CLONER)
    descr+=sprintf("%s|cl:%s%s", coff, cc, ME->QueryProp(P_CLONER));
  if (mlev&RH_IN_OUT)
    descr+=sprintf("%s|%s%s", coff, ic, 
        (ME->QueryProp(P_ENVIRONMENT)[ENV_FLAGS] && EF_INDOORS?"in":"out")+"door");
  if (mlev&RH_TERRAIN) {
    ter=({"undef", "cave", "house", "hill", "forest", "town", "mountain",
          "desert", "jungle", "swamp", "plain", "arctic", "underwater", 
	  "road", "tundra", "water"})
        [ME->QueryProp(P_ENVIRONMENT)[ENV_TERRAIN]];
    descr+=sprintf("%s|ter:%s%s", coff, ec, ter);
  }
  descr+=coff+"]";

  return descr;
}

varargs string int_long(mixed viewer,mixed viewpoint, int shortflag)
{
  string descr, inv_descr;
  int mlev;

  descr="\n";
  if( IS_LEARNER(viewer) && (mlev=viewer->QueryProp( P_WANTS_TO_LEARN )) )
    descr+=wiz_header(viewer)+"\n";
  descr += process_string(QueryProp(P_INT_LONG)||"");
  if ( viewer->QueryProp(P_SHOW_EXITS) && (!QueryProp(P_HIDE_EXITS) ||
      pointerp(QueryProp(P_HIDE_EXITS))) )
    descr += (GetExits(viewer)||"");
  inv_descr = make_invlist(viewer, all_inventory(ME)-({viewpoint}),
                                    shortflag);
  if ( inv_descr != "" )
    descr += inv_descr;
  if( environment() && QueryProp(P_TRANSPARENT) )
    descr += "Außerhalb siehst Du:\n"
      + environment()->int_short(viewer,ME);
  return descr;
}

varargs string int_short(mixed viewer,mixed viewpoint, int shortflag)
{
  string descr, inv_descr;
  int mlev;

  descr = process_string( QueryProp(P_INT_SHORT)||"");
  if( IS_LEARNER(viewer) && (mlev=viewer->QueryProp( P_WANTS_TO_LEARN )) )
    descr += " "+wiz_header(viewer);

  descr += ".\n";

  if ( ( viewer->QueryProp(P_SHOW_EXITS)
         || ( environment(viewer) == ME &&
             (viewer->QueryProp(P_BRIEF) & BRIEF_VMASK) ) )
       && (!QueryProp(P_HIDE_EXITS) || pointerp(QueryProp(P_HIDE_EXITS))) )

    descr +=  (GetExits(viewer)||"");
  inv_descr =  make_invlist(viewer, all_inventory(ME)-({viewpoint}),
				    shortflag);
  if ( inv_descr != "" )
    descr += inv_descr;
  return descr;
}

void exit()
{
  int i;
  object *inv;

  inv=all_inventory(this_object());
  for (i=sizeof(inv)-1;i>=0;i--)
    if (query_once_interactive(inv[i]))
      return;
  while (remove_call_out("WriteRoomMessage")!=-1);
}

string _query_int_long()
{
  string l1,l2;

  l2=0;
  if (QueryProp(P_DOOR_INFOS))
    l2=(({string})call_other(DOOR_MASTER,"look_doors"));
  l1=Query(P_INT_LONG); // Reihenfolge wichtig!
  if (!l1 && !l2) return 0;
  if (!l1) l1="";
  if (!l2) l2="";
  return l1+l2;
}

int _set_indoors(int in_)
{
  mapping env;
  
  if (in_)
  {
    if (!mappingp(env=QueryProp(P_ENVIRONMENT)))
      env = ([]);
    env[ENV_FLAGS] |= EF_INDOORS;
    if(env[ENV_TERRAIN]!=TT_CAVE)
       env[ENV_TERRAIN] = env[ENV_TERRAIN]==TT_MOUNTAIN ? TT_CAVE : TT_HOUSE;
    SetProp(P_ENVIRONMENT, env);
  }
  Set(P_INDOORS, in_);
  return in_;
}

int QueryTerrain()
{
  mapping env;
  if (!mappingp(env=QueryProp(P_ENVIRONMENT)))
    return TT_OTHER;
  return env[ENV_TERRAIN] || TT_OTHER;
}

int QueryExhaustion(object pl)
{
  int i;
  mapping rt;

  if (!objectp(pl)) return MVP_DEFAULT_COST;
  if (!mappingp(rt=pl->QueryProp(P_RACE_TT_MV))) return MVP_DEFAULT_COST;
  i=QueryTerrain();
  return member(rt,i) ? rt[i] : MVP_DEFAULT_COST;
}

mapping _set_environment(mapping m)
{
  mapping env;
  if (!mappingp(env=QueryProp(P_ENVIRONMENT)))
    env = ([]);
  if (mappingp(m)) {
    env += m;
    if (env[ENV_FLAGS]&EF_INDOORS && env[ENV_TERRAIN]!=TT_CAVE)
      env[ENV_TERRAIN] = env[ENV_TERRAIN]==TT_MOUNTAIN ? TT_CAVE : TT_HOUSE;
  }
  return Set(P_ENVIRONMENT, env);
}

