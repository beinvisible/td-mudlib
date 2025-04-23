/* SilberLand MUDlib
 *
 * /secure/udp_mail.c -- UDP mail system
 *
 * $Date: 1997/03/15 23:50:36 $
 * $Revision: 1.1 $
 * $Log: udp_mail.c,v $
 * Revision 1.1  1997/03/15 23:50:36  Woody
 * prepared for revision control
 *
 */

/*
 * VERSION 1.0
 * UDP MAIL system (Author: Alvin@Sushi)
 * Requires INETD V0.60 or higher (INETD Author: Nostradamus@Zebedee)
 */

#include <udp.h>
#include <udp_mail.h>

#pragma strict_types

#define DEBUG(x) if (find_player("debugger")) tell_object(find_player("debugger"),x)

#undef DEBUG
#define DEBUG(x)

mapping spool_item;

static mixed spool;

private int match_mud_name(string mudname, string match_str) {
    return mudname[0..sizeof(match_str)-1] == match_str;
}

static void save_spool_item()
{
  string name;
  int count;

  if(!spool_item || !mappingp(spool_item) || spool_item==([]))
    return;

  do {
    ++count;
    name=spool_item[UDPMS_DEST]+"-"+to_string(count);
  } while(spool && member(name,spool)!=-1);

  save_object(UDPM_SPOOL_DIR+name);

  if(!spool || !sizeof(spool))
    spool = ({ name });
  else
    spool += ({ name });
}

/* forward declaration */
void deliver_mail(string recipient,string mud,string from,
	string subj,string mail_body,int status_flag,string spool_name);

/* forward declaration */
static void start_retry_callout();

static void remove_from_spool(string spool_file)
{
  int idx;

  if(spool && (idx=member(spool_file,spool))!=-1)
    {
      spool -= ({ spool_file });
      if(!sizeof(spool))
        spool=-0;
    }

  if(file_size("/"+UDPM_SPOOL_DIR+spool_file+".o")>0)
    if(!rm("/"+UDPM_SPOOL_DIR+spool_file+".o"))
      log_file(INETD_LOG_FILE,"UPD_MAIL: Can't delete spool file "+
		"/"+UDPM_SPOOL_DIR+spool_file+".o");
}

static void retry_send()
{
  int i;
  string msg;

  if(!spool || !sizeof(spool)) return;

  for(i=0;i<sizeof(spool);++i)
    {
      if(!restore_object(UDPM_SPOOL_DIR+spool[i]))
        {
          log_file(INETD_LOG_FILE,"UDP_MAIL: Falied to restore spool file "+
		UDPM_SPOOL_DIR+spool[i]);
          continue;
        }

      if(time() - spool_item[UDPMS_TIME] > UDPM_SEND_FAIL*60)
        {
          msg="Reason: Unable to connect to site \""+spool_item[UDPMS_DEST]+
		"\"\n\nINCLUDED MESSAGE FOLLOWS :-\n\n"+
		"To: "+spool_item[UDPMS_TO]+"\n"+
		"Subject: "+spool_item[UDPMS_SUBJECT]+"\n"+
		spool_item[UDPMS_BODY];

          LOCAL_MAILER->deliver_mail(
		spool_item[UDPMS_FROM],	/* TO */
		"Mailer@"+LOCAL_NAME,	/* FROM */
		"Bounced Mail",		/* SUBJECT */
		msg			/* MAIL BODY */
	  );
          remove_from_spool(spool[i]);
          return;
        }

      deliver_mail(
	spool_item[UDPMS_TO],
	spool_item[UDPMS_DEST],
	spool_item[UDPMS_FROM],
	spool_item[UDPMS_SUBJECT],
	spool_item[UDPMS_BODY],
	UDPM_STATUS_IN_SPOOL,
	spool[i]);
    }

  start_retry_callout();
}

static void start_retry_callout()
{
  if(find_call_out("retry_send")!= -1 ) return;

  call_out("retry_send",UDPM_RETRY_SEND*60);
}

static void failed_to_deliver(mapping data)
{
  string msg;
  object obj;

  if(!data[SYSTEM] || data[SYSTEM] != TIME_OUT)
    {
      msg="Reason: Error in connection to remote site \""+data[NAME]+"\"\n\n"+
	"INCLUDED MESSAGE FOLLOWS :-\n\n"+
	"To: "+data[RECIPIENT]+"\n"+
	"Subject: "+data[UDPM_SUBJECT]+"\n"+data[DATA];

      LOCAL_MAILER->deliver_mail(
		data[UDPM_WRITER],	/* TO */
		"Mailer@"+LOCAL_NAME,	/* FROM */
		"Bounced Mail",		/* SUBJECT */
		msg			/* MAIL BODY */
      );
      return;
    }

  /* OK transmission timed out.. place in mail spool */
  
  if((obj=find_player(data[UDPM_WRITER])))
    {
      tell_object(obj,"Mail delivery to "+data[RECIPIENT]+"@"+data[NAME]+
	" Timed Out. Placing mail in spool.\n");
    }

  spool_item=([
	UDPMS_TIME:	time(),
	UDPMS_TO:	data[RECIPIENT],
	UDPMS_DEST:	data[NAME],
	UDPMS_FROM:	data[UDPM_WRITER],
	UDPMS_SUBJECT:	data[UDPM_SUBJECT],
	UDPMS_BODY:	data[DATA]
  ]);

  save_spool_item();

  start_retry_callout();
}

static void get_pending_deliveries()
{
  string *entries;
  int i;

  entries=get_dir(UDPM_SPOOL_DIR+"*.o");
  if(!entries || !sizeof(entries)) return;

  spool=allocate(sizeof(entries));
  for(i=0;i<sizeof(entries);++i)
    spool[i]=entries[i][0..<3];

  start_retry_callout();
}

void create()
{
	seteuid(getuid(this_object()));
  get_pending_deliveries();
}

/*
 * Public routines
 */

int query_valid_mail_host(string hostname)
{
  string *match;

  match=filter(m_indices(({mapping})INETD->query("hosts")),
		#'match_mud_name,lower_case(hostname));

  return (sizeof(match)==1);
}

void deliver_mail(string recipient,string mud,string from,
	string subj,string mail_body,int status_flag,string spool_name)
{
  mapping data;

  data=([
	REQUEST: "mail",
	RECIPIENT: recipient,
        SENDER: this_object(),
	UDPM_STATUS: status_flag,
	UDPM_WRITER: lower_case(from),
	UDPM_SUBJECT: subj,
        UDPM_SPOOL_NAME: spool_name,
	DATA: mail_body
  ]);

  INETD->send_udp(mud,data,1);
}

void udp_reply(mapping data)
{
  object sender;

  DEBUG(sprintf("MAILER RECEIVED %O\n",data));
  if (!member(data,UDPM_STATUS))
  {
     DEBUG("BOUNCING\n");
     LOCAL_MAILER->deliver_mail(
        data[UDPM_WRITER],	/* TO */
	"INETD@"+data[NAME],	/* FROM */
	"Bounced Mail(No mail support yet?)",		/* SUBJECT */
	data[DATA]		/* MAIL BODY */
	);
        if(data[UDPM_SPOOL_NAME])
          remove_from_spool(data[UDPM_SPOOL_NAME]);
  } else
  switch(data[UDPM_STATUS])
    {
      case UDPM_STATUS_TIME_OUT:
        failed_to_deliver(data);
        break;

      case UDPM_STATUS_DELIVERED_OK:
        if((sender=find_player(data[UDPM_WRITER])))
          {
            tell_object(sender,"Mailer@"+data[NAME]+": "+
		"Mail to "+capitalize(data[DATA])+" delivered ok.\n");
          }
        if(data[UDPM_SPOOL_NAME])
          remove_from_spool(data[UDPM_SPOOL_NAME]);

        break;

      case UDPM_STATUS_UNKNOWN_PLAYER:
        LOCAL_MAILER->deliver_mail(
		data[UDPM_WRITER],	/* TO */
		"Mailer@"+data[NAME],	/* FROM */
		"Bounced Mail",		/* SUBJECT */
		data[DATA]		/* MAIL BODY */
	);
        if(data[UDPM_SPOOL_NAME])
          remove_from_spool(data[UDPM_SPOOL_NAME]);
        break;

	case UDPM_STATUS_IN_SPOOL:
          /* Do nothing */
          break;
    }
}
