// SilberLand MUDlib
//
// std/timevars.c -- timed variables handler
//
// $Date: 1997/03/13 03:17:45 $
/* $Revision: 1.1 $
 * $Log: timevars.c,v $
 * Revision 1.1  1997/03/13 03:17:45  Woody
 * prepared for revision control
 *
 * 21.02.1996	22:50	Mupfel	erste Version
 */

// AnderLand MUDlib
//
// TIMEVARS.C -- timed variables handler
//
// Funktionen:
// void AddTimeVar(string name, int duration);
// Meldet eine neue Zeitvariable mit Namen 'name' und Ablaufzeit
// 'duration' beim Modul an.
// int ReadTimeVar(string name);
// Gibt 1 zurueck, wenn die Zeit von 'name' abgelaufen ist (und
// startet den virtuellen Timer neu), sonst 0.
//

#include <timevars.h>

private static mapping timevar;


void InitTimeVars()
{
  timevar = ([0:0;0]);
}

void AddTimeVar(string name, int duration)
{
  if (!mappingp(timevar)) InitTimeVars();
  timevar[name,0] = time()-duration;  // damit das erste 'ReadTimeVar'
  timevar[name,1] = duration;         // nicht fehlschlaegt
}

int ReadTimeVar(string name)
{
  if (!mappingp(timevar))
    raise_error("timevars::ReadTimeVar: variable not defined");
  if (time()-timevar[name,0] < timevar[name,1])
    return 0;
  else
  {
    timevar[name,0] = time();
    return 1;
  }
}
