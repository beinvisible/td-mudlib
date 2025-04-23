inherit "std/thing";
inherit "std/more";

#include <properties.h>
#include <language.h>
#include <wizlevels.h>

int gelesen;
string unterschrieben;

create()
{
	if (!geteuid(this_object()))
		call_out("remove",0);
	if (!IS_WIZARD(geteuid(this_object())))
		call_out("remove",0);
  if (!clonep(this_object())) return;
	thing::create();
	SetProp(P_SHORT,"Ein Vertrag");
	SetProp(P_LONG,"@@unterschrieben@@");
	SetProp(P_NAME,"Vertrag");
	SetProp(P_GENDER,MALE);
	SetProp(P_ARTICLE,1);
	AddId("vertrag");
        tell_object(this_player(), break_string("Bitte dem neuen Magier "
          +"nach der Magierwerdung den Magier-Zettel geben!!\n"
          +"/obj/magier_zettel.c\n\a",78));
}

unterschrieben()
{
	string s;

	s="Dies ist Dein Magier-Vertrag. Du solltest ihn vielleicht ";
	if (gelesen)
		s+="noch";
	s+="mal lesen.\nEr wurde Dir von "+capitalize(geteuid(this_object()))+" ueberreicht.\n"+
	   "Wenn Du ihn unterschreibst, wirst Du von "+capitalize(geteuid(this_object()))+" gesponsort.\n";
	if (unterschrieben)
		s+="Du hast ihn unterschrieben.\n";
	return s;
}

init()
{
	thing::init();
	add_action("lies","lies");
	add_action("unterschreibe","unterschreib",1);
	add_action("unterschreibe","unterzeichn",1);
	add_action("zerreisse","zerreiss",1);
}

lies(str)
{
	if (!id(str))
		return 0;
	gelesen=1;
	More("/doc/WIZRULES",1);
	return 1;
}

unterschreibe(str)
{
	if (!id(str))
		return;
	
	if (this_player() != this_interactive() || this_player() != environment())
		return;

	if (!gelesen)
	{
		write("Du hast ihn noch nicht gelesen. ERST lesen, DANN schreiben !\n");
		return 1;
	}
	if (getuid(this_player())=="andy")
	{
		write("Dir faellt der Stift aus der Hand.\n");
		return 1;
	}
	write("Du unterschreibst den Vertrag. Wenn das ein Versehen war, zerreiss ihn sofort.\n");
	unterschrieben=getuid(environment());
	AddId("unterschriebenvertrag");
	SetProp(P_NODROP,"Nein, der ist unterschrieben. Du kannst ihn hoechstens zerreissen.\n");
	SetProp(P_NEVERDROP,1);
	return 1;
}

zerreisse(str)
{
	if (!id(str))
		return;
	write("Du zerreisst den Vertrag.\n");
	return remove();
}
