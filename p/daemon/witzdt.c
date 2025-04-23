// Tamedhon MUDlib
//
// /p/daemon/witzdt.c -- Witz des Tages Posting
//
// $Date: 2002/06/25 19:45:40 $
/* $Revision: 1.0 $
 * Revision 1.0  2002/06/25 19:45:40  Querolin
 * ...
 */

#pragma strong_types

#include <daemon.h>
#include <config.h>
#include <properties.h>

#define WITZFILE "/p/daemon/save/witz.txt"
#define PROVIDER "witzdestages.net"
#define RUBRIK "witze"
#define ADMIN "querolin"
#define CHANNEL_ONLY_HEADLINES

//#define DEBUG(str) tell_object(find_player(ADMIN), sprintf("|\\| %O\n",str))

static void NewsUpdate();
static void InformPlayers(string group, string text);


void create()
{
  seteuid(getuid(this_object()));
  call_out("WitzUpdate", 0);
}

static void WitzUpdate()
{
  mixed *art;
  mixed tmp;
  int i,n;

  while (remove_call_out("WitzUpdate") != -1);
  call_out("WitzUpdate", (3600-(time()%3600)));

  tmp = read_file(WITZFILE);
  if (tmp)
  {
    rm(WITZFILE);

  // Artikel veroeffentlichen
    art = ({ RUBRIK, PROVIDER, 0, 0,
          "Der Witz des Tages",
          tmp });

    if ( (NEWSSERVER->WriteNote(art,1) > 0) ||
         ( (NEWSSERVER->RemoveNote(RUBRIK,0) > 0) &&
           (NEWSSERVER->WriteNote(art,1) > 0) ) )
    { }  
    else if (find_player(ADMIN))
      tell_object(find_player(ADMIN),
          "[WitzdT:] Fehler bei Posten eines Artikels, bitte prüfen!\n");
  
  
    InformPlayers(RUBRIK,
        "* Extrablatt! Neuer Artikel in Rubrik 'Witze' von '"+PROVIDER+"'\n");
  }
}

static void InformPlayers(string group, string text)
{
  object *players;
  int i;
  mixed data;

  players=users();
  for (i=sizeof(players)-1;i>=0;i--) {
    data=players[i]->QueryProp(P_WAITFOR);
    if (pointerp(data)&&(member(data,"Zeitung")>-1)) {
      data=players[i]->QueryProp(P_READ_NEWS);
      if (mappingp(data)&&(data[group]>0))
        tell_object(players[i],text);
    }
  }
}

