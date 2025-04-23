/*
 *  The new ultimate quest administration tool
 *
 * 2001/01/13 13:14:07  Samtpfote
 * 2024-10-10 Gralkor	Auf LD angepasst und alle Warnings beseitigt
 *
 */

inherit "std/thing";
inherit "std/more";

#include <defines.h>
#include <properties.h>
#include <language.h>
#include <wizlevels.h>
#include <questmaster.h>

#define UA this_interactive()->_unparsed_args()

#define QNAME 0
#define QP    1
#define XP    2
#define ALLOW 3
#define INFO  4
#define LEVEL 5
#define NEED  6
#define WIZ   7
#define END   8

#pragma strong_types

void Load_NumKey     ();
int  Add_Quest       ( string str );
void ReadNewQuest    ( string str );
int  Remove_Quest    ( string str );
int  Change_Quest    ( string str );
int  Restore_Quest   ( string str );
int  Set_Quest       ( string str );
int  Remove_Quest    ( string str );
int  Query_Quest     ( string str );
int  Query_Keys      ( string str );
int  Query_Quests    ( string str );
int  ActivateQuest   ( string str );
int  DeactivateQuest ( string str );
int  GetReturn       ( string str );

string name;
int    qp;
int    xp;
int    level;
int    need;
string *allowed;
string info;
string wizard;
mapping num_key;

object owner;
int    counter;
mixed  *savequest;
string savekey;
int    do_change;
string changekey;

void create()
{
  string str;

  if(IS_BLUE(ME)) return;
  thing::create();
  SetProp(P_GENDER,NEUTER);
  owner = PL;
  AddId("questtool");
  AddId("qt");
  SetProp(P_ARTICLE,0);
  SetProp(P_GENDER, NEUTER);
  str=capitalize(((str=owner->query_real_name())[<1]=='s'||str[<1]=='x'||str[<1]=='z')?str+="'":str+="s");
  SetProp(P_NAME, str+" QuestTool");
  SetProp(P_SHORT, str+" QuestTool");
  SetProp(P_LONG,
   "Das QuestTool dient der Administration von Quests. (by Kirk, 10.11.94)\n"
  +"implementierte Kommandos:\n"
  +"  AddQ                          : Neuen Questeintrag einfügen\n"
  +"  RemoveQ     <quest>           : Questeintrag löschen\n"
  +"  ChangeQ     <quest>           : Questeintrag ändern\n"
  +"  ReStoreQ                      : Zuletzt gelöschten oder geänderten\n"
  +"                                : Questeintrag restaurieren\n"
  +"  SetQ        <spieler> <quest> : Quest als 'gelöst' markieren\n"
  +"  DelQ        <spieler> <quest> : Quest als 'nicht gelöst' markieren\n"
  +"  QueryQ      <quest>           : Questeintrag anzeigen\n"
  +"  QueryK                        : Alle Keys und Indices anzeigen\n"
  +"  QueryAllQ                     : Alle Eintraege vollständig anzeigen\n"
  +"  ActiVateQ   <quest>           : Quest im Questmaster aktivieren\n"
  +"  DeactiVateQ <quest>           : Quest im Questmaster deaktivieren\n\n"
  +"  für <quest> kann der Index verwendet werden, den QueryK liefert!\n\n"
  +"  (Alle Kommandos können abgekürzt werden.)\n"
	+"   -> Beispiel: DeactiVateQ == DVQ == dvq == deactivateq\n");

  SetProp(P_NEVERDROP, 1);
  SetProp(P_NODROP, 
  "Mit Hilfe des QuestTools können Quests eingetragen, oder gelöscht\n"+
  "werden. Du brauchst es vielleicht noch, deshalb wirfst Du es nicht weg.\n");
  SetProp(P_NOBUY, 1);
  AddCmd(({ "AddQ", "addq", "AQ", "aq" }),                 "Add_Quest");
  AddCmd(({ "RemoveQ", "removeq", "RQ", "rq" }),           "Remove_Quest");
  AddCmd(({ "ChangeQ", "changeq", "CQ", "cq" }),           "Change_Quest");
  AddCmd(({ "ReStoreQ", "restoreq", "RSQ", "rsq" }),       "Restore_Quest");
  AddCmd(({ "SetQ", "setq", "SQ", "sq" }),                 "Set_Quest");
  AddCmd(({ "DelQ", "delq", "DQ", "dq" }),                 "Del_Quest");
  AddCmd(({ "QueryQ", "queryq", "QQ", "qq" }),             "Query_Quest");
  AddCmd(({ "QueryK", "queryk", "QK", "qk" }),             "Query_Keys");
  AddCmd(({ "QueryAllQ", "queryallq", "QAQ", "qaq" }),     "Query_Quests");
  AddCmd(({ "ActiVateQ", "activateq", "AVQ", "avq" }),     "ActivateQuest");
  AddCmd(({ "DeactiVateQ", "deactivateq", "DVQ", "dvq" }), "DeactivateQuest");
  counter = QNAME;
  savekey = "";
  do_change = 0;
  num_key = ([]);
}

int _query_autoloadobj() { return 1; }

void init()
{
  object tp;

  if(!(tp = PL) || tp != environment())
    return;

  thing::init();

  if ( !IS_LORD(tp) || tp != owner )
    return call_out("do_remove",1);
}

static void do_remove()
{
  write ( ME->QueryProp(P_SHORT)+" zerfällt zu Staub.\n");
  say ( ME->QueryProp(P_SHORT)+" zerfällt in "+PL->name(WESSEN)+
       " unbefugten Händen zu Staub.\n");
  call_out("remove",1);
}

void Load_NumKey()
{
  string* keys;
  int i;

  if ( !(keys = QM->QueryAllKeys()) ) return;
  keys=sort_array(keys,#'>);//')
  for ( i = 0, num_key = ([]); i < sizeof(keys); i++ )
    num_key += ([(i+1)+"":keys[i]]);
}

int Add_Quest(string str)
{
  counter = QNAME;
  write ( "Neue Quest: (Abbruch mit '.' oder '~q')\n" );
  write ( "Key :\n" ); 
  input_to ( "ReadNewQuest" );
  return 1;
}

void ReadNewQuest (string str)
{
  mixed *oldquest;
  int errstat;
  int active;

  if ( str == "~q" || str == "." )
  {
    counter = QNAME;
    if(do_change)
    {
      do_change = 0;
      changekey = "";
      savekey = "";
      savequest = ({});
    }
    return;
  }

  switch ( counter )
  {
    case QNAME:
      if((!str||!sizeof(str)) && do_change)
	name = savekey;
      else
	name = str;
      counter++;
      write ( sprintf("Punkte (>0) [%d]:\n",
		      (do_change?savequest[0]:10)) ); 
      input_to ( "ReadNewQuest" );
      break;
    case QP:
      if((!str||!sizeof(str)))
	if(do_change)
	  qp = savequest[0];
	else
	  qp = 10;
      else
	sscanf ( str, "%d", qp );
      counter++;
      write ( sprintf("Erfahrung (>=0) [%d]:\n",
		      (do_change?savequest[1]:qp*1000)));
      input_to ( "ReadNewQuest" );
      break;
    case XP: 
      if((!str||!sizeof(str)))
	if(do_change)
	  xp = savequest[1];
	else
	  xp = qp * 1000;
      else
	sscanf ( str, "%d", xp );
      counter++;
      write ( sprintf("Filenamen file1[,file2,...]) %s:\n",
		      (do_change?"["+implode(savequest[2],",")+"]":
		       "")) );
      input_to ( "ReadNewQuest" );
      break;
    case ALLOW:
      if((!str||!sizeof(str)) && do_change)
	allowed = savequest[2];
      else
	allowed = explode(implode(explode(str,".c"),""),",");
      counter++;
      write ( sprintf("Info %s:\n", (do_change?
				     "["+break_string("\b\b\b\b\b\b"+					      savequest[3]+"]",78,6)
				     :"")) ); 
      input_to ( "ReadNewQuest" );
      break;
    case INFO:
      if((!str||!sizeof(str)) && do_change)
	info = savequest[3];
      else
	info = str;
      counter++;
      write ( sprintf("Stufe (0<=lev<=20) [%d]:\n",
		      (do_change?savequest[4]:0)) );
      input_to ( "ReadNewQuest" );
      break;
    case LEVEL:
      if((!str||!sizeof(str)))
	if(do_change)
	  level = savequest[4];
	else
	  level = 0;
	sscanf ( str, "%d", level );
      counter++;
      write ( sprintf("Pflicht ([012]) [%d]:\n",
		      (do_change?savequest[5]:0)) ); 
      input_to ( "ReadNewQuest" );
      break;
    case NEED:
      if((!str||!sizeof(str)))
	if(do_change)
	  need = savequest[5];
	else
	  need = 0;
      else
	sscanf ( str, "%d", need );
      counter++;
      write ( sprintf("Magier %s:\n",
		      (do_change?"["+savequest[7]+"]":"")) );
      input_to ( "ReadNewQuest" );
      break;
    case WIZ:
      if((!str||!sizeof(str)) && do_change)
	wizard = savequest[7];
      else
	wizard = str;
      wizard = lower_case(wizard);
      counter++;
      write ( "Eintragen (j/n)?\n" );
      input_to ( "ReadNewQuest" );
      break;
    case END:
      counter=QNAME;

      if ( str != "j" && str != "ja" && str != "y" && str != "yes" )
        return;

      active = 0;
      if ( do_change && changekey && sizeof(changekey) )
      {
        oldquest = QM->QueryQuest ( changekey );

        if ( !pointerp ( oldquest ) || !sizeof ( oldquest ) )
        {
          write ( "Alten Eintrag nicht gefunden.\n" );
          return;
        }

        errstat = QM->RemoveQuest( changekey );

	do_change = 0;
	changekey = "";

        switch ( errstat )
        {
          case  0: write ( "Zugriff auf alten Eintrag verweigert.\n" ); return;
          case -1: write ( "Parameterfehler beim Löschen.\n" ); return;
          case -2: write ( "Alten Eintrag nicht gefunden.\n" ); return;
          default: write ( "Alter Eintrag gelöscht.\n" );
        }
	active = oldquest[6];
	savequest = oldquest;
	savekey   = changekey;
      }

      // add new Quest deactivated by default and keep old active flag
      // if changing an existing entry
      errstat= QM->AddQuest(name,qp,xp,allowed,info,level,need,active,
				 wizard);

      switch ( errstat )
      {
        case  0: write ( "Zugriff verweigert.\n" ); break;
        case -1: write ( "Key ungültig oder sizeof(key) < 5.\n" ); break;
        case -2: write ( "QP ungültig oder < 1.\n" ); break;
        case -3: write ( "XP ungültig.\n" ); break;
        case -4: write ( "Filename(n) ungültig.\n" ); break;
        case -5: write ( "Info ungültig.\n" ); break;
	case -6: write ( "Stufe ungültig oder < 0 oder > 20.\n" ); break;
        case -7: write ( "Aktiv-Flag < 0 oder > 1.\n" ); break;
        case -8: write ( "Magiername ungültig.\n" ); break;
        default: write ( "Eintrag eingefügt.\n" );
      }
    default:
      return;
  }
}

int Remove_Quest ( string str )
{
  mixed *oldquest;
  string newstr;
  int errstat;

  if ( !(str=UA) )
  {
    write ( "Syntax: RemoveQ <name>\n" );
    return 1;
  }

  Load_NumKey();

  if ( !(newstr = num_key[str+""]) )
    newstr = str;

  oldquest = QM->QueryQuest ( newstr );

  if ( !pointerp ( oldquest ) || !sizeof ( oldquest ) )
  {
    write ( "Keine Quest dieses Namens gefunden.\n" );
    return 1;
  }

  errstat = QM->RemoveQuest( newstr );

  switch ( errstat )
  {
    case  0: write ( "Zugriff verweigert.\n" ); break;
    case -1: write ( "Parameterfehler.\n" ); break;
    case -2: write ( "Quest nicht gefunden.\n" ); break;
    default: write ( "Quest entfernt.\n" );
  }

  savequest = oldquest;
  savekey   = newstr;

  return 1;
}

int Change_Quest ( string str )
{
  mixed *oldquest;
  string newstr;

  if ( !(str=UA) )
  {
    write ( "Syntax: ChangeQ <quest>\n" );
    return 1;
  }

  Load_NumKey();

  if ( !(newstr = num_key[str+""]) )
    newstr = str;

  oldquest = QM->QueryQuest ( newstr );

  if ( !pointerp( oldquest ) || !sizeof ( oldquest ) )
  {
    write ( "Keine Quest dieses Namens gefunden.\n" );
    return 1;
  }

  do_change = 1;
  changekey = newstr;
  savekey = newstr;
  savequest = oldquest;

  write ( "Aktueller Eintrag:\n");
  write ( "Key       : "+newstr+"\n" );
  write ( "Punkte    : "+oldquest[0]+"\n" );
  write ( "Erfahrung : "+oldquest[1]+"\n" );
  write ( "Filenamen : "+implode(oldquest[2],",")+"\n" );
  write ( "Info      : "+oldquest[3]+"\n" );
  write ( "Stufe     : "+oldquest[4]+"\n" );
  write ( "Pflicht   : "+oldquest[5]+"\n" );
  write ( "Magier    : "+capitalize(oldquest[7])+"\n" );

  write ( "\nNeue Quest:     (mit '.' oder '~q' kann abgebrochen werden)\n" );
  write ( sprintf("Key [%s]:\n", savekey) );
  input_to ( "ReadNewQuest" );

  return 1;
}

int Restore_Quest ( string str )
{
  int errstat;

  if ( !savekey || !sizeof(savekey) )
  {
    write ( "\nTut mir leid!\n" );
    write ( "Nichts zum Restaurieren gefunden ...\n" );
    return 1;
  }

  errstat =QM->AddQuest( savekey, savequest[0], savequest[1],
			 savequest[2], savequest[3],
			 savequest[4], savequest[5],
			 savequest[6], savequest[7],
                         savequest[8] );

  switch ( errstat )
  {
    case  0: write ( "Zugriff verweigert.\n" ); break;
    case -1: write ( "Key ungültig oder sizeof(key) < 5.\n" ); break;
    case -2: write ( "QP ungültig oder < 1.\n" ); break;
    case -3: write ( "XP ungültig.\n" ); break;
    case -4: write ( "Filename(n) ungültig.\n" ); break;
    case -5: write ( "Info ungültig.\n" ); break;
    case -6: write ( "Stufe ungültig oder < 0 oder > 20.\n" ); break;
    case -7: write ( "Aktiv-Flag < 0 oder > 1.\n" ); break;
    case -8: write ( "Magiername ungültig.\n" ); break;
    default: write ( "'"+savekey+"' restauriert.\n" );
  }
  savekey = "";
  return 1;
}

int Set_Quest ( string str )
{
  string quest, player, newquest;
  object ob;

  if ( !(str=UA) || sscanf( str, "%s %s", player, quest ) != 2 )
  {
    write ( "Syntax: SetQ <player> <quest>\n" );
    return 1;
  }

  Load_NumKey();

  if ( !(newquest = num_key[quest+""]) )
    newquest = quest;

  if ((ob=find_player(player)))
    write(ERRNO_2_STRING("GQ",ob->GiveQuest(newquest,"__silent__"))+"\n");
  else
    write("Kein solcher Spieler eingeloggt.\n");
  return 1;
}

int Del_Quest ( string str )
{
  string quest, player, newquest;
  object ob;

  if ( !(str=UA) || sscanf( str, "%s %s", player, quest ) != 2 )
  {
    write ( "Syntax: DelQ <player> <quest>\n" );
    return 1;
  }

  Load_NumKey();

  if ( !(newquest = num_key[quest+""]) )
    newquest = quest;

  if ((ob=find_player(player)))
    write(ERRNO_2_STRING("DQ", ob->DeleteQuest ( newquest ))+"\n");
  else
    write("Kein solcher Spieler eingeloggt.\n");
  return 1;
}

int Query_Quest ( string str )
{
  mixed *quest;
  string newstr;
  int i;

  if ( !(str=UA) )
  {
    write ( "Syntax: QueryQ <quest>\n" );
    return 1;
  }

  Load_NumKey();

  if ( !(newstr = num_key[str+""]) )
	newstr = str;

  quest = QM->QueryQuest( newstr );

  if ( !pointerp( quest ) || !sizeof ( quest ) )
  {
    write ( "Keine Quest dieses Namens gefunden.\n" );
    return 1;
  }

  write ( "Aktueller Eintrag:\n");
  write ( "Key       : "+newstr );
  if(quest[6])
    write (" (aktiviert)\n");
  else
    write (" (deaktiviert)\n");
  write ( "Punkte    : "+quest[0]+"\n" );
  write ( "Erfahrung : "+quest[1]+"\n" );
  write ( break_string ( implode( quest[2], " " ), 78, "Filenamen : " ) );
  write ( break_string ( quest[3], 78, "Info      : " ) );
  write ( "Stufe     : "+quest[4]+"\n" );
  switch ( quest[5] )
  {
    case 0:
      write ( "Pflicht   : optional, zählt zu den "+QP_PERCENT+"%\n" );
      break;
    case 1:
      write ( "Pflicht   : zwingend, zählt zu den "+QP_PERCENT+"%\n" );
      break;
    case 2:
      write ( "Pflicht   : optional, zählt nicht zu den "+QP_PERCENT+"%\n" );
      break;
    default:
      write ( "Pflicht   : unbekannter Status\n" );
  }
  write ( "Magier    : "+capitalize(quest[7])+"\n" );
  return 1;
}

int Query_Keys ( string str )
{
  string *keys, active;
  mixed *quest;
  int i;

  if ( !(keys = QM->QueryAllKeys()) )
    return 1;

  write ( "\n" );
  keys=sort_array(keys,#'>);//')
  for ( i = 0; i < sizeof(keys); i++ )
  {
    quest = QM->QueryQuest(keys[i]);
    if(quest[6])
      active="*";
    else
      active=" ";
    write(sprintf("%2d%s %-33s%s",i+1,quest[6]?"*":" ",keys[i],
	              !(i%2)?" ":"\n"));
  }
  if(i%2)
    write("\n");

  return 1;
}

int Query_Quests ( string str )
{
  mixed *quest;
  string *keys, rstr;
  int i;

  if ( !(keys = QM->QueryAllKeys()) )
    return 1;

  keys=sort_array(keys,#'>);//')
  for ( i = 0, rstr = ""; i < sizeof(keys); i++ )
  {
    quest = QM->QueryQuest(keys[i]);
    rstr += "\nKey       : "+keys[i];
    if(quest[6])
      rstr += " (aktiviert)\n";
    else
      rstr += " (deaktiviert)\n";
    rstr += "Punkte    : "+quest[0]+"\n" +
      "Erfahrung : "+quest[1]+"\n" +
	break_string ( implode( quest[2], " " ), 78, "Filenamen : " ) +
	  break_string ( quest[3], 78, "Info      : " ) +
	    "Stufe     : "+quest[4]+"\n";
    
    switch ( quest[5] )
    {
      case 0:
        rstr += "Pflicht   : optional, zählt zu den "+QP_PERCENT+"%\n";
	break;
      case 1:
	rstr += "Pflicht   : zwingend, zählt zu den "+QP_PERCENT+"%\n";
	break;
      case 2:
	rstr += "Pflicht   : optional, zählt nicht zu den "+QP_PERCENT+"%\n";
	break;
      default:
	rstr += "Pflicht   : unbekannter Status\n";
    }
    rstr += "Magier    : "+capitalize(quest[7])+"\n";
  }
  More( rstr, 0 );
  return 1;
}

int ActivateQuest( string str )
{
  mixed *quest;
  string newstr;
  int errstat;

  if ( !(str=UA) )
  {
    write ( "Syntax: ActiVateQ <quest>\n" );
    return 1;
  }

  Load_NumKey();

  if ( !(newstr = num_key[str+""]) )
	newstr = str;

  switch( QM->SetActive( newstr, 1 ) )
  {
    case -3: write ( "Ungültiger Flag.\n" ); break;
    case -2: write ( "Quest war bereits aktiviert.\n" ); break;
    case -1: write ( "Keine Quest dieses Namens gefunden.\n" ); break;
    case  0: write ( "Zugriff verweigert\n" ); break;
    case  1: write ( "Quest '"+newstr+"' aktiviert.\n" ); break;
    default: write ( "unbekannter Fehlerstatus.\n" );
  }
  return 1;
}

int DeactivateQuest( string str )
{
  mixed *quest;
  string newstr;
  int errstat;

  if ( !(str=UA) )
  {
    write ( "Syntax: DeactiVateQ <quest>\n" );
    return 1;
  }

  Load_NumKey();

  if ( !(newstr = num_key[str+""]) )
	newstr = str;

  switch( QM->SetActive( newstr, 0 ) )
  {
    case -3: write ( "Ungültiger Flag.\n" ); break;
    case -2: write ( "Quest war bereits deaktiviert.\n" ); break;
    case -1: write ( "Keine Quest dieses Namens gefunden.\n" ); break;
    case  0: write ( "Zugriff verweigert\n" ); break;
    case  1: write ( "Quest '"+newstr+"' deaktiviert.\n" ); break;
    default: write ( "unbekannter Fehlerstatus.\n" );
  }
  return 1;
}

