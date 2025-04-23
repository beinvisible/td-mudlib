// Tamedhon MUDlib
//
// std/player/moneyhandler.c -- money handler for players
//
// $Date: 2003/07/05 15:45:00 $                      
/* $Revision: 1.2$       
 * $Log: moneyhandler.c,v $
 * Revision 1.2  2003/07/05 15:45:00  Querolin
 * AddMoney-BUG endlich raus...
 *
 * Revision 1.1  2001/01/12 18:54:47  Samtpfote
 * Initial revision
 *
 * Revision 3.3  1999/03/30 12:38:46  Woody
 * Neuer Code fuer negative Betraege bei AddMoney()
 *
 * Revision 3.2  1999/03/10 21:51:50  Woody
 * Revidiert und fuer Geldboerse vorbereitet
 *
 * Revision 3.1  1998/08/22 17:35:52  Creative
 * Sicherheitsabfrage fuer Betraege < 0
 *
 * Revision 3.0  1997/03/13 01:41:31  Woody
 * prepared for revision control
 *
 * Revision 2.5  1997/03/11  13:07:00  Creative@SL
 * QueryDeepMoney() eingefuehrt.
 *
 * Revision 2.4  1995/07/24  19:30:33  Rochus
 * AddMoney-Logging, zum Auffinden von nicht-std-laeden usw.
 *
 * Revision 2.3  1994/07/12  00:08:17  Jof
 * *** empty log message ***
 *
 * Revision 2.2  1994/04/07  21:40:19  Freund
 * Wenn 0 Muenzen uebergeben werden, wird jetzt immer sofort ein Erfolg gemeldet... :)
 *
 * Revision 2.1  1994/03/17  23:36:55  Jof
 * PROTECTED -> SECURED & Reindented
 *
 * Revision 2.0  1994/01/13  16:59:15  mud
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/05  16:17:07  mud
 * changed save variables partly to mapping entry
 *             
 */                            

#pragma strong_types

#include <properties.h>
#include <moving.h>
#include <wizlevels.h>

int AddMoney(int amount)
{
  object money, logger;

  if (!amount) return 1;

  if (objectp(logger=find_object("p/daemon/moneylog"))
      && amount>0
      && query_once_interactive(this_object())
      && !IS_WIZARD(this_object()))
    logger->AddMoney(previous_object(),amount);

  if (amount < 0)
  {
    object *obs;
    int rest, val, i;

    rest=amount;
    obs=all_inventory(this_object());
    for (i=0; rest && (i<sizeof(obs)); i++)     // vom ersten zum letzten!
    {
    /*
      switch (explode(object_name(obs[i]),"#")[0]) {
        case "/obj/money":
        case "/obj/boerse":
          if ((val = -obs[i]->QueryProp(P_AMOUNT)) <= rest) val=rest;
          if (val>-1) break;
          obs[i]->AddAmount(val);
          rest-=val;
      }
      */
      if((explode(object_name(obs[i]),"#")[0] == "/obj/money") ||
         (member(inherit_list(obs[i]), "/p/obj/boerse.c") != -1)
        )
      {
      	 if ((val = -obs[i]->QueryProp(P_AMOUNT)) <= rest) val=rest;
//      	 if (val>-1) break;
	if (val<0) {
      	 obs[i]->AddAmount(val);
      	 rest-=val;
        }
      }
    }
    return rest ? 2 : 1;        // 1: alles OK. 2: nur Teilbetrag abgezogen.
  }

  money=clone_object("/obj/money");
  money->SetProp(P_AMOUNT,amount);
  return money->move(this_object(),M_PUT|M_MOVE_ALL);
}

int QueryMoney()
{
  object ob;
  int ret;

  ret=0;
  ob=first_inventory(this_object());
  while (ob) {

/*
    switch (explode(object_name(ob),"#")[0]) {
      case "/obj/money":
      case "/obj/boerse":
        ret+=ob->QueryProp(P_AMOUNT);
    }
*/
    if((explode(object_name(ob),"#")[0] == "/obj/money") ||
       (member(inherit_list(ob), "/p/obj/boerse.c") != -1)
      )
    {
    	ret+=ob->QueryProp(P_AMOUNT);
    }
        
    ob=next_inventory(ob);
  }
  return ret;
}

int QueryDeepMoney()
{
  object *inv;
  int i, mon;
 
  mon=0;
  inv=deep_inventory(this_object());
  for (i=sizeof(inv)-1; i>=0; i--)
    if (explode(object_name(inv[i]),"#")[0]=="/obj/money")
      mon+=inv[i]->QueryProp(P_AMOUNT);
  return mon;
}

