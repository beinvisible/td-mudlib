// SilberLand MUDlib
//
// /p/service/std/mailcabin.c -- standard mailcabin
//
// $Date: 1997/03/16 19:14:07 $
/* $Revision: 1.1 $
 * $Log: mailcabin.c,v $
 * Revision 1.1  1997/03/16 19:14:07  Woody
 * prepared for revision control
 *
 */

/* Eine Schreibkabine, in der man einigermassen ungestoert seine Post
   lesen und schreiben kann.

   (C) 1993/94 by Loco@Morgengrauen

   Verwendung ausserhalb von Morgengrauen ist gestattet unter folgenden
   Bedingungen:
   - Benutzung erfolgt auf eigene Gefahr. Jegliche Verantwortung wird
     abgelehnt.
   - Auch in veraenderten oder abgeleiteten Objekten muss ein Hinweis auf
     die Herkunft erhalten bleiben.
   Ein Update-Service besteht nicht.

   Fuer Fragen, Hinweise und Vorschlaege bitte mich in Morgengrauen oder 
   Nightfall ansprechen.
*/

#include "/mail/post.h"
#include <config.h>
#include <properties.h>
#include <language.h>
#include <moving.h>

inherit STDPOST;
inherit "std/thing/moving";
inherit "std/thing/commands";

void create() {
  (post::create(42));
  (commands::create());
  SetProp(P_NAME,"Schreibkabine");
  SetProp(P_GENDER,FEMALE);
  SetProp(P_SHORT,"Eine Schreibkabine");
  SetProp(P_INT_LONG,"\
In dieser Kabine kannst Du ungestoert Post lesen und schreiben. Niemand anders\n\
kann hier rein, solange Du drin bist, dafuer waere die Kabine auch viel zu eng.\n\
Tippe einfach 'post' oder 'mail', bzw 'mail <spieler>' zum Schreiben.\n\
Mit 'raus' kommst Du wieder raus.\n");
  SetProp(P_NOGET,"Versuch mal, eine festmontierte Kabine auszubauen!\n");
  AddId("kabine");
  AddId("schreibkabine");
  AddId("postkabine");
  AddId("kabinen");
  AddId("schreibkabinen");
  AddId("postkabinen");
      

}

int besetzt() {
  mixed abc,i;
  abc=all_inventory(this_object());
  for (i=0;i<sizeof(abc);i++)
    if (living(abc[i]) && abc[i]!=this_player()) return 1;
  return 0;
}

string long() {
  return "\
Eine kleine Ein-Personen-Kabine, in der Du ungestoert Deine Post lesen und\n\
schreiben kannst. Du kannst sie betreten. \n\
"+( besetzt() ? "Diese Kabine ist allerdings besetzt.\n" : "");
}

string int_short() {
  return (environment()->QueryProp(P_INT_SHORT))+" (Kabine)\n";
}

int move(mixed a,mixed b,mixed c,mixed d,mixed e) {
  int r;
  r=(::move(a,b));
  AddExit("raus",object_name(environment()));
  return r;
}

void init() {
  (commands::init());
  if (environment(this_player())==environment(this_object())) {
    add_action("rein","betrete");
    add_action("rein","betritt");
    return;
  }
  if (besetzt()) {
    this_player()->move(environment(this_object()),M_GO,0,
      "passt hier nicht mehr rein",
      "macht die Tuer einer Kabine auf, aber die ist schon besetzt");
    return;
  }
  if (environment(this_player())==this_object()) {
    (post::init());
    add_action("do_mail","mail");
    add_action("do_mail","post");
    return;
  } 
}

int rein(string str) {
  if (!str || !id(str)) {
    notify_fail("Wo willst Du denn rein?\n");
    return 0;
  }
  if (besetzt()) {
    notify_fail("BESETZT!\n");
    return 0;
  }
  this_player()->move(this_object(),M_GO,0,
    "betritt eine Schreibkabine","betritt die Kabine");
  return 1;
}

clean_up(arg) {
  return 1;
}
