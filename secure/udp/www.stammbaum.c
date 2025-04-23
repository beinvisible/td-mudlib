/* Tamedhon MUDlib
 *
 * /secure/udp/www.stammbaum.c -- WWW Stammbaum
 *
 * $Date: 2006/07/22 13:00:00 $
 * $Revision: 1.1 $
 * $Log: www.stammbaum.c,v $
 *
 */


#include <properties.h>
#include <www.h>
#include <parentmaster.h>

__list_children(int ebene, string elter)
{
   string* kinder;
   string text;
   int i,j;

   kinder=PARENTMASTER->GetChildren(elter);
   text="";

   if (!sizeof(kinder)) return text;

   for(i=0;i<sizeof(kinder);i++) {
     for(j=0;j<=(ebene*12);j++) {
     text+="&nbsp;";
     }
     text+="<a href=\"/cgi-bin/mudwww?REQ=finger&USER=";
     text+=lower_case(kinder[i])+"\">"+capitalize(kinder[i])+"</a><br>";
     text+=this_object()->__list_children(ebene+1, lower_case(kinder[i]));
   }
   return text;
}

string Request()
{
  string text,elter;
  string* kinder;

  text="<table><TR><TD><PRE>";
  text+="<center>Stammbaum der Magier<br></center><br>";
  elter="serii";
  text+="<a href=\"/cgi-bin/mudwww?REQ=finger&USER=serii\">Serii</a><br>";
  text+=this_object()->__list_children(1, elter);

  elter="querolin";
  text+="<br> <br> <br><a href=\"/cgi-bin/mudwww?REQ=finger&USER=querolin\">Querolin</a><br>";
  text+=this_object()->__list_children(1, elter);

  return text+"</PRE></TD></TR></table>";
}
