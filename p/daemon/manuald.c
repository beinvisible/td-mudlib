// SilberLand MUDlib
//
// /p/daemon/manuald.c -- searches in manpages and caches
//
// $Date: 1997/03/16 02:30:09 $
/* $Revision: 1.2 $
 * $Log: manuald.c,v $
 * Revision 1.2  1997/03/16 02:30:09  Woody
 * prepared for revision control
 *
 * Revision 1.1  1995/03/31  13:30:33  Hate
 * Initial revision
 *
 */

#pragma strong_types

#include "/std/sys_debug.h"
#include "html.h"
#include <ansi.h>

private static mapping cache;

void create()
{
  cache = ([]);
}

private void split(string f, string p, string *dir, string *page)
{
  if(file_size(p+"/"+f) == -2) dir += ({ p+"/"+f });
  else if(f[<5..] == ".html") page += ({ f[0..<6], p+"/"+f });
}

private mixed SearchPage(string path, string word)
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
}

varargs mixed ReadPage(string what, string section)
{
  if(!what) return -1;
  // if(member(what, cache))
  // ...
}

