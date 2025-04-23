/* Eine Forscher-Top-Liste
 * Querolin@Tamedhon 23.02.2001
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
*/
 

#pragma strong_types

#include "def.h"
#include <properties.h>
#include <wizlevels.h>
#include <moving.h>

#define FL_MAGIER	"Magier"
#define FL_TESTIE	"Testie"
#define FL_SEHER	"Seher"
#define FL_SPIELER	"Spieler"
#define FL_ZWEITIE	"Zweitspieler"

#define DUMPFILE	"/p/service/querolin/listen/fpliste.sort"
#define ADB(x)          if(find_player("querolin")) \
                            tell_object(find_player("querolin"), \
                            break_string(x, 79, "{ abg } "));

int	*ratings, *levels, *xp, *qp, *fp;
string	*spieler, *flags, *guilds, *races;

/* --- Hilfsfunktionen ------------------------------------------------- */
void save_me() { save_object("/p/service/querolin/listen/fpliste"); }

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
    int		*rneu, *lneu, *xneu, *qneu, *fpneu; 
    string	*sneu, *fneu, *gneu, *rcneu;
    int		i, is;

    if(!is=sizeof(spieler)) return;
  //  ADB("cleaning toplist");
    /* erst mal alles cleanen ... */
    rneu=({}); lneu=({}); xneu=({}); qneu=({}); sneu=({}); fneu=({});
    gneu=({}); rcneu=({}); fpneu=({});
    if (is>=150) { is=150; } // Liste auf 150 beschraenken...
    for(i=0; i<is; i++)
    {
	if ( (pl_exists(spieler[i])) && 
	   (levels[i] > 1) &&
           (flags[i]!="Magier") && (flags[i]!="Testie") )
	{
 	    rneu+=({ ratings[i] }); lneu+=({ levels[i] }); xneu+=({ xp[i] });
	    sneu+=({ spieler[i] }); qneu+=({ qp[i] }); fneu+=({ flags[i] });
	    gneu+=({ guilds[i] }); 
	    rcneu+=({ races[i] }); fpneu+=({ fp[i] }); 
	}
    }

    ratings=({})+rneu; 
    levels=({})+lneu; 
    xp=({})+xneu; 
    flags=({})+fneu; 
    spieler=({})+sneu;
    qp=({})+qneu;
    guilds=({})+gneu;
    races=({})+rcneu;
    fp=({})+fpneu;
}

void swap_record(int a, int b)
{
    int		hrating, hlevel, hxp, hqp, hfp;
    string	hspieler, hguild, hflag, hrace;
    
    hrating=ratings[a]; hlevel=levels[a]; hxp=xp[a]; hqp=qp[a];
    hspieler=spieler[a]; hguild=guilds[a]; hflag=flags[a]; hrace=races[a];
    hfp=fp[a]; 
    
    ratings[a]=ratings[b]; levels[a]=levels[b]; xp[a]=xp[b]; qp[a]=qp[b];
    spieler[a]=spieler[b]; guilds[a]=guilds[b]; flags[a]=flags[b]; 
    races[a]=races[b]; fp[a]=fp[b];
     
    ratings[b]=hrating; levels[b]=hlevel; xp[b]=hxp; qp[b]=hqp;
    spieler[b]=hspieler; guilds[b]=hguild; flags[b]=hflag;
    races[b]=hrace; fp[b]=hfp; 
}
    
void sort_list()
{
    int	i, j, is;

    is=sizeof(spieler);
//    ADB("sorting toplist ("+to_string(is)+" entries)");
    if(is < 2) return;    
    
    for(i=0; i<is; i++)
    {
	// ADB("i="+to_string(i)+" j="+to_string(j));
	for(j=i+1; j<is; j++)
	{
	    if(fp[i] < fp[j])
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
      //Zuerst sortieren, denn beim clean_list wird ab 150 
      //Eintraegern abgeschnitten!
    clean_list();    
    
    is=sizeof(spieler);
//    ADB("dumping fp-toplist ("+to_string(is)+" entries)");
    if(!is) return;
    
    rm(DUMPFILE);
    
    //      spieler stufe rating qp xp fl
    write_file(DUMPFILE, "\n-* Die besten Forscher des Tamedhons *-\n");
    write_file(DUMPFILE, "   Aktualisiert: "+dtime(time())+"\n");
    write_file(DUMPFILE,"\n"+
       "Rang    %    Name             Stufe    Rasse       Gilde\n\n");
    fmtstr="%3i.  %3i   %s%15-s    %3i    %10-s  %15-s\n";
    
    for(i=0; i<is; i++)
    {
	write_file(DUMPFILE, 
	    sprintf(fmtstr, 
		    i+1,
                    ((100*fp[i])/fp[0]),
		    (flags[i]==FL_SEHER?"*":(flags[i]==FL_ZWEITIE?"+":" ")),
		    capitalize(spieler[i]), 
		    levels[i], 
		    capitalize(races[i]),
		    capitalize(guilds[i])
		   ));
		    
	if(i >= 99) break;
    }
    write_file(DUMPFILE, "\n");
    
    save_me();
}

void Register(object pl)
{
    int		i, is, abilsum, p, ac;
    string	fl, name;

    if(!pl) return;
    if(!interactive(pl)) return;
//    if(IS_LEARNER(pl)) return;
//    if(pl->QueryProp(P_TESTPLAYER)) return;
       
    name=lowerstring(pl->query_real_name());
    if(name[0..3] == "gast") return;
    
//    ADB("trying to register in fp-topliste '"+name+"'.");    
    if (member(spieler, name) == -1)  
    {

       if(IS_LEARNER(pl)) return;
       if(pl->QueryProp(P_TESTPLAYER)) return;

	spieler+=({ name });
	levels+=({ pl->QueryProp(P_LEVEL) });
	xp+=({ pl->QueryProp(P_XP) });
	fp+=({ pl->QueryProp("ep_num") });
        qp+=({ pl->QueryProp(P_QP) });
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
	levels[p]=pl->QueryProp(P_LEVEL);
	xp[p]=pl->QueryProp(P_XP); fp[p]=pl->QueryProp("ep_num");
        ratings[p]=1; qp[p]=pl->QueryProp(P_QP);
	fl=FL_SPIELER;
	if(IS_LEARNER(pl)) { fl=FL_MAGIER; }
	else if(pl->QueryProp(P_TESTPLAYER)) { fl=FL_TESTIE; }
	else if(pl->QueryProp(P_SECOND)) { fl=FL_ZWEITIE; }
	else if(IS_SEER(pl)) { fl=FL_SEHER; }
	flags[p]=fl; 
	
	guilds[p]=lowerstring(pl->QueryProp(P_GUILD));
	races[p]=lowerstring(pl->QueryProp(P_RACE));

    }
  
  call_out("reset",0);
}

/* --- Standardfunktionen ---------------------------------------------- */
void reset()
{
//    ADB("fp-topliste resetting...");
    Dump();
    save_me();
}

void create()
{
    seteuid(getuid());
    if(!restore_object("/p/service/querolin/listen/fpliste"))
    {
	ratings=({}); levels=({}); xp=({}); flags=({}); spieler=({});
	qp=({}); guilds=({}); races=({}); fp=({});
    }
//    ADB("fp-topliste geladen...");
}
