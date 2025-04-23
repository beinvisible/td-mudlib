// 11.03 2004 Serii : man kann nu auch ne message angeben, die ausgegeben wird,
//                    wenn die prop zurueck gesetzt wird
//
// 29.07 2002 Serii@Tamedhon : PropReset() hinzugefuegt -
//			       zum vorzeitigen zuruecksetzen einer property
#define NEED_PROTOTYPES
#include <thing/properties.h>
#pragma strong_types

#define SAVEFILE "/obj/save/tmp_prop_master"

static mapping oldprops;
static mixed *reset_times;

void create() {
  if (clonep(this_object())) {
    destruct(this_object());
    return;
  }
  oldprops=([]);
  reset_times=({});
}

void print_debug_info() {
  printf("oldprops: %O\nreset_times: %O\n",oldprops,reset_times);
}

static void do_msg(object pl,string str)
{
  if(sizeof(str))tell_object(pl,str);
}

static string make_index(object ob, string s) {
  string res;

  res=((objectp(ob)?object_name(ob):""));
  res+="##";
  res+=((stringp(s)?s:""));
  return res;
}

static void call_next_update() {
  int dt;

  while (remove_call_out("do_update")!=-1);
  if (sizeof(reset_times)) {
    dt=reset_times[0][0];
    if (dt<=0) dt=5;
    call_out("do_update",dt);
  }
}

static void do_update() {
  int i,tm,max;
  string index;
  mixed *val;

  while (remove_call_out("do_update")!=-1) ;
  call_out("do_update",30); // Zur Sicherheit...

  if (!(max=sizeof(reset_times))) return;
  tm=time();
  if (!mappingp(oldprops)) oldprops=([]);
  for (i=0;(i<max && reset_times[i][0]<=tm);i++) {
    index=reset_times[i][1];
    if (!(val=oldprops[index])) continue;
    if (objectp(val[0]))
    {
      val[0]->SetProp(val[1],val[2]);
      do_msg(val[0],val[3]);
    }
    oldprops=m_delete(oldprops,index);
  }
  reset_times=reset_times[i..];
  call_next_update();
}

void SetTmpProp(object ob, string prop, mixed newval, mixed tm) { //aus kompatiblitaetsgruenden mach ich aus
  string index,msg;                                               //dem int tm mal n mixed... mag die fun nich
  mixed oldval;                                                   //als varargs machen ;)
  int max,pos,l,r,tme;

  if(pointerp(tm))
  {
    if(sizeof(tm)!=2)return;
    tme=({int})tm[0];
    msg=({string})tm[1];
    if(!msg)msg="";
  }
  else
  { 
    tme=({int})tm;
    msg="";
  }

  if (!objectp(ob) || !prop || tme<=0) return;
  oldval=ob->QueryProp(prop);
  index=make_index(ob,prop);
  if (!mappingp(oldprops)) oldprops=([]);
  if (oldprops[index]) {
    ob->SetProp(prop,newval);
    return;
  }
  oldprops[index]=({ob,prop,oldval,msg});

  if (!(max=sizeof(reset_times)))
    pos=0;
  else {
    l=0;r=max-1;
    while (l<r) {
      pos=(l+r)/2;
      if (tme<reset_times[pos][0])
        pos=r=pos-1;
      else
        pos=l=pos+1;
    }
    while (pos>=0 && pos<max && tme<reset_times[pos][0]) pos--;
  }
  reset_times=reset_times[0..pos]+({({tme,index})})+reset_times[pos+1..];

  call_next_update();
  ob->SetProp(prop,newval);
  // printf("%O\n", reset_times);
}

//----------Serii(@Tamedhon) July 29 2002---------
void PropReset(object ob,string prop)
{
string index;
int lv;
mixed *rt;
  index=make_index(ob,prop);
  if((oldprops)&&(oldprops[index]))
  {
    if(objectp(oldprops[index][0]))
    {
      oldprops[index][0]->SetProp(oldprops[index][1],oldprops[index][2]);
      do_msg(oldprops[index][0],oldprops[index][3]);
    }
    oldprops=m_delete(oldprops,index);
    rt=({});
    for(lv=0;lv<sizeof(reset_times);lv++)
    {
      if(!(reset_times[lv][1]==index))
      {
        rt+=({reset_times[lv]});
      }
    }
    reset_times=rt;
  }
  call_next_update();
}
//----End---Serii---------------------------------
