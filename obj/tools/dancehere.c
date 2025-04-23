/*
** DanceHere.c - Woody@SL
** ---
** beruht auf Sandtigerkralle von Dancer
** funktioniert nur im aktuellen Raum, das dafuer aber mit
** diversen Erweiterungen.
** ---
** 1996-05-01  1st Release
** 1997-11-17  break_string raus
*
* $Date: 2001/01/13 13:01:18 $
* $Revision: 1.2 $
* $Log: dancehere.c,v $
* Revision 1.2  2001/01/13 13:01:18  Samtpfote
* Sperre wegen Lehrlinge eingebaut.
*
* Revision 1.1  2001/01/13 13:00:36  Samtpfote
* Initial revision
*
*
*/

inherit "std/thing";
 
#include <properties.h>
#include <defines.h>
#include <wizlevels.h>
#include <ansi.h>
 
#define bs(x) break_string(x, 78)
#define DEF(x) (x=="\ndefault" ? "Default" : x)
 
private int i, counter, first_detail_line, startline, endline; 
private string *detail_shorts, *newstring, *file_lines, *new_lines;
private string detail_long, save_string, old_file, new_file, actual_file,
               detail_text;
private object here;
private mapping det;
private string *ind;
private string *col;

void create()
{
   ::create();
   SetProp( P_AUTOLOADOBJ,1 );
   SetProp( P_NAME, "Dancers Sandtigerkralle" );
   SetProp( P_GENDER, FEMALE );
   SetProp( P_SHORT, "Dancers Sandtigerkralle" );
   SetProp( P_LONG, bs( "Die Sandtigerkralle von Dancer ist "+
              "ein kleines Tool, mit dem Du gewöhnliche Details "+
              "in allen Räumen nachschauen und in Deinen eigenen "+
              "hinzufügen kannst. Weitere Informationen kannst Du "+
              "mit dem Befehl <dancetool> bekommen." ) );
   SetProp( P_ARTICLE, 2 );
   SetProp( P_WEIGHT, 500 );
   SetProp( P_VALUE, 10000 );
   SetProp( P_NOBUY, 1 );
   SetProp( P_NODROP, 1 );
   AddId( "kralle" );
   AddId( "sandtigerkralle" );
   AddId( "tigerkralle" );
   AddId( "dancetool" );
   AddId( "dancertool" );
   seteuid(getuid(this_object()));
}
 
 
void init()
{
  string tty;
  if (!IS_WIZARD(this_player())) 
  {
	call_out("remove", 0);
	return;
  }
  ::init();
  add_action("query_add_details", "krsd" );
  add_action("set_add_detail", "krad" );
  add_action("give_info", "dancetool" );
  col = ({0,0,0,0});
  if ((tty=this_player()->QueryProp(P_TTY)) == "ansi") {
    col[0] = ANSI_NORMAL;
    col[1] = ANSI_BOLD + ANSI_RED;
    col[2] = ANSI_BOLD + ANSI_CYAN;
    col[3] = ANSI_BOLD + ANSI_GREEN;
  }
  else if (tty == "vt100") {
    col[0] = ANSI_NORMAL;
    col[1] = ANSI_BOLD;
    col[2] = ANSI_BOLD;
    col[3] = ANSI_NORMAL;
  }
  else {
    col[0] = col[1] = col[2] = col[3] = ANSI_NORMAL;
  }
}
 
 
int give_info()
{
   write( "-----------------------------------------------------------------------------\n" );
   write( "Die Sandtigerkralle ermöglicht folgende Befehle:\n" );
   write( "\nkrad:\n" );
   write( bs( "Ruft einen kleinen AddDetail-Editor auf, mit dem Du "+
      "Details in Deine eigenen Räume einfügen kannst. Als Argument "+
      "kann hinter den Befehl der Filename eines Raumes eingegeben "+
      "werden. Wird kein Argument eingegeben, so wird der Raum "+
      "genommen, in dem Du gerade stehst. Achtung: Es können "+
      "nur Details in Räume eingefügt haben, die bereits ein "+
      "AddDetail in ihrem create() haben. Du musst die Lupe haben, "+
      "da der Raum nach dem Einfügen des Details automatisch "+
      "neu geladen wird." ) );
   write( "\nkrsd:\n" );
   write( bs( "Zeigt Dir alle Zeilen des angesprochenen Raumes bzw. "+
      "des Raumes in dem Du stehst, die ein AddDetail "+
      "beinhalten. So kannst Du kontrollieren, welche Details "+
      "Du schon eingebaut hast." ) );
   write( "Optionaler Parameter: <qp>. Wenn angegeben, werden die Details abgefragt\n"+
          "(via QueryProp()) statt aus dem File gelesen.\n");
   write( "-----------------------------------------------------------------------------\n" );
   return 1;
}
 
 
int renew_variables()
{
   i = counter = first_detail_line = startline = endline = 0; 
   detail_shorts = newstring = file_lines = new_lines = ({});
   detail_long = save_string = old_file = new_file = actual_file
     = detail_text = here = 0;
   return 1;
} 
 
 
int sortdet(string a, string b)
{
  if (stringp(det[a]) && stringp(det[b]))
    return (det[a] > det[b]);
  else
    return 0;
}


int query_add_details(string str)
{
   string old_file, *file_lines, new_file, *new_lines, actual_file;
   int i, counter, first_detail_line, startline, endline, qp;

   qp = (str && str == "qp");
   here = environment (this_player());
   actual_file = object_name(here)+".c";
   if ( file_size( actual_file )==-1 )
   {
     write( "File: "+actual_file+" nicht gefunden!\n" );
     return 1;
   }
   write( "\n-----------------------------------------------------------------------------\n" );
   write( "Aktuelles File:    "+col[1]+actual_file+col[0]+"\n");
   write( "Filegröße:       "+col[1]+file_size(actual_file)+col[0]+"\n");
   write( "User-ID des Files: "+col[1]+getuid(here)+col[0]+"\n");
   write( "-----------------------------------------------------------------------------\n" );
   new_file = "";
   counter = 0;
   first_detail_line = 0;
   old_file = read_file (actual_file);
   file_lines = ({ "dummy" }) + explode( old_file, "\n" );
   startline = 1;
   endline = sizeof(file_lines)-1;
  if (qp)
  {
    det = here->QueryProp(P_DETAILS);
    if (det)
    {
      int n;
      counter = n = sizeof (det);
      ind = sort_array (m_indices(det), "sortdet");
      write (col[2]);
      for (i = 0; i < n; i++)
      {
        if (mappingp(det[ind[i]]))
          det[ind[i]]="RASSENABHÄNGIG:\n"+det[ind[i]][0];
        if ((i < (n-1)) && (det[ind[i]]==det[ind[i+1]]))
          counter--, write (DEF(ind[i])+", ");
        else
        {
          // write (ind[i] + ":\n" + break_string (det[ind[i]], 78, "  "));
          write (DEF(ind[i]) + ":\n" + col[3] + det[ind[i]] + col[2]);
        }
      }
      write (col[0]);
    }
  }
  else {
   for (i = startline; i <= endline; i++)
   {
      new_file = new_file+file_lines[i]+"\n";
      if (( strstr( file_lines[i], "AddDetail" ) != -1 )
        || (strstr( file_lines[i], "AddSpecialDetail" ) != -1 )
        || (strstr( file_lines[i], "AddSound" ) != -1 )
        || (strstr( file_lines[i], "AddSmell" ) != -1 ))
      {
         if (!first_detail_line) first_detail_line = i;
         counter++;
         write(col[2]+file_lines[i]+"\n"+col[3]);
         while ((strstr(file_lines[i], ");") == -1) && (i <= endline))
         {
           i++;
           write(file_lines[i]+"\n" );
         }
      }
   }                                
  }
   write (col[0]);
   write( "-----------------------------------------------------------------------------\n" );
   write( "Details found: "+col[1]+counter+col[0]+"\n" ); 
   write( "-----------------------------------------------------------------------------\n" );
   return 1;
}    
 
 
int set_add_detail()
{
   renew_variables();

   here = environment (this_player());
   actual_file = object_name(here)+".c";
   if (file_size (actual_file) == -1)
   {
     write( "File: "+actual_file+" nicht gefunden!\n" );
     return 1;
   }
   call_other( actual_file, "???" );
   write( "\n-----------------------------------------------------------------------------\n" );
   write( "Aktuelles File:    "+col[1]+actual_file+col[0]+"\n");
   write( "Filegröße:       "+col[1]+file_size(actual_file)+col[0]+"\n");
   write( "User-ID des Files: "+col[1]+getuid(here)+col[0]+"\n");
   write( "-----------------------------------------------------------------------------\n" );
   new_file = "";
   new_lines = ({ "dummy" });
   counter = 0;
   first_detail_line = 0;
   old_file = read_file( actual_file );
   file_lines = ({ "dummy" }) + explode( old_file, "\n" );          
   startline = 1;
   endline = sizeof( file_lines )-1;
   for( i = startline; i <= endline; i++ )
   {
      new_file = new_file+file_lines[ i ]+"\n";
      if (( strstr( file_lines[i], "AddDetail" ) != -1 )
        || (strstr( file_lines[i], "AddSpecialDetail" ) != -1 )
        || (strstr( file_lines[i], "AddSound" ) != -1 )
        || (strstr( file_lines[i], "AddSmell" ) != -1 ))
      {
        if ( !first_detail_line ) first_detail_line = i;
        counter = counter + 1;
      }
   }
   if ( !first_detail_line )
   {
      write( bs( "Es wurden im File "+actual_file+" keine "+
         "Details gefunden! Um Details hinzufügen zu können, "+
         "muss mindestens ein AddDetail bereits vorhanden sein!" ) );
      write( "-----------------------------------------------------------------------------\n" );
      return 1;
   }
   write( "Details found:    "+col[1]+counter+col[0]+"\n" );
   det = here->QueryProp(P_DETAILS);
   ind = sort_array (m_indices(det), "sortdet");
   write( break_string ("Details: " + col[2] + implode(ind, ", ") + col[0] +
            "\n", 78) );
   write( "Langbeschreibung des Raumes:\n" );
   write( col[3] + break_string (here->QueryProp(P_INT_LONG), 78) + col[0] );
   write( "-----------------------------------------------------------------------------\n" );
   write( "Bitte die Detail-Kürzel eingeben (mehrere durch Leerzeichen trennen):\n"+
          "Kürzel zu existierendem hinzufügen: '*kürzel [...]'.  Abbruch: 'q'.\n->");
   input_to( "get_detail_short" );
   return 1;
}    
 
 
int get_detail_short( string str )
{
   string dummy;
   if ( str == "q" )
   {
     tell_object( this_player(), "Abgebrochen!\n" );
     return 1;
   }
   write( "-----------------------------------------------------------------------------\n" );
   detail_shorts = map( explode( str, " " ),#'lower_case);
   if (str[0] == '*')
   {
     str=str[1..];
     dummy=
     implode(sort_array(map(detail_shorts,#'lower_case),#'>),", ");
     write (break_string("Ok, zu welchem Detail soll/sollen \""+
     col[2]+dummy+col[0]+
       "\" als Kürzel hinzugefuegt werden? (Abbruch mit 'q')", 78)+"->");
     input_to( "get_detail_long" );
     return 1;
   }
   dummy = break_string ("\""+col[2]+
     implode(sort_array(map(detail_shorts,#'lower_case),#'>),", ")+
     col[0]+"\": (Abbruch mit 'q')",78);
   write( bs ( "Bitte Beschreibung eingeben fuer das Detail mit dem/den Namen\n" ) );
   write( dummy+"->" );
   input_to( "get_detail_long" );
   return 1;
}
 
 
int get_detail_long( string str )
{
   if ( str == "q" )
   {
     write( "Abgebrochen!\n" );
     write( "-----------------------------------------------------------------------------\n" );
     return 1;
   }
   detail_long = break_string( str, 74 ); 
   save_string = "  AddDetail( ({ ";        
   for( i = 0; i < sizeof( detail_shorts ); i ++ )
   {
      if ( !(i%3) && i )
        save_string = save_string + "\n                ";
      if ( i < sizeof( detail_shorts ) - 1 ) 
         detail_shorts[ i ] = "\""+detail_shorts[ i ]+"\", ";
      else
         detail_shorts[ i ] = "\""+detail_shorts[ i ]+"\" ";
      save_string = save_string + detail_shorts[ i ];      
      write( detail_shorts[ i ]+"\n" );
   }
   save_string = save_string + "}), \n";
   newstring = explode( detail_long,"\n");
   i = 0; 
   for( i = 0; i<sizeof(newstring); i++) 
   {
      if ( i < sizeof(newstring)-1 )
        save_string = save_string +"    \""+ newstring[i]+" \"\n";
      else
        save_string = save_string +"    \""+ newstring[i]+"\" );";
   }
   new_lines += ({ "dummy" });
   for ( i = 1; i < first_detail_line; i++ )
   {
     new_lines += ({ file_lines[i] });
   }                                  
   new_lines += ({ save_string });
   for ( i = first_detail_line + 1; i <= endline+1; i++ )
     new_lines += ({ file_lines[ i - 1 ] });
   write( "-----------------------------------------------------------------------------\n" );
   write( bs( "Folgende Zeilen werden in die Datei "+actual_file+" geschrieben, falls `j` eingegeben wird:\n" ) );
   write( save_string+"\n" );
   write( "\nDetail speichern? (j/n)\n->" );
   input_to( "save_detail" );
   return 1;
}
 
 
int save_detail( string str )
{  
   if ( str != "j" && str != "ja" && str != "Ja" && str != "J" )
   {
     write( "Abgebrochen!\n" );
     write( "-----------------------------------------------------------------------------\n" );
     return 1;
   }   
   if ( !rm( actual_file ) )
   {
      write( "File: "+actual_file+" wurde nicht gefunden!\n" );
      return 1;
   }                   
   for ( i = 2; i <= endline+2; i++ )
     write_file( actual_file, new_lines[i]+"\n" ); 
     //write( new_lines[i]+"\n" ); 
   write( "Das Detail wurde in die Datei geschrieben.\n" );
   this_player()->command_me( "here make" );
   write( "-----------------------------------------------------------------------------\n" );  
   return 1;
}
