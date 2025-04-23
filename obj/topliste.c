inherit "std/thing";

#include <properties.h>
#include <language.h>

string _query_long();
int magliste();
static string whatdenn;
static int mag;
int age;

void create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT,"Die Top-Liste, Top-Spielerliste, Top-Seherliste, "+
	  "Top-Zweitiliste und Magierliste");
  SetProp(P_WEIGHT,0);
  SetProp(P_GENDER,FEMALE);
  SetProp(P_WEIGHT,20);
  AddId(({"liste","topliste","top-liste",
          "topspielerliste","top-spielerliste","topspieler","spielerliste",
          "topseherliste","top-seherliste","topseher","seherliste",
          "topzweitiliste","top-zweitiliste","topzweitis","zweitiliste",
          "magierliste"}));
  SetProp(P_NAME,"Liste");
  SetProp(P_NOGET,"Die Topliste ist an der Wand festgenagelt.\n");
  seteuid(getuid(this_object()));
  mag=0;
}

void reset()
{
  mag=0;
}

void init()
{
  ::init();
  add_action("lies","lies");
}

varargs int id(string str)
{
  if (!::id(str))
    return 0;
  whatdenn="/etc/toplist.read";
  if (strstr(str,"seher")>=0)
    whatdenn="/etc/seherlist.read";
  else if (strstr(str,"spieler")>=0)
    whatdenn="/etc/spielerlist.read";
  else if (strstr(str,"zweit")>=0)
    whatdenn="/etc/topzweitis.read";
  else if (strstr(str,"magier")>=0)
    whatdenn="/etc/wizlist.read";
  return 1;
}

int lies(string str)
{
  if (!id(str))
    return 0;
  _query_long();
  return 1;
}

/*  // Auskommentiert -> siehe unten
string _query_long()
{
  if (this_player()) this_player()->More(whatdenn,1);
  whatdenn="";
  return "";
}
*/

/* Folgendes war mal long() */
string _query_long()
{
  if (this_player())
    this_player()->More(whatdenn,1);
  whatdenn="";
  return "";
}
