/* eine Top-Alters-Liste
 * Querolin@Tamedhon 09.09.2001
 *
 * Abenteurer Toplistenmaster
 * Sun Sep 17 13:39:47 2000, Samtpfote@Tamedhon
 *
 * $Date: 2001/01/12 19:09:47 $
 * $Revision: 1.1 $
 * $Log: abgstatmaster.c,v $
 * Revision 1.1  2001/01/12 19:09:47  Samtpfote
 * Initial revision
 *
 */

/*
    Eingetragen in: 
        /secure/merlin.c (register)
        ABG + Ergochach (init, create)
*/
 

#pragma strong_types

#include "def.h"
#include <properties.h>
#include <wizlevels.h>

#define FL_MAGIER	"Magier"
#define FL_TESTIE	"Testie"
#define FL_SEHER	"Seher"
#define FL_SPIELER	"Spieler"
#define FL_ZWEITIE	"Zweitspieler"

#define DUMPFILE	"/p/service/querolin/listen/alterliste.sort"
#define ADB(x)          if(find_player("querolin")) \
                            tell_object(find_player("querolin"), \
                            break_string(x, 79, "{ alterliste } "));

int	*ratings, *levels, *age;
string	*spieler, *flags, *guilds, *races;

/* --- Hilfsfunktionen ------------------------------------------------- */
void save_me() { save_object("/p/service/querolin/listen/alterliste"); }

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
    int		*rneu, *lneu, *ageneu;
    string	*sneu, *fneu, *gneu, *rcneu;
    int		i, is;

    if(!is=sizeof(spieler)) return;
  //  ADB("cleaning toplist");
    /* erst mal alles cleanen ... */
    rneu=({}); lneu=({}); sneu=({}); fneu=({});
    gneu=({}); rcneu=({}); ageneu=({});
    if (is>=150) { is=150; } // Liste auf 150 beschraenken...
    for(i=0; i<is; i++)
    {
	if ( (pl_exists(spieler[i])) && 
	   (levels[i] > 1) &&
           (flags[i]!="Testie") )
	{
 	    rneu+=({ ratings[i] }); lneu+=({ levels[i] }); 
	    sneu+=({ spieler[i] }); fneu+=({ flags[i] });
	    gneu+=({ guilds[i] });  ageneu+=({ age[i] });
	    rcneu+=({ races[i] });  
	}
    }

    ratings=({})+rneu; 
    levels=({})+lneu; 
    flags=({})+fneu; 
    spieler=({})+sneu;
    guilds=({})+gneu;
    races=({})+rcneu;
    age=({})+ageneu;
}

void swap_record(int a, int b)
{
    int		hrating, hlevel, hage;
    string	hspieler, hguild, hflag, hrace;
    
    hrating=ratings[a]; hlevel=levels[a]; hage=age[a];
    hspieler=spieler[a]; hguild=guilds[a]; hflag=flags[a]; hrace=races[a];
    
    ratings[a]=ratings[b]; levels[a]=levels[b]; 
    spieler[a]=spieler[b]; guilds[a]=guilds[b]; flags[a]=flags[b]; 
    races[a]=races[b]; age[a]=age[b];
     
    ratings[b]=hrating; levels[b]=hlevel;
    spieler[b]=hspieler; guilds[b]=hguild; flags[b]=hflag;
    races[b]=hrace; age[b]=hage;
}
    
void sort_list()
{
    int		i, j, is;

    is=sizeof(spieler);
//    ADB("sorting toplist ("+to_string(is)+" entries)");
    if(is < 2) return;    
    
    for(i=0; i<is; i++)
    {
	// ADB("i="+to_string(i)+" j="+to_string(j));
	for(j=i+1; j<is; j++)
	{
	    if(age[i] < age[j])
	    {
		swap_record(i, j);
	    }
	}
    }
    
}
    
void Dump()
{
    int		is, i;
    string	fmtstr;

    sort_list();
    // Zuerst sortieren, denn bei clean_liste wird bei 150 abgeschnitten!
    clean_list();
    
    is=sizeof(spieler);
//    ADB("dumping toplist ("+to_string(is)+" entries)");
    if(!is) return;
    
    rm(DUMPFILE);
    
    //      spieler stufe rating qp xp fl
    write_file(DUMPFILE, "\n-* Die ältesten Spieler und Magier *-\n");
    write_file(DUMPFILE, "   Aktualisiert: "+dtime(time())+"\n");
    write_file(DUMPFILE,"\n"+
       "Rang    Name             Stufe     Alter             Gilde\n\n");
    fmtstr="%3i.  %s %15-s    %3i    %14s     %s\n";
    
    for(i=0; i<is; i++)
    {
	write_file(DUMPFILE, 
	    sprintf(fmtstr, 
		    i+1,
		    (flags[i]==FL_SEHER?"*":(flags[i]==FL_ZWEITIE?"+":(flags[i]==FL_MAGIER?"|":" "))),
		    capitalize(spieler[i]), 
		    levels[i], 
		    timediff(age[i]),
		    capitalize(guilds[i])
		   ));
		    
	if(i >= 99) break;
    }
    write_file(DUMPFILE, "\n      Erklärung:  '|' - Magier   '*' - Seher   '+' - Zweiti\n");
    write_file(DUMPFILE, "\n");
    
    save_me();
}

void Register(object pl)
{
    int		i, is, p;
    string	fl, name;

    if(!pl) return;
    if(!interactive(pl)) return;
//    if(IS_LEARNER(pl)) return;
    if(pl->QueryProp(P_TESTPLAYER)) return;
//    if(pl->QueryProp(P_DEADS)==0) return;
    
    name=lowerstring(pl->query_real_name());

    if(name[0..3] == "gast") return;
    
//  ADB("trying to register '"+name+"'.");    
    if (member(spieler, name) == -1)  
    {
//        if(IS_LEARNER(pl)) return;
        if(pl->QueryProp(P_TESTPLAYER)) return;

	spieler+=({ name });
 
        if (IS_LEARNER(pl)) {       
           levels+=({ query_wiz_level(pl) }); }
        else {
           levels+=({ pl->QueryProp(P_LEVEL) }); }

        age+=({ pl->QueryProp("age") });
        
        ratings+=({ 1 });
	fl=FL_SPIELER;
	if(IS_LEARNER(pl)) { fl=FL_MAGIER; } 
	else if(pl->QueryProp(P_TESTPLAYER)) { fl=FL_TESTIE; }
	else if(pl->QueryProp(P_SECOND)) { fl=FL_ZWEITIE; }
	else if(IS_SEER(pl)) { fl=FL_SEHER; }
	flags+=({ fl }); 

	guilds+=({ lowerstring(pl->QueryProp(P_GUILD)) });
	races+=({ lowerstring(pl->QueryProp(P_RACE)) });
    }
    else 
    
    /* evtl update machen .... */    
    if((p=member(spieler, name)) >= 0)
    {
//    	ADB("... updating "+name);

        if (IS_LEARNER(pl)) {
           levels[p]=query_wiz_level(pl); }
        else {
           levels[p]=pl->QueryProp(P_LEVEL); }

        ratings[p]=1; 
        age[p]=pl->QueryProp("age");

	fl=FL_SPIELER;
	if(IS_LEARNER(pl)) { fl=FL_MAGIER; }
	else if(pl->QueryProp(P_TESTPLAYER)) { fl=FL_TESTIE; }
	else if(pl->QueryProp(P_SECOND)) { fl=FL_ZWEITIE; }
	else if(IS_SEER(pl)) { fl=FL_SEHER; }
	flags[p]=fl; 
	
	guilds[p]=lowerstring(pl->QueryProp(P_GUILD));
	races[p]=lowerstring(pl->QueryProp(P_RACE));

    }
//  call_out("reset",0);
}

/* --- Standardfunktionen ---------------------------------------------- */
void reset()
{
//    ADB("resetting...");
    Dump();
    save_me();
}

void create()
{
    seteuid(getuid());
    if(!restore_object("/p/service/querolin/listen/alterliste"))
    {
	ratings=({}); levels=({}); flags=({}); spieler=({});
	guilds=({}); races=({}); age=({});
    }
//    ADB("alterliste geladen...");
}
