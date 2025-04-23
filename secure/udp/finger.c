/* Tamedhon MUDlib
 *
 * /secure/udp/finger.c -- UDP finger request handling
 *
 * $Date: 2002/05/11 13:00:00 $
 * $Revision: 1.3 $
 * $Log: finger.c,v $
 * Revision 1.3  2002/05/11 13:00:00  Querolin
 * Div. Fixes.
 *
 * Revision 1.2  1997/03/17 19:35:16  Woody
 * Neue Verzeichnisstruktur in /p beruecksichtigt (finger nicht mehr in loco)
 *
 * Revision 1.1  1997/03/16 00:08:34  Woody
 * prepared for revision control
 *
 */

#include <udp.h>
#include <daemon.h>
#include <wizlevels.h>

#ifdef ZEBEDEE
#include <system.h>
#elif !defined(INETD)
#define INETD	"/secure/inetd"
#endif

#define DB(x)       if(find_player("samtpfote")) \
                tell_object(find_player("samtpfote"), \
                                break_string(x, 79, "{ "+object_name(this_object())+" } "));

void udp_finger(mapping data) {
    object pl;

    pl=find_player(data[DATA]);
    if(pl && IS_WIZARD(pl))
    {
       tell_object(pl,
                sprintf("%s@%s informierte sich gerade über Dich.\n",
                 capitalize(data[SENDER]), data[NAME]));

    }


    //DB(sprintf("data=%O\n", data));
    INETD->send_udp(data[NAME], ([
	REQUEST: REPLY,
	RECIPIENT: data[SENDER],
	ID: data[ID],
	DATA: FINGERMASTER->finger_single(data[DATA])
    ]) );

}

