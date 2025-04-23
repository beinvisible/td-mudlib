// SilberLand MUDlib
//
// /p/daemon/townd.c -- Town Daemon
//
// $Date: 1997/03/16 02:36:12 $
/* $Revision: 1.2 $
 * $Log: townd.c,v $
 * Revision 1.2  1997/03/16 02:36:12  Woody
 * prepared for revision control
 *
 * Revision 1.1  1996/07/31 22:08:11  Hadra
 * Initial revision
 *
 */

// This file is part of Avalon Mudlib.
// ----------------------------------------------------------------
// File:        /apps/town.c
// Description: IP-Nummern-Master
// Author:      IceT (18.2.96)
/*
 * Mit freundlicher Genehmigung von IceT fuers AnderLand adaptiert
 * 30. Juni 96  Hadra
 */

#pragma strong_types

#include <wizlevels.h>
#include <daemon.h>

#define LOGGING

mapping locations;
mapping tlevel;
mapping country;

void create() {
    seteuid(getuid());
    locations=(["127.0.0.1": LOCALHOST, "212.132.115.155": LOCALHOST]);
    tlevel=([]);
    country = ([]);
    restore_object(T_SAVEFILE);
}

int remove() {
    save_object(T_SAVEFILE);
    return 1;
}

int new_location(string netz,string town,string desc,string country) {
    if(!IS_ARCH(this_interactive()))
        return 0;
    locations+=([netz:({town,desc,country})]);
    save_object(T_SAVEFILE);
    return 1;
}

int new_toplevel(string kurz,string lang) {
    if(!IS_ARCH(this_interactive()))
        return 0;
    tlevel+=([kurz:lang]);
    save_object(T_SAVEFILE);
    return 1;
}

int del_location(string netz) {
    if(!IS_ARCH(this_interactive()))
        return 0;
    locations = m_delete(locations,netz);
    save_object(T_SAVEFILE);
    return 1;
}

int del_toplevel(string tl) {
    if(!IS_ARCH(this_interactive()))
        return 0;
    tlevel=m_delete(tlevel,tl);
    save_object(T_SAVEFILE);
    return 1;
}

int new_country(string short, string long) {
  if (!IS_ARCH(this_interactive())) return 0;
  country += ([short:long]);
  save_object(T_SAVEFILE);
  return 1;  
}

int del_country(string short) {
  if (!IS_ARCH(this_interactive())) return 0;
  country = m_delete(country, short);
  save_object(T_SAVEFILE);
  return 1;
}

varargs string *get_information(mixed ob, int test) {
    string ip,ipname;
    string *ret,*top;
    int a,b,c,d;

    if(stringp(ob))
        ip=ob;
    else
        ip=query_ip_number(ob);


    if (sscanf(ip||"","%d.%d.%d.%d",a,b,c,d)!=4) {
        if(!test)
            return ({UNKNOWN,UNKNOWN,UNKNOWN});
        return 0;
    }
    if(ip=="127.0.0.1" || ip=="212.132.115.155")
      return LOCALHOST;
    if(ret=locations[ip])
        return ret;
    if(ret=locations[a+"."+b+"."+c])
        return ret;
    if(ret=locations[a+"."+b])
        return ret;
    if(ret=locations[(string)a])
        return ret;

#ifdef LOGGING
//    if(!test && objectp(ob)) log_file(T_UNRESOLVED, 
//      sprintf("%O: %O\n",ob->query_real_name(), ip);
    if(!test) log_file(T_UNRESOLVED, ip+"\n");
#endif

    if(objectp(ob) &&
       (ipname=query_ip_name(ob)) &&
       !to_int(ipname) &&
       sizeof(top=efun::explode(lower_case(ipname),".")) &&
       ret=tlevel[top[<1]])
      return ({UNKNOWN,UNKNOWN,ret});
    
    /* Mehr kann man eigentlich gar nicht mehr unternehmen :( */
    if(!test)
        return ({UNKNOWN,UNKNOWN,UNKNOWN});
    return 0;
}

varargs string town(mixed ob, int nocountry) {
  string *inf;
  string ret;
  
  inf=get_information(ob);

  if (member( ({"at", "österreich"}), lower_case(inf[COUNTRY])) != -1)
    inf[COUNTRY]="A";
  if (member( ({"de", "deutschland"}), lower_case(inf[COUNTRY])) != -1)
    inf[COUNTRY]="D";
  if (lower_case(inf[COUNTRY])=="a")
    nocountry=1;

  if (inf[TOWN]==UNKNOWN) {
    if(inf[COUNTRY]==UNKNOWN)
      return UNKNOWN;
    else
      return inf[COUNTRY];
  }
  if (nocountry)
    return inf[TOWN];
  return inf[TOWN]+" ("+capitalize(inf[COUNTRY])+")";
}

string description(mixed ob) {
    return get_information(ob)[DESCRIPTION];
}

string country(mixed ob) {
    return get_information(ob)[COUNTRY];
}

string query_country_long(string str) {
    return country[str] || UNKNOWN;
}    

mapping query_toplevel() {
    return copy(tlevel);
}

mapping query_locations() {
    return copy(locations);
}

int list_net(string str) {
    mixed *keys,*ret;
    int i;

    keys=sort_array(m_indices(locations),#'<); //');
    ret=({});
    for(i=sizeof(keys);i--;)
        ret+=({sprintf("%-:11s %-:13s %-:49s %-:3s",keys[i],locations[keys[i]][0],
            locations[keys[i]][1],locations[keys[i]][2])});
    ret-=({});
    if(str) {
        rm(T_DUMP_NET);
        write_file(T_DUMP_NET,implode(ret,"\n"));
    }
    this_interactive()->More(implode(ret+({"Insgesamt: "+sizeof(ret)}), "\n"));
    return 1;
}

int list_top(string str) {
    mixed *keys,*ret;
    int i;

    keys=sort_array(m_indices(tlevel),#'<); //');
    ret=({});
    for(i=sizeof(keys);i--;)
        ret+=({sprintf("%-5s %-50s",keys[i],tlevel[keys[i]])});
    ret-=({});
    if(str) {
        rm(T_DUMP_TOP);
        write_file(T_DUMP_TOP,implode(ret,"\n"));
    }
    this_interactive()->More(implode(ret+({"Insgesamt: "+sizeof(ret)}), "\n"));
    return 1;
}

int list_country(string str) {
    mixed *keys,*ret;
    int i;

    keys=sort_array(m_indices(country),#'<); //');
    ret=({});
    for(i=sizeof(keys);i--;)
        ret+=({sprintf("%-5s %-50s",keys[i],country[keys[i]])});
    ret-=({});
    if(str) {
        rm(T_DUMP_COUNTRY);
        write_file(T_DUMP_COUNTRY,implode(ret,"\n"));
    }
    this_interactive()->More(implode(ret+({"Insgesamt: "+sizeof(ret)}), "\n"));
    return 1;
}
