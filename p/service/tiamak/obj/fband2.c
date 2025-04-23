// fband2.c  --  Das neue Freundschaftsband
//
// Idee und Umsetzung des "alten" Freundschaftsbandes:
//
//                Vrai@MorgenGrauen
// 
// Neue komplett ueberarbeitete Version:
//
//               Tiamak@MorgenGrauen
//
//
// Letzte Aenderung: 05.06.2003 Woody (Dir -> Euch)
//                   16.03.2004 Feyaria P_SCREENWIDTH verwenden
//                   25.03.2004 Feyaria fopt ort verstecken fuer magier
//                   20.04.2004 Feyaria in fopt ^= nach &= ~ geaendert 
//                   12.09.2004 Feyaria Query(P_GENDER), sonst buggts bei weiblichen Froeschen
 
/* {{{ Includes & Co */

inherit "/std/thing";

#pragma strict_types
#pragma combine_strings

#define NEED_PROTOTYPES

#include <defines.h>
#include <properties.h>
#include <moving.h>
#include <wizlevels.h>
#include <player/comm.h>
#include <new_skills.h>

#define ROOTDIR(x)         ("/p/service/tiamak/"+x)
#define OBJDIR(x)          (ROOTDIR("obj/"+x))
#define DOCDIR(x)          (ROOTDIR("doc/"+x))

#define FBNAME             (OBJDIR("fband2"))
#define FMASTER            (OBJDIR("fbmaster"))
#define HELPFILE           (DOCDIR("fband.txt"))
#define SEERHELP           (DOCDIR("fband_seher.txt"))
#define WIZARDHELP         (DOCDIR("fband_magier.txt"))

#define BANDID             "\n\bfband"

// #define P_CHAOS_NO_FRIENDS "chaos_no_friends" -> Gibts im SL nicht

#define ZEIGE_FREMDE       1
#define ZEIGE_EIGENE       2
#define ZEIGE_PFADE        4
#define KURZE_TITEL        8
#define ZEIGE_REGIONEN    16
#define VERSTECKE_MAGIER  32   

#define SELBSTGELOESCHT    1
#define INAKTIV            2
#define BAND_WEG           4

private static string *Freunde;
private static int logon, flags;
private static mapping angebot;

/* }}} */

/* {{{ Prototypes */

// Standard-Funktionen
void create();
string short();
varargs int move( mixed dest, int method );
void reset();
void BecomesNetAlive( object pl );
void BecomesNetDead( object pl );
varargs int remove( int silent );

// Query- und Set-Funktionen
static string _query_keep_on_sell();
static int _query_nodrop();
static mixed *_query_autoloadobj();
static mixed _set_autoloadobj( mixed wert );
static string _query_extralook();

// Eigene Funktionen (oeffentlich)
int ignoriere_scrolling();
void neues_angebot( string uid );
void freundschaft( string opfer );

// Hilfsfunktionen (intern)
private void zeige_anwesende();
private object neues_fband( object opfer );
private string ParseAdverben( string str );
private void fverb( string fremd, string selbst );
private int fhist_liste( int magier );
private string _description( object ob, string was, int invis, int magier );
private string _magierlevel( object ob );

// Spielerbefehle
static int anbieten( string str );
static int verachten( string str );
static int frufe( string str );
static int femote( string str );
static int fknuddel( string str );
static int fwinke( string str );
static int fopt( string str );
static int hilfe( string str );
static int fhist( string str );
static int fwer( string str );
static int fstat( string str );

/* }}} */

/* {{{ Standard-Funktionen */

void create()
{
    // die Blueprint braucht weder Reset noch Variablen
    if ( !is_clone(this_object()) ){
        set_next_reset(-1);
        return;
    }

    ::create();

    // FBand-Master schonmal aufwecken
    catch( call_other( FMASTER, "???" ) );

    seteuid( getuid() );

    SetProp( P_NAME, "Freundschaftsband" );
    // P_SHORT wird ueber short() gesetzt
    SetProp( P_GENDER, NEUTER );
    SetProp( P_WEIGHT, 0 );
    SetProp( P_NEVERDROP, 1 );
    // P_NODROP ist per _query_nodrop() immer auf 1
    SetProp( P_VALUE, 0 );
    SetProp( P_NOBUY, 1 );
    // P_KEEP_ON_SELL ist per _query_keep_on_sell() per default gesetzt
    SetProp( P_SHOW_INV, 1 );
    SetProp( P_MATERIAL, MAT_CLOTH );
    SetProp( P_INFO, "Probier mal 'hilfe freundschaftsband'.\n" );
    SetProp( P_LONG,
             break_string( "Ein kleines geflochtenes Freundschaftsband.\n"
                           "Es wird am Handgelenk getragen.\n"
                           "Mit 'hilfe freundschaftsband' bekommst Du eine "
                           "Hilfe über das Band.", 78, 0, BS_LEAVE_MY_LFS ) );

    AddDetail( ({ "handgelenk", "gelenk" }), "Es wird von einem "
                "Freundschaftsband geschmückt.\n" );

    AddId( ({ "band", "fband", "freundschaftsband", BANDID }) );

    AddCmd( ({ "biete", "biet" }), "anbieten" );
    AddCmd( ({ "verachte", "veracht" }), "verachten" );
    AddCmd( ({ "frufe", "fruf" }), "frufe" );
    AddCmd( "femote", "femote" );
    AddCmd( ({ "fknuddle", "fknuddl", "fknuddlen", "fknuddel" }), "fknuddel" );
    AddCmd( ({ "fwinke", "fwink" }), "fwinke" );
    AddCmd( "fopt", "fopt" );
    AddCmd( "hilfe", "hilfe" );
    AddCmd( "fhist", "fhist" );
    AddCmd( "fwer", "fwer" );
    AddCmd( "fstat", "fstat" );

    Freunde = ({});
    logon = time();
    // per Default sieht und zeigt man alle Freunde
    flags = ZEIGE_FREMDE | ZEIGE_EIGENE;
    angebot = ([]);
}


string short()
{
    if ( QueryProp(P_INVIS) )
        return 0;

    // wozu Platz in einer Property vergeuden? So stehts nur einmal in der BP
    return "Ein Freundschaftsband (getragen).\n";
}


varargs int move( mixed dest, int method )
{
    int res;

    // vorsichtshalber erstmal die Freunde sichern.
    // Ich bin nicht paraniod - aber man kann ja nie wissen ;^)
    if ( environment() && query_once_interactive(environment())
         && sizeof(Freunde) )
        FMASTER->update_data( getuid(environment()), Freunde );

    // ganz normal bewegen ...
    if ( (res = ::move( dest, method )) != 1 )
        return res;

    // ... und dann die Daten ueber die Freunde wieder beim Master abholen
    if ( environment() && query_once_interactive(environment()) ){
        Freunde = ({string *}) FMASTER->get_friends(getuid(environment()));

        // falls das move() **nicht** von /secure/login kam, die Daten nochmal
        // mit dem Master abgleichen, da sich in der Zeit, die der Spieler kein
        // FBand besass, einiges an seinem FBand geaendert haben kann
        // (verachte).
        // Wenn der Aufruf von /secure/login kommt, dann folgt danach sowieso
        // ein SetProp(P_AUTOLOADOBJ, ...), was den Master nochmal benach-
        // richtigt. Wuerde der Aufruf hier auch erfolgen, wuerde der Master
        // u.U. Freunde, die den Spieler verachtet haben, austragen, und der
        // SetProp-Aufruf mit den alten Daten traegt sie dann wieder ein ...
        if ( !previous_object(2) || BLUE_NAME(previous_object(2))
             != "/secure/login" )
            FMASTER->update_data( getuid(environment()), Freunde );
    }

    return res;
}


void reset()
{
    int *zeiten;

    // es gibt tatsaechlich Scherzkekse, die per Hand Funktionen wie reset()
    // in der Blueprint aufrufen ...
    if ( previous_object() || !is_clone(this_object()) )
        return;

    // die ({0}) dazu addieren, damit auf jeden Fall ein Eintrag existiert
    zeiten = sort_array( m_values(angebot)+({0}), #'>/*'*/ );
    
    // man sollte trotz Boing-Reset wenigstens fuenf Minuten Zeit haben,
    // um eine angebotene Freundschaft anzunehmen ;-)
    if ( (QueryProp(P_INVIS) && (time()-zeiten[0]) > 300) || !environment()
         || !query_once_interactive(environment()) )
        remove(1);
}


void BecomesNetAlive( object pl )
{
    object ob;

    // gib BUGS keine Chance!
    if ( !environment() || !query_once_interactive(environment()) ){
        remove(1);
        return;
    }

    // ruft irgendein Scherzkeks die Funktion per Hand auf?
    if ( BLUE_NAME(this_interactive()) != "/secure/login" ||
         previous_object() != environment() )
        return;

    logon = time();

    // die Daten ueber vorhandene Freunde mit dem FBand-Master abgleichen
    FMASTER->update_data( getuid(environment()), Freunde );

    // gibt es eine neuere Version vom Fband als dieses Objekt?
    if ( !(ob = find_object( FBNAME )) ||
         program_time(ob) > program_time(this_object()) )
        // dann einmal updaten bitte :-)
        if ( ob = neues_fband(environment()) ){
            ob->SetProp( P_AUTOLOADOBJ, ({ flags, Freunde }) );
            remove(1);
            return;
        }

    // anwesende Freunde anzeigen (bei neuem FBand macht das das neue FBand)
    zeige_anwesende();
}


void BecomesNetDead( object pl )
{
    // gib BUGS keine Chance!
    if ( !environment() || !query_once_interactive(environment()) ){
        remove(1);
        return;
    }

    // ruft irgendein Scherzkeks die Funktion per Hand auf?
    if ( this_interactive() || previous_object() != environment() )
        return;

    // Daten abgleichen und fhist aktualisieren
    if ( logon )
        FMASTER->logout( getuid(environment()), Freunde, logon );

    // Magier koennen beim Leitungszusammenbruch erst netztot und dann
    // nach einer Pause automatisch 'richtig' ausgeloggt werden.
    // Um doppelte Eintraege bei 'fhist' zu vermeiden, wird nach dem ersten
    // Abmelden beim Master die Loginzeit geloescht.
    logon = 0;
}


varargs int remove( int silent )
{
    if ( logon && environment() && query_once_interactive(environment()) )
        // noch schnell die wichtigsten Daten in Sicherheit bringen
        FMASTER->logout( getuid(environment()), Freunde, logon );

    return ::remove( silent );
}

/* }}} */

/* {{{ Query- und Set-Funktionen */

static string _query_keep_on_sell()
{
    // das FBand steht per Default auf "behalten"
    return environment() ? geteuid(environment()) : 0;
}


static int _query_nodrop()
{
    // auf speziellen Wunsch von Padreic wird P_NODROP vom FBand gegen
    // Zugriff von Wurstmagiern geschuetzt
    return 1;
}


static mixed *_query_autoloadobj()
{
    // alle wichtigen Daten werden direkt hier gespeichert
    return sizeof(Freunde) ? ({ flags, Freunde }) : 0;
}


#define PO(x) previous_object(x)

static mixed _set_autoloadobj( mixed wert )
{
    // darf nur vom FBand-Master, vom Freundschaftsband selbst oder
    // von /secure/master (renew) bzw. /secure/login gesetzt werden
    if ( (!PO(3) || BLUE_NAME(PO(3)) != "/secure/login") &&
         (!PO(1) || object_name(PO(1)) != FMASTER) &&
         (!PO(2) || BLUE_NAME(PO(2)) != FBNAME) &&
         (!PO(4) || object_name(PO(4)) != "/secure/master") )
        return -1;

    // *eigentlich* sollte man annehmen, dass nur korrekte Werte uebergeben
    // werden - aber man weiss ja nie ... ;-)
    if ( !pointerp(wert) || sizeof(wert) != 2 || (wert[0] && !intp(wert[0])) ||
         (wert[1] && !pointerp(wert[1])) )
        return -2;

    flags = wert[0];
    Freunde = wert[1];

    // neu eingeloggt? Dann anwesende Freunde anzeigen
    if ( PO(3) && BLUE_NAME(PO(3)) == "/secure/login" )
        zeige_anwesende();

    // Daten mit dem Master abgleichen, sofern der Aufruf nicht vom Master
    // kommt (Rekursion!)
    if ( environment() && query_once_interactive(environment()) &&
         (!PO(1) || object_name(PO(1)) != FMASTER) )
        FMASTER->update_data( getuid(environment()), Freunde );
        
    return wert;
}


#define EXTRATXT ({"Es ist Freund von ", "Er ist Freund von ", \
                   "Sie ist Freundin von "})

static string _query_extralook()
{
    int gender;
    object ob;
    
    // P_CHAOS_NO_FRIENDS ist fuer den Spruch "Daemonenmaske" der Chaoten.
    // Da die lieben Chaos-Magier anscheinend etwas gegen Shadows haben,
    // muessen halt die Objekte mit Extra-Look auf die Chaoten aufpassen ...
    if ( !(flags & ZEIGE_EIGENE) || QueryProp(P_INVIS) || !sizeof(Freunde)
         || !environment())
      //|| environment()->QueryProp(P_CHAOS_NO_FRIENDS) )
        return "";

    // wenn der Spieler kein Scrolling haben will, bekommt er auch keins
    if ( this_player() && (ob = present(BANDID, this_player()))
         && ({int})ob->ignoriere_scrolling() )
        return "";

    gender = ({int}) environment()->QueryProp(P_GENDER);
    
    return break_string( EXTRATXT[gender] + ({string}) environment()->
                         CountUp(map( Freunde, #'capitalize/*'*/ ))
                         + ".", 78 );
}

/* }}} */

/* {{{ Eigene Funktionen (oeffentlich) */

// gibt 1 zurueck, wenn man fremde Freunde *nicht* sehen will
int ignoriere_scrolling()
{
    return !(flags & ZEIGE_FREMDE);
}


// wird vom FBand des Spielers aufgerufen, der einem ein Freundschaftsangebot
// macht. Es wird die Zeit gespeichert; nach 5 Minuten verfaellt das Angebot
void neues_angebot( string uid )
{
    // ein paar Sicherheitsabfragen muessen einfach sein ...
    if ( !uid || !stringp(uid) || !strlen(uid) || !find_player(uid) ||
         BLUE_NAME(previous_object()) != FBNAME )
        return;

    angebot[uid] = time();
}


// diese Funktion wird aufgerufen, wenn ein Freundschaftsangebot von beiden
// Seiten vorliegt - sprich: eine neue Freundschaft zustande kommt
void freundschaft( string opfer )
{
    // keine Freundschaft per xcall ]:->
    if ( !opfer || !stringp(opfer) || !strlen(opfer) || !find_player(opfer) ||
         BLUE_NAME(previous_object()) != FBNAME ||
         !environment() || !query_once_interactive(environment()) )
        return;

    // Angebot loeschen, Freundschaft ist zustande gekommen
    efun::m_delete( angebot, opfer );

    SetProp( P_INVIS, 0 );

    if ( member( Freunde, opfer ) == -1 ){
        // neue Freundschaft
        Freunde += ({opfer});

        tell_object( environment(), "Du bist jetzt Freund"+
                     ((({int}) environment()->QueryProp(P_GENDER) == 2) ? "in"
                      : "")+" von "+capitalize(opfer)+".\n" );

    }
    else
        tell_object( environment(), "Eure Freundschaft ist nun frisch "
                     "gestärkt.\n" );
    
    // den FBand-Master auf jeden Fall mal aktualisieren
    FMASTER->update_data( getuid(environment()), Freunde );
}

/* }}} */

/* {{{ Hilfsfunktionen (intern) */

// beim Einloggen werden alle anwesenden (sichtbaren) Freunde angezeigt
private void zeige_anwesende()
{
    object *opfer, *invis;
    string *namen;

    // "invis" ist hier verwirrend - besser waere "alle". Variablen sparen ;-)
    invis = map( Freunde, #'find_player/*'*/ ) - ({0});
    opfer = filter( invis, lambda( ({'x/*'*/}),
                                         ({#'present, BANDID, 'x}) ) ) - ({0});
    // nachhalten, wer kein FBand mehr besitzt
    FMASTER->band_verloren( invis - opfer );
    invis = filter_objects( opfer, "QueryProp", P_INVIS );

    // es werden keine invis-Magier angezeigt ...
    namen = map( opfer - invis, lambda( ({'x/*'*/}),
                                              ({#'capitalize/*'*/,
                                              ({#'getuid/*'*/, 'x/*'*/}) }) ) );

    // ... es sei denn, der Besitzer des Fbandes ist selber Magier
    if ( IS_LEARNER(environment()) )
        namen += map( invis, lambda( ({'x/*'*/}),
                                           ({#'sprintf/*'*/, "(%s)",
                                           ({#'capitalize/*'*/,       
                                           ({#'getuid/*'*/, 'x/*'*/}) }) }) ) );
    
    if ( sizeof(namen) )
      tell_object( environment(),
          break_string( ({string})environment()->CountUp(namen) + " " +
            ((sizeof(namen) > 1) ? "sind" : "ist")
            + " im Spiel.", 78) );
}


// dem Opfer mal eben ein neues Band unterschieben
private object neues_fband( object opfer )
{
    object band;

    // neues Band clonen und dem Opfer unterschieben
    if ( !(band = clone_object( FBNAME )) ||
         ({int}) band->move( opfer, M_NOCHECK|M_SILENT ) != 1 ){
        // hups, da ist etwas schiefgelaufen
        if ( objectp(band) )
            destruct(band);
        
        return 0;
    }

    return band;
}


// wird von fknuddel und fwinke benoetigt. Adverben ersetzen.
private string ParseAdverben( string str )
{
    mapping adverben;
    string *adpart, zusatz;

    // welche Adverben kennt der Spieler?
    adverben = ({mapping}) this_player()->QueryStdAdverbs()
        + ({mapping}) this_player()->QueryAdverbs();

    // evtl. vorhandene Zusaetze nach einem / sichern
    if ( sizeof(adpart = efun::explode( str, "/" )) > 1 )
        zusatz = implode( adpart[1..], "/" );

    // den Teil mit den Adverben in einzelne Woerter zerlegen
    adpart = regexplode( adpart[0], "\\<[^, ]*\\>" );

    // Abkuerzungen durch die Adverben des Spielers ersetzen
    adpart = map( adpart, lambda( ({'x/*'*/}),
                                        ({#'[/*'*/, adverben, 'x/*'*/}) ) );

    // alles zusammensetzen und aufzaehlen
    return ({string}) this_player()->CountUp( adpart + ({zusatz}) - ({0, ""}) );
}


#define PRESAY(x) ({"Dein Freund ", "Dein Freund ", "Deine Freundin "})\
[({int}) (x)->QueryProp(P_GENDER)]+capitalize(getuid(x))+" "

// zugrundeliegende Funktion fuer fknuddel, fwinke und femote
private void fverb( string fremd, string selbst )
{
    object *nah, *fern;
    string tmp;

    // nah == alle Freunde, fern == invis. Variablen sparen ;-)
    fern = map( Freunde, #'find_player/*'*/ ) - ({0});
    nah = filter( fern, lambda( ({'x/*'*/}),
                                         ({#'present, BANDID, 'x}) ) ) - ({0});
    // nachhalten, wer kein FBand mehr besitzt
    FMASTER->band_verloren( fern - nah );
    fern = filter_objects( nah, "QueryProp", P_INVIS );

    // nein, keine Moeglichkeiten mehr, um auf invis-Freunde zu checken ;^)
    if ( !sizeof(nah) || (sizeof(nah) == sizeof(fern) &&
                          !IS_LEARNER(this_player())) ){
        write( "Derzeit ist aber keiner Deiner Freunde anwesend ...\n" );
        return;
    }

    // zwischenspeichern
    fern = nah;

    // Freunde im selben Raum
    nah = all_inventory(environment(this_player())) & nah;

    // der Rest
    fern -= nah;

    // Leute im selben Raum sehen kein "aus der Ferne"
    tmp = PRESAY(this_player()) +
        regreplace( regreplace( fremd, " *#", "", 1 ), " *\\^", "", 1 );

    filter_objects( nah - ({0}), "Message", tmp+"\n", MSGFLAG_SAY );


    // der Rest schon
    tmp = PRESAY(this_player()) +
        regreplace( regreplace( fremd, "#", "aus der Ferne", 1 ),
                    "\\^", "in der Ferne", 1 );

    filter_objects( fern - ({0}), "Message", tmp+"\n", MSGFLAG_REMOTE );

    // der Spieler selbst bekommt auch noch eine Meldung
    tell_object( this_player(), selbst+"\n" );

    return;
}


// die Login-Daten aller Freunde auflisten
private int fhist_liste( int magier )
{
    string *daten;
    closure QLogon;

    // Liste soll alphabetisch sortiert sein
    daten = sort_array( Freunde, #'>/*'*/ );

    // den externen Funktionsaufruf beschleunigen
    QLogon = symbol_function( "QueryLogon", FMASTER );

    // die folgende Closure sollte sich ungefaehr lesen wie:
    // sprintf( "%12s s", capitalize(x), FMASTER->QueryLogon(x,1,magier) )
    daten = map( daten, lambda( ({'x/*'*/}),
                                      ({#'sprintf/*'*/,
                                             "%12s %s",
                                             ({#'capitalize/*'*/, 'x/*'*/}),
                                             ({#'funcall/*'*/, QLogon,
                                                    'x/*'*/, 1, magier }) }) ));

    this_player()->More( implode( daten, "\n" ) );

    return 1;
}


// Hilfsfunktion fuer "fwer". Stellt die Ausgabe zeilenweise zusammen
private string _description( object ob, string was, int invis, int magier )
{
    string res, tmp;
    int i;
    object band;

    if ( invis ) // invis-Magier werden als (Name) ausgegeben
        res = sprintf( "%-14s - ", "("+capitalize(getuid(ob))+")" );
    else // bei "fwer" ohne Parameter gibts den Titel des Spielers
        if ( was == "default" )
            res = (({string}) ob->short())[0..<3] + " - ";
        else // sonst nur den Namen
            res = sprintf( "%-14s - ", capitalize(getuid(ob)) );
    
    
    switch (was){
    case "idlet wo":
    case "idelt wo": // auf Wunsch von Brandir: wer idlet wo?
        res += time2string( "%2h:%2m:%2s - ", query_idle(ob) );
        // kein break; - fallthru fuer Ort!
        
    case "ort":  // Kurztitel des Raumes
        // einruecken hinter den Spielernamen, da sonst mehrzeilige
        // Kurztitel (Morgoths Workroom) alles durcheinanderwuerfeln
      if ( !strlen( tmp = (((IS_LEARNER(environment()) || 
                  (!IS_LEARNER(ob) || objectp(band=present(BANDID, ob)) && 
                   (({int})((({mixed*})band->QueryProp(P_AUTOLOADOBJ))[0]) & VERSTECKE_MAGIER))) && 
                environment(ob) && ({string}) environment(ob)->QueryProp(P_INT_SHORT))
              || "Im Nirgendwo")) )
            tmp = " ";

        // falls das Scrolling von Morgoths Workroom nicht gewuenscht ist,
        // Kurztitel des Raumes zurechtstutzen
        if ( flags & KURZE_TITEL )
            tmp = efun::explode( tmp, "\n" )[0][0..76-strlen(res)];

        if (environment(ob))
        {
            // Wer will, kann sich auch die Regionen anzeigen lassen
            if ( flags & ZEIGE_REGIONEN )
            {
                string *fn;
                fn = explode(object_name(environment(ob)), "/");

                switch (fn[0])
                {
                    case "d":
                        tmp += sprintf(" (%s)", capitalize(fn[1]));
                        break;
                    case "gilden":
                        tmp += " (Gilden)";
                        break;
                    case "players":
                        tmp += " (Magiergebiet)";
                        break;
                    case "room":
                        break;
                    default:
                        tmp += " (Irgendwo)";
                }
            }

            // Magier bekommen auf Wunsch noch den Pfadnamen geliefert
            if ( magier && (flags & ZEIGE_PFADE) )
                tmp += " [" + object_name(environment(ob)) + "]";

        }
        res = break_string( tmp, 78, res,
                            BS_INDENT_ONCE|BS_LEAVE_MY_LFS )[0..<2];
        break;

    case "idle": // Idlezeit
    case "idel":
        res += time2string( "%2d:%2h:%2m:%2s", query_idle(ob) );
        break;

    case "weg":  // Wegmeldung
        res += (query_idle(ob) > 120) ? "idle    " : "anwesend";

        if ( tmp = ({string}) ob->QueryProp(P_AWAY) )
            res += " - Wegmeldung: " + tmp;

        break;

    case "gilde":
    case "gilden":
        res = sprintf( "%-22s ",
                       sprintf( "[%-15s %3s]",
                                capitalize( ({string}) ob->QueryProp(P_GUILD) ||
                                            ({string})
                                            ob->QueryProp(P_DEFAULT_GUILD) ||
                                            "<unbekannt>" )[0..14] + ",",
                                ((i = ({int}) ob->QueryProp(P_GUILD_LEVEL)) > 999)
                                || i < -99 ? "xxx" : (string) i ) )
            + res + sprintf( " Level %3d [%s]",
                             query_wiz_level(ob) >= LEARNER_LVL ?
                             query_wiz_level(ob) : ({int}) ob->QueryProp(P_LEVEL),
                             _magierlevel(ob) );
        break;

    default:     // Eingeloggt von
        res += (country(ob) || "") + " - ";
    }

    return res;
}


// Hilfsfunktion fuer "fwer gilde". Stellt den Magierlevel als [E] etc. dar.
private string _magierlevel( object ob )
{
    closure qp;

    qp = symbol_function( "QueryProp", ob );
    
    if ( funcall( qp, P_TESTPLAYER ) )
        return query_wiz_level(ob) ? "T" : "t";
    else if ( funcall( qp, P_SECOND ) /*&& funcall( qp, P_SECOND_MARK ) >= 0*/ )
        return query_wiz_level(ob) ? "Z" : "z";
    else
        switch( query_wiz_level(ob) ){
        case 0:
            return "s";
        case SEER_LVL..LEARNER_LVL-1:
            return "S";
        case LEARNER_LVL..WIZARD_LVL-1:
            return "L";
        case WIZARD_LVL..DOMAINMEMBER_LVL-1:
            return "m";
        case DOMAINMEMBER_LVL..SPECIAL_LVL-1:
            return "M";
        case SPECIAL_LVL..LORD_LVL-1:
            return "H";
        case LORD_LVL..ELDER_LVL-1:
            return "R";
        case ELDER_LVL..ARCH_LVL-1:
            return "W";
        case ARCH_LVL..GOD_LVL-1:
            return "E";
        default:
            return "G";
        }
}

/* }}} */

/* {{{ Spielerbefehle */

// die Funktion zum Befehl. Freundschaft anbieten.
static int anbieten( string str )
{
    string opfer, uid, *ignoriere;
    object ob, band;
    int gender;

    // zu manchen Sachen sollte man sich halt nicht zwingen lassen
    if ( !this_player() || this_player() != this_interactive() )
        return 0;
    
    // Syntax-Check
    if ( !str || !stringp(str) || !strlen(str) ||
         sscanf( str, "%s freundschaft an", opfer ) != 1 )
        return _notify_fail( "Wem möchtest Du was anbieten?\n" ), 0;

    // mit dem alten FBand konnte man ganz simpel testen, ob ein Magier
    // invis anwesend ist. Jetzt nicht mehr ;-)
    if ( !(ob = find_player(opfer)) ||
         environment(ob) != environment(this_player()) ||
         (({int})ob->QueryProp(P_INVIS) && !IS_LEARNER(this_player())) )
        return _notify_fail( "Aber hier steht doch gar kein Spieler namens '"
                             + capitalize(opfer)+"'!\n" ), 0;

    // Gaeste brauchen keine Freunde
    if ( sscanf( opfer, "gast%d", gender ) == 1 )
        return _notify_fail( "Die Freundschaft würde nicht "
                             "lange halten.\n" ), 0;

    gender = ({int}) this_player()->QueryProp(P_GENDER);
    
    // jeder ist sich selbst der beste Freund?
    if ( ob == this_player() )
        return _notify_fail( "Nein, lieber nicht. "+
                             ({"Es", "Er", "Sie"})[gender]
                             +" kommt immer auf so komische Ideen.\n" ), 0;

    ignoriere = ({string *}) ob->QueryProp(P_IGNORE) || ({});

    uid = getuid(this_player());
    
    // wer nicht will, der hat schon
    if ( pointerp(ignoriere) &&
         sizeof( ignoriere & ({"freundschaft", uid, "freundschaft."+uid}) ) )
        return _notify_fail( capitalize(opfer)
                             +" mag Deine Freundschaft nicht.\n" ), 0;

    // noch kein oder nur ein altes Freundschaftsangebot da
    if ( !member( angebot, opfer ) || (time() - angebot[opfer]) > 300 ){
        tell_object( ob, capitalize(uid) + " bietet Dir "
                     +({"seine", "seine", "ihre"})[gender]
                     +" Freundschaft an.\n" );

        tell_object( this_player(), "Du bietest "+capitalize(opfer)
                     +" Deine Freundschaft an.\n" );

        // das Opfer hat (noch) kein Freundschaftsband
        if ( !(band = present( BANDID, ob )) ){
            // neues Band clonen und dem Opfer unterschieben
            if ( !(band = neues_fband(ob)) ){
                // da buggt irgend etwas ...
                tell_object( ob, "Da ist leider etwas schiefgegangen. Bitte "
                             "Magier verständigen!\n" );
                
                write( "Da ist leider etwas schiefgegangen. Bitte "
                       "Magier verständigen!\n" );

                return 1;
            }

            band->SetProp( P_INVIS, 1 );

            tell_object( ob, break_string( capitalize(uid)+" gibt Dir ein "
                                           "Freundschaftsband.\nDu kannst die "
                                           "Freundschaft mit 'biete "+uid+
                                           " freundschaft an' beantworten.", 78,
                                           0, BS_LEAVE_MY_LFS ) );
        }

        band->neues_angebot(uid);
    }
    // Bingo, wir haben einen neuen Freund/eine neue Freundin
    else{
        // falls das Opfer kein Freundschaftsband mehr hat - warum auch immer
        if ( !(band = present( BANDID, ob )) )
            // neues Band clonen und dem Opfer unterschieben
            if ( !(band = neues_fband(ob)) ){
                // da buggt irgend etwas ...
                tell_object( ob, "Da ist leider etwas schiefgegangen. Bitte "
                             "Magier verständigen!\n" );
                
                write( "Da ist leider etwas schiefgegangen. Bitte "
                       "Magier verständigen!\n" );
                
                return 1;
            }

        band->freundschaft(uid);
        this_object()->freundschaft(opfer);
    }
    
    return 1;
}


// die Funktion zum Befehl - Verachten.
static int verachten( string str )
{
    // die ueblichen Sicherheitsabfragen ...
    if ( !this_player() || this_player() != this_interactive()
         || QueryProp(P_INVIS) )
        return 0;
    
    // Syntax-Check
    if ( !str || !stringp(str) || !strlen(str) || (member( str, ' ' ) != -1) )
        return _notify_fail( "Wen möchtest Du denn verachten?\n" ), 0;

    // Mudder sind schizophren
    if ( str == getuid(this_player()) )
        return _notify_fail( "Gib "+({"ihm", "ihm", "ihr"})
                             [({int}) this_player()->QueryProp(P_GENDER)]
                             +" doch bitte noch eine Chance!\n" ), 0;

    // normalerweise sollte der Test auf die eigenen Freunde vollkommen
    // ausreichen. Allerdings koennte es einseitige Freundschaften als
    // "Altlasten" vom alten FBand geben; deshalb der doppelte Test
    if ( (member( Freunde, str ) == -1) &&
         (member( ({string *}) FMASTER->get_friends(str),
                  getuid(this_player()) ) == -1 ) )
        return _notify_fail( "Zwischen "+capitalize(str)+" und Dir besteht "
                             "doch gar keine Freundschaft.\n" ), 0;

    Freunde -= ({str});

    // die Benachrichtigung des Opfers uebernimmt der FBand-Master
    FMASTER->hab_verachtet( getuid(this_player()), str, Freunde );

    write( "Okay, keine Freundschaft mehr zwischen Dir und "+capitalize(str)
           +".\n" );

    return 1;
}


// frufe jetzt mit break_string je nach zeilenlaenge
int send_ftell(object friend, string text, string pre)
{
  friend->Message( break_string(text, 78, pre), 
      MSGFLAG_CHANNEL );
  return 1;
}


// die Funktion zum ... ach, siehe oben ;-)
static int frufe( string str )
{
    object *opfer, *invis;
    string pre, text;
    
    // ein paar kleine Sicherheitsabfragen ...
    if ( !this_player() || this_player() != this_interactive()
         || QueryProp(P_INVIS) )
        return 0;

    // Syntax-Check
    if ( !str || !stringp(str) || !strlen(str) )
        return _notify_fail( "Was möchtest Du denn Deinen Freunden "
                             "zurufen?\n" ), 0;

    // Variablennamen sparen ... "invis" ist doppelt benutzt
    invis = map( Freunde, #'find_player/*'*/ ) - ({0});
    opfer = filter( invis, lambda( ({'x/*'*/}),
                                         ({#'present, BANDID, 'x}) ) ) - ({0});
    // nachhalten, wer kein FBand mehr besitzt
    FMASTER->band_verloren( invis - opfer );
    invis = filter_objects( opfer, "QueryProp", P_INVIS );

    // nein, keine Moeglichkeiten mehr, um auf invis-Freunde zu checken ;^)
    if ( !sizeof(opfer) || (sizeof(opfer) == sizeof(invis) &&
                            !IS_LEARNER(this_player())) ){
        write( "Derzeit ist aber keiner Deiner Freunde anwesend ...\n" );
        return 1;
    }

    pre = sprintf( "Dein%s %s teilt Euch mit: ",
                   ((({int}) this_player()->Query(P_GENDER) == 2)
                    ? "e Freundin" : " Freund"),
                   capitalize(getuid(this_player())) );

//    text = break_string( this_player()->_unparsed_args(), 78, pre );

    // mit dem korrigierten Typo (_D_einen) bring ich sicherlich
    // gaaaaanz viele Trigger durcheinander
    write( break_string( this_player()->_unparsed_args(), 
          78,
          "Du teilst Deinen Freunden mit: " ) );

    // P_EARMUFFS beachte ich absichtlich nicht hier. Das ist ein Bug, der in
    // der Mudlib ausgebaut gehoert - bei MSGFLAG_TELL funktionierts ja auch.
    // Ergaenzung: der Bug ist mittlerweile ausgebaut in der Mudlib ;-)
//    filter_objects( opfer, "Message", text, MSGFLAG_CHANNEL );
    filter( opfer, #'send_ftell, ({int})this_player()->_unparsed_args(), pre);

    return 1;
}

// der Befehl femote
static int femote( string str )
{
    string fremd, selbst;
    
    // environment(this_player()) muss fuer fverb() da sein,
    // ausserdem gibts femote erst als Seher
    if ( !this_player() || this_player() != this_interactive() 
         || !environment(this_player()) || !IS_SEER(this_player())
         || QueryProp(P_INVIS) )
        return 0;

    // Syntax-Check
    if ( !str || !stringp(str) || !strlen(str) )
        return _notify_fail( "Was möchtest Du denn Deinen Freunden "
                             "emoten?\n" ), 0;

    fremd = ({string}) this_player()->_unparsed_args();
    
    // Platzhalter '#' fuer "aus der Ferne" oder '^' fuer "in der Ferne"
    // muss vorhanden sein
    if ( member( str, '#' ) == -1 && member( str, '^' ) == -1 )
        fremd += " #.";

    // Falls das femote nicht mit einem Satzzeichen abgeschlossen ist, wird
    // ein Punkt angefuegt
    if ( member(  ".?!", fremd[<1] ) == -1 )
        fremd += ".";

    // fuer die Meldung an den Spieler selbst muss die Meldung ersetzt werden
    selbst = capitalize(getuid(this_player())) + " " +
        regreplace( regreplace( fremd, "#", "aus der Ferne", 1 ),
                    "\\^", "in der Ferne", 1 );

    fverb( fremd, selbst );
    
    return 1;
}


// der freundliche Knuddler zwischendurch
static int fknuddel( string str )
{
    string adverben, fremd, selbst;
    
    // environment(this_player()) muss fuer fverb() da sein
    if ( !this_player() || this_player() != this_interactive()
         || !environment(this_player()) || QueryProp(P_INVIS) )
        return 0;

    adverben = ParseAdverben(({string}) this_player()->_unparsed_args() || "");

    // Adverben einbauen
    if ( adverben )
        fremd = "knuddelt Euch " + adverben + " #.";
    else
        fremd = "knuddelt Euch #.";

    // Text fuer den Spieler selbst basteln
    if ( adverben )
        selbst = "Du knuddelst Deine Freunde " + adverben + " aus der Ferne.";
    else
        selbst = "Du knuddelst Deine Freunde aus der Ferne.";

    fverb( fremd, selbst );

    return 1;
}


// die freundschaftliche Verabschiedung. Siehe fknuddel
static int fwinke( string str)
{
    string adverben, fremd, selbst;
    
    // environment(this_player()) muss fuer fverb() da sein
    if ( !this_player() || this_player() != this_interactive()
         || !environment(this_player()) || QueryProp(P_INVIS) )
        return 0;

    adverben = ParseAdverben(({string}) this_player()->_unparsed_args() || "");

    // Adverben einbauen
    if ( adverben )
        fremd = "winkt Euch " + adverben + " # zu.";
    else
        fremd = "winkt Euch # zu.";

    // Text fuer den Spieler selbst basteln
    if ( adverben )
        selbst = "Du winkst Deinen Freunden " + adverben + " aus der Ferne zu.";
    else
        selbst = "Du winkst Deinen Freunden aus der Ferne zu.";
    
    fverb( fremd, selbst );

    return 1;
}


#define FOPTTXT1 ({ "Du interessierst Dich nicht für fremde "+ \
    "Freundschaftsbänder.\n", \
    "Du schaust Dir gerne fremde Freundschaftsbänder an.\n" })

#define FOPTTXT2 ({ "Du behälst lieber für Dich, wer zu Deinen Freunden "+ \
    "gehört.\n", \
    "Du möchtest allen zeigen, wie groß Dein Freundeskreis ist.\n" })

#define FOPTTXT3 ({ "Dir ist eine saubere Formatierung wichtiger als "+ \
    "irgendwelche Kleinigkeiten.\n", \
    "Du willst immer alles wissen - auch wenn es dabei "+ \
    "schonmal scrollt.\n" })

#define FOPTTXT4 ({ "Dich interessieren keine Pfadnamen bei 'fwer ort'.\n", \
    "Du möchtest stets genau wissen, wo sich Deine Freunde aufhalten.\n" })

#define FOPTTXT5 ({ "Dich interessieren keine Regionen bei 'fwer ort'.\n", \
    "Du möchtest immer auch wissen, in welcher Region Deine Freunde sind.\n" })

#define FOPTTXT6 ({ "Spieler sehen Deinen Aufenthaltsort bei 'fwer ort'.\n", \
    "Spieler sehen Deinen Aufenthaltsort nicht.\n" })

// mit diesem Befehl lassen sich die Optionen des Fbandes einstellen
static int fopt( string str )
{
    // die obligatorischen Sicherheitsabfragen ...
    if ( !this_player() || this_player() != this_interactive()
         || QueryProp(P_INVIS) )
        return 0;
    
    // das FBand laesst sich auch etwas konfigurieren
    // da die Syntax etwas dumm ist, gibt es eine ausfuehrliche Beschreibung
/*  
    _notify_fail( break_string( "Du kannst das Verhalten Deines "
                                "Freundschaftsbandes Deinen Vorlieben anpassen.\n"
                                "Momentan koennte man die Einstellungen so "
                                "beschreiben:\n\n"+
                                FOPTTXT1[(flags & ZEIGE_FREMDE) > 0]+
                                FOPTTXT2[(flags & ZEIGE_EIGENE) > 0]+
                                FOPTTXT3[(flags & KURZE_TITEL) > 0]+
                                (IS_LEARNER(this_player()) ?
                                 FOPTTXT4[(flags & ZEIGE_PFADE) > 0] : "")+
                                FOPTTXT5[(flags & ZEIGE_REGIONEN) > 0]+
                                (IS_LEARNER(this_player()) ?
                                FOPTTXT6[(flags & VERSTECKE_MAGIER ) > 0] : "")+
                                "\nFalls Du der Meinung bist, dass Du dadurch "
                                "eher schlecht beschrieben wirst, probier doch "
                                "einmal folgende Befehle aus:\n\n"
                                "    - fopt fremde freunde zeigen\n"
                                "    - fopt fremde freunde nicht zeigen\n\n"
                                "    - fopt eigene freunde zeigen\n"
                                "    - fopt eigene freunde nicht zeigen\n\n"
                                "    - fopt ungekuerzte beschreibungen\n"
                                "    - fopt gekuerzte beschreibungen\n\n"+
                                (IS_LEARNER(this_player()) ?
                                 "    - fopt pfade zeigen\n"
                                 "    - fopt pfade nicht zeigen\n\n" : "")+
                                "    - fopt regionen zeigen\n"
                                "    - fopt regionen nicht zeigen\n\n"+
                                (IS_LEARNER(this_player()) ?
                                "    - fopt ort verstecken\n"
                                "    - fopt ort nicht verstecken\n\n" : "")
                                ,78, 0, BS_LEAVE_MY_LFS ) );
*/
    _notify_fail( break_string( "Du kannst das Verhalten Deines "
          "Freundschaftsbandes Deinen Vorlieben anpassen.\n"
          "Ein * zeigt die gerade aktive Einstellung.\n\n"
          "    "+(flags & ZEIGE_FREMDE ? "*":"-")+" fopt fremde freunde zeigen\n"
          "    "+(flags & ZEIGE_FREMDE ? "-":"*")+" fopt fremde freunde nicht zeigen\n\n"
          "    "+(flags & ZEIGE_EIGENE ? "*":"-")+" fopt eigene freunde zeigen\n"
          "    "+(flags & ZEIGE_EIGENE ? "-":"*")+" fopt eigene freunde nicht zeigen\n\n"
          "    "+(flags & KURZE_TITEL ? "*":"-")+" fopt ungekürzte beschreibungen\n"
          "    "+(flags & KURZE_TITEL ? "-":"*")+" fopt gekürzte beschreibungen\n\n"+
          (IS_LEARNER(this_player()) ?
           "    "+(flags & ZEIGE_PFADE ? "*":"-")+" fopt pfade zeigen\n"
           "    "+(flags & ZEIGE_PFADE ? "-":"*")+" fopt pfade nicht zeigen\n\n" : "")+
          "    "+(flags & ZEIGE_REGIONEN ? "*":"-")+" fopt regionen zeigen\n"
          "    "+(flags & ZEIGE_REGIONEN ? "-":"*")+" fopt regionen nicht zeigen\n\n"+
          (IS_LEARNER(this_player()) ?
           "    "+(flags & VERSTECKE_MAGIER ? "*":"-")+" fopt ort verstecken\n"
           "    "+(flags & VERSTECKE_MAGIER ? "-":"*")+" fopt ort nicht verstecken\n\n" : "")
          ,78, 0, BS_LEAVE_MY_LFS ) );

    // Optionen checken
    switch (str) {
    case "fremde freunde zeigen":
        flags |= ZEIGE_FREMDE;
        write( FOPTTXT1[(flags & ZEIGE_FREMDE) > 0] );
        return 1;

    case "fremde freunde nicht zeigen":
        flags &= ~ZEIGE_FREMDE;
        write( FOPTTXT1[(flags & ZEIGE_FREMDE) > 0] );
        return 1;

    case "eigene freunde zeigen":
        flags |= ZEIGE_EIGENE;
        write( FOPTTXT2[(flags & ZEIGE_EIGENE) > 0] );
        return 1;

    case "gekürzte beschreibungen":
    case "gekuerzte beschreibungen":
        flags &= ~KURZE_TITEL;
        write( FOPTTXT3[(flags & KURZE_TITEL) > 0] );
        return 1;

    case "ungekürzte beschreibungen":
    case "ungekuerzte beschreibungen":
        flags |= KURZE_TITEL;
        write( FOPTTXT3[(flags & KURZE_TITEL) > 0] );
        return 1;

    case "eigene freunde nicht zeigen":
        flags &= ~ZEIGE_EIGENE;
        write( FOPTTXT2[(flags & ZEIGE_EIGENE) > 0] );
        return 1;

    case "pfade zeigen":
        if ( IS_LEARNER(this_player()) ){
            flags |= ZEIGE_PFADE;
            write( FOPTTXT4[(flags & ZEIGE_PFADE) > 0] );
            return 1;
        }
        return 0;

    case "pfade nicht zeigen":
        if ( IS_LEARNER(this_player()) ){
            flags &= ~ZEIGE_PFADE;
            write( FOPTTXT4[(flags & ZEIGE_PFADE) > 0] );
            return 1;
        }
        return 0;

    case "regionen zeigen":
        flags |= ZEIGE_REGIONEN;
        write( FOPTTXT5[(flags & ZEIGE_REGIONEN) > 0] );
        return 1;

    case "regionen nicht zeigen":
        flags &= ~ZEIGE_REGIONEN;
        write( FOPTTXT5[(flags & ZEIGE_REGIONEN) > 0] );
        return 1;

    case "ort verstecken":
        if ( IS_LEARNER(this_player()) ){
            flags |= VERSTECKE_MAGIER;
            write( FOPTTXT6[(flags & VERSTECKE_MAGIER) > 0] );
            return 1;
        }
        return 0;

    case "ort nicht verstecken":
        if ( IS_LEARNER(this_player()) ){
            flags &= ~VERSTECKE_MAGIER;
            write( FOPTTXT6[(flags & VERSTECKE_MAGIER) > 0] );
            return 1;
        }
        return 0;

    default:
        return 0;
    }
}


// diese Funktion reagiert nur auf das Schluesselwort "freundschaftsband".
// Alle anderen Schluesselwoerter werden an die allgemeine Hilfe weitergereicht.
static int hilfe( string str )
{
    string help;

    // der einzige Befehl, zu dem man einen Spieler auch zwingen darf ]:->
    if ( (str != "freundschaftsband" && str != "fband") || !this_player() )
        return 0;

    help = sprintf( "%'-'78s\n", "" );
    
    help += read_file(HELPFILE) || "Die Hilfeseite ist beschädigt.\n"
        "Bitte verständige einen Magier.\n";
    
    // Seher bekommen den Befehl 'femote' zusaetzlich
    if ( IS_SEER(this_player()) )
        help += read_file(SEERHELP) || "\n\nDie Seherhilfe ist beschädigt.\n"
            "Bitte verständige einen Magier.\n";

    // Magier haben eine Option mehr
    if ( IS_WIZARD(this_player()) )
        help += read_file(WIZARDHELP) || "\n\nDie Magierhilfe ist "
            "beschädigt.\nBitte benachrichtige einen Magier.\n";

    help += sprintf( "%'-'78s\n", "" );

    this_player()->More( help );
    
    return 1;
}


// der fhist-Befehl
static int fhist( string str )
{
    string *daten;
    int lvl, magier;
    
    // ein paar kleine Sicherheitsabfragen ...
    if ( !this_player() || this_player() != this_interactive()
         || QueryProp(P_INVIS) )
        return 0;

    // ohne Freunde ist der Befehl etwas sinnlos
    if ( !sizeof(Freunde) )
        return _notify_fail( "Du hast doch noch gar keine Freunde!\n" ), 0;

    magier = IS_LEARNER(this_player());
    
    // ohne Parameter gibts die ganze Liste
    if ( !str || (stringp(str) && !strlen(str)) )
        return fhist_liste( magier );

    // der Parameter sollte schon ein Freund oder man selbst sein
    if ( member( Freunde, str ) == -1 && str != getuid(this_player()) )
        return _notify_fail( "Du kannst nur Daten über Deine Freunde "
                             "abfragen!\n" ), 0;

    if ( magier )
        // Informationen ueber die letzten 20 Male sind genug
        lvl = 20;
    else
        // ab Spielerlevel 85 gibt's genauso viele Informationen wie bei Magiern
        lvl = (({int}) this_player()->QueryProp(P_LEVEL) - 5) / 4;

    // mindestens drei Zeilen gibt's auf jeden Fall
    // mehr als 20 rueckt der Master sowieso nicht heraus 
    if ( lvl < 3 )
        lvl = 3;
    
    if ( !sizeof(daten = ({string *}) FMASTER->QueryLogon( str, lvl, magier )) )
        daten = ({"Über "+capitalize(str)+" habe ich keine Informationen!\n"});

    // bei maximal 21 Zeilen sollte doch lieber More her ...
    this_player()->More( capitalize(str) + ":\n" + implode( daten, "\n" ) );

    return 1;
}


// die Funktion zum Befehl "fwer"
static int fwer( string str )
{
    object *opfer, *invis;
    string *daten;
    int was;
    
    // die obligatorischen Sicherheitsabfragen ...
    if ( !this_player() || this_player() != this_interactive()
         || QueryProp(P_INVIS) )
        return 0;

    // opfer == sichtbare Freunde, invis == unsichtbare Freunde
    invis = map( Freunde, #'find_player/*'*/ ) - ({0});
    opfer = filter( invis, lambda( ({'x/*'*/}),
                                         ({#'present, BANDID, 'x}) ) ) - ({0});
    // nachhalten, wer kein FBand mehr besitzt
    FMASTER->band_verloren( invis - opfer );
    invis = filter_objects( opfer, "QueryProp", P_INVIS );
    opfer -= invis;

    // auch hier: invis-Magier sind geschuetzt vor neugierigen Spielern ;-)
    if ( !sizeof(opfer) && (!sizeof(invis) || !IS_LEARNER(this_player())) ){
        write( "Wir sind alle Deine Freunde.\n" );
        return 1;
    }

    if ( !str || member( ({ "ort", "idle", "idel", "weg", "gilde", "gilden",
                                "idlet wo", "idelt wo" }), str ) == -1 )
        str = "default";

    // einmal fuer die sichtbaren Freunde ...
    daten = map( opfer, #'_description/*'*/, str, 0,
                       IS_LEARNER(this_player()) );

    // ... und danach nochmal fuer die (Magier)
    if ( IS_LEARNER(this_player()) )
        daten += map( invis, #'_description/*'*/, str, 1, 1 );

    // die Ausgabe ist sortiert
    this_player()->More( implode( sort_array( daten, #'>/*'*/ ), "\n" ) );

    return 1;
}


#define AUFZAEHLEN(x) this_player()->CountUp( map( sort_array( \
(x), #'>/*'*/ ), #'capitalize/*'*/ ) )

// der neu eingefuehrte Befehl fstat
static int fstat( string str )
{
    string res, *tmp, uid;
    
    // die obligatorischen Sicherheitsabfragen ...
    if ( !this_player() || this_player() != this_interactive()
         || QueryProp(P_INVIS) )
        return 0;

    // ohne Freunde keine Statistik
    if ( !sizeof(Freunde) )
        return _notify_fail( "Dein Freundschaftsband ist noch absolut "
                             "leer.\n" ), 0;

    // erstmal alle Freunde anzeigen
    res = "Du bist Freund"+
        ((({int}) this_player()->QueryProp(P_GENDER) == 2) ? "in" : "" )+" von "+
        ({int})AUFZAEHLEN(Freunde)+".\n";

    uid = getuid(this_player());
    
    // dann jeweils - falls vorhanden - inaktive Freunde ...
    if ( sizeof(tmp = ({string *}) FMASTER->get_friends( uid, INAKTIV) -
         ({string *}) FMASTER->get_friends( uid, BAND_WEG|SELBSTGELOESCHT)) )
        res += "\nVon "+({int})AUFZAEHLEN(tmp)+" hast Du leider seit einiger Zeit "
            "nichts mehr gehört.\n";

    // ... Freunde, die ihr FBand verbummelt haben ...
    if ( sizeof(tmp = ({string *}) FMASTER->get_friends( uid, BAND_WEG)
         - ({string *}) FMASTER->get_friends( uid, SELBSTGELOESCHT)) )
        res += "\n"+({int})AUFZAEHLEN(tmp)+" besitz"+((sizeof(tmp) > 1) ? "en" : "t")+
            " zur Zeit anscheinend kein Freundschaftsband.\n";

    // ... und Freunde, die es gar nicht mehr gibt
    if ( sizeof(tmp = ({string *}) FMASTER->get_friends( uid, SELBSTGELOESCHT)) )
        res += "\n"+({int})AUFZAEHLEN(tmp)+" ha"+((sizeof(tmp) > 1) ? "ben" : "t")+
            " sich anscheinend dazu entschlossen, hier nicht mehr zu spielen."+
            "\n";

    // vorsichtshalber per More ;-)
    this_player()->More( break_string( res, 78, 0, BS_LEAVE_MY_LFS ) );

    return 1;
}

/* }}} */
