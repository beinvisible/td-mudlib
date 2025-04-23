
/*
 * Der Raum der Entscheidung
 * ---
 * Hierher kommen Spieler bei versuchtem PK.
 * Sie koennen dann einen von drei Hebeln ziehen, der vielleicht Leben
 * oder aber den sicheren Tod (zu 66%) bedeutet.
 *
 * Erweiterungen sind noch in Planung (Begnadigung durch den
 * Angegriffenen zB.).
 *
 */

inherit "std/room";

#include <properties.h>
#include <wizlevels.h>
#include <language.h>
#include <moving.h>
#include <player.h>

void create()
{
  ::create();
  SetProp(P_LIGHT, 1 );
  SetProp(P_INT_SHORT, "Der Raum der Entscheidung");
  SetProp(P_INT_LONG,
    "Du stehst in einem kalten dunklen Verlies - vermutlich nicht ohne Grund. "
    "An einer Wand kannst Du drei Hebel erkennen, einer von ihnen bedeutet "
    "Leben, die beiden anderen den sicheren Tod. Mit <ziehe hebel x> kannst "
    "Du Dich für einen der Hebel entscheiden... Viel Glück, auch wenn Du "
    "es gar nicht verdient hast!");
  SetProp(P_KILL_NAME, "Die gerechte Strafe");
  SetProp(P_INDOORS,1);
}

void init()
{
	::init();
	if (IS_ELDER(this_player()))
		return;
	add_action("abc", "", 1);
}

int abc (string str)
{
	string v;

	v=query_verb();
	if (stringp(v)&&v!=""&&
	    (member( ({"sag", "sage", "nick", "nicke", "nein",
	               "schau", "schaue"}), v)
	     !=-1||v[0]=='\'')) return 0;
	if (stringp(v) && (v=="zieh" || v=="ziehe"))
	  if (stringp(str) && str[0..5]=="hebel ")
	  {
	    int n;
	    if ((n=member(({"1","2","3"}), str[6..])) == -1) {
	      write ("WAS für einen Hebel? Es gibt nur 1, 2 oder 3.\n");
	      write ("Entscheide Dich.\n");
	      write ("Jetzt.\n");
	      return 1;
	    }
	    else {
	      write ("\nDu ziehst am Hebel "+(n+1)+", und......\n\n");
	      if (n!=random(3)) {
	        write ("Uff - nochmal Glück gehabt!!\n\n"+
	          "Du verlierst für einen Augenblick die Orientierung...\n");
	        this_player()->move("/gilden/abenteurer", M_GO|M_NOCHECK);
                map_objects( this_player()->QueryEnemies()[0], "StopHuntFor", 
                             this_player(), 1 );
                this_player()->StopHuntingMode(1);

	      }
	      else
	      {
	        write ("Ohjeh... das war wohl der falsche Hebel...\n\n");
	        this_player()->die();
	      }
	    }
	  }
	  else
	    write("Syntax: ziehe hebel <n>\n");
	else
	  write("Nix da.\n");
	return 1;
}

