/* /p/daemon/pkmaster.c
 *       
 * 15.02. 2005 - Gralkor@Tamedhon - PK-Meldung an alle Traeger des Sterns
 *
 * 30.03. 2004 - Serii@Tamedhon - Netztote Spieler angreifen ist selbst in PK-
 *                                gebieten nicht erlaubt!
 *
 * 28.06. 2003 - Querolin@Tamedhon - den Text etwas erweitert...
 *
 * 25.03. 2003 - Serii@Tamedhon - es gibt nun auch die moeglichkeit von
 *				  "vogelfreien"
 *
 * 08.08. 2002 - Serii@Tamedhon - verlegt von /obj nach /p/daemon
 *
 * 06.08. 2002 - Serii@Tamedhon - komplette neukonzeptionierung vom pk-
 *                                handling (von statisch@/std/living/combat.c--
 *                                -->dynamisch@/obj/pkmaster.c)
 *               Pk-Verzeichnis bzw Raum hinzufuegen: /obj/pkmaster/pkm:
 *                                                    AddPkDirectory("/dir");
 *                                                    AddPkRoom("/dir/room");
 *               HINWEIS: *ALLE* Raeume in einem Pk-Verzeichnis und saemtlichen
 *                        Unterverzeichnissen sind Pk-Gebiete!
 *                        Es sei denn dieser Raum ist zusaetzlich als Pk-Raum
 *                        eingetragen!
 * ---
 * Zustaendig fuer Playerkill-Handling
 * ---
 * Jan 08, 1997  Woody  Release
 * Jun 22, 1997  Woody  move jetzt M_GO|M_NOCHECK (wg. Erschoepfung)
 */

#pragma strong_types

#include <properties.h>
#include <daemon.h>
#include <moving.h>
#include <wizlevels.h>
#include "/p/daemon/pkmaster/pkmaster.h"
int remove() {return 0;}

int CheckForPkEnvironment(object ob,object wen)
{
  call_other(PKM,"???");
  return ((int)(PKM->check_directories(explode(object_name(environment(ob)),"#")[0]))^
  (int)(PKM->check_files(explode(object_name(environment(ob)),"#")[0])))%
  (interactive(wen)+1);//keine netztoten
}

int CheckForHitList(object wer,object wen)
{
  return (call_other(PKM,"check_hitlist",wen->query_real_name())+
         call_other(PKM,"check_hitlist_race",wen->query_real_name(),wer->QueryProp(P_RACE))-
	 (!interactive(wen)));//keine netztoten
}

void TriedPK (object wer, object wen)
{
  object w,*u;
  string m;
  int i;
  if (wer->QueryProp(P_GHOST)) {
    return;
  }
  if(CheckForPkEnvironment(wer,wen))return; //pk hier erlaubt?
  if((i=CheckForHitList(wer,wen))==2)return; //oder steht er auf der abschussliste?

  u=users();
  m=sprintf("**** %s greift %s an!\n",capitalize(wer->query_real_name()),capitalize(wen->query_real_name()));
  for (i=sizeof(u)-1;i>=0;i--)
    if (IS_LORD(u[i])||deep_present("\nsheriffstern",u[i]) ) {
        tell_object(u[i],m);
       }	

  tell_object (wer,"\nEin dumpfes Gefühl überkommt Dich...\n"
    "Dein schlechtes Gewissen einen heimtückischen Mordversuch begangen zu haben\n"
    "sitzt tief in Deinem Inneren...\n"
    "Das Bild vor Deinen Augen verdunkelt sich und verschwimmt immer mehr...\n\n"
    "Du fällst...\n\n");

  wer->StopHuntFor(wen, 1);
  wen->StopHuntFor(wer, 1);
  wer->SetProp(P_KILLS, wer->QueryProp(P_KILLS)+1);
  wer->move ("/room/entscheidung", M_GO|M_NOCHECK, "geschleudert",
    "wird ganz woanders hin", "wird hereingeschleudert");
}
