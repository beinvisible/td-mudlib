// SilberLand Mudlib (Basis AnderLand/MorgenGrauen Mudlib)
//
// std/player/viewcmd.c -- player view command handling
//
// 2024/08/18 - Karte fuer Alle - Aloy
//
// 2024/05/23 - schau kurz vom MG - Aloy
//
// $Date: 1998/02/12 15:44:11 $
/* $Revision: 3.10 $
 * $Log: viewcmd.c,v $
 * Revision 3.10  1998/02/12 15:44:11  Woody
 * Text, auch wenn man nichts bei sich traegt
 *
 * Revision 3.9  1998/01/24 20:38:25  Feyaria
 * Ausgeben von BLIND_MSG
 *
 * Revision 3.8  1997/10/31 12:15:05  Feyaria
 * i +t fuer Tools, bug bei i +r +v gefixt
 *
 * Revision 3.7  1997/10/08 13:42:21  Woody
 * Befehl 'ausruestung' eingebaut
 *
 * Revision 3.6  1997/09/30 21:24:42  Woody
 * break_string() beim Aufzaehlen der Gegenstaende verwenden
 *
 * Revision 3.5  1997/09/30 13:29:32  Woody
 * Eintragen ins log/ARCH/LOOK etwas entschaerft
 *
 * Revision 3.4  1997/09/27 23:58:26  Woody
 * Bugfix fuer "unt obj in spieler an spieler", anderes Handling,
 * wenn mehrere Objekte matchen ("alles", "waffen", etc.) -> Aufzaehlung
 *
 * Revision 3.3  1997/09/19 23:07:59  Woody
 * Bug bei i +b nun endgueltig behoben - uid muss uebereinstimmen
 *
 * Revision 3.2  1997/06/28 14:47:48  Woody
 * Nur mehr interactive's loggen in LOOK mit
 *
 * Revision 3.1  1997/06/25 12:06:42  Woody
 * bei 'unt' auch auf 'x in y' parsen,
 * LOOK log fuer unt-scripts
 *
 * Revision 3.0  1997/03/13 01:50:42  Woody
 * prepared for revision control
 *
 * Revision 2.5  1997/02/27  Woody
 *  - P_BRIEF, Befehl 'kampf'
 *
 * Revision 2.4  1996/10/12  Woody
 *  - 'unt alles' (oder unt waffen oder unt ruestungen etc.) moeglich
 *  - Keine seltsame i Ausgabe mehr, wenn man *nur* unsichtbare Objekte hat
 *
 * Revision 2.3  1996/10/06  Woody
 *  - bei inv +w oder +r werden jetzt auch Tools nicht mehr angezeigt
 *  - bei inv -f wird der fuehrende space weggelassen
 *  - da die inv -f Ausgabe jetzt identisch zu 'alti' ist, wurde
 *    'alti' nun endgueltig deaktiviert.
 *
 * Revision 2.2  1996/06/11  23:50:00  Woody
 *  - inv Befehl hat jetzt auch eine eigene Sparte "Tools" fuer Magier
 *
 * Revision 2.1  1996/05/25  17:00:00  Woody
 *  - clone-Abfrage in create()
 *  - inv Befehl aufgemoebelt
 *
 * Revision 2.0  1996/04/17  02:05:00  Woody
 * _examine() fuer Details an Objekten angepasst
 *
 * Revision 1.0  1996/02/13  20:00:00  Hadra
 * Alte Logs ausgeraeumt
 * Neue Optionen fuer inventory: -w -r und -k
 */                            

#pragma strong_types

#define NEED_PROTOTYPES

#include "/sys/thing/properties.h"
#include "/sys/living/put_and_get.h"
#include "/sys/player/temperatur.h"
#include "/secure/questmaster.h"

#include <container.h>
#include <properties.h>
#include <rooms.h>
#include <wizlevels.h>
#include <defines.h>
#include <new_skills.h>
#include <ansi.h>

#define DEBUG(x)
// #define DEBUG(x) if(find_player("woody")) tell_object(find_player("woody"),x)

varargs mixed More(string str, int fflag, string returnto);
object plan;

void create()
{
  if (!is_clone(this_object())) return;
  Set(P_BRIEF, SAVE, F_MODE);
  Set(P_BLIND, SAVE, F_MODE);
}


varargs int CannotSee(int silent) 
{
  mixed is_blind;

  if ( is_blind = QueryProp( P_BLIND ) ) 
  {
    if (!silent) 
    {
      if (stringp(is_blind))
        write(is_blind);
      else if( mappingp(is_blind) )
        write(is_blind[BLIND_MSG]);
      else
        write( "Du bist blind !\n" );
    }
    return 1;
  }
  if (UseSkill(SK_NIGHTVISION)<0.5
	  && environment()
    && set_light(0)<0.5
	  && (!IS_LEARNER(this_object()) || !Query(P_WANTS_TO_LEARN))
  ) {
    if (!silent)
      write( "Es ist zu dunkel !\n" );
    return 1;
  }
  return 0;
}

int _toggle_brief()
{
  int brief;

  if (query_verb()=="kurz")
    brief=1;
  else if (query_verb()=="kürzer" || query_verb()=="ultrakurz" || query_verb()=="kuerzer")
    brief=2;
  else brief=0;
  SetProp(P_BRIEF, (QueryProp(P_BRIEF) & ~BRIEF_VMASK) | brief);
  write("Du bist nun im \""+
	(brief?(brief==1?"Kurz":"Ultrakurz"):"Lang")+"\"modus.\n");
  return 1;
}

int _set_fbrief(string str)
{
  int brief, f, v;
  string s1, s2;

  brief=QueryProp(P_BRIEF) & BRIEF_FMASK;
  if (!str) {
    write("Aktuelle Einstellungen der Kampfanzeige:\n");
    write("Angriffsmeldungen werden "+
        ((brief & BRIEF_FNOATTACK) ? "nicht " : "")+
        "angezeigt, Schadensmeldungen "+
        ((brief & BRIEF_FNODAMAGE)
         ? (brief & BRIEF_FNOATTACK ? "auch " : "")+"nicht.\n"
         : (brief & BRIEF_FNOATTACK ? "schon.\n" : "auch.\n")));
  }
  else
  {
    notify_fail("Syntax: kampf <angriff|schaden> <ein|aus>\n");
    if (sscanf(str, "%s %s", s1, s2) != 2) return 0;
    if (!member( (["ein", "an", "aus"]), s2 )) return 0;
    f = ([ "angriff" : BRIEF_FNOATTACK, "schaden" : BRIEF_FNODAMAGE ]) [s1];
    if (!f) return 0;
    v = ([ "ein": 2, "an": 2, "aus": 1 ]) [s2];
    if (!(v--)) return 0;
    if (v)
      SetProp (P_BRIEF, QueryProp(P_BRIEF) & ~f);
    else
      SetProp (P_BRIEF, QueryProp(P_BRIEF) | f);
    write ("Ok, "+capitalize(s1)+"smeldungen "+s2+".\n");
  }
  return 1;
}

static int _is_tool(object ob)
{
  return strstr(object_name(ob), "/tools/")!=-1;
}

static int _check_keep(object ob)
{
  return (ob->QueryProp(P_KEEP_ON_SELL))==geteuid(ME);
}

private int sortinv(mixed a, mixed b) { return a[0] > b[0]; }

private varargs string collectinv(mixed obj, int nospace)
{
  if(obj[0]=="") return 0;
  return (!nospace && obj[2] ? " " : "")
       + obj[0] + (obj[1] > 1 ? " ("+obj[1]+")" : "");
}

#define I_AUTOLOAD      1
#define I_KEEP          4
#define I_FORMATTED     16
#define I_ARMOUR        64
#define I_SORT          256
#define I_WEAPON        1024
#define I_FORCE_SORT    4096
#define I_TOOLS        16384
#define I_MISC         65536

private string getflags(string arg, int flags)
{
  int no, i;
  if(sizeof(arg) < 2) return 0;
  no = (arg[0] == '-');

  for(i = 1; i < sizeof(arg); i++)
  {
    switch(arg[i])
    {
    case 'a': flags |= I_AUTOLOAD << no; break;
    case 'b': flags |= I_KEEP << no; break;
    case 'f': flags |= I_FORMATTED << no; break;
    case 'r': flags |= I_ARMOUR << no; break;
    case 's': flags |= I_SORT << no; break;
    case 't': flags |= I_TOOLS << no; break;
    case 'v': flags |= I_MISC << no; break;
    case 'w': flags |= I_WEAPON << no; break;
    case 'S': flags |= I_FORCE_SORT << no; break;
    case '+': no = 0; break;
    case '-': no = 1; break;
    default : return arg[i..i];
    }
  }
  return 0;
}

int _inventory(string str)
{
  mixed *args, output;
  int ansi, i, flags, minv, col, c;
  mixed s, weapons, armours, misc, tools;

  if(CannotSee()) return 1;

  if((str = _unparsed_args()) && str!="")
  {
    string error;
    error = "Benutzung: i[nventar] [-/+abfrstvw]\n";
    args = regexp(regexplode(str, "[-+][abfrstwvS][abfrstwvS]*"),
                  "[-+][abfrstwvS][abfrstwvS]*");
    if(!sizeof(args)) return (_notify_fail(error), 0);
    if(sizeof(args = map(args, #'getflags/*'*/, &flags) - ({ 0 })))
    {
      printf("%s: Unbekanntes Argument.\n"+error, implode(args, ", "));
      return 1;
    }
  }

  if (!sizeof(s=all_inventory(ME)))
  {
    write("Du trägst derzeit nichts bei Dir.\n");
    return 1;
  }
  ansi = member(({"vt100", "ansi"}), ME->QueryProp(P_TTY)) != -1;
  minv = 1 | (flags & (I_FORMATTED | (I_FORMATTED << 1)) ? 2 : 0);
  minv |= QueryProp(P_SHORTINV) ? 0 : 2;
  col = QueryProp(P_COLUMN_MODE);

  if(flags & (I_FORCE_SORT | I_FORCE_SORT << 1))
  {
    closure sf;
    sf = flags & I_FORCE_SORT ? #'> : #'<;
    s = sort_array(s, lambda(({'a, 'b}),
                             ({#'funcall, sf,
                                   ({#'||,({#'call_other,'a,"short"}),""}),
                                   ({#'||,({#'call_other,'b,"short"}),""})})));
    map_objects(s, "move", this_object());
    s = all_inventory(ME);
  }

  if (flags & I_AUTOLOAD)
    s = filter_objects(all_inventory(ME),"QueryProp",P_AUTOLOADOBJ);
  else if (flags & (I_AUTOLOAD << 1))
    s -= filter_objects(s,"QueryProp",P_AUTOLOADOBJ);

  if(flags & I_KEEP)
    s = filter(s, #'_check_keep);
  else if(flags & (I_KEEP << 1))
    s -= filter(s, #'_check_keep);

  weapons = filter_objects(s, "QueryProp", P_WEAPON_TYPE);
  armours = filter_objects(s, "QueryProp", P_ARMOUR_TYPE);
  tools = (!IS_LEARNER(this_object()) ? ({ }) : filter(s, #'_is_tool) );
  misc = s - weapons - armours - tools;

  if(flags & I_WEAPON)
  {
    s = weapons;
    if(!(flags & (I_MISC))) misc = ({});
    if(!(flags & (I_ARMOUR))) armours = ({});
    if(!(flags & (I_TOOLS))) tools = ({});
  }
  if(flags & I_ARMOUR)
  {
     s = armours;
     if(!(flags & (I_MISC))) misc = ({});
     if(!(flags & I_WEAPON)) weapons = ({});
     if(!(flags & I_TOOLS)) tools = ({});
  }
  if(flags & I_TOOLS)
  {
    s = tools;
    if(!(flags & (I_MISC))) misc = ({});
    if(!(flags & I_WEAPON)) weapons = ({});
    if(!(flags & I_ARMOUR)) armours = ({});
  }
  if(flags & I_MISC)
  {
    s = misc;
    if(!(flags & (I_TOOLS))) tools = ({});
    if(!(flags & I_WEAPON)) weapons = ({});
    if(!(flags & I_ARMOUR)) armours = ({});
  }
  if(flags & (I_WEAPON << 1)) { weapons = ({}); s = armours + misc + tools; }
  if(flags & (I_ARMOUR << 1)) { armours = ({}); s = weapons + misc + tools; }
  if(flags & (I_TOOLS << 1))  { tools = ({}); s = weapons +armours + misc; }
  if(flags & (I_MISC << 1))   { misc = ({}); s = weapons +armours + tools; }

  output = "";
  if(flags & (I_FORMATTED | (I_FORMATTED << 1)))
  {
    s = make_invlist(this_player(), s, minv);
    // tell_object(find_player("woody"),sprintf("minv: %d\n%O\n",minv,s));
    if(flags & (I_SORT | (I_SORT << 1)))
      s = sort_array(s, #'sortinv/*'*/);
    // tell_object(find_player("woody"),sprintf("%O\n",s));
    if ((c=col)==2) c=sizeof(s) > 6;
    output += sprintf("%"+(c ? "#" : "=")+"-78s\n",
      implode(map(s,#'collectinv/*'*/, 1),"\n"));
  }
  else
  {
    if(weapons && sizeof(weapons))
    {
      weapons = make_invlist(this_player(), weapons, minv);
      if(flags & (I_SORT | (I_SORT << 1)))
        weapons = sort_array(weapons, #'sortinv/*'*/);
      if ((c=col)==2) c=sizeof(weapons) > 6;
      weapons=implode(map(weapons, #'collectinv/*'*/), "\n");
      if (weapons != "")
        output += (ansi?ANSI_BOLD:"") + "Waffen:" + (ansi?ANSI_NORMAL:"")+"\n"
                + sprintf("%"+(c ? "#" : "=")+"-78s\n", weapons);
    }
    if(armours && sizeof(armours))
    {
      armours = make_invlist(this_player(), armours, minv);
      if(flags & (I_SORT | (I_SORT << 1)))
        armours = sort_array(armours, #'sortinv/*'*/);
      if ((c=col)==2) c=sizeof(armours) > 6;
      armours=implode(map(armours, #'collectinv/*'*/), "\n");
      if (armours != "")
        output += (ansi?ANSI_BOLD:"") + "Rüstungen:" + (ansi?ANSI_NORMAL:"")+"\n"
                + sprintf("%"+(c ? "#" : "=")+"-78s\n", armours);
    }
    if(tools && sizeof(tools))
    {
      tools = make_invlist(this_player(), tools, minv);
      if(flags & (I_SORT | (I_SORT << 1)))
        tools = sort_array(tools, #'sortinv/*'*/);
      if ((c=col)==2) c=sizeof(tools) > 6;
      tools=implode(map(tools, #'collectinv/*'*/), "\n");
      if (tools != "")
        output += (ansi?ANSI_BOLD:"") + "Tools:" + (ansi?ANSI_NORMAL:"")+"\n"
                + sprintf("%"+(c ? "#" : "=")+"-78s\n", tools);
    }
    if(misc && sizeof(misc))
    {
      misc = make_invlist(this_player(), misc, minv);
      // tell_object(find_player("woody"),sprintf("minv: %d\n%O\n",minv,misc));
      if(flags & (I_SORT | (I_SORT << 1)))
        misc = sort_array(misc, #'sortinv/*'*/);
      if ((c=col)==2) c=sizeof(misc) > 6;
      misc=implode(map(misc, #'collectinv/*'*/), "\n");
      if (misc != "")
        output += (ansi?ANSI_BOLD:"") + "Verschiedenes:" + (ansi?ANSI_NORMAL:"")+"\n"
                + sprintf("%"+(c ? "#" : "=")+"-78s\n", misc);
    }
  }

  if (!sizeof(output))
    write("Davon trägst Du derzeit nichts bei Dir.\n");
  else
    More(output);
  return 1;
}

#define V_ARMOURLIST ({ AT_ARMOUR, AT_HELMET, AT_AMULET, AT_RING, AT_GLOVE, \
                        AT_CLOAK, AT_BOOT, AT_TROUSERS, AT_BELT, AT_BRACERS, AT_SHIELD, AT_MISC })

#define V_ARMOURTEXT ([ AT_ARMOUR:   "Am Körper getragen:        ", \
                        AT_HELMET:   "Auf dem Kopf:              ", \
                        AT_AMULET:   "Am Hals getragen:          ", \
                        AT_RING:     "An den Finger gesteckt:    ", \
                        AT_GLOVE:    "An den Händen getragen:    ", \
                        AT_CLOAK:    "Um den Körper gehängt:     ", \
                        AT_BOOT:     "An den Füßen getragen:     ", \
                        AT_TROUSERS: "An den Beinen getragen:    ", \
                        AT_BELT:     "Um die Taille getragen:    ", \
                        AT_BRACERS:  "An den Armen getragen:     ", \
                        AT_SHIELD:   "Als Schutz gehalten:       ", \
                        AT_MISC:     "Sonstiges:                 " ])

static int _armours(string str)
{
  mixed s, out, t, line;
  mapping used;
  int ansi, i, j;

  s = ME->QueryProp(P_ARMOURS) - ({0});
  used=([]);
  for (i=sizeof(s)-1; i>=0; i--)
    if (s[i]->QueryProp(P_SHORT) && !s[i]->QueryProp(P_INVIS)) {
      t=s[i]->QueryProp(P_ARMOUR_TYPE);
      if (!pointerp(used[t])) used[t] = ({ s[i] });
      else used[t] += ({ s[i] });
    }
  ansi = member(({"vt100", "ansi"}), ME->QueryProp(P_TTY)) != -1;
  out = "";

  out = (ansi ? ANSI_BOLD : "") + "Ausrüstung:" +
        (ansi ? ANSI_NORMAL : "") + "\n";

  for (i=0; i<sizeof(V_ARMOURLIST); i++)
  {
    t=V_ARMOURLIST[i];
    line=({});
    if (!pointerp(used[t]) || !sizeof(used[t]))
      line=({"-"});
    else
      for (j=0; j<sizeof(used[t]); j++) {
        s=used[t][j]->dam_desc();
        line += ({ used[t][j]->name()+ (s ? " ("+s+"er Zustand)" : "") });
      }
    s = (sizeof(line) >= 2 ?
          implode(line[0..<2], ", ")+" und "+line[<1] : line[0]);
    s = break_string(capitalize(s), 78, 29)[29..];
    out += "  " + V_ARMOURTEXT[t] + s;
  }

  if (s=QueryProp(P_WEAPON)) {
    if (s->QueryProp(P_SHORT) && !s->QueryProp(P_INVIS)) {
      i=s->QueryProp(P_NR_HANDS);
      t=s->dam_desc();
      out += sprintf("  Mit %-22s %s%s\n",
        (i==1 ? "einer Hand" :
         i==2 ? "zwei Händen" : "mehreren Händen")+" gezückt:",
        s->Name(), t ? " ("+t+"er Zustand)" : "");
    }
  }

  More(out);
  return 1;
}

void smart_log(string myname, string str);

/* Gebe die Umgebung des aktiven Spielers zurueck, lasse dabei  */
/* rekursiv geschachtelte Raeume zu.                            */
/* Wenn allow_short 0 ist, so wird immer die long-descr benutzt */
varargs string env_descr(int allow_short,int flags, int force_short )
{
  object env;

  env = environment(ME);

  if(!env)
    return "Du schwebst im Nichts ... Du siehst nichts, rein gar nichts ...\n";

  if (!force_short && (!allow_short || !QueryProp(P_BRIEF)))
      return env->int_long(ME,ME,flags);

  if (!flags && QueryProp(P_BRIEF)>=2)
      return "";

  return env->int_short(ME,ME);
}

private static int exa_cnt;
private static int exa_time;
private static string *exa;

varargs int _examine(string str, int mode) 
{
  object base, env;
  mixed *objs;
  string what, detail, parent, out, error;
  int i, size, done;
#ifdef EXAMINE_MULTIPLE
  int first;
#endif

  if(CannotSee()) return 1;

  _notify_fail("Was willst Du denn untersuchen?\n");
  if (!str) return 0;

  if (interactive(this_object()) &&
      member(({"boden","decke","wand","waende", "wände"}),explode(str," ")[0]) == -1) {
    exa_cnt -= (time() - exa_time);
    exa_time = time();
    exa_cnt++;
    if (!exa)
      exa = ({ str });
    else
      exa += ({ str });
    if (exa_cnt > 10) {
      log_file("ARCH/LOOK", sprintf("%s: %s in %s\n%@O\n",
               dtime(time()), getuid(this_object()),
               environment() ? object_name(environment()) : "???",exa));
      exa_cnt = 0;
      exa = ({});
    }
    else if (exa_cnt < 0) {
      exa_cnt = 0;
      exa = ({});
    }
  }

  // do we look at an object in our environment ?
  // "im raum" -> "in raum" (erledigt schon der Parser)
  if (sscanf(str,"%s in raum", what))
    base = environment();
  else
   // is the object inside of me (inventory)
   if (sscanf(str,"%s in mir", what) || sscanf(str,"%s in dir", what))
     base = this_object();
   else 
   {
     what = str;
     // get the last object we looked at
     base = QueryProp(P_REFERENCE_OBJECT);

     // if a reference object exists, test for its existance in the room
     // or in our inventory
     if(base)
       if(member(deep_inventory(environment()), base) != -1)
       {
         env = base;
         // check if base is inside of a living or non-transparent object
         while((env = environment(env)) && 
               !living(env) && env->QueryProp(P_TRANSPARENT)) 
           /* do nothing */;
         if(env && env != this_object() && env != environment()) base = 0;
       }
       else base = 0;
   }

  // scan input if we want a specific object to look at
  if(sscanf(what, "%s in %s", detail, parent) == 2 ||
     sscanf(what, "%s im %s", detail, parent) == 2 ||
     sscanf(what, "%s an %s", detail, parent) == 2 ||
     sscanf(what, "%s am %s", detail, parent) == 2)
  {
    DEBUG(sprintf("---detail: %O\n",detail));
    DEBUG(sprintf("---parent: %O\n",parent));
    // if an ref object exists get its inventory, only if it is not a living
    if (base && ((base==this_object()) || !living(base)))
      objs = base->locate_objects(parent, 1);
    else
      // else get our inv and env
      objs = environment()->locate_objects(parent, 1)
           + this_object()->locate_objects(parent, 1);
    if(sizeof(objs) > 1)
      return (notify_fail("Es gibt mehr als ein "+capitalize(parent)+".\n"), 0);
    else 
      if(sizeof(objs)) base = objs[0];
      else return (notify_fail("Hier ist kein "+capitalize(parent)+".\n"), 0);
    objs = 0;
  }
  else detail = what;

  do {
    DEBUG(sprintf("---base:   %O (parent: %O)\n",base,parent));
    DEBUG(sprintf("---detail: %O\n",detail));
    // if a base exists get its inventory, else get our inv and env
    if(base)
      if(base == this_object() || base == environment() ||
         (base->QueryProp(P_TRANSPARENT) && !living(base)))
        objs = base->locate_objects(detail, 1);
      else ;
    else
    {
      objs = environment()->locate_objects(detail, 1)
           + this_object()->locate_objects(detail, 1);
      base = environment();
    }

    if(!objs || !sizeof(objs))
      if((out = base->GetDetail(detail, QueryProp(P_RACE))) ||
         (out = base->GetDoorDesc(detail)))
      {
        SetProp(P_REFERENCE_OBJECT, base);
        return (write(out), 1);
      }
      else
        if(base == environment())
          return (_notify_fail((error?error:"")+"Sowas siehst Du "+
                               (error?"auch ":"")+"da nicht!\n"), 0);
        else
        {
          if (sizeof(base->locate_objects(detail,1)))
            error = "So nah kommst Du an "+base->name(WEN)+" nicht ran!\n";
          else
            error = "Du findest an "+base->name(WEM)+" kein \""
                  + capitalize(detail)+"\".\n";
          error += "Dein Blick wendet sich der Umgebung zu.\n";
          base = 0;
        }
    else done = 1;
  } while(!done);

  // Examine all found objects
  _notify_fail("Sowas siehst Du nicht.\n");
#ifdef EXAMINE_MULTIPLE
  size = sizeof(objs);
  first=done=0;
  out="";
  for (i = 0; i < size; i++)
    // if ((objs[i]->short()) && (objs[i]->short() != ""))	// evtl.?
    if (objs[i]->short())
    {
      if (done)
        out+="Dein Blick wendet sich weiters "+objs[i]->name(WEM)+" zu:\n";
      out+=objs[i]->long(str);
      SetProp(P_REFERENCE_OBJECT, objs[i]);
      // return (write(objs[i]->long(str)), 1);
      if (!first) first=i+1;
      done=i+1;
    }
  if (first!=done)
    out="Du erblickst zunächst "+objs[first-1]->name(WEN)+":\n"+out;
  if (done) write (out);
  return done;
#else
  objs=filter_objects(objs, "short");
  if (!size=sizeof(objs)) return 0;
  if (size==1)
  {
    SetProp(P_REFERENCE_OBJECT, objs[0]);
    write(objs[0]->long(str));
  }
  else
  {
    SetProp(P_REFERENCE_OBJECT, 0);
    objs=map_objects(objs, "name", WEN);
    out=implode(objs[0..<2], ", ")+" und "+objs[<1];
    write(break_string("Du erblickst "+out+".", 78));
  }
  return 1;
#endif
}

varargs int look_into(string str,int mode)
{
  object *found_obs;

  if( CannotSee() ) return 1;
  _notify_fail("Wo willst Du denn reinschauen ?\n");
  found_obs=select_objects(str,1);
  if (!found_obs)
  {
    if (environment() && 
	(environment()->GetDetail(str,QueryProp(P_RACE))||
	 environment()->GetDoorDesc(str)))
      _notify_fail("Da kannst Du so nicht reinsehen.\n");
    return 0;
  }
  return _examine(str, mode);
}

int _look(string str)
{
  string s;

  if(CannotSee()) return 1;
  
  if(set_light() < 1.0)
  {
    SetProp(P_REFERENCE_OBJECT, 0);
    write( env_descr(1,2,1) );
    return 1;
  }
  
  if(!str)
  {
    SetProp (P_REFERENCE_OBJECT, 0);
    write( env_descr() );
    return 1;
  }
  if(str=="-k" || str=="kurz")
  {
    SetProp(P_REFERENCE_OBJECT, 0);
    write( env_descr(1,2,1) );
    return 1;
  }
  if (sscanf(str,"%s an",s)) str=s;
  if (sscanf(str,"%s in mir",s)||sscanf(str,"%s in dir",s)) return _examine(str);
  if (sscanf(str,"in %s",s))
    return look_into(s);
  return _examine(str);
}

int _look_short(string str)
{
  string s;

  if(CannotSee()) return 1;
  
  if(!str)
  {
    SetProp (P_REFERENCE_OBJECT, 0);
    write( env_descr(1,2,1) );
    return 1;
  }

  if (sscanf(str,"%s an",s)) str=s;
  if (sscanf(str,"%s in mir",s)||sscanf(str,"%s in dir",s)) return _examine(str);
  if (sscanf(str,"in %s",s))
    return look_into(s);
  return _examine(str);
}

static string asText( int i )
{
  switch( i )
  {
    case 0: return "kann keines";
    case 1: return "kann eines";
    default:
      return sprintf( "können %d", i );
  }
}

int compare( mixed *i, mixed *j )
{
  if( i[2] == j[2] )
    return i[1] > j[1];
  else
    return i[2] > j[2];
}

int _liste()
{
  mixed pl = this_player();
  int qgroups, i, j, n, *dontneed;
  mixed *qlists, *quests;
  string str, bold, normal;

  if (!this_player()) return 0;

  if(!objectp(pl))
    if(stringp(pl))
      pl=find_player(pl)||find_netdead(pl);
  if(!objectp(pl))
    pl=PL;

  if ((this_player()->QueryProp(P_TTY) == "ansi") ||
      (this_player()->QueryProp(P_TTY) == "vt100"))
    bold=ANSI_BOLD, normal=ANSI_NORMAL;
  else
    bold=normal="";

  /*
  str = sprintf("                        Liste der Abenteuer: %s\n\n",
      (pl != PL ? "("+capitalize(getuid(pl))+")" : ""));
  str += sprintf("  %38s  AP  Stufe  geloest  verantw. Magier\n","");
  */
  str = sprintf("%s%-38s%s    AP  Stufe  gelöst  verantw. Magier\n",
      bold,"Liste der Abenteuer"+
      (pl != PL ? "("+capitalize(getuid(pl))+")" : "")+":", normal);
  for( i=0; i<39; i++, str+="--" );
  str += "\n";
  qgroups = sizeof(dontneed=QM->QueryDontneed(pl));
  qlists = allocate( qgroups );
  for( i=qgroups-1; i>=0; i-- )
    qlists[i] = ({});

  quests = QM->QueryQuests();
  for( i=sizeof(quests[0])-1; i>=0; i-- )
    if( quests[1][i][6] )
    {
      for( j=qgroups-1; QGROUPS[j]>quests[1][i][0]; j-- )
        ;
      qlists[j] += ({ ({
        quests[0][i], quests[1][i][0], quests[1][i][4], quests[1][i][7],
                quests[1][i][5]
      }) });
    }

  for( i=0; i<qgroups; i++ )
  {
    str += bold+"Gruppe "+(i+1)+":"+normal+"\n";
    if (!sizeof(qlists[i]))
      str += "  In dieser Gruppe gibt es derzeit keine Abenteuer.\n";
    else
    {
      qlists[i] = sort_array( qlists[i], #'compare, ME );
      for( j=0; j<sizeof(qlists[i]); j++ )
      {
        if(qlists[i][j][4]==0)
          str +=
            sprintf( "  %-38s %3d   %2d     %-4s    %s\n",
              qlists[i][j][0], qlists[i][j][1], qlists[i][j][2],
              (pl->QueryQuest(qlists[i][j][0]) ? " ja": "nein"),
              capitalize(qlists[i][j][3])
        );
      }
      str += sprintf(
        "  (Von den Abenteuern dieser Gruppe %s ausgelassen werden.)\n",
        asText(dontneed[i]));
    }
    str += "\n";
  }
  str += bold+"Optionale Abenteuer:"+normal+"\n";
  n=0;
  for( i=0; i<qgroups; i++ )
  {
    for( j=0; j<sizeof(qlists[i]); j++ )
    {
      if(qlists[i][j][4]!=0) {
        n++;
        str += sprintf( "  %-38s %3d   %2d     %-4s    %s\n",
                       qlists[i][j][0], qlists[i][j][1], qlists[i][j][2],
                       (pl->QueryQuest(qlists[i][j][0]) ? " ja": "nein"),
                       capitalize(qlists[i][j][3])
                      );
      }
    }
  }
  if (!n)
    str += "  In dieser Gruppe gibt es derzeit keine Abenteuer.\n";

  i=j=pl->QueryProp(P_NEEDED_QP);
  if (QM->QuerySeerQP() > i) i = QM->QuerySeerQP();
  if (QM->QueryWizQP() > j) j = QM->QueryWizQP();
  if (i==j)
    str += sprintf(
      "\nUm "+bold+"Magier oder Seher"+normal+
      " zu werden, musst Du %d der %d Abenteuerpunkte\n"+
      "und %d Erfahrungspunkte erlangen.\n",
      i, QM->QueryMaxQP(), XP_NEEDED_FOR_WIZ);
  else
    str += sprintf(
      "\nUm "+bold+"Seher"+normal+
      " zu werden, musst Du %d der %d Abenteuerpunkte und\n"+
      "%d Erfahrungspunkte erlangen. Um "+bold+"Magier"+normal+
      " zu werden,\nbenötigst Du allerdings %d Abenteuerpunkte.\n",
      i, QM->QueryMaxQP(), XP_NEEDED_FOR_WIZ, j);
  this_player()->More(str, 0);
  return 1;
}

int _skills()
{
  string pp = "|";
  string output;
  string gilde = QueryProp(P_GUILD);
  output = "/==========================================================\\\n";
  output += sprintf(pp+"  %-55s "+pp+"\n","Du bist in folgender Gilde: "+capitalize(gilde));
  output += "+----------------------------------------------------------+\n";
  output += sprintf(pp+"  %-55s "+pp+"\n","Und hast diese Fertigkeiten: ");
  output += "+----------------------------------------------------------+\n";

  mixed *skills = m_indices(QueryProp(P_NEWSKILLS));
  if(sizeof(skills)>0)
  {
    for (int i = 0; i < sizeof(skills); i++)
    {
      output += sprintf(pp+"  %-55s "+pp+"\n",capitalize(({string})skills[i]));
    }
  }
  else
    output += sprintf(pp+"  %-55s "+pp+"\n","Du hast noch keine Fertigkeiten gelernt. ");
  output += "\\==========================================================/\n";
  More(output);
  return 1;
}

int _sehertore()
{
  if(IS_SEER(this_object()))
  {
    "/obj/sehertormaster"->ShowDoors(1);
  }
  else
  {
    write("Nur Seher können das!\n");
  }

  return 1;
}

int _shortpinfo()
{
  string output=sprintf("LP:%d/%d\nMP:%d/%d\n",QueryProp(P_HP),QueryProp(P_MAX_HP),QueryProp(P_SP),QueryProp(P_MAX_SP));
  write(output);

  return 1;
}

int _temperatur()
{
  temp_berechnung();
  minmax_temp_return();
  armour_temp();
  temp_return();

  return 1;
}

static mixed *_query_localcmds()
{
  return
    ({({"i","_inventory",0,0}),
      ({"inv","_inventory",0,0}),
      ({"inventar","_inventory",0,0}),
      ({"ausruestung","_armours",0,0}),
      ({"ausrüstung","_armours",0,0}),
      ({"temperatur","_temperatur",0,0}),
      ({"temp","_temperatur",0,0}),
      ({"kurzinfo","_shortpinfo",0,0}),
      ({"kinfo","_shortpinfo",0,0}),
      ({"questbuch","_liste",0,0}),
      ({"questlog","_liste",0,0}),
      ({"quests","_liste",0,0}),
      ({"skills","_skills",0,0}),
      ({"zauber","_skills",0,0}),
      ({"zauberbuch","_skills",0,0}),
      ({"skillbuch","_skills",0,0}),
      ({"fähigkeiten","_skills",0,0}),
      ({"faehigkeiten","_skills",0,0}),
      ({"fertigkeiten","_skills",0,0}),
      ({"tore","_sehertore",0,0}),
      ({"sehertore","_sehertore",0,0}),
      ({"schau","_look",0,0}),
      ({"schaue","_look",0,0}),
      ({"kschau","_look_short",0,0}),
      ({"kschaue","_look_short",0,0}),
      ({"unt","_examine",0,0}),
      ({"untersuch","_examine",0,0}),
      ({"untersuche","_examine",0,0}),
      ({"betracht","_examine",0,0}),
      ({"betrachte","_examine",0,0}),
      ({"betr","_examine",0,0}),
      ({"kurz","_toggle_brief",0,0}),
      ({"lang","_toggle_brief",0,0}),
      ({"kuerzer","_toggle_brief",0,0}),
      ({"kürzer","_toggle_brief",0,0}),
      ({"ultrakurz","_toggle_brief",0,0}),
      ({"kampf","_set_fbrief",0,0})
    });
}

