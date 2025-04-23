// Tamedhon MUDlib
//
// letzte Änderungen: 
// 2024-10-25 Aloy	Umbau auf UTF8 
// 2024-10-26 Gralkor	Fix der Warnings und "rü" zu "rüs"
//
#pragma strong_types

inherit "/std/room";

#include <properties.h>
#include <rooms.h>
#include <language.h>
#include <moving.h>
#include <routingd.h>
#include <bank.h>
#include <combat.h>
#include <new_skills.h>
#include <laden.h>
#include <ansi.h>

#define MAX_ANZ 3 /* Max 3 Objekte einer Sorte im Laden */
#define BUY_FACT 300 /* Objekte kosten BUY_FACT mal P_VALUE / 100 */

#define RESTR_CHECKER "/std/restriction_checker"

string storage;
mapping ob_anz;
int buyfact, mymoney, wantto, shop_percent_left, last;

// Jetzt Variablen fuer staendig verfuegbare Objekte:
string *fixed_obj; // Liste der staendig verfuegbaren Objekte
mapping fixed_value; // Preise bei Sonderangeboten
mapping fixed_ids;   // Ids

// prototypes

int list(string str);
int buy(string str);
int sell(string str, int f);
int force_sell(string str);
int evaluate(string str);
int show_obj(string str);

mixed *GetList();
varargs string KeeperPrefix(string verb, string alt);
varargs string CheckKeeper(int silent, string verb);
varargs int DoList(string query_fun, string str);
mixed FindInStore(mixed x);
varargs int do_sell(object ob, int force, int no_reg, string prefix);
int ask_sell(string str, object ob, int val, string prefix, int reqamount);
int do_sell2(object ob, int value, string prefix, int reqamount);
void give_money(int value, object ob, string prefix);
varargs int sell_all(object cnt, string prefix);

varargs void AddFixedObject(string str, int val, mixed ids)
{
  int i;

  // Achtung, bei solchen Objekten muss die Blueprvoid initialisiert werden!
  if (!str) return;
  if (!val) val = str->QueryProp (P_VALUE);
  if (!ids) ids = str->QueryProp (P_IDS);
  if (!pointerp (ids)) ids = ({});
  fixed_obj += ({ str });
  fixed_value[str] = val;
  for (i=sizeof(ids)-1; i>=0; i--)
    fixed_ids[ids[i]] = str;
}

void UpdateCounter(object ob,int num)
{
  string tmp;
  mixed a;

  if (!num||!objectp(ob)) return;
  tmp=object_name(ob);
  if (tmp[0..2]!="/d/"&&tmp[0..8]!="/players/")
    tmp=ob->short()+explode(tmp,"#")[0];
  else
    tmp=explode(tmp,"#")[0];
  a=ob_anz[tmp]+num;
  if (a<0) a=0;
  if (!a)
    efun::m_delete(ob_anz,tmp);
  else
    ob_anz[tmp]=a;
}

int InflateSellValue(int value, int cnt) {
  cnt-=3;
  if (cnt<0) cnt=0;
  return value-(value*cnt)/(cnt+25);
}

int TruncateSellValue(int new_value) {
  if (new_value>500)
    new_value-=500,new_value=((1000*new_value)/(new_value+1000))+500;
  return new_value;
}

int QueryValue(object ob, int value) {
  string tmp;
  int new_value;

  if (!objectp(ob)) return 0;
  if (Query("laden::compat"))
  {
    new_value=(value>1000?1000:value);
    if(new_value>mymoney)
      return (mymoney>0?mymoney:0);
    else
      return new_value;
  }
  tmp=object_name(ob);
  if (tmp[0..2]!="/d/"&&tmp[0..8]!="/players/")
    tmp=ob->short()+explode(tmp,"#")[0];
  else
    tmp=explode(tmp,"#")[0];
  new_value=TruncateSellValue(InflateSellValue(value,ob_anz[tmp]));
  if (3*new_value>mymoney)
  {
    log_file("LADEN",sprintf("Out of money: %O for %O (%s)\n",this_object(),
	                     this_player(),dtime(time())[5..]));
    new_value=mymoney/3;
  }
  if (new_value<0) new_value=0;
  return new_value;
}

int QuerySellValue(object ob)
{
  int value, wert, dam;
  value=ob->QueryProp(P_VALUE);
  if( dam = ob->QueryProp(P_DAMAGED) )
  {
    if( wert= ob->QueryProp(P_WC) )
      value = value * wert / (dam+wert);
    else if( wert= ob->QueryProp(P_AC) )
      value = value * wert / (dam+wert);
    else
      value = 1;
  }
  return QueryValue(ob,value);
}

int QueryBuyValue(mixed ob) {
  if (objectp(ob))
    return (ob->QueryProp(P_VALUE)*buyfact)/100;
  return (fixed_value[ob]*buyfact)/100;
}


void _set_shop_percent_left()
{
  shop_percent_left=ZENTRALBANK->_query_shop_percent_left();
}

string SetStorageRoom(string str)
{
  if (str && stringp(str)) {
    storage=str;
    catch(call_other(storage, "_register_shop", this_object()));
    return storage;
  }
 return storage;
}

string QueryStorageRoom() {
  return storage;
}

void create() {
  object router;

  ::create();
  _set_shop_percent_left();
  last=wantto=mymoney=50000;
  buyfact=BUY_FACT;
  SetProp( P_INT_LONG,
	  "Der Magier, der diesen Laden erschuf, fand keine Beschreibung dafür.\n");
  SetProp( P_INT_SHORT, "StdLaden\n");
  SetProp( P_LIGHT, 1);
  AddCmd("zeige","list");
  AddCmd(({"kauf","kaufe","bestell","bestelle"}),"buy");
  AddCmd(({"verkauf","verkaufe","verk"}),"sell");
  AddCmd(({"versetz","versetze"}),"force_sell");
  AddCmd(({"schaetz","schaetze","schätz","schätze"}),"evaluate");
  AddCmd(({"untersuche","unt"}), "show_obj");
  SetStorageRoom("/room/lagerraum");
  SetProp(P_INDOORS,1);
  SetProp(P_SHOPKEEPER_ID, "haendler");
  SetProp(P_SHOPKEEPER_NAME, "der Händler");
  SetProp(P_SHOPKEEPER_GENDER, MALE);
  AddItem("/p/npc/misc/entsorg",REFRESH_REMOVE);
  ob_anz=([]);
  fixed_obj=({});fixed_value=([]);fixed_ids=([]);

  if (!clonep(this_object()) && objectp(router=find_object(ROUTER)))
    router->RegisterTarget(TARGET_SHOP,object_name(this_object()));
}

#define LIST_ALIASES ([ "waf":"waffen", "waffe":"waffen", \
                        "rue":"ruestungen", "rüs":"rüstungen", "pan":"panzer", \
                        "hel":"helme", "helm":"helme", \
                        "amulett":"amulette", "hal":"amulette", \
                        "halskette":"amulette", "halsketten":"amulette", \
                        "amu":"amulette", "rin":"ringe", "ring":"ringe", \
                        "han":"handschuhe", "umh":"umhang", \
                        "umhaenge":"umhang", "maentel":"umhang", \
                        "umhänge":"umhang", "mäntel":"umhang", \
                        "man":"umhang", "mantel":"umhang", \
                        "sti":"stiefel", "schuhe":"stiefel", \
                        "sch":"schild", "schilde":"schild", \
                        "hos":"hose", "hosen":"hose", "gue":"guertel", "gü":"gürtel", \
                        "arm":"armschienen", "armschiene":"armschienen", \
                        "beh":"behaelter", "behä":"behälter", "ver":"verschiedenes", \
                        "son":"verschiedenes", "sonstiges":"verschiedenes", \
                        "1haender":"einhaender", "1ha":"einhaender", \
                        "1händer":"einhänder", "1hä":"einhänder", \
                        "einhan":"einhaender", "zweihan":"zweihaender", \
                        "einhand":"einhänder", "zweihand":"zweihänder", \
                        "2haender":"zweihaender", "2ha":"zweihaender", \
                        "2händer":"zweihänder", "2hand":"zweihänder", \
                        "bihaender":"zweihaender", "bih":"zweihaender", \
                        "bihänder":"zweihänder", "bihand":"zweihänder"])

int list(string str)
{
  string fun, wem, was;
  _notify_fail(
    "Bitte 'zeige', 'zeige waffen', 'zeige rüstungen' oder\n"+
    "'zeige verschiedenes' eingeben.\n");
  // Kein Filter? Alles zurueckgeben.
  if (!str || !stringp(str)) return DoList("AlwaysTrue");
  // "zeige wem was" abfangen
  if (sscanf(str,"%s %s",wem,was)==2 &&
      living(present(wem,environment(this_player()))))
    return this_player()->show_things(str);
  // Gueltiger Filter?
  fun = "Is_" + (LIST_ALIASES[str] || str);
  if (function_exists(fun, this_object()))
    return DoList(fun);
  // ID-Filter? . wenn 0, dann "zeige" aus Soul am Zug
  return DoList("Is_Id",str);
}

int AlwaysTrue(object ob){
  return 1;
}

int Is_waffen(object ob) {
  return ob->QueryProp(P_WEAPON_TYPE);
}
int Is_ruestungen(object ob) {
  return ob->QueryProp(P_ARMOUR_TYPE);
}
int Is_rüstungen(object ob) {
  return ob->QueryProp(P_ARMOUR_TYPE);
}
int Is_panzer(object ob) {
  return (ob->QueryProp(P_ARMOUR_TYPE)==AT_ARMOUR);
}
int Is_helme(object ob) {
  return (ob->QueryProp(P_ARMOUR_TYPE)==AT_HELMET);
}
int Is_amulette(object ob) {
  return (ob->QueryProp(P_ARMOUR_TYPE)==AT_AMULET);
}
int Is_ringe(object ob) {
  return (ob->QueryProp(P_ARMOUR_TYPE)==AT_RING);
}
int Is_handschuhe(object ob) {
  return (ob->QueryProp(P_ARMOUR_TYPE)==AT_GLOVE);
}
int Is_umhang(object ob) {
  return (ob->QueryProp(P_ARMOUR_TYPE)==AT_CLOAK);
}
int Is_stiefel(object ob) {
  return (ob->QueryProp(P_ARMOUR_TYPE)==AT_BOOT);
}
int Is_schild(object ob) {
  return (ob->QueryProp(P_ARMOUR_TYPE)==AT_SHIELD);
}
int Is_hose(object ob) {
  return (ob->QueryProp(P_ARMOUR_TYPE)==AT_TROUSERS);
}
int Is_armschienen(object ob) {
  return (ob->QueryProp(P_ARMOUR_TYPE)==AT_BRACERS);
}
int Is_guertel(object ob) {
  return (ob->QueryProp(P_ARMOUR_TYPE)==AT_BELT);
}
int Is_gürtel(object ob) {
  return (ob->QueryProp(P_ARMOUR_TYPE)==AT_BELT);
}
int Is_einhaender(object ob) {
  return (ob->QueryProp(P_WEAPON_TYPE) && ob->QueryProp(P_NR_HANDS)==1);
}
int Is_einhänder(object ob) {
  return (ob->QueryProp(P_WEAPON_TYPE) && ob->QueryProp(P_NR_HANDS)==1);
}
int Is_zweihaender(object ob) {
  return (ob->QueryProp(P_WEAPON_TYPE) && ob->QueryProp(P_NR_HANDS)==2);
}
int Is_zweihänder(object ob) {
  return (ob->QueryProp(P_WEAPON_TYPE) && ob->QueryProp(P_NR_HANDS)==2);
}
int Is_verschiedenes(object ob) {
  return (!ob->QueryProp(P_WEAPON_TYPE) && !ob->QueryProp(P_ARMOUR_TYPE));
}
int Is_behaelter(object ob) {
  return ob->id("Container");
}
int Is_behälter(object ob) {
  return ob->id("Container");
}
int Is_Id(object ob,string str) {
  return ob->id(str);
}

mixed *GetList()
{
  object store;
  int i, padd, noascii;
  mixed *output,tmp,str;
  string comp,sho;
  mapping tmp2;

  output=({});
  call_other(storage, "_register_shop", this_object());
  if (!objectp(store=find_object(storage))) return output;
  tmp=fixed_obj+all_inventory(store); tmp2=([]);

  if (this_player()->QueryProp(P_NO_ASCII_ART)) noascii=20; //Zusatz f. Breite

  for (i=sizeof(tmp)-1;i>=0;i--)
  {
    sho=tmp[i]->short()[0..<3];
    padd=noascii+25+sizeof(sho)-colsizeof(sho);
    if (noascii || (padd>(noascii+25) && colsizeof(sho)>(noascii+25))) {
      // wenn Farbe UND Ueberlaenge --> Farbe weg, sonst unsauberes sprintf
      // oder wenn P_NO_ASCII_ART
      padd=noascii+25;
      sho=regreplace(""+(sho), "\e[[]([0-9]|[0-9][0-9])m", "", 1);
    }
    if (noascii)
      str=({({sprintf("%' '-:*s%' ':7d", padd, sho, QueryBuyValue(tmp[i])),
              tmp[i]})});
    else
      str=({({sprintf("%'.'-:*s%'.':7d", padd, sho, QueryBuyValue(tmp[i])),
              tmp[i]})});
/*
    str=({({sprintf("%'.'-25.25s%'.'7.7d",
               tmp[i]->short()[0..<3], QueryBuyValue(tmp[i])), tmp[i]})});
*/
    comp=str[0][0][0..(noascii+25)];
    if (!tmp2[comp]) {
      tmp2[comp]=1;
      output+=str;
    }
  }
  return output;
}


varargs string KeeperPrefix(string verb, string alt)
{
  object keeper;
  keeper=present(QueryProp(P_SHOPKEEPER_ID));
  return QueryProp(P_SHOP_FLAGS) & (SHOP_HASKEEPER | SHOP_NEEDSKEEPER) ?
    capitalize(keeper?keeper->name(WER,1):
        QueryProp(P_SHOPKEEPER_NAME) || "der Händler")+
    " "+(verb || "sagt: ") : alt || "";
}

//
// CheckKeeper()
// Prueft, ob sich der Haendler im Raum befindet, und gibt bei
// Notwendigkeit dessen Anwesenheit eine entsprechende Meldung aus.
// Rueckgabewert: der 'sag' Prefix-String ("Der Haendler sagt: ",
// oder bei Shops ohne Haendler: "")
//
varargs string CheckKeeper(int silent, string verb)
{
  object keeper;
  if ((QueryProp(P_SHOP_FLAGS) & SHOP_NEEDSKEEPER) )
  {
    if( !(keeper=present(QueryProp(P_SHOPKEEPER_ID))))
    {
      if (!silent)
        write((capitalize(QueryProp(P_SHOPKEEPER_NAME) || "der Händler"))+
          " ist leider nicht da!\n");
      else if (silent==2)
        _notify_fail((capitalize(QueryProp(P_SHOPKEEPER_NAME) ||
           "der Händler"))+ " ist leider nicht da!\n");
      return 0;
    }
    if( keeper->InFight() )
    {
      if (!silent)
        write((capitalize(QueryProp(P_SHOPKEEPER_NAME) || "der Händler"))+
          " kämpft gerade und kann sich leider nicht um Dich kümmern!\n");
      else if (silent==2)
        _notify_fail((capitalize(QueryProp(P_SHOPKEEPER_NAME)||"der Händler"))+
         " kämpft gerade und kann sich leider nicht um Dich kümmern!!\n");
      return 0;
    }
  }
  return KeeperPrefix(verb);
}

varargs int DoList(string query_fun, string str)
{
  object ob,keeper;
  int i,j,n,indent,noascii;
  string *output;
  string prefix,out;
  string cnorm, cbold, pipe, dot;

  if (this_player()->QueryProp(P_NO_ASCII_ART)) noascii=1;

  if (!noascii && this_player()->QueryProp(P_TTY)=="ansi")
  { cnorm=ANSI_NORMAL; cbold=ANSI_BOLD; }
  else { cnorm=""; cbold=""; }

  if (this_player()->InFight()) {
    write("Dazu bist Du im Moment viel zu beschäftigt.\n");
    return 1;
  }
  if (!prefix=CheckKeeper()) return 1;
  output=GetList();
  indent=0;
  if (!sizeof(output)) {
    write(prefix+"Im Moment sind wir leider VÖLLIG ausverkauft!\n");
    return 1;
  }
  indent=0;
  out="";
  for (i=0;i<sizeof(output);i++) {
    if (call_other(this_object(),query_fun,output[i][1],str)) {
      n++;
      out+=sprintf(cbold+"%3d."+cnorm+" %s"+cnorm, i+1,output[i][0]);
        //zweites cnorm, falls ein objekt kaputte farben in short hat
      if (noascii || !(indent=!indent)) out+="\n";
      else out+=cbold+" | "+cnorm;
    }
  }
  if (!n)
  {
    if(query_fun=="Is_Id")
    {
      _notify_fail(prefix+"Davon haben wir leider nichts!\n");
      return 0;
    }
    else
    {
      write(prefix+"Davon haben wir leider nichts!\n");
      return 1;
    }
  }
  if (indent) out+="\n";
  this_player()->More(out);
  out=0;
  return 1;
}

int show_obj(string str)
{
  int i,j;
  string was, dummy;
  mixed ob;
  string prefix;
  mapping restr;
  mapping rmap;
  string res;


  if (!str) return 0;
  // if (sscanf(str,"%s im laden",was)>0 || sscanf(str,"%s in laden",was)>0)
  // (der Parser macht bereits 'in' aus 'im')
  prefix=CheckKeeper(1);
  if (sscanf(str,"%s in laden",was)>0)
  {
    if (this_player()->InFight())
    {
      write("Dazu bist Du im Moment viel zu beschäftigt.\n");
      return 1;
    }
    if (!prefix) return CheckKeeper(), 1;
    if (!ob=FindInStore(was)) {
      write(prefix+"Das kann ich im Lager nicht finden.\n");
      return 1;
    }
  }
  else if (sscanf(str,"%d",i)>0 && (i>0) && (to_string(i)==str))
  {
    if (this_player()->InFight())
    {
      write("Dazu bist Du im Moment viel zu beschäftigt.\n");
      return 1;
    }
    if (!prefix) return CheckKeeper(), 1;
    if (!ob=FindInStore(i))
      return _notify_fail(prefix+"Das kann ich im Lager nicht finden.\n"), 0;
  }
  else return 0;
  i=QueryBuyValue(ob);

  write((QueryProp(P_SHOP_FLAGS) & (SHOP_HASKEEPER | SHOP_NEEDSKEEPER)
        ? KeeperPrefix("zeigt Dir "+ob->name(WEN)) : ob->Name()) +
      ":\n"+ob->long());
  write(capitalize(ob->QueryPronoun())+" würde"+
      (ob->IsUnit() && (ob->QueryProp(P_REQUESTED_AMOUNT) != 1) ? "n" : "")+
      " Dich "+i+" Tametaler kosten.\n");

  /* (zu spielerfreundlich :), ausserdem kein schoener Text...
  // z.B. "Der Haendler warnt Dich: Als Zwerg kannst Du das nicht."
  //
  if (ob->QueryProp(P_ARMOUR_TYPE)&&(restr=ob->QueryProp(P_WEAR_RESTRICTIONS))
      && (was=RESTR_CHECKER->check_restrictions(this_player(), restr)))
    write(KeeperPrefix("warnt Dich: ", "Der Haendler warnt Dich: ")+was);
  */
  if (ob->QueryProp(P_ARMOUR_TYPE)&&(restr=ob->QueryProp(P_WEAR_RESTRICTIONS))
      && RESTR_CHECKER->check_restrictions(this_player(), restr))
    write(break_string(
        "Du koenntest "+ob->name(WEN,1)+" allerdings nicht anziehen!", 78,
        KeeperPrefix("warnt Dich: ", "Der Händler warnt Dich: ")));

  {
   j=10+random(2)-random(2);
   if(((this_player()->QueryAttribute(A_DEX)+5)*j) < ob->QueryProp(P_WC))
   {
    if ((this_player()->QueryProp(P_LEVEL)<15) || (random(100)>33))
    {
      write(break_string("Der Händler warnt Dich: Du bist vermutlich nicht "
        "geschickt genug, um mit dieser Waffe umzugehen.\n", 78));
    }
   }

   if( (rmap=ob->QueryProp(P_WIELD_RESTRICTIONS)) &&
      (res=RESTR_CHECKER->check_restrictions(this_player(), rmap)) )
   {
    if ((this_player()->QueryProp(P_LEVEL)<15) || (random(100)>50))
    {
      write(break_string("Der Händler warnt Dich: "+res, 78));
    }
   }

  }

  return 1;
}

int buy(string str)
{
  int i,val;
  object ob, store;
  string prefix, dummy;

  if (this_player()->InFight()) {
    write("Dazu bist Du im Moment viel zu beschäftigt.\n");
    return 1;
  }
  if (!prefix=CheckKeeper(2)) return 0;
  if (!str) {
    write(KeeperPrefix("fragt Dich: ")+"Was willst Du kaufen?\n");
    return 1;
  }
  _notify_fail(prefix+"Das kann ich im Lager nicht finden.\n");
  if (sscanf(str,"%d%s",i,dummy)>0 && i>0) {
    if ((dummy==" ab") || (dummy==""))
      ob=FindInStore(i);
    else
      ob=FindInStore(str);
  } else {
    ob=FindInStore(str);
  }
  if (!ob) return 0;
  val=QueryBuyValue(ob);

  if ((i=this_player()->QueryMoney()) < val) {
    write(break_string(ob->Name(WER,1)+" würde "+val+" Tametaler kosten, "+
        (i ? "Du hast aber nur "+i+".\n"
           : "Du bist aber völlig pleite!\n"),
        78, prefix));
    return 1;
  }
  this_player()->AddMoney(-val);
  mymoney+=val;
  if (stringp(ob)) ob=clone_object((string) ob); // Staendig verfuegbares Objektm
  if (ob->move(this_player(),M_GET)<=0){
    write(prefix+"Du kannst das nicht mehr tragen!\n");
    ob->move(this_object());            // Objekt in den Raum moven
  } else {
    write("Du kaufst "+ ob->name(WEN,1) + ".\n");
  }
  say(this_player()->Name()+" kauft "+ob->name(WEN)+".\n");
  UpdateCounter(ob,-1);
  return 1;
}

mixed FindInStore(mixed x)
{
  mixed list;
  object store;
  string res;

  if (intp(x)) {
    list = GetList();
    if (x<1 || x>sizeof(list)) return 0;
    return list[x-1][1];
  }
  if (stringp(x)) {
    if (res=fixed_ids[x]) return res; // Staendig verfuegbares Objekt
    if (!store=find_object(storage)) return 0;
    return present(x,store);
  }
  return 0;
}

int evaluate(string str)
{
  object ob;
  int val,rval;
  string prefix;

  if (!str) return 0;
  if (this_player()->InFight()) {
    write("Dazu bist Du im Moment viel zu beschäftigt.\n");
    return 1;
  }
  if (!prefix=CheckKeeper(2)) return 0;
  ob=present(str,this_object());
  if (!ob) ob=present(str,this_player());
  if (!ob)
  {
    write(prefix+"Hm? "+capitalize(str)+"? Wovon redest Du?\n");
    return 1;
  }
  _notify_fail(prefix+"Nanu, seit wann werden hier Lebewesen verkauft?\n");
  if (living(ob))
    return 0;

  rval=ob->QueryProp(P_VALUE);
  if (rval) {
    val=QuerySellValue(ob);
    if (rval==val)
      write(break_string("Naja, ich denke, " +val+ " Tametaler wäre"+
                (ob->IsUnit() &&
                        (ob->QueryProp(P_REQUESTED_AMOUNT)>1) ? "n ":" ")+
                (ob->QueryPronoun(WER))+" schon wert.\n", 78, prefix));
    else
      if (val)
        write(break_string("Oh, nach der aktuellen Marktlage kann ich nur "+
            val+" Tametaler bezahlen, obwohl "+ob->QueryPronoun(WER)+
            " eigentlich "+rval+" Tametaler wert ist.", 78, prefix));
      else
        write(prefix+"Ich bin vollkommen pleite. Tut mir leid.\n");
  }
  else
    write(prefix+"Das ist völlig wertlos.\n");

  return 1;
}

int force_sell(string str)
{
  return sell(str,1);
}

int sell(string str,int f)
{
  object ob, cnt;
  string ob_str, cnt_str, prefix;

  if (!str) return 0;
  if (this_player()->InFight()) {
    write("Dazu bist Du im Moment viel zu beschäftigt.\n");
    return 1;
  }
  if (!prefix=CheckKeeper(2)) return 0;
  if (str=="alles")
  {
    sell_all(0,prefix);
    return 1;
  }
  if (sscanf(str,"alles aus %s", cnt_str))
  {
    if (cnt=present(cnt_str, this_player()))
    {
      sell_all(cnt,prefix);
      return 1;
    }
    write(prefix+capitalize(cnt_str)+"? Sehe ich hier nicht, tut mir leid!\n");
    return 1;
  }
  if (sscanf(str,"%s aus %s", ob_str, cnt_str))
  {
    if (cnt=present(cnt_str, this_player()))
    {
      if (ob=present(ob_str, cnt))
      {
        this_object()->do_sell(ob,f,0,prefix);
        return 1;
      }
      write(prefix+capitalize(ob_str)+"? Sehe ich hier nicht, tut mir leid!\n");
      return 1;
    }
    write(prefix+capitalize(cnt_str)+"? Sehe ich hier nicht, tut mir leid!\n");
    return 1;
  }
  ob=present(str,this_object());
  if (!ob) ob=present(str,this_player());
  if (!ob) {
    write(prefix+capitalize(str)+"? Sehe ich hier nicht, tut mir leid!\n");
    return 1;
  }
  this_object()->do_sell (ob,f,0,prefix);
  return 1;
}

varargs int do_sell(object ob, int force, int no_reg, string prefix)
{
  int value, rvalue;
  string str;
  mapping special;
  mixed key;
  object inv;

  if (!no_reg)
    call_other(storage, "_register_shop", this_object());
  if (explode(object_name(ob),"#")[0]=="/obj/money")
  {
    write(prefix+"Das wäre ja wohl Unsinn, oder ...?\n");
    return 1;
  }
  if (str=ob->QueryProp(P_NOSELL))
  {
    if (stringp(str))
      write(str);
    else
      write(prefix+"Du kannst "+ob->name(WEN)+" nicht verkaufen!\n");
    return 1;
  }
  if ((special=QueryProp(P_SPECIAL_SHOP)) && mappingp(special))
  {
    key=m_indices(special)[0];
    if ((key==1) || !ob->QueryProp(key))
    {
      if (stringp(special[key]))
        write(special[key]);
      else
        write(prefix+
            "Tut mir leid, aber wir kaufen hier k"+ob->name(WEN)+".\n");
      return 1;
    }
  }
  if (ob->QueryProp(P_VALUE)<=0)
  {
    write(prefix+capitalize(ob->name(WER))+
        " hat keinen materiellen Wert, tut mir leid.\n");
    return 1;
  }
  rvalue=ob->QueryProp(P_VALUE);
  value=QuerySellValue(ob);
  if (!value)
  {
    write(prefix+"Ich bin absolut pleite. Tut mir aufrichtig leid.\n");
    return 1;
  }
  if(ob->QueryProp(P_KEEP_ON_SELL)==getuid(this_player()))
  {
    write(break_string(prefix+"Du wolltest "+ob->name(WEN)+" doch behalten. "
      "Willst Du "+ob->QueryPronoun(WEN)+" mir trotzdem für " +value+
      " Tametaler überlassen? (ja/nein)\n",78)+" -> ");
    input_to("ask_sell",0,ob,value,prefix,ob->QueryProp(P_REQUESTED_AMOUNT));
    return 1;
  }
  if (value==rvalue || force)
  {
    return this_object()->do_sell2(ob,value,prefix,ob->QueryProp(P_REQUESTED_AMOUNT));
  }
  write(break_string(
    "Nach der aktuellen Marktlage kann ich Dir dafür nur "+value+
    " Tametaler bieten, obwohl "+ob->name()+
    " eigentlich "+rvalue+" Tametaler wert wäre"+
    ((ob->IsUnit() && ob->QueryProp(P_REQUESTED_AMOUNT)!=1)? "n" : "")+".",
    78, prefix));
  write(
    "Willst Du "+ob->QueryPronoun(WEN)+" mir dafuer überlassen? "+
    "(ja/nein) -> ");
  input_to("ask_sell",0,ob,value,prefix,ob->QueryProp(P_REQUESTED_AMOUNT));
 
 return 1;
}

int ask_sell(string str, object ob, int val, string prefix, int reqamount)
{
  str=lower_case(str||"");
  if (str=="ja"||str=="j")
    this_object()->do_sell2(ob,val,prefix,reqamount);
  else
    write(prefix+"Ok, dann behalts!\n");
  return 1;
}

int do_sell2(object ob, int value, string prefix, int reqamount)
{
  int ret,nrh;
  string str,at;
  closure qp;

  // if (value>MAX_VALUE) value=MAX_VALUE;
  if (value>mymoney)
  {
    write(prefix+"Ich hab das Geld leider nicht mehr.\n");
    return 1;
  }
  if (!ob || !deep_present(ob,this_player()) )
  {
    if(!present(ob, this_object()))
    {

    write(break_string("Nanu, wo ist denn "+
        (ob ? ob->name(WER,1) : "der fragliche Gegenstand")+
        " hinverschwunden?", 78, prefix));
    return 1;
    }
  }
  if (sizeof(all_inventory(ob)))
  {
    write(break_string("Aber "+ob->name(WER,1)+
        " ist doch gar nicht leer!", 78, prefix));
    return 1;
  }
  qp=symbol_function("QueryProp", ob);
  if (ob->IsUnit())
    if (funcall(qp,P_AMOUNT) < reqamount)
    {
      write(prefix+"Tut mir leid, aber irgendwas ging da schief.\n");
      return 1;
    }
    else
      ob->SetProp(P_REQUESTED_AMOUNT, reqamount);
  if ( funcall(qp,P_NOBUY)
    || ( ( nrh = funcall(qp,P_NR_HANDS) )
        && funcall(qp,P_WC)+funcall(qp,P_DAMAGED) > nrh*50+50 )
    || ( ( at = funcall(qp,P_ARMOUR_TYPE) )
        && funcall(qp,P_AC)+funcall(qp,P_DAMAGED)>=KEEP_ARMOUR_CLASS[at] ) )
  {
    string was;
    was = ob->name(WEM, 1);
    ret=ob->move(storage,M_PUT);
    if (ret>0||ob->QueryProp(P_MAYSELL))  /* Sonst werden auch Sachen */
                /* zerstoert, die man nicht weglegen kann */
    {
      say(this_player()->Name()+" verkauft "+ob->name(WEN)+".\n");
      write(
        break_string(sprintf("%sfindet Gefallen an %s und legt %s zu seinen "
          "Privatsachen.", capitalize(KeeperPrefix("", "Der Händler ")),
          was, ob->QueryPronoun(WEN)), 78));
      give_money(value,ob,prefix);
      mymoney-=value;
      mymoney+=value*shop_percent_left/100; // Wegen Zerstoerung des Objektes
      UpdateCounter(ob,1);
      if (ob->IsUnit()) {
        object units;
        units = ob->QueryProp(P_GIVEN_OBJECT);
        destruct(units);
      }
      else
        destruct(ob);
    }
    else if (ret == ME_CANT_BE_DROPPED)
    {
      if ((str=funcall(qp,P_NODROP)) && stringp(str))
      {
        write(str);
        return 1;
      }
      write("Du kannst "+ob->name(WEN,1)+" nicht verkaufen!\n");
      return 1;
    }
    else
      write(prefix+"Das interessiert mich nicht.\n");
  }
  else
    if (ob->move(storage,M_PUT)>0)
    {
      say(this_player()->Name()+" verkauft "+ob->name(WEN)+".\n");
      give_money(value,ob,prefix);
      mymoney-=value;
      UpdateCounter(ob,1);
    }
    else
      write(prefix+"Du kannst "+ob->name(WEN,1)+" nicht verkaufen!\n");
  return 1;
}

void give_money(int value, object ob, string prefix)
{
  write(KeeperPrefix("", "Der Händler ")+"zahlt Dir "+value+
      " Tametaler.\n");
  if (this_player()->AddMoney(value) <= 0)
  {
    object mon;

    write("Du kannst das Geld nicht mehr tragen!\n");
    mon=clone_object("/obj/money");
    mon->SetProp(P_AMOUNT,value);
    mon->move(this_object(),M_MOVE_ALL|M_NOCHECK);
  }
}

varargs int sell_all(object cnt, string prefix)
{
  object next, ob;
  int flag,c,keepthings;

  flag = c = keepthings = 0;
  call_other (storage,"_register_shop",this_object()); // force load
  if (cnt)
    ob = first_inventory(cnt);
  else
    ob = first_inventory(this_player());
  while (ob && c++<50)
  {
    next = next_inventory(ob);
    if (ob->name(WER) && ob->short() && ob->QueryProp(P_VALUE))
    {
      flag = 1;
//      write(capitalize(ob->name(WER)||"")+": ");
      if (ob->QueryProp(P_KEEP_ON_SELL)==getuid(this_player())
          || ob->QueryProp(P_WORN)
          || ob->QueryProp(P_WIELDED))
      {
        keepthings++;
//	write("Du behaeltst "+ob->name(WEN)+".\n");
      }
      else {
        write(capitalize(ob->name(WER)||"")+": ");
        write("\n");
	this_object()->do_sell(ob,1,1,"");
      }
    }
    ob = next;
  }
  if( keepthings )
    write("Einige Deiner Sachen hast Du wie gewünscht behalten...\n");

  if( !flag )
    write(prefix+"Hmmm, viel hast Du aber nicht zu bieten...\n" );

  return 1;
}

void SetBuyFact(int i)
{
  buyfact=i*100;
}

int QueryBuyFact()
{
  return buyfact/100;
}

void reset()
{
  mixed *keys;
  int i;

  ::reset();

  if (!mappingp(ob_anz))
    return;
  keys=m_indices(ob_anz);
  _set_shop_percent_left();
  for (i=sizeof(keys)-1;i>=0;i--)
  {
    ob_anz[keys[i]]=ob_anz[keys[i]]*7/8;
    if (!ob_anz[keys[i]])
      efun::m_delete(ob_anz,keys[i]);
  }
  if (last/3>mymoney)
    wantto=wantto*110/100;
  else
    if (last/2<mymoney)
      wantto=wantto*85/100;
  if (wantto<25000) wantto=25500;
  if (wantto>1500000) wantto=1500000;
  if (wantto<mymoney)
  {
    ZENTRALBANK->PayIn(mymoney-wantto);
    mymoney=wantto;
  } else
    mymoney+=ZENTRALBANK->WithDraw(wantto-mymoney);
  last=mymoney;
}

void _add_money(int i)
{
  log_file("ZENTRALBANK",sprintf("TRANSFER: %O got %d from %O\n",this_object(),
				 i,previous_object()));
  mymoney+=i;
}

int _query_current_money()
{
  return mymoney;
}

int _query_wantto()
{
  return wantto;
}
