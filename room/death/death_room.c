//
//  Aenderungen:
//
//  02.08.2008  TamTam
//  Killer wird jetzt hoffentlich auch bei Spellbook Angriffen ermittelt,
//  TOD zaehlt 100/250fache Tode mit
//
//  29.11.2004  Serii
//  der raum wird nun nichtmehr entladen (sonst kontrolliert ja keiner mehr -T)
//
//  25.03.2003  Serii
//  kleiner Bug, der beim moven in den prayroom auftrat, entfernt
//
//  24.02.2003  Serii
//  wenn das toetende object der spieler selbst ist (z.b. die shell)
//  MUSS der 'killer' nicht mehr gezwungenermassen 'etwas geheimnisvolles...'
//  sein. P_KILLER im spieler auf sich selber setzen und dann P_KILL_NAME
//  im spieler auf den gewuenschten namen...
//
//  15.01.2000  Querolin
//  Lost-XP-Statistik eingefuegt
//
//  06-02-1996	Mupfel
//  Blindheit/Taubheit sind nach dem Tod verschwunden
//
//  27.12.1997  Woody
//  "Vergiftung", "Hitze" und "Kaelte" als Todesursachen auf -T senden
//
//  03.02.1998  Woody
//  Wieder volle Lebenspunkte nach Tod (warum eigentlich? naja... egal)
//

#pragma strong_types

#include <properties.h>
#include <moving.h>
#include <language.h>
#include <wizlevels.h>
#include <combat.h>

inherit "/std/room";

#define LXP_LISTE "/p/service/querolin/listen/stat_lxp"

mixed *players;
mapping msgCache;

void do_remove();
void add_player(object pl);
varargs void flush(int unusedOnly);
void remove_player(int num);
/*
string name()
{
  return "Lars";
}
*/
void init() {
  object pl;
  string prayroom;
  int res;

  ::init();

  // Lost-XP Statistik
  LXP_LISTE->Register(this_player());

  pl=this_player();
  if (!query_once_interactive(pl)) return;
  if (!IS_WIZARD(this_player())) add_action("filter", "", 1);
  if (!(prayroom=this_player()->QueryPrayRoom())) prayroom="/d/gidhrog/gralkor/gidhrog-stadt/room/kirche";
  if (!pl->QueryProp(P_GHOST))
  {
    if (IS_WIZARD(pl) && pl->QueryProp(P_WANTS_TO_LEARN))
    {
      if (!pl->QueryProp(P_INVIS))
        call_out("room_msg",1,"Der Tod sagt: WAS WILLST DU HIER, "+
            upperstring(this_player()->name())+"?\n"+
            "DU BIST UNSTERBLICH, DU HAST HIER NICHTS ZU SUCHEN!\n\n");
    }
    else
    {
      write("Der Tod sagt: WAS TUST DU HIER? "+
          "DEINE ZEIT IST NOCH NICHT REIF!\n\n");
      if (catch(res=pl->move(prayroom,M_GO|M_NOCHECK|M_SILENT)) ||
          (res != 1) && (environment(pl)==this_object()))
        pl->move("/d/gidhrog/gralkor/gidhrog-stadt/room/kirche",M_GO|M_NOCHECK|M_SILENT);
    }
    return;
  }
  if( pl->QueryProp(P_FROG))
    pl->SetProp(P_FROG,0);
  add_player(pl);
  set_heart_beat(1);
}

void reset()
{
  ::reset();

  flush(1);
}

int clean_up(int ref)
{
//  flush(1);
  return 0;
}

void create()  {
  ::create();
  players= ({});
  flush();
  SetProp(P_NAME, "Lars");
  SetProp(P_GENDER, MALE);
  SetProp(P_ARTICLE, 0);
  SetProp( P_NO_TPORT, NO_TPORT_OUT );
  // damit man nicht mehr rausgemoved wird ("Rollenschutz")
  SetProp(P_LIGHT,1);
  SetProp(P_INT_SHORT,"Arbeitszimmer des Todes");
  SetProp(P_INT_LONG,
	  "Ein dunkler Raum, erleuchtet von dunklem Licht, das sich der Dunkelheit "
    "nicht so sehr zu widersetzen scheint, indem es leuchtet, als dass es "
    "der dunkelste Punkt in einer weniger dunklen Umgebung ist. Im seltsamen "
    "Licht erkennst Du einen zentral aufgestellten Schreibtisch, der mit "
    "Diagrammen und Büchern bedeckt ist. Die Wände verschwinden hinter "
    "Regalen, die gefüllt sind mit in Leder gebundenen, dunklen Wälzern, "
    "von denen geheimnissvolle Runen leuchten.");

  call_other(LXP_LISTE, "???");

  call_other(VOICEMASTER,"RegisterChannel","Tod",this_object());
}

varargs void flush(int unusedOnly)
{
  string *mi;
  int i;

  if (!unusedOnly)
    msgCache = ([]);
  else if (i=sizeof(mi=m_indices(msgCache))) {
    for (i-=1; i>=0; i--) {
      if (msgCache[mi[i],1])
	msgCache[mi[i],1] = 0;
      else
	msgCache = m_delete(msgCache, mi[i]);
    }
  }
}

static
string expand(string table, int value)
{
  int sz, wert, i;
  string *texte;

  sz = sizeof(texte = explode(table, "##"));
  wert=0;
  for (i = 0; i < sz; i++)
    if (i%2) {
      sscanf(texte[i],"%d",wert);
      if (value < wert) break;
    } else
      table=texte[i];

  return table;
}

#define TOS(s) s[<1]
#define STOS(s) s[<2]
#define PUSH(x,s) (s+= ({ x }))
#define POP(s)	(s=s[0..<2])

static
string parseText(string msg, object pl)  /* ziemlich vom htmld abgekupfert ;) */
{
  string *words, *texte, *todo, *done, tmp, cmd;
  int i,k, sz;
  int endFlag;

  sz = sizeof(words = regexplode(msg, "[<][^<]*[>]"));
  todo = ({ });
  done = ({""});

  for (i=1; i<sz; i+=2) {
    cmd = words[i][1..<2];
    TOS(done) += words[i-1];

    if (cmd[0]=='/') {
      endFlag = 1;
      cmd = cmd[1..];
    }
    else
      endFlag = 0;

    switch(cmd[0]) {
      case 'A': /*** Alignment ersetzen ***/
      if (!endFlag) {
	PUSH(cmd, todo); PUSH("",done);
      }
      else
	if (todo[<1] == "A") {
	  STOS(done) += expand(TOS(done), pl->QueryProp(P_ALIGN));
	  done = done[0..<2];
	  todo = todo[0..<2];
	}
      break;

      case 'D': /*** Tode ersetzen ***/
      if (!endFlag) {
	PUSH(cmd, todo); PUSH("",done);
      }
      else
	if (todo[<1] == "D") {
	  STOS(done) += expand(TOS(done), pl->QueryProp(P_DEADS));
	  POP(done); POP(todo);
	}
      break;

      case 'L': /*** Level ersetzen ***/
      if (!endFlag) {
	PUSH(cmd, todo); PUSH("",done);
      }
      else
	if (todo[<1] == "L") {
	  STOS(done) += expand(TOS(done), pl->QueryProp(P_LEVEL));
	  POP(done); POP(todo);
	}
      break;

      case 'Z': /*** Zufall ersetzen ***/
      if (!endFlag) {
	PUSH(cmd, todo); PUSH("",done);
      }
      else {
	if (todo[<1][0] == 'Z') {
	  int cnt, rnd, wert, sz2;

	  if (!sscanf(todo[<1], "Z=%d", rnd))
	    STOS(done) += "\n###\n### Syntax Error in <Z>!\n###\n\n";
	  else {
	    rnd=random(rnd);
	    sz2=sizeof(texte=efun::explode(TOS(done),"##"));wert=0;cnt=0;
	    for (k=1;k<sz2;k+=2) {
	      sscanf(texte[k],"%d",wert);
	      cnt+=wert;
	      if (rnd<cnt) {
		STOS(done)+=texte[k+1];
		break;
	      }
	    }
	  }
	  POP(done); POP(todo);
	}
      }
      break;

      case 'G': /*** Gender ersetzen ***/
      if (!endFlag) {
	PUSH(cmd, todo); PUSH("",done);
      }
      else {
	if(sizeof(texte=regexplode(TOS(done),":"))==3)
	  STOS(done) += texte[2*(pl->QueryProp(P_GENDER)==FEMALE)];
	POP(done); POP(todo);
      }
      break;

      case 'R': /*** Rasse ersetzen ***/
      if (!endFlag) {
	PUSH(cmd, todo); PUSH("",done);
      }
      else {
	if(sizeof(texte=regexplode(TOS(done),"\\|"))==7) {
	  int race;
	  race = member( ({ "Mensch","Elf","Zwerg","Hobbit","Ork", "Orc", "Brownie", "Goblin" }), pl->QueryProp(P_RACE)) + 1;
	  STOS(done) += texte[2*race];
	}
	POP(done); POP(todo);
      }
      break;

      case 'n': /*** Name, normal geschrieben ***/
      TOS(done) += (pl->name(RAW));
      break;

      case 'N': /*** Name, in Grossbuchstaben ***/
      TOS(done) += upperstring(pl->name(RAW));
      break;
    }
  }
  PUSH(words[<1], done);
  return implode(done, "");
}

void heart_beat() {
  int j,nr;
  string msg;

  j=0;
  while (j<sizeof(players))
    if (!objectp(players[j][0]) || environment(players[j][0])!=this_object())
      players=players[0..j-1]+players[j+1..];
    else
      j++;

  if (!sizeof(players)) {
    set_heart_beat(0);
    return;
  }

  for (j=sizeof(players)-1;j>=0;j--) {
    nr=++players[j][1];

    if (mappingp(players[j][2]))
      msg=players[j][2][nr];
    else
      msg=0;
    if (!msg)
      msg=players[j][3][1][nr];
    if (msg)
      tell_object(players[j][0],parseText(msg,players[j][0]));
  }
  do_remove();
}

#define PRBUGT 1
//#undef PRBUGT

void do_remove() {
  int j, nr, res;
  string prayroom;
  object plobj, pl;

  nr=sizeof(players);
  for (j=0;j<nr;j++) {
    if (players[j][1]>=players[j][3][0]) {
      pl = players[j][0];
      while (plobj = present("\ndeath_mark", pl))
        plobj->remove();
      if (!(prayroom=pl->QueryPrayRoom()))
        prayroom="/d/gidhrog/gralkor/gidhrog-stadt/room/kirche";
      // Bevor der Spieler wieder im seine Kapelle gebracht
      // wird, werden Blindheit und Taubheit entfernt
      if (pl->QueryProp(P_BLIND))
      {
        pl->SetProp(P_BLIND, 0);
        tell_object(pl, "Der Tod hat die Blindheit von Dir genommen!\n");
      }
      if (pl->QueryProp(P_DEAF))
      {
        pl->SetProp(P_DEAF, 0);
        tell_object(pl, "Du kannst wieder hören!\n");
      }
      pl->SetProp(P_HP, pl->QueryProp(P_MAX_HP));
      // Jetzt wird der Spieler in die Kapelle gebracht
#ifdef PRBUGT
      catch(pl->move(prayroom,M_GO|M_NOCHECK|M_SILENT));
      if(object_name(environment(pl))!=prayroom)
        pl->move("/d/gidhrog/gralkor/gidhrog-stadt/room/kirche",
	M_GO|M_NOCHECK);
#endif
#ifndef PRBUGT
      if (catch(res=pl->move(prayroom,M_GO|M_NOCHECK|M_SILENT)) || res!=1)
        pl->move("/d/gidhrog/gralkor/gidhrog-stadt/room/kirche",
	M_GO|M_NOCHECK|M_SILENT);
#endif
      remove_player(j);
      do_remove();
      return;
    }
  }
}

varargs mixed get_sequence(object pl, string str)
{
  string *sequences,seq,*pl_seq;
  int i,len,cacheable;
  mapping m;
  
  if (!stringp(str)||file_size(str)<=0)
  {
    sequences=get_dir("/room/death/sequences/*")-({".",".."});
    
    // doppelte Sequenzen reduzieren
    i = sizeof(sequences);
    if(pointerp(pl_seq = pl->QueryProp(P_LAST_DEATH_SEQUENCES)))
      sequences -= pl_seq;  // Die bereits gesehenen Sequenzen nicht beruecksichtigen
    else
      pl_seq = ({});  // Erster Tod -> neues Array
    if(!sizeof(sequences)) // Falls keine Sequenz mehr uebrig bleibt
      seq = "lars";  // bekommt man die Lars-Sequenz
    else  // ansonsten zufaellig eine der noch nicht gesehenen
      seq = sequences[random(sizeof(sequences))];
    pl_seq += ({seq}); // Die neue Sequenz zu den gesehenen dazufuegen
    if(sizeof(pl_seq) > i/2) // nun hat der Spieler die Haelfte der Sequenzen gesehen
      pl_seq -= ({pl_seq[0]}); // die aelteste Sequenz wird wieder freigegeben
    pl->SetProp(P_LAST_DEATH_SEQUENCES, pl_seq);
    
    str="/room/death/sequences/"+seq;
  }

  if (cacheable=((sizeof(str)>21) && (str[0..21]=="/room/death/sequences/"))) {
    if (member(msgCache, str)) {
      msgCache[str,1] = 1;  // Touch it!
      return msgCache[str];
    }
  }
  sequences=efun::explode(read_file(str),"\n");
  sscanf(sequences[0],"%d",len);
  seq=implode(sequences[1..],"\n");
  sequences=regexplode(seq,"[0-9][0-9]*:");
  m=([]);
  for (i=1;i<sizeof(sequences)-1;i+=2)
    m[(int)sequences[i]]=sequences[i+1];

  if (cacheable)
    msgCache += ([ str: ({ len, m }); 1 ]);

  return ({len,m});
}

void add_player(object pl) {
  // Description:   Adds a player to the list
  int i;
  int escaped,flag,plflag;
  object pre,kill_liv,kill_ob;
  mixed dseq,act_seq,msg_flags;
  string fn, killer_name, killer_msg;

  kill_liv=0;kill_ob=0;dseq=0;plflag=0;
 if (objectp(pre=pl->QueryProp(P_LAST_COMMAND_ENV)))
 {

 }
  for (i=0;pre=previous_object(i);i++) {
    if (!objectp(pre)) continue;
    if (pre==pl) continue;
    fn=object_name(pre);
    if (fn[0..12]=="/secure/login" && !kill_liv)
      escaped=1;
    if (fn[0..7]=="/secure/"&&fn[0..13]!="/secure/merlin") continue;
    if (fn[0..21]=="/room/death/death_mark") continue;
    if (living(pre)) {
      kill_liv=pre; // Killer
      break;
    }
    kill_ob=pre; // killendes Objekt
  }
  if (kill_liv && kill_ob &&
      (kill_liv->QueryProp(P_GUILD)==kill_ob->GuildName()))
      kill_ob=kill_liv; // fix Spellbook-Angriff
  if (objectp(pre = pl->QueryProp(P_KILLER) || pl->QueryProp(P_LAST_KILLER)))
  {
    dseq=pre->QueryProp(P_ENEMY_DEATH_SEQUENCE);
    if(!(killer_name=pre->QueryProp(P_KILL_NAME)))
      killer_name = pre->QueryProp(P_NAME);
    killer_msg = pre->QueryProp(P_KILL_MSG);
  }
  if (!dseq && kill_liv && function_exists("QueryProp",kill_liv))
  {
    dseq=kill_liv->QueryProp(P_ENEMY_DEATH_SEQUENCE);
    if(!(killer_name=kill_liv->QueryProp(P_KILL_NAME)))
      killer_name = kill_liv->name();
    killer_msg = kill_liv->QueryProp(P_KILL_MSG);
    msg_flags = kill_liv->QueryProp(P_MSG_FLAGS);
  }
  if (!dseq && kill_ob && function_exists("QueryProp",kill_ob))
  {
    dseq=kill_ob->QueryProp(P_ENEMY_DEATH_SEQUENCE);
    if(!(killer_name=kill_ob->QueryProp(P_KILL_NAME)))
      killer_name = kill_ob->QueryProp(P_NAME);
    killer_msg = kill_ob->QueryProp(P_KILL_MSG);
    msg_flags = kill_ob->QueryProp(P_MSG_FLAGS);
  }
  if (mappingp(msg_flags)) flag = msg_flags[P_KILL_MSG];
  act_seq=0;
  if (mappingp(dseq))
    act_seq=get_sequence(pl, "/room/death/sequences/lars");
  else if (pointerp(dseq))  // ganze Todessequenz...
    act_seq=dseq;
  else
    if (stringp(dseq))
      act_seq=get_sequence(pl, dseq);
  if (!act_seq) act_seq=get_sequence(pl);
  if (!mappingp(dseq)) dseq=0;
  players+=({({pl,0,dseq,act_seq})});

  if(
//  (!pl->QueryProp(P_TESTPLAYER) && !IS_LEARNER(pl)) &&
     (!kill_ob || !environment(kill_ob) ||
      object_name(environment(kill_ob))[0..27]!="/players/dancer/lucky/arena"))
  {
    if (!killer_name)
    {
      if (!escaped || killer_msg)
      {
        mixed damtypes;
        killer_name = "Etwas Geheimnisvolles und Unbekanntes";  // (default)
	if(pl->QueryProp(P_KILLER)==pl)
	{
	  killer_name=pl->QueryProp(P_KILL_NAME);
	  plflag=1;
	}
        damtypes = pl->QueryProp(P_LAST_DAMTYPES);
        if (sizeof(damtypes)==1)
          if (damtypes[0]==DT_COLD)
            killer_name = "Kälte";
          else if (damtypes[0]==DT_FIRE)
            killer_name = "Hitze";
          else if (damtypes[0]==DT_POISON)
            killer_name = "Eine Vergiftung";
      }
      else
      {
        killer_name="";
        killer_msg=upperstring(getuid(pl))+
          " VERSUCHTE, MIR ZU ENTKOMMEN - JETZT HABE ICH WIEDER EXTRA-ARBEIT"+
          " MIT "+ (pl->QueryProp(P_GENDER) != 2 ? "IHM" : "IHR") + " ...";
      }
    }
    SetProp(P_NAME,"Lars");  // Sicher ist sicher!
    if (killer_name!=""){
      VOICEMASTER->SendToChannel("Tod", this_object(), SAY,
				 capitalize(killer_name)+" hat gerade "+
				 capitalize(getuid(pl)) +" umgebracht.");
      if(plflag){pl->SetProp(P_KILL_NAME,0);pl->SetProp(P_KILLER,0);}}
    i=pl->QueryProp(P_DEADS);
    if ( i && (i==10 || i%100==0 || i%250==0) ) {
      SetProp(P_NAME,"Tod");
      VOICEMASTER->SendToChannel("Tod", this_object(), SAY,
				 sprintf("DAS WAR SCHON DAS %dTE MAL!",i));
      SetProp(P_NAME,"Lars");
    }
    if (killer_msg)
      if (flag)  // emote & gemote
        VOICEMASTER->SendToChannel("Tod", this_object(), EMPTY,
            break_string("[Tod:"+capitalize(killer_name)+
                " "+funcall(killer_msg)+"]", 78));
      else
        VOICEMASTER->SendToChannel("Tod", this_object(), EMPTY,
            break_string(funcall(killer_msg), 78,
                "[Tod:"+capitalize(killer_name)+"] "));
  }
}

void remove_player(int num) {
  // Description:   Removes a player from the list
  int i, j, x, oldlist;

  players=players[0..num-1]+players[num+1..];
  if (!sizeof(players)) set_heart_beat(0);
}

int filter(string str) {
  // Description:   Filter out relevant commands.
  string verb;
  object pl;

  pl=this_player();
  verb=query_verb();
  if (verb=="quit" || verb=="ende") {
    write("DU KANNST DEM TOD NICHT ENTRINNEN!\n");
    return 0;
  }
  if (verb=="typo" || verb=="fehler" || verb=="bug" || verb=="idee")
    return 0;
  write("Dein Körper gehorcht Dir nicht !\n");
  return 1;
}

