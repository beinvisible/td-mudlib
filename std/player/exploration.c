/*
 * Tamedhon MUD-Lib
 * ---
 * /std/player/exploration.c
 * Exploration Handling - verwaltet die Forscherpunkte im Spieler
 * Maintained by Woody - Credits to Wargon@MorgenGrauen
 * ---
 *
 * 2023-04-07 Gralkor
 * Der EPMaster gibt nicht immer korrekte Werte zurueck, deshalb ein 
 * kleiner Notfallhack, der dann wenigstens kein Division by Zero macht.
 *
 * 2008-02-17 TamTam:
 * QueryScores() neu fuer Erstkillerabfragen
 *
 * 2007-02-10 Gralkor:
 * Grenze von 100Mio auf 500Mio angehoben, die Umrechnungen auf 200k und 300k
 * geaendert
 *
 * 2006-03-06 Gralkor:
 * 100Mio Bug entfernt (danke an Nemox fuer einen hilfreichen Tip)
 *
 * 2005-01-01 Gralkor:
 * Eine Grenze von 100Mio eingefuehrt, ab der es nur noch 1 Stups/MioXP gibt
 *
 * $Date: 1997/10/09 16:19:42 $
 * $Revision: 3.3 $
 * $Log: exploration.c,v $
 * Revision 3.3  1997/10/09 16:19:42  Woody
 * AP auf 1:1 abgewertet, Gildenrating auf max. 500 Stupse aufgewertet
 *
 * Revision 3.2  1997/06/25 12:43:54  Woody
 * XP Faktor auf 1.6 erhoeht
 * (damit XP etwas entwertet...)
 *
 * Revision 3.1  1997/06/01 15:53:13  Woody
 * Auch der NPCMASTER darf jetzt AddScore() aufrufen
 *
 * Revision 3.0  1997/03/13 01:38:08  Woody
 * prepared for revision control
 *
 * Revision 1.4  97-02-10  Woody
 * Exp2Lp() ein wenig vereinfacht
 *
 * Revision 1.3  96-10-15  Woody
 * Random herabgesetzt
 *
 * Revision 1.2  96-10-05  Woody
 * Erstkillerbonus, XP_BASE von 500 auf 1000 angehoben
 *
 * Revision 1.1  96-10-01  Woody
 * Verwaltung von P_AP hier, weiters P_EP_NUM_FOR_AP
 *
 * Revision 1.0  96-09-25  Woody
 * Erste Implementation
 *
*/

#define NEED_PROTOTYPES

// ---------------------------- Konstanten fuer die Stufenpunktberechnung ---

#define XP_BASE      1000        // XP-2-LP geht in etwa nach der Formel
#define XP_FACT      1.6         // 1000 * 1,6 ^ (LP / 100) = XP
#define XP_EXPLIMIT  50000000    // Aber die XP die ueber 50 Mio. hinausgehn,
#define XP_PER_LP    250000      // sind nur noch 250.000 pro Stups wert.
#define XP_EXPLIMIT2 500000000   // ab 500 Millionen
#define XP_PER_LP2   350000      // nur noch 350.000 pro Stups
#define QP_FACT      2           // Abenteuerpunkte gehen mit Faktor 2 ein
#define EP_FACT      1           // Forscherpunkte mit Faktor 1
#define ZTZ_FACT     6           // zugewiesene ZTs mit Faktor 6
#define ZTN_FACT     2           // nicht zugewiesene ZTs mit Faktor 2
#define NPC_FACT     3           // Erstkillerbonus mit Faktor 3

// --------------------------------------------------------------------------


#include <thing/properties.h>
#include <player/base.h>
#include <exploration.h>
#include <npcmaster.h>
#include <properties.h>
#include <new_skills.h>
#include <attributes.h>

#pragma strong_types

// #define DEBUG(x,y) tell_object(find_player("woody"),sprintf(x,y))
#define DEBUG(x,y)

private string ep;			// Forscherpunkte (BitString)
private int epnum;			// Forscherpunkte (Anzahl)
private string given_scores;		// Monster(erst)killpunkte
private int oldepnum;			// gemeldete EP_NUM
private int epnum_upd_time;		// Zeitpunkt zu dem Aenderungen
					// der EP_NUM aktiv werden
private int ap;				// Attributpunkte
private int epnum_for_ap;		// EP die in AP gewandelt wurden

void create()
{
  Set(P_EP, SECURED, F_MODE_AS);
  Set(P_EP_NUM, SECURED, F_MODE_AS);

  Set(P_LP, SECURED|SAVE, F_MODE_AS);

  Set(P_AP, SECURED, F_MODE_AS);
  Set(P_EP_NUM_FOR_AP, SECURED, F_MODE_AS);

  ep = "";
  epnum = oldepnum = 0;
  ap = epnum_for_ap = 0;
  epnum_upd_time = -1;
  given_scores = "";
}

string Forschung()
{
  int max, my, avg, max_ep;
  string ret;
  
  max_ep=EPMASTER->QueryMaxEP()+1;
  if(max_ep==1) { max_ep=600; }
  if ((my=EPMASTER->QueryExplorationPoints(this_object())) < MIN_EP/2)
    return "Du kennst Dich auf dieser Welt so gut wie gar nicht aus.\n";

  if ((my=EPMASTER->QueryExplorationPoints(this_object())) < MIN_EP)
    return "Du kennst Dich hier etwas besser als ein Anfänger aus.\n";

  my *= 100;
  max = my / max_ep;
  avg = (my+1) / (EPMASTER->QueryAverage()+1);	// hack, um div 0 zu verhindern

  ret = "Verglichen mit Deinen Mitspielern, kennst Du Dich auf dieser Welt ";
  switch(avg) {
  case 0..25:
    ret += "kaum";
    break;
  case 26..45:
    ret += "äußerst schlecht";
    break;
  case 46..56:
    ret += "sehr schlecht";
    break;
  case 57..72:
    ret += "schlecht";
    break;
  case 73..93:
    ret += "unterdurchschnittlich";
    break;
  case 94..109:
    ret += "durchschnittlich gut";
    break;
  case 110..125:
    ret += "besser als der Durchschnitt";
    break;
  case 126..145:
    ret += "recht gut";
    break;
  case 146..170:
    ret += "ziemlich gut";
    break;
  case 171..210:
    ret += "gut";
    break;
  case 211..300:
    ret += "sehr gut";
    break;
  case 301..400:
    ret += "außerordentlich gut";
    break;
  case 401..500:
    ret += "unheimlich gut";
    break;
  default:
    ret += "einfach hervorragend";
    break;
  }
  ret += " aus.\nAbsolut gesehen ";

  switch(max) {
  case 0..15:
    ret += "kennst Du nur wenig von dieser Welt.";
    break;
  case 16..20:
    ret += "solltest Du Dich vielleicht noch genauer umsehen.";
    break;
  case 21..25:
    ret += "bist Du durchaus schon herumgekommen.";
    break;
  case 26..30:
    ret += "hast Du schon einiges gesehen.";
    break;
  case 31..35:
    ret += "bist Du schon weit herumgekommen.";
    break;
  case 36..50:
    ret += "könntest Du eigentlich einen Reiseführer herausbringen.";
    break;
  case 51..75:
    ret += "hast Du schon sehr viel gesehen.";
    break;
  default:
    ret += "besitzt Du eine hervorragende Ortskenntnis.";
  }
  return break_string(ret, 78, 0, 1);
}

static mixed _set_ep(mixed arg)
{
  if (!previous_object(1) || object_name(previous_object(1)) != EPMASTER)
    return EPERR_NOT_MASTER;

  Set (P_EP, SAVE, F_MODE_AD);	// Damit niemand die EP saveable macht
  return ep = (stringp(arg) ? arg : "");
}

static string _query_ep()
{
  return ep;
}

static int _set_ep_num(int arg)
{
  if (!previous_object(1) || object_name(previous_object(1)) != EPMASTER)
    return EPERR_NOT_MASTER;

  Set (P_EP_NUM, SAVE, F_MODE_AD);	// siehe _set_ep()
  return epnum = arg;
}

static int _query_ep_num()
{
  if (time() < epnum_upd_time) return oldepnum;

  epnum_upd_time = time()+TIME_UPD_EPNUM+random(TIME_UPD_EPNUM);
  // oldepnum = epnum;
  oldepnum = epnum + random(6)-3;
  if (oldepnum<0) oldepnum=0;
  return oldepnum;
}

static int _set_ap(int arg)
{
  if (!previous_object(1) ||
      explode(object_name(previous_object(1)), "/")[0] != "gilden")
    return -1;

  Set (P_AP, SAVE, F_MODE_AD);		// siehe _set_ep()
  return ap = arg;
}

static int _query_ap()
{
  return ap;
}

static int _set_ep_num_for_ap(int arg)
{
  if (!previous_object(1) ||
      explode(object_name(previous_object(1)), "/")[0] != "gilden")
    return -1;

  Set (P_EP_NUM_FOR_AP, SAVE, F_MODE_AD);	// siehe _set_ep()
  return epnum_for_ap = arg;
}

static int _query_ep_num_for_ap()
{
  return epnum_for_ap;
}

nomask int AddEP (int arg)
{
  if (!previous_object() || object_name(previous_object()) != EPMASTER)
    return EPERR_NOT_MASTER;

  Set (P_EP_NUM, SAVE, F_MODE_AD);	// siehe _set_ep()
  return epnum += arg;
}

int AddScore(int contributor)
{
  mixed info;

  DEBUG("AddScore called [%O]\n",this_object());
  if (!pointerp(info = NPCMASTER->QueryNPCbyNumber(contributor)))
    return -1;
  DEBUG("AddScore proceeds [%O]\n",this_object());

  if (!previous_object() ||
      ( (explode(object_name(previous_object()),"#")[0] != info[2]) &&
        (object_name(previous_object()) != "/secure/npcmaster")))
    return -2;
  DEBUG("AddScore passed security [%O]\n",this_object());

  if (!stringp(given_scores))
    given_scores = "";

  if (!test_bit(given_scores, contributor)) {
    DEBUG("AddScore succeeded, contributor: %d\n",contributor);
    given_scores = set_bit(given_scores, contributor);
    Set(P_LP, Query(P_LP) + NPC_FACT * info[1]);
    this_object()->force_save();
    return info[1];
  }
  return 0;
}

string QueryScores() {
  return given_scores;
}

// Hier kommen Funktionen fuer die Levelpunkte

/*
int Exp2Lp(int xp)
{
  if (xp <= XP_BASE) return 0;
  if (xp > XP_EXPLIMIT)
    return Exp2Lp(XP_EXPLIMIT)+((xp-XP_EXPLIMIT) / XP_PER_LP);
  else
    return to_int (100*log(to_float(xp+1)/XP_BASE) / log(XP_FACT) );
}
*/
/*
int Exp2Lp(int xp)
{
  if (xp <= XP_BASE) return 0;
  if (xp > XP_EXPLIMIT2 )
     return Exp2Lp(XP_EXPLIMIT2)+((xp-XP_EXPLIMIT2) / XP_PER_LP2 );
  if ((xp > XP_EXPLIMIT) && (xp < XP_EXPLIMIT2))
    return Exp2Lp(XP_EXPLIMIT)+((xp-XP_EXPLIMIT) / XP_PER_LP);
  else
    return to_int (100*log(to_float(xp+1)/XP_BASE) / log(XP_FACT) );
}
*/

int Exp2Lp(int xp)
{
 int _xp;
 float lp;

 lp = 0;

 if (xp <= XP_BASE) return 0;

 // Ueber 100 Mio.
 if (xp > XP_EXPLIMIT2 ) {
    _xp = XP_EXPLIMIT2; // zum weiterrechnen...
     lp = to_float(((xp - XP_EXPLIMIT2) / XP_PER_LP2));
    } else { _xp = xp; }
 // Rest ueber 50 Mio. ?
 if (_xp > XP_EXPLIMIT ) {
      lp = lp + ((_xp - XP_EXPLIMIT) / XP_PER_LP);
     _xp = XP_EXPLIMIT;
     }

 // Rest
 return to_int(lp + (100*log(to_float(_xp + 1) / XP_BASE) / log(XP_FACT)));
}

static int _query_lp()
{
  int ret, val, i, l;

  // Beitrag A: Questpunkte
  // ---
  ret =QP_FACT * QueryProp(P_QP);

  DEBUG("Nach QP: ret = %d\n", ret);

  // Beitrag B: Erfahrungspunkte
  //   Bis 50 Mio. EP haben sie noch hoehere Wirkung (aber
  //   exponentiell abnehmend), aber bei alle drueber gehenden
  //   EP gilt: 25 Mio. EP entsprechen einem Level.
  //   - siehe auch Exp2Lp() weiter oben.
  // ---
  ret += Exp2Lp(QueryProp(P_XP));

  /*
  // Alternative:
  //   Gewichtung der EP evtl. doch level- und nicht nur EP-abhaengig?
  //   Naja - ist wohl erst sinnvoll, wenns mal viele Quests gibt...
  // ---
  val = QueryProp(P_XP);
  l = QueryProp(P_LEVEL);
  if ((l < 20) && (val < XP_EXPLIMIT))
    ret += Exp2Lp(val);
  else
    ret += val / XP_PER_LP;
  */

  DEBUG("Nach XP: ret = %d\n", ret);

  // Beitrag C: Zaubertraenke
  //   Zugewiesene Traenke geben 6 LEP,
  //   nicht zugewiesene (aber gefundene) Traenke 2 LEP
  // ---
  if ((i=10-sizeof(QueryProp(P_POTIONROOMS))) > 0)
    ret += ZTZ_FACT * i;
  ret += ZTN_FACT * QueryProp(P_BONUS_POTIONS);

  DEBUG("Nach ZT: ret = %d\n", ret);

  // Beitrag D: Forscherpunkte
  // ---
  val = QueryProp(P_EP_NUM);
  ret += EP_FACT * val;

  DEBUG("Nach FP: ret = %d\n", ret);

  // Beitrag E: Gildeneinstufung
  //   Maximale Gildeneinstufung (10000) entspricht fuenf Leveln
  // ---
  ret += QueryProp(P_GUILD_RATING) / 20;

  DEBUG("Nach GR: ret = %d\n", ret);

  // Beitrag F: Levelpunkte von Monstern und Miniquests
  //   Diese werden "direkt" gesetzt
  //   Die 100 sind der Grundoffset, da man mit Level 1 das Spiel
  //   beginnt und nicht mit Level 0.
  // ---
  if (!Query("lp:recalced")) {
    Set(P_LP, NPC_FACT * NPCMASTER->Recalculate(given_scores));
    Set("lp:recalced", 1);
  }
  ret += 100 + Query(P_LP);

  DEBUG("Nach 100+LP: ret = %d\n", ret);

  // Na hoffentlich gibt die Summe was positives... :-)
  return (ret > 0) ? ret : 0;

}

