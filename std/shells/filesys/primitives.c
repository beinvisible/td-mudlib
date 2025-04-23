//
// Wunderland MUDlib
//
// std/shells/filesys/primitives.c -- file command primitives
//
// basierend auf Morgengrauen MUDlib
//
// $Log: primitives.c,v $
// Revision 1.11  2021/03/25 16:06:39  Fuchur
// Update auf ldmud 3.6.3
//
// Revision 1.10  2003/10/09 14:12:47  Fiona
// Zwei neue Fehlermeldungen
//
// Revision 1.9  2003/10/09 13:06:06  Fiona
// benutzt nun copy_file(E)
//

#pragma strong_types

#include <defines.h>
#include <config.h>
#include <files.h>
#include "/std/sys_debug.h"

#define MAXLEN	20000

#define EXEC	2		// file status (loadable)
#define WRITE	1		// writeable
#define READ	0		// readable

#define ERROR(msg, arg)		(printf(msg, arg), -1)

#define NO_READ			"%s: no read permissions\n"
#define NO_WRITE		"%s: no write permissions\n"
#define DOESNT_EXIST		"%s: file doesn't exist\n"
#define ALREADY_EXISTS		"%s: file exists\n"
#define FILE_IS_DIR             "%s: file is directory\n"
#define CANNOT_REMOVE           "%s: cannot remove file\n"
#define SAME_FILE               "%s: source and dest are the same file\n"
#define DIR_DOESNT_EXIST        "%s: directory doesn't exist\n"
#define DIR_IS_FILE             "%s: not a directory\n"

// exists() -- returns if the file or directory exists
private int exists(string file)
{ int size; return (size = file_size(file)) >= 0 || size == -2; }

private int dir_exists(string file) {
  int i;
  i = rmember(file, '/');
  return file_size(file[0..i-1]);
}


// access() -- checks the access rights for a file, returns bitfield
private int access(string file, int what)
{
  if (what==READ)
    return !!(MASTER->valid_read(file, getuid(ME), "valid_read_file", ME));
  if (what==WRITE)
    return !!(MASTER->valid_write(file, getuid(ME), "valid_write_file", ME));
  return 0;
}

// cp_file() -- copies from to to
// overlong (50k) files will be split
static int cp_file(string from, string to) {
  int ptr, sz;
  if (from==to) return ERROR(SAME_FILE, from);
  sz=file_size(from);
  if (sz==-2) return ERROR(FILE_IS_DIR, from);
  if (sz==-1) return ERROR(DOESNT_EXIST, from);
  if (!access(from, READ)) return ERROR(NO_READ, from);
  sz = dir_exists(to);
  if (sz >= 0) return ERROR(DIR_IS_FILE, to);
  else if (sz != FSIZE_DIR) return ERROR(DIR_DOESNT_EXIST, to);
  if (!access(to, WRITE)) return ERROR(NO_WRITE, to);
  if (exists(to)) rm(to);
  if (exists(to)) ERROR(CANNOT_REMOVE, to);

  return !copy_file(from, to);
}

// mv_file() -- moves file from to to
static int mv_file(string from, string to) {
  int sz;
  if (from==to) return ERROR(SAME_FILE, from);
  if (!exists(from)) return ERROR(DOESNT_EXIST, from);
  if (!access(from, READ)) return ERROR(NO_READ, from);
  if (!access(from, WRITE)) return ERROR(NO_READ, from);
  if (!access(to, WRITE)) return ERROR(NO_WRITE, to);
  sz = dir_exists(to);
  if (sz >= 0) return ERROR(DIR_IS_FILE, to);
  else if (sz != FSIZE_DIR) return ERROR(DIR_DOESNT_EXIST, to);
  rename(from, to);
  if (exists(from)) return ERROR(CANNOT_REMOVE, from);
  return 1;
}

// rm_file() -- removes file
static int rm_file(string file) {
  if (!exists(file)) return ERROR(DOESNT_EXIST, file);
  if (!access(file, WRITE)) return ERROR(NO_WRITE, file);
  rm(file);
  if (exists(file)) return ERROR(CANNOT_REMOVE, file);
  return 1;
}

// dir_file() -- makes or removes directory
static int dir_file(string file, int mk) {
  if (mk && exists(file)) return ERROR(ALREADY_EXISTS, file);
  if (!mk && !exists(file)) return ERROR(DOESNT_EXIST, file);
  if (access(file, WRITE)) {
    if (mk) {
      if (!mkdir(file) || !exists(file)) ERROR(NO_WRITE, file);
    } else {
      if (!rmdir(file) || exists(file)) ERROR(CANNOT_REMOVE, file);
    }
  } else ERROR(NO_WRITE, file);
  return 1;
}

#define IGN_CASE	4	// ignore case
#define PUT_LNUM	16	// add line numbers
#define NO_MATCH	32	// reverse matched lines

// _put_line_number() -- adds a line number to line, taken from lines
// and adds offset
private string _put_line_number(string line, string *lines, int offset)
{ return sprintf("%4d %s", member(lines, line)+offset, line); }

// _copy_original() -- return original line from new in old 
private string _copy_original(string line, string *new, string *old)
{ return old[member(new, line)]; }

// grep_file() -- grep file for pattern
static mixed grep_file(string file, string rexpr, int flags) {
  string *lines, *tmp, *tmp1, src, *result; int ptr, lno;
  if (!exists(file)) return ERROR(DOESNT_EXIST, file);
  if (!access(file, READ)) return ERROR(NO_READ, file);
  result = ({});
  do {
    tmp1 = lines = explode(src = read_bytes(file, ptr, MAXLEN), "\n"); 
    ptr += MAXLEN + 1;
    if (flags & IGN_CASE) lines = map(lines, #'lower_case);
    tmp = regexp(lines, rexpr);
    if (flags & IGN_CASE) tmp = map(tmp, #'_copy_original, lines, tmp1);
    if (flags & NO_MATCH) tmp = lines - tmp;
    if (flags & PUT_LNUM) 
      tmp = map(tmp, #'_put_line_number, lines, lno);
    result += tmp;
    lno += sizeof(lines);
  }
  while(strlen(src) == MAXLEN);
  return result;
}

// upd_file() -- update a file
static int upd_file(string file, int dummy) {
  object obj;
  if (!exists(file)) return ERROR(DOESNT_EXIST, file);
  if (!access(file, WRITE)) return ERROR(NO_WRITE, file);
  if (!access(file, READ)) return ERROR(NO_READ, file);
  if (objectp(obj = find_object(file))) {
    string err;
    if (dummy) return 1;
    if (err = CATCH_NOLOG(obj->remove()))
      ERROR("%s: remove error, hard destruct\n", file);
    if (objectp(obj)) destruct(obj);
  }
  else return 0;
  return 1;
}

