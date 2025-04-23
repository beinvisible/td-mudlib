// Tamedhon MUDlib
//
// std/pub.c -- Standard Kneipe
//
// $Date: 2012/04/15 23:00:00 $
/* $Revision: 3.11 $
 * $Log: pub.c,v $
 *
 * Revision 3.11  2012/04/15 23:00:00  Grynnis
 * Im Falle von P_NO_ASCII_ART Filtermoeglichkeite "menue speisen|getraenke"
 * eingebaut um Scroll zu vermeiden bei Sprachausgabe
 *
 * Revision 3.10  2012/04/03 19:00:00  Grynnis
 * Menueausgabe angepasst an P_NO_ASCII_ART
 *
 * Revision 3.9  2003/07/05 13:20:00  Querolin
 * P_HP_STORE anstelle von P_HP wird nun angefuttert
 *
 * Revision 3.8  2001/06/03 13:40:00  Querolin
 * Einbau von Farben im ansi-Mode bei 'menue'
 *
 * Revision 3.7  1998/01/19 15:53:53  Feyaria
 * Goldstuecke -> Muenzen, besseres notify_fail beim bestellen
 *
 * Revision 3.6  1998/01/17 00:41:20  Woody
 * Automatisches Pub-Keeper Handling (P_PUB*) implementiert
 *
 * Revision 3.5  1998/01/02 20:00:51  Angrox
 * Funktion drink_drink aufgeteilt in drink_drink und drink_it
 *
 * Revision 3.4  1997/10/18 01:17:00  Woody
 * Heilung differenzierter spezifierbar (via Array)
 *
 * Revision 3.3  1997/10/07 09:23:50  Woody
 * drink_drink() verwenden, diverse Typofixes
 *
 * Revision 3.2  1997/06/25 11:30:13  Woody
 * Exploration Points ermoeglicht
 *
 * Revision 3.1  1997/04/02 01:06:02  Woody
 * sprintf() fuer 'menue', ILLEGAL logging bei h<3*(s+r) (statt 2* bisher),
 * ignoriere spendiere[.alkohol|.essen|.getraenke],
 * AddFood() bei Bedarf wieder MG-kompatibel (varargs)
 *
 * Revision 3.0  1997/03/13 03:08:53  Woody
 * prepared for revision control
 *
 * Revision 2.20  05-06-1996  Woody
 * neue Methode PayIn()
 *
 * Revision 2.19  13-01-1996  Mupfel (Idee von Silvana)
 * Zusaetzliche Platzhalter &?, &1, &2, &3, &4, &=, &#
 *
 * Revision 2.18  1995/07/17  14:06:31  Wargon
 * Falls bei AddFood/AddDrink Funktionen angegeben werden, bekommen diese den
 * Zahler und den Empfaenger als Parameter uebergeben.
 *
 * Revision 2.17  1995/07/10  17:26:31  Jof
 * Only call bank if sum!=0 to keep logs clean
 *
 * Revision 2.16  1995/07/10  07:29:39  Jof
 * use bank.h
 *
 * Revision 2.15  1995/07/05  18:47:38  Jof
 * Ueberschuesse an Bank abfuehren :)
 *
 * Revision 2.14  1995/07/04  16:29:51  Rochus
 * Klammerung verbessert.
 *
 * Revision 2.13  1995/07/04  15:46:02  Rochus
 * Kneipen werden beim Routing-Daemon registriert,
 * damit NPCs auch dorthin finden :-)
 *
 * Revision 2.12  1995/06/06  08:42:44  Jof
 * _notify_fail
 *
 * Revision 2.12  1995/06/05  14:56:00  Jof
 * _notify_fail used
 *
 * Revision 2.11  1995/02/06  19:44:14  Jof
 * RCS-Logs aufgeraeumt (leere Logs raus usw)
 *
*/


/* JOF 1992

   Von pub.c neu eingefuehrte Funktionen:

   AddFood(name,ids,preis,heilkraft,saettigung,beimEssen);

   name:      Name der Speise in der Speisekarte
   ids:       IDs unter denen der Player die Speise ansprechen kann
   (Format: ({ id1, ..., idn }))
   zB:  ({ "altbier", "bier", "alt" });
   preis:     Preis der Speise
   heilkraft: Speise gibt direkt <heilkraft> Hitpoints
   saettigung:Speise saettigt <xx>
   beimEssen: endweder ein Funktionsname oder ein String.
   Funktionsname: zB: AddFood(....,"meineEssFkt");
   wenn nun der player die Speise isst, wird diese Fkt aufgerufen.
   Geldabbuchung, Hitpoints usw werden automatisch verwaltet.
   Dieses Feature dient dazu, besondere Aktionen stattfinden
   zu lassen, wenn der player die Speise isst. (Beispiel: Wenn
   es sich um Knoblauch handeln wuerde, koennte man dem player
   ein Mundgeruchsobjekt geben.)
   Als Parameter werden der Spender (gleichzeitig this_player())
   und der Empfaenger uebergeben.
   String: AddFood(....,({"messageFuerDenPlayer",
   messageFuerDieAndereninSeinemRaum"}));
   Ein "&&" in dem String wird durch den Playernamen
   ersetzt, ein "&?" durch den Playernamen im Genitiv.
   Ein "&!" in dem String wird durch das Personalpronomen
   fuer den Player ersetzt (Nominativ).
   Zusaetzlich werden "&1", "&2", "&3" und "&4" durch das Personalpronomen
   des Players im Nominativ, Genitiv, Dativ und Akkusativ ersetzt,
   ausserdem "&=" durch das Possessivpronomen des Players im Singular ('sein',
   'ihr') ersetzt und "&#" im Plural ('seine', 'ihre').
   ----------------------------------------------------------------------------
   AddDrink(name,ids,preis,heilkraft,staerke,saettigung,beimTrinken);

   staerke:     Wie stark die intoxination geaendert wird;
   saettigung:  Wie stark soak geandert wird;
   beimTrinken: Wie beimEssen in AddFood;
   Rest wie bei AddFood

   */

inherit "/std/room";
#include <rooms.h>
#include <properties.h>
#include <routingd.h>
#include <bank.h>
#include <exploration.h>
#include <pub.h>
#include <ansi.h>

#define DBS(x)		if(find_player("samtpfote")) \
			    tell_object(find_player("samtpfote"), x)

static mixed *food;
static mixed *drink;
int sum;

private static string *explore;
/* Prototypen fuer Funktionen */
string mess(string str, object pl);
int drink_drink(mixed drink, object zahler, object empfaenger);
int eat_food(mixed food,object zahler,object empfaenger);
int search_what(string str, object zahl,object sauf);
int drink_it(mixed drink, object zahler, object empfaenger);
void PayIn(int betrag);

int isPub() { return 1; }

mixed query_food() {
  return food;
}

mixed query_drink() {
  return drink;
}

void create()
{
  object router;

  room::create();
  SetProp(P_LIGHT, 1);
  SetProp(P_INDOORS, 1);
  SetProp(P_INT_SHORT, "Eine Kneipe");
  SetProp(P_INT_LONG,
      "Der Magier, der diesen Pub erschuf, war leider "
      "zu faul, eine Beschreibung dafür herzustellen.");

  food=({});
  drink=({});
  explore = EPMASTER->QueryExplore(EP_PUB);

  AddCmd( ({"menue", "menü"}), "menue" );
  AddCmd( ({ "kauf", "kaufe", "bestell", "bestelle" }), "bestelle" );
  AddCmd( ({ "spendier", "spendiere" }), "spendiere" );
//  AddItem("/obj/topliste", REFRESH_REMOVE);
  if (!clonep(this_object()) && objectp(router=find_object(ROUTER)))
    router->RegisterTarget(TARGET_PUB,object_name(this_object()));
}

varargs void AddFood (string nameOfFood, mixed ids, int price,
                      mixed heal, mixed soak, mixed myFunction)
{
  mixed *newfood;
  int avgheal;

  if (!nameOfFood || !ids || !price)        // erforderlich: Name, IDs, Preis
    return;                                 // Heilwirkung darf auch 0 sein!
  if (!pointerp(heal))
    heal = ({ heal,heal,0 });         // Default: Heilung = LP und MP
  else
    heal = (heal + ({0,0,0})) [0..2]; // Array gegebenenfalls mit 0 fuellen
  avgheal = (heal[0] + heal[1]) / 2 + heal[2] / (heal[0] && heal[1] ? 1 : 2);
  if (!intp(soak) && !myFunction)     // backward compatibility...
  {
    myFunction=soak;
    soak = (heal[0]+heal[1])/2+heal[2];
  }
  if (!pointerp(ids))
    ids = ({ ids });
  log_file("FOOD"+(avgheal > 3*soak ? "_ILLEGAL" : ""), sprintf(
      "%s: (%s) Heal: %s, Soak: %d\n",
      object_name(this_object()), nameOfFood,
      implode(map(heal, #'to_string), "/"), soak));
  newfood = ({ nameOfFood,ids,price,heal,soak,myFunction });
  food += ({ newfood });
}

varargs void AddDrink (string nameOfDrink, mixed ids, int price,
                       mixed heal, int strength, int soak, mixed myFunction)
{
  mixed *newdrink;
  int avgheal;

  if (!nameOfDrink || !ids || !price) return;
  if (!pointerp(heal))
    heal = ({ heal,heal,0 });         // Default: Heilung = LP und MP
  else
    heal = (heal + ({0,0,0})) [0..2]; // Array gegebenenfalls mit 0 fuellen
  avgheal = (heal[0] + heal[1]) / 2 + heal[2] / (heal[0] && heal[1] ? 1 : 2);
  if (!pointerp(ids)) ids = ({ ids });
  log_file("DRINKS"+(avgheal > 3*(strength+soak) ? "_ILLEGAL" : ""), sprintf(
      "%s: (%s) Heal: %s, Strength: %d, Soak: %d\n",
      object_name(this_object()), nameOfDrink,
      implode(map(heal, #'to_string), "/"), strength, soak));
  newdrink = ({ nameOfDrink,ids,price,heal,strength,soak,myFunction });
  drink += ({ newdrink });
}

int menue(string str) {
  int i, fd;
  string cgreen, cwhite, cbold, cnorm, cred, alk;

  if (this_player()->QueryProp(P_TTY)=="ansi")
  { cgreen=ANSI_GREEN, cnorm=ANSI_NORMAL; cbold=ANSI_BOLD;
    cred=ANSI_RED; }
  else
  { cgreen=""; cnorm=""; cbold=""; cred=""; }

  if (this_player()->QueryProp(P_NO_ASCII_ART))
  {
    if (!str) fd=3;
    else if (str == "essen" || str == "speisen") fd=2;
    else if (str == "trinken" || str == "getraenke" || str == "getränke") fd=1;
    else fd=3;
    if ((fd&2) && sizeof(food)>0)
    {
      write("\nSpeisekarte mit Preisen\n");
      for (i=0; i<sizeof(food); i++)
        printf("%-50.50s  %5.5d\n", food[i][0], food[i][2]);
    }
    if (fd&1)
    {
      write("\nGetränkekarte mit Preisen und Alkoholangabe\n");
      for (i=0; i<sizeof(drink); i++)
        printf("%-50.50s  %5.5d  %s\n",
          drink[i][0], drink[i][2], (drink[i][4] <= 0) ? "" : "mit Alk");
    }
  }
  else
  {
    if (sizeof(food)>0)
    {
      write("\n"+cbold+"Getränke                     Preis Alk "+cnorm+cgreen+
            "| "+cnorm+cbold+"Speisen                          Preis\n"+cnorm);
      write(cgreen+"---------------------------------------+-"
          "--------------------------------------\n"+cnorm);
      for (i=0; i<sizeof(drink) || i<sizeof(food); i++) {
        if(i<sizeof(drink))
        {
          alk=((drink[i][4] <= 0) ? "N" : cred+"J");
          printf(cnorm+"%-29.29s"+cbold+"%5.5d"+cnorm+"  %s  "+cgreen+"| "+cnorm,
              drink[i][0], drink[i][2],alk);
//              (drink[i][4] <= 0) ? cnorm+'N' : cred+'J');
        }
        else
          write(cgreen+"                                       | "+cnorm);
        if (i<sizeof(food))
          printf("%-33.33s"+cbold+"%5.5d"+cnorm,food[i][0], food[i][2]);
        write("\n");
      }
    }
    else
    {
      write("Verfügbare Getränke                                             "
          "Preis  Alk.\n");
      write("----------------------------------------------------------------"
          "-----------\n");
      for (i=0; i<sizeof(drink); i++)
        printf("%-62.62s  %5.5d  %s\n",
            drink[i][0], drink[i][2], (drink[i][4] <= 0) ? "Nein" : "Ja");
    }
  }
  return 1;
}

varargs int CheckKeeper(object wer, int inv_flag)
{
  int fl;
  object k;

  fl=QueryProp(P_PUB_FLAGS);
  if (!(fl & PUB_NEEDSKEEPER)) return 1;
  if (!(k=present(QueryProp(P_PUBKEEPER_ID))))
    return write(QueryProp(P_PUBMSG_NOKEEPER) ||
        capitalize(QueryProp(P_PUBKEEPER_NAME))+" ist leider nicht da.\n"), 0;
  if (!(fl & PUB_SERVEENEMIES) && k->IsEnemy(wer))
    return write(QueryProp(inv_flag ? P_PUBMSG_NOINVITE : P_PUBMSG_NOENEMY) ||
        k->Name()+" weigert sich, "+
        k->QueryPossPronoun(MALE,WEN,PLURAL)+" Gegner zu bedienen.\n"), 0;
  if (!(fl & PUB_SERVESINFIGHT) && k->InFight())
    return write(QueryProp(P_PUBMSG_FIGHTING) ||
        k->Name()+" ist zu beschäftigt, um Dich zu bedienen.\n"), 0;
  return 1;
}

int spendiere(string str)
{
  string *tmp;
  object pl;
  int i,found;

/* Waers nicht sinnvoll, wenn man im Kampf nix spendieren kann ?
   if (this_player()->InFight()) {
    write("Dazu bist Du im Moment viel zu beschaeftigt.\n");
    return 1;
  }
*/
  _notify_fail("Wem willst Du was spendieren?\n"
      "Syntax: spendiere <spieler> <drink>\n");
  if (!str || str=="") return 0;
  tmp=explode(str," ");
  if (sizeof(tmp)<2) return 0;
  if ((!(pl=present(tmp[0],this_object())) &&
       !(pl=present(tmp[0],this_player()))) ||
      !living(pl))
  {
    write("Das Lebewesen ist nicht hier...\n");
    return 1;
  }
  if (this_player() == pl)
  {
    write("Wenn Du Dir selbst was gönnen willst, "
        "verwende bitte \"bestelle\".\n");
    return 1;
  }
/* Waers nicht sinnvoll, wenn man einem Kaempfenden nix spendieren kann ?
   if (pl->InFight()) {
    write(pl->Name()+" ist im Moment viel zu beschaeftigt.\n");
    return 1;
  }
*/
  if (!CheckKeeper(pl,1)) return 1;
  str=lower_case(implode(tmp[1..]," "));
  return search_what(str,this_player(),pl);
}

int bestelle(string str)
{
  object k;
  string prefix;

/* Waers nicht sinnvoll, wenn man im Kampf nichts bestellen kann ?
  if (this_player()->InFight()) {
    write("Dazu bist Du im Moment viel zu beschaeftigt.\n");
    return 1;
  }
*/
  _notify_fail("Was willst Du bestellen?\n");
  if (!str || str=="") return 0;
  if (!CheckKeeper(this_player())) return 1;
  if (QueryProp(P_PUB_FLAGS) & PUB_HASKEEPER) {
    if (k=present(QueryProp(P_PUBKEEPER_ID)))
      prefix=k->Name()+" sagt: ";
    else
      prefix=capitalize(QueryProp(P_PUBKEEPER_NAME))+" sagt: ";
    _notify_fail(break_string("Tut mir leid, "+(str ? capitalize(str) : "das")+
        " führen wir nicht! Wir sind ein anständiges Lokal!", 78, prefix));
  }
  else
    _notify_fail((str?capitalize(str):"Sowas")+" führt dieses Lokal nicht.\n");
  if (!str) return 0;
  return search_what(str,this_player(),this_player());
}

int search_what(string str, object zahl,object sauf)
{
  int i,found;

  str = lower_case(str);
  i=0;found=-1;
  while (i<sizeof(drink) && found<0){
    found=member(drink[i][1],str);
    i++;
  }
  if (found>=0) return drink_drink(drink[i-1],zahl,sauf);
  if (!food||sizeof(food)==0) return 0;
  i=0;
  while (i<sizeof(food)&&found<0){
    found=member(food[i][1],str);
    i++;
  }
  if (found>=0) return eat_food(food[i-1],zahl,sauf);
}

int drink_drink(mixed drink, object zahler, object empfaenger)
{
  int price, soak, strength;
  mixed res;

  price=drink[2];
  strength=drink[4];
  soak=drink[5];
  if (!zahler) zahler=this_player();
  if (!empfaenger) empfaenger=this_player();
  if ((zahler!=empfaenger) && pointerp(res=empfaenger->QueryProp(P_IGNORE))) {
    if (member(res,"spendiere")>=0 ||
        member(res,"spendiere.getränke")>=0 ||
        ((member(res,"spendiere.alkohol")>=0) && (strength>0))) {
      tell_object(zahler, empfaenger->Name(WER)+" will das nicht.\n");
      return 1;
    }
  }
  if (zahler->QueryMoney()<price) {
    tell_object(zahler,"Das kostet "+price+
        " Münzen, und Du hast nicht so viel!\n");
    return 1;
  }
  if (empfaenger!=zahler)
  {
    say(zahler->Name()+" spendiert "+empfaenger->name(WEM)+" einen Drink.\n",
        ({zahler, empfaenger}));
    tell_object(empfaenger,zahler->Name()+" spendiert Dir einen Drink.\n");
    tell_object(zahler,"Du spendierst "+empfaenger->name(WEM)+
        " einen Drink.\n");
  }

  if ((soak || strength) && !empfaenger->drink_drink(soak, strength))
    return 1;
  zahler->AddMoney(-price);
  // sum+=price;
  PayIn(price);
  return drink_it(drink, zahler, empfaenger);
}

int drink_it(mixed drink, object zahler, object empfaenger)
{
  int soak, strength;
  mixed heal, res;

  heal=drink[3];
  strength=drink[4];
  soak=drink[5];

  if (heal[0]==heal[1]) {
    //empfaenger->heal_self(heal[0]+random(heal[2]));
      empfaenger->SetProp(P_HP_STORE,
          empfaenger->QueryProp(P_HP_STORE)+heal[0]+random(heal[2]));
      empfaenger->SetProp(P_SP_STORE,
          empfaenger->QueryProp(P_SP_STORE)+heal[0]+random(heal[2]));
  }
  else {
    if (heal[0])
      empfaenger->SetProp(P_HP_STORE,
          empfaenger->QueryProp(P_HP_STORE)+heal[0]+random(heal[2]));
    if (heal[1])
      empfaenger->SetProp(P_SP_STORE,
          empfaenger->QueryProp(P_SP_STORE)+heal[1]+random(heal[2]));
  }

  if (explore && (zahler == empfaenger) &&
      (sizeof(explore & drink[1]) == sizeof(explore)))
    EPMASTER->GiveExplorationPoint(drink[1][0]);

  if (closurep(drink[6]))
    funcall(drink[6], zahler, empfaenger);
  else {
    if (stringp(drink[6])&&function_exists(drink[6],this_object()))
      call_other(this_object(), drink[6], zahler, empfaenger);
    else {
      tell_object(empfaenger,
          break_string(mess(drink[6][0],empfaenger), 78, 0, 1));
      say(break_string(mess(drink[6][1],empfaenger), 78, 0, 1),
          empfaenger);
      if ((empfaenger!=zahler) && (zahler==this_player()))
        tell_object(zahler,
            break_string(mess(drink[6][1],empfaenger), 78, 0, 1));
    }
  }
  return 1;
}

int eat_food(mixed food,object zahler,object empfaenger)
{
  int price, soak;
  mixed heal, res;

  price=food[2];
  heal=food[3];
  soak=food[4];
  if (!empfaenger) empfaenger=this_player();
  if (!zahler) zahler=this_player();
  if ((zahler!=empfaenger) && pointerp(res=empfaenger->QueryProp(P_IGNORE))) {
    if (member(res,"spendiere")>=0 ||
        member(res,"spendiere.essen")>=0) {
      tell_object(zahler, empfaenger->Name(WER)+" will das nicht.\n");
      return 1;
    }
  }
  if (zahler->QueryMoney() < price)
  {
    tell_object(zahler, "Das kostet "+price+" Münze"+
        (price==1 ? "" : "n")+", soviel hast Du aber leider nicht!\n");
    return 1;
  }
  if (empfaenger!=zahler)
  {
    say(zahler->Name()+" spendiert "+empfaenger->name(WEM)+" was zu essen.\n",
        ({zahler, empfaenger}));
    tell_object(empfaenger,zahler->Name()+" spendiert Dir was zu essen.\n");
    tell_object(zahler,"Du lädst "+empfaenger->name(WEN)+" zum Essen ein.\n");
  }
  if (!empfaenger->eat_food(soak))
  {
    tell_object(empfaenger, "So viel kannst Du nicht essen!\n");
    return 1;
  }
//  tell_object(find_player("querolin"),
//   break_string(" HP_STORE:"+this_player()->QueryProp(P_HP_STORE)+"\n",78));

  if (heal[0]==heal[1]) {
    //empfaenger->heal_self(heal[0]+random(heal[2]));
      empfaenger->SetProp(P_HP_STORE,
          empfaenger->QueryProp(P_HP_STORE)+heal[0]+random(heal[2]));
      empfaenger->SetProp(P_SP_STORE,
          empfaenger->QueryProp(P_SP_STORE)+heal[0]+random(heal[2]));
  }
  else {
    if (heal[0])
      empfaenger->SetProp(P_HP_STORE,
          empfaenger->QueryProp(P_HP_STORE)+heal[0]+random(heal[2]));
    if (heal[1])
      empfaenger->SetProp(P_SP_STORE,
          empfaenger->QueryProp(P_SP_STORE)+heal[1]+random(heal[2]));
  }

//  tell_object(find_player("querolin"),
//   break_string(" HP_STORE:"+this_player()->QueryProp(P_HP_STORE)+"\n",78));

  zahler->AddMoney(-price);

  if (explore && (zahler == empfaenger) &&
      (sizeof(explore & food[1]) == sizeof(explore)))
    EPMASTER->GiveExplorationPoint(food[1][0]);

  // sum+=price;
  PayIn(price);
  if (closurep(food[5]))
    funcall(food[5], zahler, empfaenger);
  else {
    if (stringp(food[5])&&function_exists(food[5],this_object()))
      call_other(this_object(),food[5],zahler, empfaenger);
    else {
      tell_object(empfaenger,
          break_string(mess(food[5][0],empfaenger), 78, 0, 1));
      say(break_string(mess(food[5][1],empfaenger), 78, 0, 1),
          empfaenger);
      if ((empfaenger!=zahler) && (zahler==this_player()))
        tell_object(zahler,
            break_string(mess(food[5][1],empfaenger), 78, 0, 1));
    }
  }
  return 1;
}


// Name = Mupfel
// && -> 'Mupfel', &? -> 'Mupfels'
// &! -> 'er' (= &1)
// &2 -> 'ihn'
// &3 -> 'ihm'
// &4 -> 'ihn'
// &= -> 'sein'     > besessenes Objekt ist m oder n, 1. Fall, Singular
// &# -> 'seine'    >   - " - ist f, 1. Fall, Singular

string mess(string str, object pl)
{
  if (!pl)
    pl=this_player();
  if (!str || str=="") return str;
  str=implode(efun::explode(str,"&&"),pl->name(WER,2));
  str=implode(efun::explode(str,"&?"),pl->name(WESSEN,2));
  str=implode(efun::explode(str,"&!"),pl->QueryPronoun(WER));
  str=implode(efun::explode(str,"&1"),pl->QueryPronoun(WER));
  str=implode(efun::explode(str,"&2"),pl->QueryPronoun(WESSEN));
  str=implode(efun::explode(str,"&3"),pl->QueryPronoun(WEM));
  str=implode(efun::explode(str,"&4"),pl->QueryPronoun(WEN));
  str=implode(efun::explode(str,"&="),pl->QueryPossPronoun(1));
  str=implode(efun::explode(str,"&#"),pl->QueryPossPronoun(2));
  return capitalize(str);
}

void PayIn(int betrag)
{
  sum+=betrag;
  /*
  DBS(sprintf("** PUB(%O): sum=%i\n",
	this_object(), sum));
  */

  if(sum > 30000)
  {
    DBS(sprintf("** PUB(%O): ZBank einzahlen: %i\n",
	this_object(), sum));
    ZENTRALBANK->PayIn(sum);
    sum=0;
  }
}

