// SilberLand MUDlib
//
// std/thing/properties.c -- most general class (property handling)
//
// $Date: 2001/02/10 12:08:14 $                      
/* $Revision: 3.1 $       
 * $Log: properties.c,v $
 * Revision 3.1  2001/02/10 12:08:14  Samtpfote
 * prepared for revision control
 *
 * Revision 3.1  1997/06/25 12:10:29  Woody
 * PROTECTED/SECURED in SetProp() ber., Optimierungen
 *
 * Revision 3.0  1997/03/13 02:33:45  Woody
 * prepared for revision control
 *
 * Revision 2.36  1995/07/24  18:39:52  Jof
 * Bei QueryProp nicht this_object() als Argument
 *
 * Revision 2.35  1995/06/26  15:22:16  Jof
 * SWITCH jetzt lesbarer
 *
 * Revision 2.34  1995/06/26  09:45:53  Jof
 * switch to speed things up
 *
 * Revision 2.33  1995/06/26  08:18:15  Jof
 * enforce shared strings
 *
 * Revision 2.32  1995/06/07  16:20:27  Jof
 * Argl
 * ARGL
 * A R G L
 * Da war ein Typo - 8129 statt 8192 ....
 * A   R   G   L
 * Der hat mich 3 oder 4 Stunden gekostet, weil ich ihn DA nicht vermutet
 * hatte ...
 * WAAAAHHHHHHHH!!!
 *
 * Revision 2.31  1995/06/07  14:48:12  Jof
 * *** empty log message ***
 *
 * Revision 2.29  1995/05/15  06:58:33  Jof
 * NOSETMFOUND etc
 * ,
 *
 * Revision 2.28  1995/05/08  17:23:55  Jof
 * Bug in query_prop
 *
 * Revision 2.27  1995/05/03  19:20:33  Jof
 * no nomask ...
 *
 * Revision 2.26  1995/05/02  17:26:53  Jof
 * _locally_handled -> -2
 *
 * Revision 2.25  1995/05/02  10:10:14  Jof
 * Special hack ("_locally_handled") for Props w/o method ...
 * may cost MUCH memory, but preserve LOTS of time. Need to test.
 *
 * Revision 2.24  1995/05/02  08:51:16  Jof
 * Trade of some memory for performance by creating closures for each _query_
 * and _set_ methods. Keep this closures for further usage. As they are bound
 * to this_object, there should be no problem.
 *
 * Revision 2.23  1995/03/30  13:48:07  Jof
 * ???
 *
 * Revision 2.22  1995/01/06  16:35:02  Jof
 * check for destructed objeckt with closure-methods
 *
 * Revision 2.21  1994/11/21  19:18:53  Kirk
 * added F_MODE_AS, F_MODE_AD
 *
 * Revision 2.20  1994/10/04  09:24:02  Jof
 * *** empty log message ***
 *
 * Revision 2.20  1994/10/03  21:46:25  mud
 * Optimizations
 *
 * Revision 2.19  1994/09/22  09:28:46  Jof
 * (Rumata) aufraeumaktion (keine Aenderungen)
 *
 * Revision 2.18  1994/08/22  19:48:59  Jof
 * *** empty log message ***
 *
 * Revision 2.17  1994/08/16  17:41:36  Jof
 * *** empty log message ***
 *
 * Revision 2.15  1994/06/17  09:11:44  Jof
 * *** empty log message ***
 *
 * Revision 2.14  1994/06/13  06:52:38  Jof
 * Save memory in some ways ...
 *
 * Revision 2.13  1994/06/12  19:54:43  mud
 * Neue interne Struktur - THIS SAVES MUCH MEMORY !
 */                            

// PROPERTIES.C -- normalised propertyhandling
//
// (c) 1993 Hate@MorgenGrauen, Mateese@NightFall
//          Idea and Code      Flames and Destructions
// -> *grin* thats the point actually :) 
//
// IDEA: normalising the propertysystem and providing and easy to use and
//       efficient design to produce configurable objects
//
// IMPLEMENTATION:
//           * everything is a property (even the functions over properties)
//             there us just one exception the set/query filter functions :(
//           * two functions Set() and Query() provide direct access to it 
//             (and ONLY direct access)
//           * two further functions SetProp() and QueryProp() use the function
//             funcall() to apply the standard methods F_SET and F_QUERY for 
//             the specific property
//
// AENDERUNGEN (12.Jun.94/Jof)
//           * Properties werden jetzt in einem Array von 4 Mappings ab-
//             gelegt; 0-Elemente werden entfernt. Damit ist magier:jof
//             von ca 36 KB auf ca 30 KB geschrumpft
//           * Die LFUN-Closures #'this_object->set_... und query_ ...
//             entfernt, stattdessen call_resolved in Set und Query.
//             Langsamer, mag als Rueckschritt erscheinen. Aber:
//             magier:jof ist von ca 30 auf ca 27.5 KB geschrumpft - und
//             unsere Speichersituation ist verzweifelt. HOPE THIS HELPS
//           * HACK: Wenn die spezielle SetMethod -1 uebergeben wird, wird
//             stattdessen das Flag NOSETMETHOD gesetzt - hat nochmal einige
//             100 Bytes gespart. Ich hoffe, Hate lyncht mich nicht - aber
//             Einsparungen waren definitiv notwendig :(
// 
// INTERNALS:
//           * properties are stored in a mapping
//
// The propertymapping:
// STRUCTURE: ([ name : Value ; flags ; set_method ; query_method , ...])
// JETZT NEU:
// STRUCTURE: ({([ name : Value, ...]),
//              ([ name : flags, ...]),
//              ([ name : set_method, ...]),
//              ([ name : query_method, ...])
//            })
// name 	-- Name of the property
// Value    	-- the value of any type
// set_method	-- a method executed when SetProp() is called
// query_method	-- a method executed when QueryProp() is called
// flags	-- contains needed flags
//			PROTECED - any set of the property is protected
//			SAVE     - property will be saved with save_object()

#pragma strong_types

#define NEED_PROTOTYPES 1

#include "/sys/thing/properties.h"
#include "/secure/wizlevels.h"

// the mapping where the actual properties are stored
static mapping *prop;

// the mapping that is used for saving
mapping properties;

static mixed _query_uid() { return getuid(this_object()); }
static mixed _query_euid() { return geteuid(this_object()); }

static void InitializeProperties()
{
  prop=({([]),([]),([]),([])});
    
  Set(P_UID, -1, F_SET_METHOD);
  Set(P_UID, SECURED, F_MODE_AS);
  Set(P_EUID, -1, F_SET_METHOD);
  Set(P_EUID, SECURED, F_MODE_AS);
}

void create() { InitializeProperties(); }
     
static int allowed()
{
  if (previous_object() && IS_ARCH(getuid(previous_object())) &&
      this_interactive() && IS_ARCH(this_interactive()))
    return 1;
  return 0;
}

// Set() -- provides direct access to a property, no filters
//
varargs mixed Set(string name, mixed Value, int Type)
{
  if(!pointerp(prop)) InitializeProperties();
  
  if(extern_call() && previous_object() != this_object() && !allowed() && 
     (prop[F_MODE][name] & (PROTECTED|SECURED)))
    return -1;
  
  if ((Type == F_MODE || Type == F_MODE_AD ) && (Value & SECURED) &&
                                   (prop[F_MODE][name] & SECURED))
    return -2;
  
  if(Type == F_MODE) prop[F_MODE][name] ^= Value;
  else if(Type == F_MODE_AS) prop[F_MODE][name] |= Value;
  else if(Type == F_MODE_AD) prop[F_MODE][name] &= ~Value;
  else
  {
    if((Type == F_SET_METHOD || Type == F_QUERY_METHOD) && 
       (closurep(Value) && !query_closure_object(Value)))
    {
      if(extern_call() &&
	 (geteuid(previous_object())!=geteuid(this_object()) ||
	  getuid(previous_object())!=getuid(this_object())))
	return prop[Type][name];
      Value=bind_lambda(Value, this_object());
    }
    if (Value==-1 && Type==F_SET_METHOD)
    {
      Value=0;
      prop[F_MODE][name]|=NOSETMETHOD;
    }
    prop[Type][name] = Value;
  }
  if (Type == F_MODE_AS || Type == F_MODE_AD) Type = F_MODE;
  if (!prop[Type][name]) efun::m_delete(prop[Type],name);
  return prop[Type][name];
}

// Query() -- directly retrieves the current value of the property, no filters
//
varargs mixed Query(string name, int Type)
{
  if(!pointerp(prop)) InitializeProperties();
  return prop[Type][name];
}

// Quick hack to improve performance
#define Query(name,type) prop[type][name]
#define iSet(name,value,type) if (value) prop[type][name]=value;\
                              else efun::m_delete(prop[type],name);

// SetProp() -- filters the value of the property with F_SET method
//
mixed SetProp(string name, mixed Value)
{
  closure func;
  mixed result;
  
  if(!objectp(this_object())) return -1; // Klappt offenbar NICHT im Create !
  if(!pointerp(prop)) InitializeProperties();

  if (Query(name,F_MODE)&NOSETMETHOD)
    return -1;

  if (func = Query(name, F_SET_METHOD))
  {
    if (!closurep(func))
      return func;
    if (objectp(query_closure_object(func)))
      return funcall(func, Value, name);
    iSet(name,0,F_SET_METHOD);
  }

  if (call_resolved(&result,this_object(),"_set_"+name,
		    Value))
    return result;
  Set(name, Value, 0);
  return Value;
}

// QueryProp() filters the value of the property with F_QUERY method
//
mixed QueryProp(string name)
{
  closure func;
  mixed result;
  
  if(!objectp(this_object())) return -1;
  if(!pointerp(prop)) InitializeProperties();

  if (func = Query(name, F_QUERY_METHOD))
  {
    if (!closurep(func))
      return func;
    if (objectp(query_closure_object(func)))
      return funcall(func);
    iSet(name,0,F_QUERY_METHOD);
  }
  if (call_resolved(&result,this_object(),"_query_"+name))
    return result;
  return Query(name,0);
}

// SetProperties() -- sets all properties in one step
//
void SetProperties(mapping props)
{
  mixed name;
  int i,j;
 
  if(!pointerp(prop)) InitializeProperties();
  if(mappingp(props))
  {
    int same_object;

    same_object = (!extern_call() || previous_object() == this_object());
    name = m_indices(props);
    for(j=sizeof(name)-1; j>=0; j--)
      if(same_object || !(prop[F_MODE][name[j]] & (PROTECTED|SECURED)))
      	for (i=0;i<4;i++)
          if (props[name[j],i])
            prop[i][name[j]]=props[name[j],i];
          else
            prop[i]=m_delete(prop[i],name[j]);
  }
}

// QueryProperties() -- returns a whole mapping with all properties
//
mapping QueryProperties()
{
  mapping props;
  int i,j;
  string *name;

  if(!pointerp(prop)) InitializeProperties();
  props=m_allocate(0,4);
  
  for (i=0;i<4;i++)
  {
    name=m_indices(prop[i]);
    for (j=sizeof(name)-1;j>=0;j--)
      props[name[j],i]=prop[i][name[j]];
  }
  return props;
}

mixed *__query_properties()
{
  return ({copy(prop[0]),copy(prop[1]),
	   copy(prop[2]),copy(prop[3])});
}

// INTERNAL
// _set_save_data() -- sets the non static mapping properties for save
// _get_save_data() -- returns the mapping properties for restore
//
public void _set_save_data(mixed data) { properties = data; }
public mixed _get_save_data() { return properties; }
