/* Shutdown-Daemon -- die Messages muessen wohl nochmal ueberarbeitet werden */
inherit "std/npc";

#include <properties.h>
#include <defines.h>
#include <language.h>
#include <moving.h>
#include "/secure/wizlevels.h"

string *messages=
  ({"Blitz und Donner hallen durch die Welt.",
  "Erdbeben erschüttern die Welt und bringen Häuser zum Einsturz.",
  "Verzweifelt laufen die Leute umher.",
  "Kreischend fliehen die Einwohner.",
  "Der Boden reißt auf und heißer Dampf steigt empor.",
  "Schwarzer Regen fällt vom Himmel herab.",
  "Die Luft flimmert und lautes Grollen ist aus der Ferne zu hören.",
  "Die Einwohner dieser Welt beginnen, vor Deinen Augen zu verschwimmen.",
  "Violette Wolken türmen sich am Himmel auf ... blutiger Regen fällt hernieder.",
  "Lava aus unzähligen Vulkanen fließt und verbrennt, was sie berührt.",
  "Die Flüsse und Meere dieser Welt beginnen zu verdampfen.",
  "Düstere Wolken ziehen am Horizont vorüber.",
  "Die apokalyptischen Reiter traben an Dir vorüber.",
  "Armageddon ruft: DAS ENDE NAHT!",
  "Der Himmel verdunkelt sich, Sterne und Sonne beginnen zu verlöschen.",
  "Um Dich herum siehst Du nur noch brennende Trümmer. Das Ende der Welt muss nun\nunmittelbar bevorstehen."});

int restzeit;

varargs string text_time(mixed sec, int flag);

int _query_restzeit()
{
  return restzeit+find_call_out("NextStep");
}

void create()
{
  ::create();
  SetProp(P_NAME, "Armageddon");
  SetProp(P_GENDER, MALE);
  SetProp(P_SHORT, "Armageddon, der Weltenvernichter");
  AddId("armageddon");
  set_living_name("armageddon");
  SetProp(P_ARTICLE,0);
  SetProp(P_XP,0);
  if(!IS_ARCH(this_player())&&!IS_ARCH(this_interactive()))
  	this_object()->remove();
  AddCmd("shut","shut");
}

varargs string _query_long()
{
  return break_string("Dies ist Armageddon, der Weltvernichter. Als Du ihn ansiehst, blickt er auf und flüstert Dir zu: Noch "+text_time(_query_restzeit())+" müssen vergehen, ehe ich die Welt vernichte.\n",80,0);
}

varargs string text_time(mixed sec, int flag)
{
  string s;
  int tage, stunden, minuten, sekunden;
  
  sekunden=sec;
  tage=sekunden/86400;
  stunden=(sekunden%86400)/3600;
  minuten=(sekunden%3600)/60;
  sekunden=sec%60;
  s="";
  if (sec>=86400)
  {
    s+=tage+" Tag"+(tage==1 ? ", " : (flag ? "en, " : "e, "));
  }
  if (sec>=3600)
  {
    s+=stunden+" Stunde"+(stunden==1 ? ", " : "n, ");
  }
  if (sec>=60)
  {
    s+=minuten+" Minute"+(minuten==1 ? "" : "n");
  }
  s+=(sec/60?" und ":" ")+sekunden+" Sekunde"+(sekunden==1 ? "" : "n");
  return s;
}

void shut(mixed minutes)
{
  int a,b,c;

  a=b=c=0;
  move("/gilden/abenteurer",M_GO);
  if (intp(minutes))
    c = minutes;
  
  else if (stringp(minutes))
  {
    if(sscanf(minutes,"%d:%d:%d",a,b,c)==3)
      c += b*60+a*24*60;
    else if(sscanf(minutes,"%d:%d",b,c)==2)
      c += b*60;
    else if(sscanf(minutes,"%d",c)==1)
      ;
  }

  while (remove_call_out("NextStep")>=0);
  restzeit=c*60;
  call_out("NextStep",0,restzeit);
}

void SHOUT(mixed str)
{
  int i;
  
  str=explode(break_string(str,60),"\n");
  for (i=0;i<sizeof(str);i++)
  {
    write("Armageddon ruft: "+str[i]+"\n");
    shout("Armageddon ruft: "+str[i]+"\n");
  }
}

static void NextStep(mixed seconds)
{
  int neu, rufmal;

  if (seconds<=0)
  {
    SHOUT("Ich starte das Spiel jetzt neu !");
    shutdown(" durch Armageddon!");
    return;
  }
  if (seconds<=600)
  {
    SHOUT("Teile mir mit, wenn Du in den Laden gebracht werden willst!");
  }
  if ((seconds<=900) && (find_call_out("ArmasShout")==-1))
  {
    call_out("ArmasShout",random(3)+5);
  }

  if (seconds>2*86400) 
    neu=seconds-5*3600;
  else if (seconds>4800)
    neu=seconds-1800;
  else
    neu=seconds*3/4-10;
  restzeit=neu;
  call_out("NextStep",seconds-neu,neu);
  SHOUT("In "+text_time(seconds,1)+" werde ich die Welt zerstören und neu erschaffen!");
}

int ArmasShout()
{
  int i;
  string *str;

  call_out("ArmasShout",30+random(30));

  str=explode(break_string(messages[random(sizeof(messages))],60),"\n");

  for (i=0;i<sizeof(str);i++)
  {
    shout(str[i]+"\n");
  }

  return 1;
}

int movetoladen()
{
  tell_object(this_player(),break_string("Armageddon transportiert Dich "
     +"in den Laden von Gidhrog.\n"));
  this_player()->move("/d/gidhrog/gralkor/gidhrog-stadt/room/room-g-03",M_GO, "",
          " verschwindet plötzlich",
          " materialisiert sich plötzlich vor Dir"
          );

  return 1;
}

void catch_tell(string str)
{
  string who,what;
  object ob;
  
  if (sscanf(str, "%s teilt Dir mit: %s", who, what) != 2)
    return;
  if (!this_player()) return;
  if (_query_restzeit()<600)
  {
    //this_player()->move("/d/gidhrog/gralkor/gidhrog-stadt/room/room-g-03",M_TPORT);
    call_out("movetoladen",0);
    return;
  }
  tell_object(this_player(),"Armageddon teilt Dir mit: Es ist noch zu früh! Probiers 10 Minuten vor dem\nArmageddon teilt Dir mit: Ende nochmal!\n");
  return;
}

int remove()
{
  write("Armageddon doesn't want to be removed!\n");
  return 0;
}
