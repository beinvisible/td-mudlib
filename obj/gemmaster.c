/*

  /obj/gemmaster

  Usage:

  "/obj/gemmaster"->DistributeGem(amount, rand, art, automove)

    ergibt amount + random(rand) Edelsteine

    art         0                   ein beliebiger Edelstein
                1                   lauter verschiedene Edelsteine
    
    automove    Wenn gesetzt, Edelsteine nach previous_object moven            

*/

#define GEMLOC "/obj/gems"

#include <language.h>
#include <properties.h>
#include <moving.h>

static string *gems;

void create()
{
  if (clonep(this_object())) {
    destruct(this_object());
    return;
  }
  gems=(get_dir(GEMLOC+"/*.c"));
}

varargs mixed DistributeGem(int amount, int rand, int art, int automove)
{
  int i,j;
  object o;
  object *ob;
  
  if (!(i=sizeof(gems)))
    return 0;
  if (!rand)
    rand=1;
  if (!amount)
    amount=1;
  if( art )
  {
    ob = ({});
    for(j=amount+random(rand);j>0;j--)
    {
      o=clone_object(GEMLOC+"/"+gems[random(i)]);
      if( automove )
        o->move(previous_object(),M_NOCHECK);
      ob+=({o});
    }
    return ob;
  }
  else
  {
    o=clone_object(GEMLOC+"/"+gems[random(i)]);
    o->SetProp(P_AMOUNT, amount+random(rand));
    if( automove )
      o->move(previous_object(),M_NOCHECK);
    return o;
  }
}

