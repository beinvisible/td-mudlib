/* Tamedhon MUDLIB
 *
 * waffen/ruestungsmaster
 *
 * $Date: 2000/06/12 09:55:19 $
 * $Revision: 1.1 $
 * $Log: awmaster.c,v $
 * Revision 1.1  2000/06/12 09:55:19  Samtpfote
 * Initial revision
 *
 */

#include <combat.h>
#include <language.h>
#include <properties.h>
#include <wizlevels.h>

#define SAVEFILE  "/secure/ARCH/awmaster"
#define ADUMPFILE "/log/ARCH/RUESTUNGEN"
#define DDUMPFILE "/log/ARCH/DAMAGERS"
#define WDUMPFILE "/log/ARCH/WAFFEN"
#define ACCNGFILE "/log/ARCH/ACCHANGE"

//#define ACCHANGE 1

#define AWF_PUTON  1
#define AWF_PUTOFF 2
#define AWF_BOOST  4
#define AWF_ZAUBER 8
#define AWF_RESIST 16

#define AWM_TYPE      0
#define AWM_CLASS     1
#define AWM_EFF_CLASS 2
#define AWM_FLAGS     3
#define AWM_WEIGHT    4
#define AWM_VALUE     5
#define AWM_HANDS     6
#define AWM_D_TYPE    7
#define AWM_X_CLASS   8

mapping armours,weapons,damagers;

void create()
{
    seteuid(getuid(this_object()));
    if (clonep(this_object()))
    {
        destruct(this_object());
        return;
    }
    if (!restore_object(SAVEFILE))
    {
        armours = ([]);
        weapons = ([]);
        damagers = ([]);
    }
}

void save_me()
{
    save_object(SAVEFILE);
}

static void NotifyACChange(object ob, string fn)
{
#ifdef ACCHANGE
    string at,msg;
    int ac,max;

    if (!objectp(ob) || !stringp(fn))
        return;
    if (!stringp(at=ob->QueryProp(P_ARMOUR_TYPE)))
        return;
    if ((ac=ob->QueryProp(P_AC)) < (max=VALID_ARMOUR_CLASS[at]))
        return;
/* Bereits genehmigte Max-AC-Umhaenge und Ringe/Amulette, die nach den neuen
** Werten Max-Schutz haben, werden nicht gemeldet.
*/
    if ( (at==AT_CLOAK || at==AT_RING || at==AT_AMULET || at==AT_MISC) && 
          ac==VALID_ARMOUR_CLASS[at])
        return;
    msg=sprintf("ROOT [%s]\nDie Rüstung %s (P_AC=%d)\n",
        dtime(time()),fn,ac);
    if (ac==max) 
        msg+=("\
besitzt nach Umstellung der Rüstungsklassen einen MAXIMALEN Rüstungsschutz.\n\
Lass doch bitte dieses Objekt vom Balance-Gremium genehmigen.\n\
Hierzu genügt ein kurzer Antrag in /players/paracelsus/office.\n");
    else
        msg+=("\
besitzt nach Umstellung der Rüstungsklassen einen UNZULÄSSIGEN\n\
Rüstungsschutz, d.h. sie wird für Spieler in Kürze nicht mehr nutzbar\n\
sein. Bitte ändere den Rüstungsschutz entspr. der neuen Obergrenzen\n\
(-> man rüstungen). Sollte der gewünschte Rüstungsschutz oberhalb der\n\
Genehmigungsgrenze liegen, muss die Rüstung selbstverständlich vom\n\
Balance-Gremium genehmigt werden.\n");

    msg+=("### Dies ist eine automatisch generierte Meldung ###\n");

    write_file(ACCNGFILE,
        sprintf("[%s] P_AC: %d MAX: %d\nOb: %s\n\n",
            dtime(time()),ac,max,fn));

    at = MASTER->creator_file(ob);
    if (at == ROOTID)
        at = "ROOT";
    else if( !at || at[0]==' ' )
        at="STD";
    at = "report/" + at + ".rep";

    log_file(at,msg);
#endif
}

int xflags(object ob)
{   int re;

    if (!ob || !objectp(ob))
        return 0;
    re=0;
    if (ob->QueryProp(P_WEAR_FUNC)==ob  ||
        ob->QueryProp(P_WIELD_FUNC)==ob )
        re += AWF_PUTON;
    if (ob->QueryProp(P_REMOVE_FUNC)==ob  ||
        ob->QueryProp(P_UNWIELD_FUNC)==ob )
        re += AWF_PUTOFF;
    if (ob->QueryProp(P_DEFEND_FUNC)==ob ||
        ob->QueryProp(P_HIT_FUNC)==ob    )
        re += AWF_BOOST;
    if (ob->QueryProp(P_RESISTANCE_STRENGTHS))
        re += AWF_RESIST;
    return re;
}

void RegisterArmour()
{   object  ob;
    string  id;
    int     flag,h;

    if (!objectp(ob=previous_object()) || 
            member(inherit_list(ob),"std/armour.c")==-1)
        return;
    id = explode(object_name(ob),"#")[0];
    if (member(armours,id))
    {
        flag=0;
        if ((h=ob->QueryProp(P_AC)) > armours[id][AWM_CLASS])
        {
            armours[id][AWM_CLASS]=h;
            NotifyACChange(ob,id);
            flag=1;
        }
	/*
        if ((h=ob->QueryProp(P_EFFECTIVE_AC)) > armours[id][AWM_EFF_CLASS])
        {
            armours[id][AWM_EFF_CLASS]=h;
            flag=1;
        }
	*/
        if ((h=ob->QueryProp(P_NR_HANDS)) < armours[id][AWM_HANDS])
        {
            armours[id][AWM_HANDS]=h;
            flag=1;
        }
        if ((h=xflags(ob)) != armours[id][AWM_FLAGS])
        {
            armours[id][AWM_FLAGS]=h;
            flag=1;
        }
        if (!flag)
            return;
        save_me();
        return;
    }
    armours += ([ id : 
        ([
            AWM_TYPE      : ob->QueryProp(P_ARMOUR_TYPE) ,
            AWM_CLASS     : ob->QueryProp(P_AC) ,
            /* AWM_EFF_CLASS : ob->QueryProp(P_EFFECTIVE_AC) , */
            AWM_FLAGS     : xflags(ob),
            AWM_WEIGHT    : ob->QueryProp(P_WEIGHT) ,
            AWM_VALUE     : ob->QueryProp(P_VALUE) ,
            AWM_HANDS     : ob->QueryProp(P_NR_HANDS) , // Fuer Schilde
            AWM_D_TYPE    : ob->QueryProp(P_DAM_TYPE) ,
            AWM_X_CLASS   : ob->QueryProp(P_WC)
        ]) ]);
    NotifyACChange(ob,id);
    save_me();
}

void RegisterWeapon()
{   object  ob;
    string  id;
    int     flag,h;

    if (!objectp(ob=previous_object()) ||
            member(inherit_list(ob),"std/weapon.c")==-1)
        return;
    id = explode(object_name(ob),"#")[0];
    if (member(weapons,id))
    {
        flag=0;
        if ((h=ob->QueryProp(P_WC)) > weapons[id][AWM_CLASS])
        {
            weapons[id][AWM_CLASS]=h;
            flag=1;
        }
	/*
        if ((h=ob->QueryProp(P_EFFECTIVE_WC)) > weapons[id][AWM_EFF_CLASS])
        {
            weapons[id][AWM_EFF_CLASS]=h;
            flag=1;
        }
	*/
        if ((h=ob->QueryProp(P_NR_HANDS)) < weapons[id][AWM_HANDS])
        {
            weapons[id][AWM_HANDS]=h;
            flag=1;
        }
        if ((h=xflags(ob)) != weapons[id][AWM_FLAGS])
        {
            weapons[id][AWM_FLAGS]=h;
            flag=1;
        }
        if (flag)
            save_me();
        return;
    }
    weapons += ([ id :
        ([
            AWM_TYPE      : ob->QueryProp(P_WEAPON_TYPE) ,
            AWM_CLASS     : ob->QueryProp(P_WC) ,
            /* AWM_EFF_CLASS : ob->QueryProp(P_EFFECTIVE_WC) , */
            AWM_FLAGS     : xflags(ob),
            AWM_WEIGHT    : ob->QueryProp(P_WEIGHT) ,
            AWM_VALUE     : ob->QueryProp(P_VALUE) ,
            AWM_HANDS     : ob->QueryProp(P_NR_HANDS) ,
            AWM_D_TYPE    : ob->QueryProp(P_DAM_TYPE) ,
            AWM_X_CLASS   : ob->QueryProp(P_AC)
        ]) ]);
    save_me();
}

void RegisterDamager(object dam_ob,int old_dam, int new_dam)
{   object ob;
    int flag;
    string fn;

    if (!objectp(ob=previous_object()) ||
        (member(inherit_list(ob),"std/weapon.c")==-1 &&
         member(inherit_list(ob),"std/armour.c")==-1 ))
        return;
    if (old_dam>new_dam) // Repair
        flag=2;
    else if (new_dam>old_dam) // Damage
        flag=1;
    else
        return;
    if (!objectp(dam_ob))
        return;
    if (!(fn=explode(object_name(dam_ob),"#")[0]) || !stringp(fn))
        return;
    damagers[fn]=damagers[fn]|flag;
    save_me();
}

string dtdump(mixed arg)
{   string re;
    int i,w;

    if (stringp(arg))
        return capitalize(arg);
    if (!pointerp(arg) || !stringp(arg[0]))
        return "<NONE>";
    if ((i=sizeof(arg))==1)
        return capitalize(arg[0]);
    w = (31-i)/i;
    if (w--<1)
        return "<MANY>";
    for (re="",--i;i>=0;i--)
    {
        if (!stringp(arg[i]))
            re += "-";
        else
            re += capitalize(arg[i][0..w]);
        if (i)
            re += "|";
    }
    return re;
}

#define WP ({WT_HANDS,WT_KNIFE,WT_AXE,WT_SLASHING,WT_CLUB,WT_SWORD,WT_INFANTRY,\
          WT_STAFF,WT_SPEAR,WT_CHAIN,WT_THRUST,WT_WHIP,WT_SLING,WT_BOW,\
          WT_XBOW,WT_AMMU,WT_MAGIC,WT_MISC})
#define WA (m_indices(VALID_ARMOUR_CLASS))
int Stats(mixed what)
{
    int i, ws;
    mapping wpc, wwc;
    mixed *wv;   

    if (!this_interactive() || !IS_ARCH(this_interactive()))
        return -1;

    printf("Waffen:     %4i, \n"+
	   "Rüstungen: %4i, \n"+
	   "Damager:    %4i, \n",
	   
	   sizeof(weapons), sizeof(armours), sizeof(damagers));

    /* -- waffen -- */
    ws=sizeof(weapons); wpc=([]); wwc=([]);
    for(i=0; i<sizeof(WP); i++)
    {
	wpc=wpc+([WP[i]: 0]);
	wwc=wwc+([WP[i]: 0]);
    }
    wv=m_values(weapons);
    for(i=0; i<ws; i++)
    {
       wpc[wv[i][AWM_TYPE]]++;
       wwc[wv[i][AWM_TYPE]]=wwc[wv[i][AWM_TYPE]]+wv[i][AWM_CLASS];
    }
    printf("%-10s   %3s   %3s\n",
                "Waffentyp", "Anz", "dWC");

   for(i=0; i<sizeof(WP); i++)
   {
	printf("%-10s   %3i   %3i\n",
		WP[i], wpc[WP[i]],
		(wwc[WP[i]]?wwc[WP[i]]/wpc[WP[i]]:0));
   }

   /* -- Ruestungen -- */

   ws=sizeof(armours); wpc=([]); wwc=([]);
    for(i=0; i<sizeof(WA); i++)
    {
        wpc=wpc+([WA[i]: 0]);
        wwc=wwc+([WA[i]: 0]);
    }
    wv=m_values(armours);
    for(i=0; i<ws; i++)
    {
       wpc[wv[i][AWM_TYPE]]++;
       wwc[wv[i][AWM_TYPE]]=wwc[wv[i][AWM_TYPE]]+wv[i][AWM_CLASS];
    }
   
  printf("%-10s   %3s   %3s\n",
                "Rütyp", "Anz", "dAC");

   for(i=0; i<sizeof(WA); i++)
   {
        printf("%-10s   %3i   %3i\n",
                WA[i], wpc[WA[i]],
                (wwc[WA[i]]?wwc[WA[i]]/wpc[WA[i]]:0));
   }
    return 0;
}

int Dump(mixed what)
{   string  file,*ind;
    mapping dump;
    int     i;

    if (!what)
    {
        write("Nimm doch mal einen richtigen Parameter!\n");
        return 0;
    }
    if (stringp(what) && sizeof(what)>0)
    {
        what==what[0..0];
        if (what=="a")
            what=1;
        else if (what=="w")
            what=2;
        else if (what=="d")
            what=3;
        else
        {
            write("Nimm doch mal einen richtigen Parameter!\n");
            return 0;
        }
    }
    if (!intp(what) || what<1 || what>3)
    {
        write("Nimm doch mal einen richtigen Parameter!\n");
        return 0;
    }
    if (what==3) // Die 'damagers' haben ein anderes Ausgabeformat
    {
        printf("AWM: Dumping 'damagers' to '%s'\n",DDUMPFILE);
        ind=sort_array(m_indices(damagers),#'<);
        if ((i=sizeof(ind))<1)
        {
            write("AWM: Dump aborted, mapping empty.\n");
            return 1;
        }
        if (file_size(DDUMPFILE)>1)
            rm(DDUMPFILE);
        write_file(DDUMPFILE,sprintf(
            "--- Damagers-Dump --- %s --- %s ---\n\n"+
            "D R [Filename]\n",
            dtime(time()),capitalize(getuid(this_interactive()))));
        for (--i;i>=0;i--)
            write_file(DDUMPFILE,sprintf("%1s %1s %s\n",
                (damagers[ind[i]]&1?"+":"-"),
                (damagers[ind[i]]&2?"+":"-"),
                ind[i]));
        return 1; 
    }
    if (what==2)
        what=0;
    file=(what?ADUMPFILE:WDUMPFILE);

    printf("AWM: Dumping '%s' to '%s'\n",
        (what?"armours":"weapons"),file);
    
    dump=(what?armours:weapons);
    ind=sort_array(m_indices(dump),#'<);
    if ((i=sizeof(ind))<1)
    {
        write("AWM: Dump aborted, mapping empty.\n");
        return 1;
    }

    if (file_size(file)>1)
        rm(file);
    
    write_file(file,sprintf(
        "--- %s-Dump --- %s --- %s ---\n\n"+
        "[Filename]\n"+
        "    ____Typ___ CLS ECL XCL NFBR WGHT. VALUE H %30.30'_'|s\n",
        (what?"Rüstungs":"Waffen"),dtime(time()),
        capitalize(getuid(this_interactive())),"DamType(s)"));

    for (--i;i>=0;i--)
    {
        write_file(file,sprintf(
            "[%s]\n    %10s %3d %3d %3d %1s%1s%1s%1s %5d %5d %1d %-30.30s\n",
            ind[i],
            dump[ind[i]][AWM_TYPE],
            dump[ind[i]][AWM_CLASS],
            dump[ind[i]][AWM_EFF_CLASS],
            dump[ind[i]][AWM_X_CLASS],
           (dump[ind[i]][AWM_FLAGS]&AWF_PUTON?"+":"-"),
           (dump[ind[i]][AWM_FLAGS]&AWF_PUTOFF?"+":"-"),
           (dump[ind[i]][AWM_FLAGS]&AWF_BOOST?"+":"-"),
           (dump[ind[i]][AWM_FLAGS]&AWF_RESIST?"+":"-"),
            dump[ind[i]][AWM_WEIGHT],
            dump[ind[i]][AWM_VALUE],
            dump[ind[i]][AWM_HANDS],
            dtdump(dump[ind[i]][AWM_D_TYPE]) ) );
    }
    write("AWM: Done.\n");
    return 1;
}

int Unregister(string what)
{
    if (!this_interactive() || !IS_ARCH(this_interactive()))
        return -1;
    if (!what)
    {
        write("Du solltest schon einen Filenamen angeben!\n");
        return 0;
    } 
    if (member(armours,what))
    {
        efun::m_delete(armours,what);
        write("Unregistered "+what+" from 'armours'.\n");
        return 1;
    }
    if (member(weapons,what))
    {
        efun::m_delete(weapons,what);
        write("Unregistered "+what+" from 'weapons'.\n");
        return 1;
    }
    if (member(damagers,what))
    {
        efun::m_delete(damagers,what);
        write("Unregistered "+what+" from 'damagers'.\n");
        return 1;
    }
    return 0;
}
