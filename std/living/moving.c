// SilberLand MUDlib
//
// LIVING/MOVING.C -- moving of living objects
//
// $Date: 2004/04/15 08:00:00 $
/* $Revision: 3.6 $
 * $Log: moving.c,v $
 * Revision 3.6  2004/04/15 08:00:00  Serii
 * P_TMP_MOVE_HOOK handling verbessert
 * (es sind nun mehrere move-hooks auf ein object moeglich. maximal EINER pro
 *  'zielobject' (meist das obj das den hook setzt))
 *
 * Revision 3.5  1997/10/14 11:50:31  Woody
 * QueryNeededMv() geaendert
 *
 * Revision 3.4  1997/10/06 12:22:15  Woody
 * Spruchvorbereitung abbrechen
 *
 * Revision 3.3  1997/09/17 14:33:06  Woody
 * P_TMP_MOVE_HOOK eingebaut
 *
 * Revision 3.2  1997/06/26 14:17:13  Woody
 * *argl* In der Gewichtsberechnung ein || 1 eingebaut, weil bei
 * Geistern P_MAX_WEIGHT 0 ist und die in den Todesraum gemoved
 * werden wollen....... *hmpf*
 *
 * Revision 3.1  1997/06/25 11:55:31  Woody
 * Man verfolgt nicht mehr in den Netztotenraum...
 * P_MV checken, TryMove() (Gelaende und Gewicht)
 *
 * Revision 3.0  1997/03/13 00:59:34  Woody
 * prepared for revision control
 *
 * 1997/02/27  Woody
 * BRIEF_* (aber nur im auskommentiertem Code)
 *
 * 1996/05/26  Woody
 * extended MSGOUT (mit '#' als Makro fuer die Richtung)
 *
 * 02-01-1996	    Mupfel
 * im 'ultrakurz'-Modus kommt in dunklen Raeumen statt
 * 'Finsternis.\n' nur noch '!!'
 *
 * 1995/11/20      Gundur
 * move M_SILENT wenn Skill QUIETWALK an
 *
 * Revision 2.23  1995/05/22  09:36:36  Jof
 * Fixed bug in movemessages
 *
 * Revision 2.22  1995/05/22  07:17:27  Jof
 * clean_up
 *
 * Revision 2.22  1995/05/21  20:20:14  mud
 * Clean up
 *
 * Revision 2.21  1995/04/26  18:14:28  Jof
 * Some speed opimizations
 *
 * Revision 2.20  1995/03/30  13:47:42  Rumata
 * Revision 2.19  1995/01/25  22:26:23  Jof
 * Revision 2.19  1995/01/25  22:26:23  Jof
 * Revision 2.18  1995/01/04  14:49:43  Jof
 * Revision 2.17  1995/01/04  14:40:43  Jof
 * Revision 2.16  1994/11/30  00:06:07  Jof
 * Revision 2.15  1994/11/21  08:31:07  Jof
 * Revision 2.14  1994/10/04  09:45:53  Jof
 * Revision 2.13  1994/09/29  16:36:20  Rochus
 * Revision 2.12  1994/09/15  17:43:02  Rochus
 * Nachtsicht wird benutzt, falls Faehigkeit vorhanden.
 *
 * Revision 2.11  1994/06/25  01:05:58  Jof
 * Revision 2.10  1994/06/24  00:29:26  Jof
 * Revision 2.9  1994/06/24  00:10:21  Jof
 * New following implemented
 *
 * Revision 2.8  1994/05/15  18:11:04  Jof
 * Revision 2.7  1994/03/21  16:48:13  Jof
 * Revision 2.6  1994/03/18  12:35:53  Jof
 * Revision 2.5  1994/03/14  17:39:05  Jof
 * one more check for exit
 *
 * Revision 2.4  1994/03/14  16:11:02  Jof
 * only call exit() if NOT m_nocheck
 *
 * Revision 2.3  1994/03/14  16:06:49  Jof
 * removed catch() around call of exit.
 *
 * Revision 2.2  1994/03/03  17:30:02  Kirk
 * Revision 2.1  1994/02/17  16:56:14  Kirk
 * strong_types
 *
 * Revision 2.0  1994/01/13  17:00:28  mud
 *
 */

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <thing/description.h>

#include <config.h>
#include <properties.h>
#include <moving.h>
#include <language.h>
#include <wizlevels.h>
#include <defines.h>
#include <living.h>
#include <new_skills.h>

#pragma strong_types

void AddPursuer(object ob)
{
  mixed *pur;

  if (!objectp(ob))
    return;
  
  if (!pointerp(pur=Query(P_PURSUERS)))
    pur=({0,({})});
  else
    if (member(pur[1],ob)!=-1)
      return;
  SetProp(P_PURSUERS,({pur[0],pur[1]+({ob})}));
  ob->_SetPursued(ME);
}

void RemovePursuer(object ob)
{
  mixed *pur;
  
  if (pointerp(pur=Query(P_PURSUERS)) && member(pur[1],ob)!=-1)
  {
    pur[1]-=({ob});
    ob->_RemovePursued(ME);
    if (!pur[0]&&!sizeof(pur[1]))
      pur=0;
    SetProp(P_PURSUERS,pur);
  }
}

void _SetPursued(object ob)
{
  mixed *pur;

  if (!pointerp(pur=Query(P_PURSUERS)))
    pur=({0,({})});
  else
    if (objectp(pur[0]))
      pur[0]->RemovePursuer(ME);
  pur[0]=ob;
  Set(P_PURSUERS,pur);
}

void _RemovePursued(object ob)
{
  mixed *pur;

  if (!pointerp(pur=Query(P_PURSUERS)) || pur[0]!=ob)
    return;
  pur[0]=0;
  if (!sizeof(pur[1]))
    pur=0;
  Set(P_PURSUERS,pur);
}

int CheckEnemy(object ob)
{
  return (living(ob) && funcall(#'IsEnemy,ob)); //'));
  // return (living(ob) && (funcall(#'IsEnemy,ob)
  //                        || ob->IsEnemy(this_object())));
//')));
}

int QueryNeededMv()
{
  int i,w;

  if (IS_LEARNING(this_object()) || !environment()) return 0;

  // w = wieviel Prozent dessen, was man tragen kann, traegt man?
  w=100*(QueryProp(P_TOTAL_WEIGHT)-QueryProp(P_WEIGHT)) /
    (QueryProp(P_MAX_WEIGHT) || 1);
  switch (w) {
    case  0..50: w=0; break;
    case 51..75: w=1; break;
    case 76..90: w=2; break;
    case 91..95: w=4; break;
    default:     w=6;
  }
  return w + environment()->QueryExhaustion(ME);
}

mixed _query_move_hook()
{
  return mappingp(Query(P_TMP_MOVE_HOOK))?m_values(Query(P_TMP_MOVE_HOOK)):({});
}

varargs void _set_move_hook(mixed h)
{mapping mh;
string *inds;
int lv;
  mh=Query(P_TMP_MOVE_HOOK);
  if(!mappingp(mh))mh=([]);
  if(objectp(h))
  {
    if(mh[h]==h)
      mh=m_delete(mh,h);
    else
      mh[h]=h;
    Set(P_TMP_MOVE_HOOK,mh);
    return;
  }
  if(pointerp(h)&&(sizeof(h)>=3))
  {
    mh[h[1]]=h;
    Set(P_TMP_MOVE_HOOK,mh);
  }
}

int TryMove(mixed dest, int methods)
{
  if (!(methods & (M_GO | M_NOCHECK | M_TPORT)))
    return ME_PLAYER;
  if (methods & M_NOCHECK) return 1;
  if ((methods & M_GO) && QueryProp(P_RESTING))
    return ME_RESTING;
  if (query_once_interactive(this_object()) &&
      (methods & M_GO) && QueryNeededMv() > QueryProp(P_MV))
    return ME_TOO_EXHAUSTED;
  if (methods & M_TPORT)
    if (environment() &&
	(environment()->QueryProp(P_NO_TPORT) & (NO_TPORT_OUT|NO_TPORT)))
      return ME_CANT_TPORT_OUT;
    else
      if (dest->QueryProp(P_NO_TPORT) & (NO_TPORT_IN|NO_TPORT))
        return ME_CANT_TPORT_IN;
  return 1;
}

varargs int move(mixed dest, int methods, string direction, string textout,
                 string textin)
{
  int ret, i,nightvis,quiet,lv;
  object enem, *enemies;
  mixed res,res2;
  
  if ((ret=TryMove(dest, methods))<0)
    return ret;

  if (res=QueryProp(P_TMP_MOVE_HOOK))
  {
for(lv=0;lv<sizeof(res);lv++)
{
    if (pointerp(res[lv]) && sizeof(res[lv])>=3
        && intp(res[lv][0]) && time()<res[lv][0]
        && objectp(res[lv][1]) && stringp(res[lv][2]))
    {
      if ((res2=call_other(res[lv][1],res[lv][2],dest,methods,direction,textout,
          textin)) && pointerp(res2) && sizeof(res2)==5)
      {
        dest=res2[0];
        methods=res2[1];
        direction=res2[2];
        textout=res2[3];
        textin=res2[4];
      }
    }
    else if(objectp(res[lv]))
      SetProp(P_TMP_MOVE_HOOK,res[lv]);
    else
      SetProp(P_TMP_MOVE_HOOK,res[lv][1]);
}
  }

  if (QueryProp(P_INVIS)) methods|=M_SILENT;
  
  quiet=UseSkill(SK_QUIETWALK);
  if(quiet>0) methods|=M_SILENT;
  
  if(environment())
  {
    if (set_light(0)>=1 && !(methods&M_SILENT))
    {
      string *mout;

      if (!textout)
      {
	if ((methods & M_TPORT))
	  textout=({string})QueryProp(P_MMSGOUT)||({string})QueryProp(P_MSGOUT);
	else 
          textout=(mout=explode(QueryProp(P_MSGOUT),"#"))[0]||
                           ({string})QueryProp(P_MMSGOUT);
      }
      if(direction=="") direction=0;
      say(Name(WER,2)+" "+textout+(direction?" "+direction:"")+
          (sizeof(mout)>1?mout[1]:"")+".\n");
    }
    
    enem=first_inventory(environment());
    while (enem)
    {
      if (living(enem) && StopHuntFor(enem,1))
      {
	tell_object(this_object(),
		    enem->Name()+" ist jetzt hinter Dir her.\n");
	tell_object(enem,"Du verfolgst jetzt "+
		    this_object()->name(WEN)+".\n");
	enem->InsertEnemy(this_object());
      }
      enem=next_inventory(enem);
    }
    if (!(methods & M_NOCHECK))
      environment()->exit();
  }

  move_object(dest);
  SetProp(P_PREPARED_SPELL,0); // Spruchvorbereitung abgebrochen
  dest = environment();

  nightvis=UseSkill(SK_NIGHTVISION);
  if(objectp(dest) && strstr(object_name(dest),"netztot") != -1)
    return 1;
  
  if (set_light(0)>=1 && !(methods & M_SILENT))
  {
    if (!textin)
    {
      if (methods & M_TPORT)
        textin=QueryProp(P_MMSGIN);
      else
        textin=QueryProp(P_MSGIN);
    }
    say(capitalize(name(WER,0))+" "+textin+".\n");
  }
  if (interactive(this_object()) && !(methods&M_NO_SHOW))
    if( (nightvis>=1 || set_light(0)>=1) && !QueryProp(P_BLIND) )
      tell_object(this_object(),""+this_object()->env_descr(1));
    else
      tell_object(this_object(),"Finsternis.\n");
      /*
      if ((QueryProp(P_BRIEF) & BRIEF_VMASK)==2)
        tell_object(this_object(),"!!");  // d.h. Raum ist dunkel
      else
      */

  // Begruessungsschlag fuer die Gegner
  if (living(this_object()) && !(methods & M_NO_ATTACK) &&
      !present("death_mark",this_object()) )
  {
    object *enem2;

    enemies = filter_objects( all_inventory(dest)-({ME}), "CheckEnemy", ME );
    enem2 = filter( all_inventory(dest)-({ME}), #'CheckEnemy); //');
    filter_objects( enemies, "Attack", this_object() );
    filter( enem2, #'Attack);//');
  }
  if (pointerp(Query(P_PURSUERS)) && sizeof(Query(P_PURSUERS)[1]))
  {
    while (remove_call_out("TakeFollowers")>=0);
    call_out("TakeFollowers",0);
  }
  return 1;
}

void TakeFollowers()
{
  mixed *f,env;
  int meth,i,r;
  
  f=Query(P_PURSUERS);
  if (!pointerp(f))
    return;
  env=environment();
  if(object_name(env) == "/room/netztot") return;
  f=f[1]-({0});
  for (i=sizeof(f)-1;i>=0;i--)
    if (environment(f[i])!=env)
    {
      meth=M_NOCHECK;
      if (f[i]->Query(P_FOLLOW_SILENT))
	meth|=M_SILENT|M_NO_SHOW;
      catch(r=f[i]->PreventFollow(env));
      if (!r)
	f[i]->move(env,meth);
      else
	if (r==2)
	  RemovePursuer(f[i]);
    }
}

varargs int remove()
{
  destruct(this_object());
  return 1;
}
