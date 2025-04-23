// SilberLand MUDlib
//
// std/thing/language.c -- language handling object
//
// $Date: 2001/02/10 12:06:58 $                      
/* $Revision: 3.1 $       
* $Log: language.c,v $
* Revision 3.1  2001/02/10 12:06:58  Samtpfote
* prepared for revision control
*
* Revision 3.1  1997/03/30 18:46:23  Woody
* ME-> raus, in SuggestArticle() auf id!=0 pruefen
*
* Revision 3.0  1997/03/13 02:26:31  Woody
* prepared for revision control
*
* Revision 2.12  1995/03/30  13:48:07  Jof
* ???
*
* Revision 2.11  1994/10/04  09:24:02  Jof
* *** empty log message ***
*
* Revision 2.11  1994/10/03  21:46:25  mud
* Optimizations
*
* Revision 2.10  1994/09/22  16:25:56  Hate
* added P_GENDER, removed from /std/living/description.c
*
* Revision 2.9  1994/09/22  09:42:41  Hate
* some minor style fixes
*
* Revision 2.8  1994/07/10  17:02:44  Jof
* made _set... static
*
* Revision 1.3  1994/01/04  10:46:50  mud
* NEED_PROTOTYPES added
*/      
                      
#pragma strong_types
#define NEED_PROTOTYPES

#include "/sys/thing/properties.h"
#include <properties.h>
#include <language.h>
#include <defines.h>
// #include <std_headers.h>

int QueryAFlag() { return !Query(P_ARTICLE); }

static int _set_article(int fl) { return !Set(P_ARTICLE, !fl); }

static int _query_article() { return !Query(P_ARTICLE); }

private string query_c_article(int casus)
{
  return ({ ({ "das ", "des ", "dem ", "das " }),
            ({ "der ", "des ", "dem ", "den " }),
            ({ "die ", "der ", "der ", "die " }) })
         [({int})QueryProp( P_GENDER ) % 3][casus];
}

private varargs string query_g_suffix(int gen, int casus, int anzahl)
{
  return ({ ({ ({"","e"}), ({"es","er"}), ({"em","en"}), ({"","e"}) }),
      ({ ({"","e"}), ({"es","er"}), ({"em","en"}), ({"en","e"}) }),
      ({ ({"e","e"}), ({"er","er"}), ({"er","en"}), ({"e","e"}) }) })
    [gen][casus][anzahl];
}

private string query_ic_article(int casus)
{
  return "ein"+query_g_suffix( ({int})QueryProp( P_GENDER ) % 3, casus )+" ";
}

varargs string QueryArticle(int casus, int dem, int force)
{
  if(!force && !(QueryProp(P_ARTICLE))) return "";
  if (dem == 2) dem = SuggestArticle(QueryProp(P_NAME));
  if (dem) return query_c_article(casus);
  return query_ic_article(casus);
}

varargs int SuggestArticle(string id)
{
  object ob;
  if(!environment()) return 1;
  for(ob = first_inventory(environment()); ob; ob = next_inventory(ob))
    if(ob != this_object() && id && id == ob->QueryProp(P_NAME)) return 0;
  return 1;
}

varargs string QueryPossPronoun(mixed what, int casus, int number)
{
  int gen2;
  gen2 = intp(what) ? what : ({int})what->QueryProp(P_GENDER);
  return (((({int})QueryProp( P_GENDER ) % 3) < FEMALE ) ? "sein" : "ihr")
    + query_g_suffix(gen2, casus, number);
}

string QueryPronoun(int casus)
{
  int gender;
  gender = QueryProp(P_GENDER);
  switch(casus)
  {
    case WER:
    switch(gender)
    {
      case MALE  : return "er";
      case FEMALE: return "sie";
      case NEUTER: return "es";
    }
    break;
    case WEM: if(gender == FEMALE) return "ihr"; else return "ihm";
  }
  switch(gender)
  {
    case MALE: return "ihn";
    case FEMALE: return "sie";
  }
  return "es";
}

varargs string QueryDu(int casus,int gender,int zahl)
{
  return
    ({ ({ ({ "du", "ihr"}), ({ "du", "ihr"}), ({ "du", "ihr"}) }),
       ({({"deines","deiner"}),({"deines","deiner"}),({"deiner","deiner"})}),
       ({({"dir","euch"}),({"dir","euch"}),({"dir","euch"})}),
       ({({"dich","euch"}),({"dich","euch"}),({"dich","euch"})})
       })[casus][gender][zahl];
}

string QueryGenderString()
{
  switch( ({int})QueryProp( P_GENDER ) % 3 )
  {
    case MALE  : return "männlich";
    case FEMALE: return "weiblich";
    case NEUTER: return "sächlich";
  }
  return "";
}

varargs string DeclAdj(string adj, int casus, int demon)
{
  if (!adj) return "";
  
  if(demon)
    return adj + ({ ({ "e" , "en", "en", "e"  }),
        ({ "e" , "en", "en", "en" }),
        ({ "e" , "en", "en", "e"  }) })
      [({int})QueryProp( P_GENDER ) % 3][casus] + " ";
  else
    return adj + ({ ({ "es", "en", "en", "es" }),
        ({ "er", "en", "en", "en" }),
        ({ "e" , "en", "en", "e"  }) })
      [({int})QueryProp( P_GENDER ) % 3][casus] + " ";
  return "";
}

static int _set_gender(int i)
{
  if(i==MALE || i==FEMALE || i==NEUTER) return Set(P_GENDER, i);
  return 0;
}
 
