// SilberLand MUDlib
//
// /p/daemon/zentralbank.c -- Zentrale Geld-Verwaltung der Laeden
//
// $Date: 1998/01/19 13:55:07 $
/* $Revision: 1.5 $
 * $Log: zentralbank.c,v $
 * Revision 1.5  1998/01/19 13:55:07  Feyaria
 * Statusmitloggen nur alle 5 minuten.
 *
 * Revision 1.4  1997/03/16 02:40:53  Woody
 * prepared for revision control
 *
 * Revision 1.3  1995/07/10  07:31:04  Jof
 * typo fixed
 *
 * Revision 1.2  1995/07/10  07:27:59  Jof
 * Use bank.h
 * _query_current_money
 *
 * Revision 1.1  1995/07/05  16:29:26  Jof
 * Initial revision
 *
 */

#include <bank.h>
#define TIME ctime(time())[11..15]
#define LS 1000000

//#define DBS(x)		if(find_player("samtpfote")) \
//			    tell_object(find_player("samtpfote"), x)
#define DBS(x)

int allmoney,bank_default_percent,store_default_percent,shop_default_percent;
int falling;
static int last_save;

void save_me()
{
  save_object("/p/daemon/save/zentralbank");
  last_save=time();
}

void create()
{
  seteuid(getuid());
  allmoney=500000;
  bank_default_percent=BANK_DEFAULT_PERCENT;
  store_default_percent=STORE_PERCENT_LEFT;
  shop_default_percent=SHOP_PERCENT_LEFT;
  restore_object("/p/daemon/save/zentralbank");
}

void adjust_percents()
{
  if (allmoney<10000   && falling>=0) falling=-1;
  if (allmoney>750000 && falling<=0) falling=1;
  if (allmoney<200000 && falling>0 ) falling=0;
  if (allmoney>20000  && falling<0 ) falling=0;
  if (falling<0)
  {
    bank_default_percent=bank_default_percent+random(3)+1;
    store_default_percent=store_default_percent+random(3)+1;
    shop_default_percent=shop_default_percent+random(3)+1;
    log_file("ZENTRALBANK",sprintf("%s: PERCENTS SET TO BANK %d STORE %d SHOP %d\n",TIME,bank_default_percent,store_default_percent,shop_default_percent),LS);
  }
  if (falling>0)
  {
    bank_default_percent/=2;
    store_default_percent/=2;
    shop_default_percent/=2;
    log_file("ZENTRALBANK",sprintf("%s: PERCENTS SET TO BANK %d STORE %d SHOP %d\n",TIME,bank_default_percent,store_default_percent,shop_default_percent),LS);
  }
  if (bank_default_percent<1) bank_default_percent=1;
  if (bank_default_percent>90) bank_default_percent=90;
  if (store_default_percent<1) store_default_percent=1;
  if (store_default_percent>90) store_default_percent=90;
  if (shop_default_percent<1) shop_default_percent=1;
  if (shop_default_percent>90) shop_default_percent=90;
  save_me();
}

void reset()
{
  adjust_percents();
}

void log_state()
{
  if (time()-last_save>300)
  {
    log_file("ZBANKSTATUS",sprintf("%s: %d\n",dtime(time()),allmoney),LS);
    save_me();
  }
}

varargs void PayIn(int amount, int percent)
{
  string type;

  if (amount<0) return;
  percent|=bank_default_percent;
  allmoney+=amount*percent/100;
  log_file("ZENTRALBANK",sprintf("%s: Einzahlung: %6d brutto von %O\n",TIME,
  amount,previous_object()),LS);
  
  /*
  type="unknown";
  if(previous_object()->id("NPC")) { type="NPC"; } else
  if(previous_object()->isPub()) { type="Pub"; } else
  if(previous_object()->isLaden()) { type="Laden"; }
  DBS(sprintf("** ZBank.EIN: %6d - %O (%s)\n", 
	amount, previous_object(), type));
  */
  log_state();
}

int WithDraw(int amount)
{
  int got;
  string type;

  if (!amount) return 0;
  if (allmoney<0) allmoney=0;
  if (!allmoney)
    got=0;
  else
  {
    if (amount*3<allmoney)
      got=amount;
    else
    {
      got=allmoney/3;
      if (!got) got=1;
    }
    allmoney-=got;
  }

  /*
  type="unknown";
  if(previous_object()->id("NPC")) { type="NPC"; } else
  if(previous_object()->isPub()) { type="Pub"; } else
  if(previous_object()->isLaden()) { type="Laden"; }
  DBS(sprintf("** ZBank.AUS: %6d - %O (%s)\n", 
	amount, previous_object(), type));
  */

  log_file("ZENTRALBANK",sprintf("%s: Abhebung: %6d/%6d von %O\n",TIME,got,
  amount,previous_object()),LS);
  log_state();
  return got;
}

int _query_current_money()
{
  return allmoney;
}

int _query_bank_default_percent()
{
  return bank_default_percent;
}

int _query_shop_percent_left()
{
  return shop_default_percent;
}

int _query_store_percent_left()
{
  return store_default_percent;
}

int set_percents(int store,int shop,int bank)
{
  store_default_percent=store;
  shop_default_percent=shop;
  bank_default_percent=bank; 
  log_file("ZENTRALBANK",sprintf("%s: PERCENTS SET TO BANK %d STORE %d SHOP %d\n",TIME,bank_default_percent,store_default_percent,shop_default_percent),LS);
return 1;
}

int remove()
{
  save_me();
  destruct(this_object());
  return 1;
}
