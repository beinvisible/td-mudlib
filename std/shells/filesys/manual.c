// SilberLand MUDlib
//
// MANUAL.C -- manual client
//
// $Date: 1997/03/13 02:10:41 $                                            
/* $Revision: 1.3 $             
 * $Log: manual.c,v $
 * Revision 1.3  1997/03/13 02:10:41  Woody
 * prepared for revision control
 *
 * Revision 1.2  1994/09/22  16:19:40  Hate
 * *** empty log message ***
 *
 * Revision 1.2  1994/09/22  16:19:40  Hate
 * *** empty log message ***
 *
 * Revision 1.1  1994/07/14  15:16:37  Jof
 * Initial revision
 *
 */

#include <defines.h>
#include <config.h>
#include <daemon.h>
#include <ansi.h>
#include <properties.h>

#define ANSI_TERMINALS ({"vt100", "ansi"})
private static mapping refs = ([]);
private static string *back = ({});
private static string G_root = "";

private void split(string f, string p, string *dir, string *page)
{
  if(file_size(p+"/"+f) == -2) dir += ({ p+"/"+f });
  else if(f[<5..] == ".html") page += ({ f[0..<6], p+"/"+f });
}

private string SearchPage(string path, string word)
{
  string *dir, *page;
  int i;
  dir = ({}); page = ({});
  map(get_dir(path+"/*")-({".",".."}),#'split,path,&dir,&page);
  if((i = member(page, word)) != -1) return page[i+1];
  while(sizeof(dir))
  {
    string file;
    if(file = SearchPage(dir[0], word)) return file;
    dir = dir[1..];
  }
  return "";
}

static void Prompt()
{
  if(!sizeof(refs[H_HREF])) return;
  if(member(ANSI_TERMINALS, PL->QueryProp(P_TTY)) == -1)
    printf("[%s] ", 
           break_string(implode(m_indices(refs[H_HREF]), "/"), 78)[0..<2]);
  else printf("[Wähle "+ANSI_UNDERL+"Verweis"+ANSI_NORMAL+":]");
  input_to("Manual");
}

public int Manual(string word, string root)
{
  string file, page;
  string *keys;
  mapping tmp;
  int ANSI;

  if(!word || !stringp(word) || !sizeof(word)) return 1;
  if(stringp(root)) G_root = root;
  ANSI = member(ANSI_TERMINALS, PL->QueryProp(P_TTY)) != -1;
  switch(word)
  {
  case "<":
    if(sizeof(back) > 1) file = back[<2];
    break;
  case "?":
    printf("Interaktive Hilfe!\n"
          +"Kommandos: < -- zur letzten Seite zurück blättern\n"
          +"           ? -- Diese Hilfe anzeigen\n"
          +"Durch Eingabe eines der in der Referenzliste angezeigten (oder\n"
          +"wenn man ein ANSI Code fähiges Terminal hat, unterstrichenen)\n"
          +"angezeigten Wörter, oder eines Teiles davon, kann man direkt zu\n"
          +"der entsprechenden Hilfeseite springen. Es ist auch möglich ein\n"
          +"nicht angegebenes Wort zu benutzen, wonach dann gesucht wird.\n"
          +"Folgende Hilfen sind sofort zu erreichen:\n");
    Prompt();
    return 1;
  default:
    if(mappingp(refs[H_HREF]) &&
       sizeof(keys = regexp(m_indices(refs[H_HREF]), "^"+word+".*")))
      word = keys[0];
    if(!mappingp(refs[H_HREF]) || !file = refs[H_HREF][word]) 
      file = SearchPage(G_root, word);
    else file = MASTER->_get_path("/"+implode(explode(back[<1],"/")[0..<2],"/")
                                 +"/"+file);
    break;
  }
  tmp = refs; refs = ([]);
  if(!stringp(page = HTMLD->ReadPage(file, &refs, ANSI)))
  {
    printf("Keine Hilfe fuer '%s' gefunden!\n", word);
    refs = tmp;
    if(ANSI) 
      printf("Verweise: %s\n",
             break_string(implode(m_indices(refs[H_HREF]), " "), 78)[0..<2]);
    Prompt();
    return 1;
  }
  if(sizeof(back) >= 10) back = back[1..];
  back += ({ file });
  printf("### %s ###\n", stringp(refs[H_TITLE])?refs[H_TITLE]:file);
  PL->More(page, 0, #'Prompt);
  return 1;
}
