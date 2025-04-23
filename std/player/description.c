// Tamedhon MUDlib
//
// std/player/description.c -- player description handling
//
// $Date: 2003/04/24 18:30:00 $
/* $Revision: 3.4 $
 * $Log: description.c,v $
 *
 * Revision 3.5  2008/01/13 16:00:00  TamTam
 * Augen-/Haar-/Hautfarbe+Racespecial anzeigen in long()
 * P_EYE_COLOR, P_HAIR_COLOR, P_SKIN_COLOR, P_HAIR_STYLE, P_RACE_SPECIAL
 *
 * Revision 3.4  2003/04/24 18:30:00  Querolin
 * extralook umgebrochen mit break_string (in long())
 *
 * Revision 3.3  2002/04/20 00:00:00  Querolin
 * Diverse Aenderungen von Gralkor aktiv gestellt
 *
 * Revision 3.2  1997/11/08 12:16:41  Woody
 * (rastend) an die Kurzbeschreibung anhaengen, wenn der Spieler rastet
 *
 * Revision 3.1  1997/06/25 12:03:17  Woody
 * ME-> bei QueryArticle() raus
 *
 * Revision 3.0  1997/03/13 01:37:01  Woody
 * prepared for revision control
 *
 * Revision 1.1  96/05/25  18:00:00  Woody
 * short() Aenderungen wieder retour
 *
 * Revision 1.0  22/02/96  23:00:00  Hadra
 * short() geaendert
 */

#pragma strong_types

inherit "/std/living/description";

#define NEED_PROTOTYPES

#include <thing/language.h>
#include <player/description.h>
#include <properties.h>
#include <wizlevels.h>
#include <defines.h>
#include <moving.h>

#include "/std/sys_debug.h"

void create()
{
  ::create();
  Set(P_NAME, -1, F_SET_METHOD);
  Set(P_PRESAY, SAVE, F_MODE_AS);
  Set(P_TITLE, SAVE, F_MODE_AS);
  Set(P_EXTRA_LOOK, SAVE, F_MODE_AS);
  Set(P_GENDER, SAVE, F_MODE_AS);
  AddId("Spieler");
}

int _query_size() {
  if (QueryProp(P_FROG))
    return 20;
  return Query(P_SIZE);
}

// short() -- get the short description of a player
string short()
{
  string answer;
  string title;

  if(QueryProp(P_INVIS))
    if(interactive(previous_object()) && IS_LEARNING(previous_object()))
      return "("+QueryProp(P_NAME)+")\n";
    else
      return 0;

  if(QueryProp(P_GHOST))
  {
    if (QueryProp(P_FROG))
      return "Der Geist eines kleinen Frosches namens "+QueryProp(P_NAME)+
	     ".\n";
    else
      return "Der Geist von "+QueryProp(P_NAME)+".\n";
  }

  if (QueryProp(P_FROG))
    return "Ein kleiner Frosch namens "+QueryProp(P_NAME)+".\n";

  answer = QueryProp(P_PRESAY) + QueryProp(P_NAME);
  if (QueryProp(P_ARTICLE)) answer=QueryArticle(0,0)+answer;
  if((title=QueryProp(P_TITLE)) && title != "") answer += " " + title;
  if(!interactive(ME)) answer += " (netztot)";
  else if(QueryProp(P_RESTING)) answer += " (rastend)";
  return answer+".\n";
}

private void select(object o, mixed weapon, mixed armour, mixed rest)
{
  if (!o->short()) return;  // unsichtbare Objekte nicht anzeigen (Rochus@MG)
  if (member(QueryProp(P_ARMOURS), o) != -1) armour += ({ o });
  else if (QueryProp(P_WEAPON) == o) weapon += ({ o });
  else if (sizeof(all_inventory(o)) || o->QueryProp(P_LIGHT) ||
           o->QueryProp(P_LIGHTED) || o->QueryProp(P_SHOW_INV))
    rest += ({ o });
}

private string andlist(object *arr)
{
  string *tmp;
  if(!sizeof(arr)) return "";
  if(sizeof(tmp = map_objects(arr, "name", WEN))>1)
    return implode(tmp[0..<2], ", ")+(sizeof(tmp)>1?" und "+tmp[<1]:"");
  else return tmp[0];
}

varargs string long()
{
  string exl, descr, specialtext, tmp;
  string cap_pronoun;
  object ob, *w, *a, *r;
  int hpnt, max_hpnt;
  int orkview, orkme;
  mixed trans;

  string size_descr;
  string comparesize, pers1, pers2, fill, fill2;
  int relation, per;


  descr = "Das ist "+name(WER,1)+".\n";
  if(QueryProp(P_GHOST)) return descr;

  trans = QueryProp(P_TRANSPARENT); SetProp(P_TRANSPARENT, 0);
  descr += ::long(); SetProp(P_TRANSPARENT, trans);

  /* fuer groessenvergleich */
  descr = "";

  // Sicherheitsfix, absichtlich auf EINS, damits debuggt wird!
  if(!QueryProp(P_AVERAGE_SIZE))
  {
      SetProp(P_AVERAGE_SIZE, 1);
  }

  per=1000*QueryProp(P_SIZE)/QueryProp(P_AVERAGE_SIZE);
  switch(per)
  {
    case 0..800: size_descr="ziemlich winzig"; break;
    case 801..850: size_descr="eher winzig"; break;
    case 851..900: size_descr="recht klein"; break;
    case 901..950: size_descr="eher klein"; break;
    case 951..1050: size_descr="mittelgroß"; break;
    case 1051..1100: size_descr="relativ groß"; break;
    case 1101..1150: size_descr="ziemlich groß"; break;
    case 1151..1200: size_descr="sehr groß"; break;
    default: size_descr="riesig"; break;
  }
#define RassenString() ((QueryProp(P_FROG) ? "en Frosch" :\
       (!QueryProp(P_GENDER)?" ":QueryProp(P_GENDER)==2?"e ":"en ")+\
       (pointerp(QueryProp(P_RACESTRING))?\
        QueryProp(P_RACESTRING)[WEN]:QueryProp(P_RACE))))
  fill2=fill=0;
  if (QueryProp(P_AVERAGE_SIZE)<170)
  {
    if (per<950)
      fill="selbst für ein"+RassenString()+" ";
    else
      if (per>1050)
  fill2=", wenn auch nur für ein"+RassenString();
  }
  else
  {
    if (QueryProp(P_AVERAGE_SIZE)>170)
      if (per>1050)
  fill="sogar für ein"+RassenString()+" ";
      else
  if (per<950)
    fill2=", wenn auch nur für ein"+RassenString();
  }
  if (!fill&&!fill2) fill="für ein"+RassenString()+" ";
  descr = "Das ist "+name(WER,1)+". "+capitalize(QueryPronoun())+" ist "+
    (fill||"")+size_descr+(fill2||"")+".\n";

  /* groessenvergleich_anfang (NEU) */
 pers1 = QueryProp(P_GENDER)==2 ? "sie" : "er";
 pers2 = QueryProp(P_GENDER)==2 ? "ihr" : "ihm";

 relation = QueryProp(P_SIZE)*100 / this_player()->QueryProp(P_SIZE);
 switch (relation)
  {
  case   0 ..  25 : comparesize = "Damit gibt "+pers1+" einen guten"
                                  " Fußschemel für Dich ab";
                                  break;
  case  26 ..  50 : comparesize = "Damit reicht "+pers1+" Dir nicht mal bis"
                                  " zur Hüfte";
                                  break;
  case  51 ..  75 : comparesize = "Damit kannst Du "+pers2+" locker auf den"
                                  " Kopf spucken";
                                  break;
  case  76 ..  90 : comparesize = "Damit ist "+pers1+" einen Kopf kleiner"
                                  " als Du";
                                  break;
  case  91 .. 110 : comparesize = "Damit hat "+pers1+" etwa Deine Größe";
                                  break;
  case 111 .. 120 : comparesize = "Damit ist "+pers1+" einen Kopf größer"
                                  " als Du";
                                  break;
  case 121 .. 150 : comparesize = "Damit holst Du Dir einen steifen Nacken,"
                                  " wenn Du "+pers2+" in die Augen siehst";
                                  break;
  case 151 .. 200 : comparesize = "Damit versperrt "+pers1+" Dir absolut"
		                  " die Sicht";
                                  break;
  case 201 .. 300 : comparesize = "Damit wäre jeder Schlag von Dir ein"
                                  " Tiefschlag";
                                  break;
  default         : comparesize = "Damit kannst Du "+pers2+" bequem zwischen"
                                  " den Beinen durchlaufen";
                                  break;
  }

  descr+=comparesize+".";
  /* groessenvergleich_ende (NEU) */

  if(!QueryProp(P_LOOK_INVIS))
  {
  specialtext=" ";
  orkme=(this_object()->QueryProp(P_RACE)=="Ork");
  orkview=(this_player()->QueryProp(P_RACE)=="Ork" &&
           (this_object()->QueryProp(P_RACE)=="Elf" ||
            this_object()->QueryProp(P_RACE)=="Brownie") );
  // Augen-/Haar- und Hautfarbe anzeigen
  if( stringp(QueryProp(P_HAIR_STYLE)) && stringp(QueryProp(P_HAIR_COLOR)) )
    specialtext+=capitalize(QueryPronoun())+" hat "+(orkview?"widerlich ":"")+
      QueryProp(P_HAIR_STYLE)+" von "+(orkview?"ekelhaft ":"")+
      QueryProp(P_HAIR_COLOR)+"er Farbe. ";
  if( stringp(QueryProp(P_EYE_COLOR)) && stringp(QueryProp(P_SKIN_COLOR)) )
    specialtext+=capitalize(QueryPossPronoun(FEMALE, WEN, PLURAL))+" Augen "
      "sind "+(orkview?"abstoßend ":"")+QueryProp(P_EYE_COLOR)+" und "+
      (orkme?QueryPossPronoun(NEUTER, WEN)+" Fell ist ":
             QueryPossPronoun(FEMALE, WEN)+" Haut ist ")+
      (orkview?"furchtbar ":"")+QueryProp(P_SKIN_COLOR)+". ";
  if( stringp(QueryProp(P_RACE_SPECIAL)) )
    specialtext+=capitalize(QueryPronoun())+" "+QueryProp(P_RACE_SPECIAL)+
      (orkview?", wie hässlich!":".");
  descr+=specialtext;
  }
  descr=break_string(descr,78);


  /* extra_looks einsammeln und ggf in die desc einbauen */
  if(stringp(tmp = QueryProp(P_EXTRA_LOOK))) descr += break_string(tmp,78,0,1);
  for(ob = first_inventory(ME); ob; ob = next_inventory(ob))
    if(exl = ob->QueryProp(P_EXTRA_LOOK)) descr += exl;
    else if(exl = ob->extra_look()) descr += exl; // TO BE REMOVED
  w = ({}); a = ({}); r = ({});
  map(all_inventory(ME), #'select, &w, &a, &r);
//  return descr +
  descr+=
         (QueryProp(P_TRANSPARENT) ?
          break_string((this_player()==ME?capitalize(QueryPronoun()):capitalize(name(WER, 1)))
                      +(sizeof(a)?" trägt "+andlist(a):" ist unbekleidet")
//                      +(sizeof(r)?(sizeof(w)?", ":""):(sizeof(r)?" und ":""))
                      +(sizeof(w)?". Außerdem hat "+QueryPronoun(WER)+" "+andlist(w)+" gezückt":"")
		      +(sizeof(r)?". "+capitalize(QueryPronoun(WER))+" besitzt "+andlist(r):"")+".", 78): "");

  cap_pronoun = capitalize(QueryPronoun());
  hpnt = QueryProp( P_HP );
  max_hpnt = QueryProp( P_MAX_HP );
  if (hpnt < max_hpnt/10)
    descr += cap_pronoun + " steht auf der Schwelle des Todes.\n";
  else if (hpnt < max_hpnt/5)
    descr += cap_pronoun + " braucht dringend einen Arzt.\n";
  else if (hpnt < max_hpnt*3/10)
    descr += cap_pronoun + " ist in keiner guten Verfassung.\n";
  else if (hpnt < max_hpnt*4/10)
    descr += cap_pronoun + " wankt bereits bedenklich.\n";
  else if (hpnt < max_hpnt/2)
    descr += cap_pronoun + " macht einen mitgenommenen Eindruck.\n";
  else if (hpnt < max_hpnt*6/10)
    descr += cap_pronoun + " sieht nicht mehr taufrisch aus.\n";
  else if (hpnt < max_hpnt*7/10)
    descr += cap_pronoun + " ist leicht angeschlagen.\n";
  else if (hpnt < max_hpnt*8/10)
    descr += cap_pronoun + " fühlte sich heute schon besser.\n";
  else if (hpnt < max_hpnt*9/10)
    descr += cap_pronoun + " ist schon etwas geschwächt.\n";
  else
    descr += cap_pronoun + " ist absolut fit.\n";

 return descr;
}


// **** local property methods
static mixed _query_presay()
{
  string presay;
  if((presay = Query(P_PRESAY)) && (presay != "")) return presay + " ";
  return "";
}

static string _query_name()
{
  return capitalize(Query(P_NAME));
}
// ****



