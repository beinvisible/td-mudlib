// Tamedhon MUDlib
//
// std/player/commands.c -- alias, history and player command handling
//
// $Date: 2004/01/26 08:08:45 $                      
/* $Revision: 3.9 $       
 * $Log: command.c,v $
 * Revision 3.8  2004/01/26 08:08:45  Querolin
 * modify_command um umlautumwandlung erweitert 
 *
 * Revision 3.8  1998/01/24 20:39:45  Feyaria
 * P_PARALYZED beruecksichtigen
 *
 * Revision 3.7  1997/11/05 16:20:36  Woody
 * vorbereitete Sprueche abbrechen statt verzoegern
 *
 * Revision 3.6  1997/10/06 12:23:04  Woody
 * ?alias Abfrage case-insensitive, vorbereitete Sprueche verzoegern
 *
 * Revision 3.5  1997/09/04 21:08:28  Woody
 * Typo Bugfix ("defininiert")
 *
 * Revision 3.4  1997/06/25 12:02:33  Woody
 * optische Korrektur :)
 *
 * Revision 3.3  1997/06/23 11:00:28  Woody
 * Big Brother installiert
 *
 * Revision 3.2  1997/03/13 01:35:31  Woody
 * prepared for revision control
 *
 * Revision 3.12 1997/02/14 13:30:00  Woody
 * Abfrage 'ali ?text' listet alle Aliases, die text enthalten
 *
 * Revision 3.11 1996/05/25 20:00:00  Woody
 * SL-Erweiterungen implementiert (Richtungen, do syntax)
 *
 * Revision 3.1  1996/04/11 08:37:20  Highlander
 * Alias erweitert auf Liste aller mit xxx startender aliases
 *
 * Revision 3.0  1995/11/22 14:32:07  Jof
 * Updated to revision 3.0
 *
 * Revision 2.50  1995/11/22 14:19:50  Jof
 * Fixed RCS header
 *
 * Revision 2.49  1995/10/18 20:25:14  Jof
 * ME->
 * 's raus
 *
 * Revision 2.48  1995/10/04  17:21:40  Catweazle
 * Aliasse von Vielfachspaces befreit
 *
 * Revision 2.47  1995/07/03  17:32:56  Kirk
 * made decay_average faster
 *
 * Revision 2.46  1995/06/22  17:47:52  Jof
 * *** empty log message ***
 *
 * Revision 2.45  1995/06/06  08:37:36  Jof
 * _notify_fail
 *
 * Revision 2.46  1995/06/05  14:41:01  Jof
 * ACTUAL_NOTIFY_FAIL
 * -
 *
 * Revision 2.45  1995/06/05  14:28:21  Jof
 * optimized parser a little bit
 *
 * Revision 2.44  1995/05/31  17:57:36  Rochus
 * Regexp fuer Unali
 *
 * Revision 2.43  1995/05/29  18:43:25  Jof
 * No longer support Alias conversion mechanism
 *
 * Revision 2.42  1995/03/20  22:41:09  Jof
 * Bugfix for unparsed args (double " " removed to early)
 *
 * Revision 2.41  1995/03/06  09:12:16  Jof
 * Fixed bug in handling of hist-replacement with aliases
 *
 * Revision 2.40  1995/03/03  15:08:27  Jof
 * Fixed some bugs according to special char filtering and aliases
 *
 * Revision 2.39  1995/03/03  14:15:25  Jof
 * Allow soul commands for \\\ now :)
 *
 * Revision 2.38  1995/03/03  12:28:39  Jof
 * Allow UMLAUTE in Aliases ...
 *
 * Revision 2.37  1995/02/24  10:50:01  Jof
 * some parser bugs ...
 *
 * Revision 2.36  1995/02/20  11:01:38  Jof
 * *** empty log message ***
 *
 * Revision 2.35  1995/02/20  10:00:51  Jof
 * *** empty log message ***
 *
 * Revision 2.34  1995/02/14  23:06:49  Jof
 * _unparsed_args jetzt fuer alle ...
 * aeh parser meine ich
 *
 * Revision 2.33  1995/02/06  18:40:48  Jof
 * variable histsize
 *
 * Revision 2.32  1995/02/03  20:54:35  Jof
 * *** empty log message ***
 *
 * Revision 2.31  1995/01/25  17:41:24  Jof
 * Bugfixes :)
 *
 * Revision 2.30  1995/01/24  22:55:46  Jof
 * unpares
 *
 * Revision 2.29  1995/01/23  17:46:54  Jof
 * Multi-Level unparsed args
 *
 * Revision 2.28  1995/01/19  15:16:24  Jof
 * Revision 2.27  1995/01/16  09:10:09  Jof
 * Revision 2.26  1995/01/16  09:06:33  Jof
 * Revision 2.25  1995/01/13  18:55:45  Jof
 * Dont lower_case verb ...
 * Revision 2.24  1995/01/13  17:52:50  Jof
 * togglebare unparsed_args
 *
 * Revision 2.23  1995/01/13  17:09:45  Jof
 * ... unparsed_args
 *
 * Revision 2.22  1995/01/13  12:07:31  Jof
 * _unparsed_args
 *
 * Revision 2.21  1995/01/10  16:20:56  Jof
 * display new aliases
 *
 * Revision 2.20  1995/01/09  19:24:15  Jof
 * Revision 2.19  1995/01/09  18:28:05  Jof
 * default_notify_fail
 *
 * Revision 2.18  1995/01/09  08:56:28  Jof
 * Neues Aliassystem usw
 *
 * Revision 2.18  1995/01/09  23:20:24  mud
 * Replaced stupid aliases by usable ones;new history;streamlining
 *             
 */                            

#pragma strong_types
#pragma save_types
//#pragma range_check
#pragma no_clone

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <properties.h>
#include <language.h>
#include <config.h>
#include <defines.h>
#include <wizlevels.h>
#include <new_skills.h>

#define HIST_SIZE 40

mapping aliases;
int hist_size, show_processing, histmin;
string default_notify_fail;
static string *history, *unparsed_args, *hist2, unmodified;
static int hist_now,hist_now2;
static object last_command_env;
static int cmds_per_time, last_chg;

varargs int __auswerten(string str, string intern);
varargs int SoulComm(string str, string _verb);
varargs mixed More(string str, int fflag, string returnto);
static int _starts_with(string str, string start);

static void AddHistory(string str)
{
  if (!stringp(str) || str=="" || str[0]=='&' || str[0]=='^' ||
      str=="hist")
    return;
  if (sizeof(str)>=4 && hist2[(50+hist_now2-1)%50]!=str)
    hist2[(hist_now2++)%50]=str;
  if (!hist_size) return;
  if (sizeof(str)>=histmin && history[(hist_size+hist_now-1)%hist_size]!=str)
    history[(hist_now++)%hist_size]=str;
}

static void create()
{
  last_chg=0;
  for (hist2=allocate(50),hist_now=0;hist_now<50;hist_now++)
    hist2[hist_now]="";
  histmin=hist_now=hist_now2=0;
  Set(P_LOCALCMDS,({}));
  show_processing=1;
  unparsed_args=({0,0,0});
  hist_size=HIST_SIZE;
}

static int replacedisplay(string str)
{
  if (!str || str=="" || !sscanf(str,"%d",show_processing))
    printf("Unzulässige Eingabe!\n%s 0|1|2\n",query_verb());
    printf("Ersetzungsanzeige auf Level %d.\nLevel 0: Nichts anzeigen\n"+
     "Level 1: Nur History-Ersetzungen anzeigen\n"+
     "Level 2: History- und Alias-Ersetzungen anzeigen\n",show_processing);
  if (show_processing>2&&!IS_WIZARD(ME)) show_processing=2;
  return 1;
}

static int histmin(string str)
{
  int len;

  if (!str||!sscanf(str,"%d",len)||len<0)
  {
    write("Benutzung: histmin ZAHL\nLegt die Mindestlänge fest, die eine \
Befehlszeile haben muss, um in den\nHistory-Puffer zu gelangen. Derzeit \
eingestellt auf "+(string)histmin+" Zeichen.\n");
    return 1;
  }
  histmin=len;
  write("Mindestlänge auf "+(string)len+" eingestellt.\n");
  return 1;
}

static void reallocate_histbuf()
{
  int i;

  history=allocate(hist_size);
  hist_now=0;
  for (i=0;i<hist_size;i++)
    if (!stringp(history[i]))
      history[i]="\n\n";
}

static int histlen(string str)
{
  int d;
  
  if (!str)
  {
    printf("Deine History-Buffer-Länge liegt bei %d Befehlen.\n",hist_size);
    return 1;
  }
  if (!sscanf(str,"%d",d))
  {
    write("histlen <zahl>\nZahl zwischen 0 und 40\n");
    return 1;
  }
  hist_size=d;
  printf("Deine History-Buffer-Länge liegt jetzt bei %d Befehlen.\n",
   hist_size);
  reallocate_histbuf();
  return 1;
}

static void initialize()
{
  if (!pointerp(history)||sizeof(history)!=hist_size)
    reallocate_histbuf();
  add_action("__auswerten","",1);
  if (!mappingp(aliases))
    aliases=([]);
}

static mixed _set_default_notify_fail(string s)
{
  if (stringp(s)&&s!="")
  {
    if (s[<1]!='\n') s+="\n";
    return default_notify_fail=s;
  } else if (!s||s=="") default_notify_fail=0;
  return 0;
}

static mixed _query_default_notify_fail()
{
  return default_notify_fail;
}

static int set_errormessage(string s)
{
  if (!(s=_unparsed_args()))
  {
    _set_default_notify_fail(0);
    write("Standard-Fehlermeldung auf \"Wie bitte?\" gesetzt.\n");
  } else
  {
    write(break_string(sprintf("Standard-Fehlermeldung auf %s gesetzt.\n",
             s),78));
    _set_default_notify_fail(s);
  }
  return 1;
}

void reconnect()
{
  if (!mappingp(aliases)) aliases=([]);
  set_modify_command(0);
  set_modify_command(this_object());
}

static int show_hist()
{
  int i;
  string comm;
  
  write("Die History-Liste enthält folgende Kommandos:\n");
  for(i=0;i<hist_size;i++)
    if ((comm=history[(hist_now+i)% hist_size])!= "\n\n")
      write(" &"+(hist_now+i-hist_size)+"/-"+ (hist_size-i-1)
      +"\t= "+comm+"\n");
  return 1;
}

static string present_alias(mixed *ali)
{
  int j,k;
  string s,s2;
  
  for (s="",j=sizeof(ali)-1;j>=0;j--)
    if (intp(ali[j]))
      if ((k=ali[j])<0)
  s="$"+(k==-1?"":(string)-k)+"*"+s;
      else
  s="$"+(string)k+s;
    else
      s=implode(efun::explode(ali[j],"$"),"\\$")+s;
  return s;
}

static int query_aliases()
{
  int i;
  string *a,*ali;
  
  if(i=sizeof(ali=sort_array(m_indices(aliases),#'<))) //')))
  {
    for(a=({}),i--; i>=0; i--)
      a+=({sprintf(" %s\t= %s",ali[i],present_alias(aliases[ali[i]]))});
    More("Du hast folgende Aliase definiert:\n"+implode(a,"\n"));
  }
  else
    write("Du hast keine Aliase definiert.\n");
  return 1;
}

static int
_starts_with(string str, string start)
{
  return (sizeof(start)>sizeof(str) ? 0
    : str[0..sizeof(start)-1]==start);
}

static int
_contains(string ali, string what)
{
  return strstr( lower_case(present_alias(aliases[ali])), what ) != -1;
}

static int alias(string str)
{
  string command;
  string um,*hits;
  int num, l, pos, cont;
  <string|int>* tmp=({});
 
  if (unmodified&&unmodified!="")
    um=implode(old_explode(unmodified," ")[1..]," ");
  if (um=="") um=0;
  if(!(str=um||_unparsed_args()) || str=="*" || str=="?")
    return query_aliases();

  if ((pos=member(str,' '))<0) // 1 Arg only
  {
    if ((tmp=aliases[str]))
      printf(" %s\t= %s\n",str,present_alias(tmp));
    else
      if (str[<1]=='*')
      {
        str=str[0..<2];
        hits=filter(m_indices(aliases), #'_starts_with, str);
        if (!sizeof(hits))
        {
          printf("Du hast kein Alias, das mit \"%s\" anfängt.\n", str);
          return 1;
        }
        hits=sort_array(hits, #'>);
        for (l=sizeof(hits); l--;)
          hits[l]=sprintf(" %s\t= %s",hits[l],present_alias(aliases[hits[l]]));
        More("Folgende Aliase beginnen mit \""+str+"\":\n"+implode(hits,"\n"));
      }
      else if (str[0]=='?')
      {
        str=lower_case(str[1..]);
        hits=filter(m_indices(aliases), #'_contains, str);
        if (!sizeof(hits))
        {
          printf("Du hast kein Alias, das \"%s\" enthält.\n", str);
          return 1;
        }
        hits=sort_array(hits, #'>);
        for (l=sizeof(hits); l--;)
          hits[l]=sprintf(" %s\t= %s",hits[l],present_alias(aliases[hits[l]]));
        More("Folgende Aliase enthalten \""+str+"\":\n"+implode(hits,"\n"));
      }
      else
        printf("Du hast kein Alias \"%s\" definiert.\n",str);
    return 1;
  }
  if (!pos)
  {
    write("Fehler: Blanc am Alias-Anfang\n");
    return 1;
  }
  if ((command=str[0..pos-1])=="unalias")
  {
    write
      ("Es nicht mäglich, den Befehl unalias zu überladen (wär dumm :))\n");
    return 1;
  }
  str=str[pos+1..],tmp=({});
  while (l=sizeof(str))
  {
    pos=0,cont=1;
    while (cont)
    {
      // innere Schleife: scannt ein Argument und haengt es als Element in
      // tmp an. Laeuft ueber den String bis Stringende oder & oder $
      // erreicht wird, dann ist ein Argument vollstaendig und das naechste
      // faengt an.
      if (pos<l)
      {
        if(str[pos]=='\\') // escapte '\' werden zu einzelnen '\'.
        {
          str=str[0..pos-1]+str[pos+1..];
          l--;
        }
        else
        {
          if (str[pos]=='$' || str[pos]=='&') // & ist historisch...
          { // Argument-Platzhalter gefunden
            cont=0;
            if (pos>0) { // vorhergehender Textblock vollstaendig, anhaengen
              tmp+=({str[0..pos-1]});
            }
            if (pos==l-1) {
              printf("Fehler: %c am Zeilenende\n",str[pos]);
              return 1;
            }
            // $* oder $n ? Im Falle von $n landet in num der ASCII-Wert des
            // Zeichens, von welchem der Wert von '0' abgezogen wird -> num
            // enthaelt danach 0..9, wenn eine Ziffer angegeben wurde.
            num=str[++pos]; // naechstes Zeichen holen
            if (num=='*') {
              // Argument 1 und pos muss wieder eins zurueck.
              num=1;
              pos--;
            } else {
              num-='0';
            }
            if (num<0 || num>9) {
              printf("Fehler: Nach %c muss eine Ziffer oder * folgen\n",
               str[pos-1]);
              return 1;
            }
            // str nach Argumentkennung weiter verarbeiten.
            str=str[pos+1..];
            // Aber fuer den Fall $n* das naechste Zeichen auch untersuchen
            if (sizeof(str) && str[0]=='*') {
              str=str[1..]; // auch ueberspringen
              num = negate(num); // Im Array negiert kodieren
            }
            tmp+=({num});
          }
        }
        ++pos; // naechstes Zeichen im naechste inner while angucken
      }
      // Ende des gesamten Strings erreicht.
      else
      {
        cont=0; // ende inner while
        // letzten Argumentblock anhaengen
        if (str!="") tmp+=({str});
        str=""; // beendet outer while
      }
  }
}
  aliases[command]=tmp;
  printf("Neues Alias: %s\t= %s\n",command,present_alias(tmp));
  return 1;
}

static int unalias(string str) {
  int i;
  string *als,um;

  if (unmodified&&unmodified!="")
    um=implode(explode(unmodified," ")[1..]," ");
  if (um=="") um=0;
  if ( !(str=um||_unparsed_args())) return 0;
  if (!member(aliases,str)) {
    als=regexp(m_indices(aliases),("^"+str+"$"));
    if (!(i=sizeof(als))) {
      write("So ein Alias hast Du nicht definiert.\n");
      return 1;
    }
    for (--i;i>=0;i--)
      efun::m_delete(aliases,als[i]);
    write(break_string(("Du entfernst folgende Aliase: "+
      implode(als," ")+".\n"),75));
    return 1;
  }
  efun::m_delete(aliases,str);
  write("Du entfernst das Alias \""+str+"\".\n");
  return 1;
}

varargs string _unparsed_args(int level)
{
  return unparsed_args[level];
}

#define ARTIKEL ({"das","der","die","des","dem","den","ein","eine","einer",\
                  "eines"})

#define TRENNER ({"in","aus","ueber","über","auf","unter","mit","durch","fuer","für",\
                  "von","vom","im","aufs","ein","weg","zurueck","zurück"})

static string _single_spaces(string str)
{
  int i,j,l,fl;
  string s2;
  
  l=sizeof(s2=str);
  for (fl=1,i=j=0;i<l;i++)
  {
    if (fl)
    {
      if (str[i]!=' ')
      {
  s2[j]=str[i];
  j++;
  fl=0;
      }
    } else
    {
      if ((s2[j]=str[i])==' ') fl=1;
      j++;
    }
  }
  j--;
  while (j>=0&&s2[j]==' ') j--;
  return s2[0..j];
}

static mixed _return_args(string str)
{
  string *t,*t2,verb,s2;
  int i,l,j,l2;

  t=explode(str," ");
  verb=t[0];
  if ((str=unparsed_args[0]=implode(t[1..]," "))=="")
  {
    unparsed_args[0]=unparsed_args[1]=unparsed_args[2]=0;
    return str=verb;
  }
  str=unparsed_args[1]=lower_case(_single_spaces(str));
  t=regexplode(str,"\\<im\\>|\\<ins\\>");
  for (i=1;i<sizeof(t);i+=2) t[i]="in";
  t=regexplode(implode(t,""),"[\\,\\!\\:][\\,\\!\\:]*");
  l=sizeof(t);
  for(i=1;i<l;i+=2) t[i]="";
  t=explode(implode(t,"")," ")-({""});
  for (i=sizeof(t)-2;i>=0;i--)
  {
    if (member(ARTIKEL,t[i])>=0)
      t=t[0..i-1]+t[i+1..];
  }
  unparsed_args[2]=implode(t," ");
  t=regexplode((str=implode(t," ")),"[0-9][0-9]*\\.");
  if ((l=sizeof(t))>2)
  {
    i=1;
    while (i<l-1)
    {
      t[i]=" "+t[i][0..<2]+" ";
      if ((l2=sizeof(t2=explode(t[i+1]," ")))<2)
  t[i+1]+=t[i];
      else
      {
  for (j=1;j<l2;j++)
  {
    if (member(TRENNER,t2[j])>=0)
    {
      t2[j-1]+=t[i];
      l2=0;
    }
  }
  if (!l2)
    t[i+1]=implode(t2," ");
  else
    t[i+1]+=t[i];
      }
      t[i]="";
      i+=2;
    }
    str=_single_spaces(verb+" "+implode(t," "));
    if (str[<1]==' ') str=str[0..<2];
  } else str=verb+(str==""?"":" "+str);
  if (show_processing>2)
    printf("-> {%s}\n",str);
  return str;
}

int TakeFollowers();

static void decay_average()
{
  if (absolute_hb_count()-last_chg>14)
  {
    last_chg=absolute_hb_count()-last_chg;
    if (last_chg>3000)
      last_chg=absolute_hb_count(),cmds_per_time=0;
    else
    {
      while (last_chg>14)
  cmds_per_time=cmds_per_time*9/10, last_chg-=15;
      last_chg=absolute_hb_count()-last_chg;
    }
  }
}

static void HandlePreparedSpells() {
  mixed ps;

  if (pointerp(ps=QueryProp(P_PREPARED_SPELL))
      && sizeof(ps)>=1 && intp(ps[0]) && stringp(ps[1]))
  {
    /*
    ps[0]++;
    SetProp(P_PREPARED_SPELL,ps);
    write("Die Ausfuehrung Deines vorbereiteten Spruches wird verzoegert.\n");
    */
    write("Du brichst die Spruchvorbereitung für ''"+ps[1]+"'' ab.\n");
    SetProp(P_PREPARED_SPELL,0);
  }
  else if (ps)
    SetProp(P_PREPARED_SPELL,0);
}

static mixed bb;

mixed modify_command(string str)
{
  mixed commLine, input, output;
  string verb,s1,*tmp,uverb;
  int l,l2,nummer,i,p;
  mapping para;

  // for (i=sizeof(str);i--;) 
  // {
  //   switch(str[i]) {
  //     case 'ä':        str[i..i]="ae"; break;
  //     case 'Ä':        str[i..i]="Ae"; break;
  //     case 'ö':        str[i..i]="oe"; break;
  //     case 'Ö':        str[i..i]="Oe"; break;
  //     case 'ü':        str[i..i]="ue"; break;
  //     case 'Ü':        str[i..i]="Ue"; break;
  //     case 'ß':        str[i..i]="ss"; break;
  //     case 32..127:    break;
  //     default:         str[i]='?';break;
  //   }
  // }
  
  if (bb)
    log_file("ARCH/bb."+geteuid(this_object()),
             sprintf("%s %O\n",ctime(time())[4..15],str));
  decay_average();
  cmds_per_time+=10000;
  Set(P_ACTUAL_NOTIFY_FAIL,0);
  unparsed_args[0]=unparsed_args[1]=unparsed_args[2]=unmodified="";
  if (!str) return "";
  if (str=="\\ESCAPE" && IS_LORD(ME))
  {
    __set_environment(ME,"/room/void");
    environment()->init();
    printf("You escaped.\n");
    return "";
  }
  if (str[0..2]=="\\\\\\" && IS_LORD(ME))
  {
    str=_return_args(str);
    tmp=efun::explode(str[3..]," ");
    verb=tmp[0];
    if (verb && verb!="")
    {
      s1=implode(tmp[1..]," ");
      if (!__auswerten(s1,verb))
      SoulComm(s1,verb);
    } 
    unparsed_args[0]=unparsed_args[1]=unparsed_args[2]=0;
    return 1;
  }
  if(para=QueryProp(P_PARALYZED))
  {
    write(para[PARALYZE_MSG]);
    return 1;
  }
  unmodified=str;
  last_command_env=environment();
  if (remove_call_out("TakeFollowers")>=0)
    catch(TakeFollowers());  
  if (default_notify_fail) efun::notify_fail(default_notify_fail);
  if (str=="") return "";
  AddHistory(unmodified);
  HandlePreparedSpells();

  if (str[0]=='\\') return _return_args(str[1..]);
  switch (str)
  {
    case "n": return "norden";
    case "s": return "süden";
    case "sueden": return "süden";
    case "w": return "westen";
    case "o": return "osten";
    case "nw": return "nordwesten";
    case "sw": return "südwesten";
    case "suedwesten": return "südwesten";
    case "so": return "südosten";
    case "suedosten": return "südosten";
    case "no": return "nordosten";
    case "ob": return "oben";
    case "u": return "unten";

    case "nob": return "nordoben";
    case "noob": return "nordostoben";
    case "oob": return "ostoben";
    case "soob": return "südostoben";
    case "suedostoben": return "südostoben";
    case "sob": return "südoben";
    case "suedoben": return "südoben";
    case "swob": return "südwestoben";
    case "suedwestoben": return "südwestoben";
    case "wob": return "westoben";
    case "nwob": return "nordwestoben";
    case "nu": return "nordunten";
    case "nou": return "nordostunten";
    case "ou": return "ostunten";
    case "sou": return "südostunten";
    case "suedostunten": return "südostunten";
    case "su": return "südunten";
    case "suedunten": return "südunten";
    case "swu": return "südwestunten";
    case "suedwestunten": return "südwestunten";
    case "wu": return "westunten";
    case "nwu": return "nordwestunten";
  }

  if(str[0]=='&')
  {
    p=sizeof(s1=str[1..]);
    if (p)
      for (l2=0,i=0;i<hist_size-1 && !l2;i++)
      {
  if (history[(hist_size-i+hist_now-1)%hist_size][0..p-1]==s1)
    str=history[(hist_size-i+hist_now-1)%hist_size],l2=1;
  if (l2 && show_processing)
  {
    printf("[%s]\n",str);
    AddHistory(str);
  }
      }
    if (!l2)
    {
      if (str=="&&") str="&-0";
      if (sscanf(str,"&%d",nummer))
      {
  if (nummer<0||(!nummer&&str[1]=='-'))
    if (nummer<-(hist_size-1))
      nummer=-1;
    else
      nummer=(hist_now+nummer-1+hist_size)%hist_size;
  else
    if (nummer>hist_now||hist_now-nummer>hist_size)
      nummer=-1;
    else
      nummer=nummer%hist_size;
  if (nummer<0 || (commLine=history[nummer])=="\n\n")
    notify_fail("Der Befehl ist nicht in der History!\n");
  else
  {
    if (show_processing) write("["+commLine+"]\n");
    AddHistory(str=commLine);
  }
      }
    }
  }
  unmodified=str;
  if(str[0]=='^')
  {
    string *oldnew,pre,post;

    if (sizeof(oldnew=explode(str,"^"))>1)
    {  
      sscanf(history[(hist_now-1)%hist_size],"%s"+oldnew[0]+"%s", pre, post);
      if (show_processing) write("["+pre+oldnew[1]+post+"]\n");
      AddHistory((str=pre+oldnew[1]+post));
    }
  }

  // for (i=sizeof(str)-1;i>=0;i--)
  //   if (str[i]<' '||str[i]>127)
  //     str[i]='?';

  if (!str || str=="") return "";
  
  unmodified=str;
  input=explode(str," ");
  uverb=explode(unmodified," ")[0];
  if (!(commLine=aliases[uverb])) return _return_args(str);

  output="",l=sizeof(commLine),l2=sizeof(input);

  for(i=0;i<l;i++)
    if (!intp(commLine[i]))
      output+=commLine[i];
    else
      if ((p=commLine[i])>=0)
      {
  if (l2>p)
    output+=input[p];
      } else
      {
  p=-p;
  if (l2>p)
    output+=implode(input[p..]," ");
      }
  output=_single_spaces(output);
  l=i=sizeof(output)-1;
  while (i>0&&output[i]==' ') i--;
  if (i!=l) output=output[0..i];
  unmodified=output;
  if (show_processing>1)
    printf("[%s]\n",output);
  return _return_args(output);
}

static int do_list(string str)
{
  string *cmdlist;
  int i;
  
  if (!QueryProp(P_WANTS_TO_LEARN))
    return 0;
  if (!str) return notify_fail("Syntax: do <cmd1>;<cmd2>;...\n"), 0;
  cmdlist=explode(_unparsed_args(),";")-({ "" });
  for (i=0;i<sizeof(cmdlist);i++)
  {
    cmdlist[i]=implode(explode(cmdlist[i]," ")-({}), " "); 
    if (show_processing)
      write("["+cmdlist[i]+"]\n");
    command(cmdlist[i]);
  }
  return 1;
}

//falls die aliasliste kaputt ist ...

int unalias_all()
{
  if (IS_ELDER(this_interactive())) aliases=([]);
  return 1;
}

object _query_last_command_env()
{
  return last_command_env;
}

int _query_show_alias_processing()
{
  return show_processing;
}

int _query_histmin()
{
  return histmin;
}

varargs void AddAction(mixed fun, mixed cmd, int flag, int lvl)
{
  int i;
  mixed *cmds;
  
  if (!(cmds=Query(P_LOCALCMDS))) cmds=({});
  
  if (!pointerp(cmd)) cmd=({cmd});
  
  for (i = sizeof(cmd)-1; i>=0; i--)
    cmds += ({({ cmd[i] , fun, flag, lvl})});
  
  Set(P_LOCALCMDS, cmds);
}

static int auswerten(mixed cmd, string str)
{
  if (closurep(cmd))
    return funcall(cmd,str);
  if (stringp(cmd))
    return call_other(this_object(),cmd,str);
  return 0;
}

varargs int __auswerten(string str, string intern)
{
  string verb;
  mixed *cmd, cmds;
  int i,ret,lvl,l,vl;

  if (!intern)
    verb=query_verb();
  else
    verb=intern;
  lvl=query_wiz_level(ME);
  vl=sizeof(verb);
  cmds=QueryProp(P_LOCALCMDS);
  
  for(i=sizeof(cmds)-1;i>=0;i--)
  {
    cmd=cmds[i],l=sizeof(cmd[0]);
    if (cmd[0]==verb[0..l-1] && cmd[3]<=lvl && (cmd[2]||vl==l) &&
  (ret=auswerten(cmd[1],str)))
      return ret;
  }
  return 0;
}

static string *_query_localcmds()
{
  mixed *l;
  
  l=Query(P_LOCALCMDS);
  if (!pointerp(l))
    l=({});
  return ({
    ({"ali","alias",1,0}),
    ({"unali","unalias",1,0}),
    ({"histmin","histmin",0,0}),
    ({"histlen","histlen",0,0}),
    ({"hist","show_hist",0,0}),
    ({"history","show_hist",0,0}),
    ({"do","do_list",0,LEARNER_LVL}),
    ({"ersetzungsanzeige","replacedisplay",0,0}),
    ({"fehlermeldung","set_errormessage",0,SEER_LVL}),
  })+l;
}

int _query_command_average()
{
  decay_average();
  return cmds_per_time;
}
