// Tamedhon MUDlib
//
// std/npc/info.c -- Behandle Fragen von Spielern
//
// $Date: 2008/08/16 22:00:00 $
/* $Revision: 4.2 $
 * $Log: info.c,v $
 *
 * Revision 4.2  2008/08/16 22:00:00  TamTam
 * Lastseen nach Suche in Info und LIB verschoben
 *
 * Revision 4.1  2008/07/30 08:28:00  Querolin
 * Lastseen in do_frage integriert.
 *
 * Revision 4.0  2004/01/12 11:57:00  Serii
 * rassenspezifische infos (wie bei z.b. AddDetail) eingebaut
 *
 * Revision 3.6  1998/01/24 20:19:56  Feyaria
 * Taube hoeren antworten von npcs nicht mehr
 *
 * Revision 3.5  1998/01/21 16:27:42  Feyaria
 * Fragen nach .info mitloggen
 *
 * Revision 3.4  1998/01/04 00:52:26  Woody
 * Auch Einzeiler-AddInfos mit break_string() umbrechen
 *
 * Revision 3.3  1997/09/27 18:35:22  Woody
 * Ausgabe der Frage an den Raum vor Closure- und process_string()
 * Auswertung vorgezogen
 *
 * Revision 3.2  1997/06/26 00:59:08  Woody
 * lower_case(id) bei fragen (damit geht auch "frage Monster nach ...")
 *
 * Revision 3.1  1997/05/14 11:50:19  Woody
 * silent flag fuer AddInfo, Ausgabe der Frage auch an Fragenden
 *
 * Revision 3.0  1997/03/13 01:24:17  Woody
 * prepared for revision control
 *
 *
 * 19.02.1997  20:52:00  Woody
 * localflag bei AddLibrary(); wenn gesetzt, werden die Infos aus der
 * Library jeder Instanz des NPC hinzugefuegt und nicht weiter vom
 * Master verwaltet.
 *
 * 12.02.1997  21:15:00  Woody
 * AddLibrary() via libmaster
 *
 * 26.10.1996  17:30:00  Woody
 * AddInfo nimmt als antwort jetzt auch ein Array. Aus diesem wird bei
 * Befragung des NPC's eine Antwort per random ausgewaehlt.
 *
 * 26.09.1996  05:25:00  Woody
 * Exploration Handling eingebaut, GetInfo() modifiziert
 *
 * Letzte Aenderung: 08.11.95	Mupfel@AnderLand
 *
 * 12.03.1996  18:35:00  Mupfel
 * Defines geaendert
 *
 * 05.03.1996  11:00:00  Hadra
 * GetInfo() eingebaut
 *---------------------------------------------------------------------------
 * /std/npc/info.c
 * This file is part of the MorgenGrauen-Mudlib from Jof and Rumata.
 *
 * Behandle Fragen von Spielern an dieses Monster.
 *
 * AddInfo( schluessel, antwort [, indent [, silent] ] )
 *  Wenn ein Spieler dieses Monster nach "schluessel" fragt, so gib die
 *  programmierte Antwort aus. Wenn 'antwort' eine Closure ist, so wird
 *  ihr beim Aufruf der Schluessel als Parameter uebergeben.
 *  Ist silent==1, so wird weder Frage noch Antwort an den Raum aus-
 *  gegeben, ist silent ein String, so wird dieser statt der Antwort
 *  an den Raum ausgegeben.
 *
 * RemoveInfo( schluessel )
 *  Das Monster antwortet nicht mehr auf diesen Schluessel.
 *
 * SetProp( P_DEFAULT_INFO, antwort [, indent ] )
 *  Setze die Antwort, die das Monster auf unverstaendliche Fragen geben
 *  soll. (Diese Funktion ist obsolet! Benutze stattdessen
 *  AddInfo( "\ndefault info", antwort [, indent ] );
 *
 * SetProp( P_LOG_INFO, 1 )
 *  Schreibt unbeantwortete Fragen ins Reportfile des zustaendigen Magiers
 * SetProp( P_LOG_INFO, dateiname )
 *  Schreibt unbeantwortete Fragen in /log/loginfo/dateiname
 *
 * Die Antworten sollten wie emote - kommandos aussehen.
 * Der optionale Indent wird zum Umbrechen von langen Infos benutzt.
 * (Typischerweise sollte indent="sagt: " sein.)
 *
 * In den Infos darf mit process_string gearbeitet werden. Das Ergebnis von
 * process_string wird dann mit umgebrochen!
 *
 *---------------------------------------------------------------------------
 */

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <properties.h>
#include <language.h>
#include <npc/info.h>
#include <defines.h>
#include <config.h>
#include <exploration.h>


nosave mapping infos;
mapping libraries;
private static string *explore;

void create() {
  infos = ([DEFAULT_INFO:"schaut Dich fragend an.\n";0;0]);
  explore = EPMASTER->QueryExplore(EP_INFO);
  libraries = ([]);
}

void init() {
  add_action( "frage", "frag", 1 );
}

static void smart_npc_log(string str) {
  string creat;
  string *strs;

  if (stringp(creat=QueryProp(P_LOG_INFO)))
    creat="loginfo/"+creat;
  else {
    creat = MASTER->creator_file(this_object());
    if (creat == ROOTID)
      creat = "ROOT";
    else if( !creat || creat[0]==' ' )
      creat="STD";
    strs= MASTER->full_path_array(object_name(this_object()),0);
    if( strs[0]=="d" )
      creat = "report/"+strs[1]+"/"+creat+".info";
    else
      creat="report/"+creat+".rep";
  }

  log_file(creat,
           sprintf("FRAGE von %s an %s (%s):\n%s\n",
                   getuid(this_interactive()),
                   explode(object_name(this_object()),"#")[0],
                   dtime(time())[5..<11],
                   str));
}

int do_frage(string question, string text);

int frage(string str)
{
  string myname, text, question;

  str=(extern_call()?this_player()->_unparsed_args():str);
  if( !str || sscanf( str, "%s nach %s", myname, text ) != 2 ) {
    _notify_fail( "WEN willst Du nach WAS fragen?\n" );
    return 0;
  }

  if( !this_object()->id( lower_case(myname) ) )
  {
    _notify_fail( "So jemanden findest Du hier nicht.\n" );
    return 0;
  }
  tell_object(PL,"Du fragst "+ME->name(WEN,1)+" nach "+capitalize(text)+".\n");
  question = PL->Name(WER)+" fragt "+ME->name(WEN,2)+" nach "+
    capitalize(text)+".\n";

  text = lower_case(text);
  if (explore && member(explore, text) >= 0)
    EPMASTER->GiveExplorationPoint(text, EP_INFO);

  return do_frage(question, text);
}

int do_frage(string question, string text)
{
  mixed answer, silent;
  string indent;
//--Serii
  string race;
//--iireS

  answer=indent=0;

  if (!member(infos, text))
  {
    string *libs, *result;
    int i;
    libs = m_indices (libraries);
    for (i=sizeof(libs)-1; i>=0; i--)
      if (result=INFOLIB_MASTER->GetInfo( libs[i], text, libraries[libs[i]],
                                          this_player() ))
      {
        answer=result[0];
        indent=result[1];
        if (sizeof(result) > 2) silent=result[2];
//--Serii
	if(mappingp(answer))
	{
	  race=lower_case(this_player()->QueryProp(P_RACE));
          if(stringp(answer[race]))answer=answer[race];
	  else answer=answer[0];
	  if(!answer)
	  {
	    if(this_interactive()&&QueryProp(P_LOG_INFO))
	      smart_npc_log(text);
            text=DEFAULT_INFO;
	  }
	}
//--iireS
        break;
      }
    if (!answer)
    {
      if( this_interactive() && QueryProp(P_LOG_INFO) )
        smart_npc_log(text);
      text = DEFAULT_INFO;
    }
  }

  if (!answer)  // nothing found in libraries
  {
//--Serii
    if(mappingp(answer=infos[text,0]))
    {
      race=lower_case(this_player()->QueryProp(P_RACE));
      if(stringp(answer[race]))answer=answer[race];
      else answer=answer[0];
      if(!answer)
      {
        if (pointerp(answer=infos[text,0]))
        answer=answer[random(sizeof(answer))];
	if(this_interactive()&&QueryProp(P_LOG_INFO))
          smart_npc_log(text);
        text=DEFAULT_INFO;
      }
    } else
//--iireS
    if (pointerp(answer=infos[text,0]))
      answer=answer[random(sizeof(answer))];
    indent=infos[text,1];
    silent=infos[text,2];
  }

// Querolin
  if (!answer)
  {
    answer="/p/service/querolin/lastseen"->lsm_last(this_object(),text);
//    tell_object(find_player("querolin"), "Answer: "+answer+".\n");

  }
// Querolin

  if (!silent || !intp(silent))
    say(question, PL);

  if ( closurep(answer) )
  {
    answer = funcall(answer, text);
    if (!stringp(answer))
      return 1;
  }
  answer = process_string(answer);

  if (indent)
    answer = break_string(answer, 78, ME->Name(WER,2)+" "+indent, 1);
  else
    answer = break_string(ME->Name(WER,2)+" "+answer, 78, 0, 1);

  if( PL->QueryProp(P_DEAF) )
    tell_object(PL, ME->Name(WER,2)+ " sagt etwas, aber Du kannst nichts "
                "hören.\n");
  else
    tell_object(PL, answer);
  if (intp(silent) && silent)
    return 1;
  if (stringp(silent))
    say(process_string(silent), PL);
  else
    say(answer,PL);

  return 1;
}

/*
 *---------------------------------------------------------------------------
 * Setzen von Infos
 *---------------------------------------------------------------------------
 */

varargs void AddInfo(mixed key, mixed info, string indent, mixed silent)
{
  if( pointerp( key ) )
  {
    int i;
    for ( i=sizeof( key )-1; i>=0; i-- )
      infos += ([ key[i]: info; indent; silent ]);
  }
  else
    infos += ([ key: info; indent; silent ]);
}

void RemoveInfo(string key)
{
  efun::m_delete(infos, key);
}

varargs void _set_default_info( string info, string indent )
{
  AddInfo( DEFAULT_INFO, info, indent );
}

varargs mixed GetInfo(string key)
{
  if (!key) return infos;
  return infos[key];
}

varargs void AddLibrary(string library, int level, int localflag)
{
  if (localflag)
    INFOLIB_MASTER->NewLib(library, 1, this_object(), level);
  else {
    library = MASTER->make_path_absolute(library);
    libraries += ([ library: level ]);
  }
}

void RemoveLibrary(string library)
{
  library = MASTER->make_path_absolute(library);
  libraries = m_delete(libraries, library);
}

