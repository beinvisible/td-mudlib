// Tamedhon Mudlib
//
// std/unit.c -- Standard Unit Objekt
//
// $Date: 2008/07/31 14:00:00 $
/* $Revision: 1.14 $
 * $Log: unit.c,v $
 *
 * Revision 1.14  2008/07/31 14:00:00  TamTam
 * Bei Casus RAW kein Artikel mehr erzwingen
 *
 * Revision 1.13  2007/12/30 16:00:00  TamTam
 * IsPlural() - Mehrzahl?
 *
 * Revision 1.12  2004/10/28 20:20:00  Serii
 * long() - break_string() nach process_string()
 *
 * Revision 1.11  1997/10/19 19:40:43  Woody
 * ClonedOther() -> NotifyClone(), #define U_REQ P_REQUESTED_AMOUNT
 *
 * Revision 1.10  1997/10/16 15:44:05  Woody
 * ClonedOther(), P_NODROP, P_NOGET Handling, MayAddWeight() Objekt uebergeben
 *
 * Revision 1.9  1997/10/14 14:56:11  Woody
 * Units muessen um sich zu vereinigen nun auch gleiche P_SHORT haben
 *
 * Revision 1.8  1997/09/17 14:19:59  Woody
 * QueryPronoun() eingefuegt
 *
 * Revision 1.7  1997/08/30 21:57:17  Woody
 * Minor Bugfix in _set_amount() (auf <1 statt auf ==0 pruefen)
 *
 * Revision 1.6  1997/06/25 11:36:08  Woody
 * AddSingularId nimmt auch Arrays entgegen, neu: RemoveSingularId,
 * dito fuer AddPluralId() und RemovePluralId()
 *
 * Revision 1.5  1997/03/13 03:20:18  Woody
 * prepared for revision control
 *
 * Revision 1.4  96/10/07  Woody
 * P_GIVEN_OBJECT auch setzen, wenn Empfaenger Objekt bereits hatte und
 * Geber nicht alles von seinem Objekt hergibt; ausserdem in diesem
 * Fall auch P_GIVEN_AMOUNT setzen.
 *
 * Revision 1.3  96/05/25  18:00:00  Woody
 * short() Aenderungen wieder retour
 *
 * Revision 1.2  96/04/19  01:00:00  Woody
 * BUGFIX (err= Klammerung war falsch bei other unit clone),
 * check_leave() bugfix
 *
 * Revision 1.1  96/04/07  17:30:00  Woody
 * P_GIVEN_OBJECT entsprechend MG mudlib implementiert
 *
 * Revision 1.0  22/02/96  23:00:00  Hadra
 * short() geaendert
 */

inherit "std/thing";

#define NEED_PROTOTYPES

#define TW(x) tell_object(find_player("woody"), x+"\n")

#include <properties.h>
#include <thing/properties.h>
#include <defines.h>
#include <language.h>
#include <moving.h>

#define U_GPU   "u_gpu"
#define U_CPU   "u_cpu"
#define U_IDS   "u_ids"
#define U_REQ   P_REQUESTED_AMOUNT

static string lastverb;

static void check_leave()
{
  if (Query(P_AMOUNT)>0) return;  // woody
  remove();
}

static void envchk()
{
  if (!environment())
    remove();
}

void create()
{
  ::create();
  Set(U_IDS,({({}),({})}));
  Set(P_AMOUNT,1);
  Set(U_GPU,({1,1}));
  Set(U_CPU,({0,1}));
  SetProp(P_ARTICLE,0);
  Set(U_REQ,1);
  call_out("envchk",1);
  AddId("Unit");
}

void reset()
{
  if (!Query(P_AMOUNT))
    remove();
}

static void _set_name(mixed names)
{
  if(!names)
    return Set(P_NAME,({"",""}));
  if(stringp(names))
    return Set(P_NAME,({names,names}));
  if(pointerp(names))
    switch(sizeof(names))
    {
      case 0:
        return Set(P_NAME,({"",""}));
      case 1:
	if(!stringp(names[0]))
	   names[0]="";
	return Set(P_NAME,({names[0],names[0]}));
      default:
	if(!stringp(names[0]))
	   names[0]="";
	if(!stringp(names[1]))
	   names[1]="";
	return Set(P_NAME,names[0..1]);
      }
}

static string _query_name()
{
  if (Query(U_REQ)==1)
    return (Query(P_NAME))[0];
  return (Query(P_NAME))[1];
}

void AddSingularId(mixed str)
{
  string *ids;

  if (!pointerp(str))
    str=({str});
  ids=Query(U_IDS);
  Set(U_IDS,({ids[0]+str,ids[1]}));
}

void RemoveSingularId(mixed str)
{ if(stringp(str))
    str=({str});
  if(pointerp(str))
    return Set(U_IDS,({Query(U_IDS)[0]-str,Query(U_IDS)[1]}));
}

void AddPluralId(mixed str)
{
  string *ids;

  if (!pointerp(str))
    str=({str});
  ids=Query(U_IDS);
  Set(U_IDS,({ids[0],ids[1]+str}));
}

void RemovePluralId(mixed str)
{ if(stringp(str))
    str=({str});
  if(pointerp(str))
    return Set(U_IDS,({Query(U_IDS)[0],Query(U_IDS)[1]-str}));
}

varargs void SetCoinsPerUnits(int coins,int units)
{
  if (coins<0||units<=0)
    return;
  Set(U_CPU,({coins,units}));
}

varargs void SetGramsPerUnits(int grams,int units)
{
  if (grams<0||units<=0)
    return;
  Set(U_GPU,({grams,units}));
}

int *QueryCoinsPerUnits()
{
  return Query(U_CPU);
}

int *QueryGramsPerUnits()
{
  return Query(U_GPU);
}

varargs string name(int fall, int demo)
{
	int req;

  if (!(req=Query(U_REQ)))
    return 0;
  if (req==1)
    return (fall!=RAW?QueryArticle(fall,demo,1):"")+Query(P_NAME)[0];
  else
    return sprintf("%d %s",req,Query(P_NAME)[1]);
}

string short()
{
  int req;

  req=Query(U_REQ);
  if (!lastverb||query_verb()!=lastverb)
    Set(U_REQ,req=Query(P_AMOUNT));
  if (!req)
    return 0;
  if (req==1)
    return sprintf("%s%s.\n",capitalize( QueryArticle(WER,0,1)),
		   Query(P_NAME)[0]);
  else
    return sprintf("%d %s.\n",req,Query(P_NAME)[1]);
}

varargs string long()
{
  int req;

  req = Query(U_REQ);
  if (QueryProp(P_LONG))
    return break_string(process_string(QueryProp(P_LONG))) +
           sprintf("Davon siehst Du %s.\n",
                  (req==1 ? DeclAdj("ein", WEN)[0..<2] : to_string(req)));
  else
    return sprintf("Du siehst %s.\n",name(WEN));
}

varargs int id(string str)
{
  int i,amount;
  string s1,s2,*ids;

  if (!str)
    return 0;
  lastverb=query_verb();
  if (::id(str))
  {
    Set(U_REQ,Query(P_AMOUNT));
    return 1;
  }
  amount=Query(P_AMOUNT);
  if (!amount||!(ids=Query(U_IDS)))
    return 0;
  if (match_item(str,ids[1]))
  {
    Set(U_REQ,amount);
    return 1;
  }
  if (match_item(str,ids[0]))
  {
    Set(U_REQ,1);
    return 1;
  }
  if (sscanf(str,"%s %s",s1,s2) && s1[0..3]=="alle" &&
      match_item(s2,ids[1]))
  {
    Set(U_REQ,amount);
    return 1;
  }
  if (sscanf(str,"%d %s",i,s1)==2 && i==1 &&
      match_item(s1,ids[0]))
  {
    Set(U_REQ,1);
    return 1;
  }
  if (sscanf(str,"%d %s",i,s1)==2 && match_item(s1,ids[1]) &&
      i<=amount && i>0)
  {
    Set(U_REQ,i);
    return 1;
  }
  lastverb=0;
  return 0;
}

static int _query_value()
{
  int *cpu;

  cpu=Query(U_CPU);
  return (Query(U_REQ)*cpu[0])/cpu[1];
}

static int _query_weight()
{
  int *gpu;
	int req;

  gpu=Query(U_GPU);
  if ((req=Query(U_REQ))<=0)
    return req; // Hier ists egal - aufaddiert wird ja nun nicht mehr ...
  return MAX(1,(req*gpu[0])/gpu[1]);
}

static int _query_total_weight()
{
  int *gpu;
	int req;

  gpu=Query(U_GPU);
  if ((req=Query(P_AMOUNT))<=0)
    return req; // Hier ists egal - aufaddiert wird ja nun nicht mehr ...
  return MAX(1,(req*gpu[0])/gpu[1]);
}

static void _set_amount(int am)
{
  if (am < 1)
    call_out("check_leave",1);
  Set(P_AMOUNT,am);
  Set(U_REQ,am);
}

void AddAmount(int am)
{
  _set_amount(am+Query(P_AMOUNT));
}

void NotifyClone(object other) {  }

varargs int move(mixed dest, int method)
{
  object other, *inv;
  int err;
  string tf, short;
  int amount;
  int req;

  amount=Query(P_AMOUNT);
  req=Query(U_REQ);
  if (!intp(method)) method=0;
  if (!dest) dest=environment();
  if (!dest) return 0;
  if (method & (M_MOVE_ALL|M_FORCE_SPLIT)) req=amount;
  if (method & M_NOCHECK) return ::move(dest,method);
  if (!req)
  {
    Set(U_REQ,0);
    return 1;
  }

  if (QueryProp(P_NODROP) &&  (method & (M_PUT|M_GIVE)))
    return ME_CANT_BE_DROPPED;
  if (QueryProp(P_NOGET) &&  (method & M_GET))
    return ME_CANT_BE_TAKEN;

  Set(P_GIVEN_AMOUNT,req);
  if (!objectp(dest)) other=find_object(dest);
  if (other) { dest=other; other=0; }
  other=this_object();
  while (other=environment(other)) other->Set(P_LAST_CONTENT_CHANGE,time());
  other=dest;
  while (other)
  {
    other->Set(P_LAST_CONTENT_CHANGE,time());
    other=environment(other);
  }
  if (objectp(dest)) inv=all_inventory(dest); else inv=({});
  tf=explode(object_name(ME),"#")[0];
  short=Query(P_SHORT);
  while (sizeof(inv) && !other)
    if ((explode(object_name(inv[0]),"#")[0]==tf) &&
        (short==inv[0]->Query(P_SHORT)))
      other=inv[0];
    else
      inv=inv[1..];
  if (other)
    other->check_leave();
  if (!other)
    if (req==amount) {
      Set(P_GIVEN_OBJECT, this_object());  // woody
      return ::move(dest);
    }
    else
    {
      other=clone_object(tf);
      other->SetProp(P_AMOUNT,req);  // woody
      Set(P_GIVEN_OBJECT, other);    // woody
      NotifyClone(other);
      lastverb=0;
      if ((err=other->move(dest,M_MOVE_ALL))>0)  // woody
      {
	Set(P_AMOUNT,amount-req);
	call_out("check_leave",1);
	return 1;
      }
      other->remove();
      return err;
    }

  if (dest->MayAddWeight(this_object()))
    return ME_TOO_HEAVY;
  if (dest->PreventInsert(this_object()))
    return ME_CANT_BE_INSERTED;
  if (environment() && environment()->PreventLeave(this_object(),dest))
    return ME_CANT_LEAVE_ENV;

  Set(P_AMOUNT,amount-req);
  // Set(P_GIVEN_OBJECT, -1);  // woody
  Set(P_GIVEN_OBJECT, other);      // woody, oct 07
  other->Set(P_GIVEN_AMOUNT,req);  // woody, oct 07
  other->AddAmount(req);
  lastverb=0;
  call_out("check_leave",1);
  return 1;
}

int IsUnit()
{
  return 1;
}

int IsPlural()
{
  return Query(U_REQ) != 1;
}

string QueryPronoun(int casus)
{
  if (Query(U_REQ) == 1)
    return ::QueryPronoun(casus);

  switch(casus) {
    case WESSEN: return "ihrer";
    case WEM:    return "ihnen";
    default:     return "sie";
  }
}

