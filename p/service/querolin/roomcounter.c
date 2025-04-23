/* Der Roomcounter - zaehlt Raeume...
 * Querolin@Tamedhon 07.11.2005
 *
 * Verwendung ausserhalb des Tamedhons nur mit schriftlicher Erlaubnis
 * des Autors und auf eigene Gefahr.
 */

/*
    Eingetragen in:
        /room/death/death_room.c   (init, create)
        /d/gidhrof/room/room-f-05.c (init, create)
*/

// Aenderungen:
// 28.03.2008  TamTam  Nur Blueprints aufnehmen; Clones Rauswerfen;
//                     Nicht mehr vorhandene Raeume Rauswerfen; Logging;
//                     Fehler beim aufnehmen den Gildenraeume behoben
//

#pragma strong_types

#include <properties.h>

#define SAVEFILE	"/log/roomcounter"
#define DB(x)          if(find_player("querolin")) \
                            tell_object(find_player("querolin"), \
                            break_string("RC:"+x+".\n", 78));

string	*room_fname;

/* --- Hilfsfunktionen ------------------------------------------------- */
void save_me() { save_object(SAVEFILE); }

int __count_me(string fname)
{
    int i;

    if (!fname) return 0;
    if (strlen(fname)<3) return -1;

    // Nur Raeume in Regionen und Gilden
    if (extract(fname, 0, 8) =="/players/") return -2;
    if ( (extract(fname, 0, 2) !="/d/") &&
         (extract(fname, 0, 7) !="/gilden/") )
     return -3;

    // Nur Blueprints
    if(sizeof(explode(fname, "#")) != 1)
     return -4;

    if (member(room_fname, fname) != -1)
    {
        //Schon gezaehlt...
        return 2;
    }
    else
    {
        room_fname+=({fname});
        return 1;
    }
}

int __show_count()
{
   return sizeof(room_fname);
}

string __show_regions()
{
   int i, gidhrog, steppe, tal, grasland, seher, elfenwald, gebirge, nordmeer,
       tundra, unterwelt, wueste,gilden;
   string str, *del_str, *obs_str;

   gidhrog=steppe=tal=grasland=seher=elfenwald=gebirge=nordmeer=0;
   tundra=unterwelt=wueste=gilden=0;
   del_str=({}); obs_str=({});

   for(i=0;i<sizeof(room_fname);i++)
   {
     if (sizeof(explode(room_fname[i], "#")) != 1)
     { // Clones Rauswerfen
       del_str+=({room_fname[i]});
       write_file (SAVEFILE+".log",
        sprintf("%s - Clone deleted: (%s)\n",
        dtime(time()), room_fname[i]));
       continue;
     }
     if (file_size(room_fname[i]+".c") < 0)
     { // Nicht mehr vorhandene Raeume Rauswerfen
       obs_str+=({room_fname[i]});
       write_file (SAVEFILE+".log",
        sprintf("%s - Obsolete deleted: (%s)\n",
        dtime(time()), room_fname[i]));
       continue;
     }
     if (strstr(room_fname[i], "/d/tal/")!=-1) tal++;
     if (strstr(room_fname[i], "/d/gidhrog/")!=-1) gidhrog++;
     if (strstr(room_fname[i], "/d/steppe/")!=-1) steppe++;
     if (strstr(room_fname[i], "/d/grasland/")!=-1) grasland++;
//     if (strstr(room_fname[i], "/d/seher/")!=-1) seher++;
     if (strstr(room_fname[i], "/d/elfenwald/")!=-1) elfenwald++;
     if (strstr(room_fname[i], "/d/gebirge/")!=-1) gebirge++;
     if (strstr(room_fname[i], "/d/nordmeer/")!=-1) nordmeer++;
//     if (strstr(room_fname[i], "/d/tundra/")!=-1) tundra++;
//     if (strstr(room_fname[i], "/d/unterwelt/")!=-1) unterwelt++;
//     if (strstr(room_fname[i], "/d/wueste/")!=-1) wueste++;
     if (strstr(room_fname[i], "/gilden/")!=-1) gilden++;
   }
   str="\n  Region        Räume\n";
   str+="\n   Elfenwald     "+to_string(elfenwald);
   str+="\n   Gebirge       "+to_string(gebirge);
   str+="\n   Gidhrog       "+to_string(gidhrog);
   str+="\n   Grasland      "+to_string(grasland);
   str+="\n   Nordmeer      "+to_string(nordmeer);
// str+="\n   Seher         "+to_string(seher);
   str+="\n   Steppe        "+to_string(steppe);
   str+="\n   Tal           "+to_string(tal);
// str+="\n   Tundra        "+to_string(tundra);
// str+="\n   Unterwelt     "+to_string(unterwelt);
// str+="\n   Wüste        "+to_string(wueste);
   str+="\n   Gilden        "+to_string(gilden);
   str+="\n                -------";
   str+="\n                 "+to_string(i-sizeof(del_str)-sizeof(obs_str));
   str+="\n\n";
   tell_object(this_player(), str);
   if(sizeof(del_str))
   {
     tell_object(this_player(), sizeof(del_str)+" Clones deleted!\n\n");
     room_fname-=del_str;
     save_me();
   }
   if(sizeof(obs_str))
   {
     tell_object(this_player(), sizeof(obs_str)+" Obsoletes deleted!\n\n");
     room_fname-=obs_str;
     save_me();
   }
   return "";
}

/* --- Standardfunktionen ---------------------------------------------- */
void reset()
{
    save_me();
}

void create()
{
    seteuid(getuid());
    if(!restore_object(SAVEFILE))
    {
	room_fname=({});
    }
}
