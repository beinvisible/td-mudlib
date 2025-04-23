// SilberLand MUDlib
//
// file_sys.c -- filesystem command handling
//
// $Date: 2001/01/13 10:24:16 $
/* $Revision: 1.2 $
 * $Log: filesys.c,v $
 * Revision 1.2  2001/01/13 10:24:16  Samtpfote
 * Befehle cp, rm, mkdir, rmdir und aehnliche erst ab Level 20
 *
 * Revision 1.1  2001/01/13 10:21:24  Samtpfote
 * Initial revision
 *
 * Revision 2.8  1998/01/03 13:36:53  Woody
 * \e und \a fuer den prompt Befehl eingebaut
 *
 * Revision 2.7  1997/10/24 16:50:06  Woody
 * Bugfix in _glob()
 *
 * Revision 2.6  1997/10/19 16:18:29  Woody
 * Bugfix in _grep()
 *
 * Revision 2.5  1997/07/06 15:59:32  Woody
 * An neues show_dir() Verhalten angepasst (ls -l)
 *
 * Revision 2.4  1997/06/12 12:51:07  Crea
 * added \P for better path-display
 *
 * Revision 2.3  1997/05/30 02:06:32  Woody
 * *argl* hatte short_path() vergessen...
 *
 * Revision 2.2  1997/05/30 02:02:55  Woody
 * \p im prompt fuer pretty path
 *
 * Revision 2.1  1997/05/19 16:41:22  Woody
 * remote man handling (man -r mudname bzw. -R) eingebaut
 *
 * Revision 2.0  1997/03/13 02:08:27  Woody
 * prepared for revision control
 *
 * Revision 1.23  1997/03/12  22:01:36  Woody
 * man Befehl ueberarbeitet (section Angabe moeglich, nur mehr eine
 * Seite auf einmal (das war eh mehr bug als feature :) ))...
 *
 * Revision 1.22  1995/08/03  21:48:47  Hate
 * removed error message when no file is given to upd
 *
 * Revision 1.21  1995/05/22  13:47:18  Hate
 * fixed bug with USAGE() it didn'treturn sometimes
 *
 * Revision 1.20  1995/03/16  14:47:38  Wargon
 * man -M geht jetzt...
 *
 * Revision 1.20  1995/03/16  14:47:38  Wargon
 * man -M geht jetzt...
 *
 * Revision 1.19  1995/03/03  19:58:39  Jof
 * _ed_stack STATIC now
 *
 * Revision 1.18  1995/02/21  11:50:57  Jof
 * ...
 *
 * Revision 1.17  1995/02/14  14:20:47  Jof
 * ls-bug (break_string raus)
 *
 * Revision 1.16  1995/02/06  22:20:11  Jof
 * .ftp -> .readme
 *
 * Revision 1.15  1995/01/16  09:05:13  Jof
 * *** empty log message ***
 *
 * Revision 1.14  1995/01/09  12:08:42  Jof
 * *** empty log message ***
 *
 * Revision 1.13  1994/12/12  16:06:35  Jof
 * *** empty log message ***
 *
 * Revision 1.12  1994/12/12  15:32:34  Hate
 * *** empty log message ***
 *
 * Revision 1.10  1994/10/04  09:26:59  Jof
 * *** empty log message ***
 *
 * Revision 1.10  1994/10/04  18:21:26  mud
 * reconnect ...
 *
 * Revision 1.9  1994/09/22  16:19:40  Hate
 * added handling of virtual compilers to ls
 *
 * Revision 1.8  1994/07/14  15:16:34  Jof
 * *** empty log message ***
 *
 * Revision 1.7  1994/07/12  00:06:17  Hate
 * *** empty log message ***
 *
 * Revision 1.6  1994/06/13  16:46:18  Jof
 * Reinsertet #'...
 *
 * Revision 1.5  1994/06/13  07:36:24  Jof
 * Removed SET_METHODs
 *
 * Revision 1.4  1994/06/07  17:29:47  Jof
 * Bugfix in find_file
 *
 * Revision 1.3  1994/06/03  08:55:37  Hate
 * *** empty log message ***
 *
 * Revision 1.2  1994/06/03  07:18:38  Hate
 * bugfix in normalize (did not expand ~/ to /players/user/)
 *
 * Revision 1.1  1994/06/01  15:18:56  Hate
 * Initial revision
 *
*/

#pragma strong_types

private inherit "/std/shells/filesys/manual";
private inherit "/std/shells/filesys/primitives";
private inherit "/std/shells/filesys/asynchron";
private inherit "/std/shells/filesys/make";

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <properties.h>
#include <defines.h>
#include <config.h>
#include <moving.h>
#include <wizlevels.h>
#include <udp.h>
#include <masterflags.h>

#include "make.h"

#undef NAME  // in udp.h *UND* master.h deklariert :(
#include "/std/sys_debug.h"
#define C	(LFUN|CALL|RET|ARGS|APPLY|APPLY|HB|NAME)

#define GET_PATH	(symbol_function("_get_path", find_object(MASTER)))
#define USAGE(str)	(notify_fail(sprintf("usage: %s\n", str)), 0)

#define TW(x) if (find_player("woody")) tell_object(find_player("woody"),x)

private static string *_ed_stack = ({});	// ed/more file stack (item 0 contains
                                  // function to call

// prototypes
static int _prompt(string prompt);
static int _cd(string path);
private string _set_currentdir(string path);
private string _query_currentdir();

static mixed *_query_localcmds()
{
  return ({
    ({"ls","_ls",0,LEARNER_LVL}),
    ({"cd","_cd",0,LEARNER_LVL}),
    ({"pwd","_pwd",0,LEARNER_LVL}),
    ({"prompt","_prompt",0,LEARNER_LVL}),
    ({"cp","_cp",0,WIZARD_LVL}),
    ({"mv","_mv",0,WIZARD_LVL}),
    ({"rm","_rm",0,WIZARD_LVL}),
    ({"mkdir","_mkdir",0,WIZARD_LVL}),
    ({"rmdir","_mkdir",0,WIZARD_LVL}),
    ({"cat","_cat",0,LEARNER_LVL}),
    ({"head","_cat",0,LEARNER_LVL}),
    ({"tail","_cat",0,LEARNER_LVL}),
    ({"man","_man",0,LEARNER_LVL}),
    ({"more","_edit",0,LEARNER_LVL}),
    ({"ed","_edit",0,WIZARD_LVL}),
    ({"grep","_grep",0,LEARNER_LVL}),
    ({"upd","_upd",0,WIZARD_LVL})
  });
}

void initialize()
{
  Set(P_CURRENTDIR,#'_set_currentdir,F_SET_METHOD);
  _prompt(QueryProp(P_PROMPT));
  /* cd to home directory, if existing */
  _cd(({string})0);
}

// base() -- returns the basename of the file
private string base(string file) 
{ string *tmp; return sizeof(tmp = explode(file, "/"))?tmp[<1]:""; }

// path() -- returns the path part of the filename
private string path(string file) 
{ return (file[0]=='/'?"/":"")+implode(explode(file,"/")[0..<2],"/"); }

// normalize() -- gives the full path and name of the file
// additionas arguments: the current path (__path) and the normalization
// function (usually _get_path() in master.c) (__norm)
// normalizes only basenames!! /pl*/ha* will not be normalized
private string normalize(string file, 
                         string __path, closure __norm)
{ return funcall(__norm, (file[0]=='/'||file[0]=='~'||file[0]=='+'?
                          file:__path+file), getuid(ME)); }

// _glob() -- globbing a string
// additional arguments: the resulting command line array (line) to be given
// by reference, the current path (__path) and a normalization function (__norm)
private void _glob(string str, string *line, string __path, closure __norm)
{
  string *tmp, p;
  while(str[0] == ' ') str = str[1..];
  if(str[0] == '"') { line += ({ str[1..<2] }); return; } // preserve strings
  if(str[0] == '-') { line += ({ str }); return; }	  // preserve flags
  if((member(str, '*') != -1 || member(str, '?') != -1) &&
     (tmp = get_dir(p = normalize(str, __path, __norm))))
  {
    if(base(str)[0] != '.') tmp = regexp(tmp, "^[^.].*");
    line += map(tmp, lambda(({'s}), ({#'+,
        path(p)=="/" ? "/" : path(p)+"/", 's})));
  }
  else
    line += ({ normalize(str, __path, __norm) });
}

// glob() -- command line globbing
// takes the commandline as argument and returns an array with all semantic
// parts
private string *glob(string cmdline)
{
  int i;
  string *line; line = ({});
  map(regexplode(cmdline, "[\"][^\"]*[\"]| -[a-zA-Z][a-zA-Z]*| ")
           -({" ", ""}),
            #'_glob, &line, QueryProp(P_CURRENTDIR)+"/", GET_PATH);
  return line - ({""});
}

// GetOpt() -- get all options (-blafu)
// takes as first arguement an array containing strings like "-xyz", the
// possible option characters as second and the command calling as third.
// returns an integer with bits set when matching option is found
private int GetOpt(string *cmdline, string opts, string __cmd)
{
  int flags;
  while(sizeof(cmdline) && cmdline[0][0] == '-')
  {
    int i, f; i = sizeof(cmdline[0]);
    while(i-- && cmdline[0][i] != '-')
      if((f = member(opts, cmdline[0][i])) != -1) 
        flags |= (1 << f);
      else return (printf("%s: illegal flag '%c'\n", __cmd, cmdline[0][i]), -1);
    cmdline = cmdline[1..];
  }
  return flags;
}

// ls command
#define LS_A	0x01			// list all files
#define LS_L	0x02			// long listing
#define LS_OPTS		"al"

// strlcmp() -- compares the length of two strings (returns s1 < s2)
private int strlcmp(string s1, string s2) { return sizeof(s1) < sizeof(s2); }

// add_slash() -- adds a slash to directories and * to loaded blueprints
private string add_slash(string file, string p) 
{
  if(file_size((file[0]=='/'?"":p)+file)==-2) return file + "/";
  if(objectp(find_object((file[0]=='/'?"":p)+file))) return file + "*";
  return file;
}

// _ls_output() -- creates output from files given
// if file is a dir directories is extended by the listed dir
// if LS_A flag is set, also .* files will be listed
private mixed _ls_output(string file, string directories, int __flags)
{
  string *files;
  string a_file;
  mixed tmp;
  int i,num,a;

  if(files = get_dir(file+"/*"))
  { int tab;
    object vc;
    if(vc = find_object(file+"/virtual_compiler"))
      if(call_resolved(&tmp, vc, "QueryObjects") && pointerp(tmp))
        files += map(tmp, lambda(({'o}), 
                                       ({#'base, ({#'object_name, 'o}) })));
      else printf("ls: /%O is not a standard virtual compiler\n", vc);
    if(!(__flags & LS_A)) files = regexp(files, "^[^.].*");
    files = map(files, #'add_slash, file+"/");
    num=0;
    directories+=sprintf("%s: %d files/directories\n",file,sizeof(files));
    tmp=sizeof(files);
    for (i=0;i<tmp;i++)
    {
    	a_file=sprintf("%-18s ",files[i]);
	a=sizeof(a_file);
	if (a>19) 
	{
	  a_file+=sprintf("%"+(19-a%19)+"s","");
	  a+=19-a%19;
	}
	num+=(18+a)/19;
	if (num>4)
	{
	  while (directories!=""&&directories[<1]==' ') 
	    directories=directories[0..<2];
	  directories+="\n";
	  num=(18+a)/19;
        }
	directories+=a_file;
    }
    directories+="\n";
    return 0;
  }
  return base(add_slash(file, ""));
}

// _long() -- returns a long (LS_L) listing of the directory
/*
private void _long(string directory, int flags, closure __get_dir, int sdflags)
{ write(funcall(__get_dir, directory, 0, sdflags)); }
*/

// _ls() -- list directories
static int _ls(string args)
{
  int flags;
  string *pargs, *files, directories;
  
  if(!sizeof(pargs = glob(args=_unparsed_args()||QueryProp(P_CURRENTDIR))))
    return USAGE("ls [-al] filename");

  if((flags = GetOpt(&pargs, LS_OPTS, "ls")) == -1) return 1;

  if(!sizeof(pargs)) 
    pargs = ({ normalize(".", QueryProp(P_CURRENTDIR)+"/", GET_PATH) });

  if(flags & LS_L)
  {
    write(MASTER->show_dir(pargs, 0,
        SHOWDIR_SHORTPERM | SHOWDIR_SLASHDIR |
        (flags & LS_A ? 0 : SHOWDIR_NODOT) ));
    /*
    map(pargs, #'_long, flags,
                     symbol_function("show_dir", find_object(MASTER)), 1);
    */
    return 1;
  }
  directories = "";
  files = map(pargs, #'_ls_output, &directories, flags) - ({0});
  if(sizeof(files))
  { int tab;
    if(!(flags & LS_A)) files = regexp(files, "^[^.].*");
    if(sizeof(files)) tab = sizeof(sort_array(files, #'strlcmp)[0])+1;
    directories = break_string(sprintf("%-"+tab+"@s", files), 78)
                + "\n"+directories;
  }
  directories = implode(regexp(regexplode(directories, "\n[ ][ ]*"), 
                        "^[^\n][^ ].*"), "\n");
  write(directories);
  return 1;
}

// _cd() -- change current directory
static int _cd(string args)
{
  string *pargs, msg;
  int flags;
  
  args=(extern_call()?_unparsed_args():args)||"/players/"+getuid(ME);
  if(!sizeof(pargs = glob(args)))
    return USAGE("cd [-ls] directory");
  
  if(flags = GetOpt(&pargs, "ls", "cd"))
  {
    if(flags & 1) SetProp(P_SHORT_CWD, 0);
    if(flags & 2) SetProp(P_SHORT_CWD, 1);
    write("Directory status: "+(QueryProp(P_SHORT_CWD)?"short":"long")+"\n");
  }
  if(!sizeof(pargs)) pargs = ({ "/players/"+getuid(ME) });
  switch(file_size(pargs[0]))
  {
  case -2: break;
  case -1: return (printf("%s: no such file or directory\n", args), 1); break;
  default: return (printf("%s: not a directory\n", args), 1); break;
  }
  write(SetProp(P_CURRENTDIR, pargs[0])+"\n");
  if(!QueryProp(P_SHORT_CWD) && file_size(pargs[0] + "/.readme") > 0)
    write((msg = read_file(pargs[0] + "/.readme"))?msg:"");
  return 1;
}

// _pwd() -- print current directory
static int _pwd()
{
  write(QueryProp(P_CURRENTDIR)+"\n");
  return 1;
}

// prompt command

// _subst_prompt() -- substitute prompt macros
// takes a string and returns matching macro value
private string _subst_prompt(string str)
{
  switch(str)
  {
    case "\\h": return sizeof(MUDNAME)?MUDNAME:"NamenlosMud";
    case "\\u": return CAP(getuid(ME));
    case "\\e": return "\e";
    case "\\a": return "\a";
    default: return str;
  }
}

// _prompt() -- change the current prompt
static int _prompt(string args)
{
  string *pargs;

  args=(extern_call()?_unparsed_args():args)||"> ";
  if(args[0]=='"') args = args[1..<2];
  if(!sizeof(pargs = regexplode(args, "\\\\[huwea]"))) 
    return USAGE("prompt \"<newprompt>\"");

  pargs = map(pargs, #'_subst_prompt);
  SetProp(P_PROMPT, implode(pargs, ""));
  _set_currentdir(_query_currentdir());
  return 1;
}
    
// _cp() -- copy files 
#define CP_I		1		// copy interactive
#define CP_R		2		// copy recursively
#define CP_P		4		// preserver file status
#define CP_OPTS		"irp"	

static int _cp(string args)
{
  int flags;
  string *pargs, to;
  closure exec_cmd, qask_cmd;

  if(!(args=_unparsed_args()) || !sizeof(pargs = glob(args)))
    return USAGE("cp [-ip] f1 f2; or: cp [-irp] f1 ... fn d2");
  
  if((flags = GetOpt(&pargs, CP_OPTS, "cp")) == -1) return 1;
  if(flags & CP_I) return (printf("cp: interactive not implemented\n"), 1);
  if(flags & CP_R) return (printf("cp: recursive not implemented\n"), 1);
  if(flags & CP_P) return (printf("cp: preserve not implemented\n"), 1);

  if(!sizeof(pargs)) printf("cp: missing files\n");
  if(file_size(to = pargs[<1]) == -2)
    exec_cmd = lambda(({'f}), ({#'cp_file,'f,({#'+,to+"/",({#'base, 'f})})}));
  else 
    exec_cmd = #'cp_file;

  asynchron(pargs[0..<2], exec_cmd, ({ to }));
  return 1;
}

// _mv() -- move files
#define MV_I            1		// move interactively
#define MV_F            2		// force overwriting
#define MV_OPTS         "if"

static int _mv(string args)
{
  int flags, pos;
  string *pargs, to;
  closure exec_cmd;

  if(!(args=_unparsed_args()) || !sizeof(pargs = glob(args)))
    return USAGE("mv [-if] f1 f2 or mv [-if] f1 ... fn d1 "
                +"(`fn' is a file or directory)");

  if((flags = GetOpt(&pargs, MV_OPTS, "mv")) == -1) return 1;
  if(flags & MV_I) return (printf("mv: interactive not implemented\n"), 1);
  if(flags & MV_F) return (printf("mv: force not implemented\n"), 1);

  if(!sizeof(pargs)) printf("mv: missing files\n");
  if(file_size(to = pargs[<1]) == -2)
    exec_cmd = lambda(({'f}), ({#'mv_file,'f,({#'+,to+"/",({#'base, 'f})})}));
  else
    exec_cmd = #'mv_file;

  while((pos = member(pargs = pargs[0..<2], to)) != -1)
  {
    printf("mv: %s: invalid argument\n", pargs[pos]);
    pargs[pos..pos] = ({});
  }
  asynchron(pargs, exec_cmd, ({ to }));
  return 1;
}

// _rm() -- remove files
#define RM_R		1		// remove recursively
#define RM_I		2		// remove interactively
#define RM_F		4		// force remove (not even interactive)
#define RM_OPTS         "rif"

static int _rm(string args)
{
  int flags, *ret;
  string *pargs, to;
  closure exec_cmd;

  if(!(args=_unparsed_args()) || !sizeof(pargs = glob(args)))
    return USAGE("rm [-"+RM_OPTS+"] file ...");

  if((flags = GetOpt(&pargs, RM_OPTS, "rm")) == -1) return 1;
  if(flags & RM_R) return (printf("rm: recursive not implemented\n"), 1);
  if(flags & RM_I) return (printf("rm: interactive not implemented\n"), 1);
  if(flags & RM_F) return (printf("rm: force not implemented\n"), 1);

  if(!sizeof(pargs)) printf("rm: missing files\n");
  asynchron(pargs, #'rm_file);
  return 1;
}

// _mkdir() -- make or remove directories 
static int _mkdir(string args)
{
  string *pargs, msg;
  if(!(args=_unparsed_args()) || !sizeof(pargs = glob(args))) 
    return USAGE((query_verb()=="mkdir"?"mkdir":"rmdir")+" directory ...");

  asynchron(pargs, #'dir_file, ({ query_verb()=="mkdir" }));
  
  return 1;
}

// _cat() -- cat, head or tail a file
static int _cat(string args)
{
  string *pargs, verb;
  int size;
  verb = query_verb();
  if(!(args=_unparsed_args()) || 
     !(size = sizeof(pargs = glob(args))) || size > 1)
    return USAGE(verb+" file");

  switch(file_size(pargs[0]))
  {
  case -2: return (printf("%s: is a directory\n", args), 1); break;
  case -1: return (printf("%s: no such file\n", args), 1); break;
  }
  switch(verb)
  {
  case "cat" : if(!cat(pargs[0])) printf("cat: failed\n"); break;
  case "head": if(!cat(pargs[0], 0, 10)) printf("head: failed\n"); break;
  case "tail": tail(pargs[0]); break;
  }
  return 1;
}

// man command

// remote_man() -- remote manual handling
// takes the key and the mud (or 0) as arguments
private int remote_man(string key, string mud) {
  object zettel;
  string x;
  zettel=clone_object("/obj/manhelp");
  x=INETD->send_udp(mud, ([ REQUEST: "man",
                            SENDER:  zettel,
                            DATA:    key ]), 1);
  if (x) write(x);
  write("Anfrage nach \'"+key+"\' an "+(mud?capitalize(mud):"alle bekannten "
    "Muds")+" abgeschickt.\nEin magischer Zettel flattert Dir zu.\n");
  zettel->SetKey(key);
  zettel->move(this_player(), M_NOCHECK);
  return 1;
}

// print_man() -- more the found manual
// takes the filename and the manpath as arguments
private void print_man(string file, string path)
{
  if(file_size(path+file) == -2)
    PL->More("Topics for section "+file+":\n"
         +break_string(implode(get_dir(path+file+"/*")-({".",".."}),", "), 78),
         0);
  else PL->More(path+file, 1);
}

// _man_search() -- search in manpath for given keywords
// takes the manpath and an keyword array as arguments
private void _man_search(string manpath, string keyword)
{
  // string *entries, *found;
  string *entries;
  if (!pointerp(entries = get_dir(manpath+"/*"))
      || sizeof(entries) == 1) return;
  entries -= ({".", ".."});
  if (member(entries, keyword) != -1) {
    print_man(keyword, manpath+"/");
    keyword=0;  // als 'gefunden' markieren
  }
  /* evtl. spaeter mal gegen regexp pruefen
  else if (sizeof(found=regexp(entries, keyword)))
  {
  }
  */
  else
    map(map(entries, #'normalize, manpath+"/", GET_PATH),
              #'_man_search, &keyword);
}

static int _man(string args)
{
  string *pargs, *tmp, manpath, word, section, mudname;
  int inter, remote;
  if(!(args=_unparsed_args()) ||
     !sizeof(pargs=regexplode(args, "-M [^ ]*|-i|-R|-r [^ ]*| ")-({"", " "})))
    return USAGE("man [-i] [-M path] [-r mud] [-R] [section] word");
    // pargs=({});      // Alternative zur Usage Msg: MAIN TOPICS auflisten

  manpath = "/doc";
  section = "";
  if(member(pargs, "-i") != -1)
  {
    pargs -= ({ "-i" }); inter = 1;
    manpath = "/htmldoc";
  }
  if(member(pargs, "-R") != -1)
  {
    pargs -= ({ "-R" }); remote = 1;
  }
  if(sizeof(tmp = regexp(pargs, "^-M")))
  {
    if(sizeof(tmp[0]))
    {
      pargs -= tmp;
      tmp = explode(tmp[0], " ");
      if (sizeof(tmp) > 1)
	manpath = normalize(tmp[1], QueryProp(P_CURRENTDIR)+"/", GET_PATH);
      else return (printf("man: missing path for option -M\n"), 1);
    }
    else return (printf("man: missing path for option -M\n"), 1);
  }
  if(sizeof(tmp = regexp(pargs, "^-r")))
  {
    if(sizeof(tmp[0]))
    {
      pargs -= tmp;
      remote = 1;
      tmp = explode(tmp[0], " ");
      if (sizeof(tmp) > 1)
        mudname = tmp[1];
      else return (printf("man: missing mudname for option -r\n"), 1);
    }
    else return (printf("man: missing mudname for option -r\n"), 1);
  }

  if (remote)
  {
    if (!sizeof(pargs)) write("man: missing keyword for remote man query.\n");
    else remote_man(pargs[0], mudname);
    return 1;
  }
  switch (sizeof(pargs))
  {
    case 0:
      write("MAIN TOPICS:\n"+break_string(implode(
          filter(get_dir(manpath+"/*"), lambda( ({'x}),
              ({#'!=, ({ #'[, 'x, 0 }), '.' }) )), ", "), 78));
      return 1;
      break;
    case 1:
      // word=pargs[0];
      break;
    default:
      if ((section=pargs[0])=="/") section="";
      pargs=pargs[1..];
      // word=pargs[1];
      // word=implode(pargs[1..], " ");
  }
  word=pargs[0];
  manpath+="/"+section;
  if(inter)
  {
    Manual(word, manpath);
    return 1;
  }
  else
  {
    // _man_search(manpath,&word);
    "/obj/manbuf"->query_man(manpath, &pargs);
  }
  if (sizeof(pargs))
    write("No manual entry for "+implode(pargs, ", ")+
        (section != "" ? " in section "+section : "")+"\n");
  /*
  if (word)
    write("No manual entry for "+word+
        (section != "" ? " in section "+section : "")+"\n");
  */
  return 1;
}

// grep command
#define GREP_C		1		// count only found lines
#define GREP_H		2		// do not display file names
#define GREP_I		4		// ignore case
#define GREP_L		8		// display explicitely file names
#define GREP_N		16		// enumerate lines
#define GREP_V		32		// print out lines in reverse order
#define GREP_OPTS	"chilnv"

// _do_grep() -- grep in a given file for a given pattern
// takes the file, a regular expression and flags as arguments
private void _do_grep(string file, string rexpr, int flags)
{
  string *result, msg; msg = "";
  switch(file_size(file))
  {
  case -2: return;
  case -1: return printf("%s: no such file or directory\n", file); break;
  }
  if(!pointerp(result = grep_file(file, rexpr, flags)) ||
     !sizeof(result)) return;
  if(flags & GREP_L) return write(file+"\n");
  if(!(flags & GREP_H)) msg += file+":\n";
  if(flags & GREP_C) return write(msg + sizeof(result)+"\n");
  write(msg+implode(result, "\n")+"\n");
}

static int _grep(string args)
{
  int flags, pos;
  string *pargs, rexpr, *tmp;

  if(!(args=_unparsed_args()))
    return USAGE("grep [-"+GREP_OPTS+"] expression file ...");

  pargs = regexplode(args, "-["+GREP_OPTS+"]*")-({"", " "});
  if((flags = GetOpt(&pargs, GREP_OPTS, "grep")) == -1) return 1;
  if(!sizeof(pargs)) return (printf("grep: missing expression\n"), 1);
  tmp = regexplode(pargs[0], "\"[^\"]*\"|[ ]")-({"", " "});
  rexpr = tmp[0]; if(rexpr[0]=='"') rexpr = rexpr[1..<2];
  pargs = glob(implode(tmp[1..], " "));

  if(!sizeof(pargs)) printf("grep: missing files\n");
  if(flags & GREP_I) rexpr = lower_case(rexpr);
  asynchron(pargs, #'_do_grep, ({ rexpr, flags }));
  return 1;
}

// ed/more command
// edit_file() -- edit/more the first file on _ed_stack
static void edit_file()
{
  mixed funct;
  if(sizeof(_ed_stack) > 1) 
  {
    string tmp; tmp = _ed_stack[1]; _ed_stack[1..1] = ({});
    printf("Next file: %s:\n", tmp);
    funct = _ed_stack[0];
    if(funct == #'ed) funcall(_ed_stack[0], tmp, "edit_file");
    else funcall(_ed_stack[0], tmp, 1, #'edit_file);
  }
  else return printf("No more files!\n");
}

// _edit() -- edit/more files
static int _edit(string args)
{
  string *pargs, verb;
  int *size, i;
  verb = query_verb();
  if(!(args=_unparsed_args()) || !sizeof(pargs = glob(args)))
    return USAGE(verb+" file ...");

  size = map(pargs, #'file_size);
  while((i = member(size, -2)) != -1) 
  {
    printf("%s: %s is a directory\n", verb, pargs[i]);
    pargs[i..i] = ({}); size[i..i] = ({});
  }
    
  _ed_stack = ({ verb == "ed" ? #'ed : symbol_function("More", ME) }) + pargs;
  edit_file();
  return 1;
}

// upd command


// _upd() -- update files
static int _upd(string args)
{
  int flags, *ret;
  string *pargs;

  if(!(args=_unparsed_args()) || !sizeof(pargs = glob(args)))
    return USAGE("upd [-"+UPD_OPTS+"] file ...");

  if((flags = GetOpt(&pargs, UPD_OPTS, "upd")) == -1) return 1;
  if((flags & UPD_A) && !IS_ELDER(ME)) 
    return (printf("upd: -a flag only for elder use\n"), 1);

  if(!sizeof(pargs)) pargs = ({ 0 });
  asynchron(pargs, #'_make, ({ flags }));
  return 1;
}

static string short_path(string str)
{
  string erg;
  erg=regreplace(str, "^/d/", "+", 1);
  erg=regreplace(erg, "^/players/"+getuid(this_object()), "~", 1);
  erg=regreplace(erg, "^/players/", "~", 1);
  return erg;
}

static string short_path2(string str)
{
  string erg;
  erg=regreplace(str, "^/d/", "+", 1);
  erg=regreplace(erg, "^/players/"+getuid(this_object()), "~", 1);
  erg=regreplace(erg, "^/players/", "~", 1);
  return erg+(erg=="/"?"":"/");
}

// property methods
private string _set_currentdir(string path)
{
  string promptstring; 
  
  if(!(promptstring = QueryProp(P_PROMPT)) || promptstring=="")  
    promptstring="> ";
  else {
    promptstring=implode(efun::explode(promptstring,"\\w"), path);
    promptstring=implode(efun::explode(promptstring,"\\p"), short_path(path));
    promptstring=implode(efun::explode(promptstring,"\\P"), short_path2(path));
    set_prompt(promptstring);
  }
  return Set(P_CURRENTDIR, path);
}

private string _query_currentdir()
{
  string c;
  if(!c=Query(P_CURRENTDIR)) return Set(P_CURRENTDIR, "/players/"+getuid(ME));
  return c;
}

static void reconnect()
{
  _cd(QueryProp(P_CURRENTDIR));
}

// TO BE REMOVED

mixed find_file(string str, string suffix)
{
  string res;
  res = normalize(str+(stringp(suffix)?suffix:""), 
                  QueryProp(P_CURRENTDIR)+"/", GET_PATH);
  if(file_size(res)>=0) return res;
  return 0;
}

