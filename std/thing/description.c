// MorgenGrauen MUDlib
//
// std/thing/description.c -- description handling for standard objects
//
// $Date: 2003/10/22 11:00:00 $                      
/* $Revision: 5.0 $       
 * $Log: description.c,v $
 * Revision 5.0  2003/10/22 11:00:00  Serii
 * P_REPAIRS wird hier initialisiert
 *
 * Revision 4.10  2001/02/10 12:06:33  Samtpfote
 * prepared for revision control
 *
 * Revision 4.10  1997/09/25 15:09:57  Woody
 * inherited nun std/thing/material, _set_material() und Set(MAT...) hier raus
 *
 * Revision 4.9  1997/08/29 21:42:04  Woody
 * Im Rassenmapping eines Details darf man jetzt auch eine Closure angeben,
 * der autom. Umbruch erfolgt nun mit leave_lfs und ist mit \t unterdrueckbar
 *
 * Revision 4.8  1997/03/30 18:44:48  Woody
 * name() aufgeraeumt, Name() eingefuehrt
 *
 * Revision 4.7  1997/03/13 02:23:35  Woody
 * id() Bugfix fuer id("ding -1") usw. (negative Indizes)
 *
 * Revision 4.6  1997/03/13 02:20:21  Woody
 * prepared for revision control
 *
 * Revision 4.5  1997/02/10 23:55:00  Woody
 *   - read() (das ich ja am liebsten ganz rausgeschmissen haette)
 *     gibt jetzt auch das Ergebnis von lies() zurueck
 *
 * Revision 4.4  1996/09/26 05:00:00  Woody
 *   - Handling von Exploration Points eingefuehrt
 *
 * Revision 4.3  1996/09/02 21:00:00  Woody
 *   - AddDetail() und AddReadDetail() sorgen automatisch fuer
 *     Zeilenumbrueche, wenn kein \n im uebergebenen String
 *     vorkommt.
 *
 * Revision 4.2  1996/05/26 15:00:00  Woody
 *   - P_SMELL und P_SOUND eingefuehrt
 *     (incl. GetSmell() und GetSound(), rassenspezifisch)
 *
 * Revision 4.1  1996/05/25 18:00:00  Woody
 *   - short() wieder geaendert (nun wieder mit \n)
 *
 * Revision 4.0  1996/04/16 01:00:00  Woody
 * AL-Erweiterungen eingearbeitet:
 *   - _query_material entfernt, dafuer Set(P_MATERIAL, ({MAT_MISC}))
 *     im 'create'
 *   - short() geaendert
 *
 * Revision 3.3  1996/02/23 12:42:19  Wargon
 * name(): RAW wird auch als demon akzeptiert, ausserdem werden
 *   Namensadjektive auch bei Objekten mit P_ARTICLE=0 ausgewertet.
 *
 * Revision 3.2  1996/01/24 00:02:30  Hate
 * fixed bug with read messages (lies command)
 *
 * Revision 3.1  1996/01/23  10:09:42  Hate
 * moved DETAIL handling from room to this file
 *
 * Revision 3.0  1995/11/22 14:32:42  Jof
 * Updated to revision 3.0
 *
 * Revision 2.33  1995/11/22 14:19:50  Jof
 * Fixed RCS header
 *
 * Revision 2.32  1995/11/13 14:57:05  Wargon
 * name(RAW) wird vor der INVIS-Behandlung ausgewertet.
 *
 * Revision 2.31  1995/11/10 13:04:03  Wargon
 * Am CLASS-Kram rumgefummelt...
 *
 * Revision 2.30  1995/11/08 18:11:10  Rochus
 * is_class_member() und AddToClass() eingefuehrt, siehe Doku :)
 *
 * Revision 2.29  1995/08/07 19:06:37  Wargon
 * RemoveId() und RemoveAdjective()
 *
 * Revision 2.28  1995/06/14  09:01:17  Wargon
 * read() nach hier verlegt.
 *
 * Revision 2.27  1995/03/30  13:48:07  Jof
 * ???
 *
 * Revision 2.26  1995/02/16  19:09:50  Rumata
 * *** empty log message ***
 *
 * Revision 2.25  1994/10/04  09:22:39  Jof
 * *** empty log message ***
 *
 * Revision 2.24  1994/09/28  17:46:25  Jof
 * INVIS Abfrage in short
 *
 * Revision 2.23  1994/09/23  20:20:55  Hate
 * removed SetProp(P_WEIGHT, 1000) -> moved to restrictions
 *
 * Revision 2.22  1994/06/27  14:38:05  Hate
 * bugfix in name()
 *
 * Revision 2.22  1994/06/27  14:38:05  Hate
 * bugfix in name()
 *
 * Revision 2.20  1994/06/16  19:58:45  Hate
 * added multiple active adjectives (P_NAME_ADJ)
 *
 * Revision 2.18  1994/06/13  07:36:24  Jof
 * Removed SET_METHODs
 *
 * Revision 2.17  1994/06/09  17:52:02  Jof
 * ID bug once more
 *
 * Revision 2.16  1994/06/08  19:54:28  Jof
 * Bug in id()
 *
 * Revision 2.15  1994/04/13  12:04:14  Kirk
 * P_CLONER secured
 *
 * Revision 2.14  1994/04/07  10:51:29  Rumata
 * RAW added
 */                            

#pragma strong_types

inherit "std/thing/material";
 
#define NEED_PROTOTYPES 1

#include <thing/properties.h>
#include <thing/description.h>
#include <thing/material.h>
#include <properties.h>
#include <language.h>
//#include <std_headers.h>
#include <defines.h>
#include <exploration.h>

#include "/std/sys_debug.h"

// prototypes for local properties methods
static mixed _set_adjectives(mixed adjective);
static mixed _set_ids(mixed ids);
static mixed _set_class(mixed classes);
static <float|int> _set_light(<float|int> light);

private static string *explore;

void create()
{ 
  string poid, tpid;
  object po, tp;


  SetProp(P_SHORT,"Nichts besonderes");
  SetProp(P_LONG,0);

  Set(P_ADJECTIVES,({}));
  Set(P_NAME_ADJ,({}));
  Set(P_IDS, ({"Ding"}));
  Set(P_CLASS, ({}));
  Set(P_LIGHT, 0);
  Set(P_NO_INVIS, 0);
  Set(P_SMELL, 0);
  Set(P_SOUND, 0);

  Set(P_REPAIRS,SAVE,F_MODE_AS);
  Set(P_REPAIRS,100);

  explore = EPMASTER->QueryExplore(EP_DETAIL);
  SetProp(P_DETAILS, ([]));
  SetProp(P_READ_DETAILS, ([]));

  po=previous_object();
  if (po && !(poid=geteuid(po))) poid=getuid(po);
  if (!(tp=this_interactive())) tp=this_player();
  if (tp && !(tpid=geteuid(tp))) tpid=getuid(tp);
  if (!tpid) tpid="UNKNOWN";
  if (!poid) poid="UNKNOWN";
  Set(P_CLONER,(poid!=tpid ? poid+":"+tpid : tpid));
  Set(P_CLONER,-1,F_SET_METHOD);
  Set(P_CLONER,SECURED,F_MODE);
}

void init() { add_action("lies", "lies"); }

/*
 * id management 
 */

int match_item(string str, string *ids);

varargs int id(string str, int lvl) 
{ 
  string str2, tmp;
  int count;	
  mixed ids;

  if (!stringp(str)) return 0;

  ids = QueryProp(P_IDS);
  if (!pointerp(ids) || !sizeof(ids))
    return 0;
  if (match_item(str,ids)) return 1;
  if (sscanf(str,"%s %d%s",str2,count,tmp)>=2 && (!tmp||tmp=="") && (count>0))
  {
    if (match_item(str2,ids) && environment(ME))
      return
        present(sprintf("%s %d",str2,count), environment(ME))==this_object();
  }
  return 0;
}

void AddId(mixed str)
{
  if(stringp(str)) str = ({ str });
  if(pointerp(str)) return Set(P_IDS, Query(P_IDS)+str);
}

void RemoveId(mixed str)
{
  if(stringp(str)) str = ({ str });
  if(pointerp(str)) return Set(P_IDS, Query(P_IDS)-str);
}

void AddClass(mixed str)
{
  if(stringp(str)) str = ({ str });
  
  /* multiple Class-IDs unterdruecken */
  if(pointerp(str))
  if(member(Query(P_CLASS), str[0]) == -1) 
  {
    return Set(P_CLASS, Query(P_CLASS)+str);
  }
}

void RemoveClass(mixed str)
{
  if(stringp(str)) str = ({ str });
  if(pointerp(str)) return Set(P_CLASS, Query(P_CLASS)-str);
}

void AddAdjective(mixed str)
{
  if(stringp(str)) str = ({ str });
  if(pointerp(str)) return Set(P_ADJECTIVES, Query(P_ADJECTIVES)+str);
}

void RemoveAdjective(mixed str)
{
  if(stringp(str)) str = ({ str });
  if(pointerp(str)) return Set(P_ADJECTIVES, Query(P_ADJECTIVES)-str);
}

/* Ids muessen uebergeben werden, da unit machmal mit plural-Ids, */
/* also anderen als den normalen arbeiten muss. */
int match_item(string str, string *ids)
{
  mixed ads;

  if (!str) return 0;
  if (!pointerp(ids) || !sizeof(ids)) return 0;
  if (member(ids, str)>=0)
    return 1;
  if (ads = QueryProp(P_ADJECTIVES)) {
    string *obj;
    int len;
    int i;
    
    obj = explode(str," ");
    len = sizeof(obj);
    /* find as many adjectives as possible */
    for (i = 0; i < len-1; i++) {
      if (member(ads,obj[i]) < 0) break;
    }
    /* The rest must be in the id list */
    str = implode(obj[i..len-1]," ");
  }
  if (member(ids, str)>=0)
    return 1;
  return 0;
  // return (member(lower_case(str),ids)>=0);
}

varargs string name(int casus, int demon)
{
  string adj;
  mixed sh;
  mixed last;
  int art;

  art=(ME->QueryProp(P_ARTICLE));
 
  if ( casus == RAW )
  {
    if( pointerp(QueryProp(P_NAME)) )
      return QueryProp(P_NAME)[WER];
    return QueryProp(P_NAME);
  }
  if ( QueryProp( P_INVIS ) )
    return ({ "etwas", "von etwas", "etwas", "etwas" })[casus];

  sh = QueryProp( P_NAME );
  if (!sh) return 0;
  if ( pointerp(sh) )
    sh = sh[casus];
  else if (stringp(sh)) 
  {
    last = sh[<1];
    switch (casus) {
      case WEM:
      case WEN:
        if (art && QueryProp(P_GENDER)==MALE && last=='e')
          sh = ({string}) sh + "n";
        break;
      case WESSEN:
        if (!art)
        {
          switch(last)
          {
            case 'x':
            case 's':
            case 'z': sh= sh + "'";
                      break;
            default:  sh=sh + "s";
          }
        }
        else 
        {
          switch( QueryProp(P_GENDER) )
          {
            case MALE:
              switch (last)
              {
                case 'x':
                case 's':
                case 'z': break;
                case 'e': sh = sh + "n";
                          break;
                default:  sh = sh + "s";
              }
              break;
            case NEUTER:
              switch (last)
              {
                case 'x':
                case 's':
                case 'z': break;
                default:  sh = sh + "s";
              }
              break;
            case FEMALE:
              break;
          } /* switch( QueryProp(P_GENDER) ) */
        } /* if( QueryAFlag()==0 ) else */
    } /* switch( casus ) */
  } /* pointerp(sh) */

  if (demon == RAW)
    return sh;
  if (!art)
    demon=0;
  else if (demon==2)
    demon=SuggestArticle(QueryProp(P_NAME));

  if (pointerp(last=QueryProp(P_NAME_ADJ)))
    adj = implode(map(last, #'DeclAdj, casus, demon), ""); //'));
  if(!stringp(adj)) adj = "";
  return QueryArticle(casus,demon) + adj + sh;
}

varargs string Name(int casus, int demon) {
  return capitalize(name(casus,demon));
}

// Detail handling

string brstr(string str)
{
  if (stringp(str) && strlen(str)>0)
    if (str[0]!='\t')
      return break_string (str, 78, 0, 1);
    else
      return str[1..];
  else
    return str;
}

void AddDetail(mixed *keys, mixed descr)
{
  int i;
  mapping details;

  details = Query(P_DETAILS);

  // autom. break_string()
  if (stringp(descr))
    descr=brstr(descr);
  else
    if (mappingp(descr))
    {
      string *k;
      k=m_indices(descr);
      for (i=sizeof(k)-1; i>=0; i--)
        descr[k[i]]=brstr(descr[k[i]]);
    }

  if (!pointerp(keys))
    details += ([ lower_case((string)keys) : descr ]);
  else
    for (i = sizeof(keys) - 1; i >= 0; i--)
      details += ([ lower_case(({string})keys[i]) : descr ]);
  SetProp(P_DETAILS, details);
}

void RemoveDetail(mixed *keys)
{
  int i;
  mapping details;
  details = Query(P_DETAILS);
 
  if (!keys) details = ([ ]);
  else if (stringp(keys))
    details = m_delete(details, keys);
  else
    details -= mkmapping(keys);
  SetProp(P_DETAILS, details);
}

void AddSpecialDetail(mixed keys, string func)
{
  int i;
  closure cl;
 
  if(!(cl=symbol_function(func,this_object()))) return;
  return AddDetail(keys,cl);
}
 
void RemoveSpecialDetail(mixed keys) { return RemoveDetail(keys); }

void AddReadDetail(mixed keys, mixed descr)
{
  int i;
  mapping details;
  string *ind;

  details = QueryProp(P_READ_DETAILS);

  // autom. break_string()
  if (stringp(descr))
    descr=brstr(descr);
  // fuer den Fall dass es mal rassenspez. Read-Details gibt.
  else
    /*
    if (mappingp(descr))
    {
      string *k;
      k=m_indices(descr);
      for (i=sizeof(k)-1; i>=0; i--)
        descr[k[i]]=brstr(descr[k[i]]);
    }
    */
    if (mappingp(descr))
    {
      string *k;
      k=m_indices(descr);
      for (i=sizeof(k)-1; i>=0; i--)
        descr[k[i]]=brstr(descr[k[i]]);
    }
    
    
  if (stringp(keys))
    details += ([ keys : descr ]);
  else
    for (i = sizeof(keys) - 1; i >= 0; i--)
      details += ([ keys[i] : descr ]);
  SetProp(P_READ_DETAILS, details);
}
 
void RemoveReadDetail(mixed keys)
{
  int i;
  mapping details;
  details = Query(P_READ_DETAILS);
 
  if (!keys) details = ([ ]);
  else if (stringp(keys))
    details = m_delete(details, keys);
  else
    details -= mkmapping(keys);
  SetProp(P_READ_DETAILS, details);
}

varargs string GetDetail(mixed key, mixed race)
{
  mixed detail;
 
  if (stringp(race)) race=lower_case(race);
  detail = Query(P_DETAILS)[key];
  if (closurep(detail)) detail = funcall(detail,key);
  if (mappingp(detail)) detail = detail[race] || detail[0];
  if (closurep(detail)) detail = funcall(detail,key);

  if (detail && explore && member(explore,key) >= 0)
    EPMASTER->GiveExplorationPoint(key, EP_DETAIL);

  return detail;
}

int lies(string str)
{
  string detail;
 
  _notify_fail("Was möchtest Du lesen?\n");
  if (!str) return 0;
 
  if (this_player()->CannotSee()) return 1;

  if (id(str) && QueryProp(P_READ_MSG)) 
    return (write(process_string(QueryProp(P_READ_MSG))), 1);
  if (detail = QueryProp(P_READ_DETAILS)[str])
  {
    write(process_string(detail));
    if (explore && member(explore,str) >= 0)
      EPMASTER->GiveExplorationPoint(str, EP_DETAIL);
    return 1;
  }
  return 0;
}

int read(string str) { return lies(str); }

mapping _set_details(mapping arg)
{
  if(mappingp(arg))
    return Set(P_DETAILS, arg);
  return 0;
}

private int _closures(mixed x, mapping details, int yes) 
{ 
  return yes ? closurep(details[x]) : !closurep(details[x]); 
}

mapping _query_details()
{
  return filter_indices(Query(P_DETAILS),
                        #'_closures, Query(P_DETAILS), 0); //');
}

mapping _query_special_details()
{
  return filter_indices(Query(P_DETAILS),
                        #'_closures, Query(P_DETAILS), 1); //');
}
 
mapping _set_special_details(mapping arg)
{
  if (mappingp(arg))
    return Set(P_SPECIAL_DETAILS, arg);
  return 0;
}
 
mapping _set_read_details(mapping arg)
{
  if (mappingp(arg))
    return Set(P_READ_DETAILS, arg);
  return 0;
}


// Sound/Smell Handling

varargs string GetSmell(mixed race)
{
  mixed smell;

  if (stringp(race)) race=lower_case(race);
  smell = QueryProp(P_SMELL);
  if (closurep(smell)) return funcall(smell);
  if (mappingp(smell)) smell = smell[race] || smell[0];
  if (closurep(smell)) return funcall(smell);
  return smell;
}

varargs string GetSound(mixed race)
{
  mixed sound;

  if (stringp(race)) race=lower_case(race);
  sound = QueryProp(P_SOUND);
  if (closurep(sound)) return funcall(sound);
  if (mappingp(sound)) sound = sound[race] || sound[0];
  if (closurep(sound)) return funcall(sound);
  return sound;
}


//  **** local property methods

static string _set_long(string str)
{
  return Set (P_LONG, brstr(str));
}

static string _set_int_long(string str)
{
  return Set (P_INT_LONG, brstr(str));
}

static mixed _set_name_adj(mixed adjectives)
{
  if(!pointerp(adjectives)) adjectives = ({ adjectives });
  return Set(P_NAME_ADJ, adjectives);
}

static mixed _set_adjectives(mixed adjectives)
{
  Set(P_ADJECTIVES, ({}));
  return AddAdjective(adjectives);
}

static mixed _set_ids(mixed ids)
{
  Set(P_IDS, ({}));
  return AddId(ids);
}

static <float|int> _set_light(<float|int> light) 
{ 
  //set_light(light - Query(P_LIGHT));
  return Set(P_LIGHT, light);
}

static mixed _set_class(mixed classes)
{
  Set(P_CLASS, ({}));
  return AddClass(classes);
}

<float|int> _query_total_light() { return set_light(0); }

// *****
varargs string long()
{
  mixed long;
  
  return "\n"+process_string(QueryProp(P_LONG));
}

string short()
{
  string sh;

	if ( QueryProp(P_INVIS))
		return 0;
  if( sh = QueryProp( P_SHORT ) )
    return process_string( sh ) + ".\n";
  return 0;
}

mixed QueryName() {
  return QueryProp( P_NAME );
}


int is_class_member(mixed str) {
  // Soll benutzt werden, um festzustellen, ob etwas zu einer
  // bestimmten Klasse gehoert, wie "Untote" oder "Sprengstoffe" ...
  mixed cl, ids;
  int i;

  if (!str)
    return 0;
  if (stringp(str))
    str = ({ str });
  if ((cl = QueryProp(P_RACE)) && member(str, lower_case(cl)) > -1)
    return 1;
  if (!pointerp(cl=QueryProp(P_CLASS)))
    cl = ({});
  if (pointerp(ids=QueryProp(P_IDS)))
    cl += ids;

  for (i=sizeof(str)-1; i>=0; i--)
    if (member(cl, str[i]) >= 0)
      return 1;

  return 0;
}

void AddToClass(string str) {
  log_file("CLASS", sprintf("AddToClass() von %O\n", previous_object()));
  if (str)
    AddClass(str);
}
