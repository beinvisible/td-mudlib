// SilberLand MUDlib
//
// LIVING/PUT_AND_GET.C -- living put and get handling
//
// $Date: 2007/12/30 16:00:00 $
/* $Revision: 1.4 $
 * $Log: put_and_get.c,v $
 *
 * Revision 1.4  2007/12/30 16:00:00  TamTam
 * show_things() erkannte Boerse/Schneckenhaus faelschlicherweise als Mehrzahl
 *
 * Revision 1.3  2007/11/02 06:07:00  Gralkor
 * P_CNT_STATUS Bug entfernt. die Behaelter waren alle weder offen noch zu
 *
 * Revision 1.2  2001/01/15 13:50:11  Samtpfote
 * what->long() wird auch nun mitgezeigt.
 *
 * Revision 1.1  2001/01/15 13:42:30  Samtpfote
 * Initial revision
 *
 * Revision 3.8  1997/11/25 18:14:20  Feyaria
 * select_objects in give_to auf env_first
 *
 * Revision 3.7  1997/11/08 17:35:32  Crea
 * removed bad bug from drop_obj => no usedverb (from NPC's), error in sprintf
 *
 * Revision 3.6  1997/10/16 15:47:03  Woody
 * Temporaeres In-Raum-Moven ausgebaut (MayAddWeight erledigt das jetzt)
 *
 * Revision 3.5  1997/10/14 13:25:51  Woody
 * Typofix
 *
 * Revision 3.4  1997/10/13 15:22:46  Woody
 * Noch ein paar Sicherheitsabfragen bei 'nimm x (aus y)'
 *
 * Revision 3.3  1997/09/17 14:33:56  Woody
 * Differenzierte Fehlermeldungen beim wegwerfen, verwendetes Verb
 * (wegwerfen vs. fallenlassen vs. ablegen) wird bei der Ausgabe beruecksichtigt
 *
 * Revision 3.2  1997/09/09 00:57:43  Woody
 * Das Drama endlich beendet (hoffentlich...), zumindest bei 'nimm...aus'
 *
 * Revision 3.1  1997/09/08 21:37:37  Woody
 * Kleiner Bugfix fuer "nimm alles aus spieler", Generalueberholung folgt :/
 *
 * Revision 3.0  1997/03/13 01:01:05  Woody
 * prepared for revision control
 *
 * 97/01/08  Woody
 * Bugfix in put_into() - Was einmal wo reingesteckt wurde, wird nicht
 * nochmal woanders reingesteckt (etwa in noch ein matching item usw.)
 *
 * 96/11/25  Woody
 * 'zeige' typo bei Adverben raus
 *
 * 96/11/08  Woody
 * Hmm... also wenn man nur einen Teil von Units aus Behaeltern nimmt,
 * dann sollte der Rest nicht im Raum liegenbleiben, sondern wieder in
 * den Beutel rein :) - bugfixed.
 *
 * 96/10/26  Woody
 * Objekte werden beim Nehmen (z.B. aus Beuteln) erst mal ins Environment
 * des Livings gemoved; wg. Gewichtsberechnungen etc. - pick_obj()
 *
 * 96/10/12  Woody
 * 'gib alles/waffen/ruestungen an XY' geht jetzt auch,
 * ebenso wie 'leg ... in beutel'
 *
 * 96/10/07  Woody
 * Beutel duerfen jetzt beim vollstopfen nicht zu schwer werden
 *
 * 96/09/24  Woody
 * Syntax 'gib WEM WAS' ist jetzt moeglich
 *
 * 96/04/07  Woody
 * P_GIVEN_OBJECT (wg. units) in give_obj() beruecksichtigt
 *
 * Revision 2.19  1995/08/07  16:20:30  Jof
 * put only one thing a time
 *
 * Revision 2.18  1995/08/07  15:32:14  Jof
 * Fixed weight bug when putting stuff from inv into container
 *
 * Revision 2.17  1995/07/05  14:44:49  Rumata
 * Abfrage, dass Beutel durch reinstecken zu schwer werden koennten.
 *
 * Revision 2.16  1995/07/05  14:02:26  Rumata
 * kommando zurueck
 *
 * Revision 2.15  1995/07/05  13:49:31  Jof
 * "stecke" nimmt die Sachen zuerst, damit spieler nichts in Beutel
 * stecken koennen, dass sie nicht tragen koennen. (Rumata)
 *
 * Revision 2.14  1995/05/22  18:38:24  Jof
 * Selber zeigen geht nicht mehr
 *
 * Revision 2.13  1995/05/22  15:20:21  Jof
 * Removed extra ! from if ...
 *
 * Revision 2.12  1995/05/22  07:17:33  Jof
 * Zeige eingebaut
 * aufgeraeumt
 *
 * Revision 2.12  1995/05/21  20:20:14  mud
 * "zeige" eingebaut
 *
 * Revision 2.11  1994/11/07  22:09:54  Jof
 * Revision 2.10  1994/08/23  23:47:53  Kirk
 * recognize P_NOGET when moving objects into containers
 *
 * Revision 2.9  1994/06/13  06:59:21  Jof
 * Revision 2.9  1994/06/13  06:59:21  Jof
 * Revision 2.8  1994/05/15  18:11:04  Jof
 * Revision 2.7  1994/05/15  16:57:36  Jof
 * Revision 2.6  1994/04/28  09:27:32  Rumata
 * geben funktionert auch mit adjektiven
 *
 * Revision 2.5  1994/03/21  16:48:13  Jof
 * Revision 2.4  1994/03/18  12:35:53  Jof
 * Revision 2.3  1994/03/03  18:30:45  Jof
 * capitalize in ME_TOO_HEAVY error
 *
 * Revision 2.2  1994/03/03  17:31:25  Jof
 * Revision 2.1  1994/01/17  22:25:28  mud
 * Revision 2.1  1994/01/17  22:25:28  mud
 * Revision 2.0  1994/01/13  16:52:09  mud
 * Revision 1.2  1994/01/08  13:11:20  mud
 * Revision 1.2  1994/01/08  13:11:20  mud
 *
 */

#pragma strong_types
#pragma save_types

#define NEED_PROTOTYPES

// #define DEBUG
#ifdef DEBUG
#define TD(x) if (find_player("debugger")) tell_object(find_player("debugger"), x)
#else
#define TD(x)
#endif

#include <thing/properties.h>
#include <container.h>
#include <language.h>
#include <properties.h>
#include <moving.h>
#include <defines.h>
#include <thing/description.h>

// Verbhandling
// evtl. nicht 100% sauber, aber sehr pragmatisch
// legen, weg-werfen, fallen-lassen, in-etwas-stecken

static string usedverb;
#define VERBLIST \
   ([ "leg":   "legen";   "legst";   "legt";   "ab", \
      "wirf":  "werfen";  "wirfst";  "wirft";  "weg", \
      "lass":  "lassen";  "laesst";   "lässt"; "fallen", \
      "steck": "stecken"; "steckst"; "steckt"; "" ])

int pick_obj(object ob);

varargs object *select_objects(string str, mixed info, int inv_first)
{
  object *found_obs;

  if (inv_first)
    found_obs=locate_objects(str,info);
  else
    found_obs=({});
  if (environment(this_object()) && !sizeof(found_obs))
    found_obs += (environment(this_object())->locate_objects(str,info))||({});
  if (!inv_first && !sizeof(found_obs))
    found_obs += locate_objects(str,info);
  return found_obs;
}

void add_put_and_get_commands()
{
  add_action("pick_things","nimm");
  add_action("drop_things","werf");
  add_action("drop_things","werfe");
  add_action("drop_things","wirf");
  add_action("drop_things","lass");
  add_action("drop_things","lasse");
  add_action("drop_things","leg");  /* auch fuer put ! */
  add_action("drop_things","lege"); /* auch fuer put ! */
  add_action("put_things", "steck");
  add_action("put_things", "stecke");
  add_action("give_things","gebe");
  add_action("give_things","gib");
  add_action("show_things","zeig");
  add_action("show_things","zeige");
}

int pick_things(string str)
{
  object *found_obs;
  string *complex_list;
  int i, oflag;

  if (QueryProp(P_GHOST))
  {
    notify_fail("Als Geist kannst Du nichts nehmen.\n");
    return 0;
  }

  if (!str)
  {
    notify_fail("WAS nehmen ?\n");
    return 0;
  }

  oflag=0;

  complex_list = explode(str," ");
  if ((i = member(complex_list,"aus")) >0) complex_list[i] = "in";
  if ((i = member(complex_list,"von")) >0) oflag=1,complex_list[i] = "in";

  str=implode(complex_list," ");

  if (!sizeof(found_obs=select_objects(str,1)))
  {
    if (environment() && environment()->id(str))
      notify_fail("Das kannst Du nicht mitnehmen.\n");
    else
      notify_fail("Das geht nicht!\n");
    return 0;
  }

  notify_fail("Du findest hier nichts Nehmenswertes.\n");
  found_obs=filter(found_obs, #'pick_obj); //');
  return (pointerp(found_obs) && sizeof(found_obs));
}

int pick_obj(object ob)
{
  int ret, oflag, is_unit;
  string str;
  object env;

  if (!objectp(ob))
    return 0;
  if (ob == this_object())
    return notify_fail ("Du kannst Dich nicht selbst nehmen.\n"), 0;
  if ((env=environment(ob)) == this_object())
    return notify_fail ("Das hast Du schon!\n"), 0;
  if (query_once_interactive(env) && env->QueryProp(P_INVIS))
    return 0;   // unbemerkt raus, damit keiner invis Leute beklaut :)
  if (env != environment())
  {
    if (!env->QueryProp(P_CONTAINER) && !env->QueryProp(P_TRAY))
    {
      notify_fail("Du kannst nichts "+(oflag?"von":"aus")+
		  " "+env->name(WEM,1)+" nehmen.\n");
      return 0;
    }
    else
      if (env->QueryProp(P_CNT_STATUS))  // Container ist geschlossen (removed!)
      {
	notify_fail(sprintf("Aber %s ist doch geschlossen.\n",
			    env->name(WER, 1)));
	return 0;
      }
  }
  is_unit=ob->IsUnit();
  ret = ob->move(this_object(),M_GET);
  if (ret>0 && ((ob && environment(ob)==this_object()) || is_unit))
  {
    tell_object(this_object(),"Du nimmst "+(str=ob->name(WEN,0))+".\n");
    say(ME->Name(WER)+" nimmt "+str+".\n");
    return 1;
  }
  if (ret==ME_TOO_HEAVY)
  {
    tell_object(this_object(),
        "Du kannst "+ob->name(WEN,1)+" nicht mehr tragen.\n");
    return 1;
  }
  if (ret==ME_CANT_BE_TAKEN)
  {
    if ((str=ob->QueryProp(P_NOGET)) && stringp(str))
    {
      tell_object(this_object(),str);
      return 1;
    }
  }
  if (ob)
    tell_object(this_object(),"Du kannst "+ob->name(WEN,1)+" nicht nehmen.\n");
  else
    tell_object(this_object(),"Das kannst Du nicht nehmen.\n");
  return 1;
}

int main_drop(string str)
{
  object *found_obs;

  if (!str) {
    notify_fail("WAS willst Du "+
        VERBLIST[usedverb,3]+VERBLIST[usedverb,0]+"?\n");
    return 0;
  }

  if (!sizeof(found_obs=select_objects(str,1,1))) {
    notify_fail("Du hast nichts Derartiges!\n");
    return 0;
  }

  notify_fail("Sowas siehst Du hier nicht!\n");
  found_obs=filter(found_obs,"drop_obj",this_object());
  return (pointerp(found_obs) && sizeof(found_obs));
}

int put_obj(object ob, object where)
{
  int ret;
  object env;
  string str;

  if (ob == this_object() || ob == where ||  environment(ob) == where)
    return 0;
  if (first_inventory(ob))
  {
    notify_fail(ob->Name(WER,1)+" ist nicht leer !\n");
    return 0;
  }
  env=environment(ob);
  if (!where->QueryProp(P_CONTAINER))
  {
    notify_fail("Du kannst in "+where->name(WEN,0)+" nix rein"+
        VERBLIST[usedverb,0]+".\n");
    return 0;
  }
  if (where->QueryProp(P_CNT_STATUS))  // Container ist geschlossen removed !
  {
    notify_fail("Aber "+where->name(WER, 1)+" ist doch geschlossen.\n");
    return 0;
  }
  if (env!=environment(this_object()) && env!=this_object())
  {
    notify_fail("Da kommst du so nicht ran.\n");
    return 0;
  }
  if (str=ob->QueryProp(P_NOGET))
  {
    if (stringp(str))
      notify_fail(str);
    else
      notify_fail("Du kannst das so nirgendwo rein"+
          VERBLIST[usedverb,0]+".\n");
    return 0;
  }
  if( environment(where)==this_object() && environment(ob)!=this_object() &&
      -2 == where->MayAddWeight(ob->QueryProp(P_WEIGHT)) )
  {
    notify_fail(where->Name(WER,1)+" würde Dir dann zu schwer werden.\n");
    return 0;
  }

  ret=ob->move(where,M_PUT);
  if (ret>0)
  {
    tell_object(this_object(),"Du "+VERBLIST[usedverb,1]+
        " "+ob->name(WEN,0)+" in "+where->name(WEN,1)+".\n");
    say(ME->Name(WER)+" "+VERBLIST[usedverb,2]+" "+ob->name(WEN,0)+
        " in "+where->name(WEN,0)+".\n");
    return 1;
  }
  if (ret==ME_CANT_BE_DROPPED)
  {
    if (stringp((str=ob->QueryProp(P_NODROP))))
      notify_fail(str);
    else
      notify_fail("So wirst Du "+ob->name(WEN,1)+" nicht los ...\n");
    return 0;
  }
  if (ret==ME_TOO_HEAVY)
  {
    notify_fail("Das passt nicht mehr rein.\n");
    return 0;
  }
  if (ret==ME_CANT_BE_INSERTED)
  {
    notify_fail("Das kannst Du da nicht rein"+VERBLIST[usedverb,0]+".\n");
    return 0;
  }
  notify_fail("Das geht nicht.\n");
  return 0;
}

int put_into(object *src_obs,string* dest_list)
{
  object *src_obs1, *dest_obs;
  int i,j,res;

  if ( !(dest_obs = select_objects(implode(dest_list," "),1)) ||
      !sizeof(dest_obs))
  {
    tell_object(this_object(),
                "Nichts zum Hineinstecken oder drauflegen gefunden.\n");
    return 1;
  }

  src_obs1=src_obs;
  res=0;
  notify_fail("Das geht leider nicht.\n");
  for (i=0; i<sizeof(dest_obs); i++) // Reihenfolge in DIESEM Fall wichtig
    if (dest_obs[i] != this_object())
      for (j=sizeof(src_obs1)-1;j>=0;j--)
	if (put_obj(src_obs1[j],dest_obs[i]))
	{
	  src_obs1 -= ({ src_obs1[j] });
	  res = 1;
	}
  return res;
}

varargs int put_things(string str, int flag)
{
  object *src_obs;
  string *src_list, *dest_list;
  string *complex_list;
  int i, j;
  string src;

  if (!flag) usedverb="steck";
  if (!str)
  {
    notify_fail("WAS soll ich WOHIN "+VERBLIST[usedverb,0]+"?\n");
    return 0;
  }
  complex_list = explode(str," ");

  if ((i=member(complex_list,"in"))<0 || i==sizeof(complex_list)-1)
  {
    notify_fail("WO soll ich das hinein"+VERBLIST[usedverb,0]+"?\n");
    return 0;
  }

  if (!i)
  {
    notify_fail("WAS willst du dort hinein"+VERBLIST[usedverb,0]+"?\n");
    return 0;
  }

  src_list = complex_list[0..i-1];
  dest_list = complex_list[i+1..sizeof(complex_list)-1];
  if ((j = member(src_list,"auf")) >0)
    src_list[j] = "in";

  src_obs = select_objects(implode(src_list," "),1,1);
  if (!src_obs || !sizeof(src_obs))
  {
    notify_fail("Sowas siehst Du hier nicht!\n");
    return 0;
  }
  return put_into(src_obs,dest_list);
}

int drop_things(string str)
{
  string arg,dummy;

  if (!str) return 0;
  usedverb = query_verb();
  if (usedverb[<1]=='e') usedverb=usedverb[0..<2];
  if (usedverb=="werf") usedverb="wirf";
  if (QueryProp(P_GHOST))
  {
    notify_fail("Als Geist kannst Du nichts "+
        VERBLIST[usedverb,3]+VERBLIST[usedverb,0]+".\n");
    return 0;
  }
  if (usedverb=="leg")
  {
    if ( sscanf(str,"%s in %s",arg,dummy)==2 ) return put_things(str,1);
    if ( sscanf(str,"%s ab", arg)==1 ) return main_drop( arg );
    if ( sscanf(str,"%s weg", arg)==1 ) return main_drop( arg );
    notify_fail("Lege etwas AB, oder was meinst Du ?\n");
    return 0;
  }
  if (usedverb=="lass")
  {
    if ( sscanf(str,"%s fallen",arg)==1 )
      return main_drop( arg );
    notify_fail("Lass etwas FALLEN, oder was meinst Du ?\n");
    return 0;
  }
  if (usedverb=="wirf")
  {
    if ( sscanf(str,"%s weg",arg)==1 )
      return main_drop( arg );
    notify_fail("Wirf etwas WEG, oder was meinst Du ?\n");
    return 0;
  }
  return 0;
}

int drop_obj(object ob)
{
  int ret;
  string str;

  if (!objectp(ob))
    return 0;
  if (!usedverb || usedverb=="")
    usedverb="wirf";
  if (ob == this_object())
    return notify_fail ("Du kannst Dich nicht selbst "+
        VERBLIST[usedverb,3]+VERBLIST[usedverb,0]+".\n"), 0;
  if (environment(ob) != this_object())
    return notify_fail ("Das trägst Du doch gar nicht bei Dir!\n"), 0;
  ret=ob->move(environment(this_object()),M_PUT);
  if (ret>0) {
    tell_object(this_object(), sprintf("Du %s %s %s.\n",
        VERBLIST[usedverb,1], (str=ob->name(WEN,1)), VERBLIST[usedverb,3]));
    say(sprintf("%s %s %s %s.\n",
        ME->Name(WER), VERBLIST[usedverb,2], str, VERBLIST[usedverb,3]));
    return ret;
  }
  if (ret==ME_CANT_BE_DROPPED)
  {
    if ((str=ob->QueryProp(P_NODROP)) && stringp(str))
    {
      tell_object(this_object(),str);
      return 1;
    }
    tell_object(this_object(),"Du kannst "+ob->name(WEN,1)+
        " nicht "+VERBLIST[usedverb,3]+VERBLIST[usedverb,0]+"!\n");
  }
  return 1;
}

int give_obj(object ob, object where)
{
  int ret;
  string str;
  object env;
  int is_unit;

  if (environment(ob)!=this_object())
  {
    // tell_object(this_object(),"Das solltest Du erstmal nehmen.\n");
    // return 1;
    notify_fail("Das solltest Du erstmal nehmen.\n");
    return 0;
  }
  if (!ob || ob == this_object() || ob == where ||
      environment(where)!=environment(this_object()))
    return 0;
  if (environment(ob) == where) {
    notify_fail("Das Ziel ist in dem zu gebenden Objekt enthalten!\n");
    return 0;
  }
  if (environment(ob)!=this_object())
  {
    notify_fail("Das hast Du nicht.\n");
    return 0;
  }
  is_unit=ob->IsUnit();
  ret=ob->move(where,M_GIVE);
  if (ret==ME_TOO_HEAVY)
  {
    notify_fail(where->Name(WER,1)+" kann das nicht mehr tragen.\n");
    return 0;
  }
  if (ret==ME_CANT_BE_DROPPED ||
      (!is_unit && (!ob || where!=environment(ob))))
  {
    if ((str=ob->QueryProp(P_NODROP)) && stringp(str))
    {
      notify_fail(str);
      return 0;
    }
    notify_fail("Du kannst "+ob->name(WEN,1)+" nicht weggeben!\n");
    return 0;
  }
  if (ret>0)
  {
    tell_object(this_object(),"Du gibst "+where->name(WEM,1)+" "+
		ob->name(WEN,1)+".\n");
    say( sprintf("%s gibt %s %s.\n", ME->Name(WER), where->name(WEM,1),
        ob->name(WEN,1)), where );
    if (living(where))
      tell_object( where, sprintf("%s gibt Dir %s.\n",
          ME->Name(WER,1), ob->name(WEN,1)) );
    if (!query_once_interactive(where) &&
        (!is_unit || objectp(ob = ob->QueryProp(P_GIVEN_OBJECT))))
      where->give_notify(ob);
  }
  return 1;
}

int give_to(object *src_obs,string *dest_list)
{
  int i,j,res;
  object *dest_obs;

  dest_obs = select_objects(implode(dest_list," "),1,0);

  if (dest_obs && sizeof(dest_obs))
  {
    for (i=0;i<sizeof(dest_obs);i++)
      if (!living(dest_obs[i])) dest_obs[i]=0;
    dest_obs-=({0});
  }
  if (!dest_obs || !sizeof(dest_obs)) {
    notify_fail("Keinen Empfänger gefunden.\n");
    return 0;
  }

  notify_fail("Das geht leider nicht.\n");
  res=0;
  for (i=0; i<sizeof(dest_obs); i++)
    if (dest_obs[i] != this_object())
      for (j=0;j<sizeof(src_obs);j++)
	res=give_obj(src_obs[j],dest_obs[i]) || res;
  return res;
}

int show_things(string str)
{
  mixed *obs;
  string src, dest, src2;
  object what, whom;

  if (!str||sizeof(obs=explode(str," "))<2)
  {
    notify_fail("Syntax: zeige <wem> <was>\n");
    return 0;
  }
  dest=obs[0];
  src=implode(obs[1..]," ");
  if (!(what=present(src,this_object())))
  {
    notify_fail("Du kannst nur Dinge zeigen, die Du auch dabei hast!\n");
    return 0;
  }
  if (!(whom=present(dest,environment(this_object()))) && dest!="allen")
  {
    notify_fail("Wem willst Du "+what->name(WEN,1)+" zeigen?\n");
    return 0;
  }
  if (whom==ME)
  {
    write("Dazu solltest Du dann besser 'schau' benutzen!\n");
	return 1;
  }
  src=what->name(WEN,0);	// 1->0; woody
  if (what->QueryProp(P_ARTICLE) && member(src,' ')>=0)
  {
    int x;

    while (src[x]!=' ') x++;
    src2=src[x..];
    src=QueryPossPronoun(what,WEN,what->IsPlural())+src2;
    if (what->IsPlural() || what->QueryProp(P_GENDER)>1)
      src2="Deine"+src2;
    else
	  if (what->QueryProp(P_GENDER)==1)
        src2="Deinen"+src2;
	  else
	    src2="Dein"+src2;
  } else src2=src;
  str=ME->Name(WER)+" zeigt Dir "+src+".\n";
  str+=what->long();
  if (dest=="allen")
  {
    say(str,({this_object()}));
    write("Du zeigst allen "+what->name(WEN,1)+".\n");
    return 1;
  }
  tell_object(whom,str);
  dest=whom->name(WEM);
  say(ME->Name(WER)+" zeigt "+dest+" "+src+".\n", ({whom,this_player()}));
  write("Du zeigst "+dest+" "+src2+".\n");
  return 1;
}

int give_things(string str)
{
  object *src_obs, *dest_obs;
  string *src_list,*dest_list;
  string *complex_list;
  int i, j;
  string src;

  if (!str) {
    notify_fail("Gib WAS an WEN?\n");
    return 0;
  }
  if (QueryProp(P_GHOST))
  {
    notify_fail("Als Geist kannst Du nichts weggeben.\n");
    return 0;
  }

  complex_list = explode(str," ");

  if( (i=member(complex_list,"an"))<0 ) {
    i=sizeof(complex_list)-1;
    complex_list = complex_list[1..] + ({ "an", complex_list[0] });
  }

  if (i==0) {
    notify_fail("WAS möchstest Du weggeben?\n");
    return 0;
  }

  src_list = complex_list[0..i-1];
  dest_list = complex_list[i+1..sizeof(complex_list)-1];
  if ((j = member(src_list,"aus")) >0)
    src_list[j] = "in";
  if ((j = member(src_list,"von")) >0)
    src_list[j] = "in";

  src_obs = select_objects(implode(src_list," "),1,1);
  if (!src_obs || !sizeof(src_obs)) {
    notify_fail("Sowas siehst Du hier nicht!\n");
    return 0;
  }
  return give_to(src_obs,dest_list);
}
