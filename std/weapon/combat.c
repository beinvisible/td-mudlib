// Tamedhon MUDlib
//
// std/weapon/combat.c -- combat part of the weapon standard object
//
// $Date: 2010/03/06 12:00:00 $
/* $Revision: 1.4 $
 * $Log: combat.c,v $
 *
 * Revision 1.4  2010/03/06 12:00:00  Rommy
 * _set_dam_type(): Argument pruefen (mappings sind boese)
 *
 * Revision 1.3  2009/08/23 00:30:00  Grynnis
 * WieldFunc bzw. UnwieldFunc der Waffe werden nun als letztes aufgerufen
 * nach allen anderen Checks, sonst Problem bei MIs, wenn das MI Attribute
 * aendert, trotz eventuellem Verbot durch spaeteren Check
 *
 * Revision 1.21 2004/09/13 18:00:00  Querolin
 * logged awstats bug abgefangen durch if(PL) zuvor.
 *
 * Revision 1.2  2002/22/07 17:02:05  Serii
 * Aufruf von allow_wield() in der jeweiligen Gilde, in wield_me()
 *
 * Revision 1.1  2000/06/12 10:02:52  Samtpfote
 * Initial revision
 *
 * Revision 3.14  1998/01/02 02:55:52  Woody
 * Bugfix in DoUnwield() beim Zuruecksetzen auf original_(wc|decay)
 *
 * Revision 3.13  1997/10/09 00:56:59  Woody
 * P_TOTAL_WC Berechnungsbug bei Wegstecken gefixt, P_WC_FIXFACTOR und
 * neue Schadensberechnung, P_QUALITY_DECAY per default 100
 *
 * Revision 3.12  1997/09/25 15:13:17  Woody
 * Material setzen hier rausgenommen
 *
 * Revision 3.11  1997/09/04 21:52:33  Woody
 * P_UNWIELD_FUNC nicht von this_player() sondern von P_WIELDED (*argl*)
 *
 * Revision 3.10  1997/08/26 21:24:19  Woody
 * P_(UN)WIELD_FUNC im Spielerobjekt auswerten (Objekt oder Array
 * von Objekten, das automatisch von destructed objects bereinigt wird)
 *
 * Revision 3.9  1997/08/25 15:30:46  Feyaria
 * quality_decay, special_wc
 *
 * Revision 3.8  1997/07/01 15:07:36  Feyaria
 * _set_wc eingefuehrt
 *
 * Revision 3.7  1997/06/25 12:11:24  Woody
 * neues UseHands System, Fehler mit notify_fail statt write
 *
 * Revision 3.4  1997/05/12 12:07:57  Feyaria
 * materialien fuer bow und whip
 *
 * Revision 3.3  1997/04/19 20:37:15  Feyaria
 * P_WIELD_RESTRICTIONS
 *
 * Revision 3.2  1997/03/25 12:58:54  Woody
 * wielded_by steckt eine Waffe zurueck, nicht this_player() *argl*
 *
 * Revision 3.1  1997/03/20 18:10:50  Woody
 * Spielername bei (un)wieldtxt jedesmal neu Einsetzen (Frosch und so...)
 *
 * Revision 3.0  1997/03/13 02:44:52  Woody
 * prepared for revision control
 *
 * Revision 2.14  1996/10/27  Woody
 * Verwenden von P_HIT_FUNC, P_WIELD_FUNC, P_UNWIELD_FUNC
 *
 * Revision 2.13  1996/09/21  13:00:00  Woody
 * P_WIELDED_TEXT, P_(UN)WIELD_TEXT
 *
 * Revision 2.12  1996/02/18  02:14:00  Mupfel
 * Default-Material in _set_weapon_type setzen:
 * WT_SPEAR, WT_CLUB : MAT_WOOD (Holz)
 * WT_MAGIC, WT_AMMU : MAT_MAGIC (mag. Material)
 * WT_MISC           : MAT_MISC (unspezifiziert)
 * sonst             : MAT_METAL (Metall)
 *
 * Revision 2.11  1995/06/26  08:41:41  Jof
 * return 0 in DoUnwield for cursed weapons
 *
 * Revision 2.10  1995/06/06  08:45:50  Jof
 * _notify_fail
 *
 * Revision 2.10  1995/06/05  15:02:02  Jof
 * _notify_fail
 *
 * Revision 2.9  1995/04/27  19:50:26  Jof
 * Typo fixed (missing +)
 *
 * Revision 2.8  1995/04/27  19:48:17  Jof
 * Some funcalls, replaced some ME->name() by name()
 *
 * Revision 2.7  1994/12/01  13:59:37  Boing
 * Waffen mit P_CURSED gesetzt koennen nicht mehr weggesteckt werden.
 *
 * Revision 2.6  1994/09/14  19:38:59  Rochus
 * Funktion wield_me eingebaut, die den Namen nicht verlangt.
 *
 * Revision 2.5  1994/03/21  16:48:13  Jof
 * Reindented files using emacs - once again
 *
 * Revision 2.4  1994/03/11  16:51:36  Jof
 * type
 * typo
 *
 * Revision 2.3  1994/03/11  13:21:56  Jof
 * uwield -> unwield
 *
 * Revision 2.2  1994/03/09  21:50:55  Hate
 * changed to strong_types
 *
 */

#include "/std/sys_debug.h"

#pragma strong_types

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <thing/commands.h>
#include <thing/description.h>

#include <properties.h>
#include <language.h>
#include <combat.h>
#include <attributes.h>
#include <defines.h>
#include <moving.h>

#define RESTR_CHECKER "/std/restriction_checker.c"

// local prototypes
varargs int wield(string str, int silent);
int unwield(string str);
varargs int DoUnwield(int silent);

private static string *wieldtxt, *unwieldtxt;
private int original_wc, original_decay;
private static int logged;

void create()
{
  SetProp(P_WEAPON_TYPE, WT_CLUB);
  SetProp(P_DAM_TYPE, DT_BLUDGEON);
  SetProp(P_NR_HANDS, 2);
  SetProp(P_WC_FIXFACTOR, 0.3);
  SetProp(P_QUALITY_DECAY, 100);
  Set(P_WIELDED, PROTECTED, F_MODE);
}

void init()
{
  add_action("wield", "zueck");
  add_action("wield", "zuecke");
  add_action("wield", "zück");
  add_action("wield", "zücke");
  add_action("wield", "zieh");
  add_action("wield", "ziehe");
  add_action("unwield", "steck");
  add_action("unwield", "stecke");
  if (environment()!=PL) return;
  wieldtxt=QueryProp(P_WIELD_TEXT);
  if (!sizeof(wieldtxt))
    wieldtxt = ({ "Du zückst "+name(WEN,1)+".\n",
                  " zückt "+name(WEN,0)+".\n"});
  else
    wieldtxt = wieldtxt+({});
  unwieldtxt=QueryProp(P_UNWIELD_TEXT);
  if (!sizeof(unwieldtxt))
    unwieldtxt = ({ "Du steckst "+name(WEN,1)+" zurück.\n",
                    " steckt "+name(WEN,0)+" zurück.\n"});
  else
    unwieldtxt = unwieldtxt+({});
}

varargs int wield_me(int silent)
{
  object wielded_weapon;
  int used_hands;
  closure cl;
  mapping rmap;
  string res;
  mixed wield_func, spc_id;

  if(environment() != PL)
  {
    _notify_fail( "Du musst " + name(WEN,1) + " schon erst nehmen!\n" );
    return 0;
  }
  if(QueryProp(P_WIELDED))
  {
    notify_fail("Das hast Du schon gemacht!\n");
    return 0;
  }

  wield_func = PL->QueryProp(P_WIELD_FUNC);
  if (objectp(wield_func) && !wield_func->WieldFunc(ME,silent)) return 1;
  else if (pointerp(wield_func)) {
    int i;
    PL->SetProp(P_WIELD_FUNC, wield_func=filter(wield_func,#'objectp));
    for (i=sizeof(wield_func)-1; i>=0; i--)
      if (!wield_func[i]->WieldFunc(ME,silent)) return 1;
  }

  if(((PL->QueryAttribute(A_DEX)+5)*10) < QueryProp(P_WC))
  {
    notify_fail("Du bist nicht geschickt genug, "
        "um mit dieser Waffe umzugehen.\n");
    return 0;
  }

  if( (rmap=QueryProp(P_WIELD_RESTRICTIONS)) &&
      (res=RESTR_CHECKER->check_restrictions(PL, rmap)) )
  {
    notify_fail(res);
    return 0;
  }

// 22.07.2002 Serii@Tamedhon - gildenseitige WieldRestrictions
  if(PL==this_interactive())
  {
    res=call_other("/gilden/"+PL->QueryProp(P_GUILD),"allow_wield",ME);
    if(res)
    {
      notify_fail(res);
      return 0;
    }
  }
// ---------------------------

  if(wielded_weapon = PL->QueryProp(P_WEAPON))
    if(wielded_weapon->DoUnwield(silent) && !(PL->QueryProp(P_WEAPON)))
      return wield_me(silent);
    else
    {
      notify_fail("Das geht nicht, solange Du noch eine andere Waffe "
          "gezückt hast.\n");
      return 0;
    }
  if((original_wc = QueryProp(P_WC)) > MAX_WEAPON_CLASS)
  {
    notify_fail("Ungültige Waffenklasse, bitte Erzmagier verständigen.\n");
    return 0;
  }

  if (!PL->UseHands(ME,QueryProp(P_NR_HANDS))) {
    notify_fail("Du hast keine Hand mehr frei.\n");
    return 0;
  }

  wield_func = QueryProp(P_WIELD_FUNC);
  if(wield_func && !wield_func->WieldFunc(ME)) return 1;

  SetProp(P_WIELDED, PL);
  original_decay = QueryProp(P_QUALITY_DECAY);
  if (mappingp(QueryProp(P_SPECIAL_WC)))
  {
    spc_id = filter(PL->QueryProp(P_IDS), QueryProp(P_SPECIAL_WC));
    if (sizeof(spc_id))
    {
      SetProp(P_WC, QueryProp(P_SPECIAL_WC)[spc_id[0]]);
      SetProp(P_QUALITY_DECAY, 0);
    }
  }
  cl=symbol_function("SetProp",PL);
  funcall(cl,P_WEAPON, ME );
  funcall(cl,P_TOTAL_WC, QueryProp(P_WC));

  if(!silent)
  {
    write (wieldtxt[0]);
    say (PL->Name(WER)+wieldtxt[1]);
  }

  if (PL)
  {
    if(!logged && query_once_interactive(PL))
    {
        call_other("/secure/awmaster", "RegisterWeapon", ME);
        logged=1;
    }
  }
  return 1;
}

varargs int wield(string str, int silent)
{
  if(!str) return 0;
  if(query_verb()[0..3]=="zieh" && sscanf(str,"%s hervor",str)!=1) return 0;
  if(!id(str))
  {
    _notify_fail("Du hast sowas nicht.\n");
    return 0;
  }
  return wield_me(silent);
}

int unwield(string str)
{
  string dummy;

  if (!str) return 0;
  if(sscanf(str,"%s weg", dummy)!=1 &&
     sscanf(str,"%s ein", dummy)!=1 &&
     sscanf(str,"%s zurück", dummy)!=1 &&
     sscanf(str,"%s zurueck", dummy)!=1)
    return 0;
  str = dummy;
  if(!str || !id(str) || PL->QueryProp(P_WEAPON) != ME )
    return 0;
  if(environment() != PL)
  {
    _notify_fail("Diese Waffe gehört Dir nicht!\n");
    return 0;
  }
  if(!QueryProp(P_WIELDED))
  {
    _notify_fail("Diese Waffe hast Du gar nicht gezückt ...\n");
    return 0;
  }
  return DoUnwield();
}

varargs int DoUnwield(int silent)
{
  int used_hands;
  object wielded_by;
  mixed hands, unwield_func;
  closure cl;
  mixed tmp;

  if(!wielded_by = QueryProp(P_WIELDED)) return 0;

  unwield_func = wielded_by->QueryProp(P_UNWIELD_FUNC);
  if (objectp(unwield_func) && !unwield_func->UnwieldFunc(ME,silent)) return 1;
  else if (pointerp(unwield_func)) {
    int i;
    PL->SetProp(P_UNWIELD_FUNC,
        unwield_func=filter(unwield_func,#'objectp));
    for (i=sizeof(unwield_func)-1; i>=0; i--)
      if (!unwield_func[i]->UnwieldFunc(ME,silent)) return 1;
  }

  if (tmp=QueryProp(P_CURSED) && !(silent&M_NOCHECK))
  {
    if (stringp(tmp))
      tell_object(wielded_by, tmp);
    else
      tell_object(wielded_by,
		  "Du kannst "+name(WEN)+" nicht wegstecken.\n");
    return 1;
  }

  unwield_func = QueryProp(P_UNWIELD_FUNC);
  if(unwield_func && !unwield_func->UnwieldFunc(ME))
    return 1;

  cl=symbol_function("QueryProp",wielded_by);
  hands = funcall(cl,P_HANDS);

  wielded_by->FreeHands(ME);
  cl=symbol_function("SetProp",wielded_by);
  funcall(cl,P_TOTAL_WC, hands[1]); /* bare hands */
  funcall(cl,P_WEAPON, 0);

  if(!silent)
  {
    tell_object(wielded_by, unwieldtxt[0]);
    tell_room(environment(wielded_by),
              wielded_by->Name(WER)+unwieldtxt[1], ({wielded_by}));
  }
  SetProp(P_WIELDED, 0);        // Reihenfolge wichtig wg. _set_wc()
  if (!QueryProp(P_QUALITY_DECAY))
  {
    SetProp(P_WC, original_wc);   //  (wo P_WIELDED->P_TOTAL_WC gesetzt wird!)
    SetProp(P_QUALITY_DECAY, original_decay);
  }

  if (PL)
  {
    if(!logged && query_once_interactive(PL))
    {
        call_other("/secure/awmaster", "RegisterWeapon", ME);
        logged=1;
    }
  }

  return 1;
}

int QueryDamage(object enemy)
{
  int dam, wc;
  float ff;
  object hit_func;

  if(!QueryProp(P_WIELDED)) return 0;
  /*
  dam = (2*QueryProp(P_WC)+10*
	 QueryProp(P_WIELDED)->QueryAttribute(A_STR)) / 3;
  dam = random(dam);
  */
  wc=QueryProp(P_WC);
  ff=QueryProp(P_WC_FIXFACTOR);
  dam = to_int(2.0*wc*ff)+random(to_int(2.0*wc*(1-ff))+
               10*QueryProp(P_WIELDED)->QueryAttribute(A_STR));
  dam /= 3;
  if(hit_func=QueryProp(P_HIT_FUNC))
    dam += hit_func->HitFunc(enemy);
  return dam;
}

/// TO BE REMOVED

object SetWieldFunc(object ob) { return SetProp(P_WIELD_FUNC, ob); }
object QueryWieldFunc() { return QueryProp(P_WIELD_FUNC); }

object SetUnwieldFunc(object ob) { return SetProp(P_UNWIELD_FUNC, ob); }
object QueryUnwieldFunc() { return QueryProp(P_UNWIELD_FUNC); }

object SetHitFunc(object ob) { return SetProp(P_HIT_FUNC, ob); }
object QueryHitFunc() { return QueryProp(P_HIT_FUNC); }

object QueryWielded() { return QueryProp(P_WIELDED); }
// ***

int _set_nr_hands(int arg)
{
  if(arg<1) return Query(P_NR_HANDS);
  Set(P_NR_HANDS, arg);
  return 1;
}

mixed _set_dam_type(mixed arg)
{
  if(pointerp(arg)) return Set(P_DAM_TYPE, arg);
  if(stringp(arg))  return Set(P_DAM_TYPE, ({ arg }));
  // log and ignore
  log_file("bad_damage",sprintf(
    "[%O]: _set_dam_type(%O)\n",this_object(),arg));
  return Query(P_DAM_TYPE);
}

int _set_wc(int wc)
{
  object ob;

  if ((ob=QueryProp(P_WIELDED)) && query_once_interactive(ob) &&
      (wc > MAX_WEAPON_CLASS))
  {
    tell_object(ob,
        "Ungültige Waffenklasse für "+
        name(WEN,1)+", bitte Erzmagier verständigen.\n");
    return -1;
  }
  if (ob) ob->SetProp(P_TOTAL_WC, wc);
  return Set(P_WC, wc);
}

float _set_wc_fixfactor(float ff)
{
  if (ff < 0.0) ff=0.0;
  else if (ff > 1.0) ff=1.0;
  return Set(P_WC_FIXFACTOR, ff);
}

// flaw  wird bei jedem Hit hochgezaehlt
// ftime ist die Zeit des ersten Hits
// alle decay Hits wird die Waffe um 1 verschlechtert
int ftime, flaw, decay;

mixed _set_quality_decay(mixed arg)
{
  if (intp(arg)) decay = arg;
  return Set(P_QUALITY_DECAY, arg);
}

void TakeFlaw()
{
  int wc;

  if(!ftime) ftime=time();
  flaw++;
  if (decay && !(flaw % decay) && (wc=QueryProp(P_WC))>0)
  {
    SetProp(P_WC, wc-1);
    SetProp(P_DAMAGED, QueryProp(P_DAMAGED)+1);
    if(QueryProp(P_WIELDED))
    {
    	tell_object(QueryProp(P_WIELDED),
		Name()+" wurde durch den Angriff beschädigt.\n");
    }
  }

  if(PL)
  {
    if(!logged && query_once_interactive(PL))
    {
        call_other("/secure/awmaster", "RegisterWeapon", ME);
        logged=1;
    }
  }
}

mixed *QueryFlaw()
{
  return({flaw,ftime,dtime(ftime)});
}

