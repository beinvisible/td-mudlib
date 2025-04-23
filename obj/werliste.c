/*
** werliste.c -- Erzeugt die "wer" Ausgabe
** ---
** 04-01-2012   Grynnis  Ausgabe angepasst fuer Sprachausgabe
'' 08-02-2009	Gralkor  e(Erzmagier im Ruhestand) eingefuegt
** 03-01-2009   Querolin G(ott) -> E(rzmagier)
** 01-07-2006   Querolin Webmail "P" eingebaut
** 03-27-2004   Querolin idle um y und Y erweitert...
** 03-26-2003   Falgrim  sizeof durch colsizeof ersetzt...
** 02-01-2003   Querolin: Werliste um Rasse Frosch erweitert...
** 11-06-2000   Samtpfote verschiedene idleflags ergaenzt
** 05-11-97     Feyaria [l] fuer Learner, Level 10
** 10-02-97     Woody   query_editing() auch noch abfragen
** 09-03-97     Woody   kwer Codes fuer (i)dle, (z)eitung, (p)ost und (w)eg
** 08-21-97     Woody   Alph-Sort bedingt jetzt kein kwer mehr
** 07-01-97     Woody   QueryWhoListe(): obs==2 moeglich -> gibt Array zurueck
** 06-28-97     Woody   mal wieder ueberarbeitet und optimiert, Backspaces
**                      u.ae. richtig behandeln, ueberlange Titel kuerzen
** 02-12-97     Woody   [Q] Questmagier, Level 30
** 02-02-97     Woody   P_ALIGN_STRING
** 10-10-96     Woody   Invis Magier jetzt im "wer" schoener formatiert
** 06-10-96     Woody   Query(P_RACE)||QueryProp()
** 05-16-96     Woody   Query(P_RACE) statt QueryProp()
** 05-08-96     Woody   Ort rechtsbuendig ausgeben
** 04-05-96     Woody   Alignment nur ausgeben, wenn vorhanden
** 01-26-96     Woody   Gibt nun auch Alignment des Spielers aus
** 05-12-95	Mupfel	[L] (Learnerlevel 15) ersetzt durch
**			[g] (Gastmagierlevel 20)
**                      [H] (Hilfsmagier) ersetzt durch
**                      [X] (Magier fuer besondere Aufgaben)
**                      neu: [Z] Seher- und Magierzweitis
*/

inherit "/std/user_filter";

#include <properties.h>
#include <wizlevels.h>
#include <ansi.h>

#define SPACES(n) sprintf("%*s",n,"")
mixed *erwarte;

static int min(int a,int b)
{
  if (a>b) return b; else return a;
}

static int abs(int a)
{
  if (a<0) return -a; else return a;
}

static int is_greater(object a,object b)
{
  int a1,b1;

  a1=26000*query_wiz_level(a)+26*min(2121,abs(a->QueryProp(P_LEVEL)))+
    getuid(a)[0]-'a';
  b1=26000*query_wiz_level(b)+26*min(2121,abs(b->QueryProp(P_LEVEL)))+
    getuid(b)[0]-'a';
  return a1>b1;
}

static int is_alph_greater(object a, object b)
{
  return geteuid(a)>geteuid(b);
}

static string where(object ob)
{
  if (objectp(ob))
  {
    if(ob->QueryProp(P_LOGON_PLACE))
    {
	return ob->QueryProp(P_LOGON_PLACE);
    }
    else return country(ob);
  }
  else return "unbekannt";
}

string stat(object ob)
{
  int l;
  if (ob->QueryProp(P_NAME) == "Kati")
    return "E";
  l=query_wiz_level(ob);
  if (ob->QueryProp(P_TESTPLAYER)&&l<SEER_LVL)
    return "t";
  if (ob->QueryProp(P_TESTPLAYER)&&l<LEARNER_LVL)
    return "T";
  if (ob->QueryProp(P_SECOND)&&l<SEER_LVL)
    return "z";
  if (ob->QueryProp(P_SECOND))
    return "Z";
  if (l<SEER_LVL)
    return "s";
  if (l>=GOD_LVL)
    return "G";
  if (l>=ARCHON_LVL)
    return "A";
  if (l>=ARCH_LVL)
    return "E";
  if (l>=PENSION_ARCH_LVL)
    return "e";
  if (l>=ELDER_LVL)
    return "W";
  if (l>=LORD_LVL)
    return "R";
  if (l>=SPECIAL_LVL)
    return "X";
  if (l>=QUESTWIZ_LVL)
    return "Q";
  if (l>=DOMAINMEMBER_LVL)
    return "D";
  if (l>=FULL_WIZARD_LVL)
    return "M";
  if (l>=WIZARD_LVL)
    return "m";
  if (l>=LEARNER_LVL)
    return "r";
  if (l>=SEER_LVL)
    return "S";

  return "?";
}

static string istat(object ob)
{
  if (ob->id("mailer")) return "p";
  else if (ob->id("zeitung")) return "z";
  else return "e";
}

string estat(object ob)
{
  object x;
  string pc, ic, ec; //padding-, idle-, edit-char

  if (previous_object(2) && previous_object(2)->QueryProp(P_NO_ASCII_ART))
    pc=" ";  else pc=".";
  ic=pc;
  if(query_idle(ob) >= 86400) { ic="Y"; }
  else if(query_idle(ob) >= 10800) { ic="y"; }
  else if(query_idle(ob) >= 5400) { ic="J"; }
  else if(query_idle(ob) >= 3600) { ic="j"; }
  else if(query_idle(ob) >= 1200) { ic="I"; }
  else if(query_idle(ob) >=  120) { ic="i"; }
  else if(query_idle(ob) >=   60) { ic="'"; }

  ec=((x=query_input_pending(ob)) ? istat(x) :
     (query_editing(ob) ? "e" : pc));
  if (ob->Name())
  {
    if (ec==pc &&
      file_size("/log/webmail/"+to_string(lower_case(ob->Name()))+".sav") > 0)
        ec="P";
  }
  return
    ic+
    (ob->QueryProp(P_AWAY) ? ((query_idle(ob) >= 18000)?"W":"w") : pc)+
    ec+
    stat(ob);
}

varargs static mixed QueryView(object ob, int ist_magier, int sh,
                                       int obs, string tty, int old)
{
  int erw, al;
  string ret,s,s2,*reta;
  string e_prefix, m_prefix, normal;
  string pc, co, hy, bro, brc, bso, bsc;
         //padding char, colon, hyphen, bracket round|squar open|close

  if (previous_object(2) && previous_object(2)->QueryProp(P_NO_ASCII_ART))
    pc=co=hy=bro=brc=bso=bsc=" ";
  else
  { pc="."; co=":"; hy="-"; bro="("; brc=")"; bso="["; bsc="]";}

  m_prefix = "";
  switch(tty)
  {
    case "ansi":
      e_prefix = ANSI_RED;
      if(IS_LEARNER(ob))
        m_prefix = ANSI_UNDERL;
      if(ob->QueryProp(P_FROG))
	m_prefix += ANSI_GREEN;
      if(ob->QueryProp(P_GHOST))
        m_prefix += ANSI_WHITE;
      normal = ANSI_NORMAL;
      break;
    default:
      e_prefix = "*";
      normal = "";
  }

  if (ob->QueryProp(P_INVIS))
  {
    if (!ist_magier) return "";
    if (sh)
    {
      erw=member(erwarte,s=capitalize(getuid(ob)));
      if(erw!=-1&&e_prefix=="*")
        s=e_prefix+s;
      s=sprintf("%s%s%'.'-12s.%s%s",m_prefix,
                ((erw!=-1&&e_prefix!="*") ? e_prefix : ""),
                "("+s+")",estat(ob)[<(14-colsizeof(s))..],normal);
    }
    else {
      s=(!old?"["+stat(ob)+"] ":"")+"("+capitalize(getuid(ob))+")"+
        (ist_magier && ob->QueryProp(P_TESTPLAYER) && !IS_LEARNER(ob)
         ? " <TestSpieler>" : "");
      s=sprintf(s+" %"+(77-colsizeof(s))+"s", "["+where(ob)+"]");
    }
    if (obs)
      return ({ob,s});
    else
      return s;
  }
  if (sh)
  {
    if (getuid(ob)==ROOTID)
      s="Logon";
    else
      /* s=getuid(ob); */

      if(lowerstring(ob->QueryProp(P_NAME)) != ob->query_real_name())
      {
        if(find_player("samtpfote"))
        tell_object(find_player("samtpfote"),
	    sprintf("%O hat nen bug. manipuliertes p_name\n", ob));
      }

      s=ob->query_real_name();
    erw=member(erwarte,s=capitalize(s));
    if(erw!=-1&&e_prefix=="*")
      s=e_prefix+s;
    if (previous_object(2) && previous_object(2)->QueryProp(P_NO_ASCII_ART))
      ret=sprintf("%s%s%-13s%s%s",m_prefix,
                ((erw!=-1&&e_prefix!="*")?e_prefix:""),
                s,estat(ob),normal);
    else
      ret=sprintf("%s%s%'.'-13s%s%s",m_prefix,
                ((erw!=-1&&e_prefix!="*")?e_prefix:""),
                s,estat(ob),normal);
    if (obs)
      return ({ob,ret});
    else
      return ret;
  }
  if (!ret=ob->short()) return "";
  if (obs==2) {
    reta =
      ({ ob,
         stat(ob),
         ob->QueryProp(P_PRESAY) || "",
         ob->QueryProp(P_TITLE) ? " "+ob->QueryProp(P_TITLE) : "",
         ob->Query(P_RACE) || ob->QueryProp(P_RACE) || "unbekannt",
         ob->QueryProp(P_ALIGN_STRING) ||
             ob->al_to_title(ob->QueryProp(P_ALIGN)) ||
             "neutral",
         where(ob)
       });
    return reta;
  }
  // Erst \n und trailing '.' raus
  ret=regreplace(ret, "\n|\.\n$", "", 1);
  // Dann sooft wie moeglich 'x\b' entfernen
  while (ret != (ret=regreplace(ret, "[^\b]\b", "", 1)));
  // Alle uebrig gebliebenen '\b' fliegen raus
  ret=regreplace(ret, "\b", "", 1);

  if (ob->QueryProp(P_FROG)!=0) { s=" "+hy+" Frosch"; } else {
    if (s=(ob->Query(P_RACE) || ob->QueryProp(P_RACE))) s=" "+hy+" "+s;
    else s=" "+hy+" Irgendwas";
  }
  // alignment [Woody@SL]
  al=ob->QueryProp(P_ALIGN);
  s += " "+bro+
    (ob->QueryProp(P_ALIGN_STRING) ||
      (function_exists("al_to_title", ob)
       ? ob->al_to_title(al)
       : "neutral")) + brc;

  s2=" "+bso+where(ob)+bsc;
  if (old)
  {
    ret += s;
    if (ob->QueryProp(P_TESTPLAYER))
      ret+=" <TestSpieler>";
    if (ob->QueryProp(P_SECOND))
      ret+=" <Zweitie>";
  }
  else
  {
    // align where info right [Woody@SL]
    if ((al = ( colsizeof(ret)+colsizeof(s)+colsizeof(s2) )) <= 74)
      ret += s+SPACES(74-al)+s2;
    else {
      ret=ret[0..71-colsizeof(s)-colsizeof(s2)-1]+pc+pc+pc;
      ret+=s+s2;
    }
    ret=bso+stat(ob)+bsc+" "+ret;
  }
  if (obs)
    return ({ob,ret});
  else
    return ret;
}

#define OFFSET(sp) ((maxi/4)*sp+((sp<=maxi%4)?sp:maxi%4))

varargs string* QueryWhoListe(int ist_magier, int short, int obs,
			      string arg, int old) {
  mixed pl;
  string *pls;
  string *s;
  int i,j,k,*t,maxi;

  pl=filter_users(arg);
  if (short & 4)
    pl=sort_array(pl,#'is_alph_greater);
  else
    pl=sort_array(pl,#'is_greater);

  short = short & ~4;

  erwarte=0;
  if (short) {
    if (previous_object())
      erwarte=previous_object()->QueryProp(P_WAITFOR);
    if (!pointerp(erwarte))
      erwarte=({});
  }

  pls=map(pl, #'QueryView, ist_magier, short,
      obs, previous_object()->QueryProp(P_TTY), old) - ({""});
  if (!short)
    return pls;
  maxi=sizeof(pls);
  s=allocate(maxi/4+(maxi%4==0?0:1));
  if( short >= 2 ) { // senkrecht sortieren !
    t = ({ // wird so nur einmal berechnet
      0, OFFSET(1), OFFSET(2), OFFSET(3), maxi
    });
    for (i=0;i<sizeof(s);i++) {
      s[i]="";
      for(j=0;j<4;j++) {
        if ((k=i+t[j])<t[j+1] && pls[k]!="")
          s[i]+=pls[k];
        if( j<3 )
          s[i]+="  ";
      }
    }
  } else {
    for (i=0;i<maxi;i++) {
      if (!s[i/4])
        s[i/4]="";
      if (pls[i]!="")
        s[i/4]+=pls[i];
      if ((i%4!=3) && i<maxi-1)
        s[i/4]+="  ";
    }
  }
  erwarte=({});

  if (previous_object() && previous_object()->QueryProp(P_NO_ASCII_ART))
    return (({sprintf("%72s", sprintf(" %d Teilnehmer",maxi)),""})+s);
  else
    return (({sprintf("%72s", "Liste der Mitspieler vom "+dtime(time())+
      sprintf(" %d (Teilnehmer)",maxi)),""})+s);
}

