/*
 * Zaehlt die Spieler und wertet sie aus
 * Querolin@Tamedhon 15.02.2004   Daemonen reingenommen
 *
 * Querolin@Tamedhon 11.10.2001   Geburtstagscheck eingebaut.
 *
 * Querolin@Tamedhon 21.01.2001
 *
 */

/*
    Eingetragen in: 
     /secure/merlin.c
*/
 

#pragma strong_types

#include "def.h"
#include <properties.h>
#include <wizlevels.h>


#define DUMPFILE	"/log/statistik/max.users.genau"
#define ADB(x)          if(find_player("querolin")) \
                            tell_object(find_player("querolin"), \
                            break_string(x, 79, "{ abg } "));

string	*spieler, *races, *gratuliert;
int     sanzahl, r_zwerg, r_mensch, r_elf, r_brownie, r_ork, r_hobbit, 
        r_ungue, r_daemon, r_troll, r_goblin;
string  datum, dat1, dat2;
mixed  *userinfo;
int age;


/* --- Hilfsfunktionen ------------------------------------------------- */
void save_me() { save_object("/log/statistik/max.users.genau"); }

string timediff(int time)
{
  // Stammt ausm finger.c :)
  string ret;

  time=time*2;   //  Heartbeats!
  ret="";
  if(time>=86400) {
    ret+=time/86400+"d ";
    time%=86400;
  }
  if(time<36000) ret+="0";
  ret+=time/3600+":";
  time%=3600;
  if(time<600) ret+="0";
  ret+=time/60+":";
  time%=60;
  if(time<10) ret+="0";
  ret+=time+"";
  return ret;
}

int pl_exists(string str)
{
	string *fd;

	if(!str) return 0;
	fd=0; str=lowerstring(str);	
	fd=get_dir("/save/"+str[0..0]+"/"+str+".o");

	return sizeof(fd);
}

void clean_list()
{
    string	*sneu, *rcneu;
    int		i, is, r_zwergneu, sanzahlneu, r_hobbitneu, r_menschneu,
                r_orkneu, r_brownieneu, r_elfneu, r_ungueneu, r_daemonneu,
                r_trollneu, r_goblinneu;

    if(!is=sizeof(spieler)) return;
  //  ADB("cleaning toplist");
    /* erst mal alles cleanen ... */
    sneu=({}); rcneu=({}); r_zwergneu=0; sanzahlneu=0; r_hobbitneu=0;
    r_menschneu=0; r_orkneu=0; r_brownieneu=0; r_elfneu=0; r_ungueneu=0;
    r_daemonneu=0; r_trollneu=0; r_goblinneu=0;

    for(i=0; i<is; i++)
    {
	if ( (pl_exists(spieler[i])) || (races[i]=="gast") ) 
	{
	    sneu+=({ spieler[i] });
	    rcneu+=({ races[i] });
//            write(lower_case(races[i])); 
            switch(lower_case(races[i])){
              case "zwerg"  : r_zwergneu++; break;
              case "elf"    : r_elfneu++; break;
              case "ork"    : r_orkneu++; break;
              case "orc"    : r_orkneu++; break; //leider... :(
              case "mensch" : r_menschneu++; break;
              case "hobbit" : r_hobbitneu++; break;
              case "brownie": r_brownieneu++; break;
              case "dämon"  : r_daemonneu++; break;
              case "troll"  : r_trollneu++; break;
	      case "goblin" : r_goblinneu++; break;
              default: r_ungueneu++; break;
            }
            sanzahlneu++;
	}
    }

    spieler=({})+sneu;
    races=({})+rcneu;
    sanzahl=sanzahlneu;
    r_zwerg=r_zwergneu;
    r_elf=r_elfneu;
    r_ork=r_orkneu;
    r_mensch=r_menschneu;
    r_hobbit=r_hobbitneu;
    r_brownie=r_brownieneu;
    r_daemon=r_daemonneu;
    r_ungue=r_ungueneu;
    r_troll=r_trollneu;
    r_goblin=r_goblinneu;
}

void Dump()
{
    int		is, i;
    string	fmtstr;

    clean_list();    
    
    is=sizeof(spieler);
//    ADB("dumping toplist ("+to_string(is)+" entries)");
    if(!is) return;
    
    rm(DUMPFILE);
    
    fmtstr="%5i %5i %5i %5i %5i %5i %5i %5i %5i %5i %5i";
    write_file(DUMPFILE, 
      sprintf(fmtstr, 
		    // capitalize(spieler[i]), 
		    sanzahl, 
                    r_brownie,
                    r_elf,
                    r_hobbit,
                    r_mensch,
                    r_ork,
		    r_zwerg,
                    r_ungue, 
                    r_daemon,
                    r_troll,
		    r_goblin
		   ));
		    
    write_file(DUMPFILE, "\n");
    save_me();
}

void reset()
{
//    ADB("resetting playercount...");
    Dump();
    save_me();
}

void _setdate(string str)
{
  datum=str;
}

void _neuertag()
{
    string      *sneu, *rcneu;
    string 	fmtstr;
    int         i, is, r_zwergneu, sanzahlneu, r_hobbitneu, r_menschneu,
                r_orkneu, r_brownieneu, r_elfneu, r_ungueneu, r_daemonneu,
                r_trollneu, r_goblinneu;
 
    if(!is=sizeof(spieler)) return;

   // Gesamtstatistik schreiben.

    fmtstr="%20s %5i %5i %5i %5i %5i %5i %5i %5i %5i %5i %5i\n";
    write_file("/log/playercount",
      sprintf(fmtstr,
                    dtime(time()-86400),
                    sanzahl,
                    r_brownie,
                    r_elf,
                    r_hobbit,
                    r_mensch,
                    r_ork,
                    r_zwerg,
                    r_ungue,
                    r_daemon,
                    r_troll,
		    r_goblin
                   ));           

  //  ADB("cleaning toplist");
    /* erst mal alles cleanen ... */
    sneu=({}); rcneu=({}); r_zwergneu=0; sanzahlneu=0; r_hobbitneu=0;
    r_menschneu=0; r_orkneu=0; r_brownieneu=0; r_elfneu=0; r_ungueneu=0;
    r_daemonneu=0; r_trollneu=0;r_goblinneu=0;
 
    for(i=0; i<is; i++)
    {
        if  (find_player(spieler[i]))
        {
            sneu+=({ spieler[i] });
            rcneu+=({ races[i] });
            switch(lower_case(races[i])){
              case "zwerg"  : r_zwergneu++; break;
              case "elf"    : r_elfneu++; break;
              case "ork"    : r_orkneu++; break;
              case "orc"    : r_orkneu++; break; //leider... :(
              case "mensch" : r_menschneu++; break;
              case "hobbit" : r_hobbitneu++; break;
              case "brownie": r_brownieneu++; break;
              case "dämon"  : r_daemonneu++; break;
              case "troll"  : r_trollneu++; break;
	      case "goblin" : r_goblinneu++; break;
              default: r_ungueneu++; break;
            }
            sanzahlneu++;
        }
    }
 
    spieler=({})+sneu;
    races=({})+rcneu;
    sanzahl=sanzahlneu;
    r_zwerg=r_zwergneu;
    r_elf=r_elfneu;
    r_ork=r_orkneu;
    r_mensch=r_menschneu;  
    r_hobbit=r_hobbitneu;
    r_brownie=r_brownieneu;
    r_daemon=r_daemonneu;
    r_ungue=r_ungueneu;
    r_troll=r_trollneu;
    r_goblin=r_goblinneu;

//    ADB("setze Playercount auf null.\n");
    datum=extract(ctime(),0,2);
    save_me();                
    Dump();           
    gratuliert=({});
}

void Register(object pl)
{
    int		i, is;
    string	name;

    if (extract(ctime(),0,2)!=datum)
    {
      _neuertag();
    }

    if(!pl) return;
    if(!interactive(pl)) return;
    
    name=lowerstring(pl->query_real_name());

    if(name[0..3] == "gast") 
    {
      // Gaeste bekommen eine eigene Routine...
      // Da ja Gaeste net gezaehlt werden koennen nach Name
      // zaehle ich hier einfach die IP-Adr. als Name       ;)

       name=pl->QueryProp(P_CALLED_FROM_IP);
//       ADB("trying to register '"+name+"' in PlayerCount."); 
       if (member(spieler, name) == -1)
       {
//       ADB("trying to register '"+name+"' as Player in PlayerCount."); 
        spieler+=({ name });
        races+=({ "gast" });
        sanzahl++;
        r_ungue++;
       }                         
      save_me();
      Dump();   
//      reset();     
      return;
    }

//  gratuliert=({}); // wuerde die liste beim upd -l leeren! zu debuging :)

    age=pl->QueryProp("age");
    if ((age>86400) && (member(gratuliert,pl->QueryProp(P_NAME)))==-1)
    { 
        //age>86400 -> wer net aelter als 1 tag ist der hat auch net geb :)

        userinfo="/secure/master.c"->get_userinfo(lower_case(pl->QueryProp(P_NAME)));
        if (sizeof(userinfo)>4) {
          dat1=extract(to_string(dtime(userinfo[5])),5,12);
        }
        dat2=extract(to_string(dtime(time())),5,12);
//        dat2="31. Jan "; // Queros Geburtstag :)
//        ADB("Name: "+pl->QueryProp(P_NAME)+
//            " dat1:"+dat1+"! dat2:"+dat2+"!"+age+"\n");
        if (dat1==dat2)
        {
          ADB("GEBURTSTAG von "+pl->QueryProp(P_NAME)+"\n");
          gratuliert+=({ pl->QueryProp(P_NAME) });

        shout("Merlin ruft: "+pl->QueryProp(P_NAME)+" hat heute Geburtstag!\n");
        tell_room(ENV(pl),BS("Merlin ruft: "+pl->QueryProp(P_NAME)+
                          " hat heute Geburtstag!\n"));
        tell_object(pl,BS("Merlin wünscht Dir alles Gute aus der Ferne.\n"));
          
        }
       
    }
    
//    ADB("trying to register '"+name+"' in PlayerCount.");   

//   Auf Grund diverser Bugs eingefuegt:
//   Rassenueberpruefung:
     if (this_player()->QueryProp(P_RACE)==0) 
     {
       tell_object(this_player(),BS("Du hast leider keine gültige Rasse und "
        "würdest somit nicht im Playercount mitgezählt! Daher wird Deine "
        "Rasse automatisch auf 'Mensch' gesetzt. "
        "Bitte Erzmagier verständigen!\n"));
       TP->SetProp(P_RACE, "Mensch");
     }
 
    if (member(spieler, name) == -1)  
    {
	spieler+=({ name });
	races+=({ lowerstring(pl->QueryProp(P_RACE)) });
        sanzahl++;
        switch(lower_case(pl->QueryProp(P_RACE))) {
         case "zwerg"  : r_zwerg++; break;
         case "elf"    : r_elf++; break;
         case "mensch" : r_mensch++; break;
         case "hobbit" : r_hobbit++; break;
         case "ork"    : r_ork++; break;
         case "brownie": r_brownie++; break;
         case "dämon" : r_daemon++; break;
         case "troll"  : r_troll++; break;
	 case "goblin" : r_goblin++; break;
         default: r_ungue++; break;
        }
    }
   reset();
}

void create()
{
    seteuid(getuid());
    if(!restore_object("/log/statistik/max.users.genau"))
    {
	spieler=({});
	races=({}); sanzahl=0; r_zwerg=0; r_brownie=0; r_elf=0; r_hobbit=0;
        r_ork=0; r_mensch=0; r_ungue=0; r_daemon=0; r_troll=0; r_goblin=0;
        datum=extract(ctime(),0,2); 
        gratuliert=({});
    }
//    ADB("geladen...");
}

