// SilberLand MUDlib
//
// std/player/sense.c -- player sense commands (sounds, smells) handling
//
// $Date: 1998/01/24 20:37:47 $
/* $Revision: 3.2 $
 * $Log: sense.c,v $
 * Revision 3.2  1998/01/24 20:37:47  Feyaria
 * nichts mehr lauschen wenn taub, P_DEAF auf save
 *
 * Revision 3.1  1997/09/09 12:17:48  Woody
 * riech und rieche statt riech* als Command, analog fuer Rest
 *
 * Revision 3.0  1997/03/13 01:45:44  Woody
 * prepared for revision control
 *
 * Revision 2.0  1996/05/26  Woody
 * riech und schnuppern hier wieder aktiviert, incl. an Objekten
 *
*/

#pragma strong_types

#define NEED_PROTOTYPES

#include "/sys/thing/properties.h"
#include "/sys/room/description.h"

#include <container.h>
#include <properties.h>
#include <rooms.h>
#include <wizlevels.h>
#include <defines.h>
#include <new_skills.h>


void create()
{
  Set(P_DEAF, SAVE, F_MODE);
}

object getobject(string str)
{
  string what;
  object base, env, *objs;

  // do we look at an object in our environment ?
  if (sscanf(str,"%s in raum", what) || sscanf(str,"%s im raum", what))
    base = environment();
  else
  {
    // is the object inside of me (inventory)
    if (sscanf(str,"%s in mir", what) || sscanf(str,"%s in dir", what))
      base = this_object();
    else
    {
      what = str;
      base = 0;
      /*
      ** P_REFERENCE_OBJECT fuer lauschen und riechen leider unbrauchbar
      */
/*
      // get the last object we looked at
      base = QueryProp(P_REFERENCE_OBJECT);
      // if a reference object exists, test for its existance in the room
      // or in our inventory
      if(base)
      {
        if(member(deep_inventory(environment()), base) != -1)
        {
          env = base;
          // check if base is inside of a living or non-transparent object
          while((env = environment(env)) &&
                !living(env) && env->QueryProp(P_TRANSPARENT))
            ; // do nothing
          if(env && env != this_object() && env != environment()) base = 0;
        }
        else base = 0;
      }
*/
    }
  }
  // if an ref object exists get its inventory, only if it is not a living
  // if(base && !living(base))  // !living() war fuer P_REFERENCE_OBJECT
  if (base)
    objs = base->present_objects(what);
  else
    // else get our inv and env
    objs = environment()->present_objects(what)
         + this_object()->present_objects(what);
  if(sizeof(objs))
    return objs[0];
  else
    return 0;
}

int riech(string str)
{
  object ob;
  string smell;
  
  _notify_fail("Woran möchtest Du riechen?\n");
  if (!str) str = DEFAULT_SMELL;
  else
    // 'rieche an x' == 'rieche x' setzen:
    if (str[0..2]=="an ") str = str[3..];
  if (ob=getobject(str))
  {
    smell = ob->GetSmell(QueryProp(P_RACE));
    if (!smell)
    {
      write ("Du schnupperst an "+ob->name(WEM)+", riechst aber nichts.\n");
      return 1;
    }
  }
  else
    if (ob = environment(this_player()))
      smell = ob->GetSmell(str, QueryProp(P_RACE));
  if (smell)
  {
    write(smell);
    return 1;
  }
  return 0;
}

int lausch(string str)
{
  object ob;
  string sound;
  
  _notify_fail("Wem oder was möchtest Du lauschen?\n");
  if (!str) str = DEFAULT_SOUND;
  if (this_player()->QueryProp(P_DEAF))
  {
    write("Du bist doch taub!\n");
    return 1;
  }
  if (ob=getobject(str))
  {
    sound = ob->GetSound(QueryProp(P_RACE));
    if (!sound)
    {
      write ("Du lauschst an "+ob->name(WEM)+", hörst aber nichts.\n");
      return 1;
    }
  }
  else
    if (ob = environment(this_player()))
      sound = ob->GetSound(str, QueryProp(P_RACE));
  if (sound)
  {
    write(sound);
    return 1;
  }
  return 0;
}

int altlausch(string str)
{
  object env;
  string sound;
  
  _notify_fail("Wem oder was möchtest Du lauschen?\n");
  if (!str) str = DEFAULT_SOUND;
  
  env = environment(this_player());
    
  if (this_player()->QueryProp(P_DEAF))
  {
    write("Du bist taub !\n");
    return 1;
  }
  if (env && (sound = env->GetSound(str, QueryProp(P_RACE))))
  {
    write(sound);
    return 1;
  }
  return 0;
}

static mixed *_query_localcmds()
{
  return
    ({({"riech","riech",0,0}),
      ({"rieche","riech",0,0}),
      ({"schnupper","riech",0,0}),
      ({"schnuppere","riech",0,0}),
      ({"lausch","lausch",0,0}),
      ({"lausche","lausch",0,0}),
      ({"horch","lausch",0,0}),
      ({"horche","lausch",0,0})
    });
}
