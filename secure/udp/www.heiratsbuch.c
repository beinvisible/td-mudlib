/* Tamedhon MUDlib
 *
 * /secure/udp/www.topliste.c -- WWW Questliste
 *
 * $Date: 2006/07/16 13:00:00 $
 * $Revision: 1.1 $
 * $Log: www.questliste.c,v $
 *
 */


#pragma strong_types
#pragma combine_strings

#include <properties.h>
#include <www.h>

string Request(mapping cmds)
{
  string text, seiteninhalt, outtext, datum, uhrzeit, part1, part2, zeuge, back, forward, first, last;
  string *seite;
  int page;
 
  if(!sizeof(cmds) || !stringp(cmds[PAGE]))
  {
    page=0;
  }
  else 
  {
    page=to_int(cmds[PAGE]);
  }

  text=read_file("/log/HEIRAT");

  seite=explode(text, "\n");

  if (page < 0) page=0;

  if (page > (sizeof(seite)-1)) { page=sizeof(seite)-1; }

  seiteninhalt=seite[page];
  // Zwei Blank beim Datum stoeren bei der Zerteilung
  seiteninhalt=implode(explode(seiteninhalt, "  "), " ");


  outtext="\
     _________________________   _________________________     <br>\
 .-/|          ~~**~~         \\ /         ~~**~~          |\\-. <br>\
 ||||                          :                          |||| <br>\
 ||||                          :                          |||| <br>\
 ||||            Am            :        Es haben          |||| <br>\
 ||||                          :                          |||| <br>\
 ||||    <b><i>%17s</i></b>     :      <b><i>%|12s</i></b>        |||| <br>\
 ||||                          :                          |||| <br>\
 ||||            um            :           und            |||| <br>\
 ||||                          :                          |||| <br>\
 ||||         <b><i>%8s</i></b>         :      <b><i>%|12s</i></b>        |||| <br>\
 ||||                          :                          |||| <br>\
 ||||      Trauzeuge war:      :        geheiratet        |||| <br>\
 ||||                          :                          |||| <br>\
 ||||       <b><i>%|12s</i></b>       :                          |||| <br>\
 ||||                          :                          |||| <br>\
 ||||                          :                          |||| <br>\
 |||| %2s %2s             Seite  :  %2s                %2s %2s |||| <br>\
 ||||_________________________ : _________________________|||| <br>\
 ||/==========================\\:/==========================\\|| <br>\
 `---------------------------~___~--------------------------'' <br>\
<br>";

  datum=explode(seiteninhalt," ")[0];
  datum+=" "+explode(seiteninhalt," ")[1];
  datum+=" "+explode(seiteninhalt," ")[2];
  datum+=" "+extract(explode(seiteninhalt," ")[3], 0, 3);

  uhrzeit=explode(seiteninhalt," ")[4];

  part1=explode(seiteninhalt," ")[5];
  part2=explode(seiteninhalt," ")[7];
  zeuge=explode(seiteninhalt," ")[sizeof(explode(seiteninhalt," "))-1];

  first="<a href=\"/cgi-bin/mudwww?REQ=heiratsbuch&PAGE=0\"><<</a>";
  last="<a href=\"/cgi-bin/mudwww?REQ=heiratsbuch&PAGE="+
       (sizeof(seite) -1 ) + "\">>></a>";

  back="<a href=\"/cgi-bin/mudwww?REQ=heiratsbuch&PAGE="+(page-1)+"\"><-</a>";
  if (page==0) { first="  "; back="  "; }
  forward="<a href=\"/cgi-bin/mudwww?REQ=heiratsbuch&PAGE="+(page+1)+"\">-></a>";
  if (page == sizeof(seite)-1) { last="  "; forward="  "; }


  return "<font size=+2><b>Das Trauungsbuch</b></font><PRE>"+
         sprintf(outtext,
                 datum,
                 part1,
                 uhrzeit,
                 part2,
                 zeuge,
                 first,
                 back,
                 to_string(page + 1),
                 forward,
                 last
                )+
         "</PRE>";
}
