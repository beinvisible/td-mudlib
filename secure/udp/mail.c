/* SilberLand MUDlib
 *
 * /secure/udp/mail.c -- UDP mail handling
 *
 * $Date: 1997/03/16 00:11:33 $
 * $Revision: 1.1 $
 * $Log: mail.c,v $
 * Revision 1.1  1997/03/16 00:11:33  Woody
 * prepared for revision control
 *
 */

/*
 * VERSION 1.0
 * udp module for the UDP MAIL system (Author: Alvin@Sushi)
 * Requires INETD V0.60 or higher (INETD Author: Nostradamus@Zebedee)
 */

#include <udp.h>
#include <udp_mail.h>

#define DEBUG(msg) if (find_player("debugger")) tell_object(find_player("debugger"),msg)
#undef DEBUG
#define DEBUG(x)

void udp_mail(mapping data)
{
	DEBUG(sprintf("UDPMAIL %O\n",data));
  if(!member(data,RECIPIENT) || !data[RECIPIENT])
    {
      log_file("INETD","Invalid udp_mail packet. No Recipient.\n");
      return;
    }

  if(!LOCAL_MAILER->query_recipient_ok(data[RECIPIENT]))
    {
      INETD->send_udp(data[NAME], ([
	    REQUEST: REPLY,
	    RECIPIENT: data[SENDER],
	    UDPM_STATUS: UDPM_STATUS_UNKNOWN_PLAYER,
	    UDPM_WRITER: data[UDPM_WRITER],
	    UDPM_SPOOL_NAME: data[UDPM_SPOOL_NAME],
	    ID: data[ID],
	    DATA: "Reason: Unknown player \""+capitalize(data[RECIPIENT])+
		"\"\n\nINCLUDED MESSAGE FOLLOWS :-\n\n"+
		"Subject: "+data[UDPM_SUBJECT]+"\n"+data[DATA]
      ]) );

      return;
    }

  LOCAL_MAILER->deliver_mail(
	data[RECIPIENT],			/* To */
	data[UDPM_WRITER]+"@"+data[NAME],	/* From */
	data[UDPM_SUBJECT],			/* Subj */
	data[DATA]				/* Mail Body */
  );

  INETD->send_udp(data[NAME], ([
	    REQUEST: REPLY,
	    RECIPIENT: data[SENDER],
	    UDPM_STATUS: UDPM_STATUS_DELIVERED_OK,
	    UDPM_WRITER: data[UDPM_WRITER],
	    UDPM_SPOOL_NAME: data[UDPM_SPOOL_NAME],
	    ID: data[ID],
	    DATA: data[RECIPIENT]
  ]) );
}
