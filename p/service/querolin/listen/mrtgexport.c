/*
 * Gibt die anzahl der anwesenden Leute fuer die MRTG
 * Auswertung aus.
 *
 * Querolin@Tamedhon 29.11.2001
 *
 */

/*
    Eingetragen in: 
*/
 

#pragma strong_types

#include "def.h"
#include <properties.h>

#define ADB(x)          if(find_player("querolin")) \
                            tell_object(find_player("querolin"), \
                            break_string(x, 79, "{ abg } "));

#define PLAYERCOUNT "/p/service/querolin/playercount.c"

void _pc_neuertag()
{
    PLAYERCOUNT->_neuertag();
}

int _output()
{
  int i = 0;        
  string ulog = "";
  string dat;
 
  dat=sprintf("%s",ctime(time()));
  dat=extract(dat, 15, 15);

  if (sizeof(users())>i) 
  {
    i=sizeof(users()); 
    ulog = sprintf("%d",i);
    rm("/p/service/querolin/listen/currentusers");
    write_file("/p/service/querolin/listen/currentusers", ulog);
  }

  call_out("_output", 60);

  if (extract(ctime(), 10, 15)=="23:59")
  {
    //PLAYERCOUNT->_neuertag();
    call_out("_pc_neuertag", 61);
  }

  if ((dat=="1") || (dat=="6"))
  { 
     i=sizeof(users());
     rm("/p/service/querolin/listen/currentusers");
     write_file("/p/service/querolin/listen/currentusers", ulog);
  } 
//  ADB("mrtg: "+to_string(ulog)+" Users.");
  return 1;
}

void create()
{
    seteuid(getuid());
    ADB("mrtgexport load...");
    call_out("_output", 1);
}
