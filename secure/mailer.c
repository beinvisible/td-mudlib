/* Tamedhon MUDlib
 *
 * /secure/mailer.c -- mail daemon
 *
 * $Date: 2002/03/30 19:20:00 $
 * $Revision: 1.5 $
 * $Log: mailer.c,v $
 * Revision 1.5  2002/03/30 19:20:34  Querolin
 * Diverse anpassungen an Tamedhon 
 *
 * Revision 1.4  1998/02/09 18:03:34  Woody
 * "Testmud" sowie "Adresse auf Anfrage" rausgenommen
 *
 * Revision 1.3  1997/03/20 04:12:17  Woody
 * Adresse fuer outgoing mail korrigiert, RCTP -> RCPT typo entfernt
 *
 * Revision 1.2  1997/03/16 22:11:25  Woody
 * Caching, DeleteUnreadFolder, small changes (Loco@MG)
 * Use GetFolders() instead of restore_object()
 *
 * Revision 1.1  1997/03/15 23:11:48  Woody
 * prepared for revision control
 *
 */

/*
 *------------------------------------------------------------
 * The mail demon. Receives mail from users and delivers it into
 * the mail directory.
 *
 * Deepthought, Nightfall, 25-May-92
 * Remove-Functions : Jof, 29-June-92
 * Caching, DeleteUnreadFolder, small changes: Loco, 08-Feb-97
 *   DON'T USE restore_object any more, use GetFolders instead!
 *------------------------------------------------------------
 *
 *	Save file format (sort of formal notation):
 *
 *	mixed *folders = ({
 *	   ({ string name1; string name2; ... string nameN; })
 *	   ({ mixed *msgs1; mixed *msgs2; ... mixed *msgsN; })
 *	})
 *
 *	Each msgs field is an array of messages:
 *
 *	mixed *msgs = ({ mixed *message1; ... mixed *messageM })
 *
 *	A message is represented as an array with the following fields:
 *
 *	mixed *message = ({
 *	   string from;
 *	   string sender;
 *	   string recipient;
 *	   string *cc;
 *	   string *bcc;
 *	   string subject;
 *	   string date;
 *	   string id;
 *	   string body;
 *	})
 *
 *	The mailer demon (/secure/mailer, or /obj/mailer) provides
 *	the following functions:
 *
 *	string *DeliverMail(mixed *message)
 *	  Hand a mail message over to the mailer demon. The mailer
 *	  demon extracts recipients from the recipient, cc and bcc
 *	  fields and removes the bcc information. It then deposits
 *	  the message to the mail files of all recipients. A valid
 *	  message is shown above. Returns a list of successfully
 *	  delivered recipients.
 *
 *	int FingerMail(string user)
 *	  Gives the number of unread messages a user has.
 *------------------------------------------------------------
 */

#include <config.h>
#include <mail.h>

#define TELLQ(msg) if(find_player("querolin")) \
                     tell_object(find_player("querolin"), msg);

#define DEBUG(msg) if(find_player("debugger")) \
                     tell_object(find_player("debugger"), msg)
#undef DEBUG
#define DEBUG(x)

#define NOTIFY_RECIPIENT  // write out a message to the recipient
#define SECURITY(x)   (geteuid(x)==ROOTID || geteuid(x)==MAILID)

mixed *folders;		/* used for save and restore of mail files */
static mapping alias;
static string cachedname; /* whose folder is still in memory? */
static void save_msg(mixed msg, mixed user);

void create()
{
  mixed tmp;
  int i;
  string s1,s2;
  
  seteuid(ROOTID);
  alias=([]);
  if (tmp=read_file("/mail/system.mailrc"))
  {
    tmp=explode(tmp,"\n");
    for (i=sizeof(tmp)-1;i>=0;i--)
      if (sscanf(tmp[i],"%s %s",s1,s2)==2)
	alias[s1]=s2;
  }
}

// GetFolders laedt einen folder, wenn er nicht im cache ist, und gibt
// 0 zurueck, wenn der folder nicht vorhanden oder evtl auch leer ist.
// Sinn: Vor allem bei Listenargumenten im mailer kann es leicht vorkommen,
// dass dasselbe mailfile einige Male hintereinander gebraucht wird.

static int GetFolders(string user) {
  if (user==cachedname) {
    DEBUG("Using cached folder for "+user+"\n");
    return sizeof(folders[1]);
  }
  cachedname=user;
  if (!restore_object(MAILPATH+"/"+user[0..0]+"/"+user)) {
    DEBUG("Loading folder: "+user+" (empty)\n");
    folders=({({}),({})});
    return 0;
  }
  DEBUG("Loading folder:"+user+"\n");
  return 1;
}

string *unify(string *str)
{
  string *new;
  
  new=({});
  while (pointerp(str) && sizeof(str))
  {
    if (str[0])
    {
//      DEBUG(sprintf("%O\n",str[0]));
      if (str[0][0]=='\\') str[0]=str[0][1..];
      if (member(new,lower_case(str[0]))==-1)
	new+=({lower_case(str[0])});
    }
    str=str[1..];
  }
  return new;
}

string* expand(mixed addr,mixed expa)
// expa: also do alias and forward-expansion? (for inbound external mail)
{
  string s,tmp, *new, *ret;
  int i,j;

  ret=({});
  addr-=({""});
  j=sizeof(addr);
  for (i=0;i<j;i++)
  {
    s=lower_case(addr[i]);
    new=explode(s,"@");
//    DEBUG(sprintf("Now we have %O\n",new));
    if (sizeof(new)==2 &&
	(new[1]=="sl"||new[1]=="tamedhon"||new[1]=="mud"))
      /*
         || new[1]=="mg.uni-muenster.de"
         || new[1]=="mud.uni-muenster.de"))
      */
      s=new[0];
    if (!expa&&tmp=alias[s])
      ret+=explode(tmp,",");
    else
      ret+=({s});
  }
  for (i=sizeof(ret)-1;i>=0;i--) 
  {
    if (ret[i][0]=='\\') ret[i]=ret[i][1..];
    else if (expa)
      ret = ret-({ret[i]})+explode(FWSERV->QueryForward(ret[i]),",");
  }
  return ret;
}

varargs string* DeliverMail(mixed msg,mixed expa) {
  string sender;
  string *recipients;
  string *recok;
  mixed *tmp;
  mixed *newmsg;
  int i,ext;
  string footer;
  string t,str;
  
  if (!pointerp(msg) || sizeof(msg) != 9)
    return 0;
  DEBUG(sprintf("DeliverMail: %O %O\n",msg,expa));
  t=ctime(time());ext=0;
  footer="\n"
    "*****************************************************************\n"
    "*  Tamedhon MailRelay v1.1 - Processed   "+
    t[4..10]+t[20..]+", "+t[11..18]+"  *\n"
    "*    Tamedhon - mud.tamedhon.de  Port 4711 - www.tamedhon.de    *\n"
    "*****************************************************************";
  
  /* determine the real sender */
  if (extern_call() &&
      (!previous_object()||object_name(previous_object())[0..7]!="/secure/") &&
      (!previous_object()||object_name(previous_object())[0..5]!="/mail/"))
  {
//    DEBUG(object_name(previous_object()));
    sender = getuid(this_interactive());
  }
  else
//    sender = msg[MSG_SENDER];
   sender = msg[MSG_FROM];
  
  /* make a list of all recipients */
  recipients = ({msg[MSG_RECIPIENT]});
  if (pointerp(msg[MSG_CC])) recipients+=msg[MSG_CC];
  if (pointerp(msg[MSG_BCC])) recipients+=msg[MSG_BCC];

  recipients=expand(recipients,expa);
  recipients=unify(recipients);

//  DEBUG(sprintf("NEED TO DELIVER TO %O\n",recipients));

  /* build the new message */
  newmsg = ({ msg[MSG_FROM], sender, msg[MSG_RECIPIENT],
	      msg[MSG_CC], "", msg[MSG_SUBJECT],
	      dtime(time()), MUDNAME+":"+time(),
	      msg[MSG_BODY] });

  /* Send it off ... */
  recok = ({ });
  for (i = 0; i < sizeof(recipients); i++) {
    mixed *xtmp;

//    if (member(recipients[i],'%')>0 && member(recipients[i],'@')<0)
//    {
//      xtmp=explode(recipients[i],"%");
//      recipients[i]=implode(xtmp[0..<2],"%")+"@"+xtmp[<1];
//    }
    if(member(recipients[i],'@')>0 && member(recipients[i],'%')<0 &&
       member(recipients[i],'!')<0 && strstr(recipients[i],"DAEMON")<0 && 
       strstr(recipients[i],"daemon")<0)
    {
      string mud;
      mixed rec;

      rec=explode(recipients[i],"@");
      mud=rec[1];
      rec=rec[0];
      if (member(mud,'.')==-1)
      {
      	sender=implode(explode(sender,"@"),"%");
        "/secure/udp_mail"->deliver_mail(rec,mud,sender,msg[MSG_SUBJECT],
					 msg[MSG_BODY]);
      }
      else
      {
        ext=1;
//        tell_object(find_player("bort"),break_string("Outbound!",78));
	sender=implode(efun::explode(sender,"@"),"%");
	sender=explode(sender,"%")[0];
	rec=implode(efun::explode(rec,"@"),"%");
	rec=explode(rec,"%")[0];
	mud=implode(efun::explode(mud,"@"),"%");
	mud=explode(mud,"%")[0];

//	write_file(
//           sprintf("/mail/OutBound/%s.%d-%d-%d",
//                    sender, time(), i, random(123456)),
        str=(sprintf("HELO tamedhon.de\n"
                    "MAIL FROM: %s@mud.tamedhon.de\n"
                    "RCPT TO: %s@%s\n"
                    "DATA\n"
                    "Subject: %s\n\n"
                    "MUD-From: %s\n"
                    "MUD-To: %s\n"
                    "MUD-Cc: %s\n"
                    "MUD-Subject: %s\n\n", sender, rec, mud,
                    msg[MSG_SUBJECT], sender, recipients[0],
                    pointerp(msg[MSG_CC])?implode(msg[MSG_CC],","):"",
                    msg[MSG_SUBJECT])+msg[MSG_BODY]+footer+
            "\n."
            "\n\nQUIT\n");

      write_file(
           sprintf("/mail/OutBound/a%s.%d-%d-%d",
                    sender, time(), i, random(123456)), str);

      //tell_object(find_player("bort"), break_string(str, 78));

      }
      recok+=({recipients[i]});
    }
    else
      if (file_size(SAVEPATH+recipients[i][0..0]+"/"+
		    recipients[i]+".o") >=0) {
	save_msg(newmsg,recipients[i]);
	recok += ({ recipients[i] });
      }
      else
   	write_file(
            sprintf("/mail/OutBound/postmaster.%d-%d",time(),random(time())),
            sprintf("MAIL FROM: mud@tamedhon.de\n"
                    "RCPT TO: mud@tamedhon.de\n"
                    "DATA: Subject: Undeliverable Mail\n"
                    "%O\n"
                    ".\n"
                    "QUIT\n", newmsg));
  }
  if (ext) send_udp(UDPSERV,4123,"DELIVERMAIL");
  return recok;
}

int FingerMail(mixed user) {
  int newfolder;
  int i;
  
  if (!GetFolders(user)) return 0;
  newfolder = member(folders[0], "unread");
  if (newfolder == -1) {
    return 0;
  }
  return sizeof(folders[1][newfolder]);
}

sort_folders(mixed mail1, mixed mail2)
{
   if (!sizeof(mail1)) return 0;
   if (!sizeof(mail2)) return 0;
   if (mail1[MSG_ID] > mail2[MSG_ID]) {
//     tell_object(find_player("bort"), "sort_folders resorted!"+mail1[MSG_ID]+">"+mail2[MSG_ID]+"\n");
     return 1;
   }
   return 0;
}

static void save_msg(mixed msg, mixed user) {
  int newfolder,i;
  mixed sort_folders;
  object p;
  
  GetFolders(user);

  /* if folder 'unread' doesn't exist, create it */
  newfolder = member(folders[0],"unread");
  if (newfolder == -1) {
    folders[0] += ({ "unread" });
    folders[1] += ({ ({ }) });
    newfolder = member(folders[0],"unread");
  }
  folders[1][newfolder] += ({ msg });

  // Mails sortieren nach Datum
  if (sizeof(folders[0])>1)
  {
     for(i=0;i<sizeof(folders[1]);i++) {
       sort_folders=sort_array(folders[1][i], #'sort_folders );
       if (sizeof(sort_folders)) folders[1][i]=sort_folders;
     }
  }

  save_object(MAILPATH+user[0..0]+"/"+user);
#ifdef NOTIFY_RECIPIENT
  if (p=find_player(user))
    tell_object(p,"Ein Postreiter ruft Dir aus einiger Entfernung zu, "
        "dass Du neue Post hast!\n");
#endif
}

/* Remove a message from a folder */
int RemoveMsg(mixed msg, mixed folder, mixed user) {
  mixed *first, *rest;

  /* if (user != geteuid(previous_object())) return -2; /* Disallow */
  if (!SECURITY(previous_object())) return -2;

  if (!GetFolders(user)) 
    return -1; /* No such folder */

  if (!pointerp(folders) || !pointerp(folders[0]) || 
      folder>=sizeof(folders[0])) return -1;

  if (sizeof(folders[1][folder])<=msg || msg<0) return 0; /* No such msg */

  if (msg==0)
    first==({});
  else
    first=folders[1][folder][0..msg-1];
  if (msg==sizeof(folders[1][folder]))
    rest=({});
  else
    rest=folders[1][folder][msg+1..sizeof(folders[1][folder])];
  if (!rest) 
    folders[1][folder]=first;
  else 
    if (!first) 
      folders[1][folder]=rest;
    else
      folders[1][folder]=first+rest;
  save_object(MAILPATH+user[0..0]+"/"+user);
  return 1; /* Success */
}

/* Move message into another folder */
int MoveMsg(mixed msg, mixed folder, mixed newfolder, mixed user) {
  mixed sort_folders;
  int target,i;

  /* if (user != geteuid(previous_object())) return -2; /* Disallow */
  if (!SECURITY(previous_object())) return -2;

  if (!GetFolders(user))
    return -1; /* Source folder not found */

  if (!pointerp(folders) || !pointerp(folders[0]) || 
      folder>=sizeof(folders[0])) return -1;
  
  if (sizeof(folders[1][folder])<=msg || msg<0) return 0; /* No such msg */

  target=member(folders[0],newfolder);

  if (target==-1) return -3;

  if (target==folder) return 1;

  if (!pointerp(folders[1][target])) 
    folders[1][target]=({folders[1][folder][msg]});
  else 
    folders[1][target]+=({folders[1][folder][msg]});

  // Mails sortieren nach Datum
  if (sizeof(folders[1])>1)
  {
     for(i=0;i<sizeof(folders[1]);i++) {
       sort_folders=sort_array(folders[1][i], #'sort_folders );
       if (sizeof(sort_folders)) folders[1][i]=sort_folders;
     }
  }

  save_object(MAILPATH+user[0..0]+"/"+user);
  return RemoveMsg(msg,folder,user);
}

int MoveAllMessages(mixed folder, mixed newfolder, mixed user) {
  int target,i;

  /* if (user != geteuid(previous_object())) return -2; /* Disallow */
  if (!SECURITY(previous_object())) return -2;

  if (!GetFolders(user))
    return -1; /* Source folder not found */

  if (!pointerp(folders) || !pointerp(folders[0]) ||
      folder>=sizeof(folders[0])) return -1;

  target=member(folders[0],newfolder);
  if (target==-1 || target==folder)
    return 0;
  if (!pointerp(folders[1][target]))
    folders[1][target]=({});

  if (pointerp(folders[1][folder]))
    folders[1][target]+=folders[1][folder];
  folders[1][folder]=({});

  save_object(MAILPATH+user[0..0]+"/"+user);
  return 0;
}


int DeleteUnreadFolder(mixed user) {
  int unread,newmail;

  if (!SECURITY(previous_object())) return -2;

  if (!GetFolders(user))
    return -1; /* Source folder not found */

// can't happen. Checked in GetFolders
//  if (!pointerp(folders) || !pointerp(folders[0])) return -1;

  unread=member(folders[0],"unread");
  newmail=member(folders[0],"newmail");
  if (unread==-1) return 0;
  if (newmail==-1) {
    folders[0]+=({"newmail"});
    folders[1]+=({({})});
    newmail=sizeof(folders[1])-1;
  }
  if (!pointerp(folders[1][newmail]))
    folders[1][newmail]=({});
  if (pointerp(folders[1][unread]))
    folders[1][newmail]+=folders[1][unread];
  folders[0]=folders[0][0..unread-1]+folders[0][unread+1..];
  folders[1]=folders[1][0..unread-1]+folders[1][unread+1..];

  save_object(MAILPATH+user[0..0]+"/"+user);
  return 0;
}


int RemoveFolder(mixed folder, mixed user) {
  int i;
  mixed *start, *rest, *start0, *rest0;

  if (!SECURITY(previous_object())) return -2;

  if (!GetFolders(user)) return -1; /* No such f */

  i=member(folders[0],folder);

  if (i<0) return -1; /* No such folder */

  if (sizeof(folders[1][i])>0) return 0; /* Folder not empty */

  if (i==0) {
    start=({});
    start0=({});
  } else {
    start=folders[1][0..i-1];
    start0=folders[0][0..i-1];
  }
  if (i==sizeof(folders[0])-1){
    rest=({});
    rest0=({});
  } else {
    rest=folders[1][i+1..sizeof(folders[0])-1];
    rest0=folders[0][i+1..sizeof(folders[0])-1];
  }
  folders[0]=start0+rest0;
  folders[1]=start+rest;
  save_object(MAILPATH+user[0..0]+"/"+user);
  return 1;
}

int MakeFolder(mixed folder, mixed user)
{
  if (!SECURITY(previous_object())) return -2;
  if (!folder) return -1; /* Huh ? */

  if (folder=="unread") return 0; /* Folder exists virtually :) */

  /* if (geteuid(previous_object()) != user) return -2; /* Disallow */
  

  GetFolders(user);

  if (member(folders[0],folder)!=-1) return 0; /* Folder exists */

  folders[0]=folders[0]+({folder});
  folders[1]=folders[1]+({({})});
  save_object(MAILPATH+user[0..0]+"/"+user);
  return 1;
}

/* no shadowing */
int query_prevent_shadow() { return 1; }

int query_recipient_ok(string name)
{
  return 
  (file_size("secure/save/"+name[0..0]+"/"+name+".o")>0||
   member(name,'%')>0||member(name,'@')>0);
}

void deliver_mail(
		  string recipient,       /* the local players real name*/
		  string from,            /* A string depicting the sender */
		  string subject,         /* The mail subject/header */
		  string mail_body        /* The actual mail message */
		  )
{
  DEBUG(sprintf("DELIVER %O\n",({from,from,recipient,({}),({}),
      subject,time(),"EXTERNAL",mail_body})));
  DeliverMail(({from,from,recipient,({}),({}),
      subject,time(),"EXTERNAL",mail_body}));
}

