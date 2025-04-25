/* Tamedhon MUDlib
 *
 * /secure/udp/www.news.c -- WWW frontend for reading mud news
 *
 * $Date: 2004/10/10 18:00:00 $
 * $Revision: 1.5 $
 * $Log: www.news.c,v $
 * Revision 1.6  2004/10/10 18:00:00  Querolin
 * Redesign
 *
 * Revision 1.5  1997/07/19 11:52:19  Woody
 * Bugfix: & -> &amp; Ersetzung an 1. Stelle der Ersetzungen...
 *
 * Revision 1.4  1997/07/02 09:48:39  Woody
 * Kleinere Optimierungen und Verschoenerungen...
 *
 * Revision 1.3  1997/07/01 22:43:00  Woody
 * Neue Threadverwaltung, buggy headerline remove raus
 *
 * Revision 1.2  1997/07/01 19:42:40  Woody
 * Ueberarbeitet, Links nach oben etc.
 *
 * Revision 1.1  1997/03/16 00:44:43  Woody
 * prepared for revision control
 *
 */

// MorgenGrauen MUDlib Contribution

#pragma strong_types
#pragma combine_strings

#include <www.h>

#define NEWSD	"/secure/news"

#define N_GROUP 0
#define N_AUTHOR 1
#define N_TID 2
#define N_DATE 3
#define N_TITLE 4
#define N_ARTICLE 5

varargs private string url(string group, string article)
{
  return "\""+MUDWWW+"?"+REQ+"="+R_NEWS
    +(group?"&"+GROUP+"="+group:"")
    +(article?"&"+ARTICLE+"="+article:"")+"\"";
}

varargs private string make_link(string text, string group, string article)
{
  if(!text || !sizeof(text)) text = "-Unbenannt-";
  return "<A style=\"text-decoration:none; font-weight:bold; \" HREF="+url(group, article)+">"+text+"</A>";
}

string GroupList()
{
  string *list, ret;
  int i, t;

  list = NEWSD->GetGroups();
  for (i = 0, ret = "<CENTER><table width=\"60%\" border=\"0\" "+
    "cellspacing=\"0\">"; i < sizeof(list); i++)
  ret+=  "<tr BG=\"#000000\" ONMOUSEOVER=\"this.bgColor='#444444'\" "+
         "ONMOUSEOUT=\"this.bgColor='transparent'\"><td width=\"60%\">" +
         make_link(list[i],list[i]+":"+t) + "</td><td width=\"20%\">" +
         sizeof(NEWSD->GetNotes(list[i])) + " Artikel</td>" +
         "<td align=\"right\" width=\"20%\">" +
	 dtime(t = NEWSD->GetNewsTime(list[i]))[5..16]+ "</td></tr>\n";

      
  return "<br><CENTER><font size=+3>Rubriken&uuml;bersicht</font></center><br>"
        +"<I><B><Center>Insgesamt "+sizeof(list)+" Gruppen<br> "
        +"letzte &Auml;nderung: "+dtime(NEWSD->GetNewsTime())[5..16]+" um "
        +dtime(NEWSD->GetNewsTime())[19..23]+"</center></B></I><P>"
        +"<PRE>" + ret +"</table></CENTER></PRE>";
}

#define THREAD(a) ("~#! rn="+(a[N_AUTHOR])+" rt="+(a[N_DATE])+ \
                   " rg="+(a[N_GROUP]))
#define TEXPR   "rn=[^\n ]*|rt=[^\n ]*|rg=[^\n ]*"


private mixed tinfo(mixed article)
{
  mixed tmp, info;
  string rn, rt, rg, tid;
  int j, k;

  tmp = regexp(explode(article[N_ARTICLE], "\n"), "^~#!");
  for(j = sizeof(tmp); j--;) {
    mixed line;
    line = explode(tmp[j], " ");
    for(k = sizeof(line); k--;) {
      if(line[k][0..1]=="rn") rn = line[k][3..];
      if(line[k][0..1]=="rt") rt = line[k][3..];
      if(line[k][0..1]=="rg") rg = line[k][3..];
      if(line[k][0..2]=="tid") tid = line[k][4..];
    }
  }
  if(!tid) tid = ""+article[N_DATE];
  return ({ rn, rt, rg, tid });
}

#define RN      0
#define RT      1
#define RG      2
#define TID     3

private int thread(mixed article, int i, mapping t)
{
  mixed info;
  info = tinfo(article);

  if(info[TID]) {
    t[info[TID]]++;
/*
    t[info[TID], 1] = sprintf("[%-12s %-6s]%-3s %s\n",
                              article[N_AUTHOR]+":",
                              dtime(article[N_DATE])[5..11],
                              t[info[TID]] ? "+"+t[info[TID]] : " - ",
                              make_link(article[N_TITLE],
                                        article[N_GROUP], (string)i))
      + (t[info[TID], 1] ? t[info[TID], 1] : "");
*/
    t[info[TID], 1] =
         "<tr BG=\"#000000\" ONMOUSEOVER=\"this.bgColor='#444444'\" "+
         "ONMOUSEOUT=\"this.bgColor='transparent'\">" +
         "<td width=\"50%\">" +
         make_link(({string})article[N_TITLE] ,
              article[N_GROUP], (string)i)+
         "</td><td width=\"8%\"> "+ (t[info[TID]] ? "+"+t[info[TID]] : " - ") +
         "</td><td width=\"20%\"> " + article[N_AUTHOR] + "</td>"+
         "<td width=\"15%\" align=\"right\"> " +
         dtime(article[N_DATE])[5..16] +
         "</td><td width=\"7%\">&nbsp;" + dtime(article[N_DATE])[19..23] +
         "</td></tr>\n" 
	  + (t[info[TID], 1] ? t[info[TID], 1] : "");

    t[info[TID], 2] = info;
    if(article[N_DATE] > to_int(t[info[TID], 3]))
      t[info[TID], 3] = ""+article[N_DATE];
    return 1;
  }
  return 0;
}

private int expired(mixed *list, int i)
{
  mixed info;
  info = tinfo(list[i]);
  for(i--; i >= 0; i--)
    if(list[i][N_AUTHOR] == info[RN] &&
       list[i][N_DATE] == to_int(info[RT]))
      return 0;
  return 1;
}

string ArticleList(string group)
{
  string *list, ret;
  mapping t;
  int i;
  
  if(sizeof(explode(group, ":")) == 0)
    return 0;

  list = NEWSD->GetNotes(group = explode(group, ":")[0]);
  
  if(!list || intp(list) || list == 0)
    return 0;
  
  t = m_allocate(0,4);
  for (i = sizeof(list)-1, ret = ""; i >= 0; i--)
  {
    if(!thread(list[i], i, t) || expired(list, i))
    {
      mixed ttmp, tid;
      ttmp = t[tid = tinfo(list[i])[TID]] - 1;
      ret = "<tr BG=\"#000000\" ONMOUSEOVER=\"this.bgColor='#444444'\" "+
         "ONMOUSEOUT=\"this.bgColor='transparent'\"><td width=\"50%\">" +
         make_link((string)list[i][N_TITLE] ,  
              group, (string)i+":"+t[tid, 3])+ 
         "</td><td width=\"8%\">" + (ttmp > 0 ? "+ "+ttmp : " - ") +
         "</td><td width=\"20%\"> " + list[i][N_AUTHOR] + "</td>"+
         "<td width=\"15%\" align=\"right\"> " + 
         dtime(list[i][N_DATE])[5..16] + 
         "</td><td width=\"7%\">&nbsp;" + dtime(list[i][N_DATE])[19..23] +
         "</td></tr>\n"+ ret ;
    }
  }

  return "<center>"+make_link("[Gruppenübersicht]")
        +"<center><br><br><font size=+3>'"+group+"'</font><br><br>"
	+"<I><B>"+sizeof(list)+" Artikel<br>letzte &Auml;nderung am "
//        +dtime(NEWSD->GetNewsTime(group))+"</B></I><P>"
	+dtime(NEWSD->GetNewsTime(group))[5..16]+" um "
        +dtime(NEWSD->GetNewsTime())[19..23]+"</B></I><P>"
        +"<PRE>\n<center><table width=\"80%\" border=\"0\" cellspacing=\"0\">" 
        + ret + "</table></PRE><center>";
}

private varargs string Message(string group, mixed article)
{
  mixed text, tmp, next, prev, info;
  string art, *tmp2;
  mapping t;
  int i;

  if (!article) article = 0;
  else article = to_int(explode(article, ":")[0]);

  if(sizeof(explode(group, ":")) == 0 || sizeof(NEWSD->GetNotes(group = explode(group, ":")[0])) == 0 || sizeof(NEWSD->GetNotes(group = explode(group, ":")[0])) <= article)
    return 0;
  
  text = (tmp = NEWSD->GetNotes(group = explode(group, ":")[0]))[article];

  t = m_allocate(0,4);
  for(i = sizeof(tmp)-1; i > article; i--)
    thread(tmp[i], i, t);
  next = "[Nächster Artikel]";
  prev = "[Voriger Artikel]";

  art = implode((tmp2=efun::explode(text[N_ARTICLE],"\n"))
                - regexp(tmp2, "^~#!"), "\n");

  info = tinfo(text);
  art = regreplace(art, "&", "\\&amp;", 1);
  art = regreplace(art, "<", "\\&lt;", 1);
  art = regreplace(art, ">", "\\&gt;", 1);

  return "<center>"
       + (article < sizeof(tmp)-1 ? make_link(next, group, article+1) : next)
       + (article ? make_link(prev, group, (string)(article-1)) : prev)
       + make_link("[Artikelliste]", group)
       + make_link("[Gruppenübersicht]")
/*
       + "<P><TABLE BORDER=1 WIDTH=480><TR><TD><B>Gruppe/Artikel:</B></TD><TD>"
       + capitalize(group) + ", Artikel "+(article+1)+"</TD></TR>"
       + "<TR><TD><B>Autor:</B></TD><TD>"+text[N_AUTHOR]+"</TD></TR>"
       + "<TR><TD><B>Datum:</B></TD><TD>"+dtime(text[N_DATE])+"</TD></TR>"
       + "<TR><TD><B>Subject:</B></TD><TD>"+text[N_TITLE]+"</TD></TR>"
       + (info[RN] ? ("<TR><TD><B>Antwort auf:</B></TD><TD>"+info[RN]+
                      (expired(tmp, article) ? " (verfallen)" : "")+
                      "</TD></TR>") : "")
       + "</TABLE><P>\n"
*/
       + "<br><br><br>"
       + "<table width=\"80%\" cellspacing=\"0\" ><tr><td>" 
       + "<font size=+2>"
       + make_link( capitalize(group) , group)
       + "</font> - Artikel "+ (article+1) +"<br>"          
       + "<hr>"
       + "<B> <font size=+1>" + 
         "<a style=\"text-decoration:none; font-weight:bold;\" " +
         "href=\"/cgi-bin/mudwww?REQ=finger&USER=" + text[N_AUTHOR] + 
         "\">" + text[N_AUTHOR] + "</a></B> - " + 
         dtime(text[N_DATE])[5..16] + " " + 
         dtime(text[N_DATE])[19..23] + "</font><br>" 
       + (info[RN] ? ("[ Antwort auf: <a style=\"text-decoration:none; " +
         "font-weight:bold;\" href=\"/cgi-bin/mudwww?REQ=finger&USER=" + 
         info[RN] + "\">" + info[RN] + "</a>" + 
              (expired(tmp, article) ? " (verfallen) ]<br>" : " ]<br>")) : "")
       + "<br>"
       + "<B><I><CENTER><font size=+2>'" + text[N_TITLE] + 
         "'</CENTER></font></I></B><br>"
       + "<br>"
       + "<PRE>" + art + "</PRE>\n"
       + (t[info[TID]] ? "<HR><I>Weitere Artikel zu diesem Thema:</I>"
          "<PRE><center><table widht=\"80%\" border=\"0\" cellspacing=\"0\">" + 
          t[info[TID], 1] + "</table></center></PRE><HR>" : "")
       + "</td></tr></table>";

}

string Request(mapping cmds)
{
  string text;
  if(!cmds[GROUP]) text = GroupList();
  else
    if(!cmds[ARTICLE]) text = ArticleList(cmds[GROUP]);
    else text = Message(cmds[GROUP], cmds[ARTICLE]);

  return "<H2><CENTER>"+MUDNAME+" Presse</CENTER></H2>"
        +text;
}
