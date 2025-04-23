// SilberLand MUDlib
//
// LIVING/ATTRIBUTES.C -- attributes for living objects
//
// $Date: 1997/06/25 11:52:24 $                      
/* $Revision: 3.1 $       
 * $Log: attributes.c,v $
 * Revision 3.1  1997/06/25 11:52:24  Woody
 * P_MAX_MV korrekt setzen
 *
 * Revision 3.0  1997/03/13 00:49:22  Woody
 * prepared for revision control
 *
 * Revision 2.14  1997/01/23  Woody
 * Objekte koennen mit P_X_ATTR_MOD die Spieler-Attribute aendern,
 * previous_object() -> previous_object(1) in _set_/_query_ func's,
 * QueueUpdateAttributes()
 *
 * Revision 2.13  1996/10/01  Woody
 * Zahlen durch Konstanten ersetzt, SetRealAttribute(), etc.
 *
 * Revision 2.12  1995/02/23  20:52:40  Rochus
 * Vereinfachung.
 *
 * Revision 2.11  1995/02/23  20:48:00  Rochus
 * Attribut-Modifier eingebaut. Diese koennen nur von
 * dem Objekt geaendert werden, das sie gesetzt hat.
 *
 * Revision 2.10  1994/12/09  17:05:53  Jof
 * made offsets static
 * /
 *
 * Revision 2.9  1994/10/04  09:45:53  Jof
 * *** empty log message ***
 *
 * Revision 2.9  1994/10/04  13:54:50  mud
 * Indentation fixed
 *
 * Revision 2.8  1994/09/15  14:57:57  Rochus
 * Zwei Zeilen waren vertauscht, daher wurden SP/HP geupdated
 * BEVOR die neuen Attribute gesetzt wurden...
 *
 * Revision 2.7  1994/09/13  16:33:06  Rochus
 * Automatisches Update NUR bei Spielern.
 *
 * Revision 2.6  1994/09/13  16:24:27  Rochus
 * Automatisches Update von MAX_SP und MAX_HP
 *
 * Revision 2.5  1994/09/13  10:47:43  Rochus
 * Rassenspezifische Offsets fuer Attribute eingebaut.
 *
 * Revision 2.4  1994/06/13  07:36:24  Jof
 * Removed SET_METHODs
 *
 * Revision 2.3  1994/03/21  16:48:13  Jof
 * Reindented files using emacs - once again
 *
 * Revision 2.2  1994/03/18  12:35:53  Jof
 * Fixed indentation
 *
 * Revision 2.1  1994/02/24  13:02:34  Jof
 * Private removed
 *
 * Revision 2.0  1994/01/13  16:52:12  mud
 * *** empty log message ***
 *
 * Revision 1.3  1994/01/08  13:11:20  mud
 * *** empty log message ***
 *
 * Revision 1.3  1994/01/08  13:11:20  mud
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/05  17:07:50  mud
 * changed attributes to mapping
 *             
 */                            

#include "/std/sys_debug.h"
#include <moving.h>

// attribute handling
//
//   filter: str, dex, con, int
//
// In den Attributen und Abilites werden Rassenspzifische und erworbene
// Faehigkeiten (mit autoload-Eigenschaft) abgepseichert.
//
// Funktionen:
//   SetAttribute( attr, val ) (Erzeuge und) setze das Attribut auf val.
//   SetAbility( abil, val )   dito.
//   QueryAttribute( attr )    Gebe den Wert des Attributes zurueck.
//   QueryAbility( abil )      dito.
//
//   Wenn ein Objekt eine Funktion _filterattr_<name> beitzt, wird beim Setzen
//   des Attributes <name>, der vorgesclagene Wert uebergeben und der von
//   dieser Funktion zurueckgegebene gesetzt. (fuer ueberpruefungszwecke)
//   Gleiches gilt fuer _filterabil_<name>.

#pragma strong_types

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <attributes.h>

#include <config.h>
#include <properties.h>

// Save variable
mapping attributes;
mapping attributes_modifier;
// Offsets koennen geaendert werden, NICHT speichern!
static mapping attributes_offsets;
static mapping used_attributes_offsets;
static int queued_upd;

static mapping _set_attributes(mapping arr);
static mapping _query_attributes();
static mapping _set_attributes_offsets(mapping arr);
static mapping _query_attributes_offsets();
mixed _set_attributes_modifier(mixed arr);
mapping _query_attributes_modifier();

static void add_offsets(mapping arr) {
  mixed *ind;
  int i;
  
  if (!mappingp(arr)) return;
  ind=m_indices(arr);
  for (i=sizeof(ind)-1;i>=0;i--)
    used_attributes_offsets[ind[i]]+=arr[ind[i]];
}

static void update_used_attributes() {
  mixed *ind;
  int i;
  object ob;
  mapping res;

  used_attributes_offsets=([]);
  if (mappingp(attributes_offsets))
    used_attributes_offsets+=attributes_offsets;
  if (mappingp(attributes_modifier)) {
    ind=m_indices(attributes_modifier);
    for (i=sizeof(ind)-1;i>=0;i--)
      add_offsets(attributes_modifier[ind[i]]); // Modifier addieren...
  }
  for (ob=first_inventory(this_object());ob;ob=next_inventory(ob))
    if (mappingp(res=ob->QueryProp(P_X_ATTR_MOD)))
      add_offsets(res);
}

static void update_max_sp_and_hp() {
  int con;

  update_used_attributes();
  if (!query_once_interactive(this_object())) return;
  // Bei Monstern werden die HP/SP ueblicherweise selbst gesetzt
  con=QueryAttribute(A_CON);
  SetProp(P_MAX_HP, con*8+42);
  SetProp(P_MAX_SP, (QueryAttribute(A_INT))*8+42);
  SetProp(P_MAX_MV, con*MVP_CON_FACTOR+MVP_BASE);
  // SetProp(P_MAX_MV, con*MVP_CON_FACTOR+QueryProp(P_LEVEL)*L_OFF+MVP_BASE);
}

void UpdateAttributes() {
  update_max_sp_and_hp();
}

void QueueUpdateAttributes() {
  update_max_sp_and_hp();
  queued_upd = 1;
}

static void create() {
  used_attributes_offsets=([]);
  Set(P_ATTRIBUTES_MODIFIER, attributes_modifier=([])); // nicht geschuetzt
  Set(P_ATTRIBUTES_OFFSETS, attributes_offsets=([]));
  Set(P_ATTRIBUTES_OFFSETS, PROTECTED, F_MODE);
  Set(P_ATTRIBUTES, attributes=([]));
  Set(P_ATTRIBUTES, PROTECTED, F_MODE);
}

static mapping _set_attributes(mapping arr) {
  Set(P_ATTRIBUTES, attributes=arr);
  update_max_sp_and_hp();
  return arr;
}
static mapping _query_attributes() { 
  return copy(Set(P_ATTRIBUTES, attributes));
}

static mapping _set_attributes_offsets(mapping arr) {
  Set(P_ATTRIBUTES_OFFSETS, attributes_offsets=arr);
  update_max_sp_and_hp();
  return attributes_offsets;
}

static mapping _query_attributes_offsets() {
  return copy(Set(P_ATTRIBUTES_OFFSETS, attributes_offsets));
}

mixed _set_attributes_modifier(mixed arr) {
  string fn;
  mixed pre;
  mapping map;

  if (pointerp(arr) && sizeof(arr)>=2) {
    pre=arr[0];
    map=arr[1];
  } else {
    pre=previous_object(1);
    map=arr;
  }
  if (objectp(pre))
    fn=explode(object_name(pre),"#")[0];
  else
    fn=pre;
  if (!stringp(fn))
    return 0;

  if (!mappingp(map))
    efun::m_delete(attributes_modifier,fn);
  else
    attributes_modifier[fn]=map;
  Set(P_ATTRIBUTES_OFFSETS, attributes_modifier);
  update_max_sp_and_hp();
  return attributes_modifier[fn];
}

mapping _query_attributes_modifier() {
  return copy(attributes_modifier);
}

/*
mapping _query_attributes_modifier() {
  string fn;

  if (!previous_object(1)) return 0;
  fn=explode(object_name(previous_object(1)),"#")[0];
  Set(P_ATTRIBUTES_OFFSETS, attributes_modifier);
  return attributes_modifier[fn];  
}
*/

int SetAttr(string attr, int val) {
  closure filter;

  if(filter = symbol_function("_filterattr_"+attr, this_object())) 
    val = funcall(filter, val );
  attributes[attr] = val;
  update_max_sp_and_hp();
  return QueryAttribute(attr);
}

/* Aus historischen Gruenden */
int SetAttribute(string attr, int val) {
  return SetAttr(attr, val-used_attributes_offsets[attr]);
}

int QueryAttribute(string attr) {
  if (queued_upd) {
    queued_upd = 0;
    update_max_sp_and_hp();
  }
  return attributes[attr]+used_attributes_offsets[attr];
}

int QueryRealAttribute(string attr) {
  return attributes[attr];
}

int SetRealAttribute(string attr, int val) {
  return SetAttr(attr, val);
}

int QueryAttributeOffset(string attr) {
  if (queued_upd) {
    queued_upd = 0;
    update_max_sp_and_hp();
  }
  return used_attributes_offsets[attr];
}

/*
 *------------------------------------------------------------
 * attributes compatibility functions
 */

string query_stats() {
  string retstr;
  mixed i;
  retstr = "";
  for (i = m_indices(attributes); sizeof(i); i = i[1..])
    retstr += sprintf("%s: %2d  ", i[0], attributes[i[0]]);
  return retstr;
}

static int _filterattr_str(int val)
{
  if( val<0 ) return 0;
  if( val>A_MAX ) return A_MAX;
  return val;
}

static int _filterattr_dex(int val)
{
  if( val<0 ) return 0;
  if( val>A_MAX ) return A_MAX;
  return val;
}

static int _filterattr_int(int val)
{
  if( val<0 ) return 0;
  if( val>A_MAX ) return A_MAX;
  return val;
}

static int _filterattr_con(int val)
{
  if( val<0 ) return 0;
  if( val>A_MAX ) return A_MAX;
  return val;
}
