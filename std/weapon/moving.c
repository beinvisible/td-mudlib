// SilberLand MUDlib
//
// std/weapon/moving.c -- weapon moving object
//
// $Date: 1997/06/25 12:12:06 $                      
/* $Revision: 3.1 $       
 * $Log: moving.c,v $
 * Revision 3.1  1997/06/25 12:12:06  Woody
 * in move() ist DoUnwield nun silent und nocheck
 *
 * Revision 3.0  1997/03/13 02:46:27  Woody
 * prepared for revision control
 *
 * Revision 2.2  1995/06/02  13:00:40  Rumata
 * move made varargs
 *
 * Revision 2.1  1995/06/02  12:44:37  Hate
 * Ne geschrieben, da verschwunden (aus weapon.c) (Rumata)
 *
 */                            

#pragma strong_types

inherit "/std/thing/moving";

#define NEED_PROTOTYPES 1

#include <thing/properties.h>
#include <properties.h>
#include <moving.h>
#include <defines.h>
#include <weapon.h>

varargs int DoUnwield(int silent);

varargs int move(mixed dest, int method)
{
  DoUnwield( method & (M_SILENT|M_NOCHECK));
  if ((method & M_NOCHECK) || (!QueryProp(P_WIELDED)))
    return ::move(dest, method );
  return 0;
}

varargs int remove(int silent )
{
  DoUnwield( silent );
  if (!QueryProp(P_WIELDED))
    return ::remove( silent );
  return 0;
}
