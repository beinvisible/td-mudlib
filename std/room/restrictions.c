// SilberLand MUDlib
//
// std/room/restrictions.c -- weight property handling for rooms
//
// $Date: 1997/10/16 15:48:59 $                      
/* $Revision: 3.1 $       
 * $Log: restrictions.c,v $
 * Revision 3.1  1997/10/16 15:48:59  Woody
 * MayAddWeight() mit Parameter mixed
 *
 * Revision 3.0  1997/03/13 02:04:15  Woody
 * prepared for revision control
 *
 * Revision 2.6  1994/10/04  09:55:26  Jof
 * *** empty log message ***
 *
 * Revision 2.6  1994/10/04  14:41:59  mud
 * *** empty log message ***
 *
 * Revision 2.5  1994/07/19  16:14:08  Kirk
 * *** empty log message ***
 *
 * Revision 2.4  1994/07/12  00:09:10  Kirk
 * clean up
 *
 * Revision 2.3  1994/06/13  07:36:24  Jof
 * Removed SET_METHODs
 *
 * Revision 2.2  1994/03/21  16:48:13  Jof
 * Reindented files using emacs - once again
 *
 * Revision 2.1  1994/02/24  13:04:21  Jof
 * Private removed
 *
 * Revision 2.0  1994/01/13  17:00:28  mud
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/08  13:08:41  mud
 * changed to #pragma strong_types
 *             
 */                            

inherit "std/container/restrictions";

#pragma strong_types

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <properties.h>

static int _set_weight(int w);
static int _query_weight();
static int _set_total_weight(int w);
static int _query_total_weight();

void create()
{
  Set(P_WEIGHT,PROTECTED,F_MODE);
  Set(P_TOTAL_WEIGHT,PROTECTED,F_MODE);
}

varargs int MayAddWeight(mixed w)
{
  return 0;
}

varargs int PreventInsert(object ob)
{
  return 0;
}

static int _set_weight(int w)
{
  return 0;
}

static int _query_weight()
{
  return 0;
}

static int _set_total_weight(int w)
{
  return 0;
}

static int _query_total_weight()
{
  return 0;
}
