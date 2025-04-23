/* SilberLand MUDlib
 *
 * /secure/udp/channel.c -- UDP channel handling
 *
 * $Date: 1997/04/14 14:38:48 $
 * $Revision: 1.2 $
 * $Log: channel.c,v $
 * Revision 1.2  1997/04/14 14:38:48  Woody
 * Channelausgabe nicht mehr per EMPTY sondern SAY/EMOTE
 *
 * Revision 1.1  1997/03/16 00:11:09  Woody
 * prepared for revision control
 *
 */

#include <udp.h>

#include <properties.h>
#include <daemon.h>

#ifdef ZEBEDEE
inherit "/sys/format";
#endif

#define COMMAND		"cmd"
#define CHANNEL		"channel"

private static mixed _name_;

int filter_listeners(object ob, string channel) {
    return ob->QueryProp(P_INTERMUD);
}

string name() { return _name_ ? _name_ : "<Intermud>"; }

int udp_channel(mapping data) {
  object *list;
  string msg;
  int i;

  /* Compatability with older systems. */
  if (!data[CHANNEL])
    data[CHANNEL] = data["CHANNEL"];
  if (!data[COMMAND])
    data[COMMAND] = data["CMD"];

  data[DATA]=
    implode(filter(efun::explode(data[DATA], ""), #'>=, " "), "");//'))
  data[NAME]=
    implode(filter(efun::explode(data[NAME], ""), #'>=, " "), "");//'))
  switch(data[COMMAND]) {
    case "emote":
      /* A channel emote. */
      /*
      msg = sprintf("%-80=s", "[" + capitalize(data[CHANNEL]) + ":" +
                    capitalize(data[SENDER]) + "@" + capitalize(data[NAME]) +
                    " " + data[DATA] + "]") + "\n";
      */
      msg = data[DATA];
      break;
    case "list":
      /* Request for a list of people listening to a certain channel. */
      list = filter(users(), "filter_listeners", this_object(),
          data[CHANNEL]);
      if (i = sizeof(list)) {
        msg = "[" + capitalize(data[CHANNEL]) + "@" +
          LOCAL_NAME + "] Listening:\n";
        while(i--)
          msg +=
            "    " + capitalize(list[i]->query_real_name()) + "\n";
      }
      else
        msg = "[" + capitalize(data[CHANNEL]) + "@" +
          LOCAL_NAME + "] Nobody Listening.\n";
      INETD->send_udp(data[NAME], ([
                                    REQUEST: REPLY,
                                    RECIPIENT: data[SENDER],
                                    ID: data[ID],
                                    DATA: msg
                                    ]) );
      return 1;
    default:
      /* A regular channel message. */
      /*
      msg = sprintf("%-80=s", "[" + capitalize(data[CHANNEL]) + ":" +
                    capitalize(data[SENDER]) + "@" + capitalize(data[NAME]) +
                    "] " + data[DATA]) + "\n";
      msg = break_string(data[DATA], 78,
          "["+capitalize(data[CHANNEL])+":"+capitalize(data[SENDER])+"@"+
          capitalize(data[NAME])+"] ");
      */
      msg = data[DATA];
      break;
  }
  /*
  VOICEMASTER->SendToChannel(capitalize(data[CHANNEL]), this_object(), EMPTY,
			     msg);
  */
  _name_ = capitalize(data[SENDER])+"@"+capitalize(data[NAME]);
  VOICEMASTER->SendToChannel(capitalize(data[CHANNEL]), this_object(),
      data[COMMAND]=="emote" ? EMOTE : SAY, msg);
  _name_ = 0;
  return 1;
}
