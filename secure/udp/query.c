/* Tamedhon MUDlib
 *
 * /secure/udp/query.c -- UDP muds query
 *
 * $Date: 2002/02/21 08:45:00 $
 * $Revision: 1.4 $
 * $Log: query.c,v $
 * Revision 1.4  2002/02/21 08:45:00  Querolin
 * uptime & exist_days() ... eingebaut.
 * 
 * Revision 1.3  2000/12/21 09:58:00  Querolin
 * Email Adresse fuer Tame geaendert
 *
 * Revision 1.2  1997/03/16 00:14:34  Woody
 * Email Adresse fuer SL geaendert
 *
 * Revision 1.1  1997/03/16 00:14:06  Woody
 * prepared for revision control
 *
 */

#include <udp.h>
#include <config.h>

#ifdef ZEBEDEE
#include <system.h>
#elif !defined(INETD)
#define INETD	"/secure/inetd"
#endif

/* Mud / Admin email address. */
#define EMAIL	"info@fantasywelt.net"

void udp_query(mapping data) {
    mapping ret;

    switch(data[DATA]) {
	case "commands":
	    ret = INETD->query("hosts");
	    if (ret[lower_case(data[NAME])])
		ret = ([
		DATA: implode(ret[lower_case(data[NAME])][LOCAL_COMMANDS], ":")
		]);
	    else
		ret = ([ DATA: implode(INETD->query("commands"), ":") ]);
	    break;
	case "email":
	    ret = ([ DATA: EMAIL ]);
	    break;
	case "encoding":
	    ret = ([ DATA: UDP_ENCODING ]);
	    break;
	case "hosts":
	{
	    mapping hosts;
	    string *list;
	    string tmp;
	    int i;

	    tmp = "";
	    for(i = sizeof(list = m_indices(hosts = INETD->query("hosts")));
	    i--; ) {
		tmp +=
		hosts[list[i]][HOST_NAME] + ":" +
		hosts[list[i]][HOST_IP] + ":" +
		hosts[list[i]][HOST_UDP_PORT] + ":" +
		implode(hosts[list[i]][LOCAL_COMMANDS], ",") + ":" +
		implode(hosts[list[i]][HOST_COMMANDS], ",");
		if (i)
		    tmp += "\n";
	    }
	    ret = ([ DATA: tmp ]);
	    break;
	}
	case "inetd":
	    ret = ([ DATA: INETD_VERSION ]);
	    break;
	case "list":
	    /* List of thingsthat can be queried. */
	    ret = ([ DATA: "commands:email:encoding:hosts:inetd:mud_port:time:version" ]);
	    break;
	case "mud_port":
	    ret = ([ DATA: query_mud_port() ]);
	    break;
	case "time":
	    ret = ([ DATA: time() ]);
	    break;
	case "version":
	    ret = ([ DATA: version() ]);
	    break;
        case "uptime":
            ret = ([ DATA: uptime()+"\n" ]);
            break;
        case "exists":
            ret = ([ DATA: exist_days()+" Tagen\n" ]);
            break;
        case "lastreboot":
            ret = ([ DATA: "am "+dtime(last_reboot_time())+"\n" ]);
            break;
        case "users":
            ret = ([ DATA: sizeof(users())+" Anwesende\n" ]);
            break;
        case "questliste":
            ret = ([ DATA: "\n"+find_object("/p/service/querolin/listen/questliste")->liste()+"\n" ]);
            break;

	default:
	    /* Just ignore it for the time being. */
	    return;
    }
    INETD->send_udp(data[NAME], ret + ([
	REQUEST: REPLY,
	RECIPIENT: data[SENDER],
	ID: data[ID],
	"QUERY": data[DATA]
    ]) );
}
