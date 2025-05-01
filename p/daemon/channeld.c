// Tamedhon MUDlib
//
// /p/daemon/channeld.c -- (Tamedhon) Channel Daemon V5
//
// 2024/04/15 - Serii schrott entfernt, seteuid sefun, break_string sefun - Aloy
// 2024/04/11 - sefun query_once_interactive, livings sefuns, query_wiz_level - Aloy
//
// $Date: 2012/05/17 13:00:00 $
/* $Revision: 0.20 $
 * $Log: channeld.c,v $
 *
 * Revision 0.21  2024-02-25 1900 Gralkor
 * Goblinrassenkanal eingefuehrt. Brownies gelassen, wenn auch unnoetig
 *
 * Revision 0.20  2012/05/17 13:00:00  Grynnis
 * channelmember() nicht private, Absicherung in channelmember()
 *
 * Revision 0.19  2009/08/07 15:00:00  Rommy
 * get_channel(),get_channel_color(),col2colorn(),colorn2col() nicht private
 *
 * Revision 0.18  2008/04/25 16:30:00  TamTam
 * Channels fuer Daemonen und Trolle
 *
 * Revision 0.17  2008/02/14 16:30:00  TamTam
 * Ebenen history '-*<anz>' nochmal angepasst
 *
 * Revision 0.16  2008/01/20 16:30:00  TamTam
 * Ebenen history verkuerzt durch '-*<anz>'/'ebene <ebene> zeilen <zahl>'
 *
 * Revision 0.15  2007/06/30 19:10:00  Serii
 * Ebenen history und kanal info kann nun auch angesehn werden, ohne
 * dass man die ebene betreten muss
 *
 * Revision 0.14  2006/08/26 09:39:00  Serii
 * das creator-object der ebene hat nun das letzte "wort" ob wer eine
 * ebene betreten darf oder nicht
 *
 * Revision 0.13  2006/05/16 18:56:00  Serii
 * private ebenen eingebaut
 *
 * Revision 0.12  2006/05/16 00:45:00  Serii
 * fix fuer init-file eigentuemer von channels (geclonte npcs)
 *
 * Revision 0.1  2004/11/02 00:00:00  Serii
 * Komplett neu geschrieben - einige sachen aus dem original-channald
 * sind aus kompatibelitaets und funktionalitaetsgruenden wieder mit
 * eingebaut worden
 */

// Wenns geht hier bitte nix dran rum aendern - wenns verbesserungs vorschlaege
// o.ae. gibt - an mich (Serii) wenden. Ich baus dann ein ;)
// Hab das ding neu geschrieben, dasses nich mehr son riesen flickenteppich
// ist, an dem jeder mal dran rumgeprogt hat *g*

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <daemon.h>
#include <properties.h>
#include <wizlevels.h>
#include <udp.h>
#include <config.h>
#include <defines.h>
#include <terminal.h>
#include <ansi.h>

//#define BS_LEAVE_MY_LFS 	1
//#define BS_SINGLE_SPACE         2
//#define BS_BLOCK		4
//#define BS_NO_PARINDENT		8
// #define BS_INDENT_ONCE         16
// #define BS_PREPEND_INDENT      32

#define VERSION "0.18"

#define DRUNKDAEMON "/p/daemon/drunkd"
#define LAGDAEMON "/p/daemon/lag-o-daemon"

#define TU(u,msg) (previous_object()!=ME?(int)tell_object(u,msg):0)

#define SEND(ch,u,msg) funcall(#'SendToChannel,ch,u,SAY,msg)
#define FC_ESEND(ch,u,msg) funcall(#'SendToChannel,ch,u,EMOTE,msg,1)

#define CAN_COL(x) x->QueryProp(P_TTY)=="ansi"?0:1

#define C_OWNER 0

#define C_SHORT 1
#define C_MRECV 2
#define C_MSEND 3
#define C_PRECV 4
#define C_PSEND 5
#define C_FLAGS 6
#define C_INFO 7
#define C_CREATOR 8

#define C_HISTSIZE 9

#define C_USERS 10
#define C_HIST 11

#define SAVEFILE "/p/daemon/save/channeld"

#define S_CHANNELS 0
#define S_USERS 1
#define S_STATUS 2

#define S_CU_TRUSTED 0
#define S_CU_BANNED  1
#define S_CU_INVITED 2

#define S_M_SEND       0x0001
#define S_M_RECV       0x0002
#define S_TRUSTED      0x0004
#define S_M_ALTER      0x0008
#define S_M_DEST       0x0010
#define S_NEEDNOTENTER 0x0020
#define S_M_COLOR      0x0040
#define S_M_NOTCONTROL 0x0080
#define S_M_BAN	       0x0100

#define S_GODFLAG      0x0200
#define S_CHANNELDFLAG 0x0400

#define S_OWNER S_M_SEND|S_M_RECV|S_M_ALTER|S_M_BAN
#define S_CREATOR S_OWNER|S_M_DEST
#define S_M_ENTER S_M_RECV

#define S_CHANNELD S_CREATOR|S_M_COLOR|S_CHANNELDFLAG
#define S_GOD S_CREATOR|S_TRUSTED|S_M_BAN|S_GODFLAG
#define S_BANNER S_M_SEND|S_M_RECV|S_M_BAN

// not for sec_check:

#define S_M_CREATE S_NEEDNOTENTER|S_M_SEND
#define S_NPC S_M_SEND|S_M_RECV|S_NEEDNOTENTER

// C_FLAGs

#define CF_KEEP		0x1
  // ?spielerkanal?
#define CF_NTCHAN	0x2
  // invite only
#define CF_PRIVATE	0x4

// S_STATUS

#define SS_LASTSHUT  0
#define SS_LASTSTART 1

#define CONTROL_CHANNEL sprintf("#%sCD-MC#",MUDNAME[0..0])
#define STATUS_CHANNEL sprintf("#%sCD-Status#",MUDNAME[0..0])

#define INITIAL_HISTSIZE 100

// P_CHANNELS

#define PCHAN_COL 0
#define PCHAN_COL_IGNORE 1
#define PCHAN_HIST 2

#define COLNAM (["rot":"^R",\
                 "grün":"^G",\
		 "blau":"^B",\
		 "gelb":"^L",\
		 "braun":"^Y",\
		 "weiß":"^W",\
		 "lila":"^P",\
		 "normal":"",\
		 "normal":"^N",\
		 "grau":"^D",\
		 "cyan":"^C",\
         "hellcyan":"^W^C",\
		 "fett":"^A",\
		 "blink":"^F",\
		 "hellblau":"^W^B",\
		 "hellrot":"^W^R",\
		 "hellgrün":"^W^G",\
		 "helllila":"^W^P"])

static private object *Users=({});
static private int NewChannels=20;
private mapping Channels=([]);

//aus dem hier muessen noch beim speichern die channels rausgenommen werden
//die 'weich' sind :)
private mapping Status=([S_CHANNELS:([]), // TODO: remove entries for deleted channels
                         S_USERS:([
			   S_CU_TRUSTED:([]),
			   S_CU_BANNED:([]),
			   S_CU_INVITED:([]),
			 ]),
			 S_STATUS:([]),
	               ]);

string colorn2col(string col)
{
  if(stringp(COLNAM[col]))return COLNAM[col];
  return "";
}

string col2colorn(string col)
{
  mapping m;
  m=mkmapping(m_values(COLNAM),m_indices(COLNAM));
  if(stringp(m[col]))return m[col];
  return "";
}

private string get_ename(object user)
{
  return explode(object_name(user),"#")[0];
}

private int get_trust_status(mapping tmp,string un,int sret)
{int ts;
mapping temp;
temp=tmp;
  if(mappingp(tmp))
  {
    if(mappingp(tmp=tmp[S_CU_TRUSTED]))
    {
      if(intp(ts=tmp[un]))
      {
        if(ts)return(sret|ts);
      }
    }
    tmp=temp;
    if(mappingp(tmp=tmp[S_CU_BANNED]))
    {
      if(intp(ts=tmp[un]))
      {
        if(ts)return((sret|ts)^ts);
      }
    }
  }
  return -1;
}

varargs private int secure(object user, string channel)
{mapping tmp;
string un;
string *tstr;
int sret,wl,pl,ts,rtmp,cret;
object ob;
  if(!objectp(user))return 0;
  sret=0;
  un=get_ename(user);

  if(channel)
  {
    if(channel=="Mörder"&&query_once_interactive(user)&&user->QueryProp(P_GHOST))return S_M_SEND|S_M_RECV;

    tmp=Channels[channel];

    if(interactive(user)&&channel)
    {
      wl=query_wiz_level(user);
      pl=user->QueryProp(P_LEVEL);
      if(wl>=tmp[C_MRECV]&&wl>=LEARNER_LVL&&tmp[C_MRECV]>=0)sret|=S_M_RECV;
      if(wl<LEARNER_LVL&&wl>=tmp[C_MRECV]&&pl>=tmp[C_PRECV]&&
         tmp[C_MRECV]>=0&&tmp[C_PRECV]>=0)sret|=S_M_RECV;

      if(wl>=tmp[C_MSEND]&&wl>=LEARNER_LVL&&tmp[C_MSEND]>=0)sret|=S_M_SEND;
      if(wl<LEARNER_LVL&&wl>=tmp[C_MSEND]&&pl>=tmp[C_PSEND]&&
         tmp[C_MSEND]>=0&&tmp[C_PSEND]>=0)sret|=S_M_SEND;

      if(wl>=WIZARD_LVL&&sret&S_M_RECV)sret|=S_TRUSTED;
      if(wl>=ARCH_LVL&&sret&S_M_RECV)sret|=S_M_BAN;
      if(wl>=GOD_LVL&&sret&S_M_RECV)sret|=S_GOD;
    }
    if(user==this_object())return S_CHANNELD;

    if((tmp[C_CREATOR]==get_ename(user))||(find_living(tmp[C_CREATOR])==user))
    {
      sret|=S_CREATOR;
      if(!query_once_interactive(user))sret|=S_M_COLOR;
    }
    else if(tmp[C_OWNER]==user)
    {
      sret|=S_OWNER;
    }

    if(mappingp(tmp=Status[S_CHANNELS][channel]))
    {
      if((rtmp=get_trust_status(tmp,un,sret))>=0)return rtmp;
    }
  }
  else if(interactive(user))
  {
    if(query_wiz_level(user)>=WIZARD_LVL)sret|=S_TRUSTED;
    if(query_wiz_level(user)>=ARCH_LVL)sret|=S_M_BAN;
    if(query_wiz_level(user)>=GOD_LVL){sret|=S_GOD;/*return sret;*/}
  }

  if(user==this_object())return S_CHANNELD;
  if(user==find_object(__MASTER_OBJECT__))return S_CHANNELD|S_NEEDNOTENTER;

  if(mappingp(tmp=Status[S_USERS]))
  {
    if((rtmp=get_trust_status(tmp,un,sret))>=0)return rtmp;
  }

  if(!query_once_interactive(user)&&!sret)return 0;

  if(channel)
  {
    tmp=Channels[channel];
    if(tmp[C_FLAGS]&CF_PRIVATE)
    {
      if(!mappingp(Status[S_USERS][S_CU_INVITED]))
        Status[S_USERS][S_CU_INVITED]=([]);
      if(!pointerp(tstr=Status[S_USERS][S_CU_INVITED][channel]))
        tstr=({});
      if(!(tmp[C_CREATOR]==get_ename(user))&&!(find_living(tmp[C_CREATOR])==user)&&
	 !(member(tstr,user->query_real_name())>=0))
        return 0;
    }
    if((ob=find_living(tmp[C_CREATOR])||find_object(tmp[C_CREATOR]))&&ob!=user)
    {
      cret=call_other(ob,"channel_MayJoin",user,channel);
      if(cret)
      {
        return 0;
      }
    }
  }
  return sret;
}

varargs private int check_secure(object user,string channel,int priv)
{
  return ((secure(user,channel)&priv)==priv);
}

void Save()
{mapping chans;
string *inds;
int lv;
  SEND(STATUS_CHANNEL,ME,"Saving...");
chans=copy(Channels);
Channels=([]);
  inds=m_indices(chans);
  for(lv=sizeof(inds)-1;lv>=0;lv--)
  {
    if(chans[inds[lv]][C_FLAGS]&CF_KEEP)Channels[inds[lv]]=chans[inds[lv]];
  }
  save_object(SAVEFILE);
  Channels=copy(chans);
}
void Restore(int initf)
{
  restore_object(SAVEFILE);
}

private string strip_space(string str)
{return implode(efun::explode(str, " ")-({""})," ");}

private string strip_all_space(string str)
{return implode(efun::explode(str, " ")-({""}),"");}

private int is_invis_wiz(object ob)
{return (IS_WIZARD(ob)&&ob->QueryProp(P_INVIS));}

private int check_shortcut(string ch,string sc)
{
  return Channels[ch][C_SHORT]==sc;
}

private varargs string get_name(mixed pl,int fall,int trusted)
{string nam,*lname;
mixed tmp;
  if(objectp(pl))
  {
    if(object_name(pl)=="/secure/master")return "MASTER";
//    if(object_name(pl)=="/secure/udp/channel")return pl->name();
    nam=pl->name(fall)||pl->query_real_name();
    if(interactive(pl))
    {
      if(pl->QueryProp(P_INVIS))
      {
        if(trusted)
	  nam=sprintf("(%s)",capitalize(getuid(pl)));
	else
	  nam="Jemand"+(fall==WESSEN?"es":"");
      }
      else if(pl->QueryProp(P_FROG)&&pl->QueryProp(P_GHOST))
      { tmp=pl->QueryProp(P_FROG);
        pl->SetProp(P_FROG,0);
        nam=sprintf("Frosch %s Geist%s",pl->Name(WESSEN),(fall==WESSEN?"es":""));
	pl->SetProp(P_FROG,tmp);
      }
      else if(pl->QueryProp(P_FROG))
      { tmp=pl->QueryProp(P_FROG);
        pl->SetProp(P_FROG,0);
        nam=sprintf("Frosch ^G%s^N",pl->Name(fall));
	pl->SetProp(P_FROG,tmp);
      }
      else if(pl->QueryProp(P_GHOST))
      { tmp=pl->Name(WESSEN);
        nam=sprintf("Geist ^S^W%s%s^N",tmp,
	                        (fall==WESSEN?
				  (tmp[sizeof(tmp)-1..]=="s"?"'":"s"):""));
      }
    }
    if(!nam||!sizeof(nam))
    {
      lname=explode(object_name(pl),"/");
      nam=sprintf("<%s>",capitalize(lname[sizeof(lname)-1]));
    }
    return nam;
  }
  else if(stringp(pl)) return pl;
  else if(!pl)return "<NiemanD>";
  return "<UnbekannT>";
}

private int registered(object user)
{
  return member(Users,user)>=0;
}

private mixed get_channels(string chan)
{string *inds;
mapping chs;
  inds=m_indices(Channels);
  if(!chan||!sizeof(chan))return inds;
  chan=implode(regexplode(chan,"[.?*\^|/\\\\]")-({".","?","*","^","|","/","\\"}),"");
  if(!sizeof(chan))return ({});
  if(sizeof(chan)==1)
  {
    chs=filter_indices(Channels,#'check_shortcut,chan);
    if(sizeof(chs))return m_indices(chs);
  }
  return regexp(inds,"^"+chan+".*");
}

string get_channel(string chan)
{
  mapping cinfo;
  string *chans;
  if(!mappingp(cinfo=Channels[chan]))
  {
    chans=get_channels(chan);
    if(!sizeof(chans))
    {
      TU(previous_object(),sprintf("Die Ebene %s existiert nicht!\n",chan));
      return 0;
    }
    if(sizeof(chans)>1)
    {
      TU(previous_object(),break_string(sprintf("Eingabe nicht eindeutig! (%s)",implode(chans,", "))));
      return 0;
    }
    return chans[0];
  }
  return chan;
}

int channelmember(string channel,object pl)
{
  if(channel=get_channel(channel))
    return member(Channels[channel][C_USERS],pl)>=0;
  return 0;
}

string get_channel_color(string channel,object user)
{
  mapping cm;
  if(!query_once_interactive(user))return "";
  cm=user->QueryProp(P_CHANNELS)[channel];
  if(mappingp(cm))return cm[PCHAN_COL];
  return "";
}

private object get_player(object *us)
{
  us=filter(us,#'query_once_interactive);
  if(sizeof(us))return us[0];
  return ME;
}

int debug=0;

private void add_hist(string channel,object user,string message,int cmd)
{int *inds,lv,s,mh;
mapping tmp,tmp2;
tmp2=([]);
if(debug)return;
  mh=Channels[channel][C_HISTSIZE];
  s=sizeof(inds=sort_array(m_indices(Channels[channel][C_HIST]),#'>));
  for(lv=0;(lv<s)&&(lv<(mh-1));lv++)
  {
    tmp=copy(Channels[channel][C_HIST][lv]);
    Channels[channel][C_HIST][lv]=tmp2;
    tmp2=tmp;
  }
  if(lv<(mh))Channels[channel][C_HIST][lv]=tmp2;
  Channels[channel][C_HIST][0]=([0:({get_name(user,cmd==GEMOTE?WESSEN:WER),
                                          get_name(user,cmd==GEMOTE?WESSEN:WER,1)
				        }),1:message,2:cmd,3:time()]);
  if(mh<sizeof(inds))
  {
    inds-=inds[0..mh-1];
    s=sizeof(inds);
    for(lv=0;lv<s;lv++)
      Channels[channel][C_HIST]=m_delete(Channels[channel][C_HIST],inds[lv]);
  }
}

varargs private int check_banned_trusted(int bantrust,object user,string chan,int prev)
{mapping ch;
  if(!chan)
  {
//Status[S_USERS]-check!
    return 0;
  }
  if(!mappingp(ch=Status[S_CHANNELS][chan]))return 0;
  if(!mappingp(ch=ch[bantrust]))return 0;
  return ch[get_ename(user)]&prev;
}

varargs private void new_banned_trusted(int bantrust,string spec,int flags,int initf)
{string *opts,chan;
int lv;
  opts=efun::explode(strip_all_space(spec),":");
  if(sizeof(opts)>3)
  {
    opts[0]=sprintf("%s:%s",opts[0],opts[1]);
    opts[1]=opts[2];
    opts[2]=opts[3];
  }
  if(!flags)
  {
    for(lv=0;lv<sizeof(opts[1]);lv++)
    {
      if(chan=get_channel(opts[1][lv..lv]))
      {
        if(!mappingp(Status[S_CHANNELS][chan]))
          Status[S_CHANNELS][chan]=([bantrust:([])]);
	if(!member(Status[S_CHANNELS][chan][bantrust],opts[0])||
	  (member(Status[S_CHANNELS][chan][bantrust],opts[0])&&!initf))
            Status[S_CHANNELS][chan][bantrust][opts[0]]^=flags?flags:S_NPC;
      }
    }
  }
  else
  {
    if(chan=get_channel(opts[1]))
    {
      if(!mappingp(Status[S_CHANNELS][chan]))
        Status[S_CHANNELS][chan]=([bantrust:([])]);
      Status[S_CHANNELS][chan][bantrust][opts[0]]^=flags?flags:S_NPC;
    }
  }
}

varargs private int new_channel(string spec,object creator,int initf)
{string *opts,info,rcrea;
object ob;
  opts=efun::explode(spec,":");
  if(sizeof(opts)<8)
    return 0;
  info=opts[C_INFO];
  if(sizeof(opts[C_CREATOR])>=3)
    if(read_file(opts[C_CREATOR]+
                (opts[C_CREATOR][sizeof(opts[C_CREATOR])-2..]!=".c"?".c":"")
	      ))call_other(opts[C_CREATOR],"???");
  if((ob=find_living(opts[C_CREATOR])||find_object(opts[C_CREATOR])))
  {
    creator=ob;
    if(initf)
      if(member(Users,creator)<0)
        Users+=({creator});
  }
  else if(sizeof(opts[C_CREATOR]))
  {
    creator=ME;
    rcrea=opts[C_CREATOR];
  }
  opts=efun::explode(strip_all_space(implode(opts[0..sizeof(opts)-3],":")),":");
  if(mappingp(Channels[opts[0]]))
  {
    if(initf)
    {
      Channels[opts[0]][C_OWNER]=creator;
      Channels[opts[0]][C_USERS]=(creator!=ME?({ME,creator}):({ME}));
      Channels[opts[0]][C_CREATOR]=(!rcrea?get_ename(creator):rcrea);
    }
    return 0;
  }
  Channels[opts[0]]=([C_MRECV:(int)opts[C_MRECV],
                      C_MSEND:(int)opts[C_MSEND],
		      C_PRECV:(int)opts[C_PRECV],
		      C_PSEND:(int)opts[C_PSEND],
		      C_SHORT:opts[C_SHORT],
		      C_FLAGS:(sizeof(opts[C_FLAGS]))?(int)opts[C_FLAGS]:CF_KEEP,
		      C_INFO:info,
		      C_OWNER:creator,
		      C_USERS:(creator!=ME&&initf)?({ME,creator}):({ME}),
		      C_HIST:([]),
		      C_HISTSIZE:INITIAL_HISTSIZE,
		      C_CREATOR:get_ename(creator)
		      ]);
  return 1;
}

public int CreateChannel(string spec)
{string cname;
cname=explode(spec,":")[0];
  if(!mappingp(Channels[cname]))
  {
    SEND(CONTROL_CHANNEL,previous_object(),sprintf("%s lässt die Ebene \"%s\" entstehen.",get_name(previous_object(),0,1),cname));
    new_channel(spec,previous_object(),0);
    RegisterChannel(cname,previous_object());
    return 1;
  }
  return 0;
}

private void check_channel_consistency(string channel)
{object ob;
int tmp;
mapping chan,cs;
chan=Channels[channel];
  if(!objectp(chan[C_OWNER]))
  {
    if((ob=find_living(chan[C_CREATOR])||find_object(chan[C_CREATOR])))
    {
      tmp=0;
      if(query_once_interactive(ob))
      {
        if(mappingp(cs=ob->QueryProp(P_CHANNELS))&&
	   mappingp(cs[sprintf("\n%s",channel)]))
	{
	  tmp=1;
	  chan[C_OWNER]=ME;
	}
      }
      if(!tmp)
      {
        if(member(Users,ob)<0)
          Users+=({ob});
        chan[C_OWNER]=ob;
	if(member(chan[C_USERS],ob)<0)
	  chan[C_USERS]+=({ob});
      }
    }
    else
      chan[C_OWNER]=ME;
  }
  if(member(chan[C_USERS],ME)<0)
  {
    chan[C_USERS]+=({ME});
  }
  if(member(Users,ME)<0)
  {
    Users+=({ME});
  }
}

void create()
{int lv,s;
string *initfile;
  if(clonep(ME)) destruct(ME);
  seteuid(getuid(ME));
  Restore(1);
  initfile=explode(read_file(object_name(ME)+".init"),"\n");
  s=sizeof(initfile);
  Channels[STATUS_CHANNEL]=([0:ME,1:"1",2:21,3:111,4:-1,5:-1,6:0,7:""MUDNAME+"-Channel-Daemon Status (Nachfolger von \"Channel\")",10:({ME}),11:([]),9:INITIAL_HISTSIZE,8:get_ename(ME)]);
  Channels[CONTROL_CHANNEL]=([0:ME,1:"#",2:0,3:0,4:1,5:1,6:0,7:"Ebenen Kontrolle ('hilfe' für Ebenen-Hilfe)",10:({ME}),11:([]),9:INITIAL_HISTSIZE,8:get_ename(ME)]);

  Channels["Channel"]=([0:ME,1:"C",2:20,3:-1,4:-1,5:-1,6:0,7:"Channel Informationen (Dummy-Channel - use 1 !)",10:({ME}),11:([]),9:INITIAL_HISTSIZE,8:get_ename(ME)]);

  SEND(STATUS_CHANNEL,ME,sprintf("%s-Channel-Daemon V5 starting up (^W%s^N)",MUDNAME,get_name(previous_object(),0,1)));
  for(lv=0;lv<s;lv++)
  {
    if(initfile[lv][0..0]=="#"||initfile[lv][0..0]=="\n")continue;
    new_channel(initfile[lv],ME,1);
  }
  initfile=explode(read_file(object_name(ME)+".trusted"),"\n");
  s=sizeof(initfile);
  for(lv=0;lv<s;lv++)
  {
    if(initfile[lv][0..0]=="#")continue;
    new_banned_trusted(S_CU_TRUSTED,initfile[lv],0,1);
  }
  map(users(),#'RegisterUser);
//  SEND(STATUS_CHANNEL,ME,sprintf("%s-Channel-Daemon V5 starting up (^W%s^N)",MUDNAME,get_name(previous_object(),0,1)));
  Status[S_STATUS][SS_LASTSTART]=({time(),get_name(previous_object(),0,1)});

  map(m_indices(Channels),#'check_channel_consistency);
}

private void del_channel(string channel)
{object *us;
int s,lv;
  s=sizeof(us=Channels[channel][C_USERS]);
  for(lv=0;lv<s;lv++)
  {
    RemoveChannel(channel,us[lv],0,2,1);
  }
  if(Channels[channel][C_FLAGS]&CF_NTCHAN)NewChannels++;
  Channels=m_delete(Channels,channel);
}

string Name()
{
  return ME->query_real_name();
}

string name()
{
  return ME->Name();
}

string query_real_name()
{
//  return parse_color_string("^W^S{TCD}^N",0,previous_object()->QueryProp(P_TTY)=="ansi"?0:1);
  return sprintf("^W^S{%sCD}^N",MUDNAME[0..0]);
}

int remove()
{object ob;
  SEND(STATUS_CHANNEL,ME,sprintf("%s-Channel-Daemon V5 going down (^W%s^N)",MUDNAME,get_name(previous_object(),0,1)));
  Status[S_STATUS][SS_LASTSHUT]=({time(),get_name(previous_object(),0,1)});
  Save();
  Users=sort_array(Users,lambda(({'u,'p}),({#'>,({#'query_wiz_level,'u}),({#'query_wiz_level,'p})})));
  Users=sort_array(Users,lambda(({'u,'p}),({#'<,({#'query_once_interactive,'u}),({#'query_once_interactive,'p})})));
  map(Users,#'RemoveUser,1);
  ob=ME;
  destruct(ob);
  return 1;
}

void reset(){Save();}

int query_prevent_shadow() {return 1;}

private mapping convert_channels(string *chans,object user)
{int lv;
mapping pcs;
pcs=([]);
  for(lv=sizeof(chans)-1;lv>=0;lv--)
  {
    pcs[chans[lv]]=([PCHAN_COL:""]);
  }
  user->SetProp(P_CHANNELS,pcs);
SEND(STATUS_CHANNEL,ME,sprintf("^RConverting^N Channels for: ^W%s^N",get_name(user,0,1)));
  return pcs;
}

int RegisterUser(object user)
{
  mixed mcs;
  mapping pcs;
  string *acs;
if(registered(user))RemoveUser(user,1);
if(!pointerp(mcs=user->QueryProp(P_CHANNELS))&&!mappingp(mcs))return 1;
if(!mappingp(mcs))mcs=pcs=convert_channels(mcs,user);
else pcs=({mapping})mcs;
if(!sizeof(pcs))return 1;
  mcs=filter(m_indices(mcs),lambda( ({'c}), ({#'?, ({#'==,
                                  ({#'strstr,'c,"\n",0}),0}), 0, 1}) ));
  acs=filter(m_indices(pcs),lambda( ({'c}), ({#'?, ({#'==,
                                  ({#'strstr,'c,"\n",0}),0}), 1, 0}) ));
  if(sizeof(mcs))
  {
    mcs=map(mcs,#'RegisterChannel,user);
  }
  mcs-=({0});
  if(sizeof(mcs))SEND(STATUS_CHANNEL,ME,sprintf("Registering ^W%s^N on Channel(s): %s",get_name(user,0,1),implode(mcs,", ")));
  mcs+=acs;
  user->SetProp(P_CHANNELS,filter_indices(pcs,lambda( ({'c,'cs}),
                                              ({#'?,({#'<,
					      ({#'member,'cs,'c}),0
					      }),0,1 }) ),mcs));
  Users+=({user});
  return 1;
}

varargs int RemoveUser(object user,int channeld/*,int urem*/)
{mixed chans,pcs;
string *inds;
  inds=filter(m_indices(Channels),#'channelmember,user);
if(!sizeof(inds))
{
// //NEW
  Users-=({0,user});
//---
  return 1;
}
  chans=map(inds,#'RemoveChannel,user,0,channeld/*,urem*/);
if(query_once_interactive(user))
  pcs=user->QueryProp(P_CHANNELS);
  SEND(STATUS_CHANNEL,ME,sprintf("Removing ^W%s^N from Channel(s): %s",get_name(user,0,1),implode(chans,", ")));
if(query_once_interactive(user))
  user->SetProp(P_CHANNELS,filter_indices(pcs,lambda( ({'c,'cs}),
                                              ({#'?,({#'<,
					      ({#'member,'cs,'c}),0
					      }),0,1 }) ),({string *})chans));
  Users-=({0,user});
  return 1;
}

void ca_remzero(string channel)
{
  if(mappingp(Channels[channel]))Channels[channel][C_USERS]-=({0});
  Users-=({0});
}

varargs string RegisterChannel(string channel, object user, int aflag)
{mixed tmp;
//  if(interactive(user)&&clonep(user)){
  if(member(Users,user)<0)call_out("ca_remzero",1,channel);
//  }
  if(channel=="")
  {
    tmp=map(m_indices(Channels),#'RegisterChannel,user,1);
    tmp-=({0});
    if(sizeof(tmp))SEND(STATUS_CHANNEL,ME,sprintf("Registering ^W%s^N on Channel(s): %s",get_name(user,0,1),implode(tmp,", ")));
    return 0;
  }
  if(channel[0..0]=="\n"||channel=get_channel(channel))
  {
    if(channel[0..0]!="\n"&&!(check_secure(user,channel,S_M_ENTER)))
    {
      tell_object(user,sprintf("Du darfst die Ebene %s nicht betreten!\n",channel));
      return 0;
    }
    if(!channelmember(channel,user)){Channels[channel][C_USERS]+=({user});}
    else
    {
      tell_object(user,sprintf("Du hast die Ebene %s doch schon betreten!\n",channel));
      if(aflag)return 0;
      return channel;
    }
    if(((get_ename(user)==Channels[channel][C_CREATOR])||(find_living(Channels[channel][C_CREATOR])==user))&&Channels[channel][C_OWNER]!=user)
    {
//      FC_ESEND(channel,user,sprintf("uebernimmt die Ebene von %s",
           /*get_name(user,0,1),*/
//	   get_name(Channels[channel][C_OWNER],0,1)));
      SEND(CONTROL_CHANNEL,this_object(),sprintf("%s übernimmt die Ebene \"%s\" von %s.",get_name(user,0,1),channel,get_name(Channels[channel][C_OWNER],0,1)));
      Channels[channel][C_OWNER]=user;
    }
if(query_once_interactive(user))
{
    tmp=user->QueryProp(P_CHANNELS);
    if(!pointerp(tmp)&&!mappingp(tmp))tmp=([]);
    if(!mappingp(tmp))tmp=convert_channels(tmp,user);
    if(member(tmp,sprintf("\n%s",channel))&&!member(tmp,channel))
    {
      tmp[channel]=tmp[sprintf("\n%s",channel)];
      tmp=m_delete(tmp,sprintf("\n%s",channel));
    }
    else if(channel[0..0]!="\n"&&!member(tmp,channel))tmp[channel]=([PCHAN_COL:""]);
    user->SetProp(P_CHANNELS,tmp);
}
    if(user==previous_object()||aflag)
    {
      tell_object(user,sprintf("Du betrittst die Ebene %s.\n",channel));
      if(!aflag)SEND(STATUS_CHANNEL,ME,sprintf("Registering ^W%s^N on Channel: %s",get_name(user,0,1),channel));
    }
    return channel;
  }
  return 0;
}

varargs string RemoveChannel(string channel, object user, int aflag,int channeld,int urem)
{mixed tmp;
  if(channel=="")
  {
    tmp=map(m_indices(Channels),#'RemoveChannel,user,1,channeld,1);
    tmp-=({0});
    if(sizeof(tmp))SEND(STATUS_CHANNEL,ME,sprintf("Removing ^W%s^N from Channel(s): %s",get_name(user,0,1),implode(tmp,", ")));
    return 0;
  }
  if(channel=get_channel(channel))
  {
    if(!channelmember(channel,user))
    {
      tell_object(user,sprintf("Du hast die Ebene %s doch gar nicht betreten!\n",channel));
      if(aflag)return 0;
      return channel;
    }
//
    if(user==Channels[channel][C_OWNER])
    {
      Channels[channel][C_OWNER]=get_player(Channels[channel][C_USERS]-({user}));
//      FC_ESEND(channel,user,sprintf("uebergibt die Ebene an %s",
           /*get_name(user,0,1),*/
//get_name(Channels[channel][C_OWNER],0,1)));
      SEND(CONTROL_CHANNEL,this_object(),sprintf("%s übergibt die Ebene \"%s\" an %s.",get_name(user,0,1),channel,get_name(Channels[channel][C_OWNER],0,1)));
    }
//-->
    Channels[channel][C_USERS]-=({0,user});
    if(query_once_interactive(user)&&channeld!=1&&(previous_object()==user||urem))
    {
      tmp=user->QueryProp(P_CHANNELS);
      if(tmp[channel])
      {
        if(channeld!=2)tmp[sprintf("\n%s",channel)]=tmp[channel];
	tmp=m_delete(tmp,channel);
	user->SetProp(P_CHANNELS,tmp);
      }
    }
//-->
//
    if(user==previous_object()||aflag)
    {
      tell_object(user,sprintf("Du verlässt die Ebene %s.\n",channel));
      if(!aflag)SEND(STATUS_CHANNEL,ME,sprintf("Removing ^W%s^N from Channel: %s",get_name(user,0,1),channel));
    }
    return channel;
  }
  return 0;
}

private void print_chans(string chan,object user,mixed msg,int pflag)
{mapping ch;
string col,name;
  if(!(check_secure(user,chan,S_M_ENTER)))return;
  if(pflag&&!channelmember(chan,user))return;
  check_channel_consistency(chan);
  if(mappingp(ch=user->QueryProp(P_CHANNELS)[chan]))
    col=ch[PCHAN_COL];
  else col="";
  ch=Channels[chan];
  msg+=({sprintf("[%s%1s%s] %-14.14s - %|35.35s (%-2d) [%s]",
		 sizeof(ch[C_SHORT])?col:"^I"+col,ch[C_SHORT],"^N",chan,ch[C_INFO],sizeof(ch[C_USERS]),
		 (colsizeof(parse_color_string(name=get_name(ch[C_OWNER],0,check_secure(user,0,S_TRUSTED)),0,0))>15?name[0..14]:name))});
}

varargs int ListChannels(object user)
{mixed msg;
msg=({"    Ebene          |            Beschreibung           | Anz.|  Bes.      ",
      "-------------------------------------------------------------------------------"});
  if(!objectp(user))
  {
    if(!interactive(user=this_player()))return 0;
    map(sort_array(m_indices(Channels),#'>),#'print_chans,user,&msg,0);
  }
  else
  {
    map(sort_array(m_indices(Channels),#'>),#'print_chans,user,&msg,1);
  }
  msg+=({"-------------------------------------------------------------------------------"});
  user->More(parse_color_string(implode(msg,"\n"),0,CAN_COL(user))+"\n");
  return 1;
}

private string uList(string channel, object user)
{
  object *u;
  int lv;
  string rstr;
  rstr="";
  u=Channels[channel][C_USERS];
  if(!IS_LEARNING(previous_object()))
    u=filter(u,lambda(({'u}),({#'!,({#'call_other,'u,"QueryProp",P_INVIS})})));
  for(lv=sizeof(u)-1;lv>=0;lv--)
    rstr+=sprintf("%s%s ",get_name(u[lv],0,check_secure(user,channel,S_TRUSTED)),lv>0?",":"");
  return rstr;
}

private varargs string format_string(string msg,string chan,string emote)
{
  return chan+
  implode(
    explode(
      break_unprocessed_string(msg,
        78-colsizeof(parse_color_string(sprintf("\n%s",chan)))),
      "\n"),sprintf("\n^N%s",stringp(emote)?emote:chan))+"\n";
}

private varargs void tell_user(object to,string channel,
                       string msg,mixed from,int cmd,int flags)
{
  string pre, race, sname,ccol,ncol;
  int fall,c,color,ci;
  mixed res;
  if(!to)return;
  color=CAN_COL(to);
  ci=0;
  
  if(!stringp(channel))
    return;
    
  if(query_once_interactive(to))
  {
    res=to->QueryProp(P_CHANNELS)[channel];
    if(mappingp(res)&&res[PCHAN_COL_IGNORE])
    {
      msg=parse_color_string(msg,0,1);
      ci=1;
    }
  }
  ccol=get_channel_color(channel,to);
  if(sizeof(ccol))ncol="^N";
  else ncol="";

  if(!IS_LEARNING(to))
  {
    race=to->QueryProp(P_RACE);
    if(channel=="Elfen"&&race!="Elf")
      msg="elfensingsangsingsanglalala";
    if(channel=="Hobbits"&&race!="Hobbit")
      msg="hobbitgeflüstertuschel";
    if(channel=="Menschen"&&race!="Mensch")
      msg="laberrhabarberblafasel";
    if(channel=="Zwerge"&&race!="Zwerg")
      msg="zwergengemurmelmurmelmurmel";
    if(channel=="Brownies"&&race!="Brownie")
      msg="brownieblablablublaeblablu";
    if(channel=="Orks"&&race!="Ork")
      msg="Orkuggugguggorkgutuggugg";
    if(channel=="Goblins"&&race!="Goblin")
      msg="Gobbogutgobbotollgobbogross";
    if(channel=="Trolle"&&race!="Troll")
      msg="heymaanallesklaa";
    if(channel=="Dämonen"&&race!="Dämon")
      msg="chztghchgchzt";
  }

  switch(cmd)
  {
  case EMPTY : if(msg[sizeof(msg)-2..]!="\n")msg+="\n";
               break;
  case GEMOTE: fall=WESSEN;
  case EMOTE : msg=format_string(msg+(ci?"^N":"")+"]",
                     sprintf("[%s%s%s:%s %s",ccol,channel,ncol,get_name(from,fall,check_secure(to,channel,S_TRUSTED)),ci?ccol:""),
		     sprintf(
		       sprintf(" %%' ':%ds %%' ':%ds %s",
		         colsizeof(parse_color_string(channel)),
			 colsizeof(parse_color_string(
			   get_name(from,fall,
			     check_secure(to,channel,S_TRUSTED)))
			    ),
			 ci?ccol:""
			    )
			    ,"",""));
               break;
  case SAY   : msg=format_string(msg+(ci?"^N":""),
                     sprintf("[%s%s%s:%s] %s",ccol,channel,ncol,get_name(from,fall,check_secure(to,channel,S_TRUSTED)),ci?ccol:""));
               break;
  }

  if(channel=="Unsinn")
  for(c=sizeof(msg)-1;c >= 0; c--)
  {
    if(msg[c]=='r') msg[c]='\'';
    if(msg[c]=='R') msg[c]='\'';
    if(msg[c]=='s') msg[c]='f';
    if(msg[c]=='S') msg[c]='F';
  }
  to->Message((color?"":ANSI_NORMAL)+parse_color_string(msg,0,color)+(color?"":ANSI_NORMAL),flags||MSGFLAG_CHANNEL);
}

private void send_intermud(string mud,mixed data,mapping msg,string *excl)
{
  if(mud==lower_case(MUDNAME))return;
  if(member(data[HOST_COMMANDS],"channel")!=-1||
     member(data[HOST_COMMANDS],"*")!=-1)
  {
    if(excl&&member(excl,mud)==-1)return;
    INETD->send_udp(data[HOST_NAME],msg);
  }
}

void eval_control_string(string msg,object user)
{string *args,*inds;
mixed tmp;
int lv;
mapping map;
string ret;
if(!objectp(user))return;
args=explode(msg," ");
lv=0;
  switch(args[0])
  {
    case "lag":
    case "l":
      tmp=LAGDAEMON->read_lag_data();
      ret=sprintf("Lag: ^W%.1f%%^N/60 ^W%.1f%%^N/15 ^W%.1f%%^N/1",tmp[0],tmp[1],tmp[2]);
    break;
    case "info":
    case "i":
      tmp=time()-Status[S_STATUS][SS_LASTSTART][0];
      ret=sprintf("Der %s-Channel-Daemon V5 ("+VERSION+") läuft seit %d Tagen %d Stunden"
                  " und %d Minuten. Gestartet wurde er von %s. Freie Plätze"
		  " für neue Ebenen: %d.",MUDNAME,
		  tmp/86400,(tmp%86400)/3600,((tmp%86400)%3600)/60,
		  Status[S_STATUS][SS_LASTSTART][1],NewChannels);
    break;
    case "stat":
      eval_control_string("uptime",user);
      eval_control_string("info",user);
    case "statistik":
    case "s":
      tmp=({0,0,0});
      tmp[0]=sizeof(Channels);
      tmp[1]=sizeof(Users);
      tmp[2]=sizeof(filter(Users,#'query_once_interactive));
      ret=sprintf("Zur Zeit gibt es %d Ebenen und %d Zuhörer, davon sind %d"
                  " Spieler.",
                  tmp[0],tmp[1],tmp[2]);
    break;
    case "uptime":
    case "up":
    case "u":
      ret=sprintf("Das %s läuft schon %s.",MUDNAME,uptime());
    break;
    case "unbann":
    case "unban":
    case "ub":
      lv=1;
    case "bann":
    case "ban":
    case "b":
      if(sizeof(args)>=3)
      {
        tmp=find_player(args[1])||find_netdead(args[1]);
        if(objectp(tmp))
	{
	  switch(args[2])
	  {
	    case "ebene":
	      if(sizeof(args)>=5)
	      {
	       if(args[3]=get_channel(args[3]))
		if(check_secure(user,args[3],S_BANNER)&&
		   query_wiz_level(tmp)<=query_wiz_level(user)&&
		   ((!check_secure(tmp,args[3],S_CREATOR)&&
		   !check_secure(tmp,args[3],S_CHANNELD)&&
		   !check_secure(tmp,args[3],S_GOD))))
		{
		  switch(args[4])
		  {
		    case "recv":
		    case "lesen":
		    case "empfangen":
		      if(lv)
		      {
		        if(!check_banned_trusted(S_CU_BANNED,tmp,args[3],S_M_RECV))
			  return;
		        ret=sprintf("%s lässt %s wieder auf die Ebene \"%s\".",
		            user->Name(WER),tmp->Name(WEN),args[3]);
		      }
		      else
		      {
		        if(check_banned_trusted(S_CU_BANNED,tmp,args[3],S_M_RECV))
			  return;
		        ret=sprintf("%s verbannt %s von Ebene \"%s\".",
		            user->Name(WER),tmp->Name(WEN),args[3]);
                        if(channelmember(args[3],tmp))
		          RemoveChannel(args[3],({object})tmp,0,2);
		      }
                      new_banned_trusted(S_CU_BANNED,
		        sprintf("%s:%s:",get_ename(tmp),args[3]),S_M_RECV);
		    break;
		    case "send":
		    case "schreiben":
		    case "senden":
		      if(lv)
		      {
		        if(!check_banned_trusted(S_CU_BANNED,tmp,args[3],S_M_SEND))
			  return;
		        ret=sprintf("%s lässt %s wieder auf er Ebene \"%s\" senden.",
		            user->Name(WER),tmp->Name(WEN),args[3]);
		      }
		      else
		      {
		        if(check_banned_trusted(S_CU_BANNED,tmp,args[3],S_M_SEND))
			  return;
                        ret=sprintf("%s untersagt %s das Senden auf Ebene \"%s\".",
		          user->Name(WER),tmp->Name(WEN),args[3]);
		      }
                      new_banned_trusted(S_CU_BANNED,
		        sprintf("%s:%s:",get_ename(tmp),args[3]),S_M_SEND);
		    break;
		  }
		}
	      }
	    break;
	  }
	}
	else
	{
	  switch(args[2])
	  {
	    case"create":
//	      if(!mappingp(Status[S_USERS][S_CU_TRUSTED][args[1]]))Status[S_USERS][S_CU_TRUSTED][args[1]]=0;
	      Status[S_USERS][S_CU_BANNED][args[1]]=S_M_CREATE;
	      ret=sprintf("Object %s darf nun%s Ebenen erstellen.",args[1],Status[S_USERS][S_CU_TRUSTED][args[1]]&S_M_CREATE?"":" nicht mehr");
	    break;
	  }
	}
      }
    break;
    case "ebene":
    case "e":
      if(sizeof(args)>=3)
      switch(args[2])
      {
        case "info":
	case "i":
        if(args[1]=get_channel(args[1]))
        {
          if(check_secure(user,args[1],S_M_ENTER))
          {
            map=Channels[args[1]];
            ret=sprintf("Ebene \"%s\": Spieler: %d, History: %d, Besitzer: %s.",
	              args[1],sizeof(map[C_USERS]),map[C_HISTSIZE],
		      get_name(map[C_OWNER],0,0));
	    if(map[C_FLAGS]&CF_PRIVATE&&pointerp(Status[S_USERS][S_CU_INVITED][args[1]]))
	    {
	      ret+=" Privatebene - Erlaubte spieler: "+implode(Status[S_USERS][S_CU_INVITED][args[1]],", ")+".";
	    }

          }
        }
        break;
	case "modus":
	case "mode":
	case "m":
	if(args[1]=get_channel(args[1]))
	{
	  map=Channels[args[1]];
          if(check_secure(user,args[1],(S_M_ALTER|S_TRUSTED))&&
	     sizeof(args)>=4&&(args[3]=="hart"||args[3]=="weich"))
          {
            ret=sprintf("Ebene \"%s\": Modus von %s auf %s geändert.",
	                args[1],map[C_FLAGS]&CF_KEEP?"hart":"weich",
			args[3]);
	    if(args[3]=="weich"&&map[C_FLAGS]&CF_KEEP)
	      map[C_FLAGS]^=CF_KEEP;
	    else if(args[3]=="hart")
	      map[C_FLAGS]|=CF_KEEP;
          }
	  else if(check_secure(user,args[1],(S_CREATOR))&&
	    sizeof(args)>=4&&(member(({"privat","private"}),args[3])>=0||
	                      member(({"public","öffentlich"}),args[3])>=0))
	  {
	    ret=sprintf("Ebene \"%s\": Modus von %s auf %s geändert.",
	                args[1],map[C_FLAGS]&CF_PRIVATE?"privat":"öffentlich",
			args[3]);
	    if(member(({"public","öffentlich"}),args[3])>=0&&map[C_FLAGS]&CF_PRIVATE)
	    {
	      map[C_FLAGS]^=CF_PRIVATE;
	      Status[S_USERS][S_CU_INVITED]=({});
	    }
	    else if(member(({"privat","private"}),args[3])>=0)
	    {
	      if(!pointerp(Status[S_USERS][S_CU_INVITED]))
	        Status[S_USERS][S_CU_INVITED]=({});
	      map[C_FLAGS]|=CF_PRIVATE;
	      tmp=Channels[args[1]][C_USERS];
	      for(lv=0;lv<sizeof(tmp);lv++)
	        if(!check_secure(tmp[lv],args[1],S_M_ENTER))
		  RemoveChannel(args[1],tmp[lv],0,2);
	    }
	  }
	  else
	  {
	    ret=sprintf("Ebene \"%s\": Modus = %s|%s.",
	                args[1],map[C_FLAGS]&CF_KEEP?"hart":"weich",map[C_FLAGS]&CF_PRIVATE?"privat":"öffentlich");
	  }
	}
	break;
        case "invite":
	case "lade":
	if(args[1]=get_channel(args[1]))
	{
	  if(sizeof(args)>=5&&Channels[args[1]][C_FLAGS]&CF_PRIVATE)
	  {
	    tmp=find_player(args[3])||find_netdead(args[3]);
	    if(objectp(tmp)&&check_secure(user,args[1],S_CREATOR)&&user!=tmp)
	    {
	      if(args[4]=="ein")args[4]="1";
	      else args[4]="0";
	      map=Status[S_USERS][S_CU_INVITED];
	      if(!mappingp(map))
	        map=([]);
	      if(!pointerp(map[args[1]]))
	        map[args[1]]=({});

	      if(member(map[args[1]],tmp->query_real_name())<0&&args[4]=="1")
	      {
	        map[args[1]]+=({tmp->query_real_name()});
		ret=sprintf("%s läd %s auf die Ebene %s ein.",get_name(user,0,1),get_name(tmp,0,1),args[1]);
	      }
	      else if(member(map[args[1]],tmp->query_real_name())>=0&&args[4]=="0")
	      {
	        map[args[1]]-=({tmp->query_real_name()});
		ret=sprintf("%s läd %s aus der Ebene %s aus.",get_name(user,0,1),get_name(tmp,0,1),args[1]);
		RemoveChannel(args[1],({object})tmp,0,2);
	      }
	      else
	      {
	        ret=sprintf("Momentan sind folgende Spieler auf der Ebene %s erlaubt: ",args[1]);
		inds=map[args[1]];
		inds+=({user->query_real_name()});
		for(lv=0;lv<sizeof(inds);lv++)
		{
		  ret+=capitalize(inds[lv]);
		  if(lv+1<sizeof(inds))
		    ret+=", ";
		}
		ret+=".";
	      }
	    }
	    else
	    {
	      map=Status[S_USERS][S_CU_INVITED];
              ret=sprintf("Momentan sind folgende Spieler auf der Ebene %s erlaubt: ",args[1]);
              inds=map[args[1]];
	      inds+=({user->query_real_name()});
	      for(lv=0;lv<sizeof(inds);lv++)
	      {
	        ret+=capitalize(inds[lv]);
	        if(lv+1<sizeof(inds))
	          ret+=", ";
	      }
	      ret+=".";
	    }
	  }
	  else if(Channels[args[1]][C_FLAGS]&CF_PRIVATE)
	  {
	    map=Status[S_USERS][S_CU_INVITED];
            ret=sprintf("Momentan sind folgende Spieler auf der Ebene %s erlaubt: ",args[1]);
            inds=map[args[1]];
	    if(!mappingp(map))
	        map=([]);
	      if(!pointerp(map[args[1]]))
	        map[args[1]]=({});
	    inds+=({user->query_real_name()});
	    for(lv=0;lv<sizeof(inds);lv++)
	    {
	      ret+=capitalize(inds[lv]);
	      if(lv+1<sizeof(inds))
	        ret+=", ";
	    }
	    ret+=".";
	  }
	}
	break;
	case "history":
	case "hist":
	case "h":
        if(args[1]=get_channel(args[1]))
        {
	  map=Channels[args[1]];
          if(check_secure(user,args[1],S_M_ALTER)&&sizeof(args)>=4&&(int)args[3]>0)
          {
	    if((int)args[3]>50&&
	    !(check_secure(user,0,S_TRUSTED)))args[3]="50";
            ret=sprintf("Ebene \"%s\": History von %d auf %d Einträge geaendert.",
	                args[1],map[C_HISTSIZE],(int)args[3]);
	    map[C_HISTSIZE]=(int)args[3];
          }
	  else
	  {
	    ret=sprintf("Ebene \"%s\": History = %d Einträge.",
	                args[1],map[C_HISTSIZE]);
	  }
        }
	break;
	case "zeilen":
	case "z":
	if(args[1]=="!")
	{
	  if(sizeof(args)>=4&&mappingp(map=user->QueryProp(P_CHANNELS))&&(int)args[3]>0&&(int)args[3]<50)
	  {
	    inds=m_indices(map);
	    for(lv=sizeof(inds)-1;lv>=0;lv--)
	    {
	      map[inds[lv]][PCHAN_HIST]=(int)args[3];
	    }
            ret=sprintf("%s: Zeilen für ALLE Ebenen geändert auf %s.",
                get_name(user,0,1),args[3]);
	  }
	  else if(sizeof(args)>=4&&mappingp(map=user->QueryProp(P_CHANNELS)))
	  {
            inds=m_indices(map);
	    for(lv=sizeof(inds)-1;lv>=0;lv--)
	    {
	      map[inds[lv]]-=([PCHAN_HIST]);
	    }
	    ret=sprintf("%s: Zeilen für ALLE Ebenen auf Ebenendefault gesetzt.",
                get_name(user,0,1));
	  }
	}
	else if(args[1]=get_channel(args[1]))
        {
          if(sizeof(args)>=4&&mappingp(map=user->QueryProp(P_CHANNELS)[args[1]])&&(int)args[3]>0&&(int)args[3]<50)
	  {
	    map[PCHAN_HIST]=(int)args[3];
	    ret=sprintf("%s: Zeilen für Ebene \"%s\" geändert auf %s.",
	        get_name(user,0,1),args[1],args[3]);
	  }
          else if(sizeof(args)>=4&&mappingp(map=user->QueryProp(P_CHANNELS)[args[1]]))
	  {
	    map-=([PCHAN_HIST]);
	    ret=sprintf("%s: Zeilen für Ebene \"%s\" auf Ebenendefault gesetzt.",
	        get_name(user,0,1),args[1]);
	  }
        }
	break;
	case "farbe":
	case "f":
	if(args[1]=="!")
	{
	  if(sizeof(args)>=4&&mappingp(map=user->QueryProp(P_CHANNELS))&&sizeof(colorn2col(args[3])))
	  {
	    inds=m_indices(map);
	    for(lv=sizeof(inds)-1;lv>=0;lv--)
	    {
	      tmp=get_channel_color(inds[lv],user);
	      map[inds[lv]][PCHAN_COL]=colorn2col(args[3]);
	    }
            ret=sprintf("%s: Farbe für ALLE Ebenen geändert von %s%s%s zu %s%s%s.",
                get_name(user,0,1),tmp,col2colorn(tmp),"^N",colorn2col(args[3]),args[3],"^N");
	  }
	  else if(sizeof(args)>=4&&mappingp(map=user->QueryProp(P_CHANNELS))&&args[3]=="aus")
	  {
            inds=m_indices(map);
	    for(lv=sizeof(inds)-1;lv>=0;lv--)
	    {
	      map[inds[lv]][PCHAN_COL_IGNORE]=1;
	    }
	    ret=sprintf("%s: Spielerfarben für ALLE Ebenen aus.",
                get_name(user,0,1));
	  }
	  else if(sizeof(args)>=4&&mappingp(map=user->QueryProp(P_CHANNELS))&&args[3]=="ein")
	  {
            inds=m_indices(map);
	    for(lv=sizeof(inds)-1;lv>=0;lv--)
	    {
	      map[inds[lv]][PCHAN_COL_IGNORE]=0;
	    }
	    ret=sprintf("%s: Spielerfarben für ALLE Ebenen ein.",
                get_name(user,0,1));
	  }
	}
	else if(args[1]=get_channel(args[1]))
        {
          if(sizeof(args)>=4&&mappingp(map=user->QueryProp(P_CHANNELS)[args[1]])&&args[3]=="ein")
	  {
	    map[PCHAN_COL_IGNORE]=0;
	    ret=sprintf("%s: Spielerfarben für Ebene \"%s\" ein.",
                get_name(user,0,1),args[1]);
	  }
	  else if(sizeof(args)>=4&&mappingp(map=user->QueryProp(P_CHANNELS)[args[1]])&&args[3]=="aus")
	  {
	    map[PCHAN_COL_IGNORE]=1;
	    ret=sprintf("%s: Spielerfarben für Ebene \"%s\" aus.",
                get_name(user,0,1),args[1]);
	  }
          else if(sizeof(args)>=4&&mappingp(map=user->QueryProp(P_CHANNELS)[args[1]])&&stringp(colorn2col(args[3])))
	  {
	    tmp=get_channel_color(args[1],user);
	    map[PCHAN_COL]=colorn2col(args[3]);
	    ret=sprintf("%s: Farbe für Ebene \"%s\" geändert von %s%s%s zu %s%s%s.",
	        get_name(user,0,1),args[1],tmp,col2colorn(tmp),"^N",colorn2col(args[3]),args[3],"^N");
	  }
        }
	break;
	case "neu":
	args[1]=implode(explode(args[1],":"),"");
	if(!get_channel(args[1]))
	{
	  if(((check_secure(user,0,S_TRUSTED))||(!(check_secure(user,0,S_TRUSTED))&&NewChannels))&&user->query_real_name()[0..3]!="gast")
	  {
            if(sizeof(args)>=4)tmp=args[3][0..0];
	    else tmp="";
	    tmp=sprintf("%s:%s:0:0:0:0:0: Spieler-Kanal:",args[1],tmp);
	    new_channel(tmp,user);
	    ret=sprintf("%s lässt die Ebene \"%s\" entstehen.",get_name(user,0,1),args[1]);
	    RegisterChannel(args[1],user);
	    if(!(check_secure(user,0,S_TRUSTED)))
	    {
	      Channels[args[1]][C_FLAGS]|=CF_NTCHAN;
	      NewChannels--;
	    }
	  }
	}
	break;
	case "zerstoeren":
	case "aufloesen":
    case "zerstören":
	case "auflösen":
	case "dest":
	  if(args[1]=get_channel(args[1]))
	    if(check_secure(user,args[1],S_M_DEST))
	    {
	      del_channel(args[1]);
	      ret=sprintf("%s löst die Ebene \"%s\" auf.",get_name(user,0,1),args[1]);
	    }
	break;
	case "thema":
	    if(args[1]=get_channel(args[1]))
	    if(check_secure(user,args[1],S_M_ALTER))
	    {
	      if(sizeof(args)>=4)
	      {
	        Channels[args[1]][C_INFO]=implode(args[3..]," ");
	        ret=sprintf("Neues Thema für Ebene \"%s\": %s.",args[1],Channels[args[1]][C_INFO]);
	      }
	    }
	break;
      }
    break;
    case "hilfe":
    case "h":
      tmp=sprintf("Hilfe zu folgenden Themen verfügbar: "
	            "^Webene^N, ^Wbann^N, ^Wfarben^N, ^Wlag^N, ^Winfo^N, ^Wuptime^N, ^Whinweise^N");
      if(sizeof(args)>=2)
        switch(args[1])
	{
          case"lag":
	    ret="^R[^N^Wlag^N^R]^N: Wie doll lagts denn hier?";
	  break;
	  case"bann":
	    ret="^R[^N^Wbann <name> ebene <ebene> {lesen|senden}^N^R]^N: Verbietet"
	        " ^W<name>^N das Lesen bzw. das Senden auf Ebene ^W<ebene>^N."
		"          "
		"^R[^N^Wunbann <name> ebene <ebene> {lesen|senden}^N^R]^N: Laesst"
	        " ^W<name>^N wieder auf Ebene ^W<ebene>^N senden bzw. lesen.";
	  break;
          case"hinweise":
	    ret="Wer diese Ebene und deren Funktionen ^Rmissbraucht^N und/oder"
	        " im ^Rnegativen^N Sinne ausnutzt wird für ^Fimmer^N von"
		" dieser Ebene gebannt! - Verbesserungsvorschläge"
		" oder Sonstiges an die Erzmagiere.";
	  break;
	  case"ebene":
	    ret="^R[^N^Webene <ebene> info^N^R]^N: Generelle Infos über"
	        " Ebene ^W<ebene>^N. ^R[^N^Webene <ebene> farbe <farbe>^N^R]^N: Setzt Farbe"
		" ^W<farbe>^N für Ebene ^W<ebene>^N (\"!\" für alle Ebenen).                 "
		" ^R[^N^Webene <ebene> farbe ein/aus^N^R]^N: Zeigt von anderen gesendete Farben"
		" auf Ebene ^W<ebene>^N (nicht mehr) an. (\"!\" für alle Ebenen)"
		" - Ebenentext erscheint dann in der Kanalfarbe.      "
		"^R[^N^Webene <ebene> zeilen <zahl>^N^R]^N: Setzt Zahl ^W<zahl>^N Zeilen der"
		" History für Ebene ^W<ebene>^N (\"!\" für alle Ebenen).      "
		" ^R[^N^Webene <ebene> history <nummer>^N^R]^N: Ohne ^W<nummer>^N wird"
		" die Anzahl der Historyeinträge angezeigt, die für"
		" die Ebene ^W<ebene>^N gespeichert werden. Ansonsten wird"
		" die Anzahl auf ^W<nummer>^N gesetzt. Nur der Eigentümer"
		" der Ebene kann diese verändern!                                           "
		" ^R[^N^Webene <ebene> neu <kürzel>^N^R]^N: Erstellt eine neue"
		" Ebene namens ^W<ebene>^N. Wird ^W<kürzel>^N angegeben ist"
		" diese mit diesem Kürzel erreichbar (ala \"-a\" statt \"-Allgemein\")."
		" Allerdings ist es vorzuziehen ^RKEIN^N Kürzel anzugeben, da die"
		" meisten eh schon für die Standardebenen verwendet werden und"
		" Überlagerungen nerven nur!                       "
		" ^R[^N^Webene <ebene> auflösen^N^R]^N: Löst die Ebene ^W<ebene>^N auf.    "
		" ^R[^N^Webene <ebene> modus <öffentlich|privat>^N^R]^N: Standardeinstellung"
		" ist ^Wöffentlich^N (jeder darf die Ebene ^W<ebene>^N betreten). Ist die"
		" Einstellung auf ^Wprivat^N, dürfen nur eingeladene Spieler die ^W<ebene>^N"
		" betreten.         "
		" ^R[^N^Webene <ebene> lade <name> <ein|aus>^N^R]^N: Läd"
		" ^W<name>^N auf die ^Wprivate^N Ebene ^W<ebene>^N ein oder aus.";
	  break;
	  case"farben":
	  case"farbe":
	    ret="Mögliche Farben: ^Rrot^N,"
		" ^W^Rhellrot^N, ^Ggrün^N, ^W^Ghellgrün^N, ^Ybraun^N, ^W^Ygelb^N, ^Bblau^N, ^W^Bhellblau^N, ^Plila^N, ^W^Phelllila^N, "
		" ^Ccyan^N, ^W^Chellcyan^N, ^Dgrau^N,"
		" normal und ^Afett^N. Natürlich sollte"
		" man seinen Terminal-typ auf 'ansi' (^Whilfe stty^N) um die Farben"
		" alle zu sehen.";
	  break;
	  case"info":
	    ret=sprintf("^R[^N^Winfo^N^R]^N: Infos über den %s-Channel-Daemon V5.",MUDNAME);
	  break;
	  case"uptime":
	    ret=sprintf("^R[^N^Wuptime^N^R]^N: Uptime des %s.",MUDNAME);
	  break;
	  default:
	    ret=tmp;
	  break;
	}
      else
        ret=tmp;
    break;
  }
  if(ret)
  {
    SEND(CONTROL_CHANNEL,ME,ret);
  }
}
void eval_status_string(string msg,object user)
{string *args;
mixed tmp;
mapping map;
string ret;
int ft;
if(!objectp(user))return;
args=explode(msg," ");
  switch(args[0])
  {
    case "lag":
    case "l":
      tmp=LAGDAEMON->read_lag_data();
      ret=sprintf("Lag: ^W%.1f%%^N/60 ^W%.1f%%^N/15 ^W%.1f%%^N/1",tmp[0],tmp[1],tmp[2]);
      SEND(STATUS_CHANNEL,ME,query_load_average());
    break;
    case "trust":
    case "flag":
      if(sizeof(args)>=3)
      {
        tmp=find_player(args[1])||find_netdead(args[1]);
        if(objectp(tmp))
	{
	  switch(args[2])
	  {
	    case "farbe":
	      Status[S_USERS][S_CU_TRUSTED][get_ename(tmp)]^=S_M_COLOR;
	      ret=sprintf("Farbe fuer %s %s.",tmp->Name(WEN),
	          Status[S_USERS][S_CU_TRUSTED][get_ename(tmp)]&S_M_COLOR?"ein":"aus");
	    break;
	    case "ebene":
	      if(sizeof(args)>=5)
	      {
	        if(args[3]=get_channel(args[3]))
		{
		  switch(args[4])
		  {
		    case "recv":
		    case "lesen":
		    case "empfangen":
                      new_banned_trusted(S_CU_TRUSTED,
		        sprintf(
			  "%s:%s:",get_ename(tmp),args[3]),S_M_RECV);
		      ret=sprintf("Spieler %s darf nun%s auf Ebene \"%s\" hören.",
		          tmp->Name(WER),
			  (ft=Status[S_CHANNELS][args[3]][S_CU_TRUSTED][get_ename(tmp)]&
			    S_M_RECV)?"":" nicht mehr",
			  args[3]);
		      if(!ft)
		      {
		        RemoveChannel(args[3],({object})tmp,0,2);
			if(ft&S_M_SEND)
			  new_banned_trusted(S_CU_TRUSTED,
		            sprintf("%s:%s:",get_ename(tmp),args[3]),S_M_SEND);
		      }
		    break;
		    case "send":
		    case "schreiben":
		    case "senden":
                      new_banned_trusted(S_CU_TRUSTED,
		        sprintf(
			  "%s:%s:",get_ename(tmp),args[3]),S_M_SEND);
		      ret=sprintf("Spieler %s darf nun%s auf Ebene \"%s\" senden.",
		          tmp->Name(WER),
			  Status[S_CHANNELS][args[3]][S_CU_TRUSTED][get_ename(tmp)]&
			    S_M_SEND?"":" nicht mehr",
			  args[3]);
		    break;
		  }
		}
	      }
	    break;
	  }
	}
	else
	{
	  switch(args[2])
	  {
	    case"create":
//	      if(!mappingp(Status[S_USERS][S_CU_TRUSTED][args[1]]))Status[S_USERS][S_CU_TRUSTED][args[1]]=0;
	      Status[S_USERS][S_CU_TRUSTED][args[1]]^=S_M_CREATE;
	      ret=sprintf("Object %s darf nun%s Ebenen erstellen.",args[1],Status[S_USERS][S_CU_TRUSTED][args[1]]&S_M_CREATE?"":" nicht mehr");
	    break;
	  }
	}
      }
    break;
    default:
      eval_control_string(msg,user);
    break;
  }
  if(ret)
  {
    SEND(STATUS_CHANNEL,ME,ret);
  }
}

varargs int SendToChannel(string channel, object user, int cmd, string msg, int f_col)
{string tmsg;
  if(!(channel=get_channel(channel)))return 1;

  if(!channelmember(channel,user)&&!(check_secure(user,channel,S_NEEDNOTENTER)))
  {
    tell_object(user,sprintf("Du hast die Ebene %s noch nicht betreten!\n",channel));
    return 1;
  }
  if(!(check_secure(user,channel,S_M_SEND)))
  {
    tell_object(user,sprintf("Du darfst auf %s nicht senden!\n",channel));
    return 1;
  }

  if(!(check_secure(user,channel,S_M_COLOR))&&!f_col){msg=parse_color_string(msg,0,1);}

  if((user)&&(cmd==SAY))
  {
    if(user->QueryProp(P_ALCOHOL))
    {
      tmsg=call_other(DRUNKDAEMON,"drunkspeach",msg,user);
      if(tmsg)msg=tmsg;
    }
  }

  if((channel[0..4]=="Inter"||channel[0..1]=="D-")&&user!=find_object("/secure/udp/channel")
      &&query_once_interactive(user)) // Nur spieler senden hier :)
  {
    mapping request; string *excl;
    request=([REQUEST  :"channel",
              SENDER   :get_name(user,cmd==GEMOTE?WESSEN:0,0),
              "CHANNEL":lower_case(channel),
              DATA     :implode(explode(parse_color_string(msg,0,1),"\n")," ")]);
    if(cmd==GEMOTE||cmd==EMOTE)
    {
      request["EMOTE"] = 1;
      request["CMD"] = "emote";
    }
    walk_mapping(INETD->query("hosts"),#'send_intermud,request,excl);
  }
  else if(channel==CONTROL_CHANNEL)
  {
    if(!(check_secure(user,0,S_M_NOTCONTROL)))
      if(find_call_out("eval_control_string")<0)call_out("eval_control_string",2+random(3),msg,user);
  }
  else if(channel==STATUS_CHANNEL)
  {
    if(find_call_out("eval_status_string")<0)call_out("eval_status_string",1,msg,user);
  }

  if(objectp(user)&&objectp(user->QueryProp(P_PERM_STRING)))
    msg=call_other(user->QueryProp(P_PERM_STRING),"permutate_string",(msg))||msg;
  if(msg[<1]=='\n') msg = msg[0..<2];

  map(Channels[channel][C_USERS],#'tell_user,channel,msg,user,cmd);
  add_hist(channel,user,msg,cmd);
  return 1;
}

private void print_hist(int t,string channel,object user)
{mapping hist;
string p_time;
  hist=Channels[channel][C_HIST][t];
//  tell_user(user,channel,sprintf("^W[%s]^N:",dtime(hist[3])),ME,SAY,MSGFLAG_CHIST);
//  tell_object(user,parse_color_string(sprintf("^W[%s]^N:\n",dtime(hist[3])),0,CAN_COL(user)));
  if(t==sizeof(Channels[channel][C_HIST])-1||(sizeof(Channels[channel][C_HIST])+1>t&&Channels[channel][C_HIST][t+1][3]!=hist[3]))
  {
    p_time=dtime(hist[3]);
    if(p_time[0..18]==dtime(time())[0..18])p_time="Heute, "+p_time[19..];
    else if(p_time[13..16]==dtime(time())[13..16])p_time=p_time[0..11]+p_time[17..];
    tell_object(user,sprintf("[%s]:\n",p_time));
  }
  tell_user(user,channel,sprintf("%s",hist[1]),
           (check_secure(user,channel,S_TRUSTED))?hist[0][1]:hist[0][0],hist[2],
	   MSGFLAG_CHIST);
}

int ListUsers(string channel, object user)
{string rstr;
  if(channel=get_channel(channel))
  {
    check_channel_consistency(channel);
    if(!check_secure(user,channel,S_M_ENTER))
    {
     tell_object(user,sprintf("Du darfst die Ebene %s nicht betreten!\n",channel));
      return 1;
    }
/*    if(!channelmember(channel,user))
    {
      tell_object(user,sprintf("Du hast die Ebene %s noch nicht betreten!\n",channel));
      return 1;
    }*/
    rstr=sprintf("Folgende Spieler hören auf %s:\n",channel);
    rstr+=uList(channel,user);
    rstr+=sprintf("\n\nDie Ebene wird von %s kontrolliert.\nModus: %s|%s.\n",
                  get_name(Channels[channel][C_OWNER],0,check_secure(user,channel,S_TRUSTED)),
		  Channels[channel][C_FLAGS]&CF_KEEP?"hart":"weich",Channels[channel][C_FLAGS]&CF_PRIVATE?"privat":"öffentlich");
    rstr+=break_unprocessed_string(sprintf("Thema: %s",Channels[channel][C_INFO]));
    tell_object(user,parse_color_string(rstr,0,CAN_COL(user)));
  }
  return 1;
}

int ChannelHistory(string channel, object user, mixed number)
{
  int *inds, hist_size, new_size;
  mapping map;

  if(!(channel=get_channel(channel)))return 1;
  if(!check_secure(user,channel,S_M_ENTER))
  {
    tell_object(user,sprintf("Du darfst die Ebene %s nicht betreten!\n",channel));
    return 1;
  }
/*  if(!channelmember(channel,user))
  {
    tell_object(user,sprintf("Du hast die Ebene %s noch nicht betreten!\n",channel));
    return 1;
  }*/
  inds=m_indices(Channels[channel][C_HIST]);
  if(!sizeof(inds))
  {
    tell_user(user,channel,"Allein sphärisches Rauschen erfüllt diese Ebene.",ME,SAY,MSGFLAG_CHIST);
  }
  else
  {
//    tell_user(user,channel,"Was bisher geschah:",ME,SAY,MSGFLAG_CHIST);

    // History einschraenken?
    hist_size=sizeof(inds); new_size=hist_size;
    if(mappingp(map=user->QueryProp(P_CHANNELS)[channel]) &&
       map[PCHAN_HIST]>0 && map[PCHAN_HIST]<hist_size) new_size=map[PCHAN_HIST];
    if(number && intp(number) && number>0)
      if(number<hist_size) new_size=number;
      else new_size=hist_size;

    inds=sort_array(inds,#'<)[<new_size..];

    tell_object(user,parse_color_string("\t\t\t^WWas bisher geschah:^N\n",0,CAN_COL(user)));
    map(inds,#'print_hist,channel,user);
  }
//  tell_user(user,channel,"------------------\n",ME,SAY,MSGFLAG_CHIST);
  tell_object(user,parse_color_string("\t\t^W-----------------------------------^N\n",0,CAN_COL(user)));
  return 1;
}

mixed Debug(string var)
{
  switch(var)
  {
    case "Status":
      return Status;
    break;
    case "Users":
      return Users;
    break;
    case "ein":
      debug=1;
    break;
    case "aus":
      debug=0;
    break;
    default:
      return Channels;
    break;
  }
  return;
}
