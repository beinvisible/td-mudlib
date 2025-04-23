// Tamedhon MUDlib
//
// /p/daemon/inews.c -- International News
//
// $Date: 2002/03/06 19:45:40 $
/* $Revision: 1.4 $
 * $Log: inews.c,v $
 * Revision 1.4  2002/03/06 19:45:40  Querolin
 * Diverse Umbauarbeiten und Erweiterungen.
 *
 * Revision 1.3  1998/02/05 19:45:40  Woody
 * Bugfix fuer Meldungen ohne Subline (bzw. Leerstring stattdessen)
 *
 * Revision 1.2  1998/01/19 17:53:45  Woody
 * Komplett ueberarbeitet (Nachrichten von Mond / Radio Wien)
 *
 * Revision 1.1  1997/03/16 02:28:52  Woody
 * prepared for revision control
 *
 */

// INEWS.C -- International News
/*
 * Der inews-daemon sorgt dafuer, dass die Nachrichten, die extern
 * zur Verfuegung gestellt werden, regelmaessig ausgelesen und ver-
 * arbeitet werden. Das Newsfile hat eine entsprechende, vorgegebene
 * Struktur:
 *
 *   Schlagzeile (idR. ein Wort)
 *   Titelzeile (etwas laenger)
 *   Codezeile ("nummer|quelle|thema|(ON|OFF)")
 *   Nachrichtentext [..]
 *   Delimiterzeile ("-- ")
 *
 * Dieses File wird regelmaessig ausgelesen, geparsed, und geloescht.
 *
 */


#pragma strong_types
#pragma combine_strings

#include <daemon.h>
#include <config.h>
#include <properties.h>

#define NEWSFILE "/p/daemon/save/INEWS"
//#define PROVIDER "Heise"
#define RUBRIK "nachrichten"
#define ADMIN "querolin"
#define CHANNEL_ONLY_HEADLINES

//#define DEBUG(str) tell_object(find_player(ADMIN), sprintf("|\\| %O\n",str))
#define MIN(a,b) (a<b ? a : b)
#define MAX(a,b) (a>b ? a : b)

mixed data = ({});
string PROVIDER;

static void NewsUpdate();
static void Ticker(int step);
static void InformPlayers(string group, string text);


void create()
{
  seteuid(getuid(this_object()));
  PROVIDER="Tamedhon";
  call_out("NewsUpdate", 0);
}

static void NewsUpdate()
{
  mixed *art;
  mixed tmp;
  int i,n;

  while (remove_call_out("NewsUpdate") != -1);
  call_out("NewsUpdate", (3600-(time()%3600)));

  tmp = read_file(NEWSFILE);
  if (tmp)
    data = (pointerp(data) ? data : ({})) + explode(tmp, "-- \n") - ({ 0, "" });

  if (!tmp || !sizeof(data)) {
    VOICEMASTER->SendToChannel("N", this_object(), SAY,
        "Derzeit liegen keine neuen Meldungen vor. "
        "Die nächsten Nachrichten werden in "+
        ((30+find_call_out("NewsUpdate"))/60)+" Minuten gesendet.");
    return;
  }

  call_out("Ticker", 3, 0);
  rm(NEWSFILE);

  // Artikel veroeffentlichen
  n=0;
  for (i=0; i<sizeof(data); i++)
  {
    tmp = data[i];
    art = explode(data[i], "\n");
    if (strstr(art[1], "|") != -1)
      art = ({ art[0], "" }) + art[2..];
    else
      art = art[0..1] + art[3..];

    PROVIDER=art[0];
    art = ({ RUBRIK, PROVIDER, 0, 0,
          art[0],
          break_string(
              art[0]+":\n"+art[1]+"\n\n"+implode(art[2..], "\n")+"\n\n"+
              "Ein Service von "+PROVIDER+".\n\n", 78, 0, 1) });
    if ( (NEWSSERVER->WriteNote(art,1) > 0) ||
         ( (NEWSSERVER->RemoveNote(RUBRIK,0) > 0) &&
           (NEWSSERVER->WriteNote(art,1) > 0) ) )
      n++;
    else if (find_player(ADMIN))
      tell_object(find_player(ADMIN),
          "[INews:] Fehler bei Posten eines Artikels, bitte prüfen!\n");
  }
  if (n)
    InformPlayers(RUBRIK,
        "* Extrablatt! "+n+" neue Artikel von "+PROVIDER+
        " in Rubrik \""+RUBRIK+"\".\n");
}

static void Ticker(int step)
{
  string text;

  if (!sizeof(data) && (step < 4)) return;
  switch (step) {
    // Ankuendigung
    case 0:
      text = "In Kürze Schlagzeilen aus aller Welt.";
      VOICEMASTER->SendToChannel("N", this_object(), SAY, text);
      call_out("Ticker", 10, 1);
      break;
    // Schlagzeile
    case 1:
      text = explode(data[0], "\n")[0]+":";
      VOICEMASTER->SendToChannel("N", this_object(), SAY, text);
      call_out("Ticker", 3, 2);
      break;
    // Titel (laengere Schlagzeile...)
    case 2:
      text = explode(data[0], "\n")[1];
      if (strstr(text, "|") != -1)  // Ohje, die Subheadline fehlt
#ifdef CHANNEL_ONLY_HEADLINES
        Ticker(4);
#else
        Ticker(3);
#endif
      VOICEMASTER->SendToChannel("N", this_object(), SAY, text);
#ifdef CHANNEL_ONLY_HEADLINES
      data = data[1..];
      if (sizeof(data))
        call_out("Ticker", MAX(sizeof(text)/10, 5), 1);
      else
        call_out("Ticker", 5, 4);
#else
      call_out("Ticker", 5, 3);
#endif
      break;
    // Eigentlicher Text
    case 3:
      text = implode(explode(data[0], "\n")[3..], "\n");
      data = data[1..];
      VOICEMASTER->SendToChannel("N", this_object(), SAY, text);
      if (sizeof(data))
        call_out("Ticker", MAX(sizeof(text)/20, 5), 1);
      else
        call_out("Ticker", 5, 4);
      break;
    // Ende, Credits...
    case 4:
#ifdef CHANNEL_ONLY_HEADLINES
      text = "Ende der Meldungen. Nähere Informationen sind der "
        "Zeitungsrubrik \""+RUBRIK+"\" zu entnehmen. Die nächsten "
        "Nachrichten werden in "+
        ((30+find_call_out("NewsUpdate"))/60)+" Minuten gesendet.";
#else
      text = "Ende der Meldungen. Die nächsten Nachrichten werden "
        "in "+((30+find_call_out("NewsUpdate"))/60)+" Minuten gesendet.";
#endif
      VOICEMASTER->SendToChannel("N", this_object(), SAY, text);
      break;
  }
}

static void InformPlayers(string group, string text)
{
  object *players;
  int i;
  mixed data;

  players=users();
  for (i=sizeof(players)-1;i>=0;i--) {
    data=players[i]->QueryProp(P_WAITFOR);
    if (pointerp(data)&&(member(data,"Zeitung")>-1)) {
      data=players[i]->QueryProp(P_READ_NEWS);
      if (mappingp(data)&&(data[group]>0))
        tell_object(players[i],text);
    }
  }
}

