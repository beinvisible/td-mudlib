// SilberLand MUDlib
//
// /p/daemon/routingd.c -- Routing daemon
//
// $Date: 1997/03/16 02:33:42 $
/* $Revision: 1.1 $
 * $Log: routingd.c,v $
 * Revision 1.1  1997/03/16 02:33:42  Woody
 * prepared for revision control
 *
 */

#pragma strong_types
#include <wizlevels.h>

#define DB_NAME "/p/daemon/save/routingd"
#define MAX_LEN 50
#define TP this_player()

mapping domains;
static mapping exits;
static mapping targets;
static mapping directions;
static mapping reverse_directions;

void create() {
  if (!restore_object(DB_NAME))
    domains=(["/d/ebene/room/":1]);
  exits=([]);
  targets=([]);

  // Umsetzung zur Speicherersparnis:
  directions=(["norden":1,
	       "nordosten":2,
	       "osten":3,
	       "südosten":4,
	       "oben":5,
	       "süden":-1,
	       "südwesten":-2,
	       "westen":-3,
	       "nordwesten":-4,
	       "unten":-5
	       ]);
  reverse_directions=([1:"norden",
		       2:"nordosten",
		       3:"osten",
		       4:"südosten",
		       5:"oben",
		       -1:"süden",
		       -2:"südwesten",
		       -3:"westen",
		       -4:"nordwesten",
		       -5:"unten"
		       ]);
}

// Fuer Debug-Zecke:
mixed query_exits(string dir) {return exits[dir];}
mixed query_targets(string typ) {return targets[typ];}

int AddDomain(string dom, int val) {
  // Fuegt neue Domain hinzu, in der Routing moeglich sein soll.
  // +1: Einschliessen
  // -1: Ausschliessen
  if (!IS_ARCH(TP)
      || val<-1
      || val>1)
    return 0;
  if (!val)
    domains=m_delete(domains,dom);
  else
    domains[dom]=val;
  save_object(DB_NAME);
  return 1;
}

int RegisterTarget(string type, string fname) {
  // Registriert Raum als zu einer Gruppe von Zielen zugehoerig
  // (z.B. RegisterTarget("pub","/d/ebene/room/PortVain/pub2"))
  mapping map;

  if (!type || !fname)
    return 0;
  type=lower_case(type);
  if (type[0..1]!="##")
    type="##"+type;
  if (!mappingp(map=targets[type]))
    map=([]);
  map[fname]=1;
  targets[type]=map;
  return 1;
}

int RegisterExit(string start, mixed richtungen, string ziel) {
  // Registriert Ausgang in der angegebenen Richtung von Start nach Ziel,
  // wenn Routuing dort moeglich sein soll.
  string *dirs,s_dir,s_room,z_dir,z_room;
  int i,sz,ok;
  mixed ex,x,richtung;
  
  if (stringp(richtungen))
    richtungen=({richtungen});
  if (!start || !pointerp(richtungen) || !ziel)
    return 0;
  dirs=explode(start,"/");sz=sizeof(dirs)-1;x="/";ok=0;
  for (i=0;i<sz;i++) {
    x=x+dirs[i]+"/";
    if (domains[x]==-1) // Subdomain ausgeschlossen?
      return 0;
    if (domains[x]==1) // Domain erlaubt?
      ok=1;
  }
  if (!ok)
    return 0;

  s_dir="/"+implode(dirs[0..<2],"/")+"/";s_room=dirs[<1];
  dirs=explode(ziel,"/");
  z_dir="/"+implode(dirs[0..<2],"/")+"/";z_room=dirs[<1];

  if (!mappingp(exits[s_dir]))
    exits[s_dir]=([]);
  if (!pointerp(ex=exits[s_dir][s_room]))
    ex=({});
  for (i=sizeof(richtungen)-1;i>=0;i--) {
    richtung=richtungen[i];
    if (!(x=directions[richtung]))
      x=richtung;
  
    if (member(ex,x)>=0) // Richtung schon eingetragen?
      continue;

    // Eintragung:
    if (s_dir==z_dir)
      ex+=({x,z_room});
    else
      ex+=({x,z_room,z_dir});
  }
  exits[s_dir][s_room]=ex;
  return 1;
}

// Low-Level FindRoute :-)
mapping do_find_route(string s_dir, string s_room, mapping is_target) {
  mapping erreicht;
  int i,j,k;
  mixed *newx, *actx, ex;
  string room, dir, x_room, x_dir, x;
  
  erreicht=([(s_dir+s_room):"##START"]);
  newx=({s_dir,s_room});
  for (k=MAX_LEN;k>=0;k--) {
    actx=newx;
    newx=({});
    for (i=sizeof(actx)-2;i>=0;i-=2) {
      dir=actx[i];room=actx[i+1];
      // printf("%O:%O\n",dir,room);
      if (!mappingp(exits[dir]) || !pointerp(ex=exits[dir][room]))
	continue;
      // printf("-> %O\n",ex);
      for (j=sizeof(ex)-1;j>=1;j-=2) {
	x_room=ex[j];
	if (x_room[0]=='/') {
	  x_dir=x_room;
	  x_room=ex[--j];
	} else {
	  x_dir=dir;
	}
	x=x_dir+x_room;
	// printf("-> %O (%O, %O)\n",x,xdir,x_room);
	if (!erreicht[x]) {
	  erreicht[x]=({dir+room,ex[j-1]});
	  if (is_target[x]) {
	    erreicht["##ZIEL"]=erreicht[x]; // Kann ja auch Gruppe sein
	    return erreicht;
	  }
	  newx+=({x_dir,x_room});
	}
      }
    }
  }
  return 0;
}

varargs mixed FindRoute(string start, string ziel, int etyp) {
  mapping res,ziele;
  string *dirs,s_dir, s_room,x;
  mixed ex,r,result;
  int k;
  
  if (!start || !ziel)
    return 0;
  if (!mappingp(ziele=targets[ziel]))
    ziele=([ziel:1]);
  dirs=explode(start,"/");
  s_dir="/"+implode(dirs[0..<2],"/")+"/";s_room=dirs[<1];
  if (!mappingp(res=do_find_route(s_dir, s_room, ziele)))
    return 0; // nicht gefunden
  if (etyp)
    result=({});
  else
    result=([]);
  x="##ZIEL";start=s_dir+s_room;
  for (k=MAX_LEN+2;k>=0;k--) {
    ex=res[x];
    if (ex=="##START") // Fertig.
      return result;
    if (!(r=reverse_directions[ex[1]])) // richtige Richtungen eintragen.
      r=ex[1];
    x=ex[0];
    if (etyp)
      result=({r})+result;
    else
      result[x]=r;
  }
  return 0;
}
