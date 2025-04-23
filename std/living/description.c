// Tamedhon MUDlib
//
// LIVING/DESCRIPTION.C -- description for living objects
//
// $Date: 1997/09/25 15:07:51 $
/* $Revision: 2.2 $
 * $Log: description.c,v $
 * Revision 2.3  2002/02/27 18:33:00 Gralkor
 * neue Fitnessmeldungen, damit feiner unterteilt werden kann
 *
 * Revision 2.2  1997/09/25 15:07:51  Woody
 * _query_material()
 *
 * Revision 2.1  1997/06/25 11:53:32  Woody
 * unnoetiges short() entfernt, neu: is_class_member()
 *
 * Revision 2.0  1997/03/13 00:54:07  Woody
 * prepared for revision control
 *
 * Revision 1.1  96/05/25  18:00:00  Woody
 * short() Aenderungen wieder retour
 *
 * Revision 1.0  96/02/22  23:00:00  Hadra
 * short() geaendert
 */                            

#include "/std/sys_debug.h"

inherit "/std/container/description";

#pragma strong_types

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <properties.h>
#include <language.h>
#include <defines.h>
#include <class.h>

void create()
{
  ::create();
  Set(P_GENDER, SAVE, F_MODE);
}
/* 
varags string long() {
 string str, cap_pronoun;
 string descr, invl;
 int hpnt, max_hpnt;
 mixed filter;

 descr = "";
 cap_pronoun = capitalize(QueryPronoun());
 str = funcall(QueryProp(P_LONG));
 if(!stringp(str)) str = "";
 
 fitness();
 
 str += descr;
 if(filter = QueryProp(P_TRANSPARENT))
   {
    invl = make_invlist(PL, all_inventory(ME),
    this_player()->QueryProp(P_SHORTINV) ? 0 : 2);
    if(invl != "")
    str += capitalize(QueryPronoun())+" traegt bei sich:\n" + invl;
   }
 return str;
}
			    
*/

varargs string long() {
  string str, cap_pronoun;
  string descr, invl;
  int hpnt, max_hpnt;
  mixed filter;

  descr = "";
  cap_pronoun = capitalize(QueryPronoun());
  str = funcall(QueryProp(P_LONG));
/* fitness() { */  
  if(!stringp(str)) str = "";
  hpnt = QueryProp( P_HP );
  max_hpnt = QueryProp( P_MAX_HP );
  if (hpnt<3) 
    descr += cap_pronoun + " wird den nächsten Schlag nicht überleben.\n";
  else if (hpnt < max_hpnt*1/41) 
    descr += cap_pronoun + " kann sich nicht mehr halten.\n";
  else if (hpnt < max_hpnt*2/41)
    descr += cap_pronoun + " ist so gut wie erledigt.\n";
  else if (hpnt < max_hpnt*3/41)
    descr += cap_pronoun + " ist mehr tot als lebendig.\n";
  else if (hpnt < max_hpnt*4/41)
    descr += cap_pronoun + " kann sich kaum noch auf den Beinen halten.\n";    
  else if (hpnt < max_hpnt*5/41)
    descr += cap_pronoun + " wankt schwer keuchend hin und her.\n";
  else if (hpnt < max_hpnt*6/41)
    descr += cap_pronoun + " spuckt bereits Blut.\n";
  else if (hpnt < max_hpnt*7/41)
    descr += cap_pronoun + " röchelt erbärmlich.\n";
  else if (hpnt < max_hpnt*8/41)
    descr += cap_pronoun + " hält nicht mehr lange durch.\n";
  else if (hpnt < max_hpnt*9/41)
    descr += cap_pronoun + " benötigt dringendst einen Heiler.\n";
  else if (hpnt < max_hpnt*10/41)
    descr += cap_pronoun + " merkt, dass es sich dem Ende zuneigt.\n";
  else if (hpnt < max_hpnt*11/41)
    descr += cap_pronoun + " war schon mal in besserer Verfassung.\n";
  else if (hpnt < max_hpnt*12/41)
    descr += cap_pronoun + " schwankt ziemlich hin und her.\n";
  else if (hpnt < max_hpnt*13/41)
    descr += cap_pronoun + " ist bereits stark geschwächt.\n";
  else if (hpnt < max_hpnt*14/41)
    descr += cap_pronoun + " blutet stark aus mehreren schweren Wunden.\n";
  else if (hpnt < max_hpnt*15/41)
    descr += cap_pronoun + " hat mehr als eine schwere Wunde abbekommen.\n";
  else if (hpnt < max_hpnt*16/41)
    descr += cap_pronoun + " ist leicht benommen.\n";
  else if (hpnt < max_hpnt*17/41)
    descr += cap_pronoun + " hat große Schmerzen.\n";
  else if (hpnt < max_hpnt*18/41)
    descr += cap_pronoun + " zittert am ganzen Körper.\n";
  else if (hpnt < max_hpnt*19/41)
    descr += cap_pronoun + " schwankt leicht hin und her.\n";
  else if (hpnt < max_hpnt*20/41)
    descr += cap_pronoun + " zittert leicht.\n";
  else if (hpnt < max_hpnt*21/41)
    descr += cap_pronoun + " sieht nicht mehr fit aus.\n";
  else if (hpnt < max_hpnt*22/41)
    descr += cap_pronoun + " ist angeschlagen.\n";
  else if (hpnt < max_hpnt*23/41)
    descr += cap_pronoun + " ist leicht angeschlagen.\n";
  else if (hpnt < max_hpnt*24/41)
    descr += cap_pronoun + " ist geschwächt.\n";
  else if (hpnt < max_hpnt*25/41)
    descr += cap_pronoun + " macht einen mitgenommenen Eindruck.\n";
  else if (hpnt < max_hpnt*26/41)
    descr += cap_pronoun + " sieht nicht übermäßig fit aus.\n";
  else if (hpnt < max_hpnt*27/41)
    descr += cap_pronoun + " ist trotz leichter Verletzungen noch fit.\n";
  else if (hpnt < max_hpnt*28/41)
    descr += cap_pronoun + " ist etwas bleich geworden.\n";
  else if (hpnt < max_hpnt*29/41)
    descr += cap_pronoun + " blutet aus mehreren Wunden.\n";
  else if (hpnt < max_hpnt*30/41)
    descr += cap_pronoun + " blutet aus einer Wunde.\n";
  else if (hpnt < max_hpnt*31/41)
    descr += cap_pronoun + " hat mehrere Blessuren abbekommen.\n";
  else if (hpnt < max_hpnt* 32/41)
    descr += cap_pronoun + " ist leicht verwundet.\n";
  else if (hpnt < max_hpnt* 33/41)
    descr += cap_pronoun + " hat eine leichte Wunde.\n";
  else if (hpnt < max_hpnt* 34/41)
    descr += cap_pronoun + " ist leicht verletzt.\n";
  else if (hpnt < max_hpnt* 35/41)
    descr += cap_pronoun + " ist kaum merklich verletzt.\n";
  else if (hpnt < max_hpnt* 36/41)
    descr += cap_pronoun + " ist geringfügig verletzt.\n";
  else if (hpnt < max_hpnt* 37/41)
    descr += cap_pronoun + " ist leicht geschwächt.\n";
  else if (hpnt < max_hpnt* 38/41)
    descr += cap_pronoun + " ist leicht zerkratzt.\n";
  else if (hpnt < max_hpnt* 39/41)
    descr += cap_pronoun + " hat mehrere Kratzer abbekommen.\n";
  else if (hpnt < max_hpnt* 40/41)
    descr += cap_pronoun + " hat gerade mal einen Kratzer.\n";
  else if (hpnt < max_hpnt)                    
    descr += cap_pronoun + " ist nahezu unverletzt.\n";  
  else
    descr += cap_pronoun + " ist absolut topfit.\n";
  str += descr;
  /* return str;
  }
  */
  if(filter = QueryProp(P_TRANSPARENT))
  {
    invl = make_invlist(PL, all_inventory(ME),
      this_player()->QueryProp(P_SHORTINV) ? 0 : 2);
    if(invl != "")
      str += capitalize(QueryPronoun())+" trägt bei sich:\n" + invl;
  }
  return str;
}

#define DD(x) tell_object(find_player("debugger"), (x))

varargs string name(int casus, int demonst)
{
  string s;
  
// DD(sprintf("name: me = %O, prev = %O\n", this_object(), previous_object()));
  
  if( QueryProp( P_INVIS ) )
  {
    if( casus == RAW ) return "Jemand";
    return ({"Jemand","Jemands","Jemandem","Jemanden"})[casus];
  }
  if (QueryProp(P_FROG) && casus != RAW )
  {
    SetProp(P_ARTICLE,1);
    s=QueryArticle(casus,0)+"Frosch";
    if (casus==WESSEN) s += "es";
    SetProp(P_ARTICLE,0);
    return s;
  }
  return ::name( casus, demonst );
}

static int _query_gender()
{
  if (QueryProp(P_FROG)) return MALE;
  return Query(P_GENDER);
}

static mixed _set_name(mixed nm )
{
  string lvnam;
  lvnam = nm;
  if(pointerp(nm)) lvnam = nm[0];
  set_living_name(lower_case(lvnam));
  return Set(P_NAME, nm);
}

int _query_container()
{
  return 0;
}

int is_class_member(mixed str) {
  if (::is_class_member(str))
    return 1;

  if (stringp(str))
    str = ({str});

  if (!pointerp(str))
    return 0;

  if ((member(str, CL_UNDEAD) >= 0) &&
      ::is_class_member( ({CL_ZOMBIE, CL_SKELETON}) ))
    return 1;
  else if ((member(str, CL_GOBLIN) >= 0) &&
           ::is_class_member( ({ CL_HOBGOBLIN }) ))
    return 1;
  else
    return 0;
}

mapping _query_material() {
  mixed res;

  if (mappingp(res=Query(P_MATERIAL)))
    return res;
  return ([MAT_MISC_LIVING:100]);
}
