// /obj/tools/astralschwert.c   Das Astralschwert
// 15.12.98   Ezo
//
/*
        Folgende Zeilen wurden von Querolin herausgenommen
	168,169,170,181,540,545
        showroom einiges ausgenommen mit //

*/
// Ein nettes Tool zum Beschreiben von Raeumen.
// Voellig neu ueberarbeitete Version 4.0 !!!
// Das Astralschwert darf frei kopiert werden, wenn ein Verweis auf
// den Urheber ezo@turithil bleibt.
//
/* $Date: 2001/01/13 12:59:00 $
 * $Revision: 1.1 $
 * $Log: astralschwert.c,v $
 * Revision 1.1  2001/01/13 12:59:00  Samtpfote
 * Initial revision
 *
 */


//#pragma strong_types

#include <properties.h>
#include <defines.h>
#include <wizlevels.h>
#include <combat.h>
#include <moving.h>
#include "/obj/tools/astralschwert.h"

inherit "std/weapon";

void create() {
  if(!clonep(this_object())) return;
  ::create();
  SetProp(P_AUTOLOADOBJ,1);
  SetProp(P_NAME,"Astralschwert");
  SetProp(P_GENDER,NEUTER);
  SetProp(P_SHORT,"Das geheimnisumwitterte Astralschwert");
  SetProp(P_LONG,"Dieses Schwert ist anders als alle, die Du zuvor gesehen "
    "hast. In einem Moment erscheint Dir seine Existenz fragwürdig, in dem "
    "anderen erscheint es Dir wirklicher als Du selbst. Immer gewisser wird "
    "Dir, dass es nicht von dieser Welt sein kann. Einmal scheint es fast "
    "gänzlich durchsichtig, schimmert wie die Oberfläche einer "
    "Seifenblase oder glänzt, wie wenn es brennendes Chrom wäre. Am Griff "
    "ist ganz deutlich (auch für Daria sichtbar) das Wort 'showhelp' "
    "eingraviert.");
  SetProp(P_ARTICLE,2);
  SetProp(P_NOBUY,1);
  // SetProp(P_NODROP,1);
  SetProp(P_VALUE, 0);
  SetProp(P_WEIGHT, 0);
  SetProp(P_WEAPON_TYPE, WT_SWORD);
  SetProp(P_DAM_TYPE, DT_SLASH);
  SetProp(P_WC, 200);
  SetProp(P_NR_HANDS, 1);
  SetProp(P_INFO,"Das Astralschwert erlaubt Dir die Grundlage der Welt "
    "selbst zu verändern ... soweit du vermagst das Schwert zu "
    "beherrschen... Mit <showhelp> zeigt es Dir seine Befehle an.");
  AddCmd("","astralcommands",1);
  AddId(({"schwert","astralschwert"}));
}

void init() {
  ::init();
  if (!IS_LEARNER(this_player())) call_out("nixda",2);
  else seteuid(getuid(environment()));
}

void nixda() {
  //Falls es einem Magier gelingt, das Astralschwert einem Spieler zu geben:)
  say("So nicht, "+capitalize(geteuid())+"! Das gibt eine Anzeige!\n");
  write(capitalize(geteuid())+" wurde soeben bei Spielerhilfe erwischt.\n");
  remove();
}

// Jeder Befehl wird hier erst einmal ausgewertet:
int astralcommands(string str) {
  string verb;
  int key;
  verb=query_verb();
  if(member(VALID_VERBS,verb)==-1) return 0;
  if(member(OTHER_VERBS,verb)!=-1) {
    call_other(this_object(),verb);
    return 1; }
  if((key=member(SHOW_VERBS,verb))!=-1) {
//    if(key==1) return 0;
    show(key+8);
    return 1; }
  if((key=member(ADD_VERBS,verb))!=-1) {
    if(member(({5,6}),key)!=-1) return 0;
    add1(key+8);
    return 1; }
  if((key=member(REMOVE_VERBS,verb))!=-1) {
    if(key==1) return 0;
    remove1(key+8);
    return 1; }
  if((key=member(EDIT_VERBS,verb))!=-1) {
    edit1(key);
    return 1; }
  return notify_fail("Ausnahmefehler 12cx. Ezo verstaendigen.\n"),0;
}

void showhelp() {
  this_player()->More("\nFolgende Befehle gibt es:\n\n"
    "   Anzeigen   | Einfügen   |   Löschen     | Betrifft:\n"
    " -------------+------------+---------------+----------------\n"
    "  showhelp    |      -     |       -       | Diese Hilfe\n"
    "  showroom    |      -     |       -       | Infos zum Raum\n"
    "       -      | createroom |       -       | Erschafft neuen Raum\n"
    "  showdetail  | adddetail  | removedetail  | Details\n"
    "  shownight   | addnight   |       \"       | NightDetails\n"
    "  showread    | addread    | removeread    | ReadDetails\n"
    "  showspecial |      -     | removespecial | SpecialDetails\n"
    "  showitem    | additem    | removeitem    | Items\n"
    "  showsmell   | addsmell   | removesmell   | Smells\n"
    "  showsound   | addsound   | removesound   | Sounds\n"
    "  showcommand |      -     | removecommand | Commands\n"
    "  showexit    | addexit    | removeexit    | Exits\n"
    "  showfood    | addfood    | removefood    | Foods (nur in Kneipen)\n"
    "  showdrink   | adddrink   | removedrink   | Drinks (nur in Kneipen)\n"
    "\n"
    "     Editieren  | Ändert:                  Editieren  |  Ändert:\n"
    " ---------------+------------------      -------------+--------------\n"
    "  editlong      | Langbeschreibung        editlight   | Lichtlevel\n"
    "  editnightlong | Nacht-Langbeschreibung  editroom    | Raumzustand\n"
    "  editshort     | Kurzbeschreibung        editterrain | Terraintyp\n"
    "  edittemp      | Temperatur              editday     | Tageszustand\n");
}

void showroom() {
  string ausgabe,datei,*zeilen,hion,hioff,inheritfile;
  int i;
  mixed *dummy;
  datei=object_name(environment(this_player()))+".c";
  zeilen=explode(read_file(datei),"\n");
  if(this_player()->QueryProp(P_TTY)!="ansi") hion="\e[1m";
  else hion="\e[31m";
  hioff="\e[0m";
  for(i=0;i<sizeof(zeilen);i++)
    if(sscanf(zeilen[i],"inherit \"%s\";",inheritfile)==1) break;
  if(!inheritfile) inheritfile="/std/room";
  call_other(inheritfile,"create");
  
  if(floatp(datei->QueryProp(P_LIGHT)))
  {
    ausgabe=sprintf("\n/---------------------------------------"+
    "----------------------+-------------\\\n"+
    "| Datei: "+hion+"%52s "+hioff+"| %13s "+hioff+"|\n"+
    "+-----------------+---------------------"+
    "+---------------------+-------------+\n"+
    "| Zeilen:    "+hion+"%4d "+hioff+
    "| Größe: "+hion+"%5d Byte "+hioff+
    "| Userid: "+hion+"%25s "+hioff+"|\n"+
    "+-----------------+---------------------"+
    "+-----------------------------------+\n"+
    "| Lichtlevel: "+hion+"%1.1f \n"+hioff+
    // "| Temperatur: "+hion+"%7d "+hioff+
    // "| Tagzustand: "+hion+"%21s "+hioff+"|\n"+
    "+-----------------+---------------------"+
    "+-----------------------------------+\n"+
    // "| Terraintyp: "+hion+"%25s "+hioff+
    "| Raumzustand: "+hion+"%20s "+hioff+"|\n"+
    "%s"+
    "+------------+--------+--------+--------"+
    "+--------+--------+--------+--------+\n"+
    "| T Detail N |   Read |Special |  Smell "+
    "|  Sound |   Item |Command |   Exit |\n"+
    // "|"+hion+" %-5d %4d "+hioff+"|"+hion+" %6d "+hioff+  NEU SIEHE UNTEN
    "|"+hion+" %-5d      "+hioff+"|"+hion+" %6d "+hioff+
    "|"+hion+" %6d "+hioff+"|"+hion+" %6d "+hioff+
    "|"+hion+" %6d "+hioff+"|"+hion+" %6d "+hioff+
    "|"+hion+" %6d "+hioff+"|"+hion+" %6d "+hioff+"|\n"+
    "\\------------+--------+--------+--------"+
    "+--------+--------+--------+--------/\n",
    datei,
    (this_player()->QueryProp("tty")=="ansi"?"\e[34m":"\e[1m")+"Version 4.0",
    sizeof(zeilen),
    file_size(datei),
    capitalize(getuid(find_object(datei))),
    datei->QueryProp(P_LIGHT),
    // datei->QueryProp(P_TEMPERATURE),
    // DAY_TYPES[datei->QueryProp(P_ISDAY_OVERRIDE)],
    // ((i=datei->QueryProp(P_TERRAIN))>200?TERRAIN_TYPES_OUT[i-201]:
    // (i>100?TERRAIN_TYPES_IN[i-101]:"unbestimmt")),
    INDOOR_TYPES[datei->QueryProp(P_INDOORS)],
    (member(datei->QueryProp(P_COMMANDS),"menü")?
    sprintf("+---------------------------------------"+
      "+-----------------------------------+\n"+
      "| Essen: "+hion+"%30d "+hioff+"| Drinks: "+hion+
      "%25d "+hioff+"|\n",
      sizeof(datei->query_food()),
      sizeof(datei->query_drink())):""),
    ANZAHL(datei,P_DETAILS)-ANZAHL(inheritfile,P_DETAILS),
    // ANZAHL(datei,P_NIGHT_DETAILS)-ANZAHL(inheritfile,P_NIGHT_DETAILS),
    ANZAHL(datei,P_READ_DETAILS)-ANZAHL(inheritfile,P_READ_DETAILS),
    ANZAHL(datei,P_SPECIAL_DETAILS)-ANZAHL(inheritfile,P_SPECIAL_DETAILS),
    ANZAHL(datei,P_SMELLS)-ANZAHL(inheritfile,P_SMELLS),
    ANZAHL(datei,P_SOUNDS)-ANZAHL(inheritfile,P_SOUNDS),
    sizeof(datei->QueryProp(P_ITEMS))-sizeof(inheritfile->QueryProp(P_ITEMS)),
    ANZAHL(datei,P_COMMANDS)-ANZAHL(inheritfile,P_COMMANDS),
    ANZAHL(datei,P_EXITS)-ANZAHL(inheritfile,P_EXITS));
  }
  
  if(intp(datei->QueryProp(P_LIGHT)))
  {
    ausgabe=sprintf("\n/---------------------------------------"+
    "----------------------+-------------\\\n"+
    "| Datei: "+hion+"%52s "+hioff+"| %13s "+hioff+"|\n"+
    "+-----------------+---------------------"+
    "+---------------------+-------------+\n"+
    "| Zeilen:    "+hion+"%4d "+hioff+
    "| Größe: "+hion+"%5d Byte "+hioff+
    "| Userid: "+hion+"%25s "+hioff+"|\n"+
    "+-----------------+---------------------"+
    "+-----------------------------------+\n"+
    "| Lichtlevel: "+hion+"%3d \n"+hioff+
    // "| Temperatur: "+hion+"%7d "+hioff+
    // "| Tagzustand: "+hion+"%21s "+hioff+"|\n"+
    "+-----------------+---------------------"+
    "+-----------------------------------+\n"+
    // "| Terraintyp: "+hion+"%25s "+hioff+
    "| Raumzustand: "+hion+"%20s "+hioff+"|\n"+
    "%s"+
    "+------------+--------+--------+--------"+
    "+--------+--------+--------+--------+\n"+
    "| T Detail N |   Read |Special |  Smell "+
    "|  Sound |   Item |Command |   Exit |\n"+
    // "|"+hion+" %-5d %4d "+hioff+"|"+hion+" %6d "+hioff+  NEU SIEHE UNTEN
    "|"+hion+" %-5d      "+hioff+"|"+hion+" %6d "+hioff+
    "|"+hion+" %6d "+hioff+"|"+hion+" %6d "+hioff+
    "|"+hion+" %6d "+hioff+"|"+hion+" %6d "+hioff+
    "|"+hion+" %6d "+hioff+"|"+hion+" %6d "+hioff+"|\n"+
    "\\------------+--------+--------+--------"+
    "+--------+--------+--------+--------/\n",
    datei,
    (this_player()->QueryProp("tty")=="ansi"?"\e[34m":"\e[1m")+"Version 4.0",
    sizeof(zeilen),
    file_size(datei),
    capitalize(getuid(find_object(datei))),
    datei->QueryProp(P_LIGHT),
    // datei->QueryProp(P_TEMPERATURE),
    // DAY_TYPES[datei->QueryProp(P_ISDAY_OVERRIDE)],
    // ((i=datei->QueryProp(P_TERRAIN))>200?TERRAIN_TYPES_OUT[i-201]:
    // (i>100?TERRAIN_TYPES_IN[i-101]:"unbestimmt")),
    INDOOR_TYPES[datei->QueryProp(P_INDOORS)],
    (member(datei->QueryProp(P_COMMANDS),"menü")?
    sprintf("+---------------------------------------"+
      "+-----------------------------------+\n"+
      "| Essen: "+hion+"%30d "+hioff+"| Drinks: "+hion+
      "%25d "+hioff+"|\n",
      sizeof(datei->query_food()),
      sizeof(datei->query_drink())):""),
    ANZAHL(datei,P_DETAILS)-ANZAHL(inheritfile,P_DETAILS),
    // ANZAHL(datei,P_NIGHT_DETAILS)-ANZAHL(inheritfile,P_NIGHT_DETAILS),
    ANZAHL(datei,P_READ_DETAILS)-ANZAHL(inheritfile,P_READ_DETAILS),
    ANZAHL(datei,P_SPECIAL_DETAILS)-ANZAHL(inheritfile,P_SPECIAL_DETAILS),
    ANZAHL(datei,P_SMELLS)-ANZAHL(inheritfile,P_SMELLS),
    ANZAHL(datei,P_SOUNDS)-ANZAHL(inheritfile,P_SOUNDS),
    sizeof(datei->QueryProp(P_ITEMS))-sizeof(inheritfile->QueryProp(P_ITEMS)),
    ANZAHL(datei,P_COMMANDS)-ANZAHL(inheritfile,P_COMMANDS),
    ANZAHL(datei,P_EXITS)-ANZAHL(inheritfile,P_EXITS));
  }
  
  write(ausgabe);
}

varargs void createroom(string str,mixed *extra) {
  int i;
  if(str=="q") {
    write("\nAbbruch!\n");
    return; }
  if(file_size(this_player()->QueryProp(P_CURRENTDIR)+"/../def.h")==-1) {
    write("Im Verzeichnis "+this_player()->QueryProp(P_CURRENTDIR)+
          "/.. existiert kein def.h. Abgebrochen!\n");
    return; }
  if(explode(this_player()->QueryProp(P_CURRENTDIR),"/")[<1]!="room") {
    write(break_string("Das Verzeichnis "+
      this_player()->QueryProp(P_CURRENTDIR)+
      " ist kein ../room-Verzeichnis. Abgebrochen!\n"));
    return; }
  if(!extra) {
    if(!str=this_player()->_unparsed_args()) {
      write(STRICHE+"Bitte den Raumnamen eingeben:\n-> ");
      input_to("createroom",0,({}));
      return; } }
  switch(sizeof(extra)) {
    case 0: if(str==""||sizeof(str)<2||sizeof(explode(str," "))>1) {
              write("\nFehlerhafter Raumname. Abbruch!\n");
              return; }
            if(str[<2..]!=".c") str+=".c";
            if(str[0..0]!="/")
              str=this_player()->QueryProp(P_CURRENTDIR)+"/"+str;
            if(file_size(str)!=-1) {
              write("\nDie Datei "+str+" existiert schon! Abbruch!\n");
              return; }
            if(write_file(str,"test")==-1) {
              write("\nSie haben keine Schreibrechte für die Datei "+str+
                ". Abbruch!\n");
              return; }
            write("\n"+STRICHE+"   : normaler Raum\n  b: Bar\n"
              "  l: Laden\n  p: Post\n  t: Tempel\n"+STRICHE+"Welchen Raum "
              "erschaffen? (RETURN/b/l/p/t)\n-> ");
            input_to("createroom",0,({str})); break;
    case 1: extra+=({sprintf("// %s\n// %s   %s\n// Diese Datei wurde 1 "+
              "mal mit dem Astralschwert bearbeitet.\n\n#pragma strong_"+
              "types\n#include <properties.h>\n#include \"../def.h\"\ninher"+
              "it\"/std/%s\";\n\nvoid create() {\n  ::create();\n}\n",
              extra[0],dtime(time()),capitalize(geteuid(this_object())),
              (str=="l"?"laden":(str=="p"?"post":
              (str=="b"?"pub":(str=="t"?"tempel":"room")))))});
            write("\n"+STRICHE+"Neuen Raum "+extra[0]+
              " erschaffen? (j/n)\n-> ");
            write_file(extra[0],"test");
            input_to("savefile",0,extra[0],extra[1],19," neu erschaffen");
            break;
    default: write("\nAusnahmefehler 700987wue. Ezo benachrichtigen.\n"); }
}

void show(int key) {
  string datei,*zeilen,ausgabe;
  int *zeilennummern,i;
  datei=object_name(environment(this_player()))+".c";
  zeilen=explode(read_file(datei),"\n");
  zeilennummern=dateiinfo(datei,zeilen,key);
  if(zeilennummern[0]>zeilennummern[1]) {
    write("\n"+STRICHE+"\nKeine "+TYPES[key]+"s gefunden.\n"+STRICHE);
    return; }
  ausgabe="\n"+STRICHE;
  for(i=0;i<sizeof(zeilennummern);i+=2)
    ausgabe+=zeilen[zeilennummern[i]]+"\n";
  ausgabe+="\n"+TYPES[key]+"s gefunden: "+(sizeof(zeilennummern))/2+".\n"+
    STRICHE;
  this_player()->More(ausgabe);
}

void add1(int key) {
  string datei,*zeilen,str;
  int *zeilennummern,i;
  datei=object_name(environment(this_player()))+".c";
  zeilen=explode(read_file(datei),"\n");
  zeilennummern=dateiinfo(datei,zeilen,key);
  if(str=this_player()->_unparsed_args()) {
    add2(str,datei,zeilennummern,key);
    return; }
  switch(key) {
    case 11..12: write("\n"+STRICHE+"  <Return>: DEFAULT_"+
        (key==11?"SMELL":"SOUND"));
    case 8..10: write("\n"+STRICHE+TYPES[key]+"-Kürzel mit Komma getrennt "
        "angeben:\n-> "); break;
    case 15..16: write("\n"+STRICHE+TYPES[key]+"-Name angeben   :\n-> ");
      break;
    case 17: write("\n"+STRICHE+
        "   Exit-Kuerzel:                  Pfad:\n"+
        "    n: norden   nw: nordwesten     r: ROOM\n"+
        "    o: osten    no: nordosten\n"+
        "    s: süden    so: südosten\n"+
        "    w: westen   sw: südwesten\n"+
        "    u: unten    ob: oben\n\n"+
        "Syntax: <Exitname>,<Zielraum>\n"+
        "        <Exitname>,<Pfad>,<Zielraum>\n"+STRICHE+
        "Exit angeben:\n-> "); break;
    case 18: write("\n"+STRICHE+
        "   Pfad:        Refresh-Art:\n"+
        "    n: NPC       d: REFRESH_DESTRUCT\n"+
        "    o: OBJ       n: REFRESH_NONE\n"+
        "    a: ARMOUR    r: REFRESH_REMOVE\n"+
        "    w: WEAPON    a: REFRESH_ALWAYS\n\n"+
        "Syntax: <Itemname>,<Refresh-Art>\n"+
        "        <Pfad>,<Itemname>,<Refresh-Art>\n"+STRICHE+
        "Item angeben:\n-> "); break;
    default: write("Ausnahmefehler C4-20f. Ezo benachrichtigen.\n"); }
  input_to("add2",0,datei,zeilennummern,key);
}

varargs void add2(string str,string datei,int *zeilennummern,int key,
  string extra) {
  int i,m;
  string savefile,*zeilen,neuezeilen,*kuerzel;
  str=implode(explode(str,"\""),"\\\"");
  if(str==""&&key==11) str="DEFAULT_SMELL,\ndefault";
  if(str==""&&key==12) str="DEFAULT_SOUND,\ndefault";
  if(str==""||str=="q") {
    write("\nAbgebrochen!\n");
    return; }
  if(!extra) {
    extra="";
    neuezeilen="  "+KEYS[key]+"(";
    kuerzel=explode(str,",");
    for(i=0;i<sizeof(kuerzel);i++) {
      while(kuerzel[i][0..0]==" ") kuerzel[i]=kuerzel[i][1..<1];
      while(kuerzel[i][<1..<1]==" ") kuerzel[i]=kuerzel[i][0..<2];
      switch(key) {
        case 8..9:
        case 13  : if(member(datei->Query(P_DETAILS),kuerzel[i]))
                     kuerzel[i]+="#"; break;
        case 10  : if(member(datei->QueryProp(P_READ_DETAILS),kuerzel[i]))
                     kuerzel[i]+="#"; break;
        case 11  : if(member(datei->QueryProp(P_SMELLS),kuerzel[i]))
                     kuerzel[i]+="#"; break;
        case 12  : if(member(datei->QueryProp(P_SOUNDS),kuerzel[i]))
                     kuerzel[i]+="#"; break;
        default  : }
      if(kuerzel[i][<1..<1]=="#") {
        write("\n"+TYPES[key]+" "+(kuerzel[i][0..0]=="\n"?kuerzel[0]:
          "\""+kuerzel[i][0..<2]+"\"")+" existiert schon. "+
          "Abgebrochen!\n");
        return; } } }
  else neuezeilen=extra;
  switch(key) {
    case 8..12:
      if(extra=="") {
        if(kuerzel[0][0..6]=="DEFAULT") neuezeilen+=kuerzel[0]+",\n";
        else if(sizeof(kuerzel)==1) neuezeilen+="\""+kuerzel[0]+"\",\n";
        else {
          neuezeilen+="({";
          while(sizeof(kuerzel)) {
            if(sizeof(explode(neuezeilen,"\n")[<1]+kuerzel[0])>76)
              neuezeilen+="\n    ";
            neuezeilen+="\""+kuerzel[0]+"\",";
            kuerzel-=({kuerzel[0]}); }
          neuezeilen=neuezeilen[0..<2]+"}),\n"; }
        write("\n"+TYPES[key]+"-Beschreibung eingeben:\n-> ");
        input_to("add2",0,datei,zeilennummern,key,neuezeilen);
        return; }
      else if(key!=9) {
        kuerzel=explode(break_string(str,70),"\n");
        for(i=0;i<sizeof(kuerzel);i++)
          neuezeilen+="    \""+kuerzel[i]+" \"\n";
        neuezeilen=neuezeilen[0..<4]+"\");\n"; }
      else {
        kuerzel=explode(break_string(str,68),"\n");
        if(extra[<1..<1]!="#") neuezeilen+="    ({\""+kuerzel[0]+" \"\n";
        else neuezeilen=neuezeilen[0..<2]+"      \""+kuerzel[0]+" \"\n";
        for(i=1;i<sizeof(kuerzel);i++)
          neuezeilen+="      \""+kuerzel[i]+" \"\n";
        if(extra[<1..<1]!="#") {
          neuezeilen=neuezeilen[0..<4]+"\",\n#";
          write("\nNacht-Beschreibung eingeben:\n-> ");
          input_to("add2",0,datei,zeilennummern,key,neuezeilen);
          return; }
        neuezeilen=neuezeilen[0..<4]+"\"}));\n"; } break;
    case 15..16: if(sizeof(extra)==0) {
        neuezeilen+="\""+capitalize(str)+"\",1";
        write("\n"+TYPES[key]+"-Kürzel angeben:\n-> ");
        input_to("add2",0,datei,zeilennummern,key,neuezeilen);
        return; }
      switch(to_int(extra[<1..<1])) {
        case 1: kuerzel=explode(str,",");
          for(i=0;i<sizeof(kuerzel);i++) {
            while(kuerzel[i][0..0]==" ") kuerzel[i]=kuerzel[i][1..<1];
            while(kuerzel[i][<1..<1]==" ") kuerzel[i]=kuerzel[i][0..<2]; }
          if(sizeof(kuerzel)==1)
            neuezeilen=extra[0..<2]+"\""+kuerzel[0]+"\",\n    2";
          else {
            kuerzel=m_indices(mkmapping(kuerzel));
            neuezeilen=extra[0..<2]+"({";
            while(sizeof(kuerzel)) {
              if(sizeof(explode(neuezeilen,"\n")[<1]+kuerzel[0])>76)
                neuezeilen+="\n    ";
              neuezeilen+="\""+kuerzel[0]+"\",";
              kuerzel-=({kuerzel[0]}); }
            neuezeilen=neuezeilen[0..<2]+"}),\n    2"; }
          write("\nPreis angeben        :\n-> ");
          input_to("add2",0,datei,zeilennummern,key,neuezeilen);
          return;
      case 2: if(!to_int(str)) {
          write("\nPreis angeben (!=0)  :\n-> ");
          input_to("add2",0,datei,zeilennummern,key,extra);
          return; }
        neuezeilen=extra[0..<2]+str+",3";
        write("\nLP-Heilung angeben   :\n-> ");
        input_to("add2",0,datei,zeilennummern,key,neuezeilen);
        return;
      case 3:
        neuezeilen=extra[0..<2]+"({"+str+",4";
        write("\nMP-Heilung angeben   :\n-> ");
        input_to("add2",0,datei,zeilennummern,key,neuezeilen);
        return;
      case 4:
        neuezeilen=extra[0..<2]+str+"}),"+(key==16?"5":"6");
        if(key==16) write("\nAlkoholgehalt angeben:\n-> ");
        else write("\nSättigung angeben    :\n-> ");
        input_to("add2",0,datei,zeilennummern,key,neuezeilen);
        return;
      case 5:
        neuezeilen=extra[0..<2]+str+",6";
        write("\nSättigung angeben   :\n-> ");
        input_to("add2",0,datei,zeilennummern,key,neuezeilen);
        return;
      case 6:
        neuezeilen=extra[0..<2]+str+",\n7";
        write("\nText für Spieler    :\n-> ");
        input_to("add2",0,datei,zeilennummern,key,neuezeilen);
        return;
      case 7:
        neuezeilen=extra[0..<2];
        kuerzel=explode(break_string(str,70),"\n");
        if(sizeof(kuerzel)==1) neuezeilen+="    ({\""+kuerzel[0]+" \"\n";
        else neuezeilen+="    ({\""+kuerzel[0]+" \"\n";
        for(i=1;i<sizeof(kuerzel);i++)
          neuezeilen+="      \""+kuerzel[i]+" \"\n";
        neuezeilen=neuezeilen[0..<4]+"\",\n8";
        write("\nText für Andere     :\n-> ");
        input_to("add2",0,datei,zeilennummern,key,neuezeilen);
        return;
      case 8:
        neuezeilen=extra[0..<2];
        kuerzel=explode(break_string(str,70),"\n");
        for(i=0;i<sizeof(kuerzel);i++)
          neuezeilen+="      \""+kuerzel[i]+" \"\n";
        neuezeilen=neuezeilen[0..<4]+"\"}));\n";
        break;
      default: write("Ausnahmefehler 013Y. Ezo benachrichtigen.\n"); }
      break;
    case 17: if(sizeof(kuerzel)>3||sizeof(kuerzel)<2) {
        write("\nFalsche Syntax. Abgebrochen!\n");
        return; }
      if((i=member(SHORT_EXITS,kuerzel[0]))!=-1)
        kuerzel[0]=LONG_EXITS[i];
      neuezeilen+="\""+kuerzel[0]+"\","+(sizeof(kuerzel)==3?
        (kuerzel[1]=="r"?"ROOM":kuerzel[1]):"")+"\""+kuerzel[<1]+"\");\n";
      break;
    case 18: switch(kuerzel[<1]) {
        case "d": kuerzel[<1]="REFRESH_DESTRUCT"; break;
        case "n": kuerzel[<1]="REFRESH_NONE"; break;
        case "r": kuerzel[<1]="REFRESH_REMOVE"; break;
        case "a": kuerzel[<1]="REFRESH_ALWAYS"; break;
        default : write("\nFalsche Refreshart. Abgebrochen!\n"); return; }
      if(sizeof(kuerzel)==3) {
        switch(kuerzel[0]) {
          case "n": neuezeilen+="NPC"; break;
          case "a": neuezeilen+="ARMOUR"; break;
          case "w": neuezeilen+="WEAPON"; break;
          case "o": neuezeilen+="OBJ"; break;
          default: neuezeilen+=kuerzel[0]; }
        kuerzel-=({kuerzel[0]}); }
      if(sizeof(kuerzel)!=2) {
        write("\nFalsche Syntax. Abgebrochen!\n");
        return; }
      neuezeilen+="\""+kuerzel[0]+"\","+kuerzel[1]+");\n"; break;
    default: write("Ausnahmefehler t90 a). Ezo benachrichtigen.\n"); }
  zeilen=explode(read_file(datei),"\n");
  savefile=implode(zeilen[0..zeilennummern[0]-1],"\n")+"\n"+neuezeilen+
    implode(zeilen[zeilennummern[0]..<1],"\n")+"\n";
  write("\n"+STRICHE+neuezeilen+STRICHE+TYPES[key]+" einfügen? (j/n)\n-> ");
  input_to("savefile",0,datei,savefile,key," in Raum eingefügt");
}

void remove1(int key) {
  string datei,*zeilen,str;
  int *zeilennummern,i;
  datei=object_name(environment(this_player()))+".c";
  zeilen=explode(read_file(datei),"\n");
  zeilennummern=dateiinfo(datei,zeilen,key);
  if(zeilennummern[0]>zeilennummern[1]) {
    write("\nKeine "+TYPES[key]+"s gefunden.\n");
    return; }
  if(str=this_player()->_unparsed_args()) {
    remove2(str,datei,zeilennummern,key);
    return; }
  write("\n"+STRICHE);
  for(i=0;i<sizeof(zeilennummern);i+=2)
    write(zeilen[zeilennummern[i]]+"\n");
  write(STRICHE+"Welches "+TYPES[key]+" löschen?\n-> ");
  input_to("remove2",0,datei,zeilennummern,key);
}

void remove2(string str,string datei,int *zeilennummern,int key) {
  int i,m;
  string savefile,*zeilen;
  if(str==""||str=="q") {
    write("\nAbgebrochen!\n");
    return; }
  str="\""+str+"\"";
  zeilen=explode(read_file(datei),"\n");
  for(i=0;i<sizeof(zeilennummern);i+=2) {
    if(strstr(zeilen[zeilennummern[i]],str)!=-1) {
      write("\n"+STRICHE+TYPES[key]+" gefunden:\n\n");
      for(m=zeilennummern[i];m<=zeilennummern[i+1];m++)
        write(zeilen[m]+"\n");
      write(STRICHE+TYPES[key]+" löschen? (j/n)\n-> ");
      break; } }
  if(i>=sizeof(zeilennummern)) {
    write("\nKein "+TYPES[key]+" "+capitalize(str)+
      " gefunden. Abgebrochen!\n");
    return; }
  savefile=implode(zeilen[0..zeilennummern[i]-1],"\n")+"\n"+
    implode(zeilen[zeilennummern[i+1]+1..<1],"\n")+"\n";
  input_to("savefile",0,datei,savefile,key," aus Raum entfernt");
}

void edit1(int key) {
  string datei,*zeilen,str;
  int *zeilennummern,i;
  datei=object_name(environment(this_player()))+".c";
  zeilen=explode(read_file(datei),"\n");
  zeilennummern=dateiinfo(datei,zeilen,key);
  if(str=this_player()->_unparsed_args()) {
    edit2(str,datei,zeilennummern,key);
    return; }
  write("\n"+STRICHE);
  if(key==5) write(liste(TERRAIN_TYPES));
  if(key==4) write(liste(INDOOR_TYPES));
  if(key==7) write(liste(DAY_TYPES));
  if(zeilennummern[0]>zeilennummern[1])
    write("\nNoch kein"+(key<4?"e ":" ")+TYPES[key]+" vorhanden.\n");
  else
    switch(key) {
    case 0..2: for(i=zeilennummern[0];i<zeilennummern[1]+1;i++)
                 write(zeilen[i]+"\n"); break;
//    case    3: write("  "+TYPES[key]+": "+datei->
//                 QueryProp(P_TEMPERATURE)+"\n"); break;  
    case    4: write("  "+TYPES[key]+": "+INDOOR_TYPES[datei->
                 QueryProp(P_INDOORS)]+"\n"); break;
//    case    5: write("  "+TYPES[key]+": "+(((i=datei->QueryProp(P_TERRAIN))>
//                 200)?TERRAIN_TYPES_OUT[i-201]:((i>100)?
//                 TERRAIN_TYPES_IN[i-101]:"unbestimmt"))+"\n"); break;
    case 6..7: write("  "+TYPES[key]+": "+datei->
                 QueryProp(KEYS[key])+"\n"); break;
    default: write("Ausnahmefehler P150&2. Ezo benachrichtigen.\n"); }
  write(STRICHE+"Neue"+(key<4?" ":"n ")+TYPES[key]+" eingeben:\n-> ");
  input_to("edit2",0,datei,zeilennummern,key);
}

void edit2(string str,string datei,int *zeilennummern,int key) {
  int i,m;
  string savefile,*zeilen,neuezeilen;
  if(str=="q"||str=="") {
    write("\nAbgebrochen!\n");
    return; }
  neuezeilen="  SetProp("+KEYS[key]+",";
  switch(key) {
    case 0..2: if(sizeof(str)<63-sizeof(KEYS[key]))
                 neuezeilen+="\""+str+"\");\n";
               else
                 neuezeilen+="\n"+implode(explode(break_string
                 (str,70,"    \""),"\n")," \"\n")+"\");\n"; break;
    case 6   : neuezeilen+=to_int(str)+");\n"; break;
    case 7   : if(to_int(str)==0) neuezeilen="";
               else neuezeilen+=to_int(str)+");\n"; break;
    case 3   : neuezeilen+=to_int(str)+");\n"; break;
    case 5   : neuezeilen+="TT_"+((i=to_int(str))>-1&&i<sizeof(TERRAIN_TYPES)?
               TERRAIN_TYPES_ORIGINAL[i]+");\n":"OTHER);\n"); break;
    case 4   : neuezeilen+="RC_"+((i=to_int(str))>-1&&i<sizeof(INDOOR_TYPES)?
               INDOOR_TYPES_ORIGINAL[i]+");\n":"OUTDOORS);\n"); break;
    default  : write("Ausnahmefehler 2443-11x10. Ezo benachrichtigen.\n"); }
  savefile="";
  zeilen=explode(read_file(datei),"\n");
  for(i=0;i<zeilennummern[0];i++)
    savefile+=zeilen[i]+"\n";
  savefile+=neuezeilen;
  for(i=zeilennummern[1]+1;i<sizeof(zeilen);i++)
    savefile+=zeilen[i]+"\n";
  write("\n"+STRICHE+neuezeilen+STRICHE+"Geänderte"+(key<4?" ":"n ")+
    TYPES[key]+" einfügen? (j/n)\n-> ");
  input_to("savefile",0,datei,savefile,key," in Raum geschrieben.");
}

string liste(string *arr) {
  int i,m;
  string ausgabe;
  ausgabe="";
  m=sizeof(arr);
  for(i=0;i<m/2;i++)
    ausgabe+=sprintf(" %20s: %2d    %20s: %2d\n",arr[i],i,arr[i+m/2],i+m/2);
  ausgabe+=(m%2?sprintf(" %48s: %2d\n",arr[i*2],i*2):"\n");
  return ausgabe;
}

void savefile(string str,string datei,string savefile,int key,string text) {
  object raum,*objekte;
  int i;
  if(str!="j"&&str!="ja") {
    write("\nAbgebrochen!\n");
    return; }
  if(!rm(datei)) {
    write("\nSchwerer Ausnahmefehler 23/05: Keine Schreibrechte!\n");
    return; }
  write_file(datei,savefile);
  call_other(datei,"???",0);
  raum=find_object(datei);
  write("\n"+TYPES[key]+text+".\n");
  objekte=all_inventory(raum);
  for(i=0;i<sizeof(objekte);i++)
    objekte[i]->move("/room/void",M_TPORT|M_SILENT|M_NO_SHOW|M_NOCHECK);
  catch(raum->remove());
  if(raum) destruct(raum);
  if(catch(call_other(datei,"???"))) {
    write("Fehler beim Neuladen des Raumes. Du bist jetzt im void.\n");
    say(this_player()->Name()+" hat Mist gebaut. Willkommen im void.\n");
    return; }
  raum=find_object(datei);
  while(sizeof(all_inventory(raum))) all_inventory(raum)[0]->remove();
  for(i=0;i<sizeof(objekte);i++)
    if(objekte[i])
      objekte[i]->move(raum,M_TPORT|M_SILENT|M_NO_SHOW|M_NOCHECK);
  if(environment(this_player())!=raum)
    this_player()->move(raum,M_TPORT|M_SILENT|M_NO_SHOW|M_NOCHECK);
}

// Hier werden die Anfangs- und Endzeilennummern aus der Datei gelesen
int *dateiinfo(string datei,string *zeilen,int key) {
  int *createline,i,m,*zeilennummern;
  createline=createfind(datei);
  zeilennummern=({});
  for(i=createline[0];i<=createline[1];i++) {
    if(strstr(zeilen[i],KEYS[key])!=-1)
      zeilennummern+=({i});
    if(key==9&&sizeof(zeilennummern)&&zeilennummern[<1]==i-1&&
      strstr(zeilen[i],"({")==-1)
      zeilennummern-=({i-1});
    if(strstr(implode(explode(zeilen[i]," "),""),");")!=-1 &&
      sizeof(zeilennummern)%2)
      zeilennummern+=({i}); }
  if(sizeof(zeilennummern)%2||!sizeof(zeilennummern)) {
    zeilennummern=({createline[1],18});
    for(i=createline[0];i<createline[1];i++) {
      for(m=key;m<zeilennummern[1];m++) {
        if(strstr(zeilen[i],KEYS[m])!=-1) {
          zeilennummern=({i,m});
          break; } }
      if(zeilennummern[1]==key) break; }
    zeilennummern[1]=zeilennummern[0]-1; }
  return zeilennummern;
}

// Hier wird das create() ausfindig gemacht. Nur dort drin wird gesucht
int *createfind(string datei) {
  string *zeilen;
  int startline,endline,counter,i,m;
  zeilen=explode(read_file(datei),"\n");
  for(i=0;i<sizeof(zeilen);i++) {
    if(strstr(implode(explode(zeilen[i]," "),""),"create()")!=-1 &&
      !startline) {
      for(m=i;m<sizeof(zeilen);m++) {
        if(strstr(zeilen[m],"{")!=-1) {
          startline=m;
          i=m;
          break; } } }
    if (!startline) continue;
    for(m=0;m<sizeof(zeilen[i]);m++) {
      if (zeilen[i][m..m]=="{") counter++;
      if (zeilen[i][m..m]=="}") counter--; }
    if (!counter) {
      endline=i;
      break; } }
  return ({startline,endline});
}
