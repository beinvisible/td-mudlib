// SilberLand MUDlib
//
// MAKE.C -- update and make objects and classes
//
// $Date: 1997/03/13 02:09:13 $
/* $Revision: 1.3 $
 * $Log: make.c,v $
 * Revision 1.3  1997/03/13 02:09:13  Woody
 * prepared for revision control
 *
 * Revision 1.2  1995/08/03  21:49:11  Hate
 * when no file is given use the last object id
 * added error message when no file is given and no last object id present
 *
 * Revision 1.1  1995/02/21  11:50:57  Jof
 * Initial revision
 *
*/

#pragma strong_types

#include <defines.h>
#include <moving.h>
#include <config.h>
#include "make.h"
#include "/std/sys_debug.h"

private object last = 0;

// base() -- returns the basename of the file
private string base(string file) 
{ string *tmp; return sizeof(tmp = explode(file, "/"))?tmp[<1]:""; }

// _save() -- move all interactives to inv_saver
private mixed _save(object o, string inv_saver)
{ 
  if(query_once_interactive(o)) 
  { 
    o->move(inv_saver, NO_CHECK);
    return o;
  }
  return 0;
}

// _load() -- load an object
// load an object and print extra backtrace if wanted
varargs private mixed _load(string file, int flags, string err)
{
  object obj;
  if(!(obj = find_object(file)))
  {
    string bt, error; mixed *log; int pos, inst, clone;

    clone = (sscanf(file, "%s#%d", file, inst) == 2);
    if(file[<2..] != ".c") file += ".c";
    if(file[0] != '/') file = "/"+file;
    if(file_size(file) < 0) 
      err = sprintf("upd: %s: warning, no file to load\n", file);
    if(sizeof(log = get_dir("/"+__HOST_NAME__+".debug.log", 2)))
      pos = log[0];
    if(err = catch(clone?(obj = clone_object(file)):call_other(file, "")))
    {
      if(sizeof(log = get_dir("/"+__HOST_NAME__+".debug.log", 2)))
        bt = read_file("/"+__HOST_NAME__+".debug.log", pos, log[0]);
      if(!bt) bt = "NO BACKTRACE\n";
      if(flags & UPD_B) err += "\nBacktrace:\n" + bt;
    }
    if(!obj && !(obj = find_object(file)))
      err += sprintf("upd: %s: missing class after successful load\n", file);
  }
  return obj;
}

// upd_file() -- update a file
varargs private int _update(string file, int dummy)
{
  object obj;
  if(!(obj = find_object(file))) return 0;
  if(!MASTER->valid_write(file, getuid(ME))) 
    return (printf("%s: no write permissions\n", file), -1);
  if(!MASTER->valid_read(file, getuid(ME))) 
    return (printf("%s: no read permissions\n", file), -1);
  if(objectp(obj))
  {
    string err;
    if(dummy) return 1;
    if(err = catch(obj->remove()))
      printf("%s: remove error, hard destruct\n", file);
    if(objectp(obj)) destruct(obj);
  }
  else return 0;
  return 1;
}

varargs static int _make(string file, int flags);

// _instance_upd() -- update all instances found for a class
private void _instance_upd(string file, int flags, mixed o, int instances)
{
  object next, env;
  string name, tfile;
  int tmp;
  while(get_eval_cost() > 80000 &&         // eval cost check
        objectp(o))
  {
    next = debug_info(2, o);
    if(sscanf(tfile = object_name(o), "%s#%d", name, tmp) == 2 && 
       name == file)
    {
      instances++;
      if(flags & UPD_F)
        printf("upd: /%O found in %s\n", o,
         (env = environment(o))?object_name(env):"no env");
      if(flags & UPD_A) _make(tfile, flags & ~(UPD_A|UPD_F));
    }
    o = next;
  }
  if(objectp(o)) call_out(#'_instance_upd, 0, file, flags, o, instances);
  else printf("upd: %s: %d instances %s\n", file, instances,
                                            (flags & UPD_A)?"updated":"found");
}

// _do_make() -- check dependencies when making
// take the file to make and the flags
varargs private int _do_make(string file, int flags, string *dep, string *ready)
{
  object obj;
  string *ilist, err, *down;
  int ret;
  if(!ready) ready = ({});
  ready += ({ file });

  if(!(obj = _load(file, flags, &err))) 
  {
    dep += ({ err });
    return 0;
  }
  ilist = map(inherit_list(obj), 
                    lambda(({'f}), ({#'+, "/", 'f}))) - ready; 
  down = ({});
  while(sizeof(ilist))
  {
    ret = _do_make(ilist[0], flags, &down, &ready) 
        || ret;
    ilist[0..0] = ({});
    ilist -= ready;
  }
  if(ret || file_time(file) > program_time(obj) || (flags & UPD_I))
    if(_make(file, flags & ~(UPD_M|UPD_I)) < 0) 
      dep = ({ "{" + base(file) + "}", down });
    else
    {
      dep = ({ "[" + base(file) + "]", down });
      ret = 1;
    }
  else if(flags & UPD_V) dep += ({ base(file), down });

  return ret;
}

// _make_dep() -- create output from recursive array (tree)
// takes an array as argument
private string _make_dep(mixed dep, string prefix)
{
  string tmp; int i, size;
  for(i = 0, size = sizeof(dep), tmp = ""; i < size; i++)
    if(pointerp(dep[i]))
      tmp += _make_dep(dep[i], prefix + (i < (size-1) ? "| " : " "));
    else 
      tmp += prefix + "+-" + dep[i] + "\n";
  return tmp ? tmp : "";
}

// _make() -- does the update
// takes a file and flags as arguments
varargs static int _make(string file, int flags)
{
  string msg, err, inv_saver, blue;
  int inst;
  object obj; msg = ""; inv_saver = INV_SAVE;

  if(!file)
    if(!last) return (printf("upd: missing files\n"), 0);
    else file = sprintf("/%O", last);
  if(sscanf(file, "%s#%d", blue, inst) == 2) 
    blue += ".c";
  else blue = file + (file[<2..]==".c"?"":".c");

  if(file == INV_SAVE || file_size(INV_SAVE) <= 0) 
  {
    inv_saver = INV_SAVE_E;
    printf("upd: %s: using %s to save inventory\n", file, inv_saver);
  }

  if(flags & UPD_F || flags & UPD_A)
  {
    if(_update(file, 1) < 1) return -1;
    if(inst)
      return (printf("upd: %s: can't find instances from instance\n",file),-1);
    if(file[<2..] == ".c") file = file[0..<3];
    call_out(#'_instance_upd, 0, file, flags, debug_info(2,0));
    if(flags & UPD_F && !(flags & (UPD_R | UPD_L))) return 0;
  }

  if((obj = find_object(file)) || (flags & (UPD_M|UPD_I)))
  {
    object *inv, env;
    mapping pro;
    if(obj)
    {
      inv = map(deep_inventory(obj), #'_save, inv_saver) - ({0});
      env = environment(obj);
      pro = obj->QueryProperties();
    }
    else inv = ({});

    if((flags & UPD_M) || (flags & UPD_I))
    {
       string *dep; dep = ({});
       _do_make(blue, flags & ~(UPD_M|UPD_L|UPD_R|UPD_F|UPD_A), &dep);
       printf(_make_dep(dep, "")+"\n");
    }
    if(_update(file) < 0) return -1;
    else msg += (inst?"destruct":"update");
    if((flags & UPD_R) || (flags & UPD_L))
    {
      if(obj = _load(file, flags, &err))
        msg += ", "+(inst ? "reclone" : "reload");
      if(!err)
      {
        obj = obj ? obj : find_object(file);
        if(pro && (flags & UPD_C))
        {
          obj->SetProperties(pro);
          msg += ", restore props";
        }
        if(env) 
          if(obj->move(env, NO_CHECK) <= 0)
            printf("upd: /%O: could not move back to /%O\n", obj, env);
          else msg += ", move";
        if(sizeof(inv -= ({0})))
        {
          map_objects(inv, "move", obj, NO_CHECK);
          msg += ", restore";
        }
      }
      else 
        return (printf("upd: %s: %s", file, err), -1);
    }
  }
  else 
    if(!_update(file) && flags & UPD_L)
      if(!_load(file, flags, &err))
        return (printf("upd: %s: %s", file, err), -1);
      else msg += "load";
  if(sizeof(msg)) write(file+": "+msg+"\n"
                       +(inst&&obj?sprintf("upd: new object: /%O\n", obj):""));
  last = obj;

  return 1;
}
