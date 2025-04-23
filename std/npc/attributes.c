// SilberLand MUDlib
//
// std/npc/attributes.c -- attributes for NPCs
//
// $Date: 1997/06/25 11:57:11 $
/* $Revision: 1.1 $
 * $Log: attributes.c,v $
 * Revision 1.1  1997/06/25 11:57:11  Woody
 * Initial revision
 *
 */


#pragma strong_types

inherit "std/living/attributes";

#include <npc/attributes.h>

static int _filterattr_str(int val)
{
  if( val<0 ) return 0;
  if( val>A_MAX_NPC ) return A_MAX_NPC;
  return val;
}

static int _filterattr_dex(int val)
{
  if( val<0 ) return 0;
  if( val>A_MAX_NPC ) return A_MAX_NPC;
  return val;
}

static int _filterattr_int(int val)
{
  if( val<0 ) return 0;
  if( val>A_MAX_NPC ) return A_MAX_NPC;
  return val;
}

static int _filterattr_con(int val)
{
  if( val<0 ) return 0;
  if( val>A_MAX_NPC ) return A_MAX_NPC;
  return val;
}

