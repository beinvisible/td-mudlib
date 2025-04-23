/* SilberLand MUDlib
 *
 * /secure/master/network.c -- UDP-Handling
 * 
 * 2024/04/09 - (void) bei rm entfernt. send_imp mit send_udp ersetzt. udp_query und get_ip_name nach MG vorbild angepasst. ftpaccess kommentiert - Aloy
 *
 * $Date: 2001/01/18 10:37:54 $
 * $Revision: 1.1 $
 * $Log: network.c,v $
 * Revision 1.1  2001/01/18 10:37:54  Samtpfote
 * Initial revision
 *
 * Revision 1.5  1997/07/06 15:53:10  Woody
 * FTP Fileliste ohne Leerzeilen
 *
 * Revision 1.4  1997/03/21 23:59:54  Woody
 * Reply-To: Zeile von Inbound Mail in den Body aufnehmen
 *
 * Revision 1.3  1997/03/21 01:17:27  Woody
 * mailread() bereinigt ueberfluessige Spaces im Header
 *
 * Revision 1.2  1997/03/17 19:29:56  Woody
 * Bei finger neue /p Verzeichnisstruktur beruecksichtigt
 *
 * Revision 1.1  1997/03/16 00:04:09  Woody
 * prepared for revision control
 *
 */

#pragma strong_types

#include "/secure/master.h"

string *mk_rec_list(string str)
{
  DEBUG("\n"+str+"\n");
  return explode(lower_case(implode(explode(str," "),"")),",");
}

static int CheckPasswd(string name, string passwd)
{
  mixed *uinf;

  uinf=get_full_userinfo(name);
  if (sizeof(uinf)<2) return 0;
  return (crypt(passwd,uinf[USER_PASSWORD+1])==uinf[USER_PASSWORD+1]);
}

/* NASEFTPD - FtpAccess
static void FtpAccess(string host, string message, int port)
{
  string *comp;
  string reply, head;
  string *dir;
  int i;

#if __EFUN_DEFINED__(send_udp)
  comp = explode(message,"\t");
#define FTP_ID   0
#define FTP_SEQ  1
#define FTP_TAG  2
#define FTP_CMD  3
#define FTP_ARG1 4
#define FTP_ARG2 5
#define FTP_ARG3 6
  
  if (sizeof(comp) <= FTP_CMD || lower_case(comp[FTP_TAG]) != "req")
  {
    log_file("IMP_MSGS","Host: "+host+":"+port+" - '"+message+"'\n");
    return;
  }
  
  reply = "INVALID";
  head = sprintf("%s\t%s\tRPLY\t%s\t", comp[FTP_ID], comp[FTP_SEQ], comp[FTP_CMD]);
  
  switch (lower_case(comp[FTP_CMD]))
  {
    case "ping":
    if (sizeof(comp) <= FTP_CMD)
      break;
    reply = "OK";
    break;
    
    case "user":
    if (sizeof(comp) <= FTP_ARG1)
      break;
    reply = "/players/"+lower_case(comp[FTP_ARG1]);
    break;
    
    case "pass":
    if (sizeof(comp) <= FTP_ARG2)
      break;
    comp[FTP_ARG1] = lower_case(comp[FTP_ARG1]);
    if (   CheckPasswd(comp[FTP_ARG1],comp[FTP_ARG2]) 
	   && ("guest" == comp[FTP_ARG1] || IS_LEARNER(comp[FTP_ARG1]))
	   )
      reply = "OK";
    else
      reply = "FAIL";
    break;
    
    case "resv":
    if (sizeof(comp) <= FTP_ARG3)
      break;
    if (   comp[FTP_ARG2] == "" 
	   || -1 == member("+~/", comp[FTP_ARG2][0]))
      comp[FTP_ARG2] = comp[FTP_ARG3]+"/"+comp[FTP_ARG2];
    reply = _get_path(comp[FTP_ARG2], lower_case(comp[FTP_ARG1]));
    break;
    
    case "read":
    if (sizeof(comp) <= FTP_ARG2)
      break;
    comp[FTP_ARG1] = lower_case(comp[FTP_ARG1]);
    if (   (IS_LEARNER(comp[FTP_ARG1]) || comp[FTP_ARG2][0..4]=="/open")
	   && comp[FTP_ARG2][0] == '/'
	   && valid_read(comp[FTP_ARG2], comp[FTP_ARG1], "read_file", 0)
	   )
      reply = "OK";
    else
      reply = "FAIL";
    break;
    
    case "writ":
    if (sizeof(comp) <= FTP_ARG2)
      break;
    if (   comp[FTP_ARG2][0] == '/'
	   && valid_write(comp[FTP_ARG2], lower_case(comp[FTP_ARG1]), "write_file", 0)
	   )
      reply = "OK";
    else
      reply = "FAIL";
    break;
    
    case "list":
    if (sizeof(comp) <= FTP_ARG2)
      break;
    dir = explode(show_dir(comp[FTP_ARG2], comp[FTP_ARG1]),"\n")-({""});
    for (i = 0; i < sizeof(dir); i++)
      send_udp(host, port, head+"LINE\t"+dir[i]+"\n");
    reply = "OK";
    break;

    default:
    log_file("IMP_MSGS","Host: "+host+":"+port+" - '"+message+"'\n");
    break;
  }
  
  send_udp(host, port, head+reply);
#endif
}
*/

// The function 'FtpAccess()' is located within our master-object and is
// called by receive_imp(). It is responsible for the communication with
// ProFtpd and controls the access rights.

// static void FtpAccess( string host, string message, int port )
// {
//     string *comp, reply, head;
// 
// #if __EFUN_DEFINED__(send_udp)
//     comp = efun::explode( message, "\t" );
// #define FTP_ID   0
// #define FTP_SEQ  1
// #define FTP_TAG  2
// #define FTP_CMD  3
// #define FTP_ARG1 4
// #define FTP_ARG2 5
// #define FTP_ARG3 6
// 
//     // unknown requests are logged
//     if ( sizeof(comp) <= FTP_CMD || lower_case(comp[FTP_TAG]) != "req" ){
//         log_file( "IMP_MSGS", "Host: " + host + ":" + port + " - '" +
//                   message + "'\n" );
//         return;
//     }
// 
//     // prepare the answer; default is 'not allowed'
//     reply = "INVALID";
//     head = sprintf( "%s\t%s\tRPLY\t%s\t",
//                     comp[FTP_ID], comp[FTP_SEQ], comp[FTP_CMD] );
// 
// 
//     // now let's see what is requested:
//     switch ( lower_case(comp[FTP_CMD]) ){
//     // a user wants to log in; test if it is a valid account
//     case "user":
//         if ( sizeof(comp) <= FTP_ARG1 )
//             break;
//         
//         // only wizards have ftp-accounts
//         // anonymous accounts are handled by ProFtpd
//         if ( IS_LEARNER(lower_case(comp[FTP_ARG1])) )
//             reply = "/players/" + lower_case(comp[FTP_ARG1]);
//         else
//             reply = "NONE";
//         break;
// 
//     // authenticate user; does password match username?
//     case "pass":
//         if ( sizeof(comp) <= FTP_ARG2 )
//             break;
//         
//         comp[FTP_ARG1] = lower_case(comp[FTP_ARG1]);
//         
//         // only wizards have ftp-accounts
//         // anonymous accounts are handled by ProFtpd
//         if ( IS_LEARNER(comp[FTP_ARG1]) ){ 
//             if ( CheckPasswd( comp[FTP_ARG1], comp[FTP_ARG2] ) )
//                 reply = "OK";
//             else 
//                 // failed logins are logged
//                 log_file( "LOGINFAIL",
//                           sprintf( "BAD PASSWORD:      (FTP)     %s %s\n",
//                                    comp[FTP_ARG1],
//                                    ctime(time()) ) );
//         }
//         else 
//             reply = "FAIL";
//         break;
// 
//     // user wants to read files; is he allowed to?
//     case "read":
//         if ( sizeof(comp) <= FTP_ARG2 )
//             break;
//         
//         if ( comp[FTP_ARG2][0] == '/' &&
//              // check if read-access is valid
//              // access-rights for anonymous accounts are handled by ProtFpd
//              valid_read( comp[FTP_ARG2], lower_case(comp[FTP_ARG1]),
//                          "read_file", 0 ) )
//             reply = "OK";
//         else
//             reply = "FAIL";
//         break;
// 
//     // is user allowed to write the given file?
//     case "writ":
//         if ( sizeof(comp) <= FTP_ARG2 )
//             break;
//         
//         if ( comp[FTP_ARG2][0] == '/' &&
//              // the same as above
//              valid_write( comp[FTP_ARG2], lower_case(comp[FTP_ARG1]),
//                           "write_file", 0 ) )
//             reply = "OK";
//         else
//             reply = "FAIL";
//         break;
// 
//     // is user allowed to list contents of directory?
//     // NOTE: you need permissions to list the contents of a directory
//     // to 'cd' into it with ProFtpd
//     case "list":
//         if ( sizeof(comp) <= FTP_ARG2 )
//             break;
//         
//         if ( comp[FTP_ARG2][0] == '/' &&
//              // In contrast to NaseFtp, the mud-module for ProFtpd
//              // only expects "OK" or "FAIL" as an answer to the list-command.
//              // The directory listing is built by ProFtpd itself.
//              // Thus it is possible that a user can see a file but isn't
//              // allowed to read it`s contents.
//              // This solution isn't as flexible as the way NaseFtp handles
//              // directory listings, but it is _way_ faster.
//              valid_read( comp[FTP_ARG2], lower_case(comp[FTP_ARG1]),
//                          "read_file", 0 ) )
//             reply = "OK";
//         else
//             reply = "FAIL";
//         break;
//         
//     default:
//         // unknown requests are logged
//         log_file( "IMP_MSGS", "Host: " + host + ":" + port + " - '" +
//                   message + "'\n" );
//         break;
//     }
// 
//     // send the answer
//     send_udp( host, port, head+reply );
// #endif
// }



void doReadMail(string file)
{
  mixed *lines, *message;
  string rest,what,*line;
  int i;
  int to_read,pos,now;
  
  DEBUG("READING "+file+"\n");
  to_read=file_size(file);
  if (to_read>250000||to_read<5)
  {
    rm(file);
    return;
  }
  rest="";
  pos=0;
  while (to_read>0)
  {
    now=to_read;
    if (now>20000) now=20000;
    rest+=read_file(file,pos,pos+now-1);
    pos+=now;
    to_read-=now;
  }
  DEBUG("GOT "+rest+"\n");
  lines=explode(rest,"\n");
  if (!sizeof(lines))
    return;
  i=0;
  while (i<sizeof(lines) && lines[i]!="")
  {
    i++;
  }
  if (i==sizeof(lines))
    return;
  rest=implode(lines[i..],"\n");
  message=allocate(9);
  message[MSG_BODY]="";
  for (;i>=0;i--)
  {
    line=explode(lines[i],":");
    if (sizeof(line)>1)
    {
      what=lower_case(line[0]);
      line[0]=implode(line[1..],":");
      line[0]=implode(explode(line[0], " ")-({""}), " ");
      switch (what)
      {
        case "reply-to":
          message[MSG_BODY] += lines[i] + "\n";
          break;
        case "subject":
          message[MSG_SUBJECT] = line[0];
          break;
        case "from":
          message[MSG_FROM] = line[0];
          message[MSG_SENDER] = line[0];
          break;
        case "cc":
          message[MSG_CC] = mk_rec_list (line[0]);
          break;
        case "bcc":
          message[MSG_BCC] = mk_rec_list (line[0]);
          break;
        case "to":
          message[MSG_RECIPIENT] = mk_rec_list (line[0])[0];
          break;
      }
    }
  }
  message[MSG_BODY]+=rest;
  if (!message[MSG_FROM]||!message[MSG_RECIPIENT])
    return;
  "/secure/mailer"->DeliverMail(message);
  rm(file);
}

void mailread()
{
  string *files;
  
  files=get_dir("/mail/InBound/*")-({"..","."});
  while (sizeof(files))
  {
    doReadMail("/mail/InBound/"+files[0]);
    files=files[1..];
  }
}

void udp_query(string query,string host,int port)
{
  string *mess,tmp,*tmp2;
  mixed *data;
  int i,j;
  
  mess=explode(query," ");
  switch (mess[1])
  {
    case "wholist":
    case "who":
    data="/obj/werliste"->QueryWhoListe();
    break;
    case "uptime":
	  data=({funcall(symbol_function("uptime"))});
	  break;
    case "finger":
    if (sizeof(mess)<3)
      data=({"Error: Wen soll ich fingern ?"});
    else
      data=explode(FINGERMASTER->finger_single(lower_case(mess[2]),0),"\n");
    break;
    case "mailread":
    data=({"Okay"});
    mailread();
    break;
    default:
    data=({"Error: unknown request "+mess[1]+"\n"});
  }
  // send_udp(host,port,sprintf("%s 0 %d",mess[0],sizeof(data)));
  // for (i=0;i<sizeof(data);i++)
  //   send_udp(host,port,sprintf("%s %d %s",mess[0],i+1,data[i]));
      send_udp( host, port,
        to_bytes(sprintf( "%s 0 %d", mess[0], sizeof(data)),
                 "ASCII//TRANSLIT"));

    for ( i = 0, j = sizeof(data); i < j; i++ )
        send_udp( host, port,
            to_bytes(sprintf( "%s %d %s", mess[0], i+1, data[i]),
                     "ASCII//TRANSLIT") );
}

void get_ip_name(string ip_num)
{
  send_udp(UDPSERV,4123,to_bytes("IPNAME|"+ip_num,"ASCII//TRANSLIT"));
}

#if __EFUN_DEFINED__(query_ip_port)
void get_auth_user(object pl)
{
  string s;
  
//  send_udp(UDPSERV,4123,sprintf("AUTH|%s|%s|%d|%d",getuid(pl),query_ip_number(pl),query_ip_port(pl),query_mud_port(pl)));
  s=sprintf("AUTH|%s|%s|%d|%d",getuid(pl),query_ip_number(pl),query_ip_port(pl),query_mud_port());
  send_udp(UDPSERV,4123,s);
}
#endif
