// 28.07.2000 Serii@Tamedhon
// banish, unbanish und mbanish eingebaut (aus /std/gilde.c uebernommen)
// zeit fuer Armageddon ist jetzt auch einstellbar
// Armageddon kann auch abgebrochen werden...

inherit "std/room";
inherit "std/more";

#define wrs(s,str) s+=str

#include <properties.h>
#include <wizlevels.h>
#include <language.h>
#include <rooms.h>
#include <moving.h>
#include <defines.h>

void init()
{
  room::init();
  if (IS_LORD(PL)) {
    add_action("banish","banish");
    add_action("unbanish","unbanish");
  }
  if (IS_ARCH(PL))
    add_action("mbanish","mbanish");
}

int banish(string str)
{
  string grund, name;

  if (!IS_LORD(this_interactive())) return 0;

  if (!str) {
    write("Syntax: banish <name> <grund>\n");
    return 1;
  }
  str = this_interactive()->_unparsed_args();

  if (sscanf(str,"%s %s",name,grund)!=2) name=str;
  if (!name || name =="")
  {
    write("Syntax: banish <name> <grund>\n");
    return 1;
  }
  if( !grund )
  {
    write("Du musst einen Grund angeben! Syntax: banish <name> <grund>\n");
    return 1;
  }
  name=lower_case(name);
  "/secure/master"->BanishName(name,capitalize(grund));
  return 1;
}

int unbanish(string str)
{
  string name;

  if (!IS_LORD(this_interactive())) return 0;

  if (!str) {
    write("Syntax: unbanish <name>\n");
    return 1;
  }
  str = this_interactive()->_unparsed_args();

  if (sscanf(str,"%s",name) != 1) name=str;
  if (!name || name =="") {
    write("Syntax: unbanish <name>\n");
    return 1;
  }
  name=lower_case(name);
  "/secure/master"->UnbanishName(name);
  return 1;
}

int mbanish(string str)
{
  string grund, name;

  if (!IS_ARCH(this_interactive())) return 0;

  _notify_fail("Syntax: mbanish <name> [<grund>]\n");

  if (!str || str=="")
    return 0;
  if (sscanf(str,"%s %s",name,grund)!=2) name=str;
  if (!name || name =="")
    return 0;
  "/secure/merlin"->MBanishInsert(lower_case(name), capitalize(grund),
      this_interactive());
  return 1;
}

void create()
{
  room::create();
  SetProp(P_LIGHT,1);
  SetProp(P_INT_LONG,
	"Du befindest Dich in einem halbfertigen Kontrollraum. Aus riesigen Schalt-\n"
  "kästen und -tafeln schaut ein Gewirr aus bunten Kabeln hervor. In\n"+
  "dem alle Fäden zusammenlaufen und von dem aus das Mud gesteuert wird.\n"+
  "An einer Wand hängt eine Kontrolltafel, die halbwegs fertig aussieht,\n"+
	"daneben befindet sich ein Pult mit ein paar leuchtenden Schaltern.\n"+
	"Du kannst von hier aus in den Arbeitsraum eines jeden Magiers kommen,\n"+
	"indem Du einfach nur seinen Namen tippst.\n");
  SetProp(P_INT_SHORT,"Mud-Kontrollzentrum");
  SetProp(P_INDOORS, 1);
  AddSpecialDetail(({"tafel","kontrolltafel"}),"tafel");
  AddDetail(({"pult","schaltpult","kontrollpult"}),
            "Die Knöpfe sind wie folgt beschriftet:\n"+
            "1. Reparatur (preloading wird wiederholt, birgt KEINE Risiken)\n"+
            "4. Armageddon+Zeit in Minuten (default 10 Minuten)\n"+
	    "5. Armageddon abbrechen\n"+
	    "Außerdem entdeckst Du einen kleinen Monitor, der dir vorher"
	    " hier noch nicht aufgefallen ist.\n");
  AddDetail( ({"monitor"}),
            "Es stehen jetzt auch die Befehle 'banish', 'unbanish' und"
	    " 'mbanish' zur Verfügung." );
// gibts ja ned :) .. Serii
//  AddExit("ebene","/d/ebene/hadra/dorf/rooms/w_dorf_5_4");
//  AddExit("gebirge","/d/gebirge/room/zkapelle");
//  AddExit("nordland","/d/nordland/hadra/raetsel/rooms/main");
//  AddExit("tundra","/d/tundra/kaig/workroom");
//  AddExit("unterwelt","/d/unterwelt/death/katakomben/rooms/katakomben_eingang_1");
//  AddExit("wald","/d/wald/room/elfendorf/kapelle");
//  AddExit("wasser","/d/wasser/room/gildeninsel/insel24");
//  AddExit("wolken","/d/wolken/mupfel/room/wolkenbank");
//  AddExit("wueste","/d/wueste/room/wueste/wuestein");
  AddCmd(({"drueck","druecke","drück","drücke"}),"druecke");
  AddCmd("","workroom",1);
}

int workroom() {
  string pln,plw;
  if (query_verb()[0] < 'a' || query_verb()[0] > 'z')
    return 0;
  if (file_size("/players/"+query_verb()) != -2)
    return 0;
  plw="/players/"+query_verb()+"/workroom";
  pln=capitalize(query_verb());
  this_player()->move(plw,130,"in den Arbeitsraum von "+pln,"teleportiert","kommt herein");
  return 1;
}

int secure()
{
  object po, ti;
  
  //  if (!(po=previous_object())) return 0;
  //  if (!(ti=this_interactive())) return 0;
  //  if (getuid(po) != getuid(ti)) return 0;
  //  if (geteuid(po)!=geteuid(ti)) return 0;
  if (!previous_object()) return 0;
  if (geteuid(previous_object())==ROOTID) return 1;
  if (geteuid(previous_object()) != geteuid(this_interactive())) return 0;
  if (this_interactive() != this_player()) return 0;
  if (!IS_ARCH(this_interactive())) return 0;
  return 1;
  return 1;
}


string tafel()
{
  string s, *tmp;
  int t,t2,idle_1,idle_5,magier,osnabrueck,i, leipzig, tmp2;
  
  osnabrueck=0;
  idle_5=0;
  idle_1=0;
  magier=0;
  leipzig=0;
  s="";
  for (i=0;i<sizeof(users());i++) {
    tmp=explode(query_ip_number(users()[i]), ".");
    if (sizeof(tmp)==4 && (tmp[0]=="139" && tmp[1]=="18")) leipzig++;
    tmp2=query_idle(users()[i]);
    if (tmp2>300) idle_5++;
    else
        if (tmp2>60) idle_1++;
    if (IS_WIZARD(users()[i])) magier++;
}
  wrs(s,"Diese Kontrolltafel gibt Auskunft über ein paar wichtige Daten des Muds.\n");
  wrs(s,sprintf(
  "Es %s sich zur Zeit %d Person%s im Mud auf. %d davon %s Magier. Von den\n"+
  "Teilnehmern %s %d zwischen 1 und 5 Minuten, %d über 5 Minuten inaktiv. %d der\n"+
  "Mitspieler komm%s aus Thalheim.\n",
  (sizeof(users())==1 ? "hält" : "halten"), sizeof(users()),
  (sizeof(users())==1 ? "" : "en"), magier,
  (magier==1 ? "ist" : "sind"), 
  (idle_1==1 ? "ist":"sind"), idle_1, idle_5, osnabrueck,
  (osnabrueck==1 ? "t":"en")));
  t=time()-last_reboot_time();
  wrs(s,"Tamedhon ist "+dtime(last_reboot_time())+" zuletzt neu gebooted worden.\n"+
        "Damit ist es seit "+t+" Sekunden aktiv, das entspricht\n");
  t2=t;
  if (t>=86400)
  {
    wrs(s,t/86400+" Tag");
    if (t>=86400*2) 
    wrs(s,"en");
    wrs(s,", ");
    t2=t2%86400;
  }
  if (t>=3600)
  {
    wrs(s,t2/3600+" Stunde");
    if (t2/3600!=1) 
      wrs(s,"n");
    wrs(s,", ");
    t2=t2%3600;
  }
  if (t>=60)
  {
    wrs(s,t2/60+" Minute");
    if (t2/60!=1)
      wrs(s,"n");
    wrs(s," und ");
    t2=t2%60;
  }
  wrs(s,t2+" Sekunde");
  if (t2!=1)
    wrs(s,"n");
  wrs(s,".\n");
	wrs(s,"Tamedhon existiert seit ca. "+exist_days()+" Tagen.\n");
  More(s,0);
  return "";
}

int druecke()
{
  object ti;
  string *temps, str;
  mixed time;
  temps = explode(this_player()->_unparsed_args()," ");
  if(sizeof(temps)>=1)
    str = temps[0];
  if(sizeof(temps)>1)
    time = sprintf("%d", (int)temps[1]);
  if (!secure()) return 0;
  if (!str) return 0;
  ti=this_interactive();
  if (!IS_WIZARD(ti))
  {
    write("Du versuchst, einen der Knöpfe am Pult zu berühren, doch dabei wird Dir ent-\n"+
          "setzlich schwindlig ... Du beschließt, dass das wohl keine sehr gute Idee war.\n");
    return 1;
  }
  switch (str)
  {
    case "1":
    case "reparatur":
      write("Ok.\n");
      say(ti->name(WER)+" startet den Reparaturmechanismus.\n");
      "secure/master"->redo_preload();
      return 1;
    case "4":
    case "armageddon":
      if(!time)
        time = 10;
      write("Armageddon ausgelöst.\n");
      shout(ti->name(WER)+" hat Armageddon ins Spiel geholt !\n");
      "obj/shut"->shut(time);
      return 1;
    case "5":
      if(find_living("armageddon"))
      {
        write("Armageddon abgebrochen.\n");
        shout(ti->name(WER)+" hat Armageddon abgebrochen !\n");
        destruct(find_living("armageddon"));
      }
      else
      {
        write("Armageddon wurde garnicht gestartet !\nAbbruch abgebrochen :)\n");
      }
      return 1;
  }
  return 0;
}

void exit() // War schon oefter so, das Magier hier walking monsters erzeugt
  // haben, die dann in die Gilde gehopst sind ... die exits
  // zu sperren hat auch keinen Sinn - Sirs MNPC moved sich dann
  // doch an seinen Startpunkt. Naja, machen wir es eben SO - Monster
  // haben hier eh nix verloren.
{
  object tp;
  
  tp=previous_object();
  if (!query_once_interactive(tp))
    call_out("do_destruct",1,tp);
}

void do_destruct(object ob)
{
  if (ob)
    ob->remove();
  if (ob)
    destruct(ob);
}
