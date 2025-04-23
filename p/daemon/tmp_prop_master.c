//
// 03/25/2013 Serii - PropReset() hat nicht richtig funktioniert + kleinen bug in
//		      do_update() entfernt
//
// 06/22/2012 Serii - Hm - das teil scheint eigentlich ueberhaupt nicht richtig
//                    funktioniert zu haben... Nasowas! Aber nun gehts hoffentlich (:
//                    Sollte nun auch rebootsicher sein...
//
// 11.03 2004 Serii : man kann nu auch ne message angeben, die ausgegeben wird,
//                    wenn die prop zurueck gesetzt wird
//
// 29.07 2002 Serii@Tamedhon : PropReset() hinzugefuegt -
//			       zum vorzeitigen zuruecksetzen einer property
#define NEED_PROTOTYPES
#include <thing/properties.h>
#pragma strong_types

#define SAVEFILE "/p/daemon/save/tmp_prop_master"

#include "/d/unterwelt/def.h"

mapping oldprops;
mixed *reset_times;

void Save(){save_object(SAVEFILE);}
void Restore(){restore_object(SAVEFILE);}

void create() {
  if (clonep(this_object())) {
    destruct(this_object());
    return;
  }
  seteuid(getuid(this_object()));
  oldprops=([]);
  reset_times=({});
  Restore();
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

  res=((string)(objectp(ob)?object_name(ob):""));
  res+="##";
  res+=((string)(stringp(s)?s:""));
  return res;
}

static void call_next_update() {
  int dt;

  while (remove_call_out("do_update")!=-1);
  if (sizeof(reset_times)) {
    dt=reset_times[0][0]-time();
    if (dt<=0) dt=5;
    call_out("do_update",dt);
  }
}

static void do_update() {
  int i,tm,max;
  string index;
  mixed *val;
  object ob;

  while (remove_call_out("do_update")!=-1) ;
  call_out("do_update",30); // Zur Sicherheit...

  if (!(max=sizeof(reset_times))) return;
  tm=time();
  if (!mappingp(oldprops)) oldprops=([]);
  for (i=0;(i<max && reset_times[i][0]<=tm);i++) {
    index=reset_times[i][1];
    if (!(val=oldprops[index])) continue;
//    if (objectp(val[0]))
    if(ob=find_object(val[0]))
    {
      ob->SetProp(val[1],val[2]);
      do_msg(ob,val[3]);
      oldprops=m_delete(oldprops,index);
    }
    else if(sizeof(explode(val[0],"#"))>1)//falls es ein geclontes object war
    {
      oldprops=m_delete(oldprops,index);
    }
    // ansonsten in der liste lassen (z.b. ausgeloggter spieler etc)
  }
  reset_times=reset_times[i..];
  call_next_update();
  Save();
}

void SetTmpProp(object ob, string prop, mixed newval, mixed tm) { //aus kompatiblitaetsgruenden mach ich aus
  string index,msg;                                               //dem int tm mal n mixed... mag die fun nich
  mixed oldval;                                                   //als varargs machen ;)
  int max,pos,l,r,tme;

  if(pointerp(tm))
  {
    if(sizeof(tm)!=2)return;
    tme=(int)tm[0];
    msg=(string)tm[1];
    if(!msg)msg="";
  }
  else
  { 
    tme=(int)tm;
    msg="";
  }

tme=time()+tme;

  if (!objectp(ob) || !prop || tme<=0) return;
  oldval=ob->QueryProp(prop);
  index=make_index(ob,prop);
  if (!mappingp(oldprops)) oldprops=([]);
  if (oldprops[index]) {
    ob->SetProp(prop,newval);
    return;
  }
//  oldprops[index]=({ob,prop,oldval,msg});
    oldprops[index]=({object_name(ob),prop,oldval,msg});

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

  ob->SetProp(prop,newval);
  call_next_update();

  Save();

  // printf("%O\n", reset_times);
}

//----------Serii(@Tamedhon) July 29 2002---------

void PropReset(object ob,string prop)
{
string index;
int lv;
mixed *rt;
object robj;

  index=make_index(ob,prop);

  if(mappingp(oldprops)&&pointerp(oldprops[index]))
  {
    if(sizeof(oldprops[index][0]))
    {
      robj=find_object(oldprops[index][0]);
      if(objectp(robj))
      {
        robj->SetProp(oldprops[index][1],oldprops[index][2]);
        do_msg(oldprops[index][0],oldprops[index][3]);
      }
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
  Save();
}
//----End---Serii---------------------------------