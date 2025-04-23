/*
   Das Nedit-Tool.
   13.November 1997, Stahldorn@SilberLand

   Befehl "ned <argument> laedt das in <argument> angegebene 
   File in den maileditor "nedit". Alternative zum vielgehassten "ed".
   <argument> kann dabei von folgender Form sein:
      "$h":         File des Raums, in dem man steht.

      ein Filename: Das angegebene File. ".c" wird angehaengt, falls das 
                    File nicht gefunden wird.

      ID eines Objekts im inventory: Das Objektfile 
	  
 $Date: 2001/01/13 13:12:20 $
 $Revision: 1.2 $
 $Log: nedtool.c,v $
 Revision 1.2  2001/01/13 13:12:20  Samtpfote
 Sicherheitscheck, kleiner Bugfix

 Revision 1.1  2001/01/13 13:11:23  Samtpfote
 Initial revision
	  
	  
*/


inherit "std/thing";
inherit "/mail/nedit";

#include "/secure/config.h"
#include <properties.h>
#include <language.h>
#include <wizlevels.h>
#include "/players/stahldorn/def.h"

string actual_file;

void create()
{
  if (!clonep(TO)) return;
  ::create();

  SetProp(P_NAME, "Nedit-Tool");
  SetProp(P_LONG,
   "Tipp einfach 'ned <filename>' und los gehts.\n(Hilfe gibts mit 'nedhelp'");
  SetProp(P_SHORT, "Das Nedit-Tool");
  SetProp(P_GENDER, NEUTER);
  AddId( ({"nedtool", "nedit-tool"}) );
  SetProp(P_WEIGHT, 0);
  SetProp(P_VALUE, 0);
  SetProp(P_NOBUY, 1);
  SetProp(P_AUTOLOADOBJ, 1);
  SetProp(P_NEVERDROP, 1);
  SetProp(P_NODROP, 1);
  AddCmd(({ "ned", "nedit"}), "ned");
  AddCmd(({ "nedhelp"}), "nedhelp");

  seteuid(getuid(this_interactive()));
}

int ned(string str)
{
  object ob;

  seteuid(getuid(this_interactive()));

  if(!IS_WIZARD(TI))  {
    notify_fail("Das dürfen nur Magier!\n");
	call_out("remove", 0);
    return 0; }

  if(!str)  {
    notify_fail("Syntax: ned <filename>\n");
    return 0; }

  if(str == "$h")
     str=object_name(environment(TI))+".c";
  else if(ob=present(str, TI))
    str = explode(object_name(ob),"#")[0]+".c";
  else if(file_size(str=(MASTER->make_path_absolute(TI->_unparsed_args())))==-1)
  {
    notify_fail(BS(
      "File '"+str+"' nicht vorhanden oder keine Zugriffsrechte."));
    if(file_size(str+=".c") == -1)
      return 0;
  }
  else if (file_size(str) == -2)  
  {
    notify_fail(BS("'"+str+"' ist ein Verzeichnis!"));
    return 0; 
  }

  actual_file = str;
//  DB(actual_file+" ist actual_file");
  nedit("ned_save", read_file(str));

  return 1;
}

void ned_save(string str)
{
  if(!str)
  {
     write("Abgebrochen. File nicht gespeichert.\n");
     return;
  }
  if(rm(actual_file))
  {
    if(write_file(actual_file, str))
      write("File gespeichert.\n");
    else
      write("Zugriff verweigert. File nicht gespeichert.\n");
  }
  else
    write("Zugriff verweigert. File nicht gespeichert.\n");
}

int nedhelp()
{
  // TI->More("/players/stahldorn/tools/nedhelp.txt", 1);
  return 1;
}
