// Tamedhon MUDlib
//
// std/player/channel.c -- Channel Client
//
// $Date: 2008/02/10 12:00:00 $
/* $Revision: 3.3 $
 * $Log: channel.c,v $
 *
 * Revision 3.3  2008/02/10 12:00:00  TamTam
 * Ebenenhistory mit flexibler Laenge: -*<nr><Ebene>
 *
 * Revision 3.2  2004/11/09 00:00:00  Serii
 * verbesserte Ebenen-handhabung (anpassung an den neuen Channeld)
 *
 * Revision 3.1  2001/06/04 14:51:00  Querolin
 * Grats und Info hinzugefuegt bei std-channels
 *
 * Revision 3.0  1997/03/13 01:33:36  Woody
 * prepared for revision control
 *
 * Revision 1.1  1996/10/03  Woody
 * create() eingefuehrt und dafuer comm::create() abgemagert
 */

#pragma strong_types
#pragma save_types

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <wizlevels.h>
#include <defines.h>
#include <properties.h>
#include <daemon.h>
#include "/std/sys_debug.h"

#define ERROR(msg)  printf("Fehler: %s\n", msg)

#define P_SWAP_CHANNELS  "swap_channels"

void create()
{
  Set(P_CHANNELS, SAVE, F_MODE);
  Set(P_CHANNELS, ({"Tod","Abenteuer","Allgemein","Grats","Info","Gelaber"}));
  Set(P_STD_CHANNEL, "a");
  Set(P_STD_CHANNEL, SAVE, F_MODE);
}

static mixed *_query_localcmds()
{
  return ({({"-","channel_parser",1,0})});
}

int channel_parser(string str)
{
  mixed tokens, channel, tmp;
  string cmd, message;
  int i, type, histlen;

  notify_fail("Benutzung: -[+-~?]<Ebene>[' :;]<Text> oder -! -!! -%\n");
  cmd = query_verb()[1..];
  str=_unparsed_args();
  if(!str && !sizeof(cmd)) return 0;
  tokens = regexplode(cmd, "[~+-?!\*]*[a-z][a-z]*|[':;@].*|[%]")-({""});
  channel = ({});
  while(sizeof(tokens))
  {
    string token;
    token = tokens[0];
    tokens = tokens[1..];
    switch(token[0..0])
    {
    case "~": if(sizeof(token)<2)
              { ERROR("Fehlendes Argument zu '~'."); break; }
              SetProp(P_STD_CHANNEL, token[1..]+(sizeof(tokens)?implode(tokens,""):""));
              printf("Als Standardebene wird '%s' genommen.\n", token[1..]);
              break;
    case "+": if(member(token, '!') != -1)
                VOICEMASTER->RegisterChannel("", ME);
              else
                VOICEMASTER->RegisterChannel(token[1..]+(sizeof(tokens)?implode(tokens,""):""), ME);
              break;
    case "-": if(member(token, '!') != -1)
                VOICEMASTER->RemoveChannel("", ME);
              else
                VOICEMASTER->RemoveChannel(token[1..]+(sizeof(tokens)?implode(tokens,""):""), ME);
              break;
    case "?": VOICEMASTER->ListUsers(token[1..]+((sizeof(tokens)?implode(tokens,""):"")), ME);
              break;
    case "!": if(sizeof(token)==1) VOICEMASTER->ListChannels(ME);
              else
                if(token[1] == '!') VOICEMASTER->ListChannels();
                else ERROR("Falsches Argument zu '!', nur ein weiteres '!' "
                          +"erlaubt!");
              break;
    case "*": if (sizeof(token) == 1)
		VOICEMASTER->ChannelHistory(QueryProp(P_STD_CHANNEL), ME);
              else
              { // -*<nr><Ebene>
                if(to_int(token[1..]) || token[1..1]=="0")
                  histlen=sizeof(to_string(to_int(token[1..])));
                else
                  histlen=0; // -*<Ebene>
		VOICEMASTER->ChannelHistory(token[(1+histlen)..]+(sizeof(tokens)?implode(tokens,""):""),
		                            ME, to_int(token[1..]));
              }
              break;
    case "%":
              if(!PL->QueryProp(P_SWAP_CHANNELS))
              {
                VOICEMASTER->RemoveUser(ME);
                PL->SetProp(P_SWAP_CHANNELS, QueryProp(P_CHANNELS));
                if(!Query(P_SWAP_CHANNELS, F_MODE) & SAVE)
                  PL->Set(P_SWAP_CHANNELS, SAVE, F_MODE);
                printf("Alle Ebenen abgeschaltet.\n");
              } else
              {
                PL->SetProp(P_CHANNELS, QueryProp(P_SWAP_CHANNELS));
                VOICEMASTER->RegisterUser(ME);
                PL->SetProp(P_SWAP_CHANNELS, 0);
                if(Query(P_SWAP_CHANNELS, F_MODE) & SAVE)
                  PL->Set(P_SWAP_CHANNELS, SAVE, F_MODE);
                printf("Alle Ebenen wieder zugeschaltet.\n");
              } break;
/*    case "@": if(sizeof(token)>1)
              {
                VOICEMASTER->SendCommand(token[1..]
                                       +(sizeof(tokens)?implode(tokens, ""):"")
                                       +(str?":"+str:""));
                tokens = ({});
              }*/
    case "'": type = SAY;
    case ";": if(!type) type = GEMOTE;
    case ":": if(!type) type = EMOTE;
              message = (sizeof(token)>1 ? token[1..] : "")
                       +(sizeof(tokens) ? implode(tokens, "") : "")
                       +(str?" "+str:"");
              tokens = ({});
              break;
    default:  channel += efun::explode(token, "")-({""});
    }
  }
  if(!message && stringp(str)) message = str;
  if(!type && message) type = SAY;
  if(message && type)
  {
    object vm;
    string *ch; ch = ({});
    if(!sizeof(channel)) channel = ({ QueryProp(P_STD_CHANNEL) });
    if(!vm = find_object(VOICEMASTER))
    {
      VOICEMASTER->ReloadingVoiceMaster();
      vm = find_object(VOICEMASTER);
    }
    if(!IS_SEER(ME) && QueryProp(P_SP) < USAGE_COST)
        return (int)ERROR("Du bist mental zu erschöpft.\n");
    vm->SendToChannel(implode(channel,""),ME,type,message);
  }
  return 1;
}

