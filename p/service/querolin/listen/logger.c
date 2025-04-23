/* eine Top-Alters-Liste
 * Querolin@Tamedhon 09.09.2001
 *
 * Abenteurer Toplistenmaster
 * Sun Sep 17 13:39:47 2000, Samtpfote@Tamedhon
 *
 * $Date: 2001/01/12 19:09:47 $
 * $Revision: 1.1 $
 * $Log: abgstatmaster.c,v $
 * Revision 1.1  2001/01/12 19:09:47  Samtpfote
 * Initial revision
 *
 */

/*
    Eingetragen in: 
*/
 

#pragma strong_types

#include "def.h"
#include <properties.h>

#define ADB(x)          if(find_player("querolin")) \
                            tell_object(find_player("querolin"), \
                            break_string(x, 79, "{ logger } "));

/* --- Hilfsfunktionen ------------------------------------------------- */

int log(string file, string text)
{
    return write_file(file, text+"\n");
}

void create()
{
    seteuid(getuid());
}
