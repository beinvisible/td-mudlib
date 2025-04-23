// Tamedhon MUDlib
//
// CONTAINER/RESTRICTIONS.C -- container restrictions
//
// $Date: 2012/06/16 17:00:00 $
/* $Revision: 4.2 $
 * $Log: restrictions.c,v $
 *
 * Revision 4.3  2012/06/16 17:00:00  Grynnis
 * zwei verschiedene Bugfixes in present_objects() 
 * 1.) wenn das selbe Objekt mehrfach ausgewaehlt wird 
 *     "lege hose und helm und hose in beutel"
 * 2.) Arraysubtraktion mit ({0}) vermeiden 
 *     "lege alles ausser behaltenes..." im dem Fall wenn alles behalten ist
 *
 * Revision 4.2  2010/04/01 00:00:00  Grynnis
 * present_object: Angezogenes und Gezuecktes gehoeren auch zum Behaltenen
 *
 * Revision 4.1  2010/03/25 00:00:00  Grynnis
 * present_object erweitert (und, ausser, behaltenes)
 *
 * Revision 4.0  2003/09/09 22:32:00  Serii
 * nen klammer-fehler in MayAddWeight() beseitigt.
 *
 * Revision 3.4  1997/11/05 16:15:05  Woody
 * MayAddWeight() ueberarbeitet
 *
 * Revision 3.3  1997/10/16 15:45:20  Woody
 * MayAddWeight() ueberarbeitet (Objekt als Parameter, Gewichtstoleranz)
 *
 * Revision 3.2  1997/06/25 11:49:34  Woody
 * complex_id "waffe" raus, PreventLeave()
 *
 * Revision 3.1  1997/04/22 15:50:56  Feyaria
 * ruestung entfernt (gibt ja sachen mit id ruestung)
 *
 * Revision 3.0  1997/03/13 01:09:51  Woody
 * prepared for revision control
 *
 * Revision 2.9  1996/10/12  Woody
 * present_objects() versteht jetzt (ausser "alles") auch die Filter
 * "waffe(n)" und "ruestung(en)"
 *
 * Revision 2.8  1995/07/05  14:45:27  Jof
 * MayAddWeight beachtest, auch das environment (i.e. Spieler) (Rumata)
 *
 * Revision 2.7  1995/04/20  18:54:31  Jof
 * Implement caching of weightcontents
 *
 * Revision 2.6  1994/06/13  07:36:24  Jof
 * Removed SET_METHODs
 *
 * Revision 2.5  1994/05/15  18:10:34  Jof
 * cleanups
 *
 * Revision 2.4  1994/05/15  16:41:17  mud
 * Clean-Ups
 *
 * Revision 2.3  1994/03/21  16:48:13  Jof
 * Reindented files using emacs - once again
 *
 * Revision 2.2  1994/03/18  12:09:47  Jof
 * Fixed indentation
 *
 * Revision 2.1  1994/01/14  22:49:09  mud
 * fixed bug in create() : MAX -> TOTAL
 *
 * Revision 2.0  1994/01/13  16:59:58  mud
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/08  13:10:50  mud
 * #pragma strong_types
 *
 */

// This is a simple container to put objects in. It defines all functions
// which are necessary to describe an object which can be filled with
// other things.
//
// It will support restrictions for volume, weight etc.
//
// The following properties are defined:
// P_MAX_WEIGHT - maximum weight which container can carry
// P_TOTAL_WEIGHT - total weight, with contents.
// P_WEIGHT - the weight of the container without contents
//
// Functions for manipulation of weight
// MayAddWeight(weight) - Can <weight> be inserted?
// AddWeight(weight) - Add an amount of <weight>
//
// IMPORTANT: unit equals 1 gram

inherit "/std/thing/restrictions";

#pragma strong_types
#pragma save_types

#define NEED_PROTOTYPES

#include "/sys/thing/properties.h"
#include <properties.h>
#include <food.h>
#include <defines.h>

#define DEBUGGER "debugger"
#define TD(x) if(find_player(DEBUGGER)) tell_object(find_player(DEBUGGER),x)

// local properties prototypes
static int _query_total_weight();

void create()
{
  Set(P_WEIGHT_PERCENT, 50);
  Set(P_TOTAL_WEIGHT, -1, F_SET_METHOD);
  Set(P_TOTAL_WEIGHT, PROTECTED, F_MODE);
}

static int LastWeightCalc,contents;

int query_weight_contents()
{
  object *objs;
  int w,w2,i;

  if (Query(P_LAST_CONTENT_CHANGE)<LastWeightCalc) return contents;
  w=0;
  objs=all_inventory(this_object());
  i=sizeof(objs);
  while (i--)
  {
    if (!(w2=objs[i]->QueryProp(P_TOTAL_WEIGHT)))
      w2=objs[i]->QueryProp(P_WEIGHT);
    w+=w2;
  }
  LastWeightCalc=time();
  return contents=w;
}

varargs int MayAddWeight(mixed weight)
{
  int w;
  object env;
  if (intp(weight)) {
    if (weight<=0) return 0;
    w=weight;
  }
  else if (objectp(weight)) {
    w=(weight->IsUnit()) ? (weight->QueryProp(P_WEIGHT)) :
      (weight->QueryProp(P_TOTAL_WEIGHT) || weight->QueryProp(P_WEIGHT));
    env=environment(weight);
    // 1. Fall: z.B. "steck x (in mir) in beutel (in mir)"
    if (env && (env==environment()))
    {
      if (query_weight_contents() + w > ME->QueryProp(P_MAX_WEIGHT))
        return -1;
      return (QueryProp(P_WEIGHT_PERCENT)<=100) ? 0 :
             env->MayAddWeight(w*QueryProp(P_WEIGHT_PERCENT)/100-w);
    }
    // 2. Fall: z.B. "nimm x aus beutel (in mir)"
    else if (env && (environment(env)==ME))
      w-=w*env->QueryProp(P_WEIGHT_PERCENT)/100;
  }
  else
    return 0;
  if (query_weight_contents() + w > ME->QueryProp(P_MAX_WEIGHT))
    return -1;
  if (environment() &&
      (environment()->MayAddWeight(w*QueryProp(P_WEIGHT_PERCENT)/100) < 0))
    return -2;
  return 0;
}

/* Redefine PreventInsert() to prevent inserting of special objects. If
 * a value greater 0 is returned the object ob can't be inserted in the
 * container.
 */
varargs int PreventInsert(object ob) { return 0; }
varargs int PreventLeave(object ob, mixed dest) { return 0; }

//Hilfsfunktionen fuer present_objects()
static int _behalten( object ob, string idstr )
{
  return ({string})ob->QueryProp(P_KEEP_ON_SELL) == idstr;
}

static int _id( object ob, string idstr )
{
  if (ob->IsUnit() && (member(ob->QueryProp("u_ids")[0],idstr)!=-1)) {
    ob->Set(P_REQUESTED_AMOUNT,ob->Query(P_AMOUNT));
    return 1;
  }
  return ob->id(idstr);
}

/*
 * get a list of all contained objects matching a complex description
 */
object *present_objects(string complex_desc)
{
  object ob, *obs, *erg;
  int i;
  string *strlst;

  strlst = allocate(2);

  if ( sscanf( complex_desc, "%s außer %s", strlst[0], strlst[1]) == 2 )
  {
    return present_objects( strlst[0] ) - present_objects( strlst[1] );
  }

  strlst = explode( complex_desc, " und " );
  erg = ({});

  for (i=0; i<sizeof(strlst); i++)
  {
    complex_desc = strlst[i];
    obs=({});
    switch (complex_desc)
    {
      case "allem":
      case "alles":
      case "alle":
          obs = all_inventory();
          break;
      case "essen":
      case "alles essen":
      case "allem essen":
      case "jedes essen":
      case "jedem essen":
          obs = filter_objects(all_inventory(),"QueryProp", P_FOOD_INFO);
          break;
      case "waffen":
      case "alle waffen":
      case "allen waffen":
      case "jede waffe":
      case "jeder waffe":
          obs = filter_objects(all_inventory(),"QueryProp", P_WEAPON_TYPE);
          break;
      case "rüstungen":
      case "alle rüstungen":
      case "allen rüstungen":
      case "jede rüstung":
      case "jeder rüstung":
      case "ruestungen":
      case "alle ruestungen":
      case "allen ruestungen":
      case "jede ruestung":
      case "jeder ruestung":
          obs = filter_objects(all_inventory(),"QueryProp", P_ARMOUR_TYPE);
          break;
      case "verschiedenes":
      case "verschiedenem":
      case "sonstiges":
      case "sonstigem":
          obs = all_inventory();
          obs -= filter_objects(all_inventory(),"QueryProp", P_WEAPON_TYPE);
          obs -= filter_objects(all_inventory(),"QueryProp", P_ARMOUR_TYPE);
          break;
      case "behaltene":
      case "behaltenes":
      case "behaltenen":
      case "behaltenem":
      case "alles behaltene":
          obs = filter(all_inventory(),"_behalten", this_object(),
                               getuid( this_player() || previous_object() ));
          obs += (QueryProp(P_ARMOURS) || ({}))
              + ({ QueryProp(P_WEAPON) }) - ({ 0 });
          break;
      default:
          if ( complex_desc[0..3] == "jede" ||
               complex_desc[0..4] == "alle ")
          {
              if ( complex_desc[4..4] == " " )
              {
                  obs = filter( all_inventory(), "_id", this_object(),
                                      complex_desc[5..] );
                  break;
              }
              if ( complex_desc[4..5] == "m " ||
                   complex_desc[4..5] == "n " ||
                   complex_desc[4..5] == "r " ||
                   complex_desc[4..5] == "s ")
              {
                  obs = filter( all_inventory(), "_id", this_object(),
                                      complex_desc[6..] );
              }
              break;
          }
          ob = present( complex_desc, this_object() );
          if ( ob && ob != this_object() )
          {
              erg -= ({ ob });
              erg += ({ ob });
          }
          continue;
    }
    erg -= obs;
    erg += obs;
  }
  erg = filter_objects(erg, "short");
  erg -= filter_objects(erg, "QueryProp", P_INVIS);
  return erg;
}

/*
 * returns a list of all found objects inside itself
 * may call same function in objects inside
 */
object *locate_objects(string complex_desc,int info)
{
  object *search_obs;
  object *found_obs;
  object *found_i;
  int i;
  string was, wo;

  found_obs = ({});

  if( sscanf( complex_desc, "%s in %s", was, wo ) == 2 )
  {
    search_obs = present_objects( wo );
    for( i=0; i<sizeof(search_obs); i++ )
    {
      found_i = search_obs[i]->locate_objects( was, info );
      if( pointerp(found_i) )
	found_obs += found_i;
    }
    return found_obs;
  }
  // kein "in" gefunden
  found_obs = present_objects(complex_desc);
  return found_obs;
}

// **** local property methods
static int _query_total_weight()
{
  return QueryProp(P_WEIGHT)+
    (QueryProp(P_WEIGHT_PERCENT)*query_weight_contents()/100);
}
