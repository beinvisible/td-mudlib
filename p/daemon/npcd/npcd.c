
#pragma strong_types

#include <properties.h>
#include "/p/daemon/npcd/npcd.h"

#define DB(x)   "/room/debug"->debug_message(this_object(), x)

object  *npcs;

void npc_class_check(object npc)
{
    // DB(sprintf("%O", npc->QueryProp(P_CLASS)));
    /*
    if(!sizeof(npc->QueryProp(P_CLASS)))
    {
        DB(sprintf("%s hat KEINE AddClass()!",
            efun::explode(object_name(npc), "#")[0]));
    }
    */
}

/* Accessfunktionen --------------------------------------------------- */
int count_npcs() 
{ return sizeof(npcs); }

int is_registered(object npc)
{
    return (member(npcs, npc) != -1);
}

/* Grundfunktionen ---------------------------------------------------- */
int Register(object npc)
{
    if(!npc) return 0;
    if(member(npcs, npc) != -1) return 0;
    
    // DB(sprintf("Registering %O", npc));
    npcs = npcs + ({ npc });
    
    call_out("npc_class_check", 1, npc);
}

int Unregister(object npc)
{
    if(!npc) return 0;
    if(member(npcs, npc) == -1) return 0;
    
    // DB(sprintf("Unregistering %O", npc));
    npcs = npcs - ({ npc });
}

/* -------------------------------------------------------------------- */
void reset()
{
    // DB("removing missing NPCs");
    // DB(sprintf("count=%i", count_npcs()));
    npcs = filter(npcs, #'objectp);
    // DB(sprintf("cleaned=%i", count_npcs()));
}

void create()
{
    npcs = ({});
}