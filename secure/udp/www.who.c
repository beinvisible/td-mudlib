/* Tamedhon MUDlib
 *
 * /secure/udp/www.who.c -- WWW who
 *
 * $Date: 1997/07/01 16:45:08 $
 * $Revision: 1.4 $
 * $Log: www.who.c,v $
 * Revision 1.5  2004/10/10 18:30:00  Querolin
 * Redesign
 *
 * Revision 1.4  1997/07/01 16:45:08  Woody
 * Farbwert fuer Seher nochmal korrigiert...
 *
 * Revision 1.3  1997/07/01 16:12:39  Woody
 * Bugfix *argl* hatte break vergessen...
 *
 * Revision 1.2  1997/07/01 16:07:15  Woody
 * ueberarbeitet, Tabellenform
 *
 * Revision 1.1  1997/03/16 00:51:55  Woody
 * prepared for revision control
 *
 */

// MorgenGrauen MUDlib Contribution

#pragma strong_types
#pragma combine_strings

#include <properties.h>
#include <www.h>

string MakeLink(mixed entry)
{
  string nm;
  nm = (string)getuid(entry[0]);

  return 
    "<TR>"+
    "<TD ALIGN=CENTER>"+entry[1]+"</TD>"
    "<TD>"+entry[2]+"<A HREF=\""+MUDWWW+"?"+REQ+"="+R_FINGER+"&"+USER+"="+nm+"\">"+capitalize(nm)+"</B></A>"+entry[3]+"</TD>"+
    "<TD ALIGN=LEFT>"+entry[4]+", "+entry[5]+"</TD>"+
    "<TD ALIGN=LEFT>"+entry[6]+"</TD>"+
    "</TR>";
}

string Request(mapping cmds)
{
  string *who;
  if (!sizeof(cmds)) return ERROR("Anfrage ung&uuml;ltig!");
  who = map(WHO, #'MakeLink);
  return "<CENTER><TABLE BORDER=0 cellspacing=0 cellpadding=0 WIDTH=90%>"
//       + "<TR>"
       + "<TR>"
       + "<TH ALIGN=CENTER>Status</TH>"
       + "<TH ALIGN=LEFT>Voller Name</TH>"
       + "<TH ALIGN=LEFT>Rasse, Gesinnung</TH>"
       + "<TH ALIGN=LEFT>eingeloggt von</TH>"
       + "</TR><TR><TD><br></TD></TR>\n"
       + implode(who, "\n")+"</TABLE></CENTER>";
}
