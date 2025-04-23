// SilberLand MUDlib
//
// std/util/pager.c -- generic pager
//
// $Date: 1997/11/05 16:27:04 $
/* $Revision: 2.2 $
 * $Log: pager.c,v $
 * Revision 2.2  1997/11/05 16:27:04  Woody
 * P_MORE_FLAGS des Spielers auswerten (E_ABS Mode)
 *
 * Revision 2.1  1997/09/17 14:44:36  Woody
 * Bugfix fuer zeilen==0
 *
 * Revision 2.0  1997/03/13 02:41:33  Woody
 * prepared for revision control
 *
 * Revision 1.8  1995/05/08  21:39:30  Hate
 * added E_CAT
 *
 * Revision 1.7  1995/05/04  15:13:38  Hate
 * *** empty log message ***
 *
 * Revision 1.4  1995/05/03  15:38:32  Hate
 * added flags to More(), now inly E_PROMPT stops at end of file
 *
 * Revision 1.3  1995/05/03  14:44:54  Hate
 * there's always a bug in fread()
 * but we fixed it!
 *
 * Revision 1.2  1995/05/03  13:03:00  Jof
 * fixed command() bug (must be eval_command())
 *
 * Revision 1.2  1995/05/03  10:48:47  leo
 * fixed bug in file read
 *
 * Revision 1.1  1995/05/02  19:19:05  mud
 * Initial revision
 *
 */

#pragma strong_types

inherit "/std/util/input";

#include <pager.h>
#include "/std/sys_debug.h"

string prompt(mixed pinfo, string add)
{
  return sprintf(MSG_PROMPT+"%s", stringp(add)?add:"");
}

string fread(mixed pinfo, int begin, int c)
{
  if(begin > pinfo[MAXL] || begin < 1) return 0;
  if(pinfo[FILE]) return read_file(pinfo[TEXT], begin, c);
  else
  {
    int start, end, l, x;
#ifdef O_SPEED
# ifdef O_SPEED_UP
    if(member(pinfo[JUNK], begin)) 
    {
      start = pinfo[JUNK][begin];
      if(!(end = pinfo[JUNK][begin+c]))
        end = sizeof(pinfo[TEXT]);
      else end--;
      return pinfo[TEXT][start..end];
    }
    return 0;
# else 
    end = pinfo[JUNK][begin / JUNKSIZE];
# endif
#endif
#ifndef O_SPEED_UP
    l = begin + c; start = -1;
# ifdef SPEED_UP
    l -= begin / JUNKSIZE;
# endif
    while(l && (end = strstr(pinfo[TEXT], "\n", end+1)) > -1)
    {
      if(l == c) start = end;
      l--;
    }
    if(start == -1) return 0;
    return pinfo[TEXT][start..(end < 0 ? sizeof(pinfo[TEXT]) : end)];
#endif
  }
}

mixed call(mixed ctrl, mixed ctrlargs)
{
  if(!ctrl || (stringp(ctrl) && !(ctrl = symbol_function(ctrl)))) return 0;
  if(!pointerp(ctrlargs)) return funcall(ctrl);
  return apply(ctrl, ctrlargs);
}

 
varargs int eval_command(mixed in_, mixed pinfo)
{
  string cprompt, tmp;
  cprompt = "";
  if(in_ == "q" || in_ == "x")
  {
    if(closurep(pinfo[CTRL])) apply(pinfo[CTRL], pinfo[CARG]);
    return 1;
  }
  else
    if(!in_ || (stringp(in_) && !sizeof(in_))) pinfo[CURL] += pinfo[PAGE];
    else if(in_ != -1) return 0;

  if(pinfo[CURL] >= pinfo[MAXL]) 
    return (call(pinfo[CTRL], pinfo[CARG]), 1); 
  if(pinfo[CURL] <= 0) pinfo[CURL] = 1;
  if(pinfo[CURL] == 1) cprompt = MSG_TOP;
  if(pinfo[CURL] + pinfo[PAGE] >= pinfo[MAXL]) cprompt = MSG_BOTTOM;
  if(!tmp = fread(pinfo, pinfo[CURL], pinfo[PAGE]))
    return (write(MSG_OOPS+"\n"), 0);
  write(tmp);
  if(!(pinfo[FLAG] & E_PROMPT) &&
     pinfo[CURL] + pinfo[PAGE] >= pinfo[MAXL])
    return (call(pinfo[CTRL], pinfo[CARG]), 1);
  if(!(pinfo[FLAG] & E_CAT) &&
     (pinfo[MAXL] > pinfo[PAGE] || (pinfo[FLAG] & E_PROMPT)))
    input(#'prompt, ({ pinfo, cprompt }), #'eval_command, ({ pinfo }));
  return 1;
}

varargs public void More(string txt, int file,
			 mixed ctrl, mixed ctrlargs,
			 int flags)
{
  int j;
  mixed tmp, pinfo;
  if(!txt) return call(ctrl, ctrlargs);
  //         TEXT, FILE, CURL, MAXL, REGX, PAGE, CTRL, CARG
  pinfo = ({ txt, file, 1, 1, 20, 0, ctrl, ctrlargs, flags });
  if(!pinfo[FILE] && pinfo[TEXT][<1] != '\n') pinfo[TEXT] += "\n";
#ifdef O_SPEED
  pinfo += ({ ([1:0]) });
#endif
  if(!pinfo[FILE]) 
    while((j = strstr(pinfo[TEXT], "\n", j+1)) != -1)
#ifdef O_SPEED
# ifdef O_SPEED_UP
      pinfo[JUNK][++pinfo[MAXL]] = j+1;
# else
      if(!(++pinfo[MAXL] % JUNKSIZE)) 
        pinfo[JUNK][pinfo[MAXL]] = j+1;
# endif
#else
     pinfo[MAXL]++;
#endif
  else
    while(tmp = read_file(pinfo[TEXT], pinfo[MAXL], MAX_LINE_READ))
      pinfo[MAXL] += sizeof(Explode(tmp, "\n"))+1;
  pinfo[PAGE] = PAGELENGTH;
  if(!pinfo[PAGE])
  {
    // pinfo[PAGE] = 40;
    pinfo[FLAG] |= E_CAT;
  }
  if ((this_interactive() && (j=this_interactive()->QueryProp(P_MORE_FLAGS)))
      || (this_player() && interactive(this_player()) &&
       (j=this_player()->QueryProp(P_MORE_FLAGS))))
    pinfo[FLAG] |= j;

  pinfo[CURL] = 1;
  eval_command(-1, pinfo);
  return;
}
