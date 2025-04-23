// fbmaster.c  --  Der Master fuer das neue Freundschaftsband
//
// Aenderungen an diesem File bitte erst sorgfaeltig im Homemud austesten.
// Wenn dieser Master buggt, buggt jedes Freundschaftsband mit ihm und es
// koennen Freunde verloren gehen.
//
// (C) Tiamak@MorgenGrauen
//
// Letzte Aenderung: 30.06.1999 

/* {{{ Includes & Co */


#define NEED_PROTOTYPES

#include <defines.h>
#include <properties.h>

#define ROOTDIR(x)         ("/p/service/tiamak/"+x)
#define OBJDIR(x)          (ROOTDIR("obj/"+x))
#define SAVDIR(x)          (ROOTDIR("save/"+x))
#define LOGDIR(x)          (ROOTDIR("log/"+x))

#define FBNAME             (OBJDIR("fband2"))
#define SAVEFILE           (SAVDIR("fbmaster"))
#define LOGFILE            (LOGDIR("fbstatistik.log"))

#define BANDID             "\n\bfband"

#define ZEIGE_FREMDE       1 
#define ZEIGE_EIGENE       2

#define SELBSTGELOESCHT    1
#define INAKTIV            2
#define BAND_WEG           4
#define NUR_AKTIVE         8
#define GEPRUEFT          16

#define P_FRIENDS          "friends"
#define P_FOPT_FRIENDS     "fopt_friends"

private mapping logins;
private mapping friends;

/* }}} */

/* {{{ Prototypes */

// Standard-Funktionen
void create();
void reset();
varargs int remove( int silent );

// eigene Funktionen (oeffentlich)
void update_data( string uid, string *Freunde );
void logout( string uid, string *Freunde, int logon );
mixed QueryLogon( string uid, int anz, int magier );
varargs string *get_friends( string uid, int flag );
void hab_verachtet( string uid, string opfer, string *Freunde );
void band_verloren( object *opfer );

// eigene Funktionen (intern)
static void aufraeumen( int step, int tmp );
private int name_check( string name );
private string format_dates( int *times );
private int filter_friends( string name, int flag );
private void kill_player( string name );

/* }}} */

/* {{{ Funktionen */

/* {{{ Standard-Funktionen */

void create()
{
    // ein Master ist genug
    if ( is_clone(this_object()) ){
        destruct(this_object());
        return;
    }

    // wir wollen Savefiles schreiben duerfen
    seteuid(getuid());

    // alle 15 Minuten Reset, damit das Savefile geschrieben wird
    set_next_reset(900);

    logins = ([]);
    friends = ([]);

    // Daten wiederherstellen
    restore_object(SAVEFILE);

    // das erste Mal eine Stunde nach Laden des Masters aufraeumen
    call_out( "aufraeumen", 3600, 0, 0 );
}


void reset()
{
    // keinen Aufruf per Hand bitte
    if ( extern_call() && previous_object() )
        return;
    
    // der naechste Reset in 15 Minuten
    set_next_reset(900);

    save_object(SAVEFILE);
}


varargs int remove( int silent )
{
    save_object(SAVEFILE);
    destruct(this_object());
    return 1;
}

/* }}} */

/* {{{ eigene Funktionen (oeffentlich) */

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

// Funktion zum Aufraeumen der gespeicherten Daten. Wird einmal am Tag
// gestartet. Der erste Parameter gibt den Schritt an, bei dem die Aufraeum-
// arbeiten momentan sind; der zweite Parameter wird zur Zwischenspeicherung
// der Fortschrittsinformationen benoetigt. Es werden immer soviele Operationen
// pro Schritt gemacht, bis entweder der Schritt fertig ist oder nur noch
// 200k Eval-Ticks uebrig sind. Dann wird wieder 10s gewartet.
static void aufraeumen( int step, int tmp )
{
    int size, grenze, i;
    string *names;
    mixed *info, buffer;
    closure func;

    switch (step){
    case 0:
        // einmal am Tag aufraeumen. Aufruf schon im ersten Schritt - falls
        // spaeter etwas buggen sollte, wird die Kette wenigstens nicht komplett
        // abgebrochen
        call_out( "aufraeumen", 86400, 0, 0);
        step++;
        tmp = 0;

        // Logfiles per log_file() gehen leider stur nach /log. Da will ich
        // nicht hin - also muss ich per Hand die Groesse ueberwachen ...
        if ( file_size(LOGFILE) > 50000 ){
            rm( LOGFILE+".old" );
            rename( LOGFILE, LOGFILE+".old" );
        }
        
        // Logfile-Eintrag schreiben
        write_file( LOGFILE, dtime(time())+":  ************* Aufräumen "
                    "gestartet *************\n" );
        write_file( LOGFILE,
                    sprintf( "\nEinträge gesamt: %4d. Davon inaktiv: %4d, "
                             "ohne Band: %4d, gelöscht: %4d.\n\n",
                             sizeof(friends),
                             sizeof(filter( m_indices(friends),
                                     #'filter_friends/*'*/, INAKTIV ) - ({0})),
                             sizeof(filter( m_indices(friends),
                                     #'filter_friends/*'*/, BAND_WEG ) - ({0})),
                             sizeof(filter( m_indices(friends),
                                     #'filter_friends/*'*/, SELBSTGELOESCHT )
                             - ({0})) ) );
        break;

    case 1:
        // die Daten ueber Freundschaften muessen vollstaendig sein. Wenn
        // Spieler A Freund von Spieler B ist, muss auch Spieler B Freund
        // von Spieler A sein. Bei neuen Freundschaften ist das automatisch
        // der Fall - aber bei "geerbten" Freundschaften vom alten FBand
        // passt das oftmals nicht. Vor allem nicht mit geloeschten Spielern.
        // Also raeumen wir taeglich auf und basteln die fehlenden Freund-
        // schaftsdaten zurecht.
        size = sizeof(names = sort_array( m_indices(friends), #'>/*'*/ ));
        
        while ( get_eval_cost() > 200000 && tmp < size ){
            // alle Freunde jeden Spielers durchgehen
            for ( i = sizeof(friends[ names[tmp] ][1]); i--; )
                // sofern legaler Name (kein gespeichertes Datum)
                if ( name_check(friends[ names[tmp] ][1][i]) ){
                    // wenn noch gar keine Daten vorhanden, neu anlegen
                    if ( !sizeof(friends[ friends[ names[tmp] ][1][i] ]) )
                        friends[ friends[ names[tmp] ][1][i] ] =
                            ({ 0, ({names[tmp]}), ({}) });
                    else // falls Daten vorhanden, aber Freund nicht eingetragen
                        if ( member( friends[ friends[ names[tmp] ][1][i] ][1],
                                     names[tmp] ) == -1 )
                            // Freund nachtragen
                            friends[ friends[ names[tmp] ][1][i] ][1] +=
                                ({names[tmp]});
                }
            
            tmp++;
        }
        
        if ( tmp >= size ){
            // wir haben fertig
            step++;
            tmp = 0;
        }
        break;
        
    case 2:
        // Login-Daten aufraeumen. Alle Daten, die aelter sind als zwei
        // Monate, werden geloescht. Wir sind hier schliesslich nicht
        // bei der Stasi oder so ;-)
        size = sizeof(names = sort_array( m_indices(logins), #'>/*'*/ ));

        // Grenze setzen, ab wann Daten "alt" sind
        grenze = time() - 5184000;
        
        // aussortiert wird alles, was logins[uid][i][0] <= time()-2 Monate
        // allerdings wird immer wenigstens ein Datum behalten als
        // Anhaltspunkt, wie lange ein Freund/eine Freundin nicht mehr da war
        while ( get_eval_cost() > 200000 && tmp < size ){
            // letztes Einlogg-Datum sichern
            if ( sizeof(logins[names[tmp]]) )
                buffer = logins[names[tmp]][<1];
            else
                buffer = 0;

            // veraltete Daten loeschen
            logins[names[tmp]] = filter( logins[names[tmp]],
                                               lambda(({'x/*'*/}),
                                                      ({#'>/*'*/,
                                                             ({#'[/*'*/,
                                                                    'x/*'*/,
                                                                    0}),
                                                             grenze})) )-({0});

            // falls alle Daten geloescht, das letzte Einlogg-Datum behalten
            if ( buffer && !sizeof(logins[names[tmp]]) )
                logins[names[tmp]] = ({buffer});
            
            tmp++;
        }
        
        if ( tmp >= size ){
            // alle Login-Daten abgearbeitet
            step++;
            tmp = 0;
        }
        break;

    case 3:
        // alle (selbst-)geloeschten Spieler als solche markieren. Gleichzeitig
        // das Datum mitspeichern, damit spaeter restaurierte Spieler von neuen
        // Spielern desselben Namens unterschieden werden koennen
        size = sizeof(names = sort_array( m_indices(friends), #'>/*'*/ ));
        func = symbol_function( "get_userinfo", "/secure/master" );

        while ( get_eval_cost() > 200000 && tmp < size ){
            // schon als geloescht markierte Spieler nicht nochmal markieren
            if ( !(friends[names[tmp]][0] & SELBSTGELOESCHT) )
                if ( !funcall( func, names[tmp] ) ){
                    // als geloescht markieren
                    friends[names[tmp]][0] |= SELBSTGELOESCHT;
                    // und Zeit merken
                    friends[names[tmp]][1] += ({ to_string(time()) });
                }

            tmp++;
        }

        if ( tmp >= size ){
            // alle geloeschten Spieler markiert
            step++;
            tmp = 0;
        }
        break;

    case 4:
        // alle Spieler, die laenger als einen Monat nicht mehr im MG waren,
        // als inaktiv markieren. Diese Spieler werden bei "mail freunde"
        // auch nicht als Empfaenger beruecksichtigt
        size = sizeof(names = sort_array( m_indices(friends), #'>/*'*/ ));

        while ( get_eval_cost() > 200000 && tmp < size ){
            if ( !sizeof(logins[names[tmp]]) ||
                 logins[names[tmp]][<1][0] < (time()-2592000) )
                friends[names[tmp]][0] |= INAKTIV;

            tmp++;
        }

        if ( tmp >= size ){
            // alle inaktiven Spieler als solche markiert
            step++;
            tmp = 0;
        }
        break;

    case 5:
        // alle als geloescht markierten Spieler durchgehen und pruefen, ob sie
        // noch immer geloescht sind. Falls es sich um restaurierte Spieler
        // handelt, einfach das SELBSTGELOESCHT-Flag loeschen. Falls der Name
        // aber neu vergeben wurde, alle Eintraege ueber den alten Spieler
        // loeschen.
        size = sizeof(names = sort_array( m_indices(friends), #'>/*'*/ ));
        func = symbol_function( "get_userinfo", "/secure/master" );

        while ( get_eval_cost() > 200000 && tmp < size ){
            // ist der Spieler als geloescht markiert aber existiert?
            if ( (friends[names[tmp]][0] & SELBSTGELOESCHT)  &&
                 sizeof(info = funcall( func, names[tmp] )) ){
                // ist creation_date groesser als Selbstloeschungs-Datum?
                if ( to_int(sort_array( friends[names[tmp]][1], #'>/*'*/ )[0])
                     < info[5] )
                    // Name wurde neu vergeben -> Spielerdaten loeschen
                    kill_player(names[tmp]);
                else{
                    // Spieler ist restauriert - Flag loeschen
                    friends[names[tmp]][0] &= ~SELBSTGELOESCHT;
                    // und Selbstloeschungsdatum rausnehmen
                    friends[names[tmp]][1] =
                        sort_array( friends[names[tmp]][1], #'>/*'*/ )[1..];
                }
            }

            tmp++;
        }

        if ( tmp >= size ){
            // alle geloeschten Spieler ueberprueft
            step++;
            tmp = 0;
        }
        break;

    default:
        // Logfile-Eintrag schreiben
        write_file( LOGFILE,
                    sprintf( "\nEinträge gesamt: %4d. Davon inaktiv: %4d, "
                             "ohne Band: %4d, gelöscht: %4d.\n\n",
                             sizeof(friends),
                             sizeof(filter( m_indices(friends),
                                     #'filter_friends/*'*/, INAKTIV ) - ({0})),
                             sizeof(filter( m_indices(friends),
                                     #'filter_friends/*'*/, BAND_WEG ) - ({0})),
                             sizeof(filter( m_indices(friends),
                                     #'filter_friends/*'*/, SELBSTGELOESCHT )
                             - ({0})) ) );
        write_file( LOGFILE, dtime(time())+":  ************** Aufräumen "
                    "beendet **************\n\n\n" );
        // Aufraeumarbeiten sind erledigt. Bis zum naechsten Tag ...
        return;
    }

    // Statistiken ueber die Aufraeumarbeiten mitloggen
    if (tmp)
        write_file( LOGFILE, sprintf( "%s: Schritt %d bei Eintrag %d von %d "
                                      "unterbrochen. Verbliebene Eval-Ticks: "
                                      "%d.\n", dtime(time())[<8..], step,
                                      tmp, size, get_eval_cost() ) );
    else
        if (step-1)
            write_file( LOGFILE, sprintf( "%s: Schritt %d beendet. Verbliebene "
                                          "Eval-Ticks: %d.\n",
                                          dtime(time())[<8..], step-1,
                                          get_eval_cost() ) );
    
    // naechster Schritt in 10s
    call_out( "aufraeumen", 10, step, tmp );
}


// die wichtigste Funktion des Masters: wird immer aufgerufen, wenn sich etwas
// an den Daten eines Freundschaftsbandes geaendert haben koennte.
void update_data( string uid, string *Freunde )
{
    int flag, i;
    string *fr, *verachtet;
    object ob;
    mixed tmp;
    
    // darf nur vom FBand aus bzw. intern aufgerufen werden
    if ( extern_call() && (!previous_object() ||
         BLUE_NAME(previous_object()) != FBNAME) )
        return;

    if ( !friends[uid] )
        friends[uid] = ({ 0, ({}), ({}) });
    
    flag = friends[uid][0];
    verachtet = friends[uid][2];
    fr = Freunde;

    // war der Spieler mal geloescht? Dann Sonderbehandlung
    if ( flag & SELBSTGELOESCHT ){
        // keine Ahnung, ob das passieren kann - aber vielleicht ist das
        // Savefile noch nicht wieder auf die Platte geschrieben ...
        if ( !sizeof(tmp = "/secure/master"->get_userinfo(uid)) )
            return;

        // ist creation_date groesser als Selbstloeschungs-Datum?
        if ( tmp[5] > to_int(sort_array( friends[uid][1], #'>/*'*/ )[0]) )
            {
                // ja, dann ist es ein neuer Spieler mit altem Namen
                flag = 0;
                verachtet = ({});

                // aufraeumen. gruendlich :-)
                kill_player(uid);
            }
        else{
            // nein, es ist ein restaurierter Spieler - Flag loeschen
            flag &= ~SELBSTGELOESCHT;
            // und Selbstloeschungsdatum rausnehmen
            fr = sort_array( fr, #'>/*'*/ )[1..];
        }
    }

    // illegale Eintraege rauswerfen. Duerften hoechstens Altlasten sein
    fr = filter( fr, #'name_check/*'*/ ) - ({0});

    // Freunde, die einen verachtet haben, rauswerfen. Die wurden intern zwar
    // schon ausgetragen, aber durch die in der Property P_AUTOLOADOBJ
    // gespeicherten Daten beim Einloggen wieder ueberschrieben, sofern der
    // Spieler beim Verachten nicht online ist und sein FBand noch existiert
    for ( i = sizeof(verachtet); i--; )
        fr -= ({ verachtet[i] });

    // die Abfrage ist etwas komplizierter, weil bei einem "upd" z.B.
    // this_player() bzw. this_interactive() der updatende Magier ist bzw.
    // weil beim Einloggen ein find_player(uid) noch kein Ergebnis liefert
    if ( (ob = find_player(uid)) // einfachster Fall, Spieler ist anwesend
         || ((previous_object() && (ob = environment(previous_object()))
          && query_once_interactive(ob) && getuid(ob) == uid)) ){
        // Spieler ist anwesend - also nicht mehr inaktiv
        flag &= ~INAKTIV;

        // bin ich verachtet worden von jemandem? Dann benachrichtigen
        for ( i = sizeof(verachtet); i--; )
            tell_object( ob, "Irgendetwas ist mit Deiner Freundschaft zu "+
                         capitalize(verachtet[i])+" vorgefallen.\n" );
        
        verachtet = ({});

        // "ob" ist jetzt das FBand des Spielers
        if ( ob = present(BANDID, ob) ){
            // Band ist vorhanden
            flag &= ~BAND_WEG;
            
            tmp = ob->QueryProp(P_AUTOLOADOBJ);

            // Optionen des FBandes
            tmp = sizeof(tmp) ? tmp[0] : (ZEIGE_FREMDE | ZEIGE_EIGENE);

            // Daten mit dem FBand abgleichen
            ob->SetProp( P_AUTOLOADOBJ, ({ tmp, fr }) );
        }
        else
            // kein Band vorhanden
            flag |= BAND_WEG;
    }

    // interne Datenbank updaten
    friends[uid] = ({ flag, fr, verachtet });
}


// diese Funktion wird vom FBand aufgerufen, wenn sich ein Spieler ausloggt
void logout( string uid, string *Freunde, int logon )
{
    // darf nur vom FBand aus aufgerufen werden
    if ( BLUE_NAME(previous_object()) != FBNAME )
        return;

    if ( !logins[uid] )
        logins[uid] = ({});

    // Kurzaufenthalte werden nicht mitgeloggt
    if ( (time() - logon) < 300 )
        return;

    // immer nur 20 Eintraege pro Spieler speichern. Uralte Eintraege
    // (aelter als zwei Monate) werden beim taeglichen Aufraeumen entfernt
    logins[uid] = (logins[uid] + ({ ({ logon, time() }) }))[<20..];

    // sicherheitshalber die Freunde nochmal updaten
    update_data( uid, Freunde );
}


// die Funktion "fhist" des FBandes ruft QueryLogon auf
mixed QueryLogon( string uid, int anz, int magier )
{
    object ob;
    
    // Datenschutz ]:->
    /*
    if ( BLUE_NAME(previous_object()) != FBNAME ) {
	    write("nix da.\n");
        return 0;
    }
    */
    
    // wenn mehr als ein Datum fuer den Spieler angefordert wird, gibt es
    // einfach ein Array von Strings mit fertig formatierten Textzeilen
    // zurueck bzw. eine Null, falls keine Daten vorliegen
    if ( anz > 1 )
        return logins[uid] ? map( logins[uid][<anz..]
                                        || ({}), #'format_dates/*'*/ ) : 0;

    // wenn nur eine Auskunft gewuenscht ist, wird ein String zurueckgeliefert.
    // Dieser muss auf die jeweilige Situation angepasst werden:
    // invis-Magier werden nicht mehr durch "fhist" enttarnt ...
    if ( (ob = find_player(uid)) && (!ob->QueryProp(P_INVIS) || magier) )
        return "ist eingeloggt.";

    if ( !sizeof(logins[uid]) )
        return " - keine Informationen verfügbar!";

    // ... sondern als nicht eingeloggt ausgegeben
    return format_dates( logins[uid][<1] );
}


// die Freunde eines Spielers ausgeben. Je nach Option alle oder nach Flags
// sortiert. Die MPA bekommt z.B. nur die aktiven Freunde aufgelistet.
// Die Funktion wird z.B. auch bei jedem move() des FBandes aufgerufen, um
// bei einem neuen FBand sofort die richtigen Freunde wieder setzen zu koennen.
varargs string *get_friends( string uid, int flag )
{
    // Variablen "old"  und "ob" werden nur fuer die Uebergangsloesung gebraucht
    string *tmp, *old;
    object ob;

    // Datenschutz ]:->
    if ( extern_call() && BLUE_NAME(previous_object()) != FBNAME &&
         BLUE_NAME(previous_object()) != "/mail/mailer" )
        return 0;

    tmp = friends[uid] ? friends[uid][1] : ({});

    // beim ersten Aufruf durch das neue FBand werden eventuell vorhandene
    // Freunde vom "alten" FBand uebernommen
    if ( (!friends[uid] || !(friends[uid][0] & GEPRUEFT) ) && extern_call()
         && (ob = environment(previous_object())) && getuid(ob) == uid ){
        // falls noch gar keine Daten ueber den Spieler existieren, neu anlegen
        if ( !friends[uid] )
            friends[uid] = ({ 0, ({}),({}) });
    
        // Flag setzen, dass der Check auf alte Freunde ausgefuehrt wurde.
        // Das soll naemlich nur einmal geschehen pro Spieler.
        friends[uid][0] |= GEPRUEFT;

        // falls alte Freunde vorhanden sind, uebernehmen
        if ( sizeof(old = ob->QueryProp(P_FRIENDS)) ){
            // <UEBERGANGSLOESUNG>
            //
            // Fuer die Umstellung vom alten auf das neue FBand muessen die
            // folgenden Zeilen aktiviert werden. Die Properties vom alten
            // FBand werden geloescht und das evtl. noch vorhandene alte
            // FBand wird zerstoert.
        
            object *inv;
            int i;
          
            ob->Set( P_FRIENDS, 0 );
            ob->Set( P_FRIENDS, SAVE, F_MODE_AD );
            ob->Set( P_FOPT_FRIENDS, 0 );
            ob->Set( P_FOPT_FRIENDS, SAVE, F_MODE_AD );
          
            for ( i = sizeof(inv = deep_inventory(ob)); i--; )
                if ( BLUE_NAME(inv[i]) == "/obj/fband" )
                    destruct(inv[i]);
        
            // </UEBERGANGSLOESUNG>

            // ein paar Checks muessen sein - an P_FRIENDS kann jeder
            // herumgepfuscht haben
            old = filter( old, #'stringp/*'*/ ) - ({0});
            old = map( old, #'lower_case/*'*/ );
            old = filter( old, #'name_check/*'*/ ) - ({0});

            // die "neuen" Freundschaftsdaten erst einmal abziehen, damit keine
            // zwischenzeitlich schon eingetragenen Freunde doppelt gezaehlt
            // werden
            friends[uid][1] -= old;
            // ... und danach richtig eintragen.
            // Voila, die alten Daten sind kopiert
            friends[uid][1] += old;
        
            tmp = friends[uid][1];
        }
    }
    
    // einfache Abfrage liefert alle Freunde zurueck
    if (!flag)
        return tmp;

    // Sonderbehandlung fuer die MPA - alle Freunde **ohne** gesetzte Flags
    if ( flag == NUR_AKTIVE )
        return tmp - filter( tmp, #'filter_friends/*'*/,
                                   (INAKTIV|BAND_WEG|SELBSTGELOESCHT) );

    // nur Freunde mit gesetztem Flag ausgeben
    return filter( tmp, #'filter_friends/*'*/, flag ) - ({0});
}


// wenn ein Spieler einen anderen verachtet, ruft sein FBand diese Funktion auf.
// Die beiden Spieler werden aus der Freundesliste des jeweils anderen
// ausgetragen und der verachtende Spieler wird beim Opfer als "verlorener
// Freund" eingetragen, damit das Opfer benachrichtigt werden kann.
void hab_verachtet( string uid, string opfer, string *Freunde )
{
    // das sollte schon ein "echtes" Verachten sein ...
    if ( BLUE_NAME(previous_object()) != FBNAME )
        return;

    // den Ex-Freund austragen ...
    if ( friends[opfer] )
        friends[opfer][1] -= ({uid});
    else
        friends[opfer] = ({ 0, ({}), ({}) });

    // ... und fuer Benachrichtigung sorgen
    friends[opfer][2] += ({uid});

    // zum Schluss die Daten aktualisieren (und evtl. Opfer benachrichtigen)
    update_data( opfer, friends[opfer][1] );
    update_data( uid, Freunde );
}


// bei jedem fknuddel, frufe etc. wird gecheckt, ob ein Freund ohne FBand
// herumlaeuft. Dann wird diese Funktion mit einem Array aus allen
// Spielerobjekten ohne FBand aufgerufen.
void band_verloren( object *opfer )
{
    int i;
    
    // die obligatorische Abfrage
    if ( BLUE_NAME(previous_object()) != FBNAME )
        return;

    // einmal bei allen Opfern das Flag fuer "FBand verloren" setzen
    for ( i = sizeof(opfer); i--; )
        if ( friends[getuid(opfer[i])] )
            friends[getuid(opfer[i])][0] |= BAND_WEG;
        else
            friends[getuid(opfer[i])] = ({ BAND_WEG, ({}), ({}) });
}

/* }}} */

/* {{{ eigene Funktionen (intern) */

// da das alte FBand die Daten in einer oeffentlich faelschbaren Property
// gespeichert hat, muessen die Daten ueberprueft werden. Das ist zwar nicht
// 100%ig moeglich, aber wenigstens ein paar Plausibilitaetschecks sind drin.
private int name_check( string name )
{
    // Wenn eine der Namenskonventionen im MG nicht eingehalten ist,
    // kann der Freund nicht echt sein
    if ( !stringp(name) || strlen(name) < 2 || strlen(name) > 11 ||
         !sizeof(regexp( ({name}), "^[a-z]*$" )) )
        return 0;

    return 1;
}


// die Login-Daten werden intern als Integer gespeichert (Ausgabe von time()).
// Diese Funktion wandelt die Zeiten in lesbare Texte um.
private string format_dates( int *times )
{
    return sprintf( "vom %s bis %s, %s", dtime(times[0])[5..<4],
                    dtime(times[1])[5..<4], time2string( "%hh %mmin",
                                                         times[1] - times[0]) );
}


// Hilfsfunktion fuer get_friends(). Filtert Freunde nach gesetztem Flag
private int filter_friends( string name, int flag )
{
    if ( !friends[name] || !(friends[name][0] & flag) )
        return 0;

    return 1;
}


// wenn der Name eines geloeschten Spielers neu vergeben wird, muessen alle
// Daten ueber den alten Spieler geloescht werden. Vollstaendig.
private void kill_player( string name )
{
    int i;
    string *tmp;

    // bei geloeschten Spielern wird das Datum der Selbstloeschung als Freund
    // gespeichert - aber ein Datum sollte man nicht verachten ;^)
    tmp = sort_array( friends[name][1], #'>/*'*/ )[1..];

    // alle Freunde des zu loeschenden Spielers verachten
    for ( i = sizeof(tmp); i--; )
        {
            if ( !friends[tmp[i]] )
                friends[tmp[i]] = ({ 0, ({}), ({name}) });
            else{
                // als Freund austragen
                friends[tmp[i]][1] -= ({name});
                // und fuer Benachrichtigung sorgen
                friends[tmp[i]][2] += ({name});
            }

            // Benachrichtigen der Ex-Freunde des Ex-Spielers
            update_data( tmp[i], friends[tmp[i]][1] );
        }

    // die Spieler-Daten endgueltig killen ...
    efun::m_delete( friends, name );
    efun::m_delete( logins, name );
}

/* }}} */

/* }}} */
