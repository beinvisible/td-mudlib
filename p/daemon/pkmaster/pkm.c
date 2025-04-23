/* pkm.c - zentrale Funktionen fuer den PkMaster
 *
 * 25.03 2003 Serii@Tamedhon - moeglichkeit fuer "vogelfreie" eingebaut
 *
 * 07.08 2002 by Serii@Tamedhon
 */

#include <daemon.h>
#include <moving.h>
#include <wizlevels.h>
#include "/p/daemon/pkmaster/pkmaster.h"

string *PkDirectories=({});
string *PkRooms=({});
mapping HitList=([]);

void create() {
  seteuid(getuid());
  restore_object(SAVE_FILE);
}

void Save() {
save_object(SAVE_FILE);
}

void reset() {
Save();
}

int remove() {return 0;}

static int secure()
{
  if (!previous_object()) return 0;
  if (geteuid(previous_object())==ROOTID) return 1;
  if (geteuid(previous_object()) != geteuid(this_interactive())) return 0;
  if (this_interactive() != this_player()) return 0;
  if (!IS_ARCH(this_interactive())) return 0;
  return 1;
}

string AddPkDirectory(string str)
{
  if(!secure())return "security violation!";
  PkDirectories+=({str});
  Save();
  return str;
}

mixed RemovePkDirectory(string str)
{
  if(!secure())return "security violation!";
  PkDirectories-=({str});
  Save();
  return PkDirectories;
}

string AddPkRoom(string str)
{
  if(!secure())return "security violation!";
  PkRooms+=({str});
  Save();
  return str;
}

mixed RemovePkRoom(string str)
{
  if(!secure())return "security violation!";
  PkRooms-=({str});
  Save();
  return PkRooms;
}

int check_directories(string str)
{
string *dir,*pkdir;
int lv,i,ret;
  str="/"+implode(explode(str,"/")[0..sizeof(explode(str,"/"))-2],"/");
  dir=explode(str,"/");
  ret=0;
  for(lv=0;lv<sizeof(PkDirectories);lv++)
  {
    pkdir=explode(PkDirectories[lv],"/");
    if(sizeof(pkdir))
    if(sizeof(pkdir)<=sizeof(dir))
    {
      if( sizeof(pkdir&dir) == sizeof(pkdir) ) return 1;
    }
  }
  return 0;
}

int check_files(string str)
{
  return member(PkRooms,str)>=0?1:0;
}

int check_hitlist(string name)
{
  return HitList[name]?1:0;
}

int check_hitlist_race(string name,string race)
{
  return HitList[name]?(((member(HitList[name][1],race)>=0)||(HitList[name][1][0]=="ALL"))?1:0):0;
}

mixed AddToHitlist(string name,string race,mixed races)
{
  if(!secure())return "security violation!";
  HitList[name]=({race,races});
  Save();
  return HitList;
}

mixed RemoveFromHitlist(string name)
{
  if(!secure())return "security violation!";
  HitList=m_delete(HitList,name);
  Save();
  return HitList;
}