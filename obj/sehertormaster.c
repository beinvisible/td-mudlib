//
// Masterobjekt fuer Sehertore (c) by Rochus
//
// Dieses Objekt verwaltet die Sehertore.
// Nicht clonen! Nicht updaten! Nicht destructen!
// (Ausser bei Erneuerung der Torliste)
// Die einzelnen Funktionen sind in /doc/lfun dokumentiert.
//
#include <properties.h>
#include <moving.h>
#include <wizlevels.h>
#include <language.h>
#include <ansi.h>

#pragma strong_types
#define DOORFILE "/obj/sehertore"
#define TELEPORT_KOSTEN 10
#define P_SEERDOORS "rw_sehertore"

mapping doors;
// Doormapping:
// ([raumname: nummer; beschreibung; ]);

void create () {
  if (clonep(this_object())) {
    destruct(this_object());
    return;
  }
  doors=([]);
  restore_object(DOORFILE);
}

int DoorIsKnown() {
  // Tuer in Umgebung von TP ist TP bekannt.
  object pl,env;
  int num;
  mapping st;

  if ((pl=this_player()) &&
      IS_SEER(pl) && // Spieler muss Seher sein
      (env=environment(pl)) &&
      (num=doors[object_name(env)]) && // Tor existiert
      (st=pl->QueryProp(P_SEERDOORS)) && // mindestens ein Tor schon entdeckt
      st[num]) // wurde dieses Tor schon entdeckt ?
    return num;
  return 0;
}

varargs int DiscoverDoor(string dname) {
  // TP entdeckt Tor in seiner Umgebung oder dname, falls angegeben.
  object pl,env;
  int num;
  mapping st;

  if (!(pl=this_player()) ||
      !IS_SEER(pl)) return 0;
  if (!dname) { // Kein Tor an einer bestimmten Stelle angegeben?
    if (!(env=environment(pl))) return 0;
    dname=object_name(env); // Dann das Tor hier nehmen.
  }
  if (!(num=doors[dname])) return 0;
  if (!(st=pl->QueryProp(P_SEERDOORS))) { // Noch kein Tor entdeckt?
    pl->SetProp(P_SEERDOORS,([num:1])); // Dieses ist das erste.
    pl->Set(P_SEERDOORS,SAVE,F_MODE);
    return 1;
  }
  if (st[num]) // Tor war schon vorher bekannt.
    return 0;
  st[num]=1; // Tor ist jetzt bekannt.
  pl->SetProp(P_SEERDOORS,st);
  return 1;
}

varargs void ShowDoors(int toruebersicht) {
  // Zeigt alle Sehertore, die TP kennt.
  object pl;
  mixed *ind,*inf,ix;
  mapping st;
  int i,n,me;

  if(!this_player()->QueryProp(P_SEERDOORS) && toruebersicht)
  {
    write("Du hast noch kein Sehertor entdeckt!\n");
  }

  if (!(pl=this_player()) ||
          !IS_SEER(pl) ||
      !(st=pl->QueryProp(P_SEERDOORS)))
        return;
  me=DoorIsKnown();
  ind=m_indices(doors);inf=({});
  for (i=sizeof(ind)-1;i>=0;i--) {
    ix=ind[i];
    n=doors[ix];
    if (!n ||
        !st[n]) continue;
    inf+=({({n,doors[ix,1]})});
  }
  inf=sort_array(inf,lambda(({'x,'y}),
                            ({#'<,({#'[, 'x, 0}),({#'[, 'y, 0})})));


  if(pl->QueryProp(P_NO_ASCII_ART))
  {
    for (i=sizeof(inf)-1;i>=0;i--) 
    {
      if (inf[i][0]!=me)
        printf(" %2d. %s\n",inf[i][0],inf[i][1]);
      else
        printf("[%2d. %s]\n",inf[i][0],inf[i][1]);
    }
  }
  else
  {
    string pp = "|";
    string output = "";

    if(toruebersicht)
      write("/=====================================================================\\\n");

    for (i=sizeof(inf)-1;i>=0;i--) 
    {
      if (inf[i][0]!=me)
        output += sprintf(pp+"   %-65s "+pp+"\n",sprintf(" %2d. %s",inf[i][0],inf[i][1]));
      else
      {
        if(pl->QueryProp(P_TTY) == "vt100")
          output += sprintf(pp+"  %-66s "+pp+"\n",sprintf(ANSI_BOLD+"☩ %2d. %s"+ANSI_NORMAL,inf[i][0],inf[i][1]));
        else if(pl->QueryProp(P_TTY) == "ansi")
          output += sprintf(pp+"  %-66s "+pp+"\n",sprintf(ANSI_CYAN+"☩ %2d. %s"+ANSI_NORMAL,inf[i][0],inf[i][1]));
        else
          output += sprintf(pp+"  %-66s "+pp+"\n",sprintf("☩ %2d. %s"+ANSI_NORMAL,inf[i][0],inf[i][1]));
      }

      if(i!=0)
        output += "+---------------------------------------------------------------------+\n";
    }

    output += "\\=====================================================================/\n";
    printf(output);
  }
}

static void TeleportTo(int num) {
  // Teleportiert TP zu Tor Nummer <num>.
  object pl;
  mapping st;
  string *ind,ix;
  int i;

  if (!(pl=this_player()) ||
      !DoorIsKnown()) return;
  if (num==0) {
    write("OK, nicht teleportieren.\n");
    return;
  }
  if (!(st=pl->QueryProp(P_SEERDOORS)) ||
      !st[num]) { // Seher kennt dieses Tor noch nicht.
    write("Du kennst noch kein Tor mit dieser Nummer.\n");
    return;
  }
  ind=m_indices(doors);
  for (i=sizeof(ind)-1;i>=0;i--) {
    ix=ind[i];
    if (doors[ix]!=num) continue; // Dieses Tor ist nicht das angegebene.
    if (pl->QueryProp(P_SP)<TELEPORT_KOSTEN) {
      write("Du hast nicht genug Magiepunkte dafür.\n");
      return;
    }
    write("Du verlierst für einen Moment die Orientierung.\n");
    if (pl->move(ix,M_TPORT)>0)
      pl->restore_spell_points(-1*TELEPORT_KOSTEN);
    else
      write("Das Teleportieren hat nicht funktioniert.\n");
    /* 
    // Keine Parallelwelt im AL!
    if (random(100)<5 && !(pl->QueryProp(P_PARA))) {
      pl->SetProp(P_PARA,((random(100)<5)?0:1));
      write("Du hast das komische Gefuehl, dass beim Teleportieren\n"+
            "etwas falsch gelaufen sein koennte...\n");
      if (pl->QueryProp(P_PARA))
	log_file("para",sprintf("%s %s (%s): Sehertor\n",ctime(time()),
				pl->name(WER),geteuid(pl)));
    }
    */
    return;
  }
  write("Es existiert kein Ziel mit dieser Nummer.n");
}

static void GetTeleporterNum(string str) {
  // Wartet auf Eingabe einer Tornummer.
  int n;

  if (!str)
    n=0;
  else
    sscanf(str,"%d",n);
  TeleportTo(n);
}

varargs int Teleport(string str) {
  // Teleportiert Seher zum Tor, falls angegeben.
  object pl;

  if (!DoorIsKnown()) return 0;
  if (!str) {
    if(this_player()->QueryProp(P_NO_ASCII_ART))
      write("  0. nicht teleportieren\n");
    else
    {
      string pp = "|";

      write("/=====================================================================\\\n");
      write(sprintf(pp+"   %-65s "+pp+"\n","  0. nicht teleportieren"));
      write("+---------------------------------------------------------------------+\n");
    }
    ShowDoors();
    input_to("GetTeleporterNum");
  } else
    GetTeleporterNum(str);
  return 1;
}

void ReloadDoorFile()
{
  doors=([]);
  restore_object(DOORFILE);
}
