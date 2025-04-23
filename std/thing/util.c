// SilberLand MUDlib
//
// std/thing/util.c -- Various debug functions
//
// $Date: 2001/02/10 12:11:05 $
/* $Revision: 1.1 $
 * $Log: util.c,v $
 * Revision 1.1  2001/02/10 12:11:05  Samtpfote
 * Initial revision
 *
 * Revision 1.1  1997/03/13 02:37:30  Woody
 * prepared for revision control
 *
 */

static void PrettyDump(mixed x);
static void DumpArray(mixed *x);
static void DumpMapping(mixed x);
static void DumpKeyValPair(mixed x, int i);
mixed QueryProp(string name);

void ShowPropList(mixed *props) {
  int i;
  for (i = 0; i < sizeof(props); i++) {
    write("*"+props[i]+": ");
    PrettyDump(QueryProp(props[i]));
    write("\n");
  }
}

static void PrettyDump(mixed x) 
{
  if (pointerp(x)) 
  {
    DumpArray(x);
  }
  else if (mappingp(x))
  {
    DumpMapping(x);
  }
  else if (objectp(x)) 
  {
    write ("OBJ("+object_name(x)+")");
  }
  else if (stringp(x))
  {
    write("\""+x+"\"");
  }
  else
    write (x);
}

static void DumpArray(mixed *x) {
  int i;
  write ("({ ");
  if (sizeof(x)) {
    for (i = 0; i < sizeof(x)-1; i++) {
      PrettyDump(x[i]);
      write(", ");
    }
    PrettyDump(x[i]);
    write (" ");
  }
  write ("})");
}

static void DumpMapping(mixed x)
{
  int i, c;
  write("([ ");
  c = sizeof(x);
  if (!c)
  {
    write(" ])");
    return;
  }
  DumpKeyValPair(x, 0);
  for (i=1; i<c; i++)
  {
    write(", ");
    DumpKeyValPair(x, i);
  }
  write(" ])");
}

static void DumpKeyValPair(mixed x, int i) // Lacht nicht ueber den Namen!!! -Boing
{
  int j, vc;
  vc = get_type_info(x)[1];
  PrettyDump(m_indices(x)[i]);
  write(" : ");
  PrettyDump(x[m_indices(x)[i]]);
  for (j=1; j<vc; j++)
  {
    write("; ");
    PrettyDump(x[m_indices(x)[i], j]);
  }
}
