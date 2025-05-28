// SilberLand MUDlib
//
// std/player/moving.c -- player moving
//
// $Date: 1997/11/05 16:21:08 $                      
/* $Revision: 3.3 $       
 * $Log: moving.c,v $
 * Revision 3.3  1997/11/05 16:21:08  Woody
 * msgin/out: Geister schweben :)
 *
 * Revision 3.2  1997/07/18 21:27:36  Woody
 * Erschoepft Meldungen an Spieler ausgeben (vorher: life.c)
 *
 * Revision 3.1  1997/06/25 12:03:43  Woody
 * P_MV bei move() mit M_GO reduzieren, remove: silent flag,
 * msgin/msgout fuer Froesche modifiziert :)
 *
 * Revision 3.0  1997/03/13 01:40:58  Woody
 * prepared for revision control
 *
 * Revision 2.8  1994/10/04  09:54:21  Jof
 * *** empty log message ***
 *
 * Revision 2.8  1994/10/04  09:54:21  Jof
 * *** empty log message ***
 *
 * Revision 2.8  1994/10/04  18:30:22  mud
 * fixes ...
 *
 * Revision 2.7  1994/08/25  12:10:00  Jof
 * fixes
 *
 * Revision 2.6  1994/08/01  08:22:49  Jof
 * remove() with deep_inv
 *
 * Revision 2.5  1994/06/20  09:49:32  Jof
 * *** empty log message ***
 *
 * Revision 2.4  1994/06/20  08:00:39  Jof
 * *** empty log message ***
 *
 * Revision 2.3  1994/03/18  13:00:31  Jof
 * DONE
 *
 * Revision 2.2  1994/03/17  23:36:55  Jof
 * PROTECTED -> SECURED & Reindented
 *
 * Revision 2.1  1994/01/19  20:23:03  mud
 * *** empty log message ***
 *
 * Revision 2.0  1994/01/13  16:59:13  mud
 * *** empty log message ***
 *
 * Revision 2.0  1994/01/13  16:59:13  mud
 * *** empty log message ***
 *
 * Revision 1.3  1994/01/05  16:17:07  mud
 * changed save variables partly to mapping entry
 *
 * Revision 1.2  1994/01/03  18:31:41  mud
 * *** empty log message ***
 *             
 */                            

inherit "std/living/moving";

#pragma strong_types

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <properties.h>
#include <language.h>
#include <defines.h>
#include <moving.h>

void create()
{
  SetProp(P_MSGIN,   "kommt an");
  SetProp(P_MSGOUT,  "geht");
  SetProp(P_MMSGIN,  "tritt aus einer Schwefelwolke");
  SetProp(P_MMSGOUT, "verschwindet mit Knall und Schwefelduft");
  Set(P_MSGIN,   SAVE, F_MODE_AS);
  Set(P_MSGOUT,  SAVE, F_MODE_AS);
  Set(P_MMSGIN,  SAVE, F_MODE_AS);
  Set(P_MMSGOUT, SAVE, F_MODE_AS);
}

// autoload and nodrop object may not fall into the environment
varargs int remove(int silent)
{
  mixed dest_obj;

  if (interactive(this_object()))
    catch("/secure/merlin"->notify_player_leave(getuid(this_object())));
  for(dest_obj = deep_inventory(ME); sizeof(dest_obj); dest_obj = dest_obj[1..])
    if(dest_obj[0]->QueryProp(P_AUTOLOADOBJ) ||
       dest_obj[0]->QueryProp(P_NODROP))
    {
      dest_obj[0]->remove(); // trying to remove before hard destruct
      if(dest_obj[0]) destruct(dest_obj[0]);
    }

  // self destruction
  if(!QueryProp(P_INVIS) && !silent)
    catch(say(name(WER,1) + " verlässt diese Welt.\n", ME));
  if(ME && !silent) tell_object(ME, "Bis bald!\n");
  if((this_player() && getuid(ME) != getuid(this_player())))
    log_file("PLAYERDEST",dtime(time())+" "+geteuid(ME)
	     +" vern. von "+geteuid(this_player())+"\n");
  return ::remove();
}

#define EXH_MSG_ME ({ "Du fühlst Dich etwas erschöpft.\n", \
            "Du verspürst leichte Müdigkeit nach Deinen Anstrengungen.\n", \
            "Dein Herz klopft etwas schneller.\n", \
            "Du schwitzt leicht. Wie wärs mal mit etwas Ruhe?\n" })

#define EXH_MSG2_ME ({ "Du spürst die Erschöpfung nun ziemlich stark.\n", \
            "Völlig verausgabt schnappst Du nach Luft.\n", \
            "Dein Herz klopft heftig... vielleicht solltest Du ihm ja mal " \
              "etwas Ruhe\ngönnen?\n", \
            "Du keuchst vor Anstrengung.\n", \
            "Schweißperlen rinnen Dir übers Gesicht - " \
              "kein Wunder, nach dieser\nAnstrengung!\n" })

#define EXH_MSG2_ROOM ({ " schnauft vor Anstrengung.\n", \
            " macht einen stark erschöpften Eindruck.\n", \
            " schwitzt stark, kein Wunder, bei der Anstrengung!\n", \
            " keucht erschöpft.\n", \
            " schnappt völlig verausgabt nach Luft.\n" })

varargs int move(mixed dest, int methods, string direction,
                 string textout, string textin)
{
  int mv, mvp;

  mv=::move(dest, methods, direction, textout, textin);
  if (mv<=0) {
    switch (mv) {
      case ME_TOO_EXHAUSTED:
        tell_object(ME, "Du bist zu erschöpft.\n"); break;
      case ME_RESTING:
        tell_object(ME, "Du solltest zuerst aufstehen.\n");
      default:
    }
  }
  else if ((methods & M_GO) && !QueryProp(P_GHOST)) {
    SetProp(P_MV, mvp=QueryProp(P_MV)-QueryNeededMv());
    if ((mvp < 250) && !random(3))
      if (mvp < 100) {          // SEHR erschoepft, Meldung mit 33%
        write (EXH_MSG2_ME[random(sizeof(EXH_MSG2_ME))]);
        say (this_object()->Name(WER)+
            EXH_MSG2_ROOM[random(sizeof(EXH_MSG2_ROOM))]);
      }
      else if (!random(2))      // sollen nicht ganz so oft kommen... 17%
        write(EXH_MSG_ME[random(sizeof(EXH_MSG_ME))]);
     ME->SetProp(P_LAST_ROOM,object_name(environment(this_object())));
  }
  return mv;
}

/*** Query-Methoden fuer Froesche... ;^) ***/
string _query_msgin() {
  return QueryProp(P_GHOST) ? "schwebt herein" :
         QueryProp(P_FROG) ? "hüpft herein" : Query(P_MSGIN);
}

string _query_msgout() {
  return QueryProp(P_GHOST) ? "schwebt" :
         QueryProp(P_FROG) ? "hüpft" : Query(P_MSGOUT);
}
