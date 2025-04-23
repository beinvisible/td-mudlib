/* NPC-Master, registriert alle NPCs und modifiziert sie ggf.
 * Sun 03 Dec 2000 06:04:48 PM EST, Samtpfote@Tamedhon
 * 
 * $Date: 2001/01/03 20:22:07 $
 * $Revision: 1.1 $
 * $Log: peoplemaster.c,v $
 * Revision 1.1  2001/01/03 20:22:07  Samtpfote
 * Initial revision
 *
 */

#define DB(x)		if(find_player("samtpfote")) \
					    tell_object(find_player("samtpfote"), \
					    break_string(x, 79, "{ peoplemaster } "));

#define obfn(x)		efun::explode(object_name(x), "#")[0]
#define NOREGION	"** Sonstige **"

object	*people;
string	*unique_people;
mapping regionen, sonstige_region;
int		max_people, max_unique;

/* --- Verwaltungsfunktionen ------------------------------------------ */
void cleanup_unique_people()
{
  int		i, is;
  string	*new_unique_people;

  // DB(sprintf("upeople=%O\n", unique_people));  
  is=sizeof(unique_people); new_unique_people=({});
  if(!is) return;
  new_unique_people=({});
  
  for(i=0; i<is; i++)
  {
	if(sizeof(get_dir(unique_people[i]+".c")))
	{ new_unique_people+=({ unique_people[i] }); }
  }
  
  unique_people=({})+new_unique_people;  
}

void cleanup_dead_people() /* Null-Pointer-Objekte entfernen */
{
  int		i, is;
  object	*new_people, ob;

  is=sizeof(people); new_people=({});
  /*
  DB("---------------------------------------------------------");
  DB(sprintf("people=%O\n", people));
  DB(sprintf("clean up: is=%i", is));
  */
  if(!is) 
  {
	// DB("returning ...");
	return;
  }
  
  for(i=0; i<is; i++)
  {
	if(people[i]) 
	{ 
	  ob=find_object(object_name(people[i]));
	  if(environment(people[i]))
	  {
		// DB(sprintf("re-adding: %O\n", ob));
		new_people+=({ people[i] }); 
	  }
	  else
	  {
		// DB(sprintf("clean up: %O\n", ob));
	  }
	}
  }
  
  people=({})+new_people;
  // DB(sprintf("people=%O\n", people));
}

/* -- Info -------------------------------------------------- */
int count_people()
{
  cleanup_dead_people();
  if(max_people < sizeof(people)) 
  { max_people=sizeof(people); }
  return sizeof(people);
}

int count_unique_people()
{
  cleanup_unique_people();
  if(max_unique < sizeof(unique_people))
  { max_unique=sizeof(unique_people); }
  return sizeof(unique_people);
}

mapping count_regionen()
{
  return copy(regionen);
}

string balken(int wert, int max, int len)
{
  string	st;
  int		i, wl;
  
  st="";
  // DB(sprintf("wert=%i, max=%i, len=%i", wert,max,len));
  if(len < 1) len=1;
  if((wert == max) && (wert == 0))
  { wl = len; }
  else
  {
	if(max==0) max=1;
	wl=(len*wert)/max;
  }
  // DB(sprintf("wert=%i, max=%i, len=%i, wl=%i", wert,max,len,wl));
  for(i=0; i<len; i++)
  {
	if(i < wl) 
	{ st+="#"; }
	else
	{ st+="-"; }
  }
  
  return st;
}

void statistik()
{
  string	output;
  int		i, max;

  max=count_unique_people();

  output=
	"PEOPLEMASTER Statistik von "+dtime(time())+"\n"+
	"-----------------------------------------------------------------\n"+
	"\n"+
	sprintf("Geladene NPCs im Speicher:         %5i (%i)\n", 
			count_people(), max_people)+
	sprintf("Eigenständige NPCs als Dateien:   %5i  %s (%i)\n", 
			max, balken(max,max,20), max_unique)+
	"\n";
	
  if(sizeof(regionen))
  {
	if(!max) { max=1; } // wegen division by zero
	for(i=0; i<sizeof(regionen); i++)
	{
	  output+=sprintf(
	        "%-20s               %5s  %s\n",
				capitalize(m_indices(regionen)[i]), 
				(m_values(regionen)[i]==0?"-":to_string(m_values(regionen)[i])),
				balken(m_values(regionen)[i],max,20)
				);
	}
	output += "\n";
	//DB(sprintf("sonstige region:  %O\n",  sonstige_region));
	for(i=0; i<sizeof(sonstige_region); i++)
	{
	  output+=sprintf(
	        "%-20s               %5s  %s\n",
				capitalize(m_indices(sonstige_region)[i]), 
				(m_values(sonstige_region)[i]==0?"-":to_string(m_values(sonstige_region)[i])),
				balken(m_values(sonstige_region)[i],max,20)
				);
	}	
	output += "\n\n";
  }  
  
  this_player()->More(output);
}

string *dump_unique_people() { return ({}) + unique_people; }
object *dump_people() { return ({}) + people; }

/* -- Register/Unregister ------------------------------------ */
void Register(object me)
{
  string	*path;

  if(!me) return;
  
  if(member(unique_people, obfn(me)) == -1) 
  { 
	path=efun::explode(obfn(me), "/");
	// DB(sprintf("%O\n",path));
	unique_people+=({ obfn(me) }); 
	if((path[1] == "d"))			// zZ nur Regions-NPCs!
	{
	  regionen[path[2]] = regionen[path[2]] +1;
	}
	else
	{
	  sonstige_region[NOREGION]  = sonstige_region[NOREGION] +1;  
	}
  }
  
  if(member(people, me) != -1) return;
  people+=({ me });
}

void UnRegister(object me)
{
  if(!me) return;
  people-=({ me });
}





/* --- Standardobjektfunktionen ------------------------------------ */

void reset()
{
  // save_object("peoplemaster");
}

void create()
{
  string	*regs;
  int		i, *vals;

  people=({}); unique_people=({});
  regionen = ([]);
  regs = get_dir("/d/*"); // - ({ ".", "..", "access_rights.c" });
  if(sizeof(regs))
  {
	vals=({});
	for(i=0; i<sizeof(regs); i++) { vals+=({ 0 }); }
	regionen = mkmapping(regs, vals);
  }
  sonstige_region=([NOREGION: 0]);
  
  max_people=0; max_unique=0;
}
