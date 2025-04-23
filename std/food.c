// Tamedhon MUDlib
//
// std/food.c -- standard food object
//
/* $Date: 2009/10/30 12:00:00 $
 * $Revision: 1.5 $
 * $Log: food.c,v $
 *
 * Revision 1.5  2009/10/30  Rommy
 * Pronomen beim Zerfall korrigiert
 *
 * Revision 1.4  2009/09/11  Gralkor
 * Futter ist keine Handelsware
 *
 * Revision 1.3  2003/07/05 13:25:00  Querolin 
 * P_HP_STORE wird anstelle von P_HP angefuttert   
 *
 * Revision 1.2  2000/06/12 09:16:30  Samtpfote
 * essen, dass sich in Spielern oder Raeumen befindet, loest sich nun
 * nach 1-3 Resets von selber auf.
 *
 * Revision 1.1  2000/06/12 09:08:41  Samtpfote
 * Initial revision
 *
 * Revision 1.2  1997/10/07 11:45:11  Woody
 * Ueberarbeitet, nun wesentlich flexibler
 *
 * Revision 1.1  1997/03/13 02:53:57  Woody
 * prepared for revision control
 *
 * 20.10.1995	00:58	Mupfel	P_MAX_FOOD, P_MAX_ALCOHOL selber testen
 * 13.10.1995	19:54	Mupfel	/sys/food.h : F_HEAL statt "heal" etc.
 * 06.09.1995	04:00	Mupfel	erste Version
 */
// 2024-12-31	Gralkor	Bug in second_life gefixt. Returnte 1, was 
//			unlimitiertes Futtern erlaubte. Nun 0. Fiel nicht
//			auf, weil das meist überschrieben wird, aber wenn
//			mal nicht ...
//
//
// Properties:
// P_FOOD_INFO : mapping
// ([ F_HEAL:mixed,       Heilung, int (LP+SP) oder array ({LP, SP})
//    F_SOAK:int,         Saettigung
//    F_DRINK:int,        Trink-Saettigung
//    F_ALCOHOL:int,      Alkoholgehalt
//    F_POISON:int,       Giftgehalt
//    F_MSG:string,       Text beim Essen/Trinken fuer den Spieler
//    F_MSG_ROOM:string,  Text beim Essen/Trinken fuer den Raum
//                        (Spielername+" " wird automatisch vorangestellt)
//    F_VERB:string ])    Verb (wird bei der Fehlermeldung benoetigt);
//                        Default ist "essen" (Verb in Nennform!!)
//
// (F_HEAL und F_POISON wird NACH 'BeimEssen' gesetzt, F_SOAK und
// F_ALCOHOL davor, um abzutesten, ob der Spieler das noch essen kann)
//
// Funktionen:
//
// void BeimEssen();
//   Wird in der 'Essfunktion' aufgerufen. Das Nahrungsobjekt wird
//   anschliessend zerstoert, wenn die Funktion second_life() nicht
//   existiert oder 0 zurueckliefert.
//
// int second_life(object pl);
//   Wenn diese Funktion einen Wert ungleich 0 zurueckgibt, so hat
//   das Objekt ein "zweites Leben" und wird nicht destructed nach
//   der Konsumierung. Hier koennte man auch das Clonen leerer
//   Flaschen, Obstkerne, etc. implementieren.
//
//
// int is_questobject();
//   gibt an, ob es ein Questobjekt ist.
//
// void set_questobject(int state);
//   setzt das 'ist-ein-Questobjekt' Flag und verhindert, dass das 
//   Questobjekt bei einem Reset zerfallen kann (es zerfaellt dennoch
//   nach 15 Resets!
//
//   beim Ruecksetzen auf 0 werden die Anzahl der Resets auf 0 gesetzt.
//
//   Benutzung der Funktion ist vom Questerzmagier zu genehmigen.

inherit "std/thing";

#include <properties.h>
#include <defines.h>
#include <living.h>
#include <wizlevels.h>
#include <food.h>

static int check_food(int strength);
static int check_drink(int strength, int alcohol);

int rc, qo;

int is_questobject() { return qo; }

void set_questobject(int state)
{
	if(qo && state) return;     // kein Multiples aufrufen moeglich
	if(!qo && !state) return;   // dito

	qo=state;
	rc=(qo?15+random(6):random(6)+1);
}

void reset()
{
    object	env;

    rc--;
    env=environment(this_object());
    if(env && (rc <= 0))
    if(interactive(env) || env->id("Raum"))
    {
        if(interactive(env))
        {
            tell_object(env,break_string(
            Name(WER,1)+" wird ungenießbar. Angewidert wirfst Du "+
            QueryPronoun(WEN)+" weg."));
        }
        else
        {
            tell_room(env, Name()+" wird ungenießbar und zerfällt.\n");
        }
        call_out("remove", 0);
    }
    /* nach 1-6 Resets zerfaellt das Essenobjekt, da sonst echt zuviel
     * Zeugs gehortet werden kann bzw es zulange herumliegt. Essen in NPCs
     * loesen sich nicht auf.
     *
     * das sollte dazu animieren, das Essen gleich einzusetzen und nicht
     * lange zu bunkern. - Samtpfote@Tamedhon.
     */
}

varargs void create(string commands)
{
  if (IS_BLUE(ME)) return;
  ::create();
  rc=random(6)+1;
  AddId("Nahrung");
  SetProp(P_SHORT, "Ein essbares Objekt");
  SetProp(P_LONG, "Ein essbares Objekt.\n");
  SetProp(P_NAME, "Essen");
  SetProp(P_GENDER, NEUTER);
  SetProp(P_WEIGHT, 50);
  SetProp(P_VALUE, 50);
  SetProp(P_MATERIAL, MAT_FOOD);
  SetProp(P_NOBUY, 1);
  SetProp(P_NOSELL,"Händler handeln nicht mit Nahrungsmitteln.\n");
  
  if (pointerp(commands) || stringp(commands))
    AddCmd(commands, "mampf");
  else
    AddCmd(({"iss", "esse"}), "mampf");
}

void BeimEssen() {}

int second_life(object player) 
{
 return 0;
}

int mampf(string what)
{
  mapping foodinfo;
  mixed   heal;
  int     hp, sp;

  foodinfo = QueryProp(P_FOOD_INFO);
  notify_fail("Was möchtest Du "+(foodinfo[F_VERB] || "essen")+"?\n");
  if (!what) return 0;
  if (id(what))
  {
    // P_FOOD_INFO auswerten:
    if (pointerp(heal=foodinfo[F_HEAL]) && sizeof(heal)==2)
    { hp = heal[0]; sp = heal[1]; }
    else
      hp = sp = heal;
    if (foodinfo[F_SOAK] && !check_food(foodinfo[F_SOAK]))
      return 1;
    if (foodinfo[F_DRINK] || foodinfo[F_ALCOHOL])
      if (!check_drink(foodinfo[F_DRINK], foodinfo[F_ALCOHOL]))
          return 1;

    if (foodinfo[F_MSG])
      write( break_string(foodinfo[F_MSG], 78, 0, 1) );
    if (foodinfo[F_MSG_ROOM])
      say( break_string(PL->Name(WER)+" "+
          foodinfo[F_MSG_ROOM], 78, 0, 1) );
    BeimEssen();

    if (hp) PL->SetProp(P_HP_STORE, PL->QueryProp(P_HP_STORE)+hp);
    if (sp) PL->SetProp(P_SP_STORE, PL->QueryProp(P_SP_STORE)+sp);
    if (foodinfo[F_POISON])
      PL->AddPoison(foodinfo[F_POISON]);

    if (!ME->second_life(PL))
      remove();
    return 1;
  }
  return 0;
}

static int check_food(int strength)
{
  int food;

  food = PL->QueryProp(P_FOOD) + strength;
  if ((food > PL->QueryProp(P_MAX_FOOD)) && !IS_LEARNER(PL))
  {
    tell_object(PL, "Uff, das bekommst Du nicht mehr 'runter.\n");
    return 0;
  }
  PL->SetProp(P_FOOD, (food < 0 ? 0 : food));
  return 1; 
}

static int check_drink(int strength, int alcohol)
{
  int drink, alc;

  drink = PL->QueryProp(P_DRINK) + strength;
  alc = PL->QueryProp(P_ALCOHOL) + alcohol;

  if (!IS_LEARNER(PL))
  {
    if (alc > PL->QueryProp(P_MAX_ALCOHOL))
    {
      tell_object(PL,
          "Asso das kriegst Du jezz nich... nichmehr runter, hick.\n");
      return 0;
    }
    if (drink > PL->QueryProp(P_MAX_DRINK))
    {
      tell_object(PL,
          "Soviel kannst Du beim besten Willen nicht mehr trinken...\n");
      return 0;
    }
  }
  PL->SetProp(P_DRINK, (drink < 0 ? 0 : drink));
  PL->SetProp(P_ALCOHOL, (alc < 0 ? 0 : alc));
  return 1; 
}
