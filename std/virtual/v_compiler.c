// SilberLand MUDlib
//
// std/virtual/v_compiler.c -- a general virtual compiler object
//
// $Date: 1997/08/29 18:56:41 $
/* $Revision: 2.1 $
 * $Log: v_compiler.c,v $
 * Revision 2.1  1997/08/29 18:56:41  Woody
 * fuer FPs vorbereitet
 *
 * Revision 2.0  1997/03/13 02:43:30  Woody
 * prepared for revision control
 *
 * Revision 1.6  1995/07/11  14:54:19  Jof
 * turned Validate into ME->Validate
 *
 * Revision 1.5  1994/07/27  14:50:14  Jof
 * suid
 *
 * Revision 1.4  1994/04/07  09:36:31  Freund
 * *** empty log message ***
 *
 * Revision 1.3  1994/03/24  21:26:10  Hate
 * QueryObjects() added
 *
*/

// principle:
//   - inherit this object into your own 'virtual_compiler.c'
//  - customize Validate() and CustomizeObject() for you own sake
//  
//  * Validate() checks if a room filename given as argument (without path)
//    is valid and returns this filename with stripped '.c'!!
//  * CustomizeObject() uses the previous_object()->Function() strategy to
//    customize the standard object (for example to set a description)
//
// Properties: P_STD_OBJECT, P_COMPILER_PATH

#pragma strong types

inherit "/std/thing/properties";

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <defines.h>
#include <v_compiler.h>
#include <exploration.h>
#include "/std/sys_debug.h"

private static string last_loaded_file;
private static mapping objects;

void create()
{
  ::create();
	seteuid(getuid());
  SetProp(P_STD_OBJECT, "/std/room");
  SetProp(P_COMPILER_PATH, "/"+implode(explode(object_name(this_object()), "/")
                                [0..<2], "/"));
  objects = ([]);
}

string Validate(string file)
{
  if(file[<2..] == ".c") file = file[0..<3];
  EPMASTER->PrepareVCQuery(file);
  return file;
}

mixed CustomizeObject()
{
  string file;

  if(stringp(last_loaded_file)) file = last_loaded_file;
  else file = ME->Validate(explode(object_name(previous_object()), "/")[<1]);
  if(!file) return 0;
  last_loaded_file = 0;
  return file;
}

// add a new object to the object list if it compiles
static mixed AddObject(string file)
{
  object ob;
  string err;

  // clean up the object list
  objects = filter_indices(objects, 
                           lambda(({'k}), ({#'objectp, 
                                            ({#'[, ({#'objects}), 'k})
                                          })));

  last_loaded_file = file;
  // register new object
  if(ob = clone_object(QueryProp(P_STD_OBJECT)))
    objects[file] = ob;
  return ob;
}

// try to create an object for the wanted file
mixed compile_object(string file)
{
  // validate if the file name is a correct one
  if(file = ME->Validate(file)) return AddObject(file);
}  

// return all cloned virtual objects
mixed QueryObjects()
{
  return m_values(objects)-({0});
}

// clean up rooms that have not been destructed yet
int remove()
{
  mixed ob;

  if(!mappingp(objects)) return 0;
  for(ob = QueryObjects(); sizeof(ob); ob = ob[1..])
    if(objectp(ob[0]))
    {
      ob[0]->remove();
      if(objectp(ob[0])) destruct(ob[0]);
    }
  return 1;
}
