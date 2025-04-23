/*
** mpen.c - Woody@SL
** ---
** beruht auf der Sandtigerkralle von Dancer.
** Anlegen neuer Raeume und Beschreibung selbiger.
** ---
** SL-Version:
**   break_string() fuer AddDetail nicht mehr notwendig,
**   SetIntLong(), Raumtemperatur, Sounds, Smells.
** ---
** 1996-05-01      Prototyp 'Dancehere'
** 1996-09-06      Dancehere -> mpen
** 1996-09-07..14  Linken von Raeumen/mpcr, generelle Ueberarbeitung,
**                 i.e.: make it work :)
** 1996-09-18      Bugfix (P_SHORT -> P_INT_SHORT)
** 1996-09-18      Details von Vorraeumen optional kopieren
** 1996-09-30      SetIntLong() raus, stattdessen wieder SetProp(P_INT_LONG...)
** 1996-11-23      " -> \"
** 1997-02-03      mped, keepnl
** 1997-02-04      nedit auch fuer Details
** 1997-02-16      mpacmd
** 1997-11-19      Exits ohne ROOM 
*
* $Date: 2001/01/13 13:09:45 $
* $Revision: 1.2 $
* $Log: mpen.c,v $
* Revision 1.2  2001/01/13 13:09:45  Samtpfote
* zerstoert sich von selbst, wenn MLevel zu klein
*
* Revision 1.1  2001/01/13 13:09:03  Samtpfote
* Initial revision
*
*/

inherit "std/thing";
inherit "/mail/nedit";
 
#include <properties.h>
#include <defines.h>
#include <wizlevels.h>
#include <ansi.h>

// #define USE_BS

// MAKROS
// ========================================================================

#define bs(x) break_string(x, 78)
#define DEF(x) (x=="\ndefault" ? "Default" : x)

#define VERSION "0.93 (16 Feb 97)"
#define MPHEADER "/* MP-HEADER - DO NOT MODIFY ANYTHING BEFORE THE #include STATEMENTS!!"
#define HEADER_MASK "** %s  [created using mpen v"+VERSION+"]"
#define LASTMOD_MASK "** Letzte Aenderung: %s von %s"
#define STAT_MASK "** details: %d, read det: %d, special det: %d, sounds: %d, smells: %d, exits: %d."
#define NEWDET_MASK "** %s  neue(s) Detail(s) von %s. [mp]"
#define NEWEXIT_MASK "** %s  Ausgang '%s' angeschlossen von %s. [mp]"
#define NEWCMD_MASK "** %s  Kommando '%s' definiert von %s. [mp]"
#define MARK_DESCR    "  // ROOM DESCRIPTION"
#define MARK_ENDDESCR "  // ROOM DESCRIPTION END"
#define MARK_SOD "  // DETAILS START"
#define MARK_EOD "  // DETAILS END"
#define MARK_SOE "  // EXITS START"
#define MARK_EOE "  // EXITS END"
#define MARK_SOI "  // ITEMS START"
#define MARK_EOI "  // ITEMS END"
#define ALL_MARKS ({ MARK_DESCR, MARK_ENDDESCR, MARK_SOD, MARK_EOD, \
                     MARK_SOE, MARK_EOE, MARK_SOI, MARK_EOI });

#define VDATA ([ \
  "mpad": ({ P_DETAILS, "Details", "AddDetail", 0 }), \
  "mpard": ({ P_READ_DETAILS, "Read-Details", "AddReadDetail", 1 }), \
  "mpasd": ({ P_SPECIAL_DETAILS, "Special Details", "AddSpecialDetail", 2 }), \
  "mpasnd": ({ P_SOUNDS, "Sounds", "AddSound", 3 }), \
  "mpasm": ({ P_SMELLS, "Smells", "AddSmell", 4 }) ])
#define V_PROP	0
#define V_WHAT	1
#define V_CMD	2
#define V_STAT_OFFSET	3

#define DES_FNAME	0
#define DES_FROMROOM	1
#define DES_EXIT	2
#define DES_LIGHT	3
#define DES_INDOORS	4
#define DES_TEMP	5
#define DES_SHORT	6
#define DES_LONG	7
#define DES_CPDET	8

#define NONSTDEXIT "'%s' ist keiner der Standardausgaenge. " \
      "Soll er einen eigenen 'Richtungstext' (zB. 'in die Gilde' o.ä.) " \
      "erhalten? Dann gib diesen jetzt bitte ein..."


// GLOBALE VARIABLEN
// ========================================================================

private object here;		// Environment von TP
private string verb;		// mpa*d query_verb
private mapping vdata;		// mpa*d info array
private string actual_file;	// Filename desselben
private string old_file;	// der Inhalt des Files desselben
private string *file_lines;	// wie oben, aber als Array der Zeilen
private mapping det;		// Details des aktuellen Raums
private string *new_file;	// der Inhalt des geaenderten Files
private string own_include;	// eigenes Include-File
private string inherit_from;	// Raum, der inherited werden soll
private string *col;		// Farben
private int use_nedit;          // fuer Langbeschreibung:
                                // 1...nedit verwenden, 0...nur einzeilig
private int keepnl;             // Newlines behalten?
private int mph;		// MP-Header o.k.?
private int hdcounts;		// Zeilenposition der Detail-Counter im Header
private string *d_keys;		// Keys der neuen Details
private string d_str;		// neuer Detailtext
private mapping marks;		// indizes: MARK_*, values: line number
private int counts;		// Anzahl der Details
private mixed *descr;		// Beschreibung des neuen Raums
				// index
				//   0   Filename (mit .c)
				//   1   Raum von dem gelinkt wurde (mit .c)
				//   2   Exit, der in den neuen Raum fuehrt
				//   3   Lichtlevel
				//   4   P_INDOORS
				//   5   Temperatur
				//   6   Kurzbeschreibung
				//   7   Langbeschreibung


// FUNKTIONEN
// ========================================================================

void create()
{
   ::create();
   // SetProp (P_AUTOLOADOBJ, 1);
   SetProp (P_NAME, "Magic Pen");
   SetProp (P_GENDER, MALE);
   SetProp (P_SHORT, "Ein Magic Pen");
   SetProp (P_LONG, bs(
     "Der Magic Pen ist ein magischer Pinsel, ein praktisches Tool, "+
     "mit dem Du Räume, ja sogar ganze Gegenden mit Leichtigkeit "+
     "erschaffen kannst. Weitere Informationen erhältst Du mit dem "+
     "Befehl <mph> oder ausführlicher mit <mphilfe>.") );
   SetProp (P_ARTICLE, 2);
   SetProp (P_WEIGHT, 100);
   SetProp (P_VALUE, 10000);
   SetProp (P_NOBUY, 1);
   SetProp (P_NODROP, 1);
   AddId ( ({"pen", "pinsel"}) );
   AddAdjective ( ({"magic", "magischer", "magischen"}) );
   own_include = 0;
   use_nedit = 0;
   keepnl = 0;
   inherit_from = "std/room";
   vdata = VDATA;
   seteuid(getuid(this_object()));
}

mixed _query_autoloadobj ()
{
  return ({ own_include, inherit_from, use_nedit, keepnl });
}

mixed _set_autoloadobj (mixed settings)
{
  own_include  = settings[0];
  inherit_from = settings[1];
  if (sizeof(settings) > 2)
    use_nedit = settings[2];
  if (sizeof(settings) > 3)
    keepnl = settings[3];
  return settings;
}
 
void init()
{
  string tty;
  if (!IS_WIZARD(this_player())) 
  {
	call_out("remove", 0);
	return;
  }
  ::init();
  add_action ("give_short_info", "mph");
  add_action ("give_info", "mphilfe");
  add_action ("show_status", "mpss");
  add_action ("set_inc", "mpinc");
  add_action ("set_inh", "mpinh");
  add_action ("set_ned", "mped");
  add_action ("set_nl", "mpnl");
  add_action ("create_new_room", "mpcr");
  add_action ("set_add_detail", "mpad");
  add_action ("set_add_detail", "mpard");
  add_action ("set_add_detail", "mpasd");
  add_action ("set_add_detail", "mpasnd");
  add_action ("set_add_detail", "mpasm");
  add_action ("query_add_details", "mpsd");
  add_action ("add_command", "mpacmd");
  col = ({ 0,0,0,0 });
  if ((tty=this_player()->QueryProp(P_TTY)) == "ansi") {
    col[0] = ANSI_NORMAL;
    col[1] = ANSI_BOLD + ANSI_RED;
    col[2] = ANSI_BOLD + ANSI_CYAN;
    col[3] = ANSI_BOLD + ANSI_GREEN;
  }
  else if (tty == "vt100") {
    col[0] = ANSI_NORMAL;
    col[1] = ANSI_BOLD;
    col[2] = ANSI_BOLD;
    col[3] = ANSI_NORMAL;
  }
  else {
    col[0] = col[1] = col[2] = col[3] = ANSI_NORMAL;
  }
}
 
int give_short_info()
{
   write ("-------------------------------------------------------------\n");
   write (col[3]+"Magic Pen"+col[1]+" Befehle:"+col[0]+"\n");
   write (col[2]+"mpss"+col[0]+"                         show settings\n");
   write (col[2]+"mpinc [includefile]"+col[0]+"          eigenes Includefile definieren\n");
   write (col[2]+"mpinh [inheritfile]"+col[0]+"          Inherit-File definieren\n");
   write (col[2]+"mped"+col[0]+"                         Zeileneditor f.Langbeschreibung\n");
   write (col[2]+"mpnl"+col[0]+"                         Zeilenumbrüche übernehmen?\n");
   write (col[2]+"mpcr [<ausgang>] <filename>"+col[0]+"  create - neuen Raum anlegen\n");
   write (col[2]+"mpad [details]"+col[0]+"               add details - AddDetail-Editor\n");
   write (col[2]+"mpard"+col[0]+"                        add read details\n");
   write (col[2]+"mpasd"+col[0]+"                        add special details\n");
   write (col[2]+"mpasnd"+col[0]+"                       add sounds\n");
   write (col[2]+"mpasm"+col[0]+"                        add smells\n");
   write (col[2]+"mpacmd"+col[0]+"                       add commands\n");
   write (col[2]+"mpsd [qp]"+col[0]+"                    show details\n");
   write( "-------------------------------------------------------------\n");
   return 1;
}

int give_info()
{
   write ("-----------------------------------------------------------------------------\n");
   write ("Der "+col[3]+"Magic Pen"+col[0]+" ermöglicht folgende Befehle:\n");
   write ("\n"+col[2]+"mpss"+col[1]+" (show settings)\n"+col[0]);
   write (bs("Zeigt die aktuellen Einstellungen (z.B. Name des "+
     "eigenen include-Files oder Name des inherit Raums) an. "+
     "Die Einstellungen werden übrigens über Reboots hinweg "+
     "gespeichert."));
   write ("\n"+col[2]+"mpinc"+col[1]+" (set include file)\n"+col[0]);
   write (bs("Ermöglicht es, ein eigenes Include-File zu "+
     "definieren, dass beim Anlegen neuer Räume per #include "+
     "eingebunden wird. Ohne Parameter: kein eigenes Include-File."));
   write ("\n"+col[2]+"mpinh"+col[1]+" (set inherit file)\n"+col[0]);
   write (bs("Ermöglicht es, ein eigenes Inherit-File zu "+
     "definieren, von dem neu angelegte Räume inherited werden. "+
     "Ohne Parameter: Setzen von std/room als Default."));
   write ("\n"+col[2]+"mped"+col[1]+" (set editor mode)\n"+col[0]);
   write (bs("Schaltet zwischen der Verwendung des Zeileneditors "+
     "(derselbe der auch fuer Mails etc. verwendet wird) und einer "+
     "einzeiligen Abfrage fuer die Langbeschreibung eines Raumes "+
     "hin und her."));
   write ("\n"+col[2]+"mpnl"+col[1]+" (set newline mode)\n"+col[0]);
   write (bs("Schaltet zwischen der Übernahme von Zeilenumbrüchen (bei "+
     "Verwendung des Zeileneditors) in der Langbeschreibung und dem "+
     "Ignorieren derselben hin und her."));
   write ("\n"+col[2]+"mpcr [<ausgang>] <filename>"+col[1]+" (create)\n"+col[0]);
   write (bs("Ruft einen kleinen Raum-Editor auf, mit dem Du "+
     "einen neuen Raum an den, in dem Du grade stehst, 'anbauen' "+
     "kannst (zum Beispiel in Richtung Norden o.ä.). Außerdem "+
     "legst Du hier Raumbeschreibung, Temperatur und ähnliches "+
     "fest. Wenn der angegebene Raum schon existiert, kann er so "+
     "an den, in dem man gerade steht angeschlossen werden."));
   write ("\n"+col[2]+"mpad [details]"+col[1]+" (add details)\n"+col[0]);
   write (bs("Ruft einen kleinen AddDetail-Editor auf, mit dem Du "+
     "Details in den Raum in dem Du grade stehst einfügen kannst. "+
     "Achtung: Es können nur Details in Räume eingefügt werden, "+
     "die mit dem Magic Pen angelegt worden sind, oder die die hinter "+
     "dem letzten Detail die Ende-Markierung '"+MARK_EOD+"' haben. "+
     "Du musst außerdem die Lupe (/obj/tools/lupe) haben, da der "+
     "Raum nach dem Einfügen des Details automatisch neu "+
     "geladen wird. Die Kürzel der neuen Details können direkt "+
     "nach mpad angegeben werden, durch Leerzeichen getrennt."));
   write ("\n"+col[2]+"mpard"+col[1]+" (add read details)\n"+col[0]);
   write (col[2]+"mpasd"+col[1]+" (add special details)\n"+col[0]);
   write (col[2]+"mpasnd"+col[1]+" (add sounds)\n"+col[0]);
   write (col[2]+"mpasm"+col[1]+" (add smells)\n"+col[0]);
   write (bs("Analog zu mpad können hiermit ReadDetails, Special "+
     "Details, Sounds und Smells definiert werden."));
   write ("\n"+col[2]+"mpacmd"+col[1]+" (add commands)\n"+col[0]);
   write (bs("Du kannst den kompletten Rahmen sowie den passenden "
     "AddCmd() Aufruf für ein Raumkommando generieren lassen."));
   write ("\n"+col[2]+"mpsd [qp]"+col[1]+" (show details)\n"+col[0]);
   write (bs("Zeigt Dir alle Zeilen des angesprochenen Raumes bzw. "+
     "des Raumes in dem Du stehst, die ein AddDetail beinhalten. "+
     "So kannst Du kontrollieren, welche Details Du schon "+
     "eingebaut hast."));
   write ("Optionaler Parameter: <qp>. Wenn angegeben, werden die Details "+
     "abgefragt\n(via QueryProp()) statt aus dem File gelesen.\n");
   write( "-----------------------------------------------------------------------------\n");
   return 1;
}

int show_status()
{
  write (col[3]+"Magic Pen "+col[1]+"Aktuelle Einstellungen:"+col[0]+"\n");
  write (col[2]+"Eigenes Include-File: "+col[0]+(own_include ? own_include : "(keins)")+"\n");
  write (col[2]+"Inherit-Raum:         "+col[0]+inherit_from+"\n");
  write (col[2]+"Zeileneditor:         "+col[0]+(use_nedit?"ja":"nein")+"\n");
  write (col[2]+"Newlines uebernehmen: "+col[0]+(keepnl?"ja":"nein")+"\n");
  return 1;
}

int set_inc(string str)
{
  if (str)
  {
    own_include=this_player()->_unparsed_args();
    write ("Ok, selbstdefinierbares Include-File ist jetzt '"+
      own_include+"'.\n");
  }
  else
  {
    own_include=0;
    write ("Ok, kein selbstdefiniertes Include-File.\n");
  }
  return 1;
}

int set_inh(string str)
{
  if (str)
    inherit_from=MASTER->make_path_absolute(this_player()->_unparsed_args());
  else
    inherit_from="std/room";
  write ("Ok, Inherit-Raum ist jetzt '"+inherit_from+"'.\n");
  return 1;
}

int set_ned()
{
  use_nedit = !use_nedit;
  if (use_nedit)
    write ("Verwende Zeileneditor für Langbeschreibungen.\n");
  else
    write ("Verwende nur einfache Abfragezeile fuer Langbeschreibungen.\n");
  return 1;
}

int set_nl()
{
  keepnl = !keepnl;
  if (keepnl)
    write ("Ok, vorgegebene Newlines werden übernommen.\n");
  else
    write ("Ok, vorgegebene Newlines werden ignoriert.\n");
  return 1;
}

int renew_variables()
{
  here = actual_file = old_file = d_str = counts = mph = hdcounts  = det = 0;
  d_keys = file_lines = descr = ({});
  marks = ([ ]);
  return 1;
}

int ReadFile (string str)  // liest Datei in old_file (string) und
                           // file_lines (string *) ein
{
  int i, i1, i2, i3, i4, i5, i6;
  string *allmarks;

  allmarks=ALL_MARKS;
  old_file = read_file (str);
  file_lines = explode (old_file, "\n");
  hdcounts = -1;
  if (mph=file_lines[0]==MPHEADER)
  {
    for (i=1; i<sizeof(file_lines); i++)
      if (sscanf(file_lines[i], STAT_MASK, i1, i2, i3, i4, i5, i6)==6)
      {
        hdcounts=i;
        break;
      }
  }
  marks=([ ]);
  for (i=1; i<sizeof(file_lines); i++)
  {
    if (file_lines[i][0..3]=="  //")
    {
      for (i1=0; i1<sizeof(allmarks); i1++)
        if (file_lines[i]==allmarks[i1])
          marks[allmarks[i1]]=i;
    }
  }
  return 1;
}

int SearchMark (string str)  // sucht in file_lines nach einer Marke (String)
                             // und gibt den Index bzw -1 (nicht gef.) zurueck
{
  int i, j;
  if (marks[str]) return marks[str];
  j=sizeof(file_lines);
  for (i=0; i<j; i++)
  {
    if (strstr(file_lines[i], str) != -1) return i;
  }
  return -1;
}

int sortdet(string a, string b)
{
  if (stringp(det[a]) && stringp(det[b]))
    return (det[a] > det[b]);
  else
    return (sprintf("%O", det[a]) > sprintf("%O", det[b]));
}

int query_add_details(string str)
{
  int i, qp, counter, startline, endline;

  qp = (str && str == "qp");
  here = environment (this_player());
  actual_file = object_name(here)+".c";
  write( "\n-----------------------------------------------------------------------------\n" );
  write( "Aktuelles File:    "+col[1]+actual_file+col[0]+"\n");
  write( "Filegröße:       "+col[1]+file_size(actual_file)+col[0]+"\n");
  write( "User-ID des Files: "+col[1]+getuid(here)+col[0]+"\n");
  write( "-----------------------------------------------------------------------------\n" );
  ReadFile (actual_file);
  counter = 0;
  startline = 0;
  endline = sizeof(file_lines)-1;
  if (qp)
  {
    det = here->QueryProp(P_DETAILS)
        + here->QueryProp(P_SPECIAL_DETAILS)
        + here->QueryProp(P_READ_DETAILS);
    if (det)
    {
      int n;
      string *ind;
      counter = n = sizeof (det);
      ind = sort_array (m_indices(det), "sortdet");
      write (col[2]);
      for (i = 0; i < n; i++)
      {
        if (mappingp(det[ind[i]]))
          det[ind[i]]="RASSENABHÄNGIG:\n"+det[ind[i]][0];
        if ((i < (n-1)) && (det[ind[i]]==det[ind[i+1]]))
          counter--, write (DEF(ind[i])+", ");
        else
        {
          write (DEF(ind[i]) + "\n");
          if (stringp(det[ind[i]]))
            write (col[3] + det[ind[i]] + col[2]);
          else
            write (col[3] + sprintf("%O\n", det[ind[i]]) + col[2]);
        }
      }
      write (col[0]);
    }
  }
  else {
    for (i = startline; i <= endline; i++)
    {
      /*
      if (sizeof (regexp (({file_lines[i]}),             // reference cost:
          "Add(Smell|Sound|Detail|SpecialDetail|ReadDetail)")))  // 170 ms
      */
      /*
      if (( strstr (file_lines[i], "AddDetail") != -1)   // cost: 150 ms
      */
      if (( strstr (file_lines[i], "Add") != -1)         // cost: 140 ms
        &&((strstr (file_lines[i], "AddDetail") != -1)
        || (strstr (file_lines[i], "AddSpecialDetail") != -1)
        || (strstr (file_lines[i], "AddReadDetail") != -1)
        || (strstr (file_lines[i], "AddSound") != -1)
        || (strstr (file_lines[i], "AddSmell") != -1)))
      {
        counter++;
        write (col[2]+file_lines[i]+"\n"+col[3]);
        // restlichen Zeilen ausgeben, bis zum abschliessenden ");"
        while ((strstr(file_lines[i], ");") == -1) && (i <= endline))
          write(file_lines[++i]+"\n");
      }
    }                                
  }
  write (col[0]);
  write( "-----------------------------------------------------------------------------\n" );
  write( "Details found: "+col[1]+counter+col[0]+"\n" ); 
  write( "-----------------------------------------------------------------------------\n" );
  return 1;
}

int get_detail_short (string str);

int set_add_detail(string str)
{
  int EODmark, i, counter;
  string *ind, *ind2;

  renew_variables();
  verb = query_verb();
  here = environment (this_player());
  actual_file = object_name(here)+".c";
  write ("\n-----------------------------------------------------------------------------\n");
  write ("Aktuelles File:    "+col[1]+actual_file+col[0]+"\n");
  write ("Filegröße:       "+col[1]+file_size(actual_file)+col[0]+"\n");
  write ("User-ID des Files: "+col[1]+getuid(here)+col[0]+"\n");
  write ("-----------------------------------------------------------------------------\n");
  ReadFile (actual_file);
  counter = 0;
  EODmark = marks[MARK_EOD];
  if (!EODmark)
  {
    write (bs("Es wurde im File "+actual_file+" die Details Ende-Marke "+
       "nicht gefunden! Um Details hinzufügen zu können, muss diese "+
       "Marke (eine Zeile mit dem Inhalt '"+MARK_EOD+"') vorhanden sein!" ));
    write ("-----------------------------------------------------------------------------\n");
    return 1;
  }
  // copy file till mark
  new_file = ({ });
  for (i=0; i<EODmark; i++)
  {
    new_file += ({ file_lines [i] });
  }

  det = here->QueryProp(vdata[verb][V_PROP]);
  ind = sort_array (sort_array (m_indices(det), #'>), "sortdet");
  write("Gefundene "+vdata[verb][V_WHAT]+":    "+col[1]+sizeof(det)+col[0]+"\n");
  if (sizeof(det))
  {
    int n,j;
    n=sizeof(ind);
    ind2 = ({ });
    for (i=j=0; i<n; i++)
    {
      if ((i>0) && (typeof(det[ind[i]])==typeof(det[ind[i-1]])
          && (det[ind[i]])==det[ind[i-1]]))
        ind2[j-1] += " = "+ind[i];
      else
        ind2 += ({ ind[i] }), j++;
    }
    ind2=sort_array (ind2, #'>);
    write (break_string ("und zwar: "+col[2]+implode(ind2, ", ")+col[0], 78));
  }
  write ("Langbeschreibung des Raumes:\n");
  // write (col[3] + break_string (here->QueryProp(P_INT_LONG), 78) + col[0]);
  write (col[3] + here->QueryProp(P_INT_LONG) + col[0]);
  if (str) return get_detail_short(str), 1;
  write ("-----------------------------------------------------------------------------\n");
  write ("Bitte die Detail-Kürzel eingeben (mehrere durch Leerzeichen trennen):\n"+
         (verb=="mpasnd" || verb=="mpasm" ?
           "Kürzel für Default-Smell/Sound: 'default'\n" :
           "")+
         "Kürzel zu existierendem hinzufügen: '*kürzel [...]'.  Abbruch: 'q'.\n->");
  input_to ("get_detail_short");
  return 1;
}

int get_detail_short (string str)
{
  string s;
  if (str == "q")
  {
    write ("Ok, abgebrochen!\n");
    return 1;
  }
  write ("-----------------------------------------------------------------------------\n");
  if (str[0] == '*')
  {
    str=str[1..];
    d_keys = map (explode (str, " "), #'lower_case);
    s = implode (sort_array (d_keys, #'>), ", ");
    write (
      break_string("Ok, zu welchem Detail soll/sollen \""+col[2]+s+col[0]+
      "\" als Kürzel hinzugefügt werden? (Abbruch mit 'q')", 78)+"->");
    write ("\n  <<< NOCH NICHT IMPLEMENTIERT >>>\n");
    return 1;
    input_to ("get_detail_key");
    return 1;
  }
  d_keys = map (explode (str, " "), #'lower_case);
  s = break_string ("\""+col[2]+
    implode (d_keys=
      sort_array (map (d_keys, #'lower_case), #'>), ", ")+
    col[0]+"\": (Abbruch mit 'q')", 78);
  write (bs("Bitte Beschreibung eingeben für "+vdata[verb][V_WHAT]+" mit dem/den Namen"));
  write (s + (use_nedit ? "" : " -> "));
  if (use_nedit)
    nedit ("get_detail_long");
  else
    input_to ("get_detail_long");
  return 1;
}
 
int get_detail_long (string str)
{
  int i;
  string s, s2, *temp, *newstr;

  if (!str || (str=="q") || (str==""))
  {
    write ("Ok, abgebrochen!\n");
    write ("-----------------------------------------------------------------------------\n");
    return 1;
  }

  s2 = "  "+vdata[verb][V_CMD]+" ( ({";
  for (i=0; i<sizeof(d_keys); i++)
  {
    if (!(i%3) && i) s2+="\n                ";
    if (verb=="mpasnd" && d_keys[i]=="default")
      s2 += "DEFAULT_SOUND";
    else if (verb=="mpasm" && d_keys[i]=="default")
      s2 += "DEFAULT_SMELL";
    else
      s2 += "\"" + d_keys[i] + "\"";
    if (i < sizeof(d_keys)-1) s2 += ", ";
  }
  s2 += "}),\n";

  str = implode (explode("-"+str+"-", "\\"), "\\\\")[1..<2];	// \ -> \\
  str = implode (explode("-"+str+"-", "\""), "\\\"")[1..<2];	// " -> \"
  if (use_nedit)
    if (keepnl)
      str = implode (explode(str, "\n"), "\\n\n") + "\\n";
    else
      str = implode (explode(str, "\n"), " ");

  if (use_nedit && keepnl)
  {
    temp = explode (str, "\\n");
#ifdef USE_BS
    temp[0] = "break_string (\""+temp[0];
#else
    temp[0] = "\""+temp[0];
#endif
    newstr = ({});
    for (i=0; i<sizeof(temp); i++)
    {
      s = break_string( temp[i]+"\\n", 70);
      newstr += explode (s, "\n");
    }
  }
  else
  {
#ifdef USE_BS
    s = break_string( "break_string (\""+str, 70 );
                      // 0, (use_nedit && keepnl ? 7 : 0));
#else
    s = break_string("\""+str, 70);  // , 0, (use_nedit && keepnl ? 7 : 0));
#endif
    newstr = explode (s, "\n");
  }
  i = 0;
  for (i=0; i<sizeof(newstr); i++)
  {
    if (i < sizeof(newstr)-1)
    {
      if (newstr[i][<2..]=="\\n")  // nach Literal \n kein Space nach
        newstr[i]+="\"+\n";
      else                         // sonst schon, weil das bs() oben
        newstr[i]+=" \"+\n";       // geschluckt hat...
      if (i) s2 += "    \"" + newstr[i];
      else   s2 += "    "   + newstr[i];
    }
    else
    {
#ifdef USE_BS
      if (i) s2 += "    \""+ newstr[i]+"\", 78));";
      else   s2 += "    "  + newstr[i]+"\", 78));";
#else
      if (i) s2 += "    \""+ newstr[i]+"\");";
      else   s2 += "    "  + newstr[i]+"\");";
#endif
    }
  }
  new_file += ({ s2 });
  for (i=marks[MARK_EOD]; i<sizeof(file_lines); i++)
    new_file += ({ file_lines[i] });
  write ("-----------------------------------------------------------------------------\n" );
  write (bs(
    "Folgende Zeilen werden in die Datei "+actual_file+" geschrieben, "+
    "falls `j` eingegeben wird:"));
  write ("\n"+s2+"\n\n");
  write (vdata[verb][V_WHAT]+" speichern? (j/n)\n->");
  input_to ("save_detail");
  return 1;
}

int yes (string str)
{
  if (!str || !stringp(str)) return 0;
  str = lower_case (str);
  return str=="j" || str=="ja";
}

int save_detail (string str)
{
  int i;
  string hist;
  if (!yes(str))
  {
    write ("Ok, abgebrochen!\n");
    write ("-----------------------------------------------------------------------------\n");
    return 1;
  }
  if (!rm (actual_file))
  {
    write( "Datei "+actual_file+" wurde nicht gefunden!\n" );
    return 1;
  }
  for (i=0; i<sizeof(new_file); i++)
  {
    if (mph && (i==2))
      write_file (actual_file, sprintf (LASTMOD_MASK,
            dtime(time()), capitalize(getuid(this_interactive())))+"\n");
    else if (mph && (i==hdcounts-1))   // History up-to-date halten
    {
      hist = sprintf (NEWDET_MASK,
        dtime(time())[5..16], capitalize(getuid(this_interactive())));
      if (hist != new_file[i-1]) write_file (actual_file, hist+"\n");
      write_file (actual_file, new_file[i]+"\n");
    }
    else if (mph && (i==hdcounts))     // Statistik-Zeile updaten
    {
      int *cnt;
      cnt = ({ 0, 0, 0, 0, 0, 0 });
      sscanf (file_lines[i], STAT_MASK,
        cnt[0], cnt[1], cnt[2], cnt[3], cnt[4], cnt[5]);
      cnt[vdata[verb][V_STAT_OFFSET]] += sizeof (d_keys);
      write_file (actual_file, sprintf (STAT_MASK,
        cnt[0], cnt[1], cnt[2], cnt[3], cnt[4], cnt[5])+"\n");
    }
    else write_file (actual_file, new_file[i]+"\n");
  }
  write ("-----------------------------------------------------------------------------\n");
  write ("Ok, Datei "+actual_file+" aktualisiert.\n");
  this_player()->command_me ("here make");
  return 1;
}

void link_room (string fromroom, string exit, string direction, string fname);
void create_skeleton ();

string gegenrichtung (string str)
{
  string s;
  if (!str) return 0;
  switch (str)
  {
    case "oben": return "unten";
    case "unten": return "oben";
    case "rauf": return "runter";
    case "runter": return "rauf";
    case "norden": return "sueden";
    case "sueden": return "norden";
    case "osten": return "westen";
    case "westen": return "osten";
    case "nordwesten": return "südosten";
    case "nordosten": return "südwesten";
    case "südwesten": return "nordosten";
    case "südosten": return "nordwesten";
    default:
      if (sscanf(str, "%soben", s))
      {
        if ((s=gegenrichtung(s+"en")) != "raus") return s[0..<3]+"unten";
        else return "raus";
      }
      else if (sscanf(str, "%sunten", s))
      {
        if ((s=gegenrichtung(s+"en")) != "raus") return s[0..<3]+"oben";
        else return "raus";
      }
      else return "raus";
  }
}

void create_new_room(string str)
{
  string exit, fname;
  int n;
  notify_fail ("Syntax: mpcr [<ausgang>] <filename>\n");
  if (!str) return 0;
  str=this_player()->_unparsed_args();
  n = sscanf (str, "%s %s", exit, fname);
  if (n>2) return 0;
  if (n<2) {
    exit=0;
    fname=str;
  }
  fname=MASTER->make_path_absolute(fname);
  if (file_size(fname)==-2) return
    write ("Es existiert bereits ein Verzeichnis mit dem Namen '"+
    fname+"'!\n"), 1;
  if (fname[<2..]!=".c") fname+=".c";

  here = environment (this_player());
  actual_file = object_name(here)+".c";
  ReadFile (actual_file);
  if (file_size(fname)>=1)
  {
    write ("Der Raum "+fname+" existiert bereits!\n");
    if (exit)
    {
      write ("Als Ausgang '"+exit+"' anschließen? (j/n) -> ");
      input_to ("ask_link_room", 0, actual_file, exit, 0, fname, 0);
    }
    return 1;
  }
  write ("-----------------------------------------------------------------------------\n");
  write ("Neuer Raum ["+fname+"]\n");
  if (exit)
    write ("Erreichbar von hier ["+actual_file+"] via '"+exit+"'.\n");
  write ("-----------------------------------------------------------------------------\n");
  descr = ({ fname, actual_file, exit, 1, 0, 20,
             "Neuer Raum", "Du stehst in einem unbeschriebenen Raum.", 0 });
  if (exit)
  {
    descr[DES_LIGHT] = here->QueryProp(P_LIGHT);
    descr[DES_INDOORS] = here->QueryProp(P_INDOORS);
    descr[DES_TEMP] = here->QueryProp(P_ENVIRONMENT)[ENV_TEMPERATURE];
  }
  write (bs("Du musst jetzt den Raum ein wenig beschreiben, indem Du "+
    "einige Fragen beantwortest. Du kannst jederzeit durch Eingabe von "+
    "'q' abbrechen."));
  write ("Lichtlevel (default "+descr[DES_LIGHT]+"): ");
  input_to ("get_light");
  return 1;
}

int get_light (string str)
{
  if (str == "q")
  {
    write( "Ok, abgebrochen!\n" );
    return 1;
  }
  if (str && str!="") descr[DES_LIGHT] = to_int(str);
  write ("Ok, Lichtlevel: "+descr[DES_LIGHT]+"\n");
  write ("Ist der Raum drinnen (1"+(descr[DES_INDOORS] ? ", default" : "")+
    ") oder draußen (0"+(descr[DES_INDOORS] ? "" : ", default")+"): ");
  input_to ("get_indoors");
  return 1;
}

int get_indoors (string str)
{
  if (str == "q")
  {
    write( "Ok, abgebrochen!\n" );
    return 1;
  }
  if (str && str!="") descr[DES_INDOORS] = (to_int(str) ? 1 : 0 );
  write ("Ok, Raum ist "+(descr[DES_INDOORS] ? "drinnen" : "draußen")+".\n");
  write ("Temperatur des Raumes (default: "+descr[DES_TEMP]+"): ");
  input_to ("get_temp");
  return 1;
}

int get_temp (string str)
{
  if (str == "q")
  {
    write( "Ok, abgebrochen!\n" );
    return 1;
  }
  if (str && str!="") descr[DES_TEMP] = to_int(str);
  write ("Ok, Temperatur: "+descr[DES_TEMP]+" Grad.\n");
  write ("Kurzbeschreibung des Raumes: ");
  input_to ("get_short");
  return 1;
}

int get_short (string str)
{
  if (str == "q")
  {
    write( "Ok, abgebrochen!\n" );
    return 1;
  }
  if (str && str!="") {
    str = implode (explode("-"+str+"-", "\\"), "\\\\")[1..<2];	// \ -> \\
    str = implode (explode("-"+str+"-", "\""), "\\\"")[1..<2];	// " -> \"
    descr[DES_SHORT] = str;
  }
  write ("Ok, Kurzbeschreibung '"+descr[DES_SHORT]+"'.\n");
  write ("Langbeschreibung des Raumes:\n"+(use_nedit ? "" : " -> "));
  if (use_nedit)
    nedit ("get_long");
  else
    input_to ("get_long");
  return 1;
}

int get_direction (string str);

int get_copydetails(string str);

int get_long (string str)
{
  if (!str || (str == "q") || (str == ""))
  {
    write( "Ok, abgebrochen!\n" );
    return 1;
  }
  str = implode (explode("-"+str+"-", "\\"), "\\\\")[1..<2];	// \ -> \\
  str = implode (explode("-"+str+"-", "\""), "\\\"")[1..<2];	// " -> \"
  if (use_nedit)
    if (keepnl)
      str = implode (explode(str, "\n"), "\\n") + "\\n";
    else
      str = implode (explode(str, "\n"), " ");
  descr[DES_LONG] = str;
  if (mappingp(marks) &&
      (member(marks, MARK_SOD) != -1) && (member(marks, MARK_EOD) != -1))
  {
    write ("Details, Smells, Sounds, etc. von diesem Raum kopieren? (j/n) -> ");
    input_to ("get_copydetails");
  }
  else return get_copydetails(0);
}

int get_copydetails(string str)
{
  if (str == "q")
  {
    write( "Ok, abgebrochen!\n" );
    return 1;
  }
  descr[DES_CPDET] = str && ((str=="j") || (str=="ja"));
  if (gegenrichtung(descr[DES_EXIT])=="raus")
  {
    write (bs(
      "'"+descr[DES_EXIT]+"' ist keiner der Standardausgänge. "+
      "Soll er einen eigenen 'Richtungstext' (zB. 'in die Gilde' o.ä.) "+
      "erhalten? Dann gib diesen jetzt bitte ein...")+" -> ");
    input_to ("get_direction");
  }
  else get_direction (0);
  return 1;
}

int get_direction (string str)
{
  string s;
#ifdef DEBUG
  write (sprintf("\n%O\n\n", descr));
#endif
  if (str && str!="") s=str; else s=0;
  if (create_skeleton ())
  {
    write ("-----------------------------------------------------------------------------\n");
    write ("Ok, Datei "+descr[DES_FNAME]+" angelegt.\n");
    if (descr[DES_EXIT])
      link_room (descr[DES_FROMROOM], descr[DES_EXIT], s, descr[DES_FNAME]);
  }
  return 1;
}

string getpath (string file)
{
  string path;
  if (!file) return 0;
  if ((path="/"+implode(explode(file, "/")[0..<2], "/")+"/")=="//") path="/";
  return path;
}

string strippath (string file)
{
  if (!file) return 0;
  return explode(file, "/")[<1];
}

void ask_link_room2 (string str, string fromroom, string exit,
                     string direction, string fname, int flag);

void ask_link_room (string str, string fromroom, string exit,
                    string direction, string fname, int flag)
{
  if (!yes(str))
  {
    write ("Ok, abgebrochen!\n");
    return;
  }
  if (gegenrichtung(exit)=="raus")
  {
    write (bs (sprintf (NONSTDEXIT, exit))+" -> ");
    input_to ("ask_link_room2", 0, fromroom, exit, direction, fname, flag);
  }
  else
    ask_link_room2 (0, fromroom, exit, direction, fname, flag);
}

void ask_link_room2 (string str, string fromroom, string exit,
                     string direction, string fname, int flag)
{
  string retour;
  if (str=="q")
  {
    write ("Ok, abgebrochen!\n");
    return;
  }
  if (str && str!="")
    link_room (fromroom, exit, str, fname);
  else
    link_room (fromroom, exit, direction, fname);
  if (!flag)
  {
    retour = gegenrichtung(exit);
    write ("-----------------------------------------------------------------------------\n");
    write (sprintf("Ausgang von %s nach %s\n", fname, fromroom));
    write ("Als Ausgang '"+retour+"' anschließen? (j/n) -> ");
    input_to ("ask_link_room", 0, fname, retour, 0, fromroom, 1);
  }
}

void link_room (string fromroom, string exit, string direction, string fname)
{
  int EOEmark, i;
  string s, hist;
  actual_file=fromroom;
  ReadFile (fromroom);
  if (!(EOEmark=marks[MARK_EOE]))
  {
    write ("-----------------------------------------------------------------------------\n");
    write (bs("Es wurde im File "+fromroom+" die Exits Ende-Marke "+
       "nicht gefunden! Um Exits hinzufügen zu können, muss diese "+
       "Marke (eine Zeile mit dem Inhalt '"+MARK_EOE+"') vorhanden sein!"));
    write ("-----------------------------------------------------------------------------\n");
    return;
  }

  s = "  AddExit (\""+exit+"\", "+(direction ? "\""+direction+"#" : "")+
       (getpath(fromroom)==getpath(fname) ?
         (direction ? "\"+" : "")+
         "\"./"+strippath(fname)[0..<3]+"\"" :
         (direction ? "" : "\"")+
         fname[0..<3]+"\"")+
     ");\n";

  write ("-----------------------------------------------------------------------------\n");
  write ("Folgende Zeile wird in '"+actual_file+"' eingefügt:\n"+s);
  if (!rm (fromroom))
  {
    write( "Datei "+fromroom+" wurde nicht gefunden!\n" );
    return;
  }
  for (i=0; i<sizeof(file_lines); i++)
  {
    if (mph && (i==2))
      write_file (actual_file, sprintf (LASTMOD_MASK,
            dtime(time()), capitalize(getuid(this_interactive())))+"\n");
    else if (mph && (i==(hdcounts-2)))
    {
      write_file (actual_file, file_lines[i]+"\n");
      hist = sprintf (NEWEXIT_MASK, dtime(time())[5..16], exit,
        capitalize(getuid(this_interactive())));
      write_file (actual_file, hist+"\n");
    }
    else if (mph && (i==hdcounts))
    {
      int i1, i2, i3, i4, i5, i6;
      sscanf (file_lines[i], STAT_MASK, i1, i2, i3, i4, i5, i6);
      write_file (actual_file, sprintf (STAT_MASK,
            i1, i2, i3, i4, i5, i6+1)+"\n");
    }
    else
    {
      if (i==EOEmark)
        write_file (actual_file, s);
      write_file (actual_file, file_lines[i]+"\n");
    }
  }
  write ("-----------------------------------------------------------------------------\n");
  write ("Ok, Datei "+actual_file+" aktualisiert.\n");
  this_player()->command_me ("ob "+fromroom[0..<3]+" make");
}

string splitlines(string str)
// liefert zB. '    "zeile1"+\n    "zeile2"+\n    "letztezeile"' zurueck.
{
  string s, *lin;
  int i;
  lin=explode(break_string (str, 68), "\n");
  s="";
  for (i=0; i<=sizeof(lin)-2; i++)
    s+="    \""+lin[i]+" \"+\n";
  s+="    \""+lin[<1]+"\"";
  return s;
}

void create_skeleton ()
// create_skeleton (descr[DES_FNAME], descr[DES_FROMROOM]
// (string str, string fromroom, string exit)
{
  string pfn, pfr, pi, s, sdet;
  int i;
  pfn=getpath (descr[DES_FNAME]);
  pfr=getpath (descr[DES_FROMROOM]);
  pi=getpath (inherit_from);
  s=gegenrichtung(descr[DES_EXIT]);
  sdet="";
  if (descr[DES_CPDET])
    for (i=marks[MARK_SOD]+1; i<marks[MARK_EOD]; i++)
    {
      sdet += file_lines[i]+"\n";
    }
  return write_file (descr[DES_FNAME],
   MPHEADER+"\n"+
   sprintf (HEADER_MASK, strippath(descr[DES_FNAME]))+"\n"+
   sprintf (LASTMOD_MASK,
            dtime(time()), capitalize(getuid(this_interactive())))+"\n"+
   "** ---\n"+
   "** "+dtime(time())[5..16]+"  Raum von "+
   capitalize(getuid(this_interactive()))+" angelegt. [mp]\n"+
   "** ---\n"+
   sprintf(STAT_MASK, 0, 0, 0, 0, 0, (s ? 1 : 0))+"\n"+
   "*/\n\n"+
   "#include <properties.h>\n"+
   "#include <moving.h>\n"+
   (own_include ? "#include \""+own_include+"\"\n" : "")+
   "\n#ifndef ROOM\n#define ROOM(x) (\""+pfn+"\"+x)\n#endif\n\n"+
   "inherit "+
     (pfn==pi ?
     "ROOM(\""+strippath(inherit_from)+"\")" :
     "\""+inherit_from+"\"")+
   ";\n\n"+
   "void create()\n{\n"+
   "  ::create();\n"+
   MARK_DESCR+"\n"+
   "  SetProp (P_LIGHT, "+descr[DES_LIGHT]+");\n"+
   "  SetProp (P_INDOORS, "+descr[DES_INDOORS]+");\n"+
   "  SetProp (P_ENVIRONMENT, ([ENV_TEMPERATURE:"+descr[DES_TEMP]+"]));\n"+
   "  SetProp (P_INT_SHORT, \""+descr[DES_SHORT]+"\");\n"+
   "  SetProp (P_INT_LONG,"+
       (sizeof(descr[DES_LONG]) > 52 ?
       "\n"+splitlines(descr[DES_LONG]) :
       " \""+descr[DES_LONG]+"\"")+
     ");\n"+
   MARK_ENDDESCR+"\n\n"+
   MARK_SOD+"\n"+
   sdet+
   MARK_EOD+"\n\n"+
   MARK_SOE+"\n"+
   (s ?
     "  AddExit (\""+s+"\", "+
       (pfn==pfr ?
         "\"./"+strippath(descr[DES_FROMROOM])[0..<3]+"\"" :
         "\""+descr[DES_FROMROOM][0..<3]+"\"")+
     ");\n" :
     "") +
   MARK_EOE+"\n\n"+
   MARK_SOI+"\n"+
   MARK_EOI+"\n\n"+
   "}\n");
}

int add_command (string str)
{
  int EOImark, i, counter;
  string *cmds, funcname;

  if (!str)
  {
    write (
      "Syntax: mpacmd [funcname:] cmd1 cmd2...\n"
      "funcname: Name der Funktion; wenn nicht angegeben: gleich cmd1\n"
      "cmd1: Das Kommando, das die Funktion auslöst. Mehrere das gleiche\n"
      "      bewirkende Kommandos sind durch Leerzeichen zu trennen.\n");
    return 1;
  }
  str=this_player()->_unparsed_args();
  if ((i=strstr(str, ":")) != -1) {
    funcname=implode(explode(str[0..i-1], " "), "");
    cmds=explode(implode(explode(str[i+1..], " "), " "), " ");
  } else {
    cmds=explode(implode(explode(str, " "), " "), " ");
    funcname=cmds[0];
  }
  renew_variables();
  here = environment (this_player());
  actual_file = object_name(here)+".c";
  write ("\n-----------------------------------------------------------------------------\n");
  write ("Aktuelles File:    "+col[1]+actual_file+col[0]+"\n");
  write ("Filegröße:       "+col[1]+file_size(actual_file)+col[0]+"\n");
  write ("User-ID des Files: "+col[1]+getuid(here)+col[0]+"\n");
  write ("-----------------------------------------------------------------------------\n");
  ReadFile (actual_file);
  counter = 0;
  EOImark = marks[MARK_EOI];
  if (!EOImark)
  {
    write (bs("Es wurde im File "+actual_file+" die Items Ende-Marke "+
       "nicht gefunden! Um Items hinzufügen zu können, muss diese "+
       "Marke (eine Zeile mit dem Inhalt '"+MARK_EOI+"') vorhanden sein!" ));
    write ("-----------------------------------------------------------------------------\n");
    return 1;
  }
  // copy file till mark
  new_file = ({ });
  if (file_lines[EOImark+1]=="") EOImark++;
  for (i=0; i<=EOImark; i++)
  {
    new_file += ({ file_lines [i] });
  }

  new_file += ({ "  AddCmd ( ({\""+implode(cmds, "\", \"")+"\"}), \""+
                 funcname+"\");" });

  for (i=EOImark+1; i<sizeof(file_lines); i++)
  {
    new_file += ({ file_lines [i] });
  }
  new_file += ({
    "\n"
    "int "+funcname+" (string str)\n"
    "{\n"
    "  notify_fail (\"Was willst Du machen?\\n\");\n"
    "  if (!str) return 0;\n"
    "  // ...\n"
    "  return 1;\n"
    "}"
  });
  write ("Füge folgende Zeilen in "+actual_file+" ein:\n\n");
  write (new_file[EOImark+1]+"\n");
  write (new_file[<1]+"\n");
  write ("\nOk? (j/n) -> ");
  input_to ("writeok", 0, cmds[0]);
  return 1;
}

int writeok(string str, string cmd)
{
  int i;
  string hist;
  if (!str || ((str!="j") && (str!="ja"))) {
    write ("Ok, abgebrochen!\n");
    return 1;
  }
  if (!rm (actual_file))
  {
    write( "Datei "+actual_file+" wurde nicht gefunden!\n" );
    return 1;
  }
  for (i=0; i<sizeof(new_file); i++)
  {
    if (mph && (i==2))
      write_file (actual_file, sprintf (LASTMOD_MASK,
            dtime(time()), capitalize(getuid(this_interactive())))+"\n");
    else if (mph && (i==hdcounts-1))   // History up-to-date halten
    {
      hist = sprintf (NEWCMD_MASK,
        dtime(time())[5..16], cmd, capitalize(getuid(this_interactive())));
      if (hist != new_file[i-1]) write_file (actual_file, hist+"\n");
      write_file (actual_file, new_file[i]+"\n");
    }
    else write_file (actual_file, new_file[i]+"\n");
  }
  write ("-----------------------------------------------------------------------------\n");
  write ("Ok, Datei "+actual_file+" aktualisiert.\n");
  this_player()->command_me ("here make");
  return 1;
}

