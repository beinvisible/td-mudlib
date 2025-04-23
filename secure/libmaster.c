/* Tamedhon MUDlib
 *
 * /obj/libmaster.c
 * ---
 * Verwaltung der Info Libaries fuer NPCs
 * ---
 *
 * May 03, 2004  Serii  nun hat der libmaster wirklich immer leserechte
 * Jan 17, 2004  Serii  'race:0' fuer alle anderen Rassen (siehe AddDetail()).
 *                      Nur notwendig, wenn fuer diese Frage rassenspezifische
 *                      Infos angegeben wurden
 * Jan 12, 2004  Serii  Unterstuetzung von Rassenabhaengigen Infos
 *
 * Feb 12, 1997  Woody  Initial Revision
 * Feb 19, 1997  Woody  NewLib() fuer localflag vorbereitet (std/npc/info.c)
 * Feb 21, 1997  Woody  keepnl Verhalten gefixt
 * Sep 01, 1997  Woody  um silent erweitert
 *
 * ------------------------------------------------------------------------
 *
 * Library-Konzept:
 *
 * Ein NPC kann per AddLibrary() (in std/npc/info.c implementiert) eine
 * Bibliothek zu seiner Wissensbasis hinzufuegen. Wird er nach einem
 * Schluesselwort befragt, dass NICHT von einer AddInfo() Information
 * abgedeckt ist, so wendet er sich mit einer Anfrage (oder mehreren,
 * wenn er mehrere Libraries in seiner Wissensbasis referenziert hat) an
 * den libmaster, genauer an GetInfo(), der die Bibliotheken bei Bedarf
 * laedt und dann im Speicher verwaltet. Als Parameter bekommt GetInfo
 * den Informationslevel, den der NPC ueber die entsprechende Bibliothek
 * besitzt, sowie das Spielerobjekt des fragenden Spielers (ermoeglicht
 * spaeter mal weitergehende Parametrisierung, etwa align-abhaengige
 * Auskunft). Findet der libmaster eine Information, die den uebergebenen
 * Parametern genuegt, wird diese als Array retourniert, ansonsten wird
 * 0 zurueckgegeben.
 *
 * Die eigentlichen Informationen (Bibliotheken) sind in einfachen
 * Textdateien gespeichert, die das folgende Format besitzen muessen:
 *
 * # Kommentar
 * info:<level>:<key1> [,<key2> [,<key3>...]]
 * indent: <"indentstring">
 * silent: <"silentstring">
 * race: rasse1, rasse2...
 * options: [ keepnl ]
 * text...
 *
 * Das heisst: Eine neue Information wird durch das Schluesselwort
 * "info:" gefolgt vom Informationslevel (eine Masszahl fuer die Qualitaet
 * der Information) und nach einem weiteren Doppelpunkt einer durch Kommata
 * getrennten Liste von Schluesseln eingeleitet. Es sollten *keine*
 * unnoetigen Leerzeichen verwendet werden.
 * Mit "indent:" kann noch ein Einrueckstring festgelegt werden (selbe
 * Bedeutung wie bei AddInfo()), entweder in doppelten Hochkomma, oder
 * einfach nur als Text (in dem Fall wird automatisch ein Leerzeichen
 * angehaengt).
 * Mit "silent:" kann noch ein Alternativtext festgelegt werden (selbe
 * Bedeutung wie bei AddInfo()), entweder in doppelten Hochkomma, oder
 * einfach nur als Text (in dem Fall wird automatisch ein Leerzeichen
 * angehaengt). Oder 0 oder 1. Siehe AddInfo() Dokumentation.
 * Mit "options:" kann man diverse Optionen, durch Kommata getrennt,
 * festlegen. Im Moment gibt es nur die Option 'keepnl', d.h. die
 * im Text enthaltenen Zeilenumbrueche werden beibehalten.
 * Zuletzt folgt der Text (beliebig viele Zeilen), bis eine "info:"
 * Zeile die naechste Information einleitet.
 * Saemtliche Zeilen, die mit '#' beginnen, werden ignoriert.
 *
 * Bei "race:" kann man die Spielerrasse (z.b. "race: daemon") angeben,
 * fuer die die Information bestimmt ist. (siehe AddDetail())
 * ------------------------------------------------------------------------
 *
 */
#define DEBUGMODE

#include <properties.h>
#include <daemon.h>
#include <wizlevels.h>

#ifdef DEBUGMODE
#define DB(x)	if(find_player("debugger")) \
    { tell_object(find_player("debugger"), break_string(x,79,"[LM]")); }
#else
#define DB(x)	
#endif

mapping infos;
int hits, misses, errors;

/*
 * Format des Mappings infos:
 *  key: "library"
 *  val0: mapping data:
 *        key:  string "level:key"
 *        val0: data string
 *        val1: indent string
 *        val2: silent string
 *
 */

#ifdef DEBUGMODE
void Dump()
{
    write_file("/log/ARCH/libmaster.dump", 
	sprintf("%s\n\n%O\n#### END OF DUMP ####",
	    dtime(time()),infos));
}
#endif

void reset()
{
	write_file("/log/statistik/libmaster",
		sprintf("%s %i %i %i %i\n",ctime(time()),
			sizeof(m_indices(infos)), hits, misses, errors
	));
}

void create() {
  seteuid(getuid());
  infos = ([]);
  hits = misses = errors = 0;
}

int remove(){return 0;}
int clean_up(int ref){return 0;}

varargs int NewLib(string file, int localflag, object npc, int level)
{
  int i, j, skip, lev, keepnl;
  string str, keys, d1, d2;
  string *s, *opt;
  mixed indent;
  mixed silent;
  mapping data;
  mixed racea;
  mapping rmap;

  DB(sprintf("##### loading NEW library #####"));
  DB(sprintf(" file  : %s", file));
  DB(sprintf(" loader: %O", previous_object()));


  skip = 1;
  if (!str=read_file(file)) 
  {
    DB("ERROR COULD NOT LOAD OR NO ACCESS.");
    return 0;
  }
  s = explode (str, "\n");
  data = m_allocate(1,3);

  for (i=0; i<sizeof(s); i++)
  {
    if ((s[i]=="") || (s[i][0]=='#')) continue;
    if (s[i][0..4]=="info:") {
      if (!skip) {
        opt = explode (keys, ",");
    if(keepnl)str=str[0..<2];
    rmap=0;
    if(sizeof(racea))
    {
      rmap=([]);
      for(j=0;j<sizeof(racea);j++)
        rmap[racea[j]]=str;
    }					
        if (localflag)
          npc->AddInfo(opt,sizeof(rmap)?rmap:str,indent,silent);
        else
          for (j=0; j<sizeof(opt); j++)
          {
            if(mappingp(data[lev+":"+opt[j]])&&mappingp(data[lev+":"+opt[j],0]))
	    data[lev+":"+opt[j],0]+=rmap;else
            data+=([lev+":"+opt[j]:sizeof(rmap)?rmap:str;indent;silent]);}
      }
      sscanf(s[i], "info:%d:%s", lev, keys);
      if (localflag && (lev > level))
        skip=1;
      else
        skip=0;
      keepnl=0;
      str="";
      indent="sagt: ";
      silent=0;
      racea=({});
    }
    else if (skip)
      continue;
    else if(s[i][0..4]=="race:")
    {
      sscanf(s[i],"race:%s",racea);
      racea=explode(racea,",");
      if(member(racea,"0")>=0)racea[member(racea,"0")]=0;
    }
    else if (s[i][0..6]=="indent:") {
      if (sscanf(s[i], "indent:%s\"%s\"%s", d1, indent, d2) != 3) {
        indent=implode(explode(s[i][7..], " "), " ");
        if (indent=="0") indent=0; else indent += " ";
      }
    }
    else if (s[i][0..6]=="silent:") {
      if (sscanf(s[i], "silent:%s\"%s\"%s", d1, silent, d2) != 3) {
        silent=implode(explode(s[i][7..], " "), " ");
        if (silent=="0") silent=0;
        else if (silent=="1") silent=1; else silent += "\n";
      }
      else silent=implode(efun::explode(silent, "\\n"), "\n");
    }
    else if (s[i][0..7]=="options:") {
      opt = explode(implode(explode(s[i][8..]," "), ""), ",");
      if (member(opt, "keepnl") != -1) keepnl=1;
    }
    else {
      str += implode(efun::explode(s[i], "\\n"), "\n");
      if ((str[<1]!='\n') && (str[<1]!=' ')) str += (keepnl ? "\n" : " ");
    }
  }
  if (!skip) {
    opt = explode (keys, ",");
    if(keepnl)str=str[0..<2];
    rmap=0;
    if(sizeof(racea))
    {
      rmap=([]);
      for(j=0;j<sizeof(racea);j++)
        rmap[racea[j]]=str;
    }					
    if (localflag)
          npc->AddInfo(opt,sizeof(rmap)?rmap:str,indent,silent);
    else
      for (j=0; j<sizeof(opt); j++)
{
            if(mappingp(data[lev+":"+opt[j]])&&mappingp(data[lev+":"+opt[j],0]))
	    data[lev+":"+opt[j],0]+=rmap;else
            data+=([lev+":"+opt[j]:sizeof(rmap)?rmap:str;indent;silent]);}
  }
  if (!localflag) infos += ([ file: data ]);
  
  DB("# library successfully loaded #");
  return 1;
}

mixed GetInfo(string library, string key, int level, object player)
{
  int i;
  if (!member(infos, library))
    NewLib(library);
  if (!member(infos, library)) {
    errors++; misses++; return 0;
  }
  for (i=level; i>=0; i--)      // Information hoechstmgl. Qualitaet suchen
  {
    if (member(infos[library], i+":"+key)) {
      hits++;
      return ({ infos[library][i+":"+key, 0],
                infos[library][i+":"+key, 1],
                infos[library][i+":"+key, 2] });
    }
  }
  misses++;
  return 0;
}

void DiscardLib (string library)
{
  DB(sprintf("unloading %s.", library));
  infos = m_delete (infos, library);
}

void Stat ()
{
  int i,j,k;
  string *libs;
  string s;

  libs = m_indices(infos);
  k=0;
  s = sprintf(
    "\nNPC Info Libmaster (%s)\n"
    "Up since: %s\n"
    "Hits: %d, misses: %d, errors: %d\n\n"
    "Loaded Libraries:\n\n",
    ctime()[11..18], dtime(program_time()), hits, misses, errors);
  for (i=sizeof(libs)-1; i>=0; i--) {
    s += sprintf ("  %'.'-:60s %3d keys\n", libs[i]+" ",
           j=sizeof(infos[libs[i]]));
    k += j;
  }
  s += sprintf("  %'-'69s\n", "");
  s += sprintf("  %-19s"+
  " %'.'40s %3d keys\n\n", sizeof(libs)+" libraries total", "", k);
  this_player()->More(s);
}

