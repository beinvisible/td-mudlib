/* Standardfile einer werfbaren Waffe, 
 * Originalversion von Paracelsus@MG
 *
 * Anpassung ans Turithil von Samtpfote
 */

//
// 03/30/2004 Serii
// verfluchte waffen koennen nichtmehr geworfen werden!
//
// wenn die Waffe toetete, gabs nen RZG...
// habsch behoben
// Gralkor, 2004-01-14
//


#pragma strong_types

#include <properties.h>
#include <language.h>
#include <moving.h>

#define TO		this_object()
#define TP		this_player()
#define BS(x)		break_string(x)
#define ETP		environment(this_player())
#define ETO		environment(this_object())
#define TPN(x)		this_player()->name(x)

#define ENV(x)		environment(x)

int des_test();

inherit "/std/weapon";

void create()
{   
    if(!clonep(this_object())) { return 0; }
    ::create();
    SetProp(P_SHORT, "Eine Wurfwaffe");
    SetProp(P_THROWING_WEAPON, 1);
}

int throw_it(string in_)
{   object en;
    string was,wer;
    int i,de,dp;

    notify_fail("Du musst "+name(WEN,1)+" schon zücken.\n");
    if (!QueryWielded()) return 0;
    if (QueryWielded()!=TP) return 0;
    if (TP->QueryProp(P_ATTACK_BUSY))
    {   write("Du bist schon zu beschäftigt.\n");
        return 1;
    }
    notify_fail("Was willst Du werfen ?\n");
    if (!in_) return 0;
    if (id(in_))
    {   notify_fail(BS(
            "Wenn Du nicht kämpfst, musst Du schon sagen, auf wen Du "+
            "werfen willst."));
        if (!(TP->InFight())) return 0;
        en = TP->SelectEnemy();
    }
    else
    {   if (sscanf(in_,"%s auf %s",was,wer)!=2 &&
            sscanf(in_,"%s nach %s",was,wer)!=2) return 0;
        if (!id(was)) return 0;
        en = present(wer,ETP);
    }
    notify_fail("Auf wen willst Du werfen ?\n");
    if (!en) return 0;
    notify_fail(stringp(QueryProp(P_CURSED))?QueryProp(P_CURSED):
      sprintf("Du kannst %s nicht weg werfen!\n",Name(WEN,1)));
    if(QueryProp(P_CURSED))return 0;
    DoUnwield(1);
    TP->SetProp(P_ATTACK_BUSY,1);
    write(BS("Du wirfst "+name(WEN,1)+" auf "+en->name(WEN,1)+"."));
    say(BS(capitalize(TPN(WER))+" wirft "+name(WEN,1)+" auf "+
        en->name(WEN,1)+"."),({ TP,en }));
    tell_object(en,BS(capitalize(TPN(WER))+" wirft "+name(WEN,1)+
        " nach Dir.") );
    i = random(100);
    de = en->QueryAttribute(A_DEX);
    dp = TP->QueryAttribute(A_DEX);
    if ((2*de)>(3*dp))
    {   tell_room(ETP,
            capitalize(en->name(WER,1))+" fängt "+name(WEN,1)+
            " geschickt auf.\n",({en}));
        tell_object(en,
            "Du fängst "+name(WEN,1)+" geschickt auf.\n");
        if (move(en,M_SILENT)==ME_TOO_HEAVY)
        {   move(ENV(en),M_NOCHECK);
            tell_object(en,capitalize(name(WER,1))+
                " ist Dir zu schwer und Du lässt "+
                QueryPronoun(WEN)+" fallen.\n");
            tell_room(ENV(en),capitalize(en->name(WER,1))+
                " lässt "+name(WEN,1)+" wieder fallen.\n",({ en }));
        }
        return 1;
    }
    if (((2*dp)>(3*de))||((7*dp/2)>i))
    {   write("Du triffst "+en->name(WEN,1)+".\n");
        say(capitalize(TPN(WER))+" trifft "+en->name(WEN,1)+".\n",
            ({ en,TP }) );
        tell_object(en,capitalize(TPN(WER))+" trifft Dich.\n");
        i  = 4*TP->QueryRealAttribute(A_STR);
        i += 2*TP->QueryRealAttribute(A_DEX);
        i += 2*TP->QueryRealAttribute(A_CON);
        i += 2*TP->QueryRealAttribute(A_INT);
        i *= QueryProp(P_WC);
        i /= 50; // Max. 4facher WC-Schaden
        if(en->QueryProp(P_HP)<TO->QueryProp(P_WC)*4)
          {
           i=QueryProp(P_WC)-3;
          } 
        
        en->Defend(i,QueryProp(P_DAM_TYPE),1,TP);
        if (move(en,M_SILENT)==ME_TOO_HEAVY||!living(en))
        {   tell_room(ENV(en),capitalize(name(WER,1))+
                " fällt zu Boden.\n");
            move(ENV(en),M_NOCHECK);
        }
        return des_test();
    }
    write("Du verfehlst "+en->name(WEN,1)+".\n");
    say(capitalize(TPN(WER))+" wirft daneben.\n");
    tell_room(ENV(en),capitalize(name(WER,1))+" fällt zu Boden.\n");
    move(ENV(TP),M_NOCHECK);
    return des_test();
}

void init()
{   ::init();
    add_action("throw_it","wirf");
    add_action("throw_it","werfe");
    add_action("throw_it","schleuder");
    add_action("throw_it","schleudere");
}

int des_test()
{   if (random(100)>42) return 1;
    tell_object(ETO,capitalize(name(WER,1))+" zerbricht.\n");
    call_out("remove",1);
    return 1;
}
