/* SilberLand MUDlib
 *
 * /secure/guestmaster.c -- guest master object
 *
 * $Date: 1997/03/15 23:02:02 $
 * $Revision: 1.1 $
 * $Log: guestmaster.c,v $
 * Revision 1.1  1997/03/15 23:02:02  Woody
 * prepared for revision control
 *
 */

static int max_guests = 10; /* Max no. of guests. -1 is 'unlimited' */
static object *guests;
static int    *ltime;

nomask void create() {
  if (sizeof(explode(object_name(this_object()),"#")) != 1) {
    destruct(this_object());
    return;
  }
  guests = ltime = ({});
  max_guests = 9;
}

nomask int new_guest () {
  int ix;

  if (!max_guests) return 0;
  if (explode(object_name(previous_object()),"#")[0] != "/secure/login")
    return 0;
  for (ix = 0; ix < sizeof(guests); ix++)
  {
    if (guests[ix] && !interactive(guests[ix]))
    {
       guests[ix] = 0;
    }
  }

  if ((ix = member(guests,0)) == -1) {
    ix = sizeof(guests);
    if (max_guests < 0 || ix < max_guests) 
    {
      guests += ({ 0 }), ltime += ({ 0 });
    }
    else {
      int mintime, minix;
      mintime = time();
      for (ix = 0; ix < sizeof(guests); ix++) {
        if (guests[ix] && ltime[ix] < mintime) mintime=ltime[ix], minix=ix;
      }
      ix = minix;
    }
  } 
  return ix+1;
}

nomask void set_guest (int ix, object pl) {
  if (explode(object_name(previous_object()),"#")[0] != "/secure/login") return;
  guests[ix-1] = pl;
  ltime[ix-1] = time();
}

nomask int query_max_guests() { return max_guests; }
nomask int set_max_guests(int v) {
  if (this_player() && query_wiz_level(this_player()) >= 40)
    max_guests = v;
  return max_guests;
}
