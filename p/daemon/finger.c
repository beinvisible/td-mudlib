// Tamedhon MUDlib
//
// /p/daemon/finger -- finger object
//
// 2012/04/02 Grynnis - Ausgabe angepasst fuer Sprachausgabe
// 2007/01/12 Mallia ab in die Balance - Gralkor
// 2006/11/20 Zweitspieler koennen jetzt auch unerkannt markiert werden
// 2006/08/31 wieder mal die Bala modifiziert Gralkor
/* $Revision: 2.96 $
 * $Log: finger.c,v $
 *
 * Revision 2.96 2008/08/14 21:00:00  TamTam
 * Bug im GuildTitleMapping: Indexing on illegal type
 *
 * Revision 2.95 2008/03/21 14:00:00  TamTam
 * Erzseher im Finger anzeigen
 *
 * Revision 2.94 2008/02/26 21:00:00  TamTam
 * Benutzt Balancemaster um Balance im Finger anzuzeigen
 * Regionsmagier/Regionsmitarbeiter/Vater einfacher und anders formatiert
 *
 * Revision 2.93 2008/01/19 12:00:00  TamTam
 * Mailadresse nicht mehr anzeihen, wenn unsichtbar
 *
 * Revision 2.92 2006/08/12 12:12:00  Querolin
 * Vereinsmitgliedschaft anzeigen. (zwoelf is eine gute Zahl...)
 *
 * Revision 2.91 2005/07/09 16:06:00  Querolin
 * Gildenlevel wird nun endlich auch bei ausgeloggten angezeigt.
 *
 * Revision 2.9  2005/07/06 07:31:00  Querolin
 * P_LAST_ADVANCE eingebaut
 *
 * Revision 2.8  2005/02/16 20:00:00  Querolin
 * Scheidungsmaster Abfrage eingefuegt.
 *
 * Revision 2.7  2004/11/13 10:00:00  Gralkor
 * Anzeige der Balance-Spieler an die aktuelle Bala angepasst
 *
 * $Log: finger.c,v $
 * Revision 2.6  2003/10/26 14:23:00  Querolin
 * Web-Finger Mailadressen und ICQ rausgenommen (SPAM!!!)
 *
 * Revision 2.5  2003/09/16 08:35:00  Gralkor
 * Anzeige der Balance-Spieler
 * mag zwar doof sein, das hier in altes Rev reinzuschreiben, aber:
 * 2004-06-01: Shahin in Sheriff getan, Nekura aus Bala entfernt...
 * 2004-11-10: Kati in die Bala, Gembal und Priwinn raus
 *
 * Revision 2.4  2003/01/02 18:00:00  Querolin
 * Anzeige der Rasse "Frosch"
 *
 * Revision 2.3  2002/11/09 18:43:00  Querolin
 * Bugfix in der Anzeige der Magierkinder
 *
 * Revision 2.2  2002/02/19 12:45:00  Querolin
 * "finger mich" implementiert.
 *
 * Revision 2.1  2002/02/14 18:45:00  Querolin
 * Kleine Bugfixes und FingerMail (hat wer Post?) eingebaut.
 *
 * 5. August 2001 Samtpfote
 * Einbau von 'geschieden' und 'verwitwet'
 *
 * Revision 2.0  2001/06/04 12:45:00  Querolin
 * P_ICQ und P_HOMEPAGE eingebaut.
 *
 * Revision 1.9  2001/05/31 06:55:00  Querolin
 * Bugfix bei Playerkill
 *
 * Revision 1.8  2001/04/21 18:55:30  Querolin
 * Bugfix bei zu langer Mailadresse.
 *
 * Revision 1.7  2001/04/13 10:13:00  Querolin
 * Bugfix mit dem ANSI des angez. Namens (www-who etc)
 *
 * Revision 1.6  2001/04/12 19:27:00  Querolin
 * Regionsmagier und Mitarbeiter mehrzeilig ermoeglicht.
 *
 * Revision 1.5  2001/03/26 17:05:00  Querolin
 * Eingebaut dass man angezeigt bekommt wenn man gefingert wird.
 *
 * Revision 1.4  2001/02/01 14:43:09  Samtpfote
 * Korrektur der Magierraenge auf ein Mapping
 *
 * Revision 1.3  2001/02/01 14:37:45  Samtpfote
 * RCS checkin
 *
 *
 * Revision 1.3  2000/10/29 10:00:00  Querolin
 * Neues Design des Fingers
 *
 * Revision 1.2  2000/05/28 08:59:07  Samtpfote
 * magierlevel angepasst.
 *
 * Revision 1.1  2000/05/28 08:53:13  Samtpfote
 * Initial revision
 *
 * Revision 1.9  1998/02/12 15:29:06  Woody
 * P_DEFAULT_GUILD Handling, Darkelf == Dunkelelf
 *
 * Revision 1.8  1997/11/05 20:00:13  Feyaria
 * Silberland -> MUDNAME
 *
 * Revision 1.7  1997/10/18 01:15:48  Woody
 * Altersabfrage sollte jetzt auch bei Netztoten klappen
 *
 * Revision 1.6  1997/09/08 18:32:09  Woody
 * IP-Namen alternativ via Property P_CALLED_FROM_IP_NAME auslesen
 *
 * Revision 1.5  1997/06/02 10:06:24  Feyaria
 * gildenlevel
 *
 * Revision 1.4  1997/03/19 23:37:14  Woody
 * Auswerten von finger.init
 *
 * Revision 1.3  1997/03/17 19:46:34  Woody
 * ZweitspielerIN, TestspielerIN, Banishgrund anzeigen
 *
 * Revision 1.2  1997/03/16 03:39:06  Woody
 * Projekt und Plan werden nicht mehr in /p/service/loco/plans/
 * sondern in /p/service/save/plans/ gesucht
 *
 * Revision 1.1  1997/03/16 03:35:43  Woody
 * prepared for revision control
 *
 * 23.02.1997 Feyaria Weibliche Magiertiteln
 * 12.02.1997 Woody  Questmagier als solche anzeigen
 * 31.01.1997 Woody  age bei Netztoten aus den properties holen...
 * 22.01.1997 Woody  P_CALLED_FROM_IP(_NAME), ip, ipnum, country(), etc.
 * 21.01.1997 Woody  TOWND verwenden fuer Anzeige der Organisation
 * 09-30-1996 Woody  Einlog-Datum wird angezeigt, Idle-Zeit raufgerueckt,
 *                   Anzeige des ersten Logins
 * 09-24-1996 Woody  parentmaster ist jetzt in secure; parent check auf != ""
 * 05-27-1996 Woody  Ausgabe von Vorfahr/Kindern bei Magiern
 *                   (Anregung von Hadra@AL)
 * 05-09-1996 Woody  bei leerem Titel wird jetzt fuer Seher++ nicht mehr
 *                   automatisch der Gildentitel angezeigt.
 * 04-03-1996 Hadra  Unsichtbare Magier altern nicht mehr
 * 05-12-1995 Mupfel@AL  Ausgabe modifiziert, Titel gefixed
 * 24-11-1995 Gundur@AL  Ausgabe modifiziert
 * 28-09-1995 Hadra@AL   Ausgabe modifiziert
 * 02-05.Dez94 -n, -p
 * 15.Jan 1994 angepasst an neues Speicherformat
 * 13.Okt .plan und .project koennen auch in ~loco/plans sein.
 * 29.Okt 1993 Seherversion.
 * spaeter auch fuer externen Aufruf verwendbar gemacht
 */

/* a finger
 * Original (c) 14.03.1993 by Taube @Nightfall
 * Umsetzung fuer Morgengrauen 09.08.1993 by Loco

   Verwendung ausserhalb von Morgengrauen ist gestattet unter folgenden
   Bedingungen:
   - Benutzung erfolgt auf eigene Gefahr. Jegliche Verantwortung wird
     abgelehnt.
   - Auch in veraenderten oder abgeleiteten Objekten muss ein Hinweis auf
     die Herkunft erhalten bleiben.
   Ein Update-Service besteht nicht.

 */

#pragma strong_types,save_types
#pragma no_clone, no_shadow

#include <config.h>
#include <properties.h>
#include <wizlevels.h>
#include <new_skills.h>
#include <moredef.h>
#include <parentmaster.h>
#include <ansi.h>

#define HBINT    2   /* interval between two heart_beats in seconds */
#define MINIDLE 60   /* minimum idle time in seconds to be stated idle */
#define wiz (local && IS_LEARNER(TP))
#define seer (local && IS_SEER(TP))
// #define TMASTER "/p/service/querolin/team/teammaster.c"
#define SCHEIDUNGSMASTER "/secure/scheidung.c"
#define BALANCEMASTER "/p/service/tamtam/balancemaster.c"

mapping properties;
int age;
string stra,playername;
static string parent, *children;
static mapping wiznames, aliases;

string timediff(int time);
varargs mixed finger_single(string str,int local);

int parse_init_line(string str)
{
  string cmd,arg,rest;
  int i;

  if (!stringp(str) || !sizeof(str) || str[0]=='#') return 0;
  cmd=explode(str," ")[0];
  arg=explode(str," ")[1];
  rest=implode(explode(str," ")[2..]," ");
  switch (lower_case(cmd))
  {
    case "wizname":
      wiznames[arg]=rest;
      break;
    case "alias":
      aliases[arg]=rest;
      break;
    default:
      // Error parsing init file
  }
  return 1;
}

void create()
{
  mixed data;
  seteuid(getuid());

  // SL-Erweiterungen (Wiznames und Aliases aus finger.init)
  //
  wiznames = ([]);
  aliases = ([]);
  if (data = read_file(explode(object_name(TO),"#")[0]+".init"))
    map(explode(data, "\n"), #'parse_init_line);
}

void init()
{
  if(this_player() == environment() && IS_SEER(this_player())) {
    add_action("Finger", "tfinger");
    add_action("Finger","finger");
  }
}

varargs string long()
{
  return
    "Mit dem Finger kannst Du ein paar Informationen auch über ausgeloggte "+
    "Spieler\nabfragen. Befehl ist:\ntfinger <name> oder finger <name>\n";
}

mixed Finger(string str)
{
  if(!str || str=="") { notify_fail("Wen denn?\n"); return 0; }
  else
    return finger_single(str,1);
}

varargs string wiz_level_name(int level, int gender, string name)
{
  if (name && member(wiznames,name)) return wiznames[name];

  if(gender == FEMALE)
  { return WIZ_FEMALE_TITLES[level]; }
  else
  { return WIZ_MALE_TITLES[level]; }

  return "(hat einen ungültigen Magierlevel)";
}

varargs mixed finger_single(string str,int local)
{
  mixed  *userinfo;
  string ip,text,ipnum,filename,guild,title,pp;
  int wizlevel,playerlevel,idle,pos,tmp,noplan,last;
  int i,anzahl;
  mixed h,data;
  mapping gl;
  object player;

  // Set(P_HOMEPAGE, SAVE, F_MODE_AS);

  str=lower_case(str);
  text="";

  h=new_explode(str," ");
  if (member(h,"-n")>=0) noplan=1;
  if (member(h,"-p")>=0) noplan=0;
  h=(h-({"-n","-p"}));
  if (!sizeof(h)) {
    text="Du solltest schon sagen, wer Dich interessiert!\n";
    if (local) return write(text),0;
    else return text;
  }
  str=h[0];
  sscanf(str, "gast%d%s", i, text);
  // dh. fuer Gast1..n ist d != 0 und text ein Leerstring

  if (str=="mich") str=lower_case(TP->Name());

  // Alias-Handling, Variante 1
  /*
  if (member(aliases,str))
  {
    if (local)
      write(CAP(str)+" ist ein Alias auf "+CAP(aliases[str])+".\n");
      str=aliases[str];
  }
  */

  /* does this player exist? */
  userinfo=MASTER->get_userinfo(str);
  player=find_player(str);

  if (previous_object() && previous_object()->QueryProp(P_NO_ASCII_ART))
  pp=" ";  else pp="|";

  if(!userinfo && !player) {
    // Alias-Handling, Variante 2
    if (member(aliases,str))
      text=aliases[str]+"\n";
    else if (text="/secure/master"->QueryBanished(str))
      text=
       "Hoppla - dieser Name ist reserviert oder gesperrt (\"gebanisht\") !\n"+
       (wiz ? "Grund: "+text+"\n" : "");
    else
      text="Hmm... diesen Namen gibt es im "+MUDNAME+" nicht.\n";
    if (local) return write(text),0;
    else return text;
  }
  if (player)
    properties=player->QueryProperties();
  else if (find_netdead(str)) {
    properties=find_netdead(str)->QueryProperties();
    age=find_netdead(str)->QueryProp(P_AGE);
  }
  else
    restore_object("/save/"+str[0..0]+"/"+str);  

  if (!properties)
  {
    text+="Mist!!! Das Einlesen der Daten klappt nicht wie es soll :-(\n";
    properties=0;
    if (!local)
      return text;
    write(text);
    return 1;
  }
  guild=properties[P_GUILD];
  if (!guild || guild=="")
    guild=properties[P_DEFAULT_GUILD] || DEFAULT_GUILD;
  title=properties[P_TITLE];
  if (!query_wiz_level(player) && (!title || title=="") &&
       mappingp(properties[P_GUILD_TITLE]))
    title=(properties[P_GUILD_TITLE])[guild];
  if (!stringp(title)) title="";
  ipnum=properties[P_CALLED_FROM_IP] || "unbekannt";
  ip=properties[P_CALLED_FROM_IP_NAME] || ipnum;
  if(player)
  {
    if (ipnum=query_ip_number(player))
    {
      ip=player->QueryProp(P_CALLED_FROM_IP_NAME) || query_ip_name(player);
      if ((idle=query_idle(player))<MINIDLE)
	idle=0;
    }
    age=(IS_WIZARD(player) && player->QueryProp(P_INVIS))
      ? player->QueryProp(P_INVIS)
      : player->QueryProp(P_AGE);
  }
  else
    // idle=file_date("/save/"+str[0..0]+"/"+str+".o");
    idle=properties[P_LAST_LOGOUT];

  wizlevel=query_wiz_level(str);
  if ((last=properties[P_LAST_LOGOUT])==-1)
    last=file_time("/save/"+str[0..0]+"/"+str+".o");

  /* output routine for all */
    if (!previous_object() || !previous_object()->QueryProp(P_NO_ASCII_ART)) {
      text+=" _________________________________________________________________________\n";
      text+="/                                                                       / \\\n";
      text+="\\_______________________________________________________________________\\\ |\n";
      text+="  |                                                                       |\n";
    }

    if (previous_object()->QueryProp(P_TTY)=="vt100" || previous_object()->QueryProp(P_TTY)=="ansi")
    {
    stra="  "+pp+"  Voller Name: "+ANSI_BOLD+(((h=properties[P_PRESAY]) && h!="") ?h+" " : "")+
    capitalize((h=properties[P_NAME]) ? h : str)+" "+
    ((h=title) ? h : "");
    playername=properties[P_NAME];
    stra=extract(stra,0,73);
    text+=sprintf("%-74s"+ANSI_NORMAL+pp+"\n",stra);
    }
    else
    {
    stra="  "+pp+"  Voller Name: "+(((h=properties[P_PRESAY]) && h!="") ?h+" " : "")+
    capitalize((h=properties[P_NAME]) ? h : str)+" "+
    ((h=title) ? h : "");
    playername=properties[P_NAME];
    stra=extract(stra,0,73);
    text+=sprintf("%-74s"+pp+"\n",stra);
    }
  if (player)
  {
    if(ipnum && (wiz || !(player->QueryProp(P_INVIS))))
    {
      playername=player->query_real_name();
      stra="  "+pp+"  Anwesend seit "+
        (time()-player->QueryProp(P_LAST_LOGIN) >= 86400 ?
          dtime(player->QueryProp(P_LAST_LOGIN))[0..23] :
          dtime(player->QueryProp(P_LAST_LOGIN))[19..23]) +
        " Uhr ";
      text+=sprintf("%-48s",stra);
        stra="";
        if (idle) stra="(Idle-Zeit: "+timediff(idle)+")";
        text+=sprintf("%25s "+pp+"\n",stra);
        stra="  "+pp+"  Er ";
        i=0;
        if (properties[P_GENDER]==2) { stra="  "+pp+"  Sie "; i=1; }
	text+=stra+"kommt von: ";
	if (wiz)
        {
          stra=ip+" ("+ipnum+")";
          if (sizeof(stra)>53) stra=extract(stra,0,53);
          text+=sprintf("%-54s",stra);
          stra=pp+"\n";
          if (i==0) stra=" "+stra;
          text+=stra;
          stra="  "+pp+"  Organisation: "+sprintf("%s - [%s]",
                       TOWND->description(player), country(ipnum));
          text+=sprintf("%-73s "+pp+"\n",stra);
        }
        else
        {
          text+=sprintf("%-53s",country(ipnum));
          stra=" "+pp+"\n";
          if (i==0) stra=" "+stra;
          text+=stra;
       };

    }
    else
    {
      stra="  "+pp+"  "+capitalize(str)+" ist leider nicht anwesend.";
      text+=sprintf("%-73s "+pp+"\n",stra);
      stra="  "+pp+"  Zuletzt eingeloggt von: ";

      if (wiz)
      {
        stra+=ip+" ("+ipnum+")";
        if (sizeof(stra)>72) stra=extract(stra,0,72);
        text+=sprintf("%-73s "+pp+"\n",stra);
        stra="  "+pp+"  Organisation: "+sprintf("%s - [%s]",
                      TOWND->description(ipnum),country(ipnum));
        text+=sprintf("%-73s "+pp+"\n",stra);
      }
      else
      {
        text+=stra+sprintf("%-44s "+pp+"\n",country(ipnum));
      }

      stra="Zuletzt ausgeloggt: "+dtime(last)+" ("+timediff(time()-last)+")";
      text+=sprintf("  "+pp+"  %-68s "+pp+"\n",stra);
    }
  }
  else
  {
      stra="  "+pp+"  "+capitalize(str)+" ist leider nicht anwesend.";
      text+=sprintf("%-73s "+pp+"\n",stra);
      stra="  "+pp+"  Zuletzt eingeloggt von: ";

      if (wiz)
      {
        stra+=ip+" ("+ipnum+")";
        if (sizeof(stra)>72) stra=extract(stra,0,72);
        text+=sprintf("%-73s "+pp+"\n",stra);
        stra="  "+pp+"  Organisation: "+sprintf("%s - [%s]",
                      TOWND->description(ipnum),country(ipnum));
        text+=sprintf("%-73s "+pp+"\n",stra);
      }
      else
      {
        text+=stra+sprintf("%-44s "+pp+"\n",country(ipnum));
      }

      stra="Zuletzt ausgeloggt: "+dtime(last)+" ("+timediff(time()-last)+")";
      text+=sprintf("  "+pp+"  %-68s "+pp+"\n",stra);

  // }

    // stra=TMASTER->_queryteam(str);
    // if (stra)
    // {
    // stra="  "+pp+"  "+(properties[P_GENDER]==MALE?"Er":"Sie")+" ist stolzes Mitglied im Team '"+
    //       capitalize(stra)+"'";
    // stra=extract(stra,0,73);
    // text+=sprintf("%-74s"+pp+"\n",stra);
    }

if (properties[P_GHOST])
  {
    stra="  "+pp+"  Hoppla, derzeit ist "+capitalize(str)+" ein Geist!";
    text+=sprintf("%-73s "+pp+"\n",stra);
  }

if (i="secure/mailer"->FingerMail(str))
{
   switch(i) {
      case 1 : stra="einen ungelesenen Brief."; break;
      case 2 : stra="zwei ungelesene Briefe."; break;
      case 3 : stra="drei ungelesene Briefe."; break;
      case 4 : stra="vier ungelesene Briefe."; break;
      case 5 : stra="fünf ungelesene Briefe."; break;
      default : stra=to_string(i); stra+=" ungelesene Briefe."; break;
   }
   stra="  "+pp+"  "+(properties[P_GENDER]==MALE?"Er":"Sie")+
         " hat derzeit "+stra;

    stra=extract(stra,0,73);
    text+=sprintf("%-74s"+pp+"\n",stra);

}

if (BALANCEMASTER->__query(str) == 1)
{
    stra="  "+pp+"  "+(properties[P_GENDER]==MALE?"Er":"Sie")+" ist ";
    if (wizlevel>=10)
        stra+="Balancemagier.";
    else
        stra+="Spielermitglied der Balance.";
    stra=extract(stra,0,73);
    text+=sprintf("%-74s"+pp+"\n",stra);
}
if (str=="hallgrim") {
      stra="  "+pp+"  Er ist HauptNetzMeister im Adminteam des Tamedhon. ";
      stra=extract(stra,0,73);
      text+=sprintf("%-74s"+pp+"\n",stra);
     }
if (str=="kati")  {
    stra="  "+pp+"  Sie ist die Spielervertretung im Sheriff Team. ";
    stra=extract(stra,0,73);
    text+=sprintf("%-74s"+pp+"\n",stra);
  }
  if (!previous_object() || !previous_object()->QueryProp(P_NO_ASCII_ART))
    text+="  +-----------------------------------+-----------------------------------+\n";
  stra= "  "+pp+"  Rasse: "+
    ((h=properties[P_RACE]) ? h :
       ( (userinfo && pointerp(userinfo) && sizeof(userinfo)>4 &&
          stringp(userinfo[4]) &&
          sizeof(h=explode(userinfo[4],"/"))>2 ) ?
            (h=capitalize(h[2]),
             ([ "Human"   : "Mensch",
                "Dwarf"   : "Zwerg",
		"Orc"	  : "Ork",
                "Darkelf" : "Dunkelelf"])[h] || h) :
            "<keine>"));
  if (properties[P_FROG]!=0) {
   stra= "  "+pp+"  Rasse: Frosch"; }
   if (previous_object() && previous_object()->QueryProp(P_NO_ASCII_ART))
     text+=sprintf("%-38s\n     Gilde: %-25s "+pp+"\n",stra,capitalize(guild));
   else
     text+=sprintf("%-38s"+pp+"  Gilde: %-25s "+pp+"\n",stra,capitalize(guild));

//    (seer ? "Alter: "+timediff(age*HBINT)+",  " : "")+

    if (wizlevel>=10)
    {
       // Folgt der Form halber weiter unten ;)
    }
    else
    {
     if (wizlevel==0)
     {
      stra="  "+pp+"  Spieler Level: "+properties[P_LEVEL];
     }
     else
     {
      if (properties[P_CAN_FLAGS]&CAN_ERZ)
      {
        stra="  "+pp+"  Erzseher"+(properties[P_GENDER]==FEMALE ? "in":"");
        stra+=" Level: "+properties[P_LEVEL];
      }
      else
      {
        stra="  "+pp+"  Seher"+(properties[P_GENDER]==FEMALE ? "in":"");
        stra+=" Level: "+properties[P_LEVEL];
      }
     }

     if (properties[P_LAST_ADVANCE])
     {
        if ((time() - properties[P_LAST_ADVANCE]) < 86400)
        {
           anzahl=(time() - properties[P_LAST_ADVANCE]) / 3600;
           if (anzahl==1)
           {
             stra+=" ("+to_string(anzahl)+" Stunde)";
           }
           else
           {
             stra+=" ("+to_string(anzahl)+" Stunden)";
           }
        }
        else
        {
           anzahl=(time() - properties[P_LAST_ADVANCE]) / 86400;
           if (anzahl==1)
           {
             stra+=" ("+to_string(anzahl)+" Tag)";
           }
           else
           {
             stra+=" ("+to_string(anzahl)+" Tage)";
           }
        }
     }

     text+=sprintf("%-38s",stra);
     stra="";
     if (previous_object() && previous_object()->QueryProp(P_NO_ASCII_ART))
        text+="\n  ";
   // Gildenlevel laut original Finger - funktioniert aber leider net... :(
//    stra=(((h=properties[P_GUILD_LEVEL]) && h=h[properties[P_GUILD_LEVEL]]) ?
//      ("Gildenlevel: "+h) : "" );
    if (guild=="abenteurer")
    {
       // Bei den Abenteurern wird der Gildenlevel (vermutlich da auch
       // automatisch gleich Level) nicht (immer) korrekt gesetzt.
       stra="Gildenlevel: "+properties[P_LEVEL];
    }
    else
    {
       gl=properties[P_GUILD_LEVEL];
       if (guild && mappingp(gl)) {
       stra="Gildenlevel: "+to_string(gl[lower_case(guild)]); }
    }
//        stra="Gildenlevel: "+to_string(properties[P_GUILD_LEVEL]);
    // ein kleiner versuch noch doch an den gildenlevel zu kommen ;)
     if (stra=="")
     {
       if (player) stra="Gildenlevel: "+player->QueryProp(P_GUILD_LEVEL);
     }

     if (properties[P_LAST_ADVANCE_GUILD])
     {
        if ((time() - properties[P_LAST_ADVANCE_GUILD]) < 86400)
        {
           anzahl=(time() - properties[P_LAST_ADVANCE_GUILD]) / 3600;
           if (anzahl==1)
           {
             stra+="  ("+to_string(anzahl)+" Stunde)";
           }
           else
           {
             stra+="  ("+to_string(anzahl)+" Stunden)";
           }
        }
        else
        {
           anzahl=(time() - properties[P_LAST_ADVANCE_GUILD]) / 86400;
           if (anzahl==1)
           {
             stra+="  ("+to_string(anzahl)+" Tag)";
           }
           else
           {
             stra+="  ("+to_string(anzahl)+" Tage)";
           }
        }
     }

     text+=sprintf(pp+"  %-32s "+pp+"\n",stra);
   }

    stra="  "+pp+"  Geschlecht: "+
    ( ([ NEUTER:"neutral ?!", MALE:"männlich",
         FEMALE:"weiblich"]) [properties[P_GENDER]] ||
      "<verdammt seltsam>");
    text+=sprintf("%-38s"+pp+"  ",stra);

   if (previous_object() && previous_object()->QueryProp(P_NO_ASCII_ART))
      text+="\n     ";
   if ( find_player(str) && (properties[P_INVIS]!=0))
   {
     stra="Alter: "+timediff(properties[P_AGE]*HBINT);
   }
   else
   {
      stra="Alter: "+timediff(age*HBINT);
   }
      text+=sprintf("%-32s "+pp+"\n",stra);

   h=lowerstring(properties[P_MARRIED]);
   switch(h)
   {
	case "verwitwet":
	    stra="  "+pp+"  Spieler"+
		 (properties[P_GENDER]==FEMALE?"in":"")+
		 " ist verwitwet";
	    /*if(properties[P_GENDER] == MALE)
	    { stra+= "r"; }*/
	    break;

	case "geschieden":
	    stra="  "+pp+"  Ehe geschieden";
	    break;

	case "":
	case 0:
	    stra="  "+pp+"  Derzeit ohne Ehepartner";
	    break;

	default:
	    stra="  "+pp+"  Verheiratet mit "+extract(capitalize(h),0,15);
	    break;
   }
   if (SCHEIDUNGSMASTER->_finger_check(str))
   {
            stra="  "+pp+"  Derzeit in Scheidung";
   }
/*
   stra=((h=properties[P_MARRIED]) ?
     "  "+pp+"  Verheiratet mit: "+extract(capitalize(h),0,15)
     )
    :
     "  "+pp+"  Derzeit ohne Ehepartner"
    );
*/
    text+=sprintf("%-38s"+pp+"  ",stra);

   if (previous_object() && previous_object()->QueryProp(P_NO_ASCII_ART))
      text+="\n     ";
    if (h=properties[P_DEADS])
    {
      stra="Bisher "+properties[P_DEADS]+" mal gestorben!";
    }
    else
    {
     stra="Bisher nicht gestorben!";
    }
    text+=sprintf("%-32s "+pp+"\n",stra);

  if ( (h=properties[P_TESTPLAYER]) || ( h=properties[P_SECOND]))
  {
  if (h=properties[P_TESTPLAYER])
  {
    if (stringp(h))
    {
    stra="  "+pp+"  Testspieler"+
          (properties[P_GENDER]==FEMALE ? "in" : "");
    }
    else
    {
    stra="  "+pp+"  "+capitalize(str)+" ist Testspieler"+
          (properties[P_GENDER]==FEMALE ? "in" : "");
    }
    if (stringp(h)) stra+=" von "+h;
  }
  else
  {
   stra="  "+pp;
  }

  text+=sprintf("%-37s "+pp,stra);

  if ( h=properties[P_SECOND])
  {
    if (stringp(h))
    {
    stra="  Zweitspieler"+
          (properties[P_GENDER]==FEMALE ? "in" : "");
    }
    else
    {
    stra="  "+capitalize(str)+" ist Zweitspieler"+
          (properties[P_GENDER]==FEMALE ? "in" : "");
    }

    if (intp(properties[P_SECOND_INVIS])) {
        if(query_wiz_level(this_player())<properties[P_SECOND_INVIS]) {
	   if (stringp(h)) stra+=" von UNBEKANNT";

    } else

      if (stringp(h)) stra+=" von "+h;
    }
  }

  else
  {
   stra="";
  }

    text+=sprintf("%-34s "+pp+"\n",stra);
  }

  if (!previous_object() || !previous_object()->QueryProp(P_NO_ASCII_ART))
    text+="  +-----------------------------------+-----------------------------------+\n";

    if (wizlevel>=10)
    {
      stra="  "+pp+"  Magierlevel: "+wizlevel+" - "+
                    wiz_level_name(wizlevel,properties[P_GENDER],str);
      text+=sprintf("%-73s "+pp+"\n",stra);
    }

   (userinfo && pointerp(userinfo)
     ? stra=sprintf("Datum des ersten Login: %s",
       (userinfo[5] > 0 ? dtime(userinfo[5])
                        : "vor dem 15. März 2000"))
     : stra="");
    text+=sprintf("  "+pp+"  %-68s "+pp+"\n",stra);

  if (this_player()) {
   if (( (properties[P_MAILADDR]!="none") &&
         (properties[P_MAILADDR]!=0) &&
         ((properties[P_MAILADDR_INVIS]==0) || IS_ARCH(this_player()) ||
                                               (getuid(this_player())==str)) &&
         (properties[P_MAILADDR]!="") ))
   {
   stra="";
   if ( (properties[P_MAILADDR]!="none") &&
         ((properties[P_MAILADDR_INVIS]==0) || IS_ARCH(this_player()) ||
                                               (getuid(this_player())==str)) &&
         (properties[P_MAILADDR]!=0) &&
         (properties[P_MAILADDR]!="") )
   {
    stra="E-Mail-Adresse: "+properties[P_MAILADDR];

   }

    text+=sprintf("  "+pp+"  %-68s "+pp+"\n",stra);
   }
  }
  if ( (properties[P_HOMEPAGE]!="none") &&
       (properties[P_HOMEPAGE]!=0) &&
       (properties[P_HOMEPAGE]!="") )
  {
    stra="Homepage: "+properties[P_HOMEPAGE];
    if (sizeof(stra)>67)
    {
      stra=extract(stra,0,67);
    }
    text+=sprintf("  "+pp+"  %-68s "+pp+"\n",stra);
  }

  if (properties[P_AWAY])
  {
    stra="z.Z. abwesend, Grund: "+properties[P_AWAY];
    stra=extract(stra,0,67);
    text+=sprintf("  "+pp+"  %-68s "+pp+"\n",stra);
  }

  if(seer&&h=properties[P_KILLS])    //PK's nur fuer Seher und Mags
        {
        stra="";
        if (properties[P_KILLS]==1) { stra="r"; }
        stra="Bereits "+properties[P_KILLS]+" unschuldige"+stra+" Spieler "
        +"wurde"+((properties[P_KILLS]==1)?"":"n")+
        " durch "+(properties[P_GENDER]==MALE?"ihn":"sie")+" getoetet!";
        text+=sprintf("  "+pp+"  %-68s "+pp+"\n",stra);
        }

  if (wizlevel>=10)
  {
    parent = PARENTMASTER->GetParent(str);
    children = map(PARENTMASTER->GetChildren(str), #'capitalize);
    h = "";
    if (parent && parent!="")
    {
      h += ((properties[P_GENDER]==MALE)?"Sohn":
               (properties[P_GENDER]==FEMALE)?"Tochter":"ein Nachkomme")+
                " von "+capitalize(parent);
    }
    if (sizeof(children))
    {
      if (parent) h += " und ";
      h += ((properties[P_GENDER]==MALE)?"Vater":
               (properties[P_GENDER]==FEMALE)?"Mutter":"Vorfahr")+
                " von ";
      tmp = sizeof(children);
      if (tmp > 1)
      {
        h += implode(children[0..<2], ", ")+
          " und "+ children[tmp-1];
      }
      else
      {
        h += children[0];
      }
    }
    if (sizeof(h)) h += ".";
    h=explode(break_string(h,67-sizeof(capitalize(str) + " ist ")),"\n");
    text+=sprintf("  "+pp+"  %-68s "+pp+"\n",capitalize(str) + " ist "+h[0]);
    for(i=1;i<sizeof(h);i++)
      text+=sprintf("  "+pp+"  %-68s "+pp+"\n",
            sprintf("%*s", sizeof(capitalize(str) + " ist "), "")+h[i]);
  }
  if (/*wiz && */userinfo)
  {
    data=userinfo[3];
    if (sizeof(data))
    {
        h=break_string(implode(map(data,#'capitalize/*'*/),", ")+".",44);
        h=explode(h,"\n");
        text+=  sprintf("  "+pp+"  %-68s "+pp+"\n","Regionsmagier von     : "+h[0]);
        for(i=1;i<sizeof(h);i++)
          text+=sprintf("  "+pp+"  %-68s "+pp+"\n","                        "+h[i]);
    }
    data="/secure/master"->get_domain_homes(str);
    if (sizeof(data))
    {
        h=break_string(implode(map(data,#'capitalize/*'*/),", ")+".",44);
        h=explode(h,"\n");
        text+=  sprintf("  "+pp+"  %-68s "+pp+"\n","Regionsmitarbeiter von: "+h[0]);
        for(i=1;i<sizeof(h);i++)
          text+=sprintf("  "+pp+"  %-68s "+pp+"\n","                        "+h[i]);
    }
  }
  filename="/players/"+str+"/.project";
  if(file_size(filename)>=0)
    {
    stra="  "+pp+"  Projekt: "+explode(read_file(filename), "\n")[0];
    stra=extract(stra,0,72);
    text+=sprintf("%-73s "+pp+"\n",stra);
    }
  else
  {
    filename="/p/service/loco/save/plans/"+str+".project";
    if(file_size(filename)>=0)
    {
      stra="  "+pp+"  Projekt: "+explode(read_file(filename), "\n")[0];
      stra=extract(stra,0,72);
      text+=sprintf("%-73s "+pp+"\n",stra);
    }
  }

  if(properties[P_LOGON_PLACE] != 0)
    text+=  sprintf("  "+pp+"  %-68s "+pp+"\n","Heimat: "+properties[P_LOGON_PLACE]);
  else
    text+=  sprintf("  "+pp+"  %-68s "+pp+"\n","Heimat: Unbekannt");
  
  if(find_player(str))
  {
    if(tls_query_connection_state(find_player(str)))
      text+=  sprintf("  "+pp+"  %-68s "+pp+"\n","Verbindung TLS verschlüsselt.");
    else
      text+=  sprintf("  "+pp+"  %-68s "+pp+"\n","Verbindung NICHT TLS verschlüsselt.");
  }
  else
    text+=  sprintf("  "+pp+"  %-68s "+pp+"\n","Offline");

// BEGINN SPIELPAUSE
  if (i=__MASTER_OBJECT__->QueryTBanished(str))
  {
    stra=capitalize(str)+" macht eine Spielpause bis ";
    (i == -1 ? stra+="zum St. Nimmerleinstag." :
        stra+=funcall(symbol_function('dtime),i)[0..16]+".");

    text+=sprintf("  "+pp+"  %-68s "+pp+"\n",stra);
  }

// ENDE SPIELPAUSE

    if (!previous_object() || !previous_object()->QueryProp(P_NO_ASCII_ART))
      text+="  \\_______________________________________________________________________/\n";

  if (seer && !noplan)
  {
    filename="/players/"+str+"/.plan";
    if(file_size(filename)>=0)
      text+="Plan:\n"+read_file(filename);
    else
    {
      filename="/p/service/loco/save/plans/"+str+".plan";
      if(file_size(filename)>=0)
	text+="Plan:\n"+read_file(filename);
//    else
//	text+="Keine .plan-Datei.\n";   // Wird net wirklich gebraucht... :)
    }
  }
  properties=0;
  if (!local)
    return text;

  if(interactive(this_player()))
  {
    this_player()->More(text);
  }
  //  write(text);

//  tell_object(find_player("querolin"),break_string(":"+playername));
  if (!playername) { return 1; }

  if ( (find_player(lower_case(playername))) &&
       (this_player()!=player) )
  {
   if (query_wiz_level(player)>=query_wiz_level(this_player()))
   {
    if (find_player(lower_case(this_player()->QueryProp(P_NAME))))
    {
      tell_object(player,break_string(
              capitalize(this_player()->QueryProp(P_NAME)) + " informierte "
               +"sich gerade über Dich.\n"));
    }
    else
    {
      tell_object(player,break_string("Jemand informierte sich gerade "
            +"über Dich.\n"));
    }
   }
  }

  return 1;
}


string timediff(int time)
{
  string ret;

  ret="";
  if(time>=86400) {
    ret+=time/86400+"d ";
    time%=86400;
  }
  if(time<36000) ret+="0";
  ret+=time/3600+":";
  time%=3600;
  if(time<600) ret+="0";
  ret+=time/60+":";
  time%=60;
  if(time<10) ret+="0";
  ret+=time+"";
  return ret;
}



