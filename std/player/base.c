// Tamedhon MUDlib
//
// PLAYER/BASE.C -- the basic player object
//
/* 
 *
 * 2024-12-26	Gralkor	Fehlerbehandlung sollten Ausrüstungen buggen
 *
 * Revision 4.0 2023/07/19 Aloy
 * Charaktereditor implementiert statt nur Zufallswahl (beides wählbar)
 * neue Ansprache bei Geschlechterwahl
 * "maennlich/weiblich" voll ausgeschrieben bei Geschlechterwahl akzeptiert
 *
 * Revision 3.15 2012/06/17 17:00:00 Grynnis
 * noch eine kleine Verbesserung beim Fehlermelden an Objekten (detail:text)
 *
 * Revision 3.14 2012/06/15 18:30:00 Grynnis
 * kleine Verbesserung beim Fehlermelden an Objekten (lower_case)
 *
 * Revision 3.13 2012/05/17 13:00:00 Grynnis
 * Bugfix beim Farbe wechseln von Channels die nicht betreten sind
 *
 * Revision 3.12 2012/04/03 20:00:00 Rommy
 * "sprachausgabe" in "ausgabe" umbenannt
 *
 * Revision 3.11 2012/04/01 21:00:00 Rommy
 * P_NO_ASCII_ART + "sprachausgabe" eingefuehrt, "info" angepasst
 *
 * Revision 3.10 2010/09/26 14:40:00 Gralkor
 * Einfuegung von QueryDefaultHome
 * Damit koennen automatisch eigene P_START_HOMES gesetzt und wieder
 * geloescht werden
 *
 * Revision 3.9 2009/08/09 10:00:00  Rommy
 * winzige Formatierungsaenderung bei Fehlermeldungen,
 * docolor() in setcolor() umbenannt und optimiert,
 * Faerben der Ebenen ueber "farbe"-Kommando
 *
 * Revision 3.8 2009/06/07 14:00:00  Rommy
 * Gaeste erhalten eine Stadtkarte von Gidhrog
 *
 * Revision 3.7 2009/04/22 11:00:00  Rommy/Grynnis
 * Melden von Fehlern komfortabler fuer Spieler und Magier:
 * Abfrage vorm Loggen, Meldung an anwesenden Magier
 *
 * Revision 3.6  2008/05/17 13:00:00  TamTam
 * Hihi. Neue Orks muessen Menschenkleider tragen wegen Typo :)
 * Orks und Trolle bekommen ihre eigenen Kleider
 *
 * Revision 3.5  2008/05/11 16:00:00  TamTam
 * Spieler-Levelgrenze raus, Geburtsmeldung erst, wenn Spieler 'da'
 * Info an Questmeister im Tal der Pruefungen bei neuem Spieler
 *
 * Revision 3.4  2008/04/03 22:00:00  TamTam
 * Zweiti jetzt korrekt angezeigt auch wenn versteckt
 * delayed_notify() Erst wenn Spieler erfolgreich initialisiert
 *
 * Revision 3.3  2008/03/29 12:00:00  TamTam
 * 'finde' fuer Erzseher
 * remotes finden Spieler jetzt auch mit 'r: Tamtam ..'
 *
 * Revision 3.2  2008/01/19 12:00:00  TamTam
 * Augen-/Haar-/Hautfarbe+Racespecial anzeigen in long()
 * P_EYE_COLOR, P_HAIR_COLOR, P_SKIN_COLOR, P_HAIR_STYLE, P_RACE_SPECIAL
 * Erweitertes Aussehen fuer Seher abschaltbar -> setlook()
 * Email Adresse unsichtbar fuer Nicht-Admins -> aktiviert
 * 'email loeschen' setzt jetzt "none" statt '0'
 * Zweitspieler unsichtbar -> aktiviert
 *
 * Revision 3.1  2007/06/08 06:00:00  Gralkor
 * Funktion lob eingefuehrt, um Magier im Repfile zu loben
 *
 * Revision 3.00  2006/11/18 23:23:00  Serii
 * P_MAILADDR_INVIS (plus aenderung in set_email) und P_SECOND_INVIS
 * eingefuehrt - ersteres um email addressen fuer spieler unsichtbar
 * (-> nur fuer magier sichtbar) und markierte zweitis
 * fuer spieler 'unsichtbar' zu machen
 *
 * Revision 2.99  2006/07/09 00:30:00  Serii
 * anpassung fuer extra_skills: extra_skills inherit eingefuegt
 * und create() & _query_localcmds() entsprechend erweitert
 * + keine std kleidung fuer daemonen
 *
 * Revision 2.98  2006/05/23 18:42:00  Serii
 * kanal-farb-sachen ausgebaut (ist ja im channeld)
 *
 * Revision 2.97  2006/05/14 18:12:00  Querolin
 * Newbie und Reboot-Kleidung - damit man ned Nackt dasteht...
 *
 * Revision 2.96  2005/07/06 11:00:00  Gralkor
 * remote nicht mehr an QP sondern an Questbelohnung
 *
 * Revision 2.95  2003/12/21 11:51:00  Falgrim
 * neue farboptionen fuer 'inform' & 'erwarte'.
 *
 * Revision 2.92  2003/11/04 12:35:00  Serii
 * kleinen bug aus al_to_title() entfernt
 *
 * Revision 2.91  2003/08/10 11:51:00  Falgrim
 * kleinere aenderungen an:
 * static int docolor(string str1, string obj, string str2)
 * - gewaehlte farbe wird farblich angezeigt
 * - geht nur wenn stty auf stty auf ansi eingestellt ist
 *
 * Revision 2.9  2003/07/20 11:51:00  Falgrim
 * es gibt jetzt eine zentrale farbfunktion:
 * static int docolor(string str1, string obj, string str2)
 * str1 = farbe, obj = "property" str2 = (zb) "Kanaele"
 *
 * Revision 2.8  2003/04/16 17:30:00  Falgrim
 * aenderungen beim farbbefehl ('fehler'meldung
 * bei nicht unterstuetzten farboptionen)
 *
 * Revision 2.7  2003/03/21 17:30:00  Falgrim
 * "farbe" auf bunte ausgaenge erweitert.
 * ansi.h beachten!
 *
 * Revision 2.6  2003/02/23 17:20:00  Querolin
 * info um Alkoholanzeige erweitert.
 *
 * Revision 2.5  2002/10/20 17:30:00  Querolin
 * "schlaf" und "_quit" um log_file("LEAVE"); erweitert.
 *
 * Revision 2.4  2002/08/04 09:30:00  Querolin
 * Kampfmeldungsauswahl (alt/neu)
 *
 * Revision 2.3  2002/07/03 19:30:00  Querolin
 * Geburtsanzeige
 *
 * Revision 2.2  2002/06/20 19:00:00  Querolin
 * Fix bei icq (return 1; am Ende)
 *
 * Revision 2.1  2002/04/14 19:00:00  Querolin
 * filter_who_options() erweitert "long alpha" - Unitopia libs...
 *
 * Revision 2.0  2002/03/30 13:15:00  Querolin
 * Anzeige fuer saettigung bei P_FOOD und P_DRINK eingebaut in 'info'
 *
 * Revision 1.9  2002/02/14 08:54:00  Querolin
 * diverse bugs entfernt. sowie advance fuer /std/gilde.c auskommentiert.
 *
 * Revision 1.8  2001/06/24 16:56:00  Querolin
 * stty noch umgebaut, damit P_COLOR* zurueckgesetzt wird!
 *
 * Revision 1.7  2001/06/04 12:56:00  Querolin
 * Einbau von 'icq' und 'homepage'.
 *
 * Revision 1.6  2001/06/02 16:45:00  Querolin
 * Einbau von 'farbe'.
 *
 * Revision 1.5  2001/01/28 19:09:36  Samtpfote
 * Autokorrekur von P_ALIGN, wenns zu gross/klein ist.
 *
 * Revision 1.4  2001/01/18 10:19:30  Samtpfote
 * Grammatikfehler bei der Gewichtsanzeige ausgebuegelt
 *
 * Revision 1.3  2001/01/17 19:57:58  Samtpfote
 * EMail-Adresse kann nun geloescht werden
 * persoenliche Angabe des Ortes nun moeglich
 *
 * Revision 1.2  2001/01/13 17:24:05  Samtpfote
 * Invis kann man nur mehr ab Magierlevel 35 werden
 *
 * Revision 1.1  2001/01/13 12:14:22  Samtpfote
 * Initial revision
 *
 *
 * Revision 1.2   2001/01/05 18:20:00  Querolin
 * Bei 'Vorsicht' die Eingabe von Prozenten ermoeglicht
 *
 * Revision 1.1   2001/01/04 22:00:01  Querolin
 * Gewichtsanzeige bei 'info' eingebaut
 *
 * Revision 1.0   2001/01/04 22:00:00  Querolin
 * Initialiesierung der Versionsnummer
 *
 * Revision 3.21  1998/02/12 15:40:17  Woody
 * P_DEFAULT_GUILD SAVE setzen, bei score beruecksichtigen
 * Spielerobjekt speichern nach erfolgter Initialisierung
 *
 * Revision 3.20  1998/02/05 02:02:40  Woody
 * Typo bei 'bug', wodurch Bugs nicht in .bug landeten, beseitigt
 *
 * Revision 3.19  1998/01/23 21:19:11  Woody
 * Bugfix in wache()
 *
 * Revision 3.18  1998/01/21 16:05:02  Feyaria
 * Aufsplitten des repfiles nach typ und region, befehl detail
 *
 * Revision 3.17  1998/01/15 08:03:07  Crea
 * removed start_player(): Set(P_WANTS_TO_LEARN, 1); => now prop is saved and restored right.
 *
 * Revision 3.16  1997/11/25 15:53:03  Feyaria
 * pluendern
 *
 * Revision 3.15  1997/11/10 12:15:25  Feyaria
 * titel, presay msgin msgout laengenmaessig beschraenkt
 *
 * Revision 3.14  1997/11/05 19:33:16  Feyaria
 * Silberland in MUDNAME
 *
 * Revision 3.13  1997/11/05 16:20:00  Woody
 * remote fuer Seher mit allen QP, zeilen rel|abs, Rasten/Aufstehen/wachen
 *
 * Revision 3.12  1997/09/28 02:05:39  Woody
 * Zurueck von den Netztoten nicht mehr M_SILENT
 *
 * Revision 3.11  1997/09/20 11:17:55  Woody
 * behalte waffen|ruestungen|alles|nichts eingebaut
 *
 * Revision 3.10  1997/09/17 14:36:37  Woody
 * passwd Variable private, seltsamem getmanpage() Verhalten vorbeugen,
 * Erwartete auch bei Reconnect anzeigen, P_TESTPLAYER setzt autom. uid
 *
 * Revision 3.9  1997/08/28 12:52:08  Woody
 * Bei 'wer' nun auch explizit 'mud mudname' moeglich
 *
 * Revision 3.8  1997/08/21 14:55:55  Woody
 * Bugfix (wer -a ging nimmer...)
 *
 * Revision 3.7  1997/08/21 14:11:05  Woody
 * wer weiter verbessert, auch weitere Parameter per UDP verschicken
 * (kwer -a silberland elf und aus wien etc.)
 *
 * Revision 3.6  1997/08/20 22:56:02  Woody
 * wer -k etc. auch remote
 *
 * Revision 3.5  1997/08/05 14:12:34  Feyaria
 * titel mit \b NUR fuer beistrich!!
 *
 * Revision 3.4  1997/06/25 12:00:12  Woody
 * quit() im Kampf verzoegert, sonst je nach letztem Spell,
 * RCS logs aufgeraeumt, checkConsistency(), smart_log Bugfix,
 * SmartLog() in Objekten aufrufen, wieder 2 \n beim log,
 * call PlayerQuit() auch im inventory von Spieler und Raum,
 * P_NOQUIT verhindert quitten, P_NO_GLOBAL_ATTACK+P_FRIEND,
 * gremote in rgemote umbenannt, P_MV nach Reboot auf Max,
 * Mini-Editor fuer Extralook, Befehl erweitert, Autoload
 * von VC-Objekten moeglich, vorsicht bugfixed, stty reset,
 * ANSI/VT100 Farbtest bei stty, alle M_GO moves M_NOCHECK
 *
 * Revision 3.3  1997/06/23 11:00:40  Woody
 * Big Brother installiert
 *
 * Revision 3.2  1997/05/15 00:12:36  Woody
 * Auf Wuschels Quengeln hin kriegen Magier /etc/NEWS nicht mehr
 * zu lesen (beim Login), dafuer aber /etc/WIZNEWS :)
 *
 * Revision 3.1  1997/03/16 03:48:28  Woody
 * Verwendet jetzt finger aus /p/service/obj/
 *
 * Revision 3.0  1997/03/13 01:31:11  Woody
 * prepared for revision control
 *
 * Revision 2.180  97/02/10, Woody
 * punkte/score und info getrennt, altes Format endgueltig verworfen
 *
 * Revision 2.179  97/02/03, Woody
 * P_ALIGN_STRING, Vergiftung nach Reboot weg
 *
 * Revision 2.178  97/01/25, Woody
 * query_ip_name(ME) statt query_ip_name(), where() rausgeworfen
 *
 * Revision 2.177  97/01/22, Woody
 * P_CALLED_FROM_IP(_NAME) richtig setzen (zudem auch bei Reconnect),
 * ebenso P_LAST_LOGIN (ebenfalls auch bei Reconnect) und P_LAST_LOGOUT
 * (bei disconnect() und quit() statt bei save_me())
 *
 * Revision 2.176  97/01/18, Woody
 * BecomesNetAlive() auch aufrufen wenn 'nur' disconnected gewesen...
 *
 * Revision 2.175  97/01/06, Woody
 * Postkastensymbol beim Einloggen, wenn Briefe im Postamt
 * (g)remote nicht erst ab Seher
 *
 * Revision 2.174  96/10/12, Woody
 * Befehl gremote wieder aktiviert (ist dasselbe wie "r;")
 *
 * Revision 2.173  96/10/02, Woody
 * Generalrevision: :)
 * creation_date wieder raus (wird nun von login non-SAVE aber SECURE gesetzt)
 * AddId("Interactive"), selbstloeschungstext geaendert, Befehl 'spielpause',
 * Austreten aus Gilde bei selbstloeschung, Anderer Text bei Selbstloeschung
 * bei 5 oder mehr Toden, Vergiftung wird in 'score[2]' angezeigt,
 * hardcoded Einlogg-Text raus (sollte eh in /etc/NEWS stehn),
 * Saettigung (Food und Drink) und Alkohol werden nach Reboot auf 0 gesetzt,
 * Levelobergrenze von 40 auf 75 angehoben, unparsed_args fuer Fluchtrichtung
 *
 * Revision 2.172  96/10/01, Woody
 * *filterattr* an neues Attributsystem angepasst (A_MIN/A_MAX),
 * Stufen- und Attributpunkte in score-Anzeige rein
 *
 * Revision 2.171  96/09/30, Woody
 * P_GUILD_RATING rein.
 *
 * Revision 2.170  96/09/25, Woody
 * std/player/exploration reingehaengt.
 *
 * Revision 2.169  96/09/24, Woody
 * int creation_date, _query_creation_date, P_CREATION_DATE,
 * parentmaster nach /secure verschoben, score2 Anzeige.
 *
 * Revision 2.168  96/07/02, Woody
 * typo/bug/idea bezieht sich nun *immer* auf Raum (ausser bei "idee x:yz"),
 * ein evtl. P_REFERENCE_OBJECT wird in {} nachgestellt.
 *
 * Revision 2.167  96/06/08, Woody
 * Titel setzen u.ae. beim login fuer Gaeste debugged
 *
 * Revision 2.166  96/05/27, Woody
 * PARENTMASTER->RemoveWiz() bei selbstloeschung aufrufen
 *
 * Revision 2.165  96/05/26, Woody
 * "review" an "#" Makro fuer msgout angepasst
 *
 * Revision 2.164  96/05/25, Woody
 * Befehl "anzeige", inherited nun wieder /std/player/sense
 *
 * Revision 2.163g 96/04/29, Creative
 * BecomesNetAlive() -> auch fuer environment verwenden.
 *
 * Revision 2.163f 96/04/17, Woody
 * BecomesNetAlive()
 *
 * Revision 2.163e 96/04/17, Woody
 * fehler/typo/idee beruecksichtigen auch P_REFERENCE_OBJECT
 *
 * Revision 2.163d 96/04/14, Woody
 * emote gefixt (war erst ab Seher mgl.), gemote wiedereingefuehrt
 *
 * Revision 2.163c 96/01/25, Woody
 * al_to_title() nich mehr static
 *
 * Revision 2.163b 96/01/23, Woody
 * AnderLand -> SilberLand
 *
 * Revision 2.163a 96/01/16, Woody
 * msg wg. port 4711 raus
 *
 * Revision 2.163	02-02-1996	Mupfel
 * P_TEMPERATURE_RANGE wird ins savefile 'protected' uebernommen
 * dafuer '/sys/environment.h' included
 *
 */

#pragma strong_types


#include "/std/sys_debug.h"
#include <daemon.h>
#include <wizlevels.h>
#include <input_to.h>

inherit "/std/player/restrictions";

inherit "/std/player/extra_skills";

inherit "/std/living/attributes";
inherit "/std/living/combat";
inherit "/std/living/put_and_get";
inherit "/std/thing/properties";
inherit "/std/thing/util";
inherit "/std/thing/language";

inherit "/std/player/description";
inherit "/std/player/moving";
inherit "/std/player/life";
inherit "/std/player/comm";
inherit "/std/player/viewcmd";
inherit "/std/player/moneyhandler";
inherit "/std/player/command";
inherit "/std/player/sense";
inherit "/std/living/skills";
inherit "/std/player/quests";
inherit "/std/player/exploration";
inherit "/std/player/soul";
inherit "/std/player/temperatur";
inherit "/std/more";
inherit "/std/user_filter";

#ifdef NEDIT_FOR_EXTRALOOK
inherit "/mail/nedit";
#endif

#include <properties.h>
#include <udp.h>
#include <config.h>
#include <ansi.h>
#include <wizlevels.h>
#include <living.h>
#include <attributes.h>
#include <exploration.h>
#include <language.h>
#include <moving.h>
#include <defines.h>
#include <terminal.h>
#include <new_skills.h>
#include <environment.h>
#include <userinfo.h>
#include <pager.h>
#include "/secure/questmaster.h"
#include "/secure/parentmaster.h"
#include "/std/player/set_is_wizard.c"

#define DB(x) if(find_player("grynnis")) tell_object(find_player("grynnis"),x)

#undef NAME /* DEFINED BY UDP.H; BAD NAME CLASH :( */
#define NAME(who) capitalize(getuid(who))

// The rainbow is divided into 6 segments of 10 steps each.
// For simplicity some boundary colors are repeated.
#define ANSI_RAINBOW  \
    /* Segment 1: Red -> Yellow */ \
    ANSI_TRUE_COLOR(255,0,0)   + "█" + \
    ANSI_TRUE_COLOR(255,28,0)  + "█" + \
    ANSI_TRUE_COLOR(255,57,0)  + "█" + \
    ANSI_TRUE_COLOR(255,85,0)  + "█" + \
    ANSI_TRUE_COLOR(255,113,0) + "█" + \
    ANSI_TRUE_COLOR(255,142,0) + "█" + \
    ANSI_TRUE_COLOR(255,170,0) + "█" + \
    ANSI_TRUE_COLOR(255,198,0) + "█" + \
    ANSI_TRUE_COLOR(255,227,0) + "█" + \
    ANSI_TRUE_COLOR(255,255,0) + "█" + \
    /* Segment 2: Yellow -> Green */ \
    ANSI_TRUE_COLOR(255,255,0) + "█" + \
    ANSI_TRUE_COLOR(227,255,0) + "█" + \
    ANSI_TRUE_COLOR(198,255,0) + "█" + \
    ANSI_TRUE_COLOR(170,255,0) + "█" + \
    ANSI_TRUE_COLOR(142,255,0) + "█" + \
    ANSI_TRUE_COLOR(113,255,0) + "█" + \
    ANSI_TRUE_COLOR(85,255,0)  + "█" + \
    ANSI_TRUE_COLOR(57,255,0)  + "█" + \
    ANSI_TRUE_COLOR(28,255,0)  + "█" + \
    ANSI_TRUE_COLOR(0,255,0)   + "█" + \
    /* Segment 3: Green -> Cyan */ \
    ANSI_TRUE_COLOR(0,255,0)   + "█" + \
    ANSI_TRUE_COLOR(0,255,28)  + "█" + \
    ANSI_TRUE_COLOR(0,255,57)  + "█" + \
    ANSI_TRUE_COLOR(0,255,85)  + "█" + \
    ANSI_TRUE_COLOR(0,255,113) + "█" + \
    ANSI_TRUE_COLOR(0,255,142) + "█" + \
    ANSI_TRUE_COLOR(0,255,170) + "█" + \
    ANSI_TRUE_COLOR(0,255,198) + "█" + \
    ANSI_TRUE_COLOR(0,255,227) + "█" + \
    ANSI_TRUE_COLOR(0,255,255) + "█" + \
    /* Segment 4: Cyan -> Blue */ \
    ANSI_TRUE_COLOR(0,255,255) + "█" + \
    ANSI_TRUE_COLOR(0,227,255) + "█" + \
    ANSI_TRUE_COLOR(0,198,255) + "█" + \
    ANSI_TRUE_COLOR(0,170,255) + "█" + \
    ANSI_TRUE_COLOR(0,142,255) + "█" + \
    ANSI_TRUE_COLOR(0,113,255) + "█" + \
    ANSI_TRUE_COLOR(0,85,255)  + "█" + \
    ANSI_TRUE_COLOR(0,57,255)  + "█" + \
    ANSI_TRUE_COLOR(0,28,255)  + "█" + \
    ANSI_TRUE_COLOR(0,0,255)   + "█" + \
    /* Segment 5: Blue -> Magenta */ \
    ANSI_TRUE_COLOR(0,0,255)   + "█" + \
    ANSI_TRUE_COLOR(28,0,255)  + "█" + \
    ANSI_TRUE_COLOR(57,0,255)  + "█" + \
    ANSI_TRUE_COLOR(85,0,255)  + "█" + \
    ANSI_TRUE_COLOR(113,0,255) + "█" + \
    ANSI_TRUE_COLOR(142,0,255) + "█" + \
    ANSI_TRUE_COLOR(170,0,255) + "█" + \
    ANSI_TRUE_COLOR(198,0,255) + "█" + \
    ANSI_TRUE_COLOR(227,0,255) + "█" + \
    ANSI_TRUE_COLOR(255,0,255) + "█" + \
    /* Segment 6: Magenta -> Red */ \
    ANSI_TRUE_COLOR(255,0,255) + "█" + \
    ANSI_TRUE_COLOR(255,0,227) + "█" + \
    ANSI_TRUE_COLOR(255,0,198) + "█" + \
    ANSI_TRUE_COLOR(255,0,170) + "█" + \
    ANSI_TRUE_COLOR(255,0,142) + "█" + \
    ANSI_TRUE_COLOR(255,0,113) + "█" + \
    ANSI_TRUE_COLOR(255,0,85)  + "█" + \
    ANSI_TRUE_COLOR(255,0,57)  + "█" + \
    ANSI_TRUE_COLOR(255,0,28)  + "█" + \
    ANSI_TRUE_COLOR(255,0,0)   + "█" + \
    ANSI_PLAIN

mapping autoload;           /* autoload-mapping */

// Augen-/Haar-/Hautfarbe+Racespecial
static int size;
static int weight;
static string size_desc, weight_desc;
static string eye_color;
static string hair_color;
static string skin_color;
static string hair_style;
static string race_special;

static string saved_where;
private static string passw;        /* temporarily for password change */
private static string passwold;     /* temporarily for password change */

static string default_home; /* Where to move us if we dont have a home set */

static int ndead_lasttime;
static object ndead_location;
static string ndead_l_filename;
static int ndead_currently;
static int ndead_next_check;
static object *hb_obs;

static string pray_room;

static int _set_level(int i);
static int _set_invis(int a);
static string _set_tty(string str);
static mixed env_ndead_info;
static string my_ip;

// static string al_to_title(int a);
string al_to_title(mixed a);

static void ndead_revive();

static mixed PlayerMore();

int quit();
void save_me(mixed value_items);
varargs int remove(mixed arg);

void create()
{
  if(QueryProp(P_LEVEL)) return; // darf nur EINMAL gemacht werden

  call_out("checkConsistency", 0);

  ndead_next_check=NETDEAD_CHECK_TIME;
  ndead_lasttime=0;
  ndead_location=0;
  ndead_l_filename=0;
  ndead_currently=0;
  ndead_next_check=0;

  sense::create();
  command::create();
  properties::create();
  description::create();
  attributes::create();
  combat::create();
  life::create();
  comm::create();
  viewcmd::create();
  quests::create();
  restrictions::create();
  moving::create();
  exploration::create();
  extra_skills::create();

  SetProp(P_LEVEL, -1);
  Set(P_LEVEL, SAVE|SECURED, F_MODE_AS);
  Set(P_GHOST, SAVE, F_MODE_AS);
  SetProp(P_SCREENSIZE, 20);
  Set(P_SCREENSIZE, SAVE,F_MODE_AS);
  Set(P_MORE_FLAGS, SAVE, F_MODE_AS);
  SetProp(P_NO_ASCII_ART, 0);
  Set(P_NO_ASCII_ART, SAVE, F_MODE_AS);
  SetProp(P_WEIGHT_PERCENT,100);
  SetProp(P_LONG, 0);
  SetProp(P_TITLE, "der hoffnungsvolle Anfänger");
  SetProp(P_ALIGN, 0);
  SetProp(P_ALIGN_STRING, 0);
  SetProp(P_GENDER, NEUTER);
  Set(P_GENDER, SAVE, F_MODE_AS);
  SetProp(P_TTY, "ansi");
  Set(P_ANSI_TRUE_COLOR, SAVE, F_MODE_AS);
  Set(P_TTY, SAVE, F_MODE_AS);
  SetProp(P_WEIGHT, 75000);
  SetProp(P_MAX_HP,50);
  SetProp(P_MAX_SP,50);
  SetProp(P_MAX_FOOD,100);
  SetProp(P_MAX_DRINK,100);
  SetProp(P_MAX_ALCOHOL,100);
  SetProp(P_MAX_POISON,10);
  SetProp(P_DRINK, SetProp(P_MAX_DRINK,100));
  SetProp(P_FOOD, SetProp(P_MAX_FOOD,100));
  SetProp(P_ALCOHOL, 0);
  SetProp(P_ARMOURS,({}));
  SetProp(P_HANDS, ({" mit bloßen Händen", 30}));
  Set(P_HANDS, SAVE, F_MODE_AS);
  SetProp(P_MAX_HANDS, 2);
  SetProp(P_USED_HANDS, 0);

  Set(P_LOST_XP, SAVE|SECURED, F_MODE_AS);
  Set(P_LOOTING, SAVE, F_MODE_AS);
  SetProp(P_LOST_XP,0);
  SetProp(P_LOOTING,0);
  Set(P_MARRIED, SAVE, F_MODE_AS);
  Set(P_EXTRA_LOOK, SAVE, F_MODE_AS);
  Set(P_LOOK_INVIS, SAVE|SECURED, F_MODE_AS);
  SetProp(P_LOOK_INVIS,0);
  Set(P_SHOW_EXITS, SAVE, F_MODE_AS);
  Set(P_SHOW_EXITS, 1);
  Set(P_WANTS_TO_LEARN, SAVE, F_MODE_AS);
  Set(P_CAN_FLAGS, SAVE, F_MODE_AS);
  Set(P_TESTPLAYER, SAVE, F_MODE_AS);
  Set(P_LAST_ROOM, SAVE, F_MODE_AS);

  Set(P_SECOND, SAVE|SECURED, F_MODE_AS);
  Set(P_SECOND_INVIS, SAVE|SECURED, F_MODE_AS);
  SetProp(P_SECOND_INVIS,0);

  Set(P_INVIS, SAVE, F_MODE_AS);
  Set(P_READ_NEWS, SAVE, F_MODE_AS);
  Set(P_START_HOME, SAVE, F_MODE_AS);

  Set(P_MAILADDR, SAVE|SECURED, F_MODE_AS);
  Set(P_MAILADDR_INVIS, SAVE|SECURED, F_MODE_AS);
  SetProp(P_MAILADDR_INVIS,1);

  Set(P_LOGON_PLACE, SAVE, F_MODE_AS);

  Set(P_CARRIED_VALUE, SAVE, F_MODE_AS);

  Set(P_PROMPT, "> ");
  Set(P_PROMPT, SAVE, F_MODE_AS);
  Set(P_CALLED_FROM_IP, SAVE, F_MODE_AS);
  Set(P_CALLED_FROM_IP_NAME, SAVE, F_MODE_AS);
  Set(P_INFORMME,SAVE|PROTECTED,F_MODE_AS);
  Set(P_WAITFOR,SAVE|PROTECTED,F_MODE_AS);

  autoload = ([]);
  SetProp(P_ARTICLE,0);
  Set(P_GUILD,SAVE,F_MODE_AS);
  Set(P_DEFAULT_GUILD,SAVE,F_MODE_AS);
  Set(P_GUILD_TITLE,SAVE,F_MODE_AS);
  Set(P_GUILD_LEVEL,SAVE,F_MODE_AS);
  Set(P_GUILD_RATING,SAVE,F_MODE_AS);
  Set(P_NEWSKILLS,SAVE,F_MODE_AS);
  Set(P_NEEDED_QP,QM->QueryNeededQP());
  Set(P_DEADS,0);
  Set(P_DEADS,-1,F_SET_METHOD);
  Set(P_DEADS,SAVE|PROTECTED|SECURED,F_MODE_AS);
  Set(P_LAST_DEATH_SEQUENCES,SAVE,F_MODE_AS);
  Set(P_LAST_AWAKE_TIME, SAVE|PROTECTED, F_MODE_AS);
  Set(P_LAST_LOGIN,-1);
  Set(P_LAST_LOGIN,-1,F_SET_METHOD);
  Set(P_LAST_LOGIN,SAVE|PROTECTED|SECURED,F_MODE_AS);
  Set(P_LAST_LOGOUT,-1);
  Set(P_LAST_LOGOUT,-1,F_SET_METHOD);
  Set(P_LAST_LOGOUT,SAVE|PROTECTED|SECURED,F_MODE_AS);
  Set(P_CLOCKMSG,SAVE,F_MODE_AS);
  Set(P_COLUMN_MODE,SAVE,F_MODE_AS);
  Set(P_SHORTINV,SAVE,F_MODE_AS);

  Set(P_TEMPERATURE_RANGE, PROTECTED, F_MODE_AS);
  SetProp(P_TEMPERATURE_RANGE, ([T_RANGE_MIN:5,T_RANGE_MAX:25]));

  Set(P_CMSG, 0, F_MODE); // to clean out the old clone messages
  Set(P_DMSG, 0, F_MODE);
  Set(P_CLONE_MSG, SAVE, F_MODE);
  SetProp(P_CLONE_MSG, "zaubert etwas hervor");
  Set(P_DESTRUCT_MSG, SAVE, F_MODE);
  SetProp(P_DESTRUCT_MSG, "verschwindet einfach");
/*
  SetProp(P_WEIGHT, 70000+random(10000));
  SetProp(P_SIZE, 160+random(35));
  SetProp(P_AVERAGE_WEIGHT, 75000);
  SetProp(P_AVERAGE_SIZE, 173);

  Set(P_WEIGHT, SAVE, F_MODE_AS);
  Set(P_SIZE, SAVE, F_MODE_AS);
  Set(P_AVERAGE_WEIGHT, SAVE, F_MODE_AS);
  Set(P_AVERAGE_SIZE, SAVE, F_MODE_AS);
*/
  AddId("Interactive");
}

void checkConsistency()
{
  int phands;

  phands=Query(P_HANDS)[1];
  if(((phands < 20) || (phands > 40)) && !IS_LEARNER(this_object())) {
    log_file("inconsistent", sprintf(
      "[%s] %O: HANDS: %d\n",dtime(time()),this_object(),Query(P_HANDS)[1]));
    Query(P_HANDS)[1] = 30;
  }
}

varargs int remove(mixed arg)
{
  return moving::remove(arg);
}

varargs static void stop_heart_beats(mixed obs)
{
  int i;

  if (!obs)
  {
    hb_obs=({});
    obs=deep_inventory(ME);
  }
  for (i=sizeof(obs)-1;i>=0;i--)
    if (pointerp(obs[i]))
      stop_heart_beats(obs[i]);
    else
      if (set_object_heart_beat(obs[i],0))
    hb_obs+=({obs[i]});
}

static void restart_heart_beats()
{
  int i;

  if (pointerp(hb_obs))
  {
    for (i=sizeof(hb_obs)-1;i>=0;i--)
      set_object_heart_beat(hb_obs[i],1);
    hb_obs=0;
  }
}

varargs void Reconnect(int silent, string ip)
{
  int num;
  object *inv;

  my_ip=ip;
  enable_commands();
  set_living_name(getuid());
  _remove_netdead();
  set_heart_beat(1);
  restart_heart_beats();
  log_file("REENTER",
       QueryProp(P_NAME) + ", " + extract(ctime(time()), 4, 15)+ ", "+
           query_ip_name(ME)+".\n");
  if (!silent&&environment()&&object_name(environment())!=NETDEAD_ROOM)
    say(QueryProp(P_NAME)+" weilt wieder unter den Lebenden.\n");
  if (!silent)
    "/secure/merlin"->notify_player_enter(getuid(ME));
  command::reconnect();
  ME->_set_currentdir(ME->_query_currentdir());
  if (num="secure/mailer"->FingerMail(geteuid()))
    write("\n  __ /\n"
            " |__|   Du hast "+num+" neue"+(num==1 ? "n Brief" : " Briefe")+
            " im Postamt liegen.\n\n");
  catch(VOICEMASTER->RegisterUser(ME));
  funcall(#'Set, P_CALLED_FROM_IP, query_ip_number()); //');
  funcall(#'Set, P_CALLED_FROM_IP_NAME, query_ip_name(ME)); //');
  if(query_once_interactive(ME))
    Set(P_LAST_LOGIN,time());
  if (ndead_currently) {
    string *list;
    object ob;
    int i,mag;

    list=QueryProp(P_WAITFOR);
    mag=IS_LEARNER(ME);
    for (i=sizeof(list)-1;i>=0;i--)
      if (!(ob=find_player(lower_case(list[i]))) ||
          (!mag && IS_LEARNER(ob) && ob->QueryProp(P_INVIS)))
	list=list[0..i-1]+list[i+1..];
    if (sizeof(list))
      write(break_string("Anwesende Erwartete: "+CountUp(list)+".", 78));

    ndead_revive();
  }
  else
  {
    if (environment())
      catch(environment()->BecomesNetAlive(ME));
    catch(map_objects(deep_inventory(ME), "BecomesNetAlive", ME));
    // (fuer Inventory ehemals NetDeadEnvironmentReconnected()...)
  }
}

void NetDead()
{
  object *inv;
  int nr;

  say("Plötzlich weicht alles Leben aus "+QueryProp(P_NAME)+".\n");
  _set_netdead();
  remove_call_out("quit");
  remove_living_name();
  catch("secure/merlin"->notify_player_leave(geteuid(ME)));
  catch(VOICEMASTER->RemoveUser(ME));
  Set(P_LAST_LOGOUT,time());
  if (environment()) {
    catch(environment()->BecomesNetDead(ME));
    catch(map_objects(deep_inventory(ME)+all_inventory(environment()),
                      "BecomesNetDead", ME));
  }
  if (ME)
    ndead_location = environment();
}

static void ndead_move_me();

void heart_beat() {
  if (!ME)
    return;
  if (ndead_currently)
  {
    if (interactive(ME)) ndead_revive();
    else return;
  }
  else
    if (!(ndead_next_check--))
    {
      ndead_next_check=NETDEAD_CHECK_TIME;
      if (!interactive(ME))
        if (ndead_lasttime)
        {
          save_me(1);
          if (IS_LEARNER(ME))
          {
            quit();
            if (ME)
              remove();
            if (ME)
              destruct(ME);
            return;
          }
          ndead_move_me();
        }
        else
          ndead_lasttime=1;
    }
  if (ME && ndead_lasttime && interactive(ME))
    ndead_lasttime=0;
  life::heart_beat();
  combat::heart_beat();
}

/*
 * Identify ourself.
 * special temp handling for ghosts and i
 */
varargs int id(string str, int lvl)
{
  if (::id(str))
    return 1;
  if (Query(P_INVIS) && lvl < QueryProp(P_LEVEL))
    return 0;
  if (QueryProp(P_GHOST)&& str == "geist von "+ lower_case(QueryProp(P_NAME)))
    return 1;
  if (QueryProp(P_FROG) && str == "frosch") return 1;
  
  return 0;
}

static int set_ort(string str)
{
    if(!stringp(QueryProp(P_LOGON_PLACE)))
    {
	call_other("/p/daemon/townd", "???");
	SetProp(P_LOGON_PLACE,
	    "/p/daemon/townd"->get_information(this_player())[0]);
    }

    if(!(str=_unparsed_args()))
    {
	printf("Du kommst von '%s', wo immer das sein mag.\n",
	    QueryProp(P_LOGON_PLACE));
	return 1;
    }

    if(str == "loeschen" || str == "löschen")
    {
	write("Ok, persönliche Ortsangabe entfernt.\n");
	call_other("/p/daemon/townd", "???");
	SetProp(P_LOGON_PLACE,
	    "/p/daemon/townd"->get_information(this_player())[0]);
	return 1;
    }

    if(QueryProp(P_LEVEL) < 11)
    {
    	notify_fail("Sorry, dafür musst du schon Stufe 11 haben.\n");
    	return 0;
    }

    SetProp(P_LOGON_PLACE, str);
    printf("Ok, Du bist nun in '%s' daheim.\n", str);
    return 1;
}

static int set_email(string str)
{
  if(QueryProp(P_MAILADDR) == "none")
  { SetProp(P_MAILADDR, 0); }

  if (!(str=_unparsed_args()))
  {
    if(!stringp(QueryProp(P_MAILADDR)))
    {
	write("Du hast Dir keine E-Mail Adresse gesetzt!\n");
	return 1;
    }
    write("Deine offizielle E-Mail Adresse lautet: "+
          QueryProp(P_MAILADDR)+"\n");
    return 1;
  }

  if(str == "loeschen" || str == "löschen")
  {
    write("Ok, Du hast Deine offizielle E-Mail Adresse gelöscht.\n");
    SetProp(P_MAILADDR, "none");
  }
  else if(str=="sichtbar")
  {
    write("Alle Spieler können nun Deine E-Mail Addresse sehen!\n");
    SetProp(P_MAILADDR_INVIS,0);
  }
  else if(str=="unsichtbar")
  {
    write("Deine E-Mail Addresse ist nun nur für Erzmagier sichtbar!\n");
    SetProp(P_MAILADDR_INVIS,1);
  }
  else
  {
    write("Deine offizielle E-Mail Adresse wurde geaendert.\n");
    SetProp(P_MAILADDR, str);
  }
  return 1;
}

static int self_delete()
{
  write(
    "-----------------------------------------------\n"+
    "W I L L S T   D U   D A S   W I R K L I C H   ?\n"+
    "-----------------------------------------------\n"+
    "Wenn Du Dich selbstlöschst, ist Dein Charakter\n"+
    "UNWIDERRUFLICH verloren.  Es gibt  KEINE  Mög-\n"+
    "keit, ihn wiederherzustellen, Du müsstest wie-\n"+
    "der ganz von vorne anfangen!! Falls Du es immer\n"+
    "noch willst, gib jetzt Dein Passwort ein.\n"+
    "Alternativ verwende den Befehl 'spielpause', um\n"+
    "nur vorübergehend hier fernzubleiben.\n"+
    "-----------------------------------------------\n");
  write("Bitte das Passwort angeben: ");
  input_to("self_delete2",1);
  return 1;
}

int self_delete2(string str)
{
  int ret;
  ret="secure/master"->delete_player(str, getuid(PL));
  if (!ret)
  {
    write("Das hat nicht hingehauen (Gott sei Dank ....)\n");
    return 1;
  }
  if (PL->QueryProp(P_GUILD))
    catch(call_other(GUILD_DIR+PL->QueryProp(P_GUILD), "austreten"));

  PARENTMASTER->RemoveWiz(getuid(ME));

  if (QueryProp(P_DEADS) < 5) {
    write("Adios! Man sieht sich.\n");
    say(name(WER,1)+" hat sich gerade selbst zerstört.\n");
  }
  else {
    write( "\nTod kommt auf seinem weißen Pferd angeritten.\n"
+"Er steigt ab, baut sich drohend vor Dir auf und mustert Dich schadenfroh.\n"
+"\nTod sagt: ENDLICH! NUN KANN DIR AUCH LARS NICHT MEHR HELFEN!\n\n"
+"Tod holt weit mit seiner Sense aus. Mit großem Schwung lässt er sie auf\n"
+"Dich zusausen und dann...\n");
    say(name(WER,1)+" schied gerade endgültig von uns.\n");
  }
  remove(1);
  return 1;
}

static int spielpause(string str)
{
  int days;
  string foo;

  if (!str || sscanf(str, "%d tag%s", days, foo) != 2)
    return notify_fail("spielpause <x> tage\n"), 0;

  if (days < 0)
    days = -1;

  if (days > 0)
    printf("Du wirst Dich erst wieder am %s einloggen können!\n",
           dtime((time()/86400)*86400+days*86400)[0..16]);
  else if (days < 0)
    write( "Du wirst Dich auf unbestimmte Zeit nicht mehr einloggen können.\n"
          +"Wenn Du wieder spielen willst, musst Du Dich an einen Gott oder\n"
          +"Erzmagier wenden (mit einem Gast oder Mail von außen).\n" );
  else {
    write( "Die Spielpause ist aufgehoben.\n" );
    __MASTER_OBJECT__->TBanishName(getuid(this_object()), 0);
    return 1;
  }
  write( "Wenn Du das wirklich willst, gib jetzt 'ja' ein.\n]" );
  input_to( "spielpause2", 0, days);
  return 1;
}

static void spielpause2(string str, int days)
{
  if (str && (str == "ja" || str == "Ja" || str == "JA")) {
    __MASTER_OBJECT__->TBanishName(getuid(this_object()), days);
    write(
      "Ok, die Spielpause wird mit dem nächsten Ausloggen wirksam.\n"
      "Solltest Du es Dir bis dahin noch einmal überlegt haben, so kannst\n"
      "Du den Vorgang mit 'spielpause 0 tage' wieder rückgaengig machen.\n");
    return;
  }
  write("Vorgang wurde abgebrochen.\n" );
}

/*
 *-----------------------------------------------------------------
 * Aenderung das Passwords
 *-----------------------------------------------------------------
 */
static int change_password() {
  string verb;
  verb=query_verb();
  if (verb!="passwd"&&verb!="password"&&verb!="passwort")
    return 0;
  write("Bitte das ALTE Passwort angeben: ");
  input_to("change_password2",1);
  return 1;
}

static int change_password2(string str) {
  write("\n");
  if (!str) {
    str == "";
  }
  if (MASTER->update_password(str,str) == 0) {
    write("Falsches Passwort!\n");
    return 1;
  }
  passwold = str;
  write("Bitte das NEUE Passwort eingeben: ");
  input_to("change_password3",1);
  return 1;
}

static int change_password3(string str) {
  write("\n");
  if ((!str) || str == "") {
    write("Abgebrochen !\n");
    return 1;
  }
  if (lower_case(str)==getuid())
  {
    write("Das ist zu leicht zu erraten. Nimm ein anderes, bitte.\n");
    write("Bitte das NEUE Passwort eingeben: ");
    input_to("change_password3",1);
    return 1;
  }
  if (sizeof(str) < 6) {
    write("Das neue Passwort muss mindestens 6 Zeichen lang sein !\n");
    write("Bitte das NEUE Passwort eingeben: ");
    input_to("change_password3",1);
    return 1;
  }
  passw = str;
  write("Nochmal: ");
  input_to("change_password4",1);
  return 1;
}

static int change_password4(string str) {
  write("\n");
  if (!str || str != passw) {
    write("Das war verschieden! Passwort NICHT geändert.\n");
    return 1;
  }
  if (MASTER->update_password(passwold,passw))
    write("Passwort geändert.\n");
  return 1;
}

/*
 *-----------------------------------------------------------------
 * Rueckmeldungen von Spielern an Magier
 *-----------------------------------------------------------------
 */
static int bug(string str) {
  if (!(str=_unparsed_args())) {
    write( "Wie sieht der Fehler denn aus?\n]" );
    input_to("bug2");
    return 1;
  }
  smart_log("BUG",str);
  return 1;
}

static int bug2(string str) {
  if (!str || str == "") {
    write("Bug abgebrochen...\n");
    return 1;
  }
  smart_log("BUG",str);
  return 1;
}

static int typo(string str) {
  if (!(str=_unparsed_args())) {
    write( "Wo ist denn der Tippfehler?\n]" );
    input_to("typo2");
    return 1;
  }
  smart_log("TYPO",str);
  return 1;
}

static int typo2(string str) {
  if (!str || str == "") {
    write("Typo abgebrochen...\n");
    return 1;
  }
  smart_log("TYPO",str);
  return 1;
}

static int epidea(string str) {
  if (!(str=_unparsed_args())) {
    write( "Was für eine Idee für einen Forscherpunkt hast Du denn?\n]" );
    input_to("epidea2");
    return 1;
  }
  smart_log("EP",str);
  return 1;
}

static int epidea2(string str) {
  if (!str || str == "") {
    write("EP-Idee abgebrochen...\n");
    return 1;
  }
  smart_log("EP",str);
  return 1;
}

static int idea(string str) {
  if (!(str=_unparsed_args())) {
    write( "Was für eine Idee hast Du denn?\n]" );
    input_to("idea2");
    return 1;
  }
  smart_log("IDEA",str);
  return 1;
}
static int idea2(string str) {
  if (!str || str == "") {
    write("Idee abgebrochen...\n");
    return 1;
  }
  smart_log("IDEA",str);
  return 1;
}

static int detail(string str) {
  if (!(str=_unparsed_args())) {
    write( "Was für ein Detail fehlt denn?\n]" );
    input_to("detail2");
    return 1;
  }
  smart_log("DETAIL",str);
  return 1;
}
static int detail2(string str) {
  if (!str || str == "") {
    write("Detail abgebrochen...\n");
    return 1;
  }
  smart_log("DETAIL",str);
  return 1;
}
static int loben(string str) {
  if (!(str=_unparsed_args())) {
    write( "Was möchtest Du denn loben?\n]" );
    input_to("loben2");
    return 1;
   }
  smart_log("LOB",str);
  return 1;
}
static int loben2(string str) {
  if (!str || str == "") {
    write("Lob abgebrochen...\n");
    return 1;
  }
 smart_log("LOB",str);
 return 1;
}

/* smart_log()
 * Reportmeldung analysieren,
 * Abfrage, ob Spieler die Meldung senden moechte
 */
static void smart_log(string myname, string str)
{
  string obnam, rest, feedback;
  object obj;

  if (sscanf(str,"%s:%s",obnam,rest)==2 &&
      ((obj=present(lower_case(obnam),environment())) ||
       (obj=present(lower_case(obnam))) ) &&
      obj != environment(this_interactive()))
    str = rest;
  else {
    obj = QueryProp(P_REFERENCE_OBJECT);
    if (!obj || !present(obj))
      obj = environment(this_interactive());
  }

  feedback=myname+"\n"+str+"\ngeht an ";
  if (obj->id("Raum"))
    feedback+="den Raum: "+obj->QueryProp(P_INT_SHORT);
  else feedback+=obj->name(WEN,2);
  write(feedback+".\nMöchtest Du das? (j/n)\n");
  input_to("sendbugmsg", 0, myname, obj, str);
}

/* sendbugmsg()
 * Reportmeldung in repfile speichern
 * und Meldung an zustaendigen Magier
 */
static void sendbugmsg(string answ, string myname, object obj, string str)
{
  string creat, creat2, date, filext, filext2;
  string *strs;

  if (!answ || (answ != "j" && answ != "ja")) {
    write("Fehlermeldung verworfen.\n");
    return;
  }

  creat = MASTER->creator_file(obj);
  strs= MASTER->full_path_array(object_name(obj),0);
  if( strs[0]=="d" && myname!="EP" )
    creat2 = strs[1]+"/"+creat;
  if (creat == ROOTID)
    creat = "ROOT";
  else if( !creat || creat[0]==' ' )
    creat="STD";

  date = dtime(time())[5..<11];

  if(find_player(lowerstring(creat)))
    tell_object(find_player(lowerstring(creat)),
      "* Neue Meldung in Report-Datei: " + myname + " von "
      +capitalize(getuid(this_interactive())) + ".\n");

  switch(myname)
  {
    case "EP":     filext = ".ep";   break;
    case "DETAIL": filext = ".rep"; filext2 = ".det";  break;
    case "IDEA":   filext = ".rep"; filext2 = ".idea"; break;
    case "BUG":    filext = ".rep"; filext2 = ".bug";  break;
    case "TYPO":   filext = ".rep"; filext2 = ".typo"; break;
    case "LOB":    filext = ".rep"; filext2 = ".lob";  break;
    default:       filext = ".rep";  break;
  }
  creat = "report/" + creat + filext;
  // FP-Vorschlaege NICHT in Objekt-Logfiles
  if ((myname=="EP") || !obj->SmartLog(creat, myname, str, date)) {
    log_file( creat, myname + " von " + capitalize(getuid(this_interactive()))
              + " [" + object_name(obj) + "] (" + date + "):\n" );
    log_file( creat, str + "\n\n" );
    if(creat2)
    {
      creat2="report/"+creat2+filext2;
      log_file( creat2, capitalize(getuid(this_interactive()))
              + " [" + object_name(obj) + "] (" + date + "):\n" );
      log_file( creat2, str + "\n\n" );
    }
  }
  write(myname+" wurde gespeichert.\nVielen Dank für Deine Hilfe!\n");
}

static int _quit()
{
  int arg;
  object *obs;

  Set(P_LAST_LOGOUT,time());
  log_file("LEAVE", ME->Name()+" "+dtime(time())+"\n");
  if( sscanf( getuid(), "gast%d", arg) != 1 )
  {
    write("Speichere "+QueryProp(P_NAME)+".\n");
    save_me(0);

    obs=deep_inventory(ME);
    for(int i=sizeof(obs)-1;i>=0;i--)
      obs[i]->remove();
  }
  catch(VOICEMASTER->RemoveUser(ME));
  if (environment()) {
    catch(environment()->PlayerQuit(ME));
    catch(map_objects(deep_inventory(ME)+all_inventory(environment()),
                      "PlayerQuit", ME));
  }
  if (catch(remove())) destruct(ME);
  return 1;
}

int quit()
{
  return _quit();
}

static int new_quit()
{
  mixed x;
  if (IS_LEARNING(ME)) return quit();
  if (environment() && (x=environment()->QueryProp(P_NOQUIT))) {
    if (stringp(x))
      write(x);
    else
      write("Es ist an diesem Ort nicht möglich, das Spiel zu verlassen.\n");
    return 1;
  }
  if (ME->EnemyPresent())  // && (time()-QueryProp(P_LAST_QUIT)<600))
  {
    write ("Du bereitest Dich"+
        (find_call_out("quit") != -1 ? " erneut" : "")+
        " auf den Ausstieg aus dem Spiel vor.\n");
    remove_call_out("quit");
    call_out("quit", 10);
    return 1;
  }
  if (QueryProp(P_NEXT_SPELL_TIME)+30 > time())
  {
    write ("Du bist noch zu erschöpft von Deinem letzten Zauberspruch.\n");
    return 1;
  }
  return quit();
}

static string exhausted_text()
{
  int p;
  p = 100 * QueryProp(P_MV) / QueryProp(P_MAX_MV);
  switch (p)
  {
    case 0..5:   return "Du bist im Moment schwer erschöpft.";
    case 6..15:  return "Du bist ziemlich außer Atem.";
    case 16..30: return "Du bist etwas erschöpft.";
    default:     return "";
  }
}

static string traggewicht()
{
  int g;
  g = 100 * (QueryProp(P_TOTAL_WEIGHT) - QueryProp(P_WEIGHT) + 1 )
          / QueryProp(P_MAX_WEIGHT);
  switch (g)
  {
    case 0..64:
      return "";
    case 65..70:
      return "Langsam beginnt Dein Gepäck Dir Mühe zu machen.";
    case 71..80:
      return "Du schleppst schon sehr viel mit Dir herum.";
    case 81..90:
      return "Du bist schon schwer beladen.";
    case 91..95:
      return "Viel wirst Du nicht mehr tragen können.";
    case 96..99:
      return "Du bist beinahe schon überladen.";
    case 100:
    default:
      return "Du kannst absolut nichts mehr tragen.";
  }
}

static string vollgefressen()
{
  int g;
  g = 100 * (QueryProp(P_FOOD)) / (QueryProp(P_MAX_FOOD))+1;
  switch (g)
  {
    case 0..70:
      return "";
    case 71..80:
      return "Du fühlst Dich satt.";
    case 81..90:
      return "Hunger hast Du schon lange keinen mehr.";
    case 91..95:
      return "Dein Bauch bläht sich schon vom vielen Essen.";
    case 96..99:
      return "Dein Magen ist beinahe randvoll.";
    case 100:
    default:
      return "Du bist bis oben hin voll und kannst kaum noch was essen.";
  }
}

static string wasserbauch()
{
  int g;
  g = 100 * (QueryProp(P_DRINK)) / (QueryProp(P_MAX_DRINK))+1;
  switch (g)
  {
    case 0..70:
      return "";
    case 71..80:
      return "Du hast eigentlich schon jetzt keinen Durst mehr.";
    case 81..90:
      return "In Deinem Bauch gluckert es schon vom vielen Trinken.";
    case 91..95:
      return "Du bekommst langsam einen Wasserbauch vom vielen Trinken.";
    case 96..99:
      return "Du bekommst bald nichts Flüssiges mehr runter.";
    case 100:
    default:
      return "Du bist bis oben hin voll und kannst kaum noch was trinken.";
  }
}

static string betrunken()
{
  int g;
  g = (100 * (QueryProp(P_ALCOHOL)) / (QueryProp(P_MAX_ALCOHOL)+1));
  switch (g)
  {
    case 0..30:
      return "";
    case 31..33:
      return "Du bist bereits leicht angeheitert.";
    case 34..50:
      return "Du fühlst Dich mutig und stark - und etwas benommen.";
    case 51..75:
      return "Du hast das Gefühl als dreht sich die Welt um Dich.";
    case 76..90:
      return "Du lallst schon und hast Dich kaum noch unter Kontrolle.";
    case 91..999:
    default:
      return "Du bist sternhagelvoll. Dein Kopf dröhnt und alles wankt.";
  }
}


static varargs int score(string arg, int nolastline) {
  mixed ind;

  if (QueryProp(P_GHOST)) {
    write("Im ewigen Leben gibt es keine Punkte.\n");
    return 1;
  }
  ind=QueryProp(P_ATTRIBUTES);
  if (QueryProp(P_NO_ASCII_ART))
   write(sprintf(
    "Du bist: %s\n"
    "LP:        %3d von %3d\n"
    "MP:        %3d von %3d\n"
    "Stupse:          %5d\n"
    "QP:        %3d von %3d\n"
    "EP:          %9d\n",
    capitalize(implode(explode(short()||"", "\n"), "")),
    QueryProp(P_HP), QueryProp(P_MAX_HP),
    QueryProp(P_SP), QueryProp(P_MAX_SP),
    QueryProp(P_LP), QueryProp(P_QP), QM->QueryMaxQP(),
    QueryProp(P_XP)));
  else
   write(sprintf(
    "Du bist: %s\n"+
    "+------------------------------+------------------------------+\n"
    "| Erfahrungspunkte:  %9d | Lebenspunkte:      %3d (%3d) |\n"
    "| Abenteuerpunkte: %4d (%4d) | Magiepunkte:       %3d (%3d) |\n"
    "| Stufenpunkte:          %5d | Attributpunkte:           %2d |\n"
    "+------------------------------+------------------------------+\n"
    "| Intelligenz:        %8s | Ausdauer:           %8s |\n"
    "| Geschicklichkeit:   %8s | Kraft:              %8s |\n"+
    (nolastline ? "" :
    "+------------------------------+------------------------------+\n"
    ),
    capitalize(implode(explode(short()||"", "\n"), "")),
    QueryProp(P_XP), QueryProp(P_HP), QueryProp(P_MAX_HP),
    QueryProp(P_QP), QM->QueryMaxQP(),
    QueryProp(P_SP), QueryProp(P_MAX_SP),
    QueryProp(P_LP), QueryProp(P_AP),
    sprintf("%d (%+d)", ind[A_INT], QueryAttributeOffset(A_INT)),
    sprintf("%d (%+d)", ind[A_CON], QueryAttributeOffset(A_CON)),
    sprintf("%d (%+d)", ind[A_DEX], QueryAttributeOffset(A_DEX)),
    sprintf("%d (%+d)", ind[A_STR], QueryAttributeOffset(A_STR))
  ));
  return 1;
}

static int info(string arg) {
  string gift,stra,strb,strc,strd,stre,health;
  int i,val;
  mixed ind;
  mixed *enem1, *enem2, *inv;

  if (QueryProp(P_GHOST)) {
    write("Im ewigen Leben gibt es keine Punkte.\n");
    return 1;
  }

  score(arg, 1);

  if (val=QueryProp(P_POISON))
    gift="Du bist "+(val<4?"leicht vergiftet.":"vergiftet!");
  else
    gift="";
  stre=exhausted_text();
  stra=traggewicht();
  strb=vollgefressen();
  strc=wasserbauch();
  strd=betrunken();

  if (QueryProp(P_NO_ASCII_ART))
  {
    ind=QueryProp(P_ATTRIBUTES);
    health="";
    health+=((gift=="")? "" : gift+"\n");
    health+=((stre=="")? "" : stre+"\n");
    health+=((stra=="")? "" : stra+"\n");
    health+=((strb=="")? "" : strb+"\n");
    health+=((strc=="")? "" : strc+"\n");
    health+=((strd=="")? "" : strd+"\n");

    write(sprintf(
      "Attris:             %2d\n"
      "Int:            %2d + %d\n"
      "Ausd:           %2d + %d\n"
      "Gesch:          %2d + %d\n"
      "Kraft:          %2d + %d\n"
      "Level:              %2d\n"
      "Gildenlevel:        %2d\n"
      "Gilde: %:15s\n"
      "Vorsicht:          %3d%:-40s\n"
      "Ansehen: %s\n"
      "Alter: %s\n"
      "%s%s"+health,
      QueryProp(P_AP),
      ind[A_INT], QueryAttributeOffset(A_INT),
      ind[A_CON], QueryAttributeOffset(A_CON),
      ind[A_DEX], QueryAttributeOffset(A_DEX),
      ind[A_STR], QueryAttributeOffset(A_STR),
      QueryProp(P_LEVEL),
      QueryProp(P_GUILD_LEVEL) ? QueryProp(P_GUILD_LEVEL) : QueryProp(P_LEVEL),
      (QueryProp(P_GUILD) ?
        capitalize(QueryProp(P_GUILD)) :
        capitalize(QueryProp(P_DEFAULT_GUILD) || DEFAULT_GUILD)),
      QueryProp(P_WIMPY),
      (QueryProp(P_WIMPY_DIRECTION) ?
        "\nFluchtrichtung: "+to_string(QueryProp(P_WIMPY_DIRECTION)) : ""),
      al_to_title(QueryProp(P_ALIGN)),
      get_age(),
      (QueryProp(P_AWAY) ?
        sprintf("Du bist weg: %:-32s\n", QueryProp(P_AWAY)) : ""),
      break_string(Forschung(),78)
    ));
  }
  else
  {
    health="";
    health+=((gift=="")? "" : sprintf("| %-59s |\n", gift));
    health+=((stre=="")? "" : sprintf("| %-59s |\n", stre));
    health+=((stra=="")? "" : sprintf("| %-59s |\n", stra));
    health+=((strb=="")? "" : sprintf("| %-59s |\n", strb));
    health+=((strc=="")? "" : sprintf("| %-59s |\n", strc));
    health+=((strd=="")? "" : sprintf("| %-59s |\n", strd));
    write(sprintf(
      "+------------------------+-----+-----------+------------------+\n"
      "| Gilde: %:-15s | Gildenlevel: %2d | Spielerlevel: %2d |\n"
      "+------------------------+-----------------+------------------+\n"+
      health+
      "| Vorsicht-Modus: %3d%:-40s |\n"
      "| Dein Charakter ist %-40s |\n"
      "| Alter: %-52s |\n%s"
      "+-------------------------------------------------------------+\n"
      ,
      (QueryProp(P_GUILD) ?
        capitalize(QueryProp(P_GUILD)) :
        capitalize(QueryProp(P_DEFAULT_GUILD) || DEFAULT_GUILD)),
      QueryProp(P_GUILD_LEVEL) ? QueryProp(P_GUILD_LEVEL) : QueryProp(P_LEVEL),
      QueryProp(P_LEVEL),
      QueryProp(P_WIMPY),
      (QueryProp(P_WIMPY_DIRECTION) ?
        ", Fluchtrichtung: "+to_string(QueryProp(P_WIMPY_DIRECTION)) : ""),
      al_to_title(QueryProp(P_ALIGN))+".",
      get_age(),
      (QueryProp(P_AWAY) ?
        sprintf("| Du bist nicht ansprechbar: %:-32s |\n", QueryProp(P_AWAY))
        : "")
    ));

    ind = explode(break_string(Forschung(), 59), "\n");
    for (i=0;i<sizeof(ind);i++)
      write(sprintf("| %-59s |\n", to_string(ind[i])));
    write("+-------------------------------------------------------------+\n");
    
    if(this_player()->QueryProp(P_LOGON_PLACE) != 0)
      write(sprintf("| %-59s |\n", "Heimat: "+this_player()->QueryProp(P_LOGON_PLACE)));
    else
      write(sprintf("| %-59s |\n", "Heimat: Unbekannt"));
    write("+-------------------------------------------------------------+\n");

    if(tls_query_connection_state(this_player()))
    {
      write(sprintf("| %-59s |\n", "Verbindung TLS verschlüsselt."));
      write("+-------------------------------------------------------------+\n");
    }
    else
    {
      write(sprintf("| %-59s |\n", "Verbindung NICHT TLS verschlüsselt."));
      write("+-------------------------------------------------------------+\n");
    }
  }
  enem1=(({mixed})QueryEnemies())[0];
  if (!sizeof(enem1)) return 1;
  enem2=({});
  inv=all_inventory(environment(ME));
  for (i=0;i<sizeof(enem1);i++)
    if (member(inv, enem1[i])==-1) // Ist unser Feind und ist nicht hier
      enem2+=({enem1[i]});
  if (!sizeof(enem2)) return 1;

  if (QueryProp(P_NO_ASCII_ART))
  {
    write(break_string(
        "Du verfolgst "+CountUp( map_objects( enem2, "name", WEN ) )+".", 78));
    return 1;
  }
  ind = explode( break_string(
    "Du verfolgst "+CountUp( map_objects( enem2, "name", WEN ) )
    +".", 59), "\n");
  for (i=0;i<sizeof(ind);i++)
    write(sprintf("| %-59s |\n", to_string(ind[i])));
  write("+-------------------------------------------------------------+\n");
  return 1;
}

static string getmanpage(string dir, string page)
{
  string text, *syn;
  int i;

  if (dir[<1] != '/')
    dir += "/";

  if ((text=read_file(dir+page)) && sizeof(text))
    return text;

  if (text = read_file(dir+".synonym")) {
    syn = regexplode(text, "([ \t][ \t]*|\n)");
    if ((i=member(syn, page))!=-1)
      return read_file(dir+syn[i+2]);
  }
  return 0;
}

static int help(string str)
{
  string verb, rest, text;
  mixed found;

  found=0;
  text = "";
  if (str)
  {
    str = implode( explode(str, ".." ), "");
    if ( sscanf( str, "%s %s",verb,rest )==2 ) str = verb;

    if ((IS_LEARNER(PL)) ) {
      if (rest = getmanpage("/doc/w/",str)) {
	found = 1;
	text += rest;
      }
    }

    if ((IS_SEER(PL)) /*&& !found*/ ) {
      if (rest = getmanpage("/doc/s/",str)) {
	if (found)
	  text += "\n----------------\n";
	else
	  found = 1;
	text += rest;
      }
    }

    if (rest = getmanpage("/doc/helpdir/",str)) {
      if (found)
	text += "\n----------------\n";
      else
	found = 1;
      text += rest;
    }
    if (!found)
      text = "Dazu ist keine Hilfe verfügbar.\n";

   More(text,0);
   return 1;
  }
  if (IS_LEARNER(PL))
    text = read_file("/doc/magier_hilfe");
  else if (IS_SEER(PL))
    text = read_file("/doc/seher_hilfe");

  More(text + read_file("/doc/help"), 0);
  return 1;
}

static mixed PlayerMore()
{
  return More("/doc/help",1);
}

public mixed filter_who_options(string str)
{
  string par;
  string* opt;
  int i, j, len, res;

  opt = explode(str," "); len=sizeof(opt);
  res = 0;
  par = "";
  for (i=0; i<len; i++)
    switch (opt[i])
    {
      case "short":
      case "-kurz":
        res |= 1;
        par += " short";
        break;
      case "vert":
      case "-vertikal":
        res |= 3;
        break;
      case "alpha":
      case "long alpha":
      case "long":
      case "-alpha":
      case "-alphabetisch":
        res |= 4;
        break;
      default:
        if (opt[i][0] == '-')
          for (j=sizeof(opt[i])-1; j; j--)
            switch (opt[i][j])
            {
              case 'a': res|=4; par += " alpha"; break;
              case 'k': res|=1; par += " short"; break;
              case 'v': res|=3; par += " vert"; break;
            }
        else if (i)
          return ({ res, implode(opt[i..]," "), par[1..] });
        else
          return ({ 0, str, "" });
    }
  return ({ res, 0, par[1..] });
}

static int who(string str)
{
  int i, shrt;
  string res, ret, params, mud;
  mixed ans;

  if (str = _unparsed_args())
  {
    ans = filter_who_options(str);
    shrt = ans[0];
    str = ans[1];
    params = ans[2];
  }
  if (stringp(str) && sizeof(str))
  {
    ans = explode(str, " ");
    mud = ans[0];
    ans = ans[1..];
    if (mud == "mud" && sizeof(ans)) {      // Wird explizit nach einem
      mud = ans[0]; ans = ans[1..]; i=1;    // anderen Mud gefragt?
    } else i=0;
    params = implode(explode(params, " ") + ans - ({""}), " ");
    if (sizeof(INETD->expand_mud_name(mud)) || i)
    {
      if (ret=INETD->send_udp(mud,
			      ([ REQUEST: "who",
                                 SENDER:  getuid(ME),
                                 DATA:    params ]), 1 ))
	write(ret);
      else
	write("Anfrage abgeschickt.\n");
      return 1;
    }
  }

  // altes Format... Altlast -> raus.
  // if (str) i=(member(str,'o')>0); else i=0;

  res = implode("/obj/werliste"->QueryWhoListe(
      IS_LEARNER(ME) && QueryProp(P_WANTS_TO_LEARN),shrt,0,str), "\n");

  if (shrt & 1)
    ret = res + "\n\n";
  else
  {
    if (QueryProp(P_NO_ASCII_ART))
    {
      ret = sizeof(explode(res,"\n")-({""})) + " Teilnehmer\n" + res +
          (sizeof(res) ? "\n" : "");
    }
    else
    {
      ret = "---------------------------------------"+
            "---------------------------------------\n";
      ret = "Liste der Mitspieler vom " + dtime(time()) + " ("+
          sizeof(explode(res,"\n")-({""})) + " Teilnehmer):\n" + ret + res +
          (sizeof(res) ? "\n"+ret : "");
    }
  }
  More(ret,0);
  return 1;
}

static int kwho(string str)
{
  int shrt;
  mixed res;

  if(str) {
    res = filter_who_options(str);
    shrt = res[0];
    str = res[1];
  }
  More(implode( "/obj/werliste"->QueryWhoListe(IS_LEARNER(ME) && QueryProp(P_WANTS_TO_LEARN), shrt|1 ,0, str),"\n")+"\n\n",0);
  return 1;
}

static varargs int kkwho(string str) {
  object *obs;
  string *namen,res;
  int i;

  obs=filter_users(str);namen=({});
  if (IS_LEARNER(this_player())) {
    for (i=sizeof(obs)-1;i>=0;i--)
      if (environment(obs[i]))
	namen+=({capitalize(geteuid(obs[i]))});
  } else {
    for (i=sizeof(obs)-1;i>=0;i--)
      if (!obs[i]->QueryProp(P_INVIS) && environment(obs[i]))
	namen+=({capitalize(geteuid(obs[i]))});
  }
  namen=sort_array(namen,#'<); //')
  res="";
  for (i=sizeof(namen)-1;i>=0;i--) {
    res+=namen[i];
    if (i)
      res+=", ";
  }
  res+=".";
  write(break_string(res,75));

  return 1;
}

static int kill(string str) {
  object eob,wob;
  int i;

  if (QueryProp(P_GHOST))
  {
    write("Das kannst Du in Deinem immateriellen Zustand nicht.\n");
    return 1;
  }
  if (QueryGuest())
  {
    write("Du bist doch nur Gast hier.\n");
    return 1;
  }
  if (!str || str == "") {
    write("WEN willst Du töten?\n");
    return 1;
  }


  if(find_player(str))
  {

    object victim, caster;
  caster = this_player();
  victim = find_player(str);

  if (query_once_interactive(victim) && query_once_interactive(caster)
                                     && !(IS_LEARNER(caster)
                                          && IS_LEARNER(victim))
				     && !(call_other("/p/daemon/pkmaster","CheckForPkEnvironment",
				                               caster,victim)||
				          (call_other("/p/daemon/pkmaster","CheckForHitList",
					                      caster,victim)==2)))
  {
    printf("Du kannst hier keine Spieler angreifen!\n");
    return 1;
  }
}

  eob = present(lower_case(str),environment(PL));
  if (!eob || !living(eob)) {
    if (str=="alle") {
      object *lvs;
      lvs=filter(all_inventory(environment(PL)),
		       lambda(({'x}),({#'?,
			 ({#'query_once_interactive, 'x}), ({#'return, 0}),
			 ({#'living, 'x}), ({#'return, 1}),
				({#'return, 0})}))); // '})})));
      if (i=sizeof(lvs)) {
	for (i--;i>=0;i--)
          if ((!lvs[i]->QueryProp(P_NO_GLOBAL_ATTACK)) &&
              (!lvs[i]->QueryProp(P_FRIEND)))
            Kill(lvs[i]);
	return 1;
      }
    }
    write("Du siehst hier kein derartiges Wesen!\n");
    return 1;
  }
  if (eob == PL)
  {
    write("Selbstmord ist keine Lösung!\n");
    return 1;
  }
  /* Kill him */
  Kill(eob);
  return 1;
}

static int stop(string str)
{
  if (!str)
  {
    StopHuntingMode();
    write("Ok.\n");
    return 1;
  }
  if (!StopHuntFor(str)) write("So jemanden verfolgst Du nicht!\n");
  return 1;
}

int emote(string str,int genitiv)
{
  string *commands,message,verb;
  object living;
  int i,size;

  if (!(Query(P_CAN_FLAGS)&CAN_EMOTE)) return 0;
  if (query_verb()[0]==';'||query_verb()[0..5]=="gemote") genitiv=1;
  if (query_verb()[0]==':'||query_verb()[0]==';')
    verb=query_verb()[1..]+" ";
  else
    verb="";
  str=this_player()->_unparsed_args();
  commands=explode(verb+(str||""),"#");
  message=
    break_string((IS_SEER(ME) ? "" : ">")
      +capitalize(genitiv ? name(WESSEN) : name())
      +" "+commands[0],78);
  size=sizeof(commands);
  if(size>=3)
  {
    living=find_living(lower_case(commands[1]));
    if(!living || environment(living)!=environment())
    {
      write(capitalize(commands[1])+" sehe ich hier nicht!\n");
      return 1;
    }
    if (living != ME)
      tell_object(living,break_string((IS_SEER(this_player()) ? "" : ">")
				   +(genitiv ? this_player()->Name(WESSEN) :
					       this_player()->Name())
				   +" "+commands[2],78));
  }
  if(size>=4)
    write(break_string(commands[3],78));
  else
    write(message);
  tell_room(environment(),message,({this_object(),living}));
  return 1;
}

static int remote(string str, int flag)
{
  string verb, dest, message;
  string *exstr, *parts;
  int size, m;
  object destpl;

  if (!(Query(P_CAN_FLAGS)&CAN_REMOTE)) return 0;
  if(!IS_LEARNER(ME))
  {
//    if (QueryProp(P_QP) < QM->QueryMaxQP()) return 0;
      if ( !(this_player()->QueryQuest("Kopfgeldjäger"))) return 0;
  }
  if (!(str=_unparsed_args())||sizeof(exstr=explode(str," ")-({}))<=1)
  {
    write("Was willst Du zu wem `emoten`?\n");
    return 1;
  }
  dest = lower_case(exstr[0]);
  str = implode(exstr[1..]," ");
  verb = query_verb();
  if(!(destpl=find_player(match_living(dest)))||(destpl->QueryProp(P_INVIS)&&
				   !IS_LEARNER(ME)))
  {
    write("Einen solchen Spieler gibt es derzeit nicht.\n");
    return 1;
  }
  if (destpl == ME)
  {
    write("Bist Du wirklich so weit von Dir entfernt?\n");
    return 1;
  }
  if (environment() == environment(destpl))
  {
    write("Innerhalb eines Raumes solltest Du "+verb[1..]+
      " benutzen.\n");
    return 1;
  }
  message = (flag ? Name(WESSEN) : Name(WER));
  size = sizeof(parts = explode(" "+str, "#"));
  if ( size == 1 )
    message +=  parts[0] + " aus der Ferne.\n";
  else
    if ( size == 2)
      message += parts[0] + "aus der Ferne" + parts[1] + ".\n";
    else
    {
      write("Syntax: "+verb+" <name> <text>\n"+
            "oder:   "+verb+" <name> <text1> # <text2>\n");
      return 1;
    }
  if (m=destpl->Message(message, MSGFLAG_REMOTE) > 0)
    write(break_string(destpl->Name()+"->"+message, 78));
  else if (m == MESSAGE_CACHE)
    write(destpl->Name()+" ist gerade beschäftigt.\n");
  else if (m == MESSAGE_IGNORE_YOU)
    write(destpl->Name()+" ignoriert Dich.\n");
  else
    write(destpl->Name()+" ignoriert Deine Meldung.\n");
  return 1;
}

static int gemote(string str)
{
  return emote(str, 1);
}

static int rgemote(string str)
{
  return remote(str, 1);
}

static int lookfor(string str)
{
  string verb, dest, message, ans;
  string *exstr, *parts, *path;
  int m;
  object destpl, destenv;

  if (!((Query(P_CAN_FLAGS)&CAN_ERZ) || IS_LEARNER(ME))) return 0;

  if (!(str=_unparsed_args())||sizeof(exstr=explode(str," ")-({}))<1)
  {
    write("Nach wem willst Du sehen?\n");
    return 1;
  }
  dest = lower_case(exstr[0]);
  str = implode(exstr[1..]," ");
  verb = query_verb();
  if(!(destpl=find_player(match_living(dest)))||(destpl->QueryProp(P_INVIS)&&
				   !IS_LEARNER(ME)))
  {
    write("Einen solchen Spieler gibt es derzeit nicht.\n");
    return 1;
  }
  if (IS_LEARNER(destpl) || destpl->QueryProp(P_TESTPLAYER))
  {
    write("Du bekommst kein klares Bild.\n");
    return 1;
  }
  if (destpl == ME)
  {
    write("Du stehst wohl gerade etwas neben Dir?\n");
    return 1;
  }
  if (!(destenv=environment(destpl)))
  {
    write(destpl->Name()+" ist nicht zu finden.\n");
    return 1;
  }
  if (environment() == destenv)
  {
    write("Du stehst schon neben "+destpl->name()+".\n");
    return 1;
  }
  message = Name(WER);
  message += " versucht Dich zu finden.\n";
  path=explode(object_name(destenv),"/");
  switch(path[0])
  {
      case "d":
	  ans="in der Region "+capitalize(path[1]);
          break;
      case "gilden":
          if(path[1][0..5] == "files.") { path[1]=path[1][6..]; }
	  ans="in der "+capitalize(path[1])+"-Gilde";
          break;
      default:
	  ans="Weiß-Gott-Wo";
  }

  if (m=destpl->Message(message, MSGFLAG_REMOTE) > 0)
  {
    write(destpl->Name()+" befindet sich gerade "+ans+":\n\n"+
          process_string((destenv->QueryProp(P_INT_SHORT))||"Im Nirgendwo")+"\n\n");
  }
  else if (m == MESSAGE_CACHE)
    write(destpl->Name()+" ist gerade beschäftigt.\n");
  else if (m == MESSAGE_IGNORE_YOU)
    write(destpl->Name()+" ignoriert Dich.\n");
  else
    write(destpl->Name()+" ignoriert Deine Meldung.\n");
  return 1;
}

static void load_auto_objects(mapping map);

private void move_player_to_start(string where);
private void move_player_to_start2(string where);
private void move_player_to_start3(string where);
private void move_player_to_start4(string where);
static void get_race_description(string str);

// charerstellung optik fragen
static void randomgen();
static void bestaetigung();
static void wahl_hautfarbe(string str);
static void wahl_augenfarbe(string str);
static void wahl_frisur(string str);
static void wahl_haarfarbe(string str);
static void wahl_rassendetails(string str);
static void wahl_gewicht(string str);
static void wahl_groesse(string str);

// Aus der Shell
int _query_new_size();
int _query_new_weight();
string *_query_hair_colors();
string *_query_eye_colors();
string *_query_skin_colors();
string *_query_hair_styles();
string *_query_race_specials();
string *_query_race_specials_male();

static void updates_after_restore() {
  attributes::update_max_sp_and_hp();
}

varargs nomask int start_player(string str, string ip)
{
  int newflag;  /* could player be restored? */
  string str1, str2;

  str1=explode(object_name(previous_object()),"#")[0];
  if ((str1 != "/secure/login") &&
      (str1 != "/secure/master") &&
      (!geteuid(ME) ||
       geteuid(ME) != getuid(ME) ||
       capitalize(geteuid(ME)) != str ||
       geteuid(ME) != geteuid(previous_object())))
  {
    write("DIESER VERSUCH WAR ILLEGAL !!\n");
    destruct(ME);
    return 0;
  }
  /* autosave the player after 500 heartbeats */
  time_to_save = 500;
  my_ip=ip;
  /* try to restore player. If it doesn't exist, set the new flag */
  Set(P_WIMPY, 20);

  newflag = !restore_object("/"+SAVEPATH+lower_case(str)[0..0]+"/"
			    +lower_case(str));

  updates_after_restore();
  if(query_once_interactive(ME))
    Set(P_LAST_LOGIN,time());
//  Set(P_WANTS_TO_LEARN,1); // 1 sollte der default sein !!!
//  15.1.98: Nachdem die Flags fuer Magier ne Bedeutung haben,
//  sollte man das Ding hier auch wirklich mit dem urspruenglichen
//  Wert nehmen.  -Crea-
  Set(P_WANTS_TO_LEARN,PROTECTED,F_MODE_AS);
  if(!QueryProp(P_CAN_FLAGS))
  { SetProp(P_CAN_FLAGS, CAN_EMOTE); }
  if(IS_SEER(ME))
    SetProp(P_CAN_FLAGS,QueryProp(P_CAN_FLAGS)|CAN_REMOTE);
  Set(P_NAME, str);
  Set(P_NAME, SECURED, F_MODE_AS);
  if(!QueryProp(P_NEEDED_QP))
    SetProp(P_NEEDED_QP,QM->QueryNeededQP());
  Set(P_NEEDED_QP,-1,F_SET_METHOD);
  Set(P_NEEDED_QP,SAVE|SECURED,F_MODE_AS);
  time_to_save = age + 500;
  AddId(getuid());
  SetProp(P_AC,0);
  SetProp(P_WEAPON,0);
  /* Set some things which wont be set when all is OK */
  SetProp(P_MAX_HP, (QueryAttribute(A_CON))*8+42);
  SetProp(P_MAX_SP, (QueryAttribute(A_INT))*8+42);

//  call_out("delayed_notify",3,getuid(ME));

  /* If this is a new character, we call the adventurers guild to get
   * our first title !
   */
    
  if (!newflag) {
    move_player_to_start(0);
  }
  else {
    int arg;
    // if ( sscanf(getuid(),"gast%d",arg) == 1 )
    //   SetProp(P_TITLE,"durchstreift das "+MUDNAME+" ...");
    funcall(#'Set, P_LEVEL, -1 /*'*/);
    SetProp(P_ATTRIBUTES,([A_STR:1,A_CON:1,A_INT:1,A_DEX:1]));
    SetProp(P_HP, QueryProp(P_MAX_HP));
    if (IS_WIZARD(ME))
      set_is_wizard(ME,1);
    else
       set_is_wizard(ME,0);
    move_player_to_start(0);
  }
  catch(bb="/secure/bbmaster"->query_bb());
  
  return 0;
}

static void delayed_notify(string wer)
{
  int i;

  if (!find_player(wer))
  {
  /* Leider werden wenn die Login Sequenz zu lange dauert die Spieler
     nicht von Merlin gefunden... drum hier ein workaround, welches
     zumindest 5 Chancen gibt...
     Dass es hier zu Ueberschneidungen kommen kann ist mir bewusst,
     aber selbst fuer das sollten 5 Chancen reichen...
     zumindest bist mir was bessers eingefallen ist :)
     Leider aber weiss noch nicht genau warum der player noch nicht
     existiert (oder per find_player() gefunden wird), und darum hoffe
     ich mal das Problem so zu fixen...
     Querolin 2002.02.14
  */
    i++;
    if (i<5)
    {
      call_out("delayed_notify",1+i,wer);
      return;
  } }
  i=0;
//  if (find_player("querolin")) { tell_object(find_player("querolin"),
//      break_string(
//       wer+" in delayed_notify() in /std/player/base.c aufgerufen.\n",78) ); }

  catch("/secure/merlin"->notify_player_enter(wer));
}

private void move_player_to_start5(string where)
{
  int num;
  object gewand;
  object waffe;
  object fband;
  string err;
  string called_from_ip, called_from_ip_name;
  mixed start_place;
  string last_room;

  if (QueryProp(P_LEVEL) == -1)
  {
    int arg;
    funcall(#'Set, P_TITLE, 0 /*'*/);
    funcall(#'Set, P_LEVEL, 1 /*'*/);
    if (sscanf(getuid(),"gast%d",arg) == 1)
      SetProp(P_TITLE,"durchstreift das "+MUDNAME+"...");
    else
    {
      call_other("/obj/infomaster", "InfoMsg",
         capitalize(QueryProp(P_RACE))+" "+capitalize(QueryProp(P_NAME))
         +" wurde soeben geboren.");
    }
  }


  enable_commands();

  set_heart_beat(1);
  command::initialize();
  add_put_and_get_commands();
  add_soul_commands();
  add_action("UseSpell","",1);
  if (interactive(this_object()))
  {
    if (IS_WIZARD(this_object()))
      cat("/etc/WIZNEWS");
    else
    {
      cat("/etc/NEWS");
      if(QueryProp(P_NAME)[0..3] != "Gast")
      if(QueryProp(P_LAST_LOGOUT) == -1)
      {
	catch(fband=clone_object("/p/service/tiamak/obj/fband2"));
         if(!fband) {
	    write(break_string(
	    "Achtung, Freundschaftsband buggt. Bitte Magier informieren!\n"));
         }
         else
           fband->move(this_player(), M_SILENT);
        
          //dinge, die beim ersten login passieren sollen
      }
    }
    if (query_ip_number(ME)=="128.130.45.201")
      cat("/etc/NEWS.atpforest");
    if (QueryProp(P_INVIS) && !IS_WIZARD(PL)) SetProp(P_INVIS,0);
    if (num="secure/mailer"->FingerMail(getuid()))
      write("  __ /\n"
            " |__|   Du hast "+num+" neue"+(num==1 ? "n Brief" : " Briefe")+
            " im Postamt liegen.\n\n");
    catch(VOICEMASTER->RegisterUser(ME));
    // printf("%O:%O\n",ME,query_ip_name());
    called_from_ip = QueryProp(P_CALLED_FROM_IP);
    called_from_ip_name = QueryProp(P_CALLED_FROM_IP_NAME);
    if ((called_from_ip || called_from_ip_name) &&
        (query_ip_number() != called_from_ip))
      write(sprintf("Das letzte Mal kamst Du von %s (%s)\n",
        (called_from_ip_name || "unbekannt"),
        (called_from_ip || "unbekannt")));
    funcall(#'Set, P_CALLED_FROM_IP, query_ip_number()); //');
    funcall(#'Set, P_CALLED_FROM_IP_NAME, query_ip_name(ME)); //');
  }
  set_living_name(getuid());
  if (!stringp(default_home) || default_home=="")
    default_home="gilden/abenteurer";
  err=0;
  // Keine autom. Seherhaeuser im SL!!!
  /*
  if (IS_SEER(ME) && !IS_LEARNER(ME))
    start_place=HAUSVERWALTER->FindeHaus(getuid(ME));
  else
    start_place=0;
  if (!start_place)
    start_place=QueryProp(P_START_HOME);
  */
  start_place=QueryProp(P_START_HOME);
  last_room=QueryProp(P_LAST_ROOM);

  if (start_place && (objectp(start_place)
                      || (stringp(start_place) && start_place != "")))
  {
    if ((err=catch(move(start_place, M_GO|M_NOCHECK|M_SILENT|M_NO_SHOW)))
        || !environment())
    {
        if(last_room && last_room !="")
        {
          err=catch(move(last_room, M_GO|M_NOCHECK|M_SILENT|M_NO_SHOW));
        }
        else
          err=catch(move(default_home, M_GO|M_NOCHECK|M_SILENT|M_NO_SHOW));
    }
  }
  else
  {
    if(last_room && last_room !="")
        {
          err=catch(move(last_room, M_GO|M_NOCHECK|M_SILENT|M_NO_SHOW));
        }
        else
          err=catch(move(default_home,M_GO|M_NOCHECK|M_SILENT|M_NO_SHOW));
  }
  if (err) catch(move("gilden/abenteurer",M_GO|M_NOCHECK|M_SILENT|M_NO_SHOW));
  catch(ME->FinalSetup());
  load_auto_objects(autoload);
  AddMoney(ndead_lasttime);
  ndead_lasttime=0;
  if (!QueryProp(P_INVIS)) say(capitalize(name(WER)) + " betritt diese Welt.\n");
  else write("DU BIST UNSICHTBAR!\n\n");

  /*
  if (query_mud_port(this_player())==4711)
    write("...");
  */
  if (IS_WIZARD(getuid(ME)))
    set_is_wizard(ME,1);
  else
    set_is_wizard(ME,0);
  {
    string *list;
    object ob;
    int i,mag;

    list=QueryProp(P_WAITFOR);
    mag=IS_LEARNER(ME);
    for (i=sizeof(list)-1;i>=0;i--)
      if (!(ob=find_player(lower_case(list[i]))) ||
          (!mag && IS_LEARNER(ob) && ob->QueryProp(P_INVIS)))
	list=list[0..i-1]+list[i+1..];
    if (sizeof(list))
      write(break_string("Anwesende Erwartete: "+CountUp(list)+".", 78));
  }
  save_me(1);   // nach erfolgter Initialisierung abspeichern

  if (QueryGuest()) {
  object karte;
  catch(karte=clone_object("/d/gidhrog/gralkor/gidhrog-stadt/obj/karte"));
  if(karte) {
     karte->SetProp(P_NEVERDROP, 1);
     karte->SetProp(P_NODROP, "Behalte die Stadtkarte besser!\n");
     karte->move(ME, M_NOCHECK);
     write(break_string(
     "Du erhältst eine Stadtkarte von Gidhrog, Deinem Geburtsort.\n"
     "Viel Spaß beim Umsehen.\n"));
    } else {
     write(break_string("Hoppla, die Karte buggt, bitte mit "
     "\"-a Hallo, die Karte buggt!\" "
     "melden. Danke. "));
    } 

  }

  // Erst wenn Spieler erfolgreich initialisiert
  call_out("delayed_notify",3,getuid(ME));
}


/*
 * Checks for several player info:
 * - email address
 * - gender
 * - terminal
 */
private void move_player_to_start(string where) {
  string mailaddr;

  if (QueryProp(P_CARRIED_VALUE) > 0)
  {
    ndead_lasttime = QueryProp(P_CARRIED_VALUE);
    write("Du findest "+ndead_lasttime
         +" Münzen, die Du beim letzten Mal verloren hast.\n");
    SetProp(P_CARRIED_VALUE, 0);
  }
  if (file_time("/save/"+getuid()[0..0]+"/"+getuid()+".o") <
      last_reboot_time()) {
    SetProp(P_FOOD, 0);
    SetProp(P_DRINK, 0);
    SetProp(P_ALCOHOL, 0);
    SetProp(P_POISON, 0);
    SetProp(P_MV, QueryProp(P_MAX_MV));
  }
  if (!(mailaddr = Query(P_MAILADDR)) || mailaddr == "")
  {
    // write("Gib bitte Deine E-Mail Adresse an (oder 'none'): ");
    saved_where = where;
    // input_to("getmailaddr");
    // return;
    write(break_string(
      "Eine gültige E-Mail-Adresse erleichtert es erheblich, Dir "
      "ein neues Passwort setzen zu lassen, falls Du einmal Dein "
      "Passwort vergisst. (oder 'none')",78)); 
    input_to("getmailaddr",INPUT_PROMPT,
        "Gib bitte Deine E-Mail-Adresse an: " );
    return;
  }
  move_player_to_start2(where);
}
static void getmailaddr(string maddr) {
  if (!stringp(maddr) || !sizeof(maddr))
    maddr = "none";
  Set(P_MAILADDR, maddr);
  move_player_to_start2(saved_where);
}

private void move_player_to_start2(string where) {
  if( member(({MALE,FEMALE}), QueryProp(P_GENDER) ) == -1 )
  {
    write("Bist Du männlich oder weiblich: ");
    input_to("getgender", 0);
    return;
  }
  move_player_to_start3(where);
}

/*  This function is called using input_to, and sets the
 *  gender of this player.
 */
static void getgender(string gender_string) {
  string name;
  object gewand;

  gender_string = lower_case(gender_string);
  if (gender_string[0] == 'm' || gender_string == "männlich" || gender_string == "maennlich") {
    write("\nSeyd gegrüßt, mein Herr!\n");
    SetProp(P_GENDER,MALE);
  }
  else if (gender_string[0] == 'w' || gender_string == "weiblich") {
    write("\nSeyd gegrüßt, holde Maid!\n");
    SetProp(P_GENDER,FEMALE);
    SetProp(P_TITLE,"die hoffnungsvolle Anfängerin");
  }
  else {
    write("Wie? Was? Verstehe ich nicht!\n");
    write("Bist Du männlich oder weiblich? (tippe m or w): ");
    input_to("getgender");
    return;
  }

  move_player_to_start3(saved_where);
}

private void move_player_to_start3(string where) {
  if (!QueryProp(P_TTY) || QueryProp(P_TTY) == "none") {
    write("Wähle einen Terminaltyp (kann später mit <stty> geändert werden)\n");
    write("vt100, ansi, dumb [dumb] ");
    input_to("gettty", 0);
    return;
  }
  move_player_to_start4(where);
}

/*  This function is called using input_to, and sets the
 *  tty of this player.
 */
static void gettty(string ttystr) {
  if (!ttystr || ttystr == "") ttystr = "dumb";
  ttystr = lower_case(ttystr);
  if (ttystr == "vt100") {
    write("Dies sollte "+ANSI_BOLD+"fett"+ANSI_NORMAL+" sein.\n");
    SetProp(P_TTY, ttystr);
  }
  else if (ttystr == "ansi") {
    write("Dies sollte "+ANSI_RED+"rot"+ANSI_NORMAL+" sein.\n");
    SetProp(P_TTY, "ansi");
  }
  else if (ttystr == "dumb") {
    write("Ohje, öde! Besorg Dir ein besseres Terminal!\n");
    SetProp(P_TTY, "dumb");
  }
  else {
    write("Dieser Terminaltyp wird nicht unterstützt. Nimm bitte einen aus:\n");
    write("vt100, ansi, or dumb (default is dumb): ");
    input_to("gettty");
    return;
  }
  move_player_to_start4(saved_where);
} 

private void move_player_to_start4(string where)
{
  int avsize, avweight;

  if ( (this_object() != this_player()) ||
       ( QueryProp(P_WEIGHT) && QueryProp(P_SIZE) &&
         QueryProp(P_HAIR_COLOR) && QueryProp(P_EYE_COLOR) ) )
  {
    move_player_to_start5(where);
    return;
  }

  saved_where = where;
  wahl_groesse("");
}

static void randomgen()
{
  int avsize, avweight, size, weight;
  string *augenfarben, *haarfarben, *hautfarben, *haarstyle, *rassenspecial;

  avsize = QueryProp(P_AVERAGE_SIZE) || 180;
  avweight = QueryProp(P_AVERAGE_WEIGHT) || 70000;
  augenfarben = QueryProp("eye_colors") || ({ "blau", "braun", "schwarz" });
  haarfarben = QueryProp("hair_colors") || ({ "blond", "braun", "schwarz" });
  hautfarben = QueryProp("skin_colors") || ({ "weiß", "braun", "hellbraun" });
  haarstyle = (QueryProp("hair_styles")) ||
                ({"kurze Haare", "mittellange Haare", "lange Haare"});
  rassenspecial = (QueryProp("race_specials")) ||
                ({"hat keine Besonderheiten"});
  if(QueryProp(P_RACE) == "Mensch" && QueryProp(P_GENDER)== MALE)
    rassenspecial += (QueryProp("race_specials_male"));

  rassenspecial += ({"hat keine Besonderheiten"});  // Falls man nix mag

  size = QueryProp("new_size") ||  QueryProp(P_SIZE) || 180;
  weight = QueryProp("new_weight") || QueryProp(P_WEIGHT) || 70000;

  switch(1000 * size / avsize)
  {
    case 0..800: size_desc="ziemlich winzig"; break;
    case 801..850: size_desc="eher winzig"; break;
    case 851..900: size_desc="recht klein"; break;
    case 901..950: size_desc="eher klein"; break;
    case 951..1050: size_desc="mittelgroß"; break;
    case 1051..1100: size_desc="relativ groß"; break;
    case 1101..1150: size_desc="ziemlich groß"; break;
    case 1151..1200: size_desc="sehr groß"; break;
    default: size_desc="riesig"; break;
  }
  switch(1000 * weight / avweight)
  {
    case 0..800: weight_desc="ein Fliegengewicht"; break;
    case 801..850: weight_desc="sehr leicht"; break;
    case 851..900: weight_desc="recht leicht"; break;
    case 901..950: weight_desc="eher leicht"; break;
    case 951..1050: weight_desc="mittelschwer"; break;
    case 1051..1100: weight_desc="relativ schwer"; break;
    case 1101..1150: weight_desc="ziemlich schwer"; break;
    case 1151..1200: weight_desc="sehr schwer"; break;
    default: weight_desc="übergewichtig"; break;
  }
  eye_color = augenfarben[random(sizeof(augenfarben))];
  hair_color = haarfarben[random(sizeof(haarfarben))];
  skin_color = hautfarben[random(sizeof(hautfarben))];
  hair_style = haarstyle[random(sizeof(haarstyle))];
  race_special = rassenspecial[random(sizeof(rassenspecial))];
}

static void wahl_groesse(string str)
{
  int num, min, max, avsize;
  
  avsize = QueryProp(P_AVERAGE_SIZE) || 180;
  min = avsize - ((avsize/5)) - ((avsize)/10);
  max = avsize + ((avsize/5)) - ((avsize)/10);
  
  if(sscanf(str,"%d",num) && num>=min && num<=max)
  {
    size = num;
    switch(1000 * size / avsize)
    {
      case 0..800: size_desc="ziemlich winzig"; break;
      case 801..850: size_desc="eher winzig"; break;
      case 851..900: size_desc="recht klein"; break;
      case 901..950: size_desc="eher klein"; break;
      case 951..1050: size_desc="mittelgroß"; break;
      case 1051..1100: size_desc="relativ groß"; break;
      case 1101..1150: size_desc="ziemlich groß"; break;
      case 1151..1200: size_desc="sehr groß"; break;
      default: size_desc="riesig"; break;
    }   
    wahl_gewicht("");
  }
  else
  {
    write("\n\nGib deine Größe an ["+min+"cm - "+max+"cm]: ");
    input_to("wahl_groesse");
    return;
  }
}

static void wahl_gewicht(string str)
{
  int num, min, max, avweight;
  
  avweight = QueryProp(P_AVERAGE_WEIGHT) || 70000;
  min = ((avweight - avweight/5)-(avweight/10))/1000;
  max = ((avweight + avweight/5)-(avweight/10))/1000;
  
  if(sscanf(str,"%d",num) && num>=min && num<=max)
  {
    weight = num * 1000;
    switch(1000 * weight / avweight)
    {
      case 0..800: weight_desc="ein Fliegengewicht"; break;
      case 801..850: weight_desc="sehr leicht"; break;
      case 851..900: weight_desc="recht leicht"; break;
      case 901..950: weight_desc="eher leicht"; break;
      case 951..1050: weight_desc="mittelschwer"; break;
      case 1051..1100: weight_desc="relativ schwer"; break;
      case 1101..1150: weight_desc="ziemlich schwer"; break;
      case 1151..1200: weight_desc="sehr schwer"; break;
      default: weight_desc="übergewichtig"; break;
    }    
    wahl_hautfarbe("");
  }
  else
  {
    write("\n\nGib dein Gewicht an ["+min+"kg - "+max+"kg]: ");
    input_to("wahl_gewicht");
    return;
  }
}

static void wahl_hautfarbe(string str)
{
  int i, num;
  string *hautfarben;

  hautfarben = QueryProp("skin_colors") || ({ "weiß", "braun", "hellbraun" });

  if(sscanf(str,"%d",num) && num>=1 && num<=sizeof(hautfarben))
  {
    skin_color = hautfarben[num - 1];
    wahl_augenfarbe("");
  }
  else
  {
    write("\n\nWähle eine Hautfarbe:\n\n");
    for (i=0;i<sizeof(hautfarben);i++)
    {
      printf("%2d. %-30s   ",i+1,hautfarben[i]);
      if (i%2==0  && i<sizeof(hautfarben)-1) write("| "); else write("\n");
    }
    write("\nEingabe: ");
    input_to("wahl_hautfarbe");
    return;
  }
}

static void wahl_augenfarbe(string str)
{
  int i, num;
  string *augenfarben;

  augenfarben = QueryProp("eye_colors") || ({ "blau", "braun", "schwarz" });

  if(sscanf(str,"%d",num) && num>=1 && num<=sizeof(augenfarben))
  {
    eye_color = augenfarben[num - 1];
    wahl_frisur("");
  }
  else
  {
    write("\n\nWähle eine Augenfarbe:\n\n");
    for (i=0;i<sizeof(augenfarben);i++)
    {
      printf("%2d. %-30s   ",i+1,augenfarben[i]);
      if (i%2==0  && i<sizeof(augenfarben)-1) write("| "); else write("\n");
    }
    write("\nEingabe: ");
    input_to("wahl_augenfarbe");
    return;
  }
}

static void wahl_frisur(string str)
{
  int i, num;
  string *haarstyle;

  haarstyle = (QueryProp("hair_styles")) ||
                ({"kurze Haare", "mittellange Haare", "lange Haare"});

  if(sscanf(str,"%d",num) && num>=1 && num<=sizeof(haarstyle))
  {
    hair_style = haarstyle[num - 1];
    wahl_haarfarbe("");
  }
  else
  {
    write("\n\nWähle eine Frisur:\n\n");
    for (i=0;i<sizeof(haarstyle);i++)
    {
      printf("%2d. %-30s   ",i+1,haarstyle[i]);
      write("\n");
    }
    write("\nEingabe: ");
    input_to("wahl_frisur");
    return;
  }
}

static void wahl_haarfarbe(string str)
{
  int i, num;
  string *haarfarben;

  haarfarben = QueryProp("hair_colors") || ({ "blond", "braun", "schwarz" });

  if(sscanf(str,"%d",num) && num>=1 && num<=sizeof(haarfarben))
  {
    hair_color = haarfarben[num - 1];
    wahl_rassendetails("");
  }
  else
  {
    write("\n\nWähle eine Haarfarbe:\n\n");
    for (i=0;i<sizeof(haarfarben);i++)
    {
      printf("%2d. %-30s   ",i+1,haarfarben[i]);
      if (i%2==0  && i<sizeof(haarfarben)-1) write("| "); else write("\n");
    }
    write("\nEingabe: ");
    input_to("wahl_haarfarbe");
    return;
  }
}

static void wahl_rassendetails(string str)
{
  int i, num;
  string *rassenspecial;

  rassenspecial = (QueryProp("race_specials")) ||
                ({"hat keine Besonderheiten"});
  if(QueryProp(P_RACE) == "Mensch" && QueryProp(P_GENDER) == MALE)
    rassenspecial += (QueryProp("race_specials_male"));
                
  rassenspecial += ({"hat keine Besonderheiten"});  // Falls man nix mag

  if(sscanf(str,"%d",num) && num>=1 && num<=sizeof(rassenspecial))
  {
    race_special = rassenspecial[num - 1];
    bestaetigung();
  }
  else
  {
    write("\n\nWähle deine Rassendetails:\n\n");
    for (i=0;i<sizeof(rassenspecial);i++)
    {
      printf("%2d. %-30s   ",i+1,rassenspecial[i]);
      write("\n");
    } 
    write("\nEingabe: ");
    input_to("wahl_rassendetails");
    return;
  } 
}

static void bestaetigung()
{
  if (QueryGuest())
      get_race_description("ja");
  else
  {
    write(break_string("Du bist nun "+size_desc+", "+ weight_desc+", hast "+
          hair_style+" von "+hair_color+"er Farbe und "+eye_color+"e Augen."
          " Deine "+(this_object()->QueryProp(P_RACE)=="Ork"?"Fell":"Haut")+
          "farbe ist "+skin_color+".",78));
    write(break_string("Dein Charakter "+race_special+".",78));
    write("Bist Du damit zufrieden? (ja/nein) ");
    input_to("get_race_description");
  }
}

static void get_race_description(string str)
{
  if ( str == "ja" || str == "j" )
  {
    SetProp(P_WEIGHT, weight);
    Set(P_WEIGHT,SAVE|PROTECTED,F_MODE_AS);
    SetProp(P_SIZE, size);
    Set(P_SIZE,SAVE|PROTECTED,F_MODE_AS);
    SetProp(P_EYE_COLOR, eye_color);
    Set(P_EYE_COLOR,SAVE|PROTECTED,F_MODE_AS);
    SetProp(P_HAIR_COLOR, hair_color);
    Set(P_HAIR_COLOR,SAVE|PROTECTED,F_MODE_AS);
    SetProp(P_SKIN_COLOR, skin_color);
    Set(P_SKIN_COLOR,SAVE|PROTECTED,F_MODE_AS);
    SetProp(P_HAIR_STYLE, hair_style);
    Set(P_HAIR_STYLE,SAVE|PROTECTED,F_MODE_AS);
    SetProp(P_RACE_SPECIAL, race_special);
    Set(P_RACE_SPECIAL,SAVE|PROTECTED,F_MODE_AS);
    move_player_to_start5(saved_where);
  }
  else
    move_player_to_start4(saved_where);
}

nomask mixed query_real_name() {
  /* ACHTUNG !! DIES LFUN DARF NICHT ENTFERNT WERDEN !!! */
  /* Sie wird vom Gamedriver (zB bei F_ED) aufgerufen !! */
  return getuid();
}

/*
 * the wizard command review: show player moving messages
 */
int review() {
  string *msg;
  write(short());
  write("Deine Bewegungen werden wie folgt gemeldet:\n"+
        "mout:  "+Name(WER)+" "+(msg=explode(QueryProp(P_MSGOUT),"#"))[0]
       +" <Richtung>"+(sizeof(msg)>1 ? msg[1] : "")+".\n"+
        "min:   "+Name(WER)+" "+QueryProp(P_MSGIN)+".\n"+
        "mmout: "+Name(WER)+" "+QueryProp(P_MMSGOUT)+".\n"+
        "mmin:  "+Name(WER)+" "+QueryProp(P_MMSGIN)+".\n"+
        (IS_LEARNER(ME) ?
	 "cmsg:  "+Name(WER)+" "+QueryProp(P_CLONE_MSG)+".\n"+
	 "dmsg:  <Irgendetwas> "+QueryProp(P_DESTRUCT_MSG)+".\n"
	 : "")+
        "Wenn Du jemanden angreifst, sieht das so aus:\n"+
        Name(WER)+" greift Dich"+QueryProp(P_HANDS)[0]+" an.\n");
  return 1;
}

/*
 * set player moving messages
 */

static int setmin(string str)
{
  if( sizeof(_unparsed_args()) > 65 )
  {
    write("Das ist leider zu lange.\n");
    return 1;
  }
  SetProp(P_MSGIN, _unparsed_args()||"kommt an");
  write("min:   "+Name(WER)+" "+QueryProp(P_MSGIN)+".\n");
  return 1;
}

static int setmout(string str)
{
  string *msg;
  if( sizeof(_unparsed_args()) > 50 )
  {
    write("Das ist leider zu lange.\n");
    return 1;
  }

  if(sizeof(msg=explode((_unparsed_args()||"geht"),"#"))>2)
  {
    write("Du darfst nur einmal '#' für die Richtung angeben.\n");
    return 1;
  }
  if(sizeof(msg)>1)
  {
    if (msg[0]!="" && msg[0][<1]==' ') msg[0]=msg[0][0..<2];
    SetProp(P_MSGOUT, msg[0]+"#"+msg[1]);
  }
  else
    SetProp(P_MSGOUT, _unparsed_args()||"geht");
  write("mout:  "+Name(WER)+" "+(msg=explode(QueryProp(P_MSGOUT),"#"))[0]
       +" <Richtung>"+(sizeof(msg)>1 ? msg[1] : "")+".\n");
  return 1;
}

static int setmmin(string str)
{
  if( sizeof(_unparsed_args()) > 65 )
  {
    write("Das ist leider zu lange.\n");
    return 1;
  }
  SetProp(P_MMSGIN, _unparsed_args()||"erscheint");
  write("mmin:  "+Name(WER)+" "+QueryProp(P_MMSGIN)+".\n");
  return 1;
}

static int setmmout(string str)
{
  if( sizeof(_unparsed_args()) > 65 )
  {
    write("Das ist leider zu lange.\n");
    return 1;
  }
  SetProp(P_MMSGOUT, _unparsed_args()||"verschwindet");
  write("mmout: "+Name(WER)+" "+QueryProp(P_MMSGOUT)+".\n");
  return 1;
}

static int setcmsg(string str)
{
  SetProp(P_CLONE_MSG, _unparsed_args()||"zaubert etwas aus "
	  + QueryPossPronoun(MALE,WEM) + " Ärmel hervor");
  write("Ok.\n");
  return 1;
}

static int setdmsg(string str)
{
  SetProp(P_DESTRUCT_MSG, _unparsed_args()||"wird von " + name(WER,1)
	  + " zerstäubt");
  write("Ok.\n");
  return 1;
}

static int set_title(string str)
{
  string bonus;
  if( sizeof(_unparsed_args()) > 50 )
  {
    write("Das ist leider zu lange.\n");
    return 1;
  }
  if( sizeof(_unparsed_args()) + sizeof(QueryProp(P_PRESAY)) > 65  )
  {
    write("Zusammen mit Deinem Presay ist das leider zu lang.\n");
    return 1;
  }
  bonus = "";
  if (!(str=_unparsed_args()))
    str = "";
  else if( str[0..1]=="\\b" && str[2]==',')
  {
    bonus = "\b"; // ein backspace fuer ein komma ist ok! :-)
    str = str[2..];
  }
  if (str=="0")
    SetProp(P_TITLE, 0);	// (Gildentitel wird dann angezeigt)
  else
    SetProp(P_TITLE, bonus+str);
  write("Dein Titel lautet jetzt: \""+QueryProp(P_TITLE)+"\".\n");
  write(short());
  return 1;
}

#ifdef NEDIT_FOR_EXTRALOOK

static int done_extralook(string str)
{
  if (!str) {
    write("Ok, abgebrochen.\n");
    return 1;
  }
  if (str=="")
    SetProp(P_EXTRA_LOOK,0);
  else
    SetProp(P_EXTRA_LOOK,str);
  write("Ok.\n");
  return 1;
}

static int extralook(mixed str)
{
  if( str=="?" )
  {
    write( "Dein Extralook ist:\n"+QueryProp(P_EXTRA_LOOK) );
    return 1;
  }
  write("Bitte gib Deinen Extra-Look ein. Hilfe mit ~h. Beenden mit **:\n");
  nedit("done_extralook", QueryProp(P_EXTRA_LOOK));
  return 1;
}

#else

static int extra_input(string str, string look)
{
  if (str=="**" || str==".")
  {
    if (look=="")
      SetProp(P_EXTRA_LOOK,0);
    else
      SetProp(P_EXTRA_LOOK,look);
    write("Ok.\n");
    return 1;
  }
  else if (str=="~h") {
    write("Der Mini-Editor versteht folgende Befehle:\n"
          "~h       Hilfe\n"
          "~r       Zeige gesamten Text an\n"
          "~d       Lösche letzte Zeile\n"
          "~c       Lösche gesamten Text\n"
          "~q       Abbruch\n"
          ". / **   Sauber beenden\n");
  }
  else if (str=="~r") {
    write("Das hast Du bisher geschrieben:\n"+
        "--------------------------\n"+look+
        "--------------------------\n");
  }
  else if (str=="~c") {
    write("Gesamten Text gelöscht.\n");
    look="";
  }
  else if (str=="~d") {
    if (sizeof(look)) {
      write("Vorherige Zeile gelöscht.\n");
      look=implode(efun::explode(look,"\n")[0..<3],"\n");
    }
    else
      write("Du kannst nicht nichts löschen.\n");
    if (sizeof(look)) look+="\n";
  }
  else if (str=="~q") {
    write("Abbruch! Dein Extra-Look bleibt unverändert.\n");
    return 1;
  }
  else
    look=look+str+"\n";
  write("]");
  input_to("extra_input",0,look);
  return 1;
}

static int extralook(mixed str)
{
  string look;
  look=QueryProp(P_EXTRA_LOOK);
  if (str=="?")
  {
    if (!stringp(look)) write("Du hast keinen Extra-Look.\n");
    else write("Dein Extra-Look ist:\n"+look);
  }
  else if (str=="loeschen" || str=="löschen") {
    SetProp(P_EXTRA_LOOK,0);
    write("Ok, Extra-Look gelöscht.\n");
  } else {
    if (str=="neu") look="";
    write("Bitte gib Deinen Extra-Look ein. Hilfe mit ~h, beenden mit **:\n"+
        (stringp(look) && sizeof(look) ?
         "--------------------------\n"+look : "")+"]");
    input_to("extra_input", 0, look || "");
  }
  return 1;
}

#endif

static void calculate_value()
{
  int i, carried_value, value;
  mixed *inv;

  inv=deep_inventory(ME);
  carried_value=0;
  for (i=sizeof(inv)-1;i>=0;i--)
    if (!inv[i]->QueryProp(P_AUTOLOADOBJ))
      carried_value+=((value=inv[i]->QueryProp(P_VALUE))>1000?1000:value);
  SetProp(P_CARRIED_VALUE, carried_value);
}

/* Called by command 'save' */
int save_character() {
  save_me(1);
  write("Ok.\n");
  return 1;
}

int count_object_occurrences(string object_path)
{
    int count = 0;
    object *inventory = deep_inventory(ME);
    
    foreach (object item in inventory)
    {
        if (objectp(item) && explode(object_name(item),"#")[0] == object_path) {
            count++;
        }
    }
    
    return count;
}

void save_me(mixed value_items)
{
  object *obs;
  mixed val;
  string obnam, dummy;
  int i;

  if( getuid()[0..3]=="gast" )
    return;
  autoload=([]);
  obs=deep_inventory(ME);
  for( i=sizeof(obs)-1;i>=0;i--)
  {
    // if( val = obs[i]->QueryProp( P_AUTOLOADOBJ ) )
    // {
      if( sscanf( object_name( obs[i] ), "%s#%s", obnam, dummy )==2  )
      {
          if(obs[i]->QueryProp( P_AUTOLOADOBJ ))
            val = obs[i]->QueryProp( P_AUTOLOADOBJ );
          else
            val = count_object_occurrences(obnam);
        
          if (obnam=="/obj/money")
            autoload[obnam] += val;
          else
            autoload += ([obnam:val]);
      }
    // }
  }
  if (value_items)
    calculate_value();
  else
    SetProp(P_CARRIED_VALUE, 0);
  // Set(P_LAST_LOGOUT,time());
  catch(call_other(GUILD_DIR+QueryProp(P_GUILD), "GuildRating", this_object()));
  save_object("/"+SAVEPATH+extract(getuid(),0,0)+"/" + getuid());
}

static void load_auto_object(string file,mixed data)
{
  object ob;

  ob=find_object(file);
  // if (!ob && file_size("/"+file+".c")<=0)  // Das verhinderte VC-Autoloader
  //   return;

  if (!ob && catch(call_other(file,"???")))
    return;
  if(file == "/obj/money")
  {
    if (catch(ob = clone_object(file)))
      return;
    if (catch(ob->move(ME,M_NOCHECK)))
      return;
    if (catch(ob->SetProp(P_AUTOLOADOBJ, data)))
      return;
  }
  else
  {
    if(intp(data))
    {
      for(int i=1;i<=data;i++)
      {
        if (catch(ob = clone_object(file)))
          return;
        if (catch(ob->move(ME,M_NOCHECK)))
          return;
        if (catch(ob->SetProp(P_AUTOLOADOBJ, data)))
          return;
      }
    }
    else
    {
      if (catch(ob = clone_object(file)))
        return;
      if (catch(ob->move(ME,M_NOCHECK)))
        return;
      if (catch(ob->SetProp(P_AUTOLOADOBJ, data)))
        return;
    }
  }
}

static void load_auto_objects(mapping map)
{
  mapping to_load;

  if (!mappingp(map)||!sizeof(map)) return;
  to_load=copy(map);
  walk_mapping(to_load,#'load_auto_object); //');
}

/*
 * al_to_title: Make the numeric alignment value into a string

string al_to_title(mixed a)
{
  if (!intp(a)) return "unbekannt";
  if (a >= KILL_NEUTRAL_ALIGNMENT * 1000)
    return "scheinheilig";
  if (a >= KILL_NEUTRAL_ALIGNMENT * 100)
    return "heilig";
  if (a > KILL_NEUTRAL_ALIGNMENT * 20)
    return "gut";
  if (a > KILL_NEUTRAL_ALIGNMENT * 4)
    return "nett";
  if (a > - KILL_NEUTRAL_ALIGNMENT * 4)
    return "neutral";
  if (a > - KILL_NEUTRAL_ALIGNMENT * 20)
    return "frech";
  if (a > - KILL_NEUTRAL_ALIGNMENT * 100)
    return "boese";
  return "satanisch";
}
*/
string al_to_title(mixed a)
{
  if(!intp(a)) return "unbekannt";

  /* Korrektur ... al_to_title() wird eh oft genug aufgerufen */
  if(a < -1000) { SetProp(P_ALIGN, -1000); }
  if(a > 1000) { SetProp(P_ALIGN, 1000); }

  switch(a)
  {
	case -1000..-951: return "satanisch";
	case  -950..-851: return "dämonisch";
	case  -850..-751: return "teuflisch";
	case  -750..-651: return "grausam";
	case  -650..-551: return "feindselig";
	case  -550..-451: return "böse";
	case  -450..-351: return "mies";
	case  -350..-251: return "gemein";
	case  -250..-151: return "durchtrieben";
	case  -150..-51:  return "verschlagen";
	case   -50..49:   return "neutral";
	case    50..149:  return "umgänglich";
	case   150..249:  return "nett";
	case   250..349:  return "freundlich";
	case   350..449:  return "aufgeschlossen";
	case   450..549:  return "liebenswürdig";
	case   550..649:  return "gut";
	case   650..749:  return "nobel";
	case   750..849:  return "ehrenhaft";
	case   850..949:  return "edel";
	case   950..1000: return "heilig";
	default: return "unergründlich";
  }
}

static int toggle_whimpy_dir(string str) {
  SetProp(P_WIMPY_DIRECTION,str=_unparsed_args()||str);
  if (str)
    printf("Ok, Fluchtrichtung %O.\n",str);
  else
    printf("Ok, bevorzugte Fluchtrichtung deaktiviert.\n");
  return 1;
}

static int toggle_whimpy(string str)
{
  int i;

 /*
    Durch das Folgende Konstrukt ist es Spielern moeglich die Vorsicht
    auch in Prozent einzugeben.
                                 05.01.2001 18:20 Querolin
 */
  if (str)
  {
   if ( (extract(str,1,1)=="%") ||
        (extract(str,2,2)=="%") ||
        (extract(str,3,3)=="%") ||
        (extract(str,4,4)=="%") )
   {
     i=to_int(str);
     if ( (i<0) || (i>100) )
     {
       write("Eingabe muss zwischen 0% und 100% liegen.\n");
       return 1;
     }
     i=((QueryProp(P_MAX_HP)*i) /100);
     str=to_string(i);
   }
   /*
    Wenn eine Eingabe in Prozent erfolgt ist wird hier der umgerechnete
    Wert wieder normal als String weiterverarbeitet ;)
    Net ganz fein aber geht...
   */
  }

  if(!str || str=="" || (sscanf(str,"%d",i)<0))
  {
    write("Syntax: vorsicht <hp>, wobei gilt: 0 <= hp < "+QueryProp(P_MAX_HP)+
        "\nAktuelle Einstellung: "+
        (QueryProp(P_WIMPY) || "Prinz Eisenherz-Modus")+"\n");
    return 1;
  }
  if ((i<0) || (i>=QueryProp(P_MAX_HP)))
  {
    write("Der Wert ist nicht erlaubt.\n");
    return 1;
  }
  if(!i) write("Prinz Eisenherz-Modus.\n");
  else write("Vorsicht-Modus ("+i+")\n");
  SetProp(P_WIMPY,i);
  return 1;
}

/*
static int where(object ob)
{
  string ipname;
  TW ("----- where() aufgerufen\n");
  ipname = query_ip_name(ob);
  return (int)(country(lower_case(ipname)));
}
*/

nomask int query_prevent_shadow(object obj)
{
  string what;
  int    dummy;

  if (obj){
    what=object_name(obj);
    if (what && what != "" &&
        sscanf(what,"/std/player/shadows/%s#%d",what,dummy)==2)
      return 0;
    // Disabled (security)
    // if (getuid()==getuid(obj) && getuid()==geteuid(obj))
    //   return 0;
  }
  return 1;
}

/*
 * Force the monster to do a command. The force_us() function isn't
 * always good, because it checks the level of the caller, and this function
 * can be called by a room.
 */
int command_me(string cmd)
{
  if (IS_LEARNER(ME))
  {
    if (!this_interactive() || !previous_object())
      return 0;
    if( geteuid(ME)!=geteuid(this_interactive())
        || geteuid(ME)!=geteuid(previous_object()) )
    {
      if( query_wiz_level(ME)<query_wiz_level(previous_object()))
	tell_object(ME,previous_object()->name()+" zwingt Dich zu: "
		    + cmd + ".\n");
      else
      {
        tell_object(ME,previous_object()->name()
                    + " versucht, Dich zu " + cmd + " zu zwingen.\n" );
        return 0;
      }
    }
  }
  return command(cmd);
}

static int uhrzeit()
{
  write(dtime(time())+".\n");
  return 1;
}

string SetDefaultHome(string str)
{
  return default_home=str;
}

string QueryDefaultHome(string str)
{
 return default_home;
}

void SetPrayRoom(string str)
{
  pray_room=str;
}

string QueryPrayRoom()
{
  if (pray_room) return pray_room;
  else return default_home;
}

int _filterattr_con(int i)
{
  if (i<A_MIN) i=A_MIN;
  if (i>A_MAX) i=A_MAX;
  SetProp(P_MAX_HP,42+8*i);
  return i;
}

int _filterattr_int(int i)
{
  if (i<A_MIN) i=A_MIN;
  if (i>A_MAX) i=A_MAX;
  SetProp(P_MAX_SP,42+8*i);
  return i;
}

void _restart_beat()
{
  tell_object(ME,
    "Der GameDriver teilt Dir mit: Dein Herzschlag hat wieder eingesetzt.\n");
  set_heart_beat(1);
}

static int weg(string str)
{
  if (!(str=_unparsed_args()))
  {
    printf("Du bist nicht%s als abwesend gekennzeichnet.\n",
           QueryProp(P_AWAY) ? " mehr" : "");
    SetProp(P_AWAY, 0);
    return 1;
  }
  write("Du bist jetzt als abwesend gekennzeichnet.\n");
  SetProp(P_AWAY, str);
  return 1;
}

static void ndead_revive()
{
  string fname;

  set_heart_beat(1);
  ndead_next_check=NETDEAD_CHECK_TIME;
  ndead_currently=0;
  ndead_lasttime=0;
  if (!objectp(ndead_location))
    if (sizeof(explode(ndead_l_filename,"#"))==1)
    {
      catch(call_other(ndead_l_filename,"???"));
      ndead_location=find_object(ndead_l_filename);
    } else
    {
      ndead_location=find_object(ndead_l_filename);
      if (!ndead_location && env_ndead_info)
      {
        fname=explode(ndead_l_filename,"#")[0];
        call_other(fname,"?");
        ndead_location=(find_object(fname)->SetProp(P_NETDEAD_INFO,
                                                    env_ndead_info));
      }
    }
  if (!ndead_location || catch(move(ndead_location,M_GO|M_NOCHECK)))
  {
    move("gilden/abenteurer",M_GO|M_NOCHECK|M_SILENT);
    catch(move(ndead_location,M_GO|M_NOCHECK,"zurück ins Leben"));
    ndead_location = environment();
  }
  catch(ndead_location->BecomesNetAlive(ME));
  catch(map_objects(all_inventory(ndead_location) + deep_inventory(ME),
                    "BecomesNetAlive", ME));

  ndead_location=0;
  ndead_l_filename=0;
  env_ndead_info=0;
}

static void ndead_move_me()
{
  set_heart_beat(0);
  stop_heart_beats();
  if (QueryGuest())
  {
    quit();
    if (ME)
      remove();
    if (ME)
      destruct(ME);
    return;
  }
  ndead_next_check=NETDEAD_CHECK_TIME;
  ndead_currently=1;
  ndead_lasttime=0;
  ndead_location=environment();
  if (objectp(ndead_location))
    ndead_l_filename=object_name(ndead_location);
  if (sizeof(explode(object_name(environment()),"#")))
    env_ndead_info=environment()->QueryProp(P_NETDEAD_INFO);
  else
    env_ndead_info=0;
  disable_commands();
  move(NETDEAD_ROOM,M_GO|M_NO_ATTACK|M_NOCHECK,"ins Reich der Netztoten");
}

int QueryGuest()
{
  string dummy;
  return sscanf(getuid(),"gast%d",dummy);
}

int disconnect(string str)
{
  string verb;

  verb=query_verb();
  if (!verb)
    verb="AUTOCALL";
  if (verb[0..5]=="schlaf" && str!="ein")
  {
    notify_fail(
"\"schlafe ein\" beendet Deine Verbindung mit "+MUDNAME+". Du behältst Deine"
"\nSachen, aber Vorsicht: Wenn "+MUDNAME+" abstürzt, während Du weg bist,"
"\nverlieren z.B. MI ihre Verzauberung, als wenn Du Deine Verbindung mit \"ende\""
"\nbeendet hättest.\n");
    return 0;
  }
  if (IS_LEARNER(this_object()))
    return quit();
  write(
"Dieser Befehl beendet Deine Verbindung mit "+MUDNAME+". Du behältst Deine"
"\nSachen, aber Vorsicht: Wenn "+MUDNAME+" abstürzt, während Du weg bist,"
"\nverlieren z.B. MI ihre Verzauberung, als wenn Du Deine Verbindung mit \"ende\""
"\nbeendet hättest.\n");
  if (is_clone(environment()) && !environment()->QueryProp(P_NETDEAD_INFO))
    write("\nACHTUNG: Wenn Du hier länger als ETWA 10 Minuten schläfst, kommst Du nicht\nan diesen Ort zurück !\n");
  say(name(WER)+" hat gerade die Verbindung zum "+MUDNAME+" gekappt.\n");

  SetProp(P_AWAY, 0);
  log_file("LEAVE", ME->Name()+" "+dtime(time())+"\n");
  remove_interactive(ME);
  return 1;
}

static int finger (string str)
{
  string ret;
  mixed xname;

  if (!str||str==""||sizeof(explode(str," ")-({"-n","-p"}))>1)
  {
    write("finger <spielername> oder finger <spielername@mudname>\n"+
      "Bitte nur den reinen Spielernamen verwenden, keine Namensvorsätze oder Titel.\n");
    return 1;
  }
  xname=explode(str,"@");
  if(sizeof(xname)==2)
  {
    if (ret=INETD->send_udp(xname[1],([
                   REQUEST: "finger",
                   SENDER: getuid(ME),
                   DATA: (explode(xname[0]," ")-({"-n","-p"}))[0]
                   ]), 1))
            write(ret);
        else
      write("Anfrage abgeschickt.\n");
    return 1;
  }
  FINGERMASTER->finger_single(str,1);
  return 1;
}

string lalign(string str, int wid)
{
  return (str+"                                                   "+
      "                                                   ")[0..wid-1];
}

#define MUDS_BAR "\
-----------------------------------------------------------------------------"

private void format(mixed mud, mixed hosts, string output)
{
  output += lalign(hosts[mud][HOST_NAME], 20) + "  " +
        (hosts[mud][HOST_STATUS] ?
           hosts[mud][HOST_STATUS] > 0 ?
             "ONLINE   " + lalign(hosts[mud][HOST_IP], 15) + "   " + (hosts[mud][HOST_ENCODING]?lalign(hosts[mud][HOST_ENCODING],7):lalign("N/A",7)) + "    " + strftime("%Y/%m/%d, %H:%M",hosts[mud][HOST_STATUS]) :
             "OFFLINE  " + lalign(hosts[mud][HOST_IP], 15) + "   " + lalign("N/A",7) + "    " + strftime("%Y/%m/%d, %H:%M",-hosts[mud][HOST_STATUS])
         : "N/A") +"\n";
}

static int muds() {
  mapping hosts;
  int i;
  mixed muds, output;

  output = lalign("MUD", 20) + "  Status   "+lalign("IP", 15)+"   Encoding   Last access";
  output += "\n" + MUDS_BAR + "\n";
  muds = sort_array(m_indices(hosts = INETD->query("hosts")),#'>);
  map(muds, #'format, hosts, &output);
  More(output);
  return 1;
}

// **** local property methods

static int _set_level(int i)
{
  if (intp(i) && (i >= 1))
    return Set(P_LEVEL, i);
  else
    return -1;
}

static int _set_invis(int a)
{
  if(IS_DOMAINMEMBER(this_player()))
  {
	return Set(P_INVIS, intp(a) ? a : !Query(P_INVIS));
  }

  return 0;
}

/* sets the terminal type */
/* note: support vt100 (b/w), ansi (color), dumb (none) */
static string _set_tty(string str) {
  if(str != "dumb" && str != "vt100" && str != "ansi")
    return Query(P_TTY);
  return Set(P_TTY, str);
}

static int stty(string str)
{
  if (str!=TTY_DUMB&&str!=TTY_VT100&&str!=TTY_ANSI&&str!="reset"&&str!="truecolor")
  {
    write("Syntax: stty dumb|vt100|ansi oder reset\n");
    write("Wenn du 'ansi' verwendest kannst du mit 'stty truecolor' den ANSI True Color Modus ein- oder ausschalten.\n");
  }
  if(str == "reset") {
    printf("\e[30;47m\e[0mDieser Text sollte lesbar sein!\n");
    return 1;
  }

  if(str == "truecolor") {
    if(Query(P_TTY) != "ansi") {
      write("ANSI True Color benötigt 'stty ansi' aktiviert.");
      return 1;
    }
    SetProp(P_ANSI_TRUE_COLOR, !QueryProp(P_ANSI_TRUE_COLOR));
    int tc = QueryProp(P_ANSI_TRUE_COLOR);
    write("ANSI True Color: "+(tc?"ein":"aus")+"\n");
    if(tc) {
      write("\nTrueColor Support aktiviert:\n" + ANSI_RAINBOW + "\n" + "Wenn du die Farben sehen kannst, dann unterstützt dein Terminal TrueColor.\n");
    }
    return 1;
  }

  write("TTY steht jetzt auf "+SetProp(P_TTY,str)+".\n");
  if(str == "ansi" || str == "vt100") {
      printf("Terminal Test:\n");
      printf("VT100: \e[1mfett\e[0m \e[4munterstrichen\e[0m "+
            "\e[5mblinkend\e[0m \e[7minvers\e[0m\n");
      if(str == "ansi") {
          int fg, bg;
          printf("ANSI Farben und VT100 Attribute:\n");
          for(fg = 30; fg <= 37; fg++) {
              for(bg = 40; bg <= 47; bg++) {
                  printf("\e[%d;%dm\e[1m@\e[0m", fg, bg);
                  printf("\e[%d;%dm\e[4m@\e[0m", fg, bg);
                  printf("\e[%d;%dm\e[5m@\e[0m", fg, bg);
                  printf("\e[%d;%dm\e[7m@\e[0m", fg, bg);
              }
              printf("\n");
          }
          printf("Sollte dieser Text hier nicht richtig lesbar\n"
                "sein, benutze das Kommando stty reset!\n");
      } else {
        SetProp(P_ANSI_TRUE_COLOR, 0); // immer ausschalten, wenn kein ansi terminal
      }
  }
  return 1;
}

/* Gibt an this_player ein farbige Liste der verfuegbaren Farben aus.
 * Ist 'channels' ungleich 0, werden die Ebenenfarben ausgegeben,
 * welche leider nicht identisch sind.
 * */
varargs void farbliste(int channels) {
  if (QueryProp(P_TTY)=="ansi") {
    write(
      "Mögliche Farben"+(channels?" für Ebenen":"")+":\n"+
      " " +ANSI_RED+"rot"+ANSI_PLAIN+
      ", "+ANSI_LRED+"hellrot"+ANSI_PLAIN+
      ", "+ANSI_GREEN+"grün"+ANSI_PLAIN+
      ", "+ANSI_LGREEN+"hellgrün"+ANSI_PLAIN+
      ", "+ANSI_YELLOW+"braun"+ANSI_PLAIN+
      ", "+ANSI_LYELLOW+"gelb"+ANSI_PLAIN+",\n"
      " " +ANSI_BLUE+"blau"+ANSI_PLAIN+
      ", "+ANSI_LBLUE+"hellblau"+ANSI_PLAIN+
      ", "+ANSI_PURPLE+"lila"+ANSI_PLAIN+
      ", "+ANSI_LPURPLE+"helllila"+ANSI_PLAIN+
      ", "+ANSI_CYAN+"cyan"+ANSI_PLAIN+
      ", "+ANSI_LCYAN+"hellcyan"+ANSI_PLAIN+",\n"+
      (channels?"":" " +ANSI_WHITE+"weiß"+ANSI_PLAIN+
      ", "+ANSI_WHITE+"(schwarz)"+ANSI_PLAIN+",")+
      " " +ANSI_GREY+"grau"+ANSI_PLAIN+", normal und"
      " " +ANSI_BOLD+"fett"+ANSI_PLAIN+".\n");
   } else {
     write(
     "Mögliche Farben"+(channels?" für Ebenen":"")+":\n"
     " rot, hellrot, grün, hellgrün, braun, gelb,\n"
     " blau, hellblau, lila, helllila, cyan, "+
     "hellcyan,\n"+(channels?"":"weiß, schwarz,")+
     " grau, normal und fett.\n"
     "Um Farben zu sehen, \"stty ansi\" einstellen!\n");
   }
}

int *hex_to_rgb(string hex) {
  int r, g, b;
  
  // Validate input
  if (!stringp(hex) || strlen(hex) != 7 || hex[0] != '#') {
      return 0; // Invalid format
  }
  
  // Convert hex to decimal values
  r = to_int("0x" + hex[1..2]);
  g = to_int("0x" + hex[3..4]);
  b = to_int("0x" + hex[5..6]);
  
  return ({ r, g, b });
}

/* Setzt Farbe fuer Properties P_COLOR_EXIT,
 * P_COLOR_COMMUNICATION, P_COLOR_INFORM
 */
static void setcolor(string farbe, string prop, string name) {
     if(farbe[0..0] == "#") {
    int *hexcol = hex_to_rgb(farbe);
    if(!pointerp(hexcol)) {
      write(sprintf("Ungültige hex-Farbe. Format: #rrggbb\n", hexcol));
      return;
    }

    // true color support
    SetProp(prop, hexcol);
  } else {
  switch(farbe){
   case "schwarz":  SetProp(prop, 1);break;
   case "rot":      SetProp(prop, 2);break;
   case "grün":    SetProp(prop, 3);break;
   case "gelb":     SetProp(prop, 4);break;
   case "blau":     SetProp(prop, 5);break;
   case "lila":     SetProp(prop, 6);break;
   case "cyan":     SetProp(prop, 7);break;
   case "weiß":    SetProp(prop, 8);break;
   case "fett":
   case "bold":     SetProp(prop, 9);break;
   case "hellblau": SetProp(prop,10);break;
   case "braun":    SetProp(prop,11);break;
   case "dunkelgrau":
   case "grau":     SetProp(prop,12);break;
   case "hellcyan": SetProp(prop,13);break;
   case "hellgrün":SetProp(prop,14);break;
   case "hellrot":  SetProp(prop,15);break;
   case "helllila":
   case "hellila":  SetProp(prop,16);break;
   case "standard":
   case "default":
   case "hellgrau":
   case "normal":   SetProp(prop, 0);break;
   default:         write("Farbe "+farbe+" nicht erkannt.\n");
                    farbliste();
                    return;
  }
  }
  Set(prop, SAVE, F_MODE_AS);
  write(name+" "+_query_colorI(prop)+farbe+ANSI_NORMAL+" gefärbt.\n");
  return;
}

static int _farbenauswahl(string str)
{
  string channel,was,farbe;
  mixed colcode;
  mapping map;
  notify_fail("Was möchtest Du färben? Syntax: farbe <was> <farbe>\n");
  if (QueryProp(P_TTY)!="ansi") {
    write("Farben sind deaktiviert! Bitte erst 'stty ansi' einstellen.\n");
    return 1;
  }
  if (!str) {
    printf(
      "Aktuell färbbar:\n"
      "%sKommunikation"+ANSI_PLAIN+", %sAusgänge"+ANSI_PLAIN+
      ", %sInform"+ANSI_PLAIN+" und die Ebenen (anzeigen mit: '-!').\n"
      "Syntax: farbe <kommunikation|ausgang|inform> <farbe>\n"
      "        farbe <ebene> <farbe>|ein|aus\n",
      _query_colorI(P_COLOR_COMMUNICATION),_query_colorI(P_COLOR_EXIT),
      _query_colorI(P_COLOR_INFORM));
    return 1;
  }
  if (str=="liste"||str=="farbliste") {
    farbliste();
    return 1;
  }
  if (str=="liste ebene"||str=="farbliste ebene" || str=="liste ebenen"||str=="farbliste ebenen") {
    farbliste(1);
    return 1;
  }
  str=_unparsed_args();
  if (sscanf(str,"%s %s",was,farbe)!=2)
    return 0;

  farbe=lower_case(farbe);
  switch (lower_case(was)) {
  case "tm":
  case "kommunikation":
  case "teile":
    setcolor(farbe,P_COLOR_COMMUNICATION,"Kommunikation");
    return 1;
  case "ausgang":
  case "ausgänge":
  case "ausgaenge":
    setcolor(farbe,P_COLOR_EXIT,"Ausgänge");
    return 1;
  case "inform":
  case "erwarte":
    setcolor(farbe,P_COLOR_INFORM,"Inform und Erwarte");
    return 1;
  default:
    // ebenen
  }

  if (farbe=="aus"||farbe=="nein") {
    colcode=0; // ebenentext nicht faerben
  } else if (farbe=="ein"||farbe=="ja") {
    colcode=1; // ebenentext faerben
  } else {
    colcode=VOICEMASTER->colorn2col(farbe);
    if (!stringp(colcode)||colcode=="") {
      write("Farbe '"+farbe+"' nicht erkannt.\n");
      farbliste(1);
      return 1;
    }
  }
  map=QueryProp(P_CHANNELS);
  if (was=="alle"||was=="!") {
    string *inds;
    int i;
    inds=m_indices(map);
    if (colcode==0||colcode==1) {
      for (i=0; i<sizeof(inds); i++)
        map[inds[i]][1]=colcode;
      write(
        "Der Ebenentext aller Ebenen wird jetzt "+
        (colcode?"":"nicht mehr ")+"gefärbt.\n");
    } else {
      for (i=0; i<sizeof(inds); i++)
        map[inds[i]][0]=colcode;
      write("Alle Ebenen "+parse_color_string(
       VOICEMASTER->colorn2col(farbe)+farbe+"^N")+" gefärbt.\n");
    }
    return 1;
  }
  channel=VOICEMASTER->get_channel(was);
  if (channel==0 || channel==""){
    write("Die Ebene '"+was+"' gibt es nicht.\n"
    "Was möchtest Du färben? Syntax: farbe <was> <farbe>\n");
    return 1;
  }
  if(!VOICEMASTER->channelmember(was,ME)){
    write(sprintf("Du hast die Ebene '%s' nicht betreten!\n",was));
    return 1;
  }
  if (colcode==0||colcode==1) {
    map[channel][1]=colcode;
    write(
      "Der Ebenentext von "+channel+" wird jetzt "+
      (colcode?"":"nicht mehr ")+"gefärbt.\n");
  } else {
    map[channel][0]=colcode;
    write("Ebene "+channel+" "+parse_color_string(
      VOICEMASTER->colorn2col(farbe)+farbe+"^N")+" gefärbt.\n");
  }
  return 1;
}

static int _fightmessages(string stra)
{
  Set(P_FIGHTMESSAGES, SAVE, F_MODE_AS);

    if ( (QueryProp(P_FIGHTMESSAGES) == 0)   ||
         (QueryProp(P_FIGHTMESSAGES) == "0") )
    {
    write("Derzeit werden die neuen Kampfmeldungen angezeigt.\n");
    }
    else
    {
    write("Derzeit werden die alten Kampfmeldungen angezeigt.\n");
    }
    write("Ändern der Einstellungen: 'kampfmeldungen neu|alt'.\n");

  if (stra == "alt") {
    SetProp(P_FIGHTMESSAGES, 1);
    write("Ab sofort werden die alten Kampfmeldungen angezeigt.\n");
  }
  if (stra == "neu") {
    SetProp(P_FIGHTMESSAGES, 0);
    write("Ab sofort werden die neuen Kampfmeldungen angezeigt.\n");
  }
  return 1;
}


static int _set_homepage(string str)
{
  this_player()->Set(P_HOMEPAGE, SAVE, F_MODE_AS);

 if(QueryProp(P_HOMEPAGE) == "none")
  { SetProp(P_HOMEPAGE, 0); }

  if (!(str=_unparsed_args()))
  {
    if(!stringp(QueryProp(P_HOMEPAGE)))
    {
        write("Du hast Dir keine Homepage gesetzt!\n");
        return 1;
    }
    write("Deine offizielle Homepage Adresse lautet: \n"+
          QueryProp(P_HOMEPAGE)+"\n");
    return 1;
  }

  if (sizeof(str)>57)
  {
   printf("Eingabe zu lang! Maximal 57 Zeichen möglich.\n");
   return 1;
  }

  if(str == "loeschen")
  {
    write("Ok, Du hast Deine offizielle Homepage gelöscht.\n");
    SetProp(P_HOMEPAGE, 0);
  }
  else
  {
    write("Deine offizielle Homepage wurde geändert.\n");
    SetProp(P_HOMEPAGE, str);
  }
  return 1;

}

static int anzeige(string str)
{
  string s1;

  if ((!str) || (str==""))
  {
    write ("Aktuelle Einstellung der Anzeige:\n");
    switch (QueryProp (P_COLUMN_MODE))
    {
      case 0:
        write ("Objekte werden untereinander angezeigt und");
        break;
      case 1:
        write ("Objekte werden in Spalten angezeigt und");
        break;
      default:
        write ("Über sechs Objekte werden in Spalten angezeigt, es wird");
    }
    write ((QueryProp(P_SHORTINV) ? " immer" : " nie") + " gruppiert.\n");

    return 1;
  }

  if (sscanf (str, "spalten%s", s1)==1)
  {
    switch (s1)
    {
      case "":
      case " ein":
        write ("Ok, Objekte werden jetzt in Spalten angezeigt.\n");
        SetProp (P_COLUMN_MODE, 1);
        break;
      case " aus":
        write ("Ok, Objekte werden jetzt untereinander angezeigt.\n");
        SetProp (P_COLUMN_MODE, 0);
        break;
      case " auto":
        write ("Ok, Objekte (mehr als 6) werden jetzt automatisch in "+
          "Spalten angezeigt.\n");
        SetProp (P_COLUMN_MODE, 2);
        break;
      default:
        write ("anzeige spalten ein|aus|auto\n");
    }
  }
  else if (sscanf (str, "gruppieren%s", s1)==1)
  {
    switch (s1)
    {
      case "":
      case " ja":
        write ("Ok, Objekte werden jetzt gruppiert.\n");
        SetProp (P_SHORTINV, 1);
        break;
      case " nein":
        write ("Ok, Objekte werden nicht gruppiert.\n");
        SetProp (P_SHORTINV, 0);
        break;
      default:
        write ("anzeige gruppieren ja|nein\n");
    }
  }
  else
    write("Syntax:\nanzeige gruppieren ja|nein\nanzeige spalten ein|aus|auto\n");

  return 1;
}

static int ausgabe(string str)
{
  string s1;
  string verb;
  verb = query_verb();
  if ((!str) || (str==""))
  {
    printf("Aktuelle Einstellungen zur Darstellung:\n"
     "Grafik wird %s.\n"
     "Promptzeichen %s.\n"
     "Kampfmeldungen werden %s dargestellt.\n"
     "Gleiche Objekte werden %sgruppiert.\n\n",
    ((QueryProp(P_NO_ASCII_ART))?"vermieden":"angezeigt"),
    ((QueryProp(P_PROMPT)==" ")?"ist gelöscht":"wird angezeigt"),
    ((QueryProp(P_BRIEF) & BRIEF_FMASK & BRIEF_FNODAMAGE)?"minimal"
      :"vollständig"),
    ((QueryProp(P_SHORTINV))?"":"nicht "));
  }
  else if (str == "gruppiert")
  {
    anzeige("gruppieren ja");
    return 1;
  }
  else if (str == "einzeln")
  {
    anzeige("gruppieren nein");
    return 1;
  }
  else if ((str=="screenreader")||(str=="sprachausgabe")||(str=="ja"))
  {
    ausgabe("grafik aus");
    ausgabe("prompt löschen");
    viewcmd::_set_fbrief("schaden aus");
    anzeige("gruppieren ja");
    return 1;
  }
  else if ((str=="normal")||(str=="nein"))
  {
    ausgabe("grafik an");
    ausgabe("prompt wiederherstellen");
    viewcmd::_set_fbrief("schaden ein");
    anzeige("gruppieren nein");
    return 1;
  }
  else if (sscanf(str, "grafik%s", s1)==1)
  {
    if ((s1==" ein")||(s1==" an"))
    {
      SetProp(P_NO_ASCII_ART,0);
      write("ASCII-Grafik wird jetzt angezeigt.\n");
    }
    else if (s1==" aus")
    {
      SetProp(P_NO_ASCII_ART,1);
      write("ASCII-Grafik wird jetzt vermieden.\n");
    }
    else
      write(verb+" grafik ein|aus\n");
    return 1;
  }
  else if (sscanf(str, "prompt%s", s1)==1)
  {
    mixed prompt;
    prompt = QueryProp(P_PROMPT);
    if ((s1==" löschen")||(s1==" entfernen")||(s1==" loeschen"))
    {
      if (prompt != " ")
      {
        Set(P_PROMPT," ");
        set_prompt(" ");
        write("Das Promptzeichen wurde gelöscht.\n");
        if (prompt != "> ")
          write("Altes Prompt war: "+prompt+"\n");
      }
      else
        write("Das Promptzeichen war bereits gelöscht.\n");
    }
    else if ((s1==" wiederherstellen")||(s1==" setzen"))
    {
      if (prompt != "> ")
      {
        Set(P_PROMPT,"> ");
        set_prompt("> ");
        write("Das Promptzeichen ist jetzt: >\n");
        if (prompt != " ")
          write("Altes Prompt war: "+prompt+"\n");
      }
      else
        write("Das Promptzeichen war bereits gesetzt.\n");
    }
    else
      write(verb+" prompt löschen|setzen\n");
    return 1;
  }
  else if (sscanf(str, "kampfmeldungen%s", s1)==1)
  {
    if (s1==" kurz")
      viewcmd::_set_fbrief("schaden aus");
    else if ((s1==" lang"))
      viewcmd::_set_fbrief("schaden ein");
    else
      write(verb+" kampfmeldungen kurz|lang\n");
    return 1;
  }

  write("Syntax:\n"
   +verb+" screenreader|normal\n"
   +verb+" grafik aus|ein\n"
   +verb+" prompt löschen|setzen\n"
   +verb+" kampfmeldungen kurz|lang\n"
   +verb+" gruppiert|einzeln\n");
  return 1;
}


// XxXxXxXxXx

int more(string str)
{
  if(!str)
  {
    notify_fail("Usage: more <file>\n");
    return 0;
  }
  if (file_size(str) <= 0) {
    notify_fail(str+": No such file\n");
    return 0;
  }
  More(str, 1);
  return 1;
}

static int set_screensize(string str)
{
  int size;

  if (str && (str[0..2] == "abs" || str[0..2]=="rel")) {
    size = QueryProp(P_MORE_FLAGS);
    if (str[0..2] == "abs") {
      size |= E_ABS;
      write("Ok, beim Prompt wird die Zeilenzahl des Textes angegeben.\n");
    }
    else {
      size &= ~E_ABS;
      write("Ok, beim Prompt wird der prozentuale Anteil des Textes "
          "angegeben.\n");
    }
    SetProp(P_MORE_FLAGS, size);
    return 1;
  }
  if (!str || str=="" || !sscanf(str,"%d",size) || size<0 || size>100)
  {
    write(break_string(
        sprintf("Mit dem Befehl 'zeilen <größe>' kannst Du einstellen, "
            "wieviele Zeilen bei mehrseitigen Texten auf einmal ausgegeben "
            "werden. Die angegebene Größe muss zwischen 0 und 100 liegen. "
            "Bei Größe 0 wird einfach alles ausgegeben (ohne Pause). Die "
            "Voreinstellung ist 20 Zeilen.\n"
            "Mit 'zeilen abs[olut]' und 'zeilen rel[ativ]' kannst Du fest"
            "legen, ob im Prompt bei langen Texten die aktuelle Zeilennummer "
            "oder eine prozentuale Angabe ausgegeben wird.\n"
            "Deine aktuelle Einstellung ist %d Zeilen (%s).",
            QueryProp(P_SCREENSIZE),
            QueryProp(P_MORE_FLAGS) & E_ABS ? "absolut" : "relativ"),78,0,1));
    return 1;
  }
  SetProp(P_SCREENSIZE,size);

  printf("Ok, Deine Zeilenzahl steht nun auf %d.\n",size);
  return 1;
}

static int presay(string str)
{
  if( sizeof(_unparsed_args()) > 35 )
  {
    write("Das ist leider zu lange.\n");
    return 1;
  }
  if( sizeof(_unparsed_args()) + sizeof(QueryProp(P_TITLE)) > 65  )
  {
    write("Zusammen mit Deinem Titel ist das leider zu lange.\n");
    return 1;
  }
  if (!str=_unparsed_args())
    write("Dein Presay ist jetzt gelöscht.\n");
  else
    printf("Dein Presay lautet jetzt: \"%s\".\n",str=capitalize(str));
  SetProp(P_PRESAY,str);
  write(short());
  return 1;
}

static int sethands(string str)
{
  mixed *hands;

  if (!(str=_unparsed_args()))
  {
    write("sethands <message>\n");
    return 1;
  }
  hands=Query(P_HANDS);
  hands[0]=" "+str;
  Set(P_HANDS,hands);
  write("Ok.\n");
  return 1;
}

static int setlook(string str)
{
  if(QueryProp(P_LOOK_INVIS))
  {
    write("Dein erweitertes Aussehen ist jetzt sichtbar!\n");
    SetProp(P_LOOK_INVIS,0);
  }
  else
  {
    write("Dein erweitertes Aussehen ist jetzt unsichtbar!\n");
    SetProp(P_LOOK_INVIS,1);
  }
  return 1;
}

static int inform(string str)
{
  switch (str) {
    case "on":
    case "ein":
    case "an":
      if (Query(P_INFORMME))
    write("Das hattest Du schon so eingestellt.\n");
      else
      {
    write("Künftig wirst Du informiert, wenn jemand das "+MUDNAME+" "
        "verlässt/betritt.\n");
    Set(P_INFORMME,1);
      }
      return 1;
    case "aus":
    case "off":
      if (!Query(P_INFORMME))
    write("Das hattest Du schon so eingestellt.\n");
      else
      {
    write("Ok.\n");
    Set(P_INFORMME,0);
      }
      return 1;
    case 0:
      write("Inform-Mode ist "+(Query(P_INFORMME)?"an":"aus")+"geschaltet.\n");
      return 1;
    }
  write("inform an oder inform aus, bitte.\n");
  return 1;
}

void delayed_write(mixed *what)
{
  if (!pointerp(what)||!sizeof(what)||!pointerp(what[0]))
    return;
  tell_object(ME,what[0][0]);
  if (sizeof(what)>1&&sizeof(what[0])>1)
    call_out("delayed_write",what[0][1],what[1..]);
}

void notify_player_change(string who, int rein, int invis)
{
  string *list;
  string bon,boff;
  string color;

  if (QueryProp(P_TTY)=="ansi"){
	color = _query_colorI(P_COLOR_INFORM);
  }else{
  	color = a_plain;
  }

  if (QueryProp("tty")!="dumb")
  {
    bon = ANSI_BLINK;
    boff = ANSI_NORMAL;
  }

  if (Query(P_INFORMME))
  {
    if (rein)
    {
      string tls;
      if(tls_query_connection_state(this_player()))
        tls="(verschlüsselt)";
      else
        tls="(unverschlüsselt)";

      if(!IS_QUESTWIZ(ME))
      {
      	tell_object(ME,
      	 	sprintf(color+who+a_plain+" ist gerade %sins "
                   +color+MUDNAME+a_plain+" gekommen. %s\n",
                (invis?bon+"UNSICHTBAR "+boff:""),tls));
      }
      else
      {
      	tell_object(ME,
      		sprintf(color+who+a_plain+" [%s] ist gerade %sins "
                    +color+MUDNAME+a_plain+" gekommen.%s \n",
      		query_ip_name(find_player(lowerstring(who))),
      		(invis?bon+"UNSICHTBAR "+boff:""),tls));
      }
    }
    else
      tell_object(ME,color+who+a_plain+" hat gerade das "
                  +color+MUDNAME+a_plain+" verlassen.\n");
  }
  if (pointerp(list=Query(P_WAITFOR))&&sizeof(list))
    if (member(list,who)!=-1)
        tell_object(ME, sprintf("\a%s   I S T   J E T Z T  %s%sD A !!!\n", color+who+a_plain,
                   (rein ? "" : "N I C H T   M E H R   "),
                   (invis&&rein?bon+"U N S I C H T B A R  "+boff:"")));
}

static int erwarte(string str)
{
  string *list;
  int i;

  if (str)
  {
    str=capitalize(lower_case(str));
    if (!pointerp(list=Query(P_WAITFOR)))
      list=({});
    if (member(list,str)!=-1)
    {
      list-=({str});
      write(str+" aus der Liste entfernt.\n");
    } else
    {
      list+=({str});
      write(str+" an die Liste angehängt.\n");
    }
    Set(P_WAITFOR,list);
    }
  list=Query(P_WAITFOR);
  write("Du erwartest jetzt ");
  if (!pointerp(list)||!sizeof(list))
  {
    write("niemanden mehr.\n");
    return 1;
  }
  for (i=sizeof(list)-1;i>=0;i--)
    write(list[i]+(i?", ":".\n"));
  return 1;
}

static int uhrmeldung(string str)
{
  if (!(str=_unparsed_args()))
  {
    str=QueryProp(P_CLOCKMSG);
    if (!str)
    {
      write("Du hast die Standard-Uhrmeldung.\n");
      return 1;
    }
	if( !stringp(str) ) str = sprintf("%O\n",str);
    printf("Deine Uhrmeldung ist: \"%s\"\n",str[0..<2]);
    return 1;
  }
  if (str=="0")
  {
    SetProp(P_CLOCKMSG,0);
    write("Ok, Du hast jetzt wieder die Standard-Meldung.\n");
    return 1;
  }
  if (sizeof(explode(str,"%d"))>2)
  {
    write("Fehler, es darf nur ein %d im String vorkommen.\n");
    return 1;
  }
  str+="\n";
  SetProp(P_CLOCKMSG,str);
  write("Ok.\n");
  return 1;
}

static int muduptime()
{
  write ("Letzter Reboot: "+dtime(last_reboot_time())+",\n");
  write ("Uptime: "+uptime()+".\n");
  return 1;
}

static int raste()
{
  if (QueryProp(P_RESTING))
    write("Aber Du rastest doch schon!\n");
  else if (QueryProp(P_GHOST))
    write("Als Geist kannst Du nicht rasten oder Ruhe finden!\n");
  else if (time() < QueryProp(P_LAST_AWAKE_TIME) + 300)
    write("So oft kannst Du Dich nicht ausrasten!\n");
  else {
    write("Du schließt die Augen und rastest Dich ein wenig aus. "
        "Tut das gut...\n");
    SetProp(P_RESTING, 1);
    call_out("wache", 60, "auf");
  }
  return 1;
}

static int wache(string str)
{
  if (str!="auf") return 0;
  if (!QueryProp(P_RESTING))
    return write("Aber Du rastest doch gar nicht!\n"), 1;
  while (remove_call_out("wache") != -1) ;
  if (find_call_out("wache2")!=-1)
    return write("Gedulde Dich doch ein wenig!\n"), 1;
  if (QueryProp(P_GHOST)) {
    write("Etwas zu spät wachst Du wieder auf...\n");
    SetProp(P_RESTING, 0);
    SetProp(P_LAST_AWAKE_TIME, time());
    return 1;
  }
  write("Nach und nach wirst Du wieder munter.\n");
  call_out("wache2", 10);
  return 1;
}

static void wache2()
{
  if (!QueryProp(P_GHOST))
    write("Frisch erholt erhebst Du Dich.\n");
  SetProp(P_RESTING, 0);
  SetProp(P_LAST_AWAKE_TIME, time());
}

static int pluendern(string str)
{
  if( str == "ein" || str == "ja" )
  {
    write("Du plünderst jetzt alle Leichen aus.\n");
    SetProp(P_LOOTING, 1);
  }
  else if( str == "aus" || str == "nein" )
  {
    write("Du plünderst jetzt keine Leichen mehr aus.\n");
    SetProp(P_LOOTING, 0);
  }
  else
  {
    write("Du plünderst zur Zeit "+( QueryProp(P_LOOTING) ? "alle" : "keine" )+
         " Leichen aus.\n");
    write("Umschalten mit 'plündern <ein|aus>.\n");
  }
  return 1;
}

static string *_query_localcmds()
{
  return ({({"zeilen","set_screensize",0,0}),
           ({"email","set_email",0,0}),
           ({"ort", "set_ort", 0, 0}),
           ({"punkte","score",0,0}),
           ({"score","score",0,0}),
           ({"info","info",0,0}),
           ({"quit","new_quit",0,0}),
           ({"ende","new_quit",0,0}),
           ({"disconnect","disconnect",0,0}),
           ({"schlaf","disconnect",1,0}),
           ({"speichern","save_character",0,0}),
           ({"save","save_character",0,0}),
           ({"toete","kill",0,0}),
           ({"töte","kill",0,0}),
           ({"passw","change_password",1,0}),
           ({"hilfe","help",1,0}),
           ({"selbstloeschung","self_delete",0,0}),
           ({"selbstlöschung","self_delete",0,0}),
           ({"spielpause","spielpause",0,0}),
           ({"sprachausgabe","ausgabe",0,0}),
           ({"ausgabe","ausgabe",0,0}),
           ({"epidee","epidea",0,LEARNER_LVL}),
           ({"idee","idea",0,0}),
           ({"typo","typo",0,0}),
           ({"bug","bug",0,0}),
           ({"detail","detail",0,0}),
           ({"loben","loben",0,0}),
           ({"fehler","bug",0,0}),
           ({"vorsicht","toggle_whimpy",0,0}),
           ({"stop","stop",0,0}),
           ({"kwho","kwho",0,0}),
           ({"kwer","kwho",0,0}),
           ({"kkwer","kkwho",0,0}),
           ({"kkwho","kkwho",0,0}),
           ({"who","who",0,0}),
           ({"wer","who",0,0}),
           ({"zeit","uhrzeit",0,0}),
           ({"uhrzeit","uhrzeit",0,0}),
           ({"weg","weg",0,0}),
           ({"finger","finger",0,0}),
           ({"muds","muds",0,0}),
           ({"emote","emote",0,0}),
           ({":","emote",1,0}),
           ({"gemote","emote",0,0}),
           ({";","emote",1,0}),
           ({"remote","remote",0,0}),
           ({"r:","remote",1,0}),
           ({"rgemote","rgemote",0,0}),
           ({"r;","rgemote",1,0}),
           ({"lookfor","lookfor",0,SEER_LVL}),
           ({"titel","set_title",0,SEER_LVL}),
           ({"review","review",0,SEER_LVL}),
           ({"setmin","setmin",0,SEER_LVL}),
           ({"setmout","setmout",0,SEER_LVL}),
           ({"setmmin","setmmin",0,SEER_LVL}),
           ({"setmmout","setmmout",0,SEER_LVL}),
           ({"setcmsg","setcmsg",0,LEARNER_LVL}),
           ({"setdmsg","setdmsg",0,LEARNER_LVL}),
           ({"sethands","sethands",0,SEER_LVL}),
           ({"setlook","setlook",0,SEER_LVL}),
           ({"presay","presay",0,SEER_LVL}),
           ({"extralook","extralook",0,SEER_LVL}),
           ({"fluchtrichtung","toggle_whimpy_dir",0,SEER_LVL}),
           ({"inform","inform",0,0}),
           ({"erwarte","erwarte",0,0}),
           ({"stty","stty",0,0}),
           ({"farbe","_farbenauswahl",0,0}),
           ({"farben","_farbenauswahl",0,0}),
           ({"kampfmeldung","_fightmessages",0,0}),
           ({"kampfmeldungen","_fightmessages",0,0}),
           ({"homepage","_set_homepage",0,0}),
           ({"anzeige","anzeige",0,0}),
           ({"uhrmeldung","uhrmeldung",0,0}),
           ({"behalte","behalte",0,0}),
           ({"uptime","muduptime",0,0}),
           ({"rast","raste",0,0}),
           ({"raste","raste",0,0}),
           ({"wach","wache",0,0}),
           ({"wache","wache",0,0}),
           ({"steh","wache",0,0}),
           ({"stehe","wache",0,0}),
           ({"pluendern","pluendern",0,0}),
           ({"plündern","pluendern",0,0}),
  })+
     sense::_query_localcmds()+
     command::_query_localcmds()+
     viewcmd::_query_localcmds()+
     comm::_query_localcmds()+
     extra_skills::_query_localcmds();
}

#if 0
void telnet_neg(mixed opt,mixed cmd,mixed args)
{
  if (cmd==34)
  {
    switch (opt)
    {
      case 251: //WILL
      binary_message(({255,250,34,1,1,255,240}));
      break;
      default: break;
    }
  }
}
#endif

static string _query_auth_info()
{
  mixed auth;

  if (!interactive(ME))
    return Set(P_AUTH_INFO,0);
  if (stringp(auth=Query(P_AUTH_INFO))) return auth;
  if (auth>3) return Set(P_AUTH_INFO,"UNKNOWN");
  "/secure/master"->get_auth_user(ME);
  Set(P_AUTH_INFO,++auth);
  return "NOT YET KNOWN";
}

static int _set_auth_info(mixed s)
{
  if (!previous_object(1)||object_name(previous_object(1))[0..7]!="/secure/")
    return 0;
  Set(P_AUTH_INFO,s);
  return 1;
}

void _set_my_ip(string str)
{
  if (object_name(previous_object())[0..7]!="/secure/") return;
  my_ip=str;
}

string _query_my_ip()
{
  return my_ip;
}

static mixed _query_mailaddr()
{
  if(QueryProp(P_MAILADDR_INVIS)==1 &&
     !IS_ARCH(this_player()) && this_player()!=this_object())
    return 0;
  return Query(P_MAILADDR);
}

static mixed _query_second()
{
  if(query_wiz_level(this_player()) >= QueryProp(P_SECOND_INVIS) ||
     !previous_object(2))// Hack, weil merlin kein previous_object(2) liefert
    return Query(P_SECOND);
  return (Query(P_SECOND)?1:0);
}

mixed _set_testplayer(mixed arg) {
  mixed res;
  object setob;

  setob=this_player();
  if (!objectp(setob) || !query_once_interactive(setob))
    setob=this_interactive();
  if (!objectp(setob))
    setob=previous_object();
  if (setob && !IS_ARCH(setob)) {
    arg=geteuid(setob);
    if (!arg || arg=="NOBODY")
      arg=getuid(setob);
    arg=capitalize(arg);
  }
  res=Set(P_TESTPLAYER,arg);
  Set(P_TESTPLAYER,PROTECTED,F_MODE_AS);
  return res;
}

static int _check_keep(object ob)
{
  return (ob->QueryProp(P_KEEP_ON_SELL))==geteuid(ME);
}

int behalte(string str)
{
  object *obs, ob;
  string s;

  if (str)
  {
    if (str=="nichts")
    {
      map_objects(filter_objects(all_inventory(ME),"short"),
          "SetProp", P_KEEP_ON_SELL, 0);
      write("Ok, Du behältst bei 'verkaufe alles' nichts mehr.\n");
      return 1;
    }
    obs=present_objects(str);
    if (!sizeof(obs))
    {
      _notify_fail("Aber sowas hast Du nicht dabei!\n");
      return 0;
    }
    if (sizeof(filter(obs,#'_check_keep)) != sizeof(obs))
    {
      map_objects(obs, "SetProp", P_KEEP_ON_SELL, geteuid(ME));
      write(break_string("Ok, Du wirst "+CountUp(map_objects(obs,"name",WEN))+
          " jetzt bei 'verkaufe alles' behalten.", 78));
    }
    else
    {
      map_objects(obs, "SetProp", P_KEEP_ON_SELL, 0);
      write(break_string("Ok, Du wirst "+CountUp(map_objects(obs,"name",WEN))+
          " beim nächsten 'verkaufe alles' mitverkaufen!", 78));
    }
    return 1;
  }
  obs=filter(all_inventory(ME),#'_check_keep);
  if (!sizeof(obs))
    write("Du hast derzeit nichts zum Behalten markiert!\n");
  else
    More("Du behältst bei 'verkaufe alles':\n"+make_invlist(ME,obs));
  return 1;
}


/* _query_color(object ob, string prop)
 *
 * prop sollte entweder P_COLOR_EXIT, P_COLOR_INFORM oder
 * P_COLOR_COMMUNICATION sein.
 * Wandelt den int-code fuer die Farbe in den entsprechenden ANSI-Farbcode
 * */
string _query_color(object ob, string prop)
{
  mixed s1;
  if (!interactive(ob)) return "";
  s1=ob->QueryProp(prop);
  
   // true color support
   if(pointerp(s1)) {
    if(sizeof(s1) != 3) {
      tell_object(this_object(), "Warnung: Ungültige Farbeinstellung für "+prop+" zurückgesetzt auf 'plain'.\n");
      return ANSI_PLAIN;
    }

    int r = s1[0];
    int g = s1[1];
    int b = s1[2];

    if(ob->QueryProp(P_ANSI_TRUE_COLOR)) {
      return ANSI_TRUE_COLOR(s1[0], s1[1], s1[2]);
    } else {
      return rgb_to_ansi(r, g, b);
    }
   }
   
  if ((s1==0)||(!intp(s1))) return "";
  switch(s1) {
   case  1: return ANSI_BLACK;
   case  2: return ANSI_RED;
   case  3: return ANSI_GREEN;
   case  4: return ANSI_LYELLOW;
   case  5: return ANSI_BLUE;
   case  6: return ANSI_PURPLE;
   case  7: return ANSI_CYAN;
   case  8: return ANSI_WHITE;
   case  9: return ANSI_BOLD;
   case 10: return ANSI_LBLUE;
   case 11: return ANSI_BROWN;
   case 12: return ANSI_GREY;
   case 13: return ANSI_LCYAN;
   case 14: return ANSI_LGREEN;
   case 15: return ANSI_LRED;
   case 16: return ANSI_LPURPLE;
   default: return ANSI_PLAIN;
  }
  return "";
}

string _query_colorI(string prop)
{
  return _query_color(this_object(),prop);
}

// Cache fuer QueryArmourByType()
private nosave mapping QABTCache;

public object|object*|mapping QueryArmourByType(string type)
{
  // Rueckgabewert:
  // DIE Ruestung vom Typ <type>, falls <type> nicht AT_MISC,
  // Array aller AT_MISC-Ruestungen falls <type> AT_MISC (auch leer),
  // Mapping mit allen oben genannten Infos, falls <type> Null

  object *armours;
  string typ2;

  // Wenn Cache vorhanden, dann Cache liefern.
  if (mappingp(QABTCache)) {
    if (type == AT_MISC)
      return QABTCache[AT_MISC] - ({0});
    else if (type)
      return QABTCache[type];
    else
      return copy(QABTCache);
  }

  if ( !pointerp(armours=QueryProp(P_ARMOURS)) )
    armours=({});

  // Cache erzeugen
  QABTCache = ([ AT_MISC: ({}) ]);
  foreach(object ob: armours - ({0}) ) {
    if ( !stringp(typ2=ob->QueryProp(P_ARMOUR_TYPE)) )
      continue;
    if ( typ2==AT_MISC )
      QABTCache[AT_MISC] += ({ob});
    else
      QABTCache[typ2] = ob;
  }
  // Und gewuenschtes Datum liefern.
  if (type)
    return QABTCache[type];
  else
    return copy(QABTCache);
}
