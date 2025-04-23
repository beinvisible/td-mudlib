/* SilberLand MUDlib
 *
 * /secure/news.c -- news daemon
 *
 * $Date: 1998/01/19 17:06:50 $
 * $Revision: 1.3 $
 * $Log: news.c,v $
 * Revision 1.3  1998/01/19 17:06:50  Woody
 * Bug in RemoveNote() gefixt
 *
 * Revision 1.2  1997/12/17 22:58:07  Angrox
 * Funktion RemoveAllowed() fix: writers statt readers
 *
 * Revision 1.1  1997/03/15 23:24:48  Woody
 * prepared for revision control
 *
 * 18.Dez  1996 - Loco@Morgengrauen
 * lasttime
 *
 * 8. Feb  1995 - Jof@MorgenGrauen
 *
 * 1. Februar 1995 - Jof@MorgenGrauen
 * Rewrite (Mappings benutzen, Accessinfos im Speicher halten)
 *
 * 6. Juli 1992 - Jof@MorgenGrauen
 * 5. Juli 1992 - Jof@MorgenGrauen
 * Clear-Groups-Mechanismus eingebaut. Die 2 Konstanten TIME_TO_CLEAR und
 * MIN_TOUCHED muessen def. sein. Alle TIME_TO_CLEAR Sekunden werden die
 * Newsgroups, die seit dem letzten Clear seltener als MIN_TOUCHED Sekunden
 * beruehrt wurden, aus dem Cache genommen
 */

// Datenstrukturen:

// Newsgroups:  grouplist ist ein Mapping mit einem Eintrag pro Newsgroup.
//              Die Keys sind die Gruppennamen, Daten:
//              0 Zeit des letzen Artikels
//              1 Besitzer der Gruppe
//              2 Savefille-Name
//              3 Expire-Zeit
//              4 Array mit Loesch-Berechtigten
//              5 Array mit Scheib-Berechtigten
//              6 Array mit Leseberechtigten
//              7 Mindest-Level, um Artikel zu loeschen
//              8 Mindest-Level, um Artikel zu posten
//              9 Mindest-Level, um Artikel zu lesen
//             10 Max. Anzahl Nachrichten in der Gruppe

// Die Nachrichten selber stehen in einem Array.

// Eine nachricht ist ein Array mit den folgenden Elementen:
// 0         (*) string writer;
// 1         (*) string id;    Mudname+":"+time()+":"+group zur Zeit
// 2         (*) string time;
// 3             string title;
// 4             string message;

// Die mit (*) gekennzeichneten Eintraege setzt der Daemon selber

// Funktionen:
//    Returnvalues:  0 = Parameter error
//                   1 = Ok.
//                  -1 = no permission
//                  -2 = no such group/group already existing
//
//    Diese Returnvalues stehen fuer alle Funktionen

//  AddGroup(groupname, owner, savefile);
//   Funktion klar, kann nur von Erzmagiern aufgerufen werden
//    -3 = no owner, -4 = savefile already in use
//
//  RemoveGroup(groupname);
//   Ebenfalls nur fuer Erzmagier
//
//  SetGroup(groupname, dlevel, wlevel, rlevel, maxmessages, expire);
//   Erzmagier und der Groupowner koennen die nutzen. Legt Level, ab dem je-
//   mand aus der Gruppe loeschen kann (dlevel), in die Gruppe schreiben
//   kann (wlevel), die Gruppe lesen kann (rlevel) und die max. Anzahl Nach-
//   richten in der Gruppe fest.
//
//  AddAllowed(groupname, deleters, writers, readers);
//   Erzmagier/Owner koennen Arrays mit Namen von fuer die Gruppe loesch/
//   schreib/lese-Berechtigten fuer die Gruppe angeben.
//
//  RemoveAllowed(groupname, deleters, writers, readers);  analog
//
//  WriteNote(message); klar
//   -3 = Max number of msgs exceeded
//
//  RemoveNote(boardname, notenummer); notenummer>=0;
//   -3 = No such note
//
//  GetNotes(boardname); gibt einen Array mit den Notes zurueck.
//
//  AskAllowedWrite(); return wie bei WriteNote, stellt fest, ob ein Player
//  eine Note aufhaengen darf oder nicht
//
//  GetNewsTime([boardname]); gibt zurueck, wann am entsprechenden Brett zum
//  letzten Mal eine Note befestigt wurde. Falls kein boardname angegeben
//  wird, liefert GetNewsTime() den Zeitpunkt, zu dem ueberhaupt eine neue
//  Note aufgehaengt wurde.

#include "/secure/wizlevels.h"
#include <config.h>
#include <news.h>

#define WTIME 0

#define DEBUG(x) tell_object(find_player("debugger"),x)

#define TI_PO() (this_interactive()||previous_object())

mixed saveload; // Diese Variable ist als einzige nicht static und dient
                 // Uebertragen von Daten in/aus savefiles

static mapping grouplist; // Groups und ihre save-Files, zudem LastTime
static int lasttime; // Um zu vermeiden, dass 2 Notes identische Uhrzeit==id
                     // haben, wird die jeweils letzte Zeit gespeichert.

static void save_group(mixed grp,mixed group);
static void save_group_list();
static mixed load_group(mixed name);

void create()
{
  seteuid(getuid(this_object()));
  if (!restore_object(NEWSPATH+"GroupList"))
    grouplist=m_allocate(0,G_MESSAGES);
  else
    grouplist=saveload;
  call_out("expire_all",5);
}

int AddGroup(mixed name, mixed owner)
{
  mixed *group;
  string savefile, *savefilea;
  int i;

  if (!name || !owner) return 0;
  
  if (!IS_ARCH(this_interactive())) return -1; // Darf nicht

  if (member(grouplist, name)) return -2; // Gibt es schon

  if (file_size("/"+SAVEPATH+owner[0..0]+"/"+owner+".o")<0) return -3;

  savefilea = explode(name,".");
  savefile = implode(savefilea,"/");
  if (file_size(NEWSPATH+savefile+".o")>=0) return -4;

  // Notwendige Directories anlegen
  for (i = 0; i < sizeof(savefilea)-1; i++) {
    mkdir(NEWSPATH+implode(savefilea[0..i],"/"));
  }

  group=({});
  grouplist+=([name:0;owner;savefile;-1;({});({});({});20;0;0;80]);
  save_group_list();
  save_group(name,group);
  return 1;
}

int RemoveGroup(mixed name)
{
  int num;

  if (!name) return 0;

  if (!IS_ARCH(this_interactive())) return -1; // Darf nicht

  if (!mappingp(grouplist) || !member(grouplist,name))
    return -2; // -2 no such group

  catch(rm(NEWSPATH+grouplist[name,G_SAVEFILE]+".o"));
  efun::m_delete(grouplist,name);

  save_group_list();

  return 1;
}

int SetGroup(mixed name, mixed dlevel, mixed wlevel, mixed rlevel, mixed maxmessages, mixed expire)
{
  mixed *group;
  
  if (!member(grouplist,name)) return -2;
  if (grouplist[name,G_OWNER]!=geteuid(this_interactive()) &&
      !IS_ARCH(this_interactive())) return -1;
  
  grouplist[name,G_DLEVEL]=dlevel;
  grouplist[name,G_WLEVEL]=wlevel;
  grouplist[name,G_RLEVEL]=rlevel;
  grouplist[name,G_MAX_MSG]=maxmessages;
  grouplist[name,G_EXPIRE]=expire;
  
  save_group_list();
  return 1;
}

int AddAllowed(mixed name, mixed deleters, mixed writers, mixed readers)
{
  mixed *group;

  if (!member(grouplist,name)) return -2;

  if (grouplist[name,G_OWNER]!=geteuid(this_interactive()) &&
      !IS_ARCH(this_interactive())) return -1;

  if (stringp(deleters)) deleters=({deleters});
  if (stringp(writers)) writers=({writers});
  if (stringp(readers)) readers=({readers});

  if (!deleters) deleters=({});
  if (!writers) writers=({});
  if (!readers) readers=({});

  grouplist[name,G_DELETERS]+=deleters;
  grouplist[name,G_WRITERS]+=writers;
  grouplist[name,G_READERS]+=readers;
	
  save_group_list();
  return 1;
}

int RemoveAllowed(mixed name, mixed deleters, mixed writers, mixed readers)
{
  mixed *group;

  if (!member(grouplist,name)) return -2;

  if (grouplist[name,G_OWNER]!=geteuid(this_interactive()) &&
      !IS_ARCH(this_interactive())) return -1;

  if (stringp(deleters)) deleters=({deleters});
  if (stringp(writers)) writers=({writers});
  if (stringp(readers)) readers=({readers});

  if (!deleters) deleters=({});
  if (!writers) writers=({});
  if (!readers) readers=({});

  grouplist[name,G_DELETERS]-=deleters;
  grouplist[name,G_WRITERS]-=writers;
  grouplist[name,G_READERS]-=readers;

  save_group_list();
  return 1;
}

int WriteNote(mixed message,mixed keepname)
{
  mixed *group;
  int uidok,tmp;
  string name;

  if (!pointerp(message) || sizeof(message)!=6) return 0;

  if (!pointerp(group=load_group(name=message[M_BOARD]))) return -2;

/*
  if (stringp(message[M_TITLE]) && message[M_TITLE]!="" &&
      sscanf(message[M_TITLE],"Re\^%d",tmp) && tmp>500)
  {
    printf("Reply-Counter overflow.\n");
    return -1;
  }
*/
  uidok=(geteuid(previous_object())==ROOTID)||
    (geteuid(previous_object())=="p:daemon");
  if (!TI_PO() && !uidok) return -1;
  
  if (!uidok)
    if (grouplist[name,G_OWNER] != geteuid(TI_PO()) &&
	!IS_ARCH(geteuid(TI_PO())) &&
	grouplist[name,G_WLEVEL]>query_wiz_level(geteuid(TI_PO())) &&
	member(geteuid(TI_PO()),
		     grouplist[name,G_WRITERS])==-1)
      return -1;

  if (sizeof(group)>=grouplist[name,G_MAX_MSG]) return -3;

  if (!keepname || !uidok) message[M_WRITER]=capitalize(geteuid(TI_PO()));
  if (lasttime>=time()) lasttime++;
    else lasttime=time();
  message[M_TIME]=lasttime;
  message[M_ID]=MUDNAME+":"+lasttime;
  group+=({message});
  grouplist[name,WTIME]=lasttime;
  save_group(name,group);
  save_group_list();
  return 1;
}

int RemoveNote(mixed name, mixed note)
{
  int l,num;
  mixed *group;
  int uidok;

  if ((note<0) && (name=="dwnews"))
  {
    group=({});
    grouplist[name,WTIME]=0;
    save_group(name,group);
    save_group_list();
    return 1;
  }

  if (note<0) return 0;

  if (!pointerp(group=load_group(name))) return -2;

  l=sizeof(group);
  if (l<=note)
    return -3;

  uidok=(geteuid(previous_object())==ROOTID)||
    (geteuid(previous_object())=="p:daemon");
  if (!uidok)
    if (grouplist[name,G_OWNER] != geteuid(TI_PO()) &&
	!IS_ARCH(TI_PO()) &&
	grouplist[name,G_DLEVEL]>query_wiz_level(TI_PO()) &&
	lower_case(group[note][M_WRITER])!=geteuid(TI_PO()) &&
	member(geteuid(TI_PO()),grouplist[name,G_DELETERS])==-1)
      return -1;
  
  group=group[0..note-1]+group[note+1..];
  if (sizeof(group))
    grouplist[name,WTIME]=group[<1][M_TIME];
  else
    grouplist[name,WTIME]=0;
  save_group(name,group);
  save_group_list();
  return 1;
}

mixed GetNotes(mixed name)
{
  mixed *group;
  
  if (!pointerp(group=load_group(name))) return -2;
  
  if (grouplist[name,G_OWNER] != geteuid(TI_PO()) &&
      !IS_ARCH(TI_PO()) &&
      grouplist[name,G_RLEVEL]>query_wiz_level(TI_PO()) &&
      member(grouplist[name,G_READERS],geteuid(TI_PO()))==-1)
    return -2; // No such group for the requestor :)
  return group[0..]; // COPY it
}

static void dump_file(mixed filename,mixed news)
{
  int i;
  
  for (i=0;i<sizeof(news);i++)
    write_file(filename,news[i][M_TITLE]+" ("+news[i][M_WRITER]+", "+
	       extract(dtime(news[i][M_TIME]),5,26)+"):\n"+
	       news[i][M_MESSAGE]+"\n-----------------------------------------------------------------------------\n\n\n\n");
}

void expire(mixed grp,mixed etime)
{
  int to_expire,size,last;
  mixed *group;

  if (!pointerp(group=load_group(grp))) return;
  if (etime)
  {
    if (etime>0)
      etime=etime*60*60*24;
  }
  else
    etime=grouplist[grp,G_EXPIRE];
  if (etime<=0 && etime!=-4711) return;
  to_expire=time()-etime;
  size=sizeof(group);
  last=size;
  if (!last) return;
  while ( (etime != -4711) && last && group[last-1][M_TIME]>to_expire)
    last--;
  if (!last) return;
  dump_file("news/OLD."+grp,group[0..last-1]);
  group=group[last..size-1];
  save_group(grp,group);
}

void dump_group(mixed grp)
{
  int to_expire,size,last;
  mixed *group;

  if (!pointerp(group=load_group(grp))) return;
  size=sizeof(group);
  last=size;
  if (!last) return;
  dump_file("news/DUMP."+grp,group[0..last-1]);
}

void expire_all()
{
  int i;
  mixed *keys;

  while(remove_call_out("expire_all")>=0);
  while(remove_call_out("expire")>=0);
  keys=m_indices(grouplist);
  for (i=sizeof(keys)-1;i>=0;i--)
    call_out("expire",i*3,keys[i]);
  call_out("expire_all",86400);
}

static void save_group(mixed grp,mixed group)
{
  saveload=group; // Do NOT save the accessed-Info
  save_object(NEWSPATH+grouplist[grp,G_SAVEFILE]);
  saveload=0;
}

static void save_group_list()
{
  saveload=grouplist;
  save_object(NEWSPATH+"GroupList");
  saveload=0;
}

static mixed load_group(mixed name)
{
  int num;
  mixed *ret;

  if(!member(grouplist,name)) return -1;

  restore_object(NEWSPATH+grouplist[name,G_SAVEFILE]);
  if (!pointerp(saveload)) saveload=({});
  ret=saveload[0..];
  saveload=0;
  return ret;
}

mixed GetGroups()
{
  mixed *returnlist;
  int i,group;

  returnlist=sort_array(m_indices(grouplist),#'>); //');
  if (IS_ARCH(TI_PO()))
    return returnlist;

  for (i=sizeof(returnlist)-1;i>=0;i--)
    if (!(grouplist[returnlist[i],G_RLEVEL]<=
	  query_wiz_level(TI_PO()) ||
	  grouplist[returnlist[i],G_OWNER]==geteuid(TI_PO()) ||
	  member(grouplist[returnlist[i],G_READERS],geteuid(TI_PO()))!=-1))
      returnlist=returnlist[0..i-1]+returnlist[i+1..];
  return returnlist;
}

int AskAllowedWrite(mixed n)
{
  mixed *group;

  if (!member(grouplist,n)) return -2;
  if (!pointerp(group=load_group(n))) return -2;

  if (grouplist[n,G_OWNER] != geteuid(TI_PO()) &&
      !IS_ARCH(TI_PO()) &&
      grouplist[n,G_WLEVEL]>query_wiz_level(TI_PO()) &&
      member(geteuid(TI_PO()),grouplist[n,G_WRITERS])==-1)
    return -1;

  if (sizeof(group)>=grouplist[n,G_MAX_MSG]) return -3;
  return 1;
}

// Wichtig ...

int query_prevent_shadow()
{
  return 1;
}

mixed GetNewsTime(mixed boardname)
{
  int i, ltime, j;
  mixed *keys;

  if (!boardname)
  {
    ltime=-1;
    for (i=sizeof(keys=m_indices(grouplist))-1;i>=0;i--)
      if (ltime<(j=grouplist[keys[i],WTIME])) ltime=j;
    return ltime;
  }
  if (!member(grouplist,boardname)) return -1;
  return grouplist[boardname,WTIME];
}

mixed GetGroup(string name)
{
#define gl(x) grouplist[name,x]
  return ({name,gl(1),gl(2),gl(3),gl(4),gl(5),gl(6),gl(7),gl(8),gl(9),gl(10),load_group(name)});
}

