/*
 * Tamedhon Mudlib
 * std/player/soul.c -- Spielersoul
 * ---
 * $Date: 2012-04-08 22:00 $
 * $Revision: 3.23 $
 * $Log: soul.c,v $
 *
 * Revision 3.23  2012-04-08 22:00 Grynnis
 * NPC werden jetzt auch grossgeschrieben gefunden (streichel Worg)
 *
 * Revision 3.22  2008-12-18 00:00 Gralkor
 * Typo in rnicke behoben
 * rstupse eingefuehrt
 *
 * Revision 3.21  2008-02-07 15:00 TamTam
 * typo in 'frier' behoben
 *
 * Revision 3.20  2008-01-18 00:00 TamTam
 * Zielperson kann jetzt auch Gross beginnen, zB rknuddel Tamtam
 *
 * 2006-03-08 20:00 Gralkor
 * neue Verben: rwuerge, rumarm und rkuschel
 *
 * Revision 3.19  2004/01/10 18:24:00  Gralkor
 * typo in seufz behoben
 *
 * Revision 3.18  2003/11/08 15:12:00  Querolin
 * wuschel 'alle'
 *
 * Revision 3.17  2003/07/01 19:00:00  Querolin
 * den bug bei "klopfe" als Geist raus. 0 -> ""
 *
 * Revision 3.16  2003/01/05 16:02:00  Serii
 * den bug bei "wunder" entfernt
 *
 * Revision 3.15  2002/08/01 20:36:00  Serii
 * check fuer id STEIN_WUSCHEL (fuer daemonen) und HAUT_WUSCHEL (fuer leute
 * ohne haare oder sonst was...) in (r)wuschel eingebaut
 *
 * Revision 3.14  2002/04/05 13:30:00  Querolin
 * ParseRemote(string arg, string vb) - um das vb erweitert.
 *
 * Revision 3.13  2002/02/27 09:00:00  Querolin
 * rdanke, rwuschel, rnicke ... eingefuegt.
 *
 * Revision 3.12  1998/01/21 14:56:36  Feyaria
 * Neue Verben:
 * bedauern, beruhigen, betteln, glubschen, groehlen, haenseln, hecheln,
 * hoeren, huesteln, jaulen, kneifen, niederkneien, knuffen, krabbeln,
 * kraulen, kreischen, meckern, nerven, quengeln, rollen, ruetteln,
 * schielen, schmachten, schmeicheln, schmusen, schneuzen, springen,
 * strahlen, stubsen, taumlen, torkeln,verzweifeln, wiehern, winseln,
 * zweifeln, zwicken, trommeln, schulterklopfen, jammern, hauen, frieren,
 * murmeln, traellern, gackern, schlucken, piepsen, fuchteln, stolzieren,
 * ruempfen, roecheln, japsen brummen, bitten, fuerchten, glueck wuenschen,
 * ermuntern, spannen, verblueffen, beschimpfen, stolpern, flauschen,
 * beissen, geniessen, summen, wimmern, schubsen
 * StartReactions Aufrufen, einige kleinere Typos,  _notify_fail
 * ---------- Noch eine seher Lange Log-Message :) -------
 *
 * Revision 3.11  1997/10/08 21:21:08  Feyaria
 * begruesse, beneide, betaste, betrachte, fauche, kuschel, schmolle, spucke,
 * stimme, strecke, traeume, warte, wuerge auf Self und Dead geprueft
 *
 * Revision 3.10  1997/09/08 22:13:38  Woody
 * @@alle@@ Behandlung durch regreplace, aber immer noch ned 100%ig perfekt
 * (zB. Fehlverhalten, wenn NPC wie ein Spieler heisst, oder mehrere
 * Froesche... :( )
 *
 * Revision 3.9  1997/06/25 12:06:12  Woody
 * MSGFLAG_SOUL bei Message() Aufrufen, MatchAdverb()
 *
 * Revision 3.8  1997/06/13 10:21:54  Woody
 * [Nn]ame() wieder auf ME->Name() umgestellt (fuer Shadowobjekte)
 *
 * Revision 3.7  1997/05/28 19:57:58  Feyaria
 * match_living beiremote verben
 *
 * Revision 3.6  1997/05/18 16:37:24  Woody
 * JemandeN aus der Ferne angrinsen
 *
 * Revision 3.5  1997/04/22 10:47:32  Woody
 * Typo bei wuscheln raus, QueryPossPronoun() verwenden
 *
 * Revision 3.4  1997/03/31 04:31:50  Woody
 * Zig Typos und Ungereimtheiten entfernt, FeelIt() verwendet jetzt auch
 * break_string(), der Flammenwerfer kommt jetzt nicht mehr vom MG ;),
 * neu: jmd. anschmollen, Ohrfeige AN Geist, ALLE umarmen, VOR jmd.
 * zittern, Hobbits kitzeln/wuscheln (-> Fell bzw. Pelzfuesse),
 * Frosch kuessen debuggt.
 * Neue Verben: Hi/Hallo/Begruesse, Entschuldige, Erschrecke (sowohl
 * jemanden als Geist, als auch selbst erschrecken vor etwas), sich
 * oder jemanden Aergern, jmd. Loben, (jmd.) lieb (an-) Kucken,
 * (jmd. an-) Schweigen, jmd. am Aermel Zupfen, Auf jmd. Warten,
 * sich Wundern (ueber jmd.)
 * --- Ende der vielleicht laengsten Log-Msg der Welt :)
 *
 * Revision 3.3  1997/03/23 13:48:05  Feyaria
 * fauchen: wem->wen, schwapp -> IS_ARCH
 *
 * Revision 3.2  1997/03/21 10:48:22  Feyaria
 * kratzen am Kopf und Leerzeichen bei druecke alle
 *
 * Revision 3.1  1997/03/13 18:44:03  Crea
 * "Monimor raeusperst sich." => "Monimor raeuspert sich."
 *
 * Revision 3.0  1997/03/13 01:47:08  Woody
 * prepared for revision control
 *
 * Version 1.53 SL, 04 Oct 96 (Woody)
 * - P_PERM_STRING bei frage/antworte beruecksichtigt
 *
 * Version 1.52a SL (?), 11 Jul 96 (Woody)
 * - bei say() in GHOST() auch this_object() ausnehmen.
 * - \n in div. GHOST() Msgs rein, wo sie fehlten.
 * - erbleichen fuer Geister modifiziert (wg. capitalized gname())
 *
 * Version 1.51a SL (?), Mai 96 (Woody)
 * - antworte WEM statt WEN :)
 *
 * Version 1.50a SL (?), April '96 (Crea)
 * - Adverb 'mupf' -> 'wood'
 * - neues Verb: 'pruste'
 *
 */

// Set TabStop to 2 characters

// Version 1.40d AL, Jan '96 Presay bei fragt entfernt (Hadra)

// Version 1.40c AL, Aug '95 Fehler bei 'sniff' behoben (Mupfel/Yantro)

/* Version 1.40b AL, April '95                   by -Creative@AnderLand-
   - Endlich den Bug bei 'freue' beheben :)
   - Einbauen von 'stupse'
                  'fauche .. [an]'
                  'lag'
                  'idle'
                  'betrachte'
                  'baue [luftschloss]'
                  'blicke [ins leere]'
                  'traeume'
                  'wische [schweiss von stirn]' (tm) Loculus@AnderLand
                  'reibe [haende]'              (tm) Loculus@AnderLand
                  'beneide ..'                  (tm) Snooker@AnderLand
   - Hinzufuegen neuer Adverbien
   - Aendern von 'jofi' auf 'mupf' :) und updaten der Hilfe

   - Tnx an Highlander fuer seine guten Verben!
*/
/* Version 1.35 MG, Dezember 94
   - Verben
   - Aenderung des Parsings der quoted adverbs
   - rknuddel ignorieren wird vom normalen ignore uebernommen
   */

/* Version 1.32 MG, Juli 94
   - einige Verben
   - ignorieren des rknuddel
   */

/* Version 1.31 MG, Mai 94
   - einige Verben
   */

/* Version 1.3 MG, Mai 94
   - quoted adverbs GEHEN jetzt
   - ParseRest / ParseRemote neu geschrieben
   */

/* Version 1.21 MG, April 94
   - quoted adverbs
   */

/* Danke an Angus fuer den Text von "liebe" */

/* Version 1.2 MG, Januar 94
   - Umstellung der Feelings von vielen kleinen Funktionen auf eine grosse,
     damit der Funktionsoverhead wegfaellt.
   - neue Ausgabe der Adverbien, mit more und nur noch eines pro Zeile
   - mal wieder neue Verben.
   - und das neue Standardadverb "jofi" :)
   */

/* Version 1.1 MG, November 93
   Aenderungen:
   - Ich habe "alle(n)" eingebaut. Die Verwaltung steht, man kann jetzt
   Verben die Moeglichkeit "alle" geben (Bsp. "wink alle").
   - inzwischen wieder einige Verben mehr, aber muss ich das noch
   erwaehnen?
   - (HASS) Rumata hat mein schoenes System fuer die Ausgabestrings
   verkompliziert. Statt &a steht da jetzt z.B. @@adverb@@, was in
   einer weiteren unnoetigen Funktion resultierte. Naja.
   Highlander ryn Tahar
   */

/*
   Ha! Ich nehme jetzt die erste ganze Versionsnummer fuer mich in Anspruch :)
   So, heute ist's geschafft (Ich auch...). Ich bin fertig - ich mach nur
   noch neue Verben rein, wenn Ideen kommen.
   Gegeben an Dienstag, dem 22. Juni 1993 im heimatlichen Horst.
   Highlander ryn Tahar.
   P.S.: Kopiere sich das Ding, wer will - unter folgenden Bedingungen:
   - Den Goettern von MorgenGrauen und
   - Highlander@TAPPMud     Bescheid sagen und ausserdem
   *seufz* nehmt Highlander hier in MorgenGrauen!! -HrT
   Ha! Ihr koennt wieder TAPPMud nehmen :>  Aber sagt's ruhig in MG. -HrT
   - entweder den ganzen Schwall hier drinlassen oder mich mit einem
   neuen Text erwaehnen.
   Das Ganze unter der Voraussetzung, dass ueberhaupt mal jemand eine deutsche
   Seele braucht und sie nicht selber schreiben will :-) (ersparts euch lieber)
   Highlander ryn Tahar.

   **Arbeit:
   Einbau von Adverbien, andere Reaktionen bei Geistern als "Wie bitte?",
   einige neue Verben, einige alte Verben rausgeschmissen.
   Weil es Probleme mit dem autoloading der playeradverbs gab, wurde
   ausserdem die soul vom Objekt zum inheritfile fuer die playershell.

   **Ideen  zum Weitermachen:
   - (verb) alle, z.B. tritt alle
   - Geisterverben mit Adverb

   Version 1.0 fuer MorgenGrauen    Highlander Mai/Juni 93
   */


/* Hier ist sie nun, die DEUTSCHE version der soul,
   viel Spass damit.
   Poietix  Mai 1992
   Vers.: 0.4 fuer JRMud
   P.S. bitte meckert nicht dran rum , sondern verbessert und
   erweitert sie.

   Olpp November 1992
   Vers.: 0.5 fuer MorgenGrauen
   */


#pragma strong_types

#define SOULHELP "/std/player/soulhelp"
#define LF "\n"
#define NOT_SELF 1
#define NOT_DEAD 1

#define QPP QueryPossPronoun
#define RETURN return notify_fail
#define Return return 0||_notify_fail

#define ghost() QueryProp(P_GHOST)
#define gname() ((ghost() ? "Der Geist von "+ME->name(WEM) : ME->Name()))

#define GHOST(sel,oth,vic) { write(sel);\
        say(oth,({this_object(),who}));\
        if (vic) who->Message(vic,MSGFLAG_SOUL); return 1; }

#define GHOSTCHECK(sel,oth,vic) if (ghost()) GHOST(sel,oth,vic)

#ifdef SCHAU_AN
#undef SCHAU_AN
#endif

#define TOUCH 1

#define NEED_PROTOTYPES

#include <properties.h>
#include <language.h>
#include <defines.h>
#include <moving.h>
#include <wizlevels.h>
#include <thing/description.h>
#include <thing/properties.h>
#include <config.h>

static object who,ofoo;
static int for_all,flag,ifoo,last_furz_time;
mapping plr_adverbs;
static string out_sel, out_vic, out_oth, adverb,*verb_out;

static void ParseAdverb(string *words);
string CountUp(string *foo);
static string convert_string(string str);
varargs mixed More(string str, int fflag, string returnto);
string MatchAdverb(string str);

mapping
QueryStdAdverbs()
{
  return ([
           "ärg" : "ärgerlich",
           "albe" : "albern",
           "amü" : "amüsiert",
           "aner" : "anerkennend",
           "besc" : "bescheiden",
           "bitt" : "bitter",
           "bös" : "böse",
           "brei" : "breit",
           "char" : "charmant",
           "däm" : "dämonisch",
           "dank" : "dankbar",
           "drec" : "dreckig",
           "entg" : "entgeistert",
           "erns" : "ernst",
           "erst" : "erstaunt",
           "extr" : "extrem",
           "frag" : "fragend",
           "frec" : "frech",
           "freu" : "freundlich",
           "frfr" : "frank und frei",
           "frö" : "fröhlich",
           "froh" : "froh",
           "frus" : "frustriert",
           "gala" : "galant",
           "gedu" : "geduldig",
           "gela" : "gelangweilt",
          "gemä" : "gemächlich",
           "gene" : "genervt",
       "glü" : "glücklich",
           "grum" : "grummelnd",
	   "gutg" : "gutgelaunt",
          "gutm" : "gutmütig",
          "häm" : "hämisch",
          "heim" : "heimtückisch",
           "herz" : "herzlich",
           "himm" : "himmlisch",
          "hint" : "hinterhältig",
          "höf" : "höflich",
           "hoff" : "hoffnungsvoll",
           "idle" : "idlend",
           "iron" : "ironisch",
           "jamm" : "jammernd",
           "kalt" : "kalt",
           "krit" : "kritisch",
           "lagd" : "lagend",
           "lang" : "langsam",
           "laut" : "laut",
           "lieb" : "liebevoll",
          "maß" : "maßlos",
          "müt" : "mütterlich",
           "nat" : "natürlich",
          "nerv" : "nervös",
           "perf" : "perfekt",
           "rupp" : "ruppig",
           "sanf" : "sanft",
           "sata" : "satanisch",
           "scha" : "schamlos",
           "sche" : "schelmisch",
           "schn" : "schnell",
          "schü" : "schüchtern",
          "sehn" : "sehnsüchtig",
           "stra" : "strahlend",
           "stol" : "stolz",
           "teuf" : "teuflisch",
           "tief" : "tief",
           "trau" : "traurig",
           "übe" : "überzeugt",
           "unge" : "ungeduldig",
           "unglü" : "unglücklich",
           "unsi" : "unsinnigerweise",
           "unsc" : "unschuldig",
          "unve" : "unverschämt",
           "unwi" : "unwissend",
          "vät" : "väterlich",
          "verä" : "verächtlich",
          "verf" : "verführerisch",
           "vers" : "verspielt",
          "vert" : "verträumt",
           "verw" : "verwegen",
           "verz" : "verzweifelt",
           "viel" : "vielsagend",
           "vors" : "vorsichtig",
           "wild" : "wild",
           "wiss" : "wissend",
          "wüt" : "wütend",
          "zär" : "zärtlich",
           "zufr" : "zufrieden",
	   ]);
}

mapping
QueryAdverbs() { return copy(plr_adverbs); }

string
MatchAdverb(string a)  {
  ParseAdverb(efun::explode(a," "));
  return adverb;
}

// Verwaltungsroutinen

static void
add_soul_commands()  {
  if (!plr_adverbs)
    plr_adverbs=([]);
  add_action("SoulComm", "", 1);
}

static int
verben_liste()  {
  More(SOULHELP->Help());
  return 1;
}


varargs static void
ParseRest(string arg, mixed extra)  {
  string wer,wie,*words,quotea;
  int num,bis;

  who=for_all=adverb=wer=wie=0;

  if (!arg) return;

  if (extra)
    if (!pointerp(extra)) {
      if (sscanf(arg, extra+" %s", wie)==1)
	arg=wie;
    }
    else
      for (bis=sizeof(extra),num=0; num<bis; num++)
	if (sscanf(arg, extra[num]+" %s", wie)==1)
	  arg=wie;

  if ((bis=strstr(arg, "/"))>=0)
    quotea=arg[bis..],arg=arg[0..bis-1];
  quotea=quotea||"",arg=arg||"";

  words=explode(implode(explode(arg, ","), " und"), " ");
  if (!sizeof(words)) return;
  if (sizeof(words) && (words[0]=="alle" || words[0]=="allen"))
    for_all=1,wer=words[0],words=words[1..];
  if (!for_all)  {     /* noch kein Opfer */
    wer=match_living(lower_case(words[0]));
    if (stringp(wer)) who=present(lower_case(wer), environment(ME));
    if (!who) who=present(lower_case(words[0]), environment(ME));
    if (who && who->QueryProp(P_INVIS) && !IS_WIZARD(ME)) who=0;
  }
  if (who && sizeof(words))
    words=words[1..];  /* Opfer gefunden - wenn's eines gibt */
  words+=explode(quotea, " ");
  words-=({});
  if (sizeof(words)) ParseAdverb(words);
}

static int
ParseRemote(string arg, string vb)  {
  string wer,wie,*words,*ign;
  int num;

  who=adverb=wer=wie=0;

  if (!arg) return 0;
  if (!vb) return 0;

  words=explode(arg," ");
  who=find_player(lower_case(words[0])) || match_living(lower_case(words[0]));
  if (stringp(who)) who=find_player(who);
  if (!objectp(who)) who=0;
  if (who && who->QueryProp(P_INVIS) && !IS_WIZARD(ME)) who=0;
  if (who) ign=who->QueryProp(P_IGNORE);
  if (!ign) ign=({});
/*
  if (who &&
      (member((query_once_interactive(ME)?
		     getuid() : lower_case(name()))+".rknuddel",ign)>=0 ||
       member("rknuddel", ign)>=0)) {
*/
  if (who &&
      (member(ign,(query_once_interactive(ME)?
      getuid() : lower_case(name()))+"."+vb)>=0 ||
       member(ign, lower_case(vb))>=0)) {
    write(who->name()+" ignoriert Deinen Versuch.\n");
    return 1;
  }
  if (!who || sizeof(words)==1) return 0;
  words=words[1..];
  ParseAdverb(words);
  return 0;
}

static void
ParseAdverb(string *words)  {
  int num,andsign,bis;
  string qadv,*adv,cnt;

  adv=({});
  qadv=0;

  bis=sizeof(words);
  for (num=0; num<bis; num++)
    if (words[num][0..0]=="/")  {
      words[num]=words[num][1..];
      qadv=implode(words[num..], " ");
      words=words[0..num-1];
      break;
    }
  if (sizeof(words) && words[<1]=="und")  {
    words=words[0..<2];
    andsign=1;
  }                   // So, quoted adverb weggeschnippelt

  if (bis=sizeof(words))
    for (num=0; num<bis; num+=2)
      if (QueryStdAdverbs()[words[num]] || plr_adverbs[words[num]])
	adv+=({QueryStdAdverbs()[words[num]] || plr_adverbs[words[num]]});

  cnt=CountUp(adv);
  if (andsign)
    adverb=CountUp((sizeof(adv) ? adv : ({}))+(qadv ? ({qadv}) : ({})));
  else
    adverb=(cnt ? cnt : "")+(cnt && qadv ? " " : "")+(qadv ? qadv : "");
  if (adverb=="") adverb=0;
}

/*  ParseAdverb aus GueldenLand (von Fiona) ohne /
void ParseAdverb(string *words)  {
    int num, i, andsign;
    string qadv, *adv, cnt, tmp;

    adv=({});
    qadv=0;

    // So, wir versuchen jetzt immer abwechselnd ein Adverb-Kuerzel
    // und "und" zu erkennen. Wenn das fehlschlaegt ist ab der Stelle
    // automatisch ein '/', also quoted adverb. (von Fiona)
    for (num=sizeof(words), i=0; i<num; i++) {
        if (words[i][0]=='/') {
            words[i]=words[i][1..];
            qadv=implode(words[i..], " ");
            break;
        }
        tmp=plr_adverbs[words[i]] || QueryStdAdverbs()[words[i]];
        if (tmp) { // Adverbkuerzel erkannt
            if (i==num-1) { // letztes wort
                adv+=({ tmp });
                continue;
            } else if (words[i+1]=="und") {
                adv+=({ tmp });
                i++; // 'und' uebergehen;
                continue;
            }
        }
        // Wort nicht als Kuerzel erkannt: quoten
        qadv=implode(words[i..], " ");
        break;
    }
    if (qadv) adv+=({ qadv });
    adverb=CountUp(adv);
    if (adverb=="") adverb=0;
}
*/

string
CountUp(string *foo)  {
  /* Diese Idee abgekupfert von Marcus@TAPP */
  int cnt;
  string back;
  switch (sizeof(foo))  {
    case 0: back=0; break;
    case 1: back=foo[0]; break;
    default:
    back=implode(foo[0..sizeof(foo)-2], ", ");
    back+=" und "+foo[sizeof(foo)-1];
  }
  return back;
}

varargs static mixed
MixedOut(int casus, string verb, string verbgruppe, string adverb, int touch)
{
  object *envs,*vics;
  string *names,out,aufz;
  int count,msg;

  for_all=0;
  vics=({});
  names=({});
  envs=all_inventory(environment())-({this_player()});
  if (!sizeof(envs))
    RETURN("Nichts und niemand da. Schau Dich nächstes Mal besser um.\n");
  for (count=0; count<sizeof(envs); count++)
    if (living(envs[count]) && !envs[count]->QueryProp(P_INVIS))  {
      vics+=({envs[count]});
      names+=({envs[count]->name(casus)});
    }
  if (!sizeof(vics))
    RETURN("Keiner da. Schau Dich nächstes Mal besser um.\n");
  aufz=CountUp(names);
  for (count=0; count<sizeof(vics); count++)  {
    out=implode(explode(out_vic, "@@alle@@"),aufz);
    out=regreplace(out,"\\<"+vics[count]->name(casus)+"\\>",
        capitalize(vics[count]->QueryDu(casus)), 0);
    /*
    out=implode(explode(out,vics[count]->name(casus)),
		capitalize(vics[count]->QueryDu(casus)));
    */
    msg=vics[count]->Message(break_string(convert_string(out+LF), 78),
                             MSGFLAG_SOUL);
    if (msg==-1)
      write(vics[count]->name()+" ignoriert Dich oder diesen Befehl.\n");
    vics[count]->StartReactionAll( verb, verbgruppe, adverb, touch );
  }
  write(break_string(
      convert_string(implode(explode(out_sel,"@@alle@@"),aufz)+LF), 78));
  return 1;
}

varargs static int
CheckLife(int no_self,int no_dead, string no_self_text, string no_dead_text)
{
  if (who && living(who) && who!=this_player()) return 0;
  if (no_self && who && who==this_player())  {
    if (no_self_text)
      write(no_self_text+LF);
    else
      write("Mach das mit anderen, nicht mit Dir selber.\n");
    return 1;
  }
  if (who && !living(who) && no_dead)  {
    if (no_dead_text)
      write(no_dead_text+LF);
    else
      write("Das darfst Du nur mit Lebewesen.\n");
    return 2;
  }
  if (!who)  {
    write("Schau Dich erst mal um - das angegebene Objekt ist nicht da.\n");
    return 3;
  }
  return 0;
}

static string
convert_string(string str)  {
  /* Ich bin unschuldig, ich hatte das viel einfacher und schoener :)
     Rumata wollte das so ;)  -HrT                                     */
  str = implode( efun::explode( str, "@@name@@" ), ME->name() );
  str = implode( efun::explode( str, "@@Name@@" ), ME->Name() );
  str = implode( efun::explode( str, "@@gname@@" ), gname() );
  str = implode( efun::explode( str, "@@wer@@" ),
		(who?capitalize(who->name(WER,2)):"" ));
  str = implode( efun::explode( str, "@@ wen@@" ),
		(who?" "+who->name(WEN,2):""));
  str = implode( efun::explode( str, "@@ wem@@" ),
		(who?" "+who->name(WEM,2):""));
  str = implode( efun::explode( str, "@@wen@@" ),
		(who?who->name(WEN,2):""));
  str = implode( efun::explode( str, "@@wem@@" ),
		(who?who->name(WEM,2):""));
  str = implode( efun::explode( str, "@@wessen@@" ),
		(who?who->name(WESSEN,2):""));
  str = implode( efun::explode( str, "@@adverb@@" ),
		(adverb?" "+adverb:"") );
  return str;
}

varargs static int
FeelIt(string verb, string gruppe, string adverb, int touch)
{
  int msg,flg;

  flg = MSGFLAG_SOUL;
  if (query_verb() && (query_verb()[0..3]=="frag" ||
                       query_verb()[0..3]=="antw"))
    flg |= MSGFLAG_SAY;

  write( break_string(convert_string(out_sel),78,0,1) );
  if (out_vic && who)  {
    if (query_once_interactive(who))
    {
      msg=who->Message(break_string(convert_string(out_vic),78,0,1), flg);
      if (msg==-1)
        write(capitalize(who->name())+" ignoriert Dich oder diesen Befehl.\n");
    }
    else
    {
      tell_object(who, break_string(convert_string(out_vic),78,0,1) );
      who->StartReaction(verb, gruppe, adverb, touch);
    }
  }
  if (out_oth)
    say( break_string(convert_string(out_oth),78,0,1), ({who,this_player()}) );
  out_sel=out_vic=out_oth=0;
  return 1;
}

/* Die Routine zeige_adverbs ist im ersten Teil abgekupfert aus der exsoul aus
   TAPPMud, d.h. ist wahrscheinlich von Ereborn (ereborn@opal.cs.tu-berlin.de),
   von dem die urspruengliche exsoul stammt.
   Hm, Korrektur: So, wie sie da steht, stammt einiges von Marcus@TAPP.
   Das neu zu programmieren hatte ich wirklich keine Lust. Wieso sollte
   ich auch das Rad neu erfinden, die Routine ist gut.      -Highlander
   */

static int
zeige_adverbs(int mine)  {
  int s,i;
  mixed *sort_arr,*ix;
  string *out;
  mapping m;
  if (mine==1)
    ix=m_indices(m=plr_adverbs);
  else if (mine==2)
    ix=m_indices(m=QueryStdAdverbs());
  else
    ix=m_indices(m=QueryStdAdverbs()+plr_adverbs);
  sort_arr=({ ({}), ({}) });
  for (i=sizeof(m)-1; i>=0; i--)  {
    string tmp1,tmp2;
    sort_arr[0]+=({ ix[i][2]+ix[i][1]*256+ix[i][0]*65536 });
    sort_arr[1]+=({ ix[i] });
  }
  sort_arr = order_alist(sort_arr);
  sort_arr = sort_arr[1];
  out=allocate(sizeof(sort_arr)+1);
  out[0]="Abk.   Adverb\n";
  for (i=0;i<sizeof(sort_arr);i++)
    out[i+1]=
      sprintf("%-7'.'s",sort_arr[i])+break_string(m[sort_arr[i]], 78, 7)[7..];
  verb_out=out;
  write("\
Mehrere Adverbien können mit und aneinandergereiht werden.\n\
Bsp: grinse frö und frec und mupf\n\
Außerdem kann man irgendwann einen / setzen. Alles, was danach kommt,\n\
wird wie ein Adverb angehängt (sofern das Verb Adverbien unterstützt).\n\
Bsp: grins /wie zwei Vögel im Wind\n\
Frage mich jetzt keiner, was das heißen soll :>     Have fun,\n\
                                                Highlander ryn Tahar\n\
\                              (--RETURN--)\n");
  input_to("zeige_adverbs_2");
  return 1;
}

static int
zeige_adverbs_2(string foo)  {
  More(implode(verb_out,""));
  verb_out=0;
  return 1;
}

varargs static int
SoulComm(string str, string _verb)  {
  int t_g,t_n,flag,i;
  string pre,str1,str2,str3,*taenze,vb;
  out_sel=out_vic=out_oth=who=0;
  if (interactive(ME)) str=ME->_unparsed_args(); // NPCs haben das nicht :(
  if (str=="") str=0;
  vb=_verb||query_verb();
  if (vb[<1]=='e' && vb!="noe") vb=vb[0..<2];
  // some stuff to be added
  switch (vb)  {

    /**************** Achselzucken ***************/
    case "zuck":
    ParseRest(str,({"achseln","mit achseln"}));
    out_sel="Du zuckst@@adverb@@ mit den Schultern.";
    out_oth="@@gname@@ zuckt"+(adverb ? "@@adverb@@" : " ratlos")+
      " mit den Schultern.";
    return FeelIt("zuck","neutral",adverb,0);

    /**************** Aechzen ***************/
    case "ächz":
    case "aechz":
    ParseRest(str);
    if (str && !adverb)
      Return("Ächze wie?\n");
    out_sel="Du ächzt@@adverb@@.";
    out_oth="@@gname@@ ächzt@@adverb@@.";
    return FeelIt("ächz","traurig",adverb,0);

    /**************** sich/jmd Aergern ***************/
    case "ärger":
    case "aerger":
    ParseRest(str, "dich");
    if (str && str!="dich" && !adverb && !who)
      Return("Ärgere wen oder Dich selbst? und wie?\n");
    if (who && (who!=this_player()))
    {
      out_sel="Du ärgerst @@wen@@@@adverb@@.";
      out_vic="@@gname@@ ärgert Dich@@adverb@@.";
      out_oth="@@gname@@ ärgert @@wen@@@@adverb@@.";
    }
    else
    {
      out_sel="Du ärgerst Dich"+(adverb ? "@@adverb@@." : " grün und blau.");
      out_oth="@@gname@@ ärgert sich"+
        (adverb ? "@@adverb@@." : " grün und blau.");
    }
    return FeelIt("ärger","fies",adverb,TOUCH);

    /**************** Anschmiegen ***************/
    case "schmieg":
    ParseRest(str);
    if (!who)
      Return("An wen willst Du Dich anschmiegen?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
		  "Das geht doch nicht.",
		  "Nein, das macht keinen Spaß. Lebt ja nicht mal."))
      return 1;
    out_sel="Du schmiegst Dich@@adverb@@ an@@ wen@@ an.";
    out_vic="@@gname@@ schmiegt sich@@adverb@@ an Dich.";
    out_oth="@@gname@@ schmiegt sich@@adverb@@ an@@ wen@@ an.";
    return FeelIt("schmieg","intim",adverb,TOUCH);

    /**************** Antworten ***************/
    case "antwort":
    if (!str)
      Return("Antworte [WEM] WAS?\n");
    ParseRest(str);
    if (!who)
      str1=capitalize(str);
    else
      if (sscanf(str,"%s %s",str1,str1)!=2)
	Return("Antworte was?\n");
      else
	str1=capitalize(str1);
    out_sel="Du antwortest@@ wem@@: "+str1;
    if (objectp(QueryProp(P_PERM_STRING)))
      str1 = call_other(QueryProp(P_PERM_STRING),"permutate_string",str1)||"";
    if (who) out_vic="@@gname@@ antwortet Dir: "+str1;
    out_oth="@@gname@@ antwortet@@ wem@@: "+str1;
    return FeelIt("antwort","neutral",adverb,0);

    /**************** Applaudieren ***************/
    case "applaudier":
    GHOSTCHECK("Deine Hände fahren durcheinander durch - war wohl nix.\n",
        gname()+" will applaudieren, aber "+QPP(FEMALE,WER,PLURAL)+
        " Haende sausen\ndurcheinander durch.\n", 0);
    if (!str) {
      out_sel="Du applaudierst von ganzem Herzen.";
      out_oth="@@Name@@ gibt eine Runde Applaus.";
    }
    else  {
      ParseRest(str);
      if (for_all)  {
	out_sel="Du applaudierst @@alle@@@@adverb@@.";
	out_vic=gname()+" applaudiert @@alle@@@@adverb@@.";
	return MixedOut(WEM,"applaudier","nett",adverb,0);
      }
      if (!who && !adverb)
	Return("Applaudiere wem oder wie oder so ähnlich.\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
			   "Nein, das gehört sich nicht.",
			   "Sachen wird hier nicht applaudiert, OK?"))
	return 1;
      out_sel="Du applaudierst@@ wem@@@@adverb@@.";
      if (who) out_vic="@@Name@@ applaudiert Dir@@adverb@@.";
      out_oth="@@Name@@ applaudiert@@ wem@@@@adverb@@.";
    }
    return FeelIt("applaudier","nett",adverb,0);

    /**************** Argln ***************/
    case "argl":
    ParseRest(str);
    if (str && !adverb)
      Return("Argle wie?\n");
    out_sel="Du arglst"+(adverb ? "@@adverb@@." : " ein wenig vor Dich hin.");
    out_oth="@@gname@@ arglt"+
      (adverb ? "@@adverb@@." : " ein wenig vor sich hin.");
    return FeelIt("argl","ärgerlich",adverb,0);

    /**************** Aufatmen ***************/
    case "atm":
    if (!str || sscanf(str,"%sauf",str1)!=1)
      Return("Atme wie auf?\n");
    ParseRest(str1);
    out_sel="Du atmest"+(adverb ? "@@adverb@@" : " erleichtert")+" auf.";
    out_oth="@@gname@@ atmet"+(adverb ? "@@adverb@@" : " erleichtert")+" auf.";
    return FeelIt("atme","neutral",adverb,0);

    /**************** Bauen ***************/
    case "bau":
    GHOSTCHECK(
        "So leid es mir tut, aber Geister haben nix zum Bauen.\n", "", 0);
    ParseRest(str, "luftschloss");
    if (str && str!="luftschloss" && !adverb)
      Return("Baue das Luftschloss wie?\n");
    out_sel="Du baust Dir@@adverb@@ ein Luftschloss.";
    out_oth="@@Name@@ baut sich@@adverb@@ ein Luftschloss.";
    return FeelIt("bau","neutral",adverb,0);

    /**************** Beneiden ***************/
    case "beneid":
    ParseRest(str);
    if (!who)
      Return("Beneide wen?\n");
    if (who && CheckLife(NOT_SELF,0,
        "Aber Du hast doch alles, um was Du Dich beneiden könntest!",
        ""))
      return 1;
    out_sel="Du beneidest @@wen@@@@adverb@@.";
    out_vic="@@gname@@ beneidet Dich@@adverb@@.";
    out_oth="@@gname@@ beneidet @@wen@@@@adverb@@.";
    return FeelIt("beneid","nett",adverb,0);

    /**************** Betrachten ***************/
    case "betracht":
    ParseRest(str);
    if (!who)
      Return("Betrachte wen?\n");
    if (who && CheckLife(NOT_SELF,0,
        "Wie willst Du Dich denn selber betrachten? (Ohne Spiegel)", ""))
      return 1;
    out_sel="Du betrachtest @@wen@@"+(adverb ? "@@adverb@@." : " eingehend.");
    out_vic="@@gname@@ betrachtet Dich"+
      (adverb ? "@@adverb@@." : " eingehend.");
    out_oth="@@gname@@ betrachtet @@wen@@"+
      (adverb ? "@@adverb@@." : " eingehend.");
    return FeelIt("betrachte","neutral",adverb,0);

   /***************** Bedauern *****************/
    case "bedauer":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du bedauerst @@alle@@@@adverb@@.";
      out_vic="@@name@@ bedauert @@alle@@@@adverb@@.";
      return MixedOut(WEM,"bedauer","neutral",adverb,0);
    }
    if(str && !who && !adverb)
      Return("Wen willst Du Wie bedauern?\n");

    if(who==this_player()) who=0;
    out_sel="Du bedauerst "+(who?who->name(WEN):"Dich")+"@@adverb@@.";
    if(who) out_vic="@@name@@ bedauert Dich@@adverb@@.";
    out_oth="@@name@@ bedauert "+(who?who->name(WEN):"sich")+"@@adverb@@.";
    return FeelIt("bedauer","neutral",adverb,0);

   /***************** Beruhigen ****************/
    case "beruhig":
    ParseRest(str);
    if(str && !who && !adverb)
      Return("Wen oder Wie willst Du Dich beruhigen?\n");
    if(CheckLife(0,NOT_DEAD,0,"Das ist schon ruhig genug!"))
      return 1;
    if(who==this_player())
      who=0;
    out_sel="Du beruhigst "+(who?who->name(WEN):"Dich")+"@@adverb@@.";
    if(who) out_vic="@@name@@ beruhigt Dich@@adverb@@.";
    out_oth="@@name@@ beruhigt "+(who?who->name(WEN):"sich")+"@@adverb@@.";
    return FeelIt("beruhig","nett",adverb,0);

    /****************** Betteln ******************/
    case "bettel":
    case "bettle":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du bettelst @@alle@@@@adverb@@ an.";
      out_vic="@@name@@ bettelt @@alle@@@@adverb@@ an.";
      return MixedOut(WEN,"bettel","traurig",adverb,0);
    }
    if(str && !adverb && !who)
      Return("Wen willst Du Wie anbetteln?\n");
    if(CheckLife(NOT_SELF,NOT_DEAD,
        "Nee, irgendwo hört der Spaß auf!",
        "Da könntest Du lange betteln..."))
      return 1;
    out_sel="Du bettelst"+(who?" "+who->name(WEN)+
            "@@adverb@@ an":"@@adverb@@")+".";
    if(who) out_vic="@@name@@ bettelt Dich@@adverb@@ an.";
    out_oth="@@name@@ bettelt"+(who?" "+who->name(WEN)+
            "@@adverb@@ an":"@@adverb@@")+".";
    return FeelIt("bettel","traurig",adverb,0);

    /**************** Begruessen ***************/
    case "hallo":
    case "hi":
    case "begrüß":
    case "begruess":
    ParseRemote(str, vb);
    if (!who)
      Return("Wen willst Du begrüßen?\n");
    if (CheckLife(NOT_SELF, NOT_DEAD,
        "Wieso willst Du Dich begrüßen? Du bist Doch schon Dein ganzes "
        "Leben mit Dir\nzusammen.",
        "Keine Reaktion. Ist wohl sinnvoller, nur Lebewesen zu begrüßen."))
      return 1;
    if (present(who, environment()))  {
      out_sel="Du heißt @@wen@@@@adverb@@ willkommen.";
      out_vic="@@gname@@ heißt Dich@@adverb@@ willkommen.";
      out_oth="@@gname@@ heißt @@wen@@@@adverb@@ willkommen.";
    }
    else {
      out_sel="Du heißt @@wen@@@@adverb@@ aus der Ferne willkommen.";
      out_vic="@@gname@@ heißt Dich@@adverb@@ aus der Ferne willkommen.";
    }
    return FeelIt("begrüße","froh",adverb,0);

    /**************** Beissen *****************/
    case "beiß":
    case "beiss":
    ParseRest(str);
    if( who == ME ) who = 0;
    out_sel="Du beißt "+(who ? who->name(WEN) : "Dich selbst")+"@@adverb@@.";
    if( who )
      out_vic="@@gname@@ beißt Dich@@adverb@@.";
    out_oth="@@gname@@ beißt "+(who ? who->name(WEN) : "sich selbst") +
      "@@adverb@@.";
    return FeelIt("beiß","fies",adverb,TOUCH);

    /**************** Beschimpfen *****************/
    case "beschimpf":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du beschimpfst @@alle@@@@adverb@@.";
      out_vic="@@name@@ beschimpft @@alle@@@@adverb@@.";
      return MixedOut(WEN,"beschimpf","fies",adverb,0);
    }
    if(CheckLife(NOT_SELF,NOT_DEAD,
          "Wieso willst Du Dich selbst beschimpfen?",
          "Der erzieherische Effekt ist beim Beschimpfen von Dingen nicht "
          "sehr groß."))
      return 1;
    out_sel="Du beschimpfst "+who->name(WEN)+"@@adverb@@.";
    out_vic="@@gname@@ beschimpft Dich@@adverb@@.";
    out_oth="@@gname@@ beschimpft "+who->name(WEN)+"@@adverb@@.";
    return FeelIt("beschimpf","fies",adverb,0);

    /**************** Betasten ***************/
    case "betast":
    case "begrabsch":
    case "grabbel":
    case "grabbl":
    ParseRest(str);
    if (!who)
      Return("Begrabsche wen?\n");
    if( who == this_player() )
    {
      out_sel="Du grabbelst@@adverb@@ an Dir herum.";
      out_oth="@@gname@@ grabbelt@@adverb@@ an sich selbst herum.";
    }
    else
    {
      out_sel="Du grabbelst@@adverb@@ an @@wem@@ herum.";
      out_vic="@@gname@@ grabbelt@@adverb@@ an Dir herum.";
      out_oth="@@gname@@ grabbelt@@adverb@@ an @@wem@@ herum.";
    }
    return FeelIt("betast","frech",adverb,TOUCH);

    /**************** Bewundern ***************/
    case "bewunder":
    ParseRest(str);
    if (!who)
      Return("Bewundere wen?\n");
    out_sel="Du bewunderst @@wen@@@@adverb@@.";
    out_vic="@@gname@@ bewundert Dich@@adverb@@.";
    out_oth="@@gname@@ bewundert @@wen@@@@adverb@@.";
    return FeelIt("bewunder","nett",adverb,0);

    /**************** Bibbern ***************/
    case "bibber":
    ParseRest(str);
    if (str && !adverb)
      Return("Bibbere wie?\n");
    out_sel="Du bibberst@@adverb@@ vor Kälte.";
    out_oth="@@gname@@ bibbert@@adverb@@ vor Kälte.";
    return FeelIt("bibber","neutral",adverb,0);

    /**************** Bitten *****************/
    case "bitt":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du bittest @alle@@"+(adverb ? "@@adverb@@." :
              " um Verzeihung.");
      out_vic="@@name@@ bittet @@alle@@" +(adverb ? "@@adverb@@." :
              " um Verzeihung.");
      return MixedOut(WEN,"bitt","nett",adverb,0);
    }
    if(CheckLife(NOT_SELF,NOT_DEAD,
          "Du bittest Dich selbst um Verzeihung und vergibst Dir selbst "
           "großzügig.",
           "Das würde wohl wenig bringen, meinst Du nicht?"))
      return 1;
    if (!who) Return("Wen willst Du um was bitte?\n");
    out_sel="Du bittest "+who->name(WEN)+
            (adverb ?"@@adverb@@.":" um Verzeihung.");
    out_vic="@@gname@@ bittet Dich"+(adverb ? "@@adverb@@." : " um Verzeihung.");
    out_oth="@@gname@@ bittet "+who->name(WEN)+
            (adverb ? "@@adverb@@." : " um Verzeihung.");
    return FeelIt("bitt","nett",adverb,0);

    /**************** Blicken ***************/
    case "blick":
    ParseRest(str, ({"leere", "ins leere"}) );
    if (str && str!="ins leere" && !adverb)
      Return("Blicke wie ins Leere?\n");
    out_sel="Du blickst@@adverb@@ ins Leere.";
    out_oth="@@gname@@ blickt@@adverb@@ ins Leere.";
    return FeelIt("blick","neutral",adverb,0);

    /**************** Bohre Nase ***************/
    case "bohr":
    ParseRest(str, ({"nase","in der nase","in nase"}));
    if (str && str!="nase" && str!="in nase" && str!="in der nase" && !adverb)
      Return("Bohre wie Nase?\n");
    out_sel="Du bohrst@@adverb@@ in Deiner Nase.";
    out_oth="@@gname@@ bohrt@@adverb@@ in der Nase.     Igitt! :)";
    return FeelIt("bohr","frech",adverb,0);

    /**************** Brummen ***************/
    case "brumm":
    case "brumml":
    ParseRest(str);
    out_sel="Du brummst@@adverb@@.";
    out_oth="@@gname@@ brummt@@adverb@@.";
    return FeelIt("brimm","neutral",adverb,0);

    /**************** cls ***************/
    case "cls":
    write("[2J[H");
    return 1;

    /**************** Daeumchendrehen ***************/
    case "dreh":
    if (!str)
      Return("Drehe was?\n");
    if(strstr(str,"däumchen")<0 && strstr(str,"daumen")<0)
      Return("Drehe was?\n");
    ParseRest(str,({"daeumchen","däumchen","daumen"}));
    out_sel="Du drehst@@adverb@@ Däumchen.";
    out_oth="@@gname@@ dreht@@adverb@@ Däumchen.";
    return FeelIt("dreh","neutral",adverb,0);

    /**************** Danken ***************/
    case "dank":
    ParseRest(str);
    if (!who)
      Return("Bei wem willst Du Dich bedanken?\n");
    if (CheckLife(NOT_SELF, NOT_DEAD,
        "Leidest Du jetzt schon an Persönlickeitsspaltung? Ne, ne...",
        "Keine Reaktion. Ist wohl befriedigender, sich bei Lebewesen zu "
        "bedanken."))
      return 1;
    out_sel="Du bedankst Dich@@adverb@@ bei@@ wem@@.";
    out_vic="@@gname@@ bedankt sich@@adverb@@ bei Dir.";
    out_oth="@@gname@@ bedankt sich@@adverb@@ bei @@wem@@.";
    return FeelIt("dank","nett",adverb,0);

    /**************** Denken ***************/
    case "denk":
    if (ghost())
      Return("Womit willst Du denn denken? Du hast keinen Kopf...\n");
    ParseRest(str);
    str2=explode(str||""," ")[0];
    if (str && (!adverb ||
        ((QueryStdAdverbs()[str2] || plr_adverbs[str2])) != adverb)) {
      out_sel="Du denkst   . o O ("+str+")";
      out_oth="@@Name@@ denkt   . o O ("+str+")";
    }
    else {
      out_sel="Du fängst@@adverb@@ an zu denken.\n"
        "Kleine Rauchwölkchen steigen auf...";
      out_oth="@@Name@@ fängt@@adverb@@ an zu denken.\n"
        "Kleine Rauchwölkchen steigen auf...";
    }
    return FeelIt("denk","neutral",adverb,0);

    /**************** Deuten ***************/
    case "deut":
    ParseRest(str,"auf");
    if (for_all) {
      out_sel="Du deutest@@adverb@@ auf @@alle@@.";
      out_vic="@@name@@ deutet@@adverb@@ auf @@alle@@.";
      return MixedOut(WEN,"deut","neutral",adverb,0);
    }
    if (!who)
      Return("Auf wen oder was willst Du deuten?\n");
    out_sel="Du deutest@@adverb@@ auf "+
      (who==this_object()?"Dich." : "@@wen@@.");
    if (who != this_object()) out_vic="@@gname@@ deutet@@adverb@@ auf Dich.";
    out_oth="@@gname@@ deutet@@adverb@@ auf "+
      (who==this_object() ? "sich selber.": "@@wen@@.");
    return FeelIt("deut","neutral",adverb,0);

    /**************** Druecken ***************/
    case "drück":
    case "drueck":
    ParseRest(str);
    if (for_all)  {
      GHOSTCHECK(
          "Du willst alle an Dich drücken - nur hast Du schon wieder nicht\n"
          "daran gedacht, dass sowas als Geist nicht geht.\n",
          gname()+" will alle an sich drücken - hat aber mal wieder\n"
          "nicht an die Nachteile des Geisterdaseins gedacht.\n", 0);
      out_sel="Du drückst @@alle@@"+
        (adverb ? "@@adverb@@" : " zärtlich")+" an Dich.";
      out_vic="@@Name@@ drückt @@alle@@"+
        (adverb ? "@@adverb@@" : " zärtlich")+" an sich.";
      return MixedOut(WEN,"drück","nett",adverb,TOUCH);
    }
    if (!who)
      Return("Wen willst Du denn drücken?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
		  "Das macht doch keinen Spaß.",
		  "Ich drücke nur jemanden, nicht etwas."))
      return 1;
    GHOSTCHECK(
        "Du willst "+who->name(WEN)+" an Dich drücken - nur hast Du schon\n"
	"wieder nicht daran gedacht, dass sowas als Geist nicht geht.\n",
	gname()+" will "+who->name(WEN)+" an sich drücken - hat aber\n"
	"mal wieder nicht an die Nachteile des Geisterdaseins gedacht.\n",
	gname()+" will Dich an sich drücken - hat aber mal wieder\n"
	"nicht an die Nachteile des Geisterdaseins gedacht.\n");
    out_sel="Du drückst@@ wen@@"+(adverb ? "@@adverb@@" : " zärtlich")+
      " an Dich.";
    out_vic="@@Name@@ drückt Dich"+(adverb ? "@@adverb@@" : " zärtlich")+
      " an sich.";
    out_oth="@@Name@@ drückt@@ wen@@"+(adverb ? "@@adverb@@" : " zärtlich")+
      " an sich.";
    return FeelIt("drück","nett",adverb,TOUCH);

    /**************** Entschuldige ***************/
    case "entschuldig":
    case "sorry":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du entschuldigst Dich@@adverb@@ bei @@alle@@.";
      out_vic="@@name@@ entschuldigt sich@@adverb@@ bei @@alle@@.";
      return MixedOut(WEM,"sorry","nett",adverb,0);
    }
    if (str && !who && !adverb)
            Return("Entschuldige Dich wie oder bei wem?\n");
    if (CheckLife(NOT_SELF, NOT_DEAD,
        "Du nimmst Deine Entschuldigung großzügig an.",
        "Keine Reaktion. Ist wohl sinnvoller, sich nur bei Lebewesen zu "
        "entschludigen."))
      return 1;
    out_sel="Du bittest@@ wen@@@@adverb@@ um Entschuldigung.";
    if (who) out_vic="@@gname@@ bittet Dich@@adverb@@ um Entschuldigung.";
    out_oth="@@gname@@ bittet@@ wen@@@@adverb@@ um Entschuldigung.";
    return FeelIt("sorry","nett",adverb,0);

    /**************** Erbleichen ***************/
    case "erbleich":
    GHOSTCHECK(
        "Ich weiß zwar nicht, wie Du das schaffst, "
        "aber Du wirst noch bleicher.\n",
        break_string(gname()+" schafft es erstaunlicherweise, noch "
            "eine Spur bleicher zu werden.",78), 0);
    ParseRest(str);
    if (str && !adverb)
      Return("Wie willst Du erbleichen?\n");
    out_sel="Du erbleichst@@adverb@@.";
    out_oth="@@Name@@ erbleicht@@adverb@@.";
    return FeelIt("erbleich","neutral",adverb,0);

    /**************** Ermuntern *****************/
    case "ermunter":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du ermunterst @@alle@@"+(adverb ? "@@adverb@@." : ".");
      out_vic="@@name@@ ermuntert @@alle@@" +(adverb ? "@@adverb@@." : ".");
      return MixedOut(WEN,"ermunter","nett",adverb,0);
    }
    if(CheckLife(NOT_SELF,NOT_DEAD,
          "Lass Dich doch von jemand anderem ermuntern!",
          "Tote Dinge zu ermuntern hat wenig Sinn..."))
      return 1;
    out_sel="Du ermunterst "+who->name(WEN)+(adverb ?"@@adverb@@.": ".");
    out_vic="@@gname@@ ermuntert Dich"+(adverb ? "@@adverb@@." : ".");
    out_oth="@@gname@@ ermuntert "+who->name(WEN)+(adverb ?"@@adverb@@.": ".");
    return FeelIt("ermunter","nett",adverb,0);

    /**************** Erroeten ***************/
    case "erroet":
    case "erröt":
    GHOSTCHECK("Du schaffst es nur bis zu einem blassen Rosa, aber immerhin.\n",
        "Die Wangen des Geistes von "+name(WEM)+" werden leicht rosa.\n", 0);
    ParseRest(str);
    if (!adverb && str)
      Return("Erröte wie?\n");
    out_sel="Deine Wangen glühen@@adverb@@.";
    out_oth="@@Name@@ errötet@@adverb@@.";
    return FeelIt("erröt","neutral",adverb,0);

    /**************** Erschrecken ***************/
    case "erschreck":
    case "erschrick":
    ParseRest(str);
    if (!who) {
      out_sel="Du erschrickst"+(adverb ? "@@adverb@@." : " fürchterlich.");
      out_oth="@@gname@@ erschrickt"+
        (adverb ? "@@adverb@@." : " fürchterlich.");
    }
    else {
      if (!ghost())
        Return("Du bist zu harmlos, Geist müsste man sein...\n");
      out_sel="Mit einem lauten BUH! erschreckst Du @@wen@@"+
        (adverb ? "@@adverb@@." : " fürchterlich.");
      out_vic="BUH! Du zuckst entsetzt zusammen.\n@@gname@@ hat Dich"+
        (adverb ? "@@adverb@@" : " ganz schön")+" erschreckt!";
      out_oth="BUH! @@gname@@ erschreckt @@wen@@"+
        (adverb ? "@@adverb@@." : " fürchterlich.");
    }
    return FeelIt("erschrick","frech",adverb,0);

    /**************** (an)-Fauchen ***************/
    case "fauch":
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Fauche wie? Oder fauche wen an? Oder wie wen an?\n");
    if (who) {
      if ( CheckLife(NOT_SELF,0,
          "Wie willst Du Dich denn selber anfauchen?", ""))
        return 1;
      out_sel="CHCHCHCHCHHH!!!!! Du fauchst @@wen@@@@adverb@@ an.";
      out_vic="Huch? @@gname@@ faucht Dich@@adverb@@ an.";
      out_oth="@@gname@@ faucht @@wen@@@@adverb@@ an.";
    }
    else {
      out_sel="Du fauchst@@adverb@@.";
      out_oth="@@gname@@ faucht@@adverb@@.";
    }
    return FeelIt("fauch","frech",adverb,0);

    /**************** Flauschen *****************/
    case "flausch":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du flauschst @@alle@@@@adverb@@.";
      out_vic="@@name@@ flauscht @@alle@@@@adverb@@.";
      return MixedOut(WEN,"flausch","nett",adverb,TOUCH);
    }
    if(CheckLife(NOT_SELF,NOT_DEAD,
          "Wieso willst Du Dich selbst flauschen?",
          "Tote Dinge flauschen... Also wirklich!"))
      return 1;
    out_sel="Du flauscht "+who->name(WEN)+"@@adverb@@.";
    out_vic="@@gname@@ flauscht Dich@@adverb@@.";
    out_oth="@@gname@@ flauscht "+who->name(WEN)+"@@adverb@@.";
    return FeelIt("verblüff","nett",adverb,TOUCH);

    /**************** Flippen ***************/
    case "flipp":
    ParseRest(str);
    if (str && !adverb)
      Return("Ausflippen wollen wir also, so so. "
          "Und wie, wenn ich fragen darf?\n");
    out_sel="Du flippst"+(adverb ? "@@adverb@@ aus." : " total aus.");
    out_oth="@@gname@@ flippt"+(adverb ? "@@adverb@@ aus." : " total aus.");
    return FeelIt("flipp","albern",adverb,0);

    /**************** Fluchen ***************/
    case "fluch":
    GHOSTCHECK(
        "Du fängst mangels Resonanzkörper leise an zu fluchen.\n",
        gname()+" fängt leise an zu fluchen. Laut kann er nicht,\n"
        "mangels Luft und Resonanzkörper.\n", 0);
    if (!str) {
      out_sel="Du fluchst lautstark.";
      out_oth="@@Name@@ faengt fürchterlich an zu fluchen.";
    }
    else {
      ParseRest(str);
      if (!adverb)
	Return("Wie willst Du fluchen?\n");
      out_sel="Du fluchst@@adverb@@.";
      out_oth="@@Name@@ flucht auf einmal@@adverb@@.";
    }
    return FeelIt("fluch","ärgerlich",adverb);

    /**************** Fragen ***************/
    case "frag":
    if (!str)
      Return("Frage wen was?\n");
    ParseRest(str);
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
			 "Du fängst zu grübeln an...",
			 "Frage jemand, der lebt."))
      return 1;
    if (who)
      sscanf(str,"%s %s", str1,str1);
    else
      str1=str;
    if (!str1)
      Return("Frage "+who->name(WEN)+" WAS?\n");
    str1=capitalize(str1);
    if (str1[<1] != '?')
      str1 += "?";
    out_sel="Du fragst@@ wen@@: "+str1;
    if (objectp(QueryProp(P_PERM_STRING)))
      str1 = call_other(QueryProp(P_PERM_STRING),"permutate_string",str1)||"";
    if (who) out_vic="@@gname@@ fragt Dich: "+str1;
    out_oth="@@gname@@ fragt@@ wen@@: "+str1;
    return FeelIt("frag","neutral",adverb,0);

    /**************** Freuen ***************/
    case "freu":
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Freue Dich wie?\n");
    out_sel="Du "+(who ? "grinst @@wen@@ an und " : "")+
      "freust Dich@@adverb@@.";
    if (who) out_vic="@@gname@@ grinst Dich an und freut sich@@adverb@@.";
    out_oth="@@gname@@ "+(who ? "grinst @@wen@@ an und " : "")+
      "freut sich@@adverb@@.";
    return FeelIt("freu","froh",adverb,0);

    /**************** Frieren ***************/
    case "frier":
    GHOSTCHECK("Du bist selber sehr kalt, aber frieren tust Du trotzdem "
        "nicht!\n", "",0);
    ParseRest(str);
    out_sel="Du schlägst Deine Hände um Deinen Körper und stampfst mit "
        "den Füßen auf. Du frierst einfach"+
        (adverb ? "@@adverb@@.":" jämmerlich.");
    out_oth="@@gname@@ schlägt "+QPP(FEMALE, WEN,PLURAL)+" Hände um den"
        " Körper und stampft mit den Füßen auf. "
        "@@gname@@ friert einfach"+(adverb ? "@@adverb@@." : " jämmerlich.");
    return FeelIt("frier","neutral",adverb,0);

    /**************** Fuchteln ***************/
    case "fuchtel":
    case "fuchtl":
    ParseRest(str);
    out_sel="Du fuchtelst"+(adverb ? "@@adverb@@" : (QueryProp(P_GENDER) ==
      FEMALE ? "wie eine Blöde" : " wie ein Blöder") )+" in der Gegend rum.";
    out_oth="@@gname@@ fuchtelt"+(adverb ? "@@adverb@@" : (QueryProp(P_GENDER)
      == FEMALE ? "wie eine Blöde" :" wie ein Blöder"))+" in der Gegend rum.";
    return FeelIt("fuchtel","neutral",adverb,0);

    /**************** Fuerchten *****************/
    case "fuercht":
    case "fürcht":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du fürchtest Dich"+(adverb ? "@@adverb@@ " : " ")+
              "vor @@alle@@.";
      out_vic="@@name@@ fürchtet sich" +(adverb ? "@@adverb@@ " : " ")+
              "vor @@alle@@.";
      return MixedOut(WEN,"fürcht","ängstlich",adverb,0);
    }
    if(who && CheckLife(NOT_SELF,NOT_DEAD,
          "Vor Dir selbst brauchst Du Dich ja nicht zu fürchten!",
          "Davor brauchst Du Dich nicht fürchten, das ist ja schon tot!"))
      return 1;
    out_sel="Du fürchtest Dich"+(adverb ?"@@adverb@@": "")+
          (who ? " vor "+who->name(WEM) : "")+".";
    if(who) out_vic="@@gname@@ fürchtet sich"+(adverb ? "@@adverb@@" : "")+
            " vor Dir.";
    out_oth="@@gname@@ fürchtet sich"+(adverb ?"@@adverb@@": "")+
          (who ? " vor "+who->name(WEM) : ".");
    return FeelIt("fürcht","ängstlich",adverb,0);

    /**************** Furzen ***************/
    case "furz":
    if (ghost())
      GHOST("Du lässt einen fahren - aber er riecht nach gar nix.\n",
            gname()+" lässt einen fahren. Man riecht aber nix.\n", 0);
    if ((time()-last_furz_time)>20)
    {
      ParseRest(str);
      if (str && !adverb)
        Return("Furze wie?\n");
      out_sel="Du furzt"+(adverb ? "@@adverb@@." : " hemmungslos.");
      out_oth="@@Name@@ lässt@@adverb@@ einen Stinkefurz fahren.";
      ofoo=clone_object("/obj/misc/furz");
      ofoo->set_furzer(this_player());
      ofoo->move(environment(this_player()));
      last_furz_time=time();
      return FeelIt("furz","frech",adverb,0);
    }
    else
      Return("Du kannst so kurz nacheinander nicht furzen.\n");

    /**************** Gaehnen ***************/
    case "gaehn":
    case "gähn":
    if (ghost())
      Return("Als Geist wirst Du nicht müde - also nicht gähnen.\n");
    ParseRest(str);
    if (str && !adverb)
      Return("Wie willst Du gähnen?\n");
    if (!adverb)
      out_sel="Kannst Du aber Dein(en) Mund/Maul/Schnabel weit aufreißen!";
    else
      out_sel="Du gähnst@@adverb@@.";
    out_oth="@@Name@@ gähnt@@adverb@@.";
    return FeelIt("gähn","neutral",adverb,0);

    /**************** Gackern ***************/
    case "gacker":
    ParseRest(str);
    out_sel="Du gackerst"+(adverb ? "@@adverb@@.":" wie ein Huhn.");
    out_oth="@@gname@@ gackert"+(adverb ? "@@adverb@@." : " wie ein Huhn.");
    return FeelIt("gacker","albern",adverb,0);

    /**************** Glucksen ***************/
    case "glucks":
    ParseRest(str);
    if (str && !adverb)
      Return("Gluckse wie?\n");
    out_sel="Du gluckst"+(adverb ? "@@adverb@@." : " wie ein Huhn.");
    out_oth="@@gname@@ gluckst"+(adverb ? "@@adverb@@." : " wie ein Huhn.");
    return FeelIt("glucks","albern",adverb,0);

   /***************** Glubschen ****************/
    case "glubsch":
    if(ghost())
      Return("Willst Du, das Deine Augen wegfliegen?\n");
    ParseRest(str);
    if (for_all)
    {
      out_sel="Du glubscht @@alle@@@@adverb@@ ins Gesicht.";
      out_vic="@@name@@ glubscht @@alle@@@@adverb@@ ins Gesicht.";
      return MixedOut(WEN,"glubsch","frech",adverb,0);
    }
    if (str && !adverb && !who )
      Return("Wen willst Du Wie anglubschen?\n");
    if(who && CheckLife(NOT_SELF,NOT_DEAD,
         "Wie willst Du das denn bitte machen?",
         "Das ist es gar nicht wert angeglubscht zu werden."))

      return 1;
    out_sel="Du glubscht@@ wen@@@@adverb@@"+(who ? " ins Gesicht" :
            " in der Gegend rum")+".";
    if (who) out_vic="@@name@@ glubscht Dir@@adverb@@ ins Gesicht.";
    out_oth="@@name@@ grinst@@ wem@@@@adverb@@"+(who ? " ins Gesicht" :
            " in der Gegend rum")+".";
    return FeelIt("glubsch","frech",adverb,0);

    /**************** Gratulieren ***************/
    case "gratulier":
    case "beglueckwuensch":
    case "beglückwünsch":
    ParseRest(str);
    if (!who)
      Return("Wem willst Du gratulieren?\n");
    if (CheckLife(NOT_SELF, NOT_DEAD,
        "Na, meinst Du nicht, dass Eigenlob stinkt?",
        "Soll ich dem Ding vielleicht zum Totsein gratulieren? Nee nee."))
      return 1;
    out_sel="Du gratulierst @@wem@@@@adverb@@.";
    out_vic="@@gname@@ gratuliert Dir@@adverb@@.";
    out_oth="@@gname@@ gratuliert @@wem@@@@adverb@@.";
    return FeelIt("gratulier","nett",adverb,0);

    /**************** Grinsen ***************/
    case "grins":
    GHOSTCHECK("Als Du grinst, siehst Du regelrecht, wie die anderen eine "
        "Gänsehaut bekommen.\n",
        "Du bekommst eine Gänsehaut, als der Geist von "+name(WEM)+
        " zu grinsen anfängt.\n", 0);
    ParseRest(str);
    if (for_all)  {
      out_sel="Du grinst @@alle@@@@adverb@@ an.";
      out_vic="@@Name@@ grinst @@alle@@@@adverb@@ an.";
      return MixedOut(WEN,"grins","frech",adverb,0);
    }
    if (!who && !adverb && str)
      Return("Grinsen - schön und gut. Aber wen oder wie (an)grinsen?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Du grinst Dir was, aber so, dass es kein anderer sieht.",
        "Nicht mal einen Spiegel darf man hier angrinsen, nur Lebewesen!"))
      return 1;
    out_sel="Du grinst@@ wen@@@@adverb@@"+(who ? " an" : "")+".";
    if (who) out_vic="@@Name@@ grinst Dich@@adverb@@ an.";
    out_oth="@@Name@@ grinst@@ wen@@@@adverb@@"+(who ? " an" : "")+".";
    return FeelIt("grins","frech",adverb,0);

    /***************** Groehlen *****************/
    case "groehl":
    case "gröhl":
    ParseRest(str);
    if(str && !adverb)
      Return("Wie willst Du gröhlen?\n");
    out_sel="Du gröhlst@@adverb@@ die schlimmsten Trinkerlieder, die Dir "
            "einfallen.";
    out_oth="@@name@@ gröhlt@@adverb@@ schrecklich klingende Lieder durch "
            "die Gegend.";
    return FeelIt("gröhl","albern",adverb,0);

    /**************** Gruebeln ***************/
    case "gruebel":
    case "gruebl":
    case "grübel":
    case "grübl":
    ParseRest(str);
    if (str && !adverb)
      Return("Grüble wie?\n");
    out_sel="Du grübelst@@adverb@@ eine Weile vor Dich hin.";
    out_oth="@@gname@@ grübelt@@adverb@@ eine Weile vor sich hin.";
    return FeelIt("grübel","neutral",adverb,0);

    /**************** Grummeln ***************/
    case "grummel":
    case "grummle":
    ParseRest(str);
    if (for_all)  {
      out_sel="Du grummelst @@alle@@@@adverb@@ an.";
      out_vic="@@Name@@ grummelt @@alle@@@@adverb@@ an.";
      return MixedOut(WEN,"grummel","ärgerlich",adverb,0);
    }
    if (!who && !adverb && str)
      Return("Grummle wen an oder wie?\n");
    out_sel="Du grummelst@@ wen@@@@adverb@@"+(who ? " an" : "")+".";
    if (who) out_vic="@@gname@@ grummelt Dich@@adverb@@ an.";
    out_oth="@@gname@@ grummelt@@ wen@@@@adverb@@"+(who ? " an" : "")+".";
    return FeelIt("grummel","ärgerlich",adverb,0);

    /**************** Grunzen ***************/
    case "grunz":
    ParseRest(str);
    if (str && !adverb)
      Return("Grunze wie?\n");
    out_sel="Du grunzt@@adverb@@.";
    out_oth="@@gname@@ grunzt@@adverb@@.";
    return FeelIt("grunz","albern",adverb);

    /**************** Gucken ***************/
    case "guck":
    ParseRest(str);
    if (!adverb)
      Return("Gucke wie aus der Wäsche?\n");
    out_sel="Du guckst@@adverb@@ aus der Wäsche.";
    out_oth="@@gname@@ guckt@@adverb@@ aus der Wäsche.";
    return FeelIt("guck","albern",adverb,0);

    /***************** Haenseln ******************/
    case "haensel":
    case "haensl":
    case "hänsel":
    case "hänsl":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du hänselst @@alle@@"+(adverb ? " so richtig@@adverb@@":"")+".";
      out_vic="@@name@@ hänselt @@alle@@"+(adverb ? " so richtig@@adverb@@":
              "")+".";
      return MixedOut(WEN,"hänsel","fies",adverb,0);
    }
    if(str && (!adverb && !who))
      Return("Wen willst Du Wie hänseln?\n");
    if(CheckLife(0,NOT_DEAD,0,"Soweit kommt es noch, tote Sachen hänseln!"))
      return 1;
    if(who==this_player())
      who=0;
    out_sel="Du hänselst"+(who ? " "+who->name(WEN):" Dich")+(adverb ?
            " so richtig@@adverb@@":"")+".";
    if(!who) out_sel+=" Naja, wer sowas braucht...";
    if(who) out_vic="@@name@@ hänselt Dich"+(adverb ? " so richtig@@adverb@@":
                    "")+".";
    out_oth="@@name@@ hänselt "+(who ? who->name(WEN):"sich selber")+
            (adverb ? " so richtig@@adverb@@":"")+".";
    if(!who) out_oth+="\n@@name@@ braucht wohl seine Depression.";
    return FeelIt("hänsel","fies",adverb,0);

    /**************** Haetscheln ***************/
    case "haetschel":
    case "haetschl":
    case "hätschel":
    case "hätschl":
    GHOSTCHECK("Du überlegst es Dir anders - mit Deinen durchlässigen "
        "Händen...\n",
        gname()+" will anscheinend jemand hätscheln, überlegt\n"
	"es sich nach einem kurzen Blick auf seine Hände anders.\n", 0);
    ParseRest(str);
    if (for_all)  {
      out_sel="Du hätschelst @@alle@@@@adverb@@.";
      out_vic="@@Name@@ hätschelt @@alle@@@@adverb@@.";
      return MixedOut(WEN,"hätschel","nett",adverb,0);
    }
    if (!str || !who)
      Return("Wen willst Du hätscheln?\n");
    if (who && CheckLife(NOT_SELF, NOT_DEAD,
        "Das sieht viel zu albern aus - Du lässt es bleiben.",
        "Ist da überhaupt was zu hätscheln? - Nein, da lebt doch nix."))
      return 1;
    out_sel="Du hätschelst@@ wen@@@@adverb@@.";
    out_vic="@@Name@@ hätschelt Dich@@adverb@@.";
    out_oth="@@Name@@ hätschelt@@ wen@@@@adverb@@.";
    return FeelIt("hätschel","nett",adverb,0);

    /**************** Hauen *****************/
    case "hau":
    if (ghost())
      GHOST("Du hast keinen Körper, mit dem Du um Dich schlagen "
            "könntest.\n","",0);
    ParseRest(str);
    if (!who) Return("Wen willst Du hauen?\n");
    if (CheckLife(NOT_SELF, NOT_DEAD,
      "Bist Du irgendwie masochistisch veranlagt?",
      "Du haust dem Ding voll auf die Nase. Zumindest glaubst\n"+
         "Du, dass es eine Nase war."))
      return 1;
    out_sel="Du haust @@wen@@@@adverb@@.";
    out_vic="@@name@@ haut Dich@@adverb@@.";
    out_oth="@@name@@ haut @@wen@@@@adverb@@.";
    return FeelIt("hau","fies",adverb,TOUCH);

    /****************** Hecheln ****************/
    case "hechel":
    case "hechl":
    ParseRest(str);
    if(str && !adverb)
      Return("Wie willst Du hecheln?\n");
    out_sel="Du hechelst und hechelst "+(adverb ? "total@@adverb@@ ":"")+
            "und kommst ganz außer Atem.\n";
    out_sel+="(Und denke dran, NICHT pressen!)";
    out_oth="@@name@@ hechelt sich "+(adverb ? "total @@adverb@@ ":"")+
            "völlig außer Atem.\n";
    out_oth+="Wozu das wohl gut sein soll?!";
    return FeelIt("hechel","neutral",adverb,0);

    /**************** Hicksen ***************/
    case "hicks":
    GHOSTCHECK("Hoppla! Dieser Hickser zieht Dich ganz schön zusammen!\n",
        gname()+" hat anscheinend Schluckauf.\n"
        "Und was für einen! Für einen Moment zieht es "+
        QueryPronoun(WEN)+" ziemlich zusammen.\n", 0);
    if (!str)  {
      out_sel="Hicks!";
      out_oth="@@Name@@ muss hicksen. Wahrscheinlich zuviel Alkohol...";
    }
    else  {
      ParseRest(str);
      if (!adverb)
	Return("Hickse wie?\n");
      out_sel="Du hickst@@adverb@@.";
      out_oth="@@Name@@ hickst@@adverb@@.";
    }
    return FeelIt("hicks","albern",adverb,0);

    /*************** Hoeren *****************/
    case "hoer":
    case "hör":
    ParseRest(str);
    if(for_all)
      Return("Du kannst aber nicht allen gleichzeitig zuhören!\n");
    if(CheckLife(NOT_SELF,NOT_DEAD,
        "Ergibt wenig Sinn, oder?",
        "Tote Dinge reden sowieso nicht."))
      return 1;
    out_sel="Du hörst"+(who ? " "+who->name(WEM):"")+"@@adverb@@ zu.";
    if(who) out_vic="@@name@@ hört Dir@@adverb@@ zu.";
    out_oth="@@name@@ hört"+(who ? " "+who->name(WEM):"")+"@@adverb@@ zu.";
    return FeelIt("hör","neutral",adverb,0);

    /**************** Huepfen ***************/
    case "huepf":
    case "hüpf":
    GHOSTCHECK("Du schwebst durch die Gegend.\n",
        gname()+" schwebt durch die Gegend.\n", 0);
    if (!str)  {
      out_sel="B O I N G !! Du hüpfst in der Gegend herum.";
      out_oth="@@Name@@ hüpft in der Gegend herum.";
    }
    else  {
      ParseRest(str);
      if (!who && !adverb)
	Return("Hüpfe wie oder um wen oder wie oder was oder häh?\n");
      out_sel="Du hüpfst@@adverb@@"+(who ? " um@@ wen@@" : "")+" herum.";
      if (who) out_vic="@@Name@@ hüpft@@adverb@@ um Dich herum.";
      out_oth="@@Name@@ hüpft@@adverb@@"+(who ? " um@@ wen@@" : "")+" herum.";
    }
    return FeelIt("hüpf","albern",adverb,0);

    /*************** Huesteln **************/
    case "huest":
    case "huestel":
    case "huestl":
    case "hüst":
    case "hüstel":
    case "hüstl":

    ParseRest(str);
    if(str && !adverb)
      Return("Wie willst Du hüsteln?\n");
    out_sel="Du hüstelst"+(adverb ? "@@adverb@@":" verlegen")+".";
    out_oth="@@name@@ hüstelt"+(adverb ? "@@adverb@@":" verlegen")+".";
    return FeelIt("hüstel","neutral",adverb,0);

    /**************** Husten ***************/
    case "hust":
    GHOSTCHECK("Du verstreust ein paar Geisterbazillen im Raum.\n",
        gname()+" macht ufff, ufff und verteilt ein paar Geister-\n"
        "bazillen im Raum.\n", 0);
    if (!str)  {
      out_sel="Hust! Keuch! Halt dir doch wenigstens die Hand vor den Mund!";
      out_oth="@@Name@@ hustet sich fast die Seele aus dem Leib.";
    }
    else  {
      ParseRest(str);
      if (!who && !adverb)
	Return("Wenn Du schon was hinter huste tippst, "
            "dann bitte was vernünftiges!\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
          "Dir selber können nur andere was husten.",
          "Bitte huste nur Lebewesen was."))
	return 1;
      out_sel="Du hustest@@ wem@@@@adverb@@"+(who? " was" : "")+".";
      if (who) out_vic="@@Name@@ hustet Dir@@adverb@@ was.";
      out_oth="@@Name@@ hustet@@ wem@@@@adverb@@"+(who? " was" : "")+".";
    }
    return FeelIt("hust","fies",adverb,0);

    /**************** Idlen ***************/
    case "idl":
    ParseRest(str);
    if (str && !adverb)
      Return("Idle wie?\n");
    out_sel="Du idlest"+(adverb ? "@@adverb@@." : " vor Dich hin.");
    out_oth="@@gname@@ idlet"+(adverb ? "@@adverb@@." : " vor sich hin.");
    return FeelIt("idl","neutral",adverb,0);

    /**************** Jammern ***************/
    case "jammer":
    GHOSTCHECK("Du jammerst über Dein Schicksal als Geist.\n",
      gname()+" jammert über "+QPP(NEUTER, WEN)+ " Schicksal als Geist.\n",
      0);
    ParseRest(str);
    out_sel="Du jammerst@@adverb@@.";
    out_oth="@@gname@@ jammert"+(adverb ? "@@adverb@@." :
            " in Selbstmitleid zerfließend.");
    return FeelIt("jammer","traurig",adverb,0);

    /**************** Japsen ***************/
    case "japs":
    ParseRest(str);
    out_sel="Du japst"+(adverb ? "@@adverb@@." : " entgeistert.");
    out_oth="@@gname@@ japst"+(adverb ? "@@adverb@@." : " entgeistert.");
    return FeelIt("japse","neutral",adverb,0);

    /**************** Jaulen ***************/
    case "jaul":
    ParseRest(str);
    if (str && !adverb)
      Return("Jaule wie?");
    if (str)
    {
      out_sel="Du jaulst@@adverb@@.";
      out_oth="@@name@@ jault@@adverb@@.";
    }
    else
    {
      out_sel="Du jaulst wie ein Köter.";
      out_oth="@@name@@ jault wie ein Köter.";
    }
    return FeelIt("jaul","albern",adverb,0);

    /**************** Jubeln ***************/
    case "jubel":
    case "jubl":
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Juble wie? Oder wem zu?\n");
    out_sel="Du jubelst@@ wem@@@@adverb@@"+(who ? " zu." : ".");
    if (who) out_vic="@@gname@@ jubelt Dir@@adverb@@ zu.";
    out_oth="@@gname@@ jubelt@@ wem@@@@adverb@@"+(who ? " zu." : ".");
    return FeelIt("jubel","froh",adverb,0);

    /**************** Keuchen ***************/
    case "keuch":
    if (ghost())
      Return("Als Geist strengt Dich nix an - "
          "also wird auch nicht gekeucht.\n");
    ParseRest(str);
    if (str && !adverb)
      Return("Keuche wie?\n");
    out_sel="Du keuchst"+(adverb ? "@@adverb@@." : " vor Anstrengung.");
    out_oth="@@Name@@ keucht"+(adverb ? "@@adverb@@." : " vor Anstrengung.");
    return FeelIt("keuch","neutral",adverb,0);

    /**************** Kichern ***************/
    case "kicher":
    if (!str)  {
      out_sel="Du kicherst. (Wie albern von Dir)";
      out_oth="@@gname@@ gibt ein albernes Kichern von sich.";
    }
    else  {
      ParseRest(str);
      if (!who && !adverb)
	Return("Das haut so nicht hin, gib vernünftige Parameter.\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
          "In diesem Fall nimm bitte nur kicher.",
          "Musst schon etwas Lebendes angeben, nichts Totes."))
	return 1;
      out_sel="Du kicherst@@adverb@@"+
        (who ? " hinter "+who->name(WESSEN)+" Rücken." : ".");
      if (who) out_vic="Jemand kichert@@adverb@@ hinter Deinem Rücken.";
      out_oth="@@gname@@ kichert@@adverb@@"+
        (who ? " hinter "+who->name(WESSEN)+" Rücken." : ".");
    }
    return FeelIt("kicher","albern",adverb,0);

    /**************** Kitzeln ***************/
    case "kitzel":
    case "kitzl":
    GHOSTCHECK("Mit Deinen immateriellen Fingern schaffst Du das nicht.\n",
        gname()+" muss gerade feststellen, dass man mit\n"
        "immateriellen Fingern nicht kitzeln kann.\n", 0);
    ParseRest(str);
    if (!who)
      Return("Wen willst Du kitzeln?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Du bist doch kein Masochist! Du lässt es bleiben.",
        "Dinge sind so selten kitzlig. Lass es bleiben."))
      return 1;
    /*
    if (member(({"highlander","hobo","trest","woelkchen"}),
               who->query_real_name())>-1)
      switch (who->query_real_name())
      {
        case "highlander": str1="unter"; str2="Federn"; break;
        case "hobo":       str1="an";    str2="Kinn"; break;
        case "trest":
        case "woelkchen":  str1="an"; str2="Fuessen"; t_g=MALE; t_n=PLURAL;
      }
    else
    */
    if (who->QueryProp(P_RACE))
      switch (lower_case(who->QueryProp(P_RACE)))
      {
        case "fisch"   :
        case "echse"   :
        case "drache"  : str1="unter"; str2="Schuppen";
                         t_g=FEMALE; t_n=PLURAL; break;
        case "vogel"   :
        case "greif"   :
        case "erzgreif": str1="unter"; str2="Federn";
                         t_g=FEMALE; t_n=PLURAL; break;
        case "hobbit"  : str1="an"; str2="Pelzfüßen";
                         t_g=MALE; t_n=PLURAL; break;
        default        : str1="an"; str2="Kinn";
                         t_g=NEUTER; t_n=SINGULAR;
      }
    else {
      str1="an"; str2="Kinn"; t_g=NEUTER; t_n=SINGULAR;
    }
    out_sel="Du kitzelst@@ wen@@@@adverb@@ "+str1+" "+who->QPP(t_g,WEM,t_n)+
      " "+str2+".\n@@wer@@ versucht, sich zu beherrschen, "
      "muss aber doch lachen.";
    out_vic="@@Name@@ kitzelt Dich@@adverb@@ "+str1+" Deine"+
      (t_n ? "n" : (t_g==FEMALE ? "r" : "m"))+" "+str2+
      ".\nDu versuchst, Dich zu beherrschen, musst aber doch lachen.";
    out_oth="@@Name@@ kitzelt@@ wen@@@@adverb@@ "+str1+" "+
      who->QPP(t_g,WEM,t_n)+" "+str2+
      ".\n@@wer@@ versucht, sich zu beherrschen, muss aber doch lachen.";
    return FeelIt("kitzel","albern",adverb,TOUCH);

    /**************** Klatschen ***************/
    case "klatsch":
    GHOSTCHECK("Deine Hände sausen durcheinander durch.\n",
        gname()+" will in die Hände klatschen - aber sie sausen\n"
        "durcheinander durch.\n", 0);
    ParseRest(str);
    if (!adverb && str)
      Return("Klatsche wie?\n");
    out_sel="Du klatschst"+(adverb ? "@@adverb@@" : " begeistert")+
      " in die Hände.";
    out_oth="@@Name@@ klatscht"+(adverb ? "@@adverb@@" : " begeistert")+
      " in die Hände.";
    return FeelIt("klatsch","neutral",adverb,0);

    /**************** Schulterklopfen ***************/
    case "klopf":
    GHOSTCHECK("Du kannst als Geist niemandem auf die Schulter klopfen!\n",
               "",0);
    ParseRest(str);
    if (!who) Return("Wem willst Du denn auf die Schulter klopfen?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
      "Das geht doch nicht.",
      "Nein, das macht keinen Spaß. Lebt ja nicht mal."))
        return 1;
    out_sel="Du klopfst " + who->name(WEM) + "@@adverb@@ auf die Schulter.";
    out_vic="@@gname@@ klopft Dir@@adverb@@ auf der Schulter herum.";
    out_oth="@@gname@@ klopft "+ who->name(WEM) + "@@adverb@@ auf die "
            "Schulter.";
    return FeelIt("klopf","neutral",adverb,TOUCH);

    /**************** Knabbern ***************/
    case "knabber":
    if (ghost())
      Return("Sorry, aber dafür fehlt Dir im Moment der nötige "
          "\"Biss\"...\n");
    ParseRest(str);
    if (!who)
      Return("Knabbere wen an?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
        "Du kommst nicht an dein eigenes Ohr ran...",
        "Nö, nö, das schmeckt bestimmt nicht gut."))
      return 1;
    out_sel="Du knabberst@@adverb@@ an "+who->name(WESSEN)+" Ohr.";
    out_vic="@@Name@@ knabbert@@adverb@@ an Deinem Ohr.";
    out_oth="@@Name@@ knabbert@@adverb@@ an "+who->name(WESSEN)+" Ohr.";
    return FeelIt("knabber","intim",adverb,TOUCH);

    /******************** Kneifen *****************/
    case "kneif":
    if(ghost())
      Return("Leider scheitert Dein Vorhaben an Deinem momentanen Zustand.\n");
    ParseRest(str);
    if(str && !adverb && !who)
      Return("Wen willst Du Wie kneifen?\n");
    if(CheckLife(0,NOT_DEAD,0,
           "Das würde wohl wenig bringen, meinst Du nicht?"))
      return 1;
    if(!who || who==this_player())
    {
      out_sel="Du kneifst Dich@@adverb@@ in den Arm. *Aua*";
      out_oth="@@name@@ kneift sich@@adverb@@ in den Arm.";
    }
    else
    {
      out_sel="Du kneifst "+who->name(WEN)+"@@adverb@@ in den Arm.";
      out_vic="@@name@@ kneift Dich@@adverb@@ in den Arm. *Aua*";
      out_oth="@@name@@ kneift "+who->name(WEN)+"@@adverb@@ in den Arm.";
    }
    return FeelIt("kneif","frech",adverb, TOUCH);

    /**************** Knicksen ***************/
    case "knicks":
    GHOSTCHECK("Du knickst in der Mitte ab, kriegst Dich aber schnell "
        "wieder zusammen.\n",
        gname()+" knick(s)t in der Mitte ab, kriegt sich aber\n"
        "zum Glück schnell wieder zusammen.\n", 0);
    if (!str) {
      out_sel="Du machst einen anmutigen Knicks.";
      out_oth="@@Name@@ macht einen anmutigen Knicks.";
    }
    else  {
      ParseRest(str,"vor");
      if (for_all)  {
	out_sel="Du knickst@@adverb@@ vor @@alle@@.";
	out_vic="@@Name@@ knickst@@adverb@@ vor @@alle@@.";
	return MixedOut(WEM,"knicks","neutral",adverb,0);
      }
      if (!who && !adverb)
	Return("Knickse irgendwie oder vor jemandem.\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
          "Wie willst Du das denn schaffen?",
          "Vor Sachen wird hier nicht geknickst!"))
	return 1;
      out_sel="Du knickst@@adverb@@"+(who ? " vor" : "")+"@@ wen@@.";
      if (who) out_vic="@@Name@@ knickst@@adverb@@ vor Dir.";
      out_oth="@@Name@@ knickst@@adverb@@"+(who ? " vor" : "")+"@@ wen@@.";
    }
    return FeelIt("knicks","neutral",adverb,0);

    /******************Knie (nieder)************/
    case "knie":
    case "kniee":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du kniest vor @@alle@@@@adverb@@ nieder.";
      out_vic="@@name@@ kniet vor @@alle@@@@adverb@@ nieder.";
      return MixedOut(WEM,"knie","neutral",adverb,0);
    }
    if(str && str!="nieder" && !adverb && !who)
      Return("Vor Wem willst Du Wie niederknien?\n");
    if(CheckLife(NOT_SELF,0,
        "Das musst Du erstmal vormachen! ;-)",0))
      return 1;
    out_sel="Du kniest@@adverb@@"+(who?" vor "+who->name(WEM):"")+" nieder.";
    if(who) out_vic="@@name@@ kniet@@adverb@@ vor Dir nieder.";
    out_oth="@@name@@ kniet@@adverb@@"+(who?" vor "+who->name(WEM):"")+
            " nieder.";
    return FeelIt("knie","neutral",adverb,0);

    /**************** Knirschen ***************/
    case "knirsch":
    if (ghost())
      Return("Du kannst mit nichts knirschen, so als Geist. "
          "Versuche doch dafür\nmal, zu rasseln...\n");
    ParseRest(str);
    if (str && !adverb)
      Return("Knirsche wie?\n");
    switch (lower_case(QueryProp(P_RACE)))  {
      case "vogel"  :
      case "greif"  : str1="dem Schnabel."; break;
      case "katze"  :
      case "vampir" :
      case "drache" : str1="den Fangzähnen."; break;
      default       : str1="den Zähnen.";
    }
    out_sel="Du knirscht@@adverb@@ mit "+str1;
    out_oth="@@Name@@ knirscht@@adverb@@ mit "+str1;
    return FeelIt("knirsch","ärgerlich",adverb);

    /**************** Knuddeln ***************/
    case "knuddel":
    case "knuddl":
    if (ghost())
      Return("Sorry, nicht als Geist.\n");
    ParseRest(str);
    if (for_all)  {
      out_sel="Du knuddelst @@alle@@@@adverb@@.";
      out_vic="@@name@@ knuddelt @@alle@@@@adverb@@.";
      return MixedOut(WEN,"knuddel","nett",adverb,TOUCH);
    }
    if (!who)
      Return("Knuddle wen?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
        "Das bringt doch nix, lass es halt.",
        "Du kannst nur Lebewesen knuddeln."))
      return 1;
    out_sel="Du knuddelst@@ wen@@@@adverb@@.";
    out_vic="@@Name@@ knuddelt Dich@@adverb@@.";
    out_oth="@@Name@@ knuddelt@@ wen@@@@adverb@@.";
    return FeelIt("knuddel","nett",adverb,TOUCH);

    /**************** Knuffen *******************/
    case "knuff":
    if(ghost())
      Return("Du bist ein Geist!\n");
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du knuffst @@alle@@@@adverb@@ in die Seite.";
      out_vic="@@name@@ knufft @@alle@@@@adverb@@ in die Seite.";
      return MixedOut(WEN,"knuff","frech",adverb,TOUCH);
    }
    if(str && !adverb && !who)
      Return("Wen willst Du wie knuffen?\n");
    if(!who)
      who=this_player();
    if(CheckLife(NOT_SELF,0,
        "Wen willst Du knuffen? Selber knuffen geht schlecht...",0))
      return 1;
    out_sel="Du knuffst "+who->name(WEM)+"@@adverb@@ in die Seite.";
    out_vic="@@name@@ knufft Dir@@adverb@@ in die Seite.";
    out_oth="@@name@@ knufft "+who->name(WEM)+"@@adverb@@ in die Seite.";
    return FeelIt("knuff","frech",adverb,TOUCH);

    /**************** Knurren ***************/
    case "knurr":
    ParseRest(str);
    if (for_all)  {
      out_sel="Du knurrst @@alle@@@@adverb@@ an.";
      out_vic="@@Name@@ knurrt @@alle@@@@adverb@@ an.";
      return MixedOut(WEN,"knurr","ärgerlich",adverb,0);
    }
    if (str && !who && !adverb)
      Return("Wen anknurren oder wie knurren?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Du knurrst in Dich hinein.",
        "Reagiert nicht. Solltest wohl besser Lebwesen anknurren."))
      return 1;
    out_sel="Du knurrst@@ wen@@@@adverb@@"+(who ? " an." : ".");
    if (who) out_vic="@@gname@@ knurrt Dich@@adverb@@ an.";
    out_oth="@@gname@@ knurrt@@ wen@@@@adverb@@"+(who ? " an." : ".");
    return FeelIt("knurr","ärgerlich",adverb,0);

    /****************  Knutschen ***************/
    case "knutsch":
    if (ghost())
      Return("Das kannst Du als Geist leider nicht. "
          "Irgendwie fehlt Dir dazu das Herz.\n");
    ParseRest(str);
    if (!who)
      Return("Knutsche wen ab?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
        "Das geht nicht.",
        "Igitt! Lieber nicht!"))
      return 1;
    out_sel=
      "Du gibst@@ wem@@@@adverb@@ einen tiefen und hingebungsvollen Kuss.";
    out_vic=
      "@@Name@@ gibt Dir@@adverb@@ einen tiefen und hingebungsvollen Kuss.\n"
      "Du schwebst im 7. Himmel.";
    out_oth=
      "@@Name@@ gibt@@ wem@@@@adverb@@ einen tiefen und hingebungsvollen Kuss.";
    return FeelIt("knutsch","intim",adverb,TOUCH);

    /**************** Kotzen ***************/
    case "kotz":
    if (ghost())
      Return("Ne, das ist eins von den Sachen, die als Geist nicht gehen.\n");
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Kotze wie? Kotze auf wen?\n");
    if (who && CheckLife(NOT_SELF, 0, "Igitt, nein danke."))
      return 1;
    if (!str) {
      out_sel="Du kotzt über Deine Schuhe.";
      out_oth="@@Name@@ verdreht die Augen und kotzt.";
    }
    else {
      out_sel="Du kotzt@@adverb@@"+(who ? " auf@@ wen@@." : ".");
      if (who) out_vic="@@Name@@ kotzt@@adverb@@ auf Dich.";
      out_oth="@@Name@@ kotzt@@adverb@@"+(who ? " auf@@ wen@@." : ".");
    }
    return FeelIt("kotz","fies",adverb,0);

    /****************** Krabbeln ****************/
    case "krabbel":
    case "krabbl":
    if(ghost())
      Return("Deine Finger dringen durch Deinen Körper hindruch.\n");
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du krabbelst @@alle@@ so richtig@@adverb@@ bis zum "
              +"Zusammenbrechen durch.";
      out_vic="@@name@@ krabbelt @@alle@@ so richtig@@adverb@@ durch und "
              +"Du brichst vor Lachen zusammen.";
      return MixedOut(WEN,"krabbel","albern",adverb,TOUCH);
    }
    if(str && !who && !adverb)
      Return("Wen willst Du Wie krabbeln?\n");
    if(!who)
      who=this_player();
    if(CheckLife(0,NOT_DEAD,0,"Was soll das denn bringen?"))
      return 1;
    if(who==this_player())
    {
      out_sel="Du krabbelst Dich so richtig@@adverb@@ durch und "
              +"brichst vor Lachen zusammen.\nMasochist!";
      out_oth="@@name@@ krabbelt sich so richtig@@adverb@@ durch und "
              +"bricht vor Lachen zusammen.\nSchlimm, diese Masochisten.";
    }
    else
    {
      out_sel="Du krabbelst "+who->name(WEN)+" so richtig@@adverb@@ durch und "
              +who->QueryPronoun(WER)+" bricht vor Lachen zusammen.";
      out_oth="@@name@@ krabbelt "+who->name(WEN)+" so richtig@@adverb@@ "
            "durch und "+who->QueryPronoun(WER)+" bricht vor Lachen zusammen.";
      out_vic="@@name@@ krabbelt Dich so richtig@@adverb@@ durch und "
              +"Du brichst vor Lachen zusammen.";
    }
    return FeelIt("krabbel","albern",adverb,TOUCH);

    /**************** Kratzen ***************/
    case "kratz":
    ParseRest(str);
    if (who && (who!=this_player()))
      Return("Das mach mal schoen nur mit Dir selber.\n");
    if (str && !adverb)
      Return("Wie willst Du Dich am Kopf kratzen?\n");
    out_sel="Du kratzt dich@@adverb@@ am Kopf.";
    out_oth="@@gname@@ kratzt sich@@adverb@@ am Kopf.";
    return FeelIt("kratz","neutral",adverb,0);

    /****************** Kraulen *****************/
    case "kraul":
    if(ghost())
      Return("Du versuchst es, greifst aber einfach durch Dich durch.\n");
    ParseRest(str);
    if(str && !who && !adverb)
      Return("Wen willst Du Wie kraulen?\n");
    if(CheckLife(0,NOT_DEAD,0,"Da hat Kraulen wohl nicht mehr viel Sinn!"))
      return 1;
    if(!who || who==this_player())
    {
      out_sel="Du kraulst Dich@@adverb@@ am Kinn.";
      out_oth="@@name@@ krault sich@@adverb@@ am Kinn.";
    }
    else
    {
      out_sel="Du kraulst "+who->name(WEN)+"@@adverb@@ am Nacken.";
      out_vic="@@name@@ krault Dich@@adverb@@ am Nacken.";
      out_oth="@@name@@ krault "+who->name(WEN)+"@@adverb@@ am Nacken.";
    }
    return FeelIt("kraul","nett",adverb,TOUCH);

    /**************** Kreischen ********************/
    case "kreisch":
    ParseRest(str);
    if(str && !adverb)
      Return("Wie willst Du kreischen?\n");
    out_sel="Du kreischt"+(adverb ? "@@adverb@@":" in Panik laut")+" auf.";
    out_oth="@@name@@ kreischt"+(adverb ? "@@adverb@@":" in Panik laut")+" auf.";
    return FeelIt("kreisch","ängstlich",adverb,0);

    /**************** Krieche ***************/
    case "kriech":
    ParseRest(str);
    out_sel="Du kriechst"+(who ? " vor @@wem@@" : "")+"@@adverb@@ im Staub.";
    if (who) out_vic="@@gname@@ kriecht@@adverb@@ vor Dir im Staub.";
    out_oth="@@gname@@ kriecht"+(who ? " vor @@wem@@" : "")+
      "@@adverb@@ im Staub.";
    return FeelIt("kriech","neutral",adverb,0);

    /**************** Kucken ***************/
    case "kuck":
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Wen oder wie kucken?\n");
    if (who && CheckLife(NOT_SELF, NOT_DEAD,
        "Dich selbst lieb ankucken? Das kannst Du nicht...",
        "Du kannst nur Lebewesen lieb ankucken!"))
      return 1;
    out_sel="Du kuckst@@ wen@@"+(adverb ? "@@adverb@@" : " lieb")+
      (who ? " an." : ".");
    if (who) out_vic="@@gname@@ kuckt Dich"+
      (adverb ? "@@adverb@@ an." : " lieb an.");
    out_oth="@@gname@@ kuckt@@ wen@@"+(adverb ? "@@adverb@@" : " lieb")+
      (who ? " an." : ".");
    return FeelIt("kuck","nett",adverb,0);

    /**************** Kuessen ***************/
    case "kuess":
    case "küss":
    if (ghost())
      Return("Als Geist kannst Du leider niemanden küssen.\n");
    ParseRest(str);
    if (!who)
      Return("Wen willst Du küssen?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
        "Da hast Du aber Schwierigkeiten... Du gibt es schließlich auf.",
        "Nix. Absolut nix. Küsse lieber Lebewesen - die reagieren\n"
        "wenigstens (und sei es, dass sie Dich fressen...)."))
      return 1;
    out_sel="Du küsst@@ wen@@@@adverb@@.";
    out_vic="@@Name@@ küsst Dich@@adverb@@.";
    out_oth="@@Name@@ küsst@@ wen@@@@adverb@@.";
    FeelIt("küss","intim",adverb,TOUCH);
    if (!QueryProp(P_FROG) && who->QueryProp(P_FROG) &&
        (QueryProp(P_LEVEL) > who->QueryProp(P_LEVEL))) {
      who->SetProp(P_FROG,0);
      tell_room(environment(this_player()),"\nP L O P P  ! ! !\n\n");
      write("Huch! Du wirst auf einmal so grün und klein und kriegst auf "
          "einmal\nfurchtbar Hunger auf Fliegen und so...\n");
      who->Message("Auf einmal wird die Welt um Dich wieder so klein, wie "
          "sie früher mal war -\n"
          "und vor Dir sitzt ein kleiner grüner Frosch.\n", MSGFLAG_SOUL);
      say(capitalize(who->name(WER))+" steht auf einmal da und schaut dumm "
          "aus der Wäsche. Dafür fehlt\n"
          "jetzt seltsamerweise "+name()+
          ". Die Gesamtzahl an kleinen grünen Fröschen\n"
          "im Raum hat sich jedoch nicht geändert...\n",
          ({ who,this_player() }) );
      SetProp(P_FROG,1);
    }
    return 1;

    /**************** Kuscheln ***************/
    case "kuschel":
    case "kuschl":
    GHOSTCHECK("Dazu bist Du als Geist viel zu kalt und gar schrecklich "
        "anzusehen.\n",
        gname()+" scheint Anwandlungen zu haben, sich an jemand ankuscheln "
        "zu wollen.\nEntsetzt springen alle zurück, weil dazu ist er doch "
        "zu kalt und schrecklich\nanzusehen.\n", 0);
    ParseRest(str);
    if (!who)
      Return("An wen willst Du Dich ankuscheln?\n");
    if (CheckLife(NOT_SELF, NOT_DEAD,
        "Kuschel Dich lieber an jemand anderen an, das ist viel schöner.",
        "Willst Du Dich wirklich an tote Dinge ankuscheln?"))
      return 1;
    out_sel="Du kuschelst Dich@@adverb@@ an @@wen@@ an.";
    out_vic="@@Name@@ kuschelt sich@@adverb@@ an Dich an.";
    out_oth="@@Name@@ kuschelt sich@@adverb@@ an @@wen@@ an.";
    return FeelIt("kuschel","intim",adverb,TOUCH);

    /**************** Lachen ***************/
    case "lach":
    GHOSTCHECK("Du lachst mit hohler Stimme. Den Umstehenden (sind doch "
        "welche da, oder?)\n"
        "läuft es kalt den Rücken runter.\n",
        gname()+" lacht mit hohler Stimme.\n"
        "Dir läuft es eiskalt den Rücken runter.\n", 0);
    if (!str) {
      out_sel="Du brichst vor Lachen zusammen.";
      out_oth="@@Name@@ bricht vor Lachen zusammen.";
    }
    else {
      ParseRest(str);
      if (for_all) {
	out_sel="Du lachst@@adverb@@ über @@alle@@.";
	out_vic="@@Name@@ lacht@@adverb@@ über @@alle@@.";
	return MixedOut(WEN,"lach","froh",adverb,0);
      }
      if (!who && !adverb)
	Return("Lache wie, lache ueber wen?\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
          "Lach Dich doch nicht selber aus - das machen schon andere...",
          "Gelacht wird nur über Lebewesen (die können sich drüber "
          "ärgern)."))
	return 1;
      out_sel="Du lachst@@adverb@@"+(who?" über@@ wen@@":"")+".";
      if (who) out_vic="@@Name@@ lacht@@adverb@@ über Dich.";
      out_oth="@@Name@@ lacht@@adverb@@"+(who?" über@@ wen@@":"")+".";
    }
    return FeelIt("lach","froh",adverb,0);

    /**************** Laecheln ***************/
    case "laechel":
    case "laechl":
    case "lächel":
    case "lächl":
    if (ghost())
      Return("Du lächelst innerlich.\n");
    if(!str) {
      out_sel="Du lächelst.";
      out_oth="@@Name@@ lächelt.";
    }
    else  {
      ParseRest(str);
      if (for_all)  {
	out_sel="Du lächelst @@alle@@@@adverb@@ an.";
	out_vic="@@Name@@ lächelt @@alle@@@@adverb@@ an.";
	return MixedOut(WEN,"laächel","nett",adverb,0);
      }
      if (!who && !adverb && str)
	Return("Wie oder wen?\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
          "Musst schon jemand anders anlächeln.",
          "Bitte ein Lebewesen anlächeln."))
	return 1;
      out_sel="Du lächelst@@ wen@@@@adverb@@"+(who ? " an." : ".");
      if (who) out_vic="@@Name@@ lächelt Dich@@adverb@@ an.";
      out_oth="@@Name@@ lächelt@@ wen@@@@adverb@@"+(who ? " an." : ".");
    }
    return FeelIt("lächel","nett",adverb,0);

    /**************** Lagen ***************/
    case "lag":
    ParseRest(str);
    if (str && !adverb)
      Return("Lage wie?\n");
    out_sel="Du lagst"+(adverb ? "@@adverb@@." : " vor Dich hin.");
    out_oth="@@gname@@ lagt"+(adverb ? "@@adverb@@." : " vor sich her.");
    return FeelIt("lag","neutral",adverb,0);

    /**************** Liebe ***************/
    // These lines appear Courtesy of Angus@MorgenGrauen. So long, and thanks
    // for all the fish, errr, text, Angus :)
    case "lieb":
    if (ghost())
      Return("Auf diese Freuden musst Du als Geist leider verzichten.\n");
    ParseRest(str);
    if (!who)
      Return("Wen hast Du lieb?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
        "Ja, ich weiß, Du magst Dich, aber das musst Du nicht zur Schau"
        "stellen.",
        "Du entwickelst seltsame Neigungen, finde ich."))
      return 1;
    /* old version:
       out_sel="Du fluesterst @@wem@@@@adverb@@ liebevolle Worte ins Ohr.";
       out_vic=gname()+" fluestert Dir@@adverb@@ liebevolle Worte ins Ohr.";
       out_oth=gname()+" fluestert@@adverb@@ sanfte Worte zu @@wem@@.";
       */
    out_sel="Du schließt die Augen, schmiegst Dich eng an @@wen@@ und gibst"
      +LF+who->QueryPronoun(WEM)+
      " einen zärtlichen und leidenschaftlichen Kuss. Um Dich herum\n"
      "versinkt die Welt und Du glaubst, auf Wolken zu schweben.";
    out_vic="@@Name@@ drückt Dich zärtlich an sich und gibt Dir einen "
      "zärtlichen\n"
      "und leidenschaftlichen Kuss. Du schließt die Augen und träumst ein\n"
      "wenig..... Du schwebst auf Wolken direkt in den siebten Himmel.";
    out_oth="Du schaust dezent weg, als sich @@name@@ und "+who->name()+
      " verliebt in die Arme\nsinken.";
    return FeelIt("lieb","intim",adverb,TOUCH);

    /**************** Loben ***************/
    case "lob":
    if (!str) Return("Wen oder was willst Du loben?\n");
    ParseRest(str);
    if (who==ME) {
      /*
      ofoo=clone_object("/obj/misc/furz");
      ofoo->set_furzer(this_player());
      ofoo->set_eigenlob();
      ofoo->move(environment(this_player()));
      */
      //DEBUG Furz zum Eigenlob patchen :>
      out_sel="Du lobst Dich selber@@adverb@@. Die Folgen kennst Du ja...";
      out_oth="@@gname@@ lobt sich selber@@adverb@@, mit den bekannten Folgen.";
    } else if (who) {
      out_sel="Du lobst @@wen@@@@adverb@@.";
      out_vic="@@gname@@ lobt Dich@@adverb@@.";
      out_oth="@@gname@@ lobt @@wen@@@@adverb@@.";
    } else {
      out_sel="Du lobst "+str+".";
      out_oth="@@gname@@ lobt "+str+".";
    }
    return FeelIt("lob","nett",adverb,0);

   /**************** Meckern *******************/
    case "mecker":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du meckerst @@alle@@@@adverb@@ an.";
      out_vic="@@name@@ meckert @@alle@@@@adverb@@ an.";
      return MixedOut(WEN,"mecker","fies",adverb,0);
    }
    if(str && !adverb && !who)
      Return("Wen willst Du Wie anmeckern?\n");
    if(CheckLife(NOT_SELF,NOT_DEAD,
        "Lass mal, das machen schon andere für Dich.",
        "Wieso willst Du tote Dinge anmeckern?"))
      return 1;
    out_sel="Du meckerst@@adverb@@ über "+(who ? who->name(WEN):
       "den Magier der diese Welt verbrochen hat")+".";
    if(who) out_vic="@@name@@ meckert@@adverb@@ über Dich.";
    out_oth="@@name@@ meckert über "+(who ? who->name(WEN):
       "den Magier der diese Welt verbrochen hat")+".";
    return FeelIt("mecker","fies",adverb,0);

    /**************** Murmeln ***************/
    case "murmel":
    case "murml":
    ParseRest(str);
    out_sel="Du murmelst"+
        (adverb ? ":@@adverb@@.":" etwas Unverständliches.");
    out_oth="@@gname@@ murmelt"+
        (adverb ? ":@@adverb@@." : " etwas Unverständliches.");
    return FeelIt("murmel","neutral",adverb,0);

    /**************** Mustern ***************/
    case "muster":
    ParseRest(str);
    if (!who)
      Return("Mustere wen?\n");
    if (CheckLife(NOT_SELF, 0,
        "Du willst Dich selber mustern? "
        "Lass das lieber, das sieht so albern aus.", 0))
      return 1;
    out_sel="Du musterst @@wen@@@@adverb@@.";
    out_vic="@@gname@@ mustert Dich@@adverb@@.";
    out_oth="@@gname@@ mustert @@wen@@@@adverb@@.";
    return FeelIt("muster","neutral",adverb,0);

    /**************** Nerven ******************/
    case "nerv":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du gehst @@alle@@ so richtig@@adverb@@ auf die Nerven.";
      out_vic="@@name@@ geht @@alle@@ so richtig@@adverb@@ auf die Nerven. "
              "Waaaah!!!";
      return MixedOut(WEM,"nerv","fies",adverb,0);
    }
    if(str && !adverb && !who)
      Return("Wen willst Du Wie nerven?\n");
    if(!who)
      who=this_player();
    if(CheckLife(NOT_SELF,NOT_DEAD,
       "Das machst Du doch so schon immer, lass es lieber!",
       "Toten Dingen kannst Du gar nicht mehr nerven!"))
      return 1;
    out_sel="Du nervst "+who->name(WEN)+" so richtig@@adverb@@.";
    out_oth="@@name@@ nervt "+who->name(WEN)+" so richtig@@adverb@@.";
    out_vic="@@name@@ nervt Dich so richtig@@adverb@@. Waaaah!!!";
    return FeelIt("nerv","fies",adverb,0);

    /**************** Nicken ***************/
    case "ja":
    case "nick":
    ParseRest(str);
    if (for_all) {
      out_sel="Du nickst @@alle@@@@adverb@@ zu.";
      out_vic="@@gname@@ nickt @@alle@@@@adverb@@ zu.";
      return MixedOut(WEM,"nick","neutral",adverb,0);
    }
    if (str && !who && !adverb)
      Return("Nicke wie oder wem zu oder wem wie zu?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Du willst Dir selber zunicken? "
        "Nein, lieber nicht, das sieht so albern aus.",
        "Hm. Nix passiert. Von Lebewesen bekommt man "
        "im allgemeinen mehr Feedback."))
      return 1;
    out_sel="Du nickst@@ wem@@@@adverb@@"+
      (who ? " zu." : (adverb ? "." : " zustimmend."));
    if (who) out_vic="@@gname@@ nickt Dir@@adverb@@ zu.";
    out_oth="@@gname@@ nickt@@ wem@@@@adverb@@"+
      (who ? " zu." : (adverb ? "." : " zustimmend."));
    return FeelIt("nick","neutral",adverb,0);

    /**************** Niesen ***************/
    case "nies":
    if (ghost())
      Return("Du hast keine Nase mehr, in der es Dich jucken könnte...\n");
    ParseRest(str);
    if (str && !adverb)
      Return("Niese wie?\n");
    out_sel="Haaaaaa-tschi! Gesundheit!"+
      (adverb ? " Du niest@@adverb@@." : "");
    out_oth="Haaaaaa-tschi! @@Name@@ niest@@adverb@@.";
    return FeelIt("nies","neutral",adverb,0);

    /**************** Ohrfeigen ***************/
    case "gib":
    if (!str)
      Return("Gib wem was?\n");
    if (sscanf( str,"%s ohrfeige",str1)==0)
      return 0;
    ParseRest(str, ({"ohrfeige", "eine ohrfeige"}) );
    if (for_all)  {
      out_sel="Du verpasst @@alle@@@@adverb@@ eine Ohrfeige.";
      out_vic="@@name@@ verpasst @@alle@@@@adverb@@ eine Ohrfeige.";
      return MixedOut(WEM,"ohrfeig","fies",adverb,TOUCH);
    }
    if (!who)
      Return("Gib wem eine Ohrfeige?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
        "Dazu sind Dir Deine Backen doch zu schade.",
        "Du wirst doch nichts schlagen, was sich nicht wehren kann?"))
      return 1;
    GHOSTCHECK("Deine Hand saust mit voller Wucht durch Dein Opfer durch!\n",
        gname()+" will "+who->name(WEM)+" eine Ohrfeige geben - aber "+
        QPP(FEMALE,WER)+" Hand\n"
	"saust mit voller Wucht durch das Opfer durch!\n",
	gname()+" will Dir eine Ohrfeige geben - aber "+QPP(FEMALE,WER)+
        " Hand\n"
	"saust mit voller Wucht durch Dich durch!\n");
    if (who->QueryProp(P_GHOST))
    {
      out_sel="Deine Hand fährt durch den Kopf von @@wessen@@ Geist "
        "hindurch, als Du versuchst, "+who->QueryPronoun(WEM)+
        "@@adverb@@ eine Ohrfeige zu verpassen.";
      out_vic=Name(WESSEN)+" Hand fährt durch Deinen Kopf hindurch, als "+
        QueryPronoun(WER)+" versucht, Dir@@adverb@@ eine Ohrfeige zu "
        "verpassen. Du bist erstmals heilfroh, ein Geist zu sein!";
      out_oth=Name(WESSEN)+" Hand fährt durch "+who->name(WESSEN)+
        " Kopf hindurch, als "+QueryPronoun(WER)+" versucht, "+
        who->QueryPronoun(WEM)+"@@adverb@@ eine Ohrfeige zu verpassen.";
    }
    else
    {
      out_sel="Du verpasst @@wem@@@@adverb@@ eine schallende Ohrfeige.";
      out_vic="@@Name@@ verpasst Dir@@adverb@@ eine Ohrfeige, "
        "dass Dir Hören und Sehen vergeht.";
      out_oth="@@Name@@ verpasst @@wem@@@@adverb@@ eine schallende Ohrfeige.";
    }
    return FeelIt("ohrfeig","fies",adverb,TOUCH);

    /**************** Pfeifen ***************/
    case "pfeif":
    GHOSTCHECK(
        "Es kommt leider nur (nicht mal heiße) Luft, aber kein Pfiff.\n",
        gname()+" spitzt den Mund und pustet angestrengt. Nichts passiert.\n",
        0);
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Pfeife wie? Pfeife wem nach? Häh?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Was willst Du denn damit ausdrücken? Das gibt fuer mich keinen Sinn.",
        "Ich habe keine Lust dazu."))
      return 1;
    out_sel="Du pfeifst@@ wen@@@@adverb@@"+
      (who ? " nach." : (adverb ? "." : " anerkennend."));
    if (who) out_vic="@@Name@@ pfeift Dir@@adverb@@ nach.";
    out_oth="@@Name@@ pfeift@@ wen@@@@adverb@@"+
      (who ? " nach." : (adverb ? "." :" anerkennend."));
    return FeelIt("frech","neutral",adverb,0);

    /**************** Philosophieren ***************/
    case "philosophier":
    ParseRest(str);
    out_sel="Du philosophierst"+(adverb ? "@@adverb@@." :
        (str ? " ueber "+str+"." : "."));
    out_oth="@@gname@@ philosophiert"+(adverb ? "@@adverb@@." :
        (str ? " ueber "+str+"." : "."));
    return FeelIt("philosophier","neutral",adverb,0);

    /**************** Piepsen ***************/
    case "pieps":
    GHOSTCHECK("Als Geist kannst Du nicht piepsen!\n", "",0);
    ParseRest(str);
    out_sel="Du piepst"+(adverb ? "@@adverb@@" : " wie ein kleines Vögelchen")+
            " herum.";
    out_oth="@@gname@@ piepst"+(adverb ? "@@adverb@@" : " wie ein kleines "
            "Vögelchen")+ " herum.";
    return FeelIt("pieps","albern",adverb,0);

    /**************** Prusten ***************/
    case "prust":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du prustest @@alle@@@@adverb@@ an.";
      out_vic="@@name@@ prustet @@alle@@@@adverb@@ an";
      return MixedOut(WEN,"prust","frech",adverb,0);
    }
    if(!who && !adverb && str)
      Return("Wie willst Du prusten bzw. Wen willst du anprusten?\n");
    if(who==this_player())
      who=0;
    if(who && CheckLife(0,NOT_DEAD,0,"Das kannst Du nicht anprusten!"))
      return 1;
    out_sel="Du prustest"+(who?" "+who->name(WEN)+"@@adverb@@ an":"@@adverb@@ los")+".";
    if(who) out_vic="@@name@@ prustet Dich@@adverb@@ an.";
    out_oth="@@name@@ prustet"+(who?" "+who->name(WEN)+"@@adverb@@ an":"@@adverb@@ los")+".";
    return FeelIt("prust","frech",adverb,0);

    /**************** (Nase) Putzen ***************/
    case "putz":
    if (ghost())
      Return("Nix da zum Putzen, so nebulös wie Du bist...\n");
    ParseRest(str, ({"nase", "die nase"}) );
    if (str && str!="nase" && !adverb)
      Return("Putze Deine Nase wie?\n");
    out_sel="Du putzt Deine Nase@@adverb@@.";
    out_oth="@@Name@@ putzt@@adverb@@ "+QPP(FEMALE,WEN)+" Nase.";
    return FeelIt("putz","neutral",adverb,0);

   /******************* Quengeln *********************/
   case "quengel":
   case "quengl":
   ParseRest(str);
   if(for_all)
   {
     out_sel="Du quengelst @@alle@@ so richtig@@adverb@@ voll.";
     out_vic="@@name@@ quengelt Dich so richtig@@adverb@@ voll.";
     return MixedOut(WEN,"quengel","ärgerlich",adverb,0);
   }
   if(str && !adverb && !who)
     Return("Wen oder Wie willst Du denn quengeln?\n");
   if(who==this_player())
     who=0;
    if(CheckLife(0,NOT_DEAD,0,
       "Bei toten Dingen macht das doch wenig Sinn!"))
      return 1;
   out_sel="Du quengelst"+(who ? " "+who->name(WEN)+" ":" ")+
           "so richtig@@adverb@@";
   out_sel+=(who ? " voll":"")+".";
   if (who) out_vic="@@name@@ quengelt Dich so richtig@@adverb@@ "+
                    (who ? "voll":"")+".";
   out_oth="@@name@@ quengelt"+(who ? " "+who->name(WEN)+" ":" ")+
           "so richtig@@adverb@@";
   out_oth+=(who ? " voll":"")+".";
   return FeelIt("quengel","fies",adverb,0);

    /**************** Quietschen ***************/
    case "quietsch":
    case "quiek":
    ParseRest(str);
    if (str && !adverb)
      Return("Quietsche wie?\n");
    out_sel="Du quietschst"+(adverb ? "@@adverb@@." : " vergnuegt.");
    out_oth="@@gname@@ quietscht"+(adverb ? "@@adverb@@." : " vergnuegt.");
    return FeelIt("quiek","albern",adverb,0);

    /**************** Raeuspern ***************/
    case "raeusper":
    case "räusper":
    ParseRest(str);
    if (str && !adverb)
      Return("Hm? Wie meinen?\n");
    out_sel="Du räusperst Dich@@adverb@@.";
    out_oth="@@gname@@ räuspert sich@@adverb@@.";
    return FeelIt("räusper","neutral",adverb,0);

    /**************** Reiben ***************/
    case "reib":
    GHOSTCHECK(
        "'s ist halt schwierig, sich die Hände als Geist zu reiben.\n","",0);
    ParseRest(str, "hände");
    if (str && str!="hände" && !adverb)
      Return("Reibe die Hände wie?\n");
    out_sel="Du reibst Dir@@adverb@@ die Hände.";
    out_oth="@@Name@@ reibt sich@@adverb@@ die Hände.";
    return FeelIt("reib","neutral",adverb,0);

    /**************** Roecheln ***************/
    case "roechel":
    case "roechl":
    case "röchel":
    case "röchl":
    ParseRest(str);
    out_sel="Du röchelst"+(adverb ? "@@adverb@@." : " mitleiderregend.");
    out_oth="@@gname@@ röchelt"+(adverb ? "@@adverb@@." : " mitleiderregend.");
    return FeelIt("röchel","neutral",adverb,0);

    /***************** Rollen ******************/
    case "roll":
    ParseRest(str);

    out_sel="Du rollst@@adverb@@ mit den Augen.";
    out_oth="@@name@@ rollt@@adverb@@ mit den Augen.";
    return FeelIt("roll","neutral",adverb,0);

    /**************** Rotfln ***************/
    case "rotfl":
    ParseRest(str);
    if (str && !adverb)
      Return("Rotfl wie?\n");
    out_sel="Du rotflst@@adverb@@.";
    out_oth="@@gname@@ rotflt@@adverb@@.";
    return FeelIt("rotfl","albern",adverb,0);

    /**************** Ruelpsen ***************/
    case "ruelps":
    case "rülps":
    GHOSTCHECK(
        "Ein leichter Windhauch entfährt Deinem Mund, mehr nicht.\n",
        "Dem Mund des Geistes von "+name(WEM)+
        " entfährt ein leichtes Lüftchen.\n", 0);
    if (!str) {
      out_sel="BÖÖÖÖÖÖÖRRRRPP!  Entschuldige Dich gefälligst!";
      out_oth="@@Name@@ rülpst unanständig.";
    }
    else {
      ParseRest(str);
      if (!adverb)
	Return("Rülpse wie (ist schon schlimm genug, "
            "dass Du Dich nicht beherrschen kannst!)?\n");
      out_sel="Du rülpst@@adverb@@. Schäm Dich!";
      out_oth="@@Name@@ rülpst@@adverb@@ und wird nicht mal rot dabei.";
    }
    return FeelIt("rülps","frech",adverb,0);

    /**************** Ruempfen ***************/
    case "ruempf":
    case "rümpf":
    ParseRest(str);
    out_sel="Du rümpfst"+(adverb ? "@@adverb@@" : " angewidert")+
            " Deine Nase.";
    out_oth="@@gname@@ rümpft"+(adverb ? "@@adverb@@ " : " angewidert ")+
            QPP(FEMALE, WEN)+" Nase.";
    return FeelIt("rümpf","ärgerlich",adverb,0);

    /****************** Ruetteln **************/
    case "ruettel":
    case "ruettl":
    case "rüttel":
    case "rüttl":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du rüttelst @@alle@@@@adverb@@ wach.";
      out_vic="@@name@@ rüttelt @@alle@@@@adverb@@ wach";
      return MixedOut(WEN,"rüttel","neutral",adverb,TOUCH);
    }
    if(str && !adverb && !who)
      Return("Wen willst Du Wie wachrütteln?\n");
    if(!who) who=this_player();
    if(CheckLife(NOT_SELF,NOT_DEAD,
        "Das lass mal lieber die anderen machen ;-)",
        "Ich glaube nicht, dass das wachgerüttelt werden muss!"))
      return 1;
    out_sel="Du rüttelst "+who->name(WEN)+"@@adverb@@ wach.";
    out_vic="@@name@@ rüttelt Dich@@adverb@@ wach.";
    out_oth="@@name@@ rüttelt "+who->name(WEN)+"@@adverb@@ wach.";
    return FeelIt("rüttel","neutral",adverb,TOUCH);

    /**************** Runzeln ***************/
    case "runzel":
    case "runzl":
    if (ghost())
      Return("DAS geht als Geist nun wirklich nicht.\n");
    ParseRest(str,"stirn");
    if (str && !adverb)
      Return("Runzle die Stirn wie?\n");
    out_sel="Du runzelst@@adverb@@ die Stirn.";
    out_oth="@@Name@@ runzelt@@adverb@@ die Stirn.";
    return FeelIt("runzel","neutral",adverb,0);

    /**************** Sabbere ***************/
    case "sabber":
    ParseRest(str);
    if (str && !adverb && !who)
      Return("Sabber wie oder wen an?\n");
    out_sel="Du sabberst@@ wen@@@@adverb@@ "+
      (who ? "an." : "auf den Boden.");
    if (who) out_vic="@@gname@@ sabbert Dich@@adverb@@ an.";
    out_oth="@@gname@@ sabbert@@ wen@@@@adverb@@ "+
      (who ? "an." : "auf den Boden.");
    return FeelIt("sabber","frech",adverb,0);

    /**************** Schaemen ***************/
    case "schaem":
    case "schäm":
    ParseRest(str);
    if (str && !adverb && lower_case(str)!="dich")
      Return("Schäme Dich wie?\n");
    out_sel="Du schämst Dich@@adverb@@.";
    out_oth="@@gname@@ schämt sich"+(adverb ? "@@adverb@@" : " in Grund und Boden.");
    return FeelIt("schäm","traurig",adverb,0);

#ifdef SCHAU_AN
    /**************** Schau an ***************/
    case "schau":
    if (!str || explode(str, " ")[sizeof(explode(str, " "))]!="an")
      return 0;
    ParseRest(str, "an");
    if (!who)
      Return("Schau wen an?\n");
    out_sel="Du schaust @@wen@@@@adverb@@ an.";
    out_vic="@@gname@@ schaut Dich@@adverb@@ an.";
    out_oth="@@gname@@ schaut @@wen@@@@adverb@@ an.";
    return FeelIt("schau","neutral",adverb,0);
#endif

    /******************* Schielen ******************/
    case "schiel":
    ParseRest(str);
    if(str && !adverb)
      Return("Wie willst Du schielen?\n");
    out_sel="Du schielst@@adverb@@.";
    out_oth="@@name@@ schielt@@adverb@@.";
    return FeelIt("schiel","neutral",adverb,0);

    /**************** Schluchzen ***************/
    case "schluchz":
    ParseRest(str);
    if (str && !adverb)
      Return("Schluchze wie?\n");
    out_sel="Du schluchzt"+(adverb ? "@@adverb@@." : " herzzerreißend.");
    out_oth="@@gname@@ schluchzt"+
      (adverb ? "@@adverb@@." : " herzzerreißend.");
    return FeelIt("schluchz","traurig",adverb,0);

    /**************** Schlucken ***************/
    case "schluck":
    GHOSTCHECK("Was willst Du als Geist denn schlucken?\n", "",0);
    ParseRest(str);
    out_sel="*schluck* ---- GULP!";
    out_oth="@@gname@@ schluckt"+(adverb ? "@@adverb@@." : " trocken.");
    return FeelIt("schluck", "neutral", adverb,0);

    /**************** Schmachten ******************/
    case "schmacht":
    ParseRest(str);
    if(str && !adverb && !who)
      Return("Wie willst Du vor Wem dahinschmachten?\n");
    if(!who)
      who=this_player();
    if(CheckLife(NOT_SELF,0,"Selbstbewusstsein ist ja ok, aber das?",0))
      return 1;
    if(str && !adverb && !who)
      Return("Vor Wem willst Du Wie schmachten?\n");
    out_sel="Du schmachtest@@adverb@@ vor "+who->name(WEM)+" dahin.";
    out_oth="@@name@@ schmachtet@@adverb@@ vor "+who->name(WEM)+" dahin.";
    out_vic="@@name@@ schmachtet@@adverb@@ vor Dir dahin.";
    return FeelIt("schmacht","frech",adverb,0);

    /**************** Schmeicheln ****************/
    case "schmeichel":
    case "schmeichl":
    ParseRest(str);
    if(str && !adverb && !who)
      Return("Wem willst Du Wie schmeicheln?\n");
    if(!who)
      who=this_player();
    if(CheckLife(NOT_SELF,NOT_DEAD,
        "Soweit wollen wir es doch hier lieber nicht kommen lassen!",
        "Warum willst Du eine tote Sache anschmeicheln?"))
      return 1;
    out_sel="Du schmeichelst "+who->name(WEM)+"@@adverb@@.";
    out_vic="@@name@@ schmeichelt Dir@@adverb@@. ";
    out_vic+="Das hat Dir gerade noch gefehlt!";
    out_oth="@@name@@ schmeichelt "+who->name(WEM)+"@@adverb@@.";
    return FeelIt("schmeichel","nett",adverb,0);

    /***************** Schmusen *******************/
    case "schmus":
    ParseRest(str);
    if(!who)
      Return("Mit wem willst Du denn Wie schmusen?\n");
    if(CheckLife(NOT_SELF,NOT_DEAD,
        "Brauchst Du wirklich so dringend Streicheleinheiten?",
        "Mit toten Sachen schmusen macht doch gar keinen Spaß!"))
      return 1;
    out_sel="Du schmust@@adverb@@ mit "+who->name(WEM)+".";
    out_vic="@@name@@ schmust@@adverb@@ mit Dir.";
    out_oth="@@name@@ schmust@@adverb@@ mit "+who->name(WEM)+" herum.";
    return FeelIt("schmus","intim",adverb,TOUCH);

    /**************** Schmollen ***************/
    case "schmoll":
    GHOSTCHECK(
        "Du schwebst beleidigt in die Ecke.\n",
        gname()+" schwebt beleidigt in die Ecke und schmollt.\n", 0);
    ParseRest(str, "an");
    if (str && !adverb && !who)
      Return("Schmolle wie oder wen an?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
          "Hat wenig Sinn, Dich selbst anzuschmollen.",
          "Hat wenig Sinn, tote Sachen anzuschmollen."))
	return 1;
    out_sel="Du schmollst@@ wen@@@@adverb@@"+(who ? " an." : ".");
    if (who)
      out_vic="@@Name@@ geht in die Ecke und schmollt@@ wen@@@@adverb@@"+
        (who ? " an." : ".");
    out_oth="@@Name@@ geht in die Ecke und schmollt@@ wen@@@@adverb@@"+
      (who ? " an." : ".");
    return FeelIt("schmoll","traurig",adverb,0);

    /**************** Schmunzeln ***************/
    case "schmunzel":
    case "schmunzl":
    ParseRest(str);
    if (str && !adverb)
      Return("Schmunzle wie?\n");
    out_sel="Du schmunzelst@@adverb@@.";
    out_oth=gname()+" schmunzelt@@adverb@@.";
    return FeelIt("schmunzel","nett",adverb,0);

    /**************** Schnalzen ***************/
    case "schnalz":
    ParseRest(str, ({"zunge","mit zunge", "mit der zunge"}));
    out_sel="Du schnalzt@@adverb@@ mit der Zunge.";
    out_oth="@@gname@@ schnalzt@@adverb@@ mit der Zunge.";
    return FeelIt("schnalz","neutral",adverb,0);

    /**************** Schnauben ***************/
    case "schnaub":
    ParseRest(str);
    if (str && !adverb)
      Return("Schnaube wie?\n");
    out_sel="Du schnaubst"+(adverb ? "@@adverb@@." : " entrüstet.");
    out_oth="@@gname@@ schnaubt"+(adverb ? "@@adverb@@." : " entrüstet.");
    return FeelIt("schnaub","ärgerlich",adverb,0);

    /**************** Schnaufen ***************/
    case "schnauf":
    ParseRest(str);
    if (str && !adverb)
      Return("Schnaufe wie?\n");
    out_sel="Du schnaufst"+(adverb ? "@@adverb@@." : " vor Anstrengung.");
    out_oth="@@gname@@ schnauft"+(adverb ? "@@adverb@@." : " vor Anstrengung.");
    return FeelIt("schnauf","neutral",adverb,0);

    /****************** Schneuzen ***************/
    case "schneuz":
    if(ghost())
      Return("Du versuchst es, bringst aber nur ein Rasseln hervor.\n");
    ParseRest(str);
    if (str && !adverb)
      Return("Wie willst Du Dich schneuzen?\n");
    out_sel="Du schneuzt Dich"+(adverb ? "@@adverb@@." : ".");
    out_oth="@@name@@ schneuzt sich"+(adverb ? "@@adverb@@." : ".");
    return FeelIt("schneuz","neutral",adverb,0);

    /**************** Schnippen ***************/
    case "schnipp":
    case "schnipps":
    GHOSTCHECK(
        "Du schaffst es nicht, weil die Finger durcheinander durch gehen.\n",
	gname()+" versagt beim Schnippen - die Finger\n"
        "gehen durcheinander durch.\n", 0);
    ParseRest(str);
    if (str && !adverb)
      Return("Schnippe wie?\n");
    out_sel="Du schnippst@@adverb@@ mit deinen Fingern.";
    out_oth="@@Name@@ schnippt@@adverb@@ mit den Fingern.";
    return FeelIt("schnipp","neutral",adverb,0);

    /**************** Schnarchen ***************/
    case "schnarch":
    if (ghost())
      Return("Ich glaube, da fehlen Dir irgendwie die physischen "
          "Voraussetzungen dazu.\n");
    ParseRest(str);
    if (str && !adverb)
      Return("Schnarche wie?\n");
    out_sel=(adverb ? "Du schnarchst@@adverb@@." : "Zzzzzzzzzzz...");
    out_oth="@@Name@@ schnarcht"+(adverb ? "@@adverb@@." : " laut.");
    return FeelIt("schnarch","neutral",adverb,0);

    /**************** Schniefen ***************/
    case "snief":
    case "schnief":
    GHOSTCHECK(
        "Du schniefst ganz leise.\n",
        gname()+" schnieft ganz leise.\n", 0);
    ParseRest(str);
    if (str && !adverb)
      Return("Schniefe wie?\n");
    out_sel="Du schniefst@@adverb@@.";
    out_oth="@@Name@@ schnieft@@adverb@@.";
    return FeelIt("schnief","traurig",adverb,0);

    /**************** Schnurren ***************/
    case "schnurr":
    ParseRest(str);
    if (str && !adverb)
      Return("Wie willst Du schnurren?\n");
    out_sel="MMMMIIIIIAAAAAAUUUUUUUU! Du schnurrst"+
      (adverb ? "@@adverb@@." : " zufrieden.");
    out_oth="@@gname@@ schnurrt"+(adverb ? "@@adverb@@." : " zufrieden.");
    return FeelIt("schnurr","albern",adverb,0);

    /**************** Schreien ***************/
    case "schrei":
    case "aua":
    GHOSTCHECK(
        "AAAAIIIIIIIIIIIEEEEEEEEEEEEEEEEEEEEEEEEEE! "
        "Ja, nur Geister können so schreien!\n",
	gname()+" schreit - das Blut gefriert fast in Deinen Adern!\n", 0);
    if (!str)  {
      out_sel="AUUUAAAHHHHHH!!!!";
      out_oth="@@Name@@ schreit laut!";
    }
    else  {
      ParseRest(str);
      if (!who && !adverb)
	Return("Schreien - wie denn? Oder wen denn anschreien?\n");
      out_sel="Du schreist@@ wen@@@@adverb@@"+(who ? " an" : "")+".";
      if (who) out_vic="@@Name@@ schreit Dich@@adverb@@ an.";
      out_oth="@@Name@@ schreit@@ wen@@@@adverb@@"+(who? " an" : "")+".";
    }
    return FeelIt("schrei","fies",adverb,0);

    /**************** Zunge rausstrecken ***************/
    case "streck":
    GHOSTCHECK("Sorry, Du hast keine Zunge zum Rausstrecken.\n","",0);
    if (!str)
      Return("Strecke was wie wem wo wann wieso?\n");
    str=lower_case(str);
    if (sscanf(str, "%s zunge raus", str1)!=1 &&
	sscanf(str, "%s die zunge raus", str1)!=1)
      Return("Strecke was wie wem wo wann wieso?\n");
    str=str1;
    case "baeh":
    case "bäh":
    ParseRest(str);
    GHOSTCHECK("Sorry, Du hast keine Zunge zum Rausstrecken.\n","",0);
    if (for_all)  {
      out_sel="Du streckst @@alle@@@@adverb@@ die Zunge raus.";
      out_vic="@@Name@@ streckt @@alle@@@@adverb@@ die Zunge raus.";
      return MixedOut(WEM,"bäh","frech",adverb,0);
    }
    if (who &&  CheckLife(NOT_SELF,0,
          "Wie willst Du Dir denn selber ?", ""))
        return 1;
    out_sel="Du streckst@@ wem@@@@adverb@@ die Zunge raus.";
    if (who) out_vic="@@Name@@ streckt Dir@@adverb@@ die Zunge raus.";
    out_oth="@@Name@@ streckt@@ wem@@@@adverb@@ die Zunge raus.";
    return FeelIt("bäh","frech",adverb,0);

    /**************** Schubsen ***************/
    case "schubs":
    ParseRest(str);
    if (!who)
      Return("Schubse wen?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
        "Hm. Dich selber schubsen? Sieht doch blöd aus.",
        "Einen Toten lässt man besser in Ruhe."))
      return 1;
    GHOSTCHECK(break_string("Du versuchst "+who->name(WEN)+" zu schubsen, "
        "fällst aber durch "+who->QueryPronoun(WEN)+" hindurch.",78),
        gname()+" will Dich stupsen, fällt jedoch duch Dich hindurch.\n",0);
    out_sel="Du schubst@@ wen@@@@adverb@@.";
    out_vic="@@Name@@ schubst Dich@@adverb@@.";
    out_oth="@@Name@@ schubst@@ wen@@@@adverb@@.";
    return FeelIt("schubs","frech",adverb,TOUCH);


    /**************** Schuetteln ***************/
    case "schuettel":
    case "schuettl":
    case "schüttel":
    case "schüttl":
    ParseRest(str);
    if (for_all)  {
      out_sel="Du schüttelst @@alle@@@@adverb@@ die Hände.";
      out_vic="@@gname@@ schüttelt @@alle@@@@adverb@@ die Hände.";
      return MixedOut(WEM,"schüttel","nett",adverb,TOUCH);
    }
    if (str && !who && !adverb)
      Return("Schüttle wie? Schüttle wem die Hand?\n");
    if(!who) {
      out_sel="Du schüttelst Dich@@adverb@@.";
      out_oth="@@gname@@ schüttelt sich@@adverb@@.";
    }
    else  {
      if (CheckLife(0, NOT_DEAD, "", "Nö, das mach ich nur mit Lebewesen."))
	return 1;
      if (who == this_player())  {
	out_sel="Du hebst"+(adverb ? "@@adverb@@" : " triumphierend")+
          " Deine Hände über den Kopf und schüttelst sie.";
	out_oth="@@gname@@ hebt"+(adverb ? "@@adverb@@" : " triumphierend")+
          " die Hände über den Kopf und schüttelt sie.";
      }
      else  {
	out_sel="Du schüttelst@@ wem@@@@adverb@@ die Hände.";
	out_vic="@@gname@@ schüttelt Dir@@adverb@@ die Hände.";
	out_oth="@@gname@@ schüttelt@@ wem@@@@adverb@@ die Hände.";
        if (ghost()) {
	  out_sel+="\nNaja, Du versuchst es wenigstens - "
            "immer diese durchlässigen Hände...";
	  out_vic+="\nNaja, versucht es wenigstens - "
            "immer diese durchlässigen Hände...";
	  out_oth+="\nNaja, versucht es wenigstens - "
            "immer diese durchlässigen Hände...";
        }
      }
    }
    return FeelIt("schüttel","nett",adverb,TOUCH);

    /**************** Schweigen ***************/
    case "schweig":
    ParseRest(str);
    if (str && !adverb && !who)
      Return("Schweige wie oder wen an?\n");
    if (who && CheckLife(NOT_SELF,0,
        "Was bringt denn das, wenn Du Dich selbst anschweigst? "
        "Lass es lieber..."))
      return 1;
    out_sel="Du schweigst@@adverb@@"+(who ? " @@wen@@ an." : ".");
    if (who) out_vic="@@gname@@ schweigt Dich@@adverb@@ an.";
    out_oth="@@gname@@ schweigt@@adverb@@"+(who ? " @@wen@@ an." : ".");
    return FeelIt("schweig","neutral",adverb,0);

    /**************** Seufzen ***************/
    case "seufz":
    GHOSTCHECK(
        "Du seufzt geisterhaft.\n",
        gname()+" seufzt geisterhaft. Naja, wie denn sonst?\n", 0);
    ParseRest(str);
    if (!adverb && str)
      Return("Seufze wie?\n");
    out_sel="Du seufzst@@adverb@@.";
    out_oth="@@Name@@ seufzt@@adverb@@.";
    return FeelIt("seufz","traurig",adverb,0);

    /**************** Singen ***************/
    case "sing":
    if (!str) {
      out_sel="Oh sole mio!";
      out_oth="@@gname@@ singt irgendwas italienisches.";
    }
    else  {
      ParseRest(str);
      out_sel="Du singst@@adverb@@"+(adverb ? "." : " '"+capitalize(str)+"'.");
      out_oth="@@gname@@ singt@@adverb@@"+
        (adverb ? "." : " '"+capitalize(str)+"'.");
    }
    return FeelIt("sing","neutral",adverb,0);

    /**************** Sniffen ***************/
    case "sniff":
    ParseRest(str);
    if (str && !adverb && !who)
      Return("Sniffe wie?\n");
    out_sel="Du sniffst"+(who ? " @@wen@@" : "")+
      (adverb ? "@@adverb@@" : " traurig")+(who ? " an." : ".");
    if (who) out_vic="@@gname@@ snifft Dich"+
      (adverb ? "@@adverb@@" : " traurig")+" an.";
    out_oth="@@gname@@ snifft"+(who ? " @@wen@@" : "")+
      (adverb ? "@@adverb@@" : " traurig")+(who ? " an." : ".");
    return FeelIt("sniff","traurig",adverb,0);

    /**************** Spannen ***************/
    case "spann":
    ParseRest(str);
    out_sel="Du spannst@@adverb@@ Deine Muskeln an.";
    out_oth="@@gname@@ spannt@@adverb@@ "+QPP(FEMALE,WEN,PLURAL)+" Muskeln an.";
    return FeelIt("spann","neutral",adverb,0);

    /****************** Springen ****************/
    case "spring":
    if(ghost())
      Return("Als Geist springt es sich so schlecht.\n");
    ParseRest(str);
    if(str && !adverb)
      Return("Wie willst Du springen?\n");
    str1=" wie eine Verrückte";
    if(this_player()->QueryProp(P_GENDER)!=2)
      str1=" wie ein Verrückter";
    out_sel="Du springst"+(adverb ? "@@adverb@@":str1)+" durch den Raum.";
    out_oth="@@name@@ springt"+(adverb ? "@@adverb@@":str1)+" durch den Raum.";
    return FeelIt("spring","albern",adverb,0);

    /**************** Spucken ***************/
    case "spuck":
    GHOSTCHECK(
        "Du bringst nicht genug Spucke zusammen.\n",
        gname()+" stellt gerade fest, dass man ohne Spucke nicht\n"
        "spucken kann.\n", 0);
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Spucke wen wie an?\n");
    if (who && CheckLife(NOT_SELF,0,
        "Hast Du Dich so schlecht benommen? Lass es lieber bleiben."))
      return 1;
    out_sel="Du spuckst@@ wen@@@@adverb@@ "+(who ? "an." : "auf den Boden.");
    if (who) out_vic="@@Name@@ spuckt Dich@@adverb@@ an.";
    out_oth="@@Name@@ spuckt@@ wen@@@@adverb@@ "+
      (who ? "an." : "auf den Boden.");
    return FeelIt("spuck","fies",adverb,0);

    /**************** Stampfen ***************/
    case "stampf":
    ParseRest(str, "auf");
    out_sel="Du stampfst@@adverb@@ mit dem Fuß auf.";
    out_oth="@@gname@@ stampft@@adverb@@ mit dem Fuß auf.";
    return FeelIt("stampf","ärgerlich",adverb,0);

    /**************** Starren ***************/
    case "starr":
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Starre wie bzw. wen an?\n");
    if (who && CheckLife(NOT_SELF,0,
        "Wie willst Du in Deine eigenen Augen starren? "
        "(Spiegel gelten nicht...)"))
      return 1;
    if (!str)
    {
      out_sel="Du starrst ins Leere.";
      out_oth="@@gname@@ starrt ins Leere.";
    }
    else
    {
      out_sel="Du starrst@@ wen@@"+(adverb ? "@@adverb@@" : " verträumt")+
        (who ? " an." : ".");
      if (who) out_vic="@@gname@@ starrt"+(adverb ? "@@adverb@@" : " tief")+
        " in Deine Augen.";
      out_oth="@@gname@@ starrt@@ wen@@"+
        (adverb ? "@@adverb@@" : " verträumt")+(who ? " an." : ".");
    }
    return FeelIt("starr","neutral",adverb,0);

    /**************** Staunen ***************/
    case "staun":
    if (!str)  {
      out_sel="Du bist erstaunt.";
      out_oth="@@gname@@ ist erstaunt.";
    }
    else  {
      ParseRest(str, "über");
      if (!who && !adverb)
	Return("Bla schwall. Wenn Du nach staune noch was tippst, sollte das "
            "ein\nLebewesen sein.\n");
      if (who == this_player())  {
	out_sel="Du staunst@@adverb@@ über Dich selber.";
	out_oth="@@gname@@ staunt@@adverb@@ über sich selber.";
      }
      else  {
	out_sel="Du staunst@@adverb@@"+(who ? " über @@wen@@." : ".");
	if (who) out_vic="@@gname@@ staunt@@adverb@@ über Dich.";
	out_oth="@@gname@@ staunt@@adverb@@"+(who ? " über @@wen@@." : ".");
      }
    }
    return FeelIt("staun","neutral",adverb,0);

    /**************** Stieren ***************/
    case "stier":
    GHOSTCHECK(
        "Du stierst mit hohlem Blick in die Gegend.\n",
        gname()+" stiert mit hohlem Blick in die Gegend.\n", 0);
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Stiere wie oder wen an?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Du kannst Dich nicht selber anstieren.",
        "Bitte nur Lebewesen anstieren."))
      return 1;
    out_sel="Du stierst@@ wen@@@@adverb@@"+
      (who ? " an." : " in der Gegend herum.");
    if (who) out_vic="@@gname@@ stiert Dich@@adverb@@ an.";
    out_oth="@@gname@@ stiert@@ wen@@@@adverb@@"+
      (who ? " an." : " in der Gegend herum.");
    return FeelIt("stier","neutral",adverb,0);

    /**************** Stimme zu ***************/
    case "stimm":
    ParseRest(str, "zu");
    if (str && !who && !adverb)
      Return("Stimme wem zu?\n");
    if (CheckLife(NOT_SELF, NOT_DEAD,
        "Du bist doch sowieso immer einer Meinung mit Dir selbst!",
        "Tote Dinge haben doch gar keine Meinung, der Du zustimmen könntest!"))
      return 1;
    out_sel="Du stimmst@@ wem@@@@adverb@@ zu.";
    if (who) out_vic="@@gname@@ stimmt Dir@@adverb@@ zu.";
    out_oth="@@gname@@ stimmt@@ wem@@@@adverb@@ zu.";
    return FeelIt("stimm","nett",adverb,0);

    /**************** Stoehnen ***************/
    case "stoehn":
    case "stöhn":
    GHOSTCHECK(
        "Du stöhnst schauderlich.\n",
        gname()+" stöhnt schauderlich. Zum Glück "
        "hast Du ziemlich\ngute Nerven.\n", 0);
    ParseRest(str);
    if (!adverb && str)
      Return("Wie willst Du stöhnen?\n");
    out_sel="Du stöhnst@@adverb@@.";
    out_oth="@@Name@@ stöhnt@@adverb@@.";
    return FeelIt("stöhn","neutral",adverb,0);

    /**************** Stolpern **********************/
    case "stolper":
    ParseRest(str);
    if( who == ME ) who = 0;
    out_sel="Du stolperst@@adverb@@ über "+
      (who ? who->name(WESSEN) : "Deine")+" Füße.";
    if(who) out_vic="@@name@@ stolpert@@adverb@@ über Deine Füße.";
    out_oth="@@name@@ stolpert@@adverb@@ über "+
      (who ? who->name(WESSEN):QPP(MALE,WESSEN,PLURAL))+" Füße.";
    return FeelIt("stolper","neutral",adverb,0);

    /**************** Stolzieren **************/
    case "stolzier":
    ParseRest(str);
    out_sel="Du stolzierst@@adverb@@ daher.";
    out_oth="@@gname@@ stolziert@@adverb@@ daher.";
    return FeelIt("stolzier","neutral",adverb,0);

    /**************** Stossen ***************/
    case "stoss":
    case "stoß":
    ParseRest(str);
    if (!who)
      Return("Stoße wen?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
        "Was soll der Unsinn? Lass das!",
        "Das gibt nur bei Lebewesen Sinn."))
      return 1;
    GHOSTCHECK("Dein Ellenbogen versinkt in "+who->name(WEM)+".\n",
        gname()+" will "+who->name(WEM)+" in die Rippen stoßen, aber "+
        QPP(MALE)+"\nEllenbogen verteilt keinen Stoß, sondern versinkt.\n",
        gname()+" will Dich in die Rippen stossen, aber "+QPP(MALE)+
        "\nEllenbogen versinkt in Dir.\n");
    out_sel="Du stößt@@ wen@@@@adverb@@ in die Rippen.";
    out_vic="@@Name@@ stößt Dir@@adverb@@ in die Rippen.";
    out_oth="@@Name@@ stößt@@ wen@@@@adverb@@ in die Rippen.";
    return FeelIt("stoß","fies",adverb,TOUCH);

    /****************** Strahlen ******************/
    case "strahl":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du strahlst @@alle@@@@adverb@@ an.";
      out_vic="@@name@@ strahlt Dich@@adverb@@ an.";
      return MixedOut(WEN,"strahl","froh",adverb,0);
    }
    if(str && !adverb && !who)
      Return("Wen willst Du Wie anstrahlen?\n");
    if(who==this_player())
      who=0;
    out_sel="Du strahlst "+(who? who->name(WEN)+"@@adverb@@ an":
            "wie ein@@adverb@@"+ (adverb?"es":"")+" Honigkuchenpferd")+".";
    if(who) out_vic="@@name@@ strahlt Dich@@adverb@@ an.";
    out_oth="@@name@@ strahlt "+(who? who->name(WEN)+"@@adverb@@ an":
            "wie ein@@adverb@@"+ (adverb?"es":"")+" Honigkuchenpferd")+".";
    return FeelIt("strahl","froh",adverb,0);

    /**************** Streicheln ***************/
    case "streichel":
    case "streichl":
    ParseRest(str);
    if (for_all)
    {
      out_sel="Du streichelst @@alle@@@@adverb@@.";
      out_vic="@@name@@ streichelt @@alle@@@@adverb@@.";
      return MixedOut(WEN,"streichel","intim",adverb,TOUCH);
    }
    if (!who)
      Return("Streichle wen?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
        "Lass Dich von anderen streicheln.",
        "Ich streichle nur Lebewesen."))
      return 1;
    GHOSTCHECK(
        "Du willst "+who->name(WEN,2)+" streicheln, aber Deine Hände "
        "können nichts berühren.\n",
        gname()+" will "+who->name(WEN,2)+" streicheln, aber diese "
        "Geisterhände können\nhalt nix berühren...\n",
        gname()+" will Dich streicheln, scheitert aber wie so oft an\n"
        "diesen dummen durchlässigen Geisterhänden.\n");
    out_sel="Du streichelst @@wen@@@@adverb@@.";
    out_vic="@@Name@@ streichelt Dich@@adverb@@.";
    out_oth="@@Name@@ streichelt @@wen@@@@adverb@@.";
    return FeelIt("streichel","intim",adverb,TOUCH);

    /****************** Stubsen *****************/
    case "stubs":
    ParseRest(str);
    str1="";
    if(str) str1=explode(str," ")[sizeof(explode(str," "))-1];
    if(for_all)
    {
      out_sel="Du stubst @@alle@@@@adverb@@ "+(str1=="um" ? "um":"an")+".";
      out_vic="@@name@@ stubst @@alle@@@@adverb@@ "+(str1=="um" ? "um":"an")+".";
      return MixedOut(WEN,"stubs","albern",adverb,TOUCH);
    }
    if(str && !adverb && !who)
      Return("Wen willst Du wie an- oder umstubsen?\n");
    if(!who)
      who=this_player();
    if(CheckLife(NOT_SELF,0,
        "Frag doch mal einen der anderen, vielleicht stubst der Dich an!",0))
      return 1;
    out_sel="Du stubst "+who->name(WEN)+"@@adverb@@ "+(str1=="um" ? "um":"an")+
            ".";
    out_vic="@@name@@ stubst Dich@@adverb@@ "+(str1=="um" ? "um":"an")+".";
    out_oth="@@name@@ stubst "+who->name(WEN)+"@@adverb@@ "+
           (str1=="um" ? "um":"an")+".";
    return FeelIt("stubs","albern",adverb,TOUCH);

    /**************** Stupsen ***************/
    case "stups":
    case "stupf":
    ParseRest(str);
    if (!who)
      Return("Stupse wen?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
        "Hm. Dich selber stupsen? Sieht doch blöd aus.",
        "Einen Toten lässt man besser in Ruhe."))
      return 1;
    GHOSTCHECK("Dein Zeigefinger taucht in "+who->name(WESSEN)+" Schulter.\n",
        gname()+" versucht "+who->name(WEN)+" zu stupsen, doch "+
        QPP(MALE,WER,PLURAL)+"\nFinger taucht in "+who->name(WESSEN)+
        " Schulter.\n",
        gname()+" will Dich stupsen, doch "+QPP(MALE,WER,PLURAL)+
        " Finger tauchen in Deine Schulter.\n");
    out_sel="Du stupst@@ wen@@@@adverb@@.";
    out_vic="@@Name@@ stupst Dich@@adverb@@.";
    out_oth="@@Name@@ stupst@@ wen@@@@adverb@@.";
    return FeelIt("stups","albern",adverb,TOUCH);

    /**************** Summen ***************/
    case "summ":
    ParseRest(str);
    out_sel="Du summst"+(adverb ? "@@adverb@@." : " eine lustige Melodie.");
    out_oth="@@gname@@ summt"+(adverb ? "@@adverb@@." : " eine lustige "
        "Melodie.");
    return FeelIt("summ","froh",adverb,0);

    /**************** Taetscheln ***************/
    case "taetschel":
    case "taetschl":
    case "tätschel":
    case "tätschl":
    ParseRest(str);
    if (!who)
      Return("Tätschle wen?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
        "Das sieht zu doof aus, das mache ich nicht.",
        "Ich tätschle nur Lebewesen."))
      return 1;
    GHOSTCHECK(
        "Du willst "+who->name(WEN)+" tätscheln - aber Deine Hände gehen\n"
        "glatt durch den Kopf durch.\n",
        gname()+" will "+who->name(WEN)+" den Kopf tätscheln, aber die "
        "Geister-\nhände gehen glatt durch den Kopf durch.\n",
        gname()+" will Deinen Kopf tätscheln, aber diese Geisterhände "
        "gehen\nglatt durch Deinen Kopf durch - Du hast ein seltsames "
        "Gefühl dabei.\n");
    out_sel="Du tätschelst @@wem@@@@adverb@@ den Kopf.";
    out_vic="@@Name@@ tätschelt Dir@@adverb@@ den Kopf.";
    out_oth="@@Name@@ tätschelt @@wem@@@@adverb@@ den Kopf.";
    return FeelIt("tätschel","nett",adverb,TOUCH);

    /**************** Tanzen ***************/
    case "tanz":
    GHOSTCHECK(
        "Du tanzt den klassischen Geistertanz (tm).\n",
        gname()+" tanzt den klassischen Geistertanz (tm).\n", 0);
    if (!str) {
      out_sel=
        "Kommst Du Dir nicht irgendwie blöd vor? Du tanzt den Ententanz.";
      out_oth="@@Name@@ tanzt den Ententanz.";
    }
    else {
      taenze=({"Walzer","Polka","Rumba","Tango","Cha cha cha","Foxtrott",
	       "Mambo","Salsa","Slowfox","Breakdance","Pogo","Merengue",
	       "Rock'n'Roll","Ballett","Regentanz","Hexentanz"});
      ParseRest(str,"mit");
      if (!who)
	Return("Mit wem willst Du tanzen?\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
          "Mit Dir selber kannst Du nicht tanzen.",
          "Keine Reaktion - will wahrscheinlich nicht tanzen."))
	return 1;
      ifoo=random(sizeof(taenze));
      for(i=0;i<sizeof(taenze);i++)
      {
        if( strstr(str, lower_case(taenze[i]))!=-1)
        ifoo=i;
      }
      out_sel="Du tanzt@@adverb@@ mit @@wem@@ eine Runde "+taenze[ifoo]+".";
      out_vic="@@Name@@ reißt Dich an sich und tanzt@@adverb@@ eine Runde "+
        taenze[ifoo]+" mit Dir.";
      out_oth="@@Name@@ schnappt sich @@wen@@ und die beiden tanzen"
        "@@adverb@@ eine Runde "+taenze[ifoo]+".";
    }
    return FeelIt("tanz","froh",adverb,TOUCH);

    /**************** Torkeln **********************/
    case "torkel":
    case "torkl":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du torkelst@@adverb@@ vor @@alle@@.";
      out_vic="@@name@@ torkelt@@adverb@@ vor @@alle@@.";
      return MixedOut(WEM,"torkle","neutral",adverb,0);
    }
    if(who==this_player())
      Return("Wie willst Du denn vor Dir selber torkeln?\n");
    out_sel="Du torkelst@@adverb@@"+(who ? " vor "+who->name(WEM):"")+".";
    if(who) out_vic="@@name@@ torkelt@@adverb@@ vor Dir.";
    out_oth="@@name@@ torkelt@@adverb@@"+(who ? " vor "+who->name(WEM):"")+".";
    return FeelIt("torkel","neutral",adverb,0);

    /**************** Traellern ***************/
    case "traeller":
    case "träller":
    GHOSTCHECK("Als Geist hast Du viel zu schlechte Laune um zu trällern!\n",
      "",0);
    ParseRest(str);
    out_sel="Du trällerst"+
        (adverb ? ":@@adverb@@.":" ein kleines Liedchen: Lalali,lalala!");
    out_oth="@@gname@@ trällert"+
        (adverb ? ":@@adverb@@." : " ein kleines Liedchen: Lalali,lalala!");
    return FeelIt("träller","froh",adverb,0);

    /**************** Traeumen ***************/
    case "traeum":
    case "träum":
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Träume wie? Oder träume von wem?\n");
    if (who) {
      if( who == this_player() )
      {
        out_sel="Du träumst augenblicklich@@adverb@@ von Dir selbst.";
        out_oth="@@gname@@ träumt momentan@@adverb@@ von sich selbst.";
      }
      else
      {
        out_sel="Du träumst augenblicklich@@adverb@@ von @@wem@@.";
        out_vic="@@gname@@ träumt@@adverb@@ von Dir.";
        out_oth="@@gname@@ träumt momentan@@adverb@@ von @@wem@@.";
      }
    }
    else {
      out_sel="Du träumst@@adverb@@.";
      out_oth="@@gname@@ träumt@@adverb@@.";
    }
    return FeelIt("träum","neutral",adverb,0);
    /**************** Treten (tritt) ***************/
    case "kick":
    case "tritt":
    case "tret":
    ParseRest(str);
    if (!str) {
      GHOSTCHECK(
          "Dein Fuß fährt durch die berühmte langvergessene unsichtbare "
          "Schildkröte\nhindurch.\n",
          gname()+" will die berühmte langvergessene unsichtbare\n"
          "Schildkröte treten, aber "+QPP(MALE,WER)+
          " Fuß fährt durch sie hindurch.\n", 0);
      out_sel="Du trittst die berühmte langvergessene unsichtbare "
        "Schildkröte.";
      out_oth="@@name@@ tritt die berühmte langvergessene unsichtbare "
        "Schildkröte.";
    }
    else {
      if (for_all) {
        GHOSTCHECK(
            "Du trittst wie wildgeworden umher, triffst jedoch in Deinem "
            "immateriellen\nZustand nichts und niemanden.\n",
            gname()+" fängt an, herumzutreten, trifft aber mit diesen\n"
            "immateriellen Füßen nichts und niemanden.\n", 0);
	out_sel="Du trittst @@alle@@@@adverb@@. "
          "Solltest Du nicht langsam an Flucht denken?";
	out_vic="@@Name@@ tritt @@alle@@@@adverb@@. "
          "Traut sich ganz schön was!";
	return MixedOut(WEN,"tritt","fies",adverb,TOUCH);
      }
      if (!who && !adverb)
	Return("Wenn Du schon was nach tritt tippst, "
            "dann sag mir wen oder wie ich das soll.\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
          "Du schaffst es nicht, Dir selber in den Hintern zu treten.",
          "Tote Sachen tritt man nicht auch noch!"))
	return 1;
      if (who) {
        GHOSTCHECK(
            "Du trittst nach "+who->name(WEM)+", Dein Fuß fährt aber "
            "mit voller Wucht durch\n"+who->QueryPronoun(WEN)+" hindurch!\n",
            gname()+" tritt nach "+who->name(WEM)+", aber "+QPP(MALE)+
            " Fuß fährt mit voller\nWucht durch "+who->QueryPronoun(WEN)+
            " hindurch!\n",
            gname()+" tritt nach Dir, aber "+QPP(MALE)+" Fuß fährt mit "
            "voller\nWucht durch Dich hindurch!\n");
	out_sel="Du trittst@@ wen@@@@adverb@@.";
	if (who) out_vic="@@Name@@ tritt Dich@@adverb@@.";
	out_oth="@@Name@@ tritt@@ wen@@@@adverb@@.";
      }
      else {
        GHOSTCHECK(
            "Dein Fuß fährt durch die berühmte langvergessene unsichtbare "
            "Schildkröte\nhindurch.\n",
            gname()+" will die berühmte langvergessene unsichtbare\n"
            "Schildkröte treten, aber "+QPP(MALE,WER)+
            " Fuß fährt durch sie hindurch.\n", 0);
	out_sel="Du trittst die berühmte langvergessene unsichtbare "
          "Schildkröte@@adverb@@.";
	out_oth="@@Name@@ tritt die berühmte langvergessene unsichtbare "
          "Schildkröte@@adverb@@.";
      }
    }
    return FeelIt("tritt","fies",adverb,TOUCH);

    /**************** Troesten ***************/
    case "troest":
    case "tröst":
    ParseRest(str);
    if (!who)
      Return("Wen willst Du trösten?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Trost musst Du bei jemand anders suchen.",
        "Das Teil musst Du nicht trösten, das fühlt nix."))
      return 1;
    out_sel="Du tröstest@@ wen@@@@adverb@@.";
    out_vic="@@gname@@ tröstet Dich@@adverb@@.";
    out_oth="@@gname@@ tröstet@@ wen@@@@adverb@@.";
    return FeelIt("tröst","nett",adverb,0);

   /**************** Trommeln **************/
    case "trommel":
    case "tromml":
    if(ghost())
      Return("Als Geist bist Du dazu leider nicht faehig.\n");
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du trommelst mit Deinen Fingern @@alle@@@@adverb@@ auf dem "
              "Kopf herum.";
      out_vic="@@name@@ trommelt @@alle@@ mit "
              +(this_player()->QueryProp(P_GENDER)==2?"ihren":"seinen")+
              " Fingern@@adverb@@ auf dem Kopf herum.";
      return MixedOut(WEM,"trommel","frech",adverb,TOUCH);
    }
    if(str && !adverb && !who)
      Return("Auf Wessen Kopf willst Du Wie trommeln?\n");
    if(CheckLife(NOT_SELF,NOT_DEAD,
      "Nee, lass mal lieber oder willst Du Druckstellen am Kopf?",
      "Am besten trommelt es sich mit den Fingern, auf anderer Leute Köpfen!"))
        return 1;
    out_sel="Du trommelst mit Deinen Fingern@@adverb@@ "+(who?"auf "+
            who->name(WESSEN)+" Kopf ":"")+"herum.";
    if(who)
    {
      out_vic="@@name@@ trommelt mit "+
              (this_player()->QueryProp(P_GENDER)==2?"ihren":"seinen")+
              " Fingern@@adverb@@ auf Deinem Kopf herum.";
    }
    out_oth="@@name@@ trommelt mit "
        +(this_player()->QueryProp(P_GENDER)==2?"ihren":"seinen")+
        " Fingern@@adverb@@ auf "+(who? who->name(WESSEN):
     (this_player()->QueryProp(P_GENDER)==2?"ihrem":"seinem") )+" Kopf herum.";
    return FeelIt("trommel","frech",adverb,TOUCH);


    /**************** Umarmen ***************/
    case "umarm":
    ParseRest(str);
    if (!who && !for_all)
      Return("Wen willst Du umarmen?\n");
    if (who && CheckLife(0,NOT_DEAD,0,"Bitte umarme nur Lebewesen."))
      return 1;
    if (who && ghost() && CheckLife(NOT_SELF,0,
        "Du kannst Dich als Geist nicht selber wärmen."))
      return 1;
    str1=for_all ? "alle" : who->name(WEN);
    str2=for_all ? "jeden" : who->QueryPronoun(WEN);
    GHOSTCHECK(
        "Du willst "+str1+" umarmen, aber Deine Arme gehen durch "+str2+
        " hindurch.\n",
        gname()+" will "+str1+" umarmen, aber "+QPP(MALE,WER,PLURAL)+
        " Arme gehen\ndurch "+str2+" hindurch.\n",
        for_all ? 0 : gname()+" will Dich umarmen, aber "+QPP(MALE,WER,PLURAL)+
	" Arme gehen durch Dich hindurch.\n");
    if (for_all) {
      out_sel="Du umarmst @@alle@@@@adverb@@.";
      out_vic="@@Name@@ umarmt @@alle@@@@adverb@@.";
      return MixedOut(WEN,"umarm","nett",adverb,TOUCH);
    }
    if (who==this_player()) {
      out_sel=
        "Du legst Deine Arme um Dich und wärmst Dich ein bisschen selber.";
      out_oth=
        "@@Name@@ legt "+QPP(MALE,WER,PLURAL)+
        " Arme um sich und wärmt sich ein bisschen selber.";
    }
    else {
      out_sel="Du umarmst@@ wen@@@@adverb@@.";
      out_vic="@@Name@@ umarmt Dich@@adverb@@.";
      out_oth="@@Name@@ umarmt@@ wen@@@@adverb@@.";
    }
    return FeelIt("umarm","nett",adverb,TOUCH);

    /**************** Verblueffen *****************/
    case "verblueff":
    case "verblüff":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du verblüffst @@alle@@"+(adverb ? "@@adverb@@." :
              " mit einem Zaubertrick.");
      out_vic="@@name@@ ermuntert @@alle@@" +(adverb ? "@@adverb@@." :
              " mit einem Zaubertrick.");
      return MixedOut(WEN,"verblüff","albern",adverb,0);
    }
    if(CheckLife(NOT_SELF,NOT_DEAD,
          "Wieso willst Du Dich selbst verblüffen?",
          "Tote Dinge zu verblüffen hat wenig Sinn..."))
      return 1;
    out_sel="Du verblüffst "+who->name(WEN)+(adverb ?"@@adverb@@.":
            " mit einem Zaubertrick.");
    out_vic="@@gname@@ verblüfft Dich"+(adverb ? "@@adverb@@." :
            " mit einem Zaubertrick.");
    out_oth="@@gname@@ verblüfft "+who->name(WEN)+(adverb ?"@@adverb@@.":
            " mit einem Zaubertrick.");
    return FeelIt("verblüff","albern",adverb,0);

    /**************** Verfluchen ***************/
    case "verfluch":
    if (!str)
      Return("Wen oder was willst Du denn verfluchen?\n");
    ParseRest(str);
    str3=(ghost() ? "Der Geist von @@name@@ " : "@@name@@ ");
    if (!who)  {
      out_sel="Du verfluchst "+str+".";
      out_oth="@@gname@@ verflucht "+str+".";
    }
    else  {
      if (who==this_player())
	Return("Sich selber verflucht man besser nicht...\n");
      if (!adverb) {
	flag=sscanf(str, "%s %s", str1,str2);
        str2 = (flag==2 ? " "+str2+"." : ".");
	out_sel="Du verfluchst @@wen@@"+str2;
	out_vic="@@gname@@ verflucht Dich"+str2;
	out_oth="@@gname@@ verflucht @@wen@@"+str2;
      }
      else {
	out_sel="Du verfluchst @@wen@@@@adverb@@.";
	out_vic="@@gname@@ verflucht Dich@@adverb@@.";
	out_oth="@@gname@@ verflucht @@wen@@@@adverb@@.";
      }
    }
    return FeelIt("verfluch","fies",adverb,0);

    /**************** Verneigen / Verbeugen ***************/
    case "verneig":
    case "verbeug":
    GHOSTCHECK(
        "Du verneigst Dich ein bisschen heftig - "
        "Dein Kopf taucht kurz in den Boden.\n",
        gname()+" verneigt sich. Ein bisschen heftig - "+
        QPP(MALE,WER)+" Kopf\ntaucht kurz in den Boden ein.\n", 0);
    if ((!str) || (str == "dich")) {
      out_sel="Du verneigst Dich vor den Anwesenden.";
      out_oth="@@Name@@ verneigt sich anmutig.";
    }
    else {
      ParseRest(str);
      if (for_all) {
	out_sel="Du verneigst Dich@@adverb@@ vor @@alle@@.";
	out_vic="@@Name@@ verneigt sich@@adverb@@ vor @@alle@@.";
	return MixedOut(WEM,"verneig","nett",adverb,0);
      }
      if (!who && !adverb)
	Return("Verneige dich irgendwie oder vor jemandem.\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
          "Wie willst Du das denn schaffen?",
          "Vor Sachen wird hier nicht verneigt, klar?\n"))
	return 1;
      out_sel="Du verneigst Dich@@adverb@@"+(who ? " vor" : "")+"@@ wem@@.";
      if (who) out_vic="@@Name@@ verneigt sich@@adverb@@ vor Dir.";
      out_oth=
        "@@Name@@ verneigt sich@@adverb@@"+(who ? " vor" : "")+"@@ wem@@.";
    }
    return FeelIt("verneig","nett",adverb,0);

    /**************** Verneinen ***************/
    case "nein":
    case "noe":
    case "nö":
    GHOSTCHECK(
        "Du schüttelst Deinen Kopf so heftig, dass er kurz davonschwebt.\n",
        gname()+" schüttelt heftig den Kopf.\nSo heftig, dass dieser kurz "
        "davonschwebt und wieder eingefangen werden muss.\n", 0);
    ParseRest(str);
    if (str && !adverb)
      Return("Schüttle wie den Kopf?\n");
    out_sel="Du schüttelst@@adverb@@ den Kopf.";
    out_oth="@@Name@@ schüttelt@@adverb@@ den Kopf.";
    return FeelIt("nein","neutral",adverb,0);

    /******************* Verzweifeln ***************/
    case "rauf":
    case "verzweifel":
    case "verzweifl":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du raufst Dir vor Verzweiflung wegen @@alle@@@@adverb@@ die "
              "Haare.";
      out_vic="@@name@@ rauft sich vor Verzweiflung wegen @@alle@@@@adverb@@ "
              "die Haare.";
      return MixedOut(WEM,"verzweifel","traurig",adverb,0);
    }
    if(str && !adverb && !who)
      Return("Wie oder wegem Wem willst Du verzweifeln?\n");
    if(who==this_player())
      who=0;
    out_sel="Du raufst Dir vor Verzweiflung"+(who?" wegen "+who->name(WEM):"")+
            "@@adverb@@ die Haare.";
    if(who) out_vic="@@name@@ rauft sich vor Verzweiflung wegen Dir@@adverb@@ "
                    "die Haare.";
    out_oth="@@name@@ rauft sich vor Verzweiflung"+(who?" wegen "+
            who->name(WEM):"")+"@@adverb@@ die Haare.";
    return FeelIt("verzweifel","traurig",adverb,0);

    /**************** Wackeln ***************/
    case "wackel":
    case "wackl":
    if (ghost())
      Return("Da gibt es nichts mehr, womit Du wackeln kannst.\n");
    ParseRest(str);
    if (str && !adverb)
      Return("Wackle wie?\n");
    out_sel="Du wackelst@@adverb@@ mit dem Hintern.";
    out_oth="@@Name@@ wackelt@@adverb@@ mit "+QPP(MALE,WEM)+" Hintern.";
    return FeelIt("wackel","frech",adverb,0);

    /**************** Waelzen ***************/
    case "waelz":
    case "wälz":
    ParseRest(str);
    if (str && !adverb)
      Return("Wälze Dich wie auf dem Boden?\n");
    out_sel="Du wälzt Dich"+(adverb ? "@@adverb@@" : " vor Lachen")+
      " auf dem Boden.";
    out_oth="@@gname@@ wälzt sich"+(adverb ? "@@adverb@@" : " vor Lachen")+
      (ghost() ? " im" : " auf dem")+" Boden.";
    return FeelIt("wälz","albern",adverb,0);

    /**************** Warten ***************/
    case "wart":
    ParseRest(str);
    if (!str) {
      out_sel="Du tippst mit dem Fuß auf dem Boden.";
      out_oth="@@gname@@ tippt mit dem Fuß auf dem Boden.";
    } else if (!who && adverb) {
      out_sel="Du wartest@@adverb@@.";
      out_oth="@@gname@@ wartet@@adverb@@.";
    } else {
      if (who &&  CheckLife(NOT_SELF,0,
          "Wieso warten? Du bist doch schon da!", ""))
        return 1;
      out_sel="Du wartest@@adverb@@ auf "+(who ? "@@wen@@." : str+".");
      if (who) out_vic="@@gname@@ wartet@@adverb@@ auf Dich.";
      out_oth="@@gname@@ wartet@@adverb@@ auf "+(who ? "@@wen@@." : str+".");
    }
    return FeelIt("wart","neutral",adverb,0);

    /**************** Weinen ***************/
    case "wein":
    GHOSTCHECK("Es reicht leider nur für ein paar winzige Nebelwölkchen, "
        "nicht für Tränen.\n",
        gname()+" verzieht das Gesicht und ein paar winzige Nebel-\n"
        "wölkchen entfernen sich von seinen \"Augen\".\n", 0);
    if (!str) {
      out_sel="Waaaaah! Du weinst bitterlich.";
      out_oth="@@Name@@ bricht in Tränen aus und weint bitterlich.";
    }
    else {
      ParseRest(str);
      if (!who && !adverb)
	Return("Weine Dich irgendwie bei irgendwem aus, aber nicht so.\n");
      if (who && CheckLife(NOT_SELF,NOT_DEAD,
          "Bei sich selber kann man sich so schlecht ausweinen.",
          "Bei wem willst Du Dich ausweinen???"))
	return 1;
      if (who) {
	out_sel="Du weinst Dich@@adverb@@ bei @@wem@@ aus.";
	out_vic="@@Name@@ weint sich@@adverb@@ bei Dir aus.";
	out_oth="@@Name@@ weint sich@@adverb@@ bei @@wem@@ aus.";
      }
      else {
	out_sel="Du brichst in Tränen aus und weinst@@adverb@@.";
	out_oth="@@Name@@ bricht in Tränen aus und weint@@adverb@@.";
      }
    }
    return FeelIt("wein","traurig",adverb,0);

   /****************** Wiehern ******************/
   case "wieher":
   ParseRest(str);
   out_sel="Du wieherst@@adverb@@ auf.";
   out_oth="@@name@@ wiehert@@adverb@@ auf.";
   return FeelIt("wieher","albern",adverb,0);

    /**************** Wimmern ***************/
    case "wimmer":
    ParseRest(str);
    out_sel="Du wimmerst"+(adverb ? "@@adverb@@." : " um Gnade.");
    out_oth="@@gname@@ wimmert"+(adverb ? "@@adverb@@." : " um Gnade.");
    return FeelIt("wimmer","ängstlich",adverb,0);

    /**************** Winseln ******************/
    case "winsel":
    case "winsl":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du winselst@@adverb@@ vor @@alle@@.";
      out_vic="@@name@@ winselt@@adverb@@ vor @@alle@@.";
      return MixedOut(WEM,"winsel","traurig",adverb,0);
    }
    if(str && !adverb && !who)
      Return("Wie willst Du Wen anwinseln?\n");
    if(CheckLife(NOT_SELF,0,"Soweit wollen wir es doch nicht kommen lassen!",0))
      return 1;
    out_sel="Du winselst@@adverb@@"+(who?" vor "+who->name(WEM):"")+".";
    if(who) out_vic="@@name@@ winselt@@adverb@@ vor Dir.";
    out_oth="@@name@@ winselt@@adverb@@"+(who?" vor "+who->name(WEM):"")+".";
    return FeelIt("winsel","traurig",adverb,0);

    /**************** Winken ***************/
    case "wink":
    ParseRest(str);
    if (for_all)  {
      out_sel="Du winkst @@alle@@@@adverb@@ zu.";
      out_vic="@@gname@@ winkt @@alle@@@@adverb@@ zu.";
      return MixedOut(WEM,"wink","nett",adverb,0);
    }
    if (!who && !adverb && str)
      Return("Vielleicht solltest Du auch sagen, "
          "wem oder wie Du (zu)winken willst.\n");
    if (who && CheckLife(NOT_SELF, NOT_DEAD,
        "Wink Dir nicht selber zu.",
        "Du musst schon einem Lebewesen zuwinken."))
      return 1;
    out_sel="Du winkst@@ wem@@@@adverb@@"+(who ? " zu" : "")+".";
    if (who) out_vic="@@gname@@ winkt Dir@@adverb@@ zu.";
    out_oth="@@gname@@ winkt@@ wem@@@@adverb@@"+(who ? " zu" : "")+".";
    return FeelIt("wink","nett",adverb,0);

    /**************** Wischen ***************/
    case "wisch":
    case "schwitz":
    GHOSTCHECK(
        "Aber Geister schwitzen doch nicht. "
        "Also wieso den Schweiß abwischen?\n", "", 0);
    ParseRest(str, "schweiß");
    if (str && str!="schweiß" && str!="schweiß ab" && !adverb)
      Return("Wische Dir den Schweiß wie ab?\n");
    out_sel="Du wischt Dir@@adverb@@ den Schweiß von der Stirn.";
    out_oth="@@Name@@ wischt sich@@adverb@@ den Schweiß von der Stirn.";
    return FeelIt("schwitz","neutral",adverb,0);

    /**************** Wuergen ***************/
    case "wuerg":
    case "erwuerg":
    case "würg":
    case "erwürg":
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Würge wen oder wie?\n");
    if (!who) {
      out_sel="Du fängst@@adverb@@ an zu würgen.";
      out_oth="@@gname@@ fängt@@adverb@@ an zu würgen.";
    }
    else {
      if (CheckLife(0, NOT_DEAD, "",
        "Tote Sachen zu würgen macht Doch gar keinen Spaß!"))
      return 1;
      if( who == this_player() )
      {
        out_sel="Du springst Dich an und fängst an, Dich@@adverb@@ "
          "zu würgen.";
        out_oth="@@gname@@ würgt sich selbst@@adverb@@.";
      }
      else
      {
        out_sel="Du springst @@wen@@ an und fängst an, "+
          who->QueryPronoun(WEN)+"@@adverb@@ zu würgen.";
        out_vic="@@gname@@ springt Dich auf einmal an und würgt "
          "Dich@@adverb@@.";
        out_oth="@@gname@@ springt auf einmal @@wen@@ an und würgt "
  	  +who->QueryPronoun(WEN)+"@@adverb@@.";
      }
    }
    return FeelIt("würg","fies",adverb,TOUCH);

    /************** Glueck Wuenschen ***************/
    case "wuensch":
    case "wünsch":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du wünscht @@alle@@@@adverb@@ Glück.";
      out_vic="@@name@@ wünscht @@alle@@@@adverb@@ Glück.";
      return MixedOut(WEM,"wünsch","nett",adverb,0);
    }
    if(CheckLife(NOT_SELF,NOT_DEAD,
          "Lass Dir doch von jemand anderem Glück wünschen!",
          "Tote Dinge brauchen kein Glück.."))
      return 1;
    out_sel="Du wünscht "+who->name(WEM)+"@@adverb@@ Glück.";
    out_vic="@@gname@@ wünscht Dir@@adverb@@ Glück.";
    out_oth="@@gname@@ wünscht "+who->name(WEM)+"@@adverb@@ Glück.";
    return FeelIt("wünsch","nett",adverb,0);

    /**************** Wundern ***************/
    case "wunder":
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Wie oder über wen willst Du Dich wundern?\n");
    out_sel="Du wunderst Dich@@adverb@@"+(who?who==this_player()?" über Dich selbst.":" über @@wen@@." : ".");
    if (who&&who!=this_player()) out_vic="@@gname@@ wundert sich@@adverb@@ über Dich.";
    out_oth="@@gname@@ wundert sich@@adverb@@"+(who?who==this_player()?" über sich selbst.":" über @@wen@@." : ".");
    return FeelIt("wunder","neutral",adverb,0);

    /**************** Wuscheln ***************/
    case "wuschel":
    case "wuschl":
    ParseRest(str);
    if (for_all)  {
      out_sel="Du wuschelst @@alle@@@@adverb@@ kräftig durch.";
      out_vic="@@gname@@ wuschelt @@alle@@@@adverb@@ kräftig durch.";
      return MixedOut(WEN,"wuschel","nett",adverb,0);
    }
    if (!who)
      Return("Wen willst Du denn wuscheln?\n");

    if (CheckLife(0, NOT_DEAD, "", "Hmm, sehr tot. Ne, lieber nicht."))
      return 1;
    if (who->QueryProp(P_FROG)) {
      write("Du verwuschelst...  äh... hm. "
          "Ein Frosch hat wohl nix zum Wuscheln.\n");
      return 1;
    }
    if (member(({"aloy"}),
               who->query_real_name())>-1)
      switch (who->query_real_name())
      {
        case "aloy"   : str1="Flammenhaare";break;
      }
    else if (who->QueryProp(P_RACE))
      switch (lower_case(who->QueryProp(P_RACE)))
      {
        case "fisch"         :
        case "echse"         :
        case "drache"        : str1="Schuppen"; break;
        case "erzgreif"      :
        case "greif"         :
        case "vogel"         : str1="Federn"; break;
        case "pferd"         :
        case "löwe"         : str1="Mähne"; break;
        case "hobbit"        :
        case "schneekatze"   :
        case "katze"         :
        case "tier"          :
        case "steinbeißer"  :
        case "knuddeleisbär": str1="Fell"; break;
        default              : str1="Haare";
      }
    else str1="Haare";
    if( who->id("SCHUPPEN_WUSCHEL")) str1="Schuppen";
    if( who->id("FEDERN_WUSCHEL")) str1="Federn";
    if( who->id("FELL_WUSCHEL")) str1="Fell";
    if( who->id("MAEHNE_WUSCHEL")) str1="Mähne";
    if( who->id("HAARE_WUSCHEL")) str1="Haare";
//XXXXXX           01.08.2002 by Serii@Tamedhon
    if( who->id("STEIN_WUSCHEL")) str1="|Felsenschädel";
    if( who->id("HAUT_WUSCHEL")) str1="|Glatzkopf";

    if(str1[0..0]!="|")
    {
//XXXXXX
      out_sel="Du verwuschelst@@adverb@@ "+
        (who==this_player() ? (str1=="Fell" ? "Dein eigenes " : "Deine eigenen ")
         : "@@wessen@@ ")+str1+".";
      if (who!=this_player()) out_vic="@@name@@ verwuschelt@@adverb@@ Dein"+
        (str1=="Fell" ? " " : "e ")+str1+".";
      out_oth="@@gname@@ verwuschelt@@adverb@@ "+
        (who==this_player() ?
           this_player()->QueryPossPronoun(NEUTER,WEN)+
             (str1=="Fell" ? " eigenes " : "e eigenen ")
           : "@@wessen@@ ")+str1+".";
//XXXXXX           01.08.2002 by Serii@Tamedhon
    }
    else
    {
      str1=str1[1..];
      out_sel="Du rubbelst@@adverb@@ über "+
        (who==this_player() ? "Deinen eigenen "
         : "@@wessen@@ ")+str1+".";
      if (who!=this_player()) out_vic="@@name@@ rubbelt@@adverb@@ über Deinen "+
         str1+".";
      out_oth="@@gname@@ rubbelt@@adverb@@ über "+
        (who==this_player() ?
           this_player()->QueryPossPronoun(NEUTER,WEN)+
             "en eigenen "
           : "@@wessen@@ ")+str1+".";
    }
//XXXXXX
    return FeelIt("wuschel","nett",adverb,TOUCH);

    /**************** Zittern ***************/
    case "zitter":
    ParseRest(str, "vor");
    if (str && !adverb && !who) Return("Zittere wie oder vor wem?\n");
    out_sel="Du zitterst"+
      (adverb ? "@@adverb@@" : (who ? " aus Angst" : " vor Angst"))+
      (who ? " vor @@wem@@." : ".");
    if (who) out_vic="@@gname@@ zittert"+
      (adverb ? "@@adverb@@" : " aus Angst")+" vor Dir.";
    out_oth="@@gname@@ zittert"+
      (adverb ? "@@adverb@@" : (who ? " aus Angst" : " vor Angst"))+
      (who ? " vor @@wem@@." : ".");
    return FeelIt("zitter","aengstlich",adverb,0);

    /**************** Zupfen ***************/
    case "zupf":
    ParseRest(str, ({"ärmel", "am ärmel"}) );
    if (!who)
      Return("Zupfe wen (am Ärmel)?\n");
    if (CheckLife(NOT_SELF,NOT_DEAD,
        "Dich selbst am Ärmel zupfen? Ne, das ist viel zu albern, lass es.",
        "Keine Reaktion. Vielleicht solltest Du Dich auf Lebewesen "
        "beschränken?"))
      return 1;
    out_sel="Du zupfst @@wen@@"+(adverb ? "@@adverb@@" : " ungeduldig")+
      " am Ärmel.";
    out_vic="@@gname@@ zupft Dich"+(adverb ? "@@adverb@@" : " ungeduldig")+
      " am Ärmel.";
    out_oth="@@gname@@ zupft @@wen@@"+(adverb ? "@@adverb@@" : " ungeduldig")+
      " am Ärmel.";
    return FeelIt("zupf","frech",adverb,TOUCH);

   /******************** Zweifeln ****************/
    case "zweifel":
    case "zweifl":
    ParseRest(str);
    if(str && !who && !adverb)
      Return("Wie willst Du an Wessen Überlegungen zweifeln?\n");
    if(CheckLife(0,NOT_DEAD,0,"Da gibts wohl nicht viel anzuzweifeln!"))
      return 1;
    if(who==this_player())
      who=0;
    out_sel="Du zweifelst an "+(who?who->name(WESSEN):"Deinen")+
            " Überlegungen@@adverb@@.";
    if(who) out_vic="@@name@@ zweifelt an Deinen Überlegungen@@adverb@@.";
    out_oth="@@name@@ zweifelt an "+
            (who?who->name(WESSEN):QPP(WESSEN,FEMALE,PLURAL))+" ";
    out_oth+="Überlegungen@@adverb@@.";
    return FeelIt("zweifel","neutral",adverb,0);

   /***************** Zwicken ****************/
    case "zwick":
    ParseRest(str);
    if(for_all)
    {
      out_sel="Du zwickst @@alle@@@@adverb@@ in das Hinterteil und hörst "
              "ein 'quiieeeks'.";
      out_vic="@@name@@ zwickt @@alle@@@@adverb@@ in das Hinterteil und Du "
              "machst 'quiieeeks'.";
      return MixedOut(WEN,"zwick","frech",adverb,TOUCH);
    }
    if(str && !adverb && !who)
      Return("Wen willst Du Wie zwicken?\n");
    if(who==this_player())
      who=0;
    out_sel="Du zwickst "+(who ? who->name(WEN):"Dich")+"@@adverb@@ in das "
            "Hinterteil.\n"+(who ? who->name(WER)+" quiekt.":"Du quiekst.");
    if(who) out_vic="@@name@@ zwickt Dich@@adverb@@ in Dein Hinterteil. "
                    "*Autsch*";
    out_oth="@@name@@ zwickt "+(who ? who->name(WEN):"sich")+"@@adverb@@ in "
            "das Hinterteil.\n"+(who ? who->name(WER):"@@name@@")+
            " zuckt zusammen und quiekt.";
    return FeelIt("zwick","frech",adverb,TOUCH);

    /**************** Zwinkern ***************/
    case "zwinker":
    if (ghost())
      Return("Vergiss es - das ist als Geist viel zu unauffällig, als dass\n"
          "es andere Leute sehen würden.\n");
    ParseRest(str);
    if (str && !who && !adverb)
      Return("Zwinkere wie? Zwinkere wem zu?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Du kannst Dir nicht selber zuzwinkern.",
        "Wieso reagiert das Ding da nicht auf Dein Zwinkern? "
        "Ist es etwa tot?"))
      return 1;
    out_sel="Du zwinkerst@@ wem@@@@adverb@@"+(who ? " zu." : ".");
    if (who) out_vic="@@Name@@ zwinkert Dir@@adverb@@ zu.";
    out_oth="@@Name@@ zwinkert@@ wem@@@@adverb@@"+(who ? " zu." : ".");
    return FeelIt("zwinker","nett",adverb,0);

    // Spezialsachen - Geisterverben und Magierverben

    /**************** Rasseln ***************/
    case "rassel":
    case "rassl":
    if (!ghost())
      Return("Das ist nicht Dein Fachgebiet - Du bist doch kein Geist!\n");
    ParseRest(str);
    if (str && !adverb)
      Return("Rassel wie?\n");
    out_sel="Du rasselst"+(adverb ? "@@adverb@@" : " fürchterlich")+
      " mit einer rostigen Rasselkette,\n"
      "die Du auf einmal für einen Moment in der Hand hältst.";
    out_oth="@@gname@@ holt auf einmal eine rostige Rasselkette aus\n"
      "dem Nichts und fängt an,"+(adverb ? "@@adverb@@" : " fürchterlich")+
      " damit zu rasseln.\n"
      "Danach ist die Kette auf einmal wieder verschwunden.";
    return FeelIt("rassel","neutral",adverb,0);

    /**************** Heulen ***************/
    case "heul":
    if (!ghost())
      Return("Überlass das mal den Fachleuten (also den Geistern).\n");
    ParseRest(str);
    if (str && !adverb)
      Return("Heule wie?\n");
    out_sel="Du heulst"+(adverb ? "@@adverb@@." : " schauerlich.");
    out_oth="@@gname@@ heult"+(adverb ? "@@adverb@@." : " schauerlich.");
    return FeelIt("heul","neutral",adverb,0);

    /**************** Nassspritzen ***************/
    case "splash":
    if (!IS_WIZARD(this_player()) && !(IS_SEER(this_player()) &&
        present("SEHER\nspritzpistole",this_player())))
      return 0;
    ParseRest(str);
    if (for_all)  {
      out_sel="Du ziehst Deine Wasserpistole und spritzt @@alle@@"
        "@@adverb@@ patschnass.";
      out_vic="@@gname@@ zieht "+QPP(FEMALE,WEN)+" Wasserpistole und spritzt "
	+"@@alle@@@@adverb@@ patschnass.";
      return MixedOut(WEN,"splash","frech",adverb,0);
    }
    if (!who)
      Return("Wen willst Du denn nass machen?\n");
    if (who == this_player())  {
      out_sel="Sag mal, kommst Du Dir nicht ein bisschen doof vor?\n"
	"Du ziehst Deine Wasserpistole und spritzt Dich@@adverb@@ "
	"selber patschnass.";
      out_oth="@@gname@@ zieht "+QPP(FEMALE,WEN)+
        " Wasserpistole und spritzt sich@@adverb@@ "
	+"aus unerfindlichen Gründen selbst patschnass.";
    }
    else {
      out_sel="Du ziehst Deine Wasserpistole und spritzt @@wen@@"
        "@@adverb@@ patschnass.";
      out_vic=gname()+" zieht "+QPP(FEMALE,WEN)+" Wasserpistole und spritzt "
	+"Dich@@adverb@@ patschnass.";
      out_oth=gname()+" zieht "+QPP(FEMALE,WEN)+" Wasserpistole und spritzt "
	+"@@wen@@@@adverb@@ patschnass.";
    }
    return FeelIt("splash","frech",adverb,0);

    /**************** Anflammen ***************/
    case "flam":
    if (!IS_WIZARD(this_player()) && !(IS_SEER(this_player()) &&
        present("SEHER\nflammenwerfer",this_player())))
      return 0;
    if (ghost())
      Return("Du hast leider grade Deinen Flammenwerfer nicht dabei.\n");
    ParseRest(str);
    ifoo=!random(7);
    if (for_all) {
      out_sel="Du holst aus Deinen tiefsten Taschen (oder was weiß denn ich "
        "woher) Deinen "+MUDNAME+" handgearbeiteten Mini-Flammenwerfer (tm), "
        "richtest ihn aus und...\n"
        +(ifoo ? "schaust leicht frustriert auf das Streichholz, in das er "
                 "sich verwandelt hat."
	      : "feuerst@@adverb@@ einen riesigen Feuerball auf @@alle@@ ab.\n"
                "Es riecht auf einmal so verbrannt hier...");
      out_vic="@@Name@@ holt auf einmal irgendwoher einen "+MUDNAME+" "
        "handgearbeiteten\nMini-Flammenwerfer (tm), richtet ihn aus und...\n"+
	(ifoo ? "schaut ziemlich frustriert auf das Streichholz, in das sich "
                "das Ding verwandelt hat."
	      : "feuert@@adverb@@ einen riesigen Feuerball auf @@alle@@ ab.\n"
                "Dir wird so warm um's Herz...");
      return MixedOut(WEN,"flam","frech",adverb,0);
    }

    if (!who)
      Return("Wen willst Du denn ankokeln?\n");

    out_sel="Du holst aus Deinen tiefsten Taschen (oder was weiß denn ich "
      "woher) Deinen\n"+MUDNAME+" handgearbeiteten Mini-Flammenwerfer (tm), "
      "richtest ihn aus und...\n"+
      (ifoo ? "schaust leicht frustriert auf das Streichholz, "
              "in das er sich verwandelt hat."
	    : "feuerst@@adverb@@ einen riesigen Feuerball auf @@wen@@ ab.\n"
              "Es riecht auf einmal so verbrannt hier...");

    out_vic="@@Name@@ holt auf einmal irgendwoher einen "+MUDNAME+" "
      "handgearbeiteten\nMini-Flammenwerfer (tm), richtet ihn auf "
      "Dich aus und...\n"+
      (ifoo ? "schaut ziemlich frustriert auf das Streichholz, "
              "in das sich das Ding\nverwandelt hat."
	    : "feuert@@adverb@@ einen riesigen Feuerball auf Dich ab.\n"
              "Dir wird so warm ums Herz...");

    out_oth="@@Name@@ holt auf einmal irgendwoher einen "+MUDNAME+" "
      "handgearbeiteten\nMini-Flammenwerfer (tm), richtet ihn auf"
      "@@ wen@@ aus und...\n"+
      (ifoo ? "schaut ziemlich frustriert auf das Streichholz, "
              "in das sich das Ding\nverwandelt hat."
	    : "feuert@@adverb@@ einen riesigen Feuerball auf@@ wen@@ ab.\n"
              "Es riecht auf einmal irgendwie verbrannt hier...");

    return FeelIt("flam","frech",adverb,0);

    /***************** Schwappen ***************/
    case "schwapp":
    if (IS_ARCH(this_player()))
    {
      ParseRest(str);
      if (str && !adverb)
        Return("Wie willst Du schwappen?\n");
      out_sel="Du machst magische Handbewegungen. Plötzlich erscheint ein "
        "Hologramm\nneben Dir und macht@@adverb@@: SCHWAPP!!!";
      out_oth="@@gname@@ fuchtelt magisch in der Luft herum. "
        "Plötzlich erscheint\nein Hologramm und macht@@adverb@@: SCHWAPP!!!";
      return FeelIt("schwapp","albern",adverb,0);
    }
    else return 0;

    // Special 2: remote verbs

// Part von Creative@AnderLand:
// abgeschaut von Calum@MorgenGrauen 's Diddl-Knuddelmaus!
// zumindest rgrinsen & rgratulieren sollte man koennen.

    /**************** Remote gratulieren ***************/
    case "rgrats":
    case "rgratulier":
    if (ParseRemote(str, vb)) return 1;
    if (!who)
      Return("Gratuliere wem?\n");
    if (CheckLife(NOT_SELF, 0, "Dir selbst? Aus der Ferne?", 0))
      return 1;
    if (present(who, environment()))
      Return("Wenn jemand neben Dir steht, nimm gratuliere.\n");
    out_sel="Du gratulierst @@wem@@@@adverb@@ aus der Ferne.";
    out_vic="@@gname@@ gratuliert Dir@@adverb@@ aus der Ferne.";
    return FeelIt("rgrats","nett",adverb,0);

    /**************** Remote grinsen ***************/
    case "rgrins":
    if (ParseRemote(str, vb)) return 1;
    if (!who)
      Return("Grinse wen an?\n");
    if (CheckLife(NOT_SELF, 0, "Dich selbst? Aus der Ferne?", 0))
      return 1;
    if (present(who, environment()))
      Return("Wenn jemand neben Dir steht, nimm grinse.\n");
    out_sel="Du grinst @@wen@@@@adverb@@ aus der Ferne an.";
    out_vic="@@gname@@ grinst Dich@@adverb@@ aus der Ferne an.";
    return FeelIt("rgrins","frech",adverb,0);

// original

    /**************** Remote knuddeln ***************/
    case "rknuddel":
    case "rknuddl":
    if (ParseRemote(str, vb)) return 1;
    if (!who)
      Return("Knuddle wen?\n");
    if (CheckLife(NOT_SELF, 0, "Das bringt doch nix, lass es halt.", 0))
      return 1;
    if (present(who, environment()))
      Return("Wenn jemand neben Dir steht, nimm knuddel.\n");
    out_sel="Du knuddelst @@wen@@@@adverb@@ aus der Ferne.";
    out_vic="@@gname@@ knuddelt Dich@@adverb@@ aus der Ferne.";
    return FeelIt("rknuddel","nett",adverb,0);

    /* remote kuscheln */
    case "rkuschel":
    case "rkuschle":
    if (ParseRemote(str, vb)) return 1;
    if (!who)
    Return("An wen ankuscheln?\n");
    if (CheckLife(NOT_SELF, 0, "Das bringt doch nix, lass es halt.", 0))
       return 1;
    if (present(who, environment()))
        Return("Wenn jemand neben Dir steht, nimm kuschel.\n");
	out_sel="Du kuschelst Dich an @@wen@@@@adverb@@ aus der Ferne an.\n";
	out_vic="@@gname@@ kuschelt sich an Dich@@adverb@@ aus der Ferne an.";
	return FeelIt("rkuschel","nett",adverb,0);

   /* rumarme */
   case "rumarm":
   case "rumarme":
   if (ParseRemote(str, vb)) return 1;
   if (!who)
   Return("Wen willst Du umarmen?\n");
   if (CheckLife(NOT_SELF, 0, "Das bringt doch nix, lass es halt.", 0))
       return 1;
   if (present(who, environment()))
       Return("Wenn jemand neben Dir steht, nimm umarme.\n");
       out_sel="Du umarmst @@wen@@@@adverb@@ aus der Ferne.\n";
       out_vic="@@gname@@ umarmt Dich@@adverb@@ aus der Ferne.";
       return FeelIt("rumarme","nett",adverb,0);

    /**************** Remote danken *****************/
    case "rdanke":
    case "rdank":
    if (ParseRemote(str, vb)) return 1;
    if (!who)
      Return("Danke wem?\n");
    if (CheckLife(NOT_SELF, 0, "Du bist einfach toll! Auch aus der Ferne.", 0))
      return 1;
    if (present(who, environment()))
      Return("Wenn jemand neben Dir steht, nimm danke.\n");
    out_sel="Du dankst @@wem@@@@adverb@@ aus der Ferne.";
    out_vic="@@gname@@ dankt Dir@@adverb@@ aus der Ferne.";
    return FeelIt("rdanke","nett",adverb,0);

    /**************** Remote nicken ***************/
    case "rja":
    case "rnicke":
    case "rnick":
    ParseRemote(str, vb);
    if (!who)
      Return("Wem willst Du zunicken?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Du kommst Dir irgendwie doof vor bei dem Gedanken "+
        "Dir selbst zuzunicken aus der Ferne.",
        "Das bringt wirklich nix..."))
      return 1;
    if (present(who, environment()))
      Return("Wenn jemand neben Dir steht, nimm nicke.\n");
    out_sel="Du nickst@@ wem@@@@adverb@@ aus der Ferne zu.";
    out_vic="@@gname@@ nickt Dir@@adverb@@ aus der Ferne zu.";
    return FeelIt("rnicke","nett",adverb,0);

    /**************** Remote stupsen ***************/
    case "rstups":
    case "rstupse":
    ParseRemote(str, vb);
    if (!who)
      Return("Wen willst Du anstupsen?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Du kommst Dir irgendwie doof vor bei dem Gedanken. ",
        "Das bringt wirklich nix..."))
      return 1;
    if (present(who, environment()))
      Return("Wenn jemand neben Dir steht, nimm stupse.\n");
    out_sel="Du stupst@@ wen@@@@adverb@@ aus der Ferne an.";
    out_vic="@@gname@@ stupst Dich @@adverb@@aus der Ferne an.";
    return FeelIt("rstupse","nett",adverb,0);

    /**************** Remote Laecheln ***************/
    case "rlaechel":
    case "rlaechle":
    case "rlächel":
    case "rlächle":
    ParseRemote(str, vb);
    if (!who)
      Return("Wen willst Du anlächeln?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Das würde etwas bescheuert aussehen...",
        "Das bringt wirklich nix..."))
      return 1;
    if (present(who, environment()))
      Return("Wenn jemand neben Dir steht, nimm lächel.\n");
    out_sel="Du lächelst@@ wen@@@@adverb@@ aus der Ferne an.";
    out_vic="@@gname@@ lächelt Dich@@adverb@@ aus der Ferne an.";
    return FeelIt("rlächel","nett",adverb,0);

    /**************** Remote Troesten ***************/
    case "rtroest":
    case "rtroeste":
    case "rtröst":
    case "rtröste":
    ParseRemote(str, vb);
    if (!who)
      Return("Wen willst Du trösten?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Trost musst Du bei jemand anders suchen.",
        "Das Teil musst Du nicht trösten, das fühlt nix."))
      return 1;
    if (present(who, environment()))
      Return("Wenn jemand neben Dir steht, nimm tröste.\n");
    out_sel="Du tröstest@@ wen@@@@adverb@@ aus der Ferne.";
    out_vic="@@gname@@ tröstet Dich@@adverb@@ aus der Ferne.";
    return FeelIt("rtröst","nett",adverb,0);

    /**************** Remote Wuscheln ***************/

    case "rwuschel":
    case "rwuschle":
    case "rwuschl":
    ParseRemote(str, vb);
    if (!who)
      Return("Wen willst Du denn wuscheln?\n");
    if (who && CheckLife(NOT_SELF,NOT_DEAD,
        "Wie willst Du Dich denn selbst aus der Ferne wuscheln?.",
        "Hmm, sehr tot. Ne, lieber nicht."))
      return 1;
    if (present(who, environment()))
      Return("Wenn jemand neben Dir steht, nimm wuschel.\n");
    if (who->QueryProp(P_FROG)) {
      write("Du verwuschelst...  aeh... hm. "
          "Ein Frosch hat wohl nix zum Wuscheln.\n");
      return 1;
    }
    if (member(({"aloy"}),
               who->query_real_name())>-1)
      switch (who->query_real_name())
      {
        case "aloy"   : str1="Flammenhaare";break;
      }
    else if (who->QueryProp(P_RACE))
      switch (lower_case(who->QueryProp(P_RACE)))
      {
        case "fisch"         :
        case "echse"         :
        case "drache"        : str1="Schuppen"; break;
        case "erzgreif"      :
        case "greif"         :
        case "vogel"         : str1="Federn"; break;
        case "pferd"         :
        case "löwe"         : str1="Mähne"; break;
        case "hobbit"        :
        case "schneekatze"   :
        case "katze"         :
        case "tier"          :
        case "steinbeißer"  :
        case "knuddeleisbär": str1="Fell"; break;
        default              : str1="Haare";
      }
    else str1="Haare";
    if( who->id("SCHUPPEN_WUSCHEL")) str1="Schuppen";
    if( who->id("FEDERN_WUSCHEL")) str1="Federn";
    if( who->id("FELL_WUSCHEL")) str1="Fell";
    if( who->id("MAEHNE_WUSCHEL")) str1="Mähne";
    if( who->id("HAARE_WUSCHEL")) str1="Haare";
//XXXXXX           01.08.2002 by Serii@Tamedhon
    if( who->id("STEIN_WUSCHEL")) str1="|Felsenschädel";
    if( who->id("HAUT_WUSCHEL")) str1="|Glatzkopf";

    if(str1[0..0]!="|")
    {
//XXXXXX
    out_sel="Du verwuschelst@@adverb@@ "+
      (who==this_player() ? (str1=="Fell" ? "Dein eigenes " : "Deine eigenen ")
       : "@@wessen@@ ")+str1+" aus der Ferne.";
    if (who!=this_player()) out_vic="@@name@@ verwuschelt@@adverb@@ Dein"+
      (str1=="Fell" ? " " : "e ")+str1+" aus der Ferne.";
//XXXXXX           01.08.2002 by Serii@Tamedhon
    }
    else
    {
      str1=str1[1..];
      out_sel="Du rubbelst@@adverb@@ aus der Ferne ueber "+
        (who==this_player() ? "Deinen eigenen "
         : "@@wessen@@ ")+str1+".";
      if (who!=this_player()) out_vic="@@name@@ rubbelt@@adverb@@ aus der Ferne über Deinen "+
         str1+".";
    }
//XXXXXX
    return FeelIt("wuschel","nett",adverb,0);

    /**************** Remote winken ***************/
    case "rwink":
    if (ParseRemote(str, vb))
      return 1;
    if (!who)
      Return("Winke wem zu?\n");
    if (CheckLife(NOT_SELF, 0, "Sehr witzig. Pah.", 0))
      return 1;
    if (present(who, environment()))
      Return("Wenn jemand neben Dir steht, nimm wink.\n");
    out_sel="Du winkst @@wem@@@@adverb@@ aus der Ferne zu.";
    out_vic="@@gname@@ winkt Dir@@adverb@@ aus der Ferne zu.";
    return FeelIt("rwink","nett",adverb,0);

    /**************** Remote wuergen ***************/
    case "rwuerg":
    case "rwuerge": // JA! endlich remote wuergen :) G.
    case "rwürg":
    case "rwürge":
    if (ParseRemote(str, vb))
      return 1;
    if (!who)
      Return("Wen willste aus der Ferne würgen?\n");
    if (CheckLife(NOT_SELF, 0,
      "Du kannst Dich nicht selbst aus der Ferne würgen.", 0))
      return 1;
    if (present(who, environment()))
      Return("Wenn jemand neben Dir steht, nimm würg.\n");
    out_sel="Du würgst @@wen@@@@adverb@@ aus der Ferne.";
    out_vic="@@gname@@ würgt Dich@@adverb@@ aus der Ferne.";
    return FeelIt("rwürg","fies",adverb,0);

    /**************** Verbenliste ***************/
    case "verb":
    case "verben":
    More(SOULHELP->Help());
    return 1;

    /**************** Adverbienverwaltung ***************/
    case "adverb":
    case "adverbs":
    case "adverben":
    case "adverbien":  // Das ist die richtige Form, aber wer weiss das?
    {
      string f1,f2;
      if (!str || str=="#" || str=="$")
	return zeige_adverbs((str=="#" ? 1 : (str=="$" ? 2 : 0)));
      if (sscanf(str, "%s %s", f1,f2)==2)  {
	if (f1=="")
	  Return("Hm, da muss wohl ein Leerzeichen zu viel gewesen sein. "
              "Bitte nochmal,\naber ohne zuviele Leerzeichen.\n");
	if (f1=="?")
        {
	  string match;
	  match=QueryStdAdverbs()[f2] || plr_adverbs[f2];
	  if (match)
	    write("Die Abkürzung "+f2+" gehört zu dem Adverb:\n"+match+"\n");
	  else
	    write("Diese Abkürzung ist bisher nicht definiert.\n");
	  return 1;
	}
	if (QueryStdAdverbs()[f1])
	  Return("Die Standardabkürzungen können nicht neu definiert "
              "werden.\n");
	if (plr_adverbs[f1]) {
	  plr_adverbs[f1]=f2;
	  write("OK, Adverb mit der Abkürzung \""+f1+"\" auf \""+f2+
              "\" gesetzt.\n");
	}
	else {
	  if (sizeof(f1) > 6)
	    Return("Die Abkürzung ist zu lang, bitte nicht mehr als "
                "6 Zeichen.\n");
	  plr_adverbs[f1]=f2;
	  write("OK, neues Adverb \""+f2+"\" mit der Abkürzung \""+f1+
              "\".\n");
	}
      }
      else {
	if (QueryStdAdverbs()[str])
	  Return("Die Standardadverben können nicht gelöscht werden.\n");
	else if (!plr_adverbs[str])
	  Return(
"Du hast kein Adverb mit dieser Abkürzung.\n"
"Syntax: adverb, um die Adverbien anzuzeigen,\n"
"        adverb #, um nur Deine Adverbien anzuzeigen,\n"
"        adverb $, um nur die Standardadverbien anzuzeigen,\n"
"        adverb ? <Abkürzung>, um nachzusehen, ob <Abkürzung> definiert ist,\n"
"        adverb <Abkürzung> <Adverb>, um der <Abkürzung> das <Adverb>\n"
"               zuzuordnen,\n"
"        adverb <Abkürzung>, um das Adverb mit der <Abkürzung> zu löschen,\n");
	else  {
	  write("OK, Adverb \""+plr_adverbs[str]+"\" gelöscht.\n");
	  plr_adverbs=m_delete(plr_adverbs, str);
	}
      }
      return 1; }
    default: return 0;
  }
}
