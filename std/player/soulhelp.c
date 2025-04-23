// Tamedhon MUDlib
//
// std/player/soulhelp.c -- Verb-Hilfe
//
// $Date: 2002/02/27 09:00:00 $
/* $Revision: 3.3 $
 * $Log: soulhelp.c,v $
 * Revision 3.3  2002/02/27 09:00:00  Querolin
 * Neue Verben eingetragen
 *
 * Revision 3.2  1998/01/21 14:55:47  Feyaria
 * Neue Verben eingetragen
 *
 * Revision 3.1  1997/03/31 04:43:35  Woody
 * neue Verben eingetragen
 *
 * Revision 3.0  1997/03/13 01:49:13  Woody
 * prepared for revision control
 *
 * SL-Version: one new verb
 * -Crea-
 *
 * new AnderLand version by -Creative- 1995
 * adapted 2 remote-cmds, and 6 own cmds.
 * special mags may use another important cmd! :)
 */

#include <wizlevels.h>

string *wizcmds, *plrcmds, *ghostcmds;

create()  {
  plrcmds=({
        "zuck", "schmieg", "antworte", "applaudiere",
        "betaste", "cls", "drehe (däumchen)", "danke",
        "drücke", "erröte", "flippe", "frage",
      	"furze", "gähne", "gluckse", "grinse",
        "guck", "hätschel", "hickse", "hüpfe",
        "huste", "keuche", "kichere", "klatsche",
        "knabbere", "knickse", "knirsche", "knurre",
        "knutsche", "kotze", "küsse", "lache",
        "lächle", "liebe", "nicke", "niese",
        "gib", "pfeife", "rülpse", "runzle",
        "schmolle", "schmunzle", "schnippe", "schnarche",
        "schnurre", "schreie", "schüttle", "seufze",
        "singe", "sniefe/schniefe", "spucke", "starre",
        "staune", "stiere", "stöhne", "stoße",
        "streichle", "tanze", "tritt", "tröste",
        "umarme", "verneige", "wackle", "wälze",
        "weine", "winke", "zwinkere", "verben",
        "ächze", "erbleiche", "fluche", "verfluche",
        "kitzle", "nein", "deute", "zeige",
        "denke [text]", "knuddle", "tätschel", "wuschel",
        "bäh / strecke ... [die] zunge raus", "kratz",
        "grummel", "jubel / juble ... [zu]", "würg",
        "gratuliere / beglückwünsche", "räusper",
        "argl", "rotfl", "grunz", "kuschel", "atme ... auf",
        "freue", "sniff", "grüble", "bohre ... [in der nase]",
        "putze [nase]", "bibbere", "quietsche/quieke", "schluchze",
        "schnaufe", "schnaube", "philosophiere", "sabbere",
        "stimme [...] zu", "krieche", "mustere", "schäme",
        "schnalze ... [zunge]", "stampfe ... [auf]",
        "reibe ... [hände]", "fauche ... [an]",
        "beneide", "wische ... [schweiß]", "lag", "idle",
        "rgrinse", "rgratuliere / rgratse", "träume",
        "baue [luftschloss]", "betrachte", "blicke [ins leere]",
        "prust", "begrüße / hallo / hi", "entschuldige",
        "erschrecke", "ärgere", "lobe", "kuck", "schweige",
        "zupfe", "warte", "wundere",
        "bedauer", "beruhige", "bettel", "glubsche", "gröhle",
        "hänsel", "hechel", "höre .. [zu]", "hüstel", "jaule",
        "kneife", "knie nieder", "knuffe", "krabbel",
        "kraule", "kreische", "mecker", "nerve", "quengel",
        "rolle", "rüttel", "schiele", "schmachte", "schmeichel",
        "schmuse", "schneuze", "springe", "strahle", "stubse",
        "taumel", "torkel", "klopfe .. [auf schulter]",
        "jammere", "haue", "friere", "murmel", "trällere",
        "gacker", "schlucke", "piepse", "fuchtel", "stolziere",
        "rümpfe", "röchel", "japse", "brumme", "bitte",
        "fürchte", "spanne ..[Muskeln an]", "ermuntere", 
        "verblüffe", "beschimpfe","wünsche", "stolpere",
        "flausche", "beiße", "genieße", "summe", "wimmer", "schubse",
        "rdanke", "rwuschel", "rnicke", "rlächel", "rtröste",
	});
	wizcmds=({
		"splash", "flame",
	});
	ghostcmds=({
		"rassel/rassle", "heule",
	});
}

Help()  {
  string out;
  out = break_string(
      "Standardverben:\n"+implode(SortIt(plrcmds), ", ")+".", 78, 0, 1);
  if (!this_player()) return out;
  if (IS_WIZARD(this_player()))
    out += "\n"+break_string(
        "Magierverben:\n"+implode(SortIt(wizcmds),", ")+".", 78, 0, 1);
  if (this_player()->ghost())
    out += "\n"+break_string(
        "Geisterverben:\n"+implode(SortIt(ghostcmds), ", ")+".", 78, 0, 1);
  if (IS_ARCH(this_player()))
    out+="\n -"+capitalize(getuid(this_player()))+"-'s Verben:   schwapp :)";
  return out+"\n"+break_string(
      "Einige Befehle reagieren auch auf leicht unterschiedliche "
      "Schreibweise. Diese Befehlsliste ist dynamisch und passt sich "
      "Deinem Status an.", 78);
}

static SortIt(arr)  {
  int i;
  mixed *sort_arr;
  string *ix;
  sort_arr=({ ({}), ({}) });
  ix=arr;
  for (i=sizeof(ix)-1; i>=0; i--)  {
    string tmp1,tmp2;
    sort_arr[0]+=({ ix[i][2]+ix[i][1]*256+ix[i][0]*65536 });
    sort_arr[1]+=({ ix[i] });
  }
  sort_arr = order_alist(sort_arr);
  sort_arr = sort_arr[1];
  return sort_arr;
}

