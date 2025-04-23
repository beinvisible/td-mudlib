// SilberLand MUDlib
//
// obj/manbuf.c -- buffering man page client
//
// $Date$
/* $Revision$
 * $Log$
 */


/*
 * uebernommen aus der Wunderland Mudlib
 * Kann Seiten auch als String zurueckliefern
 * Autor: Fiona@Wunderland
 */

#define START "/doc"
#pragma strong_types

static mapping cache;

private void fill_cache(string path, string key);
private void old_search(string path, string* key);

/*
 * Diese Funktion durchsucht den Cache nach einer bereits gefundenen Seite,
 * sonst wird sie gesucht und im Cache eingetragen. Ist indirekt gesetzt,
 * so wird die Seite als Funktionswert zurueckgeliefert, ansonsten wird sie
 * direkt an this_player() ausgegeben.
 * Wenn path nicht im Standardpfad liegt, wird indirect ignoriert und der
 * Cache bleibt aussen vor.
 */
public varargs string query_man(string path, string* key, int indirect)
{
  string ret, *filenames, *tmp;
  int i, num, dump;
  if (!path) path=START;
  if (path[0..sizeof(START)]!=(START+"/")[0..sizeof(path)-1])
  {
    old_search(path, &key);
    return 0;
  }
  ret="";
  filenames=({});
  num=sizeof(key);
  for (i=0; i<num; i++) {
    if (!member(cache, key[i]))
      fill_cache(path, key[i]);
    if (member(cache, key[i]))
    {
      tmp=map(cache[key[i]], #'+, "/"+key[i]);
      if (sizeof(filter(tmp, lambda( ({ 'x }),
        ({ #'==, ({ #'file_size, 'x }), -1 }) )))) {
        // gecachtes file ist verschwunden ?!!
        cache=m_delete(cache, key[i]);
        fill_cache(path, key[i]);
        if (member(cache, key[i]))
          tmp=map(cache[key[i]], #'+, key[i]);
        else tmp=0;
      }
      if (tmp) {
        filenames+=tmp;
        key[i]=0;
      }
    }
  }
  key-=({0});
  dump = (num=sizeof(filenames)) > 1;
  // (More kann nicht mehrere Files hintereinander ausgeben)
  for (i=0; i<num; i++)
  {
    if (file_size(filenames[i])==-2)
    {
      ret+="Topics for section "+efun::explode(filenames[i],"/")[<1]+":\n"+
        break_string(implode(get_dir(filenames[i]+"/*")-
        ({".",".."}), ", "), 78);
      if (!indirect && !dump) {
        this_player()->More(ret);
        ret="";
      }
    }
    else
    {
      if (!indirect && !dump) this_player()->More(filenames[i], 1);
      else
      {
        if (ret!="") ret+=sprintf("%'-'|78s\n", "-");
        ret+=read_file(filenames[i]);
      }
    }
  }
  if (dump && !indirect) {
    this_player()->More(ret);
    return 0;
  }
  if (indirect) return ret;
  return 0;
}

private string add(string s1, string s2)
{
  return s2+"/"+s1;
}

private void fill_cache(string path, string key)
{
  string *files;

  if (path[<1]=='/') path=path[0..<2];
  if (pointerp(files=get_dir(path+"/*"))) {
    files=filter(files, lambda( ({'x}),
      ({#'!=, ({ #'[, 'x, 0 }), '.' }) ));
    files=filter(files, lambda( ({'x}),
      ({#'!=, ({ #'[, 'x, 0 }), '_' }) ));
  }
  else
    return;
  if (member(files, key) != -1)
  {
    if (member(cache, key)) cache[key] += ({path});
    else cache+=([ key: ({path}) ]);
  }
  files=map(files, #'add, path);
  files=filter(files, lambda( ({'x}),
    ({#'==, ({ #'file_size, 'x }), -2 }) ));
  map(files, #'fill_cache, key);
}

private void old_print(string file)
{
  this_player()->More(file, 1);
}

private void old_search(string path, string* key) {
  string *files, *treffer;
  files=get_dir(path+"/*")-({".", ".."});
  treffer=files&key;
  key-=treffer;
  if (sizeof(treffer)) {
    treffer=map(treffer, #'add, path);
    map(treffer, #'old_print);
  }
  files=map(files, #'add, path);
  map(files, #'old_search, &key);
}

void create() {
  if (clonep(this_object())) raise_error("Ich will nicht geclont werden.");
  cache=([]);
}

int remove() {
  printf("Mancache cleared.\n");
  destruct(this_object());
  return 1;
}

mapping QueryCache()
{
  return cache;
}

