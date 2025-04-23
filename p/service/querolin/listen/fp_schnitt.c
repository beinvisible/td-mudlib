
#pragma strong_types

#include "def.h"
#include <properties.h>
#include <wizlevels.h>
#include <moving.h>
#include <exploration.h>

#define DUMPFILE	"/p/service/querolin/listen/fpaverage.txt"
#define ADB(x)          if(find_player("querolin")) \
                            tell_object(find_player("querolin"), \
                            break_string(x, 79, "{ abg } "));

int average, anzahl;

void create()
{
  seteuid(getuid());
  ADB("FP-Schnitt Auswertung geladen.");
}

void reset()
{
   this_object()->RecalcAverage();
   this_object()->Dump();
}

void Dump()
{
  write_file(DUMPFILE,":"+dtime(time())+" Average: "+average+" bei "+anzahl+
    " Leuten\n");
}

nomask void RecalcAverage()
{
  int i, avg;
  object *u;

//  while (remove_call_out ("RecalcAverage") >= 0);
//  call_out ("RecalcAverage", TIME_UPD_AVG);

  avg=0;
  u=users();
  if (!sizeof(u)) return 0;
  for (i=sizeof(u)-1; i>=0; i--)
  {
    avg += u[i]->QueryProp(P_EP_NUM);
  }
  avg /= sizeof(u);
  anzahl = sizeof(u);
  average = avg;
}

