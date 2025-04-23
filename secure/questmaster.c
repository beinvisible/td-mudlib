/* SilberLand MUDlib
 *
 * /secure/questmaster.c -- questmaster object
 *
 * $Date: 2011/12/17 09:45:00 $
 * $Revision: 1.37 $
 * $Log: questmaster.c,v $
 * Revision 1.37  2011/12/17 09:45:00  Querolin
 * Facebook-Log eingebaut 
 *
 * Revision 1.36  1997/11/05 20:04:13  Feyaria
 * Silberland -> MUDNAME
 *
 * Revision 1.35  1997/10/09 18:07:08  Woody
 * Untere Schranke fuer Level 1 statt 0, obere Schranke entfaellt ganz
 *
 * Revision 1.34  1997/07/02 09:50:48  Woody
 * dritte Questgruppe entfernt, Handling derselben flexibler
 * (man muss jetzt eigtl. nur noch QGROUPS im .h aendern...)
 *
 * Revision 1.33  1997/06/12 12:25:40  Woody
 * Set/Query-Seer/Wiz-QP(), loesen QP_MIN ab. Bugfixes.
 * Ausserdem QueryReadyForSeer() fuer Seher
 *
 * Revision 1.32  1997/05/25 15:10:52  Woody
 * QueryQuests() hat jetzt einen Flag-Parameter und gibt wenn
 * dieser != 0 ist das quest mapping zurueck (statt des array-arrays)
 *
 * Revision 1.31  1997/03/30 12:52:30  Woody
 * Der Postreiter ruft hier nicht mehr :) (tut schon secure/mailer.c)
 *
 * Revision 1.30  1997/03/15 23:40:05  Woody
 * prepared for revision control
 *
 * Revision 1.20  1997/03/03  Creative
 * MorgenGrauen -> SilberLand
 *
 * Revision 1.19  1997/02/19  Woody
 * name() auf <Quest> gesetzt
 *
 * Revision 1.18  1995/08/14  06:37:17  Boing
 * Der Questmaster verschickt beim Loesen einer Quest jetzt eine mail.
 *
 * Revision 1.17  1995/06/16  12:56:01  mud
 * new computation in QueryDontneed() takes player's P_NEEDED_QP into account
 *
 * Revision 1.16  1995/05/22  19:34:21  Kirk
 * 1196 -> QP_MIN (defined in questmaster.h)
 *
 * Revision 1.15  1995/05/22  18:55:31  Kirk
 * Added QueryDontneed() for dynamic calculation of not needed quests
 *
 * Revision 1.14  1995/05/17  08:43:01  Kirk
 * little protection in Channel()
 *
 * Revision 1.13  1995/03/20  15:52:53  Kirk
 * ignore optional quests in QueryReadyForWiz()
 *
 * Revision 1.12  1994/11/21  20:08:59  Kirk
 * using P_NEEDED_QP
 *
 * Revision 1.11  1994/09/16  18:10:25  Jof
 * *** empty log message ***
 *
 * Revision 1.10  1994/08/23  11:35:17  Kirk
 * ignore deactivated quests in QueryReadyForWiz()
 *
 * Revision 1.9  1994/07/20  15:52:13  Kirk
 * fixed max_QP bug in AddQuest
 *
 * Revision 1.8  1994/06/13  22:46:13  Kirk
 * added questgroup handling in QueryReadyForWiz
 *
 * Revision 1.7  1994/06/11  00:12:56  Kirk
 * Fixed bug in activate/deactivate logging
 *
 * Revision 1.6  1994/05/16  09:16:29  Kirk
 * fixed bug in SetActive()
 *
 * Revision 1.5  1994/05/04  08:39:50  Kirk
 * ???
 *
 * Revision 1.4  1994/04/19  01:43:22  Kirk
 * storage of questlist changed from alist to mapping
 * QueryAllQuests() migrated into QueryAllKeys()
 *
 * Revision 1.3  1994/04/06  21:51:03  Kirk
 * replaced QueryQP() by QueryProp(P_QP)
 *
 * Revision 1.2  1994/03/01  22:20:43  Kirk
 * added logging
 *
 * Revision 1.1  1994/02/28  21:28:45  Kirk
 * Initial revision
 *
 * 
 */

/* Die Quests werden in einem Mapping gespeichert. Der Schluessel ist dabei der
   Quest-Name, die Eintraege sind Arrays der folgenden Form:

   1. Element ist die Zahl der durch diese Quest zu erwerbenden Questpunkte.
   2. Element ist die Zahl der Erfahrungspunkte, die der Spieler bekommt,
      wenn er diese Quest loest.
   3. Element ist ein Array mit den Filenamen der Objekte, denen es gestattet
      ist, diese Quest beim Player als geloest zu markieren (Erzmagier duerfen
      das aber sowieso immer).
   4. Element ist ein String, der die Quest kurz beschreibt. Dieser String wird
      dem Spieler vom Orakel als Hinweis gegeben.
   5. Element ist eine Zahl zwischen 0 und 20, die den Schwierigkeitsgrad der
      Quest angibt, nach Einschaetzung des EM fuer Quests. Das Orakel kann dann
      evtl. sinnige Saetze wie "Diese Quest erscheint mir aber noch recht
      schwer fuer Dich.", oder "Hm, die haettest Du ja schon viel eher loesen
      koennen." absondern. :)
   6. Element ist ein Integer, 0, 1 oder 2. 
      0: Questpunkte zaehlen zur Berechnung der maximalen Questpunkte.
         Die Quest ist freiwillig.
      1: Questpunkte zaehlen zur Berechnung der maximalen Questpunkte.
         Die Quest muss geloest werden (unabhaengig von der 80%-Regel).
      2: Questpunkte zaehlen nicht zur Berechnung der maximalen Questpunkte.
         Die Quest ist freiwillig.
   7. Element ist ein Integer, 0 oder 1.
      0: Quest voruebergehend deaktiviert (suspendiert)
      1: Quest aktiviert
   8. Element ist ein String und enthaelt den Namen des Magiers, der die
      Quest "verbrochen" hat.
*/

#include "/secure/wizlevels.h"
#include "/secure/questmaster.h"
#include <properties.h>
#include <daemon.h>
#include <config.h>
 
#pragma strong_types

int max_QP, seer_QP, wiz_QP;
mapping quests;

void save_info()
{
  save_object( QUESTS );
}

void create()
{
  seteuid(getuid(this_object()));
  if (!restore_object( QUESTS ))
  {
    quests=([]);
    max_QP=seer_QP=wiz_QP=0;
  }
}

static int secure()
{
  if (!previous_object()) return 0;
  if (geteuid(previous_object())==ROOTID) return 1;
  if (geteuid(previous_object()) != geteuid(this_interactive())) return 0;
  if (this_interactive() != this_player()) return 0;
  if (!IS_ARCH(this_interactive())) return 0;
  return 1;
}

int AddQuest(string name, int questpoints, int experience,
	     string *allowedobj, string hint, int difficulty, int need,
	     int active, string wiz)
{
  mixed *quest;
  int i;

  if (!secure()) return 0;
  if (!stringp(name) || sizeof(name)<5) return -1;
  if (questpoints<1) return -2;
  if (!intp(experience)) return -3;
  if (stringp(allowedobj)) allowedobj=({""});
  if (!pointerp(allowedobj)) return -4;
  for (i=sizeof(allowedobj)-1;i>=0;i--)
  {
    if (!stringp(allowedobj[i]) || allowedobj[i]=="") return -4;
    allowedobj[i]="/secure/master"->_get_path(allowedobj[i],0);
  }
  if (!stringp(hint) || hint=="") return -5;
  // if (difficulty<0 || difficulty>20) return -6;
  if (difficulty<1) return -6;
  if (active<0 || active>1) return -7;
  if (!stringp(wiz) || wiz=="" || 
      file_size("/players/"+(wiz=lower_case(wiz))) != -2) return -8;
  if(quests[name]&&(quests[name][5]==0||quests[name][5]==1)&&quests[name][6])
    max_QP-=quests[name][0];
  quests+=([name:({questpoints,experience,allowedobj,hint,difficulty,need,active,wiz})]);
  if ((need == 0 || need ==1) && active )
    max_QP+=questpoints;
  save_info();
  QMLOG(sprintf("add: %s %O (%s)",name,quests[name],
		getuid(this_interactive())));
  return 1;
}

int RemoveQuest(string name)
{
  mixed *quest;
    
  if (!secure()) return 0;
  if (!quests[name]) return -1;
  QMLOG(sprintf("remove: %s %O (%s)",name,quests[name],
		getuid(this_interactive())));
  if ((quests[name][5] == 0 || quests[name][5] == 1) && quests[name][6])
    max_QP-=quests[name][0];
  quests=m_delete(quests,name);
  save_info();
  return 1;
}

int SetSeerQP(int qp)
{
  if (!secure()) return 0;
  if ((qp<1) || (qp>max_QP)) return 0;
  seer_QP=qp;
  QMLOG(sprintf("set seer qp: %d (%s)",qp,getuid(this_interactive())));
  if (wiz_QP<qp) {
    wiz_QP=qp;
    QMLOG(sprintf("adjusted wiz qp: %d (%s)",qp,getuid(this_interactive())));
  }
  save_info();
  return 1;
}

int SetWizQP(int qp)
{
  if (!secure()) return 0;
  if ((qp<1) || (qp>max_QP)) return 0;
  wiz_QP=qp;
  QMLOG(sprintf("set wiz qp: %d (%s)",qp,getuid(this_interactive())));
  save_info();
  return 1;
}

int QueryNeededQP()
{
  return max_QP * QP_PERCENT / 100;
}

int QuerySeerQP() { return seer_QP; }
int QueryWizQP() { return wiz_QP; }

int QueryMaxQP()
{
  return max_QP;
}

int *QueryDontneed(object pl)
{
  int i, j, qp_percent, nrgr;
  string *indices;
  int *num, *pts, *med, *res;

  nrgr = sizeof(QGROUPS);
  if(!pl || !objectp(pl))
    pl=(this_player()?this_player():this_interactive());
  if(!pl)
    return allocate(nrgr);
  num=allocate(nrgr);
  pts=allocate(nrgr);
  med=allocate(nrgr);
  res=allocate(nrgr);
  indices=m_indices(quests);
  for (i=sizeof(indices)-1;i>=0;i--)
  {
    // inactive or optional
    if (!quests[indices[i]][6]||quests[indices[i]][5]==2)
      continue;
    j=nrgr-1;
    while ((quests[indices[i]][0] < QGROUPS[j]) && j) j--;
    num[j]++,pts[j]+=quests[indices[i]][0];
  }
  if (!QueryMaxQP())
    qp_percent=0;
  else {
    if ((qp_percent=pl->QueryProp(P_NEEDED_QP)) < QuerySeerQP())
      qp_percent = QuerySeerQP();
    qp_percent=qp_percent*100/QueryMaxQP();
  }
  if(!qp_percent)
    qp_percent=QP_PERCENT;
  for (i=0;i<sizeof(QGROUPS);i++)
  {
    med[i]=(pts[i]/(num[i]||1)||1);
    pts[i]-=pts[i]*qp_percent/100;
    res[i]=(pts[i]%med[i]>0?(pts[i]/med[i]+1):pts[i]/med[i]);
  }
  return res;
}

int QueryReadyForSeerOrWiz(object player, int wiz)
{
  int i,j;
  string *indices;
  mixed *notsolved;
  int *groups,*dontneed;
  int bonus;

  groups = allocate(sizeof(QGROUPS));
  dontneed = QueryDontneed(player);
  if (player->QueryProp(P_QP) < player->QueryProp(P_NEEDED_QP))
    return -1;
  // This is for security:
  i = wiz ? QueryWizQP() : QuerySeerQP();
  if (player->QueryProp(P_NEEDED_QP) < i && player->QueryProp(P_QP) < i)
    return -1;

  notsolved=({});
  indices=m_indices(quests);
  for (i=sizeof(indices)-1;i>=0;i--)
  {
    // inactive or optional
    if (!quests[indices[i]][6]||quests[indices[i]][5]==2)
      continue;
    j=sizeof(QGROUPS)-1;
    while ((quests[indices[i]][0] < QGROUPS[j]) && j) j--;
    if (!player->QueryQuest(indices[i])) groups[j]++;
    if (quests[indices[i]][5]==1 && !player->QueryQuest(indices[i]))
      notsolved+=({indices[i]});
  }
  for(i=(sizeof(QGROUPS)-1),bonus=0;i>=0;i--)
    if((bonus += dontneed[i]-groups[i])<0)
      break;
      
  if (!sizeof(notsolved) && bonus >= 0)
    return 1;
  return 0;
}

int QueryReadyForSeer(object player)
{
  return QueryReadyForSeerOrWiz(player, 0);
}

int QueryReadyForWiz(object player)
{
  return QueryReadyForSeerOrWiz(player, 1);
}

mixed *QueryQuest(string name) 
{
  if(!quests[name])
    return ({});
  return quests[name];
}

varargs mixed QueryQuests(int mapflag)
{
  if (mapflag)
    return quests;
  else
    return ({m_indices(quests),m_values(quests)});
}

string *QueryAllKeys()
{
  return m_indices(quests);
}

int SetActive(string name, int flag)
{
  mixed *quest;
    
  if (!secure()) return 0;
  if (!(quest=quests[name])) return -1;
  switch(flag)
  {
    case 0:
      if (quest[6] == flag)
        return -2;
      quest[6] = flag;
      if (quest[5] == 0 || quest[5] == 1)
	max_QP-=quest[0];
      break;
    case 1:
      if (quest[6] == flag)
        return -2;
      quest[6] = flag;
      if (quest[5] == 0 || quest[5] == 1)
	max_QP+=quest[0];
      break;
    default:
      return -3;
  }
  quests[name]=quest;
  save_info();
  QMLOG(sprintf("%s: %s (%s)",(flag?"activate":"deactivate"),name,
		getuid(this_interactive())));
  return 1;
}

string name()
{
  return "<Quest>";
}

void Channel(string msg)
{
    if(!interactive(previous_object()))
	return;
    catch(VOICEMASTER->SendToChannel("Abenteuer",this_object(),SAY,msg));
}

/* quoted from /sys/mail.h: */
#define MSG_FROM 0
#define MSG_SENDER 1
#define MSG_RECIPIENT 2
#define MSG_CC 3
#define MSG_BCC 4
#define MSG_SUBJECT 5
#define MSG_DATE 6
#define MSG_ID 7
#define MSG_BODY 8

void SendMail(string questname, mixed *quest, object player)
{
   mixed* mail;
   string text;

   text =
   "Hallo "+capitalize(getuid(player))+",\n"+
   break_string("Nachdem Du gerade eben das Abenteuer '"+
   questname +"' ("+quest[0]+" Punkte) mit Erfolg bestanden hast, sind "+
   "wir nun an Deiner Meinung dazu interessiert:", 78)+
   "  Hat Dir das Abenteuer gefallen und wieso bzw. wieso nicht?\n"+
   "  Ist die Einstufung Deiner Meinung nach richtig? (AP und Stufe)\n"+
   "  Gab es Probleme oder gar Fehler?\n"+
   "  Hast Du Verbesserungsvorschläge?\n\n"+
   break_string("Diese Nachricht wurde automatisch verschickt, wenn Du mit "+
   "dem 'r' Kommando darauf antwortest, geht die Antwort direkt an "+
   capitalize(quest[7])+".", 78);

   mail = allocate(9);
   mail[MSG_FROM] = quest[7];
   mail[MSG_SENDER] = MUDNAME;
   mail[MSG_RECIPIENT] = getuid(player);
   mail[MSG_CC]=0;
   mail[MSG_BCC]=0;
   mail[MSG_SUBJECT]="Das Abenteuer: "+questname;
   mail[MSG_DATE]=dtime(time());
   mail[MSG_ID]=MUDNAME+":"+time();
   mail[MSG_BODY]=text;

   return "/secure/mailer"->DeliverMail(mail,1);
}   
