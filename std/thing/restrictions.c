// SilberLand MUDlib
//
// std/thing/restrictions.c -- thing restrictions
//
// $Date: 2001/02/10 12:08:39 $                      
/* $Revision: 3.2 $       
 * $Log: restrictions.c,v $
 * Revision 3.2  2001/02/10 12:08:39  Samtpfote
 * prepared for revision control
 *
 * Revision 3.2  1997/03/13 02:35:58  Woody
 * prepared for revision control
 *
 * Revision 3.1  1995/12/11 20:02:30  Rochus
 * Set-Methode fuer externe Attribut Modifier,
 * ruft automatisch UpdateAttributes() auf und loggt mit.
 *
 * Revision 3.0  1995/11/22 14:32:48  Jof
 * Updated to revision 3.0
 *
 * Revision 2.7  1995/11/22 14:19:50  Jof
 * Fixed RCS header
 *
 * Revision 2.6  1994/09/23 20:20:55  Hate
 * added SetProp(P_WEIGHT, 1000) <- from description
 *
 * Revision 2.5  1994/06/13  07:36:24  Jof
 * Removed SET_METHODs
 *
 * Revision 2.5  1994/06/13  07:36:24  Jof
 * Removed SET_METHODs
 *
 * Revision 2.4  1994/03/21  16:48:13  Jof
 * Reindented files using emacs - once again
 *
 * Revision 2.3  1994/03/18  11:56:25  Jof
 * Fixed indentation
 *
 * Revision 2.2  1994/02/24  13:05:25  Jof
 * Private removed
 *
 * Revision 1.3  1994/01/04  10:46:50  mud
 * NEED_PROTOTYPES added
 */                            

#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone

// #include <player/base.h>
// #include <thing/moving.h>
// #include <thing/language.h>
// #include <attributes.h>
// #include <defines.h>
#include <properties.h>

#define NEED_PROTOTYPES 1
#include <thing/properties.h>

void create()
{
  SetProp(P_WEIGHT, 1000);
  // Set(P_X_ATTR_MOD, PROTECTED, F_MODE);
}

// **** local property methods
static int _set_weight(int weight) {
  return SetProp(P_TOTAL_WEIGHT,
	  Set(P_WEIGHT, (intp(weight) && !QueryProp(P_AUTOLOADOBJ)) ? 
	      weight : 0) );
  /* Redefined by container/restrictions */
}

static mapping _set_extern_attributes_modifier(mapping xmod) {
  mapping res;
  object env;

  if (!QueryProp(P_SENSITIVE))
    SetProp(P_SENSITIVE,({})); // Damit Insert/Remove-Funktion ausgeloest wird
  if (mappingp(xmod))
    log_file("XATTRMOD",
	     sprintf("%s %2d %2d %2d %2d (%O)\n",
		     ctime(time())[4..15],
		     xmod[A_INT],xmod[A_STR],xmod[A_CON],xmod[A_DEX],
		     previous_object()));
  else
    xmod=([]);
  res=Set(P_X_ATTR_MOD,xmod);
  if (living(env=environment(this_object())))
    env->UpdateAttributes();
  return res;
}
