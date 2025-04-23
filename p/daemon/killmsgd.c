/* Moerder-Meldungen-Master
 * Samtpfote@Tamedhon.at
 *
 * Damit die Leiche damit entlastet wird, und dass man da etwas flexibler
 * arbeitet.
 *
 * Der Master gibt *nur* einen string zurueck, sonst nix.
 *
 * Verschiedene Sachen, die dann da automatisch im String ersetzt werden:
 *
 *   %s        Spielername
 *   %E        einer der Erzmagier
 *   %G        Name der Gilde
 *   %W        aktuelle gezueckte Waffe
 *   %i	       zufaelliger Nicht-Autoloadgegenstand im Inv (25.8.2002 pfote)
 *                                                   (wieder abgebaut pfote)
 *   %r        Rasse des Opfers (21.12.2002 pfote)
    -"-        deaktiviert aufgrund bug. 20.07.2003 quero
 *
 * Aenderungen:
 * 02.01.2008  TamTam   Erzmagier Tamtam und Querolin hinzugefuegt
 * 04.08.2009  Grynnis  statt "Ein Frosch" nun "Frosch <Name>"
 */


#pragma strong_types

#include <language.h>
#include <properties.h>

#define MOERDERMSGS "/p/daemon/save/killmsgs.txt"
#define ERZMAGIER   ({"Gralkor", "Aloy", "Querolin","Kati"})

string  *msgs;

int query_msg_count() { return sizeof(msgs); }

int no_autoload(object ob)
{
	return !(ob->QueryProp(P_AUTOLOADOBJ));
}

int only_weapon_armour(object ob)
{
	return (ob->QueryProp(P_WEAPON_TYPE) ||
                ob->QueryProp(P_ARMOUR_TYPE));
}

string generate_msg(object killer)
{
    string      msg, wn, kname;
    int		i, is;
    object	*inv, *wp;

    if(!killer) return "";

    msg = msgs[random(sizeof(msgs))];

    /* Rasse des MONSTERs */
    //if(strstr(msg, "%r") != -1)
    //{ msg = regreplace(msg, "%r", previous_object()->QueryProp(P_RACE), 1); }

    /* Spielername */
    if(strstr(msg, "%s") != -1)
    {
        kname = killer->Name(WER, 1);
        if (killer->QueryProp(P_FROG))
            kname = "Frosch " + capitalize(geteuid(killer));
        msg = regreplace(msg, "%s", kname, 1); }

    /* Erzmagier */
    if(strstr(msg, "%E") != -1)
    {
        msg = regreplace(msg, "%E",
                ERZMAGIER[random(sizeof(ERZMAGIER))], 1);
    }

    /* Gilde des Spielers */
    if(strstr(msg, "%G") != -1)
    {
        msg = regreplace(msg, "%G", capitalize(killer->QueryProp(P_GUILD)), 1);
    }

    return msg;
}

void reload_msgs()
{
    string	fdata;

    fdata = read_file(MOERDERMSGS);
    if(!fdata)
    {
	fdata = "Beim Lesen von den Mördermeldungen trat ein Problem auf!";
    }

    msgs = efun::explode(fdata, "\n");
}

void create()
{
    msgs = ({});
    reload_msgs();
}
