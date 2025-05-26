/*
  Übernommen von Aloy
 
 
  Seer-Tool.
  Ein Tool mit dessen Hilfe Seher ihr Haus ausbauen koennen.
  Querolin@Tamedhon April 2001

  Datendefinition:
   ({"Bez", "Beschreibung", "key0", "key1", ...})

      Bez : 
           0 = P_SHORT
           1 = P_LONG
           2 = AddDetail
           3 = AddSound
           4 = AddSmell
           5 = AddReadDetail
           6 = AddExit

*/

#pragma strong_types

inherit "std/thing";
inherit "mail/nedit";

#include <defines.h>
#include <properties.h>
#include <language.h>
#include <wizlevels.h>
#include <moving.h>
#include <ansi.h>

#define LOGPATH "/d/seher/log/"
#define BUGFILE LOGPATH"bugfile"
#define MAXSTACK 10
#define BUGPATH LOGPATH
#define log(x) write_file(BUGPATH+TP->name()+".log", x+"\n");
#define PATH "/d/seher/haeuser/"+geteuid(this_player())+"/room"
#define ZEICHEN ({"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","ä","ö","ü","ß","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","Ä","Ö","Ü","0","1","2","3","4","5","6","7","8","9","0",".",",","_","-","'","`","!","/","(",")"," ","!","?","=",":",";","#","*","\\","\n"})

#define CLOSURE(x) #'x
#define BS(x)  break_string(x, 78)
#define TO     this_object()
#define TP     this_player()
#define TPN    this_player()->Name()
#define TI     this_interactive()
#define TIPO   this_interactive()||previous_object()
#define CAP_TPN capitalize(this_player()->Name() )
#define ENV(x) environment(x)

int saved, i, ausgangerlaubt;
mixed err;
string roomname, str;
string **code;
string *codedet; 
string p_long, p_short, p_light, p_env, p_temp, p_indoors;
mixed *stack;

void create()
{
 if (!clonep(this_object())) return;
 ::create();
 seteuid(getuid(this_object()));

 SetProp(P_SHORT, "Ein Seherhammer");
 SetProp(P_LONG, "Ein kleiner Hammer der allerdings ein sehr mächtiges "
  +"Werkzeug zu sein scheint. Mit 'hhilfe' kann man Näheres über ihn "
  +"erfahren.\n");
 SetProp(P_GENDER, MALE);
 SetProp(P_WEIGHT, 0);
 SetProp(P_NODROP, 1);
 SetProp(P_VALUE, 0);
 AddId(({"hammer", "sehertool","seherhammer","\nseertool"}));
 SetProp(P_NAME, "Seherhammer");
 SetProp(P_MATERIAL, MAT_MAGIC);
 SetProp(P_ARTICLE,1);
 SetProp(P_NOSELL,1);

 SetProp(P_INFO, BS("Der Hammer scheint eine starke magische Kraft zu "
   +"besitzen.\n"));

 stack=({});
 code=({});
 codedet=({});
 saved=0; 
 i=0;
 ausgangerlaubt=0;
 roomname=0; 
 str=0;
 p_long=0; 
 p_short=0;
 p_temp="22";
 p_light="1.0";
 p_env="TT_HOUSE";
 p_indoors="1";

 AddCmd( ({"hstatus"}), "_hammerstatus");
 AddCmd( ({"hhilfe"}), "_hammerhilfe");
 AddCmd( ({"hnew","hneu"}), "_hammerneu");
 AddCmd( ({"hsave","hspeichern"}), "_hammersave");
 AddCmd( ({"hadd","hdo"}), "_do");
 AddCmd( ({"hshow","hcheck"}), "_hammercheck");
 AddCmd( ({"hdel","hdelete","hdele"}), "_hammerdel");
 AddCmd( ({"hload","hladen","hlade"}), "_hammerload");
 AddCmd( ({"hreload"}), "_hammerreload");
 AddCmd( ({"hclean","hclear"}), "_hammerclean");
 AddCmd( ({"hgoto"}), "_hammergoto");
 AddCmd( ({"hshowcode"}), "_hammershowcode");
 AddCmd( ({"hlist","hliste"}), "_hammerlist");
 AddCmd( ({"htypo","htypos"}), "_hammertypoliste");
 AddCmd( ({"hls"}), "_hammerls");
 AddCmd( ({"hman"}), "_hammerman");
 AddCmd( ({"hsethome"}), "_hammersethome");
}

void init()
{
  ::init();
  
  if(sizeof(get_dir("/d/seher/haeuser/"+geteuid(this_player())))==0)
    mkdir("/d/seher/haeuser/"+geteuid(this_player()));
  
  if(sizeof(get_dir("/d/seher/haeuser/"+geteuid(this_player())+"/room"))==0)
    mkdir("/d/seher/haeuser/"+geteuid(this_player())+"/room");
  
  if (!IS_SEER(TP))
  {
     printf("Der Hammer zerstört sich aus unerklärlichen Gründen "
        +"in deiner Hand.\n");
     TO->remove();
  }
}

int _checkallow()
{
  if (!IS_SEER(TP))
  {
     printf("Der Hammer zerstört sich aus unerklärlichen Gründen "
        +"in deiner Hand.\n");
     TO->remove();  
     return 0;
  }

  return 1;
}

string killstr(string str) {
  int i;
  if (strlen(str)) {
    for (i=strlen(str)-1;i>0;i--)
      if (str[i]<32 && str[i]!='\t') str=str[0..i-1]+str[i+1..];
    if (str[0]<32) str=str[1..];
  }
  return str;
}        

int _charcheck(string checkstr)
{
  checkstr=implode(explode(checkstr, "\n"), "\\n" );

  for(i=0;i<strlen(checkstr);i++) {

   if (member(ZEICHEN, extract(checkstr,i,i)) >=0)
   {
//     printf(extract(checkstr,i,i));
   }
   else
   {
//     printf("'"+extract(checkstr,i,i)+"' nicht zulaessig!\n");
     if ((extract(checkstr,i,i+1)!="~q") && (extract(checkstr,i,i+1)!="\n"))
     {
     printf("Ungültiges Zeichen ('"+extract(checkstr,i,i)+"') verwendet! Abgebrochen!\n");
     return 0;
     }
   }
    
  }
  checkstr=implode(explode(checkstr, "\n"), "\\n" );
  return 1; 
}


int _hammerclean()
{
  if (_checkallow()==0) return 1;
  printf("Setzte alle Werte des Hammers zurück!\n");
  printf("Achtung: Alle Änderungen sind verloren gegangen.\n");
  printf("Mit 'hload' bzw 'hnew' kann wieder neu begonnen werden.\n");
  code=({});
  codedet=({});
  saved=0; i=0; ausgangerlaubt=0;
  roomname=0; str=0; p_indoors="1";
  p_long=0; p_short=0; p_light="1.0"; p_env="TT_HOUSE"; p_temp="22"; 
  log("_hammerclean");
  return 1;
}

int _listtypos()
{
  printf("Folgende fehlende Details/Typos/Ideen/Bugs wurden gemeldet:\n\n");
  TP->more(LOGPATH+"typos/"+lower_case(TP->name()));
  return 1;
}       

int _hammertypoliste(string str)
{
 string file;
 int j;

 if ((str=="del") || (str=="delete") || (str=="dele"))
 {
   rm(LOGPATH+"typos/"+lower_case(TP->name()));
   printf("Typo-File gelöscht!\n");
   return 1;
 }

 for (j=0;j<4;j++) {

  switch(j) {
    default:
    case 0: file="/log/report/seher/seher.det"; break;
    case 1: file="/log/report/seher/seher.typo"; break; 
    case 2: file="/log/report/seher/seher.idea"; break;
    case 3: file="/log/report/seher/seher.bug"; break;
  }

  //printf(file+" in progress...\n");
  for(i=1;i<1999;i++) {

  str=read_file(file,i, 1);
  if ((!str) || (strlen(str)<5))
  {
   break;
  }
   write_file(LOGPATH+"typos/"+lower_case(TP->name()), str+"\n");
   i++;
   str=read_file(file,i, 1);
   //printf(";"+i+str+"\n");
   if (str)
   {   
      write_file(LOGPATH+"typos/"+lower_case(TP->name()), str+"\n");   
   }
   i++;
  }
  rm(file); 
 }
 //printf(j+"-"+i+"\n");
 _listtypos();
 return 1;
}

int _hammergoto(string rno)
{
  string DUMPFILE, fina;
  
  if (_checkallow()==0) return 1;   
  if (!_charcheck(rno)) { return 1; }

  printf("WARNUNG: Hoffentlich hast Du gespeichert!\n");
  
  if(TP->InFight())
  {
    printf("Das geht nicht, während du kämpfst!\n");
    return 1;
  }

  if (rno=="home")
  {
     if (environment(TP)->QueryProp(P_SHORT)!="seerroom")
     {
       printf("Du befindest Dich in keinem Seherhaus!\n");
       return 1;
     }

     fina=object_name(ENV(TP));
     if (sizeof(explode(fina, lower_case(TP->Name())))!=2)
     {
       printf("Das geht leider nur in einem Deiner Räume...\n");
       return 1;
     }
     
     if(TP->QueryProp(P_START_HOME))
       TP->move(TP->QueryProp(P_START_HOME), M_TPORT);
     else
       TP->move(TP->QueryDefaultHome(), M_TPORT);
     return 1;
  }

  str=rno;
  DUMPFILE=PATH+"/"+str+".c";

  str=read_file(DUMPFILE,0,1);
  if (str) { str=extract(str,0,9); }
  else { str="."; }     
  str=extract(str,0,9);
  if (str!="/*SEERTOOL")
  // Pruefen ob so ein File schon existiert...
  {
    printf("Error: File existiert nicht oder ist kein kompatibles "
          +"Sourcefile.\n");
    return 1;
  }

  TP->move(DUMPFILE, M_TPORT);
  printf("moved to: Raum "+rno+".\n");
  return 1;
}

int _hammershowcode()
{
 if (_checkallow()==0) return 1;           

 if (!roomname)
 {
   printf("Kein Raum geladen...\n");
   return 1;
 }
 if (!saved)
 {
   printf("Bitte zuerst speichern!\n");
   return 1;
 }
 printf("Zeige Raum: "+roomname+" \n\n");
 TP->more(PATH+"/"+roomname+".c");

 return 1;
}

int _hammerload(string rno)
{
  int ready, j, raumnr;
  raumnr = 0;
  string DUMPFILE, stra, strb;
  string *strc;
  if (_checkallow()==0) return 1;   
  log("_hammerload: "+to_string(rno));
  roomname=rno;
  DUMPFILE=PATH+"/"+roomname+".c";
  ready=0;
  
  if(!rno)
  {
    printf("Syntax: 'hammerload <raumnr>'\n");
    return 1;
  }
  
  if (!_charcheck(rno)) { return 1; } 

  if (strlen(rno)>3)
  {
    printf("Raumnummer zu groß!\n");
    return 1;
  }

  str=read_file(DUMPFILE,0,1);
  if (str) { str=extract(str,0,9); }
  else { str="."; }
  if (str!="/*SEERTOOL")
  // Pruefen ob so ein File schon existiert...
  {
    printf("Error: File existiert nicht oder ist kein kompatibles "
          +"Sourcefile.\n");
    return 1;
  }
  j=0;
  code=({});
  for (i=1;i<49999;i++) {
  
   if ( (!str=read_file(DUMPFILE, i, 1)) || (str=="//ENDCODE"))
       { 
//         save_object(BUGFILE);
         printf("Raum geladen.\n");
         return 1; 
       }
  
   if (ready)
   { 
     if (extract(str,0,18)=="SetProp(P_INT_SHORT")
     {
       p_short=extract(str,22,strlen(str)-5);
       printf("P_SHORT found!\n");
     }
 
     if (extract(str,0,17)=="SetProp(P_INT_LONG")
     {
       p_long=extract(str,21,strlen(str)-5);
       printf("P_LONG found!\n");
     }

     if (extract(str,0,14)=="SetProp(P_LIGHT")
     {
       p_light=extract(str,17,strlen(str)-4);
       printf("P_LIGHT found! "+p_light+"\n");
     }

     if (extract(str,0,16)=="SetProp(P_INDOORS")
     {
       p_indoors=extract(str,19,strlen(str)-4);
       printf("P_INDOORS found! "+p_indoors+"\n");
     }

     if (extract(str,25,39)=="ENV_TEMPERATURE")
     {
       p_temp=extract(str,42,45);
       printf("TEMPERATURE found! "+p_temp+"\n");
     }

     if (extract(str,48,58)=="ENV_TERRAIN")
     {
       p_env=extract(str,61,strlen(str)-7);
       printf("TERRAIN found! "+p_env+"\n");
     }

     if (extract(str,0,8)=="AddDetail")
     {
       //printf("Detail found!\n");
       stra=extract(str,13,strlen(str)-5);
       strb=read_file(DUMPFILE, ++i, 1);
       strb=extract(strb,1,strlen(strb)-5);
       codedet=({"2"})+({strb});
       strc=explode(stra, "\"");
       for(j=0;j<sizeof(strc);j++) {
        if (strc[j]!=",")
        {
          codedet+=({strc[j]}); 
        }
       }
       code+=({codedet});
       str=""; stra=""; strb=""; strc=({}); j=0;
       codedet=({});
     }

     if (extract(str,0,7)=="AddSound")
     {
      // printf("Sound found!\n");
       if ((stra=extract(str,10,22))!="DEFAULT_SOUND")
       {
       stra=extract(str,13,strlen(str)-5);
       }
       strb=read_file(DUMPFILE, ++i, 1);
       strb=extract(strb,1,strlen(strb)-5);
       codedet=({"3"})+({strb});
       strc=explode(stra, "\"");
       for(j=0;j<sizeof(strc);j++) {
        if (strc[j]!=",")
        {
          codedet+=({strc[j]});
        }
       }
       code+=({codedet});
       str=""; stra=""; strb=""; strc=({}); j=0;
       codedet=({});
     }

     if (extract(str,0,7)=="AddSmell")
     {
      // printf("Smell found!\n");
       if ((stra=extract(str,10,22))!="DEFAULT_SMELL") 
       {
        stra=extract(str,13,strlen(str)-5);
       }
       strb=read_file(DUMPFILE, ++i, 1);
       strb=extract(strb,1,strlen(strb)-5);
       codedet=({"4"})+({strb});
       strc=explode(stra, "\"");
       for(j=0;j<sizeof(strc);j++) {
        if (strc[j]!=",")
        {
          codedet+=({strc[j]});
        }
       }
       code+=({codedet});
       str=""; stra=""; strb=""; strc=({}); j=0;
       codedet=({});
     }

     if (extract(str,0,12)=="AddReadDetail")
     {
      // printf("Read-Detail found!\n");
       stra=extract(str,17,strlen(str)-5);
       strb=read_file(DUMPFILE, ++i, 1);
       strb=extract(strb,1,strlen(strb)-5);
       codedet=({"5"})+({strb});
       strc=explode(stra, "\"");
       for(j=0;j<sizeof(strc);j++) {
        if (strc[j]!=",")
        {
          codedet+=({strc[j]});
        }
       }
       code+=({codedet});
       str=""; stra=""; strb=""; strc=({}); j=0;
       codedet=({});
     }

     if (extract(str,0,6)=="AddExit")
     {
      // printf("Ausgang found!\n");
       stra=extract(str,11,strlen(str)-5);
       strb=read_file(DUMPFILE, ++i, 1);
       strb=extract(strb,1,strlen(strb)-5);
       codedet=({"6"})+({strb});
       strc=explode(stra, "\"");
       for(j=0;j<sizeof(strc);j++) {
        if (strc[j]!=",")
        {
          codedet+=({strc[j]});
        }
       }
       code+=({codedet});
       str=""; stra=""; strb=""; strc=({}); j=0;
       codedet=({});
     }


   }

   if (extract(str,0,10)=="//BEGINCODE") { ready=1; printf("Ready.\n"); }
  }
  return 1;
}

int _hammerdel(string eingabe)
{
  string DUMPFILE;

  if (!eingabe)
  {
    printf("Ungültige Eingabe!\n");
    printf("  Syntax: hdel <datensatznummer>\n");
    printf("          hdel raum\n");
    return 1;
  } 

  DUMPFILE=PATH+"/"+roomname+".c";

  if (ENV(TP)!=find_object(DUMPFILE))
  {
    printf("Du bist leider nicht in dem aktuell geladenen Raum!\n");
    printf("Bitte bearbeite nur den Raum in dem Du stehst!\n");
    return 1;
  } 

  if (_checkallow()==0) return 1;   
  if (!_charcheck(eingabe)) { return 1; } 
  saved=0;
  log("_hammerdel: "+eingabe);
  if ( (eingabe=="raum") || (eingabe=="Raum") || (eingabe=="RAUM") || 
       (eingabe=="room") )
  {
    rm(DUMPFILE);
    printf("Das Source-File des aktuellen Raumes ("+roomname+
          ") wurde gelöscht.\n");
    roomname="";
    saved=0;
    code=({});
    codedet=({});
    saved=0;
    str=0;
    p_long=0;
    p_short=0;
    return 1;
  }
  i=to_int(eingabe);
  if (i>sizeof(code))
  {
    printf("Datensatznummer zu groß. So ein Datensatz existiert nicht.\n");
    return 1;
  }
  if (sizeof(code)==0)
  {
    printf("Keine Datensätze da, die gelöscht werden könnten.\n");
    return 1;
  }
  code[i]=({"del"});
  TO->_cleancode();
  printf("Datensatz wurde gelöscht.\n");
  return 1;
}

int _hammerlist()
{
 string text,stra;
 
 if (strlen(roomname)<1)
  {
   printf("Kein aktiver Raum -> hload oder hnew aufrufen!\n");
   return 1;
  } 

 text=0;

 text =" Seherhaus von "+TP->name()+". Erstellt mit dem Seherhammer\n";
 text+="+-------------------------------+--------------------------------+\n";
 stra ="| Raumname: "+roomname;
 text+=sprintf("%-32s| ",stra);
 stra =" Temperatur: "+p_temp+" °C";
 text+=sprintf("%-31s|\n",stra);
 stra ="| Licht: "+p_light;
 text+=sprintf("%-32s| ",stra);
 stra =" Umgebung: "+p_env;
 text+=sprintf("%-31s|\n",stra);
 stra ="| Innen/Außen: "+(p_indoors=="1"? "Innenraum":"Außenraum");
 text+=sprintf("%-32s| ",stra);
 stra =" Anzahl Details: "+sizeof(code);
 text+=sprintf("%-31s|\n",stra);
 text+="+-------------------------------+--------------------------------+\n";
 text+="Kurzbeschreibung des Raumes:\n\n"+p_short+"\n\n";
 text+="Langbeschreibung des Raumes:\n\n"+p_long+"\n\n";
 text+="+----------------------------------------------------------------+\n";
 text+=" Datensatz Nr                                        Datensatztyp\n";
 text+=" Keys / Beschreibung\n";
 text+="+----------------------------------------------------------------+\n";

 if (sizeof(code)==0)
 {
   printf("Keine Detail-Datensätze vorhanden...\n");
   return 1;
 }
 for (i=0;i<sizeof(code);i++) {
   text+=(sprintf(" # %-4d",i));

  codedet=({})+code[i];
   switch(codedet[0]) {
    case "2" :
      stra="Detail";
      break;
    case "3" :
      stra="Geräusch";
      break;
    case "4" :
      stra="Geruch";
      break;
    case "5" :
      stra="Read-Detail";
      break;
    case "6" :
      stra="Ausgang";
   }
   text+=sprintf("%58s\n",stra);
   text+=" Keys:\n ";
   for (int j=2;j<sizeof(codedet);j++) {
     text+=(codedet[j]+", ");
   }
   text+="\n"+("\n"+implode(explode(codedet[1], "\\n"), "\n" )+"\n");
 text+="+----------------------------------------------------------------+\n";

 }
 text+="\n\n-- ende --\n\n";

 TP->More(text);
 return 1;

}

int _hammercheck()
{
 int weiter;
 
 if (_checkallow()==0) return 1;  
 
 if (strlen(roomname)<1)
  {
   printf("Kein aktiver Raum -> hload oder hnew aufrufen!\n");
   return 1;
  } 

 printf("Kurzbeschreibung: ");
 printf(p_short+"\n\n");
 printf("Langbeschreibung:\n");
 printf(implode(explode(p_long, "\\n"), "\n" )+"\n\n");
 printf("Umgebung: "+p_env+"\n");
 printf("Temperatur: "+p_temp+"\n");
 printf("In- (1) / Outdoor (0): "+p_indoors+"\n");
 printf("Hell (1.0) / Dunkel (0.0): "+p_light+"\n");

 if (sizeof(code)==0)
 {
   printf("Keine Code-Zeilen vorhanden...\n");
   return 1;
 }
 for (i=0;i<sizeof(code);i++) {
   write(sprintf("Datensatz Nr.: %d  - ",i));
   TO->_list();
//   input_to("_list");
 }
 return 1;
}

int _list(string eingabe)
 {
   codedet=({})+code[i];
//   tell_object(TP,BS("codedet[0]="+ codedet[0] +";\n"));
   switch(codedet[0]) {
    case "2" :
      printf("Detail\n");
      break;
    case "3" :
      printf("Geräusch\n");
      break;
    case "4" :
      printf("Geruch\n");
      break;
    case "5" :
      printf("Read-Detail\n");
      break;
    case "6" :
      printf("Ausgang\n");
   }

 printf("Keys: ");
 for (int j=2;j<sizeof(codedet);j++) {
  printf(codedet[j]+", "); 
 }
 printf("\n"+implode(explode(codedet[1], "\\n"), "\n" )+"\n\n");
 return 1;
}

void _cleancode()
{
  string **tempcode;
  string *tempcodedet;
  string DUMPFILE;
 
  tempcode=({});
  tempcodedet=({});

  for(i=0;i<sizeof(code);i++)
  {
     if (code[i]!=({"del"})) 
     {    
       tempcodedet=({})+code[i];
       tempcode+=({code[i]});
     }
  }
  code=tempcode;
}

int _do(string auswahl)
{
  string DUMPFILE;
//  if (!_charcheck(auswahl)) { return 1; } 
  saved=0;
  if (_checkallow()==0) return 1;  
 
  if (strlen(roomname)<1)
  {
   printf("Kein aktiver Raum -> hload oder hnew aufrufen!\n");
   return 1;
  } 
 
  if (strlen(auswahl)==0)
  {
    printf("Zu wenig Parameter eingegeben.\n");
    return 1;
  }

  DUMPFILE=PATH+"/"+roomname+".c";
 
  if (ENV(TP)!=find_object(DUMPFILE))
  {
    printf("Du bist leider nicht in dem aktuell geladenen Raum!\n");
    printf("Bitte bearbeite nur den Raum in dem Du stehst!\n");
    return 1;
  }       
 
  log("_do: "+auswahl);
  if ((auswahl=="0") || (auswahl=="kurz") || (auswahl=="short"))
  {
    printf("Kurzbeschreibung:\n");
    input_to("_p_short");
    return 1;
  }

  if ((auswahl=="1") || (auswahl=="lang") || (auswahl=="long"))
  {
    printf("** oder . wenn Du fertig bist, ~q zum Abbrechen, ~h für eine Hilfsseite.\n");
    printf("Langbeschreibung:\n");
    nedit("_p_long",0); 
//    input_to("_p_long");
    return 1;
  }

  if ((auswahl=="2") || (auswahl=="adddetail") || (auswahl=="detail"))
  {
    printf("** oder . wenn Du fertig bist, ~q zum Abbrechen, ~h für eine Hilfsseite.\n");
    printf("Detailbeschreibung:\n");
    codedet=({"2"});
    nedit("_detail",0);
//    input_to("_detail");
    return 1;
  }

  if ((auswahl=="3") || (auswahl=="addsound") || (auswahl=="sound"))
  {
    printf("** oder . wenn Du fertig bist, ~q zum Abbrechen, ~h für eine Hilfsseite.\n");
    printf("Geräuschbeschreibung:\n");
    codedet=({"3"});
    nedit("_s_detail",0);
//    input_to("_s_detail");
    return 1;
  }

  if ((auswahl=="4") || (auswahl=="addsmell") || (auswahl=="smell"))
  {
    printf("** oder . wenn Du fertig bist, ~q zum Abbrechen, ~h für eine Hilfsseite.\n");
    printf("Geruchsbeschreibung:\n");
    codedet=({"4"});
    nedit("_s_detail",0);
//    input_to("_s_detail");
    return 1;
  }

  if ((auswahl=="5") || (auswahl=="addreaddetail") || (auswahl=="readdetail"))
  {
    printf("** oder . wenn Du fertig bist, ~q zum Abbrechen, ~h für eine Hilfsseite.\n");
    printf("Read-Detail-Beschreibung:\n");
    codedet=({"5"});
    nedit("_detail",0);
//    input_to("_detail");
    return 1;
  }
 
  if ((auswahl=="6") || (auswahl=="addexit") || (auswahl=="exit"))
  {
    printf("Bitte Raumnummer oder Sourcefile des anderen Raumes angeben:\n");
    codedet=({"6"});
    input_to("_ausgang");
    return 1;
  }

  if ((auswahl=="7") || (auswahl=="light") || (auswahl=="licht"))
  {
    printf("Bitte Lichtlevel eingeben 0.0=Dunkel, 1.0=Hell:\n");
    input_to("_licht");
    return 1;
  }

  if ((auswahl=="8") || (auswahl=="temp") || (auswahl=="temperatur"))
  {
    printf("Bitte Temperatur eingeben:\n");
    input_to("_temperatur");
    return 1;
  }

  if ((auswahl=="9") || (auswahl=="indoor") || (auswahl=="indoors"))
  {
    printf("Bitte auswählen: 0=draußen, 1=drinnen:\n");
    input_to("_indoors");
    return 1;
  }

  if ((auswahl=="env") || (auswahl=="umgebung") || (auswahl=="environment"))
  {
    printf("Bitte auswählen: \n");
    printf("    Höhle      ... 1        Dschungel    ... 8\n"+
           "    Haus       ... 2        Sumpf        ... 9\n"+
           "    Hügel      ... 3        Flachland    ... 10\n"+ 
           "    Wald       ... 4        Arktis       ... 11\n"+
           "    Stadt/Dorf ... 5        Unterwasser  ... 12\n"+
           "    Gebirge    ... 6        Straße/Weg   ... 13\n"+
           "    Wüste      ... 7        Tundra       ... 14\n"+
           "    Anders     ... 0        Unter Wasser ... 15\n\n  -> : ");
    input_to("_env");
    return 1;
  }

 printf("Keine oder ungültige Auswahl...\n");
 return 1;
}

int _env(string eingabe)
{
  int enr=0;
  if (!_charcheck(eingabe)) { return 1; }
  if (eingabe=="~q")
  {
    printf("Abgebrochen!\n");
    codedet=({});
    return 1;
  }
  
  sscanf(eingabe,"%d",enr);

  if ((enr<0) || (enr>15))
  {
    printf("Ungültige Umgebung ausgewählt!\n");
    return 1;
  }
   switch(enr) {
     case 0: p_env="TT_OTHER"; break;
     case 1: p_env="TT_CAVE"; break;
     case 2: p_env="TT_HOUSE"; break;
     case 3: p_env="TT_HILL"; break;
     case 4: p_env="TT_FOREST"; break;
     case 5: p_env="TT_TOWN"; break;
     case 6: p_env="TT_MOUNTAIN"; break;
     case 7: p_env="TT_DESERT"; break;
     case 8: p_env="TT_JUNGLE"; break;
     case 9: p_env="TT_SWAMP"; break;
     case 10: p_env="TT_PLAIN"; break;
     case 11: p_env="TT_ARCTIC"; break;
     case 12: p_env="TT_UNDERWATER"; break;
     case 13: p_env="TT_ROAD"; break;
     case 14: p_env="TT_TUNDRA"; break;
     case 15: p_env="TT_WATER"; break;
  }
    printf("Umgebung auf "+p_env+" gesetzt...\n");
    return 1;
}

int _indoors(string eingabe)
{
  if (!_charcheck(eingabe)) { return 1; }
  if (eingabe=="~q")
  {
    printf("Abgebrochen!\n");
    codedet=({});
    return 1;
  }
  if ((eingabe!="1") && (eingabe!="0"))
  {
    printf("Ungültigen Indoor-Bereich eingeben.\n");
    return 1;
  }
    p_indoors=eingabe;
    if (p_indoors=="1")
    {
     printf("Drinnen...\n");
    }
    else
    {
     printf("Draußen...\n");
    }
    return 1;
}

int _temperatur(string eingabe)
{
  if (!_charcheck(eingabe)) { return 1; }
  if (eingabe=="~q")
  {
    printf("Abgebrochen!\n");
    codedet=({});
    return 1;
  }
    i=to_int(eingabe);
    if ((i<7) && (i>40))
    {
      printf("Ungültigen Temperaturbereich eingegeben.\n");
      return 1;
    }

    p_temp=eingabe;
    printf("Temperatur von "+p_temp+" Grad eingestellt...\n");
    return 1;
}


int _licht(string eingabe)
{
  if (!_charcheck(eingabe)) { return 1; }
  if (eingabe=="~q")
  {
    printf("Abgebrochen!\n");
    codedet=({});
    return 1;
  }
  if ((float)eingabe<0.0 || (float)eingabe>1.0)
  {
    printf("Ungültigen Lichtlevel eingeben.\n");
    return 1;
  }
    p_light=eingabe;
    printf("Lichtlevel "+p_light+" eingefügt...\n");
    return 1;
}

int _ausgang(string eingabe)
{
  if (!_charcheck(eingabe)) { return 1; } 
  if ((eingabe=="~q") || (strlen(eingabe)==0))
  {
    printf("Abgebrochen!\n");
    codedet=({});
    return 1;
  }

  if ((strlen(eingabe)==1) || (strlen(eingabe)==2))
  {
    eingabe=PATH+"/"+eingabe;
  }
  if (!read_file(eingabe+".c"))
  {
    codedet=({});
    printf("Raum existiert nicht! Abgebrochen.\n");
    return 1;
  }
  ausgangerlaubt=0;
  if (strstr(eingabe, (PATH+"/") )!=-1)
  {
     ausgangerlaubt=1;
  }

    codedet=codedet+({eingabe});
    printf("Bitte die Richtung des Ausganges angeben (osten, oben ...):\n");
    input_to("_ausgang_a");
    return 1;
}

int _ausgang_a(string eingabe)
{
  if (!_charcheck(eingabe)) { return 1; } 
  eingabe=lower_case(eingabe);
  if (eingabe=="~q")
  {
    printf("Abgebrochen!\n");
    codedet=({});
    return 1;
  }

    codedet=codedet+({eingabe});
    if (ausgangerlaubt) 
    { 
      code+=({codedet}); 
      printf("Ausgang hinzugefügt.\n");
    }
    else 
    {
     tell_room(ENV(TP),BS
       ("Dieser Ausgang muss von einem anwesenden Magier bestätigt werden.\n"));
     tell_room(ENV(TP),BS
       ("Ausgang: "+codedet[1]+".c\n"));
    tell_room(ENV(TP),BS
       ("Wenn der Anschluss erlaubt wird bitte 'xcall <seher>.<id>->okay();' eingeben.\n"));
      ausgangerlaubt=-1;
    }
    return 1;
}

int okay()
{
    if (ausgangerlaubt!=-1)
    {
       tell_room(ENV(TP),BS("Derzeit ist um keinen Ausgang angesucht worden...\n"));
       return 1;
    }
    if (!IS_LEARNER(TP))
    { 
       printf("Nur Magier dürfen das!\n");
       return 1;
    }

    ausgangerlaubt=0;
    code+=({codedet});
    tell_room(ENV(TP),BS
       (" *** Ausgang hinzugefügt. *** \n"));
    write_file("/log/allow_seer_exit.txt",dtime(time())+": "+TP->Name()+
          " hat "+codedet[1]+" in "+roomname+" erlaubt.\n");
    codedet=({});

    return 1;
}


int _detail(string eingabe)
{
  if (!_charcheck(eingabe)) { return 1; } 
  if (eingabe=="~q")
  {
    printf("Abgebrochen!\n");
    codedet=({});
    return 1;
  }

    codedet=codedet+({eingabe});
    printf("Key eingeben. Ende mit '.' Abbruch mit '~q':\n");
    input_to("_keygen");
    return 1;
}

int _s_detail(string eingabe)
{
//  if (!_charcheck(eingabe)) { return 1; } 
//  eingabe=lower_case(eingabe);
  if (eingabe=="~q")
  {
    printf("Abgebrochen!\n");
    codedet=({});
    return 1;
  }

    codedet=codedet+({eingabe});
    printf("Key eingeben. Ende mit '.', Default mit 'D' Abbruch mit '~q':\n");
    input_to("_s_keygen");
    return 1;
}


int _keygen(string eingabe)
{
  if (!_charcheck(eingabe)) { return 1; }
  eingabe=lower_case(eingabe);
  if (eingabe=="~q")
  {
    printf("Abgebrochen!\n");
    codedet=({});
    return 1;
  }

  if (eingabe!=".")
  {
    codedet+=({eingabe});
    printf("Key eingeben. Ende mit '.' Abbruch mit '~q':\n");
    input_to("_keygen");
  }
  else
  {
   if (sizeof(codedet)<3)
   {
     printf("Keine Key-Wörter eingegeben - Abgebrochen.\n");
     codedet=({});
     return 1;
   }
   code+=({codedet});
   printf("Eingabe der Keys beendet und Code hinzugefügt.\n");
  }   
 return 1;
}

int _s_keygen(string eingabe)
{
  if (!_charcheck(eingabe)) { return 1; }
  eingabe=lower_case(eingabe);
  if (eingabe=="~q")
  {
    printf("Abgebrochen!\n");
    codedet=({});
    return 1;
  }

  if ((eingabe=="D") || (eingabe=="d"))
  { 
    switch(codedet[0]) {
     case "3": 
       codedet+=({"DEFAULT_SOUND"});
       break;
     case "4":
       codedet+=({"DEFAULT_SMELL"});
       break;

     printf("Default nicht möglich da weder Sound noch Smell!\n");
     input_to("_s_keygen");
     return 1;
    }
    code+=({codedet});
    printf("Eingabe der (Default-) Keys beendet und Code hinzugefügt.\n");
    return 1;
  }

  if (eingabe!=".")
  {
    codedet+=({eingabe});
    printf("Key eingeben. Ende mit '.'  Abbruch mit '~q':\n");
    input_to("_keygen");
  }
  else
  {
   if (sizeof(codedet)<3)
   {
     printf("Keine Key-Wörter eingegeben - Abgebrochen.\n");
     codedet=({});
     return 1;
   }
   code+=({codedet});
   printf("Eingabe der Keys beendet und Code hinzugefügt.\n");
  }
 return 1;
}


int _p_long(string eingabe)
{ 
  if (!_charcheck(eingabe)) { return 1; }
//  eingabe=killstr(eingabe);
/*
  if (!sizeof(explode(eingabe,"\n")-
              ({"q","quit"," **","** ","ende","","exit"," "})
              ) )
  {
     printf("testfehler...\n");
     return 1;
  }
*/            
  if (eingabe=="~q")
  {
    printf("Abgebrochen!\n");
    codedet=({});
    return 1;
  }
 
    p_long=eingabe;
    printf("Langbeschreibung eingefügt...\n");
    return 1;
}

int _p_short(string eingabe)
{
  if (!_charcheck(eingabe)) { return 1; } 
  if (eingabe=="~q")
  {
    printf("Abgebrochen!\n");
    codedet=({});
    return 1;
  }

    p_short=eingabe;
    printf("Kurzbeschreibung eingefügt...\n");
    return 1;
}

int _hammerneu(string rno)
{
  int raumnr;
  raumnr=0;
  
  if(rno)
    sscanf(rno,"%d",raumnr);
  
  if(raumnr<1 || raumnr>99)
  {
    write("Du musst eine Raumnummer zwischen 1 und 99 angeben!\n");
    return 1;
  }
  
  string DUMPFILE;
  if (!_charcheck(rno)) { return 1; } 
  DUMPFILE=PATH+"/"+rno+".c";
  saved=0;
  log("_hneu: "+to_string(rno));
  printf("-> hclean\n");
  TO->_hammerclean();
  printf("-> hclean done\n\n");
  if (_checkallow()==0) return 1;   
  if (str=read_file(DUMPFILE,0,10))
  // Pruefen ob so ein File schon existiert...
  {
    printf("Error: File existiert bereits. Bitte mit 'hammerload' laden oder "
     "falls\nnicht mehr benötigt mit 'hammerdel room' löschen.\n");
    return 1;
  }

  write_file(DUMPFILE, "/*SEERTOOL\n  Seher-Haus von "+
     CAP(TP->query_real_name())+"\n"+
     "  Erschaffen mit Hilfe von Aloys 'seertool'.\n\n"+
     "  "+dtime(time())+"*/\n\n"+
     "#pragma strong_types\n\n"+
     "inherit \"/std/room\";\n\n"+ 
     "#include <moving.h>\n"+
     "#include <player.h>\n"+
     "#include <properties.h>\n"+
     "#include <language.h>\n\n"+
     "void create()\n{\n::create();\n   SetProp(P_LIGHT, 1.0);\n"+
     " SetProp(P_SHORT, \"seerroom\");\n"+
     "//BEGINCODE\n");

  write_file(DUMPFILE, "\n//ENDCODE\n}");
  roomname=rno;
  printf("Neuen Raum "+rno+" angelegt.\n");
  printf("Um den neuen Raum zu bearbeiten bitte in diesen gehen!.\n");
  return 1;
}

int _hammersave()
{
  int j;
  string DUMPFILE;

  if (_checkallow()==0) return 1;   
  DUMPFILE=PATH+"/"+roomname+".c";
 
  if (ENV(TP)!=find_object(DUMPFILE))
  {
    printf("Du bist leider nicht in dem aktuell geladenen Raum!\n");
    printf("Bitte bearbeite nur den Raum in dem Du stehst!\n");
    return 1;
  }       

  if (strlen(roomname)<1)
  {
    printf("Kein Raum geöffnet.\n");
    return 1;
  }

 if (strlen(p_long)<1)
  {
    printf("Langbeschreibung fehlt! Bitte zuerst diese eingeben.\n");
    return 1;
  }

  if (strlen(p_short)<1)
  {
    printf("Kurzbeschreibung fehlt! Bitte zuerst diese eingeben.\n");
    return 1;
  }
  log("_hammersave");
//  mv(DUMPFILE, DUMPFILE+".old");
//  cp(DUMPFILE, DUMPFILE+".old");
  rm(DUMPFILE);

  write_file(DUMPFILE, "/*SEERTOOL\n  Seher-Haus von "+
     CAP(TP->query_real_name())+"\n"+
     "  Erschaffen mit Hilfe von Aloys 'seertool'.\n\n"+
     "  "+dtime(time())+"*/\n\n"+
     "#pragma strong_types\n\n"+
     "inherit \"/std/room\";\n\n"+
     "#include <moving.h>\n"+
     "#include <player.h>\n"+
     "#include <properties.h>\n"+
     "#include <language.h>\n\n"+
     "void create()\n{\n::create();\n"+
     "SetProp(P_SHORT, \"seerroom\");\n"+
     "//BEGINCODE\n");

     write_file(DUMPFILE,"SetProp(P_INT_SHORT, \""+p_short+"\");\n");
     write_file(DUMPFILE,"SetProp(P_INT_LONG, \""+
                         (implode(explode(p_long, "\n"), "\\n" ))+"\");\n");
     write_file(DUMPFILE,"SetProp(P_LIGHT, "+p_light+");\n");
     write_file(DUMPFILE,"SetProp(P_INDOORS, "+p_indoors+");\n");
     str="";
     for(;(strlen(str)+strlen(p_temp))<5;) { str+=" "; }
         // Anpassen fuer Einlesen !!
     write_file(DUMPFILE,"SetProp(P_ENVIRONMENT, ([ENV_TEMPERATURE: "+p_temp+
        str+",ENV_TERRAIN: "+p_env+"]) );\n");

 if (sizeof(code)>0)
 {
  for(j=0;j<sizeof(code);j++)
  {
    codedet=({})+code[j];

    switch(codedet[0]) {
      case "2":
       write_file(DUMPFILE,"AddDetail( ({");
        for(i=2;i<sizeof(codedet);i++) { 
         write_file(DUMPFILE, "\""+codedet[i]+"\""); 
         if (i+1<sizeof(codedet)) { write_file(DUMPFILE, ","); }
         }
       write_file(DUMPFILE,"}),\n");
       write_file(DUMPFILE,"\""+(implode(explode(codedet[1], "\n"), "\\n" ))+"\"");
       write_file(DUMPFILE,");\n");
       break;
      case "3":
       write_file(DUMPFILE,"AddSound( ");
        if (codedet[2]=="DEFAULT_SOUND")
        {
          write_file(DUMPFILE, "DEFAULT_SOUND ,\n");
        }
        else
        {
         write_file(DUMPFILE, "({"); 
          for(i=2;i<sizeof(codedet);i++) {
           write_file(DUMPFILE, "\""+codedet[i]+"\""); 
           if (i+1<sizeof(codedet)) { write_file(DUMPFILE, ","); }
          }
         write_file(DUMPFILE,"}),\n");
        }
       write_file(DUMPFILE,"\""+(implode(explode(codedet[1], "\n"), "\\n" ))+"\"");
       write_file(DUMPFILE,");\n");
       break;
      case "4":
       write_file(DUMPFILE,"AddSmell( ");
        if (codedet[2]=="DEFAULT_SMELL")
        {
          write_file(DUMPFILE, "DEFAULT_SMELL ,\n");
        }
        else
        {
          write_file(DUMPFILE, "({");
          for(i=2;i<sizeof(codedet);i++) {
            write_file(DUMPFILE, "\""+codedet[i]+"\"");
            if (i+1<sizeof(codedet)) { write_file(DUMPFILE, ","); }
          }
         write_file(DUMPFILE,"}),\n");
        }
       write_file(DUMPFILE,"\""+(implode(explode(codedet[1], "\n"), "\\n" ))+"\"");
       write_file(DUMPFILE,");\n");
       break;
      case "5":
       write_file(DUMPFILE,"AddReadDetail( ({");
        for(i=2;i<sizeof(codedet);i++) {
         write_file(DUMPFILE, "\""+codedet[i]+"\"");
         if (i+1<sizeof(codedet)) { write_file(DUMPFILE, ","); }
         }
       write_file(DUMPFILE,"}),\n");
       write_file(DUMPFILE,"\""+(implode(explode(codedet[1], "\n"), "\\n" ))+"\"");
       write_file(DUMPFILE,");\n");
       break;
      case "6":
        write_file(DUMPFILE,"AddExit( ({");
        for(i=2;i<sizeof(codedet);i++) {
         write_file(DUMPFILE, "\""+codedet[i]+"\"");
         if (i+1<sizeof(codedet)) { write_file(DUMPFILE, ","); }
         }
       write_file(DUMPFILE,"}),\n");
       write_file(DUMPFILE,"\""+(implode(explode(codedet[1], "\n"), "\\n" ))+"\"");
       write_file(DUMPFILE,");\n");
       break;

     }
    
  }
}
  write_file(DUMPFILE, "\n//ENDCODE\n}");
 saved=1;
 save_object(BUGFILE);
 printf("saved...\n");
 return 1;
}

int _hammerhilfe()
{
  string text;

  text=  "\n  Befehlsreferenz des Seher-Hammers:\n\n";
  text+= "  hnew <nr>   :   neuen Raum anlegen.\n";
  text+= "  hload <nr>  :   Raum <nr> laden (1-99).\n";
  text+= "  hsave       :   aktuellen Raum speichern.\n";
  text+= "  hstatus     :   Status des Hammers anzeigen.\n";
  text+= "  hadd        :   hinzufügen von:\n"
	+"                    long, short: Lang- bzw Kurzbeschreibung\n"
        +"                    detail     : Raumdetail\n"
        +"                    sound      : Geräusch\n"
        +"                    smell      : Geruch\n"
        +"                    readdetail : etwas zum lesen\n"
        +"                    exit       : Ausgang\n"
        +"                    temp       : Temperatur setzen\n"
        +"                    env        : Umgebung definieren\n"
        +"                    licht      : Licht an/aus\n"
        +"                    indoor     : drinnen/drausen setzen\n";
  text+= "  hshow       :   Ausgabe aller Details.\n";
  text+= "  hlist       :   Ausgabe mit 'more'.\n";
  text+= "  hdel        :   löschen des Raumes -> 'hdel room'\n"
        +"                  löschen eines Details -> 'hdel <nr>'\n"
        +"                    (Die Nummer kann von hshow entnommen werden!)\n";
  text+= "  hreload     :   den Raum neu laden.\n";
  text+= "  hclean      :   Alle Werte zurücksetzen.\n";
  text+= "  htypo       :   Logfile mit Typos/Ideas/Bugs ansehen.\n";
  text+= "                  htypo del -> Logfile löschen.\n";
  text+= "  hgoto <nr>  :   In Raum <nr> bringen.\n"
        +"                  mit 'hgoto home' zum Rasenstartpunkt.\n";
  text+= "  hls         :   Alle Räume auflisten\n";
  text+= "  hshowcode   :   Zeigt den LPC Code an\n";
  text+= "  hman        :   ausführliche Anleitung lesen\n";
  text+= "  hsethome    :   Einen Raum als Startpunkt setzen\n"
         "                  <reset> resetet den Startpunkt\n\n";
  TP->More(text);
  return 1;
}

int _hammerstatus()
{
  if (_checkallow()==0) return 1;   
  printf("Derzeitige Einstellungen:\n");
  str=roomname;
  if (strlen(str)==0) str="keiner";
  printf("  Geöffneter Raum: "+ANSI_GREEN+str+ANSI_NORMAL+"\n");
  printf("  Gespeichert     : "+(saved ? ANSI_GREEN+"Ja":ANSI_RED+"Nein")+
         ANSI_NORMAL+"\n");
  printf(ANSI_NORMAL+"\n");
  return 1;
}

// Teile aus der LUPE!


int destroyable(object ob)
{
  if (!ob)
    return 0;
  if (query_once_interactive(ob))
    return 0;
  if (ob==this_object())
    return 0;
  return 1;
}

int push(object ob)
{
  stack+=({ob});
  if (sizeof(stack)>MAXSTACK)
    stack=stack[1..MAXSTACK];
  return 1;
}

void Remove(object ob,int a)
{
  if (!objectp(ob)) return;
  if (!a)
  {
    printf("Removing %O",ob);
//    if (!hide_short) printf(" %O",ob->name());
  }
  catch(ob->remove());
  if (ob)
  {
    if (!a) printf(" HARD");
    destruct(ob);
  }
  write("\n");
}

string desc(object ob)
{
  if (!ob)
    return "<destructed object>";
  if (!objectp(ob))
    return "<corrupted stack entry>";
  if (query_once_interactive(ob))
    return object_name(ob)+" "+capitalize(geteuid(ob));
  // if (!hide_short && ob->short())
  //   return object_name(ob)+" "+ob->name();
  else
    return object_name(ob);
}

varargs string make(string arg)
{
  object *list, ob, env;
  string file,temp,dummy;
  int i,cloned;

//  ob=pop();
  ob=ENV(TP);
  if (err) return arg;
  if (!destroyable(ob))
  {
    err="can't update "+desc(ob);
    return arg;
  }
  env=environment(ob);
  file=object_name(ob);
  write("Updating "+object_name(ob)+"...\n");
  if (sscanf(file,"%s#%s",temp,dummy)==2)
  {
    file=temp;
    cloned=1;
  }
  else
    cloned=0;
  list=all_inventory(ob);
  for (i=sizeof(list)-1;i>=0;i--)
    if (list[i] && query_once_interactive(list[i]))
    {
      list[i]->move("room/void",M_TPORT | M_SILENT | M_NO_SHOW | M_NOCHECK);
    } else
      list[i]=0;
  list-=({0});

  if (ob)
  {
    Remove(ob,1);
  }
  if (cloned)
  {
    if (ob=find_object(file))
    {
      Remove(ob,1);
    }
    err=catch(ob=clone_object(file));
    if (!err)
      ob->move(env,M_TPORT | M_SILENT | M_NO_SHOW | M_NOCHECK);
  }
  else
  {
    err=catch(call_other(file,"???"));
    if (!err)
      ob=find_object(file);
    else
      ob=0;
  }
  if (!ob)
  {
    write("Error in loaded object. Staying in void ...\n");
    return arg;
  }
  for (i=sizeof(list)-1;i>=0;i--)
    if (list[i])
      list[i]->move(ob,M_TPORT | M_SILENT | M_NO_SHOW | M_NOCHECK);
  return arg;
}

// LUPE ENDE!

int _hammerreload()
{
  if (_checkallow()==0) return 1;   
  if (environment(TP)->QueryProp(P_SHORT)!="seerroom")
  {
    printf("Das geht leider nur in einem Deiner Räume...\n");
    return 1;
  }
  log("_hreload");
  printf("Raum wird neu geladen...\n");
  make();
  return 1;
}

int _hammerls()
{
  write("Raumliste:\n");
  write("==========\n\n");
  for(int i=2;i<sizeof(get_dir("/d/seher/haeuser/"+geteuid(this_player())+"/room/*"));i++)
    write("==> "+explode(get_dir("/d/seher/haeuser/"+geteuid(this_player())+"/room/*")[i],".")[0]+"\n");
  
  return 1;
}

int _hammerman()
{
  return TP->More("/d/seher/aloy/hanleitung.txt",1);
}

int _hammersethome(string rno)
{
  int raumnr;
  raumnr=0;
  string raum;
  
  if(rno=="reset")
  {
    TP->SetProp(P_START_HOME,0);
    write("Startpunkt wurde resetet!\n");
    return 1;
  }
  
  if(rno)
    sscanf(rno,"%d",raumnr);
  
  if(raumnr<1 || raumnr>99)
  {
    write("Du musst eine Raumnummer angeben!\n");
    return 1;
  }
  
  raum=PATH+"/"+rno;
  
  TP->SetProp(P_START_HOME,raum);
  write("Raum "+rno+" wurde als dein Startpunkt gesetzt!\n");
  
  return 1;
}
