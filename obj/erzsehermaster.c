/*
 * /obj/erzsehermaster.c
 * ---
 * Zustaendig fuer die Verwaltung des Erzseherkanals
 * ---
 * Autor:      TamTam
 * Datum:      26.04.2008
 * ---
 * Aenderungen:
 * DD.MM.YYYY  Name  ..
 */


#pragma strong_types

#include <properties.h>
#include <wizlevels.h>

#ifndef VOICEMASTER
#define VOICEMASTER     "/p/daemon/channeld"
#endif


int channel_MayJoin(object user)
{
  //Return 0 = erlaubt
  //Return !=0 = nicht erlaubt
  if (IS_LEARNER(user) || (user->QueryProp(P_CAN_FLAGS)&CAN_ERZ)) return 0;

  return 1;
}

int remove() {return 0;}

void create() {
  if (clonep(this_object())) return destruct(this_object());
}

string Name()
{return this_object()->query_real_name();}
string name()
{return this_object()->Name();}
string query_real_name()
{return "^W^S{TEM}^N";}

