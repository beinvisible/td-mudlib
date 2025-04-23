// Tamedhon MUDlib
//
// /p/service/obj/fwserv.c -- Mail Forward Server
//
// $Date: 1997/03/16 21:05:22 $
/* $Revision: 1.1 $
 * $Log: fwserv.c,v $
 * Revision 1.1  1997/03/16 21:05:22  Woody
 * prepared for revision control
 *
 */

/***************************************************************************

 Forward-Server
 Teil des MG-Mailsystems
 (C) 1996 Loco@Morgengrauen

 Es gilt dasselbe Copyright wie fuer mailer.c, bitte dort nachlesen.

***************************************************************************/

#include <properties.h>
#include "/secure/config.h"
#include "/mail/post.h"

#define LOG "/p/service/save/FWLOG"

static int Security(string name);

static mixed forwarddata;
mixed properties;


void create() {
  seteuid(getuid());
  forwarddata=([]);
}


void LoadFile(mixed s) {
  if (!forwarddata[s])
    forwarddata[s]=FWSAVESERV->LoadData(s);
  if (!forwarddata[s])
    forwarddata[s]=([]);
}


string QueryForward(string adr) {
/* Setze eine Adresse um. Rueckgabe: Forwardadresse oder Name, wenn keine
   gesetzt ist. Externe Adressen werden nicht umgesetzt. */
  string s;
  object player;
  int i;

  if (member(adr, '@')>=0) return adr; /* Externe Adresse */

  for (i=strlen(adr)-1;i>=1;i--) {
    if (adr[i]>'z'||adr[i]<'a') return "*UNGÜLTIGER NAME: "+adr+" *";
  }
  if (adr[0]=='\\') return adr;
  if (adr[0]>'z'||adr[0]<'a') return "*UNGÜLTIGER NAME: "+adr+" *";


  s=adr[0..0];
  LoadFile(s);
  
  if (!member(forwarddata[s],adr)) return adr;

  /* Achtung, ab hier wechselt Variable s z.T. ihre Bedeutung */

  if (player=find_player(adr)) 
    s=player->QueryProp(P_MAILADDR);
  else {
    /* Gibts den Spieler? */
    if (file_size("/save/"+s+"/"+adr+".o")<=0) return 0;
    /* hol die Daten */
    restore_object("/save/"+s+"/"+adr);
    s=properties[P_MAILADDR];
    properties=0;
  }    

  s=lower_case(s);
  if (!s || !strlen(s) ||
      s=="none")
    return adr;  /* keine sinnvolle Zieladresse: nicht umsetzen */

  return s;
}


string AddForward(string name) {
  string s;

  name=lower_case(name);
  s=name[0..0];

//  if (!Security(name)) 
//    return "Nachsendeauftrag abgelehnt, bitte Erzmagier benachrichtigen.";

  LoadFile(s);
  
  if (member(forwarddata[s],name))
    return "Nachsendeauftrag war bereits gestellt worden.";

  forwarddata[s]+=([name]);
  FWSAVESERV->SaveData(s,forwarddata[s]);

  write_file(LOG,ctime()+": ADD "+name+"\n");
  
  return "Nachsendeauftrag angenommen. Die Adresse lautet:\n  "+
    QueryForward(name)+
      "\nFalls diese falsch ist, bitte augenblicklich die email ändern!\n";

}


string DeleteForward(string name) {
  string s;

  name=lower_case(name);
  s=name[0..0];

//  if (!Security(name)) 
//    return "Loeschen des Nachsendeauftrags ist nicht erlaubt,\n"
//           "  bitte Erzmagier benachrichtigen.";

  LoadFile(s);
  
  if (!member(forwarddata[s],name))
    return "Es war kein Nachsendeauftrag gestellt worden.";

  forwarddata[s]-=([name]);
  FWSAVESERV->SaveData(s,forwarddata[s]);
  
  write_file(LOG,ctime()+": DEL "+name+"\n");

  return "Nachsendeauftrag gelöscht, Post wird wieder direkt ausgeliefert.\n";
}



static int Security(string name) {
  if (geteuid(this_interactive())==name) return 1;
  if (geteuid()==geteuid(previous_object())) return 2;
  if (member( ({ROOTID,MAILID,"debugger"}),
	     geteuid(previous_object())) >=0 ) return 3;
  return 0;
}


mixed QueryForwards() {
  if (Security(" N I X "))
    return forwarddata;
  else
    return 0;
}

