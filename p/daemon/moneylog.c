// SilberLand MUDlib
//
// /p/daemon/moneylog.c -- logs money transfers
//
// $Date: 1997/03/16 02:31:10 $
/* $Revision: 1.1 $
 * $Log: moneylog.c,v $
 * Revision 1.1  1997/03/16 02:31:10  Woody
 * prepared for revision control
 *
 */

#pragma strong_types
#define LOGNAME "ADDMONEY"

static mapping mon;

void create() {
  mon=([]);
}

void reset() {
  string *ind;
  int i;

  ind=sort_array(m_indices(mon),#'>);
  if (i=sizeof(ind))
    log_file(LOGNAME,sprintf("#%s\n",ctime(time())[4..15]));
  for (--i;i>=0;i--)
    log_file(LOGNAME,sprintf("%s: %d\n",ind[i],mon[ind[i]]));
  mon=([]);
}

void AddMoney(object ob, int amount) {
  if (!objectp(ob) || query_once_interactive(ob) || amount<=0) return;
  mon[explode(object_name(ob),"#")[0]]+=amount;
}

mixed query_status() {
  return mon;
}

