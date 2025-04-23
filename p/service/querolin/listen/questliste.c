
#include <properties.h>
#include <language.h>
#include <new_skills.h>
#include <ansi.h>
#include "/secure/questmaster.h"

static int compare( mixed *i, mixed *j )
{
  if( i[2] == j[2] )
    return i[1] > j[1];
  else
    return i[2] > j[2];
}

varargs string liste()
{
  int qgroups, i, j, n, *dontneed;
  mixed *qlists, *quests;
  string str, bold, normal;

  bold=normal="";

  str = sprintf("%s%-38s%s    AP  Stufe           verantw. Magier\n",
      bold,"Liste der Abenteuer",normal);
  for( i=0; i<39; i++, str+="--" );
  str += "\n";
  qgroups = sizeof(dontneed=QM->QueryDontneed(this_object()));
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
      qlists[i] = sort_array( qlists[i], #'compare); // ME );
      for( j=0; j<sizeof(qlists[i]); j++ )
      {
        if(qlists[i][j][4]==0)
          str +=
            sprintf( "  %-38s %3d   %2d             %s\n",
              qlists[i][j][0], qlists[i][j][1], qlists[i][j][2],
              capitalize(qlists[i][j][3])
        );
      }
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
        str += sprintf( "  %-38s %3d   %2d             %s\n",
                       qlists[i][j][0], qlists[i][j][1], qlists[i][j][2],
                       capitalize(qlists[i][j][3])
    	              );
      }
    }
  }
  if (!n)
    str += "  In dieser Gruppe gibt es derzeit keine Abenteuer.\n";

   return str+"\n";
}

