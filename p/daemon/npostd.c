/* Tamedhon MUDlib 
 *
 * npostd.c - NewsPOSTDaemon
 *          -> eingetragene NPCs koennen damit in der Zeitung posten
 *
 *  03/07/2004 Serii@Tamedhon
 */

#include <properties.h>
#include <ansi.h>
#include <daemon.h>
#include <moving.h>
#include <wizlevels.h>
#include <news.h>

#define SAVE_FILE "/p/daemon/save/npostd"

mapping Posters=([]);

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

mixed AddPoster(string ob,string *boards)
{
  if(!secure())return "security violation!";
  Posters[ob]=boards;
  Save();
  return Posters;
}

mixed RemovePoster(string ob)
{
  if(!secure())return "security violation!";
  Posters=m_delete(Posters,ob);
  Save();
  return Posters;
}

static void InformPlayers(string group,string player) // ausa zeitung geklaut ;)
{
  object *players;
  int i;
  mixed data;
  string ig;
  string color;
  players=users();
  ig=lower_case(player)+".news";
  for (i=sizeof(players)-1;i>=0;i--) {
  data=players[i]->QueryProp(P_WAITFOR);
  if (pointerp(data)&&(member(data,"Zeitung")>-1)) {
    data=players[i]->QueryProp(P_READ_NEWS);
    if (mappingp(data)&&(data[group]>0)) {
      data=players[i]->QueryProp(P_IGNORE);
      if (players[i]->QueryProp(P_TTY)=="ansi"){
        color="/std/player/base"->_query_color(players[i], P_COLOR_INFORM);
      }else{  color="a_plain";}
      if ((!pointerp(data))||(member(data,ig)==-1))
        tell_object(players[i],"*"+color+" Extrablatt!"+a_plain+
        " Neuer Artikel von "+color+capitalize(player)+a_plain+
        " in Rubrik \""+color+group+a_plain+"\".\n");
       }
     }
  }
}


int Post(string *message,int kn)
{string *groups;
  if(!previous_object())return -4;
  if(pointerp(groups=Posters[explode(object_name(previous_object()),"#")[0]]))
  {
    if(member(groups,message[M_BOARD])<0)return -5;
    if("/secure/news"->WriteNote(message,kn)>0)InformPlayers(message[M_BOARD],message[M_WRITER]);
  }
  return 0;
}
