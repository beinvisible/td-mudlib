/*
 * obj/doormaster.c
 * ---
 * Der Door-Master, verwaltet saemtliche Tueren im MUD
 *
 * 2006/06/01  1.10  Serii
 * beruecksichtigung von D_OPENMSG, D_CLOSEMSG, D_RESET_OPENMSG, D_RESET_CLOSEMSG, D_LOCKMSG und D_UNLOCKMSG eingebaut
 *
 * 1996/11/21  1.00  Woody
 * started revision history; D_CLOSETESTFUNC eingefuehrt
 *
 */
// Aenderungen:
// 30.11.2008  TamTam  Klammer an die richtige Stelle verschoben *kopfschuettel*

#include <properties.h>
#include <defines.h>
#include <moving.h>
#include <language.h>
#include <room/doors.h>

inherit "/std/thing";

#pragma strong_types

private mapping door_status;
// Format: "filename1:filename2":status

mapping QueryAllDoors() {return door_status;}

int QueryDoorStatus(string dest)
{
  string source,dkey;
  object env;

  if (!dest) return 0;
  if (!objectp(env=previous_object())) return 0;
  source=object_name(env);
  dkey=((string)((source<dest)?(source+":"+dest):(dest+":"+source)));
  return door_status[dkey];
}

void SetDoorStatus(string dest, int x)
{
  string source,dkey;
  object env;

  if (!dest) return;
  if (!objectp(env=previous_object())) return;
  source=object_name(env);
  dkey=((string)((source<dest)?(source+":"+dest):(dest+":"+source)));
  door_status[dkey]=x;
}

void create()
{
  door_status=([]);
  if (IS_CLONE(this_object())) remove();
}

int remove() {return 0;}

varargs int NewDoor(mixed cmds, string dest, mixed ids, mixed *props)
{
  object env;
  string source,dkey;
  mixed *info2;
  mapping info;
  int i;

  if (!objectp(env=previous_object())) return 0;
  if (!cmds || !dest) return 0;
  if (!ids) ids=({"tür","tuer"});
  if (stringp(cmds)) cmds=({cmds});
  if (stringp(ids)) ids=({ids});
  if (!pointerp(cmds) || !pointerp(ids)) return 0;
  if (dest[0..0]!="/") dest="/"+dest;

  info=([D_DEST:dest,
         D_CMDS:cmds,
         D_IDS:ids,
         D_FLAGS:(DOOR_CLOSED|DOOR_RESET_CL),
         D_LONG:"Eine Tür.\n",
         D_SHORT:"Eine %se Tür. ",
         D_NAME:"Tür",
         D_GENDER:FEMALE]);
  source=object_name(env);
  dkey=((string)((source<dest)?(source+":"+dest):(dest+":"+source)));

  if (pointerp(props))
    for (i=0;i<sizeof(props)-1;i+=2)
      if (intp(props[i]) && props[i]>=D_MINPROPS && props[i]<=D_MAXPROPS)
        info[props[i]]=props[i+1];

  if (!door_status[dkey])
  {
    // Nur initialisieren, wenn Tuer noch nicht existiert.
    if (info[D_FLAGS] & DOOR_OPEN)
    door_status[dkey]=1;
    if (info[D_FLAGS] & DOOR_CLOSED)
    door_status[dkey]=((info[D_FLAGS] & DOOR_NEEDKEY)?-2:-1);
  }
  info2=env->QueryProp(P_DOOR_INFOS);
  if (!pointerp(info2))
    info2=({info});
  else
    info2+=({info});
  env->SetProp(P_DOOR_INFOS,info2);
  env->AddDetail(ids,info[D_LONG]);

  return 1;
}

void init_doors ()
{
  object env;
  mixed *info;
  string source,dest,dkey;
  int i;

  if (!objectp(env=previous_object())) return;
  info=env->QueryProp(P_DOOR_INFOS);
  if (!pointerp(info)) return;

  source=object_name(env);
  for (i=sizeof(info)-1;i>=0;i--)
  {
    if (!mappingp(info[i])) continue;
    dest=info[i][D_DEST];
    dkey=((string)((source<dest)?(source+":"+dest):(dest+":"+source)));
    env->set_doors(info[i][D_CMDS],(door_status[dkey]>0));
  }
}

string look_doors ()
{
  object env;
  mixed *info;
  string source,dkey,dest,res;
  int i;

  if (!objectp(env=previous_object())) return "";
  info=env->QueryProp(P_DOOR_INFOS);
  if (!pointerp(info)) return "";
  init_doors(); // Aktueller Zustand soll auch bei den Exits angezeigt werden

  source=object_name(env);res="";
  for (i=sizeof(info)-1;i>=0;i--)
  {
    if (!mappingp(info[i])) continue;
    dest=info[i][D_DEST];
    dkey=((string)((source<dest)?(source+":"+dest):(dest+":"+source)));
    if (stringp(info[i][D_SHORT]))
    {
      res+=sprintf(info[i][D_SHORT],
           ((string)((door_status[dkey]>0)?"geöffnet":
           (((string)((door_status[dkey]!=-1)?"ab":""))+"geschlossen"))));
      if (sizeof(res) && res[<1]!='\n')
      res+=" ";
    }
  }
  if (res && res!="")
    return break_string(res,78,0,1);
  return res;
}

static void door_message(object room, string dname, int dgender, string msg)
{
  object ob;

  // printf("%O,%O,%O,%O\n",room,dname,dgender,msg);
  if (!room || !dname || !msg) return;
  ob=this_object();
  ob->SetProp(P_NAME,dname);
  ob->SetProp(P_GENDER,dgender);
  ob->SetProp(P_ARTICLE,1);
  tell_room(room,capitalize(ob->name(WER))+msg+"\n");
}

varargs static void door_message_other(string source, string dest, string msg, int res)
{
  mixed info;
  object env;
  int i;

  // printf("%O %O %O\n",source,dest,msg);
  if (!source || !dest || !msg) return;
  if (!objectp(env=find_object(dest))) return;
  if (!pointerp(info=env->QueryProp(P_DOOR_INFOS))) return;
  for (i=sizeof(info)-1;i>=0;i--)
  {
    if (!mappingp(info[i])) continue;
    if (info[i][D_DEST]!=source) continue; // Andere Tuer zu diesem Raum
    door_message(env, info[i][D_NAME], info[i][D_GENDER],(sizeof(info[i][res])&&res?info[i][res]:msg));
  }
}

void reset_doors ()
{
  object env;
  mixed *info;
  string source,dest,dkey;
  int i,j;

  if (!objectp(env=previous_object())) return;
  info=env->QueryProp(P_DOOR_INFOS);
  if (!pointerp(info)) return;

  source=object_name(env);
  for (i=sizeof(info)-1;i>=0;i--)
  {
    if (!mappingp(info[i])) continue;
    dest=info[i][D_DEST];
    dkey=((string)((source<dest)?(source+":"+dest):(dest+":"+source)));
    /*  // warum ist das ausgeklammert?
    if (door_status[dkey]>-2 && door_status[dkey]<2)
    {
      if (door_status[dkey]>0)
        door_status[dkey]=2;
      else
        door_status[dkey]=-2;
      continue; // nur jeder 2. Reset wird ausgefuehrt.
    }
    */
    if (info[i][D_FLAGS] & DOOR_RESET_CL)
    {
      // Tuer muss bei Reset geschlossen werden
      if (door_status[dkey]>0)
      {
        door_message(env,info[i][D_NAME],info[i][D_GENDER],(sizeof(info[i][D_RESET_CLOSEMSG])?info[i][D_RESET_CLOSEMSG]:" schließt sich."));
        door_message_other(source,dest," schließt sich.",D_RESET_CLOSEMSG);
      }
      if (door_status[dkey]!=-2)
        door_status[dkey]=((info[i][D_FLAGS] & DOOR_NEEDKEY)?-2:-1);
    }
    if (info[i][D_FLAGS] & DOOR_RESET_OP)
    {
      // Tuer muss bei Reset geoeffnet werden
      if (door_status[dkey]<=0)
      {
        door_message(env,info[i][D_NAME],info[i][D_GENDER],(sizeof(info[i][D_RESET_OPENMSG])?info[i][D_RESET_OPENMSG]:" öffnet sich."));
        door_message_other(source,dest," öffnet sich.",D_RESET_OPENMSG);
      }
      door_status[dkey]=1;
    }
  }
  init_doors();
}

static varargs int exec_func2(string dest, mixed func)
{
  if (!stringp(dest) || !stringp(func)) return 1;
  call_other(dest,func);
  return 1;
}

varargs int go_door (string str)
{
  object env,pl,ob;
  mixed *info;
  string source,dest,dkey;
  int i;

  if (!str) return 0;
  if (!objectp(env=previous_object())) return 0;
  if (!objectp(pl=this_player())) return 0;
  info=env->QueryProp(P_DOOR_INFOS);
  if (!pointerp(info)) return 0;

  source=object_name(env);
  ob=this_object();
  ob->SetProp(P_ARTICLE,1);
  for (i=sizeof(info)-1;i>=0;i--)
  {
    if (!mappingp(info[i])) continue;
    if (member(info[i][D_CMDS],str)<0) continue;
    dest=info[i][D_DEST];
    dkey=((string)((source<dest)?(source+":"+dest):(dest+":"+source)));
    ob->SetProp(P_NAME,info[i][D_NAME]);
    ob->SetProp(P_GENDER,info[i][D_GENDER]);
    notify_fail(capitalize(ob->name(WER,1))+" ist geschlossen.\n");
    if (door_status[dkey]<=0) continue; // Tuer geschlossen
    if (stringp(info[i][D_TESTFUNC]))
      if (call_other(env,info[i][D_TESTFUNC]))
        return 1; // Durchgang von der Tuer nicht erlaubt.
    if (stringp(info[i][D_FUNC]))
      call_other(env,info[i][D_FUNC]);
    if (stringp(info[i][D_MSGS]))
    {
      if (pl->move(dest,M_GO,info[i][D_MSGS])>0)
        return exec_func2(dest,info[i][D_FUNC2]);
      else
        return 1;
    }
    if (pointerp(info[i][D_MSGS]) && sizeof(info[i][D_MSGS])>=3)
    {
      if (pl->move(dest,M_GO,info[i][D_MSGS][0],
           info[i][D_MSGS][1],info[i][D_MSGS][2]))
        return exec_func2(dest,info[i][D_FUNC2]);
      else
        return 1;
    }
    if (pl->move(dest,M_GO,"nach "+capitalize(str)))
      return exec_func2(dest,info[i][D_FUNC2]);
    return 1;
  }
  return 0;
}

int oeffnen (string str)
{
  object env,schl,ob;
  mixed *info,s2;
  string source,dkey,dest,s1;
  int i,afg;

  notify_fail("WAS willst Du öffnen?\n");
  if (!str || !(ob=this_player())) return 0;
  /*
  notify_fail("Du hast keine Hand mehr frei.\n");
  if (ob->QueryProp(P_MAX_HANDS)-ob->QueryProp(P_USED_HANDS) < 1)
    return 0;
  */
  notify_fail("Das kannst Du nicht öffnen.\n");
  str=lower_case(str);
  if (sscanf(str,"%s mit %s",s1,s2)!=2)
    {s1=str;s2=0;}
  if (s2)
  {
    if (!(schl=present(lower_case(s2),this_player())))
    {
      notify_fail("So einen Schlüssel hast Du nicht.\n");
      return 0;
    }
    else
    {
      s2=schl->QueryDoorKey();
      if (stringp(s2)) s2=({s2});
    }
  }

  if (!objectp(env=previous_object())) return 0;
  info=env->QueryProp(P_DOOR_INFOS);
  if (!pointerp(info)) return 0;

  source=object_name(env);
  ob=this_object();
  ob->SetProp(P_ARTICLE,1);
  afg=0;
  for (i=sizeof(info)-1;i>=0;i--)
  {
    if (!mappingp(info[i])) continue;
    if (member(info[i][D_IDS],s1)<0) continue; // Falsche Tuer
    ob->SetProp(P_NAME,info[i][D_NAME]);
    ob->SetProp(P_GENDER,info[i][D_GENDER]);
    dest=info[i][D_DEST];
    dkey=((string)((source<dest)?(source+":"+dest):(dest+":"+source)));
    notify_fail(capitalize(ob->name(WER,1))+" ist doch schon geöffnet!\n");
    if (door_status[dkey]>0)
      continue; // Eine andere Tuer koennte gemeint sein.
    if ((info[i][D_FLAGS] & DOOR_NEEDKEY) && door_status[dkey]!=D_STATUS_CLOSED)
    { // abgeschlossen
      notify_fail("Du brauchst einen Schlüssel, um "+ob->name(WEN,1)+" zu öffnen.\n");
      if (!schl) continue; // Eine andere Tuer koennte gemeint sein.
      notify_fail(capitalize(schl->name(WER))+" passt nicht!\n");
      if (!pointerp(s2)) continue;
      if (member(dkey,s2)<0) continue; // Koennte an einer anderen passen.
      // Schluessel *wuerde* passen, jetzt der Test mit D_KEYTESTFUNC
      if (stringp(info[i][D_KEYTESTFUNC]))
        if (call_other(env,info[i][D_KEYTESTFUNC]))
          return 1; // Aufschliessen der Tuer nicht erlaubt.
      afg=1;
    }
    // Alle Bedingungen erfuellt, als letzter Test D_OPENTESTFUNC
    if (stringp(info[i][D_OPENTESTFUNC]))
      if (call_other(env,info[i][D_OPENTESTFUNC]))
        return 1; // Oeffnen der Tuer nicht erlaubt.
    // Tuer oeffnen:
    door_status[dkey]=D_STATUS_OPEN;
    init_doors();
    if(!afg)
    {
      write((pointerp(info[i][D_OPENMSG])?info[i][D_OPENMSG][0]:"Du öffnest "+ob->name(WEN)+".\n"));
      say((pointerp(info[i][D_OPENMSG])?sprintf(info[i][D_OPENMSG][1],this_player()->Name(RAW)):capitalize(ob->name(WER))+" wird von "+this_player()->name(WEM)+
          " geöffnet.\n"));
      door_message_other(source,dest,(pointerp(info[i][D_OPENMSG])?info[i][D_OPENMSG][2]:" wird von der anderen Seite geöffnet."));
    }
    else
    {
      write((pointerp(info[i][D_UNLOCKMSG])?info[i][D_UNLOCKMSG][0]:"Du öffnest "+ob->name(WEN)+".\n"));
      say((pointerp(info[i][D_UNLOCKMSG])?sprintf(info[i][D_UNLOCKMSG][1],this_player()->Name(RAW)):capitalize(ob->name(WER))+" wird von "+this_player()->name(WEM)+
          " geöffnet.\n"));
      door_message_other(source,dest,(pointerp(info[i][D_UNLOCKMSG])?info[i][D_UNLOCKMSG][2]:" wird von der anderen Seite geöffnet."));
    }
    return 1;
  }
  return 0;
}

int schliessen (string str)
{
  object env,schl,ob;
  mixed *info,s2;
  string source,dkey,dest,s1;
  int i,abg;

  notify_fail("WAS willst Du schließen?\n");
  if (!str || !(ob=this_player())) return 0;
  /*
  notify_fail("Du hast keine Hand mehr frei.\n");
  if (ob->QueryProp(P_MAX_HANDS)-ob->QueryProp(P_USED_HANDS) < 1)
    return 0;
  */
  notify_fail("Das kannst Du nicht schließen.\n");
  str=lower_case(str);
  if (sscanf(str,"%s mit %s",s1,s2)!=2)
    {s1=str;s2=0;}
  if (s2)
  {
    if (!(schl=present(lower_case(s2),this_player())))
    {
      notify_fail("So einen Schlüssel hast Du nicht.\n");
      return 0;
    }
    else
    {
      s2=schl->QueryDoorKey();
      if (stringp(s2)) s2=({s2});
    }
  }

  if (!objectp(env=previous_object())) return 0;
  info=env->QueryProp(P_DOOR_INFOS);
  if (!pointerp(info)) return 0;

  source=object_name(env);
  ob=this_object();
  ob->SetProp(P_ARTICLE,1);
  abg=0;
  for (i=sizeof(info)-1;i>=0;i--)
  {
    if (!mappingp(info[i])) continue;
    if (member(info[i][D_IDS],s1)<0) continue; // Falsche Tuer
    ob->SetProp(P_NAME,info[i][D_NAME]);
    ob->SetProp(P_GENDER,info[i][D_GENDER]);
    dest=info[i][D_DEST];
    dkey=((string)((source<dest)?(source+":"+dest):(dest+":"+source)));
    if (schl)
    {
      notify_fail(capitalize(ob->name(WER,1))+" ist doch schon abgeschlossen!\n");
      if (door_status[dkey]<=0 && door_status[dkey]!=D_STATUS_CLOSED)
        continue; // Eine andere Tuer koennte gemeint sein.
      if (info[i][D_FLAGS] & DOOR_CLOSEKEY)
      { // Schluessel noetig?
        notify_fail(capitalize(schl->name(WER))+" passt nicht!\n");
        if (!pointerp(s2)) continue;
        if (member(dkey,s2)<0) continue; // Koennte an einer anderen passen.
        // Alle Bedingungen erfuellt, als letzter Test D_CLOSETESTFUNC
        if (stringp(info[i][D_CLOSETESTFUNC]))
          if (call_other(env,info[i][D_CLOSETESTFUNC]))
            return 1; // Schliessen der Tuer nicht erlaubt.
        door_status[dkey]=D_STATUS_LOCKED;
        abg=1;  // Tuer wird richtig abgeschlossen
      }
      else
      {
        // Alle Bedingungen erfuellt, als letzter Test D_CLOSETESTFUNC
        if (stringp(info[i][D_CLOSETESTFUNC]))
          if (call_other(env,info[i][D_CLOSETESTFUNC]))
            return 1; // Schliessen der Tuer nicht erlaubt.
        door_status[dkey]=((info[i][D_FLAGS] & DOOR_NEEDKEY) ?
          D_STATUS_LOCKED :
          D_STATUS_CLOSED);
        // ohne Schluessel abschliessbar
      }
    }
    else
    {
      notify_fail(capitalize(ob->name(WER,1))+" ist doch schon geschlossen!\n");
      if (door_status[dkey]<=0)
        continue; // Eine andere Tuer koennte gemeint sein.
      // Alle Bedingungen erfuellt, als letzter Test D_CLOSETESTFUNC
      if (stringp(info[i][D_CLOSETESTFUNC]))
        if (call_other(env,info[i][D_CLOSETESTFUNC]))
          return 1; // Schliessen der Tuer nicht erlaubt.
      if ((info[i][D_FLAGS] & DOOR_NEEDKEY) &&
          !(info[i][D_FLAGS] & DOOR_CLOSEKEY))
        door_status[dkey]=D_STATUS_LOCKED; // ohne Schluessel abschliessbar
      else
        door_status[dkey]=D_STATUS_CLOSED;
    }
    init_doors();
    if((abg&&!pointerp(info[i][D_LOCKMSG]))||(!abg&&!pointerp(info[i][D_CLOSEMSG])))
    {
      write("Du schließt "+ob->name(WEN)+((string)(abg?" ab":""))+".\n");
      say(capitalize(ob->name(WER))+" wird von "+this_player()->name(WEM)+
          ((string)(abg?" ab":" "))+"geschlossen.\n");
      door_message_other(source,dest," wird von der anderen Seite "+
                        ((string)(abg?"ab":""))+"geschlossen.");
    }
    else
    {
      write(info[i][(abg?D_LOCKMSG:D_CLOSEMSG)][0]);
      say(sprintf(info[i][(abg?D_LOCKMSG:D_CLOSEMSG)][1],this_player()->Name(RAW)));
      door_message_other(source,dest,info[i][(abg?D_LOCKMSG:D_CLOSEMSG)][2]);
    }
    return 1;
  }
  return 0;
}
