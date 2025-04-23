// SilberLand MUDlib
//
// std/player/quests.c -- quest handler
//
// $Date: 1997/09/17 14:40:08 $                      
/* $Revision: 3.2 $       
 * $Log: quests.c,v $
 * Revision 3.2  1997/09/17 14:40:08  Woody
 * Geloeschte Quests beruecksichtigen
 *
 * Revision 3.1  1997/06/25 12:05:22  Woody
 * NotifyGiveQuest() in Gilde aufrufen
 *
 * Revision 3.0  1997/03/13 01:43:51  Woody
 * prepared for revision control
 *
 * Revision 2.17  1997/02/19  Woody
 * Verschicken der Mail reaktiviert (war im AL deaktiviert)
 *
 * Revision 2.16  1995/08/14  06:38:56  Boing
 * Beim Loesen einer Quest wird nun eine mail an den Spieler geschickt.
 *
 * Revision 2.15  1995/03/30  13:47:56  Kirk
 * ?
 *
 * Revision 2.14  1995/01/05  11:23:48  Kirk
 * in GiveQuest: explode -> efun::explode
 *
 * Revision 2.13  1994/10/04  09:49:23  Jof
 * *** empty log message ***
 *
 * Revision 2.12  1994/09/16  17:58:18  Jof
 * *** empty log message ***
 *
 * Revision 2.11  1994/07/14  15:15:51  Jof
 * Removed command mappings
 *
 * Revision 2.10  1994/06/13  07:36:24  Jof
 * Removed SET_METHODs
 *
 * Revision 2.9  1994/05/21  00:32:03  Kirk
 * new function DeleteQuest()
 *
 * Revision 2.8  1994/05/16  23:40:54  Kirk
 * minor changes in QiveQuest()
 *
 * Revision 2.7  1994/05/13  14:14:59  Kirk
 * fixed Bug in QueryQuest()
 *
 * Revision 2.6  1994/04/21  10:18:33  Jof
 * *** empty log message ***
 *
 * Revision 2.5  1994/04/06  22:20:45  Kirk
 * removed QueryQP(), SetQP() and QueryQuests()
 *
 * Revision 2.4  1994/03/17  23:36:55  Jof
 * PROTECTED -> SECURED & Reindented
 *
 * Revision 2.4  1994/03/17  23:36:55  Jof
 * PROTECTED -> SECURED & Reindented
 *
 * Revision 2.3  1994/02/24  13:03:14  Jof
 * Private removed
 *
 * Revision 2.2  1994/01/25  09:13:22  Kirk
 * take into account activ-flag in GiveQuest()
 *
 * Revision 2.1  1994/01/17  23:35:05  mud
 * *** empty log message ***
 *
 * Revision 2.0  1994/01/13  16:59:26  mud
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/05  16:17:07  mud
 * changed save variables partly to mapping entry
 *             
 */                            

// Dieses Modul enhaelt die Quest-spezifischen Teile der Playerobjekte.

#pragma strong_types

#define NEED_PROTOTYPES

#include <player/quest.h>
#include <thing/properties.h>
#include "/secure/questmaster.h"
#include <wizlevels.h>
#include <daemon.h>
#include <language.h>
#include <mail.h>
#include <player/base.h>
#include <living.h>
#include <defines.h>
#include <new_skills.h>
#include <properties.h>

#define DB(x) "/room/debug"->debug_message(this_object(), x)
#define FILE(x)	(explode(object_name(x),"#")[0])

mixed quests;


int QueryQuest(string questname);
// local properties prototype
static mixed _query_quests();
static int   _set_questpoints(int qp);
static int   _query_questpoints();

void create()
{
  Set(P_QUESTS, -1, F_SET_METHOD);
  Set(P_QUESTS, quests = ({}));
  Set(P_QUESTS, SECURED, F_MODE);
  Set(P_QP, SAVE, F_MODE);
  Set(P_QP, SECURED, F_MODE);
}

varargs int GiveQuest(string questname, string message)
{
  mixed *quest;

  quest=QM->QueryQuest(questname);
  // Questname ungueltig
  if (!quest||!pointerp(quest)||quest==({})) return GQ_KEY_INVALID;
  // Unbefugter Zugriff auf deaktivierte Quest
  if (!quest[6]&&!IS_ARCH(this_interactive())) return GQ_IS_INACTIVE;
  // Unbefugter Zugriff
  
  //DB(sprintf("QUEST[2]   == %O\n", quest[2]));
  //DB(sprintf("MA(PO, Q2) == %i\n",
  //	member(FILE(previous_object()),quest[2])));
  if (member(quest[2], load_name(previous_object()))==-1 &&
      !IS_ARCH(this_interactive()))
    return GQ_ILLEGAL_OBJ;
  // Gilde wird in jedem Fall informiert.
  call_other(GUILD_DIR+QueryProp(P_GUILD),"NotifyGiveQuest",ME,questname);
  // Quest bereits gesetzt
  if (QueryQuest(questname)) return GQ_ALREADY_SET;
  Set(P_QP, QueryProp(P_QP)+quest[0]);
  ME->AddExp(quest[1]);
  if (quests)
    quests+=({({questname,quest[0]})});
  else
    quests=({({questname,quest[0]})});
  ME->force_save();
  if (message && message!="")
    if (message!="__silent__")
      message=implode(explode(message,"@@name@@"),
		      this_object()->name(WER));
    else
      message="";
  else
    message=this_object()->name(WER)+" hat gerade ein Abenteuer bestanden: "+
      questname+"\n";
  if(message!="")
    catch("/secure/questmaster"->Channel(message));
  catch("/secure/questmaster"->SendMail(questname, quest, this_object()));
  return OK;
}

int DeleteQuest(string questname)
{
  mixed *quest;
  int i;
  
  // Quest ist nicht gesetzt
  if(!QueryQuest(questname)) return DQ_NOT_SET;
  quest=QM->QueryQuest(questname);
  // Questname ungueltig
  if (!quest||!pointerp(quest)||quest==({})) return DQ_KEY_INVALID;
  // Unbefugter Zugriff
  if (!IS_ARCH(this_interactive())) return DQ_ILLEGAL_OBJ;
  Set(P_QP, QueryProp(P_QP)-quest[0]);
  ME->AddExp(-quest[1]);
  for(i=sizeof(quests)-1;i>=0;i--)
    if(quests[i][0]==questname)
      break;
  quests=quests[0..i-1]+quests[i+1..];
  ME->force_save();
  return OK;
}

int QueryQuest(string questname)
{
  int i,dummy;

  if( sscanf( getuid(), "gast%d", dummy ) == 1 )
    return QQ_GUEST;

  if(!questname || !stringp(questname) || questname == "")
    return QQ_KEY_INVALID;

  for (i = sizeof(quests)-1; i >= 0; i--)
    if (questname==quests[i][0])
      return OK;
  return QQ_KEY_INVALID;
}

// **** local property methods

static int _query_questpoints()
{
  mixed *quest;
  string qname;
  int i;
  int questpoints;
	
  if (!pointerp(quests)||sizeof(quests)==0)
  {
    SetProp(P_QP, 0);
    // if (this_interactive()==this_object()) ME->save_me(1);
    return 0;
  }
  questpoints = Query(P_QP);
	quests-=({({})});
  for (i=0;i<sizeof(quests);i++)
  {
    if(pointerp(quests[i]) && sizeof(quests[i]))
      qname=quests[i][0];
    else
      qname=quests[i];
    quest=QM->QueryQuest(qname);
    if (pointerp(quest))
    {
      if (pointerp(quests[i]))
      {
        if (!sizeof(quest)) { // Quest wurde geloescht
          questpoints-=quests[i][1];
          quests[i]=({});
        }
        else {
          questpoints-=quests[i][1];
          questpoints+=quest[0];
          quests[i][1]=quest[0];
        }
      }
      else 
	quests[i]=({quests[i],quest[0]});
    }
  }
  quests-=({ ({}) });
  // if(this_interactive()==this_object()) 
  //   ME->save_me(1);
  return Set(P_QP, questpoints);
}

static int _set_questpoints(int qp)
{
  if (!IS_ARCH(this_interactive())) 
    return -1;
  return Set(P_QP, qp);
}

static mixed _query_quests()
{
  return quests[0..];
}

// ****
