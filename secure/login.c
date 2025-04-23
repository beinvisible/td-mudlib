/* Tamedhon MUDlib
 *
 * /secure/login.c -- login object
 *
 * 2024/06/05 - max player 225 - Aloy
 *
 * $Date: 2023/07/19 $
 * $Revision: 1.9 $
 * $Log: login.c,v $
 *
 * Revision 1.9 2023/07/19 Aloy
 * Rassenwahl Optikfix bei ungerader Anzahl, Chareditor
 * 
 * Revision 1.8 2012/01/18 21:30:00 Gralkor
 * https://www.facebook.com/tamedhon2.0/eingebunden
 *
 * Revision 1.7b 2003/08/22 10:50:00  Serii
 * nen kleinen bug aus den extralogin-sachen entfernt
 *
 * Revision 1.7a 2003/07/14 00:00:00  Serii
 *
 * Revision 1.7  2003/07/05 14:30:00  Falgrim
 * zufaelliges bild beim einloggen
 * ascii-bilder einfach in /etc/ascii/ ablegen
 *
 * Revision 1.6  2002/21/11 19:51:00  Serii
 * alles fuer extra-logins eingerichtet (mit freundlicher genehmigung des
 * Beutellandes, von denen ich mir das konzept abgeschaut hab :) )
 *
 * Revision 1.5  2002/08/05 11:00:00  Querolin
 * Neue Spieler haben jetzt cap(lowerstring()) bei Namen damit keine 
 * Namen mehr mit lauter Grossbuchstaben vorkommen etc.
 *
 * Revision 1.4  2002/04/12 16:00:00  Querolin
 * typo bei spielpause raus...
 *
 * Revision 1.3  1997/10/14 18:19:31  Woody
 * MAXUSERS auf 60 erhoeht
 *
 * Revision 1.2  1997/06/11 12:19:42  Woody
 * MAXUSERS auf 50 erhoeht
 *
 * Revision 1.1  1997/03/15 23:09:29  Woody
 * prepared for revision control
 *
 * 1997/01/21  Woody@SL  'rip' beim Einloggen jetzt fuer alle Erzis
 *                       Syntax: "<loginname> rip <ip-adresse>"
 * 1996/11/20  Woody@SL  WELCOME.localhost beim einloggen von Inside
 * 1996/10/12  Woody@SL  Gaeste kriegen den guestremover ins inventory
 *                       gecloned, dadurch werden sie destructed, wenn
 *                       sie netztot werden.
 * 1996/10/05  Woody@SL  Limit von drei Versuchen zum erfolgreichen login
 * 1996/10/02  Woody@SL  TimeBanish Support, Setzen von P_CREATION_DATE
 * 1996/07/13  Woody@SL  bugfixing (Bug von Anatol gemeldet):
 *                       Es war moeglich, zwei Spieler "Xyz" und "XyZ"
 *                       einzuloggen. Ausserdem konnte sich nach Eingabe
 *                       von "neu" und VOR Eingabe des gewuenschten Namens
 *                       nicht gleichzeitig noch jemand neu einloggen.
 *                       Durch Verschieben von der "Anmeldung unter diesem
 *                       Namen laeuft bereits"-Abfrage in newplayer()
 *                       sind beide Probleme (hofftl.) geloest. Auf
 *                       diese Art ist es ausserdem weiterhin moeglich,
 *                       Namen mit Grossbuchstaben drin anzulegen.
 * 1996/04/06  Woody@SL  bugfixing - es gab massive Probs, wenn man beim
 *                       1. Einloggen bei der EMail-Abfrage disconnectete!
 * 1996/04/02  Woody@SL  "neu" fuer neuen Charakter, "remoteip" -> "rip"
 * 1996/02/22  Woody@SL  general debugging
 * 1996/01/23  Woody@SL  "Schoen, dass Du wieder da bist, NAME"
 */

/*
 * This object is cloned for every user trying to log in 
 * We are still running root.
 *
 * login.c looks up the username in the secure/PASSWD file. If it is
 * found, the password is checked. If the user is already logged in,
 * he will be reconnected to the running object. If the other object
 * is still interactive, that will be disconnected before the user is
 * reconnected to that object.
 *
 * If the user is not in PASSWD, a new entry with level 0 is created.
 * All PASSWD writing is done in secure/master.c.
 *
 */

// Erweiterung auf defines von Mupfel@AnderLand
// MAXUSERS   max. Anzahl gleichzeitig eingeloggter Benutzer (definiert im GD)
// MAXPLAYER1 max. Anzahl Spieler (Magier koennen noch einloggen)
// MAXPLAYER2 max. Anzahl Spieler und Magier (Erzmagier koennen noch einloggen)

#pragma strong_types
#pragma no_shadow

#define MAXUSERS    225
#define MAXPLAYER1  MAXUSERS-10
#define MAXPLAYER2  MAXUSERS-5

#define NAME_QUESTION "Wie lautet Dein Name (\"neu\" für neuen Spieler) ?  "

/* Variablen des secure save file */
int level;
string password, name, shell;
int loginfails;
mixed *domains;
int creation_date;
static int guestx;

#ifndef DEBUG
#define DEBUG(x) if(find_player("debugger"))tell_object(find_player("debugger"),x)
#endif
#include <config.h>
#include <properties.h>
#include <moving.h>
#include "/secure/wizlevels.h"
#include <input_to.h>
#include <configuration.h>
#include <telnet.h>

#define GUESTMASTER "/secure/guestmaster"
static string loginname;
static string cap_name;
static object myself;
static string *userentry;
static string banish;
static mixed *races;
static string my_ip;
static int count;

nomask string loginname() { return loginname ? loginname : ""; }
int valid_name(string str);
varargs static int load_player_object(int guestflag);
varargs static void load_player_ob_2(string obname, int guestflag);
static void ask_race_question();
public nomask int logon();

static int check_too_many_logons()
{
    object *u;
    string ip;

    ip = query_ip_number(this_object());
    // users() nehmen, falls nicht-interaktive Clones von login.c existieren.
    u = filter( users(), function status (object ob, string addr) {
        return load_name(ob) == "/secure/login"
               && query_ip_number(ob) == addr;
    }, ip );

    if ( sizeof(u) > 2) {
        write( "\nEs laufen schon zu viele Anmeldungen von Deiner Adresse "
               "aus.\nProbier es bitte in ein bis zwei Minuten noch "
               "einmal.\n" );

        log_file( "LOGIN_DENY", sprintf( "%s: >2 Logons von %-15s (%s)\n",
                                         ctime(time())[4..15],
                                         query_ip_number(this_object()),
                                         query_ip_name(this_object()) ) );
        return 1;
    }
    else
        return 0;
}

/*
 * This is the function that gets called by /secure/master for every user
 */

public nomask int logon()
{
  string *sequences,str;
  count=0;
  loginname = "logon";
  myself = this_player();

//  binary_message(({255,253,34}));

  if (query_ip_number(this_object())=="127.0.0.1")
    cat("/etc/WELCOME.localhost");
  else{
    sequences=get_dir("/etc/ascii/*")-({".",".."});
    str="/etc/ascii/"+sequences[random(sizeof(sequences))];
    cat(str);
  }

  write("\n");
  if(tls_query_connection_state(myself))
  {
    write("=== Verbindung TLS verschlüsselt. ===");
  }
    else
  {
    write("=== Verbindung NICHT TLS verschlüsselt. ===\n");
    write("=== TLS Verbindung auf Port 4712 empfohlen! ===\n");
  }
  write("\n");
  write(">>> Wir benutzen UTF-8! Bitte konfiguriere deinen Client entsprechend. <<<");
  write("\n\n");
  write(NAME_QUESTION);
  input_to("logon2");
  call_out("time_out", 600);
  return 1;
}

static int logon2(string str) {
  object ob;
  int i,len,arg;
  string st1,st2,text;
  mixed txt,*xlp;

  if (!str || (str=="" && ++count>=3)) {
    write("\nHmm... das war wohl nix. Versuchs nochmal!\n\n");
    destruct(this_object());
    return 0;
  }
  if (str=="")
  {
    write(NAME_QUESTION);
    input_to("logon2");
    return 1;
  }
  count=0;
  if (loginname != "logon") {
    log_file("ILLEGAL","illegal patch of login");
    destruct(this_object());
    return 0;
  }
  if (str[0..3]=="GET ")
  {
    cat("/etc/HTMLREDIRECT");
    destruct(this_object());
    return 0;
  }
  str = lower_case(str);
  if ((sscanf(str,"%s rip %s",st1,st2)==2) && (IS_ARCH(st1)))
  {
    str=st1;
    my_ip=st2;
    write("Ok, IP-Adresse gesetzt.\n");
  }
  cap_name = capitalize(str);
  if (!valid_name(str)) {
    write(NAME_QUESTION);
    input_to("logon2");
    return 1;
  }

  if(sscanf(str,"gast%d",arg)==1)
  {
    write( "Du meinst wohl 'Gast' ...\n" );
    str = "gast";
  }
  // loginname = lower_case(str);
  loginname = str;
//#if 0
  if ((query_ip_number(this_object())[0..10]=="217.160.211" ||
       query_ip_number(this_object())[0..10]=="aaaaaaaaaaa" ) // &&
//      (str!="lug"&&str!="morgon"&&str!="baldoch"&&str!="victoria"&&
//       str!="carelli"&&str!="familiar"&&str!="harg"&&str!="isdwelg"&&
//       str!="jikhbar"&&str!="pirx"&&str!="morgonia"&&str!="symbiont"&&
//       str!="jago"&&str!="jagoda"&&str!="ruebe"&&str!="cruuv"&&str!="nork"&&
//       str!="jann"&&str!="blake"&&str!="larisia"&&str!="zora"&&str!="tiamat")
     )
  {
    write("Serii - Du kommst hier ned rein, weil Du ein böser Junge\n"
	  "warst und das MUD fünfmal gecrasht hast. Das kostet Dich\n"
	  "jetzt zumindest einige Bier!!!\n\nWenn Du wieder rein\n"
	  "willst, dann steig von einer anderen IP-Adresse ein, oder\n"
          "schick ein mail an admin@tamedhon.at!\n\n"
	  "so long,\nQuero\n\n");
    
    destruct(this_object());
  }
  if ((query_ip_number(this_object())[0..10]=="178.217.187.39")) {
     write(break_string(
     "Von dieser IP ist das Login nicht erlaubt. Tor-Exits wurden "
     "bisher nur von SPAMmern verwendet. Bitte nutze eine andere IP. "));
     destruct(this_object());
 }
//#endif
  /* See if player is tbanished */
  if (({string})__MASTER_OBJECT__->QueryTBanished(str))
  {
    write( sprintf(
         "\nAufgrund Deiner Spielpause kannst Du leider erst \n"
        +"am %s wieder ins Mud kommen. Bis dann!\n\n\n",
          (i == -1 ? "St. Nimmerleinstag" :
           funcall(symbol_function('dtime),i)[0..16])));
    destruct(this_object());
  }

  /* read the secure save file to see if character already exists */
  if (str!="gast" && !restore_object(__MASTER_OBJECT__->secure_savefile(str)))
  {
    if (str!="neu")
    {
      write("Tut mir leid, diesen Namen kenne ich nicht.\n");
      write("Versuch einen anderen (oder \"neu\"): ");
      loginname="logon";
      input_to("logon2");
      return 1;
    }
    else
    {
        /* -- Hinweis auf die Regeln -- */
  write("*****************************************************************\n");
  write("*                        H  I  N  W  E  I  S                    *\n");
  write("*****************************************************************\n");
  write("\n");
  write("In diesem MUD gibt es gewisse Regeln, die zu beachten sind. Dazu\n");
  write("gehört es auch sich...\n\n");
  write("                      einen PASSENDEN SPIELERNAMEN\n\n");
  write("auszusuchen.\n\n");
  write("Der Spielername sollte einfach nur generell im weitestens Sinne\n");
  write("FANTASY-mäßig sein und in ein imaginäres Mittelalter passen.\n");
  write("\nTitel, Berufsbezeichnungen, Schimpfwörter, Funktionsnamen, \n");
  write("technologische Begriffe sind hier als Spielernamen nicht\n");
  write("erwünscht und können jederzeit ohne Vorwarnung gelöscht und\n");
  write("gebannt werden. Für Genaueres dann im Spiel 'hilfe regeln' eingeben.\n");
  write("*****************************************************************\n");

      write("\nWie willst Du Dich denn nennen? ");
      input_to("newplayer");
      return 1;
    }
  }
  else {
    if (str == "gast") {
      load_player_object(1);
      return 1;
    }
    write("Schön, dass Du wieder da bist, "+cap_name+"!\n");
    if (password == "" || !password) {
      write("Du hast KEIN PASSWORT!\n");
      write("Benutze den \"passwort\"-Befehl, um das zu ändern !\n");
      load_player_object();
      return 1;
    }
    write("Passwort: ");
    input_to("check_password",1);
    return 1;
  }
}

static int newplayer(string str)
{
  object *user;
  mixed txt;
  int i;

  if (!str || (str=="" && ++count>=3)) {
    write("\nHmm... das war wohl nix. Versuchs nochmal!\n\n");
    destruct(this_object());
    return 0;
  }
  /* new character */
  cap_name = capitalize(lowerstring(str));
  loginname = str = lower_case(str);
  if ( sizeof(str) < 3 )
  {
    write("Der Name ist zu kurz.\n");
    write("Versuch einen anderen: ");
    // loginname="logon";
    input_to("newplayer");
    return 1;
  }
  if (!valid_name(str)) {
    write("Wie willst Du Dich denn nennen? ");
    input_to("newplayer");
    return 1;
  }
  if ((txt=({string})__MASTER_OBJECT__->QueryBanished(str)))
  {
    write(txt+"\n");
    write("Gib Dir einen anderen Namen: ");
    // loginname="logon";
    input_to("newplayer");
    return 1;
  }
  for (i=0; i<sizeof(({str})); i++)
  {
    if(sizeof(regexp(({str}),"[^a-zäöü]")))
    {
      write("Der Name ist nicht zulässig.\n");
      write("Versuch einen anderen: ");
      // loginname="logon";
      input_to("newplayer");
      return 1;
    }
  }
  for ( i = sizeof(user = users() - ({ 0, this_object() })); i--; )
  if ( object_name(user[i])[0..12] == "/secure/login" &&
       (({string})user[i]->loginname()) == loginname ){
      write( "Eine Anmeldung für diesen Namen läuft bereits.\n" );
      destruct( this_object() );
      return 1;
  }
  if (({string})__MASTER_OBJECT__->QueryTBanished(str))
  {
    write (sprintf("Es gibt schon einen Spieler diesen Namens.\n"
        +"Allerdings kann er/sie erst am %s wieder ins Mud kommen.\n",
          (i == -1 ? "St. Nimmerleinstag" :
           funcall(symbol_function('dtime),i)[0..16])));
    write("Gib Dir einen anderen Namen: ");
    // loginname="logon";
    input_to("newplayer");
    return 1;
  }
  if (restore_object(__MASTER_OBJECT__->secure_savefile(str)))
  {
    write("Einen Spieler dieses Namens gibt es hier schon.\n");
    write("Versuch einen anderen Namen: ");
    // loginname="logon";
    input_to("newplayer");
    return 1;
  }
  
  /* Initialize the new secure savefile */
  name = str;
  password = "";
  level = 0;
  domains = ({ });
  shell = "";

  creation_date = time();
  write("\nOk, neuer Spieler '"+cap_name+"'.\n");
  write("Wähle ein Passwort: ");
  input_to("new_password",1);
  return 1;
}

void sorry() {
  cat("/etc/SORRY");
  destruct(this_object());
}

static int new_password(string str) {
  write("\n");
  password = str;
  if (lower_case(str)==lower_case(loginname))
  {
    write("Das ist zu einfach zu erraten. Nimm bitte ein anderes.\n");
	write("Bitte gib ein Passwort an: ");
	input_to("new_password",1);
	return 1;
  }
  if (sizeof(str)<6)
  {
    write("Das Passwort muss wenigstens 6 Zeichen lang sein.\n");
    write("Bitte gib ein Passwort an: ");
    input_to("new_password",1);
    return 1;
  }
  write("\nZur Erinnerung: Es ist  v e r b o t e n, andere Spieler anzugreifen!\n");
  write("Das gilt auch für Frösche, bei denen \"Ein Frosch namens XXXXX\" steht.\n\n");
  write("Passwort bitte nochmal eingeben: ");
  input_to("again_password",1);
  return 1;
}

static int again_password(string str) {
  write("\n");
  if (str != password) {
    write("Die Passwörter stimmten nicht überein!\n");
    destruct(this_object());
    return 1;
  }
  password = crypt(password,0);
  save_object(SECURESAVEPATH+loginname[0..0]+"/"+loginname);
  "/secure/master"->RemoveFromCache(loginname);
  load_player_object();
  return 1;
}

static int check_password(string str)
{
  write("\n");
   //write("nochmal: '"+str+"' : '"+password+"' -> '"+crypt(str,0)+"'\n");  // Zeile auskommentieren!!!  quero
  // write("adsf1: " + crypt(str, password)[0] + " : " + crypt(str,password)[1] + " : " + crypt(str,password)[2] + " : " + crypt(str,password)[3] + " : " + crypt(str,0) + " :\n");
   //
  if (crypt(str,password) != password)
  {
    write("Falsches Password!\n");
    log_file("LOGINFAIL",sprintf("PASSWORD: %O %O %s\n",query_ip_name(this_object()),loginname,ctime(time())));
    loginfails++;
    save_object(SECURESAVEPATH+loginname[0..0]+"/"+loginname);
    "/secure/master"->RemoveFromCache(loginname);
    destruct(this_object());
    return 1;
  }
  if (name)
  {
    save_object(SECURESAVEPATH+loginname[0..0]+"/"+loginname);
    "/secure/master"->RemoveFromCache(loginname);
  }
  if (loginfails)
  {
    write(loginfails+" fehlgeschlagene"+(loginfails==1?"r":"")+" Login-Versuch"+(loginfails==1?"":"e")+" seit dem letzen erfolgreichen Login.\n");
    loginfails=0;
    save_object(SECURESAVEPATH+loginname[0..0]+"/"+loginname);
    "/secure/master"->RemoveFromCache(loginname);
  }
  load_player_object();
  return 1;
}

static void select_race()
{
  int i;
  string race;
  int selectable;

  races = get_dir( "/std/shells/*.c" );

  // Mensch soll immer als erstes in der Auswahlliste stehen.
  if (member(races,"human.c")!=-1)
          races=({"human.c"})+(races-({"human.c"}));
  
  for ( i = sizeof(races); i--; ){
      races[i] = "/std/shells/" + races[i][0..<3];
      selectable = 0;
      race = 0;
      if ( catch(selectable = ({int})call_other( races[i],
                 "QueryAllowSelect" ); publish) 
          || !selectable)
          selectable = 0;
      else if ( catch(race = ({string})call_other(races[i],
              "QueryProp", P_RACE );publish) )
          race = 0;

      if ( !selectable || !sizeof(race) )
          races[i..i] = ({});
      else
          races[i] = ({ races[i], race });
  }

  if ( sizeof(races) == 1 ){
      write( "Es gibt nur eine Rasse, Du hast also keine Wahl.\n" );

      shell = races[0][0];
      master()->set_player_object( loginname, shell );

      return load_player_ob_2( shell, 0 );
  }
  return ask_race_question();
}

static void ask_race_question()
{
  int i;

  write("Du musst Dich jetzt entscheiden, welcher Rasse Du in dieser Welt angehören\n"+
        "möchtest. Alle Rassen haben verschiedene Vor- und Nachteile, insgesamt aber\n"+
        "gleich gute Chancen. Auch das Startgebiet hängt von der ausgewählten Rasse\n"+
        "ab. Im Normalfall kann die Rasse nicht mehr gewechselt werden, nachdem sie ein\n"+
        "mal ausgewählt wurde. Überlege Dir Deine Entscheidung also gut. Derzeit ste-\n"+
        "hen folgende Rassen zur Auswahl:\n\n");
  for (i=0;i<sizeof(races);i++)
  {
    printf("% 2d. %-30s   ",i+1,capitalize(races[i][1]));
    if (i%2==0  && i<sizeof(races)-1) write("| "); else write("\n");
  }
  if (sizeof(races)%2!=0) write("\n");
  write("\nDurch Eingabe einer Ziffer wählst Du die Rasse aus, durch Eingabe eines \"\?\"\n"+
        "gefolgt von einer Ziffer erhälst Du nähere Informationen über eine Rasse.\n"+
        "Ein \"\?\" allein wiederholt diese Liste.\n"+
        "\nWas willst Du tun: ");
  input_to("get_race_answer");
  return;
}

static void get_race_answer( string str )
{
    int num;

    if ( str == "?" )
        return ask_race_question();

    if ( sscanf( str, "?%d", num ) ){
        if ( num < 1 || num > sizeof(races) ){
            write( "Das geht nicht.\n\n");
            input_to( "get_race_answer", INPUT_PROMPT,
                "Was willst Du tun: ");
            return;
        }

        write( ({string})call_other( races[num - 1][0], "QueryProp", P_RACE_DESCRIPTION ));
        input_to( "get_race_answer", INPUT_PROMPT,
            "\nWas willst Du tun: ");
        return;
    }

    if ( sscanf( str, "%d", num ) && num >= 1 && num <= sizeof(races) ){
        write( "Ok, Du bist jetzt ein "
            + capitalize(races[num-1][1]) + ".\n" );

        shell = races[num-1][0];
        master()->set_player_object( loginname, shell );
        return load_player_ob_2( shell, 0 );
    }

    write("Wie bitte?\n\n" );
    input_to( "get_race_answer", INPUT_PROMPT,
        "Was willst Du tun: ");
}

varargs static int load_player_object(int guestflag)
{
  string obname;
  object ob;
  string err,fname;
  int was_interactive;

  if ( sizeof(users()) >= MAXPLAYER1 && !IS_WIZARD(loginname) )
  {
    write("Die maximale Spielerzahl wurde bereits erreicht!!!\n"+
	  "Aus technischen Gründen dürfen sich nur noch Magier einloggen.\n"+
	  "Versuch es später noch einmal ...\n");
    destruct(this_object());
    return 1;
  }
  else if ( sizeof(users()) >= MAXPLAYER2 && !IS_ARCH(loginname) )
  {
    write("Die maximale Spieler- und Magierzahl wurde bereits erreicht!!!\n"+
	  "Aus technischen Gruenden dürfen sich nur noch Erzmagier einloggen.\n"+
	  "Versuch es später noch einmal ...\n");
    destruct(this_object());
    return 1;
  }
  if ((fname=({string})__MASTER_OBJECT__->secure_isavefile(loginname)) != "")
  {
    save_object(SECURESAVEPATH+loginname[0..0]+"/"+loginname);
    "/secure/master"->RemoveFromCache(loginname);
    // Just to be sure ...
    rm(fname+".o");
    log_file("REACTIVATE",ctime(time())+": "+capitalize(loginname)+
	     " reactivated\n");
  }

  if (guestflag)
  {
    guestx = GUESTMASTER ? GUESTMASTER->new_guest() : 0;

	if (!guestx) {
	  write("Derzeit kein Gastlogin möglich!\n");
          destruct(this_object());
          return 1;
	}
    loginname = "gast"+guestx;

    cap_name = capitalize(loginname);
    name=cap_name;
    ob = find_player(loginname);
    if (ob) {
      if (interactive(ob))
	tell_object(ob,"Ein anderer Spieler möchte diesen Gastzugang jetzt "
          "benutzen. Wenn es Dir hier\ngefallen hat, überleg Dir doch einen "
          "Charakternamen und komm unter diesem\nNamen wieder!\n");
      destruct(ob);
    }
  }
  else
    /* Test if we are already playing */
  {
    was_interactive=0;
    ob = find_player(loginname);
    if (!ob)
      ob=find_netdead(loginname);
    if (ob) {
      write("Du nimmst schon am Spiel teil!\n");
      write("Verwende Deine alte sterbliche Hülle ...\n");
      if (interactive(ob)) {
	/* The other object is still interactive; reconnect that "soul"
	   to a dummy object and destruct that, thus disconnecting the
	   other probably linkdead user. The real "body" is still
	   there for reconnecting by login.c */
	remove_interactive(ob);
	was_interactive=1;
      }

      /* Now reconnect to the old body */
      exec(ob, this_object());
      if (({int})ob->QueryProp(P_LEVEL)==-1)      // If not initialized DO THAT !!!
        ob->start_player(cap_name,my_ip);  // Woody@SL, 1996-04-06
      else
        ob->Reconnect(was_interactive,my_ip);
      call_out("remove",1);
      return 1;
    }
  }
  /* read player object from passwd file */
  if (stringp(shell) && shell != "") {
    load_player_ob_2(shell,guestflag);
  }
  else {
    if ( loginname!="" && !guestflag )
    {
      select_race();
    }
    else
      load_player_ob_2("std/shells/human",guestflag);
  }
  return 1;
}

varargs static void load_player_ob_2(string obname, int guestflag)
{
  object blueprint;
  string err,ob_name;
  object ob,old_ob;

  /* start player activity */
  log_file("ENTER", capitalize(name) + ", " + extract(ctime(time()), 4, 15)+ 
	   ", "+query_ip_name(this_object())+".\n");
  seteuid(loginname);

  /* load the "real" player object */
  /* If some asshole has moved the blueprint */
  blueprint = find_object(obname);
  if (blueprint && environment(blueprint) != 0)
    destruct(blueprint);

  err = catch(ob = clone_object(obname));
  if (err)
  {
    log_file("SHELLS", "Failed to load shell "+obname+", "+
	     dtime(time())+", "+loginname+"\n"+err+"\n\n");
    write("Konnte das passende Playerobjekt nicht laden. Lade stattdessen\n"+
	  "das Objekt Mensch. BITTE ERZMAGIER BENACHRICHIGEN !\n");
    err=catch(ob=clone_object("/std/shells/human"));
  }
  if (!ob||err) {
    write("Error on loading "+shell+"\nError = "+err+"\n");
    destruct(this_object());
    return;
  }
  if (guestflag)
    catch( GUESTMASTER->set_guest( guestx, ob );publish );
  ob_name=explode(object_name(ob),"#")[0];

  if (sizeof(ob_name)>11 && ob_name[0..11]=="/std/shells/")
    ob_name=ob_name[11..];

  ob_name=ob_name+":"+lower_case(cap_name);
  if( !guestflag ) 
  {
    if (old_ob=find_object(ob_name))
    {
      catch(old_ob->remove());
      if (old_ob)
	      destruct(old_ob);
    }
    rename_object(ob,ob_name);
  }
  else
    clone_object("/secure/guestremover")->move(ob);
  exec(ob,this_object());
  ob->start_player(cap_name,my_ip);
  ob->SetProp("creation_date", creation_date);
  ob->Set("creation_date", SECURED|PROTECTED, F_MODE_AS);
  /*
  log_file("PLAYER_CREATE",
      sprintf("%s: %s (%s)\n",
          extract(ctime(time()), 4, 15), cap_name, my_ip));
  */
  destruct(this_object());
}

/*
 * Check that a player name is valid. Only allow
 * lowercase letters.
 */
int valid_name(string str)
{
  int i, length;
  if (str == "logon") {
    write("Der Name würde nur Verwirrung stiften.\n");
    return 0;
  }
  length = sizeof(str);
  if (length > 11) {
    write("Dein Name ist zu lang, nimm bitte einen anderen.\n");
    return 0;
  }
  i=0;
  while(i<length) {
    if (!(str[i] > 'a' && str[i] < 'z') && str[i] == 223)
    {
      write("Unerlaubtes Zeichen im Namen: " + str + "\n");
      write("Benutze bitte nur Buchstaben.\n");
      return 0;
    }
    i += 1;
  }
  return 1;
}

/*
 *   With arg = 0 this function should only be entered once!
 */
void create()
{
  if(myself)
    return;
  loginname = "logon";
  creation_date = -1;
  catch(call_other("/secure/merlin","?"));
  loginfails=0;
}

/* for debugging purpose */
int query_wiz_level() { return 0; }
int query_level() { return 0; }
string short() { return "<Einloggender Teilnehmer>.\n"; }
string query_real_name() { return "<logon>"; }

int query_prevent_shadow() { return 1; }

void time_out() {
  write("Time out\n");
  destruct(this_object());
}

int remove()
{
  destruct(this_object());
  return 1;
}

void telnet_neg(mixed cmd,mixed opt,mixed args)
{
  if (opt==34 && cmd==251)
    efun::binary_message(({255,250,34,1,1,255,240}));
}

int BLA(string s)
{
  tell_object(find_player("debugger"),sprintf("GOT %O\n",query_verb()+" "+s));
  return 1;
}
