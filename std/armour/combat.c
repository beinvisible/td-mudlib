// Tamedhon MUDlib
//
// ARMOUR/COMBAT.C -- armour standard object
//
// $Date: 2009/08/23 00:30:00 $
/* $Revision: 1.3 $
 * $Log: combat.c,v $
 *
 * Revision 1.3  2009/08/23 00:30:00  Grynnis
 * WearFunc bzw. RemoveFunc der Ruestung werden nun als letztes aufgerufen
 * nach allen anderen Checks, sonst Problem bei MIs, wenn das MI Attribute
 * aendert, trotz eventuellem Verbot durch spaeteren Check
 *
 * Revision 1.2  2002/22/07 17:02:05  Serii
 * Aufruf von allow_wear() in der jeweiligen Gilde, in DoWear()
 *
 * Revision 1.1  2000/06/12 10:08:02  Samtpfote
 * Initial revision
 *
 * Revision 3.16  1997/11/05 16:10:51  Woody
 * Neue Defaults fuer den Kaelte/Waermeschutz
 *
 * Revision 3.15  1997/10/09 00:44:25  Woody
 * _set_ac() ueberarbeitet, Check gegen zu hohe P_AC rein, P_QUALITY_DECAY=100
 *
 * Revision 3.14  1997/10/08 13:29:15  Woody
 * P_TOTAL_TEMPERATURE_PROTECTION berechnen, Defaultwerte fuer Temp-Schutz
 *
 * Revision 3.13  1997/09/25 15:04:15  Woody
 * Materialsetzen hier rausgenommen
 *
 * Revision 3.12  1997/08/25 15:30:06  Feyaria
 * quality_decay, dam_protection
 *
 * Revision 3.11  1997/06/25 11:47:18  Woody
 * P_NR_HANDS auswerten, UseHands()/FreeHands() verwenden
 *
 * Revision 3.10  1997/06/22 22:36:47  Woody
 * P_REMOVE_FUNC nicht in this_player() sondern in worn_by suchen...
 *
 * Revision 3.9  1997/05/27 10:04:30  Feyaria
 * 2. Ring tragen, Wear und Removefunc auf silent, Bug beim Schild tragen weg
 *
 * Revision 3.8  1997/05/17 16:14:25  Woody
 * Aufruf der Blueprint des Restriction Checkers (statt inherit)
 *
 * Revision 3.7  1997/05/12 11:38:00  Feyaria
 * materialien fuer belt und bracers
 *
 * Revision 3.6  1997/04/30 09:58:00  Feyaria
 * nur mehr ein gleiches aAT_MISC anziehen
 *
 * Revision 3.5  1997/04/19 20:33:40  Feyaria
 * P_WEAR_RESTRICTIOPNS eingefuehrt
 *
 * Revision 3.4  1997/03/25 12:56:36  Woody
 * worn_by zieht eine Ruestung aus, nicht this_player() *argl*
 *
 * Revision 3.3  1997/03/20 18:25:34  Woody
 * Spielername bei P_(UN)WEAR_TEXT nicht schon im init() einfuegen
 *
 * Revision 3.2  1997/03/17 12:12:58  Woody
 * P_WEAR_FUNC und P_REMOVE_FUNC auch im Spielerobjekt aufrufen
 * (kann entweder auf ein object oder auf ein array of objects
 * verweisen, destructed objects werden dabei automatisch entsorgt)
 *
 * Revision 3.1  1997/03/14 14:41:23  Woody
 * _set_ac() eingefuehrt, setzt auch P_TOTAL_AC gegebenenfalls
 *
 * Revision 3.0  1997/03/13 01:04:53  Woody
 * prepared for revision control
 *
 * Revision 2.18  1996/10/27  Woody
 * Bugfix bei 'zieh alles an/aus' ('alles'-notify_fail() jetzt ganz am Anfang)
 * DoWear() eingebaut (benoetigt keine id), P_WEAR|DEFEND|REMOVE_FUNC
 *
 * Revision 2.17  1996/10/12  Woody
 * 'zieh alles aus' ist nun moeglich
 *
 * Revision 2.17  1996/09/20  12:00:00  Woody
 * An-/Ausziehtexte jetzt konfigurierbar (P_WEAR_TEXT, P_UNWEAR_TEXT)
 *
 * Revision 2.16  1996/02/29  16:00:00  Mupfel
 * In die Ruestungsberechnung gehen ausser DT_BLUDGEON, DT_SLASH
 * und DT_PIERCE auch die anderen physischen Schadenstypen DT_RIP,
 * DT_SQUEEZE und DT_WHIP mit ein.
 *
 * Revision 2.15  1996/02/19  19:10:00  Mupfel
 * Default-Material in _set_armour_type setzen:
 * AT_ARMOUR, AT_HELMET, AT_RING : MAT_METAL (Metall)
 * AT_CLOAK, AT_TROUSERS         : MAT_CLOTH (Stoff)
 * AT_GLOVE, AT_BOOT             : MAT_LEATHER (Leder)
 * AT_SHIELD                     : MAT_WOOD (Holz)
 * AT_AMULET                     : MAT_MAGIC (mag. Material)
 * AT_MISC                       : MAT_MISC (unspezifiert)
 *
 * Revision 2.14  1996/02/17  02:25:00  Mupfel
 * P_TEMPERATURE_PROTECTION in _set_armour_type setzen
 * P_TOTAL_TEMPERATURE_PROTECTION automatisch berechnen
 *
 * Revision 2.13  1995/07/07  14:15:55  Rochus
 * Physcial Attacks (z.B. Karate) werden auch abgeblockt.
 *
 * Revision 2.12  1995/06/06  08:45:37  Jof
 * _notify_fail
 *
 * Revision 2.12  1995/06/05  14:57:48  Jof
 * notify_fail -> _notify_fail
 *
 * Revision 2.11  1995/04/27  18:53:33  Jof
 * use closures for repeated calls to QueryProp/SetProp
 *
 * Revision 2.10  1995/03/27  10:00:09  Boing
 * QueryDefend() repariert .. Ruestungen waren zur Zeit total nutzlos.
 *
 * Revision 2.9  1995/02/20  09:58:29  Jof
 * Replaces SetProp in create by Set
 *
 * Revision 2.9  1995/02/20  17:53:19  Jof
 * Changed SetProp to Set in create to stop senseless logging to /log/COMBAT
 *
 * Revision 2.8  1994/12/21  18:46:04  Boing
 * Added a capitalize ...
 *
 * Revision 2.7  1994/12/01  13:43:38  Boing
 * Ruestungen mit P_CURSED gesetzt koennen nicht mehr ausgezogen werden.
 *
 * Revision 2.6  1994/12/01  13:26:14  Jof
 * Minor bugfixing
 *
 */

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <thing/description.h>
#include <config.h>
#include <properties.h>
#include <combat.h>
#include <language.h>
#include <defines.h>
#include <new_skills.h>

#define RESTR_CHECKER "/std/restriction_checker"

private static closure defend_cl;
private static string *weartxt, *unweartxt;
private static mapping dam_protect;      // dagegen schuetzt die Ruestung
private static int logged;

mixed set_dam_prot(mixed prot);

void create()
{
  Set(P_ARMOUR_TYPE, AT_ILLEGAL);
  Set(P_WORN, PROTECTED, F_MODE);
  Set(P_DAM_PROTECTION, #'set_dam_prot, F_SET_METHOD);
  Set(P_DAM_PROTECTION, PROTECTED, F_MODE);
  SetProp(P_DAM_PROTECTION, PHYSICAL_DAMAGE_TYPES);
  SetProp(P_QUALITY_DECAY, 100);
}

void init()
{
  int i;
  string s1, s2;
  add_action("ziehe", "ziehe");
  add_action("ziehe", "zieh");
  add_action("do_wear", "trage");

  if (environment()!=PL) return;
  weartxt=QueryProp(P_WEAR_TEXT);
  if (!sizeof(weartxt))
    weartxt = ({ "Du ziehst "+name(WEN,1)+" an.\n",
                 " zieht "+name(WEN,0)+" an.\n"});
  else
    weartxt = weartxt+({});
  unweartxt=QueryProp(P_UNWEAR_TEXT);
  if (!sizeof(unweartxt))
    unweartxt = ({ "Du ziehst "+name(WEN,1)+" aus.\n",
                   " zieht "+name(WEN,0)+" aus.\n"});
  else
    unweartxt = unweartxt+({});
}

/* Wenn eine Ruestung vom gleichen Typ im Array ist, gebe diese zurueck. */
private object TestType(object *armours)
{
  int i,ring;
  mixed type;

  ring=0;
  type = QueryProp(P_ARMOUR_TYPE);
  armours-=({0});
  for (i=sizeof(armours)-1;i>=0;i--)
    if (type==armours[i]->QueryProp(P_ARMOUR_TYPE))
    {
      if ((type==AT_RING) && !ring++) continue;
      return armours[i];
    }
  return 0;
}

// errechne totale Ruestungsklasse fuer die Ruestungen in armours
private int TotalAc(object *armours)
{
  int sum,i;

  sum = 0;
  armours-=({0});
  for (i=sizeof(armours)-1;i>=0;i--)
    sum+=armours[i]->QueryProp(P_AC);
  return sum;
}

// dito. fuer totalen Temperaturschutz
private mapping TotalTProt(object *armours)
{
  int i;
  mapping sum,val;

  sum = ([ T_COLD_PROT:0,T_HEAT_PROT:0 ]);
  armours-=({0});
  for (i=sizeof(armours)-1;i>=0;i--)
    if (mappingp(val=armours[i]->QueryProp(P_TEMPERATURE_PROTECTION))) {
      sum[T_COLD_PROT] += val[T_COLD_PROT];
      sum[T_HEAT_PROT] += val[T_HEAT_PROT];
    }
  return sum;
}

mixed _set_armour_type(mixed type)
{
  if (!COMBAT_MASTER->valid_armour_type(type))
    Set(P_ARMOUR_TYPE, type = AT_ILLEGAL);
  else
    Set(P_ARMOUR_TYPE, type);
  AddId(type);

  // **** Default-Worn-Text setzen:

  if( type == AT_RING )
    SetProp(P_WORN_TEXT, "auf den Finger gesteckt");
  else if( type == AT_HELMET )
    SetProp(P_WORN_TEXT, "aufgesetzt");
  else if( type == AT_SHIELD )
    SetProp(P_WORN_TEXT, "in der linken Hand");
  else if( type == AT_CLOAK )
    SetProp(P_WORN_TEXT, "umgehängt");

  return type;
}

mixed set_dam_prot(mixed prot)
{
  int i, *tmp;

  if (stringp(prot)) prot = ([prot:1]);
   else if (pointerp(prot))
   {
     tmp = ({});
     for(i=sizeof(prot);i--;) tmp += ({1});
     prot = mkmapping(prot, tmp);
   }
   if (mappingp(prot) && mappingp(Set(P_DAM_PROTECTION, prot)))
     return dam_protect = prot;
   else
     return -1;
}


int _set_ac(int ac)
{
  object ob;
  string type;

  type = QueryProp(P_ARMOUR_TYPE);
  if ((ob=QueryProp(P_WORN)) && (ac > VALID_ARMOUR_CLASS[type])) {
    tell_object(ob, "Ungültige Rüstungsklasse für "+
        name(WEN,1)+", bitte Erzmagier verständigen.\n");
    return -1;
  }
  Set(P_AC, ac);
  if (ob) ob->SetProp(P_TOTAL_AC, TotalAc(ob->QueryProp(P_ARMOURS)));

  if(!logged)
  {
      call_other("/secure/awmaster", "RegisterArmour", ME);
      logged=1;
  }

  return ac;
}

private void msg(string str, mixed fl)
{ if(fl) write(str); else _notify_fail(str); }

int is_same(object a)
{
  return ( a->QueryProp(P_ARMOUR_TYPE)==AT_MISC &&
           a->QueryProp(P_SHORT) == QueryProp(P_SHORT) );
}

varargs int DoWear(int silent, int all)
{
  object found, *armours;
  mixed wear_func,type;
  closure qp;
  mapping rmap;
  string res;
  int nh;

  if (all) notify_fail("Alles angezogen, was ging.\n");
  if(environment() != PL)
    return (msg("Du musst " + ME->name(WEN,1) + " erst nehmen !\n", all), 0);
  if(QueryProp(P_WORN))
    return (msg("Du trägst " + ME->name(WEN,1) + " bereits.\n", all), 0);
  qp=symbol_function("QueryProp", PL);
  armours = funcall(qp, P_ARMOURS);
  type = QueryProp(P_ARMOUR_TYPE);
  if((found = TestType(armours)) && objectp(found) &&
     (type != AT_MISC))
    return (msg("Du trägst bereits " + found->name(WEN,1)
               +" als Schutz der Sorte " + type + ".\n", all), 0);
  if( sizeof(filter(PL->QueryProp (P_ARMOURS), "is_same") ) )
     return (msg("Du trägst bereits "+name(WEN,0)+"!\n",all),0);
  if(type == AT_ILLEGAL || QueryProp(P_AC) > VALID_ARMOUR_CLASS[type])
  {
    write("Ungültiger Rüstungstyp, bitte Erzmagier verständigen.\n");
    return !all;
  }
  if( (rmap=QueryProp(P_WEAR_RESTRICTIONS)) &&
      (res=RESTR_CHECKER->check_restrictions(PL, rmap)) )
  {
    write(res);
    return !all;
  }

// 22.07.2002 Serii@Tamedhon - gildenseitige WearRestrictions
  if(PL==this_interactive())
  {
    res=call_other("/gilden/"+PL->QueryProp(P_GUILD),"allow_wear",ME);
    if(res)
    {
      write(res);
      return !all;
    }
  }
// ---------------------------

  wear_func = PL->QueryProp(P_WEAR_FUNC);
  if (objectp(wear_func) && !wear_func->WearFunc(ME,silent))
    return !all;
  else if (pointerp(wear_func)) {
    int i;
    PL->SetProp(P_WEAR_FUNC, wear_func=filter(wear_func,#'objectp));
    for (i=sizeof(wear_func)-1; i>=0; i--)
      if (!wear_func[i]->WearFunc(ME,silent)) return !all;
  }

  nh=QueryProp(P_NR_HANDS);
  if (nh<1 && type==AT_SHIELD)
    nh=1;

  if (nh) {
    if (!PL->UseHands(ME,nh)) {
      write("Du hast keine Hand mehr frei.\n");
      return !all;
    }
  }

  wear_func = QueryProp(P_WEAR_FUNC);
  if(wear_func && !wear_func->WearFunc(ME,silent))
    return !all;

  if(!logged && query_once_interactive(PL))
  {
      call_other("/secure/awmaster", "RegisterArmour", ME);
      logged=1;
  }

  armours += ({ ME });
  armours -= ({ 0 });
  qp=symbol_function("SetProp", PL);
  funcall(qp, P_ARMOURS, armours);
  funcall(qp, P_TOTAL_AC, TotalAc(armours));
  funcall(qp, P_TOTAL_TEMPERATURE_PROTECTION, TotalTProt(armours));
  if (!silent)
  {
    write (weartxt[0]);
    say (PL->Name(WER)+weartxt[1]);
  }
  SetProp(P_WORN, PL);
  return !all;
}

varargs int do_wear(string str, int silent)
{
  int all;
  all = (str=="alles");
  if (!str || (!id(str) && !all)) return 0;
  if(all && !silent) write(Name(WER)+": ");
  return DoWear(silent, all);
}

varargs int DoUnwear(int silent)
{
  object *armours, worn_by;
  mixed remove_func;
  string tmp;
  closure qp,sp;
  int nh;

  if(!(worn_by = QueryProp(P_WORN))) return 0;

  if (tmp=QueryProp(P_CURSED))
  {
    if (stringp(tmp))
      tell_object(worn_by, tmp);
    else
      tell_object(worn_by, "Du kannst "+name(WEN)+" nicht ausziehen.\n");
    return 1;
  }
  remove_func = worn_by->QueryProp(P_REMOVE_FUNC);
  if (objectp(remove_func) && !remove_func->RemoveFunc(ME,silent))
    return 1;
  else if (pointerp(remove_func)) {
    int i;
    PL->SetProp(P_REMOVE_FUNC,remove_func=filter(remove_func,#'objectp));
    for (i=sizeof(remove_func)-1; i>=0; i--)
      if (!remove_func[i]->RemoveFunc(ME,silent)) return 1;
  }

  remove_func = QueryProp(P_REMOVE_FUNC);
  if(remove_func && !remove_func->RemoveFunc(this_object(),silent))
    return 1;

  qp=symbol_function("QueryProp",worn_by);
  sp=symbol_function("SetProp",worn_by);

  nh=QueryProp(P_NR_HANDS);
  if (nh<1 && QueryProp(P_ARMOUR_TYPE)==AT_SHIELD)
    nh=1;

  if (nh)
    worn_by->FreeHands(ME);

  armours = funcall(qp, P_ARMOURS ) - ({ ME }) - ({ 0 });
  funcall(sp, P_TOTAL_AC, TotalAc(armours));
  funcall(sp, P_TOTAL_TEMPERATURE_PROTECTION, TotalTProt(armours));
  funcall(sp, P_ARMOURS, armours);

  if(!silent)
  {
    tell_object (worn_by, unweartxt[0]);
    tell_room(environment(worn_by),
      worn_by->Name(WER)+unweartxt[1], ({worn_by}));
  }

  if(!logged && query_once_interactive(PL))
  {
      call_other("/secure/awmaster", "RegisterArmour", ME);
      logged=1;
  }

  SetProp(P_WORN, 0);
  return 1;
}

varargs int do_unwear(string str, int silent)
{
  int all;
  if (all=(str=="alles")) notify_fail("Alles ausgezogen, was ging.\n");
  if(!str || (!id(str) && !all) ||
      member(PL->QueryProp(P_ARMOURS),ME) == -1)
    return 0;
  if(all && !silent) write(Name(WER)+": ");
  if(!QueryProp(P_WORN))
  {
    if(!ME->Query(P_ARTICLE))
      _notify_fail( "Du trägst k" + ME->name(WEN,0) +".\n");
    else
      _notify_fail( "Du trägst " + ME->name(WEN,1) + " nicht.\n" );
    return 0;
  }
  DoUnwear( silent );
  return !all;
}

int QueryDefend (string* dam_type, mixed spell, object enemy) {
  int prot, i;
  object defend_func,pl;

  prot = 0;
  if (!spell || (mappingp(spell) && spell[SP_PHYSICAL_ATTACK]))
  if (sizeof(filter(dam_type, dam_protect)))
     prot = random(1+QueryProp(P_AC));
  if (objectp(defend_func=QueryProp(P_DEFEND_FUNC))) {
    if (!closurep(defend_cl))
      defend_cl=symbol_function("DefendFunc",defend_func);
    if (!objectp(pl=QueryProp(P_WORN))
        || !query_once_interactive(pl)
        || interactive(pl)) // Bei Netztoten keine (evtl. wirksame) DefendFunc
      prot += funcall(defend_cl, dam_type, spell, enemy);
  }
  return prot;
}

int ziehe(string str)
{
  string ob;
  if(!str) return 0;
  if(sscanf(str, "%s an", ob)==1 ) return do_wear(ob);
  if(sscanf(str, "%s aus", ob)==1 ) return do_unwear(ob);
  return 0;
}

// TO BE REMOVED

void SetDefendFunc(object ob) { SetProp(P_DEFEND_FUNC,ob); }
object QueryDefendFunc() { return QueryProp(P_DEFEND_FUNC); }

void SetWearFunc(object ob) { SetProp(P_WEAR_FUNC,ob); }
object QueryWearFunc() { return QueryProp(P_WEAR_FUNC); }

void SetRemoveFunc(object ob) { SetProp(P_REMOVE_FUNC,ob); }
object QueryRemoveFunc() { return QueryProp(P_REMOVE_FUNC); }

mixed QueryWorn() { return QueryProp(P_WORN); }

// flaw  wird bei jedem Treffer hochgezaehlt
//       (in jeder Kampfrunde random fuer ein Ruestungsteil)
// ftime ist die Zeit des ersten Treffers
// decay ist ein Mapping ([dam_type:hits,...,0:hits]).
//       Wenn mit Schadenstyp 'dam_type' angegriffen wird, wird alle
//       'hits' Treffer die Ruestung um 1 verschlechtert. Default ist
//       'dam_type' 0, d.h. alle Schadenstypen.
//       Bei mehreren Schadenstypen in 'dam_type' wird pro Schlag
//       zufaellig einer ausgewaehlt.

int     ftime, flaw;
mapping decay;

mixed _set_quality_decay(mixed arg)
{
  if (intp(arg))
    decay = ([0:arg]);
  else if (mappingp(arg))
    decay = arg;
  else
    decay = arg = 0;
  return Set(P_QUALITY_DECAY, arg);
}

void TakeFlaw(mixed *dam_type)
{
  int    ac, hits;
  string dt;

  if(!ftime) ftime=time();
  flaw++;
  if (decay)  // speed things up
  {
    dt = dam_type[random(sizeof(dam_type))];  // Angriffsschaden
    if (member(decay, dt))  // decay[dt] kann auch 0 sein
      hits = decay[dt];
    else
      hits = decay[0];
    if (hits && !(flaw % hits) && (ac=QueryProp(P_AC))>0)
    {
      SetProp(P_AC, ac-1);
      SetProp(P_DAMAGED, QueryProp(P_DAMAGED)+1);
     }
  }

  if(!logged && query_once_interactive(PL))
  {
      call_other("/secure/awmaster", "RegisterArmour", ME);
      logged=1;
  }
}

mixed *QueryFlaw()
{
  return({flaw,ftime,dtime(ftime)});
}

mapping _set_temperature_protection(mixed m)
{
  if (mappingp(m))
    return Set(P_TEMPERATURE_PROTECTION,
        copy(m));
  else if (intp(m))
    return Set(P_TEMPERATURE_PROTECTION,
        ([ T_COLD_PROT:m,T_HEAT_PROT:m ]) );
  else
    return Set(P_TEMPERATURE_PROTECTION,
        ([ T_COLD_PROT:0,T_HEAT_PROT:0 ]) );
}

mapping _query_temperature_protection()
{
  mixed res, at;

  if (mappingp(res=Query(P_TEMPERATURE_PROTECTION)))
    return copy(res);
  at=QueryProp(P_ARMOUR_TYPE);
  switch (at)
  {
    case AT_ARMOUR:
      return ([ T_COLD_PROT:4,T_HEAT_PROT:-3 ]);
    case AT_CLOAK:
      return ([ T_COLD_PROT:4,T_HEAT_PROT:-1 ]);
    case AT_HELMET:
      return ([ T_COLD_PROT:2,T_HEAT_PROT:1 ]);
    case AT_TROUSERS:
      return ([ T_COLD_PROT:3,T_HEAT_PROT:-1 ]);
    case AT_GLOVE:
      return ([ T_COLD_PROT:2,T_HEAT_PROT:-1 ]);
    case AT_BOOT:
      return ([ T_COLD_PROT:2,T_HEAT_PROT:0 ]);
    case AT_BRACERS:
      return ([ T_COLD_PROT:1,T_HEAT_PROT:0 ]);
    case AT_SHIELD:
      return ([ T_COLD_PROT:0,T_HEAT_PROT:3 ]);
    default:
      return ([ T_COLD_PROT:0,T_HEAT_PROT:0 ]);
  }
}

