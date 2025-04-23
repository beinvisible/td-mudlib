/* Tamedhon MUDlib
 *
 * /secure/udp/who.c -- UDP who handling
 * 
 * 2024/04/11 - Adressen ans Tame v2 angepasst - Aloy
 *
 * $Date: 2004/02/17 19:25:00 $
 * $Revision: 2.1 $
 * $Log: who.c,v $
 *                            
 * Revision 2.1  2004/02/17 19:25:00  Querolin
 * Rassenstat: daemonen eingefuegt.
 * 
 * Revision 2.0  2002/03/18 08:57:00  Querolin
 * Bugfix: Invis Magier wurden mitgezaehlt...
 *
 * Revision 1.9  2001/03/04 20:37:00  Querolin
 * Design veraendert auf neues aussehen und paar bug-fixes
 *
 * Revision 1.8  2001/03/04 19:18:00  Querolin
 * Ab 10 Leuten nur mehr kurze Listenform. 
 * Zusaetzlich wird ab sofort die Statistik immer angezeigt
 *
 * Revision 1.7  2001/01/22 07:10:00  Querolin
 * PlayerCount eingebaut.  
 *
 * Revision 1.6
 * Mud-Adresse auf "mud.tamehon.at" geaendert....  Querolin
 * 
 * Revision 1.5  1998/01/03 14:00:09  Woody
 * "Test-MUD" rausgenommen
 *
 * Revision 1.4  1997/09/02 15:13:20  Woody
 * MUD-Adresse im remote 'wer' ausgeben
 *
 * Revision 1.3  1997/08/21 14:06:33  Woody
 * Kurzer Header bei -k, MaxUsers bei Merlin erfragen
 *
 * Revision 1.2  1997/08/20 22:59:30  Woody
 * Auswerten des DATA Fields (v.a. fuer short Listing etc.)
 *
 * Revision 1.1  1997/03/16 00:39:13  Woody
 * prepared for revision control
 *
 */

#include <udp.h>

#define MERLIN "/secure/merlin"
#define TD(x) if (find_player("bort")) \
                tell_object(find_player("bort"),x)

#define DB(x)       if(find_player("bort")) \
                tell_object(find_player("bort"), \
                                break_string(x, 79, "{ "+object_name(this_object())+" } "));
  
int breite;    //  wird benoetigt, da bei kurzer und langer anzeige
               //  unterschiedliche breiten vorkommen... 


/*
int last;
int maxtoday, maxever;

void create()
{
  string tmp1, tmp2, dummy;

  if (time()-last<1800) return;
  last=time();
  tmp1=read_file("/etc/maxusers.ever",0,1);
  tmp2=read_file("/etc/maxusers",0,1);
  if (stringp(tmp1)&&sizeof(tmp1)) (void)sscanf(tmp1,"%d %s",maxever,dummy);
  if (stringp(tmp2)&&sizeof(tmp2)) (void)sscanf(tmp2,"%d %s",maxtoday,dummy);
}
*/

string _PlayerCount()
{
  int anz,bro,elf,hob,men,ork,zwe,ung,num,i,dae,tro;
  string str,stra;

//  tell_object(find_player("querolin"),break_string("PlayerCount in who started.\n"));

  if (read_file("/log/statistik/max.users.genau"))
  {
    anz=to_int(extract(read_file("/log/statistik/max.users.genau"),2,4));
    bro=to_int(extract(read_file("/log/statistik/max.users.genau"),8,10)); 
    elf=to_int(extract(read_file("/log/statistik/max.users.genau"),14,16)); 
    hob=to_int(extract(read_file("/log/statistik/max.users.genau"),20,22)); 
    men=to_int(extract(read_file("/log/statistik/max.users.genau"),26,28)); 
    ork=to_int(extract(read_file("/log/statistik/max.users.genau"),32,34)); 
    zwe=to_int(extract(read_file("/log/statistik/max.users.genau"),38,40)); 
    ung=to_int(extract(read_file("/log/statistik/max.users.genau"),44,46)); 
    dae=to_int(extract(read_file("/log/statistik/max.users.genau"),50,52));
    tro=to_int(extract(read_file("/log/statistik/max.users.genau"),56,58));
    str=sprintf("Heute waren bereits %i verschiedene Spieler Online, davon:\n",anz);
   num=breite-sizeof(str);
   if (num>1)
   {
     for(i=0;i<(num / 2);i++) {
       str=" "+str;
     }
   }

    stra="";
//    if (bro>0) { stra+=sprintf("%i Brownie ,",bro); stra+=(bro>1?"s,":","); }
    if (bro>0) { stra+=sprintf("%i Bro, ",bro);  }
    if (dae>0) { stra+=sprintf("%i Dae, ",dae);  }
    if (elf>0) { stra+=sprintf("%i Elf, ",elf);  }
    if (hob>0) { stra+=sprintf("%i Hob, ",hob);  }
    if (men>0) { stra+=sprintf("%i Men, ",men);  }
    if (ork>0) { stra+=sprintf("%i Ork, ",ork);  }
    if (tro>0) { stra+=sprintf("%i Tro, ",tro);  }
    if (zwe>0) { stra+=sprintf("%i Zwe, ",zwe);  }
    if (ung>0) { stra+=sprintf("%i And, ",ung);  }

    // Den hintersten Beistrich noch weg...
    stra=extract(stra,0,sizeof(stra)-3);

   num=breite-sizeof(stra)-1;  // -1 weils einfach net passen wollte ;)
   if (num>1)
   {
     for(i=0;i<(num / 2);i++) {
       stra=" "+stra;
     }
   }

    str=str+stra+"\n";
    return str;
  }
 return "";
}

void udp_who(mapping data)
{
  int i,j,num,abzug;
  string *lines;
  string wholiste, header, str;
  mixed anwesende;

  //TD(sprintf("data=%O\n", data));

  // create();
  breite=78;
  num=sizeof(users());

  anwesende=({})+users();
  abzug=0;
  for(i=0;i<num;i++)
  { // Invis Magier duerfen natuerlich nicht gezaehlt werden... Quero
    if (anwesende[i]->QueryProp("invis")!=0)
    {
      abzug++;
    }
  }
  num=num-abzug; 

  lines = "/obj/werliste"->QueryWhoListe(0, 0);

  wholiste = implode(lines,"\n");

  lines=({
sprintf(" LDMud %:-11s   ",explode(__VERSION__," (")[0]),
"TESTMUD\n"});

str=sprintf("Uptime: %s",uptime());
j=breite-sizeof(str);
if (j>1)
{
  for(i=0;i<(j / 2);i++) {
    str=" "+str;
  }
}
lines+=({sprintf(str)});

  header=implode(lines,"\n");
  wholiste=header+"\n"+wholiste+"\n";
//  num=sizeof(users());

  str=sprintf("Anwesende: Aktuell %d, Max. heute gleichzeitig %d, "
              "Rekord %d\n",
              num, MERLIN->QueryMaxUsersToday(), MERLIN->QueryMaxUsers() );
  j=breite-sizeof(str);
  if (j>1) 
  { 
    for(i=0;i<(j / 2);i++) {
      str=" "+str;
    }
  }
  wholiste+=sprintf("\n"+str);

  // Playercount auch nur dann, wenn auch die MAX-Werte angezeigt werden...
  //  (ans[0] & 1 ? "":wholiste+=_PlayerCount()); 
   wholiste+=_PlayerCount();
  INETD->send_udp(data[NAME], ([
				REQUEST: REPLY,
				RECIPIENT: data[SENDER],
				ID: data[ID],
				DATA: wholiste
				]) );
}
